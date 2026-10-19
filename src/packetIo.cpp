#include "packetIo.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace flatland {

namespace {

void readFull(int socketFd, uint8_t *data, size_t length) {
  size_t totalRead = 0;
  while (totalRead < length) {
    ssize_t bytesRead = ::recv(socketFd, data + totalRead, length - totalRead, 0);
    if (bytesRead <= 0) throw std::runtime_error("connection closed while reading");
    totalRead += static_cast<size_t>(bytesRead);
  }
}

uint8_t readByteFromSocket(int socketFd) {
  uint8_t value;
  readFull(socketFd, &value, 1);
  return value;
}

int32_t readVarIntFromSocket(int socketFd) {
  uint32_t result = 0;
  for (int position = 0; position < 32; position += 7) {
    uint8_t currentByte = readByteFromSocket(socketFd);
    result |= static_cast<uint32_t>(currentByte & 0x7F) << position;
    if ((currentByte & 0x80) == 0) return static_cast<int32_t>(result);
  }
  throw std::runtime_error("VarInt too large");
}

void writeAll(int socketFd, const uint8_t *data, size_t length) {
  size_t totalWritten = 0;
  while (totalWritten < length) {
    ssize_t bytesWritten = ::send(socketFd, data + totalWritten, length - totalWritten, 0);
    if (bytesWritten <= 0) throw std::runtime_error("connection closed while writing");
    totalWritten += static_cast<size_t>(bytesWritten);
  }
}

} // namespace

RawPacket readPacket(int socketFd) {
  int32_t packetLength = readVarIntFromSocket(socketFd);
  if (packetLength <= 0 || packetLength > (1 << 21)) throw std::runtime_error("invalid packet length");

  std::vector<uint8_t> body(static_cast<size_t>(packetLength));
  readFull(socketFd, body.data(), body.size());

  ByteReader reader(body.data(), body.size());
  int32_t packetId = reader.readVarInt();
  size_t headerSize = body.size() - reader.remaining();

  RawPacket packet;
  packet.packetId = packetId;
  packet.payload.assign(body.begin() + static_cast<long>(headerSize), body.end());
  return packet;
}

void writeRaw(int socketFd, const unsigned char *data, size_t length) { writeAll(socketFd, data, length); }

void writePacket(int socketFd, int32_t packetId, const ByteBuffer &payload) {
  ByteBuffer header;
  header.writeVarInt(packetId);

  ByteBuffer lengthPrefix;
  lengthPrefix.writeVarInt(static_cast<int32_t>(header.size() + payload.size()));

  writeAll(socketFd, lengthPrefix.data().data(), lengthPrefix.size());
  writeAll(socketFd, header.data().data(), header.size());
  writeAll(socketFd, payload.data().data(), payload.size());
}

} // namespace flatland
