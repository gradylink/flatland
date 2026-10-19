#include "instruments.hpp"
#include "nbt.hpp"

namespace flatland {

namespace {

struct InstrumentData {
  const char *name;
  const char *soundEvent;
  float range;
  const char *translationKey;
  float useDuration;
};

const std::vector<InstrumentData> &instrumentTable() {
  static const std::vector<InstrumentData> table = {
    {"minecraft:admire_goat_horn", "minecraft:item.goat_horn.sound.4", 256.0f, "instrument.minecraft.admire_goat_horn", 7.0f},
    {"minecraft:call_goat_horn", "minecraft:item.goat_horn.sound.5", 256.0f, "instrument.minecraft.call_goat_horn", 7.0f},
    {"minecraft:dream_goat_horn", "minecraft:item.goat_horn.sound.7", 256.0f, "instrument.minecraft.dream_goat_horn", 7.0f},
    {"minecraft:feel_goat_horn", "minecraft:item.goat_horn.sound.3", 256.0f, "instrument.minecraft.feel_goat_horn", 7.0f},
    {"minecraft:ponder_goat_horn", "minecraft:item.goat_horn.sound.0", 256.0f, "instrument.minecraft.ponder_goat_horn", 7.0f},
    {"minecraft:seek_goat_horn", "minecraft:item.goat_horn.sound.2", 256.0f, "instrument.minecraft.seek_goat_horn", 7.0f},
    {"minecraft:sing_goat_horn", "minecraft:item.goat_horn.sound.1", 256.0f, "instrument.minecraft.sing_goat_horn", 7.0f},
    {"minecraft:yearn_goat_horn", "minecraft:item.goat_horn.sound.6", 256.0f, "instrument.minecraft.yearn_goat_horn", 7.0f},
  };
  return table;
}

} // namespace

std::vector<std::pair<std::string, ByteBuffer>> buildInstrumentEntries() {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const InstrumentData &data : instrumentTable()) {
    ByteBuffer buffer;
    NbtWriter nbt(buffer);
    nbt.beginNetworkRootCompound();
    nbt.writeString("sound_event", data.soundEvent);
    nbt.writeFloat("range", data.range);
    nbt.beginCompound("description");
    nbt.writeString("translate", data.translationKey);
    nbt.endCompound();
    nbt.writeFloat("use_duration", data.useDuration);
    nbt.endCompound();
    entries.emplace_back(data.name, buffer);
  }
  return entries;
}

} // namespace flatland
