#include "stdafx.h"
#include "./X2Unit.h"

CX2Unit::CX2Unit( const KUnitInfo& unitInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, unitInfo );
    m_UnitData.Init( this, unitInfo );
	Init();
}

CX2Unit::CX2Unit( const KRoomUserInfo& RoomUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, RoomUserInfo );
    m_UnitData.Init( this, RoomUserInfo );
	Init();
	if( RoomUserInfo.m_vecPet.size() > 0 )
		SetFullPetInfo( RoomUserInfo.m_vecPet[0] );
}

CX2Unit::CX2Unit( const KSquareUserInfo& pKSquareUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, pKSquareUserInfo );
    m_UnitData.Init( this, pKSquareUserInfo );
	Init();
}


CX2Unit::CX2Unit( const KFieldUserInfo& pKFieldUserInfo )
    //: m_pUnitData(NULL)
    : m_pUnitTemplet(NULL)
{
	//m_pUnitData					= new CX2Unit::UnitData( this, pKFieldUserInfo );
    m_UnitData.Init( this, pKFieldUserInfo );
	Init();
	if( pKFieldUserInfo.m_vecPet.size() > 0 )
		SetSimplePetInfo( pKFieldUserInfo.m_vecPet[0] );
}


void CX2Unit::Init()
{
	m_pUnitTemplet				= g_pData->GetUnitManager()->GetUnitTemplet( m_UnitData.m_UnitClass );
    ASSERT( m_pUnitTemplet != NULL );

	m_bIsLevelUp				= false;
	m_PrevLevel					= m_UnitData.m_Level;

	m_PrevEXP					= m_UnitData.m_EXP;
	m_PrevNowBaseLevelEXP		= m_UnitData.m_NowBaseLevelEXP;
	m_PrevNextBaseLevelEXP		= m_UnitData.m_NextBaseLevelEXP;

	m_ResurrectionStoneNum		= 0;

	//{{ 2007. 10. 5  ������
	m_bMyInfoInDungoenRoom		= false;
	//}}

	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );

	ResetEqip();

	//{{ kimhc // 2009-12-04 // ũ�������� �̺�Ʈ
	m_mapEventData.clear();
	//}} kimhc // 2009-12-04 // ũ�������� �̺�Ʈ
	m_bReceivedGuestUserReward				= true;
	m_bGuestUserReachedLevelLimit			= false;
	m_bGuestUserMovingToCharacterSelect		= false;


	//{{ ����� : [2009/10/14] //	�ڵ� ���� ��Ȱ��
	//}} ����� : [2009/10/14] //	�ڵ� ���� ��Ȱ��

	m_iMaxSKillNotePage = 0;

	m_SummonPetUid = 0;

	m_bTradeBlockUnit = false;

	m_bExpandQuickSlot = false;

	SetSumOfItemLevel( 0 );

	m_iRecentEnterDungeonID = 0;

	m_eServerGroupID = SEnum::SGI_INVALID;

}

CX2Unit::~CX2Unit(void)
{
	//SAFE_DELETE( m_pUnitData );

	m_ViewEqipItemUIDList.clear();
	m_FashionEqipItemUIDList.clear();
	m_NormalEqipItemUIDList.clear();

	//{{ kimhc // 2009-12-04 // ũ�������� �̺�Ʈ
	m_mapEventData.clear();

	m_vecPetInfo.clear();

	m_vecRidingPetInfo.clear();
}





void CX2Unit::Reset( const KUnitInfo& unitInfo, bool bForceUpdate /* = false */ )
{
	bool bRefresh2014UI = false;

	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		GetUID() == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		if( static_cast< int >( unitInfo.m_ucLevel ) != static_cast< int >( m_UnitData.m_Level ) ||
			static_cast< CX2Unit::UNIT_CLASS >( unitInfo.m_cUnitClass ) != m_UnitData.m_UnitClass )
		{
			bRefresh2014UI = true;
		}
	}


	// XS_PVP_GAME = 9
	if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
	{
		if ( (int)m_UnitData.m_Level < (int)unitInfo.m_ucLevel )
		{
			SetIsLevelUp( true );
		}
	}


	//SAFE_DELETE( m_pUnitData );

	m_ViewEqipItemUIDList.resize(0);
	m_FashionEqipItemUIDList.resize(0);
	m_NormalEqipItemUIDList.resize(0);


	//m_pUnitData		= new CX2Unit::UnitData( this, unitInfo );
	m_PrevLevel					= m_UnitData.m_Level;

	m_PrevEXP					= m_UnitData.m_EXP;
	m_PrevNowBaseLevelEXP		= m_UnitData.m_NowBaseLevelEXP;
	m_PrevNextBaseLevelEXP		= m_UnitData.m_NextBaseLevelEXP;

	m_UnitData.SetKUnitInfo( unitInfo );
	m_pUnitTemplet	= g_pData->GetUnitManager()->GetUnitTemplet( m_UnitData.m_UnitClass );

	m_mapDungeonPlay.clear();
	m_mapDungeonPlay.insert( unitInfo.m_mapDungeonPlay.begin(), unitInfo.m_mapDungeonPlay.end() );


	if ( bForceUpdate == true )
	{
		// Dungeon Clear Data
		m_mapDungeonClear.clear();
		m_mapDungeonClear.insert( unitInfo.m_mapDungeonClear.begin(), unitInfo.m_mapDungeonClear.end() );
	}
	else
	{
		std::map< int, KDungeonClearInfo >::const_iterator mit;

		for ( mit = unitInfo.m_mapDungeonClear.begin(); mit != unitInfo.m_mapDungeonClear.end(); mit++ )
		{
			std::map< int, KDungeonClearInfo >::const_iterator iter2;
			iter2 = m_mapDungeonClear.find( mit->first );
			if( iter2 == m_mapDungeonClear.end() )
			{
				m_mapDungeonClear.insert( std::make_pair( mit->first, mit->second ) );
			}
		}
	}

	m_mapTCClear.clear();
	m_mapTCClear = unitInfo.m_mapTCClear;

	if( NULL != m_pUnitTemplet )
	{
		g_pData->GetItemStatCalculator().SetUnitData( m_pUnitTemplet->m_UnitType, m_pUnitTemplet->m_UnitClass);
	}


	ResetEqip();

	if( bRefresh2014UI )
		g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();

}



void CX2Unit::ResetInventory( std::map< int, int >& mapInventorySlotSize, std::map< UidType, KInventoryItemInfo >& mapItem )
{
	//ASSERT( m_pUnitData != NULL );
    m_UnitData.m_Inventory.Reset( mapInventorySlotSize, mapItem );
}

void CX2Unit::ResetEqip()
{
	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );

	m_NormalEqipItemUIDList.resize(0);
	m_ViewEqipItemUIDList.resize(0);
	m_FashionEqipItemUIDList.resize(0);


	for( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		CX2Item* pCX2Item = GetInventory().GetItem( m_UnitData.m_NowEqipItemUIDList[i] );
//		ASSERT( pCX2Item != NULL );
		if( pCX2Item == NULL )
			continue;

		if ( IsPossibleAddEqip( pCX2Item ) == false )
			continue;

		//������ �߰�
		if( pCX2Item->GetItemTemplet()->GetFashion() == true )
		{
			m_FashionEqipItemUIDList.push_back( pCX2Item->GetUID() );
		}
		else
		{
			m_NormalEqipItemUIDList.push_back( pCX2Item->GetUID() );
		}

		//������ġ ������Ʈ
		UpdateEqipPosition( pCX2Item, true );

	}

	//{{ dmlee 2008.05.12
	//ViewList�� ������Ʈ�Ѵ�
	UpdateViewEqip();
	UpdateViewEqipPosition();
	//}} dmlee 2008.05.12
}



void CX2Unit::ResetUnitClass( CX2Unit::UNIT_CLASS eUnitClass )
{
	if( m_UnitData.m_UnitClass == eUnitClass )
		return;

	m_UnitData.m_UnitClass = eUnitClass;
	m_UnitData.m_UserSkillTree.SetUnitClass( eUnitClass );
	//{{ [11/11/2009 : oasis907 ]
	// �ӽ�
	//m_UnitData.m_GuildClass = 0; // Not Now
	m_UnitData.m_UserSkillTree.SetGuildClass( 0 );
	//}}  [11/11/2009 : oasis907 ]
	m_pUnitTemplet = g_pData->GetUnitManager()->GetUnitTemplet( eUnitClass );


}


bool CX2Unit::AddEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return AddEqip( pCX2Item );
}

bool CX2Unit::AddEqip( CX2Item* pItem )
{
	//�ڱ� �κ��丮�� �ִ� ���������� Ȯ���غ��� ���� ������??

	if( pItem == NULL )
		return false;
/*
	if ( pItem->GetItemTemplet()->GetEqipType() == CX2Item::ET_NONE )
		return false;

	//�ɷ�ġ�� �Ǵ��� Ȯ���Ѵ�
	if( EqipAbility( pItem ) == false )
		return false;

	//������ ����� ��ġ ������ ������Ʈ�Ѵ�
	//UpdateFashionEqipPosition();
	//UpdateNormalEqipPosition();

	//�浹���� �ʴ��� Ȯ���Ѵ�
	if( EqipConflict( pItem ) == true )
		return false;
*/

	if ( IsPossibleAddEqip( pItem ) == false )
		return false;

	//������ �߰�
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		m_FashionEqipItemUIDList.push_back( pItem->GetUID() );
	}
	else
	{
		m_NormalEqipItemUIDList.push_back( pItem->GetUID() );
	}

	//���� ����Ʈ ������Ʈ
	m_UnitData.m_NowEqipItemUIDList.push_back( pItem->GetUID() );

	//������ġ ������Ʈ
	UpdateEqipPosition( pItem, true );

	//ViewList�� ������Ʈ�Ѵ�
	UpdateViewEqip();
	UpdateViewEqipPosition();

	return true;
}

bool CX2Unit::RemoveEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return RemoveEqip( pCX2Item );
}

bool CX2Unit::RemoveEqip( CX2Item* pItem )
{
	if( pItem == NULL )
		return false;

	if( pItem->GetItemTemplet() == NULL )
		return false;

	if ( pItem->GetItemTemplet()->GetUseType() == CX2Item::UT_NONE )
		return false;

	//������ ����
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		for ( int i = 0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
		{
			UidType uidType = m_FashionEqipItemUIDList[i];
			if ( uidType == pItem->GetUID() )
			{
				m_FashionEqipItemUIDList.erase( m_FashionEqipItemUIDList.begin() + i );
				break;
			}
		}
	}
	else
	{
		for ( int i = 0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
		{
			UidType uidType = m_NormalEqipItemUIDList[i];
			if ( uidType == pItem->GetUID() )
			{
				m_NormalEqipItemUIDList.erase( m_NormalEqipItemUIDList.begin() + i );
				break;
			}
		}
	}

	//���� ����Ʈ ������Ʈ
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType uidType = m_UnitData.m_NowEqipItemUIDList[i];
		if ( uidType == pItem->GetUID() )
		{
			m_UnitData.m_NowEqipItemUIDList.erase( m_UnitData.m_NowEqipItemUIDList.begin() + i );
			break;
		}
	}

	//������ġ ������Ʈ
	UpdateEqipPosition( pItem, false );

	//ViewList�� ������Ʈ�Ѵ�
	UpdateViewEqip();
	UpdateViewEqipPosition();

	return true;
}

bool CX2Unit::IsPossibleAddEqip( UidType itemUID )
{
	CX2Item* pCX2Item = GetInventory().GetItem( itemUID );
	return IsPossibleAddEqip( pCX2Item );
}
bool CX2Unit::IsPossibleAddEqip( CX2Item* pItem )
{
	if( pItem == NULL )
		return false;

	if( pItem->GetItemTemplet() == NULL )
		return false;

	if ( pItem->GetItemTemplet()->GetUseType() == CX2Item::UT_NONE )
		return false;

	//�ɷ�ġ�� �Ǵ��� Ȯ���Ѵ�
	if( EqipAbility( pItem ) == false )
		return false;

	//�浹���� �ʴ��� Ȯ���Ѵ�
	if( EqipConflict( pItem ) == true )
		return false;


	return true;
}

