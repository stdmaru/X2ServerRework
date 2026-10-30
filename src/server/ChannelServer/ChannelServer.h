#pragma once
#include "BaseServer.h"
#include "KncSend.h"

class KChannelServer : public KBaseServer
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclDump;
    DeclPfID;

public:
    KChannelServer(void);
    virtual ~KChannelServer(void);

    // derived from KBaseServer
    virtual bool Init();
    virtual void RegToLua();
    virtual void ShutDown();
    virtual bool DestroyAllSession();         // 접속중인 세션의 종료.

    // derived from KPerformer
    virtual void ProcessEvent( const KEventPtr& spEvent );
    virtual void Tick();

    //////////////////////////////////////////////////////////////////////////

protected:
	void WriteServerInfoToDB();
	virtual void OnServerReadyComplete();

	//_DECL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK );
	_DECL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_CHANNEL_SERVER_ON_ACK );	// 엘소드 해외팀 변경
   _DECL_ON_FUNC( DBE_UPDATE_SERVER_INFO_ACK, KServerList );
	DECL_ON_FUNC( DBE_CHANNEL_LIST_ACK );

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	DECL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_ACK );
	//}}

	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	DECL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK );
	//}}
	//{{ 2012. 07. 04	최육사		대기열 시스템
	DECL_ON_FUNC( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_ACK );
	//}}

	//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능
	_DECL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	//}}

	DECL_ON_FUNC( DBE_SERVERGROUP_LIST_ACK );
public:
	void RefreshChannelList();
	void UpdateChannelList( const std::map< int, KChannelInfo >& mapChannelList );

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	void	UpdateChannelBonusList( const std::map< int, KChannelBonusInfo >& mapChannelBonusList );
	const std::map< int, KChannelBonusInfo >& GetChannelBonusList() { return m_mapChannelBonusList; }
	//}}

	const std::map< int, KChannelInfo >& GetChannelList() { return m_mapChannelList; }

	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}
	void RefreshServerGroupList();
	void UpdateServerGroupList( const std::map< int, KServerGroupInfo >& mapServerGroupList );
	const std::map< int, KServerGroupInfo >& GetServerGroupList() { return m_mapServerGroupList; }

	void InitLoginResultInfo();
	void WriteLoginResultInfoToDB();
	void PlusLoginResultCount(KLoginResultInfo::LOGIN_RESULT_TYPE_ENUM eResultType);
protected:
	std::map< int, KChannelInfo >		m_mapChannelList;

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	std::map< int, KChannelBonusInfo >	m_mapChannelBonusList;
	//}}

	boost::timer						m_kChannelListRefreshTimer;
	bool								m_bIsFirstUpdate;
	std::map< int, KServerGroupInfo >		m_mapServerGroupList;
	boost::timer							m_kServerGroupListRefreshTimer;
	bool									m_bIsFirstUpdateServerGroup;

	KLoginResultInfo					m_kLoginResultInfo;
	boost::timer						m_kLoginResultInfoTimer;

	boost::timer					m_tTimeProcessCommunicationONOFF;
	bool							m_bServerRunningProcessCommunicationOnOff;
};

DefKObjectInline( KChannelServer, KBaseServer );

//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
//}}
