#include "GameSysVal.h"
#include <algorithm>
//{{ 2010. 01. 05  최육사	PC방 프리미엄
#include "X2Data/XSLItemManager.h"
//}}
//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	#include "EventDelaySystemManager.h"
//}}

#include "GameEvent/GameEventManager.h"

ImplementRefreshSingleton( KGameSysVal );

KGameSysVal::KGameSysVal()
{

	m_iLimitsMaxLevel		= 0;
	m_iGuestUserLimitLevel	= 0;
	m_iTempItemUID			= 0;

	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	// DB에서 발급하는 ItemUID와 절대로 값이 겹쳐지면 안된다.
	// 그래서 서버에서 임시로 발급하는 TempUID를 ItemUID값으로 쓴다. ( 발급할때마다 1씩 증가 )
	KncUid::SetTempUID( m_iTempItemUID );
	KncUid::SetServerGroupID( m_iTempItemUID, KBaseServer::GetKObj()->GetServerGroupID() );
	KncUid::SetServerID( m_iTempItemUID, KBaseServer::GetKObj()->GetUID() );
	//}}

	//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
	m_bIsHackUserRegRejected = false;
	m_bIsHackUserRegMonitors = false;
	m_bIsHackUserKick = false;
	//}}
	m_bDisagreeHackUserFunc = false; // 해외 기본이 false 이므로...
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	m_bIsHackUserBlockTrade = false;
	//}}
	//{{ 2011. 02. 06	최육사	거래 차단 기능
	m_bBlockPersonalTrade = false;
	m_bBlockPersonalShop = false;
	m_bBlockSendLetter = false;
	//}}
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	m_bIsHackUserPacketMornitor = false;
	//}}

	m_bIsNewUnitTradeBlock = false;
	m_iNewUnitTradeBlockDay = 0;
	m_iNewUnitTradeBlockUnitClass = 0;

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	m_iLimitConutOfLetter				= 15;
	m_iLimitConutOfTrade				= 15;
	m_iLetterMonitoringCheckTime		= 300;
	m_iTradeMonitoringCheckTime			= 300;
	m_bLetterAndTradeMonitoringIsOn		= false;
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	m_iDeserializeFailCheckCount = 9999;
	m_bDeserializeFailCheck = false;
	//}}

	//{{ 2011. 09. 07  김민성	서브 스테이지 DIE NPC 시간 체크 기능
	m_bSubStageNpcDieCheck		= true;
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}

	//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
	//}}
	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}
	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	m_bHackingUserKick = false;
	//}}
	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	m_bEnableOpenRandomCubeDelaySystem = false;
	m_fOpenRandomCubeDelayTime = 1.0;
	m_iOpenRandomCubeDelayCount = 100;
	//}}

	//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
	m_bLanBugOutCheck				= false;
	m_byteLanBugOutCheckMaxCount	= 0;
	m_fLanBugOutCheckTime			= 1.0f;
	m_byteLanBugOutCheckCount		= 3;
	m_fLanBugOutCheckTermMin		= 0.0f;
	m_fLanBugOutCheckTerm			= 5.0f;
	m_fLanBugOutCheckRepeatTerm		= 0.0f;

	m_bLanBugOutVerify				= false;
	m_byteLanBugOutVerifyMaxCount	= 1;
	m_fLanBugOutVerifyTime			= 1.0f;
	m_byteLanBugOutVerifyCount		= 1;
	m_fLanBugOutVerifyTermMin		= 0.0f;
	m_fLanBugOutVerifyTerm			= 5.0f;
	m_fLanBugOutVerifyRepeatTerm	= 0.0f;

	m_bLanBugOutJustLog				= false;
	//}}


	//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
	//}}
	//{{ 핑 상태체크 호스트 변경 - 김민성
	m_iCheckChangeHostTime	= 180;		// 기존 180 초
    m_iBattleFieldCheckChangeHostTime = 60;
    m_dwMaxPingScore		= 500;
	//}
	//{{ 2010. 11. 12  조효진	GAMEFORGE 채팅 로그 On-Off 가능하도록
	//}}
	//{{ 2013. 2. 18	박세훈	빙고 보상 아이템 지급 실패 시 재지급 처리
	//}}
	//{{ 2013. 3. 14	박세훈	 로컬 랭킹 시스템
	//}}

	m_iPaymentItemID = 0;

	m_fHostCheckTerm	= 3.0;

	m_bProcessCommunicationON = false;


	m_iAwsCriterionNum		= 100;
	m_iAwsUnderTerm			= 5;
	m_iAwsAboveOrEqualTerm	= 300;

}

KGameSysVal::~KGameSysVal()
{

}

ImplToStringW( KGameSysVal )
{
	stm_	<< L"----------[ Game Sys Value ]----------" << std::endl
		;
	for( size_t iPcBangIndex = 0; iPcBangIndex < m_vecPcBangReward.size(); ++iPcBangIndex )
	{
		stm_	<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_iPcBangType )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_fEXPRate )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_fEDRate )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_fVSPointRate )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_bSpiritFree )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_fPetEXPRate )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_iAdditionalHenirReward )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_bSecretDungeonFree )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_bAvatar )
				<< TOSTRINGW( m_vecPcBangReward[ iPcBangIndex ].m_bTitle );
	}
	stm_
//#ifdef SERV_VIP_SYSTEM
			<< TOSTRINGW( m_fValue[GSVT_VIP_EXP] )
//#endif //SERV_VIP_SYSTEM
			<< TOSTRINGW( m_iLimitsMaxLevel )
			<< TOSTRINGW( m_iGuestUserLimitLevel )
			<< TOSTRINGW( m_wstrAdvertisementURL )
			<< TOSTRINGW( m_mapPcBangPremiumItem.size() )
			<< TOSTRINGW( m_iTempItemUID )
			//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
			<< TOSTRINGW( m_bIsHackUserRegRejected )
			<< TOSTRINGW( m_bIsHackUserRegMonitors )
			<< TOSTRINGW( m_bIsHackUserKick )
			//}}
			//{{ 2011. 02. 06	최육사	거래 차단 기능
			<< TOSTRINGW( m_bBlockPersonalTrade )
			<< TOSTRINGW( m_bBlockPersonalShop )
			<< TOSTRINGW( m_bBlockSendLetter )
			//}}
			//{{ 2011. 08. 09	최육사	머신ID 블럭기능
			<< TOSTRINGW( m_bMachineIDBlock )
			//}}
			//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
			<< TOSTRINGW( m_bIsHackUserPacketMornitor )
			//}}
			<< TOSTRINGW( m_bIsNewUnitTradeBlock )
			<< TOSTRINGW( m_iNewUnitTradeBlockDay )
			<< TOSTRINGW( m_iNewUnitTradeBlockUnitClass )
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
			<< TOSTRINGW( m_bMachineIDDuplicateCheck )
			//}}
			//{{ 2011. 04. 26	최육사	deserialize 실패 종료
			<< TOSTRINGW( m_iDeserializeFailCheckCount )
			<< TOSTRINGW( m_bDeserializeFailCheck )
			//}}
			//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
			<< TOSTRINGW( m_bEnableOpenRandomCubeDelaySystem )
			<< TOSTRINGW( m_fOpenRandomCubeDelayTime )
			<< TOSTRINGW( m_iOpenRandomCubeDelayCount )
			//}}
			//{{ 핑 상태체크 호스트 변경 - 김민성
			<< TOSTRINGW( m_iCheckChangeHostTime )
            << TOSTRINGW( m_iBattleFieldCheckChangeHostTime )
			//}
			;

	return stm_;
}

