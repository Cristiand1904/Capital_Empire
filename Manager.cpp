#include "Manager.h"
#include <cmath>
#include <iomanip>
#include <utility>

Manager::Manager(std::string name, double cost)
    : name(std::move(name)), cost(cost), level(1) {}

const std::string& Manager::getName() const {
    return name;
}

double Manager::getCost() const {
    return cost;
}

int Manager::getLevel() const {
    return level;
}

void Manager::upgrade() {
    level++;
    cost *= 5.0;
}

double Manager::getDiscountFactor() const {
    return std::pow(0.98, level - 1);
}

std::ostream& operator<<(std::ostream& os, const Manager& m) {
    os << std::fixed << std::setprecision(0);
    os << m.getName() << " (Lvl " << m.getLevel() << ")";
    os << " | Upgrade: " << m.getCost() << "$";
    os << " | Reducere: " << std::setprecision(1) << (1.0 - m.getDiscountFactor()) * 100.0 << "%";
    return os;
}