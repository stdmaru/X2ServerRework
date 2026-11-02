#include "./XSLRandomItemManager.h"
#include "NetError.h"
#include "XSLItemManager.h"


ImplementRefreshSingleton( CXSLRandomItemManager );

CXSLRandomItemManager::CXSLRandomItemManager(void) :
m_bIsNotifyMsg( false )
{
}

CXSLRandomItemManager::~CXSLRandomItemManager(void)
{
}

ImplToStringW( CXSLRandomItemManager )
{
	stm_	<< L"----------[ Random Item Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapRandomItemKeyTable.size() )
			<< TOSTRINGW( m_mapRandomItem.size() )
			<< TOSTRINGW( m_mapItemGroup.size() )
			<< TOSTRINGW( m_mapAttractionItem.size() )
			//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브
			<< TOSTRINGW( m_setSealRandomItem.size() )
			<< TOSTRINGW( m_setNotifyResultItem.size() )
			//}}
			<< TOSTRINGW( m_bIsNotifyMsg )
			;

	return stm_;
}

ImplementLuaScriptParser( CXSLRandomItemManager )
{
	lua_tinker::class_add<CXSLRandomItemManager>( GetLuaState(), "CXSLRandomItemManager" );
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddRandomItemTemplet",	&CXSLRandomItemManager::AddRandomItemTemplet_LUA );
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddRandomItemGroup",		&CXSLRandomItemManager::AddRandomItemGroup_LUA );

	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddRandomItemMappingData",&CXSLRandomItemManager::AddRandomItemMappingData_LUA );
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddRandomItemMappingTime",&CXSLRandomItemManager::AddRandomItemMappingTime_LUA );

	//{{ 2009. 6. 22  최육사	매력아이템 리스트
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "SetNotifyMsg",			&CXSLRandomItemManager::SetNotifyMsg_LUA );
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddAttractionItemInfo",	&CXSLRandomItemManager::AddAttractionItemInfo_LUA );
	//}}
	//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddSealRandomItemInfo",	&CXSLRandomItemManager::AddSealRandomItemInfo_LUA );
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddNotifyResultItemInfo",	&CXSLRandomItemManager::AddNotifyResultItemInfo_LUA );
	//}}
	//{{ 2013. 3. 16	박세훈	매력 아이템 스크립트 처리
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "AddCharmItem",			&CXSLRandomItemManager::AddCharmItem_LUA );
	//}}
	lua_tinker::class_def<CXSLRandomItemManager>( GetLuaState(), "dump",					&CXSLRandomItemManager::Dump );

	lua_tinker::decl( GetLuaState(), "g_pRandomItemManager", this );
}

