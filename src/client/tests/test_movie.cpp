// x2::movie: the MPEG-1 AVI player that replaces BandiVideo (docs/porting/client-movie.md).
//
// data/movie_red_blue.avi is our own fixture, made with
//   ffmpeg -f lavfi -i "color=c=red:s=64x48:r=24:d=0.25[a];color=c=blue:s=64x48:r=24:d=0.25[b];[a][b]concat=n=2:v=1"
//          -c:v mpeg1video -vtag MPG1 -q:v 2 -bf 0 -g 6 -an -fflags +bitexact -flags +bitexact movie_red_blue.avi
// 12 frames at 24 fps, 6 red then 6 blue, I- and P-frames only like the client's movies (pl_mpeg drops the last reference
// frame of a stream that ends with a B-frame; docs/porting/client-movie.md).

#include "MoviePlayer.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using x2::movie::MoviePlayer;

namespace {

const std::filesystem::path kFixture = std::filesystem::path(X2_CLIENT_TEST_DATA_DIR) / "movie_red_blue.avi";

std::vector<std::uint8_t> ReadFile(const std::filesystem::path& path)
{
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

struct Bgra {
    int b, g, r, a;
};

Bgra Center(MoviePlayer& player)
{
    const std::size_t pitch = static_cast<std::size_t>(player.Width()) * 4 + 16;  // a pitch wider than a row
    std::vector<std::uint8_t> frame(pitch * static_cast<std::size_t>(player.Height()));
    REQUIRE(player.GetFrameBgra(frame, pitch));
    const std::uint8_t* p = frame.data() + (player.Height() / 2) * pitch + (player.Width() / 2) * 4;
    return {p[0], p[1], p[2], p[3]};
}

bool IsRed(const Bgra& c) { return c.r > 200 && c.g < 60 && c.b < 60 && c.a == 255; }
bool IsBlue(const Bgra& c) { return c.b > 200 && c.g < 60 && c.r < 60 && c.a == 255; }

}  // namespace

TEST_CASE("movie: AVI headers and the video stream")
{
    const std::vector<std::uint8_t> file = ReadFile(kFixture);
    REQUIRE(!file.empty());
    const auto stream = x2::movie::ReadAviVideoStream(file);
    REQUIRE(stream.has_value());
    CHECK(stream->info.codec == 0x3147504Du);  // 'MPG1'
    CHECK(stream->info.width == 64);
    CHECK(stream->info.height == 48);
    CHECK(stream->info.frameRate == doctest::Approx(24.0));
    CHECK(stream->info.frameCount == 12);
    // the elementary stream starts with the MPEG-1 sequence header
    REQUIRE(stream->data.size() > 4);
    CHECK(std::equal(stream->data.begin(), stream->data.begin() + 4, std::vector<std::uint8_t>{0, 0, 1, 0xB3}.begin()));

    CHECK_FALSE(x2::movie::ReadAviVideoStream({}).has_value());
    const std::vector<std::uint8_t> notAvi = {'R', 'I', 'F', 'F', 4, 0, 0, 0, 'W', 'A', 'V', 'E'};
    CHECK_FALSE(x2::movie::ReadAviVideoStream(notAvi).has_value());
    // truncated in the middle of the data: the complete chunks are kept
    const std::vector<std::uint8_t> half(file.begin(), file.begin() + static_cast<std::ptrdiff_t>(file.size() / 2));
    const auto partial = x2::movie::ReadAviVideoStream(half);
    REQUIRE(partial.has_value());
    CHECK(partial->data.size() < stream->data.size());
}

TEST_CASE("movie: open")
{
    MoviePlayer player;
    CHECK_FALSE(player.IsOpen());
    CHECK_FALSE(player.Open(kFixture.parent_path() / "missing.avi"));
    CHECK_FALSE(player.Open(std::vector<std::uint8_t>{1, 2, 3}));
    REQUIRE(player.Open(kFixture));
    CHECK(player.IsOpen());
    CHECK(player.Width() == 64);
    CHECK(player.Height() == 48);
    CHECK(player.FrameRate() == doctest::Approx(24.0));
    CHECK(player.GetStatus() == MoviePlayer::Status::Stopped);
    CHECK_FALSE(player.IsNextFrame());

    std::vector<std::uint8_t> frame(64 * 48 * 4);
    CHECK_FALSE(player.GetFrameBgra(frame, 64 * 4));  // nothing decoded before Play
    player.Advance(1.0);                               // the clock only runs while playing
    CHECK_FALSE(player.IsNextFrame());
    player.Close();
    CHECK_FALSE(player.IsOpen());
}

TEST_CASE("movie: playback clock, end and loop")
{
    MoviePlayer player;
    REQUIRE(player.Open(kFixture));
    player.Play();
    CHECK(player.GetStatus() == MoviePlayer::Status::Playing);
    REQUIRE(player.IsNextFrame());  // the first frame is due at once
    CHECK(IsRed(Center(player)));
    CHECK_FALSE(player.IsNextFrame());

    std::vector<std::uint8_t> small(64 * 4 * 47);
    CHECK_FALSE(player.GetFrameBgra(small, 64 * 4));  // one row short
    CHECK_FALSE(player.GetFrameBgra(small, 63 * 4));  // pitch shorter than a row

    player.Advance(0.5 / 24);  // half a frame: nothing new
    CHECK_FALSE(player.IsNextFrame());
    player.Advance(0.5 / 24);  // frame 1
    CHECK(player.IsNextFrame());
    CHECK(IsRed(Center(player)));

    player.Stop();  // paused: the clock does not move
    player.Advance(1.0);
    CHECK_FALSE(player.IsNextFrame());
    player.Play();
    CHECK_FALSE(player.IsNextFrame());

    player.Advance(5.0 / 24);  // frame 6, the first blue one
    CHECK(player.IsNextFrame());
    CHECK(IsBlue(Center(player)));
    player.Advance(5.0 / 24);  // frame 11, the last
    CHECK(IsBlue(Center(player)));
    CHECK(player.GetStatus() == MoviePlayer::Status::Playing);  // still shown for its duration
    player.Advance(1.0 / 24);
    CHECK(player.GetStatus() == MoviePlayer::Status::Ended);
    CHECK_FALSE(player.IsNextFrame());

    // CMovieData's loop: seek to the start and play again
    player.SeekToStart();
    CHECK(player.GetStatus() == MoviePlayer::Status::Stopped);
    player.Play();
    REQUIRE(player.IsNextFrame());
    CHECK(IsRed(Center(player)));

    // a long stall decodes every frame that became due, then ends
    player.Advance(10.0);
    CHECK(player.GetStatus() == MoviePlayer::Status::Ended);
    CHECK(IsBlue(Center(player)));
}

TEST_CASE("movie: every frame is decoded once")
{
    MoviePlayer player;
    REQUIRE(player.Open(kFixture));
    player.Play();
    int frames = 0;
    while (player.GetStatus() == MoviePlayer::Status::Playing) {
        if (player.IsNextFrame()) {
            ++frames;
            const Bgra c = Center(player);
            CHECK((frames <= 6 ? IsRed(c) : IsBlue(c)));
        }
        player.Advance(1.0 / 24);
    }
    CHECK(frames == 12);
}

TEST_CASE("movie: the client's movies (X2_MOVIE_DIR)")
{
    const char* folder = std::getenv("X2_MOVIE_DIR");
    if (folder == nullptr || *folder == '\0') {
        MESSAGE("X2_MOVIE_DIR not set: skipped");
        return;
    }
    int movies = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() != ".avi")
            continue;
        CAPTURE(entry.path().filename().string());
        const auto stream = x2::movie::ReadAviVideoStream(ReadFile(entry.path()));
        REQUIRE(stream.has_value());
        MoviePlayer player;
        REQUIRE(player.Open(entry.path()));
        CHECK(player.Width() == stream->info.width);
        CHECK(player.Height() == stream->info.height);
        player.Play();
        std::uint32_t frames = 0;
        std::vector<std::uint8_t> frame(static_cast<std::size_t>(player.Width()) * player.Height() * 4);
        while (player.GetStatus() == MoviePlayer::Status::Playing) {
            if (player.IsNextFrame()) {
                ++frames;
                CHECK(player.GetFrameBgra(frame, static_cast<std::size_t>(player.Width()) * 4));
            }
            player.Advance(1.0 / player.FrameRate());
        }
        CHECK(frames == stream->info.frameCount);
        ++movies;
    }
    MESSAGE(movies << " movies decoded");
    CHECK(movies > 0);
}