bool CX2Unit::CheckNowEquipItem( UidType itemUID )
{
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType uidType = m_UnitData.m_NowEqipItemUIDList[i];

		if ( uidType == itemUID )
		{
			return true;
		}
	}

	return false;
}

bool CX2Unit::EqipAbility( int itemID )
{
	CX2Item x2Item( itemID );
	return EqipAbility( &x2Item );
}


bool CX2Unit::EqipAbility( CX2Item* pItem )
{
	if ( pItem == NULL )
		return false;

	const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
	if ( pItemTemplet == NULL )
		return false;

	if( false == CX2Unit::CanEquipAsParts(
        pItemTemplet->GetItemID(),
        this, m_UnitData.m_Level ) )
		return false;


	// �� ������ ��� item data�� ��ȿ������, �� ������ �ƴ� ��쿡 itemdata�� ��ȿ���� ���� �� �ִ�.
	// ���� �� ������ �ƴ� ��쿡 ������ �˻� ����
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		GetUID() == g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		// ������ �˻�
		const CX2Item::ItemData& kItemData = pItem->GetItemData();
		if( CX2Item::PT_ENDURANCE == kItemData.m_PeriodType )
		{
			if( kItemData.m_Endurance <= 0 )
				return false;
		}
		// ��ȭ ���� �˻� (���Ҵ��ΰ�?)
		if( true == pItem->IsDisabled() )
		{
			return false;
		}

		if( false == pItem->GetIsEvaluation() )
		{// �̰��� ���¶�� ���� �� �� ������ �ϱ�
			return false;
		}
	}

	return true;
}

bool CX2Unit::AddClearDungeon( int dungeonID )
{
	std::map< int, KDungeonClearInfo >::iterator mit;
	mit = m_mapDungeonClear.find( dungeonID );
	if ( mit == m_mapDungeonClear.end() )
	{
		KDungeonClearInfo kDungeonClearInfo;

		m_mapDungeonClear.insert( std::make_pair( (int)dungeonID, kDungeonClearInfo ) );

		return true;
	}

	return false;
}

bool CX2Unit::UpdateViewEqipPosition()
{
	memset( m_ViewEqipPosition,		0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_ViewEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_ViewEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_ViewEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

bool CX2Unit::UpdateFashionEqipPosition()
{
	memset( m_FashionEqipPosition,	0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_FashionEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_FashionEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

bool CX2Unit::UpdateNormalEqipPosition()
{
	memset( m_NormalEqipPosition,	0, sizeof(bool) * EP_END );
	for( int i =0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			return false;

		m_NormalEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}

	return true;
}

void CX2Unit::UpdateEqipPosition( CX2Item* pItem, bool bAdd )
{
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		m_FashionEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] = bAdd;
	}
	else
	{
		m_NormalEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] = bAdd;
	}
}

bool CX2Unit::EqipConflict( CX2Item* pItem )
{
	if( pItem->GetItemTemplet()->GetFashion() == true )
	{
		if( m_FashionEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//�浹!
			return true;
		}
	}
	else
	{
		if( m_NormalEqipPosition[pItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//�浹!
			return true;
		}
	}
	return false;
}

bool CX2Unit::UpdateViewEqip()
{
	m_ViewEqipItemUIDList.resize(0);
	SetSumOfItemLevel( 0 );	// �ʱ�ȭ

	for( int i =0; i < (int)m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			continue;

		AddItemLevel( pEqipItem );

		if( m_FashionEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] == true )
		{
			//�浹!
			continue;
		}



		m_ViewEqipItemUIDList.push_back( pEqipItem->GetUID() );
	}

	SetSumOfItemLevel( GetSumOfItemLevel() / 8 );

	for( int i =0; i < (int)m_FashionEqipItemUIDList.size(); i++ )
	{
		m_ViewEqipItemUIDList.push_back( m_FashionEqipItemUIDList[i] );
	}
	return true;
}

//{{ kimhc // 2011.1.14 // û 1�� ����
//

/** @function : UpdateEnchantStatFromPassiveSkill
	@brief : �нú� ��ų������ �Ӽ� ���� ������Ʈ
*/
void CX2Unit::UpdateEnchantStatFromPassiveSkill()
{
	const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	int iSkillLevel = 0;
	m_EnchantStatFromPassiveSkill.Init();

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A, true );

	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefWater	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WATER_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefNature	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_NATURE_DEFENCE_ABS, iSkillLevel );
		}
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B, true );

	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B );
		if( pSkillTemplet != NULL )
		{
			m_EnchantStatFromPassiveSkill.m_fDefWind	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WIND_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefLight	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_LIGHT_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefDark	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DARK_DEFENCE_ABS, iSkillLevel );
		}
	}
	// ���� ��ü
	{
		const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SPK_BODY_OF_FIRE;
		const int iSkillLevel = refUserSkillTree.GetSkillLevel( eSkillID );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			}
		}
	}
}
//}} kimhc // 2011.1.14 // û 1�� ����

//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü
bool CX2Unit::IsNormalEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_NormalEqipPosition[equipPosition];
}

bool CX2Unit::IsFashionEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_FashionEqipPosition[equipPosition];
}

bool CX2Unit::IsViewEquipInThePosition( EQIP_POSITION equipPosition ) const
{
	return m_ViewEqipPosition[equipPosition];
}

CX2Item* CX2Unit::CreateBasicEquip( CX2Unit::EQIP_POSITION equipPosition )
{
	CX2Item::ItemData kBasicEquipData;

	switch ( equipPosition )
	{
	case CX2Unit::EP_DEFENCE_HAIR:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicHairItemID;
		break;
	case CX2Unit::EP_DEFENCE_FACE:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicFaceItemID;
		break;
	case CX2Unit::EP_DEFENCE_BODY:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicBodyItemID;
		break;
	case CX2Unit::EP_DEFENCE_LEG:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicLegItemID;
		break;
	case CX2Unit::EP_DEFENCE_HAND:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicHandItemID;
		break;
	case CX2Unit::EP_DEFENCE_FOOT:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicFootItemID;
		break;
	case CX2Unit::EP_WEAPON_HAND:
		kBasicEquipData.m_ItemID = GetUnitTemplet()->m_BasicWeaponItemID;
		break;
	default:
		break;
	}
	if ( kBasicEquipData.m_ItemID == 0 )		// ���� ���ǿ� ���� �Ǽ��縮�� ���
	{
		return NULL;
	}
	kBasicEquipData.m_ItemUID		= 0;
	kBasicEquipData.m_Endurance	= 1;

	return new CX2Item( kBasicEquipData, this );
}

//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü

//{{ kimhc // 2010.12.20 // 2010-12-23 New Character CHUNG
/** @function : IsBasicEquip
	@brief : �ƹ��͵� �������� ���� �⺻ ��� ������ ������
	@param : const int iItemID_ (�⺻������� �˰� �;��ϴ� �������� ID)
	@return : bool (�⺻���� true)
*/
bool CX2Unit::IsBasicEquip( const int iItemID_ )
{
	const CX2Unit::UnitTemplet *pUnitTemplet = GetUnitTemplet();

	if ( iItemID_ == pUnitTemplet->m_BasicWeaponItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicHairItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicFaceItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicBodyItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicLegItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicHandItemID )
		return true;
	else if ( iItemID_ == pUnitTemplet->m_BasicFootItemID )
		return true;
	else
		return false;
}
//}} kimhc // 2010.12.20 //  2010-12-23 New Character CHUNG


// Ŭ������ �����߿� ���� ������ ���� ���� ID�� return
int CX2Unit::GetLastClearDungeonID()
{
	if( NULL == g_pData->GetDungeonManager() )
		return -1;

	int iRequireLevel = 0;
	int iDungeonID = -1;

	const int iMyLevel = GetUnitData().m_Level;

	std::map< int, KDungeonClearInfo >::iterator it;
	for( it = m_mapDungeonClear.begin(); it != m_mapDungeonClear.end(); it++ )
	{
		const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( (SEnum::DUNGEON_ID) it->first );
		if( NULL == pDungeonData )
			continue;

		// ���̵��� ���븸
		if( CX2Dungeon::DL_NORMAL != pDungeonData->m_eDifficulty )
			continue;

		if( pDungeonData->m_DungeonID == SEnum::DI_BATTLE_SHIP_VELDER ||
			pDungeonData->m_DungeonID == SEnum::DI_BATTLE_SHIP_HAMEL )
			continue;

		const int iAdequateLevelToPlay = pDungeonData->m_MinLevel - 2;

		// ĳ������ ������ �����÷��̷��� ���� ���ų� ����, Ŭ������ ������ �߿� ���� ������
		if ( iMyLevel >= iAdequateLevelToPlay && iAdequateLevelToPlay > iRequireLevel )
		{
			iDungeonID		= pDungeonData->m_DungeonID;
			iRequireLevel	= pDungeonData->m_MinLevel;
		}
	}

	return iDungeonID;
}

bool CX2Unit::IsClearDungeon( int iDungeonID )
{
	std::map< int, KDungeonClearInfo >::iterator iD = m_mapDungeonClear.find( iDungeonID );
	if ( iD != m_mapDungeonClear.end() )
		return true;

	return false;
}

bool CX2Unit::IsClearDungeon( vector<int> &vecDungeonId )
{
	if(vecDungeonId.size() <= 0)
		return true;

	for(UINT i=0; i<vecDungeonId.size(); ++i)
	{
		std::map< int, KDungeonClearInfo >::iterator iD = m_mapDungeonClear.find( vecDungeonId[i] );
		if ( iD == m_mapDungeonClear.end() )
		{
			return false;
		}
	}

	return true;
}

float CX2Unit::ResetMaxMp( const float fMaxMp_ )
{
	float fResultMaxMp = fMaxMp_;

	// ������ ����
	int iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AHM_UNLIMIT_MANA, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AHM_UNLIMIT_MANA );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

	// ������ ����� �������ֱ�(MAX MP �߰�)
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS,iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_FILLED_POWER, true ); // �ƶ� 1�� ���� �Ҽ� - �游�� ��

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ALH_FILLED_POWER );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

	// ���� ����
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AVP_MAGIC_AMPLIFICATION );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}

	// ��� ���� �нú� ���� ��, �ִ� MP �� ���� �߰�
	iSkillLevel = GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION );

		if( NULL != pSkillTemplet )
		{
			const float maxMPIncrement = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );
			fResultMaxMp += maxMPIncrement;
		}
	}


	//{{ kimhc // 2009-10-19 // �ִ� MP ���� �� �߰�
	{
		const float maxMPIncrement = GetInventory().GetAddMaxMPValue();
		fResultMaxMp += maxMPIncrement;
	}
	//}} kimhc // 2009-10-19 // �ִ� MP ���� �� �߰�


	return fResultMaxMp;
}

CX2Stat::Stat CX2Unit::GetUnitStat() const
{
	CX2Stat::Stat retval = m_UnitData.m_Stat;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	if( pSkillTree != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = m_UnitData.m_UserSkillTree;

		int iSkillLevel = 0;

		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ES_POWERFUL_VITAL, true );

		if( iSkillLevel > 0)
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ES_POWERFUL_VITAL );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel );
			}
		}

		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ELK_STRONG_BONE, true );
		if( iSkillLevel > 0)
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ELK_STRONG_BONE );

			if( pSkillTemplet != NULL )
			{
				retval.m_fBaseHP *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel );
			}
		}

		//{{ kimhc // 2011.1.14 // û 1�� ����
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_GUARD_MASTERY, true );

		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_GUARD_MASTERY );
			if ( NULL != pSkillTemplet )
			{
				retval.m_fBaseHP += m_UnitData.m_Stat.m_fBaseHP *
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel ) );
			} // if

		} // if
		//}} kimhc // 2011.1.14 // û 1�� ����

		//#pragma region SI_P_ASD_SELF_PROTECTION_FORTITUDE
		iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE, true );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_SELF_PROTECTION_FORTITUDE  );

			if( NULL != pSkillTemplet )
			{
				retval.m_fDefPhysic += m_UnitData.m_Stat.m_fDefPhysic *
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_PHYSIC_REL, iSkillLevel ) );
				retval.m_fDefMagic += m_UnitData.m_Stat.m_fDefMagic *
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DEF_MAGIC_REL, iSkillLevel ) );
				retval.m_fBaseHP += m_UnitData.m_Stat.m_fBaseHP *
					CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillLevel ) );
			}
		}
		//#pragma endregion ȣ�Ű���
	}
	return retval;

}

