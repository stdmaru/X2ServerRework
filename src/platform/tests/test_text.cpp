#include "x2/platform/ini.h"
#include "x2/platform/unicode.h"

#include <doctest/doctest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

using namespace x2::platform;

namespace {

const char kCp949[] = "\xB0\xA1\xB3\xAA\xB4\xD9"; // "가나다" in CP949

} // namespace

TEST_CASE("text files: CP949, UTF-8 and UTF-16 detection")
{
    CHECK(Cp949ToWide(kCp949) == L"가나다");
    CHECK(IsValidUtf8("abc \xEA\xB0\x80"));
    CHECK_FALSE(IsValidUtf8(kCp949));
    CHECK_FALSE(IsValidUtf8("\xEA\xB0")); // truncated

    CHECK(DecodeTextFile(std::string("v = ") + kCp949) == L"v = 가나다");
    CHECK(DecodeTextFile("\xEF\xBB\xBFv = \xEA\xB0\x80") == L"v = 가");
    CHECK(DecodeTextFile("v = \xEA\xB0\x80") == L"v = 가");
    CHECK(DecodeTextFile(std::string("\xFF\xFEv\0=\0", 6)) == L"v=");
}

TEST_CASE("ReadIniString reads the original CP949 config")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "x2_platform_test.ini";
    {
        std::ofstream out(path, std::ios::binary);
        out << "; comment\r\n[Version]\r\nInternal Version = " << kCp949 << "\r\n  service version=\"1.0\" \r\n[Other]\r\nN = 42\r\n";
    }
    const std::wstring file = path.wstring();

    wchar_t buf[64];
    CHECK(ReadIniString(L"version", L"INTERNAL VERSION", L"", buf, 64, file.c_str()) == 3);
    CHECK(std::wstring(buf) == L"가나다");
    ReadIniString(L"Version", L"Service Version", L"", buf, 64, file.c_str());
    CHECK(std::wstring(buf) == L"1.0");
    ReadIniString(L"Version", L"Missing", L"default", buf, 64, file.c_str());
    CHECK(std::wstring(buf) == L"default");
    CHECK(ReadIniString(L"Version", L"Internal Version", L"", buf, 2, file.c_str()) == 1); // truncated
    CHECK(ReadIniInt(L"Other", L"N", 7, file.c_str()) == 42);
    CHECK(ReadIniInt(L"Other", L"Missing", 7, file.c_str()) == 7);

    std::filesystem::remove(path);
}

TEST_CASE("WriteIniString adds, replaces and removes keys")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "x2_platform_write_test.ini";
    std::filesystem::remove(path);
    const std::wstring file = path.wstring();
    wchar_t buf[64];

    CHECK(WriteIniString(L"Ranking Refresh Time", L"1", L"100", file.c_str()));
    CHECK(WriteIniString(L"Ranking Refresh Time", L"2", L"200", file.c_str()));
    CHECK(WriteIniString(L"Other", L"N", L"7", file.c_str()));
    CHECK(WriteIniString(L"ranking refresh time", L"1", L"150", file.c_str())); // case-insensitive, replaced
    CHECK(WriteIniString(L"Ranking Refresh Time", L"3", L"300", file.c_str())); // added before [Other]

    ReadIniString(L"Ranking Refresh Time", L"1", L"", buf, 64, file.c_str());
    CHECK(std::wstring(buf) == L"150");
    CHECK(ReadIniInt(L"Ranking Refresh Time", L"3", 0, file.c_str()) == 300);
    CHECK(ReadIniInt(L"Other", L"N", 0, file.c_str()) == 7);
    {
        std::ifstream in(path, std::ios::binary);
        const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        CHECK(text == "[Ranking Refresh Time]\r\n1=150\r\n2=200\r\n3=300\r\n[Other]\r\nN=7\r\n");
    }

    CHECK(WriteIniString(L"Ranking Refresh Time", L"2", nullptr, file.c_str()));
    CHECK(ReadIniInt(L"Ranking Refresh Time", L"2", -1, file.c_str()) == static_cast<UINT>(-1));
    CHECK(WriteIniString(L"Other", nullptr, nullptr, file.c_str()));
    CHECK(ReadIniInt(L"Other", L"N", -1, file.c_str()) == static_cast<UINT>(-1));
    CHECK(ReadIniInt(L"Ranking Refresh Time", L"3", 0, file.c_str()) == 300);

    std::filesystem::remove(path);
}
