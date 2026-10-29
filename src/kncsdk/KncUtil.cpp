#include "KncUtil.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <system_error>

#if !defined(_WIN32)
#include <unistd.h>
#endif

namespace
{
    std::filesystem::path ExecutablePath()
    {
#if defined(_WIN32)
        wchar_t buf[MAX_PATH] = {};
        ::GetModuleFileNameW( NULL, buf, MAX_PATH );
        return std::filesystem::path( buf );
#else
        std::error_code ec;
        return std::filesystem::read_symlink( "/proc/self/exe", ec );
#endif
    }
}

BEGIN_NAMESPACE( KncUtil )

std::wstring GetLastErrMsg()
{
#if defined(_WIN32)
    const int code = static_cast<int>( ::GetLastError() );
#else
    const int code = errno;
#endif
    const std::string msg = std::system_category().message( code );
    if( msg.empty() )
        return L"에러값 얻기 실패.!";
    return toWideString( msg );
}

std::string GetMD5( const std::string& strIN_ )
{
    CryptoPP::byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];
    CryptoPP::Weak::MD5 md5Obj;
    md5Obj.Update( reinterpret_cast<const CryptoPP::byte*>( strIN_.data() ), strIN_.length() );
    md5Obj.Final( digest );

    static const char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve( sizeof( digest ) * 2 );
    for( const CryptoPP::byte b : digest )
    {
        out.push_back( kHex[b >> 4] );
        out.push_back( kHex[b & 0xF] );
    }
    return out;
}

void GetAppName( OUT std::wstring& strAppName )
{
    // Extract from the last separator to the first '.'
    std::wstring name = ExecutablePath().filename().wstring();
    const size_t dot = name.find( L'.' );
    strAppName = dot == std::wstring::npos ? name : name.substr( 0, dot );
}

bool GetTimeStampByNameW( OUT std::wstring& strTimeStamp )
{
    std::string ts;
    if( !GetTimeStampByNameA( ts ) )
        return false;
    strTimeStamp = toWideString( ts );
    return true;
}

bool GetTimeStampByNameA( OUT std::string& strTimeStamp )
{
    std::error_code ec;
    const auto ftime = std::filesystem::last_write_time( ExecutablePath(), ec );
    if( ec )
        return false;
    const auto sys = std::chrono::clock_cast<std::chrono::system_clock>( ftime );
    const CTime time( static_cast<std::int64_t>( std::chrono::system_clock::to_time_t( sys ) ) );
    strTimeStamp = toNarrowString( time.Format( L"%Y-%m-%d %H:%M:%S" ).GetString() );
    return true;
}

std::string toNarrowString( const wchar_t* pStr, int len )
{
    if( pStr == NULL )
        return std::string();
    if( len != -1 && len <= 0 ) // florist. 0이어도 통과시키지 말자.
        return std::string();
    const std::wstring_view view = len == -1 ? std::wstring_view( pStr ) : std::wstring_view( pStr, static_cast<size_t>( len ) );
    return x2::platform::WideToUtf8( view );
}

std::wstring toWideString( const char* pStr, int len )
{
    if( pStr == NULL )
        return std::wstring();
    if( len != -1 && len <= 0 ) // florist. 0이어도 통과시키지 말자.
        return std::wstring();
    const std::string_view view = len == -1 ? std::string_view( pStr ) : std::string_view( pStr, static_cast<size_t>( len ) );
    return x2::platform::Utf8ToWide( view );
}

bool ConvertStringToCTime( const std::wstring& wstrDate, CTime& ctDate )
{
    if( wstrDate.size() < 19 )
        return false;

    // year, month, day, hour, min, sec
    std::wstring wstrToken[6];
    int iNowToken = 0;
    for( const wchar_t c : wstrDate )
    {
        if( c == L'-' || c == L' ' || c == L':' )
            ++iNowToken;
        else if( ( c >= L'0' && c <= L'9' ) || c == L'.' )
            wstrToken[iNowToken] += c;

        if( iNowToken >= 6 )
            break;
    }

    int iToken[6];
    for( int i = 0; i < 6; ++i )
        iToken[i] = _wtoi( wstrToken[i].c_str() );

    // CTime에서 exception발생 안시키도록 밖에서 따로 검사
    if( iToken[2] < 1 || iToken[2] > 31 )
        return false;
    if( iToken[1] < 1 || iToken[1] > 12 )
        return false;
    if( iToken[0] < 1900 )
        return false;

    // 초기값 (1900-01-01 00:00:00) 도 false
    if( iToken[0] == 1900 && iToken[1] == 1 && iToken[2] == 1 && iToken[3] == 0 && iToken[4] == 0 && iToken[5] == 0 )
        return false;

    ctDate = CTime( iToken[0], iToken[1], iToken[2], iToken[3], iToken[4], iToken[5] );
    return true;
}

std::vector< int > GetRandomNumberList( const int iMaxNum )
{
    std::vector< int > vecSource;
    for( int iNum = 0; iNum < iMaxNum; ++iNum )
        vecSource.push_back( iNum );
    return NumberShuffle( vecSource );
}

END_NAMESPACE
