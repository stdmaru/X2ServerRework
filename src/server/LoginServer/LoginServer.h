#pragma once
#include "BaseServer.h"
#include "KncSend.h"

//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
//}}
class KLoginServer : public KBaseServer
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclDump;
    DeclPfID;

public:
    KLoginServer(void);
    virtual ~KLoginServer(void);

    // derived from KBaseServer
    virtual bool Init();
    virtual void RegToLua();
    virtual void ShutDown();
    virtual bool DestroyAllSession();         // 접속중인 세션의 종료.

    // derived from KPerformer
    virtual void ProcessEvent( const KEventPtr& spEvent );
    virtual void Tick();

    //////////////////////////////////////////////////////////////////////////
	//{{ 2011. 10. 26	최육사	DB해킹 트랩
	void CheckDBHackingCheck();
	//}}

	//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
	//}}
protected:
	void WriteServerInfoToDB();
	virtual void OnServerReadyComplete();
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

	template < class T > void SendToGSUser( UidType nToGSUID, UidType nToGSUSer, unsigned short usEventID, const T& data );
	template < class T > void SendToGameServer( unsigned short usEventID, const T& data ) const;
	template < class T > void BroadCastAllGS( unsigned short usEventID, const T& data ) const;
	template < class T > void SendToGameDB( unsigned short usEventID, const T& data );

	//{{ 2010. 02. 24  최육사	웹 포인트 이벤트
	//}}
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

   _DECL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK );
	DECL_ON_FUNC_NOPARAM( DBE_UPDATE_SERVER_INFO_ACK );

	DECL_ON_FUNC( EPUBLISHER_REG_USER_ACK );
	DECL_ON_FUNC( EPUBLISHER_UNREG_USER_ACK );

// 현재 사용하지 않는 코드로 보임
// #ifdef SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN
// 	DECL_ON_FUNC( DBE_GET_GIANT_DELETED_UNIT_LIST_ACK );
// #endif //SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN

	//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
	DECL_ON_FUNC( ENX_AUTH_LOGIN_ACK );
	DECL_ON_FUNC( ENX_AUTH_TERMINATE_NOT );
	DECL_ON_FUNC( ENX_AUTH_MESSAGE_NOT );
	DECL_ON_FUNC( ENX_AUTH_SYNCHRONIZE_REQ );
	//}}

	//{{ 2009. 7. 6  최육사		랭킹 개편
	DECL_ON_FUNC( DBE_GET_HENIR_RANKING_INFO_ACK );
   _DECL_ON_FUNC( ELG_CLEAR_RANKING_NOT, bool );
	DECL_ON_FUNC_NOPARAM( DMP_DUMP_RANKING_MANAGER_NOT );
	DECL_ON_FUNC( ELG_ADMIN_NEW_RECORD_TEST_NOT );
	DECL_ON_FUNC( ELG_ADMIN_NEW_RECORD_NOT );
	DECL_ON_FUNC( ELG_ADMIN_DELETE_RECORD_NOT );
	//}}
	//{{ 2009. 11. 9  최육사	로그인서버GameDB
	DECL_ON_FUNC( DBE_GET_WEB_RANKING_INFO_ACK );
	//}}

	//{{ 2009. 11. 26  최육사	길드스킬
	DECL_ON_FUNC( DBE_GUILD_LEVEL_UP_ACK );
	//}}

	//{{ 2009. 11. 26  최육사	길드스킬
	DECL_ON_FUNC( DBE_INSERT_GUILD_CASH_SKILL_POINT_ACK );
	DECL_ON_FUNC( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK );

   _DECL_ON_FUNC( DBE_ADMIN_GET_GUILD_SKILL_POINT_ACK, KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK );
	//}}

	//{{ 2010. 01. 15  최육사	길드게시판
	DECL_ON_FUNC( DBE_GET_GUILD_BOARD_INFO_ACK );
	DECL_ON_FUNC( DBE_ACCEPT_JOIN_GUILD_ACK );
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

	//{{ 2010. 06. 22  최육사	계정블럭기능
	DECL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK );
	//}}

	//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신
   _DECL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT );
   _DECL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	//}}

	//{{ 2010. 10. 13	최육사	DLL Manager
	DECL_ON_FUNC( DBE_GET_MODULE_INFO_LIST_ACK );
   _DECL_ON_FUNC( DBE_UPDATE_MODULE_INFO_LIST_ACK, KDBE_GET_MODULE_INFO_LIST_ACK );
	//}}

   //{{ 2010. 10. 12	최육사	서버 모니터링
   //}}

	//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	DECL_ON_FUNC( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_ACK );
	//}}

	//{{ 2011. 04. 18	최육사	대리 상인
	DECL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_ACK );
	DECL_ON_FUNC( DBE_LOAD_PSHOP_AGENCY_ACK );
   _DECL_ON_FUNC( DBE_BREAK_PSHOP_AGENCY_ACK, KERM_BREAK_PSHOP_AGENCY_NOT );
   _DECL_ON_FUNC( ERM_PSHOP_AGENCY_MESSAGE_NOT, KEGS_PSHOP_AGENCY_MESSAGE_NOT );
	//}}
	//{{ 2011. 10. 26	최육사	DB해킹 트랩
   _DECL_ON_FUNC( DBE_CHECK_DB_HACKING_TRAP_ACK, int );
	//}}
	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	DECL_ON_FUNC( ELG_RANKING_TITLE_REWARD_NOT );
	DECL_ON_FUNC( DBE_INSERT_TITLE_ACK );
	//}}

	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	//}}


	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	DECL_ON_FUNC( DBE_LOAD_WEDDING_HALL_INFO_ACK );
	_DECL_ON_FUNC( ELG_ADD_WEDDING_HALL_INFO_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK );
	//}


	//{{ 2010. 06. 22  최육사	OTP적용
