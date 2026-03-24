#include "first_app.h"

void FirstApp::run() {
    while (!seWindow.shouldClose()) {
        glfwPollEvents();
    }
}


