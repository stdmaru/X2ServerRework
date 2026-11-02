#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"
//{{ 2009. 4. 22  최육사	대전유저리스트
#include "PVPLobbyManager.h"
//}}

//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
//}}

//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
#include "UserManager.h"
//}}
//{{ 2012. 03. 05	최육사	배틀필드 시스템
	#include "X2Data/XSLBuffManager.h"
//}}

#include "GameEvent/GameEventManager.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

//{{ 2008. 11. 4  최육사	광장 개편
IMPL_ON_FUNC( EGS_STATE_CHANGE_FIELD_REQ )
{

	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
//#ifdef SERV_REPEAT_FILTER_REFAC
	VERIFY_STATE_REPEAT_FILTER( ( 3, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_STATE_CHANGE_FIELD_REQ, EGS_STATE_CHANGE_FIELD_ACK );
//#else
//	VERIFY_STATE( ( 3, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
//#endif SERV_REPEAT_FILTER_REFAC
	//}}

	KEGS_STATE_CHANGE_FIELD_ACK kPacketAck;







	//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
	if( CheckEnterTheVillage( kPacket_.m_iMapID, kPacketAck.m_iLimitLevel, kPacketAck.m_iRequireDungeonID ) == false )
	{
		kPacketAck.m_iOK = NetError::GetLastError();

		START_LOG( cwarn, L"The requirements for entering town are not met." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iMapID )
			<< BUILD_LOG( kPacketAck.m_iOK )
			<< BUILD_LOG( kPacketAck.m_iLimitLevel )
			<< BUILD_LOG( kPacketAck.m_iRequireDungeonID )
			<< END_LOG;
		SendPacket( EGS_STATE_CHANGE_FIELD_ACK, kPacketAck );
		return;
	}
	//}}

	if( GetFieldUID() > 0 )
	{
		//  해당 필드에서 나오자~!
		KERM_LEAVE_FIELD_REQ kPacket;
		kPacket.m_iReason = NetError::NOT_LEAVE_FIELD_REASON_00;
		kPacket.m_iMapID  = kPacket_.m_iMapID;

		SendToGSField( ERM_LEAVE_FIELD_REQ, kPacket );
	}
	else
	{
		if( !SiCXSLMapData()->VerifyMapID( kPacket_.m_iMapID ) )
		{
			START_LOG( cerr, L"Invalid MapID when attempting to enter the field." )
				<< BUILD_LOG( kPacket_.m_iMapID )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
			SendPacket( EGS_STATE_CHANGE_FIELD_ACK, kPacketAck );
			return;
		}

		// MapID 저장
		//{{ 2012. 02. 02	최육사	배틀필드 시스템
		SetMapID( kPacket_.m_iMapID );
		//}}
		kPacketAck.m_iOK = NetError::NET_OK;
		SendPacket( EGS_STATE_CHANGE_FIELD_ACK, kPacketAck );

		//{{ 버프 관리 시스템 - 김민성
		KEGS_UPDATE_BUFF_INFO_NOT kPacket;
		m_kUserBuffManager.GetBuffList( kPacket.m_vecActivateBuffList );

		if( ( kPacket.m_vecActivateBuffList.empty() == false ) )
		{
			SendPacket( EGS_UPDATE_BUFF_INFO_NOT, kPacket );
		}
		//}
	}

	//////////////////////////////////////////////////////////////////////////
	// 필드로 이동할때 처리해야하는것..

	// PVP로비 이탈 처리
	SendLeavePVPLobby();

	//{{ 2011. 07. 11	최육사	대전 개편
//#ifdef SERV_PVP_NEW_SYSTEM
//	SendLeavePVPMatch(); 여기에서는 굳이 호출할 필요는 없을듯..?
//#endif SERV_PVP_NEW_SYSTEM
	//}}

	// 개인거래 이탈 처리
	if( GetTradeUID() != 0 )
	{
		KEGS_BREAK_TRADE_REQ kPacket;
		kPacket.m_iReason	= NetError::NOT_LEAVE_ROOM_REASON_01;
		_SendToCnRoom( GetTradeUID(), ERM_BREAK_TRADE_REQ, kPacket );

		m_iTradeUID = 0;
	}

	// 개인상점 이탈 처리
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

	// 대전, 던전, 훈련소, 거래광장 이탈 처리
	if( GetStateID() == KGSFSM::S_ROOM )
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
				kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01;
				SendToCnRoom( ERM_LEAVE_TC_ROOM_REQ, kPacket );
			}
			break;

			//{{ 2012. 10. 18	최육사	배틀필드 시스템
			//{{ 2013. 04. 01	 인연 시스템 - 김민성
		case CXSLRoom::RT_WEDDING_HALL:
			//}
		case CXSLRoom::RT_BATTLE_FIELD:
			{
				KERM_LEAVE_ROOM_REQ kPacket;
				if( kPacket_.m_bUseWarpItem )
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_30;
				}
				else
				{
					kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_27;
				}
				SendToCnRoom( ERM_LEAVE_ROOM_REQ, kPacket );
			}
			break;
			//}}

		default:
			{
				KERM_LEAVE_ROOM_REQ kPacket;
				kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_01;
				//{{ 2012. 11. 7	최육사	배틀필드 시스템
				SendToCnRoom( ERM_LEAVE_ROOM_FOR_ENTER_THE_VILLAGE_REQ, kPacket );
				//}}
			}
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	m_kUserQuestManager.CheckEpicQuest_SuccessStateChange( kPacket_.m_iMapID, GetThisPtr<KGSUser>() );
}

