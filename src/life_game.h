#pragma once


#include "../engine/se_window.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"
#include "../engine/se_renderer.h"


#include <memory>
#include <vector>
#include <array>
#include <random>

class LifeGame {
public:
    static constexpr int WIDTH = 1600;
    static constexpr int HEIGHT = 1400;

    LifeGame();
    ~LifeGame();

    LifeGame(const LifeGame &) = delete;
    LifeGame& operator= (const LifeGame &) = delete;

    void run();

private:
    void loadGameObjects();

    void step();
    void createCell(glm::ivec2 cellPos);
    void removeCell(glm::ivec2 cellPos);

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeRenderer seRenderer{seWindow, seDevice};
    std::shared_ptr<se::SeModel> seModel{};
    std::vector<se::SeGameObject> gameObjects;
    std::vector<se::SeGameObject> cellObjects;
    std::vector<glm::ivec2> livingCells = {};

    static constexpr int boardSize = 40;
    float cellSize = .04f;
    float boardWidth = boardSize * cellSize - cellSize;
    float offset = boardSize % 2 == 0 ? cellSize / 2 : cellSize;

    float tickAccumulator = 0.f;
    float tickInterval = 100000000.f;

    bool justPressed = false;
};
