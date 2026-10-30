#include "GSUser.h"
#include <dbg/dbg.hpp>
#include "GSFSM.h"
#include "NetError.h"

#include "BaseServer.h"
#include "NexonBillingTCPManager.h"

#include "ODBC/Odbc.h"
#include "Enum/Enum.h"

//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
	#include "ClassChangeTable.h"
//}}

//{{ 2012. 04. 02	김민성		넥슨 auth soap
//}}

//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
//}}

//{{ 2012. 10. 2	박세훈	빙고 이벤트
//}}

//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
#include "X2Data/XSLSocketItem.h"
//}

#include "X2Data/XSLSquareUnit.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "x2/platform/string.h"

#define CLASS_TYPE      KGSUser


IMPL_ON_FUNC( EGS_PURCHASED_CASH_ITEM_LIST_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_PURCHASED_CASH_ITEM_LIST_ACK );

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 캐쉬샵 인벤토리 정보 요청" )
			<< BUILD_LOG( GetName() );

		KEGS_PURCHASED_CASH_ITEM_LIST_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_CONTENT_00;
		SendPacket( EGS_PURCHASED_CASH_ITEM_LIST_ACK, kPacketAck );
		return;
	}
	//}}

    KENX_BT_NISMS_INVENTORY_INQUIRY_REQ kPacket;
    kPacket.m_ulPacketNo = SiKNexonBillingTCPManager()->GetNextPacketNo();
    kPacket.m_bytePacketType = KNexonBillingTCPPacket::NISMS_INVENTORY_INQUIRY;
    kPacket.m_wstrGameID = GetName();
    kPacket.m_byteShowInventory = 10;       // 10 : true, 30 : false
    kPacket.m_ulPageIndex = kPacket_.m_iCurrentPage;
    kPacket.m_ulRowPerPage = kPacket_.m_nItemPerPage;

    KEventPtr spEvent( new KEvent );
    UidType anTrace[2] = { GetUID(), -1 };
    spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_NISMS_INVENTORY_INQUIRY_REQ, kPacket );
    SiKNexonBillingTCPManager()->QueueingEvent( spEvent );

/*
    KDBE_PURCHASED_CASH_ITEM_LIST_REQ kPacket;
    kPacket.m_iCurrentPage = kPacket_.m_iCurrentPage;
    kPacket.m_nItemPerPage = kPacket_.m_nItemPerPage;
    kPacket.m_wstrUnitNickName = GetCharName();

	SendToNXShopDB( EGS_PURCHASED_CASH_ITEM_LIST_REQ, kPacket );
 */
}


IMPL_ON_FUNC( ENX_BT_NISMS_INVENTORY_PICK_UP_ACK )
{
    //if( kPacket_.m_ulResult != 1 )
    //{
    //    START_LOG( cerr, L"캐쉬 아이템 가져 오기 실패." )
    //        << BUILD_LOG( GetUID() )
    //        << BUILD_LOG( GetName() )
    //        << BUILD_LOG( kPacket_.m_ulResult )
    //        << END_LOG;

    //    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    //    kPacket.m_iOK = NetError::ERR_NX_SHOP_03;
    //    SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
    //    return;
    //}

    //int iItemID = SiKNexonBillingTCPManager()->GetItemID( kPacket_.m_ulProductNo );
    //if( iItemID <= 0 )
    //{
    //    START_LOG( cerr, L"아이템 ID 변환 실패." )
    //        << BUILD_LOG( kPacket_.m_ulProductNo )
    //        << BUILD_LOG( iItemID )
    //        << END_LOG;

    //    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    //    kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
    //    SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
    //    return;
    //}

    //std::map< int, KItemInfo > mapItem;

    //CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
    //if( pItemTemplet == NULL )
    //{
    //    START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
    //        << BUILD_LOG( iItemID )
    //        << END_LOG;

    //    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    //    kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
    //    SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
    //    return;
    //}

    //// 부활석이면 바로 채워준다.
    //if( pItemTemplet->m_Name.compare( L"부활석" ) == 0 )
    //{
    //    m_iNumResurrectionStone += pItemTemplet->m_Quantity;
    //    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    //    kPacket.m_iOK = NetError::NET_OK;
    //    kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
    //    SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
    //    return;
    //}

    //KItemInfo kItemInfo;
    //kItemInfo.m_iItemID = iItemID;
    //kItemInfo.m_iUsageType = pItemTemplet->m_PeriodType;
    //kItemInfo.m_iQuantity = ( int )kPacket_.m_usOrderQuantity;
    //kItemInfo.m_iEndurance = ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;
    //kItemInfo.m_cEnchantLevel = 0;
    //std::map< int, int >::iterator mitSocket;
    //mitSocket = m_mapSocketForCash.find( iItemID );
    //if( mitSocket != m_mapSocketForCash.end() )
    //{
    //    kItemInfo.m_vecItemSocket.push_back( mitSocket->second );
    //}
    //mapItem.insert( std::make_pair( iItemID, kItemInfo ) );

    //KDBE_INSERT_ITEM_REQ kPacketReq;
    //m_kInventory.PrepareInsert( mapItem, kPacketReq.m_mapInsertedItem, kPacketReq.m_vecUpdatedInventorySlot, kPacketReq.m_vecItemInfo );

    //if( !kPacketReq.m_vecItemInfo.empty() )
    //{
    //    kPacketReq.m_iUnitUID = GetCharUID();
    //    m_kInventory.FlushQuantityChange( kPacketReq.m_kItemQuantityUpdate.m_mapQuantityChange );
    //    m_kInventory.FlushEnduranceChange( kPacketReq.m_kItemEnduranceUpdate );
    //    m_kInventory.FlushDeletedItem( kPacketReq.m_kItemQuantityUpdate.m_vecDeleted );
    //    SendToGameDB( DBE_INSERT_PURCHASED_CASH_ITEM_REQ, kPacketReq );
    //}
    //else
    //{
    //    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    //    kPacket.m_iOK = NetError::NET_OK;
    //    kPacket.m_vecInventorySlotInfo = kPacketReq.m_vecUpdatedInventorySlot;
    //    SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
    //}
}

