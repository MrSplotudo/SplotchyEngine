#include "life_game.h"

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

LifeGame::LifeGame() {
    loadGameObjects();
}

LifeGame::~LifeGame() {}

void LifeGame::run() {
    se::Simple2dRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};
    se::SeCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    glfwSetInputMode(seWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    auto viewerObject = se::SeGameObject::createGameObject();
    viewerObject.transform.translation = glm::vec3(0.f, 0.f, -1.f);

    auto lastTime = std::chrono::high_resolution_clock::now();

    // === MAIN LOOP ===

    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        float aspect = seRenderer.getAspectRatio();
        camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, 0.f, 10.f);

        double mouseX, mouseY;
        glfwGetCursorPos(seWindow.getWindow(), &mouseX, &mouseY);
        glm::vec2 cursorPos = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        cursorPos.x = (2.f * cursorPos.x / seWindow.getExtent().width - 1) * aspect;
        cursorPos.y = 2.f * cursorPos.y / seWindow.getExtent().height - 1;

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(seWindow.getWindow(), GLFW_TRUE);
        }

        if (glfwGetMouseButton(seWindow.getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            float offset = boardSize % 2 == 0 ? cellSize / 2.f : cellSize;
            glm::ivec2 cursorCell{};
            cursorCell.x = floor(cursorPos.x / cellSize + boardSize / 2 + offset);
            cursorCell.y = floor(cursorPos.y / cellSize + boardSize / 2 + offset);

            if (std::ranges::find(livingCells, cursorCell) == livingCells.end() &&
                cursorCell.x >= 0 && cursorCell.x < boardSize &&
                cursorCell.y >= 0 && cursorCell.y < boardSize) {
                createCell(cursorCell);
            }
        }

        if (glfwGetMouseButton(seWindow.getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            float offset = boardSize % 2 == 0 ? cellSize / 2.f : cellSize;
            glm::ivec2 cursorCell{};
            cursorCell.x = floor(cursorPos.x / cellSize + boardSize / 2 + offset);
            cursorCell.y = floor(cursorPos.y / cellSize + boardSize / 2 + offset);

            if (std::ranges::find(livingCells, cursorCell) != livingCells.end()) {
                removeCell(cursorCell);
            }
        }

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_1) == GLFW_PRESS) {
            tickInterval = 10000000.f;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_2) == GLFW_PRESS) {
            tickInterval = 1.f;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_3) == GLFW_PRESS) {
            tickInterval = .5f;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_4) == GLFW_PRESS) {
            tickInterval = .15f;
        } else if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_5) == GLFW_PRESS) {
            tickInterval = .05f;
        }

        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_E) == GLFW_PRESS && !justPressed) {
            step();
            justPressed = true;
        } else if (justPressed && glfwGetKey(seWindow.getWindow(), GLFW_KEY_E) == GLFW_RELEASE) {
            justPressed = false;
        }

        tickAccumulator += dt;
        if (tickAccumulator >= tickInterval) {
            tickAccumulator -= tickInterval;
            step();
        }

        // === RENDER ===
        if (auto commandBuffer = seRenderer.beginFrame()) {
            seRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
            simpleRenderSystem.renderGameObjects(commandBuffer, cellObjects, camera);
            seRenderer.endSwapChainRenderPass(commandBuffer);
            seRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(seDevice.device());
}

void LifeGame::loadGameObjects() {
    auto gameObj = se::SeGameObject::createGameObject();

    // Create board

    seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{.8f, .8f, .8f});
    for (int col = 0; col < boardSize; col++) {
        float colPos = col * cellSize - boardSize * cellSize / 2 + offset;
        for (int row = 0; row < boardSize; row++) {
            float rowPos = row * cellSize - boardSize * cellSize / 2 + offset;
            gameObj.model = seModel;
            gameObj.transform.scale = glm::vec3{.95f};
            gameObj.transform.translation = glm::vec3(colPos, rowPos, 0.f);
            gameObjects.push_back(std::move(gameObj));
        }
    }

}

void LifeGame::step() {
    std::vector<glm::ivec2> cellsToCreate{};
    std::vector<glm::ivec2> cellsToRemove{};

    for (int x = 0; x < boardSize; x++) {
        for (int y = 0; y < boardSize; y++) {
            glm::ivec2 cell{x, y};
            int neighborCount = 0;
            glm::ivec2 neighbor{};

            neighbor.x = cell.x - 1;
            neighbor.y = cell.y - 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x;
            neighbor.y = cell.y - 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x + 1;
            neighbor.y = cell.y - 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x - 1;
            neighbor.y = cell.y;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x + 1;
            neighbor.y = cell.y;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x - 1;
            neighbor.y = cell.y + 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x;
            neighbor.y = cell.y + 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            neighbor.x = cell.x + 1;
            neighbor.y = cell.y + 1;
            if (std::ranges::find(livingCells, neighbor) != livingCells.end()) neighborCount++;

            if (std::ranges::find(livingCells, cell) == livingCells.end()) {
                if (neighborCount == 3) {
                       cellsToCreate.push_back(cell);
                }
            } else {
                if (neighborCount < 2) {
                    cellsToRemove.push_back(cell);
                } else if (neighborCount > 3) {
                    cellsToRemove.push_back(cell);
                }
            }
        }
    }

    for (auto cell : cellsToCreate) {
        createCell(cell);
    }

    for (auto cell : cellsToRemove) {
        removeCell(cell);
    }
}

void LifeGame::createCell(glm::ivec2 cellPos) {
    livingCells.push_back(cellPos);

    auto cellObj = se::SeGameObject::createGameObject();
    seModel = se::SeModel::createQuad(seDevice, cellSize, glm::vec3{.1f, .8f, .1f});
    cellObj.model = seModel;
    cellObj.transform.scale = glm::vec3{.95f};
    cellObj.transform.translation.x = cellPos.x * cellSize - boardSize * cellSize / 2 + offset;
    cellObj.transform.translation.y = cellPos.y * cellSize - boardSize * cellSize / 2 + offset;
    cellObj.transform.translation.z = -.1f;

    cellObjects.push_back(std::move(cellObj));
}

void LifeGame::removeCell(glm::ivec2 cellPos) {
    auto iterator = std::ranges::find(livingCells, cellPos);
    if (iterator == livingCells.end()) return;
    size_t index = iterator - livingCells.begin();

    vkDeviceWaitIdle(seDevice.device());

    livingCells.erase(livingCells.begin() + index);
    cellObjects.erase(cellObjects.begin() + index);
}


