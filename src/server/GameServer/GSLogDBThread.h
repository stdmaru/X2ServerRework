#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below

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

class KGSLogDBThread : public KSubThread
{
    DeclareDBThread( KGSLogDBThread );
	//{{ 2009. 8. 17  최육사	sp profiler
	DeclareSPProfiler;
	//}}
    DeclPfID;

public:
    KGSLogDBThread();
    virtual ~KGSLogDBThread(void);

protected:
    // derived from KSubThread
    virtual void ProcessEvent( const KEventPtr& spEvent ); // Port: `inline` dropped, it is defined in the .cpp

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );

	//{{ 2007. 10. 16  최육사  유저 통계
	void UserStatisticsTableParsing( std::map< std::pair< int, int >, std::vector< int > >& mapStatistics,
		int iTableID, int iSubTableID, int iMaxColumn, int* arrayData );
	//}}

    void WriteServerInfoToDB( IN KDBE_UPDATE_SERVER_INFO_REQ& kPacket_, OUT KServerList& kPacket );

	// event handler
	_DECL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress );
    DECL_ON_FUNC( DBE_UPDATE_SERVER_INFO_REQ );
	DECL_ON_FUNC_NOPARAM( DBE_ADMIN_SERVER_STATUS_REQ );
	DECL_ON_FUNC_NOPARAM( DBE_SERVER_OFF_NOT );
	//{{ 2009. 2. 13  최육사	채널 리스트
	DECL_ON_FUNC( DBE_CHANNEL_LIST_REQ );
	//}}
	DECL_ON_FUNC( DBE_CHARACTER_LEVELUP_NOT );
	DECL_ON_FUNC( ELOG_STAT_PLAY_TIME );
   _DECL_ON_FUNC( ELOG_STAT_PVP_MAP, KStatisticsData );
   _DECL_ON_FUNC( ELOG_STAT_FPS, KStatisticsData );
   _DECL_ON_FUNC( ELOG_STAT_ED, KStatisticsData );
   _DECL_ON_FUNC( ELOG_STAT_QUEST, KStatisticsData );
   //{{ 2011. 02. 28	최육사	사내 통합 통계
   //}}
	DECL_ON_FUNC( ELOG_USER_STATISTICS );
	DECL_ON_FUNC( ELOG_CHEAT_LOG_NOT );
   _DECL_ON_FUNC( ELOG_STAT_DISCONNECT_USERFSM, KStatisticsData );
   _DECL_ON_FUNC( ELOG_STAT_DISCONNECT_REASON, KStatisticsData );
   _DECL_ON_FUNC( DBE_DELETE_UNIT_STAT_NOT, KEGS_DELETE_UNIT_REQ );
    DECL_ON_FUNC( DBE_UPDATE_UNIT_CLASS_NOT );
	DECL_ON_FUNC( ELOG_RESURRECTION_STONE_LOG_NOT );
	DECL_ON_FUNC( ELOG_RESOLVE_JACKPOT_LOG_NOT );
    DECL_ON_FUNC( ELOG_GET_PURCHASED_CASH_ITEM_NOT );
	DECL_ON_FUNC_NOPARAM( DBE_EVENT_UPDATE_REQ );

	DECL_ON_FUNC_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ );

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}
	DECL_ON_FUNC( ELOG_ITEM_ABUSER_LOG_NOT );
	DECL_ON_FUNC( ELOG_ENCHANT_ABUSER_LOG_NOT );
	DECL_ON_FUNC( ELOG_ED_ABUSER_LOG_NOT );
	DECL_ON_FUNC( ELOG_CASH_ITEM_RESOLVE_LOG_NOT );			// 캐쉬 분해 로그
	DECL_ON_FUNC( ELOG_ENCHANT_ITEM_BREAK_LOG_NOT );		// 강화 깨짐 로그
	DECL_ON_FUNC( DBE_ABUSER_LIST_REQ );					// 어뷰저 리스트
	DECL_ON_FUNC( DBE_REG_ABUSER_AUTOMATIC_NOT );
	DECL_ON_FUNC( ELOG_ENCHANT_SUPPORT_MATERIAL_LOG_NOT );	// 보조제 로그
	DECL_ON_FUNC( ELOG_RESTORE_ITEM_LOG_NOT );				// 깨짐 복구 로그
	DECL_ON_FUNC( ELOG_ATTACH_ITEM_ENCHANT_LOG_NOT );		// 부여 로그
	DECL_ON_FUNC( ELOG_CHARM_ITEM_LOG_NOT );				// 매력아이템 로그
	//{{ 2009. 4. 14  최육사	닉네임 변경
	DECL_ON_FUNC( ELOG_DELETE_NICK_NAME_NOT );
	DECL_ON_FUNC( ELOG_CHANGE_NICK_NAME_NOT );
	//}}

	DECL_ON_FUNC( ELOG_INIT_SKILL_TREE_NOT );
	DECL_ON_FUNC( ELOG_INSERT_CASH_SKILL_POINT_NOT );

	//{{ 2009. 7. 22  최육사	메가폰로그
	DECL_ON_FUNC( ELOG_MEGAPHONE_LOG_NOT );
	//}}

	//{{ 2009. 9. 1  최육사		비정상종료사유
	DECL_ON_FUNC( ELOG_ABNORMAL_DISCONNECT );
	//}}
	//{{ 2009. 9. 9  최육사		밀봉
	DECL_ON_FUNC( ELOG_USE_SEAL_ITEM_NOT );
	//}}
	//{{ 2009. 10. 22  최육사	트래픽 유발 유저 해킹유저로 자동등록
	DECL_ON_FUNC( ELOG_TRAFFIC_ATTACKER_LOG_NOT );
	//}}
	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	DECL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_REQ );
	//}}
	//{{ 2010. 06. 30  최육사	근성도 통계
	DECL_ON_FUNC( DBE_SPIRIT_STATISTICS_NOT );
	//}}


	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}

	//{{ 2011. 01. 05	최육사	속성 부적
	DECL_ON_FUNC( ELOG_ATTRIB_ATTACH_ITEM_LOG_NOT );
	//}}
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	DECL_ON_FUNC( ELOG_USER_CHARACTER_LOG_NOT );
	//}}
	//{{ 2011. 02. 09	최육사	캐쉬템 구매 로그
	DECL_ON_FUNC( ELOG_BUY_CASH_ITEM_LOG_NOT );
	//}}
	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	DECL_ON_FUNC( ELOG_CASH_PRODUCT_LIST_UPDATE_NOT );
	//}}
	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	DECL_ON_FUNC( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ );
	//}}
	//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
   _DECL_ON_FUNC( ELOG_STAT_ITEM_DB, KStatisticsData );
	//}}


   //{{ 2011. 05. 02  김민성	2차 보안 시스템
   DECL_ON_FUNC( DBE_FIRST_WRITE_SECURITY_TYPE_NOT );
   //}}
	_DECL_ON_FUNC( ELOG_UPDATE_SHARE_ITEM_NOT, std::vector< KTradeShareItemInfo > );
	DECL_ON_FUNC( ELOG_ITEM_TRADE_ERROR_NOT );

   //{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
   DECL_ON_FUNC( DBE_FIRST_WRITE_COME_BACK_INFO_NOT );
   //}}
   //{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{  2011.11.08     김민성    강화 로그
	DECL_ON_FUNC( ELOG_HIGH_ENCHANT_ITEM_LOG_NOT );
	//}}
	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}
	//{{ 2011. 12. 02  김민성	AP 구매 로그 수치
	DECL_ON_FUNC( DBE_BUY_AP_ITEM_LOG_NOT );
	//}}
	//{{ 2011. 12. 14	최육사	패킷 처리 지연 시간 체크
	DECL_ON_FUNC( DBE_UPDATE_PAKCET_MORNITORING_LOG_NOT );
	//}}
	//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
	//}}
	//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
	//}}
	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}
	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}
	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	_DECL_ON_FUNC( DBE_BAD_ATTITUDE_USER_LOG_NOT, KERM_BAD_ATTITUDE_USER_LOG_NOT );
	//}
	//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
	DECL_ON_FUNC( DBE_TICK_PERFORMANCE_LOG_NOT );
	//}}

	//{{ 2013. 02. 19   교환 로그 추가 - 김민성
	DECL_ON_FUNC( DBE_ITEM_EXCHANGE_LOG_NOT );
	//}
	//{{ 허상형 : [2010/8/31/] //	부활석 통계 분리 작업	// 임규수 일본 추가
	//}} 허상형 : [2010/8/31/] //	부활석 통계 분리 작업
	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}
	_DECL_ON_FUNC( DBE_CATCH_HACKUSER_INTO_NOT, KEGS_CATCH_HACKUSER_INFO_NOT );



	//{{ 2013. 02. 21	최육사	로그 시스템 개선
	DECL_ON_FUNC( DBE_LOG_STATISTICS_INFO_NOT );
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	DECL_ON_FUNC( DBE_WRITE_RELATIONSHIP_INFO_LOG_REQ );
	//}





	DECL_ON_FUNC( ELOG_KOM_FILE_CHECK_LOG_NOT );

    DECL_ON_FUNC( ELOG_EXPAND_BANK_INVENTORY_NOT );

	DECL_ON_FUNC( ELOG_EXPAND_SKILL_PAGE_NOT );

	DECL_ON_FUNC( DBE_CHANGE_PET_ID_LOG_NOT );


	DECL_ON_FUNC( DBE_UNLIMITED_SECOND_CHANGE_JOB_NOT );

};

template < class T >
void KGSLogDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KGSLogDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_GS_USER, nTo, NULL, usEventID, data );
}
