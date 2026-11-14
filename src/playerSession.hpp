#pragma once
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
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

struct SpriteTransform {
  double x = 0.0;
  double y = 0.0;
  double width = 0.0;
  double height = 0.0;
  double rotation = 0.0;
  int32_t layer = 0;

  bool operator==(const SpriteTransform &other) const {
    return x == other.x && y == other.y && width == other.width && height == other.height && rotation == other.rotation && layer == other.layer;
  }
};

class PlayerSession {
public:
  PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, const SoundRegistry &sounds, int32_t &nextEntityId);

  SpriteId createSprite(TextureId texture, double x, double y);
  void moveSprite(SpriteId sprite, double x, double y);
  void setSpriteTransform(SpriteId sprite, const SpriteTransform &transform);
  void setSpriteTexture(SpriteId sprite, TextureId texture);
  void setSpriteVisible(SpriteId sprite, bool visible);
  void removeSprite(SpriteId sprite);

  void playSound(SoundId sound, float volume = 1.0f, float pitch = 1.0f);
  void stopSound(SoundId sound);
  void stopAllSounds();

  void playSound(const std::string &soundEventName, float volume = 1.0f, float pitch = 1.0f);
  void stopSound(const std::string &soundEventName);

  void sendChatMessage(const std::string &text, bool actionBar = false);

  void transferTo(const std::string &host, uint16_t port);

  void onTick(std::function<void(PlayerSession &, double)> callback);

  void onInput(std::function<void(PlayerSession &, const InputState &)> callback);
  void onLook(std::function<void(PlayerSession &, float yaw, float pitch)> callback);
  void onHotbarSlotChanged(std::function<void(PlayerSession &, int32_t slot)> callback);
  void onDropItem(std::function<void(PlayerSession &, bool dropStack)> callback);
  void onSwapHands(std::function<void(PlayerSession &)> callback);
  void onOpenInventory(std::function<void(PlayerSession &)> callback);
  void onLeftClick(std::function<void(PlayerSession &)> callback);
  void onRightClick(std::function<void(PlayerSession &)> callback);
  void onChatMessage(std::function<void(PlayerSession &, const std::string &message)> callback);

  void post(std::function<void(PlayerSession &)> task);
  void runPostedTasks();

  void runTick(double elapsedSeconds);
  void handleServerboundPacket(int32_t packetId, const uint8_t *payload, size_t length);

private:
  void ensureHotbarItem();

  int socketFd;
  const Viewport &viewport;
  const TextureRegistry &textures;
  const SoundRegistry &sounds;
  int32_t &nextEntityId;
  std::mutex postedTasksMutex;
  std::deque<std::function<void(PlayerSession &)>> postedTasks;
  struct Sprite {
    int32_t entityId;
    TextureId texture;
    bool visible = true;
    std::optional<SpriteTransform> transform;
  };

  std::unordered_map<SpriteId, Sprite> sprites;
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
  std::function<void(PlayerSession &, const std::string &)> chatMessageCallback;
};

} // namespace flatland
