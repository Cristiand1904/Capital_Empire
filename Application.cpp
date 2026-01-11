#include "Application.h"
#include <iostream>
#include <cmath>
#include <filesystem>
#include <cstdint> // Added for std::uint8_t
#include <random> // For random numbers

#define COLOR_BG        sf::Color(30, 30, 35)
#define COLOR_PANEL     sf::Color(50, 50, 55)
#define COLOR_ACCENT    sf::Color(255, 200, 0)
#define COLOR_GREEN     sf::Color(100, 220, 50)
#define COLOR_RED       sf::Color(220, 60, 60)
#define COLOR_BLUE      sf::Color(60, 120, 220)
#define COLOR_ICON_IDLE sf::Color(255, 200, 0)
#define COLOR_ICON_RUN  sf::Color(150, 150, 150)
#define COLOR_TEXT      sf::Color(240, 240, 240)
#define COLOR_BLACK     sf::Color::Black
#define COLOR_WHITE     sf::Color::White
#define COLOR_GRAY      sf::Color(128, 128, 128)

Application::Application() {
    window.create(sf::VideoMode({1000, 900}), "Capital Empire - Ultimate Edition");
    window.setFramerateLimit(60);

    if (!font.openFromFile("Roboto-Regular.ttf")) {
        if (!font.openFromFile("fonts/Roboto-Regular.ttf")) {
            if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
                if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
                        std::cerr << "Failed to load font! Download Roboto-Regular.ttf from fonts.google.com\n";
                    }
                }
            }
        }
    }

    initAudio();

    currentState = AppState::MENU;
    stateTransitionTimer = 0.0f;

    game = std::make_unique<Game>("Capitalist", 0.0);
    initUI();
}

void Application::initAudio() {
    if (!bgMusic.openFromFile("music.mp3")) {
        std::cerr << "Failed to load music.mp3\n";
    } else {
        bgMusic.setLooping(true);
        bgMusic.setVolume(0.5);
        bgMusic.play();
    }

    if (!clickBuffer.loadFromFile("click.wav")) {
        std::cerr << "Failed to load click.wav\n";
    } else {
        clickSound.emplace(clickBuffer);
        clickSound->setVolume(3);
    }

    if (!cashBuffer.loadFromFile("cash.mp3")) {
        std::cerr << "Failed to load cash.mp3\n";
    } else {
        cashSound.emplace(cashBuffer);
        cashSound->setVolume(3);
    }

    if (!errorBuffer.loadFromFile("error.wav")) {
        std::cerr << "Failed to load error.wav\n";
    } else {
        errorSound.emplace(errorBuffer);
        errorSound->setVolume(3);
    }

    if (!achievementBuffer.loadFromFile("achievement.wav")) {
        std::cerr << "Failed to load achievement.wav\n";
    } else {
        achievementSound.emplace(achievementBuffer);
        achievementSound->setVolume(3);
    }
}

void Application::initUI() {
    initMenuUI();
    initAchievementUI();
    initGoldShopUI();
    initWheelUI();
    initGameUI();
}

void Application::initMenuUI() {
    menuButtons.clear();

    Button newGameBtn;
    newGameBtn.rect = sf::FloatRect({350, 250}, {300, 70});
    newGameBtn.text = "NEW GAME";
    newGameBtn.color = COLOR_GREEN;
    newGameBtn.type = Button::NEW_GAME;
    newGameBtn.businessIndex = -1;
    newGameBtn.isPressed = false;
    menuButtons.push_back(newGameBtn);

    if (Game::saveFileExists()) {
        Button loadGameBtn;
        loadGameBtn.rect = sf::FloatRect({350, 330}, {300, 70});
        loadGameBtn.text = "LOAD GAME";
        loadGameBtn.color = COLOR_BLUE;
        loadGameBtn.type = Button::LOAD_GAME;
        loadGameBtn.businessIndex = -1;
        loadGameBtn.isPressed = false;
        menuButtons.push_back(loadGameBtn);
    }

    Button achBtn;
    achBtn.rect = sf::FloatRect({350, 410}, {300, 70});
    achBtn.text = "ACHIEVEMENTS";
    achBtn.color = COLOR_ACCENT;
    achBtn.type = Button::SHOW_ACHIEVEMENTS;
    achBtn.businessIndex = -1;
    achBtn.isPressed = false;
    menuButtons.push_back(achBtn);
}

void Application::initAchievementUI() {
    achievementButtons.clear();
    Button backBtn;
    backBtn.rect = sf::FloatRect({20, 20}, {100, 50});
    backBtn.text = "BACK";
    backBtn.color = COLOR_RED;
    backBtn.type = Button::BACK;
    backBtn.businessIndex = -1;
    backBtn.isPressed = false;
    achievementButtons.push_back(backBtn);
}

