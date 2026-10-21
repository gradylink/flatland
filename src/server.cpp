#include "server.hpp"
#include "connection.hpp"
#include "resourcePack.hpp"
#include "sha1.hpp"
#include <cstdio>
#include <netinet/in.h>
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

Server::Server(ServerOptions options) : options(std::move(options)) {}

Server::~Server() { stop(); }

void Server::setViewport(Viewport newViewport) { viewport = newViewport; }

TextureId Server::registerTexture(const std::string &name, std::vector<uint8_t> pngBytes) { return textures.registerTexture(name, std::move(pngBytes)); }

void Server::onPlayerJoin(std::function<void(PlayerSession &)> callback) { joinCallback = std::move(callback); }

void Server::run() {
  std::vector<uint8_t> resourcePackBytes = buildResourcePackZip(viewport, textures);
  std::string resourcePackSha1 = toHexString(sha1(resourcePackBytes));
  uint16_t resourcePackPort = static_cast<uint16_t>(options.port + 1);

  resourcePackServer = std::make_unique<HttpServer>(resourcePackPort, "application/zip", std::string(resourcePackBytes.begin(), resourcePackBytes.end()));
  std::thread([this]() { resourcePackServer->run(); }).detach();

  ResourcePackInfo resourcePackInfo{resourcePackPort, resourcePackSha1};

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

    ServerOptions connectionOptions = options;
    std::thread([clientFd, connectionOptions, resourcePackInfo, this]() {
      try {
        serveConnection(clientFd, connectionOptions, resourcePackInfo, viewport, textures, joinCallback);
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
  if (resourcePackServer) resourcePackServer->stop();
}

} // namespace flatland
