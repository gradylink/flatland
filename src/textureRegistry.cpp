#include "textureRegistry.hpp"
#include <algorithm>
#include <stdexcept>

namespace flatland {

namespace {

int32_t readBigEndianInt32(const uint8_t *bytes) {
  return static_cast<int32_t>((static_cast<uint32_t>(bytes[0]) << 24) | (static_cast<uint32_t>(bytes[1]) << 16) | (static_cast<uint32_t>(bytes[2]) << 8) | bytes[3]);
}

TextureSize readPngSize(const std::vector<uint8_t> &pngBytes) {
  static constexpr uint8_t signature[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
  constexpr size_t headerEnd = 24;
  if (pngBytes.size() < headerEnd || !std::equal(signature, signature + 8, pngBytes.begin()) || !std::equal(pngBytes.begin() + 12, pngBytes.begin() + 16, "IHDR")) {
    throw std::invalid_argument("texture is not a PNG");
  }
  return {readBigEndianInt32(&pngBytes[16]), readBigEndianInt32(&pngBytes[20])};
}

} // namespace

TextureId TextureRegistry::registerTexture(const std::string &name, std::vector<uint8_t> pngBytes) {
  TextureSize size = readPngSize(pngBytes);
  TextureId id = static_cast<TextureId>(entries.size());
  entries.push_back({name, std::move(pngBytes), size});
  return id;
}

TextureSize TextureRegistry::size(TextureId texture) const {
  if (texture < 0 || static_cast<size_t>(texture) >= entries.size()) throw std::out_of_range("unknown TextureId");
  return entries[static_cast<size_t>(texture)].size;
}

std::string TextureRegistry::itemModelId(TextureId texture) const {
  if (texture < 0 || static_cast<size_t>(texture) >= entries.size()) throw std::out_of_range("unknown TextureId");
  return "flatland:sprite/" + entries[static_cast<size_t>(texture)].name;
}

void TextureRegistry::addResourcePackFiles(ZipWriter &zip) const {
  for (const Entry &entry : entries) {
    std::string itemDefinition = "{\"model\":{\"type\":\"minecraft:model\",\"model\":\"flatland:item/sprite_" + entry.name + "\"}}";
    std::string model = "{\"parent\":\"minecraft:item/generated\",\"textures\":{\"layer0\":\"flatland:item/sprite_" + entry.name + "\"}}";
    std::string pngContents(entry.pngBytes.begin(), entry.pngBytes.end());

    zip.addFile("assets/flatland/items/sprite/" + entry.name + ".json", itemDefinition);
    zip.addFile("assets/flatland/models/item/sprite_" + entry.name + ".json", model);
    zip.addFile("assets/flatland/textures/item/sprite_" + entry.name + ".png", pngContents);
  }
}

} // namespace flatland
