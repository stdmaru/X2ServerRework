#include "stdafx.h"
#include "./X2InstanceData.h"

#include <boost/assign.hpp>

CX2InstanceData::CX2InstanceData(void) :
m_pMiniMapUI( NULL ),
m_bIsDungeonLounge( false ),
m_bLoadedDungeonLoungeUnits( false ),
m_eDungeonLoungeWorldID( CX2World::WI_NONE ),
m_vLastDungeonGatePos( 0, 0, 0 ),
m_UserID( L"" ),
m_UserSessionID( L"" ),
m_UserPassword( L"" ),
m_bIDVerified(false),
m_wstrPurchaseTok( L"" ),
m_iConnectChannelServerID( -1 ),
m_bFirstJoinVillage(false)
//{{ kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
, m_iED( 0 )
//}} kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
, m_bShowCSPandSlotBDate( false )
, m_bIsChangeChannel(false)
, m_bRegisteredRank( false )
//{{ kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가
//}} kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가

//{{ kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리
#ifdef EXTEND_SERVER_GROUP_MASK
, m_iServerGroupID( -1 )	// 서버 선택이 안되있는 경우 솔레스로 초기화
, m_iServerGroupIDCashShop( -1 )	// 현재 가지고 있는 캐시아이템 리스트를 받아온 서버군 ID
, m_iServerGroupIDScript( -1 )
#else
, m_eServerGroupID( SGI_INVALID )	// 서버 선택이 안되있는 경우 솔레스로 초기화
, m_eServerGroupIDCashShop( SGI_INVALID )	// 현재 가지고 있는 캐시아이템 리스트를 받아온 서버군 ID
, m_eServerGroupIDScript( SGI_INVALID )
#endif // EXTEND_SERVER_GROUP_MASK
//}}  kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리
, m_bSendedHackUser(false)
, m_bSendedHackMail(false)
, m_bSendedHackMailAutoHack(false)
, m_bFirstSelect(false)	// kimhc // 로그인 후 처음 캐릭터 선택인가? // PC 방 인벤토리 디폴트 탭선택 작업
, m_bReceiveGameStartNotPacket( false ) // kimhc // 2010-03-23 // STATE_CHANGE_GAME_START_NOT를 받았는가?
, m_bSendGameStartReqPacket( false ) // kimhc // 2010-03-25 // Change_state_Game_start_req를 전송하면 직후에는 던전 게임이 시작 할 때 까지 스킬슬롯을 바꿀수 없도록 함
, m_fRemainedTimeByForceQuitGame(0.f)
, m_bIsPShopOpen(false)
, m_wstrAgencyShopExpirationDate(L"")
, m_strMachineId("")
, m_TimerForSendingPlayStatus( 3.0f )
, m_iMaxLevel( _CONST_X2GAME_::g_iMaxLevel )
, m_iChannelingCode( 0 )
{

#if X2_DEV_TOOLS
	m_bFrameScale = false;
#endif



#if !X2_DEV_TOOLS
	// checked

#endif


    m_vecHackList_MainThread.resize( 0 );
    {
        CSLock  lock( m_csHackList );
	    m_bChangeHacklist = false;
        m_vecHackList.clear();
    }



	m_bVerifyNpcHp = false;

#ifdef EXTEND_SERVER_GROUP_MASK
#else
	using namespace boost::assign;
	insert( m_mapServerGroupName )
		( SGI_INVALID, L"Invalid ServerGroup" )
		( SGI_SOLES, GET_STRING( STR_ID_4836 ) )
		( SGI_GAIA, GET_STRING( STR_ID_4835 ) )
		;
#endif // EXTEND_SERVER_GROUP_MASK



	m_bSendedLoadedModuleList = false;

	m_bMoneyMonitoring = false;
#if !X2_DEV_TOOLS
#endif





	m_iLastDungeonGatePosLineIndex = 0;



#ifdef FIX_REFORM_ENTRY_POINT_10TH			//	kimjh,  캐릭터 리스트 못받으면 재접속 유도
	m_vecTryConnectChannelID.clear();
#endif // FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  캐릭터 리스트 못받으면 재접속 유도


}

CX2InstanceData::~CX2InstanceData(void)
{
	SAFE_DELETE( m_pMiniMapUI );

}

