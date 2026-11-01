#pragma once
#include "playerSession.hpp"
#include "server.hpp"
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <functional>
#include <string>

namespace flatland {

struct ResourcePackInfo {
  uint16_t httpPort;
  std::string sha1Hex;
};

void serveConnection(
  int socketFd,
  const ServerOptions &options,
  const ResourcePackInfo &resourcePack,
  const Viewport &viewport,
  const TextureRegistry &textures,
  const SoundRegistry &sounds,
  const std::function<void(PlayerSession &)> &joinCallback
);

} // namespace flatland
