#include "GSSimLayer.h"
#include <iomanip>  // std::setiosflags, std::resetiosflags
#include "DBLayer.h"
#include "GameServer.h"
#include "CnProxy.h"
#include "LoginProxy.h"
#include "DBLayer.h"
#include "ProxyManager.h"
#include "KAutoPath.h"
#include "StatTable.h"

#include "BillManager.h"

#include "../Common/OnlyGlobal/AuthAndBilling/DEBBillingManager.h"


#include "NexonBillingTCPManager.h"
#include "ActorManager.h"
#include "ExpTable.h"
#include "SpiritTable.h"
#include "TrainingCenterTable.h"
#include "X2Data/XSLQuestManager.h"
#include "X2Data/XSLSkillTree.h"
#include "X2Data/XSLCheckSumManager.h"
#include "X2Data/XSLRandomItemManager.h"
#include "X2Data/XSLManufactureItemManager.h"
#include "X2Data/XSLResolveItemManager.h"
#include "X2Data/XSLEnchantItemManager.h"
#include "X2Data/XSLSocketItem.h"
#include "X2Data/XSLCashItemManager.h"
#include "X2Data/XSLStringFilter.h"
#include "RewardTable.h"
#include "RecommendUserTable.h"
#include "GameEvent/GameEventManager.h"
#include "GameSysVal/GameSysVal.h"
#include "AbuserLog/KAbuserLogManager.h"
#include "X2Data/XSLAttribEnchantItem.h"
#include "X2Data/XSLTitleManager.h"
//{{ 2008. 11. 24  최육사	랭킹 매니저
#include "GSRankingManager.h"
//}}
//{{ 2008. 11. 24  최육사	방리스트
#include "RoomListManager.h"
//}}
//{{ 2009. 7. 16  최육사	채널매니저
#include "ChannelManager.h"
//}}
//{{ 2009. 4. 23  최육사	대전유저리스트
#include "PVPLobbyManager.h"
//}}

#include "FieldManager.h"
//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
//}}
//{{ 2008. 12. 28  최육사
#include "TutorialManager.h"
//{{ 2009. 10. 27  최육사	길드레벨
	#include "X2Data/XSLGuildManager.h"
//}}
//{{ 2010. 8. 6	최육사	펫 시스템
	#include "X2Data/XSLPetManager.h"
//}}
//{{ 2010. 02. 16  최육사	해킹툴 리스트
	#include "HackingProtectionManager.h"
//}}
//{{ 2010. 05. 27  최육사	클라이언트 해킹 방지
//}}
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	#include "GameEvent/GameEventScriptManager.h"
//}}
//{{ 2011. 07. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
//}}
//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
#include "GlobalProxy.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2012. 04. 02	김민성		넥슨 auth soap
//}}

//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	#include "CompanyIPTable.h"
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
	#include "ClassChangeTable.h"
//}}
//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	#include "PacketMornitoring/PacketMornitoring.h"
//}}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//{{ 2012. 04. 21	박세훈	어둠의 문 개편
	#include "X2Data/XSLBuffManager.h"
//}}

//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
//}}

//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	#include "EventDelaySystemManager.h"
//}}

//{{ 2012. 07. 30	최육사	배틀필드 시스템
	#include "X2Data/XSLPartyManager.h"
//}}

//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	#include "SMSPhoneNumberManager.h"
//}}
//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
//}}
//{{ 2011. 11. 4	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
//}}
//{{ 2013. 04. 12	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
//}}

//{{ 2013. 05. 20	최육사	아이템 개편
	#include "X2Data/XSLCharacterStatTable.h"
//}}

//{{ 2013. 1. 26	박세훈	빙고 이벤트
//}}


//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}

//-- 2010. 12. 13. 지헌 : XTRAP 서버 - CXTrapCSM 포함
#include "XTrapCSM.h"

	#include "X2Data/XSLRidingPetManager.h"

#include "X2Data/XSLEDInventoryExpand.h"

	#include "X2Data/XSLFieldBossData.h"


NiImplementRTTI( KGSSimLayer, KSimLayer );

KGSSimLayer::KGSSimLayer(void)
{
}

KGSSimLayer::~KGSSimLayer(void)
{
}

ImplToStringW( KGSSimLayer )
{
	stm_ << L"--------------------------------- SimLayer Dump ---------------------------------" << std::endl;
    START_TOSTRING_PARENTW( KSimLayer );
	stm_ << L"---------------------------------------------------------------------------------" << std::endl;
    SiKProxyManager()->ToString( stm_ );
	stm_ << L"---------------------------------------------------------------------------------" << std::endl;
    KActorManager::GetKObj()->ToString( stm_ );
	stm_ << L"---------------------------------------------------------------------------------" << std::endl;
    KGSUser::DumpMaxLoginTime();

	//stm_ << L"Repeat Event" << std::endl; -- 지금은 볼일이 없다
	//std::map<u_short, int>::const_iterator mit;
	//for( mit = m_mapRepeatEvent.begin(); mit != m_mapRepeatEvent.end(); ++mit )
	//{
	//	stm_	<< TOSTRINGW( KEvent::GetIDStr( mit->first ) )
	//			<< TOSTRINGW( mit->second )
	//			<< std::endl;
	//}

    return stm_;
}

