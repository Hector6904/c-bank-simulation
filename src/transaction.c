#include <stdio.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"
#include "ui.h"

/* ── Add a transaction entry ──────────────────────────────────────────────── */
void addTransaction(int i, const char *msg) {
    if (bank[i].txCount < MAX_TX) {
        strncpy(bank[i].transactions[bank[i].txCount], msg, TX_MSG_LEN - 1);
        bank[i].transactions[bank[i].txCount][TX_MSG_LEN - 1] = '\0';
        bank[i].txCount++;
    } else {
        print_warning("Transaction log full. Oldest entry overwritten.");
        /* Shift entries left to make room — circular-buffer style */
        memmove(bank[i].transactions[0],
                bank[i].transactions[1],
                (MAX_TX - 1) * TX_MSG_LEN);
        strncpy(bank[i].transactions[MAX_TX - 1], msg, TX_MSG_LEN - 1);
        bank[i].transactions[MAX_TX - 1][TX_MSG_LEN - 1] = '\0';
    }
}

/* ── Table header / footer helpers ─────────────────────────────────────────── */
static void print_tx_table_header(void) {
    printf("\n");
    printf(CYAN "  ┌─────┬────────────────────────────────────────────────────┐\n" RESET);
    printf(CYAN "  │" RESET BOLD " No. │ Description                                        " CYAN "│\n" RESET);
    printf(CYAN "  ├─────┼────────────────────────────────────────────────────┤\n" RESET);
}

static void print_tx_table_footer(int count) {
    printf(CYAN "  └─────┴────────────────────────────────────────────────────┘\n" RESET);
    printf(DIM  "  Total shown: %d\n" RESET, count);
}

/* ── Full history ────────────────────────────────────────────────────────── */
void showTransactions(int i) {
    print_section("FULL TRANSACTION HISTORY");

    if (bank[i].txCount == 0) {
        print_info("No transactions recorded yet.");
        return;
    }

    print_tx_table_header();
    for (int j = 0; j < bank[i].txCount; j++)
        print_tx_row(j + 1, bank[i].transactions[j]);
    print_tx_table_footer(bank[i].txCount);
}

/* ── Mini statement (last 5) ─────────────────────────────────────────────── */
void miniStatement(int i) {
    print_section("MINI STATEMENT  (Last 5 Transactions)");

    if (bank[i].txCount == 0) {
        print_info("No transactions recorded yet.");
        return;
    }

    int start = bank[i].txCount - 5;
    if (start < 0) start = 0;
    int shown = bank[i].txCount - start;

    print_tx_table_header();
    for (int j = start; j < bank[i].txCount; j++)
        print_tx_row(j + 1, bank[i].transactions[j]);
    print_tx_table_footer(shown);
}
