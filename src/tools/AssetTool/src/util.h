// Helpers shared by the x2::assets sources (not installed).
#pragma once

#include "x2/assets/client_data.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <mutex>
#include <span>
#include <string>
#include <vector>

namespace x2::assets::detail {

using Bytes = std::vector<std::uint8_t>;

unsigned Jobs(unsigned requested, std::size_t work);

// Runs fn(i) for i in [0, count) on `jobs` threads; stops handing out work at the first failure and returns its
// error.
bool ParallelFor(std::size_t count, unsigned jobs, const std::function<bool(std::size_t, std::string&)>& fn,
                 std::string& error);

// Serializes a LogSink called from several threads.
class Log
{
public:
    explicit Log(const LogSink& sink) : sink_(sink) {}
    void operator()(const std::string& line)
    {
        if (!sink_)
            return;
        const std::lock_guard lock(mutex_);
        sink_(line);
    }

private:
    const LogSink& sink_;
    std::mutex mutex_;
};

bool ReadWholeFile(const std::filesystem::path& path, Bytes& out, std::string& error);
bool WriteWholeFile(const std::filesystem::path& path, std::span<const std::uint8_t> data, std::string& error);
std::string Sha256Hex(std::span<const std::uint8_t> data);

// The content hash of a kim record: of its two scripts' hashes.
std::string KimContentHash(const std::string& itemsSha, const std::string& setsSha);

} // namespace x2::assets::detail
