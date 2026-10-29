#pragma once

#include "CommonPacket.h"
#include "ClientPacket.h"


#pragma pack( push, 1 )


#define MAX_PACKET_SIZE_OF_GASH_AUTH 1024

DECL_DATA( KLoginResultInfo )
{
	enum LOGIN_RESULT_TYPE_ENUM		// 로그인 결과 사유
	{
		// 중국 거 포함 시켜 업그레이드 함
		LRTE_SUCCESS = 0,						// 로그인 성공
		LRTE_FAIL_UNKNOWN,						// 알 수 없는 타입의 로그인 실패
		LRTE_FAIL_REJECT,						// 특이 거부 (빈펀으로 로그인하라 등)
		LRTE_FAIL_SYSTEM_ERROR,					// 시스템 에러
		LRTE_FAIL_ACCOUNT_NOT_EXIST,			// 계정 부재
		LRTE_FAIL_PASSWORD_ERROR,				// 계정 패스워드 에러
		LRTE_FAIL_OTP_PASSWORD_ERROR,			// 자이언트 OTP 번호 에러
		LRTE_FAIL_ACCOUNT_BLOCK,				// 계정 차단
		LRTE_FAIL_MATRIX_NUMBER_ERROR,			// 보안 카드 번호 에러
		LRTE_FAIL_ACCOUNT_NOT_ACTIVATE,			// 계정 미 활성화

		LRTE_SENTINEL,							// 끝. 사유 추가는 위에 하세요.
	};

	int											m_iServerGroupID;		// 서버군 ID
	UidType										m_uiServerUID;			// 서버 UID
	std::vector<int>							m_vecResultCount;		// 0 : 성공, 1 : 실패(알 수 없음), 2 : 실패(아이디 또는 패스워드 오입력), 3 : 실패(퍼블리셔 인증 서버 연결 실패)

	KLoginResultInfo()
	{
		m_iServerGroupID = 0;
		m_uiServerUID = 0;
		m_vecResultCount.clear();
	}
};

typedef KLoginResultInfo						KDBE_INSERT_LOGIN_RESULT_INFO_REQ;



DECL_PACKET( ELG_ADMIN_DELETE_GUILD_ADD_NOT )
{
	int						m_iGuildUID;

};

//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
DECL_PACKET( ELG_ADMIN_DELETE_HENIR_RANKING_RANK_NOT )
{
	int						m_iRankingType;
	int						m_iRank;

};

DECL_PACKET( ELG_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT )
{
	int						m_iRankingType;
	std::wstring			m_wstrNickName;

};

DECL_PACKET( ELG_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT )
{
	int						m_iRankingType;
	UidType 				m_iUnitUID;

};
//}}


//====================================================================//
//====================================================================//
//====================================================================//
//{{ 2013.03.19 조효진 해외 공용 인증 시스템 구성



DECL_PACKET( EPUBLISHER_AUTHENTICATION_REQ )
{
	UidType						m_iActorUID;
	std::wstring				m_wstrIP;
	std::wstring				m_wstrServiceAccountID;
	std::wstring				m_wstrServicePassword;
	int							m_iChannelingCode;

	bool						m_bServerUseKogOTP;		// KogOTP 사용여부. 서버에서 세팅하는 값





	KEPUBLISHER_AUTHENTICATION_REQ()
	{
		m_iActorUID				= -1;
		m_wstrIP				= L"";
		m_wstrServiceAccountID	= L"";
		m_wstrServicePassword	= L"";
		m_iChannelingCode		= 0;
		m_bServerUseKogOTP = false;



	}
};


DECL_PACKET( EPUBLISHER_AUTHENTICATION_ACK )
{
	int							m_iOK;
	int							m_iResult;

	std::wstring				m_wstrID;
	unsigned int				m_uiPublisherUID;
	std::wstring				m_wstrIP;
	int							m_iChannelingCode;
	std::wstring				wstrPurchTok;

	unsigned int				m_uiAccountType;
	unsigned int				m_uiAccountState;

	std::string					m_strAgreementURL;


	KEPUBLISHER_AUTHENTICATION_ACK()
	{
		m_iOK					= -1;
		m_iResult				= 0;

		m_wstrID				= L"";

		m_uiPublisherUID		= 0;
		m_wstrIP				= L"";
		m_iChannelingCode		= 0;

		m_uiAccountType			= 0;
		m_uiAccountState		= 0;

		m_strAgreementURL		= "";

	}
};


//{{ 2011.1.24 조효진 KOG_OTP_MODIFY

DECL_PACKET( DBE_CH_USER_GENERAL_LOGIN_REQ )
{
	std::wstring    m_wstrServiceAccountID;
	std::wstring	m_wstrIP;
	unsigned int	m_uiPublisherUID;

	int				m_iChannelingCode;

	//{{ 2010.12.02 조효진	케쉬샵 입장 시. 로그인 여부 판단을 위해
	std::wstring    m_wstrPurchaseTok;
	//}}

	bool			m_bServerUseKogOTP;		// KogOTP 사용여부. 서버에서 세팅하는 값




	KDBE_CH_USER_GENERAL_LOGIN_REQ()
	{
		m_wstrIP = L"";
		m_uiPublisherUID = 0;
		m_iChannelingCode = 0;
		m_wstrPurchaseTok = L"";

		m_bServerUseKogOTP = false;



	};
};

DECL_PACKET( DBE_CH_USER_GENERAL_LOGIN_ACK )
{
	int							m_iOK;
	std::wstring				m_wstrUserID;
	UidType						m_iUserUID;
	std::wstring				m_wstrOTP;
	unsigned int				m_uiPublisherUID;
	int							m_iChannelingCode;

	//{{ 2010.12.02 조효진	케쉬샵 입장 시. 로그인 여부 판단을 위해 (유럽용)
	std::wstring				m_wstrPurchaseTok;
	//}}

	KDBE_CH_USER_GENERAL_LOGIN_ACK()
	{
		m_uiPublisherUID = 0;
		m_iChannelingCode = 0;
		m_wstrPurchaseTok = L"";
	};
};

DECL_PACKET( DBE_CH_USER_KOGOTP_LOGIN_REQ )
{
	std::wstring    m_wstrServiceAccountID;
	unsigned int	m_uiPublisherUID;
	std::wstring    m_wstrOTP;
	std::wstring	m_wstrIP;

	//{{ 2010.12.02 조효진	케쉬샵 입장 시. 로그인 여부 판단을 위해
	std::wstring    m_wstrPurchaseTok;
	//}}

	int				m_iChannelingCode;

	KDBE_CH_USER_KOGOTP_LOGIN_REQ()
	{
		m_uiPublisherUID = 0;
		m_wstrIP = L"";
		m_wstrPurchaseTok = L"";
		m_iChannelingCode = 0;
	};
};

DECL_PACKET( DBE_CH_USER_KOGOTP_LOGIN_ACK )
{
	int							m_iOK;
	std::wstring				m_wstrUserID;
	UidType						m_iUserUID;
	std::wstring				m_wstrOTP;
	unsigned int				m_uiPublisherUID;

	//{{ 2010.12.02 조효진	케쉬샵 입장 시. 로그인 여부 판단을 위해 (유럽용)
	std::wstring				m_wstrPurchaseTok;
	//}}

	KDBE_CH_USER_KOGOTP_LOGIN_ACK()
	{
		m_uiPublisherUID = 0;
		m_wstrPurchaseTok = L"";
	};
};

DECL_PACKET( ELG_GAME_USER_AUTHENTICATE_REQ )
{
	KUserAuthenticateReq        m_kAuthenticateReq;
	KNexonAccountInfo           m_kNexonAccountInfo;
};

