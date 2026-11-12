#include "stdafx.h"
#include "./X2SkillTree.h"

//{{ �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���
//}} �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���

/*static*/ const WCHAR* CX2SkillTree::GetAccelCoreParticleName( const SKILL_ID eAccelSkillID_ )
{
	switch ( eAccelSkillID_ )
	{
	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
		return L"BuffAttackCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
		return L"BuffMagickCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
		return L"ShieldBuffCore";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_SPEED_ACCEL:
		return L"BuffSpeed_Core";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_CRITICAL_ACCEL:
		return L"BuffCritical_Core";
		break;
	case CX2SkillTree::SI_SA_COMMON_AURA_ADDATK_ACCEL:
		return L"BuffAddAtk_Core";
		break;

	case CX2SkillTree::SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL:
		return L"BuffElDefence_Core";
		break;

	default:
		return NULL;
		break;
	}
}




CX2SkillTree::CX2SkillTree(void)
//{{ kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
: m_iUsedSPoint( 0 )			// ��ų����ü���� ���ΰ�? (SkillSlotChangeReq�� ������ �� Ack�� ���� ���� ���� �����ΰ�?)
, m_iUsedCSPoint( 0 )
, m_bSkillSlotChanging( false )
//}} kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
{
	m_mapGetSkillInfo.clear();
	m_vecNowLearnSkill.clear();
}

CX2SkillTree::~CX2SkillTree(void)
{
	for( SkillTempletMap::value_type& value : m_mapSkillTemplet )
	{
		SAFE_DELETE( value.second );
	}
	m_mapSkillTemplet.clear();
}


bool CX2SkillTree::OpenScriptFile( const WCHAR* pFileName, const WCHAR* pSkillDataFileName )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pSkillTree", this );

	/// ������ ���� ������ ���� ��� ����, SkillData.lua�� ���� �Ľ��Ѵ�.
	if( g_pKTDXApp->LoadLuaTinker( pSkillDataFileName ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, pSkillDataFileName );
		return false;
	}

	if( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
	{
		ErrorLogMsg( XEM_ERROR96, pFileName );
		return false;
	}



	OpenScriptFilePostProcess();

	return true;
}


void CX2SkillTree::OpenScriptFilePostProcess()
{

	for( UnitSkillTreeTempletMap::value_type& value : m_mapUnitSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		for( SkillTreeTempletMap::value_type& value2 : mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
			skillTreeTemplet.m_iFollowingSkill = 0;
		}
	}



	for( UnitSkillTreeTempletMap::value_type& value : m_mapUnitSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		for( SkillTreeTempletMap::value_type& value2 : mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
			const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetSkillTreeTemplet( value.first, static_cast<SKILL_ID>( skillTreeTemplet.m_iPrecedingSkill ) );
			if( NULL == pPrecedingSkillTreeTemplet )
				continue;

			SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
			pPrecedingSkillTemplet_NotConst->m_iFollowingSkill= value2.first;
		}
	}

	//{{ oasis907 : ����� [2009.12.4] //

	for( GuildSkillTreeTempletMap::value_type& value : m_mapGuildSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		for( SkillTreeTempletMap::value_type& value2 : mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
			skillTreeTemplet.m_iFollowingSkill = 0;
		}
	}



	for( GuildSkillTreeTempletMap::value_type& value : m_mapGuildSkillTreeTemplet )
	{
		SkillTreeTempletMap& mapSkillTreeTemplet = value.second;
		for( SkillTreeTempletMap::value_type& value2 : mapSkillTreeTemplet )
		{
			SkillTreeTemplet& skillTreeTemplet = value2.second;
				const SkillTreeTemplet* pPrecedingSkillTreeTemplet = GetGuildSkillTreeTemplet( value.first, static_cast<SKILL_ID>( skillTreeTemplet.m_iPrecedingSkill ) );
				if( NULL == pPrecedingSkillTreeTemplet )
					continue;

				SkillTreeTemplet* pPrecedingSkillTemplet_NotConst = const_cast<SkillTreeTemplet*>( pPrecedingSkillTreeTemplet );
				pPrecedingSkillTemplet_NotConst->m_iFollowingSkill = value2.first;
		}
	}
	//}} oasis907 : ����� [2009.12.4] //
}




bool CX2SkillTree::AddSkillTreeTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


	CX2Unit::UNIT_CLASS eUnitClass = CX2Unit::UC_NONE;
	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "m_eUnitClass",				eUnitClass,				CX2Unit::UNIT_CLASS,		CX2Unit::UC_NONE		);
	LUA_GET_VALUE_ENUM( luaManager, "m_iSkillID",				eSkillID,				SKILL_ID,					SI_NONE					);

	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}

	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"m_iMasterLevel",			skillTreeTemplet.m_iMaxSkillLevel,	0				);
	LUA_GET_VALUE( luaManager,		"m_iRowIndex",				skillTreeTemplet.m_iTier,			0				);
	LUA_GET_VALUE( luaManager,		"m_iColumnIndex",			skillTreeTemplet.m_iIndexInTier,	0				);

	LUA_GET_VALUE( luaManager,		"m_iPrecedingSkill",		skillTreeTemplet.m_iPrecedingSkill,	0				);


	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapUnitSkillTreeTemplet[ (int)eUnitClass ];
	mapSkillTreeTemplet[ eSkillID ] = skillTreeTemplet;


	return true;

}


bool CX2SkillTree::AddSkillTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


	bool bInHouseTestOnly = false;
	LUA_GET_VALUE( luaManager,		"IN_HOUSE_TEST_ONLY",				bInHouseTestOnly,	false );



#if !X2_DEV_TOOLS
	if( true == bInHouseTestOnly )
		return false;
