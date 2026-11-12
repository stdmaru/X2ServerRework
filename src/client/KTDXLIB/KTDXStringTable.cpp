#include "stdafx.h"

#include <KncUtil.h>
#include "KTDXStringTable.h"

#include "x2/platform/string.h"
#include "x2/platform/unicode.h"

#include <filesystem>
#include <fstream>
#include <iterator>

ImplementSingleton( CKTDXStringTable );

CKTDXStringTable::CKTDXStringTable()
{
    m_vecGeneralStringTable.clear();
    m_wstrEmpty.clear();
	m_bEnableDebugInfo = false;
}

CKTDXStringTable::~CKTDXStringTable()
{
}

bool CKTDXStringTable::InitializeStringTable( std::wstring& wstrGeneralStringFileName, std::wstring& /*wstrScriptStringFileName*/ )
{
    return LoadStringTable( wstrGeneralStringFileName, m_vecGeneralStringTable );
}

std::wstring CKTDXStringTable::GetReplacedStringV( int iID, const char* szParamTypeList, ... )
{
    int iTableSize = ( int )m_vecGeneralStringTable.size();

    ASSERT( iID >= 0 && iID < iTableSize );

    if( iID < 0 || iID >= iTableSize )
    {
        return m_wstrEmpty;
    }

    std::wstring wstrReplacedString;
	if(m_bEnableDebugInfo)
		wstrReplacedString = x2::str( x2::wformat( L"[S%1%]%2%" ) % iID % m_vecGeneralStringTable[iID] );
	else
		wstrReplacedString = m_vecGeneralStringTable[iID];

    int iParamTypeListSize = strlen( szParamTypeList );

    va_list marker;
    va_start( marker, szParamTypeList );

    for( int i = 0; i < iParamTypeListSize; i++ )
    {
        std::wstringstream wstmReplaceSymbol;
        wstmReplaceSymbol << L"@" << ( i + 1 );

        std::wstringstream wstmReplaceValue;

        // 대입될 값을 스트링으로 만든다.
        switch( szParamTypeList[i] )
        {
        case 'h':       // 부호를 표현하는 정수로 변환
            {
                int iParam = va_arg( marker, int );
                if( iParam > 0 )
                {
                    wstmReplaceValue << L"+";
                }
                wstmReplaceValue << iParam;
            }
            break;
        case 'i':       // 정수로 변환
            {
                int iParam = va_arg( marker, int );
                wstmReplaceValue << iParam;
            }
            break;
        case 'e':       // 부호를 표현하는 실수로 변환
            {
                // 소수점 뒤 둘째자리까지만 표현하고, 끝이 0이면 0을 자른다. 1.50 -> 1.5
                double dParam = va_arg( marker, double );
                wchar_t szNumber[64];
                StringCchPrintfW( szNumber, ARRAY_SIZE( szNumber ), L"%.2f", dParam );
                dParam = _wtof( szNumber );
                if( dParam > 0.0 )
                {
                    wstmReplaceValue << L"+";
                }
                wstmReplaceValue << dParam;
            }
            break;
        case 'f':       // 실수로 변환
            {
                // 소수점 뒤 둘째자리까지만 표현하고, 끝이 0이면 0을 자른다. 1.50 -> 1.5
                double dParam = va_arg( marker, double );
                wchar_t szNumber[64];
                StringCchPrintfW( szNumber, ARRAY_SIZE( szNumber ), L"%.2f", dParam );
                dParam = _wtof( szNumber );
                wstmReplaceValue << dParam;
            }
            break;
        case 'I':       // ID의 문자열로 변환
            {
                int iParam = va_arg( marker, int );
                wstmReplaceValue << GetGeneralString( iParam );
            }
            break;
        case 's':       // 문자열로 변환
            {
                const char* szValue = va_arg( marker, const char* );
                wstmReplaceValue << KncUtil::toWideString( std::string( szValue != NULL ? szValue : "" ) );
            }
            break;
        case 'S':
            {
                wstmReplaceValue << va_arg( marker, wchar_t* );
            }
            break;
        case 'l':       // std::string 으로 변환
            {
                const char* szValue = va_arg( marker, const char* );   // a std::string (KStringVarArg.h)
                wstmReplaceValue << KncUtil::toWideString( std::string( szValue != NULL ? szValue : "" ) );
            }
            break;
        case 'L':       // std::wstring 으로 변환
            {
                const wchar_t* wszValue = va_arg( marker, const wchar_t* );    // a std::wstring (KStringVarArg.h)
                wstmReplaceValue << ( wszValue != NULL ? wszValue : L"" );
            }
            break;
        case 'M':       // 영어권 국가의 달력 표현
        case 'm':
            {
            }
            break;
        case 'o':       // 영어권 국가의 서수 표현
        case 'O':
            {
            }
            break;
        case 'n':       // 영어권 국가의 서수 표현
        case 'N':
            {
            }
            break;
        }

        std::wstring wstrReplaceSymbol = wstmReplaceSymbol.str().c_str();
        std::wstring wstrReplaceValue = wstmReplaceValue.str().c_str();

        size_t iFindPos = 0;
		size_t iCurrentPos = 0;
        while( ( iFindPos = wstrReplacedString.find( wstrReplaceSymbol, iCurrentPos ) ) != std::wstring::npos )
        {
            wstrReplacedString.replace( iFindPos, wstrReplaceSymbol.size(), wstrReplaceValue );
			iCurrentPos = iFindPos + wstrReplaceValue.length();
        }
    }
    va_end( marker );

    return wstrReplacedString;
}