void Application::initGoldShopUI() {
    goldShopButtons.clear();
    Button backBtn;
    backBtn.rect = sf::FloatRect({20, 20}, {100, 50});
    backBtn.text = "BACK";
    backBtn.color = COLOR_RED;
    backBtn.type = Button::BACK;
    backBtn.businessIndex = -1;
    backBtn.isPressed = false;
    goldShopButtons.push_back(backBtn);

    struct UpgradeInfo {
        std::string text;
        int cost;
    };

    UpgradeInfo upgrades[] = {
        {"Profit x2 (5 Gold)", 5},
        {"Discount 10% (10 Gold)", 10},
        {"Speed +10% (15 Gold)", 15},
        {"Small Biz Bonus +25% (3 Gold)", 3},
        {"Lemonade Mastery x10 (5 Gold)", 5},
        {"Corp Tax Cut +50% (8 Gold)", 8},
        {"Headhunter -50% Mng (12 Gold)", 12},
        {"Night Shift 80% Off (20 Gold)", 20},
        {"Bulk Buying -20% Upg (25 Gold)", 25},
        {"Automation Speed +20% (30 Gold)", 30},
        {"Golden Touch +10% Prestige (40 Gold)", 40},
        {"Market Monopoly x5 (50 Gold)", 50},
        {"Ocean King Shrimp x5 (60 Gold)", 60}
    };

    for (int i = 0; i < 13; ++i) {
        Button btn;
        float x = (i % 2 == 0) ? 50 : 520;
        float y = 100 + (i / 2) * 70;

        btn.rect = sf::FloatRect({x, y}, {430, 60});
        btn.text = upgrades[i].text;
        btn.color = COLOR_GRAY;
        btn.type = Button::BUY_GOLD_UPGRADE;
        btn.upgradeId = i + 1;
        btn.businessIndex = -1;
        btn.isPressed = false;
        goldShopButtons.push_back(btn);
    }
}

void Application::initWheelUI() {
    wheelButtons.clear();
    Button backBtn;
    backBtn.rect = sf::FloatRect({20, 20}, {100, 50});
    backBtn.text = "BACK";
    backBtn.color = COLOR_RED;
    backBtn.type = Button::BACK;
    backBtn.businessIndex = -1;
    backBtn.isPressed = false;
    wheelButtons.push_back(backBtn);

    Button spinBtn;
    spinBtn.rect = sf::FloatRect({350, 400}, {300, 100});
    spinBtn.text = "SPIN (1 Gold)";
    spinBtn.color = COLOR_ACCENT;
    spinBtn.type = Button::SPIN_WHEEL;
    spinBtn.businessIndex = -1;
    spinBtn.isPressed = false;
    wheelButtons.push_back(spinBtn);
}

void Application::initGameUI() {
    gameButtons.clear();
    for(int i=0; i<6; ++i) {
        createBusinessUI(i, 150 + i * 110);
    }

    Button menuBtn;
    menuBtn.rect = sf::FloatRect({140, 20}, {100, 50});
    menuBtn.text = "MENU";
    menuBtn.color = COLOR_GRAY;
    menuBtn.type = Button::MAIN_MENU;
    menuBtn.businessIndex = -1;
    menuBtn.isPressed = false;
    gameButtons.push_back(menuBtn);

    Button resetBtn;
    resetBtn.rect = sf::FloatRect({20, 20}, {100, 50});
    resetBtn.text = "RESET";
    resetBtn.color = COLOR_BLUE;
    resetBtn.type = Button::RESET;
    resetBtn.businessIndex = -1;
    resetBtn.isPressed = false;
    gameButtons.push_back(resetBtn);

    float bottomY = 840;
    float btnW = 180;
    float btnH = 50;

    Button goldShopBtn;
    goldShopBtn.rect = sf::FloatRect({400, bottomY}, {btnW, btnH});
    goldShopBtn.text = "GOLD SHOP";
    goldShopBtn.color = COLOR_ACCENT;
    goldShopBtn.type = Button::SHOW_GOLD_SHOP;
    goldShopBtn.businessIndex = -1;
    goldShopBtn.isPressed = false;
    gameButtons.push_back(goldShopBtn);

    Button wheelBtn;
    wheelBtn.rect = sf::FloatRect({600, bottomY}, {btnW, btnH});
    wheelBtn.text = "LUCKY WHEEL";
    wheelBtn.color = COLOR_RED;
    wheelBtn.type = Button::SHOW_LUCKY_WHEEL;
    wheelBtn.businessIndex = -1;
    wheelBtn.isPressed = false;
    gameButtons.push_back(wheelBtn);

    Button prestigeBtn;
    prestigeBtn.rect = sf::FloatRect({800, bottomY}, {btnW, btnH});
    prestigeBtn.text = "PRESTIGE";
    prestigeBtn.color = COLOR_GRAY;
    prestigeBtn.type = Button::PRESTIGE;
    prestigeBtn.businessIndex = -1;
    prestigeBtn.isPressed = false;
    gameButtons.push_back(prestigeBtn);
}

