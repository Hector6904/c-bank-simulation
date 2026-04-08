#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bank.h"
#include "utils.h"
#include "transaction.h"
#include "file.h"
#include "account.h"
#include <openssl/rand.h>

/* ---------- INTEREST ---------- */

void processInterest(int i) {
    time_t now = time(NULL);
    if (bank[i].lastInterest == 0 ||
        difftime(now, bank[i].lastInterest) >= 30 * 24 * 60 * 60) {

        float rate;
        if (bank[i].balance < 10000) rate = 0.01f;
        else if (bank[i].balance < 50000) rate = 0.02f;
        else rate = 0.03f;

        float interest = bank[i].balance * rate;
        bank[i].balance += interest;
        bank[i].lastInterest = now;

        char msg[100];
        snprintf(msg, sizeof(msg), "Interest added: %.2f", interest);
        addTransaction(i, msg);
    }
}

void changePIN(int i) {
    int oldPin, newPin;

    printf("Enter current PIN: ");
    oldPin = maskedPIN();

    unsigned char hash[32];
    hashPIN(oldPin, bank[i].salt, hash);

    if (memcmp(hash, bank[i].pinHash, 32) != 0) {
        printf("Wrong PIN\n");
        return;
    }

    printf("Enter new PIN: ");
    newPin = maskedPIN();

    hashPIN(newPin, bank[i].salt, bank[i].pinHash);
    printf("PIN updated successfully\n");
}

int find(long long n) {
    for (int i = 0; i < accCount; i++)
        if (bank[i].accNo == n) return i;
    return -1;
}

