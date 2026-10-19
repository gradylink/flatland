#include "chunk.hpp"
#include <array>
#include <unordered_map>
#include <vector>

namespace flatland {

namespace {

constexpr int32_t blocksPerSection = 4096;

int32_t sectionBlockIndex(int32_t localX, int32_t localY, int32_t localZ) { return (localY << 8) | (localZ << 4) | localX; }

void writeSection(ByteBuffer &out, const std::array<int32_t, blocksPerSection> &blocks, int32_t biomeId) {
  int16_t blockCount = 0;
  for (int32_t id : blocks) {
    if (id != airBlockId) blockCount++;
  }
  out.writeShort(blockCount);
  out.writeShort(0); // fluid count

  bool uniform = true;
  for (int32_t id : blocks) {
    if (id != blocks[0]) {
      uniform = false;
      break;
    }
  }

  if (uniform) {
    out.writeByte(0); // block palette
    out.writeVarInt(blocks[0]);
  } else {
    std::vector<int32_t> palette;
    std::unordered_map<int32_t, int32_t> paletteIndex;
    for (int32_t id : blocks) {
      if (paletteIndex.find(id) == paletteIndex.end()) {
        paletteIndex[id] = static_cast<int32_t>(palette.size());
        palette.push_back(id);
      }
    }

    int32_t bitsPerEntry = 4;
    while ((1 << bitsPerEntry) < static_cast<int32_t>(palette.size())) bitsPerEntry++;

    out.writeByte(static_cast<uint8_t>(bitsPerEntry));
    out.writeVarInt(static_cast<int32_t>(palette.size()));
    for (int32_t id : palette) out.writeVarInt(id);

    int32_t entriesPerLong = 64 / bitsPerEntry;
    int32_t longCount = (blocksPerSection + entriesPerLong - 1) / entriesPerLong;
    for (int32_t longIndex = 0; longIndex < longCount; longIndex++) {
      uint64_t packed = 0;
      for (int32_t slot = 0; slot < entriesPerLong; slot++) {
        int32_t blockIndex = longIndex * entriesPerLong + slot;
        if (blockIndex >= blocksPerSection) break;
        uint64_t index = static_cast<uint64_t>(paletteIndex.at(blocks[blockIndex]));
        packed |= index << (slot * bitsPerEntry);
      }
      out.writeLong(static_cast<int64_t>(packed));
    }
  }

  out.writeByte(0); // biome palette
  out.writeVarInt(biomeId);
}

} // namespace

ByteBuffer buildChunkPacket(int32_t chunkX, int32_t chunkZ, const BlockAtFn &blockAt, int32_t biomeId) {
  ByteBuffer sections;
  for (int32_t section = 0; section < worldSectionCount; section++) {
    int32_t sectionBaseY = worldMinY + section * 16;

    std::array<int32_t, blocksPerSection> blocks{};
    for (int32_t localY = 0; localY < 16; localY++) {
      for (int32_t localZ = 0; localZ < 16; localZ++) {
        for (int32_t localX = 0; localX < 16; localX++) {
          int32_t worldX = chunkX * 16 + localX;
          int32_t worldY = sectionBaseY + localY;
          int32_t worldZ = chunkZ * 16 + localZ;
          blocks[sectionBlockIndex(localX, localY, localZ)] = blockAt(worldX, worldY, worldZ);
        }
      }
    }

    writeSection(sections, blocks, biomeId);
  }

  ByteBuffer packet;
  packet.writeInt(chunkX);
  packet.writeInt(chunkZ);
  packet.writeVarInt(0); // heightmaps
  packet.writeVarInt(static_cast<int32_t>(sections.size()));
  packet.writeBytes(sections.data());
  packet.writeVarInt(0); // block entities

  packet.writeVarInt(0); // sky light mask
  packet.writeVarInt(0); // block light mask
  packet.writeVarInt(0); // empty sky light mask
  packet.writeVarInt(0); // empty block light mask
  packet.writeVarInt(0); // sky light arrays
  packet.writeVarInt(0); // block light arrays

  return packet;
}

ByteBuffer buildEmptyChunkPacket(int32_t chunkX, int32_t chunkZ, int32_t biomeId) {
  return buildChunkPacket(chunkX, chunkZ, [](int32_t, int32_t, int32_t) { return airBlockId; }, biomeId);
}

int32_t spawnRoomBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) {
  constexpr int32_t roomMinX = 4, roomMaxX = 12;
  constexpr int32_t roomMinZ = 4, roomMaxZ = 12;
  constexpr int32_t floorY = 99, ceilingY = 104;

  if (worldX < roomMinX || worldX > roomMaxX || worldZ < roomMinZ || worldZ > roomMaxZ) return airBlockId;
  if (worldY < floorY || worldY > ceilingY) return airBlockId;

  bool isFloorOrCeiling = (worldY == floorY || worldY == ceilingY);
  bool isWall = (worldX == roomMinX || worldX == roomMaxX || worldZ == roomMinZ || worldZ == roomMaxZ);
  if (isFloorOrCeiling || isWall) return barrierBlockId;

  return airBlockId;
}

} // namespace flatland
