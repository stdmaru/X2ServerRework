#include "LoginSimLayer.h"
#include "BaseServer.h"

#include "./DLLManager.h"


//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
//}}




ImplementSingleton( KDLLManager );

KDLLManager::KDLLManager(void)
{
	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	//}}
}

KDLLManager::~KDLLManager(void)
{
}

ImplToStringW( KDLLManager )
{
	return stm_;
}

void KDLLManager::Tick()
{
	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	//}}

	if( m_kTimer.elapsed() < 300.0 )
		return;

	m_kTimer.restart();

	// DLL 관리 리스트
	KDBE_GET_MODULE_INFO_LIST_ACK kPacket;
	GetDBUPdateInfo( kPacket.m_vecModuleList );
	if( kPacket.m_vecModuleList.empty() == false )
	{
		SendToLogDB( DBE_UPDATE_MODULE_INFO_LIST_REQ, kPacket );
	}
}

void KDLLManager::OnServerReadyComplete()
{
	START_LOG( cout, L"DLL manager list requested!" );

	SendToLogDB( DBE_GET_MODULE_INFO_LIST_REQ );

	START_LOG( cout, L"Hack DLL list requested!" );

}

//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
void KDLLManager::Init( IN const std::vector< KModuleInfo >& vecModuleList )
{
	for( const KModuleInfo& kInfo : vecModuleList )
	{
		AddModuleInfo( kInfo );
	}
}
//}}

void KDLLManager::UpdateModuleInfo( IN const std::wstring& wstrModuleName )
{
	std::map< std::wstring, KModuleInfo >::iterator mit;
	mit = m_mapModuleList.find( wstrModuleName );
	if( mit == m_mapModuleList.end() )
	{
		KModuleInfo kInfo;
		kInfo.m_iModuleUID = 0; // 0으로 초기화! UID발급 안된 상태임
		kInfo.m_iCount = 1;
		kInfo.m_bReserveDBUpdate = true;
		kInfo.m_wstrModuleName = wstrModuleName;
		AddModuleInfo( kInfo );
	}
	else
	{
		++mit->second.m_iCount;
		mit->second.m_bReserveDBUpdate = true;
	}
}

void KDLLManager::Shutdown()
{
	KDBE_GET_MODULE_INFO_LIST_ACK kPacket;
	GetDBUPdateInfo( kPacket.m_vecModuleList );
	if( kPacket.m_vecModuleList.empty() == false )
	{
		SendToLogDB( DBE_UPDATE_MODULE_INFO_LIST_REQ, kPacket );
	}
}

void KDLLManager::UpdateModuleList( IN const std::vector< std::wstring >& vecModuleList )
{
	for( const std::wstring& wstrModuleName : vecModuleList )
	{
		UpdateModuleInfo( wstrModuleName );
	}
}

void KDLLManager::UpdateModuleList( IN const std::vector< KModuleInfo >& vecModuleList )
{
	for( const KModuleInfo& kInfo : vecModuleList )
	{
		UpdateModuleUID( kInfo.m_wstrModuleName, kInfo.m_iModuleUID );
	}
}

bool KDLLManager::AddModuleInfo( IN const KModuleInfo& kInfo )
{
	if( m_mapModuleList.find( kInfo.m_wstrModuleName ) != m_mapModuleList.end() )
	{
		START_LOG( cwarn, L"A module with this UID is already registered." )
			<< BUILD_LOG( kInfo.m_wstrModuleName )
			<< END_LOG;
		return false;
	}

	m_mapModuleList.insert( std::make_pair( kInfo.m_wstrModuleName, kInfo ) );
	return true;
}

bool KDLLManager::UpdateModuleUID( IN const std::wstring& wstrModuleName, IN int iModuleUID )
{
	std::map< std::wstring, KModuleInfo >::iterator mit;
	mit = m_mapModuleList.find( wstrModuleName );
	if( mit == m_mapModuleList.end() )
	{
		START_LOG( cerr, L"Module information not found!" )
			<< BUILD_LOG( wstrModuleName )
			<< END_LOG;
		return false;
	}

    mit->second.m_iModuleUID = iModuleUID;
	return true;
}

void KDLLManager::GetDBUPdateInfo( OUT std::vector< KModuleInfo >& vecUpdateInfo )
{
	vecUpdateInfo.clear();

	std::map< std::wstring, KModuleInfo >::iterator mit;
	for( mit = m_mapModuleList.begin(); mit != m_mapModuleList.end(); ++mit )
	{
		if( mit->second.m_bReserveDBUpdate == false )
			continue;

        vecUpdateInfo.push_back( mit->second );

		mit->second.m_bReserveDBUpdate = false;
	}
}

void KDLLManager::SendToLogDB( unsigned short usEventID )
{
	SendToLogDB( usEventID, char() );
}

//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
//}}