IMPL_ON_FUNC( ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK )
{
	if( kPacket_.m_ulResult != 1 )
	{
		START_LOG( cerr, L"캐쉬 아이템 가져 오기 실패." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_ulResult )
			<< END_LOG;

		//{{ 2012. 02. 22	박세훈	길드 이름 변경권
		if( m_kUserGuildManager.CheckGuildNameChanging() == true )
		{
			KELG_CHANGE_GUILD_NAME_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_NX_SHOP_03;
			kPacket.m_iUnitUID = GetCharUID();
			SendToLoginServer( DBE_CHANGE_GUILD_NAME_ACK, kPacket );
			return;
		}
		//}}

		KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NX_SHOP_03;
		SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
		return;
	}

	std::map< int, KItemInfo > mapItem; // 인벤토리에 들어갈 아이템들
	//{{ 2009. 9. 2  최육사		밀봉
	std::set< int > setSealCashItem;
	//}}

	//////////////////////////////////////////////////////////////////////////
	// 패키지 상품
	if( kPacket_.m_byteProductKind == 1 )
	{
		if( kPacket_.m_vecNXBTSubProductInfo.empty() )
		{
			START_LOG( cerr, L"패키지 상품인데 상품정보가 없다?" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( kPacket_.m_ulProductNo )
				<< END_LOG;
		}

		int iPackageItemID = _wtoi( kPacket_.m_wstrProductID.c_str() );
		if( iPackageItemID <= 0 )
		{
			START_LOG( cerr, L"아이템 ID 변환 실패." )
				<< BUILD_LOG( kPacket_.m_ulProductNo )
				<< BUILD_LOG( iPackageItemID )
				<< END_LOG;

			KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
			SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
			return;
		}

		// 패키지 캐쉬 아이템 가져오기 로그
		KELOG_GET_PURCHASED_CASH_ITEM_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		kPacketNot.m_iItemID = iPackageItemID;
		kPacketNot.m_sPeriod = kPacket_.m_usProductExpire;
		kPacketNot.m_iQuantity = kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
		kPacketNot.m_ucLevel = GetLevel();
		kPacketNot.m_cUnitClass = GetUnitClass();
		SendToLogDB( ELOG_GET_PURCHASED_CASH_ITEM_NOT, kPacketNot );

		std::vector< KNXBTSubProductInfo >::const_iterator vit;
		for( vit = kPacket_.m_vecNXBTSubProductInfo.begin(); vit != kPacket_.m_vecNXBTSubProductInfo.end(); ++vit )
		{
			const KNXBTSubProductInfo& subProductInfo = *vit;

			int iItemID = _wtoi( subProductInfo.m_wstrProductID.c_str() );
			if( iItemID <= 0 )
			{
				START_LOG( cerr, L"아이템 ID 변환 실패." )
					<< BUILD_LOG( kPacket_.m_ulProductNo )
					<< BUILD_LOG( iItemID )
					<< END_LOG;

				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
				SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
				return;
			}

			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
					<< BUILD_LOG( iItemID )
					<< END_LOG;

				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
				SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
				return;
			}

			// 캐쉬 아이템 가져오기 로그
			KELOG_GET_PURCHASED_CASH_ITEM_NOT kPacketNot;
			kPacketNot.m_iUnitUID = GetCharUID();
			kPacketNot.m_iItemID = iItemID;
			//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
			kPacketNot.m_sPeriod = subProductInfo.GetProductExpire();
			//}}
			kPacketNot.m_iQuantity = subProductInfo.m_usProductPieces * kPacket_.m_usOrderQuantity;
			kPacketNot.m_ucLevel = GetLevel();
			kPacketNot.m_cUnitClass = GetUnitClass();
			SendToLogDB( ELOG_GET_PURCHASED_CASH_ITEM_NOT, kPacketNot );

			//{{ 2012. 09. 25	박세훈	빙고 이벤트
			//}}

			switch( pItemTemplet->m_ItemID )
			{
			case CXSLItem::CI_RESURRECTION_STONE: // 부활석
				{
					// 부활석이면 바로 채워준다.
					m_iNumResurrectionStone += kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;

					KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
					kPacket.m_iOK = NetError::NET_OK;
					kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
					SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
					continue;
				}
				break;

			case CXSLItem::CI_EXPAND_INVENTORY: // 소지품 확장
				{
					//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
					// 소지품 확장이면 DB로 이벤트를 보낸다.
					int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_EQUIP, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_EQUIP ) ) ) );
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_ACCESSORY, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_ACCESSORY ) ) ) );
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_MATERIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_MATERIAL ) ) ) );
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_SPECIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_SPECIAL ) ) ) );
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUEST, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUEST ) ) ) );
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUICK_SLOT, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUICK_SLOT ) ) ) );
					//}}
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;

				//{{ 2011. 12. 13  김민성	퀵 슬롯 확장 아이템
			case CXSLItem::CI_EXPAND_QUICK_SLOT:
				{
					// 소지품 확장이면 DB로 이벤트를 보낸다.
					int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;

			case CXSLItem::CI_EXPAND_QUICK_SLOT_ARA:	// 아라만 사용 가능한 퀙 슬롯 확장 아이템
				{
					// 소지품 확장이면 DB로 이벤트를 보낸다.
					int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;
			case CXSLItem::CI_EXPAND_QUICK_SLOT_ELESIS:
				{
					// 소지품 확장이면 DB로 이벤트를 보낸다.
					int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;
				//}}

			case CXSLItem::CI_EXPAND_SKILL_SLOT: // 스킬 슬롯 체인지 메달
				{
					KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
					kPacket.m_iUnitUID		= GetCharUID();
					kPacket.m_iPeriodExpire = kPacket_.m_usProductExpire;

					kPacket.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

					//{{ 2011. 11. 30	최육사	패키지 상품 추가
					kPacket.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK;
					//}}
					SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
					continue; // 아이템 생성하지 않고 바로 리턴
				}
				break;

			case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
				//{{ 2012. 12. 19	최육사	아라 전용 캐쉬템
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ARA:
				//}}
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ELESIS:
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ADD:
				{
					KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
					kPacket.m_iUnitUID		= GetCharUID();
					kPacket.m_iPeriodExpire = 0;
					kPacket.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

					//{{ 2011. 11. 30	최육사	패키지 상품 추가
					kPacket.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK;
					//}}
					SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
					continue; // 아이템 생성하지 않고 바로 리턴
				} break;

			case CXSLItem::CI_CASH_SKILL_POINT_60_15:
			case CXSLItem::CI_CASH_SKILL_POINT_60_30:
			case CXSLItem::CI_CASH_SKILL_POINT_30_15:
			case CXSLItem::CI_CASH_SKILL_POINT_30_30:
			case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
			case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
			case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
			case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
			case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
			case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
			case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
			case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
			case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
				{
					const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( pItemTemplet->m_ItemID );

					if( iCSPoint <= 0 )
					{
						KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_RESET_SKILL_01;
						SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
						return;
					}

					if( true == m_kSkillTree.IsCashSkillPointExpired() )
					{
						KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
						kPacket.m_iUnitUID	= GetCharUID();
						kPacket.m_iCSPoint	= iCSPoint;
						//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
						kPacket.m_iPeriod	= subProductInfo.GetProductExpire();
						//}}
						kPacket.m_bUpdateEndDateOnly = false;
						//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
						kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
						//}}
						SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
					}
					else
					{
						if( iCSPoint == m_kSkillTree.GetMaxCSPoint() )
						{
							KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
							kPacket.m_iUnitUID = GetCharUID();
							kPacket.m_iCSPoint = iCSPoint;
							//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
							kPacket.m_iPeriod = subProductInfo.GetProductExpire();
							//}}
							kPacket.m_bUpdateEndDateOnly = true;
							//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
							kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
							//}}
							SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
						}
						else
						{
							START_LOG( cerr, L"현재 사용하고 있는 캐시스킬포인트와 다른 것으로 기간연장 하려고함!!" )
								<< BUILD_LOG( GetCharUID() )
								<< BUILD_LOG( pItemTemplet->m_ItemID )
								//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
								<< BUILD_LOG( subProductInfo.GetProductExpire() )
								//}}
								<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
								<< BUILD_LOG( m_kSkillTree.GetCSPointEndDateString() )
								<< END_LOG;

							KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
							kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
							SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
							return;
						}
					}

					return; // 아이템 생성하지 않고 바로 리턴
				} break;

				//{{ 2008. 12. 14  최육사	캐릭터 슬롯 확장
			case CXSLItem::CI_EXPAND_CHAR_SLOT_1:
			case CXSLItem::CI_EXPAND_CHAR_SLOT_2:
				{
					KDBE_EXPAND_CHAR_SLOT_REQ kPacket;
					kPacket.m_iExpandSlotSize = ( pItemTemplet->m_ItemID == CXSLItem::CI_EXPAND_CHAR_SLOT_1 ? 1 : 2 );
					kPacket.m_iCharSlotMax = CXSLUnit::CHAR_SLOT_MAX;
					SendToGameDB( DBE_EXPAND_CHAR_SLOT_REQ, kPacket );
					continue;
				}
				break;
				//}}

				//{{ 2009. 8. 7  최육사		은행
			case CXSLItem::CI_BANK_MEMBERSHIP_SILVER:
			case CXSLItem::CI_BANK_MEMBERSHIP_GOLD:
			case CXSLItem::CI_BANK_MEMBERSHIP_EMERALD:
			case CXSLItem::CI_BANK_MEMBERSHIP_DIAMOND:
			case CXSLItem::CI_BANK_MEMBERSHIP_PLATINUM:
				{
					// 은행 확장하러 가자!
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, CXSLInventory::SLOT_COUNT_ONE_LINE ) );
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;

			case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
				{
					// 은행 확장하러 가자!
					KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
					kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, ( CXSLInventory::SLOT_COUNT_ONE_LINE * 5 ) ) );
					SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
					continue;
				}
				break;
				//}}

				//{{ 2013. 2. 28	박세훈	 빙고 이벤트
				//}}

			case CXSLItem::CI_EXPAND_SKILL_PAGE:
				{
					SendExpandSkillPageReqToGameDB( ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK, kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity );
					continue;
				}
				break;

			case CXSLItem::CI_WARP_VIP_ITEM:
				{
					KDBE_INSERT_WARP_VIP_REQ kPacket;
					kPacket.m_iUnitUID	= GetCharUID();
					kPacket.m_iPeriod	= 30;
					SendToGameDB( DBE_INSERT_WARP_VIP_REQ, kPacket );

					return; // 아이템 생성하지 않고 바로 리턴
				}
				break;

			default:
				{
					//{{ 2009. 8. 4  최육사		봉인스킬
					std::vector<int> vecSkillID;
					if( SiCXSLSkillTree()->GetUnsealItemInfo( iItemID, GetUnitClass(), vecSkillID ) == true )
					{
						for( int iSkillID : vecSkillID )
						{
							KDBE_UNSEAL_SKILL_REQ kPacketToDB;
							kPacketToDB.m_iUnitUID = GetCharUID();
							kPacketToDB.m_iSkillID = iSkillID;
							SendToGameDB( DBE_UNSEAL_SKILL_REQ, kPacketToDB );
						}
						continue;
					}
				}
			}

			KItemInfo kItemInfo;
			kItemInfo.m_iItemID		= iItemID;
			kItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
			kItemInfo.m_iQuantity	= subProductInfo.m_usProductPieces * kPacket_.m_usOrderQuantity; // 상품 기본 수량 * 구매 수량
			kItemInfo.m_sEndurance	= ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;
			//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
			kItemInfo.m_sPeriod		= subProductInfo.GetProductExpire();
			//}}
			//{{ 2009. 8. 27  최육사	밀봉
			if( SiCXSLItemManager()->IsCanSealForCashItem( iItemID ) == true )
			{
				setSealCashItem.insert( pItemTemplet->m_ItemID );
			}
			//}}

			//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
			int iSocketOption = 0;
			if( m_kUserCashInventory.GetSocketOptionForCash( iItemID, iSocketOption ) )
			{
				int iItemType = static_cast<int>( pItemTemplet->m_ItemType );

				if( SiCXSLSocketItem()->CheckCashItemSocketID( iItemType, iSocketOption ) == true )
				{
					kItemInfo.m_vecItemSocket.push_back( iSocketOption );
				}
			}

			// 패키지 상품에 종속된 아이템들은 중복되는게 없는것을 전제함.
			mapItem.insert( std::make_pair( iItemID, kItemInfo ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 단일 상품
	else
	{
		const int iItemID = _wtoi( kPacket_.m_wstrProductID.c_str() );
		if( iItemID <= 0 )
		{
			START_LOG( cerr, L"아이템 ID 변환 실패." )
				<< BUILD_LOG( kPacket_.m_ulProductNo )
				<< BUILD_LOG( iItemID )
				<< END_LOG;

			KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
			SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
			return;
		}

		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
				<< BUILD_LOG( iItemID )
				<< END_LOG;

			KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
			SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
			return;
		}

		// 20080416. 여기까지 오면 정상적으로 아이템을 채워줘야함. 코드 수정시 이 점 주의 요망.

		// 캐쉬 아이템 가져오기 로그
		KELOG_GET_PURCHASED_CASH_ITEM_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		kPacketNot.m_iItemID = iItemID;
		kPacketNot.m_sPeriod = kPacket_.m_usProductExpire;
		kPacketNot.m_iQuantity = kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
		kPacketNot.m_ucLevel = GetLevel();
		kPacketNot.m_cUnitClass = GetUnitClass();
		SendToLogDB( ELOG_GET_PURCHASED_CASH_ITEM_NOT, kPacketNot );

		//{{ 2012. 09. 25	박세훈	빙고 이벤트
		//}}

		switch( pItemTemplet->m_ItemID )
		{
		case CXSLItem::CI_RESURRECTION_STONE: // 부활석
			{
				// 부활석이면 바로 채워준다.
				m_iNumResurrectionStone += kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;

				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::NET_OK;
				kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
				SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );

				// 실시간 부활석
				if( m_kUserCashInventory.IsCashResurrectionStone() )
				{
					KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ kPacketReq;
					kPacketReq.m_iUnitUID		= m_kUserCashInventory.GetCashRStoneUnitUID();
					kPacketReq.m_iUsedByUnitUID = GetCharUID();
					SendToCnRoom( ERM_RESURRECT_TO_CONTINUE_DUNGEON_REQ, kPacketReq );
				}
				return;
			}
			break;

		case CXSLItem::CI_EXPAND_INVENTORY: // 소지품 확장
			{
				//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_EQUIP, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_EQUIP ) ) ) );
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_ACCESSORY, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_ACCESSORY ) ) ) );
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_MATERIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_MATERIAL ) ) ) );
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_SPECIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_SPECIAL ) ) ) );
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUEST, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUEST ) ) ) );
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUICK_SLOT, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUICK_SLOT ) ) ) );
				//}}
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;

		case CXSLItem::CI_EXPAND_INVENTORY_EQUIP: // 소지품 확장( 장비 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_EQUIP, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_EQUIP ) ) ) );

				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_INVENTORY_ACCESSORY: // 소지품 확장( 악세사리 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_ACCESSORY, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_ACCESSORY ) ) ) );

				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_INVENTORY_QUICK_SLOT: // 소지품 확장( 소비 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUICK_SLOT, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUICK_SLOT ) ) ) );

				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_INVENTORY_MATERIAL: // 소지품 확장( 재료 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_MATERIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_MATERIAL ) ) ) );

				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_INVENTORY_QUEST: // 소지품 확장( 퀘스트 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_QUEST, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_QUEST ) ) ) );

				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_INVENTORY_SPECIAL: // 소지품 확장( 특수 )
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::SLOT_COUNT_ONE_LINE * kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_SPECIAL, nSlot + GetExpandedSlotSize( static_cast<int>( CXSLInventory::ST_SPECIAL ) ) ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
			//{{ 2011. 12. 13  김민성	퀵 슬롯 확장 아이템
		case CXSLItem::CI_EXPAND_QUICK_SLOT:
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;

		case CXSLItem::CI_EXPAND_QUICK_SLOT_ARA:	// 아라만 사용 가능한 퀙 슬롯 확장 아이템
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
		case CXSLItem::CI_EXPAND_QUICK_SLOT_ELESIS:
			{
				// 소지품 확장이면 DB로 이벤트를 보낸다.
				int nSlot = CXSLInventory::EXPAND_QUICK_SLOT;
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, nSlot ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
			//}}

		case CXSLItem::CI_EXPAND_SKILL_SLOT: // 스킬 슬롯 체인지 메달
			{
				KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
				kPacket.m_iUnitUID		= GetCharUID();
				kPacket.m_iPeriodExpire = kPacket_.m_usProductExpire;
				kPacket.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
				//{{ 2011. 11. 30	최육사	패키지 상품 추가
				kPacket.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK;
				//}}
				SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
				return; // 아이템 생성하지 않고 바로 리턴
			}
			break;

		case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
			//{{ 2012. 12. 19	최육사	아라 전용 캐쉬템
		case CXSLItem::CI_EXPAND_SKILL_SLOT_ARA:
			//}}
		case CXSLItem::CI_EXPAND_SKILL_SLOT_ELESIS:
		case CXSLItem::CI_EXPAND_SKILL_SLOT_ADD:

			{
				KDBE_EXPAND_SKILL_SLOT_REQ kPacket;
				kPacket.m_iUnitUID		= GetCharUID();
				kPacket.m_iPeriodExpire = 0;
				kPacket.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
				//{{ 2011. 11. 30	최육사	패키지 상품 추가
				kPacket.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK;
				//}}
				SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacket );
				return; // 아이템 생성하지 않고 바로 리턴
			}
			break;

		case CXSLItem::CI_CASH_SKILL_POINT_60_15:
		case CXSLItem::CI_CASH_SKILL_POINT_60_30:
		case CXSLItem::CI_CASH_SKILL_POINT_30_15:
		case CXSLItem::CI_CASH_SKILL_POINT_30_30:
		case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
		case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
		case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
		case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
			{
				const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( pItemTemplet->m_ItemID );

				if( iCSPoint <= 0 )
				{
					KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
					kPacket.m_iOK = NetError::ERR_RESET_SKILL_01;
					SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
					return;
				}

				if( true == m_kSkillTree.IsCashSkillPointExpired() )
				{
					KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
					kPacket.m_iUnitUID	= GetCharUID();
					kPacket.m_iCSPoint	= iCSPoint;
					kPacket.m_iPeriod	= kPacket_.m_usProductExpire;
					kPacket.m_bUpdateEndDateOnly = false;
					//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
					kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
					//}}
					SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
				}
				else
				{
					if( iCSPoint == m_kSkillTree.GetMaxCSPoint() )
					{
						KDBE_INSERT_CASH_SKILL_POINT_REQ kPacket;
						kPacket.m_iUnitUID = GetCharUID();
						kPacket.m_iCSPoint = iCSPoint;
						kPacket.m_iPeriod = kPacket_.m_usProductExpire;
						kPacket.m_bUpdateEndDateOnly = true;
						//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
						kPacket.m_iSkillPointItemID = pItemTemplet->m_ItemID;
						//}}
						SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacket );
					}
					else
					{
						START_LOG( cerr, L"현재 사용하고 있는 캐시스킬포인트와 다른 것으로 기간연장 하려고함!!" )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( pItemTemplet->m_ItemID )
							<< BUILD_LOG( kPacket_.m_usProductExpire )
							<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
							<< BUILD_LOG( m_kSkillTree.GetCSPointEndDateString() )
							<< END_LOG;

						KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
						SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
						return;
					}
				}

				return; // 아이템 생성하지 않고 바로 리턴
			}
			break;

			//{{ 2008. 12. 14  최육사	캐릭터 슬롯 확장
		case CXSLItem::CI_EXPAND_CHAR_SLOT_1:
		case CXSLItem::CI_EXPAND_CHAR_SLOT_2:
			{
				KDBE_EXPAND_CHAR_SLOT_REQ kPacket;
				kPacket.m_iExpandSlotSize = ( pItemTemplet->m_ItemID == CXSLItem::CI_EXPAND_CHAR_SLOT_1 ? 1 : 2 );
				kPacket.m_iCharSlotMax = CXSLUnit::CHAR_SLOT_MAX;
				SendToGameDB( DBE_EXPAND_CHAR_SLOT_REQ, kPacket );
				return;
			}
			break;
			//}}

			//{{ 2009. 8. 7  최육사		은행
		case CXSLItem::CI_BANK_MEMBERSHIP_SILVER:
		case CXSLItem::CI_BANK_MEMBERSHIP_GOLD:
		case CXSLItem::CI_BANK_MEMBERSHIP_EMERALD:
		case CXSLItem::CI_BANK_MEMBERSHIP_DIAMOND:
		case CXSLItem::CI_BANK_MEMBERSHIP_PLATINUM:
			{
				// 은행 확장하러 가자!
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, CXSLInventory::SLOT_COUNT_ONE_LINE ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;

		case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
			{
				// 은행 확장하러 가자!
				KDBE_EXPAND_INVENTORY_SLOT_REQ kPacket;
				kPacket.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, ( CXSLInventory::SLOT_COUNT_ONE_LINE * 5 ) ) );
				SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacket );
				return;
			}
			break;
			//}}

			//{{ 2010. 04. 09  최육사	기술의 노트
		case CXSLItem::CI_CASH_SKILL_NOTE_ITEM:
			{
				char cSkillNoteMaxPageNum = 0;
				if( m_kSkillTree.GetExpandSkillNotePage( GetLevel(), cSkillNoteMaxPageNum ) == true )
				{
					// 일단 기술의 노트 페이지 수를 저장하자!
					m_kSkillTree.UpdateSkillNoteMaxPageNum( cSkillNoteMaxPageNum );

					// 기술의 노트를 획득 하러 가자!
					KDBE_EXPAND_SKILL_NOTE_PAGE_REQ kPacketToDB;
					kPacketToDB.m_iUnitUID = GetCharUID();
					kPacketToDB.m_cExpandedMaxPageNum = cSkillNoteMaxPageNum;
					kPacketToDB.m_cRollBackMaxPageNum = 0;
					SendToGameDB( DBE_EXPAND_SKILL_NOTE_PAGE_REQ, kPacketToDB );
					return;
				}
				else
				{
					START_LOG( cerr, L"기술의 노트를 습득하려고 하는데 페이지 수 얻기를 실패하였습니다." )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( GetLevel() )
						<< END_LOG;
				}
			}
			break;
			//}}

		case CXSLItem::CI_CASH_SHARING_BACK_OPEN:
			{
				// 캐쉬로 은행 공유~
				KDBE_SHARING_BACK_OPEN_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID = GetCharUID();
				kPacketToDB.m_iUserUID = GetUID();
				kPacketToDB.m_iOpenType = KDBE_SHARING_BACK_OPEN_REQ::SBOT_CASH;
				SendToGameDB( DBE_SHARING_BACK_OPEN_REQ, kPacketToDB );

				return;
			}
			break;
			//{{ 2011. 04. 14	최육사	대리 상인
		case CXSLItem::CI_PERSONAL_SHOP_AGENCY:
			{
				KDBE_INSERT_PERIOD_PSHOP_AGENCY_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID = GetCharUID();
				kPacketToDB.m_sAgencyPeriod = kPacket_.m_usProductExpire;
				kPacketToDB.m_usEventID = EGS_GET_PURCHASED_CASH_ITEM_REQ;
                kPacketToDB.m_cShopType = SEnum::AST_PREMIUM;

				SendToGameDB( DBE_INSERT_PERIOD_PSHOP_AGENCY_REQ, kPacketToDB );
				return;
			}
			break;
			//}}

			//{{ 2012. 02. 22	박세훈	길드 이름 변경권
		case CXSLItem::CI_GUILD_NAME_CHANGE:
			{
				KELG_CHANGE_GUILD_NAME_REQ kPacket;
				kPacket.m_iUnitUID = GetCharUID();
				kPacket.m_iGuildUID = GetGuildUID();
				kPacket.m_wstrOldGuildName = m_kUserGuildManager.GetGuildName();
				// m_wstrNewGuildName은 LoginServer 쪽에서 얻을 것이다.
				SendToLoginServer( ELG_CHANGE_GUILD_NAME_REQ, kPacket );
				return;
			}
			break;

			//{{ 2013. 2. 28	박세훈	 빙고 이벤트
			//}}

		case CXSLItem::CI_EXPAND_SKILL_PAGE:
			{
				SendExpandSkillPageReqToGameDB( ENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK, kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity );
				return;
			}
			break;
		case CXSLItem::CI_WARP_VIP_ITEM:
			{
				KDBE_INSERT_WARP_VIP_REQ kPacket;
				kPacket.m_iUnitUID	= GetCharUID();
				kPacket.m_iPeriod	= 30;
				SendToGameDB( DBE_INSERT_WARP_VIP_REQ, kPacket );

				return; // 아이템 생성하지 않고 바로 리턴
			}
			break;

		default:
			//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
			//}}

			{
				//{{ 2009. 9. 22  최육사	전직캐쉬
				if( CXSLItem::IsJobChangeCashItem( iItemID ) )
				{
					// 전직 하러 가자!
					KDBE_CHANGE_UNIT_CLASS_REQ kPacket;
					kPacket.m_usEventID = EGS_BUY_CASH_ITEM_REQ;
					kPacket.m_iUnitUID = GetCharUID();
					kPacket.m_cUnitClass = CXSLItem::GetCashItemChangeUnitClass( pItemTemplet->m_ItemID );

					kPacket.m_iTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();

					int iDefaultSkill[6] = {0,};
					if( SiCXSLSkillTree()->GetUnitClassDefaultSkill( kPacket.m_cUnitClass, iDefaultSkill[0], iDefaultSkill[1], iDefaultSkill[2], iDefaultSkill[3], iDefaultSkill[4], iDefaultSkill[5] ) == false )
					{
						START_LOG( cerr, L"전직시 지급하려는 기본 스킬이 이상함" )
							<< BUILD_LOG( kPacket.m_iUnitUID )
							<< BUILD_LOGc( kPacket.m_cUnitClass )
							<< END_LOG;
					}

					if( CXSLUnit::IsInitNormalJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
					{
						kPacket.m_iNewDefaultSkill1 = iDefaultSkill[0];
						kPacket.m_iNewDefaultSkill2 = iDefaultSkill[1];
					}
					else if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
					{
						kPacket.m_iNewDefaultSkill1 = iDefaultSkill[2];
						kPacket.m_iNewDefaultSkill2 = iDefaultSkill[3];
					}
					else if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(kPacket.m_cUnitClass) ) == true )
					{
						kPacket.m_iNewDefaultSkill1 = iDefaultSkill[4];
						kPacket.m_iNewDefaultSkill2 = iDefaultSkill[5];
					}

					SendToGameDB( DBE_CHANGE_UNIT_CLASS_REQ, kPacket );
					return;
				}
				//}}
				//{{ 2011. 11. 21  김민성	전직 변경 아이템
				[[maybe_unused]] int iUnitClass = 0;
				if( CXSLItem::GetClassChangeCashItem( iItemID ) != CXSLUnit::UC_NONE )
				{
					KDBE_BUY_UNIT_CLASS_CHANGE_REQ kPacketToDB;

					if( GetUnitClassChangeInfo( iItemID, kPacketToDB ) == true )
					{
						SendToGameDB( DBE_BUY_UNIT_CLASS_CHANGE_REQ, kPacketToDB );
					}
					else
					{
						START_LOG( cerr, L"캐쉬 아이템 픽업 과정에서 실패가 났다! 복귀 해야 할 듯 한데!!!" )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( iItemID )
							<< END_LOG;
					}
					return;
				}
				//}}

				//{{ 2009. 8. 4  최육사		봉인스킬
				std::vector<int> vecSkillID;
				if( SiCXSLSkillTree()->GetUnsealItemInfo( iItemID, GetUnitClass(), vecSkillID ) == true )
				{
					for( int iSkillID : vecSkillID )
					{
						KDBE_UNSEAL_SKILL_REQ kPacketToDB;
						kPacketToDB.m_iUnitUID = GetCharUID();
						kPacketToDB.m_iSkillID = iSkillID;
						SendToGameDB( DBE_UNSEAL_SKILL_REQ, kPacketToDB );
					}
					return;
				}
				//}}
			}
		}

		KItemInfo kItemInfo;
		kItemInfo.m_iItemID		= iItemID;
		kItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
		kItemInfo.m_iQuantity	= kPacket_.m_usProductPieces * kPacket_.m_usOrderQuantity;
		kItemInfo.m_sEndurance	= ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;
		kItemInfo.m_sPeriod		= kPacket_.m_usProductExpire;
		//{{ 2009. 8. 27  최육사	밀봉
		if( SiCXSLItemManager()->IsCanSealForCashItem( iItemID ) == true )
		{
			setSealCashItem.insert( pItemTemplet->m_ItemID );
		}
		//}}
		//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
		int iSocketOption = 0;
		if( m_kUserCashInventory.GetSocketOptionForCash( iItemID, iSocketOption ) )
		{
			int iItemType = static_cast<int>( pItemTemplet->m_ItemType );

			if( SiCXSLSocketItem()->CheckCashItemSocketID( iItemType, iSocketOption ) == true )
			{
				kItemInfo.m_vecItemSocket.push_back( iSocketOption );
			}
		}
		//}}
		mapItem.insert( std::make_pair( iItemID, kItemInfo ) );
	}

	// 인벤토리에 아이템 넣을 준비
	KDBE_INSERT_ITEM_REQ kPacketReq;
	//{{ 2010. 9. 8	최육사	아이템 획득 사유
	kPacketReq.m_cGetItemReason = SEnum::GIR_BUY_CASH_ITEM;

	m_kInventory.PrepareInsert( mapItem, kPacketReq.m_mapInsertedItem, kPacketReq.m_vecUpdatedInventorySlot, kPacketReq.m_vecItemInfo );

	if( !kPacketReq.m_vecItemInfo.empty() )
	{
		//{{ 2009. 9. 2  최육사		봉인
		kPacketReq.m_setSealCashItem = setSealCashItem;
		//}}
		kPacketReq.m_iUnitUID = GetCharUID();
		m_kInventory.FlushQuantityChange( kPacketReq.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.FlushEnduranceChange( kPacketReq.m_kItemEnduranceUpdate );
		m_kInventory.FlushDeletedItem( kPacketReq.m_kItemQuantityUpdate.m_vecDeleted );
		SendToGameDB( DBE_INSERT_PURCHASED_CASH_ITEM_REQ, kPacketReq );
	}
	else
	{
		KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::NET_OK;
		kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
		kPacket.m_vecInventorySlotInfo = kPacketReq.m_vecUpdatedInventorySlot;
		SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
	}

	// 캐쉬 아이템 통계
	std::map< int, int >::const_iterator mit;
	for( mit = kPacketReq.m_mapInsertedItem.begin(); mit != kPacketReq.m_mapInsertedItem.end(); ++mit )
	{
		KStatisticsKey kKey;
		kKey.m_vecIntKey.push_back( mit->first );
		KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_MALL, mit->second );
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
		KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_MALL, mit->second );
	}

	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	// 아이템 어뷰저
	m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_CASH_ITEM, kPacketReq.m_mapInsertedItem );
	//}}
}