DECL_PACKET( ELG_GAME_USER_AUTHENTICATE_ACK )
{
	int                         m_iOK;
	KAccountInfo                m_kAccountInfo;
	KNexonAccountInfo           m_kNexonAccountInfo;

	char						m_cHackingUserType;
	unsigned int				m_uiGiantAccountType;
	//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
	bool						m_bJoinAccount;
	//}}
	//{{ 2011. 02. 23	최육사	계정 단위 카운트
	std::map< int, int >		m_mapAccCountInfo;
	//}}
};

DECL_PACKET( EPUBLISHER_REPORT_HACK_USER_NOT )
{
	UidType								m_iUserUID;
	std::wstring						m_wstrUserID;
	UidType								m_iUnitUID;
	std::wstring						m_wstrNickName;
	UidType								m_iServerUID;
	std::string							m_strIP;
};

//////////////////////////////////////////////////////////////////////////


//#ifdef SERV_COUNTRY_CN

DECL_PACKET( EGIANT_COMMON_INITIALIZE_REQ )
{
	std::wstring						m_wstrIP;
	unsigned short						m_usPort;
};

DECL_PACKET( EGIANT_COMMON_INITIALIZE_ACK )
{
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	std::wstring						m_wstrZoneName;					// [32]
	BYTE								m_byteNetType;
};

DECL_PACKET( EGIANT_COMMON_NULL_SERVER )
{
};

DECL_PACKET( EGIANT_COMMON_NULL_CLIENT )
{
};

DECL_PACKET( EGIANT_AUTH_LOGIN_SUCCESS )
{
	unsigned int						m_uiRequestID;
	unsigned int						m_uiUID;
	std::wstring						m_wstrName;						// [48]
	unsigned int						m_uiType;
	std::wstring						m_wstrEMail;					// [48]
	unsigned int						m_uiState;
	unsigned int						m_uiBindType;
	std::wstring						m_wstrSuperpasswd;				// [16]
	std::wstring						m_wstrMobileAccount;			// [48]
};

DECL_PACKET( EGIANT_AUTH_LOGIN_FAIL )
{
	unsigned int						m_uiRequestID;
	int									m_iReturnCode;
	std::wstring						m_wstrName;						// [48]
	unsigned int						m_uiKey;
	std::wstring						m_wstrCoordinate;				// [7]
};


DECL_PACKET( DBE_GIANT_USER_LOGIN_REQ )
{
	bool								m_bLogin;
	std::wstring						m_wstrServiceAccountID;
	unsigned int						m_uiGiantUID;
	std::wstring						m_wstrOTP;
	unsigned int						m_uiAccountType;
	unsigned int						m_uiAccountState;

	KDBE_GIANT_USER_LOGIN_REQ()
	{
		m_uiGiantUID = 0;
		m_uiAccountType = 0;
		m_uiAccountState = 0;
	}
};

DECL_PACKET( DBE_GIANT_USER_LOGIN_ACK )
{
	int									m_iOK;
	std::wstring						m_wstrUserID;
	UidType								m_iUserUID;
	std::wstring						m_wstrOTP;
	unsigned int						m_uiGiantUID;
	unsigned int						m_uiAccountType;
};


//////////////////////////////////////////////////////////////////////////
// 2010. 7. 19	박교현		자이언트 플랫폼 인증


//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}}

DECL_PACKET( EPUBLISHER_BILLING_BALANCE_REQ )
{
	unsigned int						m_uiPublisherUID;
	UidType								m_uiActorUID;
	std::wstring						m_wstrAccount;					// [65]
	std::wstring						m_wstrIP;

	KEPUBLISHER_BILLING_BALANCE_REQ()
	{
		m_uiPublisherUID		= 0;
		m_uiActorUID			= 0;
		m_wstrAccount			= L"";
		m_wstrIP				= L"";

	}
};

DECL_PACKET( EPUBLISHER_BILLING_BALANCE_ACK )
{
	int									m_iOK;
	unsigned long						m_ulBalance;
	int									m_iCashType;

	KEPUBLISHER_BILLING_BALANCE_ACK()
	{
		m_ulBalance = 0;

		m_iCashType = -1;
	}
};



DECL_PACKET( ELG_VERIFY_GIANT_MATRIX_CARD_REQ )
{
	std::wstring						m_wstrID;
	std::wstring						m_wstrMatrix;
	unsigned int						m_uiGiantMatrixKey;
};

typedef KPacketOK		KELG_VERIFY_GIANT_MATRIX_CARD_ACK;
typedef KPacketOK		KELG_VERIFY_GIANT_MATRIX_CARD_NOT;


DECL_PACKET( EGIANT_BILLING_RETURN )
{
	std::wstring						m_wstrTID;						// [33]
	int									m_iBalance;
	int									m_iBonus;
	int									m_iHadFilled;
	int									m_iRet;
	int									m_iSubat;
	int									m_iAT;
	int									m_iPoint;
	unsigned int						m_uiGiantUID;
};



DECL_PACKET( EGIANT_BILLING_USE_CARD_CMD )
{
	short								m_sSource;
	std::wstring						m_wstrAccount;
	std::wstring						m_wstrTID;
	std::wstring						m_wstrCardID;
	std::wstring						m_wstrPasswd;
	std::wstring						m_wstrIP;
	unsigned int						m_uiUID;
	int									m_iAT;
	unsigned int						m_uiConTempID;
	unsigned int						m_uiOperateUID;
};

DECL_PACKET( EGIANT_BILLING_USE_CARD_RET_CMD )
{
	short								m_sSource;
	std::wstring						m_wstrAccount;
	std::wstring						m_wstrTID;
	std::wstring						m_wstrCardID;
	std::wstring						m_wstrPasswd;
	std::wstring						m_wstrIP;
	int									m_iPoint;
	int									m_iRet;
	unsigned int						m_uiUID;
	int									m_iAT;
	unsigned int						m_uiConTempID;
	unsigned int						m_uiOperateUID;
};

DECL_PACKET( EGIANT_BILLING_CONSUME_REQ )
{
	unsigned int						m_uiGiantUID;
	int									m_iPoint;
	int									m_iSource;
	std::wstring						m_wstrTID;						// [33]
	std::wstring						m_wstrRemark;					// [201] 사용하지 않음
	std::wstring						m_wstrIP;						// [17]
	int									m_iAT;
};

DECL_PACKET( EGIANT_BILLING_CONSUME_ACK )
{
	int									m_iOK;
	int									m_iBalance;
	int									m_iBonus;
	bool								m_bHasFilled;
	int									m_iPoint;
	unsigned int						m_uiGiantUID;
};

DECL_PACKET( EGIANT_BILLING_PCARD_CMD )
{
	unsigned int						m_uiGiantUID;
	int									m_iSource;
	std::wstring						m_wstrTID;						// [33]
	std::wstring						m_wstrPCardID;					// [33]
	std::wstring						m_wstrIP;						// [17]
	int									m_iAT;
};

DECL_PACKET( EGIANT_BILLING_SCARD_CMD )
{
	unsigned int						m_uiGiantUID;
	int									m_iSource;
	std::wstring						m_wstrTID;						// [33]
	std::wstring						m_wstrSCardID;					// [33]
	std::wstring						m_wstrIP;						// [17]
	int									m_iAT;
};