bool CXSLRandomItemManager::AddRandomItemTemplet_LUA()
{
	KLuaManager luaManager( GetLuaState() );
	//TableBind( &luaManager );

	RandomItemData randomItemData;

	int iTemp = 0;
	LUA_GET_VALUE_RETURN(	luaManager, L"m_ItemID",				randomItemData.m_ItemID,					0,	goto end_proc );
	//{{ 2011. 12. 14	최육사	서버 에러 로그 버그 수정
	if( SiCXSLItemManager()->GetItemTemplet( randomItemData.m_ItemID ) == NULL )
	{
		START_LOG( cwarn, L"Item is not registered in Item.lua and cannot be included in the random cube table." )
			<< BUILD_LOG( randomItemData.m_ItemID );
		return false;
	}
	//}}
	LUA_GET_VALUE(			luaManager, L"m_KeyItemID",				randomItemData.m_KeyItemID,					0 );
	LUA_GET_VALUE(			luaManager, L"m_RequiredKeyCount",		randomItemData.m_RequiredKeyCount,			0 );

	LUA_GET_VALUE(			luaManager, L"m_iRessurectionCount",	randomItemData.m_iRessurectionCount,		0 );
	LUA_GET_VALUE(			luaManager, L"m_iRestoreSpirit",		randomItemData.m_iRestoreSpirit,			0 );

	//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
	LUA_GET_VALUE(			luaManager, L"m_iRequiredED",			randomItemData.m_iRequiredED,			0 );
	//}

	LUA_GET_VALUE(			luaManager, L"m_bGiveAll",			iTemp,	0 );
	if( iTemp == 0 )
		randomItemData.m_bGiveAll = false;
	else
		randomItemData.m_bGiveAll = true;

	//{{ 2012. 10. 16	박세훈	RandomBoxTable 구조 변경
	LUA_GET_VALUE(			luaManager, L"m_UseCondition",		iTemp,	0 );
	randomItemData.m_byteUseCondition	= static_cast<byte>( iTemp );
	if( ( randomItemData.m_byteUseCondition <= CXSLItem::UC_NONE ) || ( CXSLItem::UC_ONE_CLASS < randomItemData.m_byteUseCondition ) )
	{
		START_LOG( cerr, L"Invalid USE_CONDITION value." )
			<< BUILD_LOG( randomItemData.m_byteUseCondition )
			<< END_LOG;
		goto end_proc;
	}
	//}}

	//{{  2012.09.13 조효진	  신 기획 툴 RandomBox 출력 수정 변경에 따른 코드 수정
	// 신툴 개편후 부활석 큐브와 근성 포션 큐브는 UnitClass, GroupId 정보 출력 안됨

	// 부활석 큐브 처리
	if (randomItemData.m_iRessurectionCount > 0)
	{
		// Default 값 0,0 으로 세팅
		RandomUnitData unitData;
		unitData.m_cUnitClass = 0;
		unitData.m_iItemGroupID = 0;

		// 부활석 큐브
		randomItemData.m_vecUnitData.push_back( unitData );
	}
	// 근성 포션
	else if (randomItemData.m_iRestoreSpirit > 0)
	{
		// Default 값 0,0 으로 세팅
		RandomUnitData unitData;
		unitData.m_cUnitClass = 0;
		unitData.m_iItemGroupID = 0;

		// 근성도 큐브
		randomItemData.m_vecUnitData.push_back( unitData );
	}
	else
	{
		int index = 1;
		while( luaManager.BeginTable( index ) == S_OK )
		{
			RandomUnitData unitData;
			LUA_GET_VALUE(	luaManager, L"m_cUnitClass",	iTemp,		0 );
			unitData.m_cUnitClass = static_cast<char>(iTemp);

			//{{ 2012. 10. 16	박세훈	RandomBoxTable 구조 변경
			LUA_GET_VALUE(	luaManager, L"m_iItemGroupID",	unitData.m_iItemGroupID,	0 );

			if( randomItemData.m_byteUseCondition == CXSLItem::UC_ANYONE )
			{
				if( unitData.m_cUnitClass != CXSLUnit::UC_NONE )
				{
					START_LOG( cerr, L"Configured m_UseCondition and m_cUnitClass values are inconsistent.")
						<< BUILD_LOG( randomItemData.m_byteUseCondition )
						<< BUILD_LOG( unitData.m_cUnitClass )
						<< END_LOG;
					goto end_proc;
				}
			}
			else if( randomItemData.m_byteUseCondition == CXSLItem::UC_ONE_UNIT )
			{
				if( ( unitData.m_cUnitClass != CXSLUnit::GetUnitClassBaseGrade( static_cast<CXSLUnit::UNIT_CLASS>( unitData.m_cUnitClass ) ) ) ||
					( unitData.m_iItemGroupID == 0 ) )
				{
					START_LOG( cerr, L"Invalid cube." )
						<< BUILD_LOG( randomItemData.m_byteUseCondition )
						<< BUILD_LOGc( unitData.m_cUnitClass )
						<< BUILD_LOG( unitData.m_iItemGroupID )
						<< END_LOG;
					goto end_proc;
				}
			}
			else if( ( CXSLUnit::IsValidUnitClass( static_cast<CXSLUnit::UNIT_CLASS>( unitData.m_cUnitClass ) ) != true ) ||
					 ( unitData.m_iItemGroupID == 0 ) )
			{
				START_LOG( cerr, L"Invalid cube." )
					<< BUILD_LOG( randomItemData.m_byteUseCondition )
					<< BUILD_LOG( unitData.m_cUnitClass )
					<< BUILD_LOG( unitData.m_iItemGroupID )
					<< END_LOG;
				goto end_proc;
			}

			// 일반 큐브
			randomItemData.m_vecUnitData.push_back( unitData );
			//}}

			index++;
			luaManager.EndTable();
		}
	}

	if (randomItemData.m_vecUnitData.size() <= 0)
	{
		START_LOG( cerr, L"Invalid cube: m_vecUnitData is empty." )
			<< BUILD_LOG( randomItemData.m_ItemID )
			<< BUILD_LOG( randomItemData.m_KeyItemID )
			<< BUILD_LOG( randomItemData.m_RequiredKeyCount )
			<< BUILD_LOG( randomItemData.m_iRessurectionCount )
			<< BUILD_LOG( randomItemData.m_iRestoreSpirit )
			<< END_LOG;

		goto end_proc;
	}

	//}}

	if( randomItemData.m_iRessurectionCount < 0 )
		goto end_proc;

	// 이미 등록된 랜덤 테이블인지 검사한다!
	if( GetRandomItemData( randomItemData.m_ItemID, randomItemData.m_KeyItemID ) != NULL )
	{
		START_LOG( cerr, L"Random item table is already registered; duplicate data found." )
			<< BUILD_LOG( randomItemData.m_ItemID )
			<< BUILD_LOG( randomItemData.m_KeyItemID )
			<< END_LOG;

		goto end_proc;
	}

	//{{ 2009. 12. 5  최육사	랜덤큐브시스템개편
	{
		RandomItemKey kRandomItemKey( randomItemData.m_ItemID, randomItemData.m_KeyItemID );
		m_mapRandomItem.insert( std::make_pair( kRandomItemKey, randomItemData ) );
	}

	{
		std::map< int, std::map< int, int > >::iterator mitKey = m_mapRandomItemKeyTable.find( randomItemData.m_ItemID );
		if( mitKey == m_mapRandomItemKeyTable.end() )
		{
			std::map< int, int  > mapKeyInfo;
			mapKeyInfo.insert( std::make_pair( randomItemData.m_KeyItemID, randomItemData.m_RequiredKeyCount ) );
			m_mapRandomItemKeyTable.insert( std::make_pair( randomItemData.m_ItemID, mapKeyInfo ) );
		}
		else
		{
			mitKey->second.insert( std::make_pair( randomItemData.m_KeyItemID, randomItemData.m_RequiredKeyCount ) );
		}
	}
	//}}

	return true;

end_proc:
	START_LOG( cerr, L"Failed to parse random box data." )
		<< BUILD_LOG( randomItemData.m_ItemID )
		<< END_LOG;

	return false;
}