void CX2InstanceData::OnFrameMove( double fTime, float fElapsedTime )
{
	//{{ 09.03.23 태완
	if( NULL != m_pMiniMapUI )
	{
		m_pMiniMapUI->OnFrameMove( fTime, fElapsedTime );
	}
	//}}

	// 강제 종료 까지 남은 시간 부분이 이상하게 조작 됬다면 바로 강제 종료
	if ( !m_fRemainedTimeByForceQuitGame.Verify() )
	{
		g_pMain->SetQuitType( NetError::ERR_CLIENT_QUIT_01 );
		PostQuitMessage(0);
	}

	if ( GetVerifyGageManagerTimer() > 0.0f )
	{
		m_fRemainedTimeByForceQuitGame -= fElapsedTime;
		if( m_fRemainedTimeByForceQuitGame <= 0.f )
		{
			m_fRemainedTimeByForceQuitGame = 0.f;
			if ( NULL != g_pKTDXApp )
			{
				// kimhc // 김현철 // 안 나가지면 여기서 직접 처리 하자
				// 전체 해킹 탐지 쪽 코드 리팩토링 좀 해야겠다...
				g_pMain->SetQuitType( NetError::ERR_CLIENT_QUIT_01 );
				PostQuitMessage(0);
			}
		}
	}
}

void CX2InstanceData::ResetMiniMapUI()
{
	SAFE_DELETE( m_pMiniMapUI );
	m_pMiniMapUI = new CX2MiniMapUI;
}

//{{ kimhc // 2009-12-18 // 서버군 추가 작업으로 사용하지 않음
//}} kimhc // 2009-12-18 // 서버군 추가 작업으로 사용하지 않음

wstring CX2InstanceData::GetConnectChannelServerName()
{
	return GetChannelServerName( m_iConnectChannelServerID );
}
#ifdef EXTEND_SERVER_GROUP_MASK
wstring CX2InstanceData::GetChannelServerName( int ChannelID, int ServerID)
{
	wstring wstrChannelName = L"";
	if ( m_vecChannelInfo.empty() == false )
	{
		for(int i = 0; i < static_cast<signed>( m_vecChannelInfo.size() ); i ++)
		{
			if(m_vecChannelInfo[i].m_iChannelID == ChannelID)
			{
				wstrChannelName = m_vecChannelInfo[i].m_wstrChannelName;
			}
		}
	}

	return wstrChannelName;
}
#else
wstring CX2InstanceData::GetChannelServerName( int ChannelID, SERVER_GROUP_ID ServerID)
{
	wstring wstrChannelName = L"";

	if ( m_vecChannelInfo.empty() == false )
	{
		for( UINT i=0; i < m_vecChannelInfo.size(); ++i )
		{
			if(m_vecChannelInfo[i].m_iChannelID == ChannelID)
			{
				wstrChannelName = m_vecChannelInfo[i].m_wstrChannelName;
			}
		}
	}
	return wstrChannelName;
}
#endif // EXTEND_SERVER_GROUP_MASK

wstring CX2InstanceData::GetChannelServerName( int ChannelID )
{
	//{{ kimhc // 2009-12-15 // 서버군 추가 작업
	if( 0 == ChannelID )
	{
		ErrorLogMsg(XEM_ERROR143,"ChannelID == 0");
		return L"";
	}

	wstring wstrChannelName = L"";
	if ( false == m_vecChannelInfo.empty() && (ChannelID-1) < static_cast<int>(m_vecChannelInfo.size()) )
	{
		for( UINT i = 0; i < m_vecChannelInfo.size(); ++i )
		{
			if(m_vecChannelInfo[i].m_iChannelID == ChannelID)
			{
				wstrChannelName = m_vecChannelInfo[i].m_wstrChannelName;
			}
		}

		return wstrChannelName;
	}
	else
	{
		ErrorLog(XEM_ERROR143);
		return wstrChannelName;
	}
	//}}  kimhc // 2009-12-15 // 서버군 추가 작업
}

//{{ kimhc // 2009-12-15 // 서버군 추가 작업

