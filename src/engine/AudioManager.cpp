#include "engine/AudioManager.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// minimal WAV loader — handles PCM 8/16 bit mono/stereo
struct WavData {
    std::vector<char> data;
    ALenum  format;
    ALsizei sampleRate;
};

static WavData loadWavFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open WAV: " + path);

    // read RIFF header
    char riff[4]; f.read(riff, 4);
    if (std::strncmp(riff, "RIFF", 4) != 0)
        throw std::runtime_error("Not a RIFF file: " + path);

    uint32_t chunkSize;  f.read((char*)&chunkSize, 4);
    char wave[4];        f.read(wave, 4);
    if (std::strncmp(wave, "WAVE", 4) != 0)
        throw std::runtime_error("Not a WAVE file: " + path);

    uint16_t audioFormat, numChannels, bitsPerSample, blockAlign;
    uint32_t sampleRate, byteRate, dataSize = 0;

    // scan chunks
    char chunkId[4];
    uint32_t sz;
    while (f.read(chunkId, 4) && f.read((char*)&sz, 4)) {
        if (std::strncmp(chunkId, "fmt ", 4) == 0) {
            f.read((char*)&audioFormat,  2);
            f.read((char*)&numChannels,  2);
            f.read((char*)&sampleRate,   4);
            f.read((char*)&byteRate,     4);
            f.read((char*)&blockAlign,   2);
            f.read((char*)&bitsPerSample,2);
            if (sz > 16) f.seekg(sz - 16, std::ios::cur);
        } else if (std::strncmp(chunkId, "data", 4) == 0) {
            dataSize = sz;
            break;
        } else {
            f.seekg(sz, std::ios::cur);
        }
    }

    if (dataSize == 0)
        throw std::runtime_error("No data chunk in WAV: " + path);

    WavData wav;
    wav.data.resize(dataSize);
    f.read(wav.data.data(), dataSize);
    wav.sampleRate = (ALsizei)sampleRate;

    if      (numChannels == 1 && bitsPerSample == 8)  wav.format = AL_FORMAT_MONO8;
    else if (numChannels == 1 && bitsPerSample == 16) wav.format = AL_FORMAT_MONO16;
    else if (numChannels == 2 && bitsPerSample == 8)  wav.format = AL_FORMAT_STEREO8;
    else if (numChannels == 2 && bitsPerSample == 16) wav.format = AL_FORMAT_STEREO16;
    else throw std::runtime_error("Unsupported WAV format: " + path);

    return wav;
}

AudioManager::AudioManager() {
    m_device = alcOpenDevice(nullptr); // default device
    if (!m_device) {
        throw std::runtime_error("Fatal: Failed to open audio device");
    }

    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context || !alcMakeContextCurrent(m_context)) {
        if (m_context) {
            alcDestroyContext(m_context);
            m_context = nullptr;
        }
        alcCloseDevice(m_device);
        m_device = nullptr;
        throw std::runtime_error("Fatal: Failed to create audio context");
    }

    // set distance model — sound fades realistically with distance
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    m_audioAvailable = true;
    std::cout << "Audio device: "
              << alcGetString(m_device, ALC_DEVICE_SPECIFIER) << "\n";
}

AudioManager::~AudioManager() {
    cleanupFinishedSources();

    for (ALuint source : m_activeOneShots) {
        alSourceStop(source);
        alDeleteSources(1, &source);
    }
    m_activeOneShots.clear();

    // delete all buffers
    for (auto& [name, buf] : m_buffers)
        alDeleteBuffers(1, &buf);

    alcMakeContextCurrent(nullptr);
    if (m_context) alcDestroyContext(m_context);
    if (m_device)  alcCloseDevice(m_device);
}

void AudioManager::loadSound(const std::string& name,
                              const std::string& path) {
    if (!isAvailable()) return;
    if (m_buffers.count(name)) return; // already loaded

    WavData wav = loadWavFile(path);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, wav.format,
                 wav.data.data(), (ALsizei)wav.data.size(),
                 wav.sampleRate);

    m_buffers[name] = buffer;
    std::cout << "Loaded sound: " << name << "\n";
}

void AudioManager::play(const std::string& name,
                        float gain, float pitch) {
    if (!isAvailable()) return;
    cleanupFinishedSources();

    auto it = m_buffers.find(name);
    if (it == m_buffers.end()) {
        std::cerr << "Sound not found: " << name << "\n";
        return;
    }

    // create a temporary source — OpenAL cleans it up
    // for fire-and-forget sounds this is fine
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source,  AL_BUFFER,   (ALint)it->second);
    alSourcef(source,  AL_GAIN,     gain);
    alSourcef(source,  AL_PITCH,    pitch);
    alSourcei(source,  AL_LOOPING,  AL_FALSE);
    alSourcePlay(source);
    m_activeOneShots.push_back(source);
}

void AudioManager::cleanupFinishedSources() {
    if (!isAvailable()) return;
    auto it = m_activeOneShots.begin();
    while (it != m_activeOneShots.end()) {
        ALint state = 0;
        alGetSourcei(*it, AL_SOURCE_STATE, &state);

        if (state == AL_STOPPED || state == AL_INITIAL) {
            alDeleteSources(1, &(*it));
            it = m_activeOneShots.erase(it);
        } else {
            ++it;
        }
    }
}

ALuint AudioManager::playLooping(const std::string& name,
                                  float gain, float pitch) {
    if (!isAvailable()) return 0;
    auto it = m_buffers.find(name);
    if (it == m_buffers.end()) return 0;

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER,  (ALint)it->second);
    alSourcef(source, AL_GAIN,    gain);
    alSourcef(source, AL_PITCH,   pitch);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcePlay(source);
    return source;
}

void AudioManager::stop(ALuint sourceId) {
    if (sourceId == 0) return;
    alSourceStop(sourceId);
    alDeleteSources(1, &sourceId);
}

bool AudioManager::isPlaying(ALuint sourceId) const {
    if (!isAvailable() || sourceId == 0) return false;
    ALint state;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void AudioManager::updateListener(const glm::vec3& position,
                                   const glm::vec3& front,
                                   const glm::vec3& up) {
    if (!isAvailable()) return;
    alListener3f(AL_POSITION,
                 position.x, position.y, position.z);

    // OpenAL orientation takes a 6-float array: [front, up]
    float ori[6] = {
        front.x, front.y, front.z,
        up.x,    up.y,    up.z
    };
    alListenerfv(AL_ORIENTATION, ori);
}

bool AudioManager::isAvailable() const {
    return m_audioAvailable && m_device != nullptr && m_context != nullptr;
}
