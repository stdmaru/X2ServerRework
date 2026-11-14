// The client's sound system (src/client/Audio, decision T45), headless: no output device, the mix pulled through Mix().
// Sounds are PCM .wav files built in memory.

#include "SoundSystem.h"

#include "x2/kom/mount.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <set>
#include <span>
#include <thread>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

void Put16(std::vector<std::byte>& out, std::uint16_t v)
{
    out.push_back(static_cast<std::byte>(v & 0xff));
    out.push_back(static_cast<std::byte>(v >> 8));
}

void Put32(std::vector<std::byte>& out, std::uint32_t v)
{
    Put16(out, static_cast<std::uint16_t>(v & 0xffff));
    Put16(out, static_cast<std::uint16_t>(v >> 16));
}

void PutTag(std::vector<std::byte>& out, const char* tag)
{
    for (int i = 0; i < 4; ++i)
        out.push_back(static_cast<std::byte>(tag[i]));
}

// A mono 16-bit PCM .wav of `frames` frames at a constant level.
std::vector<std::byte> Wav(std::uint32_t frames, std::uint32_t rate = 48000, std::int16_t level = 16384)
{
    std::vector<std::byte> out;
    const std::uint32_t dataBytes = frames * 2;
    PutTag(out, "RIFF");
    Put32(out, 36 + dataBytes);
    PutTag(out, "WAVE");
    PutTag(out, "fmt ");
    Put32(out, 16);
    Put16(out, 1);  // PCM
    Put16(out, 1);  // mono
    Put32(out, rate);
    Put32(out, rate * 2);
    Put16(out, 2);
    Put16(out, 16);
    PutTag(out, "data");
    Put32(out, dataBytes);
    for (std::uint32_t i = 0; i < frames; ++i)
        Put16(out, static_cast<std::uint16_t>(level));
    return out;
}

x2::audio::ClipPtr Clip(std::uint32_t frames, std::uint32_t rate = 48000)
{
    std::string error;
    const std::vector<std::byte> file = Wav(frames, rate);
    x2::audio::ClipPtr clip = x2::audio::DecodeClip(file, error);
    REQUIRE_MESSAGE(clip, error);
    return clip;
}

std::unique_ptr<x2::audio::SoundSystem> Headless(std::uint32_t voices = 8)
{
    std::string error;
    x2::audio::SoundSystem::Options options;
    options.output = false;
    options.maxVoices = voices;
    auto system = x2::audio::SoundSystem::Create(options, error);
    REQUIRE_MESSAGE(system, error);
    CHECK_FALSE(system->HasOutput());
    return system;
}

// The mean level of each channel over `frames` frames.
struct Levels {
    float left = 0.f, right = 0.f;
};
Levels Measure(x2::audio::SoundSystem& system, std::uint32_t frames)
{
    std::vector<float> mix(static_cast<std::size_t>(frames) * 2);
    system.Mix(mix.data(), frames);
    Levels levels;
    for (std::uint32_t i = 0; i < frames; ++i) {
        levels.left += std::fabs(mix[i * 2]);
        levels.right += std::fabs(mix[i * 2 + 1]);
    }
    levels.left /= static_cast<float>(frames);
    levels.right /= static_cast<float>(frames);
    return levels;
}

}  // namespace

TEST_CASE("audio: .wav files decode into clips at their own rate; other data does not")
{
    const x2::audio::ClipPtr clip = Clip(1000, 22050);
    CHECK(clip->Channels() == 1);
    CHECK(clip->SampleRate() == 22050);
    CHECK(clip->FrameCount() == 1000);
    CHECK(clip->Frames()[500] == doctest::Approx(0.5f));

    std::string error;
    const std::vector<std::byte> junk(64, std::byte{0x41});
    CHECK(x2::audio::DecodeClip(junk, error) == nullptr);
    CHECK_FALSE(error.empty());
}

TEST_CASE("audio: the original's volume levels")
{
    CHECK(x2::audio::GainFromLevel(-3000.f) == 0.f);
    CHECK(x2::audio::GainFromLevel(-1500.f) == doctest::Approx(0.5f));
    CHECK(x2::audio::GainFromLevel(-500.f) == doctest::Approx(2500.f / 3000.f));
    CHECK(x2::audio::GainFromLevel(-1.f) == doctest::Approx(2999.f / 3000.f));
}

TEST_CASE("audio: a voice plays to its end and is freed; a looping voice goes on")
{
    auto system = Headless();
    const x2::audio::ClipPtr clip = Clip(480);  // 10 ms

    const x2::audio::VoiceId once = system->Play(clip, {});
    x2::audio::PlayParams loopParams;
    loopParams.loop = true;
    const x2::audio::VoiceId loop = system->Play(clip, loopParams);
    CHECK(once != 0);
    CHECK(system->IsPlaying(once));
    CHECK(system->ActiveVoices() == 2);

    CHECK(Measure(*system, 256).left > 0.f);
    Measure(*system, 4800);
    system->Update();
    CHECK_FALSE(system->IsPlaying(once));
    CHECK(system->IsPlaying(loop));
    CHECK(system->ActiveVoices() == 1);

    // Paused, a voice is still playing and is not freed.
    system->SetPaused(loop, true);
    system->Update();
    CHECK(system->IsPlaying(loop));
    CHECK(Measure(*system, 4800).left == doctest::Approx(0.f).epsilon(1e-4));

    system->Stop(loop);
    CHECK_FALSE(system->IsPlaying(loop));
    CHECK(system->ActiveVoices() == 0);
    system->Stop(loop);  // a stale id is nothing
    system->SetVolume(loop, 0.5f);
}