void deposit(int i) {
    float a;
    printf("Amount: ");
    scanf("%f", &a);

    if (a <= 0) {
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance += a;

    char msg[100];
    snprintf(msg, sizeof(msg), "Deposited: %.2f", a);
    addTransaction(i, msg);
}

void withdraw(int i) {
    float a;
    printf("Amount: ");
    scanf("%f", &a);

    if (a <= 0 || a > bank[i].balance) {
        printf("Invalid amount\n");
        return;
    }

    if (a > 20000)
        printf("Warning: Large transaction alert!\n");

    bank[i].balance -= a;

    char msg[100];
    snprintf(msg, sizeof(msg), "Withdrew: %.2f", a);
    addTransaction(i, msg);
}

/* ---------- TRANSFER ---------- */
void transfer(int i) {
    long long target;
    float amt;

    printf("Enter receiver AccNo: ");
    scanf("%lld", &target);

    int j = find(target);
    if (j == -1) {
        printf("Receiver not found\n");
        return;
    }

    if (j == i) {
        printf("Cannot transfer to your own account\n");
        return;
    }

    printf("Amount: ");
    scanf("%f", &amt);

    if (amt <= 0 || amt > bank[i].balance) {
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance -= amt;
    bank[j].balance += amt;

    char msg1[100], msg2[100];
    snprintf(msg1, sizeof(msg1), "Sent %.2f to %lld", amt, target);
    snprintf(msg2, sizeof(msg2), "Received %.2f from %lld", amt, bank[i].accNo);

    addTransaction(i, msg1);
    addTransaction(j, msg2);

    printf("Transfer successful\n");
}

/* ---------- CARDS ---------- */
void swipeDebit(int i) {
    long long card;
    float amt;

    printf("Enter Debit Card: ");
    scanf("%lld", &card);

    if (card != bank[i].debitCard) {
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f", &amt);

    if (amt <= 0) {
        printf("Invalid amount\n");
        return;
    }

    if (amt <= bank[i].balance) {
        bank[i].balance -= amt;

        char msg[100];
        snprintf(msg, sizeof(msg), "Debit card payment: %.2f", amt);
        addTransaction(i, msg);

        printf("Paid via Debit\n");
    } else {
        printf("Low balance\n");
    }
}

void swipeCredit(int i) {
    long long card;
    float amt;

    printf("Enter Credit Card: ");
    scanf("%lld", &card);

    if (card != bank[i].creditCard) {
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f", &amt);

    if (amt <= 0) {
        printf("Invalid amount\n");
        return;
    }

    if (bank[i].creditUsed + amt <= bank[i].creditLimit) {
        bank[i].creditUsed += amt;

        char msg[100];
        snprintf(msg, sizeof(msg), "Credit card payment: %.2f", amt);
        addTransaction(i, msg);

        printf("Paid via Credit\n");
    } else {
        printf("Limit exceeded\n");
    }
}

/* ---------- CREDIT PAYMENT ---------- */
void payCredit(int i) {
    float amt;

    printf("Enter amount: ");
    scanf("%f", &amt);

    if (amt <= 0 || amt > bank[i].balance || amt > bank[i].creditUsed) {
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance -= amt;
    bank[i].creditUsed -= amt;

    printf("Credit bill paid\n");

    char msg[100];
    snprintf(msg, sizeof(msg), "Paid credit bill: %.2f", amt);
    addTransaction(i, msg);
}
/* ---------- CREATE ---------- */
void create() {
    if (accCount >= MAX) {
        printf("Bank is full. Cannot create more accounts.\n");
        return;
    }

    struct Account *a = &bank[accCount];

    printf("Name: ");
    /* FIX #8: limit name input to prevent buffer overflow */
    scanf(" %49[^\n]", a->name);

    printf("Set 4-digit PIN: ");
    int pin = maskedPIN();

    /* FIX #6: use RAND_bytes for salt generation */
    if (RAND_bytes(a->salt, 16) != 1) {
        printf("Error generating secure random data\n");
        return;
    }

    hashPIN(pin, a->salt, a->pinHash);

    a->balance = 0;
    a->accNo = genNum(12);
    a->debitCard = genNum(16);
    a->creditCard = genNum(16);
    a->creditLimit = 50000;
    a->creditUsed = 0;
    a->lastInterest = 0;
    a->txCount = 0;
    a->locked = 0;  /* FIX #7 */

    printf("\nAccNo:  %lld\n", a->accNo);
    printf("Debit:  %lld\n", a->debitCard);
    printf("Credit: %lld\n", a->creditCard);

    accCount++;
    save();
}
/* ---------- HOME ---------- */
void home(int i) {
    int c;

    do {
        processInterest(i);

        printf("\n1.  Check Balance\n");
        printf("2.  Deposit Money\n");
        printf("3.  Withdraw Money\n");
        printf("4.  Transfer Money\n");
        printf("5.  Swipe Debit Card\n");
        printf("6.  Swipe Credit Card\n");
        printf("7.  Pay Credit Bill\n");
        printf("8.  Transaction History\n");
        printf("9.  Mini Statement\n");
        printf("10. Change PIN\n");
        printf("11. Logout\n");

        scanf("%d", &c);

        switch (c) {
            case 1:
                printf("Balance: %.2f | Credit Used: %.2f / %.2f\n",
                       bank[i].balance, bank[i].creditUsed, bank[i].creditLimit);
                break;
            case 2:  deposit(i);       break;
            case 3:  withdraw(i);      break;
            case 4:  transfer(i);      break;
            case 5:  swipeDebit(i);    break;
            case 6:  swipeCredit(i);   break;
            case 7:  payCredit(i);     break;
            case 8:  showTransactions(i); break;
            case 9:  miniStatement(i); break;
            case 10: changePIN(i);     break;
            case 11: printf("Logging out...\n"); break;
            default: printf("Invalid choice\n");
        }

        save();

    } while (c != 11);
}
/* ---------- LOGIN ---------- */
void login() {
    long long n;
    printf("AccNo: ");
    scanf("%lld", &n);

    int i = find(n);
    if (i == -1) {
        printf("Account not found\n");
        return;
    }

    /* FIX #7: check persistent lockout */
    if (bank[i].locked) {
        printf("This account is locked due to too many failed PIN attempts.\n");
        return;
    }

    int attempts = 0;

    while (attempts < 3) {
        printf("PIN: ");
        int p = maskedPIN();

        unsigned char attemptHash[32];
        hashPIN(p, bank[i].salt, attemptHash);

        if (memcmp(attemptHash, bank[i].pinHash, 32) == 0) {
            printf("Login successful\n");
            home(i);
            return;
        } else {
            attempts++;
            printf("Wrong PIN (%d/3)\n", attempts);
        }
    }

    /* FIX #7: lock account persistently after 3 failures and save */
    bank[i].locked = 1;
    save();
    printf("Too many attempts. Account locked.\n");
}

