//
// Created by Gin Liu on 2018/11/20.
//

#ifndef TENHARD_OTHER_IMPL_H
#define TENHARD_OTHER_IMPL_H

#include <array>
#include <cstdint>
#include <cstdlib>

void key_expansion(uint8_t *key, uint8_t *w);
void cipher(uint8_t *in, uint8_t *out, uint8_t *w);
void inv_cipher(uint8_t *in, uint8_t *out, uint8_t *w);

template <bool encrypt>
void Aes128Transform(uint8_t *data, uint8_t *out, uint8_t *key) {
  auto w = (uint8_t *)malloc(4 * (10 + 1) * 4); // Nb * (Nr +1) * 4
  key_expansion(key, w);
  if (encrypt) {
    cipher(data, out, w);
  } else {
    inv_cipher(data, out, w);
  }
}

class CTR {
private:
  uint8_t *nonce_, *key_, *w;
  void NonceIncrease() {
    int i = 15;
    while (nonce_[i] == 0xff) {
      nonce_[i] = 0;
      i--;
    }
    if (i != -1)
      nonce_[i]++;
  }

public:
  CTR(uint8_t *nonce, uint8_t *key) : nonce_(nonce), key_(key) {}

  void Do(uint8_t *data, uint8_t *out) {
    uint8_t buf[16];
    Aes128Transform<true>(nonce_, buf, key_);
    for (int j = 0; j < 16; ++j) {
      printf("%#x ", buf[j]);
    }
    puts("");

    for (int i = 0; i < 16; ++i) {
      out[i] = data[i] ^ buf[i];
    }
    NonceIncrease();
  }
};

#endif // TENHARD_OTHER_IMPL_H
