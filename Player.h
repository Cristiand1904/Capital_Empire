#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Business.h"
#include "Wallet.h"
#include "Achievement.h"

class Player {
private:
    std::string name;
    Wallet wallet;
    int gold;
    std::vector<std::unique_ptr<Business>> businesses;
    std::vector<Achievement> achievements;

    double globalProfitMultiplier;
    double globalDiscount;
    double globalSpeedMultiplier;

    double lemonadeMultiplier;
    double shrimpMultiplier;
    double managerCostDiscount;
    double offlineEarningsRatio;
    double prestigeGoldBonus;

    double tempBoostTimer;
    double tempBoostMultiplier;

    std::vector<bool> goldUpgradesOwned;

    void initAchievements();

public:
    Player(const std::string& name, double money);

    Player(const Player& other);
    Player& operator=(Player other);
    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;

    void addBusiness(std::unique_ptr<Business> business);

    const std::string& getName() const;
    double getMoney() const;
    int getGold() const;
    void setGold(int g);
    const std::vector<std::unique_ptr<Business>>& getBusinesses() const;
    const std::vector<Achievement>& getAchievements() const;

    void setMoney(double m);
    void unlockAchievement(const std::string& achievementName);

    std::vector<std::string> update(double deltaTime);
    std::vector<std::string> checkAchievements();

    void startBusinessProduction(int index);

    void purchaseBusiness(int index);
    void upgradeBusiness(int index);
    void hireManager(int index);
    void upgradeManager(int index);

    double calculateOfflineEarnings(double secondsOffline);

    bool canPrestige() const;
    int calculatePrestigeGold() const;
    int prestige();

    void addGlobalProfitMultiplier(double val);
    void addGlobalDiscount(double val);
    void addGlobalSpeedMultiplier(double val);

    void setLemonadeMultiplier(double val) { lemonadeMultiplier = val; }
    void setShrimpMultiplier(double val) { shrimpMultiplier = val; }
    void setManagerCostDiscount(double val) { managerCostDiscount = val; }
    void setOfflineEarningsRatio(double val) { offlineEarningsRatio = val; }
    void setPrestigeGoldBonus(double val) { prestigeGoldBonus = val; }

    void activateTempBoost(double duration, double multiplier);
    double getTempBoostTimer() const { return tempBoostTimer; }
    void setTempBoostTimer(double t) { tempBoostTimer = t; }

    double getGlobalProfitMultiplier() const { return globalProfitMultiplier; }
    double getGlobalDiscount() const { return globalDiscount; }
    double getGlobalSpeedMultiplier() const { return globalSpeedMultiplier; }

    double getLemonadeMultiplier() const { return lemonadeMultiplier; }
    double getShrimpMultiplier() const { return shrimpMultiplier; }
    double getManagerCostDiscount() const { return managerCostDiscount; }
    double getOfflineEarningsRatio() const { return offlineEarningsRatio; }
    double getPrestigeGoldBonus() const { return prestigeGoldBonus; }

    bool hasGoldUpgrade(int id) const;
    void setGoldUpgradeOwned(int id, bool owned);
    const std::vector<bool>& getGoldUpgradesOwned() const { return goldUpgradesOwned; }
    void setGoldUpgradesOwned(const std::vector<bool>& owned) { goldUpgradesOwned = owned; }

    friend void swap(Player& first, Player& second) noexcept;
};