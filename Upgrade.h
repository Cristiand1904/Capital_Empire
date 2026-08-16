#pragma once
#include <iostream>

class Upgrade {
private:
    int requiredLevel;
    double costMultiplier;
    double profitMultiplier;
    bool purchased;

public:
    Upgrade(int requiredLevel, double costMultiplier, double profitMultiplier);

    [[nodiscard]] bool isPurchased() const;
    [[nodiscard]] int getRequiredLevel() const;
    [[nodiscard]] double getCostMultiplier() const;
    [[nodiscard]] double getMultiplier() const;

    void purchase();

    friend std::ostream& operator<<(std::ostream& os, const Upgrade& u);
};