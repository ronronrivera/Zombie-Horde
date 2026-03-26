#include "engine/SoundPlayer.hpp"
#include <cstdlib>

void SoundPlayer::update(float dt, float speed,
                         bool isSprinting,
                         bool justFired,
                         bool justDraw,
                         bool justReloadStarted)
{
    bool moving = speed > 0.1f;

    // ── gunshot ───────────────────────────────────────────
    if (justFired)
        // slight random pitch variation so shots don't sound identical
        m_audio.play("shoot", 1.0f,
                     0.95f + (rand() % 100) / 1000.0f);

    // ── draw sound ────────────────────────────────────────
    if (justDraw)
        m_audio.play("draw", 0.8f, 1.0f);

    // ── reload sound ──────────────────────────────────────
    if (justReloadStarted)
        m_audio.play("reload", 0.9f, 1.0f);

    // ── footsteps ─────────────────────────────────────────
    if (!moving) {
        m_wasMoving  = false;
        return;
    }

    float interval = isSprinting ? SPRINT_INTERVAL : WALK_INTERVAL;
    m_footTimer += dt;

    if (m_footTimer >= interval) {
        m_footTimer -= interval;
        // alternate pitch slightly for left/right foot feel
        float pitch = isSprinting
                    ? 1.05f + (rand() % 100) / 500.0f
                    : 0.95f + (rand() % 100) / 500.0f;
        m_audio.play(isSprinting ? "footstep_run" : "footstep",
                     0.6f, pitch);
    }

    m_wasMoving    = moving;
    m_wasSprinting = isSprinting;
}
