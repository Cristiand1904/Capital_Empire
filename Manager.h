#pragma once
#include <iostream>
#include <string>

class Manager {
private:
    std::string name;
    double cost;
    int level;

public:
    Manager(std::string name, double cost);

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] double getCost() const;
    [[nodiscard]] int getLevel() const;

    void upgrade();
    [[nodiscard]] double getDiscountFactor() const;

    friend std::ostream& operator<<(std::ostream& os, const Manager& m);
};