//{{ 2009. 12. 15  최육사	서버관리
void KGSSimLayer::DumpToLogFile()
{
	std::wstringstream wstrDump;
	ToString( wstrDump );
	dbg::cout << wstrDump.str() << dbg::endl;
}
//}}

#include <lua_tinker.h>
//using namespace lua_tinker;

void KGSSimLayer::RegToLua()
{
    KSimLayer::RegToLua();

	// 081021. hoons.
	GetKFieldManager()->RegToLua();

	lua_tinker::class_add<KGSSimLayer>( g_pLua, "KGSSimLayer" );
	lua_tinker::class_inh<KGSSimLayer, KSimLayer>( g_pLua );
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "dump",				&KGSSimLayer::Dump );
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "SendNotMSG_GRP",	&KGSSimLayer::SendNotMSG_GRP );
	//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "SendNotMSG_ALL",	&KGSSimLayer::SendNotMSG_ALL );
	//}}
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "AddIP",			&KGSSimLayer::AddIP );
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "TickDump",			&KGSSimLayer::TickDump_LUA );
	//}}
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "ReloadXTRAP_MAP",	&KGSSimLayer::ReloadXTRAP_MAP );
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "AddCommonFlag_AllGS",		&KGSSimLayer::AddCommonFlag_AllGS_LUA );
	lua_tinker::class_def<KGSSimLayer>( g_pLua, "DeleteCommonFlag_AllGS",	&KGSSimLayer::DeleteCommonFlag_AllGS_LUA );


    lua_tinker::class_def<KGSSimLayer>( g_pLua, "SetServerGroupNum", &KGSSimLayer::SetServerGroupNum );

	lua_tinker::decl( g_pLua, "SimLayer", this );


	SiKDEBBillingManager()->RegToLua();


	//{{ 2012. 04. 02	김민성		넥슨 auth soap
	//}}
}

KActorProxy* CreateProxy( int iProxyType )
{
	if( iProxyType < 0 || iProxyType >= KProxyManager::PT_NUM )
	{
		START_LOG( cerr, L"프록시 타입이 이상함." )
			<< BUILD_LOG( iProxyType )
			<< END_LOG;

		return NULL;
	}

	KActorProxy* pkProxy;
	switch( iProxyType )
	{
	case KProxyManager::PT_CENTER:
		pkProxy = new KCnProxy;
		pkProxy->SetSendQueueSize( 512 * 1024 );
		return pkProxy;
	case KProxyManager::PT_LOGIN:
		pkProxy = new KLoginProxy;
		pkProxy->SetSendQueueSize( 512 * 1024 );
		return pkProxy;
		//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	case KProxyManager::PT_GLOBAL:
		pkProxy = new KGlobalProxy;
		pkProxy->SetSendQueueSize( 512 * 1024 );
		return pkProxy;
		//}} 2011. 04. 13  김민성  글로벌 서버 추가
	default:
		START_LOG( cerr, L"프록시 타입이 이상함." )
			<< BUILD_LOG( iProxyType )
			<< END_LOG;
		return NULL;
	}
}

