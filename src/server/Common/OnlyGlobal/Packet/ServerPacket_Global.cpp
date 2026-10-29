#include "x2/platform/net.h"
#include "ServerPacket.h"


SERIALIZE_DEFINE_PUT( KLoginResultInfo, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_uiServerUID )
		&& PUT( m_vecResultCount )
		;
}
SERIALIZE_DEFINE_GET( KLoginResultInfo, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_uiServerUID )
		&& GET( m_vecResultCount )
		;
}


SERIALIZE_DEFINE_PUT( KELG_ADMIN_DELETE_GUILD_ADD_NOT, obj, ks )
{
	return PUT( m_iGuildUID )
		;
}

SERIALIZE_DEFINE_GET( KELG_ADMIN_DELETE_GUILD_ADD_NOT, obj, ks )
{
	return GET( m_iGuildUID )
		;
}

//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
SERIALIZE_DEFINE_PUT( KELG_ADMIN_DELETE_HENIR_RANKING_RANK_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_iRank )
		;
}

SERIALIZE_DEFINE_GET( KELG_ADMIN_DELETE_HENIR_RANKING_RANK_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_iRank )
		;
}

SERIALIZE_DEFINE_PUT( KELG_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_GET( KELG_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_wstrNickName )
		;
}

SERIALIZE_DEFINE_PUT( KELG_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT, obj, ks )
{
	return PUT( m_iRankingType )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KELG_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT, obj, ks )
{
	return GET( m_iRankingType )
		&& GET( m_iUnitUID )
		;
}
//}}


//====================================================================//
//====================================================================//
//====================================================================//
//{{ 2013.03.19 조효진 해외 공용 인증 시스템 구성

//{{ 2010. 09. 10 조효진 Gameforge 인증을 위하여 SERIALIZE 작성
SERIALIZE_DEFINE_PUT( KEPUBLISHER_AUTHENTICATION_REQ, obj, ks )
{
	return PUT( m_iActorUID )
		&& PUT( m_wstrIP )
		&& PUT( m_wstrServiceAccountID )
		&& PUT( m_wstrServicePassword )
		&& PUT( m_iChannelingCode )
		&& PUT( m_bServerUseKogOTP )
		;
}


SERIALIZE_DEFINE_GET(  KEPUBLISHER_AUTHENTICATION_REQ, obj, ks )
{
	return GET( m_iActorUID )
		&& GET( m_wstrIP )
		&& GET( m_wstrServiceAccountID )
		&& GET( m_wstrServicePassword )
		&& GET( m_iChannelingCode )
		&& GET( m_bServerUseKogOTP )
		;
}


SERIALIZE_DEFINE_PUT( KEPUBLISHER_AUTHENTICATION_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iResult )
		&& PUT( m_wstrID )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_wstrIP )
		&& PUT( m_iChannelingCode )
		&& PUT( wstrPurchTok )
		&& PUT( m_uiAccountType )
		&& PUT( m_uiAccountState )
		&& PUT( m_strAgreementURL )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_AUTHENTICATION_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iResult )
		&& GET( m_wstrID )
		&& GET( m_uiPublisherUID )
		&& GET( m_wstrIP )
		&& GET( m_iChannelingCode )
		&& GET( wstrPurchTok )
		&& GET( m_uiAccountType )
		&& GET( m_uiAccountState )
		&& GET( m_strAgreementURL )
		;
}
//}}


SERIALIZE_DEFINE_PUT( KDBE_CH_USER_GENERAL_LOGIN_REQ, obj, ks )
{
	return PUT( m_wstrServiceAccountID )
		&& PUT( m_wstrIP )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_iChannelingCode )
		&& PUT( m_wstrPurchaseTok )
		&& PUT( m_bServerUseKogOTP )
		;
};

SERIALIZE_DEFINE_GET( KDBE_CH_USER_GENERAL_LOGIN_REQ, obj, ks )
{
	return GET( m_wstrServiceAccountID )
		&& GET( m_wstrIP )
		&& GET( m_uiPublisherUID )
		&& GET( m_iChannelingCode )
		&& GET( m_wstrPurchaseTok )
		&& GET( m_bServerUseKogOTP )
		;
};

SERIALIZE_DEFINE_PUT( KDBE_CH_USER_GENERAL_LOGIN_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrUserID )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrOTP )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_iChannelingCode )
		&& PUT( m_wstrPurchaseTok )
		;
};

SERIALIZE_DEFINE_GET( KDBE_CH_USER_GENERAL_LOGIN_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrUserID )
		&& GET( m_iUserUID )
		&& GET( m_wstrOTP )
		&& GET( m_uiPublisherUID )
		&& GET( m_iChannelingCode )
		&& GET( m_wstrPurchaseTok )
		;
};

SERIALIZE_DEFINE_PUT( KDBE_CH_USER_KOGOTP_LOGIN_REQ, obj, ks )
{
	return PUT( m_wstrServiceAccountID )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_wstrOTP )
		&& PUT( m_wstrIP )
		&& PUT( m_wstrPurchaseTok )
		&& PUT( m_iChannelingCode )
		;
};

SERIALIZE_DEFINE_GET( KDBE_CH_USER_KOGOTP_LOGIN_REQ, obj, ks )
{
	return GET( m_wstrServiceAccountID )
		&& GET( m_uiPublisherUID )
		&& GET( m_wstrOTP )
		&& GET( m_wstrIP )
		&& GET( m_wstrPurchaseTok )
		&& GET( m_iChannelingCode )
		;
};

SERIALIZE_DEFINE_PUT( KDBE_CH_USER_KOGOTP_LOGIN_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrUserID )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrOTP )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_wstrPurchaseTok )
		;
};

SERIALIZE_DEFINE_GET( KDBE_CH_USER_KOGOTP_LOGIN_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrUserID )
		&& GET( m_iUserUID )
		&& GET( m_wstrOTP )
		&& GET( m_uiPublisherUID )
		&& GET( m_wstrPurchaseTok )
		;
};


SERIALIZE_DEFINE_PUT( KELG_GAME_USER_AUTHENTICATE_REQ, obj, ks )
{
	return  PUT( m_kAuthenticateReq ) &&
		PUT( m_kNexonAccountInfo )
		;
}

SERIALIZE_DEFINE_GET( KELG_GAME_USER_AUTHENTICATE_REQ, obj, ks )
{
	return  GET( m_kAuthenticateReq ) &&
		GET( m_kNexonAccountInfo )
		;
}

SERIALIZE_DEFINE_PUT( KELG_GAME_USER_AUTHENTICATE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kAccountInfo )
		&& PUT( m_kNexonAccountInfo )
		&& PUT( m_cHackingUserType )
		&& PUT( m_uiGiantAccountType )
		//{{ 2011. 02. 23	최육사	계정 단위 카운트
		&& PUT( m_mapAccCountInfo )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KELG_GAME_USER_AUTHENTICATE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kAccountInfo )
		&& GET( m_kNexonAccountInfo )
		&& GET( m_cHackingUserType )
		&& GET( m_uiGiantAccountType )
		//{{ 2011. 02. 23	최육사	계정 단위 카운트
		&& GET( m_mapAccCountInfo )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_REPORT_HACK_USER_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_wstrUserID )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_iServerUID )
		&& PUT( m_strIP )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_REPORT_HACK_USER_NOT, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_wstrUserID )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		&& GET( m_iServerUID )
		&& GET( m_strIP )
		;
}

//////////////////////////////////////////////////////////////////////////


//#ifdef SERV_COUNTRY_CN

// 2010.07.07 박교현 / 자이언트 HLServer 인증
SERIALIZE_DEFINE_PUT( KEGIANT_COMMON_INITIALIZE_REQ, obj, ks )
{
	return PUT( m_wstrIP )
		&& PUT( m_usPort )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_COMMON_INITIALIZE_REQ, obj, ks )
{
	return GET( m_wstrIP )
		&& GET( m_usPort )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_COMMON_INITIALIZE_ACK, obj, ks )
{
	return PUT( m_usZone )
		&& PUT( m_usGame )
		&& PUT( m_wstrZoneName )
		&& PUT( m_byteNetType )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_COMMON_INITIALIZE_ACK, obj, ks )
{
	return GET( m_usZone )
		&& GET( m_usGame )
		&& GET( m_wstrZoneName )
		&& GET( m_byteNetType )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_COMMON_NULL_SERVER, /*obj*/, /*ks*/ )
{
	return true;
};

SERIALIZE_DEFINE_GET( KEGIANT_COMMON_NULL_SERVER, /*obj*/, /*ks*/ )
{
	return true;
};

SERIALIZE_DEFINE_PUT( KEGIANT_COMMON_NULL_CLIENT, /*obj*/, /*ks*/ )
{
	return true;
};

SERIALIZE_DEFINE_GET( KEGIANT_COMMON_NULL_CLIENT, /*obj*/, /*ks*/ )
{
	return true;
};


