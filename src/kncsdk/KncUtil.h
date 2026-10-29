#pragma once

// Narrow strings are UTF-8 on every platform (the original used the Windows ANSI code page, CP_ACP).

#define BEGIN_NAMESPACE( name ) namespace name {
#define END_NAMESPACE }
#define NUM_ELEMENTS(x) ( sizeof((x)) / sizeof(0[x]) ) // vector등에 사용시 컴파일에러를 내도록 의도된 매크로입니다.

#include "KncMacro.h"
#include "x2/platform/win32.h"

#include <cmath>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

BEGIN_NAMESPACE( KncUtil )

// Message of the last OS error (GetLastError() on Windows, errno elsewhere).
std::wstring GetLastErrMsg();

// MD5 문자열 얻기 (lowercase hex)
std::string GetMD5( const std::string& strIN_ );

// 실행파일 명을 알려준다. (file name without directory and extension)
void GetAppName( OUT std::wstring& strAppName );

// Build time of the running executable, "YYYY-MM-DD HH:MM:SS". The original read the link time from the
// PE header; here it is the executable's modification time, which is the same for a fresh build.
bool GetTimeStampByNameW( OUT std::wstring& strTimeStamp );
bool GetTimeStampByNameA( OUT std::string& strTimeStamp );

// hex 문자열을 숫자로 바꿔준다. ("0x" prefix allowed; invalid digits count as 0, as in the original)
inline long hstol( char* str )
{
    if( str == NULL )
        return 0L;
    const char* p = str;
    if( p[0] == '0' && ( p[1] == 'x' || p[1] == 'X' ) && p[2] != '\0' )
        p += 2;
    long ret = 0L;
    for( ; *p; ++p )
    {
        long index = 0;
        if( '0' <= *p && *p <= '9' )      index = *p - '0';
        else if( 'a' <= *p && *p <= 'f' ) index = *p - 'a' + 10;
        else if( 'A' <= *p && *p <= 'F' ) index = *p - 'A' + 10;
        ret = ret * 16 + index;
    }
    return ret;
}

inline time_t TimeToInt( CTime& cTime_ )
{
    struct tm tmBuff;
    cTime_.GetLocalTm( &tmBuff );
    return ::mktime( &tmBuff );
}

std::string  toNarrowString( const wchar_t* pStr, int len = -1 );
inline std::string toNarrowString( const std::wstring& str ) { return toNarrowString( str.c_str(), static_cast<int>( str.size() ) ); }

std::wstring toWideString( const char* pStr, int len = -1 );
inline std::wstring toWideString( const std::string& str ) { return toWideString( str.c_str(), static_cast<int>( str.size() ) ); }

// Narrow strings are already UTF-8, so this is the same as toWideString().
inline std::wstring toWideString_UTF8( const char* pStr, int len = -1 ) { return toWideString( pStr, len ); }
inline std::wstring toWideString_UTF8( const std::string& str ) { return toWideString( str ); }

// 날짜 문자열은 항상 "2008-01-22 11:16:00" 패턴
bool ConvertStringToCTime( const std::wstring& wstrDate, CTime& ctDate );

// 0 .. iMaxNum-1 in random order (uses rand(), like the original)
std::vector< int > GetRandomNumberList( const int iMaxNum );

template< class T >
inline std::vector< T > NumberShuffle( const std::vector< T >& vecSource )
{
    std::vector< T > vecDataSource = vecSource;
    std::vector< T > vecResult;
    vecResult.reserve( vecDataSource.size() );
    while( !vecDataSource.empty() )
    {
        const int iRandomRet = static_cast<int>( rand() % static_cast<int>( vecDataSource.size() ) );
        vecResult.push_back( vecDataSource[iRandomRet] );
        vecDataSource.erase( vecDataSource.begin() + iRandomRet );
    }
    return vecResult;
}

inline std::wstring GetCurrentTimeString()
{
    return CTime::GetCurrentTime().Format( L"%Y-%m-%d %H:%M:%S" ).GetString();
}

// Removes everything up to and including the first ';'. Unchanged when there is no ';' or it is the
// first character (the original got there through 32-bit npos overflow). `flag` is ignored, as in the original.
inline std::wstring EraseFrontWord( const std::wstring& wstrWord, const wchar_t flag )
{
    (void)flag;
    std::wstring wstrResult = wstrWord;
    const size_t index = wstrResult.find( L';' );
    if( index != 0 && index != std::wstring::npos )
        wstrResult.erase( 0, index + 1 );
    return wstrResult;
}

END_NAMESPACE
