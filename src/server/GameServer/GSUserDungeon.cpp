#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"
#include "NexonBillingTCPManager.h"
#include "X2Data/XSLCashItemManager.h"
//{{ 2008. 11. 24  최육사	랭킹 매니저
#include "GSRankingManager.h"
//}}

//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
//}}

#include "Enum/Enum.h"

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	#include "./GSWorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2012. 04. 23	박세훈	어둠의 문 개편
#include "../Common/X2Data/XSLBuffManager.h"
#include "UserManager.h"
//}}

#include "GameSysVal/GameSysVal.h"

//{{ 2013. 04. 15	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
//}}


//////////////////////////////////////////////////////////////////////////
#pragma NOTE( "The GSUserHandler.cpp file compiles successfully!" )
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

IMPL_ON_FUNC( EGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendToCnRoom( ERM_DUNGEON_PLAY_INFO_TO_SERVER_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_STAGE_LOAD_REQ )
{
	//{{ 2010. 9. 8	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_STAGE_LOAD_REQ, EGS_DUNGEON_STAGE_LOAD_ACK );
	//}}

	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	SiKPartyListManager()->GetPartyFever( GetPartyUID(), kPacket_.m_sPartyFever );
	//}}

	SendToCnRoom( ERM_DUNGEON_STAGE_LOAD_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_STAGE_LOAD_ACK, KEGS_DUNGEON_STAGE_LOAD_ACK )
{
	//{{ 2010. 04. 13  최육사	상태 오류
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_STAGE_LOAD_ACK );
	//}}

	SendPacket( EGS_DUNGEON_STAGE_LOAD_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_STAGE_LOAD_NOT, KEGS_DUNGEON_STAGE_LOAD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_STAGE_LOAD_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_STAGE_LOAD_COMPLETE_REQ )
{
	//{{ 2010. 9. 8	최육사	중복 패킷 필터 개선
	_VERIFY_STATE_REPEAT_FILTER( cerr, ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_STAGE_LOAD_COMPLETE_REQ, EGS_DUNGEON_STAGE_LOAD_COMPLETE_ACK, KPacketOK );
	//}}

	SendToCnRoom( ERM_DUNGEON_STAGE_LOAD_COMPLETE_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_STAGE_LOAD_COMPLETE_ACK, KPacketOK )
{
	//{{ 2010. 04. 13  최육사	상태 오류
	_VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_STAGE_LOAD_COMPLETE_ACK, KPacketOK );
	//}}

	SendPacket( EGS_DUNGEON_STAGE_LOAD_COMPLETE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_STAGE_LOAD_COMPLETE_NOT, KEGS_DUNGEON_STAGE_LOAD_COMPLETE_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_STAGE_LOAD_COMPLETE_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_STAGE_START_NOT, KEGS_DUNGEON_STAGE_START_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_STAGE_START_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_SUB_STAGE_CLEAR_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendToCnRoom( ERM_DUNGEON_SUB_STAGE_CLEAR_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_CLEAR_ACK, KEGS_DUNGEON_SUB_STAGE_CLEAR_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SUB_STAGE_CLEAR_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_SECRET_STAGE_LOAD_REQ )
{
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_STAGE_LOAD_ACK );

	SendToCnRoom( ERM_SECRET_STAGE_LOAD_REQ, kPacket_ );
}
_IMPL_ON_FUNC( ERM_SECRET_STAGE_LOAD_ACK, KEGS_DUNGEON_STAGE_LOAD_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	SiKPartyListManager()->GetPartyFever( GetPartyUID(), kPacket_.m_sPartyFever );
	//}}


	SendToCnRoom( ERM_DUNGEON_STAGE_LOAD_REQ, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_SUB_STAGE_OPEN_REQ )
{
	//{{ 2010. 9. 8	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_OPEN_REQ, EGS_DUNGEON_SUB_STAGE_OPEN_ACK );
	//}}

	SendToCnRoom( ERM_DUNGEON_SUB_STAGE_OPEN_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_OPEN_ACK, KEGS_DUNGEON_SUB_STAGE_OPEN_ACK )
{
	//{{ 2010. 04. 13  최육사	상태 오류
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_OPEN_ACK );
	//}}

	SendPacket( EGS_DUNGEON_SUB_STAGE_OPEN_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_OPEN_NOT, KEGS_DUNGEON_SUB_STAGE_OPEN_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SUB_STAGE_OPEN_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ )
{
	//{{ 2010. 9. 8	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ, EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK );
	//}}

	SendToCnRoom( ERM_DUNGEON_SUB_STAGE_GO_NEXT_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_GO_NEXT_ACK, KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK )
{
	//{{ 2010. 04. 13  최육사	상태 오류
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK );
	//}}

	SendPacket( EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT, KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ )
{
	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ, EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK );
	//}}

	SendToCnRoom( ERM_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK, KEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK )
{
	//{{ 2010. 03. 17  최육사	상태 오류 수정
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK );
	//}}

	SendPacket( EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SUB_STAGE_START_NOT, KEGS_DUNGEON_SUB_STAGE_START_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SUB_STAGE_START_NOT, kPacket_ );

	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	if( IsReserveKickUser() == true )
	{
		KERM_LEAVE_ROOM_REQ kPacketReq;
		kPacketReq.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_22;
		kPacketReq.m_iItemID = GetDungeonRequiredItemID();
		SendToCnRoom( ERM_LEAVE_ROOM_REQ, kPacketReq );

		SetReserveKickUser( false );
		SetDungeonRequiredItemID( 0 );
	}
	//}}
}

//{{ 2009. 7. 20  최육사	비밀스테이지
IMPL_ON_FUNC_NOPARAM( EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_REQ )
{
	//{{ 2010. 9. 8	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_ROOM ), EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_REQ, EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK );
	//}}

	if( GetPartyUID() <= 0 )
	{
		START_LOG( cerr, L"partyuid값이 이상하다! 절대 일어나서는 안되는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetPartyUID() )
			<< BUILD_LOG( GetRoomUID() )
			<< END_LOG;

		KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK kPacket;
		kPacket.m_bCanEnterSecretStage = false;
		SendPacket( EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK, kPacket );
		return;
	}

	//{{ 2010. 03. 02  최육사	채널 통합 파티
	if( SiKPartyListManager()->E_DungeonSecretStageEnterCheck( GetThisPtr<KGSUser>() ) == false )
	//}}
	{
		START_LOG( cerr, L"비밀 스테이지 입장 조건 체크 실패" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetPartyUID() )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( ERM_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK, KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT, KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DUNGEON_SECRET_STAGE_ENTER_SELECT_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendToCnRoom( ERM_DUNGEON_SECRET_STAGE_ENTER_SELECT_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT, KEGS_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT, kPacket_ );
}
//}}

_IMPL_ON_FUNC( ERM_END_GAME_DUNGEON_RESULT_DATA_NOT, KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );


	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	if( ( CXSLDungeon::IsDefenceDungeon( kPacket_.m_iDungeonID ) == true ) &&
		( SiKGSWorldMissionManager()->GetIsActive() == true )
		)
	{
		if( kPacket_.m_bIsWin == true )
		{
			KGSPartyPtr partyPtr = SiKPartyListManager()->GetParty( GetPartyUID() );

			// 솔플 유저 였음
			if( partyPtr == NULL)
			{
				SendToGlobalServer( EGB_CLEAR_MISSION_DUNGEON_NOT, 1 );
			}
			else if( partyPtr != NULL && partyPtr->GetHostUID() == GetCharUID() )
			{
				//	방장의 경우에만 글로벌서버에 기록한다.
				SendToGlobalServer( EGB_CLEAR_MISSION_DUNGEON_NOT, partyPtr->GetNumMember() );
			}
		}

	}
	//}}

	//{{ 2010. 05. 17  최육사	대전 던전 서버군 통합
	std::set< UidType > setPartyMembers;
	//}}

	// 던전 클리어 했는가?
	if( kPacket_.m_bIsWin == false )
	{
		// 던전 클리어 실패이므로 아무런 처리를 하지 않는다!
		//{{ 2011. 04. 13  김민성  글로벌 서버 추가
		kPacket_.m_iProtectedSmallCrystal= 0;
		//}} 2011. 04. 13  김민성  글로벌 서버 추가


		goto end_proc;
	}

	// RoomType 검사
	if( CXSLRoom::GetRoomType( GetRoomUID() ) != CXSLRoom::RT_DUNGEON )
	{
		START_LOG( cerr, L"RoomType이 던전이 아닌데 던전결과창 정보가 내려왔다?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;

		goto end_proc;
	}

	//{{ 버프 관리 시스템 - 김민성
	{
		bool bRefreshBuff = false;		// 버프 갱신 해야하는가?
		int iBuffID = CXSLBuffManager::BTI_NONE;
		std::vector<KBuffInfo> vecActiveBuff;
		std::vector<int> vecDeleteBuff;
		std::vector<int> vecDeactiveBuff;

		// 귀환자가 아닌데 버프가 있다면 지운다.
		if( m_kComeBackUserInfo.GetIsComeBackUser() == false && kPacket_.m_bComeBackUserInParty == false )
		{
			DeactivateBuff( CXSLBuffManager::BTI_BUFF_RETURN_OF_HERO, vecDeactiveBuff );

			bRefreshBuff = true;
		}

		//{{ 2013. 04. 13	박세훈	어둠의 문 개편
		//}}

		if( bRefreshBuff == true )
		{
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_vecActivateBuff = vecActiveBuff;
			kReq.m_vecDeActivateBuff = vecDeactiveBuff;
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
	}
	//}

	//{{ 2010. 05. 17  최육사	대전 던전 서버군 통합
	for( const KDungeonUnitResultInfo& kDungeonUnitResultInfo : kPacket_.m_vecDungeonUnitInfo )
	{
		setPartyMembers.insert( kDungeonUnitResultInfo.m_UnitUID );
	}
	//}}

	// 해당 캐릭터 정보를 찾아서 업데이트
	for( KDungeonUnitResultInfo& kDungeonUnitResultInfo : kPacket_.m_vecDungeonUnitInfo )
	{
		if( kDungeonUnitResultInfo.m_UnitUID != GetCharUID() )
			continue;

		std::map< int, KSubQuestInfo > mapOngoingQuest;

		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		kPacket_.GetOngoingQuestByUnitUID( GetCharUID(), mapOngoingQuest );
		//}}


		//{{ 2012. 12. 21  서브 퀘스트 타입 추가 (적정 레벨 던전 클리어) - 김민성
		m_kUserQuestManager.Handler_OnDungeonClear(
			GetThisPtr<KGSUser>(),
			kPacket_.m_iDungeonID,
			kPacket_.m_cDifficulty,
			kPacket_.m_cDungeonMode,
			kDungeonUnitResultInfo.m_cTotalRank,
			kPacket_.m_nSec,
			kDungeonUnitResultInfo.m_nDamaged,
			setPartyMembers,
			mapOngoingQuest,
			kPacket_.m_mapHaveExpInDungeon
			, kPacket_.m_mapSuitableLevelInfo
			, kPacket_.m_bSpecChar1, kPacket_.m_bSpecChar2
			, kPacket_.m_vecRelation
			);
		//}}


		//{{ 2013. 03. 21	 계정 퀘스트 - 적정 레벨 던전 클리어 조건 추가 - 김민성
		m_kUserAccountQuestManager.Handler_OnDungeonClear( GetThisPtr<KGSUser>(), kPacket_.m_iDungeonID, kPacket_.m_cDifficulty, kPacket_.m_cDungeonMode, setPartyMembers, kPacket_.m_mapHaveExpInDungeon, kPacket_.m_mapSuitableLevelInfo );
		//}

		//{{ 2010. 8. 23	최육사	헬모드 칭호
		m_kUserTitleManager.OnDungeonClear( GetThisPtr<KGSUser>(), kPacket_.m_iDungeonID, kPacket_.m_cDifficulty, kPacket_.m_cDungeonMode, kDungeonUnitResultInfo.m_cTotalRank, kPacket_.m_nSec, kDungeonUnitResultInfo.m_nDamaged, setPartyMembers );
		//}}

		//튜토리얼이면 보상을 한번만 주기위해..
		if( CXSLDungeon::IsTutorialDungeon( kPacket_.m_iDungeonID ) == true )
		{
			std::map< int, KDungeonClearInfo >::iterator mit;
			mit = m_mapDungeonClear.find( kPacket_.m_iDungeonID );

			if( mit != m_mapDungeonClear.end() )
			{
				kDungeonUnitResultInfo.m_nED = 0;
				kDungeonUnitResultInfo.ResetEXP();
			}
		}

		//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
		//}}

		break;
	}

end_proc:

	SendPacket( EGS_END_GAME_DUNGEON_RESULT_DATA_NOT, kPacket_ );
}

IMPL_ON_FUNC( ERM_UPDATE_DUNGEON_UNIT_INFO_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( kPacket_.m_bDungeonClear == true )
	{
		//////////////////////////////////////////////////////////////////////////
		//튜토리얼은 보상을 한번만 주기위해..
		if( CXSLDungeon::IsTutorialDungeon( kPacket_.m_iDungeonID ) == true )
		{
			std::map< int, KDungeonClearInfo >::iterator mit;
			mit = m_mapDungeonClear.find( kPacket_.m_iDungeonID );

			if( mit != m_mapDungeonClear.end() )
			{
				kPacket_.m_iED	= 0;
				kPacket_.m_iEXP	= 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//게임시작후 첫번째 던전(초심자의 숲 노말) 클리어시 클리어 보너스 exp +800 주기위해
		//#첫번째 던전클리어인지는 룸서버에서 알수없고(현재 유저정보 최적화시 빠짐)
		// 첫번째 던전인 초심자의 숲은 혼자플레이만 될수있게 진행되기때문에 이곳에서 직접해준다.
		//if( m_mapDungeonClear.empty() == true &&
		//	kPacket_.m_iDungeonID == SEnum::DI_EL_FOREST_GATE_NORMAL )
		//{
		//	kPacket_.m_iEXP += 800;
		//}
	}

	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_DUNGEON_PLAY_TIME, kPacket_.m_iPlayTime / 60 );
	//}}

	m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Dungeon_Play_Time, kPacket_.m_iPlayTime );

	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	int iBeforeED = m_iED;

	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}}

	// 보너스 ED
	m_iED += kPacket_.m_iED;

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	// ED 어뷰저
	m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_DUNGEON_REWARD, iBeforeED );
	//}}
	//}}

	KStatisticsKey kKey;
	kKey.m_vecIntKey.push_back( 0 );

	// ED 페널티
	if( kPacket_.m_iED < 0 )
	{
		KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MDunPenalty, -kPacket_.m_iED );
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_MDunPenalty, kPacket_.m_iED );
	}
	else
	{
		KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_PDungeon, kPacket_.m_iED );

		// 유저 통계
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_PDungeon, kPacket_.m_iED );

	}

	// 보너스 경험치
	m_kEXP.AddExp( kPacket_.m_iEXP );

	//Update Level/Stat
	CheckCharLevelUp();

	//{{ 버프 관리 시스템 - 김민성
	// 버프 처리
	{
		std::vector< int > vecDeactivateBuff;
		m_kUserBuffManager.OnEndDungeonGame( GetThisPtr<KGSUser>(), kPacket_.m_bDungeonClear, kPacket_.m_bStartedByAutoParty, vecDeactivateBuff );
		if( vecDeactivateBuff.empty() == false )
		{
			// 로그인 서버 버프 매니저에 업데이트
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_vecDeActivateBuff = vecDeactivateBuff;
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
	}
	//}}

	std::map< int, KDungeonPlayInfo >::iterator mitDungeonPlay = m_mapDungeonPlay.find( kPacket_.m_iDungeonID );
	if( mitDungeonPlay == m_mapDungeonPlay.end() )
	{
		// 일단 이 놈들은 해킹유저. 큰 문제는 아니니 차감횟수만 셋팅하자.
		KDungeonPlayInfo kDungeonPlayInfo;
		kDungeonPlayInfo.m_iDungeonID = kPacket_.m_iDungeonID;
		kDungeonPlayInfo.m_iPlayTimes = 1;
		kDungeonPlayInfo.m_iClearTimes = static_cast<int>( kPacket_.m_bDungeonClear );
		kDungeonPlayInfo.m_bNew = true;

		m_mapDungeonPlay.insert( std::make_pair( kDungeonPlayInfo.m_iDungeonID, kDungeonPlayInfo ) );
	}
	else
	{
		mitDungeonPlay->second.m_iClearTimes += static_cast<int>( kPacket_.m_bDungeonClear );
		mitDungeonPlay->second.m_bNew = true;
	}

	//{{ 2012. 05. 14	최육사	던전 몬스터 레벨 지정
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//Update Dungeon Clear data
	if( kPacket_.m_bDungeonClear == true )
	{
		std::map< int, KDungeonClearInfo >::iterator mit;
		mit = m_mapDungeonClear.find( kPacket_.m_iDungeonID );
		if( mit == m_mapDungeonClear.end() )
		{
			bool bIsClear = false;

			// 튜토리얼 던전은 무조건 클리어
			if( CXSLDungeon::IsTutorialDungeon( kPacket_.m_iDungeonID ) == true )
			{
				bIsClear = true;

				KDungeonClearInfo kDCInfo;

				CTime tCurrentTime = CTime::GetCurrentTime();
				kDCInfo.m_iDungeonID	= kPacket_.m_iDungeonID;
				kDCInfo.m_wstrClearTime = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
				kDCInfo.m_iMaxScore		= kPacket_.m_iMaxScore;
				kDCInfo.m_cMaxTotalRank	= kPacket_.m_cMaxTotalRank;
				kDCInfo.m_bNew = true;
				m_mapDungeonClear.insert( std::make_pair( kDCInfo.m_iDungeonID, kDCInfo ) );

				//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
				m_kUserStatistics.SetLastClearDungeonID( kPacket_.m_iDungeonID );
				WriteCharacterLogToDBWithDungeonID( KELOG_USER_CHARACTER_LOG_NOT::LT_DUNGEON_PLAY_COMPLETE, kPacket_.m_iDungeonID );
				//}}
			}
			else
			{
				// 선행 던전을 클리어 했는지 검사
				const int iDifficulty = (kPacket_.m_iDungeonID % 10);
				const int iDungeonIDWithOutDif = kPacket_.m_iDungeonID - iDifficulty;
				int iRequireDungeonID = SiCXSLDungeonManager()->GetRequireDungeonID( iDungeonIDWithOutDif );
				if( iRequireDungeonID > 0 )
				{
					std::map<int,KDungeonClearInfo>::iterator mit;
					mit = m_mapDungeonClear.find( iRequireDungeonID );
					if( mit != m_mapDungeonClear.end() )
					{
						bIsClear = true;
					}
				}
				else
				{
					bIsClear = true;
				}

				if( bIsClear == true )
				{
					switch( iDifficulty )
					{
					case CXSLDungeon::DL_EXPERT:
						{
							const int iExpertDungeonID = iDungeonIDWithOutDif + CXSLDungeon::DL_EXPERT;

							if( m_mapDungeonClear.find( iExpertDungeonID ) == m_mapDungeonClear.end() )
							{
								KDungeonClearInfo kDCInfo;
								CTime tCurrentTime = CTime::GetCurrentTime();
								kDCInfo.m_iDungeonID	= iExpertDungeonID;
								kDCInfo.m_wstrClearTime = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
								kDCInfo.m_iMaxScore		= kPacket_.m_iMaxScore;
								kDCInfo.m_cMaxTotalRank	= kPacket_.m_cMaxTotalRank;
								kDCInfo.m_bNew = true;
								m_mapDungeonClear.insert( std::make_pair( kDCInfo.m_iDungeonID, kDCInfo ) );

								//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
								m_kUserStatistics.SetLastClearDungeonID( iExpertDungeonID );
								WriteCharacterLogToDBWithDungeonID( KELOG_USER_CHARACTER_LOG_NOT::LT_DUNGEON_PLAY_COMPLETE, iExpertDungeonID );
								//}}
							}
						}
						// 의도적으로 break를 쓰지 않았습니다.

					case CXSLDungeon::DL_HARD:
						{
							const int iHardDungeonID = iDungeonIDWithOutDif + CXSLDungeon::DL_HARD;

							if( m_mapDungeonClear.find( iHardDungeonID ) == m_mapDungeonClear.end() )
							{
								KDungeonClearInfo kDCInfo;
								CTime tCurrentTime = CTime::GetCurrentTime();
								kDCInfo.m_iDungeonID	= iHardDungeonID;
								kDCInfo.m_wstrClearTime = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
								kDCInfo.m_iMaxScore		= kPacket_.m_iMaxScore;
								kDCInfo.m_cMaxTotalRank	= kPacket_.m_cMaxTotalRank;
								kDCInfo.m_bNew = true;
								m_mapDungeonClear.insert( std::make_pair( kDCInfo.m_iDungeonID, kDCInfo ) );

								//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
								m_kUserStatistics.SetLastClearDungeonID( iHardDungeonID );
								WriteCharacterLogToDBWithDungeonID( KELOG_USER_CHARACTER_LOG_NOT::LT_DUNGEON_PLAY_COMPLETE, iHardDungeonID );
								//}}
							}
						}
						// 의도적으로 break를 쓰지 않았습니다.

					case CXSLDungeon::DL_NORMAL:
						{
							const int iNormalDungeonID = iDungeonIDWithOutDif;

							if( m_mapDungeonClear.find( iNormalDungeonID ) == m_mapDungeonClear.end() )
							{
								KDungeonClearInfo kDCInfo;
								CTime tCurrentTime = CTime::GetCurrentTime();
								kDCInfo.m_iDungeonID	= iNormalDungeonID;
								kDCInfo.m_wstrClearTime = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
								kDCInfo.m_iMaxScore		= kPacket_.m_iMaxScore;
								kDCInfo.m_cMaxTotalRank	= kPacket_.m_cMaxTotalRank;
								kDCInfo.m_bNew = true;
								m_mapDungeonClear.insert( std::make_pair( kDCInfo.m_iDungeonID, kDCInfo ) );

								//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
								m_kUserStatistics.SetLastClearDungeonID( iNormalDungeonID );
								WriteCharacterLogToDBWithDungeonID( KELOG_USER_CHARACTER_LOG_NOT::LT_DUNGEON_PLAY_COMPLETE, iNormalDungeonID );
								//}}
							}
						}
						break;

					default:
						START_LOG( cerr, "난이도 정보가 이상합니다!" )
							<< BUILD_LOG( kPacket_.m_iDungeonID )
							<< BUILD_LOG( iDifficulty )
							<< END_LOG;
						break;
					}
				}
			}
		}
		//클리어했던 던전이 있다면 최고점수 갱신체크
		else
		{
			if( CXSLDungeon::IsTutorialDungeon( kPacket_.m_iDungeonID ) == false )
			{
				bool bIsChange = false;

				if( mit->second.m_iMaxScore < kPacket_.m_iMaxScore )
				{
					bIsChange = true;
					mit->second.m_iMaxScore = kPacket_.m_iMaxScore;
				}

				if( mit->second.m_cMaxTotalRank < kPacket_.m_cMaxTotalRank )
				{
					bIsChange = true;
					mit->second.m_cMaxTotalRank = kPacket_.m_cMaxTotalRank;
				}

				if( bIsChange == true )
					mit->second.m_bNew = true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//}}

	//{{ 2009. 7. 6  최육사		랭킹정보 업데이트
	if( !kPacket_.m_kHenirRankingInfo.empty()  &&  !IsGuestUser() )
	{
		std::vector< KHenirRankingInfo >::const_iterator vitHR = kPacket_.m_kHenirRankingInfo.begin();
		if( vitHR != kPacket_.m_kHenirRankingInfo.end() )
		{
			// 개인 기록 저장
			m_kUserRankingManager.SetRankingInfo( *vitHR );

			// 100위안에 드는 랭킹인지 검사!
			if( SiKGSRankingManager()->CheckNewRecord( *vitHR ) == true )
			{
				// 100위안에 들었다!
				KELG_NEW_RECORD_HENIR_RANKING_NOT kNot;
				kNot.m_kNewRecordRanking = *vitHR;
				SendToLoginServer( ELG_NEW_RECORD_HENIR_RANKING_NOT, kNot );
			}
		}
		else
		{
			START_LOG( cerr, L"랭킹정보가 이상하다? 일어나서는 안되는 에러!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_kHenirRankingInfo.size() )
				<< END_LOG;
		}
	}
	//}}

	//{{ 2012. 02. 23	최육사	배틀필드 시스템
	if( GetPartyUID() > 0 )
	{
		SendToCnParty( ERM_END_DUNGEON_GAME_PARTY_FEVER_MODIFY_NOT );
	}
	//}}

	//{{ 2010. 8. 6	최육사	펫 시스템
	m_kUserPetManager.Handler_ERM_UPDATE_DUNGEON_UNIT_INFO_NOT();
	//}}

	KEGS_UPDATE_UNIT_INFO_NOT kUpdatePacketNot;

	//아이템이 있으면 신규아이템에 대한 db처리를 할것인지 체크
	if( !kPacket_.m_mapResultItem.empty() )
	{
		KDBE_INSERT_ITEM_REQ kPacketReq;
		kPacketReq.m_iUnitUID = GetCharUID();
		//{{ 2010. 9. 8	최육사	아이템 획득 사유
		kPacketReq.m_cGetItemReason = SEnum::GIR_DUNGEON_RESULT;
		//}}

		int iEventItemID = SiKGameSysVal()->GetDungeonClearPaymentItemID();

		std::map< int, KItemInfo >::iterator mitEventItemCheck = kPacket_.m_mapResultItem.find( iEventItemID );
		if( mitEventItemCheck != kPacket_.m_mapResultItem.end() )
		{
			int iQuantity = 0;

			if( m_kInventory.GetQuantity( iEventItemID, iQuantity ) == true )   // 인벤토리에 현자의 주문서가 있다면 지급하지 않는다.
			{
				std::map< int, int >::iterator mitGetItem = kPacket_.m_mapGetItem.find( iEventItemID );
				if( mitGetItem != kPacket_.m_mapGetItem.end() )
				{
					kPacket_.m_mapGetItem.erase( mitGetItem );	// 결과창에서 보여지는 아이템
				}

				kPacket_.m_mapResultItem.erase( mitEventItemCheck );	// 실제 습득 아이템
			}
			else if( m_kUserPetManager.ExistItemInPetInventory( iEventItemID ) == true ) // 펫 인벤토리에 있으면 지급하지 않는다.
			{
				std::map< int, int >::iterator mitGetItem = kPacket_.m_mapGetItem.find( iEventItemID );
				if( mitGetItem != kPacket_.m_mapGetItem.end() )
				{
					kPacket_.m_mapGetItem.erase( mitGetItem );	// 결과창에서 보여지는 아이템
				}

				kPacket_.m_mapResultItem.erase( mitEventItemCheck );	// 실제 습득 아이템
			}
		}

		//{{ 2011. 12.13    김민성	던전 클리어 시 아이템 지급 이벤트 - 현자의 주문서(중복 지급 금지)
		//}}
		// 삭제할 아이템 목록은 따로 관리
		std::map< int, int > mapConsumingItem;

		std::map< int, KItemInfo >::iterator mitResultItem = kPacket_.m_mapResultItem.begin();
		while( mitResultItem != kPacket_.m_mapResultItem.end() )
		{
			int iItemID = mitResultItem->first;
			KItemInfo kItemInfo = mitResultItem->second;

			START_LOG( clog, L"소모되는 아이템" )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( kItemInfo.m_iQuantity );

			if( kItemInfo.m_iQuantity <= 0 )
			{
				kPacket_.m_mapResultItem.erase( mitResultItem++ );

				if( kItemInfo.m_iQuantity < 0 )
				{
					mapConsumingItem.insert( std::make_pair( iItemID, -kItemInfo.m_iQuantity ) );
				}
			}
			else
			{
				mitResultItem++;
			}
		}

		KEGS_UPDATE_INVENTORY_SLOT_INFO_NOT kPacket;
		m_kInventory.DeleteItemOnlyInventory( mapConsumingItem, kPacket.m_vecInventorySlotInfo, KDeletedItemInfo::DR_DUNGEON_REWARD );
		SendPacket( EGS_UPDATE_INVENTORY_SLOT_INFO_NOT, kPacket );

		m_kInventory.PrepareInsert( kPacket_.m_mapResultItem, kPacketReq.m_mapInsertedItem, kPacketReq.m_vecUpdatedInventorySlot, kPacketReq.m_vecItemInfo );

		//////////////////////////////////////////////////////////////////////////
		// 통계 : 게임중 습득아이템 정보
		int iDropItemCnt = 0;
		std::map< int, int >::const_iterator mit;
		for( mit = kPacketReq.m_mapInsertedItem.begin(); mit != kPacketReq.m_mapInsertedItem.end(); ++mit )
		{
			const int iItemID = mit->first;
			const int iQuantity = mit->second;

			// ED아이템인지 검사
			switch( iItemID )
			{
			case CXSLItem::EDI_BRONZE_ED:
			case CXSLItem::EDI_SILVER_ED:
			case CXSLItem::EDI_GOLD_ED:
				continue;
			}

			// 일반아이템이라면 카운트
			iDropItemCnt += iQuantity;

			// 통계 : Local Item
			{
				KStatisticsKey kKey;
				kKey.m_vecIntKey.push_back( iItemID );
				KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_DROP, iQuantity );
				//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
				KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_DROP, iQuantity );
				//}}
			}
		}

		// 유저 통계 습득 아이템
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Obtained_Item, iDropItemCnt );

		//{{ 2010. 11. 17	최육사	오토핵 검증 기능
		m_kUserAbuserManager.IncreaseGetItemCount( GetThisPtr<KGSUser>(), iDropItemCnt );
		//}}

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		// 아이템 어뷰저
		m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_DUNGEON, kPacketReq.m_mapInsertedItem );
		//}}
		//////////////////////////////////////////////////////////////////////////

		// 인벤토리에 들어가지 못한 아이템이 있는지 검사!
		std::map< int, KItemInfo >::const_iterator mitResult;
		for( mitResult = kPacket_.m_mapResultItem.begin(); mitResult != kPacket_.m_mapResultItem.end(); ++mitResult )
		{
			std::map< int, int >::const_iterator mitFind = kPacketReq.m_mapInsertedItem.find( mitResult->first );
			if( mitFind == kPacketReq.m_mapInsertedItem.end() )
			{
				// 인벤토리에 하나도 못넣었단 말이냐!?
				for( int i = 0; i < mitResult->second.m_iQuantity; ++i )
				{
					//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
					AddTempInventoryItem( mitResult->first
										, 0
										//{{ 2012. 1. 16	박세훈	던전 및 필드 드랍 아이템에 강화 부여
										, mitResult->second
										//}}
										);
					//}}
				}
			}
			else
			{
				// 몇개정도 넣었나?
				int iGetCount = mitResult->second.m_iQuantity - mitFind->second;
				LIF( iGetCount >= 0 );
				for( int i = 0; i < iGetCount; ++i )
				{
					//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
					AddTempInventoryItem( mitResult->first
										, 0
										//{{ 2012. 1. 16	박세훈	던전 및 필드 드랍 아이템에 강화 부여
										, mitResult->second
										//}}
										);
					//}}
				}
			}
		}

		// DB로 보낼정보가 있는가?
		if( !kPacketReq.m_vecItemInfo.empty() )
		{
			m_kInventory.FlushQuantityChange( kPacketReq.m_kItemQuantityUpdate.m_mapQuantityChange );
			m_kInventory.FlushDeletedItem( kPacketReq.m_kItemQuantityUpdate.m_vecDeleted );
			kPacketReq.m_bOutRoom = kPacket_.m_bOutRoom;

			//{{ 2011. 05. 04  김민성	던전 클리어시 아이템 지급 조건 추가
			kPacketReq.m_mapGetItem = kPacket_.m_mapGetItem;
			//}}

			SendToGameDB( DBE_INSERT_ITEM_REQ, kPacketReq );
			return;
		}
		else
		{
			kUpdatePacketNot.m_vecKInventorySlotInfo = kPacketReq.m_vecUpdatedInventorySlot;
		}
	}

	// Update Server Unit data
	KRoomUserInfo kInfo;
	GetRoomUserInfo( kInfo );

	// 방에서 나가는 경우가 아닐경우 센터서버에 업데이트를 시켜준다.
	// 아이템 처리가 끝난후 업데이트를 해야하기때문에 이곳에서 함.
	if( kPacket_.m_bOutRoom == false )
	{
		SendToCnRoom( ERM_SET_UNIT_INFO_NOT, kInfo );
	}

	// 유저에게 수정된 데이터를 넘겨준다.
	GetUnitInfo( kUpdatePacketNot.m_kUnitInfo );
	kUpdatePacketNot.m_bGameEnd = true;
	kUpdatePacketNot.m_mapItemObtained = kPacket_.m_mapGetItem;

	SendPacket( EGS_UPDATE_UNIT_INFO_NOT, kUpdatePacketNot );
}

IMPL_ON_FUNC( EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( IsHenirDungeonChallengeMode() )
	{
		// 헤니르 시공에서 도전모드는 부활석 사용 불가!
		START_LOG( cerr, L"클라이언트에서 도전모드인지 체크했을텐데.." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_RESURRECT_03;
		SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacketAck );
		return;
	}

	if( !kPacket_.m_bUseCash )
	{
		//{{ 2009. 10. 14  최육사	자동결제 부활석
		if( m_iNumResurrectionStone <= 0  &&  m_iNumAutoPaymentResStone <= 0 )
		{
			KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_RESURRECT_00;
			SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacket );
			return;
		}
		//}}

		kPacket_.m_iUsedByUnitUID = GetCharUID(); // 부활석 사용자
		SendToCnRoom( ERM_RESURRECT_TO_CONTINUE_DUNGEON_REQ, kPacket_ );
	}
	else
	{
		//{{ 2011. 10. 12	최육사	컨텐츠 관리자
		if( SiKGSContentManager()->IsEnableCashShop() == false )
		{
			START_LOG( clog, L"캐쉬샵 점검중 캐쉬 부활석 사용 요청" )
				<< BUILD_LOG( GetName() );

			KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacketAck;
			kPacketAck.m_iOK = NetError::ERR_CONTENT_00;
			SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacketAck );
			return;
		}
		//}}

		// 체험ID 기능 제한
		if( IsGuestUser() )
		{
			KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_GUEST_USER_00;
			SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacket );
			return;
		}

		// 실시간 부활석 사용 시간 제한
		if( m_kTimer[TM_CASH_RES_STONE].elapsed() < 2.0 )
			return;

		m_kTimer[TM_CASH_RES_STONE].restart();

//////////////////////////////////////////////////////////////////////////

		const int ciResurrectionStoneID = 51;

		KBillProductInfo kBillProductInfo;
		if( !SiKBillManager()->GetProductInfo( ciResurrectionStoneID, kBillProductInfo ) )
		{
			START_LOG( cerr, L"실시간 부활석 상품 정보 추출 실패." )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;

			KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_00;
			SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacket );
			return;
		}

		KEBILL_BUY_PRODUCT_REQ kPacketReq;
		kPacketReq.m_wstrTotalOrderID = MakeOrderID();	// TotalOrderID를 먼저 발급
		kPacketReq.m_iServerGroupID = KBaseServer::GetKObj()->GetServerGroupID();
		kPacketReq.m_wstrUserID = GetName();
		kPacketReq.m_uiPublisherUID = m_kNexonAccountInfo.m_uiNexonSN;
		kPacketReq.m_iUserUID = GetUID();
		kPacketReq.m_iUnitUID = GetCharUID();
		kPacketReq.m_wstrNickName = GetCharName();
		kPacketReq.m_strUserIP = GetIPStr();

		// 허상형 : [2010/7/29/] //	캐시 로그
		kPacketReq.m_BuyCharInfo.m_cLevel = GetLevel();
		kPacketReq.m_BuyCharInfo.m_iUnitClass = GetUnitClass();

		kPacketReq.m_iUserIP = GetIP();

		kPacketReq.m_iUseCashType = (int)KGlobalCashInfo::GCT_PUBLISHER_CASH;



		KBillBuyInfo kInfo;
		kInfo.m_wstrOrderID = MakeOrderID();
		kInfo.m_iProductNo = kBillProductInfo.m_iProductNo;
		kInfo.m_iPoint = kBillProductInfo.m_iSalePrice;
		kInfo.m_iProductID = kBillProductInfo.m_iProductID;
		kInfo.m_usOrderQuantity = 1;
		kInfo.m_iCategory = ( int )kBillProductInfo.m_cCategoryNo;
		kPacketReq.m_vecBillBuyInfo.push_back( kInfo );

		DWORD dwBillingFlag = KSimLayer::GetKObj()->GetBillingFlag();
		DWORD dwNationFlag = KSimLayer::GetKObj()->GetDwNationFlag();
		DWORD dwPublisherBillingConnectType = KSimLayer::GetKObj()->GetPublisherBillingConnectType();

		if(dwBillingFlag == KSimLayer::BF_INTERNAL)
		{
			SendToKOGBillingDB( EBILL_BUY_PRODUCT_REQ, kPacketReq );
		}
		else if (dwPublisherBillingConnectType == KSimLayer::PBCT_UNUSUAL && dwNationFlag == KSimLayer::NF_TW)
		{
			// 구분해서 DB로 보낼지 TCP로 보낼지 결정해야 함
			if (kPacketReq.m_iUseCashType == KGlobalCashInfo::GCT_PUBLISHER_CASH)
				SendToPublisherBilling( EBILL_BUY_PRODUCT_REQ, kPacketReq );
			else //
				SendToPublisherBillingDB( EBILL_BUY_PRODUCT_REQ, kPacketReq );
		}
		else if(dwPublisherBillingConnectType == KSimLayer::PBCT_DB)
		{
			SendToPublisherBillingDB( EBILL_BUY_PRODUCT_REQ, kPacketReq );
		}
		else
		{
			SendToPublisherBilling( EBILL_BUY_PRODUCT_REQ, kPacketReq );
		}
		// 찜에서 캐쉬구매
		m_kUserWishList.SetBuyCashItemInWishList( false );

		// 임시 데이터 설정
		m_kUserCashInventory.SetCashResurrectionStone( true );
		m_kUserCashInventory.SetCashRStoneUnitUID( kPacket_.m_iUnitUID );

	}
}