IMPL_ON_FUNC( ERM_LEAVE_FIELD_ACK )
{
	// 상태는 어떻게 되나..?
	VERIFY_STATE( ( 3, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iReason == NetError::NOT_LEAVE_FIELD_REASON_00 )
	{
		KEGS_STATE_CHANGE_FIELD_ACK kPacketAck;

		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			//{{ 2012. 02. 02	최육사	배틀필드 시스템
			SetMapID( kPacket_.m_iMapID );
			//}}

			SetFieldUID( 0 ); // 필드에서 나오기 성공했으므로 0으로..

			// 이동할 필드의 FieldUId를 얻는다.
			if( !SiCXSLMapData()->VerifyMapID( kPacket_.m_iMapID ) )
			{
				START_LOG( cerr, L"Invalid MapID when attempting to enter the field." )
					<< BUILD_LOG( kPacket_.m_iMapID )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
				SendPacket( EGS_STATE_CHANGE_FIELD_ACK, kPacketAck );
				return;
			}
		}

		// 클라이언트로..
		kPacketAck.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_STATE_CHANGE_FIELD_ACK, kPacketAck );
	}
	else
	{
		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			SetFieldUID( 0 ); // 필드에서 나오기 성공했으므로 0으로..
		}
		else
		{
			START_LOG( cerr, L"Failed to leave the field!" )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
		}
	}
}

