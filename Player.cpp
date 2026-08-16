#include "Player.h"
#include "Exceptions.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

const std::vector<GoldUpgrade> Player::catalog = {
    GoldUpgrade(1,   5, "Profit x2",             "Dubleaza profitul global"),
    GoldUpgrade(2,  10, "Discount 10%",          "Reduce toate costurile cu 10%"),
    GoldUpgrade(3,  15, "Speed +10%",            "Toate afacerile produc cu 10% mai repede"),
    GoldUpgrade(4,   3, "Small Biz Bonus +25%",  "Creste profitul global cu 25%"),
    GoldUpgrade(5,   5, "Lemonade Mastery x10",  "Limonada produce de 10 ori mai mult"),
    GoldUpgrade(6,   8, "Corp Tax Cut +50%",     "Creste profitul global cu 50%"),
    GoldUpgrade(7,  12, "Headhunter -50% Mng",   "Managerii costa cu 50% mai putin"),
    GoldUpgrade(8,  20, "Night Shift 80% Off",   "Castigi 80% din productia offline"),
    GoldUpgrade(9,  25, "Bulk Buying -20% Upg",  "Reduce toate costurile cu inca 20%"),
    GoldUpgrade(10, 30, "Automation Speed +20%", "Toate afacerile produc cu inca 20% mai repede"),
    GoldUpgrade(11, 40, "Golden Touch +10%",     "Primesti cu 10% mai mult gold la prestige"),
    GoldUpgrade(12, 50, "Market Monopoly x5",    "Creste profitul global de 5 ori"),
    GoldUpgrade(13, 60, "Ocean King Shrimp x5",  "Crevetii produc de 5 ori mai mult")
};

Player::Player(const std::string& name, double money)
    : name(name), wallet(money), gold(0), prestigeCount(0),
      globalProfitMultiplier(1.0), globalDiscount(0.0), globalSpeedMultiplier(1.0),
      managerCostDiscount(0.0), offlineEarningsRatio(0.5), prestigeGoldBonus(0.0),
      tempBoostTimer(0.0), tempBoostMultiplier(1.0),
      goldUpgradesOwned(catalog.size() + 1, false) {
    initAchievements();
}

void Player::initAchievements() {
    achievements.emplace_back("Primul Dolar", "Castiga primul tau dolar", 10.0, AchievementType::MONEY, 1.0);
    achievements.emplace_back("Primul Upgrade", "Fa un upgrade la o afacere", 10.0, AchievementType::HAS_UPGRADE, 1.0);
    achievements.emplace_back("Primul Manager", "Angajeaza un manager", 10.0, AchievementType::HAS_MANAGER, 1.0);
    achievements.emplace_back("Suta de Dolari", "Strange 100$", 100.0, AchievementType::MONEY, 100.0);
    achievements.emplace_back("Mie de Dolari", "Strange 1.000$", 500.0, AchievementType::MONEY, 1000.0);
    achievements.emplace_back("Magnat", "Strange 100.000$", 50000.0, AchievementType::MONEY, 100000.0);
    achievements.emplace_back("Milionar", "Strange 1.000.000$", 500000.0, AchievementType::MONEY, 1000000.0);
    achievements.emplace_back("Miliardar", "Strange 1.000.000.000$", 50000000.0, AchievementType::MONEY, 1000000000.0);
    achievements.emplace_back("Inceputuri", "Atinge total 10 nivele la afaceri", 500.0, AchievementType::TOTAL_LEVELS, 10.0);
    achievements.emplace_back("In Crestere", "Atinge total 50 nivele la afaceri", 5000.0, AchievementType::TOTAL_LEVELS, 50.0);
    achievements.emplace_back("Expansiune", "Atinge total 100 nivele la afaceri", 50000.0, AchievementType::TOTAL_LEVELS, 100.0);
    achievements.emplace_back("Imperiu", "Atinge total 500 nivele la afaceri", 1000000.0, AchievementType::TOTAL_LEVELS, 500.0);
    achievements.emplace_back("Dominatie", "Atinge total 1000 nivele la afaceri", 10000000.0, AchievementType::TOTAL_LEVELS, 1000.0);
    achievements.emplace_back("Sef de Echipa", "Angajeaza 3 manageri", 25000.0, AchievementType::TOTAL_MANAGERS, 3.0);
    achievements.emplace_back("Consiliu Complet", "Angajeaza 6 manageri", 250000.0, AchievementType::TOTAL_MANAGERS, 6.0);
    achievements.emplace_back("Rege al Crevetilor", "Ridica Crevetii la nivelul 25", 2000000.0,
                              AchievementType::SPECIFIC_BUSINESS_LEVEL, 25.0, "Creveti");
    achievements.emplace_back("Renastere", "Fa primul prestige", 100000.0, AchievementType::PRESTIGE_COUNT, 1.0);
    achievements.emplace_back("Investitor de Aur", "Strange 25 gold", 500000.0, AchievementType::TOTAL_GOLD, 25.0);
}

