#include "x2/platform/format.h"
#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"

#include "ODBC/Odbc.h"
#include "Enum/Enum.h"
//{{ 2011. 02. 06	최육사	거래 차단 기능
	#include "GameSysVal/GameSysVal.h"
//}}

//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	#include "SMSPhoneNumberManager.h"
//}}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	#include "RewardTable.h"
//}}

//////////////////////////////////////////////////////////////////////////
#pragma NOTE( "The GSUserHandler.cpp file compiles successfully!" )
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

IMPL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 보상 성공시
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// 접속중인 유저라면 실시간으로 편지 전달!
		KELG_FIND_USER_SEND_LETTER_NOT kPacketNot;
		kPacketNot.m_kLetter = kPacket_.m_iRewardLetter;
		SendToLoginServer( ELG_FIND_USER_SEND_LETTER_NOT, kPacketNot );

		//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
		//}
	}
}

//{{ 2008. 9. 23  최육사	우체국 확장
IMPL_ON_FUNC( EGS_GET_POST_LETTER_LIST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_GET_POST_LETTER_LIST_ACK kPacketAck;

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	if( !m_kUserLetterBox.GetLetterList( kPacket_.m_cLetterListType, kPacket_.m_iPostPageNo, kPacketAck.m_vecPostItemTitle ) )
	//}
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_00;
		SendPacket( EGS_GET_POST_LETTER_LIST_ACK, kPacketAck );
		return;
	}

	kPacketAck.m_iOK = NetError::NET_OK;
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kPacketAck.m_iTotalLetterCount = m_kUserLetterBox.GetTotalLetterCount( kPacket_.m_cLetterListType );
	//}
	SendPacket( EGS_GET_POST_LETTER_LIST_ACK, kPacketAck );
}

IMPL_ON_FUNC( DBE_GET_POST_LETTER_LIST_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KEGS_NEW_POST_LETTER_INFO_NOT kPacket;

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		m_kUserLetterBox.Init( GetThisPtr<KGSUser>(), kPacket_.m_vecPostItem, kPacket.m_iNewUserLetterCount, kPacket.m_iNewSystemLetterCount, kPacket.m_iNewRelationshipLetterCount, kPacket.m_vecNewLetterTitle );
		//}

		m_kUserLetterBox.InitBlackList( kPacket_.m_vecBlackList );

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		kPacket.m_iTotalLetterCount = m_kUserLetterBox.GetTotalLetterCount( KEGS_GET_POST_LETTER_LIST_REQ::LLT_NORMAL );
		//}

		// 클라이언트로 읽지않은 편지 수를 보내준다.
		SendPacket( EGS_NEW_POST_LETTER_INFO_NOT, kPacket );
	}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	std::vector< int >	vecAccessRewardIDList;
	m_kUserUnitSelectRewardManager.GetRewardAndClear( vecAccessRewardIDList );
	if( !vecAccessRewardIDList.empty() )  // 접속시 지급해야할 아이템이 있다면 우편으로 주자
	{
		for( const int iRewardID : vecAccessRewardIDList )
		{
			// 이벤트 보상을 주자!
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = iRewardID;
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
	}
	//}}
}
//}}

