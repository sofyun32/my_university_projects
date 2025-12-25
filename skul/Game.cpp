#include "Game.h"
#include <iostream>
#include <algorithm>

// Функция для проверки пересечения прямоугольников
bool checkCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
{
    return rect1.intersects(rect2);
}

Game::Game() :
    window(sf::VideoMode(800, 600), "Skul-like Game"),
    gameState(GameState::MAIN_MENU),
    ground(sf::Vector2f(800.f, 50.f)),
    chest(sf::Vector2f(80.f, 60.f)),
    playerHealthBar(sf::Vector2f(200.f, 20.f)),
    enemyHealthBar(sf::Vector2f(200.f, 20.f)),
    bossHealthBar(sf::Vector2f(300.f, 25.f)),
    playerHealthBackground(sf::Vector2f(200.f, 20.f)),
    enemyHealthBackground(sf::Vector2f(200.f, 20.f)),
    bossHealthBackground(sf::Vector2f(300.f, 25.f)),
    fontLoaded(false),
    totalEnemiesKilled(0),
    enemiesKilledThisGame(0),
    currentLevel(1),
    enemiesToNextLevel(2),
    skullsCollected(0),
    allLevelsCompleted(false),
    chestTimer(0.f),
    chestTimerMax(5.f),
    canContinueFromChest(false),
    lastSkull(SkullType::NONE),
    bossIsActive(false)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    // Настройка игровых объектов
    ground.setPosition(0.f, 550.f);
    ground.setFillColor(sf::Color::Green);

    chest.setPosition(-200.f, -200.f);
    chest.setFillColor(sf::Color(139, 69, 19));
    chest.setOutlineThickness(3.f);
    chest.setOutlineColor(sf::Color(101, 67, 33));

    // Настройка UI
    setupUI();

    // Инициализация черепов
    availableSkulls = { SkullType::WARRIOR, SkullType::HUNTER, SkullType::GIANT };
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = gameClock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();

            // Обработка смены черепов по клавише E
            if (event.key.code == sf::Keyboard::E) {
                if ((gameState == GameState::PLAYING || gameState == GameState::BOSS_FIGHT) &&
                    player.canSwitchSkulls()) {
                    player.switchToNextSkull();
                }
            }

            switch (gameState) {
            case GameState::MAIN_MENU:
                if (event.key.code == sf::Keyboard::LShift) {
                    gameState = GameState::PLAYING;
                    availableSkulls = { SkullType::WARRIOR, SkullType::HUNTER, SkullType::GIANT };
                    lastSkull = SkullType::NONE;
                    bossIsActive = false;
                }
                break;
            case GameState::CHEST:
                if (event.key.code == sf::Keyboard::Space && canContinueFromChest) {
                    if (allLevelsCompleted) {
                        gameState = GameState::BOSS_FIGHT;
                        bossIsActive = true;
                        boss.setPosition(sf::Vector2f(350.f, 470.f));
                    }
                    else {
                        gameState = GameState::PLAYING;
                    }
                    chest.setPosition(-200.f, -200.f);
                    canContinueFromChest = false;
                    chestTimer = 0.f;
                }
                break;
            case GameState::GAME_OVER:
                if (event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
                break;
            case GameState::VICTORY:
                if (event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
                break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    switch (gameState) {
    case GameState::MAIN_MENU:
        updateMainMenu(deltaTime);
        break;
    case GameState::PLAYING:
        updatePlaying(deltaTime);
        break;
    case GameState::CHEST:
        updateChest(deltaTime);
        break;
    case GameState::GAME_OVER:
        updateGameOver(deltaTime);
        break;
    case GameState::VICTORY:
        updateVictory(deltaTime);
        break;
    case GameState::BOSS_FIGHT:
        updateBossFight(deltaTime);
        break;
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    switch (gameState) {
    case GameState::MAIN_MENU:
        renderMainMenu();
        break;
    case GameState::PLAYING:
        renderPlaying();
        break;
    case GameState::CHEST:
        renderChest();
        break;
    case GameState::GAME_OVER:
        renderGameOver();
        break;
    case GameState::VICTORY:
        renderVictory();
        break;
    case GameState::BOSS_FIGHT:
        renderBossFight();
        break;
    }

    window.display();
}

void Game::updateMainMenu(float deltaTime) {
    // Анимация или другие обновления для главного меню
}

void Game::updatePlaying(float deltaTime) {
    // Обработка ввода
    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        movement.x -= player.getSpeed() * deltaTime;
        player.setFacingRight(false);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        movement.x += player.getSpeed() * deltaTime;
        player.setFacingRight(true);
    }

    // Проверка границ
    sf::Vector2f newPosition = player.getPosition() + movement;
    if (newPosition.x >= 0 && newPosition.x <= 800 - 50) {
        player.move(movement);
    }

    // Прыжок
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        player.jump();
    }

    // Атака
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        player.startAttack();
    }

    // Способности
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        switch (player.getSkull()) {
        case SkullType::WARRIOR:
            player.activateShield();
            break;
        case SkullType::HUNTER:
            if (player.isArrowCooldownReady()) {
                player.shootArrow();
            }
            break;
        case SkullType::GIANT:
            player.activateEarthquake();
            break;
        default:
            break;
        }
    }

    // Обновление игрока
    player.update(deltaTime);
    player.applyGravity(deltaTime, 550.f);

    // Обновление врага
    enemy.update(deltaTime);
    enemy.updateGravity(deltaTime, 550.f);

    // Проверка столкновений и комбат
    handlePlayerCombat();

    // Проверка смерти врага
    if (!enemy.isAlive()) {
        enemiesKilledThisGame++;  // Увеличиваем счетчик убитых врагов
        totalEnemiesKilled++;
        enemy.reset();

        // Проверяем, достигли ли мы необходимого количества убийств для перехода на уровень
        if (enemiesKilledThisGame >= enemiesToNextLevel) {
            levelUp();
        }
    }

    // Проверка смерти игрока
    if (player.getHealth() <= 0) {
        gameState = GameState::GAME_OVER;
    }

    updateUI();
}

