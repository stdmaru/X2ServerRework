#include "MoviePlayer.h"

#include <pl_mpeg.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>

namespace x2::movie {

namespace {

constexpr std::uint32_t FourCC(char a, char b, char c, char d)
{
    return static_cast<std::uint32_t>(static_cast<unsigned char>(a)) |
           static_cast<std::uint32_t>(static_cast<unsigned char>(b)) << 8 |
           static_cast<std::uint32_t>(static_cast<unsigned char>(c)) << 16 |
           static_cast<std::uint32_t>(static_cast<unsigned char>(d)) << 24;
}

std::uint32_t ReadU32(std::span<const std::uint8_t> bytes, std::size_t offset)
{
    return static_cast<std::uint32_t>(bytes[offset]) | static_cast<std::uint32_t>(bytes[offset + 1]) << 8 |
           static_cast<std::uint32_t>(bytes[offset + 2]) << 16 | static_cast<std::uint32_t>(bytes[offset + 3]) << 24;
}

std::int32_t ReadI32(std::span<const std::uint8_t> bytes, std::size_t offset)
{
    return static_cast<std::int32_t>(ReadU32(bytes, offset));
}

// Walks the RIFF chunks of an AVI file: the stream headers in LIST 'hdrl', the data chunks in LIST 'movi'.
class AviReader {
public:
    explicit AviReader(AviVideoStream& out) : m_out(out) {}

    void Chunks(std::span<const std::uint8_t> bytes)
    {
        std::size_t offset = 0;
        while (offset + 8 <= bytes.size()) {
            const std::uint32_t id = ReadU32(bytes, offset);
            std::size_t size = ReadU32(bytes, offset + 4);
            const std::size_t start = offset + 8;
            if (size > bytes.size() - start) {
                // Truncated file: a list keeps the chunks that are complete, any other chunk is dropped.
                if (id != FourCC('L', 'I', 'S', 'T') && id != FourCC('R', 'I', 'F', 'F'))
                    break;
                size = bytes.size() - start;
            }
            Chunk(id, bytes.subspan(start, size));
            offset = start + size + (size & 1);  // chunks are padded to an even size
        }
    }

    bool found() const { return m_videoStream >= 0; }

private:
    void Chunk(std::uint32_t id, std::span<const std::uint8_t> body)
    {
        if (id == FourCC('L', 'I', 'S', 'T') || id == FourCC('R', 'I', 'F', 'F')) {  // list type or RIFF form first
            if (body.size() < 4)
                return;
            if (ReadU32(body, 0) == FourCC('s', 't', 'r', 'l'))
                ++m_streamIndex;
            Chunks(body.subspan(4));
        } else if (id == FourCC('s', 't', 'r', 'h')) {
            // AVISTREAMHEADER: fccType, fccHandler, dwFlags, wPriority/wLanguage, dwInitialFrames, dwScale, dwRate,
            // dwStart, dwLength
            if (body.size() < 36 || m_videoStream >= 0 || ReadU32(body, 0) != FourCC('v', 'i', 'd', 's'))
                return;
            m_videoStream = m_streamIndex;
            m_out.info.codec = ReadU32(body, 4);
            const std::uint32_t scale = ReadU32(body, 20);
            const std::uint32_t rate = ReadU32(body, 24);
            m_out.info.frameRate = scale != 0 ? static_cast<double>(rate) / scale : 0.0;
            m_out.info.frameCount = ReadU32(body, 32);
        } else if (id == FourCC('s', 't', 'r', 'f')) {
            // BITMAPINFOHEADER of the video stream: biSize, biWidth, biHeight (negative when top-down)
            if (m_streamIndex != m_videoStream || body.size() < 12)
                return;
            m_out.info.width = ReadI32(body, 4);
            m_out.info.height = std::abs(ReadI32(body, 8));
        } else if (IsVideoData(id)) {
            m_out.data.insert(m_out.data.end(), body.begin(), body.end());
        }
    }

