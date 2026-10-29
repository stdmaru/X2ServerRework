#include "Serializer.h"
#include "SerBuffer.h"
#include "ByteOrder.h"
#include "x2/platform/unicode.h"

#include <bit>
#include <cassert>
#include <cstring>
#include <limits>
#include <vector>

static_assert(std::endian::native == std::endian::little, "ByteOrder.h assumes a little-endian host");


KSerializer::KSerializer()
    : m_buffer(0),
      m_tagsEnabled(false)
{
  // empty
}

KSerializer::~KSerializer()
{
    m_buffer = 0;
}

bool KSerializer::BeginWriting(KSerBuffer* pkBuf, bool tagging)
{
    assert(pkBuf != 0);
    m_tagsEnabled = tagging;
    m_buffer = pkBuf;
    return true;
}

bool KSerializer::EndWriting()
{
    m_buffer = 0;
    return true;
}

bool KSerializer::BeginReading(KSerBuffer* pkBuf, bool tagging)
{
    assert(pkBuf != 0);
    m_tagsEnabled = tagging;
    m_buffer = pkBuf;
    return true;
}

bool KSerializer::EndReading()
{
    m_buffer = 0;
    return true;
}

DWORD KSerializer::GetReadLength() const
{
	return m_buffer->GetReadLength();
}


bool KSerializer::Put(char x)
{
    return WriteNumber(x, eTAG_CHAR);
}

bool KSerializer::Get(char& x)
{
    return ReadNumber(x, eTAG_CHAR);
}

// wchar_t: one UTF-16 code unit, little-endian (raw MSVC layout, not byte-swapped like other numbers).
// Code points outside the BMP cannot be sent as a single wchar_t and become U+FFFD.
bool KSerializer::Put(wchar_t x)
{
    std::uint32_t cp = static_cast<std::uint32_t>(x);
    if constexpr (sizeof(wchar_t) == 4) {
        if (cp > 0xFFFF)
            cp = 0xFFFD;
    }
    const USHORT unit = static_cast<USHORT>(cp);
    return WriteNumber(unit, eTAG_WCHAR);
}

bool KSerializer::Get(wchar_t& x)
{
    USHORT unit = 0;
    bool ret = ReadNumber(unit, eTAG_WCHAR);
    x = static_cast<wchar_t>(unit);
    return ret;
}

bool KSerializer::Put(unsigned char x)
{
    return WriteNumber(x, eTAG_UCHAR);
}

bool KSerializer::Get(unsigned char& x)
{
    return ReadNumber(x, eTAG_UCHAR);
}

bool KSerializer::Put(short x)
{
    x = HTON_I16(x);
    return WriteNumber(x, eTAG_SHORT);
}

bool KSerializer::Get(short& x)
{
    bool ret = ReadNumber(x, eTAG_SHORT);
    x = NTOH_I16(x);
    return ret;
}

bool KSerializer::Put(unsigned short x)
{
    x = HTON_I16(x);
    return WriteNumber(x, eTAG_USHORT);
}

bool KSerializer::Get(unsigned short& x)
{
    bool ret = ReadNumber(x, eTAG_USHORT);
    x = NTOH_I16(x);
    return ret;
}

bool KSerializer::Put(int x)
{
    x = HTON_I32(x);
    return WriteNumber(x, eTAG_INT);
}

bool KSerializer::Get(int& x)
{
    bool ret = ReadNumber(x, eTAG_INT);
    x = NTOH_I32(x);
    return ret;
}

bool KSerializer::Put(unsigned int x)
{
    std::uint32_t v = HTON_I32(static_cast<std::uint32_t>(x));
    return WriteNumber(v, eTAG_DWORD);
}

bool KSerializer::Get(unsigned int& x)
{
    std::uint32_t v = 0;
    bool ret = ReadNumber(v, eTAG_DWORD);
    x = static_cast<unsigned int>(NTOH_I32(v));
    return ret;
}

bool KSerializer::Put(long x)
{
    if (x < std::numeric_limits<std::int32_t>::min() || x > std::numeric_limits<std::int32_t>::max())
        return false;
    return Put(static_cast<int>(x));
}

bool KSerializer::Get(long& x)
{
    int v = 0;
    bool ret = Get(v);
    x = v;
    return ret;
}

bool KSerializer::Put(unsigned long x)
{
    if (x > std::numeric_limits<std::uint32_t>::max())
        return false;
    return Put(static_cast<unsigned int>(x));
}

bool KSerializer::Get(unsigned long& x)
{
    unsigned int v = 0;
    bool ret = Get(v);
    x = v;
    return ret;
}

bool KSerializer::Put(long long x)
{
    x = HTON_I64(x);
    return WriteNumber(x, eTAG_INT64);
}

bool KSerializer::Get(long long& x)
{
    bool ret = ReadNumber(x, eTAG_INT64);
    x = NTOH_I64(x);
    return ret;
}

bool KSerializer::Put(unsigned long long x)
{
    x = HTON_I64(x);
    return WriteNumber(x, eTAG_UINT64);
}

bool KSerializer::Get(unsigned long long& x)
{
    bool ret = ReadNumber(x, eTAG_UINT64);
    x = NTOH_I64(x);
    return ret;
}


bool KSerializer::Put(float x)
{
    x = HTON_F32(x);
    return WriteNumber(x, eTAG_FLOAT);
}

bool KSerializer::Get(float& x)
{
    bool ret = ReadNumber(x, eTAG_FLOAT);
    x = NTOH_F32(x);
    return ret;
}

