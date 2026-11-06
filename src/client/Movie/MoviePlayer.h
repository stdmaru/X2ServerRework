#pragma once

// The client's movies (the character creation clips in data/movie/*.avi): MPEG-1 video in an AVI file, without audio.
// Replaces BandiVideo (CBandiVideoLibrary) for KTDXLIB's CKTDGUIControl::CMovieData, with the same playback model:
// open, play, a clock that tells when the next frame is due, the frame copied into a caller's buffer, and an end status
// the caller answers by seeking to the start and playing again or stopping. See docs/porting/client-movie.md.

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace x2::movie {

// What the AVI headers say about the video stream.
struct AviVideoInfo {
    std::uint32_t codec = 0;  // the stream's handler FourCC (strh), e.g. 'MPG1'
    int width = 0;
    int height = 0;
    double frameRate = 0.0;      // strh dwRate / dwScale
    std::uint32_t frameCount = 0;  // strh dwLength
};

// The first video stream of an AVI file: its headers and its chunks ('##dc', '##db') concatenated in file order, which
// for MPEG-1 is the elementary stream. Nested LIST 'rec ' chunks are followed. Nothing when the data is not an AVI file
// or has no video stream.
struct AviVideoStream {
    AviVideoInfo info;
    std::vector<std::uint8_t> data;
};
std::optional<AviVideoStream> ReadAviVideoStream(std::span<const std::uint8_t> file);

class MoviePlayer {
public:
    enum class Status { Stopped, Playing, Ended };

    MoviePlayer();
    ~MoviePlayer();
    MoviePlayer(const MoviePlayer&) = delete;
    MoviePlayer& operator=(const MoviePlayer&) = delete;

    // An MPEG-1 AVI file (the whole file is read into memory; the client's are 1-2 MB). False, and the player closed,
    // when it cannot be read or is not MPEG-1 video.
    bool Open(const std::filesystem::path& path);
    bool Open(std::span<const std::uint8_t> file);
    void Close();
    bool IsOpen() const;

    int Width() const;
    int Height() const;
    double FrameRate() const;

    // Play starts (or resumes) the clock; the first frame is due at once. Stop pauses it and keeps the position.
    void Play();
    void Stop();
    // Back to the first frame, keeping the status (an ended movie becomes stopped).
    void SeekToStart();
    Status GetStatus() const;

    // Advances the clock by elapsed seconds while playing, decoding the frames that became due. A movie whose last frame
    // has been shown for its duration becomes Ended.
    void Advance(double seconds);

    // True when a frame was decoded since the last GetFrameBgra (BandiVideo's IsNextFrame).
    bool IsNextFrame() const;

    // Copies the current frame as 32-bit B, G, R, A bytes (D3DFMT_A8R8G8B8 in memory), alpha 255, rows of `pitch`
    // bytes. False when no frame has been decoded or the buffer is too small (pitch < Width() * 4, or fewer than
    // Height() rows).
    bool GetFrameBgra(std::span<std::uint8_t> dest, std::size_t pitch);

private:
    struct Decoder;
    bool DecodeNext();

    std::unique_ptr<Decoder> m_decoder;
    Status m_status = Status::Stopped;
    double m_clock = 0.0;        // seconds since the first frame
    std::int64_t m_decoded = 0;  // frames decoded since the start
    bool m_nextFrame = false;
    bool m_lastFrameReached = false;
};

}  // namespace x2::movie