public:
	void SetPassportLevel_LUA( u_int uiLevel ) { m_uiPassportLevel = uiLevel; }
private:
	u_int					m_uiPassportLevel;
	//}}
	//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
	//}}
	boost::timer					m_tTimeProcessCommunicationONOFF;
	bool							m_bServerRunningProcessCommunicationOnOff;

};

DefKObjectInline( KLoginServer, KBaseServer );

//다른 게임서버로 자료를 보내기 위해
template < class T >
void KLoginServer::SendToGSUser( UidType nToGSUID, UidType nToGSUser, unsigned short usEventID, const T& data )
{
	KEvent kEvent;
	kEvent.SetData( PI_GS_USER, NULL, usEventID, data );
	LIF( kEvent.m_kDestPerformer.AddUID( nToGSUser ) );

	KActorPtr spActor = KActorManager::GetKObj()->Get( nToGSUID );

	if( spActor != NULL )
	{
		spActor->SendPacket( kEvent );
	}
	else
	{
		START_LOG( cerr, L"타 게임서버로 패킷 보내기 실패.? 왜왜" )
			<< BUILD_LOG( nToGSUID )
			<< BUILD_LOG( nToGSUser )
			<< BUILD_LOG( KEvent::GetIDStr( usEventID ) )
			<< END_LOG;
	}
}

template < class T >
void KLoginServer::SendToGameServer( unsigned short usEventID, const T& data ) const
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
	KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, iGameServerUID, anTrace, usEventID, data );
}

template < class T >
void KLoginServer::BroadCastAllGS( unsigned short usEventID, const T& data ) const
{
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0, anTrace, usEventID, data );
}

template < class T >
void KLoginServer::SendToGameDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_LOGIN_GAME_DB, 0, NULL, usEventID, data );
}

//{{ 2010. 02. 24  최육사	웹 포인트 이벤트
//}}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}

//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
//}}
