#include "first_app.h"

#include "../engine/se_camera.h"
#include "../engine/simple_render_system.h"
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

FirstApp::FirstApp() {
    loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
    se::SimpleRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};
    se::SeCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    glfwSetInputMode(seWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    auto viewerObject = se::SeGameObject::createGameObject();

    se::KeyboardMovementController cameraController{};

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        cameraController.moveInPlaneXZ(seWindow.getWindow(), dt, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        float aspect = seRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(seWindow.getWindow(), true);
        }

        // === UPDATE ===

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

void FirstApp::loadGameObjects() {
    std::shared_ptr<se::SeModel> seModel = se::SeModel::createModelFromFile(seDevice, "../assets/models/smooth_ball.obj");

    auto gameObj = se::SeGameObject::createGameObject();
    gameObj.model = seModel;
    gameObj.transform.translation = {-.5f, .0f, 2.5f};
    gameObj.transform.scale = {.5f, .5f, .5f};

    gameObjects.push_back(std::move(gameObj));

    seModel = se::SeModel::createModelFromFile(seDevice, "../assets/models/flat_ball.obj");

    gameObj = se::SeGameObject::createGameObject();
    gameObj.model = seModel;
    gameObj.transform.translation = {.5f, .0f, 2.5f};
    gameObj.transform.scale = {.5f, .5f, .5f};

    gameObjects.push_back(std::move(gameObj));
}