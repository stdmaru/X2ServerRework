#include <winsock2.h>
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "GSAccountDBThread.h"
#include "GameServer.h"
//{{ 2012. 05. 10	최육사	서버 버전
//#ifdef SERV_VERSION_FLAG
#include "SimLayer.h"
//#endif SERV_VERSION_FLAG
//}}
#include "NetError.h"

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
#include "GameSysVal/GameSysVal.h"
//}}

#define GAMEPAD_ID_MODIFIER 20000

//ImplementDBThread( KGSAccountDBThread );
ImplPfID( KGSAccountDBThread, PI_GS_ACCOUNT_DB );

#define CLASS_TYPE KGSAccountDBThread

IMPL_PROFILER_DUMP( KGSAccountDBThread )
{
	for( unsigned int ui = 0; ui < vecDump.size(); ++ui )
	{
		unsigned int iAvg = 0;
		if( vecDump[ui].m_iQueryCount > 0 )	iAvg = vecDump[ui].m_iTotalTime / vecDump[ui].m_iQueryCount;
		DO_QUERY_NO_PROFILE( L"exec dbo.mup_insert_querystats", L"N\'%s\', %d, %d, %d, %d, %d, %d",
			% vecDump[ui].m_wstrQuery
			% vecDump[ui].m_iMinTime
			% iAvg
			% vecDump[ui].m_iMaxTime
			% vecDump[ui].m_iOver1Sec
			% vecDump[ui].m_iQueryCount
			% vecDump[ui].m_iQueryFail
			);
		continue;

end_proc:
		START_LOG( cerr, vecDump[ui].m_wstrQuery )
			<< BUILD_LOG( vecDump[ui].m_iMinTime )
			<< BUILD_LOG( vecDump[ui].m_iMaxTime )
			<< BUILD_LOG( vecDump[ui].m_iTotalTime )
			<< BUILD_LOG( vecDump[ui].m_iQueryCount )
			<< BUILD_LOG( iAvg )
			<< BUILD_LOG( vecDump[ui].m_iOver1Sec )
			<< BUILD_LOG( vecDump[ui].m_iQueryFail )
			<< END_LOG;
	}
}

KGSAccountDBThread::~KGSAccountDBThread(void)
{
}

void KGSAccountDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
		//{{ 2009. 12. 22  최육사	서버군확장
		CASE( ELOG_STAT_PLAY_TIME );
		//}}

        CASE_NOPARAM( DBE_UPDATE_LOGIN_TIME_NOT );
        CASE( EGS_CHANGE_OPTION_PLAY_GUIDE_REQ );

	   _CASE( DBE_ADMIN_CHANGE_AUTH_LEVEL_REQ, KEGS_ADMIN_CHANGE_AUTH_LEVEL_REQ );
	   _CASE( DBE_ADMIN_GET_AUTH_LEVEL_LIST_REQ, KEGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ );

	   _CASE( DBE_ACCOUNT_CREATE_UNIT_REQ, KEGS_CREATE_UNIT_ACK );
	   //{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
	   _CASE( DBE_ACCOUNT_SELECT_UNIT_REQ, KDBE_SELECT_UNIT_ACK );
	   //}}
		//{{ 2009. 6. 4  최육사	캐릭터 선택
	   _CASE( DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_REQ, KDBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_ACK );
		//}}
	   _CASE( DBE_ACCOUNT_DELETE_UNIT_REQ, KEGS_FINAL_DELETE_UNIT_ACK );

		//{{ 2008. 3. 28  최육사  추천인
		CASE_NOPARAM( DBE_RECOMMEND_USER_NOT );
		//}}

	   _CASE( DBE_AGREE_HACK_USER_REQ, UidType );
	   _CASE( DBE_REPORT_HACK_USER_NOT, UidType );

	   _CASE( DBE_GET_MY_MESSENGER_SN_REQ, KEGS_GET_MY_MESSENGER_SN_REQ );

		//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
		CASE( DBE_JOIN_ACCOUNT_CHECK_IP_LOG_NOT );
		//}}

		//{{ 2011. 05. 02  김민성	2차 보안 시스템
		CASE( DBE_GET_SECOND_SECURITY_INFO_REQ );
		CASE( DBE_SEUCCESS_SECOND_SECURITY_REQ );
		_CASE( DBE_FAILED_SECOND_SECURITY_REQ, UidType );
		CASE( DBE_CREATE_SECOND_SECURITY_REQ );
		CASE( DBE_DELETE_SECOND_SECURITY_REQ );
		CASE( DBE_CHANGE_SECOND_SECURITY_PW_REQ );
		CASE( DBE_UPDATE_SECURITY_TYPE_NOT );
		//}}

		//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		CASE( DBE_WRITE_COME_BACK_REWARD_NOT );
		CASE( DBE_WRITE_COME_BACK_END_NOT );
		CASE( DBE_WRITE_COME_BACK_LOGOUT_NOT );
		//}}
		//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
		//}}
		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
		//}}

		//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
		CASE( DBE_KEYBOARD_MAPPING_INFO_WRITE_REQ );
		_CASE( DBE_KEYBOARD_MAPPING_INFO_READ_REQ, UidType );
		//}}

		//{{ 2012. 05. 29	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
		CASE( DBE_CHAT_OPTION_INFO_WRITE_REQ );
		_CASE( DBE_CHAT_OPTION_INFO_READ_REQ, UidType );
		//}}
		//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
		//}}


		//{{ 2012. 09. 22	박세훈	빙고 이벤트
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
		_CASE( DBE_NEW_ACCOUNT_QUEST_REQ, KDBE_NEW_QUEST_REQ );
		_CASE( DBE_ACCOUNT_QUEST_COMPLETE_REQ, KDBE_QUEST_COMPLETE_REQ );
		CASE( DBE_ACCOUNT_QUEST_UPDATE_NOT );
		//}}
		//{{ 2012. 12. 19  교환(한정 수량) - 김민성
		CASE( DBE_ITEM_EXCHANGE_LIMIT_CHECK_REQ );
		//}}
		//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
		//}


		//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
		//}}
		//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
		//}}



        CASE( DBE_GET_PURE_SECOND_SECURITY_INFO_REQ );
    default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KGSAccountDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KGSAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

int KGSAccountDBThread::GetMessengerSN( IN UidType iUnitUID, OUT u_int& uiKNMSerialNum )
{
	int iOK = NetError::ERR_ODBC_01;

	const int iServerGroupID = KBaseServer::GetKObj()->GetServerGroupID();

	// 넥슨 메신저용 CharacterSN.
	DO_QUERY( L"exec dbo.mup_get_MessengerSN", L"%d, %d", % iServerGroupID % iUnitUID );
	if( m_kODBC.BeginFetch() )
	{
		int iTemp = 0;
		FETCH_DATA( iTemp );

		uiKNMSerialNum = static_cast<u_int>(iTemp);

		m_kODBC.EndFetch();
	}
	else
	{
		iOK = NetError::ERR_SELECT_UNIT_04;
		goto end_proc;
	}

	iOK = NetError::NET_OK;

end_proc:
	return iOK;
}

