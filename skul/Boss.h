#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Boss {
public:
    Boss();

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

    // Стрелы босса
    void updateArrows(float deltaTime);
    void shootArrows();
    const std::vector<sf::RectangleShape>& getArrows() const { return arrows; }
    void removeArrow(size_t index);

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

    // Стрелы
    std::vector<sf::RectangleShape> arrows;
    std::vector<sf::Vector2f> arrowVelocities;
    float arrowCooldown;
    const float arrowInterval;
    const float arrowSpeed;
};