void Game::updateChest(float deltaTime) {
    if (!canContinueFromChest) {
        chestTimer += deltaTime;
        if (chestTimer >= chestTimerMax) {
            canContinueFromChest = true;
            chestInstructionText.setString("Press SPACE to continue");
            chestInstructionText.setOrigin(chestInstructionText.getLocalBounds().width / 2, 0);
        }
    }
}

void Game::updateGameOver(float deltaTime) {
    // Ничего не обновляем в Game Over
}

void Game::updateVictory(float deltaTime) {
    // Ничего не обновляем в Victory
}

void Game::updateBossFight(float deltaTime) {
    // Обработка ввода (аналогично PLAYING)
    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        movement.x -= player.getSpeed() * deltaTime;
        player.setFacingRight(false);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        movement.x += player.getSpeed() * deltaTime;
        player.setFacingRight(true);
    }

    // Проверка границ
    sf::Vector2f newPosition = player.getPosition() + movement;
    if (newPosition.x >= 0 && newPosition.x <= 800 - 50) {
        player.move(movement);
    }

    // Прыжок
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        player.jump();
    }

    // Атака
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        player.startAttack();
    }

    // Способности
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        switch (player.getSkull()) {
        case SkullType::WARRIOR:
            player.activateShield();
            break;
        case SkullType::HUNTER:
            if (player.isArrowCooldownReady()) {
                player.shootArrow();
            }
            break;
        case SkullType::GIANT:
            player.activateEarthquake();
            break;
        default:
            break;
        }
    }

    // Обновление игрока
    player.update(deltaTime);
    player.applyGravity(deltaTime, 550.f);

    // Обновление босса
    boss.update(deltaTime);
    boss.updateGravity(deltaTime, 550.f);

    // Проверка столкновений и комбат
    handleBossCombat();

    // Проверка смерти босса
    if (!boss.isAlive()) {
        gameState = GameState::VICTORY;
        bossIsActive = false;
        boss.reset();
    }

    // Проверка смерти игрока
    if (player.getHealth() <= 0) {
        gameState = GameState::GAME_OVER;
    }

    updateUI();
}

