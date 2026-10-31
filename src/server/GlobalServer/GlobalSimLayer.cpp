#include "GlobalSimLayer.h"
#include <iomanip>  // std::setiosflags, std::resetiosflags
#include "ActorManager.h"
#include "DBLayer.h"
#include "GlobalServer.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

#include "GlobalUser.h"

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	#include "WorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 06. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
	#include "MatchMakingManager.h"
//}}
//{{ 2012. 02. 09	최육사	배틀필드 시스템
	#include "X2Data/XSLDungeonManager.h"
//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	#include "../Common/AutoParty/AutoPartyScriptManager.h"
//}}
//}}

//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	#include "X2Data/XSLQuestManager.h"
//}}

//{{ 2012. 10. 15	박세훈	SMS 전화번호 통합 관리
//}}

//{{ 2013. 05. 14	최육사	제재 리스트 통합
	#include "BlockListManager.h"
//}}
	#include "GameSysVal/GameSysVal.h"

//{{ 2012. 09. 03	임홍락 글로벌 미션 매니저
#include "GlobalMissionManager.h"
//}} 2012. 09. 03	임홍락 글로벌 미션 매니저

	#include "X2Data/XSLFieldBossData.h"
	#include "X2Data/XSLBattleFieldManager.h"

#include "DBLayer.h"
#include "NetError.h"

#include <lua_tinker.h>
using namespace lua_tinker;

#include <functional>
using namespace std::placeholders;

NiImplementRTTI( KGlobalSimLayer, KSimLayer );

KGlobalSimLayer::KGlobalSimLayer(void)
{
}

KGlobalSimLayer::~KGlobalSimLayer(void)
{
}

ImplToStringW( KGlobalSimLayer )
{
    return START_TOSTRING_PARENTW( KSimLayer );
}

ImplOstmOperatorW2A( KGlobalSimLayer );

void KGlobalSimLayer::RegToLua()
{
    KSimLayer::RegToLua();

    //class_<KLoginSimLayer>( "KLoginSimLayer" )
    //    .inh<KSimLayer>()
    //    .def( "dump", KLoginSimLayer::Dump );

    //decl( "SimLayer", this );
	lua_tinker::class_add<KGlobalSimLayer>( g_pLua, "KGlobalSimLayer" );
	lua_tinker::class_inh<KGlobalSimLayer, KSimLayer>( g_pLua );
	lua_tinker::class_def<KGlobalSimLayer>( g_pLua, "dump", &KGlobalSimLayer::Dump );

	lua_tinker::decl( g_pLua, "SimLayer", this );
}

//{{ 2012. 10. 31	최육사	배틀필드 시스템
bool KGlobalSimLayer::GetGSUserInfoByUnitUID( IN const UidType iUnitUID, OUT KGlobalUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByUnitUID( iUnitUID, kInfo );
}

bool KGlobalSimLayer::GetGSUserInfoByUnitNickName( IN const UidType iServerGroupID, IN const std::wstring& wstrUnitNickName, OUT KGlobalUserList::KGSUserInfo& kInfo )
{
	return m_kUserList.GetGSUserInfoByUnitNickName( iServerGroupID, wstrUnitNickName, kInfo );
}

//{{ 2013. 05. 29	최육사	제재 리스트 통합
bool KGlobalSimLayer::GetGSUserInfoByUserUID( IN const UidType iUserUID, OUT KGlobalUserList::KGSUserInfo& kInfo )
{
	return m_kUserList.GetGSUserInfoByUserUID( iUserUID, kInfo );
}
//}}

void KGlobalSimLayer::UnRegAllUserOfThisGS( IN const UidType iGSUID )
{
    m_kUserList.UnRegAllUserOfThisGS( iGSUID );
}

int KGlobalSimLayer::RegUnit( IN const KRegUnitInfo& kUnitInfo )
{
    return m_kUserList.RegUnit( kUnitInfo );
}

void KGlobalSimLayer::UnRegUnit( IN const UidType iUnitUID )
{
    m_kUserList.UnRegUnit( iUnitUID );
}
//}}

