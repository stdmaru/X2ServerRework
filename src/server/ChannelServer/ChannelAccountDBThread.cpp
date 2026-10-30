#include <winsock2.h>
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "ChannelAccountDBThread.h"
#include "ChannelServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "SimLayer.h"

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	#include "MachineBlockManager.h"
//}}

//{{ 2010/10/23 조효진  랜돔 시드 문제 때문에 부스터 Random으로 교체함
#include <boost/random.hpp>
//}}
#define CLASS_TYPE KChannelAccountDBThread
ImplPfID( KChannelAccountDBThread, PI_CHANNEL_ACCOUNT_DB );

IMPL_PROFILER_DUMP( KChannelAccountDBThread )
{
    for( unsigned int ui = 0; ui < vecDump.size(); ++ui )
    {
        unsigned int iAvg = 0;
        if( vecDump[ui].m_iQueryCount > 0 )	iAvg = vecDump[ui].m_iTotalTime / vecDump[ui].m_iQueryCount;

        DO_QUERY_NO_PROFILE( L"exec dbo.mup_insert_querystats", L"N\'%s\', %d, %d, %d, %d, %d, %d",
            % vecDump[ui].m_wstrQuery
            % vecDump[ui].m_iMinTime
            % iAvg
            % vecDump[ui].m_iMaxTime
            % vecDump[ui].m_iOver1Sec
            % vecDump[ui].m_iQueryCount
            % vecDump[ui].m_iQueryFail
            );

        continue;

end_proc:
        START_LOG( cerr, vecDump[ui].m_wstrQuery )
            << BUILD_LOG( vecDump[ui].m_iMinTime )
            << BUILD_LOG( vecDump[ui].m_iMaxTime )
            << BUILD_LOG( vecDump[ui].m_iTotalTime )
            << BUILD_LOG( vecDump[ui].m_iQueryCount )
            << BUILD_LOG( iAvg )
            << BUILD_LOG( vecDump[ui].m_iOver1Sec )
            << BUILD_LOG( vecDump[ui].m_iQueryFail )
            << END_LOG;
    }
}

KChannelAccountDBThread::~KChannelAccountDBThread(void)
{
}

void KChannelAccountDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
		CASE( DBE_CH_USER_GENERAL_LOGIN_REQ );
		CASE( DBE_CH_USER_KOGOTP_LOGIN_REQ );

        //CASE( DBE_GASH_USER_LOGIN_REQ );
		//{{ 2011. 01. 13 김민성	동접 모니터링 툴
		CASE( DBE_GET_CONCURRENT_USER_LOGIN_REQ );
		//}}
		//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
		//}}
		//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
		CASE( DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ );
		//}}
		//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
		//}}

    default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }
}

void KChannelAccountDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KChannelAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

