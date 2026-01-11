#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Game.h"
#include <memory>
#include <vector>
#include <string>
#include <deque>
#include <optional>

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

    enum class AppState { MENU, GAME, ACHIEVEMENTS, GOLD_SHOP, LUCKY_WHEEL };
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
            SHOW_GOLD_SHOP, SHOW_LUCKY_WHEEL,
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

    void initUI();
    void initMenuUI();
    void initGameUI();
    void initAchievementUI();
    void initGoldShopUI();
    void initWheelUI();
    void initAudio();

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
    void drawWheel();

    void drawGame();
    void drawGameHeader();
    void drawGameBusinesses();
    void drawGameButtons();
    void drawGameNotifications();
    void drawFloatingTexts();

    void createBusinessUI(int index, float yPos);
    bool isButtonClicked(Button& btn, const sf::Vector2i& mousePos);

    void drawButton(const Button& btn, bool isCircle = false);
    void drawText(const std::string& text, float x, float y, int size, sf::Color color);
    int getTextWidth(const std::string& text, int size);

    void spawnFloatingText(const std::string& text, float x, float y, sf::Color color);

public:
    Application();
    void run();
};