Player::Player(const Player& other)
    : name(other.name), wallet(other.wallet), gold(other.gold), prestigeCount(other.prestigeCount),
      achievements(other.achievements),
      globalProfitMultiplier(other.globalProfitMultiplier),
      globalDiscount(other.globalDiscount),
      globalSpeedMultiplier(other.globalSpeedMultiplier),
      managerCostDiscount(other.managerCostDiscount),
      offlineEarningsRatio(other.offlineEarningsRatio),
      prestigeGoldBonus(other.prestigeGoldBonus),
      businessMultipliers(other.businessMultipliers),
      tempBoostTimer(other.tempBoostTimer),
      tempBoostMultiplier(other.tempBoostMultiplier),
      goldUpgradesOwned(other.goldUpgradesOwned) {
    businesses.reserve(other.businesses.size());
    for (const auto& business : other.businesses) {
        businesses.push_back(business->clone());
    }
}

void swap(Player& first, Player& second) noexcept {
    using std::swap;
    swap(first.name, second.name);
    swap(first.wallet, second.wallet);
    swap(first.gold, second.gold);
    swap(first.prestigeCount, second.prestigeCount);
    swap(first.businesses, second.businesses);
    swap(first.achievements, second.achievements);
    swap(first.globalProfitMultiplier, second.globalProfitMultiplier);
    swap(first.globalDiscount, second.globalDiscount);
    swap(first.globalSpeedMultiplier, second.globalSpeedMultiplier);
    swap(first.managerCostDiscount, second.managerCostDiscount);
    swap(first.offlineEarningsRatio, second.offlineEarningsRatio);
    swap(first.prestigeGoldBonus, second.prestigeGoldBonus);
    swap(first.businessMultipliers, second.businessMultipliers);
    swap(first.tempBoostTimer, second.tempBoostTimer);
    swap(first.tempBoostMultiplier, second.tempBoostMultiplier);
    swap(first.goldUpgradesOwned, second.goldUpgradesOwned);
}

Player& Player::operator=(Player other) {
    swap(*this, other);
    return *this;
}

void Player::addBusiness(std::unique_ptr<Business> business) {
    if (!business) {
        throw InvalidBusinessDataException("<null>", "afacerea adaugata nu poate fi nula");
    }
    businesses.push_back(std::move(business));
}

const std::string& Player::getName() const { return name; }
double Player::getMoney() const { return wallet.getMoney(); }
int Player::getGold() const { return gold; }
int Player::getPrestigeCount() const { return prestigeCount; }
const std::vector<std::unique_ptr<Business>>& Player::getBusinesses() const { return businesses; }
const std::vector<Achievement>& Player::getAchievements() const { return achievements; }
double Player::getTempBoostTimer() const { return tempBoostTimer; }
double Player::getTempBoostMultiplier() const { return tempBoostMultiplier; }
const std::vector<bool>& Player::getGoldUpgradesOwned() const { return goldUpgradesOwned; }

Business& Player::businessAt(int index) {
    if (index < 0 || static_cast<size_t>(index) >= businesses.size()) {
        throw InvalidBusinessIndexException(index);
    }
    return *businesses[static_cast<size_t>(index)];
}

const Business& Player::businessAt(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= businesses.size()) {
        throw InvalidBusinessIndexException(index);
    }
    return *businesses[static_cast<size_t>(index)];
}

double Player::staticBonusFor(const Business& business) const {
    double bonus = globalProfitMultiplier;
    const auto it = businessMultipliers.find(business.getName());
    if (it != businessMultipliers.end()) {
        bonus *= it->second;
    }
    return bonus;
}

double Player::bonusFor(const Business& business) const {
    double bonus = staticBonusFor(business);
    if (tempBoostTimer > 0.0) {
        bonus *= tempBoostMultiplier;
    }
    return bonus;
}

double Player::discountedPrice(double basePrice) const {
    return basePrice * (1.0 - globalDiscount);
}

double Player::managerPrice(double basePrice) const {
    return discountedPrice(basePrice) * (1.0 - managerCostDiscount);
}

void Player::addMoney(double amount) {
    if (amount > 0.0) {
        wallet.addMoney(amount);
    }
}

void Player::addGold(int amount) {
    if (amount > 0) {
        gold += amount;
    }
}

void Player::spendGold(int amount) {
    if (amount <= 0) return;
    if (gold < amount) {
        throw InsufficientGoldException(amount, gold);
    }
    gold -= amount;
}

void Player::unlockAchievement(const std::string& achievementName) {
    for (auto& ach : achievements) {
        if (ach.getName() == achievementName) {
            ach.unlock();
            return;
        }
    }
}

