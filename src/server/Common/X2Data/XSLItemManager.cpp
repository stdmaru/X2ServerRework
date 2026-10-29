#include "./XSLItemManager.h"
#include "Enum/Enum.h"
//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
#include "X2Data/XSLEnchantItemManager.h"
//}}

//{{ 2013. 05. 15	최육사	아이템 개편
	#include "X2Data/XSLCharacterStatTable.h"
	#include "X2Data/XSLSocketItem.h"
//}}

ImplementRefreshSingleton( CXSLItemManager );

CXSLItemManager::CXSLItemManager(void)
{
	m_ShowItem = 0;
	m_InfoType = 0;
}

CXSLItemManager::~CXSLItemManager(void)
{
}

ImplToStringW( CXSLItemManager )
{
	stm_	<< L"----------[ Item Manager ]----------" << std::endl
			<< TOSTRINGW( m_ItemTempletIDMap.size() )
			//{{ 2011. 08. 16	최육사	아이템 교환 개편
			<< TOSTRINGW( m_mapItemExchangeTable.size() )
			//}}
			//{{ 2010. 9. 1	최육사	기간 초기화 아이템
			<< TOSTRINGW( m_mapExpandPeriodItem.size() )
			//}}
			//{{ 2010. 11. 29	최육사	기간제템 삭제
			//}}
			//{{ 2012. 04. 04	최육사	배틀필드 시스템
			<< TOSTRINGW( m_mapCoolTimeItemGroup.size() )
			//}}
			//{{ 2013. 05. 21	최육사	아이템 개편
			<< TOSTRINGW( m_mapItemConvertGroup.size() )
			<< TOSTRINGW( m_mapItemConvertTable.size() )
			//}}
		;

	return stm_;
}

ImplementLuaScriptParser( CXSLItemManager )
{
	lua_tinker::class_add<CXSLItemManager>( GetLuaState(), "CXSLItemManager" );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddItemTemplet",			&CXSLItemManager::AddItemTemplet_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddItemExchangeData",		&CXSLItemManager::AddItemExchangeData_LUA );
	//{{ 2010. 9. 1	최육사	기간 초기화 아이템
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddItemExpandPeriodData",	&CXSLItemManager::AddItemExpandPeriodData_LUA );
	//}}
	//{{ 2010. 11. 29	최육사	기간제템 삭제
	//}}
	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddShopItemList_LUA",		&CXSLItemManager::AddShopItemList_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddShopItemGroup_LUA",		&CXSLItemManager::AddShopItemGroup_LUA );
	//}}
	//{{ 2012. 04. 04	최육사	배틀필드 시스템
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddCoolTimeGroupItem",		&CXSLItemManager::AddCoolTimeGroupItem_LUA );
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddWeddingHallItem",						&CXSLItemManager::AddWeddingHallItem_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddWeddingCompleteRewardItem",				&CXSLItemManager::AddWeddingCompleteRewardItem_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddCoupleAnniversaryDayRewardItem",			&CXSLItemManager::AddCoupleAnniversaryDayRewardItem_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddWeddingAnniversaryDayRewardItem",		&CXSLItemManager::AddWeddingAnniversaryDayRewardItem_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddWeddingAnniversaryDayRewardTitle",		&CXSLItemManager::AddWeddingAnniversaryDayRewardTitle_LUA );
	//}
	//{{ 2013. 05. 21	최육사	아이템 개편
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddItemConvertGroup",						&CXSLItemManager::AddItemConvertGroup_LUA );
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddItemConvertInfo",						&CXSLItemManager::AddItemConvertInfo_LUA );
	//}}
	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "AddKeepShowItem",		&CXSLItemManager::AddKeepShowItem_LUA );


	lua_tinker::class_def<CXSLItemManager>( GetLuaState(), "dump",						&CXSLItemManager::Dump );

	lua_tinker::decl( GetLuaState(), "g_pItemManager", this );
}

const CXSLItem::ItemTemplet* CXSLItemManager::GetItemTemplet( const int itemID ) const
{
	std::map< int, CXSLItem::ItemTemplet >::const_iterator mit;
	mit = m_ItemTempletIDMap.find( itemID );
	if( mit == m_ItemTempletIDMap.end() )
		return NULL;

	return &mit->second;
}