#endif // defined( _SERVICE_ ) || defined( _OPEN_TEST_ ) || defined( _FOREIGN_ ) || defined( _IN_HOUSE_SERVICE_READY_QA_ )


	SkillTemplet* pSkillTemplet = new SkillTemplet();

	LUA_GET_VALUE_ENUM( luaManager,		"m_eType",						pSkillTemplet->m_eType,							SKILL_TYPE,			ST_NONE					);

	/// ��� ��ų ����
	switch ( pSkillTemplet->m_eType )
	{
	case ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
	case ST_GUILD_PASSIVE:
		{
			pSkillTemplet->m_bGuildSkill = true;
		} break;
	default:
		break;
	}

	LUA_GET_VALUE_ENUM( luaManager,		"m_eID",						pSkillTemplet->m_eID,							SKILL_ID,			SI_NONE					);

	if( true == luaManager.BeginTable( "m_SkillCoolTime" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecSkillCoolTime		);
	if( true == luaManager.BeginTable( "m_MPConsumption" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecMPConsumption		);
	if( true == luaManager.BeginTable( "m_RequireCharacterLevel" ) )	SetSkillVlaue( luaManager,						pSkillTemplet->m_vecRequireCharacterLevel	);

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
	LUA_GET_VALUE( luaManager,			"m_usCBConsumption",			pSkillTemplet->m_usCBConsumption,				0		);
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

	LUA_GET_VALUE( luaManager,			"m_usFPGainCount",				pSkillTemplet->m_usFPGainCount,					0		);
	LUA_GET_VALUE( luaManager,			"m_usFPConsumtion",				pSkillTemplet->m_usFPConsumtion,				0		);

	LUA_GET_VALUE( luaManager,			"m_wstrName",					pSkillTemplet->m_wstrName,						L""		);

	LUA_GET_VALUE( luaManager,			"m_wstrMainDesc",				pSkillTemplet->m_wstrMainDesc,					L""		);

	/// ��ų ȿ�� ������ 30�������� ���� �Ǿ� ���� �ʴٸ�, �ڵ����� ä���� ����.
	if( true == luaManager.BeginTable( "m_EffectiveDesc" ) )			SetSkillVlaue( luaManager,						pSkillTemplet->m_vecEffectiveDesc, false);

	LUA_GET_VALUE( luaManager,			"m_bBornSealed",				pSkillTemplet->m_bBornSealed,					false	);

	LUA_GET_VALUE( luaManager,			"m_wstrIconTextureName",		pSkillTemplet->m_wstrIconName,					L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrIconTexturePieceName",	pSkillTemplet->m_wstrIconPieceName,				L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrImageTextureName",		pSkillTemplet->m_wstrImageName,					L""		);
	LUA_GET_VALUE( luaManager,			"m_wstrImageTexturePieceName",	pSkillTemplet->m_wstrImagePieceName,			L""		);

	SKILL_POWER_RATE_TYPE ePowerRate = SPRT_BASIC_CLASS;
	LUA_GET_VALUE_ENUM( luaManager,		"m_ePowerRate",					ePowerRate,										SKILL_POWER_RATE_TYPE,			SPRT_BASIC_CLASS	);
	SetSkillPowerRate( ePowerRate, pSkillTemplet->m_vecPowerRate );

	LUA_GET_VALUE_ENUM( luaManager,		"m_eActiveSkillUseCondtion",	pSkillTemplet->m_eActiveSkillUseCondtion,		ACTIVE_SKILL_USE_CONDITION,		ASUT_NONE	);

	LUA_GET_VALUE( luaManager,			"m_bShowSkillLevel",			pSkillTemplet->m_bShowSkillLevel,				false	);

	LUA_GET_VALUE( luaManager,			"m_iRequireLearnSkillPoint",	pSkillTemplet->m_iRequireLearnSkillPoint,		0		);

	LUA_GET_VALUE( luaManager,			"m_iRequireUpgradeSkillPoint",	pSkillTemplet->m_iRequireUpgradeSkillPoint,		0		);

	LUA_GET_VALUE( luaManager,			"m_usWOSType",					pSkillTemplet->m_iWOSType,	0		);


	SetSkillStat( luaManager, pSkillTemplet->m_vecStat );

	std::map<SKILL_ID, SkillTemplet*>::iterator it = m_mapSkillTemplet.find( pSkillTemplet->m_eID );
	if( it == m_mapSkillTemplet.end() )
	{
		m_mapSkillTemplet[ pSkillTemplet->m_eID ] = pSkillTemplet;
	}
	else
	{
		ASSERT( !"SKILL is Duplication!" );	/// ��ų �ߺ�!
		return true;
	}


	if( true == luaManager.BeginTable( "SA_EFFECTIVE_TIME" ) )					SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_TIME, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_COUNT" ) )          		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_COUNT, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SKILL_ID" ) )                 		SetSkillAbilityMap( &luaManager, SA_SKILL_ID, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_COMBO_ID" ) )                 		SetSkillAbilityMap( &luaManager, SA_COMBO_ID, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_RATE" ) )           		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_RATE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_REL" ) )                  		SetSkillAbilityMap( &luaManager, SA_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_REL" ) )            		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_PHYSIC_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_MAGIC_REL" ) )            		SetSkillAbilityMap( &luaManager, SA_DEF_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_ATK_PHYSIC_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_TEAM_ATK_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_ATK_MAGIC_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_DEF_PHYSIC_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_PHYSIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TEAM_DEF_MAGIC_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_TEAM_DEF_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_DAMAGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGED_REL" ) )              		SetSkillAbilityMap( &luaManager, SA_DAMAGED_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_UNFIXED_DEFENSE_REL" ) )      		SetSkillAbilityMap( &luaManager, SA_UNFIXED_DEFENSE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SUMMON_ATK_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_SUMMON_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_HP_ABS" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_HP_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_HP_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_HP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_MP_ABS" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_MP_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MAX_MP_REL" ) )               		SetSkillAbilityMap( &luaManager, SA_MAX_MP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TELEPORT_RANGE_ABS" ) )       		SetSkillAbilityMap( &luaManager, SA_TELEPORT_RANGE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_CHARGING_TIME" ) )            		SetSkillAbilityMap( &luaManager, SA_CHARGING_TIME, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_REL_TO_MP_ABS_1" ) )       		SetSkillAbilityMap( &luaManager, SA_HP_REL_TO_MP_ABS_1, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_REL_TO_MP_ABS_2" ) )       		SetSkillAbilityMap( &luaManager, SA_HP_REL_TO_MP_ABS_2, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_CONSUME_REL" ) )           		SetSkillAbilityMap( &luaManager, SA_MP_CONSUME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_GAIN_ON_HIT_REL" ) )       		SetSkillAbilityMap( &luaManager, SA_MP_GAIN_ON_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SCOPE_ABS" ) )                		SetSkillAbilityMap( &luaManager, SA_SCOPE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_FIRE_COUNT" ) )    		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_FIRE_COUNT, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_RANGE_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_RANGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_SIZE" ) )          		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_SIZE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_PIERCING" ) )      		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_PIERCING, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MOVE_SPEED" ) )               		SetSkillAbilityMap( &luaManager, SA_MOVE_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_JUMP_SPEED" ) )                   	SetSkillAbilityMap( &luaManager, SA_JUMP_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_TO_ATK_MAGIC" ) )      	SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_TO_ATK_MAGIC, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_TO_ATK_PHYSIC" ) )      	SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_TO_ATK_PHYSIC, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_READY_TIME_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_READY_TIME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_MELEE_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_MELEE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_RANGE_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_RANGE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_ABS" ) )               	SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_REGENERATION_ABS" ) )          	SetSkillAbilityMap( &luaManager, SA_MP_REGENERATION_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_INTAKE_ABS" ) )                	SetSkillAbilityMap( &luaManager, SA_MP_INTAKE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_ANI_SPEED" ) )             	SetSkillAbilityMap( &luaManager, SA_TARGET_ANI_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_MOVE_SPEED" ) )            	SetSkillAbilityMap( &luaManager, SA_TARGET_MOVE_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_JUMP_SPEED" ) )            	SetSkillAbilityMap( &luaManager, SA_TARGET_JUMP_SPEED, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_TARGET_ATK_REL" ) )               	SetSkillAbilityMap( &luaManager, SA_TARGET_ATK_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_HP_INTAKE_REL_DAMAGE" ) )         	SetSkillAbilityMap( &luaManager, SA_HP_INTAKE_REL_DAMAGE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_DAM_ABS" ) )         	SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_DAM_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_RATE_ABS" ) )        	SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_RATE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DEF_CRITICAL_DAM_ABS" ) )         	SetSkillAbilityMap( &luaManager, SA_DEF_CRITICAL_DAM_ABS, pSkillTemplet );
	//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
	if( true == luaManager.BeginTable( "SA_DEF_CRITICAL_RATE_ABS" ) )        	SetSkillAbilityMap( &luaManager, SA_DEF_CRITICAL_RATE_ABS, pSkillTemplet );
	//}} 2011-04 �� ��ġ�� û ĳ�� ��ų
	if( true == luaManager.BeginTable( "SA_BACK_SPEED_X" ) )                 	SetSkillAbilityMap( &luaManager, SA_BACK_SPEED_X, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ITEM_SPECIAL_ABILITY_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_ITEM_SPECIAL_ABILITY_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_GAIN_GET_HIT_REL" ) )          	SetSkillAbilityMap( &luaManager, SA_MP_GAIN_GET_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SOUL_GAIN_ON_HIT_REL" ) )         	SetSkillAbilityMap( &luaManager, SA_SOUL_GAIN_ON_HIT_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SOUL_GAIN_GET_HIT_REL" ) )        	SetSkillAbilityMap( &luaManager, SA_SOUL_GAIN_GET_HIT_REL, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_HP_GAIN_REL_MAX_HP" ) )           	SetSkillAbilityMap( &luaManager, SA_HP_GAIN_REL_MAX_HP, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ED_GAIN_REL" ) )                  	SetSkillAbilityMap( &luaManager, SA_ED_GAIN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL" ) )	SetSkillAbilityMap( &luaManager, SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EXP_GAIN_REL" ) )                 	SetSkillAbilityMap( &luaManager, SA_EXP_GAIN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_ABSORB_REL_MAX_HP" ) )     	SetSkillAbilityMap( &luaManager, SA_DAMAGE_ABSORB_REL_MAX_HP, pSkillTemplet );



	if( true == luaManager.BeginTable( "SA_STRONG_MIND_ACTIVE_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_STRONG_MIND_ACTIVE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_STRONG_MIND_SPECIAL_ACTIVE_REL" ) ) SetSkillAbilityMap( &luaManager, SA_STRONG_MIND_SPECIAL_ACTIVE_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_MP_RETAKE_ABS" ) )     				SetSkillAbilityMap( &luaManager, SA_MP_RETAKE_ABS, pSkillTemplet );


	if( true == luaManager.BeginTable( "SA_RESIST_FIRE_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_FIRE_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_WATER_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_WATER_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_GREEN_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_GREEN_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_WIND_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_WIND_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_LIGHT_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_LIGHT_ABS, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_RESIST_DARK_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RESIST_DARK_ABS, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_FIRST_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_FIRST_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_SECOND_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_SECOND_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_ATK_MAGIC_THIRD_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_ATK_MAGIC_THIRD_REL, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_REMOTE_SPEED_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_REMOTE_SPEED_ABS, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_DAMAGE_TYPE_CHANGE" ) )     		SetSkillAbilityMap( &luaManager, SA_DAMAGE_TYPE_CHANGE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DAMAGE_TIME" ) )     				SetSkillAbilityMap( &luaManager, SA_DAMAGE_TIME, pSkillTemplet );

	//{{ ����� : 2010.11.12
	if ( true == luaManager.BeginTable( "SA_RECOVER_MP_ABS" ) )     			SetSkillAbilityMap( &luaManager, SA_RECOVER_MP_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_MANA" ) )     				SetSkillAbilityMap( &luaManager, SA_TARGET_MANA, pSkillTemplet );
	//}} ����� : 2010.11.12

	//{{ JHKang / ������ / 2010/11/22
	if ( true == luaManager.BeginTable( "SA_TARGET_MANA_REL" ) )				SetSkillAbilityMap( &luaManager, SA_TARGET_MANA_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SIZE_ABS" ) )						SetSkillAbilityMap( &luaManager, SA_SIZE_ABS, pSkillTemplet );
	//}} JHKang / ������ / 2010/11/22

	if ( true == luaManager.BeginTable( "SA_REFLECT_REL" ) )					SetSkillAbilityMap( &luaManager, SA_REFLECT_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EL_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_EL_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_HP_REL" ) )					SetSkillAbilityMap( &luaManager, SA_TARGET_HP_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TARGET_HITRATE_REL" ) )			SetSkillAbilityMap( &luaManager, SA_TARGET_HITRATE_REL, pSkillTemplet );

	//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
	if ( true == luaManager.BeginTable( "SA_CHARGE_CANNONBALL" ) )				SetSkillAbilityMap( &luaManager, SA_CHARGE_CANNON_BALL, pSkillTemplet );
	//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

	//{{ kimhc // 2011.1.14 // û 1�� ����
	if ( true == luaManager.BeginTable( "SA_FIRE_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_FIRE_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_WATER_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_WATER_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_NATURE_DEFENCE_ABS" ) )			SetSkillAbilityMap( &luaManager, SA_NATURE_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_WIND_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_WIND_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_LIGHT_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_LIGHT_DEFENCE_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_DARK_DEFENCE_ABS" ) )				SetSkillAbilityMap( &luaManager, SA_DARK_DEFENCE_ABS, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_EDT_FIRE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_FIRE_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_FROZEN_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_FROZEN_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_ICE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_ICE_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_POISON_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_POISON_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_Y_PRESSED_RESIST_REL" ) )		SetSkillAbilityMap( &luaManager, SA_EDT_Y_PRESSED_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_LEG_WOUND_RESIST_REL" ) )		SetSkillAbilityMap( &luaManager, SA_EDT_LEG_WOUND_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_STUN_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_STUN_RESIST_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_EDT_CURSE_RESIST_REL" ) )			SetSkillAbilityMap( &luaManager, SA_EDT_CURSE_RESIST_REL, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_EXPAND_CB" ) )						SetSkillAbilityMap( &luaManager, SA_EXPAND_CB, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ANI_SPEED" ) )						SetSkillAbilityMap( &luaManager, SA_ANI_SPEED, pSkillTemplet );
	//}} kimhc // 2011.1.14 // û 1�� ����

	//{{ kimhc // 2011-04-01 // 2011-04 �� ��ġ�� û ĳ�� ��ų
	if ( true == luaManager.BeginTable( "SA_ACCURACY_PERCENT_ABS" ) )			SetSkillAbilityMap( &luaManager, SA_ACCURACY_PERCENT_ABS, pSkillTemplet );
	//}} kimhc // 2011-04-01 // 2011-04 �� ��ġ�� û ĳ�� ��ų

	if ( true == luaManager.BeginTable( "SA_CRITICAL_EVASION_REL" ) )			SetSkillAbilityMap( &luaManager, SA_CRITICAL_EVASION_REL, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_DECREASE_REL" ) )					SetSkillAbilityMap( &luaManager, SA_DECREASE_REL, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FIRST_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FIRST_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_SECOND_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_SECOND_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_THIRD_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_THIRD_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FOURTH_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FOURTH_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_ATK_ADD_DAMAGE_FIFTH_ABS" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_ADD_DAMAGE_FIFTH_ABS, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_TELEPORT_SPEED_REL" ) )			SetSkillAbilityMap( &luaManager, SA_TELEPORT_SPEED_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "HP_CONSUME_REL" ) )					SetSkillAbilityMap( &luaManager, HP_CONSUME_REL, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_MP_BURN_ABS" ) )					SetSkillAbilityMap( &luaManager, SA_MP_BURN_ABS, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_01" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_01, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_02" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_02, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_03" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_03, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_04" ) )		SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_04, pSkillTemplet );

	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_01_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_01_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_02_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_02_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_03_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_03_RATE, pSkillTemplet );
	if ( true == luaManager.BeginTable( "SA_SKILLDAMAGE_MULTIPLE_04_RATE" ) )	SetSkillAbilityMap( &luaManager, SA_SKILLDAMAGE_MULTIPLE_04_RATE, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_EVASION_REL" ) )     				SetSkillAbilityMap( &luaManager, SA_EVASION_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_REDUCE_EDT_TIME_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_REDUCE_EDT_TIME_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_OVERHEAT_HP" ) )     				SetSkillAbilityMap( &luaManager, SA_OVERHEAT_HP, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_SCOPE_ABS_HYPER" ) )     			SetSkillAbilityMap( &luaManager, SA_SCOPE_ABS_HYPER, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_TIME_HYPER" ) )     		SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_TIME_HYPER, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_AIR_MOVE_COUNT" ) )     			SetSkillAbilityMap( &luaManager, SA_AIR_MOVE_COUNT, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_NOT_CANNONBALL_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_NOT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_INSERT_CANNONBALL_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_INSERT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_BE_SHOT_CANNONBALL_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_BE_SHOT_CANNONBALL_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_CANNONBALL_DAMAGE_UP_REL" ) )     	SetSkillAbilityMap( &luaManager, SA_CANNONBALL_DAMAGE_UP_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE" ) )SetSkillAbilityMap( &luaManager, SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SIEGE_SHELLING_REL" ) )     		SetSkillAbilityMap( &luaManager, SA_SIEGE_SHELLING_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_EFFECTIVE_DAMAGED_ATTACK_BASE" ) )  SetSkillAbilityMap( &luaManager, SA_EFFECTIVE_DAMAGED_ATTACK_BASE, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_FIRST_HYPER_EFFECTIVE" ) )			SetSkillAbilityMap( &luaManager, SA_FIRST_HYPER_EFFECTIVE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_SECOND_HYPER_EFFECTIVE" ) )     	SetSkillAbilityMap( &luaManager, SA_SECOND_HYPER_EFFECTIVE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_THIRD_HYPER_EFFECTIVE" ) )			SetSkillAbilityMap( &luaManager, SA_THIRD_HYPER_EFFECTIVE, pSkillTemplet );

	if( true == luaManager.BeginTable( "SA_ATK_CRITICAL_RATE_ATK_BASE" ) )		SetSkillAbilityMap( &luaManager, SA_ATK_CRITICAL_RATE_ATK_BASE, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_PROJECTILE_CURVE_SPEED_REL" ) )		SetSkillAbilityMap( &luaManager, SA_PROJECTILE_CURVE_SPEED_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DP_GAIN_REL" ) )					SetSkillAbilityMap( &luaManager, SA_DP_GAIN_REL, pSkillTemplet );
	if( true == luaManager.BeginTable( "SA_DP_USE_REL" ) )						SetSkillAbilityMap( &luaManager, SA_DP_USE_REL, pSkillTemplet );

	CX2BuffTempletManager::GetInstance()->GetBuffFactorPtrFromBuffFactorList( luaManager, pSkillTemplet->m_vecBuffFactorPtr, pSkillTemplet->m_vecBuffFactorID );


	return true;
}


