#include "x2/platform/ini.h"
#include "x2/platform/unicode.h"

#include <cwctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>

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

std::optional<std::wstring> Lookup(const wchar_t* section, const wchar_t* key, const wchar_t* fileName)
{
    if (!section || !key || !fileName)
        return std::nullopt;
    std::ifstream file(std::filesystem::path(fileName), std::ios::binary); // wide path on Windows too
    if (!file)
        return std::nullopt;
    const std::string bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::wistringstream stm(x2::platform::DecodeTextFile(bytes));
    std::wstring line;
    bool inSection = false;
    while (std::getline(stm, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == L';')
            continue;
        if (line[0] == L'[') {
            const size_t end = line.find(L']');
            inSection = EqualNoCase(Trim(line.substr(1, end == std::wstring::npos ? std::wstring::npos : end - 1)), section);
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

} // namespace x2::platform