ImplementLuaScriptParser( KGameSysVal )
{
	lua_tinker::class_add<KGameSysVal>( GetLuaState(), "KGameSysVal" );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "dump",							&KGameSysVal::Dump );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AddPCBang",						&KGameSysVal::AddPCBang_LUA );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AddLimitsMaxLevel",				&KGameSysVal::AddLimitsMaxLevel_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AddPremiumEXP",					&KGameSysVal::AddPremiumEXP_LUA );
//#ifdef SERV_VIP_SYSTEM
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AddVIPEXP",						&KGameSysVal::AddVIPEXP_LUA );
//#endif //SERV_VIP_SYSTEM
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetGuestUserLimitLevel",		&KGameSysVal::SetGuestUserLimitLevel_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetAdvertisementURL",			&KGameSysVal::SetAdvertisementURL_LUA );
	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AddPcBangPremiumItem",			&KGameSysVal::AddPcBangPremiumItem_LUA );
	//}}
	//{{ 2010. 7. 28  최육사	오토핵 차단 NPC
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetIsHackUserRegRejected",		&KGameSysVal::SetIsHackUserRegRejected_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetIsHackUserRegMonitors",		&KGameSysVal::SetIsHackUserRegMonitors_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetIsHackUserKick",				&KGameSysVal::SetIsHackUserKick_LUA );
	//}}

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetDisagreeHackUserFunc",			&KGameSysVal::SetDisagreeHackUserFunc_LUA );

	//{{ 2011. 03. 02	최육사	개인 거래 차단
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetIsHackUserBlockTrade",		&KGameSysVal::SetIsHackUserBlockTrade_LUA );
	//}}
	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetAccountStatisticsStartDate",	&KGameSysVal::SetAccountStatisticsStartDate_LUA );
	//}}
	//{{ 2011. 01. 18	최육사	캐릭터 카운트 정보
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetCharStatisticsStartDate",	&KGameSysVal::SetCharStatisticsStartDate_LUA );
	//}}
	//{{ 2011. 02. 06	최육사	거래 차단 기능
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetBlockPersonalTrade",			&KGameSysVal::SetBlockPersonalTrade_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetBlockPersonalShop",			&KGameSysVal::SetBlockPersonalShop_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetBlockSendLetter",			&KGameSysVal::SetBlockSendLetter_LUA );
	//}}
	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMachineIDBlock",				&KGameSysVal::SetMachineIDBlock_LUA );
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMachineIDDuplicateCheck",	&KGameSysVal::SetMachineIDDuplicateCheck_LUA );
	//}}
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetHackUserPacketMornitor",		&KGameSysVal::SetHackUserPacketMornitor_LUA );
	//}}

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetIsNewUnitTradeBlock",		&KGameSysVal::SetIsNewUnitTradeBlock_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetNewUnitTradeBlockDay",		&KGameSysVal::SetNewUnitTradeBlockDay_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetNewUnitTradeBlockUnitClass",		&KGameSysVal::SetNewUnitTradeBlockUnitClass_LUA );

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLetterAndTradeMonitoringIsOn",	&KGameSysVal::SetLetterAndTradeMonitoringIsOn_Lua );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLimitsCountOfLetter",			&KGameSysVal::SetLimitsCountOfLetter_Lua );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLetterMonitoringCheckTime",		&KGameSysVal::SetLetterMonitoringCheckTime_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLimitsCountOfTrade",				&KGameSysVal::SetLimitsCountOfTrade_Lua );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetTradeMonitoringCheckTime",		&KGameSysVal::SetTradeMonitoringCheckTime_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMonitoringMinED",				&KGameSysVal::SetMonitoringMinED_Lua );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMonitoringMaxED",				&KGameSysVal::SetMonitoringMaxED_Lua );
	//{{ 2011. 06. 07    김민성    우편 및 거래 감시 실시간 계정 블록
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMonitoringLetterAndTradeBlockTime",				&KGameSysVal::SetMonitoringLetterAndTradeBlockTime_LUA );
	//}}
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetDeserializeFailCheckCount",		&KGameSysVal::SetDeserializeFailCheckCount_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetDeserializeFailCheck",			&KGameSysVal::SetDeserializeFailCheck_LUA );
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetComeBackReward",					&KGameSysVal::SetComeBackReward_LUA );
	//}}

	//{{ 2011. 09. 06  김민성	개인 상점 물품 등록 가격 비교
	//}}

	//{{ 2011. 09. 07  김민성	서브 스테이지 DIE NPC 시간 체크 기능
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetSubStageNpcDieCheck",					&KGameSysVal::SetSubStageNpcDieCheck_LUA );
	//}}

	//{{ 2012. 07. 09	김민성       이벤트 큐브 오픈 가능 시간
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetEventCubeOpenStartTime",					&KGameSysVal::SetEventCubeOpenStartTime_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetEventCubeOpenEndTime",					&KGameSysVal::SetEventCubeOpenEndTime_LUA );
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}

	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetEnchantEventStartTime",					&KGameSysVal::SetEnchantEventStartTime_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetEnchantEventEndTime",					&KGameSysVal::SetEnchantEventEndTime_LUA );
	//}}

	//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
	//}}

	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetHackingUserKick",					&KGameSysVal::SetHackingUserKick_LUA );
	//}}
	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetEnableOpenRandomCubeDelaySystem",	&KGameSysVal::SetEnableOpenRandomCubeDelaySystem_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetOpenRandomCubeDelayTime",			&KGameSysVal::SetOpenRandomCubeDelayTime_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetOpenRandomCubeDelayCount",			&KGameSysVal::SetOpenRandomCubeDelayCount_LUA );
	//}}

	//{{ 2012. 08. 11	박세훈	SMS 문자 발송 스크립트
	//}}

	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//#ifdef SERV_2012_PVP_SEASON2_EVENT
//	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetPvpEventTime",	&KGameSysVal::SetPvpEventTime_LUA );
//#endif SERV_2012_PVP_SEASON2_EVENT
//}}

	//{{ 2012. 09. 18   김민성   2012 대전 시즌2 이벤트
	//}}

	//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLanBugOutCheckOption",	&KGameSysVal::SetLanBugOutCheckOption_Lua );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLanBugOutVerifyOption",	&KGameSysVal::SetLanBugOutVerifyOption_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetLanBugOutJustLog",		&KGameSysVal::SetLanBugOutJustLog_Lua );
	//}}


	//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
	//}}
	//{{ 핑 상태체크 호스트 변경 - 김민성
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetCheckChangeHostTime",	&KGameSysVal::SetCheckChangeHostTime_LUA );
    lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetBattleFieldCheckChangeHostTime",	&KGameSysVal::SetBattleFieldCheckChangeHostTime_LUA );
	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetMaxPingScore",		&KGameSysVal::SetMaxPingScore_Lua );
	//}
	//{{ 2013. 02. 04  특정시간 가열기 확률 변경 - 김민성
	//}


	//{{ 2013. 2. 18	박세훈	빙고 보상 아이템 지급 실패 시 재지급 처리
	//}}
	//{{ 2013. 3. 14	박세훈	 로컬 랭킹 시스템
	//}}

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetHostCheckTerm",				&KGameSysVal::SetHostCheckTerm_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "SetProcessCommunication",				&KGameSysVal::SetProcessCommunication_Lua );



	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "ResetTimeEnchantEventInfo",		&KGameSysVal::ResetTimeEnchantEventInfo_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "ResetExchangeLimitInfo",		&KGameSysVal::ResetExchangeLimitInfo_Lua );

	lua_tinker::class_def<KGameSysVal>( GetLuaState(), "AfterWorkStorageMessageDeleteTerm_Second",		&KGameSysVal::AfterWorkStorageMessageDeleteTerm_Second_LUA );

	lua_tinker::decl( GetLuaState(), "GameSysVal", this );
}

