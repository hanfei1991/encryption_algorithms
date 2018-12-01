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
#include "aes/aes_ctr.h"

uint8_t iv[16] = {
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

uint8_t key[] = {
        0xC4,0xCA,0x42,0x38,0xA0,0xB9,0x23,0x82,0x0D,0xCC,0x50,0x9A,0x6F,0x75,0x84,0x9B,
};

int main()
{
    uint8_t input[16];
    uint8_t result[16];
    CTR ctr(iv, key);
    ctr.Do(input, result);
}
```