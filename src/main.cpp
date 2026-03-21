#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include "engine/Input.hpp"
#include "engine/Camera.hpp"
#include "renderer/Shader.hpp"
#include "world/TileMap.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

int main() {
    try {
        Window  window(1280, 720, "ZombieHorde");
        Input::init(window.getHandle());

        TileMap map;

        Camera  camera(map.getPlayerSpawn());

        Shader shader("assets/shaders/world.vert",
                      "assets/shaders/world.frag");

        glEnable(GL_DEPTH_TEST);

        // collision radius — how close to a wall centre you can get
        constexpr float PLAYER_RADIUS = 0.25f;

        auto update = [&](float dt) {

            if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window.getHandle(), true);

            // Update camera first to read mouse delta before Input::update() resets it
            glm::vec3 oldPos = camera.getPosition();
            camera.update(dt);

            Input::update();
            glm::vec3 newPos = camera.getPosition();

            // simple AABB wall slide — check X and Z separately
            // so you slide along walls instead of stopping dead
            glm::vec3 testX(newPos.x, oldPos.y, oldPos.z);
            glm::vec3 testZ(oldPos.x, oldPos.y, newPos.z);

            if (map.isWall(testX.x + PLAYER_RADIUS, testX.z) ||
                map.isWall(testX.x - PLAYER_RADIUS, testX.z))
                newPos.x = oldPos.x;

            if (map.isWall(testZ.x, testZ.z + PLAYER_RADIUS) ||
                map.isWall(testZ.x, testZ.z - PLAYER_RADIUS))
                newPos.z = oldPos.z;

            camera.setPosition(newPos);

        };

        auto render = [&]() {
            shader.bind();
            shader.setMat4("uView",       camera.getViewMatrix());
            shader.setMat4("uProjection", camera.getProjectionMatrix());
            map.draw(shader);
        };

        GameLoop loop(window, update, render);
        loop.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
