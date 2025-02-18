#ifndef A_ENNEMY_HPP
#define A_ENNEMY_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "Grid.hpp"
#include "Pathfinding.hpp"
#include "Entity.hpp"
using namespace std;
using namespace sf;
class A_Ennemy : public Entity {
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

    void Path(Vector2i start, Vector2i end, Pathfinding& path, Grid& grid);
    bool detectPlayer(Vector2f playerPos);
    
    

    static constexpr float SPEED = 100.0f;
    A_Ennemy(float x, float y, int hp);
    void rayCasting(Grid& grid, RenderWindow& window) override;
    void update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override;

};

#endif // A_ENNEMY_HPP