IMPL_ON_FUNC( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK );

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 패키지 구성 상품 pick up 요청" )
			<< BUILD_LOG( GetName() );

		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_CONTENT_00;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacketAck );
		return;
	}
	//}}

	//{{ 2013. 2. 5	박세훈	빙고 이벤트
	//}}

	// 1. 아직 이전 구매작업이 끝나지 않았다.
	if( !m_kUserCashInventory.IsEmptyPickUpPackageInfo() )
	{
		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NX_SHOP_09;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
		return;
	}

	// 2. 패키지 상품의 ItemID얻기
	int iItemID = kPacket_.m_iProductID;
	m_veciPackageBuyCheck.clear();
	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"아이템 ID 변환 실패." )
			<< BUILD_LOG( kPacket_.m_ulPackageProductNo )
			<< BUILD_LOG( kPacket_.m_wstrPackageProductID )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
		return;
	}

	switch ( GetUnitType() )
	{

	case CXSLUnit::UT_ARA:

	case CXSLUnit::UT_ELESIS:

	case CXSLUnit::UT_ADD:
		{
			switch( iItemID )
			{
			case 160570:		// 리폼 웨딩 무기 큐브
			case 160571:		// 리폼 웨딩 상의 큐브
			case 160572:		// 리폼 웨딩 하의 큐브
			case 160573:		// 리폼 웨딩 장갑 큐브
			case 160574:		// 리폼 웨딩 신발 큐브
			case 160575:		// 리폼 웨딩 헤어 큐브
				{
					KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
					kPacket.m_iOK = NetError::ERR_NX_SHOP_07;
					SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
					return;
				}break;
			}

		} break;

	default:
		break;
	}

	//{{ 2012. 12. 14  캐시 인벤토리에서 아라가 사용 불가능한 아이템 가져오는거 막는 기능 - 김민성

	switch( iItemID )
	{
		case 261590:		// 천년 여우의 스킬 슬롯 체인지 메달 패키지
		case 261600:		// 천년 여우의 마법의 목걸이(II) 패키지
		case 261610:		// 천년 여우의 퀵슬롯 확장권 패키지
		case 261620:		// 천년 여우의 축복 패키지
		{
			if( GetUnitType() != CXSLUnit::UT_ARA )
			{
				KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_07;
				SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
				return;
			}
		}break;
	}
	//}}

	if( GetUnitType() == CXSLUnit::UT_ELESIS )
	{
		switch( iItemID )
		{
		case 160570:		// 리폼 웨딩 무기 큐브
		case 160571:		// 리폼 웨딩 상의 큐브
		case 160572:		// 리폼 웨딩 하의 큐브
		case 160573:		// 리폼 웨딩 장갑 큐브
		case 160574:		// 리폼 웨딩 신발 큐브
		case 160575:		// 리폼 웨딩 헤어 큐브
			{
				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_07;
				SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
				return;
			}break;
		}
	}

	switch( iItemID )
	{
	case 261664:		//	엘리시스 붉은 기사단 패키지
	case 261665:		//	스킬 슬롯 체인지 메달(엘리시스 전용) 패키지
	case 261666:		//	마법의 목걸이(II)(엘리시스 전용) 패키지
	case 261667:		//	퀵슬롯 확장권(엘리시스 전용) 패키지
		{
			if( GetUnitType() != CXSLUnit::UT_ELESIS )
			{
				KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_07;
				SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
				return;
			}
		}break;
	}

	std::map< int, int > mapItem; // 인벤토리 검사용

	// 3. 패키지 상품인지 ProductKind를 검사한다.
	if( kPacket_.m_byteProductKind != 1  ||  kPacket_.m_vecPickUpPackageInfo.empty() )
	{
		START_LOG( cerr, L"패키지 상품이 아닌데 패키지 패킷으로 왔다?" )
			<< BUILD_LOG( kPacket_.m_ulPackageProductNo )
			<< BUILD_LOG( kPacket_.m_byteProductKind )
			<< END_LOG;

		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
		return;
	}
	else
	{
		std::vector< KNXBTPickUpPackageInfo >::const_iterator vitPU;
		for( vitPU = kPacket_.m_vecPickUpPackageInfo.begin(); vitPU != kPacket_.m_vecPickUpPackageInfo.end(); ++vitPU )
		{
			const KNXBTPickUpPackageInfo& kPickUpPackageInfo = *vitPU;

			// 4. 아이템 스크립트 검사
			int iMemberItemID = kPickUpPackageInfo.m_iSubProductNo;

			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iMemberItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
					<< BUILD_LOG( iMemberItemID )
					<< END_LOG;

				KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
				SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
				return;
			}

			// 5. 장착 가능한 클래스인지 검사한다.
			if( !m_kInventory.CompareUnitClass( iMemberItemID ) )
			{
				KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_NX_SHOP_07;
				SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
				return;
			}

			// 6. 가져오려는 아이템이 인벤토리에 있으면서 기간제이면 막는다.
			if( m_kInventory.IsExistWithBank( iMemberItemID ) )
			{
				// 구성 상품중에 수량성 아이템이 있을 수 있으므로 아래와 같이 예외처리함.
				//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
				if( kPickUpPackageInfo.GetProductExpire() > 0  &&  pItemTemplet->m_PeriodType != CXSLItem::PT_QUANTITY )
				//}}
				{
					KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
					kPacket.m_iOK = NetError::ERR_NX_SHOP_08;
					SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
					return;
				}
			}

			// 캐릭터 인벤에서는 아이템으로 존재하지 않는 캐시아이템의 경우에 중복구매에 대한 예외 처리
			switch( iMemberItemID )
			{
				//{{ 2011. 11. 30	최육사	패키지 상품 추가
			case CXSLItem::CI_EXPAND_SKILL_SLOT_IN_PACKAGE: // 4.1 가져오려는 아이템이 스킬슬롯B(기간제) 이면, 이미 스킬슬롯B(기간제, 혹은 영구)를 사용중이면 가져올 수 없게 막는다
				{
					KUserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE eSkillSlotBExpireState = m_kSkillTree.GetSkillSlotBExpirationState();
					if( KUserSkillTree::SSBES_EXPIRED != eSkillSlotBExpireState )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_56;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}
				break;

			case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
				//{{ 2012. 12. 19	최육사	아라 전용 캐쉬템
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ARA:
				//}}
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ELESIS:
			case CXSLItem::CI_EXPAND_SKILL_SLOT_ADD:
				{
					KUserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE eSkillSlotBExpireState = m_kSkillTree.GetSkillSlotBExpirationState();
					if( KUserSkillTree::SSBES_PERMANENT == eSkillSlotBExpireState )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_57;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}
				break;
				//}}

				//{{ 2011. 12. 21	최육사	은행 확장 패키지 구성 상품
			case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
				{
					// 일반 멤버쉽이 아니면 구매할 수 없다!
					if( m_kInventory.GetBankMemberShip() != CXSLInventory::MPB_NORMAL )
					{
						START_LOG( cwarn, L"구매 가능한 등급이 아닙니다." )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOG( m_kInventory.GetBankMemberShip() )
							<< END_LOG;

						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_36;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}

					// 인벤토리에 루리엘 신용보증서가 있으면 마지막 회원권은 구매할 수 없다!
					if( m_kInventory.IsExistWithBank( CXSLItem::SI_BANK_MEMBERSHIP_UPGRADE ) == true )
					{
						START_LOG( cwarn, L"루리엘 신용보증서가 인벤토리에 있으면 마지막 회원권은 구매못함." )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetCharName() )
							<< END_LOG;

						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_39;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}

					KNXBTProductInfo kProductInfo;
					if( !SiKNexonBillingTCPManager()->GetProductInfo( kPickUpPackageInfo.m_ulProductNo, kProductInfo ) )
					{
						START_LOG( cerr, L"프로덕트 정보 추출 실패." )
							<< BUILD_LOG( GetCharUID() )
							<< BUILD_LOG( GetCharName() )
							<< BUILD_LOG( kPickUpPackageInfo.m_ulProductNo )
							<< END_LOG;

						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_00;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}

					// 주문수량이 한개 이상은 안되!
					//if( vit->m_usOrderQuantity > 1 )
					//{
					//	START_LOG( cerr, L"은행 회원권은 1개이상 주문할 수 없다." )
					//		<< BUILD_LOG( GetCharUID() )
					//		<< BUILD_LOG( GetCharName() )
					//		<< BUILD_LOG( vit->m_usOrderQuantity )
					//		<< END_LOG;

					//	KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
					//	kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_37;
					//	SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
					//	return;
					//}

					// 확장가능한지 검사하자!
					const int iBankSlotToExpand = CXSLInventory::SLOT_COUNT_ONE_LINE * 5; // 8칸씩 5줄

					if( m_kInventory.IsAbleToExpandBank( iBankSlotToExpand ) == false )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_36;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}
				break;
				//}}
				//}}
				//{{ 2012. 2. 15  김민성	퀵 슬롯 확장 아이템 - 패키지상품
			case CXSLItem::CI_EXPAND_QUICK_SLOT:
				{
					int iInventorySlotToExpand = 1;
					if( !m_kInventory.IsAbleToExpandSlot( CXSLInventory::ST_E_QUICK_SLOT, iInventorySlotToExpand ) )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_66;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}break;

			case CXSLItem::CI_EXPAND_QUICK_SLOT_ARA:	// 아라만 사용 가능한 퀙 슬롯 확장 아이템
				{
					int iInventorySlotToExpand = 1;
					if( !m_kInventory.IsAbleToExpandSlot( CXSLInventory::ST_E_QUICK_SLOT, iInventorySlotToExpand ) )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_66;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}break;
			case CXSLItem::CI_EXPAND_QUICK_SLOT_ELESIS:	// 엘리시스만 사용 가능한 퀙 슬롯 확장 아이템
				{
					int iInventorySlotToExpand = 1;
					if( !m_kInventory.IsAbleToExpandSlot( CXSLInventory::ST_E_QUICK_SLOT, iInventorySlotToExpand ) )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_66;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}break;
					//}}

			case CXSLItem::CI_EXPAND_SKILL_PAGE:
				{
					if ( !m_kSkillTree.CanExpandSkillPage() )
					{
						KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
						kPacket.m_iOK = NetError::ERR_SKILL_PAGE_02;
						SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
						return;
					}
				}break;
			}

			// 8. 인벤토리에 여유공간 검사를 위한 준비
			std::map< int, int >::iterator mitFind;
			mitFind = mapItem.find( iMemberItemID );
			if( mitFind == mapItem.end() )
			{
				mapItem.insert( std::make_pair( iMemberItemID, kPickUpPackageInfo.m_usProductPieces ) );
			}
			else
			{
				mitFind->second += kPickUpPackageInfo.m_usProductPieces;
			}
		}
	}

	// 9. 인벤토리의 여유공간 검사
	if( !m_kInventory.IsEnoughSpaceExist( mapItem ) )
	{
		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_NX_SHOP_02;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
		return;
	}

	// 10. 아이템에 부여될 소켓옵션
	//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
	if( m_kUserCashInventory.SetSocketForCash( kPacket_.m_mapSocketForCash, kPacket_.m_mapSocketGroupIDForCash ) == false )
	{
		// 정상적이지 않는 소켓 정보를 셋팅하려 합니다.
		KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_SOCKET_ITEM_09;
		SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
		return;
	}
	//}

	//////////////////////////////////////////////////////////////////////////
	m_kUserCashInventory.ClearPickUpPackageInfo();

	std::vector< KNXBTPickUpPackageInfo >::const_iterator vit;
	for( vit = kPacket_.m_vecPickUpPackageInfo.begin(); vit != kPacket_.m_vecPickUpPackageInfo.end(); ++vit )
	{
		const KNXBTPickUpPackageInfo& kPickUpPackageInfo = *vit;


		KEBILL_PICK_UP_REQ kPacket;
		kPacket.m_iTransNo = kPacket_.m_ulOrderNo;
		kPacket.m_iProductNo = kPacket_.m_ulPackageProductNo;
		kPacket.m_iUnitUID = GetCharUID();
		kPacket.m_mapSocketForCash =kPacket_.m_mapSocketForCash;
		kPacket.m_byteProductKind = kPacket_.m_byteProductKind;
		kPacket.m_vecPickUpPackageInfo = kPacket_.m_vecPickUpPackageInfo;
		kPacket.m_iSubProductNo = kPickUpPackageInfo.m_ulProductNo;
		kPacket.m_iSubProductID = kPickUpPackageInfo.m_iSubProductNo;

		if ( std::find( m_veciPackageBuyCheck.begin(), m_veciPackageBuyCheck.end(), kPickUpPackageInfo.m_iSubProductNo ) == m_veciPackageBuyCheck.end() )
		{

			m_veciPackageBuyCheck.push_back( kPickUpPackageInfo.m_iSubProductNo );

			switch( kPickUpPackageInfo.m_iSubProductNo )
			{
				case CXSLItem::CI_CASH_SKILL_POINT_60_15:
				case CXSLItem::CI_CASH_SKILL_POINT_60_30:
				case CXSLItem::CI_CASH_SKILL_POINT_30_15:
				case CXSLItem::CI_CASH_SKILL_POINT_30_30:
				case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
				case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
				case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
				case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
				case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
				case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
				case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
				case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
				case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
					{
						if( false == m_kSkillTree.IsCashSkillPointExpired() )
						{
							const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( kPickUpPackageInfo.m_iSubProductNo );

							if( iCSPoint <= 0 )
							{
								KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
								kPacket.m_iOK = NetError::ERR_RESET_SKILL_01;
								SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
								return;
							}

							if( iCSPoint != m_kSkillTree.GetMaxCSPoint() )
							{
								KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
								kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_32;				// fix!! 에러메세지 수정
								SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
								return;
							}
						}
					}
			}
		}
		else
		{
			continue;
		}

		m_kUserCashInventory.AddPickUpPackageInfo( kPickUpPackageInfo.m_ulProductNo,  kPickUpPackageInfo );
		SendToKOGBillingDB( EBILL_PICK_UP_REQ, kPacket );

	}
	//////////////////////////////////////////////////////////////////////////

	/*
	KDBE_GET_PURCHASED_CASH_ITEM_REQ kPacket;
	kPacket.m_iOrderNo			= kPacket_.m_iOrderNo;
	kPacket.m_iPackageProductNo = kPacket_.m_iPackageProductNo;
	kPacket.m_iProductNo		= kPacket_.m_iProductNo;
	kPacket.m_nQuantity			= kPacket_.m_nQuantity;
	kPacket.m_iItemID			= iItemID;

	SendToNXShopDB( DBE_GET_PURCHASED_CASH_ITEM_REQ, kPacket );
	*/
}

