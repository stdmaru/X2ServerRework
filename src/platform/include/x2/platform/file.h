// Read-only files with positional reads (decision T35, docs/porting/kom.md): several threads may read one open file
// at different offsets without sharing a file position, as the client's background loading does with .kom archives.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

namespace x2::platform {

class ReadOnlyFile
{
public:
    // Opens an existing file for reading. nullptr and `error` on failure. On Windows other programs may read the file
    // but not write, rename or delete it while it is open (FILE_SHARE_READ, as the original client opened its .kom
    // files), so replacing an archive that is in use fails; POSIX has no such lock.
    static std::unique_ptr<ReadOnlyFile> Open(const std::filesystem::path& path, std::string& error);

    ~ReadOnlyFile();
    ReadOnlyFile(const ReadOnlyFile&) = delete;
    ReadOnlyFile& operator=(const ReadOnlyFile&) = delete;

    // Size when the file was opened.
    std::uint64_t Size() const { return size_; }

    // Reads exactly `count` bytes at `offset`. Thread-safe. False and `error` on an I/O error or a short read.
    bool ReadAt(std::uint64_t offset, void* buffer, std::size_t count, std::string& error) const;

private:
    ReadOnlyFile() = default;

#if defined(_WIN32)
    void* handle_ = nullptr;
#else
    int fd_ = -1;
#endif
    std::uint64_t size_ = 0;
};

} // namespace x2::platform