void KGSSimLayer::Init()
{
	KAutoPath kAutoPath;
	std::string	strFile;
	bool bRVal = true;

	{
		LoadingTimer lt( L"Enum.lua" );

		//추후 ↓에서 이루어 지는 데이터 로딩이 실패할경우 서버를 종료 시켜야한다.
		strFile = "Enum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
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
		if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
		{
			START_LOG( cerr, L"DungeonEnum 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"DungeonEnum.lua" );
		}
		else
		{
			START_LOG( cout, L"DungeonEnum 정보 로드 성공.!" );
		}
	}

	//{{ 2009. 12. 19  최육사	서버군 확장
	{
		LoadingTimer lt(L"CashItemData.lua");

		strFile = "CashItemData.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLCashItemManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Cash Item Data 정보 로드 실패.!" )
			<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"CashItemData.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Cash Item Data 정보 로드 성공.!" );
		}
	}
	//}}


	m_vecpThreadMgr.push_back( SiKDEBBillingManager()->GetInstance() );


    KSimLayer::Init();

    SiKProxyManager()->SetProxyBuilder( CreateProxy );

	{
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		SiKGameEventManager()->SetServerType( KGameEventManager::ST_GAME );

		KGameEventScriptManager::RegScriptName( "EventData.lua" );
		OPEN_SCRIPT_FILE( KGameEventScriptManager );
		//}}
	}

	{
		LoadingTimer lt( L"DLG_Map_Enum.lua" );

		strFile = "DLG_Map_Enum.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
		{
			START_LOG( cerr, L"Map Data Enum 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"DLG_Map_Enum.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Map Data Enum 정보 로드 성공.!" );
		}
	}

	{
		LoadingTimer lt(L"UnitTemplet.lua");

		strFile = "UnitTemplet.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLUnitManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Unit 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"UnitTemplet.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Unit 정보 로드 성공.!" );
		}
	}

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
		CXSLItemManager::RegScriptName( "Item.lua" );


		CXSLItemManager::RegScriptName( "ItemExchangeTable.lua" );
		//{{ 2010. 9. 1	최육사	기간 초기화 아이템
		CXSLItemManager::RegScriptName( "ExpandPeriodItemData.lua" );
		//}}
		//{{ 2010. 11. 29	최육사	기간제템 삭제
		//}}
		//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
		CXSLItemManager::RegScriptName( "ShopItem.lua" );
		//}}
		//{{ 2012. 04. 04	최육사	배틀필드 시스템
		CXSLItemManager::RegScriptName( "CoolTimeGroupItem.lua" );
		//}}
		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		CXSLItemManager::RegScriptName( "WeddingItemData.lua" );
		//}
		//{{ 2013. 05. 21	최육사	아이템 개편
		CXSLItemManager::RegScriptName( "ItemConvertTable.lua" );
		//}}
		CXSLItemManager::RegScriptName( "KeepItem.lua" );


		OPEN_SCRIPT_FILE( CXSLItemManager );
	}

	{
		CXSLRandomItemManager::RegScriptName( "RandomItemTable.lua" );
		CXSLRandomItemManager::RegScriptName( "RandomItemData.lua" );
		CXSLRandomItemManager::RegScriptName( "RandomItemMapping.lua" );
		OPEN_SCRIPT_FILE( CXSLRandomItemManager );
	}

	{
		CXSLManufactureItemManager::RegScriptName( "ManufactureItem.lua" );
		CXSLManufactureItemManager::RegScriptName( "ManufactureResultTable.lua" );
		OPEN_SCRIPT_FILE( CXSLManufactureItemManager );
	}

	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}

	{
		CXSLResolveItemManager::RegScriptName( "ResolveTable.lua" );
		OPEN_SCRIPT_FILE( CXSLResolveItemManager );
	}

	{
		CXSLEnchantItemManager::RegScriptName( "EnchantTable.lua" );
		OPEN_SCRIPT_FILE( CXSLEnchantItemManager );
	}

	{
		CXSLSocketItem::RegScriptName( "OptionItemData.lua" );
		CXSLSocketItem::RegScriptName( "SocketItemTable.lua" );
		CXSLSocketItem::RegScriptName( "SetItem.lua" );
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		CXSLSocketItem::RegScriptName( "SocketGroupDataForCashAvatar.lua" );
		//}
		//{{ 2013. 05. 15	최육사	아이템 개편
		CXSLSocketItem::RegScriptName( "ItemRandomSocket.lua" );
		CXSLSocketItem::RegScriptName( "MiracleCubeTable.lua" );
		//}}
		OPEN_SCRIPT_FILE( CXSLSocketItem );
	}

	{
		LoadingTimer lt(L"PackageItemData.lua");

		strFile = "PackageItemData.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLCashItemManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Cash Package Item 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"PackageItemData.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Cash Package Item 정보 로드 성공.!" );
		}
	}

	{
		LoadingTimer lt(L"CashItemPrice.lua");

		strFile = "CashItemPrice.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiCXSLCashItemManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Cash Item Price 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );

			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"CashItemPrice.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Cash Item Price 정보 로드 성공.!" );
		}
	}

	{
		CXSLAttribEnchantItem::RegScriptName( "AttribEnchantTable.lua" );
		CXSLAttribEnchantItem::RegScriptName( "AttribEnchantRequire.lua" );
//{{ 2011. 01. 04  김민성	속성 부적
		CXSLAttribEnchantItem::RegScriptName( "AttribAttachTable.lua" );
//}}
		OPEN_SCRIPT_FILE( CXSLAttribEnchantItem );
	}

	{
		KRewardTable::RegScriptName( "RewardTable.lua" );
		OPEN_SCRIPT_FILE( KRewardTable );
	}

	{
		KRecommendUserTable::RegScriptName( "RecommendUserTable.lua" );
		OPEN_SCRIPT_FILE( KRecommendUserTable );
	}

	//{{ 2008. 11. 24  최육사	방리스트
	{
		LoadingTimer lt(L"RoomListData.lua");

		strFile = "RoomListData.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiKRoomListManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Room List 생성 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"RoomListData.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Room List 생성 성공.!" );
		}
	}
	//}}

	{
		KExpTable::RegScriptName( "ExpTable.lua" );
		OPEN_SCRIPT_FILE( KExpTable );
	}

	{
		LoadingTimer lt(L"StatTable.lua");

		strFile = "StatTable.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiKStatTable()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"STAT TABLE 정보 로드 실패.!" );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"StatTable.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"STAT TABLE 정보 로드 성공.!" );
		}
	}

	{
		CXSLQuestManager::RegScriptName( "FieldQuest.lua" );
		CXSLQuestManager::RegScriptName( "FieldSubQuest.lua" );
		//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
		CXSLQuestManager::RegScriptName( "AccountQuest.lua" );
		//}}
		//{{ 2013. 03. 21	 계정 퀘스트 - 요일별 활성화 기능 - 김민성
		CXSLQuestManager::RegScriptName( "AccountQuestDate.lua" );
		//}
		OPEN_SCRIPT_FILE( CXSLQuestManager );
	}

	CXSLSkillTree::RegScriptName( "NewSkillTempletVer2.lua" );
	CXSLSkillTree::RegScriptName( "SkillData.lua" );
	OPEN_SCRIPT_FILE( CXSLSkillTree );

	//{ -- 안쓴다
	//	LoadingTimer lt(L"NewVer.xml");

	//	strFile = "NewVer.xml";
	//	kAutoPath.GetPullPath( strFile );
	//	if ( SiCXSLCheckSumManager()->OpenScriptFile( strFile.c_str() ) == false )
	//		START_LOG( cerr, L"체크섬 정보 로드 실패.!" );
	//	else
	//		START_LOG( cout, L"체크섬 정보 로드 성공.!" );
	//}

	{
		LoadingTimer lt(L"TrainingCenterTemplet");

		strFile = "TrainingCenterTemplet.lua";
		kAutoPath.GetPullPath( strFile );
		if ( SiKTrainingCenterTable()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"훈련소 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"TrainingCenterTemplet" );
			//}}
		}
		else
		{
			START_LOG( cout, L"훈련소 로드 성공.!" );
		}
	}

	{
		LoadingTimer lt( L"SpiritTable.lua" );

		strFile = "SpiritTable.lua";
		kAutoPath.GetPullPath( strFile );
		if ( SiKSpiritTable()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"레벨별 근성도 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"SpiritTable.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"레벨별 근성도 로드 성공.!" );
		}
	}

	{
		CXSLTitleManager::RegScriptName( "TitleTable.lua" );
		CXSLTitleManager::RegScriptName( "TitleMission.lua" );
		CXSLTitleManager::RegScriptName( "SubTitleMission.lua" );
		OPEN_SCRIPT_FILE( CXSLTitleManager );
	}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	//}}

	{
		KGameSysVal::RegScriptName( "GameSysValTable.lua" );
		OPEN_SCRIPT_FILE( KGameSysVal );
	}

	{
		KAbuserLogManager::RegScriptName( "AbuserList.lua" );
		OPEN_SCRIPT_FILE( KAbuserLogManager );
	}

	{
		CXSLMapData::RegScriptName( "MapData.lua" );
		OPEN_SCRIPT_FILE( CXSLMapData );
	}

	{
		LoadingTimer lt( L"FieldData.lua" );

		strFile = "FieldData.lua";
		kAutoPath.GetPullPath( strFile );
		if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
		{
			START_LOG( cerr, L"Field Data 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );
			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"FieldData.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Field Data 정보 로드 성공.!" );
		}
	}

	//{{ 2008. 12. 10  최육사	로봇
	//}}

	//{{ 2009. 10. 27  최육사	길드레벨
	{
		CXSLGuildManager::RegScriptName( "GuildData.lua" );
		OPEN_SCRIPT_FILE( CXSLGuildManager );
	}
	//}}

	//{{ 2010. 8. 6	최육사	펫 시스템
	{
		CXSLPetManager::RegScriptName( "PetTemplet.lua" );
		CXSLPetManager::RegScriptName( "PetData.lua" );
		OPEN_SCRIPT_FILE( CXSLPetManager );
	}
	//}}

	//{{ 2011. 07. 23	최육사	대전 개편
	{
		KPvpMatchManager::RegScriptName( "PvpMatchData.lua" );

		KPvpMatchManager::RegScriptName( "PvpNpcData.lua" );

		OPEN_SCRIPT_FILE( KPvpMatchManager );
	}
	//}}

	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	{
		KGSContentManager::RegScriptName( "ContentData.lua" );
		OPEN_SCRIPT_FILE( KGSContentManager );
	}
	//}}

	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	{
		KCompanyIPTable::RegScriptName( "CompanyIPTable.lua" );
		OPEN_SCRIPT_FILE( KCompanyIPTable );
	}
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	{
		KClassChangeTable::RegScriptName( "ClassChangeTable.lua" );
		KClassChangeTable::RegScriptName( "ClassChangeSkill.lua" );
		KClassChangeTable::RegScriptName( "ClassChangeMemo.lua" );
		KClassChangeTable::RegScriptName( "ClassChangeItem.lua" );
		KClassChangeTable::RegScriptName( "ClassChangeQuest.lua" );
		OPEN_SCRIPT_FILE( KClassChangeTable );
	}
	//}}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	{
		CXSLStringFilter::RegScriptName( "StringFilter.lua" );
		OPEN_SCRIPT_FILE( CXSLStringFilter );
	}
	//}}

	//{{ 2012. 04. 21	박세훈	어둠의 문 개편
	{
		CXSLBuffManager::RegScriptName( "BuffTemplet.lua" );
		OPEN_SCRIPT_FILE( CXSLBuffManager );
	}
	//}}

	//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	{
		KSMSPhoneNumberManager::RegScriptName( "SMSPhoneNumberList.lua" );
		OPEN_SCRIPT_FILE( KSMSPhoneNumberManager );
	}
	//}}

	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}

	//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
	//}}

	//{{ 2012. 07. 30	최육사	배틀필드 시스템
	{
		CXSLPartyManager::RegScriptName( "PartyServerData.lua" );
		OPEN_SCRIPT_FILE( CXSLPartyManager );
	}
	//}}

	//{{ 2011. 11. 4	최육사	배틀필드 시스템
	{
		CXSLBattleFieldManager::RegScriptName( "BattleFieldServerData.lua" );
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
	//}}

	//{{ 2013. 04. 12	최육사	어둠의 문 개편
	{
		CXSLDefenceDungeonManager::RegScriptName( "DefenceDungeonData.lua" );
		OPEN_SCRIPT_FILE( CXSLDefenceDungeonManager );
	}
	//}}

	//{{ 2013. 1. 26	박세훈	빙고 이벤트
	//}}

	{
		CXSLRidingPetManager::RegScriptName( "RidingPetTemplet.lua" );
		CXSLRidingPetManager::RegScriptName( "RidingPetData.lua" );
		OPEN_SCRIPT_FILE( CXSLRidingPetManager );
	}

    {
        CXSLEDInventoryExpand::RegScriptName( "EDInventoryExpand.lua" );
        OPEN_SCRIPT_FILE( CXSLEDInventoryExpand );
    }

	//{{ 2013. 05. 20	최육사	아이템 개편
	{
		if( SiCXSLCharacterStatTable()->OpenScriptFile_CharacterStatTable() == false )
			START_LOG( cerr, L"Character Stat Table 정보 로드 실패.!" );
		else
			START_LOG( cout, L"Character Stat Table 정보 로드 성공.!" );
	}
	//}}

	//{{ 2010. 05. 27  최육사	클라이언트 해킹 방지
	//}}

	{
		CXSLFieldBossData::RegScriptName( "FieldBossData.lua" );
		OPEN_SCRIPT_FILE( CXSLFieldBossData );
	}

	//{{ 2010. 01. 28  최육사	통합파티
	SiKPartyListManager();
	//}}

	//{{ 2009. 4. 23  최육사	대전유저리스트
	SiKPVPLobbyManager();
	//}}

	//{{ 2009. 7. 16  최육사	채널매니저
	SiKChannelManager();
	//}}

	//{{ 2009. 11. 9  최육사	GS랭킹매니저
	SiKGSRankingManager();
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	SiKHackingProtectionManager();
	//}}


	//{{ 2011. 04. 04  김민성   LFH
	//}
	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}
	// 해외팀 주석 처리
    //m_iServerGroupNum = DOUBLE_SERVER_GROUP;
}

