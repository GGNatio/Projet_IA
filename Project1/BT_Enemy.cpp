#include "BT_Enemy.hpp"

BTEnemy::BTEnemy(float x, float y, int hp) : Entity(x, y, sf::Color::Red, hp) {
    pos = { x,y };
    initialPos = { x,y };
    shape.setPosition(pos);
    detectionRadius = 100;
};

void BTEnemy::movement() {
    static int currentWaypoint = 0;
    static sf::Vector2f waypoints[4] = { sf::Vector2f(initialPos.x - 150, initialPos.y - 150), sf::Vector2f(initialPos.x + 150, initialPos.y + 150),
    sf::Vector2f(initialPos.x - 150, initialPos.y + 150), sf::Vector2f(initialPos.x + 150, initialPos.y - 150) };
    sf::Vector2f target = waypoints[currentWaypoint];
    sf::Vector2f direction = target - pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 5.0f) {
        currentWaypoint = (currentWaypoint + 1) % 4;
    }
    else {
        direction /= distance;
        pos += direction * 0.7f;
    }

    shape.setPosition(pos);
}

bool BTEnemy::detectPlayer(sf::Vector2f playerPos) {
    float distance = sqrt(pow(playerPos.x - pos.x, 2) + pow(playerPos.y - pos.y, 2));
    return (distance < detectionRadius);
}

void BTEnemy::shoot(sf::Vector2f playerPos) {
    std::shared_ptr<Projectile> projectile;
    projectile->target = playerPos;
    projectile->direction = projectile->target - pos;
    projectile->distance = std::sqrt(projectile->direction.x * projectile->direction.x + projectile->direction.y * projectile->direction.y);
    projectile->shape.setPosition(pos);
    projectiles.push_back(projectile);
}

void BTEnemy::moveProjectiles() {
    for (auto& projectile : projectiles) {
        projectile->direction /= projectile->distance;
        //projectile->shape.setPosition(projectile->shape.getPosition() += projectile->direction * 0.7f);
        projectile->shape.move(projectile->direction * 0.7f);
    }
}

void BTEnemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
    movement();
    shoot(playerPos);                // CA CRASH ICI
}