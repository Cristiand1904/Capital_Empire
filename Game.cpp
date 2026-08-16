#include "Game.h"
#include "Exceptions.h"
#include "FranchiseBusiness.h"
#include "PremiumBusiness.h"
#include "SeasonalBusiness.h"
#include "StandardBusiness.h"
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>

const std::string Game::SAVE_HEADER = "CAPITAL_EMPIRE_SAVE";
const int Game::SAVE_VERSION = 2;

Game::Game(const std::string& playerName, double initialMoney)
    : player(playerName, initialMoney), offlineEarnings(0.0), seasonBonusAnnounced(false) {
    setupBusinesses();
}

void Game::setupBusinesses() {
    player.addBusiness(std::make_unique<StandardBusiness>("Limonada", BusinessType::LEMONADE, 1, 4, 0, 1.0, 100.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Pizza", BusinessType::PIZZA, 10, 40, 100, 3.0, 1000.0));
    player.addBusiness(std::make_unique<SeasonalBusiness>("Inghetata", 100, 400, 1000, 10.0, 10000.0, Season::SUMMER));
    player.addBusiness(std::make_unique<PremiumBusiness>("Restaurant", 120, 360, 3000, 10.0, 15000.0, 1.75));
    player.addBusiness(std::make_unique<SeasonalBusiness>("Gogosi", 500, 1500, 10000, 20.0, 50000.0, Season::WINTER));
    player.addBusiness(std::make_unique<FranchiseBusiness>("Cafenea", 900, 2700, 20000, 30.0, 100000.0, 5, 0.25));
    player.addBusiness(std::make_unique<SeasonalBusiness>("Creveti", 2200, 6600, 40000, 45.0, 200000.0, Season::SUMMER));
}

Player& Game::getPlayer() { return player; }
const Player& Game::getPlayer() const { return player; }

double Game::getOfflineEarnings() const { return offlineEarnings; }
void Game::resetOfflineEarnings() { offlineEarnings = 0.0; }

std::string Game::getSeasonInfo() const {
    std::ostringstream info;
    info << "Sezon: " << SeasonalBusiness::seasonName(SeasonalBusiness::getCurrentSeason()) << "\n\n";
    info << "Bonusuri sezoniere active (x2 profit):\n";

    bool foundActive = false;
    for (const auto& business : player.getBusinesses()) {
        if (!business->isOwned()) continue;

        const auto* seasonal = dynamic_cast<const SeasonalBusiness*>(business.get());
        if (seasonal != nullptr && seasonal->isInFavoriteSeason()) {
            info << "- " << seasonal->getName() << " (x2 profit)\n";
            foundActive = true;
        }
    }

    if (!foundActive) {
        info << "- Nicio afacere sezoniera activa\n";
    }
    return info.str();
}

std::vector<std::string> Game::applyRandomSeasonalBoosts() {
    std::vector<std::string> boostedNames;
    const auto& businesses = player.getBusinesses();

    std::vector<size_t> ownedIndices;
    for (size_t i = 0; i < businesses.size(); ++i) {
        businesses[i]->setSessionMultiplier(1.0);
        if (businesses[i]->isOwned()) {
            ownedIndices.push_back(i);
        }
    }

    if (ownedIndices.empty()) return boostedNames;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(ownedIndices.begin(), ownedIndices.end(), gen);

    const size_t boostsToApply = std::min<size_t>(2, ownedIndices.size());
    for (size_t i = 0; i < boostsToApply; ++i) {
        businesses[ownedIndices[i]]->setSessionMultiplier(1.5);
        boostedNames.push_back(businesses[ownedIndices[i]]->getName());
    }
    return boostedNames;
}

std::vector<std::string> Game::updateSpecialBusinesses(double deltaTime) {
    std::vector<std::string> messages;

    for (const auto& business : player.getBusinesses()) {
        if (!business->isOwned()) continue;

        if (auto* premium = dynamic_cast<PremiumBusiness*>(business.get())) {
            if (premium->accumulateLoyalty(deltaTime)) {
                messages.push_back("LOYALTY BONUS!\n" + premium->getName() + " a atins " +
                                   std::to_string(premium->getLoyaltyPoints()) + " puncte de loialitate!");
            }
        }

        if (auto* seasonal = dynamic_cast<SeasonalBusiness*>(business.get())) {
            if (!seasonBonusAnnounced && seasonal->isInFavoriteSeason() && seasonal->getLevel() >= 5) {
                messages.push_back("SEASONAL BONUS!\n" + seasonal->getName() + " este in sezon de varf! x" +
                                   std::to_string(static_cast<int>(seasonal->getSeasonalMultiplier())) + " profit!");
                seasonBonusAnnounced = true;
            }
        }
    }
    return messages;
}

std::vector<std::string> Game::update(double deltaTime) {
    std::vector<std::string> messages = player.update(deltaTime);
    const std::vector<std::string> specialMessages = updateSpecialBusinesses(deltaTime);
    messages.insert(messages.end(), specialMessages.begin(), specialMessages.end());
    return messages;
}

void Game::saveGame(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw SaveFileException(filename, "nu poate fi deschis pentru scriere");
    }

    outFile << std::setprecision(std::numeric_limits<double>::max_digits10);
    outFile << SAVE_HEADER << " " << SAVE_VERSION << "\n";
    outFile << std::time(nullptr) << "\n";
    outFile << player.getMoney() << " " << player.getGold() << " " << player.getPrestigeCount() << "\n";
    outFile << player.getTempBoostTimer() << " " << player.getTempBoostMultiplier() << "\n";

    const auto& ownedUpgrades = player.getGoldUpgradesOwned();
    outFile << ownedUpgrades.size() << "\n";
    for (const bool owned : ownedUpgrades) {
        outFile << owned << " ";
    }
    outFile << "\n";

    const auto& businesses = player.getBusinesses();
    outFile << businesses.size() << "\n";
    for (const auto& business : businesses) {
        outFile << business->getName() << " "
                << business->getLevel() << " "
                << business->isOwned() << " "
                << business->hasManagerHired() << " "
                << business->getManagerLevel() << " "
                << business->getProfitPerCycle() << " "
                << business->getUpgradeCost() << "\n";
    }

    const auto& achievements = player.getAchievements();
    outFile << achievements.size() << "\n";
    for (const auto& ach : achievements) {
        outFile << ach.isUnlocked() << "\n";
    }

    if (!outFile.good()) {
        throw SaveFileException(filename, "scrierea nu s-a incheiat corect");
    }
}

bool Game::loadGame(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    std::string header;
    int version = 0;
    if (!(inFile >> header >> version) || header != SAVE_HEADER) {
        throw SaveFileException(filename, "format vechi sau necunoscut, porneste un joc nou");
    }
    if (version != SAVE_VERSION) {
        throw SaveFileException(filename, "versiune incompatibila (" + std::to_string(version) + ")");
    }

    std::time_t savedTime = 0;
    double money = 0.0;
    int savedGold = 0;
    int savedPrestige = 0;
    double boostTimer = 0.0;
    double boostMultiplier = 1.0;
    size_t upgradeCount = 0;

    if (!(inFile >> savedTime >> money >> savedGold >> savedPrestige >> boostTimer >> boostMultiplier >> upgradeCount)) {
        throw SaveFileException(filename, "datele jucatorului sunt corupte");
    }

    std::vector<bool> ownedUpgrades(upgradeCount, false);
    for (size_t i = 0; i < upgradeCount; ++i) {
        bool value = false;
        if (!(inFile >> value)) {
            throw SaveFileException(filename, "lista de upgrade-uri gold este corupta");
        }
        ownedUpgrades[i] = value;
    }

    player.restoreProgress(money, savedGold, savedPrestige, ownedUpgrades, boostTimer, boostMultiplier);

    size_t businessCount = 0;
    if (!(inFile >> businessCount)) {
        throw SaveFileException(filename, "numarul de afaceri lipseste");
    }

    const auto& businesses = player.getBusinesses();
    for (size_t i = 0; i < businessCount; ++i) {
        std::string savedName;
        int level = 0;
        bool owned = false;
        bool hasManager = false;
        int managerLevel = 0;
        double profit = 0.0;
        double upgradeCost = 0.0;

        if (!(inFile >> savedName >> level >> owned >> hasManager >> managerLevel >> profit >> upgradeCost)) {
            throw SaveFileException(filename, "datele afacerilor sunt corupte");
        }

        const auto match = std::find_if(businesses.begin(), businesses.end(),
                                        [&savedName](const std::unique_ptr<Business>& candidate) {
                                            return candidate->getName() == savedName;
                                        });
        if (match != businesses.end()) {
            (*match)->restoreState(level, owned, hasManager, managerLevel, profit, upgradeCost);
        }
    }

    size_t achievementCount = 0;
    if (!(inFile >> achievementCount)) {
        throw SaveFileException(filename, "numarul de realizari lipseste");
    }

    const auto& achievements = player.getAchievements();
    for (size_t i = 0; i < achievementCount; ++i) {
        bool unlocked = false;
        if (!(inFile >> unlocked)) {
            throw SaveFileException(filename, "datele realizarilor sunt corupte");
        }
        if (unlocked && i < achievements.size()) {
            player.unlockAchievement(achievements[i].getName());
        }
    }

    const double secondsOffline = std::difftime(std::time(nullptr), savedTime);
    if (secondsOffline > 0.0) {
        offlineEarnings = player.calculateOfflineEarnings(secondsOffline) * 0.5;
        player.addMoney(offlineEarnings);
    }
    return true;
}

bool Game::saveFileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << g.player;
    os << g.getSeasonInfo();
    return os;
}