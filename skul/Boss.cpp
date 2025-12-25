#include "Boss.h"
#include <cstdlib>
#include <ctime>

Boss::Boss() :
    health(300),
    speed(120.f),
    direction(1),
    isMoving(true),
    changeDirectionTimer(0.f),
    changeDirectionInterval(2.0f),
    stopTimer(0.f),
    stopInterval(1.0f),
    velocityY(0.f),
    gravity(1500.f),
    arrowCooldown(0.f),
    arrowInterval(4.0f),
    arrowSpeed(200.f)
{
    shape.setSize(sf::Vector2f(70.f, 70.f));
    shape.setPosition(-200.f, -200.f);
    shape.setFillColor(sf::Color::Magenta);
    shape.setOutlineThickness(3.f);
    shape.setOutlineColor(sf::Color(200, 0, 200));
}

void Boss::update(float deltaTime) {
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
            speed = 100.f + (rand() % 40);
            changeDirectionTimer = changeDirectionInterval + (rand() % 100) / 100.f;
        }
    }

    // Обновление стрел
    updateArrows(deltaTime);

    // Стрельба
    arrowCooldown -= deltaTime;
    if (arrowCooldown <= 0.f) {
        shootArrows();
        arrowCooldown = arrowInterval;
    }
}

void Boss::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    for (const auto& arrow : arrows) {
        window.draw(arrow);
    }
}

void Boss::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);
}

sf::Vector2f Boss::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Boss::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

void Boss::takeDamage(int damage) {
    health -= damage;
}

int Boss::getHealth() const {
    return health;
}

void Boss::reset() {
    health = 300;
    shape.setPosition(-200.f, -200.f);
    speed = 120.f;
    direction = 1;
    isMoving = true;
    velocityY = 0.f;
    arrows.clear();
    arrowVelocities.clear();
    arrowCooldown = 0.f;
}

void Boss::applyKnockback(bool fromRight, float distance) {
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

void Boss::applyVerticalVelocity(float velocity) {
    velocityY = velocity;
}

void Boss::updateGravity(float deltaTime, float groundLevel) {
    if (velocityY != 0.f) {
        velocityY += gravity * deltaTime;
        shape.move(0.f, velocityY * deltaTime);

        if (shape.getPosition().y + shape.getSize().y >= groundLevel) {
            shape.setPosition(shape.getPosition().x, groundLevel - shape.getSize().y);
            velocityY = 0.f;
        }
    }
}

void Boss::updateArrows(float deltaTime) {
    for (size_t i = 0; i < arrows.size(); ++i) {
        arrows[i].move(arrowVelocities[i].x * deltaTime, arrowVelocities[i].y * deltaTime);

        // Удаляем стрелы за пределами экрана
        if (arrows[i].getPosition().x < -100 || arrows[i].getPosition().x > 900) {
            arrows.erase(arrows.begin() + i);
            arrowVelocities.erase(arrowVelocities.begin() + i);
            break;
        }
    }
}

void Boss::shootArrows() {
    // Стрела влево
    sf::RectangleShape leftArrow(sf::Vector2f(40.f, 10.f));
    leftArrow.setFillColor(sf::Color::Red);
    leftArrow.setPosition(shape.getPosition().x, shape.getPosition().y + 35.f);
    arrows.push_back(leftArrow);
    arrowVelocities.push_back(sf::Vector2f(-arrowSpeed, 0.f));

    // Стрела вправо
    sf::RectangleShape rightArrow(sf::Vector2f(40.f, 10.f));
    rightArrow.setFillColor(sf::Color::Red);
    rightArrow.setPosition(shape.getPosition().x + shape.getSize().x, shape.getPosition().y + 35.f);
    arrows.push_back(rightArrow);
    arrowVelocities.push_back(sf::Vector2f(arrowSpeed, 0.f));
}

void Boss::removeArrow(size_t index) {
    if (index < arrows.size()) {
        arrows.erase(arrows.begin() + index);
        arrowVelocities.erase(arrowVelocities.begin() + index);
    }
}