//{{ 2012. 10. 08	박세훈	우편 로직 변경
//{{ 2008. 9. 8  최육사		편지 보내기 열기
IMPL_ON_FUNC( EGS_SEND_LETTER_REQ )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	bool bFreeMail = false;
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_SEND_LETTER_REQ, EGS_SEND_LETTER_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_SEND_LETTER_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_SEND_LETTER_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	KDBE_PREPARE_INSERT_LETTER_TO_POST_REQ kPacketToDB;
	KEGS_SEND_LETTER_ACK kPacketAck;

	//{{ 2011. 02. 06	최육사	거래 차단 기능
	if( SiKGameSysVal()->IsBlockSendLetter() == true )
	{
		KEGS_SEND_LETTER_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_BLOCK_TRADE_00;
		SendPacket( EGS_SEND_LETTER_ACK, kPacket );
		return;
	}
	//}}

	//{{ 2011. 03. 02	최육사	개인 거래 차단
	if( m_kUserAbuserManager.IsTradeBlockUser() == true )
	{
		KEGS_SEND_LETTER_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_TRADE_BLOCK_UNIT_01;
		SendPacket( EGS_SEND_LETTER_ACK, kPacket );
		return;
	}
	//}}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		KEGS_SEND_LETTER_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_TRADE_BLOCK_UNIT_01;
		SendPacket( EGS_SEND_LETTER_ACK, kPacket );
		return;
	}

	// 1. 체험ID 기능 제한
	if( IsGuestUser() )
	{
		KEGS_SEND_LETTER_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_GUEST_USER_00;
		SendPacket( EGS_SEND_LETTER_ACK, kPacket );
		return;
	}

	// 2. 같은 캐릭터에는 편지를 보낼 수 없다.
	if( kPacket_.m_wstrToNickName == GetCharName() )
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_11;
		goto end_proc;
	}

	//{{ 2009. 3. 24  최육사	GM체크
	if( kPacket_.m_wstrToNickName == NetError::GetErrStr( NetError::STR_GM_NICKNAME_1 )  ||
		kPacket_.m_wstrToNickName == NetError::GetErrStr( NetError::STR_GM_NICKNAME_2 ) )
	{
		kPacketAck.m_iOK = NetError::ERR_GM_CHAR_DENY_00;
		goto end_proc;
	}
	//}}

	// 3. 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrToNickName ) )
	{
		kPacketAck.m_iOK = NetError::ERR_ODBC_00;
		goto end_proc;
	}

	if( KODBC::IsInvalidMarkInForLetter( kPacket_.m_wstrTitle ) )
	{
		kPacketAck.m_iOK = NetError::ERR_ODBC_00;
		goto end_proc;
	}

	if( KODBC::IsInvalidMarkInForLetter( kPacket_.m_wstrMessage ) )
	{
		kPacketAck.m_iOK = NetError::ERR_ODBC_00;
		goto end_proc;
	}

	//{{ 2008. 10. 27  최육사	GM메시지 추가
	if( kPacket_.m_bIsGmMessage )
	{
		if( GetAuthLevel() < SEnum::UAL_GM )
		{
			START_LOG( cerr, L"운영자도 아닌데 감히" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOGc( GetAuthLevel() )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_VERIFY_12;
			goto end_proc;
		}
	}
	//}}

	// 4. 글자 길이 검사
	{
		std::string strTitle = KncUtil::toNarrowString( kPacket_.m_wstrTitle );
		if( strTitle.size() > 36 )
		{
			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_18;
			goto end_proc;
		}
	}

	{
		std::string strMessage = KncUtil::toNarrowString( kPacket_.m_wstrMessage );
		if( strMessage.size() > 400 )
		{
			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_19;
			goto end_proc;
		}
	}

	for( int iFreeMailTicketIDIndex = 0;
		iFreeMailTicketIDIndex < sizeof( _CONST_FREE_MAIL_TICKET_::arriFreeMailTicketID ) / sizeof( _CONST_FREE_MAIL_TICKET_::arriFreeMailTicketID[0] );
		++iFreeMailTicketIDIndex )
	{
		if( m_kInventory.IsExist( _CONST_FREE_MAIL_TICKET_::arriFreeMailTicketID[iFreeMailTicketIDIndex], true ) == true )
		{
			bFreeMail = true;
			break;
		}
	}

	// 5. 첨부물에 대한 검증 작업
	if( kPacket_.m_iED > 0  ||  kPacket_.m_iItemUID > 0 )
	{
		if( kPacket_.m_iED <= 0 )
		{
			// 5-1. 해당 아이템의 인벤정보 얻기
			KInventoryItemInfo kInvenItemInfo;
			if( !m_kInventory.GetInventoryItemInfo( kPacket_.m_iItemUID, kInvenItemInfo ) )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_07;
				goto end_proc;
			}

			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kInvenItemInfo.m_kItemInfo.m_iItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"해당 아이템의 템플릿 정보를 얻지 못하였습니다." )
					<< BUILD_LOG( kPacket_.m_iItemUID )
					<< BUILD_LOG( kInvenItemInfo.m_kItemInfo.m_iItemID )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_07;
				goto end_proc;
			}

			if( kInvenItemInfo.m_kItemInfo.m_sEndurance < pItemTemplet->m_Endurance )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_08;
				goto end_proc;
			}

			//{{ 2008. 10. 16  최육사	수량 검사
			if( kPacket_.m_iQuantity <= 0  ||  kInvenItemInfo.m_kItemInfo.m_iQuantity < kPacket_.m_iQuantity )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_01;
				goto end_proc;
			}
			//}}

			//{{ 2010. 01. 04  최육사	PC방 프리미엄

			if( m_kInventory.IsDBUpdateItem( kPacket_.m_iItemUID ) == false )
			{
				START_LOG( cerr, L"PC방 프리미엄 아이템을 우편에 첨부하려고 했습니다." )
					<< BUILD_LOG( kPacket_.m_iItemUID )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_PC_BANG_00;
				goto end_proc;
			}

			//}}

			if( pItemTemplet->m_bVested )
			{
				//{{ 2009. 8. 27  최육사	귀속이지만 밀봉이면 우편전송가능
				if( kInvenItemInfo.m_kItemInfo.IsSealedItem() == false )
				{
					kPacketAck.m_iOK = NetError::ERR_POST_LETTER_16;
					goto end_proc;
				}
				//}}
			}

			//{{ 2009. 1. 22  최육사	기간제 아이템은 편지에 첨부할수 없도록 예외처리
			if( kInvenItemInfo.m_kItemInfo.m_cUsageType == CXSLItem::PT_INFINITY )
			{
				if( kInvenItemInfo.m_kItemInfo.m_sPeriod > 0 )
				{
					kPacketAck.m_iOK = NetError::ERR_POST_LETTER_16;
					goto end_proc;
				}
			}
			//}}

			//{{ 2008. 12. 21  최육사	사용불가 검사
			if( kInvenItemInfo.m_kItemInfo.m_cEnchantLevel < 0 )
			{
				kPacketAck.m_iOK = NetError::ERR_RESTORE_ITEM_06;
				goto end_proc;
			}
			//}}

			if( m_kInventory.IsShareItem(kInvenItemInfo.m_iItemUID) == true )
			{
				START_LOG(cerr, L"은행 공유 : 은행 공유 아이템을 전송 하려 했음.")
					<< BUILD_LOG(kInvenItemInfo.m_iItemUID)
					<< BUILD_LOG(GetName())
					<< END_LOG;

				//	통계 전송 추가
				CTime tCurTime = CTime::GetCurrentTime();

				KELOG_ITEM_TRADE_ERROR_NOT kErrorNot;
				kErrorNot.m_iUnitUID = GetCharUID();
				kErrorNot.m_iType = SEnum::TET_POST;
				kErrorNot.m_wsrtRegDate = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );

				SendToLogDB( ELOG_ITEM_TRADE_ERROR_NOT, kErrorNot );

				kPacketAck.m_iOK = NetError::ERR_SHARE_BANK_01;
				goto end_proc;
			}

			// 5-2. 수수료 계산
			if( bFreeMail == true )
				kPacketToDB.m_iSendLetterCost = 0;
			else
			kPacketToDB.m_iSendLetterCost = static_cast<int>( ( ( ( pItemTemplet->m_Price * kPacket_.m_iQuantity ) * 0.2f ) * 0.05f ) + 200 );

			if( GetED() < kPacketToDB.m_iSendLetterCost )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_12;
				goto end_proc;
			}

			// 5-3. 해당 아이템정보 보관
			kPacketToDB.m_kSendLetter.m_iScriptIndex	= kInvenItemInfo.m_kItemInfo.m_iItemID;
			kPacketToDB.m_kSendLetter.m_iQuantity		= kPacket_.m_iQuantity;
			kPacketToDB.m_kSendLetter.m_cEnchantLevel	= kInvenItemInfo.m_kItemInfo.m_cEnchantLevel;
			kPacketToDB.m_kSendLetter.m_kAttribEnchantInfo = kInvenItemInfo.m_kItemInfo.m_kAttribEnchantInfo;
			kPacketToDB.m_kSendLetter.m_vecItemSocket	= kInvenItemInfo.m_kItemInfo.m_vecItemSocket;
			kPacketToDB.m_kSendLetter.m_byteExpandedSocketNum	= kInvenItemInfo.m_kItemInfo.m_byteExpandedSocketNum;
			//{{ 2013. 06. 04	최육사	아이템 개편
			kPacketToDB.m_kSendLetter.m_vecRandomSocket	= kInvenItemInfo.m_kItemInfo.m_vecRandomSocket;
			//}}
			//{{ 2009. 8. 27  최육사	밀봉
			kPacketToDB.m_kSendLetter.m_ucSealData		= kInvenItemInfo.m_kItemInfo.m_ucSealData;
			if( kInvenItemInfo.m_kItemInfo.IsSealedItem() )
			{
				kPacketToDB.m_kSendLetter.m_ucSealData -= 100; // 밀봉해제
			}
			//}}

			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			const int iBeforeED = m_iED;

			m_iED -= kPacketToDB.m_iSendLetterCost;

			//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
			//#ifdef SERV_USER_ABUSER_MANAGER
			// ED 어뷰저
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_COST, iBeforeED );
			//#endif SERV_USER_ABUSER_MANAGER
			//}}
			//}}
		}
		else
		{
			// 5-1. 편지 수수료 검증
			if( bFreeMail == true )
				kPacketToDB.m_iSendLetterCost = 0;
			else
			kPacketToDB.m_iSendLetterCost = static_cast<int>( ( kPacket_.m_iED * 0.05f ) + 200 );

			if( GetED() < kPacketToDB.m_iSendLetterCost )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_12;
				goto end_proc;
			}

			// 5-2. 첨부가능한 액수인지 검증
			if( kPacket_.m_iED > SEnum::UI_MAX_LETTER_ED )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_02;
				goto end_proc;
			}

			// 5-3. 수수료 + 첨부ED검증
			if( GetED() < ( kPacket_.m_iED + kPacketToDB.m_iSendLetterCost ) )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_02;
				goto end_proc;
			}

			kPacketToDB.m_kSendLetter.m_iScriptIndex = CXSLItem::EDI_BRONZE_ED; // 브론즈 ED
			kPacketToDB.m_kSendLetter.m_iQuantity	 = kPacket_.m_iED;

			//{{ 우편 ED 거래시 ED 동기화 - 김민성

			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			int iBeforeED = m_iED;

			iBeforeED -= kPacket_.m_iED;

			// ED 어뷰저
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_SEND_ED, iBeforeED );

			iBeforeED -= kPacketToDB.m_iSendLetterCost;

			// ED 어뷰저
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_COST, iBeforeED );
			//}}

			KDBE_SYNC_ED_REQ kDBReq;
			kDBReq.m_bIsGmMessage = kPacket_.m_bIsGmMessage;
			kDBReq.m_wstrToNickName = kPacket_.m_wstrToNickName;
			kDBReq.m_wstrTitle = kPacket_.m_wstrTitle;
			kDBReq.m_wstrMessage = kPacket_.m_wstrMessage;
			kDBReq.m_iED = kPacket_.m_iED;
			kDBReq.m_iItemUID = kPacket_.m_iItemUID;
			kDBReq.m_iQuantity = kPacket_.m_iQuantity;
			kDBReq.m_bIsGmMessage = kPacket_.m_bIsGmMessage;

			kDBReq.m_iFromUnitUID = GetCharUID();
			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			kDBReq.m_iChangeED = m_iED.GetChangeValue();
			//}}
			kDBReq.m_iSendLetterCost = kPacketToDB.m_iSendLetterCost;

			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			m_iED.SetValue( GetED() - kPacket_.m_iED - kPacketToDB.m_iSendLetterCost );
			//}}

			SendToGameDB( DBE_SYNC_ED_REQ, kDBReq );
			return;
			//}}
		}
	}
	else
	{
		// 편지 수수료
		if( bFreeMail == true )
			kPacketToDB.m_iSendLetterCost = 0;
		else
		kPacketToDB.m_iSendLetterCost = 200;

		if( GetED() < kPacketToDB.m_iSendLetterCost )
		{
			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_12;
			goto end_proc;
		}

		//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
		const int iBeforeED = m_iED;

		m_iED -= kPacketToDB.m_iSendLetterCost;

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		//#ifdef SERV_USER_ABUSER_MANAGER
		// ED 어뷰저
		m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_COST, iBeforeED );
		//#endif SERV_USER_ABUSER_MANAGER
		//}}
		//}}
	}

	// 6. DB로 업데이트 체크
	kPacketToDB.m_iFromUnitUID				= GetCharUID();
	kPacketToDB.m_iItemUID					= kPacket_.m_iItemUID;
	kPacketToDB.m_wstrToNickName			= kPacket_.m_wstrToNickName;
	kPacketToDB.m_kSendLetter.m_cScriptType = ( kPacket_.m_bIsGmMessage ? KPostItemInfo::LT_MESSAGE : KPostItemInfo::LT_POST_OFFICE );
	kPacketToDB.m_kSendLetter.m_wstrTitle	= kPacket_.m_wstrTitle;
	kPacketToDB.m_kSendLetter.m_wstrMessage = kPacket_.m_wstrMessage;
	kPacketToDB.m_kSendLetter.m_iFromUnitUID = GetCharUID();
	kPacketToDB.m_kSendLetter.m_wstrFromNickName = GetCharName();

	SendToGameDB( DBE_PREPARE_INSERT_LETTER_TO_POST_REQ, kPacketToDB );
	return;

