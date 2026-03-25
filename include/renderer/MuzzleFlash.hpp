#pragma once
#include "renderer/Shader.hpp"
#include "renderer/Mesh.hpp"
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <memory>

class MuzzleFlash {
public:
    MuzzleFlash();

    void draw(Shader& shader, const glm::mat4& viewModelWorld,
              bool active, float dt) const;

private:
    std::unique_ptr<Mesh> m_mesh;

    // Offset from the view-model origin to the barrel muzzle.
    static constexpr float MUZZLE_OFFSET_X =  0.0f;
    static constexpr float MUZZLE_OFFSET_Y = -2.0f;
    static constexpr float MUZZLE_OFFSET_Z = 0.0f;
};

//0.0f, -0.26f, -0.005f
