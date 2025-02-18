#include "A_Ennemy.hpp"
#include <cmath>

A_Ennemy::A_Ennemy(float x, float y, int hp) : Entity(x, y, sf::Color::Red, hp) {
    pos = { x,y };
    shape.setPosition(pos);
    //shape.setFillColor(sf::Color::Red);
    detectionRadius = 100;
    currentState = PATROL;
    bool reversePath = false;
}

void A_Ennemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
}

bool A_Ennemy::detectPlayer(Vector2f playerPos) {
    float distance = sqrt(pow(playerPos.x - pos.x, 2) + pow(playerPos.y - pos.y, 2));
    return (distance < detectionRadius);
}
void A_Ennemy::rayCasting(Grid& grid, RenderWindow& window) {
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

        m_primaryCone.setPoint(i + 1, rayPosPrimary);
        m_secondaryCone.setPoint(i + 1, rayPosSecondary);
    }

    window.draw(m_secondaryCone);
    window.draw(m_primaryCone);
}
void A_Ennemy::Path(Vector2i start, Vector2i end, Pathfinding& path, Grid& grid) {
    if (start != previousStart || end != previousEnd) {
        currentPath.clear();
        currentPathIndex = 0;
        previousStart = start;
        previousEnd = end;
    }

    if (currentPath.empty()) {
        currentPath = path.findPath(grid, start, end);
        currentPathIndex = 0;
        reversePath = false;
    }

    if (!currentPath.empty() && currentPathIndex < currentPath.size()) {
        Vector2i target = currentPath[currentPathIndex];
        Vector2f targetPos(target.x * CELL_SIZE, target.y * CELL_SIZE);
        Vector2f direction = targetPos - pos;

        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f) {

            if (!reversePath && currentPathIndex == currentPath.size() - 1) {
                reversePath = true;
            }
            else if (reversePath && currentPathIndex == 0) {
                reversePath = false;
            }

            
            if (reversePath) {
                currentPathIndex--;
            }
            else {
                currentPathIndex++;
            }
        }
        else {
            direction /= distance;
            pos += direction * 2.0f;
        }
    }
    shape.setPosition(pos);
}