end_proc:
	SendPacket( EGS_SEND_LETTER_ACK, kPacketAck );
}

IMPL_ON_FUNC( DBE_PREPARE_INSERT_LETTER_TO_POST_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
//#ifdef SERV_ADD_REPEAT_FILTER
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_SEND_LETTER_ACK );
//#else
//	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );
//#endif SERV_ADD_REPEAT_FILTER
	//}}

	KDBE_INSERT_LETTER_TO_POST_REQ kPacketToDB;
	KEGS_SEND_LETTER_ACK kPacketAck;

	if( kPacket_.m_iOK != NetError::NET_OK )
		goto end_proc;

	if( kPacket_.m_kSendLetter.m_iScriptIndex > 0  &&  kPacket_.m_kSendLetter.m_iScriptIndex != CXSLItem::EDI_BRONZE_ED )
	{
		// 1. 해당 아이템의 인벤정보 얻기
		KInventoryItemInfo kInvenItemInfo;
		if( !m_kInventory.GetInventoryItemInfo( kPacket_.m_iItemUID, kInvenItemInfo ) )
		{
			START_LOG( cerr, L"send letter에서 검사를 했는데.. 아이템이 존재 하지 않는다? 시간차인가.." )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_07;
			goto end_proc;
		}

		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kInvenItemInfo.m_kItemInfo.m_iItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"해당 아이템의 템플릿 정보를 얻지 못하였습니다." )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< BUILD_LOG( kInvenItemInfo.m_kItemInfo.m_iItemID )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_07;
			goto end_proc;
		}

		// 2. 해당 아이템의 수량 감소
		if( pItemTemplet->m_PeriodType == CXSLItem::PT_QUANTITY )
		{
			int iDecreased = 0;
			m_kInventory.DecreaseQuantity( kPacket_.m_iItemUID, kPacket_.m_kSendLetter.m_iQuantity, iDecreased, KDeletedItemInfo::DR_LETTER );
			if( kPacket_.m_kSendLetter.m_iQuantity != iDecreased )
			{
				START_LOG( cerr, L"수량 변화 이상." )
					<< BUILD_LOG( kPacket_.m_iItemUID )
					<< BUILD_LOG( kPacket_.m_kSendLetter.m_iQuantity )
					<< BUILD_LOG( iDecreased )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_01;
				goto end_proc;
			}
		}
		else
		{
			KInventoryItemInfo kDelInvenItem;
			if( !m_kInventory.DeleteItem( kPacket_.m_iItemUID, kDelInvenItem, KDeletedItemInfo::DR_LETTER ) )
			{
				START_LOG( cerr, L"아이템 삭제 실패." )
					<< BUILD_LOG( kPacket_.m_iItemUID )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_01;
				goto end_proc;
			}
		}

		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( kInvenItemInfo.m_kItemInfo.m_iItemID );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_POST, kInvenItemInfo.m_kItemInfo.m_iQuantity );
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_POST, kInvenItemInfo.m_kItemInfo.m_iQuantity );
		//}}

		// 3. DB업데이트 준비
		KInventoryItemInfo kResultItemInfo;
		//{{ 2011. 12. 15	최육사	인벤토리 슬롯ID크기 늘리기