IMPL_ON_FUNC( ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	int iItemID{};
	const CXSLItem::ItemTemplet* pItemTemplet{};
	KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
	KNXBTPickUpPackageInfo kPickUpPackageInfo;
	KNXBTProductInfo kMemberProductInfo;
	KDBE_INSERT_PURCHASED_CASH_PACKAGE_REQ kPacketReq;
	std::map< int, KItemInfo > mapItem; // 인벤토리에 들어갈 아이템들

	// 1. 빌링 결과 체크
	if( kPacket_.m_ulResult != 1 )
	{
		START_LOG( cerr, L"캐쉬 아이템 가져 오기 실패." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_ulResult )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_NX_SHOP_03;
		goto end_proc;
	}

	// 2. 임시 저장 패키지 정보 얻기
	if( !m_kUserCashInventory.GetPickUpPackageInfo( kPacket_.m_ulProductNo, kPickUpPackageInfo ) )
	{
		START_LOG( cerr, L"임시 저장한 패키지 구성상품 정보가 없습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_NX_SHOP_03;
		goto end_proc;
	}

	// 3. ItemID 변환
	iItemID = kPacket_.m_ulSubProductNo;
	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"아이템 ID 변환 실패." )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
		goto end_proc;
	}

	pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
		goto end_proc;
	}

	// 4. 예외 상황 제거. 패키지 상품의 멤버가 아닌데 포함되어 있는 경우 에러로 예외 처리한다
	switch( pItemTemplet->m_ItemID )
	{
	case CXSLItem::CI_EXPAND_INVENTORY:
	case CXSLItem::CI_EXPAND_INVENTORY_EQUIP:
	case CXSLItem::CI_EXPAND_INVENTORY_ACCESSORY:
	case CXSLItem::CI_EXPAND_INVENTORY_QUICK_SLOT:
	case CXSLItem::CI_EXPAND_INVENTORY_MATERIAL:
	case CXSLItem::CI_EXPAND_INVENTORY_QUEST:
	case CXSLItem::CI_EXPAND_INVENTORY_SPECIAL:
	case CXSLItem::CI_EXPAND_SKILL_SLOT:
		//{{ 2011. 11. 30	최육사	패키지 상품 추가
		//}}
	case CXSLItem::CI_EXPAND_CHAR_SLOT_1:
	case CXSLItem::CI_EXPAND_CHAR_SLOT_2:

	case CXSLItem::CI_EXPAND_SKILL_PAGE:
		{
			kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
			goto end_proc;
		}
		break;

		//{{ 2011. 11. 30	최육사	패키지 상품 추가
	case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
		//{{ 2012. 12. 19	최육사	아라 전용 캐쉬템
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ARA:
		//}}
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ELESIS:
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ADD:

		{
			if( kPickUpPackageInfo.GetProductExpire() != 0 )
			{
				kPacket.m_iOK = NetError::ERR_NX_SHOP_01;
				goto end_proc;
			}
		}
		break;
		//}}
	}



	// 20080416. 여기까지 오면 정상적으로 아이템을 채워줘야함. 코드 수정시 이 점 주의 요망.

	//{{ 2012. 09. 25	박세훈	빙고 이벤트
	//}}

	// 5. 캐쉬 아이템 가져오기 로그
	{
		KELOG_GET_PURCHASED_CASH_ITEM_NOT kPacketNot;
		kPacketNot.m_iUnitUID = GetCharUID();
		kPacketNot.m_iItemID = iItemID;
		//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
		kPacketNot.m_sPeriod = kPickUpPackageInfo.GetProductExpire();
		//}}
		kPacketNot.m_iQuantity = kPickUpPackageInfo.m_usProductPieces;
		kPacketNot.m_ucLevel = GetLevel();
		kPacketNot.m_cUnitClass = GetUnitClass();
		SendToLogDB( ELOG_GET_PURCHASED_CASH_ITEM_NOT, kPacketNot );
	}


	switch( pItemTemplet->m_ItemID )
	{
	case CXSLItem::CI_RESURRECTION_STONE:
		{
			// 부활석이면 바로 채워준다.
			m_iNumResurrectionStone += kPickUpPackageInfo.m_usProductPieces;

			kPacket.m_iOK = NetError::NET_OK;
			kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;


			goto end_proc;
		}
		break;

		//{{ 2011. 11. 30	최육사	패키지 상품 추가
	case CXSLItem::CI_EXPAND_SKILL_SLOT_PERMANENT:
		//{{ 2012. 12. 19	최육사	아라 전용 캐쉬템
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ARA:
		//}}
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ELESIS:
	case CXSLItem::CI_EXPAND_SKILL_SLOT_ADD:

		{
			KDBE_EXPAND_SKILL_SLOT_REQ kPacketToDB;
			kPacketToDB.m_iUnitUID		= GetCharUID();
			kPacketToDB.m_iPeriodExpire = 0;
			kPacketToDB.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
			kPacketToDB.m_usEventID		= ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}
		break;

	case CXSLItem::CI_EXPAND_SKILL_SLOT_IN_PACKAGE: // 스킬 슬롯 체인지 메달
		{
			KDBE_EXPAND_SKILL_SLOT_REQ kPacketToDB;
			kPacketToDB.m_iUnitUID		= GetCharUID();
			kPacketToDB.m_iPeriodExpire = kPickUpPackageInfo.GetProductExpire();
			kPacketToDB.m_usTheNumberOfSkillPagesAvailable = m_kSkillTree.GetTheNumberOfSkillPagesAvailable();
			kPacketToDB.m_usEventID		= ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			SendToGameDB( DBE_EXPAND_SKILL_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}
		break;
		//}}

		//{{ 2011. 12. 21	최육사	은행 확장 패키지 구성 상품
	case CXSLItem::CI_BANK_MEMBERSHIP_ONE_PACKAGE:
		{
			// 은행 확장하러 가자!
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacketToDB;
			kPacketToDB.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_BANK, ( CXSLInventory::SLOT_COUNT_ONE_LINE * 5 ) ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}
		break;
		//}}
		//{{ 2012. 2. 15  김민성	퀵 슬롯 확장 아이템 - 패키지상품
	case CXSLItem::CI_EXPAND_QUICK_SLOT:
		{
			// 은행 확장하러 가자!
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacketToDB;
			kPacketToDB.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, ( CXSLInventory::EXPAND_QUICK_SLOT ) ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}break;

	case CXSLItem::CI_EXPAND_QUICK_SLOT_ARA:	// 아라만 사용 가능한 퀙 슬롯 확장 아이템
		{
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacketToDB;
			kPacketToDB.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, ( CXSLInventory::EXPAND_QUICK_SLOT ) ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}break;

	case CXSLItem::CI_EXPAND_QUICK_SLOT_ELESIS:	// 엘리시스만 사용 가능한 퀙 슬롯 확장 아이템
		{
			KDBE_EXPAND_INVENTORY_SLOT_REQ kPacketToDB;
			kPacketToDB.m_usEventID = ENX_BT_NISMS_INVENTORY_PICK_UP_FOR_PACKAGE_ACK;
			kPacketToDB.m_iUnitUID = GetCharUID();
			kPacketToDB.m_mapExpandedSlot.insert( std::make_pair( ( int )CXSLInventory::ST_E_QUICK_SLOT, ( CXSLInventory::EXPAND_QUICK_SLOT ) ) );
			SendToGameDB( DBE_EXPAND_INVENTORY_SLOT_REQ, kPacketToDB );

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}break;
		//}}

		//{{ 2013. 2. 28	박세훈	 빙고 이벤트
		//}}
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
		{
			const int iCSPoint = SiCXSLItemManager()->GetItemCSPoint( iItemID );
			if( iCSPoint <= 0 )
			{
				kPacket.m_iOK =  NetError::ERR_RESET_SKILL_01;
				goto end_proc;
			}

			if( true == m_kSkillTree.IsCashSkillPointExpired() )
			{
				KDBE_INSERT_CASH_SKILL_POINT_REQ kPacketToDB;
				kPacketToDB.m_iUnitUID	= GetCharUID();
				kPacketToDB.m_iCSPoint	= iCSPoint;;
				kPacketToDB.m_iPeriod	= kPickUpPackageInfo.GetProductExpire();
				kPacketToDB.m_bUpdateEndDateOnly = false;
				kPacketToDB.m_iSkillPointItemID = iItemID;
				SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacketToDB );
			}
			else
			{
				if( iCSPoint == m_kSkillTree.GetMaxCSPoint() )
				{
					KDBE_INSERT_CASH_SKILL_POINT_REQ kPacketToDB;
					kPacketToDB.m_iUnitUID = GetCharUID();
					kPacketToDB.m_iCSPoint = iCSPoint;
					kPacketToDB.m_iPeriod = kPickUpPackageInfo.GetProductExpire();
					kPacketToDB.m_bUpdateEndDateOnly = true;
					kPacketToDB.m_iSkillPointItemID = iItemID;
					SendToGameDB( DBE_INSERT_CASH_SKILL_POINT_REQ, kPacketToDB );
				}
				else
				{
					START_LOG( cerr, L"현재 사용하고 있는 캐시스킬포인트와 다른 것으로 기간연장 하려고함!!" )
						<< BUILD_LOG( GetCharUID() )
						<< BUILD_LOG( pItemTemplet->m_ItemID )
						<< BUILD_LOG( kPickUpPackageInfo.GetProductExpire() )
						<< BUILD_LOG( m_kSkillTree.GetMaxCSPoint() )
						<< BUILD_LOG( m_kSkillTree.GetCSPointEndDateString() )
						<< END_LOG;

					kPacket.m_iOK =  NetError::ERR_NX_SHOP_01;
					goto end_proc;
				}
			}

			kPacket.m_iOK = NetError::NET_OK;
			goto end_proc;
		}
		break;

	default:
		// 7. 인벤토리에 넣기
		{
			KItemInfo kItemInfo;
			kItemInfo.m_iItemID	   = iItemID;
			kItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
			kItemInfo.m_iQuantity  = kPickUpPackageInfo.m_usProductPieces;
			kItemInfo.m_sEndurance = ( pItemTemplet->m_PeriodType == CXSLItem::PT_ENDURANCE ) ? pItemTemplet->m_Endurance : 0;
			//{{ 2009. 12. 2  최육사	캐쉬템기간타입변경
			kItemInfo.m_sPeriod    = kPickUpPackageInfo.GetProductExpire();
			//}}
			//{{ 2009. 9. 2  최육사		밀봉
			if( SiCXSLItemManager()->IsCanSealForCashItem( iItemID ) == true )
			{
				kPacketReq.m_setSealCashItem.insert( pItemTemplet->m_ItemID );
			}
			//}}
			//{{ 2012. 11. 28 캐시 아이템 소켓 옵션 기능 수정 - 김민성
			int iSocketOption = 0;
			if( m_kUserCashInventory.GetSocketOptionForCash( iItemID, iSocketOption ) )
			{
				int iItemType = static_cast<int>( pItemTemplet->m_ItemType );

				if( SiCXSLSocketItem()->CheckCashItemSocketID( iItemType, iSocketOption ) == true )
				{
					kItemInfo.m_vecItemSocket.push_back( iSocketOption );
				}
			}
			//}



			mapItem.insert( std::make_pair( iItemID, kItemInfo ) );

			// 인벤토리에 아이템 넣을 준비
			m_kInventory.PrepareInsert( mapItem, kPacketReq.m_mapInsertedItem, kPacketReq.m_vecUpdatedInventorySlot, kPacketReq.m_vecItemInfo );
		}
		break;
	}

	// 캐쉬 아이템 통계
	{
		std::map< int, int >::const_iterator mit;
		for( mit = kPacketReq.m_mapInsertedItem.begin(); mit != kPacketReq.m_mapInsertedItem.end(); ++mit )
		{
			KStatisticsKey kKey;
			kKey.m_vecIntKey.push_back( mit->first );
			KSIManager.IncreaseCount( KStatistics::SI_LOC_ITEM, kKey, KStatistics::SI_ITEM_MALL, mit->second );
			//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
			KSIManager.IncreaseCount( KStatistics::SI_ITEM_DB, kKey, KStatistics::SI_ITEM_MALL, mit->second );
			//}}
		}

		//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
		// 아이템 어뷰저
		m_kUserAbuserManager.CheckItemAbuser( GetThisPtr<KGSUser>(), KAbuserLogManager::RS_CASH_ITEM, kPacketReq.m_mapInsertedItem );
		//}}
	}

	// 8. DB업데이트 또는 클라이언트 통보
	if( !kPacketReq.m_vecItemInfo.empty() )
	{
		kPacketReq.m_iUnitUID = GetCharUID();
		kPacketReq.m_ulProductNo = kPacket_.m_ulProductNo;
		m_kInventory.FlushQuantityChange( kPacketReq.m_kItemQuantityUpdate.m_mapQuantityChange );
		m_kInventory.FlushEnduranceChange( kPacketReq.m_kItemEnduranceUpdate );
		m_kInventory.FlushDeletedItem( kPacketReq.m_kItemQuantityUpdate.m_vecDeleted );
		SendToGameDB( DBE_INSERT_PURCHASED_CASH_PACKAGE_REQ, kPacketReq );
		return;
	}
	else
	{
		kPacket.m_iOK = NetError::NET_OK;
		kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
	}

