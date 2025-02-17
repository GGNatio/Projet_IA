#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"


class Player : public Entity {
public:
    static constexpr float SPEED = 200.0f;
    static constexpr int DAMAGE = 2;
    static constexpr float ATTACK_COOLDOWN = 0.5f;
    float attackTimer;
    sf::CircleShape atkRadius;

    Player(float x, float y, int hp);
    void attack(std::vector<Entity*> enemies);
    void update(float deltaTime, Grid& grid, std::vector<Entity*> enemies, sf::Vector2f playerPos) override;
};

#endif // PLAYER_HPP