#include "x2/platform/format.h"
#include "GSUser.h"
#include <algorithm>
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"
#include "KncSend.h"
#include "GSSimLayer.h"

#include "ExpTable.h"
#include "StatTable.h"
#include "BaseServer.h"
#include "X2Data/XSLGame.h"
#include "X2Data/XSLStringFilter.h"
#include "GameEvent/GameEventManager.h"
#include "TutorialManager.h"
//{{ 2008. 11. 24  최육사	랭킹 매니저
#include "GSRankingManager.h"
//}}
//{{ 2008. 11. 24  최육사	방 리스트
#include "RoomListManager.h"
//}}
//{{ 2009. 7. 16  최육사	채널매니저
#include "ChannelManager.h"
//}}

//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
//}}

//{{ 2011. 05. 02  김민성	2차 보안 시스템
#include "./RewardTable.h"
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
#include "GameSysVal/GameSysVal.h"
//}}
//{{ 2011. 07. 27	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
//}}

//{{ 2012. 04. 23	박세훈	어둠의 문 개편
	#include "../Common/X2Data/XSLBuffManager.h"
	#include "GSWorldMissionManager.h"
//}}
//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	#include "SMSPhoneNumberManager.h"
//}}

//{{ 2010. 11. 15  조효진	GameForge 채팅 로그 관련
//}}

//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
//}



	#include "X2Data/XSLBattleFieldManager.h"



#include "ODBC/Odbc.h"
#include "Enum/Enum.h"


#define CLASS_TYPE      KGSUser

// 우상혁 2014-01-07 : 진입구조 개편(캐릭터창) 하면서 클라이언트에서 안보내는 패킷이 되었고,
// 실제로 인벤토리정보(디비)는 다른 곳에서 가져오는 것 같다
IMPL_ON_FUNC_NOPARAM( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ )
{
	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ, EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK );
	//}}

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
	 // 빌드 오류로 해외팀 추가
	/*if( m_kUserInfoByNexon.IsShutdownUser() > KUserInfoByNexon::NONE_AUTH )
	{
		KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK kAck;
		kAck.m_iOK = NetError::ERR_SHUT_DOWN_00;

		SendPacket( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, kAck );

		// 게임 접속 차단
		SetDisconnectReason( KStatistics::eSIColDR_Policy_Shutdown_Disconnect );
		ReserveDestroy();
		return;
	}

	if( m_kUserInfoByNexon.GetPeaceOfMindLogin() == true )
	{
		KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK kAck;
		kAck.m_iOK = NetError::ERR_SHUT_DOWN_01;

		SendPacket( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, kAck );

		// 게임 접속 차단
		SetDisconnectReason( KStatistics::eSIColDR_PeaceOfMindLogin );
		ReserveDestroy();
		return;
	}*/
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	KDBE_GET_SECOND_SECURITY_INFO_REQ kReq;
	kReq.m_iUserUID = GetUID();
	SiKGameSysVal()->GetComeBackRewardCondition( kReq.m_mapComeBackRewardCondition );

	//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
	//}}

	SendToAccountDB( DBE_GET_SECOND_SECURITY_INFO_REQ, kReq );	// AccountDB에 최근 접속 종료 정보 얻기
	//}}

}

_IMPL_ON_FUNC( DBE_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK )
{
	//{{ 2011. 06. 23	최육사	2차 비번 예외처리
	VERIFY_STATE( ( 2, KGSFSM::S_SERVER_SELECT, KGSFSM::S_CHECK_SECOND_PW ) );
	//}}

    if( kPacket_.m_iOK == NetError::NET_OK )
    {
        m_nUnitSlot = kPacket_.m_nUnitSlot;

        // udp 에서 받은 p2p ip/port를 채워보내준다.(임시가 될수있음 차후에 옴겨야 할수도있다.!머리아프네 )
        for( int i = 0; i < (int)kPacket_.m_vecUnitInfo.size(); ++i )
        {
            kPacket_.m_vecUnitInfo[i].m_iOwnerUserUID		= GetUID();
            kPacket_.m_vecUnitInfo[i].m_nStraightVictories	= 0;
            kPacket_.m_vecUnitInfo[i].m_wstrIP				= GetP2PIP();
            kPacket_.m_vecUnitInfo[i].m_usPort				= static_cast<USHORT>(GetP2PPort());
			//{{ 2011. 07. 11	최육사	대전 개편
			//}}

            const CXSLUnit::UnitTemplet* pUnitTemplet		= SiCXSLUnitManager()->GetUnitTemplet( static_cast<CXSLUnit::UNIT_CLASS>(kPacket_.m_vecUnitInfo[i].m_cUnitClass) );
			if( pUnitTemplet == NULL )
			{
				START_LOG_WITH_NAME( cerr )
					<< BUILD_LOGc( kPacket_.m_vecUnitInfo[i].m_cUnitClass )
					<< END_LOG;

#if X2_DEV_TOOLS
				continue;
#endif

				kPacket_.m_iOK = NetError::ERR_CREATE_UNIT_03;
				goto end_proc;

			}

			kPacket_.m_vecUnitInfo[i].m_ucLevel				= SiKExpTable()->CheckLevelUp( 1, kPacket_.m_vecUnitInfo[i].m_iEXP );
			kPacket_.m_vecUnitInfo[i].m_nNowBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)kPacket_.m_vecUnitInfo[i].m_ucLevel );
			kPacket_.m_vecUnitInfo[i].m_nNextBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)(kPacket_.m_vecUnitInfo[i].m_ucLevel + 1) );

			KStatTable::KUnitStatInfo sUnitStat;
			SiKStatTable()->GetUnitStat( kPacket_.m_vecUnitInfo[i].m_cUnitClass, kPacket_.m_vecUnitInfo[i].m_ucLevel, sUnitStat );
			kPacket_.m_vecUnitInfo[i].m_kStat.m_iBaseHP		+= (int)sUnitStat.m_uiHP;
			kPacket_.m_vecUnitInfo[i].m_kStat.m_iAtkPhysic	+= (int)sUnitStat.m_usAtkPhysic;
			kPacket_.m_vecUnitInfo[i].m_kStat.m_iAtkMagic	+= (int)sUnitStat.m_usAtkMagic;
			kPacket_.m_vecUnitInfo[i].m_kStat.m_iDefPhysic	+= (int)sUnitStat.m_usDefPhysic;
			kPacket_.m_vecUnitInfo[i].m_kStat.m_iDefMagic	+= (int)sUnitStat.m_usDefMagic;

			// 보스 필드라면 마을로 이동 시키자
			if( SiCXSLBattleFieldManager()->IsBossFieldID( kPacket_.m_vecUnitInfo[i].m_kLastPos.m_iMapID ) == true )
			{
				kPacket_.m_vecUnitInfo[i].m_kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( kPacket_.m_vecUnitInfo[i].m_ucLevel, kPacket_.m_vecUnitInfo[i].m_mapDungeonClear );
				kPacket_.m_vecUnitInfo[i].m_kLastPos.m_bIgnoreLastTouch = true;
			}

			//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
			// 보유하고 있는 캐릭터 리스트 세팅
			m_setMyUnitUIDList.insert( kPacket_.m_vecUnitInfo[i].m_nUnitUID );
			//}}

			GetPvpRankForClient( kPacket_.m_vecUnitInfo[i].m_iOfficialMatchCnt, kPacket_.m_vecUnitInfo[i].m_cRank );
        }

        START_LOG( clog, L"my unit list에 담겨지는 p2p data" )
            << BUILD_LOG( GetP2PIP() )
            << BUILD_LOG( GetP2PPort() );

        if( ( int )kPacket_.m_vecUnitInfo.size() > m_nUnitSlot )
        {
            START_LOG( cerr, L"캐릭터 슬롯 개수를 초과하는 수의 캐릭터를 보유하고 있음." )
                << BUILD_LOG( ( int )kPacket_.m_vecUnitInfo.size() )
                << BUILD_LOG( m_nUnitSlot )
                << BUILD_LOG( GetUID() )
                << BUILD_LOG( GetName() );
        }
		//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
		// 캐릭터 보여주는 순서 때문에 정렬
		std::stable_sort(kPacket_.m_vecUnitInfo.begin(), kPacket_.m_vecUnitInfo.end(), &LastDateSort );
		//}}

    }

end_proc:


	SendPacket( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_CREATE_UNIT_REQ )
{
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_CREATE_UNIT_REQ, EGS_CREATE_UNIT_ACK );
	//}}

    const CXSLUnit::UnitTemplet* pUnitTemplet = SiCXSLUnitManager()->GetUnitTemplet( (CXSLUnit::UNIT_CLASS)kPacket_.m_iClass );
    KEGS_CREATE_UNIT_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_UNKNOWN;
	char charBuf[255] = {0};
	std::string strNickName;

	//국가별로 서버단에서 캐릭터 막는다.
	const CXSLUnit::UNIT_TYPE eLimitType = static_cast< CXSLUnit::UNIT_TYPE >( _CONST_LIMIT_UNIT_::iLimitType );
	if( eLimitType <  CXSLUnit::GetUnitClassToUnitType( (CXSLUnit::UNIT_CLASS)kPacket_.m_iClass ) )
	{
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_03;
		goto fail_proc;
	}


    if( !pUnitTemplet ) // 070604. florist. templet을 얻어올 필요는 없다. iClass의 유효성 검증 정도의 의미만 가진다.
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_03;
        goto fail_proc;
    }

	if( CXSLUnit::IsInitNormalJob( (CXSLUnit::UNIT_CLASS)kPacket_.m_iClass ) == false )
	{
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_11;
		goto fail_proc;
	}

	WideCharToMultiByte( CP_ACP, 0, kPacket_.m_wstrNickName.c_str(), -1, charBuf, 255, NULL, NULL );
	strNickName = charBuf;

	if( strNickName.empty() || strNickName.size() > 12
		|| strNickName.size() < 2
		)   //캐릭터 닉네임 길이 검사.
	{
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_04;
        goto fail_proc;
	}

	if ( GetAuthLevel() < SEnum::UAL_GM && SiCXSLStringFilter()->CheckIsValidString( CXSLStringFilter::FT_NICKNAME, kPacket_.m_wstrNickName ) == false )
	{
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_08;
		goto fail_proc;
	}

	//{{ 2011. 07. 27	최육사	대전 개편
	if( SiKPvpMatchManager()->IsExistPvpNpcName( kPacket_.m_wstrNickName ) == true )
	{
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_01;
		goto fail_proc;
	}
	//}}

    // 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
    if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
    {
        kPacket.m_iOK = NetError::ERR_ODBC_00;
        goto fail_proc;
    }

    START_LOG_WITH_NAME( clog )
        << BUILD_LOG( kPacket_.m_wstrNickName )
        << BUILD_LOG( kPacket_.m_iClass );


	//{{ 2011. 08. 09  김민성 (2011.08.11) 특정일 이후 생성한 계정에 대하여 신규케릭터 생성 시 아이템 지급 이벤트
	{
		KDBE_GAME_CREATE_UNIT_REQ kReq;

		KAccountInfo AccountTemp;
		GetAccountInfo( AccountTemp );
		kReq.m_wstrRegDate = AccountTemp.m_wstrRegDate;
		kReq.m_wstrNickName = kPacket_.m_wstrNickName;
		kReq.m_iClass = kPacket_.m_iClass;

		SendToGameDB( DBE_GAME_CREATE_UNIT_REQ, kReq );
		return;
	}
		//}}

fail_proc:

    LOG_CONDITION( kPacket.m_iOK == NetError::ERR_CREATE_UNIT_03, cerr, cwarn )
        << BUILD_LOG( GetName() )
        << BUILD_LOG( kPacket.m_iOK )
        << BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
        << BUILD_LOG( kPacket_.m_iClass)
        << BUILD_LOG( kPacket_.m_wstrNickName )
        << END_LOG;

    SendPacket( EGS_CREATE_UNIT_ACK, kPacket );
    return;
}

_IMPL_ON_FUNC( DBE_GAME_CREATE_UNIT_ACK, KEGS_CREATE_UNIT_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		GetPvpRankForClient( kPacket_.m_kUnitInfo.m_iOfficialMatchCnt, kPacket_.m_kUnitInfo.m_cRank );

		SendToAccountDB( DBE_ACCOUNT_CREATE_UNIT_REQ, kPacket_ );

		//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
		// 추천인을 다시 등록하지 않도록 Account DB에 Update
		if( !m_kUserRecommendManager.IsRecommend() )
		{
			SendToAccountDB( DBE_RECOMMEND_USER_NOT );
		}
		//}
		return;
	}

	SendPacket( EGS_CREATE_UNIT_ACK, kPacket_ );
}

_IMPL_ON_FUNC( DBE_ACCOUNT_CREATE_UNIT_ACK, KEGS_CREATE_UNIT_ACK )
{
    VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		//닉네임 중복은 에러가 아니기때문에 체크
		if( kPacket_.m_iOK == NetError::ERR_CREATE_UNIT_01 )
		{
			START_LOG_WITH_NAME( clog )
				<< BUILD_LOG( kPacket_.m_iOK )
				<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ))
				<< END_LOG;
		}
		else
		{
			START_LOG_WITH_NAME( cerr )
				<< BUILD_LOG( kPacket_.m_iOK )
				<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ))
				<< END_LOG;
		}

		SendPacket( EGS_CREATE_UNIT_ACK, kPacket_ );
        return;
	}

    const CXSLUnit::UnitTemplet* pUnitTemplet = SiCXSLUnitManager()->GetUnitTemplet( static_cast<CXSLUnit::UNIT_CLASS>(kPacket_.m_kUnitInfo.m_cUnitClass) );
	if( pUnitTemplet == NULL )
	{
		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOGc( kPacket_.m_kUnitInfo.m_cUnitClass )
			<< END_LOG;

		kPacket_.m_iOK = NetError::ERR_CREATE_UNIT_03;
		SendPacket( EGS_CREATE_UNIT_ACK, kPacket_ );
	}

	kPacket_.m_kUnitInfo.m_ucLevel				= SiKExpTable()->CheckLevelUp( 1, kPacket_.m_kUnitInfo.m_iEXP );
	kPacket_.m_kUnitInfo.m_nNowBaseLevelEXP		= SiKExpTable()->GetRequireTotalExpbyLevel( (int)kPacket_.m_kUnitInfo.m_ucLevel );
	kPacket_.m_kUnitInfo.m_nNextBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)(kPacket_.m_kUnitInfo.m_ucLevel + 1) );

	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	// 보유하고 있는 캐릭터 리스트에 추가
	m_setMyUnitUIDList.insert( kPacket_.m_kUnitInfo.m_nUnitUID );
	//}}

	KStatTable::KUnitStatInfo sUnitStat;
	SiKStatTable()->GetUnitStat( kPacket_.m_kUnitInfo.m_cUnitClass, kPacket_.m_kUnitInfo.m_ucLevel, sUnitStat );
	kPacket_.m_kUnitInfo.m_kStat.m_iBaseHP		+= (int)sUnitStat.m_uiHP;
	kPacket_.m_kUnitInfo.m_kStat.m_iAtkPhysic	+= (int)sUnitStat.m_usAtkPhysic;
	kPacket_.m_kUnitInfo.m_kStat.m_iAtkMagic	+= (int)sUnitStat.m_usAtkMagic;
	kPacket_.m_kUnitInfo.m_kStat.m_iDefPhysic	+= (int)sUnitStat.m_usDefPhysic;
	kPacket_.m_kUnitInfo.m_kStat.m_iDefMagic	+= (int)sUnitStat.m_usDefMagic;
	SendPacket( EGS_CREATE_UNIT_ACK, kPacket_ );

	// 캐릭터가 생성된 경우, Level1의 시작시점을 DB에 남긴다.
	KDBE_CHARACTER_LEVELUP_NOT kNot;
	kNot.m_nCharUID         = kPacket_.m_kUnitInfo.m_nUnitUID;
	kNot.m_ucLevelBefore    = kPacket_.m_kUnitInfo.m_ucLevel;
	kNot.m_ucLevelAfter     = kPacket_.m_kUnitInfo.m_ucLevel;
	SendToLogDB( DBE_CHARACTER_LEVELUP_NOT, kNot );

	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	CTime tCurTime = CTime::GetCurrentTime();
	KELOG_USER_CHARACTER_LOG_NOT kPacketToLog;
	kPacketToLog.m_iUserUID				 = GetUID();
	kPacketToLog.m_iUnitUID				 = kPacket_.m_kUnitInfo.m_nUnitUID;
	kPacketToLog.m_wstrNickName 		 = kPacket_.m_kUnitInfo.m_wstrNickName;
	kPacketToLog.m_bOnlyOneUnit 		 = ( m_setMyUnitUIDList.size() == 1 );
	kPacketToLog.m_iServerUID			 = KBaseServer::GetKObj()->GetUID();
	kPacketToLog.m_sLogType				 = KELOG_USER_CHARACTER_LOG_NOT::LT_CREATE_CHARACTER;
	kPacketToLog.m_ucLevel				 = kPacket_.m_kUnitInfo.m_ucLevel;
	kPacketToLog.m_cUnitClass			 = kPacket_.m_kUnitInfo.m_cUnitClass;
	kPacketToLog.m_wstrRegDate		 	 = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	kPacketToLog.m_wstrUnitCreateDate	 = kPacketToLog.m_wstrRegDate;
	kPacketToLog.m_wstrAccountCreateDate = m_kUserStatistics.GetAccountRegDate();
	SendToLogDB( ELOG_USER_CHARACTER_LOG_NOT, kPacketToLog );
	//}}

	//{{ 2011. 02. 28	최육사	사내 통합 통계
	//}}
}

IMPL_ON_FUNC( EGS_DELETE_UNIT_REQ )
{
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_DELETE_UNIT_REQ, EGS_DELETE_UNIT_ACK );
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	if( IsUseSecurity() == true )
	{
		if( GetSuccessSecondSecurity() == false )	// 2차 보안을 사용 중인데 인증이 되지 않았다면 실패
		{
			START_LOG( cerr, L"2차 보안을 사용 중인데 인증도 되기 전에 유닉 삭제를 하려 한다." )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			KEGS_DELETE_UNIT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_VERIFY_21;
			kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
			kPacket.m_iGuildUID = 0;
			SendPacket( EGS_DELETE_UNIT_ACK, kPacket );
			return;
		}
	}
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	if( m_kUserRelationshipManager.GetRelationshipType() != SEnum::RT_SOLO )
	{
		KEGS_DELETE_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_DELETE_UNIT_06;
		SendPacket( EGS_DELETE_UNIT_ACK, kPacket );
		return;
	}
	//}

	//{{ 2012. 02. 21	김민성	캐릭터 삭제 및 길드 탈퇴 예외처리 수정
	kPacket_.m_iUserUID = GetUID();
	//}}

    SendToGameDB( DBE_GAME_DELETE_UNIT_REQ, kPacket_ );

	m_kUserTutorialInfo.OnDeleteUnitReq( kPacket_.m_iUnitUID );
}

_IMPL_ON_FUNC( DBE_GAME_DELETE_UNIT_ACK, KEGS_DELETE_UNIT_ACK )
{
    VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		SendPacket( EGS_DELETE_UNIT_ACK, kPacket_ );
		return;
	}

	//{{ 2009. 10. 8  최육사	길드
	if( kPacket_.m_iGuildUID > 0 )
	{
		KDBE_KICK_GUILD_MEMBER_ACK kPacket;
		kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
		kPacket.m_iTargetUnitUID = kPacket_.m_iUnitUID;
		SendToLoginServer( ELG_UPDATE_KICK_GUILD_MEMBER_NOT, kPacket );
	}
	//}}

	//{{ 2010. 02. 02  최육사	길드 게시판
	// 길드 소속이 아니라면 길드 가입 신청은 했는지 확인하러 가자!
	if( kPacket_.m_iGuildUID <= 0 )
	{
		KELG_DELETE_APPLY_JOIN_GUILD_REQ kPacketToLg;
		kPacketToLg.m_iDeletedUnitUID = kPacket_.m_iUnitUID;
		kPacketToLg.m_iUnitUID = kPacket_.m_iUnitUID;
		kPacketToLg.m_iGuildUID = 0;
		kPacketToLg.m_bDeleteChar = true;
		SendToLoginServer( ELG_DELETE_APPLY_JOIN_GUILD_REQ, kPacketToLg );
	}

	//}}

	//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
	//}}

	SendPacket( EGS_DELETE_UNIT_ACK, kPacket_ );
}

_IMPL_ON_FUNC( DBE_ACCOUNT_DELETE_UNIT_ACK, KEGS_FINAL_DELETE_UNIT_ACK  )
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	//{{ 2008. 1. 23  최육사  유닛 삭제 통계DB 통보
	if( kPacket_.m_iOK == NetError::NET_OK	&&  ( GetAuthLevel() == SEnum::UAL_NORMAL || GetAuthLevel() == SEnum::UAL_BLOCKED_1 ) )
	{
		KEGS_DELETE_UNIT_REQ kPacketLog;
		kPacketLog.m_iUnitUID = kPacket_.m_iUnitUID;
		SendToLogDB( DBE_DELETE_UNIT_STAT_NOT, kPacketLog );
	}
	//}}

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		// 보유하고 있는 캐릭터 리스트에서 삭제
		m_setMyUnitUIDList.erase( kPacket_.m_iUnitUID );

		CTime tCurTime = CTime::GetCurrentTime();
		KELOG_USER_CHARACTER_LOG_NOT kPacketToLog;
		kPacketToLog.m_iUserUID				= GetUID();
		kPacketToLog.m_iUnitUID				= kPacket_.m_iUnitUID;
		kPacketToLog.m_wstrNickName			= kPacket_.m_wstrNickName;
		kPacketToLog.m_bOnlyOneUnit 		= ( m_setMyUnitUIDList.size() == 1 );
		kPacketToLog.m_iServerUID			= KBaseServer::GetKObj()->GetUID();
		kPacketToLog.m_sLogType				= KELOG_USER_CHARACTER_LOG_NOT::LT_DELETE_CHARACTER;
		kPacketToLog.m_ucLevel				= kPacket_.m_ucLevel;
		kPacketToLog.m_cUnitClass			= kPacket_.m_cUnitClass;
		kPacketToLog.m_wstrRegDate		 	= ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
		kPacketToLog.m_wstrAccountCreateDate = m_kUserStatistics.GetAccountRegDate();
		SendToLogDB( ELOG_USER_CHARACTER_LOG_NOT, kPacketToLog );
		//}}

		m_kUserTutorialInfo.OnDeleteUnitAck( GetThisPtr<KGSUser>() );

		//{{ 2009. 10. 8  최육사	길드
		//}}

		//{{ 2009. 10. 27  최육사	랭킹
		KELG_FIND_DELETE_CHAR_IN_RANKING_NOT kPacketToLg;
		SiKGSRankingManager()->IsInRankingUser( GetCharUID(), kPacketToLg.m_vecRankingType );
		if( !kPacketToLg.m_vecRankingType.empty() )
		{
			kPacketToLg.m_iUnitUID = GetCharUID();
			SendToLoginServer( ELG_FIND_DELETE_CHAR_IN_RANKING_NOT, kPacketToLg );
		}
		//}}

		//{{ 2010. 02. 02  최육사	길드 게시판
		//}}

		//{{ 2011. 02. 28	최육사	사내 통합 통계
		//}}
	}
	SendPacket( EGS_FINAL_DELETE_UNIT_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_SELECT_UNIT_REQ )
{
	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
	VERIFY_STATE_WARN_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_SELECT_UNIT_REQ, EGS_SELECT_UNIT_ACK );
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	if( IsUseSecurity() == true)
	{
		if( GetSuccessSecondSecurity() == false )	// 2차 보안을 사용 중인데 인증이 되지 않았다면 실패
		{
			START_LOG( cerr, L"2차 보안을 사용 중인데 인증도 되기 전에 유닉 선택을 하려 한다." )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			KEGS_SELECT_UNIT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_VERIFY_21;
			SendPacket( EGS_SELECT_UNIT_ACK, kPacket );

			//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
			KEGS_SERVER_CHECK_HACK_USER_REQ kNot;
			kNot.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_SECOND_SECURITY_NOT_AUTH;
			SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kNot );

			m_kUserHackingManager.SetHackingUserSendType( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_SECOND_SECURITY_NOT_AUTH );
			//}}
			return;
		}
	}
	//}}

	IF_EVENT_ENABLED( CEI_OLD_YEAR_EVENT_2013 )
	{
		kPacket_.m_setCodeEventScriptID.insert( CEI_OLD_YEAR_EVENT_2013 );
	}
	IF_EVENT_ENABLED( CEI_NEW_YEAR_EVENT_2014 )
	{
		kPacket_.m_setCodeEventScriptID.insert( CEI_NEW_YEAR_EVENT_2014 );
	}




	SendToGameDB( DBE_GAME_SELECT_UNIT_REQ, kPacket_ );
}

