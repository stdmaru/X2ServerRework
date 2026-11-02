#pragma once
#include "Actor.h"
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ActorManager.h"

#include "x2/platform/win32.h"    // ::GetTickCount()


SmartPointer(KLoginUser);

class KLoginUser : public KActor
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclPfID;

	//{{ 2011. 08. 11	최육사	투니 랜드 채널링
	struct SLoginDelayDump
	{
		DWORD		m_dwLoginTick;
		DWORD		m_dwMaxTick;
		DWORD		m_dwLoginAuthCount;
		CTime		m_tRegTime;

		SLoginDelayDump()
		{
			Reset();
		}

		void Reset()
		{
			m_dwLoginTick = ::GetTickCount();
			m_dwMaxTick = 0;
			m_dwLoginAuthCount = 0;
			m_tRegTime = CTime();
		}
	};
	//}}

public:
    KLoginUser(void);
    virtual ~KLoginUser(void);

    // override
    virtual void Tick();

protected:  // util function, Packet Handling

	//{{ 2011. 08. 11	최육사	투니 랜드 채널링
	void LoginBegin();
	void LoginEnd();
	DWORD GetLoginMaxTick() const					{ return m_kLoginTickDump.m_dwMaxTick; }
	const std::wstring GetLoginMaxTickRegDate() const;
	void TickDumpReset()							{ m_kLoginTickDump.Reset(); }
	//}}

    // packet send function
    template < typename T > bool SendPacket( unsigned short usEventID, T& data, bool bLogging = true, bool bCompress = false );
    template < class T > void _SendToAccountDB( unsigned short usEventID_, UidType anTrace[], const T& data_ );
    template < class T > void SendToAccountDB( unsigned short usEventID, const T& data );
	//{{ 2009. 11. 19  최육사	길드구조개선
	template < class T > void SendToGameDB( unsigned short usEventID, const T& data );
	//}}
    template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
    template < class T > void SendToGSUser( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToGSUser( UidType nToGSUID, UidType nToGSUSer, unsigned short usEventID, const T& data );
	template < class T > void SendToGSCharacter( UidType nToUnitUID, unsigned short usEventID, const T& data );
    template < class T > void SendToGameServer( unsigned short usEventID, const T& data );


    void SendToAccountDB( unsigned short usEventID );
	//{{ 2009. 11. 19  최육사	길드구조개선
	void SendToGameDB( unsigned short usEventID );
	//}}
    void SendToGSUser( UidType nTo, unsigned short usEventID );

//#ifdef	SERV_LOCAL_RANKING_SYSTEM // 적용날짜: 2013-04-08	// 해외팀 주석 처리
	template < class T > void SendToGSCharacter( UidType nToGSUID, UidType nToUnitUID, unsigned short usEventID, const T& data );
	void SendToGSCharacter( UidType nToGSUID, UidType nToUnitUID, unsigned short usEventID );
//#endif	// SERV_LOCAL_RANKING_SYSTEM

	//{{ 2011. 07. 19	최육사	서버간 접속 코드 리팩토링
	//#ifdef SERV_SERVER_CONNECT_CODE_REFACTORING
	//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
   _DECL_ON_FUNC( ELG_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ );
	//}}
   _DECL_ON_FUNC( DBE_VERIFY_SERVER_CONNECT_ACK, KECN_VERIFY_SERVER_CONNECT_ACK );
	//#endif SERV_SERVER_CONNECT_CODE_REFACTORING
	//}}
    DECL_ON_FUNC( ELG_NEXON_USER_AUTHENTICATE_REQ );
	//{{ 2009. 5. 26  최육사	채널이동
	DECL_ON_FUNC( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ );
	//}}
	DECL_ON_FUNC( ELG_REGISTER_USER_REQ );
	//{{ 2009. 5. 28  최육사	채널이동
	DECL_ON_FUNC( ELG_CHANNEL_CHANGE_REGISTER_USER_REQ );
	//}}

	DECL_ON_FUNC( ELG_USER_DISCONNECT_REQ );





	DECL_ON_FUNC( ELG_REGISTER_UNIT_NOT );
	DECL_ON_FUNC( ELG_UNIT_DISCONNECT_NOT );
	DECL_ON_FUNC( ELG_SEARCH_UNIT_REQ );
	DECL_ON_FUNC( ELG_GET_UNIT_INFO_ACK );

   _DECL_ON_FUNC( ELG_KNM_REQUEST_NEW_FRIEND_INFO_REQ, KEGS_KNM_REQUEST_NEW_FRIEND_INFO_REQ );
    DECL_ON_FUNC( ELG_KNM_INVITE_REQ );
   _DECL_ON_FUNC( ELG_KNM_TOGETHER_REQ, KEGS_KNM_TOGETHER_REQ );
    DECL_ON_FUNC( ELG_KNM_TOGETHER_BY_UNIT_INFO_ACK );

	DECL_ON_FUNC( ELG_CHAT_REQ );
   _DECL_ON_FUNC( ELG_ADMIN_KICK_USER_REQ, KEGS_ADMIN_KICK_USER_REQ );
	//{{ 2008. 2. 4  최육사
	DECL_ON_FUNC( ELG_SEARCH_BLACK_LIST_UNIT_REQ );
	//}}

	//{{ 2008. 3. 28  최육사  추천인
   _DECL_ON_FUNC( ELG_RECOMMEND_USER_RESULT_NOT, KEGS_RECOMMEND_USER_RESULT_NOT );
	//}}

   //080407.hoons. 사제시스템
   _DECL_ON_FUNC( ELG_TUTORIAL_UPDATE_UNIT_INFO_NOT, KEGS_TUTORIAL_UPDATE_UNIT_INFO_NOT );
    DECL_ON_FUNC( ELG_DEL_TUTORIAL_NOT );
	DECL_ON_FUNC( ELG_REQUEST_TUTORIAL_NOT );
   _DECL_ON_FUNC( ELG_REQUEST_TUTORIAL_REPLY_NOT, KEGS_REQUEST_TUTORIAL_REPLY_NOT );

	//{{ 2008. 6. 20  최육사  선물하기
    DECL_ON_FUNC( ELG_PRESENT_CASH_ITEM_REQ );
   _DECL_ON_FUNC( ELG_PRESENT_MESSAGE_TO_RECEIVER_NOT, KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT );
	//}}

	//{{ 2008. 7. 14  최육사  랭킹
    DECL_ON_FUNC( ELG_GET_RANKING_FOR_GS_RANKING_MANAGER_REQ );
	DECL_ON_FUNC( ELG_NEW_RECORD_HENIR_RANKING_NOT );
	DECL_ON_FUNC( ELG_GET_WEB_RANKING_REFRESH_NOT );
	//}}

	//{{ 2008. 9. 24  최육사	우체국 확장
	DECL_ON_FUNC( ELG_FIND_USER_SEND_LETTER_NOT );
	//}}

	//{{ 2008. 10. 15  최육사	타이틀
   _DECL_ON_FUNC( ELG_INSERT_TITLE_NOT, KDBE_INSERT_TITLE_ACK );
	//}}

	//{{ 2009. 4. 1  최육사		친구차단옵션
	DECL_ON_FUNC( ELG_UPDATE_DENY_FRIEND_SHIP_NOT );
	//}}

	//{{ 2009. 4. 7  최육사		친구정보
   _DECL_ON_FUNC( ELG_FIND_FRIEND_UNIT_INFO_REQ, KEGS_UPDATE_FRIEND_INFO_REQ );
	//}}

	//{{ 2009. 4. 20  최육사	초대하기
	DECL_ON_FUNC( ELG_INVITE_PVP_ROOM_REQ );
	//}}

    DECL_ON_FUNC( ELG_REQUEST_FRIEND_NOT );

    DECL_ON_FUNC( ELG_ACCEPT_FRIEND_NOT );

    DECL_ON_FUNC( ELG_DENY_FRIEND_NOT );

    DECL_ON_FUNC( ELG_BLOCK_FRIEND_NOT );

    DECL_ON_FUNC( ELG_UNBLOCK_FRIEND_NOT );

    DECL_ON_FUNC( ELG_DELETE_FRIEND_NOT );

    DECL_ON_FUNC( ELG_FRIEND_POSITION_UPDATE_NOT );

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 9. 22  최육사	길드

	//{{ 2009. 11. 20  최육사	길드구조개선
	DECL_ON_FUNC( ELG_CREATE_GUILD_RESULT_REQ );
	//}}

	DECL_ON_FUNC( ELG_GET_GUILD_INFO_REQ );
	//{{ 2009. 11. 19  최육사	길드구조개선
	DECL_ON_FUNC( DBE_GET_GUILD_INFO_ACK );
	//}}

	DECL_ON_FUNC( ELG_GET_GUILD_USER_LIST_REQ );

	DECL_ON_FUNC( ELG_INVITE_GUILD_REQ );
	DECL_ON_FUNC( ELG_INVITE_GUILD_FIND_USER_ACK );

	DECL_ON_FUNC( ELG_INVITE_GUILD_REPLY_REQ );
	DECL_ON_FUNC( ELG_JOIN_GUILD_REQ );

	DECL_ON_FUNC( ELG_CHECK_CHANGE_GUILD_MEMBER_GRADE_REQ );

   _DECL_ON_FUNC( ELG_UPDATE_CHANGE_GUILD_MEMBER_GRADE_NOT, KDBE_CHANGE_GUILD_MEMBER_GRADE_ACK );
   _DECL_ON_FUNC( ELG_UPDATE_CHANGE_GUILD_MESSAGE_NOT, KDBE_CHANGE_GUILD_MESSAGE_ACK );
   _DECL_ON_FUNC( ELG_UPDATE_KICK_GUILD_MEMBER_NOT, KDBE_KICK_GUILD_MEMBER_ACK );
   _DECL_ON_FUNC( ELG_UPDATE_CHANGE_GUILD_MEMBER_MESSAGE_NOT, KDBE_CHANGE_GUILD_MEMBER_MESSAGE_ACK );
   _DECL_ON_FUNC( ELG_UPDATE_DISBAND_GUILD_NOT, KDBE_DISBAND_GUILD_ACK );

	DECL_ON_FUNC( ELG_UPDATE_GUILD_MEMBER_INFO_NOT );

	DECL_ON_FUNC( ELG_CHECK_EXPAND_GUILD_MAX_MEMBER_REQ );
   _DECL_ON_FUNC( DLG_UPDATE_EXPAND_GUILD_MAX_MEMBER_NOT, KDBE_EXPAND_GUILD_MAX_MEMBER_ACK );

	//{{ 2009. 10. 27  최육사	길드레벨
   _DECL_ON_FUNC( ELG_UPDATE_GUILD_EXP_NOT, KDBE_UPDATE_GUILD_EXP_ACK );
	//}}

   //{{ 2012. 02. 22	박세훈	길드 이름 변경권
   DECL_ON_FUNC( ELG_CREATE_GUILD_REQ );
   DECL_ON_FUNC( DBE_CREATE_GUILD_ACK );
   DECL_ON_FUNC( ELG_CHANGE_GUILD_NAME_CHECK_REQ );
   DECL_ON_FUNC( DBE_CHANGE_GUILD_NAME_CHECK_ACK );
   DECL_ON_FUNC( ELG_CHANGE_GUILD_NAME_TIME_CHECK_REQ );
   DECL_ON_FUNC( ELG_CHANGE_GUILD_NAME_REQ );
   DECL_ON_FUNC( DBE_CHANGE_GUILD_NAME_ACK );
   DECL_ON_FUNC( ELG_CHANGE_GUILD_NAME_RESULT_REQ );
   //}}

	//}}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2009. 10. 27  최육사	길드레벨
	DECL_ON_FUNC( ELG_FIND_DELETE_CHAR_IN_RANKING_NOT );
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 11. 23  최육사	길드스킬

	DECL_ON_FUNC( ELG_GET_GUILD_SKILL_REQ );
	DECL_ON_FUNC( DBE_GET_GUILD_SKILL_ACK );

	DECL_ON_FUNC( ELG_RESET_GUILD_SKILL_REQ );
   _DECL_ON_FUNC( ELG_RESET_GUILD_SKILL_DEL_ITEM_ACK, KDBE_RESET_GUILD_SKILL_ACK );

	DECL_ON_FUNC( ELG_INIT_GUILD_SKILL_REQ );
   _DECL_ON_FUNC( ELG_INIT_GUILD_SKILL_DEL_ITEM_ACK, KDBE_INIT_GUILD_SKILL_ACK );

	DECL_ON_FUNC( ELG_INSERT_GUILD_CASH_SKILL_POINT_NOT );

	DECL_ON_FUNC( ELG_ADMIN_GET_GUILD_SKILL_POINT_REQ );

	//}}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2010. 01. 13  최육사	길드게시판

	DECL_ON_FUNC( ELG_GET_GUILD_AD_LIST_REQ );
	DECL_ON_FUNC( ELG_GET_GUILD_SKILL_IN_BOARD_REQ );
	DECL_ON_FUNC( DBE_GET_GUILD_SKILL_IN_BOARD_ACK );

	DECL_ON_FUNC( ELG_REGISTRATION_GUILD_AD_REQ );
	DECL_ON_FUNC( DBE_REGISTRATION_GUILD_AD_ACK );

	DECL_ON_FUNC( ELG_MODIFY_REG_GUILD_AD_REQ );
	DECL_ON_FUNC( DBE_MODIFY_REG_GUILD_AD_ACK );

	DECL_ON_FUNC( ELG_APPLY_JOIN_GUILD_REQ );
	DECL_ON_FUNC( DBE_APPLY_JOIN_GUILD_ACK );

	DECL_ON_FUNC( ELG_GET_APPLY_JOIN_GUILD_LIST_REQ );

	DECL_ON_FUNC( ELG_ACCEPT_JOIN_GUILD_REQ );
	DECL_ON_FUNC( DBE_DELETE_APLLY_FOR_ACCEPT_JOIN_GUILD_ACK );

	DECL_ON_FUNC( ELG_DELETE_APPLY_JOIN_GUILD_REQ );
	DECL_ON_FUNC( DBE_DELETE_APPLY_JOIN_GUILD_ACK );

	//}}

	//{{ 2010. 01. 11  최육사	추천인리스트
   _DECL_ON_FUNC( ELG_GET_RECOMMEND_USER_LIST_REQ, KDBE_GET_RECOMMEND_USER_LIST_ACK );
	DECL_ON_FUNC( ELG_RECOMMEND_USER_INFO_NOT );
	//}}

	//{{ 2010. 05. 31  최육사	동접 정보 개편
	DECL_ON_FUNC( ELG_UPDATE_CCU_INFO_NOT );
	//}}
	//{{ 2010. 8. 16	최육사	기간 리셋 아이템 이벤트
   _DECL_ON_FUNC( ELG_ADMIN_SHOW_ED_CHEAT_REQ, KEGS_ADMIN_SHOW_ED_CHEAT_REQ );
	DECL_ON_FUNC( ELG_ADMIN_SHOW_ED_CHEAT_ACK );
	//}}
	//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	DECL_ON_FUNC( ELG_REG_REJECTED_USER_NOT );
	//}}
	//{{ 2011. 03. 02	최육사	개인 거래 차단
   _DECL_ON_FUNC( ELG_REG_TRADE_BLOCK_USER_NOT, KELG_REG_REJECTED_USER_NOT );
	//}}
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	//{{ 2010. 10. 13	최육사	DLL Manager
   _DECL_ON_FUNC( ELG_MODULE_INFO_UPDATE_NOT, KEGS_MODULE_INFO_UPDATE_NOT );
	//}}
	//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	DECL_ON_FUNC( ELG_CHECK_TIME_RANDOM_ITEM_EVENT_REQ );
	//}}
	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
   _DECL_ON_FUNC( ELG_CHECK_MACHINE_ID_REQ, KEGS_CHECK_MACHINE_ID_REQ );
   //}}
	//{{ 2011. 08. 11	최육사	투니 랜드 채널링
	DECL_ON_FUNC_NOPARAM( ELG_DUMP_LOGIN_USER_NOT );
	//}}

	//{{ 2011. 12. 23	최육사	배틀필드 시스템
	DECL_ON_FUNC( ELG_INVITE_PARTY_FIND_RECEIVER_REQ );
   _DECL_ON_FUNC( DBE_CHECK_NICKNAME_FOR_INVITE_PARTY_ACK, KELG_INVITE_PARTY_FIND_RECEIVER_ACK );
   _DECL_ON_FUNC( ELG_INVITE_PARTY_GET_RECEIVER_USER_INFO_ACK, KELG_INVITE_PARTY_FIND_RECEIVER_ACK );
	//}}

   //{{ 버프 관리 시스템 - 김민성
   DECL_ON_FUNC( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ );
   //}

   DECL_ON_FUNC( ELG_ADMIN_DELETE_GUILD_ADD_NOT );

   //{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
   DECL_ON_FUNC( ELG_ADMIN_DELETE_HENIR_RANKING_RANK_NOT );
   DECL_ON_FUNC( ELG_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT );
   DECL_ON_FUNC( ELG_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT );
   //}}


   //{{ 2013. 04. 01	 인연 시스템 - 김민성
   DECL_ON_FUNC( ELG_COUPLE_PROPOSE_CHECK_CONNECT_USER_REQ );
   _DECL_ON_FUNC( ELG_COUPLE_PROPOSE_RESULT_SUCCESS_NOT, KERM_COUPLE_PROPOSE_RESULT_SUCCESS_NOT );
   _DECL_ON_FUNC( ELG_COUPLE_PROPOSE_AGREE_CHECK_NOT, KEGS_COUPLE_PROPOSE_AGREE_NOT );
   _DECL_ON_FUNC( ELG_COUPLE_PROPOSE_RESULT_FAIL_NOT, KERM_COUPLE_PROPOSE_RESULT_FAIL_NOT );
   DECL_ON_FUNC( ELG_RELATIONSHIP_INFO_REQ );
   _DECL_ON_FUNC( DBE_RELATIONSHIP_INFO_GAME_DB_ACK, KEGS_RELATIONSHIP_INFO_ACK );
   _DECL_ON_FUNC( ELG_RELATIONSHIP_INFO_GAME_SERVER_ACK, KEGS_RELATIONSHIP_INFO_ACK );
   DECL_ON_FUNC( ELG_WEDDING_ITEM_FIND_INFO_REQ );
   _DECL_ON_FUNC( DBE_WEDDING_ITEM_FIND_INFO_ACK, KELG_WEDDING_ITEM_FIND_INFO_REQ );
   DECL_ON_FUNC( ELG_ADD_WEDDING_HALL_INFO_NOT );
   DECL_ON_FUNC( ELG_CALL_MY_LOVER_FIND_USER_VILLAGE_REQ );
   DECL_ON_FUNC( ELG_CALL_MY_LOVER_AGREE_RESULT_VILLAGE_NOT );
   DECL_ON_FUNC( ELG_CALL_MY_LOVER_FIND_USER_ROOM_REQ );
   DECL_ON_FUNC( ELG_CALL_MY_LOVER_AGREE_RESULT_ROOM_NOT );
   DECL_ON_FUNC( ELG_CALL_MY_LOVER_JOIN_BATTLE_FIELD_FAILD_NOT );
   DECL_ON_FUNC( ELG_CHECK_WEDDING_HALL_INFO_REQ );
   DECL_ON_FUNC( ELG_BREAK_UP_NOT );
   DECL_ON_FUNC( ELG_CHANGE_LOVE_WORD_NOT );
   //}

	DECL_ON_FUNC( ELG_EVENT_PROPOSE_CHECK_CONNECT_USER_REQ );
	_DECL_ON_FUNC( ELG_EVENT_PROPOSE_AGREE_CHECK_NOT, KEGS_EVENT_PROPOSE_AGREE_NOT );
	_DECL_ON_FUNC( ELG_EVENT_PROPOSE_RESULT_SUCCESS_NOT, KERM_EVENT_PROPOSE_RESULT_SUCCESS_NOT );
	DECL_ON_FUNC( ELG_DIVORCE_NOT );

    DECL_ON_FUNC_NOPARAM( E_RESERVE_DESTROY );



    bool RoutePacket( const KEvent* pkEvent );   // CnUser, GSUser가 같이 선언하지만 상속은 아님.
    //                          ^ KEvent가 수정되지 않아야 한다. (SmartPtr로는 불가능)

    // override.
    virtual void ProcessEvent( const KEventPtr& spEvent );
    virtual void OnDestroy();   // LOGINED 상태로 빠져나오고, DB data update를 신청한다


	//{{ 2011. 08. 11	최육사	투니 랜드 채널링
private:
	SLoginDelayDump				m_kLoginTickDump;
	//}}
};