void Game::renderMainMenu() {
    if (fontLoaded) {
        window.draw(titleText);
        window.draw(subtitleText);
        window.draw(startText);
    }
}

void Game::renderPlaying() {
    window.draw(ground);
    player.draw(window);
    enemy.draw(window);

    // UI
    window.draw(playerHealthBackground);
    window.draw(enemyHealthBackground);
    window.draw(playerHealthBar);
    window.draw(enemyHealthBar);

    if (fontLoaded) {
        window.draw(killsText);
        window.draw(levelText);
        window.draw(skullsText);
        window.draw(currentSkullText);
        window.draw(switchSkullHintText);
    }
}

void Game::renderChest() {
    window.clear(sf::Color(30, 20, 10));
    chest.setPosition(360.f, 300.f);
    window.draw(chest);

    sf::RectangleShape chestBackground(sf::Vector2f(600.f, 300.f));
    chestBackground.setPosition(100.f, 150.f);
    chestBackground.setFillColor(sf::Color(60, 40, 20, 200));
    window.draw(chestBackground);

    if (fontLoaded) {
        window.draw(chestTitleText);
        window.draw(skullNameText);
        window.draw(skullDescriptionText);
        window.draw(skullAbilityText);
        window.draw(chestInstructionText);

        if (!canContinueFromChest) {
            int remainingTime = static_cast<int>(chestTimerMax - chestTimer) + 1;
            chestTimerText.setString("Wait: " + std::to_string(remainingTime) + "s");
            chestTimerText.setOrigin(chestTimerText.getLocalBounds().width / 2, 0);
            window.draw(chestTimerText);
        }
    }
}

void Game::renderGameOver() {
    window.draw(ground);
    player.draw(window);
    if (bossIsActive) {
        boss.draw(window);
    }
    else {
        enemy.draw(window);
    }

    window.draw(playerHealthBackground);
    window.draw(enemyHealthBackground);
    window.draw(playerHealthBar);
    window.draw(enemyHealthBar);

    if (fontLoaded) {
        window.draw(killsText);
        window.draw(levelText);
        window.draw(skullsText);
        window.draw(currentSkullText);
    }

    sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    if (fontLoaded) {
        window.draw(gameOverText);
        window.draw(statsText);
        window.draw(restartText);
    }
}

void Game::renderVictory() {
    window.clear(sf::Color(10, 50, 10));

    sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    if (fontLoaded) {
        window.draw(victoryText);
        window.draw(victoryStatsText);

        sf::Text victoryRestartText;
        victoryRestartText.setFont(font);
        victoryRestartText.setCharacterSize(20);
        victoryRestartText.setFillColor(sf::Color::White);
        victoryRestartText.setPosition(220.f, 350.f);
        victoryRestartText.setString("Press R to play again or ESC to quit");
        window.draw(victoryRestartText);
    }
}

void Game::renderBossFight() {
    window.draw(ground);
    player.draw(window);
    boss.draw(window);

    // UI
    window.draw(playerHealthBackground);
    window.draw(bossHealthBackground);
    window.draw(playerHealthBar);
    window.draw(bossHealthBar);

    if (fontLoaded) {
        window.draw(killsText);
        window.draw(levelText);
        window.draw(skullsText);
        window.draw(currentSkullText);
        window.draw(bossText);
        window.draw(switchSkullHintText);
    }
}