bool CXSLItemManager::AddItemTemplet_LUA()
{
	CXSLItem::ItemTemplet kItemTemplet;
	KLuaManager luaManager( GetLuaState() );
	TableBind( &luaManager );

	LUA_GET_VALUE_RETURN(	luaManager, L"m_ItemID",				kItemTemplet.m_ItemID,					0,				return false; );
	LUA_GET_VALUE_RETURN(	luaManager, L"m_Name",					kItemTemplet.m_Name,					L"",			return false; );

	// 신규아이템 추가시 중복되는 이름이 있는지 체크하는 코드
	//static std::wstring g_temp;
	//if( g_temp == pItemTemplet->m_Name )
	//{
	//	START_LOG( cerr, L"아앗! 중복되는 이름 발견!" );
	//}
	//g_temp = pItemTemplet->m_Name;

	LUA_GET_VALUE( luaManager, L"m_Description",					kItemTemplet.m_Description,				L"" );
	LUA_GET_VALUE( luaManager, L"m_DescriptionInShop",				kItemTemplet.m_DescriptionInShop,		L"" );

	//{{ dmlee 2008.11.29
	LUA_GET_VALUE( luaManager, L"m_ModelName",						kItemTemplet.m_ModelName[0],				L"" );
	LUA_GET_VALUE( luaManager, L"m_ModelName1",						kItemTemplet.m_ModelName[1],				L"" );
	//}} dmlee 2008.11.29

	LUA_GET_VALUE( luaManager, L"m_TextureChangeXETName",			kItemTemplet.m_TextureChangeXETName,	L"" );
	LUA_GET_VALUE( luaManager, L"m_AniXETName",						kItemTemplet.m_AniXETName,				L"" );
	LUA_GET_VALUE( luaManager, L"m_AniName",						kItemTemplet.m_AniName,					L"" );
	LUA_GET_VALUE( luaManager, L"m_ShopImage",						kItemTemplet.m_ShopImage,				L"" );
	LUA_GET_VALUE( luaManager, L"m_DropViewer",						kItemTemplet.m_DropViewer,				L"" );

	LUA_GET_VALUE_RETURN_ENUM(	luaManager, L"m_ItemType",			kItemTemplet.m_ItemType,				CXSLItem::ITEM_TYPE,		CXSLItem::IT_NONE,		return false; );
	LUA_GET_VALUE_ENUM(			luaManager, L"m_ItemGrade",			kItemTemplet.m_ItemGrade,				CXSLItem::ITEM_GRADE,		CXSLItem::IG_NORMAL );

	LUA_GET_VALUE(			luaManager, L"m_bFashion",				kItemTemplet.m_bFashion,				false );
	LUA_GET_VALUE(			luaManager, L"m_bVested",				kItemTemplet.m_bVested,					false );
	LUA_GET_VALUE(			luaManager, L"m_bCanEnchant",			kItemTemplet.m_bCanEnchant,				false );
	LUA_GET_VALUE(			luaManager, L"m_bCanUseInventory",		kItemTemplet.m_bCanUseInventory,		false );
	//{{ 2009. 7. 30  최육사	장착불가
	LUA_GET_VALUE(			luaManager, L"m_bNoEquip",				kItemTemplet.m_bNoEquip,				false );
	//}}
	//{{ 2010. 01. 05  최육사	PC방 프리미엄
	LUA_GET_VALUE(			luaManager, L"m_bPcBang",				kItemTemplet.m_bIsPcBang,				false );
	//}}
	//{{ 2011. 10. 14	최육사	배틀필드 아이템 레벨 시스템
	LUA_GET_VALUE(			luaManager, L"m_ItemLevel",				kItemTemplet.m_iItemLevel,				0 );
	//}}
	//{{ 2009. 9. 2  최육사		밀봉
	int iMaxSealCount = 0;
	LUA_GET_VALUE(			luaManager, L"m_ucMaxSealCount",		iMaxSealCount,								0 );
	kItemTemplet.m_ucMaxSealCount = static_cast<BYTE>( iMaxSealCount );
	//}}

	//{{ 2009. 11. 17  최육사	속성 강화 가능 갯수
	LUA_GET_VALUE(			luaManager, L"m_iMaxAttribEnchantCount",kItemTemplet.m_iMaxAttribEnchantCount,		0 );
	//}}

	LUA_GET_VALUE_ENUM(		luaManager, L"m_UseType",				kItemTemplet.m_UseType,					CXSLItem::USE_TYPE,			CXSLItem::UT_NONE );

	//{{ dmlee 2008.11.29
	LUA_GET_VALUE(			luaManager, L"m_AttachFrameName",		kItemTemplet.m_AttachFrameName[0],			L"" );
	LUA_GET_VALUE(			luaManager, L"m_AttachFrameName1",		kItemTemplet.m_AttachFrameName[1],			L"" );
	//}} dmlee 2008.11.29

	LUA_GET_VALUE(			luaManager, L"m_bCanHyperMode",			kItemTemplet.m_bCanHyperMode,			false );

	LUA_GET_VALUE_ENUM(		luaManager, L"m_PeriodType",			kItemTemplet.m_PeriodType,				CXSLItem::PERIOD_TYPE,		CXSLItem::PT_INFINITY );
	LUA_GET_VALUE(			luaManager, L"m_Endurance",				kItemTemplet.m_Endurance,				-1 );
	LUA_GET_VALUE(			luaManager, L"m_EnduranceDamageMin",	kItemTemplet.m_EnduranceDamageMin,		0 );
	LUA_GET_VALUE(			luaManager, L"m_EnduranceDamageMax",	kItemTemplet.m_EnduranceDamageMax,		0 );
	LUA_GET_VALUE(			luaManager, L"m_RepairED",				kItemTemplet.m_RepairED,				0.0f );
	LUA_GET_VALUE(			luaManager, L"m_RepairVP",				kItemTemplet.m_RepairVP,				0 );
	LUA_GET_VALUE(			luaManager, L"m_Quantity",				kItemTemplet.m_Quantity,				0 );

	LUA_GET_VALUE_ENUM(		luaManager, L"m_PriceType",				kItemTemplet.m_PriceType,				CXSLItem::SHOP_PRICE_TYPE,	CXSLItem::SPT_GP );
	LUA_GET_VALUE(			luaManager, L"m_Price",					kItemTemplet.m_Price,					0 );
	LUA_GET_VALUE(			luaManager, L"m_PricePvPPoint",			kItemTemplet.m_PricePvPPoint,			0 );

	LUA_GET_VALUE_ENUM(		luaManager, L"m_UseCondition",			kItemTemplet.m_UseCondition,			CXSLItem::USE_CONDITION,	CXSLItem::UC_ANYONE );
	if( CXSLItem::UC_NONE == kItemTemplet.m_UseCondition )
	{
		// 사용하지 못하는 아이템이다
		return false;
	}

	LUA_GET_VALUE_ENUM(		luaManager, L"m_UnitType",				kItemTemplet.m_UnitType,				CXSLUnit::UNIT_TYPE,		CXSLUnit::UT_NONE );
	LUA_GET_VALUE_ENUM(		luaManager, L"m_UnitClass",				kItemTemplet.m_UnitClass,				CXSLUnit::UNIT_CLASS,		CXSLUnit::UC_NONE );
	LUA_GET_VALUE(			luaManager, L"m_UseLevel",				kItemTemplet.m_UseLevel,				0 );
	//{{ 2011. 07. 27	최육사	대전 개편
	LUA_GET_VALUE_ENUM(		luaManager, L"m_BuyPvpRankCondition",	kItemTemplet.m_BuyPvpRankCondition,		CXSLUnit::PVP_RANK,			CXSLUnit::PVPRANK_NONE );
	//}}

	LUA_GET_VALUE_ENUM(		luaManager, L"m_EqipPosition",			kItemTemplet.m_EqipPosition,			CXSLUnit::EQIP_POSITION,	CXSLUnit::EP_QUICK_SLOT );

	if( luaManager.BeginTable( "m_Stat" ) == S_OK )
	{
		float fBuffer = 0.0f;

		LUA_GET_VALUE( luaManager, "m_fBaseHP",			fBuffer,			0 );
		kItemTemplet.m_Stat.m_fBaseHP = fBuffer;
		LUA_GET_VALUE( luaManager, "m_fAtkPhysic",		fBuffer,			0 );
		kItemTemplet.m_Stat.m_fAtkPhysic = fBuffer;
		LUA_GET_VALUE( luaManager, "m_fAtkMagic",		fBuffer,			0 );
		kItemTemplet.m_Stat.m_fAtkMagic = fBuffer;
		LUA_GET_VALUE( luaManager, "m_fDefPhysic",		fBuffer,			0 );
		kItemTemplet.m_Stat.m_fDefPhysic = fBuffer;
		LUA_GET_VALUE( luaManager, "m_fDefMagic",		fBuffer,			0 );
		kItemTemplet.m_Stat.m_fDefMagic = fBuffer;

		luaManager.EndTable();
	}


	if( luaManager.BeginTable( "m_SpecialAbilityList" ) == S_OK )
	{
		int index = 1;
		while( luaManager.BeginTable( index ) == S_OK )
		{
			CXSLItem::SpecialAbility sa;

			LUA_GET_VALUE_ENUM( luaManager, L"TYPE",		sa.m_Type,		CXSLItem::SPECIAL_ABILITY_TYPE, CXSLItem::SAT_NONE );
			LUA_GET_VALUE(		luaManager, L"COOL_TIME",	sa.m_CoolTime,	0 );
			LUA_GET_VALUE(		luaManager, L"VALUE1",		sa.m_Value1,	0 );
			LUA_GET_VALUE(		luaManager, L"VALUE2",		sa.m_Value2,	0 );
			LUA_GET_VALUE(		luaManager, L"VALUE3",		sa.m_Value3,	0 );

			kItemTemplet.m_SpecialAbilityList.push_back( sa );

			luaManager.EndTable();
			index++;
		}
		luaManager.EndTable();
	}


	//{{ dmlee 2008.11.14 아이템 고유속성에 소켓옵션 추가
	kItemTemplet.m_vecSocketOption.resize(0);
	if( S_OK == luaManager.BeginTable( "SOCKET_OPTION" ) )
	{
		int socketID = 0;
		int index = 1;
		while( S_OK == luaManager.GetValue( index, socketID ) )
		{
			kItemTemplet.m_vecSocketOption.push_back( socketID );
			index++;
		}

		luaManager.EndTable();
	}
	//}} dmlee 2008.11.14 아이템 고유속성에 소켓옵션 추가


	//{{ 2013. 05. 15	최육사	아이템 개편
	// 랜덤 소켓 그룹
	if( S_OK == luaManager.BeginTable( "RANDOM_SOCKET_GROUP_ID" ) )
	{
		int iGroupID = 0;
		int index = 1;
		while( S_OK == luaManager.GetValue( index, iGroupID ) )
		{
			kItemTemplet.m_vecRandomSocketGroupID.push_back( iGroupID );
			index++;
		}

		luaManager.EndTable();
	}

    // 스탯 연동 레벨
	if( luaManager.BeginTable( "m_Stat_Relation_Level" ) == S_OK )
	{
		int iLevel = 0;
		LUA_GET_VALUE( luaManager, "m_byBaseHPRelLV",		iLevel,		0 );
		kItemTemplet.m_kStatRelationLevel.m_byBaseHPRelLV = static_cast<BYTE>(iLevel);
		LUA_GET_VALUE( luaManager, "m_byAtkPhysicRelLV",	iLevel,		0 );
		kItemTemplet.m_kStatRelationLevel.m_byAtkPhysicRelLV = static_cast<BYTE>(iLevel);
		LUA_GET_VALUE( luaManager, "m_byAtkMagicRelLV",		iLevel,		0 );
		kItemTemplet.m_kStatRelationLevel.m_byAtkMagicRelLV = static_cast<BYTE>(iLevel);
		LUA_GET_VALUE( luaManager, "m_byDefPhysicRelLV",	iLevel,		0 );
		kItemTemplet.m_kStatRelationLevel.m_byDefPhysicRelLV = static_cast<BYTE>(iLevel);
		LUA_GET_VALUE( luaManager, "m_byDefMagicRelLV ",	iLevel,		0 );
		kItemTemplet.m_kStatRelationLevel.m_byDefMagicRelLV = static_cast<BYTE>(iLevel);

		// 공용 아이템인지 확인
		if( kItemTemplet.m_kStatRelationLevel.m_byBaseHPRelLV != 0  ||
			kItemTemplet.m_kStatRelationLevel.m_byAtkPhysicRelLV != 0  ||
			kItemTemplet.m_kStatRelationLevel.m_byAtkMagicRelLV != 0  ||
			kItemTemplet.m_kStatRelationLevel.m_byDefPhysicRelLV != 0  ||
			kItemTemplet.m_kStatRelationLevel.m_byDefMagicRelLV != 0 )
		{
			kItemTemplet.m_kStatRelationLevel.m_bIsCommonItem = true;
		}

		luaManager.EndTable();
	}
	//}}

	LUA_GET_VALUE(		luaManager, L"m_CoolTime",			kItemTemplet.m_CoolTime,				0 );

	LUA_GET_VALUE(		luaManager, L"m_SetID",				kItemTemplet.m_SetID,					0 );

	//{{ 2011. 07. 08    김민성    옵션 수치화
	// 셋트효과가 있고, 요구레벨도 있는 경우
	if ( 0 < kItemTemplet.m_SetID && 0 < kItemTemplet.m_UseLevel )
	{
		UpdateSetIDAndMaxLevelMap( kItemTemplet.m_SetID, kItemTemplet.m_UseLevel );
	}
	//}}

	//{{ 2012. 06. 28	박세훈	2012 대전 시즌2
	LUA_GET_VALUE(		luaManager, L"m_iAttributeLevel",	kItemTemplet.m_iAttributeLevel,			0 );	// 요구 속성 레벨
	//}}


	//{{ 버프 관리 시스템 - 김민성
	// 배틀 필드 버프 시스템
	if( S_OK == luaManager.BeginTable( "BUFF_FACTOR" ) )
	{
		int buffID = 0;
		int index = 1;
		if( S_OK == luaManager.GetValue( index, buffID ) )
		{
			kItemTemplet.m_iBuffFactorID = buffID;
		}

		luaManager.EndTable();
	}
	//}

	LUA_GET_VALUE(			luaManager, L"m_bInventoryOnly",				kItemTemplet.m_bInventoryOnly,				false );

	if( m_ItemTempletIDMap.find( kItemTemplet.m_ItemID ) != m_ItemTempletIDMap.end() )
	{
		START_LOG( cerr, L"Item.lua(m_ItemTempletNameMap/m_ItemTempletIDMap) 확인 실패.!" )
			<< BUILD_LOG( kItemTemplet.m_Name )
			<< BUILD_LOG( kItemTemplet.m_ItemID )
			<< END_LOG;
		return false;
	}
	else
	{
		m_ItemTempletIDMap.insert( std::make_pair( kItemTemplet.m_ItemID, kItemTemplet ) );

		KItemName kItemName;
		kItemName.m_wstrKRItemName = kItemTemplet.m_Name;
		m_ItemTempletNameMap.insert( std::make_pair( kItemTemplet.m_ItemID, kItemName ) );
	}
	//}}

	return true;
}