void KGameSysVal::AddPCBang_LUA( int iPcBangType, float fEXPRate, float fEDRate, float fVSPointRate, bool bSpiritFree, float fPetEXPRate,
								 int iAdditionalHenirReward, bool bSecretDungeonFree, bool bAvatar, bool bTitle )
{
	if( iPcBangType < 0 )
	{
		START_LOG( cerr, L"Invalid PC Bang type." )
			<< BUILD_LOG( iPcBangType )
			<< END_LOG;

		return;
	}

	m_vecPcBangReward.resize( iPcBangType + 1 );
	KPcBangReward& kPcBangReward = m_vecPcBangReward[ iPcBangType ];

	if( fEXPRate < 0.0f )
	{
		START_LOG( cerr, L"Invalid PC Bang EXP reward rate." )
			<< BUILD_LOG( fEXPRate )
			<< END_LOG;

		fEXPRate = 0.0f;
	}

	if( fEDRate < 0.0f )
	{
		START_LOG( cerr, L"Invalid PC Bang ED reward rate." )
			<< BUILD_LOG( fEDRate )
			<< END_LOG;

		fEDRate = 0.0f;
	}

	if( fVSPointRate < 0.0f )
	{
		START_LOG( cerr, L"Invalid PC Bang VS Point reward rate." )
			<< BUILD_LOG( fVSPointRate )
			<< END_LOG;

		fVSPointRate = 0.0f;
	}

	if( fPetEXPRate < 0.0f )
	{
		START_LOG( cerr, L"Invalid PC Bang pet reward rate." )
			<< BUILD_LOG( fPetEXPRate )
			<< END_LOG;

		fPetEXPRate = 0.0f;
	}

	if( iAdditionalHenirReward < 0 )
	{
		START_LOG( cerr, L"Invalid Henir bonus reward count." )
			<< BUILD_LOG( iAdditionalHenirReward )
			<< END_LOG;

		iAdditionalHenirReward = 0;
	}

	kPcBangReward.m_iPcBangType				= iPcBangType;
	kPcBangReward.m_fEXPRate				= fEXPRate;
	kPcBangReward.m_fEDRate					= fEDRate;
	kPcBangReward.m_fVSPointRate			= fVSPointRate;
	kPcBangReward.m_bSpiritFree				= bSpiritFree;
	kPcBangReward.m_fPetEXPRate				= fPetEXPRate;
	kPcBangReward.m_iAdditionalHenirReward	= iAdditionalHenirReward;
	kPcBangReward.m_bSecretDungeonFree		= bSecretDungeonFree;
	kPcBangReward.m_bAvatar					= bAvatar;
	kPcBangReward.m_bTitle					= bTitle;


	START_LOG( cout2, L"PC Bang bonuses added by type." )
		<< BUILD_LOG( kPcBangReward.m_iPcBangType )
		<< BUILD_LOG( kPcBangReward.m_fEXPRate )
		<< BUILD_LOG( kPcBangReward.m_fEDRate )
		<< BUILD_LOG( kPcBangReward.m_fVSPointRate )
		<< BUILD_LOG( kPcBangReward.m_bSpiritFree )
		<< BUILD_LOG( kPcBangReward.m_fPetEXPRate )
		<< BUILD_LOG( kPcBangReward.m_iAdditionalHenirReward )
		<< BUILD_LOG( kPcBangReward.m_bSecretDungeonFree )
		<< BUILD_LOG( kPcBangReward.m_bAvatar )
		<< BUILD_LOG( kPcBangReward.m_bTitle )
		;

}

void KGameSysVal::AddLimitsMaxLevel_LUA( int iMaxLevel )
{
	if( iMaxLevel < 1 || iMaxLevel > 80  )
	{
		START_LOG( cerr, L"Invalid level limit." )
			<< BUILD_LOG( iMaxLevel )
			<< END_LOG;

		iMaxLevel = 1;
	}

	m_iLimitsMaxLevel = iMaxLevel;

	START_LOG( cout2, L"Level limit configured." )
		<< BUILD_LOG( m_iLimitsMaxLevel )
		;
}

void KGameSysVal::AddPremiumEXP_LUA( float fPremiumEXP )
{
	if( fPremiumEXP < 0.0f )
	{
		START_LOG( cerr, L"Invalid premium experience-item EXP reward rate." )
			<< BUILD_LOG( fPremiumEXP )
			<< END_LOG;

		fPremiumEXP = 0.0f;
	}

	m_fValue[GSVT_PREMIUM_EXP] = fPremiumEXP;

	START_LOG( cout2, L"Premium item EXP bonus rate: " )
		<< BUILD_LOG( m_fValue[GSVT_PREMIUM_EXP] )
		;

}

//#ifdef SERV_VIP_SYSTEM
void KGameSysVal::AddVIPEXP_LUA( float fVIPEXP )
{
	if( fVIPEXP < 0.00f )
	{
		START_LOG( cerr, L"Invalid VIP item EXP reward rate." )
			<< BUILD_LOG( fVIPEXP )
			<< END_LOG;

		fVIPEXP = 0.00f;
	}
	m_fValue[GSVT_VIP_EXP] = fVIPEXP;

	START_LOG( cout2, L"VIP EXP bonus rate: " )
		<< BUILD_LOG( m_fValue[GSVT_VIP_EXP] )
		;

}
//#endif //SERV_VIP_SYSTEM

//{{ 2008. 5. 16  최육사  체험ID 제한
void KGameSysVal::SetGuestUserLimitLevel_LUA( int iGuestUserLimitLevel )
{
	if( iGuestUserLimitLevel < 1 )
	{
		START_LOG( cerr, L"Invalid level value." )
			<< BUILD_LOG( iGuestUserLimitLevel )
			<< END_LOG;

		iGuestUserLimitLevel = 19;
	}

	m_iGuestUserLimitLevel = iGuestUserLimitLevel;

	START_LOG( cout2, L"Trial ID level limit configured." )
		<< BUILD_LOG( m_iGuestUserLimitLevel )
		;
}
//}}

//{{ 2008. 9. 2  최육사		광고 URL
void KGameSysVal::SetAdvertisementURL_LUA( const char* strURL )
{
    m_wstrAdvertisementURL = KncUtil::toWideString( strURL );
}

//{{ 2010. 01. 05  최육사	PC방 프리미엄

