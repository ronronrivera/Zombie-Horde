#include "game/Weapon.hpp"

Weapon::Weapon() {}

void Weapon::update(float dt, bool triggerHeld, bool reloadPressed) {
    m_justFired = false;

    // tick timers
    if (m_fireTimer   > 0.0f) m_fireTimer   -= dt;
    if (m_muzzleTimer > 0.0f) m_muzzleTimer -= dt;

    switch (m_state) {

    case WeaponState::Idle:
        // R pressed — start reload only if mag not full
        if (reloadPressed && m_ammo < MAG_SIZE)
            startReload();

        // trigger held + ammo available → fire
        else if (triggerHeld && m_ammo > 0 && m_fireTimer <= 0.0f)
            fireOnce();

        // out of ammo → auto reload
        else if (m_ammo == 0)
            startReload();
        break;

    case WeaponState::Firing:
        // keep firing while trigger held and ammo available
        if (triggerHeld && m_ammo > 0 && m_fireTimer <= 0.0f)
            fireOnce();

        // release trigger → back to idle
        else if (!triggerHeld)
            m_state = WeaponState::Idle;

        // ran dry mid-burst
        else if (m_ammo == 0)
            startReload();
        break;

    case WeaponState::Reloading:
        m_reloadTimer -= dt;
        if (m_reloadTimer <= 0.0f) {
            // reload complete
            m_ammo      = MAG_SIZE;
            m_state     = WeaponState::Idle;
            m_reloadTimer = 0.0f;
        }
        break;

    case WeaponState::Empty:
        // should auto-transition via Idle → startReload
        // but just in case:
        startReload();
        break;
    }
}

void Weapon::fireOnce() {
    m_ammo--;
    m_justFired   = true;
    m_shotConsumed = false;
    m_muzzleTimer = MUZZLE_TIME;
    m_fireTimer   = FIRE_RATE;
    m_state       = WeaponState::Firing;
}

void Weapon::startReload() {
    m_state       = WeaponState::Reloading;
    m_reloadTimer = RELOAD_TIME;
}

bool Weapon::consumeShot() {
    if (m_justFired && !m_shotConsumed) {
        m_shotConsumed = true;
        return true;
    }
    return false;
}