void Application::createBusinessUI(int index, float yPos) {
    Button startBtn;
    startBtn.rect = sf::FloatRect({50, yPos - 5}, {90, 90});
    startBtn.text = "GO!";
    startBtn.color = COLOR_ACCENT;
    startBtn.type = Button::START;
    startBtn.businessIndex = index;
    startBtn.isPressed = false;
    gameButtons.push_back(startBtn);

    Button buyBtn;
    buyBtn.rect = sf::FloatRect({760, yPos + 10}, {200, 60});
    buyBtn.text = "BUY";
    buyBtn.color = COLOR_ACCENT;
    buyBtn.type = Button::UPGRADE;
    buyBtn.businessIndex = index;
    buyBtn.isPressed = false;
    gameButtons.push_back(buyBtn);

    Button mngBtn;
    mngBtn.rect = sf::FloatRect({670, yPos + 10}, {80, 60});
    mngBtn.text = "M";
    mngBtn.color = COLOR_BLUE;
    mngBtn.type = Button::MANAGER;
    mngBtn.businessIndex = index;
    mngBtn.isPressed = false;
    gameButtons.push_back(mngBtn);
}

void Application::run() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                if (currentState == AppState::GAME) {
                    game->saveGame();
                }
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>()) {
                const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
                if (keyPressed->scancode == sf::Keyboard::Scancode::K && currentState == AppState::GAME) {
                    auto& businesses = game->getPlayer().getBusinesses();
                    if (!businesses.empty()) {
                        businesses[0]->setManagerHired(false);
                        spawnFloatingText("Manager Fired (Debug)", 500, 300, COLOR_RED);
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();
        update(dt);
        draw();
    }
}

bool Application::isButtonClicked(Button& btn, const sf::Vector2i& mousePos) {
    if (stateTransitionTimer > 0.0f) return false;

    bool hover = btn.rect.contains({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});

    if (hover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        btn.isPressed = true;
    } else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (btn.isPressed && hover) {
            btn.isPressed = false;
            return true;
        }
        btn.isPressed = false;
    }

    return false;
}

void Application::update(float dt) {
    if (stateTransitionTimer > 0.0f) {
        stateTransitionTimer -= dt;
    }

    if (currentState == AppState::MENU) {
        updateMenu(dt);
    } else if (currentState == AppState::ACHIEVEMENTS) {
        updateAchievements(dt);
    } else if (currentState == AppState::GOLD_SHOP) {
        updateGoldShop(dt);
    } else if (currentState == AppState::LUCKY_WHEEL) {
        updateWheel(dt);
    } else {
        updateGame(dt);
    }
}

void Application::updateMenu(float dt) {
    (void)dt;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    for (auto& btn : menuButtons) {
        if (isButtonClicked(btn, mousePos)) {
            if (clickSound.has_value()) clickSound->play();

            if (btn.type == Button::NEW_GAME) {
                game = std::make_unique<Game>("Capitalist", 0.0);
                initGameUI();
                currentState = AppState::GAME;
                stateTransitionTimer = 0.5f;
            } else if (btn.type == Button::LOAD_GAME) {
                game = std::make_unique<Game>("Capitalist", 0.0);
                if (game->loadGame()) {
                    initGameUI();
                    currentState = AppState::GAME;
                    stateTransitionTimer = 0.5f;

                    double offline = game->getOfflineEarnings();
                    if (offline > 0) {
                        spawnFloatingText("Welcome Back! Offline Earnings: $" + std::to_string((long long)offline), 500, 450, COLOR_GREEN);
                        if (cashSound.has_value()) cashSound->play();
                        game->resetOfflineEarnings();
                    }
                }
            } else if (btn.type == Button::SHOW_ACHIEVEMENTS) {
                currentState = AppState::ACHIEVEMENTS;
                stateTransitionTimer = 0.2f;
            }
        }
    }
}

void Application::updateAchievements(float dt) {
    (void)dt;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : achievementButtons) {
        if (isButtonClicked(btn, mousePos)) {
            if (clickSound.has_value()) clickSound->play();
            if (btn.type == Button::BACK) {
                currentState = AppState::MENU;
                stateTransitionTimer = 0.2f;
            }
        }
    }
}

