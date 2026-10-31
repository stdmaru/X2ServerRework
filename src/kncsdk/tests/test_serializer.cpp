// Wire format of KSerializer. Expected bytes follow the original MSVC x86 build (docs/original/protocolo.md):
// numbers big-endian, wchar_t/wstring as raw UTF-16LE, sizes as 4-byte DWORD.
#include "Serializer/SerBuffer.h"
#include "Serializer/SerMap.h"
#include "Serializer/SerPair.h"
#include "Serializer/SerVector.h"
#include "Serializer/Serializer.h"

#include <doctest/doctest.h>

#include <climits>
#include <map>
#include <string>
#include <vector>

namespace {

using Bytes = std::vector<unsigned char>;

template <typename... T>
Bytes Write(bool tagging, const T&... values)
{
    KSerBuffer buf;
    KSerializer ser;
    ser.BeginWriting(&buf, tagging);
    const bool ok = (ser.Put(values) && ...);
    ser.EndWriting();
    REQUIRE(ok);
    const auto* p = static_cast<const unsigned char*>(buf.GetData());
    return Bytes(p, p + buf.GetLength());
}

template <typename T>
T RoundTrip(const T& in, bool tagging = false)
{
    KSerBuffer buf;
    KSerializer ser;
    ser.BeginWriting(&buf, tagging);
    REQUIRE(ser.Put(in));
    ser.EndWriting();
    T out{};
    ser.BeginReading(&buf, tagging);
    REQUIRE(ser.Get(out));
    ser.EndReading();
    return out;
}

} // namespace

TEST_CASE("numbers are big-endian with fixed sizes")
{
    CHECK(Write(false, static_cast<short>(0x0102)) == Bytes{ 0x01, 0x02 });
    CHECK(Write(false, 0x01020304) == Bytes{ 0x01, 0x02, 0x03, 0x04 });
    CHECK(Write(false, static_cast<DWORD>(0xA0B0C0D0u)) == Bytes{ 0xA0, 0xB0, 0xC0, 0xD0 });
    CHECK(Write(false, static_cast<UINT>(5)) == Bytes{ 0, 0, 0, 5 });
    CHECK(Write(false, static_cast<INT64>(0x0102030405060708LL)) == Bytes{ 1, 2, 3, 4, 5, 6, 7, 8 });
    CHECK(Write(false, 1.0f) == Bytes{ 0x3F, 0x80, 0x00, 0x00 });
    CHECK(Write(false, 1.0) == Bytes{ 0x3F, 0xF0, 0, 0, 0, 0, 0, 0 });
    CHECK(Write(false, true, false) == Bytes{ 1, 0 });
    CHECK(Write(false, 'x', static_cast<unsigned char>(0xFE)) == Bytes{ 'x', 0xFE });
}

TEST_CASE("long is 4 bytes on the wire on every platform")
{
    CHECK(Write(false, 7L) == Bytes{ 0, 0, 0, 7 });
    CHECK(Write(false, 7UL) == Bytes{ 0, 0, 0, 7 });
    CHECK(RoundTrip(-123456L) == -123456L);
    if constexpr (sizeof(long) == 8) {
        KSerBuffer buf;
        KSerializer ser;
        ser.BeginWriting(&buf);
        CHECK_FALSE(ser.Put(static_cast<long>(LONG_MAX)));
    }
}

TEST_CASE("wide characters and strings are UTF-16LE")
{
    CHECK(Write(false, L'가') == Bytes{ 0x00, 0xAC });
    // DWORD byte length (big-endian) + code units little-endian
    CHECK(Write(false, std::wstring(L"a한")) == Bytes{ 0, 0, 0, 4, 0x61, 0x00, 0x5C, 0xD5 });
    CHECK(Write(false, std::wstring(L"\U0001F600")) == Bytes{ 0, 0, 0, 4, 0x3D, 0xD8, 0x00, 0xDE });
    CHECK(Write(false, std::wstring()) == Bytes{ 0, 0, 0, 0 });
    CHECK(Write(false, std::string("ab")) == Bytes{ 0, 0, 0, 2, 'a', 'b' });

    const std::wstring text = L"한글 \U0001F600 test";
    CHECK(RoundTrip(text) == text);
    CHECK(RoundTrip(std::string("narrow")) == "narrow");
}

TEST_CASE("tags are written only in tagging mode")
{
    CHECK(Write(true, 1) == Bytes{ eTAG_INT, 0, 0, 0, 1 });
    CHECK(Write(true, static_cast<DWORD>(1)) == Bytes{ eTAG_DWORD, 0, 0, 0, 1 });
    CHECK(Write(true, std::wstring(L"a")) == Bytes{ eTAG_WSTRING, eTAG_DWORD, 0, 0, 0, 2, 0x61, 0x00 });
    CHECK(RoundTrip(std::wstring(L"tagged"), true) == L"tagged");
}

TEST_CASE("STL containers")
{
    const std::vector<int> v{ 1, 2 };
    CHECK(Write(false, v) == Bytes{ 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 2 });
    CHECK(RoundTrip(v) == v);

    const std::map<std::wstring, int> m{ { L"a", 1 }, { L"b", 2 } };
    CHECK(RoundTrip(m) == m);

    const std::pair<int, std::wstring> pr{ 3, L"x" };
    CHECK(RoundTrip(pr) == pr);
}

TEST_CASE("truncated input fails instead of reading past the end")
{
    KSerBuffer buf;
    const unsigned char bytes[] = { 0, 0, 0, 10, 'a' }; // claims 10 bytes, has 1
    buf.Write(bytes, sizeof(bytes));
    KSerializer ser;
    ser.BeginReading(&buf);
    std::string s;
    CHECK_FALSE(ser.Get(s));
}

TEST_CASE("KSerBuffer compression round trip")
{
    KSerBuffer buf;
    KSerializer ser;
    ser.BeginWriting(&buf);
    const std::wstring payload(500, L'z');
    REQUIRE(ser.Put(payload));
    const DWORD rawSize = buf.GetLength();
    REQUIRE(buf.Compress());
    CHECK(buf.GetLength() < rawSize);
    // the original size is stored first as a 4-byte DWORD (raw, little-endian)
    const auto* p = static_cast<const unsigned char*>(buf.GetData());
    CHECK((p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24)) == static_cast<int>(rawSize));
    REQUIRE(buf.UnCompress());
    CHECK(buf.GetLength() == rawSize);

    std::wstring out;
    ser.BeginReading(&buf);
    REQUIRE(ser.Get(out));
    CHECK(out == payload);
}

TEST_CASE("KSerBuffer compression of an empty payload")
{
    // events without data are compressed too: size 0 + an empty zlib stream (the GameServer TCP relay got these)
    KSerBuffer buf;
    REQUIRE(buf.Compress());
    CHECK(buf.GetLength() > sizeof(DWORD));
    REQUIRE(buf.UnCompress());
    CHECK(buf.GetLength() == 0);
}
