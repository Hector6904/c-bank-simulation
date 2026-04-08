#include <stdio.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"

void addTransaction(int i, const char *msg) {
    if (bank[i].txCount < MAX_TX) {
        strncpy(bank[i].transactions[bank[i].txCount], msg, 99);
        bank[i].transactions[bank[i].txCount][99] = '\0';
        bank[i].txCount++;
    } else {
        printf("Warning: Transaction log full. Oldest entries are no longer recorded.\n");
    }
}

void showTransactions(int i) {
    printf("\n--- Transaction History ---\n");

    if (bank[i].txCount == 0) {
        printf("No transactions yet.\n");
        return;
    }

    for (int j = 0; j < bank[i].txCount; j++) {
        printf("%d. %s\n", j + 1, bank[i].transactions[j]);
    }
}

void miniStatement(int i) {
    printf("\n--- Mini Statement ---\n");

    int start = bank[i].txCount - 5;
    if (start < 0) start = 0;

    for (int j = start; j < bank[i].txCount; j++) {
        printf("%d. %s\n", j + 1, bank[i].transactions[j]);
    }
}
