#include "bannerPatterns.hpp"
#include "nbt.hpp"

namespace flatland {

namespace {

struct BannerPatternData {
  const char *name;
  const char *translationKey;
  const char *assetId;
};

const std::vector<BannerPatternData> &bannerPatternTable() {
  static const std::vector<BannerPatternData> table = {
    {"minecraft:base", "block.minecraft.banner.base", "minecraft:base"},
    {"minecraft:border", "block.minecraft.banner.border", "minecraft:border"},
    {"minecraft:bricks", "block.minecraft.banner.bricks", "minecraft:bricks"},
    {"minecraft:circle", "block.minecraft.banner.circle", "minecraft:circle"},
    {"minecraft:creeper", "block.minecraft.banner.creeper", "minecraft:creeper"},
    {"minecraft:cross", "block.minecraft.banner.cross", "minecraft:cross"},
    {"minecraft:curly_border", "block.minecraft.banner.curly_border", "minecraft:curly_border"},
    {"minecraft:diagonal_left", "block.minecraft.banner.diagonal_left", "minecraft:diagonal_left"},
    {"minecraft:diagonal_right", "block.minecraft.banner.diagonal_right", "minecraft:diagonal_right"},
    {"minecraft:diagonal_up_left", "block.minecraft.banner.diagonal_up_left", "minecraft:diagonal_up_left"},
    {"minecraft:diagonal_up_right", "block.minecraft.banner.diagonal_up_right", "minecraft:diagonal_up_right"},
    {"minecraft:flow", "block.minecraft.banner.flow", "minecraft:flow"},
    {"minecraft:flower", "block.minecraft.banner.flower", "minecraft:flower"},
    {"minecraft:globe", "block.minecraft.banner.globe", "minecraft:globe"},
    {"minecraft:gradient", "block.minecraft.banner.gradient", "minecraft:gradient"},
    {"minecraft:gradient_up", "block.minecraft.banner.gradient_up", "minecraft:gradient_up"},
    {"minecraft:guster", "block.minecraft.banner.guster", "minecraft:guster"},
    {"minecraft:half_horizontal", "block.minecraft.banner.half_horizontal", "minecraft:half_horizontal"},
    {"minecraft:half_horizontal_bottom", "block.minecraft.banner.half_horizontal_bottom", "minecraft:half_horizontal_bottom"},
    {"minecraft:half_vertical", "block.minecraft.banner.half_vertical", "minecraft:half_vertical"},
    {"minecraft:half_vertical_right", "block.minecraft.banner.half_vertical_right", "minecraft:half_vertical_right"},
    {"minecraft:mojang", "block.minecraft.banner.mojang", "minecraft:mojang"},
    {"minecraft:piglin", "block.minecraft.banner.piglin", "minecraft:piglin"},
    {"minecraft:rhombus", "block.minecraft.banner.rhombus", "minecraft:rhombus"},
    {"minecraft:skull", "block.minecraft.banner.skull", "minecraft:skull"},
    {"minecraft:small_stripes", "block.minecraft.banner.small_stripes", "minecraft:small_stripes"},
    {"minecraft:square_bottom_left", "block.minecraft.banner.square_bottom_left", "minecraft:square_bottom_left"},
    {"minecraft:square_bottom_right", "block.minecraft.banner.square_bottom_right", "minecraft:square_bottom_right"},
    {"minecraft:square_top_left", "block.minecraft.banner.square_top_left", "minecraft:square_top_left"},
    {"minecraft:square_top_right", "block.minecraft.banner.square_top_right", "minecraft:square_top_right"},
    {"minecraft:straight_cross", "block.minecraft.banner.straight_cross", "minecraft:straight_cross"},
    {"minecraft:stripe_bottom", "block.minecraft.banner.stripe_bottom", "minecraft:stripe_bottom"},
    {"minecraft:stripe_center", "block.minecraft.banner.stripe_center", "minecraft:stripe_center"},
    {"minecraft:stripe_downleft", "block.minecraft.banner.stripe_downleft", "minecraft:stripe_downleft"},
    {"minecraft:stripe_downright", "block.minecraft.banner.stripe_downright", "minecraft:stripe_downright"},
    {"minecraft:stripe_left", "block.minecraft.banner.stripe_left", "minecraft:stripe_left"},
    {"minecraft:stripe_middle", "block.minecraft.banner.stripe_middle", "minecraft:stripe_middle"},
    {"minecraft:stripe_right", "block.minecraft.banner.stripe_right", "minecraft:stripe_right"},
    {"minecraft:stripe_top", "block.minecraft.banner.stripe_top", "minecraft:stripe_top"},
    {"minecraft:triangle_bottom", "block.minecraft.banner.triangle_bottom", "minecraft:triangle_bottom"},
    {"minecraft:triangle_top", "block.minecraft.banner.triangle_top", "minecraft:triangle_top"},
    {"minecraft:triangles_bottom", "block.minecraft.banner.triangles_bottom", "minecraft:triangles_bottom"},
    {"minecraft:triangles_top", "block.minecraft.banner.triangles_top", "minecraft:triangles_top"},
  };
  return table;
}

} // namespace

std::vector<std::pair<std::string, ByteBuffer>> buildBannerPatternEntries() {
  std::vector<std::pair<std::string, ByteBuffer>> entries;
  for (const BannerPatternData &data : bannerPatternTable()) {
    ByteBuffer buffer;
    NbtWriter nbt(buffer);
    nbt.beginNetworkRootCompound();
    nbt.writeString("translation_key", data.translationKey);
    nbt.writeString("asset_id", data.assetId);
    nbt.endCompound();
    entries.emplace_back(data.name, buffer);
  }
  return entries;
}

} // namespace flatland
