#pragma once
#include <iostream>
#include <string>

class GoldUpgrade {
private:
    int id;
    int cost;
    std::string label;
    std::string description;

public:
    GoldUpgrade(int id, int cost, std::string label, std::string description);

    [[nodiscard]] int getId() const;
    [[nodiscard]] int getCost() const;
    [[nodiscard]] const std::string& getLabel() const;
    [[nodiscard]] const std::string& getDescription() const;
    [[nodiscard]] std::string getButtonText() const;

    friend std::ostream& operator<<(std::ostream& os, const GoldUpgrade& u);
};