void KGSSimLayer::Tick()
{
    KSimLayer::Tick();

	SiKBillManager()->Tick();	//SiKGameForgeBillingManager와 착각하지 말기




	if( m_tXTrapReloadTimer.elapsed() > 120.0 )
	{
		m_tXTrapReloadTimer.restart();

		ReloadXTRAP_MAP();
	}

    SiKProxyManager()->Tick();

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}

	// 게임 이벤트
	SiKGameEventManager()->Tick();

	// 어뷰저 리스트 갱신
	SiKAbuserLogManager()->Tick();


	//{{ 2010. 01. 28  최육사	통합파티
	//}}

	//{{ 2009. 7. 16  최육사	채널매니저
	SiKChannelManager()->Tick();
	//}}

	//{{ 2009. 9. 1  최육사		IOCP종료사유
	if( m_tAbnormalReasonTimer.elapsed() > 3600.0 )
	{
		m_tAbnormalReasonTimer.restart();

		SendToDBAbnormalReason( false );
	}
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	SiKHackingProtectionManager()->Tick();
	//}}

	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	SiKPacketMornitoring()->Tick();
	//}}

	//{{ 2012. 07. 12	최육사		랜덤 큐브 오픈 딜레이 시스템
	SiKEventDelaySystemManager()->Tick();
	//}}
}