void KGameSysVal::AddPcBangPremiumItem_LUA( char cUnitType, int iItemID )
{
	//const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	//if( pItemTemplet == NULL )
	//{
	//	START_LOG( cerr, L"존재하지 않는 아이템을 PC방 프리미엄 아이템으로 세팅하였습니다!" )
	//		<< BUILD_LOG( iItemID )
	//		<< END_LOG;
	//	return;
	//}

	//if( pItemTemplet->m_bIsPcBang == false )
	//{
	//	START_LOG( cerr, L"PC방 전용 아이템이 아닙니다." )
	//		<< BUILD_LOG( iItemID )
	//		<< END_LOG;
	//	return;
	//}

	//if( pItemTemplet->m_PeriodType != CXSLItem::PT_INFINITY )
	//{
	//	START_LOG( cerr, L"기간 무제한일 경우만 PC방 프리미엄 입니다." )
	//		<< BUILD_LOG( iItemID )
	//		<< BUILD_LOG( pItemTemplet->m_PeriodType )
	//		<< END_LOG;
	//	return;
	//}

	std::map< char, std::vector< int > >::iterator mit = m_mapPcBangPremiumItem.find( cUnitType );
	if( mit == m_mapPcBangPremiumItem.end() )
	{
		std::vector< int > vecInfo;
		vecInfo.push_back( iItemID );
		m_mapPcBangPremiumItem.insert( std::make_pair( cUnitType, vecInfo ) );
	}
	else
	{
        mit->second.push_back( iItemID );
	}

	START_LOG( clog2, L"PC Bang premium item: " )
		<< BUILD_LOGc( cUnitType )
		<< BUILD_LOGc( iItemID );
}

//}}

//{{ 2010. 7. 28  최육사	오토핵 차단 NPC

void KGameSysVal::SetIsHackUserRegRejected_LUA( bool bVal )
{
	m_bIsHackUserRegRejected = bVal;

	START_LOG( cout2, L"Register a login block in the database when a suspected cheater is detected: " )
		<< BUILD_LOG( m_bIsHackUserRegRejected );
}

void KGameSysVal::SetIsHackUserRegMonitors_LUA( bool bVal )
{
	m_bIsHackUserRegMonitors = bVal;

	START_LOG( cout2, L"Register suspected cheaters in the database for hack monitoring: " )
		<< BUILD_LOG( m_bIsHackUserRegMonitors );
}

void KGameSysVal::SetIsHackUserKick_LUA( bool bVal )
{
	m_bIsHackUserKick = bVal;

	START_LOG( cout2, L"Immediately disconnect suspected cheaters: " )
		<< BUILD_LOG( m_bIsHackUserKick );
}

//}}

void KGameSysVal::SetDisagreeHackUserFunc_LUA( bool bVal )
{
	m_bDisagreeHackUserFunc = bVal;

	START_LOG( cout2, L"Handle suspected cheaters who have not agreed to the terms: " )
		<< BUILD_LOG( m_bDisagreeHackUserFunc );
}

//{{ 2011. 03. 02	최육사	개인 거래 차단
void KGameSysVal::SetIsHackUserBlockTrade_LUA( bool bVal )
{
	m_bIsHackUserBlockTrade = bVal;

	START_LOG( cout2, L"Immediately suspend trading when a suspected cheater is detected: " )
		<< BUILD_LOG( m_bIsHackUserBlockTrade );
}
//}}

//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
void KGameSysVal::SetAccountStatisticsStartDate_LUA( const char* strDate )
{
	std::wstring wstrAccountStatisticsStartDate = KncUtil::toWideString( std::string( strDate ) );
	LIF( KncUtil::ConvertStringToCTime( wstrAccountStatisticsStartDate, m_tAccountStatisticsStartDate ) );

	START_LOG( cout2, L"Account statistics start date: " )
		<< BUILD_LOG( wstrAccountStatisticsStartDate );
}
//}}

//{{ 2011. 01. 18	최육사	캐릭터 카운트 정보
void KGameSysVal::SetCharStatisticsStartDate_LUA( const char* strDate )
{
	std::wstring wstrCharStatisticsStartDate = KncUtil::toWideString( std::string( strDate ) );
	LIF( KncUtil::ConvertStringToCTime( wstrCharStatisticsStartDate, m_tCharStatisticsStartDate ) );

	START_LOG( cout2, L"Character statistics start date: " )
		<< BUILD_LOG( wstrCharStatisticsStartDate );
}
//}}

void KGameSysVal::GetAdvertisementURL( std::wstring& wstrAdURL )
{
	wstrAdURL = m_wstrAdvertisementURL;
}
//}}

//{{ 2010. 01. 05  최육사	PC방 프리미엄

void KGameSysVal::GetPcBangPremiumItem( IN char cUnitType, OUT std::vector< KInventoryItemInfo >& vecPcBangPremiumItem, int iPcBangType )
{
	vecPcBangPremiumItem.clear();

	if (iPcBangType < 0 ||
		m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType) ||
		m_vecPcBangReward[iPcBangType].m_bAvatar == false)
	{
		START_LOG(clog, L"Not a PC Bang or avatar rewards are disabled for this PC Bang.");
		return;
	}

	if( m_mapPcBangPremiumItem.empty() )
	{
		START_LOG( clog, L"No registered items exclusive to PC bangs." );
		return;
	}

	std::map< char, std::vector< int > >::const_iterator mit = m_mapPcBangPremiumItem.find( cUnitType );
	if( mit == m_mapPcBangPremiumItem.end() )
	{
		START_LOG( cwarn, L"This is an unregistered UnitType." )
			<< BUILD_LOGc( cUnitType )
			<< END_LOG;
		return;
	}

	//{{ 2011. 12. 15	최육사	인벤토리 슬롯ID크기 늘리기
	short sSlotID = 0;
	//}}
	std::vector< int >::const_iterator vit;
	for( vit = mit->second.begin(); vit != mit->second.end(); ++vit )
	{
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( *vit );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"A non-existent item has been configured as a PC Bang premium item!" )
				<< BUILD_LOG( *vit )
				<< END_LOG;
			continue;
		}

		if( pItemTemplet->m_bIsPcBang == false )
		{
			START_LOG( cerr, L"This is not an item exclusive to PC bangs." )
				<< BUILD_LOG( *vit )
				<< END_LOG;
			continue;
		}

		// 임시 ItemUID발급과 동시에 아이템 획득 처리
		KInventoryItemInfo kInventoryItemInfo;
		//{{ 2013. 04. 10	최육사	어둠의 문 개편
		kInventoryItemInfo.m_iItemUID = GetTempItemUID();
		//}}
		kInventoryItemInfo.m_cSlotCategory = CXSLInventory::ST_PC_BANG;
		//{{ 2011. 12. 15	최육사	인벤토리 슬롯ID크기 늘리기
		kInventoryItemInfo.m_sSlotID = sSlotID++;
		//}}

		kInventoryItemInfo.m_kItemInfo.m_iItemID	= *vit;
		kInventoryItemInfo.m_kItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
		kInventoryItemInfo.m_kItemInfo.m_iQuantity	= 1;

		vecPcBangPremiumItem.push_back( kInventoryItemInfo );
	}
}

//}}

//{{ 2011. 02. 06	최육사	거래 차단 기능
void KGameSysVal::SetBlockPersonalTrade_LUA( bool bVal )
{
	m_bBlockPersonalTrade = bVal;

	START_LOG( cout, L"Block private trades: " << m_bBlockPersonalTrade );
}