//#ifdef SERV_EXPAND_SLOT_ID_DATA_SIZE
		m_kInventory.GetInventorySlotInfo( kInvenItemInfo.m_cSlotCategory, kInvenItemInfo.m_sSlotID, kResultItemInfo );
//#else
//		m_kInventory.GetInventorySlotInfo( kInvenItemInfo.m_cSlotCategory, kInvenItemInfo.m_cSlotID, kResultItemInfo );
//#endif SERV_EXPAND_SLOT_ID_DATA_SIZE
		//}}
		kPacketToDB.m_vecUpdatedInventorySlot.push_back( kResultItemInfo );

		m_kInventory.FlushQuantityChange( kPacketToDB.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.FlushDeletedItem( kPacketToDB.m_kItemQuantityUpdate.m_vecDeleted );
	}

	// 4. DB 업데이트
	kPacketToDB.m_iFromUnitUID				= GetCharUID();
	kPacketToDB.m_wstrToNickName			= kPacket_.m_wstrToNickName;
	kPacketToDB.m_kSendLetter				= kPacket_.m_kSendLetter;
	kPacketToDB.m_iSendLetterCost			= kPacket_.m_iSendLetterCost;
	SendToGameDB( DBE_INSERT_LETTER_TO_POST_REQ, kPacketToDB );
	return;

