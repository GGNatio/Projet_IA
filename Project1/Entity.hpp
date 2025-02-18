#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include "Grid.hpp"
using namespace sf;
class Entity {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    int health;
    Vector2f pos;

    Entity(float x, float y, sf::Color color, int hp);
    virtual void update(float deltaTime, Grid& grid, std::vector<Entity*> neededEntities, sf::Vector2f playerPos) = 0;
    bool isAlive() const;
    void takeDamage(int damage);
    virtual void rayCasting(Grid& grid, RenderWindow& window) = 0;
};


#endif // ENTITY_HPP