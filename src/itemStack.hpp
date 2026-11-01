#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <string>

namespace flatland {

constexpr int32_t paperItemId = 1145;        // minecraft:paper
constexpr int32_t itemModelComponentId = 10; // minecraft:item_model
constexpr int32_t itemNameComponentId = 9;   // minecraft:item_name

constexpr const char *emptyItemModelId = "flatland:empty";

ByteBuffer buildSpriteItemStack(const std::string &itemModelId);
ByteBuffer buildHotbarPlaceholderItemStack();

ByteBuffer buildContainerSetSlotPacket(int32_t windowId, int32_t stateId, int16_t slot, const ByteBuffer &slotData);

} // namespace flatland
