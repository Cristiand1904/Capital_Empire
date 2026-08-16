#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "Game.h"

class Application {
private:
    std::unique_ptr<Game> game;
    sf::RenderWindow window;
    sf::Font font;
    sf::Clock clock;

    sf::Music bgMusic;
    sf::SoundBuffer clickBuffer;
    std::optional<sf::Sound> clickSound;
    sf::SoundBuffer cashBuffer;
    std::optional<sf::Sound> cashSound;
    sf::SoundBuffer errorBuffer;
    std::optional<sf::Sound> errorSound;
    sf::SoundBuffer achievementBuffer;
    std::optional<sf::Sound> achievementSound;

    enum class AppState { MENU, GAME, ACHIEVEMENTS, GOLD_SHOP, LUCKY_WHEEL, SEASON_INFO };
    AppState currentState;
    float stateTransitionTimer;

    struct Button {
        sf::FloatRect rect;
        std::string text;
        sf::Color color;
        int businessIndex;
        enum Type {
            BUY, UPGRADE, MANAGER, START, NEW_GAME, LOAD_GAME, SAVE_EXIT, RESET,
            SHOW_ACHIEVEMENTS, BACK, MAIN_MENU, PRESTIGE,
            SHOW_GOLD_SHOP, SHOW_LUCKY_WHEEL, SHOW_SEASON,
            BUY_GOLD_UPGRADE, SPIN_WHEEL
        } type;
        bool isPressed;
        int upgradeId;
    };
    std::vector<Button> gameButtons;
    std::vector<Button> menuButtons;
    std::vector<Button> achievementButtons;
    std::vector<Button> goldShopButtons;
    std::vector<Button> wheelButtons;
    std::vector<Button> seasonButtons;

    struct Notification {
        std::string text;
        float timer;
    };
    std::deque<Notification> notifications;

    struct FloatingText {
        std::string text;
        sf::Vector2f position;
        float lifeTime;
        sf::Color color;
    };
    std::vector<FloatingText> floatingTexts;

    float wheelSpinTimer;
    bool wheelIsSpinning;
    std::string wheelResultText;
    std::mt19937 randomEngine;

    void initUI();
    void initMenuUI();
    void initGameUI();
    void initAchievementUI();
    void initGoldShopUI();
    void initWheelUI();
    void initSeasonUI();
    void initAudio();

    static Button makeButton(sf::FloatRect rect, std::string text, sf::Color color,
                             Button::Type type, int businessIndex = -1, int upgradeId = -1);
    static Button makeBackButton(sf::Color color);

    void update(float dt);
    void draw();

    void updateMenu(float dt);
    void drawMenu();

    void updateGame(float dt);
    void updateGameNotifications(float dt);
    void updateGameInput();
    void updateGameButtonsState();
    void updateFloatingTexts(float dt);
    void handleButtonClick(const Button& btn);

    void updateAchievements(float dt);
    void drawAchievements();

    void updateGoldShop(float dt);
    void drawGoldShop();

    void updateWheel(float dt);
    void resolveWheelSpin();
    void drawWheel();

    void updateSeasonInfo(float dt);
    void drawSeasonInfo();

    void drawGame();
    void drawGameHeader();
    void drawGameBusinesses();
    void drawGameButtons();
    void drawGameNotifications();
    void drawFloatingTexts();

    void startGameSession();
    void saveQuietly();
    void pushNotification(const std::string& text);
    void reportError(const std::exception& error);

    static constexpr float LIST_TOP = 150.f;
    static constexpr float LIST_BOTTOM = 830.f;

    [[nodiscard]] size_t businessCount() const;
    [[nodiscard]] float businessRowHeight() const;
    [[nodiscard]] float businessPanelHeight() const;
    [[nodiscard]] float businessRowY(size_t index) const;

    void createBusinessUI(int index);
    bool isButtonClicked(Button& btn, const sf::Vector2i& mousePos);

    void drawButton(const Button& btn, bool isCircle = false);
    void drawText(const std::string& text, float x, float y, int size, sf::Color color);
    void drawMultilineText(const std::string& text, float x, float y, int size, sf::Color color);
    int getTextWidth(const std::string& text, int size);

    void spawnFloatingText(const std::string& text, float x, float y, sf::Color color);

public:
    Application();
    void run();
};