IMPL_ON_FUNC( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK )
{
	// 성공적으로 부활처리 되었는데 나가버리면 어떡하지? 그러면 소환석이 소모되지 않는다.
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		//{{ 2009. 10. 14  최육사	자동결제 부활석
		if( m_iNumResurrectionStone <= 0  &&  m_iNumAutoPaymentResStone <= 0 )
		{
			START_LOG( cerr, L"소환석 부족. 이미 이전에 확인한 내용이기 때문에 이런 상황이 벌어지면 안된다." )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( m_iNumResurrectionStone )
				<< BUILD_LOG( m_iNumAutoPaymentResStone )
				<< END_LOG;
		}
		else
		{
			// 만약 자동결제 부활석이 남아있다면 자동결제 부활석 부터 소모시킵니다.
			if( m_iNumAutoPaymentResStone > 0 )
			{
				m_iNumAutoPaymentResStone -= 1;
			}
			else
			{
				m_iNumResurrectionStone -= 1;
			}


			// 부활석 로그
			KELOG_RESURRECTION_STONE_LOG_NOT kPacketToLog;
			CTime kUsedTime = CTime::GetCurrentTime();
			kPacketToLog.m_UnitUID	   = GetCharUID();
			kPacketToLog.m_iDungeonID  = kPacket_.m_iDungeonID;
			kPacketToLog.m_wstrRegDate = ( CStringW )( kUsedTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
			SendToLogDB( ELOG_RESURRECTION_STONE_LOG_NOT, kPacketToLog );
		}

		//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
		UseResurrectionStone( true );		// 부활석 사용을 저장
		//}}
		//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
		AddUseResurrectionStoneCount();
		//}
		kPacket_.m_iNumResurrectionStone = m_iNumResurrectionStone;				// 부활석
		kPacket_.m_iNumAutoPaymentResStone = m_iNumAutoPaymentResStone;			// 자동결제 부활석
		kPacket_.m_bUseCash = m_kUserCashInventory.IsCashResurrectionStone();	// 실시간 부활석 여부
		//}}

		//{{ 2013. 04. 15	최육사	어둠의 문 개편
		// 어둠의 문에서 부활한거라면 전용 버프를 다시 걸자!
		if( CXSLDungeon::IsDefenceDungeon( m_kUserDungeonManager.GetDungeonID() ) == true )
		{
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;

			if( ActivateBuff( CXSLBuffManager::BTI_BUFF_GATE_DEFENCE, kReq.m_vecActivateBuff ) == true )
			{
				kReq.m_iUnitUID = GetCharUID();
				SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
			}
		}
		//}}
	}

	m_kUserCashInventory.SetCashResurrectionStone( false ); // 초기화

	SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_STOP_DUNGEON_CONTINUE_TIME_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendToCnRoom( ERM_STOP_DUNGEON_CONTINUE_TIME_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_STOP_DUNGEON_CONTINUE_TIME_ACK, KEGS_STOP_DUNGEON_CONTINUE_TIME_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_STOP_DUNGEON_CONTINUE_TIME_ACK, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( EGS_SKIP_CONTINUE_DUNGEON_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendToCnRoom( ERM_SKIP_CONTINUE_DUNGEON_NOT );
}

//{{ 2010. 05. 06  최육사	비밀던전 헬모드

_IMPL_ON_FUNC( ERM_DUNGEON_EFFECT_REMAINING_TIME_NOT, KEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_EFFECT_REMAINING_TIME_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_DUNGEON_EFFECT_TIME_OUT_NOT, KEGS_DUNGEON_EFFECT_TIME_OUT_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DUNGEON_EFFECT_TIME_OUT_NOT, kPacket_ );
}

//}}

IMPL_ON_FUNC( ERM_DUNGEON_USER_STATISTICS_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iGamePlayTime < 0 )
		kPacket_.m_iGamePlayTime = 0;

	// 유저 플레이 타임
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_TOTAL_PLAY_TIME, kPacket_.m_iGamePlayTime );
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_DUNGEON_PLAY_TIME, kPacket_.m_iGamePlayTime );
	//{{ 2012. 11. 14	박세훈	Field PT 로그 추가
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_SPIRIT_PLAY_TIME, kPacket_.m_iGamePlayTime );
	//}}

	// 유저 통계 대전
	switch( kPacket_.m_cGameResult )
	{
	case KERM_DUNGEON_USER_STATISTICS_NOT::UGR_WIN:
		{
			// 유저 통계 던전 승
			if( kPacket_.m_bIsPartyPlay == false )
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Win, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Win, kPacket_.m_iGamePlayTime );
			}
			else
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Party_Win, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Party_Win, kPacket_.m_iGamePlayTime );
			}
		}
		break;

	case KERM_DUNGEON_USER_STATISTICS_NOT::UGR_LOSE:
		{
			// 유저 통계 던전 패
			if( kPacket_.m_bIsPartyPlay == false )
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Lose, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Lose, kPacket_.m_iGamePlayTime );
			}
			else
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Party_Lose, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Party_Lose, kPacket_.m_iGamePlayTime );
			}
		}
		break;

	case KERM_DUNGEON_USER_STATISTICS_NOT::UGR_DROP:
		{
			// 유저 통계 던전 이탈
			if( kPacket_.m_bIsPartyPlay == false )
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Drop, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Drop, kPacket_.m_iGamePlayTime );
			}
			else
			{
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Party_Drop, 1 );
				m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_PT_Party_Drop, kPacket_.m_iGamePlayTime );
			}
		}
		break;

	default:
		START_LOG( cerr, L"이런 결과 타입은 없음." )
			<< BUILD_LOGc( kPacket_.m_cGameResult )
			<< END_LOG;
		break;
	}

	// 유저 통계 던전 부활석
	if( kPacket_.m_bIsPartyPlay == false )
	{
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_ReStone, kPacket_.m_iUsedResurrectionStone );
	}
	else
	{
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_Dungeon, kPacket_.m_iDungeonID, KUserStatistics::US_D_MC_Party_ReStone, kPacket_.m_iUsedResurrectionStone );
	}
}

