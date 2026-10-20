#pragma once
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace flatland {

using SpriteId = int32_t;

class PlayerSession {
public:
  PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, int32_t &nextEntityId);

  SpriteId createSprite(TextureId texture, double x, double y);
  void moveSprite(SpriteId sprite, double x, double y);
  void removeSprite(SpriteId sprite);

  void onTick(std::function<void(PlayerSession &, double)> callback);

  void runTick(double elapsedSeconds);

private:
  int socketFd;
  const Viewport &viewport;
  const TextureRegistry &textures;
  int32_t &nextEntityId;
  std::unordered_map<SpriteId, int32_t> spriteEntityIds;
  SpriteId nextSpriteId = 0;
  std::function<void(PlayerSession &, double)> tickCallback;
};

} // namespace flatland
