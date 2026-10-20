#pragma once
#include "zipWriter.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

using TextureId = int32_t;

class TextureRegistry {
public:
  TextureId registerTexture(const std::string &name, std::vector<uint8_t> pngBytes);

  std::string itemModelId(TextureId texture) const;

  void addResourcePackFiles(ZipWriter &zip) const;

private:
  struct Entry {
    std::string name;
    std::vector<uint8_t> pngBytes;
  };
  std::vector<Entry> entries;
};

} // namespace flatland