void Game::handlePlayerCombat() {
    // Столкновение игрока с врагом
    if (checkCollision(player.getGlobalBounds(), enemy.getGlobalBounds()) &&
        player.canTakeDamage() && !player.isShieldActive()) {
        player.takeDamage(5);

        // Отталкивание
        float pushBack = 50.f;
        sf::Vector2f newPlayerPos = player.getPosition();
        if (player.getPosition().x < enemy.getPosition().x)
            newPlayerPos.x -= pushBack;
        else
            newPlayerPos.x += pushBack;

        if (newPlayerPos.x >= 0 && newPlayerPos.x <= 800 - 50)
            player.setPosition(newPlayerPos);
    }

    // Атака игрока
    static bool enemyDamagedThisAttack = false;
    if (player.isAttacking() && !enemyDamagedThisAttack) {
        if (checkCollision(player.getAttackBounds(), enemy.getGlobalBounds())) {
            int damage = player.getAttackDamage(!player.isOnGround());
            enemy.takeDamage(damage);
            enemyDamagedThisAttack = true;

            // Отталкивание врага
            enemy.applyKnockback(player.isFacingRight(), 30.f);
        }
    }
    else if (!player.isAttacking()) {
        enemyDamagedThisAttack = false;
    }

    // Стрела игрока
    if (player.isArrowActive() && checkCollision(player.getArrowBounds(), enemy.getGlobalBounds())) {
        enemy.takeDamage(15); 
        player.deactivateArrow();
    }

    // Землетрясение
    static bool earthquakeDamagedEnemy = false;
    if (player.isEarthquakeActive()) {
        if (!earthquakeDamagedEnemy) {
            if (checkCollision(player.getEarthquakeBounds(), enemy.getGlobalBounds())) {
                enemy.takeDamage(25);
                enemy.applyVerticalVelocity(-400.f);
                earthquakeDamagedEnemy = true;
            }
        }
    }
    else {
        earthquakeDamagedEnemy = false;
    }

    // Столкновение щита с врагом
    if (player.isShieldActive() && checkCollision(player.getShieldBounds(), enemy.getGlobalBounds())) {
        // Отталкивание врага
        sf::Vector2f newEnemyPos;
        if (player.getPosition().x < enemy.getPosition().x) {
            newEnemyPos = enemy.getPosition() + sf::Vector2f(40.f, 0.f);
        }
        else {
            newEnemyPos = enemy.getPosition() + sf::Vector2f(-40.f, 0.f);
        }

        if (newEnemyPos.x >= 0 && newEnemyPos.x <= 800 - enemy.getGlobalBounds().width) {
            enemy.setPosition(newEnemyPos);
        }
    }
}

void Game::handleBossCombat() {
    // Столкновение игрока с боссом
    if (checkCollision(player.getGlobalBounds(), boss.getGlobalBounds()) &&
        player.canTakeDamage() && !player.isShieldActive()) {
        player.takeDamage(15); // Босс наносит больше урона

        // Отталкивание
        float pushBack = 60.f;
        sf::Vector2f newPlayerPos = player.getPosition();
        if (player.getPosition().x < boss.getPosition().x)
            newPlayerPos.x -= pushBack;
        else
            newPlayerPos.x += pushBack;

        if (newPlayerPos.x >= 0 && newPlayerPos.x <= 800 - 50)
            player.setPosition(newPlayerPos);
    }

    // Атака игрока по боссу
    static bool bossDamagedThisAttack = false;
    if (player.isAttacking() && !bossDamagedThisAttack) {
        if (checkCollision(player.getAttackBounds(), boss.getGlobalBounds())) {
            int damage = player.getAttackDamage(!player.isOnGround());
            boss.takeDamage(damage);
            bossDamagedThisAttack = true;

            // Отталкивание босса
            boss.applyKnockback(player.isFacingRight(), 20.f);
        }
    }
    else if (!player.isAttacking()) {
        bossDamagedThisAttack = false;
    }

    // Стрела игрока по боссу
    if (player.isArrowActive() && checkCollision(player.getArrowBounds(), boss.getGlobalBounds())) {
        boss.takeDamage(15);
        player.deactivateArrow();
    }

    // Землетрясение по боссу
    static bool earthquakeDamagedBoss = false;
    if (player.isEarthquakeActive()) {
        if (!earthquakeDamagedBoss) {
            if (checkCollision(player.getEarthquakeBounds(), boss.getGlobalBounds())) {
                boss.takeDamage(25);
                boss.applyVerticalVelocity(-400.f);
                earthquakeDamagedBoss = true;
            }
        }
    }
    else {
        earthquakeDamagedBoss = false;
    }

    // Стрелы босса
    const auto& bossArrows = boss.getArrows();
    for (size_t i = 0; i < bossArrows.size(); ++i) {
        if (checkCollision(bossArrows[i].getGlobalBounds(), player.getGlobalBounds()) &&
            player.canTakeDamage() && !player.isShieldActive()) {
            player.takeDamage(15);
            boss.removeArrow(i);
            break;
        }
    }
}

