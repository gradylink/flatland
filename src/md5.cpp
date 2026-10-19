#include "md5.hpp"
#include <cstring>
#include <vector>

namespace flatland {

namespace {

constexpr uint32_t sineTable[64] = {
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122,
  0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6,
  0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60,
  0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

constexpr uint32_t shiftTable[64] = {
  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
};

uint32_t rotateLeft(uint32_t value, uint32_t amount) { return (value << amount) | (value >> (32 - amount)); }

} // namespace

std::array<uint8_t, 16> md5(const std::string &input) {
  uint32_t a0 = 0x67452301;
  uint32_t b0 = 0xefcdab89;
  uint32_t c0 = 0x98badcfe;
  uint32_t d0 = 0x10325476;

  std::vector<uint8_t> message(input.begin(), input.end());
  uint64_t originalBitLength = static_cast<uint64_t>(message.size()) * 8;

  message.push_back(0x80);
  while (message.size() % 64 != 56) message.push_back(0x00);
  for (int i = 0; i < 8; i++) message.push_back(static_cast<uint8_t>(originalBitLength >> (8 * i)));

  for (size_t chunkStart = 0; chunkStart < message.size(); chunkStart += 64) {
    uint32_t words[16];
    for (int i = 0; i < 16; i++) {
      words[i] = static_cast<uint32_t>(message[chunkStart + i * 4]) | (static_cast<uint32_t>(message[chunkStart + i * 4 + 1]) << 8) |
                 (static_cast<uint32_t>(message[chunkStart + i * 4 + 2]) << 16) | (static_cast<uint32_t>(message[chunkStart + i * 4 + 3]) << 24);
    }

    uint32_t a = a0, b = b0, c = c0, d = d0;
    for (uint32_t i = 0; i < 64; i++) {
      uint32_t f, g;
      if (i < 16) {
        f = (b & c) | (~b & d);
        g = i;
      } else if (i < 32) {
        f = (d & b) | (~d & c);
        g = (5 * i + 1) % 16;
      } else if (i < 48) {
        f = b ^ c ^ d;
        g = (3 * i + 5) % 16;
      } else {
        f = c ^ (b | ~d);
        g = (7 * i) % 16;
      }

      f = f + a + sineTable[i] + words[g];
      a = d;
      d = c;
      c = b;
      b = b + rotateLeft(f, shiftTable[i]);
    }

    a0 += a;
    b0 += b;
    c0 += c;
    d0 += d;
  }

  std::array<uint8_t, 16> digest;
  uint32_t state[4] = {a0, b0, c0, d0};
  for (int i = 0; i < 4; i++) {
    digest[i * 4 + 0] = static_cast<uint8_t>(state[i]);
    digest[i * 4 + 1] = static_cast<uint8_t>(state[i] >> 8);
    digest[i * 4 + 2] = static_cast<uint8_t>(state[i] >> 16);
    digest[i * 4 + 3] = static_cast<uint8_t>(state[i] >> 24);
  }
  return digest;
}

} // namespace flatland
