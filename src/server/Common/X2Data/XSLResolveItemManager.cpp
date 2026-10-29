#include "./XSLResolveItemManager.h"
#include "NetError.h"
#include "XSLItemManager.h"

ImplementRefreshSingleton( CXSLResolveItemManager );

CXSLResolveItemManager::CXSLResolveItemManager(void)
{
	m_iMinJackpotTime	= 0;
	m_iMaxJackpotTime	= 0;
}

CXSLResolveItemManager::~CXSLResolveItemManager(void)
{
}

ImplToStringW( CXSLResolveItemManager )
{
	stm_	<< L"----------[ Resolve Item Manager ]----------" << std::endl
		//{{ 2011. 01. 19	최육사	아이템 분해 개편
			<< TOSTRINGW( m_vecBrokenPieceResolve[RRT_EL_FLOUR].size() )
			<< TOSTRINGW( m_vecBrokenPieceResolve[RRT_WEAPON_BROKEN_PIECE].size() )
			<< TOSTRINGW( m_vecBrokenPieceResolve[RRT_ARMOR_BROKEN_PIECE].size() )
		//}}
			<< TOSTRINGW( m_mapResolveData.size() )
			<< TOSTRINGW( m_iMinJackpotTime )
			<< TOSTRINGW( m_iMaxJackpotTime )
		;

	return stm_;
}

ImplementLuaScriptParser( CXSLResolveItemManager )
{
	lua_tinker::class_add<CXSLResolveItemManager>( GetLuaState(), "CXSLResolveItemManager" );
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "SetJackpotProb",						&CXSLResolveItemManager::SetJackpotProb_LUA );
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "AddResolveDataBrokenPiece",			&CXSLResolveItemManager::AddResolveDataBrokenPiece_LUA );
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "AddResolveData",						&CXSLResolveItemManager::AddResolveData_LUA );
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "SetMinMaxOfJackpotTime",				&CXSLResolveItemManager::SetMinMaxOfJackpotTime_LUA );
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "AddWeaponResolveBrokenPieceRate",	&CXSLResolveItemManager::AddWeaponResolveBrokenPieceRate_LUA );
	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "AddArmorResolveBrokenPieceRate",		&CXSLResolveItemManager::AddArmorResolveBrokenPieceRate_LUA );
	//}}

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}

	lua_tinker::class_def<CXSLResolveItemManager>( GetLuaState(), "dump",								&CXSLResolveItemManager::Dump );

	lua_tinker::decl( GetLuaState(), "g_pResolveItemManager", this );
}

bool CXSLResolveItemManager::SetJackpotProb_LUA( float fRate )
{
	if( !m_kJackpotLottery.AddCase( JP_JACKPOT, fRate ) )
	{
		START_LOG( cerr, L"대박 확률 설정 실패!" )
			<< BUILD_LOG( fRate )
			<< END_LOG;
		return false;
	}

	return true;
}


//{{ 2011. 01. 19	최육사	아이템 분해 개편
//////////////////////////////////////////////////////////////////////////

