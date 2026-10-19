#pragma once
#include <atomic>
#include <cstdint>
#include <string>

namespace flatland {

class HttpServer {
public:
  HttpServer(uint16_t port, std::string contentType, std::string body);
  ~HttpServer();

  HttpServer(const HttpServer &) = delete;
  HttpServer &operator=(const HttpServer &) = delete;

  void run();
  void stop();

private:
  uint16_t port;
  std::string contentType;
  std::string body;
  int listenFd = -1;
  std::atomic<bool> running{false};
};

} // namespace flatland
