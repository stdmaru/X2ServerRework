#include "x2/platform/format.h"
#include "GameServer.h" // KGameServer, GetKGameServer (the original got it through the precompiled header)
#include "GSUser.h"
#include <algorithm>
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "ProxyManager.h"
#include "DBLayer.h"
#include "NetError.h"
#include "GSSimLayer.h"

#include "ActorManager.h"
#include "ExpTable.h"
#include "SpiritTable.h"
#include "StatTable.h"
#include "BaseServer.h"
#include "Socket/NetCommon.h"

// 넥슨 빌링
#include "NexonBillingTCPManager.h"

#include "X2Data/XSLSquareUnit.h"
#include "X2Data/XSLResolveItemManager.h"
#include "GameSysVal/GameSysVal.h"
#include "GameEvent/GameEventManager.h"
#include "RecommendUserTable.h"
#include "TutorialManager.h"
#include "RoomListManager.h"
#include "ChannelManager.h"
//{{ 2008. 12. 10  최육사	로봇테스트
//}}

//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
	#include "X2Data/XSLBattleFieldManager.h"
	#include "Socket/NetCommon.h"
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	#include "GSWorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	#include "ComeBackUserManager.h"
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	#include "RewardTable.h"
//}}
//}}

//{{ 2011. 06. 21	최육사	대전 개편
	#include "X2Data/XSLUnit.h"
//}}
//{{ 2011. 10. 13	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	#include "CompanyIPTable.h"
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
	#include "ClassChangeTable.h"
//}}
//{{ 2011. 12. 14	최육사	패킷 처리 지연 시간 체크
	#include "PacketMornitoring/PacketMornitoring.h"
//}}

//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
//}}

//{{ 2012. 04. 17	박세훈	어둠의 문 개편
	#include "X2Data/XSLItemManager.h"
	#include "X2Data/XSLBuffManager.h"
//}}

//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	#include "EventDelaySystemManager.h"
//}}

//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	#include "SMSPhoneNumberManager.h"
//}}


//{{ 2013. 3. 12	박세훈	 로컬 랭킹 시스템
//}}

#include "BillManager.h"

//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
#include "X2Data/XSLSocketItem.h"
//}
#include "UserRidingPetManager.h"
#include "X2Data/XSLRidingPetManager.h"

//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
#include "GSGlobalMissionManager.h"
//}} 2012. 09. 06	임홍락	글로벌 미션 매니저

	#include "GSFieldBossManager.h"
	#include "X2Data/XSLFieldBossData.h"
	#include "X2Data/XSLBattleFieldManager.h"
#include "GameEvent/GameEventScriptManager.h" //SERV_EVENT_VALENTINE_DUNGEON_GIVE_ITEM


#include "NetLayer.h"
#include "ODBC/Odbc.h"
#include "Enum/Enum.h"

//////////////////////////////////////////////////////////////////////////
#pragma NOTE( "The GSUserFuntion.cpp file compiles successfully!" )
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//{{ 2011. 04. 04	최육사	deserializing 실패 로그
#define _CASE_CHECK_ERR( id, packet ) \
case id: \
{ \
	KSerializer ks; \
	ks.BeginReading( &spEvent_->m_kbuff ); \
	packet kPacket; \
	if( !ks.Get(kPacket) ) \
	{ \
		START_LOG( cerr, L"deserialze failed." L ## #id L" - " L ## #packet ) \
			<< BUILD_LOG( GetName() ) \
			<< BUILD_LOG( GetCharName() ); \
		m_kUserHackingManager.IncreaseDeserializeFailCount( id ); \
	} \
	else \
	{ \
		ON_##id( spEvent_->m_anTrace, kPacket); \
		spEvent_->m_kbuff.Reset(); \
	} \
} \
break

#define  CASE_CHECK_ERR_NOPARAM(id)           case id: ON_##id( spEvent_->m_anTrace ); break
#define  CASE_CHECK_ERR(id)                  _CASE_CHECK_ERR(id, K##id)
//}}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
#define _CASE_CHECK_ERR_AND_FILTER( id, packet ) \
case id: \
{ \
	KSerializer ks; \
	ks.BeginReading( &spEvent_->m_kbuff ); \
	packet kPacket; \
	std::wstring wstrPacketName = L ## #packet; \
	if( CheckIsValidPacketEvent( spEvent_, wstrPacketName.c_str(), ks.Get(kPacket), spEvent_->IsValidEventID() ) ) \
	{ \
		ON_##id( spEvent_->m_anTrace, kPacket); \
		spEvent_->m_kbuff.Reset(); \
	} \
} \
break

#define  CASE_CHECK_ERR_AND_FILTER_NOPARAM(id) \
case id: \
{ \
	if( CheckIsValidPacketEvent( spEvent_, L"", true, spEvent_->IsValidEventID() ) ) \
	{ \
		ON_##id( spEvent_->m_anTrace ); \
	} \
} \
break

#define  CASE_CHECK_ERR_AND_FILTER(id)                  _CASE_CHECK_ERR_AND_FILTER(id, K##id)
//}}
//////////////////////////////////////////////////////////////////////////

NiImplementRTTI( KGSUser, KActor );
ImplPfID( KGSUser, PI_GS_USER );

KGSUser::KGSUser(void)
:
m_iGlobalMissionTickCount(0),
m_uiKNMSerialNum(0),
//{{ 2012. 02. 02	최육사	배틀필드 시스템
//}}
m_nUnitSlot(0),
m_uiRoomListID(0),
m_iRoomUID(0),
//{{ 2012. 03. 21	최육사	배틀필드 시스템
m_iBeforeRoomUID(0),
//}}
m_iTradeUID(0),
m_iPersonalShopUID(0)
//{{ 2011. 04. 13  김민성  글로벌 서버 추가
, m_iFieldUID(0)
//}} 2011. 04. 13  김민성  글로벌 서버 추가
//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
//}}
, m_iWorldMissionTickCount(0)

//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
, m_bHeroPvpUser( false )
//}} 2012. 09. 06	임홍락	글로벌 미션 매니저
{
	//////////////////////////////////////////////////////////////////////////
	// 유닛 단위 초기화
    ResetUnitInfo();

	//////////////////////////////////////////////////////////////////////////
	// 계정단위 초기화
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	m_kAccountInfo.m_iAuthLevel		= SEnum::UAL_NORMAL;
	m_kAccountInfo.m_bInternalUser	= false;
	//}}

    m_tConnectTime		= CTime::GetCurrentTime();

    m_bLoginFail		= true;
    m_bLogoutFail		= true;
    m_kInventory.SetOwner( this );

    m_bReportDisconnectToLogin	= true;

	//nProtect 한국인증 모듈 사용설정
    //if( KSimLayer::GetKObj()->CheckCommonFlag( KSimLayer::CommonFlag::CF_NPGG ) )
    //{
    //    m_kGGAuth.SetEnable( true );
    //}

	// 2011.05.05 lygan_조성욱 //  중국용 커뮤니티탭 유저 정보 찾기
	m_cCommunity_Tap_type = 0;

	//m_kGGAuth.Init();
	m_kRepeatEvent.Reset();

	//{{ 2010. 9. 2	최육사	중폭 패킷 필터 개선
	//}}

	InitTransactionEventFilter();


	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	m_iED.SetMaxValue( SEnum::UI_MAX_ED_CHAR_HAVE );
	//}}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	m_kUserAbuserManager.Clear();
	//}}
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	m_setMyUnitUIDList.clear();
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	SetIsUseSecondPW( false );
	SetSecondPWFailedCount( 0 );
	SetSuccessSecondSecurity( false );
	SetIsUseOTP( 0 );
	m_iNexonSecurityType = 0;
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	m_kComeBackUserInfo.Clear();
	//}}
	//{{ 2012. 03. 14	김민성		패킷 시퀀스 카운트 체크
	m_bCheckSequence = false;
	//}}

	//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
	//}}



		//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
		//}}

		m_bSC1 = false;
		m_bSC2 = false;
		m_iMaxLevelTickCount = 0;
		m_iActiveDungeonTickCount = -1;

		m_iCodeEventTickCount = 0;








	m_bSteamClient = false;
	m_bAeriaClient = false;

	m_ucOldYearMissionRewardedLevel = 0;
	m_iNewYearMissionStepID = -1;


}

KGSUser::~KGSUser(void)
{
}

ImplToStringW( KGSUser )
{
    return START_TOSTRING_PARENTW( KActor );
}

void KGSUser::Tick()
{
    KActor::Tick();


	// note!! 기존에 스킬 슬롯 B, 캐시 스킬 포인트를 아이템으로 기간을 표시하던 것을 UI에서 표시하는 것으로 바뀌면서 스킬슬롯체인지메달 등은 인벤에 아이템으로 존재하지 않는다


	// 1. 스킬 슬롯 B에 대한 기간 체크
	ExpireSkillSlotB();

	// 2. 캐시스킬 포인트에 대한 기간 체크
	ExpireCashSkillPoint();

	// 3. 기간제 아이템의 기간 체크
    ExpireItem();

	// 4. 시간 이벤트에 대한 체크
	CheckTimeEvent();

	// 5. 현재 장착중인 타이틀에 대한 기간 체크
	ExpireTitle();

	//{{ 2009. 10. 14  최육사	자동결제
	CheckAutoPayment();
	//}}

	//{{ 2011. 12. 22	최육사	배틀필드 시스템
	CheckWaitPartyUser();
	CheckAutoPartyBonus();
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	CheckHackingToolList();
	//}}
	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	CheckLetterAndTradeMonitoring();
	//}}

	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	CheckDeserializeFailCount();
	//}}

	//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
	CheckAutoPartyWaitUserCountRequest();
	//}}

    if( !m_pkCurrentState->CheckState( 5, KGSFSM::S_INIT, KGSFSM::S_UID_UPDATED, KGSFSM::S_LOGINED, KGSFSM::S_EXIT, KGSFSM::S_SERVER_SELECT ) )
    {
        if( m_kTimer[TM_DB_UPDATE].elapsed() >= 60.0 * 20.0 )
        {
            WriteUnitInfoToDB( false );

			//{{ 2012. 09. 22	박세훈	빙고 이벤트
			//}}
        }
    }



	//////////////////////////////////////////////////////////////////////////
	//KOREA NPROTECT 인증
	//if( m_kGGAuth.Tick( GetThisPtr<KGSUser>() ) == false )
	//{
	//	SetDisconnectReason( KStatistics::eSIColDR_NProtect_Auth_Fail );
	//	ReserveDestroy();

	//	START_LOG( cerr, L"nProtect 인증 실패.!" )
	//		<< END_LOG;
	//	return;
	//}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Port: HackShield and XTrap checks removed with the anti-cheat (T10)
	//}}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	CheckUpdateWorldMission();
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	CheckComeBackUser();
	//}}

	//{{ 2011. 08. 12   김민성      헤니르 개편
	CheckHenirRewardCountEvent();
	//}}

	//{{ 버프 관리 시스템 - 김민성
	CheckBuffPeriod();
	//}}

	CheckTradeBlockUnit();

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	CheckRelationshipReward();
	//}

    switch( GetStateID() )
	{
    case KGSFSM::S_UID_UPDATED:
		//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
		// 여기서 아무런 처리를 하지 않는다!
		//}}
        break;

	case KGSFSM::S_ROOM:
		break;

    case KGSFSM::S_EXIT:
        KActorManager::GetKObj()->ReserveDelete( m_nUID );    // 자기 자신을 스스로 삭제할 수 없음.
    	break;
    }

	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}}
	//{{ 2012. 06. 13	김민성       선택적 셧다운제
		//{{ 2012. 01. 25  김민성	자정에 셧다운제 자동 유저 검사 기능
		//}}
	//}}

	//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
	//}}

	//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	CheckRandomTimeKick();
	//}}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	{
		char cLastSendType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_NONE;
		if( m_kUserHackingManager.CheckHackingUserRecivedPacket( cLastSendType ) == false )
		{
			START_LOG( cerr, L"해킹 유저 패킷을 보냈는데 응답이 없다." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOGc( cLastSendType )
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

	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
	//}}

	//{{ 2013. 03. 18	 만우절 이벤트_레인보우 버프 - 김민성
	//}

	CheckMaxLevel();
	CheckEnableEventDungeon();

	CheckEnableCodeEvent();



	m_kAfterWorkStorage.Tick();



	//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
	CheckUpdateGlobalMission();
	//}} 2012. 09. 06	임홍락	글로벌 미션 매니저

}

void KGSUser::OnDestroy()
{

	//{{ 2011.2.11  조효진  모니터링툴 서버 포트 체크 시 에러로그 남는거 수정
	// 모니터링 툴 서버의 IP에서는 게임을 할수 없다고 가정함 //
	// 모니터링 서버에 의한 포트 체크일 경우 아래의 다른 동작들 할 필요없음
	if( KBaseServer::GetKObj()->IsMonitoringServerForPortCheck( GetIPStr() ) )
	{
		StateTransition( KGSFSM::I_TO_EXIT );	// 11. 유저객체 삭제를 위해 FSM을 S_EXIT로 변환
		KSession::OnDestroy();
		START_LOG( clog, L"Mornitoring server check Port!" );
		return;
	}
	//}}

	// 2011.05.05 lygan_조성욱 //  중국용 커뮤니티탭 유저 정보 찾기
	m_cCommunity_Tap_type = 0;

	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}}

	// 캐릭터 정보가 유효하다면 캐릭터 선택 해제 작업을 진행하자!
	if( GetCharUID() > 0 )
	{
		//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링 // 캐릭터 선택 해제 작업
		OnDeselectUnit( EGS_CLIENT_QUIT_REQ, true );
	}




	//{{ 2012. 03. 29	김민성		중복 sn 접속 감시
	//}}

	// 2-1. 종료에 대한 통계 남기기 [USER_FSM]
	const int iServerUID = static_cast<int>(KBaseServer::GetKObj()->GetUID());
	const int iIndex = KStatistics::GetDisconnectUserFSM( GetStateID() );

	if( iIndex >= 0 && iIndex < KStatistics::eSIColDU_Num )
	{
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( iServerUID );
		KSIManager.IncreaseCount( KStatistics::SI_DISCONNECT_USERFSM, kKey, iIndex, 1 );
	}
	else
	{
		START_LOG( cerr, L"disconnect userfsm 통계 인덱스 값이 이상합니다." )
			<< BUILD_LOG( GetStateID() )
			<< BUILD_LOG( iIndex )
			<< END_LOG;
	}

	// 2-2. 종료에 대한 통계 남기기 [REASON]
	KStatisticsKey kKey;
	kKey.m_vecIntKey.push_back( iServerUID );
	KSIManager.IncreaseCount( KStatistics::SI_DISCONNECT_REASON, kKey, GetDisconnectReason(), 1 );

	// 2-3. 각 종료 사유별 처리.
	switch( GetDisconnectReason() )
	{
	case KStatistics::eSIColDR_IOCP_GetStatus:
		{
			//{{ 2009. 9. 1  최육사		IOCP종료사유

			START_LOG( clog, L"IOCP종료사유" )
				<< BUILD_LOG( GetUserName() )
				<< BUILD_LOG( GetLastIocpFailedError() );

			GetKGSSimLayer()->AddAbnormalDisconnectReason( GetLastIocpFailedError() );

			//{{ 2009. 9. 13  최육사	유저 로그
			if( KSIManager.IsUserLog() )
			{
				m_kUserLogManager.DumpLogFile( GetThisPtr<KGSUser>(), KUserLogManager::ULT_ABNORMAL_DICONNECT );
			}
			//}}
			//}}
		}
		break;

		//{{ 2010. 02. 01  최육사	하트비트 유저로그

	case KStatistics::eSIColDR_Heart_Beat:
		{
			if( KSIManager.IsUserLog() )
			{
				m_kUserLogManager.DumpLogFile( GetThisPtr<KGSUser>(), KUserLogManager::ULT_HEART_BEAT );
			}
		}
		break;

		//}}

	case KStatistics::eSIColDR_SendBufferFull:
		{
			START_LOG( cout, L"Send Buffer Full 종료 유저 상태." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( KEvent::GetIDStr( GetLastSendEventID() ) )
				<< BUILD_LOG( GetStateIDString() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetFieldUID() )
				<< BUILD_LOG( GetRoomListID() )
				<< BUILD_LOG( GetRoomUID() )
				<< BUILD_LOG( GetTradeUID() )
				<< BUILD_LOG( GetPersonalShopUID() )
				<< BUILD_LOG( (int)CXSLRoom::GetRoomType( GetRoomUID() ) );
		}
		break;

	case KStatistics::eSIColDR_Traffic_Attack:
		{
			//{{ 2009. 10. 22  최육사	트래픽 유발 유저 해킹유저로 자동등록
			KELOG_TRAFFIC_ATTACKER_LOG_NOT kPacketLog;
			kPacketLog.m_wstrUserID = GetName();
			kPacketLog.m_iChannelID = SiKChannelManager()->GetChannelID();
			SendToLogDB( ELOG_TRAFFIC_ATTACKER_LOG_NOT, kPacketLog );
			//}}

			//{{ 2009. 10. 26  최육사	트래픽공격방지
			SendID( EGS_TRAFFIC_ATTACKER_NOT );
			//}}
		}
		break;

	default:
		break;
	}

	// 3. LConnectLog 기록
	if( GetStateID() >= KGSFSM::S_LOGINED )
	{
		WritePlayTimeToDB();

		//{{ 2011. 05. 02  김민성	2차 보안 시스템
		// 11. 2차 보안 패드 및 OTP 사용 유무 정보 기록
		SendSecurityType();
		//}}

		//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		if( m_kComeBackUserInfo.GetIsUnitSelect() == true )
		{
			SendComeBackLogOut();
		}
		//}}
	}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

    KGSUserPtr spUser( GetThisPtr<KGSUser>() );

    // 2007.12. 27. jseop. 로그인 못하는 유저가 있남?
	if( GetStateID() < KGSFSM::S_LOGINED )
    {
		switch( GetDisconnectReason() )
		{
		case KStatistics::eSIColDR_Limit_User_Num:
			{
				START_LOG( cwarn, L"서버 수용 인원 가득참." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetStateID() )
					<< BUILD_LOG( GetStateIDString() )
					<< END_LOG;
			}
			break;

			//{{ 2010. 06. 23  최육사	계정블럭기능
		case KStatistics::eSIColDR_GameServer_Account_Block:
			{
				START_LOG( cwarn, L"해킹 의심 유저 접속 제한 조치." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetStateID() )
					<< BUILD_LOG( GetStateIDString() )
					<< END_LOG;
			}
			break;
			//}}

			//{{ 2011. 08. 09	최육사	머신ID 블럭기능
		case KStatistics::eSIColDR_MachineID_Block:
			{
				START_LOG( cout, L"머신 ID 블럭." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetStateID() )
					<< BUILD_LOG( GetStateIDString() )
					<< END_LOG;
			}
			break;
			//}}

		default:
			{
				START_LOG( cerr2, L"로그인 실패 유저." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetStateID() )
					<< BUILD_LOG( GetStateIDString() )
					<< END_LOG;
			}
			break;
		}
    }


	// 10. 모든 접속 종료 준비가 완료 되었다면 로그인서버에 유저의 접속종료 통보
	SendDisconnectEvent();


	// 11. 유저객체 삭제를 위해 FSM을 S_EXIT로 변환
    StateTransition( KGSFSM::I_TO_EXIT );

	//{{ 2008. 11. 10  최육사	모니터링 어뷰저
	//if( m_bIsMornitoringAbuser )
	//{
	//	std::wstring wstrIPAddr = KncUtil::toWideString( GetIPStr(), strlen( GetIPStr() ) );

	//	KDBE_SEND_PHONE_MSG_NOT kPacketNot;
	//	kPacketNot.m_wstrSMSMessage  = L"어뷰저 유저 [";
	//	kPacketNot.m_wstrSMSMessage += GetName();
	//	kPacketNot.m_wstrSMSMessage += L"]가 접속종료하였습니다! : ";
	//	kPacketNot.m_wstrSMSMessage += wstrIPAddr;

	//	SiKAbuserLogManager()->GetAbuserNotifyPhoneNumList( kPacketNot.m_wstrPhoneNum );

	//	SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
	//}
	//}}

	//{{ 2011. 08. 10    김민성    채널링 SOAP
	//}}


    KSession::OnDestroy();
}

bool KGSUser::RoutePacket( const KEvent* pkEvent_ )
{
    //START_LOG( clog, L"라우팅" )
    //    << BUILD_LOG( pkEvent_->m_kDestPerformer.m_dwPerformerID )
    //    << BUILD_LOG( pkEvent_->m_kDestPerformer.m_iUID )
    //    << BUILD_LOG( pkEvent_->m_usEventID )
    //    << BUILD_LOG( pkEvent_->GetIDStr() );

    if( pkEvent_->m_kDestPerformer.m_dwPerformerID != PI_GS_USER )
    {
        int iEval = CompareServerLevel( pkEvent_->m_kDestPerformer.m_dwPerformerID, PI_GS_USER );
        if( iEval < 0 )
        {
            _JIF( !pkEvent_->IsEmptyTrace(), return true );
            KEvent kEvent( *pkEvent_ );
            kEvent.PopTrace();
            LIF( KSession::SendPacket( kEvent ) );

			//{{ 2009. 9. 13  최육사	유저 로그
			m_kUserLogManager.SaveSendPacket( kEvent.m_usEventID );
			//}}

            return true;
        }
        else if( iEval > 0 )
        {
            KEvent kEvent( *pkEvent_ );
            kEvent.PushTrace( GetUID() );
            SiKProxyManager()->SendPacket( GetUID(), kEvent );
            return true;
        }
        else
        {
            KEventPtr spEvent( pkEvent_->Clone() );
            spEvent->PushTrace( GetUID() );

            switch( GetPerformerClass( pkEvent_->m_kDestPerformer.m_dwPerformerID ) )
            {
            case PC_CHARACTER:
                break;
            case PC_SERVER:
                KBaseServer::GetKObj()->QueueingEvent( spEvent );
                return true;
            case PC_GAME_DB:
                SiKDBLayer()->QueueingEvent( spEvent );
                return true;
            default:
                START_LOG( cerr, L"도착지가 이상한 패킷." )
                    << BUILD_LOG( pkEvent_->m_kDestPerformer.m_dwPerformerID )
                    << END_LOG;
                return true;
            }
        }
    }

    return false;
}

void KGSUser::WriteUnitInfoToDB( bool bFinal, int iEventID /*= 0*/ )
{
    // 유닛이 선택되지 않았으면 업데이트 하지 말아야 한다.
    if( GetCharUID() <= 0 )
    {
		START_LOG( cerr, L"[ERROR] 유닛 정보 업데이트 하는데, 유닛이 선택되지 않았다.")
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( iEventID )
			<< END_LOG;

        return;
    }

    KDBE_UPDATE_UNIT_INFO_REQ kPacket;
    kPacket.m_bFinal		= bFinal;
	kPacket.m_iEventID		= iEventID;	// for 채널이동
    kPacket.m_iUnitUID		= GetCharUID();
	kPacket.m_iEXP			= m_kEXP.GetChangeExp();
    kPacket.m_iLevel        = static_cast<int>(GetLevel());
	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	kPacket.m_iED			= m_iED.GetChangeValue();		// SetValue()는 이 함수 아래쪽에 있습니다.
	//{{ 2011. 09. 23	최육사	로그오프 상태 ED체크
	kPacket.m_iRealDataED	= GetED();
	//}}
	//{{ 2012. 04. 17	최육사	개인거래 DDOS방어
	if( m_kInventory.IsLocked() == true )
	{
		if( m_iED.GetChangeValue() != 0 )
		{
			START_LOG( cout, L"[알림] Inventroy Lock 상태인데 ED증분값이 남아있다! 이러면 문제 생길텐데..?" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetED() )
				<< BUILD_LOG( m_iED.GetChangeValue() );
		}
	}
	//}}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//}}


	//{{ 2011. 06. 20	최육사	대전 개편
	m_kUserPvpManager.GetDBUpdateInfo( kPacket.m_iOfficialMatchCnt,
										kPacket.m_iRating,
										kPacket.m_iMaxRating,
										kPacket.m_iRPoint,
										kPacket.m_iAPoint,
										kPacket.m_bIsWinBeforeMatch,
										kPacket.m_iWin,
										kPacket.m_iLose,
										kPacket.m_cEmblemEnum

										//{{ 2012. 06. 22	박세훈	2012 대전 시즌2
										, kPacket.m_fKFactor
										//}}
										);
	//}}
	kPacket.m_kDenyOptions  = m_kUserGameOption.GetDenyOptions();

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	m_kUserSpiritManager.GetDBUpdateInfo( kPacket.m_iSpirit, kPacket.m_bIsSpiritUpdated );
	//}}


	//{{ 2009. 10. 7  최육사	길드
	kPacket.m_iGuildUID		= GetGuildUID();
	kPacket.m_iHonorPoint	= m_kUserGuildManager.GetHonorPoint();
	//}}

	// 부활석
	kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone.GetChangeValue();
    m_iNumResurrectionStone.SetValue( m_iNumResurrectionStone );

	//{{ 2009. 10. 14  최육사	자동결제 부활석
	kPacket.m_iNumAutoPaymentResStone = m_iNumAutoPaymentResStone.GetChangeValue();
	m_iNumAutoPaymentResStone.SetValue( m_iNumAutoPaymentResStone );
	//}}
	//{{ 2012. 02. 21	최육사	배틀필드 시스템
	m_kUserUnitManager.GetGamePlayStatus( kPacket.m_kGamePlayStatus );
	//}}

	//////////////////////////////////////////////////////////////////////////
	// Packing LastPosition

	//07.07.19.  hoon. 현재 상태이동시 마지막 마을 아이디가 0으로 셋팅 되는 시점이 생기기때문에 임시로 루벤을 설정함.
	//마지막 마을만 DB에 저장하면되고 현재의 마을 정보는 유지를 위해 패킷에 바로 넣는다.
	//{{ 2012. 02. 02	최육사	배틀필드 시스템
	// MapID검증
	if( bFinal )
	{
		if( SEnum::IsBattleFieldID( GetMapID() ) == true )
		{
			// 그냥 통과!
		}
		else
		{
			if( SiCXSLMapData()->CheckMapData( GetMapID() ) == false )
			{
				START_LOG( cerr, L"DB업데이트 하려는 MapID가 이상하다?" )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( GetMapID() )
					<< BUILD_LOG( SiCXSLMapData()->GetDefaultMapID() )
					<< END_LOG;

				// 기본값을 넣어준다!
				SetMapID( SiCXSLMapData()->GetDefaultMapID() );
			}
		}
	}

	// 마지막 위치가 보스 필드라면, 원래 있던 배틀 필드 ID로 바꿔준다.
	if( SiCXSLBattleFieldManager()->IsBossFieldID( GetMapID() ) == true )
	{
		KBossFieldJoinInfo kBossFieldJoinInfo;
		GetBossFieldJoinInfo( kBossFieldJoinInfo );

		// 원래 있던 필드가 정상
		if( SiCXSLBattleFieldManager()->IsExistBattleFieldID( static_cast<SEnum::BATTLE_FIELD_ID>( kBossFieldJoinInfo.m_iReturnMapID ) ) == true )
		{
			KLastPositionInfo kLastPositionInfo;
			kLastPositionInfo.m_iMapID					= kBossFieldJoinInfo.m_iReturnMapID;
			kLastPositionInfo.m_ucLastTouchLineIndex	= kBossFieldJoinInfo.m_ucLastTouchLineIndex;
			kLastPositionInfo.m_usLastPosValue			= kBossFieldJoinInfo.m_usLastPosValue;
			kLastPositionInfo.m_bIgnoreLastTouch		= false;
			SetLastPosInfo( kLastPositionInfo );
		}
	}

	kPacket.m_kLastPos = GetLastPosInfo(); // 더이상 packing해서 사용하지 않습니다.
	//}}
	//////////////////////////////////////////////////////////////////////////

    std::map< int, KDungeonClearInfo >::iterator mit;
    for( mit = m_mapDungeonClear.begin(); mit != m_mapDungeonClear.end(); mit++ )
    {
        if( mit->second.m_bNew )
        {
            kPacket.m_mapDungeonClear.insert( std::make_pair( mit->first, mit->second ) );
        }
        mit->second.m_bNew = false;
    }

	std::map< int, KDungeonPlayInfo >::iterator mitDungeonPlay;
	for( mitDungeonPlay = m_mapDungeonPlay.begin(); mitDungeonPlay != m_mapDungeonPlay.end(); mitDungeonPlay++ )
	{
		if( mitDungeonPlay->second.m_bNew )
		{
			kPacket.m_mapDungeonPlay.insert( std::make_pair( mitDungeonPlay->first, mitDungeonPlay->second ) );
		}
		mitDungeonPlay->second.m_bNew = false;
	}

	std::map< int, KTCClearInfo >::iterator mitt;
	for( mitt = m_mapTCClear.begin(); mitt != m_mapTCClear.end(); ++mitt )
	{
		if( mitt->second.m_bNew )
		{
			kPacket.m_mapTCClear.insert( std::make_pair( mitt->first, mitt->second ) );
		}
		mitt->second.m_bNew = false;
	}

    m_kInventory.FlushQuantityChange( kPacket.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.FlushEnduranceChange( kPacket.m_kItemEnduranceUpdate );
    m_kInventory.FlushDeletedItem( kPacket.m_kItemQuantityUpdate.m_vecDeleted );
    m_kInventory.FlushPositionChange( kPacket.m_kItemPositionUpdate );

	//QUEST UPDATE
	m_kUserQuestManager.GetUpdatedQuestInfo( kPacket.m_vecQuestData );

	//{{ 2010. 02. 09  최육사	일일 이벤트 퀘스트
	m_kUserQuestManager.GetReservedGiveUpQuest( kPacket.m_vecGiveUpQuestList );
	//}}

	// MISSION UPDATE
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	m_kUserTitleManager.GetUpdateInfo( IsPcBang(), kPacket.m_iInitTitleID, kPacket.m_iEquippedTitleID, kPacket.m_vecMissionData );
	//}}

	//SKILL SLOT UPDATE
	m_kSkillTree.GetSkillSlotFromEveryPage( kPacket.m_mapSkillSlotVector );
	kPacket.m_wstrSkillSlotBEndDate = m_kSkillTree.GetSkillSlotBEndDateString();

	if( KUserSkillTree::SSBES_EXPIRED == m_kSkillTree.GetSkillSlotBExpirationState() )
	{
		std::map< int, std::vector<int> >::iterator mIterSkillSlotVector
			= kPacket.m_mapSkillSlotVector.begin();

		for ( ; mIterSkillSlotVector != kPacket.m_mapSkillSlotVector.end(); ++mIterSkillSlotVector )
		{
			mIterSkillSlotVector->second.resize( KUserSkillTree::SKILL_SLOT_B1 );
		}
	}

	// 장바구니 업데이트
	if( bFinal )
	{
		m_kUserWishList.FlushWishListChange( kPacket.m_mapWishList );
	}

	//{{ 2009. 7. 7  최육사		랭킹 개편
	m_kUserRankingManager.GetUpdateInfo( kPacket.m_vecHenirRanking
		);
	//}}

	//{{ 2009. 12. 8  최육사	이벤트개편
	m_kUserEventManager.GetDBUpdateInfo( kPacket.m_vecUpdateEventTime );
	//}}

	//{{ 2010. 8. 4	최육사	펫 시스템
	m_kUserPetManager.GetDBUpdatePetInfo( kPacket.m_vecPet );
	//}}
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	if( bFinal )
	{
		// 유저 통계 접속시간
		const int iDurationTime = static_cast<int>( m_kTimer[TM_UNIT_DURATION].elapsed() + 0.5 );
		m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_DUNRATION_TIME, iDurationTime / 60 );

		// DB 업데이트 정보 얻기
		m_kUserStatistics.GetDBUpdateInfo( kPacket.m_mapCharGameCount );
	}
	//}}

	//{{ 2011. 08. 12   김민성      헤니르 개편

	kPacket.m_kHenirRewardCnt.m_iNormal = m_kUserDungeonManager.GetHenirRewardNormalCount();
	kPacket.m_kHenirRewardCnt.m_iPremium = m_kUserDungeonManager.GetHenirRewardPremiumCount();
	kPacket.m_kHenirRewardCnt.m_iEvent = m_kUserDungeonManager.GetHenirRewardEventCount();

	//}}

	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}

	m_kUserRidingPetManager.GetDBUpdateInfo( kPacket.m_vecRidingPetList );
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kPacket.m_cUnitRelationshipType = m_kUserRelationshipManager.GetRelationshipType();
	//}


    SendToGameDB( DBE_UPDATE_UNIT_INFO_REQ, kPacket ); // [중요] 유닛 정보 초기화는 DBE_UPDATE_UNIT_INFO_ACK에서..
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// DB업데이트 타이머 초기화
    m_kTimer[TM_DB_UPDATE].restart();

	m_kEXP.SetExp( m_kEXP.GetExp() );
	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	m_iED.SetValue( m_iED );
	//}}


	std::vector<KUserSkillTree::SkillPageData>& vecSkillPageData
		= m_kSkillTree.AccessSkillPageDataVector();

	for ( UINT i = 0; i < vecSkillPageData.size(); i++ )
	{
		vecSkillPageData[i].m_iSPoint.SetValue( vecSkillPageData[i].m_iSPoint );
	}


	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

	//{{ 2007. 10. 16  최육사  유저 통계
	if( bFinal  &&  ( GetAuthLevel() == SEnum::UAL_NORMAL || GetAuthLevel() == SEnum::UAL_BLOCKED_1 ) )
	{
		// 유저 통계 접속시간
		const int iDurationTime = static_cast<int>( m_kTimer[TM_UNIT_DURATION].elapsed() + 0.5 );
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Duration_Time, iDurationTime );

		// 유저 통계 습득 아이템
		//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
		const int iObtainedItemCount = m_kUserQuestManager.GetQuestRewardItemCount() + m_kUserAccountQuestManager.GetQuestRewardItemCount();
		//}}
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Obtained_Item, iObtainedItemCount );

		//{{ 2010. 11. 17	최육사	오토핵 검증 기능
		m_kUserAbuserManager.IncreaseGetItemCount( GetThisPtr<KGSUser>(), iObtainedItemCount );
		//}}

		CTime tDisconnectTime = CTime::GetCurrentTime();

		// 근성도 소모량
		const int iUsedSpirit = m_kUserStatistics.GetUsedSpirit();

		KELOG_USER_STATISTICS kPacketLog;
		kPacketLog.m_iUserUID = GetUID();
		kPacketLog.m_iUnitUID = GetCharUID();
		kPacketLog.m_iAuthLevel				=	 GetAuthLevel();
		kPacketLog.m_cUnitClass				=	 GetUnitClass();
		kPacketLog.m_iED					=	 GetED();
		kPacketLog.m_ucLevel				=	 GetLevel();
		kPacketLog.m_uiGiantUID				=	 m_kNexonAccountInfo.m_uiNexonSN;
		kPacketLog.m_wstrLoginTime			=	 ( CStringW )( GetUnitLoginTime_INT().Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );

		kPacketLog.m_wstrLogoutTime			=	 ( CStringW )( tDisconnectTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
		kPacketLog.m_wstrNickName			=	 GetCharName();
		kPacketLog.m_wstrAccountCreateDate	=	 m_kUserStatistics.GetAccountRegDate();
		kPacketLog.m_iEXP					=	 GetEXP();
		kPacketLog.m_wstrUnitCreateDate		=	 m_kUserStatistics.GetUnitCreateDate();
		kPacketLog.m_iiDurationTime			=	 iDurationTime;

		m_kUserStatistics.GetCharacterStatistics( kPacketLog.m_mapPvpStatistics, kPacketLog.m_mapDungeonStatistics, kPacketLog.m_mapTCenterStatistics, kPacketLog.m_mapEtcStatistics, kPacketLog.m_mapEDStatistics );

		SendToLogDB( ELOG_USER_STATISTICS, kPacketLog );

		// LogDBThread로 이벤트 전송 후 통계는 초기화시킨다
		m_kUserStatistics.Clear();

		//{{ 2010. 06. 30  최육사	근성도 통계
		KDBE_SPIRIT_STATISTICS_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		//{{ 2012. 11. 14	박세훈	Field PT 로그 추가
		kPacketNot.m_iSpiritPlayTime = m_kUserStatistics.GetPlayTime( KUserStatistics::PT_SPIRIT_PLAY_TIME );
		m_kUserStatistics.ResetPlayTime( KUserStatistics::PT_SPIRIT_PLAY_TIME );
		//}}
		kPacketNot.m_iDurationTime = iDurationTime;
		kPacketNot.m_iUsedSpirit = iUsedSpirit;
		SendToLogDB( DBE_SPIRIT_STATISTICS_NOT, kPacketNot );
		//}}
	}
	//}}

	//{{ 2008. 11. 19  최육사	어뷰저 자동 등록
	if( GetED() >= KUserAbuserManager::AE_ED_ABUSER_LIMIT )
	{
		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		m_kUserAbuserManager.RegEDAbuser( GetThisPtr<KGSUser>() );
		//}}
	}
	//}}

	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}
}

void KGSUser::WritePlayTimeToDB()
{
	CTime tDisconnectTime = CTime::GetCurrentTime();
	KELOG_STAT_PLAY_TIME kPacket;
	kPacket.m_iUserUID		  = GetUID();
	kPacket.m_wstrIP		  = KncUtil::toWideString( GetIPStr() );
	kPacket.m_wstrLoginTime	  = ( CStringW )( m_tConnectTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	kPacket.m_wstrLogoutTime  = ( CStringW )( tDisconnectTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	kPacket.m_bLoginFail	  = m_bLoginFail;
	kPacket.m_bLogoutFail	  = m_bLogoutFail;
	kPacket.m_iTotalPlayTime  = m_kUserStatistics.GetPlayTime( KUserStatistics::PT_TOTAL_PLAY_TIME );
	kPacket.m_iPvpPlayTime	  = m_kUserStatistics.GetPlayTime( KUserStatistics::PT_PVP_PLAY_TIME );
	//{{ 2012. 11. 14	박세훈	Field PT 로그 추가
	kPacket.m_iDungeonPlayTime	= m_kUserStatistics.GetPlayTime( KUserStatistics::PT_DUNGEON_PLAY_TIME );
	kPacket.m_iFieldPlayTime	= m_kUserStatistics.GetPlayTime( KUserStatistics::PT_FIELD_PLAY_TIME );
	//}}
	kPacket.m_iDisconnectReason	= GetDisconnectReason();
	kPacket.m_iDisconnectUserFSM = GetStateID();
	//{{ 2009. 12. 22  최육사	서버군확장
	kPacket.m_iServerGroupID  = KBaseServer::GetKObj()->GetServerGroupID();
	//}}
	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	kPacket.m_wstrMachineID	  = m_kUserSecurityManager.GetWStringMachineID();
	//}}
	//{{ 2011. 09. 27	김민성	PC 방 오과금 방지
	//}}

	kPacket.m_ucChannelCode = static_cast<u_char>(GetChannelCode());

	kPacket.m_bSteamClient = IsSteamClientUser();

	kPacket.m_bAeriaClient = IsAeriaClientUser();

	// StatisticsDB에 기록
	SendToLogDB( ELOG_STAT_PLAY_TIME, kPacket );

	// AccountDB에 통합 기록
	SendToAccountDB( ELOG_STAT_PLAY_TIME, kPacket );
}

void KGSUser::SendDisconnectEvent( u_short usEventID /*= -1*/ )
{
    if( GetStateID() >= KGSFSM::S_UID_UPDATED && GetStateID() < KGSFSM::STATE_SENTINEL )
    {
        if( m_bReportDisconnectToLogin )
        {
			KELG_USER_DISCONNECT_REQ kPacket;
			kPacket.m_usEventID = usEventID;
			//{{ 2011. 02. 23	최육사	계정 단위 카운트
			m_kUserStatistics.GetAccountDBUpdateInfo( kPacket.m_mapAccCountInfo );
			//}}
            SendToLoginServer( ELG_USER_DISCONNECT_REQ, kPacket );
        }
    }
}

//{{ 2009. 4. 3  최육사		치트로그
void KGSUser::WriteCheatLogToDB( const wchar_t* szCheatCode, const std::wstring& wstrCheatContents )
{
	// 치트 로그
	KELOG_CHEAT_LOG_NOT kPacket;
	kPacket.m_iUserUID		  = GetUID();
	kPacket.m_iUnitUID		  = GetCharUID();
	kPacket.m_cAuthLevel	  = static_cast<char>(GetAuthLevel());
	kPacket.m_wsCheatCode	  = szCheatCode;
	kPacket.m_wsCheatContents = wstrCheatContents;

	if( KODBC::IsInvalidMarkInForCheat( kPacket.m_wsCheatCode ) )
	{
		START_LOG( cerr, L"SQL Injection 방지" )
			<< BUILD_LOG( kPacket.m_wsCheatCode )
			<< END_LOG;
		return;
	}

	if( KODBC::IsInvalidMarkInForCheat( kPacket.m_wsCheatContents ) )
	{
		START_LOG( cerr, L"SQL Injection 방지" )
			<< BUILD_LOG( kPacket.m_wsCheatContents )
			<< END_LOG;
		return;
	}

	SendToLogDB( ELOG_CHEAT_LOG_NOT, kPacket );
}

void KGSUser::WriteCheatLogToDB( const wchar_t* szCheatCode, const CStringW& cwstrCheatContents )
{
	std::wstring wstrCheatContents = static_cast<LPCTSTR>(cwstrCheatContents);
	WriteCheatLogToDB( szCheatCode, wstrCheatContents );
}

void KGSUser::WriteCheatLogToDB( const wchar_t* szCheatCode )
{
	std::wstring wstrTemp;
    WriteCheatLogToDB( szCheatCode, wstrTemp );
}
//}}

//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
void KGSUser::WriteCharacterLogToDB( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType )
{
	WriteCharacterLogToDBWithParam( eLogType, m_kUserStatistics.GetLastClearDungeonID(), 0 );
}

void KGSUser::WriteCharacterLogToDBWithDungeonID( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iDungeonID )
{
	WriteCharacterLogToDBWithParam( eLogType, iDungeonID, 0 );
}

void KGSUser::WriteCharacterLogToDBWithProductNo( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iProductNo )
{
	WriteCharacterLogToDBWithParam( eLogType, m_kUserStatistics.GetLastClearDungeonID(), iProductNo );
}

void KGSUser::WriteCharacterLogToDBWithParam( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iDungeonID, IN const int iProductNo )
{
	if( KELOG_USER_CHARACTER_LOG_NOT::IsValidLogType( eLogType ) == false )
	{
		START_LOG( cerr, L"잘못된 로그 타입 입니다!" )
			<< BUILD_LOG( eLogType )
			<< END_LOG;
		return;
	}

	// 캐릭터 로그 남기는 대상이 아니면 여기서 리턴!
	if( m_kUserStatistics.IsCharCount() == false )
		return;

	const int iDurationTime = static_cast<int>( m_kTimer[TM_UNIT_DURATION].elapsed() + 0.5 ) / 60;
	CTime tCurTime = CTime::GetCurrentTime();

	KELOG_USER_CHARACTER_LOG_NOT kPacketToLog;
	kPacketToLog.m_iUserUID				 = GetUID();
	kPacketToLog.m_iUnitUID				 = GetCharUID();
	kPacketToLog.m_wstrNickName 		 = GetCharName();
	kPacketToLog.m_bOnlyOneUnit 		 = ( m_setMyUnitUIDList.size() == 1 );
	kPacketToLog.m_iServerUID			 = KBaseServer::GetKObj()->GetUID();
	kPacketToLog.m_sLogType				 = eLogType;
	kPacketToLog.m_ucLevel				 = GetLevel();
	kPacketToLog.m_cUnitClass			 = GetUnitClass();
	kPacketToLog.m_iDungeonID			 = iDungeonID;
	kPacketToLog.m_iProductNo			 = iProductNo;
	kPacketToLog.m_iTotalDungeonPlayTime = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_DUNGEON_PLAY_TIME );
	kPacketToLog.m_iTotalPvpPlayTime	 = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_PVP_PLAY_TIME );
	kPacketToLog.m_iDurationTime		 = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_DUNRATION_TIME ) + iDurationTime;
	kPacketToLog.m_wstrAccountCreateDate = m_kUserStatistics.GetAccountRegDate();
	kPacketToLog.m_wstrUnitCreateDate	 = m_kUserStatistics.GetUnitCreateDate();
	kPacketToLog.m_wstrRegDate		 	 = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	SendToLogDB( ELOG_USER_CHARACTER_LOG_NOT, kPacketToLog );
}
//}}

//{{ 2012. 11. 9	최육사	배틀필드 시스템
void KGSUser::DungeonGameStart_SingleOpenDungeonRoom( IN const char cDungeonMode, IN const int iDungeonID, IN const char cDifficultyLevel, IN const bool bAutoParty )
{
	const int iDungeonIDWithDif = iDungeonID + static_cast<int>(cDifficultyLevel);

	// 새로 방을 만들면서 던전 게임을 시작하자!
	KERM_OPEN_ROOM_REQ kPacket;
	kPacket.m_kRoomInfo.m_bStartedByAutoParty	= bAutoParty;
	kPacket.m_bQuickJoin						= false;
	kPacket.m_iCurED							= GetED();
	kPacket.m_iCurEXP							= GetEXP();
	kPacket.m_wstrChannelIP						= NetCommon::GetLocalIPW();
	kPacket.m_kRoomInfo.m_RoomType				= CXSLRoom::RT_DUNGEON;
	kPacket.m_kRoomInfo.m_uiRoomListID			= GetRoomListID();
	kPacket.m_kRoomInfo.m_RoomName				= L"혼자 하는 던전";
	kPacket.m_kRoomInfo.m_bPublic				= false;
	kPacket.m_kRoomInfo.m_cDungeonMode			= cDungeonMode;
	kPacket.m_kRoomInfo.m_iDungeonID			= iDungeonID;
	kPacket.m_kRoomInfo.m_DifficultyLevel		= cDifficultyLevel;
	kPacket.m_kRoomInfo.m_fPlayTime				= DUNGEON_DEFAULT_LIMIT_PLAY_TIME;
	//{{ 2013. 04. 10	최육사	어둠의 문 개편
	kPacket.m_kRoomInfo.m_bDefenceDungeonOpen	= SiKGSWorldMissionManager()->GetIsActive();
	//}}

	float fPlayTimeLimit = SiCXSLDungeonManager()->GetPlayTimeLimit( iDungeonIDWithDif );
	if( fPlayTimeLimit > 0.0f )
	{
		kPacket.m_kRoomInfo.m_fPlayTime = fPlayTimeLimit;
	}

	// 던전 플레이 정보를 저장
	m_kUserDungeonManager.SetDungeonGameInfo( iDungeonID, cDifficultyLevel, cDungeonMode );

	// KRoomUserInfo얻기
	GetRoomUserInfo( kPacket.m_kRoomUserInfo, CXSLRoom::RT_DUNGEON );

	// 싱글 던전 게임 시작!
	SendToCnRoom( ERM_SINGLE_OPEN_DUNGEON_ROOM_REQ, kPacket );
}
//}}

void KGSUser::ExpireItem( bool bForce/* = false */ )
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
    {
        if( ( m_kTimer[TM_CHECK_ITEM_EXPIRATION].elapsed() < 60.0 * 5.0 ) && !bForce )
        {
            return;
        }

        m_kTimer[TM_CHECK_ITEM_EXPIRATION].restart();

		CheckRidingPetProcess();

		CheckExpirePet();

        KEGS_ITEM_EXPIRATION_NOT kPacket;
        std::vector< KInventoryItemInfo > vecInventorySlotInfo;
        m_kInventory.ExpireItem( kPacket.m_vecItemUID, vecInventorySlotInfo );
        if( kPacket.m_vecItemUID.empty() )
        {
            return;
        }

        SendPacket( EGS_ITEM_EXPIRATION_NOT, kPacket );

		//{{ 2008. 12. 3  최육사	필드에서 기간다된 아이템 갱신
		if( GetStateID() == KGSFSM::S_FIELD_MAP )
		{
			if( GetFieldUID() <= 0 )
			{
				START_LOG( clog, L"필드 상태인데 FieldUID가 없다." )
					<< BUILD_LOG( GetFieldUID() )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< END_LOG;
				return;
			}

			KERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT kPacketNot;
			kPacketNot.m_iUnitUID = GetCharUID();
			std::vector< KInventoryItemInfo >::iterator vit;
			for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); vit++ )
			{
				if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
				{
					kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
				}
			}

			if( kPacketNot.m_vecInventorySlotInfo.empty() )
				return;

			//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
			kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
			//}}
			SendToGSField( ERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT, kPacketNot );
		}
		//}}
        else if( GetStateID() == KGSFSM::S_ROOM )
        {
            if( GetRoomUID() <= 0 )
            {
                START_LOG( cerr, L"룸 UID 이상." )
                    << BUILD_LOG( GetRoomUID() )
                    << BUILD_LOG( GetCharUID() )
                    << BUILD_LOG( GetCharName() )
                    << END_LOG;
                return;
            }

            switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
            {
            case CXSLRoom::RT_PVP:
            case CXSLRoom::RT_DUNGEON:
            case CXSLRoom::RT_TRAININGCENTER:
                {
                    KERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT kPacketNot;
					//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
					kPacketNot.m_cChangeEquippedPacketType = KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_EQUIP;
					//}}
                    kPacketNot.m_UnitUID = GetCharUID();
                    std::vector< KInventoryItemInfo >::iterator vit;
                    for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); vit++ )
                    {
                        if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
                        {
                            kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
                        }
                    }

                    if( kPacketNot.m_vecInventorySlotInfo.empty() )
                    {
                        return;
                    }

					//080421.hoons.
					//축복의 아이템이 지워져도 GetRoomUserInfo() 호출할때 업데이트가 되기 때문에 다른처리를
					//고려하지 않는다.
                    GetRoomUserInfo( kPacketNot.m_kRoomUserInfo );
                    SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT, kPacketNot );
                }
                break;

            case CXSLRoom::RT_SQUARE:
                {
                    KERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT kPacketNot;
                    kPacketNot.m_UnitUID = GetCharUID();
                    std::vector< KInventoryItemInfo >::iterator vit;
                    for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); vit++ )
                    {
                        if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
                        {
                            kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
                        }
                    }

                    if( kPacketNot.m_vecInventorySlotInfo.empty() )
                    {
                        return;
                    }

					//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
					kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
					//}}
                    SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT, kPacketNot );
                }
                break;

            default:
                {
                    START_LOG( cerr, L"이상한 방 타입." )
                        << BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
                        << BUILD_LOG( GetCharUID() )
                        << END_LOG;
                }
                break;
            }
        }
    }
}

void KGSUser::ExpireCashSkillPoint( bool bForce /*= false*/ )
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( ( m_kTimer[TM_CHECK_CASH_SKILL_POINT_EXPIRATION].elapsed() < 60.0 * 5.0 ) && !bForce )
			return;

		m_kTimer[TM_CHECK_CASH_SKILL_POINT_EXPIRATION].restart();

		if( m_kSkillTree.IsCashSkillPointExpired() == false )
		{
			CTime tCurrentTime = CTime::GetCurrentTime();
			if( tCurrentTime > m_kSkillTree.GetCSPointEndDate() )
			{
				// 배운 스킬 트리중에서 CSP로 배운 스킬 되돌리고, 사용된 CSP 포인트 계산하고 m_iMaxCSPoint를 빼서 SP로 돌려주고
				// 장착된 스킬중에 안배운 스킬이 있으면 탈착한다
				KDBE_EXPIRE_CASH_SKILL_POINT_REQ KDBReq;
				KDBReq.m_iUnitUID = GetCharUID();

				m_kSkillTree.CalcExpireCashSkillPointEveryPage( KDBReq.m_vecRetrievedSkillPageData );

				SendToGameDB( DBE_EXPIRE_CASH_SKILL_POINT_REQ, KDBReq );

				//{{ 2010. 07. 04  최육사	스킬 포인트 오류 확인
				if( CheckSkillPointError() == false )
				{
					if( GetAuthLevel() < SEnum::UAL_GM )
					{
						std::vector<int> vecRetrievedSPoint;
						UINT	uiSizeOfEveryUserSkillData = 0;
						int		iSumOfSPoint = 0;
						for ( UINT i = 0; i < KDBReq.m_vecRetrievedSkillPageData.size(); i++ )
						{
							iSumOfSPoint += KDBReq.m_vecRetrievedSkillPageData[i].m_iRetrievedSPoint;
							vecRetrievedSPoint.push_back( KDBReq.m_vecRetrievedSkillPageData[i].m_iRetrievedSPoint );
							uiSizeOfEveryUserSkillData += KDBReq.m_vecRetrievedSkillPageData[i].m_vecUserSkillData.size();
						}

						START_LOG( cerr, L"캐시 스킬 포인트 기간 만료 시점에 스킬 포인트 값이 비정상입니다." )
							<< BUILD_LOG( GetName() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOG( iSumOfSPoint )
							<< BUILD_LOG( uiSizeOfEveryUserSkillData )
							<< END_LOG;
					}
				}
				//}}
			}
		}
	}
}

void KGSUser::ExpireSkillSlotB( bool bForce /*= false*/ )
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( ( m_kTimer[TM_SKILL_SLOT_EXPIRATION].elapsed() < 60.0 * 5.0 ) && !bForce )
			return;

		m_kTimer[TM_SKILL_SLOT_EXPIRATION].restart();

		// 기간제 검사
		m_kSkillTree.ExpireSkillSlotB();
		SendID( EGS_EXPIRE_SKILL_SLOT_NOT );
	}
}

void KGSUser::ExpireTitle( bool bForce /*= false*/ )
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( ( m_kTimer[TM_CHECK_TITLE_EXPIRATION].elapsed() < 60.0 * 5.0 ) && !bForce )
			return;

		m_kTimer[TM_CHECK_TITLE_EXPIRATION].restart();

		// 기간제 검사
		m_kUserTitleManager.ExpireTitle( GetThisPtr<KGSUser>() );
	}
}

void KGSUser::CheckTimeEvent()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kTimer[TM_EVENT_CHECK].elapsed() < 60.0 )
			return;

		m_kTimer[TM_EVENT_CHECK].restart();

		std::set< int > setItemID;
		m_kInventory.GetItemIDSet( setItemID );

		// 접속 시간 이벤트 체크
		//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
		char cAPinkEventUserType	= 0;
		int	iAPinkScriptID			= -1;

		KGameEventManager::ConnectEventFactorSet kFactor(
			GetCharUID(),
			GetLevel(),
			IsPcBang(),
			GetChannelCode(),
			GetUnitType(),
			GetUnitClass(),
			false
			//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
			//}}
			//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
			//}}
			//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
			, GetUID()
			//}}
			, setItemID
			, IsPureSteamUser()
			);

		m_kUserEventManager.CheckConnectTimeEvent( GetThisPtr<KGSUser>(), kFactor );
		//}}

		//{{ 2009. 12. 7  최육사	누적시간이벤트
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		m_kUserEventManager.CheckCumulativeTimeEvent( GetThisPtr<KGSUser>() );
		//}}
		//}}

		//{{ 2010. 02. 06  최육사	일일 이벤트 퀘스트
		m_kUserQuestManager.CheckQuestEvent( GetThisPtr<KGSUser>() );
		//}}
		//{{ QUEST 개편 - 김민성
		m_kUserQuestManager.CheckResetDayEventQuest_AutoAccept( GetThisPtr<KGSUser>() );
		//}}
		//{{ 2013. 03. 21	 계정 퀘스트 - 요일별 활성화 기능 - 김민성
		m_kUserAccountQuestManager.CheckResetDayAccountQuest_AutoAccept( GetThisPtr<KGSUser>() );
		//}
		m_kUserEventManager.CheckAdvertisementEvent( GetThisPtr<KGSUser>() );

	}
}

//{{ 2009. 10. 14  최육사	자동결제
void KGSUser::CheckAutoPayment()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kTimer[TM_CHECK_AUTO_PAYMENT].elapsed() < 300.0 )
			return;

		m_kTimer[TM_CHECK_AUTO_PAYMENT].restart();

		if( m_tAutoPaymentCheckTime.GetTime() == 0 )
			return;

		// 부활석 갯수가 1개이상이면 체크하지 않습니다.
		if( m_iNumAutoPaymentResStone > 0 )
			return;

		CTime tCurTime = CTime::GetCurrentTime();
		if( m_tAutoPaymentCheckTime < tCurTime )
		{
			std::vector< unsigned long > vecAutoPaymentProductList;
			SiKNexonBillingTCPManager()->GetAutoPaymentProductList( vecAutoPaymentProductList );

			KENX_BT_CHECK_AUTO_PAYMENT_USER_REQ kPacketReq;
			kPacketReq.m_ulPacketNo		= SiKNexonBillingTCPManager()->GetNextPacketNo();
			kPacketReq.m_bytePacketType	= KNexonBillingTCPPacket::CHECK_AUTO_PAYMENT_USER;
			kPacketReq.m_wstrGameID		= GetName(); // 인벤토리 공유를 위해 Nexon ID를 쓴다.
			kPacketReq.m_wstrUserID		= GetName();

			if( KSimLayer::GetKObj()->GetBillingFlag() == KSimLayer::BF_NEXON_KOREA )
			{
				kPacketReq.m_ulProductNo = 67215; // 본섭 : 코보 서포트 서비스
			}
			else
			{
				kPacketReq.m_ulProductNo = 77697; // 테섭 : 코보 서포트 서비스
			}

			KEventPtr spEvent( new KEvent );
			UidType anTrace[2] = { GetUID(), -1 };
			spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_CHECK_AUTO_PAYMENT_USER_REQ, kPacketReq );
			SiKNexonBillingTCPManager()->QueueingEvent( spEvent );

			// 자동결제 체크 시각 지났으므로 다음날 6시로 다시 체크시각 설정!
			m_tAutoPaymentCheckTime += CTimeSpan( 1, 0, 0, 0 );
		}
	}
}
//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트

void KGSUser::CheckHackingToolList()
{
	if( m_kTimer[TM_HACKING_TOOL_LIST].elapsed() < m_kUserHackingManager.GetHackingToolCheckTime() )
		return;

	m_kTimer[TM_HACKING_TOOL_LIST].restart();

	KEGS_HACKING_TOOL_LIST_NOT kNot;
	m_kUserHackingManager.GetHackingToolList( kNot.m_iRandomKey, kNot.m_vecHackingToolList );
	SendPacket( EGS_HACKING_TOOL_LIST_NOT, kNot );
}
//}}

//{{ 2010. 7. 30 최육사	펫 시스템

void KGSUser::CheckAutoFeed()
{
	// 자동 먹이 기능 체크!
	KEGS_CHANGED_PET_SATIETY_NOT kNotSatiety;
	KEGS_CHANGED_PET_INTIMACY_NOT kNotIntimacy;
	KEGS_AUTO_FEED_NOT kAutoFeedPacket;
	bool bIsFirstFeed = false;

	//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
	int iAddIntimacy  = 0;
	if( m_kUserPetManager.CheckAutoFeed( GetThisPtr<KGSUser>(), bIsFirstFeed, kNotSatiety, kNotIntimacy, kAutoFeedPacket, iAddIntimacy ) == true )
	//}}
	{
		SendToCnRoom( ERM_CHANGED_PET_SATIETY_NOT, kNotSatiety );
		if( bIsFirstFeed )
		{
			SendToCnRoom( ERM_CHANGED_PET_INTIMACY_NOT, kNotIntimacy );
		}
		SendPacket( EGS_AUTO_FEED_NOT, kAutoFeedPacket );

		// 첫먹이라면 첫 먹이 패킷을 날리자!
		if( bIsFirstFeed == true )
		{
			KEGS_FIRST_FEED_PETS_NOT kPacketNot;
			kPacketNot.m_bAutoFeed = true;
			SendPacket( EGS_FIRST_FEED_PETS_NOT, kPacketNot );
		}

		// 나를 포함한 주변 유저에게 먹이 먹는 액션을 보내자!
		SendPetAction( CXSLPetManager::PAC_EAT );

		//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kAutoFeedPacket.m_iFeedItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"존재하지 않는 아이템입니다!" )
				<< BUILD_LOG( kAutoFeedPacket.m_iFeedItemID )
				<< END_LOG;

			return;
		}

		if( m_kUserTitleManager.GivePetFeed( pItemTemplet->m_ItemGrade, GetThisPtr<KGSUser>() ) == true )
		{
			// 완료된 칭호 미션이 있으면 새 미션이 있는지 검사한다
			m_kUserTitleManager.CheckNewMission( GetThisPtr<KGSUser>(), true );
		}
		//}}
	}
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
void KGSUser::CheckUpdateWorldMission()
{
	//	틱카운트가 변하거나 60초마다 한번씩 업데이트
	if( SiKGSWorldMissionManager()->GetTickCount() == m_iWorldMissionTickCount )
		return;

	m_iWorldMissionTickCount = SiKGSWorldMissionManager()->GetTickCount();

	//	정보받기
	KEGS_WORLD_MISSION_UPDATE_NOT kNot;

	SiKGSWorldMissionManager()->GetMissionInfo( kNot );

	//{{ 버프 관리 시스템 - 김민성
	CTime tCurr = CTime::GetCurrentTime();

	KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kBuffReq;
	kBuffReq.m_iUnitUID = GetCharUID();
	int iWorldBuff = SiKGSWorldMissionManager()->GetWorldBuffState();
	int iWorldBuffPriod = SiKGSWorldMissionManager()->GetWorldBuffEndTime();

	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	//}}

	if(  kBuffReq.m_vecActivateBuff.empty() == false || kBuffReq.m_vecDeActivateBuff.empty() == false )
	{
		SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kBuffReq );
	}
	//}

	//	유저에게 패킷날리기
	SendPacket( EGS_WORLD_MISSION_UPDATE_NOT, kNot );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

bool KGSUser::AutoFeedInPetInventory( IN bool bOnlyEatSpecial, OUT int& iFeedItemID, OUT std::vector< KInventoryItemInfo >& vecPetInventorySlotInfo )
{
	return m_kInventory.AutoFeedInPetInventory( GetLevel(), bOnlyEatSpecial, iFeedItemID, vecPetInventorySlotInfo );
}

//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
int KGSUser::CanIUseTheAutoLootingItem( IN const UidType iPetUID )
{
	KUserPetPtr spPet = m_kUserPetManager.GetPet( iPetUID );
	if( spPet == NULL )
	{
		START_LOG( cerr, L"보유 하고 있지 않는 PetUID입니다." )
			<< BUILD_LOG( iPetUID )
			<< END_LOG;
		return NetError::ERR_PET_26;
	}

	// 2. 소환된 펫은 생명의 결정 상태인가?
	if( spPet->GetEvolutionStep() == 0 )
	{
		if( SiCXSLPetManager()->IsEvolutionExceptionPet( spPet->GetPetID() ) == false )
		{
			return NetError::ERR_PET_27;
		}
	}


	// 3. 소환된 펫이 이미 오토 루팅 기능을 사용중이진 않은가?
	if( spPet->IsAutoLootingPet() == true )
	{
		return NetError::ERR_PET_28;
	}


	return NetError::NET_OK;
}

void KGSUser::SetAutoLootingPet( IN const UidType iPetUID )
{
	KUserPetPtr spPet = m_kUserPetManager.GetPet( iPetUID );
	if( spPet == NULL )
	{
		START_LOG( cerr, L"보유 하고 있지 않는 PetUID입니다." )
			<< BUILD_LOG( iPetUID )
			<< END_LOG;
		return;
	}
	spPet->SetAutoLootingPet();
}
//}}

int KGSUser::CanIUseTransformItem( IN const UidType iPetUID )
{
	KUserPetPtr spPet = m_kUserPetManager.GetPet( iPetUID );
	if( spPet == NULL )
	{
		START_LOG( cerr, L"보유 하고 있지 않는 PetUID입니다." )
			<< BUILD_LOG( iPetUID )
			<< END_LOG;
		return NetError::ERR_PET_26;
	}

	// 1. 소환된 펫은 변신 물약 사용할 수 있는 펫인가?
	if( spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_TANYA && spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LAEL &&
		spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA && spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA &&
		spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_TANYA_F && spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LAEL_F &&
		spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA_F && spPet->GetPetID() != CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA_F
		)
	{
		START_LOG( cerr, L"변신 물약을 사용할수 있는 펫이 아닙니다." )
			<< BUILD_LOG( iPetUID )
			<< END_LOG;
		return NetError::ERR_HW_PET_02;
	}

	// 2. 소환된 펫은 생명의 완전체 상태인가?
	if( spPet->GetEvolutionStep() != 3 )
	{
		if( SiCXSLPetManager()->IsEvolutionExceptionPet( spPet->GetPetID() ) == false )
		{
			return NetError::ERR_HW_PET_02;
		}
	}

	// 3. 소환된 펫이 친밀도 100% 인가?
	if( ( static_cast<float>(spPet->GetIntimacy()) / static_cast<float>(spPet->GetMaxIntimacy()) * 100.f ) < 100.f  )
	{
		return NetError::ERR_HW_PET_01;
	}

	return NetError::NET_OK;
}

void KGSUser::UseTransformItem( IN int iUsedItemID, IN const UidType iPetUID, OUT int& iBeforePetID, OUT int& iAfterPetID )
{
	KUserPetPtr spPet = m_kUserPetManager.GetPet( iPetUID );
	if( spPet == NULL )
	{
		START_LOG( cerr, L"보유 하고 있지 않는 PetUID입니다." )
			<< BUILD_LOG( iPetUID )
			<< END_LOG;
		return;
	}

	iBeforePetID = spPet->GetPetID();

	int iTempBeforePetID = iBeforePetID;

	if( iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_TANYA_F || iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LAEL_F ||
		iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA_F || iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA_F )
	{
		iTempBeforePetID -= 5;
	}

	iAfterPetID = SiCXSLPetManager()->GetTransformPetID( iUsedItemID, iTempBeforePetID );

	if( iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_TANYA_F || iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LAEL_F ||
		iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA_F || iBeforePetID == CXSLPetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA_F )
	{
		iAfterPetID += 5;
	}

	spPet->ChangePetID( iAfterPetID );

	// 친밀도를 0으로 초기화 하자!
	if( spPet->GetIntimacy() > 0 )
	{
		const int iCurIntimacy = spPet->DecreaseIntimacy( spPet->GetIntimacy() );
		LIF( iCurIntimacy == 0 );
	}
}

//}}

//{{ 2010. 07. 04  최육사	스킬 포인트 오류 확인


bool KGSUser::CheckSkillPointError() const
{
	if( GetCharUID() == 0 )
		return false;

	// fix!! 임시로 내 SP 수치에 오류가 있지 않은지 로그를 남긴다
	std::vector<int> vecUsedSPoint;
	std::vector<int> vecUsedCSPoint;

	m_kSkillTree.CalcUsedSPointAndCSPoint( vecUsedSPoint, vecUsedCSPoint );

	int iRetrievedSPoint = 0;
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), iRetrievedSPoint );

		const int iCheckSP = iRetrievedSPoint + m_kSkillTree.GetMaxCSPoint();

	const std::vector<KUserSkillTree::SkillPageData>& vecSkillPageData
		= m_kSkillTree.GetSkillPageDataVector();

	if ( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		for ( UINT i = 0; i < vecUsedSPoint.size() && i < vecUsedCSPoint.size(); i++ )
		{
			const int iCurSP = vecSkillPageData[i].m_iSPoint
				+ vecUsedSPoint[i] + vecUsedCSPoint[i] + vecSkillPageData[i].GetCSPoint();

			if ( iCheckSP != iCurSP )
			{
				//{{ 2009. 6. 16  최육사	일반유저일 경우만 에러찍음
				if( GetAuthLevel() < SEnum::UAL_GM )
				{
					START_LOG( cerr, L"내가 가지고 있는 SP 보유량에 문제가 있을 수 있습니다.!" )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( GetCharName() )
						<< BUILD_LOGc( GetLevel() )
						<< BUILD_LOG( iCheckSP )
						<< BUILD_LOG( iCurSP )
						<< BUILD_LOG( vecSkillPageData[i].m_iSPoint )
						<< BUILD_LOG( vecUsedSPoint[i] )
						<< BUILD_LOG( vecUsedCSPoint[i] )
						<< BUILD_LOG( vecSkillPageData[i].GetCSPoint() )
						<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
						<< END_LOG;
				}
				else
				{
					// 운영자급 이상은 테스트 목적으로 SP를 변경할 수 있기 때문에 에러를 찍지 않습니다.
					START_LOG( cwarn, L"내가 가지고 있는 SP 보유량에 문제가 있을 수 있습니다.!" )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( GetCharName() )
						<< BUILD_LOGc( GetLevel() )
						<< BUILD_LOG( iCheckSP )
						<< BUILD_LOG( iCurSP )
						<< BUILD_LOG( vecSkillPageData[i].m_iSPoint )
						<< BUILD_LOG( vecUsedSPoint[i] )
						<< BUILD_LOG( vecUsedCSPoint[i] )
						<< BUILD_LOG( vecSkillPageData[i].GetCSPoint() )
						<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
						<< END_LOG;
				}
				//}}
				return false;
			}
		}
	}
	else
	{
		for ( UINT i = 0; i < vecUsedSPoint.size() && i < vecUsedCSPoint.size(); i++ )
		{
			if( vecUsedCSPoint[i] != 0  ||  vecSkillPageData[i].GetCSPoint() != 0 )
			{
				START_LOG( cerr, L"CSP 사용기간이 만료되었는데 CSP가 남아있다" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOGc( GetLevel() )
					<< BUILD_LOG( vecSkillPageData[i].m_iSPoint )
					<< BUILD_LOG( vecUsedSPoint[i] )
					<< BUILD_LOG( vecUsedCSPoint[i] )
					<< BUILD_LOG( vecSkillPageData[i].GetCSPoint() )
					<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
					<< END_LOG;
				return false;
			}
			else
			{
				const int iCurSP = vecSkillPageData[i].m_iSPoint + vecUsedSPoint[i];

				if ( iCheckSP != iCurSP )
				{
					//{{ 2009. 6. 16  최육사	일반유저일 경우만 에러찍음
					if( GetAuthLevel() < SEnum::UAL_GM )
					{
						START_LOG( cerr, L"내가 가지고 있는 SP 보유량에 문제가 있을 수 있습니다.!!" )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOGc( GetLevel() )
							<< BUILD_LOG( iCheckSP )
							<< BUILD_LOG( iCurSP )
							<< BUILD_LOG( vecSkillPageData[i].m_iSPoint )
							<< BUILD_LOG( vecUsedSPoint[i] )
							<< BUILD_LOG( vecUsedCSPoint[i] )
							<< BUILD_LOG( vecSkillPageData[i].GetCSPoint() )
							<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
							<< END_LOG;
					}
					else
					{
						// 운영자급 이상은 테스트 목적으로 SP를 변경할 수 있기 때문에 에러를 찍지 않습니다.
						START_LOG( cwarn, L"내가 가지고 있는 SP 보유량에 문제가 있을 수 있습니다.!!" )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOGc( GetLevel() )
							<< BUILD_LOG( iCheckSP )
							<< BUILD_LOG( iCurSP )
							<< BUILD_LOG( vecSkillPageData[i].m_iSPoint )
							<< BUILD_LOG( vecUsedSPoint[i] )
							<< BUILD_LOG( vecUsedCSPoint[i] )
							<< BUILD_LOG( vecSkillPageData[i].GetCSPoint() )
							<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
							<< END_LOG;
					}
					//}}
					return false;
				}
			}
		}
	}

	return true;
}


//}}

//{{ 2011. 12. 22	최육사	배틀필드 시스템
void KGSUser::CheckWaitPartyUser()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		// 파티 초대 시간 체크
		m_kUserPartyManager.CheckWaitPartyUser();
	}
}
//}}

//{{ 2011. 06. 17	최육사	패킷 이벤트 보안
bool KGSUser::CheckIsValidPacketEvent( IN const KEventPtr& spEvent_, IN const wchar_t* pszPacketStructName, IN const bool bIsDeserializeSuccess, IN const bool bIsValidPacketID )
{
	if( bIsDeserializeSuccess == false )
	{
		START_LOG( cerr, L"deserialze failed." << spEvent_->GetIDStr() << " - " << pszPacketStructName )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetSessionType() )
			<< BUILD_LOG( spEvent_->GetFromType() )
			<< END_LOG;

		m_kUserHackingManager.IncreaseDeserializeFailCount( spEvent_->m_usEventID );
		return false;
	}

	if( bIsValidPacketID == false )
	{
		START_LOG( cerr, L"invalid packet id." << spEvent_->GetIDStr() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetSessionType() )
			<< BUILD_LOG( spEvent_->GetFromType() )
			<< END_LOG;
		return false;
	}

	return true;
}
//}}

void KGSUser::Kick( IN const int iNetError )
{
	KStatistics::ESIColumn_Disconnect_Reason eDisconnectReason = KStatistics::eSIColDR_Unknown;

	switch( iNetError )
	{
	case NetError::ERR_VERIFY_01:
		eDisconnectReason = KStatistics::eSIColDR_Diff_Server_Connect;
		break;

	case NetError::ERR_USER_REGISTRATION_00:
		eDisconnectReason = KStatistics::eSIColDR_Diff_Server_Connect;
		m_bReportDisconnectToLogin = false;
		break;

	case NetError::ERR_ADMIN_COMMAND_07:
		eDisconnectReason = KStatistics::eSIColDR_GM_Kick_User;
		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		{
			KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
			kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_KICK_BY_GM;
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		}
		//}}
		//}}
		break;

	case NetError::ERR_USER_REGISTRATION_01:
	case NetError::ERR_USER_REGISTRATION_02:
	case NetError::ERR_USER_REGISTRATION_03:
	case NetError::ERR_USER_REGISTRATION_04:
	case NetError::ERR_USER_REGISTRATION_05:
	case NetError::ERR_USER_REGISTRATION_06:
	case NetError::ERR_USER_REGISTRATION_07:
	case NetError::ERR_USER_REGISTRATION_08:
	case NetError::ERR_USER_REGISTRATION_09:
		eDisconnectReason = KStatistics::eSIColDR_User_Reg_Error;
		break;

		//{{ 2009. 3. 5  최육사		채널이동
	case NetError::ERR_CHANNEL_CHANGE_03:
		// 채널이동 성공으로 인한 접속종료
		eDisconnectReason = KStatistics::eSIColDR_Channel_Change;
		break;
		//}}
		//{{ 2010. 06. 22  최육사	계정블럭기능
	case NetError::ERR_ACCOUNT_BLOCK_00:
		eDisconnectReason = KStatistics::eSIColDR_GameServer_Account_Block;


		break;
		//}}
		//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	case NetError::ERR_CHECK_MACHINE_ID_00:
		eDisconnectReason = KStatistics::eSIColDR_MachineID_Block;
		break;
		//}}

		//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	case NetError::ERR_USER_REGISTRATION_10:
		eDisconnectReason = KStatistics::eSIColDR_Same_MachineID_Connect;
		break;
		//}}

	default:
		START_LOG( cerr, L"Kick Reason값이 이상합니다." )
			<< BUILD_LOG( iNetError )
			<< END_LOG;
		break;
	}

	Kick( eDisconnectReason );
}

void KGSUser::Kick( IN const KStatistics::ESIColumn_Disconnect_Reason eDisconnectReason )
{
	SetDisconnectReason( eDisconnectReason );
	ReserveDestroy();
}

bool KGSUser::SendID( unsigned short usEventID )
{
	return KSession::SendID( PI_GS_PROXY, GetUID(), NULL, usEventID );
}

void KGSUser::SendToAccountDB( unsigned short usEventID )
{
    SendToAccountDB( usEventID, char() );
}

void KGSUser::SendToGameDB( unsigned short usEventID )
{
    SendToGameDB( usEventID, char() );
}

void KGSUser::SendToLogDB( unsigned short usEventID )
{
    SendToLogDB( usEventID, char() );
}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//}}

void KGSUser::SendToKOGBillingDB( unsigned short usEventID )
{
	SendToKOGBillingDB( usEventID, char() );
}

void KGSUser::SendToCnUser( unsigned short usEventID )
{
    SendToCnUser( usEventID, char() );
}

//{{ 2009. 4. 24  최육사	대전유저리스트
void KGSUser::SendToCnServer( unsigned short usEventID )
{
	SendToCnServer( usEventID, char() );
}
//}}

void KGSUser::SendToLoginAccountDB( unsigned short usEventID )
{
    SendToLoginAccountDB( usEventID, char() );
}

void KGSUser::SendToCnRoom( unsigned short usEventID )
{
    SendToCnRoom( usEventID, char() );
}

//{{ 2011. 04. 29	최육사	대리상인

void KGSUser::SendToLoginRoom( unsigned short usEventID )
{
	SendToLoginRoom( usEventID, char() );
}

void KGSUser::SendToPShopRoom( unsigned short usEventID )
{
	SendToPShopRoom( usEventID, char() );
}

//}}

//{{ 2010. 02. 20  최육사	채널 파티 통합
void KGSUser::SendToCnParty( unsigned short usEventID )
{
	SendToCnParty( usEventID, char() );
}
//}}

void KGSUser::SendToGSField( unsigned short usEventID )
{
	SendToGSField( usEventID, char() );
}

void KGSUser::SendToLoginServer( unsigned short usEventID )
{
	SendToLoginServer( usEventID, char() );
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
void KGSUser::SendToGlobalServer( unsigned short usEventID )
{
	SendToGlobalServer( usEventID, char() );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

void KGSUser::SetUnitInfo( const KUnitInfo& kInfo )
{
	//{{ 2009. 4. 2  최육사		캐릭터초기화 검사
	if( GetCharUID() > 0 )
	{
		GetKUserManager()->UnregCharInfo( *this );

		START_LOG( cerr, L"캐릭터 선택하려는데 기존 캐릭터 정보가 초기화 되어있지 않다?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kInfo.m_nUnitUID )
			<< END_LOG;
	}
	//}}

    // User UID 는 이전에 세팅 되었다.
    m_nUnitUID              = kInfo.m_nUnitUID;
	m_uiKNMSerialNum		= kInfo.m_uiKNMSerialNum;
    m_cUnitClass            = kInfo.m_cUnitClass;
    m_wstrNickName          = kInfo.m_wstrNickName;
	m_wstrIP			    = kInfo.m_wstrIP;
	//m_nPort				= kInfo.m_nPort;
	m_ucLevel				= kInfo.m_ucLevel;

	//{{ 2011. 09. 27	김민성	PC 방 오과금 방지
	//}

	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	m_iED.SetValue( kInfo.m_iED );
	//}}
	m_kEXP.SetExp( kInfo.m_iEXP );
	//{{ 2011. 06. 20	최육사	대전 개편

	m_kUserPvpManager.Init( kInfo.m_iOfficialMatchCnt,
							kInfo.m_iRating,
							kInfo.m_iMaxRating,
							kInfo.m_iRPoint,
							kInfo.m_iAPoint,
							kInfo.m_bIsWinBeforeMatch,
							kInfo.m_iWin,
							kInfo.m_iLose
							//{{ 2012. 06. 20	박세훈	2012 대전 시즌2
							, kInfo.m_cRank
							//{{ 2012. 06. 25	박세훈	2012 대전 시즌2
							, kInfo.m_fKFactor
							, kInfo.m_bIsRedistributionUser
							, kInfo.m_iPastSeasonWin
							, GetCharUID()
							//}}
							);
	//}}


// 	m_kSkillTree.SetSPoint(	kInfo.m_iSPoint );
// 	m_kSkillTree.SetCSPoint( m_kSkillTree.GetActiveSkillPageNumber(), kInfo.m_iCSPoint );

	m_kSkillTree.SetMaxCSPoint( kInfo.m_iMaxCSPoint );

	if( true == kInfo.m_wstrCSPointEndDate.empty() )
	{
		// 과거 시간으로 세팅
		m_kSkillTree.SetCSPointEndDate( L"2000-01-01 00:00:00" );
	}
	else if( false == m_kSkillTree.SetCSPointEndDate( kInfo.m_wstrCSPointEndDate ) )
	{
		START_LOG( cerr, L"문자열 시간 변환 실패." )
			<< BUILD_LOG( kInfo.m_wstrCSPointEndDate )
			<< END_LOG;

		// 과거 시간으로 세팅
		m_kSkillTree.SetCSPointEndDate( L"2000-01-01 00:00:00" );
	}

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	m_kUserSpiritManager.Init( kInfo.m_iSpirit, kInfo.m_iSpiritMax );
	//}}

	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

    m_nStraightVictories    = kInfo.m_nStraightVictories;
    m_usInventorySize       = 0;

	m_kStat					= kInfo.m_kStat;

	//{{ 2012. 02. 02	최육사	배틀필드 시스템
	SetLastPosInfo( kInfo.m_kLastPos );
	//}}
	//{{ 2011. 06. 20	최육사	대전 개편
	//}}
    m_mapDungeonClear       = kInfo.m_mapDungeonClear;
	m_mapTCClear			= kInfo.m_mapTCClear;
	m_mapDungeonPlay       = kInfo.m_mapDungeonPlay;

	//{{ 2008. 10. 7  최육사	타이틀
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	m_kUserTitleManager.SetEquippedTitleID( kInfo.m_iTitleID );
	//}}
	//}}

	//{{ 2009. 9. 25  최육사	길드
	m_kUserGuildManager.SetGuildInfo( kInfo.m_kUserGuildInfo );
	//}}


    GetKUserManager()->RegCharInfo( *this );
    m_kTimer[TM_DB_UPDATE].restart();

	m_trWarpVipEndDate = kInfo.m_trWarpVipEndData;


	m_ucOldYearMissionRewardedLevel = kInfo.m_ucOldYearMissionRewardedLevel;
	m_iNewYearMissionStepID = kInfo.m_iNewYearMissionStepID;

}

void KGSUser::GetUnitInfo( KUnitInfo& kInfo )
{
    kInfo.m_iOwnerUserUID       = GetUID();
    kInfo.m_cAuthLevel          = static_cast<char>(GetAuthLevel());
    kInfo.m_nUnitUID            = m_nUnitUID;
	kInfo.m_uiKNMSerialNum		= m_uiKNMSerialNum;
    kInfo.m_cUnitClass          = GetUnitClass();
	//{{ 2011. 06. 20	최육사	대전 개편
	//}}
    kInfo.m_wstrNickName        = m_wstrNickName;
    kInfo.m_wstrIP              = m_wstrIP;
    kInfo.m_usPort				= static_cast<USHORT>(m_nPort);
    kInfo.m_iED                 = GetED();
    kInfo.m_ucLevel             = GetLevel();
    kInfo.m_nNowBaseLevelEXP    = SiKExpTable()->GetRequireTotalExpbyLevel( static_cast<int>(GetLevel()) );
    kInfo.m_nNextBaseLevelEXP   = SiKExpTable()->GetRequireTotalExpbyLevel( static_cast<int>(GetLevel() + 1) );
	kInfo.m_iEXP                = m_kEXP.GetExp();
	//{{ 2011. 06. 20	최육사	대전 개편
	kInfo.m_iOfficialMatchCnt	= m_kUserPvpManager.GetOfficialMatchCount();
	kInfo.m_iRating				= GetRating();
	kInfo.m_iMaxRating			= m_kUserPvpManager.GetMaxRating();
	kInfo.m_iRPoint				= GetRPoint();
	kInfo.m_iAPoint				= GetAPoint();
	kInfo.m_bIsWinBeforeMatch	= m_kUserPvpManager.IsWinBeforeMatch();

	//{{ 2012. 06. 25	박세훈	2012 대전 시즌2
	kInfo.m_cRank				= GetPvpRankForClient();
	kInfo.m_fKFactor			= m_kUserPvpManager.GetKFactor();
	kInfo.m_iPastSeasonWin		= m_kUserPvpManager.GetPastSeasonWin();
	//}}

	//}}
    kInfo.m_nStraightVictories  = m_nStraightVictories;
	kInfo.m_kStat				= m_kStat;

	kInfo.m_iSPoint				= m_kSkillTree.GetSPoint();

	//{{ 2012. 02. 02	최육사	배틀필드 시스템
	kInfo.m_kLastPos			= GetLastPosInfo();
	//}}
	//{{ 2011. 06. 20	최육사	대전 개편
	kInfo.m_iWin				= GetWin();
	kInfo.m_iLose				= GetLose();
	//}}
    kInfo.m_mapDungeonClear     = m_mapDungeonClear;
	kInfo.m_mapTCClear			= m_mapTCClear;
	kInfo.m_mapDungeonPlay     = m_mapDungeonPlay;
	//m_kUserQuestManager.GetQuestClearInfo( kInfo.m_mapQuestClear );
	m_kInventory.GetEquippedItem( kInfo.m_mapEquippedItem );

	//////////////////////////////////////////////////////////////////////////
	// 스킬개편
	kInfo.m_iCSPoint			= m_kSkillTree.GetCSPoint();
	kInfo.m_iMaxCSPoint			= m_kSkillTree.GetMaxCSPoint();
	kInfo.m_wstrCSPointEndDate	= m_kSkillTree.GetCSPointEndDateString();


	LIF( KUserSkillTree::MAX_SKILL_SLOT == 8 );
	KSkillData aSkillData[ KUserSkillTree::MAX_SKILL_SLOT];
	m_kSkillTree.GetSkillSlot( aSkillData );

	kInfo.m_UnitSkillData.m_aEquippedSkill[0] = aSkillData[KUserSkillTree::SKILL_SLOT_A1];
	kInfo.m_UnitSkillData.m_aEquippedSkill[1] = aSkillData[KUserSkillTree::SKILL_SLOT_A2];
	kInfo.m_UnitSkillData.m_aEquippedSkill[2] = aSkillData[KUserSkillTree::SKILL_SLOT_A3];
	kInfo.m_UnitSkillData.m_aEquippedSkill[3] = aSkillData[KUserSkillTree::SKILL_SLOT_A4];

	kInfo.m_UnitSkillData.m_aEquippedSkillSlotB[0] = aSkillData[KUserSkillTree::SKILL_SLOT_B1];
	kInfo.m_UnitSkillData.m_aEquippedSkillSlotB[1] = aSkillData[KUserSkillTree::SKILL_SLOT_B2];
	kInfo.m_UnitSkillData.m_aEquippedSkillSlotB[2] = aSkillData[KUserSkillTree::SKILL_SLOT_B3];
	kInfo.m_UnitSkillData.m_aEquippedSkillSlotB[3] = aSkillData[KUserSkillTree::SKILL_SLOT_B4];

	kInfo.m_UnitSkillData.m_cSkillSlotBExpirationState = (char) m_kSkillTree.GetSkillSlotBExpirationState();
	kInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate = m_kSkillTree.GetSkillSlotBEndDateString();

	kInfo.m_UnitSkillData.m_nActiveSkillPagesNumber			= m_kSkillTree.GetActiveSkillPageNumber();
	kInfo.m_UnitSkillData.m_nTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
	if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
		m_kSkillTree.GetPassiveSkillData( kInfo.m_UnitSkillData.m_vecPassiveSkill, m_kSkillTree.AccessLearnedSkillTree() );
	else
		AddLogWhenSkillPagesNumberIsWrong( L"GetUnitInfo" );

	//{{ 2010. 04. 08  최육사	기술의 노트
	m_kSkillTree.GetSkillNote( kInfo.m_UnitSkillData.m_vecSkillNote );
	//}}

	//////////////////////////////////////////////////////////////////////////

	kInfo.m_bIsParty = ( GetPartyUID() > 0 );

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	kInfo.m_iSpiritMax			= m_kUserSpiritManager.GetSpiritMax();
	kInfo.m_iSpirit				= m_kUserSpiritManager.GetSpirit();
	//}}
	//{{ 2010. 01. 29  최육사	PC방 상용화
	kInfo.m_bIsGameBang         = IsPcBang();
	//}}
	kInfo.m_iPcBangType			= GetPcBangType();

	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	kInfo.m_iTitleID			= m_kUserTitleManager.GetEquippedTitleID();
	//}}

	//{{ 2009. 9. 25  최육사	길드
	kInfo.m_kUserGuildInfo.m_iGuildUID			= GetGuildUID();
	kInfo.m_kUserGuildInfo.m_wstrGuildName		= m_kUserGuildManager.GetGuildName();
	kInfo.m_kUserGuildInfo.m_ucMemberShipGrade	= m_kUserGuildManager.GetMemberShipGrade();
	kInfo.m_kUserGuildInfo.m_iHonorPoint		= m_kUserGuildManager.GetHonorPoint();
	//}}

	kInfo.m_trWarpVipEndData = m_trWarpVipEndDate;



	kInfo.m_kGameStat.Init();

	kInfo.m_ucOldYearMissionRewardedLevel = m_ucOldYearMissionRewardedLevel;
	kInfo.m_iNewYearMissionStepID = m_iNewYearMissionStepID;

}

void KGSUser::ResetUnitInfo()
{
    m_nUnitUID          = 0;
    m_cUnitClass        = 0;
    m_wstrNickName      = std::wstring();
	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	m_iED.SetValue( 0 );
	//}}
    m_ucLevel           = 0;
	m_kEXP.SetExp( 0 );
	//{{ 2011. 06. 20	최육사	대전 개편
	m_kUserPvpManager.Clear();
	//}}

	m_kSkillTree.SetSPointEveryPage( 0 );
	m_kSkillTree.SetCSPointEveryPage( 0 );

	m_kSkillTree.SetMaxCSPoint( 0 );

    m_nStraightVictories = 0;
    m_usInventorySize   = 0;
    m_ucQuickSlotSize   = 0;

	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	m_kUserSpiritManager.Clear();
	//}}

	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

	m_iNumResurrectionStone.SetValue( 0 );
	//{{ 2009. 10. 14  최육사	자동결제 부활석
	m_iNumAutoPaymentResStone.SetValue( 0 );
	m_tAutoPaymentCheckTime = CTime();
	//}}

	m_dResolveJackpotTime = 30000.f; // 임시 최대값

	m_kStat.Init();

	//{{ 2012. 02. 02	최육사	배틀필드 시스템
	m_kLastPos.Clear();
	SetMapID( SEnum::VMI_RUBEN );
	//}}
    m_mapDungeonClear.clear();
	m_mapTCClear.clear();
	m_mapDungeonPlay.clear();

	//{{ 2010. 01. 08  최육사	추천인리스트
	m_kUserRecommendManager.Clear();
	//}}
	//{{ 2012. 03. 21	최육사	배틀필드 시스템
	m_iBeforeRoomUID = 0;
	//}}

	//{{ 2011. 12. 20	최육사	배틀필드 시스템
	m_kUserPartyManager.Clear();
	//}}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	m_kUserAbuserManager.Clear( true );
	//}}

	//{{ 2009. 10. 26  최육사	PC방이벤트
	m_kUserEventManager.Clear();
	//}}
	m_kUserCashInventory.Clear();	// 유저 캐쉬 정보
	m_kUserLetterBox.Clear();		// 유저 우편함
	m_kUserTitleManager.Clear();	// 유저 타이틀 매니저
	m_mapFriendList.clear();		// 친구정보
	m_kUserRankingManager.clear();	// 랭킹 정보
    m_kMessengerInfo.Clear();		// 메신저
	m_kUserStatistics.Clear();		// 캐릭터 통계

	//{{ 2009. 9. 22  최육사	길드
	m_kUserGuildManager.Clear();
	//}}
	//{{ 2010. 07. 21  최육사	펫 시스템
	m_kUserPetManager.Clear();
	//}}
	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	m_kDungeonRequired.Clear();
	//}}
	//{{ 2011. 04. 11	최육사	대리 상인
	m_kUserPersonalShopManager.Clear();
	//}}
	//{{ 2012. 02. 21	최육사	배틀필드 시스템
	m_kUserUnitManager.Clear();
	//}}

	m_kInventory.ClearShareBank();

	//{{ 2012. 04. 21	박세훈	어둠의 문 개편
	m_kUserBuffManager.Clear();
	//}}

	m_bInvisible = false;

	m_kUserRidingPetManager.Clear();



	m_iRidingPetSummon = 0;
	m_usRidingSummonedPetID = 0;

	m_bCouple = false;
	m_iRelationTargetUserUid = 0;
	m_wstrRelationTargetUserNickname = L"";



	m_ucOldYearMissionRewardedLevel = 0;
	m_iNewYearMissionStepID = -1;


	m_kBossFieldJoinInfo.clear();
}

bool KGSUser::CheckExceedRefCount( int nCount )
{
    if( KActor::CheckExceedRefCount( nCount ) )
        return true;

    START_LOG( cerr, L"Invalid Ref-count." )
        << BUILD_LOG( GetRefCount() )
        << BUILD_LOG( GetName() )
        << BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetRoomListID() )
		<< BUILD_LOG( GetPartyUID() )
        << BUILD_LOG( GetStateID() )
        << BUILD_LOG( GetStateIDString() )
		<< END_LOG;

    return false;
}

//{{ 2009. 5. 29  최육사	함수 통합
bool KGSUser::OnConnectReq( IN const KEGS_CONNECT_REQ& kReq, OUT KEGS_CONNECT_ACK& kAck )
{
	if( kReq.m_wstrVersion.compare( KBaseServer::GetKObj()->GetMainVersion() ) != 0 )
	{
		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( GetIPStr() )
			<< BUILD_LOG( kReq.m_wstrVersion )
			<< BUILD_LOG( KBaseServer::GetKObj()->GetMainVersion() );

		kAck.m_iOK	= NetError::ERR_CONNECT_00;
		//SendPacket( usAckEventID, kAck );

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_GAME_VERSION;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}
		//}}

		SetDisconnectReason( KStatistics::eSIColDR_Diff_Main_Ver );
		ReserveDestroy();
		return false;
	}

	// 동접 제한 검사
	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int iMaxUser = GetKGameServer()->GetMaxUser();
	//}}
	if( iMaxUser <= 0 )
	{
		START_LOG( cerr, L"최대 동접 이상." )
			<< BUILD_LOG( iMaxUser )
			<< END_LOG;

		iMaxUser = 1;
	}

	// 동접 인원 제한에 걸리면 채널에 입장하지 못하도록 처리
	int iNowUser = KActorManager::GetKObj()->GetCountNoLock() + SiKChannelManager()->GetChannelChangeWaitingUserCount();

	// 자기 자신은 빼고 계산해야함.
	if( ( iNowUser - 1 ) >= iMaxUser )
	{
		kAck.m_iOK   = NetError::ERR_CONNECT_04;
		//SendPacket( usAckEventID, kAck );

		SetDisconnectReason( KStatistics::eSIColDR_Limit_User_Num );
		ReserveDestroy();
		return false;
	}

	kAck.m_iOK		 = NetError::NET_OK;
	kAck.m_UDPPort	 = static_cast< int >( KNetLayer::GetKObj()->GetNCUDPPort() );
	//{{ 2009. 4. 17  최육사	채널정보
	kAck.m_iChannelID = SiKChannelManager()->GetChannelID();
	//}}
	//SendPacket( usAckEventID, kAck );
	return true;
}

bool KGSUser::OnNexonUserAuthenticateAck( IN OUT KELG_NEXON_USER_AUTHENTICATE_ACK& kAck_, OUT KEGS_VERIFY_ACCOUNT_ACK& kAck )
{
	//유저정보 받아오기 실패면 종료..
	if( kAck_.m_iOK != NetError::NET_OK )
	{
		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( kAck_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kAck_.m_iOK ) )
			<< BUILD_LOG( kAck_.m_kAccountInfo.m_wstrID )
			<< END_LOG;

		kAck.m_iOK = kAck_.m_iOK;
		kAck.m_kAccountInfo = kAck_.m_kAccountInfo;

		//SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );

		//{{ 2010. 06. 22  최육사	계정블럭기능
		if( kAck_.m_iOK == NetError::ERR_ACCOUNT_BLOCK_00 )
		{
			// 클라이언트에게는 정상처리 하기 위해 OK값 보냄
			kAck.m_iOK = NetError::NET_OK;

			// 계정 블럭으로 종료 사유 저장
			SetDisconnectReason( KStatistics::eSIColDR_GameServer_Account_Block );
		}
		else
		//}}
		{
			SetDisconnectReason( KStatistics::eSIColDR_Nexon_Auth_Fail );
		}
		ReserveDestroy();
		return false;
	}


	// 동일서버 내 이중접속 여부 확인
	KActorPtr spActor( KActorManager::GetKObj()->Get( kAck_.m_kAccountInfo.m_nUserUID ) );
	if( spActor )
	{
		START_LOG( cerr, L"동일서버간 이중접속 감지" )
			<< BUILD_LOG( m_strName )
			<< BUILD_LOG( m_nUID )
			<< BUILD_LOG( kAck_.m_kAccountInfo.m_nUserUID )
			<< BUILD_LOG( kAck_.m_kAccountInfo.m_wstrID )
			<< BUILD_LOG( kAck_.m_kAccountInfo.m_wstrName );

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_OVERLAP_ACCOUNT;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
		//}}
		//}}

		kAck.m_iOK = NetError::ERR_VERIFY_00;    // 동일서버간 이중접속 시도
		spActor->SetDisconnectReason( KStatistics::eSIColDR_Same_Server_Connect );
		spActor->ReserveDestroy();                  //기존 사용자도 접속을 끝난다.

		//SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );

		SetDisconnectReason( KStatistics::eSIColDR_Same_Server_Connect );
		ReserveDestroy();
		return false;
	}

	// 동일 계정이 발견되지 않았다면 UID 변경.
	KActorManager::GetKObj()->UpdateUID( kAck_.m_kAccountInfo.m_nUserUID, *this );

	// 아이디로 이름 설정
	SetName( kAck_.m_kAccountInfo.m_wstrID );

	// 디버깅시 user name을 확인하기 위해 실명 설정.
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨

	// 권한 설정

	// 넥슨 또는 사내 유저인가?
	m_kAccountInfo.m_wstrName = kAck_.m_kAccountInfo.m_wstrName;
	m_kAccountInfo.m_iAuthLevel = kAck_.m_kAccountInfo.m_iAuthLevel;
	m_kAccountInfo.m_bInternalUser = kAck_.m_kAccountInfo.m_bInternalUser;
	//}}

	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	if( kAck_.m_kAccountInfo.m_iAuthLevel == SEnum::UAL_SUPERDEVELOPER )
	{
		START_LOG( cout2, L"슈퍼 개발자 계정입니다!!!" )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kAck_.m_kAccountInfo.m_iAuthLevel )
			<< END_LOG;

		// 클라이언트에서 인식할 수 있도록 등급을 개발자 등급으로 변경한다.
		kAck_.m_kAccountInfo.m_iAuthLevel = SEnum::UAL_DEVELOPER;
		m_kAccountInfo.m_iAuthLevel = SEnum::UAL_DEVELOPER;
	}

	//}}

	// 운영자급 이상은 유닛통계에 적용 안시킴
	m_kUserStatistics.SetIsNormalUser( static_cast<char>(GetAuthLevel()) );

	// 넥슨 어카운트 정보 설정
	m_kNexonAccountInfo = kAck_.m_kNexonAccountInfo;
	// 사내 서버에서는 유저 유아이디로 넥슨 고유번호를 설정한다.
	if( m_kNexonAccountInfo.m_uiNexonSN == 0)
	{
		m_kNexonAccountInfo.m_uiNexonSN = ( unsigned int )kAck_.m_kAccountInfo.m_nUserUID;
	}
	//{{ 2010. 12. 1	최육사	NexonSN추가
	kAck.m_uiNexonSN = kAck_.m_kNexonAccountInfo.m_uiNexonSN;
	//}}

	// 추천인 등록 여부 저장
	//{{ 2010. 01. 08  최육사	추천인리스트
	m_kUserRecommendManager.SetIsRecommend( kAck_.m_kAccountInfo.m_bIsRecommend );
	//}}

	// 블록유저면 사유를 받아놓는다.
	//{{ 계정 제재 일수 상태 추가 - 김민성
	if( GetAuthLevel() <= SEnum::UAL_BLOCKED_1 && GetAuthLevel() >= SEnum::UAL_BLOCKED_9 )
	//}}
	{
		m_kAccountBlockInfo.m_wstrEndTime		= kAck_.m_kAccountInfo.m_kAccountBlockInfo.m_wstrEndTime;
		m_kAccountBlockInfo.m_wstrBlockReason	= kAck_.m_kAccountInfo.m_kAccountBlockInfo.m_wstrBlockReason;
	}

	//080602.hoons. 해킹유저 정보.
	m_cHackingUserType = kAck_.m_cHackingUserType;
	kAck.m_cHackingUserType = kAck_.m_cHackingUserType;

	//{{ 2008. 9. 2  최육사		클라이언트 광고
	SiKGameSysVal()->GetAdvertisementURL( kAck.m_wstrAdURL );
	//}}

	//{{ 2010. 8. 3	최육사	성별정보추가
	kAck.m_bSex = kAck_.m_kNexonAccountInfo.m_bSex;
	kAck.m_uiAge = kAck_.m_kNexonAccountInfo.m_uiAge;
	//}}

	//{{ 2011. 02. 23	최육사	계정 단위 카운트
	m_kUserStatistics.InitAccountCountInfo( kAck_.m_kAccountInfo.m_wstrRegDate, kAck_.m_mapAccCountInfo );
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	SetIsUseOTP( kAck_.m_kNexonAccountInfo.m_uSecureCode  );		// OTP 랑 지정 PC 중 하나라도 쓰고 있으면 OTP 유저

	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	//}
	//}}

	// Auth Level에 따라 접속 제한.
	//switch( GetAuthLevel() )
	//{
	//   case SEnum::UAL_BLOCKED:
	//       {
	//           kPacket.m_nOK = NetError::ERR_VERIFY_11;
	//           SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );

	//		SetDisconnectReason( KStatistics::eSIColDR_Account_Block );
	//           ReserveDestroy();
	//           return;
	//       }
	//case SEnum::UAL_NORMAL:
	//case SEnum::UAL_SUPERIOR:
	//case SEnum::UAL_GM:
	//case SEnum::UAL_DEVELOPER:
	//	break;
	//}

	// IP 제한.
	if( GetAuthLevel() < SEnum::UAL_DEVELOPER )
	{
		if( GetKGSSimLayer()->CheckIP( GetIPStr() ) == false )
		{
			kAck.m_iOK = NetError::ERR_VERIFY_14;
			//SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );

			SetDisconnectReason( KStatistics::eSIColDR_IP_Block );
			ReserveDestroy();

			START_LOG_WITH_NAME( cerr )
				<< BUILD_LOG( NetError::GetErrStr( NetError::ERR_VERIFY_14 ) )
				<< BUILD_LOG( GetIPStr() )
				<< END_LOG;

			return false;
		}
	}

	//{{
	//}}

	//{{ 2013. 05. 29	최육사	제재 리스트 통합
	std::vector< int > vecBlockType;
	vecBlockType.push_back( KBlockInfo::BT_IP_BLOCK );
	vecBlockType.push_back( KBlockInfo::BT_ACCOUNT_BLOCK );
	vecBlockType.push_back( KBlockInfo::BT_TRADE_BLOCK );
	SendCheckBlockUser( vecBlockType );
	//}}

	//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
	//}}
	return true;
}

//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
bool KGSUser::OnAccountSelectUnitAck( IN OUT KDBE_SELECT_UNIT_ACK& kAck_, IN bool bIsChannelChange )
//}}
{
	kAck_.m_kUnitInfo.m_iOwnerUserUID = GetUID();
	kAck_.m_kUnitInfo.m_nStraightVictories = 0;
	kAck_.m_kUnitInfo.m_wstrIP = GetP2PIP();
	kAck_.m_kUnitInfo.m_usPort = static_cast<USHORT>(GetP2PPort());
	kAck_.m_kUnitInfo.m_bIsGameBang = IsPcBang();
	//{{ 2011. 10. 13	최육사	컨텐츠 관리자
	kAck_.m_bCashShopOpen = SiKGSContentManager()->IsEnableCashShop();
	//}}

	const CXSLUnit::UnitTemplet* pUnitTemplet = SiCXSLUnitManager()->GetUnitTemplet( static_cast<CXSLUnit::UNIT_CLASS>(kAck_.m_kUnitInfo.m_cUnitClass) );
	if( pUnitTemplet == NULL )
	{
		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOGc( kAck_.m_kUnitInfo.m_cUnitClass )
			<< END_LOG;

		kAck_.m_iOK = NetError::ERR_SELECT_UNIT_01;
		return false;
	}

	//Item 장착 스탯 항목
	kAck_.m_kUnitInfo.m_ucLevel				= SiKExpTable()->CheckLevelUp( 1, kAck_.m_kUnitInfo.m_iEXP );
	kAck_.m_kUnitInfo.m_nNowBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)kAck_.m_kUnitInfo.m_ucLevel );
	kAck_.m_kUnitInfo.m_nNextBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)(kAck_.m_kUnitInfo.m_ucLevel + 1) );

	KStatTable::KUnitStatInfo sUnitStat;
	SiKStatTable()->GetUnitStat( kAck_.m_kUnitInfo.m_cUnitClass, kAck_.m_kUnitInfo.m_ucLevel, sUnitStat );
	kAck_.m_kUnitInfo.m_kStat.m_iBaseHP		+= (int)sUnitStat.m_uiHP;
	kAck_.m_kUnitInfo.m_kStat.m_iAtkPhysic	+= (int)sUnitStat.m_usAtkPhysic;
	kAck_.m_kUnitInfo.m_kStat.m_iAtkMagic	+= (int)sUnitStat.m_usAtkMagic;
	kAck_.m_kUnitInfo.m_kStat.m_iDefPhysic	+= (int)sUnitStat.m_usDefPhysic;
	kAck_.m_kUnitInfo.m_kStat.m_iDefMagic	+= (int)sUnitStat.m_usDefMagic;

	// 근성도 최대값
	kAck_.m_kUnitInfo.m_iSpiritMax = SiKSpiritTable()->GetMaxSpirit();


	SetUnitInfo( kAck_.m_kUnitInfo );

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	// SetUnitInfo()가 호출된 이후에 호출해야한다.
	m_kUserAbuserManager.InitCharAbuser( GetThisPtr<KGSUser>() );
	//}}

	//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	if( GetED() < 0 )
	{
		START_LOG( cout, L"ED가 마이너스인 유저가 발견되었습니다~!!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetUserID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetED() );

        int iChannelID = SiKChannelManager()->GetChannelID();
        UidType iServerUID = 0;
        SiKChannelManager()->GetServerUIDByChannelID( iChannelID, iServerUID );

		KDBE_SEND_PHONE_MSG_NOT kPacketNot;
		kPacketNot.m_wstrSMSMessage = x2::str( x2::wformat( L"(-)ED유저: UserUID(%d),NickName(%s),ED(%d)" ) //,ServerIP(%s)" ) 해외팀 주석 처리
            % GetUID()
            % GetCharName()
            % GetED()
            //% KBaseServer::GetKObj()->GetPublicIP()	해외팀 주석 처리
            );
		//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
		SiKSMSPhoneNumberManager()->GetPhoneNumberList( KSMSPhoneNumberManager::PNLT_ELSWORD_DEV_TEAM, kPacketNot.m_vecPhoneNum );
		//}}
		if( kPacketNot.m_vecPhoneNum.empty() == false )
		{
			SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
		}

		// 자동으로 어뷰저 등록
		m_kUserAbuserManager.RegEDAbuser( GetThisPtr<KGSUser>() );
		m_kUserAbuserManager.RegItemAbuser( GetThisPtr<KGSUser>() );
		//HackUserRegPacketMornitor();
	}
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2011. 09. 23	최육사	로그오프 상태 ED체크
	if( kAck_.m_iRealDataED != -1 )
	{
		if( GetAuthLevel() < SEnum::UAL_GM )
		{
			if( GetED() != kAck_.m_iRealDataED )
			{
				START_LOG( cout, L"[알림] 캐릭터 선택 해제 시점에 남겼던 ED값과 유저가 보유하고 있는 ED값이 다르다! DB해킹인가!?" )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( GetED() )
					<< BUILD_LOG( kAck_.m_iRealDataED );

				//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
				KEGS_SERVER_CHECK_HACK_USER_REQ kDummy;
				kDummy.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_LOGOUT_ED_CHECK;
				HackUserRegRejectedUser( kDummy.m_cHackingCheckType );
				HackUserKick( kDummy );
				//}}
			}
		}
		else
		{
			if( GetED() != kAck_.m_iRealDataED )
			{
				START_LOG( cout, L"[알림] 운영자급 이상 계정에 대해서 ED값 변동이 있었는지 체크 바랍니다!" )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( GetED() )
					<< BUILD_LOG( kAck_.m_iRealDataED );
			}
		}
	}
	//}}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	//{{ 2012. 02. 02	최육사	배틀필드 시스템
//#ifdef SERV_BATTLE_FIELD_SYSTEM
	// 마지막 위치 정보
	SetLastPosInfo( kAck_.m_kUnitInfo.m_kLastPos );

	// MapID 유효성 검사
	if( GetMapID() == SEnum::VMI_TUTORIAL )
	{
		// 캐릭터를 처음 생성한 경우, 오류가 아니다. 그냥 이대로 저장!
	}
	else if( SEnum::IsBattleFieldID( GetMapID() ) == true )		// 배틀필드
	{
		//{{ 2012. 11. 13	최육사		입장 가능한 마을 검사 및 보정
		int iRequireLevel = 0;
		int iRequireDungeonID = 0;

		const SEnum::BATTLE_FIELD_ID eBattleFieldID = static_cast<SEnum::BATTLE_FIELD_ID>( GetMapID() );
		const int iEnterVillageMapID = SiCXSLBattleFieldManager()->GetEnterVillageMapID( eBattleFieldID );

		// 입장 가능한 배틀필드인지 검사!
		const bool bCanEnterBattleField = SiCXSLBattleFieldManager()->IsCanEnterBattleField( eBattleFieldID, GetLevel(), m_mapDungeonClear, iRequireLevel, iRequireDungeonID );

		// 입장 가능한 배틀필드 입장 마을인지 검사!
		const bool bCanEnterVillageMap = CheckEnterTheVillage( iEnterVillageMapID );

		// 둘중 하나라도 체크를 통과 못하면 입장 마을을 다시 정하자!
		if( bCanEnterBattleField == false  ||  bCanEnterVillageMap == false )
		{
			// 최대한 입장 가능한 마을 mapid를 가져오자!
			KLastPositionInfo kLastPos;
			kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( GetLevel(), m_mapDungeonClear );

			// 다시 체크!
			if( CheckEnterTheVillage( kLastPos.m_iMapID ) == false )
			{
				kLastPos.m_iMapID = SiCXSLMapData()->GetDefaultMapID();
			}

			START_LOG( cerr, L"입장 가능한 배틀필드가 아닌데 DB에 저장되어 있습니다!" )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( kAck_.m_kUnitInfo.m_kLastPos.m_iMapID )
				<< BUILD_LOG( GetMapID() )
				<< END_LOG;

			// 선택된 캐릭터의 정보도 수정.
			SetLastPosInfo( kLastPos );
			kAck_.m_kUnitInfo.m_kLastPos = kLastPos;
		}
		//}}
	}
	else if( SEnum::IsVillageMapID( GetMapID() ) == true  ||  SEnum::IsRestFieldID( GetMapID() ) == true )			// 마을 & 휴식처
	{
		//{{ 2012. 11. 13	최육사		입장 가능한 마을 검사 및 보정
		// 마을에 입장할 조건이 안된다면..
 		if( CheckEnterTheVillage( GetMapID() ) == false )
 		{
 			// 최대한 입장 가능한 마을 mapid를 가져오자!
 			KLastPositionInfo kLastPos;
 			kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( GetLevel(), m_mapDungeonClear );

 			// 다시 체크!
 			if( CheckEnterTheVillage( kLastPos.m_iMapID ) == false )
 			{
 				kLastPos.m_iMapID = SiCXSLMapData()->GetDefaultMapID();
 			}

 			START_LOG( cerr, L"입장할 수 없는 마을인데 DB에 저장되어 있었다?" )
 				<< BUILD_LOG( GetCharName() )
 				<< BUILD_LOG( kAck_.m_kUnitInfo.m_kLastPos.m_iMapID )
 				<< BUILD_LOG( GetMapID() )
 				<< END_LOG;

 			// 선택된 캐릭터의 정보도 수정.
 			SetLastPosInfo( kLastPos );
 			kAck_.m_kUnitInfo.m_kLastPos = kLastPos;
 		}
		//}}
	}
	else
	{
		START_LOG( cerr, L"저장된 마지막 위치가 튜토리얼도 마을도 배틀필드도 아닌 위치다! 일어나면 안되는 오류!" )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kAck_.m_kUnitInfo.m_kLastPos.m_iMapID )
			<< BUILD_LOG( GetMapID() )
			<< END_LOG;

		// 선택된 캐릭터의 정보도 수정.
		KLastPositionInfo kLastPos;
		kLastPos.m_iMapID = SiCXSLMapData()->GetDefaultMapID();
		SetLastPosInfo( kLastPos );
		kAck_.m_kUnitInfo.m_kLastPos = kLastPos;
	}

	//}}

	KDBE_UPDATE_INVENTORY_ITEM_POS_NOT kPacketToDB;
	if( m_kInventory.Init( kAck_.m_mapInventorySlotSize, kAck_.m_mapItem, kPacketToDB.m_kItemPositionUpdate, bIsChannelChange ) == false )
	{
		START_LOG( cerr, L"인벤토리 초기화시 문제 발생" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( GetAuthLevel() )
			<< END_LOG;
	}

	// 잘못된 아이템 인벤토리 정보 복구
	if( !kPacketToDB.m_kItemPositionUpdate.empty() )
	{
		kPacketToDB.m_iUnitUID = GetCharUID();
		SendToGameDB( DBE_UPDATE_INVENTORY_ITEM_POS_NOT, kPacketToDB );
	}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2010. 01. 04  최육사	인벤토리 기능개선

	// 패킷 사이즈를 줄이기 위해 DB로부터 받은 인벤토리 정보를 일단 지웁니다.
	kAck_.m_mapItem.clear();

	// 기존과 동일한 동작을 보장하기위해 장착장비 정보는 select_unit_ack에 담습니다. ( 예외상황 제거 )
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_E_EQUIP, kAck_.m_mapItem );

	// 인벤토리 슬롯 사이즈 얻기
	m_kInventory.GetInventorySlotSize( kAck_.m_mapInventorySlotSize );

	//}}
	//////////////////////////////////////////////////////////////////////////

	m_iNumResurrectionStone.SetValue( kAck_.m_iNumResurrectionStone );
	//{{ 2009. 10. 14  최육사	자동결제 부활석
	m_iNumAutoPaymentResStone.SetValue( kAck_.m_iNumAutoPaymentResStone );
	//}}

	StateTransition( KGSFSM::I_TO_FIELD_MAP );

	//////////////////////////////////////////////////////////////////////////
	//획득 스킬리스트.
	LIF( KUserSkillTree::MAX_SKILL_SLOT == 8 );
	int aSkillSlot[KUserSkillTree::MAX_SKILL_SLOT];
	aSkillSlot[KUserSkillTree::SKILL_SLOT_A1] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[0].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_A2] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[1].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_A3] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[2].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_A4] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[3].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_B1] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[0].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_B2] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[1].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_B3] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[2].m_iSkillID;
	aSkillSlot[KUserSkillTree::SKILL_SLOT_B4] = (int)  kAck_.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[3].m_iSkillID;

	//{{ 2011. 01. 06  김민성  스킬슬롯체인지 체크(인벤토리-기간제) 기능 구현
	m_kInventory.GetSlotChangeBEndDate( CXSLItem::EI_EXPAND_SKILL_SLOT_EVENT, kAck_.m_kUnitInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate );
	m_kInventory.GetSlotChangeBEndDate( CXSLItem::CI_EXPAND_SKILL_SLOT_IN_PACKAGE, kAck_.m_kUnitInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate );
	//}}

	m_kSkillTree.SetMaxCSPoint( kAck_.m_kUnitInfo.m_iMaxCSPoint );
	m_kSkillTree.SetActiveSkillPageNumber( kAck_.m_kUnitInfo.m_UnitSkillData.m_nActiveSkillPagesNumber );

	int iSPointAtThisLevel = 0;
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), iSPointAtThisLevel );
	m_kSkillTree.InitEverySkillPage( IN OUT kAck_.m_vecUserSkillPageData,
		kAck_.m_kUnitInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate,
		kAck_.m_vecSkillUnsealed, GetUnitClass(), iSPointAtThisLevel );

	kAck_.m_kUnitInfo.m_iCSPoint	= m_kSkillTree.GetCSPoint();
	kAck_.m_kUnitInfo.m_iSPoint		= m_kSkillTree.GetSPoint();

	//{{ 2010. 03. 22  최육사	기술의 노트
	m_kSkillTree.InitSkillNote( kAck_.m_cSkillNoteMaxPageNum, kAck_.m_mapSkillNote );
	//}}

	kAck_.m_kUnitInfo.m_UnitSkillData.m_cSkillSlotBExpirationState = (char) m_kSkillTree.GetSkillSlotBExpirationState();
	kAck_.m_kUnitInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate = m_kSkillTree.GetSkillSlotBEndDateString();

	//{{ 2010. 07. 04  최육사	스킬 포인트 오류 확인
	if( CheckSkillPointError() == false )
	{
		if( GetAuthLevel() < SEnum::UAL_GM )
		{
			START_LOG( cerr, L"캐릭터 선택 시점에 스킬 포인트 값이 비정상입니다." )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
		}

	}
	//}}

	// 커뮤니티 옵션 정보 저장
	m_kUserGameOption.SetChatBlackList( kAck_.m_vecChatBlackList );
	m_kUserGameOption.SetDenyOptions( kAck_.m_kDenyOptions );

	//캐릭이 바뀔때마다 퀘스트 데이터를 새로 초기화 한다.
	m_kUserQuestManager.SetUnitQuest( kAck_.m_vecQuest, kAck_.m_vecCompletQuest, GetThisPtr<KGSUser>() );
	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	SiCXSLQuestManager()->GetRandomQuestGroupList( kAck_.m_mapRandomQuestList );
	//}}
	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	m_kUserAccountQuestManager.SetAccountQuest( kAck_.m_vecAccountQuesting, kAck_.m_vecAccountCompleteQuest, kAck_.m_vecQuest, kAck_.m_vecCompletQuest, GetThisPtr<KGSUser>() );
	//}}

	//{{ 2008. 10. 6  최육사	타이틀
	bool bUsePcBangTitle = IsPcBang() & SiKGameSysVal()->IsUsingPcBangTitle( GetPcBangType() );
	m_kUserTitleManager.Init( bUsePcBangTitle, kAck_.m_vecMission, kAck_.m_vecTitle );
	//}}

    // 친구 리스트
    m_kMessengerInfo = kAck_.m_kMessengerInfo;

	//캐릭선택시 마다 센터서버 유저정보에 등록시켜 놓는다.
	{
		KELG_REGISTER_UNIT_NOT kCNNot;
		kCNNot.m_nUnitUID			= GetCharUID();
		kCNNot.m_uiKNMSerialNum		= m_uiKNMSerialNum;
		kCNNot.m_wstrUnitNickName	= GetCharName();
		kCNNot.m_bDenyFriendShip	= m_kUserGameOption.IsDenyFriendShip();
		std::map< UidType, KFriendInfo >::iterator mitFriendInfo;
		for( mitFriendInfo = m_kMessengerInfo.m_mapFriendInfo.begin(); mitFriendInfo != m_kMessengerInfo.m_mapFriendInfo.end(); mitFriendInfo++ )
		{
			kCNNot.m_setFriendUID.insert( mitFriendInfo->first );
		}
		SendToLoginServer( ELG_REGISTER_UNIT_NOT, kCNNot );
	}

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	// 캐릭터 선택시 글로벌 서버에도 캐릭터 정보를 등록시키자!
	{
		KEGB_REGISTER_UNIT_NOT kGBNot;
		kGBNot.m_kUnitInfo.m_iUserUID = GetUID();
		kGBNot.m_kUnitInfo.m_iUnitUID = GetCharUID();
		kGBNot.m_kUnitInfo.m_wstrUnitNickName = GetCharName();
		kGBNot.m_kUnitInfo.m_iGSUID = KBaseServer::GetKObj()->GetUID();
		kGBNot.m_kUnitInfo.m_iChannelID = SiKChannelManager()->GetChannelID();
		kGBNot.m_kUnitInfo.m_wstrIP = NetCommon::GetLocalIPW();
		kGBNot.m_kUnitInfo.m_cAuthLevel = static_cast<char>(GetAuthLevel());
		//{{ 2012. 12. 17	박세훈	잭팟 이벤트
		//}}
		SendToGlobalServer( EGB_REGISTER_UNIT_NOT, kGBNot );
	}
	//}}

	// 유저 통계 해당 유닛의 접속 시간 체크
	m_kTimer[TM_UNIT_DURATION].restart();

	// 분해 대박
	m_kTimer[TM_RESOLVE_JACKPOT].restart();
	m_dResolveJackpotTime = SiCXSLResolveItemManager()->GetJackpotTime();

	// 추천인 설정
	//{{ 2010. 01. 08  최육사	추천인리스트
	m_kUserRecommendManager.SetRecommendedUnitUID( kAck_.m_iRecommendUnitUID );
	kAck_.m_bIsRecommend = m_kUserRecommendManager.IsRecommend();
	//}}

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	//}}

	// 아이템 어뷰저 여부 확인
	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	m_kUserAbuserManager.InitCharAbuser( GetThisPtr<KGSUser>() );
	//}}

	std::set< int > setItemID;
	m_kInventory.GetItemIDSet( setItemID );



	m_bCouple = kAck_.m_bCouple;
	m_iRelationTargetUserUid = kAck_.m_iRelationTargetUserUid;
	m_wstrRelationTargetUserNickname = kAck_.m_wstrRelationTargetUserNickname;


	//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
	//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
	KGameEventManager::ConnectEventFactorSet kFactor(
		GetCharUID(),
		GetLevel(),
		IsPcBang(),
		GetChannelCode(),
		GetUnitType(),
		GetUnitClass(),
		true
		//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
		, GetUID()
		//}}
		, setItemID
		, IsPureSteamUser()
		);

	kFactor.m_pVecConnectTimeEvent		= &kAck_.m_vecConnectTimeEvent;
	kFactor.m_pVecCumulativeTimeEvent	= &kAck_.m_vecCumulativeTimeEvent;

	kFactor.m_pVecCompletedAdvertisementEvent	= &kAck_.m_vecAdvertisementEvent;

	m_kUserEventManager.InitTimeEvent( kFactor );
	//}}
	//}}

	//030406.hoons. 사제리스트 정리 및 예외처리를 한다.
	if( m_kUserTutorialInfo.OnSelectUnitAck( GetThisPtr<KGSUser>(), kAck_.m_vecTutorialDBUnitInfo ) == false )
	{
		START_LOG( cerr, L"사제리스트 정보 셋팅실패.!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( GetLevel() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
	}
	else
	{
		if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_STUDENT &&
			m_kUserTutorialInfo.IsExistbyTutorData() == false )
		{
			KTutorialUnitInfo kTutInfo;
			kTutInfo.m_iUnitUID = GetCharUID();
			kTutInfo.m_ucLevel = GetLevel();
			kTutInfo.m_wstrNickName = GetCharName();
			kTutInfo.m_bIsOnline = true;
			if( SiKTutorialManager()->AddUnit( kTutInfo ) == false )
			{
				START_LOG( cerr, L"사제시스템 리스트 추가 실패.!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOGc( GetLevel() )
					<< BUILD_LOG( GetCharName() )
					<< END_LOG;
			}
		}
	}

	//{{ 2009. 7. 7  최육사		랭킹 개편
	m_kUserRankingManager.Init( kAck_.m_vecHenirRanking
		);
	//}}

	//{{ 2010. 07. 21  최육사	펫 시스템

	//////////////////////////////////////////////////////////////////////////
	// 로봇인 경우 강제로 펫 적용하기
	//////////////////////////////////////////////////////////////////////////

	// 펫 정보 초기화!
	int iPetCategorySlotSize = 0;
	std::map< UidType, KInventoryItemInfo > mapPetInventory;
	m_kUserPetManager.Init( GetThisPtr<KGSUser>(), kAck_.m_iSummonedPetUID, kAck_.m_mapPetItem, kAck_.m_vecPetList, iPetCategorySlotSize, mapPetInventory, kAck_.m_vecCanEvolutionPetList, kAck_.m_vecNeverSummonPetList );
	kAck_.m_mapPetItem.clear();

	// 소환되어 있는 펫이 있다면 펫 인벤토리 정보 초기화!
	if( kAck_.m_iSummonedPetUID != 0 )
	{
		std::vector< KInventoryItemInfo > vecPetInventorySlotInfo;
		m_kInventory.InitPetInventory( kAck_.m_iSummonedPetUID, iPetCategorySlotSize, mapPetInventory, vecPetInventorySlotInfo );
	}
	//}}

	//{{ 2011. 04. 11	최육사	대리 상인
	m_kUserPersonalShopManager.Init( kAck_.m_kPShopAgencyInfo );
	//}}

	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	m_kUserStatistics.InitCharacterCountInfo( kAck_.m_mapCharGameCount, kAck_.m_wstrUnitCreateDate );
	kAck_.m_mapCharGameCount.clear();
	//}}

	//{{ 2011. 03. 22	최육사	캐릭터 첫 접속 로그
	if( m_kUserStatistics.IsFirstCharSelectToday( kAck_.m_wstrUnitLastLoginDate ) == true )
	{
		// 로그 남기자!
		WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_DAILY_CHAR_FIRST_SELECT );
	}
	//}}

	//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
	//}}
	// 체험ID 제한
	if( IsGuestUser() )
	{
		if( GetLevel() > SiKGameSysVal()->GetGuestUserLimitLevel() )
		{
			// 레벨19 제한
			kAck_.m_iOK = NetError::ERR_GUEST_USER_02;
			return false;
		}
	}

	// 클라이언트로 전송되는 ACK에서 불필요한 데이터 삭제
	//{{ 2009. 7. 7  최육사		랭킹 개편
	kAck_.m_vecHenirRanking.clear();
	//}}
	kAck_.m_vecConnectTimeEvent.clear();

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	KEGS_WORLD_MISSION_UPDATE_NOT kMission;
	SiKGSWorldMissionManager()->GetMissionInfo( kMission );
	SendPacket( EGS_WORLD_MISSION_UPDATE_NOT, kMission );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	KEGS_GLOBAL_MISSION_UPDATE_NOT kGlobalMission;
	SiKGSGlobalMissionManager()->GetGlobalMissionInfo( kGlobalMission );
	SendPacket( EGS_GLOBAL_MISSION_UPDATE_NOT, kGlobalMission );

	CTime tCurr = CTime::GetCurrentTime();

	SetUnitLoginTime_INT( tCurr );

	//{{ 버프 관리 시스템 - 김민성
	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kBuffReq;
	kBuffReq.m_iUnitUID = GetCharUID();
	int iWorldBuff = SiKGSWorldMissionManager()->GetWorldBuffState();
	int iWorldBuffPriod = SiKGSWorldMissionManager()->GetWorldBuffEndTime();

	if( iWorldBuff != CXSLBuffManager::BTI_NONE )
	{
		if( m_kUserBuffManager.IsBuffActivated( iWorldBuff ) == false )
		{
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_THANKS_OF_RESIDENTS, kBuffReq.m_vecDeActivateBuff );
			ActivateBuff( CXSLBuffManager::BTI_BUFF_THANKS_OF_RESIDENTS, kBuffReq.m_vecActivateBuff, tCurr, iWorldBuffPriod );
		}
	}
	else
	{
		DeactivateBuff( CXSLBuffManager::BTI_BUFF_THANKS_OF_RESIDENTS, kBuffReq.m_vecDeActivateBuff );
	}

	// itexpertkim // 김현철 // 2014-01-22 // 네이버 이벤트 버프 // 네이버 채널링 디파인으로 둘러야하나?
	//ActivateBuff( CXSLBuffManager::BTI_BUFF_NAVER, kBuffReq.m_vecActivateBuff );

	if( IsPcBang() == true )
	{
		ActivateBuff( CXSLBuffManager::BTI_BUFF_PREMIUM_PC_ROOM, kBuffReq.m_vecActivateBuff );
	}
	//}}
	//}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		ActivateBuff( CXSLBuffManager::BTI_BUFF_TRADE_BLOCK, kBuffReq.m_vecActivateBuff );
	}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	if( m_kComeBackUserInfo.GetIsComeBackUser() == true )
	{
		CTime tComeBackBuffEnd = m_kComeBackUserInfo.GetComeBackBuffEnd();

		if( m_kComeBackUserInfo.GetIsComeBackFirst() == true )		// 복귀 최초 접속시만 지급
		{
			// 휴면 유저 복귀 보상 큐브 지급
			int iRewardItem = 0;
			int iRewardPeriod = 0;

			if( m_kComeBackUserInfo.GiveComeBackUserRewardCube( iRewardItem, tComeBackBuffEnd, iRewardPeriod ) == true )
			{
				// 휴면 유저 복귀 보상 큐브를 주자!
				m_kUserUnitSelectRewardManager.AddRewardItemInfo( iRewardItem );

				// 휴면 유저 복귀 보상 칭호 지급
				KDBE_INSERT_TITLE_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID = GetCharUID();
				//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
				kPacketToDB.m_iTitleID = KRewardTable::ERI_COME_BACK_USER_TITLE;
				//}}
				kPacketToDB.m_sPeriod  = (short)iRewardPeriod;
				SendToGameDB( DBE_INSERT_TITLE_REQ, kPacketToDB );

				// 강제 장착시켰으니 알려주자
				//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
				kAck_.m_kUnitInfo.m_iTitleID = KRewardTable::ERI_COME_BACK_USER_TITLE;
				//}}

				// DB에 기록하자
				KDBE_WRITE_COME_BACK_REWARD_NOT kNot;
				kNot.m_iUserUID					= GetUID();
				kNot.m_iRewardStep				= m_kComeBackUserInfo.GetComeBackRewardLevel();
				kNot.m_wstrComeBackBuffEndDate	= tComeBackBuffEnd.Format( _T( "%Y-%m-%d %H:%M:%S" ) );
				SendToAccountDB( DBE_WRITE_COME_BACK_REWARD_NOT, kNot );

				// 휴면 유저 복귀 시스템 우편 알림
				AnnunciateComeBackUser();
			}
		}

		//{{ 버프 관리 시스템 - 김민성
		int iEndComeBackBuddTime = static_cast<int>(tComeBackBuffEnd.GetTime() - tCurr.GetTime());
		ActivateBuff( CXSLBuffManager::BTI_BUFF_RETURN_OF_HERO, kBuffReq.m_vecActivateBuff, tCurr, iEndComeBackBuddTime );
		//}

		// 오늘 첫 접속이라면
		if( m_kUserStatistics.IsFirstUserLoginToday( GetUID(), kAck_.m_wstrUnitLastLoginDate ) == true )
		{
			// 보안 사용 정보 로그 남기자!
			SendFirstLoginComeBackUser( kAck_.m_kUnitInfo, m_kComeBackUserInfo.GetIsComeBackFirst() );
		}

		// 보상 지급 및 기록이 끝나면 보상 지급 조건 변경
		m_kComeBackUserInfo.SetIsComeBackFirst( false );
	}

	m_kComeBackUserInfo.SetIsUnitSelect( true );

	//}}


	//{{ 버프 관리 시스템 - 김민성
	SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kBuffReq );
	//}

	//{{ 2011. 08. 12   김민성      헤니르 개편
	// 현재 이벤트 정보 얻어 오기
	kAck_.m_PacketHenirRewardCount.m_bUnLimited = false;
	kAck_.m_PacketHenirRewardCount.m_iEventMAX = 0;

	// 현재 적용중인 이벤트가 있는가?
	SiKGameEventManager()->GetHenirRewardCountEvent( kAck_.m_PacketHenirRewardCount.m_bUnLimited, kAck_.m_PacketHenirRewardCount.m_iEventMAX );

	m_kUserDungeonManager.Init( kAck_.m_PacketHenirRewardCount.m_bUnLimited
							  , kAck_.m_PacketHenirRewardCount.m_iEventMAX
							  , kAck_.m_PacketHenirRewardCount.m_iNormal
							  , kAck_.m_PacketHenirRewardCount.m_iPremium
							  , kAck_.m_PacketHenirRewardCount.m_iEvent
							  , IsPcBang()
							  , GetPcBangType()
							  );

	m_kUserDungeonManager.GetHenirRewardCountInfo( kAck_.m_PacketHenirRewardCount );
	//}}
	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}

	//{{ 2012. 02. 21	최육사	배틀필드 시스템
	// 최대 HP정보를 새로 구하기 위해 KRoomUserInfo를 계산한다.
	KRoomUserInfo kRoomUserInfo;
	GetRoomUserInfo( kRoomUserInfo, CXSLRoom::RT_BATTLE_FIELD );

	// 장착 스킬 정보
	kAck_.m_kUnitInfo.m_UnitSkillData = kRoomUserInfo.m_UnitSkillData;

	// 유닛 매니저 초기화
	m_kUserUnitManager.Init( kAck_.m_kGamePlayStatus, kRoomUserInfo );
	//}}

//{{ 2012. 10. 10	박세훈	빙고 이벤트
	//}}

	//{{ 2012. 05. 16	김민성       케릭터 선택시 레벨에 따라 맵ID 를 변환
	//}}

	//{{ 2012. 12. 20	최육사	아라 첫 선택 튜토리얼
	kAck_.m_bFirstSelect = ( m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_SELECT_CHAR ) == 0 );
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	m_kUserRelationshipManager.Init( kAck_.m_kRelationshipInfo );
	//}



	// 배틀 필드 보스 정보 전송
	if( SiKGSFieldBossManager()->IsPortalOpen() == true )
	{
		KEGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT kPacket;
		kPacket.m_iBattleFieldID	= SiKGSFieldBossManager()->GetPortalAppearanceMap();
		kPacket.m_iBossFieldID		= SiKGSFieldBossManager()->GetPortalDestination();
		SendPacket( EGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT, kPacket );
	}
	return true;
}
//}}

//{{ 2009. 10. 14  최육사	코드정리
//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
void KGSUser::OnSelectUnitSuccess( IN const KDBE_SELECT_UNIT_ACK& kAck_, IN bool bIsChannelChange )
//}}
{
	// 기간제 검사
	ExpireCashSkillPoint( true );
	ExpireSkillSlotB( true );
	ExpireItem( true );
	ExpireTitle( true );

	// 도착한 캐쉬 선물이 있는지 확인하러 가기
	KENX_BT_NISMS_INVENTORY_READABLE_REQ kPacketReq;
	kPacketReq.m_ulPacketNo	  	   = SiKNexonBillingTCPManager()->GetNextPacketNo();
	kPacketReq.m_bytePacketType	   = KNexonBillingTCPPacket::NISMS_INVENTORY_READABLE;
	kPacketReq.m_wstrGameID		   = GetName(); // 인벤토리 공유를 위해 Nexon ID를 쓴다.
	kPacketReq.m_byteShowInventory = 10;		// 10 : true, 30 : false
	kPacketReq.m_byteIsPresent	   = 1;			// 0 : 일반구매, 1 : 선물, 2 : 일반구매+선물

	KEventPtr spEvent( new KEvent );
	UidType anTrace[2] = { GetUID(), -1 };
	spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_NISMS_INVENTORY_READABLE_REQ, kPacketReq );
	SiKNexonBillingTCPManager()->QueueingEvent( spEvent );

	//{{ 2009. 10. 14  최육사	자동결제
	// 자동결제 상품 체크
	// ※ 하루에 한번만 빌링서버에 날려야함! 빌링서버의 퍼포먼스를 잡아먹는 이벤트임

	CTime tCurTime = CTime::GetCurrentTime();
	CTime tCheckTime = CTime( tCurTime.GetYear(), tCurTime.GetMonth(), tCurTime.GetDay(), 6, 0, 0 );

	// 새벽 6시를 기준으로 체크하기 위해 조건검사
	if( tCurTime.GetHour() >= 0  &&  tCurTime.GetHour() < 6 )
	{
		tCheckTime -= CTimeSpan( 1, 0, 0, 0 );
	}

	// 다음 자동결제 체크 패킷 전송 시각을 내일 6시로 설정!
	m_tAutoPaymentCheckTime = tCheckTime + CTimeSpan( 1, 0, 0, 0 );

	CTime tLastRefillDate;
	if( KncUtil::ConvertStringToCTime( kAck_.m_wstrAutoPayResStoneLastDate, tLastRefillDate ) )
	{
		if( tLastRefillDate < tCheckTime  &&
			m_iNumAutoPaymentResStone == 0 ) // 자동결제 부활석이 0개일때만 체크.
		{
			std::vector< unsigned long > vecAutoPaymentProductList;
			SiKNexonBillingTCPManager()->GetAutoPaymentProductList( vecAutoPaymentProductList );

			KENX_BT_CHECK_AUTO_PAYMENT_USER_REQ kPacketReq;
			kPacketReq.m_ulPacketNo		= SiKNexonBillingTCPManager()->GetNextPacketNo();
			kPacketReq.m_bytePacketType	= KNexonBillingTCPPacket::CHECK_AUTO_PAYMENT_USER;
			kPacketReq.m_wstrGameID		= GetName(); // 인벤토리 공유를 위해 Nexon ID를 쓴다.
			kPacketReq.m_wstrUserID		= GetName(); // 인벤토리 공유를 위해 Nexon ID를 쓴다.

			if( KSimLayer::GetKObj()->GetBillingFlag() == KSimLayer::BF_NEXON_KOREA )
			{
				kPacketReq.m_ulProductNo = 67215; // 본섭 : 코보 서포트 서비스
			}
			else
			{
				kPacketReq.m_ulProductNo = 77697; // 테섭 : 코보 서포트 서비스
			}

			KEventPtr spEvent( new KEvent );
			UidType anTrace[2] = { GetUID(), -1 };
			spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_CHECK_AUTO_PAYMENT_USER_REQ, kPacketReq );
			SiKNexonBillingTCPManager()->QueueingEvent( spEvent );

			START_LOG( clog, L"자동결제 했는지 체크하러 가자!" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( kPacketReq.m_ulProductNo );
		}
		else
		{
			START_LOG( clog, L"자동결제 체크시각이 지나서 그냥 통과!" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( kAck_.m_wstrAutoPayResStoneLastDate )
				<< BUILD_LOG( m_iNumAutoPaymentResStone );
		}
	}

	//}}

	// 우편 리스트 요청
	KDBE_GET_POST_LETTER_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();
	SendToGameDB( DBE_GET_POST_LETTER_LIST_REQ, kPacketToDB );

	//{{ 2010. 02. 09  최육사	일일 이벤트 퀘스트
	m_kUserQuestManager.InitEventQuest( GetThisPtr<KGSUser>() );
	//}}
	//{{ QUEST 개편 - 김민성
	m_kTimer[TM_EVENT_CHECK].restart();
	m_kUserQuestManager.CheckEventQuest( GetThisPtr<KGSUser>() );
	m_kUserQuestManager.CheckLimitLevelQuest( GetThisPtr<KGSUser>() );
	//}}
	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	m_kUserAccountQuestManager.CheckAccountQuest( GetThisPtr<KGSUser>() );
	//}}
	// 새로운 미션 검사
	m_kUserTitleManager.CheckNewMission( GetThisPtr<KGSUser>(), true );

	//{{ 2009. 9. 22  최육사	길드
	if( GetGuildUID() > 0 )
	{
		KELG_GET_GUILD_INFO_REQ kPacketToLg;
		kPacketToLg.m_iUnitUID = GetCharUID();
		kPacketToLg.m_iGuildUID = GetGuildUID();
		GetGuildMemberInfo( kPacketToLg.m_kLoginGuildMember );
		SendToLoginServer( ELG_GET_GUILD_INFO_REQ, kPacketToLg );
	}
	//}}

	//{{ 2011. 05. 10	최육사	대리상인
	if( kAck_.m_kPShopAgencyInfo.m_bIsPShopOpen )
	{
        KERM_CHECK_MY_PSHOP_AGENCY_INFO_REQ kPacketToLgRoom;
        kPacketToLgRoom.m_iUserUID = GetUID();
		kPacketToLgRoom.m_iUnitUID = GetCharUID();
        kPacketToLgRoom.m_wstrNickName = GetCharName();
        kPacketToLgRoom.m_cPersonalShopType = kAck_.m_kPShopAgencyInfo.m_cShopType;
        kPacketToLgRoom.m_wstrAgencyExpirationDate = kAck_.m_kPShopAgencyInfo.m_wstrAgencyExpirationDate;
        kPacketToLgRoom.m_wstrAgencyOpenDate = kAck_.m_kPShopAgencyInfo.m_wstrAgencyOpenDate;
        kPacketToLgRoom.m_wstrPersonalShopName = kAck_.m_kPShopAgencyInfo.m_wstrPersonalShopName;
        kPacketToLgRoom.m_bOnSale = kAck_.m_kPShopAgencyInfo.m_bOnSale;
        kPacketToLgRoom.m_vecSellItemInfo = kAck_.m_kPShopAgencyInfo.m_vecSellItemInfo;
		SendToLoginRoom( ERM_CHECK_MY_PSHOP_AGENCY_INFO_REQ, kPacketToLgRoom );
	}
	//}}

	if( bIsChannelChange == false )
	{
		//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
		KEGS_TIME_EVENT_INFO_NOT kPacketNot;
		kPacketNot.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_INIT;
		m_kUserEventManager.GetNowProcessingTimeEvent( kPacketNot.m_vecTimeEvent );
		if( !kPacketNot.m_vecTimeEvent.empty() )
		{
			SendPacket( EGS_TIME_EVENT_INFO_NOT, kPacketNot );
		}
		//}}

		//{{ 2010. 01. 11  최육사	추천인리스트
		SendUpdateRecommendUserInfo( KRecommendUserInfo::RUIT_LOGIN );
		//}}

		//{{ 2010. 01. 29  최육사	PC방 상용화
		// 클라이언트에 메시지 출력
		if( m_kUserPcBangManager.IsReservedPcBangMessage() == true )
		{
			KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;
			kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_MESSAGE;
			kPacketNot.m_uiData = m_kUserPcBangManager.GetPcBangMessageArgument();
			SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );
		}
		//}}

		//{{ 2010. 8. 16	최육사	기간 리셋 아이템 이벤트
		// 기간 리셋 이벤트 아이템이 있는지 인벤토리를 찾아본다!
		KDBE_RESET_PERIOD_ITEM_REQ kPacketToDB;
		m_kInventory.GetResetPeriodItemUIDList( kPacketToDB.m_mapExpandPeriodItemList );

		// 있으면 기간 초기화 시키러 DB로 가자!
		if( kPacketToDB.m_mapExpandPeriodItemList.empty() == false )
		{
			kPacketToDB.m_iUnitUID = GetCharUID();
			SendToGameDB( DBE_RESET_PERIOD_ITEM_REQ, kPacketToDB );
		}
		//}}

		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_SELECT_CHAR );

		const int iSelectCount = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_SELECT_CHAR );
		if( iSelectCount == 2 )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_SECOND_SELECT );
		}
		else if( iSelectCount == 3 )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_THIRD_SELECT );
		}

		if( GetGuildUID() != 0 )
		{
			m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_JOIN_GUILD_SELECT_CHAR );

			const int iJoinGuildSelectCount = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_JOIN_GUILD_SELECT_CHAR );
			if( iJoinGuildSelectCount == 1 )
			{
				WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_JOIN_GUILD_SELECT_CHAR );
			}
		}
		//}}
	}

	// 모니터링 어뷰저
	//if( m_bIsMornitoringAbuser )
	//{
	//	KDBE_SEND_PHONE_MSG_NOT kPacketNot;
	//	kPacketNot.m_wstrSMSMessage  = L"어뷰저 유저 [";
	//	kPacketNot.m_wstrSMSMessage += GetName();
	//	kPacketNot.m_wstrSMSMessage += L"]가 캐릭터 [";
	//	kPacketNot.m_wstrSMSMessage += GetCharName();
	//	kPacketNot.m_wstrSMSMessage += L"]를 선택하였습니다!";

	//	SiKAbuserLogManager()->GetAbuserNotifyPhoneNumList( kPacketNot.m_wstrPhoneNum );

	//	SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
	//}



	m_vecPersonalShopSell_ItemInfo.clear();

	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
	//}}

	//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
	//}

}
//}}

//{{ 2010. 8. 4	최육사	서버 코드 정리
//#ifdef SERV_DESELECT_UNIT_REFAC

//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void KGSUser::OnDeselectUnit( IN const u_short usEventID, IN const bool bOnDestroy )
{
	// 캐릭터가 선택 되어 있지 않다면 그냥 넘어 가자!
	if( GetCharUID() == 0 )
		return;

	if( GetCharUID() < 0 )
	{
		START_LOG( cerr, L"UnitUID값이 마이너스다! 있을 수 없는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( usEventID )
			<< END_LOG;
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 일단 먼저 이탈에서 대한 처리를 해주자!

	// 1. 대전유저리스트
	SendLeavePVPLobby();

	//{{ 2011. 07. 11	최육사	대전 개편
	SendLeavePVPMatch( NetError::NOT_LEAVE_MATCH_WAIT_REASON_02 );
	//}}

	//{{ 2012. 02. 10	최육사	배틀필드 시스템
	SendLeaveAutoParty( NetError::NOT_LEAVE_AUTO_PARTY_WAIT_REASON_02 );
	//}}

	// 2. ResetUnitInfo에서 UnitUID를 0으로 초기화 하기 때문에 leave처리를 먼저해주어야 한다.
	SendLeaveParty( NetError::NOT_LEAVE_ROOM_REASON_01 );

	// 채널이동으로 인한 캐릭터 선택해제라면 클라이언트로 이탈 패킷을 바로 보내버리자!
	if( usEventID == EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ )
	{
		if( GetPartyUID() > 0 )
		{
			// 이탈 처리
			KEGS_LEAVE_PARTY_ACK kPacket;
			kPacket.m_iOK = NetError::NET_OK;
			SendPacket( EGS_LEAVE_PARTY_ACK, kPacket );
		}
	}

	// 3. 필드도 마찬가지..
	SendLeaveField( NetError::NOT_LEAVE_FIELD_REASON_02 );

	// 4. 개인거래 이탈 처리
	if( GetTradeUID() != 0 )
	{
		KEGS_BREAK_TRADE_REQ kPacket;
		kPacket.m_iReason	= NetError::NOT_LEAVE_ROOM_REASON_01;
		_SendToCnRoom( GetTradeUID(), ERM_BREAK_TRADE_REQ, kPacket );

		SetTradeUID( 0 );
	}

	// 5. 개인상점 이탈 처리
	if( GetPersonalShopUID() != 0 )
	{
		//{{ 2011. 05. 02	최육사	대리상인
		if( CXSLRoom::GetRoomType( GetPersonalShopUID() ) == CXSLRoom::RT_PERSONAL_SHOP )
		{
			KEGS_BREAK_PERSONAL_SHOP_REQ kPacket;
			kPacket.m_iReason	= NetError::NOT_LEAVE_ROOM_REASON_01;
			_SendToCnRoom( GetPersonalShopUID(), ERM_BREAK_PERSONAL_SHOP_REQ, kPacket );
		}
		else
		{
			KERM_LEAVE_PERSONAL_SHOP_REQ kPacket;
			kPacket.m_iReason	= NetError::NOT_LEAVE_ROOM_REASON_01;
			_SendToLoginRoom( GetPersonalShopUID(), ERM_LEAVE_PSHOP_AGENCY_REQ, kPacket );
		}
		//}}

		SetPersonalShopUID( 0 );
	}

	// 6. 현재 방에 있다면..
	if( GetRoomUID() > 0 )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_SQUARE:
			{
				KEGS_LEAVE_SQUARE_REQ kPacket;
				kPacket.m_iReason	= NetError::NOT_LEAVE_ROOM_REASON_01;
				SendToCnRoom( ERM_LEAVE_SQUARE_REQ, kPacket );
			}
			break;

		case CXSLRoom::RT_TRAININGCENTER:
			{
				KERM_LEAVE_ROOM_REQ kPacket;

				// 클라이언트 크래쉬의 경우 [클라이언트 크래쉬시 센터에서 보상 패킷이 보내지 않도록]
				if( GetDisconnectReason() == KStatistics::eSIColDR_Client_Crash )
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_23;
				}
				else
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01;
				}

				SendToCnRoom( ERM_LEAVE_TC_ROOM_REQ, kPacket );
			}
			break;

		case CXSLRoom::RT_BATTLE_FIELD:
			{
				KERM_LEAVE_ROOM_REQ kPacket;
				kPacket.m_bSendAck = false;	// [2012-12-07][최육사] 이 패킷 보내고 나서는 ACK를 받을 필요가 없으므로 SendAck값을 false로 넣자.

				// 접속 종료일 경우
				if( bOnDestroy )
				{
					// 접속 종료 사유로 Neterror를 정한다.
					switch( GetDisconnectReason() )
					{
					case KStatistics::eSIColDR_Client_Crash:	kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_23;	break;
					case KStatistics::eSIColDR_IOCP_GetStatus:	kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_33; break;
					case KStatistics::eSIColDR_Channel_Change:	kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_29; break;
					default:									kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01; break;
					}
				}
				else
				{
					switch( usEventID )
					{
					case EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ:	kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_29; break;
					default:									kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01; break;
					}
				}


				SendToCnRoom( ERM_LEAVE_ROOM_REQ, kPacket );
			}
			break;

		default:
			{
				KERM_LEAVE_ROOM_REQ kPacket;

				// 클라이언트 크래쉬의 경우 [클라이언트 크래쉬시 센터에서 보상 패킷이 보내지 않도록]
				if( GetDisconnectReason() == KStatistics::eSIColDR_Client_Crash )
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_23;
				}
				else
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01;
				}

				SendToCnRoom( ERM_LEAVE_ROOM_REQ, kPacket );
			}
			break;
		}

		//{{ 2010. 12. 30	최육사	방이탈 예외처리
		SetRoomListID( 0 );
		//}}

		//{{ 2012. 02. 07	최육사	배틀필드 시스템
		SetRoomUID( 0 );

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			StateTransition( KGSFSM::I_TO_FIELD_MAP );
		}
		//}}
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// DB에 캐릭터 정보를 업데이트 하기 전에 해야할 작업은 여기서 해두자!

	//{{ 2010. 01. 11  최육사	추천인리스트
	// 7. 추천인 리스트
	SendUpdateRecommendUserInfo( KRecommendUserInfo::RUIT_LOGOUT );
	//}}

	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	SiKEventDelaySystemManager()->UnRegOpenRandomCubeEvent( GetCharUID() );
	//}}

	//{{ 2010. 8. 4	최육사	펫 시스템
	// 캐릭터 선택 해제시 펫 관련 처리!
	m_kUserPetManager.OnDeselectUnit( GetThisPtr<KGSUser>() );
	//}}

	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}

	//{{ 버프 관리 시스템 - 김민성
	CheckDeselectUnitBuff();
	//}

	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	SendUpdateAccountQuestInfo();
	//}}

	m_kUserRidingPetManager.OnDeselectUnit();

	//사제시스템에 등록이 되어있다면 유닛을 삭제해준다.
	m_kUserTutorialInfo.DisconnectUpdateUnitInfo( GetThisPtr<KGSUser>() );

	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_STUDENT  &&  SiKTutorialManager()->IsExistbylist( GetCharUID() ) == true )
	{
		if( SiKTutorialManager()->DeleteUnit( GetCharUID() ) == false )
		{
			START_LOG( cerr, L"사세시스템중 제자정보 지우기 실패.!" )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetLevel() )
				<< END_LOG;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업 -- 캐릭터 정보 DB 업데이트 보다 먼저 호출되어야한다.
	//}}

	// 캐릭터 정보 DB업데이트
	WriteUnitInfoToDB( true, usEventID );

	//{{ 2012. 09. 22	박세훈	빙고 이벤트
	//}}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 통계나 로그에 관련된 처리는 여기서 하자!

	{
		// 로그인 서버에 등록된 캐릭터 정보 해제
		KELG_UNIT_DISCONNECT_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		//{{ 2009. 9. 29  최육사	길드
		kPacketNot.m_iGuildUID = GetGuildUID();
		//}}
		for( const auto& kFriend : m_kMessengerInfo.m_mapFriendInfo )
		{
			kPacketNot.m_setFriendUID.insert( kFriend.first );
		}
		SendToLoginServer( ELG_UNIT_DISCONNECT_NOT, kPacketNot );
	}

	{
		//{{ 2012. 03. 20	최육사	배틀필드 시스템
		// 글로벌 서버에 등록된 캐릭터 정보 해제
		KEGB_UNIT_DISCONNECT_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		//{{ 2012. 12. 17	박세훈	잭팟 이벤트
		//}}
		SendToGlobalServer( EGB_UNIT_DISCONNECT_NOT, kPacketNot );
		//}}
	}

	{
		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_DESELECT_CHAR );

		const int iDeselectCount = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_DESELECT_CHAR );
		if( iDeselectCount == 1 )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_DESELECT );
		}
		else if( iDeselectCount == 2 )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_SECOND_DESELECT );
		}
		//}}
	}

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}
//#endif SERV_DESELECT_UNIT_REFAC
//}}

//{{ 2011. 10. 14	최육사	아이템 사용 DB 업데이트 수정
void KGSUser::OnUseItemInInventory( IN const UidType iItemUID
								   //{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
//#ifdef SERV_PET_AUTO_LOOTING
								   , IN const __int64 iTempCode
//#endif SERV_PET_AUTO_LOOTING
								   //}}
								   , IN const bool bNotDeleteItem /*= false*/
								   , IN const int iWarpPointMapID /*= 0*/
								   )
{
	const int iUsedItemID = m_kInventory.GetItemID( iItemUID );

	//////////////////////////////////////////////////////////////////////////
	// 인벤토리에서 아이템 사용!
	KEGS_USE_ITEM_IN_INVENTORY_ACK kAck;
	KDBE_USE_ITEM_IN_INVENTORY_REQ kPacketToDB;

	if( m_kInventory.UseItemInInventory( iItemUID, kPacketToDB.m_vecUpdatedInventorySlot, bNotDeleteItem ) == false )
	{
		kAck.m_iOK = NetError::GetLastError();
		SendPacket( EGS_USE_ITEM_IN_INVENTORY_ACK, kAck );
		return;
	}

	//{{ 2012. 10. 31	박세훈	코보 익스프레스 티켓 추가
	if( ( iUsedItemID == CXSLItem::SI_USE_FREE_BY_FIELD ) ||
		( iUsedItemID == CXSLItem::SI_USE_COBO_EXPRESS_TICKET )
		)
	{
		if( ( GetMapID() == iWarpPointMapID )
			|| ( SiCXSLMapData()->CheckCOBOExpressTicketMapID( GetMapID() ) == 0 )
			|| ( SiCXSLMapData()->CheckCOBOExpressTicketMapID( iWarpPointMapID ) == 0 )
			)
		{
			KEGS_USE_ITEM_IN_INVENTORY_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_UNKNOWN;
			SendPacket( EGS_USE_ITEM_IN_INVENTORY_ACK, kPacket );
			return;
		}
		else if( iUsedItemID == CXSLItem::SI_USE_COBO_EXPRESS_TICKET )
		{
			int iCOBOExpressTicketCost = SiCXSLMapData()->ComputeCOBOExpressTicketCost( GetMapID(), iWarpPointMapID, GetLevel() );
			if( GetED() < iCOBOExpressTicketCost )
			{
				KEGS_USE_ITEM_IN_INVENTORY_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_USE_ITEM_IN_INVENTORY_08;
				SendPacket( EGS_USE_ITEM_IN_INVENTORY_ACK, kPacket );
				return;
			}

			//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
//#ifdef SERV_SEPARATE_DUNGEON_FIELD_ED_LOG// 작업날짜: 2013-06-17	// 박세훈
			const int iBeforeED = m_iED;
//#endif // SERV_SEPARATE_DUNGEON_FIELD_ED_LOG

			DecreaseED( iCOBOExpressTicketCost );

//#ifdef SERV_SEPARATE_DUNGEON_FIELD_ED_LOG// 작업날짜: 2013-06-17	// 박세훈
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_COBO_EXPRESS_TICKET_COST, iBeforeED );
//#endif // SERV_SEPARATE_DUNGEON_FIELD_ED_LOG

			// ED 사용량 통계
			KStatisticsKey kKey;
			kKey.m_vecIntKey.push_back( 0 );
			KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MCoboExpressTicketED, iCOBOExpressTicketCost );

			// 유저 통계
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_MCoboExpressTicketED, iCOBOExpressTicketCost );
			//}}
		}
	}
	kPacketToDB.m_iED = GetED();
	//}}

	//{{ 버프 관리 시스템 - 김민성
	std::vector<KBuffInfo> vecActiveBuff;
	std::vector<int> vecDeactiveBuff;

	// 아이템 사용 버프
	CTime tCurr = CTime::GetCurrentTime();
	ActivateItemBuff( iUsedItemID, vecActiveBuff, vecDeactiveBuff, tCurr );

	if( vecActiveBuff.empty() == false || vecDeactiveBuff.empty() == false )
	{
		// 로그인 서버 버프 매니저에 업데이트
		KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
		kReq.m_iUnitUID = GetCharUID();
		kReq.m_vecActivateBuff = vecActiveBuff;
		kReq.m_vecDeActivateBuff = vecDeactiveBuff;
		SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
	}
	//}

	kPacketToDB.m_iUsedItemID = iUsedItemID;
	kPacketToDB.m_iWarpPointMapID = iWarpPointMapID;
	kPacketToDB.m_iUnitUID = GetCharUID();
	//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
//#ifdef SERV_PET_AUTO_LOOTING
	kPacketToDB.m_iTempCode	= iTempCode;
//#endif SERV_PET_AUTO_LOOTING
	//}}
	m_kInventory.FlushQuantityChange( kPacketToDB.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.FlushDeletedItem( kPacketToDB.m_kItemQuantityUpdate.m_vecDeleted );
	SendToGameDB( DBE_USE_ITEM_IN_INVENTORY_REQ, kPacketToDB );
}
//}}

bool KGSUser::CheckCharLevelUp()
{
	const u_char ucNowLevel = GetLevel();

	m_ucLevel = SiKExpTable()->CheckLevelUp( ucNowLevel, m_kEXP.GetExp() );

	if( ucNowLevel < m_ucLevel )
	{
		//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
		m_kUserAccountQuestManager.Handler_CharaterLevelUp_Account( GetThisPtr<KGSUser>() );
		//}}
		//레벨업한 경우 파티정보 업데이트.
		SendUpdatePartyUnitInfo();


		// 체험ID 기능 제한
		if( !IsGuestUser() )
		{
			// 추천인 보상 부분
			if( m_kUserRecommendManager.GetRecommendedUnitUID() > 0 )
			{
				//{{ 2010. 01. 11  최육사	추천인리스트
				SendUpdateRecommendUserInfo( KRecommendUserInfo::RUIT_LEVEL_UP );
				//}}

				int iRewardID = SiKRecommendUserTable()->GetRecommendUserReward( GetLevel() );
				if( iRewardID != 0 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID	= GetCharUID();
					kPacketToDB.m_iToUnitUID	= m_kUserRecommendManager.GetRecommendedUnitUID(); // 추천인 UnitUID
					kPacketToDB.m_iRewardType	= KPostItemInfo::LT_RECOMMEND; // 보상 타입
					kPacketToDB.m_iRewardID		= iRewardID;

					CStringW cwstrMessage;
					cwstrMessage.Format( L"%s;%d", GetCharName().c_str(), GetLevel() );
					kPacketToDB.m_wstrMessage = static_cast<LPCTSTR>(cwstrMessage);

					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
				//}

			}

			//제자리스트에 있다면 정보업데이트.
			if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_STUDENT &&
				m_kUserTutorialInfo.IsExistbyTutorData() == false )
			{
				if( SiKTutorialManager()->IsExistbylist( GetCharUID() ) == true )
				{
					SiKTutorialManager()->SetLevelData( GetCharUID(), GetLevel() );

					//제작의 마지막 레벨이 되었으면 리스트에서 삭제해준다.
					if( GetLevel() >= KTutorialManager::UNIT_LEVEL_STUDENT_MAX  &&
						ucNowLevel < KTutorialManager::UNIT_LEVEL_STUDENT_MAX )
					{
						SiKTutorialManager()->DeleteUnit( GetCharUID() );
					}
				}
			}

			//080408.hoons. 사제 보상 부분.
			m_kUserTutorialInfo.ProcCharLevelUP( ucNowLevel, GetThisPtr<KGSUser>() );
		}

		//{{ 2010. 03. 22  최육사	기술의 노트
		// 현재 기술의 노트를 보유하고 있다면?
		if( m_kSkillTree.IsHaveSkillNote() == true )
		{
			char cSkillNoteMaxPageNum = 0;
			if( m_kSkillTree.GetExpandSkillNotePage( GetLevel(), cSkillNoteMaxPageNum ) == true )
			{
				// 새로 얻은 페이지수가 현재보다 크다면 DB로 확장하러 가자!
				if( cSkillNoteMaxPageNum > m_kSkillTree.GetSkillNoteMaxPageNum() )
				{
					KDBE_EXPAND_SKILL_NOTE_PAGE_REQ kPacketToDB;
					kPacketToDB.m_iUnitUID = GetCharUID();
					kPacketToDB.m_cExpandedMaxPageNum = cSkillNoteMaxPageNum;
					kPacketToDB.m_cRollBackMaxPageNum = m_kSkillTree.GetSkillNoteMaxPageNum();
					SendToGameDB( DBE_EXPAND_SKILL_NOTE_PAGE_REQ, kPacketToDB );
				}
			}
			else
			{
				START_LOG( cerr, L"기술의 노트 레벨별 페이지 정보 얻기 실패!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOGc( GetLevel() )
					<< END_LOG;
			}
		}
		//}}


		if( m_ucLevel == SiKGameSysVal()->GetLimitsLevel() )
		{
			// DB로 보상 정보를 보낸다.
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
			kPacketToDB.m_iRewardID	   = _CONST_PRESENT_SKILL_INIT_ITEM_INT_::iReachLimitLevel;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}

		//{{ 2012. 12. 19  아라 캐릭터 레벨업 이벤트(15, 30, 40 레벨달성 시) - 김민성
		//}}

		IF_EVENT_ENABLED( CEI_ARA_NEW_FIRST_CLASS_EVENT )
		{
			if(	GetUnitClass() == CXSLUnit::UC_ARA_LITTLE_DEVIL )
			{
				if( ucNowLevel < 20 && GetLevel() >= 20 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iLevelUpReward1;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 25 && GetLevel() >= 25 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iLevelUpReward2;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 30 && GetLevel() >= 30 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iLevelUpReward3;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 35 && GetLevel() >= 35 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iLevelUpReward4;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}
		}

		IF_EVENT_ENABLED( CEI_EVENT_LEVEL_UP_ARA )
		{
			if( GetUnitType() == CXSLUnit::UT_ARA )
			{
				if( ucNowLevel < 3  &&  GetLevel() >= 3 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = 1545; // 마나 이터
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 7  &&  GetLevel() >= 7 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = 1546; // 특수 액세서리 3일권 큐브
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 11  &&  GetLevel() >= 11 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = 1547; // 엘의 나무 씨앗 10개
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( ucNowLevel < 15  &&  GetLevel() >= 15 )
				{
					// DB로 보상 정보를 보낸다.
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
					kPacketToDB.m_iRewardID	   = 1548; // 그노시스 축복(10SP) 15일권
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}
		}

		IF_EVENT_ENABLED( CEI_INT_ENTRY_POINT_LEVEL_UP_EVENT )
		{
			int iLevelUpReward = 0;
			if( ucNowLevel < 5 && GetLevel() >= 5 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i5LevelUpReward;
			}
			else if ( ucNowLevel < 10 && GetLevel() >= 10 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i10LevelUpReward;
			}
			else if ( ucNowLevel < 15 && GetLevel() >= 15 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i15LevelUpReward;
			}
			else if ( ucNowLevel < 20 && GetLevel() >= 20 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i20LevelUpReward;
			}
			else if ( ucNowLevel < 25 && GetLevel() >= 25 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i25LevelUpReward;
			}
			else if ( ucNowLevel < 30 && GetLevel() >= 30 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i30LevelUpReward;
			}
			else if ( ucNowLevel < 35 && GetLevel() >= 35 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i35LevelUpReward;
			}
			else if ( ucNowLevel < 40 && GetLevel() >= 40 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i40LevelUpReward;
			}
			else if ( ucNowLevel < 45 && GetLevel() >= 45 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i45LevelUpReward;
			}
			else if ( ucNowLevel < 50 && GetLevel() >= 50 )
			{
				iLevelUpReward = _CONST_INT_ENTRY_POINT_LEVEL_UP_EVENT_REWARD_ID::i50LevelUpReward;
			}
			if(iLevelUpReward != 0)
			{
				// DB로 보상 정보를 보낸다.
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
				kPacketToDB.m_iRewardID	   = iLevelUpReward;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}



		}


		//{{ 2009. 5. 18  최육사	10레벨 되었을때 보상
		//if( ucNowLevel < 10  &&  GetLevel() >= 10 )
		//{
		//	// DB로 보상 정보를 보낸다.
		//	KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//	kPacketToDB.m_iFromUnitUID = GetCharUID();
		//	kPacketToDB.m_iToUnitUID   = GetCharUID();
		//	kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT; // 보상 타입
		//	kPacketToDB.m_iRewardID	   = 136; // 코보 특제 무기 큐브 <-- RewardTable.lua에 등록된 id임
		//	SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//}
		//}}

		//////////////////////////////////////////////////////////////////////////
		//{{ 2009. 12. 22  최육사	신서버이벤트  "2009-12-23 ~ 2010-01-27"
		//if( KBaseServer::GetKObj()->GetServerGroupID() == SEnum::SGI_GAIA )
		//{
		//	if( ucNowLevel < 10  &&  GetLevel() >= 10 )
		//	{
		//		// DB로 보상 정보를 보낸다.
		//		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//		kPacketToDB.m_iFromUnitUID = GetCharUID();
		//		kPacketToDB.m_iToUnitUID   = GetCharUID();
		//		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
		//		kPacketToDB.m_iRewardID	   = 189; // 코보 여행자 보급 물품(10레벨)
		//		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//	}

		//	if( ucNowLevel < 20  &&  GetLevel() >= 20 )
		//	{
		//		// DB로 보상 정보를 보낸다.
		//		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//		kPacketToDB.m_iFromUnitUID = GetCharUID();
		//		kPacketToDB.m_iToUnitUID   = GetCharUID();
		//		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
		//		kPacketToDB.m_iRewardID	   = 190; // 코보 여행자 보급 물품(20레벨), 길드 창단 허가서
		//		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//	}

		//	if( ucNowLevel < 30  &&  GetLevel() >= 30 )
		//	{
		//		// DB로 보상 정보를 보낸다.
		//		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//		kPacketToDB.m_iFromUnitUID = GetCharUID();
		//		kPacketToDB.m_iToUnitUID   = GetCharUID();
		//		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
		//		kPacketToDB.m_iRewardID	   = 191; // 코보 여행자 보급 물품(30레벨)
		//		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//	}

		//	if( ucNowLevel < 40  &&  GetLevel() >= 40 )
		//	{
		//		// DB로 보상 정보를 보낸다.
		//		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//		kPacketToDB.m_iFromUnitUID = GetCharUID();
		//		kPacketToDB.m_iToUnitUID   = GetCharUID();
		//		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
		//		kPacketToDB.m_iRewardID	   = 192; // 코보 여행자 보급 물품(40레벨)
		//		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//	}

		//	if( ucNowLevel < 50  &&  GetLevel() >= 50 )
		//	{
		//		// DB로 보상 정보를 보낸다.
		//		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		//		kPacketToDB.m_iFromUnitUID = GetCharUID();
		//		kPacketToDB.m_iToUnitUID   = GetCharUID();
		//		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
		//		kPacketToDB.m_iRewardID	   = 193; // 코보 여행자 보급 물품(50레벨)
		//		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		//	}
		//}
		//}}
		//////////////////////////////////////////////////////////////////////////

		//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
		//}}

		//{{ 2009. 6. 29  최육사	만렙 달성 공지
		if( GetLevel() == SiKGameSysVal()->GetLimitsLevel()  &&
			GetAuthLevel() < SEnum::UAL_GM )
		{
			KEGS_NOTIFY_MSG_NOT	kPacket;
			kPacket.m_cNotifyType = KEGS_NOTIFY_MSG_NOT::NT_MAX_LEVEL_UP;
			kPacket.m_Count = 1;

			CStringW cwstrMessage;
			cwstrMessage.Format( L"%s;%d", GetCharName().c_str(), GetLevel() );
			kPacket.m_wstrMSG = static_cast<LPCTSTR>(cwstrMessage);

			SendToCnServer( ECN_NOTIFY_MSG_NOT, kPacket );

			//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_FULL_LEVEL );
			//}}
		}
		//}}


		KStatTable::KUnitStatInfo	sUnitStat;
		if( SiKStatTable()->GetUnitStat( GetUnitClass(), GetLevel(), sUnitStat ) == true )
		{
			m_kStat.m_iBaseHP		= sUnitStat.m_uiHP;
			m_kStat.m_iAtkPhysic	= sUnitStat.m_usAtkPhysic;
			m_kStat.m_iAtkMagic		= sUnitStat.m_usAtkMagic;
			m_kStat.m_iDefPhysic	= sUnitStat.m_usDefPhysic;
			m_kStat.m_iDefMagic		= sUnitStat.m_usDefMagic;

			// 레벨업 하면 sp도 같이 획득한다
			for( u_char ucLevel = ucNowLevel + 1; ucLevel <= GetLevel(); ++ucLevel )
			{
				int iSPInc = 0;
				if( SiCXSLSkillTree()->GetCalcLevelUpIncreaseSkillPoint( ucLevel, iSPInc ) == true )
				{
					m_kSkillTree.AddSPointEveryPage( iSPInc );

				}
				else
				{
					START_LOG_WITH_NAME( cerr )
						<< END_LOG;
				}
			}

            // 레벨업에 대한 로그를 DB에 남긴다.
            KDBE_CHARACTER_LEVELUP_NOT kNot;
            kNot.m_nCharUID = GetCharUID();
            kNot.m_ucLevelBefore = ucNowLevel;
            kNot.m_ucLevelAfter = GetLevel();
            SendToLogDB( DBE_CHARACTER_LEVELUP_NOT, kNot );

			//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_LEVEL_UP );
			//}}

				//{{ 2010. 11. 17	최육사	오토핵 검증 기능
				if( m_kUserAbuserManager.IsAutoHackLevelUpUser( GetLevel()
					, GetCharUID()
					) == true )
				{
					if( GetAuthLevel() >= SEnum::UAL_GM )
					{
						START_LOG( cout, L"레벨업 시간 간격이 1분 미만으로 체크되었으나 계정권한이 운영자급 이상이므로 아무런 처리를 하지 않습니다!" )
							<< BUILD_LOG( GetName() )
							<< BUILD_LOG( GetUID() )
							<< BUILD_LOGc( GetAuthLevel() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOG( GetCharUID() );
					}
					else
					{
						START_LOG( cout, L"레벨업 시간 간격이 1분 미만으로 체크되어 해당 유저를 접속 종료 처리함!" )
							<< BUILD_LOG( GetName() )
							<< BUILD_LOG( GetUID() )
							<< BUILD_LOGc( GetAuthLevel() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetED() )
							<< BUILD_LOGc( GetLevel() )
							<< BUILD_LOG( GetEXP() )
							<< BUILD_LOG( GetStateIDString() )
							<< BUILD_LOG( GetRoomUID() )
							<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) );

						if( SiKAbuserLogManager()->IsAbuserCheckLevelUp() == true )
						{
							START_LOG( cout, L"해킹 유저 등록 및 계정 접속 차단 합니다!" )
								<< BUILD_LOG( GetName() )
								<< BUILD_LOG( GetUID() )
								<< BUILD_LOGc( GetAuthLevel() )
								<< BUILD_LOG( GetCharName() )
								<< BUILD_LOG( GetCharUID() );

							// RejectedUser에 등록하기!
							KELG_REG_REJECTED_USER_NOT kPacketNot;
							kPacketNot.m_iUserUID = GetUID();
							SendToLoginServer( ELG_REG_REJECTED_USER_NOT, kPacketNot );

							if( m_cHackingUserType == SEnum::HUT_NORMAL )
							{
								START_LOG( clog, L"해킹 유저 신고.!" )
									<< BUILD_LOG( GetUID() )
									<< BUILD_LOG( GetName() )
									<< BUILD_LOG( GetCharName() )
									<< BUILD_LOGc( m_cHackingUserType )
									;

								// 핵유저로 체크!
								m_cHackingUserType = SEnum::HUT_DISAGREE_HACK_USER;

								// AccountDB에도 등록하자!
								SendToAccountDB( DBE_REPORT_HACK_USER_NOT, GetUID() );
							}

							//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
							//{{ 2012. 03. 20	김민성		Game Server 에서 종료 사유별 해킹 정보 체크
							KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
							kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_FAST_LEVEL_UP;
							SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
							//}}
							//}}

							SetDisconnectReason( KStatistics::eSIColDR_Client_Hacking );
							ReserveDestroy();
						}

						//KDBE_SEND_PHONE_MSG_NOT kPacketNot;
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-4118-3867" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-5014-1853" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-8584-3123" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-4342-0365" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-7753-3789" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-8421-1075" ) );
						//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-9317-0790" ) );
						//kPacketNot.m_wstrSMSMessage += L"레업간격발견! UserUID : ";
						//kPacketNot.m_wstrSMSMessage += x2::str( x2::wformat( L"%d" ) % GetUID() );
						//kPacketNot.m_wstrSMSMessage += L", UnitUID : ";
						//kPacketNot.m_wstrSMSMessage += x2::str( x2::wformat( L"%d" ) % GetCharUID() );
						//kPacketNot.m_wstrSMSMessage += L", NickName : ";
						//kPacketNot.m_wstrSMSMessage += GetCharName();
						//SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
					}
				}
				//}}

			// 체험ID 기능 제한
			if( IsGuestUser() )
			{
				// 체험ID 레벨 제한
				if( GetLevel() > SiKGameSysVal()->GetGuestUserLimitLevel() )
				{
					SendID( EGS_GUEST_USER_LIMIT_LEVEL_NOT );
				}
			}

			// 새로운 미션 검사
			m_kUserTitleManager.CheckNewMission( GetThisPtr<KGSUser>() );

			//{{ 2009. 10. 8  최육사	길드
			SendUpdateGuildMemberInfo();
			//}}

			//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
			KEGS_UPDATE_UNIT_INFO_NOT kPacketUnitInfo;
			GetUnitInfo( kPacketUnitInfo.m_kUnitInfo );
			SendPacket( EGS_UPDATE_UNIT_INFO_NOT, kPacketUnitInfo );
			//}}

			//{{ QUEST 개편 - 김민성
			m_kUserQuestManager.CheckLimitLevelQuest( GetThisPtr<KGSUser>() );
			m_kUserQuestManager.CheckEventQuest( GetThisPtr<KGSUser>() );
			//}}

			// 실시간 레벨업
			if( GetStateID() == KGSFSM::S_ROOM  &&  GetRoomUID() != 0 )
			{
				switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
				{
				case CXSLRoom::RT_DUNGEON:
					//{{ 2012. 02. 16	최육사	배틀필드 시스템
				case CXSLRoom::RT_BATTLE_FIELD:
					//}}
					{
						KERM_CHAR_LEVEL_UP_NOT kPacketNot;
						kPacketNot.m_kBaseStat = m_kStat;
						GetRoomUserInfo( kPacketNot.m_kRoomUserInfo, CXSLRoom::RT_DUNGEON );
						SendToCnRoom( ERM_CHAR_LEVEL_UP_NOT, kPacketNot );
					}
					break;

				default:
					{
						// 클라이언트로 바로 보내자!
						KEGS_CHAR_LEVEL_UP_NOT kPacketNot;
						kPacketNot.m_iUnitUID = GetCharUID();
						kPacketNot.m_ucLevel = GetLevel();
						kPacketNot.m_kBaseStat = m_kStat;
						SendPacket( EGS_CHAR_LEVEL_UP_NOT, kPacketNot );
					}
					break;
				}
			}
			else
			{
				// 클라이언트로 바로 보내자!
				KEGS_CHAR_LEVEL_UP_NOT kPacketNot;
				kPacketNot.m_iUnitUID = GetCharUID();
				kPacketNot.m_ucLevel = GetLevel();
				kPacketNot.m_kBaseStat = m_kStat;
				SendPacket( EGS_CHAR_LEVEL_UP_NOT, kPacketNot );
			}

			//{{ 2011. 12. 19	최육사	캐릭터 레벨업 실시간 DB적용
			WriteUnitInfoToDB( false );

			//{{ 2012. 09. 22	박세훈	빙고 이벤트
			//}}

			//}}
			//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
			//}}
			return true;
		}
        else
		{
			START_LOG_WITH_NAME( cerr )
				<< L"UNIT STAT DATA 파싱 실패.!(이러면 안되는데)" << dbg::endl
				<< BUILD_LOGc( GetUnitClass() )
				<< BUILD_LOG( GetLevel() )
				<< END_LOG;

			return false;
		}
	}

	return false;
}

bool KGSUser::ResetStat()
{
	//전직후 전직한 캐릭터 스탯으로 교체하기위한 함수.
	KStatTable::KUnitStatInfo	sUnitStat;
	if( SiKStatTable()->GetUnitStat( GetUnitClass(), m_ucLevel, sUnitStat ) == true )
	{
		m_kStat.m_iBaseHP		= sUnitStat.m_uiHP;
		m_kStat.m_iAtkPhysic	= sUnitStat.m_usAtkPhysic;
		m_kStat.m_iAtkMagic		= sUnitStat.m_usAtkMagic;
		m_kStat.m_iDefPhysic	= sUnitStat.m_usDefPhysic;
		m_kStat.m_iDefMagic		= sUnitStat.m_usDefMagic;

		return true;
	}
	else
	{
		START_LOG_WITH_NAME( cerr )
			<< L"UNIT STAT DATA 파싱 실패.!(이러면 안되는데)" << dbg::endl
			<< BUILD_LOGc( GetUnitClass() )
			<< BUILD_LOG( m_ucLevel )
			<< END_LOG;

		return false;
	}
}

//{{ 2010. 8. 18	최육사	서버 코드 정리
void KGSUser::ClearTempInventory()
{
	// 임시 인벤토리에 아이템이 남아있는지 확인!
	if( m_kTempInventory.IsEmpty() == true )
		return;

	// 임시 인벤을 비우자!
	m_kTempInventory.Clear();

	// 클라이언트에도 알려주자!
	SendID( EGS_CLEAR_TEMP_INVENTORY_NOT );
}
//}}

char KGSUser::GetUnitState()
{
	char cState = static_cast<char>(CXSLUnit::CUS_FIELD_MAP);

	if( GetRoomUID() != 0 )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_SQUARE:
			cState = static_cast<char>(CXSLUnit::CUS_MARKET);
			break;
		case CXSLRoom::RT_TRAININGCENTER:
			cState = static_cast<char>(CXSLUnit::CUS_TC_PLAY);
			break;
		case CXSLRoom::RT_DUNGEON:
			cState = static_cast<char>(CXSLUnit::CUS_DUNGEON_PLAY);
			break;
		case CXSLRoom::RT_PVP:
			{
				KRoomInfoPtr spRoomInfo = SiKRoomListManager()->GetRoomInfo( GetRoomUID() );
				if( spRoomInfo != NULL )
				{
					switch( spRoomInfo->m_RoomState )
					{
					case CXSLRoom::RS_WAIT: cState = static_cast<char>(CXSLUnit::CUS_PVP_WAIT);
						break;
					case CXSLRoom::RS_PLAY: cState = static_cast<char>(CXSLUnit::CUS_PVP_PLAY);
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		//{{ 2011. 07. 29	최육사	대전 개편
		if( GetRoomListID() != 0  &&  GetRoomListID() != SiKRoomListManager()->GetPvpOffcialRoomListID() )
		//}}
			cState = static_cast<char>(CXSLUnit::CUS_PVP_LOBBY);
	}

	return cState;
}

bool KGSUser::GetPVPChannelClass( OUT int& iPvpChannelClass ) const
{
	iPvpChannelClass = KPVPChannelInfo::PCC_NONE; // default

	// RoomListID도 없으면서 여기로 들어오나? 체크!
	if( GetRoomListID() == 0 )
	{
		return false;
	}

	// RoomListID로 대전 채널 정보 얻기!
	const int iPvpChannelClassByRoomListID = SiKRoomListManager()->GetPVPChannelClassByRoomListID( GetRoomListID() );
	if( iPvpChannelClassByRoomListID == KPVPChannelInfo::PCC_NONE )
	{
		START_LOG( cerr, L"존재 하지 않는 대전 채널의 RoomListID값을 가지고 있습니다! 있을 수 없는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetRoomListID() )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( GetStateIDString() )
			<< END_LOG;

		return false;
	}

	// 리턴값 업데이트
	iPvpChannelClass = iPvpChannelClassByRoomListID;

	// RoomUID값이 있다면 대전 채널 정보를 검증하자!
	if( GetRoomUID() > 0 )
	{
		const int iPvpChannelClassByRoomUID = SiKRoomListManager()->GetPVPChannelClassByRoomUID( GetRoomUID() );
		if( iPvpChannelClassByRoomUID != KPVPChannelInfo::PCC_NONE )
		{
			if( iPvpChannelClassByRoomUID != iPvpChannelClassByRoomListID )
			{
				START_LOG( cerr, L"RoomListID로 얻은 대전채널과 RoomUID로 얻은 대전 채널이 다르다? 있을 수 없는 에러!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetRoomListID() )
					<< BUILD_LOG( GetRoomUID() )
					<< BUILD_LOG( GetStateIDString() )
					<< BUILD_LOG( iPvpChannelClassByRoomListID )
					<< BUILD_LOG( iPvpChannelClassByRoomUID )
					<< END_LOG;

				return false;
			}
		}
	}

	return true;
}

//{{ 2011. 12. 20	최육사	 배틀필드 시스템
//}}

void KGSUser::GetPartyUserInfo( KPartyUserInfo& kInfo )
{
	//////////////////////////////////////////////////////////////////////////
	//{{ 2010. 02. 17  최육사	채널파티통합
	//////////////////////////////////////////////////////////////////////////

	kInfo.m_iGSUID			= KBaseServer::GetKObj()->GetUID();
	kInfo.m_iChannelID		= SiKChannelManager()->GetChannelID();
	kInfo.m_iFieldUID		= GetFieldUID();
	kInfo.m_iUnitUID		= GetCharUID();
	kInfo.m_cUnitClass		= GetUnitClass();
	kInfo.m_wstrNickName	= GetCharName();
	kInfo.m_ucLevel			= GetLevel();
	kInfo.m_bIsGuestUser	= IsGuestUser();
	kInfo.m_cState			= GetUnitState();
	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	CTime tComeBackBuffEnd	= m_kComeBackUserInfo.GetComeBackBuffEnd();
	kInfo.m_bComeBack		= m_kComeBackUserInfo.GetIsComeBackUser();
	kInfo.m_wsrtComeBackEnd = tComeBackBuffEnd.Format( _T( "%Y-%m-%d %H:%M:%S" ) );
	//}}
	//{{ 2011. 07. 11	최육사	대전 개편
	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	kInfo.m_cRank				= GetPvpRankForClient();
	kInfo.m_iOfficialMatchCnt	= GetOfficialMatchCount();
	//}}
	kInfo.m_iRating			= GetRating();
	kInfo.m_iRPoint			= GetRPoint();
	//}}
	if( kInfo.m_cState == CXSLUnit::CUS_FIELD_MAP )
	{
		kInfo.m_iStateCode = GetMapID();
	}
	//{{ 2012. 05. 29	최육사	배틀필드 시스템
	m_kUserUnitManager.GetPartyMemberStatus( kInfo.m_kGamePlayStatus );
	//}}
	//{{ 핑 상태체크 호스트 변경 - 김민성
	kInfo.m_wstrIP = GetP2PIP();
	kInfo.m_usPort = static_cast<u_short>(GetP2PPort());
	//}

	//{{ 2013. 1. 8	박세훈	Merge 공인IP 연결 실패시 내부IP로 시도( 박진웅 )
//#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP
	kInfo.m_wstrInternalIP = GetP2PInternalIP();
	kInfo.m_usInternalPort = GetP2PInternalPort();

	START_LOG( clog, L"IP, 포트 확인" )
		<< BUILD_LOG( kInfo.m_wstrInternalIP )
		<< BUILD_LOG( kInfo.m_usInternalPort )
		<< END_LOG;
//#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
	//}}

	//////////////////////////////////////////////////////////////////////////
	//}}
	//////////////////////////////////////////////////////////////////////////
}

void KGSUser::SendUpdatePartyUnitInfo( bool bChangeField /* = false  */)
{
	if( GetPartyUID() != 0 )
	{
		//KERM_UPDATE_PARTY_USER_INFO_NOT kInfo;

		//kInfo.m_cState		= GetUnitState();
		//kInfo.m_iStateCode	= 0;
		//kInfo.m_cUnitClass	= GetUnitClass();
		//kInfo.m_ucLevel		= GetLevel();

		//if( kInfo.m_cState == CXSLUnit::CUS_FIELD_MAP )
		//	kInfo.m_iStateCode = GetMapID();

		//_SendToCnRoom( m_iPartyUID, ERM_UPDATE_PARTY_USER_INFO_NOT, kInfo );
		//{{ 2010. 02. 22  최육사	채널 파티 통합
		if( SiKPartyListManager()->E_UpdatePartyUserInfo( GetThisPtr<KGSUser>(), bChangeField ) == false )
		//}}
		{
			START_LOG( cerr, L"파티유저 정보 업데이트 실패." )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
		}
	}
}

void KGSUser::SendLeaveParty( int iReason )
{
	//{{ 2010. 03. 10  최육사	채널 통합 파티
	if( GetPartyUID() > 0  ||  GetCandidatePartyUID() > 0 )
	//}}
	{
		KEGS_LEAVE_PARTY_REQ kPacket;
		kPacket.m_iReason = iReason;

		//{{ 2010. 02. 22  최육사	채널 파티 통합
		if( SiKPartyListManager()->E_LeavePartyReq( GetThisPtr<KGSUser>(), kPacket ) == false )
		//}}
		{
			START_LOG( cerr, L"파티 나가기 실패.!" )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( NetError::GetErrStr(iReason) )
				<< END_LOG;
		}
	}
}

void KGSUser::SendLeaveField( int iReason )
{
	if( GetFieldUID() > 0 )
	{
		KERM_LEAVE_FIELD_REQ kPacket;
		kPacket.m_iReason = iReason;
		_SendToGSField( GetFieldUID(), ERM_LEAVE_FIELD_REQ, kPacket );

		SetFieldUID( 0 );
	}
}

void KGSUser::SendLeavePVPLobby()
{
	if( GetRoomListID() > 0 )
	{
		SendUpdatePVPUserInfo( KCommunityUserInfo::US_LEAVE_LOBBY );

		SetRoomListID( 0 );

		//{{ 2009. 10. 23  최육사	길드
		SendUpdateGuildMemberInfo();
		//}}
	}
}

//{{ 2011. 07. 11	최육사	대전 개편
void KGSUser::SendLeavePVPMatch( IN const int iReason )
{
	if( m_kUserPvpManager.IsRequestMatchMaking() == true  ||  GetMatchUID() != 0 )
	{
		KEGB_CANCEL_MATCH_MAKING_REQ kPacketReq;
		kPacketReq.m_iMatchUID = GetMatchUID();
		kPacketReq.m_iPartyUID = GetPartyUID();
		kPacketReq.m_iReason = iReason;
		kPacketReq.m_bSendBySystem = true;
		SendToMatch( EGB_CANCEL_MATCH_MAKING_REQ, kPacketReq );

		m_kUserPvpManager.SetMatchUID( 0 );
	}
}
//}}

//{{ 2012. 02. 10	최육사	배틀필드 시스템
void KGSUser::SendLeaveAutoParty( IN const int iReason )
{
	if( m_kUserDungeonManager.GetAutoPartyUID() != 0  ||  m_kUserDungeonManager.GetAutoPartyWaitNumber() != 0 )
	{
		// 일단은 중복 적용을 피하기 위해 기존에 저장되어있던것은 취소 시키자!
		KEGB_CANCEL_AUTO_PARTY_MAKING_REQ kPacketReq;
		kPacketReq.m_iAutoPartyUID = m_kUserDungeonManager.GetAutoPartyUID();
		kPacketReq.m_iAutoPartyWaitNumber = m_kUserDungeonManager.GetAutoPartyWaitNumber();
		kPacketReq.m_iReason = iReason;
		kPacketReq.m_bSendBySystem = true;
		SendToAutoParty( EGB_CANCEL_AUTO_PARTY_MAKING_REQ, kPacketReq );

		m_kUserDungeonManager.SetAutoPartyUID( 0 );
		m_kUserDungeonManager.SetAutoPartyWaitNumber( 0 );
	}
}
//}}

//{{ 2012. 03. 05	최육사	배틀필드 시스템
void KGSUser::SetRoomUID( IN const UidType iUID )
{
	// 방 정보 업데이트
	m_iBeforeRoomUID = GetRoomUID(); // 이전 방 정보는 따로 저장해두자! ( 배틀필드에서 원래 필드로 돌아갈때 사용 )
	m_iRoomUID = iUID;

	// 파티 소속이라면 방 입장에 대한 정보를 업데이트 하자!
	if( GetPartyUID() != 0 )
	{
		KEPM_UPDATE_PARTY_USER_JOIN_ROOM_INFO_NOT kPacketNot;
		kPacketNot.m_kJoinRoomInfo.m_iRoomUID = GetRoomUID();
		kPacketNot.m_kJoinRoomInfo.m_iBattleFieldID = GetMapID();
		SendToCnParty( EPM_UPDATE_PARTY_USER_JOIN_ROOM_INFO_NOT, kPacketNot );
	}

	// 예외 처리!
	if( GetBeforeRoomUID() == 0 )
	{
		if( iUID == 0 )
		{
			START_LOG( cerr, L"방에서 이미 나갔는데 또 나가라고 정보가 왔네? 이상한 오류다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;
		}
		else
		{
            // 방에 입장
		}
	}
	else
	{
		if( iUID == 0 )
		{
			switch( CXSLRoom::GetRoomType( GetBeforeRoomUID() ) )
			{
			case CXSLRoom::RT_DUNGEON:
				{
					//{{ 버프 관리 시스템 - 김민성
					// 던전방에 있다가 이탈하는 경우!
					std::vector<int> vecDeleteBuff;
					//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성

					std::vector< KBuffInfo > vecActivateBuff;	// 해외팀 위치 변경
					m_kUserBuffManager.OnLeaveDungeonRoom( GetThisPtr<KGSUser>(), vecActivateBuff, vecDeleteBuff, false );
					//}

					if( vecDeleteBuff.empty() == false )
					{
						// 로그인 서버 버프 매니저에 업데이트
						KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
						kReq.m_iUnitUID = GetCharUID();
						kReq.m_vecDeActivateBuff = vecDeleteBuff;
						kReq.m_vecActivateBuff = vecActivateBuff;
						SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
					}
					//}}
				}
				break;

			case CXSLRoom::RT_BATTLE_FIELD:
				{
					std::vector<int> vecDeleteBuff;
					std::vector< KBuffInfo > vecActivateBuff;
					m_kUserBuffManager.OnLeaveBattleField( GetThisPtr<KGSUser>(), vecActivateBuff, vecDeleteBuff );

					if( vecDeleteBuff.empty() == false )
					{
						// 로그인 서버 버프 매니저에 업데이트
						KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
						kReq.m_iUnitUID = GetCharUID();
						kReq.m_vecDeActivateBuff = vecDeleteBuff;
						kReq.m_vecActivateBuff = vecActivateBuff;
						SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
					}
				}break;
			default:
				break;
			}
		}
		else
		{
			START_LOG( cerr, L"이미 방에 입장해 있는데 또 입장하려고 하네? 이러면 안되!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetStateIDString() )
				<< BUILD_LOG( GetBeforeRoomUID() )
				<< BUILD_LOG( CXSLRoom::GetRoomType( GetBeforeRoomUID() ) )
				<< BUILD_LOG( GetRoomUID() )
				<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
				<< END_LOG;
		}
	}
}

void KGSUser::SetFieldUID( IN const UidType iUID )
{
	// 필드 정보 업데이트
	const UidType iBeforeFieldUID = GetFieldUID();
	m_iFieldUID = iUID;

	// 예외 처리!
	if( iBeforeFieldUID == 0 )
	{
		if( iUID == 0 )
		{
			START_LOG( cerr, L"필드에서 이미 나갔는데 또 나가라고 정보가 왔네? 이상한 오류다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;
		}
		else
		{
			// 필드에 입장
		}
	}
	else
	{
		if( iUID == 0 )
		{
			//{{ 버프 관리 시스템 - 김민성
			std::vector< int > vecDeactivateBuff;
			m_kUserBuffManager.OnLeaveField( GetThisPtr<KGSUser>(), vecDeactivateBuff );
			if( vecDeactivateBuff.empty() == false )
			{
				// 로그인 서버 버프 매니저에 업데이트
				KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
				kReq.m_iUnitUID = GetCharUID();
				kReq.m_vecDeActivateBuff = vecDeactivateBuff;
				SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
			}
			//}}
		}
		else
		{
			// 다른 필드에 입장
		}
	}
}
//}}

//{{ 2010. 02. 20  최육사	채널 파티 통합
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void KGSUser::SendUpdatePartyMemberList( IN UidType iPartyUID, IN const std::set< UidType >& setPartyUIDList )
{
	// 현재 같은 파티에 소속된 유저들의 UID를 필드에 날려준다.

	//호출되는 경우는 파티매니져에서
	//## 1. E_CreateParty
	//## 2. E_JoinPartyInviteReply
	//## 3. E_InvitePartyReply
	//## 4. E_LeaveParty
	//## 5. E_BanUser

	if( GetStateID() != KGSFSM::S_FIELD_MAP )
		return;

	if( GetFieldUID() == 0 )
	{
		// 필드상태이지만 FieldUID가 0인 경우가 있다. [대전로비, 훈련소로비]
		START_LOG( clog, L"현재 필드 상태인데 UID가 잘못되었음." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetFieldUID() )
			<< END_LOG;
		return;
	}

	KERM_UPDATE_PARTY_NOT kNot;
	kNot.m_iPartyUID = iPartyUID;
	kNot.m_setPartyUIDList = setPartyUIDList;
	SendToGSField( ERM_UPDATE_PARTY_NOT, kNot );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

//{{ 2009. 4. 14  최육사	친구리스트
void KGSUser::SendUpdateFriendList()
{
	// 접속중인 친구리스트를 필드에 날려준다.

	if( GetStateID() != KGSFSM::S_FIELD_MAP )
		return;

	if( GetFieldUID() == 0 )
	{
		// 필드상태이지만 FieldUID가 0인 경우가 있다. [대전로비, 훈련소로비]
		START_LOG( clog, L"현재 필드 상태인데 UID가 잘못되었음." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetFieldUID() )
			<< END_LOG;
		return;
	}

	KERM_UPDATE_FRIEND_NOT kNot;

	std::map< std::wstring, UidType >::const_iterator mit;
	for( mit = m_mapFriendList.begin(); mit != m_mapFriendList.end(); ++mit )
	{
		kNot.m_setFriendUIDList.insert( mit->second );
	}

	SendToGSField( ERM_UPDATE_FRIEND_NOT, kNot );
}
//}}

//{{ 2009. 10. 8  최육사	길드
void KGSUser::SendUpdateGuildMemberInfo()
{
	if( GetGuildUID() > 0 )
	{
		KELG_UPDATE_GUILD_MEMBER_INFO_NOT kPacketNot;
		kPacketNot.m_iGuildUID = GetGuildUID();
		GetGuildMemberInfo( kPacketNot.m_kMemberInfo );
		SendToLoginServer( ELG_UPDATE_GUILD_MEMBER_INFO_NOT, kPacketNot );
	}
}
//}}

//{{ 2010. 01. 11  최육사	추천인리스트
void KGSUser::SendUpdateRecommendUserInfo( IN const KRecommendUserInfo::RECOMMEND_USER_INFO_TYPE eType )
{
	if( m_kUserRecommendManager.GetRecommendedUnitUID() > 0 )
	{
		KELG_RECOMMEND_USER_INFO_NOT kPacketToLg;
		kPacketToLg.m_iRecommendedUnitUID = m_kUserRecommendManager.GetRecommendedUnitUID();
		kPacketToLg.m_iRecommendUnitUID = GetCharUID();
		kPacketToLg.m_cType = static_cast<char>(eType);
		kPacketToLg.m_iData = GetLevel();
		SendToLoginServer( ELG_RECOMMEND_USER_INFO_NOT, kPacketToLg );
	}

}
//}}

//{{ 2009. 4. 22  최육사	대전유저리스트
void KGSUser::SendUpdatePVPUserInfo( char cState )
{
	// 로비 진입
	if( cState == KCommunityUserInfo::US_ENTER_LOBBY )
	{
		if( GetRoomListID() <= 0 )
		{
			START_LOG( cerr, L"대전로비에 입장하려는데 RoomListID값이 이상합니다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetRoomListID() )
				<< END_LOG;

			// 대전로비 입장처리 하지말자!
			return;
		}

		//{{ 2011. 07. 26	최육사	대전 개편
		// 공식대전 로비라면?
		if( GetRoomListID() == SiKRoomListManager()->GetPvpOffcialRoomListID() )
			return;
		//}}

		KECN_NEW_PVP_USER_INFO_NOT kPacketNot;
		kPacketNot.m_nGSUID					  = KBaseServer::GetKObj()->GetUID();
		kPacketNot.m_kUserInfo.m_iUnitUID	  = GetCharUID();
		kPacketNot.m_kUserInfo.m_wstrNickName = GetCharName();
		kPacketNot.m_kUserInfo.m_cUnitClass	  = GetUnitClass();
		kPacketNot.m_kUserInfo.m_ucLevel	  = GetLevel();
		kPacketNot.m_kUserInfo.m_cState		  = cState;
		kPacketNot.m_kUserInfo.m_iStateCode	  = GetRoomListID();
		kPacketNot.m_kUserInfo.m_iRoomUID	  = GetRoomUID();
		SendToCnServer( ECN_NEW_PVP_USER_INFO_NOT, kPacketNot );

		//{{ 2009. 10. 23  최육사	길드
		SendUpdateGuildMemberInfo();
		//}}
	}
	// 정보 갱신
	else
	{
		//{{ 2011. 07. 26	최육사	대전 개편
		// 공식대전 로비라면?
		if( GetRoomListID() == SiKRoomListManager()->GetPvpOffcialRoomListID() )
			return;
		//}}

		KPVPUserSimpleInfo kInfo;
		kInfo.m_iUnitUID	= GetCharUID();
		kInfo.m_cState		= cState;
		kInfo.m_iStateCode	= GetRoomListID();
		kInfo.m_iRoomUID	= GetRoomUID();

        KECN_UPDATE_PVP_USER_INFO_NOT kPacketNot;
		kPacketNot.m_vecUserInfo.push_back( kInfo );
		SendToCnServer( ECN_UPDATE_PVP_USER_INFO_NOT, kPacketNot );
	}
}

//{{ 2009. 6. 5  최육사		장착정보 변경
void KGSUser::SendUpdateChangeEquippedItem()
{
	//{{ 2008. 12. 3  최육사	필드에서 기간다된 아이템 갱신
	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		if( GetFieldUID() <= 0 )
		{
			// 필드 상태이면서 fielduid가 0인경우도 있다.
			return;
		}

		KERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
		//}}
		SendToGSField( ERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT, kPacketNot );
	}
	//}}
	// 장착 타이틀을 룸에서 바꿨으면 룸 멤버들에게 알려준다.
	else if( GetStateID() == KGSFSM::S_ROOM )
	{
		if( GetRoomUID() <= 0 )
		{
			START_LOG( cerr, L"룸 UID 이상." )
				<< BUILD_LOG( GetRoomUID() )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			return;
		}

		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
		case CXSLRoom::RT_TRAININGCENTER:
			//{{ 2012. 02. 15	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
			//}}
			{
				KERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT kPacketNot;
				//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
				kPacketNot.m_cChangeEquippedPacketType = KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_EQUIP;
				//}}
				kPacketNot.m_UnitUID = GetCharUID();
				GetRoomUserInfo( kPacketNot.m_kRoomUserInfo );
				SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT, kPacketNot );
			}
			break;

		case CXSLRoom::RT_SQUARE:
			{
				KERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT kPacketNot;
				kPacketNot.m_UnitUID = GetCharUID();
				//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
				kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
				//}}
				SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT, kPacketNot );
			}
			break;

		default:
			{
				START_LOG( cerr, L"이상한 방 타입." )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;
			}
			break;
		}
	}
}
//}}

//{{ 2010. 02. 08  최육사	PC방 채널 이동 오류 수정
void KGSUser::SendUpdateChangeEquippedItem( IN const std::vector< KInventoryItemInfo >& vecInventorySlotInfo )
{
	//{{ 2008. 12. 3  최육사	필드에서 기간다된 아이템 갱신
	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		if( GetFieldUID() <= 0 )
		{
			// 필드 상태이면서 fielduid가 0인경우도 있다.
			return;
		}

		KERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		std::vector< KInventoryItemInfo >::const_iterator vit;
		for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); ++vit )
		{
			if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
			{
				kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
			}
		}

		if( kPacketNot.m_vecInventorySlotInfo.empty() == false )
		{
			//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
			kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
			//}}
			SendToGSField( ERM_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT, kPacketNot );
		}
	}
	//}}
	else if( GetStateID() == KGSFSM::S_ROOM )
	{
		if( GetRoomUID() <= 0 )
		{
			START_LOG( cerr, L"룸 UID 이상." )
				<< BUILD_LOG( GetRoomUID() )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			return;
		}

		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
		case CXSLRoom::RT_TRAININGCENTER:
			//{{ 2012. 02. 15	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
			//}}
			{
				KERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT kPacketNot;
				//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
				kPacketNot.m_cChangeEquippedPacketType = KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_EQUIP;
				//}}
				kPacketNot.m_UnitUID = GetCharUID();
				std::vector< KInventoryItemInfo >::const_iterator vit;
				for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); ++vit )
				{
					if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
					{
						kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
					}
				}

				if( kPacketNot.m_vecInventorySlotInfo.empty() == false )
				{
					GetRoomUserInfo( kPacketNot.m_kRoomUserInfo );
					SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT, kPacketNot );
				}
			}
			break;

		case CXSLRoom::RT_SQUARE:
			{
				KERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT kPacketNot;
				kPacketNot.m_UnitUID = GetCharUID();
				std::vector< KInventoryItemInfo >::const_iterator vit;
				for( vit = vecInventorySlotInfo.begin(); vit != vecInventorySlotInfo.end(); ++vit )
				{
					if( vit->m_cSlotCategory == CXSLInventory::ST_E_EQUIP )
					{
						kPacketNot.m_vecInventorySlotInfo.push_back( *vit );
					}
				}

				if( kPacketNot.m_vecInventorySlotInfo.empty() == false )
				{
					//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
					kPacketNot.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
					//}}
					SendToCnRoom( ERM_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT, kPacketNot );
				}
			}
			break;

		default:
			{
				START_LOG( cerr, L"이상한 방 타입." )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;
			}
			break;
		}
	}
}
//}}

//{{ 2010. 02. 10  최육사	일일 이벤트 퀘스트

void KGSUser::SendUpdateDropQuestItemByIngQuest()
{
	// 방상태이고 pvp & dungeon 방이면 정보업데이트 한다.
	if( GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_DUNGEON:
			//{{ 필드 드롭 개편 - 김민성
		case CXSLRoom::RT_BATTLE_FIELD:
			//}}
			{
				//Update Server quest item data
				KERM_SET_QUEST_ITEM_INFO_NOT kInfo;

				//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
				m_kUserQuestManager.GetOngoingQuestForRoom( GetThisPtr<KGSUser>(), kInfo.m_mapDropQuestItembyIngQuest );
				//}}

				SendToCnRoom( ERM_SET_QUEST_ITEM_INFO_NOT, kInfo );
			}
			break;
		default:
			break;
		}
	}
}

//}}

//{{ 2010. 8. 19	최육사	펫 시스템

void KGSUser::SendPetAction( IN CXSLPetManager::PET_ACTION_COMMAND ePetAction )
{
	//////////////////////////////////////////////////////////////////////////
	// 마을인지 방인지 체크해서 날리자!
	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		if( GetFieldUID() <= 0 )
		{
			KEGS_PET_ACTION_NOT kNot;
			kNot.m_cActionType = ePetAction;
			kNot.m_iUnitUID = GetCharUID();
			SendPacket( EGS_PET_ACTION_NOT, kNot );
		}
		else
		{
			KERM_PET_ACTION_NOT kNot;
			kNot.m_cActionType = ePetAction;
			kNot.m_iUnitUID = GetCharUID();
			SendToGSField( ERM_PET_ACTION_NOT, kNot );
		}
	}
	else if( GetStateID() == KGSFSM::S_ROOM )
	{
		if( GetRoomUID() <= 0 )
		{
			START_LOG( cerr, L"룸 UID 이상." )
				<< BUILD_LOG( GetRoomUID() )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			return;
		}

		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
		case CXSLRoom::RT_TRAININGCENTER:
			//{{ 2012. 10. 23	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
			//}}
			{
				KERM_PET_ACTION_NOT kNot;
				kNot.m_cActionType = ePetAction;
				kNot.m_iUnitUID = GetCharUID();
				SendToCnRoom( ERM_PET_ACTION_NOT, kNot );
			}
			break;

		case CXSLRoom::RT_SQUARE:
			// 광장이면 안보낸다!
			break;

		default:
			{
				START_LOG( cerr, L"이상한 방 타입." )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;
			}
			break;
		}
	}
	else
	{
		START_LOG( cerr, L"이상한 상태에서 펫 액션을 전송하려고 합니다." )
			<< BUILD_LOG( GetStateIDString() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( ePetAction )
			<< END_LOG;
	}
}

//}}

//{{ 2013. 05. 29	최육사	제재 리스트 통합
void KGSUser::SendCheckBlockUser( IN const KBlockInfo::BLOCK_TYPE eBlockType )
{
	std::vector< int > vecBlockType;
	vecBlockType.push_back( eBlockType );
	SendCheckBlockUser( vecBlockType );
}

void KGSUser::SendCheckBlockUser( IN const std::vector< int >& vecBlockType )
{
	KEGB_CHECK_BLOCK_USER_NOT kNot;
	kNot.m_vecBlockType = vecBlockType;
	kNot.m_iUserUID = GetUID();
	kNot.m_iUnitUID = GetCharUID();
	kNot.m_wstrIP = KncUtil::toWideString( GetIPStr() );
	SendToGlobalServer( EGB_CHECK_BLOCK_USER_NOT, kNot );
}
//}}

//{{ 2013. 06. 03	최육사	아이템 개편
void KGSUser::SendEnchantNotice( IN const int iItemID, IN const int iEnchantLevel, IN const int iRandomItemID /*= 0 */) const
{
	// 9강 이상 강화 성공시 전체 공지!
	if( iEnchantLevel >= 9 )
	{
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
		if( pItemTemplet != NULL )
		{
			KEGS_NOTIFY_MSG_NOT	kPacket;
			kPacket.m_cNotifyType = KEGS_NOTIFY_MSG_NOT::NT_DROP_GET_ITEM; // 아이템 획득 공지
			kPacket.m_Count			= 1;
			kPacket.m_wstrMSG		+= x2::str( x2::wformat( L"%s;+%d %s" ) % GetCharName() % iEnchantLevel % pItemTemplet->m_Name );
			kPacket.m_iRandomItem	= iRandomItemID;
			SendToCnServer( ECN_NOTIFY_MSG_NOT, kPacket );
		}
		else
		{
			START_LOG( cerr, L"존재하지 않는 아이템을 강화했다? 공지 날리기 실패!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( iItemID )
				<< END_LOG;
		}
	}
}
//}}

//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//}}

//{{ 2011. 03. 02	최육사	개인 거래 차단
void KGSUser::HackUserRegRejectedUser( IN const char cReason
									 )
{
	// 핵 유저 접속 차단 자동등록
	if( SiKGameSysVal()->IsHackUserRegRejected() == false )
		return;

	if( SEnum::UAL_DEVELOPER <= GetAuthLevel() )
	{
		START_LOG( cout, L"계정 권한이 개발자급 이상이므로 블록 조치를 취하지 않습니다!" )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOGc( GetAuthLevel() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( cReason )
			;

		return;
	}

	// RejectedUser에 등록하기!
	KELG_REG_REJECTED_USER_NOT kPacketNot;
	kPacketNot.m_iUserUID			= GetUID();
	kPacketNot.m_cRejectedReason	= cReason;
	SendToLoginServer( ELG_REG_REJECTED_USER_NOT, kPacketNot );

	START_LOG( cout, L"서버가 오토핵 유저로 판단! Rejected User로 등록!" )
		<< BUILD_LOG( GetUserName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOGc( cReason )
		;
}

void KGSUser::HackUserRegTradeBlock( IN const char cReason )
{
	// 개인 거래 차단 등록 여부
	if( SiKGameSysVal()->IsHackUserBlockTrade() == false )
		return;

	// 이미 거래 차단 유저인지 확인
	if( m_kUserAbuserManager.IsTradeBlockUser() == true )
		return;

	// TradeBlock에 등록하기!
	KELG_REG_REJECTED_USER_NOT kPacketNot;
	kPacketNot.m_iUserUID = GetUID();
	kPacketNot.m_cRejectedReason = cReason;
	SendToLoginServer( ELG_REG_TRADE_BLOCK_USER_NOT, kPacketNot );

	// 개인거래 차단!
	m_kUserAbuserManager.SetTradeBlockUser( true );

	START_LOG( cout, L"서버가 오토핵 유저로 판단! Trade Block User로 등록!" )
		<< BUILD_LOG( GetUserName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOGc( cReason );
}

void KGSUser::HackUserRegMonitorsUser( IN const char cReason )
{
	// 핵유저 감시 대상 자동 등록
	if( SiKGameSysVal()->IsHackUserRegMonitors() == false )
		return;

	if( m_cHackingUserType != SEnum::HUT_NORMAL )
		return;

	START_LOG( clog, L"해킹 유저 신고.!" )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetUserName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOGc( m_cHackingUserType )
		;

	// 핵유저로 체크!
	m_cHackingUserType = SEnum::HUT_DISAGREE_HACK_USER;

	// AccountDB에도 등록하자!
	SendToAccountDB( DBE_REPORT_HACK_USER_NOT, GetUID() );

	START_LOG( cout, L"서버가 오토핵 유저로 판단! 해킹 의심 유저로 등록!" )
		<< BUILD_LOG( GetUserName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOGc( cReason );
}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
void KGSUser::HackUserKick( IN const KEGS_SERVER_CHECK_HACK_USER_REQ& kPacket )
{
	// 핵유저 서버 접속 종료
	if( SiKGameSysVal()->IsHackUserKick() == false )
		return;

	SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );

	//{{ 2010. 07. 09  최육사	핵유저 종료처리
	SetDisconnectReason( KStatistics::eSIColDR_Client_Hacking );
	ReserveDestroy();
	//}}

	START_LOG( cout, L"서버가 오토핵 유저로 판단! 접속 종료 처리!" )
		<< BUILD_LOG( GetUserName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOGc( kPacket.m_cHackingCheckType );
}
//}}
//}}

//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
void KGSUser::HackUserRegPacketMornitor()
{
	// 핵유저 서버 접속 종료
	if( SiKGameSysVal()->IsHackUserPacketMornitor() == false )
		return;

	m_kUserAbuserManager.RegPacketMornitoring( GetThisPtr<KGSUser>() );
}
//}}

//{{ 2009. 9. 7  최육사		퀘스트 기능 수정
bool KGSUser::IsDungeonGetExp( int iDungeonID, char cDifficulty )
{
	int nConst = abs( SiCXSLDungeonManager()->GetDungeonMinLevel( static_cast<int>(iDungeonID + cDifficulty) ) - static_cast<int>(GetLevel()) );

	return ( nConst <= 19 ); // 렙차가 20이 되면 경험치 획득 불가 던전이다!
}
//}}

//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합

bool KGSUser::IsSameServerGroupUID( IN UidType iUID )
{
	return ( KncUid::ExtractServerGroupID( iUID ) == KncUid::ExtractServerGroupID( GetCharUID() ) );
}

//}}

bool KGSUser::IsEnoughItemExist( IN const std::map< int, int >& mapRequiredItem, IN const bool bOnlyInventory )
{
	return m_kInventory.IsEnoughItemExist( mapRequiredItem, bOnlyInventory );
}

//{{ 2010. 01. 11  최육사	서버코드정리
bool KGSUser::IsEnoughItemExistOnlyInventory( IN const std::map< int, int >& mapRequiredItem )
{
    return m_kInventory.IsEnoughItemExistOnlyInventory( mapRequiredItem );
}

//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저

void KGSUser::CheckEDAbuser( IN int iGetEDState, IN int iBeforeED )
{
    m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), iGetEDState, iBeforeED );
}

void KGSUser::CheckItemAbuser( IN int iGetItemState, IN const std::map< int, int >& mapGetItemInfo )
{
    m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), iGetItemState, mapGetItemInfo );
}

//}}

void KGSUser::UserStatisticsIncreaseCount( int iTableID, int iSubTableID , int iColumn, int iCount )
{
    m_kUserStatistics.IncreaseCount( iTableID, iSubTableID , iColumn, iCount );

	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	if( iColumn == KUserStatistics::US_Etc_Obtained_Item )
	{
		m_kUserAbuserManager.IncreaseGetItemCount( GetThisPtr<KGSUser>(), iCount );
	}
	//}}
}
//}}

//void KGSUser::SetPartyDungeonInfo(int iPartyDungeonID, char cPartyDifficulty)
//{
//	m_iPartyDungeonID	= iPartyDungeonID;
//	m_cPartyDifficulty	= cPartyDifficulty;
//}

//
//void KGSUser::SendUpdatePartyInfo( bool bVerifyParty )
//{
//	if( GetRoomUID() != 0 )
//	{
//		if( GetRoomType( GetRoomUID() ) == CXSLRoom::RT_SQUARE )
//		{
//			SendToCnRoom( ERM_UPDATE_PARTY_INFO_NOT, bVerifyParty );
//		}
//	}
//}

//bool KGSUser::CheckKNPAuthQuery( bool bIsFirst )
//{
//	if( m_pCSA == NULL )
//	{
//		START_LOG( cerr, L"nProtect 모듈 포인터 이상.!" )
//			<< BUILD_LOG( GetUID() )
//			<< BUILD_LOG( GetChannelID() )
//			<< END_LOG;
//
//		return false;
//	}
//
//	DWORD dwRet = m_pCSA->GetAuthQuery();
//
//	if( dwRet != ERROR_SUCCESS )
//	{
//		KPacketOK kShowDown;
//		kShowDown.m_iOK = NetError::ERR_KNP_00;
//
//		//에러처리
//		WCHAR			wszError[MAX_PATH] = L"";
//		std::wstring	wstrError;
//
//		wsprintf( wszError, L"[ERRCODE:%d] Query : %08X %08X %08X %08X",
//							dwRet,
//							m_pCSA->m_AuthQuery.dwIndex,
//							m_pCSA->m_AuthQuery.dwValue1,
//							m_pCSA->m_AuthQuery.dwValue2,
//							m_pCSA->m_AuthQuery.dwValue3
//							);
//
//		wstrError = wszError;
//
//		START_LOG( cerr, L"nProtect 인증 생성 실패.!" )
//			<< BUILD_LOG( GetUID )
//			<< BUILD_LOG( GetCharUID() )
//			<< BUILD_LOG( wstrError )
//			<< END_LOG;
//
//		if( dwRet == 12 || dwRet == 14 )
//		{
//			kShowDown.m_iOK = NetError::ERR_KNP_01;
//		}
//
//		//종료 이유를 클라이언트에게 알려주어야 한다.
//		SendPacket( EGS_KNPROTECT_USER_SHOWDOWN_NOT, kShowDown );
//
//		return false;
//	}
//
//	return true;
//}

//{{ 2012. 03. 20	최육사	배틀필드 시스템
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool KGSUser::CheckingSpirit( IN const int iDungeonID )
{
	// 근성도 매니저에서 검사
	bool bSpiritFree = ExistActivateBuff( CXSLBuffManager::BTI_BUFF_PREMIUM_PC_ROOM ) & SiKGameSysVal()->IsSpiritFree( GetPcBangType() );
	return m_kUserSpiritManager.CheckingSpirit( iDungeonID, bSpiritFree );
}

bool KGSUser::DecreaseSpirit( IN const int iDungeonID, IN const char cUserCount, IN const short sStageNpcCount, IN const bool bIsTutorial, IN const bool bIsBattleField )
{
	// 4. 길드 유저일 경우 명성치 계산
	if( GetGuildUID() > 0 )
	{
		// 근성도 이벤트 & PC방 이벤트 이전에 처리 합니다. 소모한것과 동일하게 처리하기 위해서..
		// [주의] 소모 근성도 공식계산은 사제관계가 아닌것으로 계산해야함!
		//{{ 2011. 09. 23	최육사	공존의 축제 근성도 소모 이벤트
//#ifdef SERV_COEXISTENCE_FESTIVAL
		const int iCalculatedDecreaseSpirit = m_kUserSpiritManager.CalculateDecreaseSpirit( cUserCount, sStageNpcCount, false, false, bIsBattleField );
//#else
//		const int iCalculatedDecreaseSpirit = m_kUserSpiritManager.CalculateDecreaseSpirit( cUserCount, sStageNpcCount, false );
//#endif SERV_COEXISTENCE_FESTIVAL
		//}}

		// 근성도 이벤트 & PC방 이벤트 이전에 처리 합니다. 소모한것과 동일하게 처리하기 위해서..
		// [주의] 소모 근성도 공식계산은 사제관계가 아닌것으로 계산해야함!
		m_kUserGuildManager.OnDecreaseSpirit( GetThisPtr<KGSUser>(), iCalculatedDecreaseSpirit );
	}

	//////////////////////////////////////////////////////////////////////////
	// 근성도 감소 처리!
	int iDecreaseSpirit = 0;
	bool bIsSpiritUpdated = false;

	bool bSpiritFree = IsPcBang() & SiKGameSysVal()->IsSpiritFree( GetPcBangType() );

    m_kUserSpiritManager.DecreaseSpirit( iDungeonID
									   , cUserCount
									   , sStageNpcCount
									   , bIsTutorial
									   , bSpiritFree
									   , bIsBattleField
									   , iDecreaseSpirit
									   , bIsSpiritUpdated
									   );



	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}

	if( iDecreaseSpirit > 0 )
	{
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Used_Spirit, iDecreaseSpirit );

		// local log
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( (int) GetLevel() );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_SPIRIT, kKey, KStatistics::eSIColSpirit_Play, 1 );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_SPIRIT, kKey, KStatistics::eSIColSpirit_UseSpirit, iDecreaseSpirit );

		//{{ 2010. 06. 30  최육사	근성도 통계
		m_kUserStatistics.AddUsedSpirit( iDecreaseSpirit );
		//}}
	}

	if( bIsSpiritUpdated )
	{
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_AllUsed_Spirit_Count,	1 );

		// local log
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( (int) GetLevel() );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_SPIRIT, kKey, KStatistics::eSIColSpirit_SpiritZero, 1 );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

//{{ 2011. 06. 20	최육사	대전 개편
bool KGSUser::CheckDungeonPartyCondition( IN const int iDungeonID,
										  IN const char cDifficultyLevel,
										  IN const char cDungeonMode,
										  IN const bool bCheckLowLevel,
										  IN const bool bDontCheckPartyUID )
{
	SET_ERROR( NET_OK );

	////////////////////////////////////////////////////////////////////////// 데이터 검증
	// DungeonID검사
	const int iDungeonIDAndDif = ( iDungeonID + static_cast<int>(cDifficultyLevel) );
	if( iDungeonIDAndDif <= 0 )
	{
		START_LOG( cerr, L"dungeonid값이 이상하다!" )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOGc( cDifficultyLevel )
			<< END_LOG;
	}

	//던전 아이디가 정상인지 체크한다.

	bool bIsEventDungeon = SiCXSLDungeonManager()->IsSwitchingWithEventInfo( iDungeonID );
	if( !SiKGameEventManager()->IsEnableDungeon( iDungeonID, bIsEventDungeon ) )
	{
		START_LOG( cerr, L"꺼져있는 이벤트 DungeonID입니다. 클라에서 검사했을텐데.." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOGc( cDifficultyLevel )
			<< END_LOG;

		SET_ERROR( ERR_PARTY_22 );
		return false;
	}

	//{{ 2012. 03. 03	박세훈	어둠의 문 개편
	if( CXSLDungeon::IsDefenceDungeon( iDungeonID ) == true )
	{
		int iWorldCrystalNum = SiKGSWorldMissionManager()->GetWorldCrystalCount();
		int iWorldCrystalMin, iWorldCrystalMax;

		switch( cDifficultyLevel )
		{
		case CXSLDungeon::DL_NORMAL:	iWorldCrystalMin = 0; iWorldCrystalMax = 6; break;
		case CXSLDungeon::DL_HARD:		iWorldCrystalMin = 7; iWorldCrystalMax = 12; break;
		case CXSLDungeon::DL_EXPERT:	iWorldCrystalMin = 13; iWorldCrystalMax = 15; break;
		}

		if( ( iWorldCrystalNum < iWorldCrystalMin ) || ( iWorldCrystalMax < iWorldCrystalNum ) )
		{
			SET_ERROR( ERR_PARTY_22 );
			return false;
		}
	}
	//}}

	//{{ 2009. 10. 16  최육사	벨더
	if( false == CXSLDungeon::IsRubenDungeon( iDungeonID )  &&
		false == CXSLDungeon::IsElderDungeon( iDungeonID )  &&
		false == CXSLDungeon::IsBesmaDungeon( iDungeonID )  &&
		false == CXSLDungeon::IsAlteraDungeon(iDungeonID )  &&
		false == CXSLDungeon::IsPeitaDungeon( iDungeonID )  &&
		false == CXSLDungeon::IsVelderDungeon( iDungeonID ) &&
		false == CXSLDungeon::IsHamelDungeon( iDungeonID )
		//{{ 2012. 10. 29	박세훈	샌더 마을 추가
		&& false == CXSLDungeon::IsSanderDungeon( iDungeonID )
		//}}
		)
	{
		SET_ERROR( ERR_PARTY_22 );
		return false;
	}
	//}}

	//헤니르던전모드가 정상인지 체크한다.
	if( CXSLDungeon::IsValidDungeonMode( static_cast<CXSLDungeon::DUNGEON_MODE>(cDungeonMode) ) == false )
	{
		START_LOG( cerr, L"던전 모드값이 이상합니다!" )
			<< BUILD_LOGc( cDungeonMode )
			<< END_LOG;

		SET_ERROR( ERR_PARTY_22 );
		return false;
	}

	////////////////////////////////////////////////////////////////////////// 유저 조건
	//{{ 2009. 8. 5  최육사		체험ID제한
	if( IsGuestUser()  &&  CXSLDungeon::IsHenirDungeon( iDungeonID ) )
	{
		SET_ERROR( ERR_GUEST_USER_00 );
		return false;
	}
	//}}

	//이미 파티가 있는경우..
	if( bDontCheckPartyUID == false )
	{
		if( GetPartyUID() != 0 )
		{
			SET_ERROR( ERR_PARTY_12 );
			return false;
		}
	}

	//대전 로비에 있는경우.
	if( GetUnitState() == CXSLUnit::CUS_PVP_LOBBY )
	{
		SET_ERROR( ERR_PARTY_13 );
		return false;
	}

	//::1. 근성도 검사.
	if( CheckingSpirit( iDungeonID ) == false )
	{
		SET_ERROR( ERR_ROOM_27 );
		return false;
	}

	//::2. 입장필요 아이템검사.
	int iRequireItemID = 0;
	int iRequireQuantity = 0;

	if( CheckRequiredItemToEnterDungeonRoom( iDungeonIDAndDif, (CXSLDungeon::DUNGEON_MODE) cDungeonMode, iRequireItemID, iRequireQuantity ) == false )
	{
		//{{ 2010. 05. 06  최육사	에러 팝업
		KEGS_ERROR_POPUP_NOT kNot;
		kNot.m_iOK = NetError::ERR_DUNGEON_REQUIRE_ITEM_00;
		kNot.m_iErrorData1 = iRequireItemID;
		kNot.m_iErrorData2 = iRequireQuantity;
		SendPacket( EGS_ERROR_POPUP_NOT, kNot );
		//}}

		SET_ERROR( ERR_DUNGEON_REQUIRE_ITEM_00 );
		return false;
	}

	if( CheckLimitedPlayTimes( iDungeonIDAndDif ) == false )
	{
		SET_ERROR( ERR_DUNGEON_ENTER_LIMIT_00 );
		return false;
	}

	//운영자 이상이 아니면 체크한다.
	if( GetAuthLevel() < SEnum::UAL_GM )
	{
		//::3. 선행던전 클리어를 하지 않았음.
		const int iRequireDungeonID = SiCXSLDungeonManager()->GetRequireDungeonID( iDungeonID );
		if( iRequireDungeonID != 0 )
		{
			std::map<int,KDungeonClearInfo>::iterator mit;
			mit = m_mapDungeonClear.find( iRequireDungeonID );
			if( mit == m_mapDungeonClear.end() )
			{
				SET_ERROR( ERR_ROOM_06 );
				return false;
			}
		}

		//{{ 2010. 12. 30	최육사	하멜 마을 추가
		//}}
	}
	else
	{
		START_LOG( cout, L"운영자 파티 생성 시도!" )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( iDungeonID );
	}

	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	if( bCheckLowLevel == true )	// 레벨 제한이 설정 되어 있다면
	{
		int iDungeonMinLevel = SiCXSLDungeonManager()->GetDungeonMinLevel( iDungeonIDAndDif );

		if( static_cast<int>(GetLevel()) < iDungeonMinLevel - 2 )
		{
			SET_ERROR( ERR_CREATE_PARTY_FAILE );
			return false;
		}
	}
	//}}

	return true;
}

bool KGSUser::CheckPvpPartyCondition( IN const char cPVPGameType, IN const bool bDontCheckPartyUID )
{
	SET_ERROR( NET_OK );

	////////////////////////////////////////////////////////////////////////// 데이터 검증
	// DungeonID검사

	if( CXSLRoom::IsValidPvpGameType( (CXSLRoom::PVP_GAME_TYPE)cPVPGameType ) == false )
	{
		START_LOG( cerr, L"pvp game type값이 이상하다!" )
			<< BUILD_LOGc( cPVPGameType )
			<< END_LOG;

		SET_ERROR( ERR_PARTY_12 );
		return false;
	}

	//이미 파티가 있는경우..
	if( bDontCheckPartyUID == false )
	{
		if( GetPartyUID() != 0 )
		{
			SET_ERROR( ERR_PARTY_12 );
			return false;
		}
	}

	//운영자 이상이 아니면 체크한다.
	if( GetAuthLevel() >= SEnum::UAL_GM )
	{
		START_LOG( cout, L"운영자 대전 파티 생성 시도!" )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOGc( cPVPGameType );
	}
	else
	{
		// 대전 파티 제한 레벨
		if( GetLevel() < SEnum::PE_PVP_OFFICIAL_LIMIT_LEVEL )
		{
			START_LOG( cerr, L"공식 대전을 하기에 부족한 캐릭터 레벨입니다." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetMatchUID() )
				<< BUILD_LOGc( GetLevel() )
				<< END_LOG;

			SET_ERROR( ERR_MATCH_MAKING_04 );
			return false;
		}
	}

	return true;
}
//}}

//{{ 2012. 05. 10	최육사	배틀필드 시스템
bool KGSUser::CheckForDungeonGameStart( IN const char cDungeonMode,
										IN const int iDungeonID,
										IN const char cDifficultyLevel,
										IN const bool bCheckForAutoParty,
										IN const bool bSendErrorPopUp )
{
	SET_ERROR( NET_OK );

	////////////////////////////////////////////////////////////////////////// 유저 검사
	//{{ 2009. 8. 5  최육사		체험ID제한
	if( IsGuestUser() )
	{
		if( GetLevel() > SiKGameSysVal()->GetGuestUserLimitLevel() )
		{
			START_LOG( cerr, L"클라이언트에서 체험ID가 레벨20되었을때 유저를 캐릭선택으로 이동시켰을텐데 아직 게임을 하고있네?" )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			SET_ERROR( ERR_GUEST_USER_02 );
			return false;
		}

		if( CXSLDungeon::IsHenirDungeon( iDungeonID ) )
		{
			SET_ERROR( ERR_GUEST_USER_00 );
			return false;
		}
	}

	// 5. 근성도 검사
	if( CheckingSpirit( iDungeonID ) == false )
	{
		SET_ERROR( ERR_ROOM_27 );
		return false;
	}

	if( CXSLDungeon::IsFixedMembers( iDungeonID ) == true )
	{
		int iNumMember = 0;

		if( GetPartyUID() == 0 )
		{
			iNumMember = 1;
		}
		else if( SiKPartyListManager()->GetPartyNumMember( GetPartyUID(), iNumMember ) == false )
		{
			START_LOG( cerr, L"파티 맴버 이상.!" )
				<< BUILD_LOG( GetPartyUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( iNumMember )
				<< END_LOG;

			SET_ERROR( ERR_PARTY_46 );
			return false;
		}

		if( iNumMember != CXSLDungeon::GetFixedMembers( iDungeonID ) )
		{
			SET_ERROR( ERR_PARTY_46 );
			return false;
		}
	}

	bool bIsEventDungeon = SiCXSLDungeonManager()->IsSwitchingWithEventInfo( iDungeonID );
	if( !SiKGameEventManager()->IsEnableDungeon( iDungeonID, bIsEventDungeon ) )
	{
		START_LOG( cerr, L"꺼져있는 이벤트 DungeonID입니다. 클라에서 검사했을텐데.." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOGc( (int)cDifficultyLevel )
			<< END_LOG;

		SET_ERROR( ERR_PARTY_22 );
		return false;
	}

	// 6. 입장필요 아이템검사.
	int iRequireItemID = 0;
	int iRequireQuantity = 0;
	const int iDungeonIDAndDif = iDungeonID + static_cast<int>(cDifficultyLevel);

	if( CheckRequiredItemToEnterDungeonRoom( iDungeonIDAndDif, (CXSLDungeon::DUNGEON_MODE) cDungeonMode, iRequireItemID, iRequireQuantity ) == false )
	{
		//{{ 2010. 05. 06  최육사	에러 팝업
		if( bSendErrorPopUp )
		{
			KEGS_ERROR_POPUP_NOT kNot;
			kNot.m_iOK = NetError::ERR_DUNGEON_REQUIRE_ITEM_00;
			kNot.m_iErrorData1 = iRequireItemID;
			kNot.m_iErrorData2 = iRequireQuantity;
			SendPacket( EGS_ERROR_POPUP_NOT, kNot );
		}
		//}}

		SET_ERROR( ERR_DUNGEON_REQUIRE_ITEM_00 );
		return false;
	}

	if( CheckLimitedPlayTimes( iDungeonIDAndDif ) == false )
	{
		SET_ERROR( ERR_DUNGEON_ENTER_LIMIT_00 );
		return false;
	}

	if( bCheckForAutoParty )
	{
		// 7. 캐릭터 레벨 검사
		if( SiCXSLDungeonManager()->CheckMinLevelForEnterDungeon( iDungeonIDAndDif, GetLevel() ) == false )
		{
			SET_ERROR( ERR_DUNGEON_GAME_START_00 );
			return false;
		}

		// 8. 아이템 레벨 검사
		{
			//{{ 2012. 10. 09	김민성	배틀필드 아이템 레벨 시스템
			// 혼자 이므로 신청자만 체크하자
			std::map< int, KInventoryItemInfo > mapEquippedItem;
			std::set< CXSLItem::ITEM_TYPE > setItemType;

			setItemType.insert( CXSLItem::IT_WEAPON );	// 무기
            setItemType.insert( CXSLItem::IT_DEFENCE );	// 방어구

			// 등록된 두가지 아이템만 레벨 검사에 사용한다
			// 아바타가 아닌 장비만 가져오자
			m_kInventory.GetEquippedItem( setItemType, false, mapEquippedItem );

			if( CheckItemLevel( mapEquippedItem, iDungeonIDAndDif ) == false )
			{
				SET_ERROR( ERR_DUNGEON_GAME_START_01 );
				return false;
			}
			//}}
		}
	}

	// 9. 선행던전 클리어를 검사 --> 선행던전은 이제 체크하지 않습니다!
	//if( GetAuthLevel() < SEnum::UAL_GM )
	//{
	//	int iRequireDungeonID = SiCXSLDungeonManager()->GetRequireDungeonID( iDungeonIDAndDif );
	//	if( iRequireDungeonID != 0 )
	//	{
	//		std::map<int,KDungeonClearInfo>::iterator mit;
	//		mit = m_mapDungeonClear.find( iRequireDungeonID );
	//		if( mit == m_mapDungeonClear.end() )
	//		{
	//			SET_ERROR( ERR_ROOM_06 );
	//			return false;
	//		}
	//	}
	//}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	//	디펜스던전 검사
	if( CXSLDungeon::IsDefenceDungeon( iDungeonID ) == true )
	{
		//	던전 오픈 검사
		if( false == SiKGSWorldMissionManager()->GetIsActive() )
		{
			SET_ERROR( ERR_WORLD_MISSION_02 );
			return false;
		}

		int iWorldCrystalNum = SiKGSWorldMissionManager()->GetWorldCrystalCount();
		int iWorldCrystalMin, iWorldCrystalMax;

		switch( cDifficultyLevel )
		{
		case CXSLDungeon::DL_NORMAL:	iWorldCrystalMin = 0; iWorldCrystalMax = 6; break;
		case CXSLDungeon::DL_HARD:		iWorldCrystalMin = 7; iWorldCrystalMax = 12; break;
		case CXSLDungeon::DL_EXPERT:	iWorldCrystalMin = 13; iWorldCrystalMax = 15; break;
		}

		if( ( iWorldCrystalNum < iWorldCrystalMin ) || ( iWorldCrystalMax < iWorldCrystalNum ) )
		{
			SET_ERROR( ERR_PARTY_22 );
			return false;
		}
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	return true;
}

bool KGSUser::CheckItemLevel( IN std::map< int, KInventoryItemInfo >& mapEquippedItem, IN int iDungeonIDAndDif )
{
	//////////////////////////////////////////////////////////////////////////
	// 유저의 아이템 평균 레벨 계산 방법
	//
	// ( 무기* 4 + 상의 + 하의 + 장갑 + 신발 ) / 8
	//////////////////////////////////////////////////////////////////////////
	int iDungeonItemLevel = SiCXSLDungeonManager()->GetRequireItemLevel( iDungeonIDAndDif );
	if( iDungeonItemLevel == 0 )
		return true;

	std::map< int, KInventoryItemInfo >::iterator mit = mapEquippedItem.begin();
	int iAllEquippedItemLevel = 0;
	for( ; mit != mapEquippedItem.end() ; ++mit )
	{
		int iEquippedItemLevel = 0;
		//{{ 2013. 05. 15	최육사	아이템 개편
		if( SiCXSLItemManager()->GetItemLevel( GetUnitType(), GetUnitClass(), mit->second.m_kItemInfo, iEquippedItemLevel ) == false )
		//}}
			return false;

		// 무기라면 * 4 을 해준다.
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( mit->second.m_kItemInfo.m_iItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"아이템 정보가 존재하지 않습니다!" )
				<< BUILD_LOG( mit->second.m_kItemInfo.m_iItemID )
				<< END_LOG;
			return false;
		}

		if( pItemTemplet->m_ItemType == CXSLItem::IT_WEAPON )
		{
			iEquippedItemLevel = iEquippedItemLevel * 4;
		}

		// 장비들의 아이템 레벨을 모두 더한다.
		iAllEquippedItemLevel += iEquippedItemLevel;
	}

	iAllEquippedItemLevel = iAllEquippedItemLevel / 8;

	if( iDungeonItemLevel > iAllEquippedItemLevel )
		return false;

	return true;
}
//}}

bool KGSUser::CheckRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode )
{
	int iDummy1 = 0;
	int iDummy2 = 0;

	return CheckRequiredItemToEnterDungeonRoom( iDungeonID, eDungeonMode, iDummy1, iDummy2 );
}

bool KGSUser::CheckRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode, int& iRequireItemID, int& iRequireQuantity )
{
	if( iDungeonID <= 0 )
	{
		START_LOG( cerr, L"던전id가 이상합니다" )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( (int)eDungeonMode )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;

		return false;
	}

	if( CXSLDungeon::IsValidDungeonMode( eDungeonMode ) == false )
	{
		START_LOG( cerr, L"비정상적인 DungeonMode값입니다." )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( (int)eDungeonMode )
			<< END_LOG;

		return false;
	}

	// [이벤트] 080610.hoons.비던 이벤트
	if( SiKGameEventManager()->CheckSecretDungeonEvent( iDungeonID, static_cast<int>(eDungeonMode) ) == true )
		return true;

	//{{ 2010. 01. 05  최육사	PC방 프리미엄

	// PC방 프리미엄 - PC방이면 비던 및 헤니르 시공 무료 입장
	if( IsPcBang() == true
		&& SiKGameSysVal()->IsSecretDungeonFree( GetPcBangType() ) == true
		)
	{
		if( eDungeonMode != CXSLDungeon::DM_HENIR_CHALLENGE  &&
			//{{ 2010. 05. 03  최육사	비밀던전 헬모드
			eDungeonMode != CXSLDungeon::DM_SECRET_HELL  &&
			//}}
			CXSLDungeon::IsEventDungeon( iDungeonID ) == false )
		{
			return true;
		}
	}

	//}}

	std::vector< std::pair< int, int > > vecTicketInformation;
	if( SiCXSLDungeonManager()->GetTicketInformation( iDungeonID, vecTicketInformation ) == false )
	{
		START_LOG( cerr, L"던전에 필요한 아이템 정보를 가져오다가 실패했습니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( eDungeonMode )
			<< BUILD_LOG( iRequireItemID )
			<< END_LOG;

		return false;
	}

	if( vecTicketInformation.empty() == true )
	{
		return true;
	}

	std::vector< std::pair< int, int > >::iterator vitTicketInformation = vecTicketInformation.begin();
	for( ; vitTicketInformation != vecTicketInformation.end(); ++vitTicketInformation )
	{
		iRequireItemID = vitTicketInformation->first;
		iRequireQuantity = vitTicketInformation->second;

		std::map< int, int > mapRequiredItem;
		if( iRequireQuantity <= 0 )
			mapRequiredItem.insert( std::make_pair( iRequireItemID, 1 ) );
		else
			mapRequiredItem.insert( std::make_pair( iRequireItemID, iRequireQuantity ) );

		if( m_kInventory.IsEnoughItemExistOnlyInventory( mapRequiredItem ) == true )
			return true;
	}

	return false;
}

//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
bool KGSUser::DeleteRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode, std::vector< KInventoryItemInfo >& vecInventorySlotInfo, OUT int & iItemID  )
//}}
{
	if( iDungeonID <= 0 )
	{
		START_LOG( cerr, L"던전id가 이상합니다! 일어날수 없는 에러!" )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;

		// 일단 진행처리..
		return true;
	}

	// [이벤트] 080610.hoons.비던 이벤트
	if( SiKGameEventManager()->CheckSecretDungeonEvent( iDungeonID, static_cast<int>(eDungeonMode) ) == true )
		return true;

	//{{ 2010. 01. 05  최육사	PC방 프리미엄

	// PC방 프리미엄 - PC방이면 비던 및 헤니르 시공 무료 입장
	if( IsPcBang() == true
		&& SiKGameSysVal()->IsSecretDungeonFree( GetPcBangType() ) == true
		)
	{
		if( eDungeonMode != CXSLDungeon::DM_HENIR_CHALLENGE  &&
			//{{ 2010. 05. 03  최육사	비밀던전 헬모드
			eDungeonMode != CXSLDungeon::DM_SECRET_HELL  &&
			//}}
			CXSLDungeon::IsEventDungeon( iDungeonID ) == false )
		{
			return true;
		}
	}

	//}}

	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정

	//}}
	int iQuantity = 0;

	std::vector< std::pair< int, int > > vecTicketInformation;
	if( false == SiCXSLDungeonManager()->GetTicketInformation( iDungeonID, vecTicketInformation ) )
	{
		START_LOG( cerr, L"던전에 필요한 아이템 정보를 가져오다가 실패했습니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( (int)eDungeonMode )
			<< END_LOG;

		return false;
	}

	if( vecTicketInformation.empty() == true )
	{
		return true;
	}

	// 기본티켓에서 추가티켓 인덱스 앞번호부터 뒷번호 순서로 있는 것을 확인하고,
	// 가장 먼저 발견된 티켓이 소모성일 경우 지웁니다.
	// 그리고 해당 티켓을 iItemID 에 대입한 상태로 리턴합니다.
	std::vector< std::pair< int, int > >::iterator vitTicketInformation = vecTicketInformation.begin();
	for( ; vitTicketInformation != vecTicketInformation.end(); ++vitTicketInformation )
	{
		iItemID = vitTicketInformation->first;
		iQuantity = vitTicketInformation->second;

		std::map< int, int > mapRequiredItem;
		if( iQuantity <= 0 )
		{
			mapRequiredItem.insert( std::make_pair( iItemID, 1 ) );

			if( m_kInventory.IsEnoughItemExistOnlyInventory( mapRequiredItem ) == true )
				return true;
		}
		else
		{
			mapRequiredItem.insert( std::make_pair( iItemID, iQuantity ) );

			if( m_kInventory.DeleteItemOnlyInventory( mapRequiredItem, vecInventorySlotInfo, KDeletedItemInfo::DR_DUNGEON_REQUIRE ) == true )
			{
				KStatisticsKey kKeyES;
				kKeyES.m_vecIntKey.push_back( iItemID );
				KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKeyES, KStatistics::SI_ITEM_USE, iQuantity );
				//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
				KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKeyES, KStatistics::SI_ITEM_USE, iQuantity );
				//}}

				return true;
			}
		}
	}
	return false;

	return false;
}

void KGSUser::GetConnectionUnitInfo( KConnectionUnitInfo& kInfo )
{
	kInfo.m_iUnitUID		= GetCharUID();
	kInfo.m_nKNMSN			= m_uiKNMSerialNum;
	kInfo.m_cUnitClass		= GetUnitClass();
	kInfo.m_ucLevel			= m_ucLevel;
	kInfo.m_wstrCharName	= GetCharName();
	//{{ 2011. 06. 20	최육사	대전 개편
	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	kInfo.m_cRank			= GetPvpRankForClient();
	//}}
	kInfo.m_iRPoint			= GetRPoint();
	//}}
	kInfo.m_uiPVPRanking	= 0;			//추후 랭킹 시스템 완성후 넣어줘야함
	//{{ 2011. 06. 20	최육사	대전 개편
	kInfo.m_iWin			= GetWin();
	kInfo.m_iLose			= GetLose();
	//}}
	kInfo.m_uiKillNum		= 0;			//기능추가후 넣어줘야함.
	kInfo.m_iRoomUID		= GetRoomUID();
	kInfo.m_bPublic			= false;
	kInfo.m_bCanIntrude		= false;
	kInfo.m_iDungeonID		= 0;

	kInfo.m_cState			= GetUnitState();
	kInfo.m_iStateCode		= 0;

	//{{ 2009. 4. 17  최육사	신마을
	kInfo.m_iChannelID		= SiKChannelManager()->GetChannelID();
	kInfo.m_iPartyUID		= GetPartyUID();
	kInfo.m_iRoomListID		= static_cast<char>(GetRoomListID());
	//}}

	//{{ 2009. 10. 23  최육사	길드
	kInfo.m_wstrGuildName	= m_kUserGuildManager.GetGuildName();
	//}}


	switch( kInfo.m_cState )
	{
	case CXSLUnit::CUS_FIELD_MAP:
		kInfo.m_iStateCode = GetMapID();
		break;

	case CXSLUnit::CUS_PVP_WAIT:
	case CXSLUnit::CUS_PVP_PLAY:
	case CXSLUnit::CUS_DUNGEON_PLAY:
		{
			KRoomInfoPtr spRoomInfo = SiKRoomListManager()->GetRoomInfo( GetRoomUID() );
			if( spRoomInfo != NULL )
			{
				kInfo.m_bPublic			= spRoomInfo->m_bPublic;
				kInfo.m_bCanIntrude		= spRoomInfo->m_bCanIntrude;
			}
		}
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	//광장에서 유닛정보를 보는 경우가 있어서.
	//if( GetRoomUID() != 0  &&  GetRoomType( GetRoomUID() ) != CXSLRoom::RT_SQUARE )
	//{
	//	KRoomInfoPtr spRoomInfo = SiKRoomListManager()->GetRoomInfo( GetRoomUID() );

	//	if( spRoomInfo != NULL )
	//	{
	//		kInfo.m_bPublic			= spRoomInfo->m_bPublic;
	//		kInfo.m_bCanIntrude		= spRoomInfo->m_bCanIntrude;

	//		switch( spRoomInfo->m_RoomType )
	//		{
	//		case CXSLRoom::RT_PVP:
	//			if( spRoomInfo->m_RoomState == CXSLRoom::RS_WAIT )
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_PVP_WAIT;
	//			}
	//			else
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_PVP_PLAY;
	//			}
	//			break;

	//		case CXSLRoom::RT_DUNGEON:
	//			if( spRoomInfo->m_RoomState == CXSLRoom::RS_WAIT )
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_DUNGEON_WAIT;

	//				//if( SiKChannelManager()->VerifyChannelID( GetChannelID() ) == true )
	//				//{
	//				//	kInfo.m_iDungeonID = SiKRoomListManager()->GetDungeonID( GetChannelID() );
	//				//}
	//			}
	//			else
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_DUNGEON_PLAY;
	//			}
	//			break;
	//
	//		case CXSLRoom::RT_ARCADE:
	//			if( spRoomInfo->m_RoomState == CXSLRoom::RS_WAIT )
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_ARCADE_WAIT;

	//				if( GetRoomUID() != 0 )
	//				{
	//					kInfo.m_iDungeonID = SiKRoomListManager()->GetDungeonID( GetRoomUID() );
	//				}
	//			}
	//			else
	//			{
	//				kInfo.m_cConnectionState = CXSLUnit::CUS_ARCADE_PLAY;
	//			}
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		START_LOG( cerr, L"roomuid가 0이 아닌데 방정보가 없음.!" )
	//			<< BUILD_LOG( GetCharName() )
	//			<< BUILD_LOG( GetRoomUID() )
	//			<< END_LOG
	//			;
	//	}
	//}
}

//{{ 2010. 01. 15  최육사	길드게시판

void KGSUser::GetApplyJoinGuildInfo( KApplyJoinGuildInfo& kInfo )
{
	//kInfo.m_iApplyJoinGuildUID; // 아무것도 담지 않음
	kInfo.m_iUnitUID	 = GetCharUID();
	kInfo.m_wstrNickName = GetCharName();
	kInfo.m_cUnitClass	 = GetUnitClass();
	kInfo.m_ucLevel		 = GetLevel();
	//kInfo.m_wstrMessage; // 아무것도 담지 않음
	//kInfo.m_wstrRegDate; // 아무것도 담지 않음
}

//}}

//{{ 2011. 06. 23	최육사	대전 개편
void KGSUser::GetMatchUserInfo( OUT KMatchUserInfo& kInfo )
{
	kInfo.m_iGSUID			= KBaseServer::GetKObj()->GetUID();
	kInfo.m_iUnitUID		= GetCharUID();
	kInfo.m_iPartyUID		= GetPartyUID();
	kInfo.m_wstrNickName	= GetCharName();
	kInfo.m_cUnitClass		= GetUnitClass();
	kInfo.m_ucLevel			= GetLevel();

	//{{ 2012. 06. 20	박세훈	2012 대전 시즌2
	kInfo.m_cCurrentRank		= GetPvpRank();
	kInfo.m_iOfficialMatchCnt	= GetOfficialMatchCount();
	//}}

	kInfo.m_iRating			= GetRating();
	kInfo.m_iRPoint			= GetRPoint();
}
//}}

//{{ 2012. 02. 07	최육사	배틀필드 시스템
void KGSUser::GetAutoPartyUserInfo( OUT KAutoPartyUserInfo& kInfo )
{
	kInfo.m_iGSUID			= KBaseServer::GetKObj()->GetUID();
	kInfo.m_iUnitUID		= GetCharUID();
	kInfo.m_wstrNickName	= GetCharName();
	kInfo.m_cUnitClass		= GetUnitClass();
	kInfo.m_ucLevel			= GetLevel();
}
//}}

//{{ 2012. 04. 23	최육사	배틀필드 시스템
//////////////////////////////////////////////////////////////////////////
bool KGSUser::IsHenirDungeonChallengeMode()
{
	//if( GetRoomUID() == 0 )
	//	return false;

	//if( CXSLRoom::GetRoomType( GetRoomUID() ) != CXSLRoom::RT_DUNGEON )
	//	return false;

	return m_kUserDungeonManager.IsPlayingNowHenirChallengeMode();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

void KGSUser::GetRoomUserInfo( OUT KRoomUserInfo& kRoomUserInfo, IN const int iRoomType_ )
{
	kRoomUserInfo.m_iGSUID		   = KBaseServer::GetKObj()->GetUID();
	kRoomUserInfo.m_iOwnerUserUID  = GetUID();
	kRoomUserInfo.m_cAuthLevel	   = static_cast<char>(GetAuthLevel());

	//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	kRoomUserInfo.m_sServerGroupID = static_cast<short>(KBaseServer::GetKObj()->GetServerGroupID());

	kRoomUserInfo.m_bMale		   = m_kNexonAccountInfo.m_bSex;
	kRoomUserInfo.m_ucAge		   = static_cast<UCHAR>(m_kNexonAccountInfo.m_uiAge);
	kRoomUserInfo.m_nUnitUID	   = GetCharUID();
	kRoomUserInfo.m_uiKNMSerialNum = m_uiKNMSerialNum;
	kRoomUserInfo.m_cUnitClass	   = GetUnitClass();
	kRoomUserInfo.m_ucLevel		   = GetLevel();
	kRoomUserInfo.m_bIsGuestUser   = IsGuestUser();

	//{{ 2012. 04. 17	최육사	던전 로그 컬럼 추가
	kRoomUserInfo.m_iNumResurrectionStone = m_iNumResurrectionStone;

	//{{ 2011. 07. 11	최육사	대전 개편
	kRoomUserInfo.m_iOfficialMatchCnt = m_kUserPvpManager.GetOfficialMatchCount();
	kRoomUserInfo.m_iRating		   = GetRating();
	kRoomUserInfo.m_iMaxRating	   = m_kUserPvpManager.GetMaxRating();
	kRoomUserInfo.m_bIsWinBeforeMatch = m_kUserPvpManager.IsWinBeforeMatch();

	//{{ 2012. 06. 25	박세훈	2012 대전 시즌2
	kRoomUserInfo.m_cRank					= GetPvpRankForClient();
	kRoomUserInfo.m_cRankForServer			= GetPvpRank();
	kRoomUserInfo.m_fKFactor				= m_kUserPvpManager.GetKFactor();
	kRoomUserInfo.m_bIsRedistributionUser	= m_kUserPvpManager.IsRedistributionUser();

	kRoomUserInfo.m_wstrNickName   = GetCharName();
	kRoomUserInfo.m_wstrIP		   = GetP2PIP();
	kRoomUserInfo.m_usPort		   = static_cast<USHORT>(GetP2PPort());

	//{{ 2013. 1. 8	박세훈	Merge 공인IP 연결 실패시 내부IP로 시도( 박진웅 )
	kRoomUserInfo.m_wstrInternalIP	= GetP2PInternalIP();
	kRoomUserInfo.m_usInternalPort	= GetP2PInternalPort();

	//{{ 2011. 11. 1	최육사	배틀필드 시스템
	kRoomUserInfo.m_iPartyUID	   = GetPartyUID();

	//{{ 2009. 9. 25  최육사	길드
	kRoomUserInfo.m_iGuildUID	   = GetGuildUID();
	kRoomUserInfo.m_wstrGuildName  = m_kUserGuildManager.GetGuildName();
	kRoomUserInfo.m_ucMemberShipGrade = m_kUserGuildManager.GetMemberShipGrade();

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	kRoomUserInfo.m_bComeBackUser	= m_kComeBackUserInfo.GetIsComeBackUser();

	//{{ 2012. 09. 19   김민성   캐시샵 방문 상태
	kRoomUserInfo.m_bEnterCashShop	= m_kUserUnitManager.GetEnterCashShop();

	//{{ 2011. 07. 08    김민성    옵션 수치화
	unsigned int uiHP_OnePoint = CXSLUnit::UPHP_INIT + ( CXSLUnit::UPHP_INCREASE_ONE_POINT * (GetLevel() - 1)); // 현재 레벨당 1단위 값
	unsigned int uiUnitLevel = static_cast<unsigned int>(GetLevel());	//	유닛 레벨 미리 얻기

	//080611.hoons.장착중인 아이템을 검사하여 효과정보를 전달하기위해.
	kRoomUserInfo.m_bIsRingofpvprebirth = m_kInventory.IsEquippedItem( CXSLItem::SI_RING_OF_PVP_REBIRTH ); //::1st. 리스폰반지

	if(kRoomUserInfo.m_bIsRingofpvprebirth == false)
		kRoomUserInfo.m_bIsRingofpvprebirth = m_kInventory.IsEquippedItem( CXSLItem::SI_RING_OF_PVP_REBIRTH_EVENT ); //중국 전용 리스폰반지



	kRoomUserInfo.m_bCouple = GetCouple();
	kRoomUserInfo.m_iRelationTargetUserUid = GetRelationTargetUserUid();
	kRoomUserInfo.m_wstrRelationTargetUserNickname = GetRelationTargetUserNickname();

	//기본 스탯
	KStat kModifiedBaseStatBySkill = m_kStat;

	//{{ 2010. 10. 27	최육사	펫 오라 스킬
	KStatIncreaseRate kStatIncreaseRateInfo;

	////////////////////////////////////////////////////////////////////////// [주의!]
	// 스킬중에 BaseHP를 증가시키는 것부터 미리 계산하자!
	{
		m_kSkillTree.CheckAddSkillStat_BaseHP( m_kStat, kModifiedBaseStatBySkill );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// RoomType 얻기
	CXSLRoom::ROOM_TYPE eRoomType = static_cast<CXSLRoom::ROOM_TYPE>(iRoomType_);
	if( iRoomType_ < 0 )
	{
		if( GetRoomUID() == 0 )
		{
			START_LOG( cerr, L"전달인자 RoomType이 -1이라서 RoomUID로 RoomType을 얻을려고 하는데 RoomUID가 0이다?" )
				<< BUILD_LOG( iRoomType_ )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
		}

		eRoomType = CXSLRoom::GetRoomType( GetRoomUID() );
	}

	//{{ 2012. 11. 05	박세훈	던전 전용 소켓의 경우 필드에도 적용
	const bool bIsDungeonRoom = ( ( eRoomType == CXSLRoom::RT_DUNGEON ) || ( eRoomType == CXSLRoom::RT_BATTLE_FIELD ) );

	switch( eRoomType )
	{
	case CXSLRoom::RT_PVP:
		{
			//{{ 버프 관리 시스템 - 김민성
			m_kUserBuffManager.GetBuffList( kRoomUserInfo.m_vecBuffInfo, CXSLRoom::RT_PVP );

			int iPVPChannelClass = 0;
			//LIF( GetPVPChannelClass( iPVPChannelClass ) );
			GetPVPChannelClass( iPVPChannelClass );

			// PVP 타입별 스텟 정보 셋팅
			CalculatePvpUnitStat( iPVPChannelClass, kModifiedBaseStatBySkill, bIsDungeonRoom, uiHP_OnePoint, uiUnitLevel, kRoomUserInfo );

			//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합
			SiKGameEventManager()->GetRewardEvent( kRoomUserInfo
				, 0
				, GetLevel()
				, GetUnitClass()
				, 0
				, IsPcBang()


				);
		}
		break;

	case CXSLRoom::RT_DUNGEON:
		{
			//{{ 버프 관리 시스템 - 김민성
			m_kUserBuffManager.GetBuffList( kRoomUserInfo.m_vecBuffInfo, CXSLRoom::RT_DUNGEON );

			// 보정 던전인가?
			bool bRevisionDungeon = IsHenirDungeonChallengeMode()  ||  CXSLDungeon::IsEventDungeon( m_kUserDungeonManager.GetDungeonID() );

			// Dungeon 스텟 정보 셋팅
			CalculateDungeonUnitStat( bRevisionDungeon, kModifiedBaseStatBySkill, bIsDungeonRoom, uiHP_OnePoint, uiUnitLevel, kRoomUserInfo, kStatIncreaseRateInfo );

			if( bRevisionDungeon == false )
			{
				//{{ 2009. 2. 2  최육사		소켓옵션 보상 보너스
				float fAddSocketOptExpRate	= 0.0f;
				float fAddSocketOptEDRate	= 0.0f;

				// 장착 아이템 소켓 옵션
				m_kInventory.GetEquippedItemBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );

				// 칭호 소켓 옵션
				m_kUserTitleManager.GetEquippedTitleBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );
				//}}

				if( fAddSocketOptExpRate > 0.0f )
				{
					kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_EXP_RATE, fAddSocketOptExpRate );
				}
				if( fAddSocketOptEDRate > 0.0f )
				{
					kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_ED_RATE, fAddSocketOptEDRate );
				}
			}

			//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합
			SiKGameEventManager()->GetRewardEvent( kRoomUserInfo
				, m_kUserDungeonManager.GetDungeonID()
				, GetLevel()
				, GetUnitClass()
				, 1
				, IsPcBang()


				);
		}
		break;

		//{{ 2011. 11. 7	최육사	배틀필드 시스템
	case CXSLRoom::RT_BATTLE_FIELD:
		{
			//{{ 버프 관리 시스템 - 김민성
			m_kUserBuffManager.GetBuffList( kRoomUserInfo.m_vecBuffInfo, CXSLRoom::RT_BATTLE_FIELD );

			// Dungeon 스텟 정보 셋팅
			CalculateBattleFieldUnitStat( kModifiedBaseStatBySkill, bIsDungeonRoom, uiHP_OnePoint, uiUnitLevel, kRoomUserInfo, kStatIncreaseRateInfo );

			//{{ 2009. 2. 2  최육사		소켓옵션 보상 보너스
			float fAddSocketOptExpRate	= 0.0f;
			float fAddSocketOptEDRate	= 0.0f;

			// 장착 아이템 소켓 옵션
			m_kInventory.GetEquippedItemBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );

			// 칭호 소켓 옵션
			m_kUserTitleManager.GetEquippedTitleBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );
			//}}

			//{{ 2010. 05. 11  최육사	이벤트 보상 코드 정리
			if( fAddSocketOptExpRate > 0.0f )
			{
				kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_EXP_RATE, fAddSocketOptExpRate );
			}
			if( fAddSocketOptEDRate > 0.0f )
			{
				kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_ED_RATE, fAddSocketOptEDRate );
			}

			//{{ 2012. 12. 18	최육사	필드에서도 경험치 이벤트 적용
			SiKGameEventManager()->GetRewardEvent( kRoomUserInfo
				, m_kUserDungeonManager.GetDungeonID()
				, GetLevel()
				, GetUnitClass()
				, 1
				, IsPcBang()

				);
		}
		break;

	case CXSLRoom::RT_TRAININGCENTER:
		{
			//{{ 버프 관리 시스템 - 김민성
			m_kUserBuffManager.GetBuffList( kRoomUserInfo.m_vecBuffInfo );

			// TrainingCenter 스텟 정보 셋팅
			CalculateTrainingCenterUnitStat( kModifiedBaseStatBySkill, bIsDungeonRoom, uiHP_OnePoint, uiUnitLevel, kRoomUserInfo, kStatIncreaseRateInfo );

			//{{ 2009. 2. 2  최육사		소켓옵션 보상 보너스
			float fAddSocketOptExpRate	= 0.0f;
			float fAddSocketOptEDRate	= 0.0f;

			// 장착 아이템 소켓 옵션
			m_kInventory.GetEquippedItemBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );

			// 칭호 소켓 옵션
			m_kUserTitleManager.GetEquippedTitleBonusRate( bIsDungeonRoom, fAddSocketOptExpRate, fAddSocketOptEDRate );
			//}}

			if( fAddSocketOptExpRate > 0.0f )
			{
				kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_EXP_RATE, fAddSocketOptExpRate );
			}
			if( fAddSocketOptEDRate > 0.0f )
			{
				kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_ED_RATE, fAddSocketOptEDRate );
			}
		}
		break;

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
	case CXSLRoom::RT_WEDDING_HALL:
		{
			kRoomUserInfo.m_kGameStat = kModifiedBaseStatBySkill;
		}break;
		//}

	default:
		{
			START_LOG( cerr, L"유닛정보 셋팅중 잘못된 룸타입.!" )
				<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
		}
		break;
	}

	//{{ 버프 관리 시스템 - 김민성
	m_kUserBuffManager.GetBuffBonusRate( kRoomUserInfo );

	// 스킬 스텟 정보 셋팅
	CalculateSkillUnitStat( kRoomUserInfo, kStatIncreaseRateInfo );

	//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
	//}}

	m_kInventory.GetSpecialItem( kRoomUserInfo.m_mapSpecialItem );

	m_kInventory.GetEquippedItem( kRoomUserInfo.m_mapEquippedItem );

	LIF( KUserSkillTree::MAX_SKILL_SLOT == 8 );
	KSkillData aSkillData[ KUserSkillTree::MAX_SKILL_SLOT];
	m_kSkillTree.GetSkillSlot( aSkillData );

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	for( int iSkillSlotID = 0 ; iSkillSlotID < KUserSkillTree::MAX_SKILL_SLOT ; ++iSkillSlotID )
	{
		if( aSkillData[iSkillSlotID].m_iSkillID == CXSLSkillTree::SI_ETC_WS_COMMON_LOVE )
		{
			aSkillData[iSkillSlotID].m_cSkillLevel = m_kUserRelationshipManager.GetRelationshipType();
		}
	}

	// 스킬 슬롯 정보
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkill[0] = aSkillData[KUserSkillTree::SKILL_SLOT_A1];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkill[1] = aSkillData[KUserSkillTree::SKILL_SLOT_A2];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkill[2] = aSkillData[KUserSkillTree::SKILL_SLOT_A3];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkill[3] = aSkillData[KUserSkillTree::SKILL_SLOT_A4];

	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkillSlotB[0] = aSkillData[KUserSkillTree::SKILL_SLOT_B1];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkillSlotB[1] = aSkillData[KUserSkillTree::SKILL_SLOT_B2];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkillSlotB[2] = aSkillData[KUserSkillTree::SKILL_SLOT_B3];
	kRoomUserInfo.m_UnitSkillData.m_aEquippedSkillSlotB[3] = aSkillData[KUserSkillTree::SKILL_SLOT_B4];

	kRoomUserInfo.m_UnitSkillData.m_cSkillSlotBExpirationState = (char) m_kSkillTree.GetSkillSlotBExpirationState();
	kRoomUserInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate = m_kSkillTree.GetSkillSlotBEndDateString();

	kRoomUserInfo.m_UnitSkillData.m_nActiveSkillPagesNumber			= m_kSkillTree.GetActiveSkillPageNumber();
	kRoomUserInfo.m_UnitSkillData.m_nTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

	if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
		m_kSkillTree.GetPassiveSkillData( kRoomUserInfo.m_UnitSkillData.m_vecPassiveSkill, m_kSkillTree.AccessLearnedSkillTree() );
	else
		AddLogWhenSkillPagesNumberIsWrong( L"GetRoomUserInfo" );

	//{{ 2010. 04. 08  최육사	기술의 노트
	m_kSkillTree.GetSkillNote( kRoomUserInfo.m_UnitSkillData.m_vecSkillNote );

	//{{ 2009. 11. 27  최육사	길드스킬
	float fAddGuildBonusExpRate = 0.0f;
	float fAddSocketOptEDRate = 0.0f;

	m_kUserGuildManager.GetGuildPassiveSkillData( kRoomUserInfo.m_UnitSkillData.m_vecGuildPassiveSkill );

	m_kUserGuildManager.GetGuildSkillBonusRate( fAddGuildBonusExpRate, fAddSocketOptEDRate );

	if( fAddGuildBonusExpRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GUILD_BONUS_EXP_RATE, fAddGuildBonusExpRate );
	}
	if( fAddSocketOptEDRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_SOCKET_OPT_ED_RATE, fAddSocketOptEDRate );
	}

	//{{ 2013. 03. 14	 사용하지 않는 RoomUserInfo 의 데이터 삭제 - 김민성
	//}

	//{{ 2010. 01. 29  최육사	PC방 상용화
	kRoomUserInfo.m_bIsGameBang = IsPcBang();
	//}}

	kRoomUserInfo.m_iPcBangType = GetPcBangType();

	//2008. 03. 10. hoons : 현재 진행중인 퀘스트중 퀘스트 아이템 수집퀘스트 정보를 가져온다.
	m_kUserQuestManager.GetOngoingQuestForRoom( GetThisPtr<KGSUser>(), kRoomUserInfo.m_mapOngoingQuest );

	//{{ 2011. 05. 04  김민성	던전 클리어시 아이템 지급 조건 추가
	m_kUserQuestManager.GetUserAllQuest( kRoomUserInfo.m_setQuestInfo );
	m_kUserQuestManager.GetUserGoingQuest( GetThisPtr<KGSUser>(), kRoomUserInfo.m_setGoingQuestInfo );

	// 칭호ID
	kRoomUserInfo.m_iTitleID = m_kUserTitleManager.GetEquippedTitleID();

	// 경험치 메달 소지 시 추가 경험치 셋팅
	CheckInventoryExpMedal( kRoomUserInfo );

	//{{ 2009. 5. 5  최육사		채널별 보너스
	float fAddChannelBonusExpRate = 0.0f;
	float fAddChannelBonusEDRate = 0.0f; // 아직 미적용
	SiKChannelManager()->GetChannelBonus( GetLevel(), fAddChannelBonusExpRate, fAddChannelBonusEDRate );

	if( fAddChannelBonusExpRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_CHANNEL_BONUS_EXP_RATE, fAddChannelBonusExpRate );
	}

	//{{ 2010. 07. 21  최육사	펫 시스템
	m_kUserPetManager.GetSummonedPetInfo( kRoomUserInfo.m_vecPet );

	// 탈것 펫
	m_kUserRidingPetManager.GetSummonedRidingPetInfo( kRoomUserInfo.m_iRidingPetUID, kRoomUserInfo.m_usRidingPetID );

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kRoomUserInfo.m_iLoverUnitUID = m_kUserRelationshipManager.GetLoverUnitUID();
	//}



}

void KGSUser::CalculatePvpUnitStat( IN int iPVPChannelClass,
								    IN KStat& kModifiedBaseStatBySkill,
									IN bool bIsDungeonRoom,
									IN unsigned int uiHP_OnePoint,
									IN unsigned int uiUnitLevel,
									IN OUT KRoomUserInfo& kRoomUserInfo )
{
	switch( iPVPChannelClass )
	{
	default:
	case KPVPChannelInfo::PCC_PLAY:
		{
			// 일반채널과 놀방채널은 10% 능력치를 적용해준다.
			const float RESIZE_FACTOR = 0.1f;

			// 기본 능력치는 1렙 기준으로 gap 차이만 줄인다
			KStatTable::KUnitStatInfo sUnitStatLv1;
			SiKStatTable()->GetUnitStat( GetUnitClass(), 1,	sUnitStatLv1 );

			// 롤백용 : kRoomUserInfo.m_kGameStat.m_iBaseHP		= (int)(sUnitStatLv1.m_uiHP)			+ (int)((float)(kModifiedBaseStatBySkill.m_iBaseHP		- sUnitStatLv1.m_uiHP)          * fResizeFactor);
			//}}
			kRoomUserInfo.m_kGameStat.m_iAtkPhysic	= (int)(sUnitStatLv1.m_usAtkPhysic)		+ (int)((float)(kModifiedBaseStatBySkill.m_iAtkPhysic	- sUnitStatLv1.m_usAtkPhysic)   * RESIZE_FACTOR);
			kRoomUserInfo.m_kGameStat.m_iAtkMagic	= (int)(sUnitStatLv1.m_usAtkMagic)		+ (int)((float)(kModifiedBaseStatBySkill.m_iAtkMagic	- sUnitStatLv1.m_usAtkMagic)    * RESIZE_FACTOR);
			kRoomUserInfo.m_kGameStat.m_iDefPhysic	= (int)(sUnitStatLv1.m_usDefPhysic)		+ (int)((float)(kModifiedBaseStatBySkill.m_iDefPhysic	- sUnitStatLv1.m_usDefPhysic)   * RESIZE_FACTOR);
			kRoomUserInfo.m_kGameStat.m_iDefMagic	= (int)(sUnitStatLv1.m_usDefMagic)		+ (int)((float)(kModifiedBaseStatBySkill.m_iDefMagic	- sUnitStatLv1.m_usDefMagic)    * RESIZE_FACTOR);


			// 스킬과 아이템은 기본적으로 미착용 상태가 디폴트이므로 0기준으로 줄인다
			float fTotalIncHPRate = 0.0f;
			KStat kStat;

			// 스킬스탯 추가
			if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
				m_kSkillTree.GetSkillStat( kStat, m_kSkillTree.AccessLearnedSkillTree() );
			else
			{
				kStat.Init();
				AddLogWhenSkillPagesNumberIsWrong( L"CalculatePvpUnitStat" );
			}
			kStat.MultiplyStat( RESIZE_FACTOR );
			kRoomUserInfo.m_kGameStat.AddStat( kStat );

			//{{ 2009. 11. 26  최육사	길드스킬
			m_kUserGuildManager.GetGuildSkillStat( kStat );
			kStat.MultiplyStat( RESIZE_FACTOR );
			kRoomUserInfo.m_kGameStat.AddStat( kStat );

			m_kInventory.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );

			//{{ 2012. 09. 17	박세훈	2012 대전 시즌2
			// 장착 아이템의 hp 소켓 옵션은 RESIZE_FACTOR 보정하지 않는다. 최종 hp 계산 부분에서 적용되는 보정 공식이 존재한다.
			kStat.MultiplyAttPhysic( RESIZE_FACTOR );
			kStat.MultiplyAtkMagic( RESIZE_FACTOR );
			kStat.MultiplyDefPhysic( RESIZE_FACTOR );
			kStat.MultiplyDefMagic( RESIZE_FACTOR );

			kRoomUserInfo.m_kGameStat.AddStat( kStat );

			m_kUserTitleManager.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );

			kStat.MultiplyStat( RESIZE_FACTOR );
			kRoomUserInfo.m_kGameStat.AddStat( kStat );

			//{{ 2009. 11. 16  최육사	대전길드포인트 - (레벨+장비+칭호+패시브스킬 보너스 모두 적용된값)
			const int iTotalCalcBaseHP = kRoomUserInfo.m_kGameStat.m_iBaseHP + kModifiedBaseStatBySkill.m_iBaseHP;

			kRoomUserInfo.m_kGameStat.m_iBaseHP	= (int)( sUnitStatLv1.m_uiHP * 2.7f ) + (int)((float)( iTotalCalcBaseHP - sUnitStatLv1.m_uiHP ) * 0.6f );
			//}}
		}
		break;

	case KPVPChannelInfo::PCC_FREE:
	case KPVPChannelInfo::PCC_TOURNAMENT:
		{
			//기본 능력치는 1렙 기준으로 gap 차이만 줄인다
			KStatTable::KUnitStatInfo sUnitStatLv1;
			SiKStatTable()->GetUnitStat( GetUnitClass(), 1,			sUnitStatLv1	);

			//kRoomUserInfo.m_kGameStat.m_iBaseHP		= (int)((float)sUnitStatLv1.m_uiHP * 2.7f); // 자유채널과 대회채널은 1레벨 HP의 2.7을 곱한다.
			kRoomUserInfo.m_kGameStat.m_iBaseHP		= (int)((float)sUnitStatLv1.m_uiHP * 4.5f); // 박세훈	2013-11-13 수정	( 대일이형 요청 )
			kRoomUserInfo.m_kGameStat.m_iAtkPhysic	= (int)(sUnitStatLv1.m_usAtkPhysic);
			kRoomUserInfo.m_kGameStat.m_iAtkMagic	= (int)(sUnitStatLv1.m_usAtkMagic);
			kRoomUserInfo.m_kGameStat.m_iDefPhysic	= (int)(sUnitStatLv1.m_usDefPhysic);
			kRoomUserInfo.m_kGameStat.m_iDefMagic	= (int)(sUnitStatLv1.m_usDefMagic);
		}
		break;

	case KPVPChannelInfo::PCC_NONE:
		{
			START_LOG( clog, L"대전 채널 정보가 이상합니다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetRoomListID() )
				<< BUILD_LOG( GetRoomUID() )
				<< END_LOG;
		}
		break;
	}
}

void KGSUser::CalculateDungeonUnitStat( IN bool bRevisionDungeon,
									    IN KStat& kModifiedBaseStatBySkill,
										IN bool bIsDungeonRoom,
										IN unsigned int uiHP_OnePoint,
										IN unsigned int uiUnitLevel,
										IN OUT KRoomUserInfo& kRoomUserInfo,
										IN OUT KStatIncreaseRate& kStatIncreaseRateInfo )
{
	//{{ 2012. 04. 23	최육사	배틀필드 시스템
	if( bRevisionDungeon == true )
	{
		//기본 능력치는 1렙 기준으로 gap 차이만 줄인다
		KStatTable::KUnitStatInfo sUnitStatLv1;

		//{{ 2012. 05. 02	박세훈	어둠의 문 개편
		uiUnitLevel = 1;
		SiKStatTable()->GetUnitStat( GetUnitClass(), static_cast<u_char>(uiUnitLevel),			sUnitStatLv1	);
		//}}

		kRoomUserInfo.m_kGameStat.m_iBaseHP		= (int)(sUnitStatLv1.m_uiHP);
		kRoomUserInfo.m_kGameStat.m_iAtkPhysic	= (int)(sUnitStatLv1.m_usAtkPhysic);
		kRoomUserInfo.m_kGameStat.m_iAtkMagic	= (int)(sUnitStatLv1.m_usAtkMagic);
		kRoomUserInfo.m_kGameStat.m_iDefPhysic	= (int)(sUnitStatLv1.m_usDefPhysic);
		kRoomUserInfo.m_kGameStat.m_iDefMagic	= (int)(sUnitStatLv1.m_usDefMagic);

		//{{ 2011. 07. 08    김민성    옵션 수치화
		uiHP_OnePoint = CXSLUnit::UPHP_INIT; // 1 레벨의 단위 값으로 보정

		// GM이라면 GM무기는 적용받도록 하자!
		if( GetAuthLevel() >= SEnum::UAL_GM )
		{
			// 장착아이템 스탯 추가
			KStat kStat;
			m_kInventory.GetEquippedStatOnlyGMWeapon( kStat );
			kRoomUserInfo.m_kGameStat.AddStat( kStat );
		}
	}
	else
	{
		kRoomUserInfo.m_kGameStat = kModifiedBaseStatBySkill;

		float fTotalIncHPRate = 0.0f;
		KStat kStat;

		// 스킬스탯 추가
		if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
			m_kSkillTree.GetSkillStat( kStat, m_kSkillTree.AccessLearnedSkillTree() );
		else
		{
			kStat.Init();
			AddLogWhenSkillPagesNumberIsWrong( L"CalculateDungeonUnitStat" );
		}

		kRoomUserInfo.m_kGameStat.AddStat( kStat );

		//{{ 2009. 11. 26  최육사	길드스킬

		m_kUserGuildManager.GetGuildSkillStat( kStat );
		kRoomUserInfo.m_kGameStat.AddStat( kStat );
		// 장착아이템 스탯 추가
		m_kInventory.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
		kRoomUserInfo.m_kGameStat.AddStat( kStat );

		// 칭호
		m_kUserTitleManager.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
		kRoomUserInfo.m_kGameStat.AddStat( kStat );

		// 펫 오라 스킬(add stat)
		m_kUserPetManager.GetStatByPetSkill( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
		kRoomUserInfo.m_kGameStat.AddStat( kStat );

		// 펫 오라 스킬(increase rate)	2013-10-31	박세훈	클라에서 계산해주기로 함.
		//m_kUserPetManager.GetStatIncreaseRateByPetSkill( bIsDungeonRoom, kStatIncreaseRateInfo );
	}
}

void KGSUser::CalculateBattleFieldUnitStat( IN KStat& kModifiedBaseStatBySkill,
										    IN bool bIsDungeonRoom,
											IN unsigned int uiHP_OnePoint,
											IN unsigned int uiUnitLevel,
											IN OUT KRoomUserInfo& kRoomUserInfo,
											IN OUT KStatIncreaseRate& kStatIncreaseRateInfo )
{
	kRoomUserInfo.m_kGameStat = kModifiedBaseStatBySkill;

	float fTotalIncHPRate = 0.0f;
	KStat kStat;

	// 스킬스탯 추가
	if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
		m_kSkillTree.GetSkillStat( kStat, m_kSkillTree.AccessLearnedSkillTree() );
	else
	{
		kStat.Init();
		AddLogWhenSkillPagesNumberIsWrong( L"CalculateBattleFieldUnitStat" );
	}

	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	//{{ 2009. 11. 26  최육사	길드스킬
	m_kUserGuildManager.GetGuildSkillStat( kStat );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 장착아이템 스탯 추가
	m_kInventory.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 칭호
	m_kUserTitleManager.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 펫 오라 스킬(add stat)
	m_kUserPetManager.GetStatByPetSkill( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 펫 오라 스킬(increase rate)	2013-10-31	박세훈	클라에서 계산해주기로 함.
	//m_kUserPetManager.GetStatIncreaseRateByPetSkill( bIsDungeonRoom, kStatIncreaseRateInfo );
}

void KGSUser::CalculateTrainingCenterUnitStat( IN KStat& kModifiedBaseStatBySkill,
										   IN bool bIsDungeonRoom,
										   IN unsigned int uiHP_OnePoint,
										   IN unsigned int uiUnitLevel,
										   IN OUT KRoomUserInfo& kRoomUserInfo,
										   IN OUT KStatIncreaseRate& kStatIncreaseRateInfo )
{
	kRoomUserInfo.m_kGameStat = kModifiedBaseStatBySkill;

	float fTotalIncHPRate = 0.0f;
	KStat kStat;

	// 스킬스탯 추가
	if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
		m_kSkillTree.GetSkillStat( kStat, m_kSkillTree.AccessLearnedSkillTree() );
	else
	{
		kStat.Init();
		AddLogWhenSkillPagesNumberIsWrong( L"CalculateTrainingCenterUnitStat" );
	}

	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	//{{ 2009. 11. 26  최육사	길드스킬
	m_kUserGuildManager.GetGuildSkillStat( kStat );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 장착아이템 스탯 추가
	m_kInventory.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 칭호
	m_kUserTitleManager.GetEquippedStat( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 펫 오라 스킬(add stat)
	m_kUserPetManager.GetStatByPetSkill( bIsDungeonRoom, kModifiedBaseStatBySkill, fTotalIncHPRate, kStat, uiHP_OnePoint, uiUnitLevel );
	kRoomUserInfo.m_kGameStat.AddStat( kStat );

	// 펫 오라 스킬(increase rate)	2013-10-31	박세훈	클라에서 계산해주기로 함.
	//m_kUserPetManager.GetStatIncreaseRateByPetSkill( bIsDungeonRoom, kStatIncreaseRateInfo );
}

void KGSUser::CalculateSkillUnitStat( IN OUT KRoomUserInfo& kRoomUserInfo, IN OUT KStatIncreaseRate& kStatIncreaseRateInfo )
{
	//{{ 2010. 10. 27	최육사	펫 오라 스킬
	{
		kRoomUserInfo.m_kGameStat.m_iAtkPhysic	+= static_cast<int>( static_cast<float>(kRoomUserInfo.m_kGameStat.m_iAtkPhysic) * kStatIncreaseRateInfo.m_fIncreaseAtkPhysicRate );
		kRoomUserInfo.m_kGameStat.m_iAtkMagic	+= static_cast<int>( static_cast<float>(kRoomUserInfo.m_kGameStat.m_iAtkMagic) * kStatIncreaseRateInfo.m_fIncreaseAtkMagicRate );
		kRoomUserInfo.m_kGameStat.m_iDefPhysic	+= static_cast<int>( static_cast<float>(kRoomUserInfo.m_kGameStat.m_iDefPhysic) * kStatIncreaseRateInfo.m_fIncreaseDefPhysicRate );
		kRoomUserInfo.m_kGameStat.m_iDefMagic	+= static_cast<int>( static_cast<float>(kRoomUserInfo.m_kGameStat.m_iDefMagic) * kStatIncreaseRateInfo.m_fIncreaseDefMagicRate );
	}

	KStat kModifiedFinalStatBySkill = kRoomUserInfo.m_kGameStat;


	//{{ 2010. 11. 24	최육사	마방 패시브 스킬
	{
		// oasis907 : 김상윤 [2010.12.3] // 룬 슬레이어 - 마법저항 수련
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( (int) CXSLSkillTree::SI_P_ERS_MAGIC_RESISTANCE );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( (int)CXSLSkillTree::SI_P_ERS_MAGIC_RESISTANCE );
			if( NULL != pSkillTemplet )
			{
				float fRate = pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_DEF_MAGIC_REL, iSkillLevel );
				kModifiedFinalStatBySkill.m_iDefMagic = (int) ((float)kRoomUserInfo.m_kGameStat.m_iDefMagic * fRate );
			}
		}
	}

	//{{ kimhc // 2010-12-14 // 승화된 분노 - 레피 패시브
	{
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( CXSLSkillTree::SI_P_RRF_SPIRITUALIZED_FURY );
		if( iSkillLevel > 0)
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( CXSLSkillTree::SI_P_RRF_SPIRITUALIZED_FURY );
			if( pSkillTemplet != NULL )
			{
				float fIncreasingRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );
				kModifiedFinalStatBySkill.m_iAtkMagic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iAtkMagic * fIncreasingRate );
			}
		}
	}

	{
		// oasis907 : 김상윤 [2011.6.28] 청 2차 전직 - 아이언 팔라딘 - 굳건한 의지
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( (int) CXSLSkillTree::SI_P_CIP_IRON_WILL );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( (int)CXSLSkillTree::SI_P_CIP_IRON_WILL );
			if( NULL != pSkillTemplet )
			{
				float fIncreasingRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_ATK_PHYSIC_REL, iSkillLevel ) );

				kModifiedFinalStatBySkill.m_iAtkPhysic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iAtkPhysic * fIncreasingRate );
			}
		}
	}

	//{{2010. 10. 27	김태환	레이븐 블레이드 마스터 벨런스 개편 - 승리의 자신감 물리 공격력 증가 효과 추가
	{
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( (int) CXSLSkillTree::SI_P_RRF_CONFIDENCE_VICTORY );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( (int)CXSLSkillTree::SI_P_RRF_CONFIDENCE_VICTORY );
			if( NULL != pSkillTemplet )
			{
				float fIncreasingRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_ATK_PHYSIC_REL, iSkillLevel ) );

				kModifiedFinalStatBySkill.m_iAtkPhysic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iAtkPhysic * fIncreasingRate );
			}
		}
	}

	{
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( CXSLSkillTree::SI_P_AVP_MAGIC_AMPLIFICATION );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( CXSLSkillTree::SI_P_AVP_MAGIC_AMPLIFICATION );

			if( NULL != pSkillTemplet )
			{
				float fIncreasingRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );

				kModifiedFinalStatBySkill.m_iAtkMagic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iAtkMagic * fIncreasingRate );
			}
		}
	}

	{
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( CXSLSkillTree::SI_P_ERS_RUNE_MASTERY );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( CXSLSkillTree::SI_P_ERS_RUNE_MASTERY );

			if( NULL != pSkillTemplet )
			{
				float fIncreasingRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_ATK_MAGIC_REL, iSkillLevel ) );

				kModifiedFinalStatBySkill.m_iAtkMagic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iAtkMagic * fIncreasingRate );
			}
		}
	}

	// 호신강기
	{
		const int iSkillLevel = m_kSkillTree.GetSkillLevel( CXSLSkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE );
		if( iSkillLevel > 0 )
		{
			const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( CXSLSkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE );

			if( NULL != pSkillTemplet )
			{
				// 물방
				{
					float fRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_DEF_PHYSIC_REL, iSkillLevel ) );
					kModifiedFinalStatBySkill.m_iDefPhysic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iDefPhysic * fRate );
				}
				// 마방
				{
					float fRate = CXSLSkillTree::CalulateIncreaseingRate( pSkillTemplet->GetSkillAbilityValue( CXSLSkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) );
					kModifiedFinalStatBySkill.m_iDefMagic += static_cast<int>( kRoomUserInfo.m_kGameStat.m_iDefMagic * fRate );
				}
				// HP 증가는 KUserSkillTree::CheckAddSkillStat_BaseHP() 여기서 처리
			}
		}
	}

	kRoomUserInfo.m_kGameStat = kModifiedFinalStatBySkill;
}

void KGSUser::CheckInventoryExpMedal( IN OUT KRoomUserInfo& kRoomUserInfo )
{
	//080421.hoons. 인벤에서 축복의 메달 아이템을 검사한다.
	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_BLESSING_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, SiKGameSysVal()->GetPremiumEXPRate() );
	}

	// [이벤트] 081220 ~ 081221 yuksa. 신캐릭터 기념 경험치 메달
	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_15_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.15f );
	}

	//{{ 2009. 3. 30  최육사	경험치 100% 메달
	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_100_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 1.0f );
	}

	//{{ 2011. 10. 19    김민성    200% 경험치 추가 메달(이벤트 아이템)
	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_200_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 2.0f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_10_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.10f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_30_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.30f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_10_PERCENT_MEDAL365 ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.10f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_10_PERCENT_MEDAL1 ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.10f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_50_PERCENT_MEDAL_JP ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.50f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_30_PERCENT_MEDAL_JP ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.30f );
	}



	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_STARTER_EXP_15_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.15f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_50_PERCENT_MEDAL ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.5f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_30_PERCENT_MEDAL_1DAY ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.3f );
	}

	if( m_kInventory.IsExistOnlyInventory( CXSLItem::SI_EXP_10_PERCENT_MEDAL_US ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.1f );
	}


	if( m_kInventory.IsExistOnlyInventory( 85001972 ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.1f );
	}

	if( m_kInventory.IsExistOnlyInventory( 80001331 ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.5f );
	}

	if( m_kInventory.IsExistOnlyInventory( 80001473 ) == true )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE, 0.3f );
	}
}

void KGSUser::GetTutorUnitUIDList(std::vector<UidType>& vecStudentUnitUID)
{
	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_TEACHER &&
		m_kUserTutorialInfo.IsExistbyTutorData() == true )
	{
		m_kUserTutorialInfo.GetTutorUnitUIDList( vecStudentUnitUID );
	}
}

void KGSUser::GetSquareUserInfo( KSquareUserInfo& kSquareUserInfo )
{
    kSquareUserInfo.m_iGSUID            = KBaseServer::GetKObj()->GetUID();


    kSquareUserInfo.m_iUnitUID          = GetCharUID();
    kSquareUserInfo.m_wstrNickName      = GetCharName();

    kSquareUserInfo.m_wstrIP            = GetP2PIP();
    kSquareUserInfo.m_usPort            = static_cast<u_short>(GetP2PPort());

    kSquareUserInfo.m_cUnitClass        = GetUnitClass();
    kSquareUserInfo.m_ucLevel           = GetLevel();
	//{{ 2011. 06. 20	최육사	대전 개편
	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	kSquareUserInfo.m_cRank				= GetPvpRankForClient();
	//}}
	//}}

    kSquareUserInfo.m_bIsParty          = ( GetPartyUID() > 0 );

	// 장착 장비
	m_kInventory.GetEquippedItem( kSquareUserInfo.m_vecEquippedItem );

	// 개인상점
	kSquareUserInfo.m_cPersonalShopState = CXSLSquareUnit::PSS_NONE;
	kSquareUserInfo.m_cPersonalShopType = SEnum::AST_NONE;


	// 타이틀
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	kSquareUserInfo.m_iTitleID = m_kUserTitleManager.GetEquippedTitleID();
	//}}

	//{{ 2009. 9. 25  최육사	길드
	kSquareUserInfo.m_wstrGuildName = m_kUserGuildManager.GetGuildName();
	//}}

	//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
	kSquareUserInfo.m_iED = GetED();
	//}}

}

//{{ 2008. 11. 4  최육사	광장개편
void KGSUser::GetFieldUserInfo( KFieldUserInfo& kFieldUserInfo )
{
	kFieldUserInfo.m_iUnitUID			= GetCharUID();
	kFieldUserInfo.m_wstrNickName		= GetCharName();
	kFieldUserInfo.m_cUnitClass			= GetUnitClass();
	kFieldUserInfo.m_ucLevel			= GetLevel();
	//{{ 2011. 06. 20	최육사	대전 개편
	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	kFieldUserInfo.m_cRank				= GetPvpRankForClient();
	//}}
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kFieldUserInfo.m_iLoverUnitUID = m_kUserRelationshipManager.GetLoverUnitUID();
	//}
	// 장착 장비
	m_kInventory.GetEquippedItem( kFieldUserInfo.m_vecEquippedItem );

	//////////////////////////////////////////////////////////////////////////
	// 로봇인 경우 강제로 랜덤 옷입히기
	//////////////////////////////////////////////////////////////////////////

	//{{ 2008. 10. 7  최육사	타이틀
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	kFieldUserInfo.m_iTitleID = m_kUserTitleManager.GetEquippedTitleID();
	//}}
	//}}

	//{{ 2009. 9. 25  최육사	길드
	kFieldUserInfo.m_wstrGuildName = m_kUserGuildManager.GetGuildName();
	//}}

	//{{ 2009. 2. 24  최육사	파티 말풍선
	kFieldUserInfo.m_iPartyUID = GetPartyUID();

	//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
	kFieldUserInfo.m_iED = GetED();
	//}}

	//{{ 2010. 07. 21  최육사	펫 시스템
	m_kUserPetManager.GetSummonedPetInfo( kFieldUserInfo.m_vecPet );
	//}}
	kFieldUserInfo.m_bInvisible = IsInvisible();

	m_kUserRidingPetManager.GetSummonedRidingPetInfo( kFieldUserInfo.m_iRidingPetUID, kFieldUserInfo.m_usRidingPetID );

}
//}}

//{{ 2009. 9. 22  최육사	길드
void KGSUser::GetGuildMemberInfo( KGuildMemberInfo& kInfo )
{
	kInfo.m_iGSUID		 = KBaseServer::GetKObj()->GetUID();
	kInfo.m_iUnitUID	 = GetCharUID();
	kInfo.m_wstrNickName = GetCharName();
	kInfo.m_cUnitClass	 = GetUnitClass();
	kInfo.m_ucLevel		 = GetLevel();
	kInfo.m_ucMemberShipGrade = m_kUserGuildManager.GetMemberShipGrade();
	kInfo.m_iChannelID	 = SiKChannelManager()->GetChannelID();
	kInfo.m_iMapID		 = GetMapID();
	kInfo.m_tLogOutTime	 = 0; // 새로 접속했으므로 0으로 초기화!
	//kInfo.m_wstrMessage	 = L"";

	// RoomListID가 존재하면 PVP로비이다.
	if( GetRoomListID() > 0 )
	{
		kInfo.m_cPosition = CXSLRoom::RT_PVP;
	}
	else
	{
		if( GetRoomUID() > 0 )
		{
			kInfo.m_cPosition = CXSLRoom::GetRoomType( GetRoomUID() );
		}
		else
		{
			kInfo.m_cPosition = -1; // 방이 아니면 -1
		}
	}
}
//}}

bool KGSUser::CheckRequireDungeonID( int iRequireDungeonID )
{
	std::map< int, KDungeonClearInfo >::const_iterator mit;
	mit = m_mapDungeonClear.find( iRequireDungeonID );
	if( mit == m_mapDungeonClear.end() )
		return false;

	return true;
}

bool KGSUser::IsExistInventoryItem( int iItemID )
{
	return m_kInventory.IsExistOnlyInventory( iItemID );
}

bool KGSUser::IsExistInventoryItemWithBank( int iItemID )
{
	return m_kInventory.IsExistWithBank( iItemID );
}

//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
bool KGSUser::IsEnoughSpaceExist( IN const std::map< int, int >& mapItem )
{
	return m_kInventory.IsEnoughSpaceExist( mapItem );
}
//}}

bool KGSUser::IsCompleteQuest( int iQuestID )
{
    return m_kUserQuestManager.IsCompleteQuest( iQuestID );
}

//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
//{{ 2009. 3. 9  최육사		마을 입장 조건
bool KGSUser::CheckEnterTheVillage( IN int iMapID, OUT int& iLimitLevel, OUT int& iRequireDungeonID )
{
	//{{ 2009. 4. 21  최육사	운영자는 그냥 통과
	if( GetAuthLevel() >= SEnum::UAL_GM )
		return true;
	//}}

	return SiCXSLMapData()->CheckRequireData( iMapID, GetLevel(), m_mapDungeonClear, iLimitLevel, iRequireDungeonID );
}
//}}

bool KGSUser::CheckEnterTheVillage( IN const int iMapID )
{
	//{{ 2009. 4. 21  최육사	운영자는 그냥 통과
	if( GetAuthLevel() >= SEnum::UAL_GM )
		return true;
	//}}

	int iDummy1 = 0;
	int iDummy2 = 0;
	return SiCXSLMapData()->CheckRequireData( iMapID, GetLevel(), m_mapDungeonClear, iDummy1, iDummy2 );
}

//{{ 2009. 4. 28  최육사	최대ED 검사
bool KGSUser::CheckMaxED( __int64 iAddED )
{
	iAddED += GetED();

	return ( iAddED <= static_cast<__int64>(SEnum::UI_MAX_ED_CHAR_HAVE) );
}
//}}

bool KGSUser::IsClearDungeon(int iDungeonID, char cDiffLevel)
{
	std::map< int, KDungeonClearInfo >::iterator mit;
	mit = m_mapDungeonClear.find(static_cast<int>(iDungeonID+cDiffLevel));

	if( mit == m_mapDungeonClear.end() )
		return false;

	return true;
}

//{{ 2009. 6. 12  최육사	데이터 검증
bool KGSUser::CheckValidNickName( const std::wstring& wstrNickName )
{
	if( wstrNickName.empty()  ||  wstrNickName.size() > 12 )
	{
		// 비정상적인 크기의 NickName
		SET_ERROR( ERR_CREATE_UNIT_04 );
		return false;
	}

	if( KODBC::IsInvalidMarkIn( wstrNickName ) )
	{
		// 허용되지 않는 특수문자가 포함되어있음
		SET_ERROR( ERR_ODBC_00 );
		return false;
	}

	return true;
}
//}}

//{{ 2009. 11. 10  최육사	PC방 채널이동
void KGSUser::GetChannelChangeInfo( KChannelChangeInfo& kInfo )
{
	// 채널이동 예약시 함께 가져갈 정보를 얻습니다.

	//{{ 2009. 10. 26  최육사	접속시간이벤트
	m_kUserEventManager.GetEventTimerInfo( kInfo.m_mapEventDurationTime );
	//}}
	//{{ 2009. 11. 10  최육사	칭호
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	kInfo.m_iEquippedTitleID = m_kUserTitleManager.GetEquippedTitleID();
	//}}
	//}}
	kInfo.m_bIsPcBang = IsPcBang();
	kInfo.m_iPcBangType = GetPcBangType();

	//{{ 2010. 02. 08  최육사	PC방 채널 이동 오류 수정
	m_kInventory.GetChannelChangeInventoryInfo( kInfo.m_vecPcBangItemInfo );
	//}}

	//{{ 2012. 06. 14	박세훈	매일매일 선물 상자
	//}}
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}
	//{{ 2012. 1. 2		Merge 박세훈	( 2013.1.2 지헌 : 미니맵 채널 이동 시 누락됬었던 정보 추가 )
	kInfo.m_nUnitSlot = m_nUnitSlot;
	//}}

	USHORT usRidingPetID;
	m_kUserRidingPetManager.GetSummonedRidingPetInfo( kInfo.m_iSummonedRidingPetUID, usRidingPetID );

}
//}}

//{{ 2009. 1. 20  최육사	경험치 제한
void KGSUser::KUserExpManager::AddExp( int iExp )
{
	_JIF( iExp >= 0, return; );

	const int iTotalExpByFinalLevel = SiKExpTable()->GetRequireTotalExpbyLevel( SiKGameSysVal()->GetLimitsLevel() );

	if( ( m_iExp + iExp ) > iTotalExpByFinalLevel )
	{
		iExp = iTotalExpByFinalLevel - m_iExp;
	}

	if( iExp <= 0 )
		return;

	m_iExp += iExp;
}
//}}

void KGSUser::InitTransactionEventFilter()
{
	m_kTransactionGuarantor.RegisterMutualExclusiveEvent( DBE_EXPIRE_CASH_SKILL_POINT_REQ, DBE_EXPIRE_CASH_SKILL_POINT_ACK, EGS_GET_SKILL_REQ, EGS_GET_SKILL_ACK );
	m_kTransactionGuarantor.RegisterMutualExclusiveEvent( DBE_EXPIRE_CASH_SKILL_POINT_REQ, DBE_EXPIRE_CASH_SKILL_POINT_ACK, EGS_RESET_SKILL_REQ, EGS_RESET_SKILL_ACK );
	m_kTransactionGuarantor.RegisterMutualExclusiveEvent( DBE_EXPIRE_CASH_SKILL_POINT_REQ, DBE_EXPIRE_CASH_SKILL_POINT_ACK, EGS_INIT_SKILL_TREE_REQ, EGS_INIT_SKILL_TREE_ACK );
}

bool KGSUser::IsTransactionEnabled( unsigned short usEventID )
{
	bool bTransactionEnabled = true;
	unsigned short usBlockingEventID = 0;

	switch( usEventID )
	{
	case DBE_EXPIRE_CASH_SKILL_POINT_REQ:
	case EGS_GET_SKILL_REQ:
	case EGS_RESET_SKILL_REQ:
	case EGS_INIT_SKILL_TREE_REQ:
		{
			bTransactionEnabled = m_kTransactionGuarantor.IsTransactionEnabled( usEventID, usBlockingEventID );

			if( false == bTransactionEnabled )
			{
				START_LOG( cwarn, L"패킷 처리할려고 하는데 다른 패킷 처리와 겹쳐서 타이밍문제를 피하기 위해 패킷 처리 안하고 무시!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( usEventID )
					<< BUILD_LOG( usBlockingEventID )
					<< END_LOG;
			}

		} break;
	}

	return bTransactionEnabled;
}

void KGSUser::CheckTransactionReqEvent( unsigned short usEventID )
{
	switch( usEventID )
	{
	case DBE_EXPIRE_CASH_SKILL_POINT_REQ:
	case EGS_GET_SKILL_REQ:
	case EGS_RESET_SKILL_REQ:
	case EGS_INIT_SKILL_TREE_REQ:
		{
			m_kTransactionGuarantor.CheckReqEvent( usEventID );
		} break;
	}
}

void KGSUser::CheckTransactionAckEvent( unsigned short usEventID )
{
	switch( usEventID )
	{
	case DBE_EXPIRE_CASH_SKILL_POINT_ACK:
	case EGS_GET_SKILL_ACK:
	case EGS_RESET_SKILL_ACK:
	case EGS_INIT_SKILL_TREE_ACK:
		{
			m_kTransactionGuarantor.CheckAckEvent( usEventID );
		} break;
	}
}

//////////////////////////////////////////////////////////////////////////

void KGSUser::OnBuyCashItemAck( const KEGS_BUY_CASH_ITEM_ACK& kPacket_ )
{
	// 어느 상태 막을지 아직 미정이
	KEGS_BUY_CASH_ITEM_ACK kPacket = kPacket_;
	kPacket.m_bIsWishList = m_kUserWishList.GetBuyCashItemInWishList();
	SendPacket( EGS_BUY_CASH_ITEM_ACK, kPacket );

	// 실시간 부활석
	if( m_kUserCashInventory.IsCashResurrectionStone() == true  &&  kPacket_.m_iOK != NetError::NET_OK )
	{
		KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacketAck );
	}

	//{{ 2011. 02. 08	최육사	캐쉬템 구매 로그
	for( const KNXBTPurchaseReqInfo& kReq : kPacket_.m_vecPurchaseReqInfo )
	{
		KNXBTProductInfo kProductInfo;
		if( SiKNexonBillingTCPManager()->GetProductInfo( kReq.m_ulProductNo, kProductInfo ) == false )
		{
			START_LOG( cerr, L"존재하지 않는 상품을 구매하였습니다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kReq.m_ulProductNo )
				<< END_LOG;
			continue;
		}

		// 캐쉬템 구매 카운트 체크
		m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_BUY_CASH_ITEM );
		m_kUserStatistics.IncreaseAccountCount( KUserStatistics::AGCT_BUY_CASH_ITEM );

		// 해당 캐릭터가 최초로 캐쉬템 구매한것이라면?
		if( m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_BUY_CASH_ITEM ) == 1 )
		{
			//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
			WriteCharacterLogToDBWithProductNo( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_BUY_CASH_ITEM, kReq.m_ulProductNo );
			//}}
		}

		// 캐쉬템 구매 로그 남기기
		CTime tCurTime = CTime::GetCurrentTime();
		KELOG_BUY_CASH_ITEM_LOG_NOT kPacketToLog;
		kPacketToLog.m_iUserUID			= GetUID();
		kPacketToLog.m_iServerGroupID	= KBaseServer::GetKObj()->GetServerGroupID();
		kPacketToLog.m_iUnitUID			= GetCharUID();
		kPacketToLog.m_cUnitClass		= GetUnitClass();
		kPacketToLog.m_ucLevel			= GetLevel();
		kPacketToLog.m_bIsFirstBuy		= m_kUserStatistics.IsFirstBuy();
		kPacketToLog.m_cBuyType			= KELOG_BUY_CASH_ITEM_LOG_NOT::CIBT_BUY;
		kPacketToLog.m_iProductNo		= kProductInfo.m_ulProductNo;
		kPacketToLog.m_iBuyQuantity		= kReq.m_usOrderQuantity;
		kPacketToLog.m_iTotalSalePrice	= kReq.m_usOrderQuantity * kProductInfo.m_ulSalePrice;
		kPacketToLog.m_wstrRegDate		= ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
		SendToLogDB( ELOG_BUY_CASH_ITEM_LOG_NOT, kPacketToLog );
	}
}

int KGSUser::OnGetPurchasedCashItemReq( const KEGS_GET_PURCHASED_CASH_ITEM_REQ& kPacket_, const KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ& kPacket2_ )
{
	// 1. 상품 아이템ID 얻기
	int iItemID = 0;
	KBillProductInfo kBillProductInfo;

	if( !SiKBillManager()->GetProductInfo( kPacket2_.m_iProductNo, kBillProductInfo ) )
	{
		iItemID = 0;
	}
	else
	{
		iItemID = kBillProductInfo.m_iProductID;
	}

	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"아이템 ID 변환 실패." )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< BUILD_LOG( kPacket_.m_wstrProductID )
			<< BUILD_LOG( kPacket2_.m_iTransNo )
			<< BUILD_LOG( kPacket2_.m_iProductNo )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return NetError::ERR_NX_SHOP_01;
	}

	std::map< int, int > mapItem; // 인벤토리 검사용

	// 2. 상품 종류를 검사한다.
	if( SiKBillManager()->IsPackage( kPacket2_.m_iProductNo ) )
	{
		START_LOG( cerr, L"패키지 아이템 가져오기 시도" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket2_.m_iProductNo )
			<< END_LOG;

		return NetError::ERR_NX_SHOP_01;
	}

	// 3. 장착 가능한 클래스인지 검사한다.
	if( !m_kInventory.CompareUnitClass( iItemID ) )
	{
		return NetError::ERR_NX_SHOP_07;
	}

	// 3-1. 구매 제한 레벨을 검사한다.
	if( kBillProductInfo.m_cLimitLevel > GetLevel() )
	{
		return NetError::ERR_BUY_CASH_ITEM_59;
	}

	// 4. 가져오려는 아이템이 인벤토리 또는 은행에 있으면서 기간제이면 막는다.
	if( m_kInventory.IsExistWithBank( iItemID ) )
	{
		if( kBillProductInfo.m_cPeriod > 0 )
		{
			return NetError::ERR_NX_SHOP_08;
		}
	}

	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	switch( iItemID )
	{
	case CXSLItem::CI_GUILD_NAME_CHANGE:
		if( m_kUserGuildManager.IsGuildMaster() == true )
		{
			// 길드 이름 변경권은 예외 처리
			KEGS_BILL_GET_PURCHASED_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::NET_OK;
			kPacket.m_iExceptionProcessItemID = iItemID;
			SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK, kPacket );

			return NetError::NET_OK;
		}
		else
		{
			return NetError::ERR_BUY_CASH_ITEM_67;
		}
		break;
	}
		//}}

	switch ( GetUnitType() )
	{

	case CXSLUnit::UT_ARA:

	case CXSLUnit::UT_ELESIS:

	case CXSLUnit::UT_ADD:
		{
			switch( iItemID )
			{
			case 160570:		// 리폼 웨딩 무기 큐브
			case 160571:		// 리폼 웨딩 상의 큐브
			case 160572:		// 리폼 웨딩 하의 큐브
			case 160573:		// 리폼 웨딩 장갑 큐브
			case 160574:		// 리폼 웨딩 신발 큐브
			case 160575:		// 리폼 웨딩 헤어 큐브
				{
					return NetError::ERR_NX_SHOP_07;
				}break;
			}

		} break;

	default:
		break;
	}

	//{{ 2012. 12. 14  캐시 인벤토리에서 아라가 사용 불가능한 아이템 가져오는거 막는 기능 - 김민성
	switch( iItemID )
	{
	case 261590:		// 천년 여우의 스킬 슬롯 체인지 메달 패키지
	case 261600:		// 천년 여우의 마법의 목걸이(II) 패키지
	case 261610:		// 천년 여우의 퀵슬롯 확장권 패키지
	case 261620:		// 천년 여우의 축복 패키지
		{
			if( GetUnitType() != CXSLUnit::UT_ARA )
			{
				return NetError::ERR_NX_SHOP_07;
			}
		}break;
	}
	//}}

	switch( iItemID )
	{
	case 261664:		//	엘리시스 붉은 기사단 패키지
	case 261665:		//	스킬 슬롯 체인지 메달(엘리시스 전용) 패키지
	case 261666:		//	마법의 목걸이(II)(엘리시스 전용) 패키지
	case 261667:		//	퀵슬롯 확장권(엘리시스 전용) 패키지
		{
			if( GetUnitType() != CXSLUnit::UT_ELESIS )
			{
				return NetError::ERR_NX_SHOP_07;
			}
		}break;
	}

	// 캐릭터 인벤에서는 아이템으로 존재하지 않는 캐시아이템의 경우에 중복구매에 대한 예외 처리
	switch( iItemID )
	{
	case CXSLItem::CI_EXPAND_SKILL_SLOT: // 4.1 가져오려는 아이템이 스킬슬롯B(기간제) 이면, 이미 스킬슬롯B(기간제, 혹은 영구)를 사용중이면 가져올 수 없게 막는다
		{
			KUserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE eSkillSlotBExpireState = m_kSkillTree.GetSkillSlotBExpirationState();
			if( KUserSkillTree::SSBES_EXPIRED != eSkillSlotBExpireState )
			{
				return NetError::ERR_BUY_CASH_ITEM_32;
			}
		} break;


		// 4.2 캐시스킬포인트를 이미 사용하고 있는데 포인트가 다른 캐시스킬포인트를 가져와서 기간 연장을 하려고 하면 막는다
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
	case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
		{
			if( false == m_kSkillTree.IsCashSkillPointExpired() )
			{
				const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( iItemID );

				if( iCSPoint <= 0 )
				{
					return NetError::ERR_RESET_SKILL_01;
				}

				if( iCSPoint != m_kSkillTree.GetMaxCSPoint() )
				{
					return NetError::ERR_BUY_CASH_ITEM_32;				// fix!! 에러메세지 수정
				}
			}
		} break;
	}

	// 인벤토리 여유 공간 검사
	switch( iItemID )
	{
	case CXSLItem::CI_EXPAND_SKILL_SLOT:
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
	case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
	case CXSLItem::CI_WARP_VIP_ITEM:
	case CXSLItem::CI_GUILD_NAME_CHANGE:
		{
			// 캐시인벤에서 내 캐릭터로 가져올 때 내 인벤에 아이템으로 들어가지 않는 것들은 인벤토리 여유공간 검사를 하지 않는다.

		} break;

	default:
		{
			// 5. 여유공간 검사를 위한 준비
			int iQuantity = ( int )kBillProductInfo.m_cQuantity;
			mapItem.insert( std::make_pair( iItemID, iQuantity ) );

			// 6. 인벤토리의 여유공간 검사
			if( !m_kInventory.IsEnoughSpaceExist( mapItem ) )
			{
				return NetError::ERR_NX_SHOP_02;
			}
		}
		break;
	}

	// 7. 아이템에 부여될 소켓옵션
	//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
	if( m_kUserCashInventory.SetSocketForCash( kPacket2_.m_mapSocketForCash, kPacket2_.m_mapSocketGroupIDForCash ) == false )
	{
		// 정상적이지 않는 소켓 정보를 셋팅하려 합니다.
		return NetError::ERR_SOCKET_ITEM_09;
	}
	//}

	KEBILL_PICK_UP_REQ kPacket;
	kPacket.m_iTransNo = kPacket2_.m_iTransNo;
	kPacket.m_iProductNo = kPacket2_.m_iProductNo;
	kPacket.m_iUnitUID = GetCharUID();
	kPacket.m_mapSocketForCash = kPacket2_.m_mapSocketForCash;
	kPacket.m_byteProductKind = kPacket2_.m_byteProductKind;
	SendToKOGBillingDB( EBILL_PICK_UP_REQ, kPacket );

	return NetError::NET_OK;
}

bool KGSUser::IsDirectPickUpItem( IN const int iItemID )
{
	switch( iItemID )
	{
	case CXSLItem::CI_RESURRECTION_STONE:
	case CXSLItem::CI_EXPAND_INVENTORY:
	case CXSLItem::CI_EXPAND_INVENTORY_EQUIP:
	case CXSLItem::CI_EXPAND_INVENTORY_ACCESSORY:
	case CXSLItem::CI_EXPAND_INVENTORY_QUICK_SLOT:
	case CXSLItem::CI_EXPAND_INVENTORY_MATERIAL:
	case CXSLItem::CI_EXPAND_INVENTORY_QUEST:
	case CXSLItem::CI_EXPAND_INVENTORY_SPECIAL:
	case CXSLItem::CI_EXPAND_CHAR_SLOT_1:
	case CXSLItem::CI_EXPAND_CHAR_SLOT_2:
	case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
		//{{ 2009. 8. 7  최육사		은행
	case CXSLItem::CI_BANK_MEMBERSHIP_SILVER:
	case CXSLItem::CI_BANK_MEMBERSHIP_GOLD:
	case CXSLItem::CI_BANK_MEMBERSHIP_EMERALD:
	case CXSLItem::CI_BANK_MEMBERSHIP_DIAMOND:
	case CXSLItem::CI_BANK_MEMBERSHIP_PLATINUM:
	case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
		//}}
		//{{ 2009. 9. 22  최육사	전직캐쉬
	case CXSLItem::CI_CHANGE_JOB_ELSWORD_KNIGHT:
	case CXSLItem::CI_CHANGE_JOB_MAGIC_KNIGHT:
	case CXSLItem::CI_CHANGE_JOB_COMBAT_RANGER:
	case CXSLItem::CI_CHANGE_JOB_SNIPING_RANGER:
	case CXSLItem::CI_CHANGE_JOB_HIGH_MAGICIAN:
	case CXSLItem::CI_CHANGE_JOB_DARK_MAGICIAN:
	case CXSLItem::CI_CHANGE_JOB_SOUL_TAKER:
	case CXSLItem::CI_CHANGE_JOB_OVER_TAKER:
	case CXSLItem::CI_CHANGE_JOB_EXOTIC_GEAR:
	case CXSLItem::CI_CHANGE_JOB_ARCHITECTURE:

	case CXSLItem::CI_CHANGE_JOB_LORD_KNIGHT:
	case CXSLItem::CI_CHANGE_JOB_RUNE_SLAYER:
	case CXSLItem::CI_CHANGE_JOB_WIND_SNEAKER:
	case CXSLItem::CI_CHANGE_JOB_GRAND_ARCHER:
	case CXSLItem::CI_CHANGE_JOB_ELMENTAL_MASTER:
	case CXSLItem::CI_CHANGE_JOB_VOID_PRINCESS:
	case CXSLItem::CI_CHANGE_JOB_BLADE_MASTER:
	case CXSLItem::CI_CHANGE_JOB_RECKLESS_FIST:
	case CXSLItem::CI_CHANGE_JOB_NEMESIS:
	case CXSLItem::CI_CHANGE_JOB_EMPRESS:
	case CXSLItem::CI_CHANGE_JOB_FURY_GUARDIAN:
	case CXSLItem::CI_CHANGE_JOB_SHOOTER_GUARDIAN:
	case CXSLItem::CI_CHANGE_JOB_IRON_PALADIN:
	case CXSLItem::CI_CHANGE_JOB_DEADLY_CHASER:
	case CXSLItem::CI_CHANGE_JOB_SHEATH_KNIGHT:
	case CXSLItem::CI_CHANGE_JOB_BATTLE_MAGICIAN:
	case CXSLItem::CI_CHANGE_JOB_TRAPPING_RANGER:
	case CXSLItem::CI_CHANGE_JOB_WEAPON_TAKER:
	case CXSLItem::CI_CHANGE_JOB_EVE_ELECTRA:
	case CXSLItem::CI_CHANGE_JOB_SHELLING_GUARDIAN:
	case CXSLItem::CI_CHANGE_JOB_INFINITY_SWORD:
	case CXSLItem::CI_CHANGE_JOB_DIMENSION_WITCH:
	case CXSLItem::CI_CHANGE_JOB_NIGHT_WATCHER:
	case CXSLItem::CI_CHANGE_JOB_VETERAN_COMMANDER:
	case CXSLItem::CI_CHANGE_JOB_BATTLE_SERAPH:
	case CXSLItem::CI_CHANGE_JOB_TACTICAL_TROOPER:
		//}}

		//{{ 2010. 04. 27  최육사	기술의 노트
	case CXSLItem::CI_CASH_SKILL_NOTE_ITEM:
		//}}
		//{{ 2011. 04. 14	최육사	대리 상인 / 대리 상인은 캐쉬 인벤에서 직접 사용할 수 있도록 하자
		//#ifdef SERV_PSHOP_AGENCY
		//		case CXSLItem::CI_PERSONAL_SHOP_AGENCY:
		//#endif
		//}}
		//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
	case CXSLItem::CI_CASH_SHARING_BACK_OPEN:
		//}}
	case CXSLItem::CI_EXPAND_QUICK_SLOT:
	case CXSLItem::CI_UNLIMITED_CHANGE_SECOND_JOB:
		{
			return true;
		}
		break;

		//{{ 2009. 8. 4  최육사		봉인스킬
	default:
		{

			if( SiCXSLSkillTree()->IsUnsealItemID( iItemID )  || CXSLItem::IsJobChangeCashItem( iItemID )
				|| CXSLItem::GetClassChangeCashItem(iItemID) != CXSLUnit::UC_NONE
				)
			{
				return true;
			}
		}
		break;
		//}}
	}

	return false;
}

int KGSUser::OnPickUpAck( const KENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK& kPacket_, const KEBILL_PICK_UP_ACK& kPacket2_, std::vector< KInventoryItemInfo >& vecInventorySlotInfo )
{
	if( kPacket2_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"캐쉬 아이템 가져 오기 실패." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket2_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket2_.m_iOK ) )
			<< END_LOG;

		return NetError::ERR_NX_SHOP_03;
	}

	std::map< int, KItemInfo > mapItem; // 인벤토리에 들어갈 아이템들
	//{{ 2009. 9. 2  최육사		밀봉
	std::set< int > setSealCashItem;
	//}}

	int iItemID = 0;
	int iQuantity = 0;
	int iPeriod = 0;
	KBillProductInfo kBillProductInfo;

	SiKBillManager()->GetProductInfo( kPacket2_.m_iProductNo, kBillProductInfo );

	iItemID = kBillProductInfo.m_iProductID;
	iQuantity = ( int )kBillProductInfo.m_cQuantity;
	iPeriod = ( int )kBillProductInfo.m_cPeriod;

	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"아이템 ID 변환 실패." )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< BUILD_LOG( kPacket2_.m_iProductNo )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return NetError::ERR_NX_SHOP_01;
	}

	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return NetError::ERR_NX_SHOP_01;
	}

	// 20080416. 여기까지 오면 정상적으로 아이템을 채워줘야함. 코드 수정시 이 점 주의 요망.

	// 캐쉬 아이템 가져오기 로그
	KELOG_GET_PURCHASED_CASH_ITEM_NOT kPacketNot;
	kPacketNot.m_iUnitUID = GetCharUID();
	kPacketNot.m_iItemID = iItemID;
	kPacketNot.m_sPeriod = ( short )iPeriod;
	kPacketNot.m_iQuantity = iQuantity;
	kPacketNot.m_ucLevel =  GetLevel();
	kPacketNot.m_cUnitClass = GetUnitClass();

	SendToLogDB( ELOG_GET_PURCHASED_CASH_ITEM_NOT, kPacketNot );


	switch( pItemTemplet->m_ItemID )
	{
	case CXSLItem::CI_RESURRECTION_STONE: // 부활석
		{
			// 부활석이면 바로 채워준다.
			m_iNumResurrectionStone += iQuantity;

			START_LOG( clog2, L"resurr enter" )
				<< END_LOG;

			// 실시간 부활석
			if( m_kUserCashInventory.IsCashResurrectionStone() )
			{
				START_LOG( clog2, L"resurr send" )
					<< END_LOG;
				KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ kPacketReq;
				kPacketReq.m_iUnitUID		= m_kUserCashInventory.GetCashRStoneUnitUID();
				kPacketReq.m_iUsedByUnitUID = GetCharUID();
				SendToCnRoom( ERM_RESURRECT_TO_CONTINUE_DUNGEON_REQ, kPacketReq );

			}


			return NetError::NET_OK;
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY: // 소지품 확장
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot;
			nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * iQuantity;

			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_EQUIP, nSlot ) );
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_ACCESSORY, nSlot ) );
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_MATERIAL, nSlot ) );
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_SPECIAL, nSlot ) );
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUEST, nSlot ) );
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUICK_SLOT, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09;
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_EQUIP: // 소지품 확장(장비)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_EQUIP, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_ACCESSORY: // 소지품 확장(액세서리)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_ACCESSORY, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_QUICK_SLOT: // 소지품 확장(소비)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUICK_SLOT, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_MATERIAL: // 소지품 확장(재료)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_MATERIAL, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_QUEST: // 소지품 확장(퀘스트)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUEST, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_INVENTORY_SPECIAL: // 소지품 확장(특수)
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_SPECIAL, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			//continue;
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;
		//{{ 2011. 12. 13  김민성	퀵 슬롯 확장 아이템
	case CXSLItem::CI_EXPAND_QUICK_SLOT:
		{
			// 소지품 확장이면 DB로 이벤트를 보낸다.
			int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;
		//}}

	case CXSLItem::CI_EXPAND_SKILL_SLOT: // 스킬 슬롯 체인지 메달
		{
			KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
			kPacket.m_iUnitUID		= GetCharUID();
			kPacket.m_iPeriodExpire = iPeriod;
			SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;

	case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
		{
			KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
			kPacket.m_iUnitUID		= GetCharUID();
			kPacket.m_iPeriodExpire = 0;
			SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		}
		break;
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
	case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
		{
			const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( pItemTemplet->m_ItemID );

			if( iCSPoint <= 0 )
			{
				return NetError::ERR_RESET_SKILL_01;
			}

			if( true == m_kSkillTree.IsCashSkillPointExpired() )
			{
				KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
				kPacket.m_iUnitUID	= GetCharUID();
				kPacket.m_iCSPoint	= iCSPoint;
				//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
				kPacket.m_iPeriod	= iPeriod;
				//}}
				kPacket.m_bUpdateEndDateOnly = false;
				//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
				kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
				//}}
				SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
			}
			else
			{
				if( iCSPoint == m_kSkillTree.GetMaxCSPoint() )
				{
					KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_iCSPoint = iCSPoint;
					//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
					kPacket.m_iPeriod = iPeriod;
					//}}
					kPacket.m_bUpdateEndDateOnly = true;
					//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
					kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
					//}}
					SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
				}
				else
				{
					START_LOG( cerr, L"현재 사용하고 있는 캐시스킬포인트와 다른 것으로 기간연장 하려고함!!" )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( pItemTemplet->m_ItemID )
						//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
						<< BUILD_LOG( iPeriod )
						//}}
						<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
						<< BUILD_LOG( m_kSkillTree.GetCSPointEndDateString() )
						<< END_LOG;

					return NetError::ERR_NX_SHOP_01;
				}
			}

			return NetError::ERR_NX_SHOP_09; // 아이템 생성하지 않고 바로 리턴
		} break;
		//{{ 2008. 12. 14  최육사	캐릭터 슬롯 확장
	case CXSLItem::CI_EXPAND_CHAR_SLOT_1:
	case CXSLItem::CI_EXPAND_CHAR_SLOT_2:
		{
			KDBE_EXPAND_CHAR_SLOT_REQ kPacket;
			kPacket.m_iExpandSlotSize = ( pItemTemplet->m_ItemID == CXSLItem::CI_EXPAND_CHAR_SLOT_1 ? 1 : 2 );
			kPacket.m_iCharSlotMax = CXSLUnit::CHAR_SLOT_MAX;
			SendToGameDB( DBE_EXPAND_CHAR_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09;
		}
		break;
		//}}

		//{{ 2009. 8. 7  최육사		은행
	case CXSLItem::CI_BANK_MEMBERSHIP_SILVER:
	case CXSLItem::CI_BANK_MEMBERSHIP_GOLD:
	case CXSLItem::CI_BANK_MEMBERSHIP_EMERALD:
	case CXSLItem::CI_BANK_MEMBERSHIP_DIAMOND:
	case CXSLItem::CI_BANK_MEMBERSHIP_PLATINUM:
		{
			// 은행 확장하러 가자!
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, CXSLInventory::SLOT_COUNT_ONE_LINE ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09;
		}
		break;

	case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
		{
			// 은행 확장하러 가자!
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
			kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, ( CXSLInventory::SLOT_COUNT_ONE_LINE * 5 ) ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
			return NetError::ERR_NX_SHOP_09;
		}
		break;
		//}}

		//{{ 2010. 04. 09  최육사	기술의 노트
	case CXSLItem::CI_CASH_SKILL_NOTE_ITEM:
		{
			char cSkillNoteMaxPageNum = 0;
			if( m_kSkillTree.GetExpandSkillNotePage( GetLevel(), cSkillNoteMaxPageNum ) == true )
			{
				// 일단 기술의 노트 페이지 수를 저장하자!
				m_kSkillTree.UpdateSkillNoteMaxPageNum( cSkillNoteMaxPageNum );

				// 기술의 노트를 획득 하러 가자!
				KDBE_EXPAND_SKILL_NOTE_PAGE_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID = GetCharUID();
				kPacketToDB.m_cExpandedMaxPageNum = cSkillNoteMaxPageNum;
				kPacketToDB.m_cRollBackMaxPageNum = 0;
				SendToGameDB( DBE_EXPAND_SKILL_NOTE_PAGE_REQ, kPacketToDB );
				return NetError::ERR_NX_SHOP_09;
			}
			else
			{
				START_LOG( cerr, L"기술의 노트를 습득하려고 하는데 페이지 수 얻기를 실패하였습니다." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetLevel() )
					<< END_LOG;
			}
		}
		break;
		//}}
		//{{ 2011. 04. 14	최육사	대리 상인
	case CXSLItem::CI_PERSONAL_SHOP_AGENCY:
		{
			KDBE_INSERT_PERIOD_PSHOP_AGENCY_REQ kPacketToDB;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_sAgencyPeriod = static_cast<short>(iPeriod);
			kPacketToDB.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
			SendToGameDB( DBE_INSERT_PERIOD_PSHOP_AGENCY_REQ, kPacketToDB );

			// 지헌 : 캐쉬 인벤 목록을 갱신 해야 하는데....
			//		NET_OK 리턴해주면 되지 않을까?
			return NetError::NET_OK;
			//return NetError::ERR_NX_SHOP_09;// 지헌 : 임시
		}
		break;
		//}}
	case CXSLItem::CI_EXPAND_SKILL_PAGE:
		{
			SendExpandSkillPageReqToGameDB( EBILL_PICK_UP_ACK, kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity );
			return NetError::NET_OK;
		}
		break;
		//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
	case CXSLItem::CI_CASH_SHARING_BACK_OPEN:
		{
			// 캐쉬로 은행 공유~
			KDBE_SHARING_BACK_OPEN_REQ kPacketToDB;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_iUserUID = GetUID();
			kPacketToDB.m_iOpenType = KDBE_SHARING_BACK_OPEN_REQ::SBOT_CASH;
			SendToGameDB( DBE_SHARING_BACK_OPEN_REQ, kPacketToDB );

			return NetError::ERR_NX_SHOP_09;
		}
		break;
		//}}
	case CXSLItem::CI_WARP_VIP_ITEM:
		{
			KDBE_INSERT_WARP_VIP_REQ kPacket;
			kPacket.m_iUnitUID	= GetCharUID();
			kPacket.m_iPeriod	= 30;
			SendToGameDB( DBE_INSERT_WARP_VIP_REQ, kPacket );

			return NetError::NET_OK; // 아이템 생성하지 않고 바로 리턴
		}
		break;
		//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	case CXSLItem::CI_GUILD_NAME_CHANGE:
		{
			KELG_CHANGE_GUILD_NAME_REQ kPacket;
			kPacket.m_iUnitUID = GetCharUID();
			kPacket.m_iGuildUID = GetGuildUID();
			kPacket.m_wstrOldGuildName = m_kUserGuildManager.GetGuildName();
			// m_wstrNewGuildName은 LoginServer 쪽에서 얻을 것이다.
			SendToLoginServer( ELG_CHANGE_GUILD_NAME_REQ, kPacket );

			return NetError::NET_OK;
		}
		break;

	case CXSLItem::CI_UNLIMITED_CHANGE_SECOND_JOB:
		{
			CStringW cwstrItemID;
			cwstrItemID.Format( L"%d", CXSLItem::CI_UNLIMITED_CHANGE_SECOND_JOB );
			// 2차 전직 item보낸다.
			// DB로 보상 정보를 보낸다.
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_MESSAGE;
			kPacketToDB.m_iRewardID	   = CXSLItem::SI_SECOND_CHANGE_JOB_CUBE;
			kPacketToDB.m_sQuantity	   = 1;
			kPacketToDB.m_wstrMessage  = cwstrItemID.GetBuffer();
			kPacketToDB.m_bGameServerEvent = false;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
		break;
	default:
		{
			IF_EVENT_ENABLED( CEI_BALANCE_FINALITY_SKILL_EVENT )
			{
				if( iItemID == CXSLItem::SI_FINALITY_SKILL_BOOK )
				{
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
					kPacketToDB.m_iRewardID	   = 2048;	// 카밀라의 비전서 (초급)
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}

			//{{ 2009. 9. 22  최육사	전직캐쉬
			if( CXSLItem::IsJobChangeCashItem( iItemID ) )
			{
				// 전직 하러 가자!
				KDBE_CHANGE_UNIT_CLASS_REQ kPacket;
				kPacket.m_usEventID = EGS_BUY_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_cUnitClass = CXSLItem::GetCashItemChangeUnitClass( pItemTemplet->m_ItemID );

				kPacket.m_iTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

				int iDefaultSkill[6] = {0,};
				if( SiCXSLSkillTree()->GetUnitClassDefaultSkill( kPacket.m_cUnitClass, iDefaultSkill[0], iDefaultSkill[1], iDefaultSkill[2], iDefaultSkill[3], iDefaultSkill[4], iDefaultSkill[5] ) == false )
				{
					START_LOG( cerr, L"전직시 지급하려는 기본 스킬이 이상함" )
						<< BUILD_LOG( kPacket.m_iUnitUID )
						<< BUILD_LOGc( kPacket.m_cUnitClass )
						<< END_LOG;
				}

				if( CXSLUnit::IsInitNormalJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
				{
					kPacket.m_iNewDefaultSkill1 = iDefaultSkill[0];
					kPacket.m_iNewDefaultSkill2 = iDefaultSkill[1];
				}
				else if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
				{
					kPacket.m_iNewDefaultSkill1 = iDefaultSkill[2];
					kPacket.m_iNewDefaultSkill2 = iDefaultSkill[3];
				}
				else if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
				{
					kPacket.m_iNewDefaultSkill1 = iDefaultSkill[4];
					kPacket.m_iNewDefaultSkill2 = iDefaultSkill[5];
				}
				SendToGameDB( DBE_CHANGE_UNIT_CLASS_REQ, kPacket );
				return NetError::ERR_NX_SHOP_09;
			}
			//}}
			//{{ 2011. 11. 21  김민성	전직 변경 아이템
			int iUnitClass = 0;
			if( CXSLItem::GetClassChangeCashItem( iItemID ) != CXSLUnit::UC_NONE )
			{
				KDBE_BUY_UNIT_CLASS_CHANGE_REQ kPacketToDB;

				if( GetUnitClassChangeInfo( iItemID, kPacketToDB ) == true )
				{
					SendToGameDB( DBE_BUY_UNIT_CLASS_CHANGE_REQ, kPacketToDB );
				}
				else
				{
					START_LOG( cerr, L"캐쉬 아이템 픽업 과정에서 실패가 났다! 복귀 해야 할 듯 한데!!!" )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( iItemID )
						<< END_LOG;
				}
				return NetError::ERR_NX_SHOP_09;
			}
			//}}

			//{{ 2009. 8. 4  최육사		봉인스킬
			std::vector<int> vecSkillID;
			if( SiCXSLSkillTree()->GetUnsealItemInfo( iItemID, GetUnitClass(), vecSkillID ) == true )
			{
				for( int iSkillID : vecSkillID )
				{
					KDBE_UNSEAL_SKILL_REQ kPacketToDB;
					kPacketToDB.m_iUnitUID = GetCharUID();
					kPacketToDB.m_iSkillID = iSkillID;
					SendToGameDB( DBE_UNSEAL_SKILL_REQ, kPacketToDB );
				}
				return NetError::ERR_NX_SHOP_09;
			}
			//}}
		}
		//}}
	}

	KItemInfo kItemInfo;
	kItemInfo.m_iItemID		= iItemID;
	kItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
	kItemInfo.m_iQuantity	= iQuantity;
	kItemInfo.m_sEndurance	= ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;
	kItemInfo.m_sPeriod		= ( short )iPeriod;
	//{{ 2009. 8. 27  최육사	밀봉
	if( SiCXSLItemManager()->IsCanSealForCashItem( iItemID ) == true )
	{
		setSealCashItem.insert( pItemTemplet->m_ItemID );
	}
	//}}

	//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
	int iSocketOption = 0;
	if( m_kUserCashInventory.GetSocketOptionForCash( iItemID, iSocketOption ) )
	{
		int iItemType = static_cast<int>( pItemTemplet->m_ItemType );

		if( SiCXSLSocketItem()->CheckCashItemSocketID( iItemType, iSocketOption ) == true )
		{
			kItemInfo.m_vecItemSocket.push_back( iSocketOption );
		}
	}
	//}

	mapItem.insert( std::make_pair( iItemID, kItemInfo ) );


	// 인벤토리에 아이템 넣을 준비
	KDBE_INSERT_ITEM_REQ kPacketReq;
	//{{ 2010. 9. 8	최육사	아이템 획득 사유
	kPacketReq.m_cGetItemReason = SEnum::GIR_BUY_CASH_ITEM;

	m_kInventory.PrepareInsert( mapItem, kPacketReq.m_mapInsertedItem, kPacketReq.m_vecUpdatedInventorySlot, kPacketReq.m_vecItemInfo );

	if( !kPacketReq.m_vecItemInfo.empty() )
	{
		//{{ 2009. 9. 2  최육사		봉인
		kPacketReq.m_setSealCashItem = setSealCashItem;
		//}}
		kPacketReq.m_iUnitUID = GetCharUID();
		m_kInventory.FlushQuantityChange( kPacketReq.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.FlushEnduranceChange( kPacketReq.m_kItemEnduranceUpdate );
		m_kInventory.FlushDeletedItem( kPacketReq.m_kItemQuantityUpdate.m_vecDeleted );
		SendToGameDB( DBE_INSERT_PURCHASED_CASH_ITEM_REQ, kPacketReq );
		return NetError::ERR_NX_SHOP_09;
	}
	else
	{
		vecInventorySlotInfo = kPacketReq.m_vecUpdatedInventorySlot;
	}

	// 캐쉬 아이템 통계
	std::map< int, int >::const_iterator mit;
	for( mit = kPacketReq.m_mapInsertedItem.begin(); mit != kPacketReq.m_mapInsertedItem.end(); ++mit )
	{
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( mit->first );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_MALL, mit->second );
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_MALL, mit->second );
	}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	// 아이템 어뷰저
	m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_CASH_ITEM, kPacketReq.m_mapInsertedItem );
	//}}

	return NetError::NET_OK;
}

//
std::wstring KGSUser::MakeOrderID()
{
	std::wstring wstrRet;

	CTime tCurrent = CTime::GetCurrentTime();
	wstrRet = ( CStringW )tCurrent.Format( _T( "%Y%m%d%H%M%S" ) );
	wchar_t wszNumber[32];
	_i64tow_s( KBaseServer::GetKObj()->GetUID(), wszNumber, 32, 10 ); // Port: _i64tow (no size) -> _i64tow_s
	wstrRet += L"_";
	wstrRet += wszNumber;
	_i64tow_s( GetUID(), wszNumber, 32, 10 );
	wstrRet += L"_";
	wstrRet += wszNumber;
	_i64tow_s( SiKBillManager()->GetNextPurchaseNo(), wszNumber, 32, 10 );
	wstrRet += L"_";
	wstrRet += wszNumber;

	return wstrRet;
}

// 국가별로 사용가능한 캐시 종류가 다름
bool KGSUser::IsAbleToUseCashType(int iCashType)
{
	if(iCashType < 0 || iCashType >= KGlobalCashInfo::GCT_TOTAL_NUM)
		return false;

	// 중국,대만홍콩 제외하고는 캐시 종류 하나밖에 안씀
	if (iCashType == KGlobalCashInfo::GCT_PUBLISHER_CASH)
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////

void KGSUser::ProcessEvent( const KEventPtr& spEvent_ )
{
	if( RoutePacket( spEvent_.get() ) )
		return;

	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}

	CheckRepeatEvent( spEvent_->m_usEventID );

	if( false == IsTransactionEnabled( spEvent_->m_usEventID ) )
		return;

	CheckTransactionReqEvent( spEvent_->m_usEventID ); // EGS_
	CheckTransactionAckEvent( spEvent_->m_usEventID ); // DBE_

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	//{{ 2013. 05. 29	최육사	패킷 모니터링 근성도 항목 추가
	m_kUserAbuserManager.CheckMornitoringAbuser( GetUID(), GetCharUID(), GetED(), m_kUserSpiritManager.GetSpirit(), spEvent_->m_usEventID );
	//}}
	//}}

	//{{ 2009. 9. 13  최육사	유저로그
	m_kUserLogManager.SaveProcessEvent( spEvent_->m_usEventID );
	//}}

	DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

	switch( spEvent_->m_usEventID )
	{
	//////////////////////////////////////////////////////////////////////////
		//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
#	undef EVENT_HANDLER
#	define EVENT_HANDLER( eventid ) CASE_CHECK_ERR_AND_FILTER( eventid );
#	undef EVENT_HANDLER_NOPARAM
#	define EVENT_HANDLER_NOPARAM( eventid ) CASE_CHECK_ERR_AND_FILTER_NOPARAM( eventid );
#	undef _EVENT_HANDLER
#	define _EVENT_HANDLER( eventid, structtype ) _CASE_CHECK_ERR_AND_FILTER( eventid, structtype );
#pragma message( "--------------------------------------------" )
#pragma message( "Server Packet Event Filter!!!!!" )
#pragma message( "--------------------------------------------" )
	//}}

	// 여기서 부터 GSUser Event Handler의 헤더 파일을 추가합니다.
#	include "GSUserSession.h"
#	include "GSUserSecurity.h"
#	include "GSUserGameCommon.h"
#	include "GSUserField.h"
	//{{ 2011. 12. 05	최육사	배틀필드
#	include "GSUserNewParty.h"
	//}}
#	include "GSUserInventory.h"
#	include "GSUserMessenger.h"
#	include "GSUserRoomCommon.h"
#	include "GSUserDungeon.h"
#	include "GSUserPvp.h"
#	include "GSUserTrade.h"
#	include "GSUserPersonalShop.h"
#	include "GSUserCashShop.h"
	//////////////////////////////////////////////////////////////////////////

	default:
		if( ProcessEventExpand( spEvent_ ) == false)
		{
			START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음 - " << spEvent_->GetIDStr() )
				<< BUILD_LOG( spEvent_->m_usEventID );
		}
		break;
	}

	dwElapTime = ::GetTickCount() - dwElapTime;
	if( dwElapTime > 3000 )
	{
		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( dwElapTime );
	}

	//{{ 2011. 12. 14	최육사	패킷 처리 지연 시간 체크
	SiKPacketMornitoring()->CheckPacketProcessingLatency( spEvent_->m_usEventID, GetCharUID(), dwElapTime );
	//}}
}

// 이제 추가 되는 이벤트는 여기에 추가하도록 한다.
bool KGSUser::ProcessEventExpand( const KEventPtr& spEvent_ )
{
	switch( spEvent_->m_usEventID )
	{
#pragma message( "--------------------------------------------" )
#pragma message( "Server Packet Event Filter!!!!! - ProcessEventExpand " )
#pragma message( "--------------------------------------------" )

#	include "GSUserPet.h"
#	include "GSUserGuild.h"
#	include "GSUserLetter.h"
#	include "GSUserAdminCheat.h"
#	include "GSUserMatch.h"
		//{{ 2011. 10. 27	최육사	배틀필드 시스템
#	include "GSUserBattleField.h"
#	include "GSUserAutoParty.h"
		//}}

//#ifdef SERV_COUNTRY_CN
#	include "GSUserGiant.h"
//#endif SERV_COUNTRY_CN
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
#	include "GSUserRelationship.h"
		//}

		default:
			return false;
	}

	return true;
}

//{{ 2011. 04. 05  김민성	우편 및 거래 감시
void KGSUser::CheckLetterAndTradeMonitoring()
{
	if( SiKGameSysVal()->GetLetterAndTradeMonitoringIsOn() == false )
		return;

	if( m_kTimer[TM_MONITORING_LETTER].elapsed() > SiKGameSysVal()->GetLetterMonitoringCheckTime() )
	{
		m_kUserHackingManager.ClearLetterMonitoringList();
		m_kTimer[TM_MONITORING_LETTER].restart();
	}

	if( m_kTimer[TM_MONITORING_TRADE].elapsed() > SiKGameSysVal()->GetTradeMonitoringCheckTime() )
	{
		m_kUserHackingManager.ClearTradeMonitoringList();
		m_kTimer[TM_MONITORING_TRADE].restart();
	}
}
//}}

//{{ 2011. 04. 26	최육사	deserialize 실패 종료
void KGSUser::CheckDeserializeFailCount()
{
	if( m_kTimer[TM_DESERIALIZE_FAIL].elapsed() < 300.0 )
		return;

	m_kTimer[TM_DESERIALIZE_FAIL].restart();

	// 체크 할지 안할지 보자!
	if( SiKGameSysVal()->IsDeserializeFailCheck() == false )
		return;

	// 5분마다 한번씩 체크하자!
    if( m_kUserHackingManager.HackingCheckDeserializeFailCount() == true )
	{
		if( GetAuthLevel() >= SEnum::UAL_DEVELOPER )
		{
			START_LOG( cout, L"개발자인데 Deserialize Fail Hacking Check에 걸려들었네? 코드 잘못 짰나;;" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() );
		}
		else
		{
			START_LOG( cout, L"[해킹감지!] Deserialize Fail Hacking Check에 유저가 체크 되었습니다! 모니터링 유저에 등록하고 거래차단 되었습니다!" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() );
			//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
			HackUserRegMonitorsUser( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DESERIALIZE_FAIL );
			HackUserRegTradeBlock( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DESERIALIZE_FAIL );

			KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
			kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DESERIALIZE_FAIL;
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );

			m_kUserHackingManager.SetHackingUserSendType( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_DESERIALIZE_FAIL );
			//}}
		}
	}
}
//}}

//{{ 2012. 10. 15	최육사		ED&아이템 모니터링 시스템
//}}


//{{ 2012. 05. 31	최육사	배틀필드 시스템
void KGSUser::CheckAutoPartyBonus()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kTimer[TM_AUTO_PARTY_BONUS_CHECK].elapsed() < 5.0 ) // 간격이 짧다 체크를 잘하자!
			return;

		m_kTimer[TM_AUTO_PARTY_BONUS_CHECK].restart();

		// 현재 보너스 정보 얻어 오자!
		//KEGS_UPDATE_AUTO_PARTY_BONUS_INFO_NOT kPacket;
		//m_kUserDungeonManager.CheckAutoPartyBonusRefresh( kPacket.m_mapDungeonBonusList );

		//if( kPacket.m_mapDungeonBonusList.empty() == false )
		//{
		//	SendPacket( EGS_UPDATE_AUTO_PARTY_BONUS_INFO_NOT, kPacket );
		//}
	}
}
//}}

//{{ 2011. 05. 02  김민성	2차 보안 시스템
bool KGSUser::GetSuccessSecondSecurity()
{
	if( GetIsUseOTP() == false )
	{
		return m_bPassSecondSecurity;
	}
	else
	{
		if( GetIsUseSecondPW() == true )
		{
			return m_bPassSecondSecurity;
		}
		else
		{
			return true;
		}
	}
}

bool KGSUser::IsSameSecondSecurityPW( IN const std::wstring& wstrSecondPW )
{
	if( m_wstrSecondPW.compare( wstrSecondPW ) == 0 )
	{
		return true;
	}

	START_LOG( clog, L"2차 보안 비밀번호의 맞지 않습니다." )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( m_wstrSecondPW )
		<< BUILD_LOG( wstrSecondPW )
		<< END_LOG;

	return false;
}

bool KGSUser::CheckIntegritySecondSecurityPW( IN const std::wstring& wstrSecondPW, OUT int& iOK )
{
	iOK = NetError::NET_OK;

	// 비어 있거나
	if( wstrSecondPW.empty() == true )
	{
		iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
		return false;
	}

	// 4자리 보다 작거나
	if( wstrSecondPW.size() < 4 )
	{
		iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
		return false;
	}

	// 6자리 보다 크거나
	if( wstrSecondPW.size() > 6 )
	{
		iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
		return false;
	}

	// 동일한 숫자인가
	{
		int iFirstNum = 0;
		int iNextNum = 0;
		int iCheck = 0;
		WCHAR _wchar;

		_wchar = wstrSecondPW[0];
		iFirstNum = _wchar - 48;				// '0' 의 값이 48 이다.

		for( unsigned int i = 0 ; i < wstrSecondPW.size() ; ++i )
		{
			_wchar = wstrSecondPW[i];
			iNextNum = _wchar - 48;
			if( iFirstNum == iNextNum )
			{
				++iCheck;
			}
		}

 		if( iCheck == wstrSecondPW.size() )
		{
			START_LOG( clog, L"동일한 숫자를 비밀번호로 설정하려한다." )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( wstrSecondPW );

			iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW_1;
			return false;
		}
	}

	// 연속된 숫자인가
	{
		int iFrontNum = 0;
		int iNextNum = 0;
		int iCheck = 1;			// 첫자리는 동일하므로 미리 하나 증가, 비교는 두번째 부터
		WCHAR _wchar;

		_wchar = wstrSecondPW[0];
		iFrontNum = _wchar - 48;				// '0' 의 값이 48 이다.

		for( unsigned int i = 1 ; i < wstrSecondPW.size() ; ++i )	// 오름순 검사
		{
			_wchar = wstrSecondPW[i];
			iNextNum = _wchar - 48;

			if( iFrontNum + 1 == iNextNum )
			{
				++iCheck;
			}
			iFrontNum = iNextNum;
		}

		if( iCheck == wstrSecondPW.size() )
		{
			START_LOG( clog, L"연속된 숫자를 비밀번호로 설정하려한다." )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( wstrSecondPW );

			iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW_2;
			return false;
		}

		iFrontNum = 0;
		iNextNum = 0;
		iCheck = 1;			// 첫자리는 동일하므로 미리 하나 증가, 비교는 두번째 부터

		_wchar = wstrSecondPW[0];
		iFrontNum = _wchar - 48;				// '0' 의 값이 48 이다.

		for( unsigned int i = 1 ; i < wstrSecondPW.size() ; ++i )	// 내림순 검사
		{
			_wchar = wstrSecondPW[i];
			iNextNum = _wchar - 48;

			if( iFrontNum - 1 == iNextNum )
			{
				++iCheck;
			}
			iFrontNum = iNextNum;
		}

		if( iCheck == wstrSecondPW.size() )
		{
			START_LOG( clog, L"연속된 숫자를 비밀번호로 설정하려한다." )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( wstrSecondPW );

			iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW_2;
			return false;
		}
	}

	// 숫자가 아닌 문자가 있거나
	int _count = (int)wstrSecondPW.size();

	for( int i = 0 ; i < _count ; ++i )
	{
		if( '0' >  wstrSecondPW[i] && '9' <  wstrSecondPW[i] )
		{
			iOK = NetError::ERR_SECOND_SECURITY_DEFECT_PW;
			return false;
		}
	}

	return true;
}

int KGSUser::GetSecurityType()
{
	// SecurityType 구분
	// 미사용							: 0
	// 2차 보안패드 사용				: 1
	// 핸드폰 OTP 사용					: 2
	// 지정 PC 사용						: 3
	// 넥슨 스틱 사용					: 4
	// 2차 보안 패드 + 핸드폰 OTP 사용	: 5
	// 2차 보안 패드 + 지정 PC 사용		: 6
	// 2차 보안 패드 + 넥슨 스틱 사용	: 7

	int iNexonSecurityType = GetNexonSecurityType();	// 0 OTP 미사용, 1 핸드폰OTP, 2 지정PC, 3 넥슨스틱
	int iSecurityType = 0;

	if( GetIsUseSecondPW() == true )	// 2차 보안 사용 중
	{
		switch( iNexonSecurityType )
		{
		case 0:
			{
				iSecurityType = 1;
			}break;
		case 1:
			{
				iSecurityType = 5;
			}break;
		case 2:
			{
				iSecurityType = 6;
			}break;
		case 3:
			{
				iSecurityType = 7;
			}break;
		default:
			{
				START_LOG( cerr, L"GetNexonSecurityType() 의 값이 이상합니다." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( iNexonSecurityType )
					<< BUILD_LOG( GetIsUseSecondPW() )
					<< END_LOG;
			}
		}
	}
	else		// 2차 보안 미사용 중
	{
		switch( iNexonSecurityType )
		{
		case 0:
			{
				iSecurityType = 0;
			}break;
		case 1:
			{
				iSecurityType = 2;
			}break;
		case 2:
			{
				iSecurityType = 3;
			}break;
		case 3:
			{
				iSecurityType = 4;
			}break;
		default:
			{
				START_LOG( cerr, L"GetNexonSecurityType() 의 값이 이상합니다." )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( iNexonSecurityType )
					<< BUILD_LOG( GetIsUseSecondPW() )
					<< END_LOG;
			}
		}
	}

	return iSecurityType;
}

void KGSUser::SendSecurityType()
{
	KDBE_UPDATE_SECURITY_TYPE_NOT kNot;
	kNot.m_iUserUID = GetUID();
	kNot.m_iSecurityType = GetSecurityType();

	SendToAccountDB( DBE_UPDATE_SECURITY_TYPE_NOT, kNot );
}

void KGSUser::SendFirstLoginSecurityInfo()
{
	CTime tCurr = CTime::GetCurrentTime();

	KDBE_FIRST_WRITE_SECURITY_TYPE_NOT kNot;

	kNot.m_iUserUID = GetUID();
	kNot.m_iAge	= (int)m_kNexonAccountInfo.m_uiAge;
	kNot.m_iSecurityType = GetSecurityType();
	kNot.m_wstrCurrTime = tCurr.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

	SendToLogDB( DBE_FIRST_WRITE_SECURITY_TYPE_NOT, kNot );
}

void KGSUser::SetIsUseOTP( IN const int& iNexonSecurityType )
{
	if( iNexonSecurityType > 0 )	// 0 은 미사용 , 1~3 은 OTP,지정PC,넥슨스틱
	{
		m_bIsUseOTP = true;
	}
	else
	{
		m_bIsUseOTP = false;
	}

	m_iNexonSecurityType = iNexonSecurityType;
}
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
void KGSUser::CheckComeBackUser()
{
	if( m_kComeBackUserInfo.GetIsComeBackUser() == true )
	{
		CTime tComeBackBuffEnd = m_kComeBackUserInfo.GetComeBackBuffEnd();

		if( m_kComeBackUserInfo.CheckComeBackBuffEnd() == true )
		{
			// 휴면 유저 해택 종료 DB에 기록
			// DB에 기록하자
			KDBE_WRITE_COME_BACK_END_NOT kNot;
			kNot = GetUID();
			SendToAccountDB( DBE_WRITE_COME_BACK_END_NOT, kNot );

			//{{ 버프 관리 시스템 - 김민성
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kBuffReq;
			kBuffReq.m_iUnitUID = GetCharUID();
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_RETURN_OF_HERO, kBuffReq.m_vecDeActivateBuff );
			if( kBuffReq.m_vecDeActivateBuff.empty() == false )
			{
				SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kBuffReq );
			}
			//}
		}
	}
}
void KGSUser::AnnunciateComeBackUser()
{
	KFriendInfo FriendUser;

	std::map< UidType, KFriendInfo >::iterator mit = m_kMessengerInfo.m_mapFriendInfo.begin();
	for(  ; mit != m_kMessengerInfo.m_mapFriendInfo.end() ; ++mit )
	{
		FriendUser = mit->second;
		if( FriendUser.m_iUnitUID > 0 )
		{
			KDBE_INSERT_REWARD_TO_POST_REQ kReq;
			kReq.m_iFromUnitUID		= FriendUser.m_iUnitUID;
			kReq.m_iToUnitUID		= FriendUser.m_iUnitUID;
			kReq.m_iRewardType		= KPostItemInfo::LT_COME_BACK_USER_NOTIFY;
			kReq.m_wstrMessage		= GetCharName();
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kReq );
		}
	}
}

void KGSUser::SendFirstLoginComeBackUser( IN KUnitInfo& kUnit, IN bool bFirst )
{
	CTime tCurr = CTime::GetCurrentTime();

	KDBE_FIRST_WRITE_COME_BACK_INFO_NOT kNot;

	kNot.m_iUserUID			= GetUID();
	kNot.m_iAge				= static_cast< int >( m_kNexonAccountInfo.m_uiAge );
	kNot.m_iUnitUID			= kUnit.m_nUnitUID;
	kNot.m_iUnitClass		= static_cast< int >( kUnit.m_cUnitClass );
	kNot.m_iUnitLevel		= static_cast< int >( kUnit.m_ucLevel );
	kNot.m_bTodayFirst		= bFirst;
	kNot.m_iRewardLevel		= static_cast< int >( m_kComeBackUserInfo.GetComeBackRewardLevel() );
	kNot.m_wsrtRegDate		= tCurr.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

	SendToLogDB( DBE_FIRST_WRITE_COME_BACK_INFO_NOT, kNot );
}

void KGSUser::SendComeBackLogOut()
{
	CTime tCurr = CTime::GetCurrentTime();

	KDBE_WRITE_COME_BACK_LOGOUT_NOT kNot;
	kNot.m_iUserUID			= GetUID();
	kNot.m_wsrtLogOutDate	= tCurr.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

	SendToAccountDB( DBE_WRITE_COME_BACK_LOGOUT_NOT, kNot );
}
//}}

//{{ 2011. 08. 12   김민성      헤니르 개편
void KGSUser::CheckHenirRewardCountEvent()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kTimer[TM_HENIR_REWARD_EVENT].elapsed() < 120.0 )
			return;

		m_kTimer[TM_HENIR_REWARD_EVENT].restart();

		bool bUnlimited = false;
		int  iEventMax = 0;
		KEGS_HENIR_REWARD_COUNT_NOT	kNot;

		SiKGameEventManager()->GetHenirRewardCountEvent( bUnlimited, iEventMax );

		m_kUserDungeonManager.GetHenirRewardCountInfo( kNot );

		m_kUserDungeonManager.SetUnLimitedHenirRewardEvnet( bUnlimited );
		m_kUserDungeonManager.SetHenirRewardEvnetLimitCount( iEventMax );

		if( kNot.m_bUnLimited != bUnlimited || kNot.m_iEventMAX != iEventMax )
		{
			m_kUserDungeonManager.GetHenirRewardCountInfo( kNot );
			SendPacket( EGS_HENIR_REWARD_COUNT_NOT, kNot );
		}
	}
}
//}}

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{ 2012. 06. 13	김민성       선택적 셧다운제
//{{ 2011. 09. 29  김민성	셧다운제 도입
//}}

//{{ 2012. 02. 14    김민성   넥슨 안심 로그인 - 목표 위치기반 로그인
//}}
//}}

//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
//}}

//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
bool KGSUser::CheckAuthLevel()
{
	if( SiKCompanyIPTable()->CheckIP( GetIPStr() ) == true )
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// iP가 허용 되지 않은 곳에서 치트 사용
	// userid
	// ip
	// sms 전송
	if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
	{
		return false;
	}

	// 서비스에서만 체크
	if( KSimLayer::GetKObj()->GetVersionFlag() != KSimLayer::VF_SERVICE )
	{
		return false;
	}

	KDBE_SEND_PHONE_MSG_NOT kPacketNot;

	//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-8584-3123" ) );		// 권오당 팀장님
	kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-9023-1232" ) );		// 김현철
	kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-8421-1075" ) );		// 최육사
	kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-5598-4163" ) );		// 김민성
	kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-3323-7753" ) );		// 박세훈
	//kPacketNot.m_vecPhoneNum.push_back( std::wstring( L"010-9490-8761" ) );		// 황원준
	//}}
	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	kPacketNot.m_wstrSMSMessage = x2::str( x2::wformat( L"허용안된IP에서 치트사용 : UserUID(%d), IP(%s), ServerIP(%s)" ) % GetUID() % GetIPStr() % KBaseServer::GetKObj()->GetPublicIP() );
	SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
	//////////////////////////////////////////////////////////////////////////

	return false;
}
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
bool KGSUser::GetUnitClassChangeInfo( IN int iItemID, OUT KDBE_BUY_UNIT_CLASS_CHANGE_REQ& kPacketToDB )
{
	int iNewUnitClass = CXSLItem::GetClassChangeCashItem( iItemID ); // 실제 전직하려는 직업
	char cItemUnitClass = static_cast<char>(iNewUnitClass);			// 예외처리로 그룹 아이디 검색 용 ( 2차 -> 1차)

	if( cItemUnitClass == CXSLUnit::UC_NONE )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 스킬 초기화 정보
	if( !SiCXSLSkillTree()->GetUnitClassDefaultSkill( static_cast<char>(iNewUnitClass), kPacketToDB.m_iDefaultSkillID1, kPacketToDB.m_iDefaultSkillID2, kPacketToDB.m_iDefaultSkillID3,
														kPacketToDB.m_iDefaultSkillID4, kPacketToDB.m_iDefaultSkillID5, kPacketToDB.m_iDefaultSkillID6 ) )
	{
		START_LOG( cerr, L"유닛타입이 이상함." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( cItemUnitClass )
			<< END_LOG;

		return false;
	}

	// 배운 스킬이 없다면..
	int iTotalSP = 0;
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), iTotalSP );

	int iRetrievedSPoint = 0;	// 돌려받을 SP
	int iRetrievedCSPoint = 0;	// 돌려받을 CSP

	if( false == m_kSkillTree.IsCashSkillPointExpired() )
	{
		iRetrievedSPoint	= iTotalSP;
		iRetrievedCSPoint	= m_kSkillTree.GetMaxCSPoint();
	}
	else
	{

		const int iCSPoint = m_kSkillTree.GetCSPointAnyPage();
		if ( iCSPoint == 0 )
		{
			iRetrievedSPoint	= iTotalSP;
			iRetrievedCSPoint	= 0;
		}
		else
		{
			START_LOG( cerr, L"cash skill point기한이 다되었는데 스킬트리에 csp가 남아있다." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( iCSPoint )

				<< END_LOG;

			return false;
		}
	}

	kPacketToDB.m_iUnitUID		 = GetCharUID();
	kPacketToDB.m_iSPoint		 = iRetrievedSPoint;
	kPacketToDB.m_iCSPoint		 = iRetrievedCSPoint;

	kPacketToDB.m_iNewUnitClass = static_cast<int>(cItemUnitClass);
	kPacketToDB.m_iUnitUID = GetCharUID();

	kPacketToDB.m_iTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
	// kimhc // 김현철 // 모든 페이지의 SP를 iRetrievedSPoint 로 셋팅 (초기화)
	m_kSkillTree.SetSPointEveryPage( iRetrievedSPoint );
	m_kSkillTree.SetCSPointEveryPage( iRetrievedCSPoint );



	//////////////////////////////////////////////////////////////////////////
	// 스킬
	std::set< int > setUnSealedSkillID;
	setUnSealedSkillID.clear();
	m_kSkillTree.GetUnSealedSkillList( setUnSealedSkillID );
	if ( !setUnSealedSkillID.empty() )		// 봉인 해제한 스킬이 존재한다면 스킬 교환 목록에 추가
	{
		std::set< int >::iterator sit = setUnSealedSkillID.begin();
		if( sit == setUnSealedSkillID.end() )
		{
			START_LOG( cerr, L"전직 변경 아이템 사용 - 봉인 해제한 스킬이 존재한다고 했는데...데이터가 없다!" )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOGc( GetUnitClass() )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
				<<END_LOG;
		}
		else
		{
			int iNewSkill = 0;
			int iOldSkill = *sit;
			for( ; sit != setUnSealedSkillID.end() ; ++sit )
			{
				if( SiKClassChangeTable()->GetClassChange_NewSkill( iNewUnitClass, iOldSkill, iNewSkill ) == true )
				{
					kPacketToDB.m_mapChangeSkill.insert( std::make_pair(iOldSkill, iNewSkill) );
				}
				iNewSkill = 0;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 메모
	std::map< char, int >	mapSkillNote;
	mapSkillNote.clear();
	m_kSkillTree.GetMemoList( mapSkillNote );
	if( !mapSkillNote.empty() )		// 봉인 해제한 스킬이 존재한다면 스킬 교환 목록에 추가
	{
		std::map< char, int >::iterator mit = mapSkillNote.begin();
		if( mit == mapSkillNote.end() )
		{
			START_LOG( cerr, L"전직 변경 아이템 사용 - 장착한 메모가 존재한다고 했는데...데이터가 없다!" )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOGc( GetUnitClass() )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
				<<END_LOG;
		}
		else
		{
			int iNewMemo = 0;
			for( ; mit != mapSkillNote.end() ; ++mit )
			{
				if( SiKClassChangeTable()->GetClassChange_NewMemo( iNewUnitClass, mit->second, iNewMemo ) == true )
				{
					kPacketToDB.m_mapChangeMemo.insert( std::make_pair(mit->second, iNewMemo) );
				}
				iNewMemo = 0;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 아이템
	std::map< UidType, KInventoryItemInfo > mapInventoryItem;
	std::map< UidType, KInventoryItemInfo >::iterator mitInventoryItem;

	// 특수 탭의 아이템 정보만 얻어 오자(캐쉬관련 아이템은 특수 탭에 있다.)
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_SPECIAL, mapInventoryItem, true );
	// 은행의 아이템 정보만 얻어 오자
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_BANK, mapInventoryItem, true );
	// 팻 탭의 아이템 정보만 얻어 오자
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_PET, mapInventoryItem, true );

	mitInventoryItem = mapInventoryItem.begin();
	if( mitInventoryItem == mapInventoryItem.end() )	// 아이템이 존재한다면 교환 가능한 아이템 인지 확인하자
	{
		START_LOG( clog, L"전직 변경 아이템 사용 - Inventory Item 데이터가 없다!" )
			<< BUILD_LOG( GetUserID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( GetUnitClass() )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
			<<END_LOG;
	}
	else
	{
		KInventoryItemInfo kTempItemInfo;
		int	iNewItem = 0;
		for( ; mitInventoryItem != mapInventoryItem.end() ; ++mitInventoryItem )
		{
			kTempItemInfo = mitInventoryItem->second;

			if( SiKClassChangeTable()->GetClassChange_NewItem( iNewUnitClass, kTempItemInfo.m_kItemInfo.m_iItemID, iNewItem ) == false )
			{
				continue;
			}

			kPacketToDB.m_mapChangeItem.insert( std::make_pair( mitInventoryItem->first, iNewItem ) );
			iNewItem = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 퀘스트
	// ( 완료된 )
	// 완료 퀘스트 변경 할 목록 찾자
	{
		std::vector< int > vecCompleteQuest;
		m_kUserQuestManager.GetCompleteChangeClassQuest( vecCompleteQuest );

		std::vector< int >::const_iterator vit = vecCompleteQuest.begin();

		int	iOldQuestID = 0;
		int	iNewQuestID = 0;

		for( ; vit != vecCompleteQuest.end() ; ++vit )
		{
			iOldQuestID = (*vit);
			if( SiKClassChangeTable()->GetClassChange_NewQuest( iNewUnitClass, iOldQuestID, iNewQuestID ) == false )
			{
				continue;
			}

			bool bInsert = true;

			for( u_short usSize = 0 ; usSize < kPacketToDB.m_vecDeleteCompleteQuest.size() ; ++usSize )
			{
				if( iOldQuestID == kPacketToDB.m_vecDeleteCompleteQuest[usSize] )
				{
					bInsert = false;
					break;
				}
			}

			if( bInsert == true )
			{
				kPacketToDB.m_mapChangeCompleteQuest.insert( std::make_pair( iOldQuestID, iNewQuestID ) );
			}

			iOldQuestID = 0;
			iNewQuestID = 0;
		}
	}

	// 퀘스트
	// ( 진행중 )
	{
		std::vector< int > vecInProgressQuest;
		m_kUserQuestManager.GetInProgressClassChangeQuest( vecInProgressQuest );

		std::vector< int >::const_iterator vit = vecInProgressQuest.begin();

		int	iOldQuestID = 0;
		int	iNewQuestID = 0;

		for( ; vit != vecInProgressQuest.end() ; ++vit )
		{
			iOldQuestID = (*vit);
			if( SiKClassChangeTable()->GetClassChange_NewQuest( iNewUnitClass, iOldQuestID, iNewQuestID ) == false )
			{
				continue;
			}

			bool bInsert = true;

			for( u_short usSize = 0 ; usSize < kPacketToDB.m_vecDeleteInProgressQuest.size() ; ++usSize )
			{
				if( iOldQuestID == kPacketToDB.m_vecDeleteInProgressQuest[usSize] )
				{
					bInsert = false;
					break;
				}
			}

			if( bInsert == true )
			{
				kPacketToDB.m_mapChangeInProgressQuest.insert( std::make_pair( iOldQuestID, iNewQuestID ) );
			}

			iOldQuestID = 0;
			iNewQuestID = 0;
		}
	}
	return true;
}
//}}

bool KGSUser::GetUnlimitedClassChangeInfo( IN int iNewClassID, OUT KDBE_BUY_UNIT_CLASS_CHANGE_REQ& kPacketToDB )
{
	char cItemUnitClass = static_cast<char>( iNewClassID );		// 예외처리로 그룹 아이디 검색 용 ( 2차 -> 1차)
	bool bValidSecondChangeJob = CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) );
	CXSLUnit::UNIT_TYPE eOldUnitType = CXSLUnit::GetUnitClassToUnitType( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) );
	CXSLUnit::UNIT_TYPE eNewUnitType = CXSLUnit::GetUnitClassToUnitType( static_cast<CXSLUnit::UNIT_CLASS>( iNewClassID ) );

	kPacketToDB.m_bUnlimitedSecondJobItem = true;						// 무제한 전직변경권 사용 여부. 보상 지급시 확인하기 위한 용도
	kPacketToDB.m_iOldUnitClass = static_cast<int>( GetUnitClass() );

	// 레벨이 35보다 작거나 2차 전직이 아니거나 기존 유닛타입과 변경 후 유닛타입이 다르다면 에러
	if( eNewUnitType == CXSLUnit::UC_NONE
		|| GetLevel() < 35
		|| bValidSecondChangeJob == false
		|| eOldUnitType != eNewUnitType )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 스킬 초기화 정보

	if( !SiCXSLSkillTree()->GetUnitClassDefaultSkill( static_cast<char>(iNewClassID), kPacketToDB.m_iDefaultSkillID1, kPacketToDB.m_iDefaultSkillID2, kPacketToDB.m_iDefaultSkillID3,
														kPacketToDB.m_iDefaultSkillID4, kPacketToDB.m_iDefaultSkillID5, kPacketToDB.m_iDefaultSkillID6 ) )
	{
		START_LOG( cerr, L"유닛타입이 이상함." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( cItemUnitClass )
			<< END_LOG;

		return false;
	}

	// 배운 스킬이 없다면..
	int iUsedSPoint = 0;
	int iUsedCSPoint = 0;
	int iTotalSP = 0;
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), iTotalSP );

	int iRetrievedSPoint = 0;	// 돌려받을 SP
	int iRetrievedCSPoint = 0;	// 돌려받을 CSP

	if( false == m_kSkillTree.IsCashSkillPointExpired() )
	{
		iRetrievedSPoint	= iTotalSP;
		iRetrievedCSPoint	= m_kSkillTree.GetMaxCSPoint();
	}
	else
	{
		const int iCSPoint = m_kSkillTree.GetCSPointAnyPage();
		if ( iCSPoint == 0 )
		{
			iRetrievedSPoint	= iTotalSP;
			iRetrievedCSPoint	= 0;
		}
		else
		{
			START_LOG( cerr, L"cash skill point기한이 다되었는데 스킬트리에 csp가 남아있다." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( iUsedSPoint )
				<< BUILD_LOG( iUsedCSPoint )
				<< BUILD_LOG( iCSPoint )
				<< END_LOG;

			return false;
		}
	}

	kPacketToDB.m_iUnitUID		 = GetCharUID();
	kPacketToDB.m_iSPoint		 = iRetrievedSPoint;
	kPacketToDB.m_iCSPoint		 = iRetrievedCSPoint;

	kPacketToDB.m_iNewUnitClass = static_cast<int>(cItemUnitClass);
	kPacketToDB.m_iUnitUID = GetCharUID();

	kPacketToDB.m_iTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
	// kimhc // 김현철 // 모든 페이지의 SP를 iRetrievedSPoint 로 셋팅 (초기화)
	m_kSkillTree.SetSPointEveryPage( iRetrievedSPoint );
	m_kSkillTree.SetCSPointEveryPage( iRetrievedCSPoint );

	//////////////////////////////////////////////////////////////////////////
	// 스킬
	std::set< int > setUnSealedSkillID;
	setUnSealedSkillID.clear();
	m_kSkillTree.GetUnSealedSkillList( setUnSealedSkillID );
	if( setUnSealedSkillID.size() > 0 )		// 봉인 해제한 스킬이 존재한다면 스킬 교환 목록에 추가
	{
		std::set< int >::iterator sit = setUnSealedSkillID.begin();
		if( sit == setUnSealedSkillID.end() )
		{
			START_LOG( cerr, L"전직 변경 아이템 사용 - 봉인 해제한 스킬이 존재한다고 했는데...데이터가 없다!" )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOGc( GetUnitClass() )
				<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
				<<END_LOG;
		}
		else
		{
			int iNewSkill = 0;
			int iOldSkill = *sit;
			for( ; sit != setUnSealedSkillID.end() ; ++sit )
			{
				if( SiKClassChangeTable()->GetClassChange_NewSkill( iNewClassID, iOldSkill, iNewSkill ) == true )
				{
					kPacketToDB.m_mapChangeSkill.insert( std::make_pair(iOldSkill, iNewSkill) );
				}
				iNewSkill = 0;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 메모
	std::map< char, int >	mapSkillNote;
	mapSkillNote.clear();
	m_kSkillTree.GetMemoList( mapSkillNote );
	if( mapSkillNote.size() > 0 )		// 봉인 해제한 스킬이 존재한다면 스킬 교환 목록에 추가
	{
		std::map< char, int >::iterator mit = mapSkillNote.begin();
		if( mit == mapSkillNote.end() )
		{
			START_LOG( cerr, L"전직 변경 아이템 사용 - 장착한 메모가 존재한다고 했는데...데이터가 없다!" )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOGc( GetUnitClass() )
				<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
				<<END_LOG;
		}
		else
		{
			int iNewMemo = 0;
			for( ; mit != mapSkillNote.end() ; ++mit )
			{
				if( SiKClassChangeTable()->GetClassChange_NewMemo( iNewClassID, mit->second, iNewMemo ) == true )
				{
					kPacketToDB.m_mapChangeMemo.insert( std::make_pair(mit->second, iNewMemo) );
				}
				iNewMemo = 0;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 아이템
	std::map< UidType, KInventoryItemInfo > mapInventoryItem;
	std::map< UidType, KInventoryItemInfo >::iterator mitInventoryItem;

	// 특수 탭의 아이템 정보만 얻어 오자(캐쉬관련 아이템은 특수 탭에 있다.)
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_SPECIAL, mapInventoryItem, true );
	// 은행의 아이템 정보만 얻어 오자
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_BANK, mapInventoryItem, true );
	// 팻 탭의 아이템 정보만 얻어 오자
	m_kInventory.GetInventoryInfo( CXSLInventory::ST_PET, mapInventoryItem, true );

	mitInventoryItem = mapInventoryItem.begin();
	if( mitInventoryItem == mapInventoryItem.end() )	// 아이템이 존재한다면 교환 가능한 아이템 인지 확인하자
	{
		START_LOG( clog, L"전직 변경 아이템 사용 - Inventory Item 데이터가 없다!" )
			<< BUILD_LOG( GetUserID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( GetUnitClass() )
			<< BUILD_LOG( kPacketToDB.m_iNewUnitClass )
			<<END_LOG;
	}
	else
	{
		KInventoryItemInfo kTempItemInfo;
		int	iNewItem = 0;
		for( ; mitInventoryItem != mapInventoryItem.end() ; ++mitInventoryItem )
		{
			kTempItemInfo = mitInventoryItem->second;

			if( SiKClassChangeTable()->GetClassChange_NewItem( iNewClassID, kTempItemInfo.m_kItemInfo.m_iItemID, iNewItem ) == false )
			{
				continue;
			}

			kPacketToDB.m_mapChangeItem.insert( std::make_pair( mitInventoryItem->first, iNewItem ) );
			iNewItem = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 퀘스트
	// ( 완료된 )
	// 완료 퀘스트 변경 할 목록 찾자
	{
		std::vector< int > vecCompleteQuest;
		m_kUserQuestManager.GetCompleteChangeClassQuest( vecCompleteQuest );

		std::vector< int >::const_iterator vit = vecCompleteQuest.begin();

		int	iOldQuestID = 0;
		int	iNewQuestID = 0;

		for( ; vit != vecCompleteQuest.end() ; ++vit )
		{
			iOldQuestID = (*vit);
			if( SiKClassChangeTable()->GetClassChange_NewQuest( iNewClassID, iOldQuestID, iNewQuestID ) == false )
			{
				continue;
			}

			bool bInsert = true;

			for( u_short usSize = 0 ; usSize < kPacketToDB.m_vecDeleteCompleteQuest.size() ; ++usSize )
			{
				if( iOldQuestID == kPacketToDB.m_vecDeleteCompleteQuest[usSize] )
				{
					bInsert = false;
					break;
				}
			}

			if( bInsert == true )
			{
				kPacketToDB.m_mapChangeCompleteQuest.insert( std::make_pair( iOldQuestID, iNewQuestID ) );
			}

			iOldQuestID = 0;
			iNewQuestID = 0;
		}
	}

	// 퀘스트
	// ( 진행중 )
	{
		std::vector< int > vecInProgressQuest;
		m_kUserQuestManager.GetInProgressClassChangeQuest( vecInProgressQuest );

		std::vector< int >::const_iterator vit = vecInProgressQuest.begin();

		int	iOldQuestID = 0;
		int	iNewQuestID = 0;

		for( ; vit != vecInProgressQuest.end() ; ++vit )
		{
			iOldQuestID = (*vit);
			if( SiKClassChangeTable()->GetClassChange_NewQuest( iNewClassID, iOldQuestID, iNewQuestID ) == false )
			{
				continue;
			}

			bool bInsert = true;

			for( u_short usSize = 0 ; usSize < kPacketToDB.m_vecDeleteInProgressQuest.size() ; ++usSize )
			{
				if( iOldQuestID == kPacketToDB.m_vecDeleteInProgressQuest[usSize] )
				{
					bInsert = false;
					break;
				}
			}

			if( bInsert == true )
			{
				kPacketToDB.m_mapChangeInProgressQuest.insert( std::make_pair( iOldQuestID, iNewQuestID ) );
			}

			iOldQuestID = 0;
			iNewQuestID = 0;
		}
	}
	return true;
}

//{{  2011.11.24     김민성    캐릭터 접속 로그
//}}

//{{ 버프 관리 시스템 - 김민성
void KGSUser::CheckBuffPeriod()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		// 간격은 5초로 둡니다. 체크 간격을 넓힐수록 서버 퍼포먼스를 아낄수 있음.
		if( m_kTimer[TM_CHECK_BUFF_PERIOD].elapsed() < 1.0 )
			return;

		m_kTimer[TM_CHECK_BUFF_PERIOD].restart();


		std::vector< int > vecDeactivateBuffList;
		m_kUserBuffManager.CheckBuffPeriod( vecDeactivateBuffList, CXSLRoom::GetRoomType( GetRoomUID() ) );
		if( vecDeactivateBuffList.empty() == false )
		{
			// 로그인 서버 버프 매니저에 업데이트
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_vecDeActivateBuff = vecDeactivateBuffList;
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
	}
}

void KGSUser::SendUpdateBuffInfo()
{
	if( GetRoomUID() > 0 )
	{
		const int iRoomType = CXSLRoom::GetRoomType( GetRoomUID() );
		switch( iRoomType )
		{
		case CXSLRoom::RT_PVP:
			{
				if ( GetRoomListID() != 0)
				{
				KEGS_UPDATE_BUFF_INFO_IN_ROOM_NOT kPacketNot;
				kPacketNot.m_iUnitUID = GetCharUID();
				KRoomUserInfo kRoomUserInfo;
				GetRoomUserInfo( kRoomUserInfo, iRoomType );
				m_kUserBuffManager.GetBuffList( kPacketNot.m_vecActivateBuffList, CXSLRoom::RT_PVP );
				kPacketNot.m_kGameStat = kRoomUserInfo.m_kGameStat;						// 그냥 버프 적용안 된 스텟
				//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
				//}}
				SendToCnRoom( ERM_UPDATE_BUFF_INFO_IN_ROOM_NOT, kPacketNot );
				}
			}
			break;
		case CXSLRoom::RT_DUNGEON:
			{
				KEGS_UPDATE_BUFF_INFO_IN_ROOM_NOT kPacketNot;
				kPacketNot.m_iUnitUID = GetCharUID();
				KRoomUserInfo kRoomUserInfo;
				GetRoomUserInfo( kRoomUserInfo, iRoomType );
				m_kUserBuffManager.GetBuffList( kPacketNot.m_vecActivateBuffList, CXSLRoom::RT_DUNGEON );
				kPacketNot.m_kGameStat = kRoomUserInfo.m_kGameStat;						// 그냥 버프 적용안 된 스텟
				//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
				//}}
				SendToCnRoom( ERM_UPDATE_BUFF_INFO_IN_ROOM_NOT, kPacketNot );
			}
			break;
		case CXSLRoom::RT_BATTLE_FIELD:
			{
				KEGS_UPDATE_BUFF_INFO_IN_ROOM_NOT kPacketNot;
				kPacketNot.m_iUnitUID = GetCharUID();
				KRoomUserInfo kRoomUserInfo;
				GetRoomUserInfo( kRoomUserInfo, iRoomType );
				m_kUserBuffManager.GetBuffList( kPacketNot.m_vecActivateBuffList, CXSLRoom::RT_BATTLE_FIELD );
				kPacketNot.m_kGameStat = kRoomUserInfo.m_kGameStat;						// 그냥 버프 적용안 된 스텟
				//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
				//}}
				SendToCnRoom( ERM_UPDATE_BUFF_INFO_IN_ROOM_NOT, kPacketNot );
			}
			break;

		default:
			{
				KEGS_UPDATE_BUFF_INFO_NOT kPacketNot;
				m_kUserBuffManager.GetBuffList( kPacketNot.m_vecActivateBuffList );
				SendPacket( EGS_UPDATE_BUFF_INFO_NOT, kPacketNot );
			}
			break;
		}
	}
	else
	{
		KEGS_UPDATE_BUFF_INFO_NOT kPacketNot;
		m_kUserBuffManager.GetBuffList( kPacketNot.m_vecActivateBuffList );
		SendPacket( EGS_UPDATE_BUFF_INFO_NOT, kPacketNot );
	}
}

bool KGSUser::ActivateBuff( IN const int iBuffID, OUT std::vector< KBuffInfo >& vecActivatedBuff, IN const CTime tUseTime /* = 0*/, IN int iPeriod /* = 0*/ )
{
	KBuffInfo kBuff;
	kBuff.m_kFactorInfo.m_BuffIdentity.m_eBuffTempletID = iBuffID;

	// 존재하는 버프인지 검사
	const CXSLBuffManager::BuffTemplet* pBuffTemplet = SiCXSLBuffManager()->GetBuffTemplet( iBuffID );
	if( pBuffTemplet == NULL )
	{
		START_LOG( cerr, L"존재 하지 않는 버프입니다!" )
			<< BUILD_LOG( iBuffID )
			<< END_LOG;
		return false;
	}

	kBuff.m_kFactorInfo.m_iFactorID = pBuffTemplet->m_iFactorID;

	const bool bRet = m_kUserBuffManager.ActivateBuff( kBuff, vecActivatedBuff, tUseTime, iPeriod );

	return bRet;
}

bool KGSUser::ActivateItemBuff( IN const int iUseItemID, OUT std::vector< KBuffInfo >& vecActivatedBuff, OUT std::vector<int>& vecDeactiveBuff, IN const CTime tUseTime /*= 0*/ )
{
	if( SiCXSLItemManager()->IsBuffItem( iUseItemID ) == false )
	{
		START_LOG( clog, L"존재하지 아이템(버프)입니다." )
			<< BUILD_LOG( iUseItemID )
			<< END_LOG;
		return false;
	}

	switch( iUseItemID )
	{
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_NUT:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_PTAH:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_RACHEL:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_ZADKIEL:
		{
			int iBuffID = 0;

			if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_NUT )		{ iBuffID = CXSLBuffManager::BTI_BUFF_PROTECTION_OF_NUT; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_PTAH )	{ iBuffID = CXSLBuffManager::BTI_BUFF_PROTECTION_OF_PTAH; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_RACHEL )	{ iBuffID = CXSLBuffManager::BTI_BUFF_PROTECTION_OF_LAHEL; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ARCANUM_ZADKIEL )	{ iBuffID = CXSLBuffManager::BTI_BUFF_PROTECTION_OF_ZACHIEL; }

			// 존재하는 버프인지 검사
			const CXSLBuffManager::BuffTemplet* pBuffTemplet = SiCXSLBuffManager()->GetBuffTemplet( iBuffID );
			if( pBuffTemplet == NULL )
			{
				START_LOG( cerr, L"존재 하지 않는 버프입니다!" )
					<< BUILD_LOG( iBuffID )
					<< END_LOG;
				return false;
			}

			DeactivateBuff( CXSLBuffManager::BTI_BUFF_PROTECTION_OF_NUT, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_PROTECTION_OF_PTAH, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_PROTECTION_OF_LAHEL, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_PROTECTION_OF_ZACHIEL, vecDeactiveBuff );

			// 아이템 사용 버프
			ActivateBuff( iBuffID, vecActivatedBuff, tUseTime );

			// 비약 사용 디버프
			const CXSLBuffManager::BuffTemplet* pDeBuffTemplet = SiCXSLBuffManager()->GetBuffTemplet( CXSLBuffManager::BTI_BUFF_STRENGTHENING_BODY );
			if( pDeBuffTemplet == NULL )
			{
				START_LOG( cerr, L"존재 하지 않는 버프입니다!" )
					<< BUILD_LOG( CXSLBuffManager::BTI_BUFF_STRENGTHENING_BODY )
					<< END_LOG;
				return false;
			}

			ActivateBuff( CXSLBuffManager::BTI_BUFF_STRENGTHENING_BODY, vecActivatedBuff, tUseTime );
		}
		break;
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_SERAPHIM:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_GEB:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_AMON:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_CRONOS:
		//{{ 2013. 04. 12	박세훈	어둠의 문 개편
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_GIANT_POTION:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_BLAZING_BOMB:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_SPIRIT_OF_CHASER:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_CRADLE_OF_LITTLE_FAIRY:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_ICE_BALL_OF_DENIPH:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_FEATHER_OF_VENTUS:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_FLAME_RING_OF_ROSSO:
	case CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_BIG_HAND_POTION:
		//}}
	case CXSLItem::EI_BUFF_RURIEL_MANA_ENERGIZE_POTION:
		{
			int iBuffID = 0;

			if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_SERAPHIM )					{ iBuffID = CXSLBuffManager::BTI_BUFF_BELSSING_OF_SERAPHIM; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_GEB )					{ iBuffID = CXSLBuffManager::BTI_BUFF_BELSSING_OF_GEB; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_AMON )					{ iBuffID = CXSLBuffManager::BTI_BUFF_BELSSING_OF_AMON; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_CRONOS )				{ iBuffID = CXSLBuffManager::BTI_BUFF_BELSSING_OF_CRONOS; }
			//{{ 2013. 04. 12	박세훈	어둠의 문 개편
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_GIANT_POTION )			{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_GIANT_POTION; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_BLAZING_BOMB )			{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_BLAZING_BOMB; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_SPIRIT_OF_CHASER )		{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_SPIRIT_OF_CHASER; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_CRADLE_OF_LITTLE_FAIRY ){ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_CRADLE_OF_LITTLE_FAIRY; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_ICE_BALL_OF_DENIPH )	{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_ICE_BALL_OF_DENIPH; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_FEATHER_OF_VENTUS )		{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_FEATHER_OF_VENTUS; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_FLAME_RING_OF_ROSSO )	{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_FLAME_RING_OF_ROSSO; }
			else if( iUseItemID == CXSLItem::SI_THE_GATE_OF_DARKNESS_ELIXIR_BIG_HAND_POTION )		{ iBuffID = CXSLBuffManager::BTI_BUFF_ELIXIR_BIG_HAND_POTION; }
			//}}
			else if( iUseItemID == CXSLItem::EI_BUFF_RURIEL_MANA_ENERGIZE_POTION )					{ iBuffID = CXSLBuffManager::BTI_BUFF_RURIEL_MANA_ENERGIZE_POTION; }

			// 존재하는 버프인지 검사
			const CXSLBuffManager::BuffTemplet* pBuffTemplet = SiCXSLBuffManager()->GetBuffTemplet( iBuffID );
			if( pBuffTemplet == NULL )
			{
				START_LOG( cerr, L"존재 하지 않는 버프입니다!" )
					<< BUILD_LOG( iBuffID )
					<< END_LOG;
				return false;
			}

			DeactivateBuff( CXSLBuffManager::BTI_BUFF_BELSSING_OF_SERAPHIM, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_BELSSING_OF_GEB, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_BELSSING_OF_AMON, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_BELSSING_OF_CRONOS, vecDeactiveBuff );
			//{{ 2013. 04. 12	박세훈	어둠의 문 개편
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_GIANT_POTION, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_BLAZING_BOMB, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_SPIRIT_OF_CHASER, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_CRADLE_OF_LITTLE_FAIRY, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_ICE_BALL_OF_DENIPH, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_FEATHER_OF_VENTUS, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_FLAME_RING_OF_ROSSO, vecDeactiveBuff );
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_ELIXIR_BIG_HAND_POTION, vecDeactiveBuff );
			//}}
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_RURIEL_MANA_ENERGIZE_POTION, vecDeactiveBuff );

			// 아이템 사용 버프
			ActivateBuff( iBuffID, vecActivatedBuff, tUseTime );

		}
		break;
	default:
		{
			START_LOG( cerr, L"버프 정보가 없는 아이템을 사용 했습니다." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( iUseItemID )
				<< END_LOG;

			return false;
		}
	}

	return true;
}

bool KGSUser::DeactivateBuff( IN const int iBuffID, OUT std::vector< int >& vecDeleteBuff )
{
	// 존재하는 버프인지 검사
	const CXSLBuffManager::BuffTemplet* pBuffTemplet = SiCXSLBuffManager()->GetBuffTemplet( iBuffID );
	if( pBuffTemplet == NULL )
	{
		START_LOG( cerr, L"존재 하지 않는 버프입니다!" )
			<< BUILD_LOG( iBuffID )
			<< END_LOG;
		return false;
	}

	if( m_kUserBuffManager.IsBuffActivated( iBuffID ) == false )
		return false;

	const bool bRet = m_kUserBuffManager.DeactivateBuff( iBuffID, vecDeleteBuff );

	return bRet;
}

void KGSUser::CheckDeselectUnitBuff( IN const bool bSendAck /*= true*/ )
{
	std::vector<int> vecDeleteBuff;

	// 게임 종료 시(캐릭터 선택 해제) 지워야 할 버프 정보를 찾자.

	// 따뜻한 엘의 기운 (마을 버프)
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_WARM_ENERGE_OF_EL, vecDeleteBuff );
	// 영웅의 집결 ( 자동파티버프 )
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_RALLY_OF_HERO_LEVEL_1, vecDeleteBuff );
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_RALLY_OF_HERO_LEVEL_2, vecDeleteBuff );
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_RALLY_OF_HERO_LEVEL_3, vecDeleteBuff );
	// 리벤지 모드 버프
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_PVP_REVENGE_MODE_LEVEL_1, vecDeleteBuff );
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_PVP_REVENGE_MODE_LEVEL_2, vecDeleteBuff );
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_PVP_REVENGE_MODE_LEVEL_3, vecDeleteBuff );
	//PC 방 유저라고 해도 지운다
	DeactivateBuff( CXSLBuffManager::BTI_BUFF_PREMIUM_PC_ROOM, vecDeleteBuff );

	// itexpertkim // 김현철 // 2014-01-22 // 네이버 이벤트 버프 // 네이버 채널링 디파인 둘러야하지 않을까?
	//DeactivateBuff( CXSLBuffManager::BTI_BUFF_NAVER, vecDeleteBuff );

	//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
	//}}


	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		DeactivateBuff( CXSLBuffManager::BTI_BUFF_TRADE_BLOCK, vecDeleteBuff );
	}

	if( m_kComeBackUserInfo.GetIsComeBackUser() == false )
	{
		DeactivateBuff( CXSLBuffManager::BTI_BUFF_RETURN_OF_HERO, vecDeleteBuff );
	}

	if( vecDeleteBuff.empty() == false )
	{
		KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kBuffReq;
		kBuffReq.m_iUnitUID = GetCharUID();
		kBuffReq.m_vecDeActivateBuff = vecDeleteBuff;
		kBuffReq.m_bSendAck = bSendAck;
		SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kBuffReq );
	}
}
//}

//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
//}}

//{{ 2012. 03. 27	김민성		머신 시간을 검사하자(50분 이상 차이나면 모니터링)
//}}

//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
//}}

//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
void KGSUser::SetRandomTimeKick( int iReason, int iMinTime, int iRange )
{
	m_kUserSecurityManager.SetKickRandomTime( iMinTime, iRange );
	m_kUserSecurityManager.SetKickState( KUserSecurityManager::KT_AFTER_RANDOM_TIME );
	m_kUserSecurityManager.SetKickReason( iReason );
}

void KGSUser::CheckRandomTimeKick()
{
	if( m_kUserSecurityManager.CheckIsKickUser() == true )
	{
		SetDisconnectReason( m_kUserSecurityManager.GetKickReason() );
		ReserveDestroy();
	}
}
//}}

//{{ 2012. 05. 29	김민성       개인 상점 거래 로직 변경
void KGSUser::SetPersonalShopBuyItemInfo( IN KERM_BUY_PERSONAL_SHOP_ITEM_REQ& kPacket )
{
	m_kUserPersonalShopManager.SetPersonalShopBuyItemUID( kPacket.m_iBuyItemUID );
	m_kUserPersonalShopManager.SetPersonalShopBuyItemID( kPacket.m_iBuyItemID );
	m_kUserPersonalShopManager.SetPersonalShopBuyQuantity( kPacket.m_iBuyQuantity );
	m_kUserPersonalShopManager.SetPersonalShopBuyUpdate( kPacket.m_kItemQuantityUpdateTemp );
	m_kUserPersonalShopManager.SetPersonalShopBuyItemInfo( kPacket.m_vecItemInfoTemp );
}

void KGSUser::GetPersonalShopBuyItemInfo( OUT KERM_BUY_PERSONAL_SHOP_ITEM_REQ& kPacket )
{
	kPacket.m_iBuyItemUID = m_kUserPersonalShopManager.GetPersonalShopBuyItemUID();
	kPacket.m_iBuyItemID = m_kUserPersonalShopManager.GetPersonalShopBuyItemID();
	kPacket.m_iBuyQuantity = m_kUserPersonalShopManager.GetPersonalShopBuyQuantity();
	m_kUserPersonalShopManager.GetPersonalShopBuyUpdate( kPacket.m_kItemQuantityUpdateTemp );
	m_kUserPersonalShopManager.GetPersonalShopBuyQuantity( kPacket.m_vecItemInfoTemp );
	kPacket.m_iBuyUnitNowED = GetED();
}
//}}

//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
void KGSUser::SetAgencyShopPickUpItemInfo( IN std::vector< UidType >& vecPickUpItemList )
{
	m_kUserPersonalShopManager.SetAgencyShopPickUpItemInfo( vecPickUpItemList );
}

void KGSUser::GetAgencyShopPickUpItemInfo( OUT std::vector< UidType >& vecPickUpItemList )
{
	m_kUserPersonalShopManager.GetAgencyShopPickUpItemInfo( vecPickUpItemList );
}
//}}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
void KGSUser::SetHackingUserSendType( IN char cType )
{
	m_kUserHackingManager.SetHackingUserSendType( cType );
}
//}}

//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
void KGSUser::CheckAutoPartyWaitUserCountRequest()
{
	// 애초에 자동 파티 대기열에 등록된 상태가 아니라면 체크하지 않는다.
	if( GetAutoPartyWaitNumber() == 0 )
		return;

	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kTimer[TM_AUTO_PARTY_WAIT_USER_COUNT].elapsed() < 10.0 )
			return;

		m_kTimer[TM_AUTO_PARTY_WAIT_USER_COUNT].restart();

		// 현재 등록된 대기열의 대기 인원을 요청한다!
		KEGB_REQUEST_AUTO_PARTY_WAIT_USER_COUNT_NOT kPacket;
		kPacket.m_iAutoPartyWaitNumber = GetAutoPartyWaitNumber();
		SendToAutoParty( EGB_REQUEST_AUTO_PARTY_WAIT_USER_COUNT_NOT, kPacket );
	}
}
//}}

//{{ 2012. 02. 22	박세훈	길드 이름 변경권
bool KGSUser::IsGuildMaster( void )
{
	return m_kUserGuildManager.IsGuildMaster();
}
//}}


//{{ 2012. 06. 13	김민성       선택적 셧다운제
//}}

//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
void KGSUser::AddTempInventoryItem( IN const int iItemID
								  , IN const u_char ucSealData
								  //{{ 2012. 1. 16	박세훈	던전 및 필드 드랍 아이템에 강화 부여
								  , IN const KItemInfo& kItemInfo
								  //}}
								  )
{
	// 임시 인벤토리로 저장하러 가기!
	KEGS_GET_ITEM_TEMP_INVENTORY_NOT kNot;
	if( m_kTempInventory.AddTempInventoryItem( iItemID
											 , ucSealData
											 //{{ 2012. 1. 16	박세훈	던전 및 필드 드랍 아이템에 강화 부여
											 , kItemInfo
											 //}}
											 , kNot.m_kTempInvenItem
											 ) )
	{
		SendPacket( EGS_GET_ITEM_TEMP_INVENTORY_NOT, kNot );
	}
	else
	{
		START_LOG( cerr, L"임시 인벤토리에 아이템 저장 실패!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
	}
}

void KGSUser::SendQuestRewardLetter( IN const int iQuestID, IN const int iRewardItemID, IN const int iQuantity )
{
	CStringW cwstrQuestID;
	cwstrQuestID.Format( L"%d", iQuestID );

	KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
	kPacketToDB.m_iFromUnitUID = GetCharUID();
	kPacketToDB.m_iToUnitUID   = GetCharUID();
	kPacketToDB.m_iRewardType  = KPostItemInfo::LT_QUEST_REWARD;
	kPacketToDB.m_iRewardID	   = iRewardItemID;
	kPacketToDB.m_sQuantity	   = static_cast<short>(iQuantity);
	kPacketToDB.m_wstrMessage  = cwstrQuestID.GetBuffer();
	kPacketToDB.m_bGameServerEvent = true;
	SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
}
//}}

//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
//}}

//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
CXSLUnit::PVP_RANK KGSUser::GetPvpRankForClient( void ) const
{
	return ( m_kUserPvpManager.GetOfficialMatchCount() < 10 ) ? CXSLUnit::PVPRANK_RANK_ARRANGE : m_kUserPvpManager.GetPvpRank();
}

void KGSUser::GetPvpRankForClient( IN const int iOfficialMatchCnt, IN OUT char& m_cRank ) const
{
	if( iOfficialMatchCnt < 10 )
	{
		m_cRank = static_cast<char>( CXSLUnit::PVPRANK_RANK_ARRANGE );
	}
}
//}}

//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//}}

//{{ 2012. 09. 25	박세훈	빙고 이벤트
//}}

void KGSUser::CheckMaxLevel()
{
	int iCurrentEventTickCount = SiKGameEventManager()->GetAppliedEventTick();

	if( iCurrentEventTickCount != m_iMaxLevelTickCount )
	{
		//레벨 변경사항을 보낸다.
		KEGS_UPDATE_MAX_LEVEL_NOT kPacket;
		kPacket.m_iMaxLevel = std::max( SiKGameEventManager()->GetMaxLevel(), SiKGameSysVal()->GetLimitsLevel() );

		SendPacket( EGS_UPDATE_MAX_LEVEL_NOT, kPacket );

		m_iMaxLevelTickCount = iCurrentEventTickCount;
	}
}
void KGSUser::CheckEnableEventDungeon()
{
	int iCurrentEventTickCount = SiKGameEventManager()->GetAppliedEventTick();

	if( iCurrentEventTickCount != m_iActiveDungeonTickCount )
	{
		//던전 변경사항을 보낸다.
		KEGS_UPDATE_DUNGEON_STATUS_NOT kPacket;
		SiCXSLDungeonManager()->GetEventDungeons( kPacket.m_mapEnableDungeon );
		SiKGameEventManager()->CheckEnableEventDungeon( kPacket.m_mapEnableDungeon );

		SendPacket( EGS_UPDATE_DUNGEON_STATUS_NOT, kPacket );

		m_iActiveDungeonTickCount = iCurrentEventTickCount;
	}
}

void KGSUser::CheckEnableCodeEvent()
{
	int iCurrentEventTickCount = SiKGameEventManager()->GetAppliedEventTick();

	if( iCurrentEventTickCount != m_iCodeEventTickCount )
	{
		// 코드 변경사항을 보낸다.
		KEGS_UPDATE_CODE_EVENT_NOT kPacket;
		SiKGameEventManager()->CheckEnableCode( kPacket.m_mapEnableCodeEnum );

		SendPacket( EGS_UPDATE_CODE_EVENT_NOT, kPacket );

		m_iCodeEventTickCount = iCurrentEventTickCount;
	}
}

//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
void KGSUser::SendSelectUnitACK( IN const KDBE_SELECT_UNIT_ACK& kPacket )
{
	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_1_NOT kNot_1;
	kNot_1.m_iOK							= kPacket.m_iOK;
	kNot_1.m_kUnitInfo						= kPacket.m_kUnitInfo;
	kNot_1.m_mapInventorySlotSize			= kPacket.m_mapInventorySlotSize;
	kNot_1.m_mapItem						= kPacket.m_mapItem;
	kNot_1.m_iNumResurrectionStone			= kPacket.m_iNumResurrectionStone;

	//{{ 2009. 10. 14  최육사	리필부활석
	kNot_1.m_iNumAutoPaymentResStone		= kPacket.m_iNumAutoPaymentResStone;
	kNot_1.m_wstrAutoPayResStoneLastDate	= kPacket.m_wstrAutoPayResStoneLastDate;
	//}}

	//{{ 2011. 09. 23	최육사	로그오프 상태 ED체크
	kNot_1.m_iRealDataED					= kPacket.m_iRealDataED;
	//}}

	//{{ 2011. 01. 18	최육사	캐릭터 카운트 정보
	kNot_1.m_wstrUnitCreateDate				= kPacket.m_wstrUnitCreateDate;
	//}}

	kNot_1.m_iNewUnitTradeBlockUnitClass	= SiKGameSysVal()->GetNewUnitTradeBlockUnitClass();
	kNot_1.m_bIsNewUnitTradeBlock			= SiKGameSysVal()->IsNewUnitTradeBlock();
	kNot_1.m_iNewUnitTradeBlockDay			= SiKGameSysVal()->GetNewUnitTradeBlockDay();

	//{{ 2011. 03. 22	최육사	캐릭터 첫 접속 로그
	kNot_1.m_wstrUnitLastLoginDate			= kPacket.m_wstrUnitLastLoginDate;
	//}}

	kNot_1.m_vecUserSkillPageData			= kPacket.m_vecUserSkillPageData;		// 획득한 스킬 리스트
	kNot_1.m_vecSkillUnsealed				= kPacket.m_vecSkillUnsealed;		// 봉인해제된 스킬 리스트

	//{{ 2010. 03. 13  최육사	기술의 노트
	kNot_1.m_cSkillNoteMaxPageNum			= kPacket.m_cSkillNoteMaxPageNum;
	kNot_1.m_mapSkillNote					= kPacket.m_mapSkillNote;			// [key:SlotID, value:MemoID]
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kNot_1.m_kRelationshipInfo				= kPacket.m_kRelationshipInfo;

	for( int iSkillSlotID = 0 ; iSkillSlotID < EQUIPPED_SKILL_SLOT_COUNT ; ++iSkillSlotID )
	{
		if( kNot_1.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[iSkillSlotID].m_iSkillID == CXSLSkillTree::SI_ETC_WS_COMMON_LOVE )
		{
			kNot_1.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill[iSkillSlotID].m_cSkillLevel = kPacket.m_kRelationshipInfo.m_cRelationshipType;
		}
	}
	for( int iSkillSlotID = 0 ; iSkillSlotID < EQUIPPED_SKILL_SLOT_COUNT ; ++iSkillSlotID )
	{
		if( kNot_1.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[iSkillSlotID].m_iSkillID == CXSLSkillTree::SI_ETC_WS_COMMON_LOVE )
		{
			kNot_1.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB[iSkillSlotID].m_cSkillLevel = kPacket.m_kRelationshipInfo.m_cRelationshipType;
		}
	}
	//}

	SendPacket( EGS_SELECT_UNIT_1_NOT, kNot_1 );

	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_2_NOT kNot_2;
	kNot_2.m_iOK					= kPacket.m_iOK;
	kNot_2.m_vecQuest				= kPacket.m_vecQuest;
	kNot_2.m_vecCompletQuest		= kPacket.m_vecCompletQuest;

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	kNot_2.m_mapRandomQuestList		= kPacket.m_mapRandomQuestList;
	//}}

	SendPacket( EGS_SELECT_UNIT_2_NOT, kNot_2 );

	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_3_NOT kNot_3;
	kNot_3.m_iOK							= kPacket.m_iOK;
	//{{ 2008. 10. 6  최육사	타이틀
	kNot_3.m_vecMission						= kPacket.m_vecMission;
	kNot_3.m_vecTitle						= kPacket.m_vecTitle;

	//{{ 2010. 7. 21  최육사	펫 시스템
	kNot_3.m_iSummonedPetUID				= kPacket.m_iSummonedPetUID;
	kNot_3.m_vecPetList						= kPacket.m_vecPetList;
	kNot_3.m_vecCanEvolutionPetList			= kPacket.m_vecCanEvolutionPetList;
	kNot_3.m_vecNeverSummonPetList			= kPacket.m_vecNeverSummonPetList;
	//}}
	//{{ 2010. 8. 2	최육사	펫 시스템
	kNot_3.m_mapPetItem						= kPacket.m_mapPetItem; // 서버 전용
	//}}

	SendPacket( EGS_SELECT_UNIT_3_NOT, kNot_3 );

	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_4_NOT kNot_4;
	kNot_4.m_iOK							= kPacket.m_iOK;
	kNot_4.m_vecChatBlackList				= kPacket.m_vecChatBlackList;
	kNot_4.m_kDenyOptions					= kPacket.m_kDenyOptions;

	//{{ 2008. 3. 31  최육사  추천인
	kNot_4.m_bIsRecommend					= kPacket.m_bIsRecommend;
	kNot_4.m_iRecommendUnitUID				= kPacket.m_iRecommendUnitUID;
	//080405. hoons. 사제리스트 데이터
	kNot_4.m_vecTutorialDBUnitInfo			= kPacket.m_vecTutorialDBUnitInfo;
	//{{ 2008. 5. 19  최육사  접속 시간 이벤트
	kNot_4.m_vecConnectTimeEvent			= kPacket.m_vecConnectTimeEvent;
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	kNot_4.m_vecCumulativeTimeEvent			= kPacket.m_vecCumulativeTimeEvent;
	//}}
	//{{ 2009. 7. 7  최육사		랭킹개편
	kNot_4.m_vecHenirRanking				= kPacket.m_vecHenirRanking;

	kNot_4.m_kMessengerInfo					= kPacket.m_kMessengerInfo;
	//{{ 2012. 02. 21	최육사	배틀필드 시스템
	kNot_4.m_kGamePlayStatus				= kPacket.m_kGamePlayStatus;
	//}}


	SendPacket( EGS_SELECT_UNIT_4_NOT, kNot_4 );

	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_5_NOT kNot_5;
	kNot_5.m_iOK							= kPacket.m_iOK;
	//{{ 2008. 5. 7  최육사  통계
	kNot_5.m_bIsSpiritUpdated				= kPacket.m_bIsSpiritUpdated;
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	kNot_5.m_mapCharGameCount				= kPacket.m_mapCharGameCount;
	//}}
	//{{ 2011. 04. 18	최육사	대리 상인
	kNot_5.m_kPShopAgencyInfo				= kPacket.m_kPShopAgencyInfo;
	//}}
	//{{ 2011. 08. 12   김민성      헤니르 개편
	kNot_5.m_PacketHenirRewardCount			= kPacket.m_PacketHenirRewardCount;
	//}}
	//{{ 2011. 10. 13	최육사	컨텐츠 관리자
	kNot_5.m_bCashShopOpen					= kPacket.m_bCashShopOpen;
	//}}
	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
	//}}
	//{{ 2012. 10. 10	박세훈	빙고 이벤트
	//}}
	//{{ 2012. 12. 20	최육사	아라 첫 선택 튜토리얼
	kNot_5.m_bFirstSelect					= kPacket.m_bFirstSelect;
	//}}


	kNot_5.m_bCouple = kPacket.m_bCouple;
	kNot_5.m_iRelationTargetUserUid = kPacket.m_iRelationTargetUserUid;
	kNot_5.m_wstrRelationTargetUserNickname = kPacket.m_wstrRelationTargetUserNickname;



	SendPacket( EGS_SELECT_UNIT_5_NOT, kNot_5 );

	//////////////////////////////////////////////////////////////////////////
	KEGS_SELECT_UNIT_ACK kACK;
	kACK.m_iOK = kPacket.m_iOK;
	SendPacket( EGS_SELECT_UNIT_ACK, kACK );
}
//}}

//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
void KGSUser::SendUpdateAccountQuestInfo()
{
	// 계정 퀘스트 정보 update
	// 진행 중인 정보만 업데이트 하면 된다

	KDBE_ACCOUNT_QUEST_UPDATE_NOT kNot;
	kNot.m_iUserUID = GetUID();
	m_kUserAccountQuestManager.GetAccountQuesting( kNot.m_vecAccountQuesting );

	if( kNot.m_vecAccountQuesting.empty() == true )
		return;



	SendToAccountDB( DBE_ACCOUNT_QUEST_UPDATE_NOT, kNot );
}
//}}

//{{ 2013. 3. 11	박세훈	 로컬 랭킹 시스템
//}}

//---------------------------------------------------------------------------------------------------//

void KGSUser::CheckTradeBlockUnit()
{
	if( GetStateID() > KGSFSM::S_SERVER_SELECT  &&  GetStateID() < KGSFSM::STATE_SENTINEL )
	{
		if( m_kUserAbuserManager.IsTradeBlockUnit() == false )
			return;

		CTime tCurTime = CTime::GetCurrentTime();
		if( SiKGameSysVal()->GetNewUnitTradeBlockDay() != 0 && tCurTime < m_tReleaseTradeBlockTime )
			return;

		if( SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() != 0 && GetUnitClass() < SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() )
			return;

		CStringW cwstrMessage;
		cwstrMessage.Format( L"%s;", GetCharName().c_str() );

		KEGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT kPacket;
		kPacket.m_wstrNewCharTBMsg = static_cast<LPCTSTR>(cwstrMessage);
		SendPacket( EGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT, kPacket );

		m_kUserAbuserManager.SetTradeBlockUnit( false );

		std::vector< int > vecDeleteBuff;
		DeactivateBuff( CXSLBuffManager::BTI_BUFF_TRADE_BLOCK, vecDeleteBuff );
		if( vecDeleteBuff.empty() == false )
		{
			// 로그인 서버 버프 매니저에 업데이트
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_vecDeActivateBuff = vecDeleteBuff;
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
	}
}

//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
void KGSUser::CheckUpdateGlobalMission()
{
	//	틱카운트가 변하거나 60초마다 한번씩 업데이트
	if( SiKGSGlobalMissionManager()->GetTickCount() == m_iGlobalMissionTickCount )
		return;

	m_iGlobalMissionTickCount = SiKGSGlobalMissionManager()->GetTickCount();

	//	정보받기
	KEGS_GLOBAL_MISSION_UPDATE_NOT kNot;
	SiKGSGlobalMissionManager()->GetGlobalMissionInfo( kNot );

	//	유저에게 패킷날리기
	SendPacket( EGS_GLOBAL_MISSION_UPDATE_NOT, kNot );
}
//}} 2012. 09. 06	임홍락	글로벌 미션 매니저

int KGSUser::GetiRidingPetSummoned()
{
	// m_iRidingPetSummon 값
	// 0 : 라이딩펫 미소환, 1 : 캐시용 라이딩 펫, 2 : 이벤트용 라이딩 펫

	m_iRidingPetSummon = 0;

	// 라이딩 펫 매니저가 존재하면
	KUserRidingPetPtr spRidingPet = m_kUserRidingPetManager.GetRidingPetSummoned();
	if( spRidingPet == NULL )
	{
		return m_iRidingPetSummon;
	}

	// 라이딩 펫 파괴날짜로 판단하자
	KRidingPetInfo kInfoRP;
	spRidingPet->GetData(kInfoRP);


	// 캐시용 펫이라면.
	if(kInfoRP.m_wstrDestroyDate.compare(L"") == 0 )
	{
		m_iRidingPetSummon = 1;
	}
	else
	{
		m_iRidingPetSummon = 2;
	}


	//START_LOG( clog2, L"김석근_라이딩펫 나 타고있음!" )
	//	<<BUILD_LOG( m_iRidingPetSummon )
	//	<<BUILD_LOG( kInfoRP.m_wstrDestroyDate.c_str() )
	//	<<END_LOG;

	return m_iRidingPetSummon;
}

int KGSUser::GetiRidingSummonedPetID()
{
	m_usRidingSummonedPetID = 0;
	int iRidingSummonedPetID = 0;
	// 라이딩 펫 매니저가 존재하면
	KUserRidingPetPtr spRidingPet = m_kUserRidingPetManager.GetRidingPetSummoned();
	if( spRidingPet == NULL )
	{
		return iRidingSummonedPetID;
	}

	// 라이딩 펫 파괴날짜로 판단하자
	KRidingPetInfo kInfoRP;
	spRidingPet->GetData(kInfoRP);


	m_usRidingSummonedPetID = kInfoRP.m_usRindingPetID;
	iRidingSummonedPetID = static_cast<CXSLRidingPetManager::RIDING_PET_UNIT_ID>( m_usRidingSummonedPetID );

	//START_LOG( clog2, L"김석근_라이딩펫 나 타고있음!" )
	//	<<BUILD_LOG( iRidingSummonedPetID )
	//	<<END_LOG;

	return iRidingSummonedPetID;
}



//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
bool KGSUser::LastDateSort(const KUnitInfo& refLUnitInfo, const KUnitInfo& refRUnitInfo)
{
	CTime tLTime;
	CTime tRTime;

	// 컨버팅 안되는 것을 최신으로 봄
	if( KncUtil::ConvertStringToCTime(refLUnitInfo.m_wstrLastDate, tLTime) == false )
	{
		return true;
	}
	else if( KncUtil::ConvertStringToCTime(refRUnitInfo.m_wstrLastDate, tRTime) == false )
	{
		// 오른쪽이 최신이면 return false
		return false;
	}

	// Left가 최신이면 True
	if( tLTime > tRTime )
	{
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------------//

//{{ 2013. 03. 18	 만우절 이벤트_레인보우 버프 - 김민성
//}


void KGSUser::SetRoomListID( IN const UINT uiRoomListID )
{
	if( m_bHeroPvpUser == true )
	{
		const UINT uiTournamentPVPID = SiKRoomListManager()->GetTournamentPVPID();
		if( GetRoomListID() == uiTournamentPVPID )
		{
			if( uiRoomListID != uiTournamentPVPID )
			{
				// 대회 채널 접속 종료
				KEGB_ADMIN_HERO_PVP_USER_TOURNAMENT_CONNECTION_NOT kPacket;
				kPacket.m_iServerGroupID	= KBaseServer::GetKObj()->GetServerGroupID();
				kPacket.m_wstrNickName		= GetCharName();
				kPacket.m_bConnect			= false;
				SendToGlobalServer( EGB_ADMIN_HERO_PVP_USER_TOURNAMENT_CONNECTION_NOT, kPacket );
			}
		}
		else
		{
			if( uiRoomListID == uiTournamentPVPID )
			{
				// 대회 채널 접속
				KEGB_ADMIN_HERO_PVP_USER_TOURNAMENT_CONNECTION_NOT kPacket;
				kPacket.m_iServerGroupID	= KBaseServer::GetKObj()->GetServerGroupID();
				kPacket.m_wstrNickName		= GetCharName();
				kPacket.m_bConnect			= true;
				SendToGlobalServer( EGB_ADMIN_HERO_PVP_USER_TOURNAMENT_CONNECTION_NOT, kPacket );
			}
		}
	}

	m_uiRoomListID = uiRoomListID;
}



void KGSUser::CheckRidingPetProcess( void )
{
	// 만료된 펫 리스트 얻기
	std::vector<UidType> vecRidingPetUID;
	if( m_kUserRidingPetManager.GetExpirationPetUID( vecRidingPetUID ) != KUserRidingPetManager::RIDING_PET_SUCCEED )
	{
		return;
	}

	if( vecRidingPetUID.empty() == true )
	{
		return;
	}

	KDBE_RELEASE_RIDING_PET_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();

	for( const UidType iRidingPetUID : vecRidingPetUID )
	{
		// 만료된 펫 제거하러 DB로 가자!!
		kPacketToDB.m_iRidingPetUID = iRidingPetUID;
		SendToGameDB( DBE_RELEASE_RIDING_PET_REQ, kPacketToDB );
	}
}


//{{ 2013. 04. 01	 인연 시스템 - 김민성
void KGSUser::CheckRelationshipReward()
{
	// 1분마다 체크
	if( m_kTimer[TM_RELATIONSHIP_REWARD].elapsed() < 60.0 )
		return;

	m_kTimer[TM_RELATIONSHIP_REWARD].restart();

	KDBE_INSERT_WEDDING_REWARD_REQ kPacket;
	kPacket.m_iUnitUID = GetCharUID();
	kPacket.m_iLoverUnitUID = m_kUserRelationshipManager.GetLoverUnitUID();

	// 기념일 보상
	if( m_kUserRelationshipManager.IsRewardDay() == true )
	{
		kPacket.m_bTitleReward = false;

		// 보상 지급 하러 가자
		// 커플이라면
		if( m_kUserRelationshipManager.GetRelationshipType() == SEnum::RT_COUPLE )
		{
			// 커플 기념일 큐브
			kPacket.kPostReq.m_iFromUnitUID		= GetCharUID();
			kPacket.kPostReq.m_iToUnitUID		= GetCharUID();
			kPacket.kPostReq.m_iRewardType		= KPostItemInfo::LT_WEDDING_REWARD;
			kPacket.kPostReq.m_iRewardID		= CXSLItem::SI_COUPLE_ANNIVERSARY_DAY_REWARD_ITEM;
			SendToGameDB( DBE_INSERT_WEDDING_REWARD_REQ, kPacket );

			// 보상 지급 한 정보 기록 하자
//			SendToLogDB();
		}
		else if( m_kUserRelationshipManager.GetRelationshipType() == SEnum::RT_MARRIED )
		{
			// 결혼 기념일 큐브
			kPacket.kPostReq.m_iFromUnitUID		= GetCharUID();
			kPacket.kPostReq.m_iToUnitUID		= GetCharUID();
			kPacket.kPostReq.m_iRewardType		= KPostItemInfo::LT_WEDDING_REWARD;
			kPacket.kPostReq.m_iRewardID		= CXSLItem::SI_WEDDING_ANNIVERSARY_DAY_REWARD_ITEM;
			SendToGameDB( DBE_INSERT_WEDDING_REWARD_REQ, kPacket );

			// 보상 지급 한 정보 기록 하자
//			SendToLogDB();
		}
	}

	// 칭호 보상
	int iTitleItemID = m_kUserRelationshipManager.IsRewardTitleItem( kPacket.m_iTitleRewardStep );
	if( iTitleItemID > 0 )
	{
		kPacket.m_bTitleReward = true;

		// 타이틀 지급 하자
		kPacket.kPostReq.m_iFromUnitUID		= GetCharUID();
		kPacket.kPostReq.m_iToUnitUID		= GetCharUID();
		kPacket.kPostReq.m_iRewardType		= KPostItemInfo::LT_WEDDING_REWARD;
		kPacket.kPostReq.m_iRewardID		= iTitleItemID;
		SendToGameDB( DBE_INSERT_WEDDING_REWARD_REQ, kPacket );

		// 타이틀 보상 지급 한 정보 기록 하자
//		SendToLogDB();
	}
}
//}


bool KGSUser::isCanUseUnsealCashSkillItem( IN int iItemID )
{
	int iUnitClassType = CXSLUnit::UCT_NONE;
	if( SiCXSLSkillTree()->GetCashSkillItemUnitClassType( iItemID, iUnitClassType ) == false )
	{
		return false;
	}

	switch( iUnitClassType )
	{
	case CXSLUnit::UCT_BASIC_CLASS:		// 기본, 1차, 2차 전직이면 배울 수 있다
		{
			if( CXSLUnit::IsInitNormalJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) )
				|| CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) )
				|| CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) )
			{
				return true;
			}
		}break;
	case CXSLUnit::UCT_FIRST_CLASS:		// 1차, 2차 전직이면 배울 수 있다
		{
			if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) )
				|| CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) )
			{
				return true;
			}
		}break;
	case CXSLUnit::UCT_SECOND_CLASS:	// 2차 전직이면 배울 수 있다
		{
			if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) )
			{
				return true;
			}
		}break;
	default:
		{
			START_LOG( cerr, L"unit class type 의 값이 비정상적입니다." )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( iUnitClassType )
				<< END_LOG;
		}break;
	}

	return false;
}


bool KGSUser::isCanUseUnsealFinalitySkill( IN int iItemID, IN u_char ucLevel )
{
	if( iItemID == CXSLItem::SI_FINALITY_SKILL_ITEM )
	{
		if( ucLevel < 60 )
		{
			return false;
		}
	}

	return true;
}



void KGSUser::CheckExpirePet()
{
	// 만료된 펫 리스트 얻기
	std::vector<UidType> vecPetUID;
	if( m_kUserPetManager.GetExpirePetUID( vecPetUID ) == false )
	{
		return;
	}

	if( vecPetUID.empty() == true )
	{
		return;
	}

	KDBE_RELEASE_PET_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();

	for( const UidType iPetUID : vecPetUID )
	{
		// 만료된 펫 제거하러 DB로 가자!!
		kPacketToDB.m_iPetUID = iPetUID;
		SendToGameDB( DBE_RELEASE_PET_REQ, kPacketToDB );
	}
}

bool KGSUser::CheckLimitedPlayTimes( IN const int iDungeonID )
{
	int iUnitPlayTimes = 0;
	int iUnitClearTimes = 0;

	KUnitInfo kMyUnitinfo;
	GetUnitInfo( kMyUnitinfo );

	std::map< int, KDungeonPlayInfo >::iterator mitDungeonPlay = kMyUnitinfo.m_mapDungeonPlay.find( iDungeonID );
	if( mitDungeonPlay != kMyUnitinfo.m_mapDungeonPlay.end() )
	{
		iUnitPlayTimes = mitDungeonPlay->second.m_iPlayTimes;
		iUnitClearTimes = mitDungeonPlay->second.m_iClearTimes;
	}

	int iLimitedPlayTimes = 0;
	int iLimitedClearTimes = 0;

	// 플레이 횟수 제한 던전이며 유저의 플레이 횟수가 제한에 도달했으면 체크 실패
	if( SiCXSLDungeonManager()->GetLimitedPlayTimes( iDungeonID, iLimitedPlayTimes ) == true &&
		iLimitedPlayTimes <= iUnitPlayTimes )
	{
		return false;
	}

	// 클리어 횟수 제한 던전이며 유저의 클리어 횟수가 제한에 도달했으면 체크 실패
	if( SiCXSLDungeonManager()->GetLimitedClearTimes( iDungeonID, iLimitedClearTimes ) == true &&
		iLimitedClearTimes <= iUnitClearTimes )
	{
		return false;
	}

	// 둘다 무사히 넘어왔으면 체크 성공
	return true;
}


void KGSUser::GetBossFieldCreateInfo( IN const int iBattleFieldID, OUT KBossFieldCreateInfo& kBossFieldCreateInfo ) const
{
	kBossFieldCreateInfo.clear();
	kBossFieldCreateInfo.m_bBossField = SiCXSLBattleFieldManager()->IsBossFieldID( iBattleFieldID );
	if( kBossFieldCreateInfo.m_bBossField == false )
		return;

	kBossFieldCreateInfo.m_tFieldHoldingTime = SiCXSLFieldBossData()->GetFieldBossFieldHoldingTime_M() * 60;

	// 로그 데이터 생성
	kBossFieldCreateInfo.m_iBossFieldID		= iBattleFieldID;
	kBossFieldCreateInfo.m_iPortalMapID		= SiKGSFieldBossManager()->GetPortalAppearanceMap();
	kBossFieldCreateInfo.m_tPortalOpenTime	= SiKGSFieldBossManager()->GetPortalAppearanceTime();
}

void KGSUser::CreateBossFieldJoinInfo( OUT KBossFieldJoinInfo& kBossFieldJoinInfo ) const
{
	kBossFieldJoinInfo.clear();

	const KLastPositionInfo kLastPositionInfo = GetLastPosInfo();
	if( SiCXSLBattleFieldManager()->IsBossFieldID( kLastPositionInfo.m_iMapID ) == true )
	{
		// 보스 필드로 돌아가지는 못하기 때문에, 강제로 마을로 귀환 시킨다.
		kBossFieldJoinInfo.m_iReturnMapID		= SiCXSLMapData()->GetPossibleEnterVillageMapID( GetLevel(), m_mapDungeonClear );
		kBossFieldJoinInfo.m_bIgnoreLastTouch	= true;
	}
	else
	{
		kBossFieldJoinInfo.m_iReturnMapID			= kLastPositionInfo.m_iMapID;
		kBossFieldJoinInfo.m_ucLastTouchLineIndex	= kLastPositionInfo.m_ucLastTouchLineIndex;
		kBossFieldJoinInfo.m_usLastPosValue			= kLastPositionInfo.m_usLastPosValue;
		kBossFieldJoinInfo.m_bIgnoreLastTouch		= false;
	}
}

void KGSUser::GetBossFieldJoinInfo( OUT KBossFieldJoinInfo& kBossFieldJoinInfo ) const
{
	kBossFieldJoinInfo.clear();

	if( SiCXSLBattleFieldManager()->IsBossFieldID( m_kBossFieldJoinInfo.m_iReturnMapID ) == true )
	{
		// 보스 필드로 돌아가지는 못하기 때문에, 강제로 마을로 귀환 시킨다.
		kBossFieldJoinInfo.m_iReturnMapID		= SiCXSLMapData()->GetPossibleEnterVillageMapID( GetLevel(), m_mapDungeonClear );
		kBossFieldJoinInfo.m_bIgnoreLastTouch	= true;
	}
	else
	{
		kBossFieldJoinInfo = m_kBossFieldJoinInfo;
	}
}


void KGSUser::AddLogWhenSkillPagesNumberIsWrong( const WCHAR* wszLog_ )
{
	std::wstring wstrLog = L"사용하려는 스킬페이지의 번호가 이상합니다. ";
	wstrLog += wszLog_;

	START_LOG( cerr, wstrLog.c_str() )
		<< BUILD_LOG( GetName() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOG( GetCharUID() )
		<< BUILD_LOG( m_kSkillTree.GetActiveSkillPageNumber() )
		<< END_LOG;
}

void KGSUser::SendExpandSkillPageReqToGameDB( const unsigned short usEventID_, const int iTheNumberOfSkillPagesToBeAdded_ )
{
	KDBE_EXPAND_SKILL_PAGE_REQ kPacket;

	// 기본 스킬 조회
	if( !SiCXSLSkillTree()->GetUnitClassDefaultSkill( GetUnitClass(),
		kPacket.m_iDefaultSkill[0], kPacket.m_iDefaultSkill[1],
		kPacket.m_iDefaultSkill[2], kPacket.m_iDefaultSkill[3],
		kPacket.m_iDefaultSkill[4], kPacket.m_iDefaultSkill[5] ) )
	{
		START_LOG( cerr, L"유닛타입이 이상함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetUnitType() )
			<< END_LOG;
	}

	kPacket.m_usEventID = usEventID_;
	kPacket.m_iUnitUID	= GetCharUID();

	/// 추가되어야 할 페이지 넘버
	for ( int i = 1; i <= iTheNumberOfSkillPagesToBeAdded_; i++ )
		kPacket.m_vecSkillPageNumberToBeAdded.push_back( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() + i );

	kPacket.m_cUnitClass = GetUnitClass();

	// 스킬 포인트 얻어오기
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), kPacket.m_iSPoint );
	// 캐시 스킬 포인트 얻어오기
	kPacket.m_iCSPoint	=	m_kSkillTree.GetMaxCSPoint();

	// DB에 스킬페이지 확장 요청
	SendToGameDB( DBE_EXPAND_SKILL_PAGE_REQ, kPacket );
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