end_proc:
	// 구성상품에 대한 검사
	m_kUserCashInventory.DelPickUpPackageInfo( kPacket_.m_ulProductNo );
	m_kUserCashInventory.AddUpdatedInventorySlot( kPacketReq.m_vecUpdatedInventorySlot );

	if( !m_kUserCashInventory.IsEmptyPickUpPackageInfo() )
		return;

	m_kUserCashInventory.GetUpdatedInventorySlot( kPacket.m_vecInventorySlotInfo );
	SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_INSERT_PURCHASED_CASH_ITEM_ACK, KDBE_INSERT_ITEM_ACK )
{
    // DB에 추가된 아이템을 인벤토리에 넣고 유저에게 수정된 데이터를 넘겨준다.
    KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;
    kPacket.m_iOK = NetError::NET_OK;
    kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;

    m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kPacket.m_vecInventorySlotInfo );
    kPacket.m_vecInventorySlotInfo.insert( kPacket.m_vecInventorySlotInfo.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );

	SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_INSERT_PURCHASED_CASH_PACKAGE_ACK )
{
	// DB에 추가된 아이템을 인벤토리에 넣고 유저에게 수정된 데이터를 넘겨준다.
	KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;

	m_kInventory.InsertItem( kPacket_.m_mapItemInfo, kPacket.m_vecInventorySlotInfo );
	kPacket.m_vecInventorySlotInfo.insert( kPacket.m_vecInventorySlotInfo.begin(), kPacket_.m_vecUpdatedInventorySlot.begin(), kPacket_.m_vecUpdatedInventorySlot.end() );

	// 구성상품에 대한 검사
	m_kUserCashInventory.DelPickUpPackageInfo( kPacket_.m_ulProductNo );
	m_kUserCashInventory.AddUpdatedInventorySlot( kPacket.m_vecInventorySlotInfo );

	if( !m_kUserCashInventory.IsEmptyPickUpPackageInfo() )
		return;

	m_veciPackageBuyCheck.clear();

	m_kUserCashInventory.GetUpdatedInventorySlot( kPacket.m_vecInventorySlotInfo );
	SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_CASH_PRODUCT_INFO_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_CASH_PRODUCT_INFO_ACK kPacket;

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 상품 리스트 요청" )
			<< BUILD_LOG( GetName() );

		kPacket.m_bFinal = true;
		SendPacket( EGS_CASH_PRODUCT_INFO_ACK, kPacket );
		return;
	}
	//}}

	// 체험ID인 경우 상품 리스트를 주지 않는다.
	if( IsGuestUser() )
	{
		kPacket.m_bFinal = true;
		SendPacket( EGS_CASH_PRODUCT_INFO_ACK, kPacket );
		return;
	}

	//{{ 2010. 04. 26  최육사	PC방 전용 캐쉬템
	kPacket.m_bFinal = SiKNexonBillingTCPManager()->GetProductPage( kPacket_.m_iPage, IsPcBang(), kPacket.m_vecProductInfo );
	//}}
	SendPacket( EGS_CASH_PRODUCT_INFO_ACK, kPacket );
}