void Game::levelUp() {

    // Сбрасываем атаки игрока перед переходом на новый уровень
    player.resetAbilities();

    enemy.reset();

    // Логика выбора нового черепа
    SkullType newSkull = SkullType::NONE; // Инициализируем переменную
    if (availableSkulls.empty()) {
        availableSkulls = { SkullType::WARRIOR, SkullType::HUNTER, SkullType::GIANT };
    }

    std::vector<SkullType> possibleSkulls = availableSkulls;
    if (lastSkull != SkullType::NONE) {
        possibleSkulls.erase(std::remove(possibleSkulls.begin(), possibleSkulls.end(), lastSkull), possibleSkulls.end());
    }
    if (possibleSkulls.empty()) {
        possibleSkulls = availableSkulls;
    }

    newSkull = possibleSkulls[rand() % possibleSkulls.size()];
    availableSkulls.erase(std::remove(availableSkulls.begin(), availableSkulls.end(), newSkull), availableSkulls.end());
    lastSkull = newSkull;

    // Временно сохраняем информацию о новом черепе
    std::string newSkullName = "";
    std::string skullDescription = "";
    std::string skullAbilityDescription = "";

    switch (newSkull) {
    case SkullType::WARRIOR:
        newSkullName = "Warrior Skull";
        skullDescription = "Powerful melee attacks";
        skullAbilityDescription = "Shield Wall: Block all damage and push enemies back";
        break;
    case SkullType::HUNTER:
        newSkullName = "Hunter Skull";
        skullDescription = "Ranged arrow attacks";
        skullAbilityDescription = "Arrow: Shoot projectile with cooldown";
        break;
    case SkullType::GIANT:
        newSkullName = "Giant Skull";
        skullDescription = "Massive area damage";
        skullAbilityDescription = "Earthquake: AoE ground slam that knocks up enemies";
        break;
    default:
        newSkullName = "Unknown Skull";
        break;
    }

    // Устанавливаем текст ДО добавления черепа игроку
    if (fontLoaded) {
        skullNameText.setString(newSkullName);
        skullNameText.setOrigin(skullNameText.getLocalBounds().width / 2, 0);
        skullDescriptionText.setString("Basic Attack: " + skullDescription);
        skullDescriptionText.setOrigin(skullDescriptionText.getLocalBounds().width / 2, 0);
        skullAbilityText.setString("Special Ability: " + skullAbilityDescription);
        skullAbilityText.setOrigin(skullAbilityText.getLocalBounds().width / 2, 0);
    }

    // Только ПОСЛЕ установки текста добавляем череп игроку
    player.addSkullToCollection(newSkull);
    skullsCollected++;

    enemiesKilledThisGame = 0;
    currentLevel++;

    if (currentLevel > 3) {
        allLevelsCompleted = true;
    }
    else {
        if (currentLevel == 2)
            enemiesToNextLevel = 3;
        else if (currentLevel == 3)
            enemiesToNextLevel = 5;
    }

    gameState = GameState::CHEST;
    chestTimer = 0.f;
    canContinueFromChest = false;
    chestInstructionText.setString("Wait for timer...");
    chestInstructionText.setOrigin(chestInstructionText.getLocalBounds().width / 2, 0);
}

void Game::resetGame() {
    gameState = GameState::PLAYING;
    player.reset();
    enemy.reset();
    boss.reset();
    bossIsActive = false;

    totalEnemiesKilled = 0;
    enemiesKilledThisGame = 0;
    currentLevel = 1;
    enemiesToNextLevel = 2;  // Для первого уровня нужно 2 убийства
    skullsCollected = 0;
    allLevelsCompleted = false;

    availableSkulls = { SkullType::WARRIOR, SkullType::HUNTER, SkullType::GIANT };
    lastSkull = SkullType::NONE;
}

