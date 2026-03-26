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

    void Lighting::updateMuzzleFlash(float dt, glm::vec3 gunPosition, bool isFlashing) {
        if (isFlashing) {
            m_muzzleTimer = MUZZLE_DURATION;
            m_muzzlePosition = gunPosition;
        } else if (m_muzzleTimer > 0.0f) {
            m_muzzleTimer -= dt;
        }

        // Fade intensity from 0.8 to 0 over the duration
        if (m_muzzleTimer > 0.0f) {
            m_muzzleIntensity = (m_muzzleTimer / MUZZLE_DURATION) * 0.8f;
        } else {
            m_muzzleIntensity = 0.0f;
        }
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

    // Muzzle flash as a point light (simpler than spotlight)
    shader.setVec3("uMuzzleLight.position", m_muzzlePosition);
    shader.setVec3("uMuzzleLight.color", m_muzzleColor);
    shader.setFloat("uMuzzleLight.intensity", m_muzzleIntensity);
    shader.setFloat("uMuzzleLight.constant", 1.0f);
    shader.setFloat("uMuzzleLight.linear", 0.22f);
    shader.setFloat("uMuzzleLight.quadratic", 0.015f);
}
