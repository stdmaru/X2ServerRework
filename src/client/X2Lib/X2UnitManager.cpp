#include "stdafx.h"
#include "./X2UnitManager.h"

//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

static std::string      s_strDummyUnitClassName;
static const CX2UnitManager::NPCInitData  s_dummyNPCInitData;


void	CX2UnitManager::UnitTypeLuaTemplet::Release()
{
	SAFE_DELETE( m_pUnitTypeTemplet );
	SAFE_DELETE( m_pLuaManager );
}


CX2UnitManager::NPCUnitInfo::NPCUnitInfo()
    : m_templet()
    , m_stat()
    , m_pInitData( &s_dummyNPCInitData )
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	, m_pLuaState( NULL )
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
{
}//NPCUnitInfo()

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
CX2UnitManager::FileNPCInitData::~FileNPCInitData()
{
	SAFE_DELETE( m_pInitData );
//#ifdef  X2OPTIMIZE_ACCELERATE_SHARED_NPC_LUA_BY_LUAJIT
//    if ( m_pLuaManager != NULL )
//        m_pLuaManager->SetJitCompileMode( KLuaManager::JIT_COMPILE_MODE_OFF );
//#endif  X2OPTIMIZE_ACCELERATE_SHARED_NPC_LUA_BY_LUAJIT
	SAFE_DELETE( m_pLuaManager );
}
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING



CX2UnitManager::CX2UnitManager(void)
{

}

CX2UnitManager::~CX2UnitManager(void)
{
	for( UnitTypeLuaTemplet& refTemplet : m_aUnitTypeLuaTemplet )
	{
		refTemplet.Release();
	}

	map<CX2Unit::UNIT_CLASS,const CX2Unit::UnitTemplet*>::iterator iter;
	const CX2Unit::UnitTemplet* pUnitTemplet = NULL;
	for( iter = m_UnitTempletMap.begin(); iter != m_UnitTempletMap.end(); iter++ )
	{
		pUnitTemplet = iter->second;
		SAFE_DELETE( pUnitTemplet );
	}
	m_UnitTempletMap.clear();

    m_mapNPCUnitInfo.clear();
    m_mapFileNPCInitData.clear();


	//const NPCUnitTemplet* pNPCUnitTemplet;
 //   for( MapNPCUnitTemplet::iterator iter2 = m_NPCUnitTemplet.begin(); iter2 != m_NPCUnitTemplet.end(); iter2++ )
	//{
	//	pNPCUnitTemplet = iter2->second;
	//	SAFE_DELETE( pNPCUnitTemplet );
	//}
	//m_NPCUnitTemplet.clear();


	//map<NPC_UNIT_ID, const NPCUnitStat*>::iterator iter3;
	//const NPCUnitStat* pStat;
	//for( iter3 = m_NPCUnitStat.begin(); iter3 != m_NPCUnitStat.end(); iter3++ )
	//{
	//	pStat = iter3->second;
	//	SAFE_DELETE( pStat );
	//}
	//m_NPCUnitStat.clear();

	map<NPC_ABILITY_ID, NPCExtraAbility*>::iterator iter4;
	NPCExtraAbility* pNPCExtraAbility;
	for( iter4 = m_mapNPCExtraAbility.begin(); iter4 != m_mapNPCExtraAbility.end(); iter4++ )
	{
		pNPCExtraAbility = iter4->second;
		SAFE_DELETE( pNPCExtraAbility );
	}
	m_mapNPCExtraAbility.clear();


}

void CX2UnitManager::OnFrameMove( double /*fTime*/, float /*fElapsedTime*/ )
{
	if( g_pKTDXApp->GetIsNowVeryfy() == true )
	{
		UnitTempletMap::iterator iter;
		const CX2Unit::UnitTemplet* pUnitTemplet = NULL;
		for( iter = m_UnitTempletMap.begin(); iter != m_UnitTempletMap.end(); iter++ )
		{
			pUnitTemplet = iter->second;
			pUnitTemplet->m_Stat.Verify();
		}
	}
}