CX2Stat::Stat CX2Unit::GetEqipStat() const
{
	CX2Stat::Stat retStat;
	for( int i = 0; i < GetFashionEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetFashionEqipUID(i) );

		if ( pItem != NULL
            && pItem->GetItemTemplet() != NULL
            )
		{
			//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
			// ���� �ɼ��� �������� �ʵ��� ����
			retStat.AddStat( pItem->GetStat() );
			//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
		}
	}

	for( int i = 0; i < GetNormalEqipNum(); i++ )
	{
		CX2Item* pItem = GetInventory().GetItem( GetNormalEqipUID(i) );

		if ( pItem != NULL
            && pItem->GetItemTemplet() != NULL
            )
		{
			//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ �Ҹ�
			if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
				continue;
			//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ �Ҹ�

			//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
			// ���� �ɼ��� �������� �ʵ��� ����
			retStat.AddStat( pItem->GetStat() );
			//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
		}
	}

	return retStat;
}



// �Ӽ� enchant�� ���� stat, �� �Ӽ� ��� ��ġ�� ������ش�
// @bUpdate: true�� ��� stat�� �ٽ� ������ְ�, false�� ��� ������ ����ص״� ���� �ٷ� return


// UI��¿� ���� ��� �� ���� ����ϴ� �Ű�����
// @bShowDungeonOptionUI_ : ������ ���� �������� ����
// @bShowPvpOptionUI : ������ ���� �������� ����
const CX2Stat::EnchantStat& CX2Unit::GetEnchantStat( bool bUpdate /*= true*/, bool bShowDungeonOptionUI_ /*= false*/, bool bShowPvpOptionUI_ /*= false */)
{
	if( false == bUpdate )
		return m_EnchantStat;

	m_EnchantStat.Init();

	for( UINT i=0; i< m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType eqipUID = m_UnitData.m_NowEqipItemUIDList[i];
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
		if( NULL == pItemTemplet )
			continue;


		if( pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE )
		{
			CX2DamageManager::EXTRA_DAMAGE_TYPE currExtraDamageType = g_pData->GetEnchantItem()->GetExtraDamageType( pItem->GetItemData().m_EnchantedAttribute );

			switch( currExtraDamageType )
			{

			case CX2DamageManager::EDT_ENCHANT_BLAZE:						m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN:						m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_POISON:						m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE; 		break;
			case CX2DamageManager::EDT_ENCHANT_PIERCING:					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_SHOCK:						m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE; 			break;
			case CX2DamageManager::EDT_ENCHANT_SNATCH:						m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE; 			break;

			case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:				m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:				m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:				m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:				m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:				m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;
			case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:				m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_MASTER_VALUE;	break;


			case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
				{
					m_EnchantStat.m_fDefBlaze += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE;
				} break;

			case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
				{
					m_EnchantStat.m_fDefWater += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefWind += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefLight += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
				{
					m_EnchantStat.m_fDefNature += (int)CX2EnchantItem::EAR_VALUE;
					m_EnchantStat.m_fDefDark += (int)CX2EnchantItem::EAR_VALUE;
				} break;
			default:
				break;
			}
		}


		// ������ ������ ���� �ɼǿ� ����
        unsigned uiNumSocketOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uiNumSocketOption; j++ )
		{
            int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}


		// �����ۿ� ������ ���� ���Ͽ� ����
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}

		// ������ ���� ��� ���� ���� �ɼ�
		for( int iSocketID : pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );

			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( iSocketID );
		}

	} // for(i)


//#ifdef TITLE_SYSTEM
	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() )->m_vecSocketOption;

		for(UINT i=0; i<vecTitleSocketOption.size(); ++i)
		{
			int socketOptionID = vecTitleSocketOption[i];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( false == IsShowDungeonOnly( pSocketData->m_bDungeonOnly, bShowDungeonOptionUI_ ) )
				continue;

			if( false == IsShowPvpOnly( pSocketData->m_bPVPOnly, bShowPvpOptionUI_ ) )
				continue;

			AddEchantStatFromSocket( socketOptionID );
		}
	}
//#endif TITLE_SYSTEM

	KPetInfo *pPetInfo = GetPetInfo();
	if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetMyPet() != NULL && pPetInfo != NULL )
	{
		// �������� 40% �̻��� ��츸 ��ų ����ǵ��� �Ѵ�.
		if( (float)pPetInfo->m_sSatiety / CX2PetManager::MAX_OF_SATIETY >= CX2PetManager::SATIETY_RATIO_TO_BE_ABLE_TO_APPLY_AURA )
		{
			CX2PetManager::PetTemplet *pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)pPetInfo->m_iPetID );
			int iStep = pPetTemplet->m_vecPetStatus[pPetInfo->m_cEvolutionStep];
			if( pPetTemplet != NULL && iStep > 0 )
			{
				for(int i=0; i < iStep; ++i)
				{
					const std::vector<int>& vecAuraSkillOptionTemp = pPetTemplet->m_vecAuraSkillOption[ i ];
					for( UINT j=0; j<vecAuraSkillOptionTemp.size(); ++j)
					{
						AddEchantStatFromSocket( vecAuraSkillOptionTemp[j] );
					}
				}
			}
		}
	}

	// ��Ʈ �����ۿ� ���� �Ӽ��������
	map<int,int> mapSetIDNParts;
	map<int,int>::iterator it;

	GetSetIDNPartsNum( mapSetIDNParts );
	vector< int > vecSetItemOptions;

	for( it = mapSetIDNParts.begin(); it != mapSetIDNParts.end(); it ++ )
	{
		g_pData->GetItemManager()->GetSetItemOptions( it->first, it->second, vecSetItemOptions );
	}

	for ( int i = 0; i < (int)vecSetItemOptions.size(); i++ )
	{
		AddEchantStatFromSocket( vecSetItemOptions[i] );
	}


//{{ kimhc // 2011.1.14 // û 1�� ����
	UpdateEnchantStatFromPassiveSkill();

	m_EnchantStat += GetEnchantStatFromPassiveSkill();
//}} kimhc // 2011.1.14 // û 1�� ����

	//{{ kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����
	AddEnchantStatFromDragonBreath( m_EnchantStat );
	//}} kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����

	if( m_EnchantStat.m_fDefBlaze > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefBlaze = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefWater > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefWater = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefNature > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefNature = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefWind > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefWind = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefLight > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefLight = CX2EnchantItem::EAR_MAX_VALUE;
	if( m_EnchantStat.m_fDefDark > CX2EnchantItem::EAR_MAX_VALUE )
		m_EnchantStat.m_fDefDark = CX2EnchantItem::EAR_MAX_VALUE;

	return m_EnchantStat;
}


void CX2Unit::AddEchantStatFromSocket( int iSocketOptionID )
{
	const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketOptionID );

	if( NULL == pSocketData )
		return;



	// �Ӽ� ����
	for( int k= CX2EnchantItem::ET_NONE+1; k<CX2EnchantItem::ET_END; k++ )
	{
		switch( k )
		{
		case CX2EnchantItem::ET_BLAZE:
			m_EnchantStat.m_fDefBlaze += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_WATER:
			m_EnchantStat.m_fDefWater += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_NATURE:
			m_EnchantStat.m_fDefNature += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_WIND:
			m_EnchantStat.m_fDefWind += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_LIGHT:
			m_EnchantStat.m_fDefLight += (float) pSocketData->m_aiResistEnchant[ k ];
			break;

		case CX2EnchantItem::ET_DARK:
			m_EnchantStat.m_fDefDark += (float) pSocketData->m_aiResistEnchant[ k ];
			break;
		}
	}

}

//{{ kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����
void CX2Unit::AddEnchantStatFromDragonBreath( OUT CX2Stat::EnchantStat& enchantStat_ )
{
	if ( NULL != g_pData->GetWorldMissionManager() )
	{
		const CX2Stat::EnchantStat* pEnchantStatDragonBreath = NULL;

		// 2�ܰ� ��������� ���� ���̾ ������ EnchantStat�� ������
		if ( true == g_pData->GetWorldMissionManager()->GetDragonBreathEnchantStatIncreased( pEnchantStatDragonBreath ) )
			enchantStat_ += *pEnchantStatDragonBreath;
	}
}
//}} kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����


float CX2Unit::GetRepairDiscountRate()
{
	float fDiscountRate = 0.f;


	for( const UidType& eqipUID : m_UnitData.m_NowEqipItemUIDList )
	{
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
		if( NULL == pItemTemplet )
			continue;

		// ������ ������ ���� �ɼǿ� ����
        unsigned uNumSockOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uNumSockOption; j++ )
		{
            int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}


		// �����ۿ� ������ ���� ���Ͽ� ����
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
		// ������ ���� ��� ���� ���� �ɼ�
		for( int iSocketID : pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
	}



	// Īȣ
	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( GetTitleId() )->m_vecSocketOption;

		for(UINT i=0; i<vecTitleSocketOption.size(); ++i)
		{
			int socketOptionID = vecTitleSocketOption[i];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			if( 0.f == pSocketData->m_fRepairPriceSale )
				continue;

			fDiscountRate += pSocketData->m_fRepairPriceSale;
		}
	}




	// ��Ʈ �����ۿ� ����
	map<int,int> mapSetIDNParts;
	map<int,int>::iterator it;

	GetSetIDNPartsNum( mapSetIDNParts );
	vector< int > vecSetItemOptions;

	for( it = mapSetIDNParts.begin(); it != mapSetIDNParts.end(); it ++ )
	{
		g_pData->GetItemManager()->GetSetItemOptions( it->first, it->second, vecSetItemOptions );
	}

	for ( int i = 0; i < (int)vecSetItemOptions.size(); i++ )
	{
		int socketOptionID = vecSetItemOptions[i];
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
		if( NULL == pSocketData )
			continue;

		if( 0.f == pSocketData->m_fRepairPriceSale )
			continue;

		fDiscountRate += pSocketData->m_fRepairPriceSale;
	}


	for( BUFF_FACTOR_ID eBuffFactorID : m_vecWorldBuffFactorID )
	{
		if ( NULL != CX2BuffTempletManager::GetInstance() )
		{
			CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( eBuffFactorID );

			if( NULL != ptrBuffFactor )
			{
				switch ( ptrBuffFactor->GetBuffTempletID() )
				{
				case BTI_BUFF_2013_DEFENSE_BUFF_EVENT:	/// �̺�Ʈ�� 2013 ����� �� ������ ����
					{
						fDiscountRate += 0.7f;
					} break;
				case BTI_BUFF_2013_DEFENSE_BUFF:		/// 2013 ����� �� ������ ��¥ ����
					{
						fDiscountRate += 0.5f;
					} break;
				case BTI_BUFF_2013_DEFENSE_DEBUFF:		/// 2013 ����� �� ���� �����
					{
						fDiscountRate -= 1.5f;
					} break;
				default:
					break;
				}
			}
		}
	}

	if( fDiscountRate > CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT )
		fDiscountRate = CX2SocketItem::SocketData::MAX_REPAIR_PRICE_DISCOUNT;

	if ( 0.f <= fDiscountRate )		/// ������ ���ҽ�
		fDiscountRate = 1.f - fDiscountRate;
	else							/// ������ ������
		fDiscountRate *= -1.f ;


	return fDiscountRate;
}

const char* CX2Unit::GetEmotionName(EMOTION_TYPE eEmotionType)
{
	// �̸�� id�� �ش�Ǵ� ������Ʈ�̸��� ��ȯ�Ѵ�.
	switch(eEmotionType)
	{
	case ET_NONE:
		return "";
	case ET_SITREADY:
		return "EMOTION_SITREADY";
	case ET_SITWAIT:
		return "EMOTION_SITWAIT";
	case ET_STANDUP:
		return "EMOTION_STANDUP";
	case ET_ANGRY:
	case ET_HELLO:
	case ET_NO:
	case ET_SAD:
	case ET_SMILE:
	case ET_EMOTION_AVATAR1:
	case ET_EMOTION_AVATAR2:
	case ET_EMOTION_AVATAR3:
	case ET_EMOTION_AVATAR4:
	case ET_EMOTION_AVATAR5:
	case ET_EMOTION_AVATAR6:

	case ET_EMOTION_AVATAR7:

		return "EMOTION_STATE";
	default:
		return "";
	}

	return "";
}

