#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include "Grid.hpp"
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
using namespace sf;

class Blackboard {
private:
    std::unordered_map<std::string, int> data;
public:
    sf::Vector2f target;
    sf::RectangleShape shape;
    sf::Vector2f pos;
    sf::Vector2f initialPos;
    bool fleeing = false;
    float fleeingTimer = 0;
    void SetValue(const std::string& key, int value) {
        data[key] = value;
    }
    int GetValue(const std::string& key) {
        return data[key];
    }
};

class Entity {
public:
    Blackboard blackboard;
    RectangleShape shape;
    Vector2f velocity;
    sf::Vector2f initialPos;
    
    Texture textSprite;
    int health;
    Vector2f pos;

    Entity(float x, float y, sf::Color color, int hp);
    virtual void update(float deltaTime, Grid& grid, std::vector<Entity*> neededEntities, sf::Vector2f playerPos) = 0;
    bool isAlive() const;
    void takeDamage(int damage);
    virtual void rayCasting(Grid& grid, RenderWindow& window) = 0;
};


#endif // ENTITY_HPP