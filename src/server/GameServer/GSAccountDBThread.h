#pragma once

#include "SubThread.h"
#include "ODBC/Odbc.h"
//{{ 2009. 8. 17  최육사	sp profiler
#include "ODBC/DBUtil.h"
//}}
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

class KGSAccountDBThread : public KSubThread
{
    DeclareDBThread( KGSAccountDBThread );
	//{{ 2009. 8. 17  최육사	sp profiler
	DeclareSPProfiler;
	//}}
    DeclPfID;

public:
    KGSAccountDBThread();
    virtual ~KGSAccountDBThread(void);

protected:
    // derived from KSubThread
    virtual inline void ProcessEvent( const KEventPtr& spEvent );

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );

	int  GetMessengerSN( IN UidType iUnitUID, OUT u_int& uiKNMSerialNum ); //ret val : iOK

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	bool CheckIsComeBackUser( IN std::wstring& wstrLastDate, OUT int& iRewardStep, OUT std::wstring& wstrEndBuffDate, IN std::map< int, int >& mapComeBackRewardCondition );
	bool CheckComeBackFirst( IN std::wstring& wstrEndDate );
	//}}

    // event handler
	//{{ 2009. 12. 22  최육사	서버군확장
	DECL_ON_FUNC( ELOG_STAT_PLAY_TIME );
	//}}

    DECL_ON_FUNC_NOPARAM( DBE_UPDATE_LOGIN_TIME_NOT );
    DECL_ON_FUNC( EGS_CHANGE_OPTION_PLAY_GUIDE_REQ );

   _DECL_ON_FUNC( DBE_ADMIN_CHANGE_AUTH_LEVEL_REQ, KEGS_ADMIN_CHANGE_AUTH_LEVEL_REQ );
   _DECL_ON_FUNC( DBE_ADMIN_GET_AUTH_LEVEL_LIST_REQ, KEGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ );

   _DECL_ON_FUNC( DBE_ACCOUNT_CREATE_UNIT_REQ, KEGS_CREATE_UNIT_ACK );
   //{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
   _DECL_ON_FUNC( DBE_ACCOUNT_SELECT_UNIT_REQ, KDBE_SELECT_UNIT_ACK );
   //}}
	//{{ 2009. 6. 4  최육사	캐릭터 선택
   _DECL_ON_FUNC( DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_REQ, KDBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_ACK );
	//}}

   _DECL_ON_FUNC( DBE_ACCOUNT_DELETE_UNIT_REQ, KEGS_FINAL_DELETE_UNIT_ACK );

	//{{ 2008. 3. 28  최육사  추천인
	DECL_ON_FUNC_NOPARAM( DBE_RECOMMEND_USER_NOT );
	//}}

   _DECL_ON_FUNC( DBE_AGREE_HACK_USER_REQ, UidType );
   _DECL_ON_FUNC( DBE_REPORT_HACK_USER_NOT, UidType );

   _DECL_ON_FUNC( DBE_GET_MY_MESSENGER_SN_REQ, KEGS_GET_MY_MESSENGER_SN_REQ );

   //{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
	DECL_ON_FUNC( DBE_JOIN_ACCOUNT_CHECK_IP_LOG_NOT );
   //}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	DECL_ON_FUNC( DBE_GET_SECOND_SECURITY_INFO_REQ );
	DECL_ON_FUNC( DBE_SEUCCESS_SECOND_SECURITY_REQ );
	_DECL_ON_FUNC( DBE_FAILED_SECOND_SECURITY_REQ, UidType );
	DECL_ON_FUNC( DBE_CREATE_SECOND_SECURITY_REQ );
	DECL_ON_FUNC( DBE_DELETE_SECOND_SECURITY_REQ );
	DECL_ON_FUNC( DBE_CHANGE_SECOND_SECURITY_PW_REQ );
	DECL_ON_FUNC( DBE_UPDATE_SECURITY_TYPE_NOT );
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	DECL_ON_FUNC( DBE_WRITE_COME_BACK_REWARD_NOT );
	DECL_ON_FUNC( DBE_WRITE_COME_BACK_END_NOT );
	DECL_ON_FUNC( DBE_WRITE_COME_BACK_LOGOUT_NOT );
	//}}

	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}

	//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
	DECL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_WRITE_REQ );
	_DECL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_READ_REQ, UidType );
	//}}

	//{{ 2012. 05. 29	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
	DECL_ON_FUNC( DBE_CHAT_OPTION_INFO_WRITE_REQ );
	_DECL_ON_FUNC( DBE_CHAT_OPTION_INFO_READ_REQ, UidType );
	//}}

	//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
	//}}


	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}}

	//{{ 2012. 09. 22	박세훈	빙고 이벤트
	//}}

	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}

	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	_DECL_ON_FUNC( DBE_NEW_ACCOUNT_QUEST_REQ, KDBE_NEW_QUEST_REQ );
	_DECL_ON_FUNC( DBE_ACCOUNT_QUEST_COMPLETE_REQ, KDBE_QUEST_COMPLETE_REQ );
	DECL_ON_FUNC( DBE_ACCOUNT_QUEST_UPDATE_NOT );
	//}}

	//{{ 2012. 12. 19  교환(한정 수량) - 김민성
	DECL_ON_FUNC( DBE_ITEM_EXCHANGE_LIMIT_CHECK_REQ );
	//}}

	//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
	//}


	//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
	//}}



    DECL_ON_FUNC( DBE_GET_PURE_SECOND_SECURITY_INFO_REQ );
};

template < class T >
void KGSAccountDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_ACCOUNT_DB, 0, PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KGSAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_ACCOUNT_DB, 0, PI_GS_USER, nTo, NULL, usEventID, data );
}
