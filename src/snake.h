#pragma once


#include "../engine/se_window.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"
#include "../engine/se_renderer.h"


#include <memory>
#include <deque>
#include <vector>
#include <random>

class Snake {
public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 800;

    Snake();
    ~Snake();

    Snake(const Snake &) = delete;
    Snake& operator= (const Snake &) = delete;

    void run();

private:
    void loadGameObjects();

    void step();
    void spawnFruit();

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeRenderer seRenderer{seWindow, seDevice};
    std::shared_ptr<se::SeModel> seModel{};
    std::vector<se::SeGameObject> gameObjects;
    std::vector<se::SeGameObject> snakeParts;
    std::deque<glm::ivec2> snake;
    std::vector<se::SeGameObject> fruitParts;
    std::vector<glm::ivec2> fruit;
    glm::ivec2 direction{1, 0};

    std::random_device random;

    static constexpr int boardSize = 10;
    float cellSize = .1f;
    float boardWidth = boardSize * cellSize - cellSize;

    float tickAccumulator = 0.f;
    const float tickInterval = 0.15;

    bool justPressedUp = false;
    bool justPressedDown = false;
    bool justPressedLeft = false;
    bool justPressedRight = false;
};
