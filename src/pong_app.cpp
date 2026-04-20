#include "pong_app.h"

#include "../engine/simple_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <iostream>
#include <array>

PongApp::PongApp() {
    loadGameObjects();
}

PongApp::~PongApp() {}

void PongApp::run() {
    se::SimpleRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};
    bool travelingRight = true;
    bool travelingUp = true;
    float lastTime = 0.0f;

    while (!seWindow.shouldClose()) {
        float time = static_cast<float>(glfwGetTime());
        float dt = time - lastTime;
        lastTime = time;

        glfwPollEvents();

        // === UPDATE ===
        float speed = 1.f * dt;
        auto& leftPlayer{gameObjects[0]};
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            leftPlayer.transform2d.translation.y -= speed;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            leftPlayer.transform2d.translation.y += speed;
        }
        auto& rightPlayer{gameObjects[1]};
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            rightPlayer.transform2d.translation.y -= speed;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            rightPlayer.transform2d.translation.y += speed;
        }


        auto& ball{gameObjects[2]};
        if (ball.transform2d.translation.x + 0.04f >= rightPlayer.transform2d.translation.x - 0.01f &&
            ball.transform2d.translation.y + 0.04 >= rightPlayer.transform2d.translation.y - 0.15f &&
            ball.transform2d.translation.y - 0.04 <= rightPlayer.transform2d.translation.y + 0.15f) {
            travelingRight = false;
        }

        if (ball.transform2d.translation.x - 0.04f <= leftPlayer.transform2d.translation.x + 0.01f &&
            ball.transform2d.translation.y + 0.04 >= leftPlayer.transform2d.translation.y - 0.15f &&
            ball.transform2d.translation.y - 0.04 <= leftPlayer.transform2d.translation.y + 0.15f) {
            travelingRight = true;
            }
        if (ball.transform2d.translation.y - 0.04f <= -1.0f) {
            travelingUp = false;
        } else if (ball.transform2d.translation.y + 0.04f >= 1.0f) {
            travelingUp = true;
        }

        if (travelingRight) {
            ball.transform2d.translation.x += speed;
        } else {
            ball.transform2d.translation.x -= speed;
        }
        if (travelingUp) {
            ball.transform2d.translation.y -= speed;
        } else {
            ball.transform2d.translation.y += speed;
        }



        // === RENDER ===
        if (auto commandBuffer = seRenderer.beginFrame()) {
            seRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            seRenderer.endSwapChainRenderPass(commandBuffer);
            seRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(seDevice.device());
}

void PongApp::loadGameObjects() {
    std::vector<se::SeModel::Vertex> vertices {
        {{-0.15f, -0.15f}, {1.0f, 0.0f, 0.0f}},
        {{0.15f, -0.15f}, {0.0f, 1.0f, 0.0f}},
        {{0.15f, 0.15f}, {0.0f, 0.0f, 1.0f}},
        {{-0.15f, -0.15f}, {1.0f, 0.0f, 0.0f}},
        {{0.15f, 0.15f}, {0.0f, 1.0f, 0.0f}},
        {{-0.15f, 0.15f}, {0.0f, 0.0f, 1.0f}}
    };

    auto seModel = std::make_shared<se::SeModel>(seDevice, vertices);

    auto leftPlayer = se:: SeGameObject::createGameObject();
    leftPlayer.model = seModel;
    leftPlayer.color = {0.9f, 0.2f, 0.2f};
    leftPlayer.transform2d.translation = {-0.8f, 0.0f};
    leftPlayer.transform2d.scale = {0.05f, 1.0f};
    gameObjects.push_back(std::move(leftPlayer));

    auto rightPlayer = se:: SeGameObject::createGameObject();
    rightPlayer.model = seModel;
    rightPlayer.color = {0.2f, 0.2f, 0.9f};
    rightPlayer.transform2d.translation = {0.8f, 0.0f};
    rightPlayer.transform2d.scale = {0.05f, 1.0f};
    gameObjects.push_back(std::move(rightPlayer));

    auto ball = se:: SeGameObject::createGameObject();
    ball.model = seModel;
    ball.transform2d.scale = {0.2f, 0.2f};
    gameObjects.push_back(std::move(ball));
}