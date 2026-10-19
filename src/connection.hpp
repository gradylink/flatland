#pragma once
#include "server.hpp"

namespace flatland {

void serveConnection(int socketFd, const ServerOptions &options);

}
