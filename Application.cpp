#include "Application.h"
#include "Exceptions.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>

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

Application::Application()
    : currentState(AppState::MENU), stateTransitionTimer(0.0f),
      wheelSpinTimer(0.0f), wheelIsSpinning(false), wheelResultText("SPIN TO WIN!"),
      randomEngine(std::random_device{}()) {
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

Application::Button Application::makeButton(sf::FloatRect rect, std::string text, sf::Color color,
                                            Button::Type type, int businessIndex, int upgradeId) {
    Button btn;
    btn.rect = rect;
    btn.text = std::move(text);
    btn.color = color;
    btn.type = type;
    btn.businessIndex = businessIndex;
    btn.upgradeId = upgradeId;
    btn.isPressed = false;
    return btn;
}

Application::Button Application::makeBackButton(sf::Color color) {
    return makeButton({{20.f, 20.f}, {100.f, 50.f}}, "BACK", color, Button::BACK);
}

void Application::initUI() {
    initMenuUI();
    initAchievementUI();
    initGoldShopUI();
    initWheelUI();
    initSeasonUI();
    initGameUI();
}

void Application::initMenuUI() {
    menuButtons.clear();
    menuButtons.push_back(makeButton({{350.f, 250.f}, {300.f, 70.f}}, "NEW GAME", COLOR_GREEN, Button::NEW_GAME));

    if (Game::saveFileExists()) {
        menuButtons.push_back(makeButton({{350.f, 330.f}, {300.f, 70.f}}, "LOAD GAME", COLOR_BLUE, Button::LOAD_GAME));
    }

    menuButtons.push_back(makeButton({{350.f, 410.f}, {300.f, 70.f}}, "ACHIEVEMENTS", COLOR_ACCENT,
                                     Button::SHOW_ACHIEVEMENTS));
}

void Application::initAchievementUI() {
    achievementButtons.clear();
    achievementButtons.push_back(makeBackButton(COLOR_RED));
}

void Application::initSeasonUI() {
    seasonButtons.clear();
    seasonButtons.push_back(makeBackButton(COLOR_RED));
}

void Application::initGoldShopUI() {
    goldShopButtons.clear();
    goldShopButtons.push_back(makeBackButton(COLOR_RED));

    const auto& catalog = game->getPlayer().goldUpgradeCatalog();
    for (size_t i = 0; i < catalog.size(); ++i) {
        const float x = (i % 2 == 0) ? 50.f : 520.f;
        const float y = 130.f + static_cast<float>(i / 2) * 70.f;
        goldShopButtons.push_back(makeButton({{x, y}, {430.f, 60.f}}, catalog[i].getButtonText(),
                                             COLOR_GREEN, Button::BUY_GOLD_UPGRADE, -1, catalog[i].getId()));
    }
}

void Application::initWheelUI() {
    wheelButtons.clear();
    wheelButtons.push_back(makeBackButton(COLOR_RED));
    wheelButtons.push_back(makeButton({{350.f, 400.f}, {300.f, 100.f}}, "SPIN (1 Gold)", COLOR_ACCENT,
                                      Button::SPIN_WHEEL));
}

size_t Application::businessCount() const {
    return std::max<size_t>(1, game->getPlayer().getBusinesses().size());
}

float Application::businessRowHeight() const {
    return (LIST_BOTTOM - LIST_TOP) / static_cast<float>(businessCount());
}

float Application::businessPanelHeight() const {
    return businessRowHeight() - 13.f;
}

float Application::businessRowY(size_t index) const {
    return LIST_TOP + static_cast<float>(index) * businessRowHeight();
}

void Application::initGameUI() {
    gameButtons.clear();
    for (int i = 0; i < static_cast<int>(game->getPlayer().getBusinesses().size()); ++i) {
        createBusinessUI(i);
    }

    gameButtons.push_back(makeButton({{20.f, 20.f}, {100.f, 50.f}}, "RESET", COLOR_BLUE, Button::RESET));
    gameButtons.push_back(makeButton({{140.f, 20.f}, {100.f, 50.f}}, "MENU", COLOR_GRAY, Button::MAIN_MENU));

    const float bottomY = 840.f;
    const float btnW = 180.f;
    const float btnH = 50.f;

    gameButtons.push_back(makeButton({{200.f, bottomY}, {btnW, btnH}}, "SEZON", COLOR_GREEN, Button::SHOW_SEASON));
    gameButtons.push_back(makeButton({{400.f, bottomY}, {btnW, btnH}}, "GOLD SHOP", COLOR_ACCENT,
                                     Button::SHOW_GOLD_SHOP));
    gameButtons.push_back(makeButton({{600.f, bottomY}, {btnW, btnH}}, "LUCKY WHEEL", COLOR_RED,
                                     Button::SHOW_LUCKY_WHEEL));
    gameButtons.push_back(makeButton({{800.f, bottomY}, {btnW, btnH}}, "PRESTIGE", COLOR_GRAY, Button::PRESTIGE));
}

void Application::createBusinessUI(int index) {
    const float yPos = businessRowY(static_cast<size_t>(index));
    const float panelH = businessPanelHeight();

    const float circleSize = std::min(90.f, panelH - 8.f);
    const float actionH = std::min(60.f, panelH - 20.f);
    const float circleY = yPos - 10.f + (panelH - circleSize) / 2.f;
    const float actionY = yPos - 10.f + (panelH - actionH) / 2.f;

    gameButtons.push_back(makeButton({{50.f, circleY}, {circleSize, circleSize}}, "GO!", COLOR_ACCENT,
                                     Button::START, index));
    gameButtons.push_back(makeButton({{760.f, actionY}, {200.f, actionH}}, "BUY", COLOR_ACCENT,
                                     Button::UPGRADE, index));
    gameButtons.push_back(makeButton({{670.f, actionY}, {80.f, actionH}}, "M", COLOR_BLUE,
                                     Button::MANAGER, index));
}

void Application::startGameSession() {
    initGameUI();
    initGoldShopUI();
    currentState = AppState::GAME;
    stateTransitionTimer = 0.5f;

    const std::vector<std::string> boosted = game->applyRandomSeasonalBoosts();
    if (boosted.empty()) return;

    std::ostringstream message;
    message << "BONUS DE SESIUNE x1.5\n";
    for (size_t i = 0; i < boosted.size(); ++i) {
        if (i > 0) message << ", ";
        message << boosted[i];
    }
    pushNotification(message.str());
}

void Application::pushNotification(const std::string& text) {
    notifications.push_back({text, 3.0f});
}

void Application::reportError(const std::exception& error) {
    pushNotification(std::string("EROARE\n") + error.what());
    if (errorSound.has_value()) errorSound->play();
}

void Application::saveQuietly() {
    try {
        game->saveGame();
    } catch (const GameException& error) {
        reportError(error);
    }
}

void Application::run() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                if (currentState == AppState::GAME) {
                    saveQuietly();
                }
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::K && currentState == AppState::GAME) {
                    const auto& businesses = game->getPlayer().getBusinesses();
                    if (!businesses.empty()) {
                        businesses[0]->setManagerHired(false);
                        spawnFloatingText("Manager Fired (Debug)", 500, 300, COLOR_RED);
                    }
                }
            }
        }

        const float dt = clock.restart().asSeconds();
        update(dt);
        draw();
    }
}

