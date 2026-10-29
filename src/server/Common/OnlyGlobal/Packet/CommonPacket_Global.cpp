#include "x2/platform/net.h"
#include "CommonPacket.h"



//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KGlobalCashInfo, obj, ks )
{
	return PUT( m_ulCash[0] )
		&& PUT( m_ulCash[1] )
		&& PUT( m_ulCash[2] )
		&& PUT( m_ulCash[3] )
		//&& PUT( m_ulCash[4] ) //GCT_PUBLISHER_BONUS_CASH 사용 시 주석 해제
		;
}

SERIALIZE_DEFINE_GET( KGlobalCashInfo, obj, ks )
{
	return GET( m_ulCash[0] )
		&& GET( m_ulCash[1] )
		&& GET( m_ulCash[2] )
		&& GET( m_ulCash[3] )
		//&& GET( m_ulCash[4] ) //GCT_PUBLISHER_BONUS_CASH 사용 시 주석 해제
		;
}

SERIALIZE_DEFINE_PUT( KBillProductCategory, obj, ks )
{
	return PUT( m_cCategoryNo )
		&& PUT( m_wstrCategoryName )
		&& PUT( m_cParentCategoryNo )
		&& PUT( m_cDisplayNo );
}

SERIALIZE_DEFINE_GET( KBillProductCategory, obj, ks )
{
	return GET( m_cCategoryNo )
		&& GET( m_wstrCategoryName )
		&& GET( m_cParentCategoryNo )
		&& GET( m_cDisplayNo );
}

SERIALIZE_DEFINE_PUT( KBillProductInfo, obj, ks )
{
	return PUT( m_iProductNo )
		&& PUT( m_iProductID )
		&& PUT( m_bSale )
		&& PUT( m_bEvent )
		&& PUT( m_bShow )
		&& PUT( m_bIsCashInvenSkip )
		&& PUT( m_wstrProductName )
		&& PUT( m_cCategoryNo )
		&& PUT( m_iSalePrice )
		&& PUT( m_iRealPrice )
		&& PUT( m_cPeriod )
		&& PUT( m_cQuantity )
		&& PUT( m_bEnableGift )
		&& PUT( m_cLimitLevel )
		&& PUT( m_wstrEndDate )
		;
}

SERIALIZE_DEFINE_GET( KBillProductInfo, obj, ks )
{
	return GET( m_iProductNo )
		&& GET( m_iProductID )
		&& GET( m_bSale )
		&& GET( m_bEvent )
		&& GET( m_bShow )
		&& GET( m_bIsCashInvenSkip )
		&& GET( m_wstrProductName )
		&& GET( m_cCategoryNo )
		&& GET( m_iSalePrice )
		&& GET( m_iRealPrice )
		&& GET( m_cPeriod )
		&& GET( m_cQuantity )
		&& GET( m_bEnableGift )
		&& GET( m_cLimitLevel )
		&& GET( m_wstrEndDate )
		;
}

SERIALIZE_DEFINE_PUT( KBillProductAttribute, obj, ks )
{
	return  PUT( m_iProductNo ) &&
		PUT( m_vecAttribute );
}

SERIALIZE_DEFINE_GET( KBillProductAttribute, obj, ks )
{
	return  GET( m_iProductNo ) &&
		GET( m_vecAttribute );
}

SERIALIZE_DEFINE_PUT( KBillPackageInfo, obj, ks )
{
	return PUT( m_iPackageNo )
		&& PUT( m_vecProductNo )
		&& PUT( m_ulProductNo )
		&& PUT( m_iProductID )
		&& PUT( m_wstrProductID )
		&& PUT( m_sProductExpire )
		&& PUT( m_usProductPieces )
		&& PUT( m_vecSocketOption )
		;
}

SERIALIZE_DEFINE_GET( KBillPackageInfo, obj, ks )
{
	return GET( m_iPackageNo )
		&& GET( m_vecProductNo )
		&& GET( m_ulProductNo )
		&& GET( m_iProductID )
		&& GET( m_wstrProductID )
		&& GET( m_sProductExpire )
		&& GET( m_usProductPieces )
		&& GET( m_vecSocketOption )
		;
}

SERIALIZE_DEFINE_PUT( KBillBuyInfo, obj, ks )
{
	return PUT( m_iProductNo )
		&& PUT( m_iPoint )
		&& PUT( m_iProductID )
		&& PUT( m_usOrderQuantity )
		&& PUT( m_iCategory )
		&& PUT( m_wstrMemo )
		&& PUT( m_wstrOrderID )
		;
}

SERIALIZE_DEFINE_GET( KBillBuyInfo, obj, ks )
{
	return GET( m_iProductNo )
		&& GET( m_iPoint )
		&& GET( m_iProductID )
		&& GET( m_usOrderQuantity )
		&& GET( m_iCategory )
		&& GET( m_wstrMemo )
		&& GET( m_wstrOrderID )
		;
}