void CX2InstanceData::SetChannelDistributionByServerGroup( OUT float* pfChCongestionScopeTemp, IN const float* pfChCongestionScope, IN int iChannelIndex )
{
	// 채널 그룹에서 첫번째 채널인지를 판단하는 플래그 ex) 루벤01, 02, 03 채널중 루벤01이 첫번째 채널
	bool bIsFirstChannelInChannelGroup = false;

	switch ( g_pInstanceData->GetServerGroupID() )
	{
	case SGI_GAIA:
		{

			switch ( iChannelIndex )
			{
			case 0:
			case 3:
			case 6:
			case 7:
			case 11:
			case 12:
				{
					bIsFirstChannelInChannelGroup = true;
				}
				break;
			}
		}
		break;

	case SGI_SOLES:
	default:
		{
			switch ( iChannelIndex )
			{
			case 0:
			case 5:
			case 6:
			case 11:
			case 12:
			case 18:
				{
					bIsFirstChannelInChannelGroup = true;
				}
			}
		}
		break;
	}


	if ( bIsFirstChannelInChannelGroup == false )	// 채널 그룹에서 첫번째 채널이 아니면
	{
		std::copy( pfChCongestionScope, pfChCongestionScope + 4, pfChCongestionScopeTemp );
	}
	else
	{
		// 채널그룹중 첫채널은 분포도 다르게 표시
		pfChCongestionScopeTemp[0] = 0.1f;
		pfChCongestionScopeTemp[1] = 0.2f;
		pfChCongestionScopeTemp[2] = 0.3f;
		pfChCongestionScopeTemp[3] = pfChCongestionScope[3];
	}
}


#ifdef EXTEND_SERVER_GROUP_MASK
void CX2InstanceData::SetServerGroupID( int iServerGroupID )
{
	m_iServerGroupID = iServerGroupID;
}
#else
void CX2InstanceData::SetServerGroupID( SERVER_GROUP_ID eServerGroupID )
{
	m_eServerGroupID = eServerGroupID;
}
#endif // EXTEND_SERVER_GROUP_MASK
//}}  kimhc // 2009-12-15 // 서버군 추가 작업

// 채널 이름을 받아서 DLG_UI_Channel_Selection_New.lua 에 쓰이는 각 채널 버튼 이름을 반환함
wstring CX2InstanceData::GetChannelButtonNameByChannelName( wstring wstrChannelName )
{
	if ( wstrChannelName.find ( GET_STRING(STR_ID_17731) ) == 0 )	//루벤
	{
		return L"Ruben";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_17732) ) == 0)	//엘더
	{
		return L"Elder";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_17733) ) == 0)	//베스마
	{
		return L"Besma";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_17734) ) == 0)	//알테라
	{
		return L"Altera";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_23098) ) == 0 )	//페이타
	{
		return L"Peita";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_17735) ) == 0 )	//벨더
	{
		return L"Velder";
	}
	else if ( wstrChannelName.find ( GET_STRING(STR_ID_17736) ) == 0 )	//하멜
	{
		return L"Hamel";
	}

	return L"Ruben";
}
void CX2InstanceData::AddUserUnitDataInServer( SEnum::SERVER_GROUP_ID eServerGroupID, int iNowUserUnitManyInServer, int iMaxUserUnitManyInServer )
{
	m_mapUserUnitDataInServer.insert ( std::make_pair( eServerGroupID, std::make_pair ( iNowUserUnitManyInServer, iMaxUserUnitManyInServer) ) );
}

void CX2InstanceData::ClearUserUnitDataInServer( )
{
	m_mapUserUnitDataInServer.clear();
}


int CX2InstanceData::GetMaxUserUnitManyInServer( SEnum::SERVER_GROUP_ID eServerGroupID )
{
#ifdef FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
	return 500;
#else // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
	map < SEnum::SERVER_GROUP_ID, pair< int, int > >::const_iterator cit = m_mapUserUnitDataInServer.find ( eServerGroupID );
	if ( cit != m_mapUserUnitDataInServer.end() )
	{
		return cit->second.second;
	}

	ASSERT ( ! L"Can't Find Server Group ID ! " );

#endif // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
	return 0;
}