void KGameSysVal::SetBlockPersonalShop_LUA( bool bVal )
{
	m_bBlockPersonalShop = bVal;

	START_LOG( cout, L"Block personal shops: " << m_bBlockPersonalShop );
}

void KGameSysVal::SetBlockSendLetter_LUA( bool bVal )
{
	m_bBlockSendLetter = bVal;

	START_LOG( cout, L"Block mail sending: " << m_bBlockSendLetter );
}
//}}

//{{ 2011. 08. 09	최육사	머신ID 블럭기능
void KGameSysVal::SetMachineIDBlock_LUA( bool bVal )
{
	m_bMachineIDBlock = bVal;

	START_LOG( cout, L"Machine ID blocking enabled: " << m_bMachineIDBlock );
}

bool KGameSysVal::IsMachineIDBlock() const
{
    return m_bMachineIDBlock;
}
//}}

//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
void KGameSysVal::SetHackUserPacketMornitor_LUA( bool bVal )
{
	m_bIsHackUserPacketMornitor = bVal;

	START_LOG( cout, L"Monitor packets from suspected bot users: " << m_bIsHackUserPacketMornitor );
}
//}}

void KGameSysVal::SetIsNewUnitTradeBlock_LUA( bool bVal )
{
	m_bIsNewUnitTradeBlock = bVal;

	START_LOG( cout, L"Restrict trading for new characters: " << m_bIsNewUnitTradeBlock );
}
void KGameSysVal::SetNewUnitTradeBlockDay_LUA( int iVal )
{
	m_iNewUnitTradeBlockDay = iVal;

	START_LOG( cout, L"New-character trading restriction duration (days): " << m_iNewUnitTradeBlockDay );
}
void KGameSysVal::SetNewUnitTradeBlockUnitClass_LUA( int iVal )
{
	m_iNewUnitTradeBlockUnitClass = iVal;

	START_LOG( cout, L"Character class subject to the new-character trading restriction: " << m_iNewUnitTradeBlockUnitClass );
}

//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
void KGameSysVal::SetMachineIDDuplicateCheck_LUA( bool bVal )
{
	m_bMachineIDDuplicateCheck = bVal;

	START_LOG( cout, L"Machine ID duplicate-login check enabled: " << m_bMachineIDDuplicateCheck );
}

bool KGameSysVal::IsMachineIDDuplicateCheck() const
{
	return m_bMachineIDDuplicateCheck;
}
//}}

//{{ 2011. 04. 05  김민성	우편 및 거래 감시
void KGameSysVal::SetLimitsCountOfLetter_Lua( int iCount )
{
	m_iLimitConutOfLetter = iCount ;

	START_LOG( cout, L"Mail-sending monitor - message limit: " << m_iLimitConutOfLetter );
}
void KGameSysVal::SetLetterMonitoringCheckTime_Lua( int iTime )
{
	m_iLetterMonitoringCheckTime = iTime * 60;

	START_LOG( cout, L"Mail-sending monitor - interval (seconds): " << m_iLetterMonitoringCheckTime );
}

void KGameSysVal::SetLetterAndTradeMonitoringIsOn_Lua( bool bIsOn )
{
	m_bLetterAndTradeMonitoringIsOn = bIsOn;

	START_LOG( cout, L"Mail and trade monitoring enabled: " << m_bLetterAndTradeMonitoringIsOn );
}

void KGameSysVal::SetLimitsCountOfTrade_Lua( int iCount )
{
	m_iLimitConutOfTrade = iCount;

	START_LOG( cout, L"Trade monitor - trade limit: " << m_iLimitConutOfTrade );
}

void KGameSysVal::SetTradeMonitoringCheckTime_Lua( int iTime )
{
	m_iTradeMonitoringCheckTime = iTime * 60;

	START_LOG( cout, L"Trade monitor - interval (seconds): " << m_iTradeMonitoringCheckTime );
}

void KGameSysVal::SetMonitoringMinED_Lua( int iMin )
{
	m_iMonitoringMinED = iMin;
	START_LOG( cout, L"Minimum trade amount (ED): " << m_iMonitoringMinED );
}

void KGameSysVal::SetMonitoringMaxED_Lua( int iMax )
{
	m_iMonitoringMaxED = iMax;
	START_LOG( cout, L"Maximum trade amount (ED): " << m_iMonitoringMaxED );
}
//}}


//{{ 2011. 04. 26	최육사	deserialize 실패 종료
void KGameSysVal::SetDeserializeFailCheckCount_LUA( int iCount )
{
	m_iDeserializeFailCheckCount = iCount;

	START_LOG( cout, L"Deserialize failure check count: " )
		<< BUILD_LOG( m_iDeserializeFailCheckCount );
}

void KGameSysVal::SetDeserializeFailCheck_LUA( bool bVal )
{
	m_bDeserializeFailCheck = bVal;

	START_LOG( cout, L"Deserialize failure check enabled: " )
		<< BUILD_LOG( m_bDeserializeFailCheck );
}
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
void KGameSysVal::SetComeBackReward_LUA( int iStep, int iDays )
{
	if( iStep <= 0 || iDays <= 0 )
	{
		START_LOG( cerr, L"Invalid date information for a returning-player reward stage." )
			<< BUILD_LOG( iStep )
			<< BUILD_LOG( iDays )
			<< END_LOG;
		return;
	}

	std::map< int, int >::iterator	mit = m_mapComeBackRewardCondition.find( iStep );
	if( mit != m_mapComeBackRewardCondition.end() )
	{
		START_LOG( cerr, L"Date information for this returning-player reward stage already exists." )
			<< BUILD_LOG( iStep )
			<< BUILD_LOG( iDays )
			<< END_LOG;
		return;
	}
	m_mapComeBackRewardCondition.insert( std::make_pair( iStep, iDays ) );

	START_LOG( clog, L"Added date information for a returning-player reward stage." )
		<< BUILD_LOG( iStep )
		<< BUILD_LOG( iDays )
		<< END_LOG;
}

void KGameSysVal::GetComeBackRewardCondition( OUT std::map< int, int >& mapComeBackRewardCondition )
{
	mapComeBackRewardCondition = m_mapComeBackRewardCondition;
}
//}}

//{{ 2011. 09. 06  김민성	개인 상점 물품 등록 가격 비교
//}}

//{{ 2011. 12.13    김민성	던전 클리어 시 아이템 지급 이벤트 - 현자의 주문서(중복 지급 금지)
//}}

//{{ 2012. 07. 09	김민성       이벤트 큐브 오픈 가능 시간
void KGameSysVal::SetEventCubeOpenStartTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec )
{
	m_tEventCubeStart = CTime( iYear, iMonth, iDay, iHour, iMin, iSec );

	START_LOG( cout, L"Sage's Scroll event start time: " )
		<< BUILD_LOG( iYear )
		<< BUILD_LOG( iMonth )
		<< BUILD_LOG( iDay )
		<< BUILD_LOG( iHour )
		<< BUILD_LOG( iMin )
		<< BUILD_LOG( iSec )
		<< END_LOG;
}

void KGameSysVal::SetEventCubeOpenEndTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec )
{
	m_tEventCubeEnd = CTime( iYear, iMonth, iDay, iHour, iMin, iSec );

	START_LOG( cout, L"Sage's Scroll event end time: " )
		<< BUILD_LOG( iYear )
		<< BUILD_LOG( iMonth )
		<< BUILD_LOG( iDay )
		<< BUILD_LOG( iHour )
		<< BUILD_LOG( iMin )
		<< BUILD_LOG( iSec )
		<< END_LOG;
}
//}}

