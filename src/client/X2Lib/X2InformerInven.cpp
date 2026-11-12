#include "stdafx.h"
#include "./X2InformerInven.h"

CX2InformerInven::CX2InformerInven(void)
{
	m_InformerType				= CX2Informer::XIT_INVEN;

	m_vTalkBoxPos				= D3DXVECTOR2( 785, 702 );

	m_bEnoughInven				= false;
	m_bNewItem					= false;

	m_bNewQuickSlotItem			= false;
	m_uiSlotID					= 0;
	m_fEnoughAllInvenSlotCheckTime = 0.0f;
	m_EnoughRemainSlotNum = 7;
}

CX2InformerInven::~CX2InformerInven(void)
{

}

HRESULT CX2InformerInven::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( g_pMain->GetNowStateID() != CX2Main::XS_DUNGEON_GAME )
	{
		CX2Informer::OnFrameMove( fTime, fElapsedTime );

		m_fEnoughAllInvenSlotCheckTime += fElapsedTime;
		if ( m_fEnoughAllInvenSlotCheckTime >= 1.0f )
		{
			m_fEnoughAllInvenSlotCheckTime = 0.0f;

			if ( IsEnoughAllInvenSlot() == true )
			{
				m_pTalkBoxManager->Delete(1);
			}
		}
	}

	return S_OK;
}

void CX2InformerInven::Reset()
{
	CX2Informer::Reset();
	m_mapNowInvenSizeForEnough.clear();
	m_setNowInvenForNewItem.clear();

	m_bEnoughInven	= false;
	m_bNewItem		= false;

	m_bNewQuickSlotItem = false;
	m_uiSlotID			= 0;
}


void CX2InformerInven::SetNowInfo()
{

	m_mapNowInvenSizeForEnough.clear();

	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		{
			for ( int i = (int)(CX2Inventory::ST_EQUIP); i < (int)( CX2Inventory::ST_AVARTA ); i++ )
			{
				CX2Inventory::SORT_TYPE sortType = (CX2Inventory::SORT_TYPE)i;
				int usedSlotNum = kInventory.GetUsedSlotNum( sortType );
				m_mapNowInvenSizeForEnough.insert( std::make_pair( sortType, usedSlotNum ) );
			}
		}
	}

	if ( m_bNewItem == true )
	{
		m_setNowInvenForNewItem.clear();

		if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
		{
			const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
			{
				kInventory.GetItemIDAndNum( m_setNowInvenForNewItem );
			}
		}
	}
}

bool CX2InformerInven::IsNowInfoChange()
{
	m_bEnoughInven = false;
	m_bNewItem = false;
	if ( CheckEnoughInven() == true )
	{
		m_bEnoughInven = true;
		return true;
	}
	else if ( CheckNewItem() == true )
	{
		// 새로운 아이템이 존재한다면 인벤토리쪽에 새로운 아이템이 있다고 체크한다.
		if( g_pData != NULL && g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetUIInventory() != NULL &&
			g_pData->GetUIManager()->GetUIInventory()->GetShow() == false )
			g_pData->GetUIManager()->GetUIInventory()->SetNewItem(true);

		m_bNewItem = true;
		return true;
	}

	return false;
}


