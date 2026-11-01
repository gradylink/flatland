#pragma once
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace flatland {

using SpriteId = int32_t;

struct InputState {
  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool jump = false;
  bool sneak = false;
  bool sprint = false;
};

class PlayerSession {
public:
  PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, const SoundRegistry &sounds, int32_t &nextEntityId);

  SpriteId createSprite(TextureId texture, double x, double y);
  void moveSprite(SpriteId sprite, double x, double y);
  void removeSprite(SpriteId sprite);

  void playSound(SoundId sound, float volume = 1.0f, float pitch = 1.0f);
  void stopSound(SoundId sound);
  void stopAllSounds();

  void playSound(const std::string &soundEventName, float volume = 1.0f, float pitch = 1.0f);
  void stopSound(const std::string &soundEventName);

  void onTick(std::function<void(PlayerSession &, double)> callback);

  void onInput(std::function<void(PlayerSession &, const InputState &)> callback);
  void onLook(std::function<void(PlayerSession &, float yaw, float pitch)> callback);
  void onHotbarSlotChanged(std::function<void(PlayerSession &, int32_t slot)> callback);
  void onDropItem(std::function<void(PlayerSession &, bool dropStack)> callback);
  void onSwapHands(std::function<void(PlayerSession &)> callback);
  void onOpenInventory(std::function<void(PlayerSession &)> callback);
  void onLeftClick(std::function<void(PlayerSession &)> callback);
  void onRightClick(std::function<void(PlayerSession &)> callback);

  void runTick(double elapsedSeconds);
  void handleServerboundPacket(int32_t packetId, const uint8_t *payload, size_t length);

private:
  void ensureHotbarItem();

  int socketFd;
  const Viewport &viewport;
  const TextureRegistry &textures;
  const SoundRegistry &sounds;
  int32_t &nextEntityId;
  std::unordered_map<SpriteId, int32_t> spriteEntityIds;
  SpriteId nextSpriteId = 0;
  int32_t nextContainerStateId = 0;
  std::function<void(PlayerSession &, double)> tickCallback;

  std::function<void(PlayerSession &, const InputState &)> inputCallback;
  std::function<void(PlayerSession &, float, float)> lookCallback;
  std::function<void(PlayerSession &, int32_t)> hotbarSlotCallback;
  std::function<void(PlayerSession &, bool)> dropItemCallback;
  std::function<void(PlayerSession &)> swapHandsCallback;
  std::function<void(PlayerSession &)> openInventoryCallback;
  std::function<void(PlayerSession &)> leftClickCallback;
  std::function<void(PlayerSession &)> rightClickCallback;
};

} // namespace flatland
