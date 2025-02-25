#include "BT_Enemy.hpp"

SelectorNode::SelectorNode(Blackboard& blackboard) {
    std::unique_ptr<SequenceNode> sequence = std::make_unique<SequenceNode>();
    sequence->AddChild(std::make_unique<ConditionNode>(blackboard, "getAway", 0));
    sequence->AddChild(std::make_unique<ConditionNode>(blackboard, "PlayerDetected", 0));
    sequence->AddChild(std::make_unique<ActionNode>("movement"));
    std::unique_ptr<SequenceNode> sequence1 = std::make_unique<SequenceNode>();
    sequence1->AddChild(std::make_unique<ConditionNode>(blackboard, "PlayerDetected", 1));
    sequence1->AddChild(std::make_unique<ActionNode>("chase"));
    sequence1->AddChild(std::make_unique<ConditionNode>(blackboard, "Cooldown", 1));
    sequence1->AddChild(std::make_unique<ActionNode>("shoot"));
    std::unique_ptr<SequenceNode> sequence2 = std::make_unique<SequenceNode>();
    sequence2->AddChild(std::make_unique<ConditionNode>(blackboard, "getAway", 1));
    sequence2->AddChild(std::make_unique<ConditionNode>(blackboard, "wallCollision", 0));
    sequence2->AddChild(std::make_unique<ActionNode>("getAway"));
    std::unique_ptr<SequenceNode> sequence3 = std::make_unique<SequenceNode>();
    sequence3->AddChild(std::make_unique<ConditionNode>(blackboard, "wallCollision", 1));
    sequence3->AddChild(std::make_unique<ActionNode>("wallCollision"));
    AddChild(std::move(sequence));
    AddChild(std::move(sequence1));
    AddChild(std::move(sequence2));
    AddChild(std::move(sequence3));
}

ActionNode::ActionNode(std::string name) : actionName(name) {}

NodeState ActionNode::execute(Grid& grid, Blackboard& blackboard, sf::RectangleShape& shape, std::vector<Entity*> players, std::vector<std::shared_ptr<Projectile>>& projectiles) {
    if (actionName == "shoot") {
        projectiles.emplace_back(std::make_shared<Projectile>(sf::Vector2f(players[0]->pos.x + players[0]->shape.getSize().x/2,
            players[0]->pos.y + players[0]->shape.getSize().y / 2), sf::Vector2f(blackboard.pos.x + shape.getSize().x/2, blackboard.pos.y + shape.getSize().y/2)));
    }
    if (actionName == "movement") {
        static int currentWaypoint = 0;
        static sf::Vector2f waypoints[4] = { sf::Vector2f(blackboard.initialPos.x - 150, blackboard.initialPos.y - 150), sf::Vector2f(blackboard.initialPos.x + 150, blackboard.initialPos.y + 150),
        sf::Vector2f(blackboard.initialPos.x - 150, blackboard.initialPos.y + 150), sf::Vector2f(blackboard.initialPos.x + 150, blackboard.initialPos.y - 150) };
        sf::Vector2f target = waypoints[currentWaypoint];
        sf::Vector2f direction = target - shape.getPosition();
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f) {
            currentWaypoint = (currentWaypoint + 1) % 4;
        }
        else {
        direction /= distance;
        //blackboard.pos += direction * 0.7f;
        //shape.setPosition(shape.getPosition() + direction * 1.7f);
        }

        shape.move(direction * 2.5f);

        if (shape.getGlobalBounds().intersects(grid.getCell(shape.getPosition().x / CELL_SIZE, shape.getPosition().y / CELL_SIZE).shape.getGlobalBounds())) {
            if (!grid.getCell(shape.getPosition().x / CELL_SIZE, shape.getPosition().y / CELL_SIZE).walkable) {
                blackboard.wallCollision = true;
                //shape.move(-direction * 1.7f);
            }
            else {
                blackboard.previousPos = shape.getPosition();
                blackboard.wallCollision = false;
            }
        }
    }
    if (actionName == "chase") {
        sf::Vector2f direction = players[0]->pos - shape.getPosition();
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0) {
            direction /= distance;
            //shape.setPosition(shape.getPosition() + direction * 0.3f);
            //e_direction = direction;
        }

        shape.move(direction * 0.5f);
    }
    if (actionName == "getAway") {
        sf::Vector2f direction = -(blackboard.target - shape.getPosition());
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < 5.0f) {
            blackboard.fleeing = false;
        }
        else {
            direction /= distance;
            //blackboard.pos += direction * 0.7f;
            //shape.setPosition(shape.getPosition() + direction * 3.f);
        }

        shape.move(direction * 6.f);

        if (shape.getGlobalBounds().intersects(grid.getCell(shape.getPosition().x / CELL_SIZE, shape.getPosition().y / CELL_SIZE).shape.getGlobalBounds())) {
            if (!grid.getCell(shape.getPosition().x / CELL_SIZE, shape.getPosition().y / CELL_SIZE).walkable) {
                blackboard.wallCollision = true;
                //shape.move(-direction * 3.f);
            }
            else {
                blackboard.previousPos = shape.getPosition();
                blackboard.wallCollision = false;
            }
        }
    }

    if (actionName == "wallCollision") {
        shape.setPosition(blackboard.previousPos);
    }
    return NodeState::SUCCESS;
}

