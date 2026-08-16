#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

enum class AchievementType {
    MONEY,
    TOTAL_LEVELS,
    HAS_MANAGER,
    HAS_UPGRADE,
    TOTAL_MANAGERS,
    PRESTIGE_COUNT,
    TOTAL_GOLD,
    SPECIFIC_BUSINESS_LEVEL
};

class PlayerProgress {
private:
    double money;
    int gold;
    int prestigeCount;
    int totalManagers;
    int totalLevels;
    bool hasUpgrade;
    std::map<std::string, int> businessLevels;

public:
    PlayerProgress(double money, int gold, int prestigeCount, int totalManagers,
                   std::map<std::string, int> businessLevels);

    [[nodiscard]] int levelOf(const std::string& businessName) const;

    friend class Achievement;
    friend std::ostream& operator<<(std::ostream& os, const PlayerProgress& p);
};

class Achievement {
private:
    std::string name;
    std::string description;
    bool unlocked;

    AchievementType type;
    double targetValue;
    double reward;
    std::string specificBusinessName;

    static int unlockEvents;

public:
    Achievement(std::string name, std::string desc, double reward, AchievementType type,
                double target, std::string businessName = "");

    [[nodiscard]] bool isUnlocked() const;
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] const std::string& getDescription() const;
    [[nodiscard]] double getReward() const;

    [[nodiscard]] bool checkCondition(const PlayerProgress& progress) const;
    void unlock();

    static int getUnlockEvents();
    static int countUnlocked(const std::vector<Achievement>& list);
    static std::optional<AchievementType> typeFromString(const std::string& text);

    friend std::ostream& operator<<(std::ostream& os, const Achievement& ach);
};