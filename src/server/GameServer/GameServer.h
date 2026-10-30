#pragma once
#include "BaseServer.h"

// Port: nProtect (ggsrv25.h) and HackShield (AntiCpXSvr.h) removed with the anti-cheat (T10)


class KGameServer : public KBaseServer
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclDump;
    DeclPfID;

public:
	enum EXCHANGE_LIMIT_RESULT
	{
		ELR_NOT_INITED,
		ELR_EXHAUSTED,
		ELR_CHECK,
		ELR_PASS,
	};

    KGameServer(void);
    virtual ~KGameServer(void);

    virtual bool Init();
    virtual void RegToLua();
    virtual void ShutDown();
    virtual bool DestroyAllSession();           // 접속중인 세션의 종료.

    virtual void ProcessEvent( const KEventPtr& spEvent );

    void Tick();

    //////////////////////////////////////////////////////////////////////////

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	void RequestRandomQuestInfo();
	//}}

	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int GetMaxUser() const;
	//}}



	void	GetTimeEnchantEventUID( OUT int& iTimeEnchantEventUID ) const{	iTimeEnchantEventUID = m_iTimeEnchantEventUID;	}
	bool	PopFirstTimeEnchantEvent( void );

	void	GetExchangeLimitUID( OUT int& iExchangeLimitUID ) const{	iExchangeLimitUID = m_iExchangeLimitUID;	}
	int		DecreaseExchangeLimit( IN const int iDestItemID );

