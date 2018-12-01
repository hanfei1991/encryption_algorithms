
#include <cstdint>

#include "../catch.hpp"
#include "aes.h"
#include "aes_ctr.h"
#include "matrix.h"
#include "sbox.h"

/*
 * The cipher Key.
 */
int K;

/*
 * Number of columns (32-bit words) comprising the State. For this
 * standard, Nb = 4.
 */
int Nb = 4;

/*
 * Number of 32-bit words comprising the Cipher Key. For this
 * standard, Nk = 4, 6, or 8.
 */
int Nk = 4;

/*
 * Number of rounds, which is a function of  Nk  and  Nb (which is
 * fixed). For this standard, Nr = 10, 12, or 14.
 */
int Nr = 10;

/*
 * Key Expansion
 */
void key_expansion(uint8_t *key, uint8_t *w) {

  uint8_t tmp[4];
  uint8_t i;
  uint8_t len = Nb * (Nr + 1);
  // printf("%d %d %d %d[end]", i, len, Nb, Nr);
  for (i = 0; i < Nk; i++) {
    w[4 * i + 0] = key[4 * i + 0];
    w[4 * i + 1] = key[4 * i + 1];
    w[4 * i + 2] = key[4 * i + 2];
    w[4 * i + 3] = key[4 * i + 3];
  }

  for (i = Nk; i < len; i++) {
    tmp[0] = w[4 * (i - 1) + 0];
    tmp[1] = w[4 * (i - 1) + 1];
    tmp[2] = w[4 * (i - 1) + 2];
    tmp[3] = w[4 * (i - 1) + 3];

    if (i % Nk == 0) {

      rot_word(tmp);
      sub_word(tmp);
      coef_add(tmp, Rcon(i / Nk), tmp);
    } else if (Nk > 6 && i % Nk == 4) {
      sub_word(tmp);
    }

    w[4 * i + 0] = w[4 * (i - Nk) + 0] ^ tmp[0];
    w[4 * i + 1] = w[4 * (i - Nk) + 1] ^ tmp[1];
    w[4 * i + 2] = w[4 * (i - Nk) + 2] ^ tmp[2];
    w[4 * i + 3] = w[4 * (i - Nk) + 3] ^ tmp[3];
  }
}

void cipher(uint8_t *in, uint8_t *out, uint8_t *w) {

  uint8_t state[4 * Nb];
  uint8_t r, i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < Nb; j++) {
      state[Nb * i + j] = in[i + 4 * j];
    }
  }

  add_round_key(state, w, 0);

  for (r = 1; r < Nr; r++) {
    sub_bytes(state);
    shift_rows(state);
    mix_columns(state);
    add_round_key(state, w, r);
  }

  sub_bytes(state);
  shift_rows(state);
  add_round_key(state, w, Nr);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < Nb; j++) {
      out[i + 4 * j] = state[Nb * i + j];
    }
  }
}

void inv_cipher(uint8_t *in, uint8_t *out, uint8_t *w) {

  uint8_t state[4 * Nb];
  uint8_t r, i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < Nb; j++) {
      state[Nb * i + j] = in[i + 4 * j];
    }
  }

  add_round_key(state, w, Nr);

  for (r = Nr - 1; r >= 1; r--) {
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, w, r);
    inv_mix_columns(state);
  }

  inv_shift_rows(state);
  inv_sub_bytes(state);
  add_round_key(state, w, 0);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < Nb; j++) {
      out[i + 4 * j] = state[Nb * i + j];
    }
  }
}

uint8_t key[] = {
    0xC4, 0xCA, 0x42, 0x38, 0xA0, 0xB9, 0x23, 0x82,
    0x0D, 0xCC, 0x50, 0x9A, 0x6F, 0x75, 0x84, 0x9B,
};

uint8_t key2[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};

uint8_t in[] = {0xb8, 0xe3, 0x7e, 0x4e, 0x5c, 0xc0, 0x4a, 0x84,
                0xba, 0x9b, 0x1c, 0xf0, 0xcd, 0x63, 0xd9, 0x38};