bool KSerializer::Put(double x)
{
    x = HTON_F64(x);
    return WriteNumber(x, eTAG_DOUBLE);
}

bool KSerializer::Get(double& x)
{
    bool ret = ReadNumber(x, eTAG_DOUBLE);
    x = NTOH_F64(x);
    return ret;
}

bool KSerializer::Put(bool x)
{
    UCHAR val = x ? 1 : 0;
    return WriteNumber(val, eTAG_BOOL);
}

bool KSerializer::Get(bool& x)
{
    UCHAR val;
    bool ret = ReadNumber(val, eTAG_BOOL);
    x = (val == 1);
    return ret;
}

bool KSerializer::Put(const std::string& x)
{
    // tag
    if( !WriteTag(eTAG_STRING) )
        return false;

    DWORD size = (DWORD)x.size();
    if( !Put(size) )
        return false;
    // data (only if there is some)
    return (size == 0) || WriteBytes(x.data(), size);
}

bool KSerializer::Put(const char* x)
{
    // could just return Put(string(x)), but that makes a copy..
    assert(x != 0);
    // tag
    if( !WriteTag(eTAG_STRING) )
        return false;

    // size
    DWORD size = (DWORD)strlen(x);
    if( !Put(size) )
        return false;
    // data (only if there is some)
    return (size == 0) || WriteBytes(x, size);
}

bool KSerializer::Get(std::string& x)
{
    // erase contents
    x.erase();

    // tag
    if( !ReadAndCheckTag(eTAG_STRING) )
    return false;

    // size
    DWORD size;
    if( !Get(size) )
        return false;

    if( m_buffer != NULL && m_buffer->GetReadLength() < size )
        return false;

    // data (only if there is some)
    if( size == 0 )
        return true;
    // since we can't get a pointer to string's internal data, have to
    // use a temp buffer and copy it...
    char* tempString = new char[size];
    bool ok = ReadBytes(tempString, size);
    if (ok)
    x.assign(tempString, size);
    delete [] tempString;
    return ok;
}


// wstring: DWORD byte length + UTF-16LE code units (the raw std::wstring of the MSVC build).
bool KSerializer::PutUtf16(const wchar_t* x, size_t len)
{
    if( !WriteTag(eTAG_WSTRING) )
        return false;

    const std::u16string u16 = x2::platform::WideToUtf16(std::wstring_view(x, len));
    if( u16.size() > std::numeric_limits<DWORD>::max() / 2 )
        return false;
    DWORD size = static_cast<DWORD>(u16.size() * 2);
    if( !Put(size) )
        return false;
    if( size == 0 )
        return true;

    std::vector<unsigned char> bytes(size);
    for( size_t i = 0; i < u16.size(); ++i )
    {
        bytes[i * 2]     = static_cast<unsigned char>(u16[i] & 0xFF);
        bytes[i * 2 + 1] = static_cast<unsigned char>(u16[i] >> 8);
    }
    return WriteBytes(bytes.data(), size);
}

bool KSerializer::Put(const std::wstring& x)
{
    return PutUtf16(x.data(), x.size());
}

bool KSerializer::Put(const wchar_t* x)
{
    assert(x != 0);
    return PutUtf16(x, wcslen(x));
}

bool KSerializer::Get(std::wstring& x)
{
    x.clear();

    if( !ReadAndCheckTag(eTAG_WSTRING) )
        return false;

    DWORD size;
    if( !Get(size) )
        return false;

    if( size % 2 != 0 )
        return false;

    if( m_buffer != NULL && m_buffer->GetReadLength() < size )
        return false;

    if( size == 0 )
        return true;

    std::vector<unsigned char> bytes(size);
    if( !ReadBytes(bytes.data(), size) )
        return false;
    std::u16string u16(size / 2, u'\0');
    for( size_t i = 0; i < u16.size(); ++i )
        u16[i] = static_cast<char16_t>(bytes[i * 2] | (bytes[i * 2 + 1] << 8));
    x = x2::platform::Utf16ToWide(u16);
    return true;
}

/**
 * Raw byte arrays
 **/
bool KSerializer::PutRaw(const void* buf, DWORD len)
{
    assert(buf != 0);
    assert(len > 0);

    // note: no length field
    return WriteTag(eTAG_RAWBYTES) && WriteBytes(buf, len);
}

bool KSerializer::GetRaw(void* buf, DWORD len)
{
    assert(buf != 0);
    assert(len > 0);

    // note: no length field
    return ReadAndCheckTag(eTAG_RAWBYTES) && ReadBytes(buf, len);
}


/**
 * Write a tag
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/
bool KSerializer::WriteTag(const SerializeTag tag)
{
    assert((tag >= 0) && (tag <= 255)); // fit inside uint8

    UCHAR tmp = static_cast<UCHAR>(tag);
    return !m_tagsEnabled || WriteBytes(&tmp, 1);
}

/**
 * Read a tag, and check it against an expected value
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/
bool KSerializer::ReadAndCheckTag(SerializeTag expectedTag)
{
  UCHAR tmp;
  return !m_tagsEnabled || (ReadBytes(&tmp, 1) && (tmp == expectedTag));
}


/**
 * Write to the output Buffer
 **/
bool KSerializer::WriteBytes(const void* buf, DWORD len)
{
    assert(buf != 0);
    assert(len > 0);

    return m_buffer->Write(buf, len);
}

/**
 * Read from the input Buffer
 **/
bool KSerializer::ReadBytes(void* buf, DWORD len)
{
    assert(buf != 0);
    assert(len > 0);

    return m_buffer->Read(buf, len);
}
