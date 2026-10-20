#include "assets.hpp"
#include <cstring>
#include <stdexcept>

namespace flatland {

bundle::entry findBundledAsset(const bundle::archive &archive, std::string_view path) {
  std::optional<bundle::entry> entry = archive.find(path);
  if (!entry) {
    throw std::runtime_error("flatland: missing bundled asset \"" + std::string(path) + "\"");
  }
  return *entry;
}

std::vector<uint8_t> loadBundledBytes(const bundle::archive &archive, std::string_view path) {
  bundle::entry entry = findBundledAsset(archive, path);
  std::vector<std::byte> bytes = archive.load(entry);
  std::vector<uint8_t> result(bytes.size());
  std::memcpy(result.data(), bytes.data(), bytes.size());
  return result;
}

std::string loadBundledText(const bundle::archive &archive, std::string_view path) {
  bundle::entry entry = findBundledAsset(archive, path);
  return archive.load_text(entry);
}

} // namespace flatland
