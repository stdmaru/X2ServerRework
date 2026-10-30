#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"
#include "GameSysVal/GameSysVal.h"

#include "Enum/Enum.h"

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//{{ 2011. 05. 02  김민성	2차 보안 시스템
	#include "./RewardTable.h"
//}}
//}}

//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	#include "ODBC/Odbc.h"
//}}

//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	#include "ServerSNMaker.h"
//}}

//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
//}}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

IMPL_ON_FUNC( EGS_KNPROTECT_AUTH_CHECK_NOT )
{
	//if( m_kGGAuth.CheckAuthAnswer( GetThisPtr<KGSUser>(), kPacket_ ) == false )
	//{
	//	SetDisconnectReason( KStatistics::eSIColDR_NProtect_Wrong_Ack );
	//	ReserveDestroy();

	//	START_LOG( cerr, L"nProtect 인증 수신 실패.!" )
	//		<< END_LOG;
	//	return;
	//}
}

IMPL_ON_FUNC_NOPARAM( EGS_AGREE_HACK_USER_REQ )
{
	//특별한 스테이트 제한이 없다.
	if( m_cHackingUserType == SEnum::HUT_DISAGREE_HACK_USER )
	{
		SendToAccountDB( DBE_AGREE_HACK_USER_REQ, GetUID() );

		START_LOG( clog, L"해킹유저 동의.!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( m_kNexonAccountInfo.m_wstrID )
			<< END_LOG;

		return;
	}

	KEGS_AGREE_HACK_USER_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_HACK_USER_00;
	SendPacket( EGS_AGREE_HACK_USER_ACK, kPacket );

	START_LOG( cerr, L"해킹유저 리스트에 등록된 유저가 아닌데 패킷이 날아옴.!" )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( m_kNexonAccountInfo.m_wstrID )
		<< END_LOG;
}

_IMPL_ON_FUNC( DBE_AGREE_HACK_USER_ACK, KEGS_AGREE_HACK_USER_ACK )
{
	//특별한 스테이트 제한이 없다..흠~
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_cHackingUserType = SEnum::HUT_AGREE_HACK_USER;
		kPacket_.m_cHackingUserType = m_cHackingUserType;

		SendPacket( EGS_AGREE_HACK_USER_ACK, kPacket_ );
		return;
	}

	START_LOG( cerr, L"해킹유저 동의 디비 업데이트 실패.!" )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( m_kNexonAccountInfo.m_wstrID )
		<< END_LOG;

	SendPacket( EGS_AGREE_HACK_USER_ACK, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( EGS_REPORT_HACK_USER_NOT )
{
	//특별한 스테이트 제한이 없다. 흠!!
	if( m_cHackingUserType == SEnum::HUT_NORMAL )
	{
		START_LOG( clog, L"해킹 유저 신고.!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOGc( m_cHackingUserType );

		// 동의하지 않은 핵유저로 상태를 바꾸자!
		m_cHackingUserType = SEnum::HUT_DISAGREE_HACK_USER;

		SendToAccountDB( DBE_REPORT_HACK_USER_NOT, GetUID() );
	}
	else
	{
		START_LOG( cwarn, L"이미 신고된 유저임.!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOGc( m_cHackingUserType )
			<< END_LOG;
	}
}

IMPL_ON_FUNC_NOPARAM( EGS_NPROTECT_HACKING_USER_NOT )
{
	// nProtect 해킹 감지에 따른 통계설정 - 종료작업은 클라이언트에서 처리함.
	SetDisconnectReason( KStatistics::eSIColDR_nProtect_Hacking );
}


//{{ 2009. 12. 29  최육사	NPCKILLALL해킹방지
IMPL_ON_FUNC_NOPARAM( EGS_DUNGEON_KILLALLNPC_CHECK_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() <= 0 )
	{
		START_LOG( cerr, L"방이 아닌데 이 패킷을 날렸네?" )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		return;
	}

	if( CXSLRoom::GetRoomType( GetRoomUID() ) != CXSLRoom::RT_DUNGEON )
	{
		START_LOG( cerr, L"던전방이 아닌데 이 패킷을 날렸네?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;

		return;
	}

	// 던전방으로 날리자!
	SendToCnRoom( ERM_DUNGEON_KILLALLNPC_CHECK_REQ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_KILLALLNPC_CHECK_ACK, KEGS_DUNGEON_KILLALLNPC_CHECK_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_KILLALLNPC_CHECK_ACK, kPacket_ );
}

//}}

//{{ 2010. 05. 20  최육사	스크립트 파싱
//}}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
//{{ 2010. 06. 17  최육사	오토핵 방지
_IMPL_ON_FUNC( ERM_SERVER_CHECK_HACK_USER_NOT, KEGS_SERVER_CHECK_HACK_USER_REQ )
{
	// 이런 패킷은 상태 막지 말자..

	// 운영자면 해킹 유저로 처리 되지 않는다.
	if( GetAuthLevel() >= SEnum::UAL_GM )
	{
		START_LOG( cout, L"운영자가 해킹유저로 판단되었지만 해킹유저등록 및 계정 차단을 하지는 않음." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetUserName() )
			<< BUILD_LOGc( kPacket_.m_cHackingCheckType )
			<< END_LOG;
		return;
	}

	// 체크 항목에 맞게 제재 처리
	switch( kPacket_.m_cHackingCheckType )
	{
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_NPC_UNIT_DIE:			// 던전에서 몬스터를 죽인 정보가 이상한 경우
		{
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType );
			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
			HackUserKick( kPacket_ );

			//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			//}}
		}
		break;

	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_GET_ITEM:				// 발급되지 않은 아이템과 이상한 아이템을 획득하려는 경우
		{
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType );
			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
			HackUserKick( kPacket_ );

			//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			//}}
		}
		break;

	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_CANT_KILL_NPC_FIND:
		break; // 아무것도 하지 않는다!

	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_CANT_KILL_NPC_KICK:	// 보이지 않는 몬스터를 핵유저가 잡았다!
		{
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType );
			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
			HackUserKick( kPacket_ );

			//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			//}}
		}
		break;
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_PLAY_START:			// 이미 플레이 시작 했는데 또 플레이 시작하려는 경우!
		break;
		//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_CLEAR_RESULT:			// 던전 클리어 결과가 비정상적인 경우!
		{
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType );
			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
			HackUserKick( kPacket_ );

			//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			//}}
		}
		break;
		//}}
		//{{ 2010. 11. 18	최육사	오토 핵 스테이지 로딩 체크
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_STAGE_LOADING:		// 이미 로딩한 스테이지를 다시 로딩하려는 경우!
		{
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType );
			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
			HackUserKick( kPacket_ );

			//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			//}}
		}
		break;
		//}}
		//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DUNGEON_CLEAR_TIME:			// 던전 클리어 타임이 비정상적인 경우!
		{
//			HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );
//			HackUserRegTradeBlock( kPacket_.m_cHackingCheckType );
//			//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
//#ifdef SERV_AUTO_HACK_PACKET_MORNITOING
//			HackUserRegPacketMornitor();
//#endif SERV_AUTO_HACK_PACKET_MORNITOING
//			//}}
//
//			m_kUserHackingManager.SetHackingUserSendType( kPacket_.m_cHackingCheckType );
//
//			// 패킷만 날리자!
//			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
			HackUserRegRejectedUser( kPacket_.m_cHackingCheckType
				);
			SetRandomTimeKick( KStatistics::eSIColDR_Client_Hacking, 10, 50 );
		}
		break;
		//}}
		//{{ 2011. 09. 07  김민성	서브 스테이지 DIE NPC 시간 체크 기능
	case KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_NPC_DIE_TIME:			// 던전 서브 스테이지에서 모든 액티브 몬스터 사냥 시간이 짧은 경우!!(1초)
		{
			//HackUserRegMonitorsUser( kPacket_.m_cHackingCheckType );

			m_kUserHackingManager.SetHackingUserSendType( kPacket_.m_cHackingCheckType );

			// 패킷만 날리자!
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket_ );
		}
		break;
		//}}
		//{{ 2012. 02. 06    김민성    던전 몬스터 UID 발급 방식 변경
		//}}
	default:
		{
			START_LOG( cerr, L"등록되지 않는 핵유저 체크 타입 입니다!" )
				<< BUILD_LOGc( kPacket_.m_cHackingCheckType )
				<< END_LOG;
		}
		break;
	}
}
//}}
//}}

