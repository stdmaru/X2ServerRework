#include "x2/platform/ini.h"
#include "x2/platform/unicode.h"

#include <cwctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::wstring Trim(const std::wstring& s)
{
    const size_t b = s.find_first_not_of(L" \t\r");
    if (b == std::wstring::npos)
        return {};
    return s.substr(b, s.find_last_not_of(L" \t\r") - b + 1);
}

bool EqualNoCase(const std::wstring& a, const wchar_t* b)
{
    size_t i = 0;
    for (; i < a.size() && b[i]; ++i) {
        if (std::towlower(static_cast<wint_t>(a[i])) != std::towlower(static_cast<wint_t>(b[i])))
            return false;
    }
    return i == a.size() && !b[i];
}

std::optional<std::wstring> ReadText(const wchar_t* fileName)
{
    std::ifstream file(std::filesystem::path(fileName), std::ios::binary); // wide path on Windows too
    if (!file)
        return std::nullopt;
    const std::string bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return x2::platform::DecodeTextFile(bytes);
}

// Section name of a "[name]" line, or nullopt for any other line.
std::optional<std::wstring> SectionName(const std::wstring& trimmed)
{
    if (trimmed.empty() || trimmed[0] != L'[')
        return std::nullopt;
    const size_t end = trimmed.find(L']');
    return Trim(trimmed.substr(1, end == std::wstring::npos ? std::wstring::npos : end - 1));
}

std::optional<std::wstring> Lookup(const wchar_t* section, const wchar_t* key, const wchar_t* fileName)
{
    if (!section || !key || !fileName)
        return std::nullopt;
    const std::optional<std::wstring> text = ReadText(fileName);
    if (!text)
        return std::nullopt;

    std::wistringstream stm(*text);
    std::wstring line;
    bool inSection = false;
    while (std::getline(stm, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == L';')
            continue;
        if (const std::optional<std::wstring> name = SectionName(line)) {
            inSection = EqualNoCase(*name, section);
            continue;
        }
        const size_t eq = line.find(L'=');
        if (!inSection || eq == std::wstring::npos || !EqualNoCase(Trim(line.substr(0, eq)), key))
            continue;
        std::wstring value = Trim(line.substr(eq + 1));
        if (value.size() >= 2 && (value.front() == L'"' || value.front() == L'\'') && value.back() == value.front())
            value = value.substr(1, value.size() - 2);
        return value;
    }
    return std::nullopt;
}

} // namespace

namespace x2::platform {

DWORD ReadIniString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultValue,
                    wchar_t* dst, DWORD size, const wchar_t* fileName)
{
    if (!dst || size == 0)
        return 0;
    const std::wstring value = Lookup(section, key, fileName).value_or(defaultValue ? defaultValue : L"");
    const size_t n = value.size() < size ? value.size() : size - 1;
    value.copy(dst, n);
    dst[n] = L'\0';
    return static_cast<DWORD>(n);
}

UINT ReadIniInt(const wchar_t* section, const wchar_t* key, INT defaultValue, const wchar_t* fileName)
{
    const std::optional<std::wstring> value = Lookup(section, key, fileName);
    if (!value)
        return static_cast<UINT>(defaultValue);
    return static_cast<UINT>(std::wcstol(value->c_str(), nullptr, 10));
}

bool WriteIniString(const wchar_t* section, const wchar_t* key, const wchar_t* value, const wchar_t* fileName)
{
    if (!section || !fileName)
        return false;

    std::vector<std::wstring> lines;
    {
        std::wistringstream stm(ReadText(fileName).value_or(L""));
        std::wstring line;
        while (std::getline(stm, line)) {
            if (!line.empty() && line.back() == L'\r')
                line.pop_back();
            lines.push_back(line);
        }
    }

    // [begin, end) of the section's lines (header included), end of the last key line in it
    size_t begin = lines.size(), end = lines.size(), lastKey = lines.size();
    for (size_t i = 0; i < lines.size(); ++i) {
        const std::optional<std::wstring> name = SectionName(Trim(lines[i]));
        if (!name)
            continue;
        if (begin != lines.size()) {
            end = i;
            break;
        }
        if (EqualNoCase(*name, section))
            begin = i, lastKey = i + 1;
    }

    if (!key) {
        if (begin != lines.size())
            lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(begin), lines.begin() + static_cast<std::ptrdiff_t>(end));
    } else if (begin == lines.size()) {
        if (value) {
            lines.push_back(L"[" + std::wstring(section) + L"]");
            lines.push_back(std::wstring(key) + L"=" + value);
        }
    } else {
        bool found = false;
        for (size_t i = begin + 1; i < end && !found; ++i) {
            const std::wstring line = Trim(lines[i]);
            if (line.empty() || line[0] == L';')
                continue;
            lastKey = i + 1;
            const size_t eq = line.find(L'=');
            if (eq == std::wstring::npos || !EqualNoCase(Trim(line.substr(0, eq)), key))
                continue;
            found = true;
            if (value)
                lines[i] = std::wstring(key) + L"=" + value;
            else
                lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(i));
        }
        if (!found && value)
            lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(lastKey), std::wstring(key) + L"=" + value);
    }

    std::wstring text;
    for (const std::wstring& line : lines)
        text += line + L"\r\n";
    std::ofstream file(std::filesystem::path(fileName), std::ios::binary | std::ios::trunc);
    const std::string bytes = x2::platform::WideToUtf8(text);
    file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(file);
}

} // namespace x2::platform