bool CX2InformerInven::CheckEnoughInven()
{
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		if ( m_mapNowInvenSizeForEnough.empty() == true )
		{
			for ( int i = (int)(CX2Inventory::ST_EQUIP); i < (int)( CX2Inventory::ST_AVARTA ); i++ )
			{
				CX2Inventory::SORT_TYPE sortType = (CX2Inventory::SORT_TYPE)i;
				m_mapNowInvenSizeForEnough.insert( std::make_pair( sortType, 0 ) );
			}
		}

		map < CX2Inventory::SORT_TYPE, int >::iterator mit;
		for ( mit = m_mapNowInvenSizeForEnough.begin(); mit != m_mapNowInvenSizeForEnough.end(); mit++ )
		{
			CX2Inventory::SORT_TYPE sortType = mit->first;
			int PrevUsedSlotNum = mit->second;
			int nowUsedSlotNum = kInventory.GetUsedSlotNum( sortType );

			if ( nowUsedSlotNum > PrevUsedSlotNum )
			{
				m_mapNowInvenSizeForEnough[sortType] = nowUsedSlotNum;

				if ( kInventory.GetItemMaxNum( sortType ) - nowUsedSlotNum < m_EnoughRemainSlotNum )
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CX2InformerInven::CheckNewItem()
{
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		if ( m_setNowInvenForNewItem.empty() == true )
		{
			kInventory.GetItemIDAndNum( m_setNowInvenForNewItem );
			return false;
		}

		bool bCheck = false;

		//현재 가지고 있는 아이템의 ID  파악.
		set<int> tempSetNowInvenForNewItem;
		kInventory.GetItemIDAndNum( tempSetNowInvenForNewItem );

		set<int>::iterator sit;
		for ( sit = tempSetNowInvenForNewItem.begin(); sit != tempSetNowInvenForNewItem.end(); ++sit )
		{
			//현재 가지고 있는 아이템 중, 이전에 없던 아이템인지 확인
			int itemID = *sit;
			set<int>::iterator prevSit = m_setNowInvenForNewItem.find( itemID );
			if ( prevSit == m_setNowInvenForNewItem.end() )
			{
				//퀘스트 아이템 예외처리.
				if( CX2Inventory::ST_QUEST != kInventory.GetSortTypeByID(itemID) )
				{
					bCheck = true;
				}
			}
		}

		vector< int > tempVecItemToErase;
		for ( sit = m_setNowInvenForNewItem.begin(); sit != m_setNowInvenForNewItem.end(); ++sit )
		{
			int itemID = *sit;
			if ( tempSetNowInvenForNewItem.find( itemID ) == tempSetNowInvenForNewItem.end() )
			{
				tempVecItemToErase.push_back( itemID );
			}
		}

		for ( int i = 0; i < (int)tempVecItemToErase.size(); i++ )
		{
			int itemID = tempVecItemToErase[i];

			sit = m_setNowInvenForNewItem.find( itemID );
			if ( sit != m_setNowInvenForNewItem.end() )
			{
				//퀘스트 아이템 예외처리.
				if( CX2Inventory::ST_QUEST == kInventory.GetSortTypeByID(itemID) )
				{
					if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuestNew() )
					{
						g_pData->GetUIManager()->GetUIQuestNew()->ResetQuestUI();
					}
				}
				m_setNowInvenForNewItem.erase( sit );
			}
		}
		tempVecItemToErase.clear();
		return bCheck;
	}

	return false;
}


void CX2InformerInven::SetShow( bool bShow )
{
	CX2Informer::SetShow( bShow );
	if ( bShow == true )
	{
		CX2TalkBoxManagerImp::TalkBox talkBox;

		//캐릭터 상태에 따라 메뉴 버튼의 위치가 달라 지기 때문에 위치 수정
		talkBox.m_vPos				= m_vTalkBoxPos+m_vTalkBoxOffsetPos;
		talkBox.m_GameUnitType		= CX2GameUnit::GUT_USER;
		talkBox.m_bTraceUnit		= false;
		talkBox.m_TalkBoxType		= CX2TalkBoxManagerImp::TBT_FROM_DOWN_RIGHT;
		talkBox.m_coTextColor		= D3DXCOLOR(0,0,0,1);

		if ( m_bEnoughInven == true )
		{

			talkBox.m_OwnerUnitUID		= XUUII_ENOUGH_INVEN;
			talkBox.m_wstrTalkContent	= GET_STRING( STR_ID_229 );
			talkBox.m_fRemainTime		= 4.0f;
		}
		else if ( true == m_bNewQuickSlotItem )
		{
			talkBox.m_vPos.x			= 43.f + ( m_uiSlotID * 43.f );
			if( m_uiSlotID <= 3 )
			{
				talkBox.m_TalkBoxType		= CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT;
			}
			m_bNewQuickSlotItem			= false;

			talkBox.m_OwnerUnitUID		= XUUII_NEW_QUICK_SLOT_ITEM;
			talkBox.m_wstrTalkContent	= GET_STRING( STR_ID_230 );
			talkBox.m_fRemainTime		= 4.0f;
		}
		else if ( m_bNewItem == true )
		{
			talkBox.m_OwnerUnitUID		= XUUII_NEW_ITEM;
			talkBox.m_wstrTalkContent	= GET_STRING( STR_ID_230 );
			talkBox.m_fRemainTime		= 4.0f;
		}

		g_pKTDXApp->GetDeviceManager()->PlaySound( L"MessageAlarm.ogg", false, false );

		m_pTalkBoxManager->Push( talkBox );
	}
	else
	{
		m_pTalkBoxManager->DeleteByIndex( m_pTalkBoxManager->GetTalkBoxNum() - 1 );
	}
}

bool CX2InformerInven::IsEnoughAllInvenSlot()
{
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		for ( int i = (int)(CX2Inventory::ST_EQUIP); i < (int)( CX2Inventory::ST_AVARTA ); i++ )
		{
			CX2Inventory::SORT_TYPE sortType = (CX2Inventory::SORT_TYPE)i;
			int usedSlotNum = kInventory.GetUsedSlotNum( sortType );
			if ( kInventory.GetItemMaxNum( sortType ) - usedSlotNum < m_EnoughRemainSlotNum )
				return false;
		}
	}

	return true;
}
