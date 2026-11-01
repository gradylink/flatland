#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <string>

namespace flatland {

constexpr int32_t soundCategoryMaster = 0;
constexpr int32_t soundCategoryMusic = 1;
constexpr int32_t soundCategoryRecord = 2;
constexpr int32_t soundCategoryWeather = 3;
constexpr int32_t soundCategoryBlock = 4;
constexpr int32_t soundCategoryHostile = 5;
constexpr int32_t soundCategoryNeutral = 6;
constexpr int32_t soundCategoryPlayer = 7;
constexpr int32_t soundCategoryAmbient = 8;
constexpr int32_t soundCategoryVoice = 9;

ByteBuffer buildSoundEffectPacket(const std::string &soundName, int32_t category, double x, double y, double z, float volume, float pitch, int64_t seed);
ByteBuffer buildStopSoundPacket(const std::string &soundName);
ByteBuffer buildStopAllSoundsPacket();

} // namespace flatland