void Application::updateGoldShop(float dt) {
    (void)dt;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : goldShopButtons) {
        if (btn.type == Button::BUY_GOLD_UPGRADE) {
            int cost = 0;
            if (btn.upgradeId == 1) cost = 5;
            else if (btn.upgradeId == 2) cost = 10;
            else if (btn.upgradeId == 3) cost = 15;
            else if (btn.upgradeId == 4) cost = 3;
            else if (btn.upgradeId == 5) cost = 5;
            else if (btn.upgradeId == 6) cost = 8;
            else if (btn.upgradeId == 7) cost = 12;
            else if (btn.upgradeId == 8) cost = 20;
            else if (btn.upgradeId == 9) cost = 25;
            else if (btn.upgradeId == 10) cost = 30;
            else if (btn.upgradeId == 11) cost = 40;
            else if (btn.upgradeId == 12) cost = 50;
            else if (btn.upgradeId == 13) cost = 60;

            bool owned = game->getPlayer().hasGoldUpgrade(btn.upgradeId);
            if (owned) {
                btn.color = COLOR_ACCENT;
            } else if (game->getPlayer().getGold() >= cost) {
                btn.color = COLOR_GREEN;
            } else {
                btn.color = COLOR_RED;
            }
        }

        if (isButtonClicked(btn, mousePos)) {
            if (btn.type == Button::BACK) {
                if (clickSound.has_value()) clickSound->play();
                currentState = AppState::GAME;
                stateTransitionTimer = 0.2f;
            } else if (btn.type == Button::BUY_GOLD_UPGRADE) {
                int cost = 0;
                if (btn.upgradeId == 1) cost = 5;
                else if (btn.upgradeId == 2) cost = 10;
                else if (btn.upgradeId == 3) cost = 15;
                else if (btn.upgradeId == 4) cost = 3;
                else if (btn.upgradeId == 5) cost = 5;
                else if (btn.upgradeId == 6) cost = 8;
                else if (btn.upgradeId == 7) cost = 12;
                else if (btn.upgradeId == 8) cost = 20;
                else if (btn.upgradeId == 9) cost = 25;
                else if (btn.upgradeId == 10) cost = 30;
                else if (btn.upgradeId == 11) cost = 40;
                else if (btn.upgradeId == 12) cost = 50;
                else if (btn.upgradeId == 13) cost = 60;

                if (game->getPlayer().hasGoldUpgrade(btn.upgradeId)) {
                    if (errorSound.has_value()) errorSound->play();
                } else if (game->getPlayer().getGold() >= cost) {
                    game->getPlayer().setGold(game->getPlayer().getGold() - cost);
                    game->getPlayer().setGoldUpgradeOwned(btn.upgradeId, true);

                    if (btn.upgradeId == 1) game->getPlayer().addGlobalProfitMultiplier(1.0);
                    else if (btn.upgradeId == 2) game->getPlayer().addGlobalDiscount(0.1);
                    else if (btn.upgradeId == 3) game->getPlayer().addGlobalSpeedMultiplier(0.1);
                    else if (btn.upgradeId == 4) game->getPlayer().addGlobalProfitMultiplier(0.25);
                    else if (btn.upgradeId == 5) game->getPlayer().setLemonadeMultiplier(game->getPlayer().getLemonadeMultiplier() * 10.0);
                    else if (btn.upgradeId == 6) game->getPlayer().addGlobalProfitMultiplier(0.5);
                    else if (btn.upgradeId == 7) game->getPlayer().setManagerCostDiscount(game->getPlayer().getManagerCostDiscount() + 0.5);
                    else if (btn.upgradeId == 8) game->getPlayer().setOfflineEarningsRatio(0.8);
                    else if (btn.upgradeId == 9) game->getPlayer().addGlobalDiscount(0.2);
                    else if (btn.upgradeId == 10) game->getPlayer().addGlobalSpeedMultiplier(0.2);
                    else if (btn.upgradeId == 11) game->getPlayer().setPrestigeGoldBonus(game->getPlayer().getPrestigeGoldBonus() + 0.1);
                    else if (btn.upgradeId == 12) game->getPlayer().addGlobalProfitMultiplier(4.0);
                    else if (btn.upgradeId == 13) game->getPlayer().setShrimpMultiplier(game->getPlayer().getShrimpMultiplier() * 5.0);

                    if (cashSound.has_value()) cashSound->play();
                    game->saveGame();
                } else {
                    if (errorSound.has_value()) errorSound->play();
                }
            }
        }
    }
}

static float wheelSpinTimer = 0.0f;
static bool isSpinning = false;
static std::string wheelResultText = "SPIN TO WIN!";

