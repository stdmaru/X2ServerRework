#include "x2/platform/format.h"
#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"
#include "GSSimLayer.h"

#include "BaseServer.h"
#include "UserManager.h"
//{{ 2009. 7. 16  최육사	채널매니저
#include "ChannelManager.h"
//}}

#include "NetLayer.h"
#include "Socket/NetCommon.h"
#include "ODBC/Odbc.h"
#include "Enum/Enum.h"

//{{ 2011. 05. 02  김민성	2차 보안 시스템
	#include "RewardTable.h"
//}}

//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//}}


//{{ 2011. 08. 10    김민성    채널링 SOAP
//}}

//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "GameSysVal/GameSysVal.h"
//}}

//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
//}}

//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
	#include "X2Data/XSLSquareUnit.h"
//}}


//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	#include "SMSPhoneNumberManager.h"
//}}

	#include "X2Data/XSLBattleFieldManager.h"

//////////////////////////////////////////////////////////////////////////
//#ifdef SERV_GSUSER_CPP
//#pragma NOTE( "GSUserHandler.cpp 파일 컴파일 됩니당!" )
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

KMaxProcessTime KGSUser::ms_kMaxLoginTime( 3 );

//{{ 2012. 02. 27	김민성	인벤토리 정렬	- 무거운 기능을 가볍게 변경
KMaxProcessTime KGSUser::ms_kMaxInventorySortTime( 3 );
//}}

IMPL_ON_FUNC( DBE_UPDATE_UNIT_INFO_ACK )
{
	// 상태를 막아야 하나?

	//////////////////////////////////////////////////////////////////////////
	// DB업데이트 실패에 대한 롤백처리

	m_kEXP.AdjustInitExp( m_kEXP.GetInitExp() - kPacket_.m_iEXP );
	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	m_iED.AdjustInitValue( m_iED.GetInitValue() - kPacket_.m_iED );
	//}}
	//{{ 2011. 06. 20	최육사	대전 개편
	m_kUserPvpManager.CheckRollbackInfo(  kPacket_.m_iOfficialMatchCnt
										, kPacket_.m_iRating
										, kPacket_.m_iMaxRating
										, kPacket_.m_iRPoint
										, kPacket_.m_iAPoint
										, kPacket_.m_iWin
										, kPacket_.m_iLose

										//{{ 2012. 06. 22	박세훈	2012 대전 시즌2
										, kPacket_.m_fKFactor
										//}}
										);
	//}}

	std::vector< int >::const_iterator vitDungeonClear;
	std::map< int, KDungeonClearInfo >::iterator mitDungeonClear;

	for( vitDungeonClear = kPacket_.m_vecDungeonClear.begin(); vitDungeonClear != kPacket_.m_vecDungeonClear.end(); ++vitDungeonClear )
	{
		mitDungeonClear = m_mapDungeonClear.find( *vitDungeonClear );
		if( mitDungeonClear != m_mapDungeonClear.end() )
		{
			mitDungeonClear->second.m_bNew = true;
		}
		else
		{
			START_LOG( cerr, L"Dungeon clear information is expected but missing." )
				<< BUILD_LOG( *vitDungeonClear )
				<< END_LOG;
		}
	}

	std::vector< int >::const_iterator vitTCClear;
	std::map< int, KTCClearInfo >::iterator mitTCClear;
	for( vitTCClear = kPacket_.m_vecTCClear.begin(); vitTCClear != kPacket_.m_vecTCClear.end(); ++vitTCClear )
	{
		mitTCClear = m_mapTCClear.find( *vitTCClear );
		if( mitTCClear != m_mapTCClear.end() )
		{
			mitTCClear->second.m_bNew = true;
		}
		else
		{
			START_LOG( cerr, L"Training room clear information is expected but missing." )
				<< BUILD_LOG( *vitTCClear )
				<< END_LOG;
		}
	}

	m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
	m_kInventory.RollBackInitEndurance( kPacket_.m_kItemEnduranceUpdate );
	m_kInventory.PositionUpdateComplete( kPacket_.m_kItemPositionUpdate );
	m_iNumResurrectionStone.AdjustInitValue( m_iNumResurrectionStone.GetInitValue() - kPacket_.m_iNumResurrectionStone );
	//{{ 2009. 10. 14  최육사	자동결제 부활석
	m_iNumAutoPaymentResStone.AdjustInitValue( m_iNumAutoPaymentResStone.GetInitValue() - kPacket_.m_iNumAutoPaymentResStone );
	//}}

	//{{ 2010. 8. 10	최육사	펫 시스템
	m_kUserPetManager.RollBackDBUpdatePetInfo( kPacket_.m_vecPet );
	//}}

	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}

	m_kUserRidingPetManager.DBUpdateFailedProcess( kPacket_.m_vecRidingPetUID );

	//////////////////////////////////////////////////////////////////////////
	// 후처리

	switch( kPacket_.m_iEventID )
	{
	case 0: break;
	case EGS_STATE_CHANGE_SERVER_SELECT_REQ:
		{
			KEGS_STATE_CHANGE_SERVER_SELECT_ACK kPacket;
			kPacket.m_iOK = NetError::NET_OK;
			SendPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK, kPacket );

			// 유닛리스트에서 삭제
			GetKUserManager()->UnregCharInfo( *this );

			// DB업데이트 했으니 유닛정보 초기화
			ResetUnitInfo();

			StateTransition( KGSFSM::I_TO_SERVER_SELECT );
		}
		break;

	case EGS_DISCONNECT_FOR_SERVER_SELECT_REQ:
		{
			// 유닛리스트에서 삭제
			GetKUserManager()->UnregCharInfo( *this );

			// DB업데이트 했으니 유닛정보 초기화
			ResetUnitInfo();

			// 로그인 서버로 유저정보 삭제하러 가자
			SendDisconnectEvent( EGS_DISCONNECT_FOR_SERVER_SELECT_REQ );
		}
		break;

	case EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ:
		{
			// 유닛리스트에서 삭제
			GetKUserManager()->UnregCharInfo( *this );

			// DB업데이트 했으니 유닛정보 초기화
			ResetUnitInfo();

			// 로그인 서버로 유저정보 삭제하러 가자
			SendDisconnectEvent( EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ );
		}
		break;
		//{{ 2012. 05. 08	김민성       개인 거래 로직 변경
	case ERM_APPROVE_TRADE_REQ:
		{
			_SendToCnRoom( GetTradeUID(), ERM_APPROVE_TRADE_REQ, char() );
		}
		break;
		//}}
		//{{ 2012. 05. 29	김민성       개인 상점 거래 로직 변경
	case EGS_BUY_PERSONAL_SHOP_ITEM_REQ:
		{
			KERM_BUY_PERSONAL_SHOP_ITEM_REQ kPacket;
			GetPersonalShopBuyItemInfo( kPacket );
			_SendToPShopRoom( GetPersonalShopUID(), ERM_BUY_PERSONAL_SHOP_ITEM_REQ, kPacket );
		}
		break;
		//}}
		//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
	case EGS_CREATE_PSHOP_AGENCY_REQ:
		{
			// 대리 상인 개설하러 갑시당!
			KERM_OPEN_PSHOP_AGENCY_REQ kPacket;
			kPacket.m_iUserUID = GetUID();
			kPacket.m_iED = GetED();
			kPacket.m_wstrNickName = GetCharName();
			kPacket.m_wstrAgencyExpirationDate = m_kUserPersonalShopManager.GetAgencyExpirationDate();
            kPacket.m_cPersonalShopType = m_kUserPersonalShopManager.GetPShopType();
			SendToLoginRoom( ERM_OPEN_PSHOP_AGENCY_REQ, kPacket );
		}break;
	case EGS_PICK_UP_FROM_PSHOP_AGENCY_REQ:
		{
			KEGS_PICK_UP_FROM_PSHOP_AGENCY_REQ kPacket;
			GetAgencyShopPickUpItemInfo( kPacket.m_vecPickUpItemList );
			_SendToLoginRoom( GetPShopAgencyUID(), ERM_PICK_UP_FROM_PSHOP_AGENCY_REQ, kPacket );
		}
		break;
		//}}
	default:
		START_LOG( cerr, L"Received an undefined event." )
			<< BUILD_LOG( kPacket_.m_iEventID )
			<< BUILD_LOG( KEvent::GetIDStr( static_cast<u_short>(kPacket_.m_iEventID) ) )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( EGS_CONNECT_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_INIT ) );

	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( kPacket_.m_wstrVersion );

	KEGS_CONNECT_ACK kPacket;

	// 최초 게임서버 접속시 버전 비교
	if( OnConnectReq( kPacket_, kPacket ) == false )
	{
		START_LOG( cwarn, L"connect error" )
			<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
			<< END_LOG;
	}

	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}

	SendPacket( EGS_CONNECT_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_VERIFY_ACCOUNT_REQ )
{
	//{{ 2010. 9. 2	최육사	중폭 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_INIT ), EGS_VERIFY_ACCOUNT_REQ, EGS_VERIFY_ACCOUNT_ACK );
	//}}


	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( kPacket_.m_bDebugAuth )
		<< BUILD_LOG( kPacket_.m_wstrUserID )
		<< BUILD_LOG( kPacket_.m_wstrPasswd );

	//kPacket_.m_bDebugAuth = false;
	//kPacket_.m_wstrPasswd = L"NGP10:trZUblcTlLAQ3zLkq7XCTTbK7YuMAoeHpFbnV8bTg0B3z6Y4e7Zogt8KTFFE0xvN9EnwoFBcxaSHAPxugwHeR~yMccp8NRozrgqagLsa_x3sNE7fmR8pmggGMKk8_0linaFwxVdiYKIxCk0jn4Th2uAVVrK2bP10u_EcLI_pMOGcgEZp4~DEo3mr8R98tTacbvfELPYKpvZAgwTqm0qZkMiKFA3tUPCGe7~sYzl4VrE1Jo1i";

	kPacket_.m_bDebugAuth = false; // 해외팀에서는 bDebugAuth를 이용한 예외상황 고려안함

	SetSteamClientUser( kPacket_.m_bSteamClient );
	START_LOG( clog, L"[TEST] Registered as a Steam client user.")
		<< BUILD_LOG( IsSteamClientUser())
		<< END_LOG;

	SetAeriaClientUser( kPacket_.m_bAeriaClient );
	START_LOG( clog, L"[TEST] Registered as an Aria client user.")
		<< BUILD_LOG( IsAeriaClientUser())
		<< END_LOG;

	KELG_NEXON_USER_AUTHENTICATE_REQ kPacket;
	kPacket.m_kAuthenticateReq = kPacket_;
	kPacket.m_kNexonAccountInfo.m_wstrLoginIP = KncUtil::toWideString( KSession::GetIPStr() );

	kPacket.m_kNexonAccountInfo.m_uChannelCode = static_cast<UINT8>(kPacket.m_kAuthenticateReq.m_iChannelingCode);

	START_LOG( clog, L"[TEST] Displaying the channeling code.")
		<< BUILD_LOG( kPacket_.m_iChannelingCode )
		<< END_LOG;


	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrUserID ) )
	{
		KEGS_VERIFY_ACCOUNT_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );
		return;
	}

	if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
	{
		if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrPasswd ) )
		{
			KEGS_VERIFY_ACCOUNT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_ODBC_00;
			SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );
			return;
		}
	}

	//{{ 2012. 03. 27	김민성		머신 시간을 검사하자(50분 이상 차이나면 모니터링)
	//}}

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	if( KSimLayer::GetKObj()->GetVersionFlag() != KSimLayer::VF_OPEN_TEST )		// 오픈 테섭에서는 검사 제외
	{
		if( SiKGameSysVal()->IsMachineIDDuplicateCheck() == true )
		{
			if( m_kUserSecurityManager.CheckValidMachineID( kPacket_.m_strMachineID ) == false )
			{
				START_LOG( cout, L"[Notice] Invalid machine ID; sending a notification packet to the client!" )
					<< BUILD_LOG( kPacket_.m_wstrHWID )
					<< BUILD_LOG( kPacket_.m_wstrUserID )
					<< BUILD_LOG( kPacket_.m_wstrPasswd )
					<< BUILD_LOG( kPacket_.m_bDebugAuth );

				//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
				KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
				kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_MACHINE_ID;
				SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
				//}}

				//{{ 2011. 09. 06	최육사	머신 ID 기능 업데이트
				// 머신ID가 비어있으면 접속 차단!
				Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
				//}}
			}
		}
	}

	// 머신ID 저장!
	m_kUserSecurityManager.SetMachineID( kPacket_.m_strMachineID );
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	SendToLoginServer( ELG_NEXON_USER_AUTHENTICATE_REQ, kPacket );

	m_kTimer[TM_INIT].restart();
	//}}
}