bool CXSLRandomItemManager::AddRandomItemGroup_LUA( int iGroupID, int iItemID, float fRate, int iPeriod, int iQuantity )
{
	//if( fRate == 0.0f )
	//{
	//	START_LOG( clog, L"기획의도에 의해 0%로 설정된 데이터이므로 무시하고 넘어간다." )
	//		<< BUILD_LOG( iGroupID )
	//		<< BUILD_LOG( iItemID )
	//		<< BUILD_LOG( iPeriod )
	//		<< BUILD_LOG( iQuantity );

	//	return true;
	//}

	if( iGroupID <= 0 || iItemID <= 0 || fRate <= 0.0f || iPeriod < 0  || iQuantity < 0 )
	{
		START_LOG( cerr, L"Failed to parse random item group data." )
			<< BUILD_LOG( iGroupID )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( fRate )
			<< BUILD_LOG( iPeriod )
			<< BUILD_LOG( iQuantity )
			<< END_LOG;

		return false;
	}

	ItemGroupData itemData;
	itemData.m_iItemID		= iItemID;
	itemData.m_fRate		= fRate;
	itemData.m_iPeriod		= iPeriod;
	itemData.m_iQuantity	= iQuantity;

	std::map< int, ItemGroupDataList >::iterator mit;
	mit = m_mapItemGroup.find( iGroupID );

	if( mit != m_mapItemGroup.end() )
	{
		mit->second.m_vecItemData.push_back( itemData );
		if( mit->second.m_kLottery.AddCaseIntegerCast( iItemID, fRate, iPeriod, iQuantity ) == false )
		{
			START_LOG( cerr, L"Failed to configure random item probabilities." )
				<< BUILD_LOG( iGroupID )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( fRate )
				<< BUILD_LOG( iPeriod )
				<< BUILD_LOG( iQuantity )
				<< END_LOG;
		}
	}
	else
	{
		ItemGroupDataList itemGDList;
		itemGDList.m_iGroupID	= iGroupID;
		itemGDList.m_vecItemData.push_back( itemData );
		if( itemGDList.m_kLottery.AddCaseIntegerCast( iItemID, fRate, iPeriod, iQuantity ) == false )
		{
			START_LOG( cerr, L"Failed to configure random item probabilities." )
				<< BUILD_LOG( iGroupID )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( fRate )
				<< BUILD_LOG( iPeriod )
				<< BUILD_LOG( iQuantity )
				<< END_LOG;
		}

		m_mapItemGroup.insert( std::make_pair( iGroupID, itemGDList ) );
	}

	return true;
}