int CX2InstanceData::GetNowUserUnitManyInServer( SEnum::SERVER_GROUP_ID eServerGroupID )
{
	map < SEnum::SERVER_GROUP_ID, pair< int, int > >::const_iterator cit = m_mapUserUnitDataInServer.find ( eServerGroupID );
	if ( cit != m_mapUserUnitDataInServer.end() )
	{
		return cit->second.first;
	}

	ASSERT ( ! L"Can't Find Server Group ID ! " );

	return 0;
}


//{{ robobeg : 2013-12-19
    // 여러 곳에 중복되어 있는 코드를 한 곳으로 모읍니다.
bool CX2InstanceData::OpenScriptServerGroupFile()			// 이전에 플레이 했던 서버군 읽기
{
	string			strFileName;
	SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;
	bool			bParsingOK		= false;

    KLuaManagerProxy    luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	if( g_pKTDXApp->LoadAndDoMemory_LocalFile( &luaManager, g_pData->GetSavedServerGroupFileName().c_str() ) == true )
    {
        LUA_GET_VALUE_ENUM( luaManager, "SERVER_GROUP", 			eServerGroupID,			SERVER_GROUP_ID,		SGI_INVALID	);
    }

	switch ( eServerGroupID )
	{
	case SGI_SOLES:
	case SGI_GAIA:
		{
			g_pInstanceData->SetServerGroupID( eServerGroupID );
			bParsingOK = true;
		}
		break;

	default:
		{
			if( g_pMain->GetDefaultChannelServerIPIndex() == SGI_INVALID )
			{
				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( (rand() % 2) ) );
			}
			else
			{
				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
			}
		}
		break;

	}

	return bParsingOK;
}
//}} robobeg : 2013-12-19

//{{ kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장
bool    CX2InstanceData::SaveScriptServerGroupFile()
{
	string strFileName;

	ConvertWCHARToChar( strFileName, g_pData->GetSavedServerGroupFileName() );


	FILE* file = NULL;
	file = fopen( strFileName.c_str(), "w" );

	if( NULL == file )
	{
		ErrorLogMsg( XEM_ERROR7, strFileName.c_str() );
		return false;
	}

	fputc( 0xEF, file );
	fputc( 0xBB, file );
	fputc( 0xBF, file );

	fwprintf( file, L"  \n" );
	fwprintf( file, L"  \n" );
	fwprintf( file, L"SERVER_GROUP = " );

	switch( g_pInstanceData->GetServerGroupID() )
	{
	case SGI_GAIA:
		fwprintf( file, L"SERVER_GROUP_ID[\"SGI_GAIA\"]" );
		break;

	case SGI_SOLES:
	default:
		fwprintf( file, L"SERVER_GROUP_ID[\"SGI_SOLES\"]" );
		break;
	}

	fclose( file );

	return true;
}

//}}  kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장

#ifdef FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  캐릭터 리스트 못받으면 재접속 유도
bool CX2InstanceData::IsConnectedChannel ( int iChannelID_ )
{
	// 채널에 접속했었는지 확인한다.
	BOOST_FOREACH ( int iChannelID, m_vecTryConnectChannelID )
	{
		if ( iChannelID == iChannelID_ )
			return true;
	}
	return false;
}
void CX2InstanceData::SetConnectedChannelID ( int iChannelID_ )
{
	// 접속 시도한 채널의 아이디를 남긴다.
	m_vecTryConnectChannelID.push_back ( iChannelID_ );
}
void CX2InstanceData::ResetConnectedChannelID ()
{
	// 접속 시도한 채널의 아이디를 초기화 한다.
	m_vecTryConnectChannelID.clear();
}

#endif // FIX_REFORM_ENTRY_POINT_10TH	//	kimjh,  캐릭터 리스트 못받으면 재접속 유도

const wstring CX2InstanceData::GetServerGroupName()
{
#ifdef EXTEND_SERVER_GROUP_MASK
	map<int, KServerGroupInfo>::iterator iter = m_mapServerGroupInfo.find(GetServerGroupID());
	if( iter != m_mapServerGroupInfo.end() )
		return iter->second.m_wstrServerGroupName;
	else
		return std::wstring();
#else
	if( m_mapServerGroupName.end() != m_mapServerGroupName.find(GetServerGroupID()) )
		return m_mapServerGroupName[m_eServerGroupID];
	else
		return std::wstring();
#endif // EXTEND_SERVER_GROUP_MASK
}
