#include "playerSession.hpp"
#include "byteBuffer.hpp"
#include "entities.hpp"
#include "itemStack.hpp"
#include "nbt.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "sound.hpp"
#include "transfer.hpp"
#include "uuid.hpp"
#include <cmath>

namespace flatland {

namespace {

constexpr int32_t playerActionDropAllItems = 4;
constexpr int32_t playerActionDropItem = 5;
constexpr int32_t playerActionSwapItemWithOffhand = 7;
constexpr int32_t playerCommandOpenInventory = 5;

constexpr int32_t playerInventoryWindowId = 0;
constexpr int16_t firstHotbarSlot = 36;
constexpr int16_t hotbarSlotCount = 9;

} // namespace

PlayerSession::PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, const SoundRegistry &sounds, int32_t &nextEntityId)
    : socketFd(socketFd), viewport(viewport), textures(textures), sounds(sounds), nextEntityId(nextEntityId) {
  ensureHotbarItem();
}

void PlayerSession::ensureHotbarItem() {
  for (int16_t slot = firstHotbarSlot; slot < firstHotbarSlot + hotbarSlotCount; slot++) {
    writePacket(
      socketFd,
      packet::play::containerSetSlotClientbound,
      buildContainerSetSlotPacket(playerInventoryWindowId, nextContainerStateId++, slot, buildHotbarPlaceholderItemStack())
    );
  }
}

SpriteId PlayerSession::createSprite(TextureId texture, double x, double y) {
  int32_t entityId = nextEntityId++;
  TextureSize size = textures.size(texture);

  writePacket(
    socketFd,
    packet::play::addEntityClientbound,
    buildAddEntityPacket(entityId, randomUuid(), itemDisplayEntityTypeId, viewportPixelToWorldX(viewport, x), viewportPixelToWorldY(viewport, y), layerToWorldZ(0))
  );
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildItemDisplayItemPacket(entityId, buildSpriteItemStack(textures.itemModelId(texture))));
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildDisplayBrightnessOverridePacket(entityId, fullBrightBrightnessOverride));

  SpriteId sprite = nextSpriteId++;
  sprites[sprite] = Sprite{entityId, texture, true, std::nullopt};

  SpriteTransform transform;
  transform.x = x;
  transform.y = y;
  transform.width = size.width;
  transform.height = size.height;
  setSpriteTransform(sprite, transform);
  return sprite;
}

void PlayerSession::moveSprite(SpriteId sprite, double x, double y) {
  auto it = sprites.find(sprite);
  if (it == sprites.end() || !it->second.transform) return;

  SpriteTransform transform = *it->second.transform;
  transform.x = x;
  transform.y = y;
  setSpriteTransform(sprite, transform);
}

void PlayerSession::setSpriteTransform(SpriteId sprite, const SpriteTransform &transform) {
  auto it = sprites.find(sprite);
  if (it == sprites.end()) return;
  Sprite &state = it->second;
  const std::optional<SpriteTransform> &sent = state.transform;

  if (!sent || sent->x != transform.x || sent->y != transform.y || sent->layer != transform.layer) {
    writePacket(
      socketFd,
      packet::play::teleportEntityClientbound,
      buildTeleportEntityPacket(
        state.entityId,
        viewportPixelToWorldX(viewport, transform.x),
        viewportPixelToWorldY(viewport, transform.y),
        layerToWorldZ(transform.layer),
        0.0f,
        0.0f,
        false
      )
    );
  }

  if (!sent || sent->width != transform.width || sent->height != transform.height || sent->rotation != transform.rotation) {
    float scaleX = static_cast<float>(transform.width / pixelsPerBlock);
    float scaleY = static_cast<float>(transform.height / pixelsPerBlock);
    double halfAngle = transform.rotation * (3.14159265358979323846 / 180.0) / 2.0;
    float cullSize = static_cast<float>(std::hypot(transform.width, transform.height) / pixelsPerBlock);
    writePacket(
      socketFd,
      packet::play::setEntityDataClientbound,
      buildDisplayTransformPacket(
        state.entityId,
        scaleX,
        scaleY,
        0.0f,
        0.0f,
        0.0f,
        static_cast<float>(std::sin(halfAngle)),
        static_cast<float>(std::cos(halfAngle)),
        cullSize,
        cullSize
      )
    );
  }

  state.transform = transform;
}

void PlayerSession::setSpriteTexture(SpriteId sprite, TextureId texture) {
  auto it = sprites.find(sprite);
  if (it == sprites.end() || it->second.texture == texture) return;

  it->second.texture = texture;
  if (it->second.visible)
    writePacket(socketFd, packet::play::setEntityDataClientbound, buildItemDisplayItemPacket(it->second.entityId, buildSpriteItemStack(textures.itemModelId(texture))));
}

void PlayerSession::setSpriteVisible(SpriteId sprite, bool visible) {
  auto it = sprites.find(sprite);
  if (it == sprites.end() || it->second.visible == visible) return;

  it->second.visible = visible;
  writePacket(
    socketFd,
    packet::play::setEntityDataClientbound,
    visible ? buildItemDisplayItemPacket(it->second.entityId, buildSpriteItemStack(textures.itemModelId(it->second.texture)))
            : buildItemDisplayEmptyPacket(it->second.entityId)
  );
}

void PlayerSession::removeSprite(SpriteId sprite) {
  auto it = sprites.find(sprite);
  if (it == sprites.end()) return;

  ByteBuffer buffer;
  buffer.writeVarInt(1);
  buffer.writeVarInt(it->second.entityId);
  writePacket(socketFd, packet::play::removeEntitiesClientbound, buffer);

  sprites.erase(it);
}