IMPL_ON_FUNC( EGS_FIELD_LOADING_COMPLETE_REQ )
{
	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP ), EGS_FIELD_LOADING_COMPLETE_REQ, EGS_FIELD_LOADING_COMPLETE_ACK );
	//}}

	// 해당 필드의 입장 가능 여부는 EGS_STATE_CHANGE_FIELD_REQ에서 체크했음!

	KEGS_FIELD_LOADING_COMPLETE_ACK kPacketAck;

	if( GetCharUID() == 0 )
	{
		START_LOG( cerr, L"Attempted to enter town with invalid character information!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_ucLastTouchLineIndex )
			<< BUILD_LOG( kPacket_.m_usLastPosValue )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00;
		SendPacket( EGS_FIELD_LOADING_COMPLETE_ACK, kPacketAck );
		return;
	}

	if( GetFieldUID() > 0 )
	{
		START_LOG( cerr, L"Received this packet without checking field entry?" )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00;
		SendPacket( EGS_FIELD_LOADING_COMPLETE_ACK, kPacketAck );
		return;
	}
	else
	{
		KERM_JOIN_FIELD_REQ kPacket;

		// FieldUId를 얻는다.
		if( !SiCXSLMapData()->GetFieldUID( GetMapID(), kPacket.m_iFieldUID ) )
		{
			START_LOG( cerr, L"Invalid map information." )
				<< BUILD_LOG( GetMapID() )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
			SendPacket( EGS_FIELD_LOADING_COMPLETE_ACK, kPacketAck );
			return;
		}

		// 아직 필드에 입장해있지 않다!
		GetFieldUserInfo( kPacket.m_kFieldUserInfo );
		kPacket.m_kFieldUserInfo.m_vPos = kPacket_.m_vPos;
		kPacket.m_kFieldOption		    = m_kFieldOption;
		if( GetPartyUID() > 0 )
		{
			//{{ 2010. 03. 02  최육사	채널 통합 파티
			KEPM_REQUEST_UPDATE_PARTY_MEMBER_LIST_NOT kPacketToPM;
			kPacketToPM.m_iPartyUID = GetPartyUID();
			SendToCnParty( EPM_REQUEST_UPDATE_PARTY_MEMBER_LIST_NOT, kPacketToPM );
			//}}
		}

		// warning!! join_field 하기 전에 먼저 클라이언트로 부터 받은 위치 정보를 저장한다...
		// join field 실패하고 unit sync 패킷을 전송하기 전에 클라이언트 종료하면 위치정보에 오류 있을 수 있다.
		//{{ 2012. 02. 02	최육사	배틀필드 시스템
		m_kLastPos.m_ucLastTouchLineIndex	= kPacket_.m_ucLastTouchLineIndex;
		m_kLastPos.m_usLastPosValue			= kPacket_.m_usLastPosValue;
		//}}

		//{{ 2009. 4. 9  최육사		친구리스트
		std::map< std::wstring, UidType >::const_iterator mitF;
		for( mitF = m_mapFriendList.begin(); mitF != m_mapFriendList.end(); ++mitF )
		{
			kPacket.m_setFriendUIDList.insert( mitF->second );
		}
		//}}

		// 입장패킷 보내기전에 미리 FieldUID를 저장한다.
		SetFieldUID( kPacket.m_iFieldUID );
		LIF( GetCharUID() != 0 );
		_SendToGSField( GetFieldUID(), ERM_JOIN_FIELD_REQ, kPacket );
	}
}

