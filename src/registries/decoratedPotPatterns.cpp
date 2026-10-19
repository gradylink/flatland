#include "decoratedPotPatterns.hpp"
#include "nbt.hpp"

namespace flatland {

namespace {

struct DecoratedPotPatternData {
  const char *name;
  const char *assetId;
};

const std::vector<DecoratedPotPatternData> &decoratedPotPatternTable() {
  static const std::vector<DecoratedPotPatternData> table = {
    {"minecraft:angler", "minecraft:angler_pottery_pattern"},
    {"minecraft:archer", "minecraft:archer_pottery_pattern"},
    {"minecraft:arms_up", "minecraft:arms_up_pottery_pattern"},
    {"minecraft:blade", "minecraft:blade_pottery_pattern"},
    {"minecraft:brewer", "minecraft:brewer_pottery_pattern"},
    {"minecraft:burn", "minecraft:burn_pottery_pattern"},
    {"minecraft:danger", "minecraft:danger_pottery_pattern"},
    {"minecraft:explorer", "minecraft:explorer_pottery_pattern"},
    {"minecraft:flow", "minecraft:flow_pottery_pattern"},
    {"minecraft:friend", "minecraft:friend_pottery_pattern"},
    {"minecraft:guster", "minecraft:guster_pottery_pattern"},
    {"minecraft:heart", "minecraft:heart_pottery_pattern"},
    {"minecraft:heartbreak", "minecraft:heartbreak_pottery_pattern"},
    {"minecraft:howl", "minecraft:howl_pottery_pattern"},
    {"minecraft:miner", "minecraft:miner_pottery_pattern"},
    {"minecraft:mourner", "minecraft:mourner_pottery_pattern"},
    {"minecraft:plenty", "minecraft:plenty_pottery_pattern"},
    {"minecraft:prize", "minecraft:prize_pottery_pattern"},
    {"minecraft:scrape", "minecraft:scrape_pottery_pattern"},
    {"minecraft:sheaf", "minecraft:sheaf_pottery_pattern"},
    {"minecraft:shelter", "minecraft:shelter_pottery_pattern"},
    {"minecraft:skull", "minecraft:skull_pottery_pattern"},
    {"minecraft:snort", "minecraft:snort_pottery_pattern"},
  };
  return table;
}

} // namespace

std::vector<std::pair<std::string, ByteBuffer>> buildDecoratedPotPatternEntries() {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const DecoratedPotPatternData &data : decoratedPotPatternTable()) {
    ByteBuffer buffer;
    NbtWriter nbt(buffer);
    nbt.beginNetworkRootCompound();
    nbt.writeString("asset_id", data.assetId);
    nbt.endCompound();
    entries.emplace_back(data.name, buffer);
  }
  return entries;
}

} // namespace flatland
