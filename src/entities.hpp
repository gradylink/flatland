#pragma once
#include "byteBuffer.hpp"
#include "uuid.hpp"
#include <cstdint>

namespace flatland {

constexpr int32_t blockDisplayEntityTypeId = 15; // minecraft:block_display
constexpr int32_t itemDisplayEntityTypeId = 73;  // minecraft:item_display
constexpr int32_t goldBlockStateId = 3830;       // minecraft:gold_block

constexpr int32_t itemStackEntityDataTypeId = 7;
constexpr int32_t blockStateEntityDataTypeId = 14;
constexpr int32_t intEntityDataTypeId = 1;
constexpr int32_t floatEntityDataTypeId = 3;
constexpr int32_t vector3EntityDataTypeId = 39;
constexpr int32_t quaternionEntityDataTypeId = 40;

constexpr int32_t fullBrightBrightnessOverride = 15728880;

ByteBuffer buildAddEntityPacket(int32_t entityId, const Uuid &uuid, int32_t entityTypeId, double x, double y, double z);
ByteBuffer buildTeleportEntityPacket(int32_t entityId, double x, double y, double z, float yaw, float pitch, bool onGround);
ByteBuffer buildSetCameraPacket(int32_t entityId);

ByteBuffer buildBlockDisplayBlockStatePacket(int32_t entityId, int32_t blockStateId);

ByteBuffer buildDisplayScalePacket(int32_t entityId, float scaleX, float scaleY, float scaleZ);
ByteBuffer buildDisplayTransformPacket(
  int32_t entityId, float scaleX, float scaleY, float scaleZ, float rotationX, float rotationY, float rotationZ, float rotationW, float cullWidth, float cullHeight
);

ByteBuffer buildDisplayBrightnessOverridePacket(int32_t entityId, int32_t packedBrightness);
ByteBuffer buildDisplayDimensionsPacket(int32_t entityId, float width, float height);

ByteBuffer buildItemDisplayEmptyPacket(int32_t entityId);
ByteBuffer buildItemDisplayItemPacket(int32_t entityId, const ByteBuffer &itemStack);

} // namespace flatland