bool CX2UnitManager::OpenScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pUnitManager", this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR67, pFileName );

		return false;
    }


	m_UnitClassNameMap[ CX2Unit::UC_ELSWORD_SWORDMAN ] = "ELSWORD_SWORDMAN";
	m_UnitClassNameMap[ CX2Unit::UC_ARME_VIOLET_MAGE ] = "ARME_VIOLET_MAGE";
	m_UnitClassNameMap[ CX2Unit::UC_LIRE_ELVEN_RANGER ] = "LIRE_ELVEN_RANGER";
	m_UnitClassNameMap[ CX2Unit::UC_RAVEN_FIGHTER ] = "RAVEN_FIGHTER";
	m_UnitClassNameMap[ CX2Unit::UC_EVE_NASOD ] = "EVE_NASOD";
	m_UnitClassNameMap[ CX2Unit::UC_ELSWORD_KNIGHT ] = "ELSWORD_KNIGHT";
	m_UnitClassNameMap[ CX2Unit::UC_ELSWORD_MAGIC_KNIGHT ] = "ELSWORD_MAGIC_KNIGHT";
	m_UnitClassNameMap[ CX2Unit::UC_LIRE_COMBAT_RANGER ] = "LIRE_COMBAT_RANGER";
	m_UnitClassNameMap[ CX2Unit::UC_LIRE_SNIPING_RANGER ] = "LIRE_SNIPING_RANGER";
	m_UnitClassNameMap[ CX2Unit::UC_ARME_HIGH_MAGICIAN ] = "ARME_HIGH_MAGICIAN";
	m_UnitClassNameMap[ CX2Unit::UC_ARME_DARK_MAGICIAN ] = "ARME_DARK_MAGICIAN";
	m_UnitClassNameMap[ CX2Unit::UC_RAVEN_SOUL_TAKER ] = "RAVEN_SOUL_TAKER";
	m_UnitClassNameMap[ CX2Unit::UC_RAVEN_OVER_TAKER ] = "RAVEN_OVER_TAKER";
	m_UnitClassNameMap[ CX2Unit::UC_EVE_EXOTIC_GEAR ] = "EVE_EXOTIC_GEAR";
	m_UnitClassNameMap[ CX2Unit::UC_EVE_ARCHITECTURE ] = "EVE_ARCHITECTURE";
	m_UnitClassNameMap[ CX2Unit::UC_ELSWORD_LORD_KNIGHT ] = "ELSWORD_LORD_KNIGHT";
	m_UnitClassNameMap[ CX2Unit::UC_ELSWORD_RUNE_SLAYER ] = "ELSWORD_RUNE_SLAYER";
	m_UnitClassNameMap[ CX2Unit::UC_LIRE_WIND_SNEAKER ] = "LIRE_WIND_SNEAKER";
	m_UnitClassNameMap[ CX2Unit::UC_LIRE_GRAND_ARCHER ] = "LIRE_GRAND_ARCHER";
	m_UnitClassNameMap[ CX2Unit::UC_ARME_ELEMENTAL_MASTER ] = "ARME_ELEMENTAL_MASTER";
	m_UnitClassNameMap[ CX2Unit::UC_ARME_VOID_PRINCESS ] = "ARME_VOID_PRINCESS";
	m_UnitClassNameMap[ CX2Unit::UC_RAVEN_BLADE_MASTER ] = "RAVEN_BLADE_MASTER";
	m_UnitClassNameMap[ CX2Unit::UC_RAVEN_RECKLESS_FIST ] = "RAVEN_RECKLESS_FIST";
	m_UnitClassNameMap[ CX2Unit::UC_EVE_CODE_NEMESIS ] = "EVE_CODE_NEMESIS";
	m_UnitClassNameMap[ CX2Unit::UC_EVE_CODE_EMPRESS ] = "EVE_CODE_EMPRESS";

    //ASSERT( m_UnitTempletMap.size() == m_UnitClassNameMap.size() );

    CreateUnitLuaSpace();

	return true;
}
bool CX2UnitManager::OpenDataScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pUnitManager", this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR69, pFileName );

		return false;
    }

	return true;
}
//{{ robobeg : 2011-01-24
void    CX2UnitManager::CreateUnitLuaSpace()
{
    ASSERT( g_pKTDXApp != NULL );
    if ( g_pKTDXApp == NULL )
        return;

    {
        static const WCHAR* apszScript[] =
        {
            NULL
            , L"ELSWORD_SWORDMAN.LUA"
            , L"ARME_VIOLETMAGE.LUA"
            , L"LIRE_ELVENRANGER.LUA"
            , L"RAVEN_FIGHTER.LUA"
            , L"EVE_NASOD.LUA"
			, L"CHUNG_IRON_CANNON.LUA"
			, L"ARA_MARTIAL_ARTIST.LUA"
			, L"ELESIS_KNIGHT.LUA"
			, L"ADD_NASODRULER.LUA"
        };//apszScript[]

		ASSERT( ARRAY_SIZE( apszScript ) == CX2Unit::UT_END );

		for( int iUnitType = 0; iUnitType < CX2Unit::UT_END; iUnitType++ )
        {
            if ( apszScript[ iUnitType ] == NULL )
            {
				m_aUnitTypeLuaTemplet[ iUnitType ].m_pUnitTypeTemplet = NULL;
				m_aUnitTypeLuaTemplet[ iUnitType ].m_pLuaManager = NULL;
            }
            else
            {
                UnitTypeTemplet* pUnitTypeTemplet = new UnitTypeTemplet;
                ASSERT( pUnitTypeTemplet != NULL );
				m_aUnitTypeLuaTemplet[ iUnitType ].m_pUnitTypeTemplet = pUnitTypeTemplet;
                pUnitTypeTemplet->m_wstrLuaScriptFile = apszScript[ iUnitType ];

				KLuaManager*	pLuaManager = new KLuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
				ASSERT( pLuaManager != NULL );
				m_aUnitTypeLuaTemplet[ iUnitType ].m_pLuaManager = pLuaManager;
				KLuaManager& luaManager = *pLuaManager;
				//g_pKTDXApp->LoadLuaTinker( pUnitTypeTemplet->m_wstrLuaScriptFile.c_str() );
				if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, pUnitTypeTemplet->m_wstrLuaScriptFile.c_str() ) == false )
				{
					ASSERT( !"gameunit lua script Parsing failed" );
				}

                CX2GUUser::InitInit( pUnitTypeTemplet->m_init, luaManager );
            }//if.. else..
        }//for
    }

}//CX2UnitManager::CreateUnitLuaSpace()
bool    CX2UnitManager::OpenNPCFiles( const WCHAR* pTempletFile, const WCHAR* pStatFile, const WCHAR* pExtraAbilityFile )
{
    bool    bRet = _OpenNPCScriptFile( pTempletFile );
    if ( bRet == false )
        return false;

    bRet = _OpenNPCStatScriptFile( pStatFile );
    if ( bRet == false )
        return false;

    bRet = _OpenNPCEAScripFile( pExtraAbilityFile );
    if ( bRet == false )
        return false;


    return  true;
}//CX2UnitManager::OpenNPCFiles()