uint8_t out[16]; // 128
TEST_CASE("aes other algorithms", "[aes]") {

  uint8_t i;

  /*
   * Appendix A - Key Expansion Examples
   */

  /* 128 bits */
  uint8_t key_default[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                           0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

  /* 192 bits */
  /* uint8_t key[] = {
      0x8e, 0x73, 0xb0, 0xf7,
      0xda, 0x0e, 0x64, 0x52,
      0xc8, 0x10, 0xf3, 0x2b,
      0x80, 0x90, 0x79, 0xe5,
      0x62, 0xf8, 0xea, 0xd2,
      0x52, 0x2c, 0x6b, 0x7b}; */

  /* 256 bits */
  /* uint8_t key[] = {
      0x60, 0x3d, 0xeb, 0x10,
      0x15, 0xca, 0x71, 0xbe,
      0x2b, 0x73, 0xae, 0xf0,
      0x85, 0x7d, 0x77, 0x81,
      0x1f, 0x35, 0x2c, 0x07,
      0x3b, 0x61, 0x08, 0xd7,
      0x2d, 0x98, 0x10, 0xa3,
      0x09, 0x14, 0xdf, 0xf4};
  */

  uint8_t in_default[] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31,
                          0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34}; // 128

  unsigned int in_len = 16;

  unsigned char __16_txt_enc[] = {0x03, 0xc8, 0x63, 0x41, 0x3c, 0x66,
                                  0xb8, 0x23, 0xe9, 0xdd, 0xb4, 0xc0,
                                  0xf9, 0x71, 0xc4, 0x28};

  /*
   * Appendix C - Example Vectors
   */

  /* 128 bit key */
  /* uint8_t key[] = {
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x0c, 0x0d, 0x0e, 0x0f}; */

  /* 192 bit key */
  /* uint8_t key[] = {
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x0c, 0x0d, 0x0e, 0x0f,
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17}; */

  /* 256 bit key */
  /*
  uint8_t key[] = {
          0x00, 0x01, 0x02, 0x03,
          0x04, 0x05, 0x06, 0x07,
          0x08, 0x09, 0x0a, 0x0b,
          0x0c, 0x0d, 0x0e, 0x0f,
          0x10, 0x11, 0x12, 0x13,
          0x14, 0x15, 0x16, 0x17,
          0x18, 0x19, 0x1a, 0x1b,
          0x1c, 0x1d, 0x1e, 0x1f};

  uint8_t in[] = {
          0x00, 0x11, 0x22, 0x33,
          0x44, 0x55, 0x66, 0x77,
          0x88, 0x99, 0xaa, 0xbb,
          0xcc, 0xdd, 0xee, 0xff};
          */

  uint8_t *w; // expanded key

  switch (sizeof(key)) {
  default:
  case 16:
    Nk = 4;
    Nr = 10;
    break;
  case 24:
    Nk = 6;
    Nr = 12;
    break;
  case 32:
    Nk = 8;
    Nr = 14;
    break;
  }

  w = (uint8_t *)malloc(Nb * (Nr + 1) * 4);

  key_expansion(key, w);

  cipher(in /* in */, out /* out */, w /* expanded key */);

  printf("out:\n");

  for (i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }

  printf("\n");

  inv_cipher(out, in, w);

  printf("msg:\n");
  for (i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", in[4 * i + 0], in[4 * i + 1], in[4 * i + 2],
           in[4 * i + 3]);
  }

  printf("\n");
}

uint8_t a[16] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
uint8_t a2[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t a3[] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

TEST_CASE("ctr", "[ctr]") {
  unsigned char __32_txt[] = {0x2b, 0x41, 0x5d, 0x44, 0x59, 0x6e, 0x3a, 0x28,
                              0xe5, 0x08, 0xaf, 0x0b, 0x13, 0xb4, 0x00, 0xc2,

                              0x49, 0x6f, 0x23, 0x95, 0xb6, 0x88, 0xe0, 0x41,
                              0x17, 0x44, 0x64, 0x4b, 0x96, 0x37, 0x92, 0xc3};
  unsigned int __32_txt_len = 32;
  CTR ctr(a, key);
  ctr.Do(__32_txt, out);
  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");
  ctr.Do(__32_txt + 16, out);
  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");
}

TEST_CASE("ctr 2", "[ctr]") {

  uint8_t iv[16] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  };

  uint8_t key[] = {
      0xC4, 0xCA, 0x42, 0x38, 0xA0, 0xB9, 0x23, 0x82,
      0x0D, 0xCC, 0x50, 0x9A, 0x6F, 0x75, 0x84, 0x9B,
  };

  uint8_t out[16];

  unsigned char input[] = {0x2b, 0x41, 0x5d, 0x44, 0x59, 0x6e, 0x3a, 0x28,
                           0xe5, 0x08, 0xaf, 0x0b, 0x13, 0xb4, 0x00, 0xc2,

                           0x49, 0x6f, 0x23, 0x95, 0xb6, 0x88, 0xe0, 0x41,
                           0x17, 0x44, 0x64, 0x4b, 0x96, 0x37, 0x92, 0xc3};

  CTR ctr(iv, key);
  // Do every block for ctr, and ctr support all 0xff iv, and iv will go to all
  // 0x00 from one.
  ctr.Do(input, out);

  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");

  ctr.Do(input + 16, out);

  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");

}