void Game::setupUI() {
    // Загрузка шрифта
    if (font.loadFromFile("arial.ttf")) {
        fontLoaded = true;
        std::cout << "Font loaded successfully from arial.ttf" << std::endl;
    }
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
        std::cout << "Font loaded successfully from system fonts" << std::endl;
    }
    else {
        std::cout << "WARNING: Cannot load font file 'arial.ttf'!" << std::endl;
        std::cout << "Game will continue without text rendering." << std::endl;
    }

    // Настройка полос здоровья
    playerHealthBar.setPosition(10.f, 10.f);
    playerHealthBar.setFillColor(sf::Color::Green);

    enemyHealthBar.setPosition(590.f, 10.f);
    enemyHealthBar.setFillColor(sf::Color::Red);

    bossHealthBar.setPosition(250.f, 10.f);
    bossHealthBar.setFillColor(sf::Color::Magenta);

    playerHealthBackground.setPosition(10.f, 10.f);
    playerHealthBackground.setFillColor(sf::Color(50, 50, 50));

    enemyHealthBackground.setPosition(590.f, 10.f);
    enemyHealthBackground.setFillColor(sf::Color(50, 50, 50));

    bossHealthBackground.setPosition(250.f, 10.f);
    bossHealthBackground.setFillColor(sf::Color(50, 50, 50));

    if (fontLoaded) {
        // Главное меню
        titleText.setFont(font);
        titleText.setString("Skul");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(300.f, 150.f);

        subtitleText.setFont(font);
        subtitleText.setString("The Hero Slayer");
        subtitleText.setCharacterSize(30);
        subtitleText.setFillColor(sf::Color(200, 200, 200));
        subtitleText.setPosition(270.f, 250.f);

        startText.setFont(font);
        startText.setString("Press SHIFT to start");
        startText.setCharacterSize(40);
        startText.setFillColor(sf::Color::Yellow);
        startText.setPosition(200.f, 350.f);

        // Игровой UI
        killsText.setFont(font);
        killsText.setCharacterSize(16);
        killsText.setFillColor(sf::Color::White);
        killsText.setPosition(15.f, 35.f);

        levelText.setFont(font);
        levelText.setCharacterSize(16);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(15.f, 55.f);

        skullsText.setFont(font);
        skullsText.setCharacterSize(16);
        skullsText.setFillColor(sf::Color::White);
        skullsText.setPosition(15.f, 75.f);

        currentSkullText.setFont(font);
        currentSkullText.setCharacterSize(16);
        currentSkullText.setFillColor(sf::Color::Cyan);
        currentSkullText.setPosition(15.f, 95.f);

        // Подсказка для смены черепов
        switchSkullHintText.setFont(font);
        switchSkullHintText.setCharacterSize(16);
        switchSkullHintText.setFillColor(sf::Color::Yellow);
        switchSkullHintText.setPosition(15.f, 115.f);

        bossText.setFont(font);
        bossText.setCharacterSize(18);
        bossText.setFillColor(sf::Color::Yellow);
        bossText.setPosition(250.f, 40.f);

        // Game Over
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(250.f, 200.f);
        gameOverText.setString("GAME OVER");

        statsText.setFont(font);
        statsText.setCharacterSize(20);
        statsText.setFillColor(sf::Color::White);
        statsText.setPosition(250.f, 260.f);

        restartText.setFont(font);
        restartText.setCharacterSize(20);
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition(220.f, 350.f);
        restartText.setString("Press R to restart or ESC to quit");

        // Сундук
        chestTitleText.setFont(font);
        chestTitleText.setCharacterSize(40);
        chestTitleText.setFillColor(sf::Color::Yellow);
        chestTitleText.setPosition(400.f, 180.f);
        chestTitleText.setString("CHEST UNLOCKED!");
        chestTitleText.setOrigin(chestTitleText.getLocalBounds().width / 2, 0);

        skullNameText.setFont(font);
        skullNameText.setCharacterSize(30);
        skullNameText.setFillColor(sf::Color::Cyan);
        skullNameText.setPosition(400.f, 230.f);
        skullNameText.setOrigin(skullNameText.getLocalBounds().width / 2, 0);

        skullDescriptionText.setFont(font);
        skullDescriptionText.setCharacterSize(20);
        skullDescriptionText.setFillColor(sf::Color::White);
        skullDescriptionText.setPosition(400.f, 280.f);
        skullDescriptionText.setOrigin(skullDescriptionText.getLocalBounds().width / 2, 0);

        skullAbilityText.setFont(font);
        skullAbilityText.setCharacterSize(20);
        skullAbilityText.setFillColor(sf::Color::Yellow);
        skullAbilityText.setPosition(400.f, 320.f);
        skullAbilityText.setOrigin(skullAbilityText.getLocalBounds().width / 2, 0);

        chestInstructionText.setFont(font);
        chestInstructionText.setCharacterSize(20);
        chestInstructionText.setFillColor(sf::Color::Yellow);
        chestInstructionText.setPosition(400.f, 380.f);
        chestInstructionText.setString("Wait for timer...");
        chestInstructionText.setOrigin(chestInstructionText.getLocalBounds().width / 2, 0);

        chestTimerText.setFont(font);
        chestTimerText.setCharacterSize(24);
        chestTimerText.setFillColor(sf::Color::Red);
        chestTimerText.setPosition(400.f, 350.f);
        chestTimerText.setOrigin(chestTimerText.getLocalBounds().width / 2, 0);

        // Победа
        victoryText.setFont(font);
        victoryText.setCharacterSize(50);
        victoryText.setFillColor(sf::Color::Green);
        victoryText.setPosition(260.f, 200.f);
        victoryText.setString("VICTORY!");

        victoryStatsText.setFont(font);
        victoryStatsText.setCharacterSize(24);
        victoryStatsText.setFillColor(sf::Color::White);
        victoryStatsText.setPosition(280.f, 280.f);
    }
}

