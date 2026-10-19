#pragma once
#include <array>
#include <cstdint>
#include <string>

namespace flatland {

std::array<uint8_t, 16> md5(const std::string &input);

}
