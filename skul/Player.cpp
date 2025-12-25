#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player() :
    health(100),
    maxHealth(100),
    speed(200.f),
    onGround(true),
    jumpVelocity(0.f),
    gravity(1500.f),
    jumpStrength(-600.f),
    isFacingRight_(true),
    currentSkull(SkullType::NONE),
    currentSkullName("None"),
    isAttacking_(false),
    attackTimer(0.f),
    attackCooldown(0.5f),
    currentCooldown(0.f),
    attackDamage(15),
    shieldActive(false),
    shieldTimer(0.f),
    shieldDuration(2.0f),
    arrowActive(false),
    arrowTimer(0.f),
    arrowDuration(1.0f),
    arrowSpeed(400.f),
    arrowDamage(15), // Лучник: стрела 10 урона
    arrowCooldownTimer(0.f),
    arrowCooldown(0.5f),
    earthquakeActive(false),
    earthquakeTimer(0.f),
    earthquakeDuration(1.0f),
    earthquakeDamage(25),
    canTakeDamage_(true),
    damageCooldown(0.f),
    damageCooldownTime(1.5f),
    warriorColor(sf::Color::Blue),
    hunterColor(sf::Color::Green),
    giantColor(sf::Color(255, 165, 0)),
    defaultColor(sf::Color::White),
    currentSkullIndex(0),
    arrowDirection(1.0f)
{
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setPosition(100.f, 500.f);
    shape.setFillColor(defaultColor);

    attackArea.setSize(sf::Vector2f(70.f, 50.f));
    attackArea.setFillColor(sf::Color::Yellow);
    attackArea.setPosition(-100.f, -100.f);

    shieldArea.setSize(sf::Vector2f(60.f, 60.f));
    shieldArea.setFillColor(sf::Color(0, 100, 255, 150));
    shieldArea.setPosition(-200.f, -200.f);

    arrowArea.setSize(sf::Vector2f(30.f, 10.f));
    arrowArea.setFillColor(sf::Color::Green);
    arrowArea.setPosition(-200.f, -200.f);

    earthquakeArea.setSize(sf::Vector2f(150.f, 100.f));
    earthquakeArea.setFillColor(sf::Color(150, 75, 0, 150));
    earthquakeArea.setPosition(-200.f, -200.f);
}

void Player::resetAbilities() {
    isAttacking_ = false;
    shieldActive = false;
    arrowActive = false;
    earthquakeActive = false;

    attackArea.setPosition(-100.f, -100.f);
    shieldArea.setPosition(-200.f, -200.f);
    arrowArea.setPosition(-200.f, -200.f);
    earthquakeArea.setPosition(-200.f, -200.f);

    attackTimer = 0.f;
    shieldTimer = 0.f;
    arrowTimer = 0.f;
    earthquakeTimer = 0.f;
    currentCooldown = 0.f;
    arrowCooldownTimer = 0.f;
}

void Player::update(float deltaTime) {
    updateTimers(deltaTime);
    updateAttackPositions();
    updateArrowMovement(deltaTime);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    if (isAttacking_)
        window.draw(attackArea);
    if (shieldActive)
        window.draw(shieldArea);
    if (arrowActive)
        window.draw(arrowArea);
    if (earthquakeActive)
        window.draw(earthquakeArea);
}

void Player::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Player::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

void Player::takeDamage(int damage) {
    if (canTakeDamage_ && !shieldActive) {
        health -= damage;
        canTakeDamage_ = false;
        damageCooldown = damageCooldownTime;
    }
}

int Player::getHealth() const {
    return health;
}

int Player::getMaxHealth() const {
    return maxHealth;
}

void Player::setSkull(SkullType skull) {
    currentSkull = skull;
    updateSkullProperties();
}

SkullType Player::getSkull() const {
    return currentSkull;
}

std::string Player::getSkullName() const {
    return currentSkullName;
}

void Player::reset() {
    health = maxHealth;
    setPosition(sf::Vector2f(100.f, 500.f));
    onGround = true;
    jumpVelocity = 0.f;
    isAttacking_ = false;
    currentCooldown = 0.f;
    shieldActive = false;
    arrowActive = false;
    earthquakeActive = false;
    canTakeDamage_ = true;
    currentSkull = SkullType::NONE;
    currentSkullName = "None";
    shape.setFillColor(defaultColor);
    attackDamage = 15;

    attackArea.setPosition(-100.f, -100.f);
    shieldArea.setPosition(-200.f, -200.f);
    arrowArea.setPosition(-200.f, -200.f);
    earthquakeArea.setPosition(-200.f, -200.f);

    // Сброс коллекции черепов
    collectedSkulls.clear();
    currentSkullIndex = 0;
}

void Player::startAttack() {
    if (currentCooldown <= 0.f) {
        isAttacking_ = true;
        attackTimer = 0.3f;
        currentCooldown = attackCooldown;
        updateAttackPositions();
    }
}

void Player::activateShield() {
    if (!shieldActive) {
        shieldActive = true;
        shieldTimer = shieldDuration;
        shieldArea.setPosition(shape.getPosition().x - 5.f, shape.getPosition().y - 5.f);
    }
}

void Player::shootArrow() {
    if (arrowCooldownTimer <= 0.f) {
        arrowActive = true;
        arrowTimer = arrowDuration;
        arrowCooldownTimer = arrowCooldown;

        // Сохраняем направление стрелы в момент выстрела
        arrowDirection = isFacingRight_ ? 1.0f : -1.0f;

        if (isFacingRight_) {
            arrowArea.setPosition(shape.getPosition().x + 50.f, shape.getPosition().y + 20.f);
        }
        else {
            arrowArea.setPosition(shape.getPosition().x - 30.f, shape.getPosition().y + 20.f);
        }
    }
}

