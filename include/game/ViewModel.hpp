#pragma once

#include "engine/Camera.hpp"
#include "game/Weapon.hpp"
#include "renderer/MuzzleFlash.hpp"
#include "renderer/Shader.hpp"
#include "renderer/SkinnedMesh.hpp"
#include <initializer_list>
#include <string>
#include <unordered_set>

class ViewModel {
public:
    ViewModel();

    // Updates weapon state, picks animation, and advances the skinned mesh.
    void update(float dt, bool triggerHeld, bool reloadPressed,
                bool moving, bool sprinting);

    // Draws both the hands weapon model and the muzzle flash.
    void draw(Shader& skinnedShader, Shader& emissiveShader,
              const Camera& camera);

    // Debug helper: freeze view model in world space so camera can move around it.
    void toggleDetachFromCamera(const Camera& camera);
    bool isDetachedFromCamera() const { return m_detachedFromCamera; }

    bool consumeShot() { return m_weapon.consumeShot(); }
    const Weapon& getWeapon() const { return m_weapon; }

private:
    std::string chooseAnim(const std::initializer_list<const char*>& candidates,
                           const char* fallback = "") const;

    Weapon      m_weapon;
    SkinnedMesh m_hands;
    MuzzleFlash m_muzzleFlash;

    std::unordered_set<std::string> m_animNames;

    std::string m_animDraw;
    std::string m_animIdle;
    std::string m_animWalk;
    std::string m_animRun;
    std::string m_animReload;
    std::string m_animFire;

    bool  m_drawFinished = false;
    float m_drawTimer    = 0.0f;
    float m_frameDt      = 0.0f;
    float m_fireAnimHold = 0.0f;

    bool      m_detachedFromCamera = false;
    glm::mat4 m_detachedWorld      = glm::mat4(1.0f);

    static constexpr float DRAW_DURATION_SEC = 0.85f;
    static constexpr float FIRE_ANIM_HOLD_SEC = 0.08f;
};
