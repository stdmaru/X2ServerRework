#include "util.h"

#include "Crypto/Crypto.h"

#include <atomic>
#include <fstream>
#include <mutex>
#include <thread>

namespace x2::assets::detail {

unsigned Jobs(unsigned requested, std::size_t work)
{
    unsigned jobs = requested ? requested : std::max(1u, std::thread::hardware_concurrency());
    return static_cast<unsigned>(std::min<std::size_t>(jobs, std::max<std::size_t>(work, 1)));
}

// Runs fn(i) for i in [0, count) on `jobs` threads; stops handing out work at the first failure and returns its
// error.
bool ParallelFor(std::size_t count, unsigned jobs, const std::function<bool(std::size_t, std::string&)>& fn,
                 std::string& error)
{
    std::atomic<std::size_t> next{0};
    std::atomic<bool> failed{false};
    std::mutex mutex;
    const auto worker = [&] {
        std::string e;
        while (!failed)
        {
            const std::size_t i = next++;
            if (i >= count)
                return;
            if (!fn(i, e))
            {
                const std::lock_guard lock(mutex);
                if (!failed.exchange(true))
                    error = e;
                return;
            }
        }
    };
    jobs = Jobs(jobs, count);
    std::vector<std::thread> threads;
    for (unsigned k = 1; k < jobs; ++k)
        threads.emplace_back(worker);
    worker();
    for (std::thread& t : threads)
        t.join();
    return !failed;
}

bool ReadWholeFile(const std::filesystem::path& path, Bytes& out, std::string& error)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
    {
        error = "cannot open " + PathToUtf8(path);
        return false;
    }
    in.seekg(0, std::ios::end);
    const auto size = static_cast<std::size_t>(in.tellg());
    in.seekg(0);
    out.resize(size);
    if (size && !in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size)))
    {
        error = "cannot read " + PathToUtf8(path);
        return false;
    }
    return true;
}

bool WriteWholeFile(const std::filesystem::path& path, std::span<const std::uint8_t> data, std::string& error)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (out)
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    out.flush();
    if (!out)
    {
        error = "cannot write " + PathToUtf8(path);
        return false;
    }
    return true;
}

std::string Sha256Hex(std::span<const std::uint8_t> data)
{
    static constexpr char kDigits[] = "0123456789abcdef";
    const auto digest = crypto::Sha256::Hash(data.data(), data.size());
    std::string out;
    out.reserve(digest.size() * 2);
    for (const auto b : digest)
    {
        out += kDigits[b >> 4];
        out += kDigits[b & 0xF];
    }
    return out;
}

// The content hash of a kim record: of its two scripts' hashes.
std::string KimContentHash(const std::string& itemsSha, const std::string& setsSha)
{
    const std::string both = itemsSha + setsSha;
    return Sha256Hex(std::span(reinterpret_cast<const std::uint8_t*>(both.data()), both.size()));
}

} // namespace x2::assets::detail