bool CXSLRandomItemManager::ModifyMapItemGroup(RANDOMITEM_MAPPING_DATA randItemMappingData)
{
	RandomItemKey mitRandomItemKey( randItemMappingData.iCubeID, randItemMappingData.iKeyItemID );
	std::map< RandomItemKey, RandomItemData >::iterator mit = m_mapRandomItem.find(mitRandomItemKey);

	if( mit == m_mapRandomItem.end() )
		return false;

	std::vector< RandomUnitData >::iterator mitRUD = mit->second.m_vecUnitData.begin();
	for(; mitRUD != mit->second.m_vecUnitData.end(); mitRUD++)
	{
		if( mitRUD->m_iItemGroupID == randItemMappingData.iBeforeGroupID )
		{
			mitRUD->m_iItemGroupID = randItemMappingData.iAfterGroupID;
			randItemMappingData.bCheckMapping = true;
		}
	}
	return true;
}

bool CXSLRandomItemManager::RestoreMapItemGroup(RANDOMITEM_MAPPING_DATA randItemMappingData)
{
	RandomItemKey mitRandomItemKey( randItemMappingData.iCubeID, randItemMappingData.iKeyItemID );
	std::map< RandomItemKey, RandomItemData >::iterator mit = m_mapRandomItem.find(mitRandomItemKey);

	if( mit == m_mapRandomItem.end() )
		return false;

	std::vector< RandomUnitData >::iterator mitRUD = mit->second.m_vecUnitData.begin();
	for(; mitRUD != mit->second.m_vecUnitData.end(); mitRUD++)
	{
		if( mitRUD->m_iItemGroupID == randItemMappingData.iAfterGroupID )
		{
			mitRUD->m_iItemGroupID = randItemMappingData.iBeforeGroupID;
			randItemMappingData.bCheckMapping = false;
		}
	}
	return true;
}

bool CXSLRandomItemManager::AddRandomItemMappingData_LUA( int nIndex, int iCubeID, int iKeyItemID, int iBeforeGroupID, int iAfterGroupID )
{
	RANDOMITEM_MAPPING_DATA stMappingData;
	std::wstring wstrStartDate, wstrEndDate;

	if( iCubeID <= 0 || iBeforeGroupID <= 0 || iAfterGroupID <= 0 )
	{
		START_LOG( cerr, L"Failed to parse random item mapping group data." )
			<< BUILD_LOG( nIndex )
			<< BUILD_LOG( iCubeID )
			<< BUILD_LOG( iKeyItemID )
			<< BUILD_LOG( iBeforeGroupID )
			<< BUILD_LOG( iAfterGroupID )
			<< END_LOG;

		return false;
	}

	stMappingData.iCubeID = iCubeID;
	stMappingData.iKeyItemID = iKeyItemID;
	stMappingData.iBeforeGroupID = iBeforeGroupID;
	stMappingData.iAfterGroupID = iAfterGroupID;
	stMappingData.bCheckMapping = false;

	m_mapRandomItemMappingData.insert(std::make_pair(nIndex, stMappingData));

	return true;
}