//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
_IMPL_ON_FUNC( DBE_GAME_SELECT_UNIT_ACK, KDBE_SELECT_UNIT_ACK )
//}}
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

    if( kPacket_.m_iOK != NetError::NET_OK )
    {
		//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
		if( kPacket_.m_iOK == NetError::ERR_SELECT_UNIT_07 )
		{
			START_LOG( cout, L"UserUID소속이 아닌 UnitUID로 캐릭터를 선택한 유저가 나타났다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() );

			KDBE_SEND_PHONE_MSG_NOT kPacketNot;
			kPacketNot.m_wstrSMSMessage = x2::str( x2::wformat( L"계정소속 아닌 캐릭터선택:UserUID(%d),UnitUID(%d) " ) // ,ServerIP(%s)" ) // 해외팀 주석 처리
                % GetUID()
                % kPacket_.m_nWrongUnitUID
                //% KBaseServer::GetKObj()->GetPublicIP() // 해외팀 주석 처리
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

		//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
		KEGS_SELECT_UNIT_ACK kACK;
		kACK.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_SELECT_UNIT_ACK, kACK );

        LOG_SUCCESS( kACK.m_iOK == NetError::NET_OK )
            << BUILD_LOG( GetUID() )
            << BUILD_LOG( kACK.m_iOK );

		//}}
        return;
    }

    // 2007.12. 28. jseop. 닉네임이 잘못되어 변경해야 하는 경우.
    if( kPacket_.m_kUnitInfo.m_wstrNickName.compare( L"__DELETED__" ) == 0 )
    {
        KEGS_SELECT_UNIT_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_SELECT_UNIT_06;
        SendPacket( EGS_SELECT_UNIT_ACK, kPacket );
        return;
    }





	// 보스 필드라면 마을로 이동 시키자
	if( SiCXSLBattleFieldManager()->IsBossFieldID( kPacket_.m_kUnitInfo.m_kLastPos.m_iMapID ) == true )
	{
		kPacket_.m_kUnitInfo.m_kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( kPacket_.m_kUnitInfo.m_ucLevel, kPacket_.m_kUnitInfo.m_mapDungeonClear );
		kPacket_.m_kUnitInfo.m_kLastPos.m_bIgnoreLastTouch = true;
	}
	// 최초 선택이나, 채널 변경에서 동일하게 처리되는 구문은
	// 함수로 처리해서 공용으로 처리되도록 하는게 좋을 듯 하다...


	SendToAccountDB( DBE_ACCOUNT_SELECT_UNIT_REQ, kPacket_ );
}

//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
_IMPL_ON_FUNC( DBE_ACCOUNT_SELECT_UNIT_ACK, KDBE_SELECT_UNIT_ACK )
//}}
{
    VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
    {
		//{{ 2009. 5. 13  최육사	P2P포트 체크
		if( GetP2PIP().empty() )
		{
			START_LOG( cerr, L"P2P포트 체크 실패!" )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;
		}
		//}}

		CTime tReleaseTradeBlockTime;
		LIF( KncUtil::ConvertStringToCTime( kPacket_.m_wstrUnitCreateDate, tReleaseTradeBlockTime ) );
		tReleaseTradeBlockTime += CTimeSpan( SiKGameSysVal()->GetNewUnitTradeBlockDay(), 0, 0, 0 );
		CTime tCurTime = CTime::GetCurrentTime();

		if( SiKGameSysVal()->IsNewUnitTradeBlock() == true
			&& ( ( SiKGameSysVal()->GetNewUnitTradeBlockDay() != 0 && tCurTime < tReleaseTradeBlockTime )
			|| ( SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() != 0 && kPacket_.m_kUnitInfo.m_cUnitClass < SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() ) )
			)
		{
			m_tReleaseTradeBlockTime = tReleaseTradeBlockTime;

			m_kUserAbuserManager.SetTradeBlockUnit( true );
		}
		else
		{
			m_kUserAbuserManager.SetTradeBlockUnit( false );
		}

		// 채널 이동 예약 검사
		if( SiKChannelManager()->IsExistChannelChangeUser( kPacket_.m_kUnitInfo.m_nUnitUID ) == true )
		{
			// 해당 유저가 접속할 수 있도록 예약정보 제거
			SiKChannelManager()->DelChannelChangeUser( kPacket_.m_kUnitInfo.m_nUnitUID );
		}


		//////////////////////////////////////////////////////////////////////////
		// 캐릭터 선택에 관련된 모든 처리는 이 함수 안에서 하도록 한다!
		if( OnAccountSelectUnitAck( kPacket_, false ) == false )
		{
			START_LOG( cerr, L"select unit error" )
				<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
				<< END_LOG;

			goto end_proc;
		}
		//////////////////////////////////////////////////////////////////////////

		//{{ 2012. 09. 12	박세훈	2012 대전 시즌2
		GetPvpRankForClient( kPacket_.m_kUnitInfo.m_iOfficialMatchCnt, kPacket_.m_kUnitInfo.m_cRank );
		//}}


	}

end_proc:

	//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
	SendSelectUnitACK( kPacket_ );
	//}}

    if( kPacket_.m_iOK == NetError::NET_OK )
    {
		//////////////////////////////////////////////////////////////////////////
		// 캐릭터 선택 성공한 이후에 처리하는 것들은 모두 이 함수 안에서 처리하도록 한다!
		OnSelectUnitSuccess( kPacket_, false );
		//////////////////////////////////////////////////////////////////////////
    }
}

//{{ 2010. 01. 04  최육사	인벤토리 개선
IMPL_ON_FUNC_NOPARAM( EGS_GET_MY_INVENTORY_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	std::vector< KEGS_SELECT_UNIT_INVENTORY_INFO_NOT > vecInventoryInfo;

	// 슬롯별로 인벤토리 정보 얻기
	m_kInventory.GetSelectUnitInventoryInfo( vecInventoryInfo );

	for( u_int ui = 0; ui < vecInventoryInfo.size(); ++ui )
	{
		SendPacket( EGS_SELECT_UNIT_INVENTORY_INFO_NOT, vecInventoryInfo[ui] );
	}

	START_LOG( clog, L"인벤토리 정보 분할 전송" )
		<< BUILD_LOG( GetCharUID() )
		<< BUILD_LOG( vecInventoryInfo.size() );

    KEGS_GET_MY_INVENTORY_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	//{{ 2010. 01. 08  최육사	추천인리스트
	kPacket.m_bIsRecommend = m_kUserRecommendManager.IsRecommend();
	//}}

	SendPacket( EGS_GET_MY_INVENTORY_ACK, kPacket );
}
//}}

_IMPL_ON_FUNC( EGS_ENTER_EQUIP_SHOP_REQ, KEGS_ENTER_NPC_HOUSE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KPacketOK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	SendPacket( EGS_ENTER_EQUIP_SHOP_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_ENTER_NPC_HOUSE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KPacketOK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	SendPacket( EGS_ENTER_NPC_HOUSE_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_CHAT_REQ )
{
	KEGS_CHAT_ACK kPacket;


	if( kPacket_.m_ToUnitUID == GetCharUID() )
	{
		kPacket.m_iOK = NetError::ERR_CHAT_02;
		SendPacket( EGS_CHAT_ACK, kPacket );
		return;
	}



	switch( kPacket_.m_cChatPacketType )
	{
	case KEGS_CHAT_REQ::CPT_TOTAL:
		{
			//{{ 2008. 12. 13  최육사	채팅
			if( GetStateID() == KGSFSM::S_ROOM )
			{
				SendToCnRoom( ERM_CHAT_REQ, kPacket_ );

				//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
				//}}
			}
			else
			{
				if( GetFieldUID() > 0 )
				{
					SendToGSField( ERM_CHAT_REQ, kPacket_ );

					//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
					//}}
				}
				else
				{
					START_LOG( cwarn, L"필드 상태인데 fieldUID가 0인경우?" )
						<< BUILD_LOG( GetCharUID() )
						<< END_LOG;

					// 일단 ok를 보낸다.
					kPacket.m_iOK = NetError::NET_OK;
					SendPacket( EGS_CHAT_ACK, kPacket );
				}
			}
			//}}
			return;
		}
		break;

	case KEGS_CHAT_REQ::CPT_PARTY:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

			if( GetPartyUID() == 0 )
			{
				START_LOG( cerr, L"파티 UID 이상.!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< END_LOG;

				kPacket.m_iOK = NetError::ERR_PARTY_09;
				SendPacket( EGS_CHAT_ACK, kPacket );
				return;
			}

			//{{ 2010. 02. 23  최육사	채널 파티 통합
			if( SiKPartyListManager()->E_Chat( GetThisPtr<KGSUser>(), kPacket_ ) == false )
			//}}
			{
				START_LOG( cwarn, L"파티채팅실패." )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetPartyUID() )
					<< END_LOG;
			}
			return;
		}
		break;

	case KEGS_CHAT_REQ::CPT_WHISPER:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

			KELG_CHAT_REQ kReq;
			kReq.m_cChatPacketType			= kPacket_.m_cChatPacketType;
			kReq.m_iSenderUnitUID			= GetCharUID();
			kReq.m_wstrSenderUnitNickName	= GetCharName();
			kReq.m_wstrRecieverUnitNickName	= kPacket_.m_wstrRecieverUnitNickName;
			kReq.m_wstrMsg					= kPacket_.m_wstrMsg;

			SendToLoginServer( ELG_CHAT_REQ, kReq );
			return;
		}
		break;

    case KEGS_CHAT_REQ::CPT_MESSENGER:
        {
            KELG_CHAT_REQ kReq;
            kReq.m_cChatPacketType			= kPacket_.m_cChatPacketType;
            kReq.m_iSenderUnitUID			= GetCharUID();
            kReq.m_wstrSenderUnitNickName	= GetCharName();
            kReq.m_wstrRecieverUnitNickName	= kPacket_.m_wstrRecieverUnitNickName;
            kReq.m_wstrMsg					= kPacket_.m_wstrMsg;

            SendToLoginServer( ELG_CHAT_REQ, kReq );
            return;
        }
        break;

		//{{ 2009. 9. 18  최육사	길드
	case KEGS_CHAT_REQ::CPT_GUILD:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

			if( GetGuildUID() <= 0 )
			{
				START_LOG( cerr, L"길드멤버가 아닌데 길드채팅을 날린다." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetGuildUID() )
					<< END_LOG;

				kPacket.m_iOK = NetError::ERR_CHAT_03;
				SendPacket( EGS_CHAT_ACK, kPacket );
				return;
			}

			KELG_CHAT_REQ kReq;
			kReq.m_cChatPacketType			= kPacket_.m_cChatPacketType;
			kReq.m_iChatData				= GetGuildUID();
			kReq.m_iSenderUnitUID			= GetCharUID();
			kReq.m_wstrSenderUnitNickName	= GetCharName();
			kReq.m_wstrMsg					= kPacket_.m_wstrMsg;

			SendToLoginServer( ELG_CHAT_REQ, kReq );
			return;
		}
		break;
		//}}
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
	case KEGS_CHAT_REQ::CPT_RELATIONSHIP:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

			KELG_CHAT_REQ kReq;
			kReq.m_cChatPacketType			= kPacket_.m_cChatPacketType;
			kReq.m_iSenderUnitUID			= GetCharUID();
			kReq.m_wstrSenderUnitNickName	= GetCharName();
			kReq.m_wstrRecieverUnitNickName	= kPacket_.m_wstrRecieverUnitNickName;
			kReq.m_wstrMsg					= kPacket_.m_wstrMsg;

			SendToLoginServer( ELG_CHAT_REQ, kReq );
			return;
		}
		break;
		//}
	default:
		{
			START_LOG( cerr, L"이상한 채팅 패킷 타입." )
				<< BUILD_LOGc( kPacket_.m_cChatPacketType )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_CHAT_03;
			SendPacket( EGS_CHAT_ACK, kPacket );
			return;
		}
		break;
	}
}

IMPL_ON_FUNC( ERM_CHAT_ACK )
{
	switch( kPacket_.m_cChatPacketType )
	{
	case KEGS_CHAT_REQ::CPT_TOTAL:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
		}
		break;
	default:
		{
			START_LOG( cerr, L"이상한 채팅 패킷 타입." )
				<< BUILD_LOGc( kPacket_.m_cChatPacketType )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;
		}
		break;
	}

	KEGS_CHAT_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;
	SendPacket( EGS_CHAT_ACK, kPacket );
}

_IMPL_ON_FUNC( ERM_CHAT_NOT, KEGS_CHAT_NOT )
{
	switch( kPacket_.m_cChatPacketType )
	{
	case KEGS_CHAT_REQ::CPT_TOTAL:
		{
			VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
		}
		break;
	default:
		{
			START_LOG( cerr, L"이상한 채팅 패킷 타입." )
				<< BUILD_LOGc( kPacket_.m_cChatPacketType )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;
		}
		break;
	}

	// 블랙 리스트 채팅 차단
	if( m_kUserGameOption.IsChatBlackListUnit( kPacket_.m_SenderUnitUID ) == true )
		return;

	SendPacket( EGS_CHAT_NOT, kPacket_ );
}

IMPL_ON_FUNC( ELG_CHAT_ACK )
{
	VERIFY_STATE( ( 2,	KGSFSM::S_FIELD_MAP,
						KGSFSM::S_ROOM  ) );

	KEGS_CHAT_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	SendPacket( EGS_CHAT_ACK, kPacket );
}

IMPL_ON_FUNC( ELG_CHAT_NOT )
{
	VERIFY_STATE( ( 2,	KGSFSM::S_FIELD_MAP,
						KGSFSM::S_ROOM  ) );

	// 블랙 리스트 채팅 차단
	if( m_kUserGameOption.IsChatBlackListUnit( kPacket_.m_iSenderUnitUID ) == true )
		return;

	KEGS_CHAT_NOT kPacket;

	kPacket.m_cChatPacketType			= kPacket_.m_cChatPacketType;
	kPacket.m_SenderUnitUID				= kPacket_.m_iSenderUnitUID;
	kPacket.m_wstrSenderUnitNickName	= kPacket_.m_wstrSenderUnitNickName;
	kPacket.m_wstrRecieverUnitNickName	= kPacket_.m_wstrRecieverUnitNickName;
	kPacket.m_RecieverUnitUID			= kPacket_.m_iReceiverUnitUID;
	kPacket.m_cRoomChatType				= 0;
	kPacket.m_wstrMsg					= kPacket_.m_wstrMsg;


	SendPacket( EGS_CHAT_NOT, kPacket );
}

IMPL_ON_FUNC( EGS_CHANGE_OPTION_PLAY_GUIDE_REQ )
{
    // TODO : 상태 제한 해야 한다.

    SendToAccountDB( EGS_CHANGE_OPTION_PLAY_GUIDE_REQ, kPacket_ );
}

_IMPL_ON_FUNC( EGS_CHANGE_OPTION_PLAY_GUIDE_ACK, KPacketOK )
{
    // TODO : 상태 제한 해야 한다.

    SendPacket( EGS_CHANGE_OPTION_PLAY_GUIDE_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_NEW_QUEST_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_NEW_QUEST_REQ, EGS_NEW_QUEST_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		KEGS_NEW_QUEST_ACK kAck;
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_NEW_QUEST_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	//에러 체크는 Handler_EGS_NEW_QUEST_REQ()안에서 한다.
	//{{ QUEST 개편 - 김민성
	m_kUserQuestManager.Handler_EGS_NEW_QUEST_REQ( kPacket_, GetThisPtr<KGSUser>(), false );
	//}}
}

IMPL_ON_FUNC( DBE_NEW_QUEST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 자동수락 퀘스트 이면 ACK를 보내지 않는다.(현재는 에픽퀘스트만..)
	if( SiCXSLQuestManager()->IsEpicQuest(kPacket_.m_iQuestID) == false )
	{
		KEGS_NEW_QUEST_ACK kAck;
		kAck.m_iOK	= kPacket_.m_iOK;
		SendPacket( EGS_NEW_QUEST_ACK, kAck );
	}


	//DB에서 정상처리되고 온 경우..
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KQuestInstance kQuest;
		kQuest.m_iID			= kPacket_.m_iQuestID;
		kQuest.m_OwnorUnitUID	= GetCharUID();

		const CXSLQuestManager::QuestTemplet* QuestTem = SiCXSLQuestManager()->GetQuestTemplet( kPacket_.m_iQuestID );
		if( QuestTem == NULL )
		{
			START_LOG( cerr, L"QUEST TEMPLET 얻어오기 실패.!" )
				<< BUILD_LOG( kPacket_.m_iQuestID )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
			return;
		}

		KSubQuestInstance kSubQuest;
		for( const int iSubQuestID : QuestTem->m_vecSubQuest )
		{
			kSubQuest.m_iID = iSubQuestID;

			//{{ 2012. 09. 18	박세훈	2012 대전 시즌2
			// 누적된 공식 대전 플레이 카운트가 표시되어야 하기 때문에,
			// 이곳에서 예외 처리를 통해 현재까지의 플레이 횟수를 집어 넣어준다.
			const CXSLQuestManager::SubQuestTemplet* pSubQuestTemplet = SiCXSLQuestManager()->GetSubQuestTemplet( iSubQuestID );
			if( pSubQuestTemplet == NULL )
			{
				START_LOG( cerr, L"SUB QUEST TEMPLET 얻어오기 실패.!" )
					<< BUILD_LOG( iSubQuestID )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< END_LOG;
				continue;
			}

			if( pSubQuestTemplet->m_eClearType == CXSLQuestManager::SQT_PVP_PLAY_ARRANGE )	//클리어 조건이 같은지..
			{
				// pvp info 테이블의 정보를 덮어 씌우기만 한다.
				kSubQuest.m_ucClearData = static_cast<UCHAR>(GetOfficialMatchCount());
				kSubQuest.m_bIsSuccess = ( pSubQuestTemplet->m_ClearCondition.m_iPVPPlay <= kSubQuest.m_ucClearData );
			}
			//}}

			kQuest.m_vecSubQuestInstance.push_back( kSubQuest );
		}

		m_kUserQuestManager.AddQuest( kQuest );

		KEGS_NEW_QUEST_NOT kNot;
		kNot.m_QuestInst = kQuest;
		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
		m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kNot.m_vecUpdatedInventorySlot );
		kNot.m_vecUpdatedInventorySlot.insert( kNot.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );
		//}}
		SendPacket( EGS_NEW_QUEST_NOT, kNot );

		//{{ QUEST 개편 - 김민성
		SendUpdateDropQuestItemByIngQuest();
		//}}

		//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
		for( const int iSubQuestID : QuestTem->m_vecSubQuest )
		{
			const CXSLQuestManager::SubQuestTemplet* pSubQuestTemplet = SiCXSLQuestManager()->GetSubQuestTemplet( iSubQuestID );
			if( IS_NULL( pSubQuestTemplet ) )
			{
				START_LOG( cerr, L"존재 하지 않는 서브 퀘스트 입니다!" )
					<< BUILD_LOG( iSubQuestID )
					<< END_LOG;
				continue;
			}

			switch( pSubQuestTemplet->m_eClearType )
			{
			case CXSLQuestManager::SQT_VISIT_VILLAGE:
				{
					// 필드 입장에 대한 퀘스트 완료 체크!
					m_kUserQuestManager.Handler_OnEnterTheVillage( GetThisPtr<KGSUser>() );
				}
				break;

			case CXSLQuestManager::SQT_VISIT_FIELD:
				{
					// 배틀필드 입장에 대한 퀘스트 완료 체크!
					m_kUserQuestManager.Handler_OnEnterTheBattleField( GetThisPtr<KGSUser>() );
				}
				break;

				//{{ QUEST 개편 - 김민성
			case CXSLQuestManager::SQT_VISIT_DUNGEON:
				{
					// 배틀필드 입장에 대한 퀘스트 완료 체크!
					m_kUserQuestManager.Handler_OnEnterTheDungeon( GetThisPtr<KGSUser>() );
				}
				break;
				//}}
			default:
				break;
			}
		}
		//}}

		//{{ 2012. 09. 18	박세훈	2012 대전 시즌2
		m_kUserQuestManager.Handler_OnPVPPlay_AccumulatedOfficialMatchCount( GetThisPtr<KGSUser>(), false );
		//}}
	}
}

IMPL_ON_FUNC( EGS_TALK_WITH_NPC_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_TALK_WITH_NPC_ACK	kPacket;

	//{{ 2009. 10. 15  최육사	거래 예외처리
	if( GetTradeUID() > 0  ||  GetPersonalShopUID() > 0 )
	{
		START_LOG( cwarn, L"거래중에는 사용할 수 없는 기능!" )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacket.m_iOK	= NetError::ERR_ITEM_14;
		SendPacket( EGS_TALK_WITH_NPC_ACK, kPacket );
		return;
	}
	//}}

	kPacket.m_iOK	= NetError::NET_OK;
	SendPacket( EGS_TALK_WITH_NPC_ACK, kPacket );

	m_kUserQuestManager.Handler_EGS_TALK_WITH_NPC_REQ( kPacket_.m_iNPCID, GetThisPtr<KGSUser>() );

	//{{ 2008. 10. 10  최육사	타이틀
	m_kUserTitleManager.OnTalkWithNpc( kPacket_.m_iNPCID, GetThisPtr<KGSUser>() );
	//}}
}

IMPL_ON_FUNC( EGS_GIVE_UP_QUEST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	m_kUserQuestManager.Handler_EGS_GIVE_UP_QUEST_REQ( kPacket_, GetThisPtr<KGSUser>() );
}

IMPL_ON_FUNC( EGS_GATHER_GIVE_UP_QUEST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	for( const int iQuestID : kPacket_.m_vecGatherQuestID )
	{
		KEGS_GIVE_UP_QUEST_REQ kPacket;
		kPacket.m_iQuestID = iQuestID;
		m_kUserQuestManager.Handler_EGS_GIVE_UP_QUEST_REQ( kPacket, GetThisPtr<KGSUser>() );
	}
}

IMPL_ON_FUNC( DBE_GIVE_UP_QUEST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_GIVE_UP_QUEST_ACK kAck;
	kAck.m_iOK		= kPacket_.m_iOK;
	kAck.m_iQuestID	= kPacket_.m_iQuestID;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		if( m_kUserQuestManager.RemoveQuest( kPacket_.m_iQuestID ) == false )
		{
			kAck.m_iOK = NetError::GetLastError();
		}
		else
		{
			SendUpdateDropQuestItemByIngQuest();
		}

		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		const CXSLQuestManager::QuestTemplet* pQuestTemplet = SiCXSLQuestManager()->GetQuestTemplet( kPacket_.m_iQuestID );
		if( pQuestTemplet == NULL )
		{
			START_LOG( cerr, L"퀘스트 정보가 없습니다!" )
				<< BUILD_LOG( kPacket_.m_iQuestID )
				<< END_LOG;
		}
		else
		{
			std::map< int, int >::const_iterator mit;
			for( mit = pQuestTemplet->m_mapAcceptRewardItem.begin(); mit != pQuestTemplet->m_mapAcceptRewardItem.end(); ++mit )
			{
				std::vector< KInventoryItemInfo > vecInventorySlotInfo;
				m_kInventory.DeleteItemAll( mit->first, vecInventorySlotInfo );
				kAck.m_vecUpdatedInventorySlot.insert( kAck.m_vecUpdatedInventorySlot.end(), vecInventorySlotInfo.begin(), vecInventorySlotInfo.end() );
			}
		}
		//}}
	}

	SendPacket( EGS_GIVE_UP_QUEST_ACK, kAck );
}

//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
IMPL_ON_FUNC( EGS_ALL_COMPLETED_QUEST_COMPLETE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_ALL_COMPLETED_QUEST_COMPLETE_REQ, EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK kPacketAck;
			kPacketAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, kPacketAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	// 퀘스트 정보 저장!
	m_kUserQuestManager.SetAutoCompleteQuest( kPacket_ );

	// 첫번째 퀘스트 동작 시작!
	KEGS_QUEST_COMPLETE_REQ kReq;
	if( m_kUserQuestManager.GetAndDeleteAutoCompleteQuest( kReq ) == false )
	{
		START_LOG( cerr, L"완료할 퀘스트가 없는데 여긴 왜 왔나?" )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_UNKNOWN;
		SendPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, kPacketAck );
		return;
	}

	// 퀘스트 완료 동작 시작!
	m_kUserQuestManager.Handler_EGS_QUEST_COMPLETE_REQ( kReq, GetThisPtr<KGSUser>() );
}
//}}

IMPL_ON_FUNC( EGS_QUEST_COMPLETE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_QUEST_COMPLETE_REQ, EGS_QUEST_COMPLETE_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_QUEST_COMPLETE_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_QUEST_COMPLETE_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////
	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	if( SiCXSLQuestManager()->IsAccountQuest( kPacket_.m_iQuestID ) == true )
	{
		m_kUserAccountQuestManager.Handler_EGS_QUEST_COMPLETE_REQ_Account( kPacket_, GetThisPtr<KGSUser>() );
		return;
	}
	//}}

	m_kUserQuestManager.Handler_EGS_QUEST_COMPLETE_REQ( kPacket_, GetThisPtr<KGSUser>() );
}

IMPL_ON_FUNC( DBE_QUEST_COMPLETE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
		if( kPacket_.m_bAutoComplete )
		{
			// 초기화
			m_kUserQuestManager.ClearAutoCompleteQuest();

			KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK kPacket;
			kPacket.m_iOK = kPacket_.m_iOK;
			SendPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, kPacket );
		}
		else
		//}}
		{
			KEGS_QUEST_COMPLETE_ACK	kPacket;
			kPacket.m_iOK		= kPacket_.m_iOK;
			//{{ 2010. 04. 02  최육사	일일퀘스트
			//#ifdef SERV_DAILY_QUEST
			kPacket.m_kCompleteQuestInfo = kPacket_.m_kCompleteQuestInfo;
			//#else
			//	//kPacket.m_iQuestID	= kPacket_.m_iQuestID;
			//#endif SERV_DAILY_QUEST
			//}}
			SendPacket( EGS_QUEST_COMPLETE_ACK, kPacket );
		}
		return;
	}

	//db에서 완료체크를 받고 난다음
	m_kUserQuestManager.Handler_DBE_QUEST_COMPLETE_ACK( kPacket_, GetThisPtr<KGSUser>() );
	//- 보상으로 얻은 유정 정보 client에게 알려주기 유닛 업데이트를 시켜야 하나 말아야 하나 흠~~

	//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
	if( kPacket_.m_bAutoComplete )
	{
		// 그다음 동작 퀘스트!
		KEGS_QUEST_COMPLETE_REQ kReq;
		if( m_kUserQuestManager.GetAndDeleteAutoCompleteQuest( kReq ) == true )
		{
			// 퀘스트 완료 동작 시작!
			m_kUserQuestManager.Handler_EGS_QUEST_COMPLETE_REQ( kReq, GetThisPtr<KGSUser>() );
		}
		else
		{
			// 모든 퀘스트가 비었다면!
			KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK kPacketAck;
			kPacketAck.m_iOK = NetError::NET_OK;
			m_kUserQuestManager.GetAutoCompleteQuestResult( kPacketAck );
			SendPacket( EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, kPacketAck );
		}
	}
	//}}

	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	// 첫번째 퀘스트 클리어 인지 체크!
	if( m_kUserQuestManager.GetCompleteQuestSize() == 1 )
	{
		WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_QUEST_COMPLETE );
	}
	//}}


	IF_EVENT_ENABLED( CEI_BALANCE_FINALITY_SKILL_EVENT )
	{
		if( kPacket_.m_kCompleteQuestInfo.m_iQuestID == 35370 )
		{
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = 2048;	// 카밀라의 비전서 (초급)
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
	}

	if( kPacket_.m_bIsChangeJob == true )
	{
		IF_EVENT_ENABLED( CEI_ARA_FIRST_CLASS_CHANGE_EVENT )
		{
			if( CXSLUnit::UC_ARA_LITTLE_HSIEN == GetUnitClass() )
			{
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = 1553;	// 소선을 위한 특별 선물 큐브A
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}
		}

		IF_EVENT_ENABLED( CEI_ARA_NEW_FIRST_CLASS_EVENT )
		{
			if( CXSLUnit::UC_ARA_LITTLE_DEVIL == GetUnitClass() )
			{
				// 이벤트 보상을 주자!
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iClassChangeQuestReward;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}
		}

		IF_EVENT_ENABLED( CEI_ARA_NEW_SECOND_CLASS_EVENT )
		{
			if( CXSLUnit::UC_ARA_YAMA_RAJA == GetUnitClass() )
			{
				// 이벤트 보상을 주자!
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_SECOND_CLASS_EVENT_REWARD_ID::iAraNewSecondClassChangeReward;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}

			if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true )
			{
				// 이벤트 보상을 주자!
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_SECOND_CLASS_EVENT_REWARD_ID::iSecondClassChangeReward;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}
		}



		KDBE_UPDATE_UNIT_CLASS_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		kPacketNot.m_cUnitClass = GetUnitClass();

		SendToLogDB( DBE_UPDATE_UNIT_CLASS_NOT, kPacketNot );

		m_kSkillTree.SetUnitClass( (int) GetUnitClass() );


		// kimhc // 김현철 // 전직 퀘스트 완료 시, 모든 스킬 페이지에 적용 되도록 함
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iNewDefaultSkill1, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iNewDefaultSkill2, 1, 0 );

		KEGS_CHANGE_MY_UNIT_INFO_NOT kMyInfoNot;
		kMyInfoNot.m_cUnitClass = GetUnitClass();
		kMyInfoNot.m_iNewDefaultSkill_1 = kPacket_.m_iNewDefaultSkill1;
		kMyInfoNot.m_iNewDefaultSkill_2 = kPacket_.m_iNewDefaultSkill2;
		SendPacket( EGS_CHANGE_MY_UNIT_INFO_NOT, kMyInfoNot );

		if( GetStateID() == KGSFSM::S_FIELD_MAP )
		{
			// 필드에 있을경우 유저 정보 갱신하기
			if( GetFieldUID() > 0 )
			{
				KERM_CHANGE_FIELD_USER_INFO_NOT kNot;
				GetFieldUserInfo( kNot.m_kInfo );
				SendToGSField( ERM_CHANGE_FIELD_USER_INFO_NOT, kNot );
			}
		}
		else if( GetStateID() == KGSFSM::S_ROOM )
		{
			switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
			{
			case CXSLRoom::RT_PVP:
			case CXSLRoom::RT_DUNGEON:
			case CXSLRoom::RT_TRAININGCENTER:
				//{{ 배틀필드 : 필드 시스템
			case CXSLRoom::RT_BATTLE_FIELD:
				//}}
				{
					KRoomUserInfo kRoomUserInfo;
					GetRoomUserInfo( kRoomUserInfo );
					SendToCnRoom( ERM_CHANGE_ROOM_USER_INFO_NOT, kRoomUserInfo );
				}
				break;

			case CXSLRoom::RT_SQUARE:
				{
					KSquareUserInfo kSquareUserInfo;
					GetSquareUserInfo( kSquareUserInfo );
					SendToCnRoom( ERM_CHANGE_SQUARE_USER_INFO_NOT, kSquareUserInfo );
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

		//{{ 2009. 10. 8  최육사	길드
		SendUpdateGuildMemberInfo();
		//}}

		//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
		//}}
	}

	//{{ 2008. 10. 10  최육사	타이틀
	//{{ 2010. 04. 02  최육사	일일퀘스트
	m_kUserTitleManager.OnQuestComplete( kPacket_.m_kCompleteQuestInfo.m_iQuestID, GetThisPtr<KGSUser>() );
	//}}
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

	// 일일통계 퀘스트 습득 아이템
	std::map< int, KItemInfo >::iterator mit = kPacket_.m_mapInsertedItem.begin();
	for( ; mit != kPacket_.m_mapInsertedItem.end(); ++mit )
	{
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( mit->first );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_QUEST, mit->second.m_iQuantity );
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_QUEST, mit->second.m_iQuantity );
		//}}
	}
}

_IMPL_ON_FUNC( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT )
{
	VERIFY_STATE_LOG( clog2, ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	SendPacket( EGS_NOTIFY_MSG_NOT, kPacket_ );
	//}}
	//}}
}
//}}

IMPL_ON_FUNC( EGS_SEARCH_UNIT_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	//자기 자신의 정보를 요청한 경우.

	if( x2::platform::IEquals(GetCharName(), kPacket_) )
	{
		KEGS_SEARCH_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		GetConnectionUnitInfo( kPacket.m_kCUnitInfo );

		SendPacket( EGS_SEARCH_UNIT_ACK, kPacket );
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_ ) )
	{
		KEGS_SEARCH_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_SEARCH_UNIT_ACK, kPacket );
		return;
	}

	if( m_kTimer[TM_SEARCH_UNIT_REFRESH].elapsed() < (double)SEARCH_UNIT_REFRESH_TIME_LIMIT)
	{
		KEGS_SEARCH_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_01;
		SendPacket( EGS_SEARCH_UNIT_ACK, kPacket );
		return;
	}
	m_kTimer[TM_SEARCH_UNIT_REFRESH].restart();

	KELG_SEARCH_UNIT_REQ kPacketReq;
	kPacketReq.m_usEventID = EGS_SEARCH_UNIT_REQ;
	kPacketReq.m_wstrNickName = kPacket_;
	SendToLoginServer( ELG_SEARCH_UNIT_REQ, kPacketReq );
}

