#pragma once
#include "Business.h"

class PremiumBusiness : public Business {
private:
    double luxuryFactor;
    double loyaltyProgress;
    int loyaltyPoints;

protected:
    void print(std::ostream& os) const override;

public:
    PremiumBusiness(std::string name, double profit, double upgrade, double cost,
                    double time, double mngCost, double luxuryFactor);

    [[nodiscard]] std::unique_ptr<Business> clone() const override;
    [[nodiscard]] double calculateRevenue(double bonusMultiplier) const override;
    [[nodiscard]] std::string getStatusLabel() const override;

    bool accumulateLoyalty(double seconds);
    void addLoyaltyPoints(int points);

    [[nodiscard]] int getLoyaltyPoints() const;
    [[nodiscard]] double getLoyaltyBonus() const;
    [[nodiscard]] double getLuxuryFactor() const;
};