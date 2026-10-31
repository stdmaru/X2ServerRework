// Private profile (.ini) reading, as used by the original (GetPrivateProfileString*).
// Same lookup rules as Windows (section and key case-insensitive, value trimmed, surrounding quotes removed,
// `;` comment lines), but the file may be UTF-16, UTF-8 or CP949 (see DecodeTextFile) on every platform:
// the Windows API reads ANSI files in the system code page, which is CP949 only on a Korean Windows.
#pragma once

#include "x2/platform/types.h"

namespace x2::platform {

// Copies the value (or the default) into dst, truncating to size - 1; returns the characters copied.
DWORD ReadIniString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultValue,
                    wchar_t* dst, DWORD size, const wchar_t* fileName);
UINT  ReadIniInt(const wchar_t* section, const wchar_t* key, INT defaultValue, const wchar_t* fileName);
// Sets key = value in the section, adding the section and key when missing (WritePrivateProfileString).
// A null value removes the key, a null key removes the section. The file is rewritten as UTF-8.
bool  WriteIniString(const wchar_t* section, const wchar_t* key, const wchar_t* value, const wchar_t* fileName);

} // namespace x2::platform

#if !defined(_WIN32)
inline DWORD GetPrivateProfileStringW(const wchar_t* section, const wchar_t* key, const wchar_t* defaultValue,
                                      wchar_t* dst, DWORD size, const wchar_t* fileName)
{
    return x2::platform::ReadIniString(section, key, defaultValue, dst, size, fileName);
}
inline UINT GetPrivateProfileIntW(const wchar_t* section, const wchar_t* key, INT defaultValue, const wchar_t* fileName)
{
    return x2::platform::ReadIniInt(section, key, defaultValue, fileName);
}
inline BOOL WritePrivateProfileStringW(const wchar_t* section, const wchar_t* key, const wchar_t* value, const wchar_t* fileName)
{
    return x2::platform::WriteIniString(section, key, value, fileName) ? TRUE : FALSE;
}
#endif // !_WIN32