end_proc:

	// ED 롤백
	if( kPacket_.m_kSendLetter.m_iScriptIndex == CXSLItem::EDI_BRONZE_ED )
	{
		//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
		const int iBeforeED = m_iED;

		m_iED += kPacket_.m_kSendLetter.m_iQuantity;

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		//#ifdef SERV_USER_ABUSER_MANAGER
		// ED 어뷰저
		m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_ED_ROLLBACK, iBeforeED );
		//#endif SERV_USER_ABUSER_MANAGER
		//}}
		//}}
	}

	//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
	const int iBeforeED2 = m_iED;

	m_iED += kPacket_.m_iSendLetterCost;

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	//#ifdef SERV_USER_ABUSER_MANAGER
	// ED 어뷰저
	m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_COST_ROLLBACK, iBeforeED2 );
	//#endif SERV_USER_ABUSER_MANAGER
	//}}
	//}}

	kPacketAck.m_iOK = kPacket_.m_iOK;
	kPacketAck.m_iED = GetED();
	SendPacket( EGS_SEND_LETTER_ACK, kPacketAck );

	//{{ 2012. 09. 26	최육사		우편 복사 버그 수정
	if( kPacket_.m_iOK == NetError::ERR_POST_LETTER_20 )
	{
		START_LOG( cout, L"이미 게임서버에서 동일닉네임 체크 했는데 DB체크까지 갔다온 시도가 있다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetUserID() )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_wstrToNickName );

		KDBE_SEND_PHONE_MSG_NOT kPacketNot;
		kPacketNot.m_wstrSMSMessage = x2::str( x2::wformat( L"동일닉네임 캐릭터에게 우편전송을 시도한 유저 발견! : UserUID(%d), NickName(%s), ToNickName(%s)" ) % GetUID() % GetCharName() % kPacket_.m_wstrToNickName );
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
}

IMPL_ON_FUNC( EGS_GET_ITEM_FROM_LETTER_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_GET_ITEM_FROM_LETTER_REQ, EGS_GET_ITEM_FROM_LETTER_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_GET_ITEM_FROM_LETTER_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"인벤토리 락이 걸려있는 상태입니다!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_GET_ITEM_FROM_LETTER_ACK kPacketAck;
	KDBE_GET_ITEM_FROM_LETTER_REQ kPacketToDB;
	std::map< int, KItemInfo > mapInsertItem;
	kPacketToDB.m_iED = 0;

	KPostItemInfo kPostItemInfo;
	if( m_kUserLetterBox.GetLetter( kPacket_.m_iPostNo, kPostItemInfo ) == false )
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_04;
		SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
		return;
	}

	if( kPostItemInfo.m_iScriptIndex <= 0 )
	{
		START_LOG( cerr, L"클라이언트에서 첨부물이 비어있는걸 검사했을텐데.." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iPostNo )
			<< BUILD_LOG( kPostItemInfo.m_iScriptIndex )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_06;
		SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
		return;
	}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kPacketToDB.m_cLetterType = m_kUserLetterBox.GetLetterType( kPacket_.m_iPostNo );
	//}

	//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
	if( m_kUserLetterBox.IsItemIDLetter( kPacket_.m_iPostNo ) == false )
	//}}
	{
		// 보상 아이템의 ItemInfo 얻기

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		SiKRewardTable()->GetRewardItem( kPostItemInfo, mapInsertItem );
		//}}
	}
	else
	{
		// 수량 검사
		if( kPostItemInfo.m_iQuantity <= 0 )
		{
			START_LOG( cerr, L"클라이언트에서 첨부물이 비어있는걸 검사했을텐데.." )
				<< BUILD_LOG( kPacket_.m_iPostNo )
				<< BUILD_LOG( kPostItemInfo.m_iScriptIndex )
				<< BUILD_LOG( kPostItemInfo.m_iQuantity )
				<< END_LOG;

			kPacketAck.m_iOK = NetError::ERR_POST_LETTER_06;
			SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
			return;
		}

		// 첨부된 아이템이 ED일 경우..
		if( kPostItemInfo.m_iScriptIndex == CXSLItem::EDI_BRONZE_ED )
		{
			//{{ 2009. 4. 28  최육사	최대 ED체크
			if( CheckMaxED( kPostItemInfo.m_iQuantity ) == false )
			{
				START_LOG( cwarn, L"최대ED를 초과함! 클라에서 체크했을텐데.." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( GetED() )
					<< BUILD_LOG( kPostItemInfo.m_iQuantity )
					<< END_LOG;

				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_06;
				SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
				return;
			}
			//}}
			kPacketToDB.m_iED = kPostItemInfo.m_iQuantity;

			//{{ 2011. 04. 05  김민성	우편 및 거래 감시
			if( SiKGameSysVal()->GetLetterAndTradeMonitoringIsOn() == true )
			{
				if( m_kUserHackingManager.AddLetterMonitoringList( kPostItemInfo.m_iFromUnitUID ) == true )
				{
					KDBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ PacketReq;
					CTime tCurTime = CTime::GetCurrentTime();

					PacketReq.m_iRecvUnitUID = GetCharUID();
					PacketReq.m_wstrRegDate = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
					PacketReq.m_cType = 0;

					SendToLogDB( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ, PacketReq );
				}
			}
			//}}
		}
		else
		{
			KItemInfo kInsertItemInfo;
			if( !m_kUserLetterBox.GetItemInfo( kPacket_.m_iPostNo, kInsertItemInfo ) )
			{
				kPacketAck.m_iOK = NetError::ERR_POST_LETTER_04;
				SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
				return;
			}

			mapInsertItem.insert( std::make_pair( kInsertItemInfo.m_iItemID, kInsertItemInfo ) );
		}
	}

	// 인벤토리 검사
	if( m_kInventory.PrepareInsert( mapInsertItem, kPacketToDB.m_mapInsertedItem, kPacketToDB.m_vecUpdatedInventorySlot, kPacketToDB.m_vecItemInfo, true ) == false )
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_05;
		SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacketAck );
		return;
	}

	// [참고] 보상아이템이 수량만 증가하더라도 일단 무조건 DB로 보낸다.
	// 우체국 테이블의 아이템들을 삭제하기 위해서.

	kPacketToDB.m_iUnitUID	    = GetCharUID();
	kPacketToDB.m_iPostNo	    = kPacket_.m_iPostNo;
	SendToGameDB( DBE_GET_ITEM_FROM_LETTER_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_GET_ITEM_FROM_LETTER_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_GET_ITEM_FROM_LETTER_ACK );
	//}}

	KEGS_GET_ITEM_FROM_LETTER_ACK kPacket;
	kPacket.m_iOK			= kPacket_.m_iOK;
	kPacket.m_iPostNo		= kPacket_.m_iPostNo;
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kPacket.m_cLetterType = kPacket_.m_cLetterType;
	//}
	kPacket.m_mapResultItem = kPacket_.m_mapInsertedItem;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		// ED첨부물 받기
		if( kPacket_.m_iED > 0 )
		{
			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			const int iBeforeED = m_iED;

			m_iED += kPacket_.m_iED;

			//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
			//#ifdef SERV_USER_ABUSER_MANAGER
			// ED 어뷰저
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_RECV_ED, iBeforeED );
			//#endif SERV_USER_ABUSER_MANAGER
			//}}
			//}}

			KStatisticsKey kKey;
			kKey.m_vecIntKey.push_back( 0 );
			KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_PPostLetter, kPacket_.m_iED );
		}

		// 아이템 첨부물 받기
		m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kPacket.m_vecKInventorySlotInfo );

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		switch( kPacket_.m_cLetterType )
		{
		case KPostItemInfo::LT_POST_OFFICE:
			{
				// 유저 편지라면 첨부물 초기화 한다..
				m_kUserLetterBox.SetReceivedFlag( kPacket_.m_iPostNo );
			}
			break;
		default:
			{
				// 시스템 편지라면 지우고..
				m_kUserLetterBox.DeleteLetter( kPacket_.m_iPostNo );
			}
		}
		//}

		//{{ 2013. 10. 18	박세훈	스킬슬롯체인지 체크(인벤토리-기간제) 기능 구현
		for( std::map<int, int>::const_iterator it = kPacket_.m_mapInsertedItem.begin(); it != kPacket_.m_mapInsertedItem.end(); ++it )
		{
			const int iItemID = it->first;
			switch( iItemID )
			{
			case CXSLItem::CI_EXPAND_SKILL_SLOT_IN_PACKAGE:
				{
					// 무제한 기준 (2049-12-31 23:59) 으로 기간 셋팅 되어 있음
					CTime tLimitEndDate = CTime( 2049, 12, 31, 23, 59, 0 );
					CTime tSkillSlotBEndDate;
					m_kSkillTree.GetSkillSolotBEndDate( tSkillSlotBEndDate );
					if( tLimitEndDate == tSkillSlotBEndDate )
					{
						// 스킬 슬롯 무제한 구매했었다면 갱신 할 필요 없다.
						break;
					}

					std::wstring wstrSkillSlotBEndDate;
					if( m_kInventory.GetSlotChangeBEndDate( iItemID, wstrSkillSlotBEndDate ) )
					{
						m_kSkillTree.SetSkillSolotBEndDate( wstrSkillSlotBEndDate );

						KEGS_SKILL_SLOT_CHANGE_ITEM_NOT kpacket;

						kpacket.m_cSkillSlotBExpirationState = KUserSkillTree::SSBES_NOT_EXPIRED;
						kpacket.m_wstrSkillSlotBEndDate = wstrSkillSlotBEndDate;

						SendPacket( EGS_SKILL_SLOT_CHANGE_ITEM_NOT, kpacket );
						break;
					}
				}
				break;
			}
		}
		//}}

		kPacket.m_vecKInventorySlotInfo.insert( kPacket.m_vecKInventorySlotInfo.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );
	}

	kPacket.m_iED = GetED();

	SendPacket( EGS_GET_ITEM_FROM_LETTER_ACK, kPacket );

	//////////////////////////////////////////////////////////////////////////
	// 보상 아이템에 대한 통계
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	if( kPacket_.m_cLetterType != KPostItemInfo::LT_POST_OFFICE )
		//}
	{
		KStatisticsKey kKey;
		std::map< int, int >::const_iterator mitStat;
		for( mitStat = kPacket_.m_mapInsertedItem.begin(); mitStat != kPacket_.m_mapInsertedItem.end(); ++mitStat )
		{
			kKey.m_vecIntKey.clear();
			kKey.m_vecIntKey.push_back( mitStat->first );
			KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_DROP, mitStat->second );
			//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
			KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_DROP, mitStat->second );
			//}}
		}
	}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	// 아이템 어뷰저
	m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_LETTER, kPacket_.m_mapInsertedItem );
	//}}
}

