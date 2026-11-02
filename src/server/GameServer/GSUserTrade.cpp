#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"

#include "Enum/Enum.h"
//{{ 2011. 02. 06	최육사	거래 차단 기능
	#include "GameSysVal/GameSysVal.h"
//}}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define CLASS_TYPE      KGSUser

IMPL_ON_FUNC( EGS_REQUEST_TRADE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_REQUEST_TRADE_ACK kAck;

	//{{ 2011. 02. 06	최육사	거래 차단 기능
	if( SiKGameSysVal()->IsBlockPersonalTrade() == true )
	{
		kAck.m_iOK = NetError::ERR_BLOCK_TRADE_00;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}
	//}}

	//{{ 2011. 03. 02	최육사	개인 거래 차단
	if( m_kUserAbuserManager.IsTradeBlockUser() == true )
	{
		kAck.m_iOK = NetError::ERR_BLOCK_TRADE_07;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}
	//}}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		kAck.m_iOK = NetError::ERR_TRADE_BLOCK_UNIT_02;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		kAck.m_iOK = NetError::ERR_GUEST_USER_00;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}

	if( kPacket_.m_iUnitUID == GetCharUID() )
	{
		kAck.m_iOK = NetError::ERR_TRADE_13;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}

	//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합
	if( IsSameServerGroupUID( kPacket_.m_iUnitUID ) == false )
	{
		kAck.m_iOK = NetError::ERR_DIF_SERVER_GROUP_00;
		SendPacket( EGS_REQUEST_TRADE_ACK, kAck );
		return;
	}
	//}}

	kAck.m_iOK = NetError::NET_OK;
	kAck.m_iUnitUID = kPacket_.m_iUnitUID;
	//kAck.m_wstrNickName = spUser->GetCharName();
	SendPacket( EGS_REQUEST_TRADE_ACK, kAck );

	KERM_REQUEST_TRADE_NOT kNot;
	kNot.m_iSenderUID		= GetCharUID();
	kNot.m_wstrSenderNickName = GetCharName();
	kNot.m_iReceiverUID		= kPacket_.m_iUnitUID;

	if( GetStateID() == KGSFSM::S_ROOM )
	{
		SendToCnRoom( ERM_REQUEST_TRADE_NOT, kNot );
	}
	else
	{
		SendToGSField( ERM_REQUEST_TRADE_NOT, kNot );
	}
}

IMPL_ON_FUNC( ERM_REQUEST_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 2009. 3. 24  최육사	GM제한
	if( GetAuthLevel() == SEnum::UAL_GM )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::ERR_GM_CHAR_DENY_00; // 개인거래 거절로 처리..

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}
	//}}

	//{{ 2011. 03. 02	최육사	개인 거래 차단
	if( m_kUserAbuserManager.IsTradeBlockUser() == true )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_02; // 개인거래 거절로 처리..

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}
	//}}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_02; // 개인거래 거절로 처리..

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_02; // 개인거래 거절로 처리..

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}

	//{{ 2009. 5. 12  최육사	개인거래 거부
	if( m_kUserGameOption.IsDenyTrade() )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_02; // 개인거래 거절로 처리..

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}
	//}}

	// 이미 거래 중인 경우.
	if( GetTradeUID() != 0  ||  GetPersonalShopUID() != 0 )
	{
		KERM_REQUEST_TRADE_REPLY_NOT kPacketNot;
		kPacketNot.m_iReceiverUID = kPacket_.m_iSenderUID;
		kPacketNot.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_01;

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		else
		{
			SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacketNot );
		}
		return;
	}

	KEGS_REQUEST_TRADE_NOT kPacket;
	kPacket.m_iSenderUID                = kPacket_.m_iSenderUID;
	kPacket.m_wstrSenderNickName        = kPacket_.m_wstrSenderNickName;

	SendPacket( EGS_REQUEST_TRADE_NOT, kPacket );
}

IMPL_ON_FUNC( EGS_REQUEST_TRADE_REPLY_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KERM_REQUEST_TRADE_REPLY_NOT kPacket;

	kPacket.m_iReceiverUID	= kPacket_.m_iReceiverUID;

	if( kPacket_.m_bAccept )
	{
		kPacket.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_00;
	}
	else
	{
		kPacket.m_iOK = NetError::NOT_REQUEST_TRADE_REPLY_02;
	}

	kPacket.m_iMemberUID				= GetCharUID();
	kPacket.m_iMemberGSUID				= KBaseServer::GetKObj()->GetUID();

	if( GetStateID() == KGSFSM::S_ROOM )
	{
		SendToCnRoom( ERM_REQUEST_TRADE_REPLY_NOT, kPacket );
	}
	else
	{
		SendToGSField( ERM_REQUEST_TRADE_REPLY_NOT, kPacket );
	}
}