    // '##dc' (compressed) or '##db' (uncompressed) of the video stream, ## its decimal number
    bool IsVideoData(std::uint32_t id) const
    {
        if (m_videoStream < 0 || m_videoStream > 99)
            return false;
        const char tens = static_cast<char>('0' + m_videoStream / 10);
        const char ones = static_cast<char>('0' + m_videoStream % 10);
        return id == FourCC(tens, ones, 'd', 'c') || id == FourCC(tens, ones, 'd', 'b');
    }

    AviVideoStream& m_out;
    int m_streamIndex = -1;
    int m_videoStream = -1;
};

}  // namespace

std::optional<AviVideoStream> ReadAviVideoStream(std::span<const std::uint8_t> file)
{
    if (file.size() < 12 || ReadU32(file, 0) != FourCC('R', 'I', 'F', 'F') || ReadU32(file, 8) != FourCC('A', 'V', 'I', ' '))
        return std::nullopt;
    AviVideoStream stream;
    AviReader reader(stream);
    reader.Chunks(file);  // RIFF 'AVI ' and any 'AVIX' extensions that follow it (OpenDML files over 1 GB)
    if (!reader.found())
        return std::nullopt;
    return stream;
}

// pl_mpeg reads the stream through callbacks, the way it reads a file, and not from a memory buffer: plm_video_decode
// discards read bytes from any buffer (pl_mpeg c871f2b), which in memory mode moves the caller's data and shrinks the
// buffer below its total size, so the end is never seen (the last reference frame and the B-frames before it were lost)
// and a rewind reads moved data. With callbacks the data stays here and the end is signalled when it runs out.
struct MoviePlayer::Decoder {
    static constexpr std::size_t kChunk = 64 * 1024;

    std::vector<std::uint8_t> data;  // the elementary stream
    std::size_t position = 0;        // of the next byte handed to pl_mpeg
    plm_video_t* video = nullptr;
    plm_frame_t* frame = nullptr;    // the current frame, valid until the next plm_video_decode
    double frameRate = 0.0;

    ~Decoder()
    {
        if (video != nullptr)
            plm_video_destroy(video);
    }

    static void Load(plm_buffer_t* buffer, void* user)
    {
        auto* self = static_cast<Decoder*>(user);
        const std::size_t count = std::min(kChunk, self->data.size() - self->position);
        if (count == 0) {
            plm_buffer_signal_end(buffer);  // what is in the buffer is all there is
            return;
        }
        plm_buffer_write(buffer, self->data.data() + self->position, count);
        self->position += count;
    }

    static void Seek(plm_buffer_t*, std::size_t offset, void* user)
    {
        auto* self = static_cast<Decoder*>(user);
        self->position = std::min(offset, self->data.size());
    }

