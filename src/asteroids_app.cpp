#include "asteroids_app.h"

#include "../engine/simple_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <iostream>
#include <random>

AsteroidsApp::AsteroidsApp() {
    loadGameObjects();
}

AsteroidsApp::~AsteroidsApp() {}

void AsteroidsApp::run() {
    se::SimpleRenderSystem simpleRenderSystem{seDevice, seRenderer.getSwapChainRenderPass()};

    float forwardSpeed = 1.0f;
    float rotationSpeed = 5.0f;
    float bulletSpeed = 2.0f;

    float timeSinceFired = 0.0f;
    float timeSinceAsteroid = 0.0f;

    float lastTime = 0.0f;

    std::random_device random;
    std::mt19937 gen(random());
    std::uniform_real_distribution<float> spawnNumber(0.0f, 100.0f);
    std::uniform_real_distribution<float> posRange(-1.0f, 1.0f);
    std::uniform_real_distribution<float> dirRange(0.0f, glm::two_pi<float>());

    std::vector<se::SeModel::Vertex> bulletVertices {
                {{0.f, -0.1f}, {0.0f, 0.0f, 0.0f}},
                {{0.1f, 0.1f}, {0.0f, 0.0f, 0.0f}},
                {{-0.1f, 0.1f}, {0.0f, 0.0f, 0.0f}}
    };
    auto seModel = std::make_shared<se::SeModel>(seDevice, bulletVertices);


    std::vector<se::SeModel::Vertex> asteroidVertices {};
    for (int i = 0; i < 6; i++) {
        asteroidVertices.push_back({{0.0f, 0.0f}});
        asteroidVertices.push_back({{0.1f * glm::sin(glm::two_pi<float>() / 6.0f * i), 0.1f * -glm::cos(glm::two_pi<float>() / 6.0f * i)}});
        asteroidVertices.push_back({{0.1f * glm::sin(glm::two_pi<float>() / 6.0f * (i + 1)), 0.1f * -glm::cos(glm::two_pi<float>() / 6.0f * (i + 1))}});
    }

    std::vector<float> asteroidSpeeds {};


    auto asteroidModel = std::make_shared<se::SeModel>(seDevice, asteroidVertices);

    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        float time = static_cast<float>(glfwGetTime());
        float dt = time - lastTime;
        lastTime = time;

        // === UPDATE ===

        // === SHIP LOGIC ===
        auto& ship = gameObjects[0];
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            ship.transform2d.rotation = glm::mod(ship.transform2d.rotation + rotationSpeed * dt, glm::two_pi<float>());
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            ship.transform2d.rotation = glm::mod(ship.transform2d.rotation - rotationSpeed * dt, glm::two_pi<float>());
        }
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            float dirX = glm::sin(ship.transform2d.rotation);
            float dirY = -glm::cos(ship.transform2d.rotation);
            ship.transform2d.translation.x += forwardSpeed * dirX * dt;
            ship.transform2d.translation.y += forwardSpeed * dirY * dt;
        }

        if (ship.transform2d.translation.x > 1.2f || ship.transform2d.translation.x < -1.2f) {
            ship.transform2d.translation.x *= -0.99f;
        }
        if (ship.transform2d.translation.y < -1.2f || ship.transform2d.translation.y > 1.2f) {
            ship.transform2d.translation.y *= -0.99f;
        }

        // === BULLET LOGIC ===
        timeSinceFired += dt;
        if (glfwGetKey(seWindow.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && timeSinceFired > 0.4f) {
            timeSinceFired = 0.0f;

            auto bullet = se::SeGameObject::createGameObject();
            bullet.model = seModel;
            bullet.transform2d.scale = {0.12f, 0.2f};
            bullet.transform2d.rotation = ship.transform2d.rotation;
            bullet.transform2d.translation = ship.transform2d.translation;
            bullet.color = {0.8f, 0.2f, 0.2f};
            bulletObjects.push_back(std::move(bullet));
        }
        for (auto& bullet : bulletObjects) {
            bullet.transform2d.translation.x += bulletSpeed * sin(bullet.transform2d.rotation) * dt;
            bullet.transform2d.translation.y += bulletSpeed * -cos(bullet.transform2d.rotation) * dt;
        }

        // === ASTEROID LOGIC ===
        timeSinceAsteroid += dt;
        if (timeSinceAsteroid > 1.0f) {
            timeSinceAsteroid -= 1.0f;

            if (spawnNumber(gen) > 50.0f) {
                auto asteroid = se::SeGameObject::createGameObject();
                asteroid.model = asteroidModel;
                asteroid.transform2d.translation.x = posRange(gen);
                asteroid.transform2d.translation.y = posRange(gen);
                asteroid.transform2d.rotation = dirRange(gen);
                asteroidObjects.push_back(std::move(asteroid));
            }
        }

        bool destroyAsteroid = false;
        int i = 0;
        for (auto& asteroid : asteroidObjects) {
            int j = 0;

            asteroid.transform2d.translation.x += sin(asteroid.transform2d.rotation) * dt;
            asteroid.transform2d.translation.y += -cos(asteroid.transform2d.rotation) * dt;

            if (asteroid.transform2d.translation.x > 1.2f || asteroid.transform2d.translation.x < -1.2f) {
                asteroid.transform2d.translation.x *= -0.99f;
            }
            if (asteroid.transform2d.translation.y < -1.2f || asteroid.transform2d.translation.y > 1.2f) {
                asteroid.transform2d.translation.y *= -0.99f;
            }



            for (auto& bullet : bulletObjects) {
                float disX = glm::abs(asteroid.transform2d.translation.x - bullet.transform2d.translation.x);
                float disY = glm::abs(asteroid.transform2d.translation.y - bullet.transform2d.translation.y);
                float distance = glm::sqrt(disX * disX + disY * disY);

                if (distance < 0.1f) {
                    destroyAsteroid = true;
                    bulletObjects.erase(bulletObjects.begin() + j);
                    break;
                } else {
                    j++;
                }
            }
            if (destroyAsteroid) {
                asteroidObjects.erase(asteroidObjects.begin() + i);
                destroyAsteroid = false;
            } else {
                i++;
            }
        }

        // === RENDER ===
        if (auto commandBuffer = seRenderer.beginFrame()) {
            seRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            simpleRenderSystem.renderGameObjects(commandBuffer, bulletObjects);
            simpleRenderSystem.renderGameObjects(commandBuffer, asteroidObjects);
            seRenderer.endSwapChainRenderPass(commandBuffer);
            seRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(seDevice.device());
}

void AsteroidsApp::loadGameObjects() {
    std::vector<se::SeModel::Vertex> vertices {
            {{0.f, -0.15f}, {0.0f, 0.0f, 0.0f}},
            {{0.1f, 0.15f}, {0.0f, 0.0f, 0.0f}},
            {{-0.1f, 0.15f}, {0.0f, 0.0f, 0.0f}}
    };

    auto seModel = std::make_shared<se::SeModel>(seDevice, vertices);

    auto ship = se::SeGameObject::createGameObject();
    ship.model = seModel;
    ship.color = {0.1f, 0.1f, 0.5f};
    ship.transform2d.scale = {0.4, 0.5};
    gameObjects.push_back(std::move(ship));

}