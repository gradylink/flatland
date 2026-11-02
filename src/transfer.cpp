#include "transfer.hpp"

namespace flatland {

ByteBuffer buildTransferPacket(const std::string &host, uint16_t port) {
  ByteBuffer buffer;
  buffer.writeString(host);
  buffer.writeVarInt(port);
  return buffer;
}

} // namespace flatland
