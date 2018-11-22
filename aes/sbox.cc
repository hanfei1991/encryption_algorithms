//
// Created by Gin Liu on 2018/11/20.
//

#include "sbox.h"
#include "../catch.hpp"
#include "matrix.h"
#include <cstdint>
#include <vector>

std::vector<uint8_t> m = {
    1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
};
std::vector<uint8_t> m2 = {1, 1, 0, 0, 0, 1, 1, 0};

std::vector<uint8_t> m3 = {0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0,
                           0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0,
                           0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
                           1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0};

std::vector<uint8_t> m4 = {1, 0, 1, 0, 0, 0, 0, 0};

using MT = Matrix<8, 8, uint8_t>;
using MT2 = Matrix<8, 1, uint8_t>;
MT x, x2;
MT2 c, c2;

int g_nouse = ([]() -> int {
  memcpy(x.data, m.data(), MT::byte_count);
  memcpy(c.data, m2.data(), MT2::byte_count);
  memcpy(x2.data, m3.data(), MT::byte_count);
  memcpy(c2.data, m4.data(), MT2::byte_count);
  return 0;
})();

uint8_t GetInverse(uint8_t x) {
  int tx = x;
  for (int i = 0; i < 253; ++i) {
    x = gmult(x, tx);
  }
  return x;
}

uint8_t SBoxTr(uint8_t n) {
  n = GetInverse(n);
  std::vector<uint8_t> tt;
  tt.push_back((uint8_t)((n & 0x1)));
  tt.push_back((uint8_t)((n & 0x2) >> 1));
  tt.push_back((uint8_t)((n & 0x4) >> 2));
  tt.push_back((uint8_t)((n & 0x8) >> 3));
  tt.push_back((uint8_t)((n & 0x10) >> 4));
  tt.push_back((uint8_t)((n & 0x20) >> 5));
  tt.push_back((uint8_t)((n & 0x40) >> 6));
  tt.push_back((uint8_t)((n & 0x80) >> 7));
  //  std::reverse(tt.begin(), tt.end());
  Matrix<8, 1, uint8_t> a;
  memcpy(a.data, tt.data(), decltype(a)::byte_count);
  auto y = Mul<8, 8, 1, uint8_t, AddOp, MulOp>(x, a);
  auto r = Add<8, 1, uint8_t, AddOp>(y, c);

  uint8_t kk = r.data[0][0] << 0 | r.data[1][0] << 1 | r.data[2][0] << 2 |
               r.data[3][0] << 3 | r.data[4][0] << 4 | r.data[5][0] << 5 |
               r.data[6][0] << 6 | r.data[7][0] << 7;
  return kk;
}
uint8_t SBoxInverseTr(uint8_t n) {
  std::vector<uint8_t> tt;
  tt.push_back((uint8_t)((n & 0x1)));
  tt.push_back((uint8_t)((n & 0x2) >> 1));
  tt.push_back((uint8_t)((n & 0x4) >> 2));
  tt.push_back((uint8_t)((n & 0x8) >> 3));
  tt.push_back((uint8_t)((n & 0x10) >> 4));
  tt.push_back((uint8_t)((n & 0x20) >> 5));
  tt.push_back((uint8_t)((n & 0x40) >> 6));
  tt.push_back((uint8_t)((n & 0x80) >> 7));
  //  std::reverse(tt.begin(), tt.end());
  Matrix<8, 1, uint8_t> a;
  memcpy(a.data, tt.data(), decltype(a)::byte_count);
  auto y = Mul<8, 8, 1, uint8_t, AddOp, MulOp>(x2, a);
  auto r = Add<8, 1, uint8_t, AddOp>(y, c2);

  uint8_t kk = r.data[0][0] << 0 | r.data[1][0] << 1 | r.data[2][0] << 2 |
               r.data[3][0] << 3 | r.data[4][0] << 4 | r.data[5][0] << 5 |
               r.data[6][0] << 6 | r.data[7][0] << 7;
  kk = GetInverse(kk);
  return kk;
}

std::vector<uint8_t> BuildSbox() {
  std::vector<uint8_t> sbox;
  for (size_t i = 0; i < 256; ++i) {
    sbox.push_back(SBoxTr(i));
  }
  return sbox;
}
std::vector<uint8_t> BuildSboxInverse() {
  std::vector<uint8_t> sbox;
  for (size_t i = 0; i < 256; ++i) {
    sbox.push_back(SBoxInverseTr(i));
  }
  return sbox;
}

uint8_t ginv(uint8_t a) {
  uint8_t j, b = a;
  for (j = 14; --j;)             /* for j from 13 downto 1 */
    b = gmult(b, j & 1 ? b : a); /* alternatively square and multiply */
  return b;
}

TEST_CASE("inverse", "[sbox]") {
  REQUIRE(0xca == GetInverse(0x53));
  REQUIRE(0xca == ginv(0x53));
}

TEST_CASE("basic tr", "[sbox]") {
  auto x = SBoxTr(0x53);
  REQUIRE(x == 0xed);
  auto z = SBoxTr(0x64);
  REQUIRE(z == 0x43);
}

namespace {
    auto sbox = std::move(BuildSbox());
    auto sboxi = std::move(BuildSboxInverse());
}
void SboxTr(Matrix<4, 4, uint8_t> &m) {
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      m.data[i][j] = sbox[m.data[i][j]];
    }
  }
}

uint8_t SboxTrOne(uint8_t x) {
  return sbox[x];
}

void SboxInverseTr(Matrix<4, 4, uint8_t> &m) {
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      m.data[i][j] = sboxi[m.data[i][j]];
    }
  }
}
