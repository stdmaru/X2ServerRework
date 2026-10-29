#pragma once

#include "RefreshSingleton.h"
#include "CommonPacket.h"


class KGameSysVal
{
	DeclareRefreshSingleton( KGameSysVal );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	enum GAME_SYS_VALUE_TYPE
	{
		GSVT_GB_EXP = 0,
		GSVT_GB_ED,
		GSVT_GB_VP,
		GSVT_GB_SPIRIT,
		GSVT_PREMIUM_EXP,
		//{{ 2012. 07. 02	김민성       PC 방 유저 펫 추가 경험치
		GSVT_GB_PET_EXP,
		//}}
//#ifdef SERV_VIP_SYSTEM
		GSVT_VIP_EXP,
//#endif //SERV_VIP_SYSTEM
		GSVT_TOTAL_NUM,
	};


	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	enum COME_BACK_STEP_DURATION
	{
		CBSD_NONE		= 0,
		CBSD_ONE		= 15,
		CBSD_TWO		= 30,
		CBSD_THREE		= 60,
	};

	enum COME_BACK_STEP
	{
		CBS_NONE		= 0,
		CBS_ONE			= 1,
		CBS_TWO			= 2,
		CBS_THREE		= 3,
	};
	//}}

	//{{ 2012. 08. 11	박세훈	SMS 문자 발송 스크립트
	//}}


public:
	KGameSysVal();
	~KGameSysVal();

	//check func..

	float	GetGBEXPRate( int iPcBangType = -1 );
	float	GetGBEDRate( int iPcBangType = -1 );
	float	GetGBVPRate( int iPcBangType = -1 );
	bool	IsSpiritFree( int iPcBangType = -1 );
	float	GetGBPetEXPRate( int iPcBangType = -1 );
	bool	IsSecretDungeonFree( int iPcBangType = -1 );
	int		GetAdditionalHenirRewardCount( int iPcBangType = -1 );
	bool	IsUsingPcBangTitle( int iPcBangType = -1 );
	KPcBangReward GetPcBangReward( int iPcBangType = -1 );
	float	GetPremiumEXPRate()			{ return m_fValue[GSVT_PREMIUM_EXP]; }

	int		GetLimitsLevel();

	int		GetGuestUserLimitLevel()	{ return m_iGuestUserLimitLevel; }
	void	GetAdvertisementURL( std::wstring& wstrAdURL );
	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	void	GetPcBangPremiumItem( IN char cUnitType, OUT std::vector< KInventoryItemInfo >& vecPcBangPremiumItem, int iPcBangType = 0 );
	//}}


	//{{ 2013. 04. 10	최육사	어둠의 문 개편
	UidType GetTempItemUID()				{ return m_iTempItemUID++; }
	//}}

	//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
	bool	IsHackUserRegRejected() const	{ return m_bIsHackUserRegRejected; }
	bool	IsHackUserRegMonitors() const	{ return m_bIsHackUserRegMonitors; }
	bool	IsHackUserKick() const			{ return m_bIsHackUserKick; }
	//}}
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	bool	IsHackUserBlockTrade() const	{ return m_bIsHackUserBlockTrade; }
	//}}

	//lua parsing func..
	void	AddPCBang_LUA(	int iPcBangType, float fEXPRate, float fEDRate, float fVSPointRate, bool bSpiritFree, float fPetEXPRate,
							int iAdditionalHenirReward, bool bSecretDungeonFree, bool bAvatar, bool bTitle );

	void	AddLimitsMaxLevel_LUA( int iMaxLevel );
	void	AddPremiumEXP_LUA( float fPremiumEXP );
//#ifdef SERV_VIP_SYSTEM
	void	AddVIPEXP_LUA( float fVIPEXP );
//#endif //SERV_VIP_SYSTEM
	void	SetGuestUserLimitLevel_LUA( int iGuestUserLimitLevel );
	void	SetAdvertisementURL_LUA( const char* strURL );
	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	void	AddPcBangPremiumItem_LUA( char cUnitType, int iItemID );
	//}}
	//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
	void	SetIsHackUserRegRejected_LUA( bool bVal );
	void	SetIsHackUserRegMonitors_LUA( bool bVal );
	void	SetIsHackUserKick_LUA( bool bVal );
	//}}
	void	SetDisagreeHackUserFunc_LUA( bool bVal );
	bool	IsOnDisagreeHackUserFunc() const	{ return m_bDisagreeHackUserFunc; }
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	void	SetIsHackUserBlockTrade_LUA( bool bVal );
	//}}

	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	void	SetAccountStatisticsStartDate_LUA( const char* strDate );
	CTime	GetAccountStatisticsStartDate() { return m_tAccountStatisticsStartDate; }
	//}}
	//{{ 2011. 01. 18	최육사	캐릭터 카운트 정보
	void	SetCharStatisticsStartDate_LUA( const char* strDate );
	CTime	GetCharStatisticsStartDate() { return m_tCharStatisticsStartDate; }
	//}}