bool CXSLRandomItemManager::AddRandomItemMappingTime_LUA( int nIndex,  const char* szStartDate, const char* szEndDate )
{
	RANDOMITEM_MAPPING_TIME stMappingTime;
	std::wstring wstrStartDate, wstrEndDate;

	std::wstring wstrScriptStartDate = KncUtil::toWideString(szStartDate);
	std::wstring wstrScriptEndDate = KncUtil::toWideString(szEndDate);

	CTime tStartTime, tEndTime;

	if( KncUtil::ConvertStringToCTime( wstrScriptStartDate, tStartTime ) == false)
	{
		START_LOG( cerr, L"Failed to convert tStartTime to CTime." )
			<< BUILD_LOG( wstrScriptStartDate )
			<< END_LOG;
		return false;
	}

	if( KncUtil::ConvertStringToCTime( wstrScriptEndDate, tEndTime ) == false)
	{
		START_LOG( cerr, L"Failed to convert tEndTime to CTime." )
			<< BUILD_LOG( wstrScriptEndDate )
			<< END_LOG;
		return false;
	}

	// 시간예외 처리 (월)
	if( tStartTime.GetMonth() > 12 || tEndTime.GetMonth() > 12 )
	{
		START_LOG( cerr, L"A year has only 12 months. Check RandomItemTable.lua." )
			<< BUILD_LOG( nIndex )
			<< BUILD_LOG( wstrScriptStartDate )
			<< BUILD_LOG( wstrScriptEndDate )
			<< END_LOG;
		return false;
	}
	// 시간예외 처리 (일)
	if( tStartTime.GetDay() > 31 || tEndTime.GetDay() > 31 )
	{
		START_LOG( cerr, L"A month cannot have more than 31 days. Check RandomItemTable.lua." )
			<< BUILD_LOG( nIndex )
			<< BUILD_LOG( wstrScriptStartDate )
			<< BUILD_LOG( wstrScriptEndDate )
			<< END_LOG;
		return false;
	}
	// 시간예외 처리 (시간)
	if( tStartTime.GetHour() > 23 || tEndTime.GetHour() > 23 )
	{
		START_LOG( cerr, L"A day cannot have more than 24 hours. Check RandomItemTable.lua." )
			<< BUILD_LOG( nIndex )
			<< BUILD_LOG( wstrScriptStartDate )
			<< BUILD_LOG( wstrScriptEndDate )
			<< END_LOG;
		return false;
	}

	stMappingTime.tStartTime = tStartTime;
	stMappingTime.tEndTime = tEndTime;

	m_mapRandomItemMappingTime.insert(std::make_pair(nIndex, stMappingTime));

	return true;
}

//{{ 2009. 6. 22  최육사	매력아이템 리스트
bool CXSLRandomItemManager::AddAttractionItemInfo_LUA( int iItemID, bool bNotify )
{
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Nonexistent item registered as an attractive item." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	KAttractionItemInfo kInfo;
	kInfo.m_bNotifyMsg = bNotify;
	kInfo.m_iMsgNum = 0;
	m_mapAttractionItem.insert( std::make_pair( iItemID, kInfo ) );
	return true;
}
//}}

//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브

bool CXSLRandomItemManager::AddSealRandomItemInfo_LUA( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Nonexistent item registered as a sealed random item." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	m_setSealRandomItem.insert( iItemID );
	return true;
}

bool CXSLRandomItemManager::AddNotifyResultItemInfo_LUA( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Nonexistent item registered as a global announcement item." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	m_setNotifyResultItem.insert( iItemID );
	return true;
}

//}}

//{{ 2009. 12. 5  최육사	랜덤큐브시스템개편
const std::map< int, int >* CXSLRandomItemManager::GetRandomItemKeyList( IN int iRandomItemID ) const
{
	std::map< int, std::map< int, int > >::const_iterator mit = m_mapRandomItemKeyTable.find( iRandomItemID );
	if( mit == m_mapRandomItemKeyTable.end() )
		return NULL;

	return &mit->second;
}
//}}

const CXSLRandomItemManager::RandomItemData* CXSLRandomItemManager::GetRandomItemData( IN int iRandomItemID, IN int iKeyItemID ) const
{
	std::map< RandomItemKey, RandomItemData >::const_iterator mit;
	mit = m_mapRandomItem.find( RandomItemKey( iRandomItemID, iKeyItemID ) );
	if( mit == m_mapRandomItem.end() )
		return NULL;

	return &mit->second;
}