std::vector<std::string> Player::update(double deltaTime) {
    if (tempBoostTimer > 0.0) {
        tempBoostTimer -= deltaTime;
        if (tempBoostTimer < 0.0) tempBoostTimer = 0.0;
    }

    const double scaledTime = deltaTime * globalSpeedMultiplier;

    double totalProfit = 0.0;
    for (const auto& business : businesses) {
        totalProfit += business->update(scaledTime, bonusFor(*business));
    }

    if (totalProfit > 0.0) {
        wallet.addMoney(totalProfit);
    }
    return checkAchievements();
}

PlayerProgress Player::buildProgress() const {
    std::map<std::string, int> levels;
    int totalManagers = 0;

    for (const auto& business : businesses) {
        if (!business->isOwned()) continue;
        levels[business->getName()] = business->getLevel();
        if (business->hasManagerHired()) {
            totalManagers++;
        }
    }
    return PlayerProgress(wallet.getMoney(), gold, prestigeCount, totalManagers, std::move(levels));
}

std::vector<std::string> Player::checkAchievements() {
    const PlayerProgress progress = buildProgress();
    std::vector<std::string> unlockedMessages;

    for (auto& ach : achievements) {
        if (!ach.checkCondition(progress)) continue;
        ach.unlock();
        wallet.addMoney(ach.getReward());
        unlockedMessages.push_back("ACHIEVEMENT: " + ach.getName() +
                                   "\nReward: $" + std::to_string(static_cast<long long>(ach.getReward())));
    }
    return unlockedMessages;
}

void Player::startBusinessProduction(int index) {
    businessAt(index).startProduction();
}

void Player::purchaseBusiness(int index) {
    Business& business = businessAt(index);
    if (business.isOwned()) {
        throw BusinessAlreadyOwnedException(business.getName());
    }
    wallet.spendMoney(discountedPrice(business.getPurchaseCost()));
    business.unlock();
}

void Player::upgradeBusiness(int index) {
    Business& business = businessAt(index);
    if (!business.isOwned()) {
        throw BusinessNotOwnedException(business.getName());
    }
    wallet.spendMoney(discountedPrice(business.getUpgradeCost()));
    business.levelUp();
}

void Player::hireManager(int index) {
    Business& business = businessAt(index);
    if (!business.isOwned()) {
        throw BusinessNotOwnedException(business.getName());
    }
    if (business.hasManagerHired()) {
        throw BusinessAlreadyOwnedException(business.getName() + " Manager");
    }
    wallet.spendMoney(managerPrice(business.getManagerCost()));
    business.hireManager();
}

void Player::upgradeManager(int index) {
    Business& business = businessAt(index);
    if (!business.hasManagerHired()) {
        throw BusinessNotOwnedException(business.getName() + " Manager");
    }
    wallet.spendMoney(managerPrice(business.getManagerUpgradeCost()));
    business.upgradeManager();
}

double Player::calculateOfflineEarnings(double secondsOffline) {
    if (secondsOffline <= 0.0) return 0.0;

    double boostedTime = 0.0;
    double normalTime = secondsOffline;

    if (tempBoostTimer > 0.0) {
        boostedTime = std::min(secondsOffline, tempBoostTimer);
        normalTime = secondsOffline - boostedTime;
        tempBoostTimer -= boostedTime;
    }

    double totalOfflineEarnings = 0.0;
    for (const auto& business : businesses) {
        if (!business->isOwned() || !business->hasManagerHired()) continue;

        const double cycleTime = business->getProductionTime() / globalSpeedMultiplier;
        if (cycleTime <= 0.0) continue;

        const double bonus = staticBonusFor(*business);
        totalOfflineEarnings += (boostedTime / cycleTime) * business->calculateRevenue(bonus * tempBoostMultiplier);
        totalOfflineEarnings += (normalTime / cycleTime) * business->calculateRevenue(bonus);
    }

    return totalOfflineEarnings * (offlineEarningsRatio / 0.5);
}

bool Player::canPrestige() const {
    if (wallet.getMoney() < 7000000.0) return false;
    if (businesses.empty()) return false;

    const auto& lastBusiness = businesses.back();
    return lastBusiness->isOwned() && lastBusiness->getLevel() >= 25;
}

int Player::calculatePrestigeGold() const {
    if (wallet.getMoney() < 7000000.0) return 0;
    const int baseGold = static_cast<int>(wallet.getMoney() / 3500000.0);
    return static_cast<int>(baseGold * (1.0 + prestigeGoldBonus));
}

int Player::prestige() {
    if (!canPrestige()) return 0;

    const int goldGained = calculatePrestigeGold();
    gold += goldGained;
    prestigeCount++;

    wallet = Wallet(0.0);
    for (auto& business : businesses) {
        business->reset();
    }
    tempBoostTimer = 0.0;

    return goldGained;
}

