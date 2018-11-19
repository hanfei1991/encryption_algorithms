//
// Created by Gin Liu on 2018/11/19.
//

#ifndef TENHARD_MATRIX_H
#define TENHARD_MATRIX_H
#include <array>

template <size_t X, size_t Y, class T = int> class Matrix {
public:
  using element_type = T;
  static size_t byte_count;
  T data[X][Y];
  void Print() {
    for (size_t i = 0; i < X; ++i) {
      for (size_t j = 0; j < Y; ++j) {
        printf("%03d ", data[i][j]);
      }
      printf("\n");
    }
  }
};

template <size_t X, size_t Y, class T>
size_t Matrix<X, Y, T>::byte_count = sizeof(T) * X *Y;

template <size_t X, size_t Y, size_t Z>
Matrix<X, Z> Mul(const Matrix<X, Y> &a, const Matrix<Y, Z> &b) {
  Matrix<X, Z> m;
  for (size_t i = 0; i < X; ++i) {
    for (size_t j = 0; j < Z; ++j) {
      typename Matrix<X, Y>::element_type sum = 0;
      for (size_t k = 0; k < Y; ++k) {
        sum += typename Matrix<X, Y>::element_type(1) * a.data[i][k] *
               b.data[k][j];
      }
      m.data[i][j] = sum;
      printf("[i][j] sum, %d %d %d\n", i, j, sum);
    }
  }
  return m;
}

#endif // TENHARD_MATRIX_H
