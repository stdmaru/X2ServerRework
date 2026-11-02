#pragma once

#include <KNCSingleton.h>
#include "ThreadManager.h"
#include <KncLua.h>
#include <lua_tinker.h>
using namespace lua_tinker;


//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능

#define CASE_SCRIPT_REFRESH_SWAP_INSTANCE( flag, className ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	className::SwapInstance( g_pLua ); \
	START_LOG( cout, L ## #className L" script refresh completed!" ); \
	Si##className()->DumpToLogFile(); \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신


class KChannelScriptRefreshManager : public KThreadManager
{
	DeclareSingleton( KChannelScriptRefreshManager );
	DeclPfID;

public:
	KChannelScriptRefreshManager();
	virtual ~KChannelScriptRefreshManager();

	void RegToLua( lua_State* pLuaState );

	// 스크립트 갱신 요청 lua함수
	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	void LogManager_Lua();
	//}}

	// derived from KThreadManager
	using KThreadManager::Init;
	virtual void Init();
	virtual KThread* CreateThread();

	lua_State* GetRefreshLuaState() { return m_pRefreshLuaState; }

protected:
	lua_State* m_pRefreshLuaState;
};

DefSingletonInline( KChannelScriptRefreshManager );


//}}

//}}
