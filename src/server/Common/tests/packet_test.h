// Helpers for the packet tests of the server's Common.
#pragma once

#include <Serializer/SerBuffer.h>
#include <Serializer/Serializer.h>

#include <doctest/doctest.h>

#include <string>
#include <type_traits>
#include <vector>

using Bytes = std::vector<unsigned char>;

inline Bytes BufferBytes(const KSerBuffer& buf)
{
    const auto* p = static_cast<const unsigned char*>(buf.GetData());
    return Bytes(p, p + buf.GetLength());
}

template <typename T>
Bytes Serialize(const T& value)
{
    KSerBuffer buf;
    KSerializer ks;
    ks.BeginWriting(&buf);
    const bool ok = ks.Put(value);
    ks.EndWriting();
    REQUIRE(ok);
    return BufferBytes(buf);
}

// Put -> Get -> Put -> Get -> Put of a value-initialized T. Some packet constructors leave members
// uninitialized, so the first bytes may hold garbage (a bool of 2...): only their size is compared; from the
// second Put on the values are normalized and the bytes must match. Every Get must read all bytes.
// Catches Put/Get pairs that disagree on a field's type or size.
template <typename T>
void CheckRoundTrip(const char* name)
{
    static_assert(std::is_default_constructible_v<T>, "packet types are default-constructible");
    INFO("type: " << std::string(name));

    auto put = [](const T& value, Bytes& bytes) {
        KSerBuffer buf;
        KSerializer ks;
        ks.BeginWriting(&buf);
        const bool ok = ks.Put(value);
        ks.EndWriting();
        bytes = BufferBytes(buf);
        return ok;
    };
    auto get = [](const Bytes& bytes, T& value) {
        KSerBuffer buf;
        buf.Write(bytes.data(), static_cast<DWORD>(bytes.size()));
        KSerializer ks;
        ks.BeginReading(&buf);
        const bool ok = ks.Get(value);
        ks.EndReading();
        return ok && buf.GetReadLength() == 0;
    };

    const T in{};
    Bytes first, second, third;
    if (!put(in, first)) {
        FAIL_CHECK("Put failed");
        return;
    }
    T out1{}, out2{};
    CHECK(get(first, out1));
    if (!put(out1, second)) {
        FAIL_CHECK("Put failed");
        return;
    }
    CHECK(get(second, out2));
    if (!put(out2, third)) {
        FAIL_CHECK("Put failed");
        return;
    }

    CHECK(second.size() == first.size());
    CHECK(third == second);
}
