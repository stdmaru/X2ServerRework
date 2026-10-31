#pragma once
#include "BaseServer.h"
#include "KncSend.h"

class KGlobalServer : public KBaseServer
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclDump;
    DeclPfID;

public:
	enum EXCHANGE_LIMIT_RESULT
	{
		ELR_SUCCEED,
		ELR_NOT_INITED,
		ELR_EXHAUSTED,
		ELR_CHECK,
		ELR_PASS,
	};

    KGlobalServer(void);
    virtual ~KGlobalServer(void);

    // derived from KBaseServer
    virtual bool Init();
    virtual void RegToLua();
    virtual void ShutDown();
    virtual bool DestroyAllSession();         // 접속중인 세션의 종료.

    // derived from KPerformer
    virtual void ProcessEvent( const KEventPtr& spEvent );
    virtual void Tick();

	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	void	GetTimeEnchantEventUID( OUT int& iTimeEnchantEventUID ) const{	iTimeEnchantEventUID = m_iTimeEnchantEventUID;	}
	void	GetNumOfBeginTimeEnchantEventInfo( OUT int& iNumOfTimeEnchantEvent ) const{	iNumOfTimeEnchantEvent = m_mapBeginTimeEnchantEventInfo.size();	}
	bool	RetrieveTimeEnchantEventData( OUT KTimeEnchantEventInfo& kInfo, OUT int& iTimeEnchantEventUID );
	bool	CheckTimeEnchantEventUID( IN const int iTimeEnchantEventUID ) const{	return m_iTimeEnchantEventUID == iTimeEnchantEventUID;	}

	void	GetExchangeLimitUID( OUT int& iExchangeLimitUID ) const{	iExchangeLimitUID = m_iExchangeLimitUID;	}
	void	GetBeginExchangeLimitInfo( OUT std::map< int, int >& mapBeginExchangeLimitInfo ) const;
	int		RetrieveExchangeLimitData( IN const int iDestItemID, OUT int& iLimitUID );
	int		ReInsertExchangeLimitData( IN const int iLimitUID );
	bool	CheckExchangeLimitUID( IN const int iExchangeLimitUID ) const{	return m_iExchangeLimitUID == iExchangeLimitUID;	}

    //////////////////////////////////////////////////////////////////////////

protected:
	virtual void OnServerReadyComplete();
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	void	CheckHeroPvpUserList( void );

	//{{ 2013. 05. 29	최육사	제재 리스트 통합
	template < class T > void SendToGSUser( UidType nToGSUID, UidType nToGSUSer, unsigned short usEventID, const T& data );
	//}}

	void	CheckTimeEnchantEvent( void );

	void	CheckExchangeLimit( IN const bool bForceNotify = false );

	void	CheckFieldBossSystem( IN const CTime tCurrentTime );

	template < class T > void SendToGameServer( unsigned short usEventID, const T& data ) const;
	template < class T > void BroadCastAllGS( unsigned short usEventID, const T& data ) const;


	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

//#ifdef	SERV_HERO_PVP_MANAGE_LIST // 적용날짜: 2013-04-04
	template < class T > void SendToGSCharacter( UidType nToGSUID, UidType nToUnitUID, unsigned short usEventID, const T& data );
//#endif	// SERV_HERO_PVP_MANAGE_LIST

   _DECL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK );
	DECL_ON_FUNC_NOPARAM( DBE_UPDATE_SERVER_INFO_ACK );

	//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신
   _DECL_ON_FUNC( ESR_ORDER_TO_GLOBAL_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT );
   _DECL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	//}}

   //{{ 2010. 10. 12	최육사	서버 모니터링
   //}}
   //{{ 2011. 04. 13  김민성  글로벌 서버 추가
	DECL_ON_FUNC( DBE_GET_DEFENCE_CRYSTAL_ACK );
   //}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2012. 04. 25	박세훈	어둠의 문 개편
	_DECL_ON_FUNC( EGB_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT, int );
	DECL_ON_FUNC_NOPARAM( EGB_RESET_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT );
	//}}

	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}



	DECL_ON_FUNC( DBE_CHECK_EVENT_UPDATE_ACK );

	DECL_ON_FUNC( DBE_EVENT_UPDATE_ACK );

	//{{ 2013. 05. 14	최육사	제재 리스트 통합
	DECL_ON_FUNC( DBE_CHECK_BLOCK_LIST_ACK );
	//}}

	DECL_ON_FUNC( DBE_TIME_ENCHANT_EVENT_INFO_ACK );
	DECL_ON_FUNC( DBE_TIME_ENCHANT_EVENT_INFO_NOT );

	DECL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_ACK );
	DECL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_NOT );

	DECL_ON_FUNC( DBE_GET_GLOBAL_MISSION_INFO_ACK );

