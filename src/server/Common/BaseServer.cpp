#include "x2/platform/net.h"
#include <filesystem>
#include "BaseServer.h"
#include "dbg/dbg.hpp"
#include "NetError.h"
#include "ThreadManager.h"
#include "Socket/NetCommon.h"
#include "NetLayer.h"
#include "SimLayer.h"
#include "DBLayer.h"
#include "KncSend.h"
#include "ActorManager.h"
//{{ 2010. 02. 12  최육사	서버 로그 매니저
//}}

//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

#include <ImportKncLua.h>
#include <ImportKncSerializer.h>
#include <ImportKncSecurity.h>
#include <ImportKncDebug.h>


// Port: the crash report (closed CrashRpt DLL + DumpUploader.exe, release builds only) was removed.


NiImplementRTTI( KBaseServer, KPerformer );
ImplInstanceKeeping( KBaseServer );

lua_State* g_pLua = NULL;

#define SERVER_INFO_UPDATE_GAP 60 * 1000
DWORD KBaseServer::ms_dwServerInfoUpdateGap( SERVER_INFO_UPDATE_GAP );

KBaseServer::KBaseServer(void)
:
    m_strTagVersion( L"Tag version undefined." ),
    m_eUseVersion( VE_INTERNAL ),
    m_hWnd( HWND_DESKTOP ),
    m_iServerGroupID( 0 ),
	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	m_dwShutDownWait( 1000 * 60 * 2 )
	//}}
{
    srand( (unsigned) time( NULL ) );
    m_dwServerInfoUpdateTick = ::GetTickCount();
    m_spEventAck.reset( CreateEvent( NULL, false, false, NULL ), CloseHandle );
	m_bServerReady = false;
	//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
	m_bScriptParsingErrReport = false;
	//}}
	//{{ 2011. 02. 23	최육사	서버별 역할 타입
	m_cServerRollType = KServerInfo::SRT_NONE;
	//}}
}

KBaseServer::~KBaseServer(void)
{
	//{{ 2010. 8. 30	최육사	서버 정상 종료 처리
	if( g_pLua != NULL )
	{
		lua_close( g_pLua );
		g_pLua = NULL;
	}
	//}}

    //dbg::profiler::kill_profiler( "profile_info.htm" );
}

ImplToStringW( KBaseServer )
{
    size_t uiServerList = 0;
    {
        KLocker lock( m_csServerList );
        std::map< int, std::vector< KServerInfo > >::const_iterator mit;
        for( mit = m_mapServerList.begin(); mit != m_mapServerList.end(); ++mit )
        {
            uiServerList += mit->second.size();
        }
    }

    START_TOSTRING_PARENTW( KPerformer )
        << TOSTRINGW( m_strMailingList )
        << TOSTRINGW( m_strTagVersion )
        << TOSTRINGW( uiServerList )
		//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
		<< TOSTRINGW( m_eUseVersion )
		<< TOSTRINGW( m_strMainVer[VE_INTERNAL] )
		<< TOSTRINGW( m_strMainVer[VE_OPEN_TEST] )
		<< TOSTRINGW( m_strMainVer[VE_SERVICE] )
		//}}
        << TOSTRINGW( m_dwShutDownWait );

//     m_stringTable.Dump( stm_ );

     return stm_;
}

#include <lua_tinker.h>

