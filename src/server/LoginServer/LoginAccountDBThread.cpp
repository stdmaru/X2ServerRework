#include <winsock2.h>
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "LoginAccountDBThread.h"
#include "LoginServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "Enum/Enum.h"
#include "SimLayer.h"
	#include "AccountBlockManager.h"

	#include <boost/random.hpp>


#define CLASS_TYPE KLoginAccountDBThread
ImplPfID( KLoginAccountDBThread, PI_LOGIN_ACCOUNT_DB );

IMPL_PROFILER_DUMP( KLoginAccountDBThread )
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

KLoginAccountDBThread::~KLoginAccountDBThread(void)
{
}

void KLoginAccountDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
	   _CASE( DBE_NEW_USER_JOIN_REQ, KEGS_NEW_USER_JOIN_REQ );
        CASE( ELG_NEXON_USER_AUTHENTICATE_REQ );
		//{{ 2009. 5. 26  최육사	채널이동
		CASE( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ );
		//}}
		CASE( DBE_UPDATE_IS_LOGIN_NOT );
		//{{ 2010. 06. 22  최육사	계정블럭기능
		CASE( DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ );
		//}}
		//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	   _CASE( DBE_REG_REJECTED_USER_NOT, KELG_REG_REJECTED_USER_NOT );
		//}}
	   //{{ 2011. 03. 02	최육사	개인 거래 차단
	   _CASE( DBE_REG_TRADE_BLOCK_USER_NOT, KELG_REG_REJECTED_USER_NOT );
	   //}}
	   //{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	   //}}
	   //{{ 2013. 3. 18	박세훈	 로컬 랭킹 시스템
	   //}}
	default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KLoginAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}

void KLoginAccountDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID )
{
    SendToGSUser( nGS, nTo, usEventID, char() );
}

_IMPL_ON_FUNC( DBE_NEW_USER_JOIN_REQ, KEGS_NEW_USER_JOIN_REQ )
{
    KEGS_NEW_USER_JOIN_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;

    DO_QUERY( L"exec dbo.mup_create_user", L"N\'%s\', N\'%s\', N\'%s\', %d", % kPacket_.m_wstrID % kPacket_.m_wstrPassword % kPacket_.m_wstrName % kPacket_.m_byteGuestUser );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_iOK
            >> kPacket.m_iUserUID );

        m_kODBC.EndFetch();
    }

    if( kPacket.m_iOK != NetError::NET_OK )
    {
        // 사내 계정 생성 실패.
        START_LOG( clog, L"신규 유저 생성 실패." )
            << BUILD_LOG( kPacket.m_iOK )
            << BUILD_LOG( kPacket.m_iUserUID )
            << END_LOG;

        goto end_proc;
    }

end_proc:
    LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
        << BUILD_LOG( kPacket_.m_wstrID )
        << BUILD_LOG( kPacket_.m_wstrPassword )
        << BUILD_LOG( kPacket_.m_wstrName )
        << BUILD_LOG( kPacket.m_iOK )
        << BUILD_LOG( kPacket.m_iUserUID )
        << END_LOG;

    SendToGSUser( LAST_SENDER_UID, FIRST_SENDER_UID, DBE_NEW_USER_JOIN_ACK, kPacket );
}