Projectile::Projectile(sf::Vector2f targ, sf::Vector2f originPos) : target(targ) {
    direction = target - originPos;
    distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    shape.setRadius(10);
    shape.setPosition(originPos.x - shape.getRadius(), originPos.y - shape.getRadius());
    shape.setFillColor(sf::Color::Red);
}

void Projectile::update(std::vector<Entity*> players, Grid& grid) {
    direction = target - shape.getPosition();
    distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= distance;
    //shape.setPosition(shape.getPosition() + direction * 5.f);
    shape.move(direction * 5.f);

    if (players[0]->shape.getGlobalBounds().intersects(shape.getGlobalBounds())) {
        state = false;
        players[0]->takeDamage(1);
        players[0]->gotHit = true;
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
    detectionRadius = 200;
    textSprite.loadFromFile("../assets/blob.png");
    shape.setTexture(&textSprite);
    blackboard.initialPos = initialPos;
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

//void BTEnemy::movement() {
//    static int currentWaypoint = 0;
//    static sf::Vector2f waypoints[4] = { sf::Vector2f(initialPos.x - 150, initialPos.y - 150), sf::Vector2f(initialPos.x + 150, initialPos.y + 150),
//    sf::Vector2f(initialPos.x - 150, initialPos.y + 150), sf::Vector2f(initialPos.x + 150, initialPos.y - 150) };
//    sf::Vector2f target = waypoints[currentWaypoint];
//    sf::Vector2f direction = target - pos;
//    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
//
//    if (distance < 5.0f) {
//        currentWaypoint = (currentWaypoint + 1) % 4;
//    }
//    else {
//        direction /= distance;
//        pos += direction * 0.7f;
//    }
//
//    shape.move(direction);
//    
//    
//}

bool BTEnemy::detectPlayer(sf::Vector2f playerPos) {
    float distance = sqrt(pow(playerPos.x - pos.x, 2) + pow(playerPos.y - pos.y, 2));
    return (distance < detectionRadius);
}

//void BTEnemy::shoot(std::vector<Entity*> players) {
//    projectiles.emplace_back(std::make_shared<Projectile>(players[0]->pos, pos));
//}

void BTEnemy::update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) {
    pos = shape.getPosition();
    blackboard.pos = pos;
    //blackboard.previousPos = pos;
    blackboard.shape = shape;
    //movement();
    for (int i = 0; i < projectiles.size(); i++) {
        projectiles[i]->update(players, grid);
        if (projectiles[i]->distance < 5.f || projectiles[i]->state == false) {
            projectiles.erase(projectiles.begin() + i);
        }
    }

    if (!isAlive()) {
        projectiles.clear();
    }

    shootTimer += deltaTime;
    if (shootTimer > shootCd) {
        //shoot(players);
        blackboard.SetValue("Cooldown", 1);
        shootTimer = 0;
    }
    else {
        blackboard.SetValue("Cooldown", 0);
    }
    
    if (detectPlayer(playerPos)) {
        blackboard.SetValue("PlayerDetected", 1);
    }
    else (blackboard.SetValue("PlayerDetected", 0));

    if (blackboard.fleeing) {
        blackboard.SetValue("getAway", 1);
        blackboard.fleeingTimer += deltaTime;
        if (blackboard.fleeingTimer > 0.3f) {
            blackboard.fleeing = false;
        }
    }
    else {
        blackboard.SetValue("getAway", 0);
        blackboard.fleeingTimer = 0;
    }

    if (blackboard.wallCollision) {
        blackboard.SetValue("wallCollision", 1);
        std::cout << "true" << std::endl;
    }
    else {
        blackboard.SetValue("wallCollision", 0);
    }
    root->execute(grid, blackboard, shape, players, projectiles);
}