IMPL_ON_FUNC( ERM_JOIN_FIELD_ACK )
{
	VERIFY_STATE( ( 3, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
	// 상태는 어떻게 되나?

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		LIF( GetFieldUID() == kPacket_.m_iFieldUID );

		// 필드 진입 성공
		SetFieldUID( kPacket_.m_iFieldUID );

		if( GetStateID() != KGSFSM::S_FIELD_MAP )
		{
			StateTransition( KGSFSM::I_TO_FIELD_MAP );
		}

		//{{ 2012. 04. 17	최육사	DB쿼리 성공 체크
		m_kUserQuestManager.Handler_OnEnterTheVillage( GetThisPtr<KGSUser>() );
		//}}

		//파티가 있을경우 필드이동 성공시 파티에 업데이트 함.
		SendUpdatePartyUnitInfo( true );

		//{{ 2009. 10. 8  최육사	길드
		SendUpdateGuildMemberInfo();
		//}}

		//{{ 버프 관리 시스템 - 김민성
		// 따뜻한 엘의 축복
		std::vector< KBuffInfo > vecActiveBuff;
		ActivateBuff( CXSLBuffManager::BTI_BUFF_WARM_ENERGE_OF_EL, vecActiveBuff );

		if( vecActiveBuff.empty() == false )
		{
			// 로그인 서버 버프 매니저에 업데이트
			KELG_UPDATE_USER_UNIT_BUFF_INFO_REQ kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_vecActivateBuff = vecActiveBuff;
			SendToLoginServer( ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ, kReq );
		}
		//}}

	}

	KEGS_FIELD_LOADING_COMPLETE_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
	//}}

	SendPacket( EGS_FIELD_LOADING_COMPLETE_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( EGS_LEAVE_FIELD_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	// 이탈 패킷
	SendLeaveField( NetError::NOT_LEAVE_FIELD_REASON_01 );
}

//{{ 2012. 11. 7	최육사	배틀필드 시스템
_IMPL_ON_FUNC( ERM_LEAVE_ROOM_FOR_ENTER_THE_VILLAGE_ACK, KEGS_LEAVE_ROOM_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( kPacket_.m_iOK );

	// 던전방에서 나갈경우 파티 탈퇴.
	const CXSLRoom::ROOM_TYPE eRoomType = CXSLRoom::GetRoomType( GetRoomUID() );

	//{{ 2012. 04. 16	박세훈	어둠의 문 개편
	std::vector< int > vecDeleteBuff;
	std::vector< KBuffInfo > vecActivateBuff;

	if( eRoomType == CXSLRoom::RT_DUNGEON )
	{
		//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
		m_kUserBuffManager.OnLeaveDungeonRoom( GetThisPtr<KGSUser>(), vecActivateBuff, vecDeleteBuff, false );
		//}
	}
	else if( eRoomType == CXSLRoom::RT_BATTLE_FIELD )
	{
		m_kUserBuffManager.OnLeaveBattleField( GetThisPtr<KGSUser>(), vecActivateBuff, vecDeleteBuff );
	}

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

//	if( eRoomType == CXSLRoom::RT_DUNGEON ) -- 이미 Leave Room Req에서 파티 이탈을 했기 때문에 여기서 다시 파티 이탈 처리를 하지는 않는다.
//	{
//		//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
//#ifdef SERV_DUNGEON_REQUIRED_ITEM_BUG
//		if( kPacket_.m_iReason == NetError::NOT_LEAVE_ROOM_REASON_22 )
//		{
//			SendLeaveParty( NetError::NOT_LEAVE_ROOM_REASON_22 );
//		}
//		else
//		{
//			SendLeaveParty( NetError::NOT_LEAVE_PARTY_REASON_02 );
//		}
//#else
//		SendLeaveParty( NetError::NOT_LEAVE_PARTY_REASON_02 );
//#endif SERV_DUNGEON_REQUIRED_ITEM_BUG
//		//}}
//	}

	bool bIsSuccess = false;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		bIsSuccess = true;
	}
	else
	{
		if( GetRoomUID() != 0  &&  kPacket_.m_iOK == NetError::ERR_ROOM_34 )
			bIsSuccess = true;
	}

	if( bIsSuccess == true )
	{
		SetRoomUID( 0 );
		m_kUserDungeonManager.SetDungeonGameInfo( 0, 0, 0 );

		StateTransition( KGSFSM::I_TO_FIELD_MAP );

		//SendUpdateUnitState( CXSLUnit::CUS_ONLINE );

		if( GetTradeUID() != 0 )
		{
			KEGS_BREAK_TRADE_REQ kPacket;
			kPacket.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_16;
			_SendToCnRoom( GetTradeUID(), ERM_BREAK_TRADE_REQ, kPacket );
		}
	}

	//SendPacket( EGS_LEAVE_ROOM_ACK, kPacket_ ); -- 마을 입장 중에 방을 이탈한것이므로 클라이언트에 패킷을 보내지 않아도 된다.
}
//}}

IMPL_ON_FUNC( EGS_FIELD_UNIT_SYNC_DATA_NOT )
{
	//////////////////////////////////////////////////////////////////////////
	// 필드 싱크 패킷 간격 체크 - 상태에러를 피하기위해 상태검사보다 위에서 체크합니다.
	if( m_kTimer[TM_FIELD_SYNC_GAP].elapsed() < 1.5 )
	{
		START_LOG( cwarn, L"Field sync interval is less than 1.5 seconds!" )
			<< BUILD_LOG( GetCharUID() );
		return;
	}

	// 패킷 통과 시키고 갭 타이머는 초기화
	m_kTimer[TM_FIELD_SYNC_GAP].restart();
	//////////////////////////////////////////////////////////////////////////

	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 필드 싱크
	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		if( GetFieldUID() == 0 )
		{
			START_LOG( cwarn, L"Received a sync packet with fieldUID 0." )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			// [참고] 대전채널 로비에 입장해 있다면 S_FIELD_MAP상태이면서 FieldUID가 0이 될수 있다.
			return;
		}

		SendToGSField( ERM_FIELD_UNIT_SYNC_DATA_NOT, kPacket_ );

		//{{ 2012. 02. 02	최육사	배틀필드 시스템
		m_kLastPos.m_ucLastTouchLineIndex	= kPacket_.m_ucLastTouchLineIndex;
		m_kLastPos.m_usLastPosValue			= kPacket_.m_usLastPosValue;
		//}}

		//////////////////////////////////////////////////////////////////////////
		//GetKGameServer()->AddFieldSyncCount();
		//////////////////////////////////////////////////////////////////////////
	}
	// 리플레이 방 싱크
	else if( GetStateID() == KGSFSM::S_ROOM )
	{
		if( GetRoomUID() == 0 )
		{
			START_LOG( cerr, L"Received a sync packet with RoomUID 0." )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}

		//{{ 2010. 03. 17  최육사	상태오류수정
		CXSLRoom::ROOM_TYPE eRoomType = CXSLRoom::GetRoomType( GetRoomUID() );
		switch( eRoomType )
		{
		case CXSLRoom::RT_DUNGEON:
			{
				SendToCnRoom( ERM_FIELD_UNIT_SYNC_DATA_NOT, kPacket_ );
			}
			break;

		case CXSLRoom::RT_SQUARE:
			{
				// 필드에서 광장 입장시 시간차로 발생할 수 있는 오류임.
				START_LOG( cwarn, L"Sent Field Sync while already in the plaza." )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;
			}
			break;

		default:
			{
				START_LOG( cerr, L"Sent field sync outside a dungeon or arcade room?" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( (int)eRoomType )
					<< END_LOG;
				return;
			}
			break;
		}
		//}}
	}
	else
	{
		START_LOG( cerr, L"Invalid state while sending field sync." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetStateID() )
			<< BUILD_LOG( GetStateIDString() )
			<< END_LOG;
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 로봇 스크립트 패턴 만들기
	//////////////////////////////////////////////////////////////////////////
}

_IMPL_ON_FUNC( ERM_FIELD_UNIT_SYNC_DATA_BROAD_NOT, KEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT )
{
	VERIFY_STATE_WARN( ( 1, KGSFSM::S_ROOM ) );

	// 던전 라운지로부터 오는 싱크 이벤트
	SendPacket( EGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT, kPacket_ );
}

//{{ 2009. 1. 13  최육사	필드 유저 리스트
IMPL_ON_FUNC( EGS_COMMUNITY_USER_LIST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_COMMUNITY_USER_LIST_ACK kPacket;
	kPacket.m_iOK		  = NetError::NET_OK;
	kPacket.m_uiTotalPage = 1;
	kPacket.m_uiViewPage  = 1;

	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		// 필드에 있다면..
		if( GetFieldUID() > 0 )
		{
			//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
			if( GetAuthLevel() >= SEnum::UAL_GM && GetEDMonitoringMode() == true )
			{
				kPacket_.m_bMode = true;
			}
			//}}
			SendToGSField( ERM_FIELD_USER_LIST_REQ, kPacket_ );
			return;
		}
		// 대전로비에 있다면..
		else if( GetRoomListID() > 0 )
		{
			if( !SiKPVPLobbyManager()->GetPVPUserList( kPacket_.m_uiViewPage, kPacket.m_uiTotalPage, kPacket.m_vecUserList ) )
			{
				START_LOG( cerr, L"Failed to retrieve the PvP user list!" )
					<< BUILD_LOG( kPacket_.m_uiViewPage )
					<< END_LOG;
			}
			else
			{
				kPacket.m_uiViewPage = kPacket_.m_uiViewPage;
			}
			goto end_proc;
		}
		// 여긴어디야?
		else
		{
			// 시간차로 발생가능함.
			START_LOG( cwarn, L"Where did this request come from?" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetFieldUID() )
				<< BUILD_LOG( GetRoomListID() )
				<< BUILD_LOG( GetStateIDString() )
				<< END_LOG;

			goto end_proc;
		}
	}
	else // KGSFSM::S_ROOM - 라운지
	{
		if( GetRoomUID() == 0 )
		{
			START_LOG( cerr, L"Requested the field user list while not in the dungeon lobby." )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_FIELD_04;
			goto end_proc;
		}

		//{{ 2009. 3. 20  최육사	대전방, 광장은 어떻게?
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
			{
				// 대전방이라면..
				if( !SiKPVPLobbyManager()->GetPVPUserList( kPacket_.m_uiViewPage, kPacket.m_uiTotalPage, kPacket.m_vecUserList ) )
				{
					START_LOG( cerr, L"Failed to retrieve the PvP user list!" )
						<< BUILD_LOG( kPacket_.m_uiViewPage )
						<< END_LOG;
				}
				else
				{
					kPacket.m_uiViewPage = kPacket_.m_uiViewPage;
				}
				goto end_proc;
			}
			break;
			//{{ 배틀필드 커뮤니티 리스트 확인 - 김민성
		case CXSLRoom::RT_BATTLE_FIELD:
			{
				// 배틀 필드이라면..룸에서 목록을 받아와야 함
				SendToCnRoom( ERM_COMMUNITY_IN_BATTLE_FIELD_REQ, kPacket_ );

				START_LOG( clog, L"Checking community information in battlefield" )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetRoomUID() )
					<< END_LOG;

				return;
			}
			break;
			//}}
		case CXSLRoom::RT_SQUARE:
			{
				START_LOG( cwarn, L"The client should not send this?" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< END_LOG;
				goto end_proc;
			}
			break;
		default:
			break;
		}
		//}}

		//{{ 2010. 03. 02  최육사	채널 통합 파티
		if( !SiKPartyListManager()->GetPartyUserList( GetPartyUID(), kPacket.m_vecUserList ) )
			//}}
		{
			START_LOG( cerr, L"Non-party user entered dungeon lobby and requested field user list?" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetPartyUID() )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_FIELD_04;
			goto end_proc;
		}
	}

end_proc:
	SendPacket( EGS_COMMUNITY_USER_LIST_ACK, kPacket );
}

_IMPL_ON_FUNC( ERM_FIELD_USER_LIST_ACK, KEGS_COMMUNITY_USER_LIST_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	SendPacket( EGS_COMMUNITY_USER_LIST_ACK, kPacket_ );
}
//}}

IMPL_ON_FUNC( EGS_OPTION_UPDATE_REQ )
{
	// 상태는 어떻게 되나?

	KEGS_OPTION_UPDATE_ACK kPacketAck;

	// 옵션 업데이트
	//if( !KFieldOption::VerifyLimitOption( kPacket_.m_kFieldOption.m_sLimitOption ) )
	//{
	//	kPacketAck.m_iOK = NetError::ERR_FIELD_02;
	//	SendPacket( EGS_OPTION_UPDATE_ACK, kPacketAck );
	//	return;
	//}

	m_kFieldOption = kPacket_.m_kFieldOption;

	kPacketAck.m_iOK = NetError::NET_OK;
	SendPacket( EGS_OPTION_UPDATE_ACK, kPacketAck );

	// 필드에 입장해 있다면 업데이트
	//if( m_iFieldUID > 0 )
	//{
	//	KERM_OPTION_UPDATE_NOT kPacket;
	//	kPacket.m_kFieldOption = kPacket_.m_kFieldOption;
	//	SendToGSField( ERM_OPTION_UPDATE_NOT, kPacket );
	//}
}
//}}

//{{ 2009. 4. 7  최육사		친구정보
IMPL_ON_FUNC( EGS_UPDATE_FRIEND_INFO_REQ )
{
	// 상태제한 둘까?

	KEGS_UPDATE_FRIEND_INFO_ACK kPacket;

	if( GetCharUID() <= 0 )
	{
		START_LOG( cwarn, L"Friend information requested before character selection?" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_wstrFriendNickName )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_UPDATE_FRIEND_00;
		SendPacket( EGS_UPDATE_FRIEND_INFO_ACK, kPacket );
		return;
	}

	if( kPacket_.m_bDelete )
	{
		// 친구리스트에서 삭제
		m_mapFriendList.erase( kPacket_.m_wstrFriendNickName );

		kPacket.m_iOK = NetError::ERR_UPDATE_FRIEND_00;
		SendPacket( EGS_UPDATE_FRIEND_INFO_ACK, kPacket );

		// 현재 위치가 Field라면 FieldUser의 친구리스트에서 해당 친구를 빼자!
		SendUpdateFriendList();
		return;
	}
	else
	{
		// 친구리스트에 이미 있는 친구인지 찾아보자!
		std::map< std::wstring, UidType >::const_iterator mit = m_mapFriendList.find( kPacket_.m_wstrFriendNickName );
		if( mit != m_mapFriendList.end() )
		{
			kPacket.m_iOK = NetError::NET_OK;
			kPacket.m_iFriendUnitUID = mit->second;
			SendPacket( EGS_UPDATE_FRIEND_INFO_ACK, kPacket );
			return;
		}

		// 친구 리스트에 없으면 로그인 서버로 친구찾으러 가자!
		SendToLoginServer( ELG_FIND_FRIEND_UNIT_INFO_REQ, kPacket_ );
		return;
	}
}

_IMPL_ON_FUNC( ELG_FIND_FRIEND_UNIT_INFO_ACK, KEGS_UPDATE_FRIEND_INFO_ACK )
{
	// 상태제한 둘까?

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 친구 리스트에 추가
		m_mapFriendList.insert( std::make_pair( kPacket_.m_wstrFriendNickName, kPacket_.m_iFriendUnitUID ) );

		// 현재 위치가 Field라면 FieldUser의 친구리스트에 추가하자!
		SendUpdateFriendList();
	}

	SendPacket( EGS_UPDATE_FRIEND_INFO_ACK, kPacket_ );
}
//}}