bool Application::isButtonClicked(Button& btn, const sf::Vector2i& mousePos) {
    if (stateTransitionTimer > 0.0f) return false;

    const bool hover = btn.rect.contains({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});

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

    if (!notifications.empty()) {
        notifications.front().timer -= dt;
        if (notifications.front().timer <= 0.0f) {
            notifications.pop_front();
        }
    }
    updateFloatingTexts(dt);

    switch (currentState) {
        case AppState::MENU:         updateMenu(dt); break;
        case AppState::ACHIEVEMENTS: updateAchievements(dt); break;
        case AppState::GOLD_SHOP:    updateGoldShop(dt); break;
        case AppState::LUCKY_WHEEL:  updateWheel(dt); break;
        case AppState::SEASON_INFO:  updateSeasonInfo(dt); break;
        case AppState::GAME:         updateGame(dt); break;
    }
}

void Application::updateMenu(float dt) {
    (void)dt;
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    for (auto& btn : menuButtons) {
        if (!isButtonClicked(btn, mousePos)) continue;
        if (clickSound.has_value()) clickSound->play();

        if (btn.type == Button::NEW_GAME) {
            try {
                game = std::make_unique<Game>("Capitalist", 0.0);
                startGameSession();
            } catch (const GameException& error) {
                reportError(error);
            }
        } else if (btn.type == Button::LOAD_GAME) {
            auto loaded = std::make_unique<Game>("Capitalist", 0.0);
            try {
                if (!loaded->loadGame()) continue;
                game = std::move(loaded);
                startGameSession();

                const double offline = game->getOfflineEarnings();
                if (offline > 0.0) {
                    spawnFloatingText("Welcome Back! Offline Earnings: $" +
                                      std::to_string(static_cast<long long>(offline)), 300, 450, COLOR_GREEN);
                    if (cashSound.has_value()) cashSound->play();
                    game->resetOfflineEarnings();
                }
            } catch (const GameException& error) {
                reportError(error);
            }
        } else if (btn.type == Button::SHOW_ACHIEVEMENTS) {
            currentState = AppState::ACHIEVEMENTS;
            stateTransitionTimer = 0.2f;
        }
    }
}

