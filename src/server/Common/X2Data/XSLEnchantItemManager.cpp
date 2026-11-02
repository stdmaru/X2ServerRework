#include "./XSLEnchantItemManager.h"
#include "NetError.h"
//{{ 2013. 05. 15	최육사	아이템 개편
	#include "XSLItemManager.h"
//}}

ImplementRefreshSingleton( CXSLEnchantItemManager );

CXSLEnchantItemManager::CXSLEnchantItemManager(void)
{
	m_iWeaponEnchantStone = 0;
	m_iRareWeaponEnchantStone = 0;
	m_iArmorEnchantStone = 0;
	m_iRareArmorEnchantStone = 0;
	//{{ 2011. 01. 24	최육사	플루오르 강화 레벨 제한
	m_iEnchantLimitLevel = 0;
	//}}
	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	m_iEventEnchantLimitLevel = 0;
	//}}
}

CXSLEnchantItemManager::~CXSLEnchantItemManager(void)
{
}

ImplToStringW( CXSLEnchantItemManager )
{
	stm_	<< L"----------[ Enchant Item Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapEnchantRate.size() )
			<< TOSTRINGW( m_mapEnchantProb.size() )
			<< TOSTRINGW( m_mapEnchantRareProb.size() )
			<< TOSTRINGW( m_iWeaponEnchantStone )
			<< TOSTRINGW( m_iRareWeaponEnchantStone )
			<< TOSTRINGW( m_iArmorEnchantStone )
			<< TOSTRINGW( m_iRareArmorEnchantStone )
			<< TOSTRINGW( m_mapEnchantStoneInfo.size() )
			<< TOSTRINGW( m_mapSupportMaterialInfo.size() )
			<< TOSTRINGW( m_mapRestoreItemInfo.size() )
			<< TOSTRINGW( m_mapEnchantAttach.size() )
			//{{  2011.12.21     김민성    코보 플루오르 스톤 이벤트
			<< TOSTRINGW( m_multimapEventSupportMaterialInfo.size() )
			//}}
			//{{  2011.12.21     김민성    코보 축복받은 복원의 주문서 이벤트
			<< TOSTRINGW( m_multimapEventRestoreItemInfo.size() )
			//}}
			<< TOSTRINGW( m_multimapEventEnchantStoneInfo.size() )
			//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
			//}}
			//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
			//}}
			//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
			<< TOSTRINGW( m_iEventEnchantLimitLevel )
			<< TOSTRINGW( m_mapEventEnchantProb.size() )
			//}}

		;

	return stm_;
}

ImplementLuaScriptParser( CXSLEnchantItemManager )
{
	lua_tinker::class_add<CXSLEnchantItemManager>( GetLuaState(), "CXSLEnchantItemManager" );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddEnchantStoneInfo",		&CXSLEnchantItemManager::AddEnchantStoneInfo_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddSupportMaterialInfo",		&CXSLEnchantItemManager::AddSupportMaterialInfo_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddRestoreItemInfo",			&CXSLEnchantItemManager::AddRestoreItemInfo_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddAttachItemInfo",			&CXSLEnchantItemManager::AddAttachItemInfo_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetEnchantRate",				&CXSLEnchantItemManager::SetEnchantRate_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetEnchantProbability",		&CXSLEnchantItemManager::SetEnchantProbability_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetEnchantRareProbability",	&CXSLEnchantItemManager::SetEnchantRareProbability_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "dump",						&CXSLEnchantItemManager::Dump );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetWeaponEnchantStone",		&CXSLEnchantItemManager::SetWeaponEnchantStone_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetRareWeaponEnchantStone",	&CXSLEnchantItemManager::SetRareWeaponEnchantStone_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetArmorEnchantStone",		&CXSLEnchantItemManager::SetArmorEnchantStone_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetRareArmorEnchantStone",	&CXSLEnchantItemManager::SetRareArmorEnchantStone_LUA );

	//{{ 2011. 01. 24	최육사	플루오르 강화 레벨 제한
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetSupportMaterialLimit",	&CXSLEnchantItemManager::SetSupportMaterialEnchantLimitLevel_LUA );
	//}}
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddEventEnchantStoneInfo",	&CXSLEnchantItemManager::AddEventEnchantStoneInfo_LUA );
	//{{  2011.12.21     김민성    코보 플루오르 스톤 이벤트
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddEventSupportMaterialInfo",		&CXSLEnchantItemManager::AddEventSupportMaterialInfo_LUA );
	//}}
	//{{  2011.12.21     김민성    코보 축복받은 복원의 주문서 이벤트
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddEventRestoreItemInfo",		&CXSLEnchantItemManager::AddEventRestoreItemInfo_LUA );
	//}}
	//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
	//}}
	//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
	//}}

	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetEventSupportMaterialLimit",		&CXSLEnchantItemManager::SetEventSupportMaterialEnchantLimitLevel_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "SetEnchantProbability_Event",		&CXSLEnchantItemManager::SetEnchantProbability_Event_LUA );
	//}}
	//{{ 2013. 05. 15	최육사	아이템 개편
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddDropRandomEnchantInfo",			&CXSLEnchantItemManager::AddDropRandomEnchantInfo_LUA );
	lua_tinker::class_def<CXSLEnchantItemManager>( GetLuaState(), "AddCubeRandomEnchantInfo",			&CXSLEnchantItemManager::AddCubeRandomEnchantInfo_LUA );
	//}}

	lua_tinker::decl( GetLuaState(), "EnchantItemManager", this );
}

