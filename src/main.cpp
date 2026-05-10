#include "first_app.h"
#include "ping_pong.h"
#include "snake.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {


    try {
        Snake app{};
        app.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}