void Application::updateWheel(float dt) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (isSpinning) {
        wheelSpinTimer -= dt;
        if (wheelSpinTimer > 0) {
            int r = rand() % 7;
            if (r == 0) wheelResultText = "Jackpot 5 Gold";
            else if (r == 1) wheelResultText = "Mini 2 Gold";
            else if (r == 2) wheelResultText = "Refund 1 Gold";
            else if (r == 3) wheelResultText = "Cash (Small)";
            else if (r == 4) wheelResultText = "Cash (Big)";
            else if (r == 5) wheelResultText = "Profit Boost 1h";
            else wheelResultText = "GHINION";
        } else {
            isSpinning = false;
            int r = rand() % 100;
            if (r < 2) {
                wheelResultText = "JACKPOT! 5 GOLD";
                game->getPlayer().setGold(game->getPlayer().getGold() + 5);
                if (achievementSound.has_value()) achievementSound->play();
            } else if (r < 12) {
                wheelResultText = "MINI! 2 GOLD";
                game->getPlayer().setGold(game->getPlayer().getGold() + 2);
                if (achievementSound.has_value()) achievementSound->play();
            } else if (r < 30) {
                wheelResultText = "REFUND! 1 GOLD";
                game->getPlayer().setGold(game->getPlayer().getGold() + 1);
                if (cashSound.has_value()) cashSound->play();
            } else if (r < 50) {
                double reward = std::max(1000.0, game->getPlayer().getMoney() * 0.1);
                wheelResultText = "WON $" + std::to_string((long long)reward);
                game->getPlayer().setMoney(game->getPlayer().getMoney() + reward);
                if (cashSound.has_value()) cashSound->play();
            } else if (r < 60) {
                double reward = std::max(10000.0, game->getPlayer().getMoney() * 0.5);
                wheelResultText = "BIG WIN $" + std::to_string((long long)reward);
                game->getPlayer().setMoney(game->getPlayer().getMoney() + reward);
                if (cashSound.has_value()) cashSound->play();
            } else if (r < 75) {
                wheelResultText = "PROFIT BOOST 1H (x2)";
                game->getPlayer().activateTempBoost(3600.0, 2.0);
                if (achievementSound.has_value()) achievementSound->play();
            } else {
                wheelResultText = "GHINION (Nothing)";
                if (errorSound.has_value()) errorSound->play();
            }
            game->saveGame();
        }
        return;
    }

    for (auto& btn : wheelButtons) {
        if (isButtonClicked(btn, mousePos)) {
            if (btn.type == Button::BACK) {
                if (clickSound.has_value()) clickSound->play();
                currentState = AppState::GAME;
                stateTransitionTimer = 0.2f;
            } else if (btn.type == Button::SPIN_WHEEL) {
                if (game->getPlayer().getGold() >= 1) {
                    game->getPlayer().setGold(game->getPlayer().getGold() - 1);
                    isSpinning = true;
                    wheelSpinTimer = 2.0f;
                    if (clickSound.has_value()) clickSound->play();
                } else {
                    if (errorSound.has_value()) errorSound->play();
                }
            }
        }
    }
}

void Application::updateGame(float dt) {
    double oldMoney = game->getPlayer().getMoney();

    updateGameNotifications(dt);
    updateGameInput();
    updateGameButtonsState();
    updateFloatingTexts(dt);

    double newMoney = game->getPlayer().getMoney();
    if (newMoney > oldMoney) {
        spawnFloatingText("+$" + std::to_string((long long)(newMoney - oldMoney)), 850, 80, COLOR_GREEN);
    }
}

void Application::updateFloatingTexts(float dt) {
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->lifeTime -= dt;
        it->position.y -= 50.0f * dt;
        if (it->lifeTime <= 0) {
            it = floatingTexts.erase(it);
        } else {
            ++it;
        }
    }
}

void Application::spawnFloatingText(const std::string& text, float x, float y, sf::Color color) {
    floatingTexts.push_back({text, {x, y}, 1.5f, color});
}

void Application::updateGameNotifications(float dt) {
    std::vector<std::string> newNotifications = game->update(dt);
    for (const auto& msg : newNotifications) {
        notifications.push_back({msg, 3.0f});
        if (msg.find("ACHIEVEMENT:") == 0) {
            if (achievementSound.has_value()) achievementSound->play();
        }
    }

    if (!notifications.empty()) {
        notifications.front().timer -= dt;
        if (notifications.front().timer <= 0) {
            notifications.pop_front();
        }
    }
}

void Application::updateGameInput() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    for (auto& btn : gameButtons) {
        if (isButtonClicked(btn, mousePos)) {
            handleButtonClick(btn);
        }
    }
}

