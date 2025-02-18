#include "Enemy.hpp"
#include <cmath>

Enemy::Enemy(float x, float y, int hp) : Entity(x, y, sf::Color::Red, hp) {
	pos = { x,y };
    shape.setPosition(pos);
    //shape.setFillColor(sf::Color::Red);
    detectionRadius = 100;
    currentState = PATROL;
}

void Enemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
    switch (currentState) {
    case PATROL: {
        
        patrol();
        if (detectPlayer(players[0]->pos)) {
            
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
void Enemy::Path(Vector2i start, Vector2i end, Pathfinding& path, Grid& grid) {
    if (currentPath.empty()) {
        currentPath = path.findPath(grid, start, end);
        currentPathIndex = 0;
    }

    if (!currentPath.empty() && currentPathIndex < currentPath.size()) {
        Vector2i target = currentPath[currentPathIndex];
        Vector2f targetPos(target.x * CELL_SIZE, target.y * CELL_SIZE);

        Vector2f direction = targetPos - pos;
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f) {
            currentPathIndex = (currentPathIndex + 1) % currentPath.size();
        }
        else {
            direction /= distance;
            pos += direction * 2.0f;
        }

        if (Delay.getElapsedTime().asSeconds() > 1) {
            Delay.restart();
            cout << "Start : " << target.x << " ," << target.y << endl;
            cout << "End : " << target.x << " ," << target.y << endl;
        }

    }
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
    }
    
    shape.setPosition(pos);
}

void Enemy::chase(Vector2f playerPos) {
    Vector2f direction = playerPos - pos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
        pos += direction * 2.0f;
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

