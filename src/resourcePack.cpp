#include "resourcePack.hpp"
#include "bundle.hpp"
#include "bundle_flatland_shaders.h"
#include "zipWriter.hpp"
#include <cstdio>

namespace flatland {

namespace {

std::string formatGlslFloat(double value) {
  char buffer[64];
  std::snprintf(buffer, sizeof(buffer), "%.6f", value);
  return std::string(buffer);
}

std::string replaceAll(std::string text, const std::string &from, const std::string &to) {
  size_t pos = 0;
  while ((pos = text.find(from, pos)) != std::string::npos) {
    text.replace(pos, from.size(), to);
    pos += to.size();
  }
  return text;
}

std::string instantiateShaderTemplate(const std::string &templateSource, const Viewport &viewport) {
  std::string result = replaceAll(templateSource, "__FLATLAND_VIEWPORT_HALF_WIDTH__", formatGlslFloat(viewportWorldHalfWidth(viewport)));
  result = replaceAll(result, "__FLATLAND_VIEWPORT_HALF_HEIGHT__", formatGlslFloat(viewportWorldHalfHeight(viewport)));
  return result;
}

constexpr const char *packMcmeta = R"({
  "pack": {
    "description": "Flatland server resource pack",
    "pack_format": 97,
    "min_format": [97, 1],
    "max_format": [97, 1]
  }
})";

} // namespace

std::vector<uint8_t> buildResourcePackZip(const Viewport &viewport, const TextureRegistry &textures) {
  bundle::archive shaders(bundle_flatland_shaders_archive());

  ZipWriter zip;
  zip.addFile("pack.mcmeta", packMcmeta);
  for (const bundle::entry &entry : shaders) {
    std::string instantiated = instantiateShaderTemplate(shaders.load_text(entry), viewport);
    zip.addFile("assets/" + entry.name(), instantiated);
  }
  textures.addResourcePackFiles(zip);

  return zip.finish();
}

} // namespace flatland
