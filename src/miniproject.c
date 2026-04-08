#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX 100
#define MAX_TX 100

struct Account{
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

    char transactions[MAX_TX][100];
    int txCount;
};

struct Account bank[MAX];
int accCount = 0;

// ---------- HELPERS ----------
int maskedPIN(){
    int pin;
    scanf("%d",&pin);
    return pin;
}

long long genNum(int d){
    long long n=0;
    for(int i=0;i<d;i++)
        n = n*10 + rand()%10;
    return n;
}

void hashPIN(int pin, unsigned char *salt, unsigned char *output){
    unsigned char input[32];
    memcpy(input, salt, 16);
    memcpy(input + 16, &pin, sizeof(int));
    SHA256(input, 16 + sizeof(int), output);
}

// ---------- FILE ----------
void save(){
    FILE *fp = fopen("bank.dat","wb");
    fwrite(&accCount,sizeof(int),1,fp);
    fwrite(bank,sizeof(struct Account),accCount,fp);
    fclose(fp);
}

void load(){
    FILE *fp = fopen("bank.dat","rb");
    if(fp){
        fread(&accCount,sizeof(int),1,fp);
        fread(bank,sizeof(struct Account),accCount,fp);
        fclose(fp);
    }
}

// ---------- FIND ----------
int find(long long n){
    for(int i=0;i<accCount;i++)
        if(bank[i].accNo==n) return i;
    return -1;
}

// ---------- TRANSACTIONS ----------
void addTransaction(int i, const char *msg){
    if(bank[i].txCount < MAX_TX){
        strcpy(bank[i].transactions[bank[i].txCount++], msg);
    }
}

void showTransactions(int i){
    printf("\n--- Transaction History ---\n");
    if(bank[i].txCount == 0){
        printf("No transactions yet.\n");
        return;
    }
    for(int j=0;j<bank[i].txCount;j++){
        printf("%d. %s\n", j+1, bank[i].transactions[j]);
    }
}

void miniStatement(int i){
    printf("\n--- Mini Statement ---\n");

    int start = bank[i].txCount - 5;
    if(start < 0) start = 0;

    for(int j=start;j<bank[i].txCount;j++){
        printf("%d. %s\n", j+1, bank[i].transactions[j]);
    }
}

// ---------- INTEREST ----------
void processInterest(int i){
    time_t now = time(NULL);

    if(bank[i].lastInterest == 0 || 
       difftime(now, bank[i].lastInterest) >= 30*24*60*60){

        float rate;
        if(bank[i].balance < 10000) rate = 0.01;
        else if(bank[i].balance < 50000) rate = 0.02;
        else rate = 0.03;

        float interest = bank[i].balance * rate;
        bank[i].balance += interest;
        bank[i].lastInterest = now;

        char msg[100];
        sprintf(msg,"Interest added: %.2f",interest);
        addTransaction(i,msg);
    }
}