//{{ 2010. 10. 13	최육사	DLL Manager

IMPL_ON_FUNC( EGS_MODULE_INFO_UPDATE_NOT )
{
	// 상태 제한 없음

	//{{ 2013. 06. 12	최육사	제재 리스트 통합
	SendToGlobalServer( EGB_MODULE_INFO_UPDATE_NOT, kPacket_ );
	//}}

	//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
	//}}
}

//}}

//{{ 2011. 03. 02	최육사	개인 거래 차단
IMPL_ON_FUNC( ELG_TRADE_BLOCK_NOT )
{
	// 상태 제한 없음.

	m_kUserAbuserManager.SetTradeBlockUser( kPacket_.m_bIsTradeBlock );

	if( kPacket_.m_bIsTradeBlock )
	{
		START_LOG( cout, L"해킹 의심으로 인해 개인 거래가 차단되었습니다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() );
	}
	else
	{
		START_LOG( cout, L"개인 거래 차단이 해제되었습니다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() );
	}
}
//}}

//{{ 2011. 05. 02  김민성	2차 보안 시스템
IMPL_ON_FUNC( DBE_GET_SECOND_SECURITY_INFO_ACK )
{
    VERIFY_STATE_ACK( ( 2, KGSFSM::S_LOGINED, KGSFSM::S_SERVER_SELECT ), EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK );

	KEGS_SECOND_SECURITY_INFO_NOT kNot;
	kNot.m_iOK = kPacket_.m_iOK;
	kNot.m_bUseSecondPW = kPacket_.m_bUseSecondPW;

	if( kPacket_.m_iOK == NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT ) // 비밀 번호 입력 실패가 10회 이상일 때
	{
		SendPacket( EGS_SECOND_SECURITY_INFO_NOT, kNot );

		// 클라이서 종료 시키기로 하여 주석 됨
		//SetDisconnectReason( KStatistics::eSIColDR_Normal_Disconnect );
		//ReserveDestroy();															// 강제 종료
		return;
	}

	//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
	//}

	// DB 에서 받아온 데이터 저장 (성공 일 때)
	CTime tLasstAuthDate;
	if( kPacket_.m_bUseSecondPW == true )
	{
		if( KncUtil::ConvertStringToCTime( kPacket_.m_wstrLastAuthDate, tLasstAuthDate ) == false )
		{
			START_LOG( cerr, L"문자열 시간 변환 실패." )
				<< BUILD_LOG( kPacket_.m_wstrLastAuthDate )
				<< END_LOG;
		}

		//{{ 2011. 06. 23	최육사	2차 비번 예외처리
		StateTransition( KGSFSM::I_TO_CHECK_SECOND_PW );
		//}}
	}
	SetIsUseSecondPW( kPacket_.m_bUseSecondPW );
	SetSecondPWFailedCount( kPacket_.m_iFailedCount );
	SetSecondPW( kPacket_.m_wstrSecondPW );
	SetLastAuthDate( tLasstAuthDate );

	SendPacket( EGS_SECOND_SECURITY_INFO_NOT, kNot );

	if( m_kUserStatistics.IsFirstUserLoginToday( GetUID(), GetLastLoginDate() ) == true )
	{
		// 보안 사용 정보 로그 남기자!
		SendFirstLoginSecurityInfo();
	}

	//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
	//}}
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	if( kPacket_.m_bIsComeBackUser == true )	// 휴면 복귀 유저이다
	{
		m_kComeBackUserInfo.SetIsComeBackFirst( kPacket_.m_bIsFirst );
		m_kComeBackUserInfo.SetComeBackRewardLevel( kPacket_.m_iRewardStep );
		m_kComeBackUserInfo.SetComeBackBuffEndTime( kPacket_.m_wstrComeBackBuffEndDate );

		m_kTimer[TM_COME_BACK_USER].restart();		// 버프 시간 동기화 용

		if( kPacket_.m_bIsFirst == true )	// 최초 접속일 때 클라에게 알려주자
		{
			KEGS_COME_BACK_USER_NOT kNot;
			kNot = true;
			SendPacket( EGS_COME_BACK_USER_NOT, kNot );
		}
	}

	m_kComeBackUserInfo.SetIsComeBackUser( kPacket_.m_bIsComeBackUser );	// 휴면 복귀 유저인지 아닌지 셋팅

	//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
	//}}

	SendToGameDB( DBE_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ, GetName() );
	//}}

	//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
	//}}
}

IMPL_ON_FUNC( EGS_AUTH_SECOND_SECURITY_REQ )
{
	//{{ 2011. 06. 23	최육사	2차 비번 예외처리
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_CHECK_SECOND_PW, KGSFSM::S_SERVER_SELECT ), EGS_AUTH_SECOND_SECURITY_ACK );
	//}}

	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화 하여 DB에 저장
	std::string strPW = KncUtil::toNarrowString( kPacket_.m_wstrSecondPW );
	std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
	kPacket_.m_wstrSecondPW = KncUtil::toWideString( strChangePW.c_str() );
	//}}

	if( GetSecondPWFailedCount() < 10 && IsSameSecondSecurityPW( kPacket_.m_wstrSecondPW ) == true )
	{
		KDBE_SEUCCESS_SECOND_SECURITY_REQ kReq;
		CTime tCurr = CTime::GetCurrentTime();
		SetLastAuthDate( tCurr );

		kReq.m_iUserID = GetUID();
		kReq.m_wstrCurrTime = tCurr.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

		START_LOG( clog, L"2차 보안 인증 성공!!" )
			<< BUILD_LOG( GetUID() )
			<< END_LOG;
		SendToAccountDB( DBE_SEUCCESS_SECOND_SECURITY_REQ, kReq );
	}
	else
	{
		START_LOG( clog, L"2차 보안 인증 실패!!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( m_iFailedCount )
			<< END_LOG;
		SendToAccountDB( DBE_FAILED_SECOND_SECURITY_REQ, GetUID() );
	}
}

IMPL_ON_FUNC( DBE_SEUCCESS_SECOND_SECURITY_ACK )
{
	//{{ 2011. 06. 23	최육사	2차 비번 예외처리
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_CHECK_SECOND_PW, KGSFSM::S_SERVER_SELECT ), EGS_AUTH_SECOND_SECURITY_ACK );
	//}}

	if( kPacket_.m_iOK == NetError::NET_OK )	// 인증 성공 기록이 완료 되었다
	{
		//{{ 2011. 06. 23	최육사	2차 비번 예외처리
		StateTransition( KGSFSM::I_TO_SERVER_SELECT );
		//}}

		// 인증 성공을 하게 되면 값들은 초기화 된다.
		SetSuccessSecondSecurity( true );
		SetSecondPWFailedCount( 0 );

		KEGS_AUTH_SECOND_SECURITY_ACK kAck;

		kAck.m_iOK = NetError::NET_OK;
		kAck.m_iFailedCount = 0;

		SendPacket( EGS_AUTH_SECOND_SECURITY_ACK, kAck );

		if( KRewardTable::ERI_SECOND_SECURITY == 0 )
		return;

		// 오늘 6 이후 최초 접속이면
		CTime tLastLogin;
		CTime tNow = CTime::GetCurrentTime();
		CTime tRestart = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), 6, 0, 0 );		// 초기화 기점
		CTimeSpan tOneDay = CTimeSpan( 1, 0, 0, 0 );
		CTime tYesterRestart = tRestart - tOneDay;		// 이전 초기화 기점

		if( KncUtil::ConvertStringToCTime( kPacket_.m_wstrCurrTime, tLastLogin ) == false )
		{
			START_LOG( cerr, L"시간을 문자열로 변환 실패!!!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kAck.m_iOK )
				<< BUILD_LOG( kPacket_.m_wstrCurrTime )
				<< END_LOG;
			return;
		}

		if( tLastLogin >= tYesterRestart )	// 어제 받았다
		{
			if( tLastLogin < tRestart )	// 오늘 받을 조건이 된다
			{
				if( tRestart <= tNow )
				{
					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
					m_kUserUnitSelectRewardManager.AddRewardItemInfo( KRewardTable::ERI_SECOND_SECURITY );
					//}}
				}
			}
		}
		else
		{
			//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
			m_kUserUnitSelectRewardManager.AddRewardItemInfo( KRewardTable::ERI_SECOND_SECURITY );
			//}}
		}
	}
}