_IMPL_ON_FUNC( ENX_BT_PURCHASE_ITEM_ACK, KEGS_BUY_CASH_ITEM_ACK )
{
	//{{ 2013. 2. 18	박세훈	빙고 보상 아이템 지급 실패 시 재지급 처리
	//}}

	//{{ 2012. 06. 12	박세훈	매일매일 선물 상자
	//}}

	//{{ 2012. 09. 25	박세훈	빙고 이벤트
	// 어느 상태 막을지 아직 미정이
	kPacket_.m_bIsWishList = m_kUserWishList.GetBuyCashItemInWishList();
	SendPacket( EGS_BUY_CASH_ITEM_ACK, kPacket_ );
	//}}

	// 실시간 부활석
	if( m_kUserCashInventory.IsCashResurrectionStone() == true  &&  kPacket_.m_iOK != NetError::NET_OK )
	{
		KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, kPacketAck );
	}

	//{{ 2011. 02. 08	최육사	캐쉬템 구매 로그
	for( const KNXBTPurchaseReqInfo& kReq : kPacket_.m_vecPurchaseReqInfo )
	{
		KNXBTProductInfo kProductInfo;
		if( SiKNexonBillingTCPManager()->GetProductInfo( kReq.m_ulProductNo, kProductInfo ) == false )
		{
			START_LOG( cerr, L"존재하지 않는 상품을 구매하였습니다!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kReq.m_ulProductNo )
				<< END_LOG;
			continue;
		}

		// 캐쉬템 구매 카운트 체크
		m_kUserStatistics.IncreaseCharacterCount( KUserStatistics::CGCT_BUY_CASH_ITEM );
		m_kUserStatistics.IncreaseAccountCount( KUserStatistics::AGCT_BUY_CASH_ITEM );

		// 해당 캐릭터가 최초로 캐쉬템 구매한것이라면?
		if( m_kUserStatistics.GetCharacterCount( KUserStatistics::CGCT_BUY_CASH_ITEM ) == 1 )
		{
			//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
			WriteCharacterLogToDBWithProductNo( KELOG_USER_CHARACTER_LOG_NOT::LT_FIRST_BUY_CASH_ITEM, kReq.m_ulProductNo );
			//}}
		}

		// 캐쉬템 구매 로그 남기기
		CTime tCurTime = CTime::GetCurrentTime();
		KELOG_BUY_CASH_ITEM_LOG_NOT kPacketToLog;
		kPacketToLog.m_iUserUID			= GetUID();
		kPacketToLog.m_iServerGroupID	= KBaseServer::GetKObj()->GetServerGroupID();
		kPacketToLog.m_iUnitUID			= GetCharUID();
		kPacketToLog.m_cUnitClass		= GetUnitClass();
		kPacketToLog.m_ucLevel			= GetLevel();
		kPacketToLog.m_bIsFirstBuy		= m_kUserStatistics.IsFirstBuy();
		kPacketToLog.m_cBuyType			= KELOG_BUY_CASH_ITEM_LOG_NOT::CIBT_BUY;
		kPacketToLog.m_iProductNo		= kProductInfo.m_ulProductNo;
		kPacketToLog.m_iBuyQuantity		= kReq.m_usOrderQuantity;
		kPacketToLog.m_iTotalSalePrice	= kReq.m_usOrderQuantity * kProductInfo.m_ulSalePrice;
		kPacketToLog.m_wstrRegDate		= ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
		SendToLogDB( ELOG_BUY_CASH_ITEM_LOG_NOT, kPacketToLog );

	}
	//}}
}

IMPL_ON_FUNC( EGS_PRESENT_CASH_ITEM_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_PRESENT_CASH_ITEM_ACK );

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 상품 선물하기 요청" )
			<< BUILD_LOG( GetName() );

		KEGS_PRESENT_CASH_ITEM_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_CONTENT_00;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacketAck );
		return;
	}
	//}}


	//{{ 2012. 09. 25	박세훈	빙고 이벤트
	//}}

	//{{ 2009. 3. 24  최육사	GM제한
	if( kPacket_.m_wstrReceiverNickName == NetError::GetErrStr( NetError::STR_GM_NICKNAME_1 )  ||
		kPacket_.m_wstrReceiverNickName == NetError::GetErrStr( NetError::STR_GM_NICKNAME_2 ) )
	{
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_GM_CHAR_DENY_00;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}
	//}}

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_GUEST_USER_00;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}


	// 쿼리에 직접 들어갈 문자열이므로 injection 대비 검사를 한다.
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrReceiverNickName ) )
	{
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_ODBC_00;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}

	// 본인에게 선물하려는건지 체크하기
	if( x2::platform::IEquals(kPacket_.m_wstrReceiverNickName, GetCharName()) )
	{
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_20;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}

	// 정상적인 상품인지 검사 - 차후에 필요성 체크
	std::vector< KNXBTPurchaseReqInfo >::iterator vit;
	for( vit = kPacket_.m_vecPurchaseReqInfo.begin(); vit != kPacket_.m_vecPurchaseReqInfo.end(); vit++ )
	{
		int iItemID = SiKBillManager()->GetItemID( vit->m_ulProductNo );;
		if( iItemID <= 0 )
		{
			START_LOG( cerr, L"아이템 ID 변환 실패." )
				<< BUILD_LOG( vit->m_ulProductNo )
				<< BUILD_LOG( iItemID )
				<< END_LOG;

			KEGS_PRESENT_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_00;
			SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
			return;
		}

		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"아이템 템플릿을 얻지 못함." )
				<< BUILD_LOG( iItemID )
				<< END_LOG;

			KEGS_PRESENT_CASH_ITEM_ACK kPacket;
			kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_00;
			SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
			return;
		}

		KBillProductInfo kBillProductInfo2;
		if(SiKBillManager()->GetProductInfo((int)vit->m_ulProductNo, kBillProductInfo2))
		{
			if(kBillProductInfo2.m_bEnableGift == false)
			{
				START_LOG( cerr, L"선물할 수 없는 아이템" )
					<< BUILD_LOG( vit->m_ulProductNo )
					<< BUILD_LOG( iItemID )
					<< END_LOG;

				KEGS_PRESENT_CASH_ITEM_ACK kPacket;
				kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_11;
				SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
				return;
			}
		}
	}

	// 찜에서 캐쉬구매
	m_kUserWishList.SetBuyCashItemInWishList( kPacket_.m_bIsWishList );

	//////////////////////////////////////////////////////////////////////////
	KDBE_PRESENT_CASH_ITEM_CHECK_NICKNAME_REQ kPacketToDB;
	kPacketToDB.m_wstrReceiverNickName = kPacket_.m_wstrReceiverNickName;
	kPacketToDB.m_wstrMessage		   = kPacket_.m_wstrMessage;
	kPacketToDB.m_vecPurchaseReqInfo   = kPacket_.m_vecPurchaseReqInfo;

	if(!IsAbleToUseCashType(kPacket_.m_iUseCashType))
	{
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = NetError::ERR_GLOBAL_BILLING_01;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}

	kPacketToDB.m_iUseCashType = kPacket_.m_iUseCashType;

	SendToGameDB( DBE_PRESENT_CASH_ITEM_CHECK_NICKNAME_REQ, kPacketToDB );

}

IMPL_ON_FUNC( ELG_PRESENT_CASH_ITEM_ACK )
{
	// 상태 막을까?

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		// 비밀번호 인증실패
		KEGS_PRESENT_CASH_ITEM_ACK kPacket;
		kPacket.m_iOK = kPacket_.m_iOK;
		SendPacket( EGS_PRESENT_CASH_ITEM_ACK, kPacket );
		return;
	}

	KDBE_PRESENT_CASH_ITEM_CHECK_NICKNAME_REQ kPacketToDB;
	kPacketToDB.m_wstrReceiverNickName = kPacket_.m_wstrReceiverNickName;
	kPacketToDB.m_wstrMessage		   = kPacket_.m_wstrMessage;
	kPacketToDB.m_vecPurchaseReqInfo   = kPacket_.m_vecPurchaseReqInfo;
	SendToGameDB( DBE_PRESENT_CASH_ITEM_CHECK_NICKNAME_REQ, kPacketToDB );
}


//{{ 2009. 10. 6  최육사	자동결재
IMPL_ON_FUNC( EGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_ACK );

	KEGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_ACK kPacket;
	KNXBTProductInfo kProductInfo;
	CTime tCurrent;

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 자동결제 상품 체크 요청" )
			<< BUILD_LOG( GetName() );

		kPacket.m_iOK = NetError::ERR_CONTENT_00;
		goto end_proc;
	}
	//}}

	// 체험ID 기능 제한
	if( IsGuestUser() )
	{
		kPacket.m_iOK = NetError::ERR_GUEST_USER_00;
		goto end_proc;
	}

	// 존재하는 상품인지 검사
	if( !SiKNexonBillingTCPManager()->GetProductInfo( kPacket_.m_ulProductNo, kProductInfo ) )
	{
		START_LOG( cerr, L"프로덕트 정보 추출 실패." )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_BUY_CASH_ITEM_00;
		goto end_proc;
	}

	// 클라이언트에서 웹페이지를 통해 구매가능하도록 정보를 담자!
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_uiUserSN = m_kNexonAccountInfo.m_uiNexonSN;
	kPacket.m_byDomainType = static_cast<BYTE>( SiKNexonBillingTCPManager()->GetBillingServerNo() ); // 빌링서버군
	kPacket.m_ulProductNo = kPacket_.m_ulProductNo;
	kPacket.m_ulSalePrice = kProductInfo.m_ulSalePrice;
	kPacket.m_usOrderQuantity = kPacket_.m_usOrderQuantity;

	tCurrent = CTime::GetCurrentTime();
	kPacket.m_wstrTransactionID = ( CStringW )tCurrent.Format( _T( "%Y%m%d%H%M%S" ) );
	wchar_t wszNumber[32];
	_i64tow_s( KBaseServer::GetKObj()->GetUID(), wszNumber, 32, 10 );
	kPacket.m_wstrTransactionID += L"_";
	kPacket.m_wstrTransactionID += wszNumber;
	_i64tow_s( GetUID(), wszNumber, 32, 10 );
	kPacket.m_wstrTransactionID += L"_";
	kPacket.m_wstrTransactionID += wszNumber;
	_i64tow_s( SiKNexonBillingTCPManager()->GetNextPurchaseNo(), wszNumber, 32, 10 );
	kPacket.m_wstrTransactionID += L"_";
	kPacket.m_wstrTransactionID += wszNumber;

	START_LOG( clog, L"자동결제 상품 구매 정보 얻기" )
		<< BUILD_LOG( GetCharName() )
		<< BUILD_LOG( kPacket.m_byDomainType )
		<< BUILD_LOG( kPacket.m_ulProductNo )
		<< BUILD_LOG( kPacket.m_wstrTransactionID );

end_proc:
	SendPacket( EGS_BUY_CASH_ITEM_AUTOMATIC_PAYMENT_ACK, kPacket );
}

