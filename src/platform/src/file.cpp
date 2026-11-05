#include "x2/platform/file.h"

#include <system_error>

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include "x2/platform/unicode.h"
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace x2::platform {

namespace {

std::string SystemError(const std::string& what, int code)
{
    return what + ": " + std::system_category().message(code);
}

} // namespace

#if defined(_WIN32)

std::unique_ptr<ReadOnlyFile> ReadOnlyFile::Open(const std::filesystem::path& path, std::string& error)
{
    const HANDLE handle = ::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
    {
        error = SystemError("cannot open " + WideToUtf8(path.native()), static_cast<int>(::GetLastError()));
        return nullptr;
    }
    LARGE_INTEGER size{};
    if (!::GetFileSizeEx(handle, &size))
    {
        error = SystemError("cannot get the size of " + WideToUtf8(path.native()), static_cast<int>(::GetLastError()));
        ::CloseHandle(handle);
        return nullptr;
    }
    std::unique_ptr<ReadOnlyFile> file(new ReadOnlyFile);
    file->handle_ = handle;
    file->size_ = static_cast<std::uint64_t>(size.QuadPart);
    return file;
}

ReadOnlyFile::~ReadOnlyFile()
{
    if (handle_ != nullptr)
        ::CloseHandle(static_cast<HANDLE>(handle_));
}

bool ReadOnlyFile::ReadAt(std::uint64_t offset, void* buffer, std::size_t count, std::string& error) const
{
    auto* out = static_cast<unsigned char*>(buffer);
    while (count > 0)
    {
        // A synchronous handle with an OVERLAPPED offset reads at that offset; the I/O manager serializes the calls on
        // one handle, so concurrent readers do not disturb each other.
        OVERLAPPED overlapped{};
        overlapped.Offset = static_cast<DWORD>(offset);
        overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
        const DWORD chunk = count > 0x40000000u ? 0x40000000u : static_cast<DWORD>(count);
        DWORD read = 0;
        if (!::ReadFile(static_cast<HANDLE>(handle_), out, chunk, &read, &overlapped))
        {
            const DWORD code = ::GetLastError();
            if (code == ERROR_HANDLE_EOF)
                break;
            error = SystemError("read error", static_cast<int>(code));
            return false;
        }
        if (read == 0)
            break;
        out += read;
        offset += read;
        count -= read;
    }
    if (count != 0)
    {
        error = "unexpected end of file";
        return false;
    }
    return true;
}

#else

std::unique_ptr<ReadOnlyFile> ReadOnlyFile::Open(const std::filesystem::path& path, std::string& error)
{
    const int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        error = SystemError("cannot open " + path.string(), errno);
        return nullptr;
    }
    struct stat st{};
    if (::fstat(fd, &st) != 0)
    {
        error = SystemError("cannot stat " + path.string(), errno);
        ::close(fd);
        return nullptr;
    }
    if (!S_ISREG(st.st_mode))
    {
        error = "cannot open " + path.string() + ": not a regular file";
        ::close(fd);
        return nullptr;
    }
    std::unique_ptr<ReadOnlyFile> file(new ReadOnlyFile);
    file->fd_ = fd;
    file->size_ = static_cast<std::uint64_t>(st.st_size);
    return file;
}

ReadOnlyFile::~ReadOnlyFile()
{
    if (fd_ >= 0)
        ::close(fd_);
}

bool ReadOnlyFile::ReadAt(std::uint64_t offset, void* buffer, std::size_t count, std::string& error) const
{
    auto* out = static_cast<unsigned char*>(buffer);
    while (count > 0)
    {
        const ssize_t read = ::pread(fd_, out, count, static_cast<off_t>(offset));
        if (read < 0)
        {
            if (errno == EINTR)
                continue;
            error = SystemError("read error", errno);
            return false;
        }
        if (read == 0)
        {
            error = "unexpected end of file";
            return false;
        }
        out += read;
        offset += static_cast<std::uint64_t>(read);
        count -= static_cast<std::size_t>(read);
    }
    return true;
}

#endif

} // namespace x2::platform
