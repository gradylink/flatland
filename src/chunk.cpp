#include "chunk.hpp"

namespace flatland {

ByteBuffer buildEmptyChunkPacket(int32_t chunkX, int32_t chunkZ, int32_t biomeId) {
  ByteBuffer sections;
  for (int32_t i = 0; i < worldSectionCount; i++) {
    sections.writeShort(0);  // block count
    sections.writeShort(0);  // fluid count
    sections.writeByte(0);   // block palette
    sections.writeVarInt(0); // minecraft:air
    sections.writeByte(0);   // biome palette
    sections.writeVarInt(biomeId);
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

} // namespace flatland
