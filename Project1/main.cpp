#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "PathFinding.hpp"
#include "Enemy.hpp"
#include "BT_Enemy.hpp"
#include "Grid.hpp"
#include "A_Ennemy.hpp"
#include <vector>
using namespace std;
using namespace sf;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jeu SFML - IA Ennemis");
    window.setFramerateLimit(60);

    Player player(400, 400, 10);
    sf::View view(player.shape.getPosition(), sf::Vector2f(640, 480));
    std::vector<Entity*> players;
    players.push_back(&player);
    std::vector<Entity*> enemies;
    Enemy enemy1(375,380,20);

    BTEnemy btEnemy(600.f,300.f,20);
    
    enemies.push_back(&btEnemy);
    enemies.push_back(&enemy1);
    A_Ennemy enemy2(500,100,20);
    enemies.push_back(&enemy2);
    
    Grid grid;
    grid.loadFromFile("map.txt");
    Pathfinding path;
    sf::Clock clock;
    vector<Vector2i> pathList;
    Vector2i start = { (int)enemy1.pos.x/CELL_SIZE,(int)enemy1.pos.y/CELL_SIZE };
    Vector2i target;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();
        
        target = { (int)player.pos.x / CELL_SIZE,(int)player.pos.y / CELL_SIZE };
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (int i = 0; i < enemies.size(); i++) {
            if (!enemies[i]->isAlive()) {
                enemies.erase(enemies.begin() + i);
            }
        }

        player.update(deltaTime, grid, enemies, player.pos);
        view.setCenter(player.shape.getPosition());
        window.setView(view);
        for (auto& enemy : enemies) {
            enemy->update(deltaTime, grid, players, player.pos);
            enemy->rayCasting(grid,window);
        }

        pathList = path.findPath(grid, start, target);
        enemy2.Path(start, target,path, grid);
        window.clear();
        grid.draw(window);
        window.draw(enemy1.circle);
        window.draw(player.shape);
        window.draw(player.atkRadius);
        for (const auto& enemy : enemies) {
            if (enemy->isAlive()) {
                window.draw(enemy->shape);
                
            }
        }
        for (auto& projectile : btEnemy.projectiles) {
            window.draw(projectile->shape);
        }
        window.display();
    }
    return 0;
}