wstring CX2Unit::GetEmotionAniNameById(EMOTION_TYPE eEmotionType)
{
	// �̸�� id�� �ش�Ǵ� ����̸��� ��ȯ�Ѵ�.
	switch(eEmotionType)
	{
	case ET_NONE:
		return L"";
	case ET_SITREADY:
		return L"Emotion_SitReady";
	case ET_SITWAIT:
		return L"EMOTION_SITWAIT";
	case ET_STANDUP:
		return L"Emotion_StandUp";
	case ET_ANGRY:
		return L"Emotion_Angry";
	case ET_HELLO:
		return L"Emotion_Hello";
	case ET_NO:
		return L"Emotion_No";
	case ET_SAD:
		return L"Emotion_Sad";
	case ET_SMILE:
		return L"Emotion_Smile";
	case ET_EMOTION_AVATAR1:
		return L"Emotion_BIGBANG";
	case ET_EMOTION_AVATAR2:
		return L"Emotion_DeepBow";
	case ET_EMOTION_AVATAR3:
		return L"Emotion_APINK";
	case ET_EMOTION_AVATAR4:
		return L"Emotion_APINK_LOVE";
	case ET_EMOTION_AVATAR5:
		return L"Emotion_CRAYONPOP";
	case ET_EMOTION_AVATAR6:
		return L"Emotion_BbaBbaBba";
	default:
		return L"";
	}

	return L"";
}

void CX2Unit::SetSkillNote( std::map<char, int> &mapSkillNote, char iMaxNotePage )
{
	m_mapSkillNote.clear();
	m_mapSkillNote = mapSkillNote;

	m_iMaxSKillNotePage= iMaxNotePage;


}

int CX2Unit::GetSkillMemoIdBySlot( char slotId )
{
	map<char, int>::iterator mit = m_mapSkillNote.find(slotId);

	if( mit != m_mapSkillNote.end() )
	{
		return mit->second;
	}

	return -1;
}

void CX2Unit::SetSkillNotePage(char iPage, int iMemo)
{
	map<char, int>::iterator mit = m_mapSkillNote.find(iPage);

	if( mit != m_mapSkillNote.end() )
	{
		mit->second = iMemo;
	}
	else
	{
		m_mapSkillNote.insert( std::make_pair(iPage, iMemo) );
	}
}




CX2Stat::Stat CX2Unit::GetSkillStat() const
{
	return m_UnitData.m_UserSkillTree.GetSkillStat();
}

//{{ kimhc // 2009-11-17 //��� ��ų�� ���õ� ����

CX2Stat::Stat CX2Unit::GetGuildSkillStat() const
{
	// oasis907 : ����� [2009.12.8] // ��� ���
	BYTE byMemberShipGrade = m_UnitData.m_byMemberShipGrade;
	return m_UnitData.m_UserSkillTree.GetGuildSkillStat(byMemberShipGrade);
}

//}} kimhc // 2009-11-17 //��� ��ų�� ���õ� ����



void CX2Unit::SetBlackList( vector<KChatBlackListUnit>& blackList )
{

		m_UnitData.SetBlackList( blackList );
}

bool CX2Unit::AddBlackList( KChatBlackListUnit blackList )
{

		return m_UnitData.AddBlackList( blackList );

}

bool CX2Unit::RemoveBlackList( UidType unitUID )
{
		return m_UnitData.RemoveBlackList( unitUID );

}


const vector<KChatBlackListUnit>& CX2Unit::GetBlackList() const
{
	return m_UnitData.GetBlackList();
}

UidType CX2Unit::GetBlackListUnitUID( const WCHAR* wszNickName )
{
	return m_UnitData.GetBlackListUnitUID( wszNickName );
}

void CX2Unit::GetSetIDNPartsNum( map<int,int>& mapSetIDNPartsNum )
{
	const CX2Inventory& kInventory = GetUnitData().m_Inventory;
	{
		for ( int i = 0;  i < kInventory.GetItemMaxNum( CX2Inventory::ST_E_EQUIP ); i++ )
		{
			CX2Item* pItem = kInventory.GetItem( CX2Inventory::ST_E_EQUIP, i );
			if ( pItem != NULL &&
                pItem->GetItemTemplet() != NULL
                )
			{
				//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����
				if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
					continue;
				//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����

                int setID = pItem->GetItemTemplet()->GetSetID();
				if ( setID != 0 )
				{
					map< int, int >::iterator mit = mapSetIDNPartsNum.find( setID );
					if ( mit != mapSetIDNPartsNum.end() )
					{
						mit->second += 1;
					}
					else
					{
						mapSetIDNPartsNum.insert( std::make_pair( setID, 1 ) );
					}
				}
			}
		}
	}

}




// �� ĳ���Ͱ� ����� �� �ִ� �������ΰ�?
/*static*/ bool CX2Unit::CanUse( int itemID, CX2Unit* pUnit, int iUnitLevel /*= -1*/ )
{
	ASSERT( NULL != g_pData->GetItemManager() );

	if( NULL == pUnit )
		return false;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	if( NULL == pItemTemplet )
		return false;

	if( -1 != iUnitLevel )
	{
		if( pUnit->GetUnitData().m_Level < pItemTemplet->GetUseLevel() )
			return false;
	}

	switch( pItemTemplet->GetUseCondition() )
	{
	case CX2Item::UC_NONE:
		{
			return false;
		} break;

	case CX2Item::UC_ANYONE:
		{
			return true;
		} break;

	case CX2Item::UC_ONE_UNIT:
		{
			return ( pItemTemplet->GetUnitType() == pUnit->GetType() );
		} break;

	default:
	case CX2Item::UC_ONE_CLASS:
		{
			return GetX2UnitClassCompatibility( pUnit->GetClass(), pItemTemplet->GetUnitClass() );
		} break;
	}

	return false;
}


// ���� ������ ���� ������ �� �ִ� ������ �������ΰ�?
// @iUnitLevel�� -1�̸� ���� ������ �˻����� �ʴ´�
/*static*/ bool CX2Unit::CanEquipAsParts( int itemID, CX2Unit* pUnit, int iUnitLevel /*= -1*/ )
{
	ASSERT( NULL != g_pData->GetItemManager() );

	if( NULL == pUnit )
		return false;


	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	if( NULL == pItemTemplet )
		return false;

	if( CX2Item::UT_NONE == pItemTemplet->GetUseType() ) // ������ ���� ��ġ�� �������� �ƴ���
		return false;

	if( -1 != iUnitLevel )
	{
		if( pUnit->GetUnitData().m_Level < pItemTemplet->GetUseLevel() ) // ���� ���ѿ� �ɸ��ٸ�
			return false;
	}

	// ������� �´ٸ� ���� ���ǿ� �´���
	switch( pItemTemplet->GetUseCondition() )
	{
	default:
	case CX2Item::UC_NONE:
		{
			return false;
		} break;

	case CX2Item::UC_ANYONE:
		{
			return true;
		} break;

	case CX2Item::UC_ONE_UNIT:
		{
			return pItemTemplet->GetUnitType() == pUnit->GetType();
		} break;

	case CX2Item::UC_ONE_CLASS:
		{
			return GetX2UnitClassCompatibility( pUnit->GetClass(), pItemTemplet->GetUnitClass() );
		} break;
	}

	return false;
}



// 1�� �����̸� 1�� return, 2�� �����̸� 2�� return,
int CX2Unit::GetClassLevel()
{
	switch( m_UnitData.m_UnitClass )
	{
	case UC_ELSWORD_SWORDMAN:
	case UC_ARME_VIOLET_MAGE:
	case UC_LIRE_ELVEN_RANGER:
	case UC_RAVEN_FIGHTER:
	case UC_EVE_NASOD:
//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
	case UC_CHUNG_IRON_CANNON:
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG
	case UC_ARA_MARTIAL_ARTIST:
	case UC_ELESIS_KNIGHT:

	case UC_ADD_NASOD_RULER:

		{
			return 0;
		} break;

	case UC_ELSWORD_KNIGHT:
	case UC_ELSWORD_MAGIC_KNIGHT:
	case UC_ELSWORD_SHEATH_KNIGHT:
	case UC_LIRE_COMBAT_RANGER:
	case UC_LIRE_SNIPING_RANGER:
	case UC_LIRE_TRAPPING_RANGER:
	case UC_ARME_HIGH_MAGICIAN:
	case UC_ARME_DARK_MAGICIAN:
	case UC_RAVEN_SOUL_TAKER:
	case UC_RAVEN_OVER_TAKER:
	case UC_EVE_EXOTIC_GEAR:
	case UC_EVE_ARCHITECTURE:
	//{{ kimhc // 2011.1.3 // û 1�� ����
	case UC_CHUNG_FURY_GUARDIAN:
	case UC_CHUNG_SHOOTING_GUARDIAN:
	//}} kimhc // 2011.1.3 // û 1�� ����
	case UC_ARME_BATTLE_MAGICIAN:
	case UC_RAVEN_WEAPON_TAKER:
	case UC_EVE_ELECTRA:
	case UC_CHUNG_SHELLING_GUARDIAN:
	case UC_ARA_LITTLE_HSIEN:
	case UC_ELESIS_SABER_KNIGHT:
	case UC_ELESIS_PYRO_KNIGHT:
	case UC_ARA_LITTLE_DEVIL:
	case UC_ADD_PSYCHIC_TRACER:
		{
			return 1;
		} break;

	case UC_ELSWORD_LORD_KNIGHT:
	case UC_ELSWORD_RUNE_SLAYER:
	case UC_LIRE_WIND_SNEAKER:
	case UC_LIRE_GRAND_ARCHER:
	case UC_ARME_ELEMENTAL_MASTER:
	case UC_ARME_VOID_PRINCESS:
	case UC_RAVEN_BLADE_MASTER:
	case UC_RAVEN_RECKLESS_FIST:

	//{{ kimhc // 2010.1.22 // �̺� 2�� ����
	case UC_EVE_CODE_NEMESIS:
	case UC_EVE_CODE_EMPRESS:
	//}} kimhc // 2010.1.22 // �̺� 2�� ����

		//{{ JHKang / ������ / 2011.6.13 / û 2�� ����
	case UC_CHUNG_IRON_PALADIN:
	case UC_CHUNG_DEADLY_CHASER:
		//}}
	case UC_ELSWORD_INFINITY_SWORD:
	case UC_ARME_DIMENSION_WITCH:
	case UC_LIRE_NIGHT_WATCHER:
	case UC_RAVEN_VETERAN_COMMANDER:
	case UC_EVE_BATTLE_SERAPH:
	case UC_CHUNG_TACTICAL_TROOPER:
	case UC_ARA_SAKRA_DEVANAM:
	case UC_ARA_YAMA_RAJA:
	case UC_ELESIS_GRAND_MASTER:
	case UC_ELESIS_BLAZING_HEART:
	case UC_ADD_LUNATIC_PSYKER:

		{
			return 2;
		} break;
	default:
		break;
	}


	return -1;
}

//{{ kimhc // 2010-01-04 // ������ EGS_SELECT_UNIT_ACK�� �κ������� ���ԵǾ� ���� �� ����
void	CX2Unit::ResetInventorySize( const std::map<int, int>& mapInventorySize )
{
	m_UnitData.m_Inventory.Clear();
	m_UnitData.m_Inventory.ResetSize( mapInventorySize );
}

void	CX2Unit::ResetInventoryItems( const std::map< UidType, KInventoryItemInfo >& mapItem )
{
	m_UnitData.m_Inventory.ResetItems( mapItem );

}
//}} kimhc // 2010-01-04 // ������ EGS_SELECT_UNIT_ACK�� �κ������� ���ԵǾ� ���� �� ����


