#include "StringTable.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include "x2/platform/format.h"
#include <dbg/dbg.hpp>
#include <memory>

KStringTable::KStringTable(void)
: m_strCurrentSection(L"")
{
}

KStringTable::~KStringTable(void)
{
}

void KStringTable::Dump( std::wostream& stm_ )const
{
    if( m_mapSectionTable.empty() )
        return;

    MAPSECTION::const_iterator it = m_mapSectionTable.begin();
    stm_ << L"--- Dump String Table ---" << std::endl;
    while( it != m_mapSectionTable.end() )
    {
        stm_ << x2::wformat( L"[%s]\n") % it->second.m_strSection;
        MAPKEYVALUE::const_iterator itKeyValue = it->second.m_mapKeyValue.begin();
        while( itKeyValue != it->second.m_mapKeyValue.end() )
        {
            stm_ << x2::wformat( L" %s = %s\n" ) % itKeyValue->second.m_strKey % itKeyValue->second.m_strValue;
            ++itKeyValue;
        }
        ++it;
    }
}

namespace
{
    std::wstring Trim( const std::wstring& s )
    {
        const size_t b = s.find_first_not_of( L" \t\r" );
        if( b == std::wstring::npos )
            return std::wstring();
        const size_t e = s.find_last_not_of( L" \t\r" );
        return s.substr( b, e - b + 1 );
    }

    // INI file text: UTF-16LE or UTF-8, or CP949 like the deployed config_*.ini (the original relied on the Windows API)
    bool ReadIniText( const std::wstring& strFileName, std::wstring& strText )
    {
        std::ifstream in( std::filesystem::path( strFileName ), std::ios::binary );
        if( !in )
            return false;
        const std::string data( ( std::istreambuf_iterator<char>( in ) ), std::istreambuf_iterator<char>() );
        strText = x2::platform::DecodeTextFile( data );
        return true;
    }
}

// Port: the original used GetPrivateProfileSectionNamesW/GetPrivateProfileSectionW. Same result: sections in file
// order, `key=value` lines with key and value trimmed, `;` comment lines ignored, keys split on '=' (see below).
bool KStringTable::LoadIni( IN const wchar_t* szFileName_ )
{
    if(  szFileName_ == NULL )
    {
        START_LOG( cerr, L"파일명이 NULL입니다." );
        return false;
    }

    const std::wstring strFullFileName = szFileName_;
    std::wstring strText;
    if( !ReadIniText( strFullFileName, strText ) )
    {
        START_LOG( cerr, L"파일이 존재하지 않음")
            << BUILD_LOG( strFullFileName )
            << END_LOG;
        return false;
    }

    std::vector< std::pair< std::wstring, std::vector<std::wstring> > > vecSections;
    std::wistringstream stm( strText );
    std::wstring strLine;
    while( std::getline( stm, strLine ) )
    {
        strLine = Trim( strLine );
        if( strLine.empty() || strLine[0] == L';' )
            continue;
        if( strLine.front() == L'[' )
        {
            const size_t end = strLine.find( L']' );
            vecSections.emplace_back( Trim( strLine.substr( 1, end == std::wstring::npos ? std::wstring::npos : end - 1 ) ),
                                      std::vector<std::wstring>() );
            continue;
        }
        if( vecSections.empty() )
            continue; // key outside of any section: ignored, as by the Windows API
        const size_t eq = strLine.find( L'=' );
        vecSections.back().second.push_back( eq == std::wstring::npos ? strLine : Trim( strLine.substr( 0, eq ) ) + L"=" + Trim( strLine.substr( eq + 1 ) ) );
    }

    for( const auto& kSection : vecSections )
    {
        if( !kSection.first.empty() )
            AddAndBuildSection( kSection.first.c_str(), kSection.second );
    }

    return true;
}

std::wstring KStringTable::GetValue( IN const wchar_t* szSectionName_, IN const wchar_t* szKey_ )
{
    if( szSectionName_ == NULL || szKey_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터중 NULL이 있음" );
        std::wstring strEmpty(L"");
        return strEmpty;
    }

    MITSECTION tmpMITSECTIONTABLE;
    if( !FindSection( szSectionName_, tmpMITSECTIONTABLE ) )
    {
        START_LOG( cerr, L"섹션이 존재하지 않음" )
            << BUILD_LOG( szSectionName_ )
            << END_LOG;
        std::wstring strEmpty(L"");
        return strEmpty;
    }

    MITKEYVALUE tmpMITSECTION;
    if( !FindKey_value( tmpMITSECTIONTABLE, szKey_, tmpMITSECTION ) )
    {
        START_LOG( cerr, L"키가 존재하지 않음" )
            << BUILD_LOG( szKey_ )
            << END_LOG;
        std::wstring strEmpty(L"");
        return strEmpty;
    }

    return tmpMITSECTION->second.m_strValue;
}

