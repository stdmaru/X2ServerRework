#pragma once

#include "CommonPacket.h"

#pragma pack( push, 1 )

//////////////////////////////////////////////////////////////////////////
DECL_PACKET( ECH_SECURITY_AUTH_REQ )
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



	KECH_SECURITY_AUTH_REQ()
	{
		m_iSecurityAuthType = SAT_NONE;
		m_iUserUID			= -1;
		m_wstrUserID		= L"";
	}

};

DECL_PACKET( ECH_SECURITY_AUTH_ACK )
{
	int						m_iOK;
	std::string				m_strSecondSecurityURL;	// K3R 2차 관련 URL

	KECH_SECURITY_AUTH_ACK()
	{
		m_strSecondSecurityURL = "";
	}
};

DECL_PACKET( ECH_SECURITY_INFO_NOT )
{
	enum SECURITY_INFO_TYPE
	{
		SIT_NONE,						// 없음(초기값)
		SIT_MATRIX_CARD,				// Giant MatrixKey 시스템 사용
	};

	int									m_iSecurityInfoType;
	unsigned int						m_uiMatrixKey;
	std::wstring						m_wstrCoordinate;


	KECH_SECURITY_INFO_NOT()
	{
		m_iSecurityInfoType		= SIT_NONE;
		m_uiMatrixKey			= 0;
		m_wstrCoordinate		= L"";
	}
};


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

DECL_PACKET( EGS_EXCHANGE_CASH_REQ )
{
	int									m_uiExchangeValue;
};

DECL_PACKET( EGS_EXCHANGE_CASH_ACK )
{
	int									m_iOK;
	int									m_iPoint;
	KGlobalCashInfo						m_AddBonusCashInfo;
};

DECL_PACKET( EGS_BONUS_CASH_EVENT_NOT )
{
	bool								m_bEventStart;
};


DECL_PACKET( EGS_BILL_PRODUCT_INFO_REQ )
{
	int                                 m_iPage;
};

DECL_PACKET( EGS_BILL_PRODUCT_INFO_ACK )
{
	bool                                m_bFinal;
	std::vector< KBillProductInfo >     m_vecProductInfo;
	std::map< int,std::vector< int > >	m_mapKeepShowItem;
	std::map< int, kDisCountItemInfo >	m_DisCountInfoMap;
};

DECL_PACKET( EGS_BILL_INVENTORY_INQUIRY_REQ )
{
	int                                 m_nItemPerPage;
	int                                 m_iCurrentPage;
};

DECL_PACKET( EGS_BILL_INVENTORY_INQUIRY_ACK )
{
	int                                 m_iOK;
	std::vector< KBillOrderInfo >       m_vecOrderInfo;
	std::vector< KBillPackageInfo >     m_vecPackageInfo;
	std::vector< int >					m_vecSubPackageTrans;
	std::vector< int >					m_vecSubPackageInfo;

};

DECL_PACKET( EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ )
{
	// 국내 EGS_GET_PURCHASED_CASH_ITEM_REQ 패킷과 맵핑됨

	int                                 m_iTransNo;
	int                                 m_iProductNo;
	std::map< int, int >                m_mapSocketForCash;

	//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
	std::map< int, int >                m_mapSocketGroupIDForCash;		// <ItemID,GroupID>
	//}
	BYTE								m_byteProductKind;	// 1 : 패키지 아이템

	// 해당 패킷과 유사한 패키지 아이템 가져오는 EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ는 해외도 그대로 씀
};

typedef     KEGS_GET_PURCHASED_CASH_ITEM_ACK    KEGS_BILL_GET_PURCHASED_CASH_ITEM_ACK;



//////////////////////////////////////////////////////////////////////////

DECL_PACKET( EGS_ADMIN_DELETE_GUILD_ADD_NOT )
{
	int						m_iGuildUID;
};

//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
DECL_PACKET( EGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT )
{
	int						m_iRankingType;
	int						m_iRank;
};

DECL_PACKET( EGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT )
{
	int						m_iRankingType;
	std::wstring			m_wstrNickName;
};

DECL_PACKET( EGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT )
{
	int						m_iRankingType;
	UidType 				m_iUnitUID;
};
//}}

DECL_PACKET( EGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT )
{
	std::wstring						m_wstrNewCharTBMsg;
};


// 2011.05.05 lygan_조성욱 // 중국용 커뮤니티탭 유저 정보 보기

DECL_PACKET( EGS_USER_COMMUNITY_SURVEY_NEW_REQ  )
{
	enum COMMUNITY_TAB_TYPE
	{
		NONE = 0,
		USER_WATCH,
		INVITE_GUILD,

	};

	std::wstring						m_wUnitNickName;
	int									m_cCommunity_Tab_Type;
	int									m_iOK;

	KEGS_USER_COMMUNITY_SURVEY_NEW_REQ()
	{
		m_cCommunity_Tab_Type = NONE;
	}
};


DECL_PACKET( EGS_USER_COMMUNITY_SURVEY_NEW_ACK  )
{
	enum COMMUNITY_TAB_TYPE
	{
		NONE = 0,
		USER_WATCH,
		INVITE_GUILD,

	};

	UidType								unitUID;
	int									m_cCommunity_Tab_Type;
	int									m_iOK;

	KEGS_USER_COMMUNITY_SURVEY_NEW_ACK()
	{
		m_cCommunity_Tab_Type = NONE;
	}
};



DECL_PACKET( EGS_PVP_REMATCH_REQ )
{
	bool							m_bAcceptRematch;
};
DECL_PACKET( EGS_PVP_REMATCH_ACK )
{
	int								m_iOK;
};
DECL_PACKET( EGS_PVP_REMATCH_NOT )
{
	UidType							m_iUnitUID;
	bool							m_bAcceptRematch;
};

//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
DECL_PACKET( EGS_RESTORE_UNIT_REQ )
{
	UidType                 m_iUnitUID;
	UidType                 m_iUserUID;
};

DECL_PACKET( EGS_RESTORE_UNIT_ACK )
{
	int                     m_iOK;
	UidType                 m_iUnitUID;
	//{{ // 2012.03.06 lygan_조성욱 // 기존 문자열시간을 그대로 받는 부분을 서버에서 숫자로 변경해서 클라로 전달 구조로 변경
	__int64					m_tRestoreAbleDate; //2012.03.05 lygan_조성욱 // 캐릭터 삭제 할때 최종 삭제 가능한 날을 알기 위해
	//}}
};

DECL_PACKET( EGS_FINAL_DELETE_UNIT_REQ )
{
	UidType                 m_iUnitUID;
	UidType                 m_iUserUID;
};

DECL_PACKET( EGS_FINAL_DELETE_UNIT_ACK )
{
	int                     m_iOK;
	UidType                 m_iUnitUID;
	u_int					m_uiKNMSerialNum;
	//{{ 2009. 10. 8  최육사	길드
	int						m_iGuildUID;
	//}}
	//{{ 2011. 02. 23	최육사	캐릭터 로그
	std::wstring			m_wstrNickName;
	u_char					m_ucLevel;
	char					m_cUnitClass;
	//}}
	__int64					m_tReDelAbleDate;
};

//{{ 2011.03.16   임규수 아바타 분해 시스템

//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}

//{{ 2011. 12. 09	박교현		서버군 리스트
typedef KPacketOK		KECH_GET_SERVERGROUP_LIST_ACK;

DECL_PACKET( ECH_GET_SERVERGROUP_LIST_NOT )
{
	std::map< int, KServerGroupInfo >	m_mapServerGroupList;
};
//}}

//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}}



DECL_PACKET( EGS_XTRAP_REQ )
{
	std::vector<unsigned char> m_vecData;
};

DECL_PACKET( EGS_XTRAP_ACK )
{
	std::vector<unsigned char> m_vecData;
};

DECL_PACKET( EGS_TOGGLE_INVISIBLE_NOT )
{
	UidType								m_iUnitUID;
	KFieldUserInfo						m_kFieldUserInfo;
};


DECL_PACKET( EGS_GET_SHARE_BANK_REQ )
{
	std::wstring							m_wstrNickName;
	int										m_iNewUnitTradeBlockDay;
	int										m_iNewUnitTradeBlockUnitClass;

	KEGS_GET_SHARE_BANK_REQ()
	{
		m_wstrNickName		= L"";
		m_iNewUnitTradeBlockDay  = 0;
		m_iNewUnitTradeBlockUnitClass = 0;
	}
};

DECL_PACKET( EGS_GET_SHARE_BANK_ACK )
{
	int										m_iOK;

	UidType									m_iUnitUID;
	std::wstring							m_wstrNickName;

	int										m_iBankSize;
	std::map< UidType, KInventoryItemInfo > m_mapItem;
};

DECL_PACKET( EGS_UPDATE_ITEM_POSITION_NOT )
{
	std::vector< KTradeShareItemResult >	m_vecShareItemResult;
};

DECL_PACKET( EGS_SHARING_BACK_OPEN_NOT )
{
	int									m_iOK;
	int									m_iOpenType;
};

typedef	KPacketOK	KEGS_SHARE_BANK_UPDATE_OK_NOT;

typedef KEGS_TIME_EVENT_INFO_NOT KEGS_ADVERTISEMENT_EVENT_INFO_NOT;

DECL_PACKET( EGS_GET_ADVERTISEMENT_EVENT_ACK )
{
	std::vector< std::wstring >			m_vecAdvertisementURL;
};


DECL_PACKET( EGS_WARP_BY_BUTTON_REQ )
{
	int			m_iCurrentVillageWarpIndex;
	bool		m_bFreeWarp;
};

DECL_PACKET( EGS_WARP_BY_BUTTON_ACK )
{
	int			m_iOK;
	int			m_iED;
	int			m_iWarpPointMapID;
};


DECL_PACKET( EGS_UPDATE_MAX_LEVEL_NOT )
{
	int									m_iMaxLevel;
};

DECL_PACKET( EGS_UPDATE_DUNGEON_STATUS_NOT )
{
	std::map< int, bool >				m_mapEnableDungeon;
};


DECL_PACKET( EGS_UPDATE_CODE_EVENT_NOT )
{
	std::map< int, bool >				m_mapEnableCodeEnum;
};

DECL_PACKET( EGS_UPDATE_WARP_VIP_NOT )
{
	int				m_iOK;
	__int64			m_trWarpVipEndDate;
};



DECL_PACKET( EGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT )
{
	std::vector<int>					m_vecWarningQuestID;
};


DECL_PACKET( EGS_SKILL_USE_REQ )
{
	int m_iSkillID;
	KEGS_SKILL_USE_REQ()
	{
		m_iSkillID = -1;
	}
};

typedef KPacketOK KEGS_SHOW_DISCONNECT_REASON_NOT;

//{{김준환 서버시간 받아오기



DECL_PACKET( EGS_CHANGE_PET_ID_NOT )
{
	int									m_iOK;
	UidType								m_iPetUID;

	KEGS_CHANGE_PET_ID_NOT()
		: m_iOK( 0 ), m_iPetUID( 0 )
	{
	}
};



DECL_PACKET( EGS_READY_TO_SOSUN_EVENT_ACK )
{
	int										m_iOK;
	std::vector< KInventoryItemInfo >		m_vecKInventorySlotInfo;
	int										m_iFirstUnitClass;
};

DECL_PACKET( EGS_USE_PROPOSE_ITEM_REQ )
{
	UidType									m_iUsedItemUID;
	std::wstring							m_wstrNickName;
};

DECL_PACKET( EGS_USE_PROPOSE_ITEM_ACK )
{
	int										m_iOK;
	std::wstring							m_wstrNickName;
	std::vector< KInventoryItemInfo >		m_vecKInventorySlotInfo;
};

typedef KEGS_COUPLE_PROPOSE_NOT KEGS_EVENT_PROPOSE_NOT;
typedef KEGS_COUPLE_PROPOSE_ACK KEGS_EVENT_PROPOSE_ACK;
typedef KEGS_COUPLE_PROPOSE_AGREE_NOT KEGS_EVENT_PROPOSE_AGREE_NOT;
typedef KEGS_COUPLE_PROPOSE_RESULT_NOT KEGS_EVENT_PROPOSE_RESULT_NOT;

DECL_PACKET( EGS_EVENT_PROPOSE_RESULT_ACCEPTOR_NOT )
{
	std::vector< KInventoryItemInfo >		m_vecUpdatedInventorySlot;
};

DECL_PACKET( EGS_USE_DIVORCE_ITEM_ACK )
{
	int										m_iOK;
	std::vector< KInventoryItemInfo >		m_vecUpdatedInventorySlot;
};

typedef KEGS_BREAK_UP_NOT KEGS_DIVORCE_NOT;



DECL_PACKET( EGS_RELEASE_PET_REQ )
{
	UidType		m_iPetUID;

	KEGS_RELEASE_PET_REQ()
		: m_iPetUID( 0 )
	{
	}
};
DECL_PACKET( EGS_RELEASE_PET_ACK )
{
	UidType		m_iOK;
	UidType		m_iPetUID;

	KEGS_RELEASE_PET_ACK()
		: m_iOK( 0 )
		, m_iPetUID( 0 )
	{
	}
};
DECL_PACKET( EGS_RELEASE_PET_NOT )
{
	UidType		m_iPetUID;
	UidType		m_iUnitUID;
	int			m_iPetID;

	KEGS_RELEASE_PET_NOT()
		: m_iPetUID( 0 )
		, m_iUnitUID( 0 )
		, m_iPetID( 0 )
	{
	}
};

DECL_PACKET( EGS_CUBE_IN_ITEM_MAPPING_NOT )
{
	std::map< int, std::map<int, kRandomItemMappingToInfoClient> >	m_mapCubeInItemMapping;

	KEGS_CUBE_IN_ITEM_MAPPING_NOT()
	{
		m_mapCubeInItemMapping.clear();
	}
};




DECL_PACKET( EGS_2013_EVENT_MISSION_COMPLETE_REQ )
{
	bool								m_bLevelUpEvent;
};
DECL_PACKET( EGS_2013_EVENT_MISSION_COMPLETE_ACK )
{
	int									m_iOK;
	int									m_iRewardedLevel;
	std::vector< KInventoryItemInfo >	m_vecInventorySlotInfo;
};
typedef KPacketOK		KEGS_2014_EVENT_MISSION_COMPLETE_ACK;

DECL_PACKET( EGS_GLOBAL_MISSION_UPDATE_NOT )
{
	std::map< std::wstring, KGlobalMissionInfo > m_mapGlobalMissionInfo;
};
DECL_PACKET( EGS_UNLIMITED_SECOND_CHANGE_JOB_NOT )
{
	int									m_iUnlimitedSecondChangeJob;
};









#pragma pack( pop )
