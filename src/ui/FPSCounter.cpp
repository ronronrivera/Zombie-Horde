#include "ui/FPSCounter.hpp"
#include <string>

void FPSCounter::update(GLFWwindow* window, float dt) {
    m_frameCount++;
    m_timer += dt;

    // update every 0.25 seconds so the number is readable
    if (m_timer >= 0.25f) {
        m_fps = (float)m_frameCount / m_timer;
        m_ms  = (m_timer / (float)m_frameCount) * 1000.0f;

        // show in window title — free, zero rendering cost
        std::string title = "ZombieHorde  |  "
                          + std::to_string((int)m_fps) + " FPS  "
                          + std::to_string((int)m_ms)  + " ms";
        glfwSetWindowTitle(window, title.c_str());

        m_timer      = 0.0f;
        m_frameCount = 0;
    }
}