void CX2Unit::SetSimplePetInfo( const KFieldPetInfo& kFieldPetInfo_ )
{
	KPetInfo kPetInfo;
	kPetInfo.m_iPetUID = kFieldPetInfo_.m_iPetUID;
	kPetInfo.m_iPetID = kFieldPetInfo_.m_iPetID;
	kPetInfo.m_wstrPetName = kFieldPetInfo_.m_wstrPetName;
	kPetInfo.m_cEvolutionStep = kFieldPetInfo_.m_cEvolutionStep;
	kPetInfo.m_iIntimacy = kFieldPetInfo_.m_iIntimacy;


	m_vecPetInfo.clear();
	m_vecPetInfo.push_back(kPetInfo);
}

void CX2Unit::SetFullPetInfo( const KPetInfo& kPetInfo_ )
{
	m_vecPetInfo.clear();
	m_vecPetInfo.push_back( kPetInfo_ );
}

KPetInfo *CX2Unit::GetPetInfo()
{
	if( m_vecPetInfo.size() > 0 )
	{
		return &m_vecPetInfo[0];
	}

	return NULL;
}

void CX2Unit::SetPetAutoLooting( UidType uiPetUID, bool bAutoLooting)
{
	if( m_vecPetInfo.size() == 0 )
		return;

	for(UINT i=0; i<m_vecPetInfo.size(); ++i)
	{
		KPetInfo &petInfo = m_vecPetInfo[i];
		if( petInfo.m_iPetUID == uiPetUID )
		{
			petInfo.m_bAutoLooting = bAutoLooting;
			return;
		}
	}
}

bool CX2Unit::CheckNowEquipItemByItemId( UidType itemID )
{
	for ( int i = 0; i < (int)m_UnitData.m_NowEqipItemUIDList.size(); i++ )
	{
		UidType itemUid = m_UnitData.m_NowEqipItemUIDList[i];

		CX2Item *pItem = GetInventory().GetItem( itemUid );

		if( pItem != NULL &&
            pItem->GetItemTemplet()->GetItemID() == itemID
            )
		{
			return true;
		}
	}
	return false;
}
bool CX2Unit::GetAvatarEmotion(wstring &wstrEmotionName, CX2Unit::EMOTION_TYPE &eEmotionID)
{
	switch( GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_Elsword_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_ARME:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Arme_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_ARME_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_LIRE:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Lire_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_LIRE_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_RAVEN:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Raven_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_RAVEN_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_EVE:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Eve_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Eve_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_EVE_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_CHUNG:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_BIGBANG.X";
					eEmotionID = ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Chung_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_APINK.X";
					eEmotionID = ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_APINK_LOVE.X";
					eEmotionID = ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Chung_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_CHUNG_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;

	case CX2Unit::UT_ARA:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Ara_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}

			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA5[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Ara_Emotion_CRAYONPOP_Sorted.X";
					eEmotionID = ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_ARA_Emotion_BbaBbaBba.X";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;


	case CX2Unit::UT_ELESIS:
		{
			{
				/// �Ѻ� �ƹ�Ÿ
				for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2 ); ++i )
				{
					if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2[i] ) == 6 )
					{
						wstrEmotionName = L"Motion_EL_Emotion_DeepBow.X";
						eEmotionID = ET_EMOTION_AVATAR2;
						return true;
					}
				}
			}
			break;

			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6[i] ) == 2 )
				{
					wstrEmotionName = L"Motion_EL_Emotion_BbaBbaBba.x";
					eEmotionID = ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;

	case CX2Unit::UT_ADD:
		{
			/// �Ѻ� �ƹ�Ÿ
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ADD2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ADD2[i] ) == 6 )
				{
					wstrEmotionName = L"Motion_Add_Emotion_DeepBow.X";
					eEmotionID = ET_EMOTION_AVATAR2;
					return true;
				}
			}
		}
		break;

	default:
		break;
	}

	return false;
}

bool CX2Unit::GetAvatarEmotionID(CX2Unit::EMOTION_TYPE &eEmotionID)
{
	switch( GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELSWORD6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_ARME:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARME6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARME6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_LIRE:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_LIRE6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_LIRE6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_RAVEN:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_RAVEN6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_EVE:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_EVE6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_EVE6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;
	case CX2Unit::UT_CHUNG:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG1[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR1;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG3[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR3;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG4[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR4;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_CHUNG6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;

	case CX2Unit::UT_ARA:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA5 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA5[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR5;
					return true;
				}
			}
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ARA6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ARA6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;

	case CX2Unit::UT_ELESIS:
		{
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}

			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ELESIS6[i] ) == 2 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR6;
					return true;
				}
			}
		}
		break;

	case CX2Unit::UT_ADD:
		{
			/// �Ѻ� �ƹ�Ÿ
			for( int i=0; i<std::ssize( ITEM_ID_ET_EMOTION_AVATAR_ADD2 ); ++i )
			{
				if( GetInventory().GetEqiuppingSetItemNum( ITEM_ID_ET_EMOTION_AVATAR_ADD2[i] ) == 6 )
				{
					eEmotionID = CX2Unit::ET_EMOTION_AVATAR2;
					return true;
				}
			}
		}
		break;

	default:
		break;
	}


	return false;
}


void CX2Unit::AddItemLevel( IN const CX2Item* pEquipItem_ )
{
    const CX2Item::ItemTemplet* pItemTemplet = pEquipItem_->GetItemTemplet();

	switch ( pItemTemplet->GetItemType() )
	{
	case CX2Item::IT_WEAPON:
		{
			SetSumOfItemLevel( GetSumOfItemLevel()
				+ CX2Item::WEAPON_LEVEL_WEIGHT * ( pItemTemplet->GetItemLevel() + pEquipItem_->GetIEchantedItemLevel() ) );
		} break;

	case CX2Item::IT_DEFENCE:
		{
			SetSumOfItemLevel( GetSumOfItemLevel() + ( pItemTemplet->GetItemLevel() + pEquipItem_->GetIEchantedItemLevel() ) );
		} break;

	default:
		break;
	}
}

void CX2Unit::UpdateItemLevel()
{
	SetSumOfItemLevel( 0 );	// �ʱ�ȭ

	for ( UINT i =0; i < m_NormalEqipItemUIDList.size(); i++ )
	{
		CX2Item* pEqipItem = GetInventory().GetItem( m_NormalEqipItemUIDList[i] );
		if( pEqipItem == NULL )
			continue;

		AddItemLevel( pEqipItem );
	}
}

int CX2Unit::GetAverageItemlevel() const
{
	return m_iSumOfItemLevel / CX2Item::AVERAGE_ITEM_LEVEL_DIVISOR;
}



//////////////////////////////////////////////////////////////////////////
// CX2Unit::UnitData

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KUnitInfo& data )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKUnitInfo( data );
}

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KRoomUserInfo& data )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKRoomUserInfo( data );
}

void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KSquareUserInfo& pKSquareUserInfo )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKSquareUserInfo( pKSquareUserInfo );

}


void    CX2Unit::UnitData::Init( CX2Unit* pOwnerUnit, const KFieldUserInfo& pKFieldUserInfo )
{
	//Init();

	//pOwnerUnit->SetUnitData( this );
	//m_pInventory = new CX2Inventory( pOwnerUnit );
    m_Inventory.SetOwnerUnit( pOwnerUnit );

	SetKFieldUserInfo( pKFieldUserInfo );

}


CX2Unit::UnitData::~UnitData()
{
	//SAFE_DELETE( m_pInventory );
}

void CX2Unit::UnitData::Init()
{
	m_UnitUID				= 0;
	m_UserUID				= 0;
	m_UnitClass				= UC_NONE;

	m_Port					= 0;

	m_ED					= 0;
	m_Level					= 0;
	m_EXP					= 0;


	m_iRating				= 0;
	m_iRPoint				= 0;
	m_iAPoint				= 0;

	m_cRank					= 0;
	m_iPastSeasonWin		= 0;
	m_fKFactor				= 0.f;

	m_Age					= 0;
	m_bMan					= true;

	m_Win					= 0;
	m_Lose					= 0;
	m_Seceder				= 0;

	m_NowBaseLevelEXP		= 0;
	m_NextBaseLevelEXP		= 0;

	//	m_InventorySize			= 0;


	//m_RemainStatPoint		= 0;
	m_iSPoint				= 0;
	//m_pInventory			= NULL;


	m_nStraightVictories	= 0;
	m_nMapID				= 0;

	m_bIsGameBang			= false;
	m_fAddExpRate			= 0.f;

	m_UserSkillTree.Reset( true, false, false, false );

	//{{ oasis907 : ����� //// 2009-11-17 ////
	m_UserSkillTree.Reset( true, true, false, false, false );
	//}} oasis907 : ����� //// 2009-11-17 ////
	m_bIsParty				= false;

	m_iSpirit				= 0;
	m_iSpiritMax			= 0;


//#ifdef TITLE_SYSTEM
    m_iTitleId              = 0;
//#endif

	//{{ ����� : [2009/9/25] //	��� �ʱ�ȭ
	m_wstrGuildName = L"";
	m_byMemberShipGrade	= 0;
	//}} ����� : [2009/9/25] //	��� �ʱ�ȭ

	//{{ 2012.02.20 ��ȿ��	ĳ���� ���� ���μ��� ���� (���� ��� �Ⱓ ����)
	m_wstrLastDate			= L"";
	m_bDeleted				= false;
	m_trDelAbleDate			= 0LL;
	m_trRestoreAbleDate		= 0LL;
	m_bDeletedMotionPlay	= false;// ���� ��� ��� ������ //2012.03.07 lygan_������ // ĳ���� ���� ��� �����
	//}}

	m_bInvisible			= false;

	m_cWeddingStatus = 0;
	m_iLoverUnitUID = 0;
	m_bIgnoreLastTouch = false;

	m_bCouple = false;

	m_ucOldYearMissionRewardedLevel = 0;
	m_iNewYearMissionStepID = -1;


}



