#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

class AudioManager {

public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager operator=(const AudioManager&) = delete;

    void loadSound(const std::string& name,
                   const std::string& path);

    void play(const std::string& names,
              float gain = 1.0f,
              float pitch = 1.0f);
    
    ALuint playLooping( const std::string& name,
                        float gain = 1.0f,
                        float pitch = 1.0f);
    
    void stop(ALuint sourceId);

    bool isPlaying(ALuint sourceId) const;

    void updateListener(const glm::vec3& position,
                        const glm::vec3& front,
                        const glm::vec3& up);

private:
    void cleanupFinishedSources();
    bool isAvailable() const;

    ALCdevice *m_device = nullptr;
    ALCcontext *m_context = nullptr;
    bool m_audioAvailable = false;

    //name -> OpenAL buffer ID
    std::unordered_map<std::string, ALuint> m_buffers;

    // fire-and-forget sources that must be deleted after playback finishes
    std::vector<ALuint> m_activeOneShots;

    //helper - loads WAV file into AL Buffer
    static ALuint loadWAV(const std::string& path);
};
