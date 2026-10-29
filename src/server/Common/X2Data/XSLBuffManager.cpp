#include "./XSLBuffManager.h"
#include "NetError.h"

ImplementRefreshSingleton( CXSLBuffManager );

CXSLBuffManager::CXSLBuffManager(void)
{

}

CXSLBuffManager::~CXSLBuffManager(void)
{
}

ImplToStringW( CXSLBuffManager )
{
	stm_	<< L"----------[ Buff Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapBuffList.size() );
		;

	return stm_;
}

ImplementLuaScriptParser( CXSLBuffManager )
{
	lua_tinker::class_add<CXSLBuffManager>( GetLuaState(), "CXSLBuffManager" );
	lua_tinker::class_def<CXSLBuffManager>( GetLuaState(), "AddBuffInfo",				&CXSLBuffManager::AddBuffInfo_LUA );

	lua_tinker::class_def<CXSLBuffManager>( GetLuaState(), "dump",						&CXSLBuffManager::Dump );

	lua_tinker::decl( GetLuaState(), "BuffManager", this );
}

bool CXSLBuffManager::AddBuffInfo_LUA()
{
	KLuaManager luaManager( GetLuaState() );

	BuffTemplet kBuffInfo;
	LUA_GET_VALUE_RETURN_ENUM(	luaManager, L"m_eBuffID",				kBuffInfo.m_eBuffID,					BUFF_TEMPLET_ID,		BTI_NONE,		goto end_proc );
	//LUA_GET_VALUE(			luaManager, L"m_TitleName",				kBuffInfo.m_wstrBuffName,				L"" );
	LUA_GET_VALUE(				luaManager, L"m_fBuffPeriodSecond",		kBuffInfo.m_fBuffPeriodSecond,			0.0 );

	//{{ 2012. 05. 6	박세훈	어둠의 문 개편
	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheLeaveDungeonRoom",	kBuffInfo.m_bEndBuffAtTheLeaveDungeonRoom,	false );
	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheEndDungeonGame",	kBuffInfo.m_bEndBuffAtTheEndDungeonGame,	false );

	//{{ 2012. 07. 11	박세훈	2012 대전 시즌2
	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheLeavePvpRoom",		kBuffInfo.m_bEndBuffAtTheLeavePvpRoom,		false );
	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheEndPvpGame",		kBuffInfo.m_bEndBuffAtTheEndPvpGame,		false );
	//}}

	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheUserDied",			kBuffInfo.m_bEndBuffAtTheUserDied,			false );
	LUA_GET_VALUE( luaManager, L"m_bKeepBuffAtTheUserLogOut",		kBuffInfo.m_bKeepBuffAtTheUserLogOut,		false );
	//}}

	LUA_GET_VALUE( luaManager, L"m_bEndBuffAtTheLeaveBattleFieldRoom",	kBuffInfo.m_bEndBuffAtTheLeaveBattleFieldRoom,	false );

	if( luaManager.BeginTable( L"SOCKET_OPTION" ) == S_OK )
	{
		int index = 1;
		int iSocketOption	= -1;
		while( luaManager.GetValue( index, iSocketOption ) == S_OK )
		{
			if( iSocketOption > 0 )
			{
				kBuffInfo.m_vecOption.push_back( iSocketOption );
			}
			++index;
		}
		luaManager.EndTable();
	}

	//{{ 버프 관리 시스템 - 김민성
	if( luaManager.BeginTable( L"BUFF_FACTOR" ) == S_OK )
	{
		int index = 1;
		int iFactorID	= -1;

		if( luaManager.GetValue( index, iFactorID ) == S_OK )
		{
			if( iFactorID > 0 )
			{
				kBuffInfo.m_iFactorID = iFactorID;
			}
		}
		luaManager.EndTable();
	}
	//}

	//{{ 2012. 04. 18	박세훈	어둠의 문 개편
	if( luaManager.BeginTable( L"USING_BUFF_ROOM_TYPE" ) == S_OK )
	{
		int index = 1;
		int iUsingBuffRoomType	= -1;
		while( luaManager.GetValue( index, iUsingBuffRoomType ) == S_OK )
		{
			if( 0 <= iUsingBuffRoomType )
			{
				kBuffInfo.m_vecRoomType.push_back( iUsingBuffRoomType );
			}
			++index;
		}
		luaManager.EndTable();
	}
	//}}

	m_mapBuffList.insert( std::make_pair( kBuffInfo.m_eBuffID, kBuffInfo ) );
	return true;

end_proc:
	START_LOG( cerr, L"버프 스크립트 파싱 실패.!" )
		<< BUILD_LOG( kBuffInfo.m_eBuffID )
		<< END_LOG;

	return false;
}

const CXSLBuffManager::BuffTemplet* CXSLBuffManager::GetBuffTemplet( IN const int iBuffID ) const
{
	std::map< BUFF_TEMPLET_ID, BuffTemplet >::const_iterator mit;
	mit = m_mapBuffList.find( static_cast<BUFF_TEMPLET_ID>(iBuffID) );
	if( mit == m_mapBuffList.end() )
		return NULL;

	return &mit->second;
}

//{{ 2012. 04. 17	박세훈	어둠의 문 개편
bool CXSLBuffManager::BuffTemplet::CheckRoomType( IN const int& iCheckRoomType ) const
{
	for( int iRoomType : m_vecRoomType )
	{
		if( iCheckRoomType == iRoomType )
		{
			return true;
		}
	}
	return false;
}
//}}