IMPL_ON_FUNC( ENX_BT_CHECK_AUTO_PAYMENT_USER_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	// 결과에 따라서 부활석 충전 처리 : result값이 1이면 자동결제 유저임!
	if( kPacket_.m_ulResult == 1 )
	{
		// 자동결제 부활석 갯수가 0개일때만 1개 채워줌.
		if( m_iNumAutoPaymentResStone == 0 )
		{
			START_LOG( clog, L"자동결제 부활석을 리필합니다!" )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				<< BUILD_LOG( m_iNumAutoPaymentResStone );

			m_iNumAutoPaymentResStone += 1; // 상품 종류에 따라서 처리

			// 캐릭터 정보를 업데이트 해주자! GUnit 테이블의 lastdate가 갱신되도록
			WriteUnitInfoToDB( false );

			//{{ 2012. 09. 22	박세훈	빙고 이벤트
			//}}

			KEGS_AUTO_PAYMENT_RESURRECTION_STONE_NOT kPacketNot;
			kPacketNot.m_iNumAutoPaymentResStone = m_iNumAutoPaymentResStone;
			SendPacket( EGS_AUTO_PAYMENT_RESURRECTION_STONE_NOT, kPacketNot );
		}
	}
	else
	{
		START_LOG( clog, L"자동결제 유저가 아닌가보군!" )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( GetCharName() )
			<< BUILD_LOG( kPacket_.m_ulResult );
	}
}
//}}

_IMPL_ON_FUNC( ENX_BT_PURCHASE_GIFT_ACK, KEGS_PRESENT_CASH_ITEM_ACK )
{
}

IMPL_ON_FUNC( EGS_CHECK_PRESENT_CASH_INVENTORY_NOT )
{
	// 상태 막을까..?

	KENX_BT_NISMS_INVENTORY_READ_REQ kPacketReq;
	kPacketReq.m_ulPacketNo	  	   = SiKNexonBillingTCPManager()->GetNextPacketNo();
	kPacketReq.m_bytePacketType	   = KNexonBillingTCPPacket::NISMS_INVENTORY_READ;
	kPacketReq.m_ulOrderNo		   = kPacket_.m_ulOrderNo;
	kPacketReq.m_ulProductNo	   = kPacket_.m_ulProductNo;

	KEventPtr spEvent( new KEvent );
	UidType anTrace[2] = { GetUID(), -1 };
	spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_NISMS_INVENTORY_READ_REQ, kPacketReq );
	SiKNexonBillingTCPManager()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ELG_PRESENT_MESSAGE_TO_RECEIVER_NOT, KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT )
{
	// 상태 막을까?

	SendPacket( EGS_PRESENT_MESSAGE_TO_RECEIVER_NOT, kPacket_ );
}
//}}

IMPL_ON_FUNC( DBE_INSERT_CASH_SKILL_POINT_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );


	KEGS_UPDATE_CASH_SKILL_POINT_NOT kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iUnitUID = GetCharUID();
	kPacket.m_iCSPoint = kPacket_.m_iCSPoint;
	kPacket.m_wstrCSPointEndDate = kPacket_.m_wstrEndDate;
	kPacket.m_bUpdateEndDateOnly = kPacket_.m_bUpdateEndDateOnly;
	//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
	kPacket.m_iSkillPointItemID = kPacket_.m_iSkillPointItemID;
	//}}

	KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacketToClient;
	KELOG_INSERT_CASH_SKILL_POINT_NOT kPacketToLogDB;

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"캐시 스킬 포인트 구매 DB 업데이트 실패!!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_iCSPoint )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_SKILL_23;
		goto end_proc;
	}


	if( false == kPacket_.m_bUpdateEndDateOnly )
	{
		// kimhc // 김현철 // 그노시스 축복 사용 시, 캐시 스킬 포인트는
		// 모든 페이지에서 증가 되어야 한다
		m_kSkillTree.SetCSPointEveryPage( kPacket_.m_iCSPoint );

		m_kSkillTree.SetMaxCSPoint( kPacket_.m_iCSPoint );
	}
	m_kSkillTree.SetCSPointEndDate( kPacket_.m_wstrEndDate );

	switch( kPacket_.m_iSkillPointItemID )
	{
		//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
		case CXSLItem::EI_SKILL_POINT_30_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
		case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
		case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
		//}}
		//{{ 2011. 11. 30	최육사	패키지 상품 추가
		case CXSLItem::CI_SKILL_POINT_30_USE_INVEN:
		case CXSLItem::CI_SKILL_POINT_60_USE_INVEN:
		case CXSLItem::CI_SKILL_POINT_60_USE_INVEN_ARA:
		//}}
		//{{ 2013. 03. 27	 그노시스의 축복 5point 7일 추가 - 김민성
		case CXSLItem::EI_SKILL_POINT_30_DAY_7_USE_INVEN:
		//}
		break;

	default:
		{
			// 아이템을 가져왔다는 패킷을 클라이언트로 보낸다
			kPacketToClient.m_iOK = NetError::NET_OK;
			kPacketToClient.m_iNumResurrectionStone = m_iNumResurrectionStone;
			SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK, kPacketToClient );

			// logdb로 통계 보내기
			kPacketToLogDB.m_iUnitUID		= GetCharUID();
			kPacketToLogDB.m_iUnitLevel		= GetLevel();
			kPacketToLogDB.m_iMaxCSPoint	= kPacket_.m_iCSPoint;
			kPacketToLogDB.m_iPeriod		= kPacket_.m_iPeriod;
			kPacketToLogDB.m_bUpdateEndDateOnly = kPacket_.m_bUpdateEndDateOnly;
			SendToLogDB( ELOG_INSERT_CASH_SKILL_POINT_NOT, kPacketToLogDB );
		}
		break;
	}

end_proc:
	SendPacket( EGS_UPDATE_CASH_SKILL_POINT_NOT, kPacket );
}

IMPL_ON_FUNC( DBE_EXPIRE_CASH_SKILL_POINT_ACK )
{
	VERIFY_FORBIDDEN_STATE( ( 3, KGSFSM::S_UID_UPDATED, KGSFSM::S_LOGINED, KGSFSM::S_SERVER_SELECT ) );


	// fix!! DB작업에 실패한 경우에 어떻게 예외 처리할지에 대해서 좀 더 고민해봐야함
	if( kPacket_.m_iOK != NetError::NET_OK )
	{


			int iSumSPoint = 0;
			for ( UINT i = 0; i < kPacket_.m_vecRetrievedSPoint.size(); i++ )
				iSumSPoint += kPacket_.m_vecRetrievedSPoint[i];

			START_LOG( cerr, L"cash skill expire 실패, 있을수 없는 에러!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( iSumSPoint )
				<< BUILD_LOG( kPacket_.m_iOK )
				<< END_LOG;
	}

	m_kSkillTree.AddSPointEveryPage( kPacket_.m_vecRetrievedSPoint );

	m_kSkillTree.ExpireCashSkillPoint();

	//{{ 2010. 07. 04  최육사	스킬 포인트 오류 확인
	if( CheckSkillPointError() == false )
	{
		if( GetAuthLevel() < SEnum::UAL_GM  )
		{
			START_LOG( cerr, L"캐시 스킬 포인트 기간 만료 결과 처리시점에 스킬 포인트 값이 비정상입니다." )
				<< BUILD_LOG( GetName() )
				<< BUILD_LOG( GetCharName() )
				//<< BUILD_LOG( kPacket_.m_iRetrievedSPoint )
				<< END_LOG;
		}
	}
	//}}

	KEGS_EXPIRE_CASH_SKILL_POINT_NOT kPacket;
	kPacket.m_iUnitUID = GetCharUID();

	kPacket.m_vecRetrievedSPoint = kPacket_.m_vecRetrievedSPoint;


	// kimhc // 김현철 // 2013-11-21 // 모든 스킬 페이지 별로 배운 스킬 정보 얻어오도록!
	m_kSkillTree.GetHaveSkillList( kPacket.m_vecMapHaveSKill );



	SendPacket( EGS_EXPIRE_CASH_SKILL_POINT_NOT, kPacket );


	// 방안일 경우 다른유저들에게도 전송 ( 장착스킬 정보 변경)
	if( kPacket_.m_iOK == NetError::NET_OK && GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
			break;

		default:
			return;
		}

		// 서버에 유저 정보 업데이트
		KRoomUserInfo kInfo;
		GetRoomUserInfo( kInfo );
		SendToCnRoom( ERM_SET_UNIT_INFO_NOT, kInfo );
	}

}

IMPL_ON_FUNC( DBE_GET_WISH_LIST_ACK )
{
	// 상태는..?

	// 장바구니 초기화
	m_kUserWishList.InitWishList( kPacket_.m_mapWishList );
}

IMPL_ON_FUNC_NOPARAM( EGS_GET_WISH_LIST_REQ )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_GET_WISH_LIST_ACK kPacket;
	m_kUserWishList.GetWishList( kPacket.m_mapWishList );
	SendPacket( EGS_GET_WISH_LIST_ACK, kPacket );
}

IMPL_ON_FUNC( EGS_MODIFY_WISH_LIST_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_MODIFY_WISH_LIST_ACK );

	KEGS_MODIFY_WISH_LIST_ACK kPacket;
	kPacket.m_iOK				= NetError::NET_OK;
	kPacket.m_vecWishListItemID = kPacket_.m_vecWishListItemID;
	kPacket.m_bIsDelete			= kPacket_.m_bIsDelete;

	if( !kPacket_.m_bIsDelete )
	{
		for( const int iWishItemID : kPacket_.m_vecWishListItemID )
		{
			// 장바구니 추가
			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iWishItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"존재하지 않는 아이템을 장바구니에 등록하려 했음." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( iWishItemID )
					<< END_LOG;

				kPacket.m_iOK = NetError::ERR_WISH_LIST_00;
				goto end_proc;
			}

			//{{ 캐시샵 찜하기 문제 임시 처리
#ifdef DELETE_EXCEPTION_WISHLIST_CHANGEJOBITEM
			if( CXSLItem::IsBankExpandItem( pItemTemplet->m_ItemID )
#else //DELETE_EXCEPTION_WISHLIST_CHANGEJOBITEM

			if( CXSLItem::IsChangeJobItem( pItemTemplet->m_ItemID )
				||  CXSLItem::IsBankExpandItem( pItemTemplet->m_ItemID )
				|| CXSLItem::IsItemToMakeSkillPageExpanded( pItemTemplet->m_ItemID )
#endif //DELETE_EXCEPTION_WISHLIST_CHANGEJOBITEM
				|| CXSLItem::IsPossibleCart( pItemTemplet->m_ItemID) == false
				)
			{
				kPacket.m_iOK = NetError::ERR_WISH_LIST_00;
				goto end_proc;
			}
			//}}

			if( !m_kUserWishList.AddWish( iWishItemID ) )
			{
				START_LOG( cerr, L"장바구니에 상품 등록 실패." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( iWishItemID )
					<< BUILD_LOG( NetError::GetLastErrMsg() )
					<< END_LOG;

				kPacket.m_iOK = NetError::GetLastError();
				goto end_proc;
			}
		}
	}
	else
	{
		std::vector< int >::const_iterator vit;
		for( vit = kPacket_.m_vecWishListItemID.begin(); vit != kPacket_.m_vecWishListItemID.end(); ++vit )
		{
			// 장바구니 삭제
			if( !m_kUserWishList.DelWish( *vit ) )
			{
				START_LOG( cerr, L"장바구니에 상품 삭제중 이상발생." )
					<< BUILD_LOG( GetCharUID() )
					<< BUILD_LOG( *vit )
					<< BUILD_LOG( NetError::GetLastErrMsg() )
					<< END_LOG;

				kPacket.m_iOK = NetError::GetLastError();
				goto end_proc;
			}
		}
	}

end_proc:
	SendPacket( EGS_MODIFY_WISH_LIST_ACK, kPacket );
}