// Новый метод для деактивации стрелы
void Player::deactivateArrow() {
    arrowActive = false;
    arrowArea.setPosition(-200.f, -200.f);
}

void Player::activateEarthquake() {
    if (!earthquakeActive) {
        earthquakeActive = true;
        earthquakeTimer = earthquakeDuration;
        earthquakeArea.setPosition(shape.getPosition().x - 50.f, shape.getPosition().y + 30.f);
    }
}

void Player::updateAttackPositions() {
    if (isAttacking_) {
        if (isFacingRight_) {
            attackArea.setPosition(shape.getPosition().x + 50.f, shape.getPosition().y);
        }
        else {
            attackArea.setPosition(shape.getPosition().x - 70.f, shape.getPosition().y);
        }
    }

    if (shieldActive) {
        shieldArea.setPosition(shape.getPosition().x - 5.f, shape.getPosition().y - 5.f);
    }

    if (earthquakeActive) {
        earthquakeArea.setPosition(shape.getPosition().x - 50.f, shape.getPosition().y + 30.f);
    }
}

void Player::updateTimers(float deltaTime) {
    if (currentCooldown > 0.f)
        currentCooldown -= deltaTime;

    if (arrowCooldownTimer > 0.f)
        arrowCooldownTimer -= deltaTime;

    if (!canTakeDamage_) {
        damageCooldown -= deltaTime;
        if (damageCooldown <= 0.f)
            canTakeDamage_ = true;
    }

    if (isAttacking_) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0.f) {
            isAttacking_ = false;
            attackArea.setPosition(-100.f, -100.f);
        }
    }

    if (shieldActive) {
        shieldTimer -= deltaTime;
        if (shieldTimer <= 0.f) {
            shieldActive = false;
            shieldArea.setPosition(-200.f, -200.f);
        }
    }

    if (arrowActive) {
        arrowTimer -= deltaTime;
        if (arrowTimer <= 0.f) {
            deactivateArrow();
        }
    }

    if (earthquakeActive) {
        earthquakeTimer -= deltaTime;
        if (earthquakeTimer <= 0.f) {
            earthquakeActive = false;
            earthquakeArea.setPosition(-200.f, -200.f);
        }
    }
}

void Player::updateArrowMovement(float deltaTime) {
    if (arrowActive) {
        // Используем сохраненное направление стрелы, а не текущее направление игрока
        arrowArea.move(arrowSpeed * arrowDirection * deltaTime, 0.f);

        // Проверяем границы экрана для стрелы
        if (arrowArea.getPosition().x < -100 || arrowArea.getPosition().x > 900) {
            deactivateArrow();
        }
    }
}

void Player::applyGravity(float deltaTime, float groundLevel) {
    if (!onGround) {
        jumpVelocity += gravity * deltaTime;
        shape.move(0.f, jumpVelocity * deltaTime);

        if (shape.getPosition().y + shape.getSize().y >= groundLevel) {
            shape.setPosition(shape.getPosition().x, groundLevel - shape.getSize().y);
            onGround = true;
            jumpVelocity = 0.f;
        }
    }
}

void Player::move(const sf::Vector2f& movement) {
    shape.move(movement);
    if (movement.x > 0) {
        isFacingRight_ = true;
    }
    else if (movement.x < 0) {
        isFacingRight_ = false;
    }
}

void Player::jump() {
    if (onGround) {
        jumpVelocity = jumpStrength;
        onGround = false;
    }
}

int Player::getAttackDamage(bool inJump) const {
    int damage = attackDamage;
    if (inJump) {
        damage += 5;
    }
    return damage;
}

void Player::addSkullToCollection(SkullType skull) {
    // Проверяем, нет ли уже такого черепа в коллекции
    auto it = std::find(collectedSkulls.begin(), collectedSkulls.end(), skull);
    if (it == collectedSkulls.end()) {
        collectedSkulls.push_back(skull);
        // Если это первый череп, сразу устанавливаем его
        if (collectedSkulls.size() == 1) {
            currentSkullIndex = 0;
            setSkull(skull);
        }
    }
}

void Player::switchToNextSkull() {
    if (collectedSkulls.size() >= 2) {
        currentSkullIndex = (currentSkullIndex + 1) % collectedSkulls.size();
        setSkull(collectedSkulls[currentSkullIndex]);
    }
}

void Player::updateSkullProperties() {
    switch (currentSkull) {
    case SkullType::WARRIOR:
        currentSkullName = "Warrior Skull";
        attackDamage = 25;
        shape.setFillColor(warriorColor);
        break;
    case SkullType::HUNTER:
        currentSkullName = "Hunter Skull";
        attackDamage = 15;
        shape.setFillColor(hunterColor);
        break;
    case SkullType::GIANT:
        currentSkullName = "Giant Skull";
        attackDamage = 20;
        shape.setFillColor(giantColor);
        break;
    default:
        currentSkullName = "None";
        attackDamage = 15;
        shape.setFillColor(defaultColor);
        break;
    }
}

std::string Player::getCurrentSkullDescription() const {
    switch (currentSkull) {
    case SkullType::WARRIOR:
        return "Powerful melee attacks";
    case SkullType::HUNTER:
        return "Ranged arrow attacks";
    case SkullType::GIANT:
        return "Massive area damage";
    default:
        return "Basic attacks";
    }
}

std::string Player::getCurrentSkullAbilityDescription() const {
    switch (currentSkull) {
    case SkullType::WARRIOR:
        return "Shield Wall: Block all damage and push enemies back";
    case SkullType::HUNTER:
        return "Arrow: Shoot projectile with cooldown";
    case SkullType::GIANT:
        return "Earthquake: AoE ground slam that knocks up enemies";
    default:
        return "No special ability";
    }
}