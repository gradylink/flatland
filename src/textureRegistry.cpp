#include "textureRegistry.hpp"
#include <stdexcept>

namespace flatland {

TextureId TextureRegistry::registerTexture(const std::string &name, std::vector<uint8_t> pngBytes) {
  TextureId id = static_cast<TextureId>(entries.size());
  entries.push_back({name, std::move(pngBytes)});
  return id;
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

}