bool CX2UnitManager::_OpenNPCScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pUnitManager", this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR69, pFileName );

		return false;
    }

	return true;
}

bool CX2UnitManager::_OpenNPCStatScriptFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pUnitManager", this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR69, pFileName );
		return false;
    }


#if X2_DEV_TOOLS

// 마을 NPC, 사용하지 않는 NPC등 Stat설정 안 된 NPC가 많아서 정상적인 로그로써 사용이 불가능하다 판단되어 임시 제거.
	bool bCheckEmpty = false;
	wstringstream wstrstm;
	//for ( int i = 1; i < (int)NUI_NPC_END; i++ )
    for( NPCUnitInfoMap::const_iterator iter = m_mapNPCUnitInfo.begin(); iter != m_mapNPCUnitInfo.end(); iter++ )
	{
		if ( m_setNPCStatCheck.find( iter->first ) == m_setNPCStatCheck.end() )
		{
			//ASSERT( !"npc stat error" );
			bCheckEmpty = true;
			wstrstm << L"Name: " << iter->second.m_templet.m_Name.c_str() << L"id: " << iter->first << L" ";
		}
	}

	if ( bCheckEmpty == true )
	{
		ErrorLogMsg( XEM_ERROR126, wstrstm.str().c_str() );
		//MessageBox( g_pKTDXApp->GetHWND(), wstrstm.str().c_str(), L"NPCStat Error", MB_OK );
	}