bool CXSLItemManager::ItemExchangeData::AddExchangeData( IN const ITEM_EXCHANGE_TYPE eExchangeType,
	IN const int iDestItemID,
	IN const float fDestItemRate,
	IN const int iDestQuantity
	, IN const short sPeriod
	)
{
	// 값이 들어온적이 없으면 초기화 하자!
	if( m_eExchangeType == IET_INVALID )
	{
		m_eExchangeType = eExchangeType;
	}
	else
	{
		if( m_eExchangeType != eExchangeType )
		{
			START_LOG( cerr, L"교환 타입이 다르다! 스크립트 작성이 잘못된거 같다!" )
				<< BUILD_LOG( m_eExchangeType )
				<< BUILD_LOG( eExchangeType )
				<< END_LOG;
			return false;
		}
	}

	switch( eExchangeType )
	{
	case IET_RANDOM:
		{
			if( m_kLottery.AddCaseIntegerCast( iDestItemID, fDestItemRate, iDestQuantity ) == false )
			{
				START_LOG( cerr, L"아이템 교환 확률이 올바르지 않습니다" )
					<< BUILD_LOG( iDestItemID )
					<< BUILD_LOG( fDestItemRate )
					<< BUILD_LOG( iDestQuantity )
					<< END_LOG;

				return false;
			}
		}
		break;

	case IET_ALL:
	case IET_SELECT:
		{
			m_mapDestItemID.insert( std::make_pair( iDestItemID, iDestQuantity ) );
		}
		break;

	default:
		{
			START_LOG( cerr, L"올바르지 않은 아이템교환 type 입니다" )
				<< BUILD_LOG( (int) eExchangeType )
				<< BUILD_LOG( iDestItemID )
				<< BUILD_LOG( fDestItemRate )
				<< BUILD_LOG( iDestQuantity )
				<< END_LOG;

			return false;
		}
		break;
	}

	m_mapDestItemPeriod.insert( std::make_pair( iDestItemID, sPeriod ) );

	return true;
}

