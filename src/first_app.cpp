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
        {{0.0f, -0.1f}, {1.0f, 0.0f, 0.0f}},
        {{0.1f, 0.1f}, {0.0f, 1.0f, 0.0f}},
        {{-0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}}
    };

    auto seModel = std::make_shared<se::SeModel>(seDevice, vertices);


    std::vector<glm::vec3> colors {
        {1.f, .7f, .73f},
        {1.f, .87f, .73f},
        {1.f, 1.f, .73f},
        {.73f, 1.f, .8f},
        {.73, .88f, 1.f}
    };

    for (auto& color : colors) {
        color = glm::pow(color, glm::vec3(2.2f));
    }

    for (int i = 0; i < 50; i++) {
        auto triangle = se::SeGameObject::createGameObject();

        triangle.model = seModel;
        triangle.transform2d.scale = glm::vec2(.5f) + i * 0.05f;
        triangle.transform2d.rotation = i * glm::pi<float>() * 0.025;
        triangle.color = colors[i % colors.size()];

        gameObjects.push_back(std::move(triangle));
    }

}