protected:
	void ServerVersion();
    void WriteServerInfoToDB();
	virtual void OnServerReadyComplete();
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}



    // packet send function
	template < class T > void SendToCnServer( unsigned short usEventID, const T& data );
	void SendToCnServer( unsigned short usEventID );

	template < class T > void SendToLoginServer( unsigned short usEventID, const T& data );
	void SendToLoginServer( unsigned short usEventID );

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	template < class T > void SendToGlobalServer( unsigned short usEventID, const T& data );
	void SendToGlobalServer( unsigned short usEventID );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	template < class T > void SendToGameDB( unsigned short usEventID, const T& data );

	//{{ 2010. 02. 24  최육사	웹 포인트 이벤트
	//}}


    template < class T > void SendToLogDB2nd( unsigned short usEventID, const T& data );

	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}


	void	CheckGSFieldBossSystemRequest( IN const CTime tCurrentTime );
	void	CheckGSFieldBossSystem( IN const CTime tCurrentTime );

   _DECL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_GAME_SERVER_ON_ACK );
   _DECL_ON_FUNC( DBE_UPDATE_SERVER_INFO_ACK, KServerList );
	//{{ 2009. 2. 13  최육사	채널리스트
	DECL_ON_FUNC( DBE_CHANNEL_LIST_ACK );
	//}}
    DECL_ON_FUNC( ERM_ROOM_LIST_INFO_NOT );
	DECL_ON_FUNC( ERM_SQUARE_LIST_INFO_NOT );

   _DECL_ON_FUNC( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT );
	DECL_ON_FUNC( ENX_UPDATE_PRODUCT_LIST_NOT );
	DECL_ON_FUNC( DBE_EVENT_UPDATE_ACK );

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}

	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	//}}
	DECL_ON_FUNC( DBE_INSERT_TITLE_ACK );
	DECL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_ACK );
	//{{ 2008. 10. 31  최육사	어뷰저 리스트
	DECL_ON_FUNC( DBE_ABUSER_LIST_ACK );
	//}}
	//{{ 2008. 11. 5  최육사	광장 개편
	DECL_ON_FUNC( ERM_UPDATE_FIELD_INFO_NOT );
	//}}
	//{{ 2009. 4. 22  최육사	대전유저리스트
	DECL_ON_FUNC( ECN_NEW_PVP_USER_INFO_NOT );
	DECL_ON_FUNC( ECN_UPDATE_PVP_USER_INFO_NOT );
   _DECL_ON_FUNC( ECN_CHECK_INVALID_PVP_USER_NOT, UidType );
	//}}

	//{{ 2009. 6. 1  최육사		채널이동
	DECL_ON_FUNC( ECN_CHECK_CHANNEL_CHANGE_REQ );
	//}}

	//{{ 2009. 6. 3  최육사		확성기
   _DECL_ON_FUNC( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT );
	//}}

	//{{ 2009. 7. 6  최육사		랭킹 개편
	DECL_ON_FUNC( ELG_GET_RANKING_FOR_GS_RANKING_MANAGER_ACK );
	DECL_ON_FUNC( ELG_HENIR_RANKING_REFRESH_NOT );
	DECL_ON_FUNC( ELG_WEB_RANKING_REFRESH_NOT );
	//}}

	//{{ 2009. 7. 10  최육사	사기 시스템
	DECL_ON_FUNC( ERM_INCREASE_PARTY_FEVER_NOT );
	//}}
	//{{ 2012. 05. 10	최육사	배틀필드 시스템
	DECL_ON_FUNC( ERM_MAX_PARTY_FEVER_NOT );
	//}}

	//{{ 2009. 10. 30  최육사	길드레벨
	DECL_ON_FUNC( DBE_JOIN_GUILD_ACK );

	DECL_ON_FUNC( ELG_KICK_RESULT_LETTER_NOT );

	DECL_ON_FUNC( DBE_EXPAND_GUILD_MAX_MEMBER_ACK );

	DECL_ON_FUNC( DBE_UPDATE_GUILD_EXP_ACK );

   _DECL_ON_FUNC( ELG_GUILD_LEVEL_UP_NOTIFY_NOT, KEGS_NOTIFY_MSG_NOT );
	//}}

	//{{ 2010. 02. 01  최육사	길드 게시판
   _DECL_ON_FUNC( ELG_SEND_LETTER_JOIN_GUILD_NOT, KDBE_ACCEPT_JOIN_GUILD_ACK );
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	DECL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_ACK );
	//}}

	//{{ 2010. 02. 17  최육사	채널파티통합
	DECL_ON_FUNC( EPM_OPEN_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_CLOSE_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_ADD_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_DEL_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_UPDATE_PARTY_IN_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_ADD_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_DEL_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	DECL_ON_FUNC( EPM_UPDATE_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	//}}
	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	DECL_ON_FUNC( EPM_UPDATE_PARTY_FEVER_IN_PARTY_LIST_NOT );
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

    DECL_ON_FUNC( ERM_PERSONAL_SHOP_INFO_NOT );

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
   _DECL_ON_FUNC( ESR_ORDER_TO_GAME_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT );
   _DECL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	//}}


	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

   //{{ 2011. 04. 13  김민성  글로벌 서버 추가
	_DECL_ON_FUNC( EGB_UPDATE_MISSION_INFO_NOT, KEGB_GET_MISSION_INFO_ACK );
	DECL_ON_FUNC( EGB_GET_MISSION_INFO_ACK );
	DECL_ON_FUNC( EGB_UPDATE_WORLD_BUFF_NOT );
	DECL_ON_FUNC( EGB_UPDATE_MISSION_START_TIME_NOT );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	DECL_ON_FUNC( EGB_TODAY_RANDOM_QUEST_ACK );
	//}}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}

	//{{ 2012. 03. 23	박세훈	어둠의 문 개편
	DECL_ON_FUNC( EGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT );
	//}}

	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}
	//{{ 2011. 11. 2	최육사	배틀필드 시스템
	DECL_ON_FUNC( ERM_BATTLE_FIELD_ROOM_INFO_NOT );
	DECL_ON_FUNC( EGB_GET_AUTO_PARTY_BONUS_INFO_ACK );
	DECL_ON_FUNC( EGB_UPDATE_AUTO_PARTY_BONUS_INFO_NOT );
	DECL_ON_FUNC( ERM_END_GAME_REGROUP_PARTY_NOT );
	DECL_ON_FUNC( ERM_COMPLETE_RETURN_TO_FIELD_NOT );
	//}}
	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}
	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	DECL_ON_FUNC( DBE_CHECK_EVENT_UPDATE_ACK );


	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	_DECL_ON_FUNC( ELG_WEDDING_HALL_INFO_UPDATE_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK );
	DECL_ON_FUNC( ERM_WEDDING_HALL_INFO_NOT );
	//}

	DECL_ON_FUNC( EGB_TIME_ENCHANT_EVENT_INFO_NOT );
	DECL_ON_FUNC( EGB_TIME_ENCHANT_EVENT_INFO_ACK );

	DECL_ON_FUNC( EGB_EXCHANGE_LIMIT_INFO_NOT );
	DECL_ON_FUNC( EGB_EXCHANGE_LIMIT_INFO_ACK );





	//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
	_DECL_ON_FUNC( EGB_UPDATE_GLOBAL_MISSION_INFO_NOT, KEGB_GET_GLOBAL_MISSION_INFO_ACK );
	DECL_ON_FUNC( EGB_GET_GLOBAL_MISSION_INFO_ACK );
	DECL_ON_FUNC( EGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT );
	//}} 2012. 09. 03	임홍락	글로벌 미션 매니저

	DECL_ON_FUNC( ERM_UPDATE_TOTAL_DANGEROUS_VALUE_NOT );
	DECL_ON_FUNC_NOPARAM( EGB_VERIFY_SERVER_CONNECT_NOT );
	DECL_ON_FUNC_NOPARAM( EGB_VERIFY_SERVER_DISCONNECT_NOT );
	DECL_ON_FUNC( EGB_BATTLE_FIELD_BOSS_INFO_NOT );

    _DECL_ON_FUNC( DBE_CHANNEL_LIST_2ND_ACK, KDBE_CHANNEL_LIST_ACK );

	_DECL_ON_FUNC( EGS_ADD_COMMON_FLAG_NOT, KEGS_ADD_COMMON_FLAG_NOT );
	_DECL_ON_FUNC( EGS_DEL_COMMON_FLAG_NOT, KEGS_DEL_COMMON_FLAG_NOT );


protected:






private:
	x2::Timer	m_kTimeEnchantEventTimer;
	int				m_iTimeEnchantEventUID;
	int				m_iNumOfTimeEnchantEvent;

	int						m_iExchangeLimitUID;
	std::map< int, int >	m_mapExchangeLimitInfo;	// <m_iDestItemID, m_iQuantity>

};

DefKObjectInline( KGameServer, KBaseServer );

template < class T >
void KGameServer::SendToCnServer( unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_SERVER, GetUID(), PI_CN_SERVER, 0, NULL, usEventID, data );
}

template < class T >
void KGameServer::SendToLoginServer( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_SERVER, GetUID(), PI_LOGIN_USER, 0, NULL, usEventID, data );
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
template < class T >
void KGameServer::SendToGlobalServer( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_SERVER, GetUID(), PI_GLOBAL_USER, 0, NULL, usEventID, data );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가


template < class T >
void KGameServer::SendToGameDB( unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), GetUID(), PI_GS_GAME_DB, 0, NULL, usEventID, data );
}

//{{ 2010. 02. 24  최육사	웹 포인트 이벤트
//}}


template < class T >
void KGameServer::SendToLogDB2nd( unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( GetPfID(), GetUID(), PI_GS_LOG_DB_2ND, 0, anTrace, usEventID, data );
}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}
