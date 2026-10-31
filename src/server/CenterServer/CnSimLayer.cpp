#include "CnSimLayer.h"
#include <iomanip>  // std::setiosflags, std::resetiosflags
#include "ActorManager.h"
#include "DBLayer.h"
#include "CnServer.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"
#include "CnUser.h"

#include "DBLayer.h"
#include "NetError.h"
#include "CnRoomManager.h"
#include "KAutoPath.h"
#include "KDropTable.h"
#include "KPVPResultTable.h"
#include "ResultProcess.h"
#include "X2Data/XSLDungeonManager.h"
#include "X2Data/XSLItemManager.h"
#include "X2Data/XSLUnitManager.h"
#include "X2Data/XSLQuestManager.h"
#include "GameEvent/GameEventManager.h"
#include "GameSysVal/GameSysVal.h"
#include "AbuserLog/KAbuserLogManager.h"
//{{ 2009. 7. 9  최육사		헤니르 시공
#include "HenirResultTable.h"
//}}
//{{ 2008. 10. 20  최육사	속성NPC
#include "KAttribNpcTable.h"
//}}
//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	#include "GameEvent/GameEventScriptManager.h"
//}}
//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	#include "CnHackingCheckManager.h"
//}}
//{{ 2011. 01. 20	최육사	던전 랭크 개편
	#include "RoomMonsterManager.h"
//}}
//{{ 2011. 03. 30	최육사	서버 정보 수집
//}}
//{{ 2011. 07. 19	최육사	대전 개편
	#include "PvpMatchResultTable.h"
	#include "PvpMatch/PvpMatchManager.h"
//}}
//{{ 2011. 11. 4	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
//}}

//{{ 2012. 07. 30	최육사	배틀필드 시스템
	#include "X2Data/XSLPartyManager.h"
//}}

//{{ 2012. 10. 15	박세훈	SMS 전화번호 통합 관리
//}}

//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	#include "../Common/AutoParty/AutoPartyScriptManager.h"
//}}
//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	#include "BadAttitudeTable.h"
//}
//{{ 2013. 04. 12	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
//}}

	#include "X2Data/XSLFieldBossData.h"

#include <lua_tinker.h>
using namespace lua_tinker;

#include <functional>
using namespace std::placeholders;

NiImplementRTTI( KCnSimLayer, KSimLayer );

KCnSimLayer::KCnSimLayer(void)
{
}

KCnSimLayer::~KCnSimLayer(void)
{
}

ImplToStringW( KCnSimLayer )
{
    return START_TOSTRING_PARENTW( KSimLayer );
}

//{{ 2009. 12. 15  최육사	서버관리
void KCnSimLayer::DumpToLogFile()
{
	KSimLayer::DumpToLogFile();
}
//}}

ImplOstmOperatorW2A( KCnSimLayer );

void KCnSimLayer::RegToLua()
{
    KSimLayer::RegToLua();

    // 061127. jseop. 룸매니저.
    //SiKRoomManager()->RegToLua();
	// 081017. hoons. 룸매니저.
	GetKCnRoomManager()->RegToLua();

	lua_tinker::class_add<KCnSimLayer>( g_pLua, "KCnSimLayer" );
	lua_tinker::class_inh<KCnSimLayer, KSimLayer>( g_pLua );
	lua_tinker::class_def<KCnSimLayer>( g_pLua, "dump", &KCnSimLayer::Dump );
	//{{ 2009. 3. 24  최육사	CnUser끊긴상황 재현 테스트 코드
	//lua_tinker::class_def<KCnSimLayer>( g_pLua, "DieCnUser",		&KCnSimLayer::DisconnectAllCnUser_LUA );
	//}}

	lua_tinker::decl( g_pLua, "SimLayer", this );
}

//////////////////////////////////////////////////////////////////////////
void KCnSimLayer::DisconnectAllCnUser_LUA()
{
	START_LOG( cout, L"모든 CnUser 접속 끊기!" );

	KActorManager* pkActorManager = KActorManager::GetKObj();

	pkActorManager->ForEach( std::bind( &KActor::ReserveDestroy, _1 ) );
}
//////////////////////////////////////////////////////////////////////////