void Player::activateTempBoost(double duration, double multiplier) {
    tempBoostTimer += duration;
    tempBoostMultiplier = multiplier;
}

const std::vector<GoldUpgrade>& Player::goldUpgradeCatalog() {
    return catalog;
}

const GoldUpgrade& Player::findGoldUpgrade(int id) {
    for (const auto& upgrade : catalog) {
        if (upgrade.getId() == id) {
            return upgrade;
        }
    }
    throw UnknownUpgradeException(id);
}

bool Player::hasGoldUpgrade(int id) const {
    if (id < 0 || static_cast<size_t>(id) >= goldUpgradesOwned.size()) {
        return false;
    }
    return goldUpgradesOwned[static_cast<size_t>(id)];
}

bool Player::canAffordGoldUpgrade(int id) const {
    return gold >= findGoldUpgrade(id).getCost();
}

void Player::buyGoldUpgrade(int id) {
    const GoldUpgrade& upgrade = findGoldUpgrade(id);

    if (hasGoldUpgrade(id)) {
        throw UpgradeAlreadyOwnedException(upgrade.getLabel());
    }
    if (gold < upgrade.getCost()) {
        throw InsufficientGoldException(upgrade.getCost(), gold);
    }

    gold -= upgrade.getCost();
    goldUpgradesOwned[static_cast<size_t>(id)] = true;
    applyGoldUpgradeEffect(id);
}

void Player::applyGoldUpgradeEffect(int id) {
    switch (id) {
        case 1:  globalProfitMultiplier += 1.0; break;
        case 2:  globalDiscount = std::min(0.9, globalDiscount + 0.1); break;
        case 3:  globalSpeedMultiplier += 0.1; break;
        case 4:  globalProfitMultiplier += 0.25; break;
        case 5:  businessMultipliers["Limonada"] = 10.0; break;
        case 6:  globalProfitMultiplier += 0.5; break;
        case 7:  managerCostDiscount = std::min(0.9, managerCostDiscount + 0.5); break;
        case 8:  offlineEarningsRatio = 0.8; break;
        case 9:  globalDiscount = std::min(0.9, globalDiscount + 0.2); break;
        case 10: globalSpeedMultiplier += 0.2; break;
        case 11: prestigeGoldBonus += 0.1; break;
        case 12: globalProfitMultiplier += 4.0; break;
        case 13: businessMultipliers["Creveti"] = 5.0; break;
        default: throw UnknownUpgradeException(id);
    }
}

void Player::restoreProgress(double savedMoney, int savedGold, int savedPrestigeCount,
                             const std::vector<bool>& ownedUpgrades,
                             double boostTimer, double boostMultiplier) {
    wallet = Wallet(savedMoney);
    gold = savedGold;
    prestigeCount = savedPrestigeCount;
    tempBoostTimer = std::max(0.0, boostTimer);
    tempBoostMultiplier = boostMultiplier > 0.0 ? boostMultiplier : 1.0;

    globalProfitMultiplier = 1.0;
    globalDiscount = 0.0;
    globalSpeedMultiplier = 1.0;
    managerCostDiscount = 0.0;
    offlineEarningsRatio = 0.5;
    prestigeGoldBonus = 0.0;
    businessMultipliers.clear();
    goldUpgradesOwned.assign(catalog.size() + 1, false);

    for (const auto& upgrade : catalog) {
        const auto id = static_cast<size_t>(upgrade.getId());
        if (id < ownedUpgrades.size() && ownedUpgrades[id]) {
            goldUpgradesOwned[id] = true;
            applyGoldUpgradeEffect(upgrade.getId());
        }
    }
}

std::string Player::getBonusSummary() const {
    std::ostringstream summary;
    summary << std::fixed << std::setprecision(2);
    summary << "Profit x" << globalProfitMultiplier;
    summary << " | Viteza x" << globalSpeedMultiplier;
    summary << " | Reducere " << std::setprecision(0) << globalDiscount * 100.0 << "%";
    if (tempBoostTimer > 0.0) {
        summary << " | Boost x" << std::setprecision(1) << tempBoostMultiplier
                << " (" << static_cast<long long>(tempBoostTimer) << "s)";
    }
    return summary.str();
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << p.name << " | " << p.wallet << " | Gold: " << p.gold
       << " | Prestigii: " << p.prestigeCount << "\n";
    os << p.getBonusSummary() << "\n";
    for (const auto& business : p.businesses) {
        os << "  " << *business << "\n";
    }
    for (const auto& ach : p.achievements) {
        if (ach.isUnlocked()) {
            os << "  " << ach << "\n";
        }
    }
    return os;
}