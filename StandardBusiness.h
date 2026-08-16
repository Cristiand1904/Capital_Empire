#pragma once
#include "Business.h"

enum class BusinessType {
    LEMONADE,
    ICE_CREAM,
    RESTAURANT,
    PIZZA,
    DONUT,
    SHRIMP
};

class StandardBusiness : public Business {
private:
    BusinessType type;

    [[nodiscard]] double typeFactor() const;

protected:
    void print(std::ostream& os) const override;

public:
    StandardBusiness(std::string name, BusinessType type, double profit, double upgrade,
                     double cost, double time, double mngCost);

    [[nodiscard]] std::unique_ptr<Business> clone() const override;
    [[nodiscard]] double calculateRevenue(double bonusMultiplier) const override;
    [[nodiscard]] std::string getStatusLabel() const override;

    [[nodiscard]] BusinessType getType() const;
};