void PlayerSession::playSound(SoundId sound, float volume, float pitch) { playSound(sounds.soundEventName(sound), volume, pitch); }

void PlayerSession::stopSound(SoundId sound) { stopSound(sounds.soundEventName(sound)); }

void PlayerSession::stopAllSounds() { writePacket(socketFd, packet::play::stopSoundClientbound, buildStopAllSoundsPacket()); }

void PlayerSession::playSound(const std::string &soundEventName, float volume, float pitch) {
  writePacket(
    socketFd,
    packet::play::soundEffectClientbound,
    buildSoundEffectPacket(soundEventName, soundCategoryPlayer, viewportOriginX, playerFeetY, cameraZ, volume, pitch, 0)
  );
}

void PlayerSession::stopSound(const std::string &soundEventName) { writePacket(socketFd, packet::play::stopSoundClientbound, buildStopSoundPacket(soundEventName)); }

void PlayerSession::sendChatMessage(const std::string &text, bool actionBar) {
  ByteBuffer buffer;
  NbtWriter(buffer).writeAnonymousString(text);
  buffer.writeBool(actionBar);
  writePacket(socketFd, packet::play::systemChatClientbound, buffer);
}

void PlayerSession::transferTo(const std::string &host, uint16_t port) { writePacket(socketFd, packet::play::transferClientbound, buildTransferPacket(host, port)); }

void PlayerSession::onTick(std::function<void(PlayerSession &, double)> callback) { tickCallback = std::move(callback); }
void PlayerSession::onInput(std::function<void(PlayerSession &, const InputState &)> callback) { inputCallback = std::move(callback); }
void PlayerSession::onLook(std::function<void(PlayerSession &, float, float)> callback) { lookCallback = std::move(callback); }
void PlayerSession::onHotbarSlotChanged(std::function<void(PlayerSession &, int32_t)> callback) { hotbarSlotCallback = std::move(callback); }
void PlayerSession::onDropItem(std::function<void(PlayerSession &, bool)> callback) { dropItemCallback = std::move(callback); }
void PlayerSession::onSwapHands(std::function<void(PlayerSession &)> callback) { swapHandsCallback = std::move(callback); }
void PlayerSession::onOpenInventory(std::function<void(PlayerSession &)> callback) { openInventoryCallback = std::move(callback); }
void PlayerSession::onLeftClick(std::function<void(PlayerSession &)> callback) { leftClickCallback = std::move(callback); }
void PlayerSession::onRightClick(std::function<void(PlayerSession &)> callback) { rightClickCallback = std::move(callback); }
void PlayerSession::onChatMessage(std::function<void(PlayerSession &, const std::string &)> callback) { chatMessageCallback = std::move(callback); }

void PlayerSession::post(std::function<void(PlayerSession &)> task) {
  std::lock_guard<std::mutex> lock(postedTasksMutex);
  postedTasks.push_back(std::move(task));
}

void PlayerSession::runPostedTasks() {
  std::deque<std::function<void(PlayerSession &)>> tasks;
  {
    std::lock_guard<std::mutex> lock(postedTasksMutex);
    tasks.swap(postedTasks);
  }
  for (auto &task : tasks) task(*this);
}

void PlayerSession::runTick(double elapsedSeconds) {
  if (tickCallback) tickCallback(*this, elapsedSeconds);
}

void PlayerSession::handleServerboundPacket(int32_t packetId, const uint8_t *payload, size_t length) {
  ByteReader reader(payload, length);

  if (packetId == packet::play::playerInputServerbound) {
    uint8_t flags = reader.readByte();
    InputState state;
    state.forward = (flags & 0x01) != 0;
    state.backward = (flags & 0x02) != 0;
    state.left = (flags & 0x04) != 0;
    state.right = (flags & 0x08) != 0;
    state.jump = (flags & 0x10) != 0;
    state.sneak = (flags & 0x20) != 0;
    state.sprint = (flags & 0x40) != 0;
    if (inputCallback) inputCallback(*this, state);

  } else if (packetId == packet::play::movePlayerRotServerbound) {
    float yaw = reader.readFloat();
    float pitch = reader.readFloat();
    if (lookCallback) lookCallback(*this, yaw, pitch);

  } else if (packetId == packet::play::movePlayerPosRotServerbound) {
    reader.skip(24); // position
    float yaw = reader.readFloat();
    float pitch = reader.readFloat();
    if (lookCallback) lookCallback(*this, yaw, pitch);

  } else if (packetId == packet::play::setCarriedItemServerbound) {
    int16_t slot = reader.readShort();
    if (hotbarSlotCallback) hotbarSlotCallback(*this, slot);

  } else if (packetId == packet::play::playerActionServerbound) {
    int32_t action = reader.readVarInt();
    if (action == playerActionDropItem || action == playerActionDropAllItems) {
      ensureHotbarItem();
      if (dropItemCallback) dropItemCallback(*this, action == playerActionDropAllItems);
    } else if (action == playerActionSwapItemWithOffhand && swapHandsCallback) {
      swapHandsCallback(*this);
    }

  } else if (packetId == packet::play::playerCommandServerbound) {
    reader.readVarInt(); // entity id
    int32_t action = reader.readVarInt();
    if (action == playerCommandOpenInventory && openInventoryCallback) {
      openInventoryCallback(*this);
    }

  } else if (packetId == packet::play::punchServerbound) {
    if (leftClickCallback) leftClickCallback(*this);

  } else if (packetId == packet::play::useItemServerbound) {
    if (rightClickCallback) rightClickCallback(*this);

  } else if (packetId == packet::play::chatServerbound) {
    std::string message = reader.readString();
    if (chatMessageCallback) chatMessageCallback(*this, message);
  }
}

} // namespace flatland