//{{ 2009. 8. 5  ������		���� ��ų ������ ����

bool CX2SkillTree::AddSealSkillInfo_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	int							iUnsealItemID = 0;					/// ������ ���̵�
	CX2Unit::UNIT_CLASS_TYPE	eUnitClassType = CX2Unit::UCT_NONE;	/// ��� ������ ���� Ŭ����
	vector<SKILL_ID>			vecSkillID;							/// ��� ������ ���� ��ų ���̵� ���� �����̳�

	/// ������ ���̵� �Ľ�
	LUA_GET_VALUE( luaManager,		"m_iItemID",		iUnsealItemID,		0 );

	/// ��� ������ ���� Ŭ���� �Ľ�
	LUA_GET_VALUE_ENUM( luaManager, "m_eUnitClassType",	eUnitClassType,		CX2Unit::UNIT_CLASS_TYPE,		CX2Unit::UCT_NONE		);

	/// ���� ��ų ���̵� �Ľ��ؼ� �����̳ʿ� ����
	if( true == luaManager.BeginTable( "m_SkillID" ) )
	{
		int iSkillID	= 0;	/// ��ų ���̵�
		int iTableIndex = 1;	/// ���̺� �ε���

		while( luaManager.GetValue( iTableIndex, iSkillID ) == true )
		{
			vecSkillID.push_back( static_cast<SKILL_ID>( iSkillID ) );
			++iTableIndex;
		}

		luaManager.EndTable();
	}

	/// �ʿ� ����
	SealSkillItemInfo kInfo;
	kInfo.m_iUnitClassType	= static_cast<int>( eUnitClassType );
	kInfo.m_vecSkillID		= vecSkillID;
	m_mapSealSkillItemTemplet.insert( std::make_pair( iUnsealItemID, kInfo ) );
	return true;
}

//}}



const CX2SkillTree::SkillTreeTemplet* CX2SkillTree::GetSkillTreeTemplet( int iUnitClass, SKILL_ID eSkillID )
{
	KTDXPROFILE();

	UnitSkillTreeTempletMap::iterator it = m_mapUnitSkillTreeTemplet.find( iUnitClass );
	if( it == m_mapUnitSkillTreeTemplet.end() )
		return 0;

	SkillTreeTempletMap& mapSkillTreeTemplet = it->second;
	SkillTreeTempletMap::iterator itSkillTreeTemplet = mapSkillTreeTemplet.find( eSkillID );
	if( itSkillTreeTemplet == mapSkillTreeTemplet.end() )
		return 0;

	return &itSkillTreeTemplet->second;
}
// oasis907 : ����� [2009.11.25] // ���� �Ľ� ���� �ּ� ����
//{{ oasis907 : ����� //// 2009-11-16 ////
const CX2SkillTree::SkillTreeTemplet* CX2SkillTree::GetGuildSkillTreeTemplet( int iGuildClass, SKILL_ID eSkillID )
{
	KTDXPROFILE();

	if(iGuildClass != 0)
	{
		ASSERT( !"Guild Class Not Implemented Yet!!" );
	}

	GuildSkillTreeTempletMap::iterator it = m_mapGuildSkillTreeTemplet.find( iGuildClass );
	if( it == m_mapGuildSkillTreeTemplet.end() )
		return 0;

	SkillTreeTempletMap& mapSkillTreeTemplet = it->second;
	SkillTreeTempletMap::iterator itSkillTreeTemplet = mapSkillTreeTemplet.find( eSkillID );
	if( itSkillTreeTemplet == mapSkillTreeTemplet.end() )
		return 0;

	return &itSkillTreeTemplet->second;
}
//}} oasis907 : ����� //// 2009-11-16 ////

int CX2SkillTree::GetMaxSkillLevel( int iUnitClass, SKILL_ID eSkillID )
{
#if X2_DEV_TOOLS
	/// ������ ��� - 30�������� ���� �����ϵ��� ����
	if( true == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
	{
		return MAX_LIMITED_SKILL_LEVEL;
	}
#endif // _SERVICE_

	const SkillTreeTemplet* pSkillTreeTemplet = GetSkillTreeTemplet( iUnitClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return 0;

	return pSkillTreeTemplet->m_iMaxSkillLevel;
}
//{{ oasis907 : ����� //// 2009-11-16 ////
int CX2SkillTree::GetMaxGuildSkillLevel( int iGuildClass, SKILL_ID eSkillID )
{
	const SkillTreeTemplet* pSkillTreeTemplet = GetGuildSkillTreeTemplet( iGuildClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return 0;

	return pSkillTreeTemplet->m_iMaxSkillLevel;
}
//}} oasis907 : ����� //// 2009-11-16 ////


const CX2SkillTree::SkillTemplet* CX2SkillTree::GetSkillTemplet( SKILL_ID eSkillID ) const
{
	KTDXPROFILE();


	SkillTempletMap::const_iterator it = m_mapSkillTemplet.find( eSkillID );
	if( m_mapSkillTemplet.end() != it )
	{
		SkillTemplet* pSkillTemplet = it->second;
		return pSkillTemplet;
	}

	if ( CX2SkillTree::SI_NONE != eSkillID )		/// ��ų ���ø��� ������, ��ų ���̵� 0�� �ƴ� ��
		/// �̻��� ��ų ���̵�� ��ų ���ø��� ã���� �ؿ�!!
		ASSERT( !L"Not Find Skill Templet From Unknown SKill ID!!!" );


	return NULL;
}


// 09.05.22 �¿�
const CX2SkillTree::SkillTreeTempletMap& CX2SkillTree::GetSkillTreeTempletMap( int eUnitClass )
{
	UnitSkillTreeTempletMap::iterator it = m_mapUnitSkillTreeTemplet.find(eUnitClass);
	if( it != m_mapUnitSkillTreeTemplet.end() )
	{
		return it->second;
	}

	ASSERT( !"UnitClass Not Found!!" );
	it = m_mapUnitSkillTreeTemplet.begin();
	return it->second;

}


bool CX2SkillTree::IsUnitTypeDefaultSkill( SKILL_ID eSkillID )
{
	switch( eSkillID )
	{
	case SI_A_ES_MEGASLASH:
	case SI_A_AV_CHAIN_FIRE_BALL:
	case SI_A_LE_RAIL_STINGER:
	case SI_A_RF_MAXIMUM_CANNON:
	case SI_A_EN_ILLUSION_STRIKE:
	case SI_A_ES_ROLLING:
	case SI_A_AV_FIRE_ROAD:
	case SI_A_LE_RISING_FALCON:
	case SI_A_RF_BREAKING_FIST:
		case SI_A_EN_PHOTON_BLINK:
	case SI_A_AM_WIND_WEDGE:
	case SI_A_AM_GAIN_FORCE:
		{
			return true;
		} break;
	default:
		break;
	}

	return false;
}


bool CX2SkillTree::AddGuildSkillTreeTemplet_LUA()

{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


	SKILL_ID eSkillID = SI_NONE;
	LUA_GET_VALUE_ENUM( luaManager, "m_iSkillID",				eSkillID,				SKILL_ID,					SI_NONE					);

	const SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
	{
		StateLog( L"skilltemplet is null on loading skilltree templet" );
		return false;
	}

	SkillTreeTemplet skillTreeTemplet;
	LUA_GET_VALUE( luaManager,		"m_iMasterLevel",			skillTreeTemplet.m_iMaxSkillLevel,		0			);
	LUA_GET_VALUE( luaManager,		"m_iRowIndex",				skillTreeTemplet.m_iTier,				0			);
	LUA_GET_VALUE( luaManager,		"m_iColumnIndex",			skillTreeTemplet.m_iIndexInTier,		0			);

	LUA_GET_VALUE( luaManager,		"m_iPrecedingSkill",		skillTreeTemplet.m_iPrecedingSkill,		0			);

	SkillTreeTempletMap& mapSkillTreeTemplet = m_mapGuildSkillTreeTemplet[ 0 ];


	mapSkillTreeTemplet[ eSkillID ] = skillTreeTemplet;


	return true;

}



//{{ oasis907 : ����� //// 2009-11-12 ////

const CX2SkillTree::SkillTreeTempletMap& CX2SkillTree::GetGuildSkillTreeTempletMap( int eGuildClass )
{
	GuildSkillTreeTempletMap::iterator it = m_mapGuildSkillTreeTemplet.find(eGuildClass);
	if( it != m_mapGuildSkillTreeTemplet.end() )
	{
		return it->second;
	}
	/*
	//{{ oasis907 : ����� //// 2009-11-17 //// �ӽ�
	ASSERT( !"GuildClass Not Found!!" );
	*/
	return nulltemplet;


	it = m_mapGuildSkillTreeTemplet.begin();
	return it->second;

}
//}} oasis907 : ����� //// 2009-11-12 ////


//{{ �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���
//}} �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���

//////////////////////////////////////////////////////////////////////////
bool CX2SkillTree::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{
	case EGS_RESET_SKILL_ACK:
		{
			return Handler_EGS_RESET_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_INIT_SKILL_TREE_ACK:
		{
			return Handler_EGS_INIT_SKILL_TREE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GET_SKILL_ACK:
		{
			return Handler_EGS_GET_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_SKILL_SLOT_ACK:
		{
			return Handler_EGS_CHANGE_SKILL_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_SKILL_SLOT_NOT:
		{
			return Handler_EGS_CHANGE_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPIRE_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_UPDATE_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPAND_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_EXPIRE_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPIRE_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_SKILL_SLOT_CHANGE_ITEM_NOT:
		{
			return Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_UNSEAL_SKILL_NOT:
		{
			return Handler_EGS_UNSEAL_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK:
		{
			Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( hWnd, uMsg, wParam, lParam );
			return true;
		} break;

	case EGS_RESET_GUILD_SKILL_ACK:
		{
			return Handler_EGS_RESET_GUILD_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_INIT_GUILD_SKILL_ACK:
		{
			return Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GET_GUILD_SKILL_ACK:
		{
			return Handler_EGS_GET_GUILD_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_RESET_GUILD_SKILL_NOT:
		{
			return Handler_EGS_RESET_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_INIT_GUILD_SKILL_NOT:
		{
			return Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( hWnd, uMsg, wParam, lParam );
		} break;



	case EGS_GET_GUILD_SKILL_NOT:
		{
			return Handler_EGS_GET_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;


/*
	case EGS_CHANGE_GUILD_SKILL_SLOT_ACK:
		{
			return Handler_EGS_CHANGE_GUILD_SKILL_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CHANGE_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_CHANGE_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

*/

	case EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT:
		{
			return Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;
/*
	case EGS_UPDATE_CASH_GUILD_SKILL_POINT_NOT:
		{
			return Handler_EGS_UPDATE_CASH_GUILD_SKILL_POINT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPAND_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_EXPIRE_GUILD_SKILL_SLOT_NOT:
		{
			return Handler_EGS_EXPIRE_GUILD_SKILL_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_UNSEAL_GUILD_SKILL_NOT:
		{
			return Handler_EGS_UNSEAL_GUILD_SKILL_NOT( hWnd, uMsg, wParam, lParam );
		} break;
*/

//}} oasis907 : ����� //// 2009-11-12 ////

	case EGS_GET_NEXT_SKILL_PAGE_ED_ACK:
		{
			return Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_EXPAND_SKILL_PAGE_ACK:
		{
			return Handler_EGS_EXPAND_SKILL_PAGE_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK:
		{
			return Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	}

	return false;
}




bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID )
{

	// ���� ���� ���Ŀ� skill slot �ٲٱ⸦ �õ��ϸ� �ȵǰ� ���´�. ���� �÷��� ������.
	// ������ų ������ �濡 ������ �� �ް�, ���� ���� ���Ŀ� �ٽ� �޴µ�,
	// ��ų ���ÿ� MP �Ҹ� ������ ���ӽ��������� ������ �Ǵµ� ���ӽ��� �Ŀ��� ������ �ȵǱ� ������ ����� ����
	if( false == g_pInstanceData->IsElapsedPacketTiming() )
	{
		return false;
	}
	//g_pInstanceData->GetTimerPacketTiming().restart();


//{{ kimhc // 2010.3.26 // ���� ��ų ���� ����
	// Change_state_Game_start_req�� �����ϸ� ���Ŀ��� ���� ������ ���� �� �� ���� ��ų������ �ٲܼ� ����
	if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
		return true;
//}} kimhc // 2010.3.26 // ���� ��ų ���� ����

	if ( g_pData->GetMyUser() == NULL )
		return false;

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return false;

	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
		false == g_pX2Game->GetMyUnit()->IsValideSlotChange() ) // ���� ���� ��ȿ ���� �˻�
	{
		return false;
	}

	KEGS_CHANGE_SKILL_SLOT_REQ kPacket;
	kPacket.m_iSlotID = iSlotID;
	kPacket.m_iSkillID = static_cast<int>(eSkillID);

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_SKILL_SLOT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_SKILL_SLOT_ACK );

	//{{ kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
	SetSkillSlotChanging( true );			// ��ų����ü���� ��
	//}} kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����

	return true;
}


bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// ��Ŷ ó��
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_SKILL_SLOT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_CHANGE_SKILL_SLOT_ACK );

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{

// #ifdef FIX_SKILL_SLOT_CHANGE_BUG
// 		if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
// 			false == g_pX2Game->GetMyUnit()->IsValideSlotChange() ) // ���� ���� ��ȿ ���� �˻�
// 		{
// 			return true;
// 		}
// #endif // FIX_SKILL_SLOT_CHANGE_BUG


		CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
//{{ kimhc // 2010.3.26 // ���� ��ų ���� ����
		// 2010-03-23 // Change_state_Game_Start_Not�� ���� ���Ŀ��� �ٲ� �� ����
		if ( g_pInstanceData->GetReceiveGameStartNotPacket() == false )
//}} kimhc // 2010.3.26 // ���� ��ų ���� ����
		{
			pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID,
				static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iSkillID ) );

			if ( -1 != kEvent.m_iSlotID2 )
			{
				pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID2,
					static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iSkillID2 ) );
			}


			if( NULL != g_pData->GetUIManager()  )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
			}

			//{{ kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
			SetSkillSlotChanging( false );			// ��ų����ü���� �Ϸ�
			//}} kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
		}

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
		{
			g_pX2Game->GetMyUnit()->SetCanNotInputTime_LUA(0.5f); // ���� �ð� ��ų ��� �Ұ� ���·� ����
			g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( kEvent.m_iSlotID, kEvent.m_iSkillID );

			if ( -1 != kEvent.m_iSlotID2 )
			{
				g_pX2Game->GetMyUnit()->ChangeEquippedSkillState( kEvent.m_iSlotID2, kEvent.m_iSkillID2 );
			}
 		}

		if ( NULL != g_pTFieldGame && NULL != g_pData->GetMyUser() )
		{
			g_pData->GetMyUser()->ChangeEquippedSkillState( kEvent.m_iSlotID, kEvent.m_iSkillID );

			if ( -1 != kEvent.m_iSlotID2 )
			{
				g_pData->GetMyUser()->ChangeEquippedSkillState( kEvent.m_iSlotID2, kEvent.m_iSkillID2 );
			}
		}
	}
	return true;
}


bool CX2SkillTree::Handler_EGS_CHANGE_SKILL_SLOT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	// ���� �� ������ ��� ó��
	CX2Unit::UnitData* pUnitData = NULL;
	if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_ROOM )
	{
		CX2Room::SlotData* pkSlotData = g_pX2Room->GetSlotDataByUnitUID( kEvent.m_iUnitUID );
		if ( pkSlotData != NULL )
		{
			pUnitData = &pkSlotData->m_pUnit->AccessUnitData();
		}
	}

	if ( pUnitData == NULL )
		return false;


	pUnitData->m_UserSkillTree.SetSkillSlotInfo( kEvent.m_iSlotID, (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );

	return true;
}



bool CX2SkillTree::Handler_EGS_GET_SKILL_REQ( USHORT usPageNumber_/* = 0*/ )
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_GET_SKILL_ACK ) )
	{
		return false;
	}

	if (	NULL == g_pData ||
			NULL == g_pData->GetSkillTree() )
	{
		return false;
	}

	const std::map<int, KGetSkillInfo>& mapSkillInfo = g_pData->GetSkillTree()->GetMapSkillInfo();

	if ( 0 == mapSkillInfo.size() )
		return false;

	KEGS_GET_SKILL_REQ kPacket;
	kPacket.m_mapSkillList = mapSkillInfo;
	USHORT usSkillPage = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;

	if ( usPageNumber_ > 0 )
		usSkillPage = usPageNumber_;

	kPacket.m_iActiveSkillPageNumber = usSkillPage;

	kPacket.m_vecNowLearnSkill = m_vecNowLearnSkill;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_SKILL_ACK );

	return true;
}





bool CX2SkillTree::Handler_EGS_GET_SKILL_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// ��Ŷ ó��
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	g_pMain->DeleteServerPacket( EGS_GET_SKILL_ACK );
	{

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_25108 ), g_pMain->GetNowState() );

			if (	NULL == g_pData ||
					NULL == g_pData->GetMyUser() ||
					NULL == g_pData->GetMyUser()->GetSelectUnit() ||
					NULL == g_pData->GetUIManager() ||
					NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			/// ���� ����Ʈ ����
			pUnitData->m_iSPoint	= kEvent.m_iRemainSP;
			pUnitData->m_iCSPoint	= kEvent.m_iRemainCSP;

			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );

			std::map<int, KGetSkillInfo>::iterator it = kEvent.m_mapSkillList.begin();

			for ( ; it != kEvent.m_mapSkillList.end(); ++it )
			{
				KGetSkillInfo getSkillInfo = it->second;

				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( getSkillInfo.m_iSkillID ),
																getSkillInfo.m_iSkillLevel, getSkillInfo.m_iSpendSkillCSPoint,
																pUnitData->m_UserSkillTree.GetUsingPage() );

				const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( getSkillInfo.m_iSkillID );

				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillId );
				switch( pSkillTemplet->m_eType )
				{
				case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE:
					{
						if( g_pData != NULL &&
							g_pData->GetUIManager() != NULL &&
							g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
						{
							g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
						}
					} break;
				default:
					break;
				}
			}

			// ����
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillUp.ogg", false, false );

			if  ( true == g_pData->GetUIManager()->GetUISkillTree()->GetShow() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);

			/// ��ų �߰� ���� ȿ�� ����
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelByBuff();
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();
		}


		return true;
	}


	return false;

}

