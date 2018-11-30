//
// Created by Gin Liu on 2018/11/21.
//

#include "aes.h"
#include "matrix.h"
#include "sbox.h"
#include <vector>

namespace {
int Nb = 4;
}
/*
 * Transformation in the Cipher that processes the State by cyclically
 * shifting the last three rows of the State by different offsets.
 */
void shift_rows(uint8_t *state) {

  uint8_t i, k, s, tmp;

  for (i = 1; i < 4; i++) {
    // shift(1,4)=1; shift(2,4)=2; shift(3,4)=3
    // shift(r, 4) = r;
    s = 0;
    while (s < i) {
      tmp = state[Nb * i + 0];

      for (k = 1; k < Nb; k++) {
        state[Nb * i + k - 1] = state[Nb * i + k];
      }

      state[Nb * i + Nb - 1] = tmp;
      s++;
    }
  }
}

/*
 * Transformation in the Inverse Cipher that is the inverse of
 * ShiftRows().
 */
void inv_shift_rows(uint8_t *state) {

  uint8_t i, k, s, tmp;

  for (i = 1; i < 4; i++) {
    s = 0;
    while (s < i) {
      tmp = state[Nb * i + Nb - 1];

      for (k = Nb - 1; k > 0; k--) {
        state[Nb * i + k] = state[Nb * i + k - 1];
      }

      state[Nb * i + 0] = tmp;
      s++;
    }
  }
}

std::vector<uint8_t> mixMatrix = {
    2, 3, 1, 1, 1, 2, 3, 1, 1, 1, 2, 3, 3, 1, 1, 2,
};

std::vector<uint8_t> mixMatrixInverse = {0xe, 0xb, 0xd, 0x9, 0x9, 0xe,
                                         0xb, 0xd, 0xd, 0x9, 0xe, 0xb,
                                         0xb, 0xd, 0x9, 0xe};

void mix_columns_inner(uint8_t *state,
                       const std::vector<uint8_t> &matrix_data) {
  Matrix<4, 4, uint8_t> x;
  Matrix<4, 4, uint8_t> m;
  memcpy(x.data, matrix_data.data(), decltype(x)::byte_count);
  MatrixConvert(m, state, [&x](decltype(m) &mm) {
    mm = Mul<4, 4, 4, uint8_t, AddOp, MulOp>(x, mm);
  });
}

void mix_columns(uint8_t *state) { mix_columns_inner(state, mixMatrix); }

void inv_mix_columns(uint8_t *state) {
  mix_columns_inner(state, mixMatrixInverse);
}

void sub_bytes(uint8_t *state) {
  Matrix<4, 4, uint8_t> m;
  MatrixConvert(m, state, [](decltype(m) &m) { SboxTr(m); });
}

void inv_sub_bytes(uint8_t *state) {
  Matrix<4, 4, uint8_t> m;
  MatrixConvert(m, state, [](decltype(m) &m) { SboxInverseTr(m); });
}

/*
 * Generates the round constant Rcon[i]
 */
namespace {
uint8_t R[] = {0x02, 0x00, 0x00, 0x00};
std::vector<uint8_t> cache;
std::vector<bool> flags;
auto xx = ([]() -> int {
  cache.resize(4 * (15 + 1), 0);
  flags.resize(4 * (15 + 1), false);
  return 0;
})();
} // namespace

uint8_t *Rcon(uint8_t i) {
  //if (!flags[i]) { //TODO(Gin) cache has bug
    if (i == 1) {
      R[0] = 0x01; // x^(1-1) = x^0 = 1
    } else if (i > 1) {
      R[0] = 0x02;
      i--;
      while (i - 1 > 0) {
        R[0] = gmult(R[0], 0x02);
        i--;
      }
    }
    cache[i] = R[0];
    flags[i] = true;
  //} else {
  //  R[0] = cache[i];
  //}
//  printf("R: %d [end]", R[0]);
  return R;
}

void sub_word(uint8_t *w) {
  uint8_t i;
  Matrix<4, 1, uint8_t> m;
  MatrixConvert(m, w, [](decltype(m) &m) { SboxTr(m); });
}

void add_round_key(uint8_t *state, uint8_t *w, uint8_t r) {
  Matrix<4, 4, uint8_t> m, wm;
  memcpy(m.data, state, decltype(m)::byte_count);
  memcpy(wm.data, w + 4 * 4 * r, decltype(wm)::byte_count);
  MatrixTranspose(wm);
  m = Add<4, 4, uint8_t, AddOp>(m, wm);
  memcpy(state, m.data, decltype(m)::byte_count);
}