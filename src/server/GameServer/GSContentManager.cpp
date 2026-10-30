#include "./GSContentManager.h"


//{{ 2011. 10. 12	최육사	컨텐츠 관리자
//#ifdef SERV_CONTENT_MANAGER


ImplementRefreshSingleton( KGSContentManager );

KGSContentManager::KGSContentManager(void)
{
}

KGSContentManager::~KGSContentManager(void)
{
}

ImplToStringW( KGSContentManager )
{
	stm_	<< L"----------[ Game Server Content Manager ]----------" << std::endl
			<< TOSTRINGW( m_bEnableCashShop )
		;

	return stm_;
}

ImplementLuaScriptParser( KGSContentManager )
{
	lua_tinker::class_add<KGSContentManager>( GetLuaState(), "KGSContentManager" );
	lua_tinker::class_def<KGSContentManager>( GetLuaState(), "SetEnableCashShop",		&KGSContentManager::SetEnableCashShop_LUA );
	lua_tinker::class_def<KGSContentManager>( GetLuaState(), "dump",					&KGSContentManager::Dump );

	lua_tinker::decl( GetLuaState(), "GSContentManager", this );
}

void KGSContentManager::SetEnableCashShop_LUA( bool bVal )
{
	m_bEnableCashShop = bVal;

	START_LOG( cout, L"[컨텐츠 매니저] 캐쉬샵 기능 : " << m_bEnableCashShop );
}


//#endif SERV_CONTENT_MANAGER
//}}
