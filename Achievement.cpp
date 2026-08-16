#include "Achievement.h"
#include <algorithm>
#include <iomanip>
#include <utility>

int Achievement::unlockEvents = 0;

PlayerProgress::PlayerProgress(double money, int gold, int prestigeCount, int totalManagers,
                               std::map<std::string, int> businessLevels)
    : money(money), gold(gold), prestigeCount(prestigeCount), totalManagers(totalManagers),
      totalLevels(0), hasUpgrade(false), businessLevels(std::move(businessLevels)) {
    for (const auto& [businessName, level] : this->businessLevels) {
        totalLevels += level;
        if (level > 1) {
            hasUpgrade = true;
        }
    }
}

int PlayerProgress::levelOf(const std::string& businessName) const {
    const auto it = businessLevels.find(businessName);
    return it != businessLevels.end() ? it->second : 0;
}

std::ostream& operator<<(std::ostream& os, const PlayerProgress& p) {
    os << std::fixed << std::setprecision(0);
    os << "Bani: " << p.money << "$ | Gold: " << p.gold;
    os << " | Nivele: " << p.totalLevels << " | Manageri: " << p.totalManagers;
    os << " | Prestigii: " << p.prestigeCount;
    return os;
}

Achievement::Achievement(std::string name, std::string desc, double reward, AchievementType type,
                         double target, std::string businessName)
    : name(std::move(name)), description(std::move(desc)), unlocked(false),
      type(type), targetValue(target), reward(reward), specificBusinessName(std::move(businessName)) {}

bool Achievement::isUnlocked() const {
    return unlocked;
}

const std::string& Achievement::getName() const {
    return name;
}

const std::string& Achievement::getDescription() const {
    return description;
}

double Achievement::getReward() const {
    return reward;
}

bool Achievement::checkCondition(const PlayerProgress& progress) const {
    if (unlocked) return false;

    const int target = static_cast<int>(targetValue);

    switch (type) {
        case AchievementType::MONEY:
            return progress.money >= targetValue;
        case AchievementType::TOTAL_LEVELS:
            return progress.totalLevels >= target;
        case AchievementType::HAS_MANAGER:
            return progress.totalManagers > 0;
        case AchievementType::HAS_UPGRADE:
            return progress.hasUpgrade;
        case AchievementType::TOTAL_MANAGERS:
            return progress.totalManagers >= target;
        case AchievementType::PRESTIGE_COUNT:
            return progress.prestigeCount >= target;
        case AchievementType::TOTAL_GOLD:
            return progress.gold >= target;
        case AchievementType::SPECIFIC_BUSINESS_LEVEL:
            return progress.levelOf(specificBusinessName) >= target;
    }
    return false;
}

void Achievement::unlock() {
    if (unlocked) return;
    unlocked = true;
    unlockEvents++;
}

int Achievement::getUnlockEvents() {
    return unlockEvents;
}

int Achievement::countUnlocked(const std::vector<Achievement>& list) {
    return static_cast<int>(std::count_if(list.begin(), list.end(),
                                          [](const Achievement& ach) { return ach.isUnlocked(); }));
}

std::ostream& operator<<(std::ostream& os, const Achievement& ach) {
    os << ach.getName() << " [" << (ach.isUnlocked() ? "Unlocked" : "Locked") << "]";
    os << " - " << ach.getDescription();
    os << " | Recompensa: " << std::fixed << std::setprecision(0) << ach.getReward() << "$";
    return os;
}