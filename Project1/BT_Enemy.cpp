#include "BT_Enemy.hpp"

Projectile::Projectile(sf::Vector2f targ, sf::Vector2f originPos) : target(targ) {
    direction = target - originPos;
    distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    shape.setPosition(originPos);
    shape.setRadius(10);
    shape.setFillColor(sf::Color::Red);
}

void Projectile::update(std::vector<Entity*> players, Grid& grid) {
    direction = target - shape.getPosition();
    distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= distance;
    shape.setPosition(shape.getPosition() + direction * 4.f);

    if (players[0]->shape.getGlobalBounds().intersects(shape.getGlobalBounds())) {
        state = false;
        players[0]->health -= 1;
        std::cout << "hit" << std::endl;
    }

    if (!grid.getCell(shape.getPosition().x/CELL_SIZE, shape.getPosition().y / CELL_SIZE).walkable) {
        if (shape.getGlobalBounds().intersects(grid.getCell(shape.getPosition().x / CELL_SIZE, shape.getPosition().y / CELL_SIZE).shape.getGlobalBounds())) {
            state = false;
        }
    }  
}

BTEnemy::BTEnemy(float x, float y, int hp) : Entity(x, y, sf::Color::Red, hp) {
    pos = { x,y };
    initialPos = { x,y };
    shape.setPosition(pos);
    detectionRadius = 100;
    textSprite.loadFromFile("../assets/blob.png");
    shape.setTexture(&textSprite);
};
void BTEnemy::rayCasting(Grid& grid, RenderWindow& window) {

    float fov = 40.f;
    int numRays = 30;
    float maxDistancePrimary = 150.0f;
    float maxDistanceSecondary = 300.0f;

    float angleStart = -fov / 2.0f;
    float angleStep = fov / static_cast<float>(numRays - 1);

    m_primaryCone.setPointCount(numRays + 1);
    m_primaryCone.setFillColor(Color(150, 255, 243, 100));

    m_secondaryCone.setPointCount(numRays + 1);
    m_secondaryCone.setFillColor(Color(108, 217, 204, 50));

    m_primaryCone.setPoint(0, pos);
    m_secondaryCone.setPoint(0, pos);

    float baseAngle = atan2(e_direction.y, e_direction.x) * 180.f / 3.14159f;

    for (int i = 0; i < numRays; i++) {
        float angle = baseAngle + angleStart + i * angleStep;
        float rad = angle * 3.14159f / 180.0f;

        Vector2f direction(cos(rad), sin(rad));


        Vector2f rayPosPrimary = pos;
        float distancePrimary = maxDistancePrimary;
        while (distancePrimary > 0) {
            rayPosPrimary += direction * 5.0f;
            distancePrimary -= 5.0f;

            int gridX = static_cast<int>(rayPosPrimary.x) / CELL_SIZE;
            int gridY = static_cast<int>(rayPosPrimary.y) / CELL_SIZE;

            if (gridX < 0 || gridX >= GRID_WIDTH || gridY < 0 || gridY >= GRID_HEIGHT) {
                break;
            }

            if (!grid.getCell(gridX, gridY).walkable) {
                break;
            }
        }

        Vector2f rayPosSecondary = pos;
        float distanceSecondary = maxDistanceSecondary;
        while (distanceSecondary > 0) {
            rayPosSecondary += direction * 5.0f;
            distanceSecondary -= 5.0f;

            int gridX = static_cast<int>(rayPosSecondary.x) / CELL_SIZE;
            int gridY = static_cast<int>(rayPosSecondary.y) / CELL_SIZE;

            if (gridX < 0 || gridX >= GRID_WIDTH || gridY < 0 || gridY >= GRID_HEIGHT) {
                break;
            }

            if (!grid.getCell(gridX, gridY).walkable) {
                break;
            }
        }

        m_primaryCone.setPoint(numRays, m_primaryCone.getPoint(1));
        m_secondaryCone.setPoint(numRays, m_secondaryCone.getPoint(1));
    }

    window.draw(m_secondaryCone);
    window.draw(m_primaryCone);
}

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

    shape.move(direction);
    
    
}

bool BTEnemy::detectPlayer(sf::Vector2f playerPos) {
    float distance = sqrt(pow(playerPos.x - pos.x, 2) + pow(playerPos.y - pos.y, 2));
    return (distance < detectionRadius);
}

void BTEnemy::shoot(std::vector<Entity*> players) {
    projectiles.emplace_back(std::make_shared<Projectile>(players[0]->pos, pos));
}

void BTEnemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
    movement();
    shootTimer += deltaTime;
    if (shootTimer > shootCd) {
        shoot(players);
        shootTimer = 0;
    }
    else {
        for (int i = 0; i < projectiles.size(); i++) {
            projectiles[i]->update(players, grid);
            if (projectiles[i]->distance < 5.f || projectiles[i]->state == false) {
                projectiles.erase(projectiles.begin() + i);
            }
        }
    }
}