//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
//}}

//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
void KGameSysVal::SetEnchantEventStartTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec )
{
	m_tEventEnchantStart = CTime( iYear, iMonth, iDay, iHour, iMin, iSec );

	// 오현빈 // 2014-01-27 // 적용 시간에 대한 로그 추가
	{
		std::wstring wstrBeginTime	= ( m_tEventEnchantStart.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) ).GetString();

		START_LOG( cout, L"Enhancement event start time: " )
			<< BUILD_LOG( wstrBeginTime )
			<< END_LOG;
	}
}

void KGameSysVal::SetEnchantEventEndTime_LUA( int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec )
{
	m_tEventEnchantEnd = CTime( iYear, iMonth, iDay, iHour, iMin, iSec );

	// 오현빈 // 2014-01-27 // 적용 시간에 대한 로그 추가
	{
		std::wstring wstrEndTime	= ( m_tEventEnchantEnd.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) ).GetString();

		START_LOG( cout, L"Enhancement event end time: " )
			<< BUILD_LOG( wstrEndTime )
			<< END_LOG;
	}
}

bool KGameSysVal::IsEnchantEvent()
{
	CTime tNow = CTime::GetCurrentTime();

	if( m_tEventEnchantStart <= tNow && tNow <= m_tEventEnchantEnd )
	{
		START_LOG( clog, L"Event enhancement period is active." )
			<< END_LOG;

		return true;
	}

	START_LOG( clog, L"Event enhancement period is not active." )
		<< END_LOG;
	return false;
}
//}}

//{{ 2012. 02. 06	박세훈	발렌타인 데이 이벤트
//}}

//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
//}}

//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
void KGameSysVal::SetHackingUserKick_LUA( bool bValue )
{
	m_bHackingUserKick = bValue;

	START_LOG( cout, L"Kick suspected cheaters: " )
		<< BUILD_LOG( m_bHackingUserKick )
		<< END_LOG;
}
//}}

//{{ 2011. 06. 07    김민성    우편 및 거래 감시 실시간 계정 블록
void KGameSysVal::SetMonitoringLetterAndTradeBlockTime_LUA( int hh, int mm, int ss, int iDuration_s )
{
	m_tLetterAndTradeBlockStartTime = CTimeSpan( 0, hh, mm, ss );
	m_tLetterAndTradeBlockEndTime = CTimeSpan( 0, hh, mm, ss );

	CTimeSpan tDuration = CTimeSpan( 0, 0, 0, iDuration_s );
	m_tLetterAndTradeBlockEndTime += tDuration;

	START_LOG( cout, L"Real-time account-block duration for mail and trade monitoring: " )
		<< BUILD_LOG( hh )
		<< BUILD_LOG( mm )
		<< BUILD_LOG( ss )
		<< BUILD_LOG( iDuration_s )
		<< BUILD_LOG( m_tLetterAndTradeBlockStartTime.GetTimeSpan() )
		<< BUILD_LOG( m_tLetterAndTradeBlockEndTime.GetTimeSpan() )
		<< END_LOG;

}

bool KGameSysVal::IsMonitoringLetterAndTradeBlockTime( CTimeSpan& tSpan )
{
	if( m_tLetterAndTradeBlockStartTime == m_tLetterAndTradeBlockEndTime )
	{
		return false;
	}

	if( m_tLetterAndTradeBlockStartTime <= tSpan &&  tSpan <= m_tLetterAndTradeBlockEndTime )
	{
		return true;
	}

	return false;
}
//}}

//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
void KGameSysVal::SetEnableOpenRandomCubeDelaySystem_LUA( bool bValue )
{
	m_bEnableOpenRandomCubeDelaySystem = bValue;

	START_LOG( cout, L"Random cube opening delay system enabled: " )
		<< BUILD_LOG( m_bEnableOpenRandomCubeDelaySystem );
}

void KGameSysVal::SetOpenRandomCubeDelayTime_LUA( double fTime )
{
	m_fOpenRandomCubeDelayTime = fTime;

	START_LOG( cout, L"Random cube opening delay: " )
		<< BUILD_LOG( m_fOpenRandomCubeDelayTime );
}

void KGameSysVal::SetOpenRandomCubeDelayCount_LUA( int iCount )
{
	m_iOpenRandomCubeDelayCount = iCount;

	START_LOG( cout, L"Random cube openings allowed per hour: " )
		<< BUILD_LOG( m_iOpenRandomCubeDelayCount );
}
//}}

//{{ 2012. 08. 11	박세훈	SMS 문자 발송 스크립트
//}}

