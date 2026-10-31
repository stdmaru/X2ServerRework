#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include <boost/timer.hpp>
#include "KncSend.h"


//////////////////////////////////////////////////////////////////////////
// DLL Manager : 계정 블럭 관리


class KDLLManager
{
	DeclareSingleton( KDLLManager );
	DeclDump;
	DeclToStringW;

public:
	KDLLManager(void);
	~KDLLManager(void);

	void Tick();
	void OnServerReadyComplete();

	void Shutdown();
	//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
	void Init( IN const std::vector< KModuleInfo >& vecModuleList );
	//}}

	void UpdateModuleInfo( IN const std::wstring& wstrModuleName );
	void UpdateModuleList( IN const std::vector< std::wstring >& vecModuleList );
	void UpdateModuleList( IN const std::vector< KModuleInfo >& vecModuleList );
	bool AddModuleInfo( IN const KModuleInfo& kInfo );
	bool UpdateModuleUID( IN const std::wstring& wstrModuleName, IN int iModuleUID );
	void GetDBUPdateInfo( OUT std::vector< KModuleInfo >& vecUpdateInfo );

private:
	//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
	//}}

	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	void SendToLogDB( unsigned short usEventID );

private:
	std::map< std::wstring, KModuleInfo >	m_mapModuleList;
	boost::timer							m_kTimer;

	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	//}}

};

DefSingletonInline( KDLLManager );

template < class T >
void KDLLManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_LOGIN_LOG_DB, 0, NULL, usEventID, data );
}
//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
//}}
