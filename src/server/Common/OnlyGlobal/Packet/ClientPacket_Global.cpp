#include <winsock2.h>
#include "ClientPacket.h"

SERIALIZE_DEFINE_PUT( KECH_SECURITY_AUTH_REQ, obj, ks )
{
	return PUT( m_iSecurityAuthType )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrUserID )
		;
}

SERIALIZE_DEFINE_GET( KECH_SECURITY_AUTH_REQ, obj, ks )
{
	return GET( m_iSecurityAuthType )
		&& GET( m_iUserUID )
		&& GET( m_wstrUserID )
		;
}

SERIALIZE_DEFINE_PUT( KECH_SECURITY_AUTH_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_strSecondSecurityURL )
		;
}

SERIALIZE_DEFINE_GET( KECH_SECURITY_AUTH_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_strSecondSecurityURL )
		;
}

SERIALIZE_DEFINE_PUT( KECH_SECURITY_INFO_NOT, obj, ks )
{
	return PUT( m_iSecurityInfoType )
		&& PUT( m_uiMatrixKey )
		&& PUT( m_wstrCoordinate )
		;
}

SERIALIZE_DEFINE_GET( KECH_SECURITY_INFO_NOT, obj, ks )
{
	return GET( m_iSecurityInfoType )
		&& GET( m_uiMatrixKey )
		&& GET( m_wstrCoordinate )
		;
}




SERIALIZE_DEFINE_PUT( KEGS_EXCHANGE_CASH_REQ, obj, ks )
{
	return PUT( m_uiExchangeValue )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXCHANGE_CASH_REQ, obj, ks )
{
	return GET( m_uiExchangeValue )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_EXCHANGE_CASH_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPoint )
		&& PUT( m_AddBonusCashInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EXCHANGE_CASH_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPoint )
		&& GET( m_AddBonusCashInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BONUS_CASH_EVENT_NOT, obj, ks )
{
	return PUT( m_bEventStart )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BONUS_CASH_EVENT_NOT, obj, ks )
{
	return GET( m_bEventStart )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BILL_PRODUCT_INFO_REQ, obj, ks )
{
	return  PUT( m_iPage );
}

SERIALIZE_DEFINE_GET( KEGS_BILL_PRODUCT_INFO_REQ, obj, ks )
{
	return  GET( m_iPage );
}

SERIALIZE_DEFINE_PUT( KEGS_BILL_PRODUCT_INFO_ACK, obj, ks )
{
	return PUT( m_bFinal )
		&& PUT( m_vecProductInfo )
		&& PUT( m_mapKeepShowItem )
		&& PUT( m_DisCountInfoMap )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BILL_PRODUCT_INFO_ACK, obj, ks )
{
	return GET( m_bFinal )
		&& GET( m_vecProductInfo )
		&& GET( m_mapKeepShowItem )
		&& GET( m_DisCountInfoMap )

		;
}

SERIALIZE_DEFINE_PUT( KEGS_BILL_INVENTORY_INQUIRY_REQ, obj, ks )
{
	return PUT( m_nItemPerPage )
		&& PUT( m_iCurrentPage )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BILL_INVENTORY_INQUIRY_REQ, obj, ks )
{
	return GET( m_nItemPerPage )
		&& GET( m_iCurrentPage )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BILL_INVENTORY_INQUIRY_ACK, obj, ks )
{
	return  PUT( m_iOK )
		&&	PUT( m_vecOrderInfo )
		&&	PUT( m_vecPackageInfo )
		&&	PUT( m_vecSubPackageTrans )
		&&	PUT( m_vecSubPackageInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BILL_INVENTORY_INQUIRY_ACK, obj, ks )
{
	return  GET( m_iOK )
		&&	GET( m_vecOrderInfo )
		&&	GET( m_vecPackageInfo )
		&&	GET( m_vecSubPackageTrans )
		&&	GET( m_vecSubPackageInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ, obj, ks )
{
	return PUT( m_iTransNo )
		&& PUT( m_iProductNo )
		&& PUT( m_mapSocketForCash )
		&& PUT( m_mapSocketGroupIDForCash )
		&& PUT( m_byteProductKind )
		;
}

SERIALIZE_DEFINE_GET( KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ, obj, ks )
{
	return GET( m_iTransNo )
		&& GET( m_iProductNo )
		&& GET( m_mapSocketForCash )
		&& GET( m_mapSocketGroupIDForCash )
		&& GET( m_byteProductKind )
		;
}



SERIALIZE_DEFINE_PUT( KEGS_ADMIN_DELETE_GUILD_ADD_NOT, obj, ks )
{
	return PUT( m_iGuildUID	   )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_DELETE_GUILD_ADD_NOT, obj, ks )
{
	return GET( m_iGuildUID	   )
		;
}

//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
SERIALIZE_DEFINE_PUT( KEGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_iRank		   )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_iRank		   )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_iUnitUID	   )
		;
}

