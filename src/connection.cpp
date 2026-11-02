#include "connection.hpp"
#include "byteBuffer.hpp"
#include "chunk.hpp"
#include "entities.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "playerSession.hpp"
#include "registries.hpp"
#include "transfer.hpp"
#include "uuid.hpp"
#include "viewport.hpp"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <exception>
#include <functional>
#include <poll.h>
#include <string>
#include <vector>

namespace flatland {

namespace {

constexpr int32_t protocolVersion = 777;
constexpr const char *gameVersion = "26.3";

constexpr int32_t playerEntityId = 1;
constexpr int32_t cameraAnchorEntityId = 2;
constexpr int32_t firstSpriteEntityId = 100;

struct RoomBounds {
  int32_t minX, maxX, minY, maxY, minZ, maxZ;
};

RoomBounds computePlayerCellBounds() {
  int32_t spawnX = static_cast<int32_t>(std::floor(viewportOriginX));
  int32_t spawnY = static_cast<int32_t>(std::floor(playerFeetY));
  int32_t spawnZ = static_cast<int32_t>(std::floor(cameraZ));

  RoomBounds bounds;
  bounds.minX = spawnX - 1;
  bounds.maxX = spawnX + 1;
  bounds.minY = spawnY - 1;
  bounds.maxY = spawnY + 2;
  bounds.minZ = spawnZ - 1;
  bounds.maxZ = spawnZ + 1;
  return bounds;
}

int32_t boxRoomBlockAt(const RoomBounds &room, int32_t x, int32_t y, int32_t z) {
  if (x < room.minX || x > room.maxX || y < room.minY || y > room.maxY || z < room.minZ || z > room.maxZ) return airBlockId;
  bool onShell = (x == room.minX || x == room.maxX || y == room.minY || y == room.maxY || z == room.minZ || z == room.maxZ);
  return onShell ? barrierBlockId : airBlockId;
}

int64_t packPosition(int32_t x, int32_t y, int32_t z) {
  return ((static_cast<int64_t>(x) & 0x3FFFFFF) << 38) | ((static_cast<int64_t>(z) & 0x3FFFFFF) << 12) | (static_cast<int64_t>(y) & 0xFFF);
}

ByteBuffer buildPostEffectsPacket(const std::vector<std::string> &postEffectIds) {
  ByteBuffer buffer;
  buffer.writeVarInt(static_cast<int32_t>(postEffectIds.size()));
  for (const std::string &id : postEffectIds) buffer.writeString(id);
  return buffer;
}

std::string escapeJsonString(const std::string &value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (char c : value) {
    if (c == '"' || c == '\\') escaped += '\\';
    escaped += c;
  }
  return escaped;
}

void sendTransfer(int socketFd, const std::string &host, uint16_t port) {
  writePacket(socketFd, packet::configuration::transferClientbound, buildTransferPacket(host, port));
}

void handleStatus(int socketFd, const ServerOptions &options) {
  readPacket(socketFd);

  std::string json = "{\"version\":{\"name\":\"" + std::string(gameVersion) + "\",\"protocol\":" + std::to_string(protocolVersion) +
                     "},\"players\":{\"max\":" + std::to_string(options.maxPlayers) + ",\"online\":0,\"sample\":[]},\"description\":{\"text\":\"" +
                     escapeJsonString(options.motd) + "\"}}";

  ByteBuffer response;
  response.writeString(json);
  writePacket(socketFd, packet::status::statusResponseClientbound, response);

  RawPacket ping = readPacket(socketFd);
  ByteReader pingReader(ping.payload.data(), ping.payload.size());
  int64_t pingPayload = pingReader.readLong();

  ByteBuffer pong;
  pong.writeLong(pingPayload);
  writePacket(socketFd, packet::status::pongResponseClientbound, pong);
}

struct LoginResult {
  std::string username;
  Uuid uuid;
};

LoginResult handleLogin(int socketFd) {
  RawPacket loginStart = readPacket(socketFd);
  ByteReader reader(loginStart.payload.data(), loginStart.payload.size());
  std::string username = reader.readString();

  LoginResult result;
  result.username = username;
  result.uuid = offlinePlayerUuid(username);
  Uuid sessionId = randomUuid();

  ByteBuffer success;
  success.writeUuid(result.uuid.high, result.uuid.low);
  success.writeString(username);
  success.writeVarInt(0);
  success.writeUuid(sessionId.high, sessionId.low);
  writePacket(socketFd, packet::login::loginFinishedClientbound, success);

  readPacket(socketFd);

  return result;
}

void handleConfiguration(int socketFd, const std::string &clientReportedAddress, const ResourcePackInfo &resourcePack) {
  readPacket(socketFd);

  ByteBuffer knownPacks;
  knownPacks.writeVarInt(1);
  knownPacks.writeString("minecraft");
  knownPacks.writeString("core");
  knownPacks.writeString(gameVersion);
  writePacket(socketFd, packet::configuration::selectKnownPacksClientbound, knownPacks);

  readPacket(socketFd);

  sendMinimalRegistries(socketFd);

  std::string packUrl = "http://" + clientReportedAddress + ":" + std::to_string(resourcePack.httpPort) + "/pack.zip";

  ByteBuffer resourcePackPush;
  Uuid packId = randomUuid();
  resourcePackPush.writeUuid(packId.high, packId.low);
  resourcePackPush.writeString(packUrl);
  resourcePackPush.writeString(resourcePack.sha1Hex);
  resourcePackPush.writeBool(true);  // force
  resourcePackPush.writeBool(false); // no prompt
  writePacket(socketFd, packet::configuration::resourcePackPushClientbound, resourcePackPush);

  readPacket(socketFd); // resource_pack response

  writePacket(socketFd, packet::configuration::finishConfigurationClientbound, ByteBuffer{});

  readPacket(socketFd); // acknowledge_finish_configuration
}

void sendJoinSequence(int socketFd, const ServerOptions &options, const RoomBounds &room, int32_t chunkRadiusX, int32_t chunkRadiusZ) {
  ByteBuffer loginPlay;
  loginPlay.writeInt(playerEntityId);
  loginPlay.writeBool(false); // hardcore
  loginPlay.writeVarInt(3);
  loginPlay.writeString("minecraft:overworld");
  loginPlay.writeString("minecraft:the_nether");
  loginPlay.writeString("minecraft:the_end");
  loginPlay.writeVarInt(options.maxPlayers);
  loginPlay.writeVarInt(chunkRadiusX); // view distance
  loginPlay.writeVarInt(chunkRadiusX); // simulation distance
  loginPlay.writeBool(false);          // reduced debug info
  loginPlay.writeBool(false);          // respawn screen
  loginPlay.writeBool(false);          // limited crafting
  loginPlay.writeVarInt(0);            // dimension type
  loginPlay.writeString("minecraft:overworld");
  loginPlay.writeLong(0);     // hashed seed
  loginPlay.writeByte(2);     // game mode: adventure
  loginPlay.writeByte(0);     // previous game mode
  loginPlay.writeBool(false); // debug
  loginPlay.writeBool(true);  // flat
  loginPlay.writeBool(false); // has death location
  loginPlay.writeVarInt(0);   // portal cooldown
  loginPlay.writeVarInt(0);   // sea level
  loginPlay.writeBool(false); // online mode
  loginPlay.writeBool(false); // secure chat
  writePacket(socketFd, packet::play::loginClientbound, loginPlay);

  ByteBuffer abilities;
  abilities.writeByte(0x01);   // invulnerable
  abilities.writeFloat(0.05f); // flying speed
  abilities.writeFloat(0.1f);  // field of view modifier
  writePacket(socketFd, packet::play::playerAbilitiesClientbound, abilities);

  ByteBuffer teleport;
  teleport.writeVarInt(1); // teleport id
  teleport.writeDouble(viewportOriginX);
  teleport.writeDouble(playerFeetY);
  teleport.writeDouble(cameraZ);
  teleport.writeDouble(0);
  teleport.writeDouble(0);
  teleport.writeDouble(0); // velocity
  teleport.writeFloat(0);
  teleport.writeFloat(0); // yaw, pitch
  teleport.writeInt(0);   // flags: all fields absolute
  writePacket(socketFd, packet::play::playerPositionClientbound, teleport);

  ByteBuffer spawnPosition;
  spawnPosition.writeString("minecraft:overworld");
  spawnPosition.writeLong(packPosition(static_cast<int32_t>(viewportOriginX), static_cast<int32_t>(playerFeetY), static_cast<int32_t>(cameraZ)));
  spawnPosition.writeFloat(0);
  spawnPosition.writeFloat(0);
  writePacket(socketFd, packet::play::setDefaultSpawnPositionClientbound, spawnPosition);

  ByteBuffer gameEvent;
  gameEvent.writeByte(13);
  gameEvent.writeFloat(0);
  writePacket(socketFd, packet::play::gameEventClientbound, gameEvent);

  ByteBuffer centerChunk;
  centerChunk.writeVarInt(0);
  centerChunk.writeVarInt(0);
  writePacket(socketFd, packet::play::setChunkCacheCenterClientbound, centerChunk);

  for (int32_t x = -chunkRadiusX; x <= chunkRadiusX; x++) {
    for (int32_t z = -chunkRadiusZ; z <= chunkRadiusZ; z++) {
      ByteBuffer chunkPacket = buildChunkPacket(x, z, [&room](int32_t wx, int32_t wy, int32_t wz) { return boxRoomBlockAt(room, wx, wy, wz); });
      writePacket(socketFd, packet::play::levelChunkWithLightClientbound, chunkPacket);
    }
  }
}

void spawnCameraSeat(int socketFd) {
  writePacket(
    socketFd,
    packet::play::addEntityClientbound,
    buildAddEntityPacket(cameraAnchorEntityId, randomUuid(), blockDisplayEntityTypeId, viewportOriginX, playerFeetY, cameraZ)
  );
  writePacket(socketFd, packet::play::teleportEntityClientbound, buildTeleportEntityPacket(cameraAnchorEntityId, viewportOriginX, playerFeetY, cameraZ, 0.0f, 0.0f, false));

  ByteBuffer setPassengers;
  setPassengers.writeVarInt(cameraAnchorEntityId);
  setPassengers.writeVarInt(1);
  setPassengers.writeVarInt(playerEntityId);
  writePacket(socketFd, packet::play::setPassengersClientbound, setPassengers);

  writePacket(socketFd, packet::play::setCameraClientbound, buildSetCameraPacket(cameraAnchorEntityId));

  writePacket(socketFd, packet::play::postEffectsClientbound, buildPostEffectsPacket({"flatland:flatland_letterbox"}));
}

void runPlayLoop(int socketFd, PlayerSession &session) {
  using clock = std::chrono::steady_clock;
  constexpr auto keepAliveInterval = std::chrono::seconds(10);
  constexpr auto tickInterval = std::chrono::milliseconds(25);
  auto start = clock::now();
  auto lastKeepAlive = start;
  auto lastTick = start;

  while (true) {
    pollfd pollTarget{socketFd, POLLIN, 0};
    int pollResult = ::poll(&pollTarget, 1, 50);

    auto now = clock::now();
    if (now - lastKeepAlive >= keepAliveInterval) {
      ByteBuffer keepAlive;
      keepAlive.writeLong(static_cast<int64_t>(now.time_since_epoch().count()));
      writePacket(socketFd, packet::play::keepAliveClientbound, keepAlive);
      lastKeepAlive = now;
    }

    if (now - lastTick >= tickInterval) {
      double elapsedSeconds = std::chrono::duration<double>(now - start).count();
      session.runTick(elapsedSeconds);
      lastTick = now;
    }

    if (pollResult <= 0) continue;
    if (pollTarget.revents & (POLLHUP | POLLERR)) return;

    RawPacket packet = readPacket(socketFd);
    try {
      session.handleServerboundPacket(packet.packetId, packet.payload.data(), packet.payload.size());
    } catch (const std::exception &error) {
      std::fprintf(stderr, "flatland: failed to parse serverbound packet id=%d len=%zu: %s\n", packet.packetId, packet.payload.size(), error.what());
      std::fflush(stderr);
    }
  }
}

} // namespace

void serveConnection(int socketFd, const ServerOptions &options, const std::function<HostResolution(const std::string &hostname)> &resolveHost) {
  RawPacket handshake = readPacket(socketFd);
  ByteReader handshakeReader(handshake.payload.data(), handshake.payload.size());
  handshakeReader.readVarInt(); // protocol version
  std::string clientReportedAddress = handshakeReader.readString();
  handshakeReader.readUnsignedShort(); // server port
  int32_t nextState = handshakeReader.readVarInt();

  if (nextState == 1) {
    handleStatus(socketFd, options);
    return;
  }

  handleLogin(socketFd);

  HostResolution resolution = resolveHost(clientReportedAddress);
  const App &app = *resolution.app;

  if (app.redirectTarget()) {
    sendTransfer(socketFd, app.redirectTarget()->host, app.redirectTarget()->port);
    return;
  }

  handleConfiguration(socketFd, clientReportedAddress, *resolution.resourcePack);

  RoomBounds room = computePlayerCellBounds();
  constexpr int32_t chunkRadius = 4;

  sendJoinSequence(socketFd, options, room, chunkRadius, chunkRadius);
  spawnCameraSeat(socketFd);

  int32_t nextEntityId = firstSpriteEntityId;
  PlayerSession session(socketFd, app.viewport(), app.textures(), app.sounds(), nextEntityId);
  if (app.joinCallback()) app.joinCallback()(session);

  runPlayLoop(socketFd, session);
}

} // namespace flatland
