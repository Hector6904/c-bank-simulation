#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "utils.h"
#include "ui.h"

/* ── Safe integer input ─────────────────────────────────────────────────────
 * Reads a line, parses it as int.
 * Returns 1 on success, 0 on bad input (non-numeric / overflow).
 * This replaces every raw scanf("%d") in the project.
 * -------------------------------------------------------------------------- */
int read_int(int *out) {
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    char *end;
    long val = strtol(buf, &end, 10);
    /* end must point to '\n' or '\0', not a letter */
    if (end == buf || (*end != '\n' && *end != '\0')) return 0;
    *out = (int)val;
    return 1;
}

/* ── Safe float input ───────────────────────────────────────────────────── */
int read_double(double *out) {
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    char *end;
    double val = strtod(buf, &end);
    if (end == buf || (*end != '\n' && *end != '\0')) return 0;
    *out = val;
    return 1;
}

/* ── PIN reader with validation ─────────────────────────────────────────────
 * Returns the 4-digit PIN, or -1 on invalid input.
 * -------------------------------------------------------------------------- */
int maskedPIN(void) {
    int pin;
    if (!read_int(&pin)) return -1;
    if (pin < PIN_MIN || pin > PIN_MAX) return -1;
    return pin;
}

/* ── Cryptographically secure N-digit number ────────────────────────────── */
long long genNum(int d) {
    unsigned char buf[8];
    RAND_bytes(buf, sizeof(buf));

    unsigned long long r = 0;
    for (int i = 0; i < 8; i++)
        r = r * 256 + buf[i];

    long long mod = 1;
    for (int i = 0; i < d; i++) mod *= 10;

    long long n = (long long)(r % (unsigned long long)mod);
    long long minVal = mod / 10;
    if (n < minVal) n += minVal;
    return n;
}

/* ── SHA-256 PIN hash with salt ─────────────────────────────────────────── */
void hashPIN(int pin, unsigned char *salt, unsigned char *output) {
    unsigned char input[20];          /* 16-byte salt + 4-byte pin */
    memcpy(input, salt, 16);
    memcpy(input + 16, &pin, sizeof(int));
    SHA256(input, 16 + sizeof(int), output);
}