bool CXSLEnchantItemManager::GetEnchantRate( IN int iEnchantLevel, OUT float& fPercent )
{
	std::map< int, float >::const_iterator mit;
	mit = m_mapEnchantRate.find( iEnchantLevel );
	if( mit == m_mapEnchantRate.end() )
	{
		START_LOG( cerr, L"Failed to find item stat enhancement rate information." )
			<< BUILD_LOG( iEnchantLevel )
			<< END_LOG;

		return false;
	}

	fPercent = mit->second;
	return true;
}

//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
bool CXSLEnchantItemManager::GetEnchantResult( IN bool bIsRare, IN int iEnchantLevel, OUT int& iResult )
//}}
{
	std::map< int, KLottery >::const_iterator mit;
	if( bIsRare )
	{
		mit = m_mapEnchantRareProb.find( iEnchantLevel );
		if( mit == m_mapEnchantRareProb.end() )
		{
			START_LOG( cerr, L"Failed to find rare enhancement level information." )
				<< BUILD_LOG( iEnchantLevel )
				<< END_LOG;

			return false;
		}
	}
	//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
	//}}
	else
	{
		mit = m_mapEnchantProb.find( iEnchantLevel );
		if( mit == m_mapEnchantProb.end() )
		{
			START_LOG( cerr, L"Failed to find enhancement level information." )
				<< BUILD_LOG( iEnchantLevel )
				<< END_LOG;

			return false;
		}
	}

	iResult = mit->second.Decision();
    if( iResult < NetError::ERR_ENCHANT_RESULT_00 || iResult > NetError::ERR_ENCHANT_RESULT_04 )
    {
        START_LOG( cerr, L"Invalid enhancement result." )
            << BUILD_LOG( iResult )
            << BUILD_LOG( NetError::GetErrStr( iResult ) )
            << END_LOG;

        // 변화 없음으로 처리.
        iResult = NetError::ERR_ENCHANT_RESULT_01;
    }

	return true;
}

//{{ 2008. 11. 16  최육사	강화 개편
bool CXSLEnchantItemManager::AddEnchantStoneInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0 && iItemType > 0 , goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
        KEStoneInfoKey kKey;
		kKey.first  = iIdx;
		kKey.second = iItemType;
		m_mapEnchantStoneInfo.insert( std::make_pair( kKey, iItemID ) );
	}
    return true;

err_proc:
	START_LOG( cerr, L"Failed to load enhancement stone information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}
//}}

//{{ 2008. 12. 14  최육사	강화 보조제
bool CXSLEnchantItemManager::AddSupportMaterialInfo_LUA( int iItemID, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0, goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
		m_mapSupportMaterialInfo.insert( std::make_pair( iIdx, iItemID ) );
	}
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load enhancement aid information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}
//}}

//{{ 2008. 12. 21  최육사	복구석
bool CXSLEnchantItemManager::AddRestoreItemInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0, goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );
	_JIF( iItemType >= CXSLItem::IT_NONE  &&  iItemType <= CXSLItem::IT_ETC, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
		KRestoreInfoKey kKey;
		kKey.first = iItemType;
		kKey.second = iIdx;
		m_mapRestoreItemInfo.insert( std::make_pair( kKey, iItemID ) );
	}
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load unavailable recovery stone information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iItemType )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}
//}}

