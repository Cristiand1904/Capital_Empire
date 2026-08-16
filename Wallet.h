#pragma once
#include <iostream>
#include "Exceptions.h"

class Wallet {
private:
    double money;

public:
    explicit Wallet(double initialMoney = 0.0);

    [[nodiscard]] double getMoney() const;
    [[nodiscard]] bool canAfford(double amount) const;
    void addMoney(double amount);
    void spendMoney(double amount);

    friend std::ostream& operator<<(std::ostream& os, const Wallet& w);
};