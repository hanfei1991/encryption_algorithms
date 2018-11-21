//
// Created by Gin Liu on 2018/11/19.
//

#ifndef TENHARD_MATRIX_H
#define TENHARD_MATRIX_H
#include <array>
#include <string>
/*
 * Addition in GF(2^8)
 * http://en.wikipedia.org/wiki/Finite_field_arithmetic
 */
inline uint8_t gadd(uint8_t a, uint8_t b) { return a ^ b; }

/*
 * Subtraction in GF(2^8)
 * http://en.wikipedia.org/wiki/Finite_field_arithmetic
 */
inline uint8_t gsub(uint8_t a, uint8_t b) { return a ^ b; }

/*
 * Multiplication in GF(2^8)
 * http://en.wikipedia.org/wiki/Finite_field_arithmetic
 * Irreducible polynomial m(x) = x8 + x4 + x3 + x + 1
 */
inline uint8_t gmult(uint8_t a, uint8_t b) {

  uint8_t p = 0, i = 0, hbs = 0;

  for (i = 0; i < 8; i++) {
    if (b & 1) {
      p ^= a;
    }

    hbs = a & 0x80;
    a <<= 1;
    if (hbs)
      a ^= 0x1b; // 0000 0001 0001 1011
    b >>= 1;
  }

  return (uint8_t)p;
}

/*
 * Addition of 4 byte words
 * m(x) = x4+1
 */
inline void coef_add(uint8_t a[], uint8_t b[], uint8_t d[]) {

  d[0] = a[0] ^ b[0];
  d[1] = a[1] ^ b[1];
  d[2] = a[2] ^ b[2];
  d[3] = a[3] ^ b[3];
}

/*
 * Multiplication of 4 byte words
 * m(x) = x4+1
 */
inline void coef_mult(uint8_t *a, uint8_t *b, uint8_t *d) {

  d[0] = gmult(a[0], b[0]) ^ gmult(a[3], b[1]) ^ gmult(a[2], b[2]) ^
         gmult(a[1], b[3]);
  d[1] = gmult(a[1], b[0]) ^ gmult(a[0], b[1]) ^ gmult(a[3], b[2]) ^
         gmult(a[2], b[3]);
  d[2] = gmult(a[2], b[0]) ^ gmult(a[1], b[1]) ^ gmult(a[0], b[2]) ^
         gmult(a[3], b[3]);
  d[3] = gmult(a[3], b[0]) ^ gmult(a[2], b[1]) ^ gmult(a[1], b[2]) ^
         gmult(a[0], b[3]);
}

struct AddOp {
  inline uint8_t operator()(uint8_t a, uint8_t b) { return gadd(a, b); }
};

struct MulOp {
  inline uint8_t operator()(uint8_t a, uint8_t b) { return gmult(a, b); }
};

struct AddOpInt {
  inline int operator()(int a, int b) { return a + b; }
};

struct MulOpInt {
  inline int operator()(int a, int b) { return a * b; }
};

template <size_t X, size_t Y, class T = uint8_t> class Matrix {
public:
  using element_type = T;
  static size_t byte_count;
  T data[X][Y];

  void Print(const std::string &prefix = std::string()) {
    printf("[START]%s:\n", prefix.c_str());
    for (size_t i = 0; i < X; ++i) {
      for (size_t j = 0; j < Y; ++j) {
        printf("%#03x ", data[i][j]);
      }
      printf("\n");
    }
    printf("%s[END]\n\n", prefix.c_str());
  }
};

template <size_t X, size_t Y, class T>
size_t Matrix<X, Y, T>::byte_count = sizeof(T) * X *Y;

template <size_t X, size_t Y, size_t Z, class T, class AddOp, class MulOp>
Matrix<X, Z, T> Mul(const Matrix<X, Y, T> &a, const Matrix<Y, Z, T> &b) {
  Matrix<X, Z, T> m;
  for (size_t i = 0; i < X; ++i) {
    for (size_t j = 0; j < Z; ++j) {
      typename Matrix<X, Y, T>::element_type sum = 0;
      for (size_t k = 0; k < Y; ++k) {
        sum = AddOp()(sum, MulOp()(a.data[i][k], b.data[k][j]));
      }
      m.data[i][j] = sum;
      //      printf("[i][j] sum, %d %d %d\n", i, j, sum);
    }
  }
  return m;
}
template <size_t X, size_t Y, class T, class AddOp>
Matrix<X, Y, T> Add(const Matrix<X, Y, T> &a, const Matrix<X, Y, T> &b) {
  Matrix<X, Y, T> m;
  for (size_t i = 0; i < X; ++i) {
    for (size_t j = 0; j < Y; ++j) {
      typename Matrix<X, Y, T>::element_type sum = 0;
      sum = AddOp()(a.data[i][j], b.data[i][j]);
      m.data[i][j] = sum;
      //      printf("[i][j] sum, %d %d %d\n", i, j, sum);
    }
  }
  return m;
}

template <size_t X, size_t Y, class T, class Convertor>
void MatrixConvert(Matrix<X, Y, T> &m, T *state, Convertor cc) {
  using M = Matrix<X, Y, T>;
  memcpy(m.data, state, M::byte_count);
  cc(m);
  memcpy(state, m.data, M::byte_count);
}
#endif // TENHARD_MATRIX_H
