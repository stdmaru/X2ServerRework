#include "./XSLPetManager.h"
#include "NetError.h"
#include "X2Data/XSLItemManager.h"
#include "X2Data/XSLUnit.h"


ImplementRefreshSingleton( CXSLPetManager );

CXSLPetManager::CXSLPetManager(void)
{
	//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
	m_fSpecialFeedIncreaseIntimacyRate = 0.0f;
	m_fSpecialFeedDecreaseIntimacyRate = 0.0f;
	//}}
}

CXSLPetManager::~CXSLPetManager(void)
{
}

ImplToStringW( CXSLPetManager )
{
	stm_	<< L"----------[ Pet Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapPetTemplet.size() )
			<< TOSTRINGW( m_mapPetCreateItem.size() )
			<< TOSTRINGW( m_mapPetCashFeedItem.size() )
			<< TOSTRINGW( m_setSpecialFeedItemID.size() )
			<< TOSTRINGWc( m_kNewPetInfo.m_cEvolutionStep )
			<< TOSTRINGW( m_kNewPetInfo.m_sSatiety )
			<< TOSTRINGW( m_kNewPetInfo.m_iIntimacy )
			<< TOSTRINGW( m_kNewPetInfo.m_sExtroversion )
			<< TOSTRINGW( m_kNewPetInfo.m_sEmotion )
			//{{ 2011. 04. 25	최육사	펫 포만도 소모량 변경
			<< TOSTRINGW( m_mapPetSatietyDecreaseFactor.size() )
			//}}
			<< TOSTRINGW( m_mapRandomPetCreateItem.size() )
			<< TOSTRINGW( m_mapTransformPetItem.size() )
			;

	return stm_;
}

ImplementLuaScriptParser( CXSLPetManager )
{
	lua_tinker::class_add<CXSLPetManager>( GetLuaState(), "CXSLPetManager" );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "SetNewPetInfo",					&CXSLPetManager::SetNewPetInfo_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddPetTemplet",					&CXSLPetManager::AddPetTemplet_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddPetCreateItemInfo",			&CXSLPetManager::AddPetCreateItemInfo_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddPetCashFeedItemInfo",			&CXSLPetManager::AddPetCashFeedItemInfo_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddSpecialFeedItemID",			&CXSLPetManager::AddSpecialFeedItemID_LUA );
	//{{ 2011. 04. 25	최육사	펫 포만도 소모량 변경
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddPetSatietyDecreaseFactor",	&CXSLPetManager::AddPetSatietyDecreaseFactor_LUA );
	//}}

	//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "IncreaseSpecialFeedIntimacyRate",		&CXSLPetManager::IncreaseSpecialFeedIntimacyRate_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "DecreaseSpecialFeedIntimacyRate",		&CXSLPetManager::DecreaseSpecialFeedIntimacyRate_LUA );
	//}}

	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddRandomPetCreateItemInfo",			&CXSLPetManager::AddRandomPetCreateItemInfo_LUA );
	lua_tinker::class_def<CXSLPetManager>( GetLuaState(), "AddTransformPetItemInfo",			&CXSLPetManager::AddTransformPetItemInfo_LUA );

	lua_tinker::decl( GetLuaState(), "g_pPetManager", this );
}

void CXSLPetManager::SetNewPetInfo_LUA( char cEvolutionStep, short sSatiety, int iIntimacy, short sExtroversion, short sEmotion )
{
	m_kNewPetInfo.m_cEvolutionStep	= cEvolutionStep;
	m_kNewPetInfo.m_sSatiety		= sSatiety;
	m_kNewPetInfo.m_iIntimacy		= iIntimacy;
	m_kNewPetInfo.m_sExtroversion	= sExtroversion;
	m_kNewPetInfo.m_sEmotion		= sEmotion;

	START_LOG( cout, L"Initializing pet creation values." )
		<< BUILD_LOGc( m_kNewPetInfo.m_cEvolutionStep )
		<< BUILD_LOG( m_kNewPetInfo.m_sSatiety )
		<< BUILD_LOG( m_kNewPetInfo.m_iIntimacy )
		<< BUILD_LOG( m_kNewPetInfo.m_sExtroversion )
		<< BUILD_LOG( m_kNewPetInfo.m_sEmotion );
}

