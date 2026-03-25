#include "game/ViewModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

ViewModel::ViewModel()
    : m_hands("assets/ViewModels/hand_rifle.glb") {
    for (const auto& name : m_hands.getAnimationNames()) {
        m_animNames.insert(name);
    }

    m_animDraw = chooseAnim({
        "Armature|Arms_FPS_Anim_Draw"
    }, "Armature|Arms_FPS_Anim_Idle");

    m_animIdle = chooseAnim({
        "Armature|Arms_FPS_Anim_Idle"
    });

    m_animWalk = chooseAnim({
        "Armature|Arms_FPS_Anim_Walk"
    }, "Armature|Arms_FPS_Anim_Idle");

    m_animRun = chooseAnim({
        "Armature|Arms_FPS_Anim_Run"
    }, "Armature|Arms_FPS_Anim_Walk");

    m_animReload = chooseAnim({
        "Armature|Arms_FPS_Anim_Reload_Fast",
        "Armature|Arms_FPS_Anim_Reload"
    }, "Armature|Arms_FPS_Anim_Idle");

    m_animFire = chooseAnim({
        "Armature|Arms_FPS_Anim_Shoot",
        "Armature|Arms_FPS_Anim_Fire"
    }, "Armature|Arms_FPS_Anim_Idle");
}

std::string ViewModel::chooseAnim(
    const std::initializer_list<const char*>& candidates,
    const char* fallback) const {
    for (const char* candidate : candidates) {
        if (m_animNames.find(candidate) != m_animNames.end()) {
            return candidate;
        }
    }

    if (fallback[0] != '\0' && m_animNames.find(fallback) != m_animNames.end()) {
        return fallback;
    }

    if (!m_animNames.empty()) {
        return *m_animNames.begin();
    }

    return "";
}

void ViewModel::update(float dt, bool triggerHeld, bool reloadPressed,
                       bool moving, bool sprinting) {
    m_frameDt = dt;
    m_weapon.update(dt, triggerHeld, reloadPressed);

    if (!m_drawFinished) {
        m_hands.playAnimation(m_animDraw, false);
        m_drawTimer += dt;
        if (m_drawTimer >= DRAW_DURATION_SEC) {
            m_drawFinished = true;
        }
    } else {
        if (m_weapon.justFired()) {
            m_fireAnimHold = FIRE_ANIM_HOLD_SEC;
        } else if (m_fireAnimHold > 0.0f) {
            m_fireAnimHold -= dt;
        }

        if (m_weapon.getState() == WeaponState::Reloading) {
            m_hands.playAnimation(m_animReload);
        } else if (m_fireAnimHold > 0.0f) {
            m_hands.playAnimation(m_animFire, false);
        } else if (sprinting && moving) {
            m_hands.playAnimation(m_animRun);
        } else if (moving) {
            m_hands.playAnimation(m_animWalk);
        } else {
            m_hands.playAnimation(m_animIdle);
        }
    }

    m_hands.update(dt);
}

void ViewModel::draw(Shader& skinnedShader, Shader& emissiveShader,
                     const Camera& camera) {
    glm::mat4 viewModelModel(1.0f);
    viewModelModel = glm::translate(viewModelModel, glm::vec3(0.0f, -0.26f, -0.005f));
    viewModelModel = glm::rotate(viewModelModel, glm::radians(180.0f), glm::vec3(0, 1, 1));

    glm::mat4 invView = glm::inverse(camera.getViewMatrix());
    glm::mat4 viewModelWorld = m_detachedFromCamera ? m_detachedWorld : (invView * viewModelModel);
    skinnedShader.setMat4("uModel", viewModelWorld);
    m_hands.draw(skinnedShader);

    emissiveShader.bind();
    emissiveShader.setMat4("uView", camera.getViewMatrix());
    emissiveShader.setMat4("uProjection", camera.getProjectionMatrix());
    m_muzzleFlash.draw(emissiveShader, viewModelWorld, m_weapon.isMuzzleFlash(), m_frameDt);
}

void ViewModel::toggleDetachFromCamera(const Camera& camera) {
    if (!m_detachedFromCamera) {
        glm::mat4 viewModelModel(1.0f);
        viewModelModel = glm::translate(viewModelModel, glm::vec3(0.0f, -0.26f, -0.005f));
        viewModelModel = glm::rotate(viewModelModel, glm::radians(180.0f), glm::vec3(0, 1, 1));
        m_detachedWorld = glm::inverse(camera.getViewMatrix()) * viewModelModel;
        m_detachedFromCamera = true;
        return;
    }

    m_detachedFromCamera = false;
}
