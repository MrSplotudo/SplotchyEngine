#include "first_app.h"
#include "ping_pong.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {


    try {
        PingPong app{};
        app.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}