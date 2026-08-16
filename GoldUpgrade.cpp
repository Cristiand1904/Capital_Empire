#include "GoldUpgrade.h"
#include <utility>

GoldUpgrade::GoldUpgrade(int id, int cost, std::string label, std::string description)
    : id(id), cost(cost), label(std::move(label)), description(std::move(description)) {}

int GoldUpgrade::getId() const {
    return id;
}

int GoldUpgrade::getCost() const {
    return cost;
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

std::ostream& operator<<(std::ostream& os, const GoldUpgrade& u) {
    os << "[" << u.getId() << "] " << u.getLabel() << " - " << u.getCost() << " gold";
    os << " | " << u.getDescription();
    return os;
}