const std::wstring& CKTDXStringTable::GetGeneralString( int iID )
{
    int iTableSize = ( int )m_vecGeneralStringTable.size();

    // STR_ID_EMPTY 때문에 -1이 넘어올 수도 있다.
    ASSERT( iID >= -1 && iID < iTableSize );

    if( iID < 0 || iID >= iTableSize )
    {
        return m_wstrEmpty;
    }

    return m_vecGeneralStringTable[iID];
}

const std::wstring& CKTDXStringTable::GetDisplayString( int iID )
{
    if( !m_bEnableDebugInfo )
        return GetGeneralString( iID );

    auto [it, inserted] = m_mapDebugInfoString.try_emplace( iID );
    if( inserted )
        it->second = L"[S" + std::to_wstring( iID ) + L"]" + GetGeneralString( iID );
    return it->second;
}


const std::wstring& CKTDXStringTable::GetScriptString( int iID )
{
	int iTableSize = ( int )m_vecScriptStringTable.size();

    ASSERT( iID >= 0 && iID < iTableSize );

    if( iID < 0 || iID >= iTableSize )
    {
        return m_wstrEmpty;
    }

    return m_vecScriptStringTable[iID];

}

namespace
{
// The file in the current folder whose name matches regardless of case (the client ships "general.ess"; the original
// opened "General.ess" on a case-insensitive file system). Empty when there is none.
std::filesystem::path FindInCurrentFolder( const std::wstring& wstrFileName )
{
    const std::filesystem::path wanted( wstrFileName );
    std::error_code ec;
    if( std::filesystem::is_regular_file( wanted, ec ) )
        return wanted;
    for( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( std::filesystem::current_path( ec ), ec ) )
    {
        if( entry.is_regular_file( ec ) &&
            x2::platform::IEquals( entry.path().filename().wstring(), wanted.filename().wstring() ) )
            return entry.path();
    }
    return {};
}
}

// The string table (.ess): UTF-16LE with a BOM, one "[index]\t[string]" line per string ending in CRLF, every code
// unit of a line but the CRLF XORed with 16. Read as bytes and decoded here: the original's _wfopen + fgetws relied on
// Windows' 16-bit wchar_t.
bool CKTDXStringTable::LoadStringTable( std::wstring& wstrFileName, std::vector< std::wstring >& vecStringTable )
{
    vecStringTable.clear();

    const std::filesystem::path path = FindInCurrentFolder( wstrFileName );
    std::ifstream file( path, std::ios::binary );
    ASSERT( file );
    if( path.empty() || !file )
    {
        return false;
    }
    const std::string bytes( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
    std::u16string units( bytes.size() / 2, u'\0' );
    for( size_t i = 0; i < units.size(); ++i )
    {
        units[i] = static_cast<char16_t>( static_cast<unsigned char>( bytes[2 * i] ) |
                                          ( static_cast<unsigned char>( bytes[2 * i + 1] ) << 8 ) );
    }
    // Skip the UTF-16LE byte order mark.
    size_t pos = ( !units.empty() && units[0] == 0xFEFF ) ? 1 : 0;

    int iIndex = 0;
    while( pos < units.size() )
    {
        size_t end = units.find( u'\n', pos );
        if( end == std::u16string::npos )
            end = units.size();
        std::u16string line = units.substr( pos, end - pos );
        pos = end + 1;
        if( !line.empty() && line.back() == u'\r' )
            line.pop_back();

		//09. 07. 03 김정협  XOR암호화 한 파일 복호화
        for( char16_t& unit : line )
            unit = static_cast<char16_t>( unit ^ 16 );
        const std::wstring wstrStringRead = x2::platform::Utf16ToWide( line );

        // tab 을 읽어서 index를 구분해낸다.
        size_t iTabPos = wstrStringRead.find( L"\t" );
        std::wstring wstrIndex = wstrStringRead.substr( 0, iTabPos );
        int iIndexRead = _wtoi( wstrIndex.c_str() );

        std::wstring wstrContent;
        // 파일의 index가 제대로 설정되어 있는지 검사한다.
        ASSERT( iIndexRead == iIndex );
        if( iIndexRead == iIndex && iTabPos != std::wstring::npos && ( iTabPos + 1 ) < wstrStringRead.size() )
        {
            // tab 뒤에 있는 것들만 읽는다.
            wstrContent = wstrStringRead.substr( iTabPos + 1 );
        }

        // \n 으로 표시한 부분에 실제로 줄바꿈 해준다.
        REPLACE_PART_OF_STRING( wstrContent, L"\\n", L"\r\n" );

        vecStringTable.push_back( wstrContent );
        iIndex++;
    }

    return true;
}