//{{ 2009. 12. 22  최육사	서버군확장
IMPL_ON_FUNC( ELOG_STAT_PLAY_TIME )
{
	int iOK = NetError::ERR_ODBC_01;

	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	//{{ 2012. 11. 14	박세훈	Field PT 로그 추가

	bool bPCBangFlag = kPacket_.m_bIsPcbang;	//m_bIsPcbang 해외도 필요해서 오과금 define 밖으로 뺌
	int iChannelCode = 0;

	iChannelCode = static_cast< int >(kPacket_.m_ucChannelCode);

	DO_QUERY( L"exec dbo.mup_insert_user_play_time_all", L"%d, N\'%s\', N\'%s\', N\'%s\', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, N\'%s\', %d, %d",
		% kPacket_.m_iUserUID
		% kPacket_.m_wstrIP
		% kPacket_.m_wstrLoginTime
		% kPacket_.m_wstrLogoutTime
		% kPacket_.m_bLoginFail
		% kPacket_.m_bLogoutFail
		% kPacket_.m_iTotalPlayTime
		% kPacket_.m_iPvpPlayTime
		% kPacket_.m_iDungeonPlayTime
		% kPacket_.m_iFieldPlayTime
		% kPacket_.m_iDisconnectReason
		% kPacket_.m_iDisconnectUserFSM
		% 0 //kPacket_.m_iArcadePlayTime
		% kPacket_.m_iServerGroupID
		% kPacket_.m_wstrMachineID
		% iChannelCode
		% bPCBangFlag
		);
	//}}
	//}}

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< END_LOG;
}
//}}

IMPL_ON_FUNC_NOPARAM( DBE_UPDATE_LOGIN_TIME_NOT )
{
    int iOK = NetError::ERR_ODBC_01;
    DO_QUERY( L"exec dbo.mup_update_login_time", L"%d", % FIRST_SENDER_UID );
    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );

        m_kODBC.EndFetch();
    }

end_proc:
    if( iOK != NetError::NET_OK )
    {
        START_LOG( cerr, L"최근 로그인 시각 업데이트 실패." )
            << BUILD_LOG( iOK )
            << BUILD_LOG( FIRST_SENDER_UID )
            << END_LOG;
    }

    return;
}

IMPL_ON_FUNC( EGS_CHANGE_OPTION_PLAY_GUIDE_REQ )
{
    KPacketOK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;
    DO_QUERY( L"exec dbo.mup_set_play_guide", L"%d, %d", % LAST_SENDER_UID % kPacket_.m_bOn );

    int iOK;
    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();

		switch( iOK )
		{
		case 0:
			kPacket.m_iOK = NetError::NET_OK;
			break;

		case -1:
			kPacket.m_iOK = NetError::ERR_CLIENT_STATE;
			break;

		case -11:
			kPacket.m_iOK = NetError::ERR_NEW_ACCOUNT_02;
			break;
		}
    }

end_proc:
    SendToUser( LAST_SENDER_UID, EGS_CHANGE_OPTION_PLAY_GUIDE_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_ADMIN_CHANGE_AUTH_LEVEL_REQ, KEGS_ADMIN_CHANGE_AUTH_LEVEL_REQ )
{
	KEGS_ADMIN_CHANGE_AUTH_LEVEL_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.mup_set_user_authority", L"N\'%s\', %d", % kPacket_.m_wstrUserID % (int)kPacket_.m_cAuthLevel );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK )
		{
		case -1:
			kPacket.m_iOK = NetError::ERR_ADMIN_COMMAND_03;
			break;

		case -2:
			kPacket.m_iOK = NetError::ERR_ADMIN_COMMAND_04;
			break;

		case -11:
		case -12:
			kPacket.m_iOK = NetError::ERR_ADMIN_COMMAND_05;
			break;
		}
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_ADMIN_CHANGE_AUTH_LEVEL_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_ADMIN_GET_AUTH_LEVEL_LIST_REQ, KEGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ )
{
	KEGS_ADMIN_GET_AUTH_LEVEL_LIST_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	DO_QUERY( L"exec dbo.mup_get_authority_list", L"%d", % (int)kPacket_.m_cAuthLevel );

	while( m_kODBC.Fetch() )
	{
		std::wstring wstrUserID;
		std::wstring wstrUserName;
		FETCH_DATA( wstrUserID >> wstrUserName );

		wstrUserID += L" : ";
		wstrUserID += wstrUserName;

		kPacket.m_vecUserIDList.push_back( wstrUserID );
	}

	if( kPacket.m_vecUserIDList.empty() == true )
		kPacket.m_iOK = NetError::ERR_ADMIN_COMMAND_06;

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_ADMIN_GET_AUTH_LEVEL_LIST_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_ACCOUNT_CREATE_UNIT_REQ, KEGS_CREATE_UNIT_ACK )
{
	//{{ 2012. 05. 10	최육사	서버 버전
	//#ifdef SERV_VERSION_FLAG
	if( KSimLayer::GetKObj()->GetVersionFlag() == KSimLayer::VF_INTERNAL )
	{
		START_LOG( cout, L"사내 테스트 버전 게임서버에서 새로운 캐릭터가 생성되었습니다!" )
			<< BUILD_LOG( kPacket_.m_kUnitInfo.m_wstrNickName );

		kPacket_.m_iOK = NetError::NET_OK;
	}
	else
	{
		DO_QUERY( L"exec dbo.mup_insert_MessengerSN", L"%d, %d, %d",
			% LAST_SENDER_UID
			% GetKGameServer()->GetServerGroupID()
			% kPacket_.m_kUnitInfo.m_nUnitUID
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket_.m_iOK );
			m_kODBC.EndFetch();
		}

		if( kPacket_.m_iOK != NetError::NET_OK )
		{
			kPacket_.m_iOK = NetError::ERR_CREATE_UNIT_07;
			goto end_proc;
		}
	}
	//#endif SERV_VERSION_FLAG
	//}}

end_proc:
	LOG_SUCCESS( kPacket_.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( NetError::GetErrStr(kPacket_.m_iOK) )
		<< BUILD_LOG( LAST_SENDER_UID )
		<< BUILD_LOG( kPacket_.m_kUnitInfo.m_nUnitUID )
		<< END_LOG;

	SendToUser( LAST_SENDER_UID, DBE_ACCOUNT_CREATE_UNIT_ACK, kPacket_ );
}

