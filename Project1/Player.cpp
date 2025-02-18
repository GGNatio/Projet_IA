// player.cpp
#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "Enemy.hpp"
Player::Player(float x, float y, int hp) : Entity(x, y, sf::Color::Blue, hp), attackTimer(0.f) {
    pos = { x,y };
    shape.setSize({ 20,20 });
}

void Player::update(float deltaTime, Grid& grid, std::vector<Entity*> enemies, sf::Vector2f playerPos) {
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
        if (enemy = dynamic_cast<Enemy*>(enemy)) {
            if (enemy->isAlive() && shape.getGlobalBounds().intersects(enemy->shape.getGlobalBounds())) {
                enemy->takeDamage(DAMAGE);
                std::cout << "Enemy HP: " << enemy->health << std::endl;
            }
        }
		
	}
    std::cout << "Player attacks" << std::endl;
}