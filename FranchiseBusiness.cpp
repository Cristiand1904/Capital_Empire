#include "FranchiseBusiness.h"
#include "Exceptions.h"
#include <iomanip>
#include <sstream>
#include <utility>

FranchiseBusiness::FranchiseBusiness(std::string name, double profit, double upgrade, double cost, double time,
                                     double mngCost, int levelsPerFranchise, double franchiseYield)
    : Business(std::move(name), profit, upgrade, cost, time, mngCost),
      levelsPerFranchise(levelsPerFranchise), franchiseYield(franchiseYield) {
    if (levelsPerFranchise <= 0) {
        throw InvalidBusinessDataException(getName(), "nivelele per franciza trebuie sa fie pozitive");
    }
    if (franchiseYield <= 0.0) {
        throw InvalidBusinessDataException(getName(), "randamentul unei francize trebuie sa fie pozitiv");
    }
}

std::unique_ptr<Business> FranchiseBusiness::clone() const {
    return std::make_unique<FranchiseBusiness>(*this);
}

int FranchiseBusiness::getFranchiseCount() const {
    return getLevel() / levelsPerFranchise;
}

int FranchiseBusiness::levelsUntilNextFranchise() const {
    return levelsPerFranchise - (getLevel() % levelsPerFranchise);
}

double FranchiseBusiness::getFranchiseMultiplier() const {
    return 1.0 + static_cast<double>(getFranchiseCount()) * franchiseYield;
}

double FranchiseBusiness::calculateRevenue(double bonusMultiplier) const {
    return baseRevenue(bonusMultiplier) * getFranchiseMultiplier();
}

std::string FranchiseBusiness::getStatusLabel() const {
    std::ostringstream label;
    label << std::fixed << std::setprecision(2);
    label << "Franciza x" << getFranchiseMultiplier() << " (" << getFranchiseCount() << " locatii)";
    return label.str();
}

void FranchiseBusiness::print(std::ostream& os) const {
    Business::print(os);
    os << " | " << getStatusLabel();
    os << " | Urmatoarea locatie peste " << levelsUntilNextFranchise() << " nivele";
}