//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
_IMPL_ON_FUNC( DBE_ACCOUNT_SELECT_UNIT_REQ, KDBE_SELECT_UNIT_ACK )
//}}
{
	//{{ 2012. 05. 10	최육사	서버 버전
	//#ifdef SERV_VERSION_FLAG
	if( KSimLayer::GetKObj()->GetVersionFlag() == KSimLayer::VF_INTERNAL )
	{
		START_LOG( cout, L"사내 테스트 버전 게임서버에서 캐릭터가 선택되었습니다!" )
			<< BUILD_LOG( kPacket_.m_kUnitInfo.m_wstrNickName );

		kPacket_.m_iOK = NetError::NET_OK;
		kPacket_.m_kUnitInfo.m_uiKNMSerialNum = static_cast<u_int>(rand()); // dummy값을 넣자
	}
	else
	{
		kPacket_.m_iOK = GetMessengerSN( kPacket_.m_kUnitInfo.m_nUnitUID, kPacket_.m_kUnitInfo.m_uiKNMSerialNum );

		if( kPacket_.m_kUnitInfo.m_uiKNMSerialNum == 0 )
		{
			START_LOG( cerr, L"넥슨 메신저SN값이 이상합니다. 새로운 메신저SN값을 발급합니다." )
				<< BUILD_LOG( kPacket_.m_kUnitInfo.m_nUnitUID )
				<< BUILD_LOG( kPacket_.m_kUnitInfo.m_uiKNMSerialNum )
				<< END_LOG;

			//////////////////////////////////////////////////////////////////////////
			// 넥슨 메신저SN 새로 발급
			int iOK = NetError::ERR_ODBC_01;
			DO_QUERY( L"exec dbo.mup_insert_MessengerSN", L"%d, %d, %d",
				% LAST_SENDER_UID
				% GetKGameServer()->GetServerGroupID()
				% kPacket_.m_kUnitInfo.m_nUnitUID
				);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}

			if( iOK != NetError::NET_OK )
			{
				START_LOG( cerr, L"넥슨메신저SN 재발급 실패!?" )
					<< BUILD_LOG( LAST_SENDER_UID )
					<< BUILD_LOG( GetKGameServer()->GetServerGroupID() )
					<< BUILD_LOG( kPacket_.m_kUnitInfo.m_nUnitUID )
					<< END_LOG;

				// 게임은 진행할수 있도록 ok값 보내자
				kPacket_.m_iOK = NetError::NET_OK;
			}
			else
			{
				// 발급되었으면 SN을 받자!
				kPacket_.m_iOK = GetMessengerSN( kPacket_.m_kUnitInfo.m_nUnitUID, kPacket_.m_kUnitInfo.m_uiKNMSerialNum );
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}


	//{{ 2012. 10. 10	박세훈	빙고 이벤트
	//}}


	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	// 계정 퀘스트 정보를 읽어 와야 한다.
	{
		//////////////////////////////////////////////////////////////////////////
		// 완료한 계정 퀘스트를 받아 온다.
		CTime tCompleteDate;
		std::wstring wstrCompleteDate;
		DO_QUERY( L"exec dbo.P_MQuests_Complete_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KCompleteQuestInfo kInfo;
			kInfo.m_iCompleteCount = 1;

			FETCH_DATA( kInfo.m_iQuestID
				>> wstrCompleteDate );

			// 완료 날짜 변환
			LIF( KncUtil::ConvertStringToCTime( wstrCompleteDate, tCompleteDate ) );
			kInfo.m_tCompleteDate = tCompleteDate.GetTime();

			kPacket_.m_vecAccountCompleteQuest.push_back( kInfo );
		}
		//{{ 2013. 03. 21	 계정 퀘스트 - 요일별 활성화 기능 - 김민성
		DO_QUERY( L"exec dbo.P_MQuests_Repeat_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KCompleteQuestInfo kInfo;
			kInfo.m_iCompleteCount = 1;

			FETCH_DATA( kInfo.m_iQuestID
				>> wstrCompleteDate );

			// 완료 날짜 변환
			LIF( KncUtil::ConvertStringToCTime( wstrCompleteDate, tCompleteDate ) );
			kInfo.m_tCompleteDate = tCompleteDate.GetTime();

			kPacket_.m_vecAccountCompleteQuest.push_back( kInfo );
		}
		//}

		//////////////////////////////////////////////////////////////////////////
		// 진행 중인(수락한) 계정 퀘스트를 받아 온다.
		DO_QUERY( L"exec dbo.P_MQuests_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KQuestInstance		kQuest;
			KSubQuestInstance	kSub[5];
			kQuest.m_OwnorUnitUID = kPacket_.m_kUnitInfo.m_nUnitUID;

			FETCH_DATA( kQuest.m_iID
				>> kSub[0].m_ucClearData
				>> kSub[1].m_ucClearData
				>> kSub[2].m_ucClearData
				>> kSub[3].m_ucClearData
				>> kSub[4].m_ucClearData
				);
			for( int i = 0; i < 5; i++ )
				kQuest.m_vecSubQuestInstance.push_back( kSub[i] );

			kPacket_.m_vecAccountQuesting.push_back( kQuest );
		}
	}
	//}}

	LOG_SUCCESS( kPacket_.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
		<< BUILD_LOG( LAST_SENDER_UID )
		<< BUILD_LOG( kPacket_.m_kUnitInfo.m_nUnitUID )
		<< END_LOG;
	//#endif SERV_VERSION_FLAG
	//}}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_ACCOUNT_SELECT_UNIT_ACK, kPacket_ );
}

//{{ 2009. 6. 4  최육사	캐릭터 선택
_IMPL_ON_FUNC( DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_REQ, KDBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_ACK )
{
	//{{ 2012. 05. 10	최육사	서버 버전
	//#ifdef SERV_VERSION_FLAG
	if( KSimLayer::GetKObj()->GetVersionFlag() == KSimLayer::VF_INTERNAL )
	{
		START_LOG( cout, L"사내 테스트 버전 게임서버에서 채널 이동으로 인해 캐릭터가 자동으로 재선택되었습니다!" )
			<< BUILD_LOG( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_wstrNickName );

		kPacket_.m_kSelectUnitAck.m_iOK = NetError::NET_OK;

		kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_uiKNMSerialNum = static_cast<u_int>(rand()); // dummy값을 넣자

	}
	else
	{
		kPacket_.m_kSelectUnitAck.m_iOK = GetMessengerSN( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID, kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_uiKNMSerialNum );

		if( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_uiKNMSerialNum == 0 )
		{
			START_LOG( cerr, L"넥슨 메신저SN값이 이상합니다. 새로운 메신저SN값을 발급합니다." )
				<< BUILD_LOG( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID )
				<< BUILD_LOG( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_uiKNMSerialNum )
				<< END_LOG;

			//////////////////////////////////////////////////////////////////////////
			// 넥슨 메신저SN 새로 발급
			int iOK = NetError::ERR_ODBC_01;

			DO_QUERY( L"exec dbo.mup_insert_MessengerSN", L"%d, %d, %d",
				% LAST_SENDER_UID
				% GetKGameServer()->GetServerGroupID()
				% kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID
				);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}

			if( iOK != NetError::NET_OK )
			{
				START_LOG( cerr, L"넥슨메신저SN 재발급 실패!?" )
					<< BUILD_LOG( LAST_SENDER_UID )
					<< BUILD_LOG( GetKGameServer()->GetServerGroupID() )
					<< BUILD_LOG( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID )
					<< END_LOG;

				// 게임은 진행할수 있도록 ok값 보내자
				kPacket_.m_kSelectUnitAck.m_iOK = NetError::NET_OK;
			}
			else
			{
				// 발급되었으면 SN을 받자!
				kPacket_.m_kSelectUnitAck.m_iOK = GetMessengerSN( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID, kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_uiKNMSerialNum );
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}
//#endif SERV_VERSION_FLAG
//}}


	//{{ 2012. 10. 10	박세훈	빙고 이벤트
	//}}

	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	// 계정 퀘스트 정보를 읽어 와야 한다.
	{
		//////////////////////////////////////////////////////////////////////////
		// 완료한 계정 퀘스트를 받아 온다.
		CTime tCompleteDate;
		std::wstring wstrCompleteDate;
		DO_QUERY( L"exec dbo.P_MQuests_Complete_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KCompleteQuestInfo kInfo;
			kInfo.m_iCompleteCount = 1;

			FETCH_DATA( kInfo.m_iQuestID
				>> wstrCompleteDate );

			// 완료 날짜 변환
			LIF( KncUtil::ConvertStringToCTime( wstrCompleteDate, tCompleteDate ) );
			kInfo.m_tCompleteDate = tCompleteDate.GetTime();

			kPacket_.m_kSelectUnitAck.m_vecAccountCompleteQuest.push_back( kInfo );
		}

		//{{ 2013. 03. 21	 계정 퀘스트 - 요일별 활성화 기능 - 김민성
		DO_QUERY( L"exec dbo.P_MQuests_Repeat_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KCompleteQuestInfo kInfo;
			kInfo.m_iCompleteCount = 1;

			FETCH_DATA( kInfo.m_iQuestID
				>> wstrCompleteDate );

			// 완료 날짜 변환
			LIF( KncUtil::ConvertStringToCTime( wstrCompleteDate, tCompleteDate ) );
			kInfo.m_tCompleteDate = tCompleteDate.GetTime();

			kPacket_.m_kSelectUnitAck.m_vecAccountCompleteQuest.push_back( kInfo );
		}
		//}

		//////////////////////////////////////////////////////////////////////////
		// 진행 중인(수락한) 계정 퀘스트를 받아 온다.
		DO_QUERY( L"exec dbo.P_MQuests_GET", L"%d", % LAST_SENDER_UID );
		while( m_kODBC.Fetch() )
		{
			KQuestInstance		kQuest;
			KSubQuestInstance	kSub[5];
			kQuest.m_OwnorUnitUID = kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID;

			FETCH_DATA( kQuest.m_iID
				>> kSub[0].m_ucClearData
				>> kSub[1].m_ucClearData
				>> kSub[2].m_ucClearData
				>> kSub[3].m_ucClearData
				>> kSub[4].m_ucClearData
				);
			for( int i = 0; i < 5; i++ )
				kQuest.m_vecSubQuestInstance.push_back( kSub[i] );

			kPacket_.m_kSelectUnitAck.m_vecAccountQuesting.push_back( kQuest );
		}
	}
	//}}

	LOG_SUCCESS( kPacket_.m_kSelectUnitAck.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_kSelectUnitAck.m_iOK ) )
		<< BUILD_LOG( LAST_SENDER_UID )
		<< BUILD_LOG( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_nUnitUID )
		<< END_LOG;

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_ACK, kPacket_ );
}
//}}

_IMPL_ON_FUNC( DBE_ACCOUNT_DELETE_UNIT_REQ, KEGS_FINAL_DELETE_UNIT_ACK )
{
	//{{ 2012. 05. 10	최육사	서버 버전
	//#ifdef SERV_VERSION_FLAG
	if( KSimLayer::GetKObj()->GetVersionFlag() == KSimLayer::VF_INTERNAL )
	{
		START_LOG( cout, L"사내 테스트 버전 게임서버에서 캐릭터가 삭제 되었습니다!" )
			<< BUILD_LOG( kPacket_.m_wstrNickName );

		kPacket_.m_iOK = NetError::NET_OK;
	}
	else
	{
		kPacket_.m_iOK = GetMessengerSN( kPacket_.m_iUnitUID, kPacket_.m_uiKNMSerialNum );
	}
	//#endif SERV_VERSION_FLAG
	//}}

	LOG_SUCCESS( kPacket_.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( NetError::GetErrStr(kPacket_.m_iOK) )
		<< BUILD_LOG( LAST_SENDER_UID )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< END_LOG;

	SendToUser( LAST_SENDER_UID, DBE_ACCOUNT_DELETE_UNIT_ACK, kPacket_ );
}

//{{ 2008. 3. 28  최육사  추천인
IMPL_ON_FUNC_NOPARAM( DBE_RECOMMEND_USER_NOT )
{
	DO_QUERY( L"exec dbo.mup_update_isrecommend", L"%d", % LAST_SENDER_UID );
	int iOK = NetError::ERR_ODBC_01;

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"MUser 테이블의 추천인 flag 갱신 실패!" )
			<< BUILD_LOG( LAST_SENDER_UID )
			<< END_LOG;
	}
}
//}}

