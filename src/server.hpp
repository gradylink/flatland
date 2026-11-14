#pragma once
#include "app.hpp"
#include "httpServer.hpp"
#include "resourcePack.hpp"
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace flatland {

struct ServerOptions {
  uint16_t port = 25565;
  std::string motd = "A Flatland server";
  int32_t maxPlayers = 20;
};

struct HostResolution {
  const App *app;
  const ResourcePackInfo *resourcePack;
};

class Server {
public:
  explicit Server(ServerOptions options = {});
  ~Server();

  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void onResolveApp(std::function<void(const std::string &, App &)> resolver);

  void run();
  void stop();

private:
  struct ResolvedHost {
    App app;
    std::unique_ptr<HttpServer> resourcePackServer;
    ResourcePackInfo resourcePack{};
  };

  HostResolution resolveHost(const std::string &hostname);

  ServerOptions options;
  uint16_t nextResourcePackPort;
  std::function<void(const std::string &, App &)> resolver;
  std::mutex resolveMutex;
  std::unordered_map<std::string, ResolvedHost> resolvedHosts;
  int listenFd = -1;
  std::atomic<bool> running{false};
};

} // namespace flatland