void KCnSimLayer::Init()
{
    KSimLayer::Init();

	KAutoPath		kAutoPath;
	std::string		strFile;

	//추후 파일로딩 실패시 서버다운을 시켜야한다.
    lua_State* L = g_pLua;

	{
		LoadingTimer lt(L"Enum.lua" );

		strFile = "Enum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( L, strFile.c_str() ) )
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
		LoadingTimer lt(L"DungeonEnum.lua" );

		strFile = "DungeonEnum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( L, strFile.c_str() ) )
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

	//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	{
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		SiKGameEventManager()->SetServerType( KGameEventManager::ST_CENTER );
		KGameEventScriptManager::RegScriptName( "EventData.lua" );
		//}}
		KGameEventScriptManager::RegScriptName( "EventMonsterData.lua" );
		OPEN_SCRIPT_FILE( KGameEventScriptManager );
	}
	//}}

	{
		LoadingTimer lt(L"DLG_Map_Enum.lua" );

		strFile = "DLG_Map_Enum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( L, strFile.c_str() ) )
		{
			START_LOG( cerr, L"DLG_Map_Enum 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"DLG_Map_Enum.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"DLG_Map_Enum 정보 로드 성공.!" );
		}
	}

	{
		CXSLItemManager::RegScriptName( "Item.lua" );


		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		CXSLItemManager::RegScriptName( "WeddingItemData.lua" );
		//}
		OPEN_SCRIPT_FILE( CXSLItemManager );
	}

	{
		KDropTable::RegScriptName( "DropTable.lua" );
		KDropTable::RegScriptName( "PvPDropTable.lua" );
		KDropTable::RegScriptName( "StaticDropTable.lua" );
		KDropTable::RegScriptName( "DropData.lua" );
		//{{ 2009. 12. 18  최육사	던전경험치개편
		KDropTable::RegScriptName( "NpcExpTable.lua" );
		//}}
		//{{ 2011. 05. 04  김민성	던전 클리어시 아이템 지급 조건 추가
		KDropTable::RegScriptName( "PaymentItemTable.lua" );
		//}}
		//{{ 2011. 08. 23	최육사	헤니르 시공 개편
		KDropTable::RegScriptName( "HenirSpecialNpcDrop.lua" );
		//}}
		//{{ 2011. 11. 23	최육사	배틀필드
		//KDropTable::RegScriptName( "BattleFieldNpcDropTable.lua" );
		//KDropTable::RegScriptName( "BattleFieldStaticDropTable.lua" );
		//}}

		OPEN_SCRIPT_FILE( KDropTable );
	}

	{
		CXSLDungeonManager::RegScriptName( "DungeonData.lua" );
		//{{ 2010. 03. 24  최육사	비밀던전 헬모드
		CXSLDungeonManager::RegScriptName( "SecretDungeonData.lua" );
		//}}

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

	//{{ 2011. 07. 19	최육사	대전 개편
	{
		KPvpMatchResultTable::RegScriptName( "PvpMatchResultTable.lua" );
		OPEN_SCRIPT_FILE( KPvpMatchResultTable );
	}

	{
		KPvpMatchManager::RegScriptName( "PvpMatchData.lua" );
		KPvpMatchManager::RegScriptName( "PvpNpcData.lua" );

		OPEN_SCRIPT_FILE( KPvpMatchManager );
	}
	//}}

	{
		//{{ 2011. 02. 23	최육사	던전 랭크 개편
		KResultProcess::RegScriptName( "ResultData_new.lua" );
		//}}
		OPEN_SCRIPT_FILE( KResultProcess );
	}

	{
		LoadingTimer lt( L"NPCTemplet.lua" );

		strFile = "NPCTemplet.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLUnitManager()->OpenNPCScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"NPC 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"NPCTemplet.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"NPC 정보 로드 성공.!" );
		}
	}

	{
		LoadingTimer lt( L"NPCStat.lua" );

		strFile = "NPCStat.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLUnitManager()->OpenNPCStatScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"NPC Stat 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"NPCStat.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"NPC Stat 정보 로드 성공.!" );
		}
	}

	{
		KGameSysVal::RegScriptName( "GameSysValTable.lua" );
		OPEN_SCRIPT_FILE( KGameSysVal );
	}

	//08.03.10. hoons : 던전에서 퀘스트 전용아이템이 드롭되게 되어 추가됨.
	{
		CXSLQuestManager::RegScriptName( "FieldQuest.lua" );
		CXSLQuestManager::RegScriptName( "FieldSubQuest.lua" );
		OPEN_SCRIPT_FILE( CXSLQuestManager );
	}

	{
		KHenirResultTable::RegScriptName( "HenirData.lua" );
		KHenirResultTable::RegScriptName( "HenirResultTable.lua" );
		//{{ 2011. 08. 22	최육사	헤니르 시공 개편
		KHenirResultTable::RegScriptName( "HenirChallangeReward.lua" );
		//}}
		OPEN_SCRIPT_FILE( KHenirResultTable );
	}

	//{{ 2008. 7. 16  최육사	속성NPC
	{
		KAttribNpcTable::RegScriptName( "AttribNpcTable.lua" );
		OPEN_SCRIPT_FILE( KAttribNpcTable );
	}
	//}}

	//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	{
		KCnHackingCheckManager::RegScriptName( "CnHackingCheckData.lua" );
		OPEN_SCRIPT_FILE( KCnHackingCheckManager );
	}
	//}}

	{
		KAbuserLogManager::RegScriptName( "AbuserList.lua" );
		OPEN_SCRIPT_FILE( KAbuserLogManager );
	}

	//{{ 2011. 03. 30	최육사	서버 정보 수집
	//}}

	//{{ 2011. 11. 4	최육사	배틀필드 시스템
	{
		CXSLBattleFieldManager::RegScriptName( "BattleFieldServerData.lua" );
		CXSLBattleFieldManager::RegScriptName( "BattleFieldData.lua" );
		//{{ 2013. 02. 15   필드 중간 보스 - 김민성
		CXSLBattleFieldManager::RegScriptName( "FieldBonusDrop.lua" );
		//}

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
	//}}

	//{{ 2012. 07. 30	최육사	배틀필드 시스템
	{
		CXSLPartyManager::RegScriptName( "PartyServerData.lua" );
		OPEN_SCRIPT_FILE( CXSLPartyManager );
	}
	//}}

	//{{ 2012. 10. 15	박세훈	SMS 전화번호 통합 관리
	//}}

	//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	{
		KAutoPartyScriptManager::RegScriptName( "AutoPartyData.lua" );
		OPEN_SCRIPT_FILE( KAutoPartyScriptManager );
	}
	//}}
	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	{
		KBadAttitudeTable::RegScriptName( "BadAttitudeTable.lua" );
		OPEN_SCRIPT_FILE( KBadAttitudeTable );
	}
	//}
	//{{ 2013. 04. 12	최육사	어둠의 문 개편
	{
		CXSLDefenceDungeonManager::RegScriptName( "DefenceDungeonData.lua" );
		OPEN_SCRIPT_FILE( CXSLDefenceDungeonManager );
	}
	//}}

	{
		CXSLFieldBossData::RegScriptName( "FieldBossData.lua" );
		OPEN_SCRIPT_FILE( CXSLFieldBossData );
	}
}