//{{ 2011. 08. 08	최육사	아이템 교환 시스템 개편
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CXSLItemManager::AddItemExchangeData_LUA( int iHouseID, ITEM_EXCHANGE_TYPE eExchangeType, int iSrcItemID, int iDestItemID, int iDestQuantity, float fDestItemRate )
{
	KLuaManager luaManager( GetLuaState() );
	TableBind( &luaManager );

	// fix!! eNPCID에 대한 체크가 필요
	if( iSrcItemID <= 0 || iDestItemID <= 0 || iDestQuantity <= 0 || fDestItemRate <= 0.f )
	{
		START_LOG( cerr, L"아이템 교환 정보가 이상합니다." )
			<< BUILD_LOG( iSrcItemID )
			<< BUILD_LOG( iDestItemID )
			<< BUILD_LOG( iDestQuantity )
			<< BUILD_LOG( fDestItemRate )
			<< END_LOG;

		return false;
	}

	if( eExchangeType == IET_INVALID )
	{
		START_LOG( cerr, L"교환 테이블에 잘못설정된 ExchangeType이 있습니다!" )
			<< BUILD_LOG( iHouseID )
			<< BUILD_LOG( eExchangeType )
			<< BUILD_LOG( iSrcItemID )
			<< END_LOG;

		return false;
	}

	// item.lua가 먼저 parsing되었기 때문에 검사 가능..
	if( GetItemTemplet( iSrcItemID ) == NULL  ||  GetItemTemplet( iDestItemID ) == NULL )
	{
		START_LOG( cerr, L"존재하지 않는 ItemID입니다!" )
			<< BUILD_LOG( iSrcItemID )
			<< BUILD_LOG( iDestItemID )
			<< BUILD_LOG( fDestItemRate )
			<< BUILD_LOG( iDestQuantity )
			<< END_LOG;

		return false;
	}

	int iServerGroupID = 0;
	LUA_GET_VALUE(					luaManager, L"m_iServerGroupID",	iServerGroupID,					-1 );

#ifdef EXTEND_SERVER_GROUP_MASK
	if( iServerGroupID != -1 )
#else
	if( iServerGroupID != SEnum::SGI_INVALID )
#endif // EXTEND_SERVER_GROUP_MASK
	{
		if( SEnum::IsValidServerGroupID( iServerGroupID ) == false )
		{
			START_LOG( cerr, L"정상적인 서버군ID값이 아닙니다." )
				<< BUILD_LOG( iServerGroupID )
				<< BUILD_LOG( iHouseID )
				<< BUILD_LOG( iSrcItemID )
				<< BUILD_LOG( iDestItemID );
			return false;
		}

		if( KBaseServer::GetKObj()->GetServerGroupID() != iServerGroupID )
		{
			START_LOG( clog2, L"서버군이 다르므로 해당 교환아이템 정보는 파싱하지 않습니다." )
				<< BUILD_LOG( iServerGroupID )
				<< BUILD_LOG( iHouseID )
				<< BUILD_LOG( iSrcItemID )
				<< BUILD_LOG( iDestItemID );
			return true;
		}
	}

	int iSourceQuantity = 0;
	LUA_GET_VALUE(					luaManager, L"m_iSourceQuantity",	iSourceQuantity,				1 );

	short sPeriod = 0;
	LUA_GET_VALUE(					luaManager, L"m_sPeriod",			sPeriod,				0 );

	//////////////////////////////////////////////////////////////////////////
	// 아이템 교환 테이블 구성

	// 테이블 키
	ItemExchangeKey kTableKey;
	kTableKey.first = iSrcItemID;
	kTableKey.second = iSourceQuantity;

	// 교환 정보
	ItemExchangeData kExchangeData;
	LIF( kExchangeData.AddExchangeData( eExchangeType, iDestItemID, fDestItemRate, iDestQuantity, sPeriod ) );

	std::map< int, ItemExchangeTable >::iterator mitET;
	mitET = m_mapItemExchangeTable.find( iHouseID );
	if( mitET == m_mapItemExchangeTable.end() )
	{
		// 교환 테이블
		ItemExchangeTable kExchangeTable;
		kExchangeTable.insert( std::make_pair( kTableKey, kExchangeData ) );
		m_mapItemExchangeTable.insert( std::make_pair( iHouseID, kExchangeTable ) );
	}
	else
	{
		ItemExchangeTable::iterator mitED;
		mitED = mitET->second.find( kTableKey );
		if( mitED == mitET->second.end() )
		{
			mitET->second.insert( std::make_pair( kTableKey, kExchangeData ) );
		}
		else
		{
			// 이미 교환 정보가 있으면 추가하기로 하자!
			LIF( mitED->second.AddExchangeData( eExchangeType, iDestItemID, fDestItemRate, iDestQuantity, sPeriod ) );
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

//{{ 2010. 9. 1	최육사	기간 초기화 아이템

bool CXSLItemManager::AddItemExpandPeriodData_LUA( int iItemID, short sPeriod )
{
	if( iItemID <= 0  ||  sPeriod <= 0 )
	{
		START_LOG( cerr, L"스크립트 정보가 이상합니다!" )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( sPeriod )
			<< END_LOG;
		return false;
	}

	if( GetItemTemplet( iItemID ) == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 존재하지 않습니다.!" )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( sPeriod )
			<< END_LOG;
		return false;
	}

	if( m_mapExpandPeriodItem.find( iItemID ) != m_mapExpandPeriodItem.end() )
	{
		START_LOG( cerr, L"이미 등록된 기간 초기화 아이템 정보입니다.!" )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( sPeriod )
			<< END_LOG;
		return false;
	}

	m_mapExpandPeriodItem.insert( std::make_pair( iItemID, sPeriod ) );
	return true;
}

//}}

//{{ 2010. 11. 29	최육사	기간제템 삭제
//}}


// @return : item id
// @iDestItemIndex : IET_SELECT인 경우에 유저가 고른 아이템 인덱스
//{{ 2013. 02. 19   교환 로그 추가 - 김민성
bool CXSLItemManager::GetItemExchangeResult( IN const int iHouseID,
											IN const int iSrcItemID,
											IN const int iSrcQuantity,
											IN const KItemInfo& kSrcItemInfo,
											IN const int iDestItemID,
											OUT std::map< int ,KItemInfo >& mapResultItem,
											OUT bool& bRandomExchange,
											OUT char& cExchangeType)
{
	mapResultItem.clear();

	std::map< int, ItemExchangeTable >::iterator mitET;
	mitET = m_mapItemExchangeTable.find( iHouseID );
	if( mitET == m_mapItemExchangeTable.end() )
	{
		START_LOG( cerr, L"해당 NPC와 관련된 교환 데이터는 존재하지 않습니다!" )
			<< BUILD_LOG( iHouseID )
			<< END_LOG;
		return false;
	}

	// 테이블 정보를 얻어냅니다!
	const ItemExchangeTable& kExchangeTable = mitET->second;

	// 해당 테이블에서 교환 대상이 되는 아이템 정보를 찾아 봅니다.
	ItemExchangeTable::const_iterator mit = kExchangeTable.find( ItemExchangeKey( iSrcItemID, iSrcQuantity ) );
	if( mit == kExchangeTable.end() )
	{
		START_LOG( cerr, L"교환 테이블에 없는 아이템입니다! 클라이언트에서 검사 했을텐데?" )
			<< BUILD_LOG( iSrcItemID )
			<< BUILD_LOG( iSrcQuantity )
			<< END_LOG;
		return false;
	}

	bRandomExchange = false;

	const ItemExchangeData& itemExchangeData = mit->second;
	switch( itemExchangeData.m_eExchangeType )
	{
	case IET_RANDOM:
		{
			bRandomExchange = true;

			int iResultItemID = itemExchangeData.m_kLottery.Decision();
			if( iResultItemID == KLottery::CASE_BLANK )
			{
				START_LOG( cerr, L"확률이 100%일텐데.." )
					<< BUILD_LOG( iHouseID )
					<< BUILD_LOG( iSrcItemID )
					<< END_LOG;

				return false;
			}

			int iQuantity = itemExchangeData.m_kLottery.GetParam1( iResultItemID );
			if( iQuantity <= 0 )
			{
				START_LOG( cerr, L"설정된 수량값이 이상하다!" )
					<< BUILD_LOG( iHouseID )
					<< BUILD_LOG( iSrcItemID )
					<< BUILD_LOG( iQuantity )
					<< END_LOG;

				return false;
			}

			const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iResultItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
					<< BUILD_LOG( iResultItemID )
					<< END_LOG;

				return false;
			}

			KItemInfo kInsertItemInfo;
			kInsertItemInfo.m_iItemID		= iResultItemID;
			kInsertItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
			kInsertItemInfo.m_iQuantity		= iQuantity;
			kInsertItemInfo.m_sEndurance	= pItemTemplet->m_Endurance;
			if( pItemTemplet->m_ItemType == CXSLItem::IT_WEAPON  ||
				pItemTemplet->m_ItemType == CXSLItem::IT_DEFENCE )
			{
				kInsertItemInfo.m_cEnchantLevel		 = kSrcItemInfo.m_cEnchantLevel;
				kInsertItemInfo.m_kAttribEnchantInfo = kSrcItemInfo.m_kAttribEnchantInfo;
				kInsertItemInfo.m_vecItemSocket		 = kSrcItemInfo.m_vecItemSocket;
				kInsertItemInfo.m_byteExpandedSocketNum	= kSrcItemInfo.m_byteExpandedSocketNum;
			}

			std::map< int, short >::const_iterator mitDestItemPeriod;
			mitDestItemPeriod = itemExchangeData.m_mapDestItemPeriod.find( kInsertItemInfo.m_iItemID );
			if( mitDestItemPeriod == itemExchangeData.m_mapDestItemPeriod.end() )
			{
				START_LOG( cerr, L"(기간제교환기능)유저가 고른 itemid가 이상합니다." )
					<< BUILD_LOG( iHouseID )
					<< BUILD_LOG( iSrcItemID )
					<< BUILD_LOG( kInsertItemInfo.m_iItemID )
					<< BUILD_LOG( (int) itemExchangeData.m_mapDestItemPeriod.size() )
					<< END_LOG;

				return false;
			}
			kInsertItemInfo.m_sPeriod = mitDestItemPeriod->second;

			mapResultItem.insert( std::make_pair( iResultItemID, kInsertItemInfo ) );
		}
		break;

	case IET_SELECT:
		{
			std::map< int, int >::const_iterator mitIE;
			mitIE = itemExchangeData.m_mapDestItemID.find( iDestItemID );
			if( mitIE == itemExchangeData.m_mapDestItemID.end() )
			{
				START_LOG( cerr, L"유저가 고른 itemid가 이상합니다." )
					<< BUILD_LOG( iHouseID )
					<< BUILD_LOG( iSrcItemID )
					<< BUILD_LOG( iDestItemID )
					<< BUILD_LOG( (int) itemExchangeData.m_mapDestItemID.size() )
					<< END_LOG;

				return false;
			}

			const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( mitIE->first );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
					<< BUILD_LOG( mitIE->first )
					<< END_LOG;

				return false;
			}

			KItemInfo kInsertItemInfo;
			kInsertItemInfo.m_iItemID		= mitIE->first;
			kInsertItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
			kInsertItemInfo.m_iQuantity		= mitIE->second;
			kInsertItemInfo.m_sEndurance	= pItemTemplet->m_Endurance;
			if( pItemTemplet->m_ItemType == CXSLItem::IT_WEAPON  ||
				pItemTemplet->m_ItemType == CXSLItem::IT_DEFENCE )
			{
				kInsertItemInfo.m_cEnchantLevel		 = kSrcItemInfo.m_cEnchantLevel;
				kInsertItemInfo.m_kAttribEnchantInfo = kSrcItemInfo.m_kAttribEnchantInfo;
				kInsertItemInfo.m_vecItemSocket		 = kSrcItemInfo.m_vecItemSocket;
				kInsertItemInfo.m_byteExpandedSocketNum	= kSrcItemInfo.m_byteExpandedSocketNum;
			}

			std::map< int, short >::const_iterator mitDestItemPeriod;
			mitDestItemPeriod = itemExchangeData.m_mapDestItemPeriod.find( kInsertItemInfo.m_iItemID );
			if( mitDestItemPeriod == itemExchangeData.m_mapDestItemPeriod.end() )
			{
				START_LOG( cerr, L"(기간제교환기능)유저가 고른 itemid가 이상합니다." )
					<< BUILD_LOG( iHouseID )
					<< BUILD_LOG( iSrcItemID )
					<< BUILD_LOG( kInsertItemInfo.m_iItemID )
					<< BUILD_LOG( (int) itemExchangeData.m_mapDestItemPeriod.size() )
					<< END_LOG;

				return false;
			}
			kInsertItemInfo.m_sPeriod = mitDestItemPeriod->second;

			mapResultItem.insert( std::make_pair( mitIE->first, kInsertItemInfo ) );
		}
		break;

	case IET_ALL:
		{
			std::map< int, int >::const_iterator mit = itemExchangeData.m_mapDestItemID.begin();
			for( ; mit != itemExchangeData.m_mapDestItemID.end(); ++mit )
			{
				const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( mit->first );
				if( pItemTemplet == NULL )
				{
					START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
						<< BUILD_LOG( mit->first )
						<< END_LOG;

					continue;
				}

				KItemInfo kInsertItemInfo;
				kInsertItemInfo.m_iItemID	 = mit->first;
				kInsertItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
				kInsertItemInfo.m_iQuantity	 = mit->second;
				kInsertItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
				if( pItemTemplet->m_ItemType == CXSLItem::IT_WEAPON  ||
					pItemTemplet->m_ItemType == CXSLItem::IT_DEFENCE )
				{
					kInsertItemInfo.m_cEnchantLevel		 = kSrcItemInfo.m_cEnchantLevel;
					kInsertItemInfo.m_kAttribEnchantInfo = kSrcItemInfo.m_kAttribEnchantInfo;
					kInsertItemInfo.m_vecItemSocket		 = kSrcItemInfo.m_vecItemSocket;
					kInsertItemInfo.m_byteExpandedSocketNum	= kSrcItemInfo.m_byteExpandedSocketNum;
				}

				std::map< int, short >::const_iterator mitDestItemPeriod;
				mitDestItemPeriod = itemExchangeData.m_mapDestItemPeriod.find( kInsertItemInfo.m_iItemID );
				if( mitDestItemPeriod == itemExchangeData.m_mapDestItemPeriod.end() )
				{
					START_LOG( cerr, L"(기간제교환기능)유저가 고른 itemid가 이상합니다." )
						<< BUILD_LOG( iHouseID )
						<< BUILD_LOG( iSrcItemID )
						<< BUILD_LOG( kInsertItemInfo.m_iItemID )
						<< BUILD_LOG( (int) itemExchangeData.m_mapDestItemPeriod.size() )
						<< END_LOG;

					return false;
				}
				kInsertItemInfo.m_sPeriod = mitDestItemPeriod->second;

				mapResultItem.insert( std::make_pair( mit->first, kInsertItemInfo ) );
			}
		}
		break;

	default:
		{
			START_LOG( cerr, L"이상한 교환 타입입니다." )
				<< BUILD_LOG( iHouseID )
				<< BUILD_LOG( iSrcItemID )
				<< BUILD_LOG( (int)itemExchangeData.m_eExchangeType )
				<< END_LOG;

			return false;
		}
		break;
	}

	cExchangeType = static_cast<char>(itemExchangeData.m_eExchangeType);

	return true;
}
//}

// 봉인 가능 아이템
bool CXSLItemManager::IsCanSealForCashItem( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 없습니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	// 장비 또는 아바타 아이템인지 확인
	if( pItemTemplet->m_ItemType != CXSLItem::IT_WEAPON  &&
		pItemTemplet->m_ItemType != CXSLItem::IT_DEFENCE  &&
		pItemTemplet->m_ItemType != CXSLItem::IT_ACCESSORY  &&
		//{{ 2012. 03. 03	박세훈	캐시펫구입시 결정체 1회 거래가능하도록 봉인된 상태로 나옴
		pItemTemplet->m_ItemType != CXSLItem::IT_SPECIAL  &&
		//}}
		pItemTemplet->m_bFashion == false )
	{
		return false;
	}

	// 봉인 가능 횟수가 1이상인것만
	if( pItemTemplet->m_ucMaxSealCount <= 0 )
	{
        return false;
	}

	return true;
}

bool CXSLItemManager::IsCanSealForRandomItem( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 없습니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}


	// 봉인 가능 횟수가 1이상인것만
	if( pItemTemplet->m_ucMaxSealCount <= 0 )
	{
		return false;
	}

	return true;
}

