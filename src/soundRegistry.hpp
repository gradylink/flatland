#pragma once
#include "zipWriter.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

using SoundId = int32_t;

class SoundRegistry {
public:
  SoundId registerSound(const std::string &name, std::vector<uint8_t> oggBytes);

  std::string soundEventName(SoundId sound) const;

  void addResourcePackFiles(ZipWriter &zip) const;

private:
  struct Entry {
    std::string name;
    std::vector<uint8_t> oggBytes;
  };
  std::vector<Entry> entries;
};

} // namespace flatland
