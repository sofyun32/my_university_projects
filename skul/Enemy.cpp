#include "Enemy.h"
#include <cstdlib>
#include <ctime>

Enemy::Enemy() :
    health(100),
    speed(140.f),
    direction(1),
    isMoving(true),
    changeDirectionTimer(0.f),
    changeDirectionInterval(1.5f),
    stopTimer(0.f),
    stopInterval(0.8f),
    velocityY(0.f),
    gravity(1500.f)
{
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setPosition(generateEnemyPosition());
    shape.setFillColor(sf::Color::Red);
}

void Enemy::update(float deltaTime) {
    changeDirectionTimer -= deltaTime;

    if (isMoving) {
        if (changeDirectionTimer <= 0.f) {
            isMoving = false;
            stopTimer = stopInterval;
            changeDirectionTimer = changeDirectionInterval;
        }

        sf::Vector2f movement(speed * direction * deltaTime, 0.f);
        shape.move(movement);

        // Проверка границ экрана
        if (shape.getPosition().x <= 0 || shape.getPosition().x >= 800 - shape.getSize().x) {
            direction *= -1;
        }
    }
    else {
        stopTimer -= deltaTime;
        if (stopTimer <= 0.f) {
            isMoving = true;
            direction = (rand() % 2 == 0) ? 1 : -1;
            speed = 100.f + (rand() % 80);
            changeDirectionTimer = changeDirectionInterval + (rand() % 100) / 100.f;
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

void Enemy::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);
}

sf::Vector2f Enemy::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Enemy::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

void Enemy::takeDamage(int damage) {
    health -= damage;
}

int Enemy::getHealth() const {
    return health;
}

void Enemy::reset() {
    health = 100;
    shape.setPosition(generateEnemyPosition());
    speed = 140.f;
    direction = 1;
    isMoving = true;
    velocityY = 0.f;
}

void Enemy::applyKnockback(bool fromRight, float distance) {
    sf::Vector2f newPos = shape.getPosition();
    if (fromRight) {
        newPos.x += distance;
    }
    else {
        newPos.x -= distance;
    }

    // Проверка границ
    if (newPos.x >= 0 && newPos.x <= 800 - shape.getSize().x) {
        shape.setPosition(newPos);
    }
}

void Enemy::applyVerticalVelocity(float velocity) {
    velocityY = velocity;
}

void Enemy::updateGravity(float deltaTime, float groundLevel) {
    if (velocityY != 0.f) {
        velocityY += gravity * deltaTime;
        shape.move(0.f, velocityY * deltaTime);

        if (shape.getPosition().y + shape.getSize().y >= groundLevel) {
            shape.setPosition(shape.getPosition().x, groundLevel - shape.getSize().y);
            velocityY = 0.f;
        }
    }
}

sf::Vector2f generateEnemyPosition() {
    float x = 100.f + (rand() % 600);
    return sf::Vector2f(x, 500.f);
}