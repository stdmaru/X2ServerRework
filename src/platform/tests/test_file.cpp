#include "x2/platform/file.h"

#include <doctest/doctest.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

using x2::platform::ReadOnlyFile;

TEST_CASE("ReadOnlyFile: positional reads, size, errors")
{
    const auto path = std::filesystem::temp_directory_path() / "x2_platform_file_test.bin";
    std::vector<unsigned char> data(100000);
    for (std::size_t i = 0; i < data.size(); ++i)
        data[i] = static_cast<unsigned char>(i * 13 + i / 256);
    {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    }

    std::string error;
    {
        auto file = ReadOnlyFile::Open(path, error);
        REQUIRE_MESSAGE(file, error);
        CHECK(file->Size() == data.size());

        unsigned char buf[16];
        REQUIRE(file->ReadAt(99990, buf, 10, error));
        CHECK(std::equal(buf, buf + 10, data.begin() + 99990));
        CHECK_FALSE(file->ReadAt(99995, buf, 10, error));  // short read
        CHECK_FALSE(error.empty());
        REQUIRE(file->ReadAt(0, buf, 0, error));

        // Concurrent readers share the handle but not a file position.
        std::atomic<int> failures{0};
        std::vector<std::thread> threads;
        for (int t = 0; t < 8; ++t)
            threads.emplace_back([&, t] {
                std::vector<unsigned char> chunk(1000);
                std::string err;
                for (int round = 0; round < 200; ++round)
                {
                    const std::size_t at = static_cast<std::size_t>((t * 7919 + round * 104729) % 99000);
                    if (!file->ReadAt(at, chunk.data(), chunk.size(), err) ||
                        !std::equal(chunk.begin(), chunk.end(), data.begin() + static_cast<std::ptrdiff_t>(at)))
                        ++failures;
                }
            });
        for (auto& th : threads)
            th.join();
        CHECK(failures == 0);
    }
    std::filesystem::remove(path);

    CHECK_FALSE(ReadOnlyFile::Open(path, error));
    CHECK(error.find("cannot open") != std::string::npos);
    CHECK_FALSE(ReadOnlyFile::Open(std::filesystem::temp_directory_path(), error));
}
