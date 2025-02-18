#ifndef BT_ENEMY_HPP
#define BT_ENEMY_HPP

#include "Entity.hpp"
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

enum class NodeState { SUCCESS, FAILURE, RUNNING };

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeState execute() = 0;
};


class Blackboard {
private:
    std::unordered_map<std::string, int> data;
public:
    void SetValue(const std::string& key, int value) {
        data[key] = value;
    }
    int GetValue(const std::string& key) {
        return data[key];
    }
};

class SequenceNode : public BTNode {
private:
    std::vector<std::unique_ptr<BTNode>> children;
public:
    void AddChild(std::unique_ptr<BTNode> child) {
        children.push_back(std::move(child));
    }
    NodeState execute() override {
        for (auto& child : children) {
            if (child->execute() == NodeState::FAILURE) {
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
    void AddChild(std::unique_ptr<BTNode> child) {
        children.push_back(std::move(child));
    }
    NodeState execute() override {
        for (auto& child : children) {
            if (child->execute() == NodeState::SUCCESS) {
                return NodeState::SUCCESS;
            }
        }
        return NodeState::FAILURE;
    }
};

//class ConditionNode : public BTNode {
//private:
//    Blackboard& blackboard;
//    std::string key;
//    int expectedValue;
//public:
//    ConditionNode(Blackboard& bb, const std::string& key, int value) : blackboard(bb), key(key), expectedValue(value) {}
//    NodeState execute() override {
//        return (blackboard.GetValue(key) == expectedValue) ? NodeState::SUCCESS : NodeState::FAILURE;
//    }
//};

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
    sf::Vector2f initialPos;
    float detectionRadius;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    float shootTimer = 0;
    float shootCd = 1;

	BTEnemy(float x, float y, int hp);

	void update(float deltaTime, Grid& grid, std::vector<Entity*> players, sf::Vector2f playerPos) override;

    void movement();
    bool detectPlayer(sf::Vector2f playerPos);
    void shoot(std::vector<Entity*> players);
    void getAway();
};


#endif
