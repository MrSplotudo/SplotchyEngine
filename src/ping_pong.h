#pragma once

#include "../engine/se_window.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"
#include "../engine/se_renderer.h"


#include <memory>
#include <vector>

class PingPong {

public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 800;

    PingPong();
    ~PingPong();

    PingPong(const PingPong &) = delete;
    PingPong& operator= (const PingPong &) = delete;

    void run();

private:
    void loadGameObjects();

    float rightPaddleSpeed = 1.f;
    float leftPaddleSpeed = 1.f;
    glm::vec2 ballDirection = {1.f, .4f};
    float ballSpeed = 1.f;
    bool goingRight = false;
    bool goingUp = false;

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeRenderer seRenderer{seWindow, seDevice};
    std::vector<se::SeGameObject> gameObjects;
};