SERIALIZE_DEFINE_PUT( KBillOrderInfo, obj, ks )
{
	return  PUT( m_iTransNo ) &&
		PUT( m_wstrOrderID ) &&
		PUT( m_kBillProductInfo ) &&
		PUT( m_vecSocketOption ) &&
		PUT( m_byteType ) &&
		PUT( m_wstrSenderNickName ) &&
		PUT( m_wstrSenderPresentMessage )
		//{{ 허상형 : [2010/8/19/] //	선물 기능 개편
		&& PUT( m_iFromUnitUID )
		//}} 허상형 : [2010/8/19/] //	선물 기능 개편
		&& PUT( m_byteProductKind )
		;
}

SERIALIZE_DEFINE_GET( KBillOrderInfo, obj, ks )
{
	return  GET( m_iTransNo ) &&
		GET( m_wstrOrderID ) &&
		GET( m_kBillProductInfo ) &&
		GET( m_vecSocketOption ) &&
		GET( m_byteType ) &&
		GET( m_wstrSenderNickName ) &&
		GET( m_wstrSenderPresentMessage )
		//{{ 허상형 : [2010/8/19/] //	선물 기능 개편
		&& GET( m_iFromUnitUID )
		//}} 허상형 : [2010/8/19/] //	선물 기능 개편
		&& GET( m_byteProductKind )
		;
}
SERIALIZE_DEFINE_PUT( kDisCountItemInfo, obj, ks )
{
	return  PUT( m_DisCountProNo ) &&
		PUT( m_BasicProNo ) &&
		PUT( m_DisCountKeepItem )
		;
}
SERIALIZE_DEFINE_GET( kDisCountItemInfo, obj, ks )
{

	return  GET( m_DisCountProNo ) &&
		GET( m_BasicProNo ) &&
		GET( m_DisCountKeepItem )
		;
}
//////////////////////////////////////////////////////////////////////////




SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_PUBLIC_REQ, obj, ks )
{
	return PUT( m_bPublic )
		&& PUT( m_wstrPassword )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_PUBLIC_REQ, obj, ks )
{
	return GET( m_bPublic )
		&& GET( m_wstrPassword )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_PUBLIC_ACK, obj, ks )
{
	return PUT( m_bPublic )
		&& PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_PUBLIC_ACK, obj, ks )
{
	return GET( m_bPublic )
		&& GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_PUBLIC_NOT, obj, ks )
{
	return PUT( m_bPublic )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_PUBLIC_NOT, obj, ks )
{
	return GET( m_bPublic )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_NAME_REQ, obj, ks )
{
	return PUT( m_wstrRoomName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_NAME_REQ, obj, ks )
{
	return GET( m_wstrRoomName )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_NAME_ACK, obj, ks )
{
	return PUT( m_wstrRoomName )
		&& PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_NAME_ACK, obj, ks )
{
	return GET( m_wstrRoomName )
		&& GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CHANGE_PVP_ROOM_NAME_NOT, obj, ks )
{
	return PUT( m_wstrRoomName )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CHANGE_PVP_ROOM_NAME_NOT, obj, ks )
{
	return GET( m_wstrRoomName )
		;
}


SERIALIZE_DEFINE_PUT( KPcBangReward, obj, ks )
{
	return PUT( m_iPcBangType )
		&& PUT( m_fEXPRate )
		&& PUT( m_fEDRate )
		&& PUT( m_fVSPointRate )
		&& PUT( m_bSpiritFree )
		&& PUT( m_fPetEXPRate )
		&& PUT( m_iAdditionalHenirReward )
		&& PUT( m_bSecretDungeonFree )
		&& PUT( m_bAvatar )
		&& PUT( m_bTitle )
		;
}

SERIALIZE_DEFINE_GET( KPcBangReward, obj, ks )
{
	return GET( m_iPcBangType )
		&& GET( m_fEXPRate )
		&& GET( m_fEDRate )
		&& GET( m_fVSPointRate )
		&& GET( m_bSpiritFree )
		&& GET( m_fPetEXPRate )
		&& GET( m_iAdditionalHenirReward )
		&& GET( m_bSecretDungeonFree )
		&& GET( m_bAvatar )
		&& GET( m_bTitle )
		;
}

//{{ 2011. 12. 09	박교현		서버군 리스트
SERIALIZE_DEFINE_PUT( KServerGroupInfo, obj, ks )
{
	return PUT( m_iOrder )
		&& PUT( m_iServerGroupUID )
		&& PUT( m_wstrServerGroupName )
		&& PUT( m_kServerIP )
		&& PUT( m_usMasterPort )
		&& PUT( m_usNCUDPPort )
		&& PUT( m_bEnable )
		;
}

SERIALIZE_DEFINE_GET( KServerGroupInfo, obj, ks )
{
	return GET( m_iOrder )
		&& GET( m_iServerGroupUID )
		&& GET( m_wstrServerGroupName )
		&& GET( m_kServerIP )
		&& GET( m_usMasterPort )
		&& GET( m_usNCUDPPort )
		&& GET( m_bEnable )
		;
}

SERIALIZE_DEFINE_PUT( KEGS_CATCH_HACKUSER_INFO_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_iCrashType )
		;
}

