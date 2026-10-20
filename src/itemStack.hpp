#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <string>

namespace flatland {

constexpr int32_t paperItemId = 1145;        // minecraft:paper
constexpr int32_t itemModelComponentId = 10; // minecraft:item_model

ByteBuffer buildSpriteItemStack(const std::string &itemModelId);

} // namespace flatland
