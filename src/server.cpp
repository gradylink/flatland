#include "server.hpp"
#include "connection.hpp"
#include "resourcePack.hpp"
#include "sha1.hpp"
#include <cstdio>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace flatland {

namespace {

std::string toHexString(const std::array<uint8_t, 20> &digest) {
  std::string hex;
  hex.reserve(40);
  char byteChars[3];
  for (uint8_t byte : digest) {
    std::snprintf(byteChars, sizeof(byteChars), "%02x", byte);
    hex += byteChars;
  }
  return hex;
}

} // namespace

Server::Server(ServerOptions options) : options(std::move(options)), nextResourcePackPort(static_cast<uint16_t>(this->options.port + 1)) {}

Server::~Server() { stop(); }

void Server::onResolveApp(std::function<void(const std::string &, App &)> newResolver) { resolver = std::move(newResolver); }

HostResolution Server::resolveHost(const std::string &hostname) {
  std::lock_guard<std::mutex> lock(resolveMutex);

  auto it = resolvedHosts.find(hostname);
  if (it == resolvedHosts.end()) {
    it = resolvedHosts.try_emplace(hostname).first;
    ResolvedHost &resolved = it->second;
    if (resolver) resolver(hostname, resolved.app);

    if (!resolved.app.redirectTarget()) {
      std::vector<uint8_t> resourcePackBytes = buildResourcePackZip(resolved.app.viewport(), resolved.app.textures(), resolved.app.sounds());
      std::string resourcePackSha1 = toHexString(sha1(resourcePackBytes));
      uint16_t resourcePackPort = nextResourcePackPort++;

      resolved.resourcePackServer = std::make_unique<HttpServer>(resourcePackPort, "application/zip", std::string(resourcePackBytes.begin(), resourcePackBytes.end()));
      HttpServer *resourcePackServerPtr = resolved.resourcePackServer.get();
      std::thread([resourcePackServerPtr]() { resourcePackServerPtr->run(); }).detach();

      resolved.resourcePack = ResourcePackInfo{resourcePackPort, resourcePackSha1};
    }
  }

  const ResolvedHost &resolved = it->second;
  return HostResolution{&resolved.app, resolved.app.redirectTarget() ? nullptr : &resolved.resourcePack};
}

void Server::run() {
  listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd < 0) throw std::runtime_error("failed to create socket");

  int reuse = 1;
  ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(options.port);

  if (::bind(listenFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
    ::close(listenFd);
    throw std::runtime_error("failed to bind to port " + std::to_string(options.port));
  }

  if (::listen(listenFd, 64) < 0) {
    ::close(listenFd);
    throw std::runtime_error("failed to listen on socket");
  }

  running = true;
  while (running) {
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientFd = ::accept(listenFd, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
    if (clientFd < 0) {
      if (running) continue;
      break;
    }

    int noDelay = 1;
    ::setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));

    ServerOptions connectionOptions = options;
    std::thread([clientFd, connectionOptions, this]() {
      try {
        serveConnection(clientFd, connectionOptions, [this](const std::string &hostname) { return resolveHost(hostname); });
      } catch (const std::exception &error) {
        std::fprintf(stderr, "flatland: connection on fd %d terminated by exception: %s\n", clientFd, error.what());
        std::fflush(stderr);
      } catch (...) {
        std::fprintf(stderr, "flatland: connection on fd %d terminated by an unknown exception\n", clientFd);
        std::fflush(stderr);
      }
      ::close(clientFd);
    }).detach();
  }
}

void Server::stop() {
  if (!running.exchange(false)) return;
  if (listenFd >= 0) {
    ::shutdown(listenFd, SHUT_RDWR);
    ::close(listenFd);
    listenFd = -1;
  }

  std::lock_guard<std::mutex> lock(resolveMutex);
  for (auto &[hostname, resolved] : resolvedHosts) {
    if (resolved.resourcePackServer) resolved.resourcePackServer->stop();
  }
}

} // namespace flatland
