#include "registries.hpp"
#include "bannerPatterns.hpp"
#include "byteBuffer.hpp"
#include "damageTypes.hpp"
#include "decoratedPotPatterns.hpp"
#include "instruments.hpp"
#include "jukeboxSongs.hpp"
#include "nbt.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "tags.hpp"
#include <string>
#include <utility>
#include <vector>

namespace flatland {

namespace {

constexpr const char *placeholderString = "minecraft:entity.generic.hurt";

void writePlaceholderStringsCompound(NbtWriter &nbt, const std::string &compoundName, const std::vector<std::string> &fields) {
  nbt.beginCompound(compoundName);
  for (const std::string &field : fields) nbt.writeString(field, placeholderString);
  nbt.endCompound();
}

ByteBuffer buildDimensionTypeEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeFloat("ambient_light", 0.0f);
  nbt.writeDouble("coordinate_scale", 1.0);
  nbt.writeByte("has_ceiling", 0);
  nbt.writeByte("has_ender_dragon_fight", 0);
  nbt.writeByte("has_skylight", 1);
  nbt.writeInt("height", 384);
  nbt.writeString("infiniburn", "minecraft:lava");
  nbt.writeInt("logical_height", 384);
  nbt.writeInt("min_y", -64);
  nbt.writeInt("monster_spawn_block_light_limit", 0);
  nbt.writeInt("monster_spawn_light_level", 0);
  nbt.beginCompound("attributes");
  nbt.endCompound();
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildBiomeEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeByte("has_precipitation", 0);
  nbt.writeFloat("temperature", 0.5f);
  nbt.writeFloat("downfall", 0.5f);
  nbt.beginCompound("effects");
  nbt.writeInt("fog_color", 0xC0D8FF);
  nbt.writeInt("water_color", 0x3F76E4);
  nbt.writeInt("water_fog_color", 0x050533);
  nbt.writeInt("sky_color", 0x78A7FF);
  nbt.endCompound();
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildAssetVariantEntryNbt(const std::string &assetId, const std::string &babyAssetId) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("asset_id", assetId);
  nbt.writeString("baby_asset_id", babyAssetId);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildSimpleVariantEntryNbt(const std::string &entityFolder, const std::string &name) {
  std::string base = "minecraft:entity/" + entityFolder + "/" + entityFolder + "_" + name;
  return buildAssetVariantEntryNbt(base, base + "_baby");
}

const std::vector<std::string> &catVariantNames() {
  static const std::vector<std::string> names = {"all_black", "black", "british_shorthair", "calico", "jellie", "persian", "ragdoll", "red", "siamese", "tabby", "white"};
  return names;
}

ByteBuffer buildCatSoundVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  std::vector<std::string> fields =
    {"ambient_sound", "beg_for_food_sound", "death_sound", "eat_sound", "hiss_sound", "hurt_sound", "purr_sound", "purreow_sound", "stray_ambient_sound"};
  writePlaceholderStringsCompound(nbt, "adult_sounds", fields);
  writePlaceholderStringsCompound(nbt, "baby_sounds", fields);
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &climateVariantNames() {
  static const std::vector<std::string> names = {"cold", "temperate", "warm"};
  return names;
}

ByteBuffer buildChickenSoundVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  std::vector<std::string> fields = {"ambient_sound", "death_sound", "hurt_sound", "step_sound"};
  writePlaceholderStringsCompound(nbt, "adult_sounds", fields);
  writePlaceholderStringsCompound(nbt, "baby_sounds", fields);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildCowSoundVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("ambient_sound", placeholderString);
  nbt.writeString("death_sound", placeholderString);
  nbt.writeString("hurt_sound", placeholderString);
  nbt.writeString("step_sound", placeholderString);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildPigSoundVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  std::vector<std::string> fields = {"ambient_sound", "death_sound", "eat_sound", "hurt_sound", "step_sound"};
  writePlaceholderStringsCompound(nbt, "adult_sounds", fields);
  writePlaceholderStringsCompound(nbt, "baby_sounds", fields);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildWolfVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  std::vector<std::string> fields = {"angry", "tame", "wild"};
  writePlaceholderStringsCompound(nbt, "assets", fields);
  writePlaceholderStringsCompound(nbt, "baby_assets", fields);
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &wolfVariantNames() {
  static const std::vector<std::string> names = {"ashen", "black", "chestnut", "pale", "rusty", "snowy", "spotted", "striped", "woods"};
  return names;
}

ByteBuffer buildWolfSoundVariantEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  std::vector<std::string> fields = {"ambient_sound", "death_sound", "growl_sound", "hurt_sound", "pant_sound", "step_sound", "whine_sound"};
  writePlaceholderStringsCompound(nbt, "adult_sounds", fields);
  writePlaceholderStringsCompound(nbt, "baby_sounds", fields);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildFrogVariantEntryNbt(const std::string &name) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("asset_id", "minecraft:entity/frog/frog_" + name);
  nbt.endCompound();
  return buffer;
}

ByteBuffer buildPaintingVariantEntryNbt(const std::string &name) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("asset_id", "minecraft:" + name);
  nbt.writeInt("width", 1);
  nbt.writeInt("height", 1);
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &paintingVariantNames() {
  static const std::vector<std::string> names = {"alban", "aztec",      "aztec2",   "backyard",   "baroque",      "bomb",     "bouquet",  "burning_skull",
                                                 "bust",  "cavebird",   "changing", "cotan",      "courbet",      "creebet",  "dennis",   "donkey_kong",
                                                 "earth", "endboss",    "fern",     "fighters",   "finding",      "fire",     "graham",   "humble",
                                                 "kebab", "lowmist",    "match",    "meditative", "orb",          "owlemons", "passage",  "pigscene",
                                                 "plant", "pointer",    "pond",     "pool",       "prairie_ride", "sea",      "skeleton", "skull_and_roses",
                                                 "stage", "sunflowers", "sunset",   "tides",      "unpacked",     "void",     "wanderer", "wasteland",
                                                 "water", "wind",       "wither"};
  return names;
}

ByteBuffer buildZombieNautilusVariantEntryNbt(const std::string &name) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("asset_id", "minecraft:entity/nautilus/zombie_nautilus_" + name);
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &zombieNautilusVariantNames() {
  static const std::vector<std::string> names = {"temperate", "warm"};
  return names;
}

ByteBuffer buildTrimMaterialEntryNbt(const std::string &materialName) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.beginCompound("description");
  nbt.writeString("translate", "trim_material.minecraft." + materialName);
  nbt.endCompound();
  nbt.writeString("palette_id", "minecraft:trim/" + materialName);
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &trimMaterialNames() {
  static const std::vector<std::string> names = {"amethyst", "copper", "diamond", "emerald", "gold", "iron", "lapis", "netherite", "quartz", "redstone", "resin"};
  return names;
}

ByteBuffer buildTrimPatternEntryNbt(const std::string &patternName) {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.beginNetworkRootCompound();
  nbt.writeString("asset_id", "minecraft:" + patternName);
  nbt.writeByte("decal", 0);
  nbt.beginCompound("description");
  nbt.writeString("translate", "trim_pattern.minecraft." + patternName);
  nbt.endCompound();
  nbt.endCompound();
  return buffer;
}

const std::vector<std::string> &trimPatternNames() {
  static const std::vector<std::string> names =
    {"bolt", "coast", "dune", "eye", "flow", "host", "raiser", "rib", "sentry", "shaper", "silence", "snout", "spire", "tide", "vex", "ward", "wayfinder", "wild"};
  return names;
}

ByteBuffer buildBlockTransformerEntryNbt() {
  ByteBuffer buffer;
  NbtWriter nbt(buffer);
  nbt.writeNetworkRootList(NbtTagType::Compound, 1);
  nbt.beginCompound("block_state_provider");
  nbt.writeString("type", "minecraft:rule_based");
  nbt.beginList("rules", NbtTagType::Compound, 1);
  nbt.beginCompound("if_true");
  nbt.writeString("type", "minecraft:matching_blocks");
  nbt.writeString("blocks", "minecraft:stone");
  nbt.endCompound();
  nbt.beginCompound("then");
  nbt.writeString("id", "minecraft:stone");
  nbt.endCompound();
  nbt.endCompound();
  nbt.endCompound();
  nbt.beginList("disallowed_faces", NbtTagType::String, 0);
  nbt.writeInt("item_damage_per_use", 1);
  nbt.writeString("sound", "minecraft:item.shovel.flatten");
  nbt.endCompound();
  return buffer;
}

void sendRegistry(int socketFd, const std::string &registryId, const std::vector<std::pair<std::string, ByteBuffer>> &entries) {
  ByteBuffer payload;
  payload.writeString(registryId);
  payload.writeVarInt(static_cast<int32_t>(entries.size()));
  for (const auto &entry : entries) {
    payload.writeString(entry.first);
    payload.writeBool(true);
    payload.writeBytes(entry.second.data());
  }
  writePacket(socketFd, packet::configuration::registryDataClientbound, payload);
}

void sendRegistry(int socketFd, const std::string &registryId, const std::string &entryId, const ByteBuffer &entryNbt) {
  sendRegistry(socketFd, registryId, std::vector<std::pair<std::string, ByteBuffer>>{{entryId, entryNbt}});
}

template <typename Builder> std::vector<std::pair<std::string, ByteBuffer>> buildEntriesByName(const std::vector<std::string> &names, Builder builder) {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const std::string &name : names) entries.emplace_back("minecraft:" + name, builder(name));
  return entries;
}

} // namespace