//{{ 2010. 03. 22  최육사	기술의 노트

bool CXSLItemManager::IsSkillNoteMemoItem( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 없습니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	// 기술의 노트 메모 아이템인지 여부
	return ( pItemTemplet->m_ItemType == CXSLItem::IT_SKILL_MEMO );
}

//}}

//{{ 2010. 9. 1	최육사	기간 초기화 아이템

bool CXSLItemManager::GetExpandPeriodByItemID( IN int iItemID, OUT short& sExpandPeriod ) const
{
	std::map< int, short >::const_iterator mit;
	mit = m_mapExpandPeriodItem.find( iItemID );
	if( mit == m_mapExpandPeriodItem.end() )
	{
		return false;
	}

	sExpandPeriod = mit->second;
	return true;
}

//}}

//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
bool CXSLItemManager::IsFantasticKewpielJelly( const int iItemID )
{
	switch( iItemID )
	{
	case 99910:
	case 99911:
	case 99912:
	case 99913:
	case 99914:
	case 99915:
	case 99916:
	case 99978:

		/*
			99910 환상적인 큐피엘 젤리 (STEP1)
			99911 환상적인 큐피엘 젤리 (STEP2)
			99912 환상적인 큐피엘 젤리 (STEP3)
			99913 환상적인 큐피엘 젤리 (STEP4)
			99914 환상적인 큐피엘 젤리 (STEP5)
			99915 환상적인 큐피엘 젤리 (STEP6)
			99916 환상적인 큐피엘 젤리 (STEP7)
#ifdef SERV_NO_USE_KEWPIEL_JELLY_STEP8_BUG_FIX
			99978 환상적인 큐피엘 젤리 (STEP8)
#endif //SERV_NO_USE_KEWPIEL_JELLY_STEP8_BUG_FIX
		*/
		return true;
	}
	return false;
}

bool CXSLItemManager::IsOddKewpielJelly( const int iItemID )
{
	switch( iItemID )
	{
	case 99920:
	case 99921:
	case 99922:
	case 99923:
	case 99924:
	case 99925:
	case 99926:
	case 99979:
		/*
			99920 이상한 큐피엘 젤리 (STEP1)
			99921 이상한 큐피엘 젤리 (STEP2)
			99922 이상한 큐피엘 젤리 (STEP3)
			99923 이상한 큐피엘 젤리 (STEP4)
			99924 이상한 큐피엘 젤리 (STEP5)
			99925 이상한 큐피엘 젤리 (STEP6)
			99926 이상한 큐피엘 젤리 (STEP7)
#ifdef SERV_NO_USE_KEWPIEL_JELLY_STEP8_BUG_FIX
			99979 이상한 큐피엘 젤리 (STEP8)
#endif //SERV_NO_USE_KEWPIEL_JELLY_STEP8_BUG_FIX
		*/
		return true;
	}
	return false;
}
//}}