void KGSSimLayer::ShutDown()
{
    KSimLayer::ShutDown();

	SiKDEBBillingManager()->ReleaseInstance();

	KGameEventManager::ReleaseInstance();
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	KGameEventScriptManager::ReleaseInstance();
	//}}
	KTrainingCenterTable::ReleaseInstance();
	CXSLSkillTree::ReleaseInstance();
    CXSLQuestManager::ReleaseInstance();
	CXSLCheckSumManager::ReleaseInstance();
	KStatTable::ReleaseInstance();
	KExpTable::ReleaseInstance();
	KRoomListManager::ReleaseInstance();
	CXSLDungeonManager::ReleaseInstance();
	CXSLUnitManager::ReleaseInstance();
	CXSLRandomItemManager::ReleaseInstance();
	CXSLManufactureItemManager::ReleaseInstance();
	CXSLResolveItemManager::ReleaseInstance();
	CXSLEnchantItemManager::ReleaseInstance();
	CXSLSocketItem::ReleaseInstance();
	CXSLCashItemManager::ReleaseInstance();
	CXSLAttribEnchantItem::ReleaseInstance();
	KRewardTable::ReleaseInstance();
	KRecommendUserTable::ReleaseInstance();
	CXSLItemManager::ReleaseInstance();
	CXSLMapData::ReleaseInstance();
	KSpiritTable::ReleaseInstance();
	CXSLTitleManager::ReleaseInstance();
    //KCheckIP::ReleaseInstance();
    KProxyManager::ReleaseInstance();
	CXSLStringFilter::ReleaseInstance();
	KAbuserLogManager::ReleaseInstance();
	KGameSysVal::ReleaseInstance();
	//{{ 2008. 11. 24  최육사	랭킹 매니저
	KGSRankingManager::ReleaseInstance();
	//}}
	//{{ 2010. 01. 28  최육사	통합파티
	KPartyListManager::ReleaseInstance();
	//}}
	//{{ 2008. 12. 28  최육사	객체 해제
	KTutorialManager::ReleaseInstance();
	//KRobotTestManager::ReleaseInstance();
	//}}
	//{{ 2009. 4. 23  최육사	대전유저리스트
	KPVPLobbyManager::ReleaseInstance();
	//}}
	//{{ 2009. 7. 16  최육사	채널매니저
	KChannelManager::ReleaseInstance();
	//}}
	//{{ 2009. 10. 27  최육사	길드레벨
	CXSLGuildManager::ReleaseInstance();
	//}}
	//{{ 2010. 8. 6	최육사	펫 시스템
	CXSLPetManager::ReleaseInstance();
	//}}
	//{{ 2011. 07. 23	최육사	대전 개편
	KPvpMatchManager::ReleaseInstance();
	//}}
	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	KHackingProtectionManager::ReleaseInstance();
	//}}

	//{{ 2012. 04. 02	김민성		넥슨 auth soap
	//}}
	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	KCompanyIPTable::ReleaseInstance();
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	KClassChangeTable::ReleaseInstance();
	//}}
	//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	KSMSPhoneNumberManager::ReleaseInstance();
	//}}
	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}
	//{{ 2012. 03. 04	최육사	배틀필드 시스템
	CXSLBuffManager::ReleaseInstance();
	//}}
	//{{ 2012. 07. 30	최육사	배틀필드 시스템
	CXSLPartyManager::ReleaseInstance();
	//}}
	//{{ 2011. 11. 4	최육사	배틀필드 시스템
	CXSLBattleFieldManager::ReleaseInstance();
	//}}
	//{{ 2013. 04. 12	최육사	어둠의 문 개편
	CXSLDefenceDungeonManager::ReleaseInstance();
	//}}
	//{{ 2012. 06. 05	박세훈	매일매일 선물 상자
	//}}
	//{{ 2013. 1. 26	박세훈	빙고 이벤트
	//}}

	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}
	CXSLRidingPetManager::ReleaseInstance();

	CXSLFieldBossData::ReleaseInstance();
}