/*
//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
#ifdef SERV_2012_PVP_SEASON2_EVENT
void KGameSysVal::SetPvpEventTime_LUA( int iDayType, int iCountNum, int iStartHour, int iStartMin, int iEndtHour, int iEndMin )
{
	if( iDayType < 0 || iCountNum < 0 || iStartHour < 0 || iStartMin < 0 || iEndtHour < 0 || iEndMin < 0 )
	{
		START_LOG( cerr, L"Failed to set the PvP event time." )
			<< BUILD_LOG( iDayType )
			<< BUILD_LOG( iCountNum )
			<< BUILD_LOG( iStartHour )
			<< BUILD_LOG( iStartMin )
			<< BUILD_LOG( iEndtHour )
			<< BUILD_LOG( iEndMin )
			<< END_LOG;
		return;
	}

	// 주말: iDayType = 1
	// 평일 : iDayType = 2

	CTimeSpan tStartPvpEvent = CTimeSpan( 0, iStartHour, iStartMin, 0 );
	CTimeSpan tEndPvpEvent = CTimeSpan( 0, iEndtHour, iEndMin, 0 );

	std::map< int, std::map< int, CTimeSpan > >::iterator mitS = m_mapStartPvpEvent.find( iDayType );
	std::map< int, std::map< int, CTimeSpan > >::iterator mitE = m_mapEndPvpEvent.find( iDayType );

	// 없다
	if( mitS == m_mapStartPvpEvent.end() )
	{
		std::map< int, CTimeSpan > mapNew;
		mapNew.insert( std::make_pair( iCountNum, tStartPvpEvent ) );
		m_mapStartPvpEvent.insert( std::make_pair( iDayType, mapNew ) );
	}
	else // 있다
	{
		std::map< int, CTimeSpan >::iterator mit = mitS->second.find( iCountNum );
		if( mit == mitS->second.end() )
		{
			mitS->second.insert( std::make_pair( iCountNum, tStartPvpEvent ) );
		}
		else
		{
			START_LOG( cerr, L"Duplicate PvP event time-setting request." )
				<< BUILD_LOG( iDayType )
				<< BUILD_LOG( iCountNum )
				<< BUILD_LOG( iStartHour )
				<< BUILD_LOG( iStartMin )
				<< BUILD_LOG( iEndtHour )
				<< BUILD_LOG( iEndMin )
				<< END_LOG;
		}
	}

	// 없다
	if( mitE == m_mapEndPvpEvent.end() )
	{
		std::map< int, CTimeSpan > mapNew;
		mapNew.insert( std::make_pair( iCountNum, tEndPvpEvent ) );
		m_mapEndPvpEvent.insert( std::make_pair( iDayType, mapNew ) );
	}
	else // 있다
	{
		std::map< int, CTimeSpan >::iterator mit = mitE->second.find( iCountNum );
		if( mit == mitE->second.end() )
		{
			mitE->second.insert( std::make_pair( iCountNum, tEndPvpEvent ) );
		}
		else
		{
			START_LOG( cerr, L"Duplicate PvP event time-setting request." )
				<< BUILD_LOG( iDayType )
				<< BUILD_LOG( iCountNum )
				<< BUILD_LOG( iStartHour )
				<< BUILD_LOG( iStartMin )
				<< BUILD_LOG( iEndtHour )
				<< BUILD_LOG( iEndMin )
				<< END_LOG;
		}
	}

	START_LOG( cout, L"PVP EVENT START TIME SETTING" )
		<< BUILD_LOG( iDayType )
		<< BUILD_LOG( iCountNum )
		<< BUILD_LOG( iStartHour )
		<< BUILD_LOG( iStartMin )
		<< BUILD_LOG( iEndtHour )
		<< BUILD_LOG( iEndMin )
		<< END_LOG;
}

bool KGameSysVal::IsPvpEventTime( CTime& tNow, int& iIndex, std::wstring& wstrLastTime, bool& bIsPossible )
{
	bIsPossible = false;

	// 마지막 받은 시간 변환
	CTime tLastTime;
	if( wstrLastTime.empty() == false )
	{
		if( KncUtil::ConvertStringToCTime( wstrLastTime, tLastTime ) == false )
		{
			START_LOG( clog, L"Time conversion failed." )
				<< BUILD_LOG( wstrLastTime )
				<< END_LOG;
		}
	}

	// 현재 시간이 pvp 이벤트 시간인지 확인하자.
	int iDayOfWeek = tNow.GetDayOfWeek();
	int iDayType = 0;

	switch( iDayOfWeek )
	{
	case 2:	case 3:	case 4:	case 5:	case 6: //(월요일~금요일)
		{
			iDayType = 2;		// 평일
		}
		break;
	case 1: case 7:	//(토요일,일요일)
		{
			iDayType = 1;		// 주말
		}break;
	default:
		{
			START_LOG( cerr, L"Day of the week does not exist." )
				<< BUILD_LOG( iDayOfWeek )
				<< END_LOG;

			return false;
		}break;
	}

	std::map< int, std::map< int, CTimeSpan > >::iterator mitS = m_mapStartPvpEvent.find( iDayType );
	if( mitS == m_mapStartPvpEvent.end() )
	{
		START_LOG( cerr, L"Data does not exist." )
			<< BUILD_LOG( iDayOfWeek )
			<< BUILD_LOG( iDayType )
			<< END_LOG;

		return false;
	}

	std::map< int, std::map< int, CTimeSpan > >::iterator mitE = m_mapEndPvpEvent.find( iDayType );
	if( mitE == m_mapEndPvpEvent.end() )
	{
		START_LOG( cerr, L"Data does not exist." )
			<< BUILD_LOG( iDayOfWeek )
			<< BUILD_LOG( iDayType )
			<< END_LOG;

		return false;
	}

	std::map< int, CTimeSpan >::iterator mitStart = mitS->second.begin();
	std::map< int, CTimeSpan >::iterator mitEnd = mitE->second.begin();
	for( ; mitStart != mitS->second.end(), mitEnd != mitE->second.end() ; ++mitStart, ++mitEnd )
	{
		// 제일 마지막으로 확인한 index 를 얻어온다.
		iIndex = mitStart->first;

		CTime tTempStart = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), mitStart->second.GetHours(), mitStart->second.GetMinutes(), 0 );
		CTime tTempEnd = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), mitEnd->second.GetHours(), mitEnd->second.GetMinutes(), 0 );

		if( tTempStart <= tNow && tNow <= tTempEnd )
		{
			if( mitStart->first == mitEnd->first )
			{
				if( tLastTime < tTempStart )
				{
					bIsPossible = true;
				}
				return true;
			}
			else
			{
				START_LOG( cerr, L"Data does not exist." )
					<< BUILD_LOG( iDayOfWeek )
					<< BUILD_LOG( iDayType )
					<< BUILD_LOG( mitStart->first )
					<< BUILD_LOG( mitEnd->first )
					<< END_LOG;
			}
		}
	}

	return false;
}
#endif SERV_2012_PVP_SEASON2_EVENT
//}}
*/


//{{ 2012. 09. 18   김민성   2012 대전 시즌2 이벤트
//}}

//{{ 2012. 10. 10	박세훈	튕긴 유저 마을로 보내기
//}}

//{{ 핑 상태체크 호스트 변경 - 김민성
void KGameSysVal::SetCheckChangeHostTime_LUA( int iCheckTime )
{
	m_iCheckChangeHostTime = iCheckTime;

	START_LOG( cout2, L"Room leader change timeout updated based on the ping check." )
		<< BUILD_LOG( m_iCheckChangeHostTime )
		<< END_LOG;
}

void KGameSysVal::SetBattleFieldCheckChangeHostTime_LUA( int iCheckTime )
{
	m_iBattleFieldCheckChangeHostTime = iCheckTime;

	START_LOG( cout2, L"Field leader change timeout updated based on the ping check." )
		<< BUILD_LOG( m_iBattleFieldCheckChangeHostTime )
		<< END_LOG;
}

//}

//{{ 2013. 02. 04  특정시간 가열기 확률 변경 - 김민성
//}

//{{ 2010. 11. 12  조효진	GAMEFORGE 채팅 로그 On-Off 가능하도록
//}}

float KGameSysVal::GetGBEXPRate( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType) )
	{
		return 0.0f;
	}

	return m_vecPcBangReward[iPcBangType].m_fEXPRate;
}

float KGameSysVal::GetGBEDRate( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return 0.0f;
	}

	return m_vecPcBangReward[iPcBangType].m_fEDRate;
}

float KGameSysVal::GetGBVPRate( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return 0.0f;
	}

	return m_vecPcBangReward[iPcBangType].m_fVSPointRate;
}

bool KGameSysVal::IsSpiritFree( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return false;
	}

	return m_vecPcBangReward[iPcBangType].m_bSpiritFree;
}

float KGameSysVal::GetGBPetEXPRate( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return 0.0f;
	}

	return m_vecPcBangReward[iPcBangType].m_fPetEXPRate;
}

bool KGameSysVal::IsSecretDungeonFree( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return false;
	}

	return m_vecPcBangReward[iPcBangType].m_bSecretDungeonFree;
}

int KGameSysVal::GetAdditionalHenirRewardCount( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return false;
	}

	return m_vecPcBangReward[iPcBangType].m_iAdditionalHenirReward;
}

bool KGameSysVal::IsUsingPcBangTitle( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		return false;
	}

	return m_vecPcBangReward[iPcBangType].m_bTitle;
}

