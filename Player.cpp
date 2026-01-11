#include "Player.h"
#include "Exceptions.h"
#include <iostream>
#include <iomanip>
#include <utility>
#include <stdexcept>
#include <cmath>

Player::Player(const std::string& name, double money)
    : name(name), wallet(money), gold(0),
      globalProfitMultiplier(1.0), globalDiscount(0.0), globalSpeedMultiplier(1.0),
      lemonadeMultiplier(1.0), shrimpMultiplier(1.0), managerCostDiscount(0.0),
      offlineEarningsRatio(0.5), prestigeGoldBonus(0.0),
      tempBoostTimer(0.0), tempBoostMultiplier(1.0),
      goldUpgradesOwned(14, false) {
    initAchievements();
}

void Player::initAchievements() {
    achievements.emplace_back("Primul Dolar", "Castiga primul tau dolar", 10.0, AchievementType::MONEY, 1.0);
    achievements.emplace_back("Primul Upgrade", "Fa un upgrade la o afacere", 10.0, AchievementType::HAS_UPGRADE, 1.0);
    achievements.emplace_back("Primul Manager", "Angajeaza un manager", 10.0, AchievementType::HAS_MANAGER, 1.0);
    achievements.emplace_back("Suta de Dolari", "Strange 100$", 100.0, AchievementType::MONEY, 100.0);
    achievements.emplace_back("Mie de Dolari", "Strange 1.000$", 500.0, AchievementType::MONEY, 1000.0);
    achievements.emplace_back("Magnat", "Strange 100.000$", 50000.0, AchievementType::MONEY, 100000.0);
}

Player::Player(const Player& other)
    : name(other.name), wallet(other.wallet), gold(other.gold), achievements(other.achievements),
      globalProfitMultiplier(other.globalProfitMultiplier),
      globalDiscount(other.globalDiscount),
      globalSpeedMultiplier(other.globalSpeedMultiplier),
      lemonadeMultiplier(other.lemonadeMultiplier),
      shrimpMultiplier(other.shrimpMultiplier),
      managerCostDiscount(other.managerCostDiscount),
      offlineEarningsRatio(other.offlineEarningsRatio),
      prestigeGoldBonus(other.prestigeGoldBonus),
      tempBoostTimer(other.tempBoostTimer),
      tempBoostMultiplier(other.tempBoostMultiplier),
      goldUpgradesOwned(other.goldUpgradesOwned) {
    businesses.reserve(other.businesses.size());
    for (const auto& b : other.businesses) {
        businesses.push_back(b->clone());
    }
}

void swap(Player& first, Player& second) noexcept {
    using std::swap;
    swap(first.name, second.name);
    swap(first.wallet, second.wallet);
    swap(first.gold, second.gold);
    swap(first.businesses, second.businesses);
    swap(first.achievements, second.achievements);
    swap(first.globalProfitMultiplier, second.globalProfitMultiplier);
    swap(first.globalDiscount, second.globalDiscount);
    swap(first.globalSpeedMultiplier, second.globalSpeedMultiplier);
    swap(first.lemonadeMultiplier, second.lemonadeMultiplier);
    swap(first.shrimpMultiplier, second.shrimpMultiplier);
    swap(first.managerCostDiscount, second.managerCostDiscount);
    swap(first.offlineEarningsRatio, second.offlineEarningsRatio);
    swap(first.prestigeGoldBonus, second.prestigeGoldBonus);
    swap(first.tempBoostTimer, second.tempBoostTimer);
    swap(first.tempBoostMultiplier, second.tempBoostMultiplier);
    swap(first.goldUpgradesOwned, second.goldUpgradesOwned);
}

Player& Player::operator=(Player other) {
    swap(*this, other);
    return *this;
}

void Player::addBusiness(std::unique_ptr<Business> business) {
    businesses.push_back(std::move(business));
}

const std::string& Player::getName() const { return name; }
double Player::getMoney() const { return wallet.getMoney(); }
int Player::getGold() const { return gold; }
void Player::setGold(int g) { gold = g; }
const std::vector<std::unique_ptr<Business>>& Player::getBusinesses() const { return businesses; }
const std::vector<Achievement>& Player::getAchievements() const { return achievements; }

void Player::setMoney(double m) {
    wallet = Wallet(m);
}

void Player::unlockAchievement(const std::string& achievementName) {
    for (auto& ach : achievements) {
        if (ach.getName() == achievementName) {
            ach.unlock();
            break;
        }
    }
}

std::vector<std::string> Player::update(double deltaTime) {
    if (tempBoostTimer > 0) {
        tempBoostTimer -= deltaTime;
        if (tempBoostTimer < 0) tempBoostTimer = 0;
    }

    double totalProfit = 0;
    for (const auto& b : businesses) {
        if (b->isOwned()) {
            double profit = b->update(deltaTime);

            if (b->getName() == "Limonada") profit *= lemonadeMultiplier;
            if (b->getName() == "Creveti") profit *= shrimpMultiplier;

            profit *= globalProfitMultiplier;

            if (tempBoostTimer > 0) {
                profit *= tempBoostMultiplier;
            }

            totalProfit += profit;
        }
    }
    if (totalProfit > 0) {
        wallet.addMoney(totalProfit);
    }
    return checkAchievements();
}

