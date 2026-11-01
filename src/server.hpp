#pragma once
#include "httpServer.hpp"
#include "playerSession.hpp"
#include "soundRegistry.hpp"
#include "textureRegistry.hpp"
#include "viewport.hpp"
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace flatland {

struct ServerOptions {
  uint16_t port = 25565;
  std::string motd = "A Flatland server";
  int32_t maxPlayers = 20;
};

class Server {
public:
  explicit Server(ServerOptions options = {});
  ~Server();

  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void setViewport(Viewport viewport);
  TextureId registerTexture(const std::string &name, std::vector<uint8_t> pngBytes);
  SoundId registerSound(const std::string &name, std::vector<uint8_t> oggBytes);
  void onPlayerJoin(std::function<void(PlayerSession &)> callback);

  void run();
  void stop();

private:
  ServerOptions options;
  Viewport viewport;
  TextureRegistry textures;
  SoundRegistry sounds;
  std::function<void(PlayerSession &)> joinCallback;
  int listenFd = -1;
  std::atomic<bool> running{false};
  std::unique_ptr<HttpServer> resourcePackServer;
};

} // namespace flatland
