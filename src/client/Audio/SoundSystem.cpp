#include "SoundSystem.h"

#include <SDL3/SDL.h>
#include <miniaudio.h>

#include <algorithm>

namespace x2::audio {

namespace {

constexpr std::uint32_t kOutputChannels = 2;

// Direct3D (and FMOD by default) is left-handed, miniaudio right-handed: the same point with z mirrored.
void ToEngine(const Vector3& v, float& x, float& y, float& z)
{
    x = v.x;
    y = v.y;
    z = -v.z;
}

}  // namespace

ClipPtr DecodeClip(std::span<const std::byte> file, std::string& error)
{
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);  // native channels and rate
    ma_decoder decoder;
    if (ma_decoder_init_memory(file.data(), file.size(), &config, &decoder) != MA_SUCCESS) {
        error = "not a PCM .wav or Vorbis .ogg file";
        return nullptr;
    }
    ma_format format = ma_format_unknown;
    ma_uint32 channels = 0;
    ma_uint32 sampleRate = 0;
    ma_decoder_get_data_format(&decoder, &format, &channels, &sampleRate, nullptr, 0);
    if (channels == 0 || sampleRate == 0) {
        ma_decoder_uninit(&decoder);
        error = "no channels or sample rate";
        return nullptr;
    }

    // Vorbis does not report its length up front (stb_vorbis in push mode): read in blocks until the end.
    auto clip = std::make_shared<Clip>();
    constexpr ma_uint64 kBlock = 16384;
    ma_uint64 frames = 0;
    for (;;) {
        clip->m_frames.resize(static_cast<std::size_t>((frames + kBlock) * channels));
        ma_uint64 read = 0;
        const ma_result result = ma_decoder_read_pcm_frames(&decoder, clip->m_frames.data() + frames * channels, kBlock, &read);
        frames += read;
        if (result != MA_SUCCESS || read < kBlock)
            break;
    }
    ma_decoder_uninit(&decoder);
    clip->m_frames.resize(static_cast<std::size_t>(frames * channels));
    clip->m_frames.shrink_to_fit();
    clip->m_channels = channels;
    clip->m_sampleRate = sampleRate;
    clip->m_frameCount = frames;
    return clip;
}

float GainFromLevel(float level)
{
    constexpr int kBound = 3000;
    const int shifted = static_cast<int>(static_cast<float>(kBound) + level);
    return static_cast<float>(shifted % kBound) / static_cast<float>(kBound);
}

namespace {

struct Voice {
    ma_audio_buffer buffer{};
    ma_sound sound{};
    ClipPtr clip;
    std::uint32_t generation = 1;
    bool used = false;
    bool paused = false;
    std::uint64_t order = 0;  // when it started, to take the oldest voice when the pool is full
};

struct Music {
    std::vector<std::byte> file;
    ma_decoder decoder{};
    ma_sound sound{};
    bool loaded = false;
};

}  // namespace

struct SoundSystem::Impl {
    ma_engine engine{};
    bool engineReady = false;
    std::unique_ptr<Voice[]> voices;
    std::uint32_t voiceCount = 0;
    std::uint64_t nextOrder = 0;
    Music music;

    bool sdlAudio = false;
    SDL_AudioStream* stream = nullptr;
    std::vector<float> mixBuffer;  // SDL's audio thread only

    ~Impl()
    {
        if (stream != nullptr)
            SDL_DestroyAudioStream(stream);  // stops the callback before anything it reads goes away
        if (sdlAudio)
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        for (std::uint32_t i = 0; i < voiceCount; ++i)
            Free(voices[i]);
        StopMusic();
        if (engineReady)
            ma_engine_uninit(&engine);
    }

    static void Free(Voice& voice)
    {
        if (!voice.used)
            return;
        ma_sound_uninit(&voice.sound);
        ma_audio_buffer_uninit(&voice.buffer);
        voice.clip.reset();
        voice.used = false;
        voice.paused = false;
        ++voice.generation;
    }

    void StopMusic()
    {
        if (!music.loaded)
            return;
        ma_sound_uninit(&music.sound);
        ma_decoder_uninit(&music.decoder);
        music.file.clear();
        music.loaded = false;
    }

    Voice* Find(VoiceId id)
    {
        const std::uint64_t slot = id & 0xffffffffu;
        if (slot == 0 || slot > voiceCount)
            return nullptr;
        Voice& voice = voices[static_cast<std::size_t>(slot - 1)];
        if (!voice.used || voice.generation != static_cast<std::uint32_t>(id >> 32))
            return nullptr;
        return &voice;
    }

    VoiceId IdOf(const Voice& voice) const
    {
        const auto slot = static_cast<std::uint64_t>(&voice - voices.get()) + 1;
        return (static_cast<std::uint64_t>(voice.generation) << 32) | slot;
    }

