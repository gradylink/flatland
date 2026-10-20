#include "entities.hpp"

namespace flatland {

ByteBuffer buildAddEntityPacket(int32_t entityId, const Uuid &uuid, int32_t entityTypeId, double x, double y, double z) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeUuid(uuid.high, uuid.low);
  buffer.writeVarInt(entityTypeId);
  buffer.writeDouble(x);
  buffer.writeDouble(y);
  buffer.writeDouble(z);
  buffer.writeByte(0);
  buffer.writeByte(0);
  buffer.writeByte(0);
  buffer.writeByte(0);
  buffer.writeByte(0);
  return buffer;
}

ByteBuffer buildTeleportEntityPacket(int32_t entityId, double x, double y, double z, float yaw, float pitch, bool onGround) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(0);
  buffer.writeDouble(x);
  buffer.writeDouble(y);
  buffer.writeDouble(z);
  buffer.writeFloat(yaw);
  buffer.writeFloat(pitch);
  buffer.writeBool(onGround);
  return buffer;
}

ByteBuffer buildSetCameraPacket(int32_t entityId) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  return buffer;
}

ByteBuffer buildBlockDisplayBlockStatePacket(int32_t entityId, int32_t blockStateId) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(23);
  buffer.writeVarInt(blockStateEntityDataTypeId);
  buffer.writeVarInt(blockStateId);
  buffer.writeByte(0xFF);
  return buffer;
}

ByteBuffer buildDisplayScalePacket(int32_t entityId, float scaleX, float scaleY, float scaleZ) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(12);
  buffer.writeVarInt(vector3EntityDataTypeId);
  buffer.writeFloat(scaleX);
  buffer.writeFloat(scaleY);
  buffer.writeFloat(scaleZ);
  buffer.writeByte(0xFF);
  return buffer;
}

ByteBuffer buildDisplayBrightnessOverridePacket(int32_t entityId, int32_t packedBrightness) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(16);
  buffer.writeVarInt(intEntityDataTypeId);
  buffer.writeVarInt(packedBrightness);
  buffer.writeByte(0xFF);
  return buffer;
}

ByteBuffer buildDisplayDimensionsPacket(int32_t entityId, float width, float height) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(20);
  buffer.writeVarInt(floatEntityDataTypeId);
  buffer.writeFloat(width);
  buffer.writeByte(21);
  buffer.writeVarInt(floatEntityDataTypeId);
  buffer.writeFloat(height);
  buffer.writeByte(0xFF);
  return buffer;
}

ByteBuffer buildItemDisplayItemPacket(int32_t entityId, const ByteBuffer &itemStack) {
  ByteBuffer buffer;
  buffer.writeVarInt(entityId);
  buffer.writeByte(23);
  buffer.writeVarInt(itemStackEntityDataTypeId);
  buffer.writeBytes(itemStack.data());
  buffer.writeByte(0xFF);
  return buffer;
}

} // namespace flatland