SERIALIZE_DEFINE_GET( KEGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_iUnitUID	   )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT, obj, ks )
{
	return PUT( m_wstrNewCharTBMsg )
		;
}

SERIALIZE_DEFINE_GET( KEGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT, obj, ks )
{
	return GET( m_wstrNewCharTBMsg )
		;
}

// 2011.05.05 lygan_조성욱 // 중국용 커뮤니티탭 유저 정보 보기

SERIALIZE_DEFINE_PUT( KEGS_USER_COMMUNITY_SURVEY_NEW_REQ , obj, ks )
{
	return PUT(m_wUnitNickName)
		&& PUT(m_cCommunity_Tab_Type)
		&& PUT(m_iOK);

}
SERIALIZE_DEFINE_GET( KEGS_USER_COMMUNITY_SURVEY_NEW_REQ , obj, ks )
{
	return GET(m_wUnitNickName)
		&& GET(m_cCommunity_Tab_Type)
		&& GET(m_iOK);

}

SERIALIZE_DEFINE_PUT( KEGS_USER_COMMUNITY_SURVEY_NEW_ACK , obj, ks )
{
	return PUT(unitUID)
		&& PUT(m_cCommunity_Tab_Type)
		&& PUT(m_iOK);

}
SERIALIZE_DEFINE_GET( KEGS_USER_COMMUNITY_SURVEY_NEW_ACK , obj, ks )
{
	return GET(unitUID)
		&& GET(m_cCommunity_Tab_Type)
		&& GET(m_iOK);

}


SERIALIZE_DEFINE_PUT( KEGS_PVP_REMATCH_REQ, obj, ks )
{
	return PUT( m_bAcceptRematch )
		;
}
SERIALIZE_DEFINE_GET( KEGS_PVP_REMATCH_REQ, obj, ks )
{
	return GET( m_bAcceptRematch )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_PVP_REMATCH_ACK, obj, ks )
{
	return PUT( m_iOK )
		;
}
SERIALIZE_DEFINE_GET( KEGS_PVP_REMATCH_ACK, obj, ks )
{
	return GET( m_iOK )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_PVP_REMATCH_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_bAcceptRematch )
		;
}
SERIALIZE_DEFINE_GET( KEGS_PVP_REMATCH_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_bAcceptRematch )
		;
}

//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
SERIALIZE_DEFINE_PUT( KEGS_RESTORE_UNIT_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iUserUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_UNIT_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iUserUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_RESTORE_UNIT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_tRestoreAbleDate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_RESTORE_UNIT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_tRestoreAbleDate )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_FINAL_DELETE_UNIT_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iUserUID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FINAL_DELETE_UNIT_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iUserUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_FINAL_DELETE_UNIT_ACK, obj, ks )
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
		&& PUT( m_tReDelAbleDate )
		;
}

SERIALIZE_DEFINE_GET( KEGS_FINAL_DELETE_UNIT_ACK, obj, ks )
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
		&& GET( m_tReDelAbleDate )
		;
}
//}}

//{{ 2011.03.16   임규수 아바타 분해 시스템
//}}

//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}

SERIALIZE_DEFINE_PUT( KECH_GET_SERVERGROUP_LIST_NOT, obj, ks )
{
	return PUT( m_mapServerGroupList )
		;
}

SERIALIZE_DEFINE_GET( KECH_GET_SERVERGROUP_LIST_NOT, obj, ks )
{
	return GET( m_mapServerGroupList )
		;
}

//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}}



