#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "utils.h"

int maskedPIN() {
    int pin;
    scanf("%d", &pin);
    return pin;
}

long long genNum(int d) {
    long long n = 0;
    unsigned char buf[8];
    RAND_bytes(buf, sizeof(buf));

    unsigned long long r = 0;
    for (int i = 0; i < 8; i++)
        r = r * 256 + buf[i];

    long long mod = 1;
    for (int i = 0; i < d; i++)
        mod *= 10;

    n = (r % mod);

    long long minVal = mod / 10;
    if (n < minVal)
        n += minVal;

    return n;
}

void hashPIN(int pin, unsigned char *salt, unsigned char *output) {
    unsigned char input[32];
    memcpy(input, salt, 16);
    memcpy(input + 16, &pin, sizeof(int));
    SHA256(input, 16 + sizeof(int), output);
}
