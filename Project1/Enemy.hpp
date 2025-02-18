#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "Grid.hpp"
#include "Pathfinding.hpp"
#include "Entity.hpp"
using namespace std;
using namespace sf;
class Enemy : public Entity {
public:
    enum State { PATROL, CHASE, SEARCH, PATHFINDING };
    State currentState;
    bool reversePath;
    vector<Vector2i> currentPath;
    int currentPathIndex = 0;
    ConvexShape m_primaryCone;
    ConvexShape m_secondaryCone;
    float detectionRadius;
    //RectangleShape shape;
    bool isChasing = false;
    Clock searchDelay;
    Clock Delay;
    int searchX = 50;
    int searchY = 50;
    Vector2f searchPos;
    Vector2f e_direction;
    Vector2f lastPlayerPosition;
    Vector2i previousStart;
    Vector2i previousEnd;
    CircleShape circle;

    
    bool detectPlayer(Vector2f playerPos);
    void patrol(Grid& grid);
    void chase(sf::Vector2f playerPos);
    void search(sf::Vector2f lastPlayerPos, float deltaTime);

    static constexpr float SPEED = 2.0f;
    Enemy(float x, float y, int hp);
    void rayCasting(Grid& grid, RenderWindow& window) override;
    void update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override;
    
};

#endif // ENEMY_HPP