//}}

IMPL_ON_FUNC( DBE_INSERT_LETTER_TO_POST_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_SEND_LETTER_ACK );
	//}}

	KEGS_SEND_LETTER_ACK kPacket;
	kPacket.m_iOK = kPacket_.m_iOK;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		// ED 롤백
		if( kPacket_.m_kSendLetter.m_iScriptIndex == CXSLItem::EDI_BRONZE_ED )
		{
			//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
			const int iBeforeED = m_iED;

			m_iED += kPacket_.m_kSendLetter.m_iQuantity;

			//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
			//#ifdef SERV_USER_ABUSER_MANAGER
			// ED 어뷰저
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_ED_ROLLBACK, iBeforeED );
			//#endif SERV_USER_ABUSER_MANAGER
			//}}
			//}}
		}

		//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
		const int iBeforeED2 = m_iED;

		m_iED += kPacket_.m_iSendLetterCost;

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		//#ifdef SERV_USER_ABUSER_MANAGER
		// ED 어뷰저
		m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_LETTER_COST_ROLLBACK, iBeforeED2 );
		//#endif SERV_USER_ABUSER_MANAGER
		//}}
		//}}
	}

	// 우편 삽입 sp가 실패 했을 경우에도 일단 아이템을 삭제 처리하도록 한다. ( 복구 이슈 발생 )
	// RollBack 함수를 호출한다고 해도 바로 삭제되지는 않으며,
	// 만의 하나 삭제하지 못한 상태에서 서버 다운이 발생하는 것은 불가항력으로 여긴다.

	kPacket.m_iED = GetED();
	kPacket.m_vecKInventorySlotInfo = kPacket_.m_vecUpdatedInventorySlot;
	m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
	m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
	SendPacket( EGS_SEND_LETTER_ACK, kPacket );

	// 받는 상대방이 접속해 있다면 실시간으로 편지 전해주기
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KELG_FIND_USER_SEND_LETTER_NOT kPacketNot;
		kPacketNot.m_kLetter = kPacket_.m_kSendLetter;
		SendToLoginServer( ELG_FIND_USER_SEND_LETTER_NOT, kPacketNot );

		//////////////////////////////////////////////////////////////////////////
		// ED통계
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( 0 );

		// 첨부 ED
		if( kPacket_.m_kSendLetter.m_iScriptIndex == CXSLItem::EDI_BRONZE_ED )
		{
			KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MPostLetter, kPacket_.m_kSendLetter.m_iQuantity );
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_MPostLetter, kPacket_.m_kSendLetter.m_iQuantity );
		}

		KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MSendLetterBasic, 200 );
		KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MSendLetterItem, ( kPacket_.m_iSendLetterCost - 200 ) );
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_MSendLetterBasic, 200 );
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EDData, 0, KUserStatistics::US_ED_MSendLetterItem, ( kPacket_.m_iSendLetterCost - 200 ) );
		//////////////////////////////////////////////////////////////////////////
	}
}