//{{ 2009. 4. 17  최육사	접속중인 유저찾기
IMPL_ON_FUNC( EGS_GET_CONNECTION_UNIT_INFO_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	//자기 자신의 정보를 요청한 경우.
	if( x2::platform::IEquals(GetCharName(), kPacket_) )
	{
		KEGS_GET_CONNECTION_UNIT_INFO_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		GetConnectionUnitInfo( kPacket.m_kCUnitInfo );

		SendPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK, kPacket );
		return;
	}

	// [참고] DB쿼리에 들어가는 문자열은 아니지만 혹시나 해서 검사해봄.
	if( KODBC::IsInvalidMarkIn( kPacket_ ) )
	{
		KEGS_GET_CONNECTION_UNIT_INFO_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK, kPacket );
		return;
	}

	if( m_kTimer[TM_SEARCH_UNIT_REFRESH].elapsed() < (double)SEARCH_UNIT_REFRESH_TIME_LIMIT)
	{
		KEGS_GET_CONNECTION_UNIT_INFO_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_01;
		SendPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK, kPacket );
		return;
	}
	m_kTimer[TM_SEARCH_UNIT_REFRESH].restart();

	//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
	if( GetStateID() == KGSFSM::S_ROOM  &&  GetRoomUID() > 0 )
	{
		if( CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_DUNGEON  ||
			CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_PVP )
		{
			KELG_SEARCH_UNIT_REQ kPacketReq;
			kPacketReq.m_usEventID = EGS_GET_CONNECTION_UNIT_INFO_REQ;
			kPacketReq.m_wstrNickName = kPacket_;
			SendToCnRoom( ERM_SEARCH_UNIT_REQ, kPacketReq );
			return;
		}
	}
	//}}

	KELG_SEARCH_UNIT_REQ kPacketReq;
	kPacketReq.m_usEventID = EGS_GET_CONNECTION_UNIT_INFO_REQ;
	kPacketReq.m_wstrNickName = kPacket_;
	SendToLoginServer( ELG_SEARCH_UNIT_REQ, kPacketReq );
}
//}}

//{{ 2009. 6. 23  최육사	살펴보기
IMPL_ON_FUNC( EGS_WATCH_UNIT_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//자기 자신의 정보를 요청한 경우.
	if( kPacket_.m_iUnitUID == GetCharUID() )
	{
		KEGS_WATCH_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		GetConnectionUnitInfo( kPacket.m_kCUnitInfo );

		SendPacket( EGS_WATCH_UNIT_ACK, kPacket );
		return;
	}

	if( m_kTimer[TM_SEARCH_UNIT_REFRESH].elapsed() < (double)SEARCH_UNIT_REFRESH_TIME_LIMIT)
	{
		KEGS_WATCH_UNIT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_01;
		SendPacket( EGS_WATCH_UNIT_ACK, kPacket );
		return;
	}
	m_kTimer[TM_SEARCH_UNIT_REFRESH].restart();

	//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
	if( GetStateID() == KGSFSM::S_ROOM  &&  GetRoomUID() > 0 )
	{
		if( CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_DUNGEON  ||
			CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_PVP )
		{
			KELG_SEARCH_UNIT_REQ kPacketReq;
			kPacketReq.m_usEventID = EGS_WATCH_UNIT_REQ;
			kPacketReq.m_iUnitUID = kPacket_.m_iUnitUID;
			SendToCnRoom( ERM_SEARCH_UNIT_REQ, kPacketReq );
			return;
		}
	}
	//}}

	KELG_SEARCH_UNIT_REQ kPacketReq;
	kPacketReq.m_usEventID = EGS_WATCH_UNIT_REQ;
	kPacketReq.m_iUnitUID = kPacket_.m_iUnitUID;
	SendToLoginServer( ELG_SEARCH_UNIT_REQ, kPacketReq );
}
//}}

IMPL_ON_FUNC( ELG_SEARCH_UNIT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_usEventID == EGS_SEARCH_UNIT_REQ )
	{
		//찾는 캐릭이 있을경우
		if( kPacket_.m_iOK == NetError::NET_OK  ||  kPacket_.m_iOK == NetError::ERR_GM_CHAR_DENY_01 )
		{
			KEGS_SEARCH_UNIT_ACK kPacketAck;
			kPacketAck.m_iOK = kPacket_.m_iOK;
			if( kPacket_.m_iOK == NetError::NET_OK )
			{
				kPacketAck.m_kCUnitInfo = kPacket_.m_kCUnitInfo;
			}
			SendPacket( EGS_SEARCH_UNIT_ACK, kPacketAck );
		}
		//못찾았다고 오는경우
		else
		{
			SendToGameDB( DBE_SEARCH_UNIT_REQ, kPacket_.m_kCUnitInfo.m_wstrCharName );
		}
		return;
	}
	else if( kPacket_.m_usEventID == EGS_GET_CONNECTION_UNIT_INFO_REQ )
	{
		KEGS_GET_CONNECTION_UNIT_INFO_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			kPacketAck.m_kCUnitInfo = kPacket_.m_kCUnitInfo;
		}
		SendPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK, kPacketAck );
		return;
	}
	else if( kPacket_.m_usEventID == EGS_WATCH_UNIT_REQ )
	{
		KEGS_WATCH_UNIT_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			kPacketAck.m_kCUnitInfo = kPacket_.m_kCUnitInfo;
		}
		SendPacket( EGS_WATCH_UNIT_ACK, kPacketAck );
		return;
	}
	// 2011.05.05 lygan_조성욱 // 중국용 커뮤니티탭 유저 정보 보기
	else if(kPacket_.m_usEventID == EGS_USER_COMMUNITY_SURVEY_NEW_REQ )
	{
		if( kPacket_.m_iOK == NetError::NET_OK  ||  kPacket_.m_iOK == NetError::ERR_GM_CHAR_DENY_00 )
		{
			KEGS_USER_COMMUNITY_SURVEY_NEW_ACK kPacketAck;
			kPacketAck.m_iOK = kPacket_.m_iOK;
			if( kPacket_.m_iOK == NetError::NET_OK )
			{
				kPacketAck.unitUID = kPacket_.m_kCUnitInfo.m_iUnitUID;
				kPacketAck.m_cCommunity_Tab_Type = m_cCommunity_Tap_type;
			}
			SendPacket( EGS_USER_COMMUNITY_SURVEY_NEW_ACK , kPacketAck );
			m_cCommunity_Tap_type = 0;
		}
		//못찾았다고 오는경우
		else
		{
			SendToGameDB( DBE_SEARCH_UNIT_REQ, kPacket_.m_kCUnitInfo.m_wstrCharName );
			m_cCommunity_Tap_type = 0;
		}
		return;
	}

	START_LOG( cerr, L"event id가 이상합니다!" )
		<< BUILD_LOG( KEvent::GetIDStr( kPacket_.m_usEventID ) )
		<< END_LOG;
}

_IMPL_ON_FUNC( DBE_SEARCH_UNIT_ACK, KPacketOK )
{
    KEGS_SEARCH_UNIT_ACK kPacket;
    kPacket.m_iOK = kPacket_.m_iOK;
    SendPacket( EGS_SEARCH_UNIT_ACK, kPacket );
}

IMPL_ON_FUNC( ELG_GET_UNIT_INFO_REQ )
{
	//상태제한을 넣어야 한다.?
	KELG_GET_UNIT_INFO_ACK kPacket;
	kPacket.m_usEventID		 = kPacket_.m_usEventID;
	kPacket.m_iDemandGSUID	 = kPacket_.m_iDemandGSUID;
	kPacket.m_iDemandUserUID = kPacket_.m_iDemandUserUID;

	//{{ 2009. 3. 24  최육사	운영자 제한
	if( GetAuthLevel() == SEnum::UAL_GM )
	{
		kPacket.m_iOK	= NetError::ERR_GM_CHAR_DENY_01;
	}
	else
	{
		kPacket.m_iOK	= NetError::NET_OK;

		GetConnectionUnitInfo( kPacket.m_kCUnitInfo );
	}
	//}}

    SendToLoginServer( ELG_GET_UNIT_INFO_ACK, kPacket );
}

//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합

_IMPL_ON_FUNC( ERM_GET_UNIT_INFO_REQ, KELG_GET_UNIT_INFO_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		return;
	}

	//상태제한을 넣어야 한다.?
	KELG_GET_UNIT_INFO_ACK kPacket;
	kPacket.m_usEventID		 = kPacket_.m_usEventID;
	kPacket.m_iDemandGSUID	 = kPacket_.m_iDemandGSUID;
	kPacket.m_iDemandUserUID = kPacket_.m_iDemandUserUID;

	//{{ 2009. 3. 24  최육사	운영자 제한
	if( GetAuthLevel() == SEnum::UAL_GM )
	{
		kPacket.m_iOK	= NetError::ERR_GM_CHAR_DENY_00;
	}
	else
	{
		kPacket.m_iOK	= NetError::NET_OK;

		GetConnectionUnitInfo( kPacket.m_kCUnitInfo );
	}
	//}}

	SendToCnRoom( ERM_GET_UNIT_INFO_ACK, kPacket );
}

_IMPL_ON_FUNC( ERM_SEARCH_UNIT_ACK, KELG_SEARCH_UNIT_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( kPacket_.m_usEventID == EGS_GET_CONNECTION_UNIT_INFO_REQ )
	{
		KEGS_GET_CONNECTION_UNIT_INFO_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			kPacketAck.m_kCUnitInfo = kPacket_.m_kCUnitInfo;
		}
		SendPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK, kPacketAck );
		return;
	}
	else if( kPacket_.m_usEventID == EGS_WATCH_UNIT_REQ )
	{
		KEGS_WATCH_UNIT_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			kPacketAck.m_kCUnitInfo = kPacket_.m_kCUnitInfo;
		}
		SendPacket( EGS_WATCH_UNIT_ACK, kPacketAck );
		return;
	}

	START_LOG( cerr, L"event id가 이상합니다!" )
		<< BUILD_LOG( KEvent::GetIDStr( kPacket_.m_usEventID ) )
		<< END_LOG;
}

//}}

void KGSUser::CheckRepeatEvent( u_short usEventID )
{
	//2008.01.29.hoons.. 우선 1차적으로 방관련 패킷(req => ack)을 넣어준다.
	switch( usEventID )
	{
	case EGS_CREATE_ROOM_REQ:
	case EGS_JOIN_ROOM_REQ:
	case EGS_QUICK_JOIN_REQ:
	case EGS_LEAVE_ROOM_REQ:
	case EGS_BAN_USER_REQ:
	case EGS_CHANGE_TEAM_REQ:
	case EGS_CHANGE_READY_REQ:
	case EGS_CHANGE_PITIN_REQ:
	case EGS_CHANGE_MAP_REQ:
	case EGS_CHANGE_SLOT_OPEN_REQ:
	case EGS_CHANGE_PLAY_TIME_LIMIT_REQ:
	case EGS_CHANGE_WINNING_NUM_KILL_REQ:
	case EGS_STATE_CHANGE_GAME_START_REQ:
	case EGS_END_GAME_REQ:
	case EGS_STATE_CHANGE_RESULT_REQ:
	case EGS_LEAVE_GAME_REQ:
	case EGS_USER_UNIT_RE_BIRTH_POS_REQ:
	case EGS_USER_UNIT_DIE_REQ:
	case EGS_STATE_CHANGE_GAME_INTRUDE_REQ:
	case EGS_INTRUDE_START_REQ:
	case EGS_MY_USER_UNIT_INFO_TO_SERVER_REQ:
	case EGS_DUNGEON_STAGE_LOAD_REQ:
	case EGS_DUNGEON_STAGE_LOAD_COMPLETE_REQ:
	case EGS_DUNGEON_SUB_STAGE_OPEN_REQ:
	case EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ:
	case EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ:
	case EGS_NPC_UNIT_CREATE_REQ:
	//case EGS_NPC_UNIT_DIE_REQ:	//보스몬스터를 죽이고 난후 나머지 몬스터들을 게임처리를 위해 모두 die시킬때 발생할수있어 주석처리
	//case EGS_CHANGE_DUNGEON_DIFFICULTY_REQ:
	//개인상점관련.
	case EGS_REG_PERSONAL_SHOP_ITEM_REQ:
	case EGS_CREATE_PERSONAL_SHOP_REQ:
	case EGS_JOIN_PERSONAL_SHOP_REQ:
	case EGS_BREAK_PERSONAL_SHOP_REQ:
		{
			if( m_kRepeatEvent.m_usPreEventID == usEventID )
			{
				++m_kRepeatEvent.m_iPreEventIDCnt;

				//SimLayer에서 통계..
				GetKGSSimLayer()->AddRepeatEvent( m_kRepeatEvent.m_usPreEventID, m_kRepeatEvent.m_iPreEventIDCnt );
			}
			else
			{
				m_kRepeatEvent.Reset();

				m_kRepeatEvent.m_usPreEventID = usEventID;
				++m_kRepeatEvent.m_iPreEventIDCnt;
			}
		}
		break;

	default:
		{
			m_kRepeatEvent.Reset();
		}
		break;
	}
}

IMPL_ON_FUNC( EGS_CHANGE_SKILL_SLOT_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 2013. 01. 01	최육사	던전에서 필드로 돌아오지 못하는 오류 수정
	if( m_kTimer[TM_SKILL_CHANGE_COOL_TIME_AT_START_COUNT].elapsed() < 3.0 )
	{
		START_LOG( cerr, L"스킬 슬롯 쿨타임으로 인해 현재 스킬슬롯을 변경할수 없습니다!" )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		KEGS_CHANGE_SKILL_SLOT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SKILL_28;
		SendPacket( EGS_CHANGE_SKILL_SLOT_ACK, kPacket );
		return;
	}
	//}}

	KEGS_CHANGE_SKILL_SLOT_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	//{{ 2012. 12. 3	박세훈	스킬 슬롯 체인지 패킷 통합
	if( m_kSkillTree.ChangeSkillSlot( kPacket_, kPacket ) == false )
	//}}
	{
		kPacket.m_iOK = NetError::GetLastError();

		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOG( NetError::GetLastErrMsg() )
			<< BUILD_LOG( kPacket_.m_iSlotID )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;
	}
	else
	{
		kPacket.m_iSlotID	= kPacket_.m_iSlotID;
		kPacket.m_iSkillID	= kPacket_.m_iSkillID;
	}

	SendPacket( EGS_CHANGE_SKILL_SLOT_ACK, kPacket );


	// 대전방안에서 스킬 장착, 획득 정보가 바뀌면 방유저 정보를 갱신해줘야 한다.
	if( NetError::GetLastError() == NetError::NET_OK  &&  GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
			//{{ 2012. 02. 01	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
		case CXSLRoom::RT_TRAININGCENTER:
			//}}
			break;

		default:
			return;
		}

		//{{ 2012. 02. 01	최육사	배틀필드 시스템
		if( GetRoomUID() != 0 )
		{
			KERM_CHANGE_SKILL_INFO_IN_ROOM_NOT kPacketNot;
			GetRoomUserInfo( kPacketNot );
			SendToCnRoom( ERM_CHANGE_SKILL_INFO_IN_ROOM_NOT, kPacketNot );
		}
		//}}
	}
}

IMPL_ON_FUNC( EGS_NEW_BLACKLIST_USER_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_NEW_BLACKLIST_USER_ACK kPacket;

	if( x2::platform::IEquals(kPacket_.m_wstrNickName, GetCharName()) )
	{
		// 자기 자신을 블랙리스트에 등록할 수 없음.
		kPacket.m_iOK = NetError::ERR_BLACKLIST_05;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	if( m_kUserGameOption.IsAddChatBlackListPossible() == false )
	{
		// 최대 등록 가능한 인원 수 초과
		kPacket.m_iOK = NetError::ERR_BLACKLIST_00;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	// 로그인 서버에서 닉네임으로 UnitUID 찾기
	KELG_SEARCH_BLACK_LIST_UNIT_REQ kPacketToLg;
	kPacketToLg.m_wstrNickName = kPacket_.m_wstrNickName;
	SendToLoginServer( ELG_SEARCH_BLACK_LIST_UNIT_REQ, kPacketToLg );
}

IMPL_ON_FUNC( ELG_SEARCH_BLACK_LIST_UNIT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_NEW_BLACKLIST_USER_ACK kPacket;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		kPacket.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	if( m_kUserGameOption.IsChatBlackListUnit( kPacket_.m_iUnitUID ) == true )
	{
		// 이미 블랙리스트에 등록된 유저임.
		kPacket.m_iOK = NetError::ERR_BLACKLIST_01;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	// DB에 등록
	KDBE_NEW_BLACKLIST_USER_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID			= GetCharUID();
	kPacketToDB.m_iBlackListUnitUID = kPacket_.m_iUnitUID;
	SendToGameDB( DBE_NEW_BLACKLIST_USER_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_NEW_BLACKLIST_USER_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_NEW_BLACKLIST_USER_ACK kPacketAck;
	kPacketAck.m_iOK = kPacket_.m_iOK;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 블랙리스트에 유저추가
		m_kUserGameOption.AddChatBlackListUnit( kPacket_.m_kChatBlackListUnit.m_iUnitUID, kPacket_.m_kChatBlackListUnit.m_wstrNickName );

		kPacketAck.m_kChatBlackListUnit.m_iUnitUID	   = kPacket_.m_kChatBlackListUnit.m_iUnitUID;
		kPacketAck.m_kChatBlackListUnit.m_wstrNickName = kPacket_.m_kChatBlackListUnit.m_wstrNickName;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacketAck );
		return;
	}

	SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacketAck );
}

IMPL_ON_FUNC( EGS_DEL_BLACKLIST_USER_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
	// 상태제한은 UI 완성후 다시 수정할것!

	KEGS_DEL_BLACKLIST_USER_ACK kPacket;

	if( kPacket_.m_iUnitUID == GetCharUID() )
	{
		// 자기 자신은 블랙리스트에 삭제할 수 없음.
		kPacket.m_iOK = NetError::ERR_BLACKLIST_06;
		SendPacket( EGS_NEW_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	if( m_kUserGameOption.IsChatBlackListUnit( kPacket_.m_iUnitUID ) == false )
	{
		// 이미 블랙리스트에서 삭제된 유저임
		kPacket.m_iOK = NetError::ERR_BLACKLIST_02;
		SendPacket( EGS_DEL_BLACKLIST_USER_ACK, kPacket );
		return;
	}

	// DB에서 삭제
	KDBE_DEL_BLACKLIST_USER_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID			= GetCharUID();
	kPacketToDB.m_iBlackListUnitUID = kPacket_.m_iUnitUID;
	SendToGameDB( DBE_DEL_BLACKLIST_USER_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_DEL_BLACKLIST_USER_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_DEL_BLACKLIST_USER_ACK kPacket;
	kPacket.m_iOK				= kPacket_.m_iOK;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_kUserGameOption.DelChatBlackListUnit( kPacket_.m_iBlackListUnitUID, kPacket.m_kChatBlackListUnit.m_wstrNickName );
		kPacket.m_kChatBlackListUnit.m_iUnitUID = kPacket_.m_iBlackListUnitUID;
	}

	SendPacket( EGS_DEL_BLACKLIST_USER_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_UPDATE_COMMUNITY_OPTION_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_UPDATE_COMMUNITY_OPTION_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	// 커뮤니티 옵션 업데이트
	if( !m_kUserGameOption.SetDenyOptions( kPacket_.m_kDenyOptions ) )
	{
		kPacket.m_iOK = NetError::ERR_COMMUNITY_OPT_01;
		goto end_proc;
	}

	kPacket.m_kDenyOptions = kPacket_.m_kDenyOptions;

	//{{ 2009. 4. 1  최육사		친구 차단 옵션
	KELG_UPDATE_DENY_FRIEND_SHIP_NOT kPacketNot;
	kPacketNot.m_bDenyFriendShip = m_kUserGameOption.IsDenyFriendShip();
	SendToLoginServer( ELG_UPDATE_DENY_FRIEND_SHIP_NOT, kPacketNot );
	//}}

end_proc:
	SendPacket( EGS_UPDATE_COMMUNITY_OPTION_ACK, kPacket );
}

//{{ 2010. 07. 01  최육사	내구도 개편
IMPL_ON_FUNC( ERM_DECREASE_ENDURANCE_DUNGEON_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	std::vector< KInventoryItemInfo > vecDummy;
	KEGS_DECREASE_ENDURANCE_NOT kNot;

	// 설정된값 만큼 내구도를 깎는다
	m_kInventory.DecreaseEquippedItemEndurance( kPacket_.m_iEnduranceDamage, 0, 0, kNot.m_vecEnduranceUpdate, vecDummy );

	// 클라이언트에게 소모된 내구도를 보내준다!
	if( kNot.m_vecEnduranceUpdate.empty() == false )
	{
		SendPacket( EGS_DECREASE_ENDURANCE_NOT, kNot );
	}
}
//}}

IMPL_ON_FUNC( EGS_FRAME_AVERAGE_REQ )
{
	// 정상적으로 게임 종료시 상태는 S_ROOM, 팀매치에서 이탈시 S_CHANNEL, 던전에서 이탈시 S_LOCAL_MAP, S_VILLAGE_MAP
	VERIFY_STATE_WARN( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_FRAME_AVERAGE_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	if( kPacket_.m_iFrameAverage < 0 )
	{
		START_LOG( cerr, L"잘못된 프레임 평균값입니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_iFrameAverage )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_STATISTICS_00;
		goto end_proc;
	}

	int iGameType;
	switch( kPacket_.m_cGameType )
	{
	case CXSLGame::GT_PVP:
		iGameType = 0; // 대전
		break;

	case CXSLGame::GT_DUNGEON:
		iGameType = 1; // 던전
		break;

		//{{ 2012. 04. 04	최육사	배틀필드 시스템
	case CXSLGame::GT_BATTLE_FIELD:
		iGameType = 2; // 배틀필드
		break;
		//}}

	case CXSLGame::GT_NONE:
		goto end_proc;

	default:
		START_LOG( cerr, L"클라이언트에서 이상한 게임타입을 보냈다." )
			<< BUILD_LOGc( kPacket_.m_cGameType )
			<< END_LOG;
		goto end_proc;
	}

	{
		int iIndex = 0;
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( iGameType );
		// 던전에서 이탈시에는 던전채널에서 나온상태이므로 채널타입이 0이다.

		if( kPacket_.m_iFrameAverage >= 0  &&  kPacket_.m_iFrameAverage <= 20 )
		{
			iIndex = KStatistics::eSIColFps_0_20;
		}
		else if( kPacket_.m_iFrameAverage >= 21  &&  kPacket_.m_iFrameAverage <= 30 )
		{
			iIndex = KStatistics::eSIColFps_21_30;
		}
		else if( kPacket_.m_iFrameAverage >= 31  &&  kPacket_.m_iFrameAverage <= 40 )
		{
			iIndex = KStatistics::eSIColFps_31_40;
		}
		else if( kPacket_.m_iFrameAverage >= 41  &&  kPacket_.m_iFrameAverage <= 50 )
		{
			iIndex = KStatistics::eSIColFps_41_50;
		}
		else if( kPacket_.m_iFrameAverage >= 51  &&  kPacket_.m_iFrameAverage <= 60 )
		{
			iIndex = KStatistics::eSIColFps_51_60;
		}
		else if( kPacket_.m_iFrameAverage >= 61  &&  kPacket_.m_iFrameAverage <= 70 )
		{
			iIndex = KStatistics::eSIColFps_61_70;
		}
		else if( kPacket_.m_iFrameAverage >= 71  &&  kPacket_.m_iFrameAverage <= 80 )
		{
			iIndex = KStatistics::eSIColFps_71_80;
		}
		else if( kPacket_.m_iFrameAverage >= 81  &&  kPacket_.m_iFrameAverage <= 90 )
		{
			iIndex = KStatistics::eSIColFps_81_90;
		}
		else
		{
			iIndex = KStatistics::eSIColFps_91_unlimited;
		}

		KSIManager.IncreaseCount( KStatistics::SI_FPS, kKey, iIndex, 1 );

		START_LOG( clog2, L"프레임 평균 통계" )
			<< BUILD_LOG( GetUserName() )
			<< BUILD_LOG( iIndex )
			<< BUILD_LOG( kPacket_.m_iFrameAverage );
	}

end_proc:
	SendPacket( EGS_FRAME_AVERAGE_ACK, kPacket ); // 응답 패킷
}

IMPL_ON_FUNC( EGS_CHANGE_NICK_NAME_REQ )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_CHANGE_NICK_NAME_REQ, EGS_CHANGE_NICK_NAME_ACK );
	//}}

    char charBuf[255] = {0};
    std::string strNickName;

    WideCharToMultiByte( CP_ACP, 0, kPacket_.m_wstrNickName.c_str(), -1, charBuf, 255, NULL, NULL );
    strNickName = charBuf;

	if( strNickName.empty() || strNickName.size() > 12
		|| strNickName.size() < 2
		)   //캐릭터 닉네임 길이 검사.
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_06;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

	if( GetAuthLevel() < SEnum::UAL_GM && SiCXSLStringFilter()->CheckIsValidString( CXSLStringFilter::FT_NICKNAME, kPacket_.m_wstrNickName ) == false )
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

    // 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
    if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

	//{{ 2011. 07. 27	최육사	대전 개편
	if( SiKPvpMatchManager()->IsExistPvpNpcName( kPacket_.m_wstrNickName ) == true )
	{
		KEGS_CHANGE_NICK_NAME_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_CREATE_UNIT_01;
		SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
		return;
	}
	//}}


	{
		SendToGameDB( DBE_CHANGE_NICK_NAME_REQ, kPacket_ );
	}
}

_IMPL_ON_FUNC( DBE_CHANGE_NICK_NAME_ACK, KEGS_CHANGE_NICK_NAME_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_CHANGE_NICK_NAME_ACK );
	//}}

	// 닉네임 변경이 성공했다면 로그를 남기자!
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
        KELOG_CHANGE_NICK_NAME_NOT kPacketLog;
		kPacketLog.m_iUnitUID = kPacket_.m_iUnitUID;
		kPacketLog.m_wstrNickName = kPacket_.m_wstrNickName;
		SendToLogDB( ELOG_CHANGE_NICK_NAME_NOT, kPacketLog );
	}

    SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket_ );
}

IMPL_ON_FUNC( DBE_CHANGE_UNIT_CLASS_ACK )
{
	// 상태 제한 미정
	KEGS_CHANGE_JOB_CASH_ITEM_NOT kPacket;

    if( kPacket_.m_iOK == NetError::NET_OK )
    {
		// 전직 처리
		SetUnitClass( kPacket_.m_cUnitClass );
		ResetStat();
		m_kSkillTree.SetUnitClass( (int) GetUnitClass() );
		GetUnitInfo( kPacket.m_kUnitInfo );


		// 기본 스킬 추가
		// kimhc // 김현철 // 전직 변경 시, 모든 스킬 페이지에 적용 되도록 함
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iNewDefaultSkill1, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iNewDefaultSkill2, 1, 0 );


		// 전직 로그 남기기
        KDBE_UPDATE_UNIT_CLASS_NOT kPacketNot;
        kPacketNot.m_iUnitUID = GetCharUID();
        kPacketNot.m_cUnitClass = GetUnitClass();
        SendToLogDB( DBE_UPDATE_UNIT_CLASS_NOT, kPacketNot );

		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_JOB_CHANGE );
		}
		else if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true )
		{
			WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_SECOND_JOB_CHANGE );
		}
		else
		{
			if( GetAuthLevel() < SEnum::UAL_GM )
			{
				START_LOG( cerr, L"1차 전직도 2차 전직도 아닌데 로그를 기록하려고 하네?" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOGc( GetUnitClass() )
					<< END_LOG;
			}
			else
			{
				START_LOG( cout, L"운영자가 기본직업으로 치트썼군.. 로그 기록 하지 말자." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOGc( GetUnitClass() );
			}
		}

		//}}

		KEGS_CHANGE_MY_UNIT_INFO_NOT kMyInfoNot;
		kMyInfoNot.m_cUnitClass = GetUnitClass();
		kMyInfoNot.m_iNewDefaultSkill_1 = kPacket_.m_iNewDefaultSkill1;
		kMyInfoNot.m_iNewDefaultSkill_2 = kPacket_.m_iNewDefaultSkill2;
		SendPacket( EGS_CHANGE_MY_UNIT_INFO_NOT, kMyInfoNot );

		if( GetStateID() == KGSFSM::S_FIELD_MAP )
		{
			// 필드에 있을경우 유저 정보 갱신하기
			if( GetFieldUID() > 0 )
			{
				KERM_CHANGE_FIELD_USER_INFO_NOT kNot;
				GetFieldUserInfo( kNot.m_kInfo );
				SendToGSField( ERM_CHANGE_FIELD_USER_INFO_NOT, kNot );
			}
		}
		else if( GetStateID() == KGSFSM::S_ROOM )
		{
			switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
			{
			case CXSLRoom::RT_PVP:
			case CXSLRoom::RT_DUNGEON:
			case CXSLRoom::RT_TRAININGCENTER:
			//{{ 배틀필드 : 필드 시스템
			case CXSLRoom::RT_BATTLE_FIELD:
			//}}
				{
					KRoomUserInfo kRoomUserInfo;
					GetRoomUserInfo( kRoomUserInfo );
					SendToCnRoom( ERM_CHANGE_ROOM_USER_INFO_NOT, kRoomUserInfo );
				}
				break;

			case CXSLRoom::RT_SQUARE:
				{
					KSquareUserInfo kSquareUserInfo;
					GetSquareUserInfo( kSquareUserInfo );
					SendToCnRoom( ERM_CHANGE_SQUARE_USER_INFO_NOT, kSquareUserInfo );
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

		//{{ 2009. 10. 8  최육사	길드
		SendUpdateGuildMemberInfo();
		//}}

		//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
		//}}
    }

	switch( kPacket_.m_usEventID )
	{

	case EGS_BUY_CASH_ITEM_REQ:
		{
			// 전직 캐쉬 구매를 했으면 프로모션 큐브를 우편으로 보내준다!
			{
				CStringW cwstrItemID;
				cwstrItemID.Format( L"%d", CXSLItem::GetCashItemByUnitClass( GetUnitClass() ) );

				//{{ 2009. 11. 10  최육사	2차전직
				int iPromotionCubeItemID = 0;
				if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true )
				{
					iPromotionCubeItemID = _CONST_PRESENT_SKILL_INIT_ITEM_INT_::iChangeSecondJob;
				}
				else
				{
					iPromotionCubeItemID = _CONST_PRESENT_SKILL_INIT_ITEM_INT_::iChangeFirstJob;
				}
				//}}

				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = iPromotionCubeItemID;
				kPacketToDB.m_sQuantity	   = 1;
				kPacketToDB.m_wstrMessage  = cwstrItemID.GetBuffer();
				kPacketToDB.m_bGameServerEvent = false;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
			}

			IF_EVENT_ENABLED( CEI_ARA_FIRST_CLASS_CHANGE_EVENT )
			{
				if( CXSLUnit::UC_ARA_LITTLE_HSIEN == GetUnitClass() )
				{
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
					kPacketToDB.m_iRewardID	   = 1554;	// 소선을 위한 특별 선물 큐브B
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}

			IF_EVENT_ENABLED( CEI_ARA_NEW_FIRST_CLASS_EVENT )
			{
				if( CXSLUnit::UC_ARA_LITTLE_DEVIL == GetUnitClass() )
				{
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iClassChangeReward;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}

			IF_EVENT_ENABLED( CEI_ARA_NEW_SECOND_CLASS_EVENT )
			{
				if( CXSLUnit::UC_ARA_YAMA_RAJA == GetUnitClass() )
				{
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_SECOND_CLASS_EVENT_REWARD_ID::iAraNewSecondClassChangeReward;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}

				if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true )
				{
					// 이벤트 보상을 주자!
					KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
					kPacketToDB.m_iFromUnitUID = GetCharUID();
					kPacketToDB.m_iToUnitUID   = GetCharUID();
					kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
					kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_SECOND_CLASS_EVENT_REWARD_ID::iSecondClassChangeReward;
					SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
				}
			}



			// 전직 캐쉬 아이템
			SendPacket( EGS_CHANGE_JOB_CASH_ITEM_NOT, kPacket );
		}
		break;

	default:
		{
			START_LOG( cerr, L"잘못된 이벤트입니다." )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_usEventID )
				<< BUILD_LOG( KEvent::GetIDStr( kPacket_.m_usEventID ) )
				<< END_LOG;
		}
		break;
	}
}

IMPL_ON_FUNC( EGS_RECOMMEND_USER_REQ )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_FIELD_MAP ), EGS_RECOMMEND_USER_REQ, EGS_RECOMMEND_USER_ACK );
	//}}


	//{{ 2008. 5. 16  최육사  체험ID 제한

	// 등록취소와 동일하게 처리..

	{
		// 이미 등록 처리
		m_kUserRecommendManager.SetIsRecommend( true );

		KEGS_RECOMMEND_USER_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::NET_OK;
		SendPacket( EGS_RECOMMEND_USER_ACK, kPacketAck );
		return;
	}

	// 자기 자신을 추천한 경우.
	if( x2::platform::IEquals(GetCharName(), kPacket_.m_wstrNickName) )
	{
		KEGS_RECOMMEND_USER_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_RECOMMEND_USER_01;
		SendPacket( EGS_RECOMMEND_USER_ACK, kPacketAck );
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
	{
		KEGS_RECOMMEND_USER_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_RECOMMEND_USER_ACK, kPacketAck );
		return;
	}

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성

	KDBE_RECOMMEND_USER_REQ kPacket;
	kPacket.m_iUnitUID = GetCharUID();
	kPacket.m_wstrNickName = kPacket_.m_wstrNickName;
	SendToGameDB( DBE_RECOMMEND_USER_REQ, kPacket );
	//}

}