bool CXSLPetManager::AddPetTemplet_LUA()
{
	KLuaManager luaManager( GetLuaState() );
	//TableBind( &luaManager );

	PetTemplet petData;

	LUA_GET_VALUE_RETURN_ENUM(	luaManager, L"PET_ID",					petData.m_PetId,		PET_UNIT_ID,	PUI_NONE,	goto end_proc );
	LUA_GET_VALUE_RETURN(		luaManager, L"COMMON_NAME",				petData.m_wstrCommonName,		L"",	goto end_proc );

	LUA_GET_VALUE_RETURN(		luaManager, L"CAPRICE",					petData.m_caprice,				0,		goto end_proc );
	LUA_GET_VALUE_RETURN(		luaManager, L"APPETITE",				petData.m_appetite,				0,		goto end_proc );

	LUA_GET_VALUE_RETURN(		luaManager, L"EVOLUTION_ALL_STEP",		petData.m_Evolution_All_Step,	0,		goto end_proc );

	LUA_GET_VALUE(		luaManager, L"FREE_AUTO_LOOTING",		petData.m_bFreeAutoLooting,		false );

	//{{ 2010. 10. 27	최육사	펫 오라 스킬
	if( luaManager.BeginTable( "PET_STATUS" ) == S_OK )
	{
		int tableIndex = 1;
		int iStatus = -1;
		while( luaManager.GetValue( tableIndex, iStatus ) == S_OK )
		{
			petData.m_PetStatus.push_back( iStatus );
			tableIndex++;
		}
		luaManager.EndTable();
	}
	//}}

	if( luaManager.BeginTable( "INVENSLOT_WIDTH_SIZE" ) == S_OK )
	{
		int tableIndex = 1;
		int iInvenSlotWidthSize = -1;
		while( luaManager.GetValue( tableIndex, iInvenSlotWidthSize ) == S_OK )
		{
			petData.m_InvenSlot_Width_Size.push_back( iInvenSlotWidthSize );
			tableIndex++;
		}
		luaManager.EndTable();
	}

	if( luaManager.BeginTable( "INVENSLOT_HEIGHT_SIZE" ) == S_OK )
	{
		int tableIndex = 1;
		int iInvenSlotHeightSize = -1;
		while( luaManager.GetValue( tableIndex, iInvenSlotHeightSize ) == S_OK )
		{
			petData.m_InvenSlot_Height_Size.push_back( iInvenSlotHeightSize );
			tableIndex++;
		}
		luaManager.EndTable();
	}

	if( luaManager.BeginTable( "EVOLUTION_STEP_MAX" ) == S_OK )
	{
		int tableIndex = 1;
		float fEvolutionStepMax = -1.f;
		while( luaManager.GetValue( tableIndex, fEvolutionStepMax ) == S_OK )
		{
			petData.m_Evolution_Step_Max.push_back( fEvolutionStepMax );
			tableIndex++;
		}
		luaManager.EndTable();
	}

	if( luaManager.BeginTable( "EVOLUTION_STEP_SCRIPTNAME" ) == S_OK )
	{
		int tableIndex = 1;
		std::wstring wstrScriptName;
		while( luaManager.GetValue( tableIndex, wstrScriptName ) == S_OK )
		{
			petData.m_Evolution_Step_ScriptName.push_back( wstrScriptName );
			tableIndex++;
		}
		luaManager.EndTable();
	}

	//{{ 2010. 10. 18	최육사	펫 오라 스킬
	if( luaManager.BeginTable( "AURA_SKILL_1" ) == S_OK )
	{
		int tableIndex = 1;
		//{{ 2011. 09. 28	최육사	펫 오라 스킬 오류 수정
		int iAuraSillOptionId = 0;
		//}}
		while( luaManager.GetValue( tableIndex, iAuraSillOptionId ) == S_OK )
		{
			LIF( iAuraSillOptionId > 0 );
			petData.m_vecAuraSkillOption[ASE_OPTION_1].push_back( iAuraSillOptionId );
			tableIndex++;
		}
		luaManager.EndTable();
	}
	if( luaManager.BeginTable( "AURA_SKILL_2" ) == S_OK )
	{
		int tableIndex = 1;
		//{{ 2011. 09. 28	최육사	펫 오라 스킬 오류 수정
		int iAuraSillOptionId = 0;
		//}}
		while( luaManager.GetValue( tableIndex, iAuraSillOptionId ) == S_OK )
		{
			LIF( iAuraSillOptionId > 0 );
			petData.m_vecAuraSkillOption[ASE_OPTION_2].push_back( iAuraSillOptionId );
			tableIndex++;
		}
		luaManager.EndTable();
	}
	if( luaManager.BeginTable( "AURA_SKILL_3" ) == S_OK )
	{
		int tableIndex = 1;
		//{{ 2011. 09. 28	최육사	펫 오라 스킬 오류 수정
		int iAuraSillOptionId = 0;
		//}}
		while( luaManager.GetValue( tableIndex, iAuraSillOptionId ) == S_OK )
		{
			LIF( iAuraSillOptionId > 0 );
			petData.m_vecAuraSkillOption[ASE_OPTION_3].push_back( iAuraSillOptionId );
			tableIndex++;
		}
		luaManager.EndTable();
	}
	//}}

	if( m_mapPetTemplet.find( petData.m_PetId) != m_mapPetTemplet.end() )
	{
		START_LOG( cerr ,L"Pet information is already registered." )
			<< BUILD_LOG( petData.m_PetId )
			<< END_LOG;

		goto end_proc;
	}

	m_mapPetTemplet.insert( std::make_pair( petData.m_PetId, petData ) );
	return true;

end_proc:
	START_LOG( cerr, L"Failed to parse the pet script." )
		<< BUILD_LOG( petData.m_PetId )
		<< END_LOG;

	return false;
}