    static void SDLCALL Feed(void* user, SDL_AudioStream* audioStream, int additional, int /*total*/)
    {
        auto* self = static_cast<Impl*>(user);
        constexpr int kFrameBytes = static_cast<int>(sizeof(float) * kOutputChannels);
        int frames = additional / kFrameBytes;
        constexpr int kChunk = 1024;
        if (self->mixBuffer.size() < static_cast<std::size_t>(kChunk) * kOutputChannels)
            self->mixBuffer.resize(static_cast<std::size_t>(kChunk) * kOutputChannels);
        while (frames > 0) {
            const int count = std::min(frames, kChunk);
            ma_uint64 read = 0;
            ma_engine_read_pcm_frames(&self->engine, self->mixBuffer.data(), static_cast<ma_uint64>(count), &read);
            if (read < static_cast<ma_uint64>(count))
                std::fill(self->mixBuffer.begin() + static_cast<std::ptrdiff_t>(read * kOutputChannels),
                          self->mixBuffer.begin() + static_cast<std::ptrdiff_t>(count) * kOutputChannels, 0.f);
            SDL_PutAudioStreamData(audioStream, self->mixBuffer.data(), count * kFrameBytes);
            frames -= count;
        }
    }
};

SoundSystem::SoundSystem(std::unique_ptr<Impl> impl) : m_impl(std::move(impl)) {}
SoundSystem::~SoundSystem() = default;

std::unique_ptr<SoundSystem> SoundSystem::Create(const Options& options, std::string& error)
{
    auto impl = std::make_unique<Impl>();
    ma_engine_config config = ma_engine_config_init();
    config.noDevice = MA_TRUE;
    config.channels = kOutputChannels;
    config.sampleRate = options.sampleRate;
    config.listenerCount = 1;
    if (ma_engine_init(&config, &impl->engine) != MA_SUCCESS) {
        error = "the mixer could not start";
        return nullptr;
    }
    impl->engineReady = true;
    impl->voiceCount = std::max<std::uint32_t>(options.maxVoices, 1);
    impl->voices = std::make_unique<Voice[]>(impl->voiceCount);

    if (options.output) {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            error = std::string("SDL audio: ") + SDL_GetError();
        } else {
            impl->sdlAudio = true;
            SDL_AudioSpec spec{};
            spec.format = SDL_AUDIO_F32;
            spec.channels = static_cast<int>(kOutputChannels);
            spec.freq = static_cast<int>(options.sampleRate);
            impl->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, &Impl::Feed, impl.get());
            if (impl->stream == nullptr) {
                error = std::string("SDL audio device: ") + SDL_GetError();
            } else {
                SDL_ResumeAudioStreamDevice(impl->stream);
            }
        }
    }
    return std::unique_ptr<SoundSystem>(new SoundSystem(std::move(impl)));
}

bool SoundSystem::HasOutput() const { return m_impl->stream != nullptr; }
std::uint32_t SoundSystem::SampleRate() const { return ma_engine_get_sample_rate(&m_impl->engine); }
std::uint32_t SoundSystem::Channels() const { return kOutputChannels; }

VoiceId SoundSystem::Play(const ClipPtr& clip, const PlayParams& params)
{
    if (!clip || clip->FrameCount() == 0)
        return 0;
    Impl& impl = *m_impl;

    Voice* voice = nullptr;
    for (std::uint32_t i = 0; i < impl.voiceCount && voice == nullptr; ++i)
        if (!impl.voices[i].used)
            voice = &impl.voices[i];
    if (voice == nullptr) {
        voice = &impl.voices[0];
        for (std::uint32_t i = 1; i < impl.voiceCount; ++i)
            if (impl.voices[i].order < voice->order)
                voice = &impl.voices[i];
        Impl::Free(*voice);
    }

    ma_audio_buffer_config bufferConfig =
        ma_audio_buffer_config_init(ma_format_f32, clip->Channels(), clip->FrameCount(), clip->Frames(), nullptr);
    bufferConfig.sampleRate = clip->SampleRate();
    if (ma_audio_buffer_init(&bufferConfig, &voice->buffer) != MA_SUCCESS)
        return 0;
    const ma_uint32 flags = params.spatial ? 0 : MA_SOUND_FLAG_NO_SPATIALIZATION;
    if (ma_sound_init_from_data_source(&impl.engine, &voice->buffer, flags, nullptr, &voice->sound) != MA_SUCCESS) {
        ma_audio_buffer_uninit(&voice->buffer);
        return 0;
    }
    voice->clip = clip;
    voice->used = true;
    voice->paused = false;
    voice->order = impl.nextOrder++;

    if (params.spatial) {
        ma_sound_set_attenuation_model(&voice->sound, ma_attenuation_model_linear);
        ma_sound_set_rolloff(&voice->sound, 1.f);
        ma_sound_set_min_distance(&voice->sound, params.minDistance);
        ma_sound_set_max_distance(&voice->sound, std::max(params.maxDistance, params.minDistance));
        ma_sound_set_doppler_factor(&voice->sound, 0.f);  // the original gave FMOD no velocities
        float x, y, z;
        ToEngine(params.position, x, y, z);
        ma_sound_set_position(&voice->sound, x, y, z);
    }
    ma_sound_set_looping(&voice->sound, params.loop ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&voice->sound, params.volume);
    ma_sound_start(&voice->sound);
    return impl.IdOf(*voice);
}