void KBaseServer::RegToLua()
{
	// global function set
	lua_tinker::def( g_pLua, "log",			&dbg::logfile::SetLogLevel );
	lua_tinker::def( g_pLua, "changelog",		&dbg::logfile::CloseFile );
	lua_tinker::def( g_pLua, "GetLocalIP",	&NetCommon::GetLocalIP );
	lua_tinker::def( g_pLua, "SetLocalIP",	&NetCommon::SetLocalIP );

 	lua_tinker::class_add<KThreadManager>( g_pLua, "KThreadManager" );
	lua_tinker::class_def<KThreadManager>( g_pLua, "Init",						&KThreadManager::Init );
	lua_tinker::class_def<KThreadManager>( g_pLua, "ResetMaxProcessingInfo",	&KThreadManager::ResetMaxProcessingInfo );
	lua_tinker::class_def<KThreadManager>( g_pLua, "SetThreadNum",				&KThreadManager::SetThreadNum );

 	lua_tinker::class_add<KBaseServer>( g_pLua, "KBaseServer" );
	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	lua_tinker::class_def<KBaseServer>( g_pLua, "SetUseVersion",					&KBaseServer::SetUseVersion );
	//}}
	lua_tinker::class_def<KBaseServer>( g_pLua, "SetShutDownWait",					&KBaseServer::SetShutDownWait );
	lua_tinker::class_def<KBaseServer>( g_pLua, "SetMailingList",					&KBaseServer::SetMailingList );
	//{{ 2009. 12. 17  최육사	서버군확장
	lua_tinker::class_def<KBaseServer>( g_pLua, "GetServerGroupID",					&KBaseServer::GetServerGroupID );
	//}}
	//{{ 2010. 11. 12	최육사	서버간 접속 모니터링
	//}}
	//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
	lua_tinker::class_def<KBaseServer>( g_pLua, "SetScriptParsingErrReport",		&KBaseServer::SetScriptParsingErrReport_LUA );
	lua_tinker::class_def<KBaseServer>( g_pLua, "AddScriptParsingErrReportMail",	&KBaseServer::AddScriptParsingErrReportMail_LUA );
	//}}

	//{{ 2011.2.11  조효진  모니터링툴 서버 포트 체크 시 에러로그 남는거 수정
	lua_tinker::class_def<KBaseServer>( g_pLua, "AddPortCheckMoritoringServer", &KBaseServer::AddPortCheckMoritoringServer_LUA );
	//}}
	//{{ 2011. 04. 13  우편 및 거래 감시 실시간 SMS 전송
	//}} 2011. 04. 13 우편 및 거래 감시 실시간 SMS 전송

	//{{ 2012. 1. 9	최육사		DB접속 암호 이중화
	lua_tinker::decl( g_pLua, "ESC_CENTER",			KBaseServer::ESC_CENTER );
	lua_tinker::decl( g_pLua, "ESC_LOGIN",			KBaseServer::ESC_LOGIN );
	lua_tinker::decl( g_pLua, "ESC_GAME",			KBaseServer::ESC_GAME );
	lua_tinker::decl( g_pLua, "ESC_CHANNEL",		KBaseServer::ESC_CHANNEL );
	lua_tinker::decl( g_pLua, "ESC_SIMULATION",		KBaseServer::ESC_SIMULATION );
	lua_tinker::decl( g_pLua, "ESC_GLOBAL",			KBaseServer::ESC_GLOBAL );
	//}}

	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	lua_tinker::decl( g_pLua, "VE_INTERNAL",		KBaseServer::VE_INTERNAL );
	lua_tinker::decl( g_pLua, "VE_OPEN_TEST",		KBaseServer::VE_OPEN_TEST );
	lua_tinker::decl( g_pLua, "VE_SERVICE",			KBaseServer::VE_SERVICE );
	//}}
}

extern const wchar_t* g_szConfigFile;
		const wchar_t* g_szConfigForderName = L"Config\\US\\";

#ifndef _DEBUG	// 디버그 안되는 오류 해외팀 수정
#include "Lottery.h"
#endif // _DEBUG