SERIALIZE_DEFINE_PUT( KEGS_XTRAP_REQ, obj, ks )
{
	return PUT(m_vecData);
}

SERIALIZE_DEFINE_GET( KEGS_XTRAP_REQ, obj, ks )
{
	return GET(m_vecData);
}

SERIALIZE_DEFINE_PUT( KEGS_XTRAP_ACK, obj, ks )
{
	return PUT(m_vecData);
}

SERIALIZE_DEFINE_GET( KEGS_XTRAP_ACK, obj, ks )
{
	return GET(m_vecData);
}

SERIALIZE_DEFINE_PUT( KEGS_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_kFieldUserInfo )
		;
}
SERIALIZE_DEFINE_GET( KEGS_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_kFieldUserInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_GET_SHARE_BANK_REQ, obj, ks )
{
	return PUT( m_wstrNickName )
		&& PUT( m_iNewUnitTradeBlockDay )
		&& PUT( m_iNewUnitTradeBlockUnitClass )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_SHARE_BANK_REQ, obj, ks )
{
	return GET( m_wstrNickName )
		&& GET( m_iNewUnitTradeBlockDay )
		&& GET( m_iNewUnitTradeBlockUnitClass )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GET_SHARE_BANK_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_iBankSize )
		&& PUT( m_mapItem )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_SHARE_BANK_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		&& GET( m_iBankSize )
		&& GET( m_mapItem )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_ITEM_POSITION_NOT, obj, ks )
{
	return PUT( m_vecShareItemResult )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_ITEM_POSITION_NOT, obj, ks )
{
	return GET( m_vecShareItemResult )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_SHARING_BACK_OPEN_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iOpenType )
		;
};

SERIALIZE_DEFINE_GET( KEGS_SHARING_BACK_OPEN_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iOpenType )
		;
};

SERIALIZE_DEFINE_PUT( KEGS_GET_ADVERTISEMENT_EVENT_ACK, obj, ks )
{
	return PUT( m_vecAdvertisementURL )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GET_ADVERTISEMENT_EVENT_ACK, obj, ks )
{
	return GET( m_vecAdvertisementURL )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_WARP_BY_BUTTON_REQ, obj, ks )
{
	return PUT( m_iCurrentVillageWarpIndex )
		&& PUT( m_bFreeWarp )
		;
};

SERIALIZE_DEFINE_GET( KEGS_WARP_BY_BUTTON_REQ, obj, ks )
{
	return GET( m_iCurrentVillageWarpIndex )
		&& GET( m_bFreeWarp )
		;
};

SERIALIZE_DEFINE_PUT( KEGS_WARP_BY_BUTTON_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iED )
		&& PUT( m_iWarpPointMapID )
		;
};

SERIALIZE_DEFINE_GET( KEGS_WARP_BY_BUTTON_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iED )
		&& GET( m_iWarpPointMapID )
		;
};


SERIALIZE_DEFINE_PUT( KEGS_UPDATE_MAX_LEVEL_NOT, obj, ks )
{
	return PUT( m_iMaxLevel )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_MAX_LEVEL_NOT, obj, ks )
{
	return GET( m_iMaxLevel )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_DUNGEON_STATUS_NOT, obj, ks )
{
	return PUT( m_mapEnableDungeon )
		;
}

SERIALIZE_DEFINE_GET( KEGS_UPDATE_DUNGEON_STATUS_NOT, obj, ks )
{
	return GET( m_mapEnableDungeon )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_UPDATE_CODE_EVENT_NOT, obj, ks )
{
	return PUT( m_mapEnableCodeEnum )
		;
}
SERIALIZE_DEFINE_GET( KEGS_UPDATE_CODE_EVENT_NOT, obj, ks )
{
	return GET( m_mapEnableCodeEnum )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_UPDATE_WARP_VIP_NOT, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_trWarpVipEndDate )
		;
}


SERIALIZE_DEFINE_GET( KEGS_UPDATE_WARP_VIP_NOT, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_trWarpVipEndDate )
		;
}




SERIALIZE_DEFINE_PUT( KEGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT, obj, ks )
{
	return PUT( m_vecWarningQuestID );
}

SERIALIZE_DEFINE_GET( KEGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT, obj, ks )
{
	return GET( m_vecWarningQuestID );
}