SERIALIZE_DEFINE_PUT( KEGIANT_AUTH_LOGIN_SUCCESS, obj, ks )
{
	return PUT( m_uiRequestID)
		&& PUT( m_uiUID )
		&& PUT( m_wstrName )
		&& PUT( m_uiType )
		&& PUT( m_wstrEMail )
		&& PUT( m_uiState )
		&& PUT( m_uiBindType )
		&& PUT( m_wstrSuperpasswd )
		&& PUT( m_wstrMobileAccount )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_AUTH_LOGIN_SUCCESS, obj, ks )
{
	return GET( m_uiRequestID)
		&& GET( m_uiUID )
		&& GET( m_wstrName )
		&& GET( m_uiType )
		&& GET( m_wstrEMail )
		&& GET( m_uiState )
		&& GET( m_uiBindType )
		&& GET( m_wstrSuperpasswd )
		&& GET( m_wstrMobileAccount )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_AUTH_LOGIN_FAIL, obj, ks )
{
	return PUT( m_uiRequestID )
		&& PUT( m_iReturnCode )
		&& PUT( m_wstrName )
		&& PUT( m_wstrName )
		&& PUT( m_uiKey )
		&& PUT( m_wstrCoordinate )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_AUTH_LOGIN_FAIL, obj, ks )
{
	return GET( m_uiRequestID )
		&& GET( m_iReturnCode )
		&& GET( m_wstrName )
		&& GET( m_wstrName )
		&& GET( m_uiKey )
		&& GET( m_wstrCoordinate )
		;
};


SERIALIZE_DEFINE_PUT( KDBE_GIANT_USER_LOGIN_REQ, obj, ks )
{
	return  PUT( m_bLogin )
		&&	PUT( m_wstrServiceAccountID )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_wstrOTP )
		&&	PUT( m_uiAccountType )
		&&	PUT( m_uiAccountState )
		;
};

SERIALIZE_DEFINE_GET( KDBE_GIANT_USER_LOGIN_REQ, obj, ks )
{
	return  GET( m_bLogin )
		&&	GET( m_wstrServiceAccountID )
		&&	GET( m_uiGiantUID )
		&&	GET( m_wstrOTP )
		&&	GET( m_uiAccountType )
		&&	GET( m_uiAccountState )
		;
};


SERIALIZE_DEFINE_PUT( KDBE_GIANT_USER_LOGIN_ACK, obj, ks )
{
	return  PUT( m_iOK )
		&&	PUT( m_wstrUserID )
		&&	PUT( m_iUserUID )
		&&	PUT( m_wstrOTP )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_uiAccountType )
		;
};

SERIALIZE_DEFINE_GET( KDBE_GIANT_USER_LOGIN_ACK, obj, ks )
{
	return  GET( m_iOK )
		&&	GET( m_wstrUserID )
		&&	GET( m_iUserUID )
		&&	GET( m_wstrOTP )
		&&	GET( m_uiGiantUID )
		&&	GET( m_uiAccountType )
		;
};



//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}}


SERIALIZE_DEFINE_PUT( KEPUBLISHER_BILLING_BALANCE_REQ, obj, ks )
{
	return PUT( m_uiPublisherUID )
		&& PUT( m_uiActorUID )
		&& PUT( m_wstrAccount )
		&& PUT( m_wstrIP )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_BILLING_BALANCE_REQ, obj, ks )
{
	return GET( m_uiPublisherUID )
		&& GET( m_uiActorUID )
		&& GET( m_wstrAccount )
		&& GET( m_wstrIP )
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_BILLING_BALANCE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_ulBalance )
		&& PUT( m_iCashType )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_BILLING_BALANCE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_ulBalance )
		&& GET( m_iCashType )
		;
}

SERIALIZE_DEFINE_PUT( KELG_VERIFY_GIANT_MATRIX_CARD_REQ, obj, ks )
{
	return	PUT( m_wstrID )
		&&	PUT( m_wstrMatrix )
		&&	PUT( m_uiGiantMatrixKey )
		;
}

SERIALIZE_DEFINE_GET( KELG_VERIFY_GIANT_MATRIX_CARD_REQ, obj, ks )
{
	return	GET( m_wstrID )
		&&	GET( m_wstrMatrix )
		&&	GET( m_uiGiantMatrixKey )
		;
}


SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_RETURN, obj, ks )
{
	return PUT( m_wstrTID )
		&& PUT( m_iBalance )
		&& PUT( m_iBonus )
		&& PUT( m_iHadFilled )
		&& PUT( m_iRet )
		&& PUT( m_iSubat )
		&& PUT( m_iAT )
		&& PUT( m_iPoint )
		&& PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_RETURN, obj, ks )
{
	return GET( m_wstrTID )
		&& GET( m_iBalance )
		&& GET( m_iBonus )
		&& GET( m_iHadFilled )
		&& GET( m_iRet )
		&& GET( m_iSubat )
		&& GET( m_iAT )
		&& GET( m_iPoint )
		&& GET( m_uiGiantUID )
		;
}



SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_USE_CARD_CMD, obj, ks )
{
	return PUT( m_sSource )
		&& PUT( m_wstrAccount )
		&& PUT( m_wstrTID )
		&& PUT( m_wstrCardID )
		&& PUT( m_wstrPasswd )
		&& PUT( m_wstrIP )
		&& PUT( m_uiUID )
		&& PUT( m_iAT )
		&& PUT( m_uiConTempID )
		&& PUT( m_uiOperateUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_USE_CARD_CMD, obj, ks )
{
	return GET( m_sSource )
		&& GET( m_wstrAccount )
		&& GET( m_wstrTID )
		&& GET( m_wstrCardID )
		&& GET( m_wstrPasswd )
		&& GET( m_wstrIP )
		&& GET( m_uiUID )
		&& GET( m_iAT )
		&& GET( m_uiConTempID )
		&& GET( m_uiOperateUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_USE_CARD_RET_CMD, obj, ks )
{
	return PUT( m_sSource )
		&& PUT( m_wstrAccount )
		&& PUT( m_wstrTID )
		&& PUT( m_wstrCardID )
		&& PUT( m_wstrPasswd )
		&& PUT( m_wstrIP )
		&& PUT( m_iPoint )
		&& PUT( m_iRet )
		&& PUT( m_uiUID )
		&& PUT( m_iAT )
		&& PUT( m_uiConTempID )
		&& PUT( m_uiOperateUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_USE_CARD_RET_CMD, obj, ks )
{
	return GET( m_sSource )
		&& GET( m_wstrAccount )
		&& GET( m_wstrTID )
		&& GET( m_wstrCardID )
		&& GET( m_wstrPasswd )
		&& GET( m_wstrIP )
		&& GET( m_iPoint )
		&& GET( m_iRet )
		&& GET( m_uiUID )
		&& GET( m_iAT )
		&& GET( m_uiConTempID )
		&& GET( m_uiOperateUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_CONSUME_REQ, obj, ks )
{
	return PUT( m_uiGiantUID )
		&& PUT( m_iPoint )
		&& PUT( m_iSource )
		&& PUT( m_wstrTID )
		&& PUT( m_wstrRemark )
		&& PUT( m_wstrIP )
		&& PUT( m_iAT )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_CONSUME_REQ, obj, ks )
{
	return GET( m_uiGiantUID )
		&& GET( m_iPoint )
		&& GET( m_iSource )
		&& GET( m_wstrTID )
		&& GET( m_wstrRemark )
		&& GET( m_wstrIP )
		&& GET( m_iAT )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_CONSUME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iBalance )
		&& PUT( m_iBonus )
		&& PUT( m_bHasFilled )
		&& PUT( m_iPoint )
		&& PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_CONSUME_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iBalance )
		&& PUT( m_iBonus )
		&& PUT( m_bHasFilled )
		&& PUT( m_iPoint )
		&& PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_PCARD_CMD, obj, ks )
{
	return PUT( m_uiGiantUID )
		&& PUT( m_iSource )
		&& PUT( m_wstrTID )
		&& PUT( m_wstrPCardID )
		&& PUT( m_wstrIP )
		&& PUT( m_iAT )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_PCARD_CMD, obj, ks )
{
	return GET( m_uiGiantUID )
		&& GET( m_iSource )
		&& GET( m_wstrTID )
		&& GET( m_wstrPCardID )
		&& GET( m_wstrIP )
		&& GET( m_iAT )
		;
}


SERIALIZE_DEFINE_PUT( KEGIANT_BILLING_SCARD_CMD, obj, ks )
{
	return PUT( m_uiGiantUID )
		&& PUT( m_iSource )
		&& PUT( m_wstrTID )
		&& PUT( m_wstrSCardID )
		&& PUT( m_wstrIP )
		&& PUT( m_iAT )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_BILLING_SCARD_CMD, obj, ks )
{
	return GET( m_uiGiantUID )
		&& GET( m_iSource )
		&& GET( m_wstrTID )
		&& GET( m_wstrSCardID )
		&& GET( m_wstrIP )
		&& GET( m_iAT )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG, obj, ks )
{
	return  PUT( m_usRegType )
		&&	PUT( m_usServerID )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_usZone )
		&&	PUT( m_usGame )
		&&	PUT( m_wstrName )
		&&	PUT( m_uiRoleUID )
		&&	PUT( m_usState )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG, obj, ks )
{
	return	GET( m_usRegType )
		&&	GET( m_usServerID )
		&&	GET( m_uiGiantUID )
		&&	GET( m_usZone )
		&&	GET( m_usGame )
		&&	GET( m_wstrName )
		&&	GET( m_uiRoleUID )
		&&	GET( m_usState )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_CREATE_UNIT_REQ, obj, ks )
{
	return  PUT( m_iActorUID )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_CreateUnitReqPacket )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_CREATE_UNIT_REQ, obj, ks )
{
	return	GET( m_iActorUID )
		&&	GET( m_uiGiantUID )
		&&	GET( m_CreateUnitReqPacket )
		;
}


SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_CREATE_UNIT_ACK, obj, ks )
{
	return  PUT( m_iOK )
		&&	PUT( m_CreateUnitReqPacket )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_CREATE_UNIT_ACK, obj, ks )
{
	return	GET( m_iOK )
		&&	GET( m_CreateUnitReqPacket )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_DELETE_UNIT_REQ, obj, ks )
{
	return	PUT( m_wstrNickName )
		&&	PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_DELETE_UNIT_REQ, obj, ks )
{
	return	GET( m_wstrNickName )
		&&	GET( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_DELETE_UNIT_ACK, obj, ks )
{
	return	PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_DELETE_UNIT_ACK, obj, ks )
{
	return	GET( m_iOK )
		;
}

SERIALIZE_DEFINE_PUT( KGiantDeletedUnitInfo, obj, ks )
{
	return	PUT( m_wstrNickName )
		&&	PUT( m_iUnitUID )
		&&	PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_GET( KGiantDeletedUnitInfo, obj, ks )
{
	return	GET( m_wstrNickName )
		&&	GET( m_iUnitUID )
		&&	GET( m_uiGiantUID )
		;
}
// 현재 사용하는 곳 없음 주석 처리
// SERIALIZE_DEFINE_PUT( KDBE_INSERT_GIANT_DELETE_UNIT_REQ, obj, ks )
// {
// 	return	PUT( m_DeletedUnitInfo )
// 		;
// };
//
// SERIALIZE_DEFINE_GET( KDBE_INSERT_GIANT_DELETE_UNIT_REQ, obj, ks )
// {
// 	return	GET( m_DeletedUnitInfo )
// 		;
// };

// 현재 사용하는 곳 없음 주석 처리
// SERIALIZE_DEFINE_PUT( KDBE_UPDATE_GIANT_DELETE_UNIT_NOT, obj, ks )
// {
// 	return	PUT( m_wstrNickName )
// 		&&	PUT( m_uiGiantUID )
// 		;
// };
//
// SERIALIZE_DEFINE_GET( KDBE_UPDATE_GIANT_DELETE_UNIT_NOT, obj, ks )
// {
// 	return	GET( m_wstrNickName )
// 		&&	GET( m_uiGiantUID )
// 		;
// };


SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_CREATE_GUILD_REQ, obj, ks )
{
	return	PUT( m_iActorUID )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_CreateGuildReqPacket )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_CREATE_GUILD_REQ, obj, ks )
{
	return	GET( m_iActorUID )
		&&	GET( m_uiGiantUID )
		&&	GET( m_CreateGuildReqPacket )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_CREATE_GUILD_ACK, obj, ks )
{
	return  PUT( m_iOK )
		&&	PUT( m_CreateGuildReqPacket )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_CREATE_GUILD_ACK, obj, ks )
{
	return	GET( m_iOK )
		&&	GET( m_CreateGuildReqPacket )
		;
}


SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_DELETE_GUILD_REQ, obj, ks )
{
	return  PUT( m_wstrGuildName )
		&&	PUT( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_DELETE_GUILD_REQ, obj, ks )
{
	return	GET( m_wstrGuildName )
		&&	GET( m_uiGiantUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_ROLEREG_DELETE_GUILD_ACK, obj, ks )
{
	return  PUT( m_iOK )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_ROLEREG_DELETE_GUILD_ACK, obj, ks )
{
	return	GET( m_iOK )
		;
}

// 현재 사용하는 곳 없음. 주석처리
// SERIALIZE_DEFINE_PUT( KDBE_GET_GIANT_DELETED_UNIT_LIST_ACK, obj, ks )
// {
// 	return	PUT( m_vecDeletedUnitInfo )
// 		;
// }
//
// SERIALIZE_DEFINE_GET( KDBE_GET_GIANT_DELETED_UNIT_LIST_ACK, obj, ks )
// {
// 	return	GET( m_vecDeletedUnitInfo )
// 		;
//

SERIALIZE_DEFINE_PUT( KEGIANT_INFO_GET_CCU_REQ, obj, ks )
{
	return  PUT( m_TimeStamp )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_INFO_GET_CCU_REQ, obj, ks )
{
	return  GET( m_TimeStamp )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_INFO_GET_TOTAL_CCU_ACK, obj, ks )
{
	return  PUT( m_TimeStamp )
		&&	PUT( m_usServerID )
		&&	PUT( m_usServerType )
		&&	PUT( m_usZone )
		&&	PUT( m_usGame )
		&&	PUT( m_strZoneName )
		&&	PUT( m_uiCCU )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_INFO_GET_TOTAL_CCU_ACK, obj, ks )
{
	return  GET( m_TimeStamp )
		&&	GET( m_usServerID )
		&&	GET( m_usServerType )
		&&	GET( m_usZone )
		&&	GET( m_usGame )
		&&	GET( m_strZoneName )
		&&	GET( m_uiCCU )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_INFO_GET_CHANNEL_CCU_ACK, obj, ks )
{
	return  PUT( m_TimeStamp )
		&&	PUT( m_usZone )
		&&	PUT( m_usGame )
		//&&	PUT( m_uiCountryNum )
		//&&	PUT( m_uiCountryID )
		//&&	PUT( m_uiCountryCCU )
		&&	PUT( m_mapChannelCCU )
		;
};

SERIALIZE_DEFINE_GET( KEGIANT_INFO_GET_CHANNEL_CCU_ACK, obj, ks )
{
	return  GET( m_TimeStamp )
		&&	GET( m_usZone )
		&&	GET( m_usGame )
		//&&	GET( m_uiCountryNum )
		//&&	GET( m_uiCountryID )
		//&&	GET( m_uiCountryCCU )
		&&	GET( m_mapChannelCCU )
		;
};

SERIALIZE_DEFINE_PUT( KEGIANT_INFO_CHAT_NOT, obj, ks )
{
	return  PUT( m_usZone )
		&&	PUT( m_usGame )
		&&	PUT( m_wstrUserName )
		&&	PUT( m_wstrGMName )
		&&	PUT( m_uiCountryID )
		&&	PUT( m_uiSceneID )
		&&	PUT( m_uiType )
		&&	PUT( m_wstrContent )
		&&	PUT( m_wstrServer )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_INFO_CHAT_NOT, obj, ks )
{
	return  GET( m_usZone )
		&&	GET( m_usGame )
		&&	GET( m_wstrUserName )
		&&	GET( m_wstrGMName )
		&&	GET( m_uiCountryID )
		&&	GET( m_uiSceneID )
		&&	GET( m_uiType )
		&&	GET( m_wstrContent )
		&&	GET( m_wstrServer )
		;
}

SERIALIZE_DEFINE_PUT( KEGIANT_INFO_PUNISH_REQ, obj, ks )
{
	return  PUT( m_usZone )
		&&	PUT( m_usGame )
		&&	PUT( m_wstrUserName )
		&&	PUT( m_wstrCountryName )
		&&	PUT( m_wstrServerName )
		&&	PUT( m_wstrGMName )
		&&	PUT( m_wstrReason )
		&&	PUT( m_usOperation )
		&&	PUT( m_iDelay )
		&&	PUT( m_sLevel )
		&&	PUT( m_uiGiantUID )
		&&	PUT( m_uiStartTime )
		;
}

SERIALIZE_DEFINE_GET( KEGIANT_INFO_PUNISH_REQ, obj, ks )
{
	return  GET( m_usZone )
		&&	GET( m_usGame )
		&&	GET( m_wstrUserName )
		&&	GET( m_wstrCountryName )
		&&	GET( m_wstrServerName )
		&&	GET( m_wstrGMName )
		&&	GET( m_wstrReason )
		&&	GET( m_usOperation )
		&&	GET( m_iDelay )
		&&	GET( m_sLevel )
		&&	GET( m_uiGiantUID )
		&&	GET( m_uiStartTime )
		;
}



//#endif // SERV_COUNTRY_CN
//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEPUBLISHER_SECURITY_AUTH_REQ, obj, ks )
{
	return PUT( m_iSecurityAuthType )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrUserID )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_wstrIP )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_SECURITY_AUTH_REQ, obj, ks )
{
	return GET( m_iSecurityAuthType )
		&& GET( m_iUserUID )
		&& GET( m_wstrUserID )
		&& GET( m_uiPublisherUID )
		&& GET( m_wstrIP )
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_SECURITY_AUTH_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_strSecondSecurityURL )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_SECURITY_AUTH_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_strSecondSecurityURL )
		;
}


