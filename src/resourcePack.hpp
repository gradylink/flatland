#pragma once
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

struct ResourcePackInfo {
  uint16_t httpPort;
  std::string sha1Hex;
};

std::vector<uint8_t> buildResourcePackZip(const Viewport &viewport, const TextureRegistry &textures, const SoundRegistry &sounds);

}
