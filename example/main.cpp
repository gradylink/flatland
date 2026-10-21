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
  flatland::InputState input;
};

} // namespace

int main() {
  flatland::ServerOptions options;
  options.port = 25565;
  options.motd = "Flatland";

  flatland::Server server(options);

  flatland::Viewport viewport;
  viewport.width = 480;
  viewport.height = 360;
  server.setViewport(viewport);

  bundle::archive assets(bundle_flatland_example_assets_archive());
  flatland::TextureId sprite = server.registerTexture("example", flatland::loadBundledBytes(assets, "textures/example_sprite.png"));

  server.onPlayerJoin([sprite, viewport](flatland::PlayerSession &session) {
    auto state = std::make_shared<SpriteState>();
    state->x = viewport.width / 2.0;
    state->y = viewport.height / 2.0;

    flatland::SpriteId handle = session.createSprite(sprite, state->x, state->y);

    session.onInput([state](flatland::PlayerSession &, const flatland::InputState &input) { state->input = input; });

    constexpr double pixelsPerSecond = 200.0;
    session.onTick([handle, state, viewport](flatland::PlayerSession &playerSession, double elapsedSeconds) {
      double dt = elapsedSeconds - state->lastElapsedSeconds;
      state->lastElapsedSeconds = elapsedSeconds;
      if (dt <= 0.0 || dt > 1.0) return;

      double dx = (state->input.right ? 1.0 : 0.0) - (state->input.left ? 1.0 : 0.0);
      double dy = (state->input.backward ? 1.0 : 0.0) - (state->input.forward ? 1.0 : 0.0);
      state->x = std::clamp(state->x + dx * pixelsPerSecond * dt, 0.0, static_cast<double>(viewport.width));
      state->y = std::clamp(state->y + dy * pixelsPerSecond * dt, 0.0, static_cast<double>(viewport.height));
      playerSession.moveSprite(handle, state->x, state->y);
    });

    session.onLeftClick([](flatland::PlayerSession &) {
      std::printf("left click\n");
      std::fflush(stdout);
    });
    session.onRightClick([](flatland::PlayerSession &) {
      std::printf("right click\n");
      std::fflush(stdout);
    });
    session.onDropItem([](flatland::PlayerSession &, bool dropStack) {
      std::printf("drop item (stack=%s)\n", dropStack ? "true" : "false");
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

  std::printf("Listening on port %u...\n", options.port);
  std::fflush(stdout);

  server.run();
  return 0;
}