//{{ 임홍락 [2012.05.16] 서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
//}}

void KGSSimLayer::SendNotMSG( IN char cNotifyType, IN const std::wstring& wstrNotMSG, IN int iCount )
{
	KEGS_NOTIFY_MSG_NOT	kPacket;
	kPacket.m_cNotifyType = cNotifyType;
	kPacket.m_Count = static_cast<char>(iCount);
	kPacket.m_wstrMSG = wstrNotMSG;

	KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_CN_SERVER, 0, NULL, ECN_NOTIFY_MSG_NOT, kPacket );

	START_LOG( clog, L"[NOTIFY MESSAGE] => SEND CENTER SERVER " )
		<< L" ----------------------------- "	<< dbg::endl
		<< L"MSG : " << kPacket.m_wstrMSG	    << dbg::endl
		<< L" ----------------------------- "	<< dbg::endl;
}

void KGSSimLayer::SendNotMSG_GRP( const char* szNotMSG, int iCount /*= 1 */)
{
	std::wstring wstrMsg = KncUtil::toWideString( std::string( szNotMSG ) );

	SendNotMSG( KEGS_NOTIFY_MSG_NOT::NT_GM_NOTIFY_SERVER_GROUP, wstrMsg, iCount );
}

//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
void KGSSimLayer::SendNotMSG_ALL( const char* szNotMSG, int iCount /*= 1*/ )
{
	std::wstring wstrMsg = KncUtil::toWideString( std::string( szNotMSG ) );

	SendNotMSG( KEGS_NOTIFY_MSG_NOT::NT_GM_NOTIFY_ALL_SERVERS, wstrMsg, iCount );
}
//}}

