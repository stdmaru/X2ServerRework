#include "./XSLCheckSumManager.h"

#include "../TinyXml/tinyxml.h"
#include <dbg/dbg.hpp>
#include <cwctype>

ImplementSingleton( CXSLCheckSumManager );

CXSLCheckSumManager::CXSLCheckSumManager(void)
{
	m_bLoadCheckSumFile = false;
}

CXSLCheckSumManager::~CXSLCheckSumManager(void)
{

}

// <PatchInfo><File><Item Name="..." CheckSum="..."/>...</File></PatchInfo>
// Port: read with TinyXml (already in Common) instead of CMarkup, whose license asks for permission for
// commercial use; the duplicate-file MessageBox became an error log.
bool CXSLCheckSumManager::OpenScriptFile( const WCHAR* pFileName )
{
	TiXmlDocument xml;
	if ( xml.LoadFile( x2::platform::WideToUtf8( pFileName ).c_str() ) == false )
		return false;

	m_bLoadCheckSumFile = true;

	const TiXmlElement* pPatchInfo = xml.FirstChildElement( "PatchInfo" );
	const TiXmlElement* pFile = pPatchInfo ? pPatchInfo->FirstChildElement( "File" ) : NULL;
	if( pFile == NULL )
		return true;

	for( const TiXmlElement* pItem = pFile->FirstChildElement( "Item" ); pItem != NULL; pItem = pItem->NextSiblingElement( "Item" ) )
	{
		const char* szName = pItem->Attribute( "Name" );
		const char* szCheckSum = pItem->Attribute( "CheckSum" );
		std::wstring fileFullName = x2::platform::Utf8ToWide( szName ? szName : "" );
		std::wstring fileName;

		const size_t tempStringIndex = fileFullName.rfind( L"\\" );
		if ( tempStringIndex == std::wstring::npos )
		{
			fileName = fileFullName;
		}
		else
		{
			fileName = fileFullName.substr( tempStringIndex + 1 );
		}

		const std::wstring checkSum = x2::platform::Utf8ToWide( szCheckSum ? szCheckSum : "" );

		for( std::wstring::iterator i = fileName.begin(); i != fileName.end(); i++ )
			*i = (WCHAR)std::towupper(*i);

		if ( AddCheckSum( fileName.c_str(), checkSum.c_str() ) == false )
		{
			START_LOG( cerr, L"Duplicate checksum file found" )
				<< BUILD_LOG( fileName )
				<< END_LOG;
		}
	}

	return true;
}

bool CXSLCheckSumManager::GetIsValidCheckSum( const std::map< std::wstring, std::wstring >& mapCheckSum )
{
	if ( m_bLoadCheckSumFile == false )
		return true;

	std::map< std::wstring, std::wstring >::const_iterator mit;

	for ( mit = mapCheckSum.begin(); mit != mapCheckSum.end(); mit++ )
	{
		std::wstring fileName = mit->first;

		std::map< std::wstring, std::wstring >::iterator mit2;
		mit2 = m_mapCheckSum.find( fileName );

		if ( mit2 != m_mapCheckSum.end() )
		{
			std::wstring mitCheckSum = mit->second;
			std::wstring mit2CheckSum = mit2->second;

			if ( mitCheckSum != mit2CheckSum )
			{
				return false;
			}
		}
	}

	return true;
}

std::wstring CXSLCheckSumManager::GetCheckSum( const WCHAR* pFileName )
{
	std::map< std::wstring, std::wstring >::iterator mit;

	mit = m_mapCheckSum.find( pFileName );
	if ( mit != m_mapCheckSum.end() )
	{
		return mit->second;
	}

	return L"";
}

bool CXSLCheckSumManager::AddCheckSum( const WCHAR* pFileName, const WCHAR* pCheckSum )
{
	std::map< std::wstring, std::wstring >::iterator mit;

	mit = m_mapCheckSum.find( pFileName );
	if ( mit != m_mapCheckSum.end() )
	{
		return false;
	}

	m_mapCheckSum.insert( std::make_pair( pFileName, pCheckSum ) );

	return true;
}