//{{ 2011.1.24 조효진
IMPL_ON_FUNC( DBE_CH_USER_GENERAL_LOGIN_REQ )
{
	// This request is an event requested when authentication with the publisher has been completed successfully.
	KDBE_CH_USER_GENERAL_LOGIN_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	kPacket.m_wstrOTP = L"";	// Leave blank before receiving new OTP
	kPacket.m_iChannelingCode = kPacket_.m_iChannelingCode;

		std::wstring wstrAccountID = kPacket_.m_wstrServiceAccountID;
		kPacket.m_wstrUserID = wstrAccountID;

				DO_QUERY( L"exec dbo.mup_auth_global_user", L"N\'%s\', %d", % wstrAccountID.c_str() % kPacket_.m_iChannelingCode );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_iUserUID
		);

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		// Account DB authentication failed
		START_LOG( clog, L"Account does not exist or delete/password other." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
			case -1:	kPacket.m_iOK = NetError::ERR_VERIFY_04;	break;
			// If an account is deleted in the future, processing is required when the deleted user logs in.
			case -2:    kPacket.m_iOK = NetError::ERR_VERIFY_18;	break;
			case -3:	kPacket.m_iOK = NetError::ERR_VERIFY_19;	break;
			// Added blocking part
				case -5:	kPacket.m_iOK = NetError::ERR_VERIFY_11;	break;
			case -7:	kPacket.m_iOK = NetError::ERR_VERIFY_06;	break;  // For test server
			default:	kPacket.m_iOK = NetError::ERR_UNKNOWN;	break;
		}

		// Create new user. - If successful, mup_update_user_otp is called normally.
		if( kPacket.m_iOK == NetError::ERR_VERIFY_04 )
		{
				DO_QUERY( L"exec dbo.mup_create_global_user", L"N\'%s\', N\'%s\', %d, %d, %d, N\'%s\', %d, %d",
					% wstrAccountID
					% kPacket_.m_wstrIP	// Set the OTP field blank when creating //kPacket_.m_wstrOTP
					% kPacket_.m_uiPublisherUID
					% 0
					% 0
					% kPacket_.m_wstrServiceAccountID
					% 0
					% kPacket_.m_iChannelingCode
				);

			bool bPlayGuide;
			bool bInternalUser;
			bool bGuestUser;
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( kPacket.m_iOK
					>> kPacket.m_iUserUID
					>> bPlayGuide
					>> bInternalUser
					>> bGuestUser
				);
				m_kODBC.EndFetch();
			}

			if( kPacket.m_iOK != NetError::NET_OK )
			{
				START_LOG( clog, L"Failed to create new user." )
					<< BUILD_LOG( kPacket.m_iOK )
					<< END_LOG;
			}
		}
	}

	if( kPacket.m_iOK == NetError::NET_OK )
	{
		static boost::mt19937 generator(timeGetTime());	// Fixed seed except for timeGetTime()
		static boost::uniform_int<> uni_dist(0,INT_MAX) ;
		static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gen(generator, uni_dist);

		int iOTP  = gen();	// Generate iOTP value within the range of 2.1 billion

		wchar_t wszOTP[128];
			_itow_s( iOTP, wszOTP, 10 );
		kPacket.m_wstrOTP = wszOTP;

			DO_QUERY( L"exec dbo.mup_update_user_otp", L"%d, N\'%s\'",
				% kPacket.m_iUserUID
				% kPacket.m_wstrOTP );

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK
				);
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			// OTP update failed
			START_LOG( clog, L"OTP 업데이트 실패." )
				<< BUILD_LOG( kPacket.m_iOK )
				<< END_LOG;
		}

		//{{ 2010.12.02 Jo Hyo-jin It will only be included if everything is ultimately successful.
			kPacket.m_wstrPurchaseTok	= kPacket_.m_wstrPurchaseTok;
		//}}

	}


	end_proc:
		LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
			<< BUILD_LOG( kPacket_.m_wstrServiceAccountID )
		<< BUILD_LOG( kPacket.m_wstrOTP )
		<< BUILD_LOG( kPacket.m_iOK )
		<< BUILD_LOG( kPacket.m_iUserUID )
		<< BUILD_LOG( kPacket.m_wstrPurchaseTok )
		<< END_LOG;

	SendToUser( FIRST_SENDER_UID, DBE_CH_USER_GENERAL_LOGIN_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_CH_USER_KOGOTP_LOGIN_REQ )
{
	KDBE_CH_USER_KOGOTP_LOGIN_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_wstrUserID = kPacket_.m_wstrServiceAccountID;
	kPacket.m_wstrOTP = kPacket_.m_wstrOTP;

	//{{ 2011. 1. 24  조효진	*중요* 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	// 초기 로그인 요청에서 bLogin 값을 강제로 false로 바꿔 패킷을 이쪽으로 보낼 수 있음.
	// 이 경우 OTP 스트링을 이용해 쿼리 인젝션 공격을 시도 할 수 있음
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrOTP ) )
	{
		kPacket.m_iOK = NetError::ERR_UNKNOWN;
		goto end_proc;
	}
	//}}

	DO_QUERY( L"exec dbo.mup_auth_global_user", L"N\'%s\', %d", % kPacket_.m_wstrServiceAccountID.c_str() % kPacket_.m_iChannelingCode );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_iUserUID
			);

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		// 계정 DB 인증 실패
		START_LOG( clog, L"존재하지 않거나 삭제/비번 다른 계정." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1:	kPacket.m_iOK = NetError::ERR_VERIFY_04;	break;
		// 추후에 계정 삭제가 들어갈 경우 삭제된 유저가 접속했을 때의 처리가 필요하다.
		case -2:    kPacket.m_iOK = NetError::ERR_VERIFY_18;	break;
		case -3:	kPacket.m_iOK = NetError::ERR_VERIFY_19;	break;
		// 차단 부분 추가함
			case -5:	kPacket.m_iOK = NetError::ERR_VERIFY_11;	break;
		default:
			kPacket.m_iOK = NetError::ERR_UNKNOWN;
			break;
		}

		// KOGOTP로는 신규 유저 처리 실패가 정상적인 방법으로는 나올 수가 없음
		// 따라서 신규 유저 처리 실패에 대해 등록해 주는 부분 없음

		goto end_proc;
	}


	// OTP 인증 먼저 한다.
	DO_QUERY( L"exec dbo.mup_verify_user_otp", L"%d, N\'%s\'",
		% kPacket.m_iUserUID
		% kPacket.m_wstrOTP );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			);
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		// OTP 검증 실패
		START_LOG( clog, L"OTP 검증 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket.m_iUserUID )
			<< BUILD_LOG( kPacket.m_wstrOTP )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_UNKNOWN;
		goto end_proc;
	}


	if( kPacket.m_iOK == NetError::NET_OK )
	{
		static boost::mt19937 generator(timeGetTime());	// timeGetTime() 빼면 고정 시드
		static boost::uniform_int<> uni_dist(0,INT_MAX) ;
		static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gen(generator, uni_dist);

		int iOTP  = gen();	// 21억 범위내에서 iOTP 값 생성


		wchar_t wszOTP[128];
		::_itow( iOTP, wszOTP, 10 );
		kPacket.m_wstrOTP = wszOTP;

		DO_QUERY( L"exec dbo.mup_update_user_otp", L"%d, N\'%s\'",
			% kPacket.m_iUserUID
			% kPacket.m_wstrOTP );

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK
				);
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			// OTP 업데이트 실패
			START_LOG( clog, L"OTP 업데이트 실패." )
				<< BUILD_LOG( kPacket.m_iOK )
				<< END_LOG;
		}

		//{{ 2010.12.02 조효진  최종적으로 다 성공한 경우에만 넣어준다.
		kPacket.m_wstrPurchaseTok	= kPacket_.m_wstrPurchaseTok;
		//}}
	}