_IMPL_ON_FUNC( DBE_FAILED_SECOND_SECURITY_ACK, int )
{
	// 인증 실패 기록이 완료 되었다  // 실패 카운트가 담겨 온다
	KEGS_AUTH_SECOND_SECURITY_ACK kAck;

	SetSuccessSecondSecurity( false );
	SetSecondPWFailedCount( kPacket_ );
	kAck.m_iFailedCount = kPacket_;

	if( kPacket_ < 10 )
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_AUTH_FAILED;
	}
	else
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT;
	}

	SendPacket( EGS_AUTH_SECOND_SECURITY_ACK, kAck );
}

IMPL_ON_FUNC( EGS_CREATE_SECOND_SECURITY_REQ )
{
	KEGS_CHANGE_SECOND_SECURITY_PW_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;


	// 문자열 무결성 검사( 숫자로만, 4~6자리 인가? )
	if( CheckIntegritySecondSecurityPW( kPacket_.m_wstrSecondPW, kAck.m_iOK ) == false )
	{
		START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;

		SendPacket( EGS_CREATE_SECOND_SECURITY_ACK, kAck );
		return;
	}

	KDBE_CREATE_SECOND_SECURITY_REQ kReq;
	kReq.m_iUserID = GetUID();
	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화 하여 DB에 저장
	std::string strPW = KncUtil::toNarrowString( kPacket_.m_wstrSecondPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strPW )
		<< END_LOG;
	std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strChangePW )
		<< END_LOG;
	kReq.m_wstrSecondPW = KncUtil::toWideString( strChangePW.c_str() );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( kReq.m_wstrSecondPW )
		<< END_LOG;
	//}}
	SendToAccountDB( DBE_CREATE_SECOND_SECURITY_REQ, kReq );
}