//////////////////////////////////////////////////////////////////////////
// 2010.07.07 박교현 / 자이언트 RoleRegServer 인증
DECL_PACKET( EGIANT_ROLEREG )
{
	unsigned short						m_usRegType;
	unsigned short						m_usServerID;
	unsigned int						m_uiGiantUID;
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	std::wstring						m_wstrName;						// [32]
	unsigned int						m_uiRoleUID;
	unsigned short						m_usState;
};


DECL_PACKET( EGIANT_ROLEREG_CREATE_UNIT_REQ )
{
	UidType								m_iActorUID;
	unsigned int						m_uiGiantUID;
	KEGS_CREATE_UNIT_REQ				m_CreateUnitReqPacket;
};

DECL_PACKET( EGIANT_ROLEREG_CREATE_UNIT_ACK )
{
	int									m_iOK;
	KEGS_CREATE_UNIT_REQ				m_CreateUnitReqPacket;
};

DECL_PACKET( EGIANT_ROLEREG_DELETE_UNIT_REQ )
{
	std::wstring						m_wstrNickName;
	unsigned int						m_uiGiantUID;
};

DECL_PACKET( EGIANT_ROLEREG_DELETE_UNIT_ACK )
{
	int									m_iOK;
};

DECL_DATA( KGiantDeletedUnitInfo )
{
	std::wstring						m_wstrNickName;
	UidType								m_iUnitUID;
	unsigned int						m_uiGiantUID;
};

// 현재 사용하는 곳 없음. 주석처리.
// DECL_PACKET( DBE_INSERT_GIANT_DELETE_UNIT_REQ )
// {
// 	KGiantDeletedUnitInfo				m_DeletedUnitInfo;
// };

// 현재 사용하는 곳 없음. 주석처리.
// DECL_PACKET( DBE_UPDATE_GIANT_DELETE_UNIT_NOT )
// {
// 	std::wstring						m_wstrNickName;
// 	unsigned int						m_uiGiantUID;
// };

DECL_PACKET( EGIANT_ROLEREG_CREATE_GUILD_REQ )
{
	UidType								m_iActorUID;
	unsigned int						m_uiGiantUID;
	KEGS_CREATE_GUILD_REQ				m_CreateGuildReqPacket;
};

DECL_PACKET( EGIANT_ROLEREG_CREATE_GUILD_ACK )
{
	int									m_iOK;
	KEGS_CREATE_GUILD_REQ				m_CreateGuildReqPacket;
};

DECL_PACKET( EGIANT_ROLEREG_DELETE_GUILD_REQ )
{
	std::wstring						m_wstrGuildName;
	unsigned int						m_uiGiantUID;
};

DECL_PACKET( EGIANT_ROLEREG_DELETE_GUILD_ACK )
{
	int									m_iOK;
};

// DECL_PACKET( DBE_GET_GIANT_DELETED_UNIT_LIST_ACK )
// {
// 	std::vector<KGiantDeletedUnitInfo>	m_vecDeletedUnitInfo;
// };

//////////////////////////////////////////////////////////////////////////
// 2010.08.09 박교현 / 자이언트 InfoServer 인증
DECL_PACKET( EGIANT_INFO_GET_CCU_REQ )
{
	__int64								m_TimeStamp;
};

DECL_PACKET( EGIANT_INFO_GET_TOTAL_CCU_ACK )
{
	__int64								m_TimeStamp;
	unsigned short						m_usServerID;
	unsigned short						m_usServerType;
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	std::wstring						m_strZoneName;				// [32]
	unsigned int						m_uiCCU;
};

DECL_PACKET( EGIANT_INFO_GET_CHANNEL_CCU_ACK )
{
	__int64								m_TimeStamp;
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	//unsigned int						m_uiCountryNum;
	//unsigned int						m_uiCountryID;
	//unsigned int						m_uiCountryCCU;
	std::map<UidType, UINT>				m_mapChannelCCU;
};

DECL_PACKET( EGIANT_INFO_CHAT_NOT )
{
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	std::wstring						m_wstrUserName;				// [32]
	std::wstring						m_wstrGMName;				// [32]
	unsigned int						m_uiCountryID;
	unsigned int						m_uiSceneID;
	unsigned int						m_uiType;
	std::wstring						m_wstrContent;				// [256]
	std::wstring						m_wstrServer;				// [32]
};

DECL_PACKET( EGIANT_INFO_PUNISH_REQ )
{
	unsigned short						m_usZone;
	unsigned short						m_usGame;
	std::wstring						m_wstrUserName;				// [32]
	std::wstring						m_wstrCountryName;			// [32]
	std::wstring						m_wstrServerName;			// [32]
	std::wstring						m_wstrGMName;				// [32]
	std::wstring						m_wstrReason;				// [128]
	unsigned short						m_usOperation;
	int									m_iDelay;
	short								m_sLevel;
	unsigned int						m_uiGiantUID;
	unsigned int						m_uiStartTime;
};




DECL_PACKET( ELG_CHAT_OBSERVATION_NOT )
{
	UidType								m_iServerUID;
	char								m_cChatPacketType;
	std::wstring						m_wstrSenderUnitNickName;
	std::wstring						m_wstrMsg;
};


//#endif // SERV_COUNTRY_CN
//////////////////////////////////////////////////////////////////////////

DECL_PACKET( EPUBLISHER_SECURITY_AUTH_REQ )
{
	// 추가 보안 인증 관련은 다 해당 패킷을 이용한다.
	enum SECURITY_AUTH_TYPE
	{
		SAT_NONE,						// 없음(초기값)
		SAT_CHK_SECOND_SECURITY,		// K3R 2차 보안패드 관련 체크
		SAT_MATRIX_CARD,				// Giant MatrixKey 시스템 사용
	};

	int						m_iSecurityAuthType;
	UidType					m_iUserUID;
	std::wstring			m_wstrUserID;
	unsigned int			m_uiPublisherUID;
	std::wstring			m_wstrIP;



	KEPUBLISHER_SECURITY_AUTH_REQ()
	{
		m_iSecurityAuthType = SAT_NONE;
		m_iUserUID			= -1;
		m_wstrUserID		= L"";
		m_uiPublisherUID	= 0;
	}

};

DECL_PACKET( EPUBLISHER_SECURITY_AUTH_ACK )
{
	int						m_iOK;
	std::string				m_strSecondSecurityURL;	// K3R 2차 관련 URL

	KEPUBLISHER_SECURITY_AUTH_ACK()
	{
		m_iOK					= -1;
		m_strSecondSecurityURL	= "";
	}
};


DECL_PACKET( EPUBLISHER_SECURITY_INFO_NOT )
{
	enum SECURITY_INFO_TYPE
	{
		SIT_NONE,						// 없음(초기값)
		SIT_MATRIX_CARD,				// Giant MatrixKey 시스템 사용
	};

	int									m_iSecurityInfoType;
	unsigned int						m_uiMatrixKey;
	std::wstring						m_wstrCoordinate;


	KEPUBLISHER_SECURITY_INFO_NOT()
	{
		m_iSecurityInfoType		= SIT_NONE;
		m_uiMatrixKey			= 0;
		m_wstrCoordinate		= L"";
	}
};


//EPUBLISHER_PCBANG_USER_LOGIN_REQ 같은 역할임
DECL_PACKET( EPUBLISHER_REG_USER_REQ )
{
	UidType							m_iActorUID;
	std::wstring					m_wstrServiceAccountID;
	std::wstring					m_wstrServicePassword;
	std::string						m_strIP;


	KEPUBLISHER_REG_USER_REQ()
	{
		m_iActorUID				= 0;
		m_wstrServiceAccountID	= L"";
		m_wstrServicePassword	= L"";
		m_strIP					= "";

	}
};