_IMPL_ON_FUNC( DBE_AGREE_HACK_USER_REQ, UidType )
{
	KEGS_AGREE_HACK_USER_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_update_hackerlist", L"%d, %d", % kPacket_ % 1 );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		kPacket.m_iOK = NetError::ERR_HACK_USER_01;

		START_LOG( cerr, L"해킹 동의 데이터 업데이트 실패.!" )
			<< BUILD_LOG( kPacket_ )
			<< END_LOG;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_AGREE_HACK_USER_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_REPORT_HACK_USER_NOT, UidType )
{
	int iOK = NetError::ERR_ODBC_01;

	// 이메일 감시 대상자 등록
	DO_QUERY( L"exec dbo.mup_insert_hackerlist", L"%d", % kPacket_ );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"해킹유저 신고등록 실패.!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_ )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( DBE_GET_MY_MESSENGER_SN_REQ, KEGS_GET_MY_MESSENGER_SN_REQ )
{
	KEGS_GET_MY_MESSENGER_SN_ACK kPacket;

	//{{ 2012. 05. 10	최육사	서버 버전
	//#ifdef SERV_VERSION_FLAG
	if( KSimLayer::GetKObj()->GetVersionFlag() == KSimLayer::VF_INTERNAL )
	{
		START_LOG( cout, L"사내 테스트 버전 게임서버에서 메신저SN을 요청하였습니다!" )
			<< BUILD_LOG( kPacket_ );

		kPacket.m_iOK = NetError::NET_OK;
	}
	else
	{
		kPacket.m_iOK = GetMessengerSN( kPacket_, kPacket.m_uiKNMSerialNum );
	}
	//#endif SERV_VERSION_FLAG
	//}}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"넥슨 메신져 SN 요청 실패.!" )
			<< BUILD_LOG( kPacket_ )
			<< BUILD_LOG( NetError::GetErrStr(kPacket.m_iOK) )
			<< END_LOG;
	}

	SendToUser( LAST_SENDER_UID, DBE_GET_MY_MESSENGER_SN_ACK, kPacket );
}

//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
IMPL_ON_FUNC( DBE_JOIN_ACCOUNT_CHECK_IP_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_insert_hacking_user", L"%d, N\'%s\'", % kPacket_.m_iUserUID % kPacket_.m_wstrLoginIP );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"특정 IP로 계정 생성되는 정보 로그 남기기 실패!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_wstrLoginIP )
			<< END_LOG;
	}
}
//}}

//{{ 2011. 05. 02  김민성	2차 보안 시스템

IMPL_ON_FUNC( DBE_GET_PURE_SECOND_SECURITY_INFO_REQ )
{
    KDBE_GET_SECOND_SECURITY_INFO_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;
    kPacket.m_iFailedCount = 0;
    kPacket.m_bUseSecondPW = false;

    //{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
    CTime tCurr = CTime::GetCurrentTime();
    CTime tEndDate;
    //}}

    // 2차 비번 얻기 : kPacket_ --> UserUID
	DO_QUERY( L"exec dbo.mup_get_second_pw_Info", L"%d", % kPacket_.m_iUserUID );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_bUseSecondPW
            >> kPacket.m_wstrSecondPW
            >> kPacket.m_iFailedCount
            >> kPacket.m_wstrLastAuthDate );

        m_kODBC.EndFetch();
    }

    if( kPacket.m_bUseSecondPW == true && kPacket.m_iFailedCount >= 10 )	// 2차 비밀번호 입력 실패 10회 이상
    {
        kPacket.m_iOK = NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT;

        START_LOG( clog, L"2차 비밀번호 입력 실패 10회 이상" )
            << BUILD_LOG( kPacket.m_bUseSecondPW )
            << BUILD_LOG( kPacket.m_iFailedCount );
        goto end_proc;
    }

    kPacket.m_iOK = NetError::NET_OK;

end_proc:
    SendToUser( LAST_SENDER_UID, DBE_GET_PURE_SECOND_SECURITY_INFO_ACK, kPacket );

}

