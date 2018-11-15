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
#include "sha1.h"
//#define BUILD_WITH_PROFILER
#define USE_SSE 0
#ifdef BUILD_WITH_PROFILER
#include <easy/profiler.h>
#endif

/*
 * SHA-160 F1 Function
 */
inline void F1(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E,
               uint32_t msg) {
  E += (D ^ (B & (C ^ D))) + msg + 0x5A827999 + rotl<5>(A);
  B = rotl<30>(B);
}

/*
 * SHA-160 F2 Function
 */
inline void F2(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E,
               uint32_t msg) {
  E += (B ^ C ^ D) + msg + 0x6ED9EBA1 + rotl<5>(A);
  B = rotl<30>(B);
}

/*
 * SHA-160 F3 Function
 */
inline void F3(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E,
               uint32_t msg) {
  E += ((B & C) | ((B | C) & D)) + msg + 0x8F1BBCDC + rotl<5>(A);
  B = rotl<30>(B);
}

/*
 * SHA-160 F4 Function
 */
inline void F4(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E,
               uint32_t msg) {
  E += (B ^ C ^ D) + msg + 0xCA62C1D6 + rotl<5>(A);
  B = rotl<30>(B);
}

/**
 * Swap a 16 bit integer
 */
inline uint16_t reverse_bytes(uint16_t val) { return rotl<8>(val); }

#ifdef _WIN32
#define BOTAN_BUILD_COMPILER_IS_MSVC
#endif
/**
 * Swap a 32 bit integer
 */
inline uint32_t reverse_bytes(uint32_t val) {
#if defined(BOTAN_BUILD_COMPILER_IS_GCC) || \
    defined(BOTAN_BUILD_COMPILER_IS_CLANG)
  return __builtin_bswap32(val);

#elif defined(BOTAN_BUILD_COMPILER_IS_MSVC)
  return _byteswap_ulong(val);

#elif defined(BOTAN_USE_GCC_INLINE_ASM) && \
    defined(BOTAN_TARGET_CPU_IS_X86_FAMILY)

  // GCC-style inline assembly for x86 or x86-64
  asm("bswapl %0" : "=r"(val) : "0"(val));
  return val;

#else

  // Generic implementation
  return (rotr<8>(val) & 0xFF00FF00) | (rotl<8>(val) & 0x00FF00FF);

#endif
}

/**
 * Swap a 64 bit integer
 */
inline uint64_t reverse_bytes(uint64_t val) {
#if defined(BOTAN_BUILD_COMPILER_IS_GCC) || \
    defined(BOTAN_BUILD_COMPILER_IS_CLANG)
  return __builtin_bswap64(val);

#elif defined(BOTAN_BUILD_COMPILER_IS_MSVC)
  return _byteswap_uint64(val);

#elif defined(BOTAN_USE_GCC_INLINE_ASM) && defined(BOTAN_TARGET_ARCH_IS_X86_64)
  // GCC-style inline assembly for x86-64
  asm("bswapq %0" : "=r"(val) : "0"(val));
  return val;

#else
  /* Generic implementation. Defined in terms of 32-bit bswap so any
   * optimizations in that version can help.
   */

  uint32_t hi = static_cast<uint32_t>(val >> 32);
  uint32_t lo = static_cast<uint32_t>(val);

  hi = reverse_bytes(hi);
  lo = reverse_bytes(lo);

  return (static_cast<uint64_t>(lo) << 32) | hi;
#endif
}

/**
 * Swap 4 Ts in an array
 */
template <typename T>
inline void bswap_4(T x[4]) {
  x[0] = reverse_bytes(x[0]);
  x[1] = reverse_bytes(x[1]);
  x[2] = reverse_bytes(x[2]);
  x[3] = reverse_bytes(x[3]);
}