IMPL_ON_FUNC( ERM_REQUEST_TRADE_REPLY_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK == NetError::NOT_REQUEST_TRADE_REPLY_00 )
	{
		KERM_OPEN_TRADE_REQ kPacket;
		kPacket.m_iUserUID = GetUID();
		kPacket.m_wstrSenderNickName = GetCharName();
		kPacket.m_iMemberUID	= kPacket_.m_iMemberUID;
		kPacket.m_iMemberGSUID	= kPacket_.m_iMemberGSUID;
		//{{ 2009. 4. 28  최육사	최대ED제한
		kPacket.m_iED = GetED();
		//}}
		//{{ 2011. 06. 21	최육사	거래방 생성 버그 수정
		_SendToCnRoom( 0, ERM_OPEN_TRADE_REQ, kPacket );
		//}}
	}
	else
	{
		KEGS_REQUEST_TRADE_DENY_NOT kPacketNot;
		kPacketNot.m_iOK = kPacket_.m_iOK;
		kPacketNot.m_iUnitUID = kPacket_.m_iMemberUID;
		SendPacket( EGS_REQUEST_TRADE_DENY_NOT, kPacketNot );
	}
}

IMPL_ON_FUNC( ERM_OPEN_TRADE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to create a private trade!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_TRADE_FINISH_NOT );
		}
		//else
		//{
		//	SendToGSField( ERM_TRADE_FINISH_NOT );
		//}
		return;
	}

	m_iTradeUID = kPacket_.m_iTradeUID;

	KEGS_CREATE_TRADE_NOT kPacketNot;
	kPacketNot.m_iTradeUID = kPacket_.m_iTradeUID;
	SendPacket( EGS_CREATE_TRADE_NOT, kPacketNot );
}

IMPL_ON_FUNC( ERM_ACCEPT_JOIN_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( GetTradeUID() != 0 )
	{
		START_LOG( cerr, L"Already belongs to another trade." )
			<< BUILD_LOG( GetTradeUID() )
			<< END_LOG;

		return;
	}

	KERM_JOIN_TRADE_REQ kPacket;
	kPacket.m_kMemberInfo.m_iGSUID	= KBaseServer::GetKObj()->GetUID();
	kPacket.m_kMemberInfo.m_iUserUID = GetUID();
	kPacket.m_kMemberInfo.m_iUnitUID = GetCharUID();
	kPacket.m_kMemberInfo.m_wstrNickName = GetCharName();
	//{{ 2009. 4. 28  최육사	최대ED제한
	kPacket.m_kMemberInfo.m_iED = GetED();
	//}}

	_SendToCnRoom( kPacket_, ERM_JOIN_TRADE_REQ, kPacket );
}

IMPL_ON_FUNC( ERM_JOIN_TRADE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to join the private trade." )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		return;
	}

	if( GetStateID() == KGSFSM::S_ROOM )
	{
		SendToCnRoom( ERM_TRADE_START_NOT );
	}
	//else
	//{
	//	SendToGSField( ERM_TRADE_START_NOT );
	//}

	m_iTradeUID = kPacket_.m_iTradeUID;
	KEGS_TRADE_USER_INFO_NOT kPacket;
	kPacket.m_vecTradeUserInfo = kPacket_.m_vecTradeUserInfo;
	SendPacket( EGS_TRADE_USER_INFO_NOT, kPacket );
}

