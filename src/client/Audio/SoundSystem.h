#pragma once

// The client's audio (decision T45), replacing FMOD Ex behind KTDXLIB's CKTDSManager, CKTDSMP3Play and
// CKTDXDeviceSound: miniaudio decodes and mixes, an SDL3 audio stream plays the mix. The model is the original's use
// of FMOD: sounds decoded once into memory (Clip) and played on any number of voices at once, a voice pool with a
// fixed size, one streamed music track, and 3D voices with linear roll-off between a minimum and a maximum distance
// around one listener. Positions and directions are in the game's left-handed space (Direct3D), as FMOD took them.
// See docs/porting/client-ktdxlib.md#sound.
//
// Threads: decoding (DecodeClip) runs on any thread; everything on SoundSystem runs on one thread (the game's). The
// mix runs on SDL's audio thread, or on the caller's through Mix() when there is no output (tests).

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace x2::audio {

// A sound decoded to 32-bit float frames, at its own sample rate and channel count (the voices resample and mix).
class Clip {
public:
    std::uint32_t Channels() const { return m_channels; }
    std::uint32_t SampleRate() const { return m_sampleRate; }
    std::uint64_t FrameCount() const { return m_frameCount; }
    const float* Frames() const { return m_frames.data(); }

private:
    friend std::shared_ptr<const Clip> DecodeClip(std::span<const std::byte>, std::string&);
    std::uint32_t m_channels = 0;
    std::uint32_t m_sampleRate = 0;
    std::uint64_t m_frameCount = 0;
    std::vector<float> m_frames;
};
using ClipPtr = std::shared_ptr<const Clip>;

// A .wav (PCM) or .ogg (Vorbis) file in memory. Nothing, with a reason in `error`, when it cannot be decoded.
ClipPtr DecodeClip(std::span<const std::byte> file, std::string& error);

struct Vector3 {
    float x = 0.f, y = 0.f, z = 0.f;
};

// A voice: a slot of the pool and the generation of what plays on it, so an id kept after its voice ended or was
// taken by another sound refers to nothing. 0 is no voice.
using VoiceId = std::uint64_t;

struct PlayParams {
    bool loop = false;
    float volume = 1.f;  // linear gain
    bool spatial = false;  // 3D: placed at `position`, attenuated linearly from minDistance (full) to maxDistance (silent)
    Vector3 position;
    float minDistance = 1.f;
    float maxDistance = 1000.f;
};

class SoundSystem {
public:
    struct Options {
        bool output = true;  // false: no SDL device; the mix is pulled through Mix() (tests)
        std::uint32_t maxVoices = 128;  // FMOD's channel count in the original (FIX_STOLEN_MUSIC_CHANNEL)
        std::uint32_t sampleRate = 48000;
    };

    // Nothing, with a reason in `error`, when the mixer cannot start. Opening the output can fail separately (no audio
    // device, no audio server): the system then works without output, HasOutput() is false and `error` says why.
    static std::unique_ptr<SoundSystem> Create(const Options& options, std::string& error);
    ~SoundSystem();
    SoundSystem(const SoundSystem&) = delete;
    SoundSystem& operator=(const SoundSystem&) = delete;

    bool HasOutput() const;
    std::uint32_t SampleRate() const;
    std::uint32_t Channels() const;  // 2

    // Effects. A new sound takes a free voice, or the voice that started first when the pool is full.
    VoiceId Play(const ClipPtr& clip, const PlayParams& params);
    // Playing, paused included, until it reaches its end (a looping voice never does) or is stopped.
    bool IsPlaying(VoiceId voice) const;
    void Stop(VoiceId voice);
    void SetPaused(VoiceId voice, bool paused);
    void SetVolume(VoiceId voice, float volume);
    void StopClip(const Clip* clip);  // every voice playing it
    std::size_t ActiveVoices() const;

    void SetListener(const Vector3& position, const Vector3& front, const Vector3& up);

    // The music: one track at a time, streamed from an encoded file in memory (the system keeps the bytes). Starting
    // one replaces the previous. False when the data cannot be decoded.
    bool PlayMusic(std::vector<std::byte> file, bool loop, float volume, bool paused);
    void StopMusic();
    void SetMusicVolume(float volume);
    void SetMusicPaused(bool paused);
    bool IsMusicPlaying() const;  // a track is loaded and has not ended (paused included)

    // Once per frame: frees the voices that ended.
    void Update();

    // Without output only: mixes `frames` stereo frames into `out` (interleaved, 2 floats per frame).
    void Mix(float* out, std::uint32_t frames);

    struct Impl;

private:
    explicit SoundSystem(std::unique_ptr<Impl> impl);
    std::unique_ptr<Impl> m_impl;
};

// The original's volume scale: the options give a level in [-3000, 0) (effects are held at -500 or lower, music at -1
// or lower), mapped to a linear gain as ((3000 + level) mod 3000) / 3000 with integer truncation, exactly as KTDXLIB
// computed it before every FMOD_Channel_SetVolume.
float GainFromLevel(float level);

}  // namespace x2::audio