//{{ 2008. 12. 25  최육사	강화부여
bool CXSLEnchantItemManager::AddAttachItemInfo_LUA( int iItemID )
{
	KLuaManager luaMgr( GetLuaState() );

	KAttachInfo kInfo;

	_JIF( iItemID > 0, goto err_proc );

	LUA_GET_VALUE( luaMgr,	"iItemType",		kInfo.m_iItemType,		0	);
	_JIF( kInfo.m_iItemType >= CXSLItem::IT_NONE  &&  kInfo.m_iItemType <= CXSLItem::IT_ETC, goto err_proc );

	LUA_GET_VALUE( luaMgr,	"iEnchantLevel",	kInfo.m_iEnchantLv,		0	);
	_JIF( 0 < kInfo.m_iEnchantLv && kInfo.m_iEnchantLv <= 20, goto err_proc );

	LUA_GET_VALUE( luaMgr,	"iMinItemLevel",	kInfo.m_iMinItemLevel,	0	);
	_JIF( 0 <= kInfo.m_iMinItemLevel, goto err_proc );

	LUA_GET_VALUE( luaMgr,	"iMaxItemLevel",	kInfo.m_iMaxItemLevel,	0	);
	_JIF( 0 <= kInfo.m_iMaxItemLevel, goto err_proc );

	m_mapEnchantAttach.insert( std::make_pair( iItemID, kInfo ) );
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load enhancement grant item information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( kInfo.m_iItemType )
		<< BUILD_LOG( kInfo.m_iEnchantLv )
		<< BUILD_LOG( kInfo.m_iMinItemLevel )
		<< BUILD_LOG( kInfo.m_iMaxItemLevel )
		<< END_LOG;
	return false;
}
//}}

bool CXSLEnchantItemManager::SetEnchantRate_LUA( int iEnchantLevel, float fPercent )
{
    std::map< int, float >::iterator mit;

	_JIF( 0 <= iEnchantLevel && iEnchantLevel <= MAX_ENCHANT_LEVEL, goto err_proc );
    _JIF( fPercent >= 0.0f, goto err_proc );

    mit = m_mapEnchantRate.find( iEnchantLevel );
    if( mit != m_mapEnchantRate.end() )
    {
        START_LOG( cerr, L"Duplicate item stat enhancement rate information." )
            << BUILD_LOG( iEnchantLevel )
            << BUILD_LOG( fPercent )
            << END_LOG;
    }

    // 겹치면 덮어쓴다.
	m_mapEnchantRate[iEnchantLevel] = fPercent;
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load item stat enhancement rate information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fPercent )
		<< END_LOG;

	return false;
}

bool CXSLEnchantItemManager::SetEnchantProbability_LUA( int iEnchantLevel )
{
	int iRandomCount = {}; // declared before the gotos below (MSVC let them cross the initialization)
    std::map< int, KLottery >::iterator mit;
    KLottery kLottery;
    //float fProb; 김민성
	float fProb = 0.f;
    KLuaManager luaMgr( GetLuaState() );

	_JIF( 0 < iEnchantLevel && iEnchantLevel <= MAX_ENCHANT_LEVEL, goto err_proc );

    LUA_GET_VALUE( luaMgr, "Up1", fProb, 0.f );
    _JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
    kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_00, fProb );

    LUA_GET_VALUE( luaMgr, "NoChange", fProb, 0.f );
    _JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
    kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_01, fProb );

    LUA_GET_VALUE( luaMgr, "Down1", fProb, 0.f );
    _JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
    kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_02, fProb );

    LUA_GET_VALUE( luaMgr, "DownTo0", fProb, 0.f );
    _JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
    kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_03, fProb );

    LUA_GET_VALUE( luaMgr, "Break", fProb, 0.f );
    _JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
    kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_04, fProb );

    mit = m_mapEnchantProb.find( iEnchantLevel );
    if( mit != m_mapEnchantProb.end() )
    {
        START_LOG( cerr, L"Enhancement probability is already configured for this level." )
            << BUILD_LOG( iEnchantLevel )
            << END_LOG;
    }

	//{{ 2010. 05. 31  최육사	서버 켜고나서 초기에 랜덤 처리
	iRandomCount = ( rand() % 5 ) + 5;

	for( int i = 0; i < iRandomCount; ++i )
	{
		// 의도적으로 랜덤을 n번정도 돌립니다.
		kLottery.Decision();
	}
	//}}

    // 겹치면 덮어쓴다.
    m_mapEnchantProb[iEnchantLevel] = kLottery;

	return true;