IMPL_ON_FUNC( DBE_GET_SECOND_SECURITY_INFO_REQ )
{
	KDBE_GET_SECOND_SECURITY_INFO_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iFailedCount = 0;
	kPacket.m_bUseSecondPW = false;

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	CTime tCurr = CTime::GetCurrentTime();
	CTime tEndDate;
	//}}

	// 2차 비번 얻기 : kPacket_ --> UserUID
	DO_QUERY( L"exec dbo.mup_get_second_pw_Info", L"%d", % kPacket_.m_iUserUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_bUseSecondPW
			     >> kPacket.m_wstrSecondPW
				 >> kPacket.m_iFailedCount
				 >> kPacket.m_wstrLastAuthDate );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_bUseSecondPW == true && kPacket.m_iFailedCount >= 10 )	// 2차 비밀번호 입력 실패 10회 이상
	{
		kPacket.m_iOK = NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT;

		START_LOG( clog, L"2차 비밀번호 입력 실패 10회 이상" )
			<< BUILD_LOG( kPacket.m_bUseSecondPW )
			<< BUILD_LOG( kPacket.m_iFailedCount );
		goto end_proc;
	}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	kPacket.m_iRewardStep = 0;
	kPacket.m_bIsFirst = false;
	kPacket.m_bIsComeBackUser = false;

	// DB에 복귀 보상 정보가 있는지 확인한다.
	DO_QUERY( L"exec dbo.mup_get_Comebackuser_info", L"%d", % kPacket_.m_iUserUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_bIsComeBackUser
			>> kPacket.m_wstrComeBackBuffEndDate );

		m_kODBC.EndFetch();
	}

	START_LOG( clog, L"유저의 휴면 복귀 정보를 DB에서 얻어오기 성공!!" )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
		<< BUILD_LOG( kPacket.m_bIsComeBackUser )
		<< BUILD_LOG( kPacket.m_iRewardStep  )
		<< END_LOG;

	// DB에 휴면 유저 정보가 있다.
	if( kPacket.m_bIsComeBackUser == true )
	{
		if( KncUtil::ConvertStringToCTime( kPacket.m_wstrComeBackBuffEndDate , tEndDate ) == false )
		{
			START_LOG( cerr, L"날짜 변환 실패!!!" )
				<< BUILD_LOG( kPacket_.m_iUserUID )
				<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
				<< END_LOG;

			// 이전 시간으로 최화 시킴
			tEndDate = CTime( 2000, 1, 1, 0, 0, 0 );
		}

		// 복귀 버프 끝나는 시간이 현재보다 이전(버프 없음)
		if( tEndDate < tCurr )
		{
			kPacket.m_bIsComeBackUser = false;	// 버프 없도록 한다.

			// LastConnection 정보를 이용여 복귀 유저인지 판단
			DO_QUERY( L"exec dbo.mup_get_comeback_logout", L"%d", % kPacket_.m_iUserUID );
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( kPacket.m_wstrLastConnectDate );

				m_kODBC.EndFetch();
			}

			// 시간 정보가 비어 있지 않다면
			if( kPacket.m_wstrLastConnectDate.empty() == false )
			{
				// 휴면 유저인지 판단하여 맞다면 보상 등급 확인
				if( CheckIsComeBackUser( kPacket.m_wstrLastConnectDate, kPacket.m_iRewardStep, kPacket.m_wstrComeBackBuffEndDate, kPacket_.m_mapComeBackRewardCondition ) == true )
				{
					kPacket.m_bIsFirst = true;
					kPacket.m_bIsComeBackUser = true;	// 복귀 유저가 맞다!

					START_LOG( clog, L"마지막 접속 시간으로 휴면 유저 판단 및 보상 등급 판단" )
						<< BUILD_LOG( kPacket_.m_iUserUID )
						<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
						<< BUILD_LOG( kPacket.m_bIsComeBackUser )
						<< BUILD_LOG( kPacket.m_bIsFirst )
						<< BUILD_LOG( kPacket.m_iRewardStep )
						<< END_LOG;
				}
				else
				{
					// 복귀 유저가 아니다 데이타 초기화
					kPacket.m_iRewardStep = 0;
					kPacket.m_bIsFirst = false;
					kPacket.m_bIsComeBackUser = false;
					kPacket.m_wstrComeBackBuffEndDate = L"";

					// DB에 기록하자 - 버프가 꺼졌다고
					int _iOK = NetError::ERR_ODBC_00;
					DO_QUERY( L"exec dbo.mup_update_Comebackuser_finish", L"%d", % kPacket_.m_iUserUID );
					if( m_kODBC.BeginFetch() )
					{
						FETCH_DATA( _iOK );

						m_kODBC.EndFetch();
					}

					if( _iOK != NetError::NET_OK )
					{
						START_LOG( cerr, L"잘못된 복귀 유저 정보 입력실패!!" )
							<< BUILD_LOG( kPacket_.m_iUserUID )
							<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
							<< BUILD_LOG( kPacket.m_bIsComeBackUser )
							<< BUILD_LOG( _iOK )
							<< END_LOG;
					}

					START_LOG( clog, L"마지막 접속 시간을 보고 휴면 복귀 유저거 아니라고 판단" )
						<< BUILD_LOG( kPacket_.m_iUserUID )
						<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
						<< BUILD_LOG( kPacket.m_bIsComeBackUser )
						<< BUILD_LOG( kPacket.m_bIsFirst )
						<< BUILD_LOG( kPacket.m_iRewardStep )
						<< END_LOG;
				}
			}
		}
		// 복귀 버프 끝나는 시간이 현재보다 이후(버프 있음)
		else
		{
			// 복귀 유저가 아니다
			kPacket.m_bIsFirst = false;
			kPacket.m_bIsComeBackUser = true;

			START_LOG( clog, L"이미 휴면 복귀 유저!!" )
				<< BUILD_LOG( kPacket_.m_iUserUID )
				<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
				<< BUILD_LOG( kPacket.m_bIsComeBackUser )
				<< BUILD_LOG( kPacket.m_bIsFirst )
				<< BUILD_LOG( kPacket.m_iRewardStep )
				<< END_LOG;
			//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
			//}}
		}
	}
	// DB에 휴면 유저 정보가 없다.
	else
	{
		// LastConnection 정보를 이용하여 복귀 유저인지 판단
		DO_QUERY( L"exec dbo.mup_get_comeback_logout", L"%d", % kPacket_.m_iUserUID );
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_wstrLastConnectDate );

			m_kODBC.EndFetch();
		}

		// 시간 정보가 비어 있지 않다면
		if( kPacket.m_wstrLastConnectDate.empty() == false )
		{
			// 휴면 유저인지 판단하여 맞다면 보상 등급 확인
			if( CheckIsComeBackUser( kPacket.m_wstrLastConnectDate, kPacket.m_iRewardStep, kPacket.m_wstrComeBackBuffEndDate, kPacket_.m_mapComeBackRewardCondition ) == true )
			{
				// 복귀 유저이다.
				kPacket.m_bIsFirst = true;
				kPacket.m_bIsComeBackUser = true;

				START_LOG( clog, L"마지막 접속 시간으로 휴면 유저 판단 및 보상 등급 판단" )
					<< BUILD_LOG( kPacket_.m_iUserUID )
					<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
					<< BUILD_LOG( kPacket.m_bIsComeBackUser )
					<< BUILD_LOG( kPacket.m_bIsFirst )
					<< BUILD_LOG( kPacket.m_iRewardStep )
					<< END_LOG;
			}
			else
			{
				// 복귀 유저가 아니다 데이타 초기화
				kPacket.m_iRewardStep = 0;
				kPacket.m_bIsFirst = false;
				kPacket.m_bIsComeBackUser = false;
				kPacket.m_wstrComeBackBuffEndDate = L"";

				START_LOG( clog, L"마지막 접속 시간이 없다. 새 유저가 접속하였다." )
					<< BUILD_LOG( kPacket_.m_iUserUID )
					<< BUILD_LOG( kPacket.m_wstrComeBackBuffEndDate )
					<< BUILD_LOG( kPacket.m_bIsComeBackUser )
					<< BUILD_LOG( kPacket.m_bIsFirst )
					<< BUILD_LOG( kPacket.m_iRewardStep )
					<< END_LOG;
			}
		}
	}

	kPacket.m_iRewardStep = 0;
	kPacket.m_bIsFirst = false;
	kPacket.m_bIsComeBackUser = false;
	kPacket.m_wstrComeBackBuffEndDate = L"";

	//}}

	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}

		//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
		//}}

	//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
	//}}

	//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
		//}

	kPacket.m_iOK = NetError::NET_OK;

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_GET_SECOND_SECURITY_INFO_ACK, kPacket );

}

