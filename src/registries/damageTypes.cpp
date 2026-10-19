#include "damageTypes.hpp"
#include "nbt.hpp"

namespace flatland {

namespace {

struct DamageTypeData {
  const char *name;
  const char *scaling;
  float exhaustion;
  const char *messageId;
  const char *effects;
  const char *deathMessageType;
};

const std::vector<DamageTypeData> &damageTypeTable() {
  static const std::vector<DamageTypeData> table = {
    {"minecraft:arrow", "when_caused_by_living_non_player", 0.10000000149011612f, "arrow", nullptr, nullptr},
    {"minecraft:bad_respawn_point", "always", 0.10000000149011612f, "badRespawnPoint", nullptr, "intentional_game_design"},
    {"minecraft:cactus", "when_caused_by_living_non_player", 0.10000000149011612f, "cactus", nullptr, nullptr},
    {"minecraft:campfire", "when_caused_by_living_non_player", 0.10000000149011612f, "inFire", "burning", nullptr},
    {"minecraft:cramming", "when_caused_by_living_non_player", 0.0f, "cramming", nullptr, nullptr},
    {"minecraft:dragon_breath", "when_caused_by_living_non_player", 0.0f, "dragonBreath", nullptr, nullptr},
    {"minecraft:drown", "when_caused_by_living_non_player", 0.0f, "drown", "drowning", nullptr},
    {"minecraft:dry_out", "when_caused_by_living_non_player", 0.10000000149011612f, "dryout", nullptr, nullptr},
    {"minecraft:ender_pearl", "when_caused_by_living_non_player", 0.0f, "fall", nullptr, "fall_variants"},
    {"minecraft:explosion", "always", 0.10000000149011612f, "explosion", nullptr, nullptr},
    {"minecraft:fall", "when_caused_by_living_non_player", 0.0f, "fall", nullptr, "fall_variants"},
    {"minecraft:falling_anvil", "when_caused_by_living_non_player", 0.10000000149011612f, "anvil", nullptr, nullptr},
    {"minecraft:falling_block", "when_caused_by_living_non_player", 0.10000000149011612f, "fallingBlock", nullptr, nullptr},
    {"minecraft:falling_stalactite", "when_caused_by_living_non_player", 0.10000000149011612f, "fallingStalactite", nullptr, nullptr},
    {"minecraft:fireball", "when_caused_by_living_non_player", 0.10000000149011612f, "fireball", "burning", nullptr},
    {"minecraft:fireworks", "when_caused_by_living_non_player", 0.10000000149011612f, "fireworks", nullptr, nullptr},
    {"minecraft:fly_into_wall", "when_caused_by_living_non_player", 0.0f, "flyIntoWall", nullptr, nullptr},
    {"minecraft:freeze", "when_caused_by_living_non_player", 0.0f, "freeze", "freezing", nullptr},
    {"minecraft:generic", "when_caused_by_living_non_player", 0.0f, "generic", nullptr, nullptr},
    {"minecraft:generic_kill", "when_caused_by_living_non_player", 0.0f, "genericKill", nullptr, nullptr},
    {"minecraft:hot_floor", "when_caused_by_living_non_player", 0.10000000149011612f, "hotFloor", "burning", nullptr},
    {"minecraft:in_fire", "when_caused_by_living_non_player", 0.10000000149011612f, "inFire", "burning", nullptr},
    {"minecraft:in_wall", "when_caused_by_living_non_player", 0.0f, "inWall", nullptr, nullptr},
    {"minecraft:indirect_magic", "when_caused_by_living_non_player", 0.0f, "indirectMagic", nullptr, nullptr},
    {"minecraft:lava", "when_caused_by_living_non_player", 0.10000000149011612f, "lava", "burning", nullptr},
    {"minecraft:lightning_bolt", "when_caused_by_living_non_player", 0.10000000149011612f, "lightningBolt", nullptr, nullptr},
    {"minecraft:mace_smash", "when_caused_by_living_non_player", 0.10000000149011612f, "mace_smash", nullptr, nullptr},
    {"minecraft:magic", "when_caused_by_living_non_player", 0.0f, "magic", nullptr, nullptr},
    {"minecraft:mob_attack", "when_caused_by_living_non_player", 0.10000000149011612f, "mob", nullptr, nullptr},
    {"minecraft:mob_attack_no_aggro", "when_caused_by_living_non_player", 0.10000000149011612f, "mob", nullptr, nullptr},
    {"minecraft:mob_projectile", "when_caused_by_living_non_player", 0.10000000149011612f, "mob", nullptr, nullptr},
    {"minecraft:on_fire", "when_caused_by_living_non_player", 0.0f, "onFire", "burning", nullptr},
    {"minecraft:out_of_world", "when_caused_by_living_non_player", 0.0f, "outOfWorld", nullptr, nullptr},
    {"minecraft:outside_border", "when_caused_by_living_non_player", 0.0f, "outsideBorder", nullptr, nullptr},
    {"minecraft:player_attack", "when_caused_by_living_non_player", 0.10000000149011612f, "player", nullptr, nullptr},
    {"minecraft:player_explosion", "always", 0.10000000149011612f, "explosion.player", nullptr, nullptr},
    {"minecraft:sonic_boom", "always", 0.0f, "sonic_boom", nullptr, nullptr},
    {"minecraft:spear", "when_caused_by_living_non_player", 0.10000000149011612f, "spear", nullptr, nullptr},
    {"minecraft:spit", "when_caused_by_living_non_player", 0.10000000149011612f, "mob", nullptr, nullptr},
    {"minecraft:stalagmite", "when_caused_by_living_non_player", 0.0f, "stalagmite", nullptr, nullptr},
    {"minecraft:starve", "when_caused_by_living_non_player", 0.0f, "starve", nullptr, nullptr},
    {"minecraft:sting", "when_caused_by_living_non_player", 0.10000000149011612f, "sting", nullptr, nullptr},
    {"minecraft:sulfur_cube_hot", "when_caused_by_living_non_player", 0.10000000149011612f, "sulfurCubeHot", "burning", nullptr},
    {"minecraft:sweet_berry_bush", "when_caused_by_living_non_player", 0.10000000149011612f, "sweetBerryBush", "poking", nullptr},
    {"minecraft:thorns", "when_caused_by_living_non_player", 0.10000000149011612f, "thorns", "thorns", nullptr},
    {"minecraft:thrown", "when_caused_by_living_non_player", 0.10000000149011612f, "thrown", nullptr, nullptr},
    {"minecraft:trident", "when_caused_by_living_non_player", 0.10000000149011612f, "trident", nullptr, nullptr},
    {"minecraft:unattributed_fireball", "when_caused_by_living_non_player", 0.10000000149011612f, "onFire", "burning", nullptr},
    {"minecraft:wind_charge", "when_caused_by_living_non_player", 0.10000000149011612f, "mob", nullptr, nullptr},
    {"minecraft:wither", "when_caused_by_living_non_player", 0.0f, "wither", nullptr, nullptr},
    {"minecraft:wither_skull", "when_caused_by_living_non_player", 0.10000000149011612f, "witherSkull", nullptr, nullptr},
  };
  return table;
}

} // namespace

std::vector<std::pair<std::string, ByteBuffer>> buildDamageTypeEntries() {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const DamageTypeData &data : damageTypeTable()) {
    ByteBuffer buffer;
    NbtWriter nbt(buffer);
    nbt.beginNetworkRootCompound();
    nbt.writeString("message_id", data.messageId);
    nbt.writeFloat("exhaustion", data.exhaustion);
    nbt.writeString("scaling", data.scaling);
    if (data.effects != nullptr) nbt.writeString("effects", data.effects);
    if (data.deathMessageType != nullptr) nbt.writeString("death_message_type", data.deathMessageType);
    nbt.endCompound();
    entries.emplace_back(data.name, buffer);
  }
  return entries;
}

} // namespace flatland