template <typename T>
inline void load_be(T out[], const uint8_t in[], size_t count) {
  if (count > 0) {
#if 0  // defined(BOTAN_TARGET_CPU_IS_BIG_ENDIAN)
        std::memcpy(out, in, sizeof(T)*count);

#elif 1
    std::memcpy(out, in, sizeof(T) * count);
    const size_t blocks = count - (count % 4);
    const size_t left = count - blocks;

    for (size_t i = 0; i != blocks; i += 4) bswap_4(out + i);

    for (size_t i = 0; i != left; ++i)
      out[blocks + i] = reverse_bytes(out[blocks + i]);
#else
    for (size_t i = 0; i != count; ++i) out[i] = load_be<T>(in, i);
#endif
  }
}

struct SHA1Cal {
  std::vector<uint32_t> m_digest = std::vector<uint32_t>(5, 0);

  void Init() {
    m_digest[0] = 0x67452301;
    m_digest[1] = 0xEFCDAB89;
    m_digest[2] = 0x98BADCFE;
    m_digest[3] = 0x10325476;
    m_digest[4] = 0xC3D2E1F0;
    
  }

  void SHA1Hash(uint8_t* bytes, size_t len) {

#ifdef BUILD_WITH_PROFILER
    EASY_FUNCTION();
#endif
#if USE_SSE
    sse2_compress_n(m_digest, bytes, len);
    return;
#else
    std::vector<uint32_t> m_W;
	uint32_t A = m_digest[0], B = m_digest[1], C = m_digest[2], D = m_digest[3],
    E = m_digest[4];
    /**
     * The message buffer
     */

    m_W.resize(80, 0);

    //    puts("m_W:");
    //    for (int kk = 0; kk < 80; ++kk)
    //    {
    //        if (kk % 16 == 0) puts("");
    //        printf("%08x ", m_W[kk]);
    //    }
    //    puts("\nend");

    for (size_t i = 0; i != len; ++i) {
      //        puts("input");
      //        for(int jj = 0; jj < 64; ++jj)
      //        {
      //            printf("%02x ", bytes[jj]);
      //        }
      //        puts("\nend");

      load_be(m_W.data(), bytes, 16);

      //        puts("m_W:");
      //        for (int kk = 0; kk < 80; ++kk)
      //        {
      //            if (kk % 16 == 0) puts("");
      //            printf("%08x ", m_W[kk]);
      //        }
      //        puts("\nend");

      for (size_t j = 16; j != 80; j += 8) {
        m_W[j] = rotl<1>(m_W[j - 3] ^ m_W[j - 8] ^ m_W[j - 14] ^ m_W[j - 16]);
        m_W[j + 1] =
            rotl<1>(m_W[j - 2] ^ m_W[j - 7] ^ m_W[j - 13] ^ m_W[j - 15]);
        m_W[j + 2] =
            rotl<1>(m_W[j - 1] ^ m_W[j - 6] ^ m_W[j - 12] ^ m_W[j - 14]);
        m_W[j + 3] = rotl<1>(m_W[j] ^ m_W[j - 5] ^ m_W[j - 11] ^ m_W[j - 13]);
        m_W[j + 4] =
            rotl<1>(m_W[j + 1] ^ m_W[j - 4] ^ m_W[j - 10] ^ m_W[j - 12]);
        m_W[j + 5] =
            rotl<1>(m_W[j + 2] ^ m_W[j - 3] ^ m_W[j - 9] ^ m_W[j - 11]);
        m_W[j + 6] =
            rotl<1>(m_W[j + 3] ^ m_W[j - 2] ^ m_W[j - 8] ^ m_W[j - 10]);
        m_W[j + 7] = rotl<1>(m_W[j + 4] ^ m_W[j - 1] ^ m_W[j - 7] ^ m_W[j - 9]);
      }

      F1(A, B, C, D, E, m_W[0]);
      F1(E, A, B, C, D, m_W[1]);
      F1(D, E, A, B, C, m_W[2]);
      F1(C, D, E, A, B, m_W[3]);
      F1(B, C, D, E, A, m_W[4]);
      F1(A, B, C, D, E, m_W[5]);
      F1(E, A, B, C, D, m_W[6]);
      F1(D, E, A, B, C, m_W[7]);
      F1(C, D, E, A, B, m_W[8]);
      F1(B, C, D, E, A, m_W[9]);
      F1(A, B, C, D, E, m_W[10]);
      F1(E, A, B, C, D, m_W[11]);
      F1(D, E, A, B, C, m_W[12]);
      F1(C, D, E, A, B, m_W[13]);
      F1(B, C, D, E, A, m_W[14]);
      F1(A, B, C, D, E, m_W[15]);
      F1(E, A, B, C, D, m_W[16]);
      F1(D, E, A, B, C, m_W[17]);
      F1(C, D, E, A, B, m_W[18]);
      F1(B, C, D, E, A, m_W[19]);

      F2(A, B, C, D, E, m_W[20]);
      F2(E, A, B, C, D, m_W[21]);
      F2(D, E, A, B, C, m_W[22]);
      F2(C, D, E, A, B, m_W[23]);
      F2(B, C, D, E, A, m_W[24]);
      F2(A, B, C, D, E, m_W[25]);
      F2(E, A, B, C, D, m_W[26]);
      F2(D, E, A, B, C, m_W[27]);
      F2(C, D, E, A, B, m_W[28]);
      F2(B, C, D, E, A, m_W[29]);
      F2(A, B, C, D, E, m_W[30]);
      F2(E, A, B, C, D, m_W[31]);
      F2(D, E, A, B, C, m_W[32]);
      F2(C, D, E, A, B, m_W[33]);
      F2(B, C, D, E, A, m_W[34]);
      F2(A, B, C, D, E, m_W[35]);
      F2(E, A, B, C, D, m_W[36]);
      F2(D, E, A, B, C, m_W[37]);
      F2(C, D, E, A, B, m_W[38]);
      F2(B, C, D, E, A, m_W[39]);

      F3(A, B, C, D, E, m_W[40]);
      F3(E, A, B, C, D, m_W[41]);
      F3(D, E, A, B, C, m_W[42]);
      F3(C, D, E, A, B, m_W[43]);
      F3(B, C, D, E, A, m_W[44]);
      F3(A, B, C, D, E, m_W[45]);
      F3(E, A, B, C, D, m_W[46]);
      F3(D, E, A, B, C, m_W[47]);
      F3(C, D, E, A, B, m_W[48]);
      F3(B, C, D, E, A, m_W[49]);
      F3(A, B, C, D, E, m_W[50]);
      F3(E, A, B, C, D, m_W[51]);
      F3(D, E, A, B, C, m_W[52]);
      F3(C, D, E, A, B, m_W[53]);
      F3(B, C, D, E, A, m_W[54]);
      F3(A, B, C, D, E, m_W[55]);
      F3(E, A, B, C, D, m_W[56]);
      F3(D, E, A, B, C, m_W[57]);
      F3(C, D, E, A, B, m_W[58]);
      F3(B, C, D, E, A, m_W[59]);

      F4(A, B, C, D, E, m_W[60]);
      F4(E, A, B, C, D, m_W[61]);
      F4(D, E, A, B, C, m_W[62]);
      F4(C, D, E, A, B, m_W[63]);
      F4(B, C, D, E, A, m_W[64]);
      F4(A, B, C, D, E, m_W[65]);
      F4(E, A, B, C, D, m_W[66]);
      F4(D, E, A, B, C, m_W[67]);
      F4(C, D, E, A, B, m_W[68]);
      F4(B, C, D, E, A, m_W[69]);
      F4(A, B, C, D, E, m_W[70]);
      F4(E, A, B, C, D, m_W[71]);
      F4(D, E, A, B, C, m_W[72]);
      F4(C, D, E, A, B, m_W[73]);
      F4(B, C, D, E, A, m_W[74]);
      F4(A, B, C, D, E, m_W[75]);
      F4(E, A, B, C, D, m_W[76]);
      F4(D, E, A, B, C, m_W[77]);
      F4(C, D, E, A, B, m_W[78]);
      F4(B, C, D, E, A, m_W[79]);

      A = (m_digest[0] += A);
      B = (m_digest[1] += B);
      C = (m_digest[2] += C);
      D = (m_digest[3] += D);
      E = (m_digest[4] += E);

      bytes += 64;
      //      		printf("\n%08x%08x%08x%08x%08x\n", A, B, C, D, E);
    }
#endif
  }

#define CRAZY_CRYPTO_ASSERT(a) ((void)0)