std::vector<std::string> Player::checkAchievements() {
    std::vector<std::string> unlockedMessages;

    int totalLevels = 0;
    bool hasManager = false;
    bool hasUpgrade = false;

    for (const auto& b : businesses) {
        if (b->isOwned()) {
            totalLevels += b->getLevel();
            if (b->hasManagerHired()) hasManager = true;
            if (b->getLevel() > 1) hasUpgrade = true;
        }
    }

    for (auto& ach : achievements) {
        if (ach.checkCondition(wallet.getMoney(), totalLevels, hasManager, hasUpgrade)) {
            ach.unlock();
            wallet.addMoney(ach.getReward());
            unlockedMessages.push_back("ACHIEVEMENT: " + ach.getName() + "\nReward: $" + std::to_string((int)ach.getReward()));
        }
    }
    return unlockedMessages;
}

void Player::startBusinessProduction(int index) {
    businesses.at(index)->startProduction();
}

void Player::purchaseBusiness(int index_int) {
    const auto& business = businesses.at(index_int);

    if (business->isOwned()) {
        throw BusinessAlreadyOwnedException(business->getName());
    }
    double cost = business->getPurchaseCost();
    cost *= (1.0 - globalDiscount);
    wallet.spendMoney(cost);
    business->unlock();
}

void Player::upgradeBusiness(int index_int) {
    const auto& business = businesses.at(index_int);

    if (!business->isOwned()) {
        throw BusinessNotOwnedException(business->getName());
    }
    double cost = business->getUpgradeCost();
    cost *= (1.0 - globalDiscount);
    wallet.spendMoney(cost);
    business->levelUp();
}

void Player::hireManager(int index_int) {
    const auto& business = businesses.at(index_int);

    if (!business->isOwned()) {
        throw BusinessNotOwnedException(business->getName());
    }
    double cost = business->getManagerCost();
    cost *= (1.0 - globalDiscount);
    cost *= (1.0 - managerCostDiscount);

    wallet.spendMoney(cost);
    business->hireManager();
}

void Player::upgradeManager(int index_int) {
    const auto& business = businesses.at(index_int);

    if (!business->hasManagerHired()) {
        throw BusinessNotOwnedException(business->getName() + " Manager");
    }
    double cost = business->getManagerUpgradeCost();
    cost *= (1.0 - globalDiscount);
    cost *= (1.0 - managerCostDiscount);

    wallet.spendMoney(cost);
    business->upgradeManager();
}

double Player::calculateOfflineEarnings(double secondsOffline) {
    double totalOfflineEarnings = 0.0;

    double boostedTime = 0.0;
    double normalTime = secondsOffline;

    if (tempBoostTimer > 0) {
        if (secondsOffline <= tempBoostTimer) {
            boostedTime = secondsOffline;
            normalTime = 0;
            tempBoostTimer -= secondsOffline;
        } else {
            boostedTime = tempBoostTimer;
            normalTime = secondsOffline - tempBoostTimer;
            tempBoostTimer = 0;
        }
    }

    for (const auto& b : businesses) {
        if (b->isOwned() && b->hasManagerHired()) {
            double productionTime = b->getProductionTime();
            if (productionTime > 0) {
                double profitPerCycle = b->getProfitPerCycle();

                if (b->getName() == "Limonada") profitPerCycle *= lemonadeMultiplier;
                if (b->getName() == "Creveti") profitPerCycle *= shrimpMultiplier;

                profitPerCycle *= globalProfitMultiplier;

                double boostedCycles = boostedTime / productionTime;
                double normalCycles = normalTime / productionTime;

                totalOfflineEarnings += boostedCycles * profitPerCycle * tempBoostMultiplier;
                totalOfflineEarnings += normalCycles * profitPerCycle;
            }
        }
    }

    return totalOfflineEarnings * (offlineEarningsRatio / 0.5);
}

bool Player::canPrestige() const {
    if (wallet.getMoney() < 7000000.0) return false;

    if (businesses.empty()) return false;
    const auto& lastBusiness = businesses.back();

    if (lastBusiness->getName() == "Creveti" && lastBusiness->isOwned() && lastBusiness->getLevel() >= 25) {
        return true;
    }

    return false;
}

int Player::calculatePrestigeGold() const {
    if (wallet.getMoney() < 7000000.0) return 0;
    int baseGold = static_cast<int>(wallet.getMoney() / 3500000.0);
    return static_cast<int>(baseGold * (1.0 + prestigeGoldBonus));
}

int Player::prestige() {
    if (!canPrestige()) return 0;

    int goldGained = calculatePrestigeGold();
    gold += goldGained;

    wallet = Wallet(0.0);
    for (auto& b : businesses) {
        b->reset();
    }

    tempBoostTimer = 0;

    return goldGained;
}

void Player::addGlobalProfitMultiplier(double val) {
    globalProfitMultiplier += val;
}

void Player::addGlobalDiscount(double val) {
    globalDiscount += val;
    if (globalDiscount > 0.9) globalDiscount = 0.9;
}

void Player::addGlobalSpeedMultiplier(double val) {
    globalSpeedMultiplier += val;
}

void Player::activateTempBoost(double duration, double multiplier) {
    tempBoostTimer += duration;
    tempBoostMultiplier = multiplier;
}

bool Player::hasGoldUpgrade(int id) const {
    if (id >= 0 && id < (int)goldUpgradesOwned.size()) {
        return goldUpgradesOwned[id];
    }
    return false;
}

void Player::setGoldUpgradeOwned(int id, bool owned) {
    if (id >= 0 && id < (int)goldUpgradesOwned.size()) {
        goldUpgradesOwned[id] = owned;
    }
}