void Application::updateAchievements(float dt) {
    (void)dt;
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : achievementButtons) {
        if (isButtonClicked(btn, mousePos) && btn.type == Button::BACK) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::MENU;
            stateTransitionTimer = 0.2f;
        }
    }
}

void Application::updateSeasonInfo(float dt) {
    (void)dt;
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : seasonButtons) {
        if (isButtonClicked(btn, mousePos) && btn.type == Button::BACK) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::GAME;
            stateTransitionTimer = 0.2f;
        }
    }
}

void Application::updateGoldShop(float dt) {
    (void)dt;
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    const Player& player = game->getPlayer();

    for (auto& btn : goldShopButtons) {
        if (btn.type == Button::BUY_GOLD_UPGRADE) {
            if (player.hasGoldUpgrade(btn.upgradeId)) {
                btn.color = COLOR_ACCENT;
            } else {
                btn.color = player.canAffordGoldUpgrade(btn.upgradeId) ? COLOR_GREEN : COLOR_RED;
            }
        }

        if (!isButtonClicked(btn, mousePos)) continue;

        if (btn.type == Button::BACK) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::GAME;
            stateTransitionTimer = 0.2f;
        } else if (btn.type == Button::BUY_GOLD_UPGRADE) {
            try {
                game->getPlayer().buyGoldUpgrade(btn.upgradeId);
                pushNotification("UPGRADE CUMPARAT\n" +
                                 game->getPlayer().findGoldUpgrade(btn.upgradeId).getDescription());
                if (cashSound.has_value()) cashSound->play();
                saveQuietly();
            } catch (const GameException& error) {
                reportError(error);
            }
        }
    }
}

