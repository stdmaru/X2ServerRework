#include "LoginSimLayer.h"
#include <iomanip>  // std::setiosflags, std::resetiosflags
#include "ActorManager.h"
#include "DBLayer.h"
#include "LoginServer.h"
#include "KAutoPath.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

//{{ 2011. 04. 29	최육사	대리상인
	#include "LoginRoomManager.h"
//}}

#include "LoginUser.h"
//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
	#include "NexonAuthManager.h"
//}}



//{{ 2008. 7. 17  최육사  아케이드
#include "RankingManager.h"
//}}
//{{ 2009. 9. 22  최육사	길드
#include "GuildManager.h"
//}}
//{{ 2009. 10. 27  최육사	길드레벨
	#include "X2Data/XSLGuildManager.h"
//}}
//{{ 2009. 11. 30  최육사	길드스킬
	#include "X2Data/XSLSkillTree.h"
//}}
//{{ 2010. 02. 02  최육사	길드 게시판
	#include "GuildBoardManager.h"
//}}
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	#include "X2Data/XSLTitleManager.h"
//}}

//{{ 2010. 10. 13	최육사	DLL Manager
	#include "DLLManager.h"
//}}
//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "GameSysVal/GameSysVal.h"
//}}
//{{ 버프 관리 시스템 - 김민성
	#include "BuffManager.h"
//}

//{{ 2012. 11. 22	박세훈	SMS 전화번호 통합 관리
//}}


#include "DBLayer.h"
#include "NetError.h"

#include <lua_tinker.h>
using namespace lua_tinker;

#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

NiImplementRTTI( KLoginSimLayer, KSimLayer );

KLoginSimLayer::KLoginSimLayer(void)
{
	m_bPublisherCheckGameServerLogin = false;
}

KLoginSimLayer::~KLoginSimLayer(void)
{
}

ImplToStringW( KLoginSimLayer )
{
	//{{ 2011. 08. 11	최육사	투니 랜드 채널링
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_LOGIN_USER, NULL, ELG_DUMP_LOGIN_USER_NOT, char() );
	KActorManager::GetKObj()->QueueingToAll( spEvent );
	//}}

    return START_TOSTRING_PARENTW( KSimLayer );
}

ImplOstmOperatorW2A( KLoginSimLayer );

void KLoginSimLayer::RegToLua()
{
    KSimLayer::RegToLua();

    //class_<KLoginSimLayer>( "KLoginSimLayer" )
    //    .inh<KSimLayer>()
    //    .def( "dump", KLoginSimLayer::Dump );

    //decl( "SimLayer", this );
	lua_tinker::class_add<KLoginSimLayer>( g_pLua, "KLoginSimLayer" );
	lua_tinker::class_inh<KLoginSimLayer, KSimLayer>( g_pLua );
	//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
	lua_tinker::class_def<KLoginSimLayer>( g_pLua, "SetEnableAuthWait",	&KLoginSimLayer::SetEnableAuthWait_LUA );
	//}}
	lua_tinker::class_def<KLoginSimLayer>( g_pLua, "SetPublisherCheckGameServerLogin",	&KLoginSimLayer::SetPublisherCheckGameServerLogin_LUA );
	lua_tinker::class_def<KLoginSimLayer>( g_pLua, "dump",				&KLoginSimLayer::Dump );


	lua_tinker::decl( g_pLua, "SimLayer", this );

	//{{ 2011. 04. 29	최육사	대리상인
	GetKLoginRoomManager()->RegToLua();
	//}}

	//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
	SiKNexonAuthManager()->RegToLua();
	//}}


}

//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
void KLoginSimLayer::SetEnableAuthWait_LUA( bool bEnable )
{
	START_LOG( cout, L"PC방 인증 대기 기능 동작 여부 : " << bEnable );

	m_kUserList.SetEnableAuthWait( bEnable );
}
//}}

bool KLoginSimLayer::GetGSUserInfoByUserUID( UidType iUserUID, KUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByUserUID( iUserUID, kInfo );
}

bool KLoginSimLayer::GetGSUserInfoByUserID( const std::wstring& wstrUserID, KUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByUserID( wstrUserID, kInfo );
}

bool KLoginSimLayer::GetGSUserInfoByUnitUID( UidType iUnitUID, KUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByUnitUID( iUnitUID, kInfo );
}

bool KLoginSimLayer::GetGSUserInfoByUnitNickName( std::wstring& wstrUnitNickName, KUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByUnitNickName( wstrUnitNickName, kInfo );
}

//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
bool KLoginSimLayer::GetGSUserInfoByMachineID( IN const std::string& strMachineID, KUserList::KGSUserInfo& kInfo )
{
    return m_kUserList.GetGSUserInfoByMachineID( strMachineID, kInfo );
}
//}}

//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편

void KLoginSimLayer::CheckAuthSynchronize( const KENX_AUTH_SYNCHRONIZE_REQ& kInfo )
{
    m_kUserList.CheckAuthSynchronize( kInfo );
}

bool KLoginSimLayer::UpdateSessionNoByUserID( const std::wstring& wstrUserID, __int64 iSessionNo )
{
	return m_kUserList.UpdateSessionNoByUserID( wstrUserID, iSessionNo );
}

void KLoginSimLayer::LoginTimeOut( const KELG_NEXON_USER_LOGIN_TIME_OUT_NOT& kInfo )
{
    m_kUserList.LoginTimeOut( kInfo );
}

//}}


int KLoginSimLayer::RegUser( UidType iUserUID, UidType iGSUID, const KELG_REGISTER_USER_REQ& kInfo_, bool bChannelChanged )
{
    return m_kUserList.RegUser( iUserUID, iGSUID, kInfo_, bChannelChanged );
}

