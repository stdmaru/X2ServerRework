#include <winsock2.h>
#include "ClientPacket.h"



//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NEW_USER_JOIN_REQ, obj, ks )
{
	return PUT( m_wstrID )
		&& PUT( m_wstrPassword )
		&& PUT( m_wstrName )
		&& PUT( m_byteGuestUser )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_USER_JOIN_REQ, obj, ks )
{
	return GET( m_wstrID )
		&& GET( m_wstrPassword )
		&& GET( m_wstrName )
		&& GET( m_byteGuestUser )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_USER_JOIN_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUserUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_USER_JOIN_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUserUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, obj, ks )
{
	return PUT( m_iOK)
		&& PUT( m_nUnitSlot )
		&& PUT( m_vecUnitInfo )
		&& PUT(m_bSharingBank )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK, obj, ks )
{
	return GET( m_iOK)
		&& GET( m_nUnitSlot )
		&& GET( m_vecUnitInfo )
		&& GET(m_bSharingBank )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_UNIT_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_iClass )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_UNIT_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_iClass )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_UNIT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kUnitInfo )
		&& PUT( m_vecSkillSlot )
		&& PUT( m_wstrEnableDate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_UNIT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kUnitInfo )
		&& GET( m_vecSkillSlot )
		&& GET( m_wstrEnableDate )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DELETE_UNIT_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		//{{ 2012. 02. 21	김민성	캐릭터 삭제 및 길드 탈퇴 예외처리 수정
		&& PUT( m_iUserUID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_UNIT_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		//{{ 2012. 02. 21	김민성	캐릭터 삭제 및 길드 탈퇴 예외처리 수정
		&& GET( m_iUserUID )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DELETE_UNIT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_uiKNMSerialNum )
		//{{ 2009. 10. 8  최육사	길드
		&& PUT( m_iGuildUID )
		//}}
		//{{ 2011. 02. 23	최육사	캐릭터 로그
		&& PUT( m_wstrNickName )
		&& PUT( m_ucLevel )
		&& PUT( m_cUnitClass )
		//}}
		//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
		&& PUT( m_tReDelAbleDate )
		&& PUT( m_tDelAbleDate )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_UNIT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_uiKNMSerialNum )
		//{{ 2009. 10. 8  최육사	길드
		&& GET( m_iGuildUID )
		//}}
		//{{ 2011. 02. 23	최육사	캐릭터 로그
		&& GET( m_wstrNickName )
		&& GET( m_ucLevel )
		&& GET( m_cUnitClass )
		//}}
		//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
		&& GET( m_tReDelAbleDate )
		&& GET( m_tDelAbleDate )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 01. 04  최육사	인벤토리 기능개선
