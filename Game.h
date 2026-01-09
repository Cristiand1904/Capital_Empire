#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Player.h"
#include "Business.h"

class Game {
private:
    Player player;
    double offlineEarnings; // Store offline earnings to display in UI
    void setupBusinesses();

public:
    Game(const std::string& playerName, double initialMoney);

    std::vector<std::string> update(double deltaTime);

    Player& getPlayer() { return player; }
    const Player& getPlayer() const { return player; }

    void saveGame(const std::string& filename = "savegame.txt") const;
    bool loadGame(const std::string& filename = "savegame.txt");
    static bool saveFileExists(const std::string& filename = "savegame.txt");

    double getOfflineEarnings() const { return offlineEarnings; }
    void resetOfflineEarnings() { offlineEarnings = 0.0; }
};