bool CX2SkillTree::Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eDelSkillID )
{

	if( true == g_pMain->IsWaitingServerPacket( EGS_RESET_SKILL_ACK ) )
	{
		return false;
	}


	KEGS_RESET_SKILL_REQ kPacket;

	kPacket.m_iSkillID = static_cast<int>( eDelSkillID );

	kPacket.m_iActiveSkillPageNumber = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESET_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESET_SKILL_ACK );

	return true;
}

void CX2SkillTree::Hander_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ )
{
	KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ kPacket;

	kPacket.m_iYear		= iYear_;
	kPacket.m_iMonth	= iMonth_;
	kPacket.m_iDay		= iDay_;
	kPacket.m_iHour		= iHour_;
	kPacket.m_iMinute	= iMinute_;

	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK );
}

void CX2SkillTree::Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pChatBox->AddChatLog( kEvent.m_wstrCspEndDate.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
	}
}




bool CX2SkillTree::Handler_EGS_RESET_SKILL_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
		 NULL == g_pData->GetSkillTree() )
	{
		return false;
	}

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_RESET_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();


			/// ���� �ȳ� �˾�
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_25108 ), g_pMain->GetNowState() );

			CX2Unit::UNIT_CLASS		eUnitClass		= static_cast<CX2Unit::UNIT_CLASS>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );
			CX2SkillTree::SKILL_ID	eDeleteSkillID	= static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID );
			CX2UserSkillTree& userSkillTree	= pUnitData->m_UserSkillTree;


			/// ��ų Ʈ�� ���ø� ��ȯ
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet	= g_pData->GetSkillTree()->GetSkillTreeTemplet( eUnitClass, eDeleteSkillID );

			if ( NULL == pSkillTreeTemplet )
				return false;

			/// ���� ��ų�� ���� ��ų�� �� ��ų�� ���� �ִ��� �˻�
			const bool bHaveFollowingSkill = 0 < userSkillTree.GetSkillLevel( static_cast<CX2SkillTree::SKILL_ID>( pSkillTreeTemplet->m_iFollowingSkill ) ) ? true : false ;

			/// �ʱ�ȭ �� ��ų�� �⺻ ��ų �̰ų�, ���� ��ų�� ���� �ִٸ� 1������ �ʱ�ȭ ��Ű��
			if ( true == isDefaultSkill( eDeleteSkillID ) || true == bHaveFollowingSkill )
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 1, 0,
													pUnitData->m_UserSkillTree.GetUsingPage() );
			/// �� �ܿ� 0������ �ʱ�ȭ
			else
				userSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( kEvent.m_iDelSkillID ), 0, 0,
													pUnitData->m_UserSkillTree.GetUsingPage() );

			pUnitData->m_iSPoint	= kEvent.m_iSPoint;
			pUnitData->m_iCSPoint	= kEvent.m_iCSPoint;

			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );


			// ��ų �ʱ�ȭ �������� ����߱� ������ �κ� ������ �����Ѵ�
			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI��..
				if( NULL != g_pData->GetUIManager() &&
					true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN))
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
			}


			// UI ���� �� ���� ��ų ����
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID );

			if ( NULL == pSkillTemplet )
				return false;

			switch( pSkillTemplet->m_eType )
			{
			case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
			case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
			case CX2SkillTree::ST_PASSIVE:
				{
					if( g_pData != NULL &&
						g_pData->GetUIManager() != NULL &&
						g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
					{
						g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
					}
				} break;

			case CX2SkillTree::ST_BUFF:
			case CX2SkillTree::ST_ACTIVE:
			case CX2SkillTree::ST_SPECIAL_ACTIVE:
			case CX2SkillTree::ST_RELATIONSHIP_SKILL:
			case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
				{
					for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT*2; i++ )
					{
						bool bSlotB = (i > 3) ? true : false;
						int iSlotIndex = (i > 3) ? i-4 : i;
						const CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( iSlotIndex, bSlotB );
						if( NULL != pSkillSlotData )
						{
							if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
							{
								//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
								pUnitData->m_UserSkillTree.SetSkillSlot( iSlotIndex, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, 0, 0, bSlotB );
								//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
							}
						}
					}

				} break;
			default:
				break;
			}

			if( NULL != g_pData->GetUIManager() &&
				true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
			}

			// ����
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );




		}
		return true;
	}
	return false;
}



bool CX2SkillTree::Handler_EGS_INIT_SKILL_TREE_REQ( UidType itemUID )
{
	KEGS_INIT_SKILL_TREE_REQ kPacket;
	kPacket.m_iItemUID = itemUID;
	kPacket.m_iActiveSkillPageNumber = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.GetUsingPage() + 1;

	g_pData->GetServerProtocol()->SendPacket( EGS_INIT_SKILL_TREE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_INIT_SKILL_TREE_ACK );

	return true;
}


bool CX2SkillTree::Handler_EGS_INIT_SKILL_TREE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_SKILL_TREE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_INIT_SKILL_TREE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();


			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI��..
				if( NULL != g_pData->GetUIManager() &&
					0 != g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN) )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList(kEvent.m_vecInventorySlotInfo);

				}
			}

			pUnitData->m_iSPoint = kEvent.m_iSPoint;
			pUnitData->m_iCSPoint = kEvent.m_iCSPoint;

			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iActiveSkillPageNumber );
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );

			// ��ųƮ���� ������ų �ʱ�ȭ
			pUnitData->m_UserSkillTree.Reset( true, true, false, false );

			pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetUsingPage() );	// �� ������ �⺻ ��ų ����

			/// ������ ����
			if ( NULL != g_pData->GetUIManager() &&  NULL != g_pData->GetUIManager()->GetUISkillTree() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, false, false );


			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
				g_pX2Game->GetMyUnit()->InitEquippedActiveSkillState();

			if( NULL != g_pData->GetUIManager() )
			{
				if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
				{
					g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
				}

				if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO) )
				{
					g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
				}
			}

			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"��� ��ų�� �ʱ�ȭ �Ͽ����ϴ�.", m_pNowStage );
		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"��ų �ʱ�ȭ�� �����Ͽ����ϴ�.", m_pNowStage );
		}
		return true;
	}
	return false;
}