SERIALIZE_DEFINE_PUT( KEPUBLISHER_SECURITY_INFO_NOT, obj, ks )
{
	return PUT( m_iSecurityInfoType )
		&& PUT( m_uiMatrixKey )
		&& PUT( m_wstrCoordinate )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_SECURITY_INFO_NOT, obj, ks )
{
	return GET( m_iSecurityInfoType )
		&& GET( m_uiMatrixKey )
		&& GET( m_wstrCoordinate )
		;
}




SERIALIZE_DEFINE_PUT( KEPUBLISHER_REG_USER_REQ, obj, ks )
{
	return PUT( m_iActorUID )
		&& PUT( m_wstrServiceAccountID )
		&& PUT( m_wstrServicePassword )
		&& PUT( m_strIP )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_REG_USER_REQ, obj, ks )
{
	return GET( m_iActorUID )
		&& GET( m_wstrServiceAccountID )
		&& GET( m_wstrServicePassword )
		&& GET( m_strIP )
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_REG_USER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrServiceAccountID )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_REG_USER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrServiceAccountID )
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_UNREG_USER_REQ, obj, ks )
{
	return PUT( m_iActorUID )
		&& PUT( m_wstrServiceAccountID )
		&& PUT( m_wstrServicePassword )
		&& PUT( m_strIP )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_UNREG_USER_REQ, obj, ks )
{
	return GET( m_iActorUID )
		&& GET( m_wstrServiceAccountID )
		&& GET( m_wstrServicePassword )
		&& GET( m_strIP )
		;
}

SERIALIZE_DEFINE_PUT( KEPUBLISHER_UNREG_USER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrServiceAccountID )
		;
}

SERIALIZE_DEFINE_GET( KEPUBLISHER_UNREG_USER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrServiceAccountID )
		;
}

SERIALIZE_DEFINE_PUT( KELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrServiceAccountID )
		;
}

SERIALIZE_DEFINE_GET( KELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrServiceAccountID )
		;
}
//===============================================================================//
//{{ 2011. 03. 03	최육사	중국 서버 빌링

SERIALIZE_DEFINE_PUT( KEBILL_CHECK_PRODUCT_INFO_REQ, obj, ks )
{
	return

		PUT( m_mapReleaseTick )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_CHECK_PRODUCT_INFO_REQ, obj, ks )
{
	return
		GET( m_mapReleaseTick )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_CHECK_PRODUCT_INFO_ACK, obj, ks )
{
	return
		PUT( m_mapReleaseTick )
		&& PUT( m_mapBillProductInfo )
		&& PUT( m_mapBillProductAttribute )
		&& PUT( m_mapBillPackageInfo )
		&& PUT( m_vecStartEvent )
		&& PUT( m_vecEndEvent )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_CHECK_PRODUCT_INFO_ACK, obj, ks )
{
	return
		GET( m_mapReleaseTick )
		&& GET( m_mapBillProductInfo )
		&& GET( m_mapBillProductAttribute )
		&& GET( m_mapBillPackageInfo )
		&& GET( m_vecStartEvent )
		&& GET( m_vecEndEvent )
		;
}

//{{ 허상형 : [2010/7/28/] //	캐시 로그
SERIALIZE_DEFINE_PUT( KBuyCharInfo, obj, ks )
{
	return PUT( m_cLevel )
		&& PUT( m_iUnitClass );
}

SERIALIZE_DEFINE_GET( KBuyCharInfo, obj, ks )
{
	return GET( m_cLevel )
		&& GET( m_iUnitClass );
}
//}} 허상형 : [2010/7/28/] //	캐시 로그

SERIALIZE_DEFINE_PUT( KEBILL_BUY_PRODUCT_REQ, obj, ks )
{
	return	PUT( m_iServerGroupID )
		&&	PUT( m_iChannelingCode )
		&&	PUT( m_wstrUserID )
		&&	PUT( m_uiPublisherUID )
		&&	PUT( m_iUserUID )
		&&	PUT( m_iUnitUID )
		&&	PUT( m_wstrNickName )
		&&	PUT( m_strUserIP )
		&&	PUT( m_vecBillBuyInfo )
		//{{ 허상형 : [2010/7/28/] //	캐시 로그
		&&	PUT( m_BuyCharInfo )
		//}} 허상형 : [2010/7/28/] //	캐시 로그
		&&	PUT( m_iUseCashType )
		&&	PUT( m_iUserIP )
		&&	PUT( m_wstrTotalOrderID )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_BUY_PRODUCT_REQ, obj, ks )
{
	return	GET( m_iServerGroupID )
		&&	GET( m_iChannelingCode )
		&&	GET( m_wstrUserID )
		&&	GET( m_uiPublisherUID )
		&&	GET( m_iUserUID )
		&&	GET( m_iUnitUID )
		&&	GET( m_wstrNickName )
		&&	GET( m_strUserIP )
		&&	GET( m_vecBillBuyInfo )
		//{{ 허상형 : [2010/7/28/] //	캐시 로그
		&&	GET( m_BuyCharInfo )
		//}} 허상형 : [2010/7/28/] //	캐시 로그
		&&	GET( m_iUseCashType )
		&&	GET( m_iUserIP )
		&&	GET( m_wstrTotalOrderID )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_BUY_PRODUCT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecBillOrderInfo )
		//{{ 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
		&& PUT( m_vecPackageID )
		//}} 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
		&& PUT( m_iTotalCash )
		&& PUT( m_iCashType )
		&& PUT( m_RemainCashInfo )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_BUY_PRODUCT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecBillOrderInfo )
		//{{ 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
		&& GET( m_vecPackageID )
		//}} 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
		&& GET( m_iTotalCash )
		&& GET( m_iCashType )
		&& GET( m_RemainCashInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEBILL_PREPARE_BUY_PRODUCT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kEBILL_BUY_PRODUCT_REQ )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_PREPARE_BUY_PRODUCT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kEBILL_BUY_PRODUCT_REQ )
		;
}


SERIALIZE_DEFINE_GET( KEBILL_GIFT_ITEM_REQ, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_iChannelingCode )
		&& GET( m_wstrSenderUserID )
		&& GET( m_uiSenderPublisherUID )
		&& GET( m_wstrSenderNickName )
		&& GET( m_wstrReceiverUserID )
		&& GET( m_wstrReceiverNickName )
		&& GET( m_vecBillBuyInfo )
		&& GET( m_iSenderUserUID )
		&& GET( m_iSenderUnitUID )
		&& GET( m_iReceiverUserUID )
		&& GET( m_iReceiverUnitUID )
		&& GET( m_wstrPresentMessage )
		//{{ 허상형 : [2010/7/28/] //	캐시 로그
		&& GET( m_BuyCharInfo )
		&& GET( m_RecvCharInfo )
		//}} 허상형 : [2010/7/28/] //	캐시 로그
		&& GET( m_iUseCashType )
		&& GET( m_wstrUserIP )
		&& GET( m_wstrTotalOrderID )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_GIFT_ITEM_REQ, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_iChannelingCode )
		&& PUT( m_wstrSenderUserID )
		&& PUT( m_uiSenderPublisherUID )
		&& PUT( m_wstrSenderNickName )
		&& PUT( m_wstrReceiverUserID )
		&& PUT( m_wstrReceiverNickName )
		&& PUT( m_vecBillBuyInfo )
		&& PUT( m_iSenderUserUID )
		&& PUT( m_iSenderUnitUID )
		&& PUT( m_iReceiverUserUID )
		&& PUT( m_iReceiverUnitUID )
		&& PUT( m_wstrPresentMessage )
		//{{ 허상형 : [2010/7/28/] //	캐시 로그
		&& PUT( m_BuyCharInfo )
		&& PUT( m_RecvCharInfo )
		//}} 허상형 : [2010/7/28/] //	캐시 로그
		&& PUT( m_iUseCashType )
		&& PUT( m_wstrUserIP )
		&& PUT( m_wstrTotalOrderID )
		;
}


SERIALIZE_DEFINE_GET( KEBILL_GIFT_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecBillOrderInfo )
		&& GET( m_iTotalCash )
		&& GET( m_iCashType )
		&& GET( m_RemainCashInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_GIFT_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecBillOrderInfo )
		&& PUT( m_iTotalCash )
		&& PUT( m_iCashType )
		&& PUT( m_RemainCashInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_PREPARE_GIFT_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kEBILL_GIFT_ITEM_REQ )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_PREPARE_GIFT_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kEBILL_GIFT_ITEM_REQ )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_INSERT_CASH_DECREASE_ERROR_LOG_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_iResultCode )
		&& PUT( m_strOutString )
		;
}

SERIALIZE_DEFINE_GET( KDBE_INSERT_CASH_DECREASE_ERROR_LOG_NOT, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_iResultCode )
		&& GET( m_strOutString )
		;
}

//

