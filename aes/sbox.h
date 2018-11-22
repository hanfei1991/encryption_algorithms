//
// Created by Gin Liu on 2018/11/20.
//

#ifndef TENHARD_SBOX_H
#define TENHARD_SBOX_H

#include "matrix.h"

uint8_t SboxTrOne(uint8_t x);
uint8_t SboxInverseTrOne(uint8_t x);

template <size_t X, size_t Y> void SboxTr(Matrix<X, Y, uint8_t> &m) {
  for (size_t i = 0; i < X; ++i) {
    for (size_t j = 0; j < Y; ++j) {
      m.data[i][j] = SboxTrOne(m.data[i][j]);
    }
  }
}

template <size_t X, size_t Y> void SboxInverseTr(Matrix<X, Y, uint8_t> &m) {
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      m.data[i][j] = SboxInverseTrOne(m.data[i][j]);
    }
  }
}

#endif // TENHARD_SBOX_H
