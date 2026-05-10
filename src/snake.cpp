#include "snake.h"

#include "../engine/se_camera.h"
#include "../engine/simple2d_render_system.h"
#include "../engine/keyboard_movement_controller.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std

#include <chrono>
#include <random>
#include <algorithm>

Snake::Snake() {
    loadGameObjects();
}

Snake::~Snake() {}

void Snake::run() {
    se::Simple2dRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};
    se::SeCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    glfwSetInputMode(seWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    auto viewerObject = se::SeGameObject::createGameObject();
    viewerObject.transform.translation = glm::vec3(0.f, 0.f, -1.f);

    auto lastTime = std::chrono::high_resolution_clock::now();



    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        float aspect = seRenderer.getAspectRatio();
        camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, 0.f, 10.f);

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(seWindow.getWindow(), GLFW_TRUE);
        }
        // === UPDATE ===

        tickAccumulator += dt;
        if (tickAccumulator >= tickInterval) {
            tickAccumulator -= tickInterval;
            step();
        }

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS && !justPressedUp) {
            direction = {0, -1};
            justPressedUp = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_RELEASE) {
            justPressedUp = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS && !justPressedDown) {
            direction = {0, 1};
            justPressedDown = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_RELEASE) {
            justPressedDown = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS && !justPressedRight) {
            direction = {1, 0};
            justPressedRight = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_D) == GLFW_RELEASE) {
            justPressedRight = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS && !justPressedLeft) {
            direction = {-1, 0};
            justPressedLeft = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_A) == GLFW_RELEASE) {
            justPressedLeft = false;
        }

        // === RENDER ===
        if (auto commandBuffer = seRenderer.beginFrame()) {
            seRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
            simpleRenderSystem.renderGameObjects(commandBuffer, snakeParts, camera);
            simpleRenderSystem.renderGameObjects(commandBuffer, fruitParts, camera);
            seRenderer.endSwapChainRenderPass(commandBuffer);
            seRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(seDevice.device());
}

void Snake::loadGameObjects() {
    auto gameObj = se::SeGameObject::createGameObject();
    seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{1.f, 0.f, 0.f});

    snake.push_front({boardSize/ 2, boardSize/2});
    gameObj.model = seModel;
    gameObj.transform.translation = glm::vec3(boardSize / 2, boardSize / 2, -0.1f);
    gameObj.transform.scale = glm::vec3(.9f);
    gameObj.color = glm::vec3(1.f, 0.f, 0.f);
    snakeParts.push_back(std::move(gameObj));

    fruit.push_back({boardSize/ 2, boardSize/2});
    seModel = se::SeModel::createCircle(seDevice, cellSize / 2, glm::vec3(0.f, 1.f, 0.f), 13);
    gameObj.model = seModel;
    gameObj.transform.scale = glm::vec3(.75);
    gameObj.transform.translation = glm::vec3(0.f, 0.f, -0.05f);
    fruitParts.push_back(std::move(gameObj));

    seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{0.f, 0.f, 1.f});
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            float x = i * cellSize - boardWidth / 2;
            float y = j * cellSize - boardWidth / 2;

            gameObj.model = seModel;
            gameObj.transform.translation = glm::vec3(x, y, 0.f);
            gameObj.transform.scale = glm::vec3(.95f);
            gameObjects.push_back(std::move(gameObj));
        }
    }
}

void Snake::step() {
    glm::ivec2 newHead = snake.front() + direction;
    snake.push_front(newHead);

    if (std::find(fruit.begin(), fruit.end(), newHead) != fruit.end()) {
        auto gameObj = se::SeGameObject::createGameObject();
        seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{1.f - 1.f / (boardSize*boardSize) * snake.size(), 0.f, 0.f});
        gameObj.model = seModel;
        gameObj.transform.scale = glm::vec3(.75);
        gameObj.transform.translation = glm::vec3(0.f, 0.f, -0.1f);
        snakeParts.push_back(std::move(gameObj));

        spawnFruit();
    } else {
        snake.pop_back();
    }

    for (int i = 0; i < snake.size(); i++) {
        snakeParts[i].transform.translation.x = snake[i].x * cellSize - boardWidth / 2;
        snakeParts[i].transform.translation.y = snake[i].y * cellSize - boardWidth / 2;
    }
}

void Snake::spawnFruit() {
    std::mt19937 gen(random());
    std::uniform_int_distribution<int> fruitDist(0, boardSize - 1);

    fruit.pop_back();

    glm::ivec2 newFruit = {};
    do {
        newFruit = {fruitDist(gen), fruitDist(gen)};
    } while (std::find(snake.begin(), snake.end(), newFruit) != snake.end());

    fruit.push_back(newFruit);

    fruitParts.pop_back();

    auto gameObj = se::SeGameObject::createGameObject();
    seModel = se::SeModel::createCircle(seDevice, cellSize / 2, glm::vec3(0.f, 1.f, 0.f), 13);
    gameObj.model = seModel;
    gameObj.transform.scale = glm::vec3(.75);
    gameObj.transform.translation = glm::vec3(0.f, 0.f, -0.05f);
    fruitParts.push_back(std::move(gameObj));

    for (int i = 0; i < fruit.size(); i++) {
        fruitParts[i].transform.translation.x = fruit[i].x * cellSize - boardWidth / 2;
        fruitParts[i].transform.translation.y = fruit[i].y * cellSize - boardWidth / 2;
    }
}
