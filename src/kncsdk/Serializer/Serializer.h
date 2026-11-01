#ifndef SERIALIZER_H
#define SERIALIZER_H "$Id: Serializer.h,v 1.5 2003/04/19 03:28:43 cvs Exp $"


#include "BaseTypes.h"
#include "SerDefs.h"

#include <string>

class KSerBuffer;                  // fwd

class KSerializer
{
public:
    KSerializer();
    virtual ~KSerializer();

    bool BeginWriting(KSerBuffer* pkBuf, bool tagging = false);
    bool EndWriting();

    bool BeginReading(KSerBuffer* pkBuf, bool tagging = false);
    bool EndReading();

	DWORD GetReadLength() const;

    // user-defined class. The helpers are called unqualified with a SerializeHelper::Adl argument so that
    // argument-dependent lookup also finds overloads declared after this header (packets), as MSVC did.
    template <typename T>
    bool Put(const T& obj)
    {
        SerializeTag tag = ComputeTag(obj, SerializeHelper::Adl());
        return WriteTag(tag) &&
               PutInto(*this, obj, SerializeHelper::Adl());
    }

    template <typename T>
    bool Get(T& obj)
    {
        SerializeTag tag = ComputeTag(obj, SerializeHelper::Adl());
        return ReadAndCheckTag(tag) &&
               GetFrom(*this, obj, SerializeHelper::Adl());
    }

    // builtin types. Wire sizes are fixed (the original MSVC x86 layout): int/unsigned/long/unsigned long
    // are 4 bytes (long must fit in 32 bits), wchar_t is 2 bytes (UTF-16 code unit), see docs/porting/kncsdk.md.
    bool Put(char x);
    bool Get(char& x);
    bool Put(wchar_t x);
    bool Get(wchar_t& x);
    bool Put(unsigned char x);
    bool Get(unsigned char& x);
    bool Put(short x);
    bool Get(short& x);
    bool Put(unsigned short x);
    bool Get(unsigned short& x);
    bool Put(int x);
    bool Get(int& x);
    bool Put(unsigned int x);           // DWORD tag (as UINT in the original)
    bool Get(unsigned int& x);
    bool Put(long x);                   // INT tag; fails if out of 32-bit range
    bool Get(long& x);
    bool Put(unsigned long x);          // DWORD tag (DWORD is unsigned long on Windows)
    bool Get(unsigned long& x);
    bool Put(long long x);              // INT64
    bool Get(long long& x);
    bool Put(unsigned long long x);     // UINT64
    bool Get(unsigned long long& x);
    bool Put(float x);
    bool Get(float& x);
    bool Put(double x);
    bool Get(double& x);
    bool Put(bool x);
    bool Get(bool& x);

    // string
    bool Put(const std::string& x);
    bool Put(const char* x);
    bool Get(std::string& x);

    // wstring: byte length (DWORD) + UTF-16LE
    bool Put(const std::wstring& x);
    bool Put(const wchar_t* x);
    bool Get(std::wstring& x);

    // raw byte array of known size
    bool PutRaw(const void* buf, DWORD len);
    bool GetRaw(void* buf, DWORD len);

    // array
    template <typename T>
    bool PutArray(const T* objs, DWORD numItems)
    {
        // write tag
        if( !WriteTag(eTAG_ARRAY) )
            return false;

        // store size
        if( !Put(numItems) )
            return false;
        // store each element
        for( DWORD i = 0; i < numItems; ++i )
            if( !Put(objs[i]) )
                return false;
        return true;
    }

    template <typename T>
    bool GetArray(T* objs, DWORD& numInOut)
    {
        // check tag
        if( !ReadAndCheckTag(eTAG_ARRAY) )
            return false;

        DWORD numItems;
        if( !Get(numItems) )
            return false;
        // too many to read?
        if( numItems > numInOut )
            return false;
        // read each element
        for( DWORD i = 0; i < numItems; ++i )
            if( !Get(objs[i]) )
                return false;
        numInOut = numItems;
        return true;
    }

protected:
    template <typename T>
    inline bool WriteNumber(T x, SerializeTag tag)
    {
        return WriteTag(tag) && WriteBytes(&x, sizeof(T));
    }

    template <typename T>
    inline bool ReadNumber(T& x, SerializeTag tag)
    {
        return ReadAndCheckTag(tag) && ReadBytes(&x, sizeof(T));
    }

    // tags
    bool WriteTag(SerializeTag tag);
    bool ReadAndCheckTag(SerializeTag expectedTag);

    // low-level Buffer access
    bool WriteBytes(const void* buf, DWORD len);
    bool ReadBytes(void* buf, DWORD len);

    bool PutUtf16(const wchar_t* x, size_t len);

private:
    KSerializer(const KSerializer&) = delete;
    void operator=(const KSerializer&) = delete;

protected:
    KSerBuffer*     m_buffer;
    bool            m_tagsEnabled;

};

#endif
