#pragma once
#include <GLFW/glfw3.h>
#include <string>

class FPSCounter {
public:
    FPSCounter() = default;

    // call once per frame, pass the window handle
    void update(GLFWwindow* window, float dt);

    float getFPS()  const { return m_fps; }
    float getMS()   const { return m_ms; }

private:
    float m_fps         = 0.0f;
    float m_ms          = 0.0f;
    float m_timer       = 0.0f;
    int   m_frameCount  = 0;
};