//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
//}


IMPL_ON_FUNC( DBE_RECOMMEND_USER_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_FIELD_MAP ), EGS_RECOMMEND_USER_ACK );
	//}}

	KEGS_RECOMMEND_USER_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 추천인 UnitUID 등록
		m_kUserRecommendManager.SetRecommendedUnitUID( kPacket_.m_iRecommendedUnitUID );

		// 이미 등록 처리
		m_kUserRecommendManager.SetIsRecommend( true );

		// 추천 성공시 부활석 5개 보상
        m_iNumResurrectionStone += 5;
		kPacket.m_iRewardResurrectionStone = 5;

		CTime tCurTime = CTime::GetCurrentTime();
		CStringW strCurTime = (CStringW)(tCurTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		std::wstring wstrCurTime = strCurTime.GetString();

		// 추천 메시지를 추천인에게 보냄.
		KEGS_RECOMMEND_USER_RESULT_NOT kNot;
		kNot.m_iRecommendedUnitUID	= kPacket_.m_iRecommendedUnitUID;
		kNot.m_iRecommendUnitUID	= GetCharUID();
		kNot.m_wstrFromNickName		= GetCharName();
		kNot.m_ucLevel				= GetLevel();
		kNot.m_wstrRecommendDate	= wstrCurTime;
		SendToLoginServer( ELG_RECOMMEND_USER_RESULT_NOT, kNot );

		//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
		//}
	}

	SendPacket( EGS_RECOMMEND_USER_ACK, kPacket );
}