void sendMinimalRegistries(int socketFd) {
  sendRegistry(socketFd, "minecraft:dimension_type", "minecraft:overworld", buildDimensionTypeEntryNbt());
  sendRegistry(socketFd, "minecraft:worldgen/biome", "minecraft:plains", buildBiomeEntryNbt());
  sendRegistry(socketFd, "minecraft:damage_type", buildDamageTypeEntries());
  sendRegistry(socketFd, "minecraft:jukebox_song", buildJukeboxSongEntries());
  sendRegistry(socketFd, "minecraft:instrument", buildInstrumentEntries());
  sendRegistry(socketFd, "minecraft:banner_pattern", buildBannerPatternEntries());
  sendRegistry(socketFd, "minecraft:decorated_pot_pattern", buildDecoratedPotPatternEntries());

  sendRegistry(socketFd, "minecraft:cat_variant", buildEntriesByName(catVariantNames(), [](const std::string &name) { return buildSimpleVariantEntryNbt("cat", name); }));
  sendRegistry(socketFd, "minecraft:cat_sound_variant", "minecraft:classic", buildCatSoundVariantEntryNbt());
  sendRegistry(socketFd, "minecraft:chicken_variant", buildEntriesByName(climateVariantNames(), [](const std::string &name) {
                 return buildSimpleVariantEntryNbt("chicken", name);
               }));
  sendRegistry(socketFd, "minecraft:chicken_sound_variant", "minecraft:classic", buildChickenSoundVariantEntryNbt());
  sendRegistry(socketFd, "minecraft:cow_variant", buildEntriesByName(climateVariantNames(), [](const std::string &name) { return buildSimpleVariantEntryNbt("cow", name); }));
  sendRegistry(socketFd, "minecraft:cow_sound_variant", "minecraft:moody", buildCowSoundVariantEntryNbt());
  sendRegistry(socketFd, "minecraft:pig_variant", buildEntriesByName(climateVariantNames(), [](const std::string &name) { return buildSimpleVariantEntryNbt("pig", name); }));
  sendRegistry(socketFd, "minecraft:pig_sound_variant", "minecraft:mini", buildPigSoundVariantEntryNbt());
  sendRegistry(socketFd, "minecraft:wolf_variant", buildEntriesByName(wolfVariantNames(), [](const std::string &) { return buildWolfVariantEntryNbt(); }));
  sendRegistry(socketFd, "minecraft:wolf_sound_variant", "minecraft:sad", buildWolfSoundVariantEntryNbt());
  sendRegistry(socketFd, "minecraft:frog_variant", buildEntriesByName(climateVariantNames(), buildFrogVariantEntryNbt));
  sendRegistry(socketFd, "minecraft:painting_variant", buildEntriesByName(paintingVariantNames(), buildPaintingVariantEntryNbt));
  sendRegistry(socketFd, "minecraft:zombie_nautilus_variant", buildEntriesByName(zombieNautilusVariantNames(), buildZombieNautilusVariantEntryNbt));

  std::vector<std::pair<std::string, ByteBuffer>> trimMaterials;
  for (const std::string &name : trimMaterialNames()) trimMaterials.emplace_back("minecraft:" + name, buildTrimMaterialEntryNbt(name));
  sendRegistry(socketFd, "minecraft:trim_material", trimMaterials);

  std::vector<std::pair<std::string, ByteBuffer>> trimPatterns;
  for (const std::string &name : trimPatternNames()) trimPatterns.emplace_back("minecraft:" + name, buildTrimPatternEntryNbt(name));
  sendRegistry(socketFd, "minecraft:trim_pattern", trimPatterns);

  sendRegistry(
    socketFd,
    "minecraft:block_transformer",
    {{"minecraft:shovel", buildBlockTransformerEntryNbt()}, {"minecraft:hoe", buildBlockTransformerEntryNbt()}, {"minecraft:axe", buildBlockTransformerEntryNbt()}}
  );

  sendMinimalTags(socketFd);
}

} // namespace flatland
