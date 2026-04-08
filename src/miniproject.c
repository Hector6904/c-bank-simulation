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
    unsigned char pinHash[32];   // SHA-256 hash
    unsigned char salt[16];      // random salt
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
int accCount=0;
void addTransaction(int i, const char *msg);
void showTransactions(int i);

// -------- PIN MASKING ----------
int maskedPIN() {
    int pin;
    scanf("%d", &pin);
    return pin;
}

// -------- GENERATOR ----------
long long genNum(int d){
    long long n=0;
    for(int i=0;i<d;i++)
        n=n*10+rand()%10;
    return n;
}

// -------- FILE ----------
void save(){
    FILE *fp = fopen("bank.dat", "wb");
    if (!fp) {
        printf("Error opening file for saving!\n");
        return;
    }

    fwrite(&accCount, sizeof(int), 1, fp);
    fwrite(bank, sizeof(struct Account), accCount, fp);

    fclose(fp);
    printf("[DEBUG] Saved %d accounts\n", accCount);
}

void load(){
    FILE *fp = fopen("bank.dat", "rb");
    if (!fp) {
        printf("[DEBUG] No existing data found.\n");
        return;
    }

    fread(&accCount, sizeof(int), 1, fp);
    fread(bank, sizeof(struct Account), accCount, fp);

    fclose(fp);
    printf("[DEBUG] Loaded %d accounts\n", accCount);
}

// -------- FIND ----------
int find(long long n){
    for(int i=0;i<accCount;i++)
        if(bank[i].accNo==n) return i;
    return -1;
}

void hashPIN(int pin, unsigned char *salt, unsigned char *output){
    unsigned char input[32];
    memcpy(input, salt, 16);
    memcpy(input + 16, &pin, sizeof(int));
    SHA256(input, 16 + sizeof(int), output);
}
// -------- CREATE ----------
void create(){
    struct Account *a=&bank[accCount];

    printf("Name: ");
    scanf(" %[^\n]",a->name);

   printf("Set 4-digit PIN: ");
int pin = maskedPIN();

// generate random salt
for(int i=0;i<16;i++)
    a->salt[i] = rand()%256;

// hash pin + salt
hashPIN(pin, a->salt, a->pinHash);
    a->balance=0;
    a->accNo=genNum(12);
    a->debitCard=genNum(16);
    a->creditCard=genNum(16);
    a->creditLimit=50000;
    a->creditUsed=0;

    printf("\nAccNo:%lld",a->accNo);
    printf("\nDebit:%lld",a->debitCard);
    printf("\nCredit:%lld\n",a->creditCard);
     a->lastInterest = 0;   // means never given before
    a->txCount = 0;


    accCount++;
    save();
}

// -------- INTEREST ----------
void addInterest(int i){
    time_t now = time(NULL);

    if(bank[i].lastInterest == 0 || 
       difftime(now, bank[i].lastInterest) >= 30 * 24 * 60 * 60){

        float interest = bank[i].balance * 0.02;
        bank[i].balance += interest;
        bank[i].lastInterest = now;

        printf("2%% interest added: %.2f\n", interest);

        char msg[100];
        sprintf(msg, "Interest added: %.2f", interest);
        addTransaction(i, msg);
    } 
    else {
        printf("Interest already added this month.\n");
    }
}

// -------- ATM SWIPE ----------
void swipeDebit(int i){
    long long card;
    float amt;

    printf("Enter Debit Card: ");
    scanf("%lld",&card);

    if(card!=bank[i].debitCard){
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f",&amt);

    if(amt<=bank[i].balance){
        bank[i].balance-=amt;
        printf("Paid via Debit\n");
    } else printf("Low balance\n");
}

void swipeCredit(int i){
    long long card;
    float amt;

    printf("Enter Credit Card: ");
    scanf("%lld",&card);

    if(card!=bank[i].creditCard){
        printf("Invalid card\n");
        return;
    }

    printf("Amount: ");
    scanf("%f",&amt);

    if(bank[i].creditUsed+amt<=bank[i].creditLimit){
        bank[i].creditUsed+=amt;
        printf("Paid via Credit\n");
    } else printf("Limit exceeded\n");
}

// -------- BASIC BANK ----------
void deposit(int i){
    float a;
    scanf("%f",&a);
    bank[i].balance += a;

    char msg[100];
    sprintf(msg, "Deposited: %.2f", a);
    addTransaction(i, msg);
}

void withdraw(int i){
    float a;
    scanf("%f",&a);

    if(a <= bank[i].balance){
        bank[i].balance -= a;

        char msg[100];
        sprintf(msg, "Withdrew: %.2f", a);
        addTransaction(i, msg);
    }
}

void addTransaction(int i, const char *msg){
    if(bank[i].txCount < MAX_TX){
        strcpy(bank[i].transactions[bank[i].txCount], msg);
        bank[i].txCount++;
    }
}
void showTransactions(int i){
    if(bank[i].txCount == 0){
        printf("No transactions yet.\n");
        return;
    }

    printf("\n--- Transaction History ---\n");

    for(int j = 0; j < bank[i].txCount; j++){
        printf("%d. %s\n", j+1, bank[i].transactions[j]);
    }
}

// -------- HOME ----------
void home(int i){
    int c;
    do{
        printf("\n1.Balance\n2.Deposit\n3.Withdraw");
        printf("\n4.Swipe Debit\n5.Swipe Credit");
        printf("\n6.Transaction History");
        printf("\n7.Add Interest\n8.Logout\n");

        scanf("%d",&c);

        if(c==1)
            printf("Bal:%.2f CreditUsed:%.2f\n",
            bank[i].balance, bank[i].creditUsed);

        else if(c==2){ 
            printf("Amt:"); 
            deposit(i); 
        }

        else if(c==3){ 
            printf("Amt:"); 
            withdraw(i); 
        }

        else if(c==4) swipeDebit(i);

        else if(c==5) swipeCredit(i);

        else if(c==6) showTransactions(i);

        else if(c==7) addInterest(i);

        save();

    }while(c!=8);
}
// -------- LOGIN ----------
void login(){
    long long n;
    printf("AccNo: ");
    scanf("%lld",&n);

    int i = find(n);
    if(i == -1){
        printf("Account not found.\n");
        return;
    }

    int attempts = 0;
    while(attempts < 3){
        printf("PIN: ");
        int p = maskedPIN();

        unsigned char attemptHash[32];
        hashPIN(p, bank[i].salt, attemptHash);

        if(memcmp(attemptHash, bank[i].pinHash, 32) == 0){
            printf("Login successful.\n");
            home(i);
            return;
        }
        else{
            printf("Wrong PIN.\n");
            attempts++;
        }
    }

    printf("Too many failed attempts. Access denied.\n");
}

// -------- MAIN ----------
int main(){
    srand(time(NULL));
    load();

    int c;
    do{
        printf("\n1.Create\n2.Login\n3.Exit\n");
        printf("Choice: ");
        scanf("%d",&c);

        switch(c){
            case 1:
                create();
                break;
            case 2:
                login();
                break;
            case 3:
                printf("Exiting system.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }

    }while(c != 3);

    save();
    return 0;
}



