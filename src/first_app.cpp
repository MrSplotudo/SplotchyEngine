#include "first_app.h"

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

FirstApp::FirstApp() {
    loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
    se::SimpleRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};

    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        float time = static_cast<float>(glfwGetTime());

        // === UPDATE ===

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

void FirstApp::loadGameObjects() {
    std::vector<se::SeModel::Vertex> vertices {
        {{-0.15f, -0.15f}, {1.0f, 0.0f, 0.0f}},
        {{0.15f, -0.15f}, {0.0f, 1.0f, 0.0f}},
        {{0.15f, 0.15f}, {0.0f, 0.0f, 1.0f}},
        {{-0.15f, -0.15f}, {1.0f, 0.0f, 0.0f}},
        {{0.15f, 0.15f}, {0.0f, 1.0f, 0.0f}},
        {{-0.15f, 0.15f}, {0.0f, 0.0f, 1.0f}}
    };

    auto seModel = std::make_shared<se::SeModel>(seDevice, vertices);

    for (int i = 0; i < 300; i++) {
        auto triangle = se::SeGameObject::createGameObject();

        triangle.model = seModel;
        triangle.transform2d.scale = glm::vec2(.5f) + i * 0.05f;

        triangle.color = glm::vec3(0.f + 0.004f * i, 0.f + 0.005f * i, 0.f + 0.005f * i);

        gameObjects.push_back(std::move(triangle));
    }

}