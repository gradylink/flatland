#pragma once
#include "byteBuffer.hpp"
#include <cstdint>
#include <string>

namespace flatland {

ByteBuffer buildTransferPacket(const std::string &host, uint16_t port);

} // namespace flatland