//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
//}}

IMPL_ON_FUNC( ELG_NEXON_USER_AUTHENTICATE_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_INIT ) );

	KEGS_VERIFY_ACCOUNT_ACK kPacket;

	//{{ 2012. 06. 28	김민성       통합 멤버십 ATL 1.0 -> 2.0 변경
	if( kPacket_.m_iOK == NetError::ERR_VERIFY_24 )
	{
		// 인증은 성공 했지만 ATL 2.0 이 아니라면
		START_LOG( cerr, L"Integrated membership ATL tier check result." )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;

		kPacket.m_iOK = kPacket_.m_iOK;

		SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );
		return;
	}
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}


	// 넥슨 인증 결과 처리
	if( OnNexonUserAuthenticateAck( kPacket_, kPacket ) == false )
	{
		START_LOG( cerr, L"Nexon authentication failed!" )
			<< BUILD_LOG( kPacket_.m_kAccountInfo.m_wstrID )
			<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
			<< END_LOG;

		SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );
		return;
	}


	// 로그인 서버에 유저정보 등록하기
	KELG_REGISTER_USER_REQ kPacketReq;
	kPacketReq.m_wstrUserID = GetName();
	kPacketReq.m_iChannelID = SiKChannelManager()->GetChannelID();
	kPacketReq.m_uiIP		= GetIP();
	kPacketReq.m_byteCanTry = 0;
	kPacketReq.m_byteGuestUser = m_kNexonAccountInfo.m_byteGuestUser;
	kPacketReq.m_cAuthLevel = static_cast<char>(GetAuthLevel());
	//{{ 2011. 07. 27    김민성    투니랜드 채널링
	kPacketReq.m_ucChannelCode = kPacket_.m_kNexonAccountInfo.m_uChannelCode;
	if( GetAuthLevel() >= SEnum::UAL_DEVELOPER )
	{
		START_LOG( cout, L"[Check] Verifying the channeling code." )
			<< BUILD_LOG( kPacket_.m_kNexonAccountInfo.m_uChannelCode )
			<< BUILD_LOG( m_kNexonAccountInfo.m_uChannelCode )
			<< BUILD_LOG( kPacketReq.m_ucChannelCode );
	}
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	kPacketReq.m_strMachineID = m_kUserSecurityManager.GetStringMachineID();
	//}}



	SendToLoginServer( ELG_REGISTER_USER_REQ, kPacketReq );

	//{{ 2012. 03. 29	김민성		중복 sn 접속 감시
	//}}


	// 클라이언트로 결과 통보
	kPacket.m_iOK = kPacket_.m_iOK;
	kPacket.m_kAccountInfo = kPacket_.m_kAccountInfo;
	kPacket.m_kAccountInfo.m_bIsGuestUser = IsGuestUser(); // 체험아이디 정보
	//{{ 2010. 01. 11	김민성	ip추가
	kPacket.m_wstrClientIP = KncUtil::toWideString( GetIPStr() );
		//}}

	//{{ 2011. 07. 27    김민성    투니랜드 채널링
	kPacket.m_uChannelCode = kPacket_.m_kNexonAccountInfo.m_uChannelCode;
	kPacket.m_wstrChannelUserID = kPacket_.m_kNexonAccountInfo.m_wstrChannelUID;
	//}}

	//{{ 2011. 08. 10    김민성    채널링 SOAP
	//}}

	//{{ 2011. 08. 09  김민성 (2011.08.11) 특정일 이후 생성한 계정에 대하여 신규케릭터 생성 시 아이템 지급 이벤트
	SetAccountInfo( kPacket_.m_kAccountInfo );
		//}}
	SendPacket( EGS_VERIFY_ACCOUNT_ACK, kPacket );

	StateTransition( KGSFSM::I_TO_UID_UPDATED );
	ms_kMaxLoginTime.CheckMaxProcessTime( GetName(), ( float )m_kTimer[TM_INIT].elapsed() );

	//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
	if( kPacket_.m_bJoinAccount == true )
	{
		if( SiKAbuserLogManager()->JoinAccountCheckIP( kPacket_.m_kNexonAccountInfo.m_wstrLoginIP ) == true )
		{
			KDBE_JOIN_ACCOUNT_CHECK_IP_LOG_NOT kPacketToLog;
			kPacketToLog.m_iUserUID = GetUID();
			kPacketToLog.m_wstrUserID = GetName();
			kPacketToLog.m_wstrLoginIP = kPacket_.m_kNexonAccountInfo.m_wstrLoginIP;
			SendToAccountDB( DBE_JOIN_ACCOUNT_CHECK_IP_LOG_NOT, kPacketToLog );
		}
	}
	//}}

	//080624.hoons.인증패킷 처리를 다해주어야 클라이언트에서 nProtect가 동작함.(해킹유저만)
	//if( m_cHackingUserType == SEnum::HUT_AGREE_HACK_USER )
	//{
	//	// nProtect 한국인증 모듈 사용설정
	//	if( KSimLayer::GetKObj()->CheckCommonFlag( KSimLayer::CommonFlag::CF_NPGG ) )
	//	{
	//		m_kGGAuth.SetEnable( true );
	//	}
	//	m_kGGAuth.Init();
	//}

	//korea nProtect 초기 인증을 클라이언트에 보낸다.
	//if( m_kGGAuth.FirstAuthQuery( GetThisPtr<KGSUser>() ) == false )
	//{
	//	SetDisconnectReason( KStatistics::eSIColDR_NProtect_Auth_First );
	//	ReserveDestroy();

	//	START_LOG( cerr, L"nProtect 첫인증 실패.!" )
	//		<< BUILD_LOG( GetUID() )
	//		<< BUILD_LOG( GetName() )
	//		<< END_LOG;
	//	return;
	//}

	//{{ 2010. 8. 11  최육사	개발자도 핵쉴드 체크 하도록 수정
	//if( GetAuthLevel() == SEnum::UAL_DEVELOPER )
	//{
	//	// 개발자는 HackShield끕니다.
	//	m_kHSAuth.SetEnable( false );
	//}

	// Port: HackShield's first check removed with the anti-cheat (T10)
	//}}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	m_kUserAbuserManager.InitAccountAbuser( GetThisPtr<KGSUser>() );
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템

	// 오늘 6 이후 최초 접속이면
	CTime tLastLogin;
	CTime tNow = CTime::GetCurrentTime();
	CTime tRestart = CTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), 6, 0, 0 );		// 초기화 기점
	CTimeSpan tOneDay = CTimeSpan( 1, 0, 0, 0 );
	CTime tYesterRestart = tRestart - tOneDay;		// 이전 초기화 기점

	if( KncUtil::ConvertStringToCTime( kPacket_.m_kAccountInfo.m_wstrLastLogin, tLastLogin ) == false )
	{
		START_LOG( cerr, L"Failed to convert the time to a string!!!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacket_.m_kAccountInfo.m_wstrLastLogin )
			<< END_LOG;
		return;
	}

	if( GetIsUseOTP() == true ) // OTP 사용 유저인가?
	{
		if( tLastLogin >= tYesterRestart )	// 어제 받았다
		{
			if( tLastLogin < tRestart )	// 오늘 받을 조건이 된다
			{
				if( tRestart <= tNow )
				{
					m_kUserUnitSelectRewardManager.AddRewardItemInfo( KRewardTable::ERI_SECOND_SECURITY );
				}
			}
		}
		else
		{
			m_kUserUnitSelectRewardManager.AddRewardItemInfo( KRewardTable::ERI_SECOND_SECURITY );
		}
	}

	//}}

	//{{ 2012. 05. 17	박세훈	키보드 맵핑 정보를 DB에서 읽고 쓰기
	SendToAccountDB( DBE_KEYBOARD_MAPPING_INFO_READ_REQ, GetUID() );
	//}}

	//{{ 2012. 05. 29	박세훈	채팅 옵션 정보를 DB에서 읽고 쓰기
	SendToAccountDB( DBE_CHAT_OPTION_INFO_READ_REQ, GetUID() );
	//}}
}

