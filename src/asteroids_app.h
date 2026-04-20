#pragma once

#include "../engine/se_window.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"
#include "../engine/se_renderer.h"


#include <memory>
#include <vector>

class AsteroidsApp {

public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 1000;

    AsteroidsApp();
    ~AsteroidsApp();

    AsteroidsApp(const AsteroidsApp &) = delete;
    AsteroidsApp& operator= (const AsteroidsApp &) = delete;

    void run();

private:
    void loadGameObjects();

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeRenderer seRenderer{seWindow, seDevice};
    std::vector<se::SeGameObject> gameObjects;
    std::vector<se::SeGameObject> bulletObjects;
    std::vector<se::SeGameObject> asteroidObjects;

};