bool CXSLPetManager::AddPetCreateItemInfo_LUA( int iItemID, int iPetID, short sPeriod )
{
	if( IsExistPetID( iPetID ) == false )
	{
		START_LOG( cerr, L"PetID is not registered." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	if( m_mapPetCreateItem.find( iItemID ) != m_mapPetCreateItem.end() )
	{
		START_LOG( cerr, L"ItemID is already registered." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	// 펫 인벤토리가 있는데 기간제라면 에러!
	if( HasPetInventory( iPetID ) == true && sPeriod != -1 )
	{
		START_LOG( cerr, L"Duration was assigned to a pet that has an inventory." )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( iPetID )
			<< BUILD_LOG( sPeriod )
			<< END_LOG;
		return false;
	}

	m_mapPetCreateItem.insert( std::map< int, std::pair<PET_UNIT_ID, short> >::value_type( iItemID, std::make_pair( static_cast<PET_UNIT_ID>( iPetID ), sPeriod ) ) );
	return true;
}

bool CXSLPetManager::AddRandomPetCreateItemInfo_LUA( int iItemID, int iPetID, float fRate )
{
	if( IsExistPetID( iPetID ) == false )
	{
		START_LOG( cerr, L"PetID is not registered." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	std::map< int, KLottery >::iterator mit;
	mit = m_mapRandomPetCreateItem.find( iItemID );
	if( mit != m_mapRandomPetCreateItem.end() )
	{
		mit->second.AddCase( iPetID, fRate );
	}
	else
	{
		KLottery kLot;
		kLot.AddCase( iPetID, fRate );
		m_mapRandomPetCreateItem.insert( std::make_pair( iItemID, kLot ) );
	}

	return true;
}

CXSLPetManager::PET_UNIT_ID CXSLPetManager::GetRandomPetIDByItemID( IN int iItemID )
{
	std::map< int, KLottery >::iterator mit;
	mit = m_mapRandomPetCreateItem.find( iItemID );
	if( mit == m_mapRandomPetCreateItem.end() )
	{
		START_LOG( cerr, L"Item is not a pet creation item." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return PET_UNIT_ID::PUI_NONE;
	}

	int iPetID = mit->second.Decision();
	if( iPetID == KLottery::CASE_BLANK )
	{
		START_LOG( cerr, L"Failed to get a random pet ID." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;

		return PET_UNIT_ID::PUI_NONE;
	}

	return (CXSLPetManager::PET_UNIT_ID)iPetID;
}

bool CXSLPetManager::AddTransformPetItemInfo_LUA( int iTransformID, int iPetCreateItemID, int iPetID, float fRate )
{
	if( IsExistPetID( iPetID ) == false )
	{
		START_LOG( cerr, L"PetID is not registered." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	std::map< int, TransformPetInfo >::iterator mit;
	mit = m_mapTransformPetItem.find( iTransformID );
	if( mit == m_mapTransformPetItem.end() )
	{
		TransformPetInfo kTransform;
		kTransform.m_iPetCreateItemID = iPetCreateItemID;
		kTransform.m_PetId = (CXSLPetManager::PET_UNIT_ID)iPetID;
		kTransform.m_fRate = fRate;
		m_mapTransformPetItem.insert( std::make_pair( iTransformID, kTransform ) );
	}
	else
	{
		START_LOG( cerr, L"Rare pets cannot be duplicated." )
			<< BUILD_LOG( iTransformID )
			<< BUILD_LOG( iPetCreateItemID )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	return true;
}

CXSLPetManager::PET_UNIT_ID CXSLPetManager::GetTransformPetID( IN const int iUsedItemID, IN const int iBeforePetID )
{
	std::map< int, TransformPetInfo >::iterator mit;
	mit = m_mapTransformPetItem.find( iUsedItemID );
	if( mit != m_mapTransformPetItem.end() )
	{
		TransformPetInfo kTransform = mit->second;
		std::map< int, KLottery >::iterator mit;
		mit = m_mapRandomPetCreateItem.find( kTransform.m_iPetCreateItemID );

		std::vector< int > vecCaseList;
		mit->second.GetCaseList( vecCaseList );

		// 안정성을 위해 int로 지정
		int iTotalRate = 100;
		int iRareRate = static_cast<int>(kTransform.m_fRate);
		int iNormalRate = ( iTotalRate - iRareRate ) / ( vecCaseList.size() - 2 );	// 딱 나누어 떨어져야 합니다!

		KLottery kLot;

		for( const int iCase : vecCaseList )
		{
			if( iCase == kTransform.m_PetId )
			{
				kLot.AddCase( kTransform.m_PetId, kTransform.m_fRate );
			}
			else if( iCase != iBeforePetID )
			{
				kLot.AddCase( iCase, iNormalRate );
			}
		}

		int iPetID = kLot.Decision();
		if( iPetID == KLottery::CASE_BLANK )
		{
			START_LOG( cerr, L"Failed to get a random pet ID." )
				<< BUILD_LOG( iPetID )
				<< END_LOG;

			return PET_UNIT_ID::PUI_NONE;
		}

		return (CXSLPetManager::PET_UNIT_ID)iPetID;
	}

	return PET_UNIT_ID::PUI_NONE;
}

bool CXSLPetManager::AddPetCashFeedItemInfo_LUA( int iItemID, short sIncreaseSatiety )
{
	if( m_mapPetCashFeedItem.find( iItemID ) != m_mapPetCashFeedItem.end() )
	{
		START_LOG( cerr, L"ItemID is already registered." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	m_mapPetCashFeedItem.insert( std::make_pair( iItemID, sIncreaseSatiety ) );
	return true;
}

bool CXSLPetManager::AddSpecialFeedItemID_LUA( int iItemID )
{
	if( m_setSpecialFeedItemID.find( iItemID ) != m_setSpecialFeedItemID.end() )
	{
		START_LOG( cerr, L"ItemID is already registered." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

    m_setSpecialFeedItemID.insert( iItemID );
	return true;
}

//{{ 2011. 04. 25	최육사	펫 포만도 소모량 변경
bool CXSLPetManager::AddPetSatietyDecreaseFactor_LUA( int iEvolutionStep, double fFactor )
{
	if( iEvolutionStep < 0  ||  fFactor < 0.0 )
	{
		START_LOG( cerr, L"Invalid pet fullness consumption setting." )
			<< BUILD_LOG( iEvolutionStep )
			<< BUILD_LOG( fFactor )
			<< END_LOG;
		return false;
	}

	m_mapPetSatietyDecreaseFactor.insert( std::make_pair( iEvolutionStep, fFactor ) );
	return true;
}
//}}

const CXSLPetManager::PetTemplet* CXSLPetManager::GetPetTemplet( IN int iPetID ) const
{
	std::map< PET_UNIT_ID, PetTemplet >::const_iterator mit;
	mit = m_mapPetTemplet.find( static_cast<PET_UNIT_ID>(iPetID) );
	if( mit == m_mapPetTemplet.end() )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;

		return NULL;
	}

	return &mit->second;
}

int	CXSLPetManager::GetPetInventorySlotSize( IN int iPetID, IN char cEvolutionStep ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return 0;
	}

    if( cEvolutionStep < 0  ||
		static_cast<u_int>(cEvolutionStep) >= pPetTemplet->m_InvenSlot_Width_Size.size()  ||
		static_cast<u_int>(cEvolutionStep) >= pPetTemplet->m_InvenSlot_Height_Size.size() )
	{
		START_LOG( cerr, L"Invalid evolution stage value." )
			<< BUILD_LOGc( cEvolutionStep )
			<< BUILD_LOG( pPetTemplet->m_InvenSlot_Width_Size.size() )
			<< BUILD_LOG( pPetTemplet->m_InvenSlot_Height_Size.size() )
			<< END_LOG;
		return 0;
	}

	return ( pPetTemplet->m_InvenSlot_Width_Size[cEvolutionStep] * pPetTemplet->m_InvenSlot_Height_Size[cEvolutionStep] );
}

void CXSLPetManager::GetPetIDByItemID( IN const int iItemID, OUT PET_UNIT_ID& ePetID, OUT short& sPeriod )
{
	ePetID = PUI_NONE;
	sPeriod = -1;

	std::map< int, std::pair<PET_UNIT_ID, short> >::const_iterator	mitPetCreateItem;
	mitPetCreateItem = m_mapPetCreateItem.find( iItemID );
	if( mitPetCreateItem == m_mapPetCreateItem.end() )
	{
		// 원래 있던 펫 생성 아이템들 검사하고 없으면 랜덤펫에서 검사
		ePetID = GetRandomPetIDByItemID( iItemID );
		if( ePetID != PUI_NONE )
			return;

		START_LOG( cerr, L"Item is not a pet creation item." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		sPeriod = 0;
		return;
	}

	ePetID = mitPetCreateItem->second.first;
	sPeriod = mitPetCreateItem->second.second;
}
bool CXSLPetManager::HasPetInventory( IN int iPetID ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	// 펫이 인벤토리를 갖고있는지 검사한다.
	bool bHasInventory = false;
	for( size_t iEvolutionStepIndex = 0; iEvolutionStepIndex < pPetTemplet->m_InvenSlot_Width_Size.size() && iEvolutionStepIndex < pPetTemplet->m_InvenSlot_Height_Size.size(); ++iEvolutionStepIndex )
	{
		if( pPetTemplet->m_InvenSlot_Height_Size[iEvolutionStepIndex] != 0 || pPetTemplet->m_InvenSlot_Width_Size[iEvolutionStepIndex] != 0 )
		{
			bHasInventory = true;
			break;
		}
	}

	return bHasInventory;
}


bool CXSLPetManager::GetCashPetFeedItemInfo( IN int iItemID, OUT short& sIncreaseSatiety )
{
	std::map< int, short >::const_iterator mit;
	mit = m_mapPetCashFeedItem.find( iItemID );
	if( mit == m_mapPetCashFeedItem.end() )
	{
		START_LOG( cwarn, L"Item is not pet food." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	sIncreaseSatiety = mit->second;
	return true;
}

bool CXSLPetManager::GetEvolutionStepMaxIntimacy( IN int iPetID, IN char cEvolutionStep, OUT int& iMaxIntimacy ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	if( cEvolutionStep < 0 )
	{
		START_LOG( cerr, L"Invalid evolution stage value." )
			<< BUILD_LOG( iPetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< END_LOG;
		return false;
	}

    if( pPetTemplet->m_Evolution_Step_Max.size() <= static_cast<u_int>(cEvolutionStep) )
	{
		START_LOG( cerr, L"Evolution stage does not exist for this PetID." )
			<< BUILD_LOG( iPetID )
			<< BUILD_LOG( pPetTemplet->m_Evolution_Step_Max.size() )
			<< BUILD_LOGc( cEvolutionStep )
			<< END_LOG;
		return false;
	}

	iMaxIntimacy = static_cast<int>(pPetTemplet->m_Evolution_Step_Max[cEvolutionStep]);
	return true;
}

bool CXSLPetManager::CheckNextEvolution( IN int iPetID, IN char cCurrentEvolutionStep, OUT char& cNextEvolutionStep ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	if( 0 > cCurrentEvolutionStep  ||  cCurrentEvolutionStep > pPetTemplet->m_Evolution_All_Step )
	{
		START_LOG( cerr, L"Invalid current evolution stage." )
			<< BUILD_LOG( iPetID )
			<< BUILD_LOGc( cCurrentEvolutionStep )
			<< END_LOG;
		return false;
	}

	if( cCurrentEvolutionStep == pPetTemplet->m_Evolution_All_Step )
	{
		START_LOG( clog, L"Evolution stage cannot be evolved further." )
			<< BUILD_LOG( iPetID )
			<< BUILD_LOGc( cCurrentEvolutionStep )
			<< END_LOG;
		return false;
	}

	cNextEvolutionStep = cCurrentEvolutionStep + 1; // 1 증가한 값으로 진화한다!
	return true;
}

bool CXSLPetManager::CalcIncreaseSatietyForFeed( IN int iPetID, IN int iFeedItemID, IN char cLevel, OUT short& sIncreaseSatiety ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOGc( iPetID )
			<< END_LOG;
		return false;
	}

	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iFeedItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Item does not exist." )
			<< BUILD_LOG( iFeedItemID )
			<< END_LOG;
		return false;
	}

	double fEquipItemFactor = 0;
	switch( pItemTemplet->m_EqipPosition )
	{
	case CXSLUnit::EP_WEAPON_HAND:	fEquipItemFactor = 480.0;	break;
	case CXSLUnit::EP_DEFENCE_BODY:	fEquipItemFactor = 144.0;	break;
	case CXSLUnit::EP_DEFENCE_LEG:	fEquipItemFactor = 168.0;	break;
	case CXSLUnit::EP_DEFENCE_HAND:	fEquipItemFactor = 72.0;	break;
	case CXSLUnit::EP_DEFENCE_FOOT:	fEquipItemFactor = 96.0;	break;
	default:
		{
			START_LOG( cerr, L"Item cannot be fed to a pet." )
				<< BUILD_LOG( iFeedItemID )
				<< BUILD_LOG( pItemTemplet->m_EqipPosition )
				<< END_LOG;
		}
		return false;
	}

	//sIncreaseSatiety = static_cast<short>( ( static_cast<double>(iItemPrice) * 0.0063 ) / ( 1.0 + ( static_cast<double>(cLevel) - 1.0 ) * 0.5 ) * static_cast<double>(pPetTemplet->m_appetite) );
	//sIncreaseSatiety = static_cast<short>( ( ( ( static_cast<double>(iItemPrice) - ( 2400.0 * static_cast<double>(cLevel) ) ) / 48.0 ) + 480.0 ) * 0.2 * static_cast<double>(pPetTemplet->m_appetite) ); // 수정된 공식

	sIncreaseSatiety = static_cast<short>( ( ( ( static_cast<double>(pItemTemplet->m_Price) - ( 5.0 * fEquipItemFactor * static_cast<double>(cLevel) ) ) / 20.0 ) + fEquipItemFactor ) * 0.4 * static_cast<double>(pPetTemplet->m_appetite) ); // 수정된 공식
	if( sIncreaseSatiety < 0 )
	{
		sIncreaseSatiety = 0;
	}
	return true;
}

bool CXSLPetManager::CheckExtroversion( IN int iPetID, IN short sCurExtroversion, IN int iCumulativeIntimacy, OUT short& sChangeExtroversion ) const
{
	// 로그아웃을 하거나 펫을 소환 해제할때 체크함수를 호출합니다!

	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	//{{edit : 장훈
	//if( sCurExtroversion < PET_ENUM::PE_MIN_EMOTION  ||  sCurExtroversion > PET_ENUM::PE_MAX_EMOTION )
	if( sCurExtroversion < PE_MIN_EXTROVERSION  ||  sCurExtroversion > PE_MAX_EXTROVERSION )
	{
		START_LOG( cerr, L"Invalid sociability value." )
			<< BUILD_LOG( sCurExtroversion )
			<< END_LOG;
	}
	//}}edit : 장훈

	if( iCumulativeIntimacy < 0 )
	{
		// +15 * 펫 성향 상수
		sChangeExtroversion = static_cast<short>( 15.0 * pPetTemplet->m_caprice );
	}
	else if( iCumulativeIntimacy == 0 )
	{
		// 변화없음
		sChangeExtroversion = 0;
	}
	else if( iCumulativeIntimacy > 0  &&  iCumulativeIntimacy < 9540 )
	{
		// +15 * 펫 성향 상수
		sChangeExtroversion = static_cast<short>( 15.0 * pPetTemplet->m_caprice );
	}
	else if( iCumulativeIntimacy >= 9540  &&  iCumulativeIntimacy < 19080 )
	{
		// 변화없음
		sChangeExtroversion = 0;
	}
	else
	{
		// -10 * 펫 성향 상수
		sChangeExtroversion = static_cast<short>( -15.0 * pPetTemplet->m_caprice );
	}

	return true;
}

bool CXSLPetManager::CheckEmotion( IN int iPetID, IN short sCurEmotion, IN int iCumulativeSatiety, IN int iFeedCount, OUT short& sChangeEmotion ) const
{
	// 펫에게 먹이를 줄때마다 변경되는 포만도를 평균내어 로그아웃 하거나 펫을 소환할때 변경!

	const PetTemplet* pPetTemplet = GetPetTemplet( iPetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	if( sCurEmotion < PE_MIN_EMOTION  ||  sCurEmotion > PE_MAX_EMOTION )
	{
		START_LOG( cerr, L"Invalid sensitivity value." )
			<< BUILD_LOG( sCurEmotion )
			<< END_LOG;
	}

	if( iFeedCount == 0 )
	{
		// 먹이를 준적이 없다! 그냥 패스!
		return true;
	}

	const int iAverageIncreaseSatiety = iCumulativeSatiety / iFeedCount;

	if( iAverageIncreaseSatiety <= 0 )
	{
		// 변화없음
		sChangeEmotion = 0;
	}
	else if( iAverageIncreaseSatiety > 0  &&  iAverageIncreaseSatiety < 200 )
	{
		// +15 * 펫 성향 상수
		sChangeEmotion = static_cast<short>( 15.0 * pPetTemplet->m_caprice );
	}
	else if( iAverageIncreaseSatiety >= 200  &&  iAverageIncreaseSatiety < 400 )
	{
		// 변화없음
		sChangeEmotion = 0;
	}
	else
	{
		// -15 * 펫 성향 상수
		sChangeEmotion = static_cast<short>( -15.0 * pPetTemplet->m_caprice );
	}

	return true;
}

bool CXSLPetManager::MakeNewPetInfo( IN PET_UNIT_ID ePetID, IN const std::wstring& wstrPetName, OUT KPetInfo& kNewPetInfo ) const
{
	if( IsExistPetID( ePetID ) == false )
	{
		START_LOG( cerr, L"Pet does not exist." )
			<< BUILD_LOG( ePetID )
			<< END_LOG;
		return false;
	}

	CStringW strTime = static_cast<CStringW>(CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")));
	std::wstring wstrTimeWhenPetIsCreated = strTime.GetString();


	// DB에서 생성할 펫 정보를 세팅하자!
	kNewPetInfo.m_iPetID				= static_cast<int>(ePetID);
	kNewPetInfo.m_wstrPetName			= wstrPetName;
	kNewPetInfo.m_cEvolutionStep		= m_kNewPetInfo.m_cEvolutionStep;
	kNewPetInfo.m_sSatiety				= m_kNewPetInfo.m_sSatiety;
	kNewPetInfo.m_iIntimacy				= m_kNewPetInfo.m_iIntimacy;
	kNewPetInfo.m_sExtroversion			= m_kNewPetInfo.m_sExtroversion;
	kNewPetInfo.m_sEmotion				= m_kNewPetInfo.m_sEmotion;

	kNewPetInfo.m_wstrLastFeedDate		= wstrTimeWhenPetIsCreated;
	kNewPetInfo.m_wstrLastSummonDate	= wstrTimeWhenPetIsCreated;
	kNewPetInfo.m_wstrRegDate			= wstrTimeWhenPetIsCreated;
	kNewPetInfo.m_bFreeAutoLooting		= IsFreeAutoLootingPet( kNewPetInfo.m_iPetID );

	return true;
}

//{{ 2010. 10. 18	최육사	펫 오라 스킬
//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
bool CXSLPetManager::GetAuraSkillOption( IN PET_UNIT_ID ePetID, IN char cEvolutionStep, OUT std::vector< int >& vecAuraSkillOption )
//}}
{
	vecAuraSkillOption.clear();

	const PetTemplet* pPetTemplet = GetPetTemplet( ePetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( ePetID )
			<< END_LOG;
		return false;
	}

	if( cEvolutionStep < 0  ||  static_cast<u_int>(cEvolutionStep) >= pPetTemplet->m_PetStatus.size() )
	{
		START_LOG( cerr, L"No state value exists for this evolution stage." )
			<< BUILD_LOG( ePetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< END_LOG;
		return false;
	}

	const int iPetStatus = pPetTemplet->m_PetStatus[cEvolutionStep];

	switch( iPetStatus )
	{
	case 3:
		{
			vecAuraSkillOption.insert( vecAuraSkillOption.end(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_3].begin(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_3].end() );
		}
		[[fallthrough]]; // no break: on to the next evolution step

	case 2:
		{
			vecAuraSkillOption.insert( vecAuraSkillOption.end(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_2].begin(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_2].end() );
		}
		[[fallthrough]]; // no break: on to the next evolution step

	case 1:
		{
			vecAuraSkillOption.insert( vecAuraSkillOption.end(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_1].begin(),
				pPetTemplet->m_vecAuraSkillOption[CXSLPetManager::ASE_OPTION_1].end() );
		}
	}

	return true;
}

//}}

//{{ 2011. 04. 25	최육사	펫 포만도 소모량 변경
short CXSLPetManager::CalcDecreaseSatietyValueOnDungeonRoom( IN const PET_UNIT_ID ePetID, IN const char cEvolutionStep, IN const short sSubStageExpNpcNum ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( ePetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( ePetID )
			<< END_LOG;
		return 0;
	}

	if( cEvolutionStep < 0  ||  static_cast<u_int>(cEvolutionStep) >= pPetTemplet->m_PetStatus.size() )
	{
		START_LOG( cerr, L"No state value exists for this evolution stage." )
			<< BUILD_LOG( ePetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< END_LOG;
		return 0;
	}

	const int iPetStatus = pPetTemplet->m_PetStatus[cEvolutionStep];

	std::map< int, double >::const_iterator mit;
	mit = m_mapPetSatietyDecreaseFactor.find( iPetStatus );
	if( mit == m_mapPetSatietyDecreaseFactor.end() )
	{
		START_LOG( cerr, L"Evolution grade is not registered." )
			<< BUILD_LOG( ePetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< BUILD_LOG( iPetStatus )
			<< END_LOG;
        return 0;
	}

	const double fSatietyFactor = mit->second;

	return static_cast<short>( 30.0 * fSatietyFactor /  static_cast<double>(sSubStageExpNpcNum) ); // 수정된 공식
}

short CXSLPetManager::CalcDecreaseSatietyValueOnPvpRoom( IN const PET_UNIT_ID ePetID, IN const char cEvolutionStep ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet( ePetID );
	if( pPetTemplet == NULL )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( ePetID )
			<< END_LOG;
		return 0;
	}

	if( cEvolutionStep < 0  ||  static_cast<u_int>(cEvolutionStep) >= pPetTemplet->m_PetStatus.size() )
	{
		START_LOG( cerr, L"No state value exists for this evolution stage." )
			<< BUILD_LOG( ePetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< END_LOG;
		return 0;
	}

	const int iPetStatus = pPetTemplet->m_PetStatus[cEvolutionStep];

	std::map< int, double >::const_iterator mit;
	mit = m_mapPetSatietyDecreaseFactor.find( iPetStatus );
	if( mit == m_mapPetSatietyDecreaseFactor.end() )
	{
		START_LOG( cerr, L"Evolution grade is not registered." )
			<< BUILD_LOG( ePetID )
			<< BUILD_LOGc( cEvolutionStep )
			<< BUILD_LOG( iPetStatus )
			<< END_LOG;
		return 0;
	}

	const double fSatietyFactor = mit->second;

	return static_cast<short>( 30.0 * fSatietyFactor / 2.5 ); // 수정된 공식
}
//}}

bool CXSLPetManager::IsValidCommand( IN PET_ACTION_COMMAND eCommand )
{
	if( PAC_NONE < eCommand  &&  PAC_END > eCommand )
		return true;

    return false;
}

bool CXSLPetManager::IsUnconditionalCommand( IN PET_ACTION_COMMAND eCommand )
{
	switch( eCommand )
	{
	case PAC_SIT_END:
	case PAC_SLEEP_END:
	case PAC_FORCE_SLEEP:
		return true;
	default:
		break;
	}

	return false;
}

//{{ 2012. 04. 26	박세훈	펫 장난감
int CXSLPetManager::CalcChangeIntimacyValue( IN CXSLRoom::ROOM_TYPE eRoomType, IN char cUserCount, IN short sSatiety, IN const bool& bHaveToy )
//}}
{
	double fIntimacyFactor = 0.0;

	if( 0 <= sSatiety  &&  sSatiety <= 960 )
	{
		//{{ 2012. 04. 26	박세훈	펫 장난감
		if( bHaveToy == true )	{	fIntimacyFactor = -0.05;	}
		else					{	fIntimacyFactor = -0.1;		}
		//}}
	}
	else if( 960 < sSatiety  &&  sSatiety <= 1920 )
	{
		//{{ 2012. 04. 26	박세훈	펫 장난감
		if( bHaveToy == true )	{	fIntimacyFactor = -0.025;	}
		else					{	fIntimacyFactor = -0.05;	}
		//}}
	}
	else if( 1920 < sSatiety  &&  sSatiety <= 2880 )
	{
		fIntimacyFactor = 0.0;
	}
	else if( 2880 < sSatiety  &&  sSatiety <= 3840 )
	{
		//{{ 2012. 04. 26	박세훈	펫 장난감
		if( bHaveToy == true )	{	fIntimacyFactor = 0.02;	}
		else					{	fIntimacyFactor = 0.01;	}
		//}}
	}
	else if( 3840 < sSatiety  &&  sSatiety <= PE_MAX_SATIETY )
	{
		//{{ 2012. 04. 26	박세훈	펫 장난감
		if( bHaveToy == true )	{	fIntimacyFactor = 0.04;	}
		else					{	fIntimacyFactor = 0.02;	}
		//}}
	}
	else
	{
		START_LOG( cerr, L"Invalid pet fullness value." )
			<< BUILD_LOG( sSatiety )
			<< END_LOG;
		return 0;
	}

	int iCalcChangeIntimacyValue = 0;

	switch( eRoomType )
	{
	case CXSLRoom::RT_PVP:
		{
			iCalcChangeIntimacyValue = static_cast<int>( static_cast<double>( 21 - cUserCount ) / 2.5 );
		}
		break;

	case CXSLRoom::RT_DUNGEON:
		{
			if( 0 <= sSatiety  &&  sSatiety <= 1920 )
			{
				iCalcChangeIntimacyValue = static_cast<int>( static_cast<double>( PE_MAX_SATIETY - sSatiety ) * fIntimacyFactor );
			}
			else if( 1920 < sSatiety  &&  sSatiety <= PE_MAX_SATIETY )
			{
				iCalcChangeIntimacyValue = static_cast<int>( static_cast<double>(sSatiety) * fIntimacyFactor );
			}
		}
		break;

	default:
		{
			START_LOG( cerr, L"Attempted to calculate affinity outside a PvP or dungeon room." )
				<< BUILD_LOG( eRoomType )
				<< END_LOG;
			return 0;
		}
		break;
	}

	return iCalcChangeIntimacyValue;
}

//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
void CXSLPetManager::IncreaseSpecialFeedIntimacyRate_LUA( IN float fIncreaseRate )
{
	if( fIncreaseRate > 1.0f || fIncreaseRate < 0.0f ) // the original used && (never true)
	{
		START_LOG( cerr, L"Invalid special pet food affinity increase value." )
			<< BUILD_LOG( fIncreaseRate )
			<< END_LOG;

		return;
	}

	m_fSpecialFeedIncreaseIntimacyRate = fIncreaseRate;
}

void CXSLPetManager::DecreaseSpecialFeedIntimacyRate_LUA( IN float fDecreaseRate )
{
	if( fDecreaseRate > 1.0f || fDecreaseRate < 0.0f ) // the original used && (never true)
	{
		START_LOG( cerr, L"Invalid special pet food affinity decrease value." )
			<< BUILD_LOG( fDecreaseRate )
			<< END_LOG;

		return;
	}

	m_fSpecialFeedDecreaseIntimacyRate = fDecreaseRate;
}
//}}

//{{ 2012. 12. 26	박세훈	펫 오토 루팅 기능 추가
bool CXSLPetManager::IsEvolutionExceptionPet( IN const int iPetID ) const
{
	std::map< PET_UNIT_ID, PetTemplet >::const_iterator it = m_mapPetTemplet.find( static_cast<PET_UNIT_ID>( iPetID ) );
	if( it == m_mapPetTemplet.end() )
	{
		START_LOG( cerr, L"PetID does not exist." )
			<< BUILD_LOG( iPetID )
			<< END_LOG;
		return false;
	}

	return ( it->second.m_PetStatus.size() == 1 ) && ( it->second.m_PetStatus.front() == 3 );
}
//}}

bool CXSLPetManager::IsFreeAutoLootingPet( IN const int iPetID_ ) const
{
	const PetTemplet* pPetTemplet = GetPetTemplet(iPetID_);
	if( pPetTemplet != NULL )
		return pPetTemplet->m_bFreeAutoLooting;
	else
		return false;
}