//{{ 2009. 6. 22  최육사	매력아이템 리스트
bool CXSLRandomItemManager::MakeNotifyMsgAttrationItem( IN const int iRewardItemID, IN const std::wstring& wstrNickName, OUT std::wstring& wstrNotifyMsg )
{
	wstrNotifyMsg.clear();

	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iRewardItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Item does not exist." )
			<< BUILD_LOG( iRewardItemID )
			<< END_LOG;

		return false;
	}

	std::map< int, KAttractionItemInfo >::const_iterator mit;
	mit = m_mapAttractionItem.find( iRewardItemID );
	if( mit == m_mapAttractionItem.end() )
	{
		START_LOG( cerr, L"Item is not an attractive item; cannot send a global announcement." )
			<< BUILD_LOG( iRewardItemID )
			<< END_LOG;

		return false;
	}

	if( mit->second.m_bNotifyMsg == false  ||  m_bIsNotifyMsg == false )
	{
		START_LOG( clog, L"Attractive item obtained, but no global announcement was sent." )
			<< BUILD_LOG( iRewardItemID )
			<< END_LOG;

		return false;
	}

	//{{ 2011.11.20 임홍락	서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
	wstrNotifyMsg = wstrNickName;
	wstrNotifyMsg += L";";
	wstrNotifyMsg += pItemTemplet->m_Name;
	//}}

	return true;
}
//}}

//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브

bool CXSLRandomItemManager::MakeNotifyMsgRandomResultItem( IN const int iRewardItemID, IN const std::wstring& wstrNickName, OUT std::wstring& wstrNotifyMsg )
{
	wstrNotifyMsg.clear();

	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iRewardItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Item does not exist." )
			<< BUILD_LOG( iRewardItemID )
			<< END_LOG;

		return false;
	}

	//{{ 2011.11.20 임홍락	서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
	wstrNotifyMsg = wstrNickName;
	wstrNotifyMsg += L";";
	wstrNotifyMsg += pItemTemplet->m_Name;
	//}}

	return true;
}

//}}

