#include <stdio.h>
#include "bank.h"

void save() {
    FILE *fp = fopen("bank.dat", "wb");
    if (!fp) {
        printf("Error saving data\n");
        return;
    }

    fwrite(&accCount, sizeof(int), 1, fp);
    fwrite(bank, sizeof(struct Account), accCount, fp);

    fclose(fp);
}

void load() {
    FILE *fp = fopen("bank.dat", "rb");
    if (!fp) return;

    fread(&accCount, sizeof(int), 1, fp);
    fread(bank, sizeof(struct Account), accCount, fp);

    fclose(fp);
}