IMPL_ON_FUNC( DBE_CREATE_SECOND_SECURITY_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SetIsUseSecondPW( true );
		SetSecondPWFailedCount( 0 );
		SetSecondPW( kPacket_.m_wstrSecondPW );
		SetSuccessSecondSecurity( true );

		START_LOG( clog, L"2차 보안 생성 성공!!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;
	}

	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 알려주면 안되니깐 지우자
	kPacket_.m_wstrSecondPW = L"";
	//}}

	SendPacket( EGS_CREATE_SECOND_SECURITY_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_DELETE_SECOND_SECURITY_REQ )
{
	KEGS_DELETE_SECOND_SECURITY_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;

	if( kPacket_.m_wstrSecondPW.empty() == true )
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
		SendPacket( EGS_DELETE_SECOND_SECURITY_ACK, kAck );
		return;
	}

	// 현재 비밀번호와 비교
	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화
	std::string strPW = KncUtil::toNarrowString( kPacket_.m_wstrSecondPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strPW )
		<< END_LOG;
	std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strChangePW )
		<< END_LOG;
	kPacket_.m_wstrSecondPW = KncUtil::toWideString( strChangePW.c_str() );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( kPacket_.m_wstrSecondPW )
		<< END_LOG;
	//}}

	if( IsSameSecondSecurityPW( kPacket_.m_wstrSecondPW ) == false )
	{
		START_LOG( clog, L"2차 보안 비밀번호의 값이 틀렸습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DIFFER_PW;
	}

	SendPacket( EGS_DELETE_SECOND_SECURITY_ACK, kAck );
}