void KCnSimLayer::Tick()
{
    KSimLayer::Tick();

	//{{ 2011. 03. 30	최육사	서버 정보 수집
	//}}
}

void KCnSimLayer::ShutDown()
{
	CXSLItemManager::ReleaseInstance();
	CXSLDungeonManager::ReleaseInstance();
	SiKResultProcess()->ReleaseInstance();
	KDropTable::ReleaseInstance();
	//{{ 2011. 07. 19	최육사	대전 개편
	KPvpMatchResultTable::ReleaseInstance();
	KPvpMatchManager::ReleaseInstance();
	//}}
	CXSLUnitManager::ReleaseInstance();
	KGameEventManager::ReleaseInstance();
	KGameSysVal::ReleaseInstance();
	KAbuserLogManager::ReleaseInstance();
	CXSLQuestManager::ReleaseInstance();
	//{{ 2009. 7. 9  최육사		헤니르 시공
	KHenirResultTable::ReleaseInstance();
	//}}
	//{{ 2008. 10. 20  최육사	속성NPC
	KAttribNpcTable::ReleaseInstance();
	//}}
	//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	KGameEventScriptManager::ReleaseInstance();
	//}}
	//{{ 2012. 07. 30	최육사	배틀필드 시스템
	CXSLPartyManager::ReleaseInstance();
	//}}
	//{{ 2012. 11. 22	박세훈	SMS 전화번호 통합 관리
	//}}

	//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	KAutoPartyScriptManager::ReleaseInstance();
	//}}
	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	KBadAttitudeTable::ReleaseInstance();
	//}
	//{{ 2013. 04. 12	최육사	어둠의 문 개편
	CXSLDefenceDungeonManager::ReleaseInstance();
	//}}

	CXSLFieldBossData::ReleaseInstance();

    KSimLayer::ShutDown();
}



//{{ 임홍락 [2012.05.16] 서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
//}}
