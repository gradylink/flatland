#pragma once
#include "bundle.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace flatland {

bundle::entry findBundledAsset(const bundle::archive &archive, std::string_view path);

std::vector<uint8_t> loadBundledBytes(const bundle::archive &archive, std::string_view path);

std::string loadBundledText(const bundle::archive &archive, std::string_view path);

} // namespace flatland
