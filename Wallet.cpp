#include "Wallet.h"
#include <iomanip>

Wallet::Wallet(double initialMoney) : money(initialMoney) {}

double Wallet::getMoney() const {
    return money;
}

bool Wallet::canAfford(double amount) const {
    return money >= amount;
}

void Wallet::addMoney(double amount) {
    money += amount;
}

void Wallet::spendMoney(double amount) {
    if (!canAfford(amount)) {
        throw InsufficientFundsException(amount, money);
    }
    money -= amount;
}

std::ostream& operator<<(std::ostream& os, const Wallet& w) {
    os << std::fixed << std::setprecision(0) << w.getMoney() << "$";
    return os;
}