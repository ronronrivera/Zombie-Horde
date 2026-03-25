#include "renderer/MuzzleFlash.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

MuzzleFlash::MuzzleFlash() {
    // Oversized debug quad so muzzle position is obvious while tuning offsets.
    std::vector<float> verts = {
        -0.10f,-0.10f, 0.0f,  0,0,1,  0,0,
         0.10f,-0.10f, 0.0f,  0,0,1,  1,0,
         0.10f, 0.10f, 0.0f,  0,0,1,  1,1,
        -0.10f, 0.10f, 0.0f,  0,0,1,  0,1,
    };
    std::vector<unsigned int> idx = {0,1,2,2,3,0};
    m_mesh = std::make_unique<Mesh>(verts, idx);
}

void MuzzleFlash::draw(Shader& shader, const glm::mat4& viewModelWorld,
                       bool active, float dt) const
{
    if (!active) return;

    // random slight rotation each flash so it doesn't look static
    static float rot = 0.0f;
    rot += dt * 120.0f;

    // Parent flash to the view-model transform so it follows the weapon exactly.
    glm::mat4 model = viewModelWorld;
    model = glm::translate(model,
                           glm::vec3(MUZZLE_OFFSET_X, MUZZLE_OFFSET_Y, MUZZLE_OFFSET_Z));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));

    shader.setMat4("uModel", model);

    // no texture — just draw a bright orange emissive color
    // we use the world shader's uTexture as white + tint in the HUD shader
    // simplest: just set a solid color uniform
    shader.setVec4("uColor", glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));

    // Draw as additive sprite and ignore depth so it is not hidden by the gun mesh.
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    m_mesh->draw();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}
