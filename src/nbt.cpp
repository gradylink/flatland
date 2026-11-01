#include "nbt.hpp"

namespace flatland {

void NbtWriter::writeTagHeader(NbtTagType type, const std::string &name) {
  buffer.writeByte(static_cast<uint8_t>(type));
  buffer.writeUnsignedShort(static_cast<uint16_t>(name.size()));
  buffer.writeBytes(reinterpret_cast<const uint8_t *>(name.data()), name.size());
}

void NbtWriter::beginNetworkRootCompound() { buffer.writeByte(static_cast<uint8_t>(NbtTagType::Compound)); }

void NbtWriter::endCompound() { buffer.writeByte(static_cast<uint8_t>(NbtTagType::End)); }

void NbtWriter::writeNetworkRootList(NbtTagType elementType, int32_t count) {
  buffer.writeByte(static_cast<uint8_t>(NbtTagType::List));
  buffer.writeByte(static_cast<uint8_t>(elementType));
  buffer.writeInt(count);
}

void NbtWriter::beginCompound(const std::string &name) { writeTagHeader(NbtTagType::Compound, name); }

void NbtWriter::beginList(const std::string &name, NbtTagType elementType, int32_t count) {
  writeTagHeader(NbtTagType::List, name);
  buffer.writeByte(static_cast<uint8_t>(elementType));
  buffer.writeInt(count);
}

void NbtWriter::writeByte(const std::string &name, int8_t value) {
  writeTagHeader(NbtTagType::Byte, name);
  buffer.writeByte(static_cast<uint8_t>(value));
}

void NbtWriter::writeShort(const std::string &name, int16_t value) {
  writeTagHeader(NbtTagType::Short, name);
  buffer.writeShort(value);
}

void NbtWriter::writeInt(const std::string &name, int32_t value) {
  writeTagHeader(NbtTagType::Int, name);
  buffer.writeInt(value);
}

void NbtWriter::writeLong(const std::string &name, int64_t value) {
  writeTagHeader(NbtTagType::Long, name);
  buffer.writeLong(value);
}

void NbtWriter::writeFloat(const std::string &name, float value) {
  writeTagHeader(NbtTagType::Float, name);
  buffer.writeFloat(value);
}

void NbtWriter::writeDouble(const std::string &name, double value) {
  writeTagHeader(NbtTagType::Double, name);
  buffer.writeDouble(value);
}

void NbtWriter::writeString(const std::string &name, const std::string &value) {
  writeTagHeader(NbtTagType::String, name);
  buffer.writeUnsignedShort(static_cast<uint16_t>(value.size()));
  buffer.writeBytes(reinterpret_cast<const uint8_t *>(value.data()), value.size());
}

void NbtWriter::writeAnonymousString(const std::string &value) {
  buffer.writeByte(static_cast<uint8_t>(NbtTagType::String));
  buffer.writeUnsignedShort(static_cast<uint16_t>(value.size()));
  buffer.writeBytes(reinterpret_cast<const uint8_t *>(value.data()), value.size());
}

} // namespace flatland
