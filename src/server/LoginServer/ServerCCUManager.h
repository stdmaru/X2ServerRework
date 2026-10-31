#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include <map>
#include "x2/platform/timer.h"
#include "KncSend.h"


//////////////////////////////////////////////////////////////////////////
// Server CCU Manager : 동접 정보 관리


class KServerCCUManager
{
	DeclareSingleton( KServerCCUManager );
	DeclDump;
	DeclToStringW;

	enum ENUM_TIMER
	{
		TM_CCU_UPDATE = 0,
		//{{ 2012. 07. 10	최육사		대기열 시스템
//#ifdef SERV_WAIT_QUEUE_SYSTEM
		TM_FOR_WAIT_QUEUE,
//#endif SERV_WAIT_QUEUE_SYSTEM
		//}}

		TM_TOTAL_NUM,
	};

public:
	KServerCCUManager(void);
	virtual ~KServerCCUManager(void);

	void Tick();

	void DBUpdateCCUInfo();
	void UpdateCCUInfo( IN const KUpdateCCUInfo& kInfo );

protected:
	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	void SendToLogDB( unsigned short usEventID );

private:
	std::map< UidType, KUpdateCCUInfo >		m_mapCCUInfo;

	x2::Timer							m_kTimer[TM_TOTAL_NUM];
	//CTime									m_tNextUpdateTime;

	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
	CTime									m_tLastUpdateTime;
	//}}
};

DefSingletonInline( KServerCCUManager );

template < class T >
void KServerCCUManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_LOGIN_LOG_DB, 0, NULL, usEventID, data );
}
