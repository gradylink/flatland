#pragma once
#include <cstdint>
#include <string>

namespace flatland {

struct Uuid {
  uint64_t high = 0;
  uint64_t low = 0;

  std::string toString() const;
};

Uuid offlinePlayerUuid(const std::string &username);

Uuid randomUuid();

} // namespace flatland
