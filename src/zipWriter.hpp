#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

class ZipWriter {
public:
  void addFile(const std::string &path, const std::string &contents);
  std::vector<uint8_t> finish() const;

private:
  struct Entry {
    std::string path;
    std::string contents;
  };
  std::vector<Entry> entries;
};

} // namespace flatland