bool KBaseServer::Init()
{
#ifndef _DEBUG	// 디버그 안되는 오류 해외팀 수정
	KLottery::Seed( static_cast<int>(std::time(0)) );
	KLottery kLotTest;
	kLotTest.Decision();
#endif // _DEBUG

    dbg::logfile::SetFileNameAtTime( true );            // init log file

    _JIF( NetCommon::InitWinSock(), return false );     // socket init

    g_pLua = lua_open();

    _JIF( g_pLua != NULL, return false );

    //luaopen_base( g_pLua );  // 루아함수 tonumber를 사용해야 한다.
    //luaopen_table( g_pLua );
    //luaopen_io( g_pLua );
    //luaopen_string( g_pLua );
    //luaopen_math( g_pLua );

    luaL_openlibs( g_pLua );

    lua_settop( g_pLua, 0 );

    RegToLua();
	//{{ 2010. 06. 21  최육사	통계 스레드
	m_kTStatisticsManager.RegToLua();
	//}}

    SiKDBLayer()->RegToLua();
    KSimLayer::GetKObj()->RegToLua();
    KNetLayer::GetKObj()->RegToLua();

    LoadINIFile( g_szConfigFile );

    // 050317. config 파일이 lua, ini 두가지로 늘어남. 확장자는 별도로 붙인다.
	std::string strLuaFileName = KncUtil::toNarrowString( g_szConfigFile );

	if( m_vecCmdArgs.size() >= 3 ) // 전달인자 갯수가 2이상이면 버전태그가 전달인자로 들어온건지 확인함!
	{
		std::string strVersionTag = KncUtil::toNarrowString( m_vecCmdArgs[1].GetString() );
		std::string strServerGroupID = KncUtil::toNarrowString( m_vecCmdArgs[2].GetString() );

		// 읽을 config파일 이름 설정
		strLuaFileName += "_";
		strLuaFileName += strVersionTag;

		// 서버군ID 저장
        SetServerGroupID( static_cast<int>( atoi( strServerGroupID.c_str() ) ) );

		// cmdArgs에서 DB Conn Str를 제외하고 삭제한다
		while( m_vecCmdArgs.size() > 2 )
		{
			m_vecCmdArgs.erase( m_vecCmdArgs.begin() );
		}
	}
	else
	{
		START_LOG( cerr, L"서버 켤때 필요한 전달인자가 부족합니다!" )
			<< BUILD_LOG( m_vecCmdArgs.size() )
			<< END_LOG;
        return false;
	}

	strLuaFileName += ".lua";
	// 사내에서만 폴더를 나눈다
	if( (int)(strLuaFileName.find( "INTERNAL" )) != -1 ) // 찾았다
	{
		strLuaFileName = KncUtil::toNarrowString(g_szConfigForderName) + strLuaFileName;
	}

	START_LOG( cout, L"Config File Name : " << strLuaFileName );

    _JIF( 0 == LUA_DOFILE( g_pLua, strLuaFileName.c_str() ), return false );

    if( NetError::GetLastError() != NetError::NET_OK )
        return false;

	//{{ 2010. 02. 12  최육사	서버 로그 매니저
	//}}

    SiKDBLayer()->Init();
    KSimLayer::GetKObj()->Init();
	//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
	KSimLayer::GetKObj()->TickDumpReset();
	//}}
    KSimLayer::GetKObj()->Begin();

    _JIF( Authenticate( 10000 ), return false );

    // 서버 자체 인증을 거친 뒤에 Actor에서 접속을 받는다.
    KNetLayer::GetKObj()->Init();

    //{{ 050810. jseop. tag 버전 설정.
    SetTagVersion();    // 실행 파일 생성 시각 얻기.
    //}} 050810. jseop. tag 버전 설정.

	//{{ 2010. 06. 21  최육사	통계 스레드
	m_kTStatisticsManager.Init();
	//}}
	//{{ 2010. 12. 7	최육사	메일 전송 시스템
	m_kMailManager.Init();
	//}}
	//{{ 2007. 12. 24  최육사  사내서버라면 로그를 전송하지 않는다.
	//if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_NEXON_KOREA )
	//	m_kStatisticsManager.SetSendToFTP( true );
	//}}

	// cmd args는 이제 필요없으므로 삭제
	m_vecCmdArgs.clear();

    return true;
}

void KBaseServer::Run( bool bRunInterpreter_ )
{
    if( bRunInterpreter_ )  lua_interpreter( g_pLua );
}

void KBaseServer::ShutDown()
{
	//{{ 2009. 12. 15  최육사	서버관리
	START_LOG( cout, L"---------------------------------------- 서버 종료 덤프 ----------------------------------------" );
	//{{ 2010. 8. 29	최육사	이벤트 큐 사이즈 덤프
	DumpToLogFile();
	//}}
	KSimLayer::GetKObj()->DumpToLogFile();
	KNetLayer::GetKObj()->DumpToLogFile();
	SiKDBLayer()->DumpToLogFile();
	START_LOG( cout, L"---------------------------------------- 서버 종료 덤프 ----------------------------------------" );
	//}}

    KNetLayer::GetKObj()->CloseAccepter();  // 가장먼저 새로운 접속을 막는다.

    LIF( DestroyAllSession() );             // 접속중인 세션의 종료는 child에서 한다.

	//{{ 2010. 06. 21  최육사	통계 스레드
	m_kTStatisticsManager.Shutdown();
	//}}
	//{{ 2010. 12. 7	최육사	메일 전송 시스템
	m_kMailManager.Shutdown();
	//}}

	SendToLogDB( DBE_SERVER_OFF_NOT );

    SiKDBLayer()->ShutDown( 60000 );        // 60초 동안 DB 이벤트 처리를 기다린다.
    KSimLayer::GetKObj()->ShutDown();
    KNetLayer::GetKObj()->ShutDown();

	::WSACleanup();

    SiKDBLayer()->ReleaseInstance();
    KSimLayer::ReleaseKObj();
    KNetLayer::ReleaseKObj();
}