void CX2Unit::UnitData::SetKUnitInfo( const KUnitInfo& data )
{
	m_UnitUID				= data.m_nUnitUID;
	m_UserUID				= data.m_iOwnerUserUID;
	m_UnitClass				= (UNIT_CLASS)data.m_cUnitClass;



	m_IP					= data.m_wstrIP;
	m_Port					= (int)data.m_usPort;

	m_NickName				= data.m_wstrNickName;

	m_ED					= data.m_iED;
	m_Level					= data.m_ucLevel;
	m_EXP					= data.m_iEXP;

	m_iRating				= data.m_iRating;
	m_iRPoint				= data.m_iRPoint;
	m_iAPoint				= data.m_iAPoint;

	m_cRank					= data.m_cRank;
	m_iPastSeasonWin		= data.m_iPastSeasonWin;
	m_fKFactor				= data.m_fKFactor;

	m_Win					= data.m_iWin;
	m_Lose					= data.m_iLose;
	//m_Seceder				= data.m_iSeceder;


	m_NowBaseLevelEXP		= data.m_nNowBaseLevelEXP;
	m_NextBaseLevelEXP		= data.m_nNextBaseLevelEXP;

//	m_InventorySize			= data.m_usInventorySize;
	//m_RemainStatPoint		= data.m_ucRemainStatPoint;
	m_iSPoint				= data.m_iSPoint;


	m_iCSPoint				= data.m_iCSPoint;		// ������ �ִ� ĳ�� ��ų ����Ʈ
	//{{ 09.05.28 �¿� : CSP �߸� ���ƿ��� �� ���� �ӽ� ����ó��.
	if(m_iCSPoint < 0 || m_iCSPoint > 500 )
	{
		ASSERT( !"Wrong CSP Point from server" );
		m_iCSPoint = 0;
	}
	//}}
	// oasis907 : ����� [2009.11.27] // ��� ��ų ����Ʈ ����, �̺κе� X2GuildManager�� EGS_GUILD_INFO_NOT���� ó��
	/*
	//{{ oasis907 : ����� //// 2009-11-14 //// �ӽ�
	m_iGuildSPoint			= 20;
	m_iGuildCSPoint			= 0;

	//}} oasis907 : ����� //// 2009-11-14 ////
	*/

	m_wstrCSPointEndDate	= data.m_wstrCSPointEndDate;



	m_nStraightVictories	= data.m_nStraightVictories;
	m_nMapID				= data.m_kLastPos.m_iMapID;
	m_ucLastTouchLineIndex	= data.m_kLastPos.m_ucLastTouchLineIndex;
	m_usLastPosValue		= data.m_kLastPos.m_usLastPosValue;
	m_bIgnoreLastTouch		= data.m_kLastPos.m_bIgnoreLastTouch;			// LastTouch�� ������� �ʰ� m_iMapID�� �ش��ϴ� �⺻ ��ġ�� ����ϵ��� �˸���.
	m_bIsGameBang			= data.m_bIsGameBang;



	m_UserSkillTree.SetGuildClass( 0 );
	m_UserSkillTree.SetUnitClass( m_UnitClass );
	m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) data.m_UnitSkillData.m_cSkillSlotBExpirationState );
	m_UserSkillTree.SetSkillSlotBEndDateString( data.m_UnitSkillData.m_wstrSkillSlotBEndDate );

	// �� ������ �ƴ϶�� ������ų�� �нú� ��ų ������ ���� ��ųƮ���� ����� ����
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != data.m_nUnitUID )
		{
			m_UserSkillTree.SetEquippedSkill( data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecPassiveSkill, data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructGuildSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecGuildPassiveSkill );
		}
	}

	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}


	m_NowEqipItemUIDList.resize(0);


	std::map< int, KInventoryItemInfo >::const_iterator iM;
	for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
	{
		KInventoryItemInfo kInventorySlotInfo = (*iM).second;
		m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
	}

	for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
	{
		KInventoryItemInfo kInventorySlotInfo = (*iM).second;
		if ( kInventorySlotInfo.m_iItemUID > 0 )
		{
			m_NowEqipItemUIDList.push_back( kInventorySlotInfo.m_iItemUID );
			m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
            {
			    CX2Item::ItemData kItemData( kInventorySlotInfo );
			    m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID, kItemData );
            }
		}
	}

	// ����� ��Ʈ���� ��ϵ� �޸���
	m_UserSkillTree.SetEqipSkillMemo( data.m_UnitSkillData.m_vecSkillNote );


	m_bIsParty				= data.m_bIsParty;

	//{{ 2007. 10. 5  ������  �ټ��� �߰�
	m_iSpirit				= data.m_iSpirit;
	m_iSpiritMax			= data.m_iSpiritMax;
	//}}


	m_Stat.SetKStat( data.m_kStat );
	m_GameStat.SetKStat( data.m_kGameStat );


//#ifdef TITLE_SYSTEM
	m_iTitleId              = data.m_iTitleID;
//#endif

	//{{ ����� : [2009/9/25] //	��� �̸� �߰�
	m_wstrGuildName			= data.m_kUserGuildInfo.m_wstrGuildName;
	m_byMemberShipGrade		= data.m_kUserGuildInfo.m_ucMemberShipGrade;
	m_iGuildHonorPoint		= data.m_kUserGuildInfo.m_iHonorPoint;
	//}} ����� : [2009/9/25] //	��� �̸� �߰�

	//SetPremiumBuffInfo( data.m_vecBuffInfo );		/// �� ���� �ý���

	//Room ����� �ɷ�ġ �ʱ�ȭ
	//SetPremiumBuffInfo( data.m_vecBuffInfo );
	ErasePremiumBuffInfo( m_vecPremiumBuffList );
	m_vecPremiumBuffList.resize(0);

	//{{ 2012.02.20 ��ȿ��	ĳ���� ���� ���μ��� ���� (���� ��� �Ⱓ ����)
	m_wstrLastDate			= data.m_wstrLastDate;			// ���� Logout Ÿ��
	m_bDeleted				= data.m_bDeleted;				// ���� ��� ���� ����
	m_trDelAbleDate		= data.m_trDelAbleDate;		// ���� ���� �Ͻ�
	m_trRestoreAbleDate	= data.m_trRestoreAbleDate;	// ���� ���� �Ͻ�
	//}}

	m_trWarpVipEndDate = data.m_trWarpVipEndData;	// �ں� VIP ������ �ð�


	m_ucOldYearMissionRewardedLevel = data.m_ucOldYearMissionRewardedLevel;
	m_iNewYearMissionStepID = data.m_iNewYearMissionStepID;

}

void CX2Unit::UnitData::SetKRoomUserInfo( const KRoomUserInfo& data )
{
	m_UnitUID				= data.m_nUnitUID;
	m_UserUID				= data.m_iOwnerUserUID;

	m_bMan					= data.m_bMale;
	m_Age					= data.m_ucAge;

	m_UnitClass				= (UNIT_CLASS)data.m_cUnitClass;


	// ��ũ����
	m_cRank					= data.m_cRank;


	m_IP					= data.m_wstrIP;
	m_Port					= (int)data.m_usPort;
	//{{ 2013. 1. 9	�ڼ���	Merge ����IP ���� ���н� ����IP�� �õ�( ������ )
//#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP
	m_InternalIP			= data.m_wstrInternalIP;
	m_InternalPort			= data.m_usInternalPort;
//#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
	//}}

	m_NickName				= data.m_wstrNickName;

	m_Level					= data.m_ucLevel;

	m_bIsGameBang			= data.m_bIsGameBang;
	//{{ 2010. 05. 13  ������	���� ���� ������ ����
	m_fAddExpRate			= data.GetBonusRate( KRoomUserInfo::BT_PREMIUM_EXP_RATE );
	//}}

	m_UserSkillTree.SetGuildClass( 0 );
	m_UserSkillTree.SetUnitClass( m_UnitClass );
	m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) data.m_UnitSkillData.m_cSkillSlotBExpirationState );
	m_UserSkillTree.SetSkillSlotBEndDateString( data.m_UnitSkillData.m_wstrSkillSlotBEndDate );


	// �� ������ �ƴ϶�� ������ų�� �нú� ��ų ������ ���� ��ųƮ���� ����� ����
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != data.m_nUnitUID )
		{
			m_UserSkillTree.SetEquippedSkill( data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecPassiveSkill, data.m_UnitSkillData.m_aEquippedSkill, data.m_UnitSkillData.m_aEquippedSkillSlotB );
			m_UserSkillTree.ConstructGuildSkillTreeNotMyUnit( data.m_UnitSkillData.m_vecGuildPassiveSkill );

			for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
			{
                m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
			}

			m_NowEqipItemUIDList.resize(0);
			{
				std::map< int, KInventoryItemInfo >::const_iterator iM;
				for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
				{
					KInventoryItemInfo kInventorySlotInfo = (*iM).second;
					m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
				}

				for ( iM = data.m_mapEquippedItem.begin(); iM != data.m_mapEquippedItem.end(); ++iM )
				{
					KInventoryItemInfo kInventorySlotInfo = (*iM).second;
					if ( kInventorySlotInfo.m_iItemUID > 0 )
					{
						m_NowEqipItemUIDList.push_back( kInventorySlotInfo.m_iItemUID );
						m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID );
                        {
                            CX2Item::ItemData kItemData( kInventorySlotInfo );
						    m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventorySlotInfo.m_cSlotCategory, kInventorySlotInfo.m_sSlotID, kItemData );
                        }
					}
				}
			}
		}
	}



	// ����� ��Ʈ���� ��ϵ� �޸���
	m_UserSkillTree.SetEqipSkillMemo( data.m_UnitSkillData.m_vecSkillNote );


	m_GameStat.SetKStat( data.m_kGameStat );

//#ifdef TITLE_SYSTEM
    m_iTitleId              = data.m_iTitleID;
//#endif

	//{{ ����� : [2009/9/25] //	��� �̸� �߰�
	m_wstrGuildName	= data.m_wstrGuildName;
	m_byMemberShipGrade = data.m_ucMemberShipGrade;
	//}} ����� : [2009/9/25] //	��� �̸� �߰�

	//{{ 2013. 04. 01	 �ο� �ý��� - ��μ�
	m_cWeddingStatus = data.m_cWeddingStatus;
	m_iLoverUnitUID = data.m_iLoverUnitUID;
	//}



	m_bCouple = data.m_bCouple;
	m_iRelationTargetUserUid = data.m_iRelationTargetUserUid;
	m_wstrRelationTargetUserNickname = data.m_wstrRelationTargetUserNickname;
}

void CX2Unit::UnitData::SetKSquareUserInfo( const KSquareUserInfo& pKSquareUserInfo )
{
	m_UnitUID				= pKSquareUserInfo.m_iUnitUID;
	m_UnitClass				= (UNIT_CLASS)pKSquareUserInfo.m_cUnitClass;

	m_IP					= pKSquareUserInfo.m_wstrIP;
	m_Port					= pKSquareUserInfo.m_usPort;

	m_NickName				= pKSquareUserInfo.m_wstrNickName;
	m_Level					= pKSquareUserInfo.m_ucLevel;

	m_bMan					= false; //pKSquareUserInfo.m_bMale;
	m_Age					= 0; //pKSquareUserInfo.m_ucAge;


	// ��ũ����
	m_cRank					= pKSquareUserInfo.m_cRank;

	m_bIsParty				= pKSquareUserInfo.m_bIsParty;

	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}

	m_NowEqipItemUIDList.resize(0);


	{
		for( int i = 0; i < (int)pKSquareUserInfo.m_vecEquippedItem.size(); i++ )
		{
			KInventoryItemSimpleInfo kInventoryItemSimpleInfo = pKSquareUserInfo.m_vecEquippedItem[i];

			if ( kInventoryItemSimpleInfo.m_iItemUID > 0 )
			{
				m_NowEqipItemUIDList.push_back( kInventoryItemSimpleInfo.m_iItemUID );
				CX2Item::ItemData kItemData;
				kItemData.m_ItemUID			= kInventoryItemSimpleInfo.m_iItemUID;
				kItemData.m_ItemID				= kInventoryItemSimpleInfo.m_iItemID;
				kItemData.m_EnchantLevel		= kInventoryItemSimpleInfo.m_EnchantLevel;

				kItemData.m_EnchantedAttribute.m_aEnchantedType[0]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant0;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[1]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant1;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[2]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant2;

				m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID );
				m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID, kItemData );
			}
		}
	}

//#ifdef TITLE_SYSTEM
	m_iTitleId              = pKSquareUserInfo.m_iTitleID;
//#endif

	//{{ ����� : [2009/9/25] //	��� �̸� �߰�
	m_wstrGuildName	= pKSquareUserInfo.m_wstrGuildName;
	//}} ����� : [2009/9/25] //	��� �̸� �߰�

}


void CX2Unit::UnitData::SetKFieldUserInfo( const KFieldUserInfo& pKFieldUserInfo )
{
	m_UnitUID				= pKFieldUserInfo.m_iUnitUID;
	m_UnitClass				= (UNIT_CLASS)pKFieldUserInfo.m_cUnitClass;

	m_NickName				= pKFieldUserInfo.m_wstrNickName;
	m_Level					= pKFieldUserInfo.m_ucLevel;


	// ��ũ����
	m_cRank					= pKFieldUserInfo.m_cRank;

	// �ʿ���� ����
	m_IP					= L"";
	m_Port					= 0;
	m_bMan					= false;
	m_Age					= 0;
	m_bIsParty				= false;


	for ( int i = 0; i < (int)m_NowEqipItemUIDList.size(); i++ )
	{
        m_Inventory.RemoveItem( m_NowEqipItemUIDList[i] );
	}

	m_NowEqipItemUIDList.resize(0);


	{
		for( int i = 0; i < (int)pKFieldUserInfo.m_vecEquippedItem.size(); i++ )
		{
			KInventoryItemSimpleInfo kInventoryItemSimpleInfo = pKFieldUserInfo.m_vecEquippedItem[i];

			if ( kInventoryItemSimpleInfo.m_iItemUID > 0 )
			{
				m_NowEqipItemUIDList.push_back( kInventoryItemSimpleInfo.m_iItemUID );
				CX2Item::ItemData kItemData;
				kItemData.m_ItemUID			= kInventoryItemSimpleInfo.m_iItemUID;
				kItemData.m_ItemID				= kInventoryItemSimpleInfo.m_iItemID;
				kItemData.m_EnchantLevel		= kInventoryItemSimpleInfo.m_EnchantLevel;


				kItemData.m_EnchantedAttribute.m_aEnchantedType[0]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant0;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[1]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant1;
				kItemData.m_EnchantedAttribute.m_aEnchantedType[2]	= (CX2EnchantItem::ENCHANT_TYPE) kInventoryItemSimpleInfo.m_kAttribEnchantInfo.m_cAttribEnchant2;


				m_Inventory.RemoveItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID );
				m_Inventory.AddItem( (CX2Inventory::SORT_TYPE)kInventoryItemSimpleInfo.m_cSlotCategory, kInventoryItemSimpleInfo.m_sSlotID, kItemData );
			}
		}
	}

