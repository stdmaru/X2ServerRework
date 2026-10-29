#include "./DEBBillingManager.h"


#include "KncSend.h"
#include "BaseServer.h"
#include "DEBBilling.h"

ImplToStringW( KDEBBillingManager )
{
	return stm_;
}

ImplPfID( KDEBBillingManager, PI_GS_PUBLISHER_BILLING );
ImplementSingleton( KDEBBillingManager );

KDEBBillingManager::KDEBBillingManager(void)
{
	m_strWsdlFile = "";
}

KDEBBillingManager::~KDEBBillingManager(void)
{

}

#include <KncLua.h>
#include <lua_tinker.h>
#include "BaseServer.h"

void KDEBBillingManager::RegToLua()
{
	lua_tinker::class_add<KDEBBillingManager>( g_pLua, "KDEBBillingManager" );
	lua_tinker::class_def<KDEBBillingManager>( g_pLua, "Init",				 &KDEBBillingManager::Init );
	lua_tinker::class_def<KDEBBillingManager>( g_pLua, "dump",				 &KDEBBillingManager::Dump );
	lua_tinker::class_def<KDEBBillingManager>( g_pLua, "AddWebServiceAddress", &KDEBBillingManager::AddWebServiceAddress );

	lua_tinker::decl( g_pLua, "DEBBillingManager", this );
}

void KDEBBillingManager::Init( int nThreadNum )
{
	KThreadManager::Init( nThreadNum );
}

void KDEBBillingManager::AddWebServiceAddress( const char* szWSDL )
{
	m_strWsdlFile = szWSDL;
}

KThread* KDEBBillingManager::CreateThread()
{
	return new KDEBBilling(m_strWsdlFile.c_str());
}
