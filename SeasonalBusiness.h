#pragma once
#include "Business.h"

enum class Season {
    SPRING,
    SUMMER,
    AUTUMN,
    WINTER
};

class SeasonalBusiness : public Business {
private:
    Season favoriteSeason;

    static const double SEASON_BONUS;
    static const double OFF_SEASON_PENALTY;

protected:
    void print(std::ostream& os) const override;

public:
    SeasonalBusiness(std::string name, double profit, double upgrade, double cost,
                     double time, double mngCost, Season favoriteSeason);

    [[nodiscard]] std::unique_ptr<Business> clone() const override;
    [[nodiscard]] double calculateRevenue(double bonusMultiplier) const override;
    [[nodiscard]] std::string getStatusLabel() const override;

    [[nodiscard]] bool isInFavoriteSeason() const;
    [[nodiscard]] double getSeasonalMultiplier() const;
    [[nodiscard]] Season getFavoriteSeason() const;

    static Season getCurrentSeason();
    static std::string seasonName(Season season);
};