end_proc:
	LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( kPacket_.m_wstrServiceAccountID )
		<< BUILD_LOG( kPacket_.m_wstrOTP )
		<< BUILD_LOG( kPacket.m_iOK )
		<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
		<< BUILD_LOG( kPacket.m_iUserUID )
		<< BUILD_LOG( kPacket.m_wstrPurchaseTok )
		<< END_LOG;


	SendToUser( FIRST_SENDER_UID, DBE_CH_USER_KOGOTP_LOGIN_ACK, kPacket );
}
//}} 2011.1.24 조효진 KOG_OTP_MODIFY

/*
IMPL_ON_FUNC( DBE_GASH_USER_LOGIN_REQ )
{
    KDBE_GASH_USER_LOGIN_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;
    kPacket.m_wstrOTP = kPacket_.m_wstrOTP;

    DO_QUERY( L"exec dbo.mup_auth_gash_user", L"N\'%s\', N\'%s\'", % kPacket_.m_wstrServiceAccountID.c_str() % kPacket_.m_wstrOTP.c_str() );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_iOK
            >> kPacket.m_iUserUID
            );

        m_kODBC.EndFetch();
    }

    if( kPacket.m_iOK != NetError::NET_OK )
    {
        // 계정 DB 인증 실패
        START_LOG( clog, L"존재하지 않거나 삭제/비번 다른 계정." )
            << BUILD_LOG( kPacket.m_iOK )
            << END_LOG;

        switch( kPacket.m_iOK )
        {
        case -1:
            kPacket.m_iOK = NetError::ERR_VERIFY_04;
            break;
        case -2:            // 추후에 계정 삭제가 들어갈 경우 삭제된 유저가 접속했을 때의 처리가 필요하다.
            kPacket.m_iOK = NetError::ERR_VERIFY_18;
            break;
        case -3:
            kPacket.m_iOK = NetError::ERR_VERIFY_19;
            break;
        default:
            kPacket.m_iOK = NetError::ERR_UNKNOWN;
            break;
        }

        // 신규 유저 생성.
        if( kPacket.m_iOK == NetError::ERR_VERIFY_04 )
        {
            // 이 쓰레드에서 sim layer 변수를 참조하는 것이 그닥 좋은 것은 아니다.
            DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();
            if( dwAuthFlag != KSimLayer::AF_GAMANIA_TAIWAN )
            {
                START_LOG( cerr, L"인증 플래그 이상." )
                    << BUILD_LOG( dwAuthFlag )
                    << END_LOG;

                goto end_proc;
            }

            DO_QUERY( L"exec dbo.mup_create_gash_user", L"N\'%s\', \'%s\', %d, %d, %d, N\'%s\', %d",
                % kPacket_.m_wstrServiceAccountID
                % kPacket_.m_wstrOTP
                % 0
                % 0
                % 0
                % kPacket_.m_wstrServiceAccountID
                % 0
                );

            bool bPlayGuide;
            bool bInternalUser;
            bool bGuestUser;
            if( m_kODBC.BeginFetch() )
            {
                FETCH_DATA( kPacket.m_iOK
                    >> kPacket.m_iUserUID
                    >> bPlayGuide
                    >> bInternalUser
                    >> bGuestUser
                    );
                m_kODBC.EndFetch();
            }

            if( kPacket.m_iOK != NetError::NET_OK )
            {
                // 신규 유저 생성 실패.
                START_LOG( clog, L"신규 유저 생성 실패." )
                    << BUILD_LOG( kPacket.m_iOK )
                    << END_LOG;
            }
        }
    }

    if( kPacket.m_iOK == NetError::NET_OK )
    {
        int iOTP = rand();
        wchar_t wszOTP[128];
        ::_itow( iOTP, wszOTP, 10 );
        kPacket.m_wstrOTP = wszOTP;

        DO_QUERY( L"exec dbo.mup_update_user_otp", L"%d, N\'%s\'",
            % kPacket.m_iUserUID
            % kPacket.m_wstrOTP );

        if( m_kODBC.BeginFetch() )
        {
            FETCH_DATA( kPacket.m_iOK
                );
            m_kODBC.EndFetch();
        }

        if( kPacket.m_iOK != NetError::NET_OK )
        {
            // OTP 업데이트 실패
            START_LOG( clog, L"OTP 업데이트 실패." )
                << BUILD_LOG( kPacket.m_iOK )
                << END_LOG;
        }
    }

end_proc:
    LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
#ifndef SERV_PRIVACY_AGREEMENT
        << BUILD_LOG( kPacket_.m_wstrServiceAccountID )
#endif SERV_PRIVACY_AGREEMENT
        << BUILD_LOG( kPacket_.m_wstrOTP )
        << BUILD_LOG( kPacket.m_iOK )
        << BUILD_LOG( kPacket.m_iUserUID )
        << END_LOG;

    SendToUser( FIRST_SENDER_UID, DBE_GASH_USER_LOGIN_ACK, kPacket );

}
//*/