//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
bool CXSLItemManager::AddShopItemList_LUA()
{
	KLuaManager luaManager( GetLuaState() );
	TableBind( &luaManager );

	ItemIDList itemIDList;

	int villiageID;
	LUA_GET_VALUE_RETURN(	luaManager, L"VILLIAGE_ID",	villiageID,	0, goto end_proc );

	if( luaManager.BeginTable( L"SALE_GROUP" ) == S_OK )
	{
		int index = 1;
		int buf;
		while( luaManager.GetValue(index, buf) == S_OK )
		{
			const int iSaleGroup = buf;
			// 서버에서는 파싱할 필요가 없음

			index++;
		}

		luaManager.EndTable();
	}

	if( luaManager.BeginTable( L"SALE_ITEM" ) == S_OK )
	{
		int index = 1;
		int buf;
		while( luaManager.GetValue(index, buf) == S_OK )
		{
			// 판매 아이템 리스트를 넣는다!
			m_setNpcSellEDItemList.insert( buf );
			index++;
		}
		luaManager.EndTable();
	}

	// 특정 npc만이 판매하는 물품
	if( luaManager.BeginTable( L"SALE_ITEM_NPC" ) == S_OK )
	{
		int index = 1;
		while( luaManager.BeginTable( index ) == S_OK )
		{
			NpcItemIDList itemIDList;
			int npcID;
			LUA_GET_VALUE_RETURN(	luaManager, L"NPC_ID",	npcID,	0, goto end_proc );

			itemIDList.m_NpcId = npcID;

			if( luaManager.BeginTable( L"ITEM_LIST" ) == S_OK )
			{
				int index = 1;
				int buf;
				while( luaManager.GetValue(index, buf) == S_OK )
				{
					// 판매 아이템 리스트를 넣는다!
					m_setNpcSellEDItemList.insert( buf );
					index++;
				}
				luaManager.EndTable();
			}

			index++;
			luaManager.EndTable();
		}

		luaManager.EndTable();
	}

	return true;

end_proc:
	START_LOG( cerr, L"스크립트 파싱 에러!" )
		<< BUILD_LOG( villiageID )
		<< END_LOG;
	return false;
}

bool CXSLItemManager::AddShopItemGroup_LUA()
{
	// 서버에서는 파싱할 필요가 없는 함수 입니다.
	return true;
}
//}}

//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}


//{{ 2012. 04. 04	최육사	배틀필드 시스템
bool CXSLItemManager::AddCoolTimeGroupItem_LUA( int iCoolTimeGroupEnum, int iItemID )
{
	if( iCoolTimeGroupEnum < 0  ||  iItemID < 0 )
	{
		START_LOG( cerr, L"스크립트 정보가 이상합니다!" )
			<< BUILD_LOG( iCoolTimeGroupEnum )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	m_mapCoolTimeItemGroup.insert( std::make_pair( iItemID, static_cast<CXSLItem::COOLTIME_ITEM_GROUP>(iCoolTimeGroupEnum) ) );
	return true;
}
//}}

//{{ 2011. 07. 08    김민성    옵션 수치화
void CXSLItemManager::UpdateSetIDAndMaxLevelMap( const int iSetID, const int iLevel )
{
	std::map< int, int >::iterator mit = m_mapSetIDAndMaxLevel.find( iSetID );

	// 없는 SetID 이면 추가
	if ( m_mapSetIDAndMaxLevel.end() == mit )
	{
		m_mapSetIDAndMaxLevel.insert( std::make_pair( iSetID, iLevel ) );
	}
	// 기존에 있는 SetID 이면
	else
	{
		// 기존의 레벨보다 크면 갱신
		if ( mit->second < iLevel )
			mit->second = iLevel;
	}
}

int CXSLItemManager::GetSetItemMaxLevel( const int iSetID_ )
{
	std::map< int, int >::iterator mit = m_mapSetIDAndMaxLevel.find( iSetID_ );

	if( mit != m_mapSetIDAndMaxLevel.end() )
	{
		return mit->second;
	}

	return 0;
}
//}}

//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
//{{ 2013. 05. 15	최육사	아이템 개편
bool CXSLItemManager::GetItemLevel( IN const char cUnitType, IN const char cUnitClass, IN const KItemInfo& kItemInfo, OUT int& iItemLevel )
//}}
{
	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( kItemInfo.m_iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 존재하지 않습니다!" )
			<< BUILD_LOG( kItemInfo.m_iItemID )
			<< END_LOG;
		return false;
	}

	//{{ 2013. 05. 15	최육사	아이템 개편
	KStat kItemBaseStat;
	if( pItemTemplet->IsCommonItem() == true )
	{
		// 각 소켓 정보들을 수집해서
		KStatRelationLevel kAddStatRelLevel;
		SiCXSLSocketItem()->GetAndAddStatRelationLevel( kItemInfo.m_vecItemSocket, kAddStatRelLevel );
		SiCXSLSocketItem()->GetAndAddStatRelationLevel( kItemInfo.m_vecRandomSocket, kAddStatRelLevel );
		SiCXSLSocketItem()->GetAndAddStatRelationLevel( pItemTemplet->m_vecSocketOption, kAddStatRelLevel );

		// 공용 아이템의 기본 스탯
		KStat kCommonItemStat;
		LIF( SiCXSLCharacterStatTable()->CalcCommonItemStat( pItemTemplet, cUnitType, cUnitClass, kAddStatRelLevel, kCommonItemStat ) );

		kItemBaseStat.Init();
		kItemBaseStat.AddStat( kCommonItemStat );
	}
	else
	{
		// 일반 아이템 기본 스탯
		kItemBaseStat.Init();
		kItemBaseStat.m_iBaseHP    += (int)( pItemTemplet->m_Stat.m_fBaseHP );
		kItemBaseStat.m_iAtkPhysic += (int)( pItemTemplet->m_Stat.m_fAtkPhysic );
		kItemBaseStat.m_iAtkMagic  += (int)( pItemTemplet->m_Stat.m_fAtkMagic );
		kItemBaseStat.m_iDefPhysic += (int)( pItemTemplet->m_Stat.m_fDefPhysic );
		kItemBaseStat.m_iDefMagic  += (int)( pItemTemplet->m_Stat.m_fDefMagic );
	}
	//}}

	// 강화 적용
	float fEnchantRate = 0.f;
	if( SiCXSLEnchantItemManager()->GetEnchantRate( ( int )kItemInfo.m_cEnchantLevel, fEnchantRate ) == false )
	{
		fEnchantRate = 0.f;
	}

	// BaseHP 스텟도 강화 적용 되게 변경
	// 인첸트로 증가된 수치
	KStat kItemEnchantStat;
	kItemEnchantStat.m_iBaseHP			= (int)(( kItemBaseStat.m_iBaseHP		* fEnchantRate ) - kItemBaseStat.m_iBaseHP);
	kItemEnchantStat.m_iAtkPhysic		= (int)(( kItemBaseStat.m_iAtkPhysic	* fEnchantRate ) - kItemBaseStat.m_iAtkPhysic);
	kItemEnchantStat.m_iAtkMagic		= (int)(( kItemBaseStat.m_iAtkMagic		* fEnchantRate ) - kItemBaseStat.m_iAtkMagic);
	kItemEnchantStat.m_iDefPhysic		= (int)(( kItemBaseStat.m_iDefPhysic	* fEnchantRate ) - kItemBaseStat.m_iDefPhysic);
	kItemEnchantStat.m_iDefMagic		= (int)(( kItemBaseStat.m_iDefMagic		* fEnchantRate ) - kItemBaseStat.m_iDefMagic);

	// 강화에 따른 아이템 레벨 정보 얻기
	int iEnchantItemLevel = 0;
	GetCaculatedItemLevelByEnchantStat( kItemEnchantStat, iEnchantItemLevel );

	// 소켓 옵션에 따른 아이템 레벨 정보 얻기(추가 레벨)
	int iSocketItemLevel = 0;
	BOOST_TEST_FOREACH( int, iSocket, kItemInfo.m_vecItemSocket )
	{
		if( iSocket > 0 )
		{
			iSocketItemLevel += 2;
		}
	}

	// 아이템 레벨 합산 결과
	iItemLevel = pItemTemplet->m_iItemLevel + iEnchantItemLevel + iSocketItemLevel;
	return true;
}

void CXSLItemManager::GetCaculatedItemLevelByEnchantStat( IN const KStat& kEnchantItemStat, OUT int& iItemLevel )
{
	iItemLevel = 0;

	float fItemLevel = 0.f;

	fItemLevel += (float)(kEnchantItemStat.m_iBaseHP) / IESV_BASE_HP;

	fItemLevel += (float)(kEnchantItemStat.m_iAtkPhysic) / IESV_PHYSIC_ATTACK;

	fItemLevel += (float)(kEnchantItemStat.m_iAtkMagic) / IESV_MAGIC_ATTACK;

	fItemLevel += (float)(kEnchantItemStat.m_iDefPhysic) / IESV_PHYSIC_DEFENCE;

	fItemLevel += (float)(kEnchantItemStat.m_iDefMagic) / IESV_MAGIC_DEFENCE;

	iItemLevel = static_cast<int>(fItemLevel);
}
//}}

//{{ 2012. 04. 04	최육사	배틀필드 시스템
bool CXSLItemManager::IsCanUseQuickSlotItemInVillage( IN const int iItemID ) const
{
	std::map< int, CXSLItem::COOLTIME_ITEM_GROUP >::const_iterator mit;
	mit = m_mapCoolTimeItemGroup.find( iItemID );
	if( mit == m_mapCoolTimeItemGroup.end() )
	{
		// 없으면 사용 못함!
		return false;
	}

	switch( mit->second )
	{
	case CXSLItem::CIG_THROW_ITEM:
		return false;
	}

	return true;
}
//}}

//{{ 버프 관리 시스템 - 김민성
bool CXSLItemManager::IsBuffItem( IN int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"아이템 정보가 존재하지 않습니다!" )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	if( pItemTemplet->m_iBuffFactorID > 0)
		return true;

	return false;
}
//}