bool CX2SkillTree::Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

	switch( g_pMain->GetNowStateID() )
	{
	default:
		{
			On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( kEvent );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2687 ), (CKTDXStage*) g_pMain->GetNowState() );
			g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillEquipedSlotUI();

			pMyUnit->AccessUnitData().m_wstrCSPointEndDate.clear();

// 			if ( NULL != g_pX2Game )
// 			{
// 				g_pX2Game->GetMyUnit()->UpdateSocketDataAndEnchantData();
// 				g_pX2Game->GetMyUnit()->UpdatePassiveAndActiveSkillState();
// 			}



		} break;

	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_DUNGEON_RESULT:
	case CX2Main::XS_PVP_GAME:
	case CX2Main::XS_PVP_RESULT:
		{
			// packetó���� �����صд�.
			KEGS_EXPIRE_CASH_SKILL_POINT_NOT* pEvent = new KEGS_EXPIRE_CASH_SKILL_POINT_NOT;
			*pEvent = kEvent;
			g_pMain->AddReservedServerPacket( EGS_EXPIRE_CASH_SKILL_POINT_NOT, pEvent );

			wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
			D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2687 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_2687 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );

		} break;
	}


	return true;
}

bool CX2SkillTree::Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UPDATE_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );



	if( false == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		return false;


	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
	if( pUnitData->m_UnitUID != kEvent.m_iUnitUID )
		return false;


	if( false == kEvent.m_bUpdateEndDateOnly )
	{
		pUnitData->m_iCSPoint = kEvent.m_iCSPoint;

		int iUsingPage = pUnitData->m_UserSkillTree.GetUsingPage();

		for ( int i = 0; i < pUnitData->m_UserSkillTree.GetOpenedPage(); ++i )
		{
			pUnitData->m_UserSkillTree.SetUsingPage( i + 1 );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );
		}

		pUnitData->m_UserSkillTree.SetUsingPage( iUsingPage + 1 );
	}
	pUnitData->m_wstrCSPointEndDate = kEvent.m_wstrCSPointEndDate;


	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(false, true, false);
	}

	if(kEvent.m_iSkillPointItemID  == SKILL_PLUS_ITEM_ID
		|| kEvent.m_iSkillPointItemID  == SKILL_PLUS_ITEM_ID_JP
		|| kEvent.m_iSkillPointItemID  == SKILL_POINT_10_30DAY_USE_INVEN_JP
		|| kEvent.m_iSkillPointItemID  == SKILL_POINT_10_15DAY_USE_INVEN_JP
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_7DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_15DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_30DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_7DAY_USE_INVEN_2
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_7DAY_USE_INVEN_2


		|| kEvent.m_iSkillPointItemID == SKILL_POINT_130_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_60_1DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == SKILL_POINT_30_14DAY_USE_INVEN
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_15DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_30DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_60DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_15DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_30DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_60DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_30_7DAY
		|| kEvent.m_iSkillPointItemID == LURIEL_GNOSIS_60_7DAY
		)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	//{{ 2011. 11. 30	������	��Ű�� ��ǰ �߰�
	if(kEvent.m_iSkillPointItemID  == SKILL_POINT_5_USE_INVEN_ITEM_ID || kEvent.m_iSkillPointItemID  == SKILL_POINT_5_USE_INVEN_ITEM_ID_7_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	if(kEvent.m_iSkillPointItemID  == SKILL_POINT_10_USE_INVEN_ITEM_ID)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );
	//}}


	if(kEvent.m_iSkillPointItemID  == EVENT_SKILL_POINT_5_USE_INVEN_15_DAY)
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_9836 ), (CKTDXStage*) g_pMain->GetNowState() );

	return true;
}



bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_SLOT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != kEvent.m_iUnitUID )
		return false;

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) kEvent.m_cSkillSlotBExpirationState );
	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );


	// ����
	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}

	if( NULL != g_pX2Game )
	{
		g_pX2Game->UpdateSkillSlotUI( true );
		g_pX2Game->InitExpandSkillCoolTime();
	}

	return true;
}


bool CX2SkillTree::Handler_EGS_EXPIRE_SKILL_SLOT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	if( NULL != g_pData->GetMyUser() )
		return false;

	if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );

		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

		if( g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
		{
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
		}

		switch( g_pMain->GetNowStateID() )
		{
		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
			} break;

		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_DUNGEON_RESULT:
		case CX2Main::XS_PVP_GAME:
		case CX2Main::XS_PVP_RESULT:
			{
				wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
				D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			} break;
		}
	}

	return true;
}

bool CX2SkillTree::Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	if( NULL == g_pData->GetMyUser() )
		return false;

	if( NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SKILL_SLOT_CHANGE_ITEM_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		if ( kEvent.m_cSkillSlotBExpirationState == CX2UserSkillTree::SSBES_EXPIRED )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );

			//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, 0, 0, true );
			//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

			if( g_pData->GetUIManager() != NULL &&
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
			}

			switch( g_pMain->GetNowStateID() )
			{
			default:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
				} break;

			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_DUNGEON_RESULT:
			case CX2Main::XS_PVP_GAME:
			case CX2Main::XS_PVP_RESULT:
				{
					wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
					D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

					g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
					g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				} break;
			}
		}
		else if ( kEvent.m_cSkillSlotBExpirationState == CX2UserSkillTree::SSBES_NOT_EXPIRED )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_NOT_EXPIRED );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );

			if( NULL != g_pData->GetUIManager() &&
				true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
			}
		}
	}
	if( NULL != g_pData && NULL != g_pData->GetGuildManager() && NULL != g_pData->GetUIManager()->GetUISkillTree())
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}

	if( NULL != g_pX2Game )
	{
		if ( NULL != g_pX2Game->GetMyUnit() && g_pX2Game->GetMyUnit()->IsMyUnit() )
		{
			g_pX2Game->UpdateSkillSlotUI( true);
			g_pX2Game->InitExpandSkillCoolTime();
		}
	}

	if( NULL != g_pMain)
	{
		g_pMain->GetGameOption().SetIsSkillUITypeA( g_pMain->GetGameOption().GetIsSkillUITypeA() );
	}

	// ������ // 2013-08-23 // ��Ÿ�� ���� ���� �ʱ�ȭ ���� �߰�
	CX2GageManager::GetInstance()->InitMySkillCoolTimeList( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree );

	return true;
}





bool CX2SkillTree::Handler_EGS_UNSEAL_SKILL_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UNSEAL_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.AddSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );


	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillTree( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );

		g_pData->GetUIManager()->GetUISkillTree()->OpenDLGSkillUnsealed( true );
		g_pData->GetUIManager()->GetUISkillTree()->UpdateDLGSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );
	}


	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}

	return true;
}


void CX2SkillTree::On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent )
{
	if( kEvent.m_iUnitUID <= 0 )
		return;

	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();


	if( pMyUnit->GetUID() == kEvent.m_iUnitUID )
	{
		/// ��ų ���� ���� �ʱ�ȭ �ϰ�, ��Ŷ���� �� ���ڷ� ���� �� ��ų ���� �� ���� 0�� ��ų ����
		if ( NULL != pMyUnit )
		{
			int iUsingPage = pMyUnit->AccessUnitData().m_UserSkillTree.GetUsingPage();

			for ( int i = 0; i < pMyUnit->AccessUnitData().m_UserSkillTree.GetOpenedPage(); ++i )
			{
				pMyUnit->AccessUnitData().m_UserSkillTree.SetUsingPage( i + 1 );
				pMyUnit->AccessUnitData().m_UserSkillTree.ExpireCashSkillPoint( kEvent.m_vecMapHaveSKill[i] );
				int iSP = kEvent.m_vecRetrievedSPoint[i];
				pMyUnit->AccessUnitData().m_UserSkillTree.AddSkillPoint( iSP );
				pMyUnit->AccessUnitData().m_UserSkillTree.SetCashSkillPoint( 0 );

				if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
					g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
			}
			pMyUnit->AccessUnitData().m_UserSkillTree.SetUsingPage( iUsingPage + 1 );
			pMyUnit->AccessUnitData().m_iSPoint = pMyUnit->AccessUnitData().m_UserSkillTree.GetSkillPoint( iUsingPage + 1 );
			pMyUnit->AccessUnitData().m_iCSPoint = pMyUnit->AccessUnitData().m_UserSkillTree.GetCashSkillPoint( iUsingPage + 1 );

			if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, true, true);
		}


	}

}









bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
{
	// ���� ��� ��ų �۾� �Ϸ�ñ��� �ּ�

	//{{ oasis907 : ����� [2009.11.19] // ��帶���� �ƴ� ����� ��� ��ų ���� Ŭ���̾�Ʈ ����
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}
	//}} oasis907 : ����� [2009.11.19] //


	if( true == g_pMain->IsWaitingServerPacket( EGS_GET_GUILD_SKILL_ACK ) )
	{
	return false;
	}


	if ( g_pData->GetMyUser() == NULL )
	return false;

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
	return false;


	KEGS_GET_GUILD_SKILL_REQ kPacket;
	kPacket.m_iGuildSkillID = (int) eSkillID;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_GUILD_SKILL_ACK );

	return true;
}




bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// oasis907 : ����� [2009.11.19] // ACK ���� �Ϸ�

	// ��Ŷ ó��
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	g_pMain->DeleteServerPacket( EGS_GET_GUILD_SKILL_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
		return true;
	}

	return false;

}




bool CX2SkillTree::Handler_EGS_GET_GUILD_SKILL_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// oasis907 : ����� [2009.11.19] // NOT ���� �Ϸ�


	// ��Ŷ ó��
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );



	// oasis907 : ����� [2009.11.19] // ���� �ȿ����� �ǽð� ��ų ���� �ݿ�, ������ �����ε� ����
	// ���� �� ������ ��� ó��
	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
	{
		pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iGuildSkillID, kEvent.m_iGuildSkillLv,
			kEvent.m_iGuildSkillCSPoint, pUnitData->m_UserSkillTree.GetUsingPage() );
		pUnitData->m_iGuildSPoint	= kEvent.m_iGuildSPoint;
		pUnitData->m_iGuildCSPoint	= kEvent.m_iGuildCSPoint;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( (SKILL_ID)kEvent.m_iGuildSkillID );
	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_BUFF:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
		{
			// oasis907 : ����� [2009.11.27] // ��� ��ų���� ��Ƽ�� ��ų �����Ƿ� �ּ�
			/*
			// ���� ������ ��ų�� ���� ����ٸ�
			if(kEvent.m_iSkillLevel == 1)
			{
				// �� ������ �˻��ؼ�
				int EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, false );
				if( EmptySlot != -1 )
				{
					// �������ش�
					Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot, (CX2SkillTree::SKILL_ID)kEvent.m_iSkillID );
				}
				else if( pUnitData->m_UserSkillTree.GetEnabledSkillSlotB() )
				{
					EmptySlot = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_NONE, true );
					if( EmptySlot != -1 )
					{
						Handler_EGS_CHANGE_SKILL_SLOT_REQ( EmptySlot+EQUIPPED_SKILL_SLOT_COUNT, (CX2SkillTree::SKILL_ID)kEvent.m_iSkillID );
					}
				}
			}
			*/

		} break;
	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE:
	case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
		{
			if( g_pData != NULL &&
				g_pData->GetUIManager() != NULL &&
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
			}
		} break;
	default:
		break;
	}

	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}


	// ����
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillUp.ogg", false, false );

	if(pUnitData->m_UserSkillTree.IsMaxSkillLevel( (SKILL_ID) kEvent.m_iGuildSkillID ))
	{
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillMaster.ogg", false, false );
		//const SkillTreeTempletMap& refSkillTreeTempletMap = GetSkillTreeTempletMap( (int)pUnitData->m_UnitClass );

		const SkillTreeTempletMap& refSkillTreeTempletMap = GetGuildSkillTreeTempletMap( (int) 0 ); // oasis
		SkillTreeTempletMap::const_iterator it = refSkillTreeTempletMap.find((SKILL_ID) kEvent.m_iGuildSkillID);
		if( it != refSkillTreeTempletMap.end() )
		{
			const CX2SkillTree::SkillTreeTemplet& refSkillTreeTemplet = it->second;

			if(0 != refSkillTreeTemplet.m_iFollowingSkill )
			{
				// ���� ��ų�� ������ (ȭ��ǥ�� ���� ���̴�)
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillRootOpen.ogg", false, false );
			}
		}
	}

	//	switch( pSkillTemplet->m_Type )
	//	{
	//	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	//	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	//	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	//	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	//		{
	//			if( g_pData != NULL &&
	//				g_pData->GetUIManager() != NULL &&
	//				g_pData->GetUIManager()->GetUICharInfo() != NULL)
	//			{
	//				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	//			}
	//		} break;
	//	}
	//}


	//// ��ų ������ ����ų� ������ ��ų���� ���� ������ ���� ��ų�̸� �ٷ� ����
	//EquipSkillWhenGetSkill( GetGetSkillID() );

	// oasis907 : ����� [2009.12.3] // ������ ���� ��� ��ų ����Ʈ�� ����߽��ϴ�.
	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4795, "L", g_pData->GetGuildManager()->GetMyGuildInfo()->m_wstrGuildMasterNickName)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

	return true;

}