DECL_PACKET( EPUBLISHER_REG_USER_ACK )
{
	int						m_iOK;	// 접속 끊을지 여부랑 관련이 있음. NET_OK 아니면 접속 끊김
	std::wstring			m_wstrServiceAccountID;



	KEPUBLISHER_REG_USER_ACK()
	{
		m_iOK = -1;

		m_wstrServiceAccountID	= L"";


	}
};

//EPUBLISHER_PCBANG_USER_LOGOUT_REQ
DECL_PACKET( EPUBLISHER_UNREG_USER_REQ )
{
	UidType							m_iActorUID;
	std::wstring					m_wstrServiceAccountID;
	std::wstring					m_wstrServicePassword;
	std::string						m_strIP;


	KEPUBLISHER_UNREG_USER_REQ()
	{
		m_iActorUID = 0;
		m_wstrServiceAccountID = L"";
		m_wstrServicePassword = L"";
		m_strIP = "";

	}
};

DECL_PACKET( EPUBLISHER_UNREG_USER_ACK )
{
	int						m_iOK;
	std::wstring			m_wstrServiceAccountID;

	KEPUBLISHER_UNREG_USER_ACK()
	{
		m_iOK = -1;
		m_wstrServiceAccountID = L"";
	}
};

DECL_PACKET( ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK )
{
	int						m_iOK;
	std::wstring			m_wstrServiceAccountID;



	KELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK()
	{
		m_iOK = -1;
		m_wstrServiceAccountID = L"";


	}
};

//{{ 2011. 03. 03	최육사	중국 서버 빌링
DECL_PACKET( EBILL_CHECK_PRODUCT_INFO_REQ )
{
	std::map<int, int>					m_mapReleaseTick;

};

DECL_PACKET( EBILL_CHECK_PRODUCT_INFO_ACK )
{
	std::map<int, int>					m_mapReleaseTick;

	std::map< int, KBillProductInfo >       m_mapBillProductInfo;
	std::map< int, KBillProductAttribute >  m_mapBillProductAttribute;
	std::map< int, KBillPackageInfo >       m_mapBillPackageInfo;
	std::vector<std::wstring>				m_vecStartEvent;
	std::vector<std::wstring>				m_vecEndEvent;

};


//{{ 허상형 : [2010/7/28/] //	캐시 로그(쓸수도 있기때문에 define 안함)
DECL_DATA( KBuyCharInfo )
{
	char                        m_cLevel;
	int                         m_iUnitClass;
};
//}} 허상형 : [2010/7/28/] //	캐시 로그(쓸수도 있기때문에 define 안함)

// 구매 패킷은 실제 캐시샵에서 구매와 던전에서 부활 코인 구매 시 사용됨
DECL_PACKET( EBILL_BUY_PRODUCT_REQ )
{
	int                                 m_iServerGroupID;
	int									m_iChannelingCode;

	std::wstring                        m_wstrUserID;
	unsigned int						m_uiPublisherUID;
	UidType								m_iUserUID;
	UidType								m_iUnitUID;
	std::wstring                        m_wstrNickName;
	//{{100310 김정협 아이피 추가
	std::string							m_strUserIP;
	//}}100310 김정협 아이피 추가
	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;

	//{{ 허상형 : [2010/7/28/] //	캐시 로그
	KBuyCharInfo						m_BuyCharInfo;
	//}} 허상형 : [2010/7/28/] //	캐시 로그

	int									m_iUseCashType;
	int									m_iUserIP;

	// 통합 차감을 할 때 퍼블리셔에는 m_wstrTotalOrderID를 넘겨줌.
	// 통합 차감 하는 나라는 정보확인 시 m_wstrTotalOrderID 기준으로 확인 해야함
	std::wstring                        m_wstrTotalOrderID;


	KEBILL_BUY_PRODUCT_REQ()
	{
		m_iServerGroupID	= 0;
		m_iChannelingCode	= 0;
		m_wstrUserID		= L"";
		m_uiPublisherUID	= 0;
        m_iUserUID			= 0;
		m_iUnitUID			= 0;
		m_wstrNickName		= L"";
		m_strUserIP			= "";
		m_vecBillBuyInfo.clear();
		m_iUseCashType		= 0;
		m_iUserIP			= 0;
		m_wstrTotalOrderID	= L"";
	}
};

DECL_PACKET( EBILL_BUY_PRODUCT_ACK )
{
	int                                 m_iOK;
	std::vector< KBillOrderInfo >       m_vecBillOrderInfo;
	//{{ 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
	std::vector< int >				    m_vecPackageID;
	//}} 100713 김정협 패키지아이템일 경우 찜하기 목록에서 빼주기 위한 목록
	int									m_iTotalCash;
	int									m_iCashType;
	KGlobalCashInfo						m_RemainCashInfo;
};

DECL_PACKET( EBILL_PREPARE_BUY_PRODUCT_ACK )
{
	int									m_iOK;
	KEBILL_BUY_PRODUCT_REQ				m_kEBILL_BUY_PRODUCT_REQ;
};


DECL_PACKET( EBILL_GIFT_ITEM_REQ )
{
	int                                 m_iServerGroupID;
	int									m_iChannelingCode;

	std::wstring                        m_wstrSenderUserID;
	unsigned int						m_uiSenderPublisherUID;

	std::wstring                        m_wstrSenderNickName;
	std::wstring                        m_wstrReceiverUserID;
	std::wstring                        m_wstrReceiverNickName;

	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;

	UidType								m_iSenderUserUID;
	UidType								m_iSenderUnitUID;

	UidType								m_iReceiverUserUID;
	UidType								m_iReceiverUnitUID;

	std::wstring						m_wstrPresentMessage;

	//{{ 허상형 : [2010/7/28/] //	캐시 로그
	KBuyCharInfo						m_BuyCharInfo;
	KBuyCharInfo						m_RecvCharInfo;
	//}} 허상형 : [2010/7/28/] //	캐시 로그
	int									m_iUseCashType;
	std::wstring						m_wstrUserIP;

	// 통합 차감을 할 때 퍼블리셔에는 m_wstrTotalOrderID를 넘겨줌.
	// 통합 차감 하는 나라는 정보확인 시 m_wstrTotalOrderID 기준으로 확인 해야함
	std::wstring                        m_wstrTotalOrderID;

	KEBILL_GIFT_ITEM_REQ()
	{
		m_iServerGroupID		=	0;
		m_iChannelingCode		=	0;

		m_wstrSenderUserID		=	L"";
		m_uiSenderPublisherUID	=	0;

		m_wstrSenderNickName	=	L"";
		m_wstrReceiverUserID	=	L"";
		m_wstrReceiverNickName	=	L"";

		m_iSenderUserUID		=	0;
		m_iSenderUnitUID		=	0;

		m_iReceiverUserUID		=	0;
		m_iReceiverUnitUID		=	0;

		m_wstrPresentMessage	=	L"";

		m_wstrTotalOrderID		=	L"";
	}
};

DECL_PACKET( EBILL_GIFT_ITEM_ACK )
{
	int                                 m_iOK;
	std::vector< KBillOrderInfo >       m_vecBillOrderInfo;
	int									m_iTotalCash;
	int									m_iCashType;
	KGlobalCashInfo						m_RemainCashInfo;
};

DECL_PACKET( EBILL_PREPARE_GIFT_ITEM_ACK )
{
	int									m_iOK;
	KEBILL_GIFT_ITEM_REQ				m_kEBILL_GIFT_ITEM_REQ;
};