//#ifdef TITLE_SYSTEM
	m_iTitleId              = pKFieldUserInfo.m_iTitleID;
//#endif

	//{{ ����� : [2009/9/25] //	��� �̸� �߰�
	m_wstrGuildName	= pKFieldUserInfo.m_wstrGuildName;
	//}} ����� : [2009/9/25] //	��� �̸� �߰�

	m_bInvisible = pKFieldUserInfo.m_bInvisible;

}


void CX2Unit::UnitData::Verify() const
{
	if( m_ED.Verify()							== false
		|| m_iRating.Verify()					== false
		|| m_iRPoint.Verify()					== false
		|| m_iAPoint.Verify()					== false
		|| m_Level.Verify()						== false
		|| m_EXP.Verify()						== false
		|| m_NowBaseLevelEXP.Verify()			== false
		|| m_NextBaseLevelEXP.Verify()			== false
		|| m_iSPoint.Verify()					== false
		|| m_iCSPoint.Verify()					== false
		|| m_nStraightVictories.Verify()		== false
		|| m_nMapID.Verify()					== false )
	{
		g_pKTDXApp->SetFindHacking( true );
	}

	m_Stat.Verify();
	m_GameStat.Verify();
}

void CX2Unit::UnitData::SetBlackList( vector<KChatBlackListUnit>& blackList )
{
	m_BlackList = blackList;
}

bool CX2Unit::UnitData::AddBlackList( KChatBlackListUnit blackList )
{
	for( int i = 0; i < (int)m_BlackList.size(); i++ )
	{
		KChatBlackListUnit kBlackListUnit = m_BlackList[i];
		if( kBlackListUnit.m_iUnitUID == blackList.m_iUnitUID )
			return false;
	}
	m_BlackList.push_back( blackList );
	return true;
}

bool CX2Unit::UnitData::RemoveBlackList( UidType unitUID )
{
	for( int i = 0; i < (int)m_BlackList.size(); i++ )
	{
		KChatBlackListUnit kBlackListUnit = m_BlackList[i];
		if( kBlackListUnit.m_iUnitUID == unitUID )
		{
			m_BlackList.erase( m_BlackList.begin() + i );
			return true;
		}
	}
	return false;
}

UidType CX2Unit::UnitData::GetBlackListUnitUID( const WCHAR* wszNickName ) const
{
	UidType unitUID = -1;
	for( UINT i=0; i<m_BlackList.size(); i++ )
	{
		if( 0 == m_BlackList[i].m_wstrNickName.compare( wszNickName ) )
		{
			unitUID = m_BlackList[i].m_iUnitUID;
			break;
		}
	}

	return unitUID;
}

void CX2Unit::UnitData::SetPremiumBuffInfo( const vector<KBuffInfo>& /*vecBuffInfo_*/ )
{
	/*for ( const KBuffInfo& kBuffInfo : vecBuffInfo_ )		/// �� ���� �ý���
	{
		m_vecPremiumBuffList.push_back( kBuffInfo.m_iBuffID );
	}*/
}

void CX2Unit::UnitData::ErasePremiumBuffInfo( const vector<int>& /*vecBuffInfo_*/ )
{
	/*for ( const int& iBuffID : vecBuffInfo_ )				/// �� ���� �ý���
	{
		std::vector< int >::iterator vit = m_vecPremiumBuffList.begin();

		while( m_vecPremiumBuffList.end() != vit )
		{
			if( iBuffID  == *vit )
			{
				vit = m_vecPremiumBuffList.erase( vit );
			}
			else
			{
				++vit;
			}
		}
	}*/
}

void CX2Unit::UnitData::ClearPremiumBuffInfo()
{
	m_vecPremiumBuffList.clear();
}


/** @function : IsShowDungeonOnly
	@brief : �ش� �ɼ��� ���� ��ϵǾ�� �ϴ� �ɼ����� ���� Ȯ��
	@param : bDungeonOnlyItem_( pSocketData�� m_bDungeonOnly����)
			 bShowDungeonOptionUI_ ( UI�� ����,
									true ��� ���� ��ġ�� ������ �ƴϴ��� true)
*/
bool CX2Unit::IsShowDungeonOnly( bool bDungeonOnlyItem_, bool bShowDungeonOptionUI_ /*= false */)
{
	//UI�� �ɼ�, ������ ������ ������ �����ߴٸ� ������ �����ֱ�
	if( true == bShowDungeonOptionUI_ )
		return true;

	//�����¸��� �ƴ϶�� ������ true
	if( false == bDungeonOnlyItem_ )
		return true;

	//���������� ������� �Ѵٸ�, ������ ���� �Ǵ� �ʵ����� Ȯ��
	if( NULL == g_pX2Game )
		return true;

	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_DUNGEON:
	case CX2Game::GT_BATTLE_FIELD:
		return true;
		break;
	default:
		//�����¸� �ɼ��ε�, ���� �Ǵ� �ʵ尡 �ƴ϶�� �Ⱥ�����
		return false;
		break;
	}
}
/** @function : IsShowPvpOnly
	@brief : �ش� �ɼ��� ���� ��ϵǾ�� �ϴ� �ɼ����� ���� Ȯ��
	@param : bDungeonOnlyItem_( pSocketData�� m_bPVPOnly����)
			bShowPvpOptionUI_ ( UI�� ����,
									true ��� ���� ��ġ�� ������ �ƴϴ��� true)
*/
bool CX2Unit::IsShowPvpOnly( bool bPvpOnlyItem_, bool bShowPvpOptionUI_ /*= false*/ )
{
	//UI�� �ɼ�, ������ ������ ������ �����ߴٸ� ������ �����ֱ�
	if( true == bShowPvpOptionUI_ )
		return true;

	//�����¸��� �ƴ϶�� ������ true
	if( false == bPvpOnlyItem_ )
		return true;

	//���������� ������� �Ѵٸ�, ������ �������� Ȯ��
	if( NULL == g_pX2Game )
		return false;

	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_PVP:
		return true;
		break;
	default:
		//�����¸� �ɼ��ε�, ������ �ƴ϶�� �Ⱥ�����
		return false;
		break;
	}
}

/** @function 	:
	@brief 		:
*/
void CX2Unit::SetFullRidingPetInfo( const KRidingPetInfo& pRidingPetInfo_ )
{
	m_vecRidingPetInfo.clear();
	m_vecRidingPetInfo.push_back( pRidingPetInfo_ );
}

/** @function 	:
	@brief 		:
*/
KRidingPetInfo* CX2Unit::GetRidingPetInfo()
{
	if( m_vecRidingPetInfo.size() > 0 )
	{
		return &m_vecRidingPetInfo[0];
	}

	return NULL;
}



bool CX2Unit::IsManCharacter ()
{
	UNIT_CLASS ucMyClass = GetUnitData().m_UnitClass;

	if (
		ucMyClass == UC_ELSWORD_SWORDMAN ||
		ucMyClass == UC_RAVEN_FIGHTER ||
		ucMyClass == UC_CHUNG_IRON_CANNON ||
		ucMyClass == UC_ELSWORD_KNIGHT ||
		ucMyClass == UC_ELSWORD_MAGIC_KNIGHT ||
		ucMyClass == UC_RAVEN_OVER_TAKER ||
		ucMyClass == UC_RAVEN_SOUL_TAKER ||
		ucMyClass == UC_CHUNG_FURY_GUARDIAN ||
		ucMyClass == UC_CHUNG_SHOOTING_GUARDIAN ||
		ucMyClass == UC_ELSWORD_SHEATH_KNIGHT ||
		ucMyClass == UC_RAVEN_WEAPON_TAKER ||
		ucMyClass == UC_CHUNG_SHELLING_GUARDIAN ||
		ucMyClass == UC_ELSWORD_LORD_KNIGHT ||
		ucMyClass == UC_ELSWORD_RUNE_SLAYER ||
		ucMyClass == UC_RAVEN_BLADE_MASTER ||
		ucMyClass == UC_RAVEN_RECKLESS_FIST ||
		ucMyClass == UC_CHUNG_IRON_PALADIN ||
		ucMyClass == UC_CHUNG_DEADLY_CHASER ||
		ucMyClass == UC_ELSWORD_INFINITY_SWORD ||
		ucMyClass == UC_RAVEN_VETERAN_COMMANDER ||
		ucMyClass == UC_CHUNG_TACTICAL_TROOPER
		)
		return true;		// ���� ĳ���͸� ��

	return false;			// ���� ĳ���͸� ����
}



/** @function	: ResetIncreaseSkillLevelByBuff
	@brief		: ������ ���� ��ų ���� ���� ȿ�� �缳�� �Լ�
*/
void CX2Unit::ResetIncreaseSkillLevelByBuff()
{
	/// ��ų �������� ���� ��ų ����� ���ٸ�, ���� ����
	if ( false == m_UnitData.m_UserSkillTree.IsEnableSkillAcquiredPage() )
		return;

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>& mapSkillAcquired = m_UnitData.m_UserSkillTree.AccessMapSkillAcquired();

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>::iterator mit = mapSkillAcquired.begin();

	/// ������ ��ų���� ������ �ø��� ȿ���� ���� ������, �������� ��ų ��ο��� ����
	for ( ; mit != mapSkillAcquired.end(); ++mit )
	{
		const int iIncreaseSkillLevelByBuff = GetIncreaseSkillLevelByBuff();	/// ��� ��ų�� ��ų ���� ������ ��ȯ

		mit->second.SetIncreaseSkillLevelByBuff( iIncreaseSkillLevelByBuff );
	}

	if ( NULL != g_pData &&
		NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );	/// ��� ��ų ������Ʈ
	}
}

/** @function	: GetIncreaseSkillLevelByBuff
	@brief		: ������ ���� ��ų ���� ���� ȿ���� ��ȯ �Լ�
	@return		: ������
*/
int CX2Unit::GetIncreaseSkillLevelByBuff()
{
	int iIncreaseSkillLevelByBuff = 0;

	/// ���� ��ü�� �ִٸ�, ���ֿ��� ����� ������ ȿ�� ����
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
	{
		iIncreaseSkillLevelByBuff += g_pX2Game->GetMyUnit()->GetAddSkillLevelByBuff();
	}
	/// ���� ��ü�� ���ٸ�, ���� �������� �˻� ( ���� ����� ���� ������ �����Ǿ� �ִ�.( ȿ�� ���� �����ܸ� ǥ�� ) )
	else
	{
		/// ���� ���� ���� �����̳�
		std::vector<BUFF_FACTOR_ID> vecWorldBuffID = g_pData->GetMyUser()->GetSelectUnit()->GetWorldBuffFactorID();

		for( BUFF_FACTOR_ID eAddSkillLevelByBuffID : vecWorldBuffID )
		{
			CX2BuffFactorPtr ptrBuffFactor = CX2BuffTempletManager::GetInstance()->GetBuffFactorPtr( eAddSkillLevelByBuffID );

			if ( NULL != ptrBuffFactor )
			{
				const KBuffBehaviorFactor* pBehaviorFactor = NULL;

				ptrBuffFactor->GetBehaviorFactor( BBT_ADD_SKILL_LEVEL, &pBehaviorFactor );		/// ��ų ���� ���� ȿ�� �˻�

				if ( NULL != pBehaviorFactor )
					iIncreaseSkillLevelByBuff += static_cast<int>( pBehaviorFactor->m_vecValues[0] );	/// ������ ����ġ ����
			}
		}
	}

	return iIncreaseSkillLevelByBuff;
}