private:
	boost::timer	m_kTimer;

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	boost::timer	m_kTimerRandomQuest;
	//}}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}
	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	boost::timer					m_tTimeProcessCommunicationONOFF;
	bool							m_bServerRunningProcessCommunicationOnOff;

	int										m_iTimeEnchantEventUID;
	boost::timer							m_kTimeEnchantEventTimer;
	std::map<int, KTimeEnchantEventInfo>	m_mapTimeEnchantEventInfo;		// <m_iEventUID, ...>
	std::map<int, KTimeEnchantEventInfo>	m_mapBeginTimeEnchantEventInfo;	// <m_iEventUID, ...>
	std::set<int>							m_setEndTimeEnchantEventInfo;	// <m_iEventUID>

	enum EXCHANGE_LIMIT_INFO
	{
		ELI_CHECK_TERM		= 60,
		ELI_QUANTITY_TERM	= 10,
	};
	typedef	std::list< KExchangeLimitInfo >	EXCHANGE_LIMIT_CONTAINER;
	boost::timer				m_kExchangeLimitCheckTimer;
	int							m_iExchangeLimitUID;
	EXCHANGE_LIMIT_CONTAINER	m_listExchangeLimitInfo;		// 분류되지 않은 제한 정보
	EXCHANGE_LIMIT_CONTAINER	m_listBeginExchangeLimitInfo;	// 진행 중인 제한 정보
	bool						m_bExchangeLimitInfoChanged;
	boost::timer				m_kExchangeLimitQuantityTimer;

	boost::timer	m_kTimerGlobalMission;
};

DefKObjectInline( KGlobalServer, KBaseServer );

//{{ 2013. 05. 29	최육사	제재 리스트 통합
template < class T >
void KGlobalServer::SendToGSUser( UidType nToGSUID, UidType nToGSUSer, unsigned short usEventID, const T& data )
{
	KEvent kEvent;
	kEvent.SetData( PI_GS_USER, NULL, usEventID, data );
	LIF( kEvent.m_kDestPerformer.AddUID( nToGSUSer ) );

	KActorPtr spActor = KActorManager::GetKObj()->Get( nToGSUID );

	if( spActor != NULL )
	{
		spActor->SendPacket( kEvent );
	}
	else
	{
		START_LOG( cerr, L"타 게임서버로 패킷 보내기 실패.? 왜왜" )
			<< BUILD_LOG( nToGSUID )
			<< BUILD_LOG( nToGSUSer )
			<< BUILD_LOG( KEvent::GetIDStr( usEventID ) )
			<< END_LOG;
	}
}
//}}

//다른 게임서버로 자료를 보내기 위해
template < class T >
void KGlobalServer::SendToGameServer( unsigned short usEventID, const T& data ) const
{
	// 연결된 게임서버중 한군데만 전송
	UidType iGameServerUID = KBaseServer::GetKObj()->GetFirstActorKey();
	if( iGameServerUID <= 0 )
	{
		START_LOG( cwarn, L"접속된 게임서버가 없습니다." )
			<< BUILD_LOG( iGameServerUID )
			<< END_LOG;
		return;
	}

	UidType anTrace[2] = { iGameServerUID, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, iGameServerUID, anTrace, usEventID, data );
}

template < class T >
void KGlobalServer::BroadCastAllGS( unsigned short usEventID, const T& data ) const
{
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0, anTrace, usEventID, data );
}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}

//#ifdef	SERV_HERO_PVP_MANAGE_LIST // 적용날짜: 2013-04-04
template < class T >
void KGlobalServer::SendToGSCharacter( UidType nToGSUID, UidType nToUnitUID, unsigned short usEventID, const T& data )
{
	KEvent kEvent;
	kEvent.SetData( PI_GS_CHARACTER, NULL, usEventID, data );
	LIF( kEvent.m_kDestPerformer.AddUID( nToUnitUID ) );

	KActorPtr spActor = KActorManager::GetKObj()->Get( nToGSUID );

	if( spActor != NULL )
	{
		spActor->SendPacket( kEvent );
	}
	else
	{
		START_LOG( cerr, L"타 게임서버로 패킷 보내기 실패.? 왜왜" )
			<< BUILD_LOG( nToGSUID )
			<< BUILD_LOG( nToUnitUID )
			<< BUILD_LOG( KEvent::GetIDStr( usEventID ) )
			<< END_LOG;
	}
}
//#endif	// SERV_HERO_PVP_MANAGE_LIST
