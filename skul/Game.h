#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "GameState.h"
#include "SkulType.h"

// Функция для проверки пересечения прямоугольников
bool checkCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2);

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    void handleMainMenuInput();
    void handlePlayingInput(float deltaTime);
    void handleChestInput();
    void handleGameOverInput();
    void handleVictoryInput();
    void handleBossFightInput(float deltaTime);

    void updateMainMenu(float deltaTime);
    void updatePlaying(float deltaTime);
    void updateChest(float deltaTime);
    void updateGameOver(float deltaTime);
    void updateVictory(float deltaTime);
    void updateBossFight(float deltaTime);

    void renderMainMenu();
    void renderPlaying();
    void renderChest();
    void renderGameOver();
    void renderVictory();
    void renderBossFight();

    void resetGame();
    void setupUI();
    void updateUI();
    void handlePlayerCombat();
    void handleBossCombat();
    void levelUp();

    // Окно и состояния
    sf::RenderWindow window;
    GameState gameState;

    // Игровые объекты
    Player player;
    Enemy enemy;
    Boss boss;
    sf::RectangleShape ground;
    sf::RectangleShape chest;

    // UI элементы
    sf::RectangleShape playerHealthBar;
    sf::RectangleShape enemyHealthBar;
    sf::RectangleShape bossHealthBar;
    sf::RectangleShape playerHealthBackground;
    sf::RectangleShape enemyHealthBackground;
    sf::RectangleShape bossHealthBackground;

    // Текст
    sf::Font font;
    bool fontLoaded;
    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text startText;
    sf::Text killsText;
    sf::Text levelText;
    sf::Text skullsText;
    sf::Text currentSkullText;
    sf::Text bossText;
    sf::Text gameOverText;
    sf::Text statsText;
    sf::Text restartText;
    sf::Text chestTitleText;
    sf::Text skullNameText;
    sf::Text skullDescriptionText;
    sf::Text skullAbilityText;
    sf::Text chestInstructionText;
    sf::Text chestTimerText;
    sf::Text victoryText;
    sf::Text victoryStatsText;
    sf::Text switchSkullHintText;

    // Игровые переменные
    int totalEnemiesKilled;
    int enemiesKilledThisGame;
    int currentLevel;
    int enemiesToNextLevel;
    int skullsCollected;

    bool allLevelsCompleted;
    float chestTimer;
    const float chestTimerMax;
    bool canContinueFromChest;

    // Система черепов
    std::vector<SkullType> availableSkulls;
    SkullType lastSkull;

    // Флаг активности босса
    bool bossIsActive;

    sf::Clock gameClock;
};