void Application::handleButtonClick(const Button& btn) {
    try {
        if (btn.type == Button::SAVE_EXIT) {
            if (clickSound.has_value()) clickSound->play();
            game->saveGame();
            window.close();
        } else if (btn.type == Button::RESET) {
            if (clickSound.has_value()) clickSound->play();
            if (Game::saveFileExists()) {
                std::filesystem::remove("savegame.txt");
            }
            game = std::make_unique<Game>("Capitalist", 0.0);
            notifications.push_back({"GAME RESET!", 2.0f});
        } else if (btn.type == Button::MAIN_MENU) {
            if (clickSound.has_value()) clickSound->play();
            game->saveGame();
            currentState = AppState::MENU;
            stateTransitionTimer = 0.2f;
            initMenuUI();
        } else if (btn.type == Button::SHOW_GOLD_SHOP) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::GOLD_SHOP;
            stateTransitionTimer = 0.2f;
        } else if (btn.type == Button::SHOW_LUCKY_WHEEL) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::LUCKY_WHEEL;
            stateTransitionTimer = 0.2f;
        } else if (btn.type == Button::PRESTIGE) {
            if (game->getPlayer().canPrestige()) {
                int gold = game->getPlayer().prestige();
                game->saveGame();
                initGameUI();
                spawnFloatingText("PRESTIGE! +" + std::to_string(gold) + " Gold", 500, 300, COLOR_ACCENT);
                if (achievementSound.has_value()) achievementSound->play();
            } else {
                if (errorSound.has_value()) errorSound->play();
            }
        } else if (btn.type == Button::START) {
            if (clickSound.has_value()) clickSound->play();
            game->getPlayer().startBusinessProduction(btn.businessIndex);
            spawnFloatingText("Working...", 100, 140 + btn.businessIndex * 110, COLOR_WHITE);
        } else if (btn.type == Button::UPGRADE) {
            const auto& businesses = game->getPlayer().getBusinesses();
            if (btn.businessIndex >= 0 && static_cast<size_t>(btn.businessIndex) < businesses.size()) {
                if (businesses[btn.businessIndex]->isOwned()) {
                    game->getPlayer().upgradeBusiness(btn.businessIndex);
                    spawnFloatingText("Upgraded!", 750, 140 + btn.businessIndex * 110, COLOR_GREEN);
                    if (cashSound.has_value()) cashSound->play();
                } else {
                    game->getPlayer().purchaseBusiness(btn.businessIndex);
                    spawnFloatingText("Purchased!", 750, 140 + btn.businessIndex * 110, COLOR_GREEN);
                    if (cashSound.has_value()) cashSound->play();
                }
            }
        } else if (btn.type == Button::MANAGER) {
            const auto& businesses = game->getPlayer().getBusinesses();
            if (businesses[btn.businessIndex]->hasManagerHired()) {
                game->getPlayer().upgradeManager(btn.businessIndex);
                spawnFloatingText("Manager Upgraded!", 680, 140 + btn.businessIndex * 110, COLOR_BLUE);
                if (cashSound.has_value()) cashSound->play();
            } else {
                game->getPlayer().hireManager(btn.businessIndex);
                spawnFloatingText("Manager Hired!", 680, 140 + btn.businessIndex * 110, COLOR_BLUE);
                if (cashSound.has_value()) cashSound->play();
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Eroare: " << e.what() << std::endl;
        spawnFloatingText("Error!", 500, 375, COLOR_RED);
        if (errorSound.has_value()) errorSound->play();
    }
}

void Application::updateGameButtonsState() {
    const auto& businesses = game->getPlayer().getBusinesses();
    double playerMoney = game->getPlayer().getMoney();

    for (auto& btn : gameButtons) {
        if (btn.type == Button::PRESTIGE) {
            if (game->getPlayer().canPrestige()) {
                btn.color = COLOR_ACCENT;
                btn.text = "PRESTIGE";
            } else {
                btn.color = COLOR_GRAY;
                btn.text = "PRESTIGE";
            }
        }

        if (btn.businessIndex >= 0 && static_cast<size_t>(btn.businessIndex) < businesses.size()) {
            const auto& b = businesses[btn.businessIndex];

            if (btn.type == Button::UPGRADE) {
                if (b->isOwned()) {
                    btn.text = "BUY x1\n$" + std::to_string((int)b->getUpgradeCost());
                    if (playerMoney >= b->getUpgradeCost()) {
                        btn.color = COLOR_GREEN;
                    } else {
                        btn.color = COLOR_ACCENT;
                    }
                } else {
                    if (b->getPurchaseCost() == 0) {
                        btn.text = "FREE!";
                        btn.color = COLOR_GREEN;
                    } else {
                        btn.text = "UNLOCK\n$" + std::to_string((int)b->getPurchaseCost());
                        if (playerMoney >= b->getPurchaseCost()) {
                            btn.color = COLOR_GREEN;
                        } else {
                            btn.color = COLOR_RED;
                        }
                    }
                }
            } else if (btn.type == Button::MANAGER) {
                if (b->hasManagerHired()) {
                    btn.text = "UPG\n$" + std::to_string((int)b->getManagerUpgradeCost());
                    if (playerMoney >= b->getManagerUpgradeCost()) {
                        btn.color = COLOR_GREEN;
                    } else {
                        btn.color = COLOR_GRAY;
                    }
                } else {
                    btn.text = "MNG\n$" + std::to_string((int)b->getManagerCost());
                    if (playerMoney >= b->getManagerCost()) {
                        btn.color = COLOR_GREEN;
                    } else {
                        btn.color = COLOR_GRAY;
                    }
                }
            } else if (btn.type == Button::START) {
                if (!b->isOwned()) {
                    btn.color = COLOR_GRAY;
                } else if (b->hasManagerHired()) {
                    btn.color = COLOR_GREEN;
                } else if (b->isActive()) {
                    btn.color = COLOR_ICON_RUN;
                } else {
                    btn.color = COLOR_ICON_IDLE;
                }
            }
        }
    }
}

