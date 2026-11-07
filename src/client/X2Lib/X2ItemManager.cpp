#include "stdafx.h"
#include "./X2ItemManager.h"
#include "./X2LocationManager.h"

CX2ItemManager::CX2ItemManager(void)
{
	//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
	m_iLastPackacgeID = 0;
	//}} 2013.01.26 최민철 큐브패키지아이템 미리보기기능

}

CX2ItemManager::~CX2ItemManager(void)
{

    CX2Item::KItemFormatTemplet::_ResetData();
    m_vecItemTempletInfo.resize( 0 );




	map<int, ItemIDList*>::iterator it;
	for( it=m_ShopItemList.begin(); it!=m_ShopItemList.end(); it++ )
	{
		ItemIDList* pItemIDList = it->second;
		SAFE_DELETE( pItemIDList );
	}

	for( it=m_ShopItemGroup.begin(); it!=m_ShopItemGroup.end(); it++ )
	{
		ItemIDList* pItemIDList = it->second;
		SAFE_DELETE( pItemIDList );
	}

	map< std::pair<int,int>, NpcItemIDList*>::iterator nit;
	for( nit=m_ShopNpcItemList.begin(); nit!=m_ShopNpcItemList.end(); nit++ )
	{
		NpcItemIDList* pItemIDList = nit->second;
		SAFE_DELETE( pItemIDList );
	}

	//{{ kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
	std::multimap<int, RandomItemData* >::iterator mit;
	mit = m_multimapRandomItem.begin();
	for( mit = m_multimapRandomItem.begin(); mit != m_multimapRandomItem.end(); mit++ )
	{
		SAFE_DELETE( mit->second );
	}
	m_multimapRandomItem.clear();
	//}} kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가

	std::map< int, ManufactureData* >::iterator mit2;

	mit2 = m_mapManufacture.begin();
	for( mit2 = m_mapManufacture.begin(); mit2 != m_mapManufacture.end(); mit2++ )
	{
		SAFE_DELETE( mit2->second );
	}
	m_mapManufacture.clear();

	ClearCashItemList();

}

void CX2ItemManager::ClearItemExchangeData()
{
	m_mapItemExchangeData.clear();
}
void CX2ItemManager::ClearManufactureData()
{
	std::map< int, ManufactureData* >::iterator mit2;

	mit2 = m_mapManufacture.begin();
	for( mit2 = m_mapManufacture.begin(); mit2 != m_mapManufacture.end(); mit2++ )
	{
		SAFE_DELETE( mit2->second );
	}
	m_mapManufacture.clear();
}
void CX2ItemManager::ClearManufactureVillageData()
{
	m_mapManufactureVillage.clear();
}
void CX2ItemManager::ClearManufactureResultGroupData()
{
	m_mapManufactureResultGroupData.clear();
}

//{{ kimhc // 2010.12.9 // 2010-12-23 New Character CHUNG
/** @function : CreateDummyEquip
	@brief : 전달되는 정보에 따라 장비를 생성하여 리턴함(UID는 Invalid 함)
	@param : int iItemID_, CX2Unit* pUnit_, CKTDGXSkinAnimPtr pXSkinAnim_
	@return : CX2EqipPtr
*/

/*static*/ CX2EqipPtr CX2ItemManager::CreateDummyEquip( int iItemID_, CX2Unit* pUnit_, CKTDGXSkinAnimPtr pXSkinAnim_ )
{
    CX2Item* pX2Item = NULL;
	ASSERT( NULL != pUnit_ );

    {
	    CX2Item::ItemData kItemData;
	    kItemData.m_ItemID				= iItemID_;
	    kItemData.m_PeriodType			= CX2Item::PT_INFINITY;
	    kItemData.m_Endurance			= 0;
	    pX2Item				= new CX2Item( kItemData, pUnit_ );
        ASSERT( pX2Item != NULL );
    }
    int iEnchantLevel = pX2Item->GetItemData().m_EnchantLevel;
	if( pX2Item->GetItemTemplet()->GetUseCondition() == CX2Item::UC_ANYONE )
	{
		return CX2Eqip::CreateEquip( pX2Item, pXSkinAnim_, iEnchantLevel, true, true, pUnit_->GetType() );
	}
	else
	{
		return CX2Eqip::CreateEquip( pX2Item, pXSkinAnim_, iEnchantLevel, true, true );
	}
}

//}} kimhc // 2010.12.9 //  2010-12-23 New Character CHUNG

bool CX2ItemManager::OpenScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pItemManager", this );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pManufactureItemManager", this );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCashItemManager", this );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2SetItemManager", this );
	//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2CubePackageManager", this );
	//}}
    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR9, pFileName );
		return false;
    }

	return true;
}


bool    CX2ItemManager::DoMemoryNotEncrypt( const char* pBuffer, long nSize )
{
	if( pBuffer != NULL && nSize >= sizeof(CX2Item::KItemFormatHeader) )
    {
        CX2Item::KItemFormatHeader kHeader = *( (const CX2Item::KItemFormatHeader*) ( pBuffer ) );
        if ( kHeader.m_dwMagic == KX2ITEMMANAGER_FORMAT_MAGIC
            && kHeader.m_dwVersion == KX2ITEMMANAGER_FORMAT_VERSION )
        {
            DWORD dwNumItem = kHeader.m_dwNumItems;
            if ( nSize >= (int) ( sizeof(CX2Item::KItemFormatHeader) + dwNumItem * ( sizeof(DWORD) + sizeof(CX2Item::KItemFormatTemplet) ) ) )
            {
                m_vecItemTempletInfo.resize( 0 );
                const BYTE* pData = (const BYTE*) pBuffer;
                m_vecItemTempletInfo.assign( &pData[0], &pData[nSize] );
                CX2Item::KItemFormatTemplet::_SetData( &m_vecItemTempletInfo.front() );
                return true;
            }
        }
    }
    return false;
}

bool	CX2ItemManager::OpenItemScriptFile( const WCHAR* pKimFile, unsigned uNumScriptFiles, const WCHAR* apScriptFileName[] )
{
    bool bExistRealScript = false;

    if ( bExistRealScript == false )
    {
        bool bReturn = g_pKTDXApp->LoadAndDoMemory( this, pKimFile );
			if(bReturn == false) bReturn = g_pKTDXApp->LoadAndDoMemory(this, pKimFile, false);
        if ( bReturn == true )
            return true;
    }

    if ( uNumScriptFiles == 0 || apScriptFileName == NULL )
        return false;

	{
		/* 오현빈 // X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING주석 // 제가 이해한 내용을 바탕으로 작성 해서 틀린 내용 일 수 있습니다.
		KProxy로 파싱하는 첫 번째 작업은 아이템의 개수 파악을 위한 파싱
		해당 아이템 ID와, 해당 아이템이 유효한지 체크하기 위한 값들을 검사 후 ItemID만 컨테이너에 등록*/

	    KProxy	kProxy;
	    lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pItemManager", &kProxy );
	    lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2SetItemManager", &kProxy );

        for( unsigned u = 0; u != uNumScriptFiles; ++u )
        {
            bool bRet = g_pKTDXApp->LoadLuaTinker( apScriptFileName[u] );
	        if( bRet == false )
	        {
		        ErrorLogMsg( XEM_ERROR9, apScriptFileName[u] );
                return false;
	        }
        }

        {
			/* 오현빈 // X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING주석 // 제가 이해한 내용을 바탕으로 작성 해서 틀린 내용 일 수 있습니다.
			KFileSerializer 클래스에 스크립트의 정보를 기록함
			순서대로 아이템개수와 세트아이템의 개수를 저장하는 CX2Item::KItemFormatHeader
			sizeof(DWORD) * 아이템개수,  sizeof(CX2Item::KItemFormatTemplet)* 아이템개수,
			sizeof(DWORD) * 세트아이템개수, sizeof(CX2Item::KItemFormatSetItemData)* 세트아이템개수 */
            KProxy2 kProxy2( kProxy.m_setItemIDs, kProxy.m_setSetIDs );

	        lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pItemManager", &kProxy2 );
	        lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2SetItemManager", &kProxy2 );

			/* 오현빈 // X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING주석 // 제가 이해한 내용을 바탕으로 작성 해서 틀린 내용 일 수 있습니다.
			스크립트에 작성된 자료들을 실제로 등록하는 부분 */
            for( unsigned u = 0; u != uNumScriptFiles; ++u )
            {
                bool bRet = g_pKTDXApp->LoadLuaTinker( apScriptFileName[u] );
                if ( bRet == false )
	            {
		            ErrorLogMsg( XEM_ERROR9, apScriptFileName[u] );
                    return false;
	            }
            }

            kProxy2.PostProcess();

            if ( kProxy2.m_kFileSerializer.GetSize() < sizeof(CX2Item::KItemFormatHeader) )
            {
		        string strFileName;
		        ConvertWCHARToChar( strFileName, pKimFile );
		        ErrorLogMsg( XEM_ERROR9, strFileName.c_str() );
                return false;
            }
            //kProxy2.m_kFileSerializer.SaveFile( pKimFile );
            kProxy2.m_kFileSerializer.Swap( m_vecItemTempletInfo );
        }
        CX2Item::KItemFormatTemplet::_SetData( &m_vecItemTempletInfo.front() );

	    lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pItemManager", this );
	    lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2SetItemManager", this );
    }

    return true;

}