_IMPL_ON_FUNC( ELG_RECOMMEND_USER_RESULT_NOT, KEGS_RECOMMEND_USER_RESULT_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 2010. 01. 11  최육사	추천인리스트
	// 초기화 되어 있을때만 추가하자!
	if( m_kUserRecommendManager.IsInitRecommendUserList() == true )
	{
		KRecommendUserInfo kInfo;
		kInfo.m_iUnitUID			= kPacket_.m_iRecommendUnitUID;
		kInfo.m_wstrNickName		= kPacket_.m_wstrFromNickName;
		kInfo.m_ucLevel				= kPacket_.m_ucLevel;
		kInfo.m_wstrRecommendDate	= kPacket_.m_wstrRecommendDate;
		kInfo.m_bIsOnline			= true;
        m_kUserRecommendManager.AddRecommendUser( kInfo );
	}
	//}}

	SendPacket( EGS_RECOMMEND_USER_RESULT_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_RESTORE_SPIRIT_NOT, KEGS_RESTORE_SPIRIT_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_RESTORE_SPIRIT_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_DEL_TUTORIAL_REQ )
{
	//상태는 생각해보고 추후에 막자..
	if( m_kUserTutorialInfo.OnDelTutorialReq( GetThisPtr<KGSUser>(), kPacket_ ) == false )
	{
		START_LOG( cerr, L"사제관계 종료 실패.!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_DEL_TUTORIAL_ACK )
{
	//상태는 생각해보고 추후에 막자..
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		if( m_kUserTutorialInfo.OnDelTutorialAck( GetThisPtr<KGSUser>(), kPacket_ ) == false )
		{
			START_LOG( cerr, L"사제리스트 삭제실패.!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( SiKTutorialManager()->GetDelReason( static_cast<KTutorialManager::TUTORIAL_DEL_REASON>(kPacket_.m_cReason) ) )
				<< BUILD_LOG( kPacket_.m_iTeacherUID )
				<< BUILD_LOG( kPacket_.m_iStudentUID )
				<< BUILD_LOG( kPacket_.m_iOK )
				<< END_LOG;
		}
	}
	else
	{
		START_LOG( cerr, L"사제관계 디비 지우기 실패!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( ELG_TUTORIAL_UPDATE_UNIT_INFO_NOT, KEGS_TUTORIAL_UPDATE_UNIT_INFO_NOT )
{
	//상태는 생각해보고 추후에 막자..
	if( SiKTutorialManager()->IsExistbylist( GetCharUID() ) == true )
	{
		//스승이 생기는 절차이므로 리스트에서 빼준다.
		if( SiKTutorialManager()->DeleteUnit( GetCharUID() ) == false )
		{
			START_LOG( cerr, L"투토리얼 제자리스트 유닛삭제 실패.!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
		}
	}

    if( m_kUserTutorialInfo.OnTutorialUpdateUnitInfoNot( GetThisPtr<KGSUser>(), kPacket_ ) == false )
	{
		START_LOG( cerr, L"사제정보 업데이트 실패.!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_wstrNickName )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( ELG_DEL_TUTORIAL_NOT, KEGS_DEL_TUTORIAL_NOT )
{
	//상태는 생각해보고 추후에 막자..

	if( m_kUserTutorialInfo.OnDelTutorialNot( GetThisPtr<KGSUser>(), kPacket_ ) == false )
	{
		START_LOG( cerr, L"사제리스트 지우기 실패.!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( EGS_TUTORIAL_STUDENT_LIST_REQ )
{
	//VERIFY_STATE( ( 1, KGSFSM::S_VILLAGE_MAP ) );

	switch( GetStateID() )
	{
	case KGSFSM::S_SERVER_SELECT:
		{
			START_LOG( cerr, L"상태 스테이트가 잘못되었습니다." )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
			return;
		}
	}

	KEGS_TUTORIAL_STUDENT_LIST_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	//스승 레벨 이상이여야 요청할수있음.
	if( GetLevel() >= KTutorialManager::UNIT_LEVEL_TEACHER_MIN )
	{
		if( SiKTutorialManager()->GetTutorPageInfo( kPacket_, kPacket ) == true )
		{
			goto end_proc;
		}

		kPacket.m_iOK = NetError::ERR_TUTORIAL_01;
	}
	else
	{
		kPacket.m_iOK = NetError::ERR_TUTORIAL_02;
	}

end_proc:
	SendPacket( EGS_TUTORIAL_STUDENT_LIST_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_REQUEST_TUTORIAL_REQ )
{
	//상태는 생각해보고 추후에 막자..

	//이패킷은 스승인 유저만 보낼수있다.
	KEGS_REQUEST_TUTORIAL_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	kPacket.m_iOK = NetError::ERR_TUTORIAL_02;
	SendPacket( EGS_REQUEST_TUTORIAL_ACK, kPacket );
	return;

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		kPacket.m_iOK = NetError::ERR_GUEST_USER_00;
		SendPacket( EGS_REQUEST_TUTORIAL_ACK, kPacket );
		return;
	}

	//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합
	if( IsSameServerGroupUID( kPacket_.m_iUnitUID ) == false )
	{
		kPacket.m_iOK = NetError::ERR_DIF_SERVER_GROUP_03;
		SendPacket( EGS_REQUEST_TUTORIAL_ACK, kPacket );
		return;
	}
	//}}

	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_TEACHER )
	{
		if( m_kUserTutorialInfo.GetTutorialUnitNum() < KTutorialManager::STUDENT_NUM &&
			m_kUserTutorialInfo.IsExistUnit( kPacket_.m_iUnitUID ) == false )
		{
			KELG_REQUEST_TUTORIAL_NOT kNot;
			kNot.m_iReceiverUnitUID = kPacket_.m_iUnitUID;

			kNot.m_kTutorialUnitInfo.m_bIsOnline = true;
			kNot.m_kTutorialUnitInfo.m_iUnitUID = GetCharUID();
			kNot.m_kTutorialUnitInfo.m_ucLevel = GetLevel();
			kNot.m_kTutorialUnitInfo.m_wstrNickName = GetCharName();

			SendToLoginServer( ELG_REQUEST_TUTORIAL_NOT, kNot );
		}
		else
		{
			if( m_kUserTutorialInfo.GetTutorialUnitNum() >= KTutorialManager::STUDENT_NUM )
			{
				kPacket.m_iOK = NetError::ERR_TUTORIAL_04;
			}
			else
			{
				kPacket.m_iOK = NetError::ERR_TUTORIAL_06;
			}
		}
	}
	else
	{
		kPacket.m_iOK = NetError::ERR_TUTORIAL_03;
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( clog, L"사제 맺기 실패.!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( (int)GetLevel() )
			<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
			;
	}

	SendPacket( EGS_REQUEST_TUTORIAL_ACK, kPacket );
}

IMPL_ON_FUNC( ELG_REQUEST_TUTORIAL_NOT )
{
	//상태는 생각해보고 추후에 막자..

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		// 그냥 거절함..
		goto end_proc;
	}

	//{{ 2009. 3. 24  최육사	GM제한
	if( GetAuthLevel() == SEnum::UAL_GM )
	{
		// 그냥 거절함..
		goto end_proc;
	}
	//}}

	//스승이 제자에게 사제관계 요청이 왔을경우임..
	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_STUDENT &&
		GetLevel() < KTutorialManager::UNIT_LEVEL_STUDENT_MAX ) //<< 기능이 안정될때까지 넣어둔다.
	{
		//게임중이 아니여야 함..
		if( GetStateID() == KGSFSM::S_ROOM  &&  GetRoomUID() != 0 )
		{
			KRoomInfoPtr spRoomInfo = SiKRoomListManager()->GetRoomInfo( GetRoomUID() );
			if( spRoomInfo != NULL )
			{
				if( spRoomInfo->m_RoomState == CXSLRoom::RS_PLAY )
					goto end_proc;
			}
		}

		//스승이 없어야 함.
		if( m_kUserTutorialInfo.IsExistbyTutorData() == false )
		{
			KEGS_REQUEST_TUTORIAL_NOT kNot;
			kNot = kPacket_.m_kTutorialUnitInfo;

			SendPacket( EGS_REQUEST_TUTORIAL_NOT, kNot );
			return;
		}
		else
		{
			START_LOG( clog, L"이미 스승이 있음.!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( (int)GetLevel() )
				<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_wstrNickName )
				<< BUILD_LOG( (int)kPacket_.m_kTutorialUnitInfo.m_ucLevel )
				<< END_LOG;

			goto end_proc;
		}
	}
	else
	{
		START_LOG( cerr, L"사제관계에서 제자여야 하는데..?ㅁ?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( (int)GetLevel() )
			<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_kTutorialUnitInfo.m_wstrNickName )
			<< BUILD_LOG( (int)kPacket_.m_kTutorialUnitInfo.m_ucLevel )
			<< END_LOG;

		goto end_proc;
	}

end_proc:
	//이경우는 제자가 아니거나 이미 스승이 있는경우.
	KEGS_REQUEST_TUTORIAL_REPLY_NOT kReplyNot;
	kReplyNot.m_bAccept = false;
	kReplyNot.m_iReceiverUnitUID = kPacket_.m_kTutorialUnitInfo.m_iUnitUID;

	kReplyNot.m_kTutorialUnitInfo.m_bIsOnline = true;
	kReplyNot.m_kTutorialUnitInfo.m_iUnitUID = GetCharUID();
	kReplyNot.m_kTutorialUnitInfo.m_ucLevel = GetLevel();
	kReplyNot.m_kTutorialUnitInfo.m_wstrNickName = GetCharName();

	SendToLoginServer( ELG_REQUEST_TUTORIAL_REPLY_NOT, kReplyNot );
}

IMPL_ON_FUNC( EGS_REQUEST_TUTORIAL_REPLY_NOT )
{
	return;

	//상태는 생각해보고 추후에 막자..

	//제자가 사제관계를 맺을지 여부를 결정후 서버에 알리는 패킷..
	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_STUDENT )
	{
		//스승이 없어야 함.
		if( m_kUserTutorialInfo.IsExistbyTutorData() == false )
		{
			//자시자신의 정보는 없기때문에 채워서 보내준다.
			kPacket_.m_kTutorialUnitInfo.m_bIsOnline = true;
			kPacket_.m_kTutorialUnitInfo.m_iUnitUID = GetCharUID();
			kPacket_.m_kTutorialUnitInfo.m_ucLevel = GetLevel();
			kPacket_.m_kTutorialUnitInfo.m_wstrNickName = GetCharName();

			SendToLoginServer( ELG_REQUEST_TUTORIAL_REPLY_NOT, kPacket_ );

			//if( SiKTutorialManager()->IsExistbylist( GetCharUID() ) == true )
			//{
			//	//스승이 생기는 절차이므로 리스트에서 빼준다.
			//	if( SiKTutorialManager()->DeleteUnit( GetCharUID() ) == false )
			//	{
			//		START_LOG( cerr, L"투토리얼 제자리스트 유닛삭제 실패.!" )
			//			<< BUILD_LOG( GetCharUID() )
			//			<< BUILD_LOG( GetCharName() )
			//			<< END_LOG;
			//	}
			//}

			return;
		}
	}

	START_LOG( cwarn, L"사제관계를 수락하는 입장이 틀리거나 스승이 먼저 생겼거나.!" )
		<< BUILD_LOG( GetCharUID() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOG( (int)GetLevel() )
		<< BUILD_LOG( kPacket_.m_iReceiverUnitUID )
		<< END_LOG;
}

_IMPL_ON_FUNC( ELG_REQUEST_TUTORIAL_REPLY_NOT, KEGS_REQUEST_TUTORIAL_REPLY_NOT )
{
	//상태는 생각해보고 추후에 막자..
	if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_TEACHER )
	{
		if( m_kUserTutorialInfo.GetTutorialUnitNum() < KTutorialManager::STUDENT_NUM )
		{
			if( kPacket_.m_bAccept == true )	//제자가 수락한경우.
			{
				//디비에 사제관계를 보내는 시점에서 지금유닛은 항상 스승이다.
				KDBE_INSERT_TUTORIAL_REQ	kPacket;
				kPacket.m_iTeacherUID = GetCharUID();
				kPacket.m_kStudentUnitInfo = kPacket_.m_kTutorialUnitInfo;

				SendToGameDB( DBE_INSERT_TUTORIAL_REQ, kPacket );
			}
			else
			{
				SendPacket( EGS_REQUEST_TUTORIAL_REPLY_NOT, kPacket_ );
			}

			return;
		}
	}

	START_LOG( cerr, L"최종 사제관계를 결정할려고 하는 시점에 스승설정이 이상함." )
		<< BUILD_LOG( GetCharUID() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOG( (int)m_kUserTutorialInfo.GetTutorUnitType() )
		<< BUILD_LOG( m_kUserTutorialInfo.GetTutorialUnitNum() )
		<< END_LOG;
}

IMPL_ON_FUNC( DBE_INSERT_TUTORIAL_ACK )
{
	//상태는 생각해보고 추후에 막자..

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KEGS_REQUEST_TUTORIAL_REPLY_NOT kNot;
		kNot.m_iReceiverUnitUID = GetCharUID();
		kNot.m_kTutorialUnitInfo = kPacket_.m_kStudentUnitInfo;
		kNot.m_bAccept = true;

		m_kUserTutorialInfo.OnInsertTutorialAck( GetThisPtr<KGSUser>(), kNot.m_kTutorialUnitInfo );

		SendPacket( EGS_REQUEST_TUTORIAL_REPLY_NOT, kNot );

		//사제관계를 맺었을때 던정방안일 경우 던전방으로 업데이트 함.
		if( GetRoomUID() == 0 )
			return;

		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_DUNGEON:
			{
				if( m_kUserTutorialInfo.GetTutorUnitType() == KUserTutorialInfo::TUT_TEACHER  &&  m_kUserTutorialInfo.IsExistbyTutorData() == true )
				{
					KERM_TUTORIAL_DUNGEON_ROOM_UPDATE_NOT kUpdateNot;
					m_kUserTutorialInfo.GetTutorUnitUIDList( kUpdateNot.m_vecStudentUnitUID );
					SendToCnRoom( ERM_TUTORIAL_DUNGEON_ROOM_UPDATE_NOT, kUpdateNot );
				}
			}
			break;

		default:
			break;
		}

		return;
	}

	START_LOG( cerr, L"사제처리 실패.!" )
		<< BUILD_LOG( GetCharUID() )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOG( kPacket_.m_kStudentUnitInfo.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_kStudentUnitInfo.m_wstrNickName )
		<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
		<< END_LOG;
}


IMPL_ON_FUNC( EGS_GET_SKILL_REQ )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	int iTotalSP = 0;	// 스킬을 배우는데 필요한 일반 스킬 포인트
	int iTotalCSP = 0;	// 스킬을 배우는데 필요한 캐시 스킬 포인트
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_GET_SKILL_REQ, EGS_GET_SKILL_ACK );

	KEGS_GET_SKILL_ACK kPacket;
	KDBE_INSERT_SKILL_REQ kDBReq;
	kPacket.m_iOK = NetError::ERR_UNKNOWN;

	if ( kPacket_.m_iActiveSkillPageNumber != m_kSkillTree.GetActiveSkillPageNumber() )
	{
		START_LOG( cerr, L"활성된 스킬 페이지 번호가 이상합니다 (EGS_GET_SKILL_REQ)" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iActiveSkillPageNumber )
			<< BUILD_LOG( m_kSkillTree.GetActiveSkillPageNumber() )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_PAGE_01;;
		goto end_proc;
	}

	// 변경된 스킬이 있는가?
	if( kPacket_.m_mapSkillList.empty() == true )
	{
		kPacket.m_iOK = NetError::ERR_SKILL_09;
		goto end_proc;
	}

	// 조건 검사
	if( m_kSkillTree.CheckGetNewSkill( kPacket_.m_mapSkillList, static_cast<int>(GetUnitClass()), GetLevel(), kPacket ) == false )
	{
		goto end_proc;
	}

	// CSP기간 만료인지 확인
	if( m_kSkillTree.IsCashSkillPointExpired() == true )
	{
		// 충분한 SP를 가지고있는지?
		if ( m_kSkillTree.GetSPoint() <= 0 )
		{
			kPacket.m_iOK = NetError::ERR_SKILL_17;
			goto end_proc;
		}
	}
	else
	{
		// 충분한 SP와 CSP를 가지고 있는지?
		if( m_kSkillTree.GetSPoint() + m_kSkillTree.GetCSPoint() <= 0 )
		{
			kPacket.m_iOK = NetError::ERR_SKILL_18;
			goto end_proc;
		}
	}

	// 배울려는 스킬 목록으로 필요한 sp 량 얻기

	if ( m_kSkillTree.GetNecessarySkillPoint( kPacket_.m_mapSkillList, iTotalSP, iTotalCSP,
		m_kSkillTree.AccessLearnedSkillTree() ) == false )
	{
		kPacket.m_iOK = NetError::ERR_SKILL_17;
		goto end_proc;
	}


	// sp 가 충분 한가?
	if( iTotalSP > m_kSkillTree.GetSPoint() || iTotalCSP > m_kSkillTree.GetCSPoint() )
	{
		kPacket.m_iOK = NetError::ERR_SKILL_18;
		goto end_proc;
	}

	// 캐시 스킬이 없다(기간 만료)
	if( m_kSkillTree.IsCashSkillPointExpired() == true )
	{
		if( iTotalCSP > 0 )
		{
			kPacket.m_iOK = NetError::ERR_SKILL_24;
			goto end_proc;
		}
	}
	else
	{
		if( m_kSkillTree.GetCSPoint() > 0 )
		{
			kDBReq.m_iCSPoint = m_kSkillTree.GetCSPoint() - iTotalCSP;
		}
		else
		{
			kDBReq.m_iCSPoint = -1;	// invalid 한 값을 보내서 SP로 배웠다는 것을 구분한다
		}
	}

	{
		std::map< int, KGetSkillInfo >::iterator mit = kPacket_.m_mapSkillList.begin();
		for( ; mit != kPacket_.m_mapSkillList.end() ; ++mit )
		{
			int iSkillLevel = 0;
			int iSkillCSPoint = 0;	// 배운 스킬 중 투자된 csp 얻기

			if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
			{
				if ( m_kSkillTree.GetSkillLevelAndCSP( iSkillLevel, iSkillCSPoint,
					mit->first, m_kSkillTree.AccessLearnedSkillTree() ) )
					mit->second.m_iSpendSkillCSPoint += iSkillCSPoint;	// 배운 스킬이 있다면
			}
			else
			{
				AddLogWhenSkillPagesNumberIsWrong( L"EGS_GET_SKILL_REQ" );
				goto end_proc;
			}
		}
	}

	// DB 로 보낼때 감소 시키고 보내자
	//{{

	m_kSkillTree.SubtractSPoint( iTotalSP );

	if( kDBReq.m_iCSPoint >= 0 )		// CSPoint를 사용해서 스킬을 획득한 경우
	{
		kDBReq.m_iBeforeCSPoint = m_kSkillTree.GetCSPoint();

		m_kSkillTree.SetCSPoint( m_kSkillTree.GetActiveSkillPagesIndex(), kDBReq.m_iCSPoint );
	}
	//}}
	m_kUserQuestManager.Handler_OnLearnNewSkill( GetThisPtr<KGSUser>(), kPacket_.m_vecNowLearnSkill );

	// DB 에 기록하러 가자
	kDBReq.m_iUnitUID		= GetCharUID();
	kDBReq.m_iTotalSpendSkillPoint = iTotalSP;						// 사용한 sp 총량
	kDBReq.m_mapSkillList = kPacket_.m_mapSkillList;

	kDBReq.m_iActiveSkillPageNumber = kPacket_.m_iActiveSkillPageNumber;

	SendToGameDB( DBE_INSERT_SKILL_REQ, kDBReq );
	return;

end_proc:

	// 못 배웠을 경우 처리
	SendPacket( EGS_GET_SKILL_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_INSERT_SKILL_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_GET_SKILL_ACK kPacket;
	kPacket.m_iOK	= kPacket_.m_iOK;

	kPacket.m_iActiveSkillPageNumber = kPacket_.m_iActiveSkillPageNumber;

	std::map< int, int > mapOldSkillList;

	// 실패 시 감소한 sp 롤백
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iBeforeCSPoint );
		m_kSkillTree.AddSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iTotalSpendSkillPoint );
	}
	else//스킬습득시 소모된sp 빼준다.
	{
		// 변경된 스킬 적용하기
		{
			std::map< int, KGetSkillInfo >::iterator mit = kPacket_.m_mapSkillList.begin();
			for( ; mit != kPacket_.m_mapSkillList.end() ; ++mit )
			{
				mapOldSkillList.insert( std::make_pair( mit->first, m_kSkillTree.GetSkillLevel( mit->first ) ) );

				// kimhc // 김현철 // 스킬 배우기는, 현재 사용중인 스킬 페이지에 적용 되도록 함
				m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( mit->second.m_iSkillID, mit->second.m_iSkillLevel, mit->second.m_iSpendSkillCSPoint );
			}
		}

		if( NetError::GetLastError() != NetError::NET_OK )
		{
			START_LOG_WITH_NAME( cerr )
				<< BUILD_LOG( NetError::GetLastErrMsg() )
				<< END_LOG;

			goto end_proc;
		}

		//스킬을 배우면서 변경된 유저의 데이터도 다시 넘겨준다.
		kPacket.m_mapSkillList			= kPacket_.m_mapSkillList;

		kPacket.m_iRemainSP				= m_kSkillTree.GetSPoint();

		kPacket.m_iRemainCSP			= m_kSkillTree.GetCSPoint();

		{
			std::map< int, KGetSkillInfo >::iterator mit = kPacket_.m_mapSkillList.begin();
			for( ; mit != kPacket_.m_mapSkillList.end() ; ++mit )
			{
				//{{ 2010. 10. 29	최육사	퀘스트 조건 추가
				std::map< int, int >::iterator mmit = mapOldSkillList.find( mit->first );
				if( mmit != mapOldSkillList.end() )
				{
					int iSkillCount = m_kSkillTree.GetSkillLevel( mit->first ) - mmit->second;
					for( int i = 0 ; i < iSkillCount ; ++i )
					{
						m_kUserQuestManager.Handler_OnUseSkillPoint( GetThisPtr<KGSUser>(), mit->first );

						//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
						m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_USE_SKILL_POINT );

						const int iUseSkillPointCount = m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_USE_SKILL_POINT );
						if( iUseSkillPointCount == 1 )
						{
							WriteCharacterLogToDB( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_USE_SKILL_POINT );
						}
					}
				}
			}
		}
	}

end_proc:
	kPacket.m_iOK				= NetError::GetLastError();
	SendPacket( EGS_GET_SKILL_ACK, kPacket );

	if( NetError::GetLastError() == NetError::NET_OK  &&  GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
			//{{ 2012. 02. 01	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
		case CXSLRoom::RT_TRAININGCENTER:
			//}}
			break;

		default:
			return;
		}

		//{{ 2012. 02. 01	최육사	배틀필드 시스템
		KERM_CHANGE_SKILL_INFO_IN_ROOM_NOT kPacketNot;
		GetRoomUserInfo( kPacketNot );
		SendToCnRoom( ERM_CHANGE_SKILL_INFO_IN_ROOM_NOT, kPacketNot );
	}
}

IMPL_ON_FUNC( EGS_RESET_SKILL_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_RESET_SKILL_REQ, EGS_RESET_SKILL_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_RESET_SKILL_ACK kAck;

		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_RESET_SKILL_ACK, kAck );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_RESET_SKILL_ACK kPacketAck;
	kPacketAck.m_iOK = NetError::ERR_UNKNOWN;

	if ( kPacket_.m_iActiveSkillPageNumber != m_kSkillTree.GetActiveSkillPageNumber() )
	{
		START_LOG( cerr, L"활성된 스킬 페이지 번호가 이상합니다 (EGS_RESET_SKILL_REQ)" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iActiveSkillPageNumber )
			<< BUILD_LOG( m_kSkillTree.GetActiveSkillPageNumber() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_SKILL_PAGE_01;

		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	// 초기화 정보 얻기
	const CXSLSkillTree::SkillTemplet* pDelSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( kPacket_.m_iSkillID );
	if( pDelSkillTemplet == NULL )
	{
		START_LOG( cerr, L"존재하지 않는 스킬ID를 초기화 하려 했습니다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_RESET_SKILL_00;
		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	const CXSLSkillTree::SkillTreeTemplet* pDelSkillTreeTemplet = SiCXSLSkillTree()->GetSkillTreeTemplet( GetUnitClass(), kPacket_.m_iSkillID );
	if( pDelSkillTreeTemplet == NULL )
	{
		START_LOG( cerr, L"트리에 존재하지 않는 스킬ID를 초기화 하려 했습니다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetUnitClass() )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_RESET_SKILL_00;
		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	// 조건 검사
	bool bSKillInitLevel = false;
	if( m_kSkillTree.CheckResetSkill( kPacket_, static_cast<int>(GetUnitClass()), GetLevel(), kPacketAck.m_iOK, bSKillInitLevel ) == false )
	{
		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	// 삭제 스킬 처리
	int iDelSkillLevel = 0;
	int iDelSkillCSPoint = 0;

	if ( false == m_kSkillTree.GetSkillLevelAndCSP( OUT iDelSkillLevel, OUT iDelSkillCSPoint,
		IN kPacket_.m_iSkillID, IN m_kSkillTree.AccessLearnedSkillTree() ) )
	{
		START_LOG( cerr, L"클라이언트에서 스킬 검사를 했을텐데.." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_RESET_SKILL_00;
		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	// 초기화 할 스킬인지 확인하자. 아니라면 1렙으로 한다.(기본 스킬, 선행 스킬)
	const int iAfterDelSkillLevel = ( bSKillInitLevel ? 0 : 1 );

	// 장착 중인지 검사, 장착중이라면 스킬레벨이 2이상인지 확인하기	- // note!! 나중에 장착중인 스킬중에 스킬되돌리기 이후에 없어지는 스킬은 자동으로 탈착되도록 처리하자
	std::vector< int > vecSkillData;
	m_kSkillTree.GetSkillSlot( vecSkillData );

	for( const int iEquipSkillID : vecSkillData )
	{
		if( iEquipSkillID == kPacket_.m_iSkillID )
		{
			if( iAfterDelSkillLevel <= 0 && iDelSkillLevel > 0 )
			{
				START_LOG( cwarn, L"장착중인 스킬을 초기화 할 것이므로 장착 해지해야 초기화 할 수 있다." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( kPacket_.m_iSkillID )
					<< BUILD_LOG( iDelSkillLevel )
					<< BUILD_LOG( iDelSkillCSPoint )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_RESET_SKILL_05;
				SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
				return;
			}
		}
	}

	// 스킬 포인트 계산
	int iAfterSP = 0;
	int iAfterCSP = 0;
	for( int i = 0 ; i < iDelSkillLevel ; ++i )
	{
		if( i == 0 )
		{
			if( iAfterDelSkillLevel > 0 )
				continue;

			iAfterSP += pDelSkillTemplet->m_iRequireLearnSkillPoint;
		}
		else
		{
			iAfterSP += pDelSkillTemplet->m_iRequireUpgradeSkillPoint;
		}
	}

	// 스킬에 투자된 캐시 포인트를 제외한다
	iAfterSP -= iDelSkillCSPoint;

	if( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		iAfterCSP = m_kSkillTree.GetCSPoint() + iDelSkillCSPoint;

		// max 수치를 넘을 만큼 돌려 줄 순 없다.
		if( m_kSkillTree.GetMaxCSPoint() < iAfterCSP )
		{
			iAfterSP += iAfterCSP - m_kSkillTree.GetMaxCSPoint();
			iAfterCSP = m_kSkillTree.GetMaxCSPoint();
		}
	}
	else
	{
		// kimhc // 김현철 // 2013-11-21 // 다른 곳에서는 Invaild 한 값을 -1로 셋팅하는데 여기는 왜 0으로 셋팅하나...
		// 아래 iAfterCSP <= 0 ? -1 : iAfterCSP 를 보면 0인 경우까지 Invaild 하다고 해주겠다는 것인데..
		// 0인 경우가 Invaild 한 것인가..?
		// 캐시스킬 기간이 만료 되지 않았어도 iAfterCSP가 0인 경우는 있을 것 같은데...

		// kimhc // 김현철 // 2013-11-21 // 이유를 알았음... 복잡하네..
		// 돌려받아야할 CSP가 0인 경우 DB 값을 UPDATE 해주지 않기 위해서 임
		iAfterCSP = 0; // invalid 한 값을 보내서 SP로 배웠다는 것을 구분한다
	}

	// 스킬 초기화 아이템 검사
	KInventoryItemInfo kInventoryItemInfo;
	if( m_kInventory.ResetSkillItem( kInventoryItemInfo ) == false )
	{
		START_LOG( cerr, L"스킬 초기화 아이템이 인벤토리에 없음." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_RESET_SKILL_01;
		SendPacket( EGS_RESET_SKILL_ACK, kPacketAck );
		return;
	}

	// DB로 전송
	KDBE_RESET_SKILL_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();
	kPacketToDB.m_iDelSkillID = kPacket_.m_iSkillID;
	kPacketToDB.m_iDelSkillLevel = iAfterDelSkillLevel;
	kPacketToDB.m_iCSPoint = iAfterCSP <= 0 ? -1 : iAfterCSP;

	kPacketToDB.m_iBeforeSPoint = m_kSkillTree.GetSPoint();
	kPacketToDB.m_iActiveSkillPageNumber = kPacket_.m_iActiveSkillPageNumber;

	kPacketToDB.m_iBeforeCSPoint =  m_kSkillTree.GetCSPoint();

	m_kSkillTree.AddSPoint( kPacket_.m_iActiveSkillPageNumber - 1, iAfterSP );
	m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, iAfterCSP );

	kPacketToDB.m_vecUpdatedInventorySlot.push_back( kInventoryItemInfo );
	m_kInventory.FlushQuantityChange( kPacketToDB.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.FlushDeletedItem( kPacketToDB.m_kItemQuantityUpdate.m_vecDeleted );

	SendToGameDB( DBE_RESET_SKILL_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_RESET_SKILL_ACK )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	bool isDefaultSkill{};
	bool bAllFollowingSkillLevelZero{};
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_RESET_SKILL_ACK kPacket;
	kPacket.m_iSPoint = 0;
	kPacket.m_iCSPoint = 0;
	kPacket.m_iOK = kPacket_.m_iOK;
	kPacket.m_iDelSkillID			= kPacket_.m_iDelSkillID;
	kPacket.m_vecInventorySlotInfo	= kPacket_.m_vecUpdatedInventorySlot;

	kPacket.m_iActiveSkillPageNumber	= kPacket_.m_iActiveSkillPageNumber;

	// DB쿼리 실패된 아이템이 있다면 롤백.
	m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		// DB 업데이트 실패
		// 증가한 sp 롤백

		m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iBeforeCSPoint );
		int iSPoint = m_kSkillTree.GetSPoint() - kPacket_.m_iBeforeSPoint;
		m_kSkillTree.SubtractSPoint( iSPoint );

		goto end_proc;
	}

	// 스킬 초기화
	isDefaultSkill = SiCXSLSkillTree()->IsUnitTypeDefaultSkill( kPacket_.m_iDelSkillID );
	bAllFollowingSkillLevelZero = m_kSkillTree.IsAllFollowingSkillLevelZero( kPacket_.m_iDelSkillID );

	if ( m_kSkillTree.IsActiveSkillPageNumberValid() )
	{
		m_kSkillTree.ResetSkill( m_kSkillTree.AccessLearnedSkillTree(), kPacket_.m_iDelSkillID, isDefaultSkill );
	}
	else
	{
		AddLogWhenSkillPagesNumberIsWrong( L"DBE_RESET_SKILL_ACK" );
		goto end_proc;
	}

	if( isDefaultSkill == true || bAllFollowingSkillLevelZero == false )
	{
		// kimhc // 김현철 // 2013-11-17 // 망각의 알약 등에 의해
		// 스킬 한개 초기화 시, 현재 사용중인 스킬 페이지만 초기화 하도록 함
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDelSkillID, 1, 0 );
	}

	kPacket.m_iSPoint		= m_kSkillTree.GetSPoint();

	kPacket.m_iCSPoint		= m_kSkillTree.GetCSPoint();

end_proc:
	SendPacket( EGS_RESET_SKILL_ACK, kPacket );

	// 초기화 정보 얻기
	const CXSLSkillTree::SkillTemplet* pCheckSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( kPacket_.m_iDelSkillID );
	if( pCheckSkillTemplet == NULL )
	{
		START_LOG( cerr, L"존재하지 않는 스킬템플릿 입니다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iDelSkillID )
			<< END_LOG;
		return;
	}

	//{{ 2012. 02. 03	최육사	배틀필드 시스템
	//////////////////////////////////////////////////////////////////////////
	// 패시브 스킬의 경우 RoomUserInfo 정보를 업데이트 한다.
	// 방안일 경우 다른유저들에게도 전송
	if( kPacket_.m_iOK == NetError::NET_OK  &&  GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
		case CXSLRoom::RT_BATTLE_FIELD:
		case CXSLRoom::RT_TRAININGCENTER:
			break;

		default:
			return;
		}

		if( GetRoomUID() != 0 )
		{
			KERM_CHANGE_SKILL_INFO_IN_ROOM_NOT kPacketNot;
			GetRoomUserInfo( kPacketNot );
			SendToCnRoom( ERM_CHANGE_SKILL_INFO_IN_ROOM_NOT, kPacketNot );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//}}
}

IMPL_ON_FUNC( EGS_INIT_SKILL_TREE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_INIT_SKILL_TREE_REQ, EGS_INIT_SKILL_TREE_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_INIT_SKILL_TREE_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_INIT_SKILL_TREE_ACK kAck;
	kAck.m_iOK = NetError::ERR_UNKNOWN;

	if ( kPacket_.m_iActiveSkillPageNumber != m_kSkillTree.GetActiveSkillPageNumber() )
	{
		START_LOG( cerr, L"활성된 스킬 페이지 번호가 이상합니다 (EGS_INIT_SKILL_TREE_REQ)" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iActiveSkillPageNumber )
			<< BUILD_LOG( m_kSkillTree.GetActiveSkillPageNumber() )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_SKILL_PAGE_01;;
		SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
		return;
	}

	if( !m_kInventory.IsExist( kPacket_.m_iItemUID ) )
	{
		START_LOG( cerr, L"스킬초기화 아이템이 없는데 패킷 날아옴.!" )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_ITEM_04;
		SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
		return;
	}

	int iItemID = m_kInventory.GetItemID( kPacket_.m_iItemUID );

	switch ( iItemID )
	{

	default:
		{
			START_LOG( cerr, L"스킬초기화 아이템ID가 이상함." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< BUILD_LOG( iItemID )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_ITEM_04;
			SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
			return;

		} break;

	case CXSLItem::CI_SKILL_INIT_ITEM:
	case CXSLItem::CI_SKILL_INIT_EVENT_ITEM:

	case CXSLItem::CI_SKILL_INIT_EVENT_ITEM2:
	case CXSLItem::CI_SKILL_INIT_EVENT_ITEM3:
	case CXSLItem::CI_SKILL_INIT_EVENT_ITEM4:
	case CXSLItem::EI_RURIEL_RESET_SKILL_ITEM:
	case CXSLItem::CI_SKILL_INIT_EVENT_ITEM_JP:
		break;
	}

	int iDefaultSkillID[6] = {0,};
	if( !SiCXSLSkillTree()->GetUnitClassDefaultSkill( GetUnitClass(), iDefaultSkillID[0], iDefaultSkillID[1], iDefaultSkillID[2], iDefaultSkillID[3], iDefaultSkillID[4], iDefaultSkillID[5] ) )
	{
		START_LOG( cerr, L"유닛타입이 이상함." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetUnitType() )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_SKILL_12;
		SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
		return;
	}

	[[maybe_unused]] const int DEFAULT_SKILL_SP = 0;	// 기본 스킬은 공짜
	int iRetrievedSPoint = 0 ;	// 돌려받을 SP
	int iRetrievedCSPoint = 0;	// 돌려받을 CSP

	if( SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), iRetrievedSPoint ) == false )
	{
		START_LOG( cerr, L"레벨로 환산된 sp 정보가 이상합니다." )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( static_cast<int>(GetLevel()) )
			<< BUILD_LOG( iRetrievedSPoint )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_SKILL_20;
		SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
		return;
	}

	if( false == m_kSkillTree.IsCashSkillPointExpired() )
	{
		iRetrievedCSPoint = m_kSkillTree.GetMaxCSPoint();
	}
	else
	{

		/// kimhc // 김현철 // 2013-11-21
		/// 스킬 초기화는 한개의 스킬페이지에만 적용 되지만
		/// CSPoint 가 얼마나 남았는지는 전체페이지를 검사하도록 했음
		/// 캐시 스킬 기간이 만료 됐는데, 어느 한개 페이지의 CSP만 초기화 되었는지에 대한
		/// 체크는 이상하다고 생각 되서...
		if( m_kSkillTree.GetCSPointAnyPage() )
		{
			START_LOG( cerr, L"cash skill point기한이 다되었는데 스킬트리에 csp가 남아있다." )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( iRetrievedSPoint )
				<< BUILD_LOG( iRetrievedCSPoint )

				<< BUILD_LOG( m_kSkillTree.GetCSPointAnyPage() )

				<< END_LOG;

			kAck.m_iOK = NetError::ERR_SKILL_12;
			SendPacket( EGS_INIT_SKILL_TREE_ACK, kAck );
			return;
		}
	}

	KDBE_INIT_SKILL_TREE_REQ kDBReq;
	kDBReq.m_iUnitUID  = GetCharUID();
	kDBReq.m_iItemUID  = kPacket_.m_iItemUID;
	kDBReq.m_iSPoint  = iRetrievedSPoint;
	kDBReq.m_iCSPoint  = iRetrievedCSPoint;
	kDBReq.m_iDefaultSkillID1 = iDefaultSkillID[0];
	kDBReq.m_iDefaultSkillID2 = iDefaultSkillID[1];
	kDBReq.m_iDefaultSkillID3 = iDefaultSkillID[2];
	kDBReq.m_iDefaultSkillID4 = iDefaultSkillID[3];
	kDBReq.m_iDefaultSkillID5 = iDefaultSkillID[4];
	kDBReq.m_iDefaultSkillID6 = iDefaultSkillID[5];

	kDBReq.m_iBeforeSPoint = m_kSkillTree.GetSPoint();
	kDBReq.m_iActiveSkillPageNumber = kPacket_.m_iActiveSkillPageNumber;

	kDBReq.m_iBeforeCSPoint = m_kSkillTree.GetCSPoint();

	m_kSkillTree.SetSPoint( iRetrievedSPoint );
	m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, iRetrievedCSPoint );

	SendToGameDB( DBE_INIT_SKILL_TREE_REQ, kDBReq );
}

IMPL_ON_FUNC( DBE_INIT_SKILL_TREE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_INIT_SKILL_TREE_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"스킬초기화 실패.!" )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		m_kSkillTree.SetSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iBeforeSPoint );
		m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iBeforeCSPoint );

	}
	else
	{
		std::vector< KInventoryItemInfo > vecUpdatedSlot;
		KInventoryItemInfo kInfo;

		int iUsageType = 0;
		if( m_kInventory.GetUsageType( kPacket_.m_iItemUID, iUsageType ) == false )
		{
			START_LOG( cerr, L"USAGE TYPE 읽어오기 실패.!" )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< BUILD_LOG( GetCharName() );
		}

		//usage type 읽어오기 실패를 해도 아이템 삭제시도는 해본다.
		if( iUsageType == CXSLItem::PT_QUANTITY )
		{
			int iDecreased;
			int iCategory;
			int iSlotID;
			LIF( m_kInventory.GetCategorySlot( kPacket_.m_iItemUID, iCategory, iSlotID ) );

			m_kInventory.DecreaseQuantity( kPacket_.m_iItemUID, 1, iDecreased, KDeletedItemInfo::DR_INIT_SKILL );
			LIF( 1 == iDecreased );
			m_kInventory.GetInventorySlotInfo( iCategory, iSlotID, kInfo );
		}
		else
		{
			m_kInventory.DeleteItem( kPacket_.m_iItemUID, kInfo, KDeletedItemInfo::DR_INIT_SKILL );
		}
		vecUpdatedSlot.push_back( kInfo );

		kPacket.m_vecInventorySlotInfo = vecUpdatedSlot;

		// 배운스킬 삭제.. 장착스킬 제거
		m_kSkillTree.SetSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iSPoint );
		m_kSkillTree.SetCSPoint( kPacket_.m_iActiveSkillPageNumber - 1, kPacket_.m_iCSPoint );

		kPacket.m_iSPoint					= kPacket_.m_iSPoint;
		kPacket.m_iCSPoint					= kPacket_.m_iCSPoint;
		kPacket.m_iActiveSkillPageNumber	= kPacket_.m_iActiveSkillPageNumber;

		m_kSkillTree.ResetThisSkillPage( kPacket_.m_iActiveSkillPageNumber );

		// 기본 스킬 넣기
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID1, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID2, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID3, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID4, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID5, 1, 0 );
		m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnUsedPage( kPacket_.m_iDefaultSkillID6, 1, 0 );


		// logdb로 통계 보내기
		KELOG_INIT_SKILL_TREE_NOT kNot;
		kNot.m_iUnitUID = GetCharUID();
		kNot.m_iSPoint	= kPacket_.m_iSPoint;
		kNot.m_iCSPoint = kPacket_.m_iCSPoint;
		kNot.m_iItemUID = kPacket_.m_iItemUID;

		SendToLogDB( ELOG_INIT_SKILL_TREE_NOT, kNot );
	}

	SendPacket( EGS_INIT_SKILL_TREE_ACK, kPacket );

	// 방안일 경우 다른유저들에게도 전송
	if( kPacket_.m_iOK == NetError::NET_OK  &&  GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
			//{{ 2012. 02. 02	최육사	배틀필드 시스템
		case CXSLRoom::RT_BATTLE_FIELD:
		case CXSLRoom::RT_TRAININGCENTER:
			break;

		default:
			return;
		}

		//{{ 2012. 02. 02	최육사	배틀필드 시스템
		if( GetRoomUID() != 0 )
		{
			KERM_CHANGE_SKILL_INFO_IN_ROOM_NOT kPacketNot;
			GetRoomUserInfo( kPacketNot );
			SendToCnRoom( ERM_CHANGE_SKILL_INFO_IN_ROOM_NOT, kPacketNot );
		}
	}
}


IMPL_ON_FUNC_NOPARAM( EGS_CURRENT_TIME_REQ )
{
    // 상태 막아야 함.

    CTime tCurrent = CTime::GetCurrentTime();
    KEGS_CURRENT_TIME_ACK kPacket;
    kPacket.m_wstrCurrentTime = ( CStringW )tCurrent.Format( _T( "%Y-%m-%d %H:%M:%S" ) );
	kPacket.m_tCurrentTime = tCurrent.GetTime();

    SendPacket( EGS_CURRENT_TIME_ACK, kPacket );
}

//{{ 2009. 8. 7  최육사		은행
IMPL_ON_FUNC( DBE_EXPAND_INVENTORY_SLOT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	switch( kPacket_.m_usEventID )
	{
	case EGS_GET_PURCHASED_CASH_ITEM_REQ:
		{
			KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::NET_OK;
			kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;

			std::map< int, int >::const_iterator mit;
			for( mit = kPacket_.m_mapExpandedSlot.begin(); mit != kPacket_.m_mapExpandedSlot.end(); ++mit )
			{
				int iExpanded;
// 해외팀 제거
//#ifdef SERV_REFORM_INVENTORY_INT
				//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
//#ifdef SERV_REFORM_INVENTORY_TEST
//				if( mit->first == CXSLInventory::ST_BANK )
//				{
//					m_kInventory.ExpandSlot( mit->first, mit->second, iExpanded );
//				}
//				else
//				{
//					m_kInventory.ExpandSlot( mit->first, mit->second - GetExpandedSlotSize( mit->first ), iExpanded );
//				}
//#else
				m_kInventory.ExpandSlot( mit->first, mit->second, iExpanded );
//#endif SERV_REFORM_INVENTORY_TEST
				//}}
//#endif SERV_REFORM_INVENTORY_INT
				kPacket.m_mapExpandedCategorySlot.insert( std::make_pair( mit->first, iExpanded ) );
			}

			SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
		}
		break;

	case EGS_USE_ITEM_IN_INVENTORY_REQ:
		//{{ 2011. 12. 21	최육사	은행 확장 패키지 구성 상품
	case ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK:
		//}}
		{
            KEGS_EXPAND_BANK_SLOT_NOT kNot;

			std::map< int, int >::const_iterator mit;
			for( mit = kPacket_.m_mapExpandedSlot.begin(); mit != kPacket_.m_mapExpandedSlot.end(); ++mit )
			{
				int iExpanded;
// 해외팀 제거
//#ifdef SERV_REFORM_INVENTORY_INT
				//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
//#ifdef SERV_REFORM_INVENTORY_TEST
//				if( mit->first == CXSLInventory::ST_BANK )
//				{
//					m_kInventory.ExpandSlot( mit->first, mit->second, iExpanded );
//				}
//				else
//				{
//					m_kInventory.ExpandSlot( mit->first, mit->second - GetExpandedSlotSize( mit->first ), iExpanded );
//				}
//#else
				m_kInventory.ExpandSlot( mit->first, mit->second, iExpanded );
//#endif SERV_REFORM_INVENTORY_TEST
				//}}
//#endif SERV_REFORM_INVENTORY_INT

				kNot.m_mapExpandedCategorySlot.insert( std::make_pair( mit->first, iExpanded ) );
			}

			SendPacket( EGS_EXPAND_BANK_SLOT_NOT, kNot );
		}
		break;

	default:
		{
			START_LOG( cerr, L"이상한 이벤트가 왔다?" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( KEvent::GetIDStr( kPacket_.m_usEventID ) )
				<< END_LOG;
		}
		break;
	}
}
//}}

IMPL_ON_FUNC( DBE_EXPAND_SKILL_SLOT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_kSkillTree.ExpandSkillSlotB( kPacket_.m_wstrSkillSlotBEndDate );

		KEGS_EXPAND_SKILL_SLOT_NOT kPacket;
		kPacket.m_iUnitUID = GetCharUID();
		kPacket.m_cSkillSlotBExpirationState = (char) m_kSkillTree.GetSkillSlotBExpirationState();
		kPacket.m_wstrSkillSlotBEndDate = m_kSkillTree.GetSkillSlotBEndDateString();
		SendPacket( EGS_EXPAND_SKILL_SLOT_NOT, kPacket );

			if( 0 != kPacket_.m_iPeriodExpire ) // 영구가 아니면, 아이템 가져왔다는 패킷을 보낸다
			{
				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::NET_OK;
				kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
				SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
			}

	}
}

//{{ 2008. 12. 14  최육사	캐릭터 슬롯 확장
IMPL_ON_FUNC( DBE_EXPAND_CHAR_SLOT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_nUnitSlot = kPacket_.m_iCharSlotSize;

		KEGS_EXPAND_CHAR_SLOT_NOT kPacketNot;
		kPacketNot.m_iCharSlotSize = m_nUnitSlot;
		SendPacket( EGS_EXPAND_CHAR_SLOT_NOT, kPacketNot );
	}
}
//}}

IMPL_ON_FUNC( DBE_UPDATE_EVENT_TIME_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		for( const int iEventUID : kPacket_.m_setConnectTimeEvent )
		{
			int iRewardID = 0;
			//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
			if( SiKGameEventManager()->GetConnectTimeEventReward( GetLevel(), iEventUID, iRewardID ) == false )
			//}}
			{
				START_LOG( cerr, L"이벤트 보상 정보를 얻지 못했습니다!" )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( iEventUID )
					<< END_LOG;

				continue;
			}

			if( iRewardID <= 0 )
			{
				continue;
			}

			// 이벤트 보상을 주자!
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = iRewardID;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );

			//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
			//}}
		}

		//{{ 2009. 12. 7  최육사	누적시간이벤트

		for( const int iEventUID : kPacket_.m_setCumulativeTimeEvent )
		{
			int iRewardID = 0;
			if( !SiKGameEventManager()->GetCumulativeTimeEventReward( iEventUID, iRewardID ) )
			{
				START_LOG( cerr, L"이벤트 보상 정보를 얻지 못했습니다!" )
					<< BUILD_LOG( GetCharName() )
					<< BUILD_LOG( iEventUID )
					<< END_LOG;

				continue;
			}


			if( iRewardID <= 0 )
			{
				START_LOG( cerr, L"이벤트 보상정보가 이상합니다. 이벤트테이블 확인 필요!" )
					<< BUILD_LOG( iEventUID )
					<< BUILD_LOG( iRewardID )
					<< END_LOG;

				// 보상정보가 이상하면 편지를 주지 말자!
				continue;
			}

			// 이벤트 보상을 주자!
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = iRewardID;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}

		//}}
	}
}

//{{ 2009. 7. 6  최육사		랭킹 개편
IMPL_ON_FUNC_NOPARAM( ELG_NEW_RECORD_RANKING_SUCCESS_NOT )
{
	// 상태 제한?

	// 클라이언트에게 알려주기
	SendID( EGS_NEW_RECORD_RANKING_SUCCESS_NOT );
}

IMPL_ON_FUNC( EGS_GET_RANKING_INFO_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KEGS_GET_RANKING_INFO_ACK kPacket;
	SiKGSRankingManager()->GetRankingInfo( kPacket_, kPacket );	// 랭킹 페이지 얻기
	m_kUserRankingManager.GetRankingInfo( GetThisPtr<KGSUser>(), kPacket_, kPacket );	// 유저 최근 랭킹 얻기
    SendPacket( EGS_GET_RANKING_INFO_ACK, kPacket );
}
//}}

//{{ 2008. 10. 6  최육사	타이틀
IMPL_ON_FUNC( EGS_EQUIP_TITLE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 타이틀 장착
	m_kUserTitleManager.EquipTitle( kPacket_.m_iTitleID );

	KEGS_EQUIP_TITLE_ACK kPacket;
	kPacket.m_iOK	   = NetError::GetLastError();
	kPacket.m_iTitleID = kPacket_.m_iTitleID;
	SendPacket( EGS_EQUIP_TITLE_ACK, kPacket );

	if( kPacket.m_iOK == NetError::NET_OK )
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
				//{{ 2012. 10. 23	최육사	배틀필드 시스템
			case CXSLRoom::RT_BATTLE_FIELD:
				//}}
				{
					KERM_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT kPacketNot;
					//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
					kPacketNot.m_cChangeEquippedPacketType = KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_TITLE;
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
}

IMPL_ON_FUNC( DBE_INSERT_TITLE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 보상으로 받은 칭호 추가
		__int64 tCTime = 0;
		bool bInfinity = false;
		if( m_kUserTitleManager.AddTitle( kPacket_.m_iTitleID, kPacket_.m_wstrEndDate, tCTime, bInfinity ) )
		{
			KEGS_REWARD_TITLE_NOT kPacketNot;
			kPacketNot.m_kTitle.m_iTitleID	  = kPacket_.m_iTitleID;
			kPacketNot.m_kTitle.m_wstrEndDate = kPacket_.m_wstrEndDate;
			kPacketNot.m_kTitle.m_kCTime	  = tCTime;
			kPacketNot.m_kTitle.m_bInfinity	  = bInfinity;
			SendPacket( EGS_REWARD_TITLE_NOT, kPacketNot );

			// 선행미션 완료로 인해 오픈되는 미션이 있는지 검사
			m_kUserTitleManager.CheckNewMission( GetThisPtr<KGSUser>() );
		}

		//{{ 2010. 06. 21  최육사	통계 스레드
		KE_LOCAL_LOG_TITLE_NOT kNot;
		kNot.m_iTitleID		= kPacket_.m_iTitleID;
		kNot.m_wstrCharName	= GetCharName();
		kNot.m_ucLevel		= GetLevel();
		KSIManager.QueueingEvent( E_LOCAL_LOG_TITLE_NOT, kNot );
		//////////////////////////////////////////////////////////////////////////
		//}}

		//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		if( m_kComeBackUserInfo.GetIsComeBackUser() == true
		 && kPacket_.m_iTitleID == KRewardTable::ERI_COME_BACK_USER_TITLE )
		{
			// 타이틀 장착
			m_kUserTitleManager.EquipTitle( kPacket_.m_iTitleID );
		}
		//}}



		if( kPacket_.m_iTitleID == KRewardTable::ERI_CREATE_NEW_CHARACTER_REWARD_TITLE )
		{
			// 타이틀 장착
			m_kUserTitleManager.EquipTitle( kPacket_.m_iTitleID );
		}



	}
}

