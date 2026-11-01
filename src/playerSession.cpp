#include "playerSession.hpp"
#include "byteBuffer.hpp"
#include "entities.hpp"
#include "itemStack.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "sound.hpp"
#include "uuid.hpp"

namespace flatland {

namespace {

constexpr int32_t playerActionDropAllItems = 4;
constexpr int32_t playerActionDropItem = 5;
constexpr int32_t playerActionSwapItemWithOffhand = 7;
constexpr int32_t playerCommandOpenInventory = 5;

} // namespace

PlayerSession::PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, const SoundRegistry &sounds, int32_t &nextEntityId)
    : socketFd(socketFd), viewport(viewport), textures(textures), sounds(sounds), nextEntityId(nextEntityId) {}

SpriteId PlayerSession::createSprite(TextureId texture, double x, double y) {
  int32_t entityId = nextEntityId++;
  double worldX = viewportPixelToWorldX(viewport, x);
  double worldY = viewportPixelToWorldY(viewport, y);

  writePacket(socketFd, packet::play::addEntityClientbound, buildAddEntityPacket(entityId, randomUuid(), itemDisplayEntityTypeId, worldX, worldY, viewportPlaneZ));
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildItemDisplayItemPacket(entityId, buildSpriteItemStack(textures.itemModelId(texture))));
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildDisplayScalePacket(entityId, 1.0f, 1.0f, 0.0f));
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildDisplayBrightnessOverridePacket(entityId, fullBrightBrightnessOverride));
  writePacket(socketFd, packet::play::setEntityDataClientbound, buildDisplayDimensionsPacket(entityId, 1.0f, 1.0f));

  SpriteId sprite = nextSpriteId++;
  spriteEntityIds[sprite] = entityId;
  return sprite;
}

void PlayerSession::moveSprite(SpriteId sprite, double x, double y) {
  auto it = spriteEntityIds.find(sprite);
  if (it == spriteEntityIds.end()) return;

  double worldX = viewportPixelToWorldX(viewport, x);
  double worldY = viewportPixelToWorldY(viewport, y);
  writePacket(socketFd, packet::play::teleportEntityClientbound, buildTeleportEntityPacket(it->second, worldX, worldY, viewportPlaneZ, 0.0f, 0.0f, false));
}

void PlayerSession::removeSprite(SpriteId sprite) {
  auto it = spriteEntityIds.find(sprite);
  if (it == spriteEntityIds.end()) return;

  ByteBuffer buffer;
  buffer.writeVarInt(1);
  buffer.writeVarInt(it->second);
  writePacket(socketFd, packet::play::removeEntitiesClientbound, buffer);

  spriteEntityIds.erase(it);
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

void PlayerSession::onTick(std::function<void(PlayerSession &, double)> callback) { tickCallback = std::move(callback); }
void PlayerSession::onInput(std::function<void(PlayerSession &, const InputState &)> callback) { inputCallback = std::move(callback); }
void PlayerSession::onLook(std::function<void(PlayerSession &, float, float)> callback) { lookCallback = std::move(callback); }
void PlayerSession::onHotbarSlotChanged(std::function<void(PlayerSession &, int32_t)> callback) { hotbarSlotCallback = std::move(callback); }
void PlayerSession::onDropItem(std::function<void(PlayerSession &, bool)> callback) { dropItemCallback = std::move(callback); }
void PlayerSession::onSwapHands(std::function<void(PlayerSession &)> callback) { swapHandsCallback = std::move(callback); }
void PlayerSession::onOpenInventory(std::function<void(PlayerSession &)> callback) { openInventoryCallback = std::move(callback); }
void PlayerSession::onLeftClick(std::function<void(PlayerSession &)> callback) { leftClickCallback = std::move(callback); }
void PlayerSession::onRightClick(std::function<void(PlayerSession &)> callback) { rightClickCallback = std::move(callback); }

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
    if (action == playerActionDropItem && dropItemCallback) {
      dropItemCallback(*this, false);
    } else if (action == playerActionDropAllItems && dropItemCallback) {
      dropItemCallback(*this, true);
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
  }
}

} // namespace flatland
