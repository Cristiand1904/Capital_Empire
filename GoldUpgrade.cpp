#include "GoldUpgrade.h"
#include <utility>

GoldUpgrade::GoldUpgrade(int id, int cost, GoldEffect effect, double value, std::string target,
                         std::string label, std::string description)
    : id(id), cost(cost), effect(effect), value(value), target(std::move(target)),
      label(std::move(label)), description(std::move(description)) {}

int GoldUpgrade::getId() const {
    return id;
}

int GoldUpgrade::getCost() const {
    return cost;
}

GoldEffect GoldUpgrade::getEffect() const {
    return effect;
}

double GoldUpgrade::getValue() const {
    return value;
}

const std::string& GoldUpgrade::getTarget() const {
    return target;
}

const std::string& GoldUpgrade::getLabel() const {
    return label;
}

const std::string& GoldUpgrade::getDescription() const {
    return description;
}

std::string GoldUpgrade::getButtonText() const {
    return label + " (" + std::to_string(cost) + " Gold)";
}

std::optional<GoldEffect> GoldUpgrade::effectFromString(const std::string& text) {
    if (text == "PROFIT") return GoldEffect::PROFIT;
    if (text == "DISCOUNT") return GoldEffect::DISCOUNT;
    if (text == "SPEED") return GoldEffect::SPEED;
    if (text == "MANAGER_DISCOUNT") return GoldEffect::MANAGER_DISCOUNT;
    if (text == "OFFLINE_RATIO") return GoldEffect::OFFLINE_RATIO;
    if (text == "PRESTIGE_GOLD") return GoldEffect::PRESTIGE_GOLD;
    if (text == "BUSINESS") return GoldEffect::BUSINESS_MULTIPLIER;
    return std::nullopt;
}

std::ostream& operator<<(std::ostream& os, const GoldUpgrade& u) {
    os << "[" << u.getId() << "] " << u.getLabel() << " - " << u.getCost() << " gold";
    os << " | " << u.getDescription();
    return os;
}
