#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <string>

namespace flatland {

enum class NbtTagType : uint8_t {
  End = 0,
  Byte = 1,
  Short = 2,
  Int = 3,
  Long = 4,
  Float = 5,
  Double = 6,
  ByteArray = 7,
  String = 8,
  List = 9,
  Compound = 10,
  IntArray = 11,
  LongArray = 12,
};

class NbtWriter {
public:
  explicit NbtWriter(ByteBuffer &buffer) : buffer(buffer) {}

  void beginNetworkRootCompound();
  void endCompound();

  void writeNetworkRootList(NbtTagType elementType, int32_t count);

  void beginCompound(const std::string &name);
  void beginList(const std::string &name, NbtTagType elementType, int32_t count);

  void writeByte(const std::string &name, int8_t value);
  void writeShort(const std::string &name, int16_t value);
  void writeInt(const std::string &name, int32_t value);
  void writeLong(const std::string &name, int64_t value);
  void writeFloat(const std::string &name, float value);
  void writeDouble(const std::string &name, double value);
  void writeString(const std::string &name, const std::string &value);
  void writeAnonymousString(const std::string &value);

private:
  void writeTagHeader(NbtTagType type, const std::string &name);

  ByteBuffer &buffer;
};

} // namespace flatland
