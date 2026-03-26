#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include "engine/Input.hpp"
#include "engine/Camera.hpp"
#include "engine/AudioManager.hpp"
#include "engine/SoundPlayer.hpp"
#include "renderer/Shader.hpp"
#include "ui/TextRenderer.hpp"
#include "world/TileMap.hpp"
#include "world/Lighting.hpp"
#include "game/ViewModel.hpp"
#include "ui/FPSCounter.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

int main() {
    try {
        Window      window(1280, 720, "ZombieHorde");
        Input::init(window.getHandle());
        TileMap     map;
        Camera      camera(map.getPlayerSpawn());
        AudioManager audio;
        SoundPlayer  soundPlayer(audio);
        Lighting    lighting;
        FPSCounter  fpsCounter;

        audio.loadSound("shoot", "assets/sounds/gun_shot.wav");
        audio.loadSound("reload", "assets/sounds/gun_reload.wav");
        audio.loadSound("draw", "assets/sounds/gun_draw.wav");
        audio.loadSound("footstep", "assets/sounds/walk_sound.wav");
        audio.loadSound("footstep_run", "assets/sounds/walk_sound.wav");

        Shader      shader("assets/shaders/world.vert",
                           "assets/shaders/world.frag");
        Shader      skinnedShader("assets/shaders/skinned.vert",
                                  "assets/shaders/skinned.frag");
        Shader      emissiveShader("assets/shaders/eMissive.vert",
                                   "assets/shaders/eMissive.frag");
        ViewModel viewModel;
    
        

        TextRenderer textRenderer("assets/ttf/JetBrainsMono-Bold.ttf", 20);
        Shader     hudShader("assets/shaders/hud.vert",
                             "assets/shaders/hud.frag");
        glEnable(GL_DEPTH_TEST);

        constexpr float PLAYER_RADIUS = 0.25f;

        auto update = [&](float dt) {

            
            if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window.getHandle(), true);
            if (Input::isKeyPressed(GLFW_KEY_E))
                lighting.toggleEnabled();
            if (Input::isKeyPressed(GLFW_KEY_P))
                viewModel.toggleDetachFromCamera(camera);
            static bool vsynOn = false;
            if (Input::isKeyPressed(GLFW_KEY_V)) {
                vsynOn = !vsynOn;
                window.setVSync(vsynOn ? 1 : 0);
                std::cout << "V-Sync: " << (vsynOn ? "ON" : "OFF") << "\n";
            }
            

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

            bool moving    = camera.getCurrentSpeed() > 0.1f;
            bool sprinting = camera.isSprinting();
            bool triggerHeld = glfwGetMouseButton(window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            bool reloadPressed = Input::isKeyPressed(GLFW_KEY_R);

            viewModel.update(dt, triggerHeld, reloadPressed, moving, sprinting);

            bool shotFired = viewModel.consumeShot();
            bool justDrawStarted = viewModel.consumeDrawStart();
            static bool wasReloading = false;
            bool isReloadingNow = viewModel.getWeapon().getState() == WeaponState::Reloading;
            bool justReloadStarted = isReloadingNow && !wasReloading;
            wasReloading = isReloadingNow;

            soundPlayer.update(dt, camera.getCurrentSpeed(), sprinting, shotFired, justDrawStarted, justReloadStarted);

            audio.updateListener(camera.getPosition(), camera.getFront(), camera.returnM_up());

            // Update muzzle flash light — position it at the gun barrel
            // Gun is positioned offset from camera: forward and slightly down
            glm::vec3 gunBarrelPos = camera.getPosition() 
                                    + camera.getFront() * 0.1f 
                                    - glm::vec3(0.0f, 0.15f, 0.0f);
            lighting.updateMuzzleFlash(dt, gunBarrelPos, viewModel.getWeapon().isMuzzleFlash());

            // Hook for hit/raycast logic: this is true exactly once per bullet fired.
            if (shotFired) {
                // TODO: add hitscan/raycast damage handling here.
            }
            

            lighting.update(camera);
            fpsCounter.update(dt);

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
            if (!viewModel.isDetachedFromCamera()) {
                glClear(GL_DEPTH_BUFFER_BIT);
            }
            glDisable(GL_BLEND);

            skinnedShader.bind();
            skinnedShader.setMat4("uView",       camera.getViewMatrix());
            skinnedShader.setMat4("uProjection", camera.getProjectionMatrix());
            skinnedShader.setVec3("uViewPos", camera.getPosition());
            lighting.apply(skinnedShader);

            viewModel.draw(skinnedShader, emissiveShader, camera);

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            textRenderer.drawText(
                hudShader,
                fpsCounter.getText(),
                10.0f, 10.0f,
                window.getWidth(), window.getHeight(),
                glm::vec3(0.0f, 1.0f, 0.3f)
            );
            std::string ammoText = std::to_string(viewModel.getWeapon().getAmmo()) 
                                   + " / " 
                                   + std::to_string(viewModel.getWeapon().getMagSize());
            textRenderer.drawText(
                hudShader,
                ammoText,
                (float)window.getWidth() - 120.0f,
                (float)window.getHeight() - 30.0f,
                window.getWidth(), window.getHeight(),
                glm::vec3(1.0f, 1.0f, 1.0f)
            );
            glEnable(GL_DEPTH_TEST);
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
