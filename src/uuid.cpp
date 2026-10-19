#include "uuid.hpp"
#include "md5.hpp"
#include <array>
#include <cstdio>
#include <random>

namespace flatland {

std::string Uuid::toString() const {
  char buffer[37];
  std::snprintf(
    buffer,
    sizeof(buffer),
    "%08x-%04x-%04x-%04x-%012llx",
    static_cast<uint32_t>(high >> 32),
    static_cast<uint32_t>((high >> 16) & 0xFFFF),
    static_cast<uint32_t>(high & 0xFFFF),
    static_cast<uint32_t>(low >> 48),
    static_cast<unsigned long long>(low & 0xFFFFFFFFFFFFULL)
  );
  return std::string(buffer);
}

Uuid offlinePlayerUuid(const std::string &username) {
  std::array<uint8_t, 16> digest = md5("OfflinePlayer:" + username);

  digest[6] = static_cast<uint8_t>((digest[6] & 0x0F) | 0x30);
  digest[8] = static_cast<uint8_t>((digest[8] & 0x3F) | 0x80);

  Uuid uuid;
  uuid.high = 0;
  uuid.low = 0;
  for (int i = 0; i < 8; i++) uuid.high = (uuid.high << 8) | digest[i];
  for (int i = 8; i < 16; i++) uuid.low = (uuid.low << 8) | digest[i];
  return uuid;
}

Uuid randomUuid() {
  static std::random_device randomDevice;
  static std::mt19937_64 generator(randomDevice());
  std::uniform_int_distribution<uint64_t> distribution;

  Uuid uuid;
  uuid.high = distribution(generator);
  uuid.low = distribution(generator);

  uuid.high = (uuid.high & ~0xF000ULL) | 0x4000ULL;
  uuid.low = (uuid.low & ~(0xC000000000000000ULL)) | 0x8000000000000000ULL;

  return uuid;
}

} // namespace flatland
