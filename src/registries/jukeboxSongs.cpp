#include "jukeboxSongs.hpp"
#include "nbt.hpp"

namespace flatland {

namespace {

struct JukeboxSongData {
  const char *name;
  const char *soundEvent;
  float lengthInSeconds;
  const char *translationKey;
  int32_t comparatorOutput;
};

const std::vector<JukeboxSongData> &jukeboxSongTable() {
  static const std::vector<JukeboxSongData> table = {
    {"minecraft:11", "minecraft:music_disc.11", 71.0f, "jukebox_song.minecraft.11", 11},
    {"minecraft:13", "minecraft:music_disc.13", 178.0f, "jukebox_song.minecraft.13", 1},
    {"minecraft:5", "minecraft:music_disc.5", 178.0f, "jukebox_song.minecraft.5", 15},
    {"minecraft:blocks", "minecraft:music_disc.blocks", 345.0f, "jukebox_song.minecraft.blocks", 3},
    {"minecraft:bounce", "minecraft:music_disc.bounce", 234.0f, "jukebox_song.minecraft.bounce", 8},
    {"minecraft:cat", "minecraft:music_disc.cat", 185.0f, "jukebox_song.minecraft.cat", 2},
    {"minecraft:chirp", "minecraft:music_disc.chirp", 185.0f, "jukebox_song.minecraft.chirp", 4},
    {"minecraft:creator", "minecraft:music_disc.creator", 176.0f, "jukebox_song.minecraft.creator", 12},
    {"minecraft:creator_music_box", "minecraft:music_disc.creator_music_box", 73.0f, "jukebox_song.minecraft.creator_music_box", 11},
    {"minecraft:far", "minecraft:music_disc.far", 174.0f, "jukebox_song.minecraft.far", 5},
    {"minecraft:lava_chicken", "minecraft:music_disc.lava_chicken", 134.0f, "jukebox_song.minecraft.lava_chicken", 9},
    {"minecraft:mall", "minecraft:music_disc.mall", 197.0f, "jukebox_song.minecraft.mall", 6},
    {"minecraft:mellohi", "minecraft:music_disc.mellohi", 96.0f, "jukebox_song.minecraft.mellohi", 7},
    {"minecraft:otherside", "minecraft:music_disc.otherside", 195.0f, "jukebox_song.minecraft.otherside", 14},
    {"minecraft:pigstep", "minecraft:music_disc.pigstep", 149.0f, "jukebox_song.minecraft.pigstep", 13},
    {"minecraft:precipice", "minecraft:music_disc.precipice", 299.0f, "jukebox_song.minecraft.precipice", 13},
    {"minecraft:relic", "minecraft:music_disc.relic", 218.0f, "jukebox_song.minecraft.relic", 14},
    {"minecraft:stal", "minecraft:music_disc.stal", 150.0f, "jukebox_song.minecraft.stal", 8},
    {"minecraft:strad", "minecraft:music_disc.strad", 188.0f, "jukebox_song.minecraft.strad", 9},
    {"minecraft:tears", "minecraft:music_disc.tears", 175.0f, "jukebox_song.minecraft.tears", 10},
    {"minecraft:wait", "minecraft:music_disc.wait", 238.0f, "jukebox_song.minecraft.wait", 12},
    {"minecraft:ward", "minecraft:music_disc.ward", 251.0f, "jukebox_song.minecraft.ward", 10},
  };
  return table;
}

} // namespace

std::vector<std::pair<std::string, ByteBuffer>> buildJukeboxSongEntries() {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const JukeboxSongData &data : jukeboxSongTable()) {
    ByteBuffer buffer;
    NbtWriter nbt(buffer);
    nbt.beginNetworkRootCompound();
    nbt.writeString("sound_event", data.soundEvent);
    nbt.writeFloat("length_in_seconds", data.lengthInSeconds);
    nbt.beginCompound("description");
    nbt.writeString("translate", data.translationKey);
    nbt.endCompound();
    nbt.writeInt("comparator_output", data.comparatorOutput);
    nbt.endCompound();
    entries.emplace_back(data.name, buffer);
  }
  return entries;
}

} // namespace flatland
