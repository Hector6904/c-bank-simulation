# C Banking System

A terminal-based banking simulation written in C.

## Features
- Account creation and authentication
- SHA-256 + Salt password protection
- Deposit, Withdraw, Transfer
- Debit & Credit card simulation
- Transaction history & mini statement
- PIN change system

## Project Structure
- miniproject.c : main program
- src/ : supporting files/modules
- data/ : runtime data storage (not tracked)

## How to Run
gcc src/miniproject.c -o bank
./bank
