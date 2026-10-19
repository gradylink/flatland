#include "server.hpp"
#include <cstdio>

int main() {
  flatland::ServerOptions options;
  options.port = 25565;
  options.motd = "Flatland";

  std::printf("Listening on port %u...\n", options.port);
  std::fflush(stdout);

  flatland::Server server(options);
  server.run();
  return 0;
}