template < typename T >
bool KLoginUser::SendPacket( unsigned short usEventID, T& data, bool bLogging, bool bCompress )
{
	return KSession::SendPacket( PI_LOGIN_PROXY, GetUID(), NULL, usEventID, data, bLogging, bCompress );
}

template < class T >
void KLoginUser::_SendToAccountDB( unsigned short usEventID_, UidType anTrace_[], const T& data_ )
{
    UidType anTrace[2] = { -1, -1 };
    if( anTrace_ != NULL )  // 외부에서 넘어온 값이 있다면 복사한다.
    {
        ::memcpy( anTrace, anTrace_, sizeof( anTrace ) );
    }

    if( anTrace[0] == -1 )
        anTrace[0] = GetUID();
    else
        anTrace[1] = GetUID();

    KncSend( GetPfID(), GetUID(), PI_ACCOUNT_DB, 0, anTrace, usEventID_, data_ );
}

template < class T >
void KLoginUser::SendToAccountDB( unsigned short usEventID, const T& data )
{
    _SendToAccountDB( usEventID, NULL, data );
}

//{{ 2009. 11. 19  최육사	길드구조개선
template < class T >
void KLoginUser::SendToGameDB( unsigned short usEventID_, const T& data_ )
{
	UidType anTrace[2] = { -1, -1 };

	if( anTrace[0] == -1 )
		anTrace[0] = GetUID();
	else
		anTrace[1] = GetUID();

	KncSend( GetPfID(), GetUID(), PI_LOGIN_GAME_DB, 0, anTrace, usEventID_, data_ );
}
//}}