void Application::resolveWheelSpin() {
    std::uniform_int_distribution<int> roll(0, 99);
    const int result = roll(randomEngine);
    Player& player = game->getPlayer();

    if (result < 2) {
        wheelResultText = "JACKPOT! 5 GOLD";
        player.addGold(5);
        if (achievementSound.has_value()) achievementSound->play();
    } else if (result < 12) {
        wheelResultText = "MINI! 2 GOLD";
        player.addGold(2);
        if (achievementSound.has_value()) achievementSound->play();
    } else if (result < 30) {
        wheelResultText = "REFUND! 1 GOLD";
        player.addGold(1);
        if (cashSound.has_value()) cashSound->play();
    } else if (result < 50) {
        const double reward = std::max(1000.0, player.getMoney() * 0.1);
        wheelResultText = "WON $" + std::to_string(static_cast<long long>(reward));
        player.addMoney(reward);
        if (cashSound.has_value()) cashSound->play();
    } else if (result < 60) {
        const double reward = std::max(10000.0, player.getMoney() * 0.5);
        wheelResultText = "BIG WIN $" + std::to_string(static_cast<long long>(reward));
        player.addMoney(reward);
        if (cashSound.has_value()) cashSound->play();
    } else if (result < 75) {
        wheelResultText = "PROFIT BOOST 1H (x2)";
        player.activateTempBoost(3600.0, 2.0);
        if (achievementSound.has_value()) achievementSound->play();
    } else {
        wheelResultText = "GHINION (Nothing)";
        if (errorSound.has_value()) errorSound->play();
    }
    saveQuietly();
}

void Application::updateWheel(float dt) {
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    if (wheelIsSpinning) {
        wheelSpinTimer -= dt;
        if (wheelSpinTimer > 0.0f) {
            static const std::vector<std::string> previewLabels = {
                "Jackpot 5 Gold", "Mini 2 Gold", "Refund 1 Gold", "Cash (Small)",
                "Cash (Big)", "Profit Boost 1h", "GHINION"
            };
            std::uniform_int_distribution<size_t> preview(0, previewLabels.size() - 1);
            wheelResultText = previewLabels[preview(randomEngine)];
        } else {
            wheelIsSpinning = false;
            resolveWheelSpin();
        }
        return;
    }

    for (auto& btn : wheelButtons) {
        if (!isButtonClicked(btn, mousePos)) continue;

        if (btn.type == Button::BACK) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::GAME;
            stateTransitionTimer = 0.2f;
        } else if (btn.type == Button::SPIN_WHEEL) {
            try {
                game->getPlayer().spendGold(1);
                wheelIsSpinning = true;
                wheelSpinTimer = 2.0f;
                if (clickSound.has_value()) clickSound->play();
            } catch (const GameException& error) {
                reportError(error);
            }
        }
    }
}

void Application::updateGame(float dt) {
    const double oldMoney = game->getPlayer().getMoney();

    updateGameNotifications(dt);
    updateGameInput();
    updateGameButtonsState();

    const double newMoney = game->getPlayer().getMoney();
    if (newMoney > oldMoney) {
        spawnFloatingText("+$" + std::to_string(static_cast<long long>(newMoney - oldMoney)), 850, 80, COLOR_GREEN);
    }
}