IMPL_ON_FUNC( ELG_REGISTER_USER_ACK )
{
	// 게임서버 접속 시 퍼블인증이 없는 경우에만 타는 Flow 임
	// 게임서버 접속 시 퍼블인증이 없다는 것은 PC방 인증을 우리(KOG)쪽에서 처리한다는 의미임

	VERIFY_STATE( ( 1, KGSFSM::S_UID_UPDATED ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		Kick( kPacket_.m_iOK );
		return;
	}

	if( GetStateID() != KGSFSM::S_SERVER_SELECT )
	{
		StateTransition( KGSFSM::I_TO_LOGINED );
	}

	m_bLoginFail = false;

	{
		// 해당 플로우는 게임 서버 로그인 시 퍼블 인증을 받지 않는 경우에만 탄다.
		// 즉 PC방 관리를 우리가 하기 때문에 위 SERV_KOG_HAVE_PCBANG_IP_LIST 조건이 아니라면
		// 다 PC방이 아님
		m_kUserPcBangManager.SetIsPcBang( false );
		m_kUserPcBangManager.SetPcBangType(-1); // PC방 아닌경우 -1값 0도 피씨방임


	}


	KENX_USER_LOGIN_NOT kNot;
	kNot.m_bIsGameBang = IsPcBang();
	kNot.m_kPcBangReward = GetPcBangReward();
	SendPacket( ENX_USER_LOGIN_NOT, kNot );

	// last login
	SendToAccountDB( DBE_UPDATE_LOGIN_TIME_NOT );

	// 모든 로그인 절차가 끝나고 GSUser의 State가 S_LOGINED로 바뀐후에 장바구니를 받아오자!
	KDBE_GET_WISH_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUserUID = GetUID();
	SendToGameDB( DBE_GET_WISH_LIST_REQ, kPacketToDB );
}


IMPL_ON_FUNC( ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK )
{
	// 채널선택 접속이든 채널이동 접속이든 해외는 둘다 여기 코드 탐

	//////////////////////////////////////////////////////////////////////////
	// 상태 검사
	switch( GetStateID() )
	{
	case KGSFSM::S_UID_UPDATED:
	case KGSFSM::S_LOGINED:
	case KGSFSM::S_SERVER_SELECT:
	case KGSFSM::S_FIELD_MAP:
	case KGSFSM::S_ROOM:
		// 정상적으로 채널이동 중 PC방 인증 완료
		break;

	default:
		{
			START_LOG( cerr, L"Unexpected state. Ignoring login processing." )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;
		}
		return;
	}

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		Kick( kPacket_.m_iOK );
		return;
	}

	// 채널이동의 경우 KGSFSM::S_FIELD_MAP상태다
	if( GetStateID() != KGSFSM::S_SERVER_SELECT  &&
		GetStateID() != KGSFSM::S_FIELD_MAP  &&
		GetStateID() != KGSFSM::S_ROOM )
	{
		StateTransition( KGSFSM::I_TO_LOGINED );
	}


	//PC방 인증
	{
//////////////////////////////////////////////////////////////////////////
		m_kUserPcBangManager.SetIsPcBang( false );
		m_kUserPcBangManager.SetPcBangType(-1);	// PC방 아닌경우 -1값 0도 피씨방임
		START_LOG( clog2, L"Not a PC Bang user... publisher check" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( KncUtil::toWideString( KSession::GetIPStr() ) );
	}



	// 채널이동의 경우 KGSFSM::S_FIELD_MAP상태다
	if( GetStateID() != KGSFSM::S_FIELD_MAP  &&  GetStateID() != KGSFSM::S_SERVER_SELECT )
	{
		StateTransition( KGSFSM::I_TO_LOGINED );
	}
	m_bLoginFail = false;


	KENX_USER_LOGIN_NOT kNot;
	kNot.m_bIsGameBang = IsPcBang();
	kNot.m_kPcBangReward = GetPcBangReward();
	SendPacket( ENX_USER_LOGIN_NOT, kNot );

	// last login
	SendToAccountDB( DBE_UPDATE_LOGIN_TIME_NOT );

	// 모든 로그인 절차가 끝나고 GSUser의 State가 S_LOGINED로 바뀐후에 장바구니를 받아오자!
	KDBE_GET_WISH_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUserUID = GetUID();
	SendToGameDB( DBE_GET_WISH_LIST_REQ, kPacketToDB );
}

//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

_IMPL_ON_FUNC( ELG_REGISTER_NEXON_USER_ACK, KENX_AUTH_LOGIN_ACK )
{
	//////////////////////////////////////////////////////////////////////////
	// 상태 검사
	switch( GetStateID() )
	{
	case KGSFSM::S_UID_UPDATED:
		// TIME OUT 되지 않고 정상적으로 PC방 인증 완료
		break;

	case KGSFSM::S_LOGINED:
	case KGSFSM::S_SERVER_SELECT:
	case KGSFSM::S_FIELD_MAP:
	case KGSFSM::S_ROOM:
		// 넥슨 PC방 인증 로그인 처리가 TIME OUT되었음!
		{
			// 만약 PC방 프리미엄을 받을 수 있는 유저라면?
			if( kPacket_.m_byteAuthorizeResult == 1 )
			{
				START_LOG( cout, L"PC Bang premium canceled because authentication timed out!" )
					<< BUILD_LOG( kPacket_.m_wstrUserID );

				// 인증 타임 아웃으로 인해 PC방 프리미엄 취소하고 일반 유저로 로그인 재 요청!
				KELG_NEXON_USER_LOGIN_TIME_OUT_NOT kNot;
				kNot.m_wstrUserID = GetName();
				SendToLoginServer( ELG_NEXON_USER_LOGIN_TIME_OUT_NOT, kNot );
				return;
			}
		}
		break;

	default:
		{
			START_LOG( cerr, L"Unexpected state. Ignoring login processing." )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kPacket_.m_byteAuthorizeResult )
				<< END_LOG;
		}
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// PC방 인증 결과에 따른 처리

	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;

	switch( kPacket_.m_byteAuthorizeResult )
	{
	case 0: // forbidden, 미가입 PC방
		{
			START_LOG( clog, L"PC Bang authentication server returned forbidden." )
				<< BUILD_LOG( GetIPStr() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			// 비가맹 PC방이라는 메시지를 클라이언트로 전송
			kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_FORBIDDEN;
			SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );

			// 게임 접속 차단
			SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Fail );
			ReserveDestroy();
		}
		return;

	case 1: // allowed, 가입 PC방
		{
			// PC방 프리미엄 혜택 적용
			m_kUserPcBangManager.SetIsPcBang( true );

			//{{ 2007. 11. 13  최육사  유저 통계 PC방
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_IsPcBang,	1 );
			//}}

			// - 프리미엄 적용과는 관련없는 데이터
			//if( kPacket_.m_byteIsCharged != 1 )
			//{
			//	START_LOG( cwarn, L"인증 결과가 allowed이지만 IsCharged값은 PC방이 아니라고 옴." )
			//		<< BUILD_LOG( GetIPStr() )
			//		<< BUILD_LOG( GetUID() )
			//		<< BUILD_LOG( GetName() )
			//		<< END_LOG;
			//}

			// 가맹 PC방 혜택 알림 메시지 전송
			switch( kPacket_.m_byteOption )
			{
			case 11: // PC방 계약으로 접속하였습니다.
				{
					kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_ALLOWED_1;
					kPacketNot.m_uiData = kPacket_.m_ulArgument;
					SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );
				}
				break;

			case 17: // PC방 정량제 접속입니다.
			case 29: // 바람 PC방 정량제 접속입니다.
				{
					kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_ALLOWED_2;
					SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );

					// 클라이언트에 출력할 메시지 저장
					m_kUserPcBangManager.SetPcBangMessage( kPacket_.m_ulArgument );
				}
				break;

			default:
				{
					START_LOG( cerr, L"Undefined option value." )
						<< BUILD_LOGc( kPacket_.m_byteOption )
						<< END_LOG;
				}
				break;
			}
		}
		break;

	case 2: // Trial 일반 가정집, 프리미엄 혜택 없음
		{
			m_kUserPcBangManager.SetCanTry( true );

			// AuthorizeType 확인해서 메시지 띄울거 체크하자!
			switch( kPacket_.m_byteAuthorizeType )
			{
			case 'M': // 정량PC방
			case 'F': // 정액PC방
			case 'B': // 바람정량PC방
				{
					// 프리미엄 서비스 정상화 메시지 전송
					KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;
					kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_RECONNECT;
					kPacketNot.m_uiData = 0;
					SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );
				}
				break;
			}
		}
		break;

	case 3: // Terminate - 게임 실행을 차단하고 인증서버로 로그아웃 패킷을 전송한다.
		{
			START_LOG( clog, L"PC Bang authentication server returned Terminate." )
				<< BUILD_LOG( GetIPStr() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			// 게임 접속 차단
			SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Fail );
			ReserveDestroy();
		}
		break;

	default:
		START_LOG( cerr, L"Invalid result value." )
			<< BUILD_LOG( kPacket_.m_byteAuthorizeResult )
			<< END_LOG;
		break;
	}

	// PC방 인증 결과 정보 저장
	m_kUserPcBangManager.InitPcBangAuth( kPacket_.m_byteAuthorizeResult, kPacket_.m_byteAuthorizeType, kPacket_.m_byteChargeType );

	// 채널이동의 경우 KGSFSM::S_FIELD_MAP상태다
	if( GetStateID() != KGSFSM::S_SERVER_SELECT  &&
		GetStateID() != KGSFSM::S_FIELD_MAP  &&
		GetStateID() != KGSFSM::S_ROOM )
	{
		StateTransition( KGSFSM::I_TO_LOGINED );
	}

	// PC방 인증 결과 통보
	KENX_USER_LOGIN_NOT kNot;
	kNot.m_bIsGameBang = IsPcBang();
	SendPacket( ENX_USER_LOGIN_NOT, kNot );

	// 로그인 성공 처리
	m_bLoginFail = false;

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
	//}}
	// last login
	SendToAccountDB( DBE_UPDATE_LOGIN_TIME_NOT );

	// 모든 로그인 절차가 끝나고 GSUser의 State가 S_LOGINED로 바뀐후에 장바구니를 받아오자!
	KDBE_GET_WISH_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUserUID = GetUID();
	SendToGameDB( DBE_GET_WISH_LIST_REQ, kPacketToDB );
}

