#include "tags.hpp"
#include "byteBuffer.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include <string>
#include <utility>
#include <vector>

namespace flatland {

namespace {

using TagEntries = std::vector<std::pair<std::string, std::vector<int32_t>>>;

const TagEntries &damageTypeTags() {
  static const TagEntries tags = {
    {"minecraft:bypasses_effects", {40}},
    {"minecraft:is_fall", {10, 8, 39}},
    {"minecraft:ignites_armor_stands", {21, 3}},
    {"minecraft:no_wolf_retaliation", {42}},
    {"minecraft:witch_resistant_to", {27, 23, 36, 44}},
    {"minecraft:bypasses_resistance", {32, 19}},
    {"minecraft:burns_armor_stands", {31}},
    {"minecraft:avoids_guardian_thorns", {27, 44, 15, 9, 35, 1}},
    {"minecraft:panic_environmental_causes", {2, 17, 20, 42, 21, 24, 25, 31}},
    {"minecraft:mace_smash", {26}},
    {"minecraft:bypasses_wolf_armor", {32, 19, 4, 6, 7, 17, 22, 23, 27, 33, 40, 44, 49}},
    {"minecraft:damages_helmet", {11, 12, 13}},
    {"minecraft:bypasses_shield", {31, 22, 4, 6, 16, 18, 49, 5, 40, 10, 8, 17, 39, 27, 23, 32, 19, 36, 33, 2, 3, 7, 11, 13, 20, 42, 21, 24, 25, 43}},
    {"minecraft:burn_from_stepping", {3, 20, 42}},
    {"minecraft:no_impact", {6}},
    {"minecraft:sulfur_cube_with_block_immune_to", {0, 2, 7, 10, 11, 12, 13, 17, 26, 20, 28, 29, 30, 34, 37, 38, 39, 41, 42, 43, 45, 46, 48, 15, 9, 35, 1}},
    {"minecraft:is_freezing", {17}},
    {"minecraft:is_projectile", {0, 46, 30, 47, 14, 50, 45, 48}},
    {"minecraft:bypasses_enchantments", {36}},
    {"minecraft:always_most_significant_fall", {32}},
    {"minecraft:can_break_armor_stand", {35, 34, 37, 26}},
    {"minecraft:bypasses_armor", {31, 22, 4, 6, 16, 18, 49, 5, 40, 10, 8, 17, 39, 27, 23, 32, 19, 36, 33}},
    {"minecraft:bypasses_cooldown", {}},
    {"minecraft:bypasses_invulnerability", {32, 19}},
    {"minecraft:no_knockback", {9, 35, 1, 21, 25, 31, 24, 20, 42, 22, 4, 6, 40, 2, 10, 8, 16, 32, 18, 27, 49, 5, 7, 43, 17, 39, 33, 19, 3, 37}},
    {"minecraft:no_anger", {29}},
    {"minecraft:is_drowning", {6}},
    {"minecraft:always_triggers_silverfish", {27}},
    {"minecraft:panic_causes", {2, 17, 20, 42, 21, 24, 25, 31, 0, 5, 9, 14, 15, 23, 27, 28, 30, 35, 36, 41, 45, 46, 47, 48, 49, 50, 34, 37, 26}},
    {"minecraft:always_hurts_ender_dragons", {15, 9, 35, 1}},
    {"minecraft:is_fire", {21, 3, 31, 24, 20, 42, 47, 14}},
    {"minecraft:wither_immune_to", {6}},
    {"minecraft:is_explosion", {15, 9, 35, 1}},
    {"minecraft:is_lightning", {25}},
    {"minecraft:always_kills_armor_stands", {0, 46, 14, 50, 48}},
    {"minecraft:is_player_attack", {34, 37, 26}},
  };
  return tags;
}

const TagEntries &bannerPatternTags() {
  static const TagEntries tags = {
    {"minecraft:no_item_required", {26, 27, 28, 29, 31, 38, 35, 37, 32, 36, 34, 33, 25, 5, 30, 39, 40, 41, 42, 7, 10, 9, 8, 3, 23, 19, 17, 20, 18, 1, 14, 15}},
    {"minecraft:pattern_item/piglin", {22}},
    {"minecraft:pattern_item/field_masoned", {2}},
    {"minecraft:pattern_item/flower", {12}},
    {"minecraft:pattern_item/flow", {11}},
    {"minecraft:pattern_item/creeper", {4}},
    {"minecraft:pattern_item/globe", {13}},
    {"minecraft:pattern_item/bordure_indented", {6}},
    {"minecraft:pattern_item/guster", {16}},
    {"minecraft:pattern_item/mojang", {21}},
    {"minecraft:pattern_item/skull", {24}},
  };
  return tags;
}

void writeTagRegistry(ByteBuffer &buffer, const std::string &registryId, const TagEntries &tags) {
  buffer.writeString(registryId);
  buffer.writeVarInt(static_cast<int32_t>(tags.size()));
  for (const auto &entry : tags) {
    buffer.writeString(entry.first);
    buffer.writeVarInt(static_cast<int32_t>(entry.second.size()));
    for (int32_t id : entry.second) buffer.writeVarInt(id);
  }
}

} // namespace

void sendMinimalTags(int socketFd) {
  ByteBuffer buffer;
  buffer.writeVarInt(2);
  writeTagRegistry(buffer, "minecraft:damage_type", damageTypeTags());
  writeTagRegistry(buffer, "minecraft:banner_pattern", bannerPatternTags());
  writePacket(socketFd, packet::configuration::updateTagsClientbound, buffer);
}

} // namespace flatland