template < class T >
void KLoginUser::SendToLogDB( unsigned short usEventID_, const T& data_ )
{
    UidType anTrace[2] = { -1, -1 };

    if( anTrace[0] == -1 )
        anTrace[0] = GetUID();
    else
        anTrace[1] = GetUID();

    KncSend( GetPfID(), GetUID(), PI_LOG_DB, 0, anTrace, usEventID_, data_ );
}

template < class T >
void KLoginUser::SendToGSUser( UidType nTo, unsigned short usEventID, const T& data )
{

    KEvent kEvent;
    kEvent.SetData( PI_GS_USER, NULL, usEventID, data );
    LIF( kEvent.m_kDestPerformer.AddUID( nTo ) );
    KSession::SendPacket( kEvent );
}
//다른 게임서버로 자료를 보내기 위해
template < class T >
void KLoginUser::SendToGSUser( UidType nToGSUID, UidType nToGSUser, unsigned short usEventID, const T& data )
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
		START_LOG( cerr, L"Failed to send a packet to another game server. Why?" )
			<< BUILD_LOG( nToGSUID )
			<< BUILD_LOG( nToGSUser )
			<< BUILD_LOG( KEvent::GetIDStr( usEventID ) )
			<< END_LOG;
	}
}

template < class T >
void KLoginUser::SendToGSCharacter( UidType nToUnitUID, unsigned short usEventID, const T& data )
{

	KEvent kEvent;
	kEvent.SetData( PI_GS_CHARACTER, NULL, usEventID, data );
	LIF( kEvent.m_kDestPerformer.AddUID( nToUnitUID ) );
	KSession::SendPacket( kEvent );
}

template < class T >
void KLoginUser::SendToGameServer( unsigned short usEventID, const T& data )
{

	UidType anTrace[2] = { GetUID(), -1 };
	KEvent kEvent;
	kEvent.SetData( PI_GS_SERVER, anTrace, usEventID, data );
	KSession::SendPacket( kEvent );
}


//#ifdef	SERV_LOCAL_RANKING_SYSTEM // 적용날짜: 2013-04-08	// 해외팀 주석 처리
template < class T >
void KLoginUser::SendToGSCharacter( UidType nToGSUID, UidType nToUnitUID, unsigned short usEventID, const T& data )
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
		START_LOG( cerr, L"Failed to send a packet to another game server. Why?" )
			<< BUILD_LOG( nToGSUID )
			<< BUILD_LOG( nToUnitUID )
			<< BUILD_LOG( KEvent::GetIDStr( usEventID ) )
			<< END_LOG;
	}
}
//#endif	// SERV_LOCAL_RANKING_SYSTEM