bool KStringTable::SetCurrentSection( IN const wchar_t* szSectionName_ )
{
    if( szSectionName_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터가 NULL이다." );
        return false;
    }

    MITSECTION tmpMIT;
    if(!FindSection( szSectionName_, tmpMIT ) )
    {
        START_LOG( cerr, L"섹션이 존재하지 않음" )
            << BUILD_LOG( szSectionName_ )
            << END_LOG;
        return false;
    }

    m_strCurrentSection = szSectionName_;
    return true;
}

std::wstring KStringTable::GetValue( IN const wchar_t* szKey_ )
{
    if( szKey_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터가 NULL이다." );
        std::wstring strEmpty(L"");
        return strEmpty;
    }

    if( m_strCurrentSection.empty() )
    {
        START_LOG( cerr, L"현재 섹션이 설정되지 않았음" );
        return m_strCurrentSection; // return Empty String
    }

    return GetValue( m_strCurrentSection.c_str(), szKey_ );
}

/* Protected */

bool KStringTable::AddAndBuildSection( IN const wchar_t* szSectionName_, IN const std::vector<std::wstring>& vecLines_ )
{
    if( szSectionName_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터중 NULL이 있음" );
        return false;
    }

    Section tmpSection;
    tmpSection.m_strSection = szSectionName_;

    for( const std::wstring& strLine : vecLines_ )
    {
        AddAndBuildKeyvalue( strLine.c_str(), tmpSection );
    }

    m_mapSectionTable.insert( std::make_pair( tmpSection.m_strSection, tmpSection) );

    return true;
}

bool KStringTable::AddAndBuildKeyvalue( IN const wchar_t* szKey_value_, Section& section_ )
{
    if( szKey_value_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터가 NULL이다." );
        return false;
    }

    std::wstring strIniKeyValue = szKey_value_;
    // split on '=' dropping empty tokens (boost::char_separator semantics): "k=v=w" -> key k, value v
    std::vector<std::wstring> tokens;
    for( size_t pos = 0; pos <= strIniKeyValue.size(); )
    {
        size_t next = strIniKeyValue.find( L'=', pos );
        if( next == std::wstring::npos )
            next = strIniKeyValue.size();
        if( next > pos )
            tokens.push_back( strIniKeyValue.substr( pos, next - pos ) );
        pos = next + 1;
    }
    std::vector<std::wstring>::const_iterator tok_iter = tokens.begin();

    Key_value keyValue;

    if( tok_iter == tokens.end() )
        goto _fail_;

    keyValue.m_strKey = *tok_iter;

    if( (++tok_iter) == tokens.end() )
        goto _fail_;

    keyValue.m_strValue = *tok_iter;

    section_.m_mapKeyValue.insert(std::make_pair(keyValue.m_strKey, keyValue));

    return true;

_fail_:
    START_LOG( cerr, L"토큰을 얻을수 없음." );
    return false;

}

bool KStringTable::FindSection( IN const wchar_t* szSectionName_, OUT MITSECTION& itsection_ )
{
    if( szSectionName_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터가 NULL이다." );
        return false;
    }

    itsection_ = m_mapSectionTable.find( szSectionName_ );
    if( itsection_ == m_mapSectionTable.end() )
    {
        START_LOG( cerr, L"섹션이 존재하지 않음" )
            << BUILD_LOG( szSectionName_ )
            << END_LOG;
        return false;
    }

    return true;
}

bool KStringTable::FindKey_value( IN MITSECTION& itSection_, IN const wchar_t* szKeyName_,
                                  OUT MITKEYVALUE& itKeyValue_ )
{
    if( szKeyName_ == NULL )
    {
        START_LOG( cerr, L"입력 파라미터가 NULL이다." );
        return false;
    }

    Section tmpSection = itSection_->second;

    itKeyValue_ = itSection_->second.m_mapKeyValue.find( szKeyName_ );
    if( itKeyValue_ == itSection_->second.m_mapKeyValue.end() )
    {
        START_LOG( cerr, L"키가 존재하지 않음" )
            << BUILD_LOG( szKeyName_ )
            << END_LOG;
        return false;
    }

    return true;
}