void KLoginSimLayer::UnRegUser( UidType iUserUID )
{
    m_kUserList.UnRegUser( iUserUID );
}

void KLoginSimLayer::UnRegAllUserOfThisGS( UidType iGSUID )
{
    m_kUserList.UnRegAllUserOfThisGS( iGSUID );
}

int KLoginSimLayer::RegUnit( UidType iUserUID, UidType iUnitUID, std::wstring& wstrUnitNickName, u_int& uiKNMSerialNum, bool bDenyFriendShip )
{
    return m_kUserList.RegUnit( iUserUID, iUnitUID, wstrUnitNickName, uiKNMSerialNum, bDenyFriendShip );
}

void KLoginSimLayer::UnRegUnit( UidType iUserUID )
{
    m_kUserList.UnRegUnit( iUserUID );
}

void KLoginSimLayer::Init()
{
    switch( GetAuthFlag() )
    {
    case KSimLayer::AF_NEXON_KOREA:
		//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
		m_vecpThreadMgr.push_back( SiKNexonAuthManager()->GetInstance() );
		//}}
        break;
    default:
        break;
    }



    KSimLayer::Init();

	KAutoPath kAutoPath;
	std::string		strFile;

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
			//}}
		}
		else
		{
			START_LOG( cout, L"DungeonEnum 정보 로드 성공.!" );
		}
	}

	//{{ 2009. 7. 6  최육사		랭킹개편
	{
		LoadingTimer lt(L"RankingData.lua");

		strFile = "RankingData.lua";
		kAutoPath.GetPullPath( strFile );
		if( SiKRankingManager()->OpenScriptFile( strFile.c_str() ) == false )
		{
			START_LOG( cerr, L"Ranking Data 정보 로드 실패.!" )
				<< BUILD_LOG( KncUtil::toWideString( strFile ) );

			//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
			KBaseServer::GetKObj()->AddFailScriptFileName( L"RankingData.lua" );
			//}}
		}
		else
		{
			START_LOG( cout, L"Ranking Data 정보 로드 성공.!" );
		}
	}
	//}}

	//{{ 2009. 10. 27  최육사	길드레벨
	{
		CXSLGuildManager::RegScriptName( "GuildData.lua" );
		OPEN_SCRIPT_FILE( CXSLGuildManager );
	}
	//}}

	//{{ 2009. 11. 30  최육사	길드스킬
	CXSLSkillTree::RegScriptName( "NewSkillTempletVer2.lua" );
	OPEN_SCRIPT_FILE( CXSLSkillTree );
	//}}

	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	{
		KGameSysVal::RegScriptName( "GameSysValTable.lua" );
		OPEN_SCRIPT_FILE( KGameSysVal );
	}
	//}}

	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	{
		CXSLTitleManager::RegScriptName( "TitleTable.lua" );
		CXSLTitleManager::RegScriptName( "TitleMission.lua" );
		CXSLTitleManager::RegScriptName( "SubTitleMission.lua" );
		OPEN_SCRIPT_FILE( CXSLTitleManager );
	}
	//}}

	//{{ 2012. 11. 22	박세훈	SMS 전화번호 통합 관리
	//}}

	//{{ 2009. 9. 22  최육사	길드
	SiKGuildManager(); // 싱글톤 생성
	//}}

	//{{ 버프 관리 시스템 - 김민성
	SiKBuffManager(); // 싱글톤 생성
	//}
}

void KLoginSimLayer::Tick()
{
    KSimLayer::Tick();

    switch( GetAuthFlag() )
    {
    case KSimLayer::AF_NEXON_KOREA:
		//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
		SiKNexonAuthManager()->Tick();
		//}}
        break;
    default:
        break;
    }




	//{{ 2009. 7. 6  최육사		랭킹 개편
	SiKRankingManager()->Tick();
	//}}

	//{{ 2009. 11. 26  최육사	길드스킬
	SiKGuildManager()->Tick();
	//}}

	//{{ 2010. 02. 02  최육사	길드 게시판
	SiKGuildBoardManager()->Tick();
	//}}

	//{{ 2010. 10. 13	최육사	DLL Manager
	SiKDLLManager()->Tick();
	//}}

	//{{ 2010. 02. 24  최육사	웹 포인트 이벤트
	//}}

	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

	//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
	m_kUserList.Tick();
	//}}
}

void KLoginSimLayer::ShutDown()
{
    KSimLayer::ShutDown();

    switch( GetAuthFlag() )
    {
    case KSimLayer::AF_NEXON_KOREA:
		//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
		SiKNexonAuthManager()->ReleaseInstance();
		//}}
        break;
    default:
        break;
    }



	//{{ 2009. 7. 6  최육사		랭킹 개편
	KRankingManager::ReleaseInstance();
	//}}
	//{{ 2009. 9. 22  최육사	길드
	KGuildManager::ReleaseInstance();
	//}}
	//{{ 2009. 10. 27  최육사	길드레벨
	CXSLGuildManager::ReleaseInstance();
	//}}
	//{{ 2009. 11. 30  최육사	길드스킬
	CXSLSkillTree::ReleaseInstance();
	//}}
	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	CXSLTitleManager::ReleaseInstance();
	//}}
	//{{ 2012. 11. 22	박세훈	SMS 전화번호 통합 관리
	//}}
}

//{{ 2009. 4. 1  최육사		친구 차단
bool KLoginSimLayer::SetDenyFriendShip( UidType iUnitUID, bool bDenyFriendShip )
{
	return m_kUserList.SetDenyFriendShip( iUnitUID, bDenyFriendShip );
}
//}}
