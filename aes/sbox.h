//
// Created by Gin Liu on 2018/11/20.
//

#ifndef TENHARD_SBOX_H
#define TENHARD_SBOX_H

#include "matrix.h"

void SboxTr(Matrix<4, 4, uint8_t> &m);

void SboxInverseTr(Matrix<4, 4, uint8_t> &m) ;

uint8_t SboxTrOne(uint8_t x) ;
#endif //TENHARD_SBOX_H
