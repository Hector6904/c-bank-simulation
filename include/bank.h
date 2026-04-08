#ifndef BANK_H
#define BANK_H

#include <time.h>

#define MAX 100
#define MAX_TX 100

struct Account {
    long long accNo;
    char name[50];
    unsigned char pinHash[32];
    unsigned char salt[16];
    float balance;

    long long debitCard;
    long long creditCard;
    float creditLimit;
    float creditUsed;

    time_t lastInterest;
    int locked;

    char transactions[MAX_TX][100];
    int txCount;
};

extern struct Account bank[MAX];
extern int accCount;

#endif
