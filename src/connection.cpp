#include "connection.hpp"
#include "byteBuffer.hpp"
#include "chunk.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "registries.hpp"
#include "uuid.hpp"
#include <chrono>
#include <poll.h>
#include <string>

namespace flatland {

namespace {

constexpr int32_t protocolVersion = 777;
constexpr const char *gameVersion = "26.3";

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

void handleConfiguration(int socketFd) {
  readPacket(socketFd);

  ByteBuffer knownPacks;
  knownPacks.writeVarInt(1);
  knownPacks.writeString("minecraft");
  knownPacks.writeString("core");
  knownPacks.writeString(gameVersion);
  writePacket(socketFd, packet::configuration::selectKnownPacksClientbound, knownPacks);

  readPacket(socketFd);

  sendMinimalRegistries(socketFd);

  readPacket(socketFd);
}

void sendJoinSequence(int socketFd, const ServerOptions &options, int32_t chunkRadius) {
  constexpr int32_t entityId = 1;
  constexpr double spawnX = 8.5, spawnY = 100.0, spawnZ = 8.5;

  ByteBuffer loginPlay;
  loginPlay.writeInt(entityId);
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
  loginPlay.writeByte(1);     // game mode
  loginPlay.writeByte(0);     // previous game mode
  loginPlay.writeBool(false); // debug
  loginPlay.writeBool(true);  // flat
  loginPlay.writeBool(false); // has death location
  loginPlay.writeVarInt(0);   // portal cooldown
  loginPlay.writeVarInt(0);   // sea level
  loginPlay.writeBool(false); // online mode
  loginPlay.writeBool(false); // secure chat
  writePacket(socketFd, packet::play::loginClientbound, loginPlay);

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
      writePacket(socketFd, packet::play::levelChunkWithLightClientbound, buildEmptyChunkPacket(x, z));
    }
  }
}

void runPlayLoop(int socketFd) {
  using clock = std::chrono::steady_clock;
  constexpr auto keepAliveInterval = std::chrono::seconds(10);
  auto lastKeepAlive = clock::now();

  while (true) {
    pollfd pollTarget{socketFd, POLLIN, 0};
    int pollResult = ::poll(&pollTarget, 1, 2000);

    if (clock::now() - lastKeepAlive >= keepAliveInterval) {
      ByteBuffer keepAlive;
      keepAlive.writeLong(static_cast<int64_t>(clock::now().time_since_epoch().count()));
      writePacket(socketFd, packet::play::keepAliveClientbound, keepAlive);
      lastKeepAlive = clock::now();
    }

    if (pollResult <= 0) continue;
    if (pollTarget.revents & (POLLHUP | POLLERR)) return;

    readPacket(socketFd);
  }
}

} // namespace

void serveConnection(int socketFd, const ServerOptions &options) {
  RawPacket handshake = readPacket(socketFd);
  ByteReader handshakeReader(handshake.payload.data(), handshake.payload.size());
  handshakeReader.readVarInt();        // protocol version
  handshakeReader.readString();        // server address
  handshakeReader.readUnsignedShort(); // server port
  int32_t nextState = handshakeReader.readVarInt();

  if (nextState == 1) {
    handleStatus(socketFd, options);
    return;
  }

  handleLogin(socketFd);
  handleConfiguration(socketFd);

  constexpr int32_t chunkRadius = 3;
  sendJoinSequence(socketFd, options, chunkRadius);
  runPlayLoop(socketFd);
}

} // namespace flatland