DECL_PACKET( DBE_INSERT_CASH_DECREASE_ERROR_LOG_NOT )
{
	UidType							m_iUserUID;
	int								m_iResultCode;
	std::string						m_strOutString;
};

//
DECL_DATA( KPurchaserInfo )
{
	int									m_iServerGroupID;
	UidType								m_iUserUID;
	std::wstring						m_wstrUserName;
	UidType								m_iUnitUID;
	std::wstring						m_wstrUnitNickName;
	UCHAR								m_ucLevel;
	std::wstring						m_wstrIP;
	unsigned int						m_uiPublisherUID;
	int									m_iChannelingCode;

	CON_COPYCON_ASSIGNOP( KPurchaserInfo, right )
	{
		m_iServerGroupID		= right.m_iServerGroupID;
		m_iUserUID				= right.m_iUserUID;
		m_wstrUserName			= right.m_wstrUserName;
		m_iUnitUID				= right.m_iUnitUID;
		m_wstrUnitNickName		= right.m_wstrUnitNickName;
		m_ucLevel				= right.m_ucLevel;
		m_wstrIP				= right.m_wstrIP;
		m_uiPublisherUID		= right.m_uiPublisherUID;
		m_iChannelingCode		= right.m_iChannelingCode;

		return *this;
	}
};

DECL_PACKET( EBILL_USE_COUPON_REQ )
{
	// 쿠폰 사용 에러 Return Code
	enum ENUM_RETURN_CODE_TYPE
	{
		GB_RCT_SUCCESS						= 0,	// 성공
		GB_RCT_FAIL							= -1,	// 실패
		GB_RCT_ACCOUNT_NOT_EXIST			= -2,	// 유저 부재
		GB_RCT_CASH_NOT_EXIST				= -3,	// 잔액 부족
		GB_RCT_ITEM_CARD_NOT_EXIST			= -4,	// 아이템 카드 부재
		GB_RCT_ITEM_CARD_NOT_AREA			= -5,	// 해당 아이템 카드로 해당 구역에서 사용 불가
		GB_RCT_ITEM_CARD_DUPLICATE			= -6,	// 매트릭스 카드 바인딩
		GB_RCT_ZONE_CARD_ID_ERROR			= -7,	// 전용 카드 ID에러
		GB_RCT_ZONE_CARD_PASSWORD_ERROR		= -8,	// 전용 카드 패스워드 에러
		GB_RCT_DB_ERROR						= -9,	// DB에러
	};



	KPurchaserInfo						m_PurchaserInfo;
	int									m_iCouponType;
	std::wstring						m_wstrSerialCode;
	std::wstring						m_wstrTransaction;	// 쿠폰 예약- 퍼블 인증 - 아이템 넣기 구조에서 사용

	KEBILL_USE_COUPON_REQ()
	{
		m_iCouponType		=	0;		// 현재 중국 제외하고는 의미 없는 값임
		m_wstrSerialCode	=	L"";
		m_wstrTransaction	=	L"";
	}
};

DECL_DATA( KBillCouponItemInfo )
{
	int										m_iProductNO;
	int										m_iCount;
	UidType									m_iTranNo;
};

DECL_PACKET( EBILL_USE_COUPON_ACK )
{
	int										m_iOK;
	std::vector< KBillCouponItemInfo >		m_vecBillCouponItemInfo;
};


DECL_PACKET( EBILL_USE_COUPON_RESULT_REQ )
{
	UidType								m_iGSUID;
	UidType								m_iUserUID;
	std::wstring						m_wstrTransaction;
	int									m_iRet;
	int									m_iChargeItem;
	int									m_iChargeItemCnt;
	int									m_iChargeCash;
	int									m_iChargeBonus;

	KEBILL_USE_COUPON_RESULT_REQ()
	{
		m_iGSUID				=	0;
		m_iUserUID				=	0;
		m_wstrTransaction		=	L"";
		m_iRet					=	0;
		m_iChargeItem			=	0;
		m_iChargeItemCnt		=	1;	// 수량은 Default 1
		m_iChargeCash			=	0;
		m_iChargeBonus			=	0;
	}
};

DECL_PACKET( EBILL_USE_COUPON_RESULT_ACK )
{
	int									m_iOK;
	int									m_iTotalCash;
	std::vector< KBillCouponItemInfo >		m_vecBillCouponItemInfo;
};

DECL_PACKET( EBILL_USE_COUPON_RESERVE_ACK )
{
	int									m_iOK;
	KEBILL_USE_COUPON_REQ				m_kPacketReq;
};

// 캐쉬 인벤토리에 있는 아이템을 내 캐릭터 인벤토리로 가져오는 패킷
DECL_PACKET( EBILL_PICK_UP_REQ )
{
	UidType                             m_iTransNo;
	int                                 m_iProductNo;
	UidType                             m_iUnitUID;
	std::map< int, int >                m_mapSocketForCash;
	BYTE								m_byteProductKind;
	std::vector< KNXBTPickUpPackageInfo > m_vecPickUpPackageInfo;
	int                                 m_iSubProductNo;
	int                                 m_iSubProductID;
};

DECL_PACKET( EBILL_PICK_UP_ACK )
{
	// 국내 ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK 이거와 맵핑됨

	int                                 m_iOK;
	int                                 m_iProductNo;
	std::map< int, int >                m_mapSocketForCash;
};

DECL_PACKET( ELG_EXCHANGE_CASH_REQ )
{
	KPurchaserInfo						m_PurchaserInfo;
	int									m_iPoint;
};

DECL_PACKET( ELG_EXCHANGE_CASH_ACK )
{
	int									m_iOK;
	int									m_iPoint;
};

DECL_PACKET( EBILL_CHECK_BALANCE_REQ )
{
	int									m_iPublisherCashBalance;

	KEBILL_CHECK_BALANCE_REQ()
	{
		m_iPublisherCashBalance = 0;
	}
};


DECL_PACKET( EBILL_EXCHANGE_CASH_RESERVE_REQ )
{
	KPurchaserInfo						m_PurchaserInfo;
	int									m_iPoint;
};

DECL_PACKET( EBILL_EXCHANGE_CASH_RESERVE_ACK )
{
	int									m_iOK;
	std::wstring						m_wstrTransaction;
	KEBILL_EXCHANGE_CASH_RESERVE_REQ	m_kPacketReq;
};

DECL_PACKET( EBILL_EXCHANGE_CASH_REQ )
{
	UidType								m_iGSUID;
	UidType								m_iUserUID;
	std::wstring						m_wstrTransaction;
	int									m_iRet;
	int									m_iPoint;
};

DECL_PACKET( EBILL_EXCHANGE_CASH_ACK )
{
	int									m_iOK;
	UidType								m_iUnitUID;
	int									m_iTotalCash;
	int									m_iChargeCash;
	int									m_iChargeBonus;
	int									m_iBonusItem;

	KGlobalCashInfo						m_RemainCashInfo;

	KEBILL_EXCHANGE_CASH_ACK()
	{
		m_iUnitUID = 0;
		m_iTotalCash = 0;
		m_iChargeCash = 0;
		m_iChargeBonus = 0;
		m_iBonusItem = 0;
	}
};

DECL_PACKET( EBILL_GET_TRANS_FOR_CHECK_BALANCE_REQ )
{
	KEPUBLISHER_BILLING_BALANCE_REQ			m_kCheckGiantBalanceReq;
};

DECL_PACKET( EBILL_GET_TRANS_FOR_CHECK_BALANCE_ACK )
{
	int									m_iOK;
	std::wstring						m_wstrTransaction;
	KEPUBLISHER_BILLING_BALANCE_REQ			m_kCheckGiantBalanceReq;
};