void Application::updateFloatingTexts(float dt) {
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->lifeTime -= dt;
        it->position.y -= 50.0f * dt;
        if (it->lifeTime <= 0.0f) {
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
    for (const auto& message : game->update(dt)) {
        pushNotification(message);
        if (message.rfind("ACHIEVEMENT:", 0) == 0 && achievementSound.has_value()) {
            achievementSound->play();
        }
    }
}

void Application::updateGameInput() {
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    for (auto& btn : gameButtons) {
        if (isButtonClicked(btn, mousePos)) {
            handleButtonClick(btn);
        }
    }
}

void Application::handleButtonClick(const Button& btn) {
    try {
        Player& player = game->getPlayer();
        const float labelY = btn.businessIndex >= 0
                                 ? businessRowY(static_cast<size_t>(btn.businessIndex)) - 10.f
                                 : 300.f;

        if (btn.type == Button::SAVE_EXIT) {
            if (clickSound.has_value()) clickSound->play();
            saveQuietly();
            window.close();
        } else if (btn.type == Button::RESET) {
            if (clickSound.has_value()) clickSound->play();
            if (Game::saveFileExists()) {
                std::filesystem::remove("savegame.txt");
            }
            game = std::make_unique<Game>("Capitalist", 0.0);
            initGameUI();
            initGoldShopUI();
            pushNotification("GAME RESET!");
        } else if (btn.type == Button::MAIN_MENU) {
            if (clickSound.has_value()) clickSound->play();
            saveQuietly();
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
        } else if (btn.type == Button::SHOW_SEASON) {
            if (clickSound.has_value()) clickSound->play();
            currentState = AppState::SEASON_INFO;
            stateTransitionTimer = 0.2f;
        } else if (btn.type == Button::PRESTIGE) {
            if (!player.canPrestige()) {
                throw BusinessNotOwnedException("ultima afacere la nivel 25 si 7.000.000$");
            }
            const int gold = player.prestige();
            saveQuietly();
            initGameUI();
            spawnFloatingText("PRESTIGE! +" + std::to_string(gold) + " Gold", 400, 300, COLOR_ACCENT);
            if (achievementSound.has_value()) achievementSound->play();
        } else if (btn.type == Button::START) {
            if (clickSound.has_value()) clickSound->play();
            player.startBusinessProduction(btn.businessIndex);
            spawnFloatingText("Working...", 100, labelY, COLOR_WHITE);
        } else if (btn.type == Button::UPGRADE) {
            const auto& businesses = player.getBusinesses();
            if (businesses.at(static_cast<size_t>(btn.businessIndex))->isOwned()) {
                player.upgradeBusiness(btn.businessIndex);
                spawnFloatingText("Upgraded!", 750, labelY, COLOR_GREEN);
            } else {
                player.purchaseBusiness(btn.businessIndex);
                spawnFloatingText("Purchased!", 750, labelY, COLOR_GREEN);
            }
            if (cashSound.has_value()) cashSound->play();
        } else if (btn.type == Button::MANAGER) {
            const auto& businesses = player.getBusinesses();
            if (businesses.at(static_cast<size_t>(btn.businessIndex))->hasManagerHired()) {
                player.upgradeManager(btn.businessIndex);
                spawnFloatingText("Manager Upgraded!", 680, labelY, COLOR_BLUE);
            } else {
                player.hireManager(btn.businessIndex);
                spawnFloatingText("Manager Hired!", 680, labelY, COLOR_BLUE);
            }
            if (cashSound.has_value()) cashSound->play();
        }
    } catch (const std::exception& error) {
        reportError(error);
    }
}

void Application::updateGameButtonsState() {
    const Player& player = game->getPlayer();
    const auto& businesses = player.getBusinesses();
    const double playerMoney = player.getMoney();

    for (auto& btn : gameButtons) {
        if (btn.type == Button::PRESTIGE) {
            const bool ready = player.canPrestige();
            btn.color = ready ? COLOR_ACCENT : COLOR_GRAY;
            btn.text = ready ? "PRESTIGE\n(Ready!)" : "PRESTIGE\n(Locked)";
        }

        if (btn.businessIndex < 0 || static_cast<size_t>(btn.businessIndex) >= businesses.size()) continue;
        const auto& business = businesses[static_cast<size_t>(btn.businessIndex)];

        if (btn.type == Button::UPGRADE) {
            if (business->isOwned()) {
                const double cost = business->getUpgradeCost();
                const bool milestone = business->nextUpgradeIsMilestone();
                btn.text = (milestone ? "MILESTONE\n$" : "BUY x1\n$") + std::to_string(static_cast<long long>(cost));
                btn.color = playerMoney >= cost ? COLOR_GREEN : (milestone ? COLOR_RED : COLOR_ACCENT);
            } else if (business->getPurchaseCost() == 0.0) {
                btn.text = "FREE!";
                btn.color = COLOR_GREEN;
            } else {
                const double cost = business->getPurchaseCost();
                btn.text = "UNLOCK\n$" + std::to_string(static_cast<long long>(cost));
                btn.color = playerMoney >= cost ? COLOR_GREEN : COLOR_RED;
            }
        } else if (btn.type == Button::MANAGER) {
            const bool hired = business->hasManagerHired();
            const double cost = hired ? business->getManagerUpgradeCost() : business->getManagerCost();
            btn.text = (hired ? "UPG\n$" : "MNG\n$") + std::to_string(static_cast<long long>(cost));
            btn.color = playerMoney >= cost ? COLOR_GREEN : COLOR_GRAY;
        } else if (btn.type == Button::START) {
            if (!business->isOwned()) {
                btn.color = COLOR_GRAY;
            } else if (business->hasManagerHired()) {
                btn.color = COLOR_GREEN;
            } else if (business->isActive()) {
                btn.color = COLOR_ICON_RUN;
            } else {
                btn.color = COLOR_ICON_IDLE;
            }
        }
    }
}

void Application::draw() {
    window.clear(COLOR_BG);

    switch (currentState) {
        case AppState::MENU:         drawMenu(); break;
        case AppState::ACHIEVEMENTS: drawAchievements(); break;
        case AppState::GOLD_SHOP:    drawGoldShop(); break;
        case AppState::LUCKY_WHEEL:  drawWheel(); break;
        case AppState::SEASON_INFO:  drawSeasonInfo(); break;
        case AppState::GAME:         drawGame(); break;
    }

    drawGameNotifications();
    drawFloatingTexts();
    window.display();
}

void Application::drawMenu() {
    drawText("Capital Empire", 350, 100, 50, COLOR_ACCENT);
    for (const auto& btn : menuButtons) {
        drawButton(btn);
    }
}

void Application::drawAchievements() {
    drawText("Achievements", 350, 30, 40, COLOR_WHITE);

    const auto& achievements = game->getPlayer().getAchievements();
    drawText("Deblocate: " + std::to_string(Achievement::countUnlocked(achievements)) + " / " +
             std::to_string(achievements.size()), 700, 45, 24, COLOR_ACCENT);

    float yPos = 100.f;
    for (const auto& ach : achievements) {
        const sf::Color textColor = ach.isUnlocked() ? COLOR_GREEN : COLOR_GRAY;
        const std::string status = ach.isUnlocked() ? "[UNLOCKED] " : "[LOCKED] ";
        drawText(status + ach.getName() + ": " + ach.getDescription(), 40, yPos, 22, textColor);
        yPos += 38.f;
    }

    for (const auto& btn : achievementButtons) {
        drawButton(btn);
    }
}

void Application::drawSeasonInfo() {
    drawText("Informatii Sezon", 330, 30, 40, COLOR_GREEN);
    drawMultilineText(game->getSeasonInfo(), 60, 130, 28, COLOR_WHITE);
    drawText("Bonusuri active:", 60, 500, 28, COLOR_ACCENT);
    drawText(game->getPlayer().getBonusSummary(), 60, 545, 24, COLOR_TEXT);
    drawText("Realizari deblocate in aceasta sesiune: " + std::to_string(Achievement::getUnlockEvents()),
             60, 590, 24, COLOR_TEXT);
    drawText("Prestigii: " + std::to_string(game->getPlayer().getPrestigeCount()), 60, 630, 24, COLOR_TEXT);

    for (const auto& btn : seasonButtons) {
        drawButton(btn);
    }
}

void Application::drawGoldShop() {
    drawText("Gold Shop", 400, 30, 40, COLOR_ACCENT);
    drawText("Gold: " + std::to_string(game->getPlayer().getGold()), 800, 40, 30, COLOR_ACCENT);
    drawText(game->getPlayer().getBonusSummary(), 50, 90, 20, COLOR_TEXT);

    for (const auto& btn : goldShopButtons) {
        drawButton(btn);
    }
}

void Application::drawWheel() {
    drawText("Lucky Wheel", 380, 50, 40, COLOR_RED);
    drawText("Gold: " + std::to_string(game->getPlayer().getGold()), 800, 50, 30, COLOR_ACCENT);

    const int width = getTextWidth(wheelResultText, 40);
    drawText(wheelResultText, 500.f - static_cast<float>(width) / 2.f, 300, 40, COLOR_WHITE);

    for (const auto& btn : wheelButtons) {
        drawButton(btn);
    }
}

void Application::drawGame() {
    drawGameHeader();
    drawGameBusinesses();
    drawGameButtons();
}

void Application::drawFloatingTexts() {
    for (const auto& floating : floatingTexts) {
        sf::Color color = floating.color;
        color.a = static_cast<std::uint8_t>(255.f * (floating.lifeTime / 1.5f));
        drawText(floating.text, floating.position.x, floating.position.y, 24, color);
    }
}

void Application::drawGameHeader() {
    sf::RectangleShape header(sf::Vector2f(1000, 100));
    header.setFillColor(COLOR_BLACK);
    window.draw(header);

    drawText("Capital Empire", 300, 30, 40, COLOR_WHITE);

    const std::string moneyStr = "$" + std::to_string(static_cast<long long>(game->getPlayer().getMoney()));
    drawText(moneyStr, 980.f - static_cast<float>(getTextWidth(moneyStr, 50)), 25, 50, COLOR_ACCENT);

    drawText(game->getPlayer().getBonusSummary(), 25, 106, 18, COLOR_TEXT);
    drawText("Gold: " + std::to_string(game->getPlayer().getGold()), 20, 860, 30, COLOR_ACCENT);
}

void Application::drawGameBusinesses() {
    const auto& businesses = game->getPlayer().getBusinesses();

    const float panelH = businessPanelHeight();

    for (size_t i = 0; i < businesses.size(); ++i) {
        const float yPos = businessRowY(i);
        const auto& business = businesses[i];

        sf::RectangleShape panel(sf::Vector2f(960.f, panelH));
        panel.setPosition({20, yPos - 10.f});
        panel.setFillColor(COLOR_PANEL);
        panel.setOutlineColor(COLOR_BLACK);
        panel.setOutlineThickness(2);
        window.draw(panel);

        const float barX = 160.f;
        const float barH = 35.f;
        const float barY = yPos - 10.f + panelH - barH - 8.f;
        const float barW = 500.f;

        sf::RectangleShape bar(sf::Vector2f(barW, barH));
        bar.setPosition({barX, barY});
        bar.setFillColor(COLOR_BLACK);
        window.draw(bar);

        if (business->isOwned()) {
            const float progress = static_cast<float>(business->getProgress());
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

        std::string nameLvl = business->getName();
        if (business->isOwned()) {
            nameLvl += "  [Lv " + std::to_string(business->getLevel()) + "]";
        }
        drawText(nameLvl, barX, yPos + 2.f, 28, COLOR_WHITE);
        drawText(business->getStatusLabel(), barX + 250.f, yPos + 10.f, 18, COLOR_ACCENT);

        if (business->isOwned()) {
            drawText("$" + std::to_string(static_cast<long long>(business->getProfitPerCycle())),
                     barX + 10.f, barY + 5.f, 24, COLOR_WHITE);

            std::ostringstream timeText;
            timeText.precision(1);
            timeText << std::fixed << business->getProductionTime() << "s";
            drawText(timeText.str(), barX + barW - 80.f, barY + 5.f, 24, COLOR_WHITE);
        } else {
            drawText("LOCKED", barX + 200.f, barY + 5.f, 24, COLOR_RED);
        }
    }
}

void Application::drawGameButtons() {
    for (const auto& btn : gameButtons) {
        drawButton(btn, btn.type == Button::START);
    }
}

void Application::drawGameNotifications() {
    if (notifications.empty()) return;

    const auto& notif = notifications.front();
    const size_t breakPos = notif.text.find('\n');
    const std::string line1 = notif.text.substr(0, breakPos);
    const std::string line2 = breakPos == std::string::npos ? "" : notif.text.substr(breakPos + 1);

    const int widest = std::max(getTextWidth(line1, 30), getTextWidth(line2, 30));
    const float boxW = static_cast<float>(widest) + 60.f;
    const float boxH = 120.f;
    const float boxX = 500.f - boxW / 2.f;
    const float boxY = 375.f - boxH / 2.f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition({boxX, boxY});
    box.setFillColor(COLOR_BLACK);
    box.setOutlineColor(COLOR_ACCENT);
    box.setOutlineThickness(4);
    window.draw(box);

    drawText(line1, boxX + boxW / 2.f - static_cast<float>(getTextWidth(line1, 30)) / 2.f, boxY + 20.f, 30, COLOR_ACCENT);
    if (!line2.empty()) {
        drawText(line2, boxX + boxW / 2.f - static_cast<float>(getTextWidth(line2, 30)) / 2.f, boxY + 60.f, 30, COLOR_WHITE);
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
        sf::CircleShape circle(drawRect.size.x / 2.f);
        circle.setPosition({drawRect.position.x, drawRect.position.y});
        circle.setFillColor(btn.color);
        circle.setOutlineColor(COLOR_BLACK);
        circle.setOutlineThickness(2);
        window.draw(circle);

        const std::string iconText = (btn.color == COLOR_GREEN) ? "Auto" : "Click";
        drawText(iconText,
                 drawRect.position.x + drawRect.size.x / 2.f - static_cast<float>(getTextWidth(iconText, 20)) / 2.f,
                 drawRect.position.y + drawRect.size.y / 2.f - 10.f, 20, COLOR_BLACK);
        return;
    }

    sf::RectangleShape rect(sf::Vector2f(drawRect.size.x, drawRect.size.y));
    rect.setPosition({drawRect.position.x, drawRect.position.y});
    rect.setFillColor(btn.color);
    rect.setOutlineColor(COLOR_BLACK);
    rect.setOutlineThickness(2);
    window.draw(rect);

    const size_t breakPos = btn.text.find('\n');
    if (breakPos == std::string::npos) {
        drawText(btn.text,
                 drawRect.position.x + drawRect.size.x / 2.f - static_cast<float>(getTextWidth(btn.text, 20)) / 2.f,
                 drawRect.position.y + drawRect.size.y / 2.f - 10.f, 20, COLOR_WHITE);
        return;
    }

    const std::string line1 = btn.text.substr(0, breakPos);
    const std::string line2 = btn.text.substr(breakPos + 1);
    drawText(line1, drawRect.position.x + drawRect.size.x / 2.f - static_cast<float>(getTextWidth(line1, 20)) / 2.f,
             drawRect.position.y + 10.f, 20, COLOR_WHITE);
    drawText(line2, drawRect.position.x + drawRect.size.x / 2.f - static_cast<float>(getTextWidth(line2, 20)) / 2.f,
             drawRect.position.y + 35.f, 20, COLOR_WHITE);
}

void Application::drawText(const std::string& text, float x, float y, int size, sf::Color color) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(static_cast<unsigned int>(size));
    sfText.setFillColor(color);
    sfText.setPosition({x, y});
    window.draw(sfText);
}

void Application::drawMultilineText(const std::string& text, float x, float y, int size, sf::Color color) {
    std::istringstream stream(text);
    std::string line;
    float currentY = y;
    while (std::getline(stream, line)) {
        drawText(line, x, currentY, size, color);
        currentY += static_cast<float>(size) + 8.f;
    }
}

int Application::getTextWidth(const std::string& text, int size) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(static_cast<unsigned int>(size));
    return static_cast<int>(sfText.getLocalBounds().size.x);
}