// oasis907 : ����� [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID )
{

	if( true == g_pMain->IsWaitingServerPacket( EGS_RESET_GUILD_SKILL_ACK ) )
	{
		return false;
	}


	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}


	KEGS_RESET_GUILD_SKILL_REQ kPacket;
	kPacket.m_iGuildSkillID = (int) eSkillID;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESET_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESET_GUILD_SKILL_ACK );

	return true;
}

// oasis907 : ����� [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_RESET_GUILD_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// ��ų �ʱ�ȭ �������� ����߱� ������ �κ� ������ �����Ѵ�
			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI��..
				if( NULL != g_pData->GetUIManager() &&
					true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN))
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
			}
			// ����
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );

		}
		return true;
	}
	return false;
}


// oasis907 : ����� [2009.11.27] //
bool CX2SkillTree::Handler_EGS_RESET_GUILD_SKILL_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESET_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	// ��ų Ʈ�� ����
	int iSkillLevel = 0;
	int iSkillCSPoint = 0;
	pUnitData->m_UserSkillTree.GetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, iSkillLevel, iSkillCSPoint );
	iSkillLevel -= 1;
	if( iSkillCSPoint > 0 )
	{
		iSkillCSPoint -= kEvent.m_iRestoreGuildCSPoint;
	}
	pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID, iSkillLevel, iSkillCSPoint,
		pUnitData->m_UserSkillTree.GetUsingPage() );

	pUnitData->m_iGuildSPoint	+= kEvent.m_iRestoreGuildSPoint;
	pUnitData->m_iGuildCSPoint	+= kEvent.m_iRestoreGuildCSPoint;


	// UI ���� �� ���� ��ų ����
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( (CX2SkillTree::SKILL_ID) kEvent.m_iDelGuildSkillID );
	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
	case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
	case CX2SkillTree::ST_PASSIVE:
	case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
	case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
		{
			if( g_pData != NULL &&
				g_pData->GetUIManager() != NULL &&
				g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
			}
		} break;

	case CX2SkillTree::ST_BUFF:
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
		{
			// oasis907 : ����� [2009.11.27] // ��� ��ų���� ��Ƽ�� ��ų �����Ƿ� �ּ�
			/*
			for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT; i++ )
			{
				CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( i, false );
				if( NULL != pSkillSlotData )
				{
					if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
					{
						pUnitData->m_UserSkillTree.SetSkillSlot( i, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, false );
					}
				}


				pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( i, true );
				if( NULL != pSkillSlotData )
				{
					if( pSkillSlotData->m_eID == (CX2SkillTree::SKILL_ID) kEvent.m_iDelSkillID )
					{
						pUnitData->m_UserSkillTree.SetSkillSlot( i, (CX2SkillTree::SKILL_ID)kEvent.m_iDelSkillID, 0, true );
					}
				}
			}
			*/
		} break;
	default:
		break;
	}

	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}

	// ����
	g_pKTDXApp->GetDeviceManager()->PlaySound( L"SkillTree_SkillDown.ogg", false, false );

	int iRestoreGuildSPoint = kEvent.m_iRestoreGuildSPoint + kEvent.m_iRestoreGuildCSPoint;


	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4810, "L", g_pData->GetGuildManager()->GetMyGuildInfo()->m_wstrGuildMasterNickName)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
	// oasis907 : ����� [2009.12.3] // ��� ��ų����Ʈ�� 1 �����Ǿ����ϴ�.
	g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_4797, "i", iRestoreGuildSPoint)), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

	return true;
}








// oasis907 : ����� [2009.11.27] //
bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_REQ( UidType itemUID )
{
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
		return true;
	}

	KEGS_INIT_GUILD_SKILL_REQ kPacket;
	kPacket.m_iItemUID = itemUID;

	g_pData->GetServerProtocol()->SendPacket( EGS_INIT_GUILD_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_INIT_GUILD_SKILL_ACK );

	return true;
}


bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_GUILD_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_INIT_GUILD_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//CX2Unit::UnitData* pUnitData = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();


			if( kEvent.m_vecInventorySlotInfo.size() > 0 )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
				// UI��..
				if( NULL != g_pData->GetUIManager() &&
					0 != g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_INVEN) )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList(kEvent.m_vecInventorySlotInfo);

				}
			}

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4798 ), g_pMain->GetNowState() );
			return true;

		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"��ų �ʱ�ȭ�� �����Ͽ����ϴ�.", m_pNowStage );
		}
		return true;
	}
	return false;
}



bool CX2SkillTree::Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_INIT_GUILD_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	pUnitData->m_iGuildSPoint = kEvent.m_iGuildSPoint;
	pUnitData->m_iGuildCSPoint = kEvent.m_iGuildCSPoint;

	// ��ųƮ���� ������ų �ʱ�ȭ
	pUnitData->m_UserSkillTree.Reset( false, true, true, false, false ); // Ŀ���ؾ���


	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}

	if( NULL != g_pData->GetUIManager() )
	{
		if( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO) )
		{
			g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
		}
	}

	//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"��� ��ų�� �ʱ�ȭ �Ͽ����ϴ�.", m_pNowStage );

	// oasis907 : ����� [2009.12.3] // ��� ��ų�� �ʱ�ȭ �Ǿ����ϴ�.
	g_pChatBox->AddChatLog( GET_STRING(STR_ID_4798), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

	return false;
}





bool CX2SkillTree::Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );



	// oasis907 : ����� [2009.12.4] // ��� ���Խ�
	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	pUnit->AccessUnitData().m_UserSkillTree.SetGuildClass(0);
	pUnit->AccessUnitData().m_UserSkillTree.SetAcquiredGuildSkill( kEvent.m_vecGuildSkillData );

	// oasis907 : ����� [2009.11.27] // ��� ��ų ����Ʈ, ĳ�� ��ų ����Ʈ SET
	pUnit->AccessUnitData().m_iGuildSPoint = kEvent.m_iGuildSPoint;
	pUnit->AccessUnitData().m_iGuildCSPoint = kEvent.m_iGuildCSPoint;
	pUnit->AccessUnitData().m_iMaxGuildCSPoint = 0;
	pUnit->AccessUnitData().m_wstrGuildCSPointEndDate = L"";


	//{{ oasis907 : ����� [2009.12.8] // ��� ��ų�� ���� ���� ����
	if( g_pData != NULL &&
		g_pData->GetUIManager() != NULL &&
		g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
	{
		g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	}

	// ��� ��ų Ʈ��â ������Ʈ
	if( NULL != g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() )
	{
		g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->UpdateUI(true, true, true); // oasis
	}
	return true;
}


/*

bool CX2SkillTree::Handler_EGS_EXPAND_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_SKILL_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if( NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return false;

	if( g_pData->GetMyUser()->GetSelectUnit()->GetUID() != kEvent.m_iUnitUID )
		return false;

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( (CX2UserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) kEvent.m_cSkillSlotBExpirationState );
	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBEndDateString( kEvent.m_wstrSkillSlotBEndDate );


	// ����
	if( NULL != g_pData->GetUIManager() &&
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL) )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, false);
	}

	return true;
}


bool CX2SkillTree::Handler_EGS_EXPIRE_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != g_pData->GetMyUser() )
		return false;

	if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
		return false;


	if( CX2UserSkillTree::SSBES_PERMANENT != g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillSlotBExpirationState() )
	{
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlotBExpirationState( CX2UserSkillTree::SSBES_EXPIRED );

		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 0, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 1, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 2, CX2SkillTree::SI_NONE, 0.f, true );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.SetSkillSlot( 3, CX2SkillTree::SI_NONE, 0.f, true );

		if( g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SKILL))
		{
			g_pData->GetUIManager()->GetUISkillTree()->UpdateUI(true, false, true);
		}

		switch( g_pMain->GetNowStateID() )
		{
		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2686 ), (CKTDXStage*) g_pMain->GetNowState() );
			} break;

		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_DUNGEON_RESULT:
		case CX2Main::XS_PVP_GAME:
		case CX2Main::XS_PVP_RESULT:
			{
				wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
				D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2686 ), coSystemMessageTextColor, D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_2686 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			} break;
		}
	}

	return true;
}






bool CX2SkillTree::Handler_EGS_UNSEAL_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UNSEAL_SKILL_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree.AddSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );


	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillTree( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );

		g_pData->GetUIManager()->GetUISkillTree()->OpenDLGSkillUnsealed( true );
		g_pData->GetUIManager()->GetUISkillTree()->UpdateDLGSkillUnsealed( (CX2SkillTree::SKILL_ID) kEvent.m_iSkillID );
	}


	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}

	return true;
}
*/

//}} oasis907 : ����� //// 2009-11-12 ////

//#pragma region Skill Page System
bool CX2SkillTree::Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_GET_NEXT_SKILL_PAGE_ED_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_GET_NEXT_SKILL_PAGE_ED_ACK );
	{
		if (g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( g_pData && g_pData->GetUIManager() && g_pData->GetUIManager()->GetUISkillTree() )
			{
				g_pData->GetUIManager()->GetUISkillTree()->BuySkillPageDialog( kEvent.m_iED );

				return true;
			}
		}
	}

	return false;
}

bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_PAGE_REQ()
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_EXPAND_SKILL_PAGE_ACK ) )
		return false;

	if ( g_pData && g_pData->GetMyUser() && g_pData->GetMyUser()->GetSelectUnit()
		 && g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED >= 1000 )
	{
		g_pData->GetServerProtocol()->SendID( EGS_EXPAND_SKILL_PAGE_REQ );
		g_pMain->AddServerPacket( EGS_EXPAND_SKILL_PAGE_ACK );

		return true;
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28961 ), g_pMain->GetNowState() );

		return false;
	}
}

bool CX2SkillTree::Handler_EGS_EXPAND_SKILL_PAGE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_EXPAND_SKILL_PAGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_EXPAND_SKILL_PAGE_ACK );
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( NULL == g_pData ||
				 NULL == g_pData->GetMyUser() ||
				 NULL == g_pData->GetMyUser()->GetSelectUnit() ||
				 NULL == g_pData->GetUIManager() ||
				 NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			if ( pUnitData == NULL )
				return false;

			int usDiffValue = kEvent.m_iTheNumberOfSkillPagesAvailable - pUnitData->m_UserSkillTree.GetOpenedPage();

			if ( usDiffValue > 1 )
			{
				pUnitData->m_UserSkillTree.AddSkillPage();
				pUnitData->m_UserSkillTree.AddSkillPage();

				pUnitData->m_UserSkillTree.SetOpenedPage( static_cast<USHORT>( kEvent.m_iTheNumberOfSkillPagesAvailable ) );
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );	// �� ������ �⺻ ��ų ����
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );	// �� ������ �⺻ ��ų ����

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 2 );
			}
			else
			{
				pUnitData->m_UserSkillTree.AddSkillPage();
				pUnitData->m_UserSkillTree.SetOpenedPage( static_cast<USHORT>( kEvent.m_iTheNumberOfSkillPagesAvailable ) );
				pUnitData->m_UserSkillTree.SetDefaultSkill( pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );	// �� ������ �⺻ ��ų ����

				pUnitData->m_UserSkillTree.SetSkillPoint( kEvent.m_iSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
				pUnitData->m_UserSkillTree.SetCashSkillPoint( kEvent.m_iCSPointAvailable, pUnitData->m_UserSkillTree.GetOpenedPage() - 1 );
			}

			if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
			{
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true, true );
			}

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29386 ), g_pMain->GetNowState() );

			pUnitData->m_ED = kEvent.m_iED;

			return true;
		}
	}

	return false;
}

