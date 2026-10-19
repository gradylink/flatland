#pragma once
#include "httpServer.hpp"
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

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

  void run();
  void stop();

private:
  ServerOptions options;
  int listenFd = -1;
  std::atomic<bool> running{false};
  std::unique_ptr<HttpServer> resourcePackServer;
};

} // namespace flatland
