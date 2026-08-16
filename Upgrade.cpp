#include "Upgrade.h"
#include <iomanip>

Upgrade::Upgrade(int requiredLevel, double costMultiplier, double profitMultiplier)
    : requiredLevel(requiredLevel), costMultiplier(costMultiplier),
      profitMultiplier(profitMultiplier), purchased(false) {}

bool Upgrade::isPurchased() const {
    return purchased;
}

int Upgrade::getRequiredLevel() const {
    return requiredLevel;
}

double Upgrade::getCostMultiplier() const {
    return costMultiplier;
}

double Upgrade::getMultiplier() const {
    return profitMultiplier;
}

void Upgrade::purchase() {
    purchased = true;
}

std::ostream& operator<<(std::ostream& os, const Upgrade& u) {
    os << std::fixed << std::setprecision(1);
    os << "Nivel " << u.getRequiredLevel();
    os << " | Cost x" << u.getCostMultiplier();
    os << " | Profit x" << u.getMultiplier();
    os << " | " << (u.isPurchased() ? "CUMPARAT" : "DISPONIBIL");
    return os;
}