#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

class ByteBuffer {
public:
  void writeByte(uint8_t value);
  void writeBool(bool value);
  void writeShort(int16_t value);
  void writeUnsignedShort(uint16_t value);
  void writeInt(int32_t value);
  void writeLong(int64_t value);
  void writeFloat(float value);
  void writeDouble(double value);
  void writeVarInt(int32_t value);
  void writeVarLong(int64_t value);
  void writeString(const std::string &value);
  void writeUuid(uint64_t high, uint64_t low);
  void writeBytes(const uint8_t *data, size_t length);
  void writeBytes(const std::vector<uint8_t> &data);

  const std::vector<uint8_t> &data() const { return bytes; }
  size_t size() const { return bytes.size(); }

private:
  std::vector<uint8_t> bytes;
};

class ByteReader {
public:
  ByteReader(const uint8_t *data, size_t length) : data(data), length(length) {}

  uint8_t readByte();
  bool readBool();
  int16_t readShort();
  uint16_t readUnsignedShort();
  int32_t readInt();
  int64_t readLong();
  float readFloat();
  double readDouble();
  int32_t readVarInt();
  int64_t readVarLong();
  std::string readString();
  void skip(size_t count);

  size_t remaining() const { return length - cursor; }

private:
  const uint8_t *data;
  size_t length;
  size_t cursor = 0;
};

} // namespace flatland
