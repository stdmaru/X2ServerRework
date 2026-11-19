#include "BaseServer.h"
#include "LineMap/ClientScriptState.h"

#include "KAutoPath.h"

#include <KncLua.h>

KClientScriptState::KClientScriptState() : m_pState( luaL_newstate() )
{
	luaL_openlibs( m_pState );
}

KClientScriptState::~KClientScriptState()
{
	lua_close( m_pState );
}

std::string KClientScriptState::Find( const std::string& strName )
{
	std::string strPath = strName;
	KAutoPath kAutoPath;
	return kAutoPath.GetPullPath( strPath ) ? strPath : std::string();
}

bool KClientScriptState::Run( const std::string& strName, bool bRequired )
{
	const std::string strPath = Find( strName );
	if( strPath.empty() )
	{
		if( bRequired )
		{
			START_LOG( cerr, L"Script not found." )
				<< BUILD_LOG( strName )
				<< END_LOG;
		}
		else
		{
			START_LOG( cwarn, L"Script not found." )
				<< BUILD_LOG( strName )
				<< END_LOG;
		}
		return false;
	}

	if( luaL_dofile( m_pState, strPath.c_str() ) != 0 )
	{
		START_LOG( cerr, L"Failed to run a script." )
			<< BUILD_LOG( strName )
			<< BUILD_LOG( lua_tostring( m_pState, -1 ) )
			<< END_LOG;
		lua_pop( m_pState, 1 );
		return false;
	}
	return true;
}

bool KClientScriptState::RunChunk( const char* szChunk )
{
	if( luaL_dostring( m_pState, szChunk ) != 0 )
	{
		START_LOG( cerr, L"Failed to run a Lua chunk." )
			<< BUILD_LOG( lua_tostring( m_pState, -1 ) )
			<< END_LOG;
		lua_pop( m_pState, 1 );
		return false;
	}
	return true;
}

std::string KClientScriptState::GetGlobalString( const char* szName )
{
	lua_getglobal( m_pState, szName );
	std::string str = lua_type( m_pState, -1 ) == LUA_TSTRING ? lua_tostring( m_pState, -1 ) : "";
	lua_pop( m_pState, 1 );
	return str;
}
