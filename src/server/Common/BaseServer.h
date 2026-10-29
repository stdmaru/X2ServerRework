#pragma once

#include <KncLua.h>
#include <memory>
#include <type_traits>
#include "Thread/Thread.h"
#include "StringTable.h"
#include "Performer.h"
#include <RTTI.h>
#include <ToString.h>
#include <KNCSingleton.h>
#include "CommonPacket.h"
//{{ 2010. 06. 20  최육사	통계 스레드
	#include "ThreadStatisticsManager.h"
//}}
//{{ 2010. 12. 7	최육사	메일 전송 시스템
	#include "Mail/MailManager.h"
//}}
#include "SecurityString.h"

//{{ 2010. 07. 13  최육사	using boost foreach
// include boost foreach

// (BOOST_MAP_FOREACH / BOOST_MAP_CONST_FOREACH removed: unused)
//}}


//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	#include "TickDumper.h"
//}}


class KODBC;
class KSimLayer;
class KBaseServer : public KPerformer
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclInstanceKeeping( KBaseServer );
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	DeclareTickDumper;
	//}}

public:
	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	enum VERSION_ENUM
	{
        VE_INTERNAL = 0,
		VE_OPEN_TEST,
		VE_SERVICE,

		VE_MAX,
	};
	//}}

	enum EServerClass
	{
		ESC_CENTER		= 1,
		ESC_LOGIN		= 2,
		ESC_GAME		= 3,
		ESC_CHANNEL		= 4,
		ESC_SIMULATION	= 5,
		ESC_GLOBAL		= 6,
	};


    KBaseServer(void);
    virtual ~KBaseServer(void);

	virtual bool Init();
    virtual void RegToLua();
    virtual void Run( bool bRunInterpreter );       // 콘솔일땐 자체 interpreter 구동, 윈도우일땐 무시.
    virtual void ShutDown();
    virtual bool DestroyAllSession() = 0;           // 접속중인 세션의 종료.
    //virtual void WriteServerInfoToDB();
    virtual void Tick();
	virtual void OnServerReadyComplete();

	//{{ 2009. 6. 15  최육사	cmd args
	void SetCmdArgs( const std::vector< KClearString< std::wstring > >& vecCmdArgs )	{ m_vecCmdArgs = vecCmdArgs; }
	void GetCmdArgs( std::vector< KClearString< std::wstring > >& vecCmdArgs )			{ vecCmdArgs = m_vecCmdArgs; }
	//}}

    void SetHwnd( HWND hWnd )                       { m_hWnd = hWnd; }
    HWND GetHwnd()                                  { return m_hWnd; }

	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	void SetUseVersion( int iVersionEnum )			{ m_eUseVersion = static_cast<VERSION_ENUM>(iVersionEnum); ToString( std::wcout ); }
	VERSION_ENUM	GetVersion( void ) const		{ return m_eUseVersion;	}
	//}}
    void SetShutDownWait( DWORD dwWait )            { m_dwShutDownWait = dwWait; }

    int  GetServerGroupID()                         { return m_iServerGroupID; }
    void SetServerGroupID( int iServerGroupID )     { m_iServerGroupID = iServerGroupID; }

    void GetServerList( KServerList& mapServerList );
    void SetServerList( const KServerList& mapServerList );

    int  GetServerClass()                           { return m_iServerClass; }
    void SetServerClass( int iServerClass )         { m_iServerClass = iServerClass; }

	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int  GetDefaultMaxUser() const					{ return m_nDefaultMaxUser; }
	void SetDefaultMaxUser( int nMaxUser )			{ m_nDefaultMaxUser = nMaxUser; }
	//}}

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	void SetPublicIP( IN const std::wstring& wstrIP )	{ m_wstrPublicIP = wstrIP; }
	const std::wstring& GetPublicIP() const			{ return m_wstrPublicIP; }
	void SetPrivateIP( IN const std::wstring& wstrIP )	{ m_wstrPrivateIP = wstrIP; }
	const std::wstring& GetPrivateIP() const			{ return m_wstrPrivateIP; }
	//}}

    //void GetServerInfo( KServerInfo& kServerInfo );
    void SetServerInfo( const KServerInfo& kServerInfo );

    void SetMailingList( const char* strList )      { m_strMailingList = KncUtil::toWideString( strList ); }

	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	const std::wstring& GetMainVersion() const;
	//}}
    template < class T > void SendToAccountDB( unsigned short usEventID, const T& data );
    template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	//{{ 2010. 12. 7	최육사	메일 전송 시스템
	template < class T > void QueueingEventToMailManager( unsigned short usEventID, const T& data );
	//}}
    void SendToAccountDB( unsigned short usEventID );
    void SendToLogDB( unsigned short usEventID );


	// 서버 로딩 상태
	bool GetServerReady()							{ return m_bServerReady; }
	void SetServerReady( bool bReady )				{ m_bServerReady = bReady; }

	//{{ 2008. 10. 10  최육사
	UidType GetFirstActorKey();
	//}}

	//{{ 2010. 8. 29	최육사	이벤트 큐 사이즈 덤프
	virtual void DumpToLogFile();
	//}}

	//{{ 2010. 11. 12	최육사	서버간 접속 모니터링
	//}}

	//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
	void SetScriptParsingErrReport_LUA( bool bVal );
	void AddScriptParsingErrReportMail_LUA( const char* pszMail );
	void AddFailScriptFileName( const wchar_t* pszScriptFileName )	{ m_vecFailScriptFileList.push_back( pszScriptFileName ); }
	bool IsScriptParsingSuccess() const				{ return m_vecFailScriptFileList.empty(); }
	//}}
	//{{ 2011. 02. 23	최육사	서버별 역할 타입
	char GetServerRollType() const	{ return m_cServerRollType; }
	//}}
	//{{ 2011.2.11  조효진  모니터링툴 서버 포트 체크 시 에러로그 남는거 수정
	void AddPortCheckMoritoringServer_LUA( const char* pStrIP );
	bool IsMonitoringServerForPortCheck( std::string strIP );
	//}}

	//{{ 2011. 04. 13  우편 및 거래 감시 실시간 SMS 전송
	//}} 2011. 04. 13 우편 및 거래 감시 실시간 SMS 전송