void Game::updateUI() {
    // Обновление полос здоровья
    float playerHealthWidth = (static_cast<float>(player.getHealth()) / player.getMaxHealth()) * 200.0f;
    playerHealthWidth = std::max(0.f, std::min(playerHealthWidth, 200.f));
    playerHealthBar.setSize(sf::Vector2f(playerHealthWidth, 20.f));

    float enemyHealthWidth = (static_cast<float>(enemy.getHealth()) / 100.0f) * 200.0f;
    enemyHealthWidth = std::max(0.f, std::min(enemyHealthWidth, 200.f));
    enemyHealthBar.setSize(sf::Vector2f(enemyHealthWidth, 20.f));

    float bossHealthWidth = (static_cast<float>(boss.getHealth()) / 300.0f) * 300.0f;
    bossHealthWidth = std::max(0.f, std::min(bossHealthWidth, 300.f));
    bossHealthBar.setSize(sf::Vector2f(bossHealthWidth, 25.f));

    // Обновление текста
    if (fontLoaded) {
        killsText.setString("Kills: " + std::to_string(enemiesKilledThisGame) + "/" +
            std::to_string(enemiesToNextLevel) + " | Total: " + std::to_string(totalEnemiesKilled));
        levelText.setString("Level: " + std::to_string(currentLevel));
        skullsText.setString("Skulls: " + std::to_string(skullsCollected));
        currentSkullText.setString("Skull: " + player.getSkullName());
        statsText.setString("Kills this round: " + std::to_string(enemiesKilledThisGame) +
            "\nTotal kills: " + std::to_string(totalEnemiesKilled));
        victoryStatsText.setString("Total kills: " + std::to_string(totalEnemiesKilled) +
            "\nSkulls collected: " + std::to_string(skullsCollected));

        // Обновление подсказки для смены черепов
        if (player.canSwitchSkulls()) {
            switchSkullHintText.setString("Press E to switch skulls");
        }
        else {
            switchSkullHintText.setString("");
        }

        if (gameState == GameState::BOSS_FIGHT) {
            bossText.setString("BOSS FIGHT! Health: " + std::to_string(boss.getHealth()));
        }
        else {
            bossText.setString("");
        }
    }
}