bool SoundSystem::IsPlaying(VoiceId id) const
{
    Voice* voice = m_impl->Find(id);
    return voice != nullptr && !ma_sound_at_end(&voice->sound);
}

void SoundSystem::Stop(VoiceId id)
{
    if (Voice* voice = m_impl->Find(id))
        Impl::Free(*voice);
}

void SoundSystem::SetPaused(VoiceId id, bool paused)
{
    Voice* voice = m_impl->Find(id);
    if (voice == nullptr || voice->paused == paused)
        return;
    voice->paused = paused;
    if (paused)
        ma_sound_stop(&voice->sound);
    else
        ma_sound_start(&voice->sound);
}

void SoundSystem::SetVolume(VoiceId id, float volume)
{
    if (Voice* voice = m_impl->Find(id))
        ma_sound_set_volume(&voice->sound, volume);
}

void SoundSystem::StopClip(const Clip* clip)
{
    for (std::uint32_t i = 0; i < m_impl->voiceCount; ++i)
        if (m_impl->voices[i].used && m_impl->voices[i].clip.get() == clip)
            Impl::Free(m_impl->voices[i]);
}

std::size_t SoundSystem::ActiveVoices() const
{
    std::size_t count = 0;
    for (std::uint32_t i = 0; i < m_impl->voiceCount; ++i)
        count += m_impl->voices[i].used ? 1 : 0;
    return count;
}

void SoundSystem::SetListener(const Vector3& position, const Vector3& front, const Vector3& up)
{
    float x, y, z;
    ToEngine(position, x, y, z);
    ma_engine_listener_set_position(&m_impl->engine, 0, x, y, z);
    ToEngine(front, x, y, z);
    ma_engine_listener_set_direction(&m_impl->engine, 0, x, y, z);
    ToEngine(up, x, y, z);
    ma_engine_listener_set_world_up(&m_impl->engine, 0, x, y, z);
}

bool SoundSystem::PlayMusic(std::vector<std::byte> file, bool loop, float volume, bool paused)
{
    Impl& impl = *m_impl;
    impl.StopMusic();
    Music& music = impl.music;
    music.file = std::move(file);
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    if (ma_decoder_init_memory(music.file.data(), music.file.size(), &config, &music.decoder) != MA_SUCCESS) {
        music.file.clear();
        return false;
    }
    if (ma_sound_init_from_data_source(&impl.engine, &music.decoder, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr,
                                       &music.sound) != MA_SUCCESS) {
        ma_decoder_uninit(&music.decoder);
        music.file.clear();
        return false;
    }
    music.loaded = true;
    ma_sound_set_looping(&music.sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&music.sound, volume);
    if (!paused)
        ma_sound_start(&music.sound);
    return true;
}

void SoundSystem::StopMusic() { m_impl->StopMusic(); }

void SoundSystem::SetMusicVolume(float volume)
{
    if (m_impl->music.loaded)
        ma_sound_set_volume(&m_impl->music.sound, volume);
}

void SoundSystem::SetMusicPaused(bool paused)
{
    if (!m_impl->music.loaded)
        return;
    if (paused)
        ma_sound_stop(&m_impl->music.sound);
    else
        ma_sound_start(&m_impl->music.sound);
}

bool SoundSystem::IsMusicPlaying() const
{
    return m_impl->music.loaded && !ma_sound_at_end(&m_impl->music.sound);
}

void SoundSystem::Update()
{
    for (std::uint32_t i = 0; i < m_impl->voiceCount; ++i) {
        Voice& voice = m_impl->voices[i];
        if (voice.used && !voice.paused && ma_sound_at_end(&voice.sound))
            Impl::Free(voice);
    }
}

void SoundSystem::Mix(float* out, std::uint32_t frames)
{
    if (HasOutput()) {
        std::fill(out, out + static_cast<std::size_t>(frames) * kOutputChannels, 0.f);
        return;
    }
    ma_uint64 read = 0;
    ma_engine_read_pcm_frames(&m_impl->engine, out, frames, &read);
    std::fill(out + static_cast<std::size_t>(read) * kOutputChannels, out + static_cast<std::size_t>(frames) * kOutputChannels,
              0.f);
}

}  // namespace x2::audio