//{{ 허상형 : [2010/8/19/] //	선물 기능 개편 . SERV_GLOBAL_BILLING. 유럽에 추가 조효진
DECL_PACKET( DBE_GET_NICKNAME_BY_UNITUID_REQ )
{
	std::vector< UidType >	m_vecUID;
	std::vector< KBillOrderInfo >       m_vecBillOrderInfo;
	std::vector< KBillPackageInfo >     m_vecPackageInfo;
	std::vector< int >					m_vecSubPackageTrans;
	std::vector< int >					m_vecSubPackageInfo;
};

DECL_PACKET( DBE_GET_NICKNAME_BY_UNITUID_ACK )
{
	std::map< UidType, std::wstring >	m_mapNickName;
	std::vector< KBillOrderInfo >       m_vecBillOrderInfo;
	std::vector< KBillPackageInfo >     m_vecPackageInfo;
	std::vector< int >					m_vecSubPackageTrans;
	std::vector< int >					m_vecSubPackageInfo;
};
//}} 허상형 : [2010/8/19/] //	선물 기능 개편

//}}


//////////////////////////////////////////////////////////////////////////


DECL_PACKET( EPL_BT_HEALTH_CHECK_REQ )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4
};

DECL_PACKET( EPL_BT_HEALTH_CHECK_ACK )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4
};

DECL_PACKET( EPL_BT_BALANCE_CHECK_REQ )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4

	unsigned long	m_ulUserNo;
	std::wstring	m_wstrUserID;

	unsigned long	m_ulRealCash;
	unsigned long	m_ulBonusCash;
};

DECL_PACKET( EPL_BT_BALANCE_CHECK_ACK )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4

	unsigned long	m_ulUserNo;
	std::wstring	m_wstrUserID;

	unsigned long	m_ulRealCash;
	unsigned long	m_ulBonusCash;

	//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
	std::wstring						m_wstrRetMsg;
	//}}
};


DECL_PACKET( EPL_BT_PURCHASEITEM_ACK )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4

	std::wstring                        m_wstrOrderID;
	int                                 m_iServerGroupID;
	std::wstring                        m_wstrUserID;
	UidType								m_iUserUID;
	std::wstring                        m_wstrNickName;
	//{{100310 김정협 아이피 추가
	std::string							m_strUserIP;
	//}}100310 김정협 아이피 추가
	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;

	bool                                 m_bResurrectionStone;

	//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
	std::wstring						m_wstrRetMsg;
	//}}
};

DECL_PACKET( EPL_BT_GIFTITEM_REQ )
{
	unsigned long	m_ulReqKey;		// 패킷 통신을 위한 패킷 번호

	std::wstring                        m_wstrOrderID;
	int                                 m_iServerGroupID;
	std::wstring                        m_wstrSenderUserID;
	std::wstring                        m_wstrSenderNickName;
	std::wstring                        m_wstrReceiverUserID;
	//{{ 허상형 : [2010/2/9/] //	UID 추가
	UidType								m_iSenderUnitUID;
	UidType								m_iReceiverUID;
	std::wstring						m_wstrPresentMessage;
	//}} 허상형 : [2010/2/9/] //	UID 추가
	//{{100310 김정협 아이피 추가
	std::string							m_strUserIP;
	//}}100310 김정협 아이피 추가
	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;
};

DECL_PACKET( EPL_BT_GIFTITEM_ACK )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4

	std::wstring                        m_wstrOrderID;
	int                                 m_iServerGroupID;
	std::wstring                        m_wstrUserID;
	UidType								m_iUserUID;
	std::wstring                        m_wstrNickName;
	//{{100310 김정협 아이피 추가
	std::string							m_strUserIP;
	//}}100310 김정협 아이피 추가
	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;

	//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
	std::wstring						m_wstrRetMsg;
	//}}
};

DECL_PACKET( EPL_BT_REGCOUPON_ACK )
{
	unsigned short	m_usReqLen;		// IN	1
	unsigned short	m_usReqType;	// IN	2
	unsigned long	m_ulReqKey;		// IN	3
	unsigned short	m_usRetCode;	// OUT	4

	UidType								m_iUserUID;
	//int									m_iItemID;
	//int									m_iItemCount;
	std::vector< KBillBuyInfo >         m_vecBillBuyInfo;

	//{{ 리턴메세지를 확인하기 위한 부분 추가합니다. 2012.03.07 박진웅
	std::wstring						m_wstrRetMsg;
	//}}
};


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
DECL_PACKET( DBE_SERVERGROUP_LIST_ACK )
{
	std::map< int, KServerGroupInfo >		m_mapServerGroupList;
};
//}}

DECL_PACKET( ERM_TOGGLE_INVISIBLE_NOT )
{
	UidType								m_iUnitUID;
	KFieldUserInfo						m_kFieldUserInfo;
};

DECL_PACKET( ETR_TOGGLE_INVISIBLE_NOT )
{
	std::vector< UidType >				m_vecUID;
	UidType								m_iUnitUID;
	KFieldUserInfo						m_kFieldUserInfo;
};

DECL_DATA( KTradeShareItemInfo )
{
	UidType								m_iItemUID;

	UidType								m_iFromUnitUID;
	UidType								m_iToUnitUID;

	char								m_cInventoryCategory;
	short								m_sSlotID;

	u_char								m_ucSealCnt;
	u_char								m_ucDeleteReason;

	int									m_iItemID;			//	Log
	UidType								m_iNewItemUID;		//	Log

	std::wstring						m_wstrRegDate;		//	Log

	KTradeShareItemInfo()
	{
		m_iItemUID				= 0;

		m_iFromUnitUID			= 0;
		m_iToUnitUID			= 0;

		m_cInventoryCategory	= 0;
		m_sSlotID				= 0;

		m_ucSealCnt				= 0;
		m_ucDeleteReason		= 0;

		m_iItemID				= 0;
		m_iNewItemUID			= 0;

		m_wstrRegDate			= L"";
	}
};

DECL_DATA( KUpdateShareItemInfo )
{
	UidType								m_iItemUID;
	UidType								m_iUnitUID;

	int									m_iUsageType;
	int									m_iValue;

	KUpdateShareItemInfo()
	{
		m_iItemUID				= 0;
		m_iUnitUID				= 0;

		m_iUsageType			= 0;
		m_iValue				= 0;
	}
};

DECL_PACKET( DBE_UPDATE_SHARE_ITEM_REQ )
{
	UidType										m_iUnitUID;

	KItemPositionUpdate							m_mapUpdatePosition;
	std::map< UidType, KTradeShareItemInfo >	m_mapInItem;
	std::map< UidType, KTradeShareItemInfo >	m_mapOutItem;
	std::map< UidType, KUpdateShareItemInfo >	m_mapUpdateEndurance;
	std::map< UidType, KUpdateShareItemInfo >	m_mapUpdateQuantity;

	std::vector< UidType >						m_vecDeletedItem;

	bool										m_bReload;
	std::wstring								m_wstrReloadNickname;

	std::vector< KInventoryItemInfo >			m_vecInventorySlotItem;

	KDBE_UPDATE_SHARE_ITEM_REQ()
	{
		m_iUnitUID = 0;
		m_bReload = false;
	}
};

DECL_PACKET( DBE_UPDATE_SHARE_ITEM_ACK )
{
	int											m_iOK;

	bool										m_bReload;
	std::wstring								m_wstrReloadNickname;

	std::vector< KTradeShareItemInfo >			m_vecTradeShareItemResult;	//	Log

	std::vector< KInventoryItemInfo >			m_vecInventorySlotItem;

	KDBE_UPDATE_SHARE_ITEM_ACK()
	{
		m_iOK = 0;
		m_bReload = false;
	}
};

