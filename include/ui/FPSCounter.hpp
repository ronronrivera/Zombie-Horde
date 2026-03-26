#pragma once
#include <GLFW/glfw3.h>
#include <string>

class FPSCounter {
public:
    void update(float dt);
    std::string getText() const; // returns "FPS: 144  MS: 6"

    float getFPS() const { return m_fps; }
    float getMS()  const { return m_ms;  }

private:
    float m_fps        = 0.0f;
    float m_ms         = 0.0f;
    float m_timer      = 0.0f;
    int   m_frameCount = 0;
};
