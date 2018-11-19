//
// Created by Gin Liu on 2018/11/19.
//
#include "matrix.h"
#include <vector>

int main() {
  Matrix<4, 3> x;
  Matrix<3, 5> y;
  std::vector<Matrix<4, 3>::element_type> v1 = {
      1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 4, 8,
  };

  std::vector<Matrix<3, 5>::element_type> v2 = {
      1, 2, 3, 4, 5, 6, 1, 2, 4, 8, 16, 32, 1, 2, 3,
  };

  std::vector<Matrix<4, 5>::element_type> vr = {
      61, 100, 10, 18, 30,  130, 205, 28, 48, 78,
      71, 54,  38, 62, 102, 154, 264, 22, 40, 66,
  };
  memcpy(x.data, v1.data(), Matrix<4, 3>::byte_count);
  memcpy(y.data, v2.data(), Matrix<3, 5>::byte_count);

  x.Print();
  printf("\n");
  y.Print();
  printf("\n");
  auto r = Mul(x, y);
  printf("\n");

  r.Print();
  printf("\n");
  assert(!memcmp((void *)&r.data, vr.data(), Matrix<4, 5>::byte_count));
  return 0;
}
