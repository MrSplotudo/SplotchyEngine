#pragma once

#include "se_game_object.h"
#include "se_window.h"

#include <GLFW/glfw3.h>

namespace se {

class KeyboardMovementController {

public:
    struct KeyMappings {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBack = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_CONTROL;

    };

    void moveInPlaneXZ(GLFWwindow* window, float dt, SeGameObject& gameObject);


    KeyMappings keys {};
    float moveSpeed{3.f};
    float lookSpeed{.0013f};

private:
    bool firstMouseInput = true;
    double lastCursorPosX = 0;
    double lastCursorPosY = 0;
};

}