TEST_CASE("audio: a full pool gives the oldest voice to the new sound")
{
    auto system = Headless(2);
    const x2::audio::ClipPtr clip = Clip(48000);
    const x2::audio::VoiceId first = system->Play(clip, {});
    const x2::audio::VoiceId second = system->Play(clip, {});
    const x2::audio::VoiceId third = system->Play(clip, {});
    CHECK_FALSE(system->IsPlaying(first));
    CHECK(system->IsPlaying(second));
    CHECK(system->IsPlaying(third));
    CHECK(third != first);  // same slot, new generation
    CHECK(system->ActiveVoices() == 2);

    system->StopClip(clip.get());
    CHECK(system->ActiveVoices() == 0);
}

TEST_CASE("audio: 3D voices are attenuated linearly and panned in the game's left-handed space")
{
    auto system = Headless();
    const x2::audio::ClipPtr clip = Clip(48000 * 2);
    // The listener at the origin looking down +z with +y up: +x is to its right (Direct3D).
    system->SetListener({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f});

    auto levelAt = [&](x2::audio::Vector3 position) {
        x2::audio::PlayParams params;
        params.spatial = true;
        params.position = position;
        params.minDistance = 100.f;
        params.maxDistance = 300.f;
        const x2::audio::VoiceId voice = system->Play(clip, params);
        Measure(*system, 4800);  // past the gain smoothing
        const Levels levels = Measure(*system, 4800);
        system->Stop(voice);
        return levels;
    };

    const Levels near = levelAt({0.f, 0.f, 50.f});
    const Levels middle = levelAt({0.f, 0.f, 200.f});
    const Levels far = levelAt({0.f, 0.f, 400.f});
    CHECK(near.left > 0.f);
    CHECK(near.left == doctest::Approx(near.right).epsilon(0.01));
    CHECK(middle.left / near.left == doctest::Approx(0.5f).epsilon(0.02));
    CHECK(far.left + far.right == doctest::Approx(0.f).epsilon(1e-4));

    const Levels right = levelAt({100.f, 0.f, 0.f});
    const Levels left = levelAt({-100.f, 0.f, 0.f});
    CHECK(right.right > right.left * 2.f);
    CHECK(left.left > left.right * 2.f);
}

TEST_CASE("audio: one music track streams, loops, pauses and is replaced")
{
    auto system = Headless();
    CHECK_FALSE(system->PlayMusic(std::vector<std::byte>(16, std::byte{0}), true, 1.f, false));
    CHECK_FALSE(system->IsMusicPlaying());

    REQUIRE(system->PlayMusic(Wav(480), true, 1.f, false));
    CHECK(Measure(*system, 4800).left > 0.f);  // ten times its length: it looped
    CHECK(system->IsMusicPlaying());

    system->SetMusicPaused(true);
    CHECK(Measure(*system, 480).left == doctest::Approx(0.f).epsilon(1e-4));
    system->SetMusicPaused(false);
    system->SetMusicVolume(0.f);
    Measure(*system, 4800);
    CHECK(Measure(*system, 480).left == doctest::Approx(0.f).epsilon(1e-4));

    REQUIRE(system->PlayMusic(Wav(480), false, 1.f, false));
    Measure(*system, 4800);
    CHECK_FALSE(system->IsMusicPlaying());  // played once to its end
    system->StopMusic();
}

TEST_CASE("audio: every sound of the client's archives and every music track decodes (X2_MASSFILE_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_MASSFILE_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0') {
        MESSAGE("X2_MASSFILE_CLIENT_DATA not set: skipped");
        return;
    }
    std::string error;
    x2::kom::Mount mount([](const std::string&) {});
    REQUIRE_MESSAGE(mount.AddFolder(folder, error), error);
    std::vector<std::string> names;
    std::set<std::string> seen;
    for (const auto& archive : mount.Archives()) {
        for (const x2::kom::Entry& entry : archive->Entries()) {
            std::string lower = entry.name;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                           [](char c) { return static_cast<char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c); });
            if ((lower.ends_with(".wav") || lower.ends_with(".ogg")) && seen.insert(lower).second)
                names.push_back(entry.name);
        }
    }
    std::atomic<std::size_t> next{0};
    std::atomic<std::size_t> decoded{0};
    std::mutex mutex;
    std::vector<std::string> failures;
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&] {
            std::vector<std::uint8_t> bytes;
            std::string readError;
            for (std::size_t i = next++; i < names.size(); i = next++) {
                std::string decodeError;
                if (mount.Read(names[i], bytes, readError) &&
                    x2::audio::DecodeClip(std::span(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size()),
                                          decodeError)) {
                    ++decoded;
                    continue;
                }
                const std::lock_guard lock(mutex);
                failures.push_back(names[i] + ": " + readError + decodeError);
            }
        });
    }
    for (std::thread& thread : threads)
        thread.join();

    // The music is loose files in data/music, streamed.
    std::size_t tracks = 0;
    auto system = Headless();
    for (const auto& file : std::filesystem::directory_iterator(std::filesystem::path(folder) / "music")) {
        std::ifstream in(file.path(), std::ios::binary);
        std::vector<std::byte> data;
        for (char c; in.get(c);)
            data.push_back(static_cast<std::byte>(c));
        if (system->PlayMusic(std::move(data), true, 1.f, false))
            ++tracks;
        else
            failures.push_back(file.path().filename().string() + ": music does not decode");
    }
    std::string list;
    for (const std::string& failure : failures)
        list += failure + "\n";
    MESSAGE(decoded.load(), " sounds and ", tracks, " music tracks decoded");
    CHECK_MESSAGE(failures.empty(), list);
    CHECK(decoded.load() > 6000);
    CHECK(tracks > 100);
}
