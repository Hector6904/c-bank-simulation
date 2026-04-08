# 🏦 C Bank Simulation

A modular banking system written in C that simulates real-world banking operations such as account management, transactions, and secure PIN handling.

---

## ✨ Features

- 🧑 Create and manage bank accounts  
- 🔐 Secure PIN hashing using SHA-256 + salt  
- 💳 Debit & Credit card simulation  
- 💸 Deposit, Withdraw, and Transfer money  
- 🧾 Transaction history & mini statements  
- 💰 Monthly interest calculation  
- 🔒 Account lock after multiple failed login attempts  
- 💾 Persistent storage using binary file (`bank.dat`)  

---

## 📁 Project Structure
c-bank-simulation/
├── src/
│ ├── main.c # Entry point
│ ├── account.c # Core banking logic
│ ├── transaction.c # Transaction handling
│ ├── file.c # File I/O (save/load)
│ └── utils.c # Helpers (hashing, random generation)
│
├── include/
│ ├── bank.h
│ ├── account.h
│ ├── transaction.h
│ ├── file.h
│ └── utils.h

---

## ⚙️ Requirements

- GCC compiler  
- OpenSSL (for hashing and secure random numbers)

### Install OpenSSL (Fedora)

```bash
sudo dnf install openssl-devel

```md
---

## 🛠️ How to Build

```bash
gcc src/*.c -Iinclude -lssl -lcrypto -o bank
````

---

##  How to Run

```bash
./bank
```

---

## Example Usage

1. Create an account
2. Login using account number and PIN
3. Perform transactions
4. View transaction history

---

##  Concepts Used

* Structures and arrays
* File handling (binary I/O)
* Modular programming (multi-file C project)
* Cryptography (SHA-256 hashing + salt)
* Secure random number generation

---

##  Notes

* `bank.dat` is generated at runtime to store account data
* Do not manually edit `bank.dat`
* This is a simulation project, not a real banking system

---

##  Future Improvements

* CLI enhancements
* Database integration
* GUI interface
* Better error handling

```
```

