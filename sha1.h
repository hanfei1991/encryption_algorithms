//
// Created by Gin Liu on 2018/11/2.
//

//
// Created by Gin Liu on 2018/9/9.
//
#include <array>
#include <fstream>
#include <string>
#include <vector>
/**
 * Bit rotation left by a compile-time constant amount
 * @param input the input word
 * @return input rotated left by ROT bits
 */
template <size_t ROT, typename T>
inline T rotl(T input) {
  static_assert(ROT > 0 && ROT < 8 * sizeof(T), "Invalid rotation constant");
  return static_cast<T>((input << ROT) | (input >> (8 * sizeof(T) - ROT)));
}

/**
 * Bit rotation right by a compile-time constant amount
 * @param input the input word
 * @return input rotated right by ROT bits
 */
template <size_t ROT, typename T>
inline T rotr(T input) {
  static_assert(ROT > 0 && ROT < 8 * sizeof(T), "Invalid rotation constant");
  return static_cast<T>((input >> ROT) | (input << (8 * sizeof(T) - ROT)));
}

/**
 * Bit rotation left, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated left by rot bits
 */
template <typename T>
inline T rotl_var(T input, size_t rot) {
  return rot ? static_cast<T>((input << rot) | (input >> (sizeof(T) * 8 - rot)))
             : input;
}

/**
 * Bit rotation right, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated right by rot bits
 */
template <typename T>
inline T rotr_var(T input, size_t rot) {
  return rot ? static_cast<T>((input >> rot) | (input << (sizeof(T) * 8 - rot)))
             : input;
}
void sse2_compress_n(std::vector<uint32_t>& digest, const uint8_t *input,
                     size_t blocks);