_IMPL_ON_FUNC( ELG_INSERT_TITLE_NOT, KDBE_INSERT_TITLE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		if( kPacket_.m_iUnitUID != GetCharUID() )
		{
			START_LOG( cerr, L"로그인 서버에서 해당 유닛으로 보냈는데 UnitUID가 다르다?" )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}

		// 보상으로 받은 칭호 추가
		__int64 tCTime = 0;
		bool bInfinity = false;
		if( m_kUserTitleManager.AddTitle( kPacket_.m_iTitleID, kPacket_.m_wstrEndDate, tCTime, bInfinity ) )
		{
			KEGS_REWARD_TITLE_NOT kPacketNot;
			kPacketNot.m_kTitle.m_iTitleID	  = kPacket_.m_iTitleID;
			kPacketNot.m_kTitle.m_wstrEndDate = kPacket_.m_wstrEndDate;
			kPacketNot.m_kTitle.m_kCTime	  = tCTime;
			kPacketNot.m_kTitle.m_bInfinity	  = bInfinity;
			SendPacket( EGS_REWARD_TITLE_NOT, kPacketNot );

			// 선행미션 완료로 인해 오픈되는 미션이 있는지 검사
			m_kUserTitleManager.CheckNewMission( GetThisPtr<KGSUser>() );
		}
	}
}
//}}

//{{ 2009. 4. 8  최육사		닉네임 변경
IMPL_ON_FUNC( DBE_DELETE_NICK_NAME_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"닉네임 삭제 실패? 있을수 없는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		// 닉네임 변경 실패하면 안된다!
		return;
	}


	// 닉네임 변경 성공했으므로 Log를 남기자!
	KELOG_DELETE_NICK_NAME_NOT kPacketLog;
	kPacketLog.m_iUnitUID = GetCharUID();
	kPacketLog.m_wstrNickName = GetCharName();
	SendToLogDB( ELOG_DELETE_NICK_NAME_NOT, kPacketLog );

	// 닉네임이 __DELETED__로 바뀌면 캐릭터 선택창으로 이동하라는 이벤트를 보내자!
    SendID( EGS_DELETE_NICK_NAME_SUCCESS_NOT );
}
//}}

//{{ 2009. 8. 4  최육사		봉인스킬
IMPL_ON_FUNC( DBE_UNSEAL_SKILL_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 스킬 봉인 해제
		m_kSkillTree.SkillUnseal( kPacket_.m_iSkillID );

		// 클라이언트에게 봉인 해제 알리기
		KEGS_UNSEAL_SKILL_NOT kNot;
		kNot.m_iSkillID = kPacket_.m_iSkillID;
		SendPacket( EGS_UNSEAL_SKILL_NOT, kNot );
	}
	else
	{
		START_LOG( cerr, L"스킬 봉인해제 실패!" )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iSkillID )
			<< END_LOG;
	}
}
//}}

//{{ 2009. 5. 8  최육사		실시간 레벨업
_IMPL_ON_FUNC( ERM_CHAR_LEVEL_UP_NOT, KEGS_CHAR_LEVEL_UP_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

    SendPacket( EGS_CHAR_LEVEL_UP_NOT, kPacket_ );
}
//}}

//{{ 2009. 6. 3  최육사		확성기
IMPL_ON_FUNC( EGS_USE_MEGAPHONE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_USE_MEGAPHONE_ACK kAck;


		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_USE_MEGAPHONE_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_USE_MEGAPHONE_ACK kPacket;

	// 확성기 아이템이 맞는지 검사
	const int iItemID = m_kInventory.GetItemID( kPacket_.m_iItemUID );
	if( iItemID != CXSLItem::CI_NASOD_MEGAPHONE  &&  iItemID != CXSLItem::CI_HIGH_NASOD_MEGAPHONE
		)
	{
		START_LOG( cerr, L"확성기 아이템이 아닙니다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_ITEM_04;
		SendPacket( EGS_USE_MEGAPHONE_ACK, kPacket );
		return;
	}

	// 메시지 길이 검사 : multi byte로 변환하여 체크
	std::string strMessage = KncUtil::toNarrowString( kPacket_.m_wstrMessage );

	if( strMessage.size() > 90 )
	{
		START_LOG( cerr, L"메시지 길이 제한. 90바이트 제한을 클라이언트에서 검사했을텐데?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_wstrMessage.size() )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_ITEM_13;
		SendPacket( EGS_USE_MEGAPHONE_ACK, kPacket );
		return;
	}

	// 아이템 사용
	if( m_kInventory.UseItemInInventory( kPacket_.m_iItemUID, kPacket.m_vecInventorySlotInfo ) == false )
	{
		START_LOG( cerr, L"확성기 아이템 사용 실패!" )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::GetLastError();
		SendPacket( EGS_USE_MEGAPHONE_ACK, kPacket );
		return;
	}

	kPacket.m_iOK = NetError::NET_OK;
	SendPacket( EGS_USE_MEGAPHONE_ACK, kPacket );

	KEGS_MEGAPHONE_MESSAGE_NOT kNot;
	kNot.m_wstrSenderNickName = GetCharName();
	kNot.m_iSenderChannelID = SiKChannelManager()->GetChannelID();
	kNot.m_iSenderLocation = GetMapID();
	kNot.m_wstrMessage = kPacket_.m_wstrMessage;

	// 센터서버로 브로드캐스팅을 위해 전송
	switch( iItemID )
	{
	case CXSLItem::CI_NASOD_MEGAPHONE:
		{
			KncSend( PI_GS_USER, GetUID(), PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, ECN_MEGAPHONE_MESSAGE_NOT, kNot );
		}
		break;

	case CXSLItem::CI_HIGH_NASOD_MEGAPHONE:
		{
			SendToCnServer( ECN_MEGAPHONE_MESSAGE_NOT, kNot );
		}
		break;
	}

	//{{ 2009. 7. 22  최육사	메가폰 로그
	KELOG_MEGAPHONE_LOG_NOT kPacketLog;
	kPacketLog.m_iUnitUID = GetCharUID();
	kPacketLog.m_iItemID = iItemID;
	SendToLogDB( ELOG_MEGAPHONE_LOG_NOT, kPacketLog );
	//}}

	//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
	//}}
}

//{{ 2010. 02. 01  최육사	메가폰 상태 제한
_IMPL_ON_FUNC( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT )
{
	VERIFY_STATE_LOG( clog2, ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_MEGAPHONE_MESSAGE_NOT, kPacket_ );
}
//}}

//{{ 2009. 12. 8  최육사	크리스마스이벤트
IMPL_ON_FUNC_NOPARAM( EGS_CHECK_TIME_EVENT_COMPLETE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_CHECK_TIME_EVENT_COMPLETE_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_CHECK_TIME_EVENT_COMPLETE_ACK kPacket;

	// 크리스마스 이벤트가 완료되었는지 확인
	if( m_kUserEventManager.IsCompleteDirectRewardEvent() == false )
	{
		kPacket.m_iOK = NetError::ERR_CUMULATIVE_TIME_EVENT_00;
		SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kPacket );
		return;
	}

    // 보상이 인벤토리에 들어갈수있는지 검사
	std::map< int, int > mapCheckResultItem;
	mapCheckResultItem.insert( std::make_pair( CXSLItem::EI_XMAS_EVENT_ITEM, 1 ) );

	if( m_kInventory.IsEnoughSpaceExist( mapCheckResultItem ) == false )
	{
		kPacket.m_iOK = NetError::ERR_CUMULATIVE_TIME_EVENT_01;
		SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kPacket );
		return;
	}

	std::map< int, KItemInfo > mapResultItem;
	{
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( CXSLItem::EI_XMAS_EVENT_ITEM );
		if( pItemTemplet == NULL )
		{
			kPacket.m_iOK = NetError::ERR_CUMULATIVE_TIME_EVENT_02;
			SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kPacket );
			return;
		}

		KItemInfo kInfo;
		kInfo.m_iItemID		= CXSLItem::EI_XMAS_EVENT_ITEM;
		kInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
		kInfo.m_iQuantity	= 1;
		kInfo.m_sEndurance	= ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;

		mapResultItem.insert( std::make_pair( kInfo.m_iItemID, kInfo ) );
	}

	KDBE_CHECK_TIME_EVENT_COMPLETE_REQ kPacketToDB;

	if( m_kInventory.PrepareInsert( mapResultItem, kPacketToDB.m_mapInsertedItem, kPacketToDB.m_vecUpdatedInventorySlot, kPacketToDB.m_vecItemInfo ) == false )
	{
		kPacket.m_iOK = NetError::ERR_CUMULATIVE_TIME_EVENT_01;
		SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kPacket );
		return;
	}

	// 해당 이벤트 정보를 삭제하고 DB업데이트 하러 가자!
	m_kUserEventManager.DeleteDirectRewardAndGetDBUpdateInfo( kPacketToDB.m_vecCompletedEvent );

	// DB로 업데이트 하러 가자!
	kPacketToDB.m_iUnitUID = GetCharUID();
    m_kInventory.FlushQuantityChange( kPacketToDB.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.FlushDeletedItem( kPacketToDB.m_kItemQuantityUpdate.m_vecDeleted );
	SendToGameDB( DBE_CHECK_TIME_EVENT_COMPLETE_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_CHECK_TIME_EVENT_COMPLETE_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KEGS_CHECK_TIME_EVENT_COMPLETE_ACK kPacket;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"이벤트 완료 정보 DB업데이트 실패! 일어나서는 안되는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;
	}

	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iRewardCount = kPacket_.m_iRewardCount;
	m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
	m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kPacket.m_vecInventorySlotInfo );
	kPacket.m_vecInventorySlotInfo.insert( kPacket.m_vecInventorySlotInfo.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );
	SendPacket( EGS_CHECK_TIME_EVENT_COMPLETE_ACK, kPacket );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 재시작할 이벤트가 있는지 찾아서 재시작 하자!
        m_kUserEventManager.RestartDirectRewardEvent( GetThisPtr<KGSUser>(), kPacket_.m_vecRestartEvent );
	}
}
//}}

//{{ 2010. 01. 07  최육사	추천인리스트
IMPL_ON_FUNC( EGS_GET_RECOMMEND_USER_LIST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	// 추천인 리스트가 초기화 되었는지 확인
	if( m_kUserRecommendManager.IsInitRecommendUserList() == false )
	{
		KDBE_GET_RECOMMEND_USER_LIST_REQ kPacketToDB;
		kPacketToDB.m_iUnitUID = GetCharUID();
		SendToGameDB( DBE_GET_RECOMMEND_USER_LIST_REQ, kPacketToDB );
		return;
	}

	//{{ 2012. 01. 17  김민성	추천인 리스트 최대 전송 패킷 크기 조절
	// 추천인 리스트 얻기
	KDBE_GET_RECOMMEND_USER_LIST_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_uiViewPage = kPacket_.m_uiViewPage;

	m_kUserRecommendManager.GetRecommendUserList( kPacket.m_uiViewPage, kPacket.m_uiTotalPage, kPacket.m_vecRecommendUserList );

	// 로그인서버로 가서 현재 접속중인 추천인인지 체크한다.
	SendToLoginServer( ELG_GET_RECOMMEND_USER_LIST_REQ, kPacket );
	//}}

}

IMPL_ON_FUNC( DBE_GET_RECOMMEND_USER_LIST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"추천인 리스트 받아오기 실패!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		KEGS_GET_RECOMMEND_USER_LIST_ACK kPacket;
		kPacket.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_GET_RECOMMEND_USER_LIST_ACK, kPacket );
		return;
	}

	//{{ 2012. 01. 17  김민성	추천인 리스트 최대 전송 패킷 크기 조절
	// 추천인 리스트 초기화
	m_kUserRecommendManager.SetRecommendUserList( kPacket_.m_vecRecommendUserList );

	// 추천인 리스트 얻기
	KDBE_GET_RECOMMEND_USER_LIST_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;
	kPacket.m_uiViewPage = 1;

	m_kUserRecommendManager.GetRecommendUserList( kPacket.m_uiViewPage, kPacket.m_uiTotalPage, kPacket.m_vecRecommendUserList );

	// 로그인서버로 가서 현재 접속중인 추천인인지 체크한다.
	SendToLoginServer( ELG_GET_RECOMMEND_USER_LIST_REQ, kPacket );
	//}}

}

_IMPL_ON_FUNC( ELG_GET_RECOMMEND_USER_LIST_ACK, KDBE_GET_RECOMMEND_USER_LIST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	//{{ 2012. 01. 17  김민성	추천인 리스트 최대 전송 패킷 크기 조절
	KEGS_GET_RECOMMEND_USER_LIST_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_uiViewPage = kPacket_.m_uiViewPage;
	kPacket.m_uiTotalPage = kPacket_.m_uiTotalPage;
	kPacket.m_vecRecommendUserList = kPacket_.m_vecRecommendUserList;
	//}}

	SendPacket( EGS_GET_RECOMMEND_USER_LIST_ACK, kPacket );

}

IMPL_ON_FUNC( ELG_RECOMMEND_USER_INFO_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

    // 추천인 정보 수정
	m_kUserRecommendManager.ModifyRecommendUser( kPacket_ );

	// 클라이언트로 알림
	KEGS_RECOMMEND_USER_INFO_NOT kPacket;
	kPacket.m_iRecommendUnitUID = kPacket_.m_iRecommendUnitUID;
	kPacket.m_cType = kPacket_.m_cType;
	kPacket.m_iData = kPacket_.m_iData;
    SendPacket( EGS_RECOMMEND_USER_INFO_NOT, kPacket );
}
//}}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

//{{ 2010. 03. 22  최육사	기술의 노트

IMPL_ON_FUNC( DBE_EXPAND_SKILL_NOTE_PAGE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

    // 기술의 노트 슬롯 확장 결과
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"기술의 노트 슬롯 확장 실패! 절대 일어나서는 안되는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( kPacket_.m_cExpandedMaxPageNum )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		// 롤백 처리
		m_kSkillTree.UpdateSkillNoteMaxPageNum( 0 );
		return;
	}

    // 확장된 페이지 수를 업데이트 한다.
	m_kSkillTree.UpdateSkillNoteMaxPageNum( kPacket_.m_cExpandedMaxPageNum );

	// 클라이언트에게 확장 결과를 알린다.
	KEGS_EXPAND_SKILL_NOTE_PAGE_NUM_NOT kPacketNot;
	kPacketNot.m_cExpandedPageNum = m_kSkillTree.GetSkillNoteMaxPageNum();
	SendPacket( EGS_EXPAND_SKILL_NOTE_PAGE_NUM_NOT, kPacketNot );
}

IMPL_ON_FUNC( EGS_REG_SKILL_NOTE_MEMO_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_REG_SKILL_NOTE_MEMO_REQ, EGS_REG_SKILL_NOTE_MEMO_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_REG_SKILL_NOTE_MEMO_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_REG_SKILL_NOTE_MEMO_ACK kPacket;

	// 존재하는 아이템인지 확인
	if( m_kInventory.IsExist( kPacket_.m_iItemUID ) == false )
	{
		START_LOG( cerr, L"존재하지 않는 아이템을 기술의 노트에 사용하려 했다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_NOTE_00;
		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
		return;
	}

	// 메모ID얻기 ( ItemID )
	const int iMemoID = m_kInventory.GetItemID( kPacket_.m_iItemUID );

	// 기술의 노트를 보유하고 있는지 확인한다.
	if( m_kSkillTree.IsHaveSkillNote() == false )
	{
		START_LOG( cerr, L"기술의 노트를 보유하고 있지 않은데 메모를 등록하려 했음." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_NOTE_04;
		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
		return;
	}

	// 등록하려는 페이지가 존재하는 페이지 인지 확인한다.
	if( m_kSkillTree.IsExistSkillNotePage( kPacket_.m_cSkillNotePageNum ) == false )
	{
		START_LOG( cerr, L"존재하지 않는 페이지에 사용하려 했다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( kPacket_.m_cSkillNotePageNum )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_NOTE_01;
		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
		return;
	}

	// 이미 등록된 메모인지 찾아본다.
	if( m_kSkillTree.IsExistSkillNoteMemoID( iMemoID ) == true )
	{
		START_LOG( cerr, L"이미 등록된 메모를 다시 등록하려 했다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( iMemoID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_NOTE_02;
		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
		return;
	}

 //   // 강제로 덮어씌우는건가?
	//if( !kPacket_.m_bForce )
	//{
	//	// 등록하려는 페이지에 이미 메모가 등록되어 있는지 찾아본다.
	//	if( m_kSkillTree.IsEmptySkillNotePage( kPacket_.m_cSkillNotePageNum ) == false )
	//	{
	//		kPacket.m_iOK = NetError::ERR_UNKNOWN;
	//		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
	//		return;
	//	}
	//}

	// 강제로 덮어씌우기를 동의했으면 인벤토리에서 아이템을 소비한다.
	KDBE_REG_SKILL_NOTE_MEMO_REQ kPacketToDB;

	if( m_kInventory.UseItemInInventory( kPacket_.m_iItemUID, kPacketToDB.m_vecInventorySlotInfo ) == false )
	{
		START_LOG( cerr, L"기술의 노트 아이템 사용 실패!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::GetLastError();
		SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket );
		return;
	}

	// DB로 업데이트 하러 간다!
	kPacketToDB.m_iUnitUID = GetCharUID();
	kPacketToDB.m_cPageNum = kPacket_.m_cSkillNotePageNum;
	kPacketToDB.m_iMemoID = iMemoID;
	SendToGameDB( DBE_REG_SKILL_NOTE_MEMO_REQ, kPacketToDB );
}

_IMPL_ON_FUNC( DBE_REG_SKILL_NOTE_MEMO_ACK, KEGS_REG_SKILL_NOTE_MEMO_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_REG_SKILL_NOTE_MEMO_ACK );
	//}}

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"기술의 노트 메모 업데이트 실패! 절대 일어나서는 안되는 에러!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOGc( kPacket_.m_cSkillNotePageNum )
			<< BUILD_LOG( kPacket_.m_iMemoID )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;
	}
	else
	{
		// 기술의 노트 업데이트
		m_kSkillTree.UpdateSkillNoteMemo( kPacket_.m_cSkillNotePageNum, kPacket_.m_iMemoID );

		//{{ 2012. 10. 29	박세훈	배틀필드 시스템
		m_kSkillTree.GetSkillNote( kPacket_.m_vecSkillNote );
		//}}

		// 방이라면 방안에 있는 유저들에게 알리자!
		if( GetStateID() == KGSFSM::S_ROOM )
		{
			switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
			{
			case CXSLRoom::RT_PVP:
			case CXSLRoom::RT_DUNGEON:
			case CXSLRoom::RT_TRAININGCENTER:
				//{{ 2012. 10. 29	박세훈	배틀필드 시스템
			case CXSLRoom::RT_BATTLE_FIELD:
				//}}
				{
					KERM_CHANGE_SKILL_INFO_IN_ROOM_NOT kPacketNot;
					GetRoomUserInfo( kPacketNot );
					SendToCnRoom( ERM_CHANGE_SKILL_INFO_IN_ROOM_NOT, kPacketNot );
				}
				break;
			default:
				break;
			}
		}
	}

	SendPacket( EGS_REG_SKILL_NOTE_MEMO_ACK, kPacket_ );
}

//}}

//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
IMPL_ON_FUNC( EGS_RESTORE_UNIT_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_RESTORE_UNIT_REQ, EGS_RESTORE_UNIT_ACK );

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	if( IsUseSecurity() == true )
	{
		if( GetSuccessSecondSecurity() == false )	// 2차 보안을 사용 중인데 인증이 되지 않았다면 실패
		{
			START_LOG( cerr, L"2차 보안을 사용 중인데 인증도 되기 전에 유닉 선택을 하려 한다." )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			KEGS_DELETE_UNIT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_VERIFY_21;
			kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
			kPacket.m_iGuildUID = 0;
			SendPacket( EGS_DELETE_UNIT_ACK, kPacket );
			return;
		}
	}
	//}}

	kPacket_.m_iUserUID = GetUID(); //2012.03.05 lygan_조성욱 // 캐릭터 삭제 할때 userUID도 필요하게 변경됨에 따라 추가

	SendToGameDB( DBE_GAME_RESTORE_UNIT_REQ, kPacket_ );
}

_IMPL_ON_FUNC( DBE_GAME_RESTORE_UNIT_ACK, KEGS_RESTORE_UNIT_ACK )
{
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_RESTORE_UNIT_ACK );


	SendPacket( EGS_RESTORE_UNIT_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_FINAL_DELETE_UNIT_REQ )
{
	// EGS_DELETE_UNIT_REQ 은 삭제 대기 상태로 들어가는 Delete 임
	// EGS_FINAL_DELETE_UNIT_REQ 는 최종 삭제 요청 Delete 임

	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_FINAL_DELETE_UNIT_REQ, EGS_FINAL_DELETE_UNIT_ACK );

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	if( IsUseSecurity() == true )
	{
		if( GetSuccessSecondSecurity() == false )	// 2차 보안을 사용 중인데 인증이 되지 않았다면 실패
		{
			START_LOG( cerr, L"2차 보안을 사용 중인데 인증도 되기 전에 유닉 최종 삭제를 하려 한다." )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			KEGS_DELETE_UNIT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_VERIFY_21;
			kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
			kPacket.m_iGuildUID = 0;
			SendPacket( EGS_DELETE_UNIT_ACK, kPacket );
			return;
		}
	}
	//}}
	kPacket_.m_iUserUID = GetUID(); //2012.03.05 lygan_조성욱 // 캐릭터 삭제 할때 userUID도 필요하게 변경됨에 따라 추가

	SendToGameDB( DBE_GAME_FINAL_DELETE_UNIT_REQ, kPacket_ );
}

_IMPL_ON_FUNC( DBE_GAME_FINAL_DELETE_UNIT_ACK, KEGS_FINAL_DELETE_UNIT_ACK )
{
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_FINAL_DELETE_UNIT_ACK );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		SendPacket( EGS_FINAL_DELETE_UNIT_ACK, kPacket_ );
		return;
	}


	SendToAccountDB( DBE_ACCOUNT_DELETE_UNIT_REQ, kPacket_ );
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
_IMPL_ON_FUNC( EGB_UPDATE_WORLD_BUFF_NOT, KEGS_UPDATE_WORLD_BUFF_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 버프 관리 시스템 - 김민성
	std::vector< KBuffInfo > vecActivateBuff;
	std::vector< int > vecDeleteBuff;

	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	//}}

	if( vecActivateBuff.empty() == false || vecDeleteBuff.empty() == false )
	{
		// 로그인 서버 버프 매니저에 업데이트
		KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
		kReq.m_iUnitUID = GetCharUID();
		kReq.m_vecActivateBuff = vecActivateBuff;
		kReq.m_vecDeActivateBuff = vecDeleteBuff;
		SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
	}
	//}
}
//}}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2012. 03. 23	박세훈	어둠의 문 개편
IMPL_ON_FUNC( EGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT )
{
	//{{ 버프 관리 시스템 - 김민성
	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	//}}
	//}
}
//}}


//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
IMPL_ON_FUNC( EGS_KEYBOARD_MAPPING_INFO_WRITE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 4, KGSFSM::S_LOGINED, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_KEYBOARD_MAPPING_INFO_WRITE_REQ, EGS_KEYBOARD_MAPPING_INFO_WRITE_ACK );

	KDBE_KEYBOARD_MAPPING_INFO_WRITE_REQ kPacket;

	kPacket.m_iUserUID = GetUID();
	kPacket.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo = kPacket_.m_mapKeyboardMappingInfo;

	kPacket.m_kKeyboardMappingInfo.m_mapGamePadMappingInfo = kPacket_.m_mapGamePadMappingInfo;

	SendToAccountDB( DBE_KEYBOARD_MAPPING_INFO_WRITE_REQ, kPacket );
}

_IMPL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_WRITE_ACK, KEGS_KEYBOARD_MAPPING_INFO_WRITE_ACK )
{
	SendPacket( EGS_KEYBOARD_MAPPING_INFO_WRITE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( DBE_KEYBOARD_MAPPING_INFO_READ_ACK, KEGS_KEYBOARD_MAPPING_INFO_NOT )
{
	SendPacket( EGS_KEYBOARD_MAPPING_INFO_NOT, kPacket_ );
}
//}}

//{{ 2012. 05. 23	최육사	배틀필드 시스템
IMPL_ON_FUNC( EGS_UPDATE_PLAY_STATUS_NOT )
{
	VERIFY_STATE_WARN( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 플레이 상태 정보 저장!
	bool bChangedHpMp = false;
	KPartyMemberStatus kPartyMemberStatus;

	// 유효한 값을 저장해 두도록 하자 -> 0이상의 값으로 저장
	KStatTable::KUnitStatInfo	sUnitStat;
	SiKStatTable()->GetUnitStat( GetUnitClass(), m_ucLevel, sUnitStat );

	/// kimhc // 김현철 // 해킹 때문에 HP는 기본 HP의 3배
	/// MP는 500 으로 제한 함
	/// 추후 스탯 계산 (MP포함)을 통해 체크 하도록 해야 할 듯...
	kPacket_.m_kGamePlayStatus.m_iMaxHP = std::max( sUnitStat.m_uiHP * 3, static_cast<u_int>(kPacket_.m_kGamePlayStatus.m_iMaxHP));
	kPacket_.m_kGamePlayStatus.m_iCurHP = std::max( kPacket_.m_kGamePlayStatus.m_iCurHP, kPacket_.m_kGamePlayStatus.m_iMaxHP );

	kPacket_.m_kGamePlayStatus.m_iMaxMP = std::max( 0, kPacket_.m_kGamePlayStatus.m_iMaxMP );
	kPacket_.m_kGamePlayStatus.m_iMaxMP = std::min( 500, kPacket_.m_kGamePlayStatus.m_iMaxMP );

	kPacket_.m_kGamePlayStatus.m_iCurMP = std::max( 0, kPacket_.m_kGamePlayStatus.m_iCurMP );
	kPacket_.m_kGamePlayStatus.m_iCurMP = std::min( kPacket_.m_kGamePlayStatus.m_iCurMP, kPacket_.m_kGamePlayStatus.m_iMaxMP );

	kPacket_.m_kGamePlayStatus.m_iCurHyperGage = std::max( 0, kPacket_.m_kGamePlayStatus.m_iCurHyperGage );
	kPacket_.m_kGamePlayStatus.m_cCharAbilType = std::max( 0, static_cast<int>(kPacket_.m_kGamePlayStatus.m_cCharAbilType) );

	m_kUserUnitManager.SetGamePlayStatusAndCheckChangedHpMp( kPacket_.m_kGamePlayStatus, bChangedHpMp, kPartyMemberStatus );

	// HpMp정보가 변경되었고 파티소속 유저라면 파티원들에게도 변경된 hpMp를 전송하자!
	if( bChangedHpMp  &&  GetPartyUID() != 0 )
	{
        SendToCnParty( EPM_UPDATE_GAME_PLAY_STATUS_NOT, kPartyMemberStatus );
	}

	if( CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_BATTLE_FIELD )
	{
		SendToCnRoom( ERM_FIELD_USER_FRAME_INFO_NOT, kPacket_.m_iFrame );
	}
}

_IMPL_ON_FUNC( EPM_BROADCAST_PARTY_MEMBER_STATUS_NOT, KEGS_UPDATE_PARTY_MEMBER_STATUS_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_UPDATE_PARTY_MEMBER_STATUS_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_USE_SKILL_BUFF_IN_PLAY_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() != 0 )
	{
        SendToCnRoom( ERM_USE_SKILL_BUFF_IN_PLAY_NOT, kPacket_ );
	}
}

_IMPL_ON_FUNC( ERM_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT, KEGS_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT, kPacket_ );
}
//}}

//{{ 2012. 05. 29	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
IMPL_ON_FUNC( EGS_CHAT_OPTION_INFO_WRITE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_CHAT_OPTION_INFO_WRITE_REQ, EGS_CHAT_OPTION_INFO_WRITE_ACK );

	KDBE_CHAT_OPTION_INFO_WRITE_REQ kPacket;

	kPacket.m_iUserUID = GetUID();
	kPacket.m_cIndex = kPacket_.m_cIndex;
	kPacket.m_bValue = kPacket_.m_bValue;
	SendToAccountDB( DBE_CHAT_OPTION_INFO_WRITE_REQ, kPacket );
}