IMPL_ON_FUNC( ELG_FIND_USER_SEND_LETTER_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 우편함에 편지 넣기
	m_kUserLetterBox.AddLetter( kPacket_.m_kLetter );

	KEGS_RECEIVE_LETTER_NOT kPacket;
	kPacket.m_wstrNickName	  = kPacket_.m_kLetter.m_wstrFromNickName;
	kPacket.m_wstrLetterTitle = kPacket_.m_kLetter.m_wstrTitle;
	kPacket.m_cScriptType	  = kPacket_.m_kLetter.m_cScriptType;
	kPacket.m_iScriptIndex	  = kPacket_.m_kLetter.m_iScriptIndex;
	//{{ 2011. 09. 05  김민성	우편 및 거래 감시
	kPacket.m_iFromUnitUID	  = kPacket_.m_kLetter.m_iFromUnitUID;
	//}}
	SendPacket( EGS_RECEIVE_LETTER_NOT, kPacket );
}

IMPL_ON_FUNC( EGS_READ_LETTER_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_READ_LETTER_ACK kPacketAck;
	bool bReadUpdated = false;

	if( !m_kUserLetterBox.ReadLetter( kPacket_.m_iPostNo, kPacketAck.m_kLetter, bReadUpdated ) )
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_04;
		SendPacket( EGS_READ_LETTER_ACK, kPacketAck );
		return;
	}

	kPacketAck.m_iOK = NetError::NET_OK;
	SendPacket( EGS_READ_LETTER_ACK, kPacketAck );

	// DB Update
	if( !bReadUpdated )
	{
		KDBE_READ_LETTER_NOT kPacketToDB;
		kPacketToDB.m_iUnitUID = GetCharUID();
		kPacketToDB.m_iPostNo  = kPacket_.m_iPostNo;
		SendToGameDB( DBE_READ_LETTER_NOT, kPacketToDB );
	}
}

IMPL_ON_FUNC( EGS_DELETE_LETTER_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_DELETE_LETTER_ACK kPacketAck;

	if( !m_kUserLetterBox.IsExist( kPacket_.m_vecPostNo ) )
	{
		kPacketAck.m_iOK = NetError::ERR_POST_LETTER_14;
		SendPacket( EGS_DELETE_LETTER_ACK, kPacketAck );
		return;
	}

	// 편지 삭제
	m_kUserLetterBox.DeleteLetter( kPacket_.m_vecPostNo );

	kPacketAck.m_iOK = NetError::NET_OK;
	kPacketAck.m_vecPostNo = kPacket_.m_vecPostNo;
	SendPacket( EGS_DELETE_LETTER_ACK, kPacketAck );

	// DB Update
	KDBE_DELETE_LETTER_NOT kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();
	kPacketToDB.m_vecPostNo = kPacket_.m_vecPostNo;
	SendToGameDB( DBE_DELETE_LETTER_NOT, kPacketToDB );
}
//}}

//{{ 2008. 9. 26  최육사	우체국 블랙리스트
IMPL_ON_FUNC_NOPARAM( EGS_GET_POST_BLACK_LIST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_GET_POST_BLACK_LIST_ACK kPacket;
	m_kUserLetterBox.GetBlackList( kPacket.m_vecBlackList );
	SendPacket( EGS_GET_POST_BLACK_LIST_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_NEW_POST_BLACK_LIST_REQ )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_NEW_POST_BLACK_LIST_REQ, EGS_NEW_POST_BLACK_LIST_ACK );
	//}}

	// SQL Injection 방지
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
	{
		KEGS_NEW_POST_BLACK_LIST_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_NEW_POST_BLACK_LIST_ACK, kPacket );
		return;
	}

	if( m_kUserLetterBox.IsBlackList( kPacket_.m_wstrNickName ) )
	{
		KEGS_NEW_POST_BLACK_LIST_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_POST_BLACK_LIST_01;
		SendPacket( EGS_NEW_POST_BLACK_LIST_ACK, kPacket );
		return;
	}

	KDBE_NEW_POST_BLACK_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();
	kPacketToDB.m_wstrNickName = kPacket_.m_wstrNickName;
	SendToGameDB( DBE_NEW_POST_BLACK_LIST_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_NEW_POST_BLACK_LIST_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_NEW_POST_BLACK_LIST_ACK );
	//}}

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_kUserLetterBox.AddBlackList( kPacket_.m_wstrNickName );
	}

	KEGS_NEW_POST_BLACK_LIST_ACK kPacket;
	kPacket.m_iOK		   = kPacket_.m_iOK;
	kPacket.m_wstrNickName = kPacket_.m_wstrNickName;
	SendPacket( EGS_NEW_POST_BLACK_LIST_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_DEL_POST_BLACK_LIST_REQ )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_DEL_POST_BLACK_LIST_REQ, EGS_DEL_POST_BLACK_LIST_ACK );
	//}}

	// SQL Injection 방지
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrNickName ) )
	{
		KEGS_DEL_POST_BLACK_LIST_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_DEL_POST_BLACK_LIST_ACK, kPacket );
		return;
	}

	if( !m_kUserLetterBox.IsBlackList( kPacket_.m_wstrNickName ) )
	{
		KEGS_NEW_POST_BLACK_LIST_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_POST_BLACK_LIST_03;
		SendPacket( EGS_NEW_POST_BLACK_LIST_ACK, kPacket );
		return;
	}

	KDBE_DEL_POST_BLACK_LIST_REQ kPacketToDB;
	kPacketToDB.m_iUnitUID = GetCharUID();
	kPacketToDB.m_wstrNickName = kPacket_.m_wstrNickName;
	SendToGameDB( DBE_DEL_POST_BLACK_LIST_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_DEL_POST_BLACK_LIST_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_DEL_POST_BLACK_LIST_ACK );
	//}}

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_kUserLetterBox.DelBlackList( kPacket_.m_wstrNickName );
	}

	KEGS_DEL_POST_BLACK_LIST_ACK kPacket;
	kPacket.m_iOK		   = kPacket_.m_iOK;
	kPacket.m_wstrNickName = kPacket_.m_wstrNickName;
	SendPacket( EGS_DEL_POST_BLACK_LIST_ACK, kPacket );
}
//}}