_IMPL_ON_FUNC( ELG_REGISTER_NEXON_CHANNEL_CHANGE_USER_ACK, KENX_AUTH_LOGIN_ACK )
{
	//////////////////////////////////////////////////////////////////////////
	// 상태 검사
	switch( GetStateID() )
	{
	case KGSFSM::S_UID_UPDATED:
	case KGSFSM::S_LOGINED:
	case KGSFSM::S_SERVER_SELECT:
	case KGSFSM::S_FIELD_MAP:
	case KGSFSM::S_ROOM:
		// 정상적으로 채널이동 중 PC방 인증 완료
		break;

	default:
		{
			START_LOG( cerr, L"Unexpected state. Ignoring login processing." )
				<< BUILD_LOG( GetStateID() )
				<< BUILD_LOG( GetStateIDString() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kPacket_.m_byteAuthorizeResult )
				<< END_LOG;
		}
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// PC방 인증 결과에 따른 처리

	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;

	switch( kPacket_.m_byteAuthorizeResult )
	{
	case 0: // forbidden, 미가입 PC방
		{
			START_LOG( clog, L"PC Bang authentication server returned forbidden." )
				<< BUILD_LOG( GetIPStr() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			// 비가맹 PC방이라는 메시지를 클라이언트로 전송
			kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_FORBIDDEN;
			SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );

			// 게임 접속 차단
			SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Fail );
			ReserveDestroy();
		}
		return;

	case 1: // allowed, 가입 PC방
		{
			//{{ 2007. 11. 13  최육사  유저 통계 PC방
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_IsPcBang,	1 );
			//}}

			// - 프리미엄 적용과는 관련없는 데이터
			//if( kPacket_.m_byteIsCharged != 1 )
			//{
			//	START_LOG( cerr, L"인증 결과가 allowed이지만 IsCharged값은 PC방이 아니라고 옴." )
			//		<< BUILD_LOG( GetIPStr() )
			//		<< BUILD_LOG( GetUID() )
			//		<< BUILD_LOG( GetName() )
			//		<< END_LOG;
			//}

			// 채널 이동 전후 PC방 프리미엄 정보 비교
			if( IsPcBang() == false )
			{
				START_LOG( cerr, L"User was not in a PC Bang before channel transfer but is now?" )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetUID() )
					<< END_LOG;
			}
		}
		break;

	case 2: // Trial 일반 가정집, 프리미엄 혜택 없음
		{
			m_kUserPcBangManager.SetCanTry( true );

			// 채널 이동 전후 PC방 프리미엄 정보 비교
			if( IsPcBang() == true )
			{
				START_LOG( cerr, L"User was in a PC Bang before channel transfer but is not now?" )
					<< BUILD_LOG( GetIPStr() )
					<< BUILD_LOG( GetName() )
					<< BUILD_LOG( GetUID() )
					<< END_LOG;
			}
		}
		break;

	case 3: // Terminate - 게임 실행을 차단하고 인증서버로 로그아웃 패킷을 전송한다.
		{
			START_LOG( clog, L"PC Bang authentication server returned Terminate." )
				<< BUILD_LOG( GetIPStr() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;

			// 게임 접속 차단
			SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Fail );
			ReserveDestroy();
		}
		break;

	default:
		START_LOG( cerr, L"Invalid result value." )
			<< BUILD_LOG( kPacket_.m_byteAuthorizeResult )
			<< END_LOG;
		break;
	}

	//{{ 2010. 01. 29  최육사	PC방 상용화
	m_kUserPcBangManager.InitPcBangAuth( kPacket_.m_byteAuthorizeResult, kPacket_.m_byteAuthorizeType, kPacket_.m_byteChargeType );
	//}}

	// PC방 인증 결과 통보
	KENX_USER_LOGIN_NOT kNot;
	kNot.m_bIsGameBang = IsPcBang();
	SendPacket( ENX_USER_LOGIN_NOT, kNot );

	// 로그인 성공 처리
	m_bLoginFail = false;

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
	//}}
	// last login
	SendToAccountDB( DBE_UPDATE_LOGIN_TIME_NOT );

	// 모든 로그인 절차가 끝나고 GSUser의 State가 S_LOGINED로 바뀐후에 장바구니를 받아오자!
	KDBE_GET_WISH_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUserUID = GetUID();
	SendToGameDB( DBE_GET_WISH_LIST_REQ, kPacketToDB );
}

IMPL_ON_FUNC( ENX_AUTH_TERMINATE_NOT )
{
	// 정량제를 이용하다 시간이 만료된 경우이므로, 적절한 메시지를 보여주고 접속을 끊는다, 인증서버는 로그아웃을 전송해야한다.
	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
	//{{ 2012. 02. 14    김민성   넥슨 안심 로그인 - 목표 위치기반 로그인
	//}}
	//}}

	switch( kPacket_.m_byteOption )
	{
	case 3: // 정액 만료
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_1;
		break;

	case 19: // 정량 만료
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_2;
		break;

	case 2: // IP당 접속 수를 초과
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_3;
		break;

	case 27: // IP당 접속 수를 초과
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_3;
		return; // 종료 처리 안함.

	default:
		{
			START_LOG( cerr, L"Undefined option value." )
				<< BUILD_LOG( kPacket_.m_byteOption )
				<< BUILD_LOGc( kPacketNot.m_cType )
				<< END_LOG;
		}
	}

	// 클라이언트에게 전달
	SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );

	// 접속 종료 처리
	SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Server );
	ReserveDestroy();
}

IMPL_ON_FUNC( ENX_AUTH_MESSAGE_NOT )
{
	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}

	// 클라이언트에 출력할 메시지 전송
	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;
	kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_MESSAGE;
	kPacketNot.m_uiData = kPacket_.m_ulArgument;
	SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

_IMPL_ON_FUNC( E_KICK_USER_NOT, int )
{
	START_LOG_WITH_NAME( cout )
		<< BUILD_LOG( GetStateIDString() )
		<< BUILD_LOG( NetError::GetErrStr( kPacket_ ) )
		<< BUILD_LOG( kPacket_ );

	Kick( kPacket_ );
}

//{{ 2010. 01. 29  최육사	PC방 상용화
_IMPL_ON_FUNC( ENX_USER_TERMINATE_NOT, KENX_COMMON_ACK_NOT )
{
	// 정량제를 이용하다 시간이 만료된 경우이므로, 적절한 메시지를 보여주고 접속을 끊는다, 인증서버는 로그아웃을 전송해야한다.
	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;

	switch( kPacket_.m_byteOption )
	{
	case 3: // 정액 만료
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_1;
		break;

	case 19: // 정량 만료
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_2;
		break;

	case 2: // IP당 접속 수를 초과
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_3;
		break;

	case 27: // IP당 접속 수를 초과
		kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_TERMINATE_3;
		return; // 종료 처리 안함.

	default:
		{
			START_LOG( cerr, L"Undefined option value." )
				<< BUILD_LOG( kPacket_.m_byteOption )
				<< END_LOG;
		}
	}

	// 클라이언트에게 전달
	SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );

	// 접속 종료 처리
	SetDisconnectReason( KStatistics::eSIColDR_PCBang_Auth_Server );
	ReserveDestroy();
}

_IMPL_ON_FUNC( ENX_MESSAGE_NOT, KENX_COMMON_ACK_NOT )
{
	// 클라이언트에 출력할 메시지 전송
	KEGS_PC_BANG_AUTH_RESULT_NOT kPacketNot;
	kPacketNot.m_cType = KEGS_PC_BANG_AUTH_RESULT_NOT::PBRT_MESSAGE;
	kPacketNot.m_uiData = kPacket_.m_uiArgument;
	SendPacket( EGS_PC_BANG_AUTH_RESULT_NOT, kPacketNot );
}
//}}

IMPL_ON_FUNC( EGS_NEW_USER_JOIN_REQ )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_INIT ), EGS_NEW_USER_JOIN_REQ, EGS_NEW_USER_JOIN_ACK );
	//}}

	// USER ID 길이 검사.
	std::string	multiByteString = KncUtil::toNarrowString( kPacket_.m_wstrID );
	int nSize = ( int )multiByteString.size();

	if( nSize < 4 || nSize > 20 )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NEW_ACCOUNT_03;

		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( nSize )
			<< BUILD_LOG( kPacket_.m_wstrID )
			<< END_LOG;

		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrID ) )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	// USER PASSWORD 길이 검사.
	multiByteString = KncUtil::toNarrowString( kPacket_.m_wstrPassword );
	nSize = ( int )multiByteString.size();

	if( nSize < 4 || nSize > 20 )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NEW_ACCOUNT_04;

		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( nSize )
			<< BUILD_LOG( kPacket_.m_wstrPassword )
			<< END_LOG;

		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrPassword ) )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	// USER NAME 길이 검사.
	multiByteString = KncUtil::toNarrowString( kPacket_.m_wstrName );
	nSize = ( int )multiByteString.size();

	if( nSize < 1 || nSize > 25 )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NEW_ACCOUNT_06;

		START_LOG_WITH_NAME( cwarn )
			<< BUILD_LOG( nSize )
			<< BUILD_LOG( kPacket_.m_wstrName )
			<< END_LOG;

		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrName ) )
	{
		KEGS_NEW_USER_JOIN_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket );
		ReserveDestroy();
		return;
	}

	kPacket_.m_byteGuestUser = m_kNexonAccountInfo.m_byteGuestUser;

	SendToLoginAccountDB( DBE_NEW_USER_JOIN_REQ, kPacket_ );
}

_IMPL_ON_FUNC( DBE_NEW_USER_JOIN_ACK, KEGS_NEW_USER_JOIN_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_INIT ) );

	SendPacket( EGS_NEW_USER_JOIN_ACK, kPacket_ );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG_WITH_NAME( cerr )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;
	}

	SetDisconnectReason( KStatistics::eSIColDR_Internal_Account );
	ReserveDestroy();
}

IMPL_ON_FUNC_NOPARAM( EGS_STATE_CHANGE_SERVER_SELECT_REQ )
{
	VERIFY_STATE( ( 3, KGSFSM::S_LOGINED, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( GetStateID() == KGSFSM::S_LOGINED )
	{
		KEGS_STATE_CHANGE_SERVER_SELECT_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;

		SendPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK, kPacket );
	}
	//{{ 2012. 02. 07	최육사	배틀필드 시스템
	else if( GetStateID() == KGSFSM::S_FIELD_MAP  ||  GetStateID() == KGSFSM::S_ROOM )
	//}}
	{
		//{{ 2012. 02. 07	최육사	배틀필드 시스템
		if( GetStateID() == KGSFSM::S_ROOM )
		{
			StateTransition( KGSFSM::I_TO_FIELD_MAP );
		}
		//}}

		//해킹유저인데 동의를 하지 않은상태이면 점속을 끝는다.
		if( m_cHackingUserType == SEnum::HUT_DISAGREE_HACK_USER
			&& SiKGameSysVal()->IsOnDisagreeHackUserFunc() == true
			)
		{
			KEGS_STATE_CHANGE_SERVER_SELECT_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_HACK_USER_02;

			START_LOG( cwarn, L"Terminated reported hacking user for not providing consent." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( m_kNexonAccountInfo.m_wstrID )
				<< END_LOG;

			SendPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK, kPacket );
			//{{ 2013. 2. 14	박세훈	해킹 등록 유저인데 즉시 킥하지 않는 경우 종료 사유 추가
			SetDisconnectReason( KStatistics::eSIColDR_Client_Hacking );
			//}}
			ReserveDestroy();
			return;
		}

		// 게임중 캐릭선택으로 오는것을 게임종료처리와 같은 상황으로 본다.
		// 유닛이 선택되어 있다면 해당 유닛정보를 DB업데이트, 로그인서버에서 지우기, 나머지 처리를 한다.
		if( GetCharUID() > 0 )
		{
			//{{ 2010. 8. 4	최육사	코드 리팩토링
			//#ifdef SERV_DESELECT_UNIT_REFAC
			//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링
			OnDeselectUnit( EGS_STATE_CHANGE_SERVER_SELECT_REQ, false );
			//}}
			//#endif SERV_DESELECT_UNIT_REFAC
			//}}
			return;
		}

		// 유닛 정보 초기화
		ResetUnitInfo();

		KEGS_STATE_CHANGE_SERVER_SELECT_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		SendPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK, kPacket );
	}

	StateTransition( KGSFSM::I_TO_SERVER_SELECT );
}