void KGlobalSimLayer::Init()
{
    KSimLayer::Init();

	KAutoPath kAutoPath;
	std::string		strFile;

	{
		LoadingTimer lt( L"Enum.lua" );

		//추후 ↓에서 이루어 지는 데이터 로딩이 실패할경우 서버를 종료 시켜야한다.
		strFile = "Enum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != luaL_dofile( g_pLua, strFile.c_str() ) )
		{
			START_LOG( cerr, L"Enum 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );

			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"Enum.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Enum 정보 로드 성공.!" );
		}
	}

	{
		LoadingTimer lt( L"DungeonEnum.lua" );

		//추후 ↓에서 이루어 지는 데이터 로딩이 실패할경우 서버를 종료 시켜야한다.
		strFile = "DungeonEnum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != luaL_dofile( g_pLua, strFile.c_str() ) )
		{
			START_LOG( cerr, L"DungeonEnum 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );

			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"DungeonEnum.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"DungeonEnum 정보 로드 성공.!" );
		}
	}

	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	{
		KWorldMissionManager::RegScriptName( "WorldMissionConfig.lua" );
		OPEN_SCRIPT_FILE( KWorldMissionManager );
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 07. 18	최육사	대전 개편
	{
		KPvpMatchManager::RegScriptName( "PvpMatchData.lua" );
		KPvpMatchManager::RegScriptName( "PvpNpcData.lua" );
		OPEN_SCRIPT_FILE( KPvpMatchManager );
	}
	//}}
	//{{ 2012. 02. 09	최육사	배틀필드 시스템
	{
		CXSLDungeonManager::RegScriptName( "DungeonData.lua" );

		if( SiCXSLDungeonManager()->OpenScriptFile_AllDungeonScriptLoad( g_pLua ) == false )
			START_LOG( cerr, L"Dungeon Manager 정보 로드 실패.!" );
		else
			START_LOG( cout, L"Dungeon Manager 정보 로드 성공.!" );

		//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
		if( SiCXSLDungeonManager()->IsLuaPasingSuccess() == false )
		{
			KBaseServer::GetKObj()->AddFailScriptFileName( SiCXSLDungeonManager()->GetParsingLuaFileName().c_str() );
		}
		//}}
	}
	{
		KAutoPartyScriptManager::RegScriptName( "AutoPartyData.lua" );
		OPEN_SCRIPT_FILE( KAutoPartyScriptManager );
	}
	//}}

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	{
		CXSLQuestManager::RegScriptName( "FieldQuest.lua" );
		OPEN_SCRIPT_FILE( CXSLQuestManager );
	}
	//}}

	//{{ 2012. 10. 15	박세훈	SMS 전화번호 통합 관리
	//}}

	{
		KGameSysVal::RegScriptName( "GameSysValTable.lua" );
		OPEN_SCRIPT_FILE( KGameSysVal );
	}

	{
		CXSLFieldBossData::RegScriptName( "FieldBossData.lua" );
		OPEN_SCRIPT_FILE( CXSLFieldBossData );

		CXSLBattleFieldManager::RegScriptName( "BattleFieldData.lua" );
		if( SiCXSLBattleFieldManager()->OpenScriptFile_AllBattleFieldScriptLoad( g_pLua ) == false )
			START_LOG( cerr, L"Battle Field Manager 정보 로드 실패.!" );
		else
			START_LOG( cout, L"Battle Field Manager 정보 로드 성공.!" );

		//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
		if( SiCXSLBattleFieldManager()->IsLuaPasingSuccess() == false )
		{
			KBaseServer::GetKObj()->AddFailScriptFileName( SiCXSLBattleFieldManager()->GetParsingLuaFileName().c_str() );
		}
		//}}
	}
}

void KGlobalSimLayer::Tick()
{
    KSimLayer::Tick();

	//{{ 2010. 10. 12	최육사	서버 모니터링
//#ifdef SERV_MORNITORING
//	if( GetKGlobalServer()->IsConnectionMornitoringEnable() == true )
//	{
//		SiKMornitoringManager()->Tick_LoginSImLayer();
//	}
//#endif SERV_MORNITORING
	//}}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	if( SiKWorldMissionManager()->GetWorldMissionState() == true )
	{
		SiKWorldMissionManager()->Tick();
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2013. 05. 14	최육사	제재 리스트 통합
	SiKBlockListManager()->Tick();
	//}}
	//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
	SiKGlobalMissionManager()->Tick();
	//}} 2012. 09. 03	임홍락	글로벌 미션 매니저
}

void KGlobalSimLayer::ShutDown()
{
    KSimLayer::ShutDown();

	KWorldMissionManager::ReleaseInstance();
	//{{ 2011. 07. 18	최육사	대전 개편
	KPvpMatchManager::ReleaseInstance();
	//}}
	//{{ 2012. 02. 09	최육사	배틀필드 시스템
	KAutoPartyScriptManager::ReleaseInstance();
	//}}
	//{{ 2012. 11. 22	박세훈	SMS 전화번호 통합 관리
	//}}

	KGameSysVal::ReleaseInstance();

	KGlobalMissionManager::ReleaseInstance();

	CXSLFieldBossData::ReleaseInstance();
	CXSLBattleFieldManager::ReleaseInstance();
}


//{{ 임홍락 [2012.05.16] 서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
//}}