SERIALIZE_DEFINE_GET( KEGS_CATCH_HACKUSER_INFO_NOT, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_iUnitUID )
		&& GET( m_iCrashType )
		;
}

//{{ 2013. 05. 14	최육사	아이템 개편
SERIALIZE_DEFINE_PUT( KBlockInfo, obj, ks )
{
	return PUT( m_iBlockUID )
		&& PUT( m_cBlockType )
		&& PUT( m_iBlockTarget )
		&& PUT( m_wstrBlockTarget )
		&& PUT( m_wstrStartDate )
		&& PUT( m_wstrEndDate )
		;
}

SERIALIZE_DEFINE_GET( KBlockInfo, obj, ks )
{
	return GET( m_iBlockUID )
		&& GET( m_cBlockType )
		&& GET( m_iBlockTarget )
		&& GET( m_wstrBlockTarget )
		&& GET( m_wstrStartDate )
		&& GET( m_wstrEndDate )
		;
}
//}}

SERIALIZE_DEFINE_PUT( KTradeShareItemResult, obj, ks )
{
	return  PUT( m_iBeforeItemUID )
		&&	PUT( m_iNewItemUID )
		&&	PUT( m_wstrRegDate )
		;
}

SERIALIZE_DEFINE_GET( KTradeShareItemResult, obj, ks )
{
	return  GET( m_iBeforeItemUID )
		&&	GET( m_iNewItemUID )
		&&	GET( m_wstrRegDate )
		;
}






//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//#ifdef SERV_RELAY_DB_CONNECTION
SERIALIZE_DEFINE_PUT( KJPEventRewardInfo, obj, ks )
{
	return PUT( m_iRID )
		&& PUT( m_wstrUserID )
		&& PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_iRewardID )
		;
}

SERIALIZE_DEFINE_GET( KJPEventRewardInfo, obj, ks )
{
	return GET( m_iRID )
		&& GET( m_wstrUserID )
		&& GET( m_iUserUID )
		&& GET( m_iUnitUID )
		&& GET( m_iRewardID )
		;
}
//#endif SERV_RELAY_DB_CONNECTION
//}}




SERIALIZE_DEFINE_PUT( kRandomItemMappingToInfoClient, obj, ks )
{
	return PUT( m_iBeforeGroupID )
		&& PUT( m_iAfterGroupID )
		&& PUT( m_bCheckMapping )
		&& PUT( m_tStartTime )
		&& PUT( m_tEndTime )
		;
}

SERIALIZE_DEFINE_GET( kRandomItemMappingToInfoClient, obj, ks )
{
	return GET( m_iBeforeGroupID )
		&& GET( m_iAfterGroupID )
		&& GET( m_bCheckMapping )
		&& GET( m_tStartTime )
		&& GET( m_tEndTime )
		;
}



SERIALIZE_DEFINE_PUT( KItemName, obj, ks )
{
	return PUT( m_wstrKRItemName )
		&& PUT( m_wstrTransItemName )
		;
}

SERIALIZE_DEFINE_GET( KItemName, obj, ks )
{
	return GET( m_wstrKRItemName )
		&& GET( m_wstrTransItemName )
		;
}


//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
SERIALIZE_DEFINE_PUT( KGlobalMissionReward, obj, ks )
{
	return PUT( m_iRewardType )
		&& PUT( m_iRewardDurationTime )
		&& PUT( m_iScriptID )
		;
}
SERIALIZE_DEFINE_GET( KGlobalMissionReward, obj, ks )
{
	return GET( m_iRewardType )
		&& GET( m_iRewardDurationTime )
		&& GET( m_iScriptID )
		;
}

SERIALIZE_DEFINE_PUT( KGlobalMissionInfo, obj, ks )
{
	return PUT( m_bActive )
		&& PUT( m_wstrMissionStartTime )
		&& PUT( m_iDurationMinute )
		&& PUT( m_iRemainSecond )
		&& PUT( m_iMissionType )
		&& PUT( m_iClearCondition )
		&& PUT( m_iClearCount )
		&& PUT( m_iNowCount )
		;
}
SERIALIZE_DEFINE_GET( KGlobalMissionInfo, obj, ks )
{
	return GET( m_bActive )
		&& GET( m_wstrMissionStartTime )
		&& GET( m_iDurationMinute )
		&& GET( m_iRemainSecond )
		&& GET( m_iMissionType )
		&& GET( m_iClearCondition )
		&& GET( m_iClearCount )
		&& GET( m_iNowCount )
		;
}
//}} 2012. 09. 03	임홍락	글로벌 미션 매니저
