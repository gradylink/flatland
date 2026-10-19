#include "httpServer.hpp"
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace flatland {

namespace {

void serveOneRequest(int clientFd, const std::string &contentType, const std::string &body) {
  char buffer[4096];
  std::string received;
  while (received.find("\r\n\r\n") == std::string::npos) {
    ssize_t bytesRead = ::recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
      ::close(clientFd);
      return;
    }
    received.append(buffer, static_cast<size_t>(bytesRead));
    if (received.size() > 65536) break;
  }

  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: " +
                         contentType +
                         "\r\n"
                         "Content-Length: " +
                         std::to_string(body.size()) +
                         "\r\n"
                         "Connection: close\r\n"
                         "\r\n" +
                         body;

  size_t totalSent = 0;
  while (totalSent < response.size()) {
    ssize_t sent = ::send(clientFd, response.data() + totalSent, response.size() - totalSent, 0);
    if (sent <= 0) break;
    totalSent += static_cast<size_t>(sent);
  }
  ::close(clientFd);
}

} // namespace

HttpServer::HttpServer(uint16_t port, std::string contentType, std::string body) : port(port), contentType(std::move(contentType)), body(std::move(body)) {}

HttpServer::~HttpServer() { stop(); }

void HttpServer::run() {
  listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd < 0) throw std::runtime_error("failed to create HTTP socket");

  int reuse = 1;
  ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (::bind(listenFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
    ::close(listenFd);
    throw std::runtime_error("failed to bind HTTP server to port " + std::to_string(port));
  }

  if (::listen(listenFd, 64) < 0) {
    ::close(listenFd);
    throw std::runtime_error("failed to listen on HTTP socket");
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

    std::thread([clientFd, this]() { serveOneRequest(clientFd, contentType, body); }).detach();
  }
}

void HttpServer::stop() {
  if (!running.exchange(false)) return;
  if (listenFd >= 0) {
    ::shutdown(listenFd, SHUT_RDWR);
    ::close(listenFd);
    listenFd = -1;
  }
}

} // namespace flatland