err_proc:
	START_LOG( cerr, L"Failed to register enhancement probability information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fProb )
		<< END_LOG;

	return false;
}

bool CXSLEnchantItemManager::SetEnchantRareProbability_LUA( int iEnchantLevel )
{
	int iRandomCount = {}; // declared before the gotos below (MSVC let them cross the initialization)
	std::map< int, KLottery >::iterator mit;
	KLottery kLottery;
	//float fProb; 김민성
	float fProb = 0.f;
	KLuaManager luaMgr( GetLuaState() );

	_JIF( 0 < iEnchantLevel && iEnchantLevel <= MAX_ENCHANT_LEVEL, goto err_proc );

	LUA_GET_VALUE( luaMgr, "Up1", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_00, fProb );

	LUA_GET_VALUE( luaMgr, "NoChange", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_01, fProb );

	LUA_GET_VALUE( luaMgr, "Down1", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_02, fProb );

	LUA_GET_VALUE( luaMgr, "DownTo0", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_03, fProb );

	LUA_GET_VALUE( luaMgr, "Break", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_04, fProb );

	mit = m_mapEnchantRareProb.find( iEnchantLevel );
	if( mit != m_mapEnchantRareProb.end() )
	{
		START_LOG( cerr, L"Enhancement probability is already configured for this rare level." )
			<< BUILD_LOG( iEnchantLevel )
			<< END_LOG;
	}

	//{{ 2010. 05. 31  최육사	서버 켜고나서 초기에 랜덤 처리
	iRandomCount = ( rand() % 5 ) + 5;

	for( int i = 0; i < iRandomCount; ++i )
	{
		// 의도적으로 랜덤을 n번정도 돌립니다.
		kLottery.Decision();
	}
	//}}

	// 겹치면 덮어쓴다.
	m_mapEnchantRareProb[iEnchantLevel] = kLottery;

	return true;

err_proc:
	START_LOG( cerr, L"Failed to register rare enhancement probability information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fProb )
		<< END_LOG;

	return false;
}

//{{ 2011. 01. 24	최육사	플루오르 강화 레벨 제한
void CXSLEnchantItemManager::SetSupportMaterialEnchantLimitLevel_LUA( IN int iEnchantLimitLevel )
{
	m_iEnchantLimitLevel = iEnchantLimitLevel;

	START_LOG( cout, L"Fluorite enhancement level limit: " << m_iEnchantLimitLevel );
}
//}}

//{{ 2008. 11. 16  최육사	강화 개편
int CXSLEnchantItemManager::GetEnchantStoneItemID( int iEquipLv, int iItemType )
{
	KEStoneInfoKey kKey;
	kKey.first  = iEquipLv;
	kKey.second = iItemType;

	std::map< KEStoneInfoKey, int >::const_iterator	mit;
	mit = m_mapEnchantStoneInfo.find( kKey );
	if( mit == m_mapEnchantStoneInfo.end() )
	{
		START_LOG( cerr, L"Enhancement item ID not found. This should never happen." )
			<< BUILD_LOG( iEquipLv )
			<< BUILD_LOG( iItemType )
			<< END_LOG;
		return 0;
	}

	return mit->second;
}
//}}

//{{ 2008. 12. 14  최육사	강화 보조제
int CXSLEnchantItemManager::GetEnchantSupportMaterialItemID( int iEquipLv )
{
	std::map< int, int >::const_iterator mit;
	mit = m_mapSupportMaterialInfo.find( iEquipLv );
	if( mit == m_mapSupportMaterialInfo.end() )
	{
		START_LOG( cerr, L"Enhancement aid item ID not found. This should never happen." )
			<< BUILD_LOG( iEquipLv )
			<< END_LOG;
		return 0;
	}

	return mit->second;
}
//}}

//{{ 2008. 12. 21  최육사	강화깨짐 복구
int	CXSLEnchantItemManager::GetRestoreItemID( int iEquipLv, int iItemType )
{
	KRestoreInfoKey kKey;
	kKey.first = iItemType;
	kKey.second = iEquipLv;

	std::map< KRestoreInfoKey, int >::const_iterator mit;
	mit = m_mapRestoreItemInfo.find( kKey );
	if( mit == m_mapRestoreItemInfo.end() )
	{
		START_LOG( cerr, L"Recovery stone item ID not found. This should never happen." )
			<< BUILD_LOG( iEquipLv )
			<< BUILD_LOG( iItemType )
			<< END_LOG;
		return 0;
	}

	return mit->second;
}
//}}

//{{ 2008. 12. 26  최육사	강화 부여
bool CXSLEnchantItemManager::GetAttachItemEnchantLevel( int iAttachItemID, int& iEnchantLevel, int& iItemType )
{
	std::map< int, KAttachInfo >::const_iterator mit;
	mit = m_mapEnchantAttach.find( iAttachItemID );
	if( mit == m_mapEnchantAttach.end() )
		return false;

	iEnchantLevel = mit->second.m_iEnchantLv;
	iItemType = mit->second.m_iItemType;
	return true;
}
//}}

//{{  2011.12.21     김민성    코보 플루오르 스톤 이벤트
bool CXSLEnchantItemManager::AddEventSupportMaterialInfo_LUA( int iItemID, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0, goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
		m_multimapEventSupportMaterialInfo.insert( std::make_pair( iIdx, iItemID ) );
	}
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load event enhancement aid information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}

