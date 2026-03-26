#pragma once
#include <string>

enum class WeaponState {
    Idle,
    Firing,
    Reloading,
    Empty
};

class Weapon {
public:
    Weapon();

    // call every frame
    void update(float dt, bool triggerHeld, bool reloadPressed);

    // getters for ViewModel and HUD
    WeaponState getState()      const { return m_state; }
    int         getAmmo()       const { return m_ammo; }
    int         getMagSize()    const { return MAG_SIZE; }
    bool        justFired()     const { return m_justFired; }
    bool        isMuzzleFlash() const { return m_muzzleTimer > 0.0f; }
    float       getReloadT()    const { return m_reloadDuration > 0.0f ? (m_reloadTimer / m_reloadDuration) : 0.0f; }

    // Keep gameplay reload length in sync with reload animation clip.
    void        setReloadDuration(float seconds);

    // true on the exact frame a bullet is fired — use for raycast
    bool        consumeShot();

private:
    void        fireOnce();
    void        startReload();

    WeaponState m_state       = WeaponState::Idle;

    int   m_ammo              = 30;
    static constexpr int MAG_SIZE     = 30;

    // fire rate — seconds between shots (auto fire)
    static constexpr float FIRE_RATE  = 0.1f;   // 10 rounds/sec
    float m_fireTimer         = 0.0f;

    // muzzle flash duration
    static constexpr float MUZZLE_TIME = 0.06f;
    float m_muzzleTimer       = 0.0f;

    // reload duration (seconds)
    float m_reloadDuration    = 2.0f;
    float m_reloadTimer       = 0.0f;

    bool  m_justFired         = false;  // true for one frame only
    bool  m_shotConsumed      = false;
};
