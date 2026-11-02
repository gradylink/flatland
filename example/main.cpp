#include "assets.hpp"
#include "bundle.hpp"
#include "bundle_flatland_example_assets.h"
#include "server.hpp"
#include <algorithm>
#include <cstdio>
#include <memory>

namespace {

struct SpriteState {
  double x;
  double y;
  double lastElapsedSeconds = 0.0;
  double nextAmbienceAt = 0.0;
  flatland::InputState input;
};

struct CursorState {
  double x;
  double y;
  bool hasLast = false;
  float lastYaw = 0.0f;
  float lastPitch = 0.0f;
};

void configureMainApp(flatland::App &app, bundle::archive &assets, flatland::Viewport viewport, uint16_t port) {
  app.setViewport(viewport);

  flatland::TextureId orangeSprite = app.registerTexture("example", flatland::loadBundledBytes(assets, "textures/example_sprite.png"));
  flatland::TextureId blueSprite = app.registerTexture("blue", flatland::loadBundledBytes(assets, "textures/blue_sprite.png"));

  flatland::SoundId clickSound = app.registerSound("click", flatland::loadBundledBytes(assets, "sounds/click.ogg"));
  flatland::SoundId ambienceSound = app.registerSound("ambience", flatland::loadBundledBytes(assets, "sounds/ambience.ogg"));

  constexpr double ambienceLoopSeconds = 2.0;

  app.onPlayerJoin([orangeSprite, blueSprite, clickSound, ambienceSound, viewport, ambienceLoopSeconds, port](flatland::PlayerSession &session) {
    auto state = std::make_shared<SpriteState>();
    state->x = viewport.width / 2.0;
    state->y = viewport.height / 2.0;

    flatland::SpriteId handle = session.createSprite(orangeSprite, state->x, state->y);

    session.onInput([state](flatland::PlayerSession &, const flatland::InputState &input) { state->input = input; });

    constexpr double pixelsPerSecond = 200.0;
    session.onTick([handle, state, viewport, ambienceSound, ambienceLoopSeconds](flatland::PlayerSession &playerSession, double elapsedSeconds) {
      double dt = elapsedSeconds - state->lastElapsedSeconds;
      state->lastElapsedSeconds = elapsedSeconds;
      if (dt <= 0.0 || dt > 0.25) return;

      double dx = (state->input.right ? 1.0 : 0.0) - (state->input.left ? 1.0 : 0.0);
      double dy = (state->input.backward ? 1.0 : 0.0) - (state->input.forward ? 1.0 : 0.0);
      state->x = std::clamp(state->x + dx * pixelsPerSecond * dt, 0.0, static_cast<double>(viewport.width));
      state->y = std::clamp(state->y + dy * pixelsPerSecond * dt, 0.0, static_cast<double>(viewport.height));
      playerSession.moveSprite(handle, state->x, state->y);

      if (elapsedSeconds >= state->nextAmbienceAt) {
        playerSession.playSound(ambienceSound, 0.5f);
        state->nextAmbienceAt = elapsedSeconds + ambienceLoopSeconds;
      }
    });

    auto cursor = std::make_shared<CursorState>();
    cursor->x = viewport.width / 2.0;
    cursor->y = viewport.height / 2.0;
    flatland::SpriteId cursorHandle = session.createSprite(blueSprite, cursor->x, cursor->y);

    constexpr double pixelsPerDegree = 4.0;
    session.onLook([cursorHandle, cursor, viewport](flatland::PlayerSession &playerSession, float yaw, float pitch) {
      if (cursor->hasLast) {
        float yawDelta = yaw - cursor->lastYaw;
        while (yawDelta > 180.0f) yawDelta -= 360.0f;
        while (yawDelta < -180.0f) yawDelta += 360.0f;
        float pitchDelta = pitch - cursor->lastPitch;

        cursor->x = std::clamp(cursor->x + yawDelta * pixelsPerDegree, 0.0, static_cast<double>(viewport.width));
        cursor->y = std::clamp(cursor->y + pitchDelta * pixelsPerDegree, 0.0, static_cast<double>(viewport.height));
        playerSession.moveSprite(cursorHandle, cursor->x, cursor->y);
      }
      cursor->lastYaw = yaw;
      cursor->lastPitch = pitch;
      cursor->hasLast = true;
    });

    session.onLeftClick([clickSound](flatland::PlayerSession &playerSession) {
      playerSession.playSound(clickSound);
      std::printf("left click\n");
      std::fflush(stdout);
    });
    session.onRightClick([clickSound](flatland::PlayerSession &playerSession) {
      playerSession.playSound(clickSound);
      std::printf("right click\n");
      std::fflush(stdout);
    });
    session.onDropItem([port](flatland::PlayerSession &playerSession, bool dropStack) {
      std::printf("drop item (stack=%s) - transferring to 127.0.0.1\n", dropStack ? "true" : "false");
      std::fflush(stdout);
      playerSession.transferTo("swizzle-laptop", port);
    });
    session.onChatMessage([](flatland::PlayerSession &playerSession, const std::string &message) {
      playerSession.sendChatMessage("you said: " + message);
      std::printf("chat: %s\n", message.c_str());
      std::fflush(stdout);
    });
    session.onSwapHands([](flatland::PlayerSession &) {
      std::printf("swap hands\n");
      std::fflush(stdout);
    });
    session.onOpenInventory([](flatland::PlayerSession &) {
      std::printf("open inventory\n");
      std::fflush(stdout);
    });
    session.onHotbarSlotChanged([](flatland::PlayerSession &, int32_t slot) {
      std::printf("hotbar slot -> %d\n", slot);
      std::fflush(stdout);
    });
  });
}

void configureLoopbackApp(flatland::App &app, bundle::archive &assets, flatland::Viewport viewport) {
  app.setViewport(viewport);
  flatland::TextureId loopbackSprite = app.registerTexture("blue", flatland::loadBundledBytes(assets, "textures/blue_sprite.png"));
  app.onPlayerJoin([loopbackSprite, viewport](flatland::PlayerSession &session) { session.createSprite(loopbackSprite, viewport.width / 2.0, viewport.height / 2.0); });
}

} // namespace

int main() {
  flatland::ServerOptions options;
  options.port = 25565;
  options.motd = "Flatland Example";

  flatland::Server server(options);

  flatland::Viewport viewport;
  viewport.width = 480;
  viewport.height = 360;

  bundle::archive assets(bundle_flatland_example_assets_archive());

  server.onResolveApp([&assets, viewport, port = options.port](const std::string &hostname, flatland::App &app) {
    std::printf("%s\n\n", hostname.c_str());
    std::fflush(stdout);

    if (hostname == "swizzle-laptop") {
      configureLoopbackApp(app, assets, viewport);
      return;
    }

    configureMainApp(app, assets, viewport, port);
  });

  std::printf("Listening on port %u...\n", options.port);
  std::fflush(stdout);

  server.run();
  return 0;
}
