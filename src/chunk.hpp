#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <functional>

namespace flatland {

constexpr int32_t worldMinY = -64;
constexpr int32_t worldSectionCount = 24;
constexpr int32_t plainsBiomeId = 0;

constexpr int32_t airBlockId = 0;
constexpr int32_t barrierBlockId = 14381;

using BlockAtFn = std::function<int32_t(int32_t worldX, int32_t worldY, int32_t worldZ)>;

ByteBuffer buildChunkPacket(int32_t chunkX, int32_t chunkZ, const BlockAtFn &blockAt, int32_t biomeId = plainsBiomeId);

} // namespace flatland
