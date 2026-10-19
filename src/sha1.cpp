#include "sha1.hpp"
#include <vector>

namespace flatland {

namespace {

uint32_t rotateLeft(uint32_t value, uint32_t amount) { return (value << amount) | (value >> (32 - amount)); }

std::array<uint8_t, 20> sha1Bytes(const uint8_t *data, size_t length) {
  uint32_t h0 = 0x67452301;
  uint32_t h1 = 0xEFCDAB89;
  uint32_t h2 = 0x98BADCFE;
  uint32_t h3 = 0x10325476;
  uint32_t h4 = 0xC3D2E1F0;

  std::vector<uint8_t> message(data, data + length);
  uint64_t originalBitLength = static_cast<uint64_t>(message.size()) * 8;

  message.push_back(0x80);
  while (message.size() % 64 != 56) message.push_back(0x00);
  for (int i = 7; i >= 0; i--) message.push_back(static_cast<uint8_t>(originalBitLength >> (8 * i)));

  for (size_t chunkStart = 0; chunkStart < message.size(); chunkStart += 64) {
    uint32_t words[80];
    for (int i = 0; i < 16; i++) {
      words[i] = (static_cast<uint32_t>(message[chunkStart + i * 4]) << 24) | (static_cast<uint32_t>(message[chunkStart + i * 4 + 1]) << 16) |
                 (static_cast<uint32_t>(message[chunkStart + i * 4 + 2]) << 8) | static_cast<uint32_t>(message[chunkStart + i * 4 + 3]);
    }
    for (int i = 16; i < 80; i++) words[i] = rotateLeft(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);

    uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
    for (int i = 0; i < 80; i++) {
      uint32_t f, k;
      if (i < 20) {
        f = (b & c) | (~b & d);
        k = 0x5A827999;
      } else if (i < 40) {
        f = b ^ c ^ d;
        k = 0x6ED9EBA1;
      } else if (i < 60) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8F1BBCDC;
      } else {
        f = b ^ c ^ d;
        k = 0xCA62C1D6;
      }

      uint32_t temp = rotateLeft(a, 5) + f + e + k + words[i];
      e = d;
      d = c;
      c = rotateLeft(b, 30);
      b = a;
      a = temp;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
  }

  std::array<uint8_t, 20> digest;
  uint32_t state[5] = {h0, h1, h2, h3, h4};
  for (int i = 0; i < 5; i++) {
    digest[i * 4 + 0] = static_cast<uint8_t>(state[i] >> 24);
    digest[i * 4 + 1] = static_cast<uint8_t>(state[i] >> 16);
    digest[i * 4 + 2] = static_cast<uint8_t>(state[i] >> 8);
    digest[i * 4 + 3] = static_cast<uint8_t>(state[i]);
  }
  return digest;
}

} // namespace

std::array<uint8_t, 20> sha1(const std::string &input) { return sha1Bytes(reinterpret_cast<const uint8_t *>(input.data()), input.size()); }

std::array<uint8_t, 20> sha1(const std::vector<uint8_t> &input) { return sha1Bytes(input.data(), input.size()); }

} // namespace flatland
