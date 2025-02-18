// player.cpp
#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "Enemy.hpp"
#include "BT_Enemy.hpp"
Player::Player(float x, float y, int hp) : Entity(x, y, sf::Color::Blue, hp), attackTimer(0.f) {
    pos = { x,y };
    shape.setSize({ 50,50 });

    atkRadius.setRadius(100);
    atkRadius.setFillColor(sf::Color::Transparent);
    atkRadius.setOutlineColor(sf::Color::Red);
    atkRadius.setOutlineThickness(1);
    health = 40;

    textSprite.loadFromFile("../assets/perso.png");
    shape.setTexture(&textSprite);
    /*sprite.setScale({ 0.25f,0.25f });
    sprite.setTexture(textSprite);
    sprite.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
    sprite.setPosition(pos.x-shape.getSize().x, pos.y - shape.getSize().y)*/;

}

void Player::update(float deltaTime, Grid& grid, std::vector<Entity*> enemies, sf::Vector2f playerPos) {
    

    atkRadius.setPosition(pos.x + shape.getSize().x /2 - atkRadius.getRadius(), pos.y + shape.getSize().y / 2 - atkRadius.getRadius());

    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) movement.y -= SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) movement.x -= SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += SPEED * deltaTime;

    sf::Vector2f newPosition = shape.getPosition() + movement;
    sf::FloatRect newBounds(newPosition, shape.getSize());
    

    auto isWalkable = [&](float x, float y) {
        int gridX = static_cast<int>(x / CELL_SIZE);
        int gridY = static_cast<int>(y / CELL_SIZE);
        return gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT && grid.getCell(gridX, gridY).walkable;
        };

    if (isWalkable(newBounds.left - 2, newBounds.top - 2) &&
        isWalkable(newBounds.left + newBounds.width + 2, newBounds.top - 2) &&
        isWalkable(newBounds.left - 2, newBounds.top + newBounds.height + 2) &&
        isWalkable(newBounds.left + newBounds.width + 2, newBounds.top + newBounds.height + 2)) {
        shape.move(movement);
        
        pos = shape.getPosition();
        
        
    }

    attackTimer += deltaTime;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && attackTimer >= ATTACK_COOLDOWN) {
        attack(enemies);
        attackTimer = 0.f;
    }
}
void Player::rayCasting(Grid& grid, RenderWindow& window) {

}
void Player::attack(std::vector<Entity*>enemies) {
	for (auto& enemy : enemies) {
        if (enemy = dynamic_cast<Entity*>(enemy)) {
            if (enemy->isAlive() && atkRadius.getGlobalBounds().intersects(enemy->shape.getGlobalBounds())) {
                enemy->takeDamage(DAMAGE);
                std::cout << "Enemy HP: " << enemy->health << std::endl;
            }
        }
		
	}
    std::cout << "Player attacks" << std::endl;
}