_IMPL_ON_FUNC( ERM_JOIN_TRADE_NOT, KEGS_JOIN_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 자기 자신의 JOIN은 ACK로 갔다.
	if( kPacket_.m_kTradeUserInfo.m_iUnitUID == GetCharUID() )
	{
		return;
	}

	SendPacket( EGS_JOIN_TRADE_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_BREAK_TRADE_REQ )
{
	if( GetStateID() == KGSFSM::S_INIT ||
		GetStateID() == KGSFSM::S_UID_UPDATED ||
		GetStateID() == KGSFSM::S_LOGINED ||
		GetStateID() == KGSFSM::S_SERVER_SELECT )
	{
		START_LOG( cerr, L"Invalid state!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
	}

	if( GetTradeUID() == 0 )
	{
		START_LOG( cerr, L"Invalid personal trade UID!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		KEGS_BREAK_TRADE_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_TRADE_01;
		SendPacket( EGS_BREAK_TRADE_ACK, kPacket );

		KEGS_BREAK_TRADE_NOT kPacketNot;
		kPacketNot.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_16;
		SendPacket( EGS_BREAK_TRADE_NOT, kPacketNot );
		return;
	}

	_SendToCnRoom( GetTradeUID(), ERM_BREAK_TRADE_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_BREAK_TRADE_REQ, KEGS_BREAK_TRADE_REQ )
{
	// 상태는 나중에 막음.

	if( GetTradeUID() == 0 )
	{
		START_LOG( cerr, L"Invalid personal trade UID!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		return;
	}

	_SendToCnRoom( GetTradeUID(), ERM_BREAK_TRADE_REQ, kPacket_ );
}

_IMPL_ON_FUNC( ERM_BREAK_TRADE_ACK, KEGS_BREAK_TRADE_ACK )
{
	if( GetStateID() == KGSFSM::S_INIT ||
		GetStateID() == KGSFSM::S_UID_UPDATED ||
		GetStateID() == KGSFSM::S_LOGINED ||
		GetStateID() == KGSFSM::S_SERVER_SELECT )
	{
		START_LOG( cerr, L"Invalid state!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
	}

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		m_iTradeUID = 0;

		if( GetStateID() == KGSFSM::S_ROOM )
		{
			SendToCnRoom( ERM_TRADE_FINISH_NOT );
		}
		//else
		//{
		//	SendToGSField( ERM_TRADE_FINISH_NOT );
		//}
	}

	SendPacket( EGS_BREAK_TRADE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_BREAK_TRADE_NOT, KEGS_BREAK_TRADE_NOT )
{
	if( GetStateID() == KGSFSM::S_INIT ||
		GetStateID() == KGSFSM::S_UID_UPDATED ||
		GetStateID() == KGSFSM::S_LOGINED ||
		GetStateID() == KGSFSM::S_SERVER_SELECT )
	{
		START_LOG( cerr, L"Invalid state!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;
	}

	m_iTradeUID = 0;

	if( kPacket_.m_iReason == NetError::NOT_LEAVE_ROOM_REASON_00 )
	{
		kPacket_.m_iReason = NetError::NOT_LEAVE_ROOM_REASON_16;
	}

	if( kPacket_.m_iReason != NetError::NOT_LEAVE_ROOM_REASON_06 )
	{
		SendPacket( EGS_BREAK_TRADE_NOT, kPacket_ );
	}

	if( GetStateID() == KGSFSM::S_ROOM )
	{
		SendToCnRoom( ERM_TRADE_FINISH_NOT );
	}
	//else
	//{
	//	SendToGSField( ERM_TRADE_FINISH_NOT );
	//}
}

IMPL_ON_FUNC( EGS_UPDATE_TRADE_ITEM_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_UPDATE_TRADE_ITEM_REQ, EGS_UPDATE_TRADE_ITEM_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_UPDATE_TRADE_ITEM_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"The inventory is locked!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );


			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kAck );
			return;
		}
		//}}

		//{{ 2011. 02. 06	최육사	거래 차단 기능
		if( SiKGameSysVal()->IsBlockPersonalTrade() == true )
		{
			kAck.m_iOK = NetError::ERR_BLOCK_TRADE_00;
			SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kAck );
			return;
		}
		//}}

		//{{ 2011. 03. 02	최육사	개인 거래 차단
		if( m_kUserAbuserManager.IsTradeBlockUser() == true )
		{
			kAck.m_iOK = NetError::ERR_BLOCK_TRADE_07;
			SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kAck );
			return;
		}
		//}}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		kAck.m_iOK = NetError::ERR_TRADE_BLOCK_UNIT_02;
		SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kAck );
		return;
	}

		if( GetTradeUID() == 0 )
		{
			START_LOG( cerr, L"Received a personal trade item update packet while not in a personal trade." )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_TRADE_02;
			SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kAck );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	KEGS_UPDATE_TRADE_ITEM_ACK kPacketAck;
	KERM_UPDATE_TRADE_ITEM_REQ kPacket;

	if( !kPacket_.m_bDelete )
	{
		kPacketAck.m_iOK = m_kInventory.CheckItemForTrade( kPacket_.m_iItemUID, kPacket_.m_iQuantity, kPacket.m_kInventoryItemInfo );
		if( kPacketAck.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Invalid item to register." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( kPacketAck.m_iOK )
				<< BUILD_LOG( NetError::GetErrStr( kPacketAck.m_iOK ) )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< BUILD_LOG( m_kInventory.GetItemID( kPacket_.m_iItemUID ) )
				<< BUILD_LOG( kPacket_.m_iQuantity )
				<< END_LOG;

			SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kPacketAck );
			return;
		}

		kPacket.m_kInventoryItemInfo.m_kItemInfo.m_iQuantity = kPacket_.m_iQuantity;
	}
	else
	{
		kPacket.m_kInventoryItemInfo.m_iItemUID = kPacket_.m_iItemUID;
	}

	kPacket.m_bDelete = kPacket_.m_bDelete;
	_SendToCnRoom( GetTradeUID(), ERM_UPDATE_TRADE_ITEM_REQ, kPacket );
}

