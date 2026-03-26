#pragma once
#include "engine/AudioManager.hpp"

class SoundPlayer {
public:
    SoundPlayer(AudioManager& audio) : m_audio(audio) {}

    // call every frame with current movement state
    void update(float dt, float speed, bool isSprinting,
                bool justFired, bool justDraw,
                bool justReloadStarted);

private:
    AudioManager& m_audio;

    // footstep timer — fires a footstep sound every N seconds
    float m_footTimer     = 0.0f;

    // track previous state to detect transitions
    bool  m_wasMoving     = false;
    bool  m_wasSprinting  = false;

    // footstep intervals (seconds between steps)
    static constexpr float WALK_INTERVAL   = 0.50f;
    static constexpr float SPRINT_INTERVAL = 0.30f;
};