SERIALIZE_DEFINE_PUT( KPurchaserInfo, obj, ks )
{
	return PUT( m_iServerGroupID )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrUserName )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrUnitNickName )
		&& PUT( m_ucLevel )
		&& PUT( m_wstrIP )
		&& PUT( m_uiPublisherUID )
		&& PUT( m_iChannelingCode )
		;
}

SERIALIZE_DEFINE_GET( KPurchaserInfo, obj, ks )
{
	return GET( m_iServerGroupID )
		&& GET( m_iUserUID )
		&& GET( m_wstrUserName )
		&& GET( m_iUnitUID )
		&& GET( m_wstrUnitNickName )
		&& GET( m_ucLevel )
		&& GET( m_wstrIP )
		&& GET( m_uiPublisherUID )
		&& GET( m_iChannelingCode )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_USE_COUPON_REQ, obj, ks )
{
	return PUT( m_PurchaserInfo )
		&& PUT( m_iCouponType )
		&& PUT( m_wstrSerialCode )
		&& PUT( m_wstrTransaction )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_USE_COUPON_REQ, obj, ks )
{
	return GET( m_PurchaserInfo )
		&& GET( m_iCouponType )
		&& GET( m_wstrSerialCode )
		&& GET( m_wstrTransaction )
		;
}




SERIALIZE_DEFINE_PUT( KBillCouponItemInfo, obj, ks )
{
	return PUT( m_iProductNO )
		&& PUT( m_iCount )
		&& PUT( m_iTranNo )
		;
}


SERIALIZE_DEFINE_GET( KBillCouponItemInfo, obj, ks )
{
	return GET( m_iProductNO )
		&& GET( m_iCount )
		&& GET( m_iTranNo )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_USE_COUPON_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_vecBillCouponItemInfo )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_USE_COUPON_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_vecBillCouponItemInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEBILL_USE_COUPON_RESULT_REQ, obj, ks )
{
	return PUT( m_iGSUID )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrTransaction )
		&& PUT( m_iRet )
		&& PUT( m_iChargeItem )
		&& PUT( m_iChargeItemCnt )
		&& PUT( m_iChargeCash )
		&& PUT( m_iChargeBonus )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_USE_COUPON_RESULT_REQ, obj, ks )
{
	return GET( m_iGSUID )
		&& GET( m_iUserUID )
		&& GET( m_wstrTransaction )
		&& GET( m_iRet )
		&& GET( m_iChargeItem )
		&& GET( m_iChargeItemCnt )
		&& GET( m_iChargeCash )
		&& GET( m_iChargeBonus )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_USE_COUPON_RESULT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iTotalCash )
		&& PUT( m_vecBillCouponItemInfo )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_USE_COUPON_RESULT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iTotalCash )
		&& GET( m_vecBillCouponItemInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_USE_COUPON_RESERVE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kPacketReq )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_USE_COUPON_RESERVE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kPacketReq )
		;
}



SERIALIZE_DEFINE_PUT( KEBILL_PICK_UP_REQ, obj, ks )
{
	return  PUT( m_iTransNo )
		&&	PUT( m_iProductNo )
		&&	PUT( m_iUnitUID )
		&&	PUT( m_mapSocketForCash )
		&&	PUT( m_byteProductKind )
		&&	PUT( m_vecPickUpPackageInfo )
		&&	PUT( m_iSubProductNo )
		&&	PUT( m_iSubProductID )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_PICK_UP_REQ, obj, ks )
{
	return  GET( m_iTransNo )
		&&	GET( m_iProductNo )
		&&	GET( m_iUnitUID )
		&&	GET( m_mapSocketForCash )
		&&	GET( m_byteProductKind )
		&&	GET( m_vecPickUpPackageInfo )
		&&	GET( m_iSubProductNo )
		&&	GET( m_iSubProductID )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_PICK_UP_ACK, obj, ks )
{
	return  PUT( m_iOK ) &&
		PUT( m_iProductNo ) &&
		PUT( m_mapSocketForCash );
}

SERIALIZE_DEFINE_GET( KEBILL_PICK_UP_ACK, obj, ks )
{
	return  GET( m_iOK ) &&
		GET( m_iProductNo ) &&
		GET( m_mapSocketForCash );
}

SERIALIZE_DEFINE_PUT( KELG_EXCHANGE_CASH_REQ, obj, ks )
{
	return PUT( m_PurchaserInfo )
		&& PUT( m_iPoint )
		;
}

SERIALIZE_DEFINE_GET( KELG_EXCHANGE_CASH_REQ, obj, ks )
{
	return GET( m_PurchaserInfo )
		&& GET( m_iPoint )
		;
}

SERIALIZE_DEFINE_PUT( KELG_EXCHANGE_CASH_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPoint )
		;
}

SERIALIZE_DEFINE_GET( KELG_EXCHANGE_CASH_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPoint )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_CHECK_BALANCE_REQ, obj, ks )
{
	return PUT( m_iPublisherCashBalance )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_CHECK_BALANCE_REQ, obj, ks )
{
	return GET( m_iPublisherCashBalance )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_EXCHANGE_CASH_RESERVE_REQ, obj, ks )
{
	return PUT( m_PurchaserInfo )
		&& PUT( m_iPoint )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_EXCHANGE_CASH_RESERVE_REQ, obj, ks )
{
	return GET( m_PurchaserInfo )
		&& GET( m_iPoint )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_EXCHANGE_CASH_RESERVE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrTransaction )
		&& PUT( m_kPacketReq )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_EXCHANGE_CASH_RESERVE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrTransaction )
		&& GET( m_kPacketReq )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_EXCHANGE_CASH_REQ, obj, ks )
{
	return PUT( m_iGSUID )
		&& PUT( m_iUserUID )
		&& PUT( m_wstrTransaction )
		&& PUT( m_iRet )
		&& PUT( m_iPoint )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_EXCHANGE_CASH_REQ, obj, ks )
{
	return GET( m_iGSUID )
		&& GET( m_iUserUID )
		&& GET( m_wstrTransaction )
		&& GET( m_iRet )
		&& GET( m_iPoint )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_EXCHANGE_CASH_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iUnitUID )
		&& PUT( m_iTotalCash )
		&& PUT( m_iChargeCash )
		&& PUT( m_iChargeBonus )
		&& PUT( m_iBonusItem )
		&& PUT( m_RemainCashInfo )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_EXCHANGE_CASH_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iUnitUID )
		&& GET( m_iTotalCash )
		&& GET( m_iChargeCash )
		&& GET( m_iChargeBonus )
		&& GET( m_iBonusItem )
		&& GET( m_RemainCashInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_GET_TRANS_FOR_CHECK_BALANCE_REQ, obj, ks )
{
	return PUT( m_kCheckGiantBalanceReq )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_GET_TRANS_FOR_CHECK_BALANCE_REQ, obj, ks )
{
	return GET( m_kCheckGiantBalanceReq )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_GET_TRANS_FOR_CHECK_BALANCE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrTransaction )
		&& PUT( m_kCheckGiantBalanceReq )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_GET_TRANS_FOR_CHECK_BALANCE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrTransaction )
		&& GET( m_kCheckGiantBalanceReq )
		;
}

//{{ 허상형 : [2010/8/19/] //	선물 기능 개편 SERV_GLOBAL_BILLING. 유럽에 추가 조효진
SERIALIZE_DEFINE_PUT( KDBE_GET_NICKNAME_BY_UNITUID_REQ, obj, ks )
{
	return PUT( m_vecUID )
		&& PUT( m_vecBillOrderInfo )
		&&	PUT( m_vecPackageInfo )
		&&	PUT( m_vecSubPackageTrans )
		&&	PUT( m_vecSubPackageInfo )
		;
}

SERIALIZE_DEFINE_GET( KDBE_GET_NICKNAME_BY_UNITUID_REQ, obj, ks )
{
	return GET( m_vecUID )
		&& GET( m_vecBillOrderInfo )
		&&	GET( m_vecPackageInfo )
		&&	GET( m_vecSubPackageTrans )
		&&	GET( m_vecSubPackageInfo )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_GET_NICKNAME_BY_UNITUID_ACK, obj, ks )
{
	return PUT( m_mapNickName )
		&& PUT( m_vecBillOrderInfo )
		&&	PUT( m_vecPackageInfo )
		&&	PUT( m_vecSubPackageTrans )
		&&	PUT( m_vecSubPackageInfo )
		;
}

SERIALIZE_DEFINE_GET( KDBE_GET_NICKNAME_BY_UNITUID_ACK, obj, ks )
{
	return GET( m_mapNickName )
		&& GET( m_vecBillOrderInfo )
		&&	GET( m_vecPackageInfo )
		&&	GET( m_vecSubPackageTrans )
		&&	GET( m_vecSubPackageInfo )
		;
}
//}} 허상형 : [2010/8/19/] //	선물 기능 개편
//}}



//}}

//////////////////////////////////////////////////////////////////////////

SERIALIZE_DEFINE_PUT( KEPL_BT_HEALTH_CHECK_REQ, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode );
}

SERIALIZE_DEFINE_GET( KEPL_BT_HEALTH_CHECK_REQ, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode );
}

