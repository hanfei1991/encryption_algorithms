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
#endif // TENHARD_AES_H