//{{ 2011. 05. 12  김민성	랜덤 큐브 오픈시 인벤토리 검사 수정
bool CXSLRandomItemManager::GetResultItem( IN char cUnitClass, IN const RandomItemData* pRandomItemData, OUT std::map< int, KItemInfo >& mapResultItem, OUT std::map< int, int >& mapCheckEmpty, OUT bool& bIsGiveAll ) const
//}}
{
	SET_ERROR( NET_OK );

	mapResultItem.clear();
	mapCheckEmpty.clear();

	int iGroupID = 0;
	std::map< int, ItemGroupDataList >::const_iterator mit;

	if( pRandomItemData == NULL )
	{
		START_LOG( cerr, L"Invalid random information pointer." )
			<< END_LOG;

		goto end_proc;
	}

	//{{ 2012. 10. 16	박세훈	RandomBoxTable 구조 변경
	if( pRandomItemData->m_byteUseCondition == CXSLItem::UC_ANYONE )
	{
		if( pRandomItemData->m_vecUnitData.size() > 0 )
		{
			iGroupID = pRandomItemData->m_vecUnitData[0].m_iItemGroupID;
		}
	}
	else
	{
		if( pRandomItemData->m_byteUseCondition == CXSLItem::UC_ONE_UNIT )
		{
			// 랜덤 큐브 캐릭터 UnitType별로 오픈 기능
			// 해당 UnitClass의 기본 UnitClass를 얻는다.
			cUnitClass = static_cast<char>( CXSLUnit::GetUnitClassBaseGrade( static_cast<CXSLUnit::UNIT_CLASS>( cUnitClass ) ) );
		}
		/*else if( pRandomItemData->m_byteUseCondition == CXSLItem::UC_ONE_CLASS )
		{
		}*/

		for( int i = 0; i < (int)pRandomItemData->m_vecUnitData.size(); ++i )
		{
			if( pRandomItemData->m_vecUnitData[i].m_cUnitClass == cUnitClass )
			{
				iGroupID = pRandomItemData->m_vecUnitData[i].m_iItemGroupID;
				break;
			}
		}
	}
	//}}

	if( iGroupID == 0  &&  pRandomItemData->m_iRessurectionCount > 0 )
	{
		return true;
	}
	else if( iGroupID == 0  &&  pRandomItemData->m_iRestoreSpirit > 0 )
	{
		return true;
	}
	else if( iGroupID == 0 )
	{
		goto end_proc;
	}

	mit = m_mapItemGroup.find( iGroupID );

	if( mit == m_mapItemGroup.end() )
	{
		goto end_proc;
	}

	//{{ 2011. 05. 12  김민성	랜덤 큐브 오픈시 인벤토리 검사 수정
	bIsGiveAll = pRandomItemData->m_bGiveAll;
	//}}

	//{{ 2008. 5. 27  최육사  랜덤 결과 인벤 공간 체크
	{
		std::map< int, KLottery::KCaseUnit >::const_iterator mitCase;
		for( mitCase = mit->second.m_kLottery.m_mapCase.begin(); mitCase != mit->second.m_kLottery.m_mapCase.end(); ++mitCase )
		{
			mapCheckEmpty.insert( std::make_pair( mitCase->first, mitCase->second.m_nParam2 ) );
		}
	}
	//}}

	if( pRandomItemData->m_bGiveAll == false )
	{
		int iItemID = 0;

		iItemID = mit->second.m_kLottery.Decision();

		if( iItemID == KLottery::CASE_BLANK )
		{
			goto end_proc;
		}

		// 기간제
		int iPeriod = mit->second.m_kLottery.GetParam1( iItemID );
		if( iPeriod == KLottery::PARAM_BLANK )
		{
			START_LOG( cerr, L"Parameter for this case is missing. Random cube result has no duration information; check whether the probability is 100%." )
				<< BUILD_LOG( iGroupID )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( iPeriod )
				<< END_LOG;

			goto end_proc;
		}

		// 수량
		int iQuantity = mit->second.m_kLottery.GetParam2( iItemID );
		if( iQuantity == KLottery::PARAM_BLANK )
		{
			START_LOG( cerr, L"Parameter for this case is missing. Random cube result has no quantity information; check whether the probability is 100%." )
				<< BUILD_LOG( iGroupID )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( iPeriod )
				<< BUILD_LOG( iQuantity )
				<< END_LOG;

			goto end_proc;
		}

		//{{ 2008. 3. 17  최육사  DeleteAndInsert()
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"Item template information not found." )
				<< BUILD_LOG( iItemID )
				<< END_LOG;
		}
		else
		{
			KItemInfo kInsertItemInfo;
			kInsertItemInfo.m_iItemID	 = iItemID;
			kInsertItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
			kInsertItemInfo.m_iQuantity	 = iQuantity;
			kInsertItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
			kInsertItemInfo.m_sPeriod	 = static_cast<short>(iPeriod); // 기간제 설정
			//{{ 2013. 05. 15	최육사	아이템 개편
			//}}

			mapResultItem.insert( std::make_pair( iItemID, kInsertItemInfo ) );
		}
		//}}

		const int iBonusItemID = SiCXSLRandomItemManager()->GetBonusCharmItem( pRandomItemData->m_ItemID );
		if( 0 < iBonusItemID )
		{
			const CXSLItem::ItemTemplet* pItemTempletBonus = SiCXSLItemManager()->GetItemTemplet( iBonusItemID );
			if( pItemTempletBonus == NULL )
			{
				START_LOG( cerr, L"Item template information not found." )
					<< BUILD_LOG( iBonusItemID )
					<< END_LOG;
			}
			else
			{
				KItemInfo kBonusItemInfo;
				kBonusItemInfo.m_iItemID	= iBonusItemID;
				kBonusItemInfo.m_cUsageType	= pItemTempletBonus->m_PeriodType;
				kBonusItemInfo.m_iQuantity	= 1;
				kBonusItemInfo.m_sEndurance	= pItemTempletBonus->m_Endurance;
				kBonusItemInfo.m_sPeriod	= 0;

				mapResultItem.insert( std::make_pair( iBonusItemID, kBonusItemInfo ) );
			}
		}
	}
	else
	{
		for( int i = 0; i < (int)mit->second.m_vecItemData.size(); ++i )
		{
			// 기간제
			int iPeriod = mit->second.m_kLottery.GetParam1( mit->second.m_vecItemData[i].m_iItemID );
			if( iPeriod == KLottery::PARAM_BLANK )
			{
				START_LOG( cerr, L"Parameter for this case is missing. Random cube result has no duration information." )
					<< BUILD_LOG( iGroupID )
					<< BUILD_LOG( mit->second.m_vecItemData[i].m_iItemID )
					<< BUILD_LOG( iPeriod )
					<< END_LOG;

				goto end_proc;
			}

			// 수량
			int iQuantity = mit->second.m_kLottery.GetParam2( mit->second.m_vecItemData[i].m_iItemID );
			if( iQuantity == KLottery::PARAM_BLANK )
			{
				START_LOG( cerr, L"Parameter for this case is missing. Random cube result has no quantity information." )
					<< BUILD_LOG( iGroupID )
					<< BUILD_LOG( mit->second.m_vecItemData[i].m_iItemID )
					<< BUILD_LOG( iPeriod )
					<< BUILD_LOG( iQuantity )
					<< END_LOG;

				goto end_proc;
			}

			//{{ 2008. 3. 17  최육사  DeleteAndInsert()
			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( mit->second.m_vecItemData[i].m_iItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"Item template information not found." )
					<< BUILD_LOG( mit->second.m_vecItemData[i].m_iItemID )
					<< END_LOG;
			}
			else
			{
				KItemInfo kInsertItemInfo;
				kInsertItemInfo.m_iItemID	 = mit->second.m_vecItemData[i].m_iItemID;
				kInsertItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
				kInsertItemInfo.m_iQuantity	 = iQuantity;
				kInsertItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
				kInsertItemInfo.m_sPeriod 	 = static_cast<short>(iPeriod); // 기간제 설정
				//{{ 2011. 06. 01	최육사	강화된 아이템 보상 큐브
				if( pItemTemplet->m_ItemType == CXSLItem::IT_WEAPON  || pItemTemplet->m_ItemType == CXSLItem::IT_DEFENCE )
				{
					CXSLItem::UpdateEnchantedItemCubeReward( pRandomItemData->m_ItemID, kInsertItemInfo );
				}
				//}}

				mapResultItem.insert( std::make_pair( mit->second.m_vecItemData[i].m_iItemID, kInsertItemInfo ) );
			}
			//}}

			const int iBonusItemID = SiCXSLRandomItemManager()->GetBonusCharmItem( pRandomItemData->m_ItemID );
			if( 0 < iBonusItemID )
			{
				const CXSLItem::ItemTemplet* pItemTempletBonus = SiCXSLItemManager()->GetItemTemplet( iBonusItemID );
				if( pItemTempletBonus == NULL )
				{
					START_LOG( cerr, L"Item template information not found." )
						<< BUILD_LOG( iBonusItemID )
						<< END_LOG;
				}
				else
				{
					KItemInfo kBonusItemInfo;
					kBonusItemInfo.m_iItemID	= iBonusItemID;
					kBonusItemInfo.m_cUsageType	= pItemTempletBonus->m_PeriodType;
					kBonusItemInfo.m_iQuantity	= 1;
					kBonusItemInfo.m_sEndurance	= pItemTempletBonus->m_Endurance;
					kBonusItemInfo.m_sPeriod	= 0;

					mapResultItem.insert( std::make_pair( iBonusItemID, kBonusItemInfo ) );
				}
			}
		}
	}

	if( mapResultItem.size() <= 0 )
	{
		goto end_proc;
	}

	return true;

