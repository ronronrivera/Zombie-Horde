#include "ui/FPSCounter.hpp"
#include <sstream>
#include <iomanip>

void FPSCounter::update(float dt) {
    m_frameCount++;
    m_timer += dt;

    if (m_timer >= 0.2f) {
        m_fps        = (float)m_frameCount / m_timer;
        m_ms         = (m_timer / (float)m_frameCount) * 1000.0f;
        m_timer      = 0.0f;
        m_frameCount = 0;
    }
}

std::string FPSCounter::getText() const {
    std::ostringstream ss;
    ss << "FPS: " << (int)m_fps
       << "  MS: " << std::fixed << std::setprecision(1) << m_ms;
    return ss.str();
}