//////////////////////////////////////////////////////////////////////////
// 안쓰는 패킷 될듯!
IMPL_ON_FUNC_NOPARAM( EGS_GET_SERVER_SET_DATA_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_LOGINED, KGSFSM::S_FIELD_MAP ) );

	KEGS_GET_SERVER_SET_DATA_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;

	std::map< int, std::vector< KServerInfo > > mapServerList;
	KBaseServer::GetKObj()->GetServerList( mapServerList );

	std::map< int, std::vector< KServerInfo > >::iterator mit;
	for( mit = mapServerList.begin(); mit != mapServerList.end(); mit++ )
	{
		int iMaxCCU = 0;
		int iCCU = 0;
		std::vector< KServerInfo >::iterator vit;
		for( vit = mit->second.begin(); vit != mit->second.end(); vit++ )
		{
			if( vit->m_iServerClass == KBaseServer::ESC_GAME )
			{
				iMaxCCU += vit->m_nMaxUser;
				iCCU += vit->m_nCurrentUser;
			}
		}

		KServerSetData kServerSetData;
		kServerSetData.serverSetID = mit->first;
		kServerSetData.serverSetName = L"솔레스";
		if( iMaxCCU <= 0 )
		{
			START_LOG( cwarn, L"Invalid maximum concurrent-user count." )
				<< BUILD_LOG( mit->first )
				<< BUILD_LOG( iMaxCCU )
				<< END_LOG;

			iMaxCCU = 1;
		}

		if( iCCU < 1500 )
		{
			kServerSetData.userCountLevel = KServerSetData::UCL_FREE;
		}
		else if( iCCU < 3000 )
		{
			kServerSetData.userCountLevel = KServerSetData::UCL_NORMAL;
		}
		else if( iCCU < iMaxCCU )
		{
			kServerSetData.userCountLevel = KServerSetData::UCL_BUSY;
		}
		else
		{
			kServerSetData.userCountLevel = KServerSetData::UCL_FULL;
		}

		//int iPercent = ( int )( ( float )iCCU * 100.f / ( float )iMaxCCU );
		//if( iPercent < 30 )
		//{
		//    kServerSetData.userCountLevel = KServerSetData::UCL_FREE;
		//}
		//else if( iPercent < 70 )
		//{
		//    kServerSetData.userCountLevel = KServerSetData::UCL_NORMAL;
		//}
		//else if( iPercent < 100 )
		//{
		//    kServerSetData.userCountLevel = KServerSetData::UCL_BUSY;
		//}
		//else
		//{
		//    kServerSetData.userCountLevel = KServerSetData::UCL_FULL;
		//}

		kPacket.m_ServerSetList.push_back( kServerSetData );
	}

	SendPacket( EGS_GET_SERVER_SET_DATA_ACK, kPacket );
}
//////////////////////////////////////////////////////////////////////////

IMPL_ON_FUNC( EGS_SELECT_SERVER_SET_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	// 현재는 서버군이 하나다.
	KEGS_SELECT_SERVER_SET_ACK kPacket;

	//블록된 유저면 처리를 해준다.
	switch( GetAuthLevel() )
	{
	case SEnum::UAL_BLOCKED_2:
	case SEnum::UAL_BLOCKED_3:
	case SEnum::UAL_BLOCKED_4:
	case SEnum::UAL_BLOCKED_5:
	case SEnum::UAL_BLOCKED_6:
		//{{ 계정 제재 일수 상태 추가 - 김민성
	case SEnum::UAL_BLOCKED_7:
	case SEnum::UAL_BLOCKED_8:
	case SEnum::UAL_BLOCKED_9:
		//}}
		{
			kPacket.m_iOK = NetError::ERR_VERIFY_11;
			kPacket.m_kAccountBlockInfo.m_wstrEndTime = m_kAccountBlockInfo.m_wstrEndTime;
			kPacket.m_kAccountBlockInfo.m_wstrBlockReason = m_kAccountBlockInfo.m_wstrBlockReason;

			SendPacket( EGS_SELECT_SERVER_SET_ACK, kPacket );

			SetDisconnectReason( KStatistics::eSIColDR_Account_Block );
			ReserveDestroy();
			return;
		}
		break;
	}


	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_wstrIP = NetCommon::GetLocalIPW();
	kPacket.m_usPort = KNetLayer::GetKObj()->m_usPortMaster;
	SendPacket( EGS_SELECT_SERVER_SET_ACK, kPacket );
}

//{{ 2009. 5. 26  최육사	채널이동
IMPL_ON_FUNC( EGS_CHECK_CHANNEL_CHANGE_REQ )
{
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_FIELD_MAP ), EGS_CHECK_CHANNEL_CHANGE_REQ, EGS_CHECK_CHANNEL_CHANGE_ACK );
	//}}

	// 필드상태가 아니면 이동할수 없다
	if( GetFieldUID() <= 0  ||  GetRoomUID() > 0  ||  GetRoomListID() > 0 )
	{
		START_LOG( cwarn, L"Channel transfer is not allowed from this location." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetFieldUID() )
			<< BUILD_LOG( GetRoomUID() )
			<< BUILD_LOG( GetRoomListID() )
			<< END_LOG;

		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_09;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}

	// 채널이동 쿨타임
	if( m_kTimer[TM_UNIT_DURATION].elapsed() < 5.0 )
	{
		START_LOG( cwarn, L"Channel transfer cooldown." )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_10;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}

	UidType iGSUID = 0;

	// 같은 채널을 이동하려고 한다면 실패처리!
	if( SiKChannelManager()->GetChannelID() == kPacket_.m_iChannelID )
	{
		START_LOG( cwarn, L"The destination channel is the same as the current channel." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( SiKChannelManager()->GetChannelID() )
			<< BUILD_LOG( kPacket_.m_iChannelID )
			<< END_LOG;

		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_07;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}

	// 이동할수 있는 채널인지 검사
	if( SiKChannelManager()->GetServerUIDByChannelID( kPacket_.m_iChannelID, iGSUID ) == false )
	{
		START_LOG( cerr, L"ChannelID does not exist." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iChannelID )
			<< END_LOG;

		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_01;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}

	//{{ 2011. 07. 27	최육사	대전 개편
	if( IsRequestMatchMaking() == true )
	{
		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_MATCH_MAKING_20;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}

	if( GetMatchUID() != 0 )
	{
		KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_MATCH_MAKING_20;
		SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
		return;
	}
	//}}

	KECN_CHECK_CHANNEL_CHANGE_REQ kPacketToCn;
	kPacketToCn.m_iSourGSUID = KBaseServer::GetKObj()->GetUID();
	kPacketToCn.m_iDestGSUID = iGSUID;
	kPacketToCn.m_iUserUID = GetUID();
	kPacketToCn.m_iCharUID = GetCharUID();
	//{{ 2009. 11. 10  최육사	채널이동
	GetChannelChangeInfo( kPacketToCn.m_kChangeUserInfo );
	//}}
	SendToCnServer( ECN_CHECK_CHANNEL_CHANGE_REQ, kPacketToCn );
}

IMPL_ON_FUNC( ECN_CHECK_CHANNEL_CHANGE_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KEGS_CHECK_CHANNEL_CHANGE_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;
	SendPacket( EGS_CHECK_CHANNEL_CHANGE_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_CONNECT_CHANNEL_CHANGE_REQ )
{
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_INIT ), EGS_CONNECT_CHANNEL_CHANGE_REQ, EGS_CONNECT_CHANNEL_CHANGE_ACK );
	//}}

	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( kPacket_.m_kConnectReq.m_wstrVersion );

	//////////////////////////////////////////////////////////////////////////
	// 버전 비교
	KEGS_CONNECT_CHANNEL_CHANGE_ACK kPacketAck;
	KELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ kPacket;

	// 채널 이동 예약 검사
	if( SiKChannelManager()->IsExistChannelChangeUser( kPacket_.m_kSelectUnitReq.m_iUnitUID ) == false )
	{
		START_LOG( cerr, L"No channel transfer is scheduled!" )
			<< BUILD_LOG( kPacket_.m_kSelectUnitReq.m_iUnitUID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_CHANNEL_CHANGE_06;
		goto end_proc;
	}

	// 해당 유저가 접속할 수 있도록 예약정보 제거
	if( SiKChannelManager()->DelChannelChangeUser( kPacket_.m_kSelectUnitReq.m_iUnitUID, kPacket.m_kChangeUserInfo ) == false )
	{
		START_LOG( cerr, L"No channel transfer is scheduled!" )
			<< BUILD_LOG( kPacket_.m_kSelectUnitReq.m_iUnitUID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_CHANNEL_CHANGE_06;
		goto end_proc;
	}
	else
	{
		//{{ 2010. 01. 29  최육사	PC방 상용화
		m_kUserPcBangManager.SetIsPcBang( kPacket.m_kChangeUserInfo.m_bIsPcBang );
		//}}
		m_kUserPcBangManager.SetPcBangType( kPacket.m_kChangeUserInfo.m_iPcBangType );
	}

	if( OnConnectReq( kPacket_.m_kConnectReq, kPacketAck.m_kConnectAck ) == false )
	{
		START_LOG( cwarn, L"connect error" )
			<< BUILD_LOG( NetError::GetErrStr( kPacketAck.m_kConnectAck.m_iOK ) )
			<< END_LOG;

		kPacketAck.m_iOK = kPacketAck.m_kConnectAck.m_iOK;
		goto end_proc;
	}

	//////////////////////////////////////////////////////////////////////////

	// 계정인증
	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_bDebugAuth )
		<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_wstrUserID )
		<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_wstrPasswd );

	kPacket.m_kConnectAck = kPacketAck.m_kConnectAck;
	kPacket.m_kSelectUnitReq = kPacket_.m_kSelectUnitReq;
	kPacket.m_kUserAuthReq.m_kAuthenticateReq = kPacket_.m_kVerifyAccountReq;
	kPacket.m_kUserAuthReq.m_kNexonAccountInfo.m_wstrLoginIP = KncUtil::toWideString( KSession::GetIPStr() );

	kPacket_.m_kVerifyAccountReq.m_bDebugAuth = false;// 해외팀에서는 bDebugAuth를 이용한 예외상황 고려안함
	kPacket.m_kUserAuthReq.m_kAuthenticateReq.m_bDebugAuth = false; // 해외팀에서는 bDebugAuth를 이용한 예외상황 고려안함


	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_kVerifyAccountReq.m_wstrUserID ) )
	{
		kPacketAck.m_iOK = NetError::ERR_ODBC_00;
		goto end_proc;
	}

	if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_INTERNAL )
	{
		if( KODBC::IsInvalidMarkIn( kPacket_.m_kVerifyAccountReq.m_wstrPasswd ) )
		{
			kPacketAck.m_iOK = NetError::ERR_ODBC_00;
			goto end_proc;
		}
	}

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	if( KSimLayer::GetKObj()->GetVersionFlag() != KSimLayer::VF_OPEN_TEST )		// 오픈 테섭에서는 검사 제외
	{
		if( SiKGameSysVal()->IsMachineIDDuplicateCheck() == true )
		{
			if( m_kUserSecurityManager.CheckValidMachineID( kPacket_.m_kVerifyAccountReq.m_strMachineID ) == false )
			{
				START_LOG( cout, L"[Notice] Invalid machine ID; sending a notification packet to the client!" )
					<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_wstrHWID )
					<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_wstrUserID )
					<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_wstrPasswd )
					<< BUILD_LOG( kPacket_.m_kVerifyAccountReq.m_bDebugAuth );

				//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
				KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
				kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_MACHINE_ID;
				SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );
				//}}

				//{{ 2011. 09. 06	최육사	머신 ID 기능 업데이트
				// 머신ID가 비어있으면 접속 차단!
				Kick( NetError::ERR_CHECK_MACHINE_ID_00 );
				//}}
			}
		}
	}

	// 머신ID 저장!
	m_kUserSecurityManager.SetMachineID( kPacket_.m_kVerifyAccountReq.m_strMachineID );
	//}}

	SendToLoginServer( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ, kPacket );

	m_kTimer[TM_INIT].restart();
	return;

