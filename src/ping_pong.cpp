#include "ping_pong.h"

#include "../engine/se_camera.h"
#include "../engine/simple2d_render_system.h"
#include "../engine/keyboard_movement_controller.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <cassert>
#include <iostream>
#include <stdexcept>

PingPong::PingPong() {
    loadGameObjects();
}

PingPong::~PingPong() {}

void PingPong::run() {
    se::Simple2dRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};
    se::SeCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    glfwSetInputMode(seWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    auto viewerObject = se::SeGameObject::createGameObject();

    auto lastTime = std::chrono::high_resolution_clock::now();

    auto rightPaddle = &gameObjects[0];
    auto leftPaddle = &gameObjects[1];
    auto ball = &gameObjects[2];

    float paddleHalfX = 0.1 * rightPaddle->transform.scale.x;
    float paddleHalfY = 0.1 * leftPaddle->transform.scale.y;
    float ballHalfX = 0.1 * ball->transform.scale.x;

    if (ballDirection.x > 0.f) {
        goingRight = true;
    } else if (ballDirection.x < 0.f) {
        goingRight = false;
    }
    if (ballDirection.y < 0.f) {
        goingUp = true;
    } else if (ballDirection.y > 0.f) {
        goingUp = false;
    }


    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        float aspect = seRenderer.getAspectRatio();
        //camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);
        camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, 0.f, 10.f);

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(seWindow.getWindow(), true);
        }

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            rightPaddle->transform.translation.y -= rightPaddleSpeed * dt;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            rightPaddle->transform.translation.y += rightPaddleSpeed * dt;
        }

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            leftPaddle->transform.translation.y -= leftPaddleSpeed * dt;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            leftPaddle->transform.translation.y += leftPaddleSpeed * dt;
        }

        // === UPDATE ===

        glm::vec2 direction = {};

        if (ballDirection.x != 0.f) {
            direction = glm::normalize(ballDirection);
        } else {
            direction = glm::vec2(0.f, 0.f);
        }

        glm::vec2 velocity = direction * ballSpeed;
        ball->transform.translation.x += velocity.x * dt;
        ball->transform.translation.y += velocity.y * dt;

        if (ball->transform.translation.x - ballHalfX < rightPaddle->transform.translation.x + paddleHalfX &&
            ball->transform.translation.x + ballHalfX > rightPaddle->transform.translation.x - paddleHalfX &&
            ball->transform.translation.y - ballHalfX < rightPaddle->transform.translation.y + paddleHalfY &&
            ball->transform.translation.y + ballHalfX > rightPaddle->transform.translation.y - paddleHalfY &&
            goingRight == true)
            {
            goingRight = false;
            ballDirection.x = -ballDirection.x;
            ballSpeed *= 1.05f;
            ballDirection.y = 3 * (ball->transform.translation.y - rightPaddle->transform.translation.y);
            ballDirection.y < 0.f ? goingUp = true : goingUp = false;
        }

        if (ball->transform.translation.x - ballHalfX < leftPaddle->transform.translation.x + paddleHalfX &&
            ball->transform.translation.x + ballHalfX > leftPaddle->transform.translation.x - paddleHalfX &&
            ball->transform.translation.y - ballHalfX < leftPaddle->transform.translation.y + paddleHalfY &&
            ball->transform.translation.y + ballHalfX > leftPaddle->transform.translation.y - paddleHalfY &&
            goingRight == false)
            {
            goingRight = true;
            ballDirection.x = -ballDirection.x;
            ballSpeed *= 1.05f;
            ballDirection.y = 3 * (ball->transform.translation.y - leftPaddle->transform.translation.y);
            ballDirection.y < 0.f ? goingUp = true : goingUp = false;
        }

        if (ball->transform.translation.y - ballHalfX < -1.f && goingUp == true) {
            goingUp = false;
            ballDirection.y = -ballDirection.y;
        }
        if (ball->transform.translation.y + ballHalfX > 1.f && goingUp == false) {
            goingUp = true;
            ballDirection.y = -ballDirection.y;
        }

        if (ball->transform.translation.x > 1.1f || ball->transform.translation.x < - 1.1f) {
            ball->transform.translation.x = 0.f;
            ball->transform.translation.y = 0.f;
            ballSpeed = 1.f;
        }



        // === RENDER ===
        if (auto commandBuffer = seRenderer.beginFrame()) {
            seRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
            seRenderer.endSwapChainRenderPass(commandBuffer);
            seRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(seDevice.device());
}

void PingPong::loadGameObjects() {
    std::shared_ptr<se::SeModel> seModel{};
    auto gameObj = se::SeGameObject::createGameObject();

    // Blue/Right paddle
    seModel = se::SeModel::createQuad(seDevice, .2f, glm::vec3{0.f, 0.f, 1.f});
    gameObj.model = seModel;
    gameObj.transform.translation = glm::vec3(.85f, 0.f, 0.f);
    gameObj.transform.scale = glm::vec3(0.2f, 2.f, 1.f);
    gameObjects.push_back(std::move(gameObj));

    // Red/Left paddle
    seModel = se::SeModel::createQuad(seDevice, .2f, glm::vec3(1.f, 0.f, 0.f));
    gameObj.model = seModel;
    gameObj.transform.translation = glm::vec3(-.85f, 0.f, 0.f);
    gameObj.transform.scale = glm::vec3(0.2f, 2.f, 1.f);
    gameObjects.push_back(std::move(gameObj));

    // Ball
    seModel = se::SeModel::createCircle(seDevice, .1f, glm::vec3{1.f, 0.f, 1.f}, 9);
    gameObj.model = seModel;
    gameObj.transform.translation = glm::vec3(0.f);
    gameObj.transform.scale = glm::vec3(.2f);
    gameObjects.push_back(std::move(gameObj));
}