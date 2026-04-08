#include <stdio.h>
#include "bank.h"
#include "account.h"
#include "file.h"

struct Account bank[MAX];
int accCount = 0;

int main() {
    load();

    int c;

    do {
        printf("\n1. Create Account\n2. Login\n3. Exit\n");
        scanf("%d", &c);

        switch (c) {
            case 1: create(); break;
            case 2: login();  break;
            case 3: printf("Exiting...\n"); break;
            default: printf("Invalid choice\n");
        }

    } while (c != 3);

    save();
    return 0;
}