void KGSSimLayer::AddIP( const char* szAddIP )
{
    if( szAddIP == NULL )
    {
        START_LOG( cerr, L"IP 스트링 이상." )
            << END_LOG;

        return;
    }

    std::string strIP = szAddIP;
    if( strIP.size() < 1 || strIP.size() > 15 )
    {
        START_LOG( cerr, L"추가하려는 IP 이상." )
            << BUILD_LOG( strIP.size() )
            << END_LOG;

        return;
    }

	START_LOG( cout, L"=== ADD IP ===" )
		<< BUILD_LOG( strIP.c_str() )
		<< BUILD_LOG( strIP.size() )
		<< L"================"
		<< dbg::endl;

	m_vecIP.push_back( strIP );
}

bool KGSSimLayer::CheckIP( const char* szIP )
{
    if( !CheckCommonFlag( KSimLayer::CF_CHECK_IP ) )
    {
        return true;
    }

	if( m_vecIP.empty() )
    {
        return true;
    }

    if( szIP == NULL )
    {
        START_LOG( cerr, L"비교하려는 IP 이상." )
            << END_LOG;

        return false;
    }

    std::string strIP = szIP;
    if( strIP.size() < 1 || strIP.size() > 15 )
    {
        START_LOG( cerr, L"비교하려는 IP 길이 이상." )
            << BUILD_LOG( strIP.size() )
            << END_LOG;

        return false;
    }

    std::vector< std::string >::iterator vit;
	for( vit = m_vecIP.begin(); vit != m_vecIP.end(); vit++ )
	{
        if( vit->size() < 1 || vit->size() > 15 )
        {
            START_LOG( cerr, L"비교하려는 IP 이상." )
                << BUILD_LOG( vit->size() )
                << END_LOG;

            return false;
        }

        if( vit->compare( 0, vit->size(), strIP, 0, vit->size() ) == 0 )
        {
            return true;
        }
	}

	return false;
}

void KGSSimLayer::AddRepeatEvent( u_short usEventID, int iCnt )
{
	std::map<u_short, int>::iterator mit;

	//반복 이벤트가 있는지 체크..
	mit = m_mapRepeatEvent.find( usEventID );

	//있으면..
	if( mit != m_mapRepeatEvent.end() )
	{
		//체크하여 반복획수가 더 많으면 새로 갱신.
		if( mit->second < iCnt )
			mit->second = iCnt;
	}
	//없다면..
	else
	{
		//새로 넣어줌.
		m_mapRepeatEvent.insert( std::make_pair( usEventID, iCnt ) );
	}
}

//{{ 2009. 9. 1  최육사		IOCP종료사유
void KGSSimLayer::AddAbnormalDisconnectReason( DWORD dwReason )
{
	std::map< DWORD, int >::iterator mit = m_mapAbnormalDisconnectReason.find( dwReason );
	if( mit == m_mapAbnormalDisconnectReason.end() )
	{
		m_mapAbnormalDisconnectReason.insert( std::make_pair( dwReason, 1 ) );
	}
	else
	{
		mit->second += 1;
	}
}

void KGSSimLayer::SendToDBAbnormalReason( bool bFinal )
{
	if( !m_mapAbnormalDisconnectReason.empty() )
	{
		KELOG_ABNORMAL_DISCONNECT kPacket;
		kPacket.m_iServerUID = KBaseServer::GetKObj()->GetUID();
		kPacket.m_mapAbnormalDisconnect = m_mapAbnormalDisconnectReason;
		KncSend( PC_SERVER, 0, PC_LOG_DB, 0, NULL, ELOG_ABNORMAL_DISCONNECT, kPacket );

		m_mapAbnormalDisconnectReason.clear();

		if( bFinal )
		{
			START_LOG( cout, L"비정상 종료 세부 사유 통계 DB 업데이트!" );
		}
	}
}
//}}