KPcBangReward KGameSysVal::GetPcBangReward( int iPcBangType )
{
	if( iPcBangType < 0 || m_vecPcBangReward.size() <= static_cast<size_t>(iPcBangType))
	{
		// When the location is not an internet cafe, the 'iPcBangType' value is -1; since this is a valid value, the corresponding error log is removed.

		//START_LOG( cerr, L"피씨방 타입 이상!" )
		//	<< BUILD_LOG( iPcBangType )
		//	<< BUILD_LOG( m_vecPcBangReward.size() )
		//	<< END_LOG;

		KPcBangReward kPcBangReward;
		kPcBangReward.m_iPcBangType				= -1;
		kPcBangReward.m_fEXPRate				= 0.0f;
		kPcBangReward.m_fEDRate					= 0.0f;
		kPcBangReward.m_fVSPointRate			= 0.0f;
		kPcBangReward.m_bSpiritFree				= false;
		kPcBangReward.m_fPetEXPRate				= 0.0f;
		kPcBangReward.m_iAdditionalHenirReward	= 0;
		kPcBangReward.m_bSecretDungeonFree		= false;
		kPcBangReward.m_bAvatar					= false;
		kPcBangReward.m_bTitle					= false;
		return kPcBangReward;
	}

	return m_vecPcBangReward[iPcBangType];
}

int KGameSysVal::GetLimitsLevel()
{
	// 이벤트로 더 높은 값이 들어올 경우 이용
	return std::max( SiKGameEventManager()->GetMaxLevel(), m_iLimitsMaxLevel );
}

void KGameSysVal::SetDungeonClearPaymentItemID_LUA( int iItemID )
{
	m_iPaymentItemID = iItemID;
	START_LOG( cout, L"Registered items that cannot be awarded more than once for a dungeon clear." )
		<< BUILD_LOG( m_iPaymentItemID );
}

void KGameSysVal::SetProcessCommunication_Lua( IN bool bProcessCommunicationON )
{
	m_bProcessCommunicationON = bProcessCommunicationON;
	START_LOG( cout, L"Server management tool communication mode: " )
		<< BUILD_LOG( m_bProcessCommunicationON );
}


//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
void KGameSysVal::SetLanBugOutCheckOption_Lua( void )
{
	KLuaManager luaMgr( GetLuaState() );

	char temp;

	LUA_GET_VALUE( luaMgr,	"bLanBugOutCheck",				m_bLanBugOutCheck,				false	);
	LUA_GET_VALUE( luaMgr,	"byteLanBugOutCheckMaxCount",	temp,							0		);
	m_byteLanBugOutCheckMaxCount = temp;

	LUA_GET_VALUE( luaMgr,	"fLanBugOutCheckTime",			m_fLanBugOutCheckTime,			1.0f	);
	LUA_GET_VALUE( luaMgr,	"byteLanBugOutCheckCount",		temp,							10		);
	m_byteLanBugOutCheckCount = temp;

	LUA_GET_VALUE( luaMgr,	"fLanBugOutCheckTermMin",		m_fLanBugOutCheckTermMin,		1.0f	);
	LUA_GET_VALUE( luaMgr,	"fLanBugOutCheckTerm",			m_fLanBugOutCheckTerm,			0.0f	);
	LUA_GET_VALUE( luaMgr,	"fLanBugOutCheckRepeatTerm",	m_fLanBugOutCheckRepeatTerm,	0.5f	);

	START_LOG( cout, L"Wired-network lag prevention: argument information updated." )
		<< BUILD_LOG( m_bLanBugOutCheck )
		<< BUILD_LOG( m_byteLanBugOutCheckMaxCount )
		<< BUILD_LOG( m_fLanBugOutCheckTime )
		<< BUILD_LOG( m_byteLanBugOutCheckCount )
		<< BUILD_LOG( m_fLanBugOutCheckTermMin )
		<< BUILD_LOG( m_fLanBugOutCheckTerm )
		<< BUILD_LOG( m_fLanBugOutCheckRepeatTerm );
}

void KGameSysVal::SetLanBugOutVerifyOption_Lua( void )
{
	KLuaManager luaMgr( GetLuaState() );

	char temp;

	LUA_GET_VALUE( luaMgr,	"bLanBugOutVerify",				m_bLanBugOutVerify,				false	);
	LUA_GET_VALUE( luaMgr,	"byteLanBugOutVerifyMaxCount",	temp,							10		);
	m_byteLanBugOutVerifyMaxCount = temp;

	LUA_GET_VALUE( luaMgr,	"fLanBugOutVerifyTime",			m_fLanBugOutVerifyTime,			1.0f	);
	LUA_GET_VALUE( luaMgr,	"byteLanBugOutVerifyCount",		temp,							10		);
	m_byteLanBugOutVerifyCount = temp;

	LUA_GET_VALUE( luaMgr,	"fLanBugOutVerifyTermMin",		m_fLanBugOutVerifyTermMin,		1.0f	);
	LUA_GET_VALUE( luaMgr,	"fLanBugOutVerifyTerm",			m_fLanBugOutVerifyTerm,			0.0f	);
	LUA_GET_VALUE( luaMgr,	"fLanBugOutVerifyRepeatTerm",	m_fLanBugOutVerifyRepeatTerm,	0.5f	);

	START_LOG( cout, L"Wired-network lag prevention: argument information updated." )
		<< BUILD_LOG( m_bLanBugOutVerify )
		<< BUILD_LOG( m_byteLanBugOutVerifyMaxCount )
		<< BUILD_LOG( m_fLanBugOutVerifyTime )
		<< BUILD_LOG( m_byteLanBugOutVerifyCount )
		<< BUILD_LOG( m_fLanBugOutVerifyTermMin )
		<< BUILD_LOG( m_fLanBugOutVerifyTerm )
		<< BUILD_LOG( m_fLanBugOutVerifyRepeatTerm );
}
//}}


void KGameSysVal::ResetTimeEnchantEventInfo_Lua( void ) const
{
	const int iPfID = KBaseServer::GetKObj()->GetPfID();

	if( iPfID != PI_GLOBAL_SERVER )
	{
		START_LOG( cerr, L"This option is available only on the GlobalServer." )
			<< BUILD_LOG( iPfID )
			<< END_LOG;
		return;
	}

	KncSend( iPfID, KBaseServer::GetKObj()->GetUID(), PI_LOG_DB, 0, NULL, DBE_TIME_ENCHANT_EVENT_INFO_REQ, char() );
}

void KGameSysVal::ResetExchangeLimitInfo_Lua( void ) const
{
	const int iPfID = KBaseServer::GetKObj()->GetPfID();

	if( iPfID != PI_GLOBAL_SERVER )
	{
		START_LOG( cerr, L"This option is available only on the GlobalServer." )
			<< BUILD_LOG( iPfID )
			<< END_LOG;
		return;
	}

	KncSend( iPfID, KBaseServer::GetKObj()->GetUID(), PI_LOG_DB, 0, NULL, DBE_EXCHANGE_LIMIT_INFO_REQ, char() );
}

void KGameSysVal::AfterWorkStorageMessageDeleteTerm_Second_LUA( IN int iAwsCriterionNum, IN int iAwsUnderTerm, IN int iAwsAboveOrEqualTerm )
{
	m_iAwsCriterionNum		= iAwsCriterionNum;
	m_iAwsUnderTerm			= iAwsUnderTerm;
	m_iAwsAboveOrEqualTerm	= iAwsAboveOrEqualTerm;

	START_LOG( cout, L"AfterWorkStorage information configured." )
		<< BUILD_LOG( m_iAwsCriterionNum )
		<< BUILD_LOG( m_iAwsUnderTerm )
		<< BUILD_LOG( m_iAwsAboveOrEqualTerm )
		;
}