IMPL_ON_FUNC( ELG_NEXON_USER_AUTHENTICATE_REQ )
{
    KELG_NEXON_USER_AUTHENTICATE_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;
    kPacket.m_kAccountInfo.m_wstrID = kPacket_.m_kAuthenticateReq.m_wstrUserID;
    kPacket.m_kNexonAccountInfo = kPacket_.m_kNexonAccountInfo;
	kPacket.m_cHackingUserType = SEnum::HUT_NORMAL;
	//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
	kPacket.m_bJoinAccount = false;
	//}}

    // 이 쓰레드에서 sim layer 변수를 참조하는 것이 그닥 좋은 것은 아니다.
    DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();

    std::wstring wstrPassword;
    if( !kPacket_.m_kAuthenticateReq.m_bDebugAuth && dwAuthFlag == KSimLayer::AF_NEXON_KOREA )
    {
        wstrPassword = kPacket_.m_kNexonAccountInfo.m_wstrLoginIP;
    }
    else
    {
        wstrPassword = kPacket_.m_kAuthenticateReq.m_wstrPasswd;
    }

    DO_QUERY( L"exec dbo.mup_auth_user", L"%d, N\'%s\', N\'%s\', %d, %d",
		% kPacket_.m_kAuthenticateReq.m_bDebugAuth
		% kPacket_.m_kAuthenticateReq.m_wstrUserID
		% wstrPassword.c_str()
		% kPacket_.m_kNexonAccountInfo.m_uiAge
		% kPacket_.m_kNexonAccountInfo.m_uiNexonSN );

    if( m_kODBC.BeginFetch() )
    {
		//{{ 2011. 05. 02  김민성	2차 보안 시스템
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_kAccountInfo.m_nUserUID
			>> kPacket.m_kAccountInfo.m_wstrName
			>> kPacket.m_kAccountInfo.m_iAuthLevel
			>> kPacket.m_kAccountInfo.m_kAccountOption.m_bPlayGuide
			>> kPacket.m_kAccountInfo.m_bInternalUser
			>> kPacket.m_kAccountInfo.m_kAccountBlockInfo.m_wstrEndTime
			>> kPacket.m_kAccountInfo.m_kAccountBlockInfo.m_wstrBlockReason
			>> kPacket.m_kAccountInfo.m_bIsRecommend
			>> kPacket.m_kAccountInfo.m_wstrRegDate
			>> kPacket.m_kAccountInfo.m_wstrLastLogin
			>> kPacket.m_kNexonAccountInfo.m_uiNexonSN		//SERV_GLOBAL_AUTH //SERV_INT_ONLY
			);
		//}}

        m_kODBC.EndFetch();
    }

    if( kPacket.m_iOK != NetError::NET_OK )
    {
        // 계정 DB 인증 실패
        START_LOG( cwarn, L"존재하지 않거나 삭제/비번 다른 계정." )
            << BUILD_LOG( kPacket.m_iOK )
            << END_LOG;

        switch( kPacket.m_iOK )
        {
        case -1:
            kPacket.m_iOK = NetError::ERR_VERIFY_04;
            break;
        case -2:            // 추후에 계정 삭제가 들어갈 경우 삭제된 유저가 접속했을 때의 처리가 필요하다.
            kPacket.m_iOK = NetError::ERR_VERIFY_05;
            break;
        case -3:
            kPacket.m_iOK = NetError::ERR_VERIFY_06;
            break;
        default:
			START_LOG( cerr, L"쿼리에 정의 되지 않은 오류 입니다!" )
				<< BUILD_LOG( kPacket_.m_kAuthenticateReq.m_bDebugAuth )
				<< BUILD_LOG( kPacket_.m_kAuthenticateReq.m_wstrUserID )
				<< BUILD_LOG( wstrPassword )
				<< BUILD_LOG( kPacket_.m_kNexonAccountInfo.m_uiAge )
				<< BUILD_LOG( kPacket_.m_kNexonAccountInfo.m_uiNexonSN )
				<< END_LOG;

            kPacket.m_iOK = NetError::ERR_UNKNOWN;
            break;
        }

        // 신규 유저 생성. 사내 인증이 아닌 경우에 한다.
        if( kPacket.m_iOK == NetError::ERR_VERIFY_04 && !kPacket_.m_kAuthenticateReq.m_bDebugAuth )
        {
            std::wstring wstrSP;
            switch( dwAuthFlag )
            {
            case KSimLayer::AF_NEXON_KOREA:

				//{{ 2011. 07. 27    김민성    투니랜드 채널링
				{
					int iChannelCode = static_cast< int >(kPacket_.m_kNexonAccountInfo.m_uChannelCode);

					DO_QUERY( L"exec dbo.mup_create_nx_user", L"N\'%s\', N\'%s\', %d, %d, %d, N\'%s\', %d, %d",
						% kPacket_.m_kAuthenticateReq.m_wstrUserID
						% wstrPassword
						% kPacket_.m_kNexonAccountInfo.m_uiNexonSN
						% kPacket_.m_kNexonAccountInfo.m_bSex
						% kPacket_.m_kNexonAccountInfo.m_uiAge
						% kPacket_.m_kAuthenticateReq.m_wstrUserID
						% kPacket_.m_kNexonAccountInfo.m_byteGuestUser
						% iChannelCode														// 채널링 코드 입력
						);
				}
				//}}

                break;
            // 해외 인증 다 해당됨 (해외의 경우 Game Server 접속 시 인증은 계정 생성 하지 않음.)
            default:
				START_LOG( cerr, L"이 인증모드에서는 이미 계정 생성이 되어 있어야 한다." )
					<< BUILD_LOG( dwAuthFlag )
					<< END_LOG;
				goto end_proc;
                break;
            }

            if( m_kODBC.BeginFetch() )
            {
				//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
				FETCH_DATA( kPacket.m_iOK
					>> kPacket.m_kAccountInfo.m_nUserUID
					>> kPacket.m_kAccountInfo.m_kAccountOption.m_bPlayGuide
					>> kPacket.m_kAccountInfo.m_bInternalUser
					>> kPacket.m_kAccountInfo.m_bIsRecommend
					>> kPacket.m_kAccountInfo.m_wstrRegDate
					);
				//}}

				m_kODBC.EndFetch();
            }

            if( kPacket.m_iOK == NetError::NET_OK )
            {
                kPacket.m_kAccountInfo.m_wstrName = kPacket_.m_kAuthenticateReq.m_wstrUserID;
                kPacket.m_kAccountInfo.m_iAuthLevel = 0;
				//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
				kPacket.m_bJoinAccount = true;
				//}}
            }
            else
            {
                // 신규 유저 생성 실패.
                START_LOG( clog, L"신규 유저 생성 실패." )
                    << BUILD_LOG( kPacket.m_iOK )
                    << END_LOG;
            }
        }
    }
	else if( kPacket_.m_bServerUseKogOTP )
	{
		DO_QUERY( L"exec dbo.mup_verify_user_otp", L"%d, N\'%s\'",
			% kPacket.m_kAccountInfo.m_nUserUID
			% wstrPassword );
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
				<< BUILD_LOG( kPacket.m_kAccountInfo.m_nUserUID )
				<< BUILD_LOG( wstrPassword )
				<< END_LOG;
			kPacket.m_iOK = NetError::ERR_UNKNOWN;
			goto end_proc;
		}

		//{{ 2010/10/23	조효진	랜돔 시드 문제 때문에 부스터 Random으로 교체함
		static boost::mt19937 generator(timeGetTime());	// timeGetTime() 빼면 고정 시드
		static boost::uniform_int<> uni_dist(0,INT_MAX) ;
		static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gen(generator, uni_dist);

		int iOTP  = gen();
		//}}

		wchar_t wszOTP[128];
				_itow_s( iOTP, wszOTP, 10 );
		kPacket.m_kAccountInfo.m_wstrOTP = wszOTP;

		DO_QUERY( L"exec dbo.mup_update_user_otp", L"%d, N\'%s\'",
			% kPacket.m_kAccountInfo.m_nUserUID
			% kPacket.m_kAccountInfo.m_wstrOTP );
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

			kPacket.m_iOK = NetError::ERR_UNKNOWN;
			goto end_proc;
		}
	}


	//해킹유저 정보 받아오기.
	if( kPacket.m_iOK == NetError::NET_OK )
	{
		DO_QUERY( L"exec mup_get_hackerlist", L"%d", % kPacket.m_kAccountInfo.m_nUserUID );
		int iOK = 0;

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		switch( iOK )
		{
		//일반유저
		case 0:
			kPacket.m_cHackingUserType = SEnum::HUT_NORMAL;
			break;
		//등록되고 확인한 유저
		case 1:
			kPacket.m_cHackingUserType = SEnum::HUT_AGREE_HACK_USER;
			break;
		//등록되고 확인안한 유저.
		case 2:
			kPacket.m_cHackingUserType = SEnum::HUT_DISAGREE_HACK_USER;
			break;

		default:
			{
				START_LOG( cerr, L"해킹유저 타입이상.!" )
					<< BUILD_LOG( kPacket.m_kAccountInfo.m_nUserUID )
					<< BUILD_LOG( kPacket_.m_kAuthenticateReq.m_wstrUserID )
					<< BUILD_LOG( iOK )
					<< END_LOG;
			}
			break;
		}
	}

	//{{ 2011. 02. 23	최육사	계정 단위 카운트
	// 캐릭터 카운트 정보 얻기!
	DO_QUERY( L"exec dbo.mup_get_UserUID_Count", L"%d", % kPacket.m_kAccountInfo.m_nUserUID );
	while( m_kODBC.Fetch() )
	{
		int iLogType = 0;
		int iCountValue = 0;

		FETCH_DATA( iLogType
			>> iCountValue
			);

		kPacket.m_mapAccCountInfo.insert( std::make_pair( iLogType, iCountValue ) );
	}
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}