//{{ 2010. 11. 17	최육사	넥슨 빌링 청약 철회
IMPL_ON_FUNC( EGS_CASH_ITEM_REFUND_REQ )
{
	VERIFY_STATE_ACK( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ), EGS_CASH_ITEM_REFUND_ACK );

	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	if( SiKGSContentManager()->IsEnableCashShop() == false )
	{
		START_LOG( clog, L"캐쉬샵 점검중 청약 철회 요청" )
			<< BUILD_LOG( GetName() );

		KEGS_CASH_ITEM_REFUND_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_CONTENT_00;
		SendPacket( EGS_CASH_ITEM_REFUND_ACK, kPacketAck );
		return;
	}
	//}}

	//////////////////////////////////////////////////////////////////////////
	std::wstring wstrRequestID;
	if( SiKNexonBillingTCPManager()->GetCashItemRefundRequestID( wstrRequestID ) == false )
	{
		START_LOG( cerr, L"청약 철회 기능 동작중에 RequestID발급이 실패하였습니다!" )
			<< BUILD_LOG( GetCharName() )
			<< END_LOG;

		KEGS_CASH_ITEM_REFUND_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_BUY_CASH_ITEM_45;
		SendPacket( EGS_CASH_ITEM_REFUND_ACK, kPacketAck );
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	KENX_BT_REFUND_REQ kPacketReq;
	kPacketReq.m_ulPacketNo		= SiKNexonBillingTCPManager()->GetNextPacketNo();
	kPacketReq.m_bytePacketType = KNexonBillingTCPPacket::REFUND;
	kPacketReq.m_ulRemoteIP		= GetIP();
	kPacketReq.m_wstrRequestID	= wstrRequestID;
	//{{ 2011. 08. 10    김민성    채널링 SOAP
	kPacketReq.m_wstrGameID		= GetName();
	//}}

	// 갯수나 번호에 대한 검사 (unsined 에서 sined 값으로 변경)
	int iOrderNo = static_cast<int>( kPacket_.m_ulOrderNo );
	int iProductNo = static_cast<int>( kPacket_.m_ulProductNo );

	if( iOrderNo > 0 && iProductNo > 0 && kPacket_.m_iQuantity > 0 )
	{
		kPacketReq.m_ulOrderNo		= kPacket_.m_ulOrderNo;
		kPacketReq.m_ulProductNo	= kPacket_.m_ulProductNo;
		kPacketReq.m_usQuantity		= static_cast<u_short>(kPacket_.m_iQuantity);
	}
	else
	{
		START_LOG( cerr, L"청약 철회 요청 시 클라가 주는 값이 이상하다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( GetName() )
			<< BUILD_LOG( kPacket_.m_ulOrderNo )
			<< BUILD_LOG( kPacket_.m_ulProductNo )
			<< BUILD_LOG( kPacket_.m_iQuantity )
			<< END_LOG;

		KEGS_CASH_ITEM_REFUND_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_BUY_CASH_ITEM_53;
		SendPacket( EGS_CASH_ITEM_REFUND_ACK, kPacketAck );
		return;
	}

	//{{ 2012. 06. 12	박세훈	매일매일 선물 상자
	//}}

	KEventPtr spEvent( new KEvent );
	UidType anTrace[2] = { GetUID(), -1 };
	spEvent->SetData( PI_GS_NX_BILLING_TCP, anTrace, ENX_BT_REFUND_REQ, kPacketReq );
	SiKNexonBillingTCPManager()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ENX_BT_REFUND_ACK, KEGS_CASH_ITEM_REFUND_ACK )
{
	//{{ 2012. 06. 12	박세훈	매일매일 선물 상자
	//}}

	// ACK는 상태를 막지 말자.

	SendPacket( EGS_CASH_ITEM_REFUND_ACK, kPacket_ );
}
//}}

//{{ 2011. 11. 21  김민성	전직 변경 아이템
_IMPL_ON_FUNC( DBE_BUY_UNIT_CLASS_CHANGE_ACK, KDBE_BUY_UNIT_CLASS_CHANGE_REQ )
{
// 	if( kPacket_.m_iOK != NetError::NET_OK )
// 	{
// 		START_LOG( cerr, L"전직 초기화 구입 사용 실패!" )
// 			<< BUILD_LOG( kPacket_.m_iOK )
// 			<< BUILD_LOG( GetCharName() )
// 			<< END_LOG;
//
 		KEGS_GET_PURCHASED_CASH_ITEM_ACK kPacket;				// 근데 이거 해외팀 패킷에 맞는 걸로 바꿔야하는거 아닌가?
 		kPacket.m_iOK = NetError::NET_OK;
        kPacket.m_iNumResurrectionStone = m_iNumResurrectionStone;
 		SendPacket( EGS_GET_PURCHASED_CASH_ITEM_ACK, kPacket );
// 		return;
// 	}

	KEGS_BUY_UNIT_CLASS_CHANGE_NOT kNot;

	// 입력 받은 데이터 설정 하자
	SetUnitClass( static_cast<char>(kPacket_.m_iNewUnitClass) );
	ResetStat();
	m_kSkillTree.SetUnitClass( (int) GetUnitClass() );

	// 배운스킬 삭제.. 장착스킬 제거
	m_kSkillTree.ResetEveryPage();

	//스킬포인트 새로 셋팅.
	m_kSkillTree.SetSPointEveryPage( kPacket_.m_iSPoint );
	m_kSkillTree.SetCSPointEveryPage( kPacket_.m_iCSPoint );

	// 기본 스킬 넣기
	// kimhc // 김현철 // 2013-11-17
	// 전직 변경 시에는 모든 스킬 페이지에 초기화 적용 되도록 함

	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID1, 1, 0 );
	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID2, 1, 0 );
	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID3, 1, 0 );
	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID4, 1, 0 );
	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID5, 1, 0 );
	m_kSkillTree.CheckAndUpdateSkillLevelAndCSPOnEveryPage( kPacket_.m_iDefaultSkillID6, 1, 0 );



	// skill 설정
	m_kSkillTree.SetClassChangeSkill( kPacket_.m_mapChangeSkill );

	// memo 설정
	m_kSkillTree.SetClassChangeMemo( kPacket_.m_mapChangeMemo );

	// item 설정
	m_kInventory.SetClassChangeItem( kPacket_.m_mapChangeItem, kNot.m_vecChangeItem );

	// Quest 설정
	m_kUserQuestManager.SetClassChangeQuest( GetCharUID(), kPacket_.m_mapChangeCompleteQuest, kPacket_.m_mapChangeInProgressQuest );
	// 삭제된 Quest 설정
	m_kUserQuestManager.SetClassChangeDeleteQuest( GetCharUID(), kPacket_.m_vecDeleteCompleteQuest, kPacket_.m_vecDeleteInProgressQuest );

	// 클라이언트에게 알려주자

	kNot.m_iUnitUID						= kPacket_.m_iUnitUID;
	kNot.m_iNewUnitClass				= kPacket_.m_iNewUnitClass;

	m_kSkillTree.GetMemoList( kNot.m_mapChangeMemo );

	GetUnitInfo( kNot.m_kUnitInfo );
	m_kUserQuestManager.GetCompleteChangeClassQuest( kNot.m_vecChangeCompleteQuest );
	m_kUserQuestManager.GetInProgressClassChangeQuest( kNot.m_vecChangeInProgressQuest );
	m_kSkillTree.GetUnSealedSkillList( kNot.m_vecSkillUnsealed );

	SendPacket( EGS_BUY_UNIT_CLASS_CHANGE_NOT, kNot );

	KDBE_UPDATE_UNIT_CLASS_NOT kPacketNot;
	kPacketNot.m_iUnitUID = GetCharUID();
	kPacketNot.m_cUnitClass = GetUnitClass();

	SendToLogDB( DBE_UPDATE_UNIT_CLASS_NOT, kPacketNot );

	if( GetStateID() == KGSFSM::S_FIELD_MAP )
	{
		// 필드에 있을경우 유저 정보 갱신하기
		if( GetFieldUID() > 0 )
		{
			KERM_CHANGE_FIELD_USER_INFO_NOT kNot;
			GetFieldUserInfo( kNot.m_kInfo );
			SendToGSField( ERM_CHANGE_FIELD_USER_INFO_NOT, kNot );
		}
	}
	else if( GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
		case CXSLRoom::RT_PVP:
		case CXSLRoom::RT_DUNGEON:
		case CXSLRoom::RT_TRAININGCENTER:
			{
				KRoomUserInfo kRoomUserInfo;
				GetRoomUserInfo( kRoomUserInfo );
				SendToCnRoom( ERM_CHANGE_ROOM_USER_INFO_NOT, kRoomUserInfo );
			}
			break;

		case CXSLRoom::RT_SQUARE:
			{
				KSquareUserInfo kSquareUserInfo;
				GetSquareUserInfo( kSquareUserInfo );
				SendToCnRoom( ERM_CHANGE_SQUARE_USER_INFO_NOT, kSquareUserInfo );
			}
			break;

		default:
			{
				START_LOG( cerr, L"이상한 방 타입." )
					<< BUILD_LOG( CXSLRoom::GetRoomType( GetRoomUID() ) )
					<< BUILD_LOG( GetCharUID() )
					<< END_LOG;
			}
			break;
		}
	}

	//{{ 2009. 10. 8  최육사	길드
	SendUpdateGuildMemberInfo();
	//}}

	//{{ 2013. 09. 24	최육사	일본 이벤트 중계DB작업
	//}}
	//}}

	IF_EVENT_ENABLED( CEI_ARA_NEW_FIRST_CLASS_EVENT )
	{
		if( GetUnitClass() == CXSLUnit::UC_ARA_LITTLE_DEVIL )
		{
			// 이벤트 보상을 주자!
			KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
			kPacketToDB.m_iFromUnitUID = GetCharUID();
			kPacketToDB.m_iToUnitUID   = GetCharUID();
			kPacketToDB.m_iRewardType  = KPostItemInfo::LT_EVENT;
			kPacketToDB.m_iRewardID	   = _CONST_ARA_NEW_FIRST_CLASS_EVENT_REWARD_ID::iClassChangeReward;	// 여기 전직 변경권 구매했을 때임. 헷갈리지 말 것
			SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// 프로모션 아바타 큐브 지급
	CStringW cwstrItemID;

	// 전직 변경권을 구매 했는데, 전직 아이템 이름을 가져오고 있다. 고칩시다.
	cwstrItemID.Format( L"%d", CXSLItem::GetClassChangeCashItemIDByUnitClass( GetUnitClass() ) );

	if( CXSLUnit::IsSecondChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true
		&& kPacket_.m_bUnlimitedSecondJobItem == false //무제한 전직 아이템으로 전직 시에는 보상 주지 않는다.
		)
	{
		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		kPacketToDB.m_iFromUnitUID = GetCharUID();
		kPacketToDB.m_iToUnitUID   = GetCharUID();
		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_MESSAGE;
		kPacketToDB.m_iRewardID	   = CXSLItem::SI_SECOND_CHANGE_JOB_CUBE;
		kPacketToDB.m_sQuantity	   = 1;
		kPacketToDB.m_wstrMessage  = cwstrItemID.GetBuffer();
		kPacketToDB.m_bGameServerEvent = false;
		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
	}
	else if( CXSLUnit::IsFirstChangeJob( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) == true
			&& kPacket_.m_bUnlimitedSecondJobItem == false //무제한 전직 아이템으로 전직 시에는 보상 주지 않는다.
			)
	{
		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		kPacketToDB.m_iFromUnitUID = GetCharUID();
		kPacketToDB.m_iToUnitUID   = GetCharUID();
		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_MESSAGE;
		kPacketToDB.m_iRewardID	   = CXSLItem::SI_FIRST_CHANGEJOB_CUBE;
		kPacketToDB.m_sQuantity	   = 1;
		kPacketToDB.m_wstrMessage  = cwstrItemID.GetBuffer();
		kPacketToDB.m_bGameServerEvent = false;
		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
	}
}
//}}

//{{ 2012. 04. 02	김민성		넥슨 auth soap
//}

//{{ 2012. 09. 19   김민성   캐시샵 방문 상태
IMPL_ON_FUNC( EGS_VISIT_CASH_SHOP_NOT )
{
	if( m_kUserUnitManager.GetEnterCashShop() != kPacket_.m_bEnterCashShop )
	{
		m_kUserUnitManager.SetEnterCashShop( kPacket_.m_bEnterCashShop );
	}
	else
	{
		m_kUserUnitManager.SetEnterCashShop( kPacket_.m_bEnterCashShop );
	}

	// 현재 룸 상태라면
	if( GetStateID() == KGSFSM::S_ROOM )
	{
		switch( CXSLRoom::GetRoomType( GetRoomUID() ) )
		{
			// 배틀 필드 일때만 알려주자
		case CXSLRoom::RT_BATTLE_FIELD:
			{
				kPacket_.m_iUnitUID = GetCharUID();
				SendToCnRoom( ERM_VISIT_CASH_SHOP_NOT, kPacket_ );
			}
			break;

		default:
			return;
		}
	}
}
//}}

//{{ 2012. 09. 22	박세훈	빙고 이벤트
//}}

IMPL_ON_FUNC( DBE_INSERT_WARP_VIP_ACK )
{
	VERIFY_STATE( ( 2, KGSFSM::S_FIELD_MAP, KGSFSM::S_ROOM ) );

	KEGS_UPDATE_WARP_VIP_NOT kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_trWarpVipEndDate = 0LL;

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KUnitInfo kUnitInfo;
		GetUnitInfo(kUnitInfo);

		CTime ctVipEndDate;
		if( KncUtil::ConvertStringToCTime(  kPacket_.m_wstrEndDate, ctVipEndDate ) == true )
		{
			SetWarpVipEndDate( ctVipEndDate.GetTime() );
			kPacket.m_trWarpVipEndDate = ctVipEndDate.GetTime();

			START_LOG( clog, L"[TEST] 워프 VIP 구매 종료 시간 수정 적용" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_wstrEndDate.c_str() )
				<< BUILD_LOG( ctVipEndDate.GetTime() )
				<< END_LOG;
		}
		else
		{
			START_LOG( cerr, L"워프 VIP 구매 종료 시간 이상!" )
				<< BUILD_LOG( GetCharUID() )
				<< BUILD_LOG( kPacket_.m_wstrEndDate.c_str() )
				<< END_LOG;

			// 기간 갱신 중 문제가 발생했습니다
			kPacket.m_iOK = NetError::ERR_UNKNOWN;
		}
	}
	else
	{
		START_LOG( cerr, L"워프 VIP 구매 DB 업데이트 실패!!" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( kPacket_.m_wstrEndDate.c_str() )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;

		// 기간 갱신 중 문제가 발생했습니다
		kPacket.m_iOK = NetError::ERR_SKILL_23;
	}

	SendPacket( EGS_UPDATE_WARP_VIP_NOT, kPacket );
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