//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
IMPL_ON_FUNC( EGS_ED_MONITORING_BLOCK_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( GetAuthLevel() < SEnum::UAL_DEVELOPER )
	{
		START_LOG( cerr, L"Unauthorized attempt to use a developer-only feature." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetUserName() )
			<< END_LOG;
		return;
	}

	KEGS_ED_MONITORING_BLOCK_ACK kAck;
	kAck.m_iOK = NetError::NET_OK;

	KGSUserPtr spUser = GetKUserManager()->GetByCID( kPacket_.m_iUnitUID );
	if( spUser == NULL )
	{
		// 시간차로 유저가 존재하지 않을 수 있다.
		START_LOG( cwarn, L"User does not exist!" )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;

		kAck.m_iOK = NetError::ERR_ROOM_23;
		SendPacket( EGS_ED_MONITORING_BLOCK_ACK, kAck );
		return;
	}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;

	switch( kPacket_.m_ucType )
	{
	case KEGS_ED_MONITORING_BLOCK_REQ::EMBT_SEND_MAIL:
		{
			kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_ED_MONITORING_SEND_MAIL;
			spUser->SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );

			spUser->SetHackingUserSendType( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_ED_MONITORING_SEND_MAIL );
		}break;
	case KEGS_ED_MONITORING_BLOCK_REQ::EMBT_ACCOUNT_BLOCK:
		{
			kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_ED_MONITORING_ACCOUNT_BLOCK;
			spUser->HackUserRegRejectedUser( kPacket.m_cHackingCheckType );
			spUser->HackUserRegMonitorsUser( kPacket.m_cHackingCheckType );
			spUser->HackUserKick( kPacket );

		}break;
	case KEGS_ED_MONITORING_BLOCK_REQ::EMBT_MACHINE_ID_BLOCK:
		{
			kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_ED_MONITORING_MACHINE_ID_BLOCK;
			spUser->SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );

			spUser->SetHackingUserSendType( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_ED_MONITORING_MACHINE_ID_BLOCK );
		}break;
	default:
		{
			START_LOG( cerr, L"Invalid ED monitoring type." )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOGc( kPacket_.m_ucType )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_CLIENT_STATE;
			SendPacket( EGS_ED_MONITORING_BLOCK_ACK, kAck );
			return;
		}
	}
	//}}

	SendPacket( EGS_ED_MONITORING_BLOCK_ACK, kAck );
}

