#pragma once

#include "../engine/se_window.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"
#include "../engine/se_renderer.h"


#include <memory>
#include <vector>

class FirstApp {

public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 800;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp& operator= (const FirstApp &) = delete;

    void run();

private:
    void loadGameObjects();

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeRenderer seRenderer{seWindow, seDevice};
    std::vector<se::SeGameObject> gameObjects;
};