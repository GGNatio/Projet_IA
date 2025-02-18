#ifndef BT_ENEMY_HPP
#define BT_ENEMY_HPP

#include "Entity.hpp"
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

class Blackboard {
private:
    std::unordered_map<std::string, int> data;
public:
    sf::Vector2f initialPos;
    Vector2f e_direction;
    float detectionRadius;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    float shootTimer = 0;
    float shootCd = 1;
    ConvexShape m_primaryCone;
    ConvexShape m_secondaryCone;
    void SetValue(const std::string& key, int value) {
        data[key] = value;
    }
    int GetValue(const std::string& key) {
        return data[key];
    }
};

enum class NodeState { SUCCESS, FAILURE, RUNNING };

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeState execute(Blackboard& blackboard, float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) = 0;
};

class SequenceNode : public BTNode {
private:
    std::vector<std::unique_ptr<BTNode>> children;
public:
    void AddChild(std::unique_ptr<BTNode> child) {
        children.push_back(std::move(child));
    }
    NodeState execute(Blackboard& blackboard, float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override {
        for (auto& child : children) {
            if (child->execute(blackboard, deltaTime, grid, players, playerPos) == NodeState::FAILURE) {
                return NodeState::FAILURE;
            }
        }
        return NodeState::SUCCESS;
    }
};

class SelectorNode : public BTNode {
private:
    std::vector<std::unique_ptr<BTNode>> children;
public:
    SelectorNode();

    void AddChild(std::unique_ptr<BTNode> child) {
        children.push_back(std::move(child));
    }
    NodeState execute(Blackboard& blackboard, float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override {
        for (auto& child : children) {
            if (child->execute(blackboard, deltaTime, grid, players, playerPos) == NodeState::SUCCESS) {
                return NodeState::SUCCESS;
            }
        }
        return NodeState::FAILURE;
    }
};

class ConditionNode : public BTNode {
private:
    Blackboard& blackboard;
    std::string key;
    int expectedValue;
public:
    ConditionNode(Blackboard& bb, const std::string& key, int value) : blackboard(bb), key(key), expectedValue(value) {}
    NodeState execute(Blackboard& blackboard, float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override {
        return (blackboard.GetValue(key) == expectedValue) ? NodeState::SUCCESS : NodeState::FAILURE;
    }
};

class ActionNode : public BTNode {
private:
    std::string actionName;
public:
    ActionNode(std::string name);
    NodeState execute(Blackboard& blackboard, float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override;
};


class Projectile {
public:
    bool state = true;
    sf::CircleShape shape;
    sf::Vector2f target = {0,0};
    sf::Vector2f direction;
    float distance = 0;

    Projectile(sf::Vector2f targ, sf::Vector2f originPos);

    void update(std::vector<Entity*> players, Grid& grid);
};

class BTEnemy : public Entity {
public:
    Blackboard blackboard;
    std::unique_ptr<SelectorNode> root = std::make_unique<SelectorNode>();
    sf::Vector2f initialPos;
    Vector2f e_direction;
    float detectionRadius;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    float shootTimer = 0;
    float shootCd = 1;
    ConvexShape m_primaryCone;
    ConvexShape m_secondaryCone;
	BTEnemy(float x, float y, int hp);
    
	void update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override;
    void rayCasting(Grid& grid, RenderWindow& window) override;
    void movement();
    bool detectPlayer(sf::Vector2f playerPos);
    void shoot(std::vector<Entity*> players);
    void getAway();
};


#endif
