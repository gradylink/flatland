#pragma once
#include "byteBuffer.hpp"
#include <string>
#include <utility>
#include <vector>

namespace flatland {

std::vector<std::pair<std::string, ByteBuffer>> buildDecoratedPotPatternEntries();

} // namespace flatland
