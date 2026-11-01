#include "itemStack.hpp"
#include "nbt.hpp"

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

ByteBuffer buildHotbarPlaceholderItemStack() {
  ByteBuffer buffer;
  buffer.writeVarInt(1);
  buffer.writeVarInt(paperItemId);
  buffer.writeVarInt(2);
  buffer.writeVarInt(0);
  buffer.writeVarInt(itemModelComponentId);
  buffer.writeString(emptyItemModelId);
  buffer.writeVarInt(itemNameComponentId);
  NbtWriter(buffer).writeAnonymousString("");
  return buffer;
}

ByteBuffer buildContainerSetSlotPacket(int32_t windowId, int32_t stateId, int16_t slot, const ByteBuffer &slotData) {
  ByteBuffer buffer;
  buffer.writeVarInt(windowId);
  buffer.writeVarInt(stateId);
  buffer.writeShort(slot);
  buffer.writeBytes(slotData.data());
  return buffer;
}

} // namespace flatland
