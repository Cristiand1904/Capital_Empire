#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include <memory>
#include <vector>
#include <string>
#include <deque>

class Application {
private:
    std::unique_ptr<Game> game;
    sf::RenderWindow window;
    sf::Font font;
    sf::Clock clock;

    enum class AppState { MENU, GAME, ACHIEVEMENTS }; // Added ACHIEVEMENTS state
    AppState currentState;
    float stateTransitionTimer;

    struct Button {
        sf::FloatRect rect;
        std::string text;
        sf::Color color;
        int businessIndex;
        enum Type { BUY, UPGRADE, MANAGER, START, NEW_GAME, LOAD_GAME, SAVE_EXIT, RESET, SHOW_ACHIEVEMENTS, BACK, MAIN_MENU } type;
        bool isPressed;
    };
    std::vector<Button> gameButtons;
    std::vector<Button> menuButtons;
    std::vector<Button> achievementButtons; // Buttons for the achievement screen

    struct Notification {
        std::string text;
        float timer;
    };
    std::deque<Notification> notifications;

    // Floating Text System (Juice)
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
    void initAchievementUI(); // New init

    void update(float dt);
    void draw();

    void updateMenu(float dt);
    void drawMenu();

    void updateGame(float dt);
    void updateGameNotifications(float dt);
    void updateGameInput();
    void updateGameButtonsState();
    void updateFloatingTexts(float dt); // Update floating texts
    void handleButtonClick(const Button& btn);

    void updateAchievements(float dt); // Update achievement screen
    void drawAchievements(); // Draw achievement screen

    void drawGame();
    void drawGameHeader();
    void drawGameBusinesses();
    void drawGameButtons();
    void drawGameNotifications();
    void drawFloatingTexts(); // Draw floating texts

    void createBusinessUI(int index, float yPos);
    bool isButtonClicked(Button& btn, const sf::Vector2i& mousePos);

    void drawButton(const Button& btn, bool isCircle = false);
    void drawText(const std::string& text, float x, float y, int size, sf::Color color);
    int getTextWidth(const std::string& text, int size);

    // Helper to spawn text
    void spawnFloatingText(const std::string& text, float x, float y, sf::Color color);

public:
    Application();
    void run();
};