    static std::size_t Tell(plm_buffer_t*, void* user) { return static_cast<Decoder*>(user)->position; }
};

MoviePlayer::MoviePlayer() = default;
MoviePlayer::~MoviePlayer() = default;

bool MoviePlayer::Open(const std::filesystem::path& path)
{
    Close();
    std::ifstream in(path, std::ios::binary);
    if (!in)
        return false;
    std::vector<std::uint8_t> file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return Open(file);
}

bool MoviePlayer::Open(std::span<const std::uint8_t> file)
{
    Close();
    std::optional<AviVideoStream> stream = ReadAviVideoStream(file);
    if (!stream || stream->data.empty())
        return false;
    const std::uint32_t codec = stream->info.codec;
    if (codec != FourCC('M', 'P', 'G', '1') && codec != FourCC('m', 'p', 'g', '1'))
        return false;

    auto decoder = std::make_unique<Decoder>();
    decoder->data = std::move(stream->data);
    plm_buffer_t* buffer = plm_buffer_create_with_callbacks(&Decoder::Load, &Decoder::Seek, &Decoder::Tell,
                                                            decoder->data.size(), decoder.get());
    decoder->video = plm_video_create_with_buffer(buffer, 1);
    if (decoder->video == nullptr || !plm_video_has_header(decoder->video))
        return false;
    // The AVI rate, as BandiVideo played it; the sequence header's when the AVI has none.
    decoder->frameRate = stream->info.frameRate > 0.0 ? stream->info.frameRate : plm_video_get_framerate(decoder->video);
    if (!(decoder->frameRate > 0.0))
        return false;
    m_decoder = std::move(decoder);
    return true;
}

void MoviePlayer::Close()
{
    m_decoder.reset();
    m_status = Status::Stopped;
    m_clock = 0.0;
    m_decoded = 0;
    m_nextFrame = false;
    m_lastFrameReached = false;
}

bool MoviePlayer::IsOpen() const { return m_decoder != nullptr; }
int MoviePlayer::Width() const { return m_decoder ? plm_video_get_width(m_decoder->video) : 0; }
int MoviePlayer::Height() const { return m_decoder ? plm_video_get_height(m_decoder->video) : 0; }
double MoviePlayer::FrameRate() const { return m_decoder ? m_decoder->frameRate : 0.0; }
MoviePlayer::Status MoviePlayer::GetStatus() const { return m_status; }
bool MoviePlayer::IsNextFrame() const { return m_nextFrame; }

void MoviePlayer::Play()
{
    if (!m_decoder || m_status == Status::Playing)
        return;
    m_status = Status::Playing;
    Advance(0.0);  // the first frame is due at once
}

void MoviePlayer::Stop()
{
    if (m_status == Status::Playing)
        m_status = Status::Stopped;
}

void MoviePlayer::SeekToStart()
{
    if (!m_decoder)
        return;
    plm_video_rewind(m_decoder->video);
    m_decoder->frame = nullptr;
    m_clock = 0.0;
    m_decoded = 0;
    m_nextFrame = false;
    m_lastFrameReached = false;
    if (m_status == Status::Ended)
        m_status = Status::Stopped;
    else if (m_status == Status::Playing)
        Advance(0.0);
}

void MoviePlayer::Advance(double seconds)
{
    if (!m_decoder || m_status != Status::Playing)
        return;
    if (seconds > 0.0)
        m_clock += seconds;
    // Frame n is due at n / rate. The tolerance keeps a clock summed from frame-long steps (1/24 s is not exact in
    // binary) from falling just short of a frame.
    const double position = m_clock * m_decoder->frameRate + 1e-6;
    const auto due = static_cast<std::int64_t>(std::floor(position));
    while (!m_lastFrameReached && m_decoded <= due) {
        if (!DecodeNext())
            m_lastFrameReached = true;
    }
    if (m_lastFrameReached && position >= static_cast<double>(m_decoded))
        m_status = Status::Ended;
}

bool MoviePlayer::DecodeNext()
{
    plm_frame_t* frame = plm_video_decode(m_decoder->video);
    if (frame == nullptr)
        return false;
    m_decoder->frame = frame;
    ++m_decoded;
    m_nextFrame = true;
    return true;
}

bool MoviePlayer::GetFrameBgra(std::span<std::uint8_t> dest, std::size_t pitch)
{
    if (!m_decoder || m_decoder->frame == nullptr)
        return false;
    const plm_frame_t* frame = m_decoder->frame;
    const std::size_t width = frame->width;
    const std::size_t height = frame->height;
    if (pitch < width * 4 || height == 0 || dest.size() < pitch * (height - 1) + width * 4)
        return false;
    plm_frame_to_bgra(m_decoder->frame, dest.data(), static_cast<int>(pitch));
    for (std::size_t y = 0; y < height; ++y) {  // pl_mpeg leaves the fourth byte untouched
        std::uint8_t* row = dest.data() + y * pitch;
        for (std::size_t x = 0; x < width; ++x)
            row[x * 4 + 3] = 0xFF;
    }
    m_nextFrame = false;
    return true;
}

}  // namespace x2::movie
