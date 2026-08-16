#include "PremiumBusiness.h"
#include "Exceptions.h"
#include <iomanip>
#include <sstream>
#include <utility>

PremiumBusiness::PremiumBusiness(std::string name, double profit, double upgrade, double cost,
                                 double time, double mngCost, double luxuryFactor)
    : Business(std::move(name), profit, upgrade, cost, time, mngCost),
      luxuryFactor(luxuryFactor), loyaltyProgress(0.0), loyaltyPoints(0) {
    if (luxuryFactor < 1.0) {
        throw InvalidBusinessDataException(getName(), "factorul de lux trebuie sa fie cel putin 1.0");
    }
}

std::unique_ptr<Business> PremiumBusiness::clone() const {
    return std::make_unique<PremiumBusiness>(*this);
}

double PremiumBusiness::getLoyaltyBonus() const {
    return 1.0 + static_cast<double>(loyaltyPoints) * 0.002;
}

double PremiumBusiness::calculateRevenue(double bonusMultiplier) const {
    return baseRevenue(bonusMultiplier) * luxuryFactor * getLoyaltyBonus();
}

bool PremiumBusiness::accumulateLoyalty(double seconds) {
    if (!isOwned() || getLevel() <= 10) {
        return false;
    }
    const int before = loyaltyPoints / 100;
    loyaltyProgress += seconds * static_cast<double>(getLevel() - 10);
    while (loyaltyProgress >= 1.0) {
        loyaltyProgress -= 1.0;
        loyaltyPoints++;
    }
    return loyaltyPoints / 100 > before;
}

void PremiumBusiness::addLoyaltyPoints(int points) {
    if (points > 0) {
        loyaltyPoints += points;
    }
}

int PremiumBusiness::getLoyaltyPoints() const {
    return loyaltyPoints;
}

double PremiumBusiness::getLuxuryFactor() const {
    return luxuryFactor;
}

std::string PremiumBusiness::getStatusLabel() const {
    std::ostringstream label;
    label << std::fixed << std::setprecision(2);
    label << "Premium x" << luxuryFactor << " | Loialitate " << loyaltyPoints;
    return label.str();
}

void PremiumBusiness::print(std::ostream& os) const {
    Business::print(os);
    os << " | " << getStatusLabel();
}