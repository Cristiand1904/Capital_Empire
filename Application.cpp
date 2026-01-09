#include "Application.h"
#include <iostream>
#include <cmath>
#include <filesystem>
#include <cstdint> // Added for std::uint8_t

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
    // Increased window height to accommodate 6 businesses
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

    currentState = AppState::MENU;
    stateTransitionTimer = 0.0f;

    game = std::make_unique<Game>("Capitalist", 0.0);
    initUI();
}

void Application::initUI() {
    initMenuUI();
    initAchievementUI();
}

void Application::initMenuUI() {
    menuButtons.clear();

    Button newGameBtn;
    newGameBtn.rect = sf::FloatRect({350, 250}, {300, 80});
    newGameBtn.text = "NEW GAME";
    newGameBtn.color = COLOR_GREEN;
    newGameBtn.type = Button::NEW_GAME;
    newGameBtn.businessIndex = -1; // Initialized
    newGameBtn.isPressed = false;
    menuButtons.push_back(newGameBtn);

    if (Game::saveFileExists()) {
        Button loadGameBtn;
        loadGameBtn.rect = sf::FloatRect({350, 350}, {300, 80});
        loadGameBtn.text = "LOAD GAME";
        loadGameBtn.color = COLOR_BLUE;
        loadGameBtn.type = Button::LOAD_GAME;
        loadGameBtn.businessIndex = -1; // Initialized
        loadGameBtn.isPressed = false;
        menuButtons.push_back(loadGameBtn);
    }

    Button achBtn;
    achBtn.rect = sf::FloatRect({350, 450}, {300, 80});
    achBtn.text = "ACHIEVEMENTS";
    achBtn.color = COLOR_ACCENT;
    achBtn.type = Button::SHOW_ACHIEVEMENTS;
    achBtn.businessIndex = -1; // Initialized
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
    backBtn.businessIndex = -1; // Initialized
    backBtn.isPressed = false;
    achievementButtons.push_back(backBtn);
}

void Application::initGameUI() {
    gameButtons.clear();
    for(int i=0; i<6; ++i) {
        createBusinessUI(i, 120 + i * 120);
    }

    Button saveBtn;
    saveBtn.rect = sf::FloatRect({820, 20}, {160, 50});
    saveBtn.text = "SAVE & EXIT";
    saveBtn.color = COLOR_RED;
    saveBtn.type = Button::SAVE_EXIT;
    saveBtn.businessIndex = -1;
    saveBtn.isPressed = false;
    gameButtons.push_back(saveBtn);

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
}

void Application::createBusinessUI(int index, float yPos) {
    Button startBtn;
    startBtn.rect = sf::FloatRect({50, yPos}, {90, 90});
    startBtn.text = "GO!";
    startBtn.color = COLOR_ACCENT;
    startBtn.type = Button::START;
    startBtn.businessIndex = index;
    startBtn.isPressed = false;
    gameButtons.push_back(startBtn);

    Button buyBtn;
    buyBtn.rect = sf::FloatRect({760, yPos + 15}, {200, 60}); // Moved to 760
    buyBtn.text = "BUY";
    buyBtn.color = COLOR_ACCENT;
    buyBtn.type = Button::UPGRADE;
    buyBtn.businessIndex = index;
    buyBtn.isPressed = false;
    gameButtons.push_back(buyBtn);

    Button mngBtn;
    mngBtn.rect = sf::FloatRect({670, yPos + 15}, {80, 60}); // Widened to 80, moved to 670
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
    } else {
        updateGame(dt);
    }
}

void Application::updateMenu(float dt) {
    (void)dt;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    for (auto& btn : menuButtons) {
        if (isButtonClicked(btn, mousePos)) {
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
            if (btn.type == Button::BACK) {
                currentState = AppState::MENU;
                stateTransitionTimer = 0.2f;
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
            game->saveGame();
            window.close();
        } else if (btn.type == Button::RESET) {
            if (Game::saveFileExists()) {
                std::filesystem::remove("savegame.txt");
            }
            game = std::make_unique<Game>("Capitalist", 0.0);
            notifications.push_back({"GAME RESET!", 2.0f});
        } else if (btn.type == Button::MAIN_MENU) {
            game->saveGame();
            currentState = AppState::MENU;
            stateTransitionTimer = 0.2f;
            initMenuUI();
        } else if (btn.type == Button::START) {
            game->getPlayer().startBusinessProduction(btn.businessIndex);
            spawnFloatingText("Working...", 100, 140 + btn.businessIndex * 120, COLOR_WHITE);
        } else if (btn.type == Button::UPGRADE) {
            const auto& businesses = game->getPlayer().getBusinesses();
            if (btn.businessIndex >= 0 && static_cast<size_t>(btn.businessIndex) < businesses.size()) {
                if (businesses[btn.businessIndex]->isOwned()) {
                    game->getPlayer().upgradeBusiness(btn.businessIndex);
                    spawnFloatingText("Upgraded!", 750, 140 + btn.businessIndex * 120, COLOR_GREEN);
                } else {
                    game->getPlayer().purchaseBusiness(btn.businessIndex);
                    spawnFloatingText("Purchased!", 750, 140 + btn.businessIndex * 120, COLOR_GREEN);
                }
            }
        } else if (btn.type == Button::MANAGER) {
            const auto& businesses = game->getPlayer().getBusinesses();
            if (businesses[btn.businessIndex]->hasManagerHired()) {
                game->getPlayer().upgradeManager(btn.businessIndex);
                spawnFloatingText("Manager Upgraded!", 680, 140 + btn.businessIndex * 120, COLOR_BLUE);
            } else {
                game->getPlayer().hireManager(btn.businessIndex);
                spawnFloatingText("Manager Hired!", 680, 140 + btn.businessIndex * 120, COLOR_BLUE);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Eroare: " << e.what() << std::endl;
        spawnFloatingText("Error!", 500, 375, COLOR_RED);
    }
}

void Application::updateGameButtonsState() {
    const auto& businesses = game->getPlayer().getBusinesses();
    double playerMoney = game->getPlayer().getMoney();

    for (auto& btn : gameButtons) {
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
    } else {
        drawGame();
    }

    window.display();
}

void Application::drawMenu() {
    drawText("Capital Empire", 350, 150, 50, COLOR_ACCENT);

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
    drawText(moneyStr, 780 - moneyWidth, 25, 50, COLOR_ACCENT);
}

void Application::drawGameBusinesses() {
    const auto& businesses = game->getPlayer().getBusinesses();

    for (size_t i = 0; i < businesses.size(); ++i) {
        float yPos = 120 + i * 120;
        const auto& b = businesses[i];

        sf::RectangleShape panel(sf::Vector2f(960, 110));
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