_IMPL_ON_FUNC( DBE_CHAT_OPTION_INFO_WRITE_ACK, KEGS_CHAT_OPTION_INFO_WRITE_ACK )
{
	SendPacket( EGS_CHAT_OPTION_INFO_WRITE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( DBE_CHAT_OPTION_INFO_READ_ACK, KEGS_CHAT_OPTION_INFO_NOT )
{
	SendPacket( EGS_CHAT_OPTION_INFO_NOT, kPacket_ );
}
//}}



//{{ 버프 관리 시스템 - 김민성
IMPL_ON_FUNC( EGS_UPDATE_USER_UNIT_BUFF_INFO_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"현재 방에 입장해있는 상태가 아닌데 버프정보를 보내려고 한다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_vecBuffFactor.size() )
			<< END_LOG;
		return;
	}

	SendToCnRoom( ERM_UPDATE_USER_UNIT_BUFF_INFO_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT, KEGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_UPDATE_NPC_UNIT_BUFF_INFO_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( GetRoomUID() == 0 )
	{
		START_LOG( cerr, L"현재 방에 입장해있는 상태가 아닌데 버프정보를 보내려고 한다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_vecNpcUnitBuff.size() )
			<< END_LOG;
		return;
	}

	SendToCnRoom( ERM_UPDATE_NPC_UNIT_BUFF_INFO_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT, KEGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT, kPacket_ );
}

IMPL_ON_FUNC( ELG_UPDATE_USER_UNIT_BUFF_INFO_ACK )
{
	VERIFY_STATE_LOG( clog, ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	m_kUserBuffManager.Init( kPacket_.m_kUnitBuffInfo );

	START_LOG( clog, L"버프 정보가 새로 갱신 되었습니다." )
		<< BUILD_LOG( GetCharName() )
		<< END_LOG;

	SendUpdateBuffInfo();

	//////////////////////////////////////////////////////////////////////////
	//std::vector< int > vecTest;
	//m_kUserBuffManager.GetBuffIDList( vecTest );
	//START_LOG( cout, L"[버프체크] 현재 발동된 버프가 어떤게 있는가?------------------------------" );
	//for( const int iBuffID : vecTest )
	//{
	//	START_LOG( cout, L"[버프체크] 현재 발동된 버프" )
	//		<< BUILD_LOG( iBuffID );
	//}
	//////////////////////////////////////////////////////////////////////////
}
//}}

//{{ QUEST 개편 - 김민성
IMPL_ON_FUNC( ERM_UPDATE_QUEST_INFO_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_vecNPC.empty() == false )
		m_kUserQuestManager.Handler_OnFindNPC( GetThisPtr<KGSUser>(), kPacket_.m_vecNPC, true );
}
//}}

//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
//}}

//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
//}}

//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
//}}

//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
IMPL_ON_FUNC( DBE_NEW_ACCOUNT_QUEST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// Account DB에서 정상처리되고 온 경우..
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// Game DB 에 아이템 기록 하러 가자~
		SendToGameDB( DBE_NEW_ACCOUNT_QUEST_GAME_DB_REQ, kPacket_.m_kQuestReq );
	}
}

_IMPL_ON_FUNC( DBE_NEW_ACCOUNT_QUEST_GAME_DB_ACK, KDBE_NEW_QUEST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 계정 퀘스트는 자동 수락 이므로
	// EGS_NEW_QUEST_ACK 를 보내지 않는다.

	// Account DB에서 계정 퀘스트 정상처리되고,
	// Game DB에서 아이템 정상처리되고 온 경우..
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KQuestInstance kQuest;
		kQuest.m_iID			= kPacket_.m_iQuestID;
		kQuest.m_OwnorUnitUID	= GetCharUID();

		const CXSLQuestManager::QuestTemplet* QuestTem = SiCXSLQuestManager()->GetAccountQuestTemplet( kPacket_.m_iQuestID );
		if( QuestTem == NULL )
		{
			START_LOG( cerr, L"Account QUEST TEMPLET 얻어오기 실패.!" )
				<< BUILD_LOG( kPacket_.m_iQuestID )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() )
				<< END_LOG;
			return;
		}

		KSubQuestInstance kSubQuest;
		for( const int iSubQuestID : QuestTem->m_vecSubQuest )
		{
			kSubQuest.m_iID = iSubQuestID;

			const CXSLQuestManager::SubQuestTemplet* pSubQuestTemplet = SiCXSLQuestManager()->GetSubQuestTemplet( iSubQuestID );
			if( pSubQuestTemplet == NULL )
			{
				START_LOG( cerr, L"SUB QUEST TEMPLET 얻어오기 실패.!" )
					<< BUILD_LOG( iSubQuestID )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetCharName() )
					<< END_LOG;
				continue;
			}

			kQuest.m_vecSubQuestInstance.push_back( kSubQuest );
		}

		m_kUserAccountQuestManager.AddAccountQuest( kQuest );

		KEGS_NEW_QUEST_NOT kNot;
		kNot.m_QuestInst = kQuest;

		m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
		m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kNot.m_vecUpdatedInventorySlot );
		kNot.m_vecUpdatedInventorySlot.insert( kNot.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );

		SendPacket( EGS_NEW_QUEST_NOT, kNot );


		for( const int iSubQuestID : QuestTem->m_vecSubQuest )
		{
			const CXSLQuestManager::SubQuestTemplet* pSubQuestTemplet = SiCXSLQuestManager()->GetSubQuestTemplet( iSubQuestID );
			if( IS_NULL( pSubQuestTemplet ) )
			{
				START_LOG( cerr, L"존재 하지 않는 서브 퀘스트 입니다!" )
					<< BUILD_LOG( iSubQuestID )
					<< END_LOG;
				continue;
			}

			switch( pSubQuestTemplet->m_eClearType )
			{
			case CXSLQuestManager::SQT_CHAR_LEVEL_UP:
				{
					// 필드 입장에 대한 퀘스트 완료 체크!
					m_kUserAccountQuestManager.Handler_CharaterLevelUp_Account( GetThisPtr<KGSUser>() );
				}
				break;
			default:
				break;
			}
		}
	}
}

IMPL_ON_FUNC( DBE_ACCOUNT_QUEST_COMPLETE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		KEGS_QUEST_COMPLETE_ACK	kPacket;
		kPacket.m_iOK				 = kPacket_.m_iOK;
		kPacket.m_kCompleteQuestInfo.m_iQuestID = kPacket_.m_kQuestReq.m_iQuestID;
		kPacket.m_kCompleteQuestInfo.m_iCompleteCount = 0;
		kPacket.m_kCompleteQuestInfo.m_tCompleteDate = 0;
		SendPacket( EGS_QUEST_COMPLETE_ACK, kPacket );
		return;
	}

	// Game DB 에 아이템 기록 하러 가자~
	SendToGameDB( DBE_ACCOUNT_QUEST_COMPLETE_GAME_DB_REQ, kPacket_ );
}

_IMPL_ON_FUNC( DBE_ACCOUNT_QUEST_COMPLETE_GAME_DB_ACK, KDBE_QUEST_COMPLETE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//db에서 완료체크를 받고 난다음
	m_kUserAccountQuestManager.Handler_DBE_QUEST_COMPLETE_ACK_Account( kPacket_, GetThisPtr<KGSUser>() );
	//- 보상으로 얻은 유정 정보 client에게 알려주기 유닛 업데이트를 시켜야 하나 말아야 하나 흠~~

	// 일일통계 퀘스트 습득 아이템
	std::map< int, KItemInfo >::iterator mit = kPacket_.m_mapInsertedItem.begin();
	for( ; mit != kPacket_.m_mapInsertedItem.end(); ++mit )
	{
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( mit->first );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_QUEST, mit->second.m_iQuantity );
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_QUEST, mit->second.m_iQuantity );
	}
}
//}}

//{{ 2013. 02. 07   플루오르 스톤 강화 이벤트 실시간 적용 - 김민성
IMPL_ON_FUNC_NOPARAM( EGS_SUPPORT_MATERIAL_EVENT_TIME_REQ )
{
	KEGS_SUPPORT_MATERIAL_EVENT_TIME_ACK kAck;
	kAck = SiKGameSysVal()->IsEnchantEvent();
	SendPacket( EGS_SUPPORT_MATERIAL_EVENT_TIME_ACK, kAck );
}
//}

// 2011.05.05 lygan_조성욱 // 중국용 커뮤니티탭 유저 정보 보기
IMPL_ON_FUNC( EGS_USER_COMMUNITY_SURVEY_NEW_REQ  )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//자기 자신의 정보를 요청한 경우.
	if( x2::platform::IEquals(GetCharName(), kPacket_.m_wUnitNickName))
	{
		KEGS_USER_COMMUNITY_SURVEY_NEW_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		SendPacket( EGS_USER_COMMUNITY_SURVEY_NEW_ACK , kPacket );
		m_cCommunity_Tap_type = 0;
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wUnitNickName ) )
	{
		KEGS_USER_COMMUNITY_SURVEY_NEW_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_USER_COMMUNITY_SURVEY_NEW_ACK , kPacket );
		m_cCommunity_Tap_type = 0;
		return;
	}

	if( m_kTimer[TM_SEARCH_UNIT_REFRESH].elapsed() < (double)SEARCH_UNIT_REFRESH_TIME_LIMIT)
	{
		KEGS_USER_COMMUNITY_SURVEY_NEW_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_01;
		SendPacket( EGS_USER_COMMUNITY_SURVEY_NEW_ACK , kPacket );
		m_cCommunity_Tap_type = 0;

		m_kTimer[TM_SEARCH_UNIT_REFRESH].restart();
		return;
	}

	KELG_SEARCH_UNIT_REQ kPacketReq;
	kPacketReq.m_usEventID = EGS_USER_COMMUNITY_SURVEY_NEW_REQ;
	m_cCommunity_Tap_type = kPacket_.m_cCommunity_Tab_Type;
	kPacketReq.m_wstrNickName = kPacket_.m_wUnitNickName;
	SendToLoginServer( ELG_SEARCH_UNIT_REQ, kPacketReq );

}

IMPL_ON_FUNC( EGS_CATCH_HACKUSER_INFO_NOT )
{
	SendToLogDB( DBE_CATCH_HACKUSER_INTO_NOT, kPacket_);
}

IMPL_ON_FUNC_NOPARAM( EGS_GET_ADVERTISEMENT_EVENT_REQ )
{
	KEGS_GET_ADVERTISEMENT_EVENT_ACK kPacket;

	m_kUserEventManager.GetAdvertisementURL(kPacket.m_vecAdvertisementURL);
	SendPacket( EGS_GET_ADVERTISEMENT_EVENT_ACK, kPacket );
}

//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
//}}

IMPL_ON_FUNC( EGS_WARP_BY_BUTTON_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_WARP_BY_BUTTON_REQ, EGS_WARP_BY_BUTTON_ACK );

	KEGS_WARP_BY_BUTTON_ACK kPacketAck;
	kPacketAck.m_iOK = NetError::ERR_UNKNOWN;

	int iWarpPointMapID = 0;

	//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
	int iTempLevel = 0;
	int iTempDungeonID = 0;
	if( CheckEnterTheVillage( kPacket_.m_iCurrentVillageWarpIndex , iTempLevel, iTempDungeonID ) == false )
		//}}
	{
		kPacketAck.m_iOK = NetError::ERR_USE_ITEM_IN_INVENTORY_05;
		SendPacket( EGS_WARP_BY_BUTTON_ACK, kPacketAck );
		return;
	}

	iWarpPointMapID = static_cast<int>( kPacket_.m_iCurrentVillageWarpIndex );

	if( true == kPacket_.m_bFreeWarp )
	{
		// 기간 다시 한번 검사
		KUnitInfo kUnitInfo;
		GetUnitInfo(kUnitInfo);
		CTime ctVipTime = kUnitInfo.m_trWarpVipEndData;
		if( ctVipTime < CTime::GetCurrentTime() )
		{
			START_LOG(cerr, L"코보 VIP 기간 완료. 패킷 속도가 느리면 발생 할 수도 있다.")
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( ctVipTime.Format(_T( "%Y-%m-%d %H:%M:%S" )).GetString())
				<< BUILD_LOG( ctVipTime.Format(_T( "%Y-%m-%d %H:%M:%S" )).GetString())
				<< END_LOG;

			// VIP 기간이 지났습니다. 다시 한번 시도해 주세요
			kPacketAck.m_iOK = NetError::ERR_COBO_EXPRESS_01;
			SendPacket( EGS_WARP_BY_BUTTON_ACK, kPacketAck );
			return;
		}
	}
	else
	{
		if( ( GetMapID() == iWarpPointMapID )
			|| ( SiCXSLMapData()->CheckCOBOExpressTicketMapID( GetMapID() ) == 0 )
			|| ( SiCXSLMapData()->CheckCOBOExpressTicketMapID( iWarpPointMapID ) == 0 )
			)
		{
			kPacketAck.m_iOK = NetError::ERR_UNKNOWN;
			SendPacket( EGS_WARP_BY_BUTTON_ACK, kPacketAck );
			return;
		}
		else
		{
			int iCOBOExpressTicketCost = SiCXSLMapData()->ComputeCOBOExpressTicketCost( GetMapID(), iWarpPointMapID, GetLevel() );
			if( GetED() < iCOBOExpressTicketCost )
			{
				kPacketAck.m_iOK = NetError::ERR_USE_ITEM_IN_INVENTORY_08;
				SendPacket( EGS_WARP_BY_BUTTON_ACK, kPacketAck );
				return;
			}

			//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
			DecreaseED( iCOBOExpressTicketCost );
			//}}
		}
	}

	// 이동 성공
	kPacketAck.m_iOK = NetError::NET_OK;
	kPacketAck.m_iED = GetED();
	kPacketAck.m_iWarpPointMapID = iWarpPointMapID;

	SendPacket( EGS_WARP_BY_BUTTON_ACK, kPacketAck );
}


//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
IMPL_ON_FUNC( EGS_FORCE_RELAY_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( ( SiKGameSysVal()->GetLanBugOutCheck() == true ) &&
		( 0 < GetRoomUID() )
		)
	{
		SendToCnRoom( ERM_FORCE_RELAY_NOT, kPacket_ );
	}
}

_IMPL_ON_FUNC( ERM_FORCE_RELAY_NOT, KEGS_FORCE_RELAY_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_FORCE_RELAY_NOT, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( ERM_UDP_KICK_STATE_CHANGE_FIELD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	KEGS_UDP_KICK_STATE_CHANGE_FIELD_NOT kPacket;
	kPacket.m_iOK	= NetError::ERR_UDP_01;
	SendPacket( EGS_UDP_KICK_STATE_CHANGE_FIELD_NOT, kPacket );
}
//}}


IMPL_ON_FUNC( EGS_SKILL_USE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iSkillID == -1 )
		return;

	// 던전, 대전, 배틀필드 에서만 체크합니다.
	if( CXSLRoom::RT_DUNGEON != CXSLRoom::GetRoomType( GetRoomUID() ) &&
		CXSLRoom::RT_PVP != CXSLRoom::GetRoomType( GetRoomUID() ) &&
		CXSLRoom::RT_BATTLE_FIELD != CXSLRoom::GetRoomType( GetRoomUID() ) )
		return;

	if( CXSLDungeon::IsTutorialDungeon( m_kUserDungeonManager.GetDungeonID() ) == true ||
		CXSLDungeon::IsHenirDungeon( m_kUserDungeonManager.GetDungeonID() ) == true ||
		CXSLDungeon::IsRubenDungeon( m_kUserDungeonManager.GetDungeonID() ) == true ||
		CXSLDungeon::IsTCDungeon( m_kUserDungeonManager.GetDungeonID() ) == true)
		return;

	m_kUserQuestManager.Handler_EGS_SKILL_USE_REQ( kPacket_.m_iSkillID, GetThisPtr<KGSUser>() );
}





IMPL_ON_FUNC( DBE_CHANGE_PET_ID_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KDBE_CHANGE_PET_ID_LOG_NOT kPacketLog;
	kPacketLog.m_iUnitUID		= GetCharUID();
	kPacketLog.m_iPetUID		= kPacket_.m_iPetUID;
	kPacketLog.m_iBeforePetID	= kPacket_.m_iBeforePetID;
	kPacketLog.m_iAfterPetID	= kPacket_.m_iAfterPetID;
	SendToLogDB( DBE_CHANGE_PET_ID_LOG_NOT, kPacketLog );

	KEGS_CHANGE_PET_ID_NOT kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iPetUID = kPacket_.m_iPetUID;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		kPacket.m_iOK = NetError::ERR_PET_25;

		START_LOG( cerr, L"할로윈 요정 펫 변신 물약 DB 변경 실패!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iPetUID )
			<< BUILD_LOG( kPacket_.m_iBeforePetID )
			<< BUILD_LOG( kPacket_.m_iAfterPetID )
			<< END_LOG;
	}

	SendPacket( EGS_CHANGE_PET_ID_NOT, kPacket );
}





IMPL_ON_FUNC( EGS_2013_EVENT_MISSION_COMPLETE_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_2013_EVENT_MISSION_COMPLETE_ACK );

	IF_EVENT_ENABLED( CEI_OLD_YEAR_EVENT_2013 )
	{
	}
	ELSE
	{
		START_LOG( cwarn, L"이벤트가 끝났는데 완료 요청? 해커! 또는 약간 늦게 요청한 애들" )
			<< END_LOG;

		KEGS_2013_EVENT_MISSION_COMPLETE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_QUEST_09;
		SendPacket( EGS_2013_EVENT_MISSION_COMPLETE_ACK, kPacket );
		return;
	}

	KEGS_2013_EVENT_MISSION_COMPLETE_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_QUEST_09;

	if( true == kPacket_.m_bLevelUpEvent )
	{
		if( GetLevel() > m_ucOldYearMissionRewardedLevel &&
			GetLevel() >= 26 &&
			CXSLUnit::IsInitNormalJob( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) ) == false )
		{
			if( GetLevel() == SiKGameSysVal()->GetLimitsLevel() )
			{
				if( m_ucOldYearMissionRewardedLevel == GetLevel() - 1 )
				{
					bool bClassChange = false;
					if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) ) == true )
						bClassChange = true;

					IF_EVENT_ENABLED( CEI_ALLOW_EVE_FIRST_CLASS_OLD_YEAR_EVENT )
					{
						if( static_cast< CXSLUnit::UNIT_TYPE >( GetUnitType() ) == CXSLUnit::UT_EVE )
							bClassChange = true;
					}
					IF_EVENT_ENABLED( CEI_ALLOW_ARA_FIRST_CLASS_OLD_YEAR_EVENT )
					{
						if( static_cast< CXSLUnit::UNIT_TYPE >( GetUnitType() ) == CXSLUnit::UT_ARA )
							bClassChange = true;
					}
					IF_EVENT_ENABLED( CEI_ALLOW_ELESIS_FIRST_CLASS_OLD_AND_NEW_YEAR_EVENT )
					{
						if( static_cast< CXSLUnit::UNIT_TYPE >( GetUnitType() ) == CXSLUnit::UT_ELESIS )
							bClassChange = true;
					}

					if( bClassChange )
					{
						// 만렙이며, 서포트 보상을 받은 상태일 땐 만렙 보상을 준다.
						KDBE_2013_EVENT_MISSION_COMPLETE_REQ kPacketToDB;
						kPacketToDB.m_iUnitUID	   = GetCharUID();
						kPacketToDB.m_ucLevel	   = GetLevel();
						SendToGameDB( DBE_2013_EVENT_MISSION_COMPLETE_REQ, kPacketToDB );
						return;
					}
				}
				else
				{
					// 만렙이며, 서포트 보상을 다 못 받은 상태일 땐 서포트 보상을 한번 더 줘야 한다.
					KDBE_2013_EVENT_MISSION_COMPLETE_REQ kPacketToDB;
					kPacketToDB.m_iUnitUID	   = GetCharUID();
					kPacketToDB.m_ucLevel	   = GetLevel() - 1;
					SendToGameDB( DBE_2013_EVENT_MISSION_COMPLETE_REQ, kPacketToDB );
					return;
				}
			}
			else
			{
				KDBE_2013_EVENT_MISSION_COMPLETE_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID	   = GetCharUID();
				kPacketToDB.m_ucLevel	   = GetLevel();
				SendToGameDB( DBE_2013_EVENT_MISSION_COMPLETE_REQ, kPacketToDB );
				return;
			}
		}

		START_LOG( cwarn, L"완료 조건에 해당이 안 되는데 완료 요청? 해커!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetLevel() )
			<< BUILD_LOG( (int)m_ucOldYearMissionRewardedLevel )
			<< BUILD_LOG( (int)GetUnitClass() )
			<< BUILD_LOG( SiKGameSysVal()->GetLimitsLevel() )
			<< END_LOG;

		// 조건에 안 맞는 해킹유저 놈들
		kPacket.m_iOK = NetError::ERR_QUEST_09;
	}
	else if( m_ucOldYearMissionRewardedLevel <= SiKGameSysVal()->GetLimitsLevel() )
	{
		// 패킷 보낸다.
		KDBE_2013_EVENT_MISSION_COMPLETE_REQ kPacketToDB;
		kPacketToDB.m_iUnitUID	   = GetCharUID();
		kPacketToDB.m_ucLevel	   = 99;
		SendToGameDB( DBE_2013_EVENT_MISSION_COMPLETE_REQ, kPacketToDB );
		return;
	}

	SendPacket( EGS_2013_EVENT_MISSION_COMPLETE_ACK, kPacket );
}
IMPL_ON_FUNC( DBE_2013_EVENT_MISSION_COMPLETE_ACK )
{
	KEGS_2013_EVENT_MISSION_COMPLETE_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 보상받은 레벨 갱신(서버)
		m_ucOldYearMissionRewardedLevel = kPacket_.m_ucLevel;

		if( kPacket_.m_ucLevel > SiKGameSysVal()->GetLimitsLevel() )
		{
			std::map< int, int > mapItem;
			mapItem.insert( std::make_pair( _CONST_SERV_NEW_YEAR_EVENT_2014_::iMaxLevelSpecialMissionItemID, 1 ) );
			if( true == m_kInventory.DeleteItemOnlyInventory( mapItem, kPacket.m_vecInventorySlotInfo, KDeletedItemInfo::DR_QUEST_COMPLETE ) )
			{
				// 퀘템 삭제에 성공하였으니 보상을 주자
				KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
				kPacketToDB.m_iFromUnitUID = GetCharUID();
				kPacketToDB.m_iToUnitUID   = GetCharUID();
				kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
				kPacketToDB.m_iRewardID	   = 2004;
				SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );

				kPacket.m_iOK = NetError::NET_OK;
			}
			else
			{
				kPacket.m_iOK = NetError::ERR_QUEST_09;
			}
		}
		else
		{
			// DB에 잘 기록하였으니 보상을 주자
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			if( kPacket_.m_ucLevel >= SiKGameSysVal()->GetLimitsLevel() )
				kPacketToDB.m_iRewardID = 2003;
			else
				kPacketToDB.m_iRewardID = 2002;

			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
	}

	kPacket.m_iRewardedLevel = static_cast< int >( m_ucOldYearMissionRewardedLevel );
	SendPacket( EGS_2013_EVENT_MISSION_COMPLETE_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( EGS_2014_EVENT_MISSION_COMPLETE_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_2014_EVENT_MISSION_COMPLETE_ACK );

	IF_EVENT_ENABLED( CEI_NEW_YEAR_EVENT_2014 )
	{
	}
	ELSE
	{
		KEGS_2014_EVENT_MISSION_COMPLETE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_QUEST_09;
		SendPacket( EGS_2014_EVENT_MISSION_COMPLETE_ACK, kPacket );
		return;
	}

	bool bCompleted = false;
	switch( m_iNewYearMissionStepID )
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 8:
		bCompleted = true;
		break;
	case 1:
		{
			// 단계0 : 1차 전직 달성하면 완료
			if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) ) == true ||
				CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) ) == true )
			{
				bCompleted = true;
			}
		} break;
	case 3:
		{
			// 단계2 : 20레벨 달성하면 완료
			if( GetLevel() >= 20 )
			{
				 bCompleted = true;
			}
		} break;
	case 5:
		{
			// 단계4 : 30레벨 달성하면 완료
			if( GetLevel() >= 30 )
			{
				bCompleted = true;
			}
		} break;
	case 7:
		{
			IF_EVENT_ENABLED( CEI_ALLOW_ELESIS_FIRST_CLASS_OLD_AND_NEW_YEAR_EVENT )
			{
				if( GetLevel() >= 35 && static_cast< CXSLUnit::UNIT_TYPE >( GetUnitType() ) == CXSLUnit::UT_ELESIS )
					bCompleted = true;
			}

			// 단계6 : 2차 전직 달성하면 완료
			if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>( GetUnitClass() ) ) == true )
			{
				bCompleted = true;
			}
		} break;
	case 9:
		{
			// 단계8 : 2차 전직 달성하면 완료
			if( GetLevel() >= 45 )
			{
				bCompleted = true;
			}
		} break;
	default:
		break;
	}

	if( bCompleted == true )
	{
		KDBE_2014_EVENT_MISSION_COMPLETE_REQ kPacketToDB;
		kPacketToDB.m_iUnitUID = GetCharUID();
		kPacketToDB.m_iUserUID = GetUID();
		kPacketToDB.m_iNewYearMissionStepID = m_iNewYearMissionStepID + 1;
		SendToGameDB( DBE_2014_EVENT_MISSION_COMPLETE_REQ, kPacketToDB );
	}
	else
	{
		START_LOG( cwarn, L"완료 조건에 해당이 안 되는데 완료 요청? 해커!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( (int)GetUnitClass() )
			<< BUILD_LOG( GetLevel() )
			<< END_LOG;

		KEGS_2014_EVENT_MISSION_COMPLETE_ACK kAck;
		kAck.m_iOK = NetError::ERR_QUEST_09;
		SendPacket( EGS_2014_EVENT_MISSION_COMPLETE_ACK, kAck );
	}
}

_IMPL_ON_FUNC( DBE_2014_EVENT_MISSION_COMPLETE_ACK, KEGS_2014_EVENT_MISSION_COMPLETE_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK)
	{
		// 성공했으면 단계를 증가시키고
		++m_iNewYearMissionStepID;

		int iRewardID = 0;

		// 보상을 줍시다.
		switch( m_iNewYearMissionStepID )
		{
		case 1:
			{
				iRewardID = 2005;
			} break;
		case 2:
			{
				iRewardID = 2006;
			} break;
		case 3:
			{
				iRewardID = 2007;
			} break;
		case 4:
			{
				iRewardID = 2008;
			} break;
		case 5:
			{
				iRewardID = 2009;
			} break;
		case 6:
			{
				iRewardID = 2010;
			} break;
		case 7:
			{
				iRewardID = 2011;
			} break;
		case 8:
			{
				iRewardID = 2012;
			} break;
		case 9:
			{
				iRewardID = 2013;
			} break;
		case 10:
			{
				iRewardID = 2014;
			} break;
		default :
			break;
		}

		// 이벤트 보상을 주자!
		if( iRewardID > 0 )
		{
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = iRewardID;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
	}

	SendPacket( EGS_2014_EVENT_MISSION_COMPLETE_ACK, kPacket_ );
}


IMPL_ON_FUNC( EGS_UNLIMITED_SECOND_CHANGE_JOB_NOT )
{
	KDBE_BUY_UNIT_CLASS_CHANGE_REQ kPacketToDB;

	if( GetUnlimitedClassChangeInfo( kPacket_.m_iUnlimitedSecondChangeJob, kPacketToDB ) == true )
	{
		SendToGameDB( DBE_BUY_UNIT_CLASS_CHANGE_REQ, kPacketToDB );
	}
	else
	{
		START_LOG( cerr, L"2차 무제한 전직이 실패 했다." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iUnlimitedSecondChangeJob )
			<< END_LOG;
	}
}


IMPL_ON_FUNC( EGS_KOM_FILE_CHECK_LOG_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_KOM_FILE_CHECK_LOG_REQ, EGS_KOM_FILE_CHECK_LOG_ACK );

	KELOG_KOM_FILE_CHECK_LOG_NOT kPacket;
	kPacket.m_iUserUID = GetUID();
	kPacket.m_wstrInvaildKomName = kPacket_.m_wstrInvalidKomName;
	SendToLogDB( ELOG_KOM_FILE_CHECK_LOG_NOT, kPacket );

	KEGS_KOM_FILE_CHECK_LOG_ACK kPacket2;
	kPacket2.m_wstrInvalidKomName = kPacket_.m_wstrInvalidKomName;
	kPacket2.m_iOK = NetError::NET_OK;
	SendPacket( EGS_KOM_FILE_CHECK_LOG_ACK, kPacket2 );
}