IMPL_ON_FUNC( EGS_COMMUNITY_USER_LIST_MONITORING_MODE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_COMMUNITY_USER_LIST_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_FIELD_04;

	// 필드에 있다면..
	if( GetFieldUID() > 0 )
	{
		if( GetAuthLevel() >= SEnum::UAL_GM && GetEDMonitoringMode() == true )
		{
			// 최신 정보를 필드로 가져 가서 갱신하자.

			KERM_FIELD_USER_LIST_MONITORING_MODE_REQ kReq;
			kReq.m_uiViewPage = kPacket_.m_uiViewPage;

			for( const UidType iUnitUID : kPacket_.m_vecUnitUIDList )
			{
				KGSUserPtr spReceiver = GetKUserManager()->GetByCID( iUnitUID );
				if( spReceiver == NULL )
				{
					// 시간차로 유저가 존재하지 않을 수 있다.
					START_LOG( cerr, L"User to invite does not exist!" )
						<< BUILD_LOG( GetCharName() )
						<< BUILD_LOG( iUnitUID )
						<< END_LOG;
					continue;
				}

				kReq.m_mapUnit_ED.insert( std::make_pair( iUnitUID, spReceiver->GetED() ) );
			}

			SendToGSField( ERM_FIELD_USER_LIST_MONITORING_MODE_REQ, kReq );
			return;
		}
	}
	else
	{
		if( GetRoomUID() == 0 )
		{
			START_LOG( cerr, L"Requested the field user list while not in the dungeon lobby." )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
		}

		//{{ 2009. 3. 20  최육사	대전방, 광장은 어떻게?
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
			{
				// 대전방이라면..
// 				if( !SiKPVPLobbyManager()->GetPVPUserList( kPacket_.m_uiViewPage, kPacket.m_uiTotalPage, kPacket.m_vecUserList ) )
// 				{
// 					START_LOG( cerr, L"pvp user list얻기 실패!" )
// 						<< BUILD_LOG( kPacket_.m_uiViewPage )
// 						<< END_LOG;
// 				}
// 				else
// 				{
// 					kPacket.m_uiViewPage = kPacket_.m_uiViewPage;
// 				}

			}break;
		case CXSLRoom::RT_SQUARE:
			{
				START_LOG( clog, L"The client does not handle this, so skipping." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< END_LOG;

				kPacket.m_iOK = NetError::NET_OK;
			}break;
		default:
			break;
		}
	}

	SendPacket( EGS_COMMUNITY_USER_LIST_ACK, kPacket );
}
//}}

//{{ 2012. 11. 15	박세훈	Field PT 로그 추가
IMPL_ON_FUNC( ERM_FIELD_USER_STATISTICS_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iGamePlayTime < 0 )
		kPacket_.m_iGamePlayTime = 0;

	// 유저 플레이 타임
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_TOTAL_PLAY_TIME, kPacket_.m_iGamePlayTime );
	m_kUserStatistics.AddPlayTime( KUserStatistics::PT_FIELD_PLAY_TIME, kPacket_.m_iGamePlayTime );
}
//}}

IMPL_ON_FUNC_NOPARAM( EGS_TOGGLE_INVISIBLE_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	// GM 이하면 실패
	if( GetAuthLevel() < SEnum::UAL_GM )
		return;

	// 미리 상태를 갱신
	ToggleInvisible();

	// 이후 전송
	KERM_TOGGLE_INVISIBLE_NOT kPacket;
	kPacket.m_iUnitUID = GetCharUID();
	GetFieldUserInfo( kPacket.m_kFieldUserInfo );

	SendToGSField( ERM_TOGGLE_INVISIBLE_NOT, kPacket );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
