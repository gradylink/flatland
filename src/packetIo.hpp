#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <vector>

namespace flatland {

struct RawPacket {
  int32_t packetId;
  std::vector<uint8_t> payload;
};

RawPacket readPacket(int socketFd);
void writePacket(int socketFd, int32_t packetId, const ByteBuffer &payload);
void writeRaw(int socketFd, const unsigned char *data, size_t length);

} // namespace flatland