end_proc:
    LOG_SUCCESS( kPacket.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( kPacket.m_iOK )
		<< BUILD_LOG( kPacket_.m_kAuthenticateReq.m_bDebugAuth )
		<< BUILD_LOG( kPacket_.m_kAuthenticateReq.m_wstrUserID )
		<< BUILD_LOG( wstrPassword )
		<< BUILD_LOG( kPacket_.m_kNexonAccountInfo.m_uiAge )
		<< BUILD_LOG( kPacket_.m_kNexonAccountInfo.m_uiNexonSN );

    SendToGSUser( LAST_SENDER_UID, FIRST_SENDER_UID, ELG_NEXON_USER_AUTHENTICATE_ACK, kPacket );
}

//{{ 2009. 5. 26  최육사	채널이동
IMPL_ON_FUNC( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ )
{
	KELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_ACK kPacket;
	kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrID = kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_wstrUserID;
	kPacket.m_kUserAuthAck.m_kNexonAccountInfo = kPacket_.m_kUserAuthReq.m_kNexonAccountInfo;
	kPacket.m_kUserAuthAck.m_cHackingUserType = SEnum::HUT_NORMAL;
	kPacket.m_kConnectAck = kPacket_.m_kConnectAck;			// 채널이동을 위한 정보
	kPacket.m_kSelectUnitReq = kPacket_.m_kSelectUnitReq;	// 채널이동을 위한 정보
	kPacket.m_kChangeUserInfo = kPacket_.m_kChangeUserInfo; // 채널이동을 위한 정보

    // 이 쓰레드에서 sim layer 변수를 참조하는 것이 그닥 좋은 것은 아니다.
    DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();

    std::wstring wstrPassword;
    if( !kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_bDebugAuth && dwAuthFlag == KSimLayer::AF_NEXON_KOREA )
    {
        wstrPassword = kPacket_.m_kUserAuthReq.m_kNexonAccountInfo.m_wstrLoginIP;
    }
    else
    {
        wstrPassword = kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_wstrPasswd;
    }

	DO_QUERY( L"exec dbo.mup_auth_user", L"%d, N\'%s\', N\'%s\', %d, %d", % kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_bDebugAuth % kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_wstrUserID % wstrPassword.c_str() % kPacket_.m_kUserAuthReq.m_kNexonAccountInfo.m_uiAge % kPacket_.m_kUserAuthReq.m_kNexonAccountInfo.m_uiNexonSN );

	if( m_kODBC.BeginFetch() )
	{
		//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
		FETCH_DATA( kPacket.m_kUserAuthAck.m_iOK
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrName
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_iAuthLevel
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_kAccountOption.m_bPlayGuide
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_bInternalUser
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_kAccountBlockInfo.m_wstrEndTime
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_kAccountBlockInfo.m_wstrBlockReason
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_bIsRecommend
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrRegDate
			>> kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrLastLogin
			>> kPacket.m_kUserAuthAck.m_kNexonAccountInfo.m_uiNexonSN		// SERV_GLOBAL_AUTH
			);
		//}}

		m_kODBC.EndFetch();
	}

	if( kPacket.m_kUserAuthAck.m_iOK != NetError::NET_OK )
	{
		// 계정 DB 인증 실패
		START_LOG( clog, L"존재하지 않거나 삭제/비번 다른 계정." )
			<< BUILD_LOG( kPacket.m_kUserAuthAck.m_iOK )
			<< END_LOG;

		switch( kPacket.m_kUserAuthAck.m_iOK )
		{
		case -1:
			kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_VERIFY_04;
			break;
		case -2:            // 추후에 계정 삭제가 들어갈 경우 삭제된 유저가 접속했을 때의 처리가 필요하다.
			kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_VERIFY_05;
			break;
		case -3:
			kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_VERIFY_06;
			break;
		default:
			kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_UNKNOWN;
			break;
		}

		// 채널이동 실패처리 합니다.
	}
	else if( kPacket_.m_bServerUseKogOTP )
    {
		DO_QUERY( L"exec dbo.mup_verify_user_otp", L"%d, N\'%s\'",
            % kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID
            % wstrPassword );
        if( m_kODBC.BeginFetch() )
        {
            FETCH_DATA( kPacket.m_kUserAuthAck.m_iOK
                );
            m_kODBC.EndFetch();
        }

		if( kPacket.m_kUserAuthAck.m_iOK != NetError::NET_OK )
		{
			// OTP 검증 실패
			START_LOG( clog, L"OTP 검증 실패." )
				<< BUILD_LOG( kPacket.m_kUserAuthAck.m_iOK )
				<< BUILD_LOG( kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID )
				<< BUILD_LOG( wstrPassword )
				<< END_LOG;

			kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_UNKNOWN;
			goto end_proc;
		}

        if( kPacket.m_kUserAuthAck.m_iOK == NetError::NET_OK )
        {
			//{{ 2011/1/6	조효진	랜돔 시드 문제 때문에 부스터 Random으로 교체함
			static boost::mt19937 generator(timeGetTime());	// timeGetTime() 빼면 고정 시드
			static boost::uniform_int<> uni_dist(0,INT_MAX) ;
			static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gen(generator, uni_dist);

			int iOTP  = gen();
            wchar_t wszOTP[128];
			_itow_s( iOTP, wszOTP, 10 );
            kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrOTP = wszOTP;

			DO_QUERY( L"exec dbo.mup_update_user_otp", L"%d, N\'%s\'",
                % kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID
                % kPacket.m_kUserAuthAck.m_kAccountInfo.m_wstrOTP );
            if( m_kODBC.BeginFetch() )
            {
                FETCH_DATA( kPacket.m_kUserAuthAck.m_iOK
                    );
                m_kODBC.EndFetch();
            }

            if( kPacket.m_kUserAuthAck.m_iOK != NetError::NET_OK )
            {
                // OTP 업데이트 실패
                START_LOG( clog, L"OTP 업데이트 실패." )
                    << BUILD_LOG( kPacket.m_kUserAuthAck.m_iOK )
                    << END_LOG;

				kPacket.m_kUserAuthAck.m_iOK = NetError::ERR_UNKNOWN;
				goto end_proc;
            }
        }
    }


	//해킹유저 정보 받아오기.
	if( kPacket.m_kUserAuthAck.m_iOK == NetError::NET_OK )
	{
		DO_QUERY( L"exec mup_get_hackerlist", L"%d", % kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID );
		int iOK = 0;

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		switch( iOK )
		{
			//일반유저
		case 0:
			kPacket.m_kUserAuthAck.m_cHackingUserType = SEnum::HUT_NORMAL;
			break;
			//등록되고 확인한 유저
		case 1:
			kPacket.m_kUserAuthAck.m_cHackingUserType = SEnum::HUT_AGREE_HACK_USER;
			break;
			//등록되고 확인안한 유저.
		case 2:
			kPacket.m_kUserAuthAck.m_cHackingUserType = SEnum::HUT_DISAGREE_HACK_USER;
			break;

		default:
			{
				START_LOG( cerr, L"해킹유저 타입이상.!" )
					<< BUILD_LOG( kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID )
					<< BUILD_LOG( kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_wstrUserID )
					<< BUILD_LOG( iOK )
					<< END_LOG;
			}
			break;
		}
	}

	//{{ 2011. 02. 23	최육사	계정 단위 카운트
	// 캐릭터 카운트 정보 얻기!
	DO_QUERY( L"exec dbo.mup_get_UserUID_Count", L"%d", % kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID );
	while( m_kODBC.Fetch() )
	{
		int iLogType = 0;
		int iCountValue = 0;

		FETCH_DATA( iLogType
			>> iCountValue
			);

		kPacket.m_kUserAuthAck.m_mapAccCountInfo.insert( std::make_pair( iLogType, iCountValue ) );
	}
	//}}

	//{{ 2012. 02. 21	김민성	2차 보안 채널 이동시(미니맵) 오류 수정
	// 2차 비번 얻기 : kPacket_ --> UserUID
	DO_QUERY( L"exec dbo.mup_get_second_pw_Info", L"%d", % kPacket.m_kUserAuthAck.m_kAccountInfo.m_nUserUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_bUseSecondPW
				 >> kPacket.m_wstrSecondPW
				 >> kPacket.m_iFailedCount
				 >> kPacket.m_wstrLastAuthDate );

		m_kODBC.EndFetch();
	}
	//}}


