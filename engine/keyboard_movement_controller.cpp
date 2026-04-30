#include "keyboard_movement_controller.h"
#include <iostream>

void se::KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, SeGameObject& gameObject) {
    double cursorPosX, cursorPosY;
    glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
    if (firstMouseInput) {
        lastCursorPosX = cursorPosX;
        lastCursorPosY = cursorPosY;
        firstMouseInput = false;
    }
    double cursorMovementX = cursorPosX - lastCursorPosX;
    double cursorMovementY = cursorPosY - lastCursorPosY;
    lastCursorPosX = cursorPosX;
    lastCursorPosY = cursorPosY;

    glm::vec3 rotate{0.f};
    rotate.x -= cursorMovementY;
    rotate.y += cursorMovementX;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.rotation += lookSpeed * rotate;
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBack) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }



}