/** @function	: ResetIncreaseSkillLevelBySocket
	@brief		: �������� ���� ��ų ���� ���� ȿ�� �缳�� �Լ�
*/
void CX2Unit::ResetIncreaseSkillLevelBySocket()
{
	/// ��ų �������� ���� ��ų ����� ���ٸ�, ���� ����
	if ( false == m_UnitData.m_UserSkillTree.IsEnableSkillAcquiredPage() )
		return;

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>& mapSkillAcquired = m_UnitData.m_UserSkillTree.AccessMapSkillAcquired();

	std::map<CX2SkillTree::SKILL_ID, CX2UserSkillTree::UserSkillData>::iterator mit = mapSkillAcquired.begin();

	for( ; mit != mapSkillAcquired.end(); ++mit )
	{
		const CX2SkillTree::SKILL_ID eSkillID = mit->first;		/// ��ų ���̵�

		const int iIncreaseSkillLevel = GetIncreaseSkillLevelBySocket( eSkillID );		///	�������� ���� �߰� ������ ����

		mit->second.SetIncreaseSkillLevelBySocket( iIncreaseSkillLevel );	/// ��ų ���� ����
	}

	if ( NULL != g_pData->GetUIManager() &&
		 NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );	/// ��� ��ų ������Ʈ
	}
}

/** @function	: GetIncreaseSkillLevelBySocket
	@brief		: �������� ���� ��ų ���� ���� ȿ���� ��ȯ �Լ�
	@param		: ��ų ���̵�
	@return		: ������
*/
int CX2Unit::GetIncreaseSkillLevelBySocket( CX2SkillTree::SKILL_ID eSkillID )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetSocketItem() ||
		NULL == g_pData->GetTitleManager() )
		return 0;

	int iIncreaseSkillLevelBySocket = 0;
	/// ���Ͽ� ���� ���� ���� ȿ���� �ִ��� �˻�

	for ( const UidType& eqipUID : m_UnitData.m_NowEqipItemUIDList )		/// ������� �������� ��� �˻�
	{
		CX2Item* pItem = m_UnitData.m_Inventory.GetItem( eqipUID );		/// ������ UID�� ���� ������ ��ü ��ȯ
		if( NULL == pItem )
			continue;

		const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();	/// ������ ���ø� ��ȯ
		if( NULL == pItemTemplet )
			continue;

		/// �ش� �������� ���� �ɼ� �˻�
		unsigned uNumSockOption = pItemTemplet->GetNumSocketOption();
		for( UINT j=0; j<uNumSockOption; j++ )
		{
			int socketOptionID = pItemTemplet->GetSocketOption(j);
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			/// ��ų ���� ���� ȿ���� ���� ��ų ���� ����ġ ����
			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}

		/// �����ۿ� ������ ���� ���Ͽ� ���� ���� �ɼ� �˻�
		for( UINT j=0; j < pItem->GetItemData().m_SocketOption.size(); j++ )
		{
			int socketOptionID = pItem->GetItemData().m_SocketOption[j];
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}

		/// ������ ���� ��� ���� ���� �ɼ� �˻�
		for( int iSocketID : pItem->GetItemData().m_vecRandomSocket )
		{
			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}

	}

	/// Īȣ�� ���� ���� ���� ȿ���� �ִ��� �˻�
	const int iTitleID = m_UnitData.m_iTitleId;		/// Īȣ ���̵�

	if( NULL != g_pData->GetTitleManager()->GetTitleInfo( iTitleID ) )
	{
		const vector<int>& vecTitleSocketOption = g_pData->GetTitleManager()->GetTitleInfo( iTitleID )->m_vecSocketOption;

		for( int socketOptionID : vecTitleSocketOption )
		{
			const CX2SocketItem::SocketData*	pSocketData		= g_pData->GetSocketItem()->GetSocketData( socketOptionID );
			if( NULL == pSocketData )
				continue;

			g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
		}
	}

	/// {{ ��Ʈ ȿ���� ���� ��Ʈ �ɼ� �˻�
	map<int,int> mapSetIDNParts;

	g_pData->GetMyUser()->GetSelectUnit()->GetSetIDNPartsNum( mapSetIDNParts );

	if ( false == mapSetIDNParts.empty() )
	{
		map< int, int >::const_iterator mItr = mapSetIDNParts.begin();

		while ( mItr != mapSetIDNParts.end() )
		{
			vector<int> vecOptions;

			// SetID ���� Option��� SetItem�� �ִ� ���� ������ ����
			g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );

			// Option�� ������, �˻� ����
			if ( false == vecOptions.empty() )
			{
				/// �ɼ��� ��ų ���� � ȿ���� ������, ����
				for( int iSocketID : vecOptions )
				{
					const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( iSocketID );
					if( NULL != pSocketData )
						g_pData->GetSocketItem()->GetSkillLevelIncreaseValue( pSocketData, eSkillID, iIncreaseSkillLevelBySocket, m_UnitData.m_UnitClass );
				}
			}

			++mItr;		/// iterator ����
		}
	}

	return iIncreaseSkillLevelBySocket;
}



wstring CX2Unit::GetLocalMapPlayTimesDesc( int iDungeonID )
{
	std::map< int, KDungeonPlayInfo >::iterator mitDungeonPlay = m_mapDungeonPlay.find( iDungeonID );

	// �÷����� Ƚ�� ȹ��
	int iUserPlayTimes = 0;
	int iUserClearTimes = 0;
	if( mitDungeonPlay != m_mapDungeonPlay.end() )
	{
		iUserPlayTimes = mitDungeonPlay->second.m_iPlayTimes;
		iUserClearTimes = mitDungeonPlay->second.m_iClearTimes;
	}

	// �÷��� ������ Ƚ�� ȹ��
	int iLimitedPlayTimes = 0;
	int iLimitedClearTimes = 0;
	bool bLimitedPlayTimes = g_pData->GetDungeonManager()->GetLimitedPlayTimes( iDungeonID, iLimitedPlayTimes );
	bool bLimitedClearTimes = g_pData->GetDungeonManager()->GetLimitedClearTimes( iDungeonID, iLimitedClearTimes );

	// ��Ʈ�� �����
	wstring wstrLimitedPlayTimes;
	if( bLimitedPlayTimes && bLimitedClearTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28236, "ii", iLimitedPlayTimes - iUserPlayTimes, bLimitedClearTimes - iUserClearTimes ) ) + std::wstring( L"\n" );
	}
	else if( bLimitedPlayTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28237, "i", iLimitedPlayTimes - iUserPlayTimes ) ) + std::wstring( L"\n" );
	}
	else if( bLimitedClearTimes )
	{
		wstrLimitedPlayTimes += GET_REPLACED_STRING( ( STR_ID_28238, "i", iLimitedClearTimes - iUserClearTimes ) ) + std::wstring( L"\n" );
	}

	return wstrLimitedPlayTimes;
}

bool CX2Unit::IsInfinityElEssence() const
{
	if( NULL != GetInventory().GetItemByTID( INFINITY_EL_ESSENCE_ITEM_EVENT, false, false ) )
	{
		if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
			&& NULL != g_pData->GetPartyManager()
			&& NULL != g_pData->GetPartyManager()->GetMyPartyData())
		{
			if( true == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
				|| true == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
				return false;
		}

		return true;
	}

	return false;
}

float CX2Unit::GetSkillCoolTimeDecreaseRate(CX2SkillTree::SKILL_ID eSkillID, CX2SkillTree::SKILL_TYPE eSkillType) const
{
	float fDecreaseRate = 1.0f;

	if( CX2SkillTree::ST_HYPER_ACTIVE_SKILL == eSkillType )
	{
		if( NULL != GetInventory().GetItemByTID( FINALITY_SKILL_COOL_TIME_LEFT_ITEM_EVENT, false, false ) )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
				&& NULL != g_pData->GetPartyManager()
				&& NULL != g_pData->GetPartyManager()->GetMyPartyData())
			{
				if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
					&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
					fDecreaseRate = 0.7f;
			}
			else
				fDecreaseRate = 0.7f;
		}

		return fDecreaseRate;
	}

	switch(eSkillID)
	{
	case 202:
	case 204:
	case 1000:
	case 1002:
	case 1004:
	case 1007:
	case 1015:
	case 1200:
	case 1209:
	case 1210:
	case 1217:
	case 1222:
	case 1224:
	case 1402:
	case 1406:
	case 1408:
	case 2000:
	case 2007:
	case 2013:
	case 2014:
	case 2016:
	case 2017:
	case 2020:
	case 2021:
	case 2201:
	case 2203:
	case 2206:
	case 2212:
	case 2214:
	case 2217:
	case 2218:
	case 2219:
	case 2223:
	case 2415:
	case 3000:
	case 3001:
	case 3003:
	case 3005:
	case 3008:
	case 3011:
	case 3016:
	case 3017:
	case 3018:
	case 3201:
	case 3206:
	case 3207:
	case 3210:
	case 3218:
	case 3219:
	case 3220:
	case 3222:
	case 3224:
	case 3226:
	case 3227:
	case 3229:
	case 3400:
	case 3404:
	case 3410:
	case 3412:
	case 3413:
	case 4000:
	case 4002:
	case 4005:
	case 4009:
	case 4010:
	case 4013:
	case 4016:
	case 4017:
	case 4019:
	case 4021:
	case 4022:
	case 4200:
	case 4203:
	case 4213:
	case 4214:
	case 4215:
	case 4216:
	case 4218:
	case 4411:
	case 4412:
	case 4414:
	case 4417:
	case 5005:
	case 5008:
	case 5009:
	case 5011:
	case 5013:
	case 5015:
	case 5017:
	case 5024:
	case 5200:
	case 5202:
	case 5206:
	case 5214:
	case 5217:
	case 5407:
	case 6000:
	case 6001:
	case 6002:
	case 6006:
	case 6009:
	case 6010:
	case 6016:
	case 6020:
	case 6021:
	case 6200:
	case 6201:
	case 6206:
	case 6210:
	case 6213:
	case 6214:
	case 6218:
	case 6219:
	case 6222:
	case 6224:
	case 6225:
	case 6410:
	case 6413:
	case 7001:
	case 7006:
	case 7008:
	case 7010:
	case 7011:
	case 7012:
	case 7014:
	case 7200:
	case 7202:
	case 7203:
	case 7204:
	case 7206:
	case 7207:
	case 7212:
	case 7214:
	case 7216:
	case 7219:
	case 7408:
	case 7410:
	case 8003:
	case 8004:
	case 8006:
	case 8011:
	case 8016:
	case 8019:
	case 8202:
	case 8204:
	case 8205:
	case 8206:
	case 8209:
	case 8210:
	case 8213:
	case 8217:
	case 8218:
	case 8401:
	case 8402:
	case 8403:
	case 8404:
	case 8405:
	case 8406:
	case 8407:
	case 8408:
	case 8409:
	case 8600:
	case 8601:
		{
			if( NULL != GetInventory().GetItemByTID( BALANCE_SKILL_COOL_TIME_LEFT_ITEM_EVENT, false, false ) )
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
					&& NULL != g_pData->GetPartyManager()
					&& NULL != g_pData->GetPartyManager()->GetMyPartyData())
				{
					if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
						&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
						fDecreaseRate = 0.7f;
				}
				//else
				//	fDecreaseRate = 0.7f;
			}

			return fDecreaseRate;
		}
		break;
	default:
		break;
	}

	return fDecreaseRate;
}

float CX2Unit::GetSkillMpDecreaseRate(CX2SkillTree::SKILL_ID /*eSkillID*/, CX2SkillTree::SKILL_TYPE eSkillType) const
{
	float fDecreaseRate = 1.0f;

	if( CX2SkillTree::ST_HYPER_ACTIVE_SKILL == eSkillType )
	{
		if( NULL != GetInventory().GetItemByTID( FINALITY_SKILL_COOL_ACTIVATE_ITEM_EVENT, false, false ) )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME
				&& NULL != g_pData->GetPartyManager()
				&& NULL != g_pData->GetPartyManager()->GetMyPartyData())
			{
				if( false == CX2Dungeon::IsHenirDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ))
					&& false == CX2Dungeon::IsSecretDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ))
					fDecreaseRate = 0.8f;
			}
			else
				fDecreaseRate = 0.8f;
		}
	}

	return fDecreaseRate;
}
