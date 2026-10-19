#include "byteBuffer.hpp"
#include <cstring>
#include <stdexcept>

namespace flatland {

void ByteBuffer::writeByte(uint8_t value) { bytes.push_back(value); }

void ByteBuffer::writeBool(bool value) { writeByte(value ? 1 : 0); }

void ByteBuffer::writeShort(int16_t value) { writeUnsignedShort(static_cast<uint16_t>(value)); }

void ByteBuffer::writeUnsignedShort(uint16_t value) {
  writeByte(static_cast<uint8_t>(value >> 8));
  writeByte(static_cast<uint8_t>(value));
}

void ByteBuffer::writeInt(int32_t value) {
  uint32_t unsignedValue = static_cast<uint32_t>(value);
  for (int shift = 24; shift >= 0; shift -= 8) writeByte(static_cast<uint8_t>(unsignedValue >> shift));
}

void ByteBuffer::writeLong(int64_t value) {
  uint64_t unsignedValue = static_cast<uint64_t>(value);
  for (int shift = 56; shift >= 0; shift -= 8) writeByte(static_cast<uint8_t>(unsignedValue >> shift));
}

void ByteBuffer::writeFloat(float value) {
  uint32_t bits;
  std::memcpy(&bits, &value, sizeof(bits));
  writeInt(static_cast<int32_t>(bits));
}

void ByteBuffer::writeDouble(double value) {
  uint64_t bits;
  std::memcpy(&bits, &value, sizeof(bits));
  writeLong(static_cast<int64_t>(bits));
}

void ByteBuffer::writeVarInt(int32_t value) {
  uint32_t unsignedValue = static_cast<uint32_t>(value);
  while (true) {
    if ((unsignedValue & ~0x7Fu) == 0) {
      writeByte(static_cast<uint8_t>(unsignedValue));
      return;
    }
    writeByte(static_cast<uint8_t>((unsignedValue & 0x7F) | 0x80));
    unsignedValue >>= 7;
  }
}

void ByteBuffer::writeVarLong(int64_t value) {
  uint64_t unsignedValue = static_cast<uint64_t>(value);
  while (true) {
    if ((unsignedValue & ~0x7Full) == 0) {
      writeByte(static_cast<uint8_t>(unsignedValue));
      return;
    }
    writeByte(static_cast<uint8_t>((unsignedValue & 0x7F) | 0x80));
    unsignedValue >>= 7;
  }
}

void ByteBuffer::writeString(const std::string &value) {
  writeVarInt(static_cast<int32_t>(value.size()));
  writeBytes(reinterpret_cast<const uint8_t *>(value.data()), value.size());
}

void ByteBuffer::writeUuid(uint64_t high, uint64_t low) {
  writeLong(static_cast<int64_t>(high));
  writeLong(static_cast<int64_t>(low));
}

void ByteBuffer::writeBytes(const uint8_t *data, size_t length) { bytes.insert(bytes.end(), data, data + length); }

void ByteBuffer::writeBytes(const std::vector<uint8_t> &data) { writeBytes(data.data(), data.size()); }

uint8_t ByteReader::readByte() {
  if (cursor >= length) throw std::runtime_error("ByteReader: read past end of buffer");
  return data[cursor++];
}

bool ByteReader::readBool() { return readByte() != 0; }

int16_t ByteReader::readShort() { return static_cast<int16_t>(readUnsignedShort()); }

uint16_t ByteReader::readUnsignedShort() {
  uint16_t value = static_cast<uint16_t>(readByte()) << 8;
  value |= readByte();
  return value;
}

int32_t ByteReader::readInt() {
  uint32_t value = 0;
  for (int i = 0; i < 4; i++) value = (value << 8) | readByte();
  return static_cast<int32_t>(value);
}

int64_t ByteReader::readLong() {
  uint64_t value = 0;
  for (int i = 0; i < 8; i++) value = (value << 8) | readByte();
  return static_cast<int64_t>(value);
}

float ByteReader::readFloat() {
  int32_t bits = readInt();
  float value;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

double ByteReader::readDouble() {
  int64_t bits = readLong();
  double value;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

int32_t ByteReader::readVarInt() {
  uint32_t result = 0;
  for (int position = 0; position < 32; position += 7) {
    uint8_t currentByte = readByte();
    result |= static_cast<uint32_t>(currentByte & 0x7F) << position;
    if ((currentByte & 0x80) == 0) return static_cast<int32_t>(result);
  }
  throw std::runtime_error("VarInt too large");
}

int64_t ByteReader::readVarLong() {
  uint64_t result = 0;
  for (int position = 0; position < 64; position += 7) {
    uint8_t currentByte = readByte();
    result |= static_cast<uint64_t>(currentByte & 0x7F) << position;
    if ((currentByte & 0x80) == 0) return static_cast<int64_t>(result);
  }
  throw std::runtime_error("VarLong too large");
}

std::string ByteReader::readString() {
  int32_t stringLength = readVarInt();
  if (stringLength < 0 || static_cast<size_t>(stringLength) > remaining()) throw std::runtime_error("ByteReader: invalid string length");
  std::string value(reinterpret_cast<const char *>(data + cursor), static_cast<size_t>(stringLength));
  cursor += static_cast<size_t>(stringLength);
  return value;
}

void ByteReader::skip(size_t count) {
  if (count > remaining()) throw std::runtime_error("ByteReader: skip past end of buffer");
  cursor += count;
}

} // namespace flatland