SERIALIZE_DEFINE_PUT( KEPL_BT_HEALTH_CHECK_ACK, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode );
}

SERIALIZE_DEFINE_GET( KEPL_BT_HEALTH_CHECK_ACK, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode );
}

SERIALIZE_DEFINE_PUT( KEPL_BT_BALANCE_CHECK_REQ, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode ) &&

		PUT( m_ulUserNo ) &&
		PUT( m_wstrUserID ) &&

		PUT( m_ulRealCash ) &&
		PUT( m_ulBonusCash )
		;
}

SERIALIZE_DEFINE_GET( KEPL_BT_BALANCE_CHECK_REQ, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode ) &&

		GET( m_ulUserNo ) &&
		GET( m_wstrUserID ) &&

		GET( m_ulRealCash ) &&
		GET( m_ulBonusCash )
		;
}

SERIALIZE_DEFINE_PUT( KEPL_BT_BALANCE_CHECK_ACK, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode ) &&

		PUT( m_ulUserNo ) &&
		PUT( m_wstrUserID ) &&

		PUT( m_ulRealCash ) &&
		PUT( m_ulBonusCash ) &&

		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		PUT( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEPL_BT_BALANCE_CHECK_ACK, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode ) &&

		GET( m_ulUserNo ) &&
		GET( m_wstrUserID ) &&

		GET( m_ulRealCash ) &&
		GET( m_ulBonusCash ) &&

		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		GET( m_wstrRetMsg )
		//}}
		;
}


SERIALIZE_DEFINE_PUT( KEPL_BT_PURCHASEITEM_ACK, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode ) &&

		PUT( m_wstrOrderID ) &&
		PUT( m_iServerGroupID ) &&
		PUT( m_wstrUserID ) &&
		PUT( m_iUserUID ) &&
		PUT( m_wstrNickName ) &&
		PUT( m_strUserIP ) &&
		PUT( m_vecBillBuyInfo ) &&
		PUT(m_bResurrectionStone) &&
		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		PUT( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEPL_BT_PURCHASEITEM_ACK, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode ) &&

		GET( m_wstrOrderID ) &&
		GET( m_iServerGroupID ) &&
		GET( m_wstrUserID ) &&
		GET( m_iUserUID ) &&
		GET( m_wstrNickName ) &&
		GET( m_strUserIP ) &&
		GET( m_vecBillBuyInfo ) &&
		GET(m_bResurrectionStone) &&
		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		GET( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEPL_BT_GIFTITEM_REQ, obj, ks )
{
	return   PUT( m_ulReqKey ) &&

		PUT( m_wstrOrderID ) &&
		PUT( m_iServerGroupID ) &&
		PUT( m_wstrSenderUserID ) &&
		PUT( m_wstrSenderNickName ) &&
		PUT( m_wstrReceiverUserID ) &&
		PUT( m_iSenderUnitUID ) &&
		PUT( m_iReceiverUID ) &&
		PUT( m_wstrPresentMessage ) &&
		PUT( m_strUserIP ) &&
		PUT( m_vecBillBuyInfo )
		;
}

SERIALIZE_DEFINE_GET( KEPL_BT_GIFTITEM_REQ, obj, ks )
{
	return  GET( m_ulReqKey ) &&

		GET( m_wstrOrderID ) &&
		GET( m_iServerGroupID ) &&
		GET( m_wstrSenderUserID ) &&
		GET( m_wstrSenderNickName ) &&
		GET( m_wstrReceiverUserID ) &&
		GET( m_iSenderUnitUID ) &&
		GET( m_iReceiverUID ) &&
		GET( m_wstrPresentMessage ) &&
		GET( m_strUserIP ) &&
		GET( m_vecBillBuyInfo )
		;
}

SERIALIZE_DEFINE_PUT( KEPL_BT_GIFTITEM_ACK, obj, ks )
{
	return  PUT( m_usReqLen ) &&
		PUT( m_usReqType ) &&
		PUT( m_ulReqKey ) &&
		PUT( m_usRetCode ) &&

		PUT( m_wstrOrderID ) &&
		PUT( m_iServerGroupID ) &&
		PUT( m_wstrUserID ) &&
		PUT( m_iUserUID ) &&
		PUT( m_wstrNickName ) &&
		PUT( m_strUserIP ) &&
		PUT( m_vecBillBuyInfo ) &&

		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		PUT( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_GET( KEPL_BT_GIFTITEM_ACK, obj, ks )
{
	return  GET( m_usReqLen ) &&
		GET( m_usReqType ) &&
		GET( m_ulReqKey ) &&
		GET( m_usRetCode ) &&

		GET( m_wstrOrderID ) &&
		GET( m_iServerGroupID ) &&
		GET( m_wstrUserID ) &&
		GET( m_iUserUID ) &&
		GET( m_wstrNickName ) &&
		GET( m_strUserIP ) &&
		GET( m_vecBillBuyInfo ) &&

		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		GET( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_PUT( KEPL_BT_REGCOUPON_ACK, obj, ks )
{
	return PUT( m_usReqLen )
		&& PUT( m_usReqType )
		&& PUT( m_ulReqKey )
		&& PUT( m_usRetCode )
		&& PUT( m_iUserUID )
		&& PUT( m_vecBillBuyInfo )
		//&& PUT( m_iItemID )
		//&& PUT( m_iItemCount )

		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		&& PUT( m_wstrRetMsg )
		//}}
		;
}

SERIALIZE_DEFINE_GET(  KEPL_BT_REGCOUPON_ACK, obj, ks )
{
	return GET( m_usReqLen )
		&& GET( m_usReqType )
		&& GET( m_ulReqKey )
		&& GET( m_usRetCode )
		&& GET( m_iUserUID )
		&& GET( m_vecBillBuyInfo )
		//&& GET( m_iItemID )
		//&& GET( m_iItemCount )
		//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
		&& GET( m_wstrRetMsg )
		//}}
		;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//}}

//{{ 허상형 : [2010/8/31/] //	부활석 통계 분리 작업
//}} 허상형 : [2010/8/31/] //	부활석 통계 분리 작업

//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
//}}

//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}

//{{ 2011. 12. 09	박교현		서버군 리스트
SERIALIZE_DEFINE_PUT( KDBE_SERVERGROUP_LIST_ACK, obj, ks )
{
	return PUT( m_mapServerGroupList )
		;
};

SERIALIZE_DEFINE_GET( KDBE_SERVERGROUP_LIST_ACK, obj, ks )
{
	return GET( m_mapServerGroupList )
		;
};
//}}

SERIALIZE_DEFINE_PUT( KERM_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_kFieldUserInfo )
		;
};
SERIALIZE_DEFINE_GET( KERM_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_kFieldUserInfo )
		;
};
SERIALIZE_DEFINE_PUT( KETR_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return PUT( m_vecUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_kFieldUserInfo )
		;
};
SERIALIZE_DEFINE_GET( KETR_TOGGLE_INVISIBLE_NOT, obj, ks )
{
	return GET( m_vecUID )
		&& GET( m_iUnitUID )
		&& GET( m_kFieldUserInfo )
		;
};

SERIALIZE_DEFINE_PUT( KTradeShareItemInfo, obj, ks )
{
	return  PUT( m_iItemUID )
		&&	PUT( m_iFromUnitUID )
		&&	PUT( m_iToUnitUID )
		&&	PUT( m_cInventoryCategory )
		&&	PUT( m_sSlotID )
		&&	PUT( m_ucSealCnt )
		&&	PUT( m_ucDeleteReason )
		&&	PUT( m_iItemID )
		&&	PUT( m_iNewItemUID )
		;
}

SERIALIZE_DEFINE_GET( KTradeShareItemInfo, obj, ks )
{
	return  GET( m_iItemUID )
		&&	GET( m_iFromUnitUID )
		&&	GET( m_iToUnitUID )
		&&	GET( m_cInventoryCategory )
		&&	GET( m_sSlotID )
		&&	GET( m_ucSealCnt )
		&&	GET( m_ucDeleteReason )
		&&	GET( m_iItemID )
		&&	GET( m_iNewItemUID )
		;
}

SERIALIZE_DEFINE_PUT( KUpdateShareItemInfo, obj, ks )
{
	return  PUT( m_iItemUID )
		&&	PUT( m_iUnitUID )
		&&	PUT( m_iUsageType )
		&&	PUT( m_iValue )
		;
}

SERIALIZE_DEFINE_GET( KUpdateShareItemInfo, obj, ks )
{
	return  GET( m_iItemUID )
		&&	GET( m_iUnitUID )
		&&	GET( m_iUsageType )
		&&	GET( m_iValue )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_UPDATE_SHARE_ITEM_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_mapUpdatePosition )
		&& PUT( m_mapInItem )
		&& PUT( m_mapOutItem )
		&& PUT( m_mapUpdateEndurance )
		&& PUT( m_mapUpdateQuantity )
		&& PUT( m_vecDeletedItem )
		&& PUT( m_bReload )
		&& PUT( m_wstrReloadNickname )
		&& PUT( m_vecInventorySlotItem )
		;
}
SERIALIZE_DEFINE_GET( KDBE_UPDATE_SHARE_ITEM_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_mapUpdatePosition )
		&& GET( m_mapInItem )
		&& GET( m_mapOutItem )
		&& GET( m_mapUpdateEndurance )
		&& GET( m_mapUpdateQuantity )
		&& GET( m_vecDeletedItem )
		&& GET( m_bReload )
		&& GET( m_wstrReloadNickname )
		&& GET( m_vecInventorySlotItem )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_UPDATE_SHARE_ITEM_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_bReload )
		&& PUT( m_wstrReloadNickname )
		&& PUT( m_vecTradeShareItemResult )
		&& PUT( m_vecInventorySlotItem )
		;
}
SERIALIZE_DEFINE_GET( KDBE_UPDATE_SHARE_ITEM_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_bReload )
		&& GET( m_wstrReloadNickname )
		&& GET( m_vecTradeShareItemResult )
		&& GET( m_vecInventorySlotItem )
		;
}

