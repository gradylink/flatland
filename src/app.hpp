#pragma once
#include "playerSession.hpp"
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace flatland {

struct RedirectTarget {
  std::string host;
  uint16_t port;
};

class App {
public:
  void setViewport(Viewport newViewport) { viewportValue = newViewport; }
  const Viewport &viewport() const { return viewportValue; }

  TextureId registerTexture(const std::string &name, std::vector<uint8_t> pngBytes) { return textureRegistry.registerTexture(name, std::move(pngBytes)); }
  SoundId registerSound(const std::string &name, std::vector<uint8_t> oggBytes) { return soundRegistry.registerSound(name, std::move(oggBytes)); }

  const TextureRegistry &textures() const { return textureRegistry; }
  const SoundRegistry &sounds() const { return soundRegistry; }

  void onPlayerJoin(std::function<void(PlayerSession &)> callback) { joinHandler = std::move(callback); }
  const std::function<void(PlayerSession &)> &joinCallback() const { return joinHandler; }

  void redirectTo(std::string host, uint16_t port) { redirect = RedirectTarget{std::move(host), port}; }
  const std::optional<RedirectTarget> &redirectTarget() const { return redirect; }

private:
  Viewport viewportValue;
  TextureRegistry textureRegistry;
  SoundRegistry soundRegistry;
  std::function<void(PlayerSession &)> joinHandler;
  std::optional<RedirectTarget> redirect;
};

} // namespace flatland