end_proc:
	SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );

	// 채널 이동실패로 인한 종료처리
	SetDisconnectReason( KStatistics::eSIColDR_Channel_Change_Fail );
	ReserveDestroy();
}

IMPL_ON_FUNC( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_INIT ) );

	KEGS_CONNECT_CHANNEL_CHANGE_ACK kPacketAck;
	KELG_CHANNEL_CHANGE_REGISTER_USER_REQ kPacketReq;

	if( OnNexonUserAuthenticateAck( kPacket_.m_kUserAuthAck, kPacketAck.m_kVerifyAccountAck ) == false )
	{
		START_LOG( cerr, L"Nexon authentication failed!" )
			<< BUILD_LOG( NetError::GetErrStr( kPacketAck.m_kVerifyAccountAck.m_iOK ) )
			<< END_LOG;

		kPacketAck.m_iOK = kPacketAck.m_kVerifyAccountAck.m_iOK;
		goto end_proc;
	}


	//////////////////////////////////////////////////////////////////////////
	// 로그인 서버에 등록하기
	kPacketReq.m_kConnectAck = kPacket_.m_kConnectAck;
	kPacketReq.m_kVerifyAccountAck = kPacketAck.m_kVerifyAccountAck;
	kPacketReq.m_kSelectUnitReq = kPacket_.m_kSelectUnitReq;
	kPacketReq.m_kChangeUserInfo = kPacket_.m_kChangeUserInfo;

	// Verify Account
	kPacketReq.m_kVerifyAccountAck.m_kAccountInfo = kPacket_.m_kUserAuthAck.m_kAccountInfo;
	kPacketReq.m_kVerifyAccountAck.m_kAccountInfo.m_bIsGuestUser = IsGuestUser(); // 체험아이디 정보

	//{{ 2010. 01. 11	김민성	IP추가
	kPacketReq.m_kVerifyAccountAck.m_wstrClientIP = KncUtil::toWideString( GetIPStr() );
	//}}


	// Register User
	kPacketReq.m_kRegUserReq.m_wstrUserID = GetName();
	kPacketReq.m_kRegUserReq.m_iChannelID = SiKChannelManager()->GetChannelID();
	kPacketReq.m_kRegUserReq.m_uiIP		= GetIP();
	kPacketReq.m_kRegUserReq.m_byteCanTry = 0;
	kPacketReq.m_kRegUserReq.m_byteGuestUser = m_kNexonAccountInfo.m_byteGuestUser;
	kPacketReq.m_kRegUserReq.m_cAuthLevel = static_cast<char>(GetAuthLevel());
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	kPacketReq.m_kRegUserReq.m_strMachineID = m_kUserSecurityManager.GetStringMachineID();
	//}}



	SendToLoginServer( ELG_CHANNEL_CHANGE_REGISTER_USER_REQ, kPacketReq );

	//////////////////////////////////////////////////////////////////////////
	StateTransition( KGSFSM::I_TO_UID_UPDATED );
	ms_kMaxLoginTime.CheckMaxProcessTime( GetName(), ( float )m_kTimer[TM_INIT].elapsed() );

	//080624.hoons.인증패킷 처리를 다해주어야 클라이언트에서 nProtect가 동작함.(해킹유저만)
	//if( m_cHackingUserType == SEnum::HUT_AGREE_HACK_USER )
	//{
	//	// nProtect 한국인증 모듈 사용설정
	//	if( KSimLayer::GetKObj()->CheckCommonFlag( KSimLayer::CommonFlag::CF_NPGG ) )
	//	{
	//		m_kGGAuth.SetEnable( true );
	//	}
	//	m_kGGAuth.Init();
	//}

	//korea nProtect 초기 인증을 클라이언트에 보낸다.
	//if( m_kGGAuth.FirstAuthQuery( GetThisPtr<KGSUser>() ) == false )
	//{
	//	SetDisconnectReason( KStatistics::eSIColDR_NProtect_Auth_First );
	//	ReserveDestroy();

	//	START_LOG( cerr, L"nProtect 첫인증 실패.!" )
	//		<< BUILD_LOG( GetUID() )
	//		<< BUILD_LOG( GetName() )
	//		<< END_LOG;
	//	return;
	//}

	//{{ 2009. 8. 18  최육사	hack shield
	// Port: HackShield's first check removed with the anti-cheat (T10)
	//}}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	m_kUserAbuserManager.InitAccountAbuser( GetThisPtr<KGSUser>() );


	//}}

	//{{ 2012. 02. 21	김민성	2차 보안 채널 이동시(미니맵) 오류 수정
	{
		CTime tLasstAuthDate;
		if( kPacket_.m_bUseSecondPW == true )
		{
			if( KncUtil::ConvertStringToCTime( kPacket_.m_wstrLastAuthDate, tLasstAuthDate ) == false )
			{
				START_LOG( cerr, L"Failed to convert the time string." )
					<< BUILD_LOG( kPacket_.m_wstrLastAuthDate )
					<< END_LOG;
			}
		}
		SetIsUseSecondPW( kPacket_.m_bUseSecondPW );
		SetSecondPWFailedCount( kPacket_.m_iFailedCount );
		SetSecondPW( kPacket_.m_wstrSecondPW );
		SetLastAuthDate( tLasstAuthDate );

		if( kPacket_.m_bUseSecondPW == true )
		{
			SetSuccessSecondSecurity( true );
		}
		else
		{
			SetSuccessSecondSecurity( false );
		}

		KEGS_SECOND_SECURITY_INFO_NOT kNot;
		kNot.m_iOK = kPacketAck.m_kVerifyAccountAck.m_iOK;
		kNot.m_bUseSecondPW = kPacket_.m_bUseSecondPW;
		SendPacket( EGS_SECOND_SECURITY_INFO_NOT, kNot );
	}
	//}}

	return;

end_proc:
	SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );

	// 채널 이동실패로 인한 종료처리
	SetDisconnectReason( KStatistics::eSIColDR_Channel_Change_Fail );
	ReserveDestroy();
}

IMPL_ON_FUNC( ELG_CHANNEL_CHANGE_REGISTER_USER_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_UID_UPDATED ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		KEGS_CONNECT_CHANNEL_CHANGE_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );

		// 채널 이동실패로 인한 종료처리
		SetDisconnectReason( KStatistics::eSIColDR_Channel_Change_Fail );
		ReserveDestroy();
		return;
	}

	//{{ 2012. 06. 14	박세훈	매일매일 선물 상자
	//}}

	// 캐릭터 선택하러 가기위해 서버 선택 상태까지 바로 이동!
	StateTransition( KGSFSM::I_TO_LOGINED );
	StateTransition( KGSFSM::I_TO_SERVER_SELECT );

	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 선택
	KDBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_REQ kPacketToDB;
	kPacketToDB.m_kConnectAck = kPacket_.m_kConnectAck;
	kPacketToDB.m_kVerifyAccountAck = kPacket_.m_kVerifyAccountAck;
	kPacketToDB.m_kSelectUnitReq = kPacket_.m_kSelectUnitReq;
	kPacketToDB.m_kChangeUserInfo = kPacket_.m_kChangeUserInfo;

	IF_EVENT_ENABLED( CEI_OLD_YEAR_EVENT_2013 )
	{
		kPacketToDB.m_kSelectUnitReq.m_setCodeEventScriptID.insert( CEI_OLD_YEAR_EVENT_2013 );
	}
	IF_EVENT_ENABLED( CEI_NEW_YEAR_EVENT_2014 )
	{
		kPacketToDB.m_kSelectUnitReq.m_setCodeEventScriptID.insert( CEI_NEW_YEAR_EVENT_2014 );
	}



	SendToGameDB( DBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	KEGS_CONNECT_CHANNEL_CHANGE_ACK kPacketAck;

	if( kPacket_.m_kSelectUnitAck.m_iOK != NetError::NET_OK )
	{
		//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
		if( kPacket_.m_kSelectUnitAck.m_iOK == NetError::ERR_SELECT_UNIT_07 )
		{
			START_LOG( cout, L"A user attempted to select a character using a UnitUID that does not belong to their UserUID!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetUserID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetCharName() );

			KDBE_SEND_PHONE_MSG_NOT kPacketNot;
			kPacketNot.m_wstrSMSMessage = x2::str( x2::wformat( L"계정소속이 아닌 캐릭터로 선택시도한 유저 발견! : UserUID(%d)" ) % GetUID() );
			//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
			SiKSMSPhoneNumberManager()->GetPhoneNumberList( KSMSPhoneNumberManager::PNLT_ELSWORD_DEV_TEAM, kPacketNot.m_vecPhoneNum );
			//}}
			if( kPacketNot.m_vecPhoneNum.empty() == false )
			{
				SendToSMSDB( DBE_SEND_PHONE_MSG_NOT, kPacketNot );
			}

			// 자동으로 어뷰저 등록
			m_kUserAbuserManager.RegEDAbuser( GetThisPtr<KGSUser>() );
			m_kUserAbuserManager.RegItemAbuser( GetThisPtr<KGSUser>() );
			//HackUserRegPacketMornitor();
		}
		//}}

		kPacketAck.m_iOK = kPacket_.m_kSelectUnitAck.m_iOK;
		SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );

		// 채널 이동실패로 인한 종료처리
		SetDisconnectReason( KStatistics::eSIColDR_Channel_Change_Fail );
		ReserveDestroy();
		return;
	}

	// 2007.12. 28. jseop. 닉네임이 잘못되어 변경해야 하는 경우.
	if( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_wstrNickName.compare( L"__DELETED__" ) == 0 )
	{
		kPacketAck.m_iOK = NetError::ERR_SELECT_UNIT_06;
		SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );
		return;
	}

	// 보스 필드라면 마을로 이동 시키자
	if( SiCXSLBattleFieldManager()->IsBossFieldID( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_kLastPos.m_iMapID ) == true )
	{
		kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_kLastPos.m_iMapID = SiCXSLMapData()->GetPossibleEnterVillageMapID( kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_ucLevel, kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_mapDungeonClear );
		kPacket_.m_kSelectUnitAck.m_kUnitInfo.m_kLastPos.m_bIgnoreLastTouch = true;
	}
	// 최초 선택이나, 채널 변경에서 동일하게 처리되는 구문은
	// 함수로 처리해서 공용으로 처리되도록 하는게 좋을 듯 하다...

	SendToAccountDB( DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_REQ, kPacket_ );
}


