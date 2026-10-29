#include "DEBAuthManager.h"

	#include "KncSend.h"
	#include "BaseServer.h"
	#include "DEBAuth.h"
	#include <KncLua.h>
	#include <lua_tinker.h>
	#include "BaseServer.h"

	ImplToStringW( KDEBAuthManager )
	{
		return stm_;
	}

	ImplPfID( KDEBAuthManager, PI_NULL );

	ImplementSingleton( KDEBAuthManager );

	KDEBAuthManager::KDEBAuthManager()
	{
		m_strWsdlPath = "";
	}

	KDEBAuthManager::~KDEBAuthManager()
	{

	}

	void KDEBAuthManager::RegToLua()
	{
		lua_tinker::class_add<KDEBAuthManager>( g_pLua, "KDEBAuthManager" );
		lua_tinker::class_def<KDEBAuthManager>( g_pLua, "Init", &KDEBAuthManager::Init );
		lua_tinker::class_def<KDEBAuthManager>( g_pLua, "dump", &KDEBAuthManager::Dump );
		lua_tinker::class_def<KDEBAuthManager>( g_pLua, "AddAuthServiceAddress", &KDEBAuthManager::AddAuthServiceAddress );

		#undef REG_LUA_ENUM
		#define REG_LUA_ENUM( id ) lua_tinker::decl( g_pLua, #id, KDEBAuth::##id );

		REG_LUA_ENUM( DEB_AUTH )
		REG_LUA_ENUM( DEB_AUTH_SECOND_SECURITY )
		REG_LUA_ENUM( DEB_NUM )

		lua_tinker::decl( g_pLua, "DEBAuthManager", this );
	}

	void KDEBAuthManager::Init( int nThreadNum )
	{
		// You must call Init only after another desired methods, hence, is the last method you want to call
		KThreadManager::Init( nThreadNum );
	}

	void KDEBAuthManager::AddAuthServiceAddress( const char* szURL ) {
		m_strWsdlPath = szURL;
	}

	//KSimLayer::GetKObj()->Init(); is called when BeginThread is called
	KThread* KDEBAuthManager::CreateThread()
	{
		return new KDEBAuth(m_strWsdlPath.c_str());
	}

	void KDEBAuthManager::Tick()
	{

	}
