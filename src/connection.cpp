#include "connection.hpp"
#include "byteBuffer.hpp"
#include "chunk.hpp"
#include "entities.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "registries.hpp"
#include "uuid.hpp"
#include <chrono>
#include <cmath>
#include <poll.h>
#include <string>

namespace flatland {

namespace {

constexpr int32_t protocolVersion = 777;
constexpr const char *gameVersion = "26.3";

constexpr int32_t playerEntityId = 1;

constexpr int32_t testCameraAnchorEntityId = 1000;
constexpr int32_t testDisplayEntityId = 1001;
constexpr double testCameraAnchorX = 8.0, testCameraAnchorY = 101.5, testCameraAnchorZ = 5.0;
constexpr double testDisplayCenterX = 8.0, testDisplayY = 101.5, testDisplayZ = 8.0;
constexpr double testDisplayAmplitude = 2.0;

int64_t packPosition(int32_t x, int32_t y, int32_t z) {
  return ((static_cast<int64_t>(x) & 0x3FFFFFF) << 38) | ((static_cast<int64_t>(z) & 0x3FFFFFF) << 12) | (static_cast<int64_t>(y) & 0xFFF);
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

void sendJoinSequence(int socketFd, const ServerOptions &options, int32_t chunkRadius) {
  constexpr double spawnX = 8.5, spawnY = 100.0, spawnZ = 8.5;

  ByteBuffer loginPlay;
  loginPlay.writeInt(playerEntityId);
  loginPlay.writeBool(false); // hardcore
  loginPlay.writeVarInt(3);
  loginPlay.writeString("minecraft:overworld");
  loginPlay.writeString("minecraft:the_nether");
  loginPlay.writeString("minecraft:the_end");
  loginPlay.writeVarInt(options.maxPlayers);
  loginPlay.writeVarInt(chunkRadius); // view distance
  loginPlay.writeVarInt(chunkRadius); // simulation distance
  loginPlay.writeBool(false);         // reduced debug info
  loginPlay.writeBool(false);         // respawn screen
  loginPlay.writeBool(false);         // limited crafting
  loginPlay.writeVarInt(0);           // dimension type
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
  teleport.writeDouble(spawnX);
  teleport.writeDouble(spawnY);
  teleport.writeDouble(spawnZ);
  teleport.writeDouble(0);
  teleport.writeDouble(0);
  teleport.writeDouble(0); // velocity
  teleport.writeFloat(0);
  teleport.writeFloat(0); // yaw, pitch
  teleport.writeInt(0);   // flags: all fields absolute
  writePacket(socketFd, packet::play::playerPositionClientbound, teleport);

  ByteBuffer spawnPosition;
  spawnPosition.writeString("minecraft:overworld");
  spawnPosition.writeLong(packPosition(8, 100, 8));
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

  for (int32_t x = -chunkRadius; x <= chunkRadius; x++) {
    for (int32_t z = -chunkRadius; z <= chunkRadius; z++) {
      ByteBuffer chunkPacket = (x == 0 && z == 0) ? buildChunkPacket(x, z, spawnRoomBlockAt) : buildEmptyChunkPacket(x, z);
      writePacket(socketFd, packet::play::levelChunkWithLightClientbound, chunkPacket);
    }
  }
}

void spawnTestCameraDemo(int socketFd) {
  writePacket(
    socketFd,
    packet::play::addEntityClientbound,
    buildAddEntityPacket(testCameraAnchorEntityId, randomUuid(), blockDisplayEntityTypeId, testCameraAnchorX, testCameraAnchorY, testCameraAnchorZ)
  );
  // add_entity
  writePacket(
    socketFd,
    packet::play::teleportEntityClientbound,
    buildTeleportEntityPacket(testCameraAnchorEntityId, testCameraAnchorX, testCameraAnchorY, testCameraAnchorZ, 0.0f, 0.0f, false)
  );

  writePacket(
    socketFd,
    packet::play::addEntityClientbound,
    buildAddEntityPacket(testDisplayEntityId, randomUuid(), blockDisplayEntityTypeId, testDisplayCenterX, testDisplayY, testDisplayZ)
  );
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildBlockDisplayBlockStatePacket(testDisplayEntityId, goldBlockStateId));

  writePacket(socketFd, packet::play::setCameraClientbound, buildSetCameraPacket(testCameraAnchorEntityId));
}

void runPlayLoop(int socketFd) {
  using clock = std::chrono::steady_clock;
  constexpr auto keepAliveInterval = std::chrono::seconds(10);
  constexpr auto animationInterval = std::chrono::milliseconds(50);
  auto start = clock::now();
  auto lastKeepAlive = start;
  auto lastAnimation = start;

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

    if (now - lastAnimation >= animationInterval) {
      double elapsedSeconds = std::chrono::duration<double>(now - start).count();
      double x = testDisplayCenterX + testDisplayAmplitude * std::sin(elapsedSeconds);
      writePacket(socketFd, packet::play::teleportEntityClientbound, buildTeleportEntityPacket(testDisplayEntityId, x, testDisplayY, testDisplayZ, 0.0f, 0.0f, false));
      lastAnimation = now;
    }

    if (pollResult <= 0) continue;
    if (pollTarget.revents & (POLLHUP | POLLERR)) return;

    readPacket(socketFd);
  }
}

} // namespace

void serveConnection(int socketFd, const ServerOptions &options, const ResourcePackInfo &resourcePack) {
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
  handleConfiguration(socketFd, clientReportedAddress, resourcePack);

  constexpr int32_t chunkRadius = 3;
  sendJoinSequence(socketFd, options, chunkRadius);
  spawnTestCameraDemo(socketFd);
  runPlayLoop(socketFd);
}

} // namespace flatland