IMPL_ON_FUNC_NOPARAM( EGS_RENEWAL_LETTER_REQ )
{
	KEGS_RENEWAL_LETTER_ACK kPacket;
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	m_kUserLetterBox.GetRenewalLetter( GetThisPtr<KGSUser>(), kPacket.m_iNewUserLetterCount, kPacket.m_iNewSystemLetterCount, kPacket.m_iNewRelationshipLetterCount, kPacket.m_vecNewLetterTitle );
	//}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	kPacket.m_iTotalLetterCount = m_kUserLetterBox.GetTotalLetterCount( KEGS_GET_POST_LETTER_LIST_REQ::LLT_NORMAL );
	//}

	SendPacket(EGS_RENEWAL_LETTER_ACK, kPacket);
}

//{{ 2011. 04. 05  김민성	우편 및 거래 감시
IMPL_ON_FUNC( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_ACK )
{
	switch( kPacket_.m_iOK )
	{
	case NetError::ERR_EXIST_LETTER_MONITORING:
		{
			m_kUserHackingManager.ClearLetterMonitoringList();

			START_LOG( cerr, L"우편 감시 로그가 중복 되는 것이 있다. :")
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}break;
	case NetError::ERR_EXIST_TRADE_MONITORING:
		{
			m_kUserHackingManager.ClearTradeMonitoringList();

			START_LOG( cerr, L"거래 감시 로그가 중복 되는 것이 있다. :")
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}break;
	case NetError::ERR_INSERT_FAILD_LETTER_MONITORING:
		{
			m_kUserHackingManager.ClearLetterMonitoringList();

			START_LOG( cerr, L"우편 감시 로그가 추가 실패! :")
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}break;
	case NetError::ERR_INSERT_FAILD_TRADE_MONITORING:
		{
			m_kUserHackingManager.ClearTradeMonitoringList();

			START_LOG( cerr, L"거래 감시 로그가 추가 실패! :")
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;
			return;
		}break;
	case 0:
		{
			m_kUserHackingManager.ClearLetterMonitoringList();
		}break;
	case 1:
		{
			m_kUserHackingManager.ClearTradeMonitoringList();
		}break;
	default:
		{
			START_LOG( cerr, L"리턴 값이 이상합니다. 예외처리")
				<< BUILD_LOG( kPacket_.m_iOK )
				<< END_LOG;
			return;
		}
	}

	//{{ 2011. 04. 13  우편 및 거래 감시 실시간 SMS 전송
	//}} 2011. 04. 13 우편 및 거래 감시 실시간 SMS 전송


	START_LOG( cout, L"거래 감시 로그가 추가 성공! :")
		<< BUILD_LOG( GetCharUID() )
		<< END_LOG;
}
//}}

//{{ 우편 ED 거래시 ED 동기화 - 김민성
IMPL_ON_FUNC( DBE_SYNC_ED_ACK )
{
	KEGS_SEND_LETTER_ACK kAck;
	KDBE_PREPARE_INSERT_LETTER_TO_POST_REQ kPacketToDB;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		//{{ 2012. 10. 11	최육사		ED&아이템 모니터링 시스템
		m_iED.SetValue( m_iED - kPacket_.m_kInfo.m_iChangeED + kPacket_.m_kInfo.m_iED + kPacket_.m_kInfo.m_iSendLetterCost );

		m_iED += kPacket_.m_kInfo.m_iChangeED;
		//}}

		kAck.m_iOK = NetError::ERR_POST_LETTER_02;
		goto end_proc;
	}

	// 6. DB로 업데이트 체크
	kPacketToDB.m_iFromUnitUID				= GetCharUID();
	kPacketToDB.m_iItemUID					= kPacket_.m_kInfo.m_iItemUID;
	kPacketToDB.m_wstrToNickName			= kPacket_.m_kInfo.m_wstrToNickName;
	kPacketToDB.m_iSendLetterCost = kPacket_.m_kInfo.m_iSendLetterCost;
	kPacketToDB.m_kSendLetter.m_cScriptType = ( kPacket_.m_kInfo.m_bIsGmMessage ? KPostItemInfo::LT_MESSAGE : KPostItemInfo::LT_POST_OFFICE );
	kPacketToDB.m_kSendLetter.m_wstrTitle	= kPacket_.m_kInfo.m_wstrTitle;
	kPacketToDB.m_kSendLetter.m_wstrMessage = kPacket_.m_kInfo.m_wstrMessage;
	kPacketToDB.m_kSendLetter.m_iFromUnitUID = GetCharUID();
	kPacketToDB.m_kSendLetter.m_wstrFromNickName = GetCharName();
	kPacketToDB.m_kSendLetter.m_iScriptIndex = CXSLItem::EDI_BRONZE_ED; // 브론즈 ED
	kPacketToDB.m_kSendLetter.m_iQuantity	 = kPacket_.m_kInfo.m_iED;
	SendToGameDB( DBE_PREPARE_INSERT_LETTER_TO_POST_REQ, kPacketToDB );

	return;

end_proc:

	SendPacket( EGS_SEND_LETTER_ACK, kAck );
}
//}}

//{{ 2012. 08. 21	박세훈	우편 로직 변경
//}}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