IMPL_ON_FUNC( EGS_DELETE_SECOND_SECURITY_VERIFY_REQ )
{
	KEGS_DELETE_SECOND_SECURITY_VERIFY_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;

	if( kPacket_.m_wstrSecondPW.empty() == true )
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
		SendPacket( EGS_DELETE_SECOND_SECURITY_ACK, kAck );
		return;
	}

	// 현재 비밀번호와 비교
	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화
	std::string strPW = KncUtil::toNarrowString( kPacket_.m_wstrSecondPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strPW )
		<< END_LOG;
	std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( strChangePW )
		<< END_LOG;
	kPacket_.m_wstrSecondPW = KncUtil::toWideString( strChangePW.c_str() );
	START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( kPacket_.m_wstrSecondPW )
		<< END_LOG;
	//}}
	if( IsSameSecondSecurityPW( kPacket_.m_wstrSecondPW ) == false )
	{
		START_LOG( clog, L"2차 보안 비밀번호의 값이 틀렸습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrSecondPW )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DIFFER_PW;
		SendPacket( EGS_DELETE_SECOND_SECURITY_ACK, kAck );
		return;
	}

	if( kPacket_.m_bDelete )
	{
		KDBE_DELETE_SECOND_SECURITY_REQ kReq;
		kReq.m_iUserID = GetUID();
		GetSecondPW( kReq.m_wstrSecondPW );

		SendToAccountDB( DBE_DELETE_SECOND_SECURITY_REQ, kReq );
		return;
	}

	kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DELETE_FAILE;
	SendPacket( EGS_DELETE_SECOND_SECURITY_VERIFY_ACK, kAck );
}

