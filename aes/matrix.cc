//
// Created by Gin Liu on 2018/11/19.
//
#include "matrix.h"
#include <vector>

int main() {
  using MatrixInt4_3 = Matrix<4, 3, int>;
  using MatrixInt3_5 = Matrix<3, 5, int>;
  using EleType = MatrixInt4_3 ::element_type;
  MatrixInt4_3 x;
  MatrixInt3_5 y;
  std::vector<EleType> v1 = {
      1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 4, 8,
  };

  std::vector<EleType> v2 = {
      1, 2, 3, 4, 5, 6, 1, 2, 4, 8, 16, 32, 1, 2, 3,
  };

  std::vector<EleType> vr = {
      61, 100, 10, 18, 30,  130, 205, 28, 48, 78,
      71, 54,  38, 62, 102, 154, 264, 22, 40, 66,
  };
  memcpy(x.data, v1.data(), MatrixInt4_3::byte_count);
  memcpy(y.data, v2.data(), MatrixInt3_5::byte_count);

  x.Print();
  printf("\n");
  y.Print();
  printf("\n");
  auto r = Mul<4, 3, 5, int, AddOpInt, MulOpInt>(x, y);
  printf("\n");

  r.Print();
  printf("\n");
  assert(!memcmp((void *)&r.data, vr.data(), Matrix<4, 5>::byte_count));
  return 0;
}