SERIALIZE_DEFINE_PUT( KEGS_SKILL_USE_REQ, obj, ks )
{
	return PUT( m_iSkillID )
		;
}

SERIALIZE_DEFINE_GET( KEGS_SKILL_USE_REQ, obj, ks )
{
	return GET( m_iSkillID )
		;
}


//{{김준환 서버시간 받아오기


SERIALIZE_DEFINE_PUT(KEGS_CHANGE_PET_ID_NOT, obj,ks)
{
	return PUT( m_iOK )
		&& PUT( m_iPetUID )
		;
}
SERIALIZE_DEFINE_GET(KEGS_CHANGE_PET_ID_NOT, obj,ks)
{
	return GET( m_iOK )
		&& GET( m_iPetUID )
		;
}



SERIALIZE_DEFINE_PUT( KEGS_READY_TO_SOSUN_EVENT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecKInventorySlotInfo )
		&& PUT( m_iFirstUnitClass )
		;
}

SERIALIZE_DEFINE_GET( KEGS_READY_TO_SOSUN_EVENT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecKInventorySlotInfo )
		&& GET( m_iFirstUnitClass )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_USE_PROPOSE_ITEM_REQ, obj, ks )
{
	return PUT( m_iUsedItemUID )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_PROPOSE_ITEM_REQ, obj, ks )
{
	return GET( m_iUsedItemUID )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_PROPOSE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrNickName )
		&& PUT( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_PROPOSE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrNickName )
		&& GET( m_vecKInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_EVENT_PROPOSE_RESULT_ACCEPTOR_NOT, obj, ks )
{
	return PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_EVENT_PROPOSE_RESULT_ACCEPTOR_NOT, obj, ks )
{
	return GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_USE_DIVORCE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KEGS_USE_DIVORCE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecUpdatedInventorySlot )
		;
}


SERIALIZE_DEFINE_PUT( KEGS_RELEASE_PET_REQ, obj, ks )
{
	return PUT( m_iPetUID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_RELEASE_PET_REQ, obj, ks )
{
	return GET( m_iPetUID )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_RELEASE_PET_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPetUID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_RELEASE_PET_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPetUID )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_RELEASE_PET_NOT, obj, ks )
{
	return PUT( m_iPetUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_iPetID )
		;
}
SERIALIZE_DEFINE_GET( KEGS_RELEASE_PET_NOT, obj, ks )
{
	return GET( m_iPetUID )
		&& GET( m_iUnitUID )
		&& GET( m_iPetID )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CUBE_IN_ITEM_MAPPING_NOT , obj, ks )
{
	return PUT( m_mapCubeInItemMapping )
		;
}
SERIALIZE_DEFINE_GET( KEGS_CUBE_IN_ITEM_MAPPING_NOT , obj, ks )
{
	return GET( m_mapCubeInItemMapping )
		;
}




SERIALIZE_DEFINE_PUT( KEGS_2013_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return PUT( m_bLevelUpEvent )
		;
}
SERIALIZE_DEFINE_GET( KEGS_2013_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return GET( m_bLevelUpEvent )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_2013_EVENT_MISSION_COMPLETE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRewardedLevel )
		&& PUT( m_vecInventorySlotInfo )
		;
}
SERIALIZE_DEFINE_GET( KEGS_2013_EVENT_MISSION_COMPLETE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRewardedLevel )
		&& GET( m_vecInventorySlotInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_GLOBAL_MISSION_UPDATE_NOT, obj, ks )
{
	return PUT( m_mapGlobalMissionInfo )
		;
}

SERIALIZE_DEFINE_GET( KEGS_GLOBAL_MISSION_UPDATE_NOT, obj, ks )
{
	return GET( m_mapGlobalMissionInfo )
		;
}
SERIALIZE_DEFINE_PUT( KEGS_UNLIMITED_SECOND_CHANGE_JOB_NOT , obj, ks )
{
	return PUT( m_iUnlimitedSecondChangeJob )
		;
}
SERIALIZE_DEFINE_GET( KEGS_UNLIMITED_SECOND_CHANGE_JOB_NOT , obj, ks )
{
	return GET( m_iUnlimitedSecondChangeJob )
		;
}