_IMPL_ON_FUNC( DBE_CHANNEL_CHANGE_ACCOUNT_SELECT_UNIT_ACK, KDBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_SERVER_SELECT ) );

	KEGS_CONNECT_CHANNEL_CHANGE_ACK kPacketAck;

		CTime tReleaseTradeBlockTime;
		LIF( KncUtil::ConvertStringToCTime( kPacket_.m_kSelectUnitAck.m_wstrUnitCreateDate, tReleaseTradeBlockTime ) );
		tReleaseTradeBlockTime += CTimeSpan( SiKGameSysVal()->GetNewUnitTradeBlockDay(), 0, 0, 0 );
		CTime tCurTime = CTime::GetCurrentTime();

		if( SiKGameSysVal()->IsNewUnitTradeBlock() == true
			&& ( ( SiKGameSysVal()->GetNewUnitTradeBlockDay() != 0 && tCurTime < tReleaseTradeBlockTime )
			|| ( SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() != 0 && GetUnitClass() < SiKGameSysVal()->GetNewUnitTradeBlockUnitClass() ) )
			)
		{
			m_tReleaseTradeBlockTime = tReleaseTradeBlockTime;

			m_kUserAbuserManager.SetTradeBlockUnit( true );
		}
		else
		{
			m_kUserAbuserManager.SetTradeBlockUnit( false );
		}

	if( kPacket_.m_kSelectUnitAck.m_iOK != NetError::NET_OK )
	{
		kPacketAck.m_iOK = kPacket_.m_kSelectUnitAck.m_iOK;
		goto end_proc;
	}
	else
	{
		//{{ 2009. 5. 13  최육사	P2P포트 체크 - 채널이동에서는
		//if( GetP2PIP().empty() )
		//{
		//	START_LOG( cerr, L"P2P포트 체크 실패!" )
		//		<< BUILD_LOG( GetUID() )
		//		<< END_LOG;
		//}
		//}}

		//////////////////////////////////////////////////////////////////////////
		// 캐릭터 선택에 관련된 모든 처리는 이 함수 안에서 하도록 한다!
		if( OnAccountSelectUnitAck( kPacket_.m_kSelectUnitAck, true ) == false )
		{
			START_LOG( cerr, L"select unit error" )
				<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_kSelectUnitAck.m_iOK ) )
				<< END_LOG;

			goto end_proc;
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 채널 이동 하면서 임시 보관 했던 데이터는 여기서 처리하자!
		SetAccountInfo( kPacket_.m_kVerifyAccountAck.m_kAccountInfo );
		m_kUserEventManager.SetEventTimerInfo( kPacket_.m_kChangeUserInfo.m_mapEventDurationTime );
		//}}
		//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
		m_kUserTitleManager.EquipTitle( kPacket_.m_kChangeUserInfo.m_iEquippedTitleID );
		//}}
		//}}
		//{{ 2010. 02. 08  최육사	PC방 채널 이동 오류 수정
		std::vector< KInventoryItemInfo > vecMoveItemInfo;
		m_kInventory.SetChannelChangeInventoryInfo( kPacket_.m_kChangeUserInfo.m_vecPcBangItemInfo, vecMoveItemInfo );
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		//{{ 2012. 1. 2		Merge 박세훈	( 2013.1.2 지헌 : 미니맵 채널 이동 시 누락됬었던 정보 추가 )
		m_nUnitSlot = kPacket_.m_kChangeUserInfo.m_nUnitSlot;
		//}}

		if( 0 < kPacket_.m_kChangeUserInfo.m_iSummonedRidingPetUID )
		{
			KEGS_SUMMON_RIDING_PET_REQ kPacket;
			kPacket.m_iRidingPetUID = kPacket_.m_kChangeUserInfo.m_iSummonedRidingPetUID;
			kPacket.m_bNeedAck		= false;

			KEventPtr spEvent( new KEvent );
			spEvent->SetData( PI_GS_USER, NULL, EGS_SUMMON_RIDING_PET_REQ, kPacket );
			LIF( spEvent->m_kDestPerformer.AddUID( GetUID() ) );
			QueueingEvent( spEvent );
		}

		// 장착 정보가 바뀌었으므로 다른 유저들에게도 알린다.
		SendUpdateChangeEquippedItem( vecMoveItemInfo );

		//////////////////////////////////////////////////////////////////////////
		// 채널이동
		kPacketAck.m_kConnectAck = kPacket_.m_kConnectAck;
		kPacketAck.m_kVerifyAccountAck = kPacket_.m_kVerifyAccountAck;
		kPacketAck.m_kChangeUserInfo = kPacket_.m_kChangeUserInfo;
		kPacketAck.m_iOK = NetError::NET_OK;
		//////////////////////////////////////////////////////////////////////////
	}

end_proc:
	SendPacket( EGS_CONNECT_CHANNEL_CHANGE_ACK, kPacketAck );



	if( kPacket_.m_kSelectUnitAck.m_iOK == NetError::NET_OK )
	{
		//////////////////////////////////////////////////////////////////////////
		// 캐릭터 선택 성공한 이후에 처리하는 것들은 모두 이 함수 안에서 처리하도록 한다!
		OnSelectUnitSuccess( kPacket_.m_kSelectUnitAck, true );
		//////////////////////////////////////////////////////////////////////////



	}
	else
	{
		// 채널 이동실패로 인한 종료처리
		SetDisconnectReason( KStatistics::eSIColDR_Channel_Change_Fail );
		ReserveDestroy();
	}
}

IMPL_ON_FUNC( EGS_JOIN_FIELD_CHANNEL_CHANGE_REQ )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	int iTempLevel = 0;
	int iTempDungeonID = 0;
	//{{ 2010. 9. 7	최육사	중복 패킷 필터 개선
	VERIFY_STATE_REPEAT_FILTER( ( 1, KGSFSM::S_FIELD_MAP ), EGS_JOIN_FIELD_CHANNEL_CHANGE_REQ, EGS_JOIN_FIELD_CHANNEL_CHANGE_ACK );
	//}}

	KEGS_JOIN_FIELD_CHANNEL_CHANGE_ACK kPacketAck;
	KERM_JOIN_FIELD_REQ kPacket;

	// 채널이동전에 저장했던 mapid와 진입요청 mapid가 일치하는지 검사
	if( GetMapID() != kPacket_.m_iMapID )
	{
		START_LOG( cerr, L"Map ID mismatch after channel transfer; this should never happen!" )
			<< BUILD_LOG( GetMapID() )
			<< BUILD_LOG( kPacket_.m_iMapID )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
		goto end_proc;
	}

	// 해당 map에 들어갈 수 있는지 조건 체크
	//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
	if( !CheckEnterTheVillage( kPacket_.m_iMapID, iTempLevel, iTempDungeonID ) )
	//}}
	{
		START_LOG( cwarn, L"The requirements for entering town are not met." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iMapID )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::GetLastError();
		goto end_proc;
	}

	if( GetFieldUID() > 0 )
	{
		START_LOG( cerr, L"User is already in a field after channel transfer; this should never happen!" )
			<< BUILD_LOG( kPacket_.m_iMapID )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
		goto end_proc;
	}

	if( !SiCXSLMapData()->VerifyMapID( kPacket_.m_iMapID ) )
	{
		START_LOG( cerr, L"Invalid MapID when attempting to enter the field." )
			<< BUILD_LOG( kPacket_.m_iMapID )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
		goto end_proc;
	}

	//////////////////////////////////////////////////////////////////////////
	// FieldUId를 얻는다.
	if( !SiCXSLMapData()->GetFieldUID( GetMapID(), kPacket.m_iFieldUID ) )
	{
		START_LOG( cerr, L"Invalid map information." )
			<< BUILD_LOG( GetMapID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_FIELD_00; // map정보가 이상합니다.
		goto end_proc;
	}

	// 아직 필드에 입장해있지 않다!
	GetFieldUserInfo( kPacket.m_kFieldUserInfo );
	kPacket.m_kFieldUserInfo.m_vPos = kPacket_.m_vPos;
	kPacket.m_kFieldOption		    = m_kFieldOption;
	if( GetPartyUID() > 0 )
	{
		//{{ 2010. 03. 02  최육사	채널 통합 파티
		KEPM_REQUEST_UPDATE_PARTY_MEMBER_LIST_NOT kPacketToPM;
		kPacketToPM.m_iPartyUID = GetPartyUID();
		SendToCnParty( EPM_REQUEST_UPDATE_PARTY_MEMBER_LIST_NOT, kPacketToPM );
		//}}
	}

	//{{ 2009. 4. 9  최육사		친구리스트
	for( std::map< std::wstring, UidType >::const_iterator mitF = m_mapFriendList.begin(); mitF != m_mapFriendList.end(); ++mitF )
	{
		kPacket.m_setFriendUIDList.insert( mitF->second );
	}
	//}}

	// 입장패킷 보내기전에 미리 FieldUID를 저장한다.
	SetFieldUID( kPacket.m_iFieldUID );
	LIF( GetCharUID() != 0 );
	_SendToGSField( GetFieldUID(), ERM_JOIN_FIELD_CHANNEL_CHANGE_REQ, kPacket );
	return;

end_proc:
	SendPacket( EGS_JOIN_FIELD_CHANNEL_CHANGE_ACK, kPacketAck );
}

_IMPL_ON_FUNC( ERM_JOIN_FIELD_CHANNEL_CHANGE_ACK, KERM_JOIN_FIELD_ACK )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );
	// 상태는 어떻게 되나?

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		LIF( GetFieldUID() == kPacket_.m_iFieldUID );

		// 필드 진입 성공
		SetFieldUID( kPacket_.m_iFieldUID );

		//파티가 있을경우 필드이동 성공시 파티에 업데이트 함.
		SendUpdatePartyUnitInfo( true );

		//{{ 2009. 5. 13  최육사	P2P포트 체크
		if( GetP2PIP().empty() )
		{
			START_LOG( cerr, L"Failed to check P2P port after channel transfer!" )
				<< BUILD_LOG( GetUID() )
				<< END_LOG;
		}
		//}}
	}

	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}

	KEGS_JOIN_FIELD_CHANNEL_CHANGE_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;
	SendPacket( EGS_JOIN_FIELD_CHANNEL_CHANGE_ACK, kPacket );
}
//}}

