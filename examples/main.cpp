#include "assets.hpp"
#include "bundle.hpp"
#include "bundle_flatland_example_assets.h"
#include "server.hpp"
#include <cmath>
#include <cstdio>

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
    flatland::SpriteId handle = session.createSprite(sprite, viewport.width / 2.0, viewport.height / 2.0);
    session.onTick([handle, viewport](flatland::PlayerSession &playerSession, double elapsedSeconds) {
      double x = viewport.width / 2.0 + (viewport.width / 4.0) * std::sin(elapsedSeconds);
      playerSession.moveSprite(handle, x, viewport.height / 2.0);
    });
  });

  std::printf("Listening on port %u...\n", options.port);
  std::fflush(stdout);

  server.run();
  return 0;
}