void Application::draw() {
    window.clear(COLOR_BG);

    if (currentState == AppState::MENU) {
        drawMenu();
    } else if (currentState == AppState::ACHIEVEMENTS) {
        drawAchievements();
    } else if (currentState == AppState::GOLD_SHOP) {
        drawGoldShop();
    } else if (currentState == AppState::LUCKY_WHEEL) {
        drawWheel();
    } else {
        drawGame();
    }

    window.display();
}

void Application::drawMenu() {
    drawText("Capital Empire", 350, 100, 50, COLOR_ACCENT);

    for (const auto& btn : menuButtons) {
        drawButton(btn);
    }
}

void Application::drawAchievements() {
    drawText("Achievements", 350, 50, 40, COLOR_WHITE);

    const auto& achievements = game->getPlayer().getAchievements();
    float yPos = 120;

    for (const auto& ach : achievements) {
        sf::Color textColor = ach.isUnlocked() ? COLOR_GREEN : COLOR_GRAY;
        std::string status = ach.isUnlocked() ? "[UNLOCKED] " : "[LOCKED] ";

        std::string text = status + ach.getName() + ": " + ach.getDescription();

        drawText(text, 50, yPos, 24, textColor);
        yPos += 40;
    }

    for (const auto& btn : achievementButtons) {
        drawButton(btn);
    }
}

void Application::drawGoldShop() {
    drawText("Gold Shop", 400, 50, 40, COLOR_ACCENT);

    std::string goldStr = "Gold: " + std::to_string(game->getPlayer().getGold());
    drawText(goldStr, 800, 50, 30, COLOR_ACCENT);

    for (const auto& btn : goldShopButtons) {
        drawButton(btn);
    }
}

void Application::drawWheel() {
    drawText("Lucky Wheel", 380, 50, 40, COLOR_RED);

    std::string goldStr = "Gold: " + std::to_string(game->getPlayer().getGold());
    drawText(goldStr, 800, 50, 30, COLOR_ACCENT);

    int w = getTextWidth(wheelResultText, 40);
    drawText(wheelResultText, 500 - w/2, 300, 40, COLOR_WHITE);

    for (const auto& btn : wheelButtons) {
        drawButton(btn);
    }
}

void Application::drawGame() {
    drawGameHeader();
    drawGameBusinesses();
    drawGameButtons();
    drawGameNotifications();
    drawFloatingTexts();
}

void Application::drawFloatingTexts() {
    for (const auto& ft : floatingTexts) {
        sf::Color c = ft.color;
        c.a = static_cast<std::uint8_t>(255 * (ft.lifeTime / 1.5f));
        drawText(ft.text, ft.position.x, ft.position.y, 24, c);
    }
}

void Application::drawGameHeader() {
    sf::RectangleShape header(sf::Vector2f(1000, 100));
    header.setFillColor(COLOR_BLACK);
    window.draw(header);

    drawText("Capital Empire", 300, 30, 40, COLOR_WHITE);

    std::string moneyStr = "$" + std::to_string((long long)game->getPlayer().getMoney());
    int moneyWidth = getTextWidth(moneyStr, 50);
    drawText(moneyStr, 980 - moneyWidth, 25, 50, COLOR_ACCENT);

    std::string goldStr = "Gold: " + std::to_string(game->getPlayer().getGold());
    drawText(goldStr, 20, 860, 30, COLOR_ACCENT);
}

void Application::drawGameBusinesses() {
    const auto& businesses = game->getPlayer().getBusinesses();

    for (size_t i = 0; i < businesses.size(); ++i) {
        float yPos = 150 + i * 110;
        const auto& b = businesses[i];

        sf::RectangleShape panel(sf::Vector2f(960, 100));
        panel.setPosition({20, yPos - 10});
        panel.setFillColor(COLOR_PANEL);
        panel.setOutlineColor(COLOR_BLACK);
        panel.setOutlineThickness(2);
        window.draw(panel);

        float barX = 160;
        float barY = yPos + 45;
        float barW = 500;
        float barH = 35;

        sf::RectangleShape bar(sf::Vector2f(barW, barH));
        bar.setPosition({barX, barY});
        bar.setFillColor(COLOR_BLACK);
        window.draw(bar);

        if (b->isOwned()) {
            float progress = b->getProgress();
            sf::RectangleShape progressBar(sf::Vector2f(barW * progress, barH));
            progressBar.setPosition({barX, barY});
            progressBar.setFillColor(COLOR_GREEN);
            window.draw(progressBar);

            if (progress >= 1.0f) {
                sf::RectangleShape outline(sf::Vector2f(barW, barH));
                outline.setPosition({barX, barY});
                outline.setFillColor(sf::Color::Transparent);
                outline.setOutlineColor(COLOR_WHITE);
                outline.setOutlineThickness(2);
                window.draw(outline);
            }
        }

        std::string nameLvl = b->getName();
        if (b->isOwned()) nameLvl += "  [Lv " + std::to_string(b->getLevel()) + "]";
        drawText(nameLvl, barX, yPos + 5, 30, COLOR_WHITE);

        if (b->isOwned()) {
            std::string profitStr = "$" + std::to_string((int)b->getProfitPerCycle());
            drawText(profitStr, barX + 10, barY + 5, 24, COLOR_WHITE);

            char timeBuffer[16];
            snprintf(timeBuffer, sizeof(timeBuffer), "%.1fs", b->getProductionTime());
            drawText(timeBuffer, barX + barW - 80, barY + 5, 24, COLOR_WHITE);
        } else {
            drawText("LOCKED", barX + 200, barY + 5, 24, COLOR_RED);
        }
    }
}