//void KBaseServer::WriteServerInfoToDB()
//{
//    KServerInfo kPacket;
//    GetServerInfo( kPacket );
//    SendToLogDB( DBE_UPDATE_SERVER_INFO_REQ, kPacket );
//
//    m_dwServerInfoUpdateTick = ::GetTickCount();
//}

void KBaseServer::Tick()
{
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	BEGIN_CHECK_TICK_LATENCY;
	//}}

    KPerformer::Tick();

	//{{ 2010. 06. 21  최육사	통계 스레드
	//}}

	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	SiKLogManager()->Tick();
	//}}

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	END_CHECK_TICK_LATENCY;
	//}}
}

void KBaseServer::OnServerReadyComplete()
{
	// 스크립트 파싱 에러 리포트
	if( m_bScriptParsingErrReport == false )
		return;

    if( IsScriptParsingSuccess() == true )
		return;

	std::wstring wstrLogFileName = KncUtil::toWideString( dbg::logfile::GetLogFileName() );

	// 메일로 서버 로그를 전송하자!
	KEMAIL_SEND_EMAIL_NOT kPacket;
	kPacket.m_vecRecvMailAddrList = m_vecScriptParsingErrMailList;

	// 제목
	kPacket.m_wstrTitle = L"스크립트 파싱 에러 리포트! : ";
	kPacket.m_wstrTitle += GetName();

	// 본문
	kPacket.m_wstrDesc = GetName();
	kPacket.m_wstrDesc += L" 서버를 켜는 도중에 스크립트 파싱이 실패하였습니다! 서버로그 확인 바랍니다!\n\n";

	for( const std::wstring& wstrScriptFileName : m_vecFailScriptFileList )
	{
		kPacket.m_wstrDesc += wstrScriptFileName;
		kPacket.m_wstrDesc += L"\n";
	}

	// 서버 로그 파일 첨부
	kPacket.m_vecAttachFileFullPathList.push_back( wstrLogFileName );
	QueueingEventToMailManager( EMAIL_SEND_EMAIL_NOT, kPacket );

	START_LOG( cout, L"--- 스크립트 파싱 에러 리포트 ---" )
		<< BUILD_LOG( m_bScriptParsingErrReport )
		<< BUILD_LOG( IsScriptParsingSuccess() )
		<< BUILD_LOG( wstrLogFileName );
}

void KBaseServer::SetTagVersion()
{
    KncUtil::GetTimeStampByNameW( m_strTagVersion );

    // the title is printed on initialization, so nothing may have been logged yet
    dbg::logfile::SetLogFileTitle( m_strTagVersion.c_str() );
}

//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace ConfigINI
{
    /*//// ...in ini file... ////////////////////////////////////////////////////

    [Version]
    Protocol Version 1 = XXX
    Protocol Version 2 = YYY

    [String]
    aaa = AAA
    bbb = BBB
    ...

    /////////////////////////////////////////////////////////////////////////*/

    const wchar_t* szSectionVersion = L"Version";
    const wchar_t* szSectionString  = L"String";
	const wchar_t* szKeyVersion1    = L"Internal Version";
	const wchar_t* szKeyVersion2    = L"OpenTest Version";
	const wchar_t* szKeyVersion3    = L"Service Version";
}