DECL_PACKET( ELOG_ITEM_TRADE_ERROR_NOT )
{
	UidType									m_iUnitUID;
	int										m_iType;

	std::wstring							m_wsrtRegDate;
};

DECL_PACKET( DBE_CHANGE_INVENTORY_SLOT_ITEM_REQ )
{
	char								m_cFromSlotType;
	int									m_iFromSlotID;
	char								m_cToSlotType;
	int									m_iToSlotID;

	UidType                             m_iUnitUID;
	KItemQuantityUpdate                 m_kItemQuantityUpdate;
	KItemEnduranceUpdate				m_kItemEnduranceUpdate;
	KItemPositionUpdate					m_kItemPositionUpdate;

	KDBE_CHANGE_INVENTORY_SLOT_ITEM_REQ()
	{
		m_cFromSlotType = 0;
		m_iFromSlotID = 0;
		m_cToSlotType = 0;
		m_iToSlotID = 0;
		m_iUnitUID = 0;
	}
};

DECL_PACKET( DBE_CHANGE_INVENTORY_SLOT_ITEM_ACK )
{
	char								m_cFromSlotType;
	int									m_iFromSlotID;
	char								m_cToSlotType;
	int									m_iToSlotID;

	KItemQuantityUpdate                 m_kItemQuantityUpdate;
	KItemEnduranceUpdate				m_kItemEnduranceUpdate;
	KItemPositionUpdate					m_kItemPositionUpdate;

	KDBE_CHANGE_INVENTORY_SLOT_ITEM_ACK()
	{
		m_cFromSlotType = 0;
		m_iFromSlotID = 0;
		m_cToSlotType = 0;
		m_iToSlotID = 0;
	}
};

//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
DECL_PACKET( DBE_SHARING_BACK_OPEN_REQ )
{
	enum SHARING_BANK_OPEN_TYPE{
		SBOT_QUEST = 1,
		SBOT_CASH  = 2,
	};

	UidType							m_iUserUID;
	UidType							m_iUnitUID;
	int								m_iOpenType;
};

DECL_PACKET( DBE_SHARING_BACK_OPEN_ACK )
{
	int								m_iOK;
	int								m_iOpenType;
};
//}}

DECL_PACKET( DBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT )
{
	UidType								m_iUserUID;
	std::vector< int >					m_vecCompletedEvent;
};

DECL_PACKET( DBE_CHECK_EVENT_UPDATE_ACK )
{
	std::map< int, int >	m_mapReleaseTick;
};

//{{ 2013. 05. 14	최육사	아이템 개편
DECL_PACKET( DBE_CHECK_BLOCK_LIST_REQ )
{
	int								m_iReleaseTick;

	KDBE_CHECK_BLOCK_LIST_REQ()
	{
		m_iReleaseTick = 0;
	}
};

DECL_PACKET( DBE_CHECK_BLOCK_LIST_ACK )
{
	int											m_iReleaseTick;
	std::map< int, std::vector< KBlockInfo > >	m_mapBlockInfo;

	KDBE_CHECK_BLOCK_LIST_ACK()
	{
		m_iReleaseTick = 0;
	}
};

DECL_PACKET( EGB_CHECK_BLOCK_USER_NOT )
{
	std::vector< int >				m_vecBlockType;
	UidType							m_iUserUID;
	UidType							m_iUnitUID;
	std::wstring					m_wstrIP;

	KEGB_CHECK_BLOCK_USER_NOT()
	{
		m_iUserUID = 0;
		m_iUnitUID = 0;
	}
};

DECL_PACKET( EGB_UPDATE_BLOCK_RESULT_NOT )
{
	std::map< int, bool >			m_mapBlockResult;
};
//}}

DECL_PACKET( DBE_INSERT_WARP_VIP_REQ )
{
	UidType			m_iUnitUID;
	int				m_iPeriod;
};


DECL_PACKET( DBE_INSERT_WARP_VIP_ACK )
{
	int				m_iOK;
	std::wstring	m_wstrEndDate;
};



DECL_PACKET( DBE_READY_TO_SOSUN_EVENT_REQ )
{
	UidType								m_iUnitUID;
	std::map< int, int >				m_mapInsertedItem;
	KItemQuantityUpdate					m_kItemQuantityUpdate;
	std::vector< KInventoryItemInfo >	m_vecUpdatedInventorySlot;
	std::vector< KItemInfo >			m_vecItemInfo;
	int									m_iFirstUnitClass;
};

DECL_PACKET( DBE_READY_TO_SOSUN_EVENT_ACK )
{
	int									m_iOK;
	std::map< int, int >				m_mapInsertedItem;
	KItemQuantityUpdate					m_kItemQuantityUpdate;
	std::vector< KInventoryItemInfo >	m_vecUpdatedInventorySlot;
	std::map< UidType, KItemInfo >		m_mapItemInfo;
	int									m_iFirstUnitClass;
};






DECL_PACKET( DBE_EVENT_PROPOSE_USER_FIND_ACK )
{
	int						m_iOK;
	std::wstring			m_wstrOtherNickName;
	UidType					m_iOtherUserUID;
	UidType					m_iOtherUnitUID;
	char					m_cUnitClass;
	bool					m_bCouple;
};

typedef KELG_COUPLE_PROPOSE_CHECK_CONNECT_USER_REQ KELG_EVENT_PROPOSE_CHECK_CONNECT_USER_REQ;
typedef KELG_COUPLE_PROPOSE_CHECK_CONNECT_USER_ACK KELG_EVENT_PROPOSE_CHECK_CONNECT_USER_ACK;
typedef KELG_COUPLE_PROPOSE_NOT	KELG_EVENT_PROPOSE_NOT;
typedef KERM_COUPLE_PROPOSE_RESULT_FAIL_NOT KERM_EVENT_PROPOSE_RESULT_FAIL_NOT;
typedef KDBE_COUPLE_MAKING_SUCCESS_REQ KDBE_EVENT_MAKING_SUCCESS_REQ;
typedef KDBE_COUPLE_MAKING_SUCCESS_ACK KDBE_EVENT_MAKING_SUCCESS_ACK;
typedef KERM_COUPLE_PROPOSE_RESULT_SUCCESS_NOT KERM_EVENT_PROPOSE_RESULT_SUCCESS_NOT;

DECL_PACKET( DBE_EVENT_MAKING_SUCCESS_ACCEPTOR_REQ )
{
	std::vector< KInventoryItemInfo >   m_vecUpdatedInventorySlot;
	std::vector< KDeletedItemInfo >		m_vecDeleted;
};

DECL_PACKET( DBE_EVENT_MAKING_SUCCESS_ACCEPTOR_ACK )
{
	int									m_iOK;
	KItemQuantityUpdate                 m_kItemQuantityUpdate;
	std::vector< KInventoryItemInfo >   m_vecUpdatedInventorySlot;

	KDBE_EVENT_MAKING_SUCCESS_ACCEPTOR_ACK()
	{
		m_iOK = 0;
	}
};

DECL_PACKET( DBE_EVENT_DIVORCE_REQ )
{
	UidType								m_iUnitUID;
	UidType								m_iRelationTargetUserUid;
	std::vector< KInventoryItemInfo >   m_vecUpdatedInventorySlot;
	std::vector< KDeletedItemInfo >		m_vecDeleted;
};

