#include "engine/Window.hpp"
#include "engine/GameLoop.hpp"
#include "engine/Input.hpp"
#include "engine/Camera.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Texture.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    try {
        Window window(1280, 720, "ZombieHorde");
        Input::init(window.getHandle());
        Camera camera(glm::vec3(0.0f, 1.0f, 4.0f));

        // cube: 24 vertices (4 per face × 6 faces), each is x,y,z,u,v
        std::vector<float> verts = {
            // front
            -0.5f,-0.5f, 0.5f,  0,0,   0.5f,-0.5f, 0.5f,  1,0,
             0.5f, 0.5f, 0.5f,  1,1,  -0.5f, 0.5f, 0.5f,  0,1,
            // back
             0.5f,-0.5f,-0.5f,  0,0,  -0.5f,-0.5f,-0.5f,  1,0,
            -0.5f, 0.5f,-0.5f,  1,1,   0.5f, 0.5f,-0.5f,  0,1,
            // left
            -0.5f,-0.5f,-0.5f,  0,0,  -0.5f,-0.5f, 0.5f,  1,0,
            -0.5f, 0.5f, 0.5f,  1,1,  -0.5f, 0.5f,-0.5f,  0,1,
            // right
             0.5f,-0.5f, 0.5f,  0,0,   0.5f,-0.5f,-0.5f,  1,0,
             0.5f, 0.5f,-0.5f,  1,1,   0.5f, 0.5f, 0.5f,  0,1,
            // top
            -0.5f, 0.5f, 0.5f,  0,0,   0.5f, 0.5f, 0.5f,  1,0,
             0.5f, 0.5f,-0.5f,  1,1,  -0.5f, 0.5f,-0.5f,  0,1,
            // bottom
            -0.5f,-0.5f,-0.5f,  0,0,   0.5f,-0.5f,-0.5f,  1,0,
             0.5f,-0.5f, 0.5f,  1,1,  -0.5f,-0.5f, 0.5f,  0,1,
        };

        std::vector<unsigned int> idx;
        for (int f = 0; f < 6; f++) {
            unsigned int b = f * 4;
            idx.insert(idx.end(), {b,b+1,b+2, b+2,b+3,b});
        }

        Mesh    cube(verts, idx);
        Shader  shader("assets/shaders/world.vert",
                       "assets/shaders/world.frag");

        // drop any CC0 PNG into assets/textures/ and point to it here
        Texture texture("assets/textures/walls/me.jpg");

        glEnable(GL_DEPTH_TEST);

        auto update = [&](float dt) {
            if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
                glfwSetWindowShouldClose(window.getHandle(), true);

            camera.update(dt);
            Input::update();
        };

        auto render = [&]() {
            shader.bind();
            texture.bind(0);
            shader.setInt("uTexture", 0);
            shader.setMat4("uView",       camera.getViewMatrix());
            shader.setMat4("uProjection", camera.getProjectionMatrix());

            // model matrix — rotate the cube slowly so we can see all sides
            float angle = (float)glfwGetTime() * 30.0f;
            glm::mat4 model = glm::rotate(
                glm::mat4(1.0f),
                glm::radians(angle),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            shader.setMat4("uModel", model);

            cube.draw();
        };

        GameLoop loop(window, update, render);
        loop.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