IMPL_ON_FUNC( DBE_SEUCCESS_SECOND_SECURITY_REQ )
{

	KDBE_SEUCCESS_SECOND_SECURITY_ACK kAck;
	kAck.m_iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_update_second_pw_authsuccess", L"%d, N\'%s\'", % kPacket_.m_iUserID % kPacket_.m_wstrCurrTime );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kAck.m_iOK
				 >> kAck.m_wstrCurrTime );
		m_kODBC.EndFetch();
	}

end_proc:
	if( kAck.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"2차 보안 인증 성공 기록 실패!!" )
			<< BUILD_LOG( kAck.m_iOK )
			<< BUILD_LOG( kAck.m_wstrCurrTime )
			<< END_LOG;
	}

	SendToUser( LAST_SENDER_UID, DBE_SEUCCESS_SECOND_SECURITY_ACK, kAck );
}

_IMPL_ON_FUNC( DBE_FAILED_SECOND_SECURITY_REQ, UidType )
{
	int iOK = NetError::ERR_ODBC_01;
	int iFailedCount = 0;
	DO_QUERY( L"exec dbo.mup_update_second_pw_authfailed", L"%d", % kPacket_ );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iFailedCount );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"2차 보안 인증 실패 기록 실패!!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_ )
			<< BUILD_LOG( iFailedCount )
			<< END_LOG;
	}
	else
	{
		if( iFailedCount >= 10 )
		{
			START_LOG( clog, L"실패 카운트가 10이상인 경우 10으로 보정!" )
				<< BUILD_LOG( kPacket_ )
				<< BUILD_LOG( iFailedCount )
				<< END_LOG;

			iFailedCount = 10;
		}
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_FAILED_SECOND_SECURITY_ACK, iFailedCount );
}

IMPL_ON_FUNC( DBE_CREATE_SECOND_SECURITY_REQ )
{
	KDBE_CREATE_SECOND_SECURITY_ACK kPacket;
	DO_QUERY( L"exec dbo.mup_create_second_pw", L"%d, N\'%s\'", % kPacket_.m_iUserID % kPacket_.m_wstrSecondPW );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}
	kPacket.m_wstrSecondPW = kPacket_.m_wstrSecondPW;

end_proc:
	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"2차 보안 생성 실패!!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUserID )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;

	}
	SendToUser( LAST_SENDER_UID, DBE_CREATE_SECOND_SECURITY_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_DELETE_SECOND_SECURITY_REQ )
{
	KDBE_DELETE_SECOND_SECURITY_ACK kPacket;
	DO_QUERY( L"exec dbo.mup_delete_second_pw", L"%d, N\'%s\'", % kPacket_.m_iUserID % kPacket_.m_wstrSecondPW );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"2차 보안 삭제 실패!!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUserID )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;

	}
	SendToUser( LAST_SENDER_UID, DBE_DELETE_SECOND_SECURITY_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_CHANGE_SECOND_SECURITY_PW_REQ )
{
	KDBE_CHANGE_SECOND_SECURITY_PW_ACK kPacket;
	DO_QUERY( L"exec dbo.mup_update_second_pw", L"%d, N\'%s\', N\'%s\'", % kPacket_.m_iUserID % kPacket_.m_wstrOldSecondPW % kPacket_.m_wstrNewSecondPW );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	kPacket.m_wstrSecondPW = kPacket_.m_wstrNewSecondPW;

end_proc:
	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"2차 보안 비밀번호 변경 실패!!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUserID )
			<< BUILD_LOG( kPacket_.m_wstrOldSecondPW  )
			<< BUILD_LOG( kPacket_.m_wstrNewSecondPW  )
			<< END_LOG;

	}
	SendToUser( LAST_SENDER_UID, DBE_CHANGE_SECOND_SECURITY_PW_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_UPDATE_SECURITY_TYPE_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_update_securitytype", L"%d, %d", % kPacket_.m_iUserUID % kPacket_.m_iSecurityType );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK ); // DB쿼리 규칙상 쿼리가 성공하면 무조건 0값이 반환
		m_kODBC.EndFetch();
	}
end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"유저 보안 사용 여부 DB 기록 실패!!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iSecurityType )
			<< END_LOG;
	}
}
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
bool KGSAccountDBThread::CheckIsComeBackUser( IN std::wstring& wstrLastDate, OUT int& iRewardStep, OUT std::wstring& wstrEndBuffDate, IN std::map< int, int >& mapComeBackRewardCondition )
{
	CTime tCurr = CTime::GetCurrentTime();
	CTime tLastConnect;

	if( KncUtil::ConvertStringToCTime( wstrLastDate, tLastConnect ) == false )
	{
		START_LOG( cerr, L"시간 변경 실패" )
			<< BUILD_LOG( wstrLastDate )
			<< END_LOG;
		return false;
	}

	CTimeSpan tSpanLastConnect = tCurr - tLastConnect;

	if( tSpanLastConnect.GetDays() < 0 )
	{
		CStringW cStrCurr, cStrLast;
		cStrCurr.Format(L"%d/%d/%d %d:%d:%d", tCurr.GetYear(), tCurr.GetMonth(), tCurr.GetDay(), tCurr.GetHour(), tCurr.GetMinute(), tCurr.GetSecond());
		cStrLast.Format(L"%d/%d/%d %d:%d:%d", tLastConnect.GetYear(), tLastConnect.GetMonth(), tLastConnect.GetDay(), tLastConnect.GetHour(),
			tLastConnect.GetMinute(), tLastConnect.GetSecond());

		START_LOG( clog, L"계산된 날짜 값이 이상합니다" )
			<< BUILD_LOG( tSpanLastConnect.GetDays() )
			<< BUILD_LOG( cStrCurr.GetBuffer())
			<< BUILD_LOG( cStrLast.GetBuffer())
			<< END_LOG;
		return false;
	}

	{
		// 반복문을 돌면서 접속 날 수 대비 보상 단계를 구한다.
		std::map< int, int >::iterator	mitMain = mapComeBackRewardCondition.begin();
		for( ; mitMain != mapComeBackRewardCondition.end() ; ++mitMain )
		{
			if( mitMain->second <= tSpanLastConnect.GetDays() )
			{
				if( iRewardStep < mitMain->first )
				{
					iRewardStep = mitMain->first;
				}
			}
		}
	}

	CTimeSpan tSpan;

	switch( iRewardStep )
	{
	case KGameSysVal::CBS_ONE:
		{
			tSpan = CTimeSpan( KGameSysVal::CBSD_ONE , 0, 0, 0);
			tCurr += tSpan;
			wstrEndBuffDate = tCurr.Format( _T("%Y-%m-%d %H:%M:%S") );
		}break;
	case KGameSysVal::CBS_TWO:
		{
			tSpan = CTimeSpan( KGameSysVal::CBSD_TWO , 0, 0, 0);
			tCurr += tSpan;
			wstrEndBuffDate = tCurr.Format( _T("%Y-%m-%d %H:%M:%S") );
		}break;
	case KGameSysVal::CBS_THREE:
		{
			tSpan = CTimeSpan( KGameSysVal::CBSD_THREE , 0, 0, 0);
			tCurr += tSpan;
			wstrEndBuffDate = tCurr.Format( _T("%Y-%m-%d %H:%M:%S") );
		}break;
	default:
		{
			wstrEndBuffDate = L"2000-01-01 00:00:00";
			START_LOG( clog, L"복귀유저 보상 단계가 잘못 되었다" )
				<< BUILD_LOG( iRewardStep )
				<< BUILD_LOG( tSpanLastConnect.GetDays() )
				<< END_LOG;
			return false;
		}break;
	}

	return true;
}