//{{ 2011. 01. 13 김민성	동접 모니터링 툴
IMPL_ON_FUNC( DBE_GET_CONCURRENT_USER_LOGIN_REQ )
{
	KDBE_GET_CONCURRENT_USER_LOGIN_ACK kPacket;

	kPacket.m_iOK = NetError::NET_OK;

	DO_QUERY( L"exec dbo.mup_login_mornitor_user", L"N\'%s\', N\'%s\'", % kPacket_.m_wstrID.c_str() % kPacket_.m_wstrPW.c_str() );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iAuthLevel );

		m_kODBC.EndFetch();
	}
end_proc:
    SendToUser( FIRST_SENDER_UID, DBE_GET_CONCURRENT_USER_LOGIN_ACK, kPacket );

}
//}}

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
IMPL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ )
{
	KDBE_CHECK_ACCOUNT_BLOCK_LIST_ACK kPacket;

	// Release Tick 얻기
	DO_QUERY_NO_ARG( L"exec dbo.mup_get_release_tick" );

	while( m_kODBC.Fetch() )
	{
		int iType = 0;
		int iReleaseTick = 0;

		FETCH_DATA( iType
			>> iReleaseTick );

		// Machine ID 만 필요하다
		if( iType != KMachineBlockManager::BT_MACHINE_ID_BLOCK )
			continue;

		// 해당 타입 있는가?
		std::map< int, int >::const_iterator mit = kPacket_.m_mapReleaseTick.find( iType );
		if( mit == kPacket_.m_mapReleaseTick.end() )
			continue;

		// 해당 타입의 Tick값보다 높은가?
		if( iReleaseTick <= mit->second )
			continue;

		kPacket.m_mapReleaseTick.insert( std::make_pair( iType, iReleaseTick ) );
	}

	if( kPacket.m_mapReleaseTick.empty() )
		return;

	{
		std::map< int, int >::const_iterator mit;
		for( mit = kPacket.m_mapReleaseTick.begin(); mit != kPacket.m_mapReleaseTick.end(); ++mit )
		{
			switch( mit->first )
			{
			case KMachineBlockManager::BT_MACHINE_ID_BLOCK:
				{
					// 거래 블럭 리스트 얻기
					DO_QUERY_NO_ARG( L"exec dbo.mup_get_add" );

					while( m_kODBC.Fetch() )
					{
						std::string strMachineID;

						FETCH_DATA( strMachineID );

						kPacket.m_vecMachineIDBlockList.push_back( strMachineID );
					}
				}
				break;
			default:
				break;
			}
		}
	}

end_proc:
	SendToServer( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK, kPacket );
}
//}}

//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
//}}