bool CX2SkillTree::Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( USHORT usPageNumber_ )
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK ) )
		return false;

	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ kPacket;
	kPacket.m_iSkillPagesNumberDecidedToUse = usPageNumber_;

	g_pData->GetServerProtocol()->SendPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );

	return true;
}

bool CX2SkillTree::Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK );
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( NULL == g_pData ||
				NULL == g_pData->GetMyUser() ||
				NULL == g_pData->GetMyUser()->GetSelectUnit() ||
				NULL == g_pData->GetUIManager() ||
				NULL == g_pData->GetUIManager()->GetUISkillTree() )
			{
				return false;
			}

			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			if ( pUnitData == NULL )
				return false;

			pUnitData->m_UserSkillTree.SetUsingPage( kEvent.m_iSkillPagesNumberDecidedToUse );
			pUnitData->m_UserSkillTree.SetSelectPage( kEvent.m_iSkillPagesNumberDecidedToUse );
			pUnitData->m_iSPoint = kEvent.m_iSPointAvailable;
			pUnitData->m_iCSPoint = kEvent.m_iCSPointAvailable;
			pUnitData->m_UserSkillTree.SetSkillPoint( pUnitData->m_iSPoint );
			pUnitData->m_UserSkillTree.SetCashSkillPoint( pUnitData->m_iCSPoint );

			for( const KUserSkillData& userSkillData : kEvent.m_kUserSkillPageData.m_vecUserSkillData )
			{
				pUnitData->m_UserSkillTree.SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( userSkillData.m_iSkillID ),
					userSkillData.m_cSkillLevel, userSkillData.m_cSkillCSPoint, pUnitData->m_UserSkillTree.GetUsingPage() );

				const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( userSkillData.m_iSkillID );

				if ( eSkillId == CX2SkillTree::SI_NONE )
					continue;

				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetSkillTemplet( eSkillId );

				switch( pSkillTemplet->m_eType )
				{
				case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
				case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
				case CX2SkillTree::ST_PASSIVE:
					{
						if( g_pData != NULL &&
							g_pData->GetUIManager() != NULL &&
							g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
						{
							g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
						}
					} break;
				default:
					break;
				}
			}

			pUnitData->m_UserSkillTree.SetEquippedSkill( kEvent.m_kUserSkillPageData.m_aEquippedSkill,
				kEvent.m_kUserSkillPageData.m_aEquippedSkillSlotB );

			if ( NULL != g_pTFieldGame && NULL != g_pData->GetMyUser() )
			{
				for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; ++i )
				{
					g_pData->GetMyUser()->ChangeEquippedSkillState( i, kEvent.m_kUserSkillPageData.m_aEquippedSkill[i].m_iSkillID );
					g_pData->GetMyUser()->ChangeEquippedSkillState( i, kEvent.m_kUserSkillPageData.m_aEquippedSkillSlotB[i].m_iSkillID );
				}
			}

			if ( true == g_pData->GetUIManager()->GetUISkillTree()->GetShow() )
				g_pData->GetUIManager()->GetUISkillTree()->UpdateUI( true, true, true );

			// ��ų �߰� ���� ȿ�� ����
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelByBuff();
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();

			return true;
		}
	}

	return false;
}
//#pragma endregion ������ Ȯ��, ���� ��Ŷ ó��





// �¿� : ���ҽ� ������ �۾�
wstring CX2SkillTree::GetSkillLevelTextureName( int /*iLevel*/ )
{
	return L"DLG_UI_Common_Texture_NewSkill_01.tga";

}
wstring CX2SkillTree::GetSkillLevelTextureKey( int iLevel )
{

	WCHAR wszText[16] = {0,};
	StringCchPrintf( wszText, 16, L"LV.%d", iLevel );
	//wsprintf( wszText, L"LV%d", iLevel );
	wstring retval = L"";
	retval += wszText;

	return retval;

}


//{{ 2009. 8. 5  ������		���� ��ų ������ ����

CX2SkillTree::SKILL_ID CX2SkillTree::GetUnsealSkillItemInfo( const int iItemID ) const
{
	if ( NULL != g_pData &&
		 NULL != g_pData->GetMyUser() &&
		 NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 NULL != g_pData->GetSkillTree() )
	{
		/// ���� ��ų �����̳ʿ��� ��ų ������ �˻�
		map<int, SealSkillItemInfo>::const_iterator mit = m_mapSealSkillItemTemplet.find( iItemID );

		if( mit == m_mapSealSkillItemTemplet.end() )
			return CX2SkillTree::SI_NONE;

		const int iUnitClass = static_cast<CX2SkillTree::SKILL_ID>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );	/// ���� Ŭ���� Ÿ��

		const std::vector<CX2SkillTree::SKILL_ID> vVal = mit->second.m_vecSkillID;		/// �ش� ���� �������� ���� ��ų �����̳�

		/// �ش� �ɸ����� ��ų�̶��, ���̵� ��ȯ
		for( CX2SkillTree::SKILL_ID eSkillID : vVal )
		{
			/// ��ų ���̵� �ش� ĳ������ ��ų Ʈ�� ���ø��� �����ϴ��� �˻�
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( iUnitClass, eSkillID );

			if ( NULL != pSkillTreeTemplet )
				return eSkillID;
		}
	}

	return CX2SkillTree::SI_NONE;
}

//}}

/** @function : UnequipRelationSkill
	@breif : �ο� ��ų ���� ����
*/
void CX2SkillTree::UnequipRelationSkill()
{
	// �ο� ��ų ���� ����
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
		{
			// A���Կ��� �˻�
			int iSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, false );

			// ���ٸ� B���Կ��� �˻�
			if( -1 == iSlotIndex )
				pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( CX2SkillTree::SI_ETC_WS_COMMON_LOVE, true );

			// B���Կ��� ���ٸ� ������ ����
			if( -1 == iSlotIndex )
				return;

			Handler_EGS_CHANGE_SKILL_SLOT_REQ( iSlotIndex, CX2SkillTree::SI_NONE );
		}
	}
}

/** @function	: SetSkillAbilityMap
	@brief		: SKILL_ABILITY_TYPE�� �ش��ϴ� ���� �����ϴ� �Լ�
	@param		: �߰��� SKILL_ABILITY_TYPE, �߰��� SkillTemplet

*/
void CX2SkillTree::SetSkillAbilityMap( KLuaManager* pLuaManager, SKILL_ABILITY_TYPE eSkillAbility, CX2SkillTree::SkillTemplet* pSkillTemplet )
{
	if ( NULL == pLuaManager )
	{
		pLuaManager->EndTable();

		return;
	}

	vector<float>	vecSkillValue;			/// 30 ���� ������ ��ų ȿ�� ���� �����ϴ� ��ü
	int				iTableIndex		= 1;	/// ���̺� �ε���
	float			fValue			= 0.f;	/// �Ľ̵� ��

	while( pLuaManager->GetValue( iTableIndex, fValue ) == true )		/// 30���� ���ڰ� �Ľ�
	{
		vecSkillValue.push_back( fValue );
		++iTableIndex;
	}

	pLuaManager->EndTable();				/// ���̺� �ݱ�

	if ( true == vecSkillValue.empty() )	/// ���� ���� �ϳ��� ���� ���Ͽ��ٸ�, 0���� ���� ä����.
	{
		ASSERT( !"SKILL_ABILITY_TYPE Value is Empty!" );	/// ���� �����!
		vecSkillValue.push_back( 0.f );		/// 0���� ��� ä��� ����, ù��° ���� �߰�
	}

	if ( MAX_LIMITED_SKILL_LEVEL >= iTableIndex )	/// ���� ���ڼ��� �ִ� ���� ��ŭ ���ٸ�, ���� �ε����� ���ڷ� ä�� ������.
	{
		for( ; iTableIndex > MAX_LIMITED_SKILL_LEVEL; ++iTableIndex )
		{
			vecSkillValue.push_back( vecSkillValue[iTableIndex - 1] );
		}
	}

	/// ���ڰ� �߰�
	pSkillTemplet->m_mapSkillAbility.insert( std::make_pair( eSkillAbility, vecSkillValue ) );
}

/** @function	: SetSkillVlaue
	@brief		: ��ų ������ �ش��ϴ� ���� �����ϴ� �Լ�
	@param		: ������ �����̳�
*/
template< typename T >
	void CX2SkillTree::SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue, bool bAutoFill /*= true*/ )
{
	/*if ( NULL != pLuaManager )
	{
		pLuaManager.EndTable();

		return;
	}*/

	vecValue.clear();
	int				iTableIndex		= 1;	/// ���̺� �ε���
	T				fValue;					/// �Ľ̵� ��

	while( pLuaManager.GetValue( iTableIndex, fValue ) == true )		/// 30���� ���ڰ� �Ľ�
	{
		vecValue.push_back( fValue );
		++iTableIndex;
	}

	pLuaManager.EndTable();				/// ���̺� �ݱ�

	/// ���� ���ڼ��� �ִ� ���� ��ŭ ����, �ڵ� ä��⸦ ���� �ߴٸ� ���� �ε����� ���ڷ� ä�� ������.
	if ( true != vecValue.empty() && MAX_LIMITED_SKILL_LEVEL >= iTableIndex && true == bAutoFill )
	{
		for( iTableIndex = 1; iTableIndex < MAX_LIMITED_SKILL_LEVEL; ++iTableIndex )
		{
			vecValue.push_back( vecValue[iTableIndex - 1] );
		}
	}
}

/** @function	: SetSkillStat
	@brief		: ��ų ���� ��ġ�� �����ϴ� �Լ�
	@param		: ������ �����̳�
*/
void CX2SkillTree::SetSkillStat( KLuaManager& pLuaManager, vector<CX2Stat::Stat>& vecStat )
{
	/// �� ���� ���ڵ�
	vector<float> vecBaseHP;
	vector<float> vecAtkPhysic;
	vector<float> vecAtkMagic;
	vector<float> vecDefPhysic;
	vector<float> vecDefMagic;

	vecStat.clear();

	/// ���� ��ġ �Ľ�
	if( pLuaManager.BeginTable( "m_Stat" ) == true )
	{
		if( true == pLuaManager.BeginTable( "m_BaseHP" ) )			SetSkillVlaue( pLuaManager, vecBaseHP );
		if( true == pLuaManager.BeginTable( "m_AtkPhysic" ) )		SetSkillVlaue( pLuaManager, vecAtkPhysic );
		if( true == pLuaManager.BeginTable( "m_AtkMagic" ) )		SetSkillVlaue( pLuaManager, vecAtkMagic );
		if( true == pLuaManager.BeginTable( "m_DefPhysic" ) )		SetSkillVlaue( pLuaManager, vecDefPhysic );
		if( true == pLuaManager.BeginTable( "m_DefMagic" ) )		SetSkillVlaue( pLuaManager, vecDefMagic );

		pLuaManager.EndTable(); // m_Stat
	}

	/// ���� ���Ϳ� ���� ��ġ ����
	for ( int i = 0; i < MAX_LIMITED_SKILL_LEVEL; ++i )
	{
		/// ���� ���� ���Ϳ� ���� ������, 0���� ����
		CX2Stat::Stat statvalue;
		statvalue.m_fBaseHP		= 0 <= i && i < static_cast<int>( vecBaseHP.size() ) ? vecBaseHP[i] : 0.f;
		statvalue.m_fAtkPhysic	= 0 <= i && i < static_cast<int>( vecAtkPhysic.size() ) ? vecAtkPhysic[i] : 0.f;
		statvalue.m_fAtkMagic	= 0 <= i && i < static_cast<int>( vecAtkMagic.size() ) ? vecAtkMagic[i] : 0.f;
		statvalue.m_fDefPhysic	= 0 <= i && i < static_cast<int>( vecDefPhysic.size() ) ? vecDefPhysic[i] : 0.f;
		statvalue.m_fDefMagic	= 0 <= i && i < static_cast<int>( vecDefMagic.size() ) ? vecDefMagic[i] : 0.f;

		vecStat.push_back( statvalue );
	}
}

