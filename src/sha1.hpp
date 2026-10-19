#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace flatland {

std::array<uint8_t, 20> sha1(const std::string &input);
std::array<uint8_t, 20> sha1(const std::vector<uint8_t> &input);

} // namespace flatland
