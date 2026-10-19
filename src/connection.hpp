#pragma once
#include "server.hpp"
#include <cstdint>
#include <string>

namespace flatland {

struct ResourcePackInfo {
  uint16_t httpPort;
  std::string sha1Hex;
};

void serveConnection(int socketFd, const ServerOptions &options, const ResourcePackInfo &resourcePack);

} // namespace flatland