void KBaseServer::LoadINIFile( const wchar_t* szFileName_ )
{
	// 2005-03-18 florist: the ini file needs a full (or relative) path, otherwise Windows only looks in
	// the Windows folder. Use an absolute path to be sure.
	wchar_t buff[MAX_PATH] = {0};
	std::error_code ec;
	m_strIniFilePath = ( std::filesystem::current_path( ec ) / L"" ).wstring(); // with the trailing separator
	// separate config folder only in-house
	if( m_vecCmdArgs.size() >= 3 )
	{
		std::string strVersionTag = KncUtil::toNarrowString( m_vecCmdArgs[1].GetString() );
		if( (int)(strVersionTag.find( "INTERNAL" )) != -1 ) // 찾았다
		{
			m_strIniFilePath += g_szConfigForderName;
		}
	}
	m_strIniFilePath += szFileName_;
	m_strIniFilePath += L".ini";

	// 2005-03-17 read the version info. Port: our reader on every platform, since the deployed
	// config_*.ini is CP949 and Windows would read it in the system code page.
	x2::platform::ReadIniString( ConfigINI::szSectionVersion, ConfigINI::szKeyVersion1, 0, buff, MAX_PATH, m_strIniFilePath.c_str() );
	m_strMainVer[VE_INTERNAL] = buff;
	x2::platform::ReadIniString( ConfigINI::szSectionVersion, ConfigINI::szKeyVersion2, 0, buff, MAX_PATH, m_strIniFilePath.c_str() );
	m_strMainVer[VE_OPEN_TEST] = buff;
	x2::platform::ReadIniString( ConfigINI::szSectionVersion, ConfigINI::szKeyVersion3, 0, buff, MAX_PATH, m_strIniFilePath.c_str() );
	m_strMainVer[VE_SERVICE] = buff;

	LIF( !m_strMainVer[VE_INTERNAL].empty() );
	LIF( !m_strMainVer[VE_OPEN_TEST].empty() );
	LIF( !m_strMainVer[VE_SERVICE].empty() );

	if( !m_stringTable.LoadIni( m_strIniFilePath.c_str() ) )
	{
		START_LOG( cerr, L"INI 파일을 읽어 오지 못했습니다." )
			<< BUILD_LOG( m_strIniFilePath )
			<< END_LOG;
	}

	m_stringTable.SetCurrentSection( ConfigINI::szSectionString );
}

const std::wstring& KBaseServer::GetMainVersion() const
{
	if( m_eUseVersion < 0  ||  m_eUseVersion >= VE_MAX )
	{
		START_LOG( cerr, L"잘못된 버전 인덱스 입니다!" )
			<< BUILD_LOG( m_eUseVersion )
			<< END_LOG;

		return m_strMainVer[VE_INTERNAL];
	}

	return m_strMainVer[m_eUseVersion];
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}


void KBaseServer::SetServerList( const std::map< int, std::vector< KServerInfo > >& mapServerList )
{
    KLocker lock( m_csServerList );
    m_mapServerList = mapServerList;
}

void KBaseServer::GetServerList( std::map< int, std::vector< KServerInfo > >& mapServerList )
{
    KLocker lock( m_csServerList );
    mapServerList = m_mapServerList;
}

//void KBaseServer::GetServerInfo( KServerInfo& kServerInfo )
//{
//    kServerInfo.m_iUID = ( int )GetUID();
//    kServerInfo.m_wstrName = GetName();
//    kServerInfo.m_iServerGroupID = GetServerGroupID();
//    kServerInfo.m_iServerClass = GetServerClass();
//	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
//#ifdef SERV_PRIVATE_IP_SERVER_NETWORKING
//	kServerInfo.m_wstrIP = NetCommon::GetLocalIPW();
//#else
//	kServerInfo.m_wstrIP = NetCommon::GetLocalIPW();
//#endif SERV_PRIVATE_IP_SERVER_NETWORKING
//	//}}
//    kServerInfo.m_usMasterPort = KNetLayer::GetKObj()->m_usPortMaster;
//    kServerInfo.m_usNCUDPPort = KNetLayer::GetKObj()->GetNCUDPPort();
//	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
//#ifdef SERV_REALTIME_MAX_USER
//	kServerInfo.m_nMaxUser = GetDefaultMaxUser();
//#else
//	kServerInfo.m_nMaxUser = GetMaxUser();
//#endif SERV_REALTIME_MAX_USER
//	//}}
//    kServerInfo.m_nCurrentUser = KActorManager::GetKObj()->GetCount();
//    kServerInfo.m_bOn = true;
//}