end_proc:
	LOG_SUCCESS( kPacket.m_kUserAuthAck.m_iOK == NetError::NET_OK )
		<< BUILD_LOG( kPacket.m_kUserAuthAck.m_iOK )
		<< BUILD_LOG( kPacket_.m_kUserAuthReq.m_kAuthenticateReq.m_wstrUserID );

	SendToGSUser( LAST_SENDER_UID, FIRST_SENDER_UID, ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_ACK, kPacket );
}
//}}

IMPL_ON_FUNC( DBE_UPDATE_IS_LOGIN_NOT )
{
	DO_QUERY( L"exec dbo.mup_update_islogin", L"%d, %d", % kPacket_.m_iUserUID % kPacket_.m_bIsLogin );
	int iOK = NetError::NET_OK;

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	//{{ 2011. 02. 23	최육사	계정 단위 카운트
	{
		std::map< int, int >::const_iterator mitCGC;
		for( mitCGC = kPacket_.m_mapAccCountInfo.begin(); mitCGC != kPacket_.m_mapAccCountInfo.end(); ++mitCGC )
		{
			DO_QUERY( L"exec dbo.mup_update_UserUID_Count", L"%d, %d, %d",
				% kPacket_.m_iUserUID
				% mitCGC->first
				% mitCGC->second
				);
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}
		}
	}
	//}}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( L"Guest User 로그인 정보 갱신.!" )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< BUILD_LOG( kPacket_.m_bIsLogin )
		;
}

