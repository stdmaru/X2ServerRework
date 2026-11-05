#include "x2/kom/mount.h"

#include <algorithm>
#include <cstdio>
#include <system_error>

namespace x2::kom {

namespace {

std::string FileName(const std::filesystem::path& path)
{
    const std::u8string s = path.filename().u8string();
    return std::string(s.begin(), s.end());
}

} // namespace

void DefaultWarningSink(const std::string& message)
{
    std::fprintf(stderr, "kom: warning: %s\n", message.c_str());
}

bool ArchivesInLoadOrder(const std::filesystem::path& folder, std::vector<std::filesystem::path>& out,
                         std::string& error)
{
    out.clear();
    std::error_code ec;
    std::filesystem::directory_iterator it(folder, ec);
    if (ec)
    {
        const std::u8string s = folder.u8string();
        error = "cannot list " + std::string(s.begin(), s.end()) + ": " + ec.message();
        return false;
    }
    std::vector<std::pair<std::string, std::filesystem::path>> found;
    for (const auto& item : it)
    {
        if (!item.is_regular_file(ec))
            continue;
        const std::string lower = AsciiLower(FileName(item.path()));
        if (lower.size() > 4 && lower.ends_with(".kom"))
            found.emplace_back(lower, item.path());
    }
    std::sort(found.begin(), found.end());
    for (auto& f : found)
        out.push_back(std::move(f.second));
    return true;
}

Mount::Mount(WarningSink warn) : warn_(std::move(warn))
{
    if (!warn_)
        warn_ = [](const std::string&) {};
}

bool Mount::AddArchive(const std::filesystem::path& path, std::string& error)
{
    std::shared_ptr<Archive> archive = Archive::Open(path, error);
    if (!archive)
        return false;
    return AddArchive(std::move(archive));
}

bool Mount::AddArchive(std::shared_ptr<Archive> archive)
{
    if (!archive)
        return false;
    const Archive* a = archive.get();
    archives_.push_back(std::move(archive));
    for (const Entry& e : a->Entries())
    {
        const auto [it, inserted] = names_.try_emplace(AsciiLower(e.name), Hit{a, &e});
        if (!inserted)
        {
            const Hit& first = it->second;
            if (first.archive == a)
                warn_(e.name + " appears twice in " + FileName(a->Path()) + "; the first entry is used");
            else
                warn_(e.name + " is in both " + FileName(first.archive->Path()) + " and " + FileName(a->Path()) +
                      "; using " + FileName(first.archive->Path()));
        }
    }
    return true;
}

bool Mount::AddFolder(const std::filesystem::path& folder, std::string& error)
{
    std::vector<std::filesystem::path> paths;
    if (!ArchivesInLoadOrder(folder, paths, error))
        return false;
    for (const auto& path : paths)
        if (!AddArchive(path, error))
            return false;
    return true;
}

Mount::Hit Mount::Find(std::string_view name) const
{
    const auto it = names_.find(AsciiLower(name));
    return it == names_.end() ? Hit{} : it->second;
}

bool Mount::Read(std::string_view name, std::vector<std::uint8_t>& out, std::string& error) const
{
    const Hit hit = Find(name);
    if (!hit)
    {
        error = std::string(name) + ": not found";
        return false;
    }
    return hit.archive->Read(*hit.entry, out, error);
}

} // namespace x2::kom
