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
#include <iostream>

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
            if (!moveBuffer1Used && direction.y == 0) {
                moveBuffer1 = {0, -1};
                moveBuffer1Used = true;
            } else if (!moveBuffer2Used && moveBuffer1Used && moveBuffer1.y == 0) {
                moveBuffer2 = {0, -1};
                moveBuffer2Used = true;
            }
            justPressedUp = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_RELEASE) {
            justPressedUp = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS && !justPressedDown) {
            if (!moveBuffer1Used && direction.y == 0) {
                moveBuffer1 = {0, 1};
                moveBuffer1Used = true;
            } else if (!moveBuffer2Used && moveBuffer1Used && moveBuffer1.y == 0) {
                moveBuffer2 = {0, 1};
                moveBuffer2Used = true;
            }
            justPressedDown = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_S) == GLFW_RELEASE) {
            justPressedDown = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS && !justPressedRight) {
            if (!moveBuffer1Used && direction.x == 0) {
                moveBuffer1 = {1, 0};
                moveBuffer1Used = true;
            } else if (!moveBuffer2Used && moveBuffer1Used && moveBuffer1.x == 0) {
                moveBuffer2 = {1, 0};
                moveBuffer2Used = true;
            }
            justPressedRight = true;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_D) == GLFW_RELEASE) {
            justPressedRight = false;
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS && !justPressedLeft) {
            if (!moveBuffer1Used && direction.x == 0) {
                moveBuffer1 = {-1, 0};
                moveBuffer1Used = true;
            } else if (!moveBuffer2Used && moveBuffer1Used && moveBuffer1.x == 0) {
                moveBuffer2 = {-1, 0};
                moveBuffer2Used = true;
            }
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

    // Initial snake head spawn
    seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{0.05f, 1.f, 0.f});
    snake.push_front({boardSize/ 2, boardSize/2});
    gameObj.model = seModel;
    gameObj.transform.translation = glm::vec3(boardSize / 2, boardSize / 2, -0.1f);
    gameObj.transform.scale = glm::vec3(.8f);
    gameObj.color = glm::vec3(1.f, 0.f, 0.f);
    snakeParts.push_back(std::move(gameObj));

    // Initial fruit spawn
    std::uniform_int_distribution fruitDist(0, boardSize - 1);

    fruit.push_back({fruitDist(gen), fruitDist(gen)});
    seModel = se::SeModel::createCircle(seDevice, cellSize / 2, glm::vec3(1.f, 0.f, 0.05f), 13);
    gameObj.model = seModel;
    gameObj.transform.scale = glm::vec3(.75);
    gameObj.transform.translation = glm::vec3(fruit[0].x * cellSize - boardWidth / 2,  fruit[0].y * cellSize - boardWidth / 2, -0.05f);
    fruitParts.push_back(std::move(gameObj));

    // Board grid
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
    if (moveBuffer1Used) {
        direction = moveBuffer1;
        if (!moveBuffer2Used) {
            moveBuffer1Used = false;
        }
        moveBuffer1 = moveBuffer2;
        moveBuffer2Used = false;
    }


    glm::ivec2 newHead = snake.front() + direction;

    // snake collision check
    if (newHead.x > boardSize - 1 || newHead.x < 0 || newHead.y > boardSize - 1 || newHead.y < 0 ||
        std::find(snake.begin(), snake.end(), newHead) != snake.end()) {
        vkDeviceWaitIdle(seDevice.device());
        snake.clear();
        snakeParts.clear();

        // New initial snake head
        auto gameObj = se::SeGameObject::createGameObject();
        seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{0.05f, 1.f, 0.f});
        snake.push_front({boardSize/ 2, boardSize/2});
        gameObj.model = seModel;
        gameObj.transform.translation = glm::vec3(snake.front().x, snake.front().y, -0.1f);
        gameObj.transform.scale = glm::vec3(.8f);
        gameObj.color = glm::vec3(1.f, 0.f, 0.f);
        snakeParts.push_back(std::move(gameObj));

        return;
    }

    snake.push_front(newHead);

    // Head on fruit check
    if (std::find(fruit.begin(), fruit.end(), newHead) != fruit.end()) {
        auto gameObj = se::SeGameObject::createGameObject();
        seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{0.05, 1.f - 1.f / (boardSize*boardSize) * snake.size(), 0.f});
        gameObj.model = seModel;
        gameObj.transform.scale = glm::vec3(.65);
        gameObj.transform.translation = glm::vec3(0.f, 0.f, -0.1f);
        snakeParts.push_back(std::move(gameObj));

        spawnFruit();
    } else {
        snake.pop_back();
    }

    // Sync deque to snake parts
    for (int i = 0; i < snake.size(); i++) {
        snakeParts[i].transform.translation.x = snake[i].x * cellSize - boardWidth / 2;
        snakeParts[i].transform.translation.y = snake[i].y * cellSize - boardWidth / 2;
    }
}

void Snake::spawnFruit() {

    std::uniform_int_distribution fruitDist(0, boardSize - 1);

    fruit.pop_back();

    glm::ivec2 newFruit = {};
    do {
        newFruit = {fruitDist(gen), fruitDist(gen)};
    } while (std::find(snake.begin(), snake.end(), newFruit) != snake.end());

    fruit.push_back(newFruit);

    fruitParts.pop_back();

    auto gameObj = se::SeGameObject::createGameObject();
    seModel = se::SeModel::createCircle(seDevice, cellSize / 2, glm::vec3(1.f, 0.f, 0.05f), 13);
    gameObj.model = seModel;
    gameObj.transform.scale = glm::vec3(.75);
    gameObj.transform.translation = glm::vec3(0.f, 0.f, -0.05f);
    fruitParts.push_back(std::move(gameObj));

    for (int i = 0; i < fruit.size(); i++) {
        fruitParts[i].transform.translation.x = fruit[i].x * cellSize - boardWidth / 2;
        fruitParts[i].transform.translation.y = fruit[i].y * cellSize - boardWidth / 2;
    }
}
