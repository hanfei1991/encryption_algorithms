//
// Created by Gin Liu on 2018/11/21.
//

#ifndef TENHARD_AES_H
#define TENHARD_AES_H
#include "matrix.h"
#include <cstdint>

void shift_rows(uint8_t *state);
void inv_shift_rows(uint8_t *state);

void mix_columns(uint8_t *state);
void inv_mix_columns(uint8_t *state);

void sub_bytes(uint8_t *state);
void inv_sub_bytes(uint8_t *state);

uint8_t *Rcon(uint8_t i);
/*
 * Function used in the Key Expansion routine that takes a four-byte
 * word and performs a cyclic permutation.
 */
inline void rot_word(uint8_t *w) {

  uint8_t tmp;
  uint8_t i;

  tmp = w[0];

  for (i = 0; i < 3; i++) {
    w[i] = w[i + 1];
  }

  w[3] = tmp;
}
/*
 * Function used in the Key Expansion routine that takes a four-byte
 * input word and applies an S-box to each of the four bytes to
 * produce an output word.
 */
void sub_word(uint8_t *w);
/*
 * Transformation in the Cipher and Inverse Cipher in which a Round
 * Key is added to the State using an XOR operation. The length of a
 * Round Key equals the size of the State (i.e., for Nb = 4, the Round
 * Key length equals 128 bits/16 bytes).
 */
void add_round_key(uint8_t *state, uint8_t *w, uint8_t r);
#endif // TENHARD_AES_H
