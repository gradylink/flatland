#include "playerSession.hpp"
#include "byteBuffer.hpp"
#include "entities.hpp"
#include "itemStack.hpp"
#include "packetIds.hpp"
#include "packetIo.hpp"
#include "uuid.hpp"

namespace flatland {

PlayerSession::PlayerSession(int socketFd, const Viewport &viewport, const TextureRegistry &textures, int32_t &nextEntityId)
    : socketFd(socketFd), viewport(viewport), textures(textures), nextEntityId(nextEntityId) {}

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

void PlayerSession::onTick(std::function<void(PlayerSession &, double)> callback) { tickCallback = std::move(callback); }

void PlayerSession::runTick(double elapsedSeconds) {
  if (tickCallback) tickCallback(*this, elapsedSeconds);
}

} // namespace flatland