void CXSLEnchantItemManager::GetEventEnchantSupportMaterialItemID( int iEquipLv , std::vector< int >& vecItemID )
{
	vecItemID.clear();

	std::multimap< int, int >::iterator mit;
	std::pair< std::multimap< int, int >::iterator , std::multimap< int, int >::iterator > range = m_multimapEventSupportMaterialInfo.equal_range( iEquipLv );
	for( mit = range.first; mit != range.second; ++mit )
	{
		vecItemID.push_back( mit->second );
	}
}

//}}

//{{  2011.12.21     김민성    코보 축복받은 복원의 주문서 이벤트
bool CXSLEnchantItemManager::AddEventRestoreItemInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0, goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );
	_JIF( iItemType >= CXSLItem::IT_NONE  &&  iItemType <= CXSLItem::IT_ETC, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
		KRestoreInfoKey kKey;
		kKey.first = iItemType;
		kKey.second = iIdx;

		m_multimapEventRestoreItemInfo.insert( std::make_pair( kKey, iItemID ) );

	}
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load unavailable recovery stone information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iItemType )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}

void CXSLEnchantItemManager::GetEventRestoreItemID( int iEquipLv, int iItemType , std::vector< int >& vecItemID )
{
	vecItemID.clear();

	KRestoreInfoKey kKey;
	kKey.first = iItemType;
	kKey.second = iEquipLv;

	std::multimap< KRestoreInfoKey, int >::iterator mit;
	std::pair< std::multimap< KRestoreInfoKey, int >::iterator , std::multimap< KRestoreInfoKey, int >::iterator > range = m_multimapEventRestoreItemInfo.equal_range( kKey );
	for( mit = range.first; mit != range.second; ++mit )
	{
		vecItemID.push_back( mit->second );
	}
}

//}}

bool CXSLEnchantItemManager::AddEventEnchantStoneInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax )
{
	_JIF( iItemID > 0 && iItemType > 0 , goto err_proc );
	_JIF( 0 <= iEquipLvMin && iEquipLvMax <= 100, goto err_proc );

	for( int iIdx = iEquipLvMin; iIdx <= iEquipLvMax; ++iIdx )
	{
		KEStoneInfoKey kKey;
		kKey.first  = iIdx;
		kKey.second = iItemType;
		m_multimapEventEnchantStoneInfo.insert( std::make_pair( kKey, iItemID ) );
	}
	return true;

err_proc:
	START_LOG( cerr, L"Failed to load event enhancement stone information." )
		<< BUILD_LOG( iItemID )
		<< BUILD_LOG( iEquipLvMin )
		<< BUILD_LOG( iEquipLvMax )
		<< END_LOG;
	return false;
}

