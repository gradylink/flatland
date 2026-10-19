#pragma once
#include "byteBuffer.hpp"
#include <cstdint>

namespace flatland {

constexpr int32_t worldSectionCount = 24;
constexpr int32_t plainsBiomeId = 0;

ByteBuffer buildEmptyChunkPacket(int32_t chunkX, int32_t chunkZ, int32_t biomeId = plainsBiomeId);

} // namespace flatland