#endif // _SERVICE_

	return true;
}

bool CX2UnitManager::_OpenNPCEAScripFile( const WCHAR* pFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pCX2NpcExtraAbility", this );


    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR69, pFileName );

		return false;
    }

	return true;
}



const CX2UnitManager::UnitTypeLuaTemplet&     CX2UnitManager::GetUnitTypeLuaTemplet( CX2Unit::UNIT_TYPE unitType )
{
	static UnitTypeLuaTemplet	s_default;

    if ( unitType >= CX2Unit::UNIT_TYPE( 0 ) && unitType < CX2Unit::UT_END )
    {
		return m_aUnitTypeLuaTemplet[ unitType ];
    }//if

    return s_default;
}//CX2UnitManager::GetUnitTypeTemplet()


const CX2Unit::UnitTemplet* CX2UnitManager::GetUnitTemplet( CX2Unit::UNIT_CLASS unitClass )
{
	UnitTempletMap::iterator iter;
	iter = m_UnitTempletMap.find( unitClass );
	if( iter == m_UnitTempletMap.end() )
		return NULL;

	const CX2Unit::UnitTemplet* pUnitTemplet = iter->second;

	return pUnitTemplet;
}

const CX2UnitManager::NPCUnitInfo*      CX2UnitManager::GetNPCUnitInfo( NPC_UNIT_ID nNPCUnitID )
{
	NPCUnitInfoMap::iterator iter = m_mapNPCUnitInfo.find( nNPCUnitID );
	if ( iter == m_mapNPCUnitInfo.end() )
		return NULL;

	NPCUnitInfo& info = iter->second;

	if ( info.m_templet.m_LuaFileName.empty() == false
		&& info.m_pInitData == &s_dummyNPCInitData )
	{
		WCHAR    wszUpper[MAX_PATH];
		MakeUpperCase( wszUpper, ARRAY_SIZE( wszUpper ), info.m_templet.m_LuaFileName.c_str() );
		FileNPCInitDataMap::iterator  fiter = m_mapFileNPCInitData.find( wszUpper );
		if ( fiter != m_mapFileNPCInitData.end() )
		{
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
			info.m_pInitData = fiter->second.m_pInitData;
			info.m_pLuaState = ( fiter->second.m_pLuaManager != NULL ) ? fiter->second.m_pLuaManager->GetLuaState() : NULL;
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//			info.m_pInitData = &fiter->second;
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
		}
		else
		{
			auto ret = m_mapFileNPCInitData.insert(
				FileNPCInitDataMap::value_type( wszUpper, FileNPCInitDataMap::value_type::second_type() ) );
			ASSERT( ret.second == true );
			ASSERT( ret.first != m_mapFileNPCInitData.end() );

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING

			FileNPCInitData& kFileNPCInitData = ret.first->second;
			kFileNPCInitData.m_pInitData = new NPCInitData();
			ASSERT( kFileNPCInitData.m_pInitData != NULL );
			info.m_pInitData = kFileNPCInitData.m_pInitData;
			info.m_pLuaState = NULL;
			KLuaManager* pLuaManager = new KLuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
			ASSERT( pLuaManager );
			if ( g_pKTDXApp->LoadAndDoMemory( pLuaManager, info.m_templet.m_LuaFileName.c_str() ) == true )
			{
				CX2GUNPC::InitInit( kFileNPCInitData.m_pInitData->m_init, *pLuaManager );
				if ( kFileNPCInitData.m_pInitData->m_init.m_bLuaShareable == true )
				{
					kFileNPCInitData.m_pLuaManager = pLuaManager;
					info.m_pLuaState = pLuaManager->GetLuaState();
                    pLuaManager->SetJitCompileMode( KLuaManager::JIT_COMPILE_MODE_ON );
					pLuaManager = NULL;
				}
			}//if
			SAFE_DELETE( pLuaManager );

//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//
//			info.m_pInitData = &ret.first->second;
//
//			KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//			//g_pKTDXApp->LoadLuaTinker( pUnitTypeTemplet->m_wstrLuaScriptFile.c_str() );
//			if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, info.m_templet.m_LuaFileName.c_str() ) == true )
//				CX2GUNPC::InitInit( ret.first->second.m_init, luaManager );
//
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING

		}
	}//if

	return  &info;
}//CX2UnitManager::GetNPCUnitInfo()

