#include "server.hpp"
#include "connection.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace flatland {

Server::Server(ServerOptions options) : options(std::move(options)) {}

Server::~Server() { stop(); }

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

    ServerOptions connectionOptions = options;
    std::thread([clientFd, connectionOptions]() {
      try {
        serveConnection(clientFd, connectionOptions);
      } catch (...) {
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
}

} // namespace flatland