bool CX2ItemManager::AddShopItemList_LUA()
{
	ItemIDList* pItemIDList = new ItemIDList();



	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	int villiageID;
	LUA_GET_VALUE_RETURN(	luaManager, "VILLIAGE_ID",	villiageID,	0, goto LFail );

	if( luaManager.BeginTable( "SALE_GROUP" ) == true )
	{
		int index = 1;
		int buf;
		while( luaManager.GetValue(index, buf) == true )
		{
			map<int, ItemIDList*>::iterator iter;
			iter = m_ShopItemGroup.find(buf);
			if( iter != m_ShopItemGroup.end() )
			{
				ItemIDList* pGroupItemIDList = iter->second;
				for( int i = 0; i < (int)pItemIDList->m_ItemIDList.size(); i++ )
				{
					int itemID = pItemIDList->m_ItemIDList[i];
					pItemIDList->m_ItemIDList.push_back( itemID );
				}
			}

			index++;
		}

		luaManager.EndTable();
	}

	if( luaManager.BeginTable( "SALE_ITEM" ) == true )
	{
		int index = 1;
		int buf;
		while( luaManager.GetValue(index, buf) == true )
		{
			pItemIDList->m_ItemIDList.push_back( buf );
			index++;
		}
		luaManager.EndTable();
	}


	if( m_ShopItemList.find(villiageID) != m_ShopItemList.end() )
	{
		SAFE_DELETE(pItemIDList);
		return false;
	}
	else
	{
		m_ShopItemList.insert( std::make_pair(villiageID, pItemIDList) );
	}

	// 특정 npc만이 판매하는 물품
	NpcItemIDList* pNpcItemIDList = NULL;
	if( luaManager.BeginTable( "SALE_ITEM_NPC" ) == true )
	{
		int index = 1;
		while( luaManager.BeginTable( index ) == true )
		{
			pNpcItemIDList = new NpcItemIDList();

			int npcID;
			LUA_GET_VALUE_RETURN(	luaManager, "NPC_ID",	npcID,	0, goto LFail01 );

			pNpcItemIDList->m_NpcId = npcID;

			if( luaManager.BeginTable( "ITEM_LIST" ) == true )
			{
				int index = 1;
				int buf;
				while( luaManager.GetValue(index, buf) == true )
				{
					pNpcItemIDList->m_ItemIDList.push_back( buf );
					index++;
				}
				luaManager.EndTable();
			}

			if( m_ShopNpcItemList.find( std::make_pair(villiageID, pNpcItemIDList->m_NpcId) ) != m_ShopNpcItemList.end() )
			{
				SAFE_DELETE(pNpcItemIDList);
				return false;
			}
			else
			{
				m_ShopNpcItemList.insert( std::make_pair( std::make_pair(villiageID, pNpcItemIDList->m_NpcId), pNpcItemIDList) );
			}

			index++;
			luaManager.EndTable();
		}

		luaManager.EndTable();
	}

	return true;

LFail01:
	SAFE_DELETE( pNpcItemIDList );
LFail:
	SAFE_DELETE( pItemIDList );
	return false;
}

bool CX2ItemManager::AddShopItemGroup_LUA()
{
	ItemIDList* pItemIDList = new ItemIDList();



	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	int groupID;
	LUA_GET_VALUE_RETURN(	luaManager, "GROUP_ID",	groupID,	0, goto LFail );

	if( luaManager.BeginTable( "ITEM_ID" ) == true )
	{
		int index = 1;
		int buf;
		while( luaManager.GetValue(index, buf) == true )
		{
			pItemIDList->m_ItemIDList.push_back( buf );
			index++;
		}

		luaManager.EndTable();
	}






	if( m_ShopItemGroup.find(groupID) != m_ShopItemGroup.end() )
	{
		SAFE_DELETE(pItemIDList);
		return false;
	}
	else
	{
		m_ShopItemGroup.insert( std::make_pair(groupID, pItemIDList) );
	}



	return true;

LFail:
	SAFE_DELETE( pItemIDList );
	return false;
}

void CX2ItemManager::GetShopItemList( int mapID, vector<const CX2Item::ItemTemplet*>& itemTempletList )
{
	map<int, ItemIDList*>::iterator iter;
	iter = m_ShopItemList.find( mapID );
	if( m_ShopItemList.end() == iter )
	{
		// mapID가 이상하다
		itemTempletList.clear();
		return;
	}

	ItemIDList* pItemIDList = iter->second;

	itemTempletList.clear();
	for( int i = 0; i < (int)pItemIDList->m_ItemIDList.size(); i++ )
	{
		const CX2Item::ItemTemplet* pItemTemplet = GetItemTemplet( pItemIDList->m_ItemIDList[i] );
		if( pItemTemplet != NULL )
			itemTempletList.push_back( pItemTemplet );
	}
}

bool CX2ItemManager::GetNpcShopItemList( int mapID, int npcID, vector<const CX2Item::ItemTemplet*>& itemTempletList )
{
	map< std::pair<int,int>, NpcItemIDList*>::iterator iter;
	iter = m_ShopNpcItemList.find( std::make_pair( mapID, npcID ) );
	if( m_ShopNpcItemList.end() == iter )
	{
		// mapID가 이상하다
		itemTempletList.clear();
		return false;
	}

	NpcItemIDList* pItemIDList = iter->second;

	if(pItemIDList->m_NpcId != npcID)
	{
		itemTempletList.clear();
		return false;
	}

	itemTempletList.clear();
	for( int i = 0; i < (int)pItemIDList->m_ItemIDList.size(); i++ )
	{
		const CX2Item::ItemTemplet* pItemTemplet = GetItemTemplet( pItemIDList->m_ItemIDList[i] );
		if( pItemTemplet != NULL )
			itemTempletList.push_back( pItemTemplet );
	}

	if(itemTempletList.size() > 0 )
		return true;

	return false;
}








void CX2ItemManager::UpdateEqip( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim,
									CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET,
									IN OUT vector<CX2EqipPtr>& viewEqipList, bool bLoad )
{
	//기존 장비 삭제
	viewEqipList.resize( 0 );


	//장비 리스트 업데이트
	CX2Item* pItem = NULL;
	for( int i = 0; i < pUnit->GetViewEqipNum(); i++ )
	{
		int enchantLevel = 0;

		pItem = pUnit->GetInventory().GetItem( pUnit->GetViewEqipUID(i) );
		ASSERT( NULL != pItem );
		if( NULL == pItem )
			continue;

		enchantLevel = pItem->GetItemData().m_EnchantLevel;


		CX2EqipPtr pCX2Eqip;
		if( pItem->GetItemTemplet()->GetUseCondition() == CX2Item::UC_ANYONE )
		{
			pCX2Eqip = CX2Eqip::CreateEquip( pItem, pAnim, enchantLevel, false, bLoad, pUnit->GetType() );
		}
		else
		{
			pCX2Eqip = CX2Eqip::CreateEquip( pItem, pAnim, enchantLevel, false, bLoad );
		}

		viewEqipList.push_back( pCX2Eqip );
	}



	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, IN OUT viewEqipList, bLoad );
}