IMPL_ON_FUNC( DBE_DELETE_SECOND_SECURITY_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SetIsUseSecondPW( false );
		SetSecondPWFailedCount( 0 );
		SetSuccessSecondSecurity( false );

		START_LOG( clog, L"2차 보안 해지 성공!!" )
			<< BUILD_LOG( GetUID() )
			<< END_LOG;
	}

	KEGS_DELETE_SECOND_SECURITY_VERIFY_ACK kAck;
	kAck.m_iOK = kPacket_.m_iOK;
	SendPacket( EGS_DELETE_SECOND_SECURITY_VERIFY_ACK, kAck );
}

IMPL_ON_FUNC( EGS_CHANGE_SECOND_SECURITY_PW_REQ )
{
	KEGS_CHANGE_SECOND_SECURITY_PW_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;

	// 문자열 무결성 검사( 숫자로만, 4~6자리 인가? )
	if( CheckIntegritySecondSecurityPW( kPacket_.m_wstrNewSecondPW, kAck.m_iOK ) == false ||
		CheckIntegritySecondSecurityPW( kPacket_.m_wstrOldSecondPW, kAck.m_iOK ) == false )
	{
		START_LOG( clog, L"2차 보안 비밀번호의 값이 이상합니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrNewSecondPW )
			<< BUILD_LOG( kPacket_.m_wstrOldSecondPW )
			<< END_LOG;

		SendPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK, kAck );
		return;
	}

	// 현재 비밀번호와 비교
	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화
	std::string strOldPW = KncUtil::toNarrowString( kPacket_.m_wstrOldSecondPW );
	std::string strChangeOldPW = KServerSNMaker::GetCRCFromString( strOldPW );
	kPacket_.m_wstrOldSecondPW = KncUtil::toWideString( strChangeOldPW.c_str() );
	//}}
	if( IsSameSecondSecurityPW( kPacket_.m_wstrOldSecondPW ) == false )
	{
		START_LOG( clog, L"2차 보안 비밀번호의 값이 틀렸습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrNewSecondPW )
			<< BUILD_LOG( kPacket_.m_wstrOldSecondPW )
			<< END_LOG;

		KEGS_CHANGE_SECOND_SECURITY_PW_ACK kAck;
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DIFFER_PW;

		SendPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK, kAck );
		return;
	}

	// 현재 비밀번호와 동일한 비번으로 셋팅 하려 할 때
	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화
	std::string strNewPW = KncUtil::toNarrowString( kPacket_.m_wstrNewSecondPW );
	std::string strChangeNewPW = KServerSNMaker::GetCRCFromString( strNewPW );
	kPacket_.m_wstrNewSecondPW = KncUtil::toWideString( strChangeNewPW.c_str() );
	//}}
	if( IsSameSecondSecurityPW( kPacket_.m_wstrNewSecondPW ) == true )
	{
		START_LOG( clog, L"2차 보안의 새비밀번호가 이전 비밀번호와 동일합니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_wstrNewSecondPW )
			<< BUILD_LOG( kPacket_.m_wstrOldSecondPW )
			<< END_LOG;

		KEGS_CHANGE_SECOND_SECURITY_PW_ACK kAck;
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW_3;

		SendPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK, kAck );
		return;
	}

	KDBE_CHANGE_SECOND_SECURITY_PW_REQ kReq;
	kReq.m_iUserID = GetUID();
	kReq.m_wstrOldSecondPW = kPacket_.m_wstrOldSecondPW;
	kReq.m_wstrNewSecondPW = kPacket_.m_wstrNewSecondPW;

	SendToAccountDB( DBE_CHANGE_SECOND_SECURITY_PW_REQ, kReq );
}