void Application::drawGameButtons() {
    for (const auto& btn : gameButtons) {
        drawButton(btn, btn.type == Button::START);
    }
}

void Application::drawGameNotifications() {
    if (!notifications.empty()) {
        const auto& notif = notifications.front();
        int textWidth = getTextWidth(notif.text, 30);
        int boxW = textWidth + 60;
        int boxH = 120;
        int boxX = 500 - boxW / 2;
        int boxY = 375 - boxH / 2;

        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition({(float)boxX, (float)boxY});
        box.setFillColor(COLOR_BLACK);
        box.setOutlineColor(COLOR_ACCENT);
        box.setOutlineThickness(4);
        window.draw(box);

        if (notif.text.find('\n') != std::string::npos) {
            size_t pos = notif.text.find('\n');
            std::string line1 = notif.text.substr(0, pos);
            std::string line2 = notif.text.substr(pos + 1);

            int w1 = getTextWidth(line1, 30);
            int w2 = getTextWidth(line2, 30);

            drawText(line1, boxX + boxW/2 - w1/2, boxY + 20, 30, COLOR_ACCENT);
            drawText(line2, boxX + boxW/2 - w2/2, boxY + 60, 30, COLOR_WHITE);
        } else {
            drawText(notif.text, boxX + 30, boxY + 45, 30, COLOR_ACCENT);
        }
    }
}

void Application::drawButton(const Button& btn, bool isCircle) {
    sf::FloatRect drawRect = btn.rect;
    if (btn.isPressed) {
        drawRect.position.x += 2;
        drawRect.position.y += 2;
        drawRect.size.x -= 4;
        drawRect.size.y -= 4;
    }

    if (isCircle) {
        sf::CircleShape circle(drawRect.size.x / 2);
        circle.setPosition({drawRect.position.x, drawRect.position.y});
        circle.setFillColor(btn.color);
        circle.setOutlineColor(COLOR_BLACK);
        circle.setOutlineThickness(2);
        window.draw(circle);

        const char* iconText = "Click";
        if (btn.color == COLOR_GREEN) iconText = "Auto";
        int w = getTextWidth(iconText, 20);
        drawText(iconText, drawRect.position.x + drawRect.size.x/2 - w/2, drawRect.position.y + drawRect.size.y/2 - 10, 20, COLOR_BLACK);
    } else {
        sf::RectangleShape rect(sf::Vector2f(drawRect.size.x, drawRect.size.y));
        rect.setPosition({drawRect.position.x, drawRect.position.y});
        rect.setFillColor(btn.color);
        rect.setOutlineColor(COLOR_BLACK);
        rect.setOutlineThickness(2);
        window.draw(rect);

        if (btn.text.find('\n') != std::string::npos) {
            size_t pos = btn.text.find('\n');
            std::string line1 = btn.text.substr(0, pos);
            std::string line2 = btn.text.substr(pos + 1);

            int w1 = getTextWidth(line1, 20);
            int w2 = getTextWidth(line2, 20);

            drawText(line1, drawRect.position.x + drawRect.size.x/2 - w1/2, drawRect.position.y + 10, 20, COLOR_WHITE);
            drawText(line2, drawRect.position.x + drawRect.size.x/2 - w2/2, drawRect.position.y + 35, 20, COLOR_WHITE);
        } else {
            int w = getTextWidth(btn.text, 20);
            drawText(btn.text, drawRect.position.x + drawRect.size.x/2 - w/2, drawRect.position.y + drawRect.size.y/2 - 10, 20, COLOR_WHITE);
        }
    }
}

void Application::drawText(const std::string& text, float x, float y, int size, sf::Color color) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition({x, y});
    window.draw(sfText);
}

int Application::getTextWidth(const std::string& text, int size) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    return sfText.getLocalBounds().size.x;
}