end_proc:
	SET_ERROR( ERR_RANDOM_ITEM_02 );

	START_LOG( cwarn, L"Failed to find the random item group." )
		<< BUILD_LOG( pRandomItemData->m_ItemID )
		<< BUILD_LOGc( cUnitClass )
		<< END_LOG;

	return false;
}

//{{ 2013. 3. 16	박세훈	매력 아이템 스크립트 처리
void CXSLRandomItemManager::AddCharmItem_LUA( IN int iItemID, IN int iBonusItemID )
{
	CharmItemInfo kInfo;
	kInfo.m_iBonusItemID = iBonusItemID;
	std::pair< std::map<int, CharmItemInfo>::iterator, bool > pairResult = m_mapCharItemInfo.insert( std::map<int, CharmItemInfo>::value_type( iItemID, kInfo ) );

	if( pairResult.second == false )
	{
		START_LOG( cerr, L"An attractive item with this ItemID is already registered." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
	}
}

bool CXSLRandomItemManager::IsCharmItem( IN const int iItemID ) const
{
	return m_mapCharItemInfo.find( iItemID ) != m_mapCharItemInfo.end();
}

int CXSLRandomItemManager::GetBonusCharmItem( IN const int iItemID ) const
{
	std::map<int, CharmItemInfo>::const_iterator it = m_mapCharItemInfo.find( iItemID );
	if( it == m_mapCharItemInfo.end() )
	{
		return 0;
	}

	return it->second.m_iBonusItemID;
}
//}}