IMPL_ON_FUNC( DBE_CHANGE_SECOND_SECURITY_PW_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SetSecondPW( kPacket_.m_wstrSecondPW );
		SetSecondPWFailedCount( 0 );

		START_LOG( clog, L"2차 보안 비밀번호 변경 성공!!" )
			<< BUILD_LOG( GetUID() )
			<< END_LOG;
	}

	KEGS_CHANGE_SECOND_SECURITY_PW_ACK kAck;
	kAck.m_iOK = kPacket_.m_iOK;

	SendPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK, kAck );
}

IMPL_ON_FUNC( EGS_COMPARE_SECOND_SECURITY_PW_REQ )
{
	KEGS_COMPARE_SECOND_SECURITY_PW_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;

	//{{  2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
	// 암호화
	std::string strPW = KncUtil::toNarrowString( kPacket_ );
	std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
	kPacket_ = KncUtil::toWideString( strChangePW.c_str() );
	//}}

	// 2차 보안 패드 사용 중이고 비밀번호가 동일 할 때
	if( GetIsUseSecondPW() == false )
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_NO_USE;
	}
	else if( IsSameSecondSecurityPW( kPacket_ ) == false )
	{
		kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DIFFER_PW;
	}

	SendPacket( EGS_COMPARE_SECOND_SECURITY_PW_ACK, kAck );
}

IMPL_ON_FUNC( EGS_CHECK_SECOND_SECURITY_PW_CHRACTER_LIST_REQ )
{
    KEGS_CHECK_SECOND_SECURITY_PW_CHRACTER_LIST_ACK kAck;
    kAck.m_iOK = NetError::NET_OK;

    // 2011.12.20     김민성    2차 보안 패드 비밀번호 암호화
    std::string strPW = KncUtil::toNarrowString( kPacket_ );
    std::string strChangePW = KServerSNMaker::GetCRCFromString( strPW );
    kPacket_ = KncUtil::toWideString( strChangePW.c_str() );

    // 2차 보안 패드 사용 중이고 비밀번호가 동일 할 때 통과
    if( GetIsUseSecondPW() == false )
    {
        kAck.m_iOK = NetError::ERR_SECOND_SECURITY_NO_USE;
    }
    else if( IsSameSecondSecurityPW( kPacket_ ) == false )
    {
        kAck.m_iOK = NetError::ERR_SECOND_SECURITY_DIFFER_PW;
    }
    else
    {
        StateTransition( KGSFSM::I_TO_SERVER_SELECT );
    }

    SendPacket( EGS_CHECK_SECOND_SECURITY_PW_CHRACTER_LIST_ACK, kAck );
}

//}}

//{{ 2011. 08. 09	최육사	머신ID 블럭기능
IMPL_ON_FUNC( EGS_CHECK_MACHINE_ID_REQ )
{
	// 상태 제한은 없습니다!

	// 차단 기능 동작 여부
	if( SiKGameSysVal()->IsMachineIDBlock() == false )
	{
		KEGS_CHECK_MACHINE_ID_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::NET_OK;
		kPacketAck.m_strMachineID = kPacket_.m_strMachineID;
        SendPacket( EGS_CHECK_MACHINE_ID_ACK, kPacketAck );
		return;
	}

	// 문자열 길이 체크!
	if( kPacket_.m_strMachineID.empty()  ||  kPacket_.m_strMachineID.size() > 24 )
	{
		START_LOG( cerr, L"머신ID값이 이상합니다! 접속 차단!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_strMachineID )
			<< END_LOG;

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_MACHINE_ID;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}
		//}}

		Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	std::wstring wstrMachineID = KncUtil::toWideString( kPacket_.m_strMachineID );
	if( KODBC::IsInvalidMarkInForMachineID( wstrMachineID ) == true )
	{
		START_LOG( cerr, L"머신ID값이 이상합니다! 접속 차단!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_strMachineID )
			<< END_LOG;

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_MACHINE_ID;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}
		//}}

		Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
		return;
	}

	//{{ 2011. 09. 07	최육사	머신 ID 기능 업데이트
	if( m_kUserSecurityManager.CheckValidMachineID( kPacket_.m_strMachineID ) == false )
	{
		START_LOG( cout, L"[알림] 머신ID가 유효하지 않다! 접속 차단 시키자!" )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_strMachineID );

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_MACHINE_ID;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}

		// 머신ID가 비어있으면 접속 차단!
		Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
		return;
	}
	//}}

	SendToLoginServer( ELG_CHECK_MACHINE_ID_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ELG_CHECK_MACHINE_ID_ACK, KEGS_CHECK_MACHINE_ID_ACK )
{
	// 상태 제한은 없습니다!

	// 일단 저장해두자!
	m_kUserSecurityManager.SetMachineID( kPacket_.m_strMachineID );

	// 인증 실패했다면 접속 차단!
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_BLOCK_MACHINE_ID;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}
		//}}

		Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
		return;
	}

	SendPacket( EGS_CHECK_MACHINE_ID_ACK, kPacket_ );
}
//}}