SERIALIZE_DEFINE_PUT( KELOG_ITEM_TRADE_ERROR_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iType )
		&& PUT( m_wsrtRegDate )
		;
}
SERIALIZE_DEFINE_GET( KELOG_ITEM_TRADE_ERROR_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iType )
		&& PUT( m_wsrtRegDate )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_CHANGE_INVENTORY_SLOT_ITEM_REQ, obj, ks )
{
	return PUT( m_cFromSlotType )
		&& PUT( m_iFromSlotID )
		&& PUT( m_cToSlotType )
		&& PUT( m_iToSlotID )
		&& PUT( m_iUnitUID )
		&& PUT( m_kItemQuantityUpdate )
		&& PUT( m_kItemEnduranceUpdate )
		&& PUT( m_kItemPositionUpdate )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHANGE_INVENTORY_SLOT_ITEM_REQ, obj, ks )
{
	return GET( m_cFromSlotType )
		&& GET( m_iFromSlotID )
		&& GET( m_cToSlotType )
		&& GET( m_iToSlotID )
		&& GET( m_iUnitUID )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_kItemEnduranceUpdate )
		&& GET( m_kItemPositionUpdate )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_CHANGE_INVENTORY_SLOT_ITEM_ACK, obj, ks )
{
	return PUT( m_cFromSlotType )
		&& PUT( m_iFromSlotID )
		&& PUT( m_cToSlotType )
		&& PUT( m_iToSlotID )
		&& PUT( m_kItemQuantityUpdate )
		&& PUT( m_kItemEnduranceUpdate )
		&& PUT( m_kItemPositionUpdate )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHANGE_INVENTORY_SLOT_ITEM_ACK, obj, ks )
{
	return GET( m_cFromSlotType )
		&& GET( m_iFromSlotID )
		&& GET( m_cToSlotType )
		&& GET( m_iToSlotID )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_kItemEnduranceUpdate )
		&& GET( m_kItemPositionUpdate )
		;
}

//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
SERIALIZE_DEFINE_PUT( KDBE_SHARING_BACK_OPEN_REQ, obj, ks )
{
	return PUT(m_iUserUID)
		&& PUT(m_iUnitUID)
		&& PUT(m_iOpenType)
		;
};

SERIALIZE_DEFINE_GET( KDBE_SHARING_BACK_OPEN_REQ, obj, ks )
{
	return GET(m_iUserUID)
		&& GET(m_iUnitUID)
		&& GET(m_iOpenType)
		;
};

SERIALIZE_DEFINE_PUT( KDBE_SHARING_BACK_OPEN_ACK, obj, ks )
{
	return PUT(m_iOK)
		&& PUT(m_iOpenType)
		;
};

SERIALIZE_DEFINE_GET( KDBE_SHARING_BACK_OPEN_ACK, obj, ks )
{
	return GET(m_iOK)
		&& GET(m_iOpenType)
		;
};
//}}

SERIALIZE_DEFINE_PUT( KDBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_vecCompletedEvent )
		;
}

SERIALIZE_DEFINE_GET( KDBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_vecCompletedEvent )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_CHECK_EVENT_UPDATE_ACK, obj, ks )
{
	return PUT( m_mapReleaseTick )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHECK_EVENT_UPDATE_ACK, obj, ks )
{
	return GET( m_mapReleaseTick )
		;
}


//{{ 2013. 05. 14	최육사	아이템 개편
SERIALIZE_DEFINE_PUT( KDBE_CHECK_BLOCK_LIST_REQ, obj, ks )
{
	return PUT( m_iReleaseTick )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHECK_BLOCK_LIST_REQ, obj, ks )
{
	return GET( m_iReleaseTick )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_CHECK_BLOCK_LIST_ACK, obj, ks )
{
	return PUT( m_iReleaseTick )
		&& PUT( m_mapBlockInfo )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHECK_BLOCK_LIST_ACK, obj, ks )
{
	return GET( m_iReleaseTick )
		&& GET( m_mapBlockInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEGB_CHECK_BLOCK_USER_NOT, obj, ks )
{
	return PUT( m_vecBlockType )
		&& PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrIP )
		;
}

SERIALIZE_DEFINE_GET( KEGB_CHECK_BLOCK_USER_NOT, obj, ks )
{
	return GET( m_vecBlockType )
		&& GET( m_iUserUID )
		&& GET( m_iUnitUID )
		&& GET( m_wstrIP )
		;
}


SERIALIZE_DEFINE_PUT( KEGB_UPDATE_BLOCK_RESULT_NOT, obj, ks )
{
	return PUT( m_mapBlockResult )
		;
}

SERIALIZE_DEFINE_GET( KEGB_UPDATE_BLOCK_RESULT_NOT, obj, ks )
{
	return GET( m_mapBlockResult )
		;
}
//}}
SERIALIZE_DEFINE_PUT( KDBE_INSERT_WARP_VIP_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPeriod )
		;
}


SERIALIZE_DEFINE_GET( KDBE_INSERT_WARP_VIP_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPeriod )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_INSERT_WARP_VIP_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrEndDate )
		;
}


SERIALIZE_DEFINE_GET( KDBE_INSERT_WARP_VIP_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrEndDate )
		;
}










//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//#ifdef SERV_RELAY_DB_CONNECTION
SERIALIZE_DEFINE_PUT( KDBE_UPDATE_USER_INFO_TO_RELAY_DB_JP_EVENT_NOT, obj, ks )
{
	return PUT( m_wstrUserID )
		&& PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_wstrNickName )
		&& PUT( m_cLevel )
		&& PUT( m_cUnitClass )
		&& PUT( m_bDeletedUnit )
		&& PUT( m_wstrUserRegDate )
		&& PUT( m_wstrUnitRegDate )
		&& PUT( m_iUpdateType )
		;
}

SERIALIZE_DEFINE_GET( KDBE_UPDATE_USER_INFO_TO_RELAY_DB_JP_EVENT_NOT, obj, ks )
{
	return GET( m_wstrUserID )
		&& GET( m_iUserUID )
		&& GET( m_iUnitUID )
		&& GET( m_wstrNickName )
		&& GET( m_cLevel )
		&& GET( m_cUnitClass )
		&& GET( m_bDeletedUnit )
		&& GET( m_wstrUserRegDate )
		&& GET( m_wstrUnitRegDate )
		&& GET( m_iUpdateType )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_REQ, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_REQ, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_iUnitUID )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_ACK, obj, ks )
{
	return PUT( m_vecRewardInfo )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_ACK, obj, ks )
{
	return GET( m_vecRewardInfo )
		;
}


SERIALIZE_DEFINE_PUT( KEBILL_REWARD_COUPON_JP_EVENT_REQ, obj, ks )
{
	return PUT( m_kRewardInfo )
		&& PUT( m_kUseCouponPacket )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_REWARD_COUPON_JP_EVENT_REQ, obj, ks )
{
	return GET( m_kRewardInfo )
		&& GET( m_kUseCouponPacket )
		;
}

SERIALIZE_DEFINE_PUT( KEBILL_REWARD_COUPON_JP_EVENT_ACK, obj, ks )
{
	return PUT( m_kRewardInfo )
		&& PUT( m_kUseCouponPacket )
		;
}

SERIALIZE_DEFINE_GET( KEBILL_REWARD_COUPON_JP_EVENT_ACK, obj, ks )
{
	return GET( m_kRewardInfo )
		&& GET( m_kUseCouponPacket )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_UPDATE_REWARD_COMPLETE_JP_EVENT_NOT, obj, ks )
{
	return PUT( m_iRID )
		;
}

SERIALIZE_DEFINE_GET( KDBE_UPDATE_REWARD_COMPLETE_JP_EVENT_NOT, obj, ks )
{
	return GET( m_iRID )
		;
}
//#endif SERV_RELAY_DB_CONNECTION
//}}