  static constexpr int BLOCK_SIZE = 64000000;
  void Update(const char* buf, int b) {
    SHA1Hash(reinterpret_cast<uint8_t*>(const_cast<char*>(buf)), b);
  }

  void Finish(std::vector<char>& data, uint64_t size) {
#ifdef BUILD_WITH_PROFILER
    EASY_FUNCTION();
#endif

    // can a file mod 7 == 0 in binary  bit level??
    // YES, But on most operating system, file always in bytes (8 bits). So we only deal with file bits mode 8 == 0, do not deal with file mod 7 things, so std::vector<char>'s all bits are file binary.

    CRAZY_CRYPTO_ASSERT(data.size() * 8 >= 0 && data.size() * 8 <= 511);
    unsigned char xx = static_cast<unsigned char>(0x80);
    data.push_back(xx);
    int len = data.size() * 8;
    //    printf("%d\n", len);
    int x = 448 - len;

    if (x <= 0) {
      x += 512;
    }

    if (x > 0) {
      x /= 8;
      data.insert(data.end(), x, 0);
    }

    //    printf("data size : %d\n", data.size() * 8);
    uint64_t l = size * 8;
    data.resize(data.size() + 8, 0);
    l = reverse_bytes(l);
    memcpy(reinterpret_cast<char*>(&data.back() - 7),
           reinterpret_cast<char*>(&l), 8);
    if (data.size() * 8 > 512) {
      Update(data.data(), 1);
      Update(data.data() + 64, 1);
    } else {
      Update(data.data(), 1);
    }
  }
  void FileStreamTransform(std::ifstream& fin) {

#ifdef BUILD_WITH_PROFILER
    EASY_FUNCTION();
#endif
    std::array<char, BLOCK_SIZE>* bufraw = new std::array<char, BLOCK_SIZE>();
    std::array<char, BLOCK_SIZE>& buf = *bufraw;
    memset(buf.data(), 0, sizeof(char) * BLOCK_SIZE);

#ifdef BUILD_WITH_PROFILER
      EASY_BLOCK("read file");
#endif
    fin.read(buf.data(), BLOCK_SIZE);
    auto p = fin.gcount();
#ifdef BUILD_WITH_PROFILER
    EASY_END_BLOCK;
#endif
    uint64_t i = 0;
    while (p == BLOCK_SIZE) {
      Update(buf.data(), BLOCK_SIZE / 64);
#ifdef BUILD_WITH_PROFILER
      EASY_BLOCK("read file");
#endif
      fin.read(buf.data(), BLOCK_SIZE);
      p = fin.gcount();
      i++;
#ifdef BUILD_WITH_PROFILER
      EASY_END_BLOCK;
#endif
    }

    auto left = p;
    int b = static_cast<int>(left / 64);
    if (b) {
      std::vector<char> data(buf.begin(), buf.begin() + b * 64);
      left -= b * 64;
      Update(data.data(), b);
    }
    if (left) {
      std::vector<char> data(buf.begin() + b * 64, buf.begin() + b * 64 + int(left));
      Finish(data, i * BLOCK_SIZE + uint64_t(1) * b * 64 + data.size());
    } else {
      std::vector<char> data;
      Finish(data, i * BLOCK_SIZE + uint64_t(1) * b * 64);
    }
	 uint32_t A = (m_digest[0]),
      B = (m_digest[1]),
      C = (m_digest[2]),
      D = (m_digest[3]),
      E = (m_digest[4]);
    printf("%08x%08x%08x%08x%08x", A, B, C, D, E);
  }
};

int main(int argc, char** argv) {
#ifdef BUILD_WITH_PROFILER
  EASY_PROFILER_ENABLE;
#endif
  int l(argc);
  argc--;
  while (argc--) {
    std::string path(argv[l - argc - 1]);
    SHA1Cal sha;
    sha.Init();
    std::ifstream fin(path, std::ios::binary);
    sha.FileStreamTransform(fin);
    printf(" %s\n", path.c_str());
  }
#ifdef BUILD_WITH_PROFILER
  profiler::dumpBlocksToFile("test.prof");
#endif
}