//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
//}}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
IMPL_ON_FUNC( EGS_SERVER_CHECK_HACK_USER_ACK )
{
	char cLastSendType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_NONE;
	if( m_kUserHackingManager.SetHackingUserReciveType( kPacket_.m_cHackingCheckType, cLastSendType ) == false )
	{
		START_LOG( cerr, L"해킹 유저 패킷을 보냈는데 잘못된 응답이다." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOGc( cLastSendType )
			<< BUILD_LOGc( kPacket_.m_cHackingCheckType )
			<<END_LOG;

		// 클라에게 패킷을 쏘자
		SendID( EGS_CHECK_HACK_USER_PACKET_REQUEST_NOT );

		// 킥하자
		if( SiKGameSysVal()->GetHackingUserKick() == true )
		{
			SetDisconnectReason( KStatistics::eSIColDR_Client_Hacking );
			ReserveDestroy();
		}
	}
}
//}}



IMPL_ON_FUNC_NOPARAM( ERM_DUNGEON_KILLALLNPC_CHECK_NOT )
{
	SendID( EGS_DUNGEON_KILLALLNPC_CHECK_NOT );
}




IMPL_ON_FUNC_NOPARAM( EGS_GET_SECOND_SECURITY_INFO_REQ )
{
    KDBE_GET_PURE_SECOND_SECURITY_INFO_REQ kReq;
    kReq.m_iUserUID = GetUID();
    //SiKGameSysVal()->GetComeBackRewardCondition( kReq.m_mapComeBackRewardCondition );

    SendToAccountDB( DBE_GET_PURE_SECOND_SECURITY_INFO_REQ, kReq );	// AccountDB에 최근 접속 종료 정보 얻기
}

IMPL_ON_FUNC( DBE_GET_PURE_SECOND_SECURITY_INFO_ACK )
{
    KEGS_SECOND_SECURITY_INFO_NOT kNot;
    kNot.m_iOK = kPacket_.m_iOK;
    kNot.m_bUseSecondPW = kPacket_.m_bUseSecondPW;

    if( kPacket_.m_iOK == NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT ) // 비밀 번호 입력 실패가 10회 이상일 때
    {
        SendPacket( EGS_GET_SECOND_SECURITY_INFO_ACK, kNot );

        // 클라이서 종료 시키기로 하여 주석 됨
        //SetDisconnectReason( KStatistics::eSIColDR_Normal_Disconnect );
        //ReserveDestroy();															// 강제 종료
        return;
    }

    // DB 에서 받아온 데이터 저장 (성공 일 때)
    CTime tLastAuthDate;
    if( kPacket_.m_bUseSecondPW == true )
    {
        if( KncUtil::ConvertStringToCTime( kPacket_.m_wstrLastAuthDate, tLastAuthDate ) == false )
        {
            START_LOG( cerr, L"문자열 시간 변환 실패." )
                << BUILD_LOG( kPacket_.m_wstrLastAuthDate )
                << END_LOG;
        }

        //{{ 2011. 06. 23	최육사	2차 비번 예외처리
        //StateTransition( KGSFSM::I_TO_CHECK_SECOND_PW );
        //}}
    }
    SetIsUseSecondPW( kPacket_.m_bUseSecondPW );
    SetSecondPWFailedCount( kPacket_.m_iFailedCount );
    SetSecondPW( kPacket_.m_wstrSecondPW );
    SetLastAuthDate( tLastAuthDate );

    SendPacket( EGS_GET_SECOND_SECURITY_INFO_ACK, kNot );

    if( m_kUserStatistics.IsFirstUserLoginToday( GetUID(), GetLastLoginDate() ) == true )
    {  // 보안 사용 정보 로그 남기자!
        SendFirstLoginSecurityInfo();
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
