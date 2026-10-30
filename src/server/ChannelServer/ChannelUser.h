#pragma once
#include "Actor.h"
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ActorManager.h"

#include "x2/platform/win32.h"    // ::GetTickCount()
#include "x2/platform/timer.h"

//{{ 2009. 10. 14  최육사	중복패킷차단
#include "RepeatEventFilter/RepeatEventFilter.h"
//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트
	#include "UserHackingManager.h"
//}}
//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	#include "UserSecurityManager.h"
//}}

SmartPointer(KChannelUser);



/**
@class : KChannelUser
@brief : 게임 클라이언트와 패킷을 주고받고 패킷들을 핸들링 하기위한 클래스
@date : 2012/09/28
*/
class KChannelUser : public KActor
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclPfID;

public:
    KChannelUser(void);
    virtual ~KChannelUser(void);

	enum ENUM_TIMER
	{
		TM_CHANNEL_LIST = 0,						// DB 업데이트 주기
		//{{ 2010. 02. 16  최육사	해킹툴 리스트
		TM_HACKING_TOOL_LIST,					// 해킹툴 리스트
		//}}
		//{{ 2012. 07. 04	최육사		대기열 시스템
		TM_WAIT_QUEUE,
		//}}

		TM_TOTAL_NUM,
	};

    // override
    virtual void Tick();

protected:  // util function, Packet Handling

	//{{ 2009. 10. 14  최육사	필터
	void		InitRepeatEventFilter();
	bool		CheckReqEvent( unsigned short usEventID );
	void		CheckAckEvent( unsigned short usEventID );
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	void			CheckHackingToolList();
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	bool				SetMachineID( IN const std::string& strMachineID );
	std::string			GetMachineID();
	//}}
	//{{ 2012. 07. 02	최육사		대기열 시스템
	void			SendChannelList();
	//}}

	void RequestPublisherLogin(const KECH_VERIFY_ACCOUNT_REQ& kPacket_);
	void HandlePublisherLoginAck(const KEPUBLISHER_AUTHENTICATION_ACK& kPacket_, OUT bool& bDoNotDisconnect);

	void RequestKOGOTPLogin(const KECH_VERIFY_ACCOUNT_REQ& kPacket_);
	void HandleKOGOTPLoginAck();

	void RequestPublisherSecurityAuth(const KECH_SECURITY_AUTH_REQ& kPacket_);
	void HandlePublisherSecurityAuthAck(const KEPUBLISHER_SECURITY_AUTH_ACK& kPacket_);

	void HandlePublisherSecurityInfoNot(const KEPUBLISHER_SECURITY_INFO_NOT& kPacket_);


	void ProcessLoginResultCount( IN const int iResult );

    // packet send function
    template < typename T > bool SendPacket( unsigned short usEventID, T& data, bool bLogging = true, bool bCompress = false );
	bool SendID( unsigned short usEventID );

    template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
    template < class T > void SendToAccountDB( unsigned short usEventID, const T& data );

	void SendToLogDB( unsigned short usEventID );

    DECL_ON_FUNC( ECH_VERIFY_ACCOUNT_REQ );
    //DECL_ON_FUNC( DBE_GASH_USER_LOGIN_ACK );// 해외에서 이제 사용안함
	DECL_ON_FUNC_NOPARAM( ECH_GET_CHANNEL_LIST_REQ );
	DECL_ON_FUNC_NOPARAM( ECH_DISCONNECT_REQ );

	//{{ 2009. 12. 16  최육사	동접툴
	DECL_ON_FUNC( E_TOOL_GET_CCU_INFO_REQ );
	DECL_ON_FUNC( DBE_GET_CONCURRENT_USER_INFO_ACK );
	//}}

	DECL_ON_FUNC_NOPARAM( E_RESERVE_DESTROY );

	//{{ 2011. 01. 13 김민성	동접 모니터링 툴
	DECL_ON_FUNC( E_TOOL_CHECK_LOGIN_REQ );
	DECL_ON_FUNC_NOPARAM( E_TOOL_SERVER_LIST_REQ );

	DECL_ON_FUNC( DBE_GET_CONCURRENT_USER_LOGIN_ACK );
	DECL_ON_FUNC( DBE_GET_GAME_SERVER_LIST_ACK );
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
	//}}

	DECL_ON_FUNC( EPUBLISHER_AUTHENTICATION_ACK );

	DECL_ON_FUNC( DBE_CH_USER_GENERAL_LOGIN_ACK );
	DECL_ON_FUNC( DBE_CH_USER_KOGOTP_LOGIN_ACK );

	DECL_ON_FUNC( ECH_SECURITY_AUTH_REQ );
	DECL_ON_FUNC( EPUBLISHER_SECURITY_AUTH_ACK );

	DECL_ON_FUNC( EPUBLISHER_SECURITY_INFO_NOT );



	DECL_ON_FUNC_NOPARAM( ECH_GET_SERVERGROUP_LIST_REQ );

	//{{김준환 서버시간 받아오기

    bool RoutePacket( const KEvent* pkEvent );   // CnUser, GSUser가 같이 선언하지만 상속은 아님.
    //                          ^ KEvent가 수정되지 않아야 한다. (SmartPtr로는 불가능)

    // override.
    virtual void ProcessEvent( const KEventPtr& spEvent );
    virtual void OnDestroy();   // LOGINED 상태로 빠져나오고, DB data update를 신청한다

private:
	x2::Timer					m_kTimer[TM_TOTAL_NUM];

	//{{ 2009. 10. 14  최육사	필터
	KRepeatEventFilter				m_kRepeatEventFilter;
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	KUserHackingManager				m_kUserHackingManager;
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	std::wstring					m_wstrID;



	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	KUserSecurityManager			m_kUserSecurityManager;
	//}}
	//{{ 2012. 07. 04	최육사		대기열 시스템
	bool							m_bPassedEnterChannel;
	//}}
	//{{ 2012. 07. 18	최육사		재접속 로직
	//}}
};

template < typename T >
bool KChannelUser::SendPacket( unsigned short usEventID, T& data, bool bLogging, bool bCompress )
{
	//{{ 2008. 8. 28  최육사	이벤트 필터
	CheckAckEvent( usEventID );
	//}}

    return KSession::SendPacket( PI_GS_PROXY, GetUID(), NULL, usEventID, data, bLogging, bCompress );
}

template < class T >
void KChannelUser::SendToLogDB( unsigned short usEventID_, const T& data_ )
{
    UidType anTrace[2] = { GetUID(), -1 };

    KncSend( GetPfID(), GetUID(), PI_LOG_DB, 0, anTrace, usEventID_, data_ );
}

template < class T >
void KChannelUser::SendToAccountDB( unsigned short usEventID_, const T& data_ )
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( GetPfID(), GetUID(), PI_ACCOUNT_DB, 0, anTrace, usEventID_, data_ );
}
