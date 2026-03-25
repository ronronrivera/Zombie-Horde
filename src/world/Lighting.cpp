#include "world/Lighting.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void Lighting::update(const Camera& camera) {
    // chest/body-cam offset: slightly below eye level and a touch forward
    m_position  = camera.getPosition()
                - glm::vec3(0.0f, 0.05f, 0.0f)
                + camera.getFront() * 0.06f;
    m_direction = camera.getFront();
}

void Lighting::apply(Shader& shader) const {
    const float effectiveIntensity = m_enabled ? m_intensity : 0.0f;

    shader.setVec3("uLight.position",  m_position);
    shader.setVec3("uLight.direction", m_direction);
    shader.setVec3("uLight.color",     m_color);
    shader.setFloat("uLight.intensity",   effectiveIntensity);
    shader.setFloat("uLight.cutoffInner",
                    std::cos(glm::radians(m_cutoffInner)));
    shader.setFloat("uLight.cutoffOuter",
                    std::cos(glm::radians(m_cutoffOuter)));
    shader.setFloat("uLight.constant",  m_constant);
    shader.setFloat("uLight.linear",    m_linear);
    shader.setFloat("uLight.quadratic", m_quadratic);
    shader.setVec3("uAmbient", m_ambient);
}