void CX2ItemManager::ReplaceEmptyToBasic( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim,
										 CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET,
										 IN OUT vector<CX2EqipPtr>& viewEqipList, bool bLoad )
{
	//빠진 부위가 있으면 기본 아이템으로 대체한다
	bool viewEqipPosition[50];
	CheckEmptyEqipPosition( viewEqipPosition, viewEqipList );

	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_HAIR,	pUnit->GetUnitTemplet()->m_BasicHairItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_FACE,	pUnit->GetUnitTemplet()->m_BasicFaceItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_BODY,	pUnit->GetUnitTemplet()->m_BasicBodyItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_LEG,	pUnit->GetUnitTemplet()->m_BasicLegItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_HAND,	pUnit->GetUnitTemplet()->m_BasicHandItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_DEFENCE_FOOT,	pUnit->GetUnitTemplet()->m_BasicFootItemID,		bLoad	);
	ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_WEAPON_HAND,	pUnit->GetUnitTemplet()->m_BasicWeaponItemID,	bLoad	);


	switch( pUnit->GetUnitTemplet()->m_UnitType )
	{
	case CX2Unit::UT_RAVEN:
		{
			ReplaceEmptyToBasic( pUnit, pAnim, pMotion, pAnimXET, viewEqipList, viewEqipPosition, CX2Unit::EP_RAVEN_LEFT_ARM, pUnit->GetUnitTemplet()->m_RavenLeftArmItemID,	bLoad	);
		} break;
	}
}

void CX2ItemManager::ReplaceEmptyToBasic( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim,
										 CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET,
										 IN OUT vector<CX2EqipPtr>& viewEqipList, bool* pViewEqipPosition,
										 CX2Unit::EQIP_POSITION eqipPos, int itemID, bool bLoad )
{
	if( pViewEqipPosition[eqipPos] == false
		&& itemID != 0 )
	{
        CX2Item* pX2Item = NULL;
        {
		    CX2Item::ItemData kItemData;
		    kItemData.m_ItemID				= itemID;
		    //kItemData.m_OwnerUnitUID		= pUnit->GetUID();
		    //kItemData.m_OwnerUserUID		= pUnit->GetOwnerUserUID();
		    kItemData.m_Endurance			= 1;

		    pX2Item				= new CX2Item( kItemData, pUnit );
            ASSERT( pX2Item != NULL );
        }
		int enchantLevel = pX2Item->GetItemData().m_EnchantLevel;



		CX2EqipPtr pCX2Eqip;
		if( pX2Item->GetItemTemplet()->GetUseCondition() == CX2Item::UC_ANYONE )
		{
			pCX2Eqip = CX2Eqip::CreateEquip( pX2Item, pAnim, enchantLevel, true, bLoad, pUnit->GetType() );
		}
		else
		{
			pCX2Eqip = CX2Eqip::CreateEquip( pX2Item, pAnim, enchantLevel, true, bLoad );
		}

		viewEqipList.push_back( pCX2Eqip );
	}
}







void CX2ItemManager::CheckEmptyEqipPosition( bool* pViewEqipPosition, IN OUT vector<CX2EqipPtr>& viewEqipList )
{
	ZeroMemory( pViewEqipPosition, sizeof(bool) * 50 );
	for( int i =0; i < (int)viewEqipList.size(); ++i )
	{
		CX2EqipPtr pEqip = viewEqipList[i];
		CX2Item* pEqipItem = pEqip->GetItem();

		pViewEqipPosition[pEqipItem->GetItemTemplet()->GetEqipPosition()] = true;
	}
}


void CX2ItemManager::GetItemList( vector<int>& vecItemID, int page, int quantityPerPage )
{
    unsigned int uNumItems = GetItemNum();

    unsigned    uItemIndex = 0;

	int repeatNum = 0;
	while( repeatNum <= (page-1)*quantityPerPage )
	{
		++uItemIndex;
		repeatNum++;
	}

	repeatNum = 0;

    unsigned    uItemIndex2 = uItemIndex;
	while( repeatNum <= quantityPerPage - 1 )
	{
		++uItemIndex2;
		repeatNum++;
	}

	for ( ; uItemIndex < uNumItems && uItemIndex != uItemIndex2; uItemIndex++ )
	{
		vecItemID.push_back( GetItemIDFromIndex( uItemIndex ) );
	}
}




bool CX2ItemManager::AddRandomItemTemplet_LUA( int iItemID_, int iKeyItemID_, int iRequiredKeyCount_, int iRequiredED_ )
{
	RandomItemData* pRandomItemData = new RandomItemData();

	pRandomItemData->m_ItemID			= iItemID_;
	pRandomItemData->m_KeyItemID		= iKeyItemID_;
	pRandomItemData->m_RequiredKeyCount	= iRequiredKeyCount_;
	pRandomItemData->m_iRequiredED		= iRequiredED_;

	//{{ kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
	m_multimapRandomItem.insert( std::make_pair( pRandomItemData->m_ItemID, pRandomItemData ) );
	//}} kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
	return true;
}

//{{ kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
std::pair< CX2ItemManager::RandomItemItr, CX2ItemManager::RandomItemItr > CX2ItemManager::GetRandomItemData( int itemID )
{
	return m_multimapRandomItem.equal_range( itemID );
}

bool CX2ItemManager::IsRandomItem( std::pair< CX2ItemManager::RandomItemItr, CX2ItemManager::RandomItemItr >& pairValue )
{
	if ( ( pairValue.first == m_multimapRandomItem.end() ) && ( pairValue.second == m_multimapRandomItem.end() )
		 || ( pairValue.first == pairValue.second ) )
		return false;
	else
		return true;
}

//}} kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가

CX2ItemManager::ManufactureData* CX2ItemManager::GetManufactureData( int ManufactureID )
{
	std::map< int, ManufactureData* >::iterator mit;
	mit = m_mapManufacture.find( ManufactureID );
	if( mit == m_mapManufacture.end() )
		return NULL;

	return mit->second;
}

bool CX2ItemManager::AddManufactureTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	ManufactureData* pManufactureData = new ManufactureData();

	int iManufactureID = 0;


	int iServerGroupId = -1;
	LUA_GET_VALUE(		  luaManager, "m_iServerGroupID",	iServerGroupId,							-1 );
	if( iServerGroupId != -1 && iServerGroupId != g_pInstanceData->GetServerGroupID() )
	{
		SAFE_DELETE( pManufactureData );
		return false;
	}

	pManufactureData->m_iServerGroupID = iServerGroupId;

	LUA_GET_VALUE_RETURN( luaManager, "m_ManufactureID",	iManufactureID,							0,		goto LoadFail; );
	LUA_GET_VALUE_RETURN( luaManager, "m_ResultGroupID",	pManufactureData->m_ResultGroupID,		0,		goto LoadFail; );
	LUA_GET_VALUE(		  luaManager, "m_Cost",			pManufactureData->m_Cost,				0 );
	LUA_GET_VALUE(		  luaManager, "m_ManufactureType",	pManufactureData->m_ManufactureType,	-1 );


	if( luaManager.BeginTable( "m_MaterialTable" ) == true )
	{
		int index = 1;
		while( luaManager.BeginTable( index ) == true )
		{
			MaterialData materialData;
			LUA_GET_VALUE_RETURN( luaManager, "m_MaterialID",		materialData.m_MaterialItemID,		0,		goto LoadFail; );
			LUA_GET_VALUE_RETURN( luaManager, "m_MaterialCount",	materialData.m_MaterialCount,		0,		goto LoadFail; );

			if( materialData.m_MaterialItemID == 0 || materialData.m_MaterialCount == 0 )
			{
				goto LoadFail;
			}

			pManufactureData->m_vecMaterials.push_back( materialData );

			luaManager.EndTable();
			index++;
		}
		luaManager.EndTable();
	}
	else
	{
		goto LoadFail;
	}


	m_mapManufacture.insert( std::make_pair( iManufactureID, pManufactureData ) );

	return true;

LoadFail:
	wstringstream wstrStream;
	wstrStream << L"ManufactureDataLoadFail_" << iManufactureID << L"_";

	ErrorLogMsg( XEM_ERROR105, wstrStream.str().c_str() );
	SAFE_DELETE( pManufactureData );
	return false;
}