void CXSLEnchantItemManager::GetEventEnchantStoneItemID( int iEquipLv, int iItemType, std::vector< int >& vecItemID )
{
	vecItemID.clear();

	KEStoneInfoKey kKey;
	kKey.first  = iEquipLv;
	kKey.second = iItemType;

	std::multimap< KEStoneInfoKey, int >::iterator	mit;
	std::pair< std::multimap< KEStoneInfoKey, int >::iterator , std::multimap< KEStoneInfoKey, int >::iterator > range = m_multimapEventEnchantStoneInfo.equal_range( kKey );
	for( mit = range.first; mit != range.second; ++mit )
	{
		vecItemID.push_back( mit->second );
	}
}


//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
//}}

//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
//}}

//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
void CXSLEnchantItemManager::SetEventSupportMaterialEnchantLimitLevel_LUA( IN int iEnchantLimitLevel )
{
	m_iEventEnchantLimitLevel = iEnchantLimitLevel;

	START_LOG( cout, L"Event Fluorite enhancement level limit: " << m_iEventEnchantLimitLevel );
}

bool CXSLEnchantItemManager::SetEnchantProbability_Event_LUA( int iEnchantLevel )
{
	int iRandomCount = {}; // declared before the gotos below (MSVC let them cross the initialization)
	std::map< int, KLottery >::iterator mit;
	KLottery kLottery;
	float fProb = 0.f;
	KLuaManager luaMgr( GetLuaState() );

	_JIF( 0 < iEnchantLevel && iEnchantLevel <= MAX_ENCHANT_LEVEL, goto err_proc );

	LUA_GET_VALUE( luaMgr, "Up1", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_00, fProb );

	LUA_GET_VALUE( luaMgr, "NoChange", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_01, fProb );

	LUA_GET_VALUE( luaMgr, "Down1", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_02, fProb );

	LUA_GET_VALUE( luaMgr, "DownTo0", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_03, fProb );

	LUA_GET_VALUE( luaMgr, "Break", fProb, 0.f );
	_JIF( 0.f <= fProb && fProb <= 100.f, goto err_proc );
	kLottery.AddCaseIntegerCast( NetError::ERR_ENCHANT_RESULT_04, fProb );

	mit = m_mapEventEnchantProb.find( iEnchantLevel );
	if( mit != m_mapEventEnchantProb.end() )
	{
		START_LOG( cerr, L"Enhancement probability is already configured for this event level." )
			<< BUILD_LOG( iEnchantLevel )
			<< END_LOG;
	}

	iRandomCount = ( rand() % 5 ) + 5;

	for( int i = 0; i < iRandomCount; ++i )
	{
		// 의도적으로 랜덤을 n번정도 돌립니다.
		kLottery.Decision();
	}

	// 겹치면 덮어쓴다.
	m_mapEventEnchantProb[iEnchantLevel] = kLottery;

	return true;

err_proc:
	START_LOG( cerr, L"Failed to register event enhancement probability information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fProb )
		<< END_LOG;

	return false;
}

bool CXSLEnchantItemManager::GetEventEnchantResult( IN bool bIsRare, IN int iEnchantLevel, OUT int& iResult )
{
	std::map< int, KLottery >::const_iterator mit;
	if( bIsRare )
	{
		mit = m_mapEnchantRareProb.find( iEnchantLevel );
		if( mit == m_mapEnchantRareProb.end() )
		{
			START_LOG( cerr, L"Failed to find rare enhancement level information." )
				<< BUILD_LOG( iEnchantLevel )
				<< END_LOG;

			return false;
		}
	}
	else
	{
		mit = m_mapEventEnchantProb.find( iEnchantLevel );
		if( mit == m_mapEventEnchantProb.end() )
		{
			START_LOG( cerr, L"Failed to find enhancement level information." )
				<< BUILD_LOG( iEnchantLevel )
				<< END_LOG;

			return false;
		}
	}

	iResult = mit->second.Decision();
	if( iResult < NetError::ERR_ENCHANT_RESULT_00 || iResult > NetError::ERR_ENCHANT_RESULT_04 )
	{
		START_LOG( cerr, L"Invalid enhancement result." )
			<< BUILD_LOG( iResult )
			<< BUILD_LOG( NetError::GetErrStr( iResult ) )
			<< END_LOG;

		// 변화 없음으로 처리.
		iResult = NetError::ERR_ENCHANT_RESULT_01;
	}

	return true;
}
//}}

//{{ 2013. 05. 15	최육사	아이템 개편
bool CXSLEnchantItemManager::AddDropRandomEnchantInfo_LUA( int iEnchantLevel, float fRate )
{
	_JIF( iEnchantLevel >= 0 , goto err_proc );
	_JIF( 100.f >= fRate  &&  fRate >= 0.f, goto err_proc );

	// 드롭에 대한 랜덤 강화 정보 추가
	LIF( m_kRandomEnchant[RET_DROP].AddCase( iEnchantLevel, fRate ) );
	return true;

err_proc:
	START_LOG( cerr, L"Invalid random enhancement information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fRate )
		<< END_LOG;
	return false;
}

bool CXSLEnchantItemManager::AddCubeRandomEnchantInfo_LUA( int iEnchantLevel, float fRate )
{
	_JIF( iEnchantLevel >= 0 , goto err_proc );
	_JIF( 100.f >= fRate  &&  fRate >= 0.f, goto err_proc );

	// 큐브에 대한 랜덤 강화 정보 추가
	LIF( m_kRandomEnchant[RET_CUBE].AddCase( iEnchantLevel, fRate ) );
	return true;

err_proc:
	START_LOG( cerr, L"Invalid random enhancement information." )
		<< BUILD_LOG( iEnchantLevel )
		<< BUILD_LOG( fRate )
		<< END_LOG;
	return false;
}

bool CXSLEnchantItemManager::RandomEnchant( IN const int iItemID
										  , IN const RANDOM_ENCHANT_TYPE eType
										  , IN OUT char& cTimeEnchantEventLevel
										  , OUT char& cEnchantLevel
										  )
{
	if( CheckRandomEnchant( iItemID, eType ) == false )
	{
		return false;
	}

	if( 0 < cTimeEnchantEventLevel )
	{
		cEnchantLevel			= cTimeEnchantEventLevel;
		cTimeEnchantEventLevel	= 0;
		return true;
	}

	int iEnchantLevelResult = m_kRandomEnchant[eType].Decision();
	if( iEnchantLevelResult == KLottery::CASE_BLANK )
	{
		iEnchantLevelResult = 0;
	}

	cEnchantLevel = static_cast<char>(iEnchantLevelResult);
	return true;
}

bool CXSLEnchantItemManager::CheckRandomEnchant( IN const int iItemID, IN const RANDOM_ENCHANT_TYPE eType ) const
{
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( IS_NULL( pItemTemplet ) )
	{
		START_LOG( cerr, L"Item ID does not exist." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return false;
	}

	if( eType >= RET_MAX  ||  eType < 0 )
	{
		START_LOG( cerr, L"Invalid random enhancement type." )
			<< BUILD_LOG( eType )
			<< END_LOG;
		return false;
	}

	if( pItemTemplet->m_bFashion == true )
	{
		return false;
	}

	if( ( pItemTemplet->m_ItemType != CXSLItem::IT_WEAPON )		&&
		( pItemTemplet->m_ItemType != CXSLItem::IT_DEFENCE )
		)
	{
		return false;
	}

	switch( pItemTemplet->m_EqipPosition )
	{
	case CXSLUnit::EP_WEAPON_HAND:
	case CXSLUnit::EP_DEFENCE_BODY:
	case CXSLUnit::EP_DEFENCE_LEG:
	case CXSLUnit::EP_DEFENCE_HAND:
	case CXSLUnit::EP_DEFENCE_FOOT:
		break;

	default:
		return false;
	}

	return true;
}
//}}

bool CXSLEnchantItemManager::CheckAttachItemEnableLevel( IN const int iAttachItemID, IN const int iUseLevel ) const
{
	std::map< int, KAttachInfo >::const_iterator mit = m_mapEnchantAttach.find( iAttachItemID );
	if( mit == m_mapEnchantAttach.end() )
		return false;

	if( ( mit->second.m_iMinItemLevel == 0 ) && ( mit->second.m_iMaxItemLevel == 0 ) )
	{
		return true;
	}
	else if( iUseLevel < mit->second.m_iMinItemLevel )
	{
		return false;
	}
	else if( mit->second.m_iMaxItemLevel < iUseLevel )
	{
		return false;
	}
	else
	{
		return true;
	}
}