IMPL_ON_FUNC_NOPARAM( EGS_GET_NEXT_SKILL_PAGE_ED_REQ )
{
	// 상태 체크
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_GET_NEXT_BANK_ED_ACK );
	KEGS_GET_NEXT_BANK_ED_ACK kPacket;

	const UCHAR ucTheNumberOfSkillPagesAvailable
		= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

	int iNextED = 0;

	switch ( ucTheNumberOfSkillPagesAvailable )
	{
	case 1:	/// 현재 사용 가능한 페이지 수가 한개라 2개로 확장 하려는 경우
	case 2:	/// 현재 사용 가능한 페이지 수가 두개라 3개로 확장 하려는 경우
		iNextED = KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE;
		break;

	default:
		{
			SET_ERROR( ERR_SKILL_PAGE_02 );
			goto end_proc;
		} break;
	}

	kPacket.m_iED = iNextED;
	SET_ERROR( NET_OK );

end_proc:

	kPacket.m_iOK = NetError::GetLastError();
	SendPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( EGS_EXPAND_SKILL_PAGE_REQ )
{
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_FIELD_MAP ), EGS_EXPAND_INVENTORY_ED_ACK );

	KDBE_EXPAND_SKILL_PAGE_REQ kPacket;

	// 스킬 페이지를 확장할 페이지 여유가 있는 확인
	if ( !m_kSkillTree.CanExpandSkillPage() )
	{
		SET_ERROR( ERR_SKILL_PAGE_02 );
		goto end_proc;
	}

	// 확장할 수 있는 ED를 보유하고 있는지 체크
	if ( m_iED < KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE || m_iED < 0 )
	{
		// 일단 인벤토리 것을 사용 하자
		SET_ERROR( ERR_EXPAND_INVENTORY_ED_01 );
		goto end_proc;
	}

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

		SET_ERROR( ERR_SKILL_12 );
		goto end_proc;
	}

	kPacket.m_usEventID = EGS_EXPAND_SKILL_PAGE_REQ;
	kPacket.m_iUnitUID	= GetCharUID();
	/// 추가되어야 할 페이지 넘버
	kPacket.m_vecSkillPageNumberToBeAdded.push_back( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() + 1 );
	kPacket.m_cUnitClass = GetUnitClass();

	// 스킬 포인트 얻어오기
	SiCXSLSkillTree()->GetCalcInitSkillPoint( GetLevel(), kPacket.m_iSPoint );
	// 캐시 스킬 포인트 얻어오기
	kPacket.m_iCSPoint	=	m_kSkillTree.GetMaxCSPoint();

	// ED 차감
	m_iED -= KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE;

	// DB에 스킬페이지 확장 요청
	SendToGameDB( DBE_EXPAND_SKILL_PAGE_REQ, kPacket );

end_proc:
	if ( NetError::GetLastError() != NetError::NET_OK )
	{
		KEGS_EXPAND_SKILL_PAGE_ACK kAck;
		kAck.m_iOK = NetError::GetLastError();
		SendPacket( EGS_EXPAND_SKILL_PAGE_ACK, kAck );

		START_LOG( cerr, L"스킬 페이지 확장 실패 , User ID :" << GetUserID() )
			// 현재 이용 가능한 스킬 페이지 수
			// 현재 보유 ED 양
			<< BUILD_LOG( NetError::GetLastErrMsg() )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_EXPAND_SKILL_PAGE_ACK )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	int iHowManyTimesToBeExpanded{};
	 VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP ), EGS_EXPAND_SKILL_PAGE_ACK );

	 KEGS_EXPAND_SKILL_PAGE_ACK kPacket;
	 KELOG_EXPAND_SKILL_PAGE_NOT kLog;

	 if ( kPacket_.m_iOK != NetError::NET_OK )
	 {
		 START_LOG( cerr, L"스킬 페이지 확장 실패(DBE_EXPAND_SKILL_PAGE_ACK) , User ID :" << GetUserID() )
			 << BUILD_LOG( GetUID() )
			 << BUILD_LOG( GetCharName() )
			 << BUILD_LOG( GetCharUID() )
			 << BUILD_LOG( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
			 << BUILD_LOG( kPacket_.m_iSkillPageNumberToBeAdded )
			 << END_LOG;
		 goto end_proc;
	 }

	 if ( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() >= kPacket_.m_iSkillPageNumberToBeAdded )
	 {
		 /// kimhc // 김현철 // 이미 DB 처리등은 되어 있을 것이므로 로그만 출력하자.
		START_LOG( cerr, L"이미 확장이 되어 있다는데?(DBE_EXPAND_SKILL_PAGE_ACK) , User ID :" << GetUserID() )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( "m_kSkillTree.GetTheNumberOfSkillPagesAvailable() >= kPacket_.m_iSkillPageNumberToBeAdded" )
			<< BUILD_LOG( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
			<< BUILD_LOG( kPacket_.m_iSkillPageNumberToBeAdded )
			<< END_LOG;
	 }

	 iHowManyTimesToBeExpanded
		 = kPacket_.m_iSkillPageNumberToBeAdded - m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

	 for ( int i = 0; i < iHowManyTimesToBeExpanded; i++ )
		 m_kSkillTree.ExpandSkillPage( kPacket_.m_iDefaultSkill, kPacket_.m_iSPoint, kPacket_.m_iCSPoint );

	 kPacket.m_iCSPointAvailable				= kPacket_.m_iCSPoint;
	 kPacket.m_iSPointAvailable					= kPacket_.m_iSPoint;
	 kPacket.m_iTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

	 m_kSkillTree.GetUnSealedSkillList( kPacket.m_vecUnsealedSkillID );

	 for ( int i = 0; i < THE_NUMBER_OF_DEFAULT_SKILLS; i++ )
		 kPacket.m_vecSkillListLearned.push_back( kPacket_.m_iDefaultSkill[i] );

	 kLog.m_iUnitUID				= GetCharUID();
	 kLog.m_wstrSenderNickName		= GetCharName();
	 kLog.m_ucLevel					= GetLevel();
	 kLog.m_ucUnitClass				= GetUnitClass();
	 kLog.m_ucBeforeTheNumberOfSkillPagesAvailable	= m_kSkillTree.GetTheNumberOfSkillPagesAvailable() - iHowManyTimesToBeExpanded;

	 if ( kPacket_.m_usEventID == EGS_EXPAND_SKILL_PAGE_REQ )
	 {
		 kLog.m_ucPaymentType = 0;	// ED 구매
		 // DB통계 ED
		 KStatisticsKey kKey;
		 kKey.m_vecIntKey.push_back( 0 );
		 KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_ExpandSkillPage, ( KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE ) );
	 }
	 else
	 {
		 kPacket.m_bPayWithED = false;
		 kLog.m_ucPaymentType = 1;	// 캐시 구매
	 }

	 SendToLogDB( ELOG_EXPAND_SKILL_PAGE_NOT, kLog );

end_proc:
	 // 실패 한 경우
	 if ( kPacket_.m_iOK != NetError::NET_OK )
	 {
		 // ED로 구매했을 때만 ED 원래 대로
		 if ( kPacket_.m_usEventID == EGS_EXPAND_SKILL_PAGE_REQ )
			 m_iED += KUserSkillTree::ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE;
	 }

	 kPacket.m_iOK = kPacket_.m_iOK;
	 kPacket.m_iED = m_iED;

	 SendPacket( EGS_EXPAND_SKILL_PAGE_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ )
{
	KDBE_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ kReq(GetCharUID(), kPacket_.m_iSkillPagesNumberDecidedToUse);
	// 마을, 쉼터에서만 가능
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_FIELD_MAP ), EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );


	if ( kPacket_.m_iSkillPagesNumberDecidedToUse < 1 ||
		kPacket_.m_iSkillPagesNumberDecidedToUse > m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
	{
		SET_ERROR( ERR_SKILL_PAGE_03 );
		goto end_proc;
	}

	SendToGameDB( DBE_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ, kReq );
	return;

end_proc:
	if ( NetError::GetLastError() != NetError::NET_OK )
	{
		KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK kAck;
		kAck.m_iOK = NetError::GetLastError();
		SendPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );

		START_LOG( cerr, L"EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ) , User ID :" << GetUserID() )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
			<< BUILD_LOG( kPacket_.m_iSkillPagesNumberDecidedToUse )
			<< BUILD_LOG( NetError::GetLastErrMsg() )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK )
{
	// 마을, 쉼터에서만 가능
	VERIFY_STATE_ACK( ( 1, KGSFSM::S_FIELD_MAP ), EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );

	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK kAck;

	if ( kPacket_.m_iSkillPagesNumberToBeActive < 1 ||
		kPacket_.m_iSkillPagesNumberToBeActive > m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
	{
		SET_ERROR( ERR_SKILL_PAGE_03 );
		goto end_proc;
	}

	m_kSkillTree.SetActiveSkillPageNumber( static_cast<u_char>(kPacket_.m_iSkillPagesNumberToBeActive) );
	kAck.m_iOK								= NetError::NET_OK;
	kAck.m_iCSPointAvailable				= m_kSkillTree.GetCSPoint();
	kAck.m_iSPointAvailable					= m_kSkillTree.GetSPoint();
	kAck.m_iSkillPagesNumberDecidedToUse	= kPacket_.m_iSkillPagesNumberToBeActive;

	m_kSkillTree.GetKUserSkillPageData( kAck.m_kUserSkillPageData, kPacket_.m_iSkillPagesNumberToBeActive );
	SendPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );
	return;

end_proc:
	if ( NetError::GetLastError() != NetError::NET_OK )
	{
		kAck.m_iOK = NetError::GetLastError();
		SendPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, kAck );

		START_LOG( cerr, L"EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ) , User ID :" << GetUserID() )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( m_kSkillTree.GetTheNumberOfSkillPagesAvailable() )
			<< BUILD_LOG( kPacket_.m_iSkillPagesNumberToBeActive )
			<< BUILD_LOG( NetError::GetLastErrMsg() )
			<< END_LOG;
	}
}


IMPL_ON_FUNC_NOPARAM( EGS_CHARACTER_LIST_REQ )
{
    VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_LOGINED, KGSFSM::S_SERVER_SELECT ), EGS_CHARACTER_LIST_REQ, EGS_CHARACTER_LIST_ACK );
    SendToGameDB( DBE_CHARACTER_LIST_REQ, GetName() );

    KDBE_GET_SECOND_SECURITY_INFO_REQ kReq;
    kReq.m_iUserUID = GetUID();
    SiKGameSysVal()->GetComeBackRewardCondition( kReq.m_mapComeBackRewardCondition );

    //{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
    //}}

    SendToAccountDB( DBE_GET_SECOND_SECURITY_INFO_REQ, kReq );	// AccountDB에 최근 접속 종료 정보 얻기


}

IMPL_ON_FUNC( EGS_CHARACTER_LIST_1ST_ACK )
{
    switch( GetKGSSimLayer()->GetServerGroupNum() )
    {
    case KGSSimLayer::SINGLE_SERVER_GROUP:
        // EGS_CHARACTER_LIST_ACK 처리와 동일하게 하고..
        ON_EGS_CHARACTER_LIST_ACK( anTrace_, kPacket_ );
        break;
    case KGSSimLayer::DOUBLE_SERVER_GROUP: // 한국, 중국
        SendToGameDB2nd( DBE_CHARACTER_LIST_2ND_REQ, kPacket_ );
        break;
    default:
        break;
    }
}

IMPL_ON_FUNC( EGS_CHARACTER_LIST_ACK )
{
    VERIFY_STATE( ( 2, KGSFSM::S_SERVER_SELECT, KGSFSM::S_CHECK_SECOND_PW ) );

    if( kPacket_.m_iOK == NetError::NET_OK )
    {
        m_nUnitSlot = kPacket_.m_mapServerGroupUnitSlot[KBaseServer::GetKObj()->GetServerGroupID()];

        std::map< int, std::vector<KUnitInfo> >::iterator mit;
        for ( mit = kPacket_.m_mapServerGroupUnitInfo.begin(); mit != kPacket_.m_mapServerGroupUnitInfo.end() ; ++mit )
        {
            std::vector<KUnitInfo>::iterator vit;
            for( vit = mit->second.begin(); vit != mit->second.end(); ++vit )
            {
                vit->m_iOwnerUserUID		= GetUID();
                vit->m_nStraightVictories	= 0;
                vit->m_wstrIP				= GetP2PIP();
                vit->m_usPort				= static_cast<USHORT>(GetP2PPort());

                const CXSLUnit::UnitTemplet* pUnitTemplet = SiCXSLUnitManager()->GetUnitTemplet( static_cast<CXSLUnit::UNIT_CLASS>(vit->m_cUnitClass) );
                if( pUnitTemplet == NULL )
                {
                    START_LOG_WITH_NAME( cerr )
                        << BUILD_LOGc( vit->m_cUnitClass )
                        << END_LOG;

#if X2_DEV_TOOLS
                    continue;
#endif

                    kPacket_.m_iOK = NetError::ERR_CREATE_UNIT_03;
                    goto end_proc;

                }

                vit->m_ucLevel				= SiKExpTable()->CheckLevelUp( 1, vit->m_iEXP );
                vit->m_nNowBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)(vit->m_ucLevel) );
                vit->m_nNextBaseLevelEXP	= SiKExpTable()->GetRequireTotalExpbyLevel( (int)(vit->m_ucLevel + 1) );

                KStatTable::KUnitStatInfo sUnitStat;
                SiKStatTable()->GetUnitStat( vit->m_cUnitClass, vit->m_ucLevel, sUnitStat );
                vit->m_kStat.m_iBaseHP		+= (int)sUnitStat.m_uiHP;
                vit->m_kStat.m_iAtkPhysic	+= (int)sUnitStat.m_usAtkPhysic;
                vit->m_kStat.m_iAtkMagic	+= (int)sUnitStat.m_usAtkMagic;
                vit->m_kStat.m_iDefPhysic	+= (int)sUnitStat.m_usDefPhysic;
                vit->m_kStat.m_iDefMagic	+= (int)sUnitStat.m_usDefMagic;

                // 보스 필드라면 마을로 이동 시키자
                if( SiCXSLBattleFieldManager()->IsBossFieldID( vit->m_kLastPos.m_iMapID ) == true )
                {
                    vit->m_kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( vit->m_ucLevel, vit->m_mapDungeonClear );
                    vit->m_kLastPos.m_bIgnoreLastTouch = true;
                }

                // 보유하고 있는 캐릭터 리스트 세팅
                m_setMyUnitUIDList.insert( vit->m_nUnitUID );

                GetPvpRankForClient( vit->m_iOfficialMatchCnt, vit->m_cRank );
            }

            START_LOG( clog, L"EGS_CHARACTER_LIST_ACK 에 담겨지는 p2p data" )
                << BUILD_LOG( GetP2PIP() )
                << BUILD_LOG( GetP2PPort() );

            if( ( int )mit->second.size() > kPacket_.m_mapServerGroupUnitSlot[mit->first] )
            {
                START_LOG( cerr, L"캐릭터 슬롯 개수를 초과하는 수의 캐릭터를 보유하고 있음." )
                    << BUILD_LOG( ( int )mit->second.size() )
                    << BUILD_LOG( kPacket_.m_mapServerGroupUnitSlot[mit->first] )
                    << BUILD_LOG( GetUID() )
                    << BUILD_LOG( GetName() );
            }

        }
        // udp 에서 받은 p2p ip/port를 채워보내준다.(임시가 될수있음 차후에 옴겨야 할수도있다.!머리아프네 )
    }
end_proc:

    START_LOG( clog, L"캐릭터 리스트 결과(슬롯) : " )
        << BUILD_LOG( kPacket_.m_mapServerGroupUnitSlot[0] )
        << BUILD_LOG( kPacket_.m_mapServerGroupUnitSlot[1] )
        << END_LOG;

    SendPacket( EGS_CHARACTER_LIST_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_CREATE_NEW_UNIT_REQ )
{
    VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_CREATE_NEW_UNIT_REQ, EGS_CREATE_UNIT_ACK );

    const CXSLUnit::UnitTemplet* pUnitTemplet = SiCXSLUnitManager()->GetUnitTemplet( (CXSLUnit::UNIT_CLASS)kPacket_.m_iClass );
    KEGS_CREATE_UNIT_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_UNKNOWN;
    char charBuf[255] = {0};
    std::string strNickName;

    KDBE_GAME_CREATE_UNIT_REQ kDBPacket;
    KAccountInfo AccountTemp;

    if( !pUnitTemplet ) // 070604. florist. templet을 얻어올 필요는 없다. iClass의 유효성 검증 정도의 의미만 가진다.
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_03;
        goto end_proc;
    }

    if( CXSLUnit::IsInitNormalJob( (CXSLUnit::UNIT_CLASS)kPacket_.m_iClass ) == false )
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_11;
        goto end_proc;
    }

    WideCharToMultiByte( CP_ACP, 0, kPacket_.m_wstrNickName.c_str(), -1, charBuf, 255, NULL, NULL );
    strNickName = charBuf;

    if( strNickName.empty() || strNickName.size() > 12 )   //캐릭터 닉네임 길이 검사.
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_04;
        goto end_proc;
    }

    if ( GetAuthLevel() < SEnum::UAL_GM && SiCXSLStringFilter()->CheckIsValidString( CXSLStringFilter::FT_NICKNAME, kPacket_.m_wstrNickName ) == false )
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_08;
        goto end_proc;
    }

    if( SiKPvpMatchManager()->IsExistPvpNpcName( kPacket_.m_wstrNickName ) == true )
    {
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_01;
        goto end_proc;
    }

    // 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
    if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
    {
        kPacket.m_iOK = NetError::ERR_ODBC_00;
        goto end_proc;
    }

    START_LOG_WITH_NAME( clog )
        << BUILD_LOG( kPacket_.m_wstrNickName )
        << BUILD_LOG( kPacket_.m_iClass );

    GetAccountInfo( AccountTemp );
    kDBPacket.m_wstrRegDate = AccountTemp.m_wstrRegDate;
    kDBPacket.m_wstrNickName = kPacket_.m_wstrNickName;
    kDBPacket.m_iClass = kPacket_.m_iClass;

    // 1st 솔레스, 2nd 가이아가 아니라 1st 는 내가 속한 디비군, 2nd 는 다른 디비군인 점에 유의
    switch ( KBaseServer::GetKObj()->GetServerGroupID() )
    { // 해외의 서버군1개 국가에서는 서버=솔레스, kPacket_.m_iServerGroup=솔레스 로 보내면 됨
        case SEnum::SGI_SOLES:
            if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
            {   // 기존 디비 함수로 큐잉한다는 것에 주의.
                SendToGameDB( DBE_GAME_CREATE_UNIT_REQ, kDBPacket );
            }
            else if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA
                && GetKGSSimLayer()->GetServerGroupNum() == KGSSimLayer::DOUBLE_SERVER_GROUP )
            {
                SendToGameDB2nd( DBE_GAME_CREATE_UNIT_REQ, kDBPacket );
            }
            break;
        case SEnum::SGI_GAIA:
            if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
            {
                SendToGameDB( DBE_GAME_CREATE_UNIT_REQ, kDBPacket );
            }
            else if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
            {
                SendToGameDB2nd( DBE_GAME_CREATE_UNIT_REQ, kDBPacket );
            }

            break;
        default:
            if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
            {
                SendToGameDB( DBE_GAME_CREATE_UNIT_REQ, kDBPacket );
            }
    }
    kPacket.m_iOK = NetError::NET_OK;

end_proc:
    LOG_CONDITION( kPacket.m_iOK == NetError::ERR_CREATE_UNIT_03, cerr, cwarn )
        << BUILD_LOG( GetName() )
        << BUILD_LOG( kPacket.m_iOK )
        << BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
        << BUILD_LOG( kPacket_.m_iClass)
        << BUILD_LOG( kPacket_.m_wstrNickName )
        << END_LOG;

    if ( kPacket.m_iOK != NetError::NET_OK )
    {
        SendPacket( EGS_CREATE_UNIT_ACK, kPacket );
    }
}

IMPL_ON_FUNC( EGS_ENTRY_POINT_DELETE_UNIT_REQ )
{
    VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_LOGINED, KGSFSM::S_SERVER_SELECT ), EGS_DELETE_UNIT_REQ, EGS_DELETE_UNIT_ACK );

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
    if( IsUseSecurity() == true )
    {
        if( GetSuccessSecondSecurity() == false )	// 2차 보안을 사용 중인데 인증이 되지 않았다면 실패
        {
            START_LOG( cerr, L"2차 보안을 사용 중인데 인증도 되기 전에 유닉 삭제를 하려 한다." )
                << BUILD_LOG( GetUID() )
                << END_LOG;

            KEGS_DELETE_UNIT_ACK kPacket;
            kPacket.m_iOK = NetError::ERR_VERIFY_21;
            kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
            kPacket.m_iGuildUID = 0;
            SendPacket( EGS_DELETE_UNIT_ACK, kPacket );
            return;
        }
    }
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
    if( m_kUserRelationshipManager.GetRelationshipType() != SEnum::RT_SOLO )
    {
        KEGS_DELETE_UNIT_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_DELETE_UNIT_06;
        SendPacket( EGS_DELETE_UNIT_ACK, kPacket );

        START_LOG( cerr, L"진입구조개편 캐릭터 삭제 실패, UID : " << GetUID() )
            << BUILD_LOG( kPacket.m_iOK )
            //<< BUILD_LOG( NetError::GetLastErrMsg() )
            << END_LOG;

        return;
    }
	//}

    KEGS_DELETE_UNIT_REQ kDBPacket;
    kDBPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	//{{ 2012. 02. 21	김민성	캐릭터 삭제 및 길드 탈퇴 예외처리 수정
    kDBPacket.m_iUserUID = GetUID();
	//}}

    switch ( KBaseServer::GetKObj()->GetServerGroupID() )
    {
    case SEnum::SGI_SOLES:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB( DBE_GAME_DELETE_UNIT_REQ, kDBPacket );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA && GetKGSSimLayer()->GetServerGroupNum() == KGSSimLayer::DOUBLE_SERVER_GROUP )
        {
            SendToGameDB2nd( DBE_GAME_DELETE_UNIT_REQ, kDBPacket );
        }
        break;
    case SEnum::SGI_GAIA:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
        {
            SendToGameDB( DBE_GAME_DELETE_UNIT_REQ, kDBPacket );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB2nd( DBE_GAME_DELETE_UNIT_REQ, kDBPacket );
        }

        break;
    default:
        if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
        {
            SendToGameDB( DBE_GAME_DELETE_UNIT_REQ, kDBPacket );
        }
    }

    m_kUserTutorialInfo.OnDeleteUnitReq( kPacket_.m_iUnitUID );

}

IMPL_ON_FUNC( EGS_ENTRY_POINT_CHECK_NICK_NAME_REQ )
{
    // 닉네임 길이 체크
    char charBuf[255] = {0};
    std::string strNickName;

    WideCharToMultiByte( CP_ACP, 0, kPacket_.m_wstrNickName.c_str(), -1, charBuf, 255, NULL, NULL );
    strNickName = charBuf;

    KEGS_CHECK_NICK_NAME_ACK kPacket;

    if( strNickName.empty() || strNickName.size() > 12 )   //캐릭터 닉네임 길이 검사.
    { // TODO : 해외팀 코드에는 닉네임 최소 기준도 있는데, 이런 부분은 디파인보다는 gssimlayer에 루아로 제어하면 좋겠다.

        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_06;
        goto end_proc;
    }

    if( GetAuthLevel() < SEnum::UAL_GM && SiCXSLStringFilter()->CheckIsValidString( CXSLStringFilter::FT_NICKNAME, kPacket_.m_wstrNickName ) == false )
    {
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        goto end_proc;
    }

    if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
    {
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        goto end_proc;
    }
    // 디비에 있는 닉네임인가 , 디비로 큐잉해야 알 수 있다.
    // 닉네임 변경 쿨타임인지 체크.
    kPacket.m_iOK = NetError::NET_OK;

    switch ( KBaseServer::GetKObj()->GetServerGroupID() )
    {
    case SEnum::SGI_SOLES:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB( DBE_ENTRY_POINT_CHECK_NICK_NAME_REQ, kPacket_ );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
        {
            SendToGameDB2nd( DBE_ENTRY_POINT_CHECK_NICK_NAME_REQ, kPacket_ );
        }
        break;
    case SEnum::SGI_GAIA:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
        {
            SendToGameDB( DBE_ENTRY_POINT_CHECK_NICK_NAME_REQ, kPacket_ );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB2nd( DBE_ENTRY_POINT_CHECK_NICK_NAME_REQ, kPacket_ );
        }

        break;
    default:
        if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
		{
			START_LOG( cerr, L"사내서버 서버군 이상" )
				<< BUILD_LOG( KBaseServer::GetKObj()->GetServerGroupID() )
				<< BUILD_LOG( kPacket_.m_iServerGroup)
				<< END_LOG;
        }
		else
		{
			kPacket.m_iOK = NetError::ERR_HERO_PVP_USER_LIST_01;
			goto end_proc;
		}
    }

end_proc:
    LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
        << BUILD_LOG( GetUID() )
        << BUILD_LOG( kPacket_.m_iServerGroup )
        << END_LOG;

    if ( kPacket.m_iOK != NetError::NET_OK )
    {
        SendPacket( EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK, kPacket );
    }
}

IMPL_ON_FUNC( EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK )
{
    SendPacket( EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK, kPacket_ );

    LOG_SUCCESS( kPacket_.m_iOK == NetError::NET_OK )
        << BUILD_LOG( GetUID() )
        << BUILD_LOG( kPacket_.m_wstrNickName )
        << END_LOG;
}

IMPL_ON_FUNC_NOPARAM( EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ )
{
    KDBE_GET_CREATE_UNIT_TODAY_COUNT_REQ kDBPacket;
    kDBPacket.m_iUserUID = GetUID();
    // 두 서버군의 유닛 생성 수를 불러오려면 1st, 2nd 큐잉 사용. 결과는 합쳐서 클라이언트로 보내주기
    SendToGameDB( DBE_GET_CREATE_UNIT_TODAY_COUNT_REQ, kDBPacket );
}

_IMPL_ON_FUNC( EGS_GET_CREATE_UNIT_TODAY_COUNT_1ST_ACK, KEGS_GET_CREATE_UNIT_TODAY_COUNT_ACK )
{
    switch( GetKGSSimLayer()->GetServerGroupNum() )
    {
    case KGSSimLayer::SINGLE_SERVER_GROUP:
        SendPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK, kPacket_ );
        break;
    case KGSSimLayer::DOUBLE_SERVER_GROUP: // 서버군이 2개인 국가
        SendToGameDB2nd( DBE_GET_CREATE_UNIT_TODAY_COUNT_REQ, kPacket_ );
        break;
    default:
        break;
    }
}

IMPL_ON_FUNC( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK )
{
    SendPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK, kPacket_ );
}

IMPL_ON_FUNC( EGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ )
{
    //{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
    VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_SERVER_SELECT ), EGS_CHANGE_NICK_NAME_REQ, EGS_CHANGE_NICK_NAME_ACK );
    //}}

    char charBuf[255] = {0};
    std::string strNickName;
    KEGS_CHANGE_NICK_NAME_REQ kDBPacket;

    WideCharToMultiByte( CP_ACP, 0, kPacket_.m_wstrNickName.c_str(), -1, charBuf, 255, NULL, NULL );
    strNickName = charBuf;

    if( strNickName.empty() || strNickName.size() > 12 )   //캐릭터 닉네임 길이 검사.
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_06;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

    if( GetAuthLevel() < SEnum::UAL_GM && SiCXSLStringFilter()->CheckIsValidString( CXSLStringFilter::FT_NICKNAME, kPacket_.m_wstrNickName ) == false )
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

    // 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
    if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_RESTORE_NICK_NAME_05;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }

    //{{ 2011. 07. 27	최육사	대전 개편
    if( SiKPvpMatchManager()->IsExistPvpNpcName( kPacket_.m_wstrNickName ) == true )
    {
        KEGS_CHANGE_NICK_NAME_ACK kPacket;
        kPacket.m_iOK = NetError::ERR_CREATE_UNIT_01;
        SendPacket( EGS_CHANGE_NICK_NAME_ACK, kPacket );
        return;
    }
    //}}

    kDBPacket.m_wstrNickName = kPacket_.m_wstrNickName;
    kDBPacket.m_iUnitUID = kPacket_.m_iUnitUID;
    kDBPacket.m_bCheckOnly = kPacket_.m_bCheckOnly;

    switch ( KBaseServer::GetKObj()->GetServerGroupID() )
    {
    case SEnum::SGI_SOLES:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB( DBE_CHANGE_NICK_NAME_REQ, kDBPacket );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
        {
            SendToGameDB2nd( DBE_CHANGE_NICK_NAME_REQ, kDBPacket );
        }
        break;
    case SEnum::SGI_GAIA:
        if ( kPacket_.m_iServerGroup == SEnum::SGI_GAIA )
        {
            SendToGameDB( DBE_CHANGE_NICK_NAME_REQ, kDBPacket );
        }
        else if ( kPacket_.m_iServerGroup == SEnum::SGI_SOLES )
        {
            SendToGameDB2nd( DBE_CHANGE_NICK_NAME_REQ, kDBPacket );
        }

        break;
    default:
        if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
        {

        }
    }
}