IMPL_ON_FUNC( ERM_TC_USER_STATISTICS_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iGamePlayTime < 0 )
		kPacket_.m_iGamePlayTime = 0;

	// 유저 플레이 타임
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_TOTAL_PLAY_TIME, kPacket_.m_iGamePlayTime );

	// 유저 통계 훈련소
	switch( kPacket_.m_cGameResult )
	{
	case KERM_TC_USER_STATISTICS_NOT::UGR_SUCCESS:
		{
			// 유저 통계 훈련소 성공
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_MC_Success, 1 );
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_PT_Success, kPacket_.m_iGamePlayTime );
		}
		break;

	case KERM_TC_USER_STATISTICS_NOT::UGR_FAIL:
		{
			// 유저 통계 훈련소 실패
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_MC_Fail, 1 );
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_PT_Fail, kPacket_.m_iGamePlayTime );
		}
		break;

	case KERM_TC_USER_STATISTICS_NOT::UGR_DROP:
		{
			// 유저 통계 훈련소 이탈
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_MC_Drop, 1 );
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_TrainingCenter, kPacket_.m_iTCID, KUserStatistics::US_TC_PT_Drop, kPacket_.m_iGamePlayTime );
		}
		break;

	default:
		START_LOG( cerr, L"이런 결과 타입은 없음." )
			<< BUILD_LOGc( kPacket_.m_cGameResult )
			<< END_LOG;
		break;
	}
}

