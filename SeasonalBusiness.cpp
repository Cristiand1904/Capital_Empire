#include "SeasonalBusiness.h"
#include <ctime>
#include <utility>

const double SeasonalBusiness::SEASON_BONUS = 2.0;
const double SeasonalBusiness::OFF_SEASON_PENALTY = 0.75;

SeasonalBusiness::SeasonalBusiness(std::string name, double profit, double upgrade, double cost,
                                   double time, double mngCost, Season favoriteSeason)
    : Business(std::move(name), profit, upgrade, cost, time, mngCost), favoriteSeason(favoriteSeason) {}

std::unique_ptr<Business> SeasonalBusiness::clone() const {
    return std::make_unique<SeasonalBusiness>(*this);
}

Season SeasonalBusiness::getCurrentSeason() {
    const std::time_t now = std::time(nullptr);
    std::tm parts{};
#ifdef _WIN32
    localtime_s(&parts, &now);
#else
    localtime_r(&now, &parts);
#endif
    const int month = parts.tm_mon + 1;

    if (month >= 3 && month <= 5) return Season::SPRING;
    if (month >= 6 && month <= 8) return Season::SUMMER;
    if (month >= 9 && month <= 11) return Season::AUTUMN;
    return Season::WINTER;
}

std::string SeasonalBusiness::seasonName(Season season) {
    switch (season) {
        case Season::SPRING: return "Primavara";
        case Season::SUMMER: return "Vara";
        case Season::AUTUMN: return "Toamna";
        case Season::WINTER: return "Iarna";
    }
    return "Necunoscut";
}

std::optional<Season> SeasonalBusiness::seasonFromString(const std::string& text) {
    if (text == "SPRING") return Season::SPRING;
    if (text == "SUMMER") return Season::SUMMER;
    if (text == "AUTUMN") return Season::AUTUMN;
    if (text == "WINTER") return Season::WINTER;
    return std::nullopt;
}

bool SeasonalBusiness::isInFavoriteSeason() const {
    return getCurrentSeason() == favoriteSeason;
}

double SeasonalBusiness::getSeasonalMultiplier() const {
    return isInFavoriteSeason() ? SEASON_BONUS : OFF_SEASON_PENALTY;
}

double SeasonalBusiness::calculateRevenue(double bonusMultiplier) const {
    return baseRevenue(bonusMultiplier) * getSeasonalMultiplier();
}

Season SeasonalBusiness::getFavoriteSeason() const {
    return favoriteSeason;
}

std::string SeasonalBusiness::getStatusLabel() const {
    std::string label = "Sezonier (" + seasonName(favoriteSeason) + ")";
    label += isInFavoriteSeason() ? " x2" : " x0.75";
    return label;
}

void SeasonalBusiness::print(std::ostream& os) const {
    Business::print(os);
    os << " | " << getStatusLabel();
}