//{{ 2010. 06. 22  최육사	계정블럭기능

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
			case KAccountBlockManager::BT_CONNECT_BLOCK: // 접속 차단 리스트
				{
					std::set< UidType > setAccountBlockList;

					// 계정 블럭 리스트 얻기
					DO_QUERY_NO_ARG( L"exec dbo.mup_get_rejected_user" );
					while( m_kODBC.Fetch() )
					{
						UidType iUserUID = 0;

						FETCH_DATA( iUserUID );

						setAccountBlockList.insert( iUserUID );
					}

					kPacket.m_mapAccountBlockList.insert( std::make_pair( mit->first, setAccountBlockList ) );
				}
				break;

			case KAccountBlockManager::BT_TRADE_BLOCK: // 개인 거래 차단 리스트
				{
					std::set< UidType > setTradeBlockList;
					// 거래 블럭 리스트 얻기
					DO_QUERY_NO_ARG( L"exec dbo.mup_get_TradeBlock_user" );

					while( m_kODBC.Fetch() )
					{
						UidType iUserUID = 0;

						FETCH_DATA( iUserUID );

						setTradeBlockList.insert( iUserUID );
					}

					kPacket.m_mapAccountBlockList.insert( std::make_pair( mit->first, setTradeBlockList ) );
				}
				break;

				//{{ 2011. 08. 09	최육사	머신ID 블럭기능
			case KAccountBlockManager::BT_MACHINE_ID_BLOCK:
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
				//}}

			default:
				break;
			}
		}
	}

end_proc:
	SendToServer( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK, kPacket );
}

//}}

//{{ 2010. 10. 05	최육사	센터서버 해킹체크

_IMPL_ON_FUNC( DBE_REG_REJECTED_USER_NOT, KELG_REG_REJECTED_USER_NOT )
{
	// DB에 Rejected User 등록!
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.mup_insert_rejected_user", L"%d, %d", % kPacket_.m_iUserUID % (int)kPacket_.m_cRejectedReason );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Rejected User 등록 실패.!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOGc( kPacket_.m_cRejectedReason )
			<< END_LOG;
	}
}

//}}

//{{ 2011. 03. 02	최육사	개인 거래 차단
_IMPL_ON_FUNC( DBE_REG_TRADE_BLOCK_USER_NOT, KELG_REG_REJECTED_USER_NOT )
{
	// DB에 Trade Block User 등록!
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.mup_insert_TradeBlock_user", L"%d, %d", % kPacket_.m_iUserUID % (int)kPacket_.m_cRejectedReason );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Trade Block User 등록 실패.!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOGc( kPacket_.m_cRejectedReason )
			<< END_LOG;
	}
}
//}}

//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
//}}

//{{ 2013. 3. 18	박세훈	 로컬 랭킹 시스템
//}}
