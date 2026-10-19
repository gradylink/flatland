#include "zipWriter.hpp"
#include <cstring>
#include <miniz.h>
#include <stdexcept>

namespace flatland {

void ZipWriter::addFile(const std::string &path, const std::string &contents) { entries.push_back({path, contents}); }

std::vector<uint8_t> ZipWriter::finish() const {
  mz_zip_archive zip;
  std::memset(&zip, 0, sizeof(zip));

  if (!mz_zip_writer_init_heap(&zip, 0, 0)) throw std::runtime_error("failed to initialize zip writer");

  for (const Entry &entry : entries) {
    if (!mz_zip_writer_add_mem(&zip, entry.path.c_str(), entry.contents.data(), entry.contents.size(), MZ_DEFAULT_COMPRESSION)) {
      mz_zip_writer_end(&zip);
      throw std::runtime_error("failed to add " + entry.path + " to zip");
    }
  }

  void *finalBuffer = nullptr;
  size_t finalSize = 0;
  if (!mz_zip_writer_finalize_heap_archive(&zip, &finalBuffer, &finalSize)) {
    mz_zip_writer_end(&zip);
    throw std::runtime_error("failed to finalize zip archive");
  }

  std::vector<uint8_t> result(static_cast<const uint8_t *>(finalBuffer), static_cast<const uint8_t *>(finalBuffer) + finalSize);
  mz_free(finalBuffer);
  mz_zip_writer_end(&zip);
  return result;
}

} // namespace flatland
