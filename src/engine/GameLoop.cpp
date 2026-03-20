
#include "engine/GameLoop.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GameLoop::GameLoop(Window &window, UpdateFn update, RenderFn render)
: m_window(window), m_update(update), m_render(render) {}

void GameLoop::run(){
    double previous = glfwGetTime();
    double lag = 0.0;

    while(!m_window.shouldClose()){
        double current = glfwGetTime();
        double elapsed = current - previous;

        previous = current;
        lag += elapsed;

        m_window.pollEvents();

        while(lag>=FIXED_DT){
            m_update(FIXED_DT);
            lag-=FIXED_DT;
        }

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_render();
        
        m_window.swapBuffers();
    }
}

