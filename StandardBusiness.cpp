#include "StandardBusiness.h"
#include <utility>

StandardBusiness::StandardBusiness(std::string name, BusinessType type, double profit, double upgrade,
                                   double cost, double time, double mngCost)
    : Business(std::move(name), profit, upgrade, cost, time, mngCost), type(type) {}

std::unique_ptr<Business> StandardBusiness::clone() const {
    return std::make_unique<StandardBusiness>(*this);
}

double StandardBusiness::typeFactor() const {
    switch (type) {
        case BusinessType::LEMONADE:  return 1.5;
        case BusinessType::ICE_CREAM: return 1.2;
        case BusinessType::PIZZA:     return 1.1;
        case BusinessType::DONUT:     return 1.15;
        case BusinessType::SHRIMP:    return 1.25;
        case BusinessType::RESTAURANT:
        default:                      return 1.0;
    }
}

double StandardBusiness::calculateRevenue(double bonusMultiplier) const {
    if (type == BusinessType::RESTAURANT) {
        return baseRevenue(1.0 + (bonusMultiplier - 1.0) * 0.5);
    }
    return baseRevenue(bonusMultiplier) * typeFactor();
}

BusinessType StandardBusiness::getType() const {
    return type;
}

std::string StandardBusiness::getStatusLabel() const {
    return "Standard x" + std::to_string(typeFactor()).substr(0, 4);
}

void StandardBusiness::print(std::ostream& os) const {
    Business::print(os);
    os << " | Tip: " << getStatusLabel();
}