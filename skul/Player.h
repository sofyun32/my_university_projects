#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "SkulType.h"

class Player {
public:
    Player();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    void takeDamage(int damage);
    int getHealth() const;
    int getMaxHealth() const;
    void setSkull(SkullType skull);
    SkullType getSkull() const;
    std::string getSkullName() const;
    void reset();

    // Атаки и способности
    void startAttack();
    void activateShield();
    void shootArrow();
    void activateEarthquake();
    void deactivateArrow(); // Добавляем новый метод

    // Геттеры для состояний атак
    bool isAttacking() const { return isAttacking_; }
    bool isShieldActive() const { return shieldActive; }
    bool isArrowActive() const { return arrowActive; }
    bool isEarthquakeActive() const { return earthquakeActive; }

    // Геттеры для областей атак
    sf::FloatRect getAttackBounds() const { return attackArea.getGlobalBounds(); }
    sf::FloatRect getShieldBounds() const { return shieldArea.getGlobalBounds(); }
    sf::FloatRect getArrowBounds() const { return arrowArea.getGlobalBounds(); }
    sf::FloatRect getEarthquakeBounds() const { return earthquakeArea.getGlobalBounds(); }

    // Обновление позиций атак
    void updateAttackPositions();

    // Обновление таймеров
    void updateTimers(float deltaTime);

    // Проверка столкновений
    bool canTakeDamage() const { return canTakeDamage_; }

    // Гравитация и движение
    void applyGravity(float deltaTime, float groundLevel);
    void move(const sf::Vector2f& movement);

    // Направление
    bool isFacingRight() const { return isFacingRight_; }
    void setFacingRight(bool facingRight) { isFacingRight_ = facingRight; }

    // Прыжок
    void jump();
    bool isOnGround() const { return onGround; }

    // Скорость
    float getSpeed() const { return speed; }

    // Урон
    int getAttackDamage(bool inJump) const;
    int getArrowDamage() const { return arrowDamage; }

    // Перезарядка стрелы
    bool isArrowCooldownReady() const { return arrowCooldownTimer <= 0.f; }

    // Обновление движения стрелы
    void updateArrowMovement(float deltaTime);

    // Система смены черепов
    void addSkullToCollection(SkullType skull);
    void switchToNextSkull();
    bool canSwitchSkulls() const { return collectedSkulls.size() >= 2; }
    size_t getCollectedSkullsCount() const { return collectedSkulls.size(); }
    std::string getCurrentSkullDescription() const;
    std::string getCurrentSkullAbilityDescription() const;
    void resetAbilities();

private:
    float arrowDirection;
    sf::RectangleShape shape;
    int health;
    int maxHealth;
    float speed;
    bool onGround;
    float jumpVelocity;
    const float gravity;
    const float jumpStrength;
    bool isFacingRight_;
    SkullType currentSkull;
    std::string currentSkullName;

    // Базовая атака
    bool isAttacking_;
    float attackTimer;
    const float attackCooldown;
    float currentCooldown;
    int attackDamage;
    sf::RectangleShape attackArea;

    // Уникальные способности
    bool shieldActive;
    float shieldTimer;
    const float shieldDuration;
    sf::RectangleShape shieldArea;

    bool arrowActive;
    float arrowTimer;
    const float arrowDuration;
    sf::RectangleShape arrowArea;
    float arrowSpeed;
    int arrowDamage;
    float arrowCooldownTimer;
    const float arrowCooldown;

    bool earthquakeActive;
    float earthquakeTimer;
    const float earthquakeDuration;
    sf::RectangleShape earthquakeArea;
    int earthquakeDamage;

    bool canTakeDamage_;
    float damageCooldown;
    const float damageCooldownTime;

    // Цвета для каждого черепа
    sf::Color warriorColor;
    sf::Color hunterColor;
    sf::Color giantColor;
    sf::Color defaultColor;

    // Коллекция черепов
    std::vector<SkullType> collectedSkulls;
    size_t currentSkullIndex;

    // Приватные методы для обновления свойств черепа
    void updateSkullProperties();
};