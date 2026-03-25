#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include "engine/Input.hpp"
#include "engine/Camera.hpp"
#include "renderer/Shader.hpp"
#include "world/TileMap.hpp"
#include "world/Lighting.hpp"
#include "game/ViewModel.hpp"
#include "ui/FPSCounter.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main() {
    try {
        Window      window(1280, 720, "ZombieHorde");
        Input::init(window.getHandle());
        TileMap     map;
        Camera      camera(map.getPlayerSpawn());
        Lighting    lighting;
        FPSCounter  fpsCounter;

        Shader      shader("assets/shaders/world.vert",
                           "assets/shaders/world.frag");
        Shader      skinnedShader("assets/shaders/skinned.vert",
                                  "assets/shaders/skinned.frag");
        Shader      emissiveShader("assets/shaders/eMissive.vert",
                                   "assets/shaders/eMissive.frag");

        ViewModel viewModel;

        glEnable(GL_DEPTH_TEST);

        constexpr float PLAYER_RADIUS = 0.25f;

        auto update = [&](float dt) {

            
            if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window.getHandle(), true);
            if (Input::isKeyPressed(GLFW_KEY_E))
                lighting.toggleEnabled();
            if (Input::isKeyPressed(GLFW_KEY_P))
                viewModel.toggleDetachFromCamera(camera);
            

            glm::vec3 oldPos = camera.getPosition();
            camera.update(dt);
            Input::update();
            glm::vec3 newPos = camera.getPosition();

            glm::vec3 testX(newPos.x, oldPos.y, oldPos.z);
            glm::vec3 testZ(oldPos.x, oldPos.y, newPos.z);

            if (map.isWall(testX.x + PLAYER_RADIUS, testX.z) ||
                map.isWall(testX.x - PLAYER_RADIUS, testX.z))
                newPos.x = oldPos.x;

            if (map.isWall(testZ.x, testZ.z + PLAYER_RADIUS) ||
                map.isWall(testZ.x, testZ.z - PLAYER_RADIUS))
                newPos.z = oldPos.z;

            camera.setPosition(newPos);

            bool sprinting = camera.isSprinting();
            bool moving    = camera.getCurrentSpeed() > 0.1f;
            bool triggerHeld = glfwGetMouseButton(window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            bool reloadPressed = Input::isKeyPressed(GLFW_KEY_R);

            viewModel.update(dt, triggerHeld, reloadPressed, moving, sprinting);

            // Hook for hit/raycast logic: this is true exactly once per bullet fired.
            if (viewModel.consumeShot()) {
                // TODO: add hitscan/raycast damage handling here.
            }
            

            lighting.update(camera);
            fpsCounter.update(window.getHandle(), dt);


        };

        auto render = [&]() {
            // ── pass 1: world ────────────────────────────────
            shader.bind();
            shader.setMat4("uView",       camera.getViewMatrix());
            shader.setMat4("uProjection", camera.getProjectionMatrix());
            shader.setVec3("uViewPos", camera.getPosition());
            lighting.apply(shader);
            map.draw(shader);

            // ── pass 2: viewmodel ────────────────────────────
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            skinnedShader.bind();
            skinnedShader.setMat4("uView",       camera.getViewMatrix());
            skinnedShader.setMat4("uProjection", camera.getProjectionMatrix());
            skinnedShader.setVec3("uViewPos", camera.getPosition());
            lighting.apply(skinnedShader);

            viewModel.draw(skinnedShader, emissiveShader, camera);
            glDisable(GL_BLEND);
        };

        GameLoop loop(window, update, render);
        loop.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