//{{ 2009. 2. 13  최육사	채널리스트
IMPL_ON_FUNC_NOPARAM( EGS_GET_CHANNEL_LIST_REQ )
{
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	KEGS_GET_CHANNEL_LIST_ACK kPacketAck;
	kPacketAck.m_iServerGroupID = KBaseServer::GetKObj()->GetServerGroupID();
	kPacketAck.m_vecChannelInfo = SiKChannelManager()->GetChannelInfo();
	kPacketAck.m_mapChannelBonusInfo = SiKChannelManager()->GetChannelBonusInfo();
	SendPacket( EGS_GET_CHANNEL_LIST_ACK, kPacketAck );
}
//}}

IMPL_ON_FUNC_NOPARAM( EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ )
{
    KEGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK kPacketAck;
    //kPacketAck.m_iServerGroupID = ( KBaseServer::GetKObj()->GetServerGroupID() == SEnum::SGI_SOLES ? SEnum::SGI_GAIA : SEnum::SGI_SOLES );

    switch( KBaseServer::GetKObj()->GetServerGroupID() )
    {
    case SEnum::SGI_SOLES:
        kPacketAck.m_mapSolesChannelList = SiKChannelManager()->GetChannelMap();
        kPacketAck.m_mapGaiaChannelList = SiKChannelManager()->GetChannelMap2nd();
        kPacketAck.m_mapSolesChannelBonusList = SiKChannelManager()->GetChannelBonusList();
        kPacketAck.m_mapGaiaChannelBonusList = SiKChannelManager()->GetChannelBonusList2nd();
        break;
    case SEnum::SGI_GAIA:
        kPacketAck.m_mapSolesChannelList = SiKChannelManager()->GetChannelMap2nd();
        kPacketAck.m_mapGaiaChannelList = SiKChannelManager()->GetChannelMap();
        kPacketAck.m_mapSolesChannelBonusList = SiKChannelManager()->GetChannelBonusList2nd();
        kPacketAck.m_mapGaiaChannelBonusList = SiKChannelManager()->GetChannelBonusList();
        break;
    default:
        kPacketAck.m_mapSolesChannelList = SiKChannelManager()->GetChannelMap();
        kPacketAck.m_mapGaiaChannelList = SiKChannelManager()->GetChannelMap2nd();
        kPacketAck.m_mapSolesChannelBonusList = SiKChannelManager()->GetChannelBonusList();
        kPacketAck.m_mapGaiaChannelBonusList = SiKChannelManager()->GetChannelBonusList2nd();
    }

    SendPacket( EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK, kPacketAck );

}

IMPL_ON_FUNC_NOPARAM( EGS_DISCONNECT_FOR_SERVER_SELECT_REQ )
{
	// 채널 선택창과 필드, 보안패드 상태에서만 호출 가능
	VERIFY_STATE( ( 3, KGSFSM::S_SERVER_SELECT, KGSFSM::S_FIELD_MAP, KGSFSM::S_CHECK_SECOND_PW ) );

	// 게임중 캐릭선택으로 오는것을 게임종료처리와 같은 상황으로 본다.
	// 유닛이 선택되어 있다면 해당 유닛정보를 DB업데이트, 로그인서버에서 지우기, 나머지 처리를 한다.
	if( GetCharUID() > 0 )
	{
		//{{ 2010. 8. 4	최육사	코드 리팩토링
		//#ifdef SERV_DESELECT_UNIT_REFAC
		//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링
		OnDeselectUnit( EGS_DISCONNECT_FOR_SERVER_SELECT_REQ, false );
		//}}
		//#endif SERV_DESELECT_UNIT_REFAC
		//}}
		return;
	}

	// 로그인서버로 유저정보 빼내러 가자! - 이중접속 방지
	SendDisconnectEvent( EGS_DISCONNECT_FOR_SERVER_SELECT_REQ );
}

IMPL_ON_FUNC_NOPARAM( EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ )
{
	// 필드에서만 호출 가능
	VERIFY_STATE( ( 1, KGSFSM::S_FIELD_MAP ) );

	// 게임중 캐릭선택으로 오는것을 게임종료처리와 같은 상황으로 본다.
	// 유닛이 선택되어 있다면 해당 유닛정보를 DB업데이트, 로그인서버에서 지우기, 나머지 처리를 한다.
	if( GetCharUID() > 0 )
	{
		//{{ 2010. 8. 4	최육사	코드 리팩토링
		//#ifdef SERV_DESELECT_UNIT_REFAC
		//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링
		OnDeselectUnit( EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ, false );
		//}}
		//#endif SERV_DESELECT_UNIT_REFAC
		//}}
		return;
	}

	// 로그인서버로 유저정보 빼내러 가자! - 이중접속 방지
	SendDisconnectEvent( EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ );
}

_IMPL_ON_FUNC( ELG_USER_DISCONNECT_ACK, KELG_USER_DISCONNECT_REQ )
{
	// 상태제한?

	// 채널이동을 위한 종료!
	m_bLogoutFail = false;
	m_bReportDisconnectToLogin = false; // 로그인서버에 유저정보 지우고 왔으므로 OnDestroy()에서 다시 빼내러 가면 안된다!

	// 정상적으로 종료처리 되었음을 알려줌
	u_short usEventIDAck = 0;

	switch( kPacket_.m_usEventID )
	{
	case EGS_DISCONNECT_FOR_SERVER_SELECT_REQ:  usEventIDAck = EGS_DISCONNECT_FOR_SERVER_SELECT_ACK; break;
	case EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ: usEventIDAck = EGS_DISCONNECT_FOR_CHANNEL_CHANGE_ACK; break;
	default:
		START_LOG( cerr, L"Disconnected due to an invalid request event." )
			<< BUILD_LOG( KEvent::GetIDStr( kPacket_.m_usEventID ) )
			<< END_LOG;
	}

	SendID( usEventIDAck );

	// 채널이동으로 인한 접속종료처리
	SetDisconnectReason( KStatistics::eSIColDR_Channel_Change );
	ReserveDestroy();
}
//}}

IMPL_ON_FUNC( EGS_CLIENT_QUIT_REQ )
{
	// 로그인 실패 flag
	m_bLogoutFail = false;

	SendID( EGS_CLIENT_QUIT_ACK );

	// 종료 사유 설정
	int iIndex = KStatistics::eSIColDR_Unknown;

	switch( kPacket_.m_iReason )
	{
	case NetError::ERR_CLIENT_QUIT_00: // 정상종료
		iIndex = KStatistics::eSIColDR_Normal_Disconnect;
		break;

	case NetError::ERR_CLIENT_QUIT_01: // 해킹 감지
		iIndex = KStatistics::eSIColDR_Client_Hacking;
		break;

	case NetError::ERR_CLIENT_QUIT_02: // 클라이언트 크래쉬
		iIndex = KStatistics::eSIColDR_Client_Crash;
		break;

	case NetError::ERR_CLIENT_QUIT_03: // 클라이언트 CheckKom.xml 파싱 오류
		iIndex = KStatistics::eSIColDR_InvlideCheckKomScript;
		break;

	case NetError::ERR_CLIENT_QUIT_04: // 클라이언트 Kom 파일 변조 감지
		iIndex = KStatistics::eSIColDR_InvaildKomDetect;
		break;

	default:
		START_LOG( cerr, L"Invalid disconnect reason statistics index." )
			<< BUILD_LOG( kPacket_.m_iReason )
			<< END_LOG;
		break;
	}

	SetDisconnectReason( iIndex );
	ReserveDestroy();
}

IMPL_ON_FUNC_NOPARAM( E_DISABLE_HB_CHECK_REQ )
{
	START_LOG_WITH_NAME( clog )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( m_bCheckHBTick );

	m_bCheckHBTick = false;
}

IMPL_ON_FUNC_NOPARAM( E_RESERVE_DESTROY )
{
	m_bDestroyReserved = true;
}

//{{ 2011. 08. 10    김민성    채널링 SOAP
//}}


IMPL_ON_FUNC_NOPARAM( E_CHECK_SEQUENCE_COUNT_NOT )
{
	//{{ 2012. 03. 14	김민성		패킷 시퀀스 카운트 체크
	if( m_bCheckSequence == false )
	{
		m_bCheckSequence = true;

		//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
		KEGS_SERVER_CHECK_HACK_USER_REQ kPacket;
		kPacket.m_cHackingCheckType = KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_SEQUENCE_COUNT;
		SendPacket( EGS_SERVER_CHECK_HACK_USER_REQ, kPacket );

		m_kUserHackingManager.SetHackingUserSendType( KEGS_SERVER_CHECK_HACK_USER_REQ::HCT_CHECK_SEQUENCE_COUNT );
		//}}
	}
	//}}
}

//{{ 2012. 03. 29	김민성		중복 sn 접속 감시
//}}

//{{ 2012. 08. 14	박세훈	대천사의 주화 이벤트 가이드 문구 출력
//}}




//{{ 2013. 05. 29	최육사	제재 리스트 통합
IMPL_ON_FUNC( EGB_UPDATE_BLOCK_RESULT_NOT )
{
	VERIFY_STATE( ( 4, KGSFSM::S_UID_UPDATED, KGSFSM::S_LOGINED, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	std::map< int, bool >::const_iterator mit;
	for( mit = kPacket_.m_mapBlockResult.begin(); mit != kPacket_.m_mapBlockResult.end(); ++mit )
	{
		switch( mit->first )
		{
		case KBlockInfo::BT_TRADE_BLOCK:
			{
				m_kUserAbuserManager.SetTradeBlockUser( mit->second );
			}
			break;

		case KBlockInfo::BT_ACCOUNT_BLOCK:
			{
				KEGS_SHOW_DISCONNECT_REASON_NOT kPacket;
				kPacket.m_iOK = NetError::ERR_VERIFY_11;

				SendPacket( EGS_SHOW_DISCONNECT_REASON_NOT, kPacket );

				// 접속 종료 처리
				SetDisconnectReason( KStatistics::eSIColDR_Account_Block );
				ReserveDestroy();
			}
			break;

		case KBlockInfo::BT_IP_BLOCK:
			{
				KEGS_SHOW_DISCONNECT_REASON_NOT kPacket;
				kPacket.m_iOK = NetError::ERR_IP_BLOCK_00;
				SendPacket( EGS_SHOW_DISCONNECT_REASON_NOT, kPacket );

				// 접속 종료 처리
				SetDisconnectReason( KStatistics::eSIColDR_IP_Block );
				ReserveDestroy();
			}
			break;

		case KBlockInfo::BT_DLL_BLOCK:
			{
				// DLL체크
			}
			break;

		default:
			{
				START_LOG( cerr, L"Undefined type." )
					<< BUILD_LOG( mit->first )
					<< BUILD_LOG( mit->second )
					<< END_LOG;
			}
			break;
		}
	}
}
//}}

//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
//}}

//////////////////////////////////////////////////////////////////////////
//#endif SERV_GSUSER_CPP
//////////////////////////////////////////////////////////////////////////