DECL_PACKET( DBE_EVENT_DIVORCE_ACK )
{
	int									m_iOK;
	UidType								m_iRelationTargetUserUid;
	KItemQuantityUpdate                 m_kItemQuantityUpdate;
	std::vector< KInventoryItemInfo >	m_vecUpdatedInventorySlot;

	KDBE_EVENT_DIVORCE_ACK()
	{
		m_iOK = 0;
	}
};

typedef KELG_BREAK_UP_NOT KELG_DIVORCE_NOT;



//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//#ifdef SERV_RELAY_DB_CONNECTION
DECL_PACKET( DBE_UPDATE_USER_INFO_TO_RELAY_DB_JP_EVENT_NOT )
{
	enum UPDATE_TYPE
	{
		UT_NONE = 0,
		UT_LEVEL_UP,
		UT_UNITCLASS_CHANGE,
	};

	std::wstring						m_wstrUserID;
	UidType								m_iUserUID;
	UidType								m_iUnitUID;
	std::wstring						m_wstrNickName;
	char								m_cLevel;
	char								m_cUnitClass;
	bool								m_bDeletedUnit;
	std::wstring						m_wstrUserRegDate;
	std::wstring						m_wstrUnitRegDate;
	int									m_iUpdateType;

	KDBE_UPDATE_USER_INFO_TO_RELAY_DB_JP_EVENT_NOT()
	{
		m_iUserUID		= 0;
		m_iUnitUID		= 0;
		m_cLevel		= 0;
		m_cUnitClass	= 0;
		m_bDeletedUnit	= false;
		m_iUpdateType	= 0;
	}
};

DECL_PACKET( DBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_REQ )
{
	UidType								m_iUserUID;
	UidType								m_iUnitUID;

	KDBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_REQ()
	{
		m_iUserUID = 0;
		m_iUnitUID = 0;
	}
};

DECL_PACKET( DBE_CHECK_REWARD_FROM_RELAY_DB_JP_EVENT_ACK )
{
	std::vector< KJPEventRewardInfo >	m_vecRewardInfo;
};

DECL_PACKET( EBILL_REWARD_COUPON_JP_EVENT_REQ )
{
	KJPEventRewardInfo					m_kRewardInfo;
	KEBILL_USE_COUPON_REQ				m_kUseCouponPacket;
};

DECL_PACKET( EBILL_REWARD_COUPON_JP_EVENT_ACK )
{
	KJPEventRewardInfo					m_kRewardInfo;
	KEBILL_USE_COUPON_ACK				m_kUseCouponPacket;
};

DECL_PACKET( DBE_UPDATE_REWARD_COMPLETE_JP_EVENT_NOT )
{
    __int64								m_iRID;

	KDBE_UPDATE_REWARD_COMPLETE_JP_EVENT_NOT()
	{
		m_iRID = 0;
	}
};
//#endif SERV_RELAY_DB_CONNECTION
//}}

DECL_PACKET( DBE_CHANGE_PET_ID_REQ )
{
	UidType							m_iPetUID;
	int								m_iBeforePetID;
	int								m_iAfterPetID;

	KDBE_CHANGE_PET_ID_REQ()
		: m_iPetUID( 0 ), m_iBeforePetID( 0 ), m_iAfterPetID( 0 )
	{
	}
};

DECL_PACKET( DBE_CHANGE_PET_ID_ACK )
{
	int								m_iOK;
	UidType							m_iPetUID;
	int								m_iBeforePetID;
	int								m_iAfterPetID;

	KDBE_CHANGE_PET_ID_ACK()
		: m_iOK( 0 ), m_iPetUID( 0 ), m_iBeforePetID( 0 ), m_iAfterPetID( 0 )
	{
	}
};

DECL_PACKET( DBE_CHANGE_PET_ID_LOG_NOT )
{
	UidType                         m_iUnitUID;
	UidType							m_iPetUID;
	int								m_iBeforePetID;
	int								m_iAfterPetID;

	KDBE_CHANGE_PET_ID_LOG_NOT()
		: m_iUnitUID( 0 ), m_iPetUID( 0 ), m_iBeforePetID( 0 ), m_iAfterPetID( 0 )
	{
	}
};





DECL_PACKET( DBE_RELEASE_PET_REQ )
{
	UidType		m_iPetUID;
	UidType		m_iUnitUID;

	KDBE_RELEASE_PET_REQ()
		: m_iPetUID( 0 )
		, m_iUnitUID( 0 )
	{
	}
};





DECL_PACKET( DBE_2013_EVENT_MISSION_COMPLETE_REQ )
{
	UidType		m_iUnitUID;
	u_char		m_ucLevel;
};
DECL_PACKET( DBE_2013_EVENT_MISSION_COMPLETE_ACK )
{
	int			m_iOK;
	u_char		m_ucLevel;
};
DECL_PACKET( DBE_2014_EVENT_MISSION_COMPLETE_REQ )
{
	UidType		m_iUserUID;
	UidType		m_iUnitUID;
	int			m_iNewYearMissionStepID;
};

DECL_PACKET( DBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT )
{
	std::map< int, KItemName >			m_CurrentItemTempletNameMap;
};

//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
DECL_PACKET( EGB_GET_GLOBAL_MISSION_INFO_REQ )
{
	UidType										m_iServerUID;
};

DECL_PACKET( EGB_GET_GLOBAL_MISSION_INFO_ACK )
{
	enum FLAG_TYPE
	{
		FT_NONE = 0,
		FT_INIT,
		FT_UPDATE,
		FT_BEGIN,

		FT_SUCCESS,
		FT_FAIL,
	};

	int												m_iFlag;
	std::wstring									m_wstrGlobalMissionDay;
	std::map< std::wstring, KGlobalMissionInfo >	m_mapGlobalMissionInfo;
};

typedef KEGB_GET_GLOBAL_MISSION_INFO_ACK		KEGB_UPDATE_GLOBAL_MISSION_INFO_NOT;

DECL_PACKET( DBE_GET_GLOBAL_MISSION_INFO_REQ )
{
	std::map< std::wstring, KGlobalMissionInfo >	m_mapGlobalMissionInfo;
};

typedef KDBE_GET_GLOBAL_MISSION_INFO_REQ		KDBE_GET_GLOBAL_MISSION_INFO_ACK;

DECL_PACKET( DBE_UPDATE_GLOBAL_MISSION_INFO_REQ )
{
	std::wstring								m_wstrGlobalMissionDay;
	int											m_iNowCount;
};

typedef KDBE_UPDATE_GLOBAL_MISSION_INFO_REQ		KEGB_GLOBAL_MISSION_UPDATE_NOT;


DECL_PACKET( EGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT )
{
	std::map< int, std::wstring >				m_mapGlobalMissionStartTime;
};

DECL_PACKET( DBE_INSERT_CONNECT_EVENT_REQ )
{
	std::wstring								m_wstrEventName;
	int											m_iEventType;
	std::wstring								m_wstrNextDayStartTime;
	std::wstring								m_wstrNextDayEndTime;
	int											m_iScriptID;
};

DECL_PACKET( DBE_REGIST_GLOBAL_MISSION_CLEAR_NOT )
{
	std::wstring								m_wstrGlobalMissionDay;
	UidType										m_iUnitUID;
};
//}} 2012. 09. 03	임홍락	글로벌 미션 매니저

DECL_PACKET( DBE_UNLIMITED_SECOND_CHANGE_JOB_NOT )
{
	int											m_iUnitUID;
	int											m_iOldUnitClass;
	int											m_iNewUnitClass;
};












#pragma pack( pop )