// ---------- BASIC ----------
void deposit(int i){
    float a;
    scanf("%f",&a);

    if(a <= 0){
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance += a;

    char msg[100];
    sprintf(msg,"Deposited: %.2f",a);
    addTransaction(i,msg);
}

void withdraw(int i){
    float a;
    scanf("%f",&a);

    if(a <= 0 || a > bank[i].balance){
        printf("Invalid amount\n");
        return;
    }

    if(a > 20000){
        printf("⚠ Large transaction alert!\n");
    }

    bank[i].balance -= a;

    char msg[100];
    sprintf(msg,"Withdrew: %.2f",a);
    addTransaction(i,msg);
}

// ---------- TRANSFER ----------
void transfer(int i){
    long long target;
    float amt;

    printf("Enter receiver AccNo: ");
    scanf("%lld",&target);

    int j = find(target);
    if(j == -1){
        printf("Receiver not found\n");
        return;
    }

    printf("Amount: ");
    scanf("%f",&amt);

    if(amt <= 0 || amt > bank[i].balance){
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance -= amt;
    bank[j].balance += amt;

    char msg1[100], msg2[100];
    sprintf(msg1,"Sent %.2f to %lld",amt,target);
    sprintf(msg2,"Received %.2f from %lld",amt,bank[i].accNo);

    addTransaction(i,msg1);
    addTransaction(j,msg2);

    printf("Transfer successful\n");
}

// ---------- CARDS ----------
void swipeDebit(int i){
    long long card;
    float amt;

    printf("Enter Debit Card: ");
    scanf("%lld",&card);

    if(card != bank[i].debitCard){
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f",&amt);

    if(amt <= bank[i].balance){
        bank[i].balance -= amt;
        printf("Paid via Debit\n");
    } else printf("Low balance\n");
}

void swipeCredit(int i){
    long long card;
    float amt;

    printf("Enter Credit Card: ");
    scanf("%lld",&card);

    if(card != bank[i].creditCard){
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f",&amt);

    if(bank[i].creditUsed + amt <= bank[i].creditLimit){
        bank[i].creditUsed += amt;
        printf("Paid via Credit\n");
    } else printf("Limit exceeded\n");
}

// ---------- CREDIT PAYMENT ----------
void payCredit(int i){
    float amt;

    printf("Enter amount: ");
    scanf("%f",&amt);

    if(amt <= 0 || amt > bank[i].balance || amt > bank[i].creditUsed){
        printf("Invalid amount\n");
        return;
    }

    bank[i].balance -= amt;
    bank[i].creditUsed -= amt;

    printf("Credit bill paid\n");

    char msg[100];
    sprintf(msg,"Paid credit bill: %.2f",amt);
    addTransaction(i,msg);
}

// ---------- CHANGE PIN ----------
void changePIN(int i){
    int oldPin, newPin;

    printf("Enter current PIN: ");
    oldPin = maskedPIN();

    unsigned char hash[32];
    hashPIN(oldPin, bank[i].salt, hash);

    if(memcmp(hash, bank[i].pinHash, 32) != 0){
        printf("Wrong PIN\n");
        return;
    }

    printf("Enter new PIN: ");
    newPin = maskedPIN();

    hashPIN(newPin, bank[i].salt, bank[i].pinHash);

    printf("PIN updated successfully\n");
}

// ---------- CREATE ----------
void create(){
    struct Account *a = &bank[accCount];

    printf("Name: ");
    scanf(" %[^\n]",a->name);

    printf("Set 4-digit PIN: ");
    int pin = maskedPIN();

    for(int i=0;i<16;i++)
        a->salt[i] = rand()%256;

    hashPIN(pin, a->salt, a->pinHash);

    a->balance = 0;
    a->accNo = genNum(12);
    a->debitCard = genNum(16);
    a->creditCard = genNum(16);
    a->creditLimit = 50000;
    a->creditUsed = 0;
    a->lastInterest = 0;
    a->txCount = 0;

    printf("\nAccNo:%lld",a->accNo);
    printf("\nDebit:%lld",a->debitCard);
    printf("\nCredit:%lld\n",a->creditCard);

    accCount++;
    save();
}

// ---------- HOME ----------
void home(int i){
    int c;

    do{
        processInterest(i);

        printf("\n1. Check Balance");
        printf("\n2. Deposit Money");
        printf("\n3. Withdraw Money");
        printf("\n4. Transfer Money");
        printf("\n5. Swipe Debit Card");
        printf("\n6. Swipe Credit Card");
        printf("\n7. Pay Credit Bill");
        printf("\n8. Transaction History");
        printf("\n9. Mini Statement");
        printf("\n10. Change PIN");
        printf("\n11. Logout\n");

        scanf("%d",&c);

        switch(c){
            case 1:
                printf("Balance: %.2f | Credit Used: %.2f\n",
                bank[i].balance, bank[i].creditUsed);
                break;

            case 2:
                printf("Amount: ");
                deposit(i);
                break;

            case 3:
                printf("Amount: ");
                withdraw(i);
                break;

            case 4:
                transfer(i);
                break;

            case 5:
                swipeDebit(i);
                break;

            case 6:
                swipeCredit(i);
                break;

            case 7:
                payCredit(i);
                break;

            case 8:
                showTransactions(i);
                break;

            case 9:
                miniStatement(i);
                break;

            case 10:
                changePIN(i);
                break;

            case 11:
                printf("Logging out...\n");
                break;

            default:
                printf("Invalid choice\n");
        }

        save();

    }while(c != 11);
}

// ---------- LOGIN ----------
void login(){
    long long n;
    printf("AccNo: ");
    scanf("%lld",&n);

    int i = find(n);
    if(i == -1){
        printf("Account not found\n");
        return;
    }

    int attempts = 0;

    while(attempts < 3){
        printf("PIN: ");
        int p = maskedPIN();

        unsigned char attemptHash[32];
        hashPIN(p, bank[i].salt, attemptHash);

        if(memcmp(attemptHash, bank[i].pinHash, 32) == 0){
            printf("Login successful\n");
            home(i);
            return;
        } else {
            printf("Wrong PIN\n");
            attempts++;
        }
    }

    printf("Too many attempts\n");
}

// ---------- MAIN ----------
int main(){
    srand(time(NULL));
    load();

    int c;

    do{
        printf("\n1. Create Account\n2. Login\n3. Exit\n");
        scanf("%d",&c);

        switch(c){
            case 1: create(); break;
            case 2: login(); break;
            case 3: printf("Exiting...\n"); break;
            default: printf("Invalid choice\n");
        }

    }while(c != 3);

    save();
    return 0;
}