/** @function	: FixOrInsertMapSkillInfoValue
	@brief		: ��ų ���� Ȥ�� ��ȭ�� ������ �ѱ�� ���� ������ ���� �����̳��� �� ���� Ȥ�� �߰� �Լ�
	@param		: ������ ����ü
*/
void CX2SkillTree::FixOrInsertMapSkillInfoValue( CX2SkillTree::SKILL_ID eSkillID )
{
	if (	NULL == g_pData ||
			NULL == g_pData->GetSkillTree() ||
			NULL == g_pData->GetUIManager() ||
			NULL == g_pData->GetUIManager()->GetUISkillTree() ||
			NULL == g_pData->GetMyUser() ||
			NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	const int				iSkillLevel			= g_pData->GetUIManager()->GetUISkillTree()->GetSkillLevelInSkillWimdow( eSkillID );	/// ���� ��ų�� ����

	int						iUnitClass			= static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );				/// ���� Ŭ����
	const CX2SkillTree::SkillTreeTemplet*		pSkillTreeTemplet	= g_pData->GetSkillTree()->GetSkillTreeTemplet( iUnitClass, eSkillID );	/// ��ų Ʈ�� ���ø�

	/// ���� �ø� ������ ��ų�� �Ѱ� ������ �Ѿ�ٸ�, �н�
	if ( NULL != pSkillTreeTemplet && pSkillTreeTemplet->m_iMaxSkillLevel < iSkillLevel + 1 )
		return;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );				/// ��ų ���ø�

	if ( NULL != pSkillTemplet )
	{
		const int iUnitLevel	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level;						/// ���� ���� ����

		/// ���� �ø� ������ ��ų�� �䱸 ���� ���ѿ� �ɸ���, �н�
		if ( pSkillTemplet->GetSkillRequireLevelValue( iSkillLevel + 1 ) > iUnitLevel )
			return;

		const int iHaveCSPoint	= g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint - m_iUsedCSPoint;	/// ���� ĳ�� ��ų ����Ʈ

		/// �Һ� ��ų ����Ʈ ���� ( �������� ��ȭ���� )
		const int	iRequireSP			= 0 == iSkillLevel ? pSkillTemplet->m_iRequireLearnSkillPoint : pSkillTemplet->m_iRequireUpgradeSkillPoint;
		int			iSpendSkillCSPoint	= 0;		/// ����ü�� ������ ĳ�� ��ų ����Ʈ ��� ��

		/// ��ų ����Ʈ �Һ� ó��
		if ( 0 < iHaveCSPoint )  /// ĳ�� ��ų ����Ʈ�� ���� ��
		{
			if ( iRequireSP <= iHaveCSPoint )	/// ĳ�� ��ų ����Ʈ�� ����� ���� ��
			{
				iSpendSkillCSPoint	=  iRequireSP;	/// ���� ĳ�� ��ų ����Ʈ ����
				m_iUsedCSPoint		+= iRequireSP;	/// ĳ�� ��ų ����Ʈ �Һ�
			}
			else		/// ĳ�� ��ų ����Ʈ�� ������ ��
			{
				iSpendSkillCSPoint	=  iHaveCSPoint;					/// ���� ĳ�� ��ų ����Ʈ ���θ� ���
				m_iUsedCSPoint		+= iSpendSkillCSPoint;				/// ĳ�� ��ų ����Ʈ �Һ�
				m_iUsedSPoint		+= iRequireSP - iSpendSkillCSPoint;	/// ������ ����Ʈ�� ��ų ����Ʈ�� �Һ�
			}
		}
		else		/// ĳ�� ����Ʈ�� ���ٸ�, �Ϲ� ��ų ����Ʈ �Һ�
		{
			m_iUsedSPoint += iRequireSP;
		}

		std::map<int, KGetSkillInfo>::iterator it = m_mapGetSkillInfo.find( static_cast<int>( eSkillID ) );

		if ( it != m_mapGetSkillInfo.end() )	/// �ش� ��ų�� �����ϸ�, �ʿ��� ���� ����
		{
			KGetSkillInfo&						getSkillInfo	= it->second;


			getSkillInfo.m_iSkillLevel			+= 1;
			getSkillInfo.m_iSpendSkillCSPoint	+= iSpendSkillCSPoint;

		}
		else									/// �ش� ��ų�� �������� ������, �ʿ� �߰�
		{
			KGetSkillInfo getSkillInfo;

			getSkillInfo.m_iSkillID				=  static_cast<int>( eSkillID );
			getSkillInfo.m_iBeforeSkillLevel	=  iSkillLevel;
			getSkillInfo.m_iSkillLevel			=  iSkillLevel + 1;
			getSkillInfo.m_iSpendSkillCSPoint	+= iSpendSkillCSPoint;

			m_mapGetSkillInfo.insert( std::make_pair( static_cast<int>( getSkillInfo.m_iSkillID ), getSkillInfo ) );

			if( iSkillLevel == 0 )	// ���� ��� ��ų�̸�
			{
				m_vecNowLearnSkill.push_back(getSkillInfo.m_iSkillID);
			}
		}
	}
	else
	{
		ASSERT( !"FixOrInsertMapSkillInfoValue() SkillTemplet Is Error!!!" );
	}
}

/** @function	: AddSkillPowerRate_LUA
	@brief		: ������ ������ ���� ���̺� �Ľ� �Լ�
*/
void CX2SkillTree::AddSkillPowerRate_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	SKILL_POWER_RATE_TYPE	eSkillPowerRateType = SPRT_NONE;	/// ������ ���� ���̵�
	vector<float>			vecPoewrRate;						/// ������ ���� �����̳�

	LUA_GET_VALUE_ENUM( luaManager,	"m_eID", eSkillPowerRateType, SKILL_POWER_RATE_TYPE, SPRT_NONE );

	if( true == luaManager.BeginTable( "m_Value" ) )
		SetSkillVlaue( luaManager, vecPoewrRate );

	if ( SPRT_NONE != eSkillPowerRateType && true != vecPoewrRate.empty() )		/// ������ �Ľ� �Ǿ��� ����, �߰� �Ѵ�.
		m_mapSkillPowerRateType.insert( make_pair( eSkillPowerRateType, vecPoewrRate ) );
}

void CX2SkillTree::SetSkillPowerRate( IN SKILL_POWER_RATE_TYPE eSkillPowerRateType, OUT vector<float>& vecPowerRate )
{
	std::map<SKILL_POWER_RATE_TYPE, vector<float>>::iterator it = m_mapSkillPowerRateType.find( eSkillPowerRateType );

	if ( it != m_mapSkillPowerRateType.end() )
	{
		vector<float> vecValue = it->second;

		if ( vecValue.size() == MAX_LIMITED_SKILL_LEVEL )		/// 30���� ���� �� ������, ���� ���� ������
			vecPowerRate = vecValue;
		else
			ASSERT( !"PowerRate Value Size is Error!!!" );		/// ������ ���� ���� �Ľ� Ȥ�� ������ �߸��Ȱ� ��������!!!
	}
	else
		ASSERT( !"PowerRate Value Size is Error!!!" );			/// ������ ���� ���� �Ľ� Ȥ�� ������ �߸��Ȱ� ��������!!!
}

/** @function	: AddDefaultSkill_LUA
	@brief		: ������ �⺻ ��ų ���̺� �Ľ� �Լ�
*/
void CX2SkillTree::AddDefaultSkill_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	int iTableIndex = 1;	/// ���̺� �ε��� ( Lua�� ���̺� ù��° �ε����� 1 )
	int iValue		= -1;	/// ���ް�
	int iUnitClass	= -1;	/// ���� Ŭ����
	std::vector<int> vecDefaultSkill;

	while( luaManager.GetValue( iTableIndex, iValue ) == true )
	{
		if ( 1 == iTableIndex )		/// ù��° �ε����� Ŭ���� ��
			iUnitClass = static_cast<CX2Unit::UNIT_CLASS>( iValue );
		else						/// ���ķδ� ��ų ���̵�
		{
			if ( 0 != iValue )
				vecDefaultSkill.push_back( iValue );
		}

		++iTableIndex;
	}

	m_mapDefaultSkill.insert( std::make_pair( iUnitClass, vecDefaultSkill ) );
}

/** @function	: AddDefaultSkill_LUA
	@brief		: �ش� ��ų ���̵� �⺻ ��ų���� �˻�
	@param		: �˻��� ��ų ���̵�
	@return		: �⺻ ��ų ����
*/
bool CX2SkillTree::isDefaultSkill( SKILL_ID eSkillID )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
		 return false;

	const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();


	/// �ش� Ŭ������ �⺻ ��ų ��� ��ȯ
	const std::map<int, std::vector<int>>::const_iterator mit = m_mapDefaultSkill.find( pUnitData->m_UnitClass );

	if ( mit != m_mapDefaultSkill.end() )
	{
		std::vector<int> vecDefaultSkill = mit->second;		/// �⺻ ��ų ���

		/// ���ڷ� ���� ���̵� �⺻ ��ų ��Ͽ� �ִ��� �˻�
		for( int iVal : vecDefaultSkill )
		{
			if ( eSkillID == static_cast<SKILL_ID>( iVal ) )
				return true;		/// ��ġ�ϴ� ���̵� �����Ƿ�, �⺻ ��ų�̴�.
		}
	}

	return false;
}

/** @function	:
	@brief		: ���� �ɸ����� ��ų���� �˻�
	@param		: �˻��� ��ų ���̵�
	@return		: �ڽ��� ��ų ����
*/
bool CX2SkillTree::IsMyClassSkillID( SKILL_ID eSkillID )
{
	if ( NULL == g_pData ||
		 NULL == g_pData->GetMyUser() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return false;
	}

	/// �ش� ��ų ���̵� ���� �ɸ����� ��ų���� �˻�
	const CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

	const SkillTreeTempletMap& mapSkillTreeTemplet = GetSkillTreeTempletMap( static_cast<int>( eUnitClass ) );

	SkillTreeTempletMap::const_iterator mit = mapSkillTreeTemplet.find( eSkillID );

	if ( mapSkillTreeTemplet.end() != mit )
		return true;

	return false;
}

/** @function	: GetUnsealSkillItemRequireClass
	@brief		: �ش� ���� ��ų �������� �䱸�ϴ� ���� Ŭ���� ���� ��ȯ
	@param		: ������ ���̵�
	@return		: ���� Ŭ���� ����
*/
int CX2SkillTree::GetUnsealSkillItemRequireClass( const int iItemID ) const
{
	std::map< int, SealSkillItemInfo >::const_iterator mit = m_mapSealSkillItemTemplet.find( iItemID );

	if ( mit != m_mapSealSkillItemTemplet.end() )
		return mit->second.m_iUnitClassType;

	return 0;
}

/** @function	: GetCalcInitSkillPoint
	@brief		: ������ ���� �� ��ų ����Ʈ ��ȯ
	@param		: ���� ����
	@return		: �� ��ų ����Ʈ
*/
int CX2SkillTree::GetCalcInitSkillPoint( const int iUnitLevel ) const
{
	int iSkillPoint = 0;

	for( int iLevel = 2 ; iLevel <= iUnitLevel ; ++iLevel )
	{
		int iLevelSkillPoint = lua_tinker::call<int>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "CalcLevelUpIncreaseSkillPoint", iLevel );
		iSkillPoint += iLevelSkillPoint;
	}

	return iSkillPoint;
}




bool CX2SkillTree::SkillTemplet::GetSkillEffectiveDescExpand ( OUT wstring & wstrSkillEffectiveDesc, int iLevel ) const
{
	// ��ų ������ Ȯ���ؾ� �� ��ų �ΰ�?
	// Ex. ���� ��ġ, ������ ����Ʈ
	switch ( m_eID )
	{
	case CX2SkillTree::SI_SA_ABM_ENERGY_SPURT:
		// Ȥ�� �� ũ���ø� ���� ���� ó��..
		if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			float fEnergySpurtAddDamageValue = powf ( static_cast<float> ( g_pData->GetSelectUnitLevel() ), 2.f ) * 35.f * GetSkillPowerRateValue( iLevel );

			wstrSkillEffectiveDesc = g_pMain->GetReplacedWstring ( wstrSkillEffectiveDesc, "i", static_cast<int> ( fEnergySpurtAddDamageValue ) );
			return true;
		}
		break;
	default :
		return false;
	}
	return false;
}