SERIALIZE_DEFINE_PUT( KEGS_GET_MY_INVENTORY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bIsRecommend )
		&& PUT( m_bSharingBank )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_MY_INVENTORY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bIsRecommend )
		&& GET( m_bSharingBank )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_SELECT_UNIT_INVENTORY_INFO_NOT, obj, ks )
{
	return PUT( m_iSlotCategory )
		&& PUT( m_mapItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_UNIT_INVENTORY_INFO_NOT, obj, ks )
{
	return GET( m_iSlotCategory )
		&& GET( m_mapItem )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_SERVER_SELECT_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_SERVER_SELECT_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KServerSetData, obj, ks )
{
	return PUT( serverSetID )
		&& PUT( serverSetName )
		&& PUT( userCountLevel )
		;
}

SERIALIZE_DEFINE_GET( KServerSetData, obj, ks )
{
	return GET( serverSetID )
		&& GET( serverSetName )
		&& GET( userCountLevel )
		;
}



//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_SERVER_SET_DATA_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_ServerSetList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_SERVER_SET_DATA_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_ServerSetList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELECT_SERVER_SET_REQ, obj, ks )
{
	return PUT( m_ServerSetID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_SERVER_SET_REQ, obj, ks )
{
	return GET( m_ServerSetID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELECT_SERVER_SET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrIP )
		&& PUT( m_usPort )
		&& PUT( m_kAccountBlockInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_SERVER_SET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrIP )
		&& GET( m_usPort )
		&& GET( m_kAccountBlockInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHECK_CHANNEL_CHANGE_REQ, obj, ks )
{
	return PUT( m_iChannelID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_CHANNEL_CHANGE_REQ, obj, ks )
{
	return GET( m_iChannelID )
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 5. 26  최육사	채널이동
SERIALIZE_DEFINE_PUT( KEGS_CONNECT_CHANNEL_CHANGE_REQ, obj, ks )
{
	return PUT( m_kConnectReq )
		&& PUT( m_kVerifyAccountReq )
		&& PUT( m_kSelectUnitReq )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CONNECT_CHANNEL_CHANGE_REQ, obj, ks )
{
	return GET( m_kConnectReq )
		&& GET( m_kVerifyAccountReq )
		&& GET( m_kSelectUnitReq )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CONNECT_CHANNEL_CHANGE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kConnectAck )
		&& PUT( m_kVerifyAccountAck )
		&& PUT( m_kChangeUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CONNECT_CHANNEL_CHANGE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kConnectAck )
		&& GET( m_kVerifyAccountAck )
		&& GET( m_kChangeUserInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_FIELD_CHANNEL_CHANGE_REQ, obj, ks )
{
	return PUT( m_iMapID )
		&& PUT( m_vPos )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_FIELD_CHANNEL_CHANGE_REQ, obj, ks )
{
	return GET( m_iMapID )
		&& GET( m_vPos )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ENTER_NPC_HOUSE_REQ, obj, ks )
{
	return PUT( m_iNPCID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENTER_NPC_HOUSE_REQ, obj, ks )
{
	return GET( m_iNPCID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KECH_GET_CHANNEL_LIST_NOT, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_mapChannelList )

		//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
		&& PUT( m_mapChannelBonusList )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KECH_GET_CHANNEL_LIST_NOT, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_mapChannelList )

		//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
		&& GET( m_mapChannelBonusList )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_CHANNEL_LIST_ACK, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_vecChannelInfo )
		&& PUT( m_mapChannelBonusInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_CHANNEL_LIST_ACK, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_vecChannelInfo )
		&& GET( m_mapChannelBonusInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KStateChangeReq, obj, ks )
{
	return PUT( m_nMapID );
}

SERIALIZE_DEFINE_GET( KStateChangeReq, obj, ks )
{
	return GET( m_nMapID );
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KStateChangeAck, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_nMapID )
		;
}

SERIALIZE_DEFINE_GET( KStateChangeAck, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_nMapID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ROOM_LIST_REQ, obj, ks )
{
	return PUT( m_nViewPage )
		&& PUT( m_cPVPGameType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ROOM_LIST_REQ, obj, ks )
{
	return GET( m_nViewPage )
		&& GET( m_cPVPGameType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ROOM_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_nTotalPage )
		&& PUT( m_nViewPage )
		&& PUT( m_vRoomInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ROOM_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_nTotalPage )
		&& GET( m_nViewPage )
		&& GET( m_vRoomInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_ROOM_REQ, obj, ks )
{
	return PUT( m_bObserver )
		&& PUT( m_RoomInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_ROOM_REQ, obj, ks )
{
	return GET( m_bObserver )
		&& GET( m_RoomInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_ROOM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		&& PUT( m_wstrCNIP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_ROOM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		&& GET( m_wstrCNIP )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2008. 12. 4  최육사	튜토리얼 던전
SERIALIZE_DEFINE_PUT( KEGS_CREATE_TUTORIAL_ROOM_REQ, obj, ks )
{
	return PUT( m_RoomInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_TUTORIAL_ROOM_REQ, obj, ks )
{
	return GET( m_RoomInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_TUTORIAL_ROOM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_wstrCNIP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_TUTORIAL_ROOM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_wstrCNIP )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_TC_ROOM_REQ, obj, ks )
{
	return PUT( m_iTCID );
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_TC_ROOM_REQ, obj, ks )
{
	return GET( m_iTCID );
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_TC_ROOM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kTCInfo )
		&& PUT( m_kRoomUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_TC_ROOM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kTCInfo )
		&& GET( m_kRoomUserInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_QUICK_JOIN_REQ, obj, ks )
{
    return	PUT( m_iOption )
		&&	PUT( m_bIsItemMode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUICK_JOIN_REQ, obj, ks )
{
    return	GET( m_iOption )
		&&	GET( m_bIsItemMode )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_QUICK_JOIN_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUICK_JOIN_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_ROOM_REQ, obj, ks )
{
	return PUT( m_iRoomUID )
		&& PUT( m_wstrPassword )
		&& PUT( m_bIsObserver )
		&& PUT( m_bInvitation )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_ROOM_REQ, obj, ks )
{
	return GET( m_iRoomUID )
		&& GET( m_wstrPassword )
		&& GET( m_bIsObserver )
		&& GET( m_bInvitation )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_ROOM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		&& PUT( m_wstrCNIP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_ROOM_ACK, obj, ks )
{
	return GET( m_iOK )
        && GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		&& GET( m_wstrCNIP )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_ROOM_NOT, obj, ks )
{
    return PUT( m_JoinSlot )
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& PUT( m_StartPosIndex )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_ROOM_NOT, obj, ks )
{
	return GET( m_JoinSlot )
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& GET( m_StartPosIndex )
		//}}
		;
}




//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_ROOM_REQ, obj, ks )
{
	return PUT( m_iReason )
		//{{ 2012. 04. 17	최육사	던전 로그 컬럼 추가
		&& PUT( m_kResultInfo )
		//}}
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& PUT( m_iBattleFieldID )
		//}}
		//{{ 2012. 06. 18	박세훈	던전 FPS 로그 추가
		//}}
		//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
		&& PUT( m_kGamePlayNetworkInfo )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_ROOM_REQ, obj, ks )
{
	return GET( m_iReason )
		//{{ 2012. 04. 17	최육사	던전 로그 컬럼 추가
		&& GET( m_kResultInfo )
		//}}
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& GET( m_iBattleFieldID )
		//}}
		//{{ 2012. 06. 18	박세훈	던전 FPS 로그 추가
		//}}
		//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
		&& GET( m_kGamePlayNetworkInfo )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_ROOM_NOT, obj, ks )
{
    return PUT( m_cRoomState )
		&& PUT( m_iUnitUID )
		&& PUT( m_iReason )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_ROOM_NOT, obj, ks )
{
	return GET( m_cRoomState )
		&& GET( m_iUnitUID )
		&& GET( m_iReason )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_SQUARE_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_SQUARE_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iReason )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_GAME_NOT, obj, ks )
{
    return PUT( m_cRoomState )
		&& PUT( m_iUnitUID )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_GAME_NOT, obj, ks )
{
    return GET( m_cRoomState )
		&& GET( m_iUnitUID )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BAN_USER_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BAN_USER_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_TEAM_REQ, obj, ks )
{
	return PUT( m_TeamNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_TEAM_REQ, obj, ks )
{
	return GET( m_TeamNum )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_TEAM_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_TEAM_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_TEAM_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_TeamNum )
		&& PUT( m_SwapSlotIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_TEAM_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_TeamNum )
		&& GET( m_SwapSlotIndex )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_READY_REQ, obj, ks )
{
	return PUT( m_bReady )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_READY_REQ, obj, ks )
{
	return GET( m_bReady )
		;
}

//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_READY_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_READY_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_READY_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_bReady )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_READY_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_bReady )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PITIN_REQ, obj, ks )
{
	return PUT( m_bPitIn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PITIN_REQ, obj, ks )
{
	return GET( m_bPitIn )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PITIN_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_bPitIn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PITIN_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_bPitIn )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MAP_REQ, obj, ks )
{
	return PUT( m_WorldID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MAP_REQ, obj, ks )
{
	return GET( m_WorldID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MAP_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MAP_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MAP_NOT, obj, ks )
{
	return PUT( m_WorldID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MAP_NOT, obj, ks )
{
	return GET( m_WorldID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SLOT_OPEN_REQ, obj, ks )
{
	return PUT( m_SlotIndex )
		&& PUT( m_SlotState )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SLOT_OPEN_REQ, obj, ks )
{
	return GET( m_SlotIndex )
		&& GET( m_SlotState )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SLOT_OPEN_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SLOT_OPEN_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SLOT_OPEN_NOT, obj, ks )
{
	return PUT( m_SlotIndex )
		&& PUT( m_TeamBalanceSlotIndex )
		&& PUT( m_SlotState )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SLOT_OPEN_NOT, obj, ks )
{
	return GET( m_SlotIndex )
		&& GET( m_TeamBalanceSlotIndex )
		&& GET( m_SlotState )
		;
}

//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PLAY_TIME_LIMIT_REQ, obj, ks )
{
	return PUT( m_fPlayTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PLAY_TIME_LIMIT_REQ, obj, ks )
{
	return GET( m_fPlayTime )
		;
}

//////////////////////////////////////////////////////////////////////////


SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PLAY_TIME_LIMIT_NOT, obj, ks )
{
	return PUT( m_fPlayTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PLAY_TIME_LIMIT_NOT, obj, ks )
{
	return GET( m_fPlayTime )
		;
}

/////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_WINNING_NUM_KILL_REQ, obj, ks )
{
	return PUT( m_iWinKillNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_WINNING_NUM_KILL_REQ, obj, ks )
{
	return GET( m_iWinKillNum )
		;
}

//////////////////////////////////////////////////////////////////////////


SERIALIZE_DEFINE_PUT( KEGS_CHANGE_WINNING_NUM_KILL_NOT, obj, ks )
{
	return PUT( m_iWinKillNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_WINNING_NUM_KILL_NOT, obj, ks )
{
	return GET( m_iWinKillNum )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHAT_REQ, obj, ks )
{
	return PUT( m_cChatPacketType )
		&& PUT( m_cRoomChatType )
		&& PUT( m_ToUnitUID )
		&& PUT( m_wstrRecieverUnitNickName )
		&& PUT( m_wstrMsg )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHAT_REQ, obj, ks )
{
	return GET( m_cChatPacketType )
		&& GET( m_cRoomChatType )
		&& GET( m_ToUnitUID )
		&& GET( m_wstrRecieverUnitNickName )
		&& GET( m_wstrMsg )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHAT_NOT, obj, ks )
{
	return PUT( m_cChatPacketType )
		&& PUT( m_SenderUnitUID )
		&& PUT( m_wstrSenderUnitNickName )
		&& PUT( m_wstrRecieverUnitNickName )
		&& PUT( m_RecieverUnitUID )
		&& PUT( m_cRoomChatType )
		&& PUT( m_wstrMsg )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHAT_NOT, obj, ks )
{
	return GET( m_cChatPacketType )
		&& GET( m_SenderUnitUID )
		&& GET( m_wstrSenderUnitNickName )
		&& GET( m_wstrRecieverUnitNickName )
		&& GET( m_RecieverUnitUID )
		&& GET( m_cRoomChatType )
		&& GET( m_wstrMsg )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MOTION_REQ, obj, ks )
{
	return PUT( m_cMotionID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MOTION_REQ, obj, ks )
{
	return GET( m_cMotionID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MOTION_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MOTION_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MOTION_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_cMotionID )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MOTION_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_cMotionID )
        ;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2008. 5. 27  최육사  랜덤맵
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_GAME_START_REQ, obj, ks )
{
	return PUT( m_sWorldID )
		&& PUT( m_iDungeonID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_GAME_START_REQ, obj, ks )
{
	return GET( m_sWorldID )
		&& GET( m_iDungeonID )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_GAME_START_NOT, obj, ks )
{
    return PUT( m_cRoomState )
		&& PUT( m_iGameType )
		&& PUT( m_iSpirit )
		&& PUT( m_sWorldID )
		&& PUT( m_vecSlot )
		&& PUT( m_vecInventorySlotInfo )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_GAME_START_NOT, obj, ks )
{
    return GET( m_cRoomState )
		&& GET( m_iGameType )
		&& GET( m_iSpirit )
		&& GET( m_sWorldID )
		&& GET( m_vecSlot )
		&& GET( m_vecInventorySlotInfo )
        ;
}

/////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_GAME_INTRUDE_NOT, obj, ks )
{
	return PUT( m_RoomState )
		&& PUT( m_IntruderSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_GAME_INTRUDE_NOT, obj, ks )
{
	return GET( m_RoomState )
		&& GET( m_IntruderSlot )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_LOADING_REQ, obj, ks )
{
	return PUT( m_iLoadingProgress )
		//{{ 2012. 12. 21  서브 퀘스트 타입 추가 (적정 레벨 던전 클리어) - 김민성
		&& PUT( m_iUnitLevel )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_LOADING_REQ, obj, ks )
{
	return GET( m_iLoadingProgress )
		//{{ 2012. 12. 21  서브 퀘스트 타입 추가 (적정 레벨 던전 클리어) - 김민성
		&& GET( m_iUnitLevel )
		//}}
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_LOADING_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iLoadingProgress )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_LOADING_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iLoadingProgress )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_P2P_CONNECT_RESULT_REQ, obj, ks )
{
	return PUT( m_bConnectSuccess )
		&& PUT( m_UnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_P2P_CONNECT_RESULT_REQ, obj, ks )
{
	return GET( m_bConnectSuccess )
		&& GET( m_UnitUID )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_P2P_CONNECT_RESULT_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_P2P_CONNECT_RESULT_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PLAY_START_NOT, obj, ks )
{
	return PUT( m_RoomState )
		&& PUT( m_vecSlot )
		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		&& PUT( m_iDungeonID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_PLAY_START_NOT, obj, ks )
{
	return GET( m_RoomState )
		&& GET( m_vecSlot )
		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		&& GET( m_iDungeonID )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INTRUDE_START_REQ, obj, ks )
{
	return PUT( m_StartPosIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INTRUDE_START_REQ, obj, ks )
{
	return GET( m_StartPosIndex )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INTRUDE_START_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INTRUDE_START_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INTRUDE_START_NOT, obj, ks )
{
	return PUT( m_RoomState )
		&& PUT( m_StartPosIndex )
		&& PUT( m_IntruderSlot )
		//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
		&& PUT( m_bObserver )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_INTRUDE_START_NOT, obj, ks )
{
	return GET( m_RoomState )
		&& GET( m_StartPosIndex )
		&& GET( m_IntruderSlot )
		//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
		&& GET( m_bObserver )
		//}}
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CURRENT_KILL_SCORE_NOT, obj, ks )
{
	return PUT( m_mapKillScore )
		&& PUT( m_mapMDKillScore )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CURRENT_KILL_SCORE_NOT, obj, ks )
{
	return GET( m_mapKillScore )
		&& GET( m_mapMDKillScore )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CURRENT_TEAM_KILL_SCORE_NOT, obj, ks )
{
	return PUT( m_RedTeamKill )
		&& PUT( m_BlueTeamKill )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CURRENT_TEAM_KILL_SCORE_NOT, obj, ks )
{
	return GET( m_RedTeamKill )
		&& GET( m_BlueTeamKill )
		;
}



/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_MY_USER_UNIT_INFO_TO_SERVER_REQ, obj, ks )
{
		//{{ 2011. 01. 21	최육사	던전 랭크 개편
	return PUT( m_kMyPlayResult )
		//}}

		//{{ 2012. 06. 18	박세훈	던전 FPS 로그 추가
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_MY_USER_UNIT_INFO_TO_SERVER_REQ, obj, ks )
{
		//{{ 2011. 01. 21	최육사	던전 랭크 개편
	return GET( m_kMyPlayResult )
		//}}

		//{{ 2012. 06. 18	박세훈	던전 FPS 로그 추가
		//}}
		;
}

//{{ 2008. 9. 19  최육사	통계
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT, obj, ks )
{
	return PUT( m_ComboScore )
		&& PUT( m_TechScore )
		&& PUT( m_nDamageCount )
	//{{ 2011. 01. 21	최육사	던전 랭크 개편
		&& PUT( m_iTotalAttackedDamage )
		&& PUT( m_iTotalRecoveryHP )
		&& PUT( m_iUsingKindOfSkill )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT, obj, ks )
{
	return GET( m_ComboScore )
		&& GET( m_TechScore )
		&& GET( m_nDamageCount )
		//{{ 2011. 01. 21	최육사	던전 랭크 개편
		&& GET( m_iTotalAttackedDamage )
		&& GET( m_iTotalRecoveryHP )
		&& GET( m_iUsingKindOfSkill )
		//}}
		;
}
//}}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KDungeonUnitResultInfo, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_bIsDie )
		&& PUT( m_nTotalScore )
		&& PUT( m_nComboScore )
		&& PUT( m_nTechnicalScore )
		&& PUT( m_nDamaged )
		//{{ 2009. 6. 10  최육사	게임시작시 EXP, ED
		&& PUT( m_nOldED )
		&& PUT( m_nOldEXP )
		//}}
		&& PUT( m_nED )
		&& PUT( m_nEXP )
		&& PUT( m_nClearBonusEXP )
		&& PUT( m_nRankBonusEXP )
		&& PUT( m_nPartyBonusEXP )
		&& PUT( m_nMvpBonusEXP )
		&& PUT( m_nGameBangBonusEXP )
		&& PUT( m_nPremiumBonusEXP )
		&& PUT( m_nEventEXP )
		//{{ 2009. 5. 6  최육사		채널보너스
		&& PUT( m_nChannelBonusEXP )
		//}}
		&& PUT( m_nKillNPCNum )
		&& PUT( m_cComboRank )
		&& PUT( m_cTechnicalRank )
		&& PUT( m_cDamagedRank )
		&& PUT( m_cTimeRank )
		&& PUT( m_cTotalRank )
		&& PUT( m_bIsMVP )
		;
}

SERIALIZE_DEFINE_GET( KDungeonUnitResultInfo, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_bIsDie )
		&& GET( m_nTotalScore )
		&& GET( m_nComboScore )
		&& GET( m_nTechnicalScore )
		&& GET( m_nDamaged )
		//{{ 2009. 6. 10  최육사	게임시작시 EXP, ED
		&& GET( m_nOldED )
		&& GET( m_nOldEXP )
		//}}
		&& GET( m_nED )
		&& GET( m_nEXP )
		&& GET( m_nClearBonusEXP )
		&& GET( m_nRankBonusEXP )
		&& GET( m_nPartyBonusEXP )
		&& GET( m_nMvpBonusEXP )
		&& GET( m_nGameBangBonusEXP )
		&& GET( m_nPremiumBonusEXP )
		&& GET( m_nEventEXP )
		//{{ 2009. 5. 6  최육사		채널보너스
		&& GET( m_nChannelBonusEXP )
		//}}
		&& GET( m_nKillNPCNum )
		&& GET( m_cComboRank )
		&& GET( m_cTechnicalRank )
		&& GET( m_cDamagedRank )
		&& GET( m_cTimeRank )
		&& GET( m_cTotalRank )
		&& GET( m_bIsMVP )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT, obj, ks )
{
	return PUT( m_nSec )
		&& PUT( m_bIsWin )
		&& PUT( m_iDungeonID )
		&& PUT( m_cDifficulty )
		//{{ 2010. 05. 01  최육사	비밀던전 헬모드
		&& PUT( m_cDungeonMode )
		//}}
		&& PUT( m_vecDungeonUnitInfo )
		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		&& PUT( m_mapOngoingQuest )
		//}}
		//{{ 2009. 8. 25  최육사	신학기 이벤트
		&& PUT( m_bIsHenirDungeon )
		&& PUT( m_iDungeonEventPoint )
		&& PUT( m_iHenirEventPoint )
		//}}
		//{{ 2011. 03. 08	김민성 경험치를 획득한 던전의 퀘스트 완료 조건 변경
		&& PUT( m_mapHaveExpInDungeon )			// 던전에서 경험치를 얻은저기 있는가?
	//}}
	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
		&& PUT( m_bIsClearDefenceDungeon )
		&& PUT( m_iProtectedSmallCrystal )
	//}} 2011. 04. 13  김민성  글로벌 서버 추가
	//{{ 버프 관리 시스템 - 김민성
		&& PUT( m_bComeBackUserInParty )
	//}
	//{{ 2012. 12. 21  서브 퀘스트 타입 추가 (적정 레벨 던전 클리어) - 김민성
		&& PUT( m_mapSuitableLevelInfo )
	//}}
		&& PUT( m_bSpecChar1 )
		&& PUT( m_bSpecChar2 )

		&& PUT( m_vecRelation )

		;
}

SERIALIZE_DEFINE_GET( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT, obj, ks )
{
	return GET( m_nSec )
		&& GET( m_bIsWin )
		&& GET( m_iDungeonID )
		&& GET( m_cDifficulty )
		//{{ 2010. 05. 01  최육사	비밀던전 헬모드
		&& GET( m_cDungeonMode )
		//}}
		&& GET( m_vecDungeonUnitInfo )
		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		&& GET( m_mapOngoingQuest )
		//}}
		//{{ 2009. 8. 25  최육사	신학기 이벤트
		&& GET( m_bIsHenirDungeon )
		&& GET( m_iDungeonEventPoint )
		&& GET( m_iHenirEventPoint )
		//}}
		//{{ 2011. 03. 08	김민성 경험치를 획득한 던전의 퀘스트 완료 조건 변경
		&& GET( m_mapHaveExpInDungeon )			// 던전에서 경험치를 얻은저기 있는가?
		//}}
		//{{ 2011. 04. 13  김민성  글로벌 서버 추가
		&& GET( m_bIsClearDefenceDungeon )
		&& GET( m_iProtectedSmallCrystal )
		//}} 2011. 04. 13  김민성  글로벌 서버 추가
		//{{ 버프 관리 시스템 - 김민성
		&& GET( m_bComeBackUserInParty )
		//}
		//{{ 2012. 12. 21  서브 퀘스트 타입 추가 (적정 레벨 던전 클리어) - 김민성
		&& GET( m_mapSuitableLevelInfo )
		//}}

		&& GET( m_bSpecChar1 )
		&& GET( m_bSpecChar2 )

		&& GET( m_vecRelation )

		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KPVPUnitResultInfo, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_nRanking )
		&& PUT( m_nKillNum )
		&& PUT( m_nMDKillNum )
		&& PUT( m_nDeathNum )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& PUT( m_iRating )
		&& PUT( m_iRPoint )
		&& PUT( m_iAPoint )

		//{{ 2012. 06. 22	박세훈	2012 대전 시즌2
		&& PUT( m_fKFactorIncrement )
		//}}

		//}}
		&& PUT( m_nEXP )
		&& PUT( m_bIsMVP )
		&& PUT( m_bWin )
		;
}

SERIALIZE_DEFINE_GET( KPVPUnitResultInfo, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_nRanking )
		&& GET( m_nKillNum )
		&& GET( m_nMDKillNum )
		&& GET( m_nDeathNum )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& GET( m_iRating )
		&& GET( m_iRPoint )
		&& GET( m_iAPoint )

		//{{ 2012. 06. 22	박세훈	2012 대전 시즌2
		&& GET( m_fKFactorIncrement )
		//}}

		//}}
		&& GET( m_nEXP )
		&& GET( m_bIsMVP )
		&& GET( m_bWin )
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_GAME_PVP_RESULT_DATA_NOT, obj, ks )
{
	return PUT( m_cWinTeam )
		&& PUT( m_bIsDrawn )
		//{{ 2011. 07. 19	최육사	대전 개편
		&& PUT( m_bIsAbnormalEndPlay )
		//}}
		&& PUT( m_iPlayTime )
		&& PUT( m_iGameType )
		&& PUT( m_iSpirit )
		&& PUT( m_uiChannelID )
		&& PUT( m_vecPVPUnitInfo )
		//{{ 2011. 07. 25    김민성    대전 퀘스트 조건 추가
//{{ 2012. 02. 22	김민성	대전 퀘스트, PVP NPC 관련 퀘스트	오류 수정
//}}
		&& PUT( m_bOfficialMatch )
		//}}
		&& PUT( m_bIsHeroNPC )
		&& PUT( m_iRematchCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_END_GAME_PVP_RESULT_DATA_NOT, obj, ks )
{
	return GET( m_cWinTeam )
		&& GET( m_bIsDrawn )
		//{{ 2011. 07. 19	최육사	대전 개편
		&& GET( m_bIsAbnormalEndPlay )
		//}}
		&& GET( m_iPlayTime )
		&& GET( m_iGameType )
		&& GET( m_iSpirit )
		&& GET( m_uiChannelID )
		&& GET( m_vecPVPUnitInfo )
		//{{ 2011. 07. 25    김민성    대전 퀘스트 조건 추가
//{{ 2012. 02. 22	김민성	대전 퀘스트, PVP NPC 관련 퀘스트	오류 수정
//}}
		&& GET( m_bOfficialMatch )
		//}}
		&& GET( m_bIsHeroNPC )
		&& GET( m_iRematchCount )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_RE_BIRTH_POS_REQ, obj, ks )
{
	return PUT( m_StartPosIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_RE_BIRTH_POS_REQ, obj, ks )
{
	return GET( m_StartPosIndex )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_RE_BIRTH_NOT, obj, ks )
{
	return PUT( m_UserUnitUID )
		&& PUT( m_StartPosIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_RE_BIRTH_NOT, obj, ks )
{
	return GET( m_UserUnitUID )
		&& GET( m_StartPosIndex )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2007. 8. 28  최육사  level -> VP
SERIALIZE_DEFINE_PUT( KPVPChannelInfo, obj, ks )
{
	//{{ 2011. 07. 11	최육사	대전 개편
	return PUT( m_iMinRating )
		&& PUT( m_iMaxRating )
	//}}
		&& PUT( m_iChannelID )
		&& PUT( m_cPVPChannelClass )
        ;
}

SERIALIZE_DEFINE_GET( KPVPChannelInfo, obj, ks )
{
	//{{ 2011. 07. 11	최육사	대전 개편
	return GET( m_iMinRating )
		&& GET( m_iMaxRating )
		//}}
		&& GET( m_iChannelID )
		&& GET( m_cPVPChannelClass )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_OPEN_PVP_ROOM_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iConnectChannelID )
		&& PUT( m_vecChannel )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_OPEN_PVP_ROOM_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iConnectChannelID )
		&& GET( m_vecChannel )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2008. 12. 1  최육사	대전방 초대
SERIALIZE_DEFINE_PUT( KEGS_INVITE_PVP_ROOM_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PVP_ROOM_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_INVITE_PVP_ROOM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PVP_ROOM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_INVITE_PVP_ROOM_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_kRoomInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PVP_ROOM_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_kRoomInfo )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_DIE_REQ, obj, ks )
{
	return PUT( m_KillerNPCUID )
		&& PUT( m_KillerUserUnitUID )
		&& PUT( m_MaxDamageKillerUserUnitUID )
	//{{ 2011. 09. 19  김민성	칭호 획득 조건 추가
		&& PUT( m_ucDieReason )
	//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_DIE_REQ, obj, ks )
{
	return GET( m_KillerNPCUID )
		&& GET( m_KillerUserUnitUID )
		&& GET( m_MaxDamageKillerUserUnitUID )
	//{{ 2011. 09. 19  김민성	칭호 획득 조건 추가
		&& GET( m_ucDieReason )
	//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_DIE_ACK, obj, ks )
{
	return PUT( m_fReBirthTime )
		&& PUT( m_iOK )
		&& PUT( m_VP )
		&& PUT( m_EXP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_DIE_ACK, obj, ks )
{
	return GET( m_fReBirthTime )
		&& GET( m_iOK )
		&& GET( m_VP )
		&& GET( m_EXP )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_DIE_NOT, obj, ks )
{
	return PUT( m_KillerNPCUID )
		&& PUT( m_KillerUserUnitUID )
		&& PUT( m_MaxDamageKillerUserUnitUID )
		&& PUT( m_KilledUserUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_DIE_NOT, obj, ks )
{
	return GET( m_KillerNPCUID )
		&& GET( m_KillerUserUnitUID )
		&& GET( m_MaxDamageKillerUserUnitUID )
		&& GET( m_KilledUserUnitUID )
		;
}


/////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USER_UNIT_DIE_COMPLETE_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USER_UNIT_DIE_COMPLETE_NOT, obj, ks )
{
	return GET( m_UnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_CREATE_REQ, obj, ks )
{
	return PUT( m_vecNPCUnitReq )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_CREATE_REQ, obj, ks )
{
	return GET( m_vecNPCUnitReq )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KNPCUnitNot, obj, ks )
{
	return PUT( m_kNPCUnitReq )
		;
}

SERIALIZE_DEFINE_GET( KNPCUnitNot, obj, ks )
{
	return GET( m_kNPCUnitReq )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_CREATE_NOT, obj, ks )
{
	return PUT( m_vecNPCUnitAck )
		//{{ 2011. 11. 18	최육사	배틀필드 시스템
		&& PUT( m_mapAttribNpcInfo )
		&& PUT( m_iBattleFieldID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_CREATE_NOT, obj, ks )
{
	return GET( m_vecNPCUnitAck )
		//{{ 2011. 11. 18	최육사	배틀필드 시스템
		&& GET( m_mapAttribNpcInfo )
		&& GET( m_iBattleFieldID )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_DIE_REQ, obj, ks )
{
	return PUT( m_nDieNPCUID )
		&& PUT( m_cAttUnitType )
		&& PUT( m_uiAttUnit )
		&& PUT( m_uiMDAttUnit )
		&& PUT( m_DiePos )
		&& PUT( m_cNpcDieState )
		//{{ 2011. 11. 22	최육사	배틀필드 시스템
		&& PUT( m_iNpcMaxHP )
		&& PUT( m_mapDamageByUser )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_DIE_REQ, obj, ks )
{
	return GET( m_nDieNPCUID )
		&& GET( m_cAttUnitType )
		&& GET( m_uiAttUnit )
		&& GET( m_uiMDAttUnit )
		&& GET( m_DiePos )
		&& GET( m_cNpcDieState )
		//{{ 2011. 11. 22	최육사	배틀필드 시스템
		&& GET( m_iNpcMaxHP )
		&& GET( m_mapDamageByUser )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_DIE_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_DIE_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_DIE_NOT, obj, ks )
{
	return PUT( m_nDieNPCUID )
		&& PUT( m_iNPCID )
		&& PUT( m_cAttUnitType )
		&& PUT( m_uiAttUnit )
		&& PUT( m_EXPList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_DIE_NOT, obj, ks )
{
	return GET( m_nDieNPCUID )
		&& GET( m_iNPCID )
		&& GET( m_cAttUnitType )
		&& GET( m_uiAttUnit )
		&& GET( m_EXPList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_STAGE_LOAD_REQ, obj, ks )
{
	return PUT( m_iStageID )
		//{{ 2012. 02. 28	최육사	배틀필드 시스템
		&& PUT( m_sPartyFever )
		//}}

		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_STAGE_LOAD_REQ, obj, ks )
{
	return GET( m_iStageID )
		//{{ 2012. 02. 28	최육사	배틀필드 시스템
		&& GET( m_sPartyFever )
		//}}

		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_STAGE_LOAD_NOT, obj, ks )
{
	return PUT( m_iStageID )
		&& PUT( m_iNumMember )
		&& PUT( m_mapNPCData )
		&& PUT( m_mapAttribNpcInfo )
		//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장
		&& PUT( m_fItemDropEventProbRate )
		&& PUT( m_bWithPlayPcBangEvent )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_STAGE_LOAD_NOT, obj, ks )
{
	return GET( m_iStageID )
		&& GET( m_iNumMember )
		&& GET( m_mapNPCData )
		&& GET( m_mapAttribNpcInfo )
		//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장
		&& GET( m_fItemDropEventProbRate )
		&& GET( m_bWithPlayPcBangEvent )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 7. 21  최육사	비밀 스테이지
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_STAGE_LOAD_COMPLETE_REQ, obj, ks )
{
	return PUT( m_bIsSecretStage )
		//{{ 2011. 06. 13	최육사	스테이지 로그
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_STAGE_LOAD_COMPLETE_REQ, obj, ks )
{
	return GET( m_bIsSecretStage )
		//{{ 2011. 06. 13	최육사	스테이지 로그
		//}}
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_STAGE_LOAD_COMPLETE_NOT, obj, ks )
{
	return PUT( m_iSlotID )
        && PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_STAGE_LOAD_COMPLETE_NOT, obj, ks )
{
	return GET( m_iSlotID )
        && GET( m_iUnitUID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_STAGE_START_NOT, obj, ks )
{
	return PUT( m_iStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_STAGE_START_NOT, obj, ks )
{
	return GET( m_iStageID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_OPEN_REQ, obj, ks )
{
	return PUT( m_iSubStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_OPEN_REQ, obj, ks )
{
	return GET( m_iSubStageID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_OPEN_NOT, obj, ks )
{
	return PUT( m_iSubStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_OPEN_NOT, obj, ks )
{
	return GET( m_iSubStageID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ, obj, ks )
{
	return PUT( m_iBeforeStageID )
		&& PUT( m_iNextStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ, obj, ks )
{
	return GET( m_iBeforeStageID )
		&& GET( m_iNextStageID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT, obj, ks )
{
	return PUT( m_iBeforeStageID )
		&& PUT( m_iNextStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT, obj, ks )
{
	return GET( m_iBeforeStageID )
		&& GET( m_iNextStageID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ, obj, ks )
{
	return PUT( m_iSubStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ, obj, ks )
{
	return GET( m_iSubStageID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_START_NOT, obj, ks )
{
	return PUT( m_iSubStageID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_START_NOT, obj, ks )
{
	return GET( m_iSubStageID )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_GAME_REQ, obj, ks )
{
	return PUT( m_iWinTeam )
		;
}

SERIALIZE_DEFINE_GET( KEGS_END_GAME_REQ, obj, ks )
{
	return GET( m_iWinTeam )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_GAME_NOT, obj, ks )
{
    return PUT( m_cRoomState )
		&& PUT( m_iWinTeam )
		&& PUT( m_vecSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_END_GAME_NOT, obj, ks )
{
    return GET( m_cRoomState )
		&& GET( m_iWinTeam )
		&& GET( m_vecSlot )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_RESULT_REQ, obj, ks )
{
	return PUT( m_Dummy )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_RESULT_REQ, obj, ks )
{
	return GET( m_Dummy )
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_RESULT_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_RESULT_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_RESULT_NOT, obj, ks )
{
	return PUT( m_cRoomState )
		&& PUT( m_vecSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_RESULT_NOT, obj, ks )
{
	return GET( m_cRoomState )
		&& GET( m_vecSlot )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELECT_REWARD_BOX_REQ, obj, ks )
{
	return PUT( m_BoxNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_REWARD_BOX_REQ, obj, ks )
{
	return GET( m_BoxNum )
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELECT_REWARD_BOX_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_REWARD_BOX_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELECT_REWARD_BOX_NOT, obj, ks )
{
	return PUT( m_BoxNum )
		&& PUT( m_UnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELECT_REWARD_BOX_NOT, obj, ks )
{
	return GET( m_BoxNum )
		&& GET( m_UnitUID )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KDropItemData, obj, ks )
{
	return PUT( m_iItemID )
		&& PUT( m_iDropItemUID )
		&& PUT( m_bLeft )
		&& PUT( m_fSpeed )
		//{{ 2012. 02. 27	최육사	배틀필드 시스템
		&& PUT( m_setGetPossibleUnitUID )
		//}}
		//{{ 2013. 1. 14	박세훈	던전 및 필드 드랍 아이템에 강화 부여
		&& PUT( m_cEnchantLevel )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KDropItemData, obj, ks )
{
	return GET( m_iItemID )
		&& GET( m_iDropItemUID )
		&& GET( m_bLeft )
		&& GET( m_fSpeed )
		//{{ 2012. 02. 27	최육사	배틀필드 시스템
		&& GET( m_setGetPossibleUnitUID )
		//}}
		//{{ 2013. 1. 14	박세훈	던전 및 필드 드랍 아이템에 강화 부여
		&& GET( m_cEnchantLevel )
		//}}
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DROP_ITEM_NOT, obj, ks )
{
	return PUT( m_DropItemDataList )
		&& PUT( m_CreatePos )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DROP_ITEM_NOT, obj, ks )
{
	return GET( m_DropItemDataList )
		&& GET( m_CreatePos )
		;
}


/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_ITEM_REQ, obj, ks )
{
	return PUT( m_iDropItemUID )
		&& PUT( m_kItemInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_ITEM_REQ, obj, ks )
{
	return GET( m_iDropItemUID )
		&& GET( m_kItemInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 5. 11  최육사	실시간아이템
SERIALIZE_DEFINE_PUT( KEGS_GET_ITEM_REALTIME_NOT, obj, ks )
{
	return PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_ITEM_REALTIME_NOT, obj, ks )
{
	return GET( m_vecKInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_ITEM_TEMP_INVENTORY_NOT, obj, ks )
{
	return PUT( m_kTempInvenItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_ITEM_TEMP_INVENTORY_NOT, obj, ks )
{
	return GET( m_kTempInvenItem )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_TEMP_ITEM_REQ, obj, ks )
{
	return PUT( m_iTempItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_TEMP_ITEM_REQ, obj, ks )
{
	return GET( m_iTempItemUID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_TEMP_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iItemID )
		&& PUT( m_iTempItemUID )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_TEMP_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iItemID )
		&& GET( m_iTempItemUID )
		&& GET( m_vecInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DELETE_TEMP_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iTempItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_TEMP_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iTempItemUID )
		;
}
//}}

/////////////////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_UNIT_INFO_NOT, obj, ks )
{
	return PUT( m_kUnitInfo )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_mapItemObtained )
		&& PUT( m_bGameEnd )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_UNIT_INFO_NOT, obj, ks )
{
	return GET( m_kUnitInfo )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_mapItemObtained )
		&& GET( m_bGameEnd )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REFRESH_ROOM_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REFRESH_ROOM_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NOTIFY_MSG_REQ, obj, ks )
{
	return PUT( m_cNotifyType )
		&& PUT( m_Count )
		&& PUT( m_wstrMSG )
		//{{ 아이템 획득 공지에 큐브 이름 출력 - 김민성
		&& PUT( m_iRandomItem )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_NOTIFY_MSG_REQ, obj, ks )
{
	return GET( m_cNotifyType )
		&& GET( m_Count )
		&& GET( m_wstrMSG )
		//{{ 아이템 획득 공지에 큐브 이름 출력 - 김민성
		&& GET( m_iRandomItem )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_SERVER_STATUS_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iCCU )
		&& PUT( m_mapNumPvPRoom )
		&& PUT( m_iCurPartyCount )
		&& PUT( m_iPlayGamePartyCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_SERVER_STATUS_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iCCU )
		&& GET( m_mapNumPvPRoom )
		&& GET( m_iCurPartyCount )
		&& GET( m_iPlayGamePartyCount )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CLIENT_QUIT_REQ, obj, ks )
{
    return PUT( m_iReason )
		&& PUT( m_ErrorMsgInfoList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CLIENT_QUIT_REQ, obj, ks )
{
    return GET( m_iReason )
		&& GET( m_ErrorMsgInfoList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_MODIFY_UNIT_LEVEL_REQ, obj, ks )
{
    return PUT( m_wstrUnitNickName )
		&& PUT( m_iLevel )
		&& PUT( m_iEXP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_MODIFY_UNIT_LEVEL_REQ, obj, ks )
{
    return GET( m_wstrUnitNickName )
		&& GET( m_iLevel )
		&& GET( m_iEXP )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_MODIFY_UNIT_LEVEL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_MODIFY_UNIT_LEVEL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kUnitInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_OPTION_PLAY_GUIDE_REQ, obj, ks )
{
    return PUT( m_bOn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_OPTION_PLAY_GUIDE_REQ, obj, ks )
{
    return GET( m_bOn )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BUY_ED_ITEM_REQ, obj, ks )
{
    return PUT( m_bDebug )
		&& PUT( m_sPeriod )
		&& PUT( m_mapItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_ED_ITEM_REQ, obj, ks )
{
    return GET( m_bDebug )
		&& GET( m_sPeriod )
		&& GET( m_mapItem )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BUY_ED_ITEM_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iED )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& PUT( m_iAPoint )
		//}}
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_ED_ITEM_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_iED )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& GET( m_iAPoint )
		//}}
		&& GET( m_vecInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELL_ED_ITEM_REQ, obj, ks )
{
    return PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELL_ED_ITEM_REQ, obj, ks )
{
    return GET( m_iItemUID )
		&& GET( m_iQuantity )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SELL_ED_ITEM_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_kInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SELL_ED_ITEM_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_kInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NEW_QUEST_REQ, obj, ks )
{
	return PUT( m_iQuestID )
		&& PUT( m_iTalkNPCID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_QUEST_REQ, obj, ks )
{
	return GET( m_iQuestID )
		&& GET( m_iTalkNPCID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NEW_QUEST_NOT, obj, ks )
{
	return PUT( m_QuestInst )
		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		&& PUT( m_vecUpdatedInventorySlot )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_QUEST_NOT, obj, ks )
{
	return GET( m_QuestInst )
		//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
		&& GET( m_vecUpdatedInventorySlot )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_QUEST_REQ, obj, ks )
{
	return PUT( m_QuestInst )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_QUEST_REQ, obj, ks )
{
	return GET( m_QuestInst )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_QUEST_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_QUEST_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_QUEST_NOT, obj, ks )
{
	return PUT( m_vecQuestInst )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_QUEST_NOT, obj, ks )
{
	return GET( m_vecQuestInst )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_QUEST_COMPLETE_REQ, obj, ks )
{
	return PUT( m_iQuestID )
		&& PUT( m_iTalkNPCID )
		&& PUT( m_mapSelectItemID )
		//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
		&& PUT( m_bAutoComplete )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUEST_COMPLETE_REQ, obj, ks )
{
	return GET( m_iQuestID )
		&& GET( m_iTalkNPCID )
		&& GET( m_mapSelectItemID )
		//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
		&& GET( m_bAutoComplete )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_QUEST_COMPLETE_ACK, obj, ks )
{
	return PUT( m_iOK )
		//{{ 2010. 04. 05  최육사	일일퀘스트
		&& PUT( m_kCompleteQuestInfo )
		//}}
		&& PUT( m_kUpdateUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUEST_COMPLETE_ACK, obj, ks )
{
	return GET( m_iOK )
		//{{ 2010. 04. 05  최육사	일일퀘스트
		&& GET( m_kCompleteQuestInfo )
		//}}
		&& GET( m_kUpdateUnitInfo )
		;
}


//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
SERIALIZE_DEFINE_PUT( KEGS_ALL_COMPLETED_QUEST_COMPLETE_REQ, obj, ks )
{
	return PUT( m_vecQuestCompleteReq )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ALL_COMPLETED_QUEST_COMPLETE_REQ, obj, ks )
{
	return GET( m_vecQuestCompleteReq )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecQuestCompleteAck )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ALL_COMPLETED_QUEST_COMPLETE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecQuestCompleteAck )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GIVE_UP_QUEST_REQ, obj, ks )
{
	return PUT( m_iQuestID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GIVE_UP_QUEST_REQ, obj, ks )
{
	return GET( m_iQuestID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GATHER_GIVE_UP_QUEST_REQ, obj, ks )
{
	return PUT( m_vecGatherQuestID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GATHER_GIVE_UP_QUEST_REQ, obj, ks )
{
	return GET( m_vecGatherQuestID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_TALK_WITH_NPC_REQ, obj, ks )
{
	return PUT( m_iNPCID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TALK_WITH_NPC_REQ, obj, ks )
{
	return GET( m_iNPCID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, obj, ks )
{
	return PUT( m_cFromSlotType )
		&& PUT( m_iFromSlotID )
		&& PUT( m_cToSlotType )
		&& PUT( m_iToSlotID )
		&& PUT( m_iShareUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, obj, ks )
{
	return GET( m_cFromSlotType )
		&& GET( m_iFromSlotID )
		&& GET( m_cToSlotType )
		&& GET( m_iToSlotID )
		&& GET( m_iShareUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_INVENTORY_SLOT_ITEM_ACK, obj, ks )
{
	return PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_INVENTORY_SLOT_ITEM_ACK, obj, ks )
{
	return GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT, obj, ks )
{
    return PUT( m_UnitUID )
		&& PUT( m_vecInventorySlotInfo )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& PUT( m_iEquippedTitleID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_EQUIPPED_ITEM_IN_SQUARE_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_vecInventorySlotInfo )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& GET( m_iEquippedTitleID )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT, obj, ks )
{
	//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
	return PUT( m_cChangeEquippedPacketType )
		&& PUT( m_UnitUID )
		//}}
		&& PUT( m_vecInventorySlotInfo )
		&& PUT( m_kGameStat )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& PUT( m_iEquippedTitleID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT, obj, ks )
{
	//{{ 2010. 12. 23	최육사	장비 변경 패킷 타입
	return GET( m_cChangeEquippedPacketType )
		&& GET( m_UnitUID )
		//}}
		&& GET( m_vecInventorySlotInfo )
		&& GET( m_kGameStat )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& GET( m_iEquippedTitleID )
		//}}
		;
}

//{{ 2008. 11. 30  최육사
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_vecInventorySlotInfo )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& PUT( m_iEquippedTitleID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_vecInventorySlotInfo )
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		&& GET( m_iEquippedTitleID )
		//}}
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DELETE_ITEM_REQ, obj, ks )
{
    return PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_ITEM_REQ, obj, ks )
{
    return GET( m_iItemUID )
		&& GET( m_iQuantity )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DELETE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kInventoryItemInfo );
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kInventoryItemInfo );
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_AUTH_LEVEL_REQ, obj, ks )
{
	return PUT( m_wstrUserID )
		&& PUT( m_cAuthLevel )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_AUTH_LEVEL_REQ, obj, ks )
{
	return GET( m_wstrUserID )
		&& GET( m_cAuthLevel )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ, obj, ks )
{
	return PUT( m_cAuthLevel );
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ, obj, ks )
{
	return GET( m_cAuthLevel );
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_AUTH_LEVEL_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecUserIDList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_AUTH_LEVEL_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecUserIDList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHECK_INVALID_ROOM_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iMapID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_INVALID_ROOM_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iMapID )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
SERIALIZE_DEFINE_PUT( KEGS_RESULT_SUCCESS_REQ, obj, ks )
{
	return PUT( m_kGamePlayNetworkInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESULT_SUCCESS_REQ, obj, ks )
{
	return GET( m_kGamePlayNetworkInfo )
		;
}
//}}


//{{ 2009. 4. 28  최육사	던전id
SERIALIZE_DEFINE_PUT( KEGS_RESULT_SUCCESS_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iDungeonID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESULT_SUCCESS_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iDungeonID )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESULT_ALL_UNIT_SUCCESS_NOT, obj, ks )
{
	return PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		//{{ 2012. 06. 07	최육사	배틀필드 시스템
		&& PUT( m_kReturnToFieldInfo )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESULT_ALL_UNIT_SUCCESS_NOT, obj, ks )
{
	return GET( m_RoomInfo )
		&& GET( m_vecSlot )
		//{{ 2012. 06. 07	최육사	배틀필드 시스템
		&& GET( m_kReturnToFieldInfo )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USE_QUICK_SLOT_REQ, obj, ks )
{
	//{{ 2011. 12. 15	최육사	인벤토리 슬롯ID크기 늘리기
	return PUT( m_sSlotID )
	//}}
	;
}

SERIALIZE_DEFINE_GET( KEGS_USE_QUICK_SLOT_REQ, obj, ks )
{
	//{{ 2011. 12. 15	최육사	인벤토리 슬롯ID크기 늘리기
	return GET( m_sSlotID )
	//}}
	;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USE_QUICK_SLOT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_InventorySlotInfo )
		&& PUT( m_ItemID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_QUICK_SLOT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_InventorySlotInfo )
		&& GET( m_ItemID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USE_QUICK_SLOT_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_ItemID )
		&& PUT( m_bUseItemNeedPayment )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_QUICK_SLOT_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_ItemID )
		&& GET( m_bUseItemNeedPayment )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_ED_REQ, obj, ks )
{
	return PUT( m_wstrUnitNickName )
		&& PUT( m_nED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_ED_REQ, obj, ks )
{
	return GET( m_wstrUnitNickName )
		&& GET( m_nED )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SEARCH_UNIT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kCUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEARCH_UNIT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kCUnitInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_WATCH_UNIT_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WATCH_UNIT_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SQUARE_LIST_REQ, obj, ks )
{
	return PUT( m_nViewPage )
		&& PUT( m_cSquareType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SQUARE_LIST_REQ, obj, ks )
{
	return GET( m_nViewPage )
		&& GET( m_cSquareType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SQUARE_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_nTotalPage )
		&& PUT( m_nViewPage )
		&& PUT( m_vecSquareInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SQUARE_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_nTotalPage )
		&& GET( m_nViewPage )
		&& GET( m_vecSquareInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SQUARE_UNIT_SYNC_DATA_REQ, obj, ks )
{
	return PUT( m_StateID )
		&& PUT( m_StateChangeNum )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_bIsRight )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SQUARE_UNIT_SYNC_DATA_REQ, obj, ks )
{
	return GET( m_StateID )
		&& GET( m_StateChangeNum )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_bIsRight )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SQUARE_UNIT_SYNC_DATA_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_StateID )
		&& PUT( m_StateChangeNum )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_bIsRight )
		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		&& PUT( m_iED )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_SQUARE_UNIT_SYNC_DATA_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_StateID )
		&& GET( m_StateChangeNum )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_bIsRight )
		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		&& GET( m_iED )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_SQUARE_REQ, obj, ks )
{
	return PUT( m_iSquareUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_SQUARE_REQ, obj, ks )
{
	return GET( m_iSquareUID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_SQUARE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kSquareInfo )
		&& PUT( m_vecUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_SQUARE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kSquareInfo )
		&& GET( m_vecUserInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_SQUARE_NOT, obj, ks )
{
	return PUT( m_kUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_SQUARE_NOT, obj, ks )
{
	return GET( m_kUserInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_SKILL_REQ, obj, ks )
{
	return
		PUT( m_vecNowLearnSkill )
		&&
		PUT( m_iActiveSkillPageNumber)	/// 활성화 중인 페이지
		&&
		PUT( m_mapSkillList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_SKILL_REQ, obj, ks )
{
	return
		GET( m_vecNowLearnSkill )
		&&
		GET( m_iActiveSkillPageNumber)	/// 활성화 중인 페이지
		&&
		GET( m_mapSkillList )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_mapSkillList )
		&& PUT( m_iRemainSP )
		&& PUT( m_iRemainCSP )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_mapSkillList )
		&& GET( m_iRemainSP )
		&& GET( m_iRemainCSP )
		;
}
//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SKILL_SLOT_REQ, obj, ks )
{
	return PUT( m_iSlotID )
		&& PUT( m_iSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SKILL_SLOT_REQ, obj, ks )
{
	return GET( m_iSlotID )
		&& GET( m_iSkillID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SKILL_SLOT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iSlotID )
		&& PUT( m_iSkillID )
		//{{ 2012. 12. 3	박세훈	스킬 슬롯 체인지 패킷 통합
		&& PUT( m_iSlotID2 )
		&& PUT( m_iSkillID2 )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SKILL_SLOT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iSlotID )
		&& GET( m_iSkillID )
		//{{ 2012. 12. 3	박세훈	스킬 슬롯 체인지 패킷 통합
		&& GET( m_iSlotID2 )
		&& GET( m_iSkillID2 )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SKILL_SLOT_NOT, obj, ks )
{
	return PUT( m_iSlotID )
		&& PUT( m_iSkillID )
		//{{ 2012. 12. 3	박세훈	스킬 슬롯 체인지 패킷 통합
		&& PUT( m_iSlotID2 )
		&& PUT( m_iSkillID2 )
		//}}
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SKILL_SLOT_NOT, obj, ks )
{
	return GET( m_iSlotID )
		&& GET( m_iSkillID )
		//{{ 2012. 12. 3	박세훈	스킬 슬롯 체인지 패킷 통합
		&& GET( m_iSlotID2 )
		&& GET( m_iSkillID2 )
		//}}
		&& GET( m_iUnitUID )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_SKILL_POINT_REQ, obj, ks )
{
	return PUT( m_iSPoint )
		&& PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_SKILL_POINT_REQ, obj, ks )
{
	return GET( m_iSPoint )
		&& GET( m_iActiveSkillPageNumber )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_SKILL_POINT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iSPoint )
		&& PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_SKILL_POINT_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_iSPoint )
		&& GET( m_iActiveSkillPageNumber )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ADMIN_INIT_SKILL_TREE_REQ, obj, ks )
{
	return PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_INIT_SKILL_TREE_REQ, obj, ks )
{
	return GET( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_INIT_SKILL_TREE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_iSPoint )
		&& PUT( m_iCSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_INIT_SKILL_TREE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_iSPoint )
		&& GET( m_iCSPoint )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_KICK_USER_REQ, obj, ks )
{
    return PUT( m_wstrUnitNickName )
		&& PUT( m_bIsUserID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_KICK_USER_REQ, obj, ks )
{
    return GET( m_wstrUnitNickName )
		&& GET( m_bIsUserID )
		;
}


//{{ 2011. 12. 22	최육사	배틀필드 시스템
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_PARTY_REQ, obj, ks )
{
	return PUT( m_iReceiverUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_iServerGroupID )
		&& PUT( m_kCreatePartyInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PARTY_REQ, obj, ks )
{
	return GET( m_iReceiverUID )
		&& GET( m_wstrNickName )
		&& GET( m_iServerGroupID )
		&& GET( m_kCreatePartyInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_PARTY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iReceiverUID )
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		//}
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PARTY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iReceiverUID )
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		//}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_INVITE_PARTY_DENY_NOT, obj, ks )
{
	return PUT( m_iDenyReasonNetError )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PARTY_DENY_NOT, obj, ks )
{
	return GET( m_iDenyReasonNetError )
		&& GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_INVITE_PARTY_NOT, obj, ks )
{
	return PUT( m_iPartyUID )
		&& PUT( m_iHostUnitUID )
		&& PUT( m_wstrHostNickName )
		//{{ 2011. 07. 08	최육사	대전 개편
		&& PUT( m_cPartyType )
		//}}
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		//}
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PARTY_NOT, obj, ks )
{
	return GET( m_iPartyUID )
		&& GET( m_iHostUnitUID )
		&& GET( m_wstrHostNickName )
		//{{ 2011. 07. 08	최육사	대전 개편
		&& GET( m_cPartyType )
		//}}
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		//}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_INVITE_PARTY_REPLY_NOT, obj, ks )
{
	return PUT( m_iPartyUID )
		&& PUT( m_iHostUnitUID )
		&& PUT( m_cReplyReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_PARTY_REPLY_NOT, obj, ks )
{
	return GET( m_iPartyUID )
		&& GET( m_iHostUnitUID )
		&& GET( m_cReplyReason )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_JOIN_PARTY_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecPartyInfo )
		&& PUT( m_vecPartyUserInfo )
		//{{ 2012. 02. 22	최육사	배틀필드 시스템
		&& PUT( m_vecCurPartyUnitUIDList )
		//}}
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		//}
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_PARTY_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecPartyInfo )
		&& GET( m_vecPartyUserInfo )
		//{{ 2012. 02. 22	최육사	배틀필드 시스템
		&& GET( m_vecCurPartyUnitUIDList )
		//}}
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		//}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CLOSE_PARTY_NOT, obj, ks )
{
	return PUT( m_iPartyUID )
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CLOSE_PARTY_NOT, obj, ks )
{
	return GET( m_iPartyUID )
		&& GET( m_iReason )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_LEAVE_PARTY_REQ, obj, ks )
{
	return PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_PARTY_REQ, obj, ks )
{
	return GET( m_iReason )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_LEAVE_PARTY_NOT, obj, ks )
{
	return PUT( m_iLeaveUnitUID )
		&& PUT( m_iNewHostUID )
		//{{ 2012. 02. 21	최육사	배틀필드 시스템
		&& PUT( m_iPartyUID )
		&& PUT( m_vecPartyUnitUIDList )
		//}}
		//{{ 버프 관리 시스템 - 김민성
		&& PUT( m_bExistComeBackUser )
		//}
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_PARTY_NOT, obj, ks )
{
	return GET( m_iLeaveUnitUID )
		&& GET( m_iNewHostUID )
		//{{ 2012. 02. 21	최육사	배틀필드 시스템
		&& GET( m_iPartyUID )
		&& GET( m_vecPartyUnitUIDList )
		//}}
		//{{ 버프 관리 시스템 - 김민성
		&& GET( m_bExistComeBackUser )
		//}
		&& GET( m_iReason )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_UPDATE_PARTY_USER_INFO_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cState )
		&& PUT( m_iStateCode )
		&& PUT( m_cUnitClass )
		&& PUT( m_ucLevel )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_PARTY_USER_INFO_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cState )
		&& GET( m_iStateCode )
		&& GET( m_cUnitClass )
		&& GET( m_ucLevel )
        ;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 3. 31  최육사	파티유저
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_PARTY_UID_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPartyUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_PARTY_UID_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPartyUID )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PARTY_CHANGE_READY_REQ, obj, ks )
{
	return PUT( m_bReady )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_CHANGE_READY_REQ, obj, ks )
{
	return GET( m_bReady )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PARTY_CHANGE_READY_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_bReady )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_CHANGE_READY_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_bReady )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PARTY_CHANGE_DUNGEON_REQ, obj, ks )
{
	return PUT( m_iDungeonID )
		&& PUT( m_DifficultyLevel )
		&& PUT( m_cDungeonMode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_CHANGE_DUNGEON_REQ, obj, ks )
{
	return GET( m_iDungeonID )
		&& GET( m_DifficultyLevel )
		&& GET( m_cDungeonMode )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PARTY_GAME_START_REQ, obj, ks )
{
	return PUT( m_sWorldID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_GAME_START_REQ, obj, ks )
{
	return GET( m_sWorldID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PARTY_GAME_START_NOT, obj, ks )
{
	return PUT( m_sWorldID )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		&& PUT( m_wstrCNIP )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_GAME_START_NOT, obj, ks )
{
	return GET( m_sWorldID )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		&& GET( m_wstrCNIP )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PARTY_GAME_START_OPINION_CHECK_REPLY_NOT, obj, ks )
{
	return PUT( m_bAccept )
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_GAME_START_OPINION_CHECK_REPLY_NOT, obj, ks )
{
	return GET( m_bAccept )
		&& GET( m_iReason )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PARTY_GAME_START_FAILED_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_iFailUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_GAME_START_FAILED_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_iFailUnitUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PARTY_CHANGE_HOST_NOT, obj, ks )
{
	return PUT( m_iBeforeUID )
		&& PUT( m_iAfterUID )
		//{{ 2012. 11. 13	최육사	배틀필드 시스템
		&& PUT( m_bRegroupParty )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_CHANGE_HOST_NOT, obj, ks )
{
	return GET( m_iBeforeUID )
		&& GET( m_iAfterUID )
		//{{ 2012. 11. 13	최육사	배틀필드 시스템
		&& GET( m_bRegroupParty )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iDungeonID )
		&& PUT( m_iNumResurrectionStone )
		//{{ 2009. 10. 14  최육사	자동결제 부활석
		&& PUT( m_iNumAutoPaymentResStone )
		//}}
		&& PUT( m_bUseCash )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iDungeonID )
		&& GET( m_iNumResurrectionStone )
		//{{ 2009. 10. 14  최육사	자동결제 부활석
		&& GET( m_iNumAutoPaymentResStone )
		//}}
		&& GET( m_bUseCash )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT, obj, ks )
{
	return PUT( m_iUsedByUnitUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_iStartPosIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT, obj, ks )
{
	return GET( m_iUsedByUnitUID )
		&& GET( m_iUnitUID )
		&& GET( m_iStartPosIndex )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_BLACKLIST_USER_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_BLACKLIST_USER_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_BLACKLIST_USER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kChatBlackListUnit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_BLACKLIST_USER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kChatBlackListUnit )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DEL_BLACKLIST_USER_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DEL_BLACKLIST_USER_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_COMMUNITY_OPTION_REQ, obj, ks )
{
	return PUT( m_kDenyOptions )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_COMMUNITY_OPTION_REQ, obj, ks )
{
	return GET( m_kDenyOptions )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_COMMUNITY_OPTION_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kDenyOptions )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_COMMUNITY_OPTION_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kDenyOptions )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHECK_SUM_REQ, obj, ks )
{
	return PUT( m_mapCheckSum )
		&& PUT( m_ProtectTableCSVal )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_SUM_REQ, obj, ks )
{
	return GET( m_mapCheckSum )
		&& GET( m_ProtectTableCSVal )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHECK_SUM_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_SUM_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KNM_REQUEST_NEW_FRIEND_INFO_REQ, obj, ks )
{
	return PUT( m_wstrUnitNickName )
		//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
		&& PUT( m_iUnitUID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_KNM_REQUEST_NEW_FRIEND_INFO_REQ, obj, ks )
{
	return GET( m_wstrUnitNickName )
		//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
		&& GET( m_iUnitUID )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KNM_REQUEST_NEW_FRIEND_INFO_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiKNMSerialNum )
		&& PUT( m_wstrUnitNickName )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_KNM_REQUEST_NEW_FRIEND_INFO_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiKNMSerialNum )
		&& GET( m_wstrUnitNickName )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TRADE_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TRADE_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TRADE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TRADE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TRADE_NOT, obj, ks )
{
	return PUT( m_iSenderUID )
		&& PUT( m_wstrSenderNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TRADE_NOT, obj, ks )
{
	return GET( m_iSenderUID )
		&& GET( m_wstrSenderNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TRADE_REPLY_NOT, obj, ks )
{
	return PUT( m_iReceiverUID )
		&& PUT( m_bAccept )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TRADE_REPLY_NOT, obj, ks )
{
	return GET( m_iReceiverUID )
		&& GET( m_bAccept )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TRADE_DENY_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TRADE_DENY_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_TRADE_NOT, obj, ks )
{
	return PUT( m_iTradeUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_TRADE_NOT, obj, ks )
{
	return GET( m_iTradeUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_PERSONAL_SHOP_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPersonalShopUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_PERSONAL_SHOP_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPersonalShopUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_TRADE_USER_INFO_NOT, obj, ks )
{
	return PUT( m_vecTradeUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TRADE_USER_INFO_NOT, obj, ks )
{
	return GET( m_vecTradeUserInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_JOIN_TRADE_NOT, obj, ks )
{
	return PUT( m_kTradeUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_TRADE_NOT, obj, ks )
{
	return GET( m_kTradeUserInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_TRADE_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		&& PUT( m_bDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_TRADE_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iQuantity )
		&& GET( m_bDelete )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_TRADE_ITEM_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iItemUID )
		&& PUT( m_bDelete )
		&& PUT( m_kItemInfo )
		&& PUT( m_mapOfferState )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_TRADE_ITEM_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iItemUID )
		&& GET( m_bDelete )
		&& GET( m_kItemInfo )
		&& GET( m_mapOfferState )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_TRADE_ED_REQ, obj, ks )
{
	return PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_TRADE_ED_REQ, obj, ks )
{
	return GET( m_iED )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_TRADE_ED_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iED )
		&& PUT( m_mapOfferState )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_TRADE_ED_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iED )
		&& GET( m_mapOfferState )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_TRADE_COMPLETE_NOT, obj, ks )
{
	return PUT( m_vecInventorySlotInfo )
		&& PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TRADE_COMPLETE_NOT, obj, ks )
{
	return GET( m_vecInventorySlotInfo )
		&& GET( m_iED )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REPAIR_ITEM_REQ, obj, ks )
{
	return PUT( m_vecItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REPAIR_ITEM_REQ, obj, ks )
{
	return GET( m_vecItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REPAIR_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& PUT( m_iAPoint )
		//}}
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REPAIR_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		//{{ 2011. 07. 11	최육사	대전 개편
		&& GET( m_iAPoint )
		//}}
		&& GET( m_vecInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_TC_GAME_REQ, obj, ks )
{
	return PUT( m_bIsSuccess )
		;
}

SERIALIZE_DEFINE_GET( KEGS_END_TC_GAME_REQ, obj, ks )
{
	return GET( m_bIsSuccess )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_END_TC_GAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bIsSuccess )
		&& PUT( m_kUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_END_TC_GAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bIsSuccess )
		&& GET( m_kUnitInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SET_TC_REMAINING_TIME_REQ, obj, ks )
{
	return PUT( m_bEnable )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SET_TC_REMAINING_TIME_REQ, obj, ks )
{
	return GET( m_bEnable )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2007. 10. 4  최육사  랜덤 아이템 패킷
SERIALIZE_DEFINE_PUT( KEGS_OPEN_RANDOM_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
		&& PUT( m_bDelayedOK )
		//}}
		&& PUT( m_bNotifyGetItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_OPEN_RANDOM_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
		&& GET( m_bDelayedOK )
		//}}
		&& GET( m_bNotifyGetItem )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_OPEN_RANDOM_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRessurectionCount )
		&& PUT( m_iRestoreSpirit )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_vecKInventorySlotInfo )
		//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
		&& PUT( m_iED )
		//}
		;
}

SERIALIZE_DEFINE_GET( KEGS_OPEN_RANDOM_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRessurectionCount )
		&& GET( m_iRestoreSpirit )
		&& GET( m_mapInsertedItem )
		&& GET( m_vecKInventorySlotInfo )
		//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
		&& GET( m_iED )
		//}
		;
}
//}}

//{{ 2007. 10. 10  최육사  제조
SERIALIZE_DEFINE_PUT( KEGS_ITEM_MANUFACTURE_REQ, obj, ks )
{
	return PUT( m_iManufactureID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_MANUFACTURE_REQ, obj, ks )
{
	return GET( m_iManufactureID )
		&& GET( m_iQuantity )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ITEM_MANUFACTURE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_iED )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_MANUFACTURE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_mapInsertedItem )
		&& GET( m_iED )
		&& GET( m_vecKInventorySlotInfo )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KNPROTECT_AUTH_CHECK_NOT, obj, ks )
{
	return PUT( dwIndex )
		&& PUT( dwValue1 )
		&& PUT( dwValue2 )
		&& PUT( dwValue3 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_KNPROTECT_AUTH_CHECK_NOT, obj, ks )
{
	return GET( dwIndex )
		&& GET( dwValue1 )
		&& GET( dwValue2 )
		&& GET( dwValue3 )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADD_ON_STAT_REQ, obj, ks )
{
	return PUT( m_UID )
		&& PUT( m_bIsUnit )
		&& PUT( m_cStatType )
		&& PUT( m_fStatVal )
		&& PUT( m_fTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADD_ON_STAT_REQ, obj, ks )
{
	return GET( m_UID )
		&& GET( m_bIsUnit )
		&& GET( m_cStatType )
		&& GET( m_fStatVal )
		&& GET( m_fTime )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADD_ON_STAT_NOT, obj, ks )
{
	return PUT( m_UID )
		&& PUT( m_bIsUnit )
		&& PUT( m_cStatType )
		&& PUT( m_fStatVal )
		&& PUT( m_fTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADD_ON_STAT_NOT, obj, ks )
{
	return GET( m_UID )
		&& GET( m_bIsUnit )
		&& GET( m_cStatType )
		&& GET( m_fStatVal )
		&& GET( m_fTime )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_APPLY_COUPON_REQ, obj, ks )
{
    return PUT( m_wstrCouponSerial )
		&& PUT( m_iCouponType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_APPLY_COUPON_REQ, obj, ks )
{
    return GET( m_wstrCouponSerial )
		&& GET( m_iCouponType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PURCHASED_CASH_ITEM_LIST_REQ, obj, ks )
{
    return PUT( m_nItemPerPage )
		&& PUT( m_iCurrentPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PURCHASED_CASH_ITEM_LIST_REQ, obj, ks )
{
    return GET( m_nItemPerPage )
		&& GET( m_iCurrentPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PURCHASED_CASH_ITEM_LIST_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_vecOrderInfo )
		&& PUT( m_vecPackageInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PURCHASED_CASH_ITEM_LIST_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_vecOrderInfo )
		&& GET( m_vecPackageInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_PURCHASED_CASH_ITEM_REQ, obj, ks )
{
    return PUT( m_ulOrderNo )
		&& PUT( m_ulProductNo )
		&& PUT( m_wstrProductID )
		&& PUT( m_usProductExpire )
		&& PUT( m_usProductPieces )
		&& PUT( m_byteProductKind )
		&& PUT( m_iQuantity )
		&& PUT( m_mapSocketForCash )
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		&& PUT( m_mapSocketGroupIDForCash )
		//}
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_PURCHASED_CASH_ITEM_REQ, obj, ks )
{
    return GET( m_ulOrderNo )
		&& GET( m_ulProductNo )
		&& GET( m_wstrProductID )
		&& GET( m_usProductExpire )
		&& GET( m_usProductPieces )
		&& GET( m_byteProductKind )
		&& GET( m_iQuantity )
		&& GET( m_mapSocketForCash )
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		&& GET( m_mapSocketGroupIDForCash )
		//}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_PURCHASED_CASH_ITEM_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iNumResurrectionStone )
		&& PUT( m_mapExpandedCategorySlot )
		&& PUT( m_vecInventorySlotInfo )
		//{{ 2012. 02. 22	박세훈	길드 이름 변경권
		&& PUT( m_iExceptionProcessItemID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_PURCHASED_CASH_ITEM_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_iNumResurrectionStone )
		&& GET( m_mapExpandedCategorySlot )
		&& GET( m_vecInventorySlotInfo )
		//{{ 2012. 02. 22	박세훈	길드 이름 변경권
		&& GET( m_iExceptionProcessItemID )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ, obj, ks )
{
	return PUT( m_ulOrderNo )
		&& PUT( m_ulPackageProductNo )
		&& PUT( m_wstrPackageProductID )
		&& PUT( m_byteProductKind )
		&& PUT( m_vecPickUpPackageInfo )
		&& PUT( m_mapSocketForCash )
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		&& PUT( m_mapSocketGroupIDForCash )
		//}
		&& PUT( m_iProductID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ, obj, ks )
{
	return GET( m_ulOrderNo )
		&& GET( m_ulPackageProductNo )
		&& GET( m_wstrPackageProductID )
		&& GET( m_byteProductKind )
		&& GET( m_vecPickUpPackageInfo )
		&& GET( m_mapSocketForCash )
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		&& GET( m_mapSocketGroupIDForCash )
		//}
		&& GET( m_iProductID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KNM_INVITE_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_kUnitInfo )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_KNM_INVITE_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_kUnitInfo )
        ;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KNM_TOGETHER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_KNM_TOGETHER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kUnitInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_FRAME_AVERAGE_REQ, obj, ks )
{
	return PUT( m_cGameType )
		&& PUT( m_iTotalFrame )
		&& PUT( m_iFrameAverage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FRAME_AVERAGE_REQ, obj, ks )
{
	return GET( m_cGameType )
		&& GET( m_iTotalFrame )
		&& GET( m_iFrameAverage )
		;
}

SERIALIZE_DEFINE_PUT( KENX_USER_LOGIN_NOT, obj, ks )
{
	return PUT( m_bIsGameBang )
		&& PUT( m_kPcBangReward )
		;
}

SERIALIZE_DEFINE_GET( KENX_USER_LOGIN_NOT, obj, ks )
{
	return GET( m_bIsGameBang )
		&& GET( m_kPcBangReward )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ, obj, ks )
{
    return PUT( m_bUseCash )
		&& PUT( m_iUsedByUnitUID )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ, obj, ks )
{
	return GET( m_bUseCash )
		&& GET( m_iUsedByUnitUID )
		&& GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_NICK_NAME_REQ, obj, ks )
{
    return PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_bCheckOnly )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_NICK_NAME_REQ, obj, ks )
{
    return GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		&& GET( m_bCheckOnly )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_NICK_NAME_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_bCheckOnly )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_NICK_NAME_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_iUnitUID )
        && GET( m_wstrNickName )
		&& GET( m_bCheckOnly )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_SPIRIT_REQ, obj, ks )
{
	return PUT( m_iSpirit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_SPIRIT_REQ, obj, ks )
{
	return GET( m_iSpirit )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_SPIRIT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iChangedSpirit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_SPIRIT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iChangedSpirit )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_SET_PC_BANG_REQ, obj, ks )
{
    return PUT( m_bOn )
		&& PUT( m_iPcBangType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_SET_PC_BANG_REQ, obj, ks )
{
    return GET( m_bOn )
		&& GET( m_iPcBangType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_SET_PC_BANG_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_bOn )
		&& PUT( m_iPcBangType )
		&& PUT( m_kPcBangReward )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_SET_PC_BANG_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_bOn )
		&& GET( m_iPcBangType )
		&& GET( m_kPcBangReward )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_VSPOINT_REQ, obj, ks )
{
	//{{ 2011. 07. 11	최육사	대전 개편
	return PUT( m_iRating )
		&& PUT( m_iRPoint )
		&& PUT( m_iAPoint )
		;
	//}}
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_VSPOINT_REQ, obj, ks )
{
	//{{ 2011. 07. 11	최육사	대전 개편
	return GET( m_iRating )
		&& GET( m_iRPoint )
		&& GET( m_iAPoint )
		;
	//}}
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_VSPOINT_ACK, obj, ks )
{
	return PUT( m_iOK )
		//{{ 2011. 07. 11	최육사	대전 개편
		//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
		&& PUT( m_cRank )
		//}}
		&& PUT( m_iChangedRPoint )
		&& PUT( m_iChangedAPoint )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_VSPOINT_ACK, obj, ks )
{
	return GET( m_iOK )
		//{{ 2011. 07. 11	최육사	대전 개편
		//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
		&& GET( m_cRank )
		//}}
		&& GET( m_iChangedRPoint )
		&& GET( m_iChangedAPoint )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHECK_BALANCE_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_ulBalance )
		&& PUT( m_bOnlyType )
		&& PUT( m_iCashType )
		&& PUT( m_GlobalCashInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_BALANCE_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_ulBalance )
		&& GET( m_bOnlyType )
		&& GET( m_iCashType )
		&& GET( m_GlobalCashInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REG_PERSONAL_SHOP_ITEM_REQ, obj, ks )
{
	return PUT( m_wstrPersonalShopName )
		&& PUT( m_vecRegItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_PERSONAL_SHOP_ITEM_REQ, obj, ks )
{
	return GET( m_wstrPersonalShopName )
		&& GET( m_vecRegItem )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REG_PERSONAL_SHOP_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_wstrPersonalShopName )
		&& PUT( m_vecSellItemInfo )
		//{{ 2011. 04. 18	최육사	대리 상인
		&& PUT( m_vecInventorySlotInfo )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_PERSONAL_SHOP_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_wstrPersonalShopName )
		&& GET( m_vecSellItemInfo )
		//{{ 2011. 04. 18	최육사	대리 상인
		&& GET( m_vecInventorySlotInfo )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BUY_PERSONAL_SHOP_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iItemID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_PERSONAL_SHOP_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iItemID )
		&& GET( m_iQuantity )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BUY_PERSONAL_SHOP_ITEM_COMPLETE_NOT, obj, ks )
{
	return PUT( m_vecInventorySlotInfo )
		&& PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_PERSONAL_SHOP_ITEM_COMPLETE_NOT, obj, ks )
{
	return GET( m_vecInventorySlotInfo )
		&& GET( m_iED )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BREAK_PERSONAL_SHOP_REQ, obj, ks )
{
	return PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BREAK_PERSONAL_SHOP_REQ, obj, ks )
{
	return GET( m_iReason )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BREAK_PERSONAL_SHOP_NOT, obj, ks )
{
	return PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BREAK_PERSONAL_SHOP_NOT, obj, ks )
{
	return GET( m_iReason )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_PERSONAL_SHOP_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cStageKey )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_PERSONAL_SHOP_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cStageKey )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_JOIN_PERSONAL_SHOP_ACK, obj, ks )
{
	return PUT( m_iOK )
		//{{ 2011. 07. 19	최육사	대리상점 구매 수수료 없앤 이벤트
		//}}
		&& PUT( m_wstrPersonalShopName )
		&& PUT( m_wstrNickName )
		&& PUT( m_vecSellItemInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_PERSONAL_SHOP_ACK, obj, ks )
{
	return GET( m_iOK )
		//{{ 2011. 07. 19	최육사	대리상점 구매 수수료 없앤 이벤트
		//}}
		&& GET( m_wstrPersonalShopName )
		&& GET( m_wstrNickName )
		&& GET( m_vecSellItemInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_SQUARE_PERSONAL_SHOP_INFO_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cPersonalShopState )
		&& PUT( m_cPersonalShopType )
		&& PUT( m_wstrPersonalShopName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_SQUARE_PERSONAL_SHOP_INFO_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cPersonalShopState )
		&& GET( m_cPersonalShopType )
		&& GET( m_wstrPersonalShopName )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_UNIT_CLASS_REQ, obj, ks )
{
	return PUT( m_cUnitClass )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_UNIT_CLASS_REQ, obj, ks )
{
	return GET( m_cUnitClass )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_UNIT_CLASS_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_cUnitClass )
		&& PUT( m_iSPoint )
		&& PUT( m_iCSPoint )
		&& PUT( m_iDefaultSkill1 )
		&& PUT( m_iDefaultSkill2 )
		&& PUT( m_iDefaultSkill3 )
		&& PUT( m_iDefaultSkill4 )
		&& PUT( m_iDefaultSkill5 )
		&& PUT( m_iDefaultSkill6 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_UNIT_CLASS_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_cUnitClass )
		&& GET( m_iSPoint )
		&& GET( m_iCSPoint )
		&& GET( m_iDefaultSkill1 )
		&& GET( m_iDefaultSkill2 )
		&& GET( m_iDefaultSkill3 )
		&& GET( m_iDefaultSkill4 )
		&& GET( m_iDefaultSkill5 )
		&& GET( m_iDefaultSkill6 )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CHANGE_TRADE_NOT, obj, ks )
{
    return PUT( m_iUnitUID )
		&& PUT( m_bTrade )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_TRADE_NOT, obj, ks )
{
    return GET( m_iUnitUID )
		&& GET( m_bTrade )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESOLVE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bJackpot )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_vecKInventorySlotInfo )
		//{{ 2011.03.16   임규수 아바타 분해 시스템
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESOLVE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bJackpot )
		&& GET( m_mapInsertedItem )
		&& GET( m_vecKInventorySlotInfo )
		//{{ 2011.03.16   임규수 아바타 분해 시스템
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ENCHANT_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_bIsRareEnchantStone )
		&& PUT( m_bIsNewEnchant )
		&& PUT( m_bIsSupportMaterial )
		//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
		//}}
		//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
		//}}
		&& PUT( m_bDebug )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENCHANT_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_bIsRareEnchantStone )
		&& GET( m_bIsNewEnchant )
		&& GET( m_bIsSupportMaterial )
		//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
		//}}
		//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
		//}}
		&& GET( m_bDebug )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ENCHANT_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_iEnchantResult )
		&& PUT( m_iEnchantedItemUID )
		&& PUT( m_iEnchantedItemID )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENCHANT_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_iEnchantResult )
		&& GET( m_iEnchantedItemUID )
		&& GET( m_iEnchantedItemID )
		&& GET( m_vecInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2010. 04. 06  최육사	마법석 개편
//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEGS_SOCKET_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_mapSocketInfo )
		&& PUT( m_bCheat )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SOCKET_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_mapSocketInfo )
		&& GET( m_bCheat )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_SOCKET_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_iItemUID )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SOCKET_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_iItemUID )
		&& GET( m_vecInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ITEM_EXPIRATION_NOT, obj, ks )
{
    return PUT( m_vecItemUID )
		;
};

SERIALIZE_DEFINE_GET( KEGS_ITEM_EXPIRATION_NOT, obj, ks )
{
    return GET( m_vecItemUID )
		;
};

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INIT_SKILL_TREE_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INIT_SKILL_TREE_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iActiveSkillPageNumber )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INIT_SKILL_TREE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_iSPoint )
		&& PUT( m_iCSPoint )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INIT_SKILL_TREE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_iSPoint )
		&& GET( m_iCSPoint )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RECOMMEND_USER_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_bCancel )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RECOMMEND_USER_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_bCancel )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RECOMMEND_USER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRewardResurrectionStone )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RECOMMEND_USER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRewardResurrectionStone )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RECOMMEND_USER_RESULT_NOT, obj, ks )
{
	return PUT( m_iRecommendedUnitUID )
		&& PUT( m_iRecommendUnitUID )
		&& PUT( m_wstrFromNickName )
		//{{ 2010. 01. 11  최육사	추천인리스트
		&& PUT( m_ucLevel )
		&& PUT( m_wstrRecommendDate )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_RECOMMEND_USER_RESULT_NOT, obj, ks )
{
	return GET( m_iRecommendedUnitUID )
		&& GET( m_iRecommendUnitUID )
		&& GET( m_wstrFromNickName )
		//{{ 2010. 01. 11  최육사	추천인리스트
		&& GET( m_ucLevel )
		&& GET( m_wstrRecommendDate )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REWARD_TO_RECOMMEND_USER_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_cLevel )
		&& PUT( m_mapRewardItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REWARD_TO_RECOMMEND_USER_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_cLevel )
		&& GET( m_mapRewardItem )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REWARD_TO_TUTORIAL_USER_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_cLevel )
		&& PUT( m_mapRewardItem )
		&& PUT( m_bGraduation )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REWARD_TO_TUTORIAL_USER_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_cLevel )
		&& GET( m_mapRewardItem )
		&& GET( m_bGraduation )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_POST_LETTER_LIST_REQ, obj, ks )
{
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	return PUT( m_cLetterListType )
	//}
		&& PUT( m_iPostPageNo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_POST_LETTER_LIST_REQ, obj, ks )
{
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	return GET( m_cLetterListType )
	//}
		&& GET( m_iPostPageNo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_POST_LETTER_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iTotalLetterCount )
		&& PUT( m_vecPostItemTitle )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_POST_LETTER_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iTotalLetterCount )
		&& GET( m_vecPostItemTitle )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_POST_LETTER_INFO_NOT, obj, ks )
{
	return PUT( m_iNewUserLetterCount )
		&& PUT( m_iNewSystemLetterCount )
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		&& PUT( m_iNewRelationshipLetterCount )
		//}
		&& PUT( m_iTotalLetterCount )
		//{{ 2009. 5. 18  최육사	안읽은편지 제목
		&& PUT( m_vecNewLetterTitle )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_POST_LETTER_INFO_NOT, obj, ks )
{
	return GET( m_iNewUserLetterCount )
		&& GET( m_iNewSystemLetterCount )
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		&& GET( m_iNewRelationshipLetterCount )
		//}
		&& GET( m_iTotalLetterCount )
		//{{ 2009. 5. 18  최육사	안읽은편지 제목
		&& GET( m_vecNewLetterTitle )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SEND_LETTER_REQ, obj, ks )
{
	return PUT( m_bIsGmMessage )
		&& PUT( m_wstrToNickName )
		&& PUT( m_wstrTitle )
		&& PUT( m_wstrMessage )
		&& PUT( m_iED )
		&& PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEND_LETTER_REQ, obj, ks )
{
	return GET( m_bIsGmMessage )
		&& GET( m_wstrToNickName )
		&& GET( m_wstrTitle )
		&& GET( m_wstrMessage )
		&& GET( m_iED )
		&& GET( m_iItemUID )
		&& GET( m_iQuantity )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SEND_LETTER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEND_LETTER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RECEIVE_LETTER_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_cScriptType )
		//{{ 2009. 5. 18  최육사	편지제목
		&& PUT( m_wstrLetterTitle )
		//}}
		&& PUT( m_iScriptIndex )
		//{{ 2011. 04. 05  김민성	우편 및 거래 감시
		&& PUT( m_iFromUnitUID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_RECEIVE_LETTER_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_cScriptType )
		//{{ 2009. 5. 18  최육사	편지제목
		&& GET( m_wstrLetterTitle )
		//}}
		&& GET( m_iScriptIndex )
		//{{ 2011. 04. 05  김민성	우편 및 거래 감시
		&& GET( m_iFromUnitUID )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_READ_LETTER_REQ, obj, ks )
{
	return PUT( m_iPostNo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_READ_LETTER_REQ, obj, ks )
{
	return GET( m_iPostNo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_READ_LETTER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kLetter )
		;
}

SERIALIZE_DEFINE_GET( KEGS_READ_LETTER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kLetter )
		;
}

//{{ 2012. 08. 21	박세훈	우편 로직 변경
SERIALIZE_DEFINE_PUT( KEGS_GET_ITEM_FROM_LETTER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPostNo )
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		&& PUT( m_cLetterType )
		//}
		&& PUT( m_mapResultItem )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_ITEM_FROM_LETTER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPostNo )
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		&& GET( m_cLetterType )
		//}
		&& GET( m_mapResultItem )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_iED )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KEGS_DELETE_LETTER_REQ, obj, ks )
{
	return PUT( m_vecPostNo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_LETTER_REQ, obj, ks )
{
	return GET( m_vecPostNo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DELETE_LETTER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecPostNo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_LETTER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecPostNo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESTORE_SPIRIT_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iSpirit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_SPIRIT_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iSpirit )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CASH_PRODUCT_INFO_REQ, obj, ks )
{
    return PUT( m_iPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CASH_PRODUCT_INFO_REQ, obj, ks )
{
    return GET( m_iPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CASH_PRODUCT_INFO_ACK, obj, ks )
{
    return PUT( m_bFinal )
		&& PUT( m_vecProductInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CASH_PRODUCT_INFO_ACK, obj, ks )
{
    return GET( m_bFinal )
		&& GET( m_vecProductInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BUY_CASH_ITEM_REQ, obj, ks )
{
    return PUT( m_vecPurchaseReqInfo )
		&& PUT( m_bIsWishList )
		&& PUT( m_iUseCashType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_CASH_ITEM_REQ, obj, ks )
{
    return GET( m_vecPurchaseReqInfo )
		&& GET( m_bIsWishList )
		&& GET( m_iUseCashType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BUY_CASH_ITEM_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_vecPurchaseReqInfo )
        && PUT( m_bIsWishList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_CASH_ITEM_ACK, obj, ks )
{
    return GET( m_iOK )
		&& GET( m_vecPurchaseReqInfo )
		&& GET( m_bIsWishList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PRESENT_CASH_ITEM_REQ, obj, ks )
{
    return PUT( m_wstrPassword )
		&& PUT( m_wstrReceiverNickName )
		&& PUT( m_wstrMessage )
		&& PUT( m_vecPurchaseReqInfo )
		&& PUT( m_bIsWishList )
		&& PUT( m_iUseCashType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PRESENT_CASH_ITEM_REQ, obj, ks )
{
    return GET( m_wstrPassword )
		&& GET( m_wstrReceiverNickName )
		&& GET( m_wstrMessage )
		&& GET( m_vecPurchaseReqInfo )
		&& GET( m_bIsWishList )
		&& GET( m_iUseCashType )
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 10. 6  최육사	자동결재
SERIALIZE_DEFINE_PUT( KEGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_REQ, obj, ks )
{
	return PUT( m_ulProductNo )
		&& PUT( m_usOrderQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_REQ, obj, ks )
{
	return GET( m_ulProductNo )
		&& GET( m_usOrderQuantity )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiUserSN )
		&& PUT( m_byDomainType )
		&& PUT( m_wstrTransactionID )
		&& PUT( m_ulProductNo )
		&& PUT( m_ulSalePrice )
		&& PUT( m_usOrderQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiUserSN )
		&& GET( m_byDomainType )
		&& GET( m_wstrTransactionID )
		&& GET( m_ulProductNo )
		&& GET( m_ulSalePrice )
		&& GET( m_usOrderQuantity )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_AUTO_PAYMENT_RESURRECTION_STONE_NOT, obj, ks )
{
	return PUT( m_iNumAutoPaymentResStone )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PAYMENT_RESURRECTION_STONE_NOT, obj, ks )
{
	return GET( m_iNumAutoPaymentResStone )
		;
}
//}}


SERIALIZE_DEFINE_PUT( KEGS_DEL_TUTORIAL_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DEL_TUTORIAL_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DEL_TUTORIAL_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		//{{ 2011.09.16 조효진  넷 에러 스트링을 클라이언트쪽에서 가져와서 출력
		&& PUT( m_wstrReason )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DEL_TUTORIAL_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		//{{ 2011.09.16 조효진  넷 에러 스트링을 클라이언트쪽에서 가져와서 출력
		&& GET( m_wstrReason )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_TUTORIAL_UPDATE_UNIT_INFO_NOT, obj, ks )
{
	return PUT( m_bIsReply )
		&& PUT( m_iReceiverUID )
		&& PUT( m_kTutorialUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TUTORIAL_UPDATE_UNIT_INFO_NOT, obj, ks )
{
	return GET( m_bIsReply )
		&& GET( m_iReceiverUID )
		&& GET( m_kTutorialUnitInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TUTORIAL_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TUTORIAL_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REQUEST_TUTORIAL_REPLY_NOT, obj, ks )
{
	return PUT( m_iReceiverUnitUID )
		&& PUT( m_kTutorialUnitInfo )
		&& PUT( m_bAccept )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_TUTORIAL_REPLY_NOT, obj, ks )
{
	return GET( m_iReceiverUnitUID )
		&& GET( m_kTutorialUnitInfo )
		&& GET( m_bAccept )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_TUTORIAL_STUDENT_LIST_REQ, obj, ks )
{
	return PUT( m_iViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TUTORIAL_STUDENT_LIST_REQ, obj, ks )
{
	return GET( m_iViewPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_TUTORIAL_STUDENT_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iTotalPage )
		&& PUT( m_iViewPage )
		&& PUT( m_vecTutorialStudentList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TUTORIAL_STUDENT_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iTotalPage )
		&& GET( m_iViewPage )
		&& GET( m_vecTutorialStudentList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESET_SKILL_REQ, obj, ks )
{
	return PUT( m_iSkillID )
		&& PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_SKILL_REQ, obj, ks )
{
	return GET( m_iSkillID )
		&& GET( m_iActiveSkillPageNumber )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESET_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_iDelSkillID )
		&& PUT( m_iSPoint )
		&& PUT( m_iCSPoint )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_iDelSkillID )
		&& GET( m_iSPoint )
		&& GET( m_iCSPoint )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CURRENT_TIME_ACK, obj, ks )
{
    return PUT( m_wstrCurrentTime )
		&& PUT( m_tCurrentTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CURRENT_TIME_ACK, obj, ks )
{
    return GET( m_wstrCurrentTime )
		&& GET( m_tCurrentTime )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_WISH_LIST_ACK, obj, ks )
{
	return PUT( m_mapWishList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_WISH_LIST_ACK, obj, ks )
{
	return GET( m_mapWishList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_MODIFY_WISH_LIST_REQ, obj, ks )
{
	return PUT( m_vecWishListItemID )
		&& PUT( m_bIsDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MODIFY_WISH_LIST_REQ, obj, ks )
{
	return GET( m_vecWishListItemID )
		&& GET( m_bIsDelete )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_MODIFY_WISH_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecWishListItemID )
		&& PUT( m_bIsDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MODIFY_WISH_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecWishListItemID )
		&& GET( m_bIsDelete )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_AGREE_HACK_USER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_cHackingUserType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AGREE_HACK_USER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_cHackingUserType )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STOP_DUNGEON_CONTINUE_TIME_REQ, obj, ks )
{
	return PUT( m_bIsStop )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STOP_DUNGEON_CONTINUE_TIME_REQ, obj, ks )
{
	return GET( m_bIsStop )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_STOP_DUNGEON_CONTINUE_TIME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bIsStop )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STOP_DUNGEON_CONTINUE_TIME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bIsStop )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_NEW_PRESENT_CASH_INVENTORY_NOT, obj, ks )
{
	return PUT( m_iPresentCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_PRESENT_CASH_INVENTORY_NOT, obj, ks )
{
	return GET( m_iPresentCount )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHECK_PRESENT_CASH_INVENTORY_NOT, obj, ks )
{
	return PUT( m_ulOrderNo )
		&& PUT( m_ulProductNo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_PRESENT_CASH_INVENTORY_NOT, obj, ks )
{
	return GET( m_ulOrderNo )
		&& GET( m_ulProductNo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT, obj, ks )
{
	return PUT( m_wstrSenderNickName )
		&& PUT( m_wstrReceiverUserID )
		&& PUT( m_wstrMessage )
		&& PUT( m_kBillProductInfo )	// SERV_GLOBAL_BILLING
		;
}

SERIALIZE_DEFINE_GET( KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT, obj, ks )
{
	return GET( m_wstrSenderNickName )
		&& GET( m_wstrReceiverUserID )
		&& GET( m_wstrMessage )
		&& GET( m_kBillProductInfo )	// SERV_GLOBAL_BILLING
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_RANKING_INFO_REQ, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_uiViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RANKING_INFO_REQ, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_uiViewPage )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_RANKING_INFO_ACK, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_uiViewPage )
		&& PUT( m_uiTotalPage )
		&& PUT( m_vecHenirRankingPage )
		&& PUT( m_vecDungeonRankingPage )
		&& PUT( m_vecPvpRankingPage )
		&& PUT( m_vecMyHenirRanking )
		&& PUT( m_vecMyDungeonRanking )
		&& PUT( m_vecMyPvpRanking )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RANKING_INFO_ACK, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_uiViewPage )
		&& GET( m_uiTotalPage )
		&& GET( m_vecHenirRankingPage )
		&& GET( m_vecDungeonRankingPage )
		&& GET( m_vecPvpRankingPage )
		&& GET( m_vecMyHenirRanking )
		&& GET( m_vecMyDungeonRanking )
		&& GET( m_vecMyPvpRanking )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ATTRIB_ENCHANT_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_cAttribEnchantSlotNo )
		&& PUT( m_cAttribEnchantID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ATTRIB_ENCHANT_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_cAttribEnchantSlotNo )
		&& GET( m_cAttribEnchantID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ATTRIB_ENCHANT_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_iEnchantedItemUID )
		&& PUT( m_iEnchantedItemID )
		&& PUT( m_cAttribEnchantSlotNo )
		&& PUT( m_cAttribEnchantID )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ATTRIB_ENCHANT_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_iEnchantedItemUID )
		&& GET( m_iEnchantedItemID )
		&& GET( m_cAttribEnchantSlotNo )
		&& GET( m_cAttribEnchantID )
		&& GET( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_IDENTIFY_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_IDENTIFY_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iQuantity )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_IDENTIFY_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_IDENTIFY_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_mapInsertedItem )
		&& GET( m_vecKInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_POST_BLACK_LIST_ACK, obj, ks )
{
	return PUT( m_vecBlackList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_POST_BLACK_LIST_ACK, obj, ks )
{
	return GET( m_vecBlackList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_POST_BLACK_LIST_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_POST_BLACK_LIST_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_POST_BLACK_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_POST_BLACK_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrNickName )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EQUIP_TITLE_REQ, obj, ks )
{
	return PUT( m_iTitleID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EQUIP_TITLE_REQ, obj, ks )
{
	return GET( m_iTitleID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_EQUIP_TITLE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iTitleID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EQUIP_TITLE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iTitleID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_TITLE_EXPIRATION_NOT, obj, ks )
{
    return PUT( m_iTitleID )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_TITLE_EXPIRATION_NOT, obj, ks )
{
    return GET( m_iTitleID )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_NEW_MISSION_NOT, obj, ks )
{
	return PUT( m_bFirst )
		&& PUT( m_vecNewMission )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_MISSION_NOT, obj, ks )
{
	return GET( m_bFirst )
		&& GET( m_vecNewMission )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_MISSION_NOT, obj, ks )
{
	return PUT( m_vecMissionInst )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_MISSION_NOT, obj, ks )
{
	return GET( m_vecMissionInst )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_REWARD_TITLE_NOT, obj, ks )
{
	return PUT( m_kTitle )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REWARD_TITLE_NOT, obj, ks )
{
	return GET( m_kTitle )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USE_ITEM_IN_INVENTORY_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iTempCode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_ITEM_IN_INVENTORY_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iTempCode )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_ITEM_IN_INVENTORY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_iUsedItemID )
		&& PUT( m_iWarpPointMapID )
		//{{ 2012. 10. 25	박세훈	배틀필드 시스템
		&& PUT( m_iED )
		//}}
		//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
//#ifdef SERV_PET_AUTO_LOOTING
		&& PUT( m_iTempCode )
//#endif SERV_PET_AUTO_LOOTING
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_ITEM_IN_INVENTORY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_iUsedItemID )
		&& GET( m_iWarpPointMapID )
		//{{ 2012. 10. 25	박세훈	배틀필드 시스템
		&& GET( m_iED )
		//}}
		//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
//#ifdef SERV_PET_AUTO_LOOTING
		&& GET( m_iTempCode )
//#endif SERV_PET_AUTO_LOOTING
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_TITLE_CHEAT_REQ, obj, ks )
{
	return PUT( m_iTitleID )
		&& PUT( m_iPeriod )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_TITLE_CHEAT_REQ, obj, ks )
{
	return GET( m_iTitleID )
		&& GET( m_iPeriod )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EXPAND_CHAR_SLOT_NOT, obj, ks )
{
	return PUT( m_iCharSlotSize )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_CHAR_SLOT_NOT, obj, ks )
{
	return GET( m_iCharSlotSize )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESTORE_ITEM_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_ITEM_REQ, obj, ks )
{
	return GET( m_iItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RESTORE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ENCHANT_ATTACH_ITEM_REQ, obj, ks )
{
	return PUT( m_iDestItemUID )
		&& PUT( m_iAttachItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENCHANT_ATTACH_ITEM_REQ, obj, ks )
{
	return GET( m_iDestItemUID )
		&& GET( m_iAttachItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ENCHANT_ATTACH_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENCHANT_ATTACH_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_FIELD_REQ, obj, ks )
{
	return PUT( m_iMapID )
		//{{ 2012. 10. 18	최육사	배틀필드 시스템
		&& PUT( m_bUseWarpItem )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_FIELD_REQ, obj, ks )
{
	return GET( m_iMapID )
		//{{ 2012. 10. 18	최육사	배틀필드 시스템
		&& GET( m_bUseWarpItem )
		//}}
		;
}

//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
SERIALIZE_DEFINE_PUT( KEGS_STATE_CHANGE_FIELD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iLimitLevel )
		&& PUT( m_iRequireDungeonID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_STATE_CHANGE_FIELD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iLimitLevel )
		&& GET( m_iRequireDungeonID )
		;
}
//}}


SERIALIZE_DEFINE_PUT( KEGS_FIELD_LOADING_COMPLETE_REQ, obj, ks )
{
	return PUT( m_vPos )
		&& PUT( m_ucLastTouchLineIndex )
		&& PUT( m_usLastPosValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FIELD_LOADING_COMPLETE_REQ, obj, ks )
{
	return GET( m_vPos )
		&& GET( m_ucLastTouchLineIndex )
		&& GET( m_usLastPosValue )
		;
}

//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
//}}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_USER_LIST_IN_SECTOR_NOT, obj, ks )
{
	return PUT( m_vecNewUserList )
        && PUT( m_vecDelUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_USER_LIST_IN_SECTOR_NOT, obj, ks )
{
	return GET( m_vecNewUserList )
        && GET( m_vecDelUserList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_FIELD_UNIT_SYNC_DATA_NOT, obj, ks )
{
	return PUT( m_vPos )
		&& PUT( m_vecFieldFrame )
		&& PUT( m_bIsRight )
		&& PUT( m_ucLastTouchLineIndex )
		&& PUT( m_usLastPosValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FIELD_UNIT_SYNC_DATA_NOT, obj, ks )
{
	return GET( m_vPos )
		&& GET( m_vecFieldFrame )
		&& GET( m_bIsRight )
		&& GET( m_ucLastTouchLineIndex )
		&& GET( m_usLastPosValue )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_vPos )
		&& PUT( m_vecFieldFrame )
		&& PUT( m_bIsRight )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_vPos )
		&& GET( m_vecFieldFrame )
		&& GET( m_bIsRight )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_COMMUNITY_USER_LIST_REQ, obj, ks )
{
	return PUT( m_uiViewPage )
		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		&& PUT( m_bMode )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_COMMUNITY_USER_LIST_REQ, obj, ks )
{
	return GET( m_uiViewPage )
		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		&& GET( m_bMode )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COMMUNITY_USER_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiTotalPage )
		&& PUT( m_uiViewPage )
		&& PUT( m_vecUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COMMUNITY_USER_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiTotalPage )
		&& GET( m_uiViewPage )
		&& GET( m_vecUserList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_OPTION_UPDATE_REQ, obj, ks )
{
	return PUT( m_kFieldOption )
		;
}

SERIALIZE_DEFINE_GET( KEGS_OPTION_UPDATE_REQ, obj, ks )
{
	return GET( m_kFieldOption )
		;
}

//////////////////////////////////////////////////////////////////////////
//SERIALIZE_DEFINE_PUT( KEGS_CHANNEL_CHANGE_REQ, obj, ks )
//{
//	return PUT( m_iChannelID )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_CHANNEL_CHANGE_REQ, obj, ks )
//{
//	return GET( m_iChannelID )
//		;
//}
//
//SERIALIZE_DEFINE_PUT( KEGS_CHANNEL_CHANGE_ACK, obj, ks )
//{
//	return PUT( m_iOK )
//		&& PUT( m_iServerUID )
//		&& PUT( m_iAccountAuthKey )
//		&& PUT( m_iUnitUID )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_CHANNEL_CHANGE_ACK, obj, ks )
//{
//	return GET( m_iOK )
//		&& GET( m_iServerUID )
//		&& GET( m_iAccountAuthKey )
//		&& GET( m_iUnitUID )
//		;
//}

//SERIALIZE_DEFINE_PUT( KEGS_CONNECT_CHANNEL_CHANGE_REQ, obj, ks )
//{
//	return PUT( m_wstrVersion )
//		&& PUT( m_iServerUID )
//		&& PUT( m_iAccountAuthKey )
//		&& PUT( m_iUnitUID )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_CONNECT_CHANNEL_CHANGE_REQ, obj, ks )
//{
//	return GET( m_wstrVersion )
//		&& GET( m_iServerUID )
//		&& GET( m_iAccountAuthKey )
//		&& GET( m_iUnitUID )
//		;
//}
//
//SERIALIZE_DEFINE_PUT( KEGS_CONNECT_CHANNEL_CHANGE_ACK, obj, ks )
//{
//	return PUT( m_iOK )
//		&& PUT( m_UDPPort )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_CONNECT_CHANNEL_CHANGE_ACK, obj, ks )
//{
//	return GET( m_iOK )
//		&& GET( m_UDPPort )
//		;
//}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_FRIEND_INFO_REQ, obj, ks )
{
	return PUT( m_bDelete )
		&& PUT( m_wstrFriendNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_FRIEND_INFO_REQ, obj, ks )
{
	return GET( m_bDelete )
		&& GET( m_wstrFriendNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_FRIEND_INFO_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrFriendNickName )
		&& PUT( m_iFriendUnitUID )
		&& PUT( m_iChannelID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_FRIEND_INFO_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrFriendNickName )
		&& GET( m_iFriendUnitUID )
		&& GET( m_iChannelID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_MY_MESSENGER_SN_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiKNMSerialNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_MY_MESSENGER_SN_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiKNMSerialNum )
		;
}


//{{ 2009. 5. 8  최육사		실시간 레벨업
SERIALIZE_DEFINE_PUT( KEGS_CHAR_LEVEL_UP_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_ucLevel )
		&& PUT( m_kBaseStat )
		&& PUT( m_kGameStat )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHAR_LEVEL_UP_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_ucLevel )
		&& GET( m_kBaseStat )
		&& GET( m_kGameStat )
		;
}
//}}

//{{ 2009. 5. 14  최육사	내구도 소모 개편
SERIALIZE_DEFINE_PUT( KEGS_DECREASE_ENDURANCE_NOT, obj, ks )
{
	return PUT( m_vecEnduranceUpdate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DECREASE_ENDURANCE_NOT, obj, ks )
{
	return GET( m_vecEnduranceUpdate )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ZERO_ENDURANCE_ITEM_IN_ROOM_NOT, obj, ks )
{
	return PUT( m_UnitUID )
		&& PUT( m_vecInventorySlotInfo )
		&& PUT( m_kGameStat )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ZERO_ENDURANCE_ITEM_IN_ROOM_NOT, obj, ks )
{
	return GET( m_UnitUID )
		&& GET( m_vecInventorySlotInfo )
		&& GET( m_kGameStat )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DECREASE_SPIRIT_NOT, obj, ks )
{
	return PUT( m_iSpirit )
		//{{ 2009. 10. 29  최육사	길드레벨
		&& PUT( m_iHonorPoint )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_DECREASE_SPIRIT_NOT, obj, ks )
{
	return GET( m_iSpirit )
		//{{ 2009. 10. 29  최육사	길드레벨
		&& GET( m_iHonorPoint )
		//}}
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_CASH_SKILL_POINT_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_iCSPoint )
		&& PUT( m_wstrCSPointEndDate )
		&& PUT( m_bUpdateEndDateOnly )
		//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
		&& PUT( m_iSkillPointItemID )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_CASH_SKILL_POINT_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_iCSPoint )
		&& GET( m_wstrCSPointEndDate )
		&& GET( m_bUpdateEndDateOnly )
		//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
		&& GET( m_iSkillPointItemID )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EXPIRE_CASH_SKILL_POINT_NOT, obj, ks )
{
	return PUT( m_iUnitUID )

		&& PUT( m_vecRetrievedSPoint )


		&& PUT( m_vecMapHaveSKill )

		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPIRE_CASH_SKILL_POINT_NOT, obj, ks )
{
	return GET( m_iUnitUID )

		&& GET( m_vecRetrievedSPoint )

		&& GET( m_vecMapHaveSKill )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EXPAND_SKILL_SLOT_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cSkillSlotBExpirationState )
		&& PUT( m_wstrSkillSlotBEndDate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_SKILL_SLOT_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cSkillSlotBExpirationState )
		&& GET( m_wstrSkillSlotBEndDate )
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 8. 4  최육사		봉인스킬
SERIALIZE_DEFINE_PUT( KEGS_UNSEAL_SKILL_NOT, obj, ks )
{
	return PUT( m_iSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNSEAL_SKILL_NOT, obj, ks )
{
	return GET( m_iSkillID )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 6. 3  최육사		확성기
SERIALIZE_DEFINE_PUT( KEGS_USE_MEGAPHONE_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_MEGAPHONE_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_MEGAPHONE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_MEGAPHONE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_MEGAPHONE_MESSAGE_NOT, obj, ks )
{
	return PUT( m_wstrSenderNickName )
		&& PUT( m_iSenderChannelID )
		&& PUT( m_iSenderLocation )
		&& PUT( m_cMessageColor )
		&& PUT( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MEGAPHONE_MESSAGE_NOT, obj, ks )
{
	return GET( m_wstrSenderNickName )
		&& GET( m_iSenderChannelID )
		&& GET( m_iSenderLocation )
		&& GET( m_cMessageColor )
		&& GET( m_wstrMessage )
		;
}
//}}

//{{ 2009. 7. 10  최육사	사기 시스템
SERIALIZE_DEFINE_PUT( KEGS_PARTY_FEVER_POINT_NOT, obj, ks )
{
	return PUT( m_iPartyFever )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PARTY_FEVER_POINT_NOT, obj, ks )
{
	return GET( m_iPartyFever )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PARTY_FEVER_STATE_NOT, obj, ks )
{
	return PUT( m_bIsStart )
		&& PUT( m_iPartyFever )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PARTY_FEVER_STATE_NOT, obj, ks )
{
	return GET( m_bIsStart )
		&& GET( m_iPartyFever )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KEGS_FRIEND_POSITION_UPDATE_NOT, obj, ks )
{
    return  PUT( m_mapFriendPosition );
}

SERIALIZE_DEFINE_GET( KEGS_FRIEND_POSITION_UPDATE_NOT, obj, ks )
{
    return  GET( m_mapFriendPosition );
}








//{{ dmlee 2009/07/17 헤니르 시공 비밀 스테이지 입장 관련



SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bCanEnterSecretStage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bCanEnterSecretStage )
		;
}



SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SECRET_STAGE_ENTER_SELECT_NOT, obj, ks )
{
	return  PUT( m_bWantToEnterSecretStage );
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SECRET_STAGE_ENTER_SELECT_NOT, obj, ks )
{
	return  GET( m_bWantToEnterSecretStage );
}




SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT, obj, ks )
{
	return  PUT( m_bProceedToEnterSecretStage );
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT, obj, ks )
{
	return  GET( m_bProceedToEnterSecretStage );
}


//{{ 2009. 8. 7  최육사		은행
SERIALIZE_DEFINE_PUT( KEGS_GET_MY_BANK_INFO_ACK, obj, ks )
{
	return PUT( m_mapItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_MY_BANK_INFO_ACK, obj, ks )
{
	return GET( m_mapItem )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_EXPAND_BANK_SLOT_NOT, obj, ks )
{
	return PUT( m_mapExpandedCategorySlot )
        && PUT( m_iOK )
        && PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_BANK_SLOT_NOT, obj, ks )
{
	return GET( m_mapExpandedCategorySlot )
        && GET( m_iOK )
        && GET( m_iED )
		;
}
//}}

//{{ 2009. 8. 20  최육사	신학기 이벤트
SERIALIZE_DEFINE_PUT( KEGS_PVP_KILL_DIE_INFO_NOT, obj, ks )
{
	return PUT( m_bShowKillCount )
		&& PUT( m_iKillCount )
		&& PUT( m_iDieCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PVP_KILL_DIE_INFO_NOT, obj, ks )
{
	return GET( m_bShowKillCount )
		&& GET( m_iKillCount )
		&& GET( m_iDieCount )
		;
}
//}}

//{{ 2009. 8. 27  최육사	밀봉
SERIALIZE_DEFINE_PUT( KEGS_SEAL_ITEM_REQ, obj, ks )
{
	return PUT( m_iDestItemUID )
		&& PUT( m_iSealItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEAL_ITEM_REQ, obj, ks )
{
	return GET( m_iDestItemUID )
		&& GET( m_iSealItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SEAL_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEAL_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UNSEAL_ITEM_REQ, obj, ks )
{
	return PUT( m_iDestItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNSEAL_ITEM_REQ, obj, ks )
{
	return GET( m_iDestItemUID )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KECH_VERIFY_ACCOUNT_REQ, obj, ks )
{
    return PUT( m_bLogin )
		&& PUT( m_wstrID )
		&& PUT( m_wstrPassword )
		&& PUT( m_iChannelingCode )
		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
		&& PUT( m_strMachineID )
		//}}
		//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
		//}}
		//{{ 2013.03.05 조효진		KOG_OTP 정리 및 재구성
		&& PUT( m_bIDVerified )
		//}}
		&& PUT( m_bSteamClient )
		&& PUT( m_bAeriaClient )
		;
}

SERIALIZE_DEFINE_GET( KECH_VERIFY_ACCOUNT_REQ, obj, ks )
{
    return GET( m_bLogin )
        && GET( m_wstrID )
        && GET( m_wstrPassword )
		&& GET( m_iChannelingCode )
		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
		&& GET( m_strMachineID )
		//}}
		//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
		//}}
		//{{ 2013.03.05 조효진		KOG_OTP 정리 및 재구성
		&& GET( m_bIDVerified )
		//}}
		&& GET( m_bSteamClient )
		&& GET( m_bAeriaClient )
		;
}

SERIALIZE_DEFINE_PUT( KECH_VERIFY_ACCOUNT_ACK, obj, ks )
{
    return PUT( m_iOK )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrPassport )
		&& PUT( m_iChannelingCode )
		&& PUT( m_wstrCurrentTime )

		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{ 2013.03.05 조효진		KOG_OTP 정리 및 재구성
		&& PUT( m_wstrUserID )
		//}}
		//{{ 2010. 12. 02 조효진	캐쉬샵 접속 시, 로그인 여부 인증을 위한 문자열
		&& PUT( m_wstrPurchaseTok )
		//}}
		&& PUT( m_strAgreementURL )
		;
}

SERIALIZE_DEFINE_GET( KECH_VERIFY_ACCOUNT_ACK, obj, ks )
{
    return GET( m_iOK )
        && GET( m_iUserUID )
        && GET( m_wstrPassport )
		&& GET( m_iChannelingCode )
		&& GET( m_wstrCurrentTime )

		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{ 2013.03.05 조효진		KOG_OTP 정리 및 재구성
		&& GET( m_wstrUserID )
		//}}
		//{{ 2010. 12. 02 조효진	캐쉬샵 접속 시, 로그인 여부 인증을 위한 문자열
		&& GET( m_wstrPurchaseTok )
		//}}
		&& GET( m_strAgreementURL )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 9. 17  최육사	길드

SERIALIZE_DEFINE_PUT( KEGS_CREATE_GUILD_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_wstrGuildName )
		&& PUT( m_wstrGuildMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_GUILD_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_wstrGuildName )
		&& GET( m_wstrGuildMessage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CREATE_GUILD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kGuildInfo )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_GUILD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kGuildInfo )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_USER_LIST_REQ, obj, ks )
{
	return PUT( m_uiViewPage )
		&& PUT( m_cGuildUserListType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_USER_LIST_REQ, obj, ks )
{
	return GET( m_uiViewPage )
		&& GET( m_cGuildUserListType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_USER_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiTotalPage )
		&& PUT( m_uiViewPage )
		&& PUT( m_cGuildUserListType )
		&& PUT( m_iLoginedMemberCount )
		&& PUT( m_vecMemberPositionGradeList )
		&& PUT( m_vecMemberMessageList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_USER_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiTotalPage )
		&& GET( m_uiViewPage )
		&& GET( m_cGuildUserListType )
		&& GET( m_iLoginedMemberCount )
		&& GET( m_vecMemberPositionGradeList )
		&& GET( m_vecMemberMessageList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GUILD_MEMBER_INFO_NOT, obj, ks )
{
	return PUT( m_kMemberInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GUILD_MEMBER_INFO_NOT, obj, ks )
{
	return GET( m_kMemberInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GUILD_MESSAGE_NOT, obj, ks )
{
	return PUT( m_cMessageType )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_wstrMessage )
		&& PUT( m_iMessageData )
		&& PUT( m_iMessageData2 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GUILD_MESSAGE_NOT, obj, ks )
{
	return GET( m_cMessageType )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		&& GET( m_wstrMessage )
		&& GET( m_iMessageData )
		&& GET( m_iMessageData2 )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_GUILD_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_GUILD_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_iUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_GUILD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrReceiverNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_GUILD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrReceiverNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_GUILD_NOT, obj, ks )
{
	return PUT( m_kGuildInfo )
		&& PUT( m_wstrSenderNickName )
		&& PUT( m_bAutoJoinGuild )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_GUILD_NOT, obj, ks )
{
	return GET( m_kGuildInfo )
		&& GET( m_wstrSenderNickName )
		&& GET( m_bAutoJoinGuild )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_GUILD_REPLY_REQ, obj, ks )
{
	return PUT( m_iGuildUID )
		&& PUT( m_wstrSenderNickName )
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_GUILD_REPLY_REQ, obj, ks )
{
	return GET( m_iGuildUID )
		&& GET( m_wstrSenderNickName )
		&& GET( m_iReason )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INVITE_GUILD_REPLY_DENY_NOT, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_iReason )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INVITE_GUILD_REPLY_DENY_NOT, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_iReason )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_MEMBER_GRADE_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_ucMemberShipGrade )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_MEMBER_GRADE_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_ucMemberShipGrade )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_MEMBER_GRADE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrTargetNickName )
		&& PUT( m_ucMemberShipGrade )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_MEMBER_GRADE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrTargetNickName )
		&& GET( m_ucMemberShipGrade )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_MESSAGE_REQ, obj, ks )
{
	return PUT( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_MESSAGE_REQ, obj, ks )
{
	return GET( m_wstrMessage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KICK_GUILD_MEMBER_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_KICK_GUILD_MEMBER_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_KICK_GUILD_MEMBER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bKickBySysop )
		;
}

SERIALIZE_DEFINE_GET( KEGS_KICK_GUILD_MEMBER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bKickBySysop )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_MEMBER_MESSAGE_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_MEMBER_MESSAGE_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_wstrMessage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_MEMBER_MESSAGE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_MEMBER_MESSAGE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bDelete )
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2012. 02. 22	박세훈	길드 이름 변경권
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_NAME_CHECK_REQ, obj, ks )
{
	return PUT( m_wstrGuildName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_NAME_CHECK_REQ, obj, ks )
{
	return GET( m_wstrGuildName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_NAME_REQ, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_ulOrderNo )
		&& PUT( m_ulProductNo )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_NAME_REQ, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_ulOrderNo )
		&& GET( m_ulProductNo )
		&& GET( m_iQuantity )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GUILD_NAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kGuildInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GUILD_NAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kGuildInfo )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 9. 22  최육사	전직캐쉬
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_JOB_CASH_ITEM_NOT, obj, ks )
{
	return PUT( m_kUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_JOB_CASH_ITEM_NOT, obj, ks )
{
	return GET( m_kUnitInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_USER_INFO_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cUnitClass )
		//{{ 2009. 10. 9  최육사	길드
		&& PUT( m_wstrGuildName )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_USER_INFO_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cUnitClass )
		//{{ 2009. 10. 9  최육사	길드
		&& GET( m_wstrGuildName )
		//}}
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 11. 23  최육사	길드스킬

SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_SKILL_REQ, obj, ks )
{
	return PUT( m_iGuildSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_SKILL_REQ, obj, ks )
{
	return GET( m_iGuildSkillID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_SKILL_NOT, obj, ks )
{
	return PUT( m_iGuildSkillID )
		&& PUT( m_iGuildSkillLv )
		&& PUT( m_iGuildSkillCSPoint )
		&& PUT( m_iGuildSPoint )
		&& PUT( m_iGuildCSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_SKILL_NOT, obj, ks )
{
	return GET( m_iGuildSkillID )
		&& GET( m_iGuildSkillLv )
		&& GET( m_iGuildSkillCSPoint )
		&& GET( m_iGuildSPoint )
		&& GET( m_iGuildCSPoint )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESET_GUILD_SKILL_REQ, obj, ks )
{
	return PUT( m_iGuildSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_GUILD_SKILL_REQ, obj, ks )
{
	return GET( m_iGuildSkillID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESET_GUILD_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_GUILD_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RESET_GUILD_SKILL_NOT, obj, ks )
{
	return PUT( m_iDelGuildSkillID )
		&& PUT( m_iRestoreGuildSPoint )
		&& PUT( m_iRestoreGuildCSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_GUILD_SKILL_NOT, obj, ks )
{
	return GET( m_iDelGuildSkillID )
		&& GET( m_iRestoreGuildSPoint )
		&& GET( m_iRestoreGuildCSPoint )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INIT_GUILD_SKILL_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INIT_GUILD_SKILL_REQ, obj, ks )
{
	return GET( m_iItemUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INIT_GUILD_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INIT_GUILD_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_INIT_GUILD_SKILL_NOT, obj, ks )
{
	return PUT( m_iGuildSPoint )
		&& PUT( m_iGuildCSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_INIT_GUILD_SKILL_NOT, obj, ks )
{
	return GET( m_iGuildSPoint )
		&& GET( m_iGuildCSPoint )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT, obj, ks )
{
	return PUT( m_iGuildSPoint )
		&& PUT( m_iGuildCSPoint )
		&& PUT( m_vecGuildSkillData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT, obj, ks )
{
	return GET( m_iGuildSPoint )
		&& GET( m_iGuildCSPoint )
		&& GET( m_vecGuildSkillData )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SKILL_INFO_IN_ROOM_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_ucMemberShipGrade )
		&& PUT( m_kGameStat )
		&& PUT( m_UnitSkillData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SKILL_INFO_IN_ROOM_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_ucMemberShipGrade )
		&& GET( m_kGameStat )
		&& GET( m_UnitSkillData )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_GUILD_SKILL_POINT_REQ, obj, ks )
{
	return PUT( m_iGuildSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_GUILD_SKILL_POINT_REQ, obj, ks )
{
	return GET( m_iGuildSPoint )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_GUILD_SKILL_POINT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iCurGuildSPoint )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_GUILD_SKILL_POINT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iCurGuildSPoint )
		;
}

//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
SERIALIZE_DEFINE_PUT( KEGS_TIME_EVENT_INFO_NOT, obj, ks )
{
	return PUT( m_cNotifyType )
		&& PUT( m_vecTimeEvent )
		;
}

SERIALIZE_DEFINE_GET( KEGS_TIME_EVENT_INFO_NOT, obj, ks )
{
	return GET( m_cNotifyType )
		&& GET( m_vecTimeEvent )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_PVP_ITEM_REQ, obj, ks )
{
	return PUT( m_iPositionIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_PVP_ITEM_REQ, obj, ks )
{
	return GET( m_iPositionIndex )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_PVP_ITEM_NOT, obj, ks )
{
	return PUT( m_iPositionIndex )
		&& PUT( m_kDropItemData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_PVP_ITEM_NOT, obj, ks )
{
	return GET( m_iPositionIndex )
		&& GET( m_kDropItemData )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ITEM_NOT, obj, ks )
{
	return PUT( m_iDropItemUID )
		&& PUT( m_iItemID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ITEM_NOT, obj, ks )
{
	return GET( m_iDropItemUID )
		&& GET( m_iItemID )
		;
}

//////////////////////////////////////////////////////////////////////////
//{{ 2009. 12. 8  최육사	크리스마스이벤트
SERIALIZE_DEFINE_PUT( KEGS_CHECK_TIME_EVENT_COMPLETE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRewardCount )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_TIME_EVENT_COMPLETE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRewardCount )
		&& GET( m_vecInventorySlotInfo )
		;
}
//}}












//{{ 2009. 12. 29  최육사	NPCKILLALL해킹방지
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_KILLALLNPC_CHECK_ACK, obj, ks )
{
	return PUT( m_bResult )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_KILLALLNPC_CHECK_ACK, obj, ks )
{
	return GET( m_bResult )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 12. 31  최육사	길드 광고

SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_AD_LIST_REQ, obj, ks )
{
	return PUT( m_cSortType )
		&& PUT( m_uiViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_AD_LIST_REQ, obj, ks )
{
	return GET( m_cSortType )
		&& GET( m_uiViewPage )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_AD_LIST_ACK, obj, ks )
{
	return PUT( m_cSortType )
		&& PUT( m_uiTotalPage )
		&& PUT( m_uiViewPage )
		&& PUT( m_vecGuildAdList )
		&& PUT( m_bCanShowApplyList )
		&& PUT( m_bRegMyGuildAd )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_AD_LIST_ACK, obj, ks )
{
	return GET( m_cSortType )
		&& GET( m_uiTotalPage )
		&& GET( m_uiViewPage )
		&& GET( m_vecGuildAdList )
		&& GET( m_bCanShowApplyList )
		&& GET( m_bRegMyGuildAd )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_SKILL_IN_BOARD_REQ, obj, ks )
{
	return PUT( m_iGuildUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_SKILL_IN_BOARD_REQ, obj, ks )
{
	return GET( m_iGuildUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_GUILD_SKILL_IN_BOARD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kGuildSkillInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_GUILD_SKILL_IN_BOARD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kGuildSkillInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REGISTRATION_GUILD_AD_REQ, obj, ks )
{
	return PUT( m_sPeriod )
		&& PUT( m_wstrAdMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REGISTRATION_GUILD_AD_REQ, obj, ks )
{
	return GET( m_sPeriod )
		&& GET( m_wstrAdMessage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REGISTRATION_GUILD_AD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_sPeriod )
		&& PUT( m_wstrAdMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REGISTRATION_GUILD_AD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_sPeriod )
		&& GET( m_wstrAdMessage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_APPLY_JOIN_GUILD_REQ, obj, ks )
{
	return PUT( m_iGuildUID )
		&& PUT( m_wstrMessage )
		&& PUT( m_bOldApplyDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_APPLY_JOIN_GUILD_REQ, obj, ks )
{
	return GET( m_iGuildUID )
		&& GET( m_wstrMessage )
		&& GET( m_bOldApplyDelete )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_APPLY_JOIN_GUILD_LIST_REQ, obj, ks )
{
	return PUT( m_uiViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_APPLY_JOIN_GUILD_LIST_REQ, obj, ks )
{
	return GET( m_uiViewPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_APPLY_JOIN_GUILD_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrGuildName )
		&& PUT( m_uiTotalPage )
		&& PUT( m_uiViewPage )
		&& PUT( m_vecApplyJoinGuildList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_APPLY_JOIN_GUILD_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrGuildName )
		&& GET( m_uiTotalPage )
		&& GET( m_uiViewPage )
		&& GET( m_vecApplyJoinGuildList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ACCEPT_JOIN_GUILD_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ACCEPT_JOIN_GUILD_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ACCEPT_JOIN_GUILD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrReceiverNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ACCEPT_JOIN_GUILD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrReceiverNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_DELETE_APPLY_JOIN_GUILD_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_APPLY_JOIN_GUILD_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		;
}


//}}
//////////////////////////////////////////////////////////////////////////

//{{ 2010. 01. 07  최육사	추천인리스트
SERIALIZE_DEFINE_PUT( KEGS_GET_RECOMMEND_USER_LIST_REQ, obj, ks )
{
	return PUT( m_uiViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RECOMMEND_USER_LIST_REQ, obj, ks )
{
	return GET( m_uiViewPage )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_RECOMMEND_USER_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_uiTotalPage )
		&& PUT( m_uiViewPage )
		&& PUT( m_vecRecommendUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RECOMMEND_USER_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_uiTotalPage )
		&& GET( m_uiViewPage )
		&& GET( m_vecRecommendUserList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_RECOMMEND_USER_INFO_NOT, obj, ks )
{
	return PUT( m_iRecommendUnitUID )
		&& PUT( m_cType )
		&& PUT( m_iData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RECOMMEND_USER_INFO_NOT, obj, ks )
{
	return GET( m_iRecommendUnitUID )
		&& GET( m_cType )
		&& GET( m_iData )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 01. 29  최육사	PC방 상용화
SERIALIZE_DEFINE_PUT( KEGS_PC_BANG_AUTH_RESULT_NOT, obj, ks )
{
	return PUT( m_cType )
		&& PUT( m_uiData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PC_BANG_AUTH_RESULT_NOT, obj, ks )
{
	return GET( m_cType )
		&& GET( m_uiData )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 02. 06  최육사	일일 이벤트 퀘스트

SERIALIZE_DEFINE_PUT( KEGS_EVENT_QUEST_INFO_NOT, obj, ks )
{
	return PUT( m_cType )
		&& PUT( m_vecQuestID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EVENT_QUEST_INFO_NOT, obj, ks )
{
	return GET( m_cType )
		&& GET( m_vecQuestID )
		;
}

//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2010. 02. 12  최육사	해킹툴 리스트

SERIALIZE_DEFINE_PUT( KEGS_HACKING_TOOL_LIST_NOT, obj, ks )
{
	return PUT( m_iRandomKey )
		&& PUT( m_vecHackingToolList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_HACKING_TOOL_LIST_NOT, obj, ks )
{
	return GET( m_iRandomKey )
		&& GET( m_vecHackingToolList )
		;
}

//}}


SERIALIZE_DEFINE_PUT( KEGS_SEARCH_TRADE_BOARD_REQ, obj, ks )
{
	return PUT( m_wstrFinder )
		&& PUT( m_cSlotCategory )
		&& PUT( m_cEqipPosition )
		&& PUT( m_cUnitType )
		&& PUT( m_cUnitClass )
		&& PUT( m_cMinLevel )
		&& PUT( m_cMaxLevel )
		&& PUT( m_vecItemGrade )
        && PUT( m_bStrictSearchOption )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEARCH_TRADE_BOARD_REQ, obj, ks )
{
	return GET( m_wstrFinder )
		&& GET( m_cSlotCategory )
		&& GET( m_cEqipPosition )
		&& GET( m_cUnitType )
		&& GET( m_cUnitClass )
		&& GET( m_cMinLevel )
		&& GET( m_cMaxLevel )
		&& GET( m_vecItemGrade )
        && GET( m_bStrictSearchOption )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SEARCH_TRADE_BOARD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecItemInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SEARCH_TRADE_BOARD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecItemInfo )
		;
}



//{{ 2010. 03. 22  최육사	기술의 노트

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_EXPAND_SKILL_NOTE_PAGE_NUM_NOT, obj, ks )
{
	return PUT( m_cExpandedPageNum )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_SKILL_NOTE_PAGE_NUM_NOT, obj, ks )
{
	return GET( m_cExpandedPageNum )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REG_SKILL_NOTE_MEMO_REQ, obj, ks )
{
	return PUT( m_cSkillNotePageNum )
		&& PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_SKILL_NOTE_MEMO_REQ, obj, ks )
{
	return GET( m_cSkillNotePageNum )
		&& GET( m_iItemUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_REG_SKILL_NOTE_MEMO_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_cSkillNotePageNum )
		&& PUT( m_iMemoID )
		&& PUT( m_vecInventorySlotInfo )
		&& PUT( m_vecSkillNote )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_SKILL_NOTE_MEMO_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_cSkillNotePageNum )
		&& GET( m_iMemoID )
		&& GET( m_vecInventorySlotInfo )
		&& GET( m_vecSkillNote )
		;
}

//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 05. 06  최육사	비밀던전 헬모드
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT, obj, ks )
{
	return PUT( m_fRemainingTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT, obj, ks )
{
	return GET( m_fRemainingTime )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_EFFECT_TIME_OUT_NOT, obj, ks )
{
	return PUT( m_cType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DUNGEON_EFFECT_TIME_OUT_NOT, obj, ks )
{
	return GET( m_cType )
		;
}
//}}


//{{ 2010. 05. 06  최육사	에러 팝업
SERIALIZE_DEFINE_PUT( KEGS_ERROR_POPUP_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iErrorData1 )
		&& PUT( m_iErrorData2 )
		//{{ 던전 입장권 에러 팝업 변경
		&& PUT( m_bPartyMember )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_ERROR_POPUP_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iErrorData1 )
		&& GET( m_iErrorData2 )
		//{{ 던전 입장권 에러 팝업 변경
		&& GET( m_bPartyMember )
		//}}
		;
}
//}}


//{{ 2010. 05. 20  최육사	스크립트 파싱
//}}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
//{{ 2010. 06. 17  최육사	오토핵 방지
SERIALIZE_DEFINE_PUT( KEGS_SERVER_CHECK_HACK_USER_REQ, obj, ks )
{
	return PUT( m_cHackingCheckType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SERVER_CHECK_HACK_USER_REQ, obj, ks )
{
	return GET( m_cHackingCheckType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SERVER_CHECK_HACK_USER_ACK, obj, ks )
{
	return PUT( m_cHackingCheckType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SERVER_CHECK_HACK_USER_ACK, obj, ks )
{
	return GET( m_cHackingCheckType )
		;
}
//}}
//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2010. 7. 21  최육사	펫 시스템

SERIALIZE_DEFINE_PUT( KEGS_CREATE_PET_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_wstrPetName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_PET_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_wstrPetName )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CREATE_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kCreatedPetInfo )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kCreatedPetInfo )
		&& GET( m_vecKInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GET_PET_LIST_REQ, obj, ks )
{
	return PUT( m_nViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_PET_LIST_REQ, obj, ks )
{
	return GET( m_nViewPage )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_PET_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_nTotalPage )
		&& PUT( m_nViewPage )
		&& PUT( m_vecPetList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_PET_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_nTotalPage )
		&& GET( m_nViewPage )
		&& GET( m_vecPetList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SUMMON_PET_REQ, obj, ks )
{
	return PUT( m_iSummonPetUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_PET_REQ, obj, ks )
{
	return GET( m_iSummonPetUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_SUMMON_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kSummonedPetInfo )
		&& PUT( m_vecPetInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kSummonedPetInfo )
		&& GET( m_vecPetInventorySlotInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_SUMMON_PET_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_vecPetInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_PET_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_vecPetInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_COMMANDS_FOR_PETS_REQ, obj, ks )
{
	return PUT( m_cActionType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COMMANDS_FOR_PETS_REQ, obj, ks )
{
	return GET( m_cActionType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PET_ACTION_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_cActionType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PET_ACTION_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_cActionType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGED_PET_SATIETY_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPetUID )
		&& PUT( m_sCurrentSatiety )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGED_PET_SATIETY_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPetUID )
		&& GET( m_sCurrentSatiety )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGED_PET_INTIMACY_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPetUID )
		&& PUT( m_iCurrentIntimacy )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGED_PET_INTIMACY_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPetUID )
		&& GET( m_iCurrentIntimacy )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_CHANGED_PET_SATIETY_INTIMACY_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPetUID )
		&& PUT( m_sCurrentSatiety )
		&& PUT( m_iCurrentIntimacy )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGED_PET_SATIETY_INTIMACY_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPetUID )
		&& GET( m_sCurrentSatiety )
		&& GET( m_iCurrentIntimacy )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_FEED_PETS_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FEED_PETS_REQ, obj, ks )
{
	return GET( m_iItemUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_FEED_PETS_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_sSatiety )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FEED_PETS_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_sSatiety )
		&& GET( m_vecInventorySlotInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_FIRST_FEED_PETS_NOT, obj, ks )
{
	return PUT( m_bAutoFeed )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FIRST_FEED_PETS_NOT, obj, ks )
{
	return GET( m_bAutoFeed )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_SET_AUTO_FEED_PETS_REQ, obj, ks )
{
	return PUT( m_bAutoFeed )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SET_AUTO_FEED_PETS_REQ, obj, ks )
{
	return GET( m_bAutoFeed )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_SET_AUTO_FEED_PETS_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bAutoFeed )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SET_AUTO_FEED_PETS_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bAutoFeed )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_AUTO_FEED_NOT, obj, ks )
{
	return PUT( m_iFeedItemID )
		&& PUT( m_vecPetInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_FEED_NOT, obj, ks )
{
	return GET( m_iFeedItemID )
		&& GET( m_vecPetInventorySlotInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PET_EVOLUTION_REQ, obj, ks )
{
	return PUT( m_iPetUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PET_EVOLUTION_REQ, obj, ks )
{
	return GET( m_iPetUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PET_EVOLUTION_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kEvolutionResultPetInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PET_EVOLUTION_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kEvolutionResultPetInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PET_EVOLUTION_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_kPetInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PET_EVOLUTION_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_kPetInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_PET_INFO_NOT, obj, ks )
{
	return PUT( m_iCheatType )
		&& PUT( m_iValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_PET_INFO_NOT, obj, ks )
{
	return GET( m_iCheatType )
		&& GET( m_iValue )
		;
}


//////////////////////////////////////////////////////////////////////////
//{{ 2012. 02. 22	박세훈	펫 이름 변경권
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PET_NAME_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_wstrPetName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PET_NAME_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_wstrPetName )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PET_NAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kChangedPetInfo )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PET_NAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kChangedPetInfo )
		&& GET( m_vecKInventorySlotInfo )
		;
}
//}}


//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 8. 16	최육사	기간 리셋 아이템 이벤트

SERIALIZE_DEFINE_PUT( KEGS_RESET_PERIOD_ITEM_NOT, obj, ks )
{
	return PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESET_PERIOD_ITEM_NOT, obj, ks )
{
	return GET( m_vecInventorySlotInfo )
		;
}

//}}



//{{ 2010. 9. 29	최육사	ED 모니터링 치트

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_SHOW_ED_CHEAT_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_SHOW_ED_CHEAT_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ADMIN_SHOW_ED_CHEAT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrNickName )
		&& PUT( m_iED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_SHOW_ED_CHEAT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrNickName )
		&& GET( m_iED )
		;
}

//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 10. 13	최육사	DLL Manager

SERIALIZE_DEFINE_PUT( KEGS_MODULE_INFO_UPDATE_NOT, obj, ks )
{
	return PUT( m_vecModuleName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MODULE_INFO_UPDATE_NOT, obj, ks )
{
	return GET( m_vecModuleName )
		;
}

//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2010. 11. 17	최육사	넥슨 빌링 청약 철회

SERIALIZE_DEFINE_PUT( KEGS_CASH_ITEM_REFUND_REQ, obj, ks )
{
	return PUT( m_ulOrderNo )
		&& PUT( m_ulProductNo )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CASH_ITEM_REFUND_REQ, obj, ks )
{
	return GET( m_ulOrderNo )
		&& GET( m_ulProductNo )
		&& GET( m_iQuantity )
		;
}

//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 01. 04  김민성	속성 부적

SERIALIZE_DEFINE_PUT( KEGS_ATTRIB_ATTACH_ITEM_REQ , obj, ks )
{
	return PUT( m_iDestItemUID )
		&& PUT( m_iAttachItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ATTRIB_ATTACH_ITEM_REQ , obj, ks )
{
	return GET( m_iDestItemUID )
		&& GET( m_iAttachItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ATTRIB_ATTACH_ITEM_ACK , obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ATTRIB_ATTACH_ITEM_ACK , obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}

//}}

//{{ 2011. 01. 06  김민성  스킬슬롯체인지 체크(인벤토리-기간제) 기능 구현
SERIALIZE_DEFINE_PUT( KEGS_SKILL_SLOT_CHANGE_ITEM_NOT , obj, ks )
{
	return PUT( m_cSkillSlotBExpirationState )
		&& PUT( m_wstrSkillSlotBEndDate )
		;
}
SERIALIZE_DEFINE_GET( KEGS_SKILL_SLOT_CHANGE_ITEM_NOT , obj, ks )
{
	return GET( m_cSkillSlotBExpirationState )
		&& GET( m_wstrSkillSlotBEndDate )
		;
}
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
SERIALIZE_DEFINE_PUT( KEGS_WORLD_MISSION_UPDATE_NOT, obj, ks )
{
	return PUT( m_bActive )
		&& PUT( m_iRemainTime )
		&& PUT( m_iWorldCrystalCount )
		//{{ 2013. 04. 15	박세훈	어둠의 문 개편
		&& PUT( m_iWorldMaxCrystalCount )
		//}}
		&& PUT( m_iMissionStartRemainTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WORLD_MISSION_UPDATE_NOT, obj, ks )
{
	return GET( m_bActive )
		&& GET( m_iRemainTime )
		&& GET( m_iWorldCrystalCount )
		//{{ 2013. 04. 15	박세훈	어둠의 문 개편
		&& GET( m_iWorldMaxCrystalCount )
		//}}
		&& GET( m_iMissionStartRemainTime )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_ATTRIB_NPC_NOT, obj, ks )
{
	return PUT( m_vecAttribNPCUnit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_ATTRIB_NPC_NOT, obj, ks )
{
	return GET( m_vecAttribNPCUnit )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_WORLD_BUFF_NOT, obj, ks )
{
	return PUT( m_usSuccessWorldBuff )
		&& PUT( m_usWorldBuffDurationTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_WORLD_BUFF_NOT, obj, ks )
{
	return GET( m_usSuccessWorldBuff )
		&& GET( m_usWorldBuffDurationTime )
		;
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_ROOM_ACK , obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iReason )
		&& PUT( m_iItemID )
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& PUT( m_bNotLeaveParty )
		&& PUT( m_kBattleFieldJoinInfo )
		//}}
		;
}
SERIALIZE_DEFINE_GET( KEGS_LEAVE_ROOM_ACK , obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iReason )
		&& GET( m_iItemID )
		//{{ 2011. 11. 1	최육사	배틀필드 시스템
		&& GET( m_bNotLeaveParty )
		&& GET( m_kBattleFieldJoinInfo )
		//}}
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2011. 04. 12	최육사	대리 상인

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PSHOP_AGENCY_EXPIRATION_NOT, obj, ks )
{
	return PUT( m_cShopType )
       && PUT( m_wstrAgencyExpirationDate )
		   ;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PSHOP_AGENCY_EXPIRATION_NOT, obj, ks )
{
	return GET( m_cShopType )
        && GET( m_wstrAgencyExpirationDate )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_JOIN_MY_PSHOP_AGENCY_REQ, obj, ks )
{
	return PUT( m_cJoinType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_MY_PSHOP_AGENCY_REQ, obj, ks )
{
	return GET( m_cJoinType )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_JOIN_MY_PSHOP_AGENCY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_cJoinType )
		&& PUT( m_wstrPersonalShopName )
		&& PUT( m_wstrNickName )
		&& PUT( m_vecSellItemInfo )
        && PUT( m_iUsedItemID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_MY_PSHOP_AGENCY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_cJoinType )
		&& GET( m_wstrPersonalShopName )
		&& GET( m_wstrNickName )
		&& GET( m_vecSellItemInfo )
        && GET( m_iUsedItemID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PSHOP_AGENCY_MESSAGE_NOT, obj, ks )
{
	return PUT( m_cMessageType )
		&& PUT( m_iHostUnitUID )
		&& PUT( m_iSellItemID )
		&& PUT( m_iSellQuantity )
		&& PUT( m_iSellED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PSHOP_AGENCY_MESSAGE_NOT, obj, ks )
{
	return GET( m_cMessageType )
		&& GET( m_iHostUnitUID )
		&& GET( m_iSellItemID )
		&& GET( m_iSellQuantity )
		&& GET( m_iSellED )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PICK_UP_FROM_PSHOP_AGENCY_REQ, obj, ks )
{
	return PUT( m_vecPickUpItemList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PICK_UP_FROM_PSHOP_AGENCY_REQ, obj, ks )
{
	return GET( m_vecPickUpItemList )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PICK_UP_FROM_PSHOP_AGENCY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_vecSellItemInfo )
		&& PUT( m_vecInventorySlotInfo )
        && PUT( m_bRemainSellItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PICK_UP_FROM_PSHOP_AGENCY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_vecSellItemInfo )
		&& GET( m_vecInventorySlotInfo )
        && GET( m_bRemainSellItem )
		;
}


//}}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_PSHOP_AGENCY_CHEAT_REQ, obj, ks )
{
	return PUT( m_sPeriod )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_PSHOP_AGENCY_CHEAT_REQ, obj, ks )
{
	return GET( m_sPeriod )
		;
}


//{{ 2011. 05. 02  김민성	2차 보안 시스템
SERIALIZE_DEFINE_PUT( KEGS_SECOND_SECURITY_INFO_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bUseSecondPW )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SECOND_SECURITY_INFO_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bUseSecondPW )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_AUTH_SECOND_SECURITY_REQ, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_wstrSecondPW )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTH_SECOND_SECURITY_REQ, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_wstrSecondPW )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_AUTH_SECOND_SECURITY_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iFailedCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTH_SECOND_SECURITY_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iFailedCount )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CREATE_SECOND_SECURITY_REQ, obj, ks )
{
	return PUT( m_wstrSecondPW )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_SECOND_SECURITY_REQ, obj, ks )
{
	return GET( m_wstrSecondPW )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DELETE_SECOND_SECURITY_VERIFY_REQ, obj, ks )
{
	return PUT( m_wstrSecondPW )
		&& PUT( m_bDelete )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DELETE_SECOND_SECURITY_VERIFY_REQ, obj, ks )
{
	return GET( m_wstrSecondPW )
		&& GET( m_bDelete )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_SECOND_SECURITY_PW_REQ, obj, ks )
{
	return PUT( m_wstrOldSecondPW )
		&& PUT( m_wstrNewSecondPW )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_SECOND_SECURITY_PW_REQ, obj, ks )
{
	return GET( m_wstrOldSecondPW )
		&& GET( m_wstrNewSecondPW )
		;
}
//}}

//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
SERIALIZE_DEFINE_PUT( KEGS_EAT_SPECIAL_FEED_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPetUID )
		&& PUT( m_sPercent )
		&& PUT( m_iCurrentIntimacy )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EAT_SPECIAL_FEED_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPetUID )
		&& GET( m_sPercent )
		&& GET( m_iCurrentIntimacy )
		;
}
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
SERIALIZE_DEFINE_PUT( KEGS_COME_BACK_BUFF_NOT, obj, ks )
{
	return PUT( m_bIsFinish )
		&& PUT( m_iComeBackBuffEnd )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COME_BACK_BUFF_NOT, obj, ks )
{
	return GET( m_bIsFinish )
		&& GET( m_iComeBackBuffEnd )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 06. 21	최육사	대전 개편

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PARTY_TYPE_NOT, obj, ks )
{
	return PUT( m_kPartyInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PARTY_TYPE_NOT, obj, ks )
{
	return GET( m_kPartyInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_PVP_PARTY_CHANGE_MATCH_INFO_REQ, obj, ks )
{
	return PUT( m_cPvpGameType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PVP_PARTY_CHANGE_MATCH_INFO_REQ, obj, ks )
{
	return GET( m_cPvpGameType )
		;
}


//SERIALIZE_DEFINE_PUT( KEGS_PVP_PARTY_SELECT_MY_MAP_REQ, obj, ks )
//{
//	return PUT( m_setSelectedPvpWorldID )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_PVP_PARTY_SELECT_MY_MAP_REQ, obj, ks )
//{
//	return GET( m_setSelectedPvpWorldID )
//		;
//}
//
//
//SERIALIZE_DEFINE_PUT( KEGS_PVP_PARTY_SELECT_MODE_REQ, obj, ks )
//{
//	return PUT( m_bCheckedPowerMode )
//		&& PUT( m_bCheckedItemMode )
//		;
//}
//
//SERIALIZE_DEFINE_GET( KEGS_PVP_PARTY_SELECT_MODE_REQ, obj, ks )
//{
//	return GET( m_bCheckedPowerMode )
//		&& GET( m_bCheckedItemMode )
//		;
//}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REQUEST_MATCH_MAKING_REQ, obj, ks )
{
	return PUT( m_cPvpPlayerCount )
		&& PUT( m_cPvpGameType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_MATCH_MAKING_REQ, obj, ks )
{
	return GET( m_cPvpPlayerCount )
		&& GET( m_cPvpGameType )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_REG_MATCH_WAIT_LIST_SUCCESS_NOT, obj, ks )
{
	return PUT( m_iEstimatedTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_MATCH_WAIT_LIST_SUCCESS_NOT, obj, ks )
{
	return GET( m_iEstimatedTime )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_MATCH_MAKING_SUCCESS_NOT, obj, ks )
{
	return PUT( m_iMatchUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MATCH_MAKING_SUCCESS_NOT, obj, ks )
{
	return GET( m_iMatchUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_MATCH_MAKING_SUCCESS_REPLY_NOT, obj, ks )
{
	return PUT( m_bGameStartAgree )
		&& PUT( m_bPvpPowerMode )
		&& PUT( m_bPvpItemMode )
		&& PUT( m_setSelectedPvpMap )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MATCH_MAKING_SUCCESS_REPLY_NOT, obj, ks )
{
	return GET( m_bGameStartAgree )
		&& GET( m_bPvpPowerMode )
		&& GET( m_bPvpItemMode )
		&& GET( m_setSelectedPvpMap )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UNREG_MATCH_WAIT_LIST_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_wstrCancelUnitNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNREG_MATCH_WAIT_LIST_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_wstrCancelUnitNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_MATCH_CLOSE_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_iCancelUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_MATCH_CLOSE_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_iCancelUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_START_PVP_MATCH_NOT, obj, ks )
{
	return PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		&& PUT( m_mapPvpNpcInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_START_PVP_MATCH_NOT, obj, ks )
{
	return GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		&& GET( m_mapPvpNpcInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PVP_NPC_UNIT_RE_BIRTH_POS_REQ, obj, ks )
{
	return PUT( m_iPvpNpcUnitUID )
		&& PUT( m_iStartPosIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PVP_NPC_UNIT_RE_BIRTH_POS_REQ, obj, ks )
{
	return GET( m_iPvpNpcUnitUID )
		&& GET( m_iStartPosIndex )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_PVP_NPC_UNIT_INFO_TO_SERVER_REQ, obj, ks )
{
	return PUT( m_mapPvpNpcInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_PVP_NPC_UNIT_INFO_TO_SERVER_REQ, obj, ks )
{
	return GET( m_mapPvpNpcInfo )
		;
}
//}}

//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2011. 08. 08	최육사	mac주소 블럭 기능
SERIALIZE_DEFINE_PUT( KEGS_CHECK_MACHINE_ID_REQ, obj, ks )
{
	return PUT( m_strMachineID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_MACHINE_ID_REQ, obj, ks )
{
	return GET( m_strMachineID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CHECK_MACHINE_ID_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_strMachineID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_MACHINE_ID_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_strMachineID )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2011. 08. 10    김민성    채널링 SOAP
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2011. 08. 11	최육사	투니 랜드 채널링
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANNELING_CODE_CHEAT_NOT, obj, ks )
{
	return PUT( m_iChannelingCode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANNELING_CODE_CHEAT_NOT, obj, ks )
{
	return GET( m_iChannelingCode )
		;
}
//}}

//{{ 2011. 08. 12   김민성      헤니르 개편
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_HENIR_REWARD_CHEAT_NOT, obj, ks )
{
	return PUT( m_iHenirRewardNormalCnt )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_HENIR_REWARD_CHEAT_NOT, obj, ks )
{
	return GET( m_iHenirRewardNormalCnt )
		;
}
//}}

//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_RANDOM_QUEST_NOT, obj, ks )
{
	return PUT( m_mapRandomQuestList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_RANDOM_QUEST_NOT, obj, ks )
{
	return GET( m_mapRandomQuestList )
		;
}
//}}

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_ED_MONITORING_CHEAT_NOT, obj, ks )
{
	return PUT( m_bEDMonitoringMode )
		&& PUT( m_iMinED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_ED_MONITORING_CHEAT_NOT, obj, ks )
{
	return GET( m_bEDMonitoringMode )
		&& GET( m_iMinED )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ED_MONITORING_BLOCK_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_ucType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ED_MONITORING_BLOCK_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_ucType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ED_MONITORING_BLOCK_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ED_MONITORING_BLOCK_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COMMUNITY_USER_LIST_MONITORING_MODE_REQ, obj, ks )
{
	return PUT( m_uiViewPage )
		&& PUT( m_vecUnitUIDList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COMMUNITY_USER_LIST_MONITORING_MODE_REQ, obj, ks )
{
	return GET( m_uiViewPage )
		&& GET( m_vecUnitUIDList )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 10. 13	최육사	컨텐츠 관리자
SERIALIZE_DEFINE_PUT( KEGS_CASH_SHOP_OPEN_NOT, obj, ks )
{
	return PUT( m_bCashShopOpen )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CASH_SHOP_OPEN_NOT, obj, ks )
{
	return GET( m_bCashShopOpen )
		;
}
//}}

//{{ 2011. 09. 29  김민성	셧다운제 도입
//}}

//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
SERIALIZE_DEFINE_PUT( KEGS_BUY_UNIT_CLASS_CHANGE_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iNewUnitClass )
		&& PUT( m_vecSkillUnsealed )

		&& PUT( m_mapChangeMemo )
		&& PUT( m_vecChangeItem )
		&& PUT( m_vecChangeCompleteQuest )
		&& PUT( m_vecChangeInProgressQuest )
		&& PUT( m_kUnitInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BUY_UNIT_CLASS_CHANGE_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iNewUnitClass )
		&& GET( m_vecSkillUnsealed )

		&& GET( m_mapChangeMemo )

		&& GET( m_vecChangeItem )
		&& GET( m_vecChangeCompleteQuest )
		&& GET( m_vecChangeInProgressQuest )
		&& GET( m_kUnitInfo )
		;
}
//}}


SERIALIZE_DEFINE_PUT( KEGS_ADMIN_NPC_UNIT_CREATE_REQ, obj, ks )
{
	return PUT( m_kNPCUnitReq )
		&& PUT( m_cAttrib1 )
		&& PUT( m_cAttrib2 )
		&& PUT( m_cSA1 )
		&& PUT( m_cSA2 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_NPC_UNIT_CREATE_REQ, obj, ks )
{
	return GET( m_kNPCUnitReq )
		&& GET( m_cAttrib1 )
		&& GET( m_cAttrib2 )
		&& GET( m_cSA1 )
		&& GET( m_cSA2 )
		;
};
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_NPC_UNIT_CREATE_NOT, obj, ks )
{
	return PUT( m_kNPCUnitNot )
		&& PUT( m_cAttrib1 )
		&& PUT( m_cAttrib2 )
		&& PUT( m_cSA1 )
		&& PUT( m_cSA2 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_NPC_UNIT_CREATE_NOT, obj, ks )
{
	return GET( m_kNPCUnitNot )
		&& GET( m_cAttrib1 )
		&& GET( m_cAttrib2 )
		&& GET( m_cSA1 )
		&& GET( m_cSA2 )
		;
};

//{{ 2012. 02. 14    김민성   PVP 대전 Item 생성 위치 변경
SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ITEM_POS_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iPos )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ITEM_POS_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iPos )
		;
};
//}}

//{{ 2012. 02. 21	김민성	인벤토리 정렬
SERIALIZE_DEFINE_PUT( KEGS_SORT_CATEGORY_ITEM_REQ, obj, ks )
{
	return PUT( m_iCategoryType );
}

SERIALIZE_DEFINE_GET( KEGS_SORT_CATEGORY_ITEM_REQ, obj, ks )
{
	return GET( m_iCategoryType );
}

SERIALIZE_DEFINE_PUT( KEGS_SORT_CATEGORY_ITEM_ACK, obj, ks )
{
	return PUT( m_iCategoryType )
		&& PUT( m_vecUpdatedInventorySlot )
		&& PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SORT_CATEGORY_ITEM_ACK, obj, ks )
{
	return GET( m_iCategoryType )
		&& GET( m_vecUpdatedInventorySlot )
		&& GET( m_iOK )
		;
}
//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 10. 27	최육사	배틀필드 시스템
SERIALIZE_DEFINE_PUT( KEGS_JOIN_BATTLE_FIELD_REQ, obj, ks )
{
    return PUT( m_kBattleFieldJoinInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_BATTLE_FIELD_REQ, obj, ks )
{
    return GET( m_kBattleFieldJoinInfo )
        ;
}


SERIALIZE_DEFINE_PUT( KEGS_JOIN_BATTLE_FIELD_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kBattleFieldJoinInfo )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_wstrCNIP )
		&& PUT( m_iRequireLevel )
		&& PUT( m_iRequireDungeonID )
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		//}
		//{{ 2012. 11. 21	최육사		난입자에게 캐쉬샵 유저 정보 보내기
		&& PUT( m_vecEnterCashShopUser )
		//}}
		&& PUT( m_tRemainFieldHoldingTime )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_BATTLE_FIELD_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kBattleFieldJoinInfo )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_wstrCNIP )
		&& GET( m_iRequireLevel )
		&& GET( m_iRequireDungeonID )
		//{{ 핑 상태체크 호스트 변경 - 김민성
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		//}
		//{{ 2012. 11. 21	최육사		난입자에게 캐쉬샵 유저 정보 보내기
		&& GET( m_vecEnterCashShopUser )
		//}}
		&& GET( m_tRemainFieldHoldingTime )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_LEAVE_BATTLE_FIELD_NOT, obj, ks )
{
	return PUT( m_cRoomState )
		&& PUT( m_iUnitUID )
		&& PUT( m_iPartyUID )
		&& PUT( m_iReason )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_BATTLE_FIELD_NOT, obj, ks )
{
	return GET( m_cRoomState )
		&& GET( m_iUnitUID )
		&& GET( m_iPartyUID )
		&& GET( m_iReason )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_JOIN_USER_LOAD_COMPLETE_NOT, obj, ks )
{
	return PUT( m_iLoadedUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_JOIN_USER_LOAD_COMPLETE_NOT, obj, ks )
{
	return GET( m_iLoadedUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_NPC_LOAD_NOT, obj, ks )
{
	return PUT( m_vecNPCData )
		&& PUT( m_mapAttribNpcInfo )
		&& PUT( m_iBattleFieldID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_NPC_LOAD_NOT, obj, ks )
{
	return GET( m_vecNPCData )
		&& GET( m_mapAttribNpcInfo )
		&& GET( m_iBattleFieldID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_NPC_P2P_SYNC_NOT, obj, ks )
{
	return PUT( m_vecNonNpcSyncUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_NPC_P2P_SYNC_NOT, obj, ks )
{
	return GET( m_vecNonNpcSyncUserList )
		;
}

//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_EVENT_MESSAGE_NOT, obj, ks )
{
	return PUT( m_cEventType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_EVENT_MESSAGE_NOT, obj, ks )
{
	return GET( m_cEventType )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_BATTLE_FIELD_USER_POS_NOT, obj, ks )
{
	return PUT( m_kLastPos )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_BATTLE_FIELD_USER_POS_NOT, obj, ks )
{
	return GET( m_kLastPos )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_QUICK_START_DUNGEON_GAME_REQ, obj, ks )
{
	return PUT( m_iDungeonID )
		&& PUT( m_DifficultyLevel )
		&& PUT( m_cGetItemType )
		&& PUT( m_cDungeonMode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUICK_START_DUNGEON_GAME_REQ, obj, ks )
{
	return GET( m_iDungeonID )
		&& GET( m_DifficultyLevel )
		&& GET( m_cGetItemType )
		&& GET( m_cDungeonMode )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_QUICK_START_DUNGEON_GAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iFailUnitUID )
		&& PUT( m_wstrFailUserNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_QUICK_START_DUNGEON_GAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iFailUnitUID )
		&& GET( m_wstrFailUserNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_AUTO_PARTY_DUNGEON_GAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrFailUserNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PARTY_DUNGEON_GAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrFailUserNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REG_AUTO_PARTY_WAIT_LIST_SUCCESS_NOT, obj, ks )
{
	return PUT( m_iWaitNumber )
		&& PUT( m_iEstimatedTime )
		&& PUT( m_bRemakingAutoParty )
		&& PUT( m_bAutoPartyBonus )
		//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
		&& PUT( m_iAutoPartyWaitUserCount )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_REG_AUTO_PARTY_WAIT_LIST_SUCCESS_NOT, obj, ks )
{
	return GET( m_iWaitNumber )
		&& GET( m_iEstimatedTime )
		&& GET( m_bRemakingAutoParty )
		&& GET( m_bAutoPartyBonus )
		//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
		&& GET( m_iAutoPartyWaitUserCount )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_AUTO_PARTY_MAKING_SUCCESS_NOT, obj, ks )
{
	return PUT( m_iAutoPartyUID )
		&& PUT( m_iMemberCount )
		&& PUT( m_iDungeonID )
		&& PUT( m_cDifficultyLevel )
		//{{ 2012. 12. 17	박세훈	수동 파티 시 던전 팝업 창 문구 오류 수정
		&& PUT( m_cDungeonMode )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PARTY_MAKING_SUCCESS_NOT, obj, ks )
{
	return GET( m_iAutoPartyUID )
		&& GET( m_iMemberCount )
		&& GET( m_iDungeonID )
		&& GET( m_cDifficultyLevel )
		//{{ 2012. 12. 17	박세훈	수동 파티 시 던전 팝업 창 문구 오류 수정
		&& GET( m_cDungeonMode )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_AUTO_PARTY_MAKING_SUCCESS_REPLY_NOT, obj, ks )
{
	return PUT( m_bGameStartAgree )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PARTY_MAKING_SUCCESS_REPLY_NOT, obj, ks )
{
	return GET( m_bGameStartAgree )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_AUTO_PARTY_CLOSE_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_iCancelUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PARTY_CLOSE_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_iCancelUnitUID )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UNREG_AUTO_PARTY_WAIT_LIST_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_wstrCancelUnitNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNREG_AUTO_PARTY_WAIT_LIST_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_wstrCancelUnitNickName )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REGROUP_PARTY_NOT, obj, ks )
{
	return PUT( m_kPartyInfo )
		&& PUT( m_vecPartyUserInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REGROUP_PARTY_NOT, obj, ks )
{
	return GET( m_kPartyInfo )
		&& GET( m_vecPartyUserInfo )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_REQUEST_GET_AUTO_PARTY_BONUS_INFO_NOT, obj, ks )
{
	return PUT( m_iVillageMapID )
		&& PUT( m_vecDungeonList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_REQUEST_GET_AUTO_PARTY_BONUS_INFO_NOT, obj, ks )
{
	return GET( m_iVillageMapID )
		&& GET( m_vecDungeonList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_AUTO_PARTY_BONUS_INFO_NOT, obj, ks )
{
	return PUT( m_mapDungeonBonusList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_AUTO_PARTY_BONUS_INFO_NOT, obj, ks )
{
	return GET( m_mapDungeonBonusList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_PLAY_STATUS_NOT, obj, ks )
{
	return PUT( m_kGamePlayStatus )
		//{{ 2012. 11. 20	박세훈	Field Frame 로그 추가
		&& PUT( m_iFrame )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_PLAY_STATUS_NOT, obj, ks )
{
	return GET( m_kGamePlayStatus )
		//{{ 2012. 11. 20	박세훈	Field Frame 로그 추가
		&& GET( m_iFrame )
		//}}
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_MOVE_TO_PARTY_MEMBER_INFO_NOT, obj, ks )
{
	return PUT( m_bIsOn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_MOVE_TO_PARTY_MEMBER_INFO_NOT, obj, ks )
{
	return GET( m_bIsOn )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_PARTY_MEMBER_STATUS_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_kStatus )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_PARTY_MEMBER_STATUS_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_kStatus )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_GAME_START_TIME_COUNT_NOT, obj, ks )
{
	return PUT( m_iRoomUID )
		&& PUT( m_cSecondCount )
		&& PUT( m_bIsDungeonRoom )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GAME_START_TIME_COUNT_NOT, obj, ks )
{
	return GET( m_iRoomUID )
		&& GET( m_cSecondCount )
		&& GET( m_bIsDungeonRoom )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_USE_SKILL_BUFF_IN_PLAY_NOT, obj, ks )
{
	return PUT( m_iSkillID )
		&& PUT( m_bOn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_SKILL_BUFF_IN_PLAY_NOT, obj, ks )
{
	return GET( m_iSkillID )
		&& GET( m_bOn )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iSkillID )
		&& PUT( m_bOn )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iSkillID )
		&& GET( m_bOn )
		;
}


//}}

//{{ 버프 관리 시스템 - 김민성
//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_BUFF_INFO_NOT, obj, ks )
{
	return PUT( m_vecActivateBuffList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_BUFF_INFO_NOT, obj, ks )
{
	return GET( m_vecActivateBuffList )
		;
}


//////////////////////////////////////////////////////////////////////////
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_BUFF_INFO_IN_ROOM_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_vecActivateBuffList )
		&& PUT( m_kGameStat )
		//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_BUFF_INFO_IN_ROOM_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_vecActivateBuffList )
		&& GET( m_kGameStat )
		//{{ 2012. 12. 18	최육사	아라 파티 플레이 보너스 경험치
		//}}
		;
}
//}

//{{ 2012. 06. 13	김민성       선택적 셧다운제
//}}



//////////////////////////////////////////////////////////////////////////
//{{ 2012. 07. 02	최육사		대기열 시스템
SERIALIZE_DEFINE_PUT( KECH_WAIT_QUEUE_MESSAGE_NOT, obj, ks )
{
	return PUT( m_iWaitUserCount )
		&& PUT( m_bPassed )
		;
}

SERIALIZE_DEFINE_GET( KECH_WAIT_QUEUE_MESSAGE_NOT, obj, ks )
{
	return GET( m_iWaitUserCount )
		&& GET( m_bPassed )
		;
}
//}}
//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
SERIALIZE_DEFINE_PUT( KEGS_KEYBOARD_MAPPING_INFO_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kKeyboardMappingInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_KEYBOARD_MAPPING_INFO_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kKeyboardMappingInfo )
		;
}
//}}

//{{ 2012. 05. 25	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
SERIALIZE_DEFINE_PUT( KEGS_CHAT_OPTION_INFO_WRITE_REQ, obj, ks )
{
	return PUT( m_cIndex )
		&& PUT( m_bValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHAT_OPTION_INFO_WRITE_REQ, obj, ks )
{
	return GET( m_cIndex )
		&& GET( m_bValue )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHAT_OPTION_INFO_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_mapChatOptionSetInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHAT_OPTION_INFO_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_mapChatOptionSetInfo )
		;
}
//}}


//////////////////////////////////////////////////////////////////////////
//{{ 버프 관리 시스템 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_UPDATE_USER_UNIT_BUFF_INFO_NOT, obj, ks )
{
	return PUT( m_iToUnitUID )
		&& PUT( m_vecBuffFactor )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_USER_UNIT_BUFF_INFO_NOT, obj, ks )
{
	return GET( m_iToUnitUID )
		&& GET( m_vecBuffFactor )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_vecBuffFactor )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_vecBuffFactor )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_UPDATE_NPC_UNIT_BUFF_INFO_NOT, obj, ks )
{

	return
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        PUT( m_vecToUnitUID )
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//        PUT( m_iToUnitUID )
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
		&& PUT( m_vecNpcUnitBuff )
		;
}


SERIALIZE_DEFINE_GET( KEGS_UPDATE_NPC_UNIT_BUFF_INFO_NOT, obj, ks )
{
	return
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        GET( m_vecToUnitUID )
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//        GET( m_iToUnitUID )
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
		&& GET( m_vecNpcUnitBuff )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT, obj, ks )
{

	return PUT( m_vecNpcUnitBuff )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT, obj, ks )
{
	return GET( m_vecNpcUnitBuff )
		;
}



//}}


//////////////////////////////////////////////////////////////////////////
//{{ 2012. 05. 30	최육사	그노시스 기간 치트
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ, obj, ks )
{
	return PUT( m_iYear )
		&& PUT( m_iMonth )
		&& PUT( m_iDay )
		&& PUT( m_iHour )
		&& PUT( m_iMinute )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ, obj, ks )
{
	return GET( m_iYear )
		&& GET( m_iMonth )
		&& GET( m_iDay )
		&& GET( m_iHour )
		&& GET( m_iMinute )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrCspEndDate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrCspEndDate )
		;
}
//}}

//{{ 2012. 09. 19   김민성   캐시샵 방문 상태
SERIALIZE_DEFINE_PUT( KEGS_VISIT_CASH_SHOP_NOT, obj, ks )
{
	return PUT( m_bEnterCashShop )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_VISIT_CASH_SHOP_NOT, obj, ks )
{
	return GET( m_bEnterCashShop )
		&& GET( m_iUnitUID )
		;
}
//}}

//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
//}}

//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//}}

//{{ 2012. 09. 21	박세훈	빙고 이벤트
//}}

//{{ 배틀필드 위험도 확인 치트 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT, obj, ks )
{
	return PUT( m_iDangerousValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT, obj, ks )
{
	return GET( m_iDangerousValue )
		;
}
//}}

//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
SERIALIZE_DEFINE_PUT( KEGS_FORCE_RELAY_NOT, obj, ks )
{
	return PUT( m_iTargetUnitUID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_FORCE_RELAY_NOT, obj, ks )
{
	return GET( m_iTargetUnitUID )
		;
}

//}}

//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
//}}

//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
SERIALIZE_DEFINE_PUT( KEGS_AUTO_PARTY_WAIT_USER_COUNT_NOT, obj, ks )
{
	return PUT( m_iAutoPartyWaitUserCount )
		;
}

SERIALIZE_DEFINE_GET( KEGS_AUTO_PARTY_WAIT_USER_COUNT_NOT, obj, ks )
{
	return GET( m_iAutoPartyWaitUserCount )
		;
}
//}}

//{{ 핑 상태체크 호스트 변경 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_UDP_PING_SCORE_NOT, obj, ks )
{
	return PUT( m_dwPingScore )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UDP_PING_SCORE_NOT, obj, ks )
{
	return GET( m_dwPingScore )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHECK_PING_SCORE_CHANGE_HOST_NOT, obj, ks )
{
	return PUT( m_iOldHostUID )
		&& PUT( m_iNewHostUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_PING_SCORE_CHANGE_HOST_NOT, obj, ks )
{
	return GET( m_iOldHostUID )
		&& GET( m_iNewHostUID )
		;
}
//}

//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_OBSERVER_MODE_REQ, obj, ks )
{
	return PUT( m_bObserverMode )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_OBSERVER_MODE_REQ, obj, ks )
{
	return GET( m_bObserverMode )
		;
}
//}}

//{{ 2012. 11. 21	박세훈	영웅 대전 참가자 관리용 리스트
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_HERO_PVP_USER_LIST_ACK, obj, ks )
{
	return PUT( m_vecUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_HERO_PVP_USER_LIST_ACK, obj, ks )
{
	return GET( m_vecUserList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_HERO_PVP_USER_ADD_REQ, obj, ks )
{
	return PUT( m_vecUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_HERO_PVP_USER_ADD_REQ, obj, ks )
{
	return GET( m_vecUserList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_wstrNickName )
		&& PUT( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_wstrNickName )
		&& GET( m_wstrMessage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_HERO_PVP_USER_CONNECTION_NOT, obj, ks )
{
	return PUT( m_vecUserList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_HERO_PVP_USER_CONNECTION_NOT, obj, ks )
{
	return GET( m_vecUserList )
		;
}
//}}

//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT, obj, ks )
{
	return PUT( m_iStageID )
		&& PUT( m_iSubStageID )
		&& PUT( m_iComboScore )
		&& PUT( m_iTechScore )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT, obj, ks )
{
	return GET( m_iStageID )
		&& GET( m_iSubStageID )
		&& GET( m_iComboScore )
		&& GET( m_iTechScore )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BAD_ATTITUDE_USER_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BAD_ATTITUDE_USER_NOT, obj, ks )
{
	return GET( m_iUserUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_FORCED_EXIT_VOTE_REQ, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_bWantForceExit )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FORCED_EXIT_VOTE_REQ, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_bWantForceExit )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_FORCED_EXIT_VOTE_ACK, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FORCED_EXIT_VOTE_ACK, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BAD_ATTITUDE_USER_MSG_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_MsgType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BAD_ATTITUDE_USER_MSG_NOT, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_MsgType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DEFENCE_DUNGEON_WAVE_ID_NOT, obj, ks )
{
	return PUT( m_iWaveID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DEFENCE_DUNGEON_WAVE_ID_NOT, obj, ks )
{
	return GET( m_iWaveID )
		;
}
//}

//{{ 2013. 02. 01  이벤트 발렌타인 던전 - 김민성
//}

//{{ 2013. 03. 12	최육사	몬스터 유효 검사
SERIALIZE_DEFINE_PUT( KEGS_CHECK_EXIST_MONSTER_UID_REQ, obj, ks )
{
	return PUT( m_vecNpcUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_EXIST_MONSTER_UID_REQ, obj, ks )
{
	return GET( m_vecNpcUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CHECK_EXIST_MONSTER_UID_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecNotExistNpcUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_EXIST_MONSTER_UID_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecNotExistNpcUID )
		;
}
//}}

//{{ 2013. 02. 15   필드 중간 보스 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_NPC_UNIT_CREATE_MIDDLE_BOSS_NOT, obj, ks )
{
	return PUT( m_kCreatePacket )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NPC_UNIT_CREATE_MIDDLE_BOSS_NOT, obj, ks )
{
	return GET( m_kCreatePacket )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_REQ, obj, ks )
{
	return PUT( m_iDanger )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_REQ, obj, ks )
{
	return GET( m_iDanger )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iDanger )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iDanger )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SYNC_BATTLE_FIELD_DAMAGED_MAP_REQ, obj, ks )
{
	return PUT( m_iNPCUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SYNC_BATTLE_FIELD_DAMAGED_MAP_REQ, obj, ks )
{
	return GET( m_iNPCUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_NPC_DAMAGED_MAP_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iNPCUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_NPC_DAMAGED_MAP_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iNPCUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_NPC_DAMAGED_MAP_ACK, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iNPCUID )
		&& PUT( m_mapDamagedMap )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_NPC_DAMAGED_MAP_ACK, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iNPCUID )
		&& GET( m_mapDamagedMap )
		;
}
//}

//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
//}}



//{{ 2013. 04. 05	최육사	어둠의 문 개편
SERIALIZE_DEFINE_PUT( KEGS_DECISION_ENTER_DEFENCE_DUNGEON_NOT, obj, ks )
{
	return PUT( m_DieBossPos )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DECISION_ENTER_DEFENCE_DUNGEON_NOT, obj, ks )
{
	return GET( m_DieBossPos )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_DEFENCE_DUNGEON_GAME_START_NOT, obj, ks )
{
	return PUT( m_sWorldID )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_vecObserverSlot )
		&& PUT( m_wstrCNIP )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DEFENCE_DUNGEON_GAME_START_NOT, obj, ks )
{
	return GET( m_sWorldID )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_vecObserverSlot )
		&& GET( m_wstrCNIP )
		&& GET( m_vecInventorySlotInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_kGameStat )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_kGameStat )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_REQ, obj, ks )
{
	return PUT( m_sSlotID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_REQ, obj, ks )
{
	return GET( m_sSlotID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_InventorySlotInfo )
		&& PUT( m_ItemID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_DEFENCE_DUNGEON_QUICK_SLOT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_InventorySlotInfo )
		&& GET( m_ItemID )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_INVENTORY_SLOT_INFO_NOT, obj, ks )
{
	return PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_INVENTORY_SLOT_INFO_NOT, obj, ks )
{
	return GET( m_vecInventorySlotInfo )
		;
}



//{{ 2013. 04. 01	 인연 시스템 - 김민성
SERIALIZE_DEFINE_PUT( KEGS_COUPLE_PROPOSE_REQ, obj, ks )
{
	return  PUT( m_wstrUnitName )
		 ;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_PROPOSE_REQ, obj, ks )
{
	return  GET( m_wstrUnitName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COUPLE_PROPOSE_ACK, obj, ks )
{
	return  PUT( m_iOK )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_PROPOSE_ACK, obj, ks )
{
	return  GET( m_iOK )
		 && GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COUPLE_PROPOSE_NOT, obj, ks )
{
	return  PUT( m_wstrRequestUnitName )
		 && PUT( m_iRequestUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_PROPOSE_NOT, obj, ks )
{
	return  GET( m_wstrRequestUnitName )
		 && GET( m_iRequestUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COUPLE_PROPOSE_AGREE_NOT, obj, ks )
{
	return  PUT( m_cAnswer )
		 && PUT( m_iRequestUnitUID )
		 && PUT( m_wstrRequestUnitName )
		 && PUT( m_iAcceptUserUID )
		 && PUT( m_ucAcceptUnitLevel )
		 && PUT( m_cAcceptUnitClass )
		 && PUT( m_iAcceptUnitUID )
		 && PUT( m_wstrAcceptUnitName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_PROPOSE_AGREE_NOT, obj, ks )
{
	return  GET( m_cAnswer )
		 && GET( m_iRequestUnitUID )
		 && GET( m_wstrRequestUnitName )
		 && GET( m_iAcceptUserUID )
		 && GET( m_ucAcceptUnitLevel )
		 && GET( m_cAcceptUnitClass )
		 && GET( m_iAcceptUnitUID )
		 && GET( m_wstrAcceptUnitName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COUPLE_PROPOSE_RESULT_NOT, obj, ks )
{
	return  PUT( m_iOK )
		 && PUT( m_cResult )
		 && PUT( m_iUnitUID )
		 && PUT( m_wstrUnitName )
		 && PUT( m_tDate )
		 && PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_PROPOSE_RESULT_NOT, obj, ks )
{
	return  GET( m_iOK )
		 && GET( m_cResult )
		 && GET( m_iUnitUID )
		 && GET( m_wstrUnitName )
		 && GET( m_tDate )
		 && GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_COUPLE_MAKING_SUCCESS_NOT, obj, ks )
{
	return PUT( m_wstrRequestUnitName )
		&& PUT( m_wstrAcceptUnitName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_COUPLE_MAKING_SUCCESS_NOT, obj, ks )
{
	return GET( m_wstrRequestUnitName )
		&& GET( m_wstrAcceptUnitName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELATIONSHIP_INFO_REQ, obj, ks )
{
	return PUT( m_iAcceptUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELATIONSHIP_INFO_REQ, obj, ks )
{
	return GET( m_iAcceptUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELATIONSHIP_INFO_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRequestGSUID )
		&& PUT( m_iRequestUnitUID )
		&& PUT( m_iAcceptUnitUID )
		&& PUT( m_ucMyLevel )
		&& PUT( m_cMyUnitClass )
		&& PUT( m_iMyMapID )
		&& PUT( m_cMyPosition )
		&& PUT( m_wstrMyChannelName )
		&& PUT( m_mapOtherUnitEquippedItem )
		&& PUT( m_ucLevel )
		&& PUT( m_cUnitClass )
		&& PUT( m_iMapID )
		&& PUT( m_tLastLogOutTime )
		&& PUT( m_bIsConnect )
		&& PUT( m_wstrChannelName )
		&& PUT( m_cPosition )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELATIONSHIP_INFO_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRequestGSUID )
		&& GET( m_iRequestUnitUID )
		&& GET( m_iAcceptUnitUID )
		&& GET( m_ucMyLevel )
		&& GET( m_cMyUnitClass )
		&& GET( m_iMyMapID )
		&& GET( m_cMyPosition )
		&& GET( m_wstrMyChannelName )
		&& GET( m_mapOtherUnitEquippedItem )
		&& GET( m_ucLevel )
		&& GET( m_cUnitClass )
		&& GET( m_iMapID )
		&& GET( m_tLastLogOutTime )
		&& GET( m_bIsConnect )
		&& GET( m_wstrChannelName )
		&& GET( m_cPosition )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELATIONSHIP_EFFECT_NOT, obj, ks )
{
	return PUT( m_iManUID )
		&& PUT( m_iGirlUID )
		&& PUT( m_iEffectIndex )
		&& PUT( m_wstrLoveWord )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELATIONSHIP_EFFECT_NOT, obj, ks )
{
	return GET( m_iManUID )
		&& GET( m_iGirlUID )
		&& GET( m_iEffectIndex )
		&& GET( m_wstrLoveWord )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_WEDDING_PROPOSE_REQ, obj, ks )
{
	return PUT( m_cWeddingHallType )
		&& PUT( m_cOfficiantNPC )
		&& PUT( m_tWeddingDate )
		&& PUT( m_wstrWeddingMsg )
		&& PUT( m_vecInviteUnitList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WEDDING_PROPOSE_REQ, obj, ks )
{
	return GET( m_cWeddingHallType )
		&& GET( m_cOfficiantNPC )
		&& GET( m_tWeddingDate )
		&& GET( m_wstrWeddingMsg )
		&& GET( m_vecInviteUnitList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_WEDDING_PROPOSE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WEDDING_PROPOSE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHECK_NICK_NAME_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_NICK_NAME_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHECK_NICK_NAME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_NICK_NAME_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_WEDDING_ITEM_INFO_REQ, obj, ks )
{
	return PUT( m_vecWeddingItemList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WEDDING_ITEM_INFO_REQ, obj, ks )
{
	return GET( m_vecWeddingItemList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_WEDDING_ITEM_INFO_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_mapWeddingItemInfo )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_WEDDING_ITEM_INFO_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_mapWeddingItemInfo )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_REQ, obj, ks )
{
	return PUT( m_iMapID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_REQ, obj, ks )
{
	return GET( m_iMapID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_VILLAGE_NOT, obj, ks )
{
	return PUT( m_iMapID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_iCalledUserUID )
		&& PUT( m_iCalledGSUID )
		&& PUT( m_cCallType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_VILLAGE_NOT, obj, ks )
{
	return GET( m_iMapID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_iCalledUserUID )
		&& GET( m_iCalledGSUID )
		&& GET( m_cCallType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_AGREE_VILLAGE_NOT, obj, ks )
{
	return PUT( m_bAgree )
		&& PUT( m_iMapID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_iCalledUserUID )
		&& PUT( m_iCalledGSUID )
		&& PUT( m_cCallType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_AGREE_VILLAGE_NOT, obj, ks )
{
	return GET( m_bAgree )
		&& GET( m_iMapID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_iCalledUserUID )
		&& GET( m_iCalledGSUID )
		&& GET( m_cCallType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_STATE_CHANGE_FIELD_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iMapID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_iLimitLevel )
		&& PUT( m_iRequireDungeonID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_STATE_CHANGE_FIELD_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iMapID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_iLimitLevel )
		&& GET( m_iRequireDungeonID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_ROOM_NOT, obj, ks )
{
	return PUT( m_iBattleFieldID )
		&& PUT( m_RoomUID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_iCalledUserUID )
		&& PUT( m_iCalledGSUID )
		&& PUT( m_cCallType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_ROOM_NOT, obj, ks )
{
	return GET( m_iBattleFieldID )
		&& GET( m_RoomUID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_iCalledUserUID )
		&& GET( m_iCalledGSUID )
		&& GET( m_cCallType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_AGREE_ROOM_NOT, obj, ks )
{
	return PUT( m_bAgree )
		&& PUT( m_iBattleFieldID )
		&& PUT( m_RoomUID )
		&& PUT( m_vPos )
		&& PUT( m_LastTouchLineIndex )
		&& PUT( m_iCalledUserUID )
		&& PUT( m_iCalledGSUID )
		&& PUT( m_cCallType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_AGREE_ROOM_NOT, obj, ks )
{
	return GET( m_bAgree )
		&& GET( m_iBattleFieldID )
		&& GET( m_RoomUID )
		&& GET( m_vPos )
		&& GET( m_LastTouchLineIndex )
		&& GET( m_iCalledUserUID )
		&& GET( m_iCalledGSUID )
		&& GET( m_cCallType )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CALL_MY_LOVER_JOIN_BATTLE_FIELD_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kBattleFieldJoinInfo )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_wstrCNIP )
		&& PUT( m_vPos )
		&& PUT( m_iRequireLevel )
		&& PUT( m_iRequireDungeonID )
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		&& PUT( m_vecEnterCashShopUser )
		&& PUT( m_iCalledUserUID )
		&& PUT( m_iCalledGSUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CALL_MY_LOVER_JOIN_BATTLE_FIELD_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kBattleFieldJoinInfo )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_wstrCNIP )
		&& GET( m_vPos )
		&& GET( m_iRequireLevel )
		&& GET( m_iRequireDungeonID )
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		&& GET( m_vecEnterCashShopUser )
		&& GET( m_iCalledUserUID )
		&& GET( m_iCalledGSUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_JOIN_WEDDING_HALL_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iWeddingUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_WEDDING_HALL_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iWeddingUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_JOIN_WEDDING_HALL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kWeddingInfo )
		&& PUT( m_RoomInfo )
		&& PUT( m_vecSlot )
		&& PUT( m_wstrCNIP )
		&& PUT( m_wstrUDPRelayIP )
		&& PUT( m_usUDPRelayPort )
		;
}

SERIALIZE_DEFINE_GET( KEGS_JOIN_WEDDING_HALL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kWeddingInfo )
		&& GET( m_RoomInfo )
		&& GET( m_vecSlot )
		&& GET( m_wstrCNIP )
		&& GET( m_wstrUDPRelayIP )
		&& GET( m_usUDPRelayPort )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_LEAVE_WEDDING_HALL_NOT, obj, ks )
{
	return PUT( m_iReason )
		&& PUT( m_cRoomState )
		&& PUT( m_iUnitUID )
		&& PUT( m_vecSlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_LEAVE_WEDDING_HALL_NOT, obj, ks )
{
	return GET( m_iReason )
		&& GET( m_cRoomState )
		&& GET( m_iUnitUID )
		&& GET( m_vecSlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHECK_BREAK_UP_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iCost )
		&& PUT( m_cRelationshipType )
		&& PUT( m_wstrLoverNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHECK_BREAK_UP_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iCost )
		&& GET( m_cRelationshipType )
		&& GET( m_wstrLoverNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BREAK_UP_NOT, obj, ks )
{
	return PUT( m_iCost )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BREAK_UP_NOT, obj, ks )
{
	return GET( m_iCost )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_START_WEDDING_ASK_ACK, obj, ks )
{
	return PUT( m_bAgree )
		;
}

SERIALIZE_DEFINE_GET( KEGS_START_WEDDING_ASK_ACK, obj, ks )
{
	return GET( m_bAgree )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_START_WEDDING_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_START_WEDDING_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_LOVE_WORD_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_wstrNewLoveWord )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_LOVE_WORD_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_wstrNewLoveWord )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_LOVE_WORD_NOT, obj, ks )
{
	return PUT( m_iUseItemUnitUID )
		&& PUT( m_wstrNewLoveWord )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_LOVE_WORD_NOT, obj, ks )
{
	return GET( m_iUseItemUnitUID )
		&& GET( m_wstrNewLoveWord )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_CHANGE_COUPLE_DATE_REQ, obj, ks )
{
	return PUT( m_iAddHour )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_CHANGE_COUPLE_DATE_REQ, obj, ks )
{
	return GET( m_iAddHour )
		;
}
//}

SERIALIZE_DEFINE_PUT( KEGS_GET_RIDING_PET_LIST_REQ, obj, ks )
{
	return PUT( m_byteViewPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RIDING_PET_LIST_REQ, obj, ks )
{
	return GET( m_byteViewPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_RIDING_PET_LIST_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecRidingPetList )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_RIDING_PET_LIST_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecRidingPetList )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SUMMON_RIDING_PET_REQ, obj, ks )
{
	return PUT( m_iRidingPetUID )
		&& PUT( m_bNeedAck )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_RIDING_PET_REQ, obj, ks )
{
	return GET( m_iRidingPetUID )
		&& GET( m_bNeedAck )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SUMMON_RIDING_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kRidingPetInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_RIDING_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kRidingPetInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SUMMON_RIDING_PET_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iRidingPetUID )
		&& PUT( m_usRidingPetID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SUMMON_RIDING_PET_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iRidingPetUID )
		&& GET( m_usRidingPetID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UNSUMMON_RIDING_PET_REQ, obj, ks )
{
	return PUT( m_iRidingPetUID )
		&& PUT( m_fStamina )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNSUMMON_RIDING_PET_REQ, obj, ks )
{
	return GET( m_iRidingPetUID )
		&& GET( m_fStamina )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UNSUMMON_RIDING_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNSUMMON_RIDING_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UNSUMMON_RIDING_PET_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UNSUMMON_RIDING_PET_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_RIDING_PET_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_RIDING_PET_REQ, obj, ks )
{
	return GET( m_iItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_RIDING_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kCreatedPetInfo )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_RIDING_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kCreatedPetInfo )
		&& GET( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELEASE_RIDING_PET_REQ, obj, ks )
{
	return PUT( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELEASE_RIDING_PET_REQ, obj, ks )
{
	return GET( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELEASE_RIDING_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELEASE_RIDING_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRidingPetUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RELEASE_RIDING_PET_NOT, obj, ks )
{
	return PUT( m_iRidingPetUID )
		&& PUT( m_usRindingPetID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RELEASE_RIDING_PET_NOT, obj, ks )
{
	return GET( m_iRidingPetUID )
		&& GET( m_usRindingPetID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_RIDING_PET_NOT, obj, ks )
{
	return PUT( m_iRidingPetUID )
		&& PUT( m_fStamina )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_RIDING_PET_NOT, obj, ks )
{
	return GET( m_iRidingPetUID )
		&& GET( m_fStamina )
		;
}


//{{ 2013. 05. 10	최육사	아이템 개편
SERIALIZE_DEFINE_PUT( KEGS_ITEM_EVALUATE_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_EVALUATE_REQ, obj, ks )
{
	return GET( m_iItemUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ITEM_EVALUATE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_EVALUATE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_vecInventorySlotInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_RESTORE_ITEM_EVALUATE_REQ, obj, ks )
{
	return PUT( m_iSupportItemUID )
		&& PUT( m_iTargetItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_ITEM_EVALUATE_REQ, obj, ks )
{
	return GET( m_iSupportItemUID )
		&& GET( m_iTargetItemUID )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_RESTORE_ITEM_EVALUATE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_ITEM_EVALUATE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecInventorySlotInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ITEM_CONVERT_REQ, obj, ks )
{
	return PUT( m_iItemUID )
        && PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_CONVERT_REQ, obj, ks )
{
	return GET( m_iItemUID )
        && GET( m_iQuantity )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_ITEM_CONVERT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_CONVERT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_vecInventorySlotInfo )
		;
}
//}}


//{{ 2013. 04. 22	최육사	어둠의 문 입장 로직 변경
SERIALIZE_DEFINE_PUT( KEGS_ENTER_DEFENCE_DUNGEON_POPUP_REPLY_NOT, obj, ks )
{
	return PUT( m_bAgree )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ENTER_DEFENCE_DUNGEON_POPUP_REPLY_NOT, obj, ks )
{
	return GET( m_bAgree )
		;
}
//}}




SERIALIZE_DEFINE_PUT( KEGS_CHANGE_MY_UNIT_INFO_NOT, obj, ks )
{
	return PUT( m_cUnitClass )
		&& PUT( m_iNewDefaultSkill_1 )
		&& PUT( m_iNewDefaultSkill_2 )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_MY_UNIT_INFO_NOT, obj, ks )
{
	return GET( m_cUnitClass )
		&& GET( m_iNewDefaultSkill_1 )
		&& GET( m_iNewDefaultSkill_2 )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_AUTO_GET_ALL_SKILL_REQ, obj, ks )
{
	return PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_AUTO_GET_ALL_SKILL_REQ, obj, ks )
{
	return GET( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_AUTO_GET_ALL_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_iSPoint )
		&& PUT( m_iCSPoint )
		&& PUT( m_mapSkillList )
		&& PUT( m_vecUnsealedSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_AUTO_GET_ALL_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_iSPoint )
		&& GET( m_iCSPoint )
		&& GET( m_mapSkillList )
		&& GET( m_vecUnsealedSkillID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_SKILL_REQ, obj, ks )
{
	return PUT( m_iSkillID )
		&& PUT( m_iSkillLevel )
		&& PUT( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_SKILL_REQ, obj, ks )
{
	return GET( m_iSkillID )
		&& GET( m_iSkillLevel )
		&& GET( m_iActiveSkillPageNumber )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_GET_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iSkillID )
		&& PUT( m_iSkillLevel )
		&& PUT( m_iActiveSkillPageNumber )
		&& PUT( m_iCSPoint )
		&& PUT( m_bUnsealed )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_GET_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iSkillID )
		&& GET( m_iSkillLevel )
		&& GET( m_iActiveSkillPageNumber )
		&& GET( m_iCSPoint )
		&& GET( m_bUnsealed )
		;
}




SERIALIZE_DEFINE_PUT( KEGS_ITEM_EXTRACT_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_iQuantity )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_EXTRACT_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_iQuantity )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ITEM_EXTRACT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_mapInsertedItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ITEM_EXTRACT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_mapInsertedItem )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_FINALITY_SKILL_REQ, obj, ks )
{
	return PUT( m_iItemUID )
		&& PUT( m_bNoConsume )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_FINALITY_SKILL_REQ, obj, ks )
{
	return GET( m_iItemUID )
		&& GET( m_bNoConsume )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_FINALITY_SKILL_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_bNoConsume )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_FINALITY_SKILL_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_bNoConsume )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT, obj, ks )
{
	return PUT( m_iBattleFieldID )
		&& PUT( m_iBossFieldID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT, obj, ks )
{
	return GET( m_iBattleFieldID )
		&& GET( m_iBossFieldID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT, obj, ks )
{
	return PUT( m_iBattleFieldID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT, obj, ks )
{
	return GET( m_iBattleFieldID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SOCKET_EXPAND_ITEM_REQ, obj, ks )
{
	return PUT( m_iMaterialItemUID )
		&& PUT( m_iTargetItemUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SOCKET_EXPAND_ITEM_REQ, obj, ks )
{
	return GET( m_iMaterialItemUID )
		&& GET( m_iTargetItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SOCKET_EXPAND_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SOCKET_EXPAND_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_CLEAR_REQ, obj, ks )
{
	return PUT( m_iClearConditionIndex )
		;
}
SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_CLEAR_REQ, obj, ks )
{
	return GET( m_iClearConditionIndex )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SUB_STAGE_CLEAR_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iClearType )
		&& PUT( m_iStageIndex )
		&& PUT( m_iSubStageIndex )
		;
}
SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SUB_STAGE_CLEAR_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iClearType )
		&& GET( m_iStageIndex )
		&& GET( m_iSubStageIndex )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_SECRET_STAGE_LOAD_REQ, obj, ks )
{
	return PUT( m_iPadID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_SECRET_STAGE_LOAD_REQ, obj, ks )
{
	return GET( m_iPadID )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT, obj, ks )
{
	return PUT( m_iStartSecretStageEnteringEvent )
		;
}
SERIALIZE_DEFINE_GET( KEGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT, obj, ks )
{
	return GET( m_iStartSecretStageEnteringEvent )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_NEXT_BANK_ED_ACK, obj, ks )
{
    return PUT( m_iOK )
        && PUT( m_iED )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_GET_NEXT_BANK_ED_ACK, obj, ks )
{
    return GET( m_iOK )
        && GET( m_iED )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_EXPAND_BANK_SLOT_ED_ACK, obj, ks )
{
    return PUT( m_iOK )
        && PUT( m_iED )
        && PUT( m_mapExpandedCategorySlot )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_BANK_SLOT_ED_ACK, obj, ks )
{
    return GET( m_iOK )
        && GET( m_iED )
        && GET( m_mapExpandedCategorySlot )
        ;
}




SERIALIZE_DEFINE_PUT( KEGS_KOM_FILE_CHECK_LOG_REQ, obj, ks )
{
	return PUT( m_wstrInvalidKomName )
		;
}
SERIALIZE_DEFINE_GET( KEGS_KOM_FILE_CHECK_LOG_REQ, obj, ks )
{
	return GET( m_wstrInvalidKomName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_KOM_FILE_CHECK_LOG_ACK, obj, ks )
{
	return PUT( m_wstrInvalidKomName )
		&& PUT( m_iOK )
		;
}
SERIALIZE_DEFINE_GET( KEGS_KOM_FILE_CHECK_LOG_ACK, obj, ks )
{
	return GET( m_wstrInvalidKomName )
		&& GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_EXPAND_SKILL_PAGE_ACK, obj, ks )
{
	return	PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_iTheNumberOfSkillPagesAvailable )
		&& PUT( m_iSPointAvailable )
		&& PUT( m_iCSPointAvailable )
		&& PUT( m_vecSkillListLearned )
		&& PUT( m_vecUnsealedSkillID )
		&& PUT( m_bPayWithED )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXPAND_SKILL_PAGE_ACK, obj, ks )
{
	return	GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_iTheNumberOfSkillPagesAvailable )
		&& GET( m_iSPointAvailable )
		&& GET( m_iCSPointAvailable )
		&& GET( m_vecSkillListLearned )
		&& GET( m_vecUnsealedSkillID )
		&& GET( m_bPayWithED )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ, obj, ks )
{
	return	PUT( m_iSkillPagesNumberDecidedToUse )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ, obj, ks )
{
	return	GET( m_iSkillPagesNumberDecidedToUse )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, obj, ks )
{
	return	PUT( m_iOK )
		&& PUT( m_iSkillPagesNumberDecidedToUse )
		&& PUT( m_iSPointAvailable )
		&& PUT( m_iCSPointAvailable )
		&& PUT( m_kUserSkillPageData )
		;
}

SERIALIZE_DEFINE_GET( KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK, obj, ks )
{
	return	GET( m_iOK )
		&& GET( m_iSkillPagesNumberDecidedToUse )
		&& GET( m_iSPointAvailable )
		&& GET( m_iCSPointAvailable )
		&& GET( m_kUserSkillPageData )
		;

}

SERIALIZE_DEFINE_PUT( KEGS_CHARACTER_LIST_ACK, obj, ks )
{
    return PUT( m_iOK )
        && PUT( m_mapServerGroupUnitSlot )
        && PUT( m_mapServerGroupUnitInfo )
        && PUT( m_strUserID )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_CHARACTER_LIST_ACK, obj, ks )
{
    return GET( m_iOK )
        && GET( m_mapServerGroupUnitSlot )
        && GET( m_mapServerGroupUnitInfo )
        && GET( m_strUserID )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_CREATE_NEW_UNIT_REQ, obj, ks )
{
    return PUT( m_wstrNickName )
        && PUT( m_iClass )
        && PUT( m_iServerGroup )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_CREATE_NEW_UNIT_REQ, obj, ks )
{
    return GET( m_wstrNickName )
        && GET( m_iClass )
        && GET( m_iServerGroup )
        ;
}
SERIALIZE_DEFINE_PUT( KEGS_ENTRY_POINT_DELETE_UNIT_REQ, obj, ks )
{
    return PUT( m_iUnitUID )
        && PUT( m_iUserUID )
        && PUT( m_iServerGroup )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_ENTRY_POINT_DELETE_UNIT_REQ, obj, ks )
{
    return GET( m_iUnitUID )
        && GET( m_iUserUID )
        && GET( m_iServerGroup )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_ENTRY_POINT_CHECK_NICK_NAME_REQ, obj, ks )
{
    return PUT( m_iServerGroup )
        && PUT( m_wstrNickName )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_ENTRY_POINT_CHECK_NICK_NAME_REQ, obj, ks )
{
    return GET( m_iServerGroup )
        && GET( m_wstrNickName )
        ;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_CREATE_UNIT_TODAY_COUNT_ACK, obj, ks )
{
    return PUT( m_iUserUID )
        && PUT( m_mapCreateCharCountToday )
        //&& PUT( m_wstrNickName )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_GET_CREATE_UNIT_TODAY_COUNT_ACK, obj, ks )
{
    return GET( m_iUserUID )
        && GET( m_mapCreateCharCountToday )
        //&& GET( m_wstrNickName )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ, obj, ks )
{
    return PUT( m_iServerGroup )
        && PUT( m_iUnitUID )
        && PUT( m_wstrNickName )
        && PUT( m_bCheckOnly )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ, obj, ks )
{
    return GET( m_iServerGroup )
        && GET( m_iUnitUID )
        && GET( m_wstrNickName )
        && GET( m_bCheckOnly )
        ;
}

SERIALIZE_DEFINE_PUT( KEGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK, obj, ks )
{
    return PUT( m_mapSolesChannelList )
        && PUT( m_mapGaiaChannelList )
        && PUT( m_mapSolesChannelBonusList )
        && PUT( m_mapGaiaChannelBonusList )
        ;
}

SERIALIZE_DEFINE_GET( KEGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK, obj, ks )
{
    return GET( m_mapSolesChannelList )
        && GET( m_mapGaiaChannelList )
        && GET( m_mapSolesChannelBonusList )
        && GET( m_mapGaiaChannelBonusList )
        ;
}
