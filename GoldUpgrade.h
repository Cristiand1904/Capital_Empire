#pragma once
#include <iostream>
#include <optional>
#include <string>

enum class GoldEffect {
    PROFIT,
    DISCOUNT,
    SPEED,
    MANAGER_DISCOUNT,
    OFFLINE_RATIO,
    PRESTIGE_GOLD,
    BUSINESS_MULTIPLIER
};

class GoldUpgrade {
private:
    int id;
    int cost;
    GoldEffect effect;
    double value;
    std::string target;
    std::string label;
    std::string description;

public:
    GoldUpgrade(int id, int cost, GoldEffect effect, double value, std::string target,
                std::string label, std::string description);

    [[nodiscard]] int getId() const;
    [[nodiscard]] int getCost() const;
    [[nodiscard]] GoldEffect getEffect() const;
    [[nodiscard]] double getValue() const;
    [[nodiscard]] const std::string& getTarget() const;
    [[nodiscard]] const std::string& getLabel() const;
    [[nodiscard]] const std::string& getDescription() const;
    [[nodiscard]] std::string getButtonText() const;

    static std::optional<GoldEffect> effectFromString(const std::string& text);

    friend std::ostream& operator<<(std::ostream& os, const GoldUpgrade& u);
};
