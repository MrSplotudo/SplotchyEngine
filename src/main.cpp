#include "first_app.h"
#include "pong_app.h"
#include "asteroids_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {


    try {
        AsteroidsApp app{};
        app.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}