//{{ 2011. 01. 25  김민성  월드 트리거 전달(던전만 적용중 - 이후 PVP 확장)
IMPL_ON_FUNC( EGS_WORLD_TRIGGER_RELOCATION_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"방 상태인데 RoomUID가 없네? 이상하다." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		KEGS_WORLD_TRIGGER_RELOCATION_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_UNKNOWN;
		SendPacket( EGS_WORLD_TRIGGER_RELOCATION_ACK, kPacket );
		return;
	}

	SendToCnRoom( ECN_WORLD_TRIGGER_RELOCATION_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ECN_WORLD_TRIGGER_RELOCATION_ACK, KEGS_WORLD_TRIGGER_RELOCATION_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_WORLD_TRIGGER_RELOCATION_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ECN_WORLD_TRIGGER_RELOCATION_NOT, KEGS_WORLD_TRIGGER_RELOCATION_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_WORLD_TRIGGER_RELOCATION_NOT, kPacket_ );
}

//}}

//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
IMPL_ON_FUNC( EGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"방 상태인데 RoomUID가 없네? 이상하다." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;
		return;
	}

	SendToCnRoom( ERM_BAD_ATTITUDE_USER_CHECK_INFO_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_BAD_ATTITUDE_USER_MSG_NOT, KEGS_BAD_ATTITUDE_USER_MSG_NOT )
{
	SendPacket( EGS_BAD_ATTITUDE_USER_MSG_NOT , kPacket_ );
}