//{{ kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록
bool CX2ItemManager::AddManufactureVillageData_LUA( int iManufactureID, int iHouseID )
{
	if( iManufactureID <= 0 || iHouseID <= 0 )
	{
		wstringstream wstrStream;
		wstrStream << L"ManufactureVaillageDataLoadFail_" << iManufactureID << L"_";

		ErrorLogMsg( XEM_ERROR105, wstrStream.str().c_str() );
		return false;
	}

	int iServerGroup = 0;
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	LUA_GET_VALUE( luaManager, "m_iServerGroupID",	iServerGroup,	-1 );
	if( iServerGroup != -1 && iServerGroup != g_pInstanceData->GetServerGroupID() )
	{
		return false;
	}

	map<int, std::vector<int> >::iterator mit;
	mit = m_mapManufactureVillage.find( iHouseID );

	if( mit == m_mapManufactureVillage.end() )
	{
		std::vector< int > vecManufactureData;
		vecManufactureData.push_back( iManufactureID );
		m_mapManufactureVillage.insert( std::make_pair( iHouseID, vecManufactureData ) );
	}
	else
	{
		std::vector< int >::iterator vit;
		vit = std::find( mit->second.begin(), mit->second.end(), iManufactureID );
		if( vit == mit->second.end() )
		{
			mit->second.push_back( iManufactureID );
		}
	}
	return true;
}
//}} kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록




bool CX2ItemManager::AddManufactureResultGroup( int iGroupID, int iItemID )
{
	map<int, ManufactureResultGroupData>::iterator it =
		m_mapManufactureResultGroupData.find( iGroupID );
	if( it != m_mapManufactureResultGroupData.end() )
	{
		it->second.m_vecResultGroupItemID.push_back( iItemID );
	}
	else
	{
		ManufactureResultGroupData resultData;
		resultData.m_vecResultGroupItemID.push_back( iItemID );
		m_mapManufactureResultGroupData.insert( std::make_pair( iGroupID, resultData ) );
	}

	return true;
}

bool CX2ItemManager::AddCashItem( vector< KBillProductInfo >& vecKBillProductInfo )
{
	bool bCheckError = false;

	for ( int i = 0; i < (int)vecKBillProductInfo.size(); i++ )
	{
		KBillProductInfo& kBillProductInfo = vecKBillProductInfo[i];

		int itemID = kBillProductInfo.m_iProductID;

		map<int, CX2ItemManager::CashItem* >::iterator mit;
		mit = m_mapCashItem.find( itemID );

		if ( mit == m_mapCashItem.end() )
		{
			CX2ItemManager::CashItem* pCashItem = new CX2ItemManager::CashItem();
			pCashItem->m_ItemID = itemID;

			if ( IsShowPackageItem( itemID ) == true )
				pCashItem->m_bShow = true;
			else
				pCashItem->m_bShow = false;

			pCashItem->m_vecKBillProductInfo.push_back( kBillProductInfo );


			m_mapCashItem.insert( std::make_pair( itemID, pCashItem ) );
			m_vecCashItem.push_back( itemID );
		}
		else
		{
			CX2ItemManager::CashItem* pCashItem = mit->second;
			if ( pCashItem != NULL )
			{
				bool bOverlap = false;
				for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
				{
					KBillProductInfo& tempKBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
					if ( tempKBillProductInfo.m_iProductNo == kBillProductInfo.m_iProductNo )
					{
						bOverlap = true;
						bCheckError = true;
						//에러 로그 찍어보실까낭
						WCHAR buffer[32];
						_itow(kBillProductInfo.m_iProductID, buffer, 10);
						ErrorLogMsg( KEM_ERROR354, buffer );
						break;
					}
				}

				if ( bOverlap == false )
				{
					pCashItem->m_vecKBillProductInfo.push_back( kBillProductInfo );
					std::sort( pCashItem->m_vecKBillProductInfo.begin(), pCashItem->m_vecKBillProductInfo.end(), CX2ItemManager::CCashProductSort() );
				}
			}
		}
	}

	if ( bCheckError == true )
		return false;

	return true;
}

