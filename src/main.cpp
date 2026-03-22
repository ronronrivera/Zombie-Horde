#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include "engine/Input.hpp"
#include "engine/Camera.hpp"
#include "renderer/Shader.hpp"
#include "world/TileMap.hpp"
#include "renderer/SkinnedMesh.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main() {
    try {
        constexpr const char* ANIM_DRAW = "Armature|Arms_FPS_Anim_Draw";
        constexpr const char* ANIM_IDLE = "Armature|Arms_FPS_Anim_Idle";
        constexpr const char* ANIM_WALK = "Armature|Arms_FPS_Anim_Walk";
        constexpr const char* ANIM_RUN  = "Armature|Arms_FPS_Anim_Run";
        constexpr const char* RELOAD = "Armature|Arms_FPS_Anim_Reload_Fast";

        Window      window(1280, 720, "ZombieHorde");
        Input::init(window.getHandle());
        TileMap     map;
        Camera      camera(map.getPlayerSpawn());

        Shader      shader("assets/shaders/world.vert",
                           "assets/shaders/world.frag");
        Shader      skinnedShader("assets/shaders/skinned.vert",
                                  "assets/shaders/skinned.frag");

        SkinnedMesh hands("assets/ViewModels/hand_rifle.glb");

        for (auto& name : hands.getAnimationNames())
            std::cout << "Animation: " << name << "\n";

        glEnable(GL_DEPTH_TEST);

        constexpr float PLAYER_RADIUS = 0.25f;

        hands.playAnimation(ANIM_DRAW);
        auto update = [&](float dt) {
            if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window.getHandle(), true);
            

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
            bool reloading = camera.isReloading();

            if      (sprinting && moving)   hands.playAnimation(ANIM_RUN);
            else if (moving)                hands.playAnimation(ANIM_WALK);
            else if(reloading)              hands.playAnimation(RELOAD);
            else                            hands.playAnimation(ANIM_IDLE);
            

            hands.update(dt);
        };

        auto render = [&]() {
            // ── pass 1: world ────────────────────────────────
            shader.bind();
            shader.setMat4("uView",       camera.getViewMatrix());
            shader.setMat4("uProjection", camera.getProjectionMatrix());
            map.draw(shader);

            // ── pass 2: viewmodel ────────────────────────────
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            skinnedShader.bind();
            skinnedShader.setMat4("uView",       camera.getViewMatrix());
            skinnedShader.setMat4("uProjection", camera.getProjectionMatrix());

            // FPS viewmodel placement:
            // - centered horizontally
            // - slightly lowered vertically
            // - pushed forward along camera Z axis
            // Model forward is assumed +X, so +90 deg yaw aligns it to camera forward (-Z).
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.26f, -0.005f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 1));
            model = glm::scale(model, glm::vec3(1.0f));

            // attach to camera
            glm::mat4 invView = glm::inverse(camera.getViewMatrix());
            skinnedShader.setMat4("uModel", invView * model);

            hands.draw(skinnedShader);
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