void	CX2UnitManager::UnloadAllNPCInitData()
{
	for( NPCUnitInfoMap::value_type& value : m_mapNPCUnitInfo )
	{
		if ( value.second.m_pInitData != &s_dummyNPCInitData )
		{
			value.second.m_pInitData = &s_dummyNPCInitData;
		}//if
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
		if ( value.second.m_pLuaState != NULL )
		{
			value.second.m_pLuaState = NULL;
		}
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
	}//BOOST_TEST_FOREACH()

// 	for( const FileNPCInitDataMap::value_type& value : m_mapFileNPCInitData )
// 	{
// 		g_pKTDXApp->GetLuaFunctionManager()->UnregisterLuaFunction( value.first.c_str() );
// 	}//BOOST_TEST_FOREACH()

	m_mapFileNPCInitData.clear();
}//CX2UnitManager::UnloadAllNPCInitData()



const CX2UnitManager::NPCUnitTemplet*	CX2UnitManager::GetNPCUnitTemplet( NPC_UNIT_ID nNPCUnitID )
{
    NPCUnitInfoMap::const_iterator iter = m_mapNPCUnitInfo.find( nNPCUnitID );
    if ( iter == m_mapNPCUnitInfo.end() )
        return NULL;
    return  &iter->second.m_templet;
}


const CX2UnitManager::NPCUnitStat* CX2UnitManager::GetNPCUnitStat( NPC_UNIT_ID nNPCUnitID )
{
    NPCUnitInfoMap::const_iterator iter = m_mapNPCUnitInfo.find( nNPCUnitID );
    if ( iter == m_mapNPCUnitInfo.end() )
        return NULL;
    return  &iter->second.m_stat;
}


const CX2UnitManager::NPCExtraAbility* CX2UnitManager::GetNPCExtraAbility( NPC_ABILITY_ID npcAtilityID )
{
	NPCExtraAbilityMap::iterator mit = m_mapNPCExtraAbility.find( npcAtilityID );
	if( mit == m_mapNPCExtraAbility.end() )
		return NULL;

	CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = mit->second;
	return pNPCExtraAbility;
}