protected:
    inline void SetTagVersion();
    inline void LoadINIFile( const wchar_t* szFileName );
    bool Authenticate( DWORD dwTimeOut );

    std::wstring                                m_strTagVersion;            ///< 050716. florist. 태그빌드 버전 문자열
    std::wstring                                m_strMailingList;           ///< 061011. florist. crash report mailing list.
    std::wstring                                m_strIniFilePath;
	//{{ 2012. 12. 24	최육사	게임 서버 버전 세분화
	std::wstring								m_strMainVer[VE_MAX];
	VERSION_ENUM								m_eUseVersion;
	//}}
    HWND                                        m_hWnd;
    int                                         m_iServerGroupID;
    DWORD							            m_dwServerInfoUpdateTick;

    std::shared_ptr< std::remove_pointer_t< HANDLE > > m_spEventAck; // HANDLE is not void* outside Windows

    // 서버 리스트
    KServerList                                 m_mapServerList;
    mutable KncCriticalSection                  m_csServerList;

    int                                         m_iServerClass;
	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int                                         m_nDefaultMaxUser;
	//}}

	//{{ 2009. 6. 15  최육사	cmd args
	std::vector< KClearString< std::wstring > >	m_vecCmdArgs;
	//}}

public:
    KStringTable                                m_stringTable;
    DWORD                                       m_dwShutDownWait;
    static DWORD                                ms_dwServerInfoUpdateGap;

	// 서버 로딩 상태
	bool										m_bServerReady;

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	std::wstring								m_wstrPublicIP;
	std::wstring								m_wstrPrivateIP;
	//}}

	//{{ 2010. 06. 21  최육사	통계 스레드
	KThreadStatisticsManager					m_kTStatisticsManager;
	//}}

	//{{ 2010. 12. 7	최육사	메일 전송 시스템
	KMailManager								m_kMailManager;
	//}}

	//{{ 2010. 11. 12	최육사	서버간 접속 모니터링
	//}}
	//{{ 2011. 02. 23	최육사	서버별 역할 타입
	char										m_cServerRollType;
	//}}
	//{{ 2011. 02. 07	최육사	스크립트 파싱 오류 리포트
	bool										m_bScriptParsingErrReport;
	std::vector< std::wstring >					m_vecFailScriptFileList;
	std::vector< std::wstring >					m_vecScriptParsingErrMailList;
	//}}

	//{{ 2011.2.11  조효진  모니터링툴 서버 포트 체크 시 에러로그 남는거 수정
	std::set<std::string>					m_PortCheckMoritoringServerIPList;	// 센터 서버 포트를 체크하는 모니터링 서버의 아이피 리스프
	//}}
	//{{ 2011. 04. 13  우편 및 거래 감시 실시간 SMS 전송
	//}} 2011. 04. 13 우편 및 거래 감시 실시간 SMS 전송
};

extern lua_State* g_pLua;

//{{ 2010. 12. 7	최육사	메일 전송 시스템
template < class T >
void KBaseServer::QueueingEventToMailManager( unsigned short usEventID, const T& data )
{
	m_kMailManager.QueueingEvent( usEventID, data );
}
//}}

template < class T >
void KBaseServer::SendToAccountDB( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), GetUID(), PI_ACCOUNT_DB, 0, NULL, usEventID, data );
}

template < class T >
void KBaseServer::SendToLogDB( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), GetUID(), PI_LOG_DB, 0, NULL, usEventID, data );
}