SERIALIZE_DEFINE_PUT( KDBE_CHANGE_PET_ID_REQ, obj, ks )
{
	return PUT( m_iPetUID )
		&& PUT( m_iBeforePetID )
		&& PUT( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHANGE_PET_ID_REQ, obj, ks )
{
	return GET( m_iPetUID )
		&& GET( m_iBeforePetID )
		&& GET( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_CHANGE_PET_ID_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iPetUID )
		&& PUT( m_iBeforePetID )
		&& PUT( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHANGE_PET_ID_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iPetUID )
		&& GET( m_iBeforePetID )
		&& GET( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_CHANGE_PET_ID_LOG_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iPetUID )
		&& PUT( m_iBeforePetID )
		&& PUT( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_GET( KDBE_CHANGE_PET_ID_LOG_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iPetUID )
		&& GET( m_iBeforePetID )
		&& GET( m_iAfterPetID )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_RELEASE_PET_REQ, obj, ks )
{
	return PUT( m_iPetUID )
		&& PUT( m_iUnitUID )
		;
}
SERIALIZE_DEFINE_GET( KDBE_RELEASE_PET_REQ, obj, ks )
{
	return GET( m_iPetUID )
		&& GET( m_iUnitUID )
		;
}




SERIALIZE_DEFINE_PUT( KDBE_READY_TO_SOSUN_EVENT_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_kItemQuantityUpdate )
		&& PUT( m_vecUpdatedInventorySlot )
		&& PUT( m_vecItemInfo )
		&& PUT( m_iFirstUnitClass )
		;
}
SERIALIZE_DEFINE_GET( KDBE_READY_TO_SOSUN_EVENT_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_mapInsertedItem )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_vecUpdatedInventorySlot )
		&& GET( m_vecItemInfo )
		&& GET( m_iFirstUnitClass )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_READY_TO_SOSUN_EVENT_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_mapInsertedItem )
		&& PUT( m_kItemQuantityUpdate )
		&& PUT( m_vecUpdatedInventorySlot )
		&& PUT( m_mapItemInfo )
		&& PUT( m_iFirstUnitClass )
		;
}
SERIALIZE_DEFINE_GET( KDBE_READY_TO_SOSUN_EVENT_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_mapInsertedItem )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_vecUpdatedInventorySlot )
		&& GET( m_mapItemInfo )
		&& GET( m_iFirstUnitClass )
		;
}


SERIALIZE_DEFINE_PUT( KDBE_EVENT_PROPOSE_USER_FIND_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_wstrOtherNickName )
		&& PUT( m_iOtherUserUID )
		&& PUT( m_iOtherUnitUID )
		&& PUT( m_cUnitClass )
		&& PUT( m_bCouple )
		;
}

SERIALIZE_DEFINE_GET( KDBE_EVENT_PROPOSE_USER_FIND_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_wstrOtherNickName )
		&& GET( m_iOtherUserUID )
		&& GET( m_iOtherUnitUID )
		&& GET( m_cUnitClass )
		&& GET( m_bCouple )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_EVENT_MAKING_SUCCESS_ACCEPTOR_REQ, obj, ks )
{
	return PUT( m_vecUpdatedInventorySlot )
		&& PUT( m_vecDeleted )
		;
}

SERIALIZE_DEFINE_GET( KDBE_EVENT_MAKING_SUCCESS_ACCEPTOR_REQ, obj, ks )
{
	return GET( m_vecUpdatedInventorySlot )
		&& GET( m_vecDeleted )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_EVENT_MAKING_SUCCESS_ACCEPTOR_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_kItemQuantityUpdate )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KDBE_EVENT_MAKING_SUCCESS_ACCEPTOR_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_EVENT_DIVORCE_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iRelationTargetUserUid )
		&& PUT( m_vecUpdatedInventorySlot )
		&& PUT( m_vecDeleted )
		;
}

SERIALIZE_DEFINE_GET( KDBE_EVENT_DIVORCE_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iRelationTargetUserUid )
		&& GET( m_vecUpdatedInventorySlot )
		&& GET( m_vecDeleted )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_EVENT_DIVORCE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_iRelationTargetUserUid )
		&& PUT(	m_kItemQuantityUpdate )
		&& PUT( m_vecUpdatedInventorySlot )
		;
}

SERIALIZE_DEFINE_GET( KDBE_EVENT_DIVORCE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_iRelationTargetUserUid )
		&& GET( m_kItemQuantityUpdate )
		&& GET( m_vecUpdatedInventorySlot )
		;
}




SERIALIZE_DEFINE_PUT( KDBE_2013_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_ucLevel )
		;
}
SERIALIZE_DEFINE_GET( KDBE_2013_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_ucLevel )
		;
}
SERIALIZE_DEFINE_PUT( KDBE_2013_EVENT_MISSION_COMPLETE_ACK, obj, ks )
{
	return PUT( m_iOK )
		&& PUT( m_ucLevel )
		;
}
SERIALIZE_DEFINE_GET( KDBE_2013_EVENT_MISSION_COMPLETE_ACK, obj, ks )
{
	return GET( m_iOK )
		&& GET( m_ucLevel )
		;
}
SERIALIZE_DEFINE_PUT( KDBE_2014_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return PUT( m_iUserUID )
		&& PUT( m_iUnitUID )
		&& PUT( m_iNewYearMissionStepID )
		;
}
SERIALIZE_DEFINE_GET( KDBE_2014_EVENT_MISSION_COMPLETE_REQ, obj, ks )
{
	return GET( m_iUserUID )
		&& GET( m_iUnitUID )
		&& GET( m_iNewYearMissionStepID )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT, obj, ks )
{
	return PUT( m_CurrentItemTempletNameMap )
		;
}

SERIALIZE_DEFINE_GET( KDBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT, obj, ks )
{
	return GET( m_CurrentItemTempletNameMap )
		;
}


//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
SERIALIZE_DEFINE_PUT( KEGB_GET_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return PUT( m_iServerUID )
		;
}
SERIALIZE_DEFINE_GET( KEGB_GET_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return GET( m_iServerUID )
		;
}

SERIALIZE_DEFINE_PUT( KEGB_GET_GLOBAL_MISSION_INFO_ACK, obj, ks )
{
	return PUT( m_iFlag )
		&& PUT( m_wstrGlobalMissionDay )
		&& PUT( m_mapGlobalMissionInfo )
		;
}
SERIALIZE_DEFINE_GET( KEGB_GET_GLOBAL_MISSION_INFO_ACK, obj, ks )
{
	return GET( m_iFlag )
		&& GET( m_wstrGlobalMissionDay )
		&& GET( m_mapGlobalMissionInfo )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_GET_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return PUT( m_mapGlobalMissionInfo )
		;
}
SERIALIZE_DEFINE_GET( KDBE_GET_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return GET( m_mapGlobalMissionInfo )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_UPDATE_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return PUT( m_wstrGlobalMissionDay )
		&& PUT( m_iNowCount )
		;
}
SERIALIZE_DEFINE_GET( KDBE_UPDATE_GLOBAL_MISSION_INFO_REQ, obj, ks )
{
	return GET( m_wstrGlobalMissionDay )
		&& GET( m_iNowCount )
		;
}

SERIALIZE_DEFINE_PUT( KEGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT, obj, ks )
{

	return PUT( m_mapGlobalMissionStartTime )
		;
}
SERIALIZE_DEFINE_GET( KEGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT, obj, ks )
{
	return GET( m_mapGlobalMissionStartTime )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_INSERT_CONNECT_EVENT_REQ, obj, ks )
{
	return PUT( m_wstrEventName )
		&& PUT( m_iEventType )
		&& PUT( m_wstrNextDayStartTime )
		&& PUT( m_wstrNextDayEndTime )
		&& PUT( m_iScriptID )
		;
}
SERIALIZE_DEFINE_GET( KDBE_INSERT_CONNECT_EVENT_REQ, obj, ks )
{
	return GET( m_wstrEventName )
		&& GET( m_iEventType )
		&& GET( m_wstrNextDayStartTime )
		&& GET( m_wstrNextDayEndTime )
		&& GET( m_iScriptID )
		;
}

SERIALIZE_DEFINE_PUT( KDBE_REGIST_GLOBAL_MISSION_CLEAR_NOT, obj, ks )
{
	return PUT( m_wstrGlobalMissionDay )
		&& PUT( m_iUnitUID )
		;
}
SERIALIZE_DEFINE_GET( KDBE_REGIST_GLOBAL_MISSION_CLEAR_NOT, obj, ks )
{
	return GET( m_wstrGlobalMissionDay )
		&& GET( m_iUnitUID )
		;
}
//}} 2012. 09. 03	임홍락	글로벌 미션 매니저


SERIALIZE_DEFINE_PUT( KDBE_UNLIMITED_SECOND_CHANGE_JOB_NOT, obj, ks )
{
	return PUT( m_iUnitUID )
		&& PUT( m_iOldUnitClass )
		&& PUT( m_iNewUnitClass )
		;
}

SERIALIZE_DEFINE_GET( KDBE_UNLIMITED_SECOND_CHANGE_JOB_NOT, obj, ks )
{
	return GET( m_iUnitUID )
		&& GET( m_iOldUnitClass )
		&& GET( m_iNewUnitClass )
		;
}


//}}
//}}