_IMPL_ON_FUNC( ERM_UPDATE_TRADE_ITEM_ACK, KEGS_UPDATE_TRADE_ITEM_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_UPDATE_TRADE_ITEM_ACK );
	//}}

	SendPacket( EGS_UPDATE_TRADE_ITEM_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_UPDATE_TRADE_ITEM_NOT, KEGS_UPDATE_TRADE_ITEM_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_UPDATE_TRADE_ITEM_NOT, kPacket_ );
}

IMPL_ON_FUNC( EGS_UPDATE_TRADE_ED_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_UPDATE_TRADE_ED_REQ, EGS_UPDATE_TRADE_ED_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		KEGS_UPDATE_TRADE_ED_ACK kAck;

		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"The inventory is locked!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
		//}}

		//{{ 2011. 02. 06	최육사	거래 차단 기능
		if( SiKGameSysVal()->IsBlockPersonalTrade() == true )
		{
			kAck.m_iOK = NetError::ERR_BLOCK_TRADE_00;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
		//}}

		//{{ 2011. 03. 02	최육사	개인 거래 차단
		if( m_kUserAbuserManager.IsTradeBlockUser() == true )
		{
			kAck.m_iOK = NetError::ERR_BLOCK_TRADE_07;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
		//}}

	if( m_kUserAbuserManager.IsTradeBlockUnit() == true )
	{
		kAck.m_iOK = NetError::ERR_TRADE_BLOCK_UNIT_02;
		SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
		return;
	}

		//{{ 2011. 04. 11	최육사	거래 제한
		if( kPacket_.m_iED > SEnum::UI_MAX_TRADE_ED )
		{
			START_LOG( cerr, L"The maximum trade limit was exceeded; possible hacking!" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( kPacket_.m_iED )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_TRADE_04;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
		//}}

		//{{ 2010. 03. 17  최육사	상태 오류 수정
		if( GetTradeUID() == 0 )
		{
			START_LOG( cerr, L"Received a private trade event while not in a private trade?" )
				<< BUILD_LOG( GetCharUID() )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_TRADE_00;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
		//}}

		if( kPacket_.m_iED < 0 || kPacket_.m_iED > GetED() )
		{
			START_LOG( cerr, L"Could this be hacking?" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_iED )
				<< BUILD_LOG( GetED() )
				<< END_LOG;

			kAck.m_iOK = NetError::ERR_TRADE_04;
			SendPacket( EGS_UPDATE_TRADE_ED_ACK, kAck );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	KERM_UPDATE_TRADE_ED_REQ kPacket;
	kPacket = kPacket_.m_iED;
	_SendToCnRoom( GetTradeUID(), ERM_UPDATE_TRADE_ED_REQ, kPacket );
}

_IMPL_ON_FUNC( ERM_UPDATE_TRADE_ED_ACK, KEGS_UPDATE_TRADE_ED_ACK )
{
	//{{ 2010. 10. 07	최육사	중복 패킷 필터 추가
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_UPDATE_TRADE_ED_ACK );
	//}}

	SendPacket( EGS_UPDATE_TRADE_ED_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_UPDATE_TRADE_ED_NOT, KEGS_UPDATE_TRADE_ED_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_UPDATE_TRADE_ED_NOT, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( EGS_OFFER_TRADE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//{{ 2010. 03. 17  최육사	상태 오류 수정
	if( GetTradeUID() == 0 )
	{
		START_LOG( cerr, L"Received a private trade event when no private trade is active?" )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;
		return;
	}
	//}}

	_SendToCnRoom( GetTradeUID(), ERM_OFFER_TRADE_REQ, char() );
}

_IMPL_ON_FUNC( ERM_OFFER_TRADE_ACK, KEGS_OFFER_TRADE_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_OFFER_TRADE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_OFFER_TRADE_NOT, KEGS_OFFER_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_OFFER_TRADE_NOT, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( EGS_APPROVE_TRADE_REQ )
{
	VERIFY_STATE_REPEAT_FILTER( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_APPROVE_TRADE_REQ, EGS_APPROVE_TRADE_ACK );

	//////////////////////////////////////////////////////////////////////////
	// 예외처리
	{
		//{{ 2012. 03. 29	최육사	Inventory Lock 기능
		KEGS_APPROVE_TRADE_ACK kAck;
		if( m_kInventory.IsLocked() == true )
		{
			START_LOG( cout, L"The inventory is locked!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( GetCharUID() );

			kAck.m_iOK = NetError::ERR_INVENTORY_LOCK_00;
			SendPacket( EGS_APPROVE_TRADE_ACK, kAck );
			return;
		}
		//}}
	}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2012. 05. 08	김민성       개인 거래 로직 변경
	WriteUnitInfoToDB( false, ERM_APPROVE_TRADE_REQ );

	//{{ 2012. 09. 22	박세훈	빙고 이벤트
	//}}
	//}}
}

_IMPL_ON_FUNC( ERM_APPROVE_TRADE_ACK, KEGS_APPROVE_TRADE_ACK )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_APPROVE_TRADE_ACK );

	SendPacket( EGS_APPROVE_TRADE_ACK, kPacket_ );
}

_IMPL_ON_FUNC( ERM_APPROVE_TRADE_NOT, KEGS_APPROVE_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	SendPacket( EGS_APPROVE_TRADE_NOT, kPacket_ );
}

IMPL_ON_FUNC( ERM_CONFIRM_TRADE_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KERM_CONFIRM_TRADE_ACK kPacketAck;

	if( m_kInventory.CheckTradeItemOut( kPacket_.m_vecItemOut, kPacketAck.m_kItemQuantityUpdateTemp, CXSLRoom::RT_TRADE ) == false )
	{
		START_LOG( cerr, L"Personal trade confirmation check failed!" )
			<< BUILD_LOG( GetCharUID() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::GetLastError();
		_SendToCnRoom( GetTradeUID(), ERM_CONFIRM_TRADE_ACK, kPacketAck );

		//	거래는 나가는것만 체크하면 될까?
		if( kPacketAck.m_iOK == NetError::ERR_SHARE_BANK_02 )
		{
			//	통계 남기기
			CTime tCurTime = CTime::GetCurrentTime();

			KELOG_ITEM_TRADE_ERROR_NOT kErrorNot;
			kErrorNot.m_iUnitUID = GetCharUID();
			kErrorNot.m_iType = SEnum::TET_TRADE;
			kErrorNot.m_wsrtRegDate = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );

			SendToLogDB( ELOG_ITEM_TRADE_ERROR_NOT, kErrorNot );
		}
		return;
	}

	if( kPacket_.m_iEDOut > GetED() )
	{
		START_LOG( cerr, L"Could this be hacking?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iEDOut )
			<< BUILD_LOG( GetED() )
			<< END_LOG;

		kPacketAck.m_iOK = NetError::ERR_TRADE_06;
		_SendToCnRoom( GetTradeUID(), ERM_CONFIRM_TRADE_ACK, kPacketAck );
		return;
	}

	if( m_kInventory.CheckTradeItemIn( kPacket_.m_vecItemIn, kPacketAck.m_kItemQuantityUpdateTemp, kPacketAck.m_vecItemInfoTemp, CXSLRoom::RT_TRADE ) == false )
	{
		kPacketAck.m_iOK = NetError::GetLastError();
		_SendToCnRoom( GetTradeUID(), ERM_CONFIRM_TRADE_ACK, kPacketAck );
		return;
	}

	// 들어오고 빠져나가는 ED를 감소시킨후의 현재 ED값을 Temp로 저장
	kPacketAck.m_iEDTemp = ( GetED() - kPacket_.m_iEDOut + kPacket_.m_iEDIn );

	kPacketAck.m_iOK = NetError::NET_OK;
	_SendToCnRoom( GetTradeUID(), ERM_CONFIRM_TRADE_ACK, kPacketAck );
	return;
}

//{{ 2012. 05. 08	김민성       개인 거래 로직 변경
IMPL_ON_FUNC( ERM_CONFIRM_TRADE_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KERM_DB_UPDATE_TRADE_RESULT_NOT kPacketToCnTrade;
	kPacketToCnTrade.m_iUnitUID = GetCharUID();

	//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
	if( IsSameServerGroupUID( kPacket_.m_iTradeUID ) == false )
	{
		START_LOG( cerr, L"Received a TradeUID from another server group; this should never happen!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_iTradeUID )
			<< BUILD_LOG( KncUid::ExtractServerGroupID( kPacket_.m_iTradeUID ) )
			<< BUILD_LOG( KncUid::ExtractServerGroupID( GetCharUID() ) )
			<< END_LOG;

		// 로그만 출력.
	}
	//}}

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	if( SiKGameSysVal()->GetLetterAndTradeMonitoringIsOn() == true )
	{
		// 거래 시 준 아이템과 ED는 없고 받은 ED 만 있을 때
		if( kPacket_.m_iEDOut <= 0 && kPacket_.m_vecItemOut.empty() && kPacket_.m_iEDIn > 0  )
		{
			if( m_kUserHackingManager.AddTradeMonitoringList( kPacket_.m_iOtherCID ) == true )
			{
				KDBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ PacketReq;
				CTime tCurTime = CTime::GetCurrentTime();

				PacketReq.m_iRecvUnitUID = GetCharUID();
				PacketReq.m_wstrRegDate = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
				PacketReq.m_cType = 1;

				SendToLogDB( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ, PacketReq );
			}
		}
	}
	//}}

	//////////////////////////////////////////////////////////////////////////
	// ED거래
	{
		//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
		// ED 어뷰저
		if( kPacket_.m_iEDOut != 0 )
		{
			//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
//#ifdef SERV_USER_ABUSER_MANAGER
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_OUT_TRADE, GetED() - kPacket_.m_iEDOut );
//#endif SERV_USER_ABUSER_MANAGER
			//}}
		}

		if( kPacket_.m_iEDIn != 0 )
		{
			//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
//#ifdef SERV_USER_ABUSER_MANAGER
			m_kUserAbuserManager.CheckEDAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::ELS_IN_TRADE, GetED() - kPacket_.m_iEDOut + kPacket_.m_iEDIn );
//#endif SERV_USER_ABUSER_MANAGER
			//}}
		}
		//}}

		// ED 변동 전에 동기화 내용 확인 하자
		//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
		if( m_iED.GetChangeValue() != 0 )
		//}}
		{
			START_LOG( cerr, L"Server-database synchronization was not completed before the private trade." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetCharUID() )
				//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
				<< BUILD_LOG( m_iED.GetChangeValue() )
				//}}
				<< END_LOG;
		}

		// DB업데이트할 ED변화량 정보
		kPacketToCnTrade.m_iChangedED = ( kPacket_.m_iEDIn - kPacket_.m_iEDOut );
		//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
		m_iED.SetValue( GetED() + kPacketToCnTrade.m_iChangedED );
		//}}


		{
			KStatisticsKey kKey;
			kKey.m_vecIntKey.push_back( 0 );
			KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_PItemTrade, kPacket_.m_iEDIn );
			KSIManager.IncreaseCount( KStatistics::SI_ED, kKey, KStatistics::eSIColDB_ED_MItemTrade, kPacket_.m_iEDOut );
		}

		{
			// 유저 통계
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_TradeCount, 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 나가는 아이템
	for( const KInventoryItemInfo& kOutItemInfo : kPacket_.m_vecItemOut )
	{
		int iCategory;
		int iSlotID;
		LIF( m_kInventory.GetCategorySlot( kOutItemInfo.m_iItemUID, iCategory, iSlotID ) );
		LIF( static_cast<int>(kOutItemInfo.m_cSlotCategory) == iCategory );
		LIF( static_cast<int>(kOutItemInfo.m_sSlotID) == iSlotID );

		KInventoryItemInfo kInfo;
		KInventoryItemInfo kInfoUpdate;
		if( kOutItemInfo.m_kItemInfo.m_cUsageType == CXSLItem::PT_QUANTITY )
		{
			int iDecreased;
			KInventoryItemInfo kBeforQuantityInfo;
			m_kInventory.GetInventoryItemInfo( kOutItemInfo.m_iItemUID, kBeforQuantityInfo );

			m_kInventory.DecreaseQuantity( kOutItemInfo.m_iItemUID, kOutItemInfo.m_kItemInfo.m_iQuantity, iDecreased, KDeletedItemInfo::DR_TRADE );
			LIF( kOutItemInfo.m_kItemInfo.m_iQuantity == iDecreased );
			m_kInventory.GetInventorySlotInfo( iCategory, iSlotID, kInfo );

			// 수량성 아이템이 삭제 되면 아이템 정보가 초기화 되기 때문에 아이템아이디를 저장해 둔다.
			kInfo.m_kItemInfo.m_iItemID = kBeforQuantityInfo.m_kItemInfo.m_iItemID;
			kInfo.m_kItemInfo.m_cUsageType = kBeforQuantityInfo.m_kItemInfo.m_cUsageType;
			kInfo.m_kItemInfo.m_iQuantity = kBeforQuantityInfo.m_kItemInfo.m_iQuantity - iDecreased;

			kInfoUpdate = kInfo;
			kInfoUpdate.m_kItemInfo.m_iQuantity = iDecreased;
		}
		else
		{
			KInventoryItemInfo kBeforDeleteInfo;
			KInventoryItemInfo kTempInfo;
			m_kInventory.GetInventoryItemInfo( kOutItemInfo.m_iItemUID, kBeforDeleteInfo );
			m_kInventory.DeleteItem( kOutItemInfo.m_iItemUID, kTempInfo, KDeletedItemInfo::DR_TRADE );

			kInfoUpdate = kBeforDeleteInfo;
			kInfo = kBeforDeleteInfo;
			kInfo.m_kItemInfo.m_iQuantity = 0;
		}

		// 인벤토리 정보 갱신
		kInfoUpdate.m_iItemUID = kOutItemInfo.m_iItemUID;
		kPacketToCnTrade.m_vecUpdated.push_back( kInfoUpdate );

		kInfo.m_iItemUID = kOutItemInfo.m_iItemUID;
		kPacketToCnTrade.m_vecUpdatedInventorySlot.push_back( kInfo );

		// 유저 통계 개인거래 판매 카운트
		m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Trade_Sell_Item, kOutItemInfo.m_kItemInfo.m_iQuantity );

		// 일일통계 거래 아이템
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( kOutItemInfo.m_kItemInfo.m_iItemID );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_TRADE, kOutItemInfo.m_kItemInfo.m_iQuantity );
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_TRADE, kOutItemInfo.m_kItemInfo.m_iQuantity );
		//}}
	}

	// 나가는 아이템으로 인한 인벤토리 정보 변경
	{
		m_kInventory.FlushQuantityChange( kPacketToCnTrade.m_kItemQuantityUpdate_OUT.m_mapQuantityChange );
		m_kInventory.FlushDeletedItem( kPacketToCnTrade.m_kItemQuantityUpdate_OUT.m_vecDeleted );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 들어오는 아이템
	if( !kPacket_.m_vecItemIn.empty() )
	{
		for( u_int ui = 0; ui < kPacket_.m_vecItemIn.size(); ++ui )
		{
			// 유저 통계 개인거래 구매 카운트
			m_kUserStatistics.IncreaseCount( KUserStatistics::USTable_EtcData, 0, KUserStatistics::US_Etc_Trade_Buy_Item, kPacket_.m_vecItemIn[ui].m_kItemInfo.m_iQuantity );
		}

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		// 아이템 어뷰저
		std::map<int, int> mapInsertedItem;
		std::vector<KInventoryItemInfo>::iterator vit = kPacket_.m_vecItemIn.begin();
		for( ; vit != kPacket_.m_vecItemIn.end() ; ++vit )
		{
			mapInsertedItem.insert(std::make_pair( vit->m_kItemInfo.m_iItemID, vit->m_kItemInfo.m_iQuantity ));
		}

		m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_TRADE, mapInsertedItem );
		//}}
	}

	//////////////////////////////////////////////////////////////////////////
	// DB업데이트를 위한 정보 전송
	{
		_SendToCnRoom( GetTradeUID(), ERM_DB_UPDATE_TRADE_RESULT_NOT, kPacketToCnTrade );

		m_kInventory.Lock( KInventoryLocker::LT_TIME_OUT, 60.0 );
	}
	//////////////////////////////////////////////////////////////////////////
}

_IMPL_ON_FUNC( ERM_DB_UPDATE_TRADE_COMPLETE_NOT, KERM_DB_UPDATE_TRADE_RESULT_NOT )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	//////////////////////////////////////////////////////////////////////////
	// ED결과 처리
	{
		// 인벤토리 언락!
		m_kInventory.Unlock();

		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		if( GetRoomUID() > 0  &&  CXSLRoom::GetRoomType( GetRoomUID() ) == CXSLRoom::RT_SQUARE )
		{
			// 변화한 ED 정보를 SQUARE 로 갱신 시켜 주자.
			KERM_SQUARE_UNIT_REFRESH_ED_NOT kReq;
			kReq.m_iUnitUID = GetCharUID();
			kReq.m_iED = GetED();
			SendToCnRoom( ERM_SQUARE_UNIT_REFRESH_ED_NOT, kReq );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	KDBE_TRADE_COMPLETE_REQ kPacketReq;
	std::vector< KInventoryItemInfo >	vecInventorySlotInfo;
	std::map< UidType, KInventoryItemInfo > mapItemInfo;
	std::map< UidType, KInventoryItemInfo >::iterator mit;
	std::map< UidType, KInventoryItemInfo >::iterator mitTemp;

	std::map< UidType, KItemInfo > mapInsertItemInfo;

	kPacketReq.m_iUnitUID = GetCharUID();
	kPacketReq.m_iED = GetED();

	kPacketReq.m_vecInventorySlotInfo = kPacket_.m_vecUpdatedInventorySlot;

	// 나가는 아이템 이므로 아이템 UID 를 0으로 하면 클라에서 삭제 처리된다.
	for( KInventoryItemInfo& kInsertItemInfo : kPacketReq.m_vecInventorySlotInfo )
	{
		// 클라이언트에서 슬롯을 비우는 것은 uid 가 없을때
		if( kInsertItemInfo.m_kItemInfo.m_iQuantity <= 0 )
		{
			kInsertItemInfo.m_iItemUID  = 0;
		}
	}

	// 들어 오는 아이템이 있다.
	if( kPacket_.m_vecInsertDBItem.empty() == false )
	{
		// 새로 추가된 아이템 중 중복되는 인벤토리 목록을 정리하자
		m_kInventory.FixUpInventoryItem( kPacket_.m_vecInsertDBItem, kPacketReq.m_kQuantityUpdate , kPacketReq.m_vecInventorySlotInfo );

		// 거래로 인하여 추가된 아이템을 먼저 추가 하자
		// 실제로 들어 온 item
		for( const KInventoryItemInfo& kInsertItemInfo : kPacket_.m_vecInsertDBItem )
		{
			KItemInfo kInfo = kInsertItemInfo.m_kItemInfo;

			if( kInsertItemInfo.m_iItemUID <= 0 )
				continue;

			if( kInfo.m_iQuantity <= 0 )
				continue;

			mapInsertItemInfo.insert( std::make_pair( kInsertItemInfo.m_iItemUID, kInfo ) );
		}
		std::vector< KInventoryItemInfo >	vecInsertInventoryTemp;

		if( m_kInventory.InsertItem( mapInsertItemInfo, vecInsertInventoryTemp ) == false )
		{
			for( const KInventoryItemInfo& kInsertItemInfo : vecInsertInventoryTemp )
			{
				START_LOG( cerr, L"Adding the item failed?!" )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( kInsertItemInfo.m_iItemUID )
					<< BUILD_LOG( kInsertItemInfo.m_sSlotID )
					<< BUILD_LOG( kInsertItemInfo.m_kItemInfo.m_iQuantity )
					<< END_LOG;
			}
		}

		// 추가된 아이템을 갱신 목록에 추가한다.
		for( const KInventoryItemInfo& kInsertItemInfo : vecInsertInventoryTemp )
		{
			for( KInventoryItemInfo& kTempInfo : kPacketReq.m_vecInventorySlotInfo )
			{
				if( kInsertItemInfo.m_iItemUID <= 0 )
					continue;

				if( kInsertItemInfo.m_iItemUID == kTempInfo.m_iItemUID )
				{
					kTempInfo.m_sSlotID = kInsertItemInfo.m_sSlotID;
					break;
				}
			}

			kPacketReq.m_vecInventorySlotInfo.push_back( kInsertItemInfo );
		}

		// 중복 갱신,추가 방지
		KItemQuantityUpdate kQuantityUpdate;
		m_kInventory.FlushQuantityChange( kQuantityUpdate.m_mapQuantityChange );
		m_kInventory.FlushDeletedItem( kQuantityUpdate.m_vecDeleted );
	}

	SendToGameDB( DBE_TRADE_COMPLETE_REQ, kPacketReq );
}

IMPL_ON_FUNC( DBE_TRADE_COMPLETE_ACK )
{
	KEGS_TRADE_COMPLETE_NOT kPacketNot;
	//m_kInventory.RollBackInitQuantity( kPacket_.m_kItemQuantityUpdate.m_mapQuantityChange );
	//m_kInventory.RollBackDeletedItem( kPacket_.m_kItemQuantityUpdate.m_vecDeleted );
	kPacketNot.m_iED = kPacket_.m_iED;
	kPacketNot.m_vecInventorySlotInfo.insert( kPacketNot.m_vecInventorySlotInfo.begin(), kPacket_.m_vecInventorySlotInfo.begin(), kPacket_.m_vecInventorySlotInfo.end() );
	SendPacket( EGS_TRADE_COMPLETE_NOT, kPacketNot );
}
//}}

_IMPL_ON_FUNC( ERM_CHANGE_TRADE_NOT, KEGS_CHANGE_TRADE_NOT )
{
	VERIFY_STATE( ( 1, KGSFSM::S_ROOM ) );

	SendPacket( EGS_CHANGE_TRADE_NOT, kPacket_ );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
