#pragma once
#include "Business.h"

class FranchiseBusiness : public Business {
private:
    int levelsPerFranchise;
    double franchiseYield;

protected:
    void print(std::ostream& os) const override;

public:
    FranchiseBusiness(std::string name, double profit, double upgrade, double cost, double time,
                      double mngCost, int levelsPerFranchise, double franchiseYield);

    [[nodiscard]] std::unique_ptr<Business> clone() const override;
    [[nodiscard]] double calculateRevenue(double bonusMultiplier) const override;
    [[nodiscard]] std::string getStatusLabel() const override;

    [[nodiscard]] int getFranchiseCount() const;
    [[nodiscard]] int levelsUntilNextFranchise() const;
    [[nodiscard]] double getFranchiseMultiplier() const;
};
