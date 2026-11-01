#include "sound.hpp"
#include <cmath>

namespace flatland {

namespace {

int32_t toSoundFixedPoint(double worldCoordinate) { return static_cast<int32_t>(std::llround(worldCoordinate * 8.0)); }

constexpr float soundFixedRange = 64.0f;

} // namespace

ByteBuffer buildSoundEffectPacket(const std::string &soundName, int32_t category, double x, double y, double z, float volume, float pitch, int64_t seed) {
  ByteBuffer buffer;
  buffer.writeVarInt(0);
  buffer.writeString(soundName);
  buffer.writeBool(true);
  buffer.writeFloat(soundFixedRange);
  buffer.writeVarInt(category);
  buffer.writeInt(toSoundFixedPoint(x));
  buffer.writeInt(toSoundFixedPoint(y));
  buffer.writeInt(toSoundFixedPoint(z));
  buffer.writeFloat(volume);
  buffer.writeFloat(pitch);
  buffer.writeLong(seed);
  return buffer;
}

ByteBuffer buildStopSoundPacket(const std::string &soundName) {
  ByteBuffer buffer;
  buffer.writeByte(0x02);
  buffer.writeString(soundName);
  return buffer;
}

ByteBuffer buildStopAllSoundsPacket() {
  ByteBuffer buffer;
  buffer.writeByte(0x00);
  return buffer;
}

} // namespace flatland