bool CXSLResolveItemManager::AddResolveDataBrokenPiece_LUA( int iResolveResultType, int iItemID )
{
	if( iItemID <= 0  ||  iResolveResultType < RRT_EL_FLOUR  ||  iResolveResultType >= RRT_MAX )
	{
		START_LOG( cerr, L"분해 그룹 데이터 파싱 실패.!" )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( iResolveResultType )
			<< END_LOG;

		return false;
	}

	std::map< int, BrokenPieceResolve >::iterator mit;

	BrokenPieceResolve kResolveData;
	kResolveData.m_iItemID = iItemID;

	KLuaManager luaMgr( GetLuaState() );
	LUA_GET_VALUE_RETURN( luaMgr, "iUnitCost",	kResolveData.m_iUnitCost,	 0,		goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iBeginLv",	kResolveData.m_iBeginLv,	 0,		goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iEndLv",		kResolveData.m_iEndLv,		 0,		goto error_proc; );

	m_vecBrokenPieceResolve[iResolveResultType].push_back( kResolveData );

	return true;

error_proc:
	START_LOG( cerr, L"분해 스크립트 파싱 실패!" )
		<< BUILD_LOG( iResolveResultType )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( kResolveData.m_iUnitCost )
		<< BUILD_LOG( kResolveData.m_iBeginLv )
		<< BUILD_LOG( kResolveData.m_iEndLv )
		<< END_LOG;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

bool CXSLResolveItemManager::AddResolveData_LUA( int iItemGrade, int iItemID )
{
	if( iItemGrade <= 0 || iItemID <= 0 )
	{
		START_LOG( cerr, L"분해 마법석 데이터 파싱 실패.!" )
			<< BUILD_LOG( iItemGrade )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	std::map< int, std::vector< SResolveData > >::iterator mit;

	SResolveData resolveData;
	resolveData.m_iResultItemID = iItemID;

	float fRate = 0.0f;

	KLuaManager luaMgr( GetLuaState() );
	LUA_GET_VALUE_RETURN( luaMgr, "fSuccRate",		fRate,							 0.0f,	goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iRandomCnt",		resolveData.m_iRandomCount,		 0,		goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iResultCnt",		resolveData.m_iResultCount,		 0,		goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iBeginLv",		resolveData.m_iBeginLv,			 0,		goto error_proc; );
	LUA_GET_VALUE_RETURN( luaMgr, "iEndLv",			resolveData.m_iEndLv,			 0,		goto error_proc; );

	resolveData.m_kLottery.AddCase( iItemID, fRate );

	mit = m_mapResolveData.find( iItemGrade );
	if( mit == m_mapResolveData.end() )
	{
		std::vector< SResolveData > vecResolveData;
		vecResolveData.push_back( resolveData );
		m_mapResolveData.insert( std::make_pair( iItemGrade, vecResolveData ) );
	}
	else
	{
		mit->second.push_back( resolveData );
	}

	return true;

error_proc:
	START_LOG( cerr, L"스크립트 파싱 실패!" )
		<< BUILD_LOG( iItemGrade )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( fRate )
		<< BUILD_LOG( resolveData.m_iRandomCount )
		<< BUILD_LOG( resolveData.m_iResultCount )
		<< BUILD_LOG( resolveData.m_iBeginLv )
		<< BUILD_LOG( resolveData.m_iEndLv )
		<< END_LOG;

	return false;
}

//{{ 2011. 01. 19	최육사	아이템 분해 개편
void CXSLResolveItemManager::AddWeaponResolveBrokenPieceRate_LUA( int iItemType, float fRate )
{
	if( m_kWeaponResolveLottery.AddCase( iItemType, fRate ) == false )
	{
		START_LOG( cerr, L"무기 분해 파편 확률 세팅 실패!" )
			<< BUILD_LOG( iItemType )
			<< BUILD_LOG( fRate )
			<< END_LOG;
	}
}

void CXSLResolveItemManager::AddArmorResolveBrokenPieceRate_LUA( int iItemType, float fRate )
{
	if( m_kArmorResolveLottery.AddCase( iItemType, fRate ) == false )
	{
		START_LOG( cerr, L"방어구 분해 파편 확률 세팅 실패!" )
			<< BUILD_LOG( iItemType )
			<< BUILD_LOG( fRate )
			<< END_LOG;
	}
}
//}}

double CXSLResolveItemManager::GetJackpotTime() const
{
	const int iRandomValue = m_iMaxJackpotTime - m_iMinJackpotTime;
	if( iRandomValue <= 0 )
	{
		START_LOG( cerr, L"분해 대박 확률값이 0이거나 0보다 작다. 0 나누기 연산 경고!" )
			<< BUILD_LOG( m_iMinJackpotTime )
			<< BUILD_LOG( m_iMaxJackpotTime )
			<< BUILD_LOG( iRandomValue )
			<< END_LOG;

		// 일단 기본값 3시간을 준다
		return 10800.f;
	}

	return static_cast<double>( ( rand() % iRandomValue ) + m_iMinJackpotTime );
}

//{{ 2011. 01. 19	최육사	아이템 분해 개편
//////////////////////////////////////////////////////////////////////////

bool CXSLResolveItemManager::GetResultItem( IN const int iItemGrade,
										   IN const int iEqipLevel,
										   IN const int iItemType,
										   IN const int iSellPrice,
										   IN OUT bool& bJackpot,
										   OUT std::map< int, KItemInfo >& mapResultItem ) const
{
	SET_ERROR( NET_OK );

	mapResultItem.clear();

	// 대박 확률 돌리기
	if( bJackpot )
	{
		const int iResult = m_kJackpotLottery.Decision();
		if( iResult != JP_JACKPOT )
		{
			bJackpot = false;
		}
	}

	////////////////////////////////////////////////////////////////////////// 분해 작업

	// 1. 아이템 등급에 맞는 강화석 획득
	{
		std::map< int, std::vector< SResolveData > >::const_iterator mitRD;
		mitRD = m_mapResolveData.find( iItemGrade );
		if( mitRD != m_mapResolveData.end() )
		{
			BOOST_TEST_FOREACH( const SResolveData&, kResolveData, mitRD->second )
			{
				if( iEqipLevel < kResolveData.m_iBeginLv  ||  iEqipLevel > kResolveData.m_iEndLv )
					continue;

				ResolveResult_EnchantStone( kResolveData, iEqipLevel, bJackpot, mapResultItem );
			}
		}
	}

	// 2. 엘의 가루 획득
	{
		ResolveResult_BrokenPiece( RRT_EL_FLOUR, iEqipLevel, iSellPrice, mapResultItem );
	}

	// 3. 무기 조각 또는 방어구 파편 획득
	switch( iItemType )
	{
	case CXSLItem::IT_WEAPON:
		{
			const int iResolveItemType = m_kWeaponResolveLottery.Decision();
			if( iResolveItemType == CXSLItem::IT_WEAPON )
			{
				ResolveResult_BrokenPiece( RRT_WEAPON_BROKEN_PIECE, iEqipLevel, iSellPrice, mapResultItem );
			}
			else if( iResolveItemType == CXSLItem::IT_DEFENCE )
			{
				ResolveResult_BrokenPiece( RRT_ARMOR_BROKEN_PIECE, iEqipLevel, iSellPrice, mapResultItem );
			}
			else
			{
                START_LOG( cerr, L"기획상 꽝이 없을텐데 꽝이 나왔네?" )
					<< BUILD_LOG( iItemType )
					<< BUILD_LOG( iResolveItemType )
					<< END_LOG;
			}
		}
		break;

	case CXSLItem::IT_DEFENCE:
		{
			int iResolveItemType = CXSLItem::IT_NONE;
			int iWeaponMinUnitCost = 0;

			BOOST_TEST_FOREACH( const BrokenPieceResolve&, kResolveData, m_vecBrokenPieceResolve[RRT_WEAPON_BROKEN_PIECE] )
			{
				if( iEqipLevel < kResolveData.m_iBeginLv  ||  iEqipLevel > kResolveData.m_iEndLv )
					continue;

				iWeaponMinUnitCost = kResolveData.m_iUnitCost;
				break;
			}

			if( iWeaponMinUnitCost <= iSellPrice )
			{
				// 방어구의 되팔기 가격이 무기 조각 1Unit의 가치보다 높다면 확률 돌려서 무기 조각 나올지 방어구 파편 나올지 결정!
				iResolveItemType = m_kArmorResolveLottery.Decision();
			}
			else
			{
				// 방어구의 되팔기 가격이 무기 조각 1Unit의 가치보다 낮다면 100% 방어구 파편으로 환원
				iResolveItemType = CXSLItem::IT_DEFENCE;
			}

			if( iResolveItemType == CXSLItem::IT_WEAPON )
			{
				ResolveResult_BrokenPiece( RRT_WEAPON_BROKEN_PIECE, iEqipLevel, iSellPrice, mapResultItem );
			}
			else if( iResolveItemType == CXSLItem::IT_DEFENCE )
			{
				ResolveResult_BrokenPiece( RRT_ARMOR_BROKEN_PIECE, iEqipLevel, iSellPrice, mapResultItem );
			}
			else
			{
				START_LOG( cerr, L"기획상 꽝이 없을텐데 꽝이 나왔네?" )
					<< BUILD_LOG( iItemType )
					<< BUILD_LOG( iResolveItemType )
					<< END_LOG;
			}
		}
		break;

	default:
		{
			START_LOG( cerr, L"분해 할 수 없는 아이템 타입 입니다!" )
				<< BUILD_LOG( iItemType )
				<< END_LOG;
			goto end_proc;
		}
		break;
	}

	return true;

end_proc:
	SET_ERROR( ERR_RESOLVE_ITEM_02 );

	START_LOG( cerr, L"분해 아이템 그룹찾기 실패.!" )
		<< BUILD_LOG( iItemGrade )
		<< BUILD_LOG( iEqipLevel )
		<< BUILD_LOG( iItemType )
		<< BUILD_LOG( iSellPrice )
		<< END_LOG;
	return false;
}
//}}

//////////////////////////////////////////////////////////////////////////
//}}

void CXSLResolveItemManager::ResolveResult_EnchantStone( IN const SResolveData& kResolveData,
														 IN const int iEqipLevel,
														 IN const bool bJackpot,
														 IN OUT std::map< int, KItemInfo >& mapResultItem ) const
{
	// 분해 랜덤 횟수 만큼 돌립니다
	for( int iCnt = 0; iCnt < kResolveData.m_iRandomCount; ++iCnt )
	{
		int iResultItemID = 0;
		if( bJackpot )
		{
			// 대박!!
			iResultItemID = kResolveData.m_iResultItemID;
		}
		else
		{
			// 룰렛!
			iResultItemID = kResolveData.m_kLottery.Decision();
		}

		// 분해 돌렸는데 꽝이군요
		if( iResultItemID == KLottery::CASE_BLANK )
			continue;

		std::map< int, KItemInfo >::iterator mitResult;
		mitResult = mapResultItem.find( iResultItemID );
		if( mitResult == mapResultItem.end() )
		{
			//{{ 2008. 3. 17  최육사  DeleteAndInsert()
			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iResultItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
					<< BUILD_LOG( iResultItemID )
					<< END_LOG;
			}
			else
			{
				KItemInfo kInsertItemInfo;
				kInsertItemInfo.m_iItemID	 = iResultItemID;
				kInsertItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
				kInsertItemInfo.m_iQuantity	 = ( bJackpot ? kResolveData.m_iResultCount * 2 : kResolveData.m_iResultCount );
				kInsertItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
				mapResultItem.insert( std::make_pair( iResultItemID, kInsertItemInfo ) );
			}
			//}}
		}
		else
		{
			mitResult->second.m_iQuantity += ( bJackpot ? kResolveData.m_iResultCount : kResolveData.m_iResultCount );
		}
	}
}

//{{ 2011. 01. 19	최육사	아이템 분해 개편
void CXSLResolveItemManager::ResolveResult_BrokenPiece( IN RESOLVE_RESULT_TYPE eType, IN const int iEqipLevel, IN const int iSellPrice, IN OUT std::map< int, KItemInfo >& mapResultItem ) const
{
	if( eType < RRT_EL_FLOUR  ||  eType >= RRT_MAX )
	{
		START_LOG( cerr, L"분해 결과 타입이 이상합니다!" )
			<< BUILD_LOG( eType )
			<< END_LOG;
		return;
	}

	BOOST_TEST_FOREACH( const BrokenPieceResolve&, kResolveData, m_vecBrokenPieceResolve[eType] )
	{
		if( iEqipLevel < kResolveData.m_iBeginLv  ||  iEqipLevel > kResolveData.m_iEndLv )
			continue;

		_JIF( kResolveData.m_iUnitCost > 0, continue; );
		const int iQuantityInteger = (int)( (float)iSellPrice / (float)kResolveData.m_iUnitCost );
		const float fQuantityFloat = ( ( (float)iSellPrice / (float)kResolveData.m_iUnitCost ) - (float)iQuantityInteger ) * 100.f;
		int iResultQuantity = iQuantityInteger;

		KLottery kLot;
		kLot.AddCase( 0, fQuantityFloat );
		if( kLot.Decision() != KLottery::CASE_BLANK )
		{
			// 1개 더 추가!
			++iResultQuantity;
		}

		// 수량이 0이면 넘어가자!
		if( iResultQuantity <= 0 )
			continue;

		std::map< int, KItemInfo >::iterator mitResult;
		mitResult = mapResultItem.find( kResolveData.m_iItemID );
		if( mitResult == mapResultItem.end() )
		{
			//{{ 2008. 3. 17  최육사  DeleteAndInsert()
			const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kResolveData.m_iItemID );
			if( pItemTemplet == NULL )
			{
				START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
					<< BUILD_LOG( kResolveData.m_iItemID )
					<< END_LOG;
			}
			else
			{
				KItemInfo kInsertItemInfo;
				kInsertItemInfo.m_iItemID	 = kResolveData.m_iItemID;
				kInsertItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
				kInsertItemInfo.m_iQuantity	 = iResultQuantity;
				kInsertItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
				mapResultItem.insert( std::make_pair( kResolveData.m_iItemID, kInsertItemInfo ) );
			}
			//}}
		}
		else
		{
			mitResult->second.m_iQuantity += iResultQuantity;
		}
	}
}
//}}

//{{ 2011.03.16   임규수 아바타 분해 시스템
//}}