bool KGSAccountDBThread::CheckComeBackFirst( IN std::wstring& wstrEndDate )
{
	CTime tCurr = CTime::GetCurrentTime();
	CTime tEndDate;

	if( KncUtil::ConvertStringToCTime( wstrEndDate , tEndDate ) == false )
	{
		START_LOG( cerr, L"날짜 변환 실패!!!" )
			<< BUILD_LOG( wstrEndDate )
			<< END_LOG;
		return false;
	}

	if( tEndDate < tCurr )	// 복귀 버프 기간이 지났다.
	{
		// 그런데 복귀 유저다.
		// 보상 지급해야 한다.
		return true;
	}

	return false;
}

IMPL_ON_FUNC( DBE_WRITE_COME_BACK_REWARD_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	CTime tCurr = CTime::GetCurrentTime();
	CStringW strCurr = tCurr.Format(_T("%Y-%m-%d %H:%M:%S")).GetString();
	std::wstring wstrComeBackRegDate = strCurr.GetString();
	DO_QUERY( L"exec dbo.mup_create_Comebackuser_info", L"%d, %d, N\'%s\', N\'%s\'", % kPacket_.m_iUserUID %kPacket_.m_iRewardStep %kPacket_.m_wstrComeBackBuffEndDate %wstrComeBackRegDate );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

	// iOK = -1 : MUser 테이블에 해당 UserUID가 없을때
	// iOK = -2 : 휴먼복귀 유저 테이블에 최초로 데이터 insert 시, 에러
	// iOK = -3 : 휴먼복귀 유저 테이블에 이미 데이터 있는 상태에서 update 시, 에러

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"휴면 복귀 정보가 입력되지 않았습니다." )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iRewardStep )
			<< BUILD_LOG( kPacket_.m_wstrComeBackBuffEndDate )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_WRITE_COME_BACK_END_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.mup_update_Comebackuser_finish", L"%d", % kPacket_ );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

	// iOK = -1 : MUserComeback 테이블에 해당 UserUID가 없을때
	// iOK = -2 : 휴먼복귀 유저 테이블에 IsComeBackUser = 0 업데이트 시, 에러

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"휴면 복귀 정보가 입력되지 않았습니다." )
			<< BUILD_LOG( kPacket_ )
			<< BUILD_LOG( iOK )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_WRITE_COME_BACK_LOGOUT_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_insert_comeback_logout", L"%d, N\'%s\'", % kPacket_.m_iUserUID %kPacket_.m_wsrtLogOutDate );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

	// dbo.muser 에 해당 useruid 없을때 @iOK = -1
	// dbo.MUserComeback_Logout 인설트 시 에러 @iOK = -2
	// dbo.MUserComeback_Logout 업데이트 시 에러 @iOK = -3

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"휴면 복귀 정보용 로그아웃 기록 실패!!" )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_wsrtLogOutDate )
			<< BUILD_LOG( iOK )
			<< END_LOG;
	}
}
//}}

//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//}}

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
//}}

//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
IMPL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_WRITE_REQ )
{
	KEGS_KEYBOARD_MAPPING_INFO_WRITE_ACK kPacket;
	std::map<short, short>::const_iterator cit;

	kPacket.m_iOK = NetError::NET_OK;

	for( cit = kPacket_.m_kKeyboardMappingInfo.m_mapGamePadMappingInfo.begin(); cit != kPacket_.m_kKeyboardMappingInfo.m_mapGamePadMappingInfo.end(); ++cit )
	{
		short sAction = cit->first + GAMEPAD_ID_MODIFIER;
		short sKey = cit->second;
		kPacket_.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.insert( std::make_pair( sAction, sKey ) );
	}

	for( cit = kPacket_.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.begin(); cit != kPacket_.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.end(); ++cit )
	{
		DO_QUERY( L"exec dbo.P_MUserKeyBoard_INT", L"%d, %d, %d", % kPacket_.m_iUserUID % cit->first % cit->second );
		int iResult = -1;
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iResult );
			m_kODBC.EndFetch();
		}

		if( iResult != NetError::NET_OK )
		{
			kPacket.m_iOK = -1;
		}
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		if( cit == kPacket_.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.begin() )
		{
			// 한개도 기록 못했다.
			kPacket.m_iOK = NetError::ERR_KEYBOARD_MAPPING_01;
		}
		else
		{
			// 일부만 기록했다.
			kPacket.m_iOK = NetError::ERR_KEYBOARD_MAPPING_02;
		}
	}

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_KEYBOARD_MAPPING_INFO_WRITE_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_READ_REQ, UidType )
{
	KEGS_KEYBOARD_MAPPING_INFO_NOT kPacket;

	kPacket.m_iOK = NetError::NET_OK;
	DO_QUERY( L"exec dbo.P_MUserKeyBoard_GET", L"%d", % kPacket_ );
	while( m_kODBC.Fetch() )
	{
		int iResult;
		short iKey;
		short iValue;
		FETCH_DATA( iResult
			>> iKey
			>> iValue
			);

		if( iResult != NetError::NET_OK )
		{
			if( iResult != -1 )
			{
				kPacket.m_iOK = iResult;
			}
		}
		else
		{
			if( iKey >= GAMEPAD_ID_MODIFIER )
				kPacket.m_kKeyboardMappingInfo.m_mapGamePadMappingInfo.insert( std::map<int, int>::value_type( iKey - GAMEPAD_ID_MODIFIER, iValue ) );
			else
			kPacket.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.insert( std::map<int, int>::value_type( iKey, iValue ) );
		}
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		if( kPacket.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.empty() == true )
		{
			// 한개도 가져오지 못했다.
			kPacket.m_iOK = NetError::ERR_KEYBOARD_MAPPING_03;
		}
		else
		{
			// 일부만 가져왔다.
			kPacket.m_iOK = NetError::ERR_KEYBOARD_MAPPING_04;
		}
	}

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_KEYBOARD_MAPPING_INFO_READ_ACK, kPacket );
}
//}}

//{{ 2012. 05. 29	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
IMPL_ON_FUNC( DBE_CHAT_OPTION_INFO_WRITE_REQ )
{
	KEGS_CHAT_OPTION_INFO_WRITE_ACK kPacket;

	kPacket.m_iOK = NetError::NET_OK;
	DO_QUERY( L"exec dbo.P_MChatOption_INT", L"%d, %d, %d", % kPacket_.m_iUserUID % kPacket_.m_cIndex % kPacket_.m_bValue );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		kPacket.m_iOK = NetError::ERR_CHAT_OPTION_01;
		START_LOG( cerr, L"채팅 옵션 정보 저장 실패!!" )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_cIndex )
			<< BUILD_LOG( kPacket_.m_bValue )
			<< END_LOG;
	}

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_CHAT_OPTION_INFO_WRITE_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_CHAT_OPTION_INFO_READ_REQ, UidType )
{
	KEGS_CHAT_OPTION_INFO_NOT kPacket;

	kPacket.m_iOK = NetError::NET_OK;
	DO_QUERY( L"exec dbo.P_MChatOption_GET", L"%d", % kPacket_ );
	while( m_kODBC.Fetch() )
	{
		byte cIndex;
		bool bValue;
		FETCH_DATA( kPacket.m_iOK
			>> cIndex
			>> bValue
			);

		if( kPacket.m_iOK == NetError::NET_OK )
		{
			kPacket.m_mapChatOptionSetInfo.insert( std::map<byte, bool>::value_type( cIndex, bValue ) );
		}
		else if( kPacket.m_iOK == -1 )
		{
			kPacket.m_iOK = NetError::NET_OK;
		}
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		if( kPacket.m_mapChatOptionSetInfo.empty() == true )
		{
			// 한개도 가져오지 못했다.
			kPacket.m_iOK = NetError::ERR_CHAT_OPTION_02;
		}
		else
		{
			// 일부만 가져왔다.
			kPacket.m_iOK = NetError::ERR_CHAT_OPTION_03;
		}
	}

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_CHAT_OPTION_INFO_READ_ACK, kPacket );
}
//}}


