#include "Enemy.hpp"
#include <cmath>

Enemy::Enemy(float x, float y, int hp) : Entity(x, y, sf::Color::Red, hp) {
	pos = { x,y };
    shape.setPosition(pos);
    //shape.setFillColor(sf::Color::Red);
    detectionRadius = 100;
    currentState = PATROL;
    bool reversePath = false;
    
}

void Enemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
    switch (currentState) {
    case PATROL: {
        
        patrol();
        if (detectPlayer(players[0]->pos)) {
            lastPlayerPosition = players[0]->pos;
            currentState = CHASE;
        }
        break;
    }
    

    case CHASE: {
        
        chase(players[0]->pos);
        if (!detectPlayer(players[0]->pos)) {
            
            currentState = PATROL;
        }
        break;
    }
        

    case SEARCH:
        search(lastPlayerPosition, deltaTime);
        break;
    }

    
}

bool Enemy::detectPlayer(Vector2f playerPos) {
    float distance = sqrt(pow(playerPos.x - pos.x, 2) + pow(playerPos.y - pos.y, 2));
    return (distance < detectionRadius);
}
void Enemy::rayCasting(Grid& grid, RenderWindow& window) {
    std::cout << "Primary Cone: " << m_primaryCone.getPointCount() << " points" << std::endl;
    std::cout << "Secondary Cone: " << m_secondaryCone.getPointCount() << " points" << std::endl;
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


void Enemy::patrol() {
    static int currentWaypoint = 0;
    static sf::Vector2f waypoints[4] = { sf::Vector2f(100, 300), sf::Vector2f(500, 100), sf::Vector2f(100, 300), sf::Vector2f(500, 300) };
    sf::Vector2f target = waypoints[currentWaypoint];
    sf::Vector2f direction = target - pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 5.0f) {
        currentWaypoint = (currentWaypoint + 1) % 4;
    }
    else {
        direction /= distance;
        pos += direction * 0.2f;
        e_direction = direction;
    }
    shape.move(direction*SPEED);
    
}

void Enemy::chase(Vector2f playerPos) {
    Vector2f direction = playerPos - pos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
        pos += direction * 2.0f;
        e_direction = direction;
    }

    shape.setPosition(pos);
}

void Enemy::search(sf::Vector2f lastPlayerPos, float deltaTime) {
    static float searchTimer = 0.0f;
    static sf::Vector2f searchDirection;

    if (searchTimer == 0.0f) {
        searchDirection = sf::Vector2f(rand() % 2 == 0 ? -1 : 1, rand() % 2 == 0 ? -1 : 1);
        searchDirection /= std::sqrt(searchDirection.x * searchDirection.x + searchDirection.y * searchDirection.y);
    }

    searchTimer += deltaTime;
    if (searchTimer < 10.0f) {
        pos += searchDirection * 5.f * deltaTime;
    }
    else {
        searchTimer = 0.0f;
        currentState = PATROL;
    }

    float distance = std::sqrt((lastPlayerPos.x - pos.x) * (lastPlayerPos.x - pos.x) + (lastPlayerPos.y - pos.y) * (lastPlayerPos.y - pos.y));
    if (distance < detectionRadius) {
        searchTimer = 0.0f;
    }

    shape.setPosition(pos);
}

