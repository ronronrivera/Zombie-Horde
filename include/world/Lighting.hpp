#pragma once
#include "renderer/Shader.hpp"
#include "engine/Camera.hpp"
#include <glm/glm.hpp>

class Lighting {
public:
    Lighting() = default;

    // call every frame — flashlight follows camera
    void update(const Camera& camera);

    // upload all uniforms to shader before drawing the map
    void apply(Shader& shader) const;

    void toggleEnabled() { m_enabled = !m_enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    // flashlight position and direction (updated from camera)
    glm::vec3 m_position  {0.0f};
    glm::vec3 m_direction {0.0f, 0.0f, -1.0f};

    // flashlight properties
    glm::vec3 m_color     {1.0f, 0.95f, 0.85f}; // warm white torch feel
    float m_cutoffInner   = 12.5f; // degrees — bright center cone
    float m_cutoffOuter   = 40.0f; // degrees — soft falloff edge
    float m_intensity     = 2.0f;
    bool m_enabled        = true;

    // attenuation — how fast light fades with distance
    float m_constant      = 1.0f;
    float m_linear        = 0.14f;
    float m_quadratic     = 0.005f;

    // ambient — VERY dark for horror atmosphere
    glm::vec3 m_ambient   {0.01f, 0.01f, 0.015f}; // barely visible blue-black
};