//{{ 2013. 05. 20	최육사	아이템 개편
void CXSLItemManager::ItemSealProcess( IN const int iItemID
									 , IN const short sPeriod
									 , OUT unsigned char& ucSealData
									 ) const
{
	if( 0 < sPeriod)
	{
		return;
	}

	const CXSLItem::ItemTemplet* pItemTemplet = GetItemTemplet( iItemID );
	if( IS_NULL( pItemTemplet ) )
	{
		START_LOG( cerr, L"존재 하지 않는 아이템 정보 입니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	if( pItemTemplet->m_ucMaxSealCount <= 0 )
		return;

	switch( pItemTemplet->m_ItemGrade )
	{
	case CXSLItem::IG_ELITE:
	case CXSLItem::IG_UNIQUE:
		break;

	default:
		return;
	}

	switch( pItemTemplet->m_ItemType )
	{
	case CXSLItem::IT_WEAPON:
	case CXSLItem::IT_DEFENCE:
	case CXSLItem::IT_ACCESSORY:
		break;

	default:
		return;
	}

	// 봉인!
    ucSealData = 100;
}

bool CXSLItemManager::AddItemConvertGroup_LUA( int iGroupID, int iItemID )
{
	if( iGroupID <= 0  ||  iItemID <= 0 )
	{
		START_LOG( cerr, L"스크립트 정보가 이상합니다!" )
			<< BUILD_LOG( iGroupID )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	std::map< int, std::vector< int > >::iterator mit;
	mit = m_mapItemConvertGroup.find( iGroupID );
	if( mit == m_mapItemConvertGroup.end() )
	{
		std::vector< int > vecItemID;
		vecItemID.push_back( iItemID );
		m_mapItemConvertGroup.insert( std::make_pair( iGroupID, vecItemID ) );
	}
	else
	{
        mit->second.push_back( iItemID );
	}
	return true;
}

bool CXSLItemManager::AddItemConvertInfo_LUA()
{
	KLuaManager luaManager( GetLuaState() );
	TableBind( &luaManager );

	std::vector< int > vecTargetItemID;
	SItemConvertInfo kConvertInfo;

	int iTargetItemGroup = 0;
	LUA_GET_VALUE(	luaManager, L"m_TargetItemGroup",		iTargetItemGroup,							0 );
	if( iTargetItemGroup > 0 )
	{
		std::map< int, std::vector< int > >::const_iterator mit;
		mit = m_mapItemConvertGroup.find( iTargetItemGroup );
		if( mit == m_mapItemConvertGroup.end() )
		{
            START_LOG( cerr, L"존재 하지 않는 GroupID가 지정되었습니다." )
				<< BUILD_LOG( iTargetItemGroup )
				<< END_LOG;
		}
		else
		{
			// ItemID리스트를 복사한다.
			vecTargetItemID = mit->second;
		}
	}

	if( luaManager.BeginTable( "m_TargetItemID" ) == S_OK )
	{
		int iItemID = 0;
		int index = 1;
		while( luaManager.GetValue( index, iItemID ) == S_OK  )
		{
			vecTargetItemID.push_back( iItemID );
			++index;
		}

		luaManager.EndTable();
	}

	LUA_GET_VALUE(	luaManager, L"m_ConvertType",			kConvertInfo.m_iConverType,					0 );
	LUA_GET_VALUE(	luaManager, L"m_Commission",			kConvertInfo.m_iCommission,					0 );

	if( luaManager.BeginTable( "m_ResultItemID" ) == S_OK )
	{
		int iItemID = 0;
		int index = 1;
		while( luaManager.GetValue( index, iItemID ) == S_OK )
		{
			kConvertInfo.m_kLotResultItemID.AddCaseSameProb( iItemID );
			++index;
		}

		luaManager.EndTable();
	}

	// 파싱한 아이템 교환 정보를 저장하자!
	BOOST_TEST_FOREACH( const int, iItemID, vecTargetItemID )
	{
		m_mapItemConvertTable.insert( std::make_pair( iItemID, kConvertInfo ) );
	}

	return true;
}

bool CXSLItemManager::GetResultItemConvert( IN const int iTargetItemID, OUT int& iResultItemID, OUT int& iCommissionED ) const
{
	iResultItemID = 0;
	iCommissionED = 0;

	std::map< int, SItemConvertInfo >::const_iterator mit;
	mit = m_mapItemConvertTable.find( iTargetItemID );
	if( mit == m_mapItemConvertTable.end() )
	{
		// 에러가 아닙니다.
		START_LOG( clog, L"교환할 아이템 정보가 없습니다!" )
			<< BUILD_LOG( iTargetItemID )
			<< END_LOG;
		return false;
	}

	// 아이템 결과
	const int iResult = mit->second.m_kLotResultItemID.DecisionSameProb();
	if( iResult == KLotterySameProb<int>::CASE_BLANK )
	{
		START_LOG( cerr, L"아이템 교환 랜덤 결과값이 이상합니다." )
			<< BUILD_LOG( iTargetItemID )
			<< BUILD_LOG( iResult )
			<< END_LOG;
		return false;
	}

	iResultItemID = iResult;
	iCommissionED = mit->second.m_iCommission;
	return true;
}
//}}

//{{ 2013. 04. 01	 인연 시스템 - 김민성
void CXSLItemManager::AddWeddingHallItem_LUA( int iWeddinHallType, int iItemID )
{
	char cType = static_cast<char>(iWeddinHallType);
	m_mapWeddingHallTypeItem.insert( std::make_pair(cType, iItemID) );
}

int CXSLItemManager::GetWeddingHallItem( const char cWeddingHall )
{
	std::map<char,int>::iterator mit = m_mapWeddingHallTypeItem.find( cWeddingHall );
	if( mit == m_mapWeddingHallTypeItem.end() )
	{
		return 0;
	}
	return mit->second;
}

void CXSLItemManager::AddWeddingCompleteRewardItem_LUA( IN int iItemID, IN int iCount )
{
	if( iItemID <= 0 || iCount <= 0 )
	{
		START_LOG( cerr, L"정상적인 아이템이 아닙니다." )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( iCount )
			<< END_LOG;
		return;
	}

	std::map<int,int>::iterator mit = m_mapWeddingCompleteRewardItem.find( iItemID );
	if( mit != m_mapWeddingCompleteRewardItem.end() )
	{
		START_LOG( cerr, L"동일한 아이템이 존재합니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	m_mapWeddingCompleteRewardItem.insert( std::make_pair( iItemID, iCount) );
}

void CXSLItemManager::GetWeddingCompleteRewardItem( OUT std::map<int, int>& mapItemList )
{
	mapItemList = m_mapWeddingCompleteRewardItem;
}

void CXSLItemManager::AddCoupleAnniversaryDayRewardItem_LUA( int iItemID )
{
	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"정상적인 아이템이 아닙니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	BOOST_TEST_FOREACH( int, iID, m_vecCoupleAnniversaryDayRewardItem )
	{
		if( iItemID == iID )
		{
			START_LOG( cerr, L"동일한 아이템이 존재합니다." )
				<< BUILD_LOG( iItemID )
				<< END_LOG;
			return;
		}
	}

	m_vecCoupleAnniversaryDayRewardItem.push_back( iItemID );
}

void CXSLItemManager::GetCoupleAnniversaryDayRewardItem( OUT std::vector<int>& vecItemList )
{
	vecItemList = m_vecCoupleAnniversaryDayRewardItem;
}

void CXSLItemManager::AddWeddingAnniversaryDayRewardItem_LUA( int iItemID )
{
	if( iItemID <= 0 )
	{
		START_LOG( cerr, L"정상적인 아이템이 아닙니다." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	BOOST_TEST_FOREACH( int, iID, m_vecWeddingAnniversaryDayRewardItem )
	{
		if( iItemID == iID )
		{
			START_LOG( cerr, L"동일한 아이템이 존재합니다." )
				<< BUILD_LOG( iItemID )
				<< END_LOG;
			return;
		}
	}

	m_vecWeddingAnniversaryDayRewardItem.push_back( iItemID );
}

void CXSLItemManager::GetWeddingAnniversaryDayRewardItem( OUT std::vector<int>& vecItemList )
{
	vecItemList = m_vecWeddingAnniversaryDayRewardItem;
}

void CXSLItemManager::AddWeddingAnniversaryDayRewardTitle_LUA( int iDays, int iItemID )
{
	if( iItemID <= 0 || iDays <= 0 )
	{
		START_LOG( cerr, L"정상적인 날짜 및 아이템이 아닙니다." )
			<< BUILD_LOG( iDays )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	std::map<int,int>::iterator mit = m_mapWeddingAnniversaryDayRewardTitle.find( iDays );
	if( mit != m_mapWeddingAnniversaryDayRewardTitle.end() )
	{
		START_LOG( cerr, L"동일한 날짜가 존재합니다." )
			<< BUILD_LOG( iDays )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	m_mapWeddingAnniversaryDayRewardTitle.insert( std::make_pair( iDays, iItemID) );
}

int CXSLItemManager::GetWeddingAnniversaryDayRewardTitle( const int iDays )
{
	std::map<int,int>::iterator mit = m_mapWeddingAnniversaryDayRewardTitle.find( iDays );
	if( mit != m_mapWeddingAnniversaryDayRewardTitle.end() )
	{
		return mit->second;
	}

	return 0;
}
//}

int	CXSLItemManager::GetItemCSPoint( IN const int iItemID )
{
	switch( iItemID )
	{
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
	case CXSLItem::CI_SKILL_POINT_60_USE_INVEN:
	case CXSLItem::CI_SKILL_POINT_60_USE_INVEN_ARA:
	case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
	case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
		return CXSLItem::CSP_60_POINT;

	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_15_DAY:
	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_30_DAY:
	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_60_DAY:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_DAY_7_USE_INVEN:
	case CXSLItem::CI_SKILL_POINT_30_USE_INVEN:
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
	case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
		return CXSLItem::CSP_30_POINT;

	case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
		return CXSLItem::CSP_130_POINT;
	default:
		return CXSLItem::CSP_NONE;
	}
}

int	CXSLItemManager::GetItemCSPointPeriod( IN const int iItemID )
{
	switch( iItemID )
	{
	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_60_DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_30_60DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_60DAY:
		return 60;

	case CXSLItem::CI_SKILL_POINT_60_USE_INVEN_ARA:
	case CXSLItem::CI_CASH_SKILL_POINT_60_30:
	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_30_DAY:
	case CXSLItem::CI_CASH_SKILL_POINT_30_30:
	case CXSLItem::EI_SKILL_POINT_60_30DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_10_30DAY_USE_INVEN_JP:
	case CXSLItem::EI_LURIEL_GNOSIS_30_30DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_30DAY:
		return 30;

	case CXSLItem::CI_SKILL_POINT_60_USE_INVEN:
	case CXSLItem::CI_SKILL_POINT_30_USE_INVEN:
	case CXSLItem::SI_COME_BACK_REWARD_SKILL_30_POINT_15_DAY:
	case CXSLItem::CI_CASH_SKILL_POINT_30_15:
	case CXSLItem::CI_CASH_SKILL_POINT_60_15:
	case CXSLItem::EI_SKILL_POINT_60_15DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_10_15DAY_USE_INVEN_JP:
	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN_JP:
	case CXSLItem::EI_LURIEL_GNOSIS_30_15DAY:
	case CXSLItem::EI_LURIEL_GNOSIS_60_15DAY:
		return 15;

	case CXSLItem::EI_SKILL_POINT_30_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_14DAY_USE_INVEN:
		return 14;

	case CXSLItem::EI_SKILL_POINT_30_DAY_7_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_60_7DAY_USE_INVEN_2:
	case CXSLItem::EI_SKILL_POINT_30_7DAY_USE_INVEN_2:
		case CXSLItem::EI_LURIEL_GNOSIS_30_7DAY:
		case CXSLItem::EI_LURIEL_GNOSIS_60_7DAY:
		return 7;

	case CXSLItem::EI_SKILL_POINT_130_1DAY_USE_INVEN:
		return 1;
	case CXSLItem::EI_SKILL_POINT_60_1DAY_USE_INVEN:
	case CXSLItem::EI_SKILL_POINT_30_1DAY_USE_INVEN:
		return 1;
	default:
		return 0;
	}
}

bool CXSLItemManager::IsKeepItemShowItem( const int iItemID )
{

	std::map< int , std::vector< int > >::iterator mit;
	mit = m_mapKeepShowItem.find(iItemID);
	if( mit != m_mapKeepShowItem.end() )
	{
		return true;
	}
	return false;
}

bool CXSLItemManager::AddKeepShowItem_LUA(void)
{
	KLuaManager luaManager( GetLuaState() );
	TableBind( &luaManager );

	m_vecKeepItem_Key.clear();
	kDisCountItemInfo m_DisCountItemInfo;
	LUA_GET_VALUE( luaManager, L"m_Type", m_InfoType, 0 );
	LUA_GET_VALUE( luaManager, L"m_ShowItem", m_ShowItem, 0 );
	LUA_GET_VALUE( luaManager, L"m_ProDuctNumber_One", m_DisCountItemInfo.m_DisCountProNo , 0 );
	LUA_GET_VALUE( luaManager, L"m_ProDuctNumber_One", m_DisCountItemInfo.m_BasicProNo , 0 );
	if( S_OK == luaManager.BeginTable( "KEEP_SHOW_ITEM_KEY") )
	{
		int KeepItem_Key = 0;
		int Tempindex = 1;
		while( S_OK == luaManager.GetValue( Tempindex,KeepItem_Key ) )
		{
			m_vecKeepItem_Key.push_back( KeepItem_Key );
			Tempindex++;
		}
		luaManager.EndTable();
	}
	if( m_InfoType == 1 )
	{
		m_mapKeepShowItem.insert(std::make_pair( m_ShowItem , m_vecKeepItem_Key));
	}
	else if( m_InfoType == 2 )
	{
		m_DisCountItemInfo.m_DisCountKeepItem = m_vecKeepItem_Key;
		m_DisCountInfoMap.insert( std::make_pair( m_ShowItem , m_DisCountItemInfo) );
	}
	return false;
}

bool CXSLItemManager::IsInventoryOnly( IN const int iItemID )
{
	std::map< int, CXSLItem::ItemTemplet >::iterator mitItemTemplet = m_ItemTempletIDMap.find( iItemID );

	if( mitItemTemplet == m_ItemTempletIDMap.end() )
		return false;

	return mitItemTemplet->second.m_bInventoryOnly;
}