bool CX2UnitManager::AddUnitTemplet_LUA()
{
	CX2Unit::UnitTemplet* pUnitTemplet = new CX2Unit::UnitTemplet();
    ASSERT( pUnitTemplet != NULL );
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	//TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );

	LUA_GET_VALUE_RETURN_ENUM( luaManager, "unitType",	pUnitTemplet->m_UnitType,		CX2Unit::UNIT_TYPE,		CX2Unit::UT_NONE,	SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN_ENUM( luaManager, "unitClass",	pUnitTemplet->m_UnitClass,		CX2Unit::UNIT_CLASS,	CX2Unit::UC_NONE,	SAFE_DELETE(pUnitTemplet); return false; );

	// 09.08.11 태완 : 스트링 추출
	int stringid = -1;
	LUA_GET_VALUE_RETURN(	luaManager, "name",			stringid,			STR_ID_EMPTY, SAFE_DELETE(pUnitTemplet); return false; );
	pUnitTemplet->m_Name = GET_STRING( stringid );
	LUA_GET_VALUE_RETURN(	luaManager, "description",			stringid,			STR_ID_EMPTY, SAFE_DELETE(pUnitTemplet); return false; );
	pUnitTemplet->m_Description = GET_STRING( stringid );


	LUA_GET_VALUE_RETURN( luaManager, "basicWeaponItemID",	pUnitTemplet->m_BasicWeaponItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicHairItemID",	pUnitTemplet->m_BasicHairItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicFaceItemID",	pUnitTemplet->m_BasicFaceItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicBodyItemID",	pUnitTemplet->m_BasicBodyItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicLegItemID",		pUnitTemplet->m_BasicLegItemID,			0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicHandItemID",	pUnitTemplet->m_BasicHandItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "basicFootItemID",	pUnitTemplet->m_BasicFootItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );

	LUA_GET_VALUE( luaManager, "m_RavenLeftArmItemID",	pUnitTemplet->m_RavenLeftArmItemID,		0 );
	LUA_GET_VALUE( luaManager, "m_SecondWeaponItemID",	pUnitTemplet->m_SecondWeaponItemID,		0 );

	LUA_GET_VALUE( luaManager, "m_CommonMotion",	pUnitTemplet->m_CommonMotion,		L"" );
	LUA_GET_VALUE( luaManager, "m_FieldMotion",		pUnitTemplet->m_FieldMotion,		L"" );
	LUA_GET_VALUE( luaManager, "m_LobbyMotion",		pUnitTemplet->m_LobbyMotion,		L"" );
	LUA_GET_VALUE( luaManager, "m_EmotionMotion",	pUnitTemplet->m_EmotionMotion,		L"" );

	LUA_GET_VALUE( luaManager, "m_GameMotion0",		pUnitTemplet->m_GameMotion0,		L"" );
	LUA_GET_VALUE( luaManager, "m_GameMotion1",		pUnitTemplet->m_GameMotion1,		L"" );
	LUA_GET_VALUE( luaManager, "m_GameMotion2",		pUnitTemplet->m_GameMotion2,		L"" );

	LUA_GET_VALUE_RETURN( luaManager, "promotionWeaponItemID",	pUnitTemplet->m_PromotionWeaponItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "promotionBodyItemID",	pUnitTemplet->m_PromotionBodyItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "promotionLegItemID",		pUnitTemplet->m_PromotionLegItemID,			0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "promotionHandItemID",	pUnitTemplet->m_PromotionHandItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );
	LUA_GET_VALUE_RETURN( luaManager, "promotionFootItemID",	pUnitTemplet->m_PromotionFootItemID,		0, SAFE_DELETE(pUnitTemplet); return false; );

	LUA_GET_VALUE( luaManager, "introMovieFIleName",	pUnitTemplet->m_IntroMovieFileName,					L"" );
	if( m_UnitTempletMap.find(pUnitTemplet->m_UnitClass) != m_UnitTempletMap.end() )
	{
		SAFE_DELETE(pUnitTemplet);
		return false;
	}
	else
	{
		m_UnitTempletMap.insert( std::make_pair(pUnitTemplet->m_UnitClass, pUnitTemplet) );
	}

	return true;
}



bool CX2UnitManager::AddNPCTemplet_LUA()
{
	//NPCUnitTemplet* pNPCUnitTemplet = new NPCUnitTemplet();
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	//TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );

    NPC_UNIT_ID		nNPCUnitID = NUI_NONE;
	NPC_CLASS_TYPE	nClassType = NCT_BASIC;
    bool            bIsNPC = false;

	LUA_GET_VALUE_RETURN_ENUM(	luaManager, "NPC_ID",			nNPCUnitID,		NPC_UNIT_ID,	NUI_NONE,	return false; );

	if( m_mapNPCUnitInfo.find( nNPCUnitID ) != m_mapNPCUnitInfo.end() )
        return false;

	LUA_GET_VALUE_RETURN_ENUM(	luaManager, "CLASS_TYPE",		nClassType,		NPC_CLASS_TYPE,	NCT_BASIC,	return false; );
	LUA_GET_VALUE_RETURN(	luaManager, "IS_NPC",				bIsNPC,			false, return false; );

    std::pair<NPCUnitInfoMap::iterator, bool > ret = m_mapNPCUnitInfo.insert( NPCUnitInfoMap::value_type( nNPCUnitID, NPCUnitInfo() ) );
    ASSERT( ret.second == true );
    ASSERT( ret.first != m_mapNPCUnitInfo.end() );

    NPCUnitTemplet& npcUnitTemplet = ret.first->second.m_templet;

    npcUnitTemplet.m_nNPCUnitID = nNPCUnitID;
    npcUnitTemplet.m_ClassType = nClassType;
    npcUnitTemplet.m_bIsNPC = bIsNPC;

	LUA_GET_VALUE( luaManager, "NAME", npcUnitTemplet.m_Name,	L""	);
	LUA_GET_VALUE( luaManager, "DESC", npcUnitTemplet.m_Description,	L""	);

	LUA_GET_VALUE(			luaManager, "LUA_FILE_NAME",		npcUnitTemplet.m_LuaFileName,		L"" );
	LUA_GET_VALUE(			luaManager, "LUA_FILE_NAME_UI",	npcUnitTemplet.m_LuaFileNameUI ,	L"" );


	return true;
}

bool CX2UnitManager::AddNPCStat_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
	//TableBind( &luaManager, g_pKTDXApp->GetLuaBinder() );

    NPC_UNIT_ID		nNPCUnitID = NUI_NONE;
	LUA_GET_VALUE_ENUM(	luaManager, "NPC_ID", nNPCUnitID, CX2UnitManager::NPC_UNIT_ID, CX2UnitManager::NUI_NONE );

#if X2_DEV_TOOLS
// 	if ( m_setNPCStatCheck.find( nNPCUnitID ) != m_setNPCStatCheck.end() )
// 	{
// 		ASSERT( 0 );
// 		return false;
// 	}//if
#endif // _SERVICE_

    NPCUnitInfoMap::iterator iter = m_mapNPCUnitInfo.find( nNPCUnitID );
    //ASSERT( iter != m_mapNPCUnitInfo.end() );
    if ( iter == m_mapNPCUnitInfo.end() )
        return false;

    NPCUnitStat& stat = iter->second.m_stat;
    stat.m_nNPCUnitID = nNPCUnitID;


	LUA_GET_VALUE(		luaManager, "MAX_HP",			stat.m_fMaxHP,					0.0f );
	LUA_GET_VALUE(		luaManager, "ATK_PHYSIC",		stat.m_fAtkPhysic,				0.0f );
	LUA_GET_VALUE(		luaManager, "ATK_MAGIC",		stat.m_fAtkMagic,					0.0f );
	LUA_GET_VALUE(		luaManager, "DEF_PHYSIC",		stat.m_fDefPhysic,				0.0f );
	LUA_GET_VALUE(		luaManager, "DEF_MAGIC",		stat.m_fDefMagic,					0.0f );


	if( true == luaManager.BeginTable( "HARD_LEVEL" ) )
	{
		LUA_GET_VALUE(		luaManager, "HP",				stat.m_HardLevel.m_fHP,			0.0f );
		LUA_GET_VALUE(		luaManager, "ATK_PHYSIC",		stat.m_HardLevel.m_fAtkPhysic,	0.0f );
		LUA_GET_VALUE(		luaManager, "ATK_MAGIC",		stat.m_HardLevel.m_fAtkMagic,		0.0f );
		LUA_GET_VALUE(		luaManager, "DEF_PHYSIC",		stat.m_HardLevel.m_fDefPhysic,	0.0f );
		LUA_GET_VALUE(		luaManager, "DEF_MAGIC",		stat.m_HardLevel.m_fDefMagic,		0.0f );
		LUA_GET_VALUE(		luaManager, "EXP",				stat.m_HardLevel.m_fExp,			0.0f );

		luaManager.EndTable();
	}

	LUA_GET_VALUE_ENUM(	luaManager, "DAMAGE_TYPE",			stat.m_DamageType,	CX2DamageManager::DAMAGE_TYPE,	CX2DamageManager::DT_PHYSIC );

	LUA_GET_VALUE(		luaManager, "DEF_RED",			stat.m_DefenseRed,					0 );
	LUA_GET_VALUE(		luaManager, "DEF_BLUE",		stat.m_DefenseBlue,					0 );
	LUA_GET_VALUE(		luaManager, "DEF_GREEN",		stat.m_DefenseGreen,					0 );
	LUA_GET_VALUE(		luaManager, "DEF_WIND",		stat.m_DefenseWind,					0 );
	LUA_GET_VALUE(		luaManager, "DEF_LIGHT",		stat.m_DefenseLight,					0 );
	LUA_GET_VALUE(		luaManager, "DEF_DARK",		stat.m_DefenseDark,					0 );

	// 클러킹 스캔 확율 및 100%스캔 거리
	LUA_GET_VALUE(		luaManager, "SCAN_RATE",		stat.m_fScanRate,						0.f );
	LUA_GET_VALUE(		luaManager, "SCAN_NEAR_RANGE",	stat.m_fScanNearRange,				0.f );

	// 명중 및 회피
	LUA_GET_VALUE(		luaManager, "ACCURACY",		stat.m_fAccuracy,						0.f );
	LUA_GET_VALUE(		luaManager, "AVOIDANCE",		stat.m_fAvoidance,					0.f );

	//{{ megagame / 박교현 / 2010.04.30 / NPC 스탯 추가
	// 크리티컬율
	LUA_GET_VALUE(		luaManager, "CRITICAL_PERCENT",	stat.m_fPercentCritical,			0.f );
	//}} megagame / 박교현 / 2010.04.30 / NPC 스탯 추가

#if X2_DEV_TOOLS
	m_setNPCStatCheck.insert( nNPCUnitID );
#endif // _SERVICE_

	return true;
}

bool CX2UnitManager::AddNPCExtraAbility_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	NPCExtraAbility* pNpcExtraAbility = new NPCExtraAbility;

	LUA_GET_VALUE_ENUM(	luaManager, "m_AbilityID",						pNpcExtraAbility->m_AbilityID,							CX2UnitManager::NPC_ABILITY_ID,	CX2UnitManager::NAI_NONE );
	int StrID = STR_ID_EMPTY;
	LUA_GET_VALUE(		luaManager, "m_AbilityName",					StrID,						STR_ID_EMPTY );
	pNpcExtraAbility->m_AbilityName = GET_STRING( StrID );

	LUA_GET_VALUE(		luaManager, "m_bFront",						pNpcExtraAbility->m_bFront,								false );

	LUA_GET_VALUE(		luaManager, "m_fPhysicAttackPercentUp",		pNpcExtraAbility->m_fPhysicAttackPercentUp,				0.0f );
	LUA_GET_VALUE(		luaManager, "m_fMagicAttackPercentUp",			pNpcExtraAbility->m_fMagicAttackPercentUp,				0.0f );
	LUA_GET_VALUE(		luaManager, "m_fAllSpeedUpPercentUp",			pNpcExtraAbility->m_fAllSpeedUpPercentUp,				0.0f );
	LUA_GET_VALUE(		luaManager, "m_fCriticalPercent",				pNpcExtraAbility->m_fCriticalPercent,					0.0f );

	LUA_GET_VALUE(		luaManager, "m_fPhysicDefensePercentUp",		pNpcExtraAbility->m_fPhysicDefensePercentUp,			0.0f );
	LUA_GET_VALUE(		luaManager, "m_fMagicDefensePercentUp",		pNpcExtraAbility->m_fMagicDefensePercentUp,				0.0f );
	LUA_GET_VALUE(		luaManager, "m_bAlwaysSuperArmor",				pNpcExtraAbility->m_bAlwaysSuperArmor,					false );
	LUA_GET_VALUE(		luaManager, "m_bHeavy",						pNpcExtraAbility->m_bHeavy,								false );

	m_mapNPCExtraAbility.insert( std::make_pair( pNpcExtraAbility->m_AbilityID, pNpcExtraAbility ) );

	return true;
}

void CX2UnitManager::GetNPCUnitList( std::map< CX2UnitManager::NPC_UNIT_ID, std::wstring >& mapNpcList )
{
	mapNpcList.clear();
    for( const NPCUnitInfoMap::value_type& value : m_mapNPCUnitInfo )
	{
		mapNpcList.insert( std::make_pair( value.first, value.second.m_templet.m_Description ) );
	}
}


const std::string&      CX2UnitManager::GetUnitClassName( CX2Unit::UNIT_CLASS unitClass ) const
{
    UnitClassNameMap::const_iterator iter = m_UnitClassNameMap.find( unitClass );
    if ( iter != m_UnitClassNameMap.end() )
        return iter->second;
    return s_strDummyUnitClassName;
}//CX2UnitManager::GetUnitClassName()




//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