void KBaseServer::SetServerInfo( const KServerInfo& kServerInfo )
{
	if( GetServerGroupID() != kServerInfo.m_iServerGroupID )
	{
		START_LOG( cerr, L"명령인자로 받은 ServerGroupID와 DB로부터 서버인증받은 ServerGroupID가 다릅니다!" )
			<< BUILD_LOG( GetServerGroupID() )
			<< BUILD_LOG( kServerInfo.m_iServerGroupID )
			<< END_LOG;
	}

    SetUID( kServerInfo.m_iUID );
    SetName( kServerInfo.m_wstrName );
    SetServerGroupID( kServerInfo.m_iServerGroupID );
    SetServerClass( kServerInfo.m_iServerClass );
	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	SetPublicIP( kServerInfo.m_wstrPublicIP );
	SetPrivateIP( kServerInfo.m_wstrPrivateIP );
	//}}
	KNetLayer::GetKObj()->SetPort( kServerInfo.m_usMasterPort );
    KNetLayer::GetKObj()->InitNCUDP( kServerInfo.m_usNCUDPPort );
	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	SetDefaultMaxUser( kServerInfo.m_nMaxUser );
	//}}
	//{{ 2011. 02. 23	최육사	서버별 역할 타입
	m_cServerRollType = kServerInfo.m_cServerRollType;
	if( m_cServerRollType != KServerInfo::SRT_NONE )
	{
		START_LOG( cout, L"--- 서버별 역할 부여 ---" )
			<< BUILD_LOGc( GetServerRollType() );
	}
	//}}
}

bool KBaseServer::Authenticate( DWORD dwTimeOut_ )
{
    KNetAddress kPacket;
    kPacket.m_wstrIP    = NetCommon::GetLocalIPW();
    kPacket.m_usPort    = KNetLayer::GetKObj()->m_usPortMaster;

    m_bAuthenticateOK = false;
    SendToLogDB( DBE_SERVER_ON_REQ, kPacket );

    switch( ::WaitForSingleObject( m_spEventAck.get(), dwTimeOut_ ) )
    {
    case WAIT_OBJECT_0: // the ack arrived: accepted or refused by the DB
        return m_bAuthenticateOK;
    case WAIT_TIMEOUT: // 시간 초과
        START_LOG( cerr, L"verify account timeout." );
        return false;
    default:
        START_LOG( cerr, L"invalide event object." );
        return false;
    }

    return false; // never get here.
}

//{{ 2008. 10. 10  최육사
UidType KBaseServer::GetFirstActorKey()
{
	return KActorManager::GetKObj()->GetFirstActorKey();
}
//}}

//{{ 2010. 8. 29	최육사	이벤트 큐 사이즈 덤프
void KBaseServer::DumpToLogFile()
{
	std::wstringstream wstrDump;
	ToString( wstrDump );
	dbg::cout << wstrDump.str() << dbg::endl;
}
//}}

//{{ 2010. 11. 12	최육사	서버간 접속 모니터링
//}}

//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
void KBaseServer::SetScriptParsingErrReport_LUA( bool bVal )
{
	m_bScriptParsingErrReport = bVal;

	START_LOG( cout, L"스크립트 파싱 오류 리포트 기능 동작 여부 : " << m_bScriptParsingErrReport );
}

void KBaseServer::AddScriptParsingErrReportMail_LUA( const char* pszMail )
{
	std::wstring wstrMail = KncUtil::toWideString( pszMail );
    m_vecScriptParsingErrMailList.push_back( wstrMail );

	START_LOG( cout, L"-- 스크립트 파싱 오류 리포트 대상 메일 주소 등록 --" )
		<< BUILD_LOG( wstrMail );
}

//}}

void KBaseServer::SendToAccountDB( unsigned short usEventID )
{
    SendToAccountDB( usEventID, char() );
}

void KBaseServer::SendToLogDB( unsigned short usEventID )
{
    SendToLogDB( usEventID, char() );
}


//{{ 2011.2.11  조효진  모니터링툴 서버 포트 체크 시 에러로그 남는거 수정
void KBaseServer::AddPortCheckMoritoringServer_LUA( const char* pStrIP )
{
	m_PortCheckMoritoringServerIPList.insert( pStrIP );
}

bool KBaseServer::IsMonitoringServerForPortCheck( std::string strIP )
{
	if( m_PortCheckMoritoringServerIPList.find(strIP) == m_PortCheckMoritoringServerIPList.end() )
		return false;
	else
		return true;
}
//}}
