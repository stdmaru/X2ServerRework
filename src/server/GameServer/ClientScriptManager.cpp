#include "GameServer.h"
#include "ClientScriptManager.h"


ImplementSingleton( KClientScriptManager );

KClientScriptManager::KClientScriptManager()
{
}

KClientScriptManager::~KClientScriptManager()
{
}

ImplToStringW( KClientScriptManager )
{
	return stm_;
}

bool KClientScriptManager::LoadClientScript( IN const char* pFileName, IN int iScriptEnum )
{
	std::string strPath = pFileName;
	std::wstring wstrScriptString;

	std::wfstream fin;
	//fin.imbue( std::locale("korean") );
	fin.open( strPath.c_str(), std::ios::in );

	WCHAR pcBuf[MAX_PATH] = { 0 };

	// Load Head
	fin.getline( pcBuf, MAX_PATH );

	wstrScriptString += pcBuf;

	// Load Data
	while ( true )
	{
		if ( fin.eof() == true )
			break;

		fin.getline( pcBuf, MAX_PATH );

		wstrScriptString += pcBuf;
	}

	fin.close();

	m_mapScriptInfo.insert( std::make_pair( iScriptEnum, wstrScriptString ) );

	START_LOG( cout, L"Client script loaded successfully: " << KncUtil::toWideString( pFileName ) );
	return true;
}

bool KClientScriptManager::GetClientScriptInfo( IN int iScriptEnum, OUT std::wstring& wstrScriptInfo )
{
	std::map< int, std::wstring >::const_iterator mit;
	mit = m_mapScriptInfo.find( iScriptEnum );
	if( mit == m_mapScriptInfo.end() )
	{
		START_LOG( cerr, L"The script does not exist." )
			<< BUILD_LOG( iScriptEnum )
			<< END_LOG;
		return false;
	}

	wstrScriptInfo = mit->second;
	return true;
}
