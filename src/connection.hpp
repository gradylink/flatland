#pragma once
#include "server.hpp"
#include <functional>
#include <string>

namespace flatland {

void serveConnection(int socketFd, const ServerOptions &options, const std::function<HostResolution(const std::string &hostname)> &resolveHost);

} // namespace flatland
