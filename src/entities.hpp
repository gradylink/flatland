#pragma once
#include "byteBuffer.hpp"
#include "uuid.hpp"
#include <cstdint>

namespace flatland {

constexpr int32_t blockDisplayEntityTypeId = 15; // minecraft:block_display
constexpr int32_t goldBlockStateId = 3830;       // minecraft:gold_block

ByteBuffer buildAddEntityPacket(int32_t entityId, const Uuid &uuid, int32_t entityTypeId, double x, double y, double z);
ByteBuffer buildTeleportEntityPacket(int32_t entityId, double x, double y, double z, float yaw, float pitch, bool onGround);
ByteBuffer buildSetCameraPacket(int32_t entityId);

ByteBuffer buildBlockDisplayBlockStatePacket(int32_t entityId, int32_t blockStateId);

} // namespace flatland