//{{ 2011. 04. 04  김민성   LFH
//}}

//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
void KGSSimLayer::TickDump_LUA()
{
	std::wstringstream wstrDump;
	KSimLayer::GetKObj()->TickDump( "KSimLayer", wstrDump );
	KActorManager::GetKObj()->TickDump( "KActorManager", wstrDump );
	SiKProxyManager()->TickDump( "KProxyManager", wstrDump );
	KBaseServer::GetKObj()->TickDump( "KBaseServer", wstrDump );
	SiKTRServer()->TickDump( "KTRServer", wstrDump );
	SiKGameEventManager()->TickDump( "KGameEventManager", wstrDump );
	SiKAbuserLogManager()->TickDump( "KAbuserLogManager", wstrDump );
	SiKChannelManager()->TickDump( "KChannelManager", wstrDump );
	dbg::cout << wstrDump.str() << dbg::endl;
}
//}}

void KGSSimLayer::ReloadXTRAP_MAP()
{
	KLocker lock( m_csXTrapReload );

	if( false == KSimLayer::GetKObj()->CheckCommonFlag( KSimLayer::CommonFlag::CF_XTRAP ) )
		return;

	bool bModified = false;

	FILE *fi;

	// 바뀐 맵파일이 있는지 검사
	for( int iCSFileIndex = 1; iCSFileIndex <= CSFILE_NUM; ++iCSFileIndex )
	{
		std::stringstream strstmFileName;
		strstmFileName << "map" << iCSFileIndex << ".cs3";

		fi = fopen( strstmFileName.str().c_str(), "rb" );	// MAP 파일 읽는 부분
		if( fi == NULL )
		{
			START_LOG( cerr, L"맵파일 로드 실패!" )
				<< BUILD_LOG( strstmFileName.str() )
				<< END_LOG;
			continue;
		}
		std::vector< char > vecXTrapMap;
		vecXTrapMap.resize( XTRAP_CS4_BUFSIZE_MAP );

		fread( &vecXTrapMap[0], XTRAP_CS4_BUFSIZE_MAP, 1, fi );
		fclose( fi );

		if( m_setLoadedMaps.find( vecXTrapMap ) == m_setLoadedMaps.end() )
		{
			// 아직 없던 맵이므로 추가합니다.
			bModified = true;
			m_setLoadedMaps.insert( vecXTrapMap );

			// 실제 xtrap에서 사용하는 버퍼를 채웁니다.
			m_vecUsingMap.resize( m_setLoadedMaps.size() * XTRAP_CS4_BUFSIZE_MAP );
			int iOffset = XTRAP_CS4_BUFSIZE_MAP * m_setLoadedMaps.size() - XTRAP_CS4_BUFSIZE_MAP;
			memcpy( &m_vecUsingMap[iOffset], &vecXTrapMap[0], XTRAP_CS4_BUFSIZE_MAP );

			START_LOG( clog2, L"다음 맵파일이 변경되었으므로 추가합니다." )
				<< BUILD_LOG( strstmFileName.str() )
				<< END_LOG;
		}
	}

	// 바뀐파일이 있을 경우 전체 로드합니다.
	if( bModified )
	{
		unsigned int result = XTrap_S_LoadDll();	// XTrap4Server.dll 로드

		if(result == 0)
			START_LOG( cout, L"XTrap Server DLL 로드 성공!");
		else
			START_LOG( cerr, L"XTrap Server DLL 로드 실패!");

		START_LOG( clog2, L"맵파일이 추가되어 현재 개수는 다음과 같습니다." )
			<< BUILD_LOG( m_setLoadedMaps.size() )
			<< END_LOG;

		XTrap_S_Start( 600, m_setLoadedMaps.size(), &m_vecUsingMap[0], NULL );	// XTrap server 모듈 시작
	}
}

void KGSSimLayer::DeleteCommonFlag_AllGS_LUA( DWORD dwFlag )
{
	KECN_DEL_COMMON_FLAG_NOT kNot;
	kNot.dwFlag = dwFlag;
	KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_CN_SERVER, 0, NULL, ECN_DEL_COMMON_FLAG_NOT, kNot );
}
void KGSSimLayer::AddCommonFlag_AllGS_LUA( DWORD dwFlag )
{
	KECN_ADD_COMMON_FLAG_NOT kNot;
	kNot.dwFlag = dwFlag;
	KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_CN_SERVER, 0, NULL, ECN_ADD_COMMON_FLAG_NOT, kNot );
}

void KGSSimLayer::SetServerGroupNum( int iGroupNum_ )
{
    m_iServerGroupNum = iGroupNum_;
}

int KGSSimLayer::GetServerGroupNum()
{
    return m_iServerGroupNum;
}
