#pragma once
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <vector>

namespace flatland {

std::vector<uint8_t> buildResourcePackZip(const Viewport &viewport, const TextureRegistry &textures);

}
