# encryption_algorithms

Study some common encryption algos.

List some algorithms to implement
- [x] SHA-1
- [ ] SHA-256
- [ ] AES-128-ecb
- [ ] AES-256-ecb
- [ ] AES-128-cfb
- [x] AES-128-ctr
- [ ] AES-256-cfb

Todos:
- [ ] Base test cases

Currently the aes-128-ctr algorithm is working good. but it's still in **experimental**. Be careful for usage.

The project is built in C++17 and will compile on Windows, Mac, Ios, Android. Just cpp codes.

This project is not focus on speed, but study. And I want to use this project by myself later, so it'll be easy to integration later.

```cpp
#include "aes_ctr.h"

uint8_t iv[16] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  };

  uint8_t key[] = {
      0xC4, 0xCA, 0x42, 0x38, 0xA0, 0xB9, 0x23, 0x82,
      0x0D, 0xCC, 0x50, 0x9A, 0x6F, 0x75, 0x84, 0x9B,
  };

  uint8_t out[16];

  unsigned char input[] = {0x2b, 0x41, 0x5d, 0x44, 0x59, 0x6e, 0x3a, 0x28,
                           0xe5, 0x08, 0xaf, 0x0b, 0x13, 0xb4, 0x00, 0xc2,

                           0x49, 0x6f, 0x23, 0x95, 0xb6, 0x88, 0xe0, 0x41,
                           0x17, 0x44, 0x64, 0x4b, 0x96, 0x37, 0x92, 0xc3};

  CTR ctr(iv, key);
  // Do every block for ctr, and ctr support all 0xff iv, and iv will go to all
  // 0x00 from one.
  ctr.Do(input, out);

  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");

  ctr.Do(input + 16, out);

  printf("msg:\n");
  for (int i = 0; i < 4; i++) {
    printf("%02x%02x%02x%02x ", out[4 * i + 0], out[4 * i + 1], out[4 * i + 2],
           out[4 * i + 3]);
  }
  puts("");
```