_IMPL_ON_FUNC( ERM_BAD_ATTITUDE_USER_NOT, KEGS_BAD_ATTITUDE_USER_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_BAD_ATTITUDE_USER_NOT , kPacket_ );
}

IMPL_ON_FUNC( EGS_FORCED_EXIT_VOTE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"방 상태인데 RoomUID가 없네? 이상하다." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;
		return;
	}

	SendToCnRoom( ERM_FORCED_EXIT_VOTE_REQ, kPacket_ );
}

IMPL_ON_FUNC( EGS_DEFENCE_DUNGEON_WAVE_ID_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"방 상태인데 RoomUID가 없네? 이상하다." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;
		return;
	}

	SendToCnRoom( ERM_DEFENCE_DUNGEON_WAVE_ID_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_FORCED_EXIT_VOTE_ACK, KEGS_FORCED_EXIT_VOTE_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_FORCED_EXIT_VOTE_ACK , kPacket_ );
}

IMPL_ON_FUNC( ERM_BAD_ATTITUDE_USER_LOG_NOT )
{
	SendToLogDB( DBE_BAD_ATTITUDE_USER_LOG_NOT, kPacket_ );
}
//}

//{{ 2013. 04. 09	최육사	어둠의 문 개편
_IMPL_ON_FUNC( ERM_DECISION_ENTER_DEFENCE_DUNGEON_NOT, KEGS_DECISION_ENTER_DEFENCE_DUNGEON_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_DECISION_ENTER_DEFENCE_DUNGEON_NOT, kPacket_ );
}

