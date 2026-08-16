#include "Business.h"
#include "Exceptions.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <utility>

Business::Business(std::string name, double profit, double upgrade, double cost, double time, double mngCost)
    : name(std::move(name)),
      profitPerCycle(profit),
      initialProfit(profit),
      upgradeCost(upgrade),
      initialUpgradeCost(upgrade),
      purchaseCost(cost),
      managerBaseCost(mngCost),
      initialProductionTime(time),
      level(0),
      owned(false),
      manager(nullptr),
      productionTime(time),
      currentTimer(0.0),
      isProducing(false),
      sessionMultiplier(1.0) {

    if (this->name.empty()) {
        throw InvalidBusinessDataException("<fara nume>", "numele nu poate fi gol");
    }
    if (profit <= 0.0) {
        throw InvalidBusinessDataException(this->name, "profitul pe ciclu trebuie sa fie pozitiv");
    }
    if (upgrade <= 0.0) {
        throw InvalidBusinessDataException(this->name, "costul de upgrade trebuie sa fie pozitiv");
    }
    if (cost < 0.0) {
        throw InvalidBusinessDataException(this->name, "costul de achizitie nu poate fi negativ");
    }
    if (time <= 0.0) {
        throw InvalidBusinessDataException(this->name, "timpul de productie trebuie sa fie pozitiv");
    }
    if (mngCost <= 0.0) {
        throw InvalidBusinessDataException(this->name, "costul managerului trebuie sa fie pozitiv");
    }

    initMilestones();
}

Business::Business(const Business& other)
    : name(other.name),
      profitPerCycle(other.profitPerCycle),
      initialProfit(other.initialProfit),
      upgradeCost(other.upgradeCost),
      initialUpgradeCost(other.initialUpgradeCost),
      purchaseCost(other.purchaseCost),
      managerBaseCost(other.managerBaseCost),
      initialProductionTime(other.initialProductionTime),
      level(other.level),
      owned(other.owned),
      manager(other.manager ? std::make_unique<Manager>(*other.manager) : nullptr),
      upgrades(other.upgrades),
      productionTime(other.productionTime),
      currentTimer(other.currentTimer),
      isProducing(other.isProducing),
      sessionMultiplier(other.sessionMultiplier) {}

Business& Business::operator=(const Business& other) {
    if (this == &other) {
        return *this;
    }
    name = other.name;
    profitPerCycle = other.profitPerCycle;
    initialProfit = other.initialProfit;
    upgradeCost = other.upgradeCost;
    initialUpgradeCost = other.initialUpgradeCost;
    purchaseCost = other.purchaseCost;
    managerBaseCost = other.managerBaseCost;
    initialProductionTime = other.initialProductionTime;
    level = other.level;
    owned = other.owned;
    manager = other.manager ? std::make_unique<Manager>(*other.manager) : nullptr;
    upgrades = other.upgrades;
    productionTime = other.productionTime;
    currentTimer = other.currentTimer;
    isProducing = other.isProducing;
    sessionMultiplier = other.sessionMultiplier;
    return *this;
}

void Business::initMilestones() {
    upgrades.clear();
    upgrades.emplace_back(10, 3.0, 2.0);
    upgrades.emplace_back(25, 4.0, 3.0);
    upgrades.emplace_back(50, 5.0, 5.0);
}

const Upgrade* Business::nextMilestone() const {
    for (const auto& u : upgrades) {
        if (!u.isPurchased() && u.getRequiredLevel() == level + 1) {
            return &u;
        }
    }
    return nullptr;
}

double Business::baseRevenue(double bonusMultiplier) const {
    return profitPerCycle * bonusMultiplier * sessionMultiplier;
}

double Business::update(double deltaTime, double bonusMultiplier) {
    if (!owned) return 0.0;

    if (hasManagerHired() && !isProducing) {
        isProducing = true;
    }

    if (isProducing) {
        currentTimer += deltaTime;
        if (currentTimer >= productionTime) {
            currentTimer = 0.0;
            if (!hasManagerHired()) {
                isProducing = false;
            }
            return std::floor(calculateRevenue(bonusMultiplier));
        }
    }
    return 0.0;
}

void Business::startProduction() {
    if (owned && !isProducing) {
        isProducing = true;
    }
}

