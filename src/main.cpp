#include "first_app.h"
#include "ping_pong.h"
#include "snake.h"
#include "life_game.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {


    try {
        // Change the app class to be any of: "FirstApp", "PingPong", "Snake"
        LifeGame app{};
        app.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}