IMPL_ON_FUNC( ERM_PREPARE_FOR_DEFENCE_DUNGEON_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	//////////////////////////////////////////////////////////////////////////
	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"현재 방에 입장한 상태가 아닙니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
		return;
	}

	if( CXSLRoom::GetRoomType( GetRoomUID() ) != CXSLRoom::RT_DUNGEON )
	{
		START_LOG( cerr, L"현재 던전 방에 입장한 상태가 아닙니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	//{{ QUEST 개편 - 김민성
	m_kUserDungeonManager.SetDungeonGameInfo( kPacket_.m_kDefenceDungeonInfo );
	//}}

	KERM_PREPARE_FOR_DEFENCE_DUNGEON_ACK kPacketAck;
	GetRoomUserInfo( kPacketAck.m_kRoomUserInfo );
	SendToCnRoom( ERM_PREPARE_FOR_DEFENCE_DUNGEON_ACK, kPacketAck );

	//{{ 2013. 04. 22	최육사	어둠의 문 입장 로직 변경
	// 어둠의 문 입장 여부를 확인하기 위한 팝업 띄우는 패킷 전송
	SendID( EGS_ENTER_DEFENCE_DUNGEON_POPUP_NOT );
	//}}
}

_IMPL_ON_FUNC( ERM_DEFENCE_DUNGEON_GAME_START_NOT, KEGS_DEFENCE_DUNGEON_GAME_START_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	//////////////////////////////////////////////////////////////////////////
	if( GetRoomUID() > 0  &&  CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_BATTLE_FIELD )
	{
		KERM_LEAVE_ROOM_REQ kPacket;
		kPacket.m_bSendAck = false;
		kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_28;
		SendToCnRoom( ERM_LEAVE_ROOM_REQ, kPacket );

		// 이탈처리 했으니 여기서 쫑!
		SetRoomUID( 0 );

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			StateTransition( KGSFSM::I_TO_FIELD_MAP );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if( GetAuthLevel() < SEnum::UAL_GM )
		kPacket_.m_wstrCNIP.clear();

	//SetRoomUID( kPacket_.m_RoomInfo.m_RoomUID );

	//{{ QUEST 개편 - 김민성
	m_kUserDungeonManager.SetDungeonGameInfo( kPacket_.m_RoomInfo.m_iDungeonID, kPacket_.m_RoomInfo.m_DifficultyLevel, kPacket_.m_RoomInfo.m_cDungeonMode );
	//}}

	UseResurrectionStone( false );
	SetStartedNumMember( kPacket_.m_RoomInfo.m_JoinSlot );

	//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
	ClearUseResurrectionStoneCount();
	//}

	//{{ 2009. 10. 8  최육사	길드
	SendUpdateGuildMemberInfo();
	//}}

	//////////////////////////////////////////////////////////////////////////
	// 어둠의 문 전용 퀵슬롯 아이템 설정
	m_kInventory.SetupDefenceDungeonQuickSlotItems( CXSLDefenceDungeonManager::DDP_PHASE_1, kPacket_.m_vecInventorySlotInfo );
    //////////////////////////////////////////////////////////////////////////

	SendPacket( EGS_DEFENCE_DUNGEON_GAME_START_NOT, kPacket_ );

	//{{ 2009. 5. 30  최육사	임시 인벤
	ClearTempInventory();
	//}}

	// 던전이 새로 바뀌었으니 GameStat도 변경!
	//KRoomUserInfo kRoomUserInfo;
	//GetRoomUserInfo( kRoomUserInfo );
	//SendToCnRoom( ERM_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT, kRoomUserInfo );

	// 어둠의 문 전용 버프도 걸자!
	{
		KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;

		if( ActivateBuff( CXSLBuffManager::BTI_BUFF_GATE_DEFENCE, kReq.m_vecActivateBuff ) == true )
		{
			kReq.m_iUnitUID = GetCharUID();
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
	}
}

_IMPL_ON_FUNC( ERM_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT, KEGS_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

    SendPacket( EGS_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT, kPacket_ );
}

IMPL_ON_FUNC( ERM_CHANGE_DEFENCE_DUNGEON_PHASE_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	// 어둠의 문 던전의 페이즈가 변경되었다 해당 페이즈에 맞는 퀵슬롯 아이템으로 변경하자!
	KEGS_UPDATE_INVENTORY_SLOT_INFO_NOT kPacket;
	m_kInventory.SetupDefenceDungeonQuickSlotItems( kPacket_.m_iDefenceDungeonPhase, kPacket.m_vecInventorySlotInfo );
	SendPacket( EGS_UPDATE_INVENTORY_SLOT_INFO_NOT, kPacket );
}
//}}

//{{ 2013. 04. 22	최육사	어둠의 문 입장 로직 변경
IMPL_ON_FUNC( EGS_ENTER_DEFENCE_DUNGEON_POPUP_REPLY_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	//////////////////////////////////////////////////////////////////////////
	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"현재 방에 입장한 상태가 아닙니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
		return;
	}

	if( CXSLRoom::GetRoomType( GetRoomUID() ) != CXSLRoom::RT_DUNGEON )
	{
		START_LOG( cerr, L"현재 던전 방에 입장한 상태가 아닙니다!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
			<< END_LOG;
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	SendToCnRoom( ERM_ENTER_DEFENCE_DUNGEON_POPUP_REPLY_NOT, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( ERM_LEAVE_ROOM_BEFORE_DEFENCE_DUNGEON_START_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	// 파티 소속이라면 자동으로 이탈 처리 하자!

	SendLeaveParty( NetError::NOT_LEAVE_PARTY_REASON_10 );

	// 임시 인벤토리 초기화
	ClearTempInventory();

	// 방 초기화
	SetRoomUID( 0 );

	// 마을로 상태 변경
	StateTransition( KGSFSM::I_TO_FIELD_MAP );

	// 마을로 이동!
	SendID( EGS_LEAVE_ROOM_BEFORE_DEFENCE_DUNGEON_START_NOT );
}
//}}

//{{ 2013. 02. 01  이벤트 발렌타인 던전 - 김민성
//}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
