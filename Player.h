#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Achievement.h"
#include "Business.h"
#include "GoldUpgrade.h"
#include "Wallet.h"

class Player {
private:
    std::string name;
    Wallet wallet;
    int gold;
    int prestigeCount;
    std::vector<std::unique_ptr<Business>> businesses;
    std::vector<Achievement> achievements;
    std::vector<GoldUpgrade> goldUpgrades;

    double globalProfitMultiplier;
    double globalDiscount;
    double globalSpeedMultiplier;
    double managerCostDiscount;
    double offlineEarningsRatio;
    double prestigeGoldBonus;

    std::map<std::string, double> businessMultipliers;

    double tempBoostTimer;
    double tempBoostMultiplier;

    std::vector<bool> goldUpgradesOwned;

    void resetBonuses();
    void applyGoldUpgradeEffect(const GoldUpgrade& upgrade);

    [[nodiscard]] Business& businessAt(int index);
    [[nodiscard]] const Business& businessAt(int index) const;

    [[nodiscard]] double staticBonusFor(const Business& business) const;
    [[nodiscard]] double bonusFor(const Business& business) const;
    [[nodiscard]] double discountedPrice(double basePrice) const;
    [[nodiscard]] double managerPrice(double basePrice) const;

    [[nodiscard]] PlayerProgress buildProgress() const;
    std::vector<std::string> checkAchievements();

public:
    Player(std::string name, double money, std::vector<GoldUpgrade> upgrades,
           std::vector<Achievement> achievements);

    Player(const Player& other);
    Player& operator=(Player other);
    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;
    ~Player() = default;

    void addBusiness(std::unique_ptr<Business> business);

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] double getMoney() const;
    [[nodiscard]] int getGold() const;
    [[nodiscard]] int getPrestigeCount() const;
    [[nodiscard]] const std::vector<std::unique_ptr<Business>>& getBusinesses() const;
    [[nodiscard]] const std::vector<Achievement>& getAchievements() const;
    [[nodiscard]] std::string getBonusSummary() const;

    std::vector<std::string> update(double deltaTime);

    void startBusinessProduction(int index);
    void purchaseBusiness(int index);
    void upgradeBusiness(int index);
    void hireManager(int index);
    void upgradeManager(int index);

    void addMoney(double amount);
    void addGold(int amount);
    void spendGold(int amount);

    double calculateOfflineEarnings(double secondsOffline);

    [[nodiscard]] bool canPrestige() const;
    [[nodiscard]] int calculatePrestigeGold() const;
    int prestige();

    void activateTempBoost(double duration, double multiplier);
    [[nodiscard]] double getTempBoostTimer() const;
    [[nodiscard]] double getTempBoostMultiplier() const;

    [[nodiscard]] const std::vector<GoldUpgrade>& goldUpgradeCatalog() const;
    [[nodiscard]] const GoldUpgrade& findGoldUpgrade(int id) const;
    [[nodiscard]] bool hasGoldUpgrade(int id) const;
    [[nodiscard]] bool canAffordGoldUpgrade(int id) const;
    void buyGoldUpgrade(int id);
    [[nodiscard]] const std::vector<bool>& getGoldUpgradesOwned() const;

    void restoreProgress(double savedMoney, int savedGold, int savedPrestigeCount,
                         const std::vector<bool>& ownedUpgrades,
                         double boostTimer, double boostMultiplier);
    void unlockAchievement(const std::string& achievementName);

    friend void swap(Player& first, Player& second) noexcept;
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
