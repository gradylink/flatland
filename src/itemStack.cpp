#include "itemStack.hpp"

namespace flatland {

ByteBuffer buildSpriteItemStack(const std::string &itemModelId) {
  ByteBuffer buffer;
  buffer.writeVarInt(1);
  buffer.writeVarInt(paperItemId);
  buffer.writeVarInt(1);
  buffer.writeVarInt(0);
  buffer.writeVarInt(itemModelComponentId);
  buffer.writeString(itemModelId);
  return buffer;
}

} // namespace flatland
