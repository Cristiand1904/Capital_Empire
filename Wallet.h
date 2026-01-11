#pragma once
#include "Exceptions.h"

class Wallet {
private:
    double money;

public:
    explicit Wallet(double initialMoney = 0.0);

    [[nodiscard]] double getMoney() const;
    void addMoney(double amount);
    void spendMoney(double amount);
};