//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
//}}


//{{ 2012. 09. 22	박세훈	빙고 이벤트
//}}

//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
//}}

//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
_IMPL_ON_FUNC( DBE_NEW_ACCOUNT_QUEST_REQ, KDBE_NEW_QUEST_REQ )
{
	CTime tNow = CTime::GetCurrentTime();
	CStringW strNow = tNow.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrNow = strNow.GetString();

	KDBE_NEW_ACCOUNT_QUEST_ACK kPacket;
	kPacket.m_iOK		= NetError::ERR_ODBC_01;
	kPacket.m_kQuestReq	= kPacket_;

	//////////////////////////////////////////////////////////////////////////
	// 계정 퀘스트 수락
	DO_QUERY( L"exec dbo.P_MQuests_INS_UPT", L"%d, %d, %d, %d, %d, %d, %d, N\'%s\'",
		% kPacket_.m_UserUID
		% kPacket_.m_iQuestID
		% 0
		% 0
		% 0
		% 0
		% 0
		% wstrNow  );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"계정 퀘스트 DB 생성 실패." )
			<< BUILD_LOG( LAST_SENDER_UID )
			<< BUILD_LOG( kPacket_.m_iQuestID )
			<< BUILD_LOG( kPacket.m_iOK )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_QUEST_10;
		goto end_proc;
	}

end_proc:

	SendToUser( LAST_SENDER_UID, DBE_NEW_ACCOUNT_QUEST_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_ACCOUNT_QUEST_COMPLETE_REQ, KDBE_QUEST_COMPLETE_REQ )
{
	CTime tNow = CTime::GetCurrentTime();
	CStringW strNow = tNow.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrNow = strNow.GetString();

	KDBE_ACCOUNT_QUEST_COMPLETE_ACK kPacket;
	kPacket.m_iOK			= NetError::ERR_ODBC_01;
	kPacket.m_kQuestReq		= kPacket_;

	std::wstring wstrCompleteDate;

	//{{ 2013. 03. 21	 계정 퀘스트 - 요일별 활성화 기능 - 김민성
	if( kPacket_.m_bIsRepeat == true )
	{
		DO_QUERY( L"exec dbo.P_MQuests_Repeat_INT", L"%d, %d, %d, N\'%s\'",
			% FIRST_SENDER_UID
			% kPacket_.m_UnitUID
			% kPacket_.m_iQuestID
			% wstrNow   );
	}
	else
	{
		DO_QUERY( L"exec dbo.P_MQuests_Complete_INT", L"%d, %d, %d, N\'%s\'",
			% FIRST_SENDER_UID
			% kPacket_.m_UnitUID
			% kPacket_.m_iQuestID
			% wstrNow   );
	}

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"계정 퀘스트 완료 DB 업데이트 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( FIRST_SENDER_UID )
			<< BUILD_LOG( kPacket_.m_UnitUID )
			<< BUILD_LOG( kPacket_.m_iQuestID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_QUEST_10;
		goto end_proc;
	}
	else
	{
		CTime tCompleteDate;
		LIF( KncUtil::ConvertStringToCTime( wstrCompleteDate, tCompleteDate ) );

		// 퀘스트 완료 업데이트 성공!
		kPacket.m_tCompleteTime = tNow.GetTime();
	}
	//}

end_proc:

	SendToUser( LAST_SENDER_UID, DBE_ACCOUNT_QUEST_COMPLETE_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_ACCOUNT_QUEST_UPDATE_NOT )
{
	CTime tNow = CTime::GetCurrentTime();
	CStringW strNow = tNow.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrNow = strNow.GetString();

	BOOST_TEST_FOREACH( KQuestUpdate&, kInfo, kPacket_.m_vecAccountQuesting )
	{
		if( kInfo.m_vecClearData.size() != 5 )
		{
			START_LOG( cerr, L"퀘스트 클리어 정보가 5개가 아니다! 절대 일어나서는 안되는 에러!" )
				<< BUILD_LOG( kPacket_.m_iUserUID )
				<< BUILD_LOG( kInfo.m_iQuestID )
				<< BUILD_LOG( kInfo.m_vecClearData.size() )
				<< END_LOG;
			continue;
		}

		int iOK = 0;
		DO_QUERY( L"exec dbo.P_MQuests_INS_UPT", L"%d, %d, %d, %d, %d, %d, %d, N\'%s\'",
			% kPacket_.m_iUserUID
			% kInfo.m_iQuestID
			% (int)kInfo.m_vecClearData[0]
			% (int)kInfo.m_vecClearData[1]
			% (int)kInfo.m_vecClearData[2]
			% (int)kInfo.m_vecClearData[3]
			% (int)kInfo.m_vecClearData[4]
			% wstrNow  );
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"계정 퀘스트 DB 생성 실패." )
				<< BUILD_LOG( kPacket_.m_iUserUID )
				<< BUILD_LOG( kInfo.m_iQuestID )
				<< BUILD_LOG( iOK )
				<< END_LOG;
		}
	}

	return;
end_proc:
	START_LOG( cerr, L"아왜!!!!계정 퀘스트 DB 생성 실패." )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< END_LOG;
}
//}}

//{{ 2012. 12. 19  교환(한정 수량) - 김민성
IMPL_ON_FUNC( DBE_ITEM_EXCHANGE_LIMIT_CHECK_REQ )
{
	KDBE_ITEM_EXCHANGE_LIMIT_CHECK_ACK kAck;
	kAck.m_iOK = NetError::ERR_ODBC_00;
	kAck.m_kDB = kPacket_.m_kToDB;

	// 결과물이 하나 뿐이라는 검사를 앞에서 했기 때문에...
	std::map<int, int>::iterator mit = kPacket_.m_kToDB.m_mapResultItem.begin();
	if( mit != kPacket_.m_kToDB.m_mapResultItem.end() )
	{
		DO_QUERY( L"exec dbo.P_MEvent_ExchangeQuantity_UPT", L"%d, %d, %d",
			% kPacket_.m_kReq.m_iSourceItemID
			% mit->first
			% mit->second	  );
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kAck.m_iOK );
			m_kODBC.EndFetch();
		}

		if( kAck.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"계정 퀘스트 DB 생성 실패." )
				<< BUILD_LOG( FIRST_SENDER_UID )
				<< BUILD_LOG( kPacket_.m_kReq.m_iSourceItemID )
				<< BUILD_LOG( kPacket_.m_kReq.m_iDestItemID )
				<< BUILD_LOG( kAck.m_iOK )
				<< END_LOG;
		}
	}

end_proc:

	SendToUser( LAST_SENDER_UID, DBE_ITEM_EXCHANGE_LIMIT_CHECK_ACK, kAck );
}
//}}

//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
//}


//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}}

//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
//}}
