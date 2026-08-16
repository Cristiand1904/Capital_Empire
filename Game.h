#pragma once
#include <string>
#include <vector>
#include "Player.h"

class Game {
private:
    Player player;
    double offlineEarnings;
    bool seasonBonusAnnounced;

    static const std::string SAVE_HEADER;
    static const int SAVE_VERSION;

    void setupBusinesses();
    std::vector<std::string> updateSpecialBusinesses(double deltaTime);

public:
    Game(const std::string& playerName, double initialMoney);

    std::vector<std::string> update(double deltaTime);

    Player& getPlayer();
    [[nodiscard]] const Player& getPlayer() const;

    void saveGame(const std::string& filename = "savegame.txt") const;
    bool loadGame(const std::string& filename = "savegame.txt");
    static bool saveFileExists(const std::string& filename = "savegame.txt");

    [[nodiscard]] std::string getSeasonInfo() const;
    std::vector<std::string> applyRandomSeasonalBoosts();

    [[nodiscard]] double getOfflineEarnings() const;
    void resetOfflineEarnings();

    friend std::ostream& operator<<(std::ostream& os, const Game& g);
};