bool CX2ItemManager::AddCashItemEx()
{
	m_vecNewCashItem.resize(0);
	m_vecHotCashItem.resize(0);
	m_vecRecommendCashItem.resize(0);
	m_vecLimitedCashItem.resize(0);
	m_vecEventCashItem.resize(0);

	//{{ robobeg : 2008-10-28
	//KLuaManager kLuamanager;
	KLuaManager kLuamanager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
	//}} robobeg : 2008-10-28
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"CashShopItemList.lua");

	if( kLuamanager.BeginTable( "FIRST_PAGE_ITEM_LIST" ) == true )
	{
		if( kLuamanager.BeginTable( "NEW_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;
			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true )
					{
						if ( pCashItem->m_vecKBillProductInfo.empty() == false )
						{
							pCashItem->m_bIsNew = true;
							if ( std::find( m_vecNewCashItem.begin(), m_vecNewCashItem.end(), itemID ) == m_vecNewCashItem.end() )
							{
								if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
								{
									m_vecNewCashItem.push_back( itemID );
								}
								else
								{
									std::vector< KBillProductInfo >::iterator vit;

									for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
									{
										if( vit->m_iProductNo == iProductNO )
										{
											m_vecNewCashItem.push_back( itemID );
										}
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}

			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "HOT_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;
			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true )
					{
						if ( pCashItem->m_vecKBillProductInfo.empty() == false )
						{
							pCashItem->m_bIsHot = true;
							if ( std::find( m_vecHotCashItem.begin(), m_vecHotCashItem.end(), itemID ) == m_vecHotCashItem.end() )
							{
								if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
								{
									m_vecHotCashItem.push_back( itemID );
								}
								else
								{
									std::vector< KBillProductInfo >::iterator vit;

									for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
									{
										if( vit->m_iProductNo == iProductNO )
										{
											m_vecHotCashItem.push_back( itemID );
										}
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}

			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "RECOMMEND_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;
			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true )
					{
						if ( pCashItem->m_vecKBillProductInfo.empty() == false )
						{
							pCashItem->m_bIsRecommend = true;
							if ( std::find( m_vecRecommendCashItem.begin(), m_vecRecommendCashItem.end(), itemID ) == m_vecRecommendCashItem.end() )
							{
								if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
								{
									m_vecRecommendCashItem.push_back( itemID );
								}
								else
								{
									std::vector< KBillProductInfo >::iterator vit;

									for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
									{
										if( vit->m_iProductNo == iProductNO )
										{
											m_vecRecommendCashItem.push_back( itemID );
										}
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}

			kLuamanager.EndTable();
		}

		kLuamanager.EndTable();
	}

	if( kLuamanager.BeginTable( "ICON_ITEM_LIST" ) == true )
	{
		if( kLuamanager.BeginTable( "ICON_LIMITED_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;
			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true )
					{
						if ( pCashItem->m_vecKBillProductInfo.empty() == false )
						{
							pCashItem->m_bIsLimited = true;
							if ( std::find( m_vecLimitedCashItem.begin(), m_vecLimitedCashItem.end(), itemID ) == m_vecLimitedCashItem.end() )
							{
								if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
								{
									m_vecLimitedCashItem.push_back( itemID );
								}
								else
								{
									std::vector< KBillProductInfo >::iterator vit;

									for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
									{
										if( vit->m_iProductNo == iProductNO )
										{
											m_vecLimitedCashItem.push_back( itemID );
										}
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}
			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "ICON_EVENT_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;
			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true )
					{
						if ( pCashItem->m_vecKBillProductInfo.empty() == false )
						{
							pCashItem->m_bIsLimited = true;
							if ( std::find( m_vecEventCashItem.begin(), m_vecEventCashItem.end(), itemID ) == m_vecEventCashItem.end() )
							{
								if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
								{
									m_vecEventCashItem.push_back( itemID );
								}
								else
								{
									std::vector< KBillProductInfo >::iterator vit;

									for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
									{
										if( vit->m_iProductNo == iProductNO )
										{
											m_vecEventCashItem.push_back( itemID );
										}
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}

			kLuamanager.EndTable();
		}

		if( kLuamanager.BeginTable( "ICON_HOT_ITEM" ) == true )
		{
			int tableIndex = 1;
			int itemID = -1;

			while( kLuamanager.BeginTable( tableIndex ) == true )
			{
				int iProductNO = -1;

				LUA_GET_VALUE( kLuamanager, 1,	itemID, -1 );
				LUA_GET_VALUE( kLuamanager, 2,	iProductNO, -1 );

				CX2ItemManager::CashItem* pCashItem = GetCashItem( itemID );
				if ( pCashItem != NULL )
				{
					if ( pCashItem->m_bShow == true && pCashItem->m_vecKBillProductInfo.empty() == false )
					{
						pCashItem->m_bIsLimited = true;
						if ( std::find( m_vecHotIconCashItem.begin(), m_vecHotIconCashItem.end(), itemID ) == m_vecHotIconCashItem.end() )
						{
							if( iProductNO == -1 )	//	ProductNO가 -1이면 무조건 등록
							{
								m_vecHotIconCashItem.push_back( itemID );
							}
							else
							{
								std::vector< KBillProductInfo >::iterator vit;

								for( vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); vit++ )
								{
									if( vit->m_iProductNo == iProductNO )
									{
										m_vecHotIconCashItem.push_back( itemID );
									}
								}
							}
						}
					}
				}
				tableIndex++;
				kLuamanager.EndTable();
			}
			kLuamanager.EndTable();
		}
		kLuamanager.EndTable();
	}

	return true;
}

//{{ 2008. 11. 18  최육사
void CX2ItemManager::AddItemExchangeGroup_LUA( int iSourceItemID, int iResultItemID, float fRate )
{
	m_mapItemExchange.insert( std::make_pair( iSourceItemID, iSourceItemID ) );
}

bool CX2ItemManager::IsCanItemExchange( int iItemID )
{
	map< int, int >::const_iterator mit;
	mit = m_mapItemExchange.find( iItemID );
	if( mit == m_mapItemExchange.end() )
		return false;

	return true;
}
//}}

bool CX2ItemManager::GetIsUpdateCashItem()
{
	if ( m_mapCashItem.empty() == false )
		return true;

	return false;
}

void CX2ItemManager::ClearCashItemList()
{
	std::map< int, CashItem* >::iterator mit3;
	mit3 = m_mapCashItem.begin();
	for( mit3 = m_mapCashItem.begin(); mit3 != m_mapCashItem.end(); mit3++ )
	{
		SAFE_DELETE( mit3->second );
	}
	m_mapCashItem.clear();
	m_vecCashItem.clear();
}

CX2ItemManager::CashItem* CX2ItemManager::GetCashItem( int itemID )
{
	CX2ItemManager::CashItem* pCashItem = NULL;

	map<int, CX2ItemManager::CashItem* >::iterator mit;
	mit = m_mapCashItem.find( itemID );

	if ( mit != m_mapCashItem.end() )
		pCashItem = mit->second;


	return pCashItem;
}

wstring CX2ItemManager::GetCashItemPeriod( int iProductNo )
{
	map<int, CX2ItemManager::CashItem* >::iterator mit;

	for ( mit = m_mapCashItem.begin(); mit != m_mapCashItem.end(); mit++ )
	{
		CX2ItemManager::CashItem* pCashItem = mit->second;
		if ( pCashItem == NULL )
			continue;

		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kKBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			if ( kKBillProductInfo.m_iProductNo == iProductNo )
			{
				return pCashItem->GetPeriod( kKBillProductInfo );
			}
		}
	}

	return L"";
}

KBillProductInfo* CX2ItemManager::GetCashItemProduct( int iProductNo )
{
	map<int, CX2ItemManager::CashItem* >::iterator mit;

	for ( mit = m_mapCashItem.begin(); mit != m_mapCashItem.end(); mit++ )
	{
		CX2ItemManager::CashItem* pCashItem = mit->second;
		if ( pCashItem == NULL )
			continue;

		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kKBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			if ( kKBillProductInfo.m_iProductNo == iProductNo )
			{
				return &kKBillProductInfo;
			}
		}
	}

	return NULL;
}

vector<int> CX2ItemManager::GetNewCashItemList()
{
	return m_vecNewCashItem;
}

vector<int> CX2ItemManager::GetHotCashItemList()
{
	return m_vecHotCashItem;
}

vector<int> CX2ItemManager::GetRecommendCashItemList()
{
	return m_vecRecommendCashItem;
}

vector<int> CX2ItemManager::GetLimitedCashItemList()
{
	return m_vecLimitedCashItem;
}

// 한정판매 캐시아이템인지 체크
bool CX2ItemManager::CheckLimitedCashItem( int itemID )
{
	if ( std::find( m_vecLimitedCashItem.begin(), m_vecLimitedCashItem.end(), itemID ) != m_vecLimitedCashItem.end() )
		return true;
	else
		return false;
}


bool CX2ItemManager::CheckEventCashItem( int itemID )
{
	if ( std::find( m_vecEventCashItem.begin(), m_vecEventCashItem.end(), itemID ) != m_vecEventCashItem.end() )
		return true;
	else
		return false;
}

bool CX2ItemManager::CheckHotIconCashItem( int itemID )
{
	if ( std::find( m_vecHotIconCashItem.begin(), m_vecHotIconCashItem.end(), itemID ) != m_vecHotIconCashItem.end() )
		return true;
	else
		return false;
}

vector<int> CX2ItemManager::GetAllCashItemList()
{
	vector< int > vecItemID = m_vecCashItem;;

	map<int, CX2ItemManager::CashItem* >::iterator mit;

	for ( int i = 0; i < (int)vecItemID.size(); i++ )
	{
		int itemID = vecItemID[i];

		mit = m_mapCashItem.find( itemID );
		if ( mit != m_mapCashItem.end() )
		{
			CX2ItemManager::CashItem* pCashItem = mit->second;
			if ( pCashItem == NULL )
				continue;

			if ( pCashItem->m_bShow == false )
			{
				vecItemID.erase( vecItemID.begin() + i );
				i--;
			}
		}
	}

	return vecItemID;
}


//{{ kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록
bool CX2ItemManager::GetManufactureVillageData( int iHouseID, std::vector< int >& vecManufactureVillage )
{
	map<int, std::vector<int> >::iterator mit;
	mit = m_mapManufactureVillage.find( iHouseID );
	if( mit == m_mapManufactureVillage.end() )
		return false;

	vecManufactureVillage = mit->second;

	return true;
}
//}} kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록

const CX2ItemManager::ManufactureResultGroupData* CX2ItemManager::GetManufactureResultGroupData( int iResultGroupID )
{
	map<int, ManufactureResultGroupData>::iterator mit;
	mit = m_mapManufactureResultGroupData.find( iResultGroupID );
	if( mit != m_mapManufactureResultGroupData.end() )
	{
		return &mit->second;
	}

	return NULL;
}

int CX2ItemManager::CashItem::GetCash()
{
	unsigned long cashPrice = 0;

	const KBillProductInfo* pKBillProductInfo = GetGateProduct();
	if ( pKBillProductInfo != NULL )
	{
		cashPrice = pKBillProductInfo->m_iSalePrice;
	}

	return cashPrice;
}

const KBillProductInfo* CX2ItemManager::CashItem::GetGateProduct()
{
	// 	for ( int i = 0; i < (int)m_vecKBillProductInfo.size(); i++ )
	// 	{
	// 		KBillProductInfo& kBillProductInfo = m_vecKBillProductInfo[i];
	// 		if ( kBillProductInfo.m_ulProductNo == kBillProductInfo.m_ulRelationProductNo )
	// 		{
	// 			return &kBillProductInfo;
	// 		}
	// 	}

	if ( m_vecKBillProductInfo.empty() == false )
	{
		KBillProductInfo& kBillProductInfo = m_vecKBillProductInfo[0];
		return &kBillProductInfo;
	}

	return NULL;
}


wstring CX2ItemManager::CashItem::GetPeriod( KBillProductInfo& kKBillProductInfo )
{
	wstringstream wstrstm;

	if ( (int)kKBillProductInfo.m_cPeriod != 0 )
	{
		wstrstm << (int)kKBillProductInfo.m_cPeriod << GET_STRING( STR_ID_14 ) << L"/";
	}
	else
	{
		wstrstm << (int)kKBillProductInfo.m_cQuantity << GET_STRING( STR_ID_24 ) << L"/";
	}

	wstrstm << kKBillProductInfo.m_iSalePrice << GET_STRING( STR_ID_34 );

	return wstrstm.str().c_str();
}


bool CX2ItemManager::CashItem::GetIsRecommend()
{
	/*
	for ( int i = 0; i < (int)m_vecKNXBTProductInfo.size(); i++ )
	{
		KNXBTProductInfo& kNXBTProductInfo = m_vecKNXBTProductInfo[i];

		if ( kNXBTProductInfo.m_ulRelationProductNo == kNXBTProductInfo.m_ulProductNo )
			return true;
		else
			return false;
	}
	*/
	return m_bIsRecommend;
}
//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
bool CX2ItemManager::AddCube_PackageData_LUA( int iCubePackageItemID, int iPackageItemID1, int iPackageItemID2, int iUnitType){

	if( iCubePackageItemID <= 0 || iPackageItemID1 <= 0 )
	{
		ErrorLog(KEM_ERROR355);
		return false;
	}
	// 패키지 정보
	CubePackageData kCubePackageData;
	kCubePackageData.m_iPackageID1		   = iPackageItemID1;
	kCubePackageData.m_iPackageID2		   = iPackageItemID2 ;
	std::map< int, CubePackageData > mapCubePackageData;
	mapCubePackageData.insert(std::make_pair(iUnitType,kCubePackageData));

	std::map< int, std::map< int,CubePackageData > >::iterator mit;
	mit = m_mapCube_PackageData.find( iCubePackageItemID );
	if( mit == m_mapCube_PackageData.end() )
	{
		//std::vector< CubePackageData > vecCubePackageData;
		//vecCubePackageData.push_back( kCubePackageData );
		m_mapCube_PackageData.insert(std::make_pair(iCubePackageItemID,mapCubePackageData));
	}
	else
	{
		mit->second.insert(std::make_pair(iUnitType,kCubePackageData));
	}

	return true;
}
//}}

//{{ 2008. 4. 24  최육사  패키지 아이템
bool CX2ItemManager::AddPackageItemData_LUA( int iPackageItemID, int iItemID, unsigned short usProductPieces, bool bShowItem )
{
	if( iPackageItemID <= 0 || iItemID <= 0 )
	{
		ErrorLog(KEM_ERROR355);
		return false;
	}

	// 패키지 정보
	PackageItemData kPackageItemData;
	kPackageItemData.m_iItemID		   = iItemID;
	kPackageItemData.m_usProductPieces = usProductPieces;
	kPackageItemData.m_bShowItem	   = bShowItem;

	std::map< int, std::vector< PackageItemData > >::iterator mit;
	mit = m_mapPackageData.find( iPackageItemID );
	if( mit == m_mapPackageData.end() )
	{
		std::vector< PackageItemData > vecPackageData;
		vecPackageData.push_back( kPackageItemData );
		m_mapPackageData.insert( std::make_pair( iPackageItemID, vecPackageData ) );
	}
	else
	{
		mit->second.push_back( kPackageItemData );
	}

	// 패키지 멤버 아이템 안보여주는 리스트 [key값은 중복되지 않으므로 바로 insert]
	if( !bShowItem )
	{
		m_setShowPackageItem.insert( iItemID );
	}

	return true;
}
//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
bool CX2ItemManager::GetPackageCubeItemData( int iPackageItemID, std::vector< PackageItemData >& vecPackageData)
{
	vecPackageData.clear();

	std::map< int, std::map< int, CubePackageData > >::const_iterator mit;
	mit = m_mapCube_PackageData.find( iPackageItemID );
	if( mit == m_mapCube_PackageData.end() )
		return false;
	//큐브 패키지 아이템을 AddCubePackageData.lua 를 참조하여 아바타 패키지 ID로 변환
	std::map< int, std::vector< PackageItemData > >::const_iterator mit1;
	//CubePackageData kCubePackageData = mit->second;
	std::map<int,CubePackageData> vecCubePackageData = mit->second;
	int iCheakUnitPackageItemID = -1;
	int iUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
	std::map< int, CubePackageData >::const_iterator CPmit;
		CPmit = vecCubePackageData.find( iUnitType );
		if( CPmit == vecCubePackageData.end() )
			return false;

		if(CPmit->second.m_iPackageID1 == m_iLastPackacgeID){
			iCheakUnitPackageItemID = CPmit->second.m_iPackageID2;
			m_iLastPackacgeID = iCheakUnitPackageItemID;
		}
		else{
			iCheakUnitPackageItemID = CPmit->second.m_iPackageID1;
			m_iLastPackacgeID = iCheakUnitPackageItemID;
		}

	mit1 = m_mapPackageData.find( iCheakUnitPackageItemID );
	if( mit1 == m_mapPackageData.end() )
		return false;

	vecPackageData = mit1->second;
	return true;
}
//}} 2013.01.26 최민철 큐브패키지아이템 미리보기기능
bool CX2ItemManager::GetPackageData( int iPackageItemID, std::vector< PackageItemData >& vecPackageData )
{
	//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
	m_iLastPackacgeID = 0;
	//}} 2013.01.26 최민철 큐브패키지아이템 미리보기기능
	vecPackageData.clear();

	std::map< int, std::vector< PackageItemData > >::const_iterator mit;
	mit = m_mapPackageData.find( iPackageItemID );
	if( mit == m_mapPackageData.end() )
		return false;

	vecPackageData = mit->second;
	return true;
}

bool CX2ItemManager::IsShowPackageItem( int iItemID )
{
	std::set< int >::const_iterator sit;
	sit = m_setShowPackageItem.find( iItemID );
	if( sit == m_setShowPackageItem.end() )
		return true;

	return false;
}
//}}

bool CX2ItemManager::IsShowItemTooniland( int iItemId )
{
	// 나중에 스크립트로 빼자...
	switch( iItemId )
	{
	case 131134:	// 코보 서포터 서비스
		return false;
	default:
		break;
	}

	return true;
}





bool CX2ItemManager::AddItemExchangeData( int iHOUSEID, ITEM_EXCHANGE_TYPE eExchangeType, int iSrcItemID, int iDestItemID, int iQuantity, float fDestItemRate /*= 0.f*/ )
{
	if( CX2LocationManager::HI_INVALID == iHOUSEID )
		return false;

	if( IET_INVALID == eExchangeType )
		return false;

	int iServerGroup = 0;
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	LUA_GET_VALUE( luaManager, "m_iServerGroupID",	iServerGroup,	-1 );
	if( iServerGroup != -1 && iServerGroup != g_pInstanceData->GetServerGroupID() )
	{
		return false;
	}

	int iSrcQuantity = 0;
	LUA_GET_VALUE( luaManager, "m_iSourceQuantity",	iSrcQuantity,	1 );

	short sPeriod = 0;
	LUA_GET_VALUE( luaManager, "m_sPeriod",			sPeriod,		0 );


	//ItemExchangeData& itemExchangeData = m_mapItemExchangeData[ iHOUSEID ];

	std::map< int, vector< ItemExchangeData > >::iterator	mItrItemExchangeData;

	mItrItemExchangeData		=	m_mapItemExchangeData.find( iHOUSEID );

	if ( mItrItemExchangeData	== m_mapItemExchangeData.end() )
	{
		m_mapItemExchangeData.insert( std::make_pair( iHOUSEID, std::vector< ItemExchangeData >() ) );
		mItrItemExchangeData	=	m_mapItemExchangeData.find( iHOUSEID );
	}

	ItemExchangeData	itemExchangeData;

	itemExchangeData.m_eExchangeType	=	eExchangeType;
	itemExchangeData.m_iSrcItemID		=	iSrcItemID;
	itemExchangeData.m_iDestItemID		=	iDestItemID;
	itemExchangeData.m_iSrcQuantity		=	iSrcQuantity;
	itemExchangeData.m_iQuantity		=	iQuantity;
	itemExchangeData.m_sPeriod			=	sPeriod;

	mItrItemExchangeData->second.push_back( itemExchangeData );

	return true;
}

bool CX2ItemManager::GetItemExchangeData( int iHouseID, std::vector< ItemExchangeData >& vecItemExchangeData )
{
	if ( iHouseID == CX2LocationManager::HI_INVALID )
		return false;

	std::map< int, vector< ItemExchangeData > >::iterator	mItrItemExchangeData;
	mItrItemExchangeData = m_mapItemExchangeData.find( iHouseID );

	if ( mItrItemExchangeData == m_mapItemExchangeData.end () )
		return false;

	vecItemExchangeData.reserve( mItrItemExchangeData->second.size() );

	vecItemExchangeData = mItrItemExchangeData->second;
	// 	std::copy( mItrItemExchangeData->second.begin(),
	// 		mItrItemExchangeData->second.begin() + mItrItemExchangeData->second.size(), vecItemExchangeData.begin() );
	//
	// 	if ( mItrItemExchangeData->second.size() !=	vecItemExchangeData.size() )	// 임시 테스트용
	// 	{
	// 		ASSERT( false );
	// 		return false;
	// 	}

	return true;
}


//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업




void CX2ItemManager::AddCoolTimeGroupItem_LUA( int iCoolTimeGroupID, int iSourceItemID )
{
	m_mapCoolTimeGroupID.insert( std::make_pair( iSourceItemID, iCoolTimeGroupID ) );

	map< int, std::vector<int> >::iterator mit = m_mapCoolTimeGroupItemIDList.find( iCoolTimeGroupID );
	if ( mit != m_mapCoolTimeGroupItemIDList.end() )
	{
		mit->second.push_back( iSourceItemID );
	}
	else
	{
		vector< int > vecTempItemID;
		vecTempItemID.push_back( iSourceItemID );
		m_mapCoolTimeGroupItemIDList.insert( std::make_pair( iCoolTimeGroupID, vecTempItemID ) );
	}
	return;
}
//  SERV_CHANGE_QUICK_SLOT_COOL_TIME_DB_SP 작업으로 ItemID에서 GroupID로 변경됨. 변수명만 변경되서 디파인 적용하지 않음.
int CX2ItemManager::GetCoolTimeGroupID( int iGroupID ) const
{
	map< int, int >::const_iterator mit = m_mapCoolTimeGroupID.find( iGroupID );
	if ( mit != m_mapCoolTimeGroupID.end() )
	{
		return mit->second;
	}
	return iGroupID;// 등록된 그룹이 없으면 아이템 하나가 하나의 그룹이 되도록 한다
}

//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
bool CX2ItemManager::SetItemLevelEnchantData_LUA( int iAtkPhysic, int iAtkMagic, int iDefPhysic, int iDefMagic, int iBaseHP )
{
	if( iAtkPhysic <= 0  ||  iAtkMagic <= 0  ||  iDefPhysic <= 0  ||  iDefMagic <= 0  ||  iBaseHP <= 0 )
	{
		START_LOG( cerr, L"스크립트 정보가 이상합니다!" )
			<< BUILD_LOG( iAtkPhysic )
			<< BUILD_LOG( iAtkMagic )
			<< BUILD_LOG( iDefPhysic )
			<< BUILD_LOG( iDefMagic )
			<< BUILD_LOG( iBaseHP )
			<< END_LOG;
		return false;
	}

	m_kItemLevelEnchantData.m_iAtkPhysic = iAtkPhysic;
	m_kItemLevelEnchantData.m_iAtkMagic = iAtkMagic;
	m_kItemLevelEnchantData.m_iDefPhysic = iDefPhysic;
	m_kItemLevelEnchantData.m_iDefMagic = iDefMagic;
	m_kItemLevelEnchantData.m_iBaseHP = iBaseHP;
	return true;
}

bool CX2ItemManager::GetItemLevel( IN const KItemInfo& kItemInfo, OUT int& iItemLevel )
{
	//const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( kItemInfo.m_iItemID );
	//if( pItemTemplet == NULL )
	//{
	//	START_LOG( cerr, L"아이템 정보가 존재하지 않습니다!" )
	//		<< BUILD_LOG( kItemInfo.m_iItemID )
	//		<< END_LOG;
	//	return false;
	//}

	//// 해당 아이템의 스탯
	//KStat kItemBaseStat;
	//kItemBaseStat.Init();
	//kItemBaseStat.m_iBaseHP    += (int)( pItemTemplet->m_Stat.m_fBaseHP );
	//kItemBaseStat.m_iAtkPhysic += (int)( pItemTemplet->m_Stat.m_fAtkPhysic );
	//kItemBaseStat.m_iAtkMagic  += (int)( pItemTemplet->m_Stat.m_fAtkMagic );
	//kItemBaseStat.m_iDefPhysic += (int)( pItemTemplet->m_Stat.m_fDefPhysic );
	//kItemBaseStat.m_iDefMagic  += (int)( pItemTemplet->m_Stat.m_fDefMagic );

	//// 강화 적용
	//float fEnchantRate;
	//if( SiCXSLEnchantItemManager()->GetEnchantRate( ( int )kItemInfo.m_cEnchantLevel, fEnchantRate ) == false )
	//{
	//	fEnchantRate = 1.f;
	//}

	//// BaseHP 스텟도 강화 적용 되게 변경
	//KStat kItemEnchantStat;
	//kItemEnchantStat.m_iBaseHP			= (int)( kItemBaseStat.m_iBaseHP	* fEnchantRate ) - kItemBaseStat.m_iBaseHP;
	//kItemEnchantStat.m_iAtkPhysic		= (int)( kItemBaseStat.m_iAtkPhysic	* fEnchantRate ) - kItemBaseStat.m_iAtkPhysic;
	//kItemEnchantStat.m_iAtkMagic		= (int)( kItemBaseStat.m_iAtkMagic	* fEnchantRate ) - kItemBaseStat.m_iAtkMagic;
	//kItemEnchantStat.m_iDefPhysic		= (int)( kItemBaseStat.m_iDefPhysic	* fEnchantRate ) - kItemBaseStat.m_iDefPhysic;
	//kItemEnchantStat.m_iDefMagic		= (int)( kItemBaseStat.m_iDefMagic	* fEnchantRate ) - kItemBaseStat.m_iDefMagic;

	//// 강화에 따른 아이템 레벨 정보 얻기
	//int iEnchantItemLevel = 0;
	//GetCaculatedItemLevelByEnchantStat( kItemEnchantStat, iEnchantItemLevel );
	\
	//// 아이템 레벨 합산 결과
	//iItemLevel = pItemTemplet->m_iItemLevel + iEnchantItemLevel;
	return true;
}

void CX2ItemManager::GetCaculatedItemLevelByEnchantStat( IN const KStat& kEnchantItemStat, OUT int& iItemLevel )
{
	iItemLevel = 0;

	iItemLevel += ( kEnchantItemStat.m_iBaseHP / m_kItemLevelEnchantData.m_iBaseHP );
	iItemLevel += ( kEnchantItemStat.m_iAtkPhysic / m_kItemLevelEnchantData.m_iAtkPhysic );
	iItemLevel += ( kEnchantItemStat.m_iAtkMagic / m_kItemLevelEnchantData.m_iAtkMagic );
	iItemLevel += ( kEnchantItemStat.m_iDefPhysic / m_kItemLevelEnchantData.m_iDefPhysic );
	iItemLevel += ( kEnchantItemStat.m_iDefMagic / m_kItemLevelEnchantData.m_iDefMagic );
}
//}}



bool CX2ItemManager::AddIceHeaterItemData_LUA( int iIceHeaterItemID_, int iItemID_, int eUnitType_ )
{
	if( iIceHeaterItemID_ <= 0 || iItemID_ <= 0 )
	{
		return false;
	}

	// 가열기 아이템 정보
	IceHeaterItemData IceHeaterData;
	IceHeaterData.m_iItemID = iItemID_;
	IceHeaterData.m_eUnitType = static_cast<CX2Unit::UNIT_TYPE>(eUnitType_);


	std::map< int, std::vector< IceHeaterItemData > >::iterator mit;
	mit = m_mapIceHeaterItemData.find( iIceHeaterItemID_ );
	if( mit == m_mapIceHeaterItemData.end() )
	{
		std::vector< IceHeaterItemData > vecIceHeaterItemData;
		vecIceHeaterItemData.push_back( IceHeaterData );
		m_mapIceHeaterItemData.insert( std::make_pair( iIceHeaterItemID_, vecIceHeaterItemData ) );
	}
	else
	{
		mit->second.push_back( IceHeaterData );
	}

	return true;
}
bool CX2ItemManager::GetIceHeaterItemData( int iIceHeaterItemID_, CX2Unit::UNIT_TYPE eUnitType_, OUT std::vector< int >& vecIceHeaterItemData_ )
{
	vecIceHeaterItemData_.clear();

	std::map< int, std::vector< IceHeaterItemData > >::const_iterator mit;
	mit = m_mapIceHeaterItemData.find( iIceHeaterItemID_ );
	if( mit == m_mapIceHeaterItemData.end() )
		return false;

	for( IceHeaterItemData ItemData : mit->second )
	{
		if( eUnitType_ == ItemData.m_eUnitType )
		{
			vecIceHeaterItemData_.push_back ( ItemData.m_iItemID );
		}
	}

	return true;
}



bool CX2ItemManager::AddPetPreViewData_LUA( int iCashShopItemID_, int iPetUnitID_, PREVIEW_PET_TYPE ePetType_ /*= PPT_PET*/)
{
#if X2_DEV_TOOLS
	// 오류 팝업 1회만 보여주도록 수정
	static bool bHasInvalidData = false;

	if( true == bHasInvalidData )
		return false;
#endif // _IN_HOUSE_

	if( iCashShopItemID_ <= 0 || iPetUnitID_ <= 0 )
	{
#if X2_DEV_TOOLS
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300),
								L"PetPreview.lua 오류 : 아이템ID 또는 펫 ID를 확인 해 주세요.",
								(CKTDXStage*)g_pMain->GetNowState() );

		bHasInvalidData = true;
#endif // _IN_HOUSE_
		return false;
	}

	// 패키지 정보
	m_vecPetPreViewData.push_back( PREVIEW_PET_DATA( iCashShopItemID_, iPetUnitID_, ePetType_ ) );

#if X2_DEV_TOOLS
	const CX2Item::ItemTemplet* pItemTemplet = GetItemTemplet( iCashShopItemID_ );
	if ( NULL == pItemTemplet )
	{
		WCHAR strTemp[255];
		StringCchPrintfW( strTemp, 255, L"PetPreview.lua 에러 : ItemTemplet 에서 %d ItemID 에 대한 정보를 찾을 수 없습니다.", iCashShopItemID_ );
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), strTemp, (CKTDXStage*)g_pMain->GetNowState() );

		bHasInvalidData = true;
	}

	// Pet Unit ID 유효성 체크
	bool bIsValidPetUnitID = true;
	if ( NULL != g_pData->GetPetManager() )
		bIsValidPetUnitID = (NULL != g_pData->GetPetManager()->GetPetTemplet ( static_cast <CX2PetManager::PET_UNIT_ID> ( iPetUnitID_ ) ));

	if( false == bIsValidPetUnitID && NULL != CX2RidingPetManager::GetInstance() )
		bIsValidPetUnitID = (NULL != CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(iPetUnitID_)));

	if( false == bIsValidPetUnitID )
	{
		WCHAR strTemp[255];
		StringCchPrintfW( strTemp, 255, L"PetPreview.lua 에러 : PetID %d에 대한 정보를 찾을 수 없습니다.", iPetUnitID_ );
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), strTemp, (CKTDXStage*)g_pMain->GetNowState() );

		bHasInvalidData = true;
	}
#endif // _IN_HOUSE_

	return true;
}
int CX2ItemManager::GetPreviewPetIdByItemID( int iItemID_ )
{
	return GetPreviewIDByItemID( iItemID_, PPT_PET );
}
int CX2ItemManager::GetPreviewRidingPetIdByItemID( int iItemID_ )
{
	return GetPreviewIDByItemID( iItemID_, PPT_RIDING_PET );
}
int	CX2ItemManager::GetPreviewIDByItemID( int iItemID_, PREVIEW_PET_TYPE ePetType_)
{
	if( true == m_vecPetPreViewData.empty() )
		return -1;

	std::vector<int> vecPetID;
	for( PREVIEW_PET_DATA PreviewData : m_vecPetPreViewData )
	{
		if( ePetType_ == PreviewData.ePreviewPetType )
		{

			if( iItemID_ == PreviewData.iCashShopItemID )
				vecPetID.push_back( PreviewData.iPreviewUnitID );
		}
	}

	if( true == vecPetID.empty() )
		return -1;

	int iPetIDIndex = rand() % vecPetID.size() ;
	return vecPetID[iPetIDIndex];
}


void CX2ItemManager::AddItemConvertInfo_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	// NPC ID 파싱
	CX2UnitManager::NPC_UNIT_ID eNpcUnitID = CX2UnitManager::NUI_NONE;
	LUA_GET_VALUE_ENUM(	luaManager, "m_ConvertNPC",	eNpcUnitID,	CX2UnitManager::NPC_UNIT_ID, CX2UnitManager::NUI_NONE );
	if( CX2UnitManager::NUI_NONE == eNpcUnitID )
	{
		eNpcUnitID = CX2UnitManager::NUI_ARIEL;
	}

	// 결과 아이템
	int iResultItemID = 0;
	if( luaManager.BeginTable( "m_ResultItemID" ) == true )
	{
		luaManager.GetValue( 1, iResultItemID);
		luaManager.EndTable();
	}

	if( 0 == iResultItemID )
		return;

	// 재료 아이템
	if( luaManager.BeginTable( "m_TargetItemID" ) == true )
	{
		std::map< int, ItemConvertList>::iterator it = m_mapNpcItemConvertList.find( static_cast<int>(eNpcUnitID));
		if( it != m_mapNpcItemConvertList.end() )
		{
			int index = 1;
			int buf;
			while( luaManager.GetValue(index++, buf) == true )
			{
				it->second.insert( std::make_pair(buf, iResultItemID) );
			}
			luaManager.EndTable();
		}
		else
		{
			int index = 1;
			int buf;
			ItemConvertList itConverList;
			while( luaManager.GetValue(index++, buf) == true )
			{
				itConverList.insert( std::make_pair(buf, iResultItemID) );
			}
			luaManager.EndTable();

			m_mapNpcItemConvertList.insert( std::make_pair( static_cast<int>(eNpcUnitID), itConverList ) );
		}
	}

	return;
}
int	CX2ItemManager::GetConvertResultItemID( int iTargetItemID_ )
{
	if( NULL != g_pTFieldGame )
	{
		int iNpcID = g_pTFieldGame->GetJoinNpcId();

		std::map< int, ItemConvertList>::iterator it = m_mapNpcItemConvertList.find(iNpcID);
		if( it != m_mapNpcItemConvertList.end() )
		{
			ItemConvertList::iterator itConverList = it->second.find( iTargetItemID_ );
			if( itConverList != it->second.end() )
				return itConverList->second;
		}
	}
	return 0;
}


bool CX2ItemManager::CheckKeepShowItem( int TempItemID , std::vector<int>& GetItemID )
{
	std::map<int,std::vector<int>>::iterator mit;
	mit = m_mapKeepShowItemList.find( TempItemID );
	if( mit != m_mapKeepShowItemList.end() )
	{
		GetItemID = mit->second;
		return true;
	}
	return false;
}

bool CX2ItemManager::CheckDisCountItemList( int TempProDuct ,bool& bIdexReset )
{
	int nCount = 0;
	std::map< int,kDisCountItemInfo >::iterator mit;
	for( mit = m_DisCountInfoMap.begin(); mit != m_DisCountInfoMap.end(); ++mit)
	{
		if( mit->second.m_DisCountProNo == TempProDuct )
		{
			for(int i = 0; i < mit->second.m_DisCountKeepItem.size(); ++i)
			{
				if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID(mit->second.m_DisCountKeepItem[i]) == NULL )
				{
					bIdexReset = true;
					return false;
				}
			}
			return true;
		}
		else if( mit->second.m_BasicProNo == TempProDuct )
		{
			for(int i = 0; i < mit->second.m_DisCountKeepItem.size(); ++i)
			{
				if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID(mit->second.m_DisCountKeepItem[i]) != NULL )
				{
					nCount++;
				}
			}
			if( nCount == mit->second.m_DisCountKeepItem.size() )
			{
				return false;
			}
			return true;
		}
	}
	return true;
}

bool CX2ItemManager::DisCountItemID_Find( int TempID, std::vector<int>& nTempList,int& nTempSale )
{
	std::map< int , kDisCountItemInfo >::iterator mit;
	mit = m_DisCountInfoMap.find( TempID );
	if( mit != m_DisCountInfoMap.end() )
	{
		nTempList = mit->second.m_DisCountKeepItem;
		nTempSale = mit->second.m_BasicProNo;
		return true;
	}
	return false;
}

int	CX2ItemManager::GetChangeCashPoint(int TempItemID)
{
	map<int, CX2ItemManager::CashItem* >::iterator mit;

	for ( mit = m_mapCashItem.begin(); mit != m_mapCashItem.end(); mit++ )
	{
		CX2ItemManager::CashItem* pCashItem = mit->second;
		if ( pCashItem == NULL )
			continue;
		if(pCashItem->GetGateProduct()->m_iProductNo == TempItemID)
		{
			return pCashItem->GetCash();
		}

	}
	return 0;
}
