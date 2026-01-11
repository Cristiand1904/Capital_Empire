#include "Game.h"
#include "Exceptions.h"
#include "StandardBusiness.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <ctime>

Game::Game(const std::string& playerName, double initialMoney)
    : player(playerName, initialMoney), offlineEarnings(0.0) {
    setupBusinesses();
}

void Game::setupBusinesses() {
    player.addBusiness(std::make_unique<StandardBusiness>("Limonada", BusinessType::LEMONADE, 1, 4, 0, 1.0, 100.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Inghetata", BusinessType::ICE_CREAM, 10, 40, 100, 3.0, 1000.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Restaurant", BusinessType::RESTAURANT, 100, 400, 1000, 10.0, 10000.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Pizza", BusinessType::PIZZA, 120, 360, 3000, 10.0, 15000.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Gogosi", BusinessType::DONUT, 500, 1500, 10000, 20.0, 50000.0));
    player.addBusiness(std::make_unique<StandardBusiness>("Creveti", BusinessType::SHRIMP, 2200, 6600, 40000, 45.0, 200000.0));
}

std::vector<std::string> Game::update(double deltaTime) {
    return player.update(deltaTime);
}

void Game::saveGame(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Eroare la salvarea jocului!\n";
        return;
    }

    std::time_t now = std::time(nullptr);
    outFile << now << "\n";

    outFile << player.getMoney() << "\n";
    outFile << player.getGold() << "\n";

    outFile << player.getGlobalProfitMultiplier() << " "
            << player.getGlobalDiscount() << " "
            << player.getGlobalSpeedMultiplier() << " "
            << player.getLemonadeMultiplier() << " "
            << player.getShrimpMultiplier() << " "
            << player.getManagerCostDiscount() << " "
            << player.getOfflineEarningsRatio() << " "
            << player.getPrestigeGoldBonus() << "\n";

    const auto& ownedUpgrades = player.getGoldUpgradesOwned();
    outFile << ownedUpgrades.size() << "\n";
    for (bool owned : ownedUpgrades) {
        outFile << owned << " ";
    }
    outFile << "\n";

    const auto& businesses = player.getBusinesses();
    outFile << businesses.size() << "\n";
    for (const auto& b : businesses) {
        outFile << b->getName() << " "
                << b->getLevel() << " "
                << b->isOwned() << " "
                << b->hasManagerHired() << " "
                << b->getManagerLevel() << " "
                << b->getProfitPerCycle() << " "
                << b->getUpgradeCost() << "\n";
    }

    const auto& achievements = player.getAchievements();
    outFile << achievements.size() << "\n";
    for (const auto& ach : achievements) {
        outFile << ach.isUnlocked() << "\n";
    }

    outFile.close();
    std::cout << "Joc salvat cu succes!\n";
}

bool Game::loadGame(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    std::time_t savedTime;
    inFile >> savedTime;

    double money;
    inFile >> money;
    player.setMoney(money);

    int gold;
    if (inFile >> gold) {
        player.setGold(gold);
    } else {
        player.setGold(0);
        inFile.clear();
    }

    double profitMult, discount, speedMult;
    double lemonadeMult, shrimpMult, managerDisc, offlineRatio, prestigeBonus;

    if (inFile >> profitMult >> discount >> speedMult >> lemonadeMult >> shrimpMult >> managerDisc >> offlineRatio >> prestigeBonus) {
        player.addGlobalProfitMultiplier(profitMult - player.getGlobalProfitMultiplier());
        player.addGlobalDiscount(discount - player.getGlobalDiscount());
        player.addGlobalSpeedMultiplier(speedMult - player.getGlobalSpeedMultiplier());

        player.setLemonadeMultiplier(lemonadeMult);
        player.setShrimpMultiplier(shrimpMult);
        player.setManagerCostDiscount(managerDisc);
        player.setOfflineEarningsRatio(offlineRatio);
        player.setPrestigeGoldBonus(prestigeBonus);

        size_t size;
        if (inFile >> size) {
            std::vector<bool> owned(size);
            for (size_t i = 0; i < size; ++i) {
                bool val;
                inFile >> val;
                owned[i] = val;
            }
            if (size < 14) {
                owned.resize(14, false);
            }
            player.setGoldUpgradesOwned(owned);
        }
    } else {
        inFile.clear();
    }

    int businessCount;
    inFile >> businessCount;

    const auto& businesses = player.getBusinesses();
    for (int i = 0; i < businessCount && i < (int)businesses.size(); ++i) {
        std::string name;
        int level;
        bool owned, hasManager;
        int managerLevel;
        double profit, upgradeCost;

        inFile >> name >> level >> owned >> hasManager >> managerLevel >> profit >> upgradeCost;

        businesses[i]->setLevel(level);
        businesses[i]->setOwned(owned);
        businesses[i]->setProfit(profit);
        businesses[i]->setUpgradeCost(upgradeCost);

        if (owned) {
            if (hasManager) {
                businesses[i]->hireManager();
                for (int j = 1; j < managerLevel; ++j) {
                    businesses[i]->upgradeManager();
                }
            }
        }
    }

    int achCount;
    inFile >> achCount;
    const auto& achievements = player.getAchievements();
    for (int i = 0; i < achCount && i < (int)achievements.size(); ++i) {
        bool unlocked;
        inFile >> unlocked;
        if (unlocked) {
            player.unlockAchievement(achievements[i].getName());
        }
    }

    inFile.close();

    std::time_t now = std::time(nullptr);
    double secondsOffline = std::difftime(now, savedTime);

    if (secondsOffline > 0) {
        double earnings = player.calculateOfflineEarnings(secondsOffline);
        offlineEarnings = earnings * 0.5;
        if (offlineEarnings > 0) {
            player.setMoney(player.getMoney() + offlineEarnings);
        }
    }

    return true;
}

bool Game::saveFileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}