void Business::levelUp() {
    level++;

    for (auto& u : upgrades) {
        if (!u.isPurchased() && u.getRequiredLevel() == level) {
            u.purchase();
            profitPerCycle *= u.getMultiplier();
        }
    }

    const double profitFactor = (level > 50) ? 1.05 : 1.15;
    profitPerCycle *= profitFactor;
    upgradeCost *= 1.15;

    if (level % 25 == 0) {
        productionTime /= 2.0;
    }
}

void Business::unlock() {
    owned = true;
    level = 1;
    isProducing = false;
}

void Business::hireManager() {
    manager = std::make_unique<Manager>(name + " Manager", getManagerCost());
    isProducing = true;
}

void Business::upgradeManager() {
    if (manager) {
        manager->upgrade();
    }
}

void Business::reset() {
    level = 0;
    owned = false;
    profitPerCycle = initialProfit;
    upgradeCost = initialUpgradeCost;
    productionTime = initialProductionTime;
    manager = nullptr;
    currentTimer = 0.0;
    isProducing = false;
    sessionMultiplier = 1.0;
    initMilestones();
}

void Business::restoreState(int savedLevel, bool savedOwned, bool savedManager, int managerLevel,
                            double savedProfit, double savedUpgradeCost) {
    level = std::max(0, savedLevel);
    owned = savedOwned;
    profitPerCycle = savedProfit;
    upgradeCost = savedUpgradeCost;
    currentTimer = 0.0;
    isProducing = false;
    sessionMultiplier = 1.0;

    initMilestones();
    for (auto& u : upgrades) {
        if (u.getRequiredLevel() <= level) {
            u.purchase();
        }
    }

    productionTime = initialProductionTime;
    for (int reached = 25; reached <= level; reached += 25) {
        productionTime /= 2.0;
    }

    manager = nullptr;
    if (owned && savedManager) {
        hireManager();
        for (int i = 1; i < managerLevel; ++i) {
            manager->upgrade();
        }
    }
}

void Business::setManagerHired(bool hired) {
    if (hired && !manager) {
        hireManager();
    } else if (!hired) {
        manager = nullptr;
    }
}

void Business::setSessionMultiplier(double multiplier) {
    sessionMultiplier = multiplier > 0.0 ? multiplier : 1.0;
}

bool Business::isOwned() const { return owned; }
bool Business::hasManagerHired() const { return manager != nullptr; }
const std::string& Business::getName() const { return name; }
int Business::getLevel() const { return level; }
double Business::getProfitPerCycle() const { return profitPerCycle; }
double Business::getSessionMultiplier() const { return sessionMultiplier; }

double Business::getUpgradeCost() const {
    double cost = upgradeCost;
    if (const Upgrade* milestone = nextMilestone()) {
        cost *= milestone->getCostMultiplier();
    }
    if (manager) {
        cost *= manager->getDiscountFactor();
    }
    return std::floor(cost);
}

double Business::getPurchaseCost() const { return std::floor(purchaseCost); }

double Business::getManagerCost() const {
    return std::floor(managerBaseCost);
}

double Business::getManagerUpgradeCost() const {
    if (manager) {
        return std::floor(manager->getCost());
    }
    return 0.0;
}

bool Business::nextUpgradeIsMilestone() const {
    return nextMilestone() != nullptr;
}

double Business::getProgress() const {
    if (!isProducing) return 0.0;
    if (currentTimer >= productionTime - 0.1) {
        return 1.0;
    }
    return std::min(1.0, currentTimer / productionTime);
}

double Business::getProductionTime() const { return productionTime; }
bool Business::isActive() const { return isProducing; }

int Business::getManagerLevel() const {
    if (manager) {
        return manager->getLevel();
    }
    return 0;
}

std::string Business::getStatusLabel() const {
    return "Standard";
}

void Business::display(std::ostream& os) const {
    print(os);
}

void Business::print(std::ostream& os) const {
    os << std::fixed << std::setprecision(0);
    os << name << " (Lvl " << level << ")";
    os << " | Profit: " << profitPerCycle << "$";
    os << " | Timp: " << std::setprecision(1) << productionTime << "s";
    if (manager) {
        os << " | " << *manager;
    }
}

std::ostream& operator<<(std::ostream& os, const Business& b) {
    b.display(os);
    return os;
}