#pragma once
#include <SFML/Graphics.hpp>
#include <random>

class Enemy {
public:
    Enemy();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    void takeDamage(int damage);
    int getHealth() const;
    void reset();
    void applyKnockback(bool fromRight, float distance);
    void applyVerticalVelocity(float velocity);
    void updateGravity(float deltaTime, float groundLevel);

    bool isAlive() const { return health > 0; }
    float getGlobalBoundsWidth() const { return shape.getSize().x; }

private:
    sf::RectangleShape shape;
    int health;
    float speed;
    int direction;
    bool isMoving;

    // Таймеры для ИИ
    float changeDirectionTimer;
    const float changeDirectionInterval;
    float stopTimer;
    const float stopInterval;

    // Гравитация
    float velocityY;
    const float gravity;
};

// Функция для генерации позиции врага
sf::Vector2f generateEnemyPosition();