	//{{ 2011. 02. 06	최육사	거래 차단 기능
	void	SetBlockPersonalTrade_LUA( bool bVal );
	void	SetBlockPersonalShop_LUA( bool bVal );
	void	SetBlockSendLetter_LUA( bool bVal );

	bool	IsBlockPersonalTrade() const	{ return m_bBlockPersonalTrade; }
	bool	IsBlockPersonalShop() const		{ return m_bBlockPersonalShop; }
	bool	IsBlockSendLetter() const		{ return m_bBlockSendLetter; }
	//}}

	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	void	SetMachineIDBlock_LUA( bool bVal );
	bool	IsMachineIDBlock() const;
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	void	SetMachineIDDuplicateCheck_LUA( bool bVal );
	bool	IsMachineIDDuplicateCheck() const;
	//}}
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	void	SetHackUserPacketMornitor_LUA( bool bVal );
	bool	IsHackUserPacketMornitor() const							{ return m_bIsHackUserPacketMornitor; }
	//}}

	void	SetIsNewUnitTradeBlock_LUA( bool bVal );
	bool	IsNewUnitTradeBlock() const								{ return m_bIsNewUnitTradeBlock; }
	void	SetNewUnitTradeBlockDay_LUA( int iVal );
	int		GetNewUnitTradeBlockDay() const							{ return m_iNewUnitTradeBlockDay; }
	void	SetNewUnitTradeBlockUnitClass_LUA( int iVal );
	int		GetNewUnitTradeBlockUnitClass() const					{ return m_iNewUnitTradeBlockUnitClass; }

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	int		GetLimitsCountOfLetter()									{ return m_iLimitConutOfLetter; }
	int		GetLimitsCountOfTrade()										{ return m_iLimitConutOfTrade; }

	int		GetLetterMonitoringCheckTime()								{ return m_iLetterMonitoringCheckTime; }
	int		GetTradeMonitoringCheckTime()								{ return m_iTradeMonitoringCheckTime; }

	bool	GetLetterAndTradeMonitoringIsOn()							{ return m_bLetterAndTradeMonitoringIsOn; }
	void	SetLetterAndTradeMonitoringIsOn_Lua( bool bIsOn);

	void	SetLimitsCountOfLetter_Lua( int iCount );
	void	SetLetterMonitoringCheckTime_Lua( int iTime );

	void	SetLimitsCountOfTrade_Lua( int iCount );
	void	SetTradeMonitoringCheckTime_Lua( int iTime );

	void	SetMonitoringMinED_Lua( int iMin );
	void	SetMonitoringMaxED_Lua( int iMax );

	int		GetMonitoringMinED()										{ return m_iMonitoringMinED; }
	int		GetMonitoringMaxED()										{ return m_iMonitoringMaxED; }
	//}}

	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	void	SetDeserializeFailCheckCount_LUA( int iCount );
	int		GetDeserializeFailCheckCount() const					{ return m_iDeserializeFailCheckCount; }

	void	SetDeserializeFailCheck_LUA( bool bVal );
	bool	IsDeserializeFailCheck() const							{ return m_bDeserializeFailCheck; }
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	void	SetComeBackReward_LUA( int iStep, int iDays );
	void	GetComeBackRewardCondition( OUT std::map< int, int >& mapComeBackRewardCondition );
	//}}

	//{{ 2011. 09. 06  김민성	개인 상점 물품 등록 가격 비교
	//}}
	//{{ 2011. 06. 07    김민성    우편 및 거래 감시 실시간 계정 블록
	void SetMonitoringLetterAndTradeBlockTime_LUA( int hh, int mm, int ss, int iDuration_s );
	bool IsMonitoringLetterAndTradeBlockTime( CTimeSpan& tSpan );
	//}}
	//{{ 2011. 09. 07  김민성	서브 스테이지 DIE NPC 시간 체크 기능
	void	SetSubStageNpcDieCheck_LUA( bool bCheck )		{ m_bSubStageNpcDieCheck = bCheck; }
	bool	GetSubStageNpcDieCheck()						{ return m_bSubStageNpcDieCheck; }
	//}}


	void	SetDungeonClearPaymentItemID_LUA( int iItemID );
	const int	GetDungeonClearPaymentItemID() const { return m_iPaymentItemID; }

	//{{ 2012. 07. 09	김민성       이벤트 큐브 오픈 가능 시간
	void	SetEventCubeOpenStartTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec );
	void	SetEventCubeOpenEndTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec );
	CTime	GetEventCubeOpenEventStartTime()				{ return m_tEventCubeStart; }
	CTime	GetEventCubeOpenEventEndTime()				{ return m_tEventCubeEnd; }
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}

	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	void SetEnchantEventStartTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec );
	void SetEnchantEventEndTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec );
	bool IsEnchantEvent();
	//}}

	//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
	//}}

	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}


	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	void	SetHackingUserKick_LUA( bool bValue );
	bool	GetHackingUserKick()							{	return m_bHackingUserKick;	}
	//}}

	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	void	SetEnableOpenRandomCubeDelaySystem_LUA( bool bValue );
	void	SetOpenRandomCubeDelayTime_LUA( double fTime );
	void	SetOpenRandomCubeDelayCount_LUA( int iCount );

	bool	IsEnableOpenRandomCubeDelaySystem() const			{ return m_bEnableOpenRandomCubeDelaySystem; }
	double	GetOpenRandomCubeDelayTime() const					{ return m_fOpenRandomCubeDelayTime; }
	int		GetOpenRandomCubeDelayCount() const					{ return m_iOpenRandomCubeDelayCount; }
	//}}

	//{{ 2012. 08. 11	박세훈	SMS 문자 발송 스크립트
	//}}

	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//#ifdef SERV_2012_PVP_SEASON2_EVENT
//	void SetPvpEventTime_LUA( int iDayType, int iCountNum, int iStartHour, int iStartMin, int iEndtHour, int iEndMin );
//	bool IsPvpEventTime( CTime& tNow, int& iIndex, std::wstring& wstrLastTime, bool& bIsPossible );
//#endif SERV_2012_PVP_SEASON2_EVENT
	//}}
	//{{ 2012. 09. 18   김민성   2012 대전 시즌2 이벤트
	//}}

	//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
	void	SetLanBugOutCheckOption_Lua( void );
	bool	GetLanBugOutCheck( void )			const {	return m_bLanBugOutCheck;	}
	byte	GetLanBugOutCheckMaxCount( void )	const {	return m_byteLanBugOutCheckMaxCount;	}
	float	GetLanBugOutCheckTime( void	)		const {	return m_fLanBugOutCheckTime;	}
	byte	GetLanBugOutCheckCount( void )		const {	return m_byteLanBugOutCheckCount;	}
	float	GetLanBugOutCheckTerm( void )		const {	return m_fLanBugOutCheckTermMin + ( m_fLanBugOutCheckTerm * ( rand() / static_cast<float>( RAND_MAX ) ) );	}
	float	GetLanBugOutCheckRepeatTerm( void )	const {	return m_fLanBugOutCheckRepeatTerm;	}

	void	SetLanBugOutVerifyOption_Lua( void );
	bool	GetLanBugOutVerify( void )			const {	return m_bLanBugOutVerify;	}
	byte	GetLanBugOutVerifyMaxCount( void )	const {	return m_byteLanBugOutVerifyMaxCount;	}
	float	GetLanBugOutVerifyTime( void )		const {	return m_fLanBugOutVerifyTime;	}
	byte	GetLanBugOutVerifyCount( void )		const {	return m_byteLanBugOutVerifyCount;	}
	float	GetLanBugOutVerifyTerm( void )		const {	return m_fLanBugOutVerifyTermMin + ( m_fLanBugOutVerifyTerm * ( rand() / static_cast<float>( RAND_MAX  ) ) );	}
	float	GetLanBugOutVerifyRepeatTerm( void )const {	return m_fLanBugOutVerifyRepeatTerm;	}


	void	SetLanBugOutJustLog_Lua( bool bJustLog ){	m_bLanBugOutJustLog = bJustLog;	}
	bool	GetLanBugOutJustLog( void )			const {	return m_bLanBugOutJustLog;	}
	//}}
	//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
	//}}

	//{{ 핑 상태체크 호스트 변경 - 김민성
	void	SetCheckChangeHostTime_LUA( int iCheckTime );
	int		GetCheckChangeHostTime()					{	return m_iCheckChangeHostTime;	}
	void	SetBattleFieldCheckChangeHostTime_LUA( int iCheckTime );
	int		GetBattleFieldCheckChangeHostTime()					{	return m_iBattleFieldCheckChangeHostTime;	}
	void	SetMaxPingScore_Lua( DWORD dwMaxPingScore ){	m_dwMaxPingScore = dwMaxPingScore;	}
	DWORD	GetMaxPingScore( void ) const{	return m_dwMaxPingScore;	}
	//}
	//{{ 2013. 02. 04  특정시간 가열기 확률 변경 - 김민성
	//}

	//{{ 2010. 11. 12  조효진	GAMEFORGE 채팅 로그 On-Off 가능하도록
	//}}

	void	SetProcessCommunication_Lua( IN bool bProcessCommunicationON );
	bool	GetProcessCommunication( void ) const{	return m_bProcessCommunicationON;	}

	//{{ 2013. 2. 18	박세훈	빙고 보상 아이템 지급 실패 시 재지급 처리
	//}}
	//{{ 2013. 3. 14	박세훈	 로컬 랭킹 시스템
	//}}

	void	SetHostCheckTerm_Lua( IN float bHostCheckTerm ){	m_fHostCheckTerm = bHostCheckTerm;	}
	float	GetHostCheckTerm( void ) const{	return m_fHostCheckTerm;	}




	void	ResetTimeEnchantEventInfo_Lua( void ) const;

	void	ResetExchangeLimitInfo_Lua( void ) const;

	void	AfterWorkStorageMessageDeleteTerm_Second_LUA( IN int iAwsCriterionNum, IN int iAwsUnderTerm, IN int iAwsAboveOrEqualTerm );
	int		GetAwsCriterionNum( void )		const{	return m_iAwsCriterionNum;	}
	int		GetAwsUnderTerm( void )			const{	return m_iAwsUnderTerm;	}
	int		GetAwsAboveOrEqualTerm( void )	const{	return m_iAwsAboveOrEqualTerm;	}

protected:
	//## Game System value..
	//{{

	std::vector< KPcBangReward > m_vecPcBangReward;
	// KOREA NEXON GAME BANG BONUS..
	float			m_fValue[GSVT_TOTAL_NUM];
	//}}

	int				m_iLimitsMaxLevel;		// 제한 레벨..
	int				m_iGuestUserLimitLevel; // 체험ID 제한 레벨

	std::wstring	m_wstrAdvertisementURL; // 클라이언트 종료 광고 URL

	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	std::map< char, std::vector< int > > m_mapPcBangPremiumItem; // PC방 프리미엄 아이템
	UidType			m_iTempItemUID;			// PC방 전용 임시 발급 ItemUID
	//}}
	//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
	bool			m_bIsHackUserRegRejected;
	bool			m_bIsHackUserRegMonitors;
	bool			m_bIsHackUserKick;
	//}}
	bool			m_bDisagreeHackUserFunc;	// 동의 안한 핵 유저에 관한 처리 유/무
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	bool			m_bIsHackUserBlockTrade;
	//}}
	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	CTime			m_tAccountStatisticsStartDate;
	//}}
	//{{ 2011. 01. 18	최육사	캐릭터 카운트 정보
	CTime			m_tCharStatisticsStartDate;
	//}}

	//{{ 2011. 02. 06	최육사	거래 차단 기능
	bool			m_bBlockPersonalTrade;
	bool			m_bBlockPersonalShop;
	bool			m_bBlockSendLetter;
	//}}
	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	bool			m_bMachineIDBlock;
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	bool			m_bMachineIDDuplicateCheck;
	//}}
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	bool			m_bIsHackUserPacketMornitor;
	//}}

	bool			m_bIsNewUnitTradeBlock;
	int				m_iNewUnitTradeBlockDay;
	int				m_iNewUnitTradeBlockUnitClass;

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	int				m_iLimitConutOfLetter;
	int				m_iLetterMonitoringCheckTime;

	int				m_iLimitConutOfTrade;
	int				m_iTradeMonitoringCheckTime;

	bool			m_bLetterAndTradeMonitoringIsOn;

	int				m_iMonitoringMinED;
	int				m_iMonitoringMaxED;
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	int				m_iDeserializeFailCheckCount;
	bool			m_bDeserializeFailCheck;
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	std::map< int, int >			m_mapComeBackRewardCondition;		// 복귀 유저 (단계, 기간)
	//}}

	//{{ 2011. 09. 06  김민성	개인 상점 물품 등록 가격 비교
	//}}

	//{{ 2011. 09. 07  김민성	서브 스테이지 DIE NPC 시간 체크 기능
	bool							m_bSubStageNpcDieCheck;
	//}}


	int								m_iPaymentItemID;

	//{{ 2012. 07. 09	김민성       이벤트 큐브 오픈 가능 시간
	CTime							m_tEventCubeStart;
	CTime							m_tEventCubeEnd;
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}
	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	CTime						m_tEventEnchantStart;
	CTime						m_tEventEnchantEnd;
	//}}

	//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
	//}}

	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	bool						m_bHackingUserKick;
	//}}

	//{{ 2011. 06. 07    김민성    우편 및 거래 감시 실시간 계정 블록
	CTimeSpan					m_tLetterAndTradeBlockStartTime;
	CTimeSpan					m_tLetterAndTradeBlockEndTime;
	//}}
	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	double						m_fOpenRandomCubeDelayTime;
	int							m_iOpenRandomCubeDelayCount;
	bool						m_bEnableOpenRandomCubeDelaySystem;
	//}}

	//{{ 2012. 08. 11	박세훈	SMS 문자 발송 스크립트
	//}}

	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//#ifdef SERV_2012_PVP_SEASON2_EVENT
	//{{ 2012. 09. 18   김민성   2012 대전 시즌2 이벤트
	//}}
//#endif SERV_2012_PVP_SEASON2_EVENT
	//}}

	//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
	bool						m_bLanBugOutCheck;
	byte						m_byteLanBugOutCheckMaxCount;
	float						m_fLanBugOutCheckTime;
	byte						m_byteLanBugOutCheckCount;
	float						m_fLanBugOutCheckTermMin;
	float						m_fLanBugOutCheckTerm;
	float						m_fLanBugOutCheckRepeatTerm;

	bool						m_bLanBugOutVerify;
	byte						m_byteLanBugOutVerifyMaxCount;
	float						m_fLanBugOutVerifyTime;
	byte						m_byteLanBugOutVerifyCount;
	float						m_fLanBugOutVerifyTermMin;
	float						m_fLanBugOutVerifyTerm;
	float						m_fLanBugOutVerifyRepeatTerm;

	bool						m_bLanBugOutJustLog;
	//}}
	//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
	//}}
	//{{ 핑 상태체크 호스트 변경 - 김민성
	int							m_iCheckChangeHostTime;
    int							m_iBattleFieldCheckChangeHostTime;
	DWORD						m_dwMaxPingScore;
	//}
	//{{ 2013. 02. 04  특정시간 가열기 확률 변경 - 김민성
	//}

	//{{ 2010. 11. 12  조효진	GAMEFORGE 채팅 로그 On-Off 가능하도록
	//}}
	//{{ 2013. 2. 18	박세훈	빙고 보상 아이템 지급 실패 시 재지급 처리
	//}}
	//{{ 2013. 3. 14	박세훈	 로컬 랭킹 시스템
	//}}

	float						m_fHostCheckTerm;

	bool						m_bProcessCommunicationON;




	int		m_iAwsCriterionNum;		// 처리 시간을 변경할 기준 수량
	int		m_iAwsUnderTerm;			// 기준 수량 미만일 경우의 처리 간격
	int		m_iAwsAboveOrEqualTerm;	// 기준 수량 이상일 경우의 처리 간격
};

DefRefreshSingletonInline( KGameSysVal );
