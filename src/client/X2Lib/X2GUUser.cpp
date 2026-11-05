#include "stdafx.h"
#include "./X2GUUser.h"




// dmlee 2009.3.12 ��׶��� �ε������ؼ� eqip�� unit_xskinanim�� shared_ptr�� �ؾ��ؼ� guuser�� m_pxskinanim, m_porgxskinanim�� shared_ptr�� �����

// dmlee 2009.11.10 - skip point���� �Ʒ��� 3���� ������ �ֽ��ϴ�.
// �ʻ�� ĵ��	: 	else if( SpecialAttackEventProcess() == true )
// ��� ĵ��	:	else if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.5f && (m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true) ) { StateChangeDashIfPossible(); }
// �ȱ� ĵ��	:	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC { if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.267f ) }



#if X2_DEV_TOOLS
IMPLEMENT_HELP_SYSTEM( CX2GUUser )
{
	WCHAR   wbuffer[1024];
	int     ix = 16;
	int     iy = 16;
	StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer), L"%S", __FUNCTION__ );

	if ( g_pMain && g_pX2Game && g_pX2Game->GetMyUnit() )
	{
		g_pMain->GetFontForDebuf()->OutTextXY( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );

		g_pX2Game->GetMyUnit()->Debug_RenderHelp( ix, iy );
	}//if
}
#endif // KHELP_SYSTEM_TEST


namespace _CONST_CUTIN_
{
	static const wstring s_SkillCutInElsword[] =
	{
		L"0",
		L"Bringit",
		L"1"
	};

	static const wstring s_SkillCutInElswordKnight[] =
	{
		L"taste some steel",
		L"thiswillhurtalot",
		L"timetocallyourmomma"
	};

	static const wstring s_SkillCutInElswordMagicKnight[] =
	{
		L"feeltheburn",
		L"fired",
		L"readysomeheat"
	};

	static const wstring s_SkillCutInAisha[] =
	{
		L"heyugly",
		L"ididitagain",
		L"idontthink"
	};

	static const wstring s_SkillCutInAishaHigh[] =
	{
		L"mighttikle",
		L"playtimesover",
		L"youshallnotpass"
	};

	static const wstring s_SkillCutInAishaDark[] =
	{
		L"despair",
		L"fearthedarkness",
		L"youcan'thandlethis"
	};

	static const wstring s_SkillCutInLena[] =
	{
		L"forgiveme",
		L"maydeath",
		L"surrenderyourself"
	};

	static const wstring s_SkillCutInLenaCombat[] =
	{
		L"Elders",
		L"isthiswhat",
		L"staydown"
	};

	static const wstring s_SkillCutInLenaSniper[] =
	{
		L"Letmethrough",
		L"Thismarksyourend",
		L"Youhaveveenwarned"
	};

	static const wstring s_SkillCutInRaven[] =
	{
		L"raven_01",
		L"raven_02",
		L"raven_03"
	};

	static const wstring s_SkillCutInRavenSoul[] =
	{
		L"Sword_Taker_01",
		L"Sword_Taker_02",
		L"Sword_Taker_03"
	};

	static const wstring s_SkillCutInRavenOver[] =
	{
		L"Over_taker_01",
		L"Over_taker_02",
		L"Over_taker_03"
	};

	static const wstring s_SkillCutInEve[] =
	{
		L"eve1_0",
		L"eve1_1",
		L"eve1_2",
	};

	static const wstring s_SkillCutInEveExotic[] =
	{
		L"eve1_3",
		L"eve1_4",
		L"eve1_5",
	};

	static const wstring s_SkillCutInEveArchitecture[] =
	{
		L"eve1_6",
		L"eve1_7",
		L"eve1_8",
	};

	static const wstring s_SkillCutInEveNemesis[] =
	{
		L"eve1_3",
		L"eve1_4",
		L"eve1_5",
	};

	static const wstring s_SkillCutInEveEmpress[] =
	{
		L"eve1_6",
		L"eve1_7",
		L"eve1_8",
	};

	static const wstring s_SkillCutInChung[] =
	{
		L"Chung_01",
		L"Chung_02",
		L"Chung_03",
	};

	static const wstring s_SkillCutInChungShooting[] =
	{
		L"Shooting_Guardian_01",
		L"Shooting_Guardian_02",
		L"Shooting_Guardian_03",
	};

	static const wstring s_SkillCutInChungFury[] =
	{
		L"Fury_Guadian_01",
		L"Fury_Guadian_02",
		L"Fury_Guadian_03",
	};

	static const wstring s_SkillCutInChungChaser[] =
	{
		L"Shooting_Guardian_01",
		L"Shooting_Guardian_02",
		L"Shooting_Guardian_03",
	};

	static const wstring s_SkillCutInChungPaladin[] =
	{
		L"Fury_Guadian_01",
		L"Fury_Guadian_02",
		L"Fury_Guadian_03",
	};
	static const wstring s_SkillCutInElswordShethKnight[] =
	{
		L"Sheth_Knight_01",
		L"Sheth_Knight_02",
		L"Sheth_Knight_03",
	};

	static const wstring s_SkillCutInAishaBattleMagician[] =
	{
		L"Battle_Magician_01",
		L"Battle_Magician_02",
		L"Battle_Magician_03",
	};

	static const wstring s_SkillCutInLenaTrappingRanger[] =
	{
		L"Trapping_Ranger_01",
		L"Trapping_Ranger_02",
		L"Trapping_Ranger_03",
	};

	static const wstring s_SkillCutInRavenWeaponTaker[] =
	{
		L"Weapon_Taker_01",
		L"Weapon_Taker_02",
		L"Weapon_Taker_03",
	};

	static const wstring s_SkillCutInEveElectra[] =
	{
		L"Code_Electra_01",
		L"Code_Electra_02",
		L"Code_Electra_03",
	};

	static const wstring s_SkillCutInChungShellingGuardian[] =
	{
		L"Shelling_Guardian_01",
		L"Shelling_Guardian_02",
		L"Shelling_Guardian_03",
	};

	static const wstring s_SkillCutInAra[] =
	{
		L"Ara_01",
		L"Ara_02",
		L"Ara_03",
	};

	static const wstring s_SkillCutInSosun[] =
	{
		L"Sosun_01",
		L"Sosun_02",
		L"Sosun_03",
	};

	static const wstring s_SkillCutInSoma[] =
	{
		L"Soma_01",
		L"Soma_02",
		L"Soma_03",
	};
}


wstring CX2GUUser::GetCutInFileName(bool IsRight)
{
	wstring wstrCutIn = L"HQ_CutIn_Talk_";
	if(IsRight)
	{
		wstrCutIn += L"R_";
	}
	else
	{
		wstrCutIn += L"L_";
	}

	int texID = rand() % 3;
	switch( m_pUnit->GetClass() )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElsword[texID];
		break;
	case CX2Unit::UC_ELSWORD_KNIGHT:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordKnight[texID];
		break;
	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordMagicKnight[texID];
		break;
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordMagicKnight[texID];
		break;
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordKnight[texID];
		break;
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordShethKnight[texID];
		break;
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInElswordShethKnight[texID];
		break;
	case CX2Unit::UC_ARME_VIOLET_MAGE:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAisha[texID];
		break;
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaHigh[texID];
		break;
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaDark[texID];
		break;
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaHigh[texID];
		break;
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaDark[texID];
		break;
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaBattleMagician[texID];
		break;
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAishaBattleMagician[texID];
		break;
	case CX2Unit::UC_LIRE_ELVEN_RANGER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLena[texID];
		break;
	case CX2Unit::UC_LIRE_COMBAT_RANGER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaCombat[texID];
		break;
	case CX2Unit::UC_LIRE_SNIPING_RANGER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaSniper[texID];
		break;
	case CX2Unit::UC_LIRE_WIND_SNEAKER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaCombat[texID];
		break;
	case CX2Unit::UC_LIRE_GRAND_ARCHER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaSniper[texID];
		break;
	case CX2Unit::UC_LIRE_TRAPPING_RANGER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaTrappingRanger[texID];
		break;
	case CX2Unit::UC_LIRE_NIGHT_WATCHER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInLenaTrappingRanger[texID];
		break;
	case CX2Unit::UC_RAVEN_FIGHTER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRaven[texID];
		break;
	case CX2Unit::UC_RAVEN_SOUL_TAKER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenSoul[texID];
		break;
	case CX2Unit::UC_RAVEN_OVER_TAKER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenOver[texID];
		break;
	case CX2Unit::UC_RAVEN_BLADE_MASTER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenSoul[texID];
		break;
	case CX2Unit::UC_RAVEN_RECKLESS_FIST:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenOver[texID];
		break;
	case CX2Unit::UC_RAVEN_WEAPON_TAKER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenWeaponTaker[texID];
		break;
	case CX2Unit::UC_RAVEN_VETERAN_COMMANDER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInRavenWeaponTaker[texID];
		break;
	case CX2Unit::UC_EVE_NASOD:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEve[texID];
		break;
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveExotic[texID];
		break;
	case CX2Unit::UC_EVE_ARCHITECTURE:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveArchitecture[texID];
		break;
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveNemesis[texID];
		break;
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveEmpress[texID];
		break;
	case CX2Unit::UC_EVE_ELECTRA:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveElectra[texID];
		break;
	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInEveElectra[texID];
		break;
	case CX2Unit::UC_CHUNG_IRON_CANNON:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChung[texID];
		break;
	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungFury[texID];
		break;
	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungShooting[texID];
		break;
	case CX2Unit::UC_CHUNG_IRON_PALADIN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungPaladin[texID];
		break;
	case CX2Unit::UC_CHUNG_DEADLY_CHASER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungChaser[texID];
		break;
	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungShellingGuardian[texID];
		break;
	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInChungShellingGuardian[texID];
		break;
	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInAra[texID];
		break;
	case CX2Unit::UC_ARA_LITTLE_HSIEN:
	case CX2Unit::UC_ARA_SAKRA_DEVANAM:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInSosun[texID];
		break;
	case CX2Unit::UC_ARA_LITTLE_DEVIL:
	case CX2Unit::UC_ARA_YAMA_RAJA:
		wstrCutIn += _CONST_CUTIN_::s_SkillCutInSoma[texID];
		break;
	}
	return wstrCutIn;
}


void CX2GUUser::ShowSpecialEffectHyperMode()
{
	// �ϵ� �ڵ� ���� ���� �ɼ����� �ϴ°� ������? ����....�ϴ� �ϵ� �ڵ�����
	//switch(m_pUnit->GetTitleId())// �� �Լ��� Ÿ��Ʋ�� �ȿ��� �ٸ� ��� ã�ƺ���.
	CX2Item* pItem = m_pUnit->GetInventory().GetEquippingItemByEquipPos( CX2Unit::EP_AC_BODY, false );
	if( NULL == g_pX2Game || NULL == pItem )
		return;

	switch( pItem->GetItemTemplet()->GetItemID() )
	{
	case HYPER_EFFECT_ITEM_DARK_DRAGON:
		{
			if( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD )	//���� �� �ʵ�� ����Ʈ
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_DarkDragon_HyperMode_Dungeon", this);
			}

			if( g_pX2Game->GetGameType() == CX2Game::GT_PVP )	// ������ ����Ʈ
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_DarkDragon_HyperMode_Pvp", this);
			}
		}
		break;
	}
}

void CX2GUUser::Debug_RenderHelp( int ix_, int iy_ )
{
	int     ix = ix_ + 16;
	int     iy = iy_ + 16;

	wstringstream wstrm;
	wstrm << L"UID: " << GetUnitUID() << ", " << std::endl;
	wstrm << L"Name: " << GetUnit()->GetNickName() << ", " << std::endl;
	wstrm << L"SPEED:         ( " << m_PhysicParam.nowSpeed.x << ", " << m_PhysicParam.nowSpeed.y << " ), " << std::endl;
	wstrm << L"PASSIVE_SPEED: ( " << m_PhysicParam.passiveSpeed.x << ", " << m_PhysicParam.passiveSpeed.y << " ), " << std::endl;



	wstrm << std::endl;
	wstrm << L"---- FUTURE ---- " << std::endl;
	wstrm << L"nowState: " << (int) m_FrameDataFuture.syncData.nowState << ", " << std::endl;
	wstrm << L"lastTouchLineIndex: " << (int) m_FrameDataFuture.syncData.lastTouchLineIndex << ", " << std::endl;
	wstrm << L"position: ( " << m_FrameDataFuture.syncData.position.x << ", "
		<< m_FrameDataFuture.syncData.position.y << ","
		<< m_FrameDataFuture.syncData.position.z << " ), " << std::endl;


	wstrm << L"m_usRandomTableIndex: " << GetRandomFloat() << ", " << std::endl;


	//wstrm << L"bInvincible: " << (int) m_FrameDataFuture.stateParam.bInvincible << ", " << std::endl;
	//wstrm << L"bSuperArmor: " << (int) m_FrameDataFuture.stateParam.bSuperArmor << ", " << std::endl;

	//wstrm << L"fStateTime: " << (int)m_FrameDataFuture.unitCondition.fStateTime << ", " << std::endl;
	wstrm << L"dirDegree: ( " << m_FrameDataFuture.unitCondition.dirDegree.x << ", "
		<< m_FrameDataFuture.unitCondition.dirDegree.y << ","
		<< m_FrameDataFuture.unitCondition.dirDegree.z << " ), " << std::endl;
	//wstrm << L"dirVector: ( " << m_FrameDataFuture.unitCondition.dirVector.x << ", "
	//	<< m_FrameDataFuture.unitCondition.dirVector.y << ","
	//	<< m_FrameDataFuture.unitCondition.dirVector.z << " ), " << std::endl;

	wstrm << L"bFootOnLine: " << (int)m_FrameDataFuture.unitCondition.bFootOnLine << ", " << std::endl;
//#ifdef STEP_ON_MONSTER_TEST
//	wstrm << L"bFootOnUnit: " << (int)m_FrameDataFuture.unitCondition.bFootOnUnit << ", " << std::endl;
//#endif STEP_ON_MONSTER_TEST



	wstrm << std::endl;
	wstrm << L"---- CURRENT ---- " << std::endl;
	wstrm << L"nowState: " << (int) m_FrameDataNow.syncData.nowState << ", " << std::endl;
	wstrm << L"lastTouchLineIndex: " << (int) m_FrameDataNow.syncData.lastTouchLineIndex << ", " << std::endl;
	wstrm << L"position: ( " << m_FrameDataNow.syncData.position.x << ", "
		<< m_FrameDataNow.syncData.position.y << ","
		<< m_FrameDataNow.syncData.position.z << " ), " << std::endl;

	wstrm << L"m_usRandomTableIndex: " << GetRandomFloat() << ", " << std::endl;

	//wstrm << L"bInvincible: " << (int) m_FrameDataNow.stateParam.bInvincible << ", " << std::endl;
	//wstrm << L"bSuperArmor: " << (int) m_FrameDataNow.stateParam.bSuperArmor << ", " << std::endl;
	//wstrm << L"fStateTime: " << (int)m_FrameDataNow.unitCondition.fStateTime << ", " << std::endl;
	wstrm << L"dirDegree: ( " << m_FrameDataNow.unitCondition.dirDegree.x << ", "
		<< m_FrameDataNow.unitCondition.dirDegree.y << ","
		<< m_FrameDataNow.unitCondition.dirDegree.z << " ), " << std::endl;
	//wstrm << L"dirVector: ( " << m_FrameDataNow.unitCondition.dirVector.x << ", "
	//	<< m_FrameDataNow.unitCondition.dirVector.y << ","
	//	<< m_FrameDataNow.unitCondition.dirVector.z << " ), " << std::endl;

	wstrm << L"bFootOnLine: " << (int)m_FrameDataNow.unitCondition.bFootOnLine << ", " << std::endl;
//#ifdef STEP_ON_MONSTER_TEST
//	wstrm << L"bFootOnUnit: " << (int)m_FrameDataNow.unitCondition.bFootOnUnit << ", " << std::endl;
//#endif STEP_ON_MONSTER_TEST



	g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wstrm.str().c_str(), D3DXCOLOR( 1, 1, 0, 1),
		CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
	iy += 16 * 3;

}





CX2GUUser::CX2GUUser( int unitIndex, int teamNum,
	lua_State*   pScriptLuaState,
	int frameBufferNum, CX2Unit* pUnit )
: CX2GameUnit( unitIndex, teamNum,
	NULL
	)
, m_hSeqWeaponCommon1( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_hSeqWeaponCommon2( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_hSeqWeaponCommon3( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_hSeqEnchantedWeaponCommon4( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_DeadStageIndex( 0 )
, m_DeadSubStageIndex( 0 )
, m_bRotateBone( false )
, m_fRotateBoneDegree( 0.f )
, m_eSpecialAttackKeyPressed( SAKP_NONE )
, m_bSpecialAttackEventProcessedAtThisFrame( false )
, m_iSkillCutInSetIndex( 0 )
, m_bHaveSacrificeOfHero( false )
, m_fPassiveCriticalRate( 0.f )
, m_fShadowOfDisasterRate( 0.f )
//#ifdef TRANSFORMER_TEST
//, m_pTransformer( NULL )
//#endif TRANSFORMER_TEST
	//{{ �ӱԼ� �ӱԼ� ���� ��ŸƮ ǥ�� ������ �� , ���� ���� ���� �Ŀ� ������ ��� ���ϵ��� ����
, m_bStarted( false )
	//}}
//{{ kimhc // 2010.4.5 // ��д��� �۾�(��Ʈ������ȿ��)
, m_fManaRecoveryDurationTime( 0.0f )
, m_fManaRecoveryVal( 0.0f )
, m_fIntervalTimeForCureDebuff( 0.0f )
, m_pBoneShieldData( NULL )
//}} kimhc // 2010.4.5 // ��д��� �۾�(��Ʈ������ȿ��)

//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
, m_iHoldStateID( 0 )
//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

//{{ kimhc // 2010.11.3 // ���� - ��ȭ�� �г�
, m_bCanChangeHyperModeInHyperModeState( false )
//}} kimhc // 2010.11.3 // ���� - ��ȭ�� �г�
, m_bDownForce( false )
, m_fAddAnimSpeedFactor( 0.0f )
, m_iGrapStateID( USI_DAMAGE_AIR_FLY_FRONT )
//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
, m_fCannonBallDamageUp( 0.f )
, m_fForceDownRelDamageRateBase ( 0.f )
, m_iDashCount ( 0 )

, m_ElapsedTimeNotBusyState( 2.0f )

, m_bLearnLightningStep( false )
, m_OldStateId( USI_BASE )
, m_iAdvancedTeleportationLv( 0 )
, m_delegatePhysicProcessPortalByGameType( DelegatePhysicProcessPortalByGameType() )
, m_fCurrentFinalDamage( 0 )
, m_hEnterCashShop(INVALID_PARTICLE_SEQUENCE_HANDLE)
, m_bHasHyperModeItem(false)
, m_pTextureRank (NULL)
, m_bEnterCashShop( false )
, m_bCanAvoidance( false)
, m_bChangeGroggyState( false )		/// ������ ���� ���� �׷α� ������Ʈ ��ȯ ����
, m_ComboAndSkillChainSystem()
, m_bRidingOn( false )
, m_wstrRidingMotionName( L"" )
, m_ElapsedTimeRidingWaitHabit( 5.f )
, m_iRidingBoost( 0 )
, m_fAirDashTime( 0.f )
, m_iAirDashCount( 0 )
, m_iDoubleJumpCount( 0 )
, m_RelationshipTargetUID( 0 )
, m_fAddAccuaracyPercent(0.f)
, m_fAddMagicAttackCriticalRate(0.f)
, m_fAddMagicAttackCriticalDamageRate(0.f)
, m_bSetCustomRenderParam( false )
, m_uiNowStateSkillLevel( 0 )
, m_fShowSkillCutInTime(-1.f)
, m_bChangeWorldColor(false)
, m_iSkillCutInSetSubIndex( 0 )
, m_bIsLimitCrusherCharge( false )
, m_bRightAtThisState( false )
, m_pHairXSkinMesh(NULL)
, fPreWorldNear(-1.f)
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
, m_dwLastRecvFrameMoveCount( 0 )
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
, m_fSlotChangeLeftCoolTime ( -1.f )

, m_bDoubleAttack( false )

, m_bIsPlayAvatarEmotionSoundWithoutEmotion ( false )

, m_fGetFinalDamageValue( 0.f )
{

    m_bFutureCanPassUnit = false;
    m_bFutureApplyAnimMove = true;
    m_bNowFallDown = true;
    m_bDisableGravity = false;
    m_bDisableGravityInScript = false;
    m_vDisableGravityInScriptTime = D3DXVECTOR2( 0, 0 );
    m_vFutureIgnoreLineTime = D3DXVECTOR2( 0, 0 );
    m_fNowSlashTraceTipWide = 4.0f;
    m_fCommonDamageChangeTime =  -1.f;
//{{ kimhc // 2011.1.21 // û 1�� ����
    m_eHittedTypeAtState = CX2DamageManager::HTD_NO_SOUND;	// ������Ʈ �� HittedType ����
//}} kimhc // 2011.1.21 // û 1�� ����
    m_v2NowAfterImageTime = D3DXVECTOR2( -1.f, -1.f );
	m_bDeleteEffectSetOnStateEnd = false;
	m_bDeleteEffectSetOnDamageReact = false;
	m_bDeleteEffectSetOnDie = false;
	m_bDeleteEffectSetOnCustomState = false;
	m_bHyperEffectSet = false;
	m_fEffectSetLifeTime = 0.f;
    m_iMemoId = 0;
    m_bIsCustomStateDeleteEffectOnDamageReact = false;


	if ( pScriptLuaState != NULL )
		m_LuaManager.AssignNewLuaState( pScriptLuaState, 0, false );
	else
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	m_eObjectType = OT_CX2GUUser;


	m_GameUnitType				= GUT_USER;
	m_OwnerUserUID				= pUnit->GetOwnerUserUID();
	m_UnitUID					= pUnit->GetUID();
	m_FrameBufferNum			= frameBufferNum;
	m_pUnit						= pUnit;
	m_MachineGunWaitState		= 0;
	m_ChargeMpState				= 0;
	//{{ robobeg : 2008-10-27
	//	m_DownDamageLandFront		= 0;
	//}} robobeg : 2008-10-27


//#ifdef PVP_BOSS_COMBAT_TEST
//	m_FrozenState = 0;
//#endif PVP_BOSS_COMBAT_TEST

	m_bFirstDataReceive			= false;
	m_bPopAgain					= false;
	m_OrgInputData.Init();

	m_SendSyncDataList.reserve( m_FrameBufferNum * 2 );
	m_ReceiveSyncDataList.reserve( m_FrameBufferNum * 2 );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_LastSendSyncDataList.reserve( m_FrameBufferNum * 2 );
    m_adwInitFrameMoveCount[0] = 0;
	m_adwInitFrameMoveCount[1] = 0;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	//���� ������Ʈ
	m_pMotionFuture				= NULL;
	m_pMotion					= NULL;
	m_pXSkinAnimFuture			= NULL;
	m_pAniXET					= NULL;

	m_pOrgMotionFuture			= NULL;
	m_pOrgMotion				= NULL;
	m_pOrgXSkinAnimFuture		= NULL;

	m_pAfterImageMotion			= NULL;
	//m_pAfterImage				= NULL;

	m_hSeqHeadMarker			= INVALID_PARTICLE_SEQUENCE_HANDLE;
    m_hSeqEmblem				= INVALID_PARTICLE_SEQUENCE_HANDLE;
    m_hHeadMarkerParticle		= INVALID_PARTICLE_HANDLE;
	m_hPart_Emblem_200			= INVALID_PARTICLE_HANDLE;

	m_hHoldEffCenter			= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hHoldEffCircle			= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_AddFrameBuffer			= 1;
	m_fFrameWaitTime			= 0.0f;
	m_FrameWaitNum				= 0;
	m_fFrameSkipTime			= 0.0f;
	m_FrameSkipNum				= 0;

	m_fRebirthTime				= 0.0f;
	m_nDamageCount				= 0;

	m_StopAdvanceTimeFuture		= false;
	m_pFrame_Bip01_Spine		= NULL;


	m_bHyperModeUsed					= false;
	m_fElaspedTimeLastGuideMessage		= 0.f;

	m_bConnect						= false;
	m_fConnectTimeOut				= 0.0f;
	m_bConnectSend					= false;

	m_bAddForceDownGageThisState	= false;
	m_fLastDamagePassTime			= 0.0f;
	m_fMaxDownForce = -9999.f;


	m_bResurrectionTimeOut			= false;
	m_fDevAddDamage					= 0.f;

	m_iNowSpecialAttack				= 0;
	m_bHoldSpecialAttack			= false;
	m_bHoldingSpecialAttack			= false;
//
//#ifndef NOT_USE_DICE_ROLL
//	m_pDiceRoll		= NULL;
//	//m_pDiceRoll		= new DiceRoll( this );
//#endif //NOT_USE_DICE_ROLL

	m_vAddJumpSpeed = D3DXVECTOR3(0, 0, 0 );
	m_bSteppedJumpLinemap = false;

	m_fAnimSpeedRate = 1.f;
	m_fAnimSpeed = 1.f;
	m_fAnimSpeedFuture = 1.f;
	m_bColdSlowApplied = false;
	m_bFrozenSlowApplied = false;


	m_fTimeToDashCameraChange			= 0.0f;
	m_fAngleDegree						= 0.0f;
	m_fTimeToReturnCameraAngle			= 0.0f;

	//{{ kimhc // 2010.06.14 // �巡���м�Ʈȿ�� ������ ���� ����
	SetOriginalMPChangeRate( 1.f );
	//}} kimhc // 2010.6.14 // �巡���м�Ʈȿ�� ������ ���� ����

	//{{ mauntain : ����ȯ [2012.07.26] Ŭ��ŷ �޸� ������ ��ٸ� ���� ���� ( �۾��� : ���ҵ� �븸 ������ )
	m_fAdditionalMPChangeRateByPassive = 0.f;
	//}}

	m_bEnterPortal = false;
	m_bLeavePortal = true;


	m_fPepperRunTime = 0.f;

	m_bRidingMachine = false;
	m_bLeaping = false;


	m_bReserveForceDownForShock	= false;

	m_ePlayedEmotion = CX2Unit::ET_NONE;
	m_bReserveSitReadyEmotion = false;

	m_fSocketSuperArmor = 0.f;
	m_bSocketSuperArmor = false;

	m_bStateSuperArmor	= false;

	m_fSuperArmorByItem = 0.f;
	m_bSuperArmorByItem = false;

	m_hEffectCrystalOfElBuff = INVALID_EFFECTSET_HANDLE;
	//10�ʴ� ��ü ü���� % ȸ�� ���� - ȸ�� ī��Ʈ �ð�
	m_fSocketHPIncreasePerSecondTime = 0.f;

	m_ViewEqipList.reserve( 64 );







//#ifdef PVP_BOSS_COMBAT_TEST
//	m_hEffectSetBossMark = INVALID_EFFECTSET_HANDLE;
//	m_iFriendlyHittedCountAtFrozenState = 0;
//#endif PVP_BOSS_COMBAT_TEST

	m_bHold = false;
	m_vHold = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_hSeqDrainParticle = INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_bNoDetonation = false;
	m_bNowStateDamageReact = false;

	m_pComboGuide = new CX2ComboGuide();

	//{{ kimhc // 2010.2.10 //	�̺� 2�� ���� �׸޽ý�
	SetHitNasodWeapon( false );	// ���ҵ� ���� ��ȭ�� �¾Ҵ°�?
	//}} kimhc // 2010.2.10 //	�̺� 2�� ���� �׸޽ý�

	//{{ oasis907 : ����� [2010.5.17] //
	m_pTextureServer = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_UI_Common_Texture11.tga" );
	//}}

	m_iDieCount = 0;

	m_bApplyAura = false;

	m_fOxygenGage = MAGIC_OXYGEN_GAGE;

	//{{ JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?
	m_bNowHit = false;
	//}} JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?

	//{{ JHKang / ������ / 2011/01/17 / ���� ������ ���� ������ ��� ����
	m_fRealDamage = 0.f;
	m_fAnimSpeedRateByNpc = 1.f;
	//}} JHKang / ������ / 2011/01/17 / ���� ������ ���� ������ ��� ����

	//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
	m_iTotalAttackedDamage	= 0;
	m_iTotalRecoveryHP		= 0;
	m_iUsingSkillCount		= 0;
	m_iTotalGivenDamamge	= 0;
	ZeroMemory( &m_iSkillSlotACount, sizeof( m_iSkillSlotACount ) );
	ZeroMemory( &m_iSkillSlotBCount, sizeof( m_iSkillSlotBCount ) );
	//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����

	m_bGrap = false;
	m_iGrapShakeCount = 0;
	m_vGrap				= D3DXVECTOR3(0.f, 0.f, 0.f);
	m_vBeforeGrapPos	= D3DXVECTOR3(0.f, 0.f, 0.f);


	m_bGrapEscapeKeyLeftCheck = false;
	m_bGrapEscapeKeyRightCheck = false;


	m_fTriggerTime = 0.f;

	for(int i=0; i < AVATAR_EMOTION_NUM; ++i)
		m_bMixedEmotion[i] = false;
	m_pAvatarEmotionSound = NULL;

	m_hHenirBuff_Nature = INVALID_EFFECTSET_HANDLE;
	m_hHenirBuff_Wind	= INVALID_EFFECTSET_HANDLE;

	m_hSpecialItemBuff_Wind	= INVALID_EFFECTSET_HANDLE;
	m_iThrowItemValue		= 0;
	m_bThrowSuccess			= false;
	m_fThrowStartTime		= 0.f;		// ���� �Ļ�Ŭ�������� �� �ʱ�ȭ ��

	m_bHideNoRenderable = false;

	m_ucDieReason = KEGS_USER_UNIT_DIE_REQ::UUDR_UNKNOWN;

	m_bParryingState = false;


	m_bQuickenGuardBlocking = false;
	m_bQuickenGuardTurning = false;

	m_bIsTransformed = false;

	m_iHoldStateID = USI_DAMAGE_AIR_FALL;

	m_fAttackCancelAfterEx	= 9999.f;
	m_fAttackCancelAfter	= 9999.f;
	m_fSkillCancelAfter		= 9999.f;
	m_fWalkCancelAfter		= 9999.f;
	m_fBWalkCancelAfter		= 9999.f;
	m_fDashCancelAfter		= 9999.f;

	m_fSkillCancelAfterFly	= 9999.f;		/// ���߿� ��ų ĵ��

	m_iLastStateHyperModeCount	= 0;
	m_bReserveHyperModeZero		= false;

	m_iHyperModeCountUsed		= 0;

	m_hSeqWhiteOut  = INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_bWhiteOut		= false;

	m_pSlashTrace = NULL;
	m_pSlashTraceTip = NULL;


	InitByGameType();

	for ( int index = 0; index < COMMON_MAJOR_PII_END; ++index )
		m_ahCommonMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	for ( int index = 0; index < COMMON_MINOR_PII_END; ++index )
		m_ahCommonMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_fSummonNpcCoolTime = 0.f;

	m_delegateVitalPointPiercing = DelegateVitalPointPiercing();

    m_hCEffectTacticalField = INVALID_DAMAGE_EFFECT_HANDLE;					/// ��Ƽ�� �ʵ� ������ ����Ʈ

	INIT_VECTOR3( m_vSaddlePos, 0, 0, 0 )
	INIT_VECTOR3( m_vDirDegree, 0, 0, 0 )

	if ( NULL != g_pMain->GetPVPEmblem() )
	{
		CX2PVPEmblem::PVPEmblemData *pPvpEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( GetUnit()->GetPvpRank() );

		if( pPvpEmblemData != NULL )
			m_pTextureRank = g_pKTDXApp->GetDeviceManager()->OpenTexture( pPvpEmblemData->m_TextureName );
	}





//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
    ResetLocalAndRelativePosAndTimers();
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

}

CX2GUUser::~CX2GUUser(void)
{

//#ifdef TRANSFORMER_TEST
//	TransformIntoUser();
//	//m_pTransformer = NULL;
//	//SAFE_DELETE_KTDGOBJECT( m_pTrasformer );
//#endif TRANSFORMER_TEST

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		g_pData->GetPetManager()->RemovePet( GetUnitUID() );
	}

	if( CX2RidingPetManager::GetInstance() != NULL )
	{
		CX2RidingPetManager::GetInstance()->RemoveRidingPet( GetUnitUID() );
	}

	m_pUnit = NULL;

	m_SendSyncDataList.clear();
	m_ReceiveSyncDataList.clear();
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_LastSendSyncDataList.clear();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
	{
		CX2EqipPtr pEqip = m_ViewEqipList[i];
		if( pEqip != NULL )
		{
			pEqip->SetManagerObject( NULL );
		}
	}
	m_ViewEqipList.clear();




	bool bDeleteOrgMotion = false;
	if( m_pMotion != m_pOrgMotion )
		bDeleteOrgMotion = true;

	SAFE_CLOSE( m_pMotionFuture );
	SAFE_CLOSE( m_pMotion );
	SAFE_CLOSE( m_pAniXET );
	SAFE_CLOSE( m_pTextureRank );

	//SAFE_DELETE( m_pXSkinAnimFuture );
	//SAFE_DELETE( m_pXSkinAnim );
	//{{ seojt // 2009-1-12, 16:44
	bool bTryDelete_m_pOrgXSkinAnimFuture = false;
	if( m_pXSkinAnimFuture != m_pOrgXSkinAnimFuture )
	{
		bTryDelete_m_pOrgXSkinAnimFuture = true;
	}//if


	SAFE_DELETE_KTDGOBJECT( m_pXSkinAnimFuture );
	SAFE_CLOSE( m_pAfterImageMotion );

	bool bTryDelete_m_pOrgXSkinAnim = false;
	if( m_pXSkinAnim != m_pOrgXSkinAnim )
	{
		bTryDelete_m_pOrgXSkinAnim = true;
	}//if

	m_pXSkinAnim.reset();

	SAFE_CLOSE( m_pOrgMotionFuture );
	if( bDeleteOrgMotion == true )
		SAFE_CLOSE( m_pOrgMotion );


	/** ���� �����͸� �ι� delete�Ͽ� crash�� ���ɼ��� ���´�.
	- jintaeks on 2009-01-12, 16:45 */
	if( bTryDelete_m_pOrgXSkinAnimFuture == true )
	{
		SAFE_DELETE_KTDGOBJECT( m_pOrgXSkinAnimFuture)
	}//if
	if( bTryDelete_m_pOrgXSkinAnim == true )
	{
		m_pOrgXSkinAnim.reset();
	}//if
	//}} seojt // 2009-1-12, 16:44


	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqHeadMarker );



//#ifdef TITLE_SYSTEM
	g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqEmblem );
//#else
//	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqEmblem );
//#endif

    m_hHeadMarkerParticle = INVALID_PARTICLE_HANDLE;
    m_hPart_Emblem_200 = INVALID_PARTICLE_HANDLE;


	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hHoldEffCenter );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hHoldEffCircle );

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hEnterCashShop )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle(m_hEnterCashShop);
	}

	DeleteEnchantedWeaponEffectAtHand();


	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqDrainParticle );

	//SAFE_DELETE_KTDGOBJECT( m_pAfterImage );
//#ifndef NOT_USE_DICE_ROLL
//	SAFE_DELETE( m_pDiceRoll );
//#endif //NOT_USE_DICE_ROLL

//#ifdef PVP_BOSS_COMBAT_TEST
//	g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectSetBossMark );
//#endif PVP_BOSS_COMBAT_TEST

	//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
	//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ

	SAFE_DELETE( m_pComboGuide );

//{{ kimhc // 2010.4.9 // ��д��� �۾�(��Ʈ������ȿ��)
	SAFE_DELETE( m_pBoneShieldData );
//}} kimhc // 2010.4.9 // ��д��� �۾�(��Ʈ������ȿ��)

	SAFE_CLOSE( m_pTextureServer );

	if( m_pAvatarEmotionSound != NULL )
	{
		m_pAvatarEmotionSound->Stop();
		SAFE_CLOSE(m_pAvatarEmotionSound);
	}

	m_StateList.clear();

	//{{ mauntain : ����ȯ [2012.06.14] ���� ī�� ��ȯ ��� - �ش� User ��Ż �� ���� ó��
	int SummonMonsterUID = GetSummonMonsterCardData().GetSummonMonsterUID();

	if( -1 != SummonMonsterUID )
	{
		CX2GUNPC* pNPCUnit = g_pX2Game->GetNPCUnitByUID( SummonMonsterUID );

		if( NULL != pNPCUnit )
		{
			pNPCUnit->SetNowHp( 0.f );
			if ( false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE0 ) &&
				false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE1 ) &&
				false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE2 ) )
				AccessSummonMonsterCardData().init();
		}
	}
	//}}


}



//#ifdef TRANSFORMER_TEST
//
//
//	void CX2GUUser::TransformIntoUser()
//	{
//		if( NULL != GetTransformer() )
//		{
//			GetTransformer()->SetNowHp( 0.f );
//		}
//
//		SetTransformed( false );
//		SetTransformer( NULL );
//
//		//SAFE_DELETE_KTDGOBJECT( m_pTrasformer );
//		StateChange( m_CommonState.m_Wait );
//		SetShowObject( true );
//
//	}
//
//	void CX2GUUser::TransformIntoMonster( CX2UnitManager::NPC_UNIT_ID eNPCID )
//	{
//		TransformIntoUser();
//
//		const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();
//
//		int iHyperMode = 0;
//		g_pX2Game->CreateNPCReq( eNPCID, iHyperMode, true, GetPos(),
//			GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_NO_BRAIN, GetUnitUID() );
//
//
//		//StateChange( m_CommonState.m_Transformed );
//	}
//
//
//#endif TRANSFORMER_TEST

/*static*/
void     CX2GUUser::InitInit( InitData& OutInit_, KLuaManager& luaManager_ )
{
	CX2GameUnit::InitDevice( OutInit_.m_device, luaManager_ );
	InitMotion( OutInit_, luaManager_ );
}//CX2GUUser::InitInit()


void CX2GUUser::Init( bool bUseTeamPos, int startPosIndex )
{
//#ifndef NOT_USE_DICE_ROLL
//	if( m_pDiceRoll == NULL )
//		m_pDiceRoll		= new DiceRoll( this );
//#endif //NOT_USE_DICE_ROLL

	InitSystem();
	InitDevice();
	InitMotion();


	InitStat();

	{
		InitSocketData();
		InitEnchantData();
	}

	InitPhysic();
	InitEqip();
	InitComponent();

	ParseCommonRandomState();
	InitEffect();
	InitAdditionalBasicStatValue();	/// InitPassiveSikllState() �� ���� �������� �־�� ��
	InitState();

	// FieldFix: �ӽ÷� �̷��� ó��...
	if ( NULL != g_pX2Game )
	{
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_BATTLE_FIELD:
			{
				if ( IsMyUnit() && false == g_pData->GetBattleFieldManager().GetNowBattleFieldPositionInfoStartPosition() )
					InitPosByBattleFieldPosition();
				else
					InitPosition( bUseTeamPos, startPosIndex );
			}
			break;

		default:
			InitPosition( bUseTeamPos, startPosIndex );
			break;
		}
	}
	InitEtc();

// 	CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
// 	if( pPet != NULL )
// 		pPet->InitPos();

	SetEnableAttackBox( L"Rfoot", false );
	SetEnableAttackBox( L"Lfoot", false );
	SetEnableAttackBox( L"Lhand2", false );

	if ( NULL != GetGageData() )
		m_FrameDataFuture.syncData.m_HyperModeCount = GetGageData()->GetHyperModeCount();

	m_bPositionBackup = false;
	m_FrameDataNow		= m_FrameDataFuture;

	SetCenter( m_pMotion->GetCenter() );
	SetBoundingRadius( m_pMotion->GetBoundingRadius() );

	// FieldFix: �ӽ÷� �̷��� ó��...
	if ( NULL != g_pX2Game )
	{
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_BATTLE_FIELD:
			StateChange( USI_WAIT );
			break;

		case CX2Game::GT_PVP:
			StateChange( USI_START );
			break;

		default:
			{
				if ( NULL != GetUnit() && NULL != GetUnit()->GetRidingPetInfo() )
					StateChange( GetRidingStartStateID() );
				else
					StateChange( USI_START );
			} break;
		}

		if ( CX2Game::GT_PVP != g_pX2Game->GetGameType() )
		{
			if ( NULL != GetUnit() )
			{
				const KRidingPetInfo* pRidingPetInfo
					= GetUnit()->GetRidingPetInfo();
				if ( NULL != pRidingPetInfo )
					ProcessRidingOn( true, *pRidingPetInfo );
			}
		}
	}

	m_bWaitingForRebirthAfterBungee = false;

	m_bEnterPortal = false;
	m_bLeavePortal = true;

	m_bDisableGravity = false;

	ClearEffectiveBlackHoleDamageEffect();

	//{{ kimhc // 2009-12-29 // ��彺ų ��������� ���� �κ� �ʱ�ȭ
	m_bHaveSacrificeOfHero		= false;
	m_RenderParam.renderType	= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
	//m_GuildSkillData.Init();
	//}} kimhc // 2009-12-29 // ��彺ų ��������� ���� �κ� �ʱ�ȭ

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		if( GetUnit()->GetPetInfo() != NULL )
		{
			KPetInfo *pPetInfo = GetUnit()->GetPetInfo();
			CX2PET::PetInfo petInfo;

			petInfo.m_PetUid = pPetInfo->m_iPetUID;
			petInfo.m_PetId = pPetInfo->m_iPetID;
			petInfo.m_Evolution_Step = pPetInfo->m_cEvolutionStep;
			petInfo.m_wstrPetName = pPetInfo->m_wstrPetName;
			petInfo.m_Intimacy = (float)pPetInfo->m_iIntimacy;
			petInfo.m_Satiety = pPetInfo->m_sSatiety;
			petInfo.m_Extroversion = pPetInfo->m_sExtroversion;
			petInfo.m_Emotion = pPetInfo->m_sEmotion;
			petInfo.m_bIsDropItemPickup = pPetInfo->m_bAutoLooting;
			petInfo.m_bSummon = true;
			g_pData->GetPetManager()->CreateGamePet( GetUnitUID(), petInfo );
		}
		else
		{
			g_pData->GetPetManager()->RemovePet( GetUnitUID() );
		}
	}

	//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
	//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ


}

void CX2GUUser::ReInit( bool bUseTeamPos, int startPosIndex )
{
	int hyperModeCount = GetHyperModeCount();
	m_bDownForce = false;

	CX2GameUnit::ReInit( bUseTeamPos, startPosIndex );
    m_vecAttackTime.resize( 0 );
	m_StopAllList.resize(0);
	m_StopOtherList.resize(0);
	m_StopMyList.resize(0);
	m_Stop2AllList.resize(0);
	m_Stop2OtherList.resize(0);
	m_Stop2MyList.resize(0);
    m_vecSoundPlayData.resize( 0 );

	//{{ kimhc // 2009-12-29 // ��彺ų ��������� ���� �κ� �ʱ�ȭ
	m_bHaveSacrificeOfHero		= false;
	m_RenderParam.renderType	= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
	m_GuildSkillData.Init();
	//}} kimhc // 2009-12-29 // ��彺ų ��������� ���� �κ� �ʱ�ȭ

	m_bFirstDataReceive			= false;
	m_bPopAgain					= false;
	m_fRebirthTime				= 0.0f;

	m_fExtraBigHead				= 0.f;

	ClearRecieveSyncData();
	m_SendSyncDataList.reserve( m_FrameBufferNum * 2 );
	m_ReceiveSyncDataList.reserve( m_FrameBufferNum * 2 );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_LastSendSyncDataList.reserve( m_FrameBufferNum * 2 );
    m_adwInitFrameMoveCount[0] = 0;
	m_adwInitFrameMoveCount[1] = 0;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	m_FutureStateData.Init();
	m_NowStateData.Init();


	//m_FrameDataFutureBefore.Init();
	m_FrameDataNowBefore.Init();

	m_BeforeInputData.Init();
	m_InputData.Init();

    ClearAnimEventTimerOneshotFuture();
    ClearAnimEventTimerOneshotNow();

	m_DamagedMap.clear();

	m_bPositionBackup = false;

    m_vecNowSuperArmorTime.resize(0);
	m_bFutureCanPassUnit = false;
    m_bFutureApplyAnimMove = true;
    m_bNowFallDown = true;
    m_fNowSlashTraceTipWide = 4.0f;
    m_mapNowAddSlashTrace.clear();
    m_v2NowAfterImageTime = D3DXVECTOR2( -1.f, -1.f );



	//�Ʊ��� ������ �Ʊ� ��ġ���� ��Ȱ
	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_DUNGEON:
		{
            m_FrameDataFuture.Init();
            m_FrameDataNow.Init();

			bool bInitPos = false;
			bool bIsExistMyTeam = false;
			for( int i= 0; i < g_pX2Game->GetUserUnitNum(); i++ )
			{
				CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnit( i );
				if( pCX2GUUser != NULL && pCX2GUUser->GetTeam() == GetTeam() && NULL != g_pX2Game->GetMyUnit() )
				{
					if ( pCX2GUUser != g_pX2Game->GetMyUnit() )
						bIsExistMyTeam = true;

					if ( pCX2GUUser != g_pX2Game->GetMyUnit() &&
						pCX2GUUser->GetGameUnitState() != CX2GUUser::GUSI_DIE &&
						pCX2GUUser->GetNowHp() > 0.0f )
					{
						// �츮���� �ְ� �Ѹ��̶� ����ִٸ� ���� ��ġ���� ��Ȱ
						bInitPos = true;
						InitPosition( pCX2GUUser );
					}
				}
			}

			if ( bInitPos == false )
			{

				//{{ kimhc // 2010.8.10 // Ư�� ������ ��ġ ���� ��Ȱ ��ġ�� ���
				if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
				{
					CX2DungeonGame*		pDungeonGame	= static_cast<CX2DungeonGame*>(g_pX2Game);
					if( NULL != pDungeonGame )
					{
						CX2DungeonSubStage*	pSubStage		= pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStage();

						// �ַ� �÷����� ��쿡�� ���� ��ġ���� ��Ȱ
						if ( bIsExistMyTeam == false
							&& m_DeadStageIndex == pDungeonGame->GetDungeon()->GetNowStageIndex()
							&& m_DeadSubStageIndex == pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStageIndex() )
						{
							InitPosByDeadPos( g_pX2Game->GetMyUnit() );
						}
						// �ַ� �÷��̿��� ���� ���� stage�� �ٸ��ų�, �츮���� �ֱ�� �ִµ� �� ���� ��Ȳ�̶�� �ش� ���꽺�������� ���� ��ġ���� ��Ȱ
						else
						{
							//{{ kimhc // 2011-04-29 // ���潺 ���� ��Ȱ ��ġ ����
							if ( true == g_pData->GetDungeonManager()->IsDefenceDungeon( pDungeonGame->GetDungeon()->GetDungeonData()->m_DungeonID ) )
							{
								InitPosByDeadPos( g_pX2Game->GetMyUnit() );	// ���� �ڸ����� ��Ȱ
							}
							else
								//}} kimhc // 2011-04-29 // ���潺 ���� ��Ȱ ��ġ ����
							{
								// ��ϸ� �ð��� ��� pSubStage�� NULL ��
								bool bSuccessRebirth = true;

								if ( pSubStage != NULL && pSubStage->GetSubStageData()->GetUsePosOfMonsterForRebirth() == true )
									bSuccessRebirth = InitPosByMonsterPos();

								if ( bSuccessRebirth == false || pSubStage == NULL || pSubStage->GetSubStageData()->GetUsePosOfMonsterForRebirth() == false )
									InitPosition( bUseTeamPos, startPosIndex );
							}
						}
				//}} kimhc // 2010.8.10 // Ư�� ������ ��ġ ���� ��Ȱ ��ġ�� ���
					}
				}
			}
			//{{ kimhc // 2011-04-29 // ���潺 ���� ��Ȱ ��ġ ����
			else
			{
				if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
				{
					CX2DungeonGame*		pDungeonGame	= static_cast<CX2DungeonGame*>(g_pX2Game);
					if ( NULL != pDungeonGame &&
						NULL != pDungeonGame->GetDungeon() &&
						NULL != pDungeonGame->GetDungeon()->GetDungeonData() &&
						true == g_pData->GetDungeonManager()->IsDefenceDungeon( pDungeonGame->GetDungeon()->GetDungeonData()->m_DungeonID ) )
					{
						InitPosByDeadPos( g_pX2Game->GetMyUnit() );	// ���� �ڸ����� ��Ȱ
					}
				}
			}
			//}} kimhc // 2011-04-29 // ���潺 ���� ��Ȱ ��ġ ����
		} break;

	case CX2Game::GT_BATTLE_FIELD:
		{
			m_FrameDataFuture.Init();
			m_FrameDataFuture.syncData.position				= m_FrameDataNow.syncData.position;
			m_FrameDataFuture.unitCondition.dirDegree		= m_FrameDataNow.unitCondition.dirDegree;
			m_FrameDataFuture.unitCondition.landPosition	= m_FrameDataNow.unitCondition.landPosition;
			m_FrameDataFuture.syncData.lastTouchLineIndex	= m_FrameDataNow.syncData.lastTouchLineIndex;
			m_FrameDataFuture.syncData.bIsRight				= m_FrameDataNow.syncData.bIsRight;

			m_FrameDataNow.Init();
			m_FrameDataNow.syncData.position				= m_FrameDataFuture.syncData.position;
			m_FrameDataNow.unitCondition.dirDegree			= m_FrameDataFuture.unitCondition.dirDegree;
			m_FrameDataNow.unitCondition.landPosition		= m_FrameDataFuture.unitCondition.landPosition;
			m_FrameDataNow.syncData.lastTouchLineIndex		= m_FrameDataFuture.syncData.lastTouchLineIndex;
			m_FrameDataNow.syncData.bIsRight				= m_FrameDataFuture.syncData.bIsRight;

		} break;

	default:
		{
			m_FrameDataFuture.Init();
			m_FrameDataNow.Init();

			InitPosition( bUseTeamPos, startPosIndex );

		} break;

	}

	m_FrameDataDead.Init();


	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_FrameDataFuture.unitCondition.fUnitWidth, 35.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_FrameDataFuture.unitCondition.fUnitHeight, 150.f );
		m_FrameDataNow.unitCondition.fUnitWidth = m_FrameDataFuture.unitCondition.fUnitWidth;
		m_FrameDataNow.unitCondition.fUnitHeight = m_FrameDataFuture.unitCondition.fUnitHeight;


		m_LuaManager.EndTable();
	}

	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		ResetMaxHP();
		m_LuaManager.GetValue( "MP_CHANGE_RATE",		m_fOriginalMPChangeRate );

		//{{ kimhc // 2010.06.04 // �巡���м�Ʈȿ�� ������ ���� ����
		SetManaRecoveryTime( 0.0f );
		SetManaRecoveryVal( 0.0f );
		ResetMPChangeRate( GetOriginalMPChangeRate() );


		//}} kimhc // 2010.06.04 // �巡���м�Ʈȿ�� ������ ���� ����

		float fData = 0.f;
		m_LuaManager.GetValue( "MP_CHARGE_RATE",		fData );
		SetChangeRateChargeMpForDetonation( fData );


		SetNowHp( GetMaxHp() );
		ResetMaxMP();
		SetMaxSoul( 100.f );


		if ( CX2Room::TN_RED == GetTeam() || CX2Room::TN_BLUE == GetTeam() )
		{
			int iDiffKillNum = 0;

			if ( CX2Room::TN_RED == GetTeam() )
				iDiffKillNum = g_pX2Game->GetRedTeamKill() - g_pX2Game->GetBlueTeamKill();
			else if ( CX2Room::TN_BLUE == GetTeam() )
				iDiffKillNum = g_pX2Game->GetBlueTeamKill() - g_pX2Game->GetRedTeamKill();

			if ( 1 < iDiffKillNum )
				SetNowMp( 0.f );
			else if ( iDiffKillNum == 1 )
				SetNowMp( GetNowMp() * 0.5f );
		}

		//�������� ��Ȱ�� ��� �� MP �ִ�ġ�� ȸ�� ���� �ֵ��� ����
		if( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD )
		{
			SetNowMp( GetMaxMp() );
		}
		GetGageData()->SetEnable( true );

		m_FrameDataFuture.syncData.fNowHP				= GetNowHp();
		m_FrameDataFuture.syncData.fNowMP				= GetNowMp();

		if ( m_pAfterImage != NULL )
		{
			m_pAfterImage->SetAfterImageColor( 0xffcccccc );
		}

		//{{ kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����
		for( UINT i=0; i<m_vecpWeapon.size(); i++ )
		{
			Weapon* pWeapon = m_vecpWeapon[i];
			if( pWeapon != NULL )
			{
				pWeapon->LoadDataForUser( m_LuaManager );
			}
		}

		InitEnchantedWeaponEffectAtHand();
		InitEnchantWeaponEffectUnitSpecific();

		//SetShowEnchantWeaponParticle( true );
		//}} kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����


		m_LuaManager.EndTable();
	}

//
// 	CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRFoot );
// 	CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLFoot );
// 	CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRArm );
// 	CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLArm );
//
// 	if( pSeq_RFoot != NULL )
// 	{
// 		pSeq_RFoot->SetPosition( GetBonePos( L"Bip01_R_Foot" ) );
// 		pSeq_RFoot->SetEmitRate( 0.f, 0.f );
// 	}
// 	if( pSeq_LFoot != NULL )
// 	{
// 		pSeq_LFoot->SetPosition( GetBonePos( L"Bip01_L_Foot" ) );
// 		pSeq_LFoot->SetEmitRate( 0.f, 0.f );
// 	}
// 	if( pSeq_RArm != NULL )
// 	{
// 		pSeq_RArm->SetPosition( GetBonePos( L"Bip01_R_UpperArm" ) );
// 		pSeq_RArm->SetEmitRate( 0.f, 0.f );
// 	}
// 	if( pSeq_LArm != NULL )
// 	{
// 		pSeq_LArm->SetPosition( GetBonePos( L"Bip01_L_UpperArm" ) );
// 		pSeq_LArm->SetEmitRate( 0.f, 0.f );
// 	}
//
//
// 	SetRemainHyperModeTime( 0.0f );


	m_pXSkinAnimFuture->SetPlaySpeed( 1.0f );
	m_pXSkinAnim->SetPlaySpeed( 1.0f );



	m_bAddForceDownGageThisState	= false;
	m_fLastDamagePassTime			= 0.0f;
	m_fMaxDownForce = -9999.f;

	for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
	{
		CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
		pCX2Eqip->ChangeNormalMode();
	}



	SetEnableAttackBox( L"Rfoot", false );
	SetEnableAttackBox( L"Lfoot", false );
	SetEnableAttackBox( L"Lhand2", false );

	SetHyperModeCount( hyperModeCount );

	m_bWaitingForRebirthAfterBungee = false;

	m_bEnterPortal = false;
	m_bLeavePortal = true;


	m_bDisableGravity = false;
	ClearEffectiveBlackHoleDamageEffect();

	SetEnableDash( true );
	SetEntangled( false );

	if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f &&
		m_ExtraDamagePack.m_Frozen.m_fTime <= 0.f )
	{
		g_pX2Game->SetEnableAllKeyProcess( true );
	}

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		if( GetUnit()->GetPetInfo() != NULL )
		{
			KPetInfo *pPetInfo = GetUnit()->GetPetInfo();
			CX2PET::PetInfo petInfo;

			petInfo.m_PetUid = pPetInfo->m_iPetUID;
			petInfo.m_PetId = pPetInfo->m_iPetID;
			petInfo.m_Evolution_Step = pPetInfo->m_cEvolutionStep;
			petInfo.m_wstrPetName = pPetInfo->m_wstrPetName;
			petInfo.m_Intimacy = (float)pPetInfo->m_iIntimacy;
			petInfo.m_Satiety = pPetInfo->m_sSatiety;
			petInfo.m_Extroversion = pPetInfo->m_sExtroversion;
			petInfo.m_Emotion = pPetInfo->m_sEmotion;
			petInfo.m_bIsDropItemPickup = pPetInfo->m_bAutoLooting;
			petInfo.m_bSummon = true;
			g_pData->GetPetManager()->CreateGamePet( GetUnitUID(), petInfo );
		}
		else
		{
			g_pData->GetPetManager()->RemovePet( GetUnitUID() );
		}

		ClearMPFactor();
	}

	m_fOxygenGage = MAGIC_OXYGEN_GAGE;

	//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
	//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ


	EndHenirBuff();


	EndSpecialItemBuff();

	m_fSummonNpcCoolTime = 0.f;


	 /// ��Ȱ��, �������� �� ������ ������ �ش�.
	 if ( NULL != GetUnit() )
	 {
		 const vector<BUFF_FACTOR_ID>& vecWorldBuffFactorID = GetUnit()->GetWorldBuffFactorID();

		 BOOST_FOREACH( BUFF_FACTOR_ID eWorldBuffFactorID, vecWorldBuffFactorID )
		 {
			 SetBuffFactorToGameUnitByBuffFactorID( eWorldBuffFactorID );
		 }
	 }

	 m_bSetCustomRenderParam = false;

	 PlaySoundRevive();

}

/*static*/
void	CX2GUUser::AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pX2Unit_ )
{
	if ( pX2Unit_ == NULL )
		return;

	switch( pX2Unit_->GetType() )
	{
	case CX2Unit::UT_NONE:
		break;
	case CX2Unit::UT_ELSWORD:
		CX2GUElsword_SwordMan::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_ARME:
		CX2GUArme_VioletMage::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_LIRE:
		CX2GULire_ElvenRanger::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_RAVEN:
		CX2GURaven::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_EVE:
		CX2GUEve::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_CHUNG:
		CX2GUChung::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_ARA:
		CX2GUAra::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_ELESIS:
		CX2GUEL::AppendDeviceToDeviceList( listInOut_ );
		break;
	case CX2Unit::UT_ADD:
		CX2GUAdd::AppendDeviceToDeviceList( listInOut_ );
		break;
	}
}



void CX2GUUser::InitDevice()
{
	CX2GameUnit::InitDevice();
}

void CX2GUUser::InitSystem()
{
#if X2_DEV_TOOLS
	SetScriptFileName();
#endif //REALTIME_SCRIPT_PATCH
	CX2GameUnit::InitSystem();

	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_FrameDataFuture.unitCondition.fUnitWidth, 75.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_FrameDataFuture.unitCondition.fUnitHeight, 150.f );

		m_vOrgUnitSize.x = m_FrameDataFuture.unitCondition.fUnitWidth;
		m_vOrgUnitSize.y = m_FrameDataFuture.unitCondition.fUnitHeight;

		m_FrameDataNow.unitCondition.fUnitWidth = m_FrameDataFuture.unitCondition.fUnitWidth;
		m_FrameDataNow.unitCondition.fUnitHeight = m_FrameDataFuture.unitCondition.fUnitHeight;



		// fix!! �ϴ� ���⿡ �ΰ� ���߿� ���̺��� ���� ������� ����
		if( true == m_LuaManager.BeginTable( "DUNGEON_SPEECH" ) )
		{
			int iStringIndex = -1;
			int iValueIndex = 1;
			while( true )
			{
				LUA_GET_VALUE_RETURN( m_LuaManager,	iValueIndex, iStringIndex, STR_ID_EMPTY, break; );
				m_vecDungeonWinSpeech.push_back( GET_STRING( iStringIndex ) );
				iValueIndex++;
			}

			m_LuaManager.EndTable(); // DUNGEON_SPEECH
			iValueIndex++;
		}


		m_FrameDataFuture.syncData.nowState = GUSI_LOADING;

		if ( /*IsMyUnit() && */m_LuaManager.BeginTable( "CHAIN_SYSTEM" ) )
		{
			if ( !m_ComboAndSkillChainSystem.CreateComboAndSkillChain( m_LuaManager ) )
			{
				DISPLAY_ERROR( L"ComboAndSkillChainSystem Parsing Error" );
			}
			m_LuaManager.EndTable();	/// CHAIN_SYSTEM
		}

		m_LuaManager.EndTable();
	}
}

void    CX2GUUser::InitData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pX2Unit_ ) const
{
	m_device.AppendToDeviceList( listInOut_ );

	CX2GUUser::AppendDeviceToDeviceList( listInOut_, pX2Unit_ );
	CX2GUUser::AppendMotionToDeviceList( listInOut_, pX2Unit_ );
	CX2GUUser::AppendComponentToDeviceList( listInOut_, pX2Unit_ );
	CX2GUUser::AppendEffectToDeviceList( listInOut_, pX2Unit_ );

}//CX2GUUser::InitData::AppendToDeviceList()

void CX2GUUser::InitPosition( bool bUseTeamPos, int startPosIndex )
{

    m_bPositionBackup = false;

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
    ResetLocalAndRelativePosAndTimers();
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

	KTDXPROFILE();
	if( g_pX2Game->GetWorld() == NULL )
	{
		//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.
		m_FrameDataFuture.syncData.position				= D3DXVECTOR3(0,0,0);
		m_FrameDataFuture.unitCondition.dirDegree		= D3DXVECTOR3(0,0,0);
		m_FrameDataFuture.unitCondition.landPosition	= D3DXVECTOR3(0,0,0);
		m_FrameDataFuture.syncData.lastTouchLineIndex	= 0;
		m_FrameDataFuture.syncData.bIsRight				= true;
		m_FrameDataFuture.unitCondition.dirDegree		= D3DXVECTOR3(0,0,0);

		m_FrameDataNow.syncData.position				= m_FrameDataFuture.syncData.position;
		m_FrameDataNow.unitCondition.dirDegree			= m_FrameDataFuture.unitCondition.dirDegree;
		m_FrameDataNow.unitCondition.landPosition		= m_FrameDataFuture.unitCondition.landPosition;
		m_FrameDataNow.syncData.lastTouchLineIndex		= m_FrameDataFuture.syncData.lastTouchLineIndex;
		m_FrameDataNow.syncData.bIsRight				= m_FrameDataFuture.syncData.bIsRight;
		m_FrameDataNow.unitCondition.dirDegree			= m_FrameDataFuture.unitCondition.dirDegree;
		return;
	}
	CKTDGLineMap*	pLineMap = g_pX2Game->GetWorld()->GetLineMap();

	if( NULL == pLineMap )
		return;

	D3DXVECTOR3		startPos;
	int lineIndex = -1;

	if ( bUseTeamPos == true )
	{
		if ( startPosIndex == -1 )	//���� ��𿡼� �¾�� �������� �������
		{
			if( m_TeamNum == CX2Room::TN_RED )
			{
				startPos								= pLineMap->GetRedTeamStartPosition( m_UnitIndex );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetRedTeamStartRight( m_UnitIndex );
				lineIndex								= pLineMap->GetRedTeamStartLineIndex( m_UnitIndex );
			}
			else if( m_TeamNum == CX2Room::TN_BLUE )
			{
				int index = m_UnitIndex - 4;
				if( index < 0 )
					index = 0;

				startPos								= pLineMap->GetBlueTeamStartPosition( index );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetBlueTeamStartRight( index );
				lineIndex								= pLineMap->GetBlueTeamStartLineIndex( index );
			}
			else
			{
				startPos								= pLineMap->GetStartPosition( m_UnitIndex );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetStartRight( m_UnitIndex );
				lineIndex								= pLineMap->GetStartLineIndex( m_UnitIndex );
			}
		}
		else
		{
			if( m_TeamNum == CX2Room::TN_RED )
			{
				startPos								= pLineMap->GetRedTeamStartPosition( startPosIndex );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetRedTeamStartRight( startPosIndex );
				lineIndex								= pLineMap->GetRedTeamStartLineIndex( startPosIndex );
			}
			else if( m_TeamNum == CX2Room::TN_BLUE )
			{
				startPos								= pLineMap->GetBlueTeamStartPosition( startPosIndex );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetBlueTeamStartRight( startPosIndex );
				lineIndex								= pLineMap->GetBlueTeamStartLineIndex( startPosIndex );
			}
			else
			{
				startPos								= pLineMap->GetStartPosition( startPosIndex );
				m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetStartRight( startPosIndex );
				lineIndex								= pLineMap->GetStartLineIndex( startPosIndex );
			}
		}
	}
	else
	{
		if ( startPosIndex != -1 )
		{
			startPos								= pLineMap->GetStartPosition( startPosIndex );
			m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetStartRight( startPosIndex );
			lineIndex								= pLineMap->GetStartLineIndex( startPosIndex );

			const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData(lineIndex);
			if( NULL != pLineData )
			{
				if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
				{
					// ��Ȱ��ġ �� ����
					for(int lineInx = 0; lineInx < pLineMap->GetNumLineData(); ++lineInx)
					{
						if(pLineMap->GetLineData(lineInx) == NULL)
							continue;

						if(pLineMap->GetLineData(lineInx)->lineType == CKTDGLineMap::LT_NORMAL && pLineMap->GetLineData(lineInx)->bEnable == true)
						{
							startPos = (pLineMap->GetLineData(lineInx)->startPos + pLineMap->GetLineData(lineInx)->endPos) * 0.5f;
							lineIndex = lineInx;
							break;
						}
					}
				}
			}
		}
		else
		{
            int index = pLineMap->GetRandomStartPosIndex();

			startPos								= pLineMap->GetStartPosition( index );
			m_FrameDataFuture.syncData.bIsRight		= pLineMap->GetStartRight( index );
			lineIndex								= pLineMap->GetStartLineIndex( index );


			const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData(lineIndex);
			if( NULL != pLineData )
			{
				if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
				{
					// ��Ȱ��ġ �� ����
					for(int lineInx = 0; lineInx < pLineMap->GetNumLineData(); ++lineInx)
					{
						if(pLineMap->GetLineData(lineInx) == NULL)
							continue;

						if(pLineMap->GetLineData(lineInx)->lineType == CKTDGLineMap::LT_NORMAL && pLineMap->GetLineData(lineInx)->bEnable == true)
						{
							startPos = (pLineMap->GetLineData(lineInx)->startPos + pLineMap->GetLineData(lineInx)->endPos) * 0.5f;
							lineIndex = lineInx;
							break;
						}
					}
				}
			}
		}
	}



	//��Ȯ�� ���� ������ġ�� ���� �ε����� ���Ѵ�
	D3DXVECTOR3	landPos = startPos;
	if( lineIndex < 0 || lineIndex >= pLineMap->GetNumLineData() )
	{
		pLineMap->GetLandPosition( startPos, LINE_RADIUS, &lineIndex );
		pLineMap->IsOnLineConnect( startPos, &lineIndex, LINE_RADIUS, &landPos );
	}

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( lineIndex );
	startPos = landPos;
	D3DXVECTOR3 dirDegree(1,0,0);
	if( NULL != pLineData )
	{
		dirDegree = pLineData->dirDegree;
	}


	//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.
	m_FrameDataFuture.syncData.position					= startPos;
	m_FrameDataFuture.unitCondition.dirDegree			= dirDegree;
	m_FrameDataFuture.unitCondition.landPosition		= startPos;
	m_FrameDataFuture.syncData.lastTouchLineIndex		= lineIndex;
	if( m_FrameDataFuture.syncData.bIsRight == false )
		m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;

	m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
	m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
	m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
	m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;
	m_FrameDataNow.syncData.bIsRight					= m_FrameDataFuture.syncData.bIsRight;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    DWORD   dwFrameMoveCount = 0;
    if ( m_ReceiveSyncDataList.empty() == false )
        dwFrameMoveCount = m_ReceiveSyncDataList.back().dwFrameMoveCount;
    else
        dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	while( m_FrameBufferNum > (int)m_ReceiveSyncDataList.size() )
	{
		m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        dwFrameMoveCount++;
        m_ReceiveSyncDataList.back().dwFrameMoveCount = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_adwInitFrameMoveCount[0] = dwFrameMoveCount - m_FrameBufferNum + 1;
	m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
	if( pPet != NULL )
		pPet->InitPos();

	m_vecNeverMovePtr.clear();
}


void CX2GUUser::InitPosition( CX2GUUser* pCX2GUUser )
{
    m_bPositionBackup = false;

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
    ResetLocalAndRelativePosAndTimers();
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

	if( pCX2GUUser != NULL )
	{
		//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.
		m_FrameDataFuture.syncData.position					= pCX2GUUser->GetFrameData().unitCondition.landPosition;
		m_FrameDataFuture.unitCondition.dirDegree			= pCX2GUUser->GetFrameData().unitCondition.dirDegree;
		m_FrameDataFuture.unitCondition.landPosition		= pCX2GUUser->GetFrameData().unitCondition.landPosition;
		m_FrameDataFuture.syncData.lastTouchLineIndex		= pCX2GUUser->GetFrameData().syncData.lastTouchLineIndex;
		if( m_FrameDataFuture.syncData.bIsRight == false )
			m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;

		m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
		m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
		m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
		m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;
		m_FrameDataNow.syncData.bIsRight					= m_FrameDataFuture.syncData.bIsRight;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    DWORD   dwFrameMoveCount = 0;
    if ( m_ReceiveSyncDataList.empty() == false )
        dwFrameMoveCount = m_ReceiveSyncDataList.back().dwFrameMoveCount;
    else
        dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    while( m_FrameBufferNum > (int)m_ReceiveSyncDataList.size() )
	    {
		    m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            dwFrameMoveCount++;
            m_ReceiveSyncDataList.back().dwFrameMoveCount = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    }

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		m_adwInitFrameMoveCount[0] = dwFrameMoveCount - m_FrameBufferNum + 1;
		m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		m_vecNeverMovePtr.clear();
	}
}

void CX2GUUser::InitPosByDeadPos( CX2GUUser* pCX2GUUser )
{
	if( pCX2GUUser != NULL )
	{
		//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.
		m_FrameDataFuture.syncData.position					= pCX2GUUser->GetFrameDataDead().unitCondition.landPosition;
		m_FrameDataFuture.unitCondition.dirDegree			= pCX2GUUser->GetFrameDataDead().unitCondition.dirDegree;
		m_FrameDataFuture.unitCondition.landPosition		= pCX2GUUser->GetFrameDataDead().unitCondition.landPosition;
		m_FrameDataFuture.syncData.lastTouchLineIndex		= pCX2GUUser->GetFrameDataDead().syncData.lastTouchLineIndex;
		if( m_FrameDataFuture.syncData.bIsRight == false )
			m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;

		m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
		m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
		m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
		m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;
		m_FrameDataNow.syncData.bIsRight					= m_FrameDataFuture.syncData.bIsRight;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    DWORD   dwFrameMoveCount = 0;
    if ( m_ReceiveSyncDataList.empty() == false )
        dwFrameMoveCount = m_ReceiveSyncDataList.back().dwFrameMoveCount;
    else
        dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    while( m_FrameBufferNum > (int)m_ReceiveSyncDataList.size() )
	    {
		    m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            dwFrameMoveCount++;
            m_ReceiveSyncDataList.back().dwFrameMoveCount = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    }

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		m_adwInitFrameMoveCount[0] = dwFrameMoveCount - m_FrameBufferNum + 1;
		m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	}
}


void CX2GUUser::InitPosByLineData( int iLineIndex )
{
	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetLineMap() )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( iLineIndex );
		if( NULL == pLineData )
			return; // fix!! ������~ plinedata should not be null

		D3DXVECTOR3 vLandPos = ( pLineData->startPos + pLineData->endPos ) * 0.5f;
		D3DXVECTOR3 vPos = vLandPos + D3DXVECTOR3( 0, 200, 0 );

		vPos = pLineData->startPos + ( (pLineData->endPos - pLineData->startPos) * 0.5f ); // fix InitPos



		//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.
		m_FrameDataFuture.syncData.position					= vPos;
		m_FrameDataFuture.unitCondition.dirDegree			= pLineData->dirDegree;
		m_FrameDataFuture.unitCondition.landPosition		= vLandPos;
		m_FrameDataFuture.syncData.lastTouchLineIndex		= iLineIndex;

		m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
		m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
		m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
		m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    DWORD   dwFrameMoveCount = 0;
    if ( m_ReceiveSyncDataList.empty() == false )
        dwFrameMoveCount = m_ReceiveSyncDataList.back().dwFrameMoveCount;
    else
        dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    while( m_FrameBufferNum > (int)m_ReceiveSyncDataList.size() )
	    {
		    m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            dwFrameMoveCount++;
            m_ReceiveSyncDataList.back().dwFrameMoveCount = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	    }

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		m_adwInitFrameMoveCount[0] = dwFrameMoveCount - m_FrameBufferNum + 1;
		m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	}
}

void CX2GUUser::InitPosByBattleFieldPosition()
{
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetLineMap() )
	{
		CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();

		// StartPosition �̸� ���� ����
		if ( true == battleFieldManager.GetNowBattleFieldPositionInfoStartPosition() )
		{
			ASSERT( !"GetNowBattleFieldPositionInfo is wrong" );
			StateLog( "GetNowBattleFieldPositionInfo is wrong" );
			return;
		}
		else
		{
			const CKTDGLineMap::LineData* pLineData
				= g_pX2Game->GetLineMap()->GetLineData( battleFieldManager.GetBattleFieldPositionIndexWhereIShouldBe() );

			if ( NULL != pLineData )
			{
				const float fPositionValue = battleFieldManager.GetBattleFieldPositionValue() * 0.01f;
				const D3DXVECTOR3 vDistanceVector3( pLineData->endPos - pLineData->startPos );

				m_FrameDataFuture.syncData.position					= pLineData->startPos + ( vDistanceVector3 * fPositionValue );
				m_FrameDataFuture.unitCondition.dirDegree			= pLineData->dirDegree;
				m_FrameDataFuture.unitCondition.landPosition		= m_FrameDataFuture.syncData.position;
				m_FrameDataFuture.syncData.lastTouchLineIndex		=  static_cast<UCHAR>( battleFieldManager.GetBattleFieldPositionIndexWhereIShouldBe() );
				m_FrameDataFuture.syncData.bIsRight					= true;

				m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
				m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
				m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
				m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;
				m_FrameDataNow.syncData.bIsRight					= m_FrameDataFuture.syncData.bIsRight;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                DWORD   dwFrameMoveCount = 0;
                if ( m_ReceiveSyncDataList.empty() == false )
                    dwFrameMoveCount = m_ReceiveSyncDataList.back().dwFrameMoveCount;
                else
                    dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	            while( m_FrameBufferNum > (int)m_ReceiveSyncDataList.size() )
	            {
		            m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    dwFrameMoveCount++;
                    m_ReceiveSyncDataList.back().dwFrameMoveCount = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	            }

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
				m_adwInitFrameMoveCount[0] = dwFrameMoveCount - m_FrameBufferNum + 1;
				m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

				CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
				if( pPet != NULL )
					pPet->InitPos();
			}
			else
			{
				ASSERT( !"GetNowBattleFieldPostionIndex is wrong" );
				StateLog( "GetNowBattleFieldPostionIndex is wrong" );
				return;
			}
		}
	}
}

//{{ kimhc // 2010.8.10 // Ư�� ������ ��ġ ���� ��Ȱ ��ġ�� ���
bool CX2GUUser::InitPosByMonsterPos()
{
	if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		CX2DungeonGame*		pDungeonGame	= static_cast<CX2DungeonGame*>(g_pX2Game);
		if( NULL != pDungeonGame &&
			NULL != pDungeonGame->GetDungeon() &&
			NULL != pDungeonGame->GetDungeon()->GetNowStage() &&
			NULL != pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStage() )
		{
			CX2DungeonSubStage* pSubStage		= pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStage();

			// ��Ȱ ��ġ�� ���ϴµ� ������ ������ Ű�ڵ带 ������ �� �ش� ���͸� ����
			int iKeyCode = pSubStage->GetSubStageData()->GetKeyCodeOfMonsterForRebirth();
			CX2GUNPC* pKeyCodeNpc = pDungeonGame->GetNPCUnitByKeyCode( iKeyCode );

			if ( pKeyCodeNpc == NULL )
				return false;


			D3DXVECTOR3 vPosForRebirth	= pKeyCodeNpc->GetPos();
			D3DXVECTOR3	vSpacing		= pKeyCodeNpc->GetDirVector() * ( pKeyCodeNpc->GetUnitWidth( false ) / 2 );

			vPosForRebirth -= vSpacing;

			CKTDGLineMap* pLineMap = g_pX2Game->GetLineMap();
			int iLineIndex = pLineMap->GetNearestLine( vPosForRebirth );

			const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );

			if ( pLineData == NULL )
				return false;

			D3DXVECTOR3 vLandPos = ( pLineData->startPos + pLineData->endPos ) * 0.5f;

			if ( vLandPos.x > vPosForRebirth.x )
				vLandPos = pLineData->startPos;

			D3DXVECTOR3 vPos = vLandPos + D3DXVECTOR3( 0, LINE_RADIUS, 0 );

			//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.

			if ( IsMyUnit() )
			{
				m_FrameDataFuture.syncData.position					= vPos;
				m_FrameDataFuture.unitCondition.dirDegree			= pLineData->dirDegree;
				m_FrameDataFuture.unitCondition.landPosition		= vLandPos;
				m_FrameDataFuture.syncData.lastTouchLineIndex		= iLineIndex;
			}

			m_FrameDataNow.syncData.position					= m_FrameDataFuture.syncData.position;
			m_FrameDataNow.unitCondition.dirDegree				= m_FrameDataFuture.unitCondition.dirDegree;
			m_FrameDataNow.unitCondition.landPosition			= m_FrameDataFuture.unitCondition.landPosition;
			m_FrameDataNow.syncData.lastTouchLineIndex			= m_FrameDataFuture.syncData.lastTouchLineIndex;

			BOOST_FOREACH( SyncData& syncData, m_ReceiveSyncDataList )
			{
				syncData.position = vPos;
				syncData.lastTouchLineIndex = iLineIndex;
			}
			return true;
		}
		else
			return false;
	}

	return false;
}
//}} kimhc // 2010.8.10 // Ư�� ������ ��ġ ���� ��Ȱ ��ġ�� ���

void CX2GUUser::InitStat()
{
	CX2GameUnit::InitStat();

	AccessStat().InitStat();

	if( GetUnit()->GetUnitData().m_GameStat.m_fBaseHP > 0 )
	{
		//AccessStat().AddStat( GetUnit()->GetUnitData().m_GameStat );
		SetGameStat( GetUnit()->GetUnitData().m_GameStat );
	}
	else
	{
		SetGameStat( GetUnit()->GetUnitData().m_Stat );
	}

}


void CX2GUUser::InitSocketData()
{
	if( NULL == GetUnit() )
	{
		return;
	}

	{
		m_SocketData.m_SocketExtraDamage.m_DamagePerSec = GetStat().GetStat().m_fAtkMagic;	// Ȥ�� �Ʒ��� �ڵ忡�� �ʱ�ȭ�� �ȵǴ� ��츦 ����ؼ�
	}

	for( int i = 0; i < (int)GetUnit()->GetUnitData().m_NowEqipItemUIDList.size(); i++ )
	{
		UidType eqipUID = GetUnit()->GetUnitData().m_NowEqipItemUIDList[i];
		CX2Item* pItem = GetUnit()->GetInventory().GetItem( eqipUID );

		if( NULL == pItem ||
			NULL == pItem->GetItemTemplet()
            )
		{
			continue;
		}

		//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����
		if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
			continue;
		//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����

		const float fAtkMagic = pItem->GetStat().m_fAtkMagic;

		//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�

			std::vector<int> vecSocketOptions;// �Ϲ� ���ϰ� ���� �ɼ��� ���� ���� �����̳�
			// ���� ���� �ɼ� ���
			if( NULL != pItem )
			{
				vecSocketOptions.assign( pItem->GetItemData().m_vecRandomSocket.begin(),
					pItem->GetItemData().m_vecRandomSocket.end() );
			}
			// �Ϲ� ���� �ɼ� ���
			std::vector<int> vecNormalSocketOption;
			pItem->GetItemTemplet()->GetSocketOptions( vecNormalSocketOption );
			vecSocketOptions.insert( vecSocketOptions.end(), vecNormalSocketOption.begin(), vecNormalSocketOption.end() );


		// �����ۿ� �䱸 ������ ������ �䱸 ������ ������ ������ ���
		int iSocketLevel = 0;
		if ( 0 < pItem->GetItemTemplet()->GetUseLevel() )
			iSocketLevel = GetUnitLevel();
		// ������ ������ ������ ������ ������ ���
		else
			iSocketLevel = GetUnit()->GetUnitData().m_Level;

		// ������ ������ ���� �ɼ� ����

		SumSocketOption( vecSocketOptions, fAtkMagic, iSocketLevel );

		// �����ۿ� ������ ���� ���Ͽ� ���� �ɼ� ����
		SumSocketOption( pItem->GetItemData().m_SocketOption, fAtkMagic, iSocketLevel );
		//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
	}

	// �� ��ų ����ȿ�� (��� ���ų�� ������������ �����ȴ�.)
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_TRAINING_GAME:
	case CX2Main::XS_BATTLE_FIELD:
		{
			if( NULL != g_pX2Game && NULL != g_pX2Game->GetDropItemManager() && NULL != g_pX2Game->GetMyUnit())
			{	//�����Ű�� �� �ʱ�ȭ
				if( GetUnit()->GetUID() == g_pX2Game->GetMyUnit()->GetUnitUID() )
				{
					g_pX2Game->GetDropItemManager()->SetSummonItemPickupPet( false );
				}
			}

			if( GetUnit() != NULL )
			{
				KPetInfo *kPetInfo = GetUnit()->GetPetInfo();
				if( g_pData != NULL && g_pData->GetPetManager() != NULL && kPetInfo != NULL )
				{
					//���� ������ �ݱ� ��ų�� Ȱ��ȭ �Ǿ� �ִٸ� ������ �ݱ� ����
					if( NULL != g_pX2Game && NULL != g_pX2Game->GetDropItemManager() && NULL != g_pX2Game->GetMyUnit() )
					{	//�ڱ� ���ֿ��� ����
						if( GetUnit()->GetUID() == g_pX2Game->GetMyUnit()->GetUnitUID() )
						{
							g_pX2Game->GetDropItemManager()->SetSummonItemPickupPet( kPetInfo->m_bAutoLooting );
						}
					}

					CX2PetManager::PetTemplet *pPetTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)kPetInfo->m_iPetID );
					if( pPetTemplet != NULL )
					{
						// �������� 40% �̻��� ��츸 ��ų ����ǵ��� �Ѵ�.
						if( (float)kPetInfo->m_sSatiety / CX2PetManager::MAX_OF_SATIETY >= CX2PetManager::SATIETY_RATIO_TO_BE_ABLE_TO_APPLY_AURA )
						{
							int iStep = 0;
							iStep = pPetTemplet->m_vecPetStatus[kPetInfo->m_cEvolutionStep];
							if( iStep > 0 )
							{
								for(int i=0; i < iStep; ++i)
								{
									//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
									// �� ��ų�� ���� ������ ������ ������ ����
									SumSocketOption( pPetTemplet->m_vecAuraSkillOption[ i ], 1.f, GetUnitLevel() );
									//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
								}	// for
							}	// if
						}	// if
					}	// if
				}	// if
			}	// if
		} break;

	default:
		break;
	}	// switch


	/// ���̵� ���� ���� ��쿡 ž�� ���� �ɼ� ����
	if ( GetRidingOn() && m_ptrRidingPet )
	{
		CX2RidingPetManager::RidingPetTemplet* pRidingPetTemplet =
			CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( m_ptrRidingPet->GetInfo().m_Id ) );
		if ( NULL != pRidingPetTemplet )
			SumSocketOption( pRidingPetTemplet->m_vecSocketOption, 1.0f, GetUnitLevel() );
	}


//#ifdef TITLE_SYSTEM
	if( g_pData->GetTitleManager()->GetTitleInfo(GetUnit()->GetTitleId()) != NULL )
	{
		m_vecTitleOptions = g_pData->GetTitleManager()->GetTitleInfo( GetUnit()->GetTitleId() )->m_vecSocketOption;

		//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
		// Īȣ�� ���� ������ ������ ������ ����
		SumSocketOption( m_vecTitleOptions, 1.f, GetUnitLevel() );
		//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
	}
//#endif TITLE_SYSTEM

	// ��Ʈ ������ �ɼǿ� ���� ����ȿ��
	InitSetItemOptions();


	m_DamageData.m_SocketExtraDamage	= m_SocketData.m_SocketExtraDamage;
	m_DamageData.m_MultipleExtraDamage	= m_SocketData.m_MultipleSocketExtraDamage;

	// ������ ���� ����ȿ��
	vector<int> vecBuffSocketOptions;
	g_pData->GetPremiumBuffManager()->GetSocketOptions( GetUnit()->GetUnitData().GetPremiumBuffList(), vecBuffSocketOptions );

	if ( !vecBuffSocketOptions.empty() )
		SumSocketOption( vecBuffSocketOptions, 1.f, GetUnitLevel() );

	// �ִ�ġ ����
	m_SocketData.LimitMaximum();

	InitAdditionalStatOptionValue();

	/// �������� ���� �߰� ��ų ���� ȿ�� ����
	if ( NULL != GetUnit() )
		GetUnit()->ResetIncreaseSkillLevelBySocket();
}



//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
void CX2GUUser::SumSocketOption( const vector<int>& vecSocketOption, const float fAtkMagic, const int iSocketLevel_ )
//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
{
	for( UINT j=0; j<vecSocketOption.size(); j++ )
	{
		int socketOptionID = vecSocketOption[j];
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( socketOptionID );

		if( NULL == pSocketData )
			continue;
        const CX2SocketItem::SocketData& kSocketData = *pSocketData;


		if ( !CanApplySocketDataByGameType( kSocketData ) )
			continue;


		AddEnchantAttack( kSocketData );
		AddEnchantResist( kSocketData );

		//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�

		if ( NULL != g_pData &&
			 NULL != g_pData->GetMyUser() &&
			 NULL != g_pData->GetMyUser()->GetSelectUnit()  )
		{
			CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();

			m_SocketData.ConvertAndAdd( kSocketData, iSocketLevel_, eUnitType );
		}

		//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�

		if( false == kSocketData.m_vecSkillLevelUpID.empty() )
		{
			m_SocketData.m_vecSkillLevelUpID.insert( m_SocketData.m_vecSkillLevelUpID.end(), kSocketData.m_vecSkillLevelUpID.begin(), kSocketData.m_vecSkillLevelUpID.end() );
		}
		if( false == kSocketData.m_vecSkillLevelUpGroupID.empty() &&
			NULL != g_pData->GetSocketItem() )
		{
			BOOST_FOREACH( int iGroupID, kSocketData.m_vecSkillLevelUpGroupID )
			{
				vector<SkillLevelUpIDAndNum> vecSkillLevelUPIDAndNumList;
				g_pData->GetSocketItem()->GetSkillIDByUnicClassAndGropID( vecSkillLevelUPIDAndNumList, GetUnitClass(), iGroupID );

				// �׷� ID�� �ش��ϴ� ��� ��ų ����Ʈ ���.
				BOOST_FOREACH( const SkillLevelUpIDAndNum& SkillLevelUPIDAndNumList, vecSkillLevelUPIDAndNumList )
				{
					m_SocketData.m_vecSkillLevelUpID.push_back( SkillLevelUPIDAndNumList );
				}
			}
		}

//{{ kimhc // 2010.4.1 // ��д��� �۾�(��Ʈ������ȿ��)
		if ( InitSocketEffectEx( kSocketData ) )
			continue;
//}} kimhc // 2010.4.1 // ��д��� �۾�(��Ʈ������ȿ��)


		/// ���� ������ ��Ÿ�� ���� ���� ���� ---------------------------------------------------------------------------------------------------------------------
		if( CX2DamageManager::EDT_NONE != kSocketData.m_SocketExtraDamage.m_ExtraDamageType )
		{
			m_SocketData.m_SocketExtraDamage.Copy( kSocketData.m_SocketExtraDamage );


			// �м��ۿ� EDT_FIRE, EDT_POISON ������ ��쿡 ���� unit stat�� magicAtk
			if( 0.f == fAtkMagic ) // note!! �м����� ��쿡 ���� ���ݷ��� 0�̴�
			{
				switch( kSocketData.m_SocketExtraDamage.m_ExtraDamageType )
				{
				case CX2DamageManager::EDT_FIRE:
					{
						m_SocketData.m_SocketExtraDamage.m_DamagePerSec = GetStat().GetStat().m_fAtkMagic * kSocketData.m_SocketExtraDamage.m_DamagePerSec;
					} break;

				case CX2DamageManager::EDT_POISON:
					{
						m_SocketData.m_SocketExtraDamage.m_DamagePerSec = GetStat().GetStat().m_fAtkMagic * kSocketData.m_SocketExtraDamage.m_DamagePerSec;
					} break;

				default:
					{
						ASSERT( !"unexpected" );
						m_SocketData.m_SocketExtraDamage.m_DamagePerSec = GetStat().GetStat().m_fAtkMagic * kSocketData.m_SocketExtraDamage.m_DamagePerSec;
					} break;
				}
			}
			else
			{
				m_SocketData.m_SocketExtraDamage.m_DamagePerSec = fAtkMagic * kSocketData.m_SocketExtraDamage.m_DamagePerSec;
			}
		}

		if( false == kSocketData.m_MultipleSocketExtraDamage.empty() )
		{
			m_SocketData.m_MultipleSocketExtraDamage.insert( m_SocketData.m_MultipleSocketExtraDamage.begin(),
				kSocketData.m_MultipleSocketExtraDamage.begin(), kSocketData.m_MultipleSocketExtraDamage.end() );
		}
		/// -----------------------------------------------------------------------------------------------------------------------------------------------------



		if ( NULL != kSocketData.m_ptrBuffFactor )
			CreateAndInsertSkillAndSocketBuffFactorToList( kSocketData.m_ptrBuffFactor );

		if ( kSocketData.m_fDamageUpPerAtSpecificMonster > 0 )
		{
			for( set<int>::const_iterator sit = kSocketData.m_setMonsterIDForDamageUp.begin(); sit != kSocketData.m_setMonsterIDForDamageUp.end(); sit++ )
			{
				map<int,float>::iterator mit = m_mapNpcIdNDamageUpPercent.find( *sit );
				if ( mit != m_mapNpcIdNDamageUpPercent.end() )
				{
					mit->second += kSocketData.m_fDamageUpPerAtSpecificMonster;
				}
				else
				{
					m_mapNpcIdNDamageUpPercent.insert( std::make_pair( *sit, kSocketData.m_fDamageUpPerAtSpecificMonster ) );
				}
			}
		}


		if ( kSocketData.m_fEvadePercentByMonsterAttack > 0 )
		{
			for( set<int>::const_iterator sit = kSocketData.m_setMonsterIDForEvade.begin(); sit != kSocketData.m_setMonsterIDForEvade.end(); sit++ )
			{
				map<int,float>::iterator mit = m_mapNpcIDNDEvadeUpPercent.find( *sit );
				if ( mit != m_mapNpcIDNDEvadeUpPercent.end() )
				{
					mit->second += kSocketData.m_fEvadePercentByMonsterAttack;
				}
				else
				{
					m_mapNpcIDNDEvadeUpPercent.insert( std::make_pair( *sit, kSocketData.m_fEvadePercentByMonsterAttack ) );
				}
			}
		}


		if ( kSocketData.m_ImmuneExtraDamageType != CX2DamageManager::EDT_NONE )
		{
			AddExtraDamageImmuneData( kSocketData.m_ImmuneExtraDamageType, kSocketData.m_fPercentImmuneExtraDamage );
		}

		if( kSocketData.m_iSummonNpcId > 0 && kSocketData.m_fSummonNpcRate > 0.f )
		{
			SummonNpcSocket tSummonNpc = {CX2UnitManager::NUI_NONE, 0.f, 20.f};
			tSummonNpc.m_iNpcId = (CX2UnitManager::NPC_UNIT_ID)kSocketData.m_iSummonNpcId;
			tSummonNpc.m_fSummonRate = kSocketData.m_fSummonNpcRate;
			tSummonNpc.m_fSummonNpcCoolTime = kSocketData.m_fSummonNpcCoolTime;

			m_vecSummonNpcSocket.push_back(tSummonNpc);
		}

		/// �������� ���� ������ Ư�� ȿ���� �����ϴ� ��� �߰�
		if ( CX2SocketItem::HMET_NONE != kSocketData.m_eHyperModeEffectiveType )
		{
			/// ��� ���� ����
			CX2SocketItem::POSSIBLE_USE_PLACE bPossibleUsePlace = CX2SocketItem::PUP_ALL;

			/// ���������̶� ���������� ���� ������ ������ ���� �Ǿ� �־, �켱���� ������ ��ƴ�...
			/// �׳� ����������� �˻�����.
			if ( true == kSocketData.m_bDungeonOnly )
				bPossibleUsePlace = CX2SocketItem::PUP_DUNGEON_ONLY;
			else if ( true == kSocketData.m_bDungeonOnly )
				bPossibleUsePlace = CX2SocketItem::PUP_PVP_ONLY;

			/// ������ ���Ǵ� ���� ���� ����
			CX2SocketItem::InfoByUseHyperMode sInfoByUseHyperMode;
			sInfoByUseHyperMode.m_ePossibleUsePlace			= bPossibleUsePlace;						/// ��� ��ġ
			sInfoByUseHyperMode.m_eHyperModeEffectiveType	= kSocketData.m_eHyperModeEffectiveType;	/// ��� Ÿ��
			sInfoByUseHyperMode.m_fEffectiveValue			= kSocketData.m_fEffectiveValue;			/// ���� ��

			if ( NULL != kSocketData.m_ptrBuffFactorCustomUse )				/// ������ ����Ѵٸ�, ���� ���� ����
				sInfoByUseHyperMode.m_pBuffFactorPtr = kSocketData.m_ptrBuffFactorCustomUse;

			/// ������ ���Ǵ� ���� ���� ����
			m_SocketData.m_vecInfoByUseHyperMode.push_back( sInfoByUseHyperMode );
		}
	}
}



void CX2GUUser::InitSetItemOptions()
{
	//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
	m_vecSetItemOptions.resize(0);

	// first: ��ƮID, second: ��Ʈ�� �̷�� ����� ����
	map<int,int> mapSetIDNParts;
	GetUnit()->GetSetIDNPartsNum( mapSetIDNParts );

	if ( false == mapSetIDNParts.empty() )
	{
		map< int, int >::const_iterator mItr = mapSetIDNParts.begin();
		vector<int> vecOptions;

		while ( mItr != mapSetIDNParts.end() )
		{
			// SetID ���� Option��� SetItem�� �ִ� ���� ������ ����
			const int iMaxLevel = g_pData->GetItemManager()->GetSetItemOptions( mItr->first, mItr->second, vecOptions );

			// Option�� �ִ� ���
			if ( false == vecOptions.empty() )
			{
				// SetItem�� �䱸 ������ ������ �䱸������ �ִ밪�� ������ ������ ���
				if ( 0 < iMaxLevel )
					SumSocketOption( vecOptions, 1.f, iMaxLevel );
				// ������ ������ ������ ������ ������ ���
				else
					SumSocketOption( vecOptions, 1.f, GetUnit()->GetUnitData().m_Level );

				// �ɼǵ��� GUUser�� ������ �ִ� m_vecSetItemOptions�� �־���
				m_vecSetItemOptions.insert( m_vecSetItemOptions.end(), vecOptions.begin(), vecOptions.end() );
				vecOptions.resize(0);
			}
			++mItr;
		}
	}
	//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
}

//{{ robobeg : 2012-11-16
/*static*/
CX2DamageManager::EXTRA_DAMAGE_TYPE	CX2GUUser::CalcWeaponEnchantExtraDamageType( CX2Unit* pX2Unit_ )
{
	if ( pX2Unit_ != NULL )
	{
		CX2Item* pItem = pX2Unit_->GetInventory().GetEquippingItemByEquipPos( CX2Unit::EP_WEAPON_HAND, false );
		if( NULL != pItem &&
			// ������ ���� ������� ����ó��
			//pItem->GetItemData().m_Endurance > 0
			( (	pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance > 0) ||
				pItem->GetItemData().m_PeriodType == CX2Item::PT_INFINITY ) )
		{
			return	g_pData->GetEnchantItem()->GetExtraDamageType( pItem->GetItemData().m_EnchantedAttribute );
		}
	}//if

	return	CX2DamageManager::EDT_NONE;
}
//}} robobeg : 2012-11-16


void CX2GUUser::CalcWeaponEnchantExtraDamageType()
{
	// ���� ��æƮ �Ӽ�
	m_eWeaponEnchantExtraDamageType = CalcWeaponEnchantExtraDamageType( GetUnit() );

}


void CX2GUUser::InitEnchantData()
{
	if( NULL == GetUnit() )
	{
		return;
	}

	// ���� ��æƮ �Ӽ�
	CalcWeaponEnchantExtraDamageType();


	// �� ��æƮ �Ӽ�
	// ����� �Ʒ��� 4���� ������ġ�� �ش��ϴ� �����۸� �Ӽ������� ����
	//EP_DEFENCE_BODY,		//����
	//EP_DEFENCE_LEG,		//����
	//EP_DEFENCE_HAND,		//�尩
	//EP_DEFENCE_FOOT,		//�Ź�
	for ( int i = CX2Unit::EP_DEFENCE_BODY; i <= CX2Unit::EP_DEFENCE_FOOT; i++ )
	{
		CX2Item* pItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( (CX2Unit::EQIP_POSITION)i, false );
		if ( pItem != NULL )
		{

			//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����
			if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pItem->GetItemData().m_Endurance <= 0 )
				continue;
			//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����


			CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType = g_pData->GetEnchantItem()->GetExtraDamageType( pItem->GetItemData().m_EnchantedAttribute );
			CX2DamageManager::EXTRA_DAMAGE_TYPE eDecomposedEDT[2] = { CX2DamageManager::EDT_NONE, };
			CX2EnchantItem::DecomposeEnchantExtraDamage( extraDamageType, eDecomposedEDT[0], eDecomposedEDT[1] );


			map< CX2EnchantItem::ENCHANT_TYPE, int >::iterator mit;
			for( int i=0; i<2; i++ )
			{
				if( eDecomposedEDT[i] == CX2DamageManager::EDT_NONE )
					continue;

				CX2EnchantItem::ENCHANT_TYPE enchantType = GetEnchantTypeFromExtraDamageType( eDecomposedEDT[i] );
				AddDefenceEnchantData( enchantType, CX2EnchantItem::GetEnchantResistValue( eDecomposedEDT[i] ) );
			}
		}
	}

	//{{ kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����
	 if ( CX2Game::GT_PVP != g_pX2Game->GetGameType() )
		 AddEnchantStatFromDragonBreathToDefenceEnchantData();
	//}} kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����
}

//{{ kimhc // 2011.1.14 // û 1�� ����
/** @function : AddDefenceEnchantData
	@brief : m_mapDefenceEnchantData���� Key�� eEnchantType�ΰ��� ã�Ƽ� �� ���ְų� �߰� ��
	@param : CX2EnchantItem::ENCHANT_TYPE eEnchantType(������ ��þƮ ����), int iEnchantValue (������ ��þƮ ��)
*/
void CX2GUUser::AddDefenceEnchantData( CX2EnchantItem::ENCHANT_TYPE eEnchantType, int iEnchantValue )
{
	map<CX2EnchantItem::ENCHANT_TYPE, int>::iterator mItr = m_mapDefenceEnchantData.find( eEnchantType );

	if( m_mapDefenceEnchantData.end() != mItr )
	{
		mItr->second += iEnchantValue;

		if( mItr->second > CX2EnchantItem::EAR_MAX_VALUE )
			mItr->second = CX2EnchantItem::EAR_MAX_VALUE;
	}
	else
	{
		if( iEnchantValue > CX2EnchantItem::EAR_MAX_VALUE )
			m_mapDefenceEnchantData[ eEnchantType ] = CX2EnchantItem::EAR_MAX_VALUE;
		else
			m_mapDefenceEnchantData[ eEnchantType ] = iEnchantValue;
	}
}

/** @function : AddExtraDamageImmuneData
	@brief : m_mapExtraDamageNImmunePer���� Key�� eExtraDamageType�ΰ��� ã�Ƽ� �� ���ְų� �߰� ��
	@param : CX2DamageManager::EXTRA_DAMAGE_TYPE eExtraDamageType(������ ExtraDamage ����), float fImmuneValue (Immune ��)
*/
void CX2GUUser::AddExtraDamageImmuneData( CX2DamageManager::EXTRA_DAMAGE_TYPE eExtraDamageType, float fImmuneValue )
{
	map<CX2DamageManager::EXTRA_DAMAGE_TYPE, float> ::iterator mItr = m_mapExtraDamageNImmunePer.find( eExtraDamageType );

	if( m_mapExtraDamageNImmunePer.end() != mItr )
	{
		mItr->second += fImmuneValue;
	}
	else
	{
		m_mapExtraDamageNImmunePer[ eExtraDamageType ] = fImmuneValue;
	}
}

/** @function : AddEnchantStatFromPassiveSkillToDefenceEnchantData
	@brief : �нú� ��ų�� ���ؼ� ������ EnchantStat�� m_mapDefenceEnchantData�� ����
*/
void CX2GUUser::AddEnchantStatFromPassiveSkillToDefenceEnchantData()
{
	for ( int iEnchantType = CX2EnchantItem::ET_NONE + 1; iEnchantType < CX2EnchantItem::ET_END; ++iEnchantType )
	{
		const float fEnchantValue = GetEnchantStatFromPassiveSkillByEnchantType( iEnchantType );

		if ( 0.0f < fEnchantValue )
		{
			CX2EnchantItem::ENCHANT_TYPE eEnchantType = CX2EnchantItem::ENCHANT_TYPE( iEnchantType );
			AddDefenceEnchantData( eEnchantType, static_cast<int>( fEnchantValue ) );
		} // if
	} // for
}

/** @function : AddExtraDamageImmuneRateFromPassiveSkillToImmuneData
	@brief : �нú� ��ų�� ���ؼ� ������ ExtraDamageImmune Ȯ���� m_mapExtraDamageNImmunePer�� ����
*/
void CX2GUUser::AddExtraDamageImmuneRateFromPassiveSkillToImmuneData()
{
	const CX2UserSkillTree& refUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
	int iSkillLevel = 0;

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A, true );

	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A );
		if( pSkillTemplet != NULL )
		{
			float fImmuneValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_FIRE_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_FIRE, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_FROZEN_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_FROZEN, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_ICE_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_COLD, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_POISON_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_POISON, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_Y_PRESSED_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_Y_PRESSED, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_LEG_WOUND_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_LEG_WOUND, fImmuneValue );
		}
	}

	iSkillLevel = refUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B, true );

	if( iSkillLevel > 0)
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B );
		if( pSkillTemplet != NULL )
		{
			float fImmuneValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_STUN_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_STUN, fImmuneValue );

			fImmuneValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EDT_CURSE_RESIST_REL, iSkillLevel );
			AddExtraDamageImmuneData( CX2DamageManager::EDT_CURSE, fImmuneValue );
		}
	}
}
/** @function : GetEnchantStatFromPassiveSkillByEnchantType
	@brief : ��þƮ Ÿ�Ժ� ��ġ �� ����
	@param : CX2EnchantItem::ENCHANT_TYPE eType (��ġ�� �˰����� ��þƮ Ÿ��)
	@return : const float (�ش� ��þƮ Ÿ���� ��ġ ��)
*/
const float CX2GUUser::GetEnchantStatFromPassiveSkillByEnchantType( int iEnchantType )
{
	switch ( static_cast<CX2EnchantItem::ENCHANT_TYPE>( iEnchantType ) )
	{
	case CX2EnchantItem::ET_BLAZE:
		return m_EnchantStatFromPassiveSkill.m_fDefBlaze;
		break;

	case CX2EnchantItem::ET_WATER:
		return m_EnchantStatFromPassiveSkill.m_fDefWater;
		break;

	case CX2EnchantItem::ET_NATURE:
		return m_EnchantStatFromPassiveSkill.m_fDefNature;
		break;

	case CX2EnchantItem::ET_WIND:
		return m_EnchantStatFromPassiveSkill.m_fDefWind;
		break;

	case CX2EnchantItem::ET_LIGHT:
		return m_EnchantStatFromPassiveSkill.m_fDefLight;
		break;

	case CX2EnchantItem::ET_DARK:
		return m_EnchantStatFromPassiveSkill.m_fDefDark;
		break;

	default:
		return 0.0f;
		break;
	} // switch

}
//}} kimhc // 2011.1.14 // û 1�� ����

//{{ kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����

/** @function : AddEnchantStatFromDragonBreathToDefenceEnchantData
	@brief : ���� ���� 2�ܰ�, �巡���� ���ῡ ���ؼ� ������ EnchantStat�� m_mapDefenceEnchantData�� ����
*/
void CX2GUUser::AddEnchantStatFromDragonBreathToDefenceEnchantData()
{
	// ������� 2�ܰ谡 ������� �ʾ����� �׳� ������
	if ( NULL == g_pData->GetWorldMissionManager() ||
		 true == g_pData->GetWorldMissionManager()->IsNullWorldBuffDragonBreath() )
		 return;

	// ������� 2�ܰ谡 ����Ǿ�����
	for ( int iEnchantType = CX2EnchantItem::ET_NONE + 1; iEnchantType < CX2EnchantItem::ET_END; ++iEnchantType )
	{
		const float fEnchantValue = g_pData->GetWorldMissionManager()->GetEnchantStatFromDrgonBreath( iEnchantType );

		if ( 0.0f < fEnchantValue )
		{
			CX2EnchantItem::ENCHANT_TYPE eEnchantType = CX2EnchantItem::ENCHANT_TYPE( iEnchantType );
			AddDefenceEnchantData( eEnchantType, static_cast<int>( fEnchantValue ) );
		} // if
	} // for
}
//}} kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����

//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ



/*static*/
void	CX2GUUser::InitComponent( InitData& OutInit_, KLuaManager& luaManager_)
{
	CX2GameUnit::InitComponent( OutInit_.m_device, luaManager_ );
}

/*static*/
void	CX2GUUser::AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pX2Unit_ )
{
	g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"HoldCenter" );
	g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"HoldCircle" );
	g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"HeadMarkerRed3D" );
	g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"HeadMarkerBlue3D" );

	if ( pX2Unit_ != NULL )
	{

//#ifdef TITLE_SYSTEM
		int titleId = pX2Unit_->GetTitleId();
		if(titleId > 0)
		{
			wstring titleName = g_pData->GetTitleManager()->GetTitleModel(titleId);
			g_pData->GetUIMajorParticle()->AppendToDeviceList( listInOut_, titleName );
		}
//#endif	TITLE_SYSTEM

		switch( pX2Unit_->GetType() )
		{
		case CX2Unit::UT_NONE:
			break;
		case CX2Unit::UT_ELSWORD:
			CX2GUElsword_SwordMan::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_ARME:
			CX2GUArme_VioletMage::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_LIRE:
			CX2GULire_ElvenRanger::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_RAVEN:
			CX2GURaven::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_EVE:
			CX2GUEve::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_CHUNG:
			CX2GUChung::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_ARA:
			CX2GUAra::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_ELESIS:
			CX2GUEL::AppendComponentToDeviceList( listInOut_ );
			break;
		case CX2Unit::UT_ADD:
			CX2GUAdd::AppendComponentToDeviceList( listInOut_ );
			break;
		}
	}

}



void CX2GUUser::InitComponent()
{
	CX2GameUnit::InitComponent();

	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		//ResetMaxHP();
		m_LuaManager.GetValue( "MP_CHANGE_RATE",		m_fOriginalMPChangeRate );

	//{{ kimhc // 2010.6.14	// �巡���м�Ʈ ȿ�� ���� ���� ����
	// GetOriginalMPChangeRate() ���� �巡���м�Ʈ ȿ���� ���� ���� �������� �Ǵ���

		ResetMPChangeRate( GetOriginalMPChangeRate() );
	//}} kimhc // 2010.6.14 // �巡���м�Ʈ ȿ�� ���� ���� ����

		ResetMaxHP();
		ResetMaxMP();

		if ( !IsMyUnit() )
		{
			SetNowHp( GetMaxHp() );
			SetNowMp( 0.f );
			SetMaxSoul( 100.f );
			SetNowSoul( 0.f );
		}
		else
		{
			ASSERT( NULL != g_pX2Game );
			switch ( g_pX2Game->GetGameType() )
			{
			case CX2Game::GT_PVP:
				{
					CX2GageManager::GetInstance()->SaveGageData();

					if( CX2Unit::UT_ADD != GetUnitType() )
					{
						if( true == m_bHasHyperModeItem )
							GetGageData()->SetHyperModeCount(1);
						else
							GetGageData()->SetHyperModeCount(0);

						m_bHasHyperModeItem = false;;
					}

					GetGageData()->SetHyperModeRemainTime(0.f);
					SetNowHp( GetMaxHp() );
					SetNowMp( 0.f );
					SetMaxSoul( 100.f );
					SetNowSoul( 0.f );
				} break;

			default:
				{
					if ( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
					{
						CX2GageManager::GetInstance()->SaveGageData();

						SetNowHp( GetMaxHp() );
						SetNowMp( 0.f );
						SetMaxSoul( 100.f );
						SetNowSoul( 0.f );
					}
					else
					{
						CX2GageManager::GetInstance()->RestoreGageData();
						if( NULL != CX2GageManager::GetInstance()->GetMyGageData())
							m_pGageData = CX2GageManager::GetInstance()->GetMyGageData()->GetCloneGageData();
						ResetMaxHP();
						ResetMaxMP();
						SetNowHp( GetMaxHp() * CX2GageManager::GetInstance()->GetMyNowHpPercent() );
						SetNowMp( GetMaxMp() * CX2GageManager::GetInstance()->GetMyNowMpPercent() );
					}
				} break;
			}
		}
		float fData = 0.f;
		m_LuaManager.GetValue( "MP_CHARGE_RATE",		fData );
		SetChangeRateChargeMpForDetonation( fData );

		SetMaxForceDown( 100.f );
		SetNowForceDown( 0.f );

		GetGageData()->SetEnable( true );

		m_FrameDataFuture.syncData.fNowHP				= GetNowHp();
		m_FrameDataFuture.syncData.fNowMP				= GetNowMp();


		AccessStat().AccessAddOnStat().Init();
		AccessStat().AccessAddOnTime().Init();


		for( UINT i=0; i<m_vecpWeapon.size(); ++i )
		{
			Weapon* pWeapon = m_vecpWeapon[i];
			pWeapon->LoadDataForUser( m_LuaManager );
		}


		m_hHoldEffCenter = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HoldCenter", 0,0,0, 0, 0 );
        CKTDGParticleSystem::CParticleEventSequence* pSeqHoldCenter = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCenter );
		if( NULL != pSeqHoldCenter )
		{
			pSeqHoldCenter->SetTrace( true );
		}

		m_hHoldEffCircle = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HoldCircle", 0,0,0, 0, 0 );
        CKTDGParticleSystem::CParticleEventSequence* pSeqHoldCircle = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCircle );
		if( NULL != pSeqHoldCircle )
		{
			pSeqHoldCircle->SetTrace( true );
		}


		if ( m_pAfterImage == NULL )
		{
			if( g_pX2Room->GetMySlot()->GetUnit()->GetUID() == m_UnitUID )
			{
				m_pAfterImage = CKTDGXSkinAfterImage::CreateSkinAfterImage( m_pXSkinAnim.get(), 10, XL_EFFECT_0 );
			}
			else
			{
				m_pAfterImage = CKTDGXSkinAfterImage::CreateSkinAfterImage( m_pXSkinAnim.get(), 5, XL_EFFECT_0 );
			}

			if ( m_pAfterImage != NULL )
			{
				m_pAfterImage->SetAfterImageColor( 0xffcccccc );
				m_pAfterImage->AddMesh( m_pMotion );
				// ���� ���� �� ������ �̹����� ���� �������� Hair�� �߰� �ϱ� ���� ����
				m_pAfterImage->SetpHairSkinMesh(m_pHairXSkinMesh);
				UpdateAfterImageEquip();
// #ifdef FIX_MEMLEAK01
// 				m_pAfterImageMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_pMotion->GetDeviceID() );
// 				m_pAfterImage->AddMesh( m_pAfterImageMotion );
// #else
// 				m_pAfterImage->AddMesh( g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_pMotion->GetDeviceID() ) );
// #endif
			}
		}

		if( m_hSeqHeadMarker == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			if( IsMyUnit() == true )
			{
				if ( g_pData->GetPVPRoom() != NULL && g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_SURVIVAL )
				{
					m_hSeqHeadMarker = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HeadMarkerRedMy3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
				}
				else
				{
					if( m_TeamNum == CX2Room::TN_RED )
						m_hSeqHeadMarker = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HeadMarkerRedMy3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
					else if( m_TeamNum == CX2Room::TN_BLUE )
						m_hSeqHeadMarker = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HeadMarkerBlueMy3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
				}
			}
			else
			{
				if ( g_pData->GetPVPRoom() != NULL && g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_SURVIVAL )
				{
					m_hSeqHeadMarker = INVALID_PARTICLE_SEQUENCE_HANDLE;
				}
				else
				{
					if( m_TeamNum == CX2Room::TN_RED )
						m_hSeqHeadMarker = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HeadMarkerRed3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
					else if( m_TeamNum == CX2Room::TN_BLUE )
						m_hSeqHeadMarker = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HeadMarkerBlue3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
				}
			}
		}



		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
		if( NULL != pSeq )
		{
			pSeq->SetTrace( true );
			pSeq->SetBillBoardType( CKTDGMatrix::BT_Y );
            if ( m_hHeadMarkerParticle == INVALID_PARTICLE_HANDLE )
            {
                m_hHeadMarkerParticle = pSeq->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
                pSeq->SetShowObject(false);
            }
		}

		if( NULL != GetUnit() )
		{
//#ifdef TITLE_SYSTEM

			int titleId = GetUnit()->GetTitleId();

			if ( IsMyUnit() == true )
			{
				// pc�� Īȣ ����ó��
				int localTitleId = g_pData->GetMyUser()->GetSelectUnit()->GetTitleId();
				if( localTitleId == 10390 && titleId != localTitleId )
				{
					// ������ pc�� Ÿ��Ʋ�� ��ü��Ų��.
					g_pData->GetTitleManager()->EqipTitle(localTitleId); //SelectTitle(localTitleId)
				}
			}

			if(titleId > 0)
			{
				if( m_hSeqEmblem == INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					wstring titleName = g_pData->GetTitleManager()->GetTitleModel(titleId);
					m_hSeqEmblem = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  titleName.c_str(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
				}

				CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
				if( pSeqEmblem != NULL )
				{
                    if ( m_hPart_Emblem_200 == INVALID_PARTICLE_HANDLE )
                    {
						m_hPart_Emblem_200 = pSeqEmblem->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
						pSeqEmblem->SetShowObject(false);
                    }
				}

			}
//#else
//			// fix!! �ӽ÷� �Ӹ����� ������ �������� �̷��� ���� ������
//			const int MAGIC_EMBLEM_ITEM_ID				= 129785;
//			const int MAGIC_EMBLEM_ITEM_ID_GOLD_MEDAL	= 130002;
//
//			bool bEquippedEmblem = false;
//
//            const CX2Item::ItemTemplet* pItemTempet = g_pData->GetItemManager()->GetItemTemplet( MAGIC_EMBLEM_ITEM_ID );
//			if( NULL != pItemTempet )
//			{
//				CX2Item* pEmblemItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( pItemTempet->GetEqipPosition(), false );
//                if( NULL != pEmblemItem && pEmblemItem->GetItemTemplet()->GetItemID() == pItemTempet->GetItemID() )
//				{
//					bEquippedEmblem = true;
//
//					if( m_hSeqEmblem == INVALID_PARTICLE_SEQUENCE_HANDLE )
//					{
//						m_hSeqEmblem = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitEmblem200", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
//					}
//
//					CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//					if( pSeqEmblem != NULL )
//					{
//						if( m_pPart_Emblem_200 == NULL )
//						{
//							m_pPart_Emblem_200 = pSeqEmblem->CreateNewParticle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
//							pSeqEmblem->SetShowObject(false);
//						}
//					}
//				}
//			}
//
//			if( false == bEquippedEmblem )
//			{
//				pItemTempet = g_pData->GetItemManager()->GetItemTemplet( MAGIC_EMBLEM_ITEM_ID_GOLD_MEDAL );
//				if( NULL != pItemTempet )
//				{
//					CX2Item* pEmblemItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( pItemTempet->GetEqipPosition(), false );
//					if( NULL != pEmblemItem &&
//                        pEmblemItem->GetItemTemplet()->GetItemID() == pItemTemplet->GetItemID()
//                        )
//					{
//						bEquippedEmblem = true;
//
//						if( m_hSeqEmblem == INVALID_PARTICLE_SEQUENCE_HANDLE )
//						{
//							m_hSeqEmblem = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitEmblemGoldMedal", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
//						}
//
//						CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//						if( pSeqEmblem != NULL )
//						{
//							if( m_pPart_Emblem_200 == NULL )
//							{
//								m_pPart_Emblem_200 = pSeqEmblem->CreateNewParticle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
//								pSeqEmblem->SetShowObject(false);
//							}
//						}
//					}
//				}
//			}
//#endif
		}




		m_bAddForceDownGageThisState	= false;
		m_fLastDamagePassTime			= 0.0f;
		m_fMaxDownForce = -9999.f;


		m_LuaManager.EndTable();
	}

}

/*static*/
void	CX2GUUser::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pX2Unit_ )
{
	CX2DamageManager::EXTRA_DAMAGE_TYPE	eDamageType = CalcWeaponEnchantExtraDamageType( pX2Unit_ );

	AppendEnchantedWeaponEffectAtHandToDeviceList( listInOut_, eDamageType );

	AppendMajorParticleToDeviceList( listInOut_ );
	AppendMinorParticleToDeviceList( listInOut_ );

	if ( pX2Unit_ != NULL )
	{
		switch( pX2Unit_->GetType() )
		{
		case CX2Unit::UT_NONE:
			break;
		case CX2Unit::UT_ELSWORD:
			CX2GUElsword_SwordMan::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_ARME:
			CX2GUArme_VioletMage::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_LIRE:
			CX2GULire_ElvenRanger::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_RAVEN:
			CX2GURaven::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_EVE:
			CX2GUEve::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_CHUNG:
			CX2GUChung::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_ARA:
			CX2GUAra::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_ELESIS:
			CX2GUEL::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		case CX2Unit::UT_ADD:
			CX2GUAdd::AppendEffectToDeviceList( listInOut_, eDamageType );
			break;
		}

	}

}


void CX2GUUser::InitEffect()
{
	CX2GameUnit::InitEffect();

	InitEnchantedWeaponEffectAtHand();
	InitEnchantWeaponEffectUnitSpecific();

	if( GetUnit() != NULL && IsMyUnit() == true )
	{
		if(m_pComboGuide != NULL)
			m_pComboGuide->InitComboGuide( GetUnit()->GetClass() );
	}

}

void CX2GUUser::InitializeGUUserMajorParticleArray()
{
	for ( int index = 0; index < COMMON_MAJOR_PII_END; index++ )
	{
		m_ahCommonMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUUser::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMajorParticle() == NULL )
		return;

	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"RevengeImpactSlashCW" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"RevengeImpactSlashCCW" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"RevengeImpact" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"RevengeImpactTick" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"MagicFail" );
}


/*virtual*/ void CX2GUUser::CreateMajorParticleArray()
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	// �ݰݱ⿡ ���ϱ� ������ ������ ��ƼŬ
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_SLASH_CW,	L"RevengeImpactSlashCW",		1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_SLASH_CCW,	L"RevengeImpactSlashCCW",	1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT,	L"RevengeImpact",						1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_TICK,	L"RevengeImpactTick",				2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	// MP�� ���� ���¿��� ���� ������ ���� �� ������ ��ƼŬ(����)
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_MAGIC_FAIL,	L"MagicFail" );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	// ���� �̿��� ���� �� ĳ���͸��� ���̴� ���� ������, ����Ʈ ���� �̸��� �޶� �� ĳ������ InitializeCommonMajorParticleArray()���� �ʱ�ȭ
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUUser::SetCommonMajorParticleByEnum( COMMON_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1 */ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleCommonMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleCommonMajorParticleByEnum( eVal_, hHandle );

	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUUser::InitializeGUUserMinorParticleArray()
{
	for ( int index = 0; index < COMMON_MINOR_PII_END; index++ )
	{
		m_ahCommonMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUUser::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMinorParticle() == NULL )
		return;

	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"StepSmoke" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"Water_Particle01" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"HyperModeChangeLine" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"HyperModeChangeCenter" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"GroundShockWave" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"WhiteShockWave" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"ImpactTick" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"STR_ToKang" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"AirDownTick" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"DownSmoke" );
}


void CX2GUUser::CreateMinorParticleArray()
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_STEP_SMOKE,					L"StepSmoke",				-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_STEP_WATER,					L"Water_Particle01",		-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_LINE,		L"HyperModeChangeLine",		3 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_CENTER,	L"HyperModeChangeCenter",	-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE,			L"GroundShockWave",			-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_WHITE_SHOCK_WAVE,			L"WhiteShockWave",			3 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_IMPACT_TICK,					L"ImpactTick",				2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_STR_TOKANG,					L"STR_ToKang",				-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_AIR_DOWN_TICK,				L"AirDownTick",				-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMinorParticleByEnum( COMMON_MINOR_PII_DOWN_SMOKE,					L"DownSmoke",				-1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUUser::SetCommonMinorParticleByEnum( COMMON_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrPatricleName_, int iDrawCount_ /*= -1 */ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleCommonMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrPatricleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleCommonMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( eVal_ ) );

	return pSeq;
}

void CX2GUUser::CreateEnchantedWeaponParticleAtHand( const WCHAR* pName1, const WCHAR* pName2 /*= NULL*/, const WCHAR* pName3 /*= NULL*/, const WCHAR* pName4 /*= NULL*/ )
{
	if( NULL != pName1 )
	{
		if ( m_hSeqWeaponCommon1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqWeaponCommon1 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  pName1, 0,0,0 );
			CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire1 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon1 );
			if(pSeqWeaponFire1 != NULL )
				pSeqWeaponFire1->SetEmitRate( 0, 0 );
		}
	}


	if( NULL != pName2 )
	{
		if ( m_hSeqWeaponCommon2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqWeaponCommon2 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  pName2, 0,0,0 );
			CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire2 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon2 );
			if (pSeqWeaponFire2 != NULL )
				pSeqWeaponFire2->SetEmitRate( 0, 0 );
		}
	}

	if( NULL != pName3 )
	{
		if ( m_hSeqWeaponCommon3 == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqWeaponCommon3 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  pName3, 0,0,0 );
			CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire3 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon3 );
			if (pSeqWeaponFire3 != NULL )
				pSeqWeaponFire3->SetEmitRate( 0, 0 );
		}
	}


	if( NULL != pName4 )
	{
		if ( m_hSeqEnchantedWeaponCommon4 == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqEnchantedWeaponCommon4 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  pName4, 0,0,0 );
			CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire4 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchantedWeaponCommon4 );
			if (pSeqWeaponFire4 != NULL )
				pSeqWeaponFire4->SetEmitRate( 0, 0 );
		}
	}
}

void CX2GUUser::DeleteGUUserMajorParticle()
{
	CX2GameUnit::DeleteGameUnitMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < COMMON_MAJOR_PII_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahCommonMajorParticleInstance[index] );
	}
}

void CX2GUUser::DeleteGUUserMinorParticle()
{
	CX2GameUnit::DeleteGameUnitMinorParticle();

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	for ( int index = 0; index < COMMON_MINOR_PII_END; index++ )
	{
		pMinorParticleSystem->DestroyInstanceHandle( m_ahCommonMinorParticleInstance[index] );
	}
}

/*static*/
void    CX2GUUser::InitMotion( InitData& OutInit_, KLuaManager& luaManager_ )
{
	if( luaManager_.BeginTable( "INIT_MOTION" ) == true )
	{
		std::wstring wstrMotionName;
		if ( luaManager_.GetValue( "MOTION_FILE_NAME", wstrMotionName ) == true )
		{
			OutInit_.m_device.m_vecstrReadyXSkinMesh.push_back( wstrMotionName );
		}
		luaManager_.EndTable();
	}
}//CX2GUUser::InitMotion()


/*static*/
void CX2GUUser::AppendMotionToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pX2Unit_ )
{

	if ( pX2Unit_ != NULL && pX2Unit_->GetUnitTemplet() != NULL )
	{
		const CX2Unit::UnitTemplet* pUnitTemplet = pX2Unit_->GetUnitTemplet();

		if( pUnitTemplet->m_GameMotion0.empty() == false )
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, pUnitTemplet->m_GameMotion0 );
		}

		int iGrade = GetX2UnitClassGrade( pUnitTemplet->m_UnitClass );
		switch( iGrade )
		{
		case 2:
			if ( pUnitTemplet->m_GameMotion2.empty() == false )
			{
				g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, pUnitTemplet->m_GameMotion2 );
			}//if
			break;
		case 1:
			if ( pUnitTemplet->m_GameMotion1.empty() == false )
			{
				g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, pUnitTemplet->m_GameMotion1 );
			}//if
			break;
		default:
			break;
		}
		if ( pUnitTemplet->m_EmotionMotion.empty() == false )
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, pUnitTemplet->m_EmotionMotion );
		}//if
	}//if

}


void CX2GUUser::InitMotion()
{
	if( m_LuaManager.BeginTable( "INIT_MOTION" ) == true )
	{
		wstring motion;
		LUA_GET_VALUE( m_LuaManager, "MOTION_FILE_NAME", motion, L"" );

		m_pMotionFuture		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motion.c_str() );
		m_pMotion			= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motion.c_str() );

		m_pXSkinAnimFuture	= CKTDGXSkinAnim::CreateSkinAnim();
		m_pXSkinAnim		= CKTDGXSkinAnim::CreateSkinAnimPtr();

		m_pXSkinAnimFuture->SetAnimXSkinMesh( m_pMotionFuture );
		m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );

		std::vector<wstring> vecMotion;
		CKTDXDeviceXSkinMesh* pMixMotion = NULL;
		int iGrade = GetX2UnitClassGrade( GetUnit()->GetUnitTemplet()->m_UnitClass );

		vecMotion.push_back( GetUnit()->GetUnitTemplet()->m_GameMotion0 );

		switch( iGrade )
		{
		case 2:
			vecMotion.push_back( GetUnit()->GetUnitTemplet()->m_GameMotion2 );
		case 1:
			vecMotion.push_back( GetUnit()->GetUnitTemplet()->m_GameMotion1 );
			break;
		default:
			break;
		}

		vecMotion.push_back( GetUnit()->GetUnitTemplet()->m_EmotionMotion );

		if( GetUnit()->GetUnitTemplet()->m_GameMotion0 == L"" )
		{
			vecMotion.clear();
		}

		for(UINT i=0; i<vecMotion.size(); ++i)
		{
			pMixMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( vecMotion[i] );
			if(  pMixMotion != NULL )
			{
				m_pXSkinAnimFuture->MixAnim( pMixMotion );
				m_pXSkinAnim->MixAnim( pMixMotion );
				SAFE_CLOSE( pMixMotion );
			}
		}


		m_pXSkinAnimFuture->SetModelDetailPercent( g_pData->GetModelDetailPercent() );
		m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );


		m_pFrame_Bip01				= m_pXSkinAnim->GetCloneFrame( L"Bip01" );
		m_pFrame_Bip01_Head			= m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
		m_pFrame_Bip01_R_Foot		= m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
		m_pFrame_Bip01_L_Foot		= m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
		m_pFrame_Bip01_R_UpperArm	= m_pXSkinAnim->GetCloneFrame( L"Bip01_R_UpperArm" );
		m_pFrame_Bip01_L_UpperArm	= m_pXSkinAnim->GetCloneFrame( L"Bip01_L_UpperArm" );
		m_pFrame_Bip01_Spine		= m_pXSkinAnim->GetCloneFrame( L"Bip01_Spine" );

		SetUnitReForm();

		m_LuaManager.EndTable();
	}
}

void CX2GUUser::InitEqip()
{

	//{{AFX
	g_pData->GetItemManager()->UpdateEqip( GetUnit(), m_pXSkinAnim, m_pMotion, m_pAniXET, OUT m_ViewEqipList );
	//}}AFX




	for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
	{
		CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];

		//{{ robobeg : 2008-10-28
		if ( pCX2Eqip != NULL )
		{
			pCX2Eqip->SetManagerObject( this );
			if ( IsRecursivelyManagedByDGManager() )
				g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2Eqip.get() );
		}//if
		//}} robobeg : 2008-10-28



		ASSERT( pCX2Eqip != NULL );
		ASSERT( pCX2Eqip->GetItem() != NULL );
		ASSERT( pCX2Eqip->GetItem()->GetItemTemplet() != NULL );


		if ( pCX2Eqip != NULL &&
			NULL != pCX2Eqip->GetItem() &&
			NULL != pCX2Eqip->GetItem()->GetItemTemplet()
            )
		{
            const CX2Item::ItemTemplet* pItemTemplet = pCX2Eqip->GetItem()->GetItemTemplet();

			switch( pItemTemplet->GetEqipPosition() )
			{
			case CX2Unit::EP_WEAPON_HAND:
				{
					//{{ robobeg : 2008-10-28
					CX2GameUnit::Weapon* pWeapon = CX2GameUnit::Weapon::CreateWeapon( *this );
					ASSERT( pWeapon != NULL );
					//}} robobeg : 2008-10-28
					pWeapon->m_iWeaponIndex = 0;
					pWeapon->m_pUserWeaponEquip		= pCX2Eqip;
					pWeapon->m_bUseSlashTrace		= true;


					if( NULL != GetUnit() )
					{
						pItemTemplet->GetSlashTraceColors( pWeapon->m_coSlashTrace, pWeapon->m_coSlashTraceTip
							, pWeapon->m_coSlashTraceHyper, pWeapon->m_coSlashTraceTipHyper, GetUnit()->GetType() );
					}


					m_vecpWeapon.push_back( pWeapon );

					if( NULL != g_pX2Game &&
						NULL != g_pX2Game->GetMajorParticle() )
					{
                        unsigned uNumParticle = pItemTemplet->GetNumAttachedParticleData();
						for( UINT j=0; j<uNumParticle; j++ )
						{

                            const wchar_t*  pwszParticleName = L"";
                            const wchar_t*  pwszBoneName = L"";
                            pItemTemplet->GetAttachedParticleData( j, &pwszParticleName, &pwszBoneName );
							CKTDGParticleSystem::CParticleEventSequence* pSeq =
								g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwszParticleName, D3DXVECTOR3( 0, 0, 0 ) );
							if( NULL != pSeq )
							{
								AttachWeaponParticle( pWeapon->m_iWeaponIndex, true, pwszBoneName, pSeq->GetHandle() );
							}
						}
					}


				} break;

			case CX2Unit::EP_WEAPON_SECOND:
				{
					//{{ robobeg : 2008-10-28
					CX2GameUnit::Weapon* pWeapon = CX2GameUnit::Weapon::CreateWeapon( *this );
					ASSERT( pWeapon != NULL );
					//}} robobeg : 2008-10-28
					pWeapon->m_iWeaponIndex = 1;
					pWeapon->m_pUserWeaponEquip		= pCX2Eqip;
					pWeapon->m_bUseSlashTrace		= true;


					if( NULL != GetUnit() )
					{
						pItemTemplet->GetSlashTraceColors( pWeapon->m_coSlashTrace, pWeapon->m_coSlashTraceTip
							, pWeapon->m_coSlashTraceHyper, pWeapon->m_coSlashTraceTipHyper, GetUnit()->GetType() );
					}


					m_vecpWeapon.push_back( pWeapon );


					if( NULL != g_pX2Game &&
						NULL != g_pX2Game->GetMajorParticle() )
					{
                        unsigned uNumParticle = pItemTemplet->GetNumAttachedParticleData();
						for( UINT j=0; j<uNumParticle; j++ )
						{

                            const wchar_t*  pwszParticleName = L"";
                            const wchar_t*  pwszBoneName = L"";
                            pItemTemplet->GetAttachedParticleData( j, &pwszParticleName, &pwszBoneName );
							CKTDGParticleSystem::CParticleEventSequence* pSeq =
								g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwszParticleName, D3DXVECTOR3( 0, 0, 0 ) );
							if( NULL != pSeq )
							{
								AttachWeaponParticle( pWeapon->m_iWeaponIndex, true, pwszBoneName, pSeq->GetHandle() );
							}
						}
					}


				} break;
			}
		}
	}
	CX2GUUser::UpdateNotRenderingOtherEquipList();


	//{{ kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
	UpdateEquipCollisionData( m_ViewEqipList );
	//}} kimhc // 2010.12.10 //  2010-12-23 New Character CHUNG



	//{{ dmlee 2008.04.11 - ���� ĳ�� ������ ability
	if( NULL != g_pX2Game )
	{
		{
			for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
			{
				CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];

				ASSERT( pCX2Eqip != NULL );
				ASSERT( pCX2Eqip->GetItem() != NULL );
				ASSERT( pCX2Eqip->GetItem()->GetItemTemplet() != NULL );

				if ( pCX2Eqip != NULL && pCX2Eqip->GetItem() != NULL &&
                    pCX2Eqip->GetItem()->GetItemTemplet() != NULL
                    )
				{
                    const CX2Item::ItemTemplet* pItemTemplet = pCX2Eqip->GetItem()->GetItemTemplet();
					g_pX2Game->UseItemSpecialAbility( pItemTemplet, this );
				}
			}
		}
	}
	//}} dmlee 2008.04.11 - ���� ĳ�� ������ ability

}

/*virtual*/ void CX2GUUser::UpdateEquipCollisionData()
{

	//{{ kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
	UpdateEquipCollisionData( m_ViewEqipList );
	//}} kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
}


//{{ kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
void CX2GUUser::UpdateEquipCollisionData( IN vector<CX2EqipPtr>& m_vecEquipList )
{
	KTDXPROFILE();
	m_CollisionListSet.clear();
	m_AttackListSet.clear();


	for( int i =0; i < (int)m_vecEquipList.size(); i++ )
	{
		CKTDGXSkinAnim* pAnim = NULL;
		CX2EqipPtr pEqip = m_vecEquipList[i];
		if ( pEqip == NULL )
			continue;

		if( pEqip->GetEqipType() == CX2Item::UT_ATTACH_ANIM )
		{
			for( int j=0; j<CX2Item::MAX_MODEL_COUNT_A_ITEM; j++ )
			{
				pAnim = pEqip->GetNormalAnim(j);
				if( pAnim != NULL)
				{
					if ( false == pAnim->GetCollisionDataList().empty() )
						m_CollisionListSet.insert( &pAnim->GetCollisionDataList() );

					if ( false == pAnim->GetAttackDataList().empty() )
						m_AttackListSet.insert( &pAnim->GetAttackDataList() );
				}
			}
		}
	}

	m_CollisionListSet.insert( &m_pXSkinAnim->GetCollisionDataList() );
	m_AttackListSet.insert( &m_pXSkinAnim->GetAttackDataList() );
}
//}} kimhc // 2010.12.10 //  2010-12-23 New Character CHUNG



void CX2GUUser::UpdateSlashTraceTextureName()
{
	for( UINT i=0; i<m_vecpWeapon.size(); ++i )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		if( NULL == pWeapon )
			continue;

		pWeapon->UpdateSlashTraceTextureName();
	}
}

/** @function : ParseCommonRandomState
	@brief : Start, Win, Lose ������Ʈ�� �����ϰ� �ε��Ѵ�
	@param : �� ������ start, win, lose ������Ʈ�� ����ִ� ������ ���̺���
*/
void CX2GUUser::ParseCommonRandomState( const char* tableName )
{
	if( true == m_LuaManager.BeginTable( tableName ) )
	{
		if( true == m_LuaManager.BeginTable( "START_STATE_LIST" ) )
		{
			CommonRandomState commonRandomState;
			for( int i=1; ; i+=2 )
			{
				if( false == m_LuaManager.GetValueUtf8( i, commonRandomState.m_StateName ) )
					break;

				if( false == m_LuaManager.GetValue( i+1, commonRandomState.m_iPercent ) )
					break;

				m_vecRandomStartState.push_back( commonRandomState );
			}

			m_LuaManager.EndTable(); // tableName.c_str()
		}



		if( true == m_LuaManager.BeginTable( "WIN_STATE_LIST" ) )
		{
			CommonRandomState commonRandomState;
			for( int i=1; ; i+=2 )
			{
				if( false == m_LuaManager.GetValueUtf8( i, commonRandomState.m_StateName ) )
					break;

				if( false == m_LuaManager.GetValue( i+1, commonRandomState.m_iPercent ) )
					break;

				m_vecRandomWinState.push_back( commonRandomState );
			}

			m_LuaManager.EndTable(); // tableName.c_str()
		}



		if( true == m_LuaManager.BeginTable( "LOSE_STATE_LIST" ) )
		{
			CommonRandomState commonRandomState;
			for( int i=1; ; i+=2 )
			{
				if( false == m_LuaManager.GetValueUtf8( i, commonRandomState.m_StateName ) )
					break;

				if( false == m_LuaManager.GetValue( i+1, commonRandomState.m_iPercent ) )
					break;

				m_vecRandomLoseState.push_back( commonRandomState );
			}

			m_LuaManager.EndTable(); // tableName.c_str()
		}

		m_LuaManager.EndTable(); // tableName.c_str(), ELSWORD_SWORDMAN
	}
}



void CX2GUUser::InitState()
{
	UserUnitStateDataMap::iterator iter;
	for( iter = m_StateList.begin(); iter != m_StateList.end(); iter++ )
	{
		UserUnitStateData* pData = &(iter->second);
		if( pData->StateInit != NULL )
			pData->StateInit();
	}
}


void CX2GUUser::ProcessAcceleraterBuff( CX2SkillTree::SKILL_ID eSkillID, bool bHyperMode )
{
	if( NULL == GetUnit() )
		return;


	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
		return;

	D3DXVECTOR3 vMyPos = GetPos();
	vMyPos.y += 100.0f;

	switch ( eSkillID )
	{


	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SPEED_ACCEL:

	// ��ȣ, ����, �¸��� ���� �߰�
	case CX2SkillTree::SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_CRITICAL_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_ADDATK_ACCEL:

		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingX", vMyPos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingY", vMyPos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingZ", vMyPos );

			/// �׼����������� ��� �߰� �ð��� ����
			ApplyAuraAcceleratorBuffToGameUnit( pSkillTemplet, GetAdditionalAccelBuffTime( eSkillID ), vMyPos );
		} break;
	default:
		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingX", vMyPos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingY", vMyPos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BuffRingZ", vMyPos );

			ApplyAuraAcceleratorBuffToGameUnit( pSkillTemplet, 0.f, vMyPos );
		}
		break;
	}
}

/** @function : ApplyAcceleratorBuffToGameUnit
	@brief : �׼��������͹����� �ڽſ��� ����
	@param : �ټ��������ͽ�ų���ø�(pSkillTemplet_), �ܺο���(fAddtionalAccelBuffTime_)
*/
void CX2GUUser::ApplyAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ )
{
	/// ������ �ɸ����ִ� ��������, ����� �׼��������Ϳ� �������Ͱ� �ִ��� �˻�
	if ( CanApplyBuffToGameUnit() && false == pSkillTemplet_->m_vecBuffFactorPtr.empty() )
	{
		if ( NULL == GetUnit()  )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

		CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet_->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );
		if ( NULL != ptrBuffFactorClone )
		{
			ptrBuffFactorClone->SetGameUnitBuffFactor( this );	/// ������������ �������� ����
			ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, fAddtionalAccelBuffTime_ );
			if ( SetBuffFactorToGameUnit( *ptrBuffFactorClone ) )
			{
				const WCHAR* pwsCoreParticleName = CX2SkillTree::GetAccelCoreParticleName( pSkillTemplet_->m_eID );
				ASSERT( NULL != pwsCoreParticleName );

				CKTDGParticleSystem::CParticleEventSequence* pBuffPart = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwsCoreParticleName, vMyPos_ );
				if ( NULL != pBuffPart )
					pBuffPart->SetBlackHolePosition( vMyPos_ );
			}
		}
	}
}

/** @function : ApplyAuraAcceleratorBuffToGameUnit
	@brief : �׼��������͹����� Ư�� �������� ���ֿ��� ����
	@param : �ټ��������ͽ�ų���ø�(pSkillTemplet_), �ܺο���(fAddtionalAccelBuffTime_)
*/
/*virtual*/ void CX2GUUser::ApplyAuraAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ )
{
	if ( false == pSkillTemplet_->m_vecBuffFactorPtr.empty() )
	{
		if ( NULL == GetUnit()  )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

		CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet_->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );

		if ( NULL != ptrBuffFactorClone )
		{
			ptrBuffFactorClone->SetGameUnitBuffFactor( this );
			ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, fAddtionalAccelBuffTime_ );

			const WCHAR* pwsCoreParticleName = CX2SkillTree::GetAccelCoreParticleName( pSkillTemplet_->m_eID );
			ASSERT( NULL != pwsCoreParticleName );

			for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
			{
				CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
				if( pGameUnit != NULL &&
					pGameUnit->GetTeam() == GetTeam() &&
					GetDistance3Sq( vMyPos_, pGameUnit->GetPos() ) < 490000.0f &&
					pGameUnit->CanApplyBuffToGameUnit() &&
					( (g_pX2Game->GetGameType() == CX2Game::GT_PVP &&
						pGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )	/// PVP������ ���� �� �����鿡�Ը�
						|| pGameUnit->GetUnitUID() == GetUnitUID() ||	/// �ڱ� �ڽſ��Ե�
						/// �׿�(�ڱ��ڽ��� �ƴϰ�, PVP�� ������ �ƴ� ���)���� ��Ƽ������
					( NULL != g_pData && NULL != g_pData->GetPartyManager() && true == g_pData->GetPartyManager()->IsMyPartyMember( pGameUnit->GetUnitUID() ) ) ) )
				{
					if ( pGameUnit->SetBuffFactorToGameUnit( *ptrBuffFactorClone ) )
					{
						D3DXVECTOR3 vGameUnitPos = pGameUnit->GetPos();
						vGameUnitPos.y += 100.0f;
						g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwsCoreParticleName, vGameUnitPos );
					}
				}
			}	// for
		}
	}
}

/** @function : CanApplyBuffToGameUnit
	@brief : ������ �ɸ��� �ִ� �������� �˾Ƴ��� �Լ�
	@return : �ɸ��� ������ true, ������ false
*/
/*virtual*/ bool CX2GUUser::CanApplyBuffToGameUnit() const
{
	/// GameUnit���� üũ �� ���� true �� ��� �ٽ� �ѹ� üũ
	if ( CX2GameUnit::CanApplyBuffToGameUnit()
		&& GetNowStateID() != USI_DIE_FRONT && GetNowStateID() != USI_DIE_BACK )
		return true;
	else
		return false;	/// GameUnit���� üũ�� ���� false�� �ٷ� false�� ���� ��
}

void CX2GUUser::InitEquippedActiveSkillState( bool bOnlySkillLevel /*= false*/ )
{
	ASSERT( NULL != GetUnit() );

	if( NULL == GetUnit() )
		return;

	CX2Unit::UnitData& kUnitData = GetUnit()->AccessUnitData();

	CalcEquippedSkillLevelPlusAndMPCost( kUnitData );

	if( false == bOnlySkillLevel )
	{
		SetEquippedSkillFunc( kUnitData, false );		// SetEquippedSkillLevel()�� ȣ���ϱ����� �ؾ��Ѵ�.
		SetEquippedSkillFunc( kUnitData, true );
	}

	m_ComboAndSkillChainSystem.ResetStateIdMatchedToChain();
	m_ComboAndSkillChainSystem.ResetChaining();
	m_ComboAndSkillChainSystem.InitSkillStateChained( this );

	SetEquippedSkillLevelCommon( kUnitData, true );
	SetEquippedSkillLevelCommon( kUnitData, false );

	SetEquippedSkillLevel( CX2SkillTree::SI_NONE, true );
}
void CX2GUUser::InitPassiveSkillState()
{
	InitAdditionalPassive();	/// �нú�� ������ ȿ���� ���� �ʱ�ȭ

	m_vecDelegateProcessInAttackResult.resize( 0 );
	m_vecDelegateProcessInDamageReact.resize( 0 );
	m_vecDelegateProcessInCommonFrameMove.resize( 0 );
	m_vecDelegateProcessWithDamageDataInDamageReact.resize( 0 );

	// kimhc // 2009-11-27 // �нú� ��ų �ʱ�ȭ �ÿ� ability�� �׵������ �ʱ�ȭ
	m_SkillRelatedData.SetForceDownValueRateOfMeleeAttack( 1.f );
	m_SkillRelatedData.SetForceDownValueRateOfRangeAttack( 1.f );

	int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_RF_INDUCE_DESIRE_WINNING, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_RF_INDUCE_DESIRE_WINNING );

		if( NULL != pSkillTemplet )
		{
			float fAddForceDownRate = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL, iSkillLevel ) );

			m_SkillRelatedData.SetForceDownValueRateOfMeleeAttack( m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack() + fAddForceDownRate );
			m_SkillRelatedData.SetForceDownValueRateOfRangeAttack( m_SkillRelatedData.GetForceDownValueRateOfRangeAttack() + fAddForceDownRate );
		}
	}

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_RF_HARDEN_BODY, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_RF_HARDEN_BODY );

		if( NULL != pSkillTemplet )
		{
			const float fDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevel );
			m_AdditionalFinalDefenceMultiplier.SetPassiveMultiple( m_AdditionalFinalDefenceMultiplier.GetPassiveMultiple() * fDamageRate );
		}
	}

	// kimhc // 2009-11-27 // �нú� ��ų �ʱ�ȭ �ÿ� ability�� �׵������ �ʱ�ȭ
	m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent = 0;

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_GET_CHANCE, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_GET_CHANCE );
		if( NULL != pSkillTemplet )
		{
			m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent +=
				(int) (pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel ) * 100.f );
		}
	}

	/// ��� ��ų ������ ��ȸ
	const CX2SkillTree::SkillTemplet* pSkillTempletChanceToReverse =
		GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_CHANCE_TO_REVERSE, GetUnit()->GetUnitData().m_byMemberShipGrade );
	if ( NULL != pSkillTempletChanceToReverse )
		m_GuildSkillData.m_bLearnChanceToReverse = true;


	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_VIGOR_ENERGIZE, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ALH_VIGOR_ENERGIZE );
		if( NULL != pSkillTemplet )
		{
			float fAddForceDownRate = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL, iSkillLevel ) );

			m_SkillRelatedData.SetForceDownValueRateOfMeleeAttack( m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack() + fAddForceDownRate );
		}
	}

	m_EnchantStatFromPassiveSkill.Init();
	const int iSkillLevelElementalDefenceA = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A, true );

	if ( 0 < iSkillLevelElementalDefenceA )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_A );
		if( NULL != pSkillTemplet )
		{
			m_EnchantStatFromPassiveSkill.m_fDefBlaze	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefWater	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WATER_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefNature	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_NATURE_DEFENCE_ABS, iSkillLevel );
		}
	} // if

	const int iSkillLevelElementalDefenceB = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B, true );

	if ( 0 < iSkillLevelElementalDefenceB )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_ELEMENTAL_DEFENCE_B );
		if( NULL != pSkillTemplet )
		{
			m_EnchantStatFromPassiveSkill.m_fDefWind	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_WIND_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefLight	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_LIGHT_DEFENCE_ABS, iSkillLevel );
			m_EnchantStatFromPassiveSkill.m_fDefDark	+=	pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DARK_DEFENCE_ABS, iSkillLevel );
		}
	} // if
	// ���� ��ü
	{
		const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SPK_BODY_OF_FIRE;
		const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				m_EnchantStatFromPassiveSkill.m_fDefBlaze += pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FIRE_DEFENCE_ABS, iSkillLevel );
			}
		}
	}

	AddEnchantStatFromPassiveSkillToDefenceEnchantData();
	AddExtraDamageImmuneRateFromPassiveSkillToImmuneData();
}

void CX2GUUser::ChangeEquippedSkillState( const int iSkillSlotId_, const int iSkillId_ )
{
	ASSERT( NULL != GetUnit() );
	if ( NULL == GetUnit() )
		return;

	CX2Unit::UnitData& kUnitData = GetUnit()->AccessUnitData();

	// iSkillSlotId_�� B�������� �ƴ���..
	bool bSlotB = false;
	// B�����̸� B������ ���� ������ ���� ���ڷ� �Ѱ��� ���̵𿡼� ���� (0~3) ������ ���� Index�� ����
	int iSlotIndex = 0;

	CX2UserSkillTree::GetSlotIndexAndSlotB( IN iSkillSlotId_, OUT iSlotIndex, OUT bSlotB );
	const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( iSkillId_ );
	const int iSkillLevel = kUnitData.m_UserSkillTree.UpdateEquippedSkillLevelFromAcqureidMap( eSkillId, iSlotIndex, bSlotB );

	// ��ų ���� ������ ��Ÿ�� ����
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillId );
	if ( NULL != pSkillTemplet )
	{
		kUnitData.m_UserSkillTree.SetSkillCoolTimeLeft( eSkillId, pSkillTemplet->GetSkillCoolTimeValue( iSkillLevel ) );
	}
	ChangeEquippedSkillState( kUnitData, eSkillId, iSlotIndex, bSlotB );

}

void CX2GUUser::ChangeEquippedSkillState( CX2Unit::UnitData& kUnitData_, const CX2SkillTree::SKILL_ID eSkillId_, const int iSlotIndex_, const bool bSlotB_ )
{
	// �����ۿ����� ��ų������ ���� �ǰų� MP ���� ���� �Ȱ� ó��
	CalcEquippedSkillLevelPlusAndMPCostBySlotID( kUnitData_.m_UserSkillTree, iSlotIndex_, bSlotB_ );

	// ��ų ���� ó��
	SetEquippedSkillFuncBySkillSlotIndex( eSkillId_, iSlotIndex_, bSlotB_ );

	// ������ ��ų�� Level�� ����� ���� ����(PowerRate ��)
	SetEquippedSkillLevelCommon( kUnitData_, bSlotB_ );

	// ������ ��ų�� SubState �� ����(�ʻ�Ⱑ �ΰ� �̻��� ������Ʈ�� �̷���� �͵�)
	SetEquippedSkillLevel( eSkillId_, false );
}

void CX2GUUser::CalcEquippedSkillLevelPlusAndMPCostBySlotID( CX2UserSkillTree& cUserSkillTree_, const int iSlotIndex_, const bool bSlotB_ )
{
	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = cUserSkillTree_.GetSkillSlot( iSlotIndex_, bSlotB_ );

	if( NULL != pSkillSlotData )
	{
		const int iEquippedSkillLevelPlus = GetSkillLevelUpNum( pSkillSlotData->m_eID );


		float fMpConsumption = GetActualMPConsume( pSkillSlotData->m_eID, pSkillSlotData->m_iSkillLevel + iEquippedSkillLevelPlus );

		cUserSkillTree_.SetMpConsumption( iSlotIndex_, bSlotB_, fMpConsumption );


		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );
		if ( NULL != pSkillTemplet )
		{
			const USHORT usAdditionalCBConsumption = GetActualCBConsume( pSkillSlotData->m_eID, pSkillSlotData->m_iSkillLevel + iEquippedSkillLevelPlus );
			cUserSkillTree_.SetCBConsumption( iSlotIndex_, bSlotB_, usAdditionalCBConsumption );

			cUserSkillTree_.SetFPConsumption( iSlotIndex_, bSlotB_, pSkillTemplet->m_usFPConsumtion );
			cUserSkillTree_.SetFPGainCount( iSlotIndex_, bSlotB_, pSkillTemplet->m_usFPGainCount );
		}


		cUserSkillTree_.SetEquippedSkillLevelPlusBySlotIndex( iEquippedSkillLevelPlus, iSlotIndex_, bSlotB_ );
		cUserSkillTree_.UpdateEquippedSkillLevelFromAcqureidMap( pSkillSlotData->m_eID, iSlotIndex_, bSlotB_ );
	}
}

// �����ۿ� ���� �������� ��ų�� �������� ���
void CX2GUUser::CalcEquippedSkillLevelPlusAndMPCost( CX2Unit::UnitData& kUnitData )
{
	CX2UserSkillTree& cUserSkillTree = kUnitData.m_UserSkillTree;

	for( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT*2; ++i )
	{
		bool bSlotB = (i > 3) ? true : false;
		int iSlotIndex = (i > 3) ? i-4 : i;
		CalcEquippedSkillLevelPlusAndMPCostBySlotID( cUserSkillTree, iSlotIndex, bSlotB );
	}
}

/** @function : SetEquippedSkillLevelCommon
	@brief : ��ų ���� ������Ʈ�� ��ųID, ��ų ����, �Ŀ�����Ʈ ���� ������Ʈ�����Ϳ� �ʱ�ȭ
	@param : UnitData*, B�������� ����
*/
void CX2GUUser::SetEquippedSkillLevelCommon( const CX2Unit::UnitData& kUnitData_, bool bSlotB_ )
{
    const CX2UserSkillTree& userSkillTree = kUnitData_.m_UserSkillTree;

	for( int i=0; i< EQUIPPED_SKILL_SLOT_COUNT; i++ )
	{
		const CX2UserSkillTree::SkillSlotData* pSkillSlotData = userSkillTree.GetSkillSlot( i, bSlotB_ );
		ASSERT( NULL != pSkillSlotData );
		if( NULL == pSkillSlotData )
			continue;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );
		if( NULL == pSkillTemplet )
			continue;

		int normalStateID = 0;
		int hyperStateID = 0;

		if( bSlotB_ == false )
		{
			normalStateID	= _CONST_GUUSER_::aNormalSkillStateID[i];
			hyperStateID	= _CONST_GUUSER_::aHyperSkillStateID[i];
		}
		else
		{
			normalStateID	= _CONST_GUUSER_::aNormalSkillStateIDSlotB[i];
			hyperStateID	= _CONST_GUUSER_::aHyperSkillStateIDSlotB[i];
		}



		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		UserUnitStateData& normalStateData = m_StateList[ normalStateID ];
		normalStateData.m_SPLevel = pSkillSlotData->m_iSkillLevel + pSkillSlotData->m_iSkillLevelPlus;
		normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( pSkillSlotData->m_iSkillLevel + pSkillSlotData->m_iSkillLevelPlus );
		normalStateData.m_eSkillID = pSkillTemplet->m_eID;


		UserUnitStateData& hyperStateData = m_StateList[ hyperStateID ];
		hyperStateData.m_SPLevel = pSkillSlotData->m_iSkillLevel + pSkillSlotData->m_iSkillLevelPlus;
		hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( pSkillSlotData->m_iSkillLevel + pSkillSlotData->m_iSkillLevelPlus );
		hyperStateData.m_eSkillID = pSkillTemplet->m_eID;

		CX2ComboAndSkillChainInfoPtr ptrComboAndSkillChainInfo
			= m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( pSkillTemplet->m_eID );
		if ( NULL != ptrComboAndSkillChainInfo )
		{
			ptrComboAndSkillChainInfo->SetStateIdMatchedToChain( normalStateID );
			ptrComboAndSkillChainInfo->SetHyperStateIdMatchedToChain( hyperStateID );

			normalStateData.m_ptrComboAndSkillChainInfo = ptrComboAndSkillChainInfo;
			hyperStateData.m_ptrComboAndSkillChainInfo = ptrComboAndSkillChainInfo;
		}
	}
}


const CX2SkillTree::SkillTemplet* CX2GUUser::GetEquippedActiveSkillTemplet( CX2SkillTree::SKILL_ID eSkillID ) const
{
	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

	bool bSlotB = false;
	int iSkillSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( eSkillID, bSlotB );
	if( iSkillSlotIndex < 0 )
	{
		bSlotB = true;
		iSkillSlotIndex = pUnitData->m_UserSkillTree.GetSkillEquippedSlotIndex( eSkillID, bSlotB );
		if( iSkillSlotIndex < 0 )
		{
			return pUnitData->m_UserSkillTree.GetUserSkillTemplet( eSkillID );;
		}
	}


	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( iSkillSlotIndex, bSlotB );
	if( NULL == pSkillSlotData )
		return pUnitData->m_UserSkillTree.GetUserSkillTemplet( eSkillID );;

	const CX2SkillTree::SkillTemplet* pSkillTemplet
		= g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );

	if ( NULL == pSkillTemplet )
		pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( eSkillID );

	return pSkillTemplet;
}

bool CX2GUUser::GetEqippedSkillMemo(CX2SkillTree::SKILL_MEMO_ID eMemoId) const
{
#if X2_DEV_TOOLS
	if( g_pInstanceData != NULL )
	{
		if( g_pInstanceData->GetFrameScale() == true )
		{
			return true;
		}
	}
#endif

	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

	return pUnitData->m_UserSkillTree.GetEqipSkillMemo( eMemoId );
}


//{{ kimhc // 2009-11-18 // ��彺ų�� ������ Action_ID ó���� ���� ����

///*virtual*/ bool	CX2GUUser::SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet )
//{
//	switch ( pSkillTemplet->m_eID )
//	{
//	case CX2SkillTree::SI_GP_COMMON_CHANCE_TO_REVERSE:	// ������ ��ȸ
//		{
//			// 50% Ȯ�� �ߵ� ó��
//			if ( pSkillTemplet != NULL &&
//				 m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_bProcessingBuff == false &&
//				 GetRandomFloat() > 0.5f )
//				 m_FrameDataFuture.syncData.nowAction = UAI_GUILD_CHANGE_TO_REVERSE;
//		}
//		break;
//
//	default:
//		return false;
//	}
//
//	return true;
//}

//void	CX2GUUser::NoStateChangeGuildActionFrameMove()
//{
//	switch ( m_FrameDataNow.syncData.nowAction )
//	{
//	case UAI_GUILD_SACRIFICE_OF_HERO:
//		break;
//
//	default:
//		{
//			if( m_FrameDataFuture.syncData.nowState == GUSI_DIE )
//				return;
//		}
//	}
//
//	switch ( m_FrameDataNow.syncData.nowAction )
//	{
//	case UAI_GUILD_CHANGE_TO_REVERSE:
//		{
//			const CX2Unit::UnitData*	pUnitData	= &GetUnit()->GetUnitData();
//
//			const CX2SkillTree::SkillTemplet* pSkillTemplet = pUnitData->m_UserSkillTree.GetSkillTemplet( CX2SkillTree::SI_GP_COMMON_CHANCE_TO_REVERSE,
//				pUnitData->m_byMemberShipGrade );
//
//			if ( pSkillTemplet != NULL )
//			{
//				m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_bProcessingBuff	= true;
//				m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_byCountLeft		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT );
//				m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_fTimeLeft			= 30.0f;
//
//				if ( m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_hSeq == INVALID_PARTICLE_SEQUENCE_HANDLE )
//					m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"BuffAttack", 0,0,0, 0, 0 );
//
//				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_SkillRelatedData.m_TimedNCountedCriticalBuff.m_hSeq );
//				if( NULL != pSeq )
//				{
//					pSeq->SetEmitRate( CMinMax<float>(1000,1000) );
//					pSeq->SetPosition( GetPos() );
//				}
//
//			}
//		}
//		break;
//
//	default:
//		break;
//	}
//}

//}} kimhc // 2009-11-18 // ��彺ų�� ������ Action_ID ó���� ���� ����


bool CX2GUUser::CheckSkillUseCondition( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition, CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseConditionSkillTemplet )
{


	if( true == GetEntangled() )	// ������ ���ʱۿ� �ɸ��� ��ų ������
		return false;

	if( GetNoSkillTime() > 0.f )	// silence ������ ��
		return false;

	if( CX2SkillTree::ASUT_NONE == eActiveSkillUseCondition ||
		CX2SkillTree::ASUT_NONE == eActiveSkillUseConditionSkillTemplet )
	{
		return false;
	}


	switch( eActiveSkillUseCondition )
	{
	case CX2SkillTree::ASUT_AIR:
		{
			switch( eActiveSkillUseConditionSkillTemplet )
			{
			default:
				{
					return false;
				} break;

			case CX2SkillTree::ASUT_AIR_ONLY:
			case CX2SkillTree::ASUT_AIR:
				{
				} break;
			}
		} break;

	case CX2SkillTree::ASUT_ANYTIME:
		{
			if( CX2SkillTree::ASUT_ANYTIME_EXCEPT_AIR == eActiveSkillUseConditionSkillTemplet )
			{
				if( false == IsOnSomethingFuture() )
					return false;
			}
			else if( eActiveSkillUseCondition != eActiveSkillUseConditionSkillTemplet )
			{
				return false;
			}
		} break;

	case CX2SkillTree::ASUT_GROUND:
		{
			switch( eActiveSkillUseConditionSkillTemplet )
			{
			case CX2SkillTree::ASUT_AIR_ONLY:
			case CX2SkillTree::ASUT_ANYTIME:
				{
					return false;
				} break;
			}
		} break;
	}

	return true;
}



bool CX2GUUser::CheckSkillUsePVPOfficial( CX2SkillTree::SKILL_ID eSkill_ID_ )
{
	bool bCheck = true;

	if( NULL != g_pX2Game && CX2Game::GT_PVP != g_pX2Game->GetGameType() )
		return bCheck;

	bool bIsOfficalChannel = false;	// ���� �����ΰ�? ���� ������ �ƴϰ� bCheck �� false �̸� ���� �������� �Ǵ��Ѵ�.
	// ���� ä��, ���� ä���� �ƴϸ� ������� ���ϰ� �Ѵ�.
	if( g_pMain->GetConnectedChannelID() != KPVPChannelInfo::PCC_E3 &&	// ���� ä��
		g_pMain->GetConnectedChannelID() != KPVPChannelInfo::PCC_E5 )	// ���� ä��
	{
		if( CX2SkillTree::SI_ETC_WS_COMMON_LOVE == eSkill_ID_ )
		{
			if ( g_pMain->GetConnectedChannelID() == KPVPChannelInfo::PCC_OFFICIAL )
				bIsOfficalChannel = true;
			bCheck = false;
		}
	}

	if( false == bCheck )
	{
		if ( false == bIsOfficalChannel )
		{	// ���� ������ �ƴ� ���, �� ���� ������ ��
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_28982 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
		}
		else
		{	// ���� ���� �� ���
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_24675 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
		}
	}

	return bCheck;
}

void CX2GUUser::DoStateEndStartFuture()
{
	KTDXPROFILE();

	if( m_FrameDataFuture.syncData.stateChangeNum != m_FrameDataFuture.unitCondition.stateChangeNum )
	{

		bool bTableOpen = m_LuaManager.BeginTableByReference( m_FutureStateData.stateID );
		{
			m_FrameDataFuture.syncData.stateChangeNum = m_FrameDataFuture.unitCondition.stateChangeNum ;
			if( m_FutureStateData.StateEndFuture != NULL )
				m_FutureStateData.StateEndFuture();
			else
				CommonStateEndFuture();

		}

		if( true == bTableOpen )
			m_LuaManager.EndTable();



		m_FrameDataFuture.syncData.nowState			= m_FrameDataFuture.unitCondition.nextState;
		m_FrameDataFuture.syncData.nowSubState		= m_FrameDataFuture.unitCondition.nextSubState;
		//m_FrameDataFuture.unitCondition.nextState	= GUSI_NONE;

		m_FutureStateData			= m_StateList[m_FrameDataFuture.syncData.nowState];
		//m_FutureStateData.stateID	= m_FrameDataFuture.syncData.nowState;	/// FieldFix: �̰� �ʿ� �Ѱ�?? �̰͸� ������ �׳� ������ �������� ����ص� �ɵ�.
		//{{ robobeg : 2008-10-28
		//m_StateTableNameFuture		= m_FutureStateData.stateTable;
		//}} robobeg : 2008-10-28


		bTableOpen = m_LuaManager.BeginTableByReference( m_FutureStateData.stateID );
		{
			if( m_FutureStateData.StateStartFuture != NULL )
				m_FutureStateData.StateStartFuture();
			else
				CommonStateStartFuture();
		}

		if( true == bTableOpen )
			m_LuaManager.EndTable();

	}
}

void CX2GUUser::DoStateEndStart()
{
	KTDXPROFILE();

	if( m_FrameDataNow.syncData.stateChangeNum != m_FrameDataNow.unitCondition.stateChangeNum )
	{
		m_FrameDataNow.unitCondition.stateChangeNum = m_FrameDataNow.syncData.stateChangeNum;

		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );
		{
			if( m_NowStateData.StateEnd != NULL )
				m_NowStateData.StateEnd();
			else
				CommonStateEnd();

		}

		if( true == bTableOpen )
			m_LuaManager.EndTable();


		//m_FrameDataNow.unitCondition.nextState	= GUSI_NONE;

		m_NowStateData			= m_StateList[m_FrameDataNow.syncData.nowState];
		m_NowStateData.stateID	= m_FrameDataNow.syncData.nowState;
		//{{ robobeg : 2008-10-28
		//m_StateTableName		= m_NowStateData.stateTable;
		//}} robobeg : 2008-10-28

		bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );
		{
			if( m_NowStateData.StateStart != NULL )
				m_NowStateData.StateStart();
			else
				CommonStateStart();

		}
		if( true == bTableOpen )
			m_LuaManager.EndTable();

	}
}

void CX2GUUser::DoFrameMoveFuture()
{
	KTDXPROFILE();


	bool bTableOpen = m_LuaManager.BeginTableByReference( m_FutureStateData.stateID );
	{
		if( m_FutureStateData.OnFrameMoveFuture != NULL )
			m_FutureStateData.OnFrameMoveFuture();
		else
			CommonFrameMoveFuture();

		if( m_FutureStateData.OnEventProcess != NULL )
			m_FutureStateData.OnEventProcess();
	}

	if( true == bTableOpen )
		m_LuaManager.EndTable();

}

void CX2GUUser::DoFrameMove()
{
	KTDXPROFILE();


	bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );
	{
		if( m_NowStateData.OnFrameMove != NULL )
			m_NowStateData.OnFrameMove();
		else
			CommonFrameMove();
	}

	if( true == bTableOpen )
		m_LuaManager.EndTable();


}





HRESULT CX2GUUser::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	CX2GameUnit::OnFrameMove( fTime, fElapsedTime );

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
    IncrementLocalRelativePosTimers( fElapsedTime );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

	m_fConnectTimeOut += m_fElapsedTime;

	if( m_fSocketSuperArmor > 0.f )
	{
		m_fSocketSuperArmor -= fElapsedTime;
		if( m_fSocketSuperArmor <= 0.f )
			m_fSocketSuperArmor = 0.f;
	}

	if( g_pX2Room != NULL )
	{
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( GetUnitUID() );
		if( pSlotData != NULL )
		{
			// fix!! ���⿡�� �̷��� pointer�� ��ü���ָ� �� ������ �ƴ� �� ������...
			if( pSlotData->m_pUnit != GetUnit() )
			{
				StateLog( L"who am I? possible slotdata error" );
				ASSERT( !"who am I" );
				m_pUnit = pSlotData->m_pUnit;
			}
		}
		else
		{
			//�̷� ���� ����� �ȵ�. ����� ���� ��ü ����� ����� ó�� �ؾ� ��

			ASSERT( !"my slot data is null" );
			m_pUnit = NULL;

			ErrorLogMsg( XEM_ERROR1, L"my slot data is null!!!" );
		}
	}

	if( m_fConnectTimeOut >= 10.0f )
	{
		if( m_bConnectSend == false )
		{
			if( m_bConnect == false )
			{
				//�� ������ P2P ���ӿ� �����ߴ�
				KEGS_GAME_P2P_CONNECT_RESULT_REQ kPacket;
				kPacket.m_UnitUID			= m_UnitUID;
				kPacket.m_bConnectSuccess	= false;
				//g_pData->GetServerProtocol()->SendPacket( EGS_GAME_P2P_CONNECT_RESULT_REQ, kPacket );
				ErrorLogMsg( XEM_ERROR87, L"P2PFail" );
			}
			else
			{
				//�� ������ P2P ���ӿ� �����ߴ�
				KEGS_GAME_P2P_CONNECT_RESULT_REQ kPacket;
				kPacket.m_UnitUID			= m_UnitUID;
				kPacket.m_bConnectSuccess	= true;
				//g_pData->GetServerProtocol()->SendPacket( EGS_GAME_P2P_CONNECT_RESULT_REQ, kPacket );
			}

			m_bConnectSend = true;
		}
	}

	if( g_pKTDXApp->GetIsNowVeryfy() == true )
	{
		Verify();
	}

	if( GetNowHp() <= 0.f )
	{
		m_fDieAfterTime += m_fElapsedTime;
		m_fRebirthTime -= m_fElapsedTime;
		if( m_fRebirthTime < 0.0f )
			m_fRebirthTime = 0.0f;
	}

	if ( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->GetMatrix().Scale( m_vTransScale.GetVector3() * m_fBasicBoneScale );
	}

	if ( m_pXSkinAnimFuture != NULL )
	{
		m_pXSkinAnimFuture->GetMatrix().Scale( m_vTransScale.GetVector3() * m_fBasicBoneScale );
	}

	if( m_NowStateData.stateID == GUSI_DIE )
	{
		return S_OK;
	}

	//{{ kimhc // 2009-11-20 // ��� ��ų 3, 4����
	if ( GetNowMp() > 0.0f )
	{
		// ��彺ų 3����, ������ ü��
		if ( m_GuildSkillData.m_InfiniteStrength.m_byCountLeft != 0 )
		{
			if ( m_GuildSkillData.m_InfiniteStrength.IsElaspedTimeOverGap( fElapsedTime ) == true )
			{
				// ȸ��
				UpNowHp( m_GuildSkillData.m_InfiniteStrength.m_fRecoveryHpOnce );

				if ( m_GuildSkillData.m_InfiniteStrength.m_byCountLeft == 0 )
					PlaySound( L"GS_Infinite_Strength.ogg" );

				std::wstringstream strStream;
				strStream << L"HP+ " << static_cast< int >( m_GuildSkillData.m_InfiniteStrength.m_fRecoveryHpOnce );
				D3DXVECTOR3 pos = GetPos();
				pos.y += 50.0f;
				if( NULL != g_pData->GetPicCharGreen() )
					g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
		}

		if ( m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect != INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect );

			if ( pSeq != NULL )
			{
				D3DXVECTOR3 pos = GetPos();
				pos.y += 60.0f;
				pSeq->SetBlackHolePosition( pos );
				if( pSeq->GetTime() > 1.5f )
				{
					pSeq->SetGravity( CMinMax<D3DXVECTOR3>(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) );
					pSeq->SetUseLand( false );
				}
			}
			else
			{
				m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}
	}
	//}} kimhc // 2009-11-20 // ��� ��ų 3, 4����


//{{ kimhc // 2010.4.6 // ��д��� �۾�(��Ʈ������ȿ��)
	CheckNChangeManaRecoveryValueNTime( fElapsedTime );
	CheckNChangeIntervalTimeForCureDebuff( fElapsedTime );
	CheckAndDeleteBoneShield( fElapsedTime );


//}} kimhc // 2010.4.6 // ��д��� �۾�(��Ʈ������ȿ��)

	//////////////////////////////////////////////////
	// Future
	//////////////////////////////////////////////////
	if( IsLocalUnit() == true )
	{
		ResetNotBusyTimerOnSpecificState();

		// �����ð����� random seed�� �����ؼ� sync�� �����.
		// ������ ���°� �ٲ� ���� random seed�� �ٲ�µ� ���°� �ٲ��� ������ random seed�� �ٲ��� �ʴ� ������ �־
		// �ϴ��� ���°� �ٲ� �� random seed�� ���� �����ϴ� �ڵ嵵 �״�� ���ܵд�
		if( m_TimerRandomSeedSync.elapsed() > CX2GameUnit::RANDOM_SEED_RESET_TIME )
		{
			m_TimerRandomSeedSync.restart();

			m_FrameDataFuture.syncData.m_usRandomTableIndex = (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );
		}


		if( m_FrameDataFuture.unitCondition.fStopTime <= 0.0f &&
			m_FrameDataFuture.unitCondition.fStop2Time <= 0.0f )
		{
			m_FrameDataFuture.syncData.bFrameStop = false;

			DoStateEndStartFuture();
			DoFrameMoveFuture();
		}
		else
		{
			m_FrameDataFuture.syncData.bFrameStop = true;

			m_FrameDataFuture.unitCondition.fStopTime -= fElapsedTime;
			if( m_FrameDataFuture.unitCondition.fStopTime < 0.0f )
				m_FrameDataFuture.unitCondition.fStopTime = 0.0f;
			m_FrameDataFuture.unitCondition.fStop2Time -= fElapsedTime;
			if( m_FrameDataFuture.unitCondition.fStop2Time < 0.0f )
				m_FrameDataFuture.unitCondition.fStop2Time = 0.0f;
		}

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
        if ( IsMyUnit() == true )
        {
            PhysicProcess_UpdateRelativePos();
        }
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        PushFrameData( ( g_pX2Game->IsHost() == true && g_pX2Game->IsPacketSendFrame() == true ) ? true : false );
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//		PushFrameData();
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
	}

	//////////////////////////////////////////////////
	// Now
	//////////////////////////////////////////////////

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
    bool    bPopOnce = false;
    bool    bPopAgain = false;
    m_bPopAgain = false;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
    bool    bFrameBufferPass = false;
	do
	{
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        bFrameBufferPass = false;
        bPopAgain = PopFrameData( bPopOnce, bFrameBufferPass );
        if ( bPopAgain == true )
        {
            m_bPopAgain = true;
            bPopOnce = true;
        }
        else
        {
            m_bPopAgain = false;
        }
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//		PopFrameData();
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        if( bFrameBufferPass == false )
		{
			DoStateEndStart();
			DoFrameMove();
		}

		// weapon
		for( UINT i=0; i<m_vecpWeapon.size(); i++ )
		{
			Weapon* pWeapon = m_vecpWeapon[i];
			pWeapon->OnFrameMove( fTime, fElapsedTime );
		}

	} while( m_bPopAgain == true );


	// �������� ó��
	m_fDoubleAttackTime += fElapsedTime;
	if(m_fDoubleAttackTime >= 0.2f && m_bDoubleAttack == true )
	{
		DamageDoubleAttack();
	}

	if( m_fMaxDownForce > -9999.f &&
		g_pX2Game->GetGameType() == CX2Game::GT_PVP )
	{
		m_fLastDamagePassTime = 0.f;
		AddForceDownGage( m_fMaxDownForce );
	}
	m_fMaxDownForce = -9999.f;

	if( m_fSlotChangeLeftCoolTime > 0.f )
		m_fSlotChangeLeftCoolTime -= fElapsedTime;

	return S_OK;
}

HRESULT CX2GUUser::OnCameraMove()
{


	g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = D3DXVECTOR3( 0, 0, 0 );


	if( true == g_pX2Game->GetFixedCamera() )
	{
		CX2World* pWorld = g_pX2Game->GetWorld();
		CKTDGLineMap* pLineMap = pWorld->GetLineMap();
		if( NULL != pWorld && NULL != pLineMap )
		{
			g_pX2Game->GetX2Camera()->FixedCamera( this, pWorld->GetLookAt(), pWorld->GetEye(), pLineMap->GetXMin(), pLineMap->GetXMax(),
				pLineMap->GetYMin(), pLineMap->GetYMax() );
		}
	}
	else
	{

		if( g_pX2Game != NULL && g_pX2Game->GetWorld() != NULL )
		{
			int iCameraIndex = g_pX2Game->GetWorld()->GetTriggerCamera();
			if( iCameraIndex >= 0 )
			{
				const CKTDGLineMap::CameraData* pCameraData = g_pX2Game->GetWorld()->GetLineMap()->GetCameraDataList( iCameraIndex );
				if( pCameraData != NULL )
				{
					g_pX2Game->GetX2Camera()->GameLineCamera( this, pCameraData->m_vEye, pCameraData->m_vLookAt, pCameraData->m_bFocusUnit, 0.4f );
					return S_OK;
				}
			}
		}

		if( g_pMain->GetGameOption().GetOptionList().m_bDynamicCamera == false &&
			g_pX2Game != NULL && g_pX2Game->GetWorld() != NULL && g_pX2Game->GetWorld()->GetLineMap() != NULL )
		{
			if( 0.f < fPreWorldNear )
			{ // ���̳��� ī�޶� ���� SetNear���� 10���� ���� �� ���¶�� ����
				g_pKTDXApp->GetDGManager()->SetNear(fPreWorldNear);
				fPreWorldNear = -1.f;

				g_pKTDXApp->GetDGManager()->SetProjection(
					g_pKTDXApp->GetDGManager()->GetNear(),
					g_pKTDXApp->GetDGManager()->GetFar(), true );
			}

			const CKTDGLineMap::CameraData* pCameraData = g_pX2Game->GetWorld()->GetLineMap()->GetCameraData( m_FrameDataFuture.syncData.lastTouchLineIndex );
			if( NULL != pCameraData )
			{

				switch( pCameraData->m_eType )
				{
				case CKTDGLineMap::LCT_ABSOULTE_POSITION:
					{
						g_pX2Game->GetX2Camera()->GameLineCamera( this, pCameraData->m_vEye, pCameraData->m_vLookAt, pCameraData->m_bFocusUnit, 0.4f );
					} break;
				case CKTDGLineMap::LCT_RELATIE_POSITION:
					{
						g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = pCameraData->m_vRelativeEye;
						g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_fSpeed = pCameraData->m_fCameraRepositionSpeed;

						g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() );

					} break;

				default:
					{
						g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() );

					} break;
				}
			}
			else
			{

				if( m_GameCameraOffset.bEnable == true )
				{
					g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() + m_GameCameraOffset.fCameraDistance,
																		200.f + m_GameCameraOffset.fHeight,
																		0.f + m_GameCameraOffset.fAngleDegree,
																		0.f + m_GameCameraOffset.fEyeDistance,
																		0.f + m_GameCameraOffset.fLookatDistance,
																		0.3f + m_GameCameraOffset.fTrackingTime,
																		0.0f + m_GameCameraOffset.fTrackingAtTime );

				}
				else
					g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() );
			}
		}
		else
		{
			if( true == m_LuaManager.BeginTableByReference( m_NowStateData.stateID ) )
			{
				// �ٸ� ī�޶� ���갡 �ִ� ��� �ڽ��� ���� �Ǵ� �Ÿ��� �����Ÿ� ���� �ִ� ������ �ƴϸ� ī�޶� �ߵ� ����
				if( m_NowStateData.OnCameraMove != NULL )
					//&& ( IsMyUnit() || GetDistance3Sq( g_pX2Game->GetMyUnit()->GetPos(), GetPos() ) ) )
					m_NowStateData.OnCameraMove();
				else
				{
					if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
						m_ptrRidingPet->CommonCameraFrameMove();
					else
						CommonCameraMove();
				}



				m_LuaManager.EndTable();
			}
		}
	}

	return S_OK;
}


//{{ robobeg : 2008-10-28

/*virtual*/
RENDER_HINT CX2GUUser::OnFrameRender_Prepare()
{
	if ( m_bFirstDataReceive == false )
		return RENDER_HINT_NORENDER;

	bool bTableOpen = false;
	RENDER_HINT renderHint;
	//{{ robobeg : 2008-10-28
	//bTableOpen = m_LuaManager.BeginTable( m_StateTableName.c_str() );
	bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );
	//}} robobeg : 2008-10-28
	{
		renderHint = CommonRender_Prepare();
	}
	if( bTableOpen == true )
		m_LuaManager.EndTable();

	return renderHint;
}//CX2GUUser::OnFrameRender_Prepare()


/*virtual*/
RENDER_HINT CX2GUUser::CommonRender_Prepare()
{
	if ( m_pXSkinAnim == NULL )
		return RENDER_HINT_NORENDER;

	m_pXSkinAnim->SetInstantShow( false );
	if ( !m_pXSkinAnim->GetShowObject() )
		return RENDER_HINT_NORENDER;

	CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
	*pRenderParam = m_RenderParam;



	//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
	// ExtraDamage Pain�� ���� ���̸�
	if ( 0 < m_ExtraDamagePack.m_Pain.m_Accumulation )
		pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON_FADE;
	//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

	//{{ kimhc // 2009-12-29 // ������ ����� �ߵ� ���̸� RT_CARTOON_FADE_NONTEX�� ����
	if ( m_bHaveSacrificeOfHero == true )
		pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON_FADE_NONTEX;
	//}} kimhc // 2009-12-29 // ������ ����� �ߵ� ���̸� RT_CARTOON_FADE_NONTEX�� ����

	if( g_pX2Game->GetWorld() != NULL && m_bUseWorldColor == true )
	{
		pRenderParam->lightPos = g_pX2Game->GetWorld()->GetLightPos();
	}

	if( false == m_bSetCustomRenderParam )
		ApplyRenderParam( pRenderParam );

	if( GetDistanceToCamera() > 700.0f )
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 1000.0f ;
	}
	else
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 500.0f ;
	}

	if( IsSuperArmor() && m_FrameDataNow.stateParam.bSuperArmorNotRed == false)
	{
		if( pRenderParam->renderType == CKTDGXRenderer::RT_CARTOON_BLACK_EDGE )
		{
			pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
			pRenderParam->outLineColor = 0xffff0000;
		}
	}

	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_MEDIUM )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
	}
	else if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_LOW )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
	}

	if( m_DamageLightTime > 0.0f && g_pMain->GetGameOption().GetOptionList().m_UnitDetail != CX2GameOption::OL_LOW )
	{
		if( m_FrameDataNow.unitCondition.bAttackerFront == true )
		{
			if( m_FrameDataNow.syncData.bIsRight == true )
			{
				pRenderParam->lightPos = GetPos() + GetDirVector() * 1000.0f;
				pRenderParam->lightPos -= GetPos();
			}
			else
			{
				pRenderParam->lightPos = GetPos() - GetDirVector() * 1000.0f;
				pRenderParam->lightPos -= GetPos();
			}
		}
		else
		{
			if( m_FrameDataNow.syncData.bIsRight == true )
			{
				pRenderParam->lightPos = GetPos() - GetDirVector() * 1000.0f;
				pRenderParam->lightPos -= GetPos();
			}
			else
			{
				pRenderParam->lightPos = GetPos() + GetDirVector() * 1000.0f;
				pRenderParam->lightPos -= GetPos();
			}
		}

		pRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_DAMAGE;

		switch( pRenderParam->renderType )
		{
		case CKTDGXRenderer::RT_CARTOON_BLACK_EDGE:
			pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_BLACK_EDGE;
			break;

		case CKTDGXRenderer::RT_CARTOON_COLOR_EDGE:
			pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_COLOR_EDGE;
			break;

		case CKTDGXRenderer::RT_CARTOON:
			pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON;
			break;
		}
	}


	if( m_fShowInvincibleTime > 0.0f && m_bShowInvincibleWhite == true )
	{
		pRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_INVINCIBLE;
		pRenderParam->renderType		= CKTDGXRenderer::RT_ADD_CARTOON;
	}

	//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
	// EDT_PAIN�� �ɷ������� �ٸ� EDT�� ���� �� ������ �Ͼ�� �ʴ´�.
	if ( m_ExtraDamagePack.m_Pain.m_Accumulation == 0 )
	//}} 2011-04 �� ��ġ�� û ĳ�� ��ų
	{
		//Ư�������� üũ
		if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			pRenderParam->renderType			= CKTDGXRenderer::RT_CARTOON_FADE;
			m_RenderParam.color = D3DXCOLOR( 160.f/255.f, 235.f/255.f, 1.f, 0.7f );
		}
		else if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f )
		{
			pRenderParam->renderType			= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
			pRenderParam->cartoonTexType		= CKTDGXRenderer::CTT_PETRIFIED;
			pRenderParam->outLineColor			= D3DXCOLOR( 0.7f, 0.7f, 0.7f, 1.f );
		}
		else if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f ||
			m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.0f )
		{
			pRenderParam->color = D3DXCOLOR(0.15f, 0.8f, 1.f, 1.f );
		}
		else if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f ||
			m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
		{
			pRenderParam->color = 0xffff0000;
		}
		else if( m_ExtraDamagePack.m_Poison.m_fTime  > 0.0f )
		{
			pRenderParam->color = 0xff00ff00;
		}
		else if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f )
		{
			pRenderParam->color = 0xff6600ff;
		}
		else if( m_fPepperRunTime > 0.f )
		{
			pRenderParam->color = 0xffff0000;
		}
	}

	if( m_bUnderWaterHead == true || m_bForceChagneColor == true )
	{
		pRenderParam->color.r -= m_cLineUnitColor.r;
		pRenderParam->color.g -= m_cLineUnitColor.g;
		pRenderParam->color.b -= m_cLineUnitColor.b;
	}


	pRenderParam->color.a = m_RenderParam.color.a;
	if ( g_pX2Room != NULL )
	{
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( m_UnitUID );
		if( pSlotData != NULL
			&& pSlotData->GetSlotState() != CX2Room::SS_CLOSE
			&& pSlotData->GetSlotState() != CX2Room::SS_EMPTY
			&& pSlotData->m_pUnit != GetUnit() )
			m_pUnit = pSlotData->m_pUnit;
	}//if



	RENDER_HINT renderHintResult = m_pXSkinAnim->OnFrameRender_Prepare();
	if ( renderHintResult.m_eHint != RENDER_HINT::NORENDER )
		m_pXSkinAnim->SetInstantShow( true );

	if ( !IsRecursivelyManagedByDGManager() && !m_ViewEqipList.empty() )
	{
		RENDER_HINT renderHint;
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			ASSERT( pCX2Eqip != NULL && !pCX2Eqip->IsManagedByDGManager() );
			if ( pCX2Eqip == NULL || pCX2Eqip->IsManagedByDGManager() )
				continue;
			pCX2Eqip->SetInstantShow( false );
			if ( !pCX2Eqip->GetShowObject() )
				continue;
			renderHint = pCX2Eqip->OnFrameRender_Prepare();
			if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
			{
				pCX2Eqip->SetInstantShow( true );
				renderHintResult.MergeRenderHint( renderHint );
			}//if
		}//for
	}//if

	return  renderHintResult;

}//CX2GUUser::CommonRender_Prepare()


/*virtual*/
void    CX2GUUser::OnFrameRender_Draw()
{
	KTDXPROFILE();

	if ( m_pXSkinAnim == NULL || !m_pXSkinAnim->GetShowObject() )
		return;

	if(m_bHideNoRenderable == true)
	{
		m_pXSkinAnim->SetHideNoRenderable(true);
	}
	if ( m_pXSkinAnim->IsInstantShow() )
	{
		m_pXSkinAnim->OnFrameRender_Draw();
	}
	if(m_bHideNoRenderable == true)
	{
		m_pXSkinAnim->SetHideNoRenderable(false);
	}

	if ( !IsRecursivelyManagedByDGManager() )
	{
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			ASSERT( pCX2Eqip != NULL && !pCX2Eqip->IsManagedByDGManager() );
			if ( pCX2Eqip == NULL || pCX2Eqip->IsManagedByDGManager() )
				continue;
			if ( pCX2Eqip->GetShowObject() && pCX2Eqip->IsInstantShow() )
				pCX2Eqip->OnFrameRender_Draw();
		}//if
	}//if

	//{{ robobeg : 2008-10-28

	// �޺� �Ŵ����� ������������ ���� �ƹ� �ϵ� ���� �ʴ´�.
	// CX2GUUser::OnFrameRender_Draw ���� ĳ���� ��Ų�� �������Ѵٴ� ���� ��Ȯ�� �ϱ� ����
	// �Ʒ� �ڵ带 ����

	//if ( g_pX2Game->GetMyUnit() == this )
	//{
	//	if( NULL != m_pComboManager )
	//		m_pComboManager->OnFrameRender();
	//}
	//}} robobeg : 2008-10-28
}

//}} robobeg : 2008-10-28


void CX2GUUser::PreRenderForce()
{
	/** �� �Լ��� ������ �޽� �����͸� pre-load�ϴ� ��������,
	BeginScene()���� ���� ���¿���, DrawPrimitive()�� ȣ���ϹǷ�
	Invalid�ϴ�. - jintaeks on 2008-10-15, 14:19 */
	if ( NULL != m_pXSkinAnim )
		m_pXSkinAnim->OnFrameRender();

	if ( !IsRecursivelyManagedByDGManager() )
	{
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];

			//if( pCX2Eqip->GetEqipType() == CX2Item::ET_SKIN )
			//    continue;
			if ( pCX2Eqip == NULL )
				continue;
			ASSERT( !pCX2Eqip->IsManagedByDGManager() );
			pCX2Eqip->OnFrameRender();
		}
	}//if

	/* //{{ seojt on 2008-10-18
	// �Ʒ��� ���� �����Ͽ�����, D3D ������ memory leak�� �����Ǿ� �ϴ� ��ó��
	// ������� �ǵ��� �ξ���. background thread loading �۾��� ������
	// �ٽ� ������ ����. - jintaeks on 2008-10-18, 12:03
	m_pXSkinAnim->PreLoad();

	for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
	{
	CX2Eqip* pCX2Eqip = m_ViewEqipList[i];
	if ( pCX2Eqip == NULL )
	continue;

	if( pCX2Eqip->GetEqipType() == CX2Item::ET_SKIN )
	continue;
	pCX2Eqip->PreLoad();
	}
	/**/ //}} seojt on 2008-10-18
}//CX2GUUser::PreRenderForce()


HRESULT CX2GUUser::OnResetDevice()
{

	if ( !IsRecursivelyManagedByDGManager() )
	{
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			//{{ robobeg : 2008-10-28
			if ( pCX2Eqip == NULL )
				continue;
			ASSERT( !pCX2Eqip->IsManagedByDGManager() );
			pCX2Eqip->OnResetDevice();
			//}} robobeg : 2008-10-28

		}//for
	}//if







	return S_OK;
}

HRESULT CX2GUUser::OnLostDevice()
{
	if ( !IsRecursivelyManagedByDGManager() )
	{
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			//{{ robobeg : 2008-10-28
			if ( pCX2Eqip == NULL )
				continue;
			ASSERT( !pCX2Eqip->IsManagedByDGManager() );
			pCX2Eqip->OnLostDevice();
			//}} robobeg : 2008-10-28
		}
	}//if



	return S_OK;
}

bool CX2GUUser::StateChange( int state, bool bDirectChange )
{
	KTDXPROFILE();


///	if ( GetGrap() == true ) return;

	m_ePlayedEmotion = CX2Unit::ET_NONE;

	if( IsLocalUnit() == false )
		return false;

	if ( GetNowHp() > 0.f )
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			return false;
		}
	}


	m_FrameDataFuture.unitCondition.nextState			= state;
	m_FrameDataFuture.unitCondition.nextSubState		= -1;
	m_FrameDataFuture.syncData.bDirectChange			= bDirectChange;


	m_FrameDataFuture.syncData.m_usRandomTableIndex		= (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );
	m_FrameDataFuture.unitCondition.stateChangeNum++;
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#ifdef OPTIMIZED_P2P
//    if( m_FrameDataFuture.unitCondition.stateChangeNum == 7 )
//        m_FrameDataFuture.unitCondition.stateChangeNum = 0;
//#else
//	if( m_FrameDataFuture.unitCondition.stateChangeNum == 15 )
//		m_FrameDataFuture.unitCondition.stateChangeNum = 0;
//#endif // OPTIMIZED_P2P
//#endif SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	GetGageData()->SetEnable( false );


	if( bDirectChange == true )
	{
		m_LuaManager.EndTable();


		//m_FrameDataFuture.syncData		= m_FrameDataNow.syncData;
		//m_FrameDataFuture.unitCondition = m_FrameDataNow.unitCondition;
		//m_FrameDataFuture.stateParam	= m_FrameDataNow.stateParam;

		int receiveSize	= (int)m_ReceiveSyncDataList.size();
		int sendSize	= (int)m_SendSyncDataList.size();

		map< int, UCHAR > mapActionSyncData;
		GetActionSyncData( mapActionSyncData, m_ReceiveSyncDataList );


		m_ReceiveSyncDataList.resize(0);
		m_SendSyncDataList.resize(0);
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        int lastSentSize = (int) m_LastSendSyncDataList.size();
        DWORD dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		DoStateEndStartFuture();

		//m_StopAdvanceTimeFuture = true;
		for( int i = 0; i < receiveSize; i++ )
		{
			DoFrameMoveFuture();
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            ++dwFrameMoveCount;
            m_FrameDataFuture.syncData.dwFrameMoveCount = dwFrameMoveCount;
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
		}

		SetActionSyncData( m_ReceiveSyncDataList, mapActionSyncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

        int iMinSize = __min( lastSentSize, receiveSize );
        if ( iMinSize > 0 )
        {
            std::vector<SyncData>::iterator iterDst = m_LastSendSyncDataList.end() - iMinSize;
            std::vector<SyncData>::const_iterator iterSrc = m_ReceiveSyncDataList.end() - iMinSize;
            for( int i = 0; i < iMinSize; i++ )
            {
                *(iterDst++) = *(iterSrc++);
            }
        }

		m_adwInitFrameMoveCount[0] = dwFrameMoveCount - receiveSize + 1;
		m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		for( int i = 0; i < sendSize; i++ )
		{
			DoFrameMoveFuture();
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            ++dwFrameMoveCount;
            m_FrameDataFuture.syncData.dwFrameMoveCount = dwFrameMoveCount;
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			m_SendSyncDataList.push_back( m_FrameDataFuture.syncData );
		}
		//m_StopAdvanceTimeFuture = false;
		//m_pXSkinAnimFuture->OnFrameMove( m_fTime, m_fElapsedTime * (receiveSize+sendSize) );
	}
	GetGageData()->SetEnable( true );

	// �Ʒü� ����
	m_iNowSpecialAttack = 0;

	SetSlotChangeCoolTime(0.5f);
    return true;
}

bool CX2GUUser::StateChange( int state, int subState, bool bDirectChange /* = false */ )
{
	KTDXPROFILE();

	//if ( GetGrap() == true ) 		return;

	m_ePlayedEmotion = CX2Unit::ET_NONE;

	if( IsLocalUnit() == false )
		return false;

	if ( GetNowHp() > 0.f )
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f ||
			false == m_vecFreezePtr.empty() )
		{
			return false;
		}
	}

	m_FrameDataFuture.unitCondition.nextState			= state;
	m_FrameDataFuture.unitCondition.nextSubState		= subState;
	m_FrameDataFuture.syncData.bDirectChange			= bDirectChange;

	m_FrameDataFuture.syncData.m_usRandomTableIndex = (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );


	m_FrameDataFuture.unitCondition.stateChangeNum++;
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#ifdef OPTIMIZED_P2P
//    if( m_FrameDataFuture.unitCondition.stateChangeNum == 7 )
//        m_FrameDataFuture.unitCondition.stateChangeNum = 0;
//#else
//	if( m_FrameDataFuture.unitCondition.stateChangeNum == 15 )
//		m_FrameDataFuture.unitCondition.stateChangeNum = 0;
//#endif // OPTIMIZED_P2P
//#endif SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


	GetGageData()->SetEnable( false );


	if( bDirectChange == true )
	{
		m_LuaManager.EndTable();
		bool bTableOpen = false;

		//m_FrameDataFuture.syncData		= m_FrameDataNow.syncData;
		//m_FrameDataFuture.unitCondition = m_FrameDataNow.unitCondition;
		//m_FrameDataFuture.stateParam	= m_FrameDataNow.stateParam;

		int receiveSize	= (int)m_ReceiveSyncDataList.size();
		int sendSize	= (int)m_SendSyncDataList.size();
		map< int, UCHAR > mapActionSyncData;
		GetActionSyncData( mapActionSyncData, m_ReceiveSyncDataList );
		m_ReceiveSyncDataList.resize(0);
		m_SendSyncDataList.resize(0);
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        int lastSentSize = (int) m_LastSendSyncDataList.size();
        DWORD dwFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		DoStateEndStartFuture();

		//m_StopAdvanceTimeFuture = true;
		for( int i = 0; i < receiveSize; i++ )
		{
			DoFrameMoveFuture();
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            ++dwFrameMoveCount;
            m_FrameDataFuture.syncData.dwFrameMoveCount = dwFrameMoveCount;
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
			m_ReceiveSyncDataList.push_back( m_FrameDataFuture.syncData );
		}

		SetActionSyncData( m_ReceiveSyncDataList, mapActionSyncData );
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

        int iMinSize = __min( lastSentSize, receiveSize );
        if ( iMinSize > 0 )
        {
            std::vector<SyncData>::iterator iterDst = m_LastSendSyncDataList.end() - iMinSize;
            std::vector<SyncData>::const_iterator iterSrc = m_ReceiveSyncDataList.end() - iMinSize;
            for( int i = 0; i < iMinSize; i++ )
            {
                *(iterDst++) = *(iterSrc++);
            }
        }

		m_adwInitFrameMoveCount[0] = dwFrameMoveCount - receiveSize + 1;
		m_adwInitFrameMoveCount[1] = dwFrameMoveCount;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		for( int i = 0; i < sendSize; i++ )
		{
			DoFrameMoveFuture();
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            ++dwFrameMoveCount;
            m_FrameDataFuture.syncData.dwFrameMoveCount = dwFrameMoveCount;
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			m_FrameDataFuture.syncData.bDirectChange = bDirectChange;
			m_SendSyncDataList.push_back( m_FrameDataFuture.syncData );
		}
		//m_StopAdvanceTimeFuture = false;
		//m_pXSkinAnimFuture->OnFrameMove( m_fTime, m_fElapsedTime * (receiveSize+sendSize) );
	}
	GetGageData()->SetEnable( true );

	// �Ʒü� ����
	m_iNowSpecialAttack = 0;

	SetSlotChangeCoolTime(0.5f);
    return true;
}

void CX2GUUser::DamageReact( CX2DamageManager::DamageData* pDamageData )
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	KTDXPROFILE();
	//{{ kimhc // 2010.2.17 //
	m_bApplyMotionOffset = pDamageData->m_bApplyMotionOffset;
	//}} kimhc // 2010.2.17 //

	m_bHitedDrainMana = false;
	m_bDoubleAttack = false;
	m_FrameDataNow.unitCondition.bAttackerFront = true;


	/// DT_BUFF�� ��, ���� �Ұ��� ���� �Ǿ� ������ ���� ���� ����
	if ( true != GetNoDetonation() || CX2DamageManager::DT_BUFF != pDamageData->damageType )
		SetNoDetonation( pDamageData->m_bNoDetonation );

	m_bNowStateDamageReact = true;

	float	fMpPet			= 0.f;

	m_bParryingState = false;

	m_bQuickenGuardBlocking = false;
	m_bQuickenGuardTurning = false;


	if( pDamageData->bForceFly == true )
	{
		m_FlyCount = 0;
	}
	else
	{
		switch( pDamageData->reActResult )
		{
		case CX2DamageManager::RT_UP:
		case CX2DamageManager::RT_FLY:

//{{ kimhc // 2010.2.8 // ��ġ���� ���� ���� ��� ���� �ø��� ����
		case CX2DamageManager::RT_DRAG_UP:
//}} kimhc // 2010.2.8 // ��ġ���� ���� ���� ��� ���� �ø��� ����
			{
				m_FlyCount++;
			} break;
		}
	}

    CX2DamageEffect::CEffect*   pAttackerEffect = g_pX2Game->GetDamageEffect()->GetInstance( pDamageData->hAttackerEffect );


	CX2GameUnit*	pAttackerUnit = NULL;
	D3DXVECTOR3		dirUnitToAttacker = D3DXVECTOR3(0,0,0);

	if( null != pDamageData->optrAttackerGameUnit )
	{
		pAttackerUnit		= pDamageData->optrAttackerGameUnit.GetObservable();
	}

	if( pAttackerUnit != NULL )
	{
		switch(pDamageData->attackerType)
		{
		case CX2DamageManager::AT_UNIT:
			{
				dirUnitToAttacker	= pAttackerUnit->GetPos() - GetPos();
			} break;
		case CX2DamageManager::AT_MESH:
			{
				dirUnitToAttacker	= pDamageData->pAttackerAnim->GetMatrix().GetPos() - GetPos();
			} break;
		case CX2DamageManager::AT_MANUAL_COLLISION_DATA:
			{
				dirUnitToAttacker	= pDamageData->pCollisionData->GetPointStart() - GetPos();
			} break;
		case CX2DamageManager::AT_EFFECT:
			{
				if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = ( pAttackerEffect != NULL ) ? pAttackerEffect->GetMainEffect() : NULL )
					dirUnitToAttacker	= pMeshInstance->GetMatrix().GetPos() - GetPos();
			} break;
		default:
			{
				ASSERT( !"Unexpected Attacker Type" );
			} break;
		}
	}

	//{{ oasis907 : ����� [2010.2.11] //
	bool IsDamageDirRight = false;
	if ( pDamageData->bArrangedFly == true )
	{
		if( pAttackerUnit )
		{
			switch(pDamageData->attackerType)
			{
			case CX2DamageManager::AT_UNIT:
				{
					IsDamageDirRight =pAttackerUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_MESH:
				{
					IsDamageDirRight = pAttackerUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_MANUAL_COLLISION_DATA:
				{
					IsDamageDirRight = pAttackerUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_EFFECT:
				{
					if( pAttackerEffect != NULL )
						IsDamageDirRight = pAttackerEffect->GetFollowLineRight();
				} break;
			default:
				{
					ASSERT( !"Unexpected Attacker Type" );
				} break;
			}
		}
	}
	//}} oasis907 : ����� [2010.2.11] //








	if( m_FrameDataNow.stateParam.bInvincible == true &&
		pDamageData->m_fHpPercentUp == 0.f &&
		pDamageData->m_fMpPercentUp == 0.f &&
		CX2DamageManager::RT_THROW != pDamageData->reActType )
	{
		pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
		return;
	}

	//ĳ���� ���� ���� �� ������ ���� �ʵ��� ����
	if( true == m_bEnterCashShop )
	{
		pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
		return;
	}

	switch( pDamageData->reActType )
	{
	case CX2DamageManager::RT_CREATE_DAMAGE_EFFECT:
	case CX2DamageManager::RT_CREATE_EFFECT_SET:
		{
			if( IsSuperArmor() != true && pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE && pDamageData->fStopTimeDef > 0.f )
			{
				SetStopTime( pDamageData->fStopTimeDef );
				SetStop2Time( 0.0f );
			}
			return;
		} break;
	}




	m_optrLastAttackUnit.Reset();

	if( pAttackerUnit != NULL && pDamageData->m_bDrainHpParticle == true )
	{
		if( CX2GameUnit::GUT_USER == pAttackerUnit->GetGameUnitType() )
		{
			if( m_hSeqDrainParticle == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				D3DXVECTOR3 pos = GetPos();
				pos.y = pos.y + 100.0f;

				m_optrLastAttackUnit = pAttackerUnit;

				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GameUnitCreateSequence_LUA( this, "DrainLight",	pos, D3DXVECTOR2(-1,-1), D3DXVECTOR2(3,-1) );
				if( pSeq != NULL )
				{
					D3DXVECTOR3 pos = m_optrLastAttackUnit->GetWeaponBonePos( 0, L"TRACE_END0", 0 );

					pSeq->SetLandPosition( GetLandPosition().y );
					pSeq->SetBlackHolePosition( pos );
					m_hSeqDrainParticle = pSeq->GetHandle();
				}
			}
		}
	}

	if ( AbsorbEffectAttackProcess( pDamageData ) )
		return;

// #ifdef ELSWORD_SHEATH_KNIGHT
// 	if( pAttackerUnit != NULL ) // Valid üũ�� ������ ��
// 	{
// 		if ( pDamageData->attackerType == CX2DamageManager::AT_UNIT &&
// 			pAttackerUnit->GetGameUnitType() == GUT_USER &&
// 			pDamageData->attackType == CX2DamageManager::AT_NORMAL )
// 		{
// 			CX2GUUser* pAttackerUser = static_cast<CX2GUUser*>( pAttackerUnit );
// 			if( pAttackerUser->GetUnitClass() == CX2Unit::UC_ELSWORD_SHEATH_KNIGHT
// #ifdef SERV_ELSWORD_INFINITY_SWORD
// 				|| pAttackerUser->GetUnitClass() == CX2Unit::UC_ELSWORD_INFINITY_SWORD
// #endif
// 				)
// 			{
// 				if( GetRandomFloat() <= pAttackerUser->GetBrutalSlayerManaBurnRate() )
// 				{
// 					float fBurnMP = pAttackerUser->GetBrutalSlayerManaBurnABS();
// 					UpNowMp( -fBurnMP );
//
// 					std::wstringstream strStream;
// 					strStream << L"MP -" << (int)fBurnMP;
// 					D3DXVECTOR3 numPos;
// 					GetFramePos( &numPos, m_pFrame_Bip01_Head );
// 					if( numPos.x == 0.0f && numPos.y == 0.0f && numPos.z == 0.0f )
// 					{
// 						numPos = GetPos();
// 						numPos.y += 150.0f;
// 					}
// 					numPos.y += -50.0f;
// 					if( NULL != g_pData->GetPicCharRed() )
// 						g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
//
// #ifdef SERV_ELSWORD_INFINITY_SWORD
// 					pAttackerUser->SetMindOfFighterCount();
// #endif
// 				}
// 			}
// 		}
// 	}
// #endif ELSWORD_SHEATH_KNIGHT


	//���� ���濡 �ִ°� �Ĺ濡 �ִ°�
    dirUnitToAttacker.y = 0.f;

	{
		if( D3DXVec3Dot( &GetDirVector(), &dirUnitToAttacker ) >= 0 )
			m_FrameDataNow.unitCondition.bAttackerFront = true;
		else
			m_FrameDataNow.unitCondition.bAttackerFront = false;

		if( m_FrameDataNow.syncData.bIsRight == false )
		{
			m_FrameDataNow.unitCondition.bAttackerFront = !m_FrameDataNow.unitCondition.bAttackerFront;
		}
	}

	// �������� ���⼺�� ĳ������ ������ ��ġ�ϴ���
	bool bViewDamageDir = false;
	if ( pDamageData->bArrangedFly == true )
	{
		if( IsDamageDirRight == GetIsRight() )
		{
			bViewDamageDir = true;
			m_FrameDataNow.unitCondition.bAttackerFront = false;
		}
		else
		{
			bViewDamageDir = false;
			m_FrameDataNow.unitCondition.bAttackerFront = true;
		}
	}

#ifdef ADD_BACKATTACK
	bool bCriticalHit = false;
	bool bBackAttack = false;
	if( pDamageData->m_bApplyBackAttack == true &&
		IsSuperArmor() != true &&
		m_FrameDataNow.unitCondition.bAttackerFront == false )
	{
		bBackAttack = true;
	}
#endif

	if( IsSuperArmor() != true &&
		pDamageData->reActType != CX2DamageManager::RT_DUMMY_DAMAGE) // oasis907 : ����� [2010.12.3] // ���� �������� �̼ӿ� ���� ���� �ʵ���
	{
		if( pDamageData->m_bWBackSpeed == true )
		{
			if( m_FrameDataNow.syncData.bIsRight == true )
			{
				m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
			}
			else
			{
				m_PhysicParam.nowSpeed.x = pDamageData->backSpeed.x;
			}
		}
//{{ oasis907 : ����� [2010.2.10] //
		else if ( pDamageData->bArrangedFly == true )
		{
			if( bViewDamageDir == true )
			{
				m_PhysicParam.nowSpeed.x = pDamageData->backSpeed.x;
			}
			else
			{
				m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
			}
		}
//}} oasis907 : ����� [2010.2.10] //
		else
		{

			switch( pDamageData->reActType )
			{
			case CX2DamageManager::RT_FLIP_SAME_DIR:
				{
					// �� �Ǵ� ���� ���θ�
					m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
				} break;
			case CX2DamageManager::RT_NO_DAMAGE:
			case CX2DamageManager::RT_DUMMY_DAMAGE:
				{

				} break;
			default:
				{
					if( m_FrameDataNow.unitCondition.bAttackerFront == true )
					{
						m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
					}
					else
					{
						m_PhysicParam.nowSpeed.x = pDamageData->backSpeed.x;
					}
				}
				break;
			}
		}
	}




	m_FrameDataFuture.unitCondition.bAttackerFront = m_FrameDataNow.unitCondition.bAttackerFront;

	bool bRevenge = false;
	bool bReflexMagic = false;

	// �ݰݿ��ΰ˻�
	if( m_FrameDataNow.stateParam.fRevengeStartTime < m_pXSkinAnim->GetNowAnimationTime()
		&& m_FrameDataNow.stateParam.fRevengeEndTime > m_pXSkinAnim->GetNowAnimationTime()
		&& m_FrameDataNow.unitCondition.bAttackerFront == true )
	{
		bRevenge = true;
	}
	// �����ݻ翩�� �˻�
	if( m_FrameDataNow.stateParam.fReflexMagicStartTime < m_pXSkinAnim->GetNowAnimationTime()
		&& m_FrameDataNow.stateParam.fReflexMagicEndTime > m_pXSkinAnim->GetNowAnimationTime()
		&& m_FrameDataNow.unitCondition.bAttackerFront == true )
	{
		bReflexMagic = true;
	}

	bool bDamageReactReflectMagic = false;
	//{{ oasis907 : ����� [2010.11.1] // �� �����̾� - �������� ����
	if( CX2Unit::UC_ELSWORD_RUNE_SLAYER == GetUnitClass() )
	{
		CX2GUElsword_SwordMan* pElsword = (CX2GUElsword_SwordMan*) this;

		if(GetRandomFloat() <= pElsword->GetMagicReflectRate())
		{
			bReflexMagic = true;
			bDamageReactReflectMagic = true;
		}
	}
	//}

	/// ������ ���� ���� ���� ȿ�� �˻�
	if ( false == m_vecReflectMagicByBuffPtr.empty() )
		bReflexMagic = true;

	// �ݰݿ� ���� �ǰ����� ���� �� ����, ���� �ǰ������� ���� �����ֱ� ���� �ӽ� ���� ��
	CX2DamageManager::HIT_TYPE eOldHitType = ( NULL != pDamageData) ? pDamageData->hitType : CX2DamageManager::HT_NO_SOUND;

	if(bRevenge == true || bReflexMagic == true)
	{

		if( (bRevenge == true && pDamageData->bCanRevenge == true) ||
			(bReflexMagic == true && pDamageData->bCanReflexMagic == true) )
		{
			pDamageData->fStopTimeDef				= 0.f;

			m_fRevengeAddDamage					= pDamageData->damage.GetAllDamage();

			if(bDamageReactReflectMagic == true && bRevenge == false )
				pDamageData->reActResult			= CX2DamageManager::RT_REFLECT;
			else
				pDamageData->reActResult			= CX2DamageManager::RT_REVENGE;

			if(bReflexMagic == true && pDamageData->bCanReflexMagic == true)
			{
				float fPowerRate = 1.f;


				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_LE_REFLEX_MAGIC );
				if( NULL != pSkillTemplet )
				{
					if ( NULL == GetUnit()  )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

					fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				}

				/// ������ ���� ���� �ݻ簡 �������̶��
				if ( false == m_vecReflectMagicByBuffPtr.empty() )
				{
					vector<CX2ReflectMagicByBuffPtr>::iterator vit = m_vecReflectMagicByBuffPtr.begin();

					/// �����κ��� �ݻ�ü ������ ������ ��ȯ
					if ( NULL != vit->get() )
						fPowerRate = vit->get()->GetValue();
				}

				if(pAttackerEffect != NULL)
				{
					pAttackerEffect->SetPowerRate( pAttackerEffect->GetPowerRate() * fPowerRate );
				}
			}

			switch( pDamageData->hitType )
			{
			default:
			case CX2DamageManager::HT_SWORD_SLASH:
			case CX2DamageManager::HT_SWORD_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_SWORD_CRASH;
				break;

			case CX2DamageManager::HT_ROD_SLASH:
			case CX2DamageManager::HT_ROD_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_ROD_CRASH;
				break;

			case CX2DamageManager::HT_PUNCH_HIT:
			case CX2DamageManager::HT_PUNCH_HIT2:
				pDamageData->hitType				= CX2DamageManager::HT_PUNCH_CRASH;
				break;

			case CX2DamageManager::HT_KICK_SLASH:
			case CX2DamageManager::HT_KICK_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_KICK_CRASH;
				break;

			case CX2DamageManager::HT_ARROW_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_ARROW_CRASH;
				break;
			}

			m_FrameDataNow.unitCondition.bHit	= true;
		}
		else if( true == bRevenge && false == pDamageData->bCanRevenge &&
				true == m_bCanAvoidance )
		{
			pDamageData->fStopTimeDef				= 0.f;
			pDamageData->reActResult = CX2DamageManager::RT_AVOIDANCE;
		}
	}

	//Ÿ�� ��Ŀ� ���� ����(���� ��)
	wstring wstrHitSound = L"";
	if( pDamageData->reActResult != CX2DamageManager::RT_NO_DAMAGE
		&& pDamageData->reActResult != CX2DamageManager::RT_SENSOR )
		//&& pDamageData->reActResult != CX2DamageManager::RT_REVENGE )
	{
		const WCHAR* wszSoundName = NULL;
	//{{ kimhc // 2011.1.21 // û 1�� ����

		if ( CX2DamageManager::HTD_NO_SOUND != m_eHittedTypeAtState)
			wszSoundName = g_pData->GetDamageManager()->GetDamageSound( pDamageData->hitType, m_eHittedTypeAtState );
		else
	//}} kimhc // 2011.1.21 // û 1�� ����
			wszSoundName = g_pData->GetDamageManager()->GetDamageSound( pDamageData->hitType, m_HittedType );

		if( NULL != wszSoundName )
		{
			PlaySound( wszSoundName );
			wstrHitSound = wszSoundName;
		}

		if( NULL != pDamageData &&
			CX2DamageManager::HT_NO_SOUND != eOldHitType )
		{
			pDamageData->hitType = eOldHitType;
		}

	}


	if( IsFocusUnit() == true )
	{

		switch( pDamageData->m_eCameraShakeType )
		{
		case CKTDGCamera::DECT_UP:
			{
				// fix!! not yet!!

			} break;
		case CKTDGCamera::DECT_DOWN:
			{
				// fix!! not yet!!

			} break;
		case CKTDGCamera::DECT_UP_DOWN:
			{
				UpDownCrashCamera( pDamageData->fCameraCrashGap, pDamageData->fCameraCrashTime );

			} break;
		case CKTDGCamera::DECT_UP_DOWN_NO_RESET:
			{
				UpDownCrashCameraNoReset( pDamageData->fCameraCrashGap, pDamageData->fCameraCrashTime );
			} break;

		case CKTDGCamera::DECT_LEFT:
			{
				// fix!! not yet!!

			} break;

		case CKTDGCamera::DECT_RIGHT:
			{
				// fix!! not yet!!

			} break;

		case CKTDGCamera::DECT_LEFT_RIGHT:
			{
				ShakeLeftRight( pDamageData->fCameraCrashGap, pDamageData->fCameraCrashTime, 0.05f );
			} break;
		case CKTDGCamera::DECT_LEFT_RIGHT_NO_RESET:
			{
				LeftRightCrashCameraNoReset( pDamageData->fCameraCrashGap, pDamageData->fCameraCrashTime );
			} break;



		case CKTDGCamera::DECT_RANDOM:
			{
				// fix!! not yet!!
			} break;
		}

		if ( false == g_pMain->GetGameOption().GetFlashEffect() )
		{
			g_pKTDXApp->GetDGManager()->ClearScreen( pDamageData->clearScreen );
			g_pKTDXApp->GetDGManager()->SetClearScreenColor( pDamageData->clearScreenColor );
		}
	}







	if( true == IsDamagedReaction( pDamageData ) )
	{
		DoCustomFunctionByBuffInDamageReact( *pDamageData );
		const bool bBuffApplied = ApplyBuffToGameUnitInDamageReact( pDamageData );
		PlayEffectSetHittedByBuff();
		DoDelegateProcessInDamageReact();
		DoDelegateProcessWithDamageDataInDamageReact( *pDamageData );

//{{ kimhc // 2010.4.6 // ��д��� �۾�(��Ʈ������ȿ��)

		switch ( pDamageData->reActType )
		{
		case CX2DamageManager::RT_NO_DAMAGE:
		case CX2DamageManager::RT_DUMMY_DAMAGE:
		case CX2DamageManager::RT_GUARD:
		case CX2DamageManager::RT_SENSOR:
			break;

		default:
			{
				ProcessSocketEffectEx( CX2SocketItem::SIEE_CURE_DEBUFF, this );
				ProcessSocketEffectEx( CX2SocketItem::SIEE_DEBUFF_ATTACKER, pAttackerUnit  );

				//{{ kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage
				ProcessSocketEffectEx( CX2SocketItem::SIEE_ICE_NOVA, this  );
				//}} kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage

				ProcessSocketEffectEx( CX2SocketItem::SIEE_POISON_BURST, this  );
				ProcessSocketEffectEx( CX2SocketItem::SIEE_ABSORB_HP, this  );

				ProcessSocketEffectEx( CX2SocketItem::SIEE_ABSORB_HP_NO_REACT, this  );		/// �ǰ� �� HP ���� ��Ʈ ȿ�� ( ���� ���� )
				ProcessSocketEffectEx( CX2SocketItem::SIEE_ACTIVE_BUFF, this  );			/// �ǰ� �� ���� ���� ȿ��

			}
			break;
		}

//}} kimhc // 2010.4.6 // ��д��� �۾�(��Ʈ������ȿ��)

		bool bCheckExtraDamage = false;

		switch( pDamageData->m_ExtraDamage.m_ExtraDamageType )
		{
		case CX2DamageManager::EDT_MANA_DAMAGE:
		case CX2DamageManager::EDT_DAMAGE_RATE:
		case CX2DamageManager::EDT_WATER_HOLD:
		case CX2DamageManager::EDT_ATTACK_ALL_TEAM:
			bCheckExtraDamage = true;
			break;
		default:
			break;
		}

		if( GetUnit() != NULL && GetUnit()->GetType() == CX2Unit::UT_RAVEN &&
			bCheckExtraDamage == false &&
			pDamageData->m_bNoDamage == false &&
			pDamageData->m_fHpPercentUp <= 0.f &&
			pDamageData->m_fIncreaseMP <= 0.f &&
			pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE &&
			pDamageData->reActResult != CX2DamageManager::RT_SENSOR &&
			pDamageData->hitType != CX2DamageManager::HT_GROUND_HIT &&
			GetHold() == false && GetGrap() == false
			&& false == GetRidingOn()
			)
		{
			CX2GURaven *pRaven = (CX2GURaven*)this;
			if( pRaven != NULL &&
				pRaven->IsAttackNasodBall() == false &&
				( pRaven->CanParryingState() == true || pRaven->GetParrying() == true ) &&
				GetRemainHyperModeTime() > 0.f &&
				( pRaven->GetNasodBall() > 0 || pRaven->CanParrying() == true ) &&
				IsSuperArmor() != true &&
				m_FrameDataFuture.unitCondition.bAttackerFront == true &&
				true == pRaven->GetFrameData().unitCondition.bFootOnLine )
			{
				m_bParryingState = true;
// 				pDamageData->fStopTimeDef = 0.f;
// 				pDamageData->m_ExtraDamage.Init();
			}
			else
			{
				DamageReact_ExtraDamage( pDamageData, true );
			}
		}
		else if ( GetUnit() != NULL && GetUnit()->GetType() == CX2Unit::UT_ARA && GetUnit()->GetClass() == CX2Unit::UC_ARA_SAKRA_DEVANAM
			&& bCheckExtraDamage == false && CX2DamageManager::AT_NORMAL == pDamageData->attackType && pDamageData->m_bNoDamage == false
			&& pDamageData->m_fHpPercentUp <= 0.f && pDamageData->m_fIncreaseMP <= 0.f
			&& pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE
			&& pDamageData->reActResult != CX2DamageManager::RT_SENSOR && pDamageData->hitType != CX2DamageManager::HT_GROUND_HIT
			&& GetHold() == false && GetGrap() == false
			&& false == GetRidingOn()
			)
		{
			CX2GUAra *pAra = static_cast<CX2GUAra*>(this);
			if( pAra != NULL && IsSuperArmor() != true && true == pAra->GetFrameData().unitCondition.bFootOnLine )
			{
				float fBlockingRate = pAra->GetBlockingRate();

				switch ( GetNowStateID() )
				{
				case USI_WAIT:
				case USI_WALK:
				case USI_DASH:
					fBlockingRate *= 2.f;
					break;
				default:
					break;
				}
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					fBlockingRate *= 1.f;
					break;
				default:
					break;
				}

				if ( GetRandomFloat() < fBlockingRate && m_FrameDataFuture.unitCondition.bAttackerFront == true )
					m_bQuickenGuardBlocking = true;

				if ( GetRandomFloat() < pAra->GetTurnigRate() && m_FrameDataFuture.unitCondition.bAttackerFront == false )
					m_bQuickenGuardTurning = true;

				if ( true != m_bQuickenGuardBlocking )
					DamageReact_ExtraDamage( pDamageData, true );
			}
			else
			{
				DamageReact_ExtraDamage( pDamageData, true );
			}
		}

		else
		{
			DamageReact_ExtraDamage( pDamageData, true );
		}



		// �������� �Ӽ� ��æƮ extra_damage
		{
			if( false == m_bParryingState && false == m_bQuickenGuardBlocking &&
				false == pDamageData->m_bNoDamage )
			{
				const float fRandomValue1 = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_ENCHANT );
				const float fRandomValue2 = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_ENCHANT + 1 );

				//{{ kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)

				const bool bApplyEnchant = g_pData->GetEnchantItem()->ProcessEnchant( this, pDamageData, fRandomValue1, fRandomValue2 );

				if ( bApplyEnchant == true || bBuffApplied )
				{
					ProcessSocketEffectEx( CX2SocketItem::SIEE_STAT_UP, this  );
					ProcessSocketEffectEx( CX2SocketItem::SIEE_UP_MANA_RECOVERY_VALUE, this  );
				}

				if( true == bApplyEnchant )
					SetSpecificValueByEnchant();

				//}} kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)
			}
		}




		if ( pDamageData->m_bPenetrate == false )
		{
			float fFinalDamagePerSec = 0.0f;

			fFinalDamagePerSec = m_ExtraDamagePack.m_Fire.m_DamagePerSec;
			GetDamageReduceByResistExtraDamage( m_ExtraDamagePack.m_Fire.m_ExtraDamageType, fFinalDamagePerSec );
			m_ExtraDamagePack.m_Fire.m_DamagePerSec = fFinalDamagePerSec;

			fFinalDamagePerSec = m_ExtraDamagePack.m_Lightning.m_DamagePerSec;
			GetDamageReduceByResistExtraDamage( m_ExtraDamagePack.m_Lightning.m_ExtraDamageType, fFinalDamagePerSec );
			m_ExtraDamagePack.m_Lightning.m_DamagePerSec = fFinalDamagePerSec;
		}



		float fPercentMPUp = 0.0f;
		float fMPUp = 0.0f;
		//��Ʈ�����ۿ����� ��ų �ö󰡴� �༮�� �ִ��� �˻��غ���.
		if( false == pDamageData->m_bNoDamage )
		{
			SetSuperArmorSocket();
		}

		if ( IsSuperArmor() == false && m_bSocketSuperArmor == true)
		{
			m_FrameDataNow.stateParam.bSuperArmor = true;
			m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
		}

		//{{ kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����
		//}} kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����



		if ( pAttackerUnit != NULL )
		{
			if ( 0.f >= pDamageData->m_fHpPercentUp && 0.f >= pDamageData->m_fMpPercentUp &&
				0.f >= pDamageData->m_fIncreaseMP )
			{
				//{{ kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

				// ���ݽ� ���� �� HP Percent
				float fHPPercentToBeUp = CalculateHpUpPercentBySocketOption( m_SocketData.m_vecHpUpByAttacked );
				float fHPUp = GetMaxHp() * fHPPercentToBeUp;

				if( 0.0f < fHPUp )
				{
					UpNowHp( fHPUp );

					std::wstringstream strStream;
					strStream << L"HP+ " << (int)fHPUp;
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f;
					if( NULL != g_pData->GetPicCharGreen() )
						g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
				fMPUp = CalculateMpUpBySocketOption( m_SocketData.m_vecMpUpByAttacked );

				//}} kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

				if( 0.f < fMPUp )
				{
					UpNowMp( fMPUp );

					std::wstringstream strStream;
					strStream << L"MP+ " << (int)fMPUp;
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f;
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		}


	}


	if(pDamageData->m_fHpPercentUp > 0.f)
	{
		float fUpRate = GetMaxHp() * pDamageData->m_fHpPercentUp;
		float fHpUp = GetNowHp() + fUpRate;
		SetNowHp( fHpUp );


		std::wstringstream strStream;
		strStream << L"HP+ " << (int)fUpRate;
		D3DXVECTOR3 pos = GetPos();
		pos.y += 50.0f;
		if( NULL != g_pData->GetPicCharGreen() )
			g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

		g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_Heal", (CX2GameUnit*)this );
		PlaySound( L"Nasod_Healer_Heal.ogg" );
	}


	if( pDamageData->m_fMpPercentUp > 0.f )
	{
		float fUpRate = GetMaxMp() * pDamageData->m_fMpPercentUp;
		float fMpUp = GetNowMp() + fUpRate;
		SetNowMp( fMpUp );

		std::wstringstream strStream;
		strStream << L"MP+ " << (int)fUpRate;
		D3DXVECTOR3 pos = GetPos();
		pos.y += 30.0f;
		if( NULL != g_pData->GetPicCharGreen() )
			g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
	}
	// modified by wonpok. 20090821. dummy damage�� ���ؼ��� StopTime�� Ǯ�� �ʴ´�.
	//if( m_FrameDataNow.stateParam.bSuperArmor != true )
	if ( IsSuperArmor() != true && pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE )
	{
		if(pDamageData->reActResult != CX2DamageManager::RT_NO_DAMAGE)
		{
			SetStopTime( pDamageData->fStopTimeDef );
			SetStop2Time( 0.0f );
		}
	}

	m_DamageLightTime = 0.2f;

	if( true == IsDamagedReaction( pDamageData ) &&
		false == pDamageData->m_bNoDamage )
	{





		// �����ǵ�� ���� Ÿ�ݹ��ÿ� ���� ��� ���� ������ �����ؾ���
		// m_iDefenceCount

// 		if ( null != pDamageData->optrAttackerGameUnit )
// 			pDamageData->optrAttackerGameUnit->ModifyFinalDamageByGameUnitType( pDamageData );

		float fFinalDamage = GetFinalDamage( pDamageData );

		// ���̺� �и�
		if( m_bParryingState == true )
		{
			CX2GURaven *pRaven = (CX2GURaven*)this;
			if( pRaven != NULL )
			{
				m_PhysicParam.nowSpeed.x *= 0.7f;
				m_PhysicParam.nowSpeed.y = 0.f;

				fFinalDamage *= ( 1.f - 0.75 );

				D3DXVECTOR3 vDirVec = pRaven->GetDirVector();
				D3DXVECTOR3 vPos = pRaven->GetPos();

				const PROTECT_VECTOR3 &vScale = pRaven->GetScaleByUnit();
				D3DXVECTOR3 vOffset = vScale.x * vDirVec * 70.f;
				float fOffsetY = vScale.y * 100.f;

				vPos = vPos +  vOffset + D3DXVECTOR3( 0.f, fOffsetY, 0.f );

				g_pX2Game->GetEffectSet()->PlayEffectSetWithCustomPos_LUA( "EffectSet_RAVEN_PARRING", this, vPos, vDirVec );

				PlaySound(L"Raven_NasodCore_Guard.ogg");
			}
		}

		float fReducedDamagePercent = GetBasicReducedDamagePercent();

		if ( pAttackerUnit != NULL )
		{
			if ( pAttackerUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{
				CX2GUNPC* pGUNPC = static_cast<CX2GUNPC*>( pAttackerUnit );

				fReducedDamagePercent += GetReducedDamagePercentByMonsterAttack( pGUNPC->GetNPCTemplet().m_nNPCUnitID );
			}
		}

		if ( pDamageData->m_bPenetrate == false )
		{
			fFinalDamage -= fFinalDamage * fReducedDamagePercent;
		}

//{{ kimhc // 2010.4.7 // ��д��� �۾�(��Ʈ������ȿ��)
		if ( m_pBoneShieldData != NULL )
		{
			if ( m_pBoneShieldData->m_fTimeLeft > 0.0f && m_pBoneShieldData->m_byCountLeft > 0 )
			{
				fFinalDamage -= fFinalDamage * m_pBoneShieldData->m_fReducePercent;
				m_pBoneShieldData->m_byCountLeft--;
				g_pX2Game->GetEffectSet()->PlayEffectSet( m_pBoneShieldData->m_wstrEffectName, this, this );
			}
		}
		else
			ProcessSocketEffectEx( CX2SocketItem::SIEE_BONE_SHIELD, this  );
//}} kimhc // 2010.4.7 // ��д��� �۾�(��Ʈ������ȿ��)

		if ( IsMyUnit() )
		{
			SetHittedCount( GetHittedCount() + 1 );
		}

		if( (CX2EnchantItem::ENCHANT_TYPE)pDamageData->m_ResistType != CX2EnchantItem::ET_NONE )
		{
			ApplyEnchantResist( (CX2EnchantItem::ENCHANT_TYPE)pDamageData->m_ResistType, fFinalDamage );
		}

		D3DXVECTOR3 numPos;
		GetFramePos( &numPos, m_pFrame_Bip01_Head );
		numPos.y += 70.0f;


		D3DXVECTOR3 criticalNumPos = numPos;
		criticalNumPos.y += 40.0f;

		//{{ dmlee 2009.5.13 �������¿� ������� ũ��Ƽ�� ������ �ְ� ����, ���̻� ����Ʈ�׺�Ʈ�� ��ü��Ÿ, ���ҵ� �ʻ�� �� �Ϻΰ� AT_MESH, AT_MANUAL_COLLISION_DATA�� ����ϰ� ����
		//if ( ( pDamageData->attackerType == CX2DamageManager::AT_UNIT ||
		//	pDamageData->attackerType == CX2DamageManager::AT_EFFECT ) &&
		//	pDamageData->pAttackerUnit != NULL )
		//{

		bool bShowRedPicture = false;	// ���ҵ� HP�� ���������� ������� �ϴ��� ����
		if( pAttackerUnit != NULL )
		{
		//}} dmlee 2009.5.13 �������¿� ������� ũ��Ƽ�� ������ �ְ� ����, ���̻� ����Ʈ�׺�Ʈ�� ��ü��Ÿ, ���ҵ� �ʻ�� �� �Ϻΰ� AT_MESH, AT_MANUAL_COLLISION_DATA�� ����ϰ� ����

			//{{ 2011-04 �� ��ġ�� û �޸� ��弦
			// ��弦�� ����� ������ �Ǵ� �� ������
			ApplyHeadShotDamage( &fFinalDamage );
			//}} 2011-04 �� ��ġ�� û �޸� ��弦

			float fCriticalRate = pAttackerUnit->GetCriticalPercent( pDamageData->damageType, pDamageData->m_fCriticalRate );

			pAttackerUnit->SetCriticalPercent_WayOfSword( IN pDamageData->m_WayofSwordType, OUT fCriticalRate );

#ifdef ADD_BACKATTACK
			if( bBackAttack == true )
			{
				fCriticalRate *= 1.5f;
			}
#endif

			if( 0.f < m_fAntiCriticalRate )
			{
				fCriticalRate -= m_fAntiCriticalRate;
				if(fCriticalRate < 0.f)
					fCriticalRate = 0.f;
			}

			//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
			if( GetCriticalValueFromDefender() <= fCriticalRate )
				//}} 2011-04 �� ��ġ�� û ĳ�� ��ų
			{

				bShowRedPicture = true;
				float fDefenderCriticalValue = 0.f;

				// ���� ������ ��ݿ� ���ظ� ���� ������ ��쿡 �޴� ũ��Ƽ�� �������� �����Ѵ�
				if( m_ExtraDamagePack.m_StigmaDebuff.m_fTime > 0.f )
				{
					fDefenderCriticalValue = m_ExtraDamagePack.m_StigmaDebuff.m_DamagePerSec;	// ũ��Ƽ�� ������ ���� ������ ���Ѵ�
				}

				// ���� ���ݿ��� ����Ǵ� ũ��Ƽ�� ������ ���� ����
				if( CX2GUUser::GUT_USER	== pAttackerUnit->GetGameUnitType() )
				{
					CX2GUUser* pGUUser = static_cast<CX2GUUser*>(pAttackerUnit);
					if( NULL != pGUUser )
					{
						switch( pDamageData->damageType )
						{
						case CX2DamageManager::DT_MAGIC:
						case CX2DamageManager::DT_MIX:
						case CX2DamageManager::DT_WEAPON_MAGIC:
						case CX2DamageManager::DT_WEAPON_MIX:
							{
								// �⺻ ũ��Ƽ�� ���� ���� + ���� ���� ũ��Ƽ�� ����
								float fCriticalDamageRate = pGUUser->GetCriticalValue( fDefenderCriticalValue );
								fCriticalDamageRate += pGUUser->GetAddMagicAttackCriticalDamageRate();

								fCriticalDamageRate += pDamageData->m_fApplyCriticalDamage ;
								// ũ��Ƽ�� ���� ���� ����
								fCriticalDamageRate *= GetCriticalDefence();

								// FinalDamage �� ũ��Ƽ�� ���� ����
								fFinalDamage *= fCriticalDamageRate;
							} break;
						default:
							{
								float fCritical = pAttackerUnit->GetCriticalValue( fDefenderCriticalValue );

								fCritical += pDamageData->m_fApplyCriticalDamage ;

								// ũ��Ƽ�� ���� ���� ����
								fCritical *= GetCriticalDefence();

								if ( fCritical >= 1.0f )
									fFinalDamage *= fCritical;
							} break;
						}
					}
				}
				else
				{
					float fCritical = pAttackerUnit->GetCriticalValue( fDefenderCriticalValue ) * GetCriticalDefence();
					if ( fCritical >= 1.0f )
						fFinalDamage *= fCritical;
				}

				//ũ��Ƽ��~~
				D3DXVECTOR3 criticalEffPos = criticalNumPos;
				criticalEffPos.y += 20.f;

				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"CriticalHit01", criticalEffPos );

#ifdef ADD_BACKATTACK
				g_pX2Game->GetX2Camera()->GetCamera().ImpactForward( 25.f, 150.f, 15.f, 0.6f, 1 );
#else
				UpDownCrashCamera(40.f, 0.3f);
#endif
				//pDamageData->fCameraCrashGap = 40.0f;
				//pDamageData->fCameraCrashTime = 0.3f;

				PlaySound( L"Critical_Hit.wav" );

#ifdef ADD_BACKATTACK
				bCriticalHit = true;
#endif
			}
		}

#ifdef ADD_BACKATTACK
		if( bBackAttack == true && bCriticalHit == false )
		{
			bShowRedPicture = true;
			fFinalDamage *= 1.1f;
			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera(50.f, 0.3f);


			PlaySound( L"HitKingNasod.wav", true, false );

			D3DXVECTOR3 vBackAttackPos = GetHeadBonePos();
			vBackAttackPos.y += 110.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"damage_backAttack_P01", vBackAttackPos );
		}
#endif

		/// ���� ���� ����� �ߵ� ���� ����
		if(  CX2DamageManager::AT_NORMAL ==  pDamageData->attackType )
		{
			if( IsDoubleAttackByBuff() )
				m_bDoubleAttack = true;
		}

		// ���������� �������� 2��� ������, UI�� ǥ���ϱ�� �������� �ݹݾ� 2�� �����ش�. (UI �и��� �޺κп�..)
		if ( true == m_bDoubleAttack )
		{
			fFinalDamage *= 2.f;	// ���� �������� ����
			bShowRedPicture = true; // ���������� ��� ���������� ǥ��!!
		}

		bool bCanDrainHp = false;

		CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
		if( NULL != pAttackerUnit )
		{
			pAttackerUnit->GetDrainHpBuffValue( pDamageData );

		}

		if( pDamageData->m_fDrainHpRate > 0.f && pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
		{
			bCanDrainHp = true;
		}

		if( bCanDrainHp == true )
		{
			if( pAttackerUnit != NULL && pAttackerUnit->GetGageData() != NULL )
			{
				float fDrainDamage = fFinalDamage * pDamageData->m_fDrainHpRate;

				if( fDrainDamage > 0.f )
				{
					std::wstringstream strStream;
					strStream << L"HP+ " << (int)fDrainDamage;
					D3DXVECTOR3 pos = pAttackerUnit->GetPos();
					pos.y += 50.0f;
					if( NULL != g_pData->GetPicCharGreen() )
						g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pAttackerUnit->GetDirVector(), CKTDGPicChar::AT_CENTER );

					//if( IsMyUnit() == true )
					{
						pAttackerUnit->UpNowHp( fDrainDamage );
					}
				}
			}
		}

		if( pDamageData->m_fDrainMpByReact > 0.f )
		{
			if( pAttackerUnit != NULL && pAttackerUnit->GetGageData() != NULL )
			{
				pAttackerUnit->UpNowMp( pDamageData->m_fDrainMpByReact * pDamageData->m_fDrainMpRate);
				std::wstringstream strStream;
				strStream << L"MP+ " << (int)(pDamageData->m_fDrainMpByReact * pDamageData->m_fDrainMpRate);
				D3DXVECTOR3 pos = pAttackerUnit->GetPos();
				pos.y += 70.0f;
				if( NULL != g_pData->GetPicCharGreen() )
					g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pAttackerUnit->GetDirVector(), CKTDGPicChar::AT_CENTER );

				UpNowMp( -1.f * pDamageData->m_fDrainMpByReact );
				std::wstringstream strStreamDamage;
				strStreamDamage << L"MP -" << (int)pDamageData->m_fDrainMpByReact;
				D3DXVECTOR3 posTarget = GetPos();
				pos.y += 70.f;
				if( NULL != g_pData->GetPicCharRed() )
					g_pData->GetPicCharRed()->DrawText( strStreamDamage.str().c_str(), posTarget, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
		}

		if ( true == m_bQuickenGuardBlocking )
		{
			CX2GUAra *pAra = static_cast<CX2GUAra*>(this);

			if ( NULL != pAra )
			{
				fFinalDamage *= 0.f;
			}
		}

		bool bIsPossibleProvoke = false;			// �ݰ��� ������ ���� �ΰ�?
		if ( GetUnit() != NULL && GetUnit()->GetType() == CX2Unit::UT_ELESIS && GetUnit()->GetClass() == CX2Unit::UC_ELESIS_GRAND_MASTER )
		{
			CX2GUEL * pElesisUser = static_cast<CX2GUEL *> ( this );

			if ( NULL != pElesisUser && true == pElesisUser->GetIsProvokeStateChange() )
			{
				pElesisUser->SetIsProvokeStateChange(false);	// ������ ���� �ʱ�ȭ �Ѵ�, �����Ѵٸ� true �� �����Ѵ�.
				if ( NULL != g_pData->GetDamageManager() )
				{
					// �ݰ��� ������ �����̰�, Attack Type �� �븻 �̰�, NO_DAMAGE �� �ƴϰ�
					// �ǳ� ������ ä���ִ� ���°� �ƴϰ� ���� ������ Ÿ���� �ƴϰ�, ���� Ÿ�Ե� �ƴϰ�
					// �׶��� ��Ʈ ���µ� �ƴϰ� Ȧ�� ���³� �׷� ���µ� �ƴϰ� ���̵� �� ź ���µ� �ƴϰ�
					// ���� �ְ� �ڱ� �տ� �ְ� Provoke Loop ���� �� ����.........
					if (  CX2DamageManager::AT_NORMAL == pDamageData->attackType && pDamageData->m_bNoDamage == false
						&& true == g_pData->GetDamageManager()->GetIsPossibleProvokeExtraDamage(pDamageData)
						&& pDamageData->m_fHpPercentUp <= 0.f && pDamageData->m_fIncreaseMP <= 0.f
						&& pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE
						&& pDamageData->reActResult != CX2DamageManager::RT_SENSOR && pDamageData->hitType != CX2DamageManager::HT_GROUND_HIT
						&& GetHold() == false && GetGrap() == false
						&& false == GetRidingOn() && true == m_FrameDataFuture.unitCondition.bAttackerFront
						)
					{
						pDamageData->reActResult = CX2DamageManager::RT_SMALL_DAMAGE;
						pElesisUser->SetIsProvokeStateChange(true);	// �ݰ��� ������ �����̴�, true �� �����Ѵ�.
						bIsPossibleProvoke = true;
						fFinalDamage *= 0.f;
					}
				}
			}
		}


		////////////////////////// fFinalDamage�� ������ �� ���� ���ּ���!!! (�ɼ� ��ġȭ ����)///////////////////////////////////

		//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
		if( pDamageData->m_bLvDamage == false )
		{
			// ��� ����� ��ģ ���� ������� �ɼ� ��ġȭ�� ������ ���Ҹ� ���� (snatch�� ������ ���� ����)
			if ( 0.0f < GetAdditionalDefence() )
			{
				fFinalDamage -= min( fFinalDamage * GetAdditionalDefence(), fFinalDamage );
			}
		}


		if( pDamageData->m_bLvDamage == false )
		{
			if( NULL != pAttackerUnit )
			{
				const float fAdditionalAttackValue = pAttackerUnit->GetAdditionalAttackDamage( pDamageData );
				if ( 0.0f < fAdditionalAttackValue )
				{
					fFinalDamage += fAdditionalAttackValue;

					// ���� ������ ��� �ѹ� �� �����ش�.
					if ( true == m_bDoubleAttack )
						fFinalDamage += fAdditionalAttackValue;
				}
			}
		}


		if ( true == bIsPossibleProvoke )	// �����ý� �ݰ��� ������ ���¶��
		{
			fFinalDamage = 0;				//�ߴ� ���⵵ ���� �ʱ� ���� ���⿡ ���� ó��
		}
		//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�

		if ( null != pDamageData->optrAttackerGameUnit && GUT_USER == pDamageData->optrAttackerGameUnit->GetGameUnitType() )
		{
			CX2GUUser* pUser = static_cast< CX2GUUser* >( pDamageData->optrAttackerGameUnit.GetObservable() );

			if ( NULL != pUser &&
				 NULL != pUser->GetUnit() &&
				 NULL != pUser->GetUnit()->GetUnitTemplet() &&
				 CX2Unit::UT_ADD == pUser->GetUnit()->GetType() )
			{
				/// �����̽ý� �ʵ�� Ÿ�� ��ġ ������ ����
				pUser->SetStasisfieldHitValue( fFinalDamage );
			}
		}

		// UI�� Ÿ�ݵ� �������� �����ֱ� ���� �뵵
		float fFinalDamageToShow = fFinalDamage;
		// ���� ������ ���
		if ( m_bDoubleAttack )
		{
			// �����ִ� UI���� ��ġ�� 1/2�� �Ͽ� �ι� �����ش�.
			fFinalDamageToShow *= 0.5f;
			SetDoubleAttack(wstrHitSound, fFinalDamageToShow, pDamageData->impactPoint);	// �ѹ� �� ������
		}

		// ũ��Ƽ���̰ų� ���� �����̸� ���������� ǥ��
		if ( true == bShowRedPicture )
		{
			if( NULL != g_pData->GetPicCharRed() )
				g_pData->GetPicCharRed()->DrawText( static_cast<int>( fFinalDamageToShow ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}
		else	// �׷��� ������ �Ϲ� �����
		{
			if( NULL != g_pData->GetPicChar() )
				g_pData->GetPicChar()->DrawText( static_cast<int>( fFinalDamageToShow ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}





		/// ���� �� �ߵ��Ǵ� ��ų �˻� �� ó��
		ProcessSkillAtDie( fFinalDamage, pDamageData );


		if ( NULL != GetUnit() && CX2Unit::UT_ADD == GetUnit()->GetType() )
		{
			/// �����̽ý� �ʵ�� �ǰ� ��ġ ������ ����
			SetStasisfieldDamageValue( fFinalDamage );

			switch ( GetUnit()->GetClass() )
			{
			case CX2Unit::UC_ADD_LUNATIC_PSYKER:
				{
					/// ������ �����Ϳ� �ǰ� ������ ����
					SetReverseReactorTransitionDamage( fFinalDamage );
				} break;
			}
		}

		if( IsMyUnit() == true )
		{
			m_nDamageCount++;

			/// �ֵ��� ��, �ǰݽ� DP ��ġ �÷� ����
			if ( NULL != GetUnit() && CX2Unit::UT_ADD == GetUnit()->GetType() )
			{
				/// �� �����Ӵ� �ѹ��� ����
				if ( false == GetUpDPByDamageThisFrame() )
				{
					UpNowDPValueByDamage( fFinalDamage );
					SetUpDPByDamageThisFrame( true );
				}

			}

			float fOldHP = GetNowHp();

//#ifdef PVP_BOSS_COMBAT_TEST
//
//			if( CX2Game::GT_PVP == g_pX2Game->GetGameType() )
//			{
//				CX2PVPRoom* pPVPRoom = (CX2PVPRoom*) g_pX2Room;
//				if( CX2PVPRoom::PGT_TEAM_DEATH == pPVPRoom->GetPVPGameType() &&
//					NULL != g_pX2Room &&
//					NULL != g_pX2Room->GetSlotDataByUnitUID( GetUnitUID() ) &&
//					false == g_pX2Room->GetSlotDataByUnitUID( GetUnitUID() )->GetIsBoss() )
//				{
//					if( m_TeamNum == m_TeamNumOriginal )
//					{
//						if( fOldHP - fFinalDamage <= 1.f )
//						{
//							SetNowHp( 1.f );
//							pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
//
//							m_TeamNum = 9;
//							StateChange( m_FrozenState );
//						}
//						else
//						{
//							UpNowHp( -fFinalDamage );
//						}
//					}
//					else
//					{
//
//						if( NULL != pAttackerUnit &&
//							m_TeamNumOriginal == pAttackerUnit->GetTeam() )
//						{
//							// fix!! ���� ���¿��� �ǰ� ��� �߰��ؾ� ��!!
//							const int MAGIC_FRIENDLY_HIT_COUNT_TO_MELT = 5;
//							if( m_iFriendlyHittedCountAtFrozenState > MAGIC_FRIENDLY_HIT_COUNT_TO_MELT )
//							{
//								SetNowHp( GetMaxHp() * 0.9f );
//								m_TeamNum = m_TeamNumOriginal;
//								StateChange( USI_DAMAGE_AIR_FALL );
//							}
//							else
//							{
//								m_iFriendlyHittedCountAtFrozenState++;
//								fFinalDamage = 0;
//								pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
//							}
//						}
//						else
//						{
//							fFinalDamage = 0.f;
//							pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
//						}
//					}
//				}
//				else
//				{
//					UpNowHp( -fFinalDamage );
//				}
//			}
//			else
//			{
//				UpNowHp( -fFinalDamage );
//			}
//#else PVP_BOSS_COMBAT_TEST
			//{{AFX
			UpNowHp( -fFinalDamage );
			//}}AFX
//#endif PVP_BOSS_COMBAT_TEST

			if( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
			{
				if( GetNowHp() <= 0.f )
					SetNowHp( 1.f );
			}

		/// ���� �� �ߵ��Ǵ� ��ų���� �Լ� �ϳ��� ����

			if( m_bUpMPThisFrame == false )
			{
				int		redAdv						= 0;
				int		blueAdv						= 0;
				float	fIncreaseMpRate				= 1.0f;		// ��*Ÿ�ݽ� ���� + ����ġ(1.0�� ������ ���� ����)
				float	fIncreaseSoulRate			= 1.0f;		// ��*Ÿ�ݽ� ���� + ����ġ
				if( CX2GameUnit::GUT_USER == GetGameUnitType() )
				{
					if( CX2Unit::UT_ELSWORD == GetUnit()->GetType() )
					{
						fIncreaseMpRate *= g_pData->GetDamageManager()->CalcVigorMpUpOnDamageReact( GetWayOfSwordState() );
					}
				}

			//{{ kimhc // 2009-11-17 // ��� ��ų �� ��������, ���ߵ� ����, �ݳ�, �г��ع�

				const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

				// ��������
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_CONCENTRATION_OF_SPIRIT, pUnitData->m_byMemberShipGrade );

				if( NULL != pSkillTemplet )
				{
					const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

					fIncreaseMpRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_GET_HIT_REL, iSkillTempletLevel );
				}

				// �ݳ�

				pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_VIOLENT, pUnitData->m_byMemberShipGrade );

				if( NULL != pSkillTemplet )
				{
					const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

					float fRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_GET_HIT_REL, iSkillTempletLevel );

					fIncreaseSoulRate += CalculateIncreasingRate( fRate );
				}
			//}} kimhc // 2009-11-17 // ��� ��ų �� ��������, ���ߵ� ����, �ݳ�, �г��ع�

				//{{ kimhc // 2010.11.3 // ���� - ��ȭ�� �г�
				const CX2SkillTree::SkillTemplet* pSkillTempletSpritualizedFury
					= pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY );

				if ( NULL != pSkillTempletSpritualizedFury )
				{
					const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTempletSpritualizedFury->m_eID, true ) );	/// ��ų ����

					float fRate = pSkillTempletSpritualizedFury->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_GET_HIT_REL, iSkillTempletLevel );

					fIncreaseSoulRate += CalculateIncreasingRate( fRate );
				} // if
				//}} kimhc // 2010.11.3 // ���� - ��ȭ�� �г�

				if( g_pX2Room != NULL && g_pData->GetPVPRoom() != NULL )
				{
					if( g_pX2Room->GetRoomType() == CX2Room::RT_PVP )
					{
						if( g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM
							|| g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM_DEATH )
						{
							int redNum		= g_pX2Room->GetTeamNum( CX2Room::TN_RED, true );
							int blueNum		= g_pX2Room->GetTeamNum( CX2Room::TN_BLUE, true );
							if( redNum > blueNum )
							{
								blueAdv = redNum - blueNum;
							}
							else if( redNum < blueNum )
							{
								redAdv = blueNum - redNum;
							}
						}
					}
				}

				//{{ dmlee 2008.04.11 ĳ�� ������ �����
				if( 0.f != m_CashItemAbility.m_fUpMPAtAttackOrDamage )
				{
					UpNowMp( m_CashItemAbility.m_fUpMPAtAttackOrDamage );
				}
				//}} dmlee 2008.04.11 ĳ�� ������ �����

				//{{ ������ : ���� ������ ���� ȸ���� ���ʽ� ���� ���� ����
				float teamAdv = 0.f;
				if( GetTeam() == CX2Room::TN_RED )
					teamAdv = (float)redAdv;
				else if( GetTeam() == CX2Room::TN_BLUE )
					teamAdv = (float)blueAdv;

				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_PVP:
					{
						//float fIncreaseMp = (3.0f + (2.0f * teamAdv)) * fIncreaseMpRate;
						SetBasicStatValueChargeMpHitted( (3.0f + (2.0f * teamAdv)) * fIncreaseMpRate );
						CheckAndUpdateAdditionalChargeMpHitted();

						UpNowMp( GetChargeMpHitted() );
						fMpPet = (3.0f + (2.0f * teamAdv)) * fIncreaseMpRate;
					} break;

				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					{
						SetBasicStatValueChargeMpHitted( (3.0f + (2.0f * teamAdv))*0.8f * fIncreaseMpRate );
						CheckAndUpdateAdditionalChargeMpHitted();

						UpNowMp( GetChargeMpHitted() );
						fMpPet = (3.0f + (2.0f * teamAdv))*0.8f * fIncreaseMpRate;
					} break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}

				if( GetHyperModeCount() < 3 && pDamageData->m_fHpPercentUp <= 0.f )
				{
					/// ���� ���� ������ ����
					UpNowSoul( ( (2.0f * (1.0f + GetHyperChargeSpeed() ) ) + (2.0f * teamAdv) ) * fIncreaseSoulRate );
					//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
				}

				m_bUpMPThisFrame = true;

				CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
				if( pPet != NULL )
				{
					pPet->DamageReact( fMpPet );
				}

			}

			// ���������� Ÿ���� ����� üũ�ϱ� ����
			if( pAttackerUnit != NULL )
			{
				if( pAttackerUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					AddToDamagedMap( pAttackerUnit, fFinalDamage );
					SetGameUnitWhoAttackedMe( pAttackerUnit );
				}
				else
				{
					CX2GUNPC* pNPC = (CX2GUNPC*) pAttackerUnit;
					if( NULL != pNPC->GetNPCAI() &&
						CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )	// ������ ��ȯ�� ���� �̸�, ������ �����ɷ� ī��Ʈ �Ѵ�
					{
						CX2AllyNPCAI* pAllyAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
						if( CX2GameUnit::GUT_USER == pAllyAI->GetAllyUnitType() )
						{
							CX2GameUnit* pAllyGameUnit = pAllyAI->GetAllyGameUnit();
							AddToDamagedMap( pAllyGameUnit, fFinalDamage );
                            if ( pAllyGameUnit != NULL && pAllyGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
                                SetGameUnitWhoAttackedMe( pAllyGameUnit );
                            else
                            {
                                ResetGameUnitAttackedByMe();
                                SetAttackedByUserUnit( true );
                            }

						}
						else
						{
							SetGameUnitWhoAttackedMe( pAttackerUnit );
						}
					}
					else
					{
						//	���� ���� ���� ��쿡�� ������ ���� ������ ī��Ʈ
						if( pAttackerUnit->GetAllyTeam() != CX2Room::TN_NONE && pAttackerUnit->IsPvpBot() == false )
						{
                            SetGameUnitWhoAttackedMe( pNPC->GetOwnerGameUnit() );
						}
						else
						{
							SetGameUnitWhoAttackedMe( pAttackerUnit );
						}
					}
				}
			}

			float fForceDownValue = pDamageData->fForceDownValue;
			fForceDownValue -= pDamageData->m_fDecreaseForceDown;
			if(pDamageData->m_bPenetrate == true)
			{
				if(fForceDownValue < 0.f)
					fForceDownValue -= 10.f;
				else
					fForceDownValue = -10.f;
			}

			/// ��� ��� ����, �ٿ� ��ġ ����
			if  ( NULL != GetUnit() )
			{
				const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

				int iSkillLevel = pUnitData->m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_SGM_ADVANCED_KNIGHT_MASTERY, true );
				const CX2SkillTree::SkillTemplet* pSkillTempletAdvancedKnightMastery = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_SGM_ADVANCED_KNIGHT_MASTERY );

				if ( NULL != pSkillTempletAdvancedKnightMastery )
				{
					const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTempletAdvancedKnightMastery->m_eID, true ) );	/// ��ų ����

					float fRate = pSkillTempletAdvancedKnightMastery->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL, iSkillTempletLevel );
					fForceDownValue = fForceDownValue * fRate;

				}
			}
			AddForceDownDelay( fForceDownValue );

			if ( IsMyUnit() )
				CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 1, 0, 0, 1) );

		}

		//{{ kimhc // 2009-11-24 // ��� ��ų �� ������ ü��
		if ( pAttackerUnit != NULL &&
			pDamageData->m_bNoDamage == false )
		{
			GuildSkillProcess( CX2SkillTree::SI_GP_COMMON_INFINITE_STRENGTH );

			if ( m_GuildSkillData.m_InfiniteStrength.m_byCountLeft > 0 )
			{
				float fRecoveryHpOnce = 0.3f;
				if ( fFinalDamage > ( GetMaxHp() * fRecoveryHpOnce ) ) // MAX_HP�� 50%���� �������� ũ��
				{
					m_GuildSkillData.m_InfiniteStrength.m_fRecoveryHpOnce = ( GetMaxHp() * fRecoveryHpOnce ) / m_GuildSkillData.m_InfiniteStrength.m_byCountLeft;
				}
				else
				{
					m_GuildSkillData.m_InfiniteStrength.m_fRecoveryHpOnce = fFinalDamage * m_GuildSkillData.m_InfiniteStrength.m_fRecoveryPerOneDamage;
				}

				m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL, L"InfiniteStrengthLight",
					pAttackerUnit->GetPos().x, pAttackerUnit->GetPos().y + 100.0f, pAttackerUnit->GetPos().z, -1, -1, -1, 3, -1  );

				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect );
				if ( pSeq != NULL )
					pSeq->SetLandPosition( pAttackerUnit->GetLandPos().y );
			}
		}
		//}} kimhc // 2009-11-24 // ��� ��ų �� ������ ü��

		//{{ JHKang / ������ / 2011/01/17 / ���� ������ ���� ������ ��� ����
		m_fRealDamage = fFinalDamage;
		//}} JHKang / ������ / 2011/01/17 / ���� ������ ���� ������ ��� ����

		//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
		m_iTotalAttackedDamage += static_cast<int>( fFinalDamage );
		//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����

		if( GetUnit() != NULL && GetUnit()->GetType() == CX2Unit::UT_RAVEN )
		{
			CX2GURaven *pRaven = (CX2GURaven*)this;
			if( pRaven != NULL && GetRemainHyperModeTime() > 0.f )
			{
				float fMaxHp = pRaven->GetMaxHp();
				float fRage = fFinalDamage / fMaxHp * 500.f;
				pRaven->AddRageGuage( fRage );
			}
		}

	}

	// Ÿ�ݹ޾������ ����Ʈ ���Ḧ ����
	// DELETE_EFFECT_SET_ON_DAMAGE_REACT�� TRUE�� ��� ��ϵ� ���� ������Ʈ�� effectset���� �����.
	if ( IsSuperArmor() == false &&
		m_FrameDataNow.stateParam.bInvincible == false &&
		CX2DamageManager::RT_DUMMY_DAMAGE != pDamageData->reActResult
		&& CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE != pDamageData->reActResult
		)
	{
		DeleteEffectSetOnDamageReact();

		// ������ ������Ʈ���� ����Ʈ�� ����� ���, �ǰ� �� ���� �ϵ��� ó��
		if( true == m_bIsCustomStateDeleteEffectOnDamageReact )
			DeleteEffectSetOnCustomState();
	}

#ifdef PVP_ZOOM_CAMERA
	SetSubZoomCameraTimer();
#endif


	PostProcessEnchantDamage( pDamageData );

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}

void CX2GUUser::DamageDoubleAttack()
{
	m_bDoubleAttack = false;
	m_fDoubleAttackTime = 0.f;

	if( m_wstrHitSound.empty() == false )
	{
		PlaySound( m_wstrHitSound.c_str() );
	}

	D3DXVECTOR3 numPos;
	GetFramePos( &numPos, m_pFrame_Bip01_Head );
	numPos.y += 70.0f;

	if( g_pData->GetPicCharRed() != NULL )
		g_pData->GetPicCharRed()->DrawText( (int)m_fFinalDamage, numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );

	int randVal = rand() % 1001;
	int iRandX = randVal % 21 - 10;
	int iRandZ = (randVal / 100) % 21 - 10;
	int iRandY = (rand() % 21 - 10);
	m_vImpactPoint.x += (float)iRandX;
	m_vImpactPoint.y += (float)iRandY;
	m_vImpactPoint.z += (float)iRandZ;

	CKTDGParticleSystem::CParticleEventSequence* pSeq1 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpact", m_vImpactPoint, 50, 50, 1, 1 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq2 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpact", m_vImpactPoint, 100, 200, 2, 10 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq3 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpactCore", m_vImpactPoint, 100, 200, 2, 10 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq4 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpactSlash", m_vImpactPoint, 50, 50, 2, 2 );

	if(pSeq1 != NULL)
		pSeq1->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq2 != NULL)
		pSeq2->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq3 != NULL)
		pSeq3->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq4 != NULL)
		pSeq4->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));


	UpDownCrashCamera(20.f);
}



// @bApplyExtraDamageOfAttacker: pDamageData->pAttackerUnit�� damagedata�� �������� ����
void CX2GUUser::DamageReact_ExtraDamage( CX2DamageManager::DamageData* pDamageData, bool bApplyExtraDamageOfAttacker )
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	if( NULL == pDamageData )
		return;








	// oasis907 : ����� [2010.3.22] // �Լ� �ϳ��� �������ϴ�
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_ExtraDamage);
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_BufExtraDamage);
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_SocketExtraDamage);
	// oasis907 : ����� [2010.10.11] // �ߺ� ������ EDT�� ������ �߰��մϴ�.
	for( int i=0; i<(int)pDamageData->m_MultipleExtraDamage.size(); i++ )
	{
		LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_MultipleExtraDamage[i]);
	}
	//}}




	float randVal = GetRandomFloat();
	const CX2DamageManager::ExtraDamageData& extraDamageData = pDamageData->m_ExtraDamage;
	if( CX2DamageManager::EDT_NONE != extraDamageData.m_ExtraDamageType )
	{
		ASSERT( extraDamageData.m_fRate >= 0.f && extraDamageData.m_fRate <= 1.f );
		if( extraDamageData.m_fRate > randVal )
		{
			Process_ExtraDamage( pDamageData, extraDamageData );
		}
	}

	// ������ extra damage
	if( pDamageData != NULL && null != pDamageData->optrAttackerGameUnit
		//&& pDamageData->optrAttackerGameUnit->GetDamageData() != NULL
        )
	{
		const CX2DamageManager::ExtraDamageData& bufExtraDamageData = pDamageData->optrAttackerGameUnit->GetDamageData().m_BufExtraDamage;
		if( CX2DamageManager::EDT_NONE != bufExtraDamageData.m_ExtraDamageType &&
			bufExtraDamageData.m_fRate > (float)randVal )
		{
			if( pDamageData->attackerType != CX2DamageManager::AT_EFFECT ||
				pDamageData->optrAttackerGameUnit->GetDamageData().m_bApplyExtraDamage == true )
			{
				if( pDamageData->m_bNoBuff == false && pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
					Process_ExtraDamage( pDamageData, bufExtraDamageData );
			}
		}
	}

	if( pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
	{
		const CX2DamageManager::ExtraDamageData& socketExtraDamageData = pDamageData->m_SocketExtraDamage;
		if( CX2DamageManager::EDT_NONE != socketExtraDamageData.m_ExtraDamageType )
		{
			ASSERT( socketExtraDamageData.m_fRate >= 0.f && socketExtraDamageData.m_fRate <= 1.f );
			if( socketExtraDamageData.m_fRate > randVal )
			{
				Process_ExtraDamage( pDamageData, socketExtraDamageData );
			}
		}

		// ��ø������ extra damage
		for( int i=0; i<(int)pDamageData->m_MultipleExtraDamage.size(); i++ )
		{
			const CX2DamageManager::ExtraDamageData& multipleExtraDamageData = pDamageData->m_MultipleExtraDamage[i];
			ASSERT( multipleExtraDamageData.m_fRate >= 0.f && multipleExtraDamageData.m_fRate <= 1.f );
			if( GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_MULTIPLE + i ) < multipleExtraDamageData.m_fRate )
			{
				Process_ExtraDamage( pDamageData, multipleExtraDamageData );
			}
		}

		// ���� ���°� AT_EFFECT�̸� (�������, ȭ���̳� ���̾) �������� ���Ͽɼ� extra_damage�� �����Ѵ�.
		if( true == bApplyExtraDamageOfAttacker &&
			CX2DamageManager::AT_EFFECT == pDamageData->attackerType &&
			null != pDamageData->optrAttackerGameUnit )
		{
			pDamageData->optrAttackerGameUnit->GetDamageData().optrAttackerGameUnit = pDamageData->optrAttackerGameUnit;
			//{{ oasis907 : ����� [2010.3.23] //
			pDamageData->optrAttackerGameUnit->GetDamageData().optrDefenderGameUnit = pDamageData->optrDefenderGameUnit;
			//}}
			pDamageData->optrAttackerGameUnit->GetDamageData().impactPoint = pDamageData->impactPoint;

			//{{ ������ // 2012-06-28 // AT_EFFECT ���ݿ� �ǵ����� ���� �������� �ͽ�Ʈ�󵥹����� ����Ǵ� ���� ����
			//���Ͽɼ��� ���� ���� �� �ֵ��� m_SocketExtraDamage, m_MultipleExtraDamage �� ó���ϵ��� ����
			const CX2DamageManager::ExtraDamageData& SocketExtraDamage = pDamageData->optrAttackerGameUnit->GetDamageData().m_SocketExtraDamage;
			if( CX2DamageManager::EDT_NONE != SocketExtraDamage.m_ExtraDamageType &&
				SocketExtraDamage.m_fRate > (float)randVal )
			{
				LevelCorrectionExtraDamage( pDamageData, &pDamageData->optrAttackerGameUnit->GetDamageData().m_SocketExtraDamage );
				Process_ExtraDamage( pDamageData, SocketExtraDamage );
			}

			for( int i=0; i<(int)pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage.size(); i++ )
			{
				const CX2DamageManager::ExtraDamageData& multipleExtraDamageData = pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage[i];
				LevelCorrectionExtraDamage( pDamageData, &pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage[i] );

				float r = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_MULTIPLE + i );
				if( r < multipleExtraDamageData.m_fRate )
				{
					Process_ExtraDamage( pDamageData, multipleExtraDamageData );
				}
			}
		}
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}

//{{ oasis907 : ����� [2010.3.22] // ���� ������ ExtraDamage ���� ����
void CX2GUUser::LevelCorrectionExtraDamage( const CX2DamageManager::DamageData* pDamageData, CX2DamageManager::ExtraDamageData* pExtraDamageData )
{
	int iLevel = pExtraDamageData->m_iLevel;
	if( iLevel == 0 )
	{
		if( pExtraDamageData->m_bDefenderLevel == false && null != pDamageData->optrAttackerGameUnit )
			iLevel = pDamageData->optrAttackerGameUnit->GetUnitLevel();
		else if( pExtraDamageData->m_bDefenderLevel == true && null != pDamageData->optrDefenderGameUnit )
			iLevel = pDamageData->optrDefenderGameUnit->GetUnitLevel();
	}

	CX2EnchantItem::ENCHANT_TYPE eEnchantType = GetEnchantTypeFromExtraDamageType( pExtraDamageData->m_ExtraDamageType );
	float fResist = GetEnchantResist(eEnchantType); // �ǰ��� �Ӽ� ����ġ

	pExtraDamageData->m_DamagePerSec = pExtraDamageData->GetCalcPerDamage(iLevel, fResist);

	float fRate = pExtraDamageData->m_fPureRate;
	float fPureRate = pExtraDamageData->m_fPureRate;

	if( pExtraDamageData->m_bIgnoreResist == true )
	{
		fRate = 1.f;
	}
	else if( null != pDamageData->optrAttackerGameUnit && null != pDamageData->optrDefenderGameUnit && pExtraDamageData->m_iLevel >= 0)
	{
		// ( ( (������ ���� - �ǰ��� ����) * 0.025f ) + 0.5f ) * (1.f - (�ǰ��� �Ӽ�����ġ / 500.f))
		int attackerLv = pDamageData->optrAttackerGameUnit->GetUnitLevel();
		int defencerLv = pDamageData->optrDefenderGameUnit->GetUnitLevel();

		fRate = ( ( (attackerLv - defencerLv) * 0.025f ) + 0.5f ) * (1.f - (fResist / CX2EnchantItem::EAR_MAX_VALUE));
		if(fPureRate >= 1.f) // oasis907 : ����� [2011.5.25] ��ũ��Ʈ �Ľ̽� ����Ʈ fRate�� 1.f,
		{					// �ܺο��� Rate������ ���������� ������ �ִ� Rate�� 0.5�� ����
			if( fRate > 0.5f )
				fRate = 0.5f;
		}
		else
		{
			if( fRate > fPureRate )
				fRate = fPureRate;
		}

		//{{ ����Ʈ�󵥹��� Ȯ���� ������ �� 0���� ó�� (ASSERT ����)
		if( fRate < 0.f )
			fRate = 0.f;
		//}} ����Ʈ�󵥹��� Ȯ���� ������ �� 0���� ó�� (ASSERT ����)
	}

	pExtraDamageData->m_fRate = fRate;

	// oasis907 : ����� [2010.10.5] // ���� �ɼ� �ߵ� Ȯ�� ����
	if( pExtraDamageData->m_bIgnoreResist == false )
	{
		{
			float fRateModifier = 1.f;
			if( iLevel >= 0 )
			{
				fRateModifier = pExtraDamageData->m_fRate;
			}
			else
			{
				fRateModifier = pExtraDamageData->m_fPureRate;
			}
			pExtraDamageData->m_fRate = fRateModifier * pDamageData->m_fRateModifier;
		}
	}

	return;
}
//}} oasis907 : ����� [2010.3.22] //


// @pDamageData : ��Ÿ �ʿ��� ������ ��� ���� ����
// @extraDamageData : ���� ó���ؾ��� extra damage
void CX2GUUser::Process_ExtraDamage( const CX2DamageManager::DamageData* pDamageData, const CX2DamageManager::ExtraDamageData &extraDamageData )
{
	ASSERT( pDamageData != NULL );

	// kimhc / 2010-04-02 // ��д��� ��Ʈ������ �۾����� �߰�
	bool bGetExtraDamage = false;	// �����̻� �ɷȴ°�?

	switch( extraDamageData.m_ExtraDamageType )
	{
	case CX2DamageManager::EDT_POISON:
		{
			if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
			{
				m_ExtraDamagePack.m_Poison = extraDamageData;
				if(extraDamageData.m_iLevel < 0.f)
				{
					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_NATURE, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Poison.m_fTime = fDuration;
				}
				if( extraDamageData.m_fRunJumpRate > 0.f && m_ExtraDamagePack.m_Poison.m_fTime > 0.f )
				{
					SetMoveSpeedFactor(extraDamageData.m_fRunJumpRate, m_ExtraDamagePack.m_Poison.m_fTime);
					SetJumpSpeedFactor(extraDamageData.m_fRunJumpRate, m_ExtraDamagePack.m_Poison.m_fTime);
					SetPoisonDeBuff(m_ExtraDamagePack.m_Poison.m_fTime);
				}
				bGetExtraDamage = true;
			}
		}
		break;

	case CX2DamageManager::EDT_SLOW:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Slow = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Slow.m_fTime = fDuration;

					bGetExtraDamage = true;
				}
			}
		}
		break;

	case CX2DamageManager::EDT_FIRE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					bool bNewExtra = true;
					if( m_ExtraDamagePack.m_Fire.m_fTime > 0.f )
					{
						if( m_ExtraDamagePack.m_Fire.m_DamagePerSec > extraDamageData.m_DamagePerSec )
						{
							bNewExtra = false;
						}
						else if( m_ExtraDamagePack.m_Fire.m_DamagePerSec < extraDamageData.m_DamagePerSec )
						{
							bNewExtra = true;
						}
						else
						{
							if( m_ExtraDamagePack.m_Fire.m_fTime >  extraDamageData.m_fTime )
							{
								bNewExtra = false;
							}
							else
							{
								bNewExtra = true;
							}
						}
					}

					if( bNewExtra == true )
					{
						m_ExtraDamagePack.m_Fire = extraDamageData;

						if(extraDamageData.m_iLevel < 0.f)
						{
							float fDuration = extraDamageData.m_fTime;
							ApplyEnchantResist( CX2EnchantItem::ET_BLAZE, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
							m_ExtraDamagePack.m_Fire.m_fTime = fDuration;
						}
					}

					bGetExtraDamage = true;
				}
			}
		}
		break;

	case CX2DamageManager::EDT_CURSE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Curse = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Curse.m_fTime = fDuration;

					bGetExtraDamage = true;
				}
			}
		}
		break;

	case CX2DamageManager::EDT_LIGHTNING:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Lightning = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_WIND, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Lightning.m_fTime = fDuration;

					bGetExtraDamage = true;
				}
			}
		}
		break;

	case CX2DamageManager::EDT_COLD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Cold = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Cold.m_fTime = fDuration;

					bGetExtraDamage = true;
				}
			}
		} break;

	case CX2DamageManager::EDT_MANA_DAMAGE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				//{{ JHKang / ������ / 2010/11/05 / �糪 �����̵�, ���� ������ ������ ���� �߰�
				if ( extraDamageData.m_fDamageRate > 0.f && extraDamageData.m_Damage < 0.f )
				{
					float fDamageMP = GetNowMp() * extraDamageData.m_fDamageRate;
					SetNowMp( fDamageMP );

					std::wstringstream strStream;
					strStream << L"MP -" << (1.f - extraDamageData.m_fDamageRate) * 100.f << L"%";
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f + (0 * 30.0f);
					if( NULL != g_pData->GetPicCharRed() )
						g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

					m_bHitedDrainMana = false;

					bGetExtraDamage = true;
				}
				else
				{
					UpNowMp(-1* extraDamageData.m_Damage);

					std::wstringstream strStream;
					strStream << L"MP -" << (int)extraDamageData.m_Damage;
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f + (0 * 30.0f);
					if( NULL != g_pData->GetPicCharRed() )
						g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

					m_bHitedDrainMana = true;

					bGetExtraDamage = true;
				}
				//}} JHKang / ������ / 2010/11/05 / �糪 �����̵�, ���� ������ ������ ���� �߰�
			}
		} break;

	case CX2DamageManager::EDT_FIRE_SMASH:
		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Fire_Smash_Smoke_FireWave04_Nasod_Element_Fire", pDamageData->impactPoint );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Fire_Smash_Flare_FireWave05_Nasod_Element_Fire", pDamageData->impactPoint );

			UpDownCrashCamera( 20.0f, 0.5f );

			bGetExtraDamage = true;
		} break;

	case CX2DamageManager::EDT_COLD_SMASH:
		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Cold_Smash_Smoke_IceWave04_Nasod_Element_Ice", pDamageData->impactPoint );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Cold_Smash_Flare_IceWave05_Nasod_Element_Ice", pDamageData->impactPoint );

			UpDownCrashCamera( 20.0f, 0.5f );

			bGetExtraDamage = true;
		} break;


	case CX2DamageManager::EDT_EVENT_SMASH:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_FrameDataFuture.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_CoupleHell", vPos );

			UpDownCrashCamera( 30.0f, 0.5f );

			bGetExtraDamage = true;
		} break;
	case CX2DamageManager::EDT_CAMILLA_SMASH:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_FrameDataFuture.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_FightKing", vPos );

			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 30.0f, 0.5f );

			bGetExtraDamage = true;
		} break;
	case CX2DamageManager::EDT_EVENT_SMASH_3:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_FrameDataFuture.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence(  this,  L"STR_EventSmash2", vPos );

			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 30.0f, 0.5f );

			bGetExtraDamage = true;

		} break;
	case CX2DamageManager::EDT_EVENT_SMASH_2:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_FrameDataFuture.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_EventSmash2", vPos );

			UpDownCrashCamera( 30.0f, 0.5f );

			bGetExtraDamage = true;
		} break;

	case CX2DamageManager::EDT_POISON_CLOUD:
		{
			if( null != pDamageData->optrAttackerGameUnit )
			{
				const float MAGIC_POWER_RATE = 1.f;
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pDamageData->optrAttackerGameUnit.GetObservable(), L"EXTRA_DAMAGE_POISON_CLOUD", MAGIC_POWER_RATE,
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition().y );

				UpDownCrashCamera( 20.0f, 0.5f );

				bGetExtraDamage = true;
			}
		} break;

	case CX2DamageManager::EDT_DOUBLE_ATTACK:
		{
			if ( pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
			{
				m_bDoubleAttack = true;

				bGetExtraDamage = true;
			}
			//g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 20.0f, 0.5f );
		} break;


	case CX2DamageManager::EDT_FROZEN:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Frozen = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Frozen.m_fTime = fDuration;

					if( fDuration > 0.f )
					{
						AnimStop();
					}

					m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
					m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

					UpDownCrashCamera( 20.0f, 0.5f );

					bGetExtraDamage = true;

				}
			}
		} break;

	case CX2DamageManager::EDT_ENTANGLE:
		{

			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Entangled = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					//ApplyEnchantResist( CX2EnchantItem::ET_WIND, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_Entangled.m_fTime = fDuration;

					bGetExtraDamage = true;
				}
			}
		} break;

	case CX2DamageManager::EDT_TIME_BOMB:
		{

			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					if( m_ExtraDamagePack.m_TimeBomb.m_fTime <= 0.f )		// �̹� time bomb �� �ɷ������� �߰��� timebomb�� �߰����� ����~
					{
						m_ExtraDamagePack.m_TimeBomb = extraDamageData;

						float fDuration = extraDamageData.m_fTime;
						//ApplyEnchantResist( CX2EnchantItem::ET_WIND, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
						m_ExtraDamagePack.m_TimeBomb.m_fTime = fDuration;

						bGetExtraDamage = true;
					}
				}
			}
		} break;


	case CX2DamageManager::EDT_DAMAGE_RATE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				float fNowHp = GetNowHp();
				float fDamage = fNowHp * extraDamageData.m_Damage;

				UpNowHp( fDamage * -1.f );

				std::wstringstream strStream;
				strStream << L"HP -" << (int)fDamage;
				D3DXVECTOR3 pos = GetPos();
				pos.y += 50.0f + (0 * 30.0f);
				if( NULL != g_pData->GetPicCharRed() )
					g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

				bGetExtraDamage = true;
			}
		} break;


	case CX2DamageManager::EDT_STIGMA_DEBUFF:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_StigmaDebuff = extraDamageData;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Rena_Stigma_User", this );

				bGetExtraDamage = true;
			}
		} break;

	case CX2DamageManager::EDT_LIGHTNING_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"LIGHTNING_SMASH", pAttackerUnit->GetPowerRate(),
					GetLandPos(), GetRotateDegree(), GetRotateDegree() );
			}
		} break;
		//{{ oasis907 : ����� [2010.11.8] // �ε� ����Ʈ - ���� ���� - EDT_BLIND
	case CX2DamageManager::EDT_BLIND:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Blind = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				m_ExtraDamagePack.m_Blind.m_fTime = fDuration;
				float fEffectiveRate = extraDamageData.m_fDamageRate;
				m_ExtraDamagePack.m_Blind.m_fDamageRate = fEffectiveRate;
			}
		} break;
		//}} oasis907 : ����� [2010.11.8] // �ε� ����Ʈ - ���� ���� - EDT_BLIND

		//{{ kimhc // 2010.12.29 // �ϸ� ���� �߰��� ���� ExtraDamage
	case CX2DamageManager::EDT_ICE_STING:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_ICE_STING", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;

	case CX2DamageManager::EDT_FIRE_BALL:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_FIRE_BALL", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;
		//}} kimhc // 2010.12.29 // �ϸ� ���� �߰��� ���� ExtraDamage


		//{{ lucidash : ����� [2011.1.4] // ����3 ���� ���� ���� ����
	case CX2DamageManager::EDT_WATER_HOLD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_WaterHold = extraDamageData;

					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// �Ӽ� ������ ������ ���ӽð��� �ٿ��ش�
					m_ExtraDamagePack.m_WaterHold.m_fTime = fDuration;

					if( fDuration > 0.f )
					{
						//StateChange( USI_DAMAGE_SMALL_FRONT );
						SetStopTime( fDuration );
						//AnimStop();
					}

					m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
					m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

					UpDownCrashCamera( 20.0f, 0.5f );

					bGetExtraDamage = true;
				}
			}
		} break;
		//}} lucidash : ����� [2011.1.4] // ����3 ���� ���� ���� ����

		//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
	case CX2DamageManager::EDT_PANIC:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					m_ExtraDamagePack.m_Panic = extraDamageData;

					UpDownCrashCamera( 20.0f, 0.5f );

					bGetExtraDamage = true;
				}
			}
		} break;

	case CX2DamageManager::EDT_PAIN:
		{
			//if ( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				// Accumulation�� �ƹ��͵� ������ (���� ���� extraDamage�� ó�� ���۵� PAIN �̸�)
				if ( 0 == extraDamageData.m_Accumulation )
				{
					// ���� ĳ���Ϳ��� �ɸ� ����� ������ üũ�Ѵ�.

					vector<KBuffFactor> VecDebuffFactor;
					GetBuffFactorOnlyDebuff(VecDebuffFactor);

					int iNumOfExtraDamage = VecDebuffFactor.size();
					if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f )
						++iNumOfExtraDamage;
					// ������ �ɷ��ִ� Pain�� ���ų�, �־ ���� ���� �ɸ� Pain���� ����Ƚ���� ������ ����
					if ( iNumOfExtraDamage > m_ExtraDamagePack.m_Pain.m_Accumulation )
					{
						m_ExtraDamagePack.m_Pain = extraDamageData;
						m_ExtraDamagePack.m_Pain.m_Accumulation = iNumOfExtraDamage;
						m_ExtraDamagePack.m_Pain.m_optrGameUnit	= pDamageData->optrAttackerGameUnit;
					}
				}
				// Accumulation�� 0�� �ƴϸ� (�ٸ� ������ ���� ���� �� ���̸�)
				else
				{
					// ������ EDT_PAIN�� �ɷ� �־��ٸ�, ���� Ƚ���� �� ���� ���� ���� ���� ��
					if ( extraDamageData.m_Accumulation > m_ExtraDamagePack.m_Pain.m_Accumulation)
					{
						m_ExtraDamagePack.m_Pain = extraDamageData;
						m_ExtraDamagePack.m_Pain.m_optrGameUnit	= pDamageData->optrAttackerGameUnit;
					}
				}
			}
		} break;

	case CX2DamageManager::EDT_HEAD_SHOT:
		{
			SetHittedExtraDamageHeadShot( true );
			bGetExtraDamage = true;
		} break;
		//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

	case CX2DamageManager::EDT_BIGHEAD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false )
				{
					float fDuration = extraDamageData.m_fTime;

					// ����/�̼�/����/���ݷ� ����
					m_ExtraDamagePack.m_Aging = extraDamageData;
					m_ExtraDamagePack.m_Aging.m_fTime = fDuration;

					// ���ݷ� ����
// #ifdef SKILL_CASH_10_TEST
// 					SetAttackBuffRate( extraDamageData.m_fDamageRate );
// 					SetAttackBuffTime( fDuration );
// #endif

					m_fExtraBigHead = fDuration;

					bGetExtraDamage = true;
				}
			}

		} break;
		// oasis907 : ����� [2011.6.16] û 2�� ���� - ���̾� �ȶ��
	case CX2DamageManager::EDT_TRANSIT_DEBUFF:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				pAttackerUnit->ProcessTransitDebuff( pDamageData, this );
			}
			//bGetExtraDamage = true; extradamage�ƴ�
		} break;

	case CX2DamageManager::EDT_ENCHANT_BLAZE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false &&
					IsImmuneToEnchant( CX2EnchantItem::ET_BLAZE ) == false )
				{
					m_ExtraDamagePack.m_EnchantBlaze = extraDamageData;
					g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EnchantBlazeAttack", pDamageData->impactPoint );
					UpDownCrashCamera( 20.0f, 0.5f );
					bGetExtraDamage = true;
				}
				else
				{
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( L"RESIST", GetHeadBonePos(), GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		} break;

	case CX2DamageManager::EDT_ENCHANT_FROZEN:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false &&
					IsImmuneToEnchant( CX2EnchantItem::ET_WATER ) == false )
				{
					m_ExtraDamagePack.m_EnchantFrozen = extraDamageData;
					bGetExtraDamage = true;
				}
				else
				{
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( L"RESIST", GetHeadBonePos(), GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		} break;

	case CX2DamageManager::EDT_ENCHANT_POISON:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false &&
					IsImmuneToEnchant( CX2EnchantItem::ET_NATURE ) == false )
				{
					m_ExtraDamagePack.m_EnchantPoison = extraDamageData;
					bGetExtraDamage = true;
				}
				else
				{
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( L"RESIST", GetHeadBonePos(), GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		} break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				if ( IsImmuneBySocketOption( extraDamageData.m_ExtraDamageType ) == false &&
					IsImmuneToEnchant( CX2EnchantItem::ET_LIGHT ) == false )
				{
					m_ExtraDamagePack.m_EnchantShock = extraDamageData;
					bGetExtraDamage = true;
				}
				else
				{
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( L"RESIST", GetHeadBonePos(), GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		} break;

	case CX2DamageManager::EDT_WIND_STORM:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_WIND_STORM", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;

	case CX2DamageManager::EDT_EARTH_QUAKE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_EarthQuake = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				m_ExtraDamagePack.m_EarthQuake.m_fTime = fDuration;

				SetMoveSpeedFactor(0.5f, m_ExtraDamagePack.m_EarthQuake.m_fTime);
				SetAnimSpeedFactor(0.75f, m_ExtraDamagePack.m_EarthQuake.m_fTime);
			}
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 outPos( 0.0f, 0.0f, 0.0f );
				int iLineIndex = -1;

				CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
				if( NULL != pLineMap )
				{
					if ( true == pLineMap->GetDownNearestWithBound( GetPos(), 100.f, &outPos, &iLineIndex ) )
					{
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_EARTH_QUAKE", pAttackerUnit, NULL, false,
							-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, outPos,
							pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
					}
				}
			} // if
		} break;


	case CX2DamageManager::EDT_CURSE_CLOUD:
		{
			if( null != pDamageData->optrAttackerGameUnit )
			{
				const float MAGIC_POWER_RATE = 1.f;
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pDamageData->optrAttackerGameUnit.GetObservable(), L"EXTRA_DAMAGE_CURSE_CLOUD", MAGIC_POWER_RATE,
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition().y );

				UpDownCrashCamera( 20.0f, 0.5f );

				bGetExtraDamage = true;
			}
		} break;

		case CX2DamageManager::EDT_RUNJUMP_SPEED:
			{
				float fDuration = extraDamageData.m_fTime;

				SetVecMoveSpeedFactor(extraDamageData.m_fRunJumpRate, fDuration, CX2GameUnit::SFI_SPEED_9, CX2GameUnit::SFST_UPDATE);
				SetVecJumpSpeedFactor(extraDamageData.m_fRunJumpRate, fDuration, CX2GameUnit::SFI_SPEED_9, CX2GameUnit::SFST_UPDATE);
			}
			break;

	case CX2DamageManager::EDT_FLASH_BANG:
		{
			m_ExtraDamagePack.m_FlashBang = extraDamageData;
			float fDuration = extraDamageData.m_fTime;

            if ( CX2DamageEffect::CEffect*   pAttackerEffect = g_pX2Game->GetDamageEffect()->GetInstance( pDamageData->hAttackerEffect ) )
			{
				const float CONST_FLASH_BANG_HIGH = 400.f;
				const float CONST_FLASH_BANG_LOW = 800.f;

				D3DXVECTOR3 vFlashCenterPos = pAttackerEffect->GetPos();
				float fDistanceFromCenter3Sq = D3DXVec3LengthSq( &(vFlashCenterPos - GetPos()) );

				if ( fDistanceFromCenter3Sq > CONST_FLASH_BANG_LOW * CONST_FLASH_BANG_LOW )
					fDuration *= 0.25f;
				else if( fDistanceFromCenter3Sq > CONST_FLASH_BANG_HIGH * CONST_FLASH_BANG_LOW )
					fDuration *= 0.5f;
				else
					fDuration *= 1.f;

				if( IsMyUnit() == true )
				{
					g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWhiteOut );
					m_hSeqWhiteOut = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  L"WhiteOutExtraDamageStart", D3DXVECTOR3(0,0,0) );
				}
				m_ExtraDamagePack.m_FlashBang.m_fTime = fDuration;
				m_bWhiteOut = true;
			}
		} break;

	case CX2DamageManager::EDT_FLAME_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FLAME_SMASH", pAttackerUnit->GetPowerRate(),
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition().y );
			}
		} break;

	case CX2DamageManager::EDT_FIRE_WRAITH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				if ( true == pAttackerUnit->GetIsRight() )
				{
					g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FIRE_WRAITH", pAttackerUnit->GetPowerRate(),
						pDamageData->impactPoint, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree() );
				}
				else
				{
					g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FIRE_WRAITH_INVERSE", pAttackerUnit->GetPowerRate(),
						pDamageData->impactPoint, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree() );
				}
			}
		} break;

	case CX2DamageManager::EDT_START_OF_DELAYED_FIRING:
		{
			float	fDuration	= extraDamageData.m_fTime;		/// ���� �ð�
			int		iArrowType	= extraDamageData.m_iLevel;		/// ȭ�� Ÿ��( 0 = ������ ȭ��, 1 = ������ ��ȣź )

			CX2DamageManager::StartOfDelayedFiringData* pData = NULL;

			if( iArrowType == 0 )	/// ������ ȭ��
				pData = new CX2DamageManager::StartOfDelayedFiringData( CX2DamageEffect::DET_ARROW_OF_EXPLOSION_DELAY,
					pDamageData->optrAttackerGameUnit->GetUnitIndex(), 100, -1, fDuration );
			else					/// ������ ��ȣź
				pData = new CX2DamageManager::StartOfDelayedFiringData( CX2DamageEffect::DET_START_OF_DELAYED_FIRING,
					pDamageData->optrAttackerGameUnit->GetUnitIndex(), -1, -1, fDuration );

			if( true == GetStartOfDelayedFiringData().empty() )		/// ������ ��ȣź EDT ������ Ȯ�� ����Ʈ
				m_hActiveStartOfDelayedFiringEDT = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Rena_Night_Watcher_ExplosionArrow_Active_EDT", (CX2GameUnit*) this );

			if( NULL != pData )		/// ������ ��ȣź �߰�
				AccessStartOfDelayedFiringData().push_back( pData );

			if( iArrowType == 0 )	/// ������ ȭ���� ���� ȭ�� ���� ����
			{
				if( null != pDamageData->optrAttackerGameUnit &&
					CX2GameUnit::GUT_USER == pDamageData->optrAttackerGameUnit->GetGameUnitType() )
				{
					CX2GUUser* pAttacker = static_cast<CX2GUUser*>( pDamageData->optrAttackerGameUnit.GetObservable() );

					if( NULL != pAttacker )
						pAttacker->UpdateStartOfDelayedFiring( true );
				}
			}

			SetStartOfDelayedFiringEmoticon();		/// �̸�Ƽ�� ����
		}break;

	case CX2DamageManager::EDT_STARLIGHT_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_STARLIGHT_SMASH", pAttackerUnit->GetPowerRate(),
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition().y );
			}
		} break;

	case CX2DamageManager::EDT_FALLING_ROCK:/// ���� ��Ʈ ȿ�� - ���� ��ȯ
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				vPos.y += 300.f;

				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EQUIP_FALLING_ROCK", 1.f,
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition().y );
			}
		} break;

	case CX2DamageManager::EDT_SANDSTORM:/// ���� ��Ʈ ȿ�� - �𷡵�ǳ ��ȯ
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				vPos.y = GetLandPosition().y;

				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EQUIP_SANDSTORM", 1.f,
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition().y );
			}
		} break;
	case CX2DamageManager::EDT_WIND_VIBRATION:/// ���� Īȣ ���� Ÿ��
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_WIND_VIBRATION", 1.f,
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition().y );
			}
		} break;

	case CX2DamageManager::EDT_BREEZE_ACCEL:///���� Īȣ ���ٶ��� ��� ����
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pAttackerUnit->GetBonePos(L"Bip01");
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_BUFF_BREEZE_ACCEL", 1.f,
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition().y );
			}
		} break;
	case CX2DamageManager::EDT_EVIL_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_EVIL_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;
	case CX2DamageManager::EDT_SPIRIT_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_SPIRIT_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;
	case CX2DamageManager::EDT_GHOST_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pAttackerUnit->GetBonePos(L"Bip01");
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_GHOST_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;

	case CX2DamageManager::EDT_ACTIVE_DEBUFF:	/// Ÿ�ݽ� Ư�� ������� ����
		{
			CX2GameUnit* pDefenderUnit = pDamageData->optrDefenderGameUnit.GetObservable();

			if ( NULL != pDefenderUnit && NULL != extraDamageData.m_ptrBuffFactor )
			{
				CX2BuffFactorPtr pCloneBuffFactorPtr =  extraDamageData.m_ptrBuffFactor->GetClonePtr( 0 );

				if ( NULL != pCloneBuffFactorPtr )
				{
					pCloneBuffFactorPtr->SetGameUnitBuffFactor( this );
					pDefenderUnit->SetBuffFactorToGameUnit( *pCloneBuffFactorPtr );
				}
			}
		} break;

	default:
		{
			ASSERT( !"EDT_NONE extra damage" );
		} break;
	}

//{{ kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)
	if ( bGetExtraDamage == true )
	{
		ProcessSocketEffectEx( CX2SocketItem::SIEE_STAT_UP, this  );
		ProcessSocketEffectEx( CX2SocketItem::SIEE_UP_MANA_RECOVERY_VALUE, this  );
	}

//}} kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)
}


void CX2GUUser::DamageReactStateChange( CX2DamageManager::DamageData* pDamageData,
										int iRevengeAttack /*= -1*/,
										int iAvoidance /*= -1*/  )
{
	if( IsMyUnit() == false )
		return;

	if ( m_bRidingMachine == true )
		return;

	// note!!! �� �κ� �ڵ尡 �� �ʿ����� ������ ã�ƾ� ��
	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
	{
		if( true == g_pData->GetUIManager()->GetUIQuickSlot()->GetWaiting_EGS_USE_QUICK_SLOT_ACK() )
			return;
	}

	// ���̺� �и�
	if( m_bParryingState == true )
	{
		m_bParryingState = false;

		CX2GURaven *pRaven = (CX2GURaven*)this;
		if( pRaven != NULL )
		{
			if( pRaven->CanParrying() == false )
			{
				if( pDamageData->reActResult == CX2DamageManager::RT_SMALL_DAMAGE )
					StateChange(CX2GURaven::RSI_PARRYING_SMALL, true);
				else
					StateChange(CX2GURaven::RSI_PARRYING, true);
			}
			return;
		}
	}


	if ( true == m_bQuickenGuardBlocking )
	{
		m_bQuickenGuardBlocking = false;

		CX2GUAra *pAra = static_cast<CX2GUAra*>(this);

		if ( NULL != pAra )
		{
			if ( CX2DamageManager::RT_SMALL_DAMAGE == pDamageData->reActResult )
				StateChange( CX2GUAra::ASI_P_ASD_QUICKEN_GUARD_SMALL, true );
			else
				StateChange( CX2GUAra::ASI_P_ASD_QUICKEN_GUARD, true );
		}
		return;
	}

	if ( true == m_bQuickenGuardTurning )
	{
		m_bQuickenGuardTurning = false;

		CX2GUAra *pAra = static_cast<CX2GUAra*>(this);

		if ( NULL != pAra )
		{
			m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
			m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
			m_FrameDataFuture.unitCondition.bAttackerFront = true;
			m_PhysicParam.nowSpeed.x *= -1.f;
		}
	}

	/// ���̺� ��ũ���� �ǽ�Ʈ - ����Ʈ ũ���� ���� ����
	if ( true == m_bIsLimitCrusherCharge )
	{
		if ( true == pDamageData->bCanRevenge )
			StateChange( CX2GURaven::RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK );
		else
			StateChange( CX2GURaven::RSI_A_RRF_LIMIT_CRUSHER_AVOID_ATTACK );

		return;
	}

	CX2GameUnit*	pAttackerUnit = NULL;

	if( null != pDamageData->optrAttackerGameUnit )
	{
		pAttackerUnit		= pDamageData->optrAttackerGameUnit.GetObservable();
	}

	m_fGroggyTime = 0.f;
	switch( pDamageData->reActResult )
	{
	case CX2DamageManager::RT_DUMMY_DAMAGE:
	case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
		break;
		break;

	case CX2DamageManager::RT_SMALL_DAMAGE:
//{{ kimhc // 2011.1.8 // û 1�� ����
	case CX2DamageManager::RT_FLIP_SAME_DIR:
//}} kimhc // 2011.1.8 // û 1�� ����
		//{{ JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?

	case CX2DamageManager::RT_FLIP_DIFF_DIR:

		SetNowHit( true );
		//}} JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?

		if ( IsSuperArmor() )
			break;

		if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y < LINE_RADIUS * 5.0f
			|| (pAttackerUnit != NULL && pDamageData->attackerType == CX2DamageManager::AT_UNIT && pAttackerUnit->GetPos().y - pAttackerUnit->GetUnitCondition().landPosition.y < LINE_RADIUS * 3.0f) )
		{
			if( m_PhysicParam.nowSpeed.y > 0.0f )
				m_PhysicParam.nowSpeed.y = 0.0f;
			if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT, true );
				else
					StateChange( USI_DAMAGE_SMALL_FRONT, true );
			}
			else
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_BACK, true );
				else
					StateChange( USI_DAMAGE_SMALL_BACK, true );
			}
		}
		else
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
			/// Ż�� ž���� ���� �ǰݽ�, Ż���� ������Ʈ�� ��ȯ�ǵ��� ����
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
			else
				StateChange( USI_DAMAGE_AIR_SMALL, true );
		}
		break;

	case CX2DamageManager::RT_GROGGY:
		m_fGroggyTime = pDamageData->fGroggyTime;
		StateChange( USI_DAMAGE_GROGGY, true );
		break;


	case CX2DamageManager::RT_REFLECT:
	case CX2DamageManager::RT_BIG_DAMAGE:
		//{{ JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?
		SetNowHit( true );
		//}} JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?

		if ( IsSuperArmor() )
			break;

		if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y < LINE_RADIUS * 5.0f
			|| (pAttackerUnit != NULL && pDamageData->attackerType == CX2DamageManager::AT_UNIT && pAttackerUnit->GetPos().y - pAttackerUnit->GetUnitCondition().landPosition.y < LINE_RADIUS * 3.0f) )
		{
			if( m_PhysicParam.nowSpeed.y > 0.0f )
				m_PhysicParam.nowSpeed.y = 0.0f;
			if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT, true );
				else
					StateChange( USI_DAMAGE_BIG_FRONT, true );
			}
			else
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_BACK, true );
				else
					StateChange( USI_DAMAGE_BIG_BACK, true );
			}
		}
		else
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

			/// Ż�� ž���� ���� �ǰݽ�, Ż���� ������Ʈ�� ��ȯ�ǵ��� ����
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
			else
				StateChange( USI_DAMAGE_AIR_SMALL, true );
		}
		break;

	case CX2DamageManager::RT_DOWN:
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_SUPER_AMOR_FOR_RIDING ) );

		if ( IsSuperArmor() )
			break;

		if( true == IsOnSomethingFuture() )
		{
			if( m_PhysicParam.nowSpeed.y > 0.0f )
				m_PhysicParam.nowSpeed.y = 0.0f;
			if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT, true );
				else
					StateChange( USI_DAMAGE_DOWN_FRONT, true );
			}
			else
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_BACK, true );
				else
					StateChange( USI_DAMAGE_DOWN_BACK, true );

			}
		}
		else
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

			/// Ż�� ž���� ���� �ǰݽ�, Ż���� ������Ʈ�� ��ȯ�ǵ��� ����
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
			else
				StateChange( USI_DAMAGE_AIR_DOWN, true );
		}

//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
		SetHoldStateID( USI_DAMAGE_AIR_FALL );
//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

		SetNowForceDown(0.f);
		break;

	case CX2DamageManager::RT_UP:
		//{{ JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?
		SetNowHit( true );
		//}} JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_SUPER_AMOR_FOR_RIDING ) );

		if ( IsSuperArmor() )
			break;

		if( m_FlyCount > 2 )
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y / (m_FlyCount - 1);
		}
		else
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
		}

		ResetOnSomethingFuture();

		if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
			else
				StateChange( USI_DAMAGE_AIR_UP, true );
		}
		else
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_BACK, true );
			else
				StateChange( USI_DAMAGE_AIR_UP, true );

		}
		break;

	case CX2DamageManager::RT_FLY:
		//{{ JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?
		SetNowHit( true );
		//}} JHKang / ������ / 2010/12/16 / ���� �ǰ� �����ΰ�?

		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_SUPER_AMOR_FOR_RIDING ) );

		if ( IsSuperArmor() )
			break;

		if( m_FlyCount > 2 )
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y / (m_FlyCount - 1);
		}
		else
		{
			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
		}

		ResetOnSomethingFuture();

//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
		if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
		{
			if ( true == GetRidingOn() )
			{
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
				SetHoldStateID( USI_RIDING_DAMAGE_FRONT );
				SetGrapStateID ( USI_RIDING_DAMAGE_FRONT );
			}
			else
			{
				StateChange( USI_DAMAGE_AIR_FLY_FRONT, true );
				SetHoldStateID( USI_DAMAGE_AIR_FLY_FRONT );
				SetGrapStateID ( USI_DAMAGE_AIR_FLY_FRONT );
			}
		}
		else
		{
			if ( true == GetRidingOn() )
			{
				StateChange( USI_RIDING_DAMAGE_BACK, true );
				SetHoldStateID( USI_RIDING_DAMAGE_BACK );
				SetGrapStateID ( USI_RIDING_DAMAGE_BACK );
			}
			else
			{
				StateChange( USI_DAMAGE_AIR_FLY_BACK, true );
				SetHoldStateID( USI_DAMAGE_AIR_FLY_BACK );
				SetGrapStateID ( USI_DAMAGE_AIR_FLY_BACK );
			}
		}
//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

		break;

		//{{ kimhc // 2010.2.8 // ��ġ���� ���� ���� ��� ���� �ø��� ����
	case CX2DamageManager::RT_DRAG_UP:
		{
			if ( IsSuperArmor() )
				break;

			m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

			ResetOnSomethingFuture();

			if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT, true );
				else
					StateChange( USI_DAMAGE_AIR_FLY_FRONT, true );
			}
			else
			{
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_BACK, true );
				else
					StateChange( USI_DAMAGE_AIR_FLY_BACK, true );
			}
		}
		break;
		//}} kimhc // 2010.2.8 // ��ġ���� ���� ���� ��� ���� �ø��� ����

	case CX2DamageManager::RT_REVENGE:
		if(iRevengeAttack != -1)
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
			m_PhysicParam.nowSpeed.y = 0.0f;

			/// ���� �ݻ� ������ ������ �̶��, �ǰ� ����� ���ش�.
			if ( false == m_vecReflectMagicByBuffPtr.empty() )
				return;
			else

			StateChange( iRevengeAttack, true );
		}
		break;






	case CX2DamageManager::RT_AVOIDANCE:
		if( -1 != iAvoidance )
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
			m_PhysicParam.nowSpeed.y = 0.0f;

			StateChange( iAvoidance, true );
		}
		break;
	}



	switch( pDamageData->reActResult )
	{
	case CX2DamageManager::RT_SMALL_DAMAGE:
	case CX2DamageManager::RT_BIG_DAMAGE:
		{
			if( true == pDamageData->m_bFlipDirOnHit )
			{
				m_FrameDataFuture.syncData.bIsRight			= !m_FrameDataFuture.syncData.bIsRight;
				m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;



			}
		} break;

		//{{ kimhc // 2011.1.8 // û 1�� ����
	case CX2DamageManager::RT_FLIP_SAME_DIR:	// ������ ����
		{
			// ���۾ƸӰ� �ƴϸ�
			if ( false == IsSuperArmor() )
			{
				bool bIsAttackerUnitInRightDirection = false; // ������ ����

				if ( null != pDamageData->optrAttackerGameUnit )
				{
					bIsAttackerUnitInRightDirection = pDamageData->optrAttackerGameUnit->GetIsRight();
				} // if

				// ������ ����� NPC�� ������ �ݴ��̸�
				if ( bIsAttackerUnitInRightDirection != m_FrameDataFuture.syncData.bIsRight )
				{
					// ȸ�� ��Ŵ
					m_FrameDataFuture.syncData.bIsRight			= !m_FrameDataFuture.syncData.bIsRight;
					m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
				} // if

			} // if
		} break;
		//}} kimhc // 2011.1.8 // û 1�� ����

	case CX2DamageManager::RT_FLIP_DIFF_DIR:	// ������ ����
	case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
		{
			// ���۾ƸӰ� �ƴϸ�
			if ( false == IsSuperArmor() )
			{
				bool bIsAttackerUnitInRightDirection = false; // ������ ����

				if ( null != pDamageData->optrAttackerGameUnit )
				{
					bIsAttackerUnitInRightDirection = pDamageData->optrAttackerGameUnit->GetIsRight();
				} // if

				// ������ ����� NPC�� ������ ���� �����̸�
				if ( bIsAttackerUnitInRightDirection == m_FrameDataFuture.syncData.bIsRight )
				{
					// ȸ�� ��Ŵ
					m_FrameDataFuture.syncData.bIsRight			= !m_FrameDataFuture.syncData.bIsRight;
					m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
				} // if

			} // if
		} break;
	}



	if( true == IsDamagedReaction( pDamageData ) )
	{
		if( DamageFlushMp()== true )
		{
			DetonationStateChange( pDamageData );
		}
		else if( FlushForceDown() == true )
		{
			if( true == IsOnSomethingFuture() )
			{
				if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
				{
					if ( true == GetRidingOn() )
						StateChange( USI_RIDING_DAMAGE_FRONT, true );
					else
						StateChange( USI_DAMAGE_DOWN_FRONT, true );
				}
				else
				{
					if ( true == GetRidingOn() )
						StateChange( USI_RIDING_DAMAGE_BACK, true );
					else
						StateChange( USI_DAMAGE_DOWN_BACK, true );

				}
			}
			else
			{
				m_PhysicParam.nowSpeed.y = 0.0f;

				/// Ż�� ž���� ���� �ǰݽ�, Ż���� ������Ʈ�� ��ȯ�ǵ��� ����
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT, true );
				else
					StateChange( USI_DAMAGE_AIR_DOWN_INVINCIBLE, true );
			}

			if ( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.0f )
			{
				m_bReserveForceDownForShock = true;
			}
		}
	}

	if( pDamageData->m_bNoDetonation == true )
	{
		SetNoDetonation( true );
	}
}
/** @function : DetonationStateChange()
	@brief : ������̼ǿ� ���� ������Ʈ ���� ó��.
			 �����ý� ���Ǳ� ȿ�� ������ ���� �����Լ� ó��
*/
/*virtual*/ void CX2GUUser::DetonationStateChange( CX2DamageManager::DamageData* pDamageData )
{
	if( NULL == pDamageData )
		return;

	/// DT_BUFF�϶��� ���� ���ϰ� ����
	if ( CX2DamageManager::DT_BUFF == pDamageData->damageType )
		return;

	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.unitCondition.bAttackerFront == true )
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT, true );
			else
				StateChange( USI_DAMAGE_DOWN_FRONT, true );
		}
		else
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_BACK, true );
			else
				StateChange( USI_DAMAGE_DOWN_BACK, true );
		}
	}
	else
	{
		m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

		/// Ż�� ž���� ���� �ǰݽ�, Ż���� ������Ʈ�� ��ȯ�ǵ��� ����
		if ( true == GetRidingOn() )
			StateChange( USI_RIDING_DAMAGE_FRONT, true );
		else
			StateChange( USI_DAMAGE_AIR_DOWN_INVINCIBLE, true );
	}
	SetStopTime( 0.7f );
	g_pKTDXApp->GetDGManager()->ClearScreen( 1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpactTick",
		pDamageData->impactPoint,
		200, 200, 2, 20 );

	if( pSeq != NULL )
	{
		pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
	}
}
//{{ kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

float CX2GUUser::CalculateHpUpPercentBySocketOption( const vector<D3DXVECTOR2>& vecHpUpPercentBySocketOption )
{
	float fHpPercentToBeUp = 0.0f;
	int iRandomSeed = 0;
	BOOST_TEST_FOREACH( const D3DXVECTOR2&, val, vecHpUpPercentBySocketOption )
	{
		if ( GetRandomFloat( CKTDXRandomNumbers::SRO_ADD_MANA_ON_ATTACK + iRandomSeed++ ) < val.x )
			fHpPercentToBeUp += val.y;
	}

	return fHpPercentToBeUp;
}

float CX2GUUser::CalculateMpUpBySocketOption( const vector<D3DXVECTOR2>& vecMpUpBySocketOption )
{
	float fMpUp = 0.0f;
	int iRandomSeed = 0;
	BOOST_TEST_FOREACH( const D3DXVECTOR2&, val, vecMpUpBySocketOption )
	{
		if ( GetRandomFloat( CKTDXRandomNumbers::SRO_ADD_MANA_ON_ATTACK + iRandomSeed++ ) < val.x )
			fMpUp += val.y;
	}

	return fMpUp;
}


//}} kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

void CX2GUUser::ProcessAttackResult()
{
	if ( CX2DamageManager::AT_SPECIAL != m_DamageData.attackType )
	{
		// ���ݽ� mp up ���
		//float fPercentMPUp = 0.0f;


		//{{ kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

		// ���ݽ� ���� �� HP Percent
		float fHPPercentToBeUp = CalculateHpUpPercentBySocketOption( m_SocketData.m_vecHpUpByAttack );
		float fHPUp = GetMaxHp() * fHPPercentToBeUp;

		if( 0.0f < fHPUp )
		{
			UpNowHp( fHPUp );

			std::wstringstream strStream;
			strStream << L"HP+ " << (int)fHPUp;
			D3DXVECTOR3 pos = GetPos();
			pos.y += 50.0f;
			if( NULL != g_pData->GetPicCharGreen() )
				g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}

		float fMPUp = CalculateMpUpBySocketOption( m_SocketData.m_vecMpUpByAttack );

		// ���ݽ� ������ MP ���� ������
		if( 0.0f < fMPUp )
		{
			UpNowMp( fMPUp );

			std::wstringstream strStream;
			strStream << L"MP+ " << (int)fMPUp;
			D3DXVECTOR3 pos = GetPos();
			pos.y += 50.0f;
			if( NULL != g_pData->GetPicCharBlue() )
				g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}

		//}} kimhc // 2011-05-25 // SetOption, TitleOption���� ���� �Ǵ� option ��� itemOption�� ���� �ǵ��� ����

//{{ kimhc // 2010.4.28 // ��д��� �۾�(���� ���� ȿ��)
		ProcessSocketEffectEx( CX2SocketItem::SIEE_ATTACK_SPEED_UP, this  );
//}} kimhc // 2010.4.28 // ��д��� �۾�(���� ���� ȿ��)
	}

}


void CX2GUUser::AttackResult()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	KTDXPROFILE();
	if( m_DamageData.reActResult == CX2DamageManager::RT_NO_DAMAGE || m_DamageData.reActResult == CX2DamageManager::RT_SENSOR )
		return;

	CX2GameUnit*	pDefenderUnit = NULL;
	if( null != m_DamageData.optrDefenderGameUnit )
	{
		pDefenderUnit		= m_DamageData.optrDefenderGameUnit.GetObservable();

		SetGameUnitAttackedByMe( pDefenderUnit );
	}

	D3DXVECTOR3 dirUnitToAttacker = D3DXVECTOR3(0,0,0);
	if( pDefenderUnit != NULL )
	{
		dirUnitToAttacker = pDefenderUnit->GetPos() - GetPos();
        dirUnitToAttacker.y = 0.f;
	}


	if( D3DXVec3Dot( &GetDirVector(), &dirUnitToAttacker ) >= 0 )
		m_FrameDataNow.unitCondition.bAttackerFront = true;
	else
		m_FrameDataNow.unitCondition.bAttackerFront = false;

	if( m_FrameDataNow.syncData.bIsRight == false )
	{
		m_FrameDataNow.unitCondition.bAttackerFront = !m_FrameDataNow.unitCondition.bAttackerFront;
	}

	m_FrameDataFuture.unitCondition.bAttackerFront = m_FrameDataNow.unitCondition.bAttackerFront;


	bool bCheckRevenge = false;

	m_DamageLightTime = 0.2f;


	if( IsMyUnit() == true )
	{
		if( m_bUpMPThisFrame == false
		&& CX2DamageManager::DT_BUFF != m_DamageData.damageType
		)
		{
			int		redAdv						= 0;
			int		blueAdv						= 0;
			float	fIncreaseMpRate				= 1.0f;		// ��*Ÿ�ݽ� ���� + ����ġ(1.0�� ������ ���� ����)
			float	fIncreaseSoulRate			= 1.0f;		// ��*Ÿ�ݽ� ���� + ����ġ

			if( CX2Unit::UT_ELSWORD == GetUnit()->GetType() )
			{
				float fWSPGain = 0.f;
				fWSPGain = g_pData->GetDamageManager()->CalcWayOfSwordPoint(m_DamageData.m_WayofSwordType, m_DamageData.damage.GetAllDamage(), GetWayOfSwordState() );
				SetWayOfSwordPoint( GetWayOfSwordPoint() + fWSPGain );

				fIncreaseMpRate *= g_pData->GetDamageManager()->CalcVigorMpUpOnAttack(m_DamageData.m_WayofSwordType, GetWayOfSwordState() );
			}
			else if( CX2Unit::UT_ELESIS == GetUnit()->GetType() )
			{
				float fWSPGain = 0.f;
				fWSPGain = g_pData->GetDamageManager()->CalcWayOfSwordPoint(m_DamageData.m_WayofSwordType, m_DamageData.damage.GetAllDamage(), GetWayOfSwordState() );
				SetWayOfSwordPoint( GetWayOfSwordPoint() + fWSPGain );

				fIncreaseMpRate *= g_pData->GetDamageManager()->CalcVigorMpUpOnAttack_Elesis(m_DamageData.m_WayofSwordType, GetWayOfSwordState() );
			}


			//{{ kimhc // 2009-11-17 // ��� ��ų �� ��������, ���ߵ� ����, �ݳ�, �г��ع�
			const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

			// ���ߵ� ����
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER, pUnitData->m_byMemberShipGrade );

			if( NULL != pSkillTemplet )
			{
				const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

				fIncreaseMpRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillTempletLevel );
			}

			// �г��ع�
			pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_LIBERATION_OF_ANGER, pUnitData->m_byMemberShipGrade );

			if( NULL != pSkillTemplet )
			{
				const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

				float fRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_ON_HIT_REL, iSkillTempletLevel );

				fIncreaseSoulRate += CalculateIncreasingRate( fRate );
			}
			//}} kimhc // 2009-11-17 // ��� ��ų �� ��������, ���ߵ� ����, �ݳ�, �г��ع�

//{{ kimhc // 2010.11.3 // ���� - ��ȭ�� �г�
			const CX2SkillTree::SkillTemplet* pSkillTempletSpritualizedFury
				= pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_RRF_SPIRITUALIZED_FURY );

			if ( NULL != pSkillTempletSpritualizedFury )
			{
				const int iSkillTempletLevel = max( 1, pUnitData->m_UserSkillTree.GetSkillLevel( pSkillTempletSpritualizedFury->m_eID, true ) );	/// ��ų ����

				float fRate = pSkillTempletSpritualizedFury->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_ON_HIT_REL, iSkillTempletLevel );

				fIncreaseSoulRate += CalculateIncreasingRate( fRate );
			} // if
//}} kimhc // 2010.11.3 // ���� - ��ȭ�� �г�

			if( g_pX2Room != NULL && g_pData->GetPVPRoom() != NULL )
			{
				if( g_pX2Room->GetRoomType() == CX2Room::RT_PVP )
				{
					if( g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM
						|| g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_TEAM_DEATH )
					{
						int redNum		= g_pX2Room->GetTeamNum( CX2Room::TN_RED, true );
						int blueNum		= g_pX2Room->GetTeamNum( CX2Room::TN_BLUE, true );
						if( redNum > blueNum )
						{
							blueAdv = redNum - blueNum;
						}
						else if( redNum < blueNum )
						{
							redAdv = blueNum - redNum;
						}
					}
				}
			}


			//{{ dmlee 2008.04.11 ĳ�� ������ �����
			if( 0.f != m_CashItemAbility.m_fUpMPAtAttackOrDamage )
			{
				UpNowMp( m_CashItemAbility.m_fUpMPAtAttackOrDamage );
			}
			//}} dmlee 2008.04.11 ĳ�� ������ �����

			ProcessAttackResult();

			float fUpMPScaleBySkill = 1.f;
			if( CX2DamageManager::AT_SPECIAL != m_DamageData.attackType )
			{
				int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ESK_GET_MANA_UP, true );

				if( iSkillLevel > 0 )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ESK_GET_MANA_UP );

					if( NULL != pSkillTemplet )
					{
						fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
					}
				}
			}

			if( CX2DamageManager::AT_SPECIAL != m_DamageData.attackType )
			{
				int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE, true );
				if( iSkillLevel > 0 )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LGA_COMMUNE_OF_NATURE );
					if( NULL != pSkillTemplet )
					{
						fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
					}
				}
			}

			if( CX2DamageManager::AT_SPECIAL != m_DamageData.attackType )
			{
				/// ���� ��ݿ� MP ȸ���� �߰�
				int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LGA_VIBRATION_SHOOTING, true );

				if( iSkillLevel > 0 )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LGA_VIBRATION_SHOOTING );
					if( NULL != pSkillTemplet )
					{
						fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
					}
				}

				/// ������� ���� ��ȭ
				iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CIP_ADVANCED_METABOLISM_BOOST, true );

				if( iSkillLevel > 0 )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CIP_ADVANCED_METABOLISM_BOOST );
					if( NULL != pSkillTemplet )
					{
						fUpMPScaleBySkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
					}
				}
			}



			/// �ֵ��� ��, Ÿ�ݽ� DP ��ġ �÷� ����
			if ( NULL != GetUnit() && CX2Unit::UT_ADD == GetUnit()->GetType() )
			{
				if ( false == GetUpDPByAttackThisFrame() )	/// �����Ӵ� �ѹ��� DP ȸ�� ó��
				{
					UpNowDPValueByHit( m_DamageData );
					SetUpDPByAttackThisFrame( true );
				}
			}

			if( GetTeam() == CX2Room::TN_RED )
			{
				UpNowMpPerHitOthers( m_DamageData.fHitAddMP, redAdv, fUpMPScaleBySkill * fIncreaseMpRate );


				if( GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f &&
					m_DamageData.m_fIncreaseMP <= 0.f )
				{
					//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
					/// ���� ���� ������ ����
					UpNowSoul( ( (1.0f * (1.0f + GetHyperChargeSpeed() ) ) + (2.0f * redAdv) ) * fIncreaseSoulRate );
					//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
				}
			}
			else if( GetTeam() == CX2Room::TN_BLUE )
			{
				UpNowMpPerHitOthers( m_DamageData.fHitAddMP, blueAdv, fUpMPScaleBySkill * fIncreaseMpRate );


				if( GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f &&
					m_DamageData.m_fIncreaseMP <= 0.f )

				{
					//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
					/// ���� ���� ������ ����
					UpNowSoul( ( (1.0f * (1.0f + GetHyperChargeSpeed() ) ) + (2.0f * blueAdv) ) * fIncreaseSoulRate );
					//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
				}
			}
			else
			{
				UpNowMpPerHitOthers( m_DamageData.fHitAddMP, 0, fUpMPScaleBySkill * fIncreaseMpRate );


				if( GetHyperModeCount() < 3 && m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f &&
					m_DamageData.m_fIncreaseMP <= 0.f )
				{
					//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
					UpNowSoul( ( (1.0f * (1.0f + GetHyperChargeSpeed() ) ) ) * fIncreaseSoulRate );
					//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
				}
			}
			m_bUpMPThisFrame = true;
		}

		SetStopTime( m_DamageData.fStopTimeAtt );
		// oasis907 : ����� [2010.11.1] // �� �����̾� - �������� ����
		if( m_DamageData.reActResult == CX2DamageManager::RT_REVENGE ||
			m_DamageData.reActResult == CX2DamageManager::RT_REFLECT )
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
			m_PhysicParam.nowSpeed.y = 0.0f;

			bCheckRevenge = true;
		}

		if( m_DamageData.techPoint > 0 )
			g_pX2Game->AddTechPoint( m_DamageData.techPoint );

		SetNowForceDown( 0.f );




		if( m_DamageData.m_bDrain == true && pDefenderUnit != NULL )
		{
//			ModifyFinalDamageByGameUnitType( &m_DamageData );
			float fFinalDamage = pDefenderUnit->GetFinalDamage( &m_DamageData );
			UpNowHp( fFinalDamage );
		}
	}

	if ( 0.f < m_DamageData.fHitAddHPPer )		/// ���� ������, ü�� ȸ�� ����
		UpNowHpPerHitOthers( m_DamageData.fHitAddHPPer );
	if( 0.f < m_DamageData.fHitAddHPbyAttackPower ) /// ���� ������, ü�� ȸ�� ����(������ ������ ��տ� ���)
		UpNowHpPerHitbyAttackPower( m_DamageData.fHitAddHPbyAttackPower );


	if( m_DamageData.m_fHpPercentUp <= 0.f && m_DamageData.m_fMpPercentUp <= 0.f && bCheckRevenge == false &&
		m_DamageData.m_fIncreaseMP <= 0.f )
	{

		if( NULL != m_pComboManager )
		{
			SetCurrentDamage( pDefenderUnit->GetFinalDamage( &m_DamageData ) );
			m_pComboManager->AddCombo();
		}

		DoDelegateProcessInAttackResult( m_DamageData );

		CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
		if( pPet != NULL )
		{
			pPet->AttackResult( m_DamageData.fHitAddMP );
		}
	}

	if( IsFocusUnit() == true )
	{

		switch( m_DamageData.m_eCameraShakeType )
		{
		case CKTDGCamera::DECT_UP:
			{
				// fix!! not yet!!

			} break;
		case CKTDGCamera::DECT_DOWN:
			{
				// fix!! not yet!!

			} break;
		case CKTDGCamera::DECT_UP_DOWN:
			{
				UpDownCrashCamera( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );

			} break;
		case CKTDGCamera::DECT_UP_DOWN_NO_RESET:
			{
				UpDownCrashCameraNoReset( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );
			} break;


		case CKTDGCamera::DECT_LEFT:
			{
				// fix!! not yet!!

			} break;

		case CKTDGCamera::DECT_RIGHT:
			{
				// fix!! not yet!!

			} break;

		case CKTDGCamera::DECT_LEFT_RIGHT:
			{
				ShakeLeftRight( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime, 0.05f );
			} break;
		case CKTDGCamera::DECT_LEFT_RIGHT_NO_RESET:
			{
				LeftRightCrashCameraNoReset( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );
			} break;


		case CKTDGCamera::DECT_RANDOM:
			{
				// fix!! not yet!!
			} break;

		default:
			break;
		}



		if ( false == g_pMain->GetGameOption().GetFlashEffect() )
		{
			g_pKTDXApp->GetDGManager()->ClearScreen( m_DamageData.clearScreen );
			g_pKTDXApp->GetDGManager()->SetClearScreenColor( m_DamageData.clearScreenColor );
		}
	}




	switch( m_DamageData.reActType )
	{
	case CX2DamageManager::RT_CREATE_DAMAGE_EFFECT:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetDamageEffect() )
			{
				// fix!! damage effect�̸��� powerrate�� �Է� ���� �� �ְ�
				g_pX2Game->GetDamageEffect()->CreateInstance( this, m_DamageData.m_wstrCreateDamageEffectName.c_str(),
					GetPowerRate(), m_DamageData.impactPoint, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			}
		} break;

	case CX2DamageManager::RT_CREATE_EFFECT_SET:
		{

			if( NULL != g_pX2Game )
			{
				CX2GameUnit* pTraceUnit = NULL;
				if( true == m_DamageData.m_bCreateEffectSetTraceUnit )
				{
					pTraceUnit = m_DamageData.optrDefenderGameUnit.GetObservable();
				}

				CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), this, pTraceUnit, false, GetPowerRate() );
				if ( CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSet ) )
				{
					pInstance->m_bTraceUnitDieDeleteEffectSet = m_DamageData.m_bTraceUnitDieDeleteEffectSet;
					pInstance->m_bCreateEffectSetTraceUnit = m_DamageData.m_bCreateEffectSetTraceUnit;
				}
			}

		} break;

	default:
		{
			if( g_pX2Game != NULL && g_pX2Game->GetDamageEffect() != NULL &&
				( GetHitCreateDamageEffectRate() > 0.f && GetRandomFloat() <= GetHitCreateDamageEffectRate() ) ||
				( m_DamageData.m_fCreateDamageEffectRate > 0.f && GetRandomFloat() <= m_DamageData.m_fCreateDamageEffectRate ) )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, m_DamageData.m_wstrCreateDamageEffectName.c_str(),
					GetHitCreateDamageEffectPowerRate(),
					m_DamageData.impactPoint, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			}
		}
		break;
	}

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetMajorParticle() )
	{
		switch( m_DamageData.reActResult )
		{
		case CX2DamageManager::RT_DUMMY_DAMAGE:
			break;

		default:
			{
				for( UINT i=0; i<m_DamageData.m_vecHitParticleName.size(); i++ )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq =
						g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  m_DamageData.m_vecHitParticleName[i].c_str(), m_DamageData.impactPoint );
					if( NULL != pSeq )
					{
						pSeq->SetLandPosition( GetLandPosition().y );
					}
				}
			} break;
		}
	}

	if ( CX2DamageManager::RT_DUMMY_DAMAGE != m_DamageData.reActResult && IsMyUnit() )
		SetHitCount( GetHitCount() + 1 );

	AttackResultPlayEffect();

#ifdef PVP_ZOOM_CAMERA
	SetSubZoomCameraTimer();
#endif

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

	if( NULL != g_pX2Game && true == g_pX2Game->IsHost() )
	{
		if( m_fSummonNpcCoolTime <= 0.f )
		{
			for(UINT i=0; i<m_vecSummonNpcSocket.size(); ++i)
			{
				CX2GUUser::SummonNpcSocket tSummonNpc = m_vecSummonNpcSocket[i];
				if( GetRandomFloat(i) <= tSummonNpc.m_fSummonRate )
				{
					m_fSummonNpcCoolTime = tSummonNpc.m_fSummonNpcCoolTime;
					g_pX2Game->CreateNPCReq( tSummonNpc.m_iNpcId, GetUnitLevel(), true, GetPos(),
						GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
					break;
				}
			}
		}
	}

	/// �Ϲ� ���� Ÿ�ݽ� ����Ʈ �� �����ϴ� �Լ� �˻�
	PlayEffectSetNormalAttackByBuff( m_DamageData.attackType );

}

/** @function : AttackResultPlayEffect
	@brief : ������ �������� reActResult Ÿ�Կ� ���� �ٸ� ����Ʈ�� �����ִ� �Լ�\n
	�� ĳ���� Ŭ������ AttackResult�� �������� ��� �̰����� �ű�
	@param : void
	@return : void
*/
/*virtual*/ void CX2GUUser::AttackResultPlayEffect()
{
	// ������ �� ĳ������ AttackResult�� �־��� ���� VIRTUALIZER�� ���־�����
	// �������� ���õ� �κ��� �ƴϱ� ������ �� �ʿ� ����

	// ASSERT ó���� ������, Release���� üũ�� ����( ���� �Լ��� NULL ó������ pDefenderUnit�� NULL�̸� AttackResult()�� ȣ��� �� ���� )
	CX2GameUnit *pDefender = m_DamageData.optrDefenderGameUnit.GetObservable();
	ASSERT( NULL != pDefender );
	if ( NULL == pDefender )
		return;
	switch ( m_DamageData.reActResult )
	{
	case CX2DamageManager::RT_REVENGE:
		{
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH ||	IsMyUnit() == true || pDefender->IsMyUnit() == true )
            {
			    PlayRevengeReactTypeParticle( m_DamageData.impactPoint );
            }

			g_pX2Game->GetWorld()->SetWorldColor( 0xff000000 );
			g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

			AttackResultStateChange( USI_DAMAGE_REVENGE );
		} break;

	case CX2DamageManager::RT_DUMMY_DAMAGE:
	case CX2DamageManager::RT_GRAPPLE:
	case CX2DamageManager::RT_THROW:
	case CX2DamageManager::RT_FLIP_DIFF_DIR :
	case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
		break;

	case CX2DamageManager::RT_SMALL_DAMAGE:
//{{ kimhc // 2011.1.8 // û 1�� ����
	case CX2DamageManager::RT_FLIP_SAME_DIR:
//}} kimhc // 2011.1.8 // û 1�� ����
		{
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH ||	IsMyUnit() == true || pDefender->IsMyUnit() == true )
			{
				PlaySmallDamageReactTypeParticle( m_DamageData.impactPoint );
				PlayEffectSetImpactPointByBuff();
			}
		} break;


	default:
		{
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH ||	IsMyUnit() == true || pDefender->IsMyUnit() == true )
			{
				PlayEtcReactTypeParticle( m_DamageData.impactPoint );
				PlayEffectSetImpactPointByBuff();
			}
		} break;
	}
}

void CX2GUUser::AttackResultByType( CX2DamageManager::DamageData &pDamageData )
{
	return;
}

/** @function : PlayRevengeReactTypeParticle
@brief : �ݰݿ� ���� ��� �����ִ� ��ƼŬ
@param : vImpactPoint_( ��ƼŬ ��� ��ġ�� ���۷���)
@return : void
*/
void CX2GUUser::PlayRevengeReactTypeParticle( const D3DXVECTOR3& vImpactPoint_ )
{
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeImpactSlashCW	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_SLASH_CW ) );
	PlaySequenceByTriggerCount( pSeqRevengeImpactSlashCW,	m_DamageData.impactPoint, 50,50, 3 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeImpactSlashCCW	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_SLASH_CCW ) );
	PlaySequenceByTriggerCount( pSeqRevengeImpactSlashCCW,	m_DamageData.impactPoint, 50,50, 3 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeImpact			= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT ) );
	PlaySequenceByTriggerCount( pSeqRevengeImpact,			m_DamageData.impactPoint, 50,50, 1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeImpactTick		= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_REVENGE_IMPACT_TICK ) );
	PlaySequenceByTriggerCount( pSeqRevengeImpactTick,		m_DamageData.impactPoint, 200, 200, 20 );

	if( pSeqRevengeImpactTick != NULL )
		pSeqRevengeImpactTick->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );

}

/** @function : PlaySmallDamageReactTypeParticle
@brief : �������ݿ� ���� ��� �����ִ� ��ƼŬ
@param : vImpactPoint_( ��ƼŬ ��� ��ġ�� ���۷���)
@return : void
*/
void CX2GUUser::PlaySmallDamageReactTypeParticle( const D3DXVECTOR3& vImpactPoint_ )
{
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpact		= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT ) );
	PlaySequenceByTriggerCount( pSeqDamageImpact,		m_DamageData.impactPoint, 100,200, 10 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactCore	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE )  );
	PlaySequenceByTriggerCount( pSeqDamageImpactCore,	m_DamageData.impactPoint, 100,200, 10 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactSlash	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH )  );
	PlaySequenceByTriggerCount( pSeqDamageImpactSlash,	m_DamageData.impactPoint, 50,50, 2 );
}

/** @function : PlayEtcReactTypeParticle
@brief : �ݰ�, ����, �׿ܿ� ��ƼŬ�� �������� �ʴ� ��츦 ������ ��쿡 �����ִ� ��ƼŬ
@param : vImpactPoint_( ��ƼŬ ��� ��ġ�� ���۷���)
@return : void
*/
void CX2GUUser::PlayEtcReactTypeParticle( const D3DXVECTOR3& vImpactPoint_ )
{
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactRed		= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RED ) );
	PlaySequenceByTriggerCount( pSeqDamageImpactRed, 			m_DamageData.impactPoint, 100,200, 10 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactRingRed	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RING_RED ) );
	PlaySequenceByTriggerCount( pSeqDamageImpactRingRed, 		m_DamageData.impactPoint, 100,200, 1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactCoreRed	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE_RED ) );
	PlaySequenceByTriggerCount( pSeqDamageImpactCoreRed, 		m_DamageData.impactPoint, 100,100, 1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqDamageImpactSlashRed	= pMajorParticleSystem->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH_RED ) );
	PlaySequenceByTriggerCount( pSeqDamageImpactSlashRed,		m_DamageData.impactPoint, 50,50, 1 );
}



void CX2GUUser::AddForceDownDelay( float fDelay )
{
	if( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
	{
		if( m_bAddForceDownGageThisState == false )
		{
			m_fMaxDownForce = fDelay;
			m_bAddForceDownGageThisState = true;
		}
		else
		{
			m_fMaxDownForce = max(m_fMaxDownForce, fDelay);
		}
	}
}

void CX2GUUser::AttackResultStateChange( int damageRevenge )
{
	if( IsMyUnit() == false )
		return;

	StateChange( damageRevenge, true );
}

void CX2GUUser::Win()
{
	if( IsMyUnit() == true )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameWin", 0,0,0, 1000, 1000, 1, 1 );
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Win.ogg", false, false );
	}
}

void CX2GUUser::Lose()
{
#ifdef GATE_DUNGEON
	if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
		if( NULL != pDungeonGame &&
			NULL != pDungeonGame->GetDungeon &&
			NULL != pDungeonGame->GetDungeon->GetDungeonData &&
			pDungeonGame->GetDungeon()->GetDungeonData()->m_bShowClearText == false )
			return;
	}
#endif

	if( IsMyUnit() == true )
	{
		switch( g_pX2Room->GetRoomType() )
		{
		case CX2Room::RT_PVP:
			{
				g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameLose", 0,0,0, 1000, 1000, 1, 1 );
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Lose.ogg", false, false );

			} break;

		case CX2Room::RT_DUNGEON:
		case CX2Room::RT_TRAININGCENTER:
			{
				g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameFail", 0,0,0, 1000, 1000, 1, 1 );
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Lose.ogg", false, false );

			} break;
		}
	}
}


void CX2GUUser::Draw()
{
	if( IsMyUnit() == true )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameDraw", 0,0,0, 1000, 1000, 1, 1 );
		//g_pKTDXApp->GetDeviceManager()->PlaySound( L"Win.ogg" );
	}
}

D3DXVECTOR3 CX2GUUser::GetHeadBonePos() const
{
	D3DXVECTOR3 tempPos;
	GetFramePos( &tempPos, m_pFrame_Bip01_Head );
	tempPos.y += 70.0f;

	return tempPos;
}

//{{ kimhc // 2011.1.17 // ������ Bone�� ��Ʈ���� ���� Trace �� (chung �ڵ� ����)
/** @function : GetCombineMatrixFromBoneName
	@brief : �� �̸��� �ش��ϴ� CombineMatrix���� ����
	@param : const wstring& wstrBoneName
	@return : const D3DXMATRIX*
*/
/*virtual*/ const D3DXMATRIX*	CX2GUUser::GetCombineMatrixFromBoneName( const wstring& wstrBoneName ) const
{
	if ( true == wstrBoneName.empty() )
	{
		return NULL;
	} // if


	CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( wstrBoneName.c_str() );
	if ( NULL == pMultiAnimFrame )
	{
		return NULL;
	} // if

	return &(pMultiAnimFrame->combineMatrix);
}
//}} kimhc // 2011.1.17 // ������ Bone�� ��Ʈ���� ���� Trace �� (chung �ڵ� ����)

//{{ robobeg : 2008-10-21
//void CX2GUUser::SetShow( bool bShow )
/*virtual*/
void CX2GUUser::NotifyShowObjectChanged()
//}} robobeg : 2008-10-21
{
	//{{ robobeg : 2008-10-21
	//CX2GameUnit::SetShow( bShow );

	bool bShow = GetShowObject();

	CX2GameUnit::NotifyShowObjectChanged();

	//}} robobeg : 2008-10-21

	if( m_pUnitShadow != NULL )
		m_pUnitShadow->SetShowObject( bShow );


	//{{ kimhc // 2010.6.16 // �տ� �ٴ� ��������Ʈ �Ӹ��� �ƴ϶� ���⿡ �ٴ� ����Ʈ ���� �̰����� SetShow ����
	SetShowEnchantWeaponParticle( bShow );
	//}} kimhc // 2010.6.16 // �տ� �ٴ� ��������Ʈ �Ӹ��� �ƴ϶� ���⿡ �ٴ� ����Ʈ ���� �̰����� SetShow ����



	CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarker    = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
//#ifdef TITLE_SYSTEM
	CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#else
//	CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#endif TITLE_SYSTEM


	CKTDGParticleSystem::CParticleEventSequence* pSeqHoldCenter   = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCenter );
	CKTDGParticleSystem::CParticleEventSequence* pSeqHoldCircle   = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCircle );


	if( pSeqHeadMarker != NULL )
		pSeqHeadMarker->SetShowObject( bShow );

	if( pSeqEmblem != NULL )
		pSeqEmblem->SetShowObject( bShow );

	if( pSeqHoldCenter != NULL )
		pSeqHoldCenter->SetShowObject( bShow );
	if( pSeqHoldCircle != NULL )
		pSeqHoldCircle->SetShowObject( bShow );

	if( false == bShow )
		DisableAfterImage( true );

	BOOST_TEST_FOREACH( CX2EqipPtr, pEqip, m_ViewEqipList )
	{
		if ( pEqip != NULL )
			pEqip->NotifyShowObjectChanged();

	}//BOOST_TEST_FOREACH()


	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		g_pData->GetPetManager()->NotifyShowObjectChanged( GetUnitUID(), bShow );
	}

}
//CX2GUUser::SetShowObject()


// CX2GUUser �� KTDGManager �� ���, ������ �� ���� ��ǰ�鵵 ���� ���, �����ϱ� ���� ����
/*virtual*/
void CX2GUUser::NotifyDGManagerChanged( CKTDGManager& refInManager_ )
{
	if ( IsRecursivelyManagedByDGManager() )
	{
		BOOST_TEST_FOREACH( CX2EqipPtr, pEqip, m_ViewEqipList )
		{
			if ( pEqip != NULL )
				refInManager_.AddObjectChain( pEqip.get() );
		}//BOOST_TEST_FOREACH()
	}
	else
	{
		BOOST_TEST_FOREACH( CX2EqipPtr, pEqip, m_ViewEqipList )
		{
			if ( pEqip != NULL )
				refInManager_.RemoveObjectChain( pEqip.get() );
		}//BOOST_TEST_FOREACH()
	}//if.. else..
}//CX2GUUser::NotifyDGManagerChanged()
//}} robobeg : 2008-10-17




bool CX2GUUser::IsMyUnit() const
{
	if( NULL != g_pX2Game &&
		g_pX2Game->GetMyUnit() == this )
		return true;
	else
		return false;
}

bool CX2GUUser::IsLocalUnit() const
{
	return IsMyUnit();
}

bool CX2GUUser::IsFocusUnit() const
{
	if( g_pX2Game->GetFocusUnit() == this )
		return true;
	else
		return false;
}

const CX2GUUser::SyncData& CX2GUUser::GetSyncData( bool bFuture ) const
{
	KTDXPROFILE();
	if( bFuture == true )
	{
		if( IsMyUnit() == true )
		{
			return m_FrameDataFuture.syncData;
		}
		else
		{
			if( m_ReceiveSyncDataList.empty() == true )
			{
				return m_FrameDataNow.syncData;
			}
			else
			{
				return m_ReceiveSyncDataList.back();
			}
		}
	}
	else
	{
		return m_FrameDataNow.syncData;
	}
}

const CX2GUUser::FrameData& CX2GUUser::GetFrameData( bool bFuture ) const
{
	KTDXPROFILE();
	if( bFuture == true )
	{
		if( IsMyUnit() == true )
		{
			return m_FrameDataFuture;
		}
		else
		{
			return m_FrameDataNow;
		}
	}
	else
	{
		return m_FrameDataNow;
	}
}

void CX2GUUser::ClearRecieveSyncData()
{
	m_SendSyncDataList.resize(0);
	m_ReceiveSyncDataList.resize(0);
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_LastSendSyncDataList.resize(0);
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

void CX2GUUser::DyingStart()
{
	if( m_bDyingStart  == true )
		return;

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
	{
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DyingState );
		m_ptrRidingPet->GetXSkinAnimPtr()->SetPlaySpeed( 0.5f );
		m_bPassDash = false;
	}

	CX2GameUnit::DyingStart();

	g_pX2Game->SetEnableAllKeyProcess( true );


	if( CX2Main::XS_TRAINING_GAME != g_pMain->GetNowStateID() )
	{
		if( IsMyUnit() == true )
		{
			//������ ���� ��� �����͸� ������

			KEGS_USER_UNIT_DIE_REQ kPacket;

			//Init
			kPacket.m_KillerNPCUID = -1;
			kPacket.m_KillerUserUnitUID = -1;

			kPacket.m_ucDieReason = m_ucDieReason;

			if( GetAttackedByUserUnit() == true )
			{
                CX2GameUnit* pAttackerGameUnit = GetGameUnitWhoAttackedMe();
				if ( pAttackerGameUnit != NULL
                    && pAttackerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER
                    )
				{

					kPacket.m_KillerUserUnitUID = pAttackerGameUnit->GetUnitUID();
					pAttackerGameUnit->DoProcessWhenKillEnemy( this );
				}
                else
                {
				    kPacket.m_KillerUserUnitUID = -1;
                }
			}
			else
			{
				// PVP ���ӿ��� ������ ��ȯ ���Ϳ� ���� ������ ���� ���, User�� ���� ������ ó������� �Ѵ�.
				if( NULL != g_pX2Game )
				{
                    CX2GameUnit* pAttacker = GetGameUnitWhoAttackedMe();
                    CX2GUNPC* pNPC = ( pAttacker != NULL && pAttacker->GetGameUnitType() == CX2GameUnit::GUT_NPC )
                        ? static_cast<CX2GUNPC*>( pAttacker ) : NULL;

					if( NULL != pNPC && g_pX2Room->IsNpcSlot( pNPC->GetUnitUID() ) == true )
					{
						kPacket.m_KillerNPCUID = -1;
						kPacket.m_KillerUserUnitUID = pNPC->GetUnitUID();
					}
					else
					{
						if( NULL != pNPC &&
							NULL != pNPC->GetNPCAI() )
						{
							switch( pNPC->GetNPCAI()->GetAIType() )
							{
							case CX2NPCAI::NAT_ALLY:
								{
									CX2AllyNPCAI* pNPCAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
									kPacket.m_KillerUserUnitUID = pNPCAI->GetAllyUnitUID();
								} break;
							}
							kPacket.m_KillerNPCUID = static_cast<int>( pNPC->GetUnitUID() );
						}
						else
							kPacket.m_KillerNPCUID = -1;

					}
				}
			}


			kPacket.m_MaxDamageKillerUserUnitUID = GetGameUnitUIDMaxDamage();

			g_pData->GetServerProtocol()->SendPacket( EGS_USER_UNIT_DIE_REQ, kPacket );
			g_pMain->AddServerPacket( EGS_USER_UNIT_DIE_ACK );

			if( IsMyUnit() == true )
			{
				if ( m_hSeqWhiteOut != INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWhiteOut );
					g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  L"WhiteOutExtraDamageEnd", D3DXVECTOR3(0,0,0) );
				}
			}
			m_bWhiteOut = false;
		}

		//{{ kimhc // 2009-12-28 // ��彺ų 3����
		// ������ ü��, ������ ����Ʈ ���� �� ��ų ����Ÿ �ʱ�ȭ
		if ( m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect != INVALID_PARTICLE_SEQUENCE_HANDLE )
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect );
			m_GuildSkillData.m_InfiniteStrength.Init();
		//}} kimhc // 2009-12-28 // ��彺ų 3����

		//{{ kimhc // 2010.6.15 // �����̹� ��� Ȥ�� ������ġ �÷��� �� ų�� �� ������ ��ǻ�Ϳ����� ��Ȱ�� ���� �Ӽ���æƮ�� ������ �ʴ� ����
		SetShowEnchantWeaponParticle( false );
		//}} kimhc // 2010.6.15 // �����̹� ��� Ȥ�� ������ġ �÷��� �� ų�� �� ������ ��ǻ�Ϳ����� ��Ȱ�� ���� �Ӽ���æƮ�� ������ �ʴ� ����

		++m_iDieCount;

		//{{ kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
		//}} kimhc // 2011-08-08 // ���� ���� ä�θ� �̺�Ʈ
	}
	ClearStartOfDelayedFiringData();		/// ����� ������ ��ȣź ���� ���� ����
}

void CX2GUUser::CommonStateStartFuture()
{
	KTDXPROFILE();

	int index = 0;
	// fix!! �װ� �������� �Ѿ�� ��쿡 wait���¿��� ���� HP�� 0�� �� �� �ִ�, �ٸ� ������� ��ŷ�� ����
	//if( m_FrameDataFuture.syncData.nowState == m_CommonState.m_Wait && GetGageManager()->GetNowHp() <= 0.0f )
	//	g_pKTDXApp->SetFindHacking( true );

    ClearAnimEventTimerOneshotFuture();

	m_FrameDataFuture.unitCondition.fStateTimeBack	= 0.0f;
	m_FrameDataFuture.unitCondition.fStateTime		= 0.0f;

	m_FrameDataFuture.stateParam.bLandConnect		= true;
	m_FrameDataFuture.stateParam.bCanPushUnit		= true;
	m_FrameDataFuture.stateParam.bCanPassUnit		= false;

	if( -9999.f != m_fMaxDownForce )
		m_bAddForceDownGageThisState				= false;

	m_FrameDataFuture.unitCondition.bHit			= false;


	ZeroMemory( m_FrameDataFuture.stateParam.bEventFlagList, sizeof(bool) * EVENT_FLAG_LIST_NUM );


	wstring animName;
	LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

	SetCustomAnimName( m_FutureStateData, animName );		/// Ư�� ������ ��, �ִϸ��̼��� �ٲٴ� �Լ�

	wstring ridingAnimName;
	LUA_GET_VALUE( m_LuaManager, "RIDING_ANIM_NAME", ridingAnimName, L"" );

	if ( true == GetRidingOn() && false == ridingAnimName.empty() )
		animName = ridingAnimName;

	CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType;
	LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", playType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
	bool bTransition = true;
	LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
	float fStartTime = 0.0f;
	LUA_GET_VALUE( m_LuaManager, "ANIM_START_TIME", fStartTime, 0.0f );

	bool bAnimationChanged = false;

	if( m_FrameDataFuture.unitCondition.bTransitionChange == false || bTransition == false || m_FrameDataFuture.syncData.bDirectChange == true )
		bAnimationChanged = m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), false, fStartTime );
	else
		bAnimationChanged = m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), true, fStartTime );

	if ( true == bAnimationChanged )
		m_pXSkinAnimFuture->Play( playType );
// 	else
// 	{
// 		ASSERT(!"NOT GOOD");
// 	}


	LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", m_fAnimSpeedFuture, 1.f );

	// �������� �ִϸ��̼� �ӵ� ���� �ɼǿ� ������ ���� �ʴ� ��������
	bool bApplyAnimSpeedByItemAbility = true;
	LUA_GET_VALUE( m_LuaManager, "APPLY_ANIM_SPEED_BY_ITEM_ABILITY",	bApplyAnimSpeedByItemAbility,	true );


	if ( true == bApplyAnimSpeedByItemAbility )
	{
		m_fAnimSpeedFuture += GetAddAnimSpeedFactor();
	} // if

	ResetAnimSpeed( m_fAnimSpeedFuture, bApplyAnimSpeedByItemAbility, true );


	LUA_GET_VALUE( m_LuaManager, "LAND_CONNECT",	m_FrameDataFuture.stateParam.bLandConnect,	true );
	bool bCanPushUnit = true;
	LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT",	bCanPushUnit,	true );
	m_FrameDataFuture.stateParam.bCanPushUnit = bCanPushUnit;
	bool bCanPassUnit = false;
	LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
	m_FrameDataFuture.stateParam.bCanPassUnit = bCanPassUnit;

    LUA_GET_VALUE( m_LuaManager, "APPLY_ANIM_MOVE", m_bFutureApplyAnimMove, true );

	index = 0;
	m_vecCanPushUnit.clear();
	while( m_LuaManager.BeginTable( "CAN_PUSH_UNIT_TIME", index ) == true )
	{
		TIME_PUSHPASS canPushUnitTime;

		LUA_GET_VALUE( m_LuaManager, 1, canPushUnitTime.fTime , 0.0f );
		LUA_GET_VALUE( m_LuaManager, 2, canPushUnitTime.bCan, true );

		m_vecCanPushUnit.push_back(canPushUnitTime);
		index++;
		m_LuaManager.EndTable();
	}

	index = 0;
	m_vecCanPassUnit.clear();
	while( m_LuaManager.BeginTable( "CAN_PASS_UNIT_TIME", index ) == true )
	{
		TIME_PUSHPASS canPassUnitTime;

		LUA_GET_VALUE( m_LuaManager, 1, canPassUnitTime.fTime, 0.0f );
		LUA_GET_VALUE( m_LuaManager, 2, canPassUnitTime.bCan, false );

		m_vecCanPassUnit.push_back(canPassUnitTime);
		index++;
		m_LuaManager.EndTable();
	}




	D3DXVECTOR3 speed;
	if( m_LuaManager.GetValue( "SPEED_X", speed.x ) == true )
		m_PhysicParam.nowSpeed.x = speed.x;
	if( m_LuaManager.GetValue( "SPEED_Y", speed.y ) == true )
		m_PhysicParam.nowSpeed.y = speed.y;

	index = 0;
	m_vecSpeedFactor.clear();
	while( m_LuaManager.BeginTable( "SPEED_TIME", index ) == true )
	{
		TIME_SPEED timeSpeed;

		LUA_GET_VALUE( m_LuaManager, 1, timeSpeed.vSpeed.x, 0.0f );		/// X�� �ӵ�
		LUA_GET_VALUE( m_LuaManager, 2, timeSpeed.vSpeed.y, 0.0f );		/// Y�� �ӵ�
		LUA_GET_VALUE( m_LuaManager, 3, timeSpeed.vSpeed.z, -1.0f );	/// ����� �ִϸ��̼� Ÿ��
		LUA_GET_VALUE( m_LuaManager, 4, timeSpeed.iFlag, 0 );			/// m_PhysicParam.nowSpeed, 1�̸� +=, 2�̸� = ������ ������, 3�̸� �нú� ���ǵ� �� ����

		m_vecSpeedFactor.push_back(timeSpeed);
		index++;
		m_LuaManager.EndTable();
	}


	LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_X", m_PhysicParam.passiveSpeed.x, -1.0f );
	LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_Y", m_PhysicParam.passiveSpeed.y, -1.0f );



	LUA_GET_VALUE( m_LuaManager, "DISABLE_GRAVITY_TIME_START", m_vDisableGravityInScriptTime.x, 0.f );
	LUA_GET_VALUE( m_LuaManager, "DISABLE_GRAVITY_TIME_END", m_vDisableGravityInScriptTime.y, 0.f );
	ASSERT( m_vDisableGravityInScriptTime.x <= m_vDisableGravityInScriptTime.y );

	LUA_GET_VALUE( m_LuaManager, "IGNORE_LINE_TIME_START", m_vFutureIgnoreLineTime.x, 0.f );
	LUA_GET_VALUE( m_LuaManager, "IGNORE_LINE_TIME_END", m_vFutureIgnoreLineTime.y, 0.f );
	ASSERT( m_vFutureIgnoreLineTime.x <= m_vFutureIgnoreLineTime.y );



	bool bIsRight;
	if( m_LuaManager.GetValue( "RIGHT", bIsRight ) == true )
		m_FrameDataFuture.syncData.bIsRight = bIsRight;


	/// �ش� ������Ʈ������ ������ ����, ������Ʈ�� ������ ���� �������� ���ƺ�
	LUA_GET_VALUE( m_LuaManager, "RIGHT_AT_THIS_STATE", m_bRightAtThisState, false );

	/// ������ �ڸ� ���� �������� �ش� ����� ���� ��Ų��. StataEndFuture���� ������ ������� �����ش�.
	if ( m_FrameDataFuture.syncData.bIsRight == false && true == m_bRightAtThisState )
		m_FrameDataFuture.syncData.bIsRight = true;
	else
		m_bRightAtThisState = false;


	index = 0;
	m_StopAllList.clear();
	while( m_LuaManager.BeginTable( "STOP_ALL_UNIT", index ) == true )
	{
		D3DXVECTOR3 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 3, stopTime.z, -1.0f );
		if( stopTime.x != -1.0f )
		{
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
			{
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataFuture.syncData.position, 800.0f, this, stopTime.z );
					g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataFuture.syncData.position, 700.0f, this, stopTime.z );
					break;

				case CX2Game::GT_PVP:
					g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataFuture.syncData.position, 2000.0f, this, stopTime.z );
					g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataFuture.syncData.position, 1300.0f, this, stopTime.z );
					break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}
			}
			else
				m_StopAllList.push_back( stopTime );
		}

		index++;
		m_LuaManager.EndTable();
	}

	index = 0;
	m_Stop2AllList.clear();
	while( m_LuaManager.BeginTable( "STOP2_ALL_UNIT", index ) == true )
	{
		D3DXVECTOR3 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 3, stopTime.z, -1.0f );
		if( stopTime.x != -1.0f )
		{
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
			{
				if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
				{
					g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataFuture.syncData.position,800.0f, this, stopTime.z );
					g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataFuture.syncData.position, 700.0f, this, stopTime.z );
				}
				else
				{
					g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataFuture.syncData.position, 2000.0f, this, stopTime.z );
					g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataFuture.syncData.position, 1300.0f, this, stopTime.z );
				}
				//if( stopTime.z > 0.0f )
				//	SetStopTime( stopTime.z );
				//else
				//	SetStopTime( stopTime.y );
			}
			else
				m_Stop2AllList.push_back( stopTime );
		}
		index++;
		m_LuaManager.EndTable();
	}
	index = 0;
	m_StopMyList.clear();
	while( m_LuaManager.BeginTable( "STOP_MY_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		if( stopTime.x != -1.0f )
		{
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
				SetStopTime( stopTime.y );
			else
				m_StopMyList.push_back( stopTime );
		}

		index++;
		m_LuaManager.EndTable();
	}

	index = 0;
	m_Stop2MyList.clear();
	while( m_LuaManager.BeginTable( "STOP2_MY_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		if( stopTime.x != -1.0f )
		{
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
				SetStopTime( stopTime.y );
			else
				m_Stop2MyList.push_back( stopTime );
		}

		index++;
		m_LuaManager.EndTable();
	}

	index = 0;
	while( m_LuaManager.BeginTable( "EVENT_PROCESS_TIME", index ) == true )
	{
		if( index >= 10 )
		{
			m_LuaManager.EndTable();
			break;
		}
		float fDummy = 0.f;
		LUA_GET_VALUE( m_LuaManager, 1, m_fEventTime[index].keyInputStart, 0.f );
		LUA_GET_VALUE( m_LuaManager, 2, m_fEventTime[index].keyInputEnd, 9999.f );
		LUA_GET_VALUE( m_LuaManager, 3, m_fEventTime[index].stateChange, 9999.f );
		index++;
		m_LuaManager.EndTable();
	}
	for( int i = index; i < 10; ++i )
	{
		m_fEventTime[i].keyInputStart = 0.f;
		m_fEventTime[i].keyInputEnd = 9999.f;
		m_fEventTime[i].stateChange = 9999.f;
	}

	LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER",					m_fSkillCancelAfter,			9999.f );
	LUA_GET_VALUE(	m_LuaManager, "BWALK_CANCEL_AFTER",					m_fBWalkCancelAfter,			9999.f );
	LUA_GET_VALUE(	m_LuaManager, "WALK_CANCEL_AFTER",					m_fWalkCancelAfter,				9999.f );
	LUA_GET_VALUE(	m_LuaManager, "DASH_CANCEL_AFTER",					m_fDashCancelAfter,				9999.f );
	LUA_GET_VALUE(	m_LuaManager, "ATTACK_CANCEL_AFTER",				m_fAttackCancelAfter,			m_fWalkCancelAfter );
	LUA_GET_VALUE(	m_LuaManager, "ATTACK_CANCEL_AFTER_EX",				m_fAttackCancelAfterEx,			9999.f );


	LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER_FLY",				m_fSkillCancelAfterFly,			9999.f );		/// ���߿� ��ų ĵ��


}

void CX2GUUser::CommonStateStart()
{
	KTDXPROFILE();

	if( NULL != m_pUnitSlashTraceManager )
	{
		m_pUnitSlashTraceManager->OnStateStart();
	}


	m_bRotateBone = false;



//#ifdef LINEMAP_FAST_WIND_TEST
//	m_bEnableUpsideWind = true;
//	m_bWaitInTheAir = false;
//	m_fTimeInTheAir	= 0.f;
//#endif LINEMAP_FAST_WIND_TEST

    ClearAnimEventTimerOneshotNow();

	m_HitUnitList.clear();

	LoadImmunityAtThisStateFromScript( m_LuaManager );

	m_FrameDataNow.unitCondition.fStateTimeBack	= 0.0f;
	m_FrameDataNow.unitCondition.fStateTime		= 0.0f;

	m_FrameDataNow.stateParam.bCanPushUnit		= true;
	m_FrameDataNow.stateParam.bCanPassUnit		= false;

	bool bCanPushUnit = true;
	LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT",	bCanPushUnit,	true );
	m_FrameDataNow.stateParam.bCanPushUnit = bCanPushUnit;
	bool bCanPassUnit = false;
	LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
	m_FrameDataNow.stateParam.bCanPassUnit = bCanPassUnit;

//#ifdef  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX
    {
	    int index = 0;
	    m_vecCanPushUnitNow.clear();
	    while( m_LuaManager.BeginTable( "CAN_PUSH_UNIT_TIME", index ) == true )
	    {
		    TIME_PUSHPASS canPushUnitTime;

		    LUA_GET_VALUE( m_LuaManager, 1, canPushUnitTime.fTime , 0.0f );
		    LUA_GET_VALUE( m_LuaManager, 2, canPushUnitTime.bCan, true );

		    m_vecCanPushUnitNow.push_back(canPushUnitTime);
		    index++;
		    m_LuaManager.EndTable();
	    }

	    index = 0;
	    m_vecCanPassUnitNow.clear();
	    while( m_LuaManager.BeginTable( "CAN_PASS_UNIT_TIME", index ) == true )
	    {
		    TIME_PUSHPASS canPassUnitTime;

		    LUA_GET_VALUE( m_LuaManager, 1, canPassUnitTime.fTime, 0.0f );
		    LUA_GET_VALUE( m_LuaManager, 2, canPassUnitTime.bCan, false );

		    m_vecCanPassUnitNow.push_back(canPassUnitTime);
		    index++;
		    m_LuaManager.EndTable();
	    }
    }
//#endif  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX


	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( GetNowStateSkillID() );
	if( pSkillTemplet != NULL )
	{
		g_pX2Game->SetStateString( pSkillTemplet->m_wstrName );
#if X2_DEV_TOOLS
		if( true == IsMyUnit() )
		{
			g_pChatBox->AddChatLog( pSkillTemplet->m_wstrName.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM);
		}
#endif // ADD_DAMAGE_LOG
	}

	LUA_GET_VALUE( m_LuaManager, "IS_ATTACK_STATE",			m_FrameDataNow.stateParam.bAttackState,			false );


	//{{ dmlee 2008.12.13 �̺� ��ȯ �ʻ�� ����ϸ� tech point �ֱ� ���ؼ�
	if( true == IsMyUnit() )
	{
		int iTechPoint = 0;
		LUA_GET_VALUE( m_LuaManager, "TECH_POINT",				iTechPoint,				0	 );
		if( iTechPoint > 0 )
		{
			g_pX2Game->AddTechPoint( iTechPoint );
		}
	}
	//}} dmlee 2008.12.13 �̺� ��ȯ �ʻ�� ����ϸ� tech point �ֱ� ���ؼ�


//#ifdef HEAD_INVERSE_KINEMATICS_TEST
//	LUA_GET_VALUE( m_LuaManager, "HEAD_IK",			m_bEnableHeadIK,			false );
//#endif HEAD_INVERSE_KINEMATICS_TEST
//


	bool bSuperArmor = false;
	LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR",			bSuperArmor,			false );
	m_FrameDataNow.stateParam.bSuperArmor			= bSuperArmor;

	bool bSuperArmorNotRed = true;
	LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR_NOT_RED", bSuperArmorNotRed,		true );
	m_FrameDataNow.stateParam.bSuperArmorNotRed		= bSuperArmorNotRed;

	m_bStateSuperArmor = bSuperArmor;

    std::vector<D3DXVECTOR2>& vecSuperArmorTime = m_vecNowSuperArmorTime;
    vecSuperArmorTime.clear();

	int indexTimeTable = 0;
	while( m_LuaManager.BeginTable( "SUPER_ARMOR_TIME", indexTimeTable ) == true )
	{
		D3DXVECTOR2 superArmorTime;
		LUA_GET_VALUE( m_LuaManager, 1, superArmorTime.x, 0.f );
		LUA_GET_VALUE( m_LuaManager, 2, superArmorTime.y, 0.f );
		vecSuperArmorTime.push_back(superArmorTime);

		indexTimeTable++;
		m_LuaManager.EndTable();
	}

	if( bSuperArmor == true )
	{
		if( (m_bSuperArmorByItem == true && m_fSuperArmorByItem > 0.f) ||  //�����ۿ� ���� ���۾Ƹ� �� ��
			(m_bSocketSuperArmor == true && m_fSocketSuperArmor > 0.f) || // ���Ͽ� ���� ���� �Ƹ� �� ��
			false == bSuperArmorNotRed ) //��ũ��Ʈ���� ������ ���۾Ƹ� �ܰ����� ������ ���� ��
		{
			m_FrameDataNow.stateParam.bSuperArmorNotRed = false; //�ܰ� ���� ���������� ����
		}
		else
		{
			m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
		}
	}
	else
	{
		if( (m_bSuperArmorByItem == true && m_fSuperArmorByItem > 0.f) || (m_bSocketSuperArmor == true && m_fSocketSuperArmor > 0.f) )
		{
			m_FrameDataNow.stateParam.bSuperArmor = true;
			m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
		}
		else
		{
			m_fSocketSuperArmor = 0.f;
			m_bSocketSuperArmor = false;
			m_fSuperArmorByItem = 0.f;
			m_bSuperArmorByItem = false;
		}
	}

	ZeroMemory( m_FrameDataNow.stateParam.bEventFlagList, sizeof(bool) * EVENT_FLAG_LIST_NUM );

	m_FrameDataNow.unitCondition.bHit				= false;






	wstring animName;
	LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

	SetCustomAnimName( m_NowStateData, animName );		/// Ư�� ������ ��, �ִϸ��̼��� �ٲٴ� �Լ�

	wstring ridingAnimName;
	LUA_GET_VALUE( m_LuaManager, "RIDING_ANIM_NAME", ridingAnimName, L"" );

	if ( true == GetRidingOn() && false == ridingAnimName.empty() )
		animName = ridingAnimName;

	CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType;
	LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", playType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );

	bool bTransition = true;
	LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
	float fStartTime = 0.0f;
	LUA_GET_VALUE( m_LuaManager, "ANIM_START_TIME", fStartTime, 0.0f );

	if( m_FrameDataNow.unitCondition.bTransitionChange == false || bTransition == false || m_FrameDataNow.syncData.bDirectChange == true )
		m_pXSkinAnim->ChangeAnim( animName.c_str(), false, fStartTime );
	else
		m_pXSkinAnim->ChangeAnim( animName.c_str(), true, fStartTime );
	m_pXSkinAnim->Play( playType );

	LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", m_fAnimSpeed, 1.f );


	// �������� �ִϸ��̼� �ӵ� ���� �ɼǿ� ������ ���� �ʴ� �������� üũ
	bool bApplyAnimSpeedByItemAbility = true;
	LUA_GET_VALUE( m_LuaManager, "APPLY_ANIM_SPEED_BY_ITEM_ABILITY",	bApplyAnimSpeedByItemAbility,	true );

	// ���� ����� ������ ���(�нú�)�� ���� �ִϸ��̼� �ӵ� ����
	if ( true == bApplyAnimSpeedByItemAbility )
	{
		m_fAnimSpeed += GetAddAnimSpeedFactor();
	} // if

	ResetAnimSpeed( m_fAnimSpeed, bApplyAnimSpeedByItemAbility, false );

//{{ kimhc // 2011.1.21 // û 1�� ����
	// ������Ʈ �� HittedType ����, CommonStateStartFuture������ �Ľ� ����
	LUA_GET_VALUE_ENUM( m_LuaManager, "HITTED_TYPE_AT_STATE", m_eHittedTypeAtState, CX2DamageManager::HITTED_TYPE, CX2DamageManager::HTD_NO_SOUND );
//}} kimhc // 2011.1.21 // û 1�� ����

	bool bCustomStateDeleteEffectSet = false;
	LUA_GET_VALUE( m_LuaManager, "CUSTOM_STATE_DELETE_EFFECT_SET",	bCustomStateDeleteEffectSet,	false );
	if(bCustomStateDeleteEffectSet)
	{
		DeleteEffectSetOnCustomState();
	}


	m_bApplyMemo = false;
	CX2SkillTree::SKILL_MEMO_ID eMemoID = CX2SkillTree::SMI_NONE;
	LUA_GET_VALUE_ENUM( m_LuaManager, "APPLY_MEMO", eMemoID,	CX2SkillTree::SKILL_MEMO_ID,	CX2SkillTree::SMI_NONE );
	if( CX2SkillTree::SMI_NONE != eMemoID )
	{
		m_bApplyMemo = GetEqippedSkillMemo( eMemoID );
	}
	// ���罺����Ʈ���� �߻��ϴ� effectset list�� ��´�.
	LoadEffectSetFromScript( m_LuaManager, m_bApplyMemo );

	m_vecEffectSetToDeleteOnDamageReact.resize(0);
	m_vecEffectSetToDeleteOnStateEnd.resize(0);





	if( m_LuaManager.BeginTable( "INVINCIBLE" ) == true )
	{
		float fInvin = -1.0f;
		LUA_GET_VALUE( m_LuaManager, 1,	fInvin,	-1.0f );
		m_FrameDataNow.stateParam.invincibleTime.m_fX = fInvin;
		fInvin = -1.0f;
		LUA_GET_VALUE( m_LuaManager, 2,	fInvin,	-1.0f );
		m_FrameDataNow.stateParam.invincibleTime.m_fY = fInvin;

		if( m_FrameDataNow.stateParam.invincibleTime.m_fX <= 0.0f && m_FrameDataNow.stateParam.invincibleTime.m_fY > 0.0f )
			m_FrameDataNow.stateParam.bInvincible = true;
		else
			m_FrameDataNow.stateParam.bInvincible = false;

		m_LuaManager.EndTable();
	}
	else
	{
		m_FrameDataNow.stateParam.invincibleTime.m_fX	= -1.0f;
		m_FrameDataNow.stateParam.invincibleTime.m_fY	= -1.0f;
		m_FrameDataNow.stateParam.bInvincible		= false;
	}

	if( m_fForceInvincibleTime > 0.0f )
		m_FrameDataNow.stateParam.bInvincible = true;


    m_mapNowAddSlashTrace.clear();
    LUA_GET_VALUE( m_LuaManager, "SLASH_TRACE_TIP_WIDE",	m_fNowSlashTraceTipWide,	4.0f );

	if( NULL != m_pUnitSlashTraceManager )
	{
		m_pUnitSlashTraceManager->SetSlashTraceTipWidth(
            m_fNowSlashTraceTipWide
            );
	}


	if( m_LuaManager.BeginTable( "SLASH_TRACE" ) == true )
	{
		D3DXVECTOR3 vSlashTraceTime;

		LUA_GET_VALUE( m_LuaManager, 1,	vSlashTraceTime.x,	-1.0f );
		LUA_GET_VALUE( m_LuaManager, 2,	vSlashTraceTime.y,	-1.0f );
		LUA_GET_VALUE( m_LuaManager, 3, vSlashTraceTime.z,  0.f );			// slash trace type
        m_mapNowAddSlashTrace[ 0 ] = vSlashTraceTime;

		m_LuaManager.EndTable();
	}


	int iTableIndex = 0;
	while( true == m_LuaManager.BeginTable( "SLASH_TRACE", iTableIndex ) )
	{
		int iWeaponIndex = 0;
		D3DXVECTOR3 vSlashTraceTime;

		LUA_GET_VALUE( m_LuaManager, 1,	vSlashTraceTime.x,	-1.0f );
		LUA_GET_VALUE( m_LuaManager, 2,	vSlashTraceTime.y,	-1.0f );
		LUA_GET_VALUE( m_LuaManager, 3, iWeaponIndex, 0 );
		LUA_GET_VALUE( m_LuaManager, 4, vSlashTraceTime.z, 0.f );			// slash trace type
        m_mapNowAddSlashTrace[ iWeaponIndex ] = vSlashTraceTime;

		m_LuaManager.EndTable(); // SLASH_TRACE%d
		iTableIndex++;
	}

	DoUnitSlashTraceInCommonStateStart();

    D3DXVECTOR2& v2AfterImage = m_v2NowAfterImageTime;
	if( m_LuaManager.BeginTable( "AFTER_IMAGE" ) == true )
	{

		LUA_GET_VALUE( m_LuaManager, 1,	v2AfterImage.x,	-1.0f );
		LUA_GET_VALUE( m_LuaManager, 2,	v2AfterImage.y,	-1.0f );

		m_LuaManager.EndTable();
	}
	else
	{
		v2AfterImage.x	= -1.0f;
		v2AfterImage.y	= -1.0f;
		DisableAfterImage();
	}

	if( IsFocusUnit() == true && m_FrameDataNow.stateParam.bResetCamera == true )
	{
		m_FrameDataNow.stateParam.hitCamera = 0;
		if( g_pX2Game->GetX2Camera()->GetCameraState() != CX2Camera::CS_NORMAL )
		{
			g_pX2Game->GetX2Camera()->NomalDirectCamera( this, g_pMain->GetGameOption().GetCameraDistance() );
		}

		int camera0Rate = 100;
		int camera1Rate = 100;
		int camera2Rate = 100;
		int ran = rand()%100 + 1;

		//09.02.18.hoons : �̺� ��Ű������ lose �����϶��� ���̷�Ʈ ī�޶� ���ֱ� ����
		//                 ���Ŀ� �ڵ����� �ʿ�.
		//if( GetUnit()->GetClass() == CX2Unit::UC_EVE_ARCHITECTURE && GetGameUnitState() == CX2GUEve::ENSI_LOSE )
		//{
		//	camera0Rate = 100;
		//	camera1Rate = 0;
		//	camera2Rate = 0;
		//}
		//else
		//{
		LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA0_RATE", camera0Rate, 0 );
		LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA1_RATE", camera1Rate, 0 );
		LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA2_RATE", camera2Rate, 0 );
		//}
		// hoons.
		if( ran < camera0Rate )
		{
			m_FrameDataNow.stateParam.normalCamera = 0;
		}
		else if( ran < (camera0Rate + camera1Rate) )
		{
			m_FrameDataNow.stateParam.normalCamera = 1;
		}
		else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
		{
			m_FrameDataNow.stateParam.normalCamera = 2;
		}
		else
			m_FrameDataNow.stateParam.normalCamera = 0;

		ran = rand()%100 + 1;
		LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA0_RATE", camera0Rate, 0 );
		LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA1_RATE", camera1Rate, 0 );
		LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA2_RATE", camera2Rate, 0 );
		if( ran < camera0Rate )
		{
			m_FrameDataNow.stateParam.hitCamera = 0;
		}
		else if( ran < (camera0Rate + camera1Rate) )
		{
			m_FrameDataNow.stateParam.hitCamera = 1;
		}
		else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
		{
			m_FrameDataNow.stateParam.hitCamera = 2;
		}
		else
			m_FrameDataNow.stateParam.hitCamera = m_FrameDataNow.stateParam.normalCamera;
	}

    LUA_GET_VALUE( m_LuaManager, "FALL_DOWN", m_bNowFallDown, true );


	m_DamageData.SimpleInit();
	m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
	m_DamageData.optrAttackerGameUnit		= this;

	if( true == m_bApplyMemo )
	{
		if( false == SetDamageData( "DAMAGE_DATA_MEMO", m_bApplyMemo ) )
		{
			SetDamageData( "DAMAGE_DATA" );
		}
	}
	else
	SetDamageData( "DAMAGE_DATA" );



	// ������ �׽�Ʈ��
#if X2_DEV_TOOLS
	if( NULL != g_pData->GetMyUser() &&
		g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
		true == this->IsMyUnit() &&
		true == g_pX2Game->IsHost() )
	{
		if( 0.f != m_fDevAddDamage )
		{
			m_DamageData.damage.fPhysic		+= m_fDevAddDamage;
			m_DamageData.damage.fMagic		+= m_fDevAddDamage;
		}
	}
#endif


	m_FrameDataNow.stateParam.bResetCamera			= true;

	if( m_LuaManager.BeginTable( "REVENGE" ) == true )
	{
		float fReven = -1.0f;
		LUA_GET_VALUE( m_LuaManager, 1, fReven,	-1.0f );
		m_FrameDataNow.stateParam.fRevengeStartTime = fReven;
		fReven = -1.0f;
		LUA_GET_VALUE( m_LuaManager, 2, fReven,		-1.0f );
		m_FrameDataNow.stateParam.fRevengeEndTime = fReven;

		m_LuaManager.EndTable();
	}
	else
	{
		m_FrameDataNow.stateParam.fRevengeStartTime	= -1.0f;
		m_FrameDataNow.stateParam.fRevengeEndTime	= -1.0f;
	}

	// �����ݻ�
	if( m_LuaManager.BeginTable( "REFLEX_MAGIC" ) == true )
	{
		float fReflex = -1.0f;
		LUA_GET_VALUE( m_LuaManager, 1, fReflex,	-1.0f );
		m_FrameDataNow.stateParam.fReflexMagicStartTime = fReflex;
		fReflex= -1.0f;
		LUA_GET_VALUE( m_LuaManager, 2, fReflex,		-1.0f );
		m_FrameDataNow.stateParam.fReflexMagicEndTime = fReflex;

		m_LuaManager.EndTable();
	}
	else
	{
		m_FrameDataNow.stateParam.fReflexMagicStartTime	= -1.0f;
		m_FrameDataNow.stateParam.fReflexMagicEndTime	= -1.0f;
	}

	int index = 0;
    m_vecAttackTime.resize( 0 );
    m_vecAttackTime.reserve( m_LuaManager.GetNumIndexedTables( "ATTACK_TIME" ) );
	while( m_LuaManager.BeginTable( "ATTACK_TIME", index ) == true )
	{
        m_vecAttackTime.resize( m_vecAttackTime.size() + 1 );
        KAttackTimeProjSeqName& kNode = m_vecAttackTime.back();

		LUA_GET_VALUE( m_LuaManager, 1, kNode.m_v2AttackTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, kNode.m_v2AttackTime.y, -1.0f );
        kNode.m_wstrProjSeqName.resize( 0 );
		m_LuaManager.GetValue( 3, kNode.m_wstrProjSeqName );
		index++;
		m_LuaManager.EndTable();
	}

	StopTime_StateStart();

	index = 0;
	m_vecSoundPlayData.resize(0);
	while( m_LuaManager.BeginTable( "SOUND_PLAY", index ) == true )
	{
		CX2GameUnit::SoundPlayData sndPlayData;

		LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,	-1.0f	);
		LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,	L""		);
		LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,	100		);
		LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,	false	);

		m_vecSoundPlayData.push_back( sndPlayData );

		index++;
		m_LuaManager.EndTable();
	}

	int iSoundRate0 = 100;
	int iSoundRate1 = 100;
	int iSoundRate2 = 100;
	int iRand = rand()%100 + 1;

	LUA_GET_VALUE( m_LuaManager, "SOUND_SET0_RATE", iSoundRate0, 0 );
	LUA_GET_VALUE( m_LuaManager, "SOUND_SET1_RATE", iSoundRate1, 0 );
	LUA_GET_VALUE( m_LuaManager, "SOUND_SET2_RATE", iSoundRate2, 0 );

    USHORT  usShoundSet = 0;
	if( iRand < iSoundRate0 )
	{
		usShoundSet = 0;
	}
	else if( iRand < ( iSoundRate0 + iSoundRate1 ) )
	{
		usShoundSet = 1;
	}
	else if( iRand < ( iSoundRate0 + iSoundRate1 + iSoundRate2 ) )
	{
		usShoundSet = 2;
	}
	else
		usShoundSet = 0;

	if ( m_LuaManager.BeginTable( "SOUND_SET", usShoundSet ) == true )
	{
		m_vecSoundPlayData.resize(0);

		bool bUseHyperSound = false;
		bool bUseBerserkSound = false;
		LUA_GET_VALUE( m_LuaManager, "USE_HYPER_SOUND", bUseHyperSound, false );
		LUA_GET_VALUE( m_LuaManager, "USE_BERSERK_SOUND", bUseBerserkSound, false );

		const char* pszState = "NORMAL";
		if ( 0.f < GetRemainHyperModeTime() && true == bUseHyperSound )
			pszState = "HYPER";

		if ( true == IsFullHyperMode() && 0.f < GetRemainHyperModeTime() && true == bUseBerserkSound )
			pszState = "BERSERK";

		if( m_LuaManager.BeginTable( pszState ) == true )
		{
			index = 0;
			while( m_LuaManager.BeginTable( "SOUND_PLAY", index ) == true )
			{
				CX2GameUnit::SoundPlayData sndPlayData;

				LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,	-1.0f	);
				LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,	L""		);
				LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,	100		);
				LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,	false	);

				m_vecSoundPlayData.push_back( sndPlayData );

				index++;
				m_LuaManager.EndTable();	// SOUND_PLAY#
			}
		    m_LuaManager.EndTable();	// �̸����� Table
		}
		m_LuaManager.EndTable();	// SOUND_SET
	}



	//// ���ü�� ���ùڽ� �缳���Ǵ� ���� ����ó��
	//SetEnableAttackBox( L"Rfoot", false );
	//SetEnableAttackBox( L"Lfoot", false );
	//SetEnableAttackBox( L"Lhand2", false );
	if( m_LuaManager.BeginTable( "ENABLE_ATTACK_BOX" ) == true )
	{
		int index = 1;
		wstring wname;
		while( m_LuaManager.GetValue( index, wname ) == true )
		{
			SetEnableAttackBox( wname.c_str(), true );

			index++;
		}

		m_LuaManager.EndTable();
	}

	if( m_LuaManager.BeginTable( "DISABLE_ATTACK_BOX" ) == true )
	{
		int index = 1;
		wstring wname;
		while( m_LuaManager.GetValue( index, wname ) == true )
		{
			SetEnableAttackBox( wname.c_str(), false );

			index++;
		}

		m_LuaManager.EndTable();
	}

	if( m_FrameDataFuture.syncData.nowState == GUSI_DIE )
	{
		if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
		{
			CX2StateTrainingGame* pState = (CX2StateTrainingGame*) g_pMain->GetNowState();
			pState->Handler_EGS_END_TC_GAME_REQ( false );
		}
		else
		{
			//if ( g_pX2Game != NULL && g_pX2Game->GetGameEnd() == false )
			g_pData->GetServerProtocol()->SendID( EGS_USER_UNIT_DIE_COMPLETE_REQ );

			// FieldFix: landPostion, dirDegree, lastTouchLineIndex �� �ʿ��ѵ� �̷��� �����ϴ� ���� ���� ��������...
			m_FrameDataDead = m_FrameDataNow;

			if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
			{
				CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
				if( NULL != pDungeonGame &&
					NULL != pDungeonGame->GetDungeon() &&
					NULL != pDungeonGame->GetDungeon()->GetNowStage() )
				{
					m_DeadStageIndex	= pDungeonGame->GetDungeon()->GetNowStageIndex();
					m_DeadSubStageIndex = pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStageIndex();
				}
			}
		}
	}

	if( m_bNowStateDamageReact == true && GetNoDetonation() == true )
	{
		SetNoDetonation(true);
	}
	else
	{
		SetNoDetonation(false);
	}
	m_bNowStateDamageReact = false;

	if( IsMyUnit() == true && m_pComboGuide != NULL )
	{
		wstring comboId, comboCommand;
		comboId = L"";
		comboCommand = L"";
		if( m_LuaManager.BeginTable( "COMBO_GUIDE" ) == true )
		{
			LUA_GET_VALUE( m_LuaManager, 1,	comboId,		L"" );
			LUA_GET_VALUE( m_LuaManager, 2,	comboCommand,	L"" );

			m_LuaManager.EndTable();
		}

		if(comboId == L"")
		{
			m_pComboGuide->ShowComboCommand(L"", L"", false);
		}
		else
		{
			m_pComboGuide->ShowComboCommand(comboId, comboCommand);
		}
	}




	index = 0;
	while( m_LuaManager.BeginTable( "EVENT_PROCESS_TIME", index ) == true )
	{
		if( index >= 10 )
		{
			m_LuaManager.EndTable();
			break;
		}
		float fDummy = 0.f;
		LUA_GET_VALUE( m_LuaManager, 1, fDummy, 0.f );
		LUA_GET_VALUE( m_LuaManager, 2, fDummy, 9999.f );
		LUA_GET_VALUE( m_LuaManager, 3, m_fEventTime[index].stateChange, 9999.f );
		index++;
		m_LuaManager.EndTable();
	}

	LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER",					m_fSkillCancelAfter,			9999.f );
	LUA_GET_VALUE(	m_LuaManager, "BWALK_CANCEL_AFTER",					m_fBWalkCancelAfter,			9999.f );
	LUA_GET_VALUE(	m_LuaManager, "WALK_CANCEL_AFTER",					m_fWalkCancelAfter,				9999.f );
	LUA_GET_VALUE(	m_LuaManager, "DASH_CANCEL_AFTER",					m_fDashCancelAfter,				9999.f );
	LUA_GET_VALUE(	m_LuaManager, "ATTACK_CANCEL_AFTER",				m_fAttackCancelAfter,			m_fWalkCancelAfter );
	LUA_GET_VALUE(	m_LuaManager, "ATTACK_CANCEL_AFTER_EX",				m_fAttackCancelAfterEx,			9999.f );


	LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER_FLY",				m_fSkillCancelAfterFly,			9999.f );		/// ���߿� ��ų ĵ��

	LUA_GET_VALUE( m_LuaManager, "IS_NOCHANGE_SPECTRO",		m_FrameDataNow.stateParam.bNoChangSpectro,		false );

	LUA_GET_VALUE(	m_LuaManager, "DAMAGE_DATA_CHANGE_TIME",			m_fCommonDamageChangeTime,		-1.f );

	LUA_GET_VALUE(	m_LuaManager, "CHANGE_WORLD_COLOR",					m_bChangeWorldColor,		false );
	LUA_GET_VALUE(	m_LuaManager, "SHOW_SKILL_CUT_IN_TIME",				m_fShowSkillCutInTime,		-1.f );

	DoCustomFunctionByBuffInCommonStateStart();
	ApplyDrainHpBuff();

	/// �޸� ȿ�� �������� ���Ͽ� ������ ������, ���� ��ų ������ ��� ����
	/*SetDamageReducePercent( 0 );		/// ������Ʈ�� ���۵� ��, ������ ����ġ�� 0���� �ʱ�ȭ �����ش�.*/

	ForceDownAndDetonationProcess();

		ParsingBuffFactorID( m_LuaManager, m_bApplyMemo );

}


void CX2GUUser::CommonFrameMoveFuture()
{
	KTDXPROFILE();

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	++m_dwFrameMoveCountFuture;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_ElapsedTimeNotBusyState.OnFrameMove( m_fElapsedTime );


	if ( m_iGrapShakeCount > 0 )
	{
		if ( m_InputData.pureLeft == true && m_InputData.pureRight == false && m_bGrapEscapeKeyLeftCheck == false )
		{
			m_bGrapEscapeKeyLeftCheck = true;
			m_bGrapEscapeKeyRightCheck	= false;
			m_iGrapShakeCount--;
			m_InputData.Init();
		}
		else if ( m_InputData.pureRight == true && m_InputData.pureLeft == false && m_bGrapEscapeKeyRightCheck == false )
		{
			m_bGrapEscapeKeyRightCheck	= true;
			m_bGrapEscapeKeyLeftCheck	= false;
			m_iGrapShakeCount--;
			m_InputData.Init();
		}
	}
	else
	{
		m_bGrapEscapeKeyLeftCheck = false;
		m_bGrapEscapeKeyRightCheck = false;
	}

	float fNowAnimTime = m_pXSkinAnimFuture->GetNowAnimationTime();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() )
		fNowAnimTime = m_ptrRidingPet->GetXSkinAnimFuturePtr()->GetNowAnimationTime();

	// ��ũ��Ʈ �󿡼� canPassUnit, canPushUnit�� �ð��뺰�� ������ �ټ� �ֵ��� �Ѵ�.
	if( m_pXSkinAnimFuture != NULL && m_vecCanPushUnit.size() > 0)
	{
		for(UINT i=0; i<m_vecCanPushUnit.size(); ++i)
		{
			TIME_PUSHPASS canPushUnitTime = m_vecCanPushUnit[i];

			if( canPushUnitTime.fTime <= fNowAnimTime )
			{
				m_FrameDataFuture.stateParam.bCanPushUnit = canPushUnitTime.bCan;
				m_vecCanPushUnit.erase(m_vecCanPushUnit.begin() + i);
				--i;
			}
		}
	}
	if( m_pXSkinAnimFuture != NULL && m_vecCanPassUnit.size() > 0)
	{
		for(UINT i=0; i<m_vecCanPassUnit.size(); ++i)
		{
			TIME_PUSHPASS canPassUnitTime = m_vecCanPassUnit[i];

			if( canPassUnitTime.fTime <= fNowAnimTime )
			{
				m_FrameDataFuture.stateParam.bCanPassUnit = canPassUnitTime.bCan;
				m_vecCanPassUnit.erase(m_vecCanPassUnit.begin() + i);
				--i;
			}
		}
	}

	// ��ũ��Ʈ �󿡼� Ư�� �ð��� Ư�����ǵ带 ������ �ټ� �ֵ����Ѵ�.
	if( m_pXSkinAnimFuture != NULL && m_vecSpeedFactor.size() > 0)
	{
		for(UINT i=0; i<m_vecSpeedFactor.size(); ++i)
		{
			TIME_SPEED timeSpeed = m_vecSpeedFactor[i];

			if( timeSpeed.iFlag > 0 && timeSpeed.vSpeed.z >= 0.f && timeSpeed.vSpeed.z <= fNowAnimTime )
			{
				switch(timeSpeed.iFlag)
				{
				case 1:
					m_PhysicParam.nowSpeed.x += timeSpeed.vSpeed.x;
					m_PhysicParam.nowSpeed.y += timeSpeed.vSpeed.y;
					break;
				case 2:
					m_PhysicParam.nowSpeed.x = timeSpeed.vSpeed.x;
					m_PhysicParam.nowSpeed.y = timeSpeed.vSpeed.y;
					break;
				case 3:
					m_PhysicParam.passiveSpeed.x = timeSpeed.vSpeed.x;
					m_PhysicParam.passiveSpeed.y = timeSpeed.vSpeed.y;
					break;
				default:
					break;
				}

				m_vecSpeedFactor.erase(m_vecSpeedFactor.begin() + i);
				--i;
			}
		}
	}

	m_FrameDataFuture.unitCondition.fStateTimeBack	= m_FrameDataFuture.unitCondition.fStateTime;
	m_FrameDataFuture.unitCondition.fStateTime		+= m_fElapsedTime;



	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= m_vDisableGravityInScriptTime.x &&
		m_pXSkinAnimFuture->GetNowAnimationTime() < m_vDisableGravityInScriptTime.y )
	{
		m_bDisableGravityInScript = true;
	}
	else
	{
		m_bDisableGravityInScript = false;
	}

	m_fLastDamagePassTime += m_fElapsedTime;
	if( m_fLastDamagePassTime >= 2.0f )
	{
		AddForceDownGage( -10.0f * m_fElapsedTime );
	}


	if( m_fCanNotAttackTime > 0.0f )
	{
		m_fCanNotAttackTime -= m_fElapsedTime;
		if( m_fCanNotAttackTime < 0.0f )
			m_fCanNotAttackTime = 0.0f;
	}
	if( m_fCanNotJumpTime > 0.0f )
	{
		m_fCanNotJumpTime -= m_fElapsedTime;
		if( m_fCanNotJumpTime < 0.0f )
			m_fCanNotJumpTime = 0.0f;
	}
	if( m_fCanNotInputTime > 0.0f )
	{
		m_fCanNotInputTime -= m_fElapsedTime;
		if( m_fCanNotInputTime < 0.0f )
			m_fCanNotInputTime = 0.0f;
	}

	if( m_fCanNotInputTimeZXArrow > 0.0f )
	{
		m_fCanNotInputTimeZXArrow -= m_fElapsedTime;
		if( m_fCanNotInputTimeZXArrow < 0.0f )
			m_fCanNotInputTimeZXArrow = 0.0f;
	}




	m_fReverseGravitySpeedDeltaY = 0.f;
	if( true == g_pX2Game->IsInReverseGravityRegion(
        m_fElapsedTime,
        GetPos(true ), m_fReverseGravitySpeedDeltaY, m_vReverseGravityRotateSpeedDelta, GetTeam() ) )
	{
		m_fCanNotInputTime = 0.1f;
		m_InputData.Init();
	}


	if(GetHold() == true)
	{
		m_FrameDataFuture.syncData.position = m_vHold;
	}

	if (GetGrap() == true)
	{
		m_FrameDataFuture.syncData.position = m_vGrap;
	}

	if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
		m_pXSkinAnimFuture->GetMatrix().Move( GetSaddlePos() );
	else
		m_pXSkinAnimFuture->GetMatrix().Move( m_FrameDataFuture.syncData.position );


	{
		if(m_fReverseGravitySpeedDeltaY == 0.f)
		{
			if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
				m_pXSkinAnimFuture->GetMatrix().RotateDegree( GetSaddleDegree() );
			else
				m_pXSkinAnimFuture->GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );
		}
	}

    bool    bXSkinAnimFutureOnFrameMoveCalled = false;
    bool    bRidingPetXSkinAnimFutureOnFrameMoveCalled = false;
	if( m_FrameDataFuture.syncData.bFrameStop == false )
	{
		if( m_StopAdvanceTimeFuture == false )
		{
			m_pXSkinAnimFuture->OnFrameMove( m_fTime, m_fElapsedTime );
            bXSkinAnimFutureOnFrameMoveCalled = true;
			if ( true == GetRidingOn() && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() )
            {
				m_ptrRidingPet->GetXSkinAnimFuturePtr()->OnFrameMove( m_fTime, m_fElapsedTime );
                bRidingPetXSkinAnimFutureOnFrameMoveCalled = true;
            }
		}

		GetGageData()->OnFrameMove( m_fTime, m_fElapsedTime );
	}

    if ( bXSkinAnimFutureOnFrameMoveCalled == false )
        m_pXSkinAnimFuture->UpdateBeforeAnimationTime();
    if ( bRidingPetXSkinAnimFutureOnFrameMoveCalled == false && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() )
        m_ptrRidingPet->GetXSkinAnimFuturePtr()->UpdateBeforeAnimationTime();


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_FrameDataFuture.syncData.dwFrameMoveCount		= g_pX2Game->GetFrameUDPPack().GetFrameMoveCount();
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_FrameDataFuture.syncData.dwFrameMoveCount		= GetFrameMoveCountFuture();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_FrameDataFuture.syncData.fNowHP				= GetNowHp();
	m_FrameDataFuture.syncData.fNowMP				= GetNowMp();

	m_FrameDataFuture.syncData.ucHitCount			= GetHitCount();
	m_FrameDataFuture.syncData.ucHittedCount		= GetHittedCount();
	m_FrameDataFuture.syncData.ucNumOfDeBuff		= GetNumOfDeBuff();

	CommonFrameMoveFuture_ExtraDamage();


	while( FlushSoul( 100.0f ) == true )
	{
		SetHyperModeCount( GetHyperModeCount() + 1 );
		if ( GetHyperModeCount() > 3 )
		{
			SetHyperModeCount( 3 );
		}
		else
		{
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"Dama.ogg", false, false );
		}
	}
	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
	{
		if( GetHyperModeCount() >= 1 )
		{	//�������� 1�� �̻� �� �� ���� ���̵� ���
			if( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
			{
				g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_HYPER_MODE, true, g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_AWAKE ));
			}
		}
		else
		{
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_HYPER_MODE, false);
		}
	}

	if( m_FrameDataFuture.syncData.bFrameStop == false )
	{





		//{{AFX
		{
			PhysicProcess();
		}
		//}}AFX





		if( m_PhysicParam.passiveSpeed.x != -1.0f )
			m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;
		if( m_PhysicParam.passiveSpeed.y != -1.0f )
			m_PhysicParam.nowSpeed.y = m_PhysicParam.passiveSpeed.y;

		if( NULL != m_pComboManager )
		{
			m_pComboManager->SetMyUnit( IsMyUnit(), this );
			m_pComboManager->OnFrameMove( m_fTime, m_fElapsedTime );
		}
	}

	CommonFrameMoveFuture_TimedEvent();


	if( true == g_pMain->GetGameOption().GetOptionList().m_bPlayGuide &&
		NULL != g_pX2Game &&
		this == g_pX2Game->GetMyUnit() )
	{
		m_fElaspedTimeLastGuideMessage += m_fElapsedTime;

		if( m_pComboGuide != NULL )
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.x -= 50.f;
			vPos.y -= 30.f;
			D3DXVECTOR2 vPosGuide = g_pKTDXApp->GetProj3DPos(vPos);
			vPosGuide.x /= g_pKTDXApp->GetResolutionScaleX();
			vPosGuide.y /= g_pKTDXApp->GetResolutionScaleY();

			m_pComboGuide->SetPos( vPosGuide );
			m_pComboGuide->SetShow( true );
		}
	}
	else
	{
		if( m_pComboGuide != NULL )
		{
			m_pComboGuide->SetShow( false );
		}
	}



	// �̹� �������� event process ó���� ���� �غ�

	m_bSpecialAttackEventProcessedAtThisFrame = false;

	m_FrameDataFuture.syncData.nowAction = UAI_NONE;
}




void CX2GUUser::CommonFrameMoveFuture_ExtraDamage()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	//Ư�������� üũ
	if( m_ExtraDamagePack.m_Poison.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Poison.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

	if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Fire.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );

		if ( IsMyUnit() )
			CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 1, 0, 0, 1) );
	}

	if( m_ExtraDamagePack.m_Lightning.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );

		if ( IsMyUnit() )
			CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 1, 0, 0, 1) );
	}
	if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Cold.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );

		if ( IsMyUnit() )
			CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 0, 0, 1, 1) );
	}


	if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		FlushMp( m_ExtraDamagePack.m_Curse.m_DamagePerSec * m_fElapsedTime );
	}

	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Fire.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );

		if ( IsMyUnit() )
			CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 0, 0, 1, 1) );
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

}

void CX2GUUser::CommonFrameMoveFuture_TimedEvent()
{
	for( int i = 0; i < (int)m_StopAllList.size(); i++ )
	{
		D3DXVECTOR3 stopTime = m_StopAllList[i];
        if( m_pXSkinAnimFuture->EventTimerOneshot( stopTime.x ) == true )
		{
			switch ( g_pX2Game->GetGameType() )
			{
			case CX2Game::GT_DUNGEON:
			case CX2Game::GT_BATTLE_FIELD:
				g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
				g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
				break;

			case CX2Game::GT_PVP:
				g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
				g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
				break;

			default:
				ASSERT( !L"wrong path" );
				break;
			}
		}
	}
	for( int i = 0; i < (int)m_Stop2AllList.size(); i++ )
	{
		D3DXVECTOR3 stopTime = m_Stop2AllList[i];
        if( m_pXSkinAnimFuture->EventTimerOneshot( stopTime.x ) == true )
		{
			switch ( g_pX2Game->GetGameType() )
			{
			case CX2Game::GT_DUNGEON:
			case CX2Game::GT_BATTLE_FIELD:
				g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
				g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
				break;

			case CX2Game::GT_PVP:
				g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
				g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
				break;

			default:
				ASSERT( !L"wrong path" );
				break;
			}
		}
	}
	for( int i = 0; i < (int)m_StopMyList.size(); i++ )
	{
		D3DXVECTOR2 stopTime = m_StopMyList[i];
        if( m_pXSkinAnimFuture->EventTimerOneshot( stopTime.x ) == true )
		{
			SetStopTime( stopTime.y );
		}
	}
	for( int i = 0; i < (int)m_Stop2MyList.size(); i++ )
	{
		D3DXVECTOR2 stopTime = m_Stop2MyList[i];
        if( m_pXSkinAnimFuture->EventTimerOneshot( stopTime.x ) == true )
		{
			SetStop2Time( stopTime.y );
		}
	}
}


void CX2GUUser::CommonFrameMove()
{
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	++m_dwFrameMoveCountNow;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	KTDXPROFILE();

	if ( GetNowStateID() == USI_RIDING_WAIT && m_ePlayedEmotion == CX2Unit::ET_NONE  )
    {
        m_ElapsedTimeRidingWaitHabit.OnFrameMove( m_fElapsedTime );
    }

	DoDelegateProcessInCommonFrameMove();

	if( m_hSeqHeadMarker != INVALID_PARTICLE_SEQUENCE_HANDLE)
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarker    = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
		if( pSeqHeadMarker != NULL )
		{
			if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
				pSeqHeadMarker->SetShowObject(false);
			else
				pSeqHeadMarker->SetShowObject(true);
		}
	}


	if( m_hSeqEmblem != INVALID_PARTICLE_SEQUENCE_HANDLE)
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );

		if( pSeqEmblem != NULL )
		{
			bool bIsEmblemShow = true;			// Īȣ�� ���ܾ� �ϴ°�?
			//
			if ( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() ) // ��ũ���� ��� �ΰ�?
			{
				bIsEmblemShow = false;
			}
			else if ( CX2GameUnit::GUSI_DIE == GetGameUnitState () ) // ������ State �� ���� ���¶��..
			{
				bIsEmblemShow = false;
			}
			pSeqEmblem->SetShowObject( bIsEmblemShow );

		}
	}






	GetUnit()->AccessUnitData().m_UserSkillTree.OnFrameMove( m_fTime, m_fElapsedTime );

	if( m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY > 0.f )
	{
		m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY -= m_fElapsedTime;
		if( m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY < 0.f )
			m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY = 0.f;
	}

	if( m_fSuperArmorByItem > 0.f )
	{
		m_fSuperArmorByItem -= m_fElapsedTime;
		if( m_fSuperArmorByItem <= 0.f )
		{
			m_bSuperArmorByItem = false;
			m_fSuperArmorByItem = 0.f;
		}
	}


	// Drain Particle
	if( m_hSeqDrainParticle != INVALID_PARTICLE_SEQUENCE_HANDLE &&
		null != m_optrLastAttackUnit &&
		CX2GameUnit::GUT_USER == m_optrLastAttackUnit->GetGameUnitType() )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDrainParticle );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 pos = m_optrLastAttackUnit->GetWeaponBonePos( 0, L"TRACE_START0", 0 );
			pos.y += 60.0f;
			pSeq->SetBlackHolePosition( pos );
			if( pSeq->GetTime() > 1.5f )
			{
				pSeq->SetGravity( CMinMax<D3DXVECTOR3>(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) );
				pSeq->SetUseLand( false );
			}
		}
		else
		{
			m_hSeqDrainParticle = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}


	if( m_petCheer.m_fTime > 0.f )
	{
		if( GetNowHp() <= 0.f )
			m_petCheer.m_fTime = 0.f;

		m_petCheer.m_fTime -= m_fElapsedTime;

		if( m_petCheer.m_fTime <= 0.f && m_petCheer.m_fChangeHp > 0.f )
		{
			m_petCheer.m_fTime = 0.f;

			SetMaxHp( GetMaxHp() / m_petCheer.m_fChangeHp );

			if( GetNowHp() > GetMaxHp() )
			{
				SetNowHp( GetMaxHp() );
			}

			m_petCheer.Init();
		}
		else if( m_petCheer.m_fTime > 0.f && m_petCheer.m_fChargeHp > 0.f )
		{
			SetNowHp( GetNowHp() + (GetMaxHp() * m_petCheer.m_fChargeHp * m_fElapsedTime) );
			if( GetNowHp() > GetMaxHp() )
			{
				SetNowHp( GetMaxHp() );
			}
		}
	}

	if( m_pXSkinAnimFuture != NULL )
	{
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
			CommonFrameMove_EffectSet( m_ptrRidingPet->GetXSkinAnimPtr()->GetNowAnimationTime() );
		else
			CommonFrameMove_EffectSet( m_pXSkinAnim->GetNowAnimationTime() );
	}





	if( m_FrameDataFuture.syncData.nowState == GUSI_DIE )
	{
		return;
	}

	NoStateChangeActionFrameMove();

	EmotionFrameMove();

	bool bSuperArmor = m_bSocketSuperArmor || m_bSuperArmorByItem;
	if( m_fSocketSuperArmor <= 0.f )
		m_bSocketSuperArmor = false;
	if( m_fSuperArmorByItem <= 0.f )
		m_bSuperArmorByItem = false;

	if( m_bSocketSuperArmor == false && m_bSuperArmorByItem == false )
	{
		if( m_bStateSuperArmor == false )
			m_FrameDataNow.stateParam.bSuperArmor = false;
	}
	else
	{
		if( m_bStateSuperArmor == false )
			m_FrameDataNow.stateParam.bSuperArmor = true;
	}


	float fAnimTime = -1.f;
	if( m_pXSkinAnim != NULL )
		fAnimTime = m_pXSkinAnim->GetNowAnimationTime();

    const std::vector<D3DXVECTOR2>& vecSuperArmorTime = m_vecNowSuperArmorTime;
	for(UINT iSuperArmorTime=0; iSuperArmorTime < vecSuperArmorTime.size(); ++iSuperArmorTime)
	{
		const D3DXVECTOR2& superArmorTime = vecSuperArmorTime[iSuperArmorTime];
		if( fAnimTime >= superArmorTime.x && fAnimTime <= superArmorTime.y )
		{
			m_FrameDataNow.stateParam.bSuperArmor = true;
			break;
		}
	}
	//#ifdef HEAVY_LOG_TEST
	//	const CKTDXCollision::CollisionDataListSet& subatkList = GetSubAttackListSet();
	//	if( subatkList.size() > 0 )
	//	{
	//		std::set<const CKTDXCollision::CollisionDataList*>::const_iterator it = subatkList.begin();
	//		const CKTDXCollision::CollisionDataList* pColList = *it;
	//		if( pColList->size() == 0 )
	//		{
	//			ASSERT( !"impossible" );
	//		}
	//	}
	//#endif HEAVY_LOG_TEST

//#ifdef  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX
    {
        float   fNowAnimTime = fAnimTime;
	        if ( true == GetRidingOn() && NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
		        fNowAnimTime = m_ptrRidingPet->GetXSkinAnimPtr()->GetNowAnimationTime();


	    // ��ũ��Ʈ �󿡼� canPassUnit, canPushUnit�� �ð��뺰�� ������ �ټ� �ֵ��� �Ѵ�.
	    if( m_pXSkinAnim != NULL && m_vecCanPushUnitNow.size() > 0)
	    {
		    for(UINT i=0; i<m_vecCanPushUnitNow.size(); ++i)
		    {
			    TIME_PUSHPASS canPushUnitTime = m_vecCanPushUnitNow[i];

			    if( canPushUnitTime.fTime <= fNowAnimTime )
			    {
				    m_FrameDataNow.stateParam.bCanPushUnit = canPushUnitTime.bCan;
				    m_vecCanPushUnitNow.erase(m_vecCanPushUnitNow.begin() + i);
				    --i;
			    }
		    }
	    }
	    if( m_pXSkinAnim != NULL && m_vecCanPassUnitNow.size() > 0)
	    {
		    for(UINT i=0; i<m_vecCanPassUnitNow.size(); ++i)
		    {
			    TIME_PUSHPASS canPassUnitTime = m_vecCanPassUnitNow[i];

			    if( canPassUnitTime.fTime <= fNowAnimTime )
			    {
				    m_FrameDataNow.stateParam.bCanPassUnit = canPassUnitTime.bCan;
				    m_vecCanPassUnitNow.erase(m_vecCanPassUnitNow.begin() + i);
				    --i;
			    }
		    }
	    }
    }
//#endif  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX



//#ifdef LINEMAP_FAST_WIND_TEST
//	if( true == m_bEnableUpsideWind )
//	{
//		if( true == m_bWaitInTheAir )
//		{
//			if( m_fTimeInTheAir > 0.f )
//			{
//				m_fTimeInTheAir -= m_fElapsedTime;
//			}
//			else if( m_fTimeInTheAir < 0.f )
//			{
//				m_fTimeInTheAir = 0.f;
//			}
//		}
//	}
//#endif LINEMAP_FAST_WIND_TEST



	m_bUpMPThisFrame = false;

	m_FrameDataNow.unitCondition.fStateTimeBack	= m_FrameDataNow.unitCondition.fStateTime;
	m_FrameDataNow.unitCondition.fStateTime		+= m_fElapsedTime;


	KTDXPROFILE_BEGIN( "ANIM" );





	if(GetHold() == true)
	{
		m_FrameDataNow.syncData.position = m_vHold;
	}

	if ( GetGrap() == true )
	{
		m_FrameDataNow.syncData.position = m_vGrap;
	}

	if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
		m_pXSkinAnim->GetMatrix().Move( GetSaddlePos() );
	else
		m_pXSkinAnim->GetMatrix().Move( m_FrameDataNow.syncData.position );


	{
		if(m_fReverseGravitySpeedDeltaY == 0.f)
			if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
				m_pXSkinAnim->GetMatrix().RotateDegree( GetSaddleDegree() );
			else
				m_pXSkinAnim->GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );
	}

    bool    bXSkinAnimOnFrameMoveCalled = false;
	if( m_FrameDataNow.syncData.bFrameStop == false )
	{
		if( m_bPopAgain == false )
		{
			m_pXSkinAnim->OnFrameMove( m_fTime, m_fElapsedTime
                );
            bXSkinAnimOnFrameMoveCalled = true;

//#ifdef HEAD_INVERSE_KINEMATICS_TEST
//
//			if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH )
//			{
//				HeadIKFrameMove( m_fTime, m_fElapsedTime );
//			}
//
//#endif HEAD_INVERSE_KINEMATICS_TEST



			if( m_bRotateBone == true )
			{
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_Spine1" );
				RotateBone( pFrame, D3DXVECTOR3( 0, 0, -1 ), m_fRotateBoneDegree );
			}


			for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
			{
				CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
				if ( pCX2Eqip == NULL )
					continue;

				//{{ robobeg : 2008-10-17
				//pCX2Eqip->OnFrameMove( m_fTime, m_fElapsedTime * m_AdvanceTimeCount, m_pXSkinAnim->GetRenderParam() );
				pCX2Eqip->SetRenderParam( m_fTime, m_fElapsedTime , m_pXSkinAnim->GetRenderParam() );
				pCX2Eqip->OnFrameMove( m_fTime, m_fElapsedTime );
				//}} robobeg : 2008-10-17


				if ( pCX2Eqip->GetItem() != NULL &&
                    pCX2Eqip->GetItem()->GetItemTemplet() != NULL
                    )
				{
					CX2Unit::EQIP_POSITION Epos = pCX2Eqip->GetItem()->GetItemTemplet()->GetEqipPosition();
					bool bFashion = pCX2Eqip->GetItem()->GetItemTemplet()->GetFashion();
					if( bFashion == true )
					{
						CX2Item* pTempItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( pCX2Eqip->GetItem()->GetItemTemplet()->GetEqipPosition(), false );

						//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����
						if( pTempItem != NULL && pCX2Eqip->GetEnchantLevel() != pTempItem->GetItemData().m_EnchantLevel &&
							( ( pTempItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pTempItem->GetItemData().m_Endurance > 0) ||
								pTempItem->GetItemData().m_PeriodType == CX2Item::PT_INFINITY ) )
						{
							pCX2Eqip->ChangeEnchantLevel( pTempItem->GetItemData().m_EnchantLevel );
						}


						if( pCX2Eqip->GetEnchantLevel() != 0 )
						{
							if ( pTempItem == NULL ||
								( pTempItem != NULL &&
									pTempItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE && pTempItem->GetItemData().m_Endurance <= 0 ) )
								pCX2Eqip->ChangeEnchantLevel( 0 );
						}
						//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ����
					}
				}
			}


		}
	}
	KTDXPROFILE_END();
    if ( bXSkinAnimOnFrameMoveCalled == false )
    {
        m_pXSkinAnim->UpdateBeforeAnimationTime();
    }






	CKTDGParticleSystem::CParticleEventSequence* pSeq_Poison		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPoisonBody );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Fire1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody1 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Fire2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody2 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Curse 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCurseBody );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_SlowRight 	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_SlowLeft  	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyLeft  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold_Head		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold_Head );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold1L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold2L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold1R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1_Right  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold2R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2_Right  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Pepper1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody1  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Pepper2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody2  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Stun 			= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqStun  );





	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Stun				= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Stun_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun_Second  );

	/// Old Blaze
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Blaze_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Blaze_Second  );
	/// Old Blaze

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Frozen_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Frozen_Second  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Poison_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Poison_Second  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation2		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation2  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation3		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation3  );

	/// EDT �̸�Ƽ��
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_ArrowOfExplosion  = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_ArrowOfExplosion );

	/// ��ø�� �̸�Ƽ��
	CKTDGParticleSystem::CParticleEventSequence* pSeq_StartOfDelayedFiringOverlap  = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hStartOfDelayedFiringOverlap );

	D3DXVECTOR3 poisonBodyEffPos = GetPos();
	poisonBodyEffPos.y += 100.0f;
	if( pSeq_Poison != NULL )
		pSeq_Poison->SetPosition( poisonBodyEffPos );
	if( pSeq_Curse != NULL )
		pSeq_Curse->SetPosition( poisonBodyEffPos );

	D3DXVECTOR3 vEmoticonPos = GetPos() + D3DXVECTOR3( 0, 250.f, 0 ) - 40.f * GetZVector();

	D3DXVECTOR3 vOverlapEmoticonPos = GetPos() + D3DXVECTOR3( 0, 310.f, 0 ) - 40.f * GetZVector();	/// ��ø�� EDT Ƚ�� ǥ�� �̸�Ƽ�� ��ġ

	vOverlapEmoticonPos += 50.f * m_FrameDataNow.unitCondition.dirVector;

	if( pSeq_Cold_Head != NULL )
		pSeq_Cold_Head->SetPosition( vEmoticonPos );



	if( pSeq_Enchant_Stun != NULL )
		pSeq_Enchant_Stun->SetPosition( vEmoticonPos );

	if( pSeq_Enchant_Exclamation != NULL )
	{
		pSeq_Enchant_Exclamation->SetPosition( vOverlapEmoticonPos );
	}

	if( pSeq_Enchant_Exclamation2 != NULL )
	{
		pSeq_Enchant_Exclamation2->SetPosition( vOverlapEmoticonPos );
	}

	if( pSeq_Enchant_Exclamation3 != NULL )
	{
		pSeq_Enchant_Exclamation3->SetPosition( vOverlapEmoticonPos );
	}

	if ( m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation >= 2 ||
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 2 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 2 )
	{
		if( m_hSeqEnchant_Exclamation3 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqEnchant_Exclamation3 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Overlap_EDT_Emoticon_3", 0,0,0 );
            pSeq_Enchant_Exclamation3		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation3  );
        }

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->SetEmitRate( 20, 20 );
		}


		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

	}
	else if ( m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation >= 1 ||
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 1 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 1 )
	{
		if( m_hSeqEnchant_Exclamation2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqEnchant_Exclamation2 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Overlap_EDT_Emoticon_2", 0,0,0 );
                pSeq_Enchant_Exclamation2		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation2  );
        }

		if( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->SetEmitRate( 20, 20 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

	}
	else if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0 ||
		m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0  )
	{
		if( m_hSeqEnchant_Exclamation == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqEnchant_Exclamation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( static_cast<CKTDGObject*>(this),  L"Overlap_EDT_Emoticon_1", 0,0,0 );
                pSeq_Enchant_Exclamation		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation  );
        }

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->SetEmitRate( 20, 20 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}
	}
	else
	{

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}
	}

	if( NULL != pSeq_Enchant_Blaze_Second )
		pSeq_Enchant_Blaze_Second->SetPosition( vEmoticonPos );

	if( pSeq_Enchant_Frozen_Second != NULL )
		pSeq_Enchant_Frozen_Second->SetPosition( vEmoticonPos );

	if( pSeq_Enchant_Poison_Second != NULL )
		pSeq_Enchant_Poison_Second->SetPosition( vEmoticonPos );

	if( pSeq_Enchant_Stun_Second != NULL )
		pSeq_Enchant_Stun_Second->SetPosition( vEmoticonPos );


	if( pSeq_Stun != NULL )
		pSeq_Stun->SetPosition( vEmoticonPos );

	/// mauntain : ����ȯ [2012.05.21] ���� 2�� ���� ����Ʈ ���� - ������ ��ȣź
	if( NULL != pSeq_Enchant_ArrowOfExplosion )		/// EDT �̸�Ƽ��
	{
		pSeq_Enchant_ArrowOfExplosion->SetPosition( vEmoticonPos );

		if( false == pSeq_Enchant_ArrowOfExplosion->GetShowObject() )
			pSeq_Enchant_ArrowOfExplosion->SetShowObject( true );
	}

	if( NULL != pSeq_StartOfDelayedFiringOverlap )	/// ��ø �̸�Ƽ��
	{
		pSeq_StartOfDelayedFiringOverlap->SetPosition( vOverlapEmoticonPos );

		if( false == pSeq_StartOfDelayedFiringOverlap->GetShowObject() )
			pSeq_StartOfDelayedFiringOverlap->SetShowObject( true );
	}


	D3DXVECTOR3 fireBodyEffPos1 = GetPos();
	fireBodyEffPos1.y += 120.0f;
	fireBodyEffPos1.x += 30.0f;
	if( pSeq_Fire1 != NULL )
		pSeq_Fire1->SetPosition( fireBodyEffPos1 );
	D3DXVECTOR3 fireBodyEffPos2 = GetPos();
	fireBodyEffPos2.y += 80.0f;
	fireBodyEffPos2.x -= 20.0f;
	if( pSeq_Fire2 != NULL )
		pSeq_Fire2->SetPosition( fireBodyEffPos2 );

	if( pSeq_Pepper1 != NULL )
		pSeq_Pepper1->SetPosition( fireBodyEffPos1 );
	if( pSeq_Pepper2 != NULL )
		pSeq_Pepper2->SetPosition( fireBodyEffPos2 );

	D3DXVECTOR3 lFoot;
	GetFramePos( &lFoot, m_pFrame_Bip01_L_Foot );
	D3DXVECTOR3 rFoot;
	GetFramePos( &rFoot, m_pFrame_Bip01_R_Foot );

	if( pSeq_SlowRight != NULL )
	{
		pSeq_SlowRight->SetPosition( rFoot );
	}
	if( pSeq_SlowLeft != NULL )
	{
		pSeq_SlowLeft->SetPosition( lFoot );
	}

	if( pSeq_Cold1L != NULL )
	{
		pSeq_Cold1L->SetPosition( lFoot );
	}
	if( pSeq_Cold2L != NULL )
	{
		pSeq_Cold2L->SetPosition( lFoot );
	}
	if( pSeq_Cold1R != NULL )
	{
		pSeq_Cold1R->SetPosition( rFoot );
	}
	if( pSeq_Cold2R != NULL )
	{
		pSeq_Cold2R->SetPosition( rFoot );
	}



	CommonFrameMove_Particle();

	CommonFrameMoveStateAbnormality();

	if( m_vecFreezePtr.empty() == false || m_vecStunPtr.empty() == false )
	{
		if ( this == g_pX2Game->GetMyUnit() )
			g_pX2Game->SetEnableAllKeyProcess( false );

		if( (m_vecStunPtr.empty() == false && GetNowStateID() != GetDamageGroggyStateID())
			&& (false == m_pGageData->GetActivateDetonation() && false == m_pGageData->GetActivateForceDown())
			)
		{

			if( true == m_bChangeGroggyState )
			{
				/// Ż�Ϳ� ž�½ÿ��� Ż���� ������Ʈ�� ��ȯ
				if ( true == GetRidingOn() )
					StateChange( USI_RIDING_DAMAGE_FRONT );
				else
					StateChange( GetDamageGroggyStateID() );

				m_bChangeGroggyState = false;
			}
			else
			{
				switch ( GetNowStateID() )
				{
				case USI_DAMAGE_SMALL_FRONT:
				case USI_DAMAGE_SMALL_BACK:
				case USI_DAMAGE_BIG_FRONT:
				case USI_DAMAGE_BIG_BACK:
				case USI_DAMAGE_STANDUP_FRONT:
				case USI_DAMAGE_STANDUP_BACK:
				case USI_DAMAGE_AIR_SMALL:
				case USI_DAMAGE_AIR_DOWN:
				case USI_DAMAGE_AIR_DOWN_LANDING:
				case USI_DAMAGE_AIR_FALL:
				case USI_DAMAGE_AIR_UP:
				case USI_DAMAGE_AIR_FLY_FRONT:
				case USI_DAMAGE_AIR_FLY_BACK:
				case USI_DAMAGE_DOWN_FRONT:
				case USI_DAMAGE_DOWN_BACK:
				case USI_DAMAGE_AIR_DOWN_INVINCIBLE:
				case USI_RIDING_DAMAGE_FRONT:
				case USI_RIDING_DAMAGE_BACK:
					break;

				default:
					{
						/// Ż�Ϳ� ž�½ÿ��� Ż���� ������Ʈ�� ��ȯ
						if ( true == GetRidingOn() )
							StateChange( USI_RIDING_DAMAGE_FRONT );
						else
							StateChange( GetDamageGroggyStateID() );
					}
					break;
				}
			}
		}
	}
	else
	{
		if ( this == g_pX2Game->GetMyUnit() )
			g_pX2Game->SetEnableAllKeyProcess( true );
	}

	// fix!! �� �κ� ��ü framemove���� �ȵ��� event�� ó���ǵ��� �����ϱ�

	if ( m_delegateProcessReturnFromScaleChange )
		m_delegateProcessReturnFromScaleChange();

	float fAnimSpeedRate = 1.f;
	float fRunJumpSpeedRate = 1.0f;
	//bool bCheckEnchantFrozen = false;

	fAnimSpeedRate *= m_PhysicParam.GetAnimSpeed();

	if( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();

		if ( m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage >= 1.0f )
		{
			m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage = 0;

			float fPrevNow = GetNowHp();	// �Ӽ� ���������� ��� ����(�����)

			/// ���� �� �ߵ��Ǵ� ��ų �˻� �� ó��
			if ( false == ProcessSkillAtDie( m_ExtraDamagePack.m_EnchantPoison.m_DamagePerSec ) )
				UpNowHp(-m_ExtraDamagePack.m_EnchantPoison.m_DamagePerSec);

			D3DXVECTOR3 numPos;
			GetFramePos( &numPos, m_pFrame_Bip01_Head );
			numPos.y += 70.0f;
			if( NULL != g_pData->GetPicChar() )
			{
				g_pData->GetPicChar()->DrawText( static_cast<int>( fPrevNow - GetNowHp() ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
		}
	}

	if( m_ExtraDamagePack.m_Poison.m_fTime > 0.0f || m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
	{
		if( m_hSeqPoisonBody == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqPoisonBody = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitPoison", 0,0,0 );
                pSeq_Poison		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPoisonBody );
        }

		if( pSeq_Poison != NULL )
			pSeq_Poison->SetEmitRate( 10,20 );

		m_ExtraDamagePack.m_Poison.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Poison.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Poison.m_fTime = 0.0f;

		if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
		{
			if( m_hSeqEnchant_Poison_Second == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqEnchant_Poison_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Poison_Emoticon_Second", 0,0,0 );
                    pSeq_Enchant_Poison_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Poison_Second  );
            }

			if ( pSeq_Enchant_Poison_Second != NULL )
			{
				pSeq_Enchant_Poison_Second->SetEmitRate( 20, 40 );
			}



			const CX2EnchantItem::EnchantData* pEnchantData = NULL;
			if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
			{
				pEnchantData = g_pData->GetEnchantItem()->GetEnchantData( CX2DamageManager::EDT_ENCHANT_POISON );
			}
			else
			{
				pEnchantData = g_pData->GetEnchantItem()->GetNPCEnchantData( CX2DamageManager::EDT_ENCHANT_POISON );
			}


			if ( pEnchantData != NULL )
			{
				if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation == 0 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_FirstSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					// oasis907 : ����� [2010.10.5] // �Ӽ� ������ �̵��ӵ� -> �ӵ� ������� ����
					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;

				}
				else if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation == 1 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_SecondSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 2 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_ThirdSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;
				}
				if( fAnimSpeedRate >= 1.f )
				{
					fAnimSpeedRate = 1.f;
				}
			}
		}
		else
		{
			if( pSeq_Enchant_Poison_Second != NULL )
			{
				pSeq_Enchant_Poison_Second->ClearAllParticle( );
				pSeq_Enchant_Poison_Second->SetEmitRate( 0 ,0 );
			}
		}


		m_ExtraDamagePack.m_EnchantPoison.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantPoison.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantPoison.m_fTime = 0.0f;


		m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage += m_fElapsedTime;
	}
	else
	{
		if( pSeq_Poison != NULL )
			pSeq_Poison->SetEmitRate( 0,0 );



		if( pSeq_Enchant_Poison_Second != NULL )
		{
			pSeq_Enchant_Poison_Second->ClearAllParticle( );
			pSeq_Enchant_Poison_Second->SetEmitRate( 0 ,0 );
		}
	}

	if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.0f )
	{
		if ( this == g_pX2Game->GetMyUnit() )
			g_pX2Game->SetEnableAllKeyProcess( false );


		if ( m_ExtraDamagePack.m_EnchantShock.m_Accumulation >= 1 )
		{
			if ( pSeq_Enchant_Stun != NULL )
			{
				pSeq_Enchant_Stun->ClearAllParticle();
				pSeq_Enchant_Stun->SetEmitRate( 0, 0 );
			}

			if( m_hSeqEnchant_Stun_Second == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqEnchant_Stun_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Stun_Emoticon_Second", 0,0,0 );
                    pSeq_Enchant_Stun_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun_Second  );
            }

			if ( pSeq_Enchant_Stun_Second != NULL )
			{
				pSeq_Enchant_Stun_Second->SetEmitRate( 20, 40 );
			}
		}
		else
		{
			if ( pSeq_Enchant_Stun_Second != NULL )
			{
				pSeq_Enchant_Stun_Second->ClearAllParticle();
				pSeq_Enchant_Stun_Second->SetEmitRate( 0, 0 );
			}

			if( m_hSeqEnchant_Stun == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqEnchant_Stun = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Stun_Emoticon", 0,0,0 );
                    pSeq_Enchant_Stun				= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun  );
            }

			if ( pSeq_Enchant_Stun != NULL )
			{
				pSeq_Enchant_Stun->SetEmitRate( 20, 40 );
			}
		}


		m_ExtraDamagePack.m_EnchantShock.m_Accumulation = 1;

		m_FrameDataNow.stateParam.bSuperArmor = true;
		m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
		m_fSocketSuperArmor = 0.f;
		m_bSocketSuperArmor = false;

		m_ExtraDamagePack.m_EnchantShock.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.0f )
		{
			m_ExtraDamagePack.m_EnchantShock.m_fTime = 0.0f;
			if ( m_ExtraDamagePack.m_EnchantShock.m_Accumulation == 1 )
			{
				if ( this == g_pX2Game->GetMyUnit() )
					g_pX2Game->SetEnableAllKeyProcess( true );

				if ( GetNowHp() > 0.f )
				{
					if ( m_bReserveForceDownForShock == true )
						if ( true == GetRidingOn() )
							StateChange( USI_RIDING_DAMAGE_BACK, true );
						else
							StateChange( USI_DAMAGE_SMALL_BACK, true );
					else
					{
						if( m_ExtraDamagePack.m_EnchantShock.m_bCured == true )
						{
							m_ExtraDamagePack.m_EnchantShock.m_bCured = false;
							m_bReserveForceDownForShock = false;

							if ( true == GetRidingOn() )
								StateChange( USI_RIDING_DAMAGE_FRONT, true );
							else
								StateChange( USI_DAMAGE_SMALL_FRONT, true );
						}
						else
						{
							m_bReserveForceDownForShock = false;
							if ( true == GetRidingOn() )
								StateChange( USI_RIDING_DAMAGE_FRONT, true );
							else
								StateChange( USI_DAMAGE_DOWN_FRONT, true );
						}
					}

					g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Smoke_Stone_Property01", GetPos() );
					g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Stone_Property01", GetPos(), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,0) );

						DeleteEffectSetOnDamageReact();

						// ������ ������Ʈ���� ����Ʈ�� ����� ���, �ǰ� �� ���� �ϵ��� ó��
						if( true == m_bIsCustomStateDeleteEffectOnDamageReact )
							DeleteEffectSetOnCustomState();
				}



				m_ExtraDamagePack.m_EnchantShock.m_Accumulation = 0;
			}
		}
	}
	else
	{
		m_bReserveForceDownForShock = false;

		if ( pSeq_Enchant_Stun != NULL)
		{
			pSeq_Enchant_Stun->ClearAllParticle();
			pSeq_Enchant_Stun->SetEmitRate(0, 0 );
		}

		if ( pSeq_Enchant_Stun_Second != NULL )
		{
			pSeq_Enchant_Stun_Second->ClearAllParticle();
			pSeq_Enchant_Stun_Second->SetEmitRate( 0, 0 );
		}
	}

	if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation = 0;
	}

	/// �� ��� �������� ------------------------------------------------------------------------------------------------------
	if( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f && GetNowHp() > 0.0f )
	{
		float fBeforeHP = GetNowHp();
		//m_GageManager.GetHPGage()->fNow -= m_ExtraDamagePack.m_Fire.m_DamagePerSec * m_fElapsedTime;
		if ( m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage >= 1.0f )
		{
			m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage = 0;

			float fPrevNow = GetNowHp();

			/// ���� �� �ߵ��Ǵ� ��ų �˻� �� ó��
			if ( false == ProcessSkillAtDie( m_ExtraDamagePack.m_EnchantBlaze.m_DamagePerSec ) )
				SetNowHp( fPrevNow - m_ExtraDamagePack.m_EnchantBlaze.m_DamagePerSec );


			//m_GageManager.CrashMyStateGageBar( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 1, 0, 0, 1) );

			D3DXVECTOR3 numPos;
			GetFramePos( &numPos, m_pFrame_Bip01_Head );
			numPos.y += 70.0f;
			if( NULL != g_pData->GetPicChar() )
			{
				g_pData->GetPicChar()->DrawText( (int)( fPrevNow - GetNowHp() ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
		}
	}

	if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f || m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
	{
		if( m_hSeqFireBody1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqFireBody1 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
                pSeq_Fire1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody1 );
        }
		if( m_hSeqFireBody2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqFireBody2 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
                pSeq_Fire2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody2 );
        }

		if( pSeq_Fire1 != NULL )
			pSeq_Fire1->SetEmitRate( 20,40 );
		if( pSeq_Fire2 != NULL )
			pSeq_Fire2->SetEmitRate( 20,40 );


		m_ExtraDamagePack.m_Fire.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Fire.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Fire.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantBlaze.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantBlaze.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantBlaze.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage += m_fElapsedTime;

		if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
		{
			if( m_hSeqEnchant_Blaze_Second == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqEnchant_Blaze_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Blaze_Emoticon_Second", 0,0,0 );
                     pSeq_Enchant_Blaze_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Blaze_Second  );
            }

			if ( pSeq_Enchant_Blaze_Second != NULL )
			{
				pSeq_Enchant_Blaze_Second->SetEmitRate( 20, 40 );
			}

		}
		else
		{
			if ( pSeq_Enchant_Blaze_Second != NULL )
			{
				pSeq_Enchant_Blaze_Second->ClearAllParticle();
				pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
			}
		}
	}
	else
	{
		if( pSeq_Fire1 != NULL )
			pSeq_Fire1->SetEmitRate( 0,0 );
		if( pSeq_Fire2 != NULL )
			pSeq_Fire2->SetEmitRate( 0,0 );



		if ( pSeq_Enchant_Blaze_Second != NULL )
		{
			pSeq_Enchant_Blaze_Second->ClearAllParticle();
			pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
		}


	}
	if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation = 0;
	}
	/// ----------------------------------------------------------------------------------------------------------------------


	if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f )
	{
		if( m_hSeqCurseBody == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCurseBody = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitCurse", 0,0,0 );
                pSeq_Curse 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCurseBody );
        }

		if( pSeq_Curse != NULL )
			pSeq_Curse->SetEmitRate( 10,20 );

		m_ExtraDamagePack.m_Curse.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Curse.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Curse.m_fTime = 0.0f;
	}
	else
	{
		if( pSeq_Curse != NULL )
			pSeq_Curse->SetEmitRate( 0,0 );
	}
	if( m_ExtraDamagePack.m_Slow.m_fTime > 0.0f )
	{
		if( m_hSeqSlowBodyRight == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqSlowBodyRight = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitSlow", 0,0,0 );
                pSeq_SlowRight 	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyRight );
        }
		if( m_hSeqSlowBodyLeft == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqSlowBodyLeft = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitSlow", 0,0,0 );
                pSeq_SlowLeft  	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyLeft  );
        }

		if( pSeq_SlowRight != NULL )
			pSeq_SlowRight->SetEmitRate( 5,10 );
		if( pSeq_SlowLeft != NULL )
			pSeq_SlowLeft->SetEmitRate( 5,10 );

		m_ExtraDamagePack.m_Slow.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Slow.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Slow.m_fTime = 0.0f;
	}
	else
	{
		if( pSeq_SlowRight != NULL )
			pSeq_SlowRight->SetEmitRate( 0,0 );
		if( pSeq_SlowLeft != NULL )
			pSeq_SlowLeft->SetEmitRate( 0,0 );
	}


	if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f ||
		m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.0f )
	{
		if ( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f )
		{
			if(m_ExtraDamagePack.m_Cold.m_iLevel < 0)
			{
				//������ = 0.2
				if ( fAnimSpeedRate > 0.8f )
					fAnimSpeedRate = 0.8f;
			}
			else
			{
				//������ = 0.2 + ( 0.0025 * EDT���� )
				float fAnimSpeedReduceRate = 0.2f + ( 0.0025f * (float)m_ExtraDamagePack.m_Cold.m_iLevel );
				if( fAnimSpeedRate > 1.f - fAnimSpeedReduceRate)
				{
					fAnimSpeedRate = 1.f - fAnimSpeedReduceRate;
				}
				if( fAnimSpeedRate >= 1.f )
				{
					fAnimSpeedRate = 1.f;
				}
			}

			if( false == m_bColdSlowApplied )
			{
				m_bColdSlowApplied = true;
			}
		}
		else
		{
			if( true == m_bColdSlowApplied )
			{
				m_bColdSlowApplied = false;
			}
		}

		if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.f )
		{
			m_bFrozenSlowApplied = true;

			if( m_hSeqEnchant_Frozen_Second == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqEnchant_Frozen_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Frozen_Emoticon_Second", 0,0,0 );
                    pSeq_Enchant_Frozen_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Frozen_Second  );
            }


			if ( pSeq_Enchant_Frozen_Second != NULL )
			{
				pSeq_Enchant_Frozen_Second->SetEmitRate( 20, 40 );
			}


			const CX2EnchantItem::EnchantData* pEnchantData = NULL;
			if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
			{
				pEnchantData = g_pData->GetEnchantItem()->GetEnchantData( CX2DamageManager::EDT_ENCHANT_FROZEN );
			}
			else
			{
				pEnchantData = g_pData->GetEnchantItem()->GetNPCEnchantData( CX2DamageManager::EDT_ENCHANT_FROZEN );
			}


			if ( pEnchantData != NULL )
			{
				if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation == 0 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_FirstSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;



					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation == 1 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_SecondSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;



					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 2 )
				{
					float fApplySlowFact = ( 1.0f - ((float)pEnchantData->m_ThirdSlowPercent / 100.0f) );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;



					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;
				}
			}
		}
		else
		{

			if( pSeq_Enchant_Frozen_Second != NULL )
			{
				pSeq_Enchant_Frozen_Second->ClearAllParticle();
				pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
			}
		}

		if( m_hSeqCold1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold1 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error01", 0, 0, 0 );
                pSeq_Cold1L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1  );
        }
		if( m_hSeqCold2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold2 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error02", 0, 0, 0 );
                pSeq_Cold2L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2  );
        }
		if( m_hSeqCold1_Right == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold1_Right = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error01", 0, 0, 0 );
                pSeq_Cold1R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1_Right  );
        }
		if( m_hSeqCold2_Right == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold2_Right = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error02", 0, 0, 0 );
                pSeq_Cold2R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2_Right  );
        }



		if( pSeq_Cold1L != NULL )
		{
			pSeq_Cold1L->SetEmitRate( 20,30 );
		}
		if( pSeq_Cold2L != NULL )
		{
			pSeq_Cold2L->SetEmitRate( 10,20 );
		}
		if( pSeq_Cold1R != NULL )
		{
			pSeq_Cold1R->SetEmitRate( 20,30 );
		}
		if( pSeq_Cold2R != NULL )
		{
			pSeq_Cold2R->SetEmitRate( 10,20 );
		}
		//if( pSeq_Cold_Head != NULL )
		//	pSeq_Cold_Head->SetEmitRate( 10, 20 );



		if ( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f )
		{
			if( m_hSeqCold_Head == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqCold_Head = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitCold", 0, 0, 0 );
                    pSeq_Cold_Head		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold_Head );
            }

			if( pSeq_Cold_Head != NULL )
				pSeq_Cold_Head->SetEmitRate( 10,20 );
		}
		else
		{
			if( pSeq_Cold_Head != NULL )
			{
				pSeq_Cold_Head->ClearAllParticle();
				pSeq_Cold_Head->SetEmitRate( 0,0 );
			}
		}


		m_ExtraDamagePack.m_Cold.m_fTime -= m_fElapsedTime;

		if( m_ExtraDamagePack.m_Cold.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Cold.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantFrozen.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantFrozen.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantFrozen.m_fTime = 0.0f;

	}
	else
	{
		if( true == m_bColdSlowApplied )
		{
			m_bColdSlowApplied = false;
		}

		if ( m_bFrozenSlowApplied == true )
		{
			m_bFrozenSlowApplied = false;

		}


		if( pSeq_Cold1L != NULL )
			pSeq_Cold1L->SetEmitRate( 0,0 );
		if( pSeq_Cold2L != NULL )
			pSeq_Cold2L->SetEmitRate( 0,0 );
		if( pSeq_Cold1R != NULL )
			pSeq_Cold1R->SetEmitRate( 0,0 );
		if( pSeq_Cold2R != NULL )
			pSeq_Cold2R->SetEmitRate( 0,0 );
		if( pSeq_Cold_Head != NULL )
		{
			pSeq_Cold_Head->ClearAllParticle();
			pSeq_Cold_Head->SetEmitRate( 0, 0 );
		}

		if( pSeq_Enchant_Frozen_Second != NULL )
		{
			pSeq_Enchant_Frozen_Second->ClearAllParticle();
			pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
		}
	}

	if( m_ExtraDamagePack.m_Aging.m_fTime > 0.f )
	{
		fAnimSpeedRate *= m_ExtraDamagePack.m_Aging.m_fAnimSpeedRate;
		fRunJumpSpeedRate *= m_ExtraDamagePack.m_Aging.m_fRunJumpRate;
	}


	if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantFrozen.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation = 0;
	}

	if( m_fGroggyTime > 0.f )
	{
		if( m_hSeqStun == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqStun = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitStun", 0,0,0 );
                pSeq_Stun 			= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqStun  );
        }

		if( NULL != pSeq_Stun )
			pSeq_Stun->SetEmitRate( 10, 20 );
	}
	else
	{
		if( NULL != pSeq_Stun )
		{
			pSeq_Stun->ClearAllParticle();
			pSeq_Stun->SetEmitRate( 0, 0 );
		}
	}




	if( m_fPepperRunTime > 0.f )
	{
		if( m_hSeqPepperBody1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqPepperBody1 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
                pSeq_Pepper1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody1  );
        }
		if( m_hSeqPepperBody2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqPepperBody2 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
                pSeq_Pepper2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody2  );
        }

		if( NULL != pSeq_Pepper1 )
			pSeq_Pepper1->SetEmitRate( 30, 40 );

		if( NULL != pSeq_Pepper2 )
			pSeq_Pepper2->SetEmitRate( 30, 40 );
	}
	else
	{
		if( NULL != pSeq_Pepper1 )
			pSeq_Pepper1->SetEmitRate( 0, 0 );

		if( NULL != pSeq_Pepper2 )
			pSeq_Pepper2->SetEmitRate( 0, 0 );
	}




	if( m_ExtraDamagePack.m_Lightning.m_fTime > 0.0f )
	{
		//�ٰŸ� �Ʊ��� ��´�
		D3DXVECTOR3 pos = GetPos();
		for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
			if( pUnit != NULL
				&& pUnit != this
				&& pUnit->IsLocalUnit() == true
				&& pUnit->GetTeam() == GetTeam()
				&& pUnit->GetInvincible() == false
				&& pUnit->GetNowHp() > 0.0f
				&& GetDistance( pos, pUnit->GetPos() ) < 200.0f )
			{



				float fBeforeHP = pUnit->GetNowHp();
				//pUnit->GetGageManager()->GetHPGage()->fNow -= m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime;
				float fNowHp = pUnit->GetNowHp() - m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime;
				SetNowHp( fNowHp );
				if( fBeforeHP > 0.0f && pUnit->GetNowHp() < 1.0f )
				{
					pUnit->SetNowHp( 1.f );
				}



				*m_fDamageHistory += (m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime);









			}
		}

		m_ExtraDamagePack.m_Lightning.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Lightning.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Lightning.m_fTime = 0.0f;
	}

	// mauntain : ����ȯ [2012.05.21] ���� 2�� ���� ����Ʈ ���� - ������ ȭ�� EDT Emoticon ����
	if ( false == GetStartOfDelayedFiringData().empty() )	//�ڽſ��� ���� ������ ȭ���� �ִٸ�, �̸�Ƽ�� ����
	{
		if ( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 20, 40 );
		}

		if ( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 20, 40 );
		}
	}
	else
	{
		if( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->ClearAllParticle();
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 0, 0 );
		}

		if( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->ClearAllParticle();
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 0, 0 );
		}
	}

	CommonFrameMoveStateAbnormalityStartOfDelayedFiring();

	if( m_ExtraDamagePack.m_StigmaDebuff.m_fTime > 0.f )
	{
		fRunJumpSpeedRate *= m_ExtraDamagePack.m_StigmaDebuff.m_fRunJumpRate;
	}


	// �̵��ӵ� ��ȭ ����
	if( false == IsSamef( m_fRunJumpSpeedRate, fRunJumpSpeedRate ) )
		m_fRunJumpSpeedRate = fRunJumpSpeedRate;

	if( g_pX2Game != NULL && g_pX2Game->GetLineMap() != NULL )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( m_FrameDataNow.syncData.lastTouchLineIndex );
		if( pLineData != NULL && pLineData->m_bUnderWater == true && GetPos().y <= pLineData->m_fWaterHeight)
		{
			ChangeUnderWater(true);
			//m_bUnderWaterHead = true;
		}
		else if( pLineData != NULL && pLineData->m_bForceChangeColor == true )
		{
			m_bForceChagneColor = true;
		}
		else
		{
			ChangeUnderWater(false);
			//m_bUnderWaterHead = false;
			m_bForceChagneColor = false;
		}

		if( pLineData != NULL && pLineData->m_bUnderWater == true && GetHeadBonePos().y <= pLineData->m_fWaterHeight )
		{
			float fMinY = min( pLineData->startPos.y, pLineData->endPos.y );
			float fColorRate = (pLineData->m_fWaterHeight - GetPos().y/*GetHeadBonePos().y*/) / (pLineData->m_fWaterHeight - fMinY);
			m_cLineUnitColor = D3DXCOLOR( pLineData->m_cLineColor.r * fColorRate, pLineData->m_cLineColor.g * fColorRate, pLineData->m_cLineColor.b * fColorRate, 0.f );

			fAnimSpeedRate *= pLineData->m_fAnimSpeed;
			m_fOxygenGage += pLineData->m_fVarianceOxyen * m_fElapsedTime;
			if( m_fOxygenGage < 0.f )
				m_fOxygenGage = 0.f;
			else if( m_fOxygenGage > MAGIC_OXYGEN_GAGE )
				m_fOxygenGage = MAGIC_OXYGEN_GAGE;

			if( m_fOxygenGage <= 0.f )
			{
				float fNowHP = GetNowHp() - GetMaxHp() * 0.05f * m_fElapsedTime;
				if( fNowHP <= 0.f )
				{
					// �ͻ�
					fNowHP = 0.f;
					m_ucDieReason = KEGS_USER_UNIT_DIE_REQ::UUDR_LACK_OF_OXYGEN;
				}
				SetNowHp( fNowHP );
			}
		}
		else
		{
			m_cLineUnitColor = D3DXCOLOR( 0.f, 0.f, 0.f, 0.f );

			if( m_fOxygenGage < MAGIC_OXYGEN_GAGE )
			{
				m_fOxygenGage += 40 * m_fElapsedTime;
				if( m_fOxygenGage > MAGIC_OXYGEN_GAGE )
					m_fOxygenGage = MAGIC_OXYGEN_GAGE;
			}
		}

		if( m_bForceChagneColor == true )
		{
			m_cLineUnitColor = D3DXCOLOR( pLineData->m_cLineColor.r, pLineData->m_cLineColor.g, pLineData->m_cLineColor.b, 0.f );
		}
	}

	//{{ JHKang / ������ / 2011/01/25 / �ִϸ��̼� �ӵ� ���� ������
	fAnimSpeedRate *= m_fAnimSpeedRateByNpc;
	//}} JHKang / ������ / 2011/01/25 / �ִϸ��̼� �ӵ� ���� ������

	// �ִϸ��̼� �ӵ� ��ȭ ����
	if( m_fAnimSpeedRate != fAnimSpeedRate )
	{
		m_fAnimSpeedRate = fAnimSpeedRate;
		ResetAnimSpeed( m_fAnimSpeedFuture, true, true );
		ResetAnimSpeed( m_fAnimSpeed, true, false );
	}

	/*
	//�ֵ�� ���� ����
	AccessStat().AccessAddOnTime().m_fAtkPhysic = AccessStat().AccessAddOnTime().m_fAtkPhysic - m_fElapsedTime;
	if( AccessStat().AccessAddOnTime().m_fAtkPhysic <= 0.0f )
	{
		AccessStat().AccessAddOnTime().m_fAtkPhysic = 0.0f;
		AccessStat().AccessAddOnStat().m_fAtkPhysic = 0.0f;
		if( NULL != pSeq_AtkPhysic )
		{
			pSeq_AtkPhysic->ClearAllParticle();
			pSeq_AtkPhysic->SetEmitRate( CMinMax<float>(0,0) );
		}
	}
	AccessStat().AccessAddOnTime().m_fAtkMagic = AccessStat().AccessAddOnTime().m_fAtkMagic - m_fElapsedTime;
	if( AccessStat().AccessAddOnTime().m_fAtkMagic <= 0.0f )
	{
		AccessStat().AccessAddOnTime().m_fAtkMagic = 0.0f;
		AccessStat().AccessAddOnStat().m_fAtkMagic = 0.0f;
		if( NULL != pSeq_AtkMagic )
		{
			pSeq_AtkMagic->ClearAllParticle();
			pSeq_AtkMagic->SetEmitRate( CMinMax<float>(0,0) );
		}
	}


	AccessStat().AccessAddOnTime().m_fDefPhysic = AccessStat().AccessAddOnTime().m_fDefPhysic - m_fElapsedTime;
	if( AccessStat().AccessAddOnTime().m_fDefPhysic <= 0.0f )
	{
		AccessStat().AccessAddOnTime().m_fDefPhysic = 0.0f;
		AccessStat().AccessAddOnStat().m_fDefPhysic = 0.0f;
		if( NULL != pSeq_DefPhysic )
		{
			pSeq_DefPhysic->ClearAllParticle();
			pSeq_DefPhysic->SetEmitRate( CMinMax<float>(0,0) );
		}
	}
	AccessStat().AccessAddOnTime().m_fDefMagic = AccessStat().AccessAddOnTime().m_fDefMagic - m_fElapsedTime;
	if( AccessStat().AccessAddOnTime().m_fDefMagic <= 0.0f )
	{
		AccessStat().AccessAddOnTime().m_fDefMagic = 0.0f;
		AccessStat().AccessAddOnStat().m_fDefMagic = 0.0f;
	}
	*/

	CommonFrameMove_Invincible();


	KTDXPROFILE_BEGIN( "LAND_CHECK" );
	m_FrameDataNow.unitCondition.landPosition = g_pX2Game->GetWorld()->GetLineMap()->GetLandPosition( m_FrameDataNow.syncData.position,
		LINE_RADIUS,
		&m_FrameDataNow.syncData.lastTouchLineIndex );


	if( fabs( m_FrameDataNow.syncData.position.y - m_FrameDataNow.unitCondition.landPosition.y ) < LINE_RADIUS )
		m_FrameDataNow.unitCondition.bFootOnLine = true;
	else
		m_FrameDataNow.unitCondition.bFootOnLine = false;

	if( m_fTriggerTime < 0.5f )
		m_fTriggerTime += m_fElapsedTime;

	if( g_pX2Game != NULL &&
		g_pX2Game->GetWorld() != NULL &&
		g_pX2Game->GetLineMap() != NULL &&
		IsMyUnit() == true &&
		m_fTriggerTime >= 0.5f )
		//m_FrameDataNow.unitCondition.bFootOnLine == true )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( m_FrameDataNow.syncData.lastTouchLineIndex );
		ASSERT( NULL != pLineData );
		if ( NULL != pLineData )
		{
			if( pLineData->m_bFootOnLine == false || (pLineData->m_bFootOnLine == true && m_FrameDataNow.unitCondition.bFootOnLine == true) )
			{
				m_fTriggerTime = 0.f;
				g_pX2Game->Handler_EGS_WORLD_TRIGGER_RELOCATION_REQ( pLineData->m_iTriggerId );
			}
		}
	}

	KTDXPROFILE_END();

	KTDXPROFILE_BEGIN( "RE_ATTACK" );
	if( m_DamageData.bReAttack == true )
	{
		for( CX2DamageManager::HitUnitVector::iterator iter = m_DamageData.hitUnitList.begin();
            iter != m_DamageData.hitUnitList.end(); )
		{
			CX2DamageManager::HitUnit* pHitUnit = &(*iter);
			pHitUnit->fRemainGap -= m_fElapsedTime;
			if( pHitUnit->fRemainGap <= 0.0f )
			{
                iter = EraseUnorderlyUsingSwap( m_DamageData.hitUnitList, iter );
			}
            else
            {
                ++iter;
            }
		}
	}
	KTDXPROFILE_END();

	KTDXPROFILE_BEGIN( "MATRIX_AND_GAGE" );
	m_DamageLightTime -= m_fElapsedTime;
	if( m_DamageLightTime < 0.0f )
		m_DamageLightTime = 0.0f;

	if(GetHold() == true)
	{
		m_FrameDataNow.syncData.position = m_vHold;
	}

	if (GetGrap() == true)
	{
		m_FrameDataNow.syncData.position = m_vGrap;
	}

	GetMatrix().Move( m_FrameDataNow.syncData.position );

	{
		if(m_fReverseGravitySpeedDeltaY == 0.f)
		{
			if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
				GetMatrix().RotateDegree( GetSaddleDegree() );
			else
				GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );
		}



	}



	KTDXPROFILE_END();
// #ifdef FIXED_EMBLEM_HIDE_WHEN_DIE ĳ���� �׸��� �ѷ��ִ� �κ�,
// m_pUnitShadow �� SetShowObject �� false �� �θ� �����
	KTDXPROFILE_BEGIN( "ETC_FRAME_MOVE" );
	if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH )
	{
		if( m_pUnitShadow != NULL
			)
		{
			m_pUnitShadow->SetShowObject( true );
			m_pUnitShadow->UpdatePosition( m_FrameDataNow.syncData.position, m_FrameDataNow.unitCondition.landPosition, m_FrameDataNow.unitCondition.dirDegree );
		}

		if( m_pAfterImage != NULL )
			m_pAfterImage->OnFrameMove( m_fTime, m_fElapsedTime );
	}
	else
	{
		if( m_pUnitShadow != NULL )
		{
			m_pUnitShadow->SetShowObject( false );
		}
	}
	KTDXPROFILE_END();




	KTDXPROFILE_BEGIN( "SET_COMPONENT_POS" );
	if( IsMyUnit() == false )
	{
		SetNowHp( m_FrameDataNow.syncData.fNowHP );
		SetNowMp( m_FrameDataNow.syncData.fNowMP );

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		SetFrameMoveCountNow( m_FrameDataNow.syncData.dwFrameMoveCount );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

		SetHitCount( m_FrameDataNow.syncData.ucHitCount );
		SetHittedCount( m_FrameDataNow.syncData.ucHittedCount );
		SetNumOfDeBuff( m_FrameDataNow.syncData.ucNumOfDeBuff );
	}

	D3DXVECTOR3 markerPos, vHeadPos, vTempPos;
	GetFramePos( &vHeadPos, m_pFrame_Bip01_Head );
	markerPos = GetPos();
	markerPos.y = vHeadPos.y;

	markerPos.y += 70.0f;
	vTempPos = markerPos;

	D3DXVECTOR3 vSize = D3DXVECTOR3(0.6f, 0.6f, 0.6f);
    if ( CKTDGParticleSystem::CParticle* pPart_Emblem_200 = g_pData->GetUIMajorParticle()->ValidateParticleHandle( m_hPart_Emblem_200 ) )
	{
		// �̺� ����ȭ ������ ���� ��ǥ�� ��ũ�� �Ⱥ��̰�
		if( true == GetAbsoluteInvisibility() )
		{
			pPart_Emblem_200->SetPos( D3DXVECTOR3( 0.f, -999999.f, 0.f ) );
		}
		else
		{
			pPart_Emblem_200->SetPos( vTempPos + D3DXVECTOR3( 0, 20, 0 ) );
		}
		markerPos.y += 60.f;

		vSize = D3DXVECTOR3(0.36f, 0.36f, 0.36f);
	}

    if ( CKTDGParticleSystem::CParticle* pHeadMarkerParticle = g_pX2Game->GetMajorParticle()->ValidateParticleHandle( m_hHeadMarkerParticle ) )
	{
//#ifdef TITLE_SYSTEM
		pHeadMarkerParticle->SetSize( vSize );
//#endif
		pHeadMarkerParticle->SetPos( markerPos );


		// �̺� ����ȭ ������ ���� ��ǥ�� ��ũ�� �Ⱥ��̰�
		if( true == GetAbsoluteInvisibility() )
		{
			pHeadMarkerParticle->SetPos( D3DXVECTOR3( 0.f, -999999.f, 0.f ) );
		}
	}

	markerPos.y += 100.0f;

	CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMaker = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
	if( NULL != pSeqHeadMaker )
	{
		pSeqHeadMaker->SetPosition( markerPos );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeqQuestion = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hQuestionMark );
	if( NULL != pSeqQuestion )
	{
		pSeqQuestion->SetPosition( markerPos );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeqExclamationMark = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hExclamationMark );
	if( NULL != pSeqExclamationMark )
	{
		pSeqExclamationMark->SetPosition( markerPos );
	}

	KTDXPROFILE_END();

	if( m_FrameDataNow.stateParam.bInvincible == true || IsSuperArmor() )
		m_FlyCount = 0;


	KTDXPROFILE_BEGIN( "HYPER" );

	if( GetNowHp() <= 0.0f )
	{
		if( pSeq_Cold_Head != NULL )
		{
			pSeq_Cold_Head->ClearAllParticle();
			pSeq_Cold_Head->SetEmitRate( 0,0 );
		}

		if( NULL !=	pSeq_Stun )
		{
			pSeq_Stun->ClearAllParticle();
			pSeq_Stun->SetEmitRate( 0, 0 );
		}


		if ( pSeq_Enchant_Stun != NULL )
		{
			pSeq_Enchant_Stun->ClearAllParticle();
			pSeq_Enchant_Stun->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Frozen_Second != NULL )
		{
			pSeq_Enchant_Frozen_Second->ClearAllParticle();
			pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Blaze_Second != NULL )
		{
			pSeq_Enchant_Blaze_Second->ClearAllParticle();
			pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Poison_Second != NULL )
		{
			pSeq_Enchant_Poison_Second->ClearAllParticle();
			pSeq_Enchant_Poison_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Stun_Second != NULL )
		{
			pSeq_Enchant_Stun_Second->ClearAllParticle();
			pSeq_Enchant_Stun_Second->SetEmitRate( 0, 0 );
		}

		//{{ kimhc // 2010.6.15 // �����̹� ��� Ȥ�� ������ġ �÷��� �� ų�� �� ������ ��ǻ�Ϳ����� ��Ȱ�� ���� �Ӽ���æƮ�� ������ �ʴ� ����
//}} kimhc // 2010.6.15 //



		//2008.11.03 �׾��� �� �����̻� ��ƼŬ ���� - ���¿�
		// �� �κ��� ���߿� function �ϳ� ���� �ڵ������� �� ����߰ڴ�

		m_ExtraDamagePack.Init();	// �ϴ� �迡 �����̻� ��ü�� �ʱ�ȭ

		if ( pSeq_Poison != NULL )
		{
			//pSeq_Poison->ClearAllParticle();
			pSeq_Poison->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Fire1 != NULL )
		{
			//pSeq_Fire1->ClearAllParticle();
			pSeq_Fire1->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Fire2 != NULL )
		{
			//pSeq_Fire2->ClearAllParticle();
			pSeq_Fire2->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Curse != NULL )
		{
			//pSeq_Curse->ClearAllParticle();
			pSeq_Curse->SetEmitRate( 0, 0 );
		}
		if ( pSeq_SlowLeft != NULL )
		{
			//pSeq_SlowLeft->ClearAllParticle();
			pSeq_SlowLeft->SetEmitRate( 0, 0 );
		}
		if ( pSeq_SlowRight != NULL )
		{
			//pSeq_SlowRight->ClearAllParticle();
			pSeq_SlowRight->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Cold1L != NULL )
		{
			//pSeq_Cold1L->ClearAllParticle();
			pSeq_Cold1L->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Cold2L != NULL )
		{
			//pSeq_Cold2L->ClearAllParticle();
			pSeq_Cold2L->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Cold1R != NULL )
		{
			//pSeq_Cold1R->ClearAllParticle();
			pSeq_Cold1R->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Cold2R != NULL )
		{
			//pSeq_Cold2R->ClearAllParticle();
			pSeq_Cold2R->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Pepper1 != NULL )
		{
			//pSeq_Pepper1->ClearAllParticle();
			pSeq_Pepper1->SetEmitRate( 0, 0 );
		}
		if ( pSeq_Pepper2 != NULL )
		{
			//pSeq_Pepper2->ClearAllParticle();
			pSeq_Pepper2->SetEmitRate( 0, 0 );
		}
		// mauntain : ����ȯ [2012.05.21] ���� 2�� ���� ����Ʈ ���� - ������ ��ȣź
		if ( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->ClearAllParticle();
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 0, 0 );
		}

		if ( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->ClearAllParticle();
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 0, 0 );
		}

	}
	KTDXPROFILE_END();



	for( UINT i=0; i<m_vecpWeapon.size(); ++i )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		pWeapon->CommonFrameMoveForUser( m_fTime, m_fElapsedTime );
	}
//#ifndef NOT_USE_DICE_ROLL
//	if( NULL != m_pDiceRoll )
//	{
//		m_pDiceRoll->OnFrameMove( m_fTime, m_fElapsedTime );
//	}
//#endif //NOT_USE_DICE_ROLL



	CommonFrameMove_TimedEvent();

	if( NULL != GetUnit() )
	{
		ProcessEnchantedWeaponEffectAtHand( GetUnit()->GetType() );
	}




	if(m_fSpecialItemBuff_Wind > 0.f)
	{
		m_fSpecialItemBuff_Wind -= m_fElapsedTime;
		if(m_fSpecialItemBuff_Wind <= 0.f)
		{
			StopSpecialItemBuffWind();
		}
	}

	if( m_fSummonNpcCoolTime > 0.f )
	{
		m_fSummonNpcCoolTime -= m_fElapsedTime;
		if( m_fSummonNpcCoolTime < 0.f )
			m_fSummonNpcCoolTime = 0.f;
	}

	if( GetNowHp() > 0.f )							//ü���� �������� �� ����
		//10�ʴ� HP ȸ�� ����
		SetHPChangeRatePerSecond( m_fElapsedTime );

	if( GetNowMp() < GetMaxMp() )					//���ǰ� �ִ�ġ�� �ƴ� �� ����
		//���� ȸ���� �߰� ����
		SetMPChangeRateValue( m_fElapsedTime );

	if( -1 != GetSummonMonsterCardData().GetSummonMonsterUID() )
	{
		float fSummonMonsterTime = GetSummonMonsterCardData().GetSummonMonsterTime();
		if( fSummonMonsterTime > 0.f)
			fSummonMonsterTime -= m_fElapsedTime;

		if( 0.f >=  fSummonMonsterTime )		//�����ð� �����
		{
			CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetSummonMonsterCardData().GetSummonMonsterUID() );
			if( NULL != pNPC )
			{
				pNPC->StateChange( pNPC->GetSummmonEndState() );			//��ȯ ���� State�� ���� ��ȯ
				//������ Ÿ�ֿ̹� 4���� ��ȯ ��ī�� ���ÿ� ���� �����ش�.
				std::vector<int> temp = AccessSummonMonsterCardData().GetSummonMonsterUIDVec();
				if( !temp.empty() )
				{
					for( size_t i = 0; i < temp.size(); ++i )
					{
						CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( temp[i] );
						if( NULL != pNPC )
							pNPC->StateChange( pNPC->GetSummmonEndState() );
					}
					AccessSummonMonsterCardData().ClearSummonMonsterUIDInVec();
				}

				if ( false == pNPC->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE0 ) &&
					false == pNPC->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE1 ) &&
					false == pNPC->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE2 ) )
					AccessSummonMonsterCardData().init();									//��ȯ ���� �ʱ�ȭ
			}
		}
		else
			AccessSummonMonsterCardData().SetSummonMonsterTime( fSummonMonsterTime );
	}

	if( true == m_bChangeWorldColor )
		ChangeWorldColorByHyperMode();

	if( false == IsSamef( -1.f, m_fShowSkillCutInTime ) )
		ShowActiveSkillCutInAndLightByScript( m_fShowSkillCutInTime, false );

	DoCustomFunctionByBuffInCommonFrameMove();

	DamageDataChangeProcess();
}



void CX2GUUser::CommonFrameMove_Invincible()
{
	KTDXPROFILE_BEGIN( "INVINCIBLE" );
	if( m_FrameDataNow.stateParam.invincibleTime.m_fX != -1.0f
		&& m_FrameDataNow.stateParam.invincibleTime.m_fY != -1.0f )
	{
		if( m_FrameDataNow.stateParam.invincibleTime.m_fX <= m_pXSkinAnim->GetNowAnimationTime()
			&& m_FrameDataNow.stateParam.invincibleTime.m_fY > m_pXSkinAnim->GetNowAnimationTime() )
			m_FrameDataNow.stateParam.bInvincible = true;
		else
			m_FrameDataNow.stateParam.bInvincible = false;
	}
	else if( m_fForceInvincibleTime <= 0.0f )	// FieldFix: else�� ���� ���� �߸� �Ȱ����� �ʳ�?? m_FrameDataNow.stateParam.invincibleTime
	{
		m_FrameDataNow.stateParam.bInvincible = false;
	}

	if( m_fShowInvincibleTime > 0.0f )
	{
		m_fShowInvincibleSwapTime -= m_fElapsedTime;
		if( m_fShowInvincibleSwapTime <= 0.0f )
		{
			m_fShowInvincibleSwapTime	= 0.05f;
			m_bShowInvincibleWhite		= !m_bShowInvincibleWhite;
		}

	}
	if( m_FrameDataNow.syncData.bFrameStop == false )
	{
		m_fShowInvincibleTime -= m_fElapsedTime;
		if( m_fShowInvincibleTime <= 0.0f )
			m_fShowInvincibleTime = 0.0f;

		m_fForceInvincibleTime -= m_fElapsedTime;
		if( m_fForceInvincibleTime <= 0.0f )
			m_fForceInvincibleTime = 0.0f;
	}

	if( m_fForceInvincibleTime > 0.0f )
	{
		m_FrameDataNow.stateParam.bInvincible = true;
	}
	KTDXPROFILE_END();
}

void CX2GUUser::CommonFrameMove_TimedEvent()
{
	KTDXPROFILE();

    {
        const D3DXVECTOR2& v2AfterImage = m_v2NowAfterImageTime;
	    if( v2AfterImage.x != -1.0f
		    && v2AfterImage.y != -1.0f )
	    {
		    if( v2AfterImage.x <= m_pXSkinAnim->GetNowAnimationTime()
			    && v2AfterImage.y > m_pXSkinAnim->GetNowAnimationTime() )
			    EnableAfterImage();
		    else
			    DisableAfterImage();
	    }
    }

	//���� üũ
    for( int i = 0; i < (int)m_vecAttackTime.size(); i++ )
	{
        D3DXVECTOR2 attackTime = m_vecAttackTime[i].m_v2AttackTime;
		if( m_pXSkinAnim->GetNowAnimationTime() > attackTime.x && m_pXSkinAnim->GetNowAnimationTime() < attackTime.y )
		{
			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				if( m_FrameDataNow.unitCondition.bHit == false && GetShowSmallGageAndName() )
				{
					D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( m_DamageData.impactPoint );
					projPos.y -= 50.0f;
					projPos.x += 50.0f;

		//{{ kimhc // 2010.12.21 // 2010-12-23 New Character CHUNG
					CKTDGParticleSystem::CParticleEventSequence* pParticle = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,
                        m_vecAttackTime[i].m_wstrProjSeqName.c_str(),
                        projPos.x, projPos.y, 0.0f,	100, 100, -1, 1 );
					if ( NULL != pParticle && false == m_FrameDataNow.syncData.bIsRight )
					{
						D3DXVECTOR3 vAddRotateRel = pParticle->GetAddRotateRel();
						// Z�� ȸ���� �ݴ�� ��Ŵ
						vAddRotateRel.z *= -1.0f;
						pParticle->SetAddRotateRel( vAddRotateRel );
					}
		//}} kimhc // 2010.12.21 //  2010-12-23 New Character CHUNG

				}
				m_FrameDataNow.unitCondition.bHit = true;
				m_FrameDataFuture.unitCondition.bHit = m_FrameDataNow.unitCondition.bHit;
			}
		}
	}

	if( IsMyUnit() == false )
	{
		for( int i = 0; i < (int)m_StopAllList.size(); i++ )
		{
			D3DXVECTOR3 stopTime = m_StopAllList[i];
            if( m_pXSkinAnim->EventTimerOneshot( stopTime.x ) == true )
			{
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					{
						g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
						g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
					} break;

				case CX2Game::GT_PVP:
					{
						g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
						g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
					} break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}
			}
		}
		for( int i = 0; i < (int)m_Stop2AllList.size(); i++ )
		{
			D3DXVECTOR3 stopTime = m_Stop2AllList[i];
            if( m_pXSkinAnim->EventTimerOneshot( stopTime.x ) == true )
			{
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					{
						g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
						g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
					} break;

				case CX2Game::GT_PVP:
					{
						g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
						g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
					} break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}
			}
		}
	}
	for( int i = 0; i < (int)m_StopOtherList.size(); i++ )
	{
		D3DXVECTOR2 stopTime = m_StopOtherList[i];
        if( m_pXSkinAnim->EventTimerOneshot( stopTime.x ) == true )
		{
			switch ( g_pX2Game->GetGameType() )
			{
			case CX2Game::GT_DUNGEON:
			case CX2Game::GT_BATTLE_FIELD:
				{
					g_pX2Game->StopOtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f );
					g_pX2Game->StopOtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 700.0f );
				} break;

			case CX2Game::GT_PVP:
				{
					g_pX2Game->StopOtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f );
					g_pX2Game->StopOtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f );
				} break;

			default:
				ASSERT( !L"wrong path" );
				break;
			}
		}
	}
	for( int i = 0; i < (int)m_Stop2OtherList.size(); i++ )
	{
		D3DXVECTOR2 stopTime = m_Stop2OtherList[i];
        if( m_pXSkinAnim->EventTimerOneshot( stopTime.x ) == true )
		{
			switch ( g_pX2Game->GetGameType() )
			{
			case CX2Game::GT_DUNGEON:
			case CX2Game::GT_BATTLE_FIELD:
				{
					g_pX2Game->Stop2OtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f );
					g_pX2Game->Stop2OtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 700.0f );
				} break;

			case CX2Game::GT_PVP:
				{
					g_pX2Game->Stop2OtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f );
					g_pX2Game->Stop2OtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f );
				} break;

			default:
				ASSERT( !L"wrong path" );
				break;
			}
		}
	}

	BOOST_TEST_FOREACH( const CX2GameUnit::SoundPlayData&, soundData, m_vecSoundPlayData )
	{
		float fTime = soundData.m_SoundPlayTime;
		int iRate	= soundData.m_SoundPlayRate;

		if(GetGrap() == true)
			continue;

        bool bSkinAnimEventTimer = m_pXSkinAnim->EventTimerOneshot( fTime );

		if ( NULL != m_ptrRidingPet && true == GetRidingOn() && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
            bSkinAnimEventTimer = m_ptrRidingPet->GetXSkinAnimPtr()->EventTimerOneshot( fTime );

        if( bSkinAnimEventTimer == true )
		{
			if( rand()%100 < iRate )
			{

				// �� ���ָ� ��� �ؾ� �� ������
				if( true == soundData.m_bOnlyIfMyUnit )
				{
					// ���� ��ü�� �� ���� ��ü�� ���� ���
					if( true == IsMyUnit() )
					{
						// 3D ���� �������� �ʱ�.
						// (�Ϻ� ���� �ʿ��� ���� ���尡 ������ �ʴ� ���� ������ ����)
						PlaySound( soundData.m_SoundPlayName.c_str(), false );
					}
				}
				else
				{
					// 3D ���� ����
					PlaySound( soundData.m_SoundPlayName.c_str() );
				}
			}
		}
	}

	CommonFrameMove_InsertBuffFactor();
}



void CX2GUUser::CommonFrameMove_Particle()
{
	KTDXPROFILE();


	D3DXVECTOR3 hpAndMpUpPos = GetBonePos( L"Bip01_Spine" );
	if ( m_hSeqHPUp != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHPUp );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( hpAndMpUpPos );
		}
		else
		{
			m_hSeqHPUp = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}

	if ( m_hSeqMPUp != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqMPUp );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( hpAndMpUpPos );
		}
		else
		{
			m_hSeqMPUp = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}




	D3DXVECTOR3 MPHoldEffPos( 0, 0, 0);
	GetFramePos( &MPHoldEffPos, m_pFrame_Bip01 );
	CKTDGParticleSystem::CParticleEventSequence* pSeqHoldEffCenter = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCenter );
	CKTDGParticleSystem::CParticleEventSequence* pSeqHoldEffCircle = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hHoldEffCircle );
	if( NULL != pSeqHoldEffCenter )
		pSeqHoldEffCenter->SetPosition( MPHoldEffPos );
	if( NULL != pSeqHoldEffCircle )
		pSeqHoldEffCircle->SetPosition( MPHoldEffPos );

	D3DXVECTOR3 vEffectPos = GetPos();
	vEffectPos.y += 110.f;
	for( SeqHitParticleTraceUnitContainer::iterator lit = m_listSeqHitParticleTraceUnit.begin() ; lit != m_listSeqHitParticleTraceUnit.end(); )
	{
		CKTDGParticleSystem::CParticleEventSequenceHandle hParticle = *lit;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( hParticle );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( vEffectPos );
			++lit;
		}
		else
		{
            lit = EraseUnorderlyUsingSwap( m_listSeqHitParticleTraceUnit, lit );
		}

	}

//#ifdef PVP_BOSS_COMBAT_TEST
//
//	if( INVALID_EFFECTSET_HANDLE == m_hEffectSetBossMark )
//	{
//		if( NULL != g_pX2Room )
//		{
//			CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( GetUnitUID() );
//			if( NULL != pSlotData )
//			{
//				if( true == pSlotData->GetIsBoss() )
//				{
//					m_hEffectSetBossMark = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_BossHeadMark_Test", (CX2GameUnit*) this );
//				}
//			}
//		}
//	}
//
//#endif PVP_BOSS_COMBAT_TEST


}











// enchant frozen �� �ƴϿ���~
/*virtual*/ void CX2GUUser::CommonFrameMoveStateAbnormalityFrozen()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
	{
		if ( this == g_pX2Game->GetMyUnit() )
			g_pX2Game->SetEnableAllKeyProcess( false );

		m_FrameDataNow.stateParam.bSuperArmor = true;
		m_FrameDataNow.stateParam.bSuperArmorNotRed = true;

		m_ExtraDamagePack.m_Frozen.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Frozen.m_fTime <= 0.0f )
		{
			m_ExtraDamagePack.m_Frozen.m_fTime = 0.0f;
			if ( this == g_pX2Game->GetMyUnit() )
			{
				g_pX2Game->SetEnableAllKeyProcess( true );
			}

			if( GetNowHp() > 0.f )
			{
				if( m_ExtraDamagePack.m_Frozen.m_bCured == true )
				{
					m_ExtraDamagePack.m_Frozen.m_bCured = false;
					if ( true == GetRidingOn() )
						StateChange( USI_RIDING_DAMAGE_FRONT, true );
					else
						StateChange( USI_DAMAGE_SMALL_FRONT, true );
				}
				else
				{
					if ( true == GetRidingOn() )
						StateChange( USI_RIDING_DAMAGE_FRONT, true );
					else
						StateChange( USI_DAMAGE_DOWN_FRONT, true );
				}
			}
		}
	}





	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
	{
		if( INVALID_EFFECTSET_HANDLE == m_hEffSetStateAbnormalityFrozen )
		{
			m_hEffSetStateAbnormalityFrozen = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_State_Abnormality_Frozen", this ); /*, NULL, false, -1.f, m_ExtraDamagePack.m_Frozen.m_fTime );*/
		}

		//if( INVALID_EFFECTSET_HANDLE != m_hEffSetStateAbnormalityFrozen )
		//{
		//	CX2EffectSet::EffectSetInstance* pEffInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( m_hEffSetStateAbnormalityFrozen );
		//	if( NULL != pEffInst )
		//	{
		//		pEffInst->m_fLifeTime = m_ExtraDamagePack.m_Frozen.m_fTime;
		//	}
		//
		//}
	}
	else
	{
		if( INVALID_EFFECTSET_HANDLE != m_hEffSetStateAbnormalityFrozen )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffSetStateAbnormalityFrozen );
		}
	}
}


//{{ lucidash : ����� [2010.1.4]
void CX2GUUser::CommonFrameMoveStateAbnormalityWaterHold()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_WaterHold.m_fTime > 0.f )
	{
		if ( this == g_pX2Game->GetMyUnit() )
			g_pX2Game->SetEnableAllKeyProcess( false );

		m_FrameDataNow.stateParam.bSuperArmor = true;
		m_FrameDataNow.stateParam.bSuperArmorNotRed = true;

		m_ExtraDamagePack.m_WaterHold.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_WaterHold.m_fTime <= 0.0f )
		{
			m_bDisableGravity = false;
			m_ExtraDamagePack.m_WaterHold.m_fTime = 0.0f;
			if ( this == g_pX2Game->GetMyUnit() )
			{
				g_pX2Game->SetEnableAllKeyProcess( true );
			}

			if( GetNowHp() > 0.f )
			{
				if ( IsOnSomethingFuture() == false )
					StateChange( USI_DAMAGE_AIR_FALL, true );
				else
					if ( true == GetRidingOn() )
						StateChange( USI_RIDING_DAMAGE_FRONT, true );
					else
						StateChange( USI_DAMAGE_DOWN_FRONT, true );
			}
		}
		else
		{

			if( GetNowHp() > 0.f )
			{
				m_bDisableGravity = true;
			}
			else
			{
				m_bDisableGravity = false;
			}


			 m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
			 m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
		//if ( IsOnSomethingFuture() == true )
//			{
//				m_FrameDataFuture.syncData.position.y				= 	m_FrameDataFuture.unitCondition.landPosition.y + 50.f;
//				m_FrameDataNow.syncData.position.y				= 	m_FrameDataFuture.unitCondition.landPosition.y + 50.f;
//			}
			if ( m_pXSkinAnim->EventTimer( 0.1f ) == true )
				AnimStop();
		}
	}


	if( m_ExtraDamagePack.m_WaterHold.m_fTime > 0.f )
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hSeqWaterHold1 )
		{
			m_hSeqWaterHold1 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Pa_LAGUZ_SPAB_BigBouble", 0,0,0 );
		}

		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqWaterHold1 )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWaterHold1 );
			//ASSERT( NULL != pSeq );
			if( NULL != pSeq )
			{

				pSeq->SetPosition( GetBonePos(L"Bip01_Spine") );
				pSeq->SetEmitRate( 20, 20 );
			}
			else
			{
				m_hSeqWaterHold1 = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}
	}
	else
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqWaterHold1 )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWaterHold1 );
			//ASSERT( NULL != pSeq );
			if( NULL != pSeq )
			{
				pSeq->SetEmitRate( 0, 0 );
				pSeq->ClearAllParticle();
			}
			else
			{
				m_hSeqWaterHold1 = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}
	}

	if( m_ExtraDamagePack.m_WaterHold.m_fTime > 0.f )
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hSeqWaterHold2 )
		{
			m_hSeqWaterHold2 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Pa_LAGUZ_SPAB_SmallBouble", 0,0,0 );
		}

		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqWaterHold2 )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWaterHold2 );
			//ASSERT( NULL != pSeq );
			if( NULL != pSeq )
			{
				pSeq->SetPosition( GetBonePos(L"Bip01_Spine") );
				pSeq->SetEmitRate( 20, 20 );
			}
			else
			{
				m_hSeqWaterHold2 = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}
	}
	else
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqWaterHold2 )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWaterHold2 );
			//ASSERT( NULL != pSeq );
			if( NULL != pSeq )
			{
				pSeq->SetEmitRate( 0, 0 );
				pSeq->ClearAllParticle();
			}
			else
			{
				m_hSeqWaterHold2 = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}
	}
}
//}} lucidash : ����� [2010.1.4]

void CX2GUUser::CommonFrameMoveStateAbnormalityFlashBang()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_FlashBang.m_fTime > 0.f )
	{
		m_ExtraDamagePack.m_FlashBang.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_FlashBang.m_fTime < 0.0f )
		{
			m_ExtraDamagePack.m_FlashBang.m_fTime = 0.0f;

			if( IsMyUnit() == true )
			{
				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWhiteOut );
				g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  L"WhiteOutExtraDamageEnd", D3DXVECTOR3(0,0,0) );
			}
			m_bWhiteOut = false;
		}
	}
}



//#ifdef HEAD_INVERSE_KINEMATICS_TEST
//void CX2GUUser::HeadIKFrameMove( double fTime, float fElapsedTime )
//{
//	KTDXPROFILE();
//
//	if( NULL == m_pFrame_Bip01_Head )
//		return;
//
//	if( m_pXSkinAnim == NULL )
//		return;
//
//	bool bReturnToZeroAngle = false;
//	D3DXVECTOR3 vLocalRotationAxis(1, 0, 0);
//
//	const float MAGIC_DELAY_TIME = 1.f;
//	if( true == m_bEnableHeadIK &&
//		m_FrameDataNow.unitCondition.fStateTime > MAGIC_DELAY_TIME )
//	{
//		float fEnemyDistance = 0.f;
//		CX2GameUnit* pEnemyUnit = g_pX2Game->GetFrontNearestEnemyUnit( (CX2GameUnit*)this, fEnemyDistance );
//
//		if( NULL != pEnemyUnit &&
//			fEnemyDistance < m_fHeadIKRadius )
//		{
//			const D3DXVECTOR3 LOCAL_HEAD_FORWARD( 0.f, 0.f, 1.f );
//			D3DXVECTOR3 vLookAt = pEnemyUnit->GetPos();
//			vLookAt.y += pEnemyUnit->GetUnitHeight(false) * 3.f / 4.f;
//
//			D3DXMATRIX matToLocal;
//			if( NULL == D3DXMatrixInverse( &matToLocal, NULL, &m_pFrame_Bip01_Head->combineMatrix ) )
//				return;
//
//
//			D3DXVECTOR3 vLocalLookAt;
//			D3DXVec3TransformCoord( &vLocalLookAt, &vLookAt, &matToLocal );
//			D3DXVec3Normalize( &vLocalLookAt, &vLocalLookAt );
//
//
//			D3DXVec3Cross( &vLocalRotationAxis, &LOCAL_HEAD_FORWARD, &vLocalLookAt );
//			D3DXVec3Normalize( &vLocalRotationAxis, &vLocalRotationAxis );
//
//			float fLocalRotationAngle = acosf( D3DXVec3Dot( &LOCAL_HEAD_FORWARD, &vLocalLookAt ) );
//			if( fLocalRotationAngle < -m_fHeadIKAngleMax )
//			{
//				fLocalRotationAngle = -m_fHeadIKAngleMax;
//			}
//			else if( fLocalRotationAngle > m_fHeadIKAngleMax )
//			{
//				fLocalRotationAngle = m_fHeadIKAngleMax;
//			}
//
//			if( fLocalRotationAngle < m_fHeadIKAngle )
//			{
//				m_fHeadIKAngle -= fElapsedTime * m_fHeadIKAngleChangeRate;
//				if( m_fHeadIKAngle < fLocalRotationAngle )
//				{
//					m_fHeadIKAngle = fLocalRotationAngle;
//				}
//			}
//			else if( fLocalRotationAngle > m_fHeadIKAngle )
//			{
//				m_fHeadIKAngle += fElapsedTime * m_fHeadIKAngleChangeRate;
//				if( m_fHeadIKAngle > fLocalRotationAngle )
//				{
//					m_fHeadIKAngle = fLocalRotationAngle;
//				}
//			}
//		}
//		else
//		{
//			bReturnToZeroAngle = true;
//		}
//	}
//	else if( 0.f != m_fHeadIKAngle )
//	{
//		bReturnToZeroAngle = true;
//	}
//
//
//	if( true == bReturnToZeroAngle )
//	{
//		if( m_fHeadIKAngle > 0.f )
//		{
//			m_fHeadIKAngle -= fElapsedTime * m_fHeadIKAngleChangeRate;
//			if( m_fHeadIKAngle < 0.f )
//			{
//				m_fHeadIKAngle = 0.f;
//			}
//		}
//		else if( m_fHeadIKAngle < 0.f )
//		{
//			m_fHeadIKAngle += fElapsedTime * m_fHeadIKAngleChangeRate;
//			if( m_fHeadIKAngle > -0.f )
//			{
//				m_fHeadIKAngle = 0.f;
//			}
//		}
//	}
//
//
//	if( 0.f != m_fHeadIKAngle )
//	{
//		D3DXMATRIX matRotation;
//		D3DXMatrixRotationAxis( &matRotation, &vLocalRotationAxis, m_fHeadIKAngle );
//		m_pFrame_Bip01_Head->combineMatrix = matRotation * m_pFrame_Bip01_Head->combineMatrix;
//
//		if( NULL != m_pFrame_Bip01_Head->pFrameFirstChild &&
//			NULL != m_pMotion )
//		{
//			CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameFirstChildClone = (CKTDXDeviceXSkinMesh::MultiAnimFrame*) m_pFrame_Bip01_Head->pFrameFirstChild;
//			CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameFirstChildSrc = m_pMotion->GetFrame( pFrameFirstChildClone->m_iIndex );
//			if( NULL != pFrameFirstChildSrc )
//			{
//				m_pXSkinAnim->UpdateAnimationFrame_Public( pFrameFirstChildSrc, &m_pFrame_Bip01_Head->combineMatrix, pFrameFirstChildClone );
//			}
//		}
//	}
//}
//
//#endif HEAD_INVERSE_KINEMATICS_TEST





float CX2GUUser::DoAimingTarget(float fDist, const D3DXVECTOR3& vCoefficient, bool &bIsTarget, D3DXVECTOR3 &vTargetPosition, D3DXVECTOR3 &vLandPos, bool bGap )
{
	bIsTarget = false;

	if( g_pX2Game == NULL )
		return 0.f;

	CX2GameUnit *pAimedUnit = NULL;
	float fMinDist = fDist * fDist;
	float fX = 0.f;
	float fY = 0.f;

	for(int i=0; i<g_pX2Game->GetUnitNum(); ++i)
	{
		// ������ ���� �����Ѵ�.
		CX2GameUnit *pUnit = g_pX2Game->GetUnit(i);
		if( pUnit != NULL && g_pX2Game->IsValidUnit( pUnit ) == true &&
			pUnit->GetTeam() != GetTeam() && pUnit->GetNowHp() > 0.f )
		{
			// npc��� basic type ���͸� Ÿ������ �� �ִ�.
			if( pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{
				CX2GUNPC *pNpc = (CX2GUNPC*)pUnit;
				if( pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC ||
					pNpc->GetInvincible() )
					continue;
			}

			D3DXVECTOR3 vTargetPos = pUnit->GetPos();
			D3DXVECTOR3 vMyPos = GetPos();
			const CKTDXCollision::CollisionDataListSet& AttackListSet = pUnit->GetCollisionListSet();
			bool    bInit = false;

			BOOST_FOREACH( const CKTDXCollision::CollisionDataList* collisionDataList, AttackListSet )
			{
				BOOST_FOREACH( CKTDXCollision::CollisionData* collisionData, *collisionDataList )
				{
					D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
					if ( bInit == false )
					{
						bInit = true;
						vTargetPos = vCollPos;
						continue;
					}

					float fDist = GetDistance( vMyPos, vCollPos );
					float fOldDist = GetDistance( vMyPos, vTargetPos );

					if ( fDist <= fOldDist )
						vTargetPos = vCollPos;
				}
			}

			if ( bInit == false )
			{
				vTargetPos = pUnit->GetPos();
			}

			D3DXVECTOR3 vDir = vTargetPos - vMyPos;
			float fDistTemp = D3DXVec3LengthSq( &vDir);
			if( fDistTemp > fMinDist)
				continue;

			float fDir = 0.1f;
			if( GetIsRight() == false )
				fDir = -0.1f;

			const CKTDGLineMap::LineData* pLineData =  g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );
			if( pLineData == NULL )
				continue;

			D3DXVECTOR3 vLineDir = pLineData->dirVector;
			D3DXVECTOR3 vRight = vTargetPos - (vMyPos + (vLineDir * fDir));
			bool bFront = false;
			if( D3DXVec3LengthSq( &vRight ) < fDistTemp )
			{
				bFront = true;
			}
			if( bFront == false )
				continue;

			vDir = pUnit->GetLandPos() - GetLandPos();
			vDir.y = 0.f;
			float fFixDir = ( pUnit->GetHeadBonePos().y - vTargetPos.y ) / 2.f;
			float fTempY = pUnit->GetHeadBonePos().y - fFixDir;
			float fLocalX = D3DXVec3LengthSq( &vDir );
			float fLocalY = static_cast<float>(pow( fTempY - vMyPos.y, 2 ));	// �㸮���� ���� ����� ���� ǥ�� ��
			float fLocalPosY = static_cast<float>(pow( vTargetPos.y - vMyPos.y, 2 ));	// �������� �������� �Ǵ��ϱ� ���� ��
			float fMaxLocalY = vCoefficient.x * fLocalX;
			float fMinLocalY = vCoefficient.y * fLocalX;

			if( bGap == true )
			{
				fMaxLocalY += 50.f;
				fMinLocalY -= 30.f;
			}

			if( fMaxLocalY < 0.f || fMinLocalY > 0.f )
				continue;

			// y�� ���̰�... ������ �� ��������.
			D3DXVECTOR3 v1 = vTargetPos - vMyPos;
			D3DXVECTOR3 v2 = vLineDir * fDir;
			v1.y = v2.y = 0.f;
			D3DXVec3Normalize( &v1, &v1 );
			D3DXVec3Normalize( &v2, &v2 );

			float fDot = acos(D3DXVec3Dot( &v1, &v2));
			if( fDot < -1.f * vCoefficient.z || fDot > vCoefficient.z )
				continue;



			if(  fTempY - vMyPos.y < 0.f )
				fLocalY *= -1.f;

			float fSignedValue = 1.f;
			if( vTargetPos.y < vMyPos.y )
			{
				fSignedValue *= -1.f;
			}

			if( fSignedValue * fLocalPosY <= fMaxLocalY && fSignedValue * fLocalPosY >= fMinLocalY )
			{
				fMinDist = fDistTemp;
				pAimedUnit = pUnit;
				fX = fLocalX;
				fY = fLocalY;
				vTargetPosition = vTargetPos;
			}
		}
	}

	if( pAimedUnit == NULL )
	{
		bIsTarget = false;
		return 0.f;
	}
	vLandPos = pAimedUnit->GetLandPos();
	bIsTarget = true;
	return atan( fY / fX );
}

void CX2GUUser::RotateBone( CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame, const D3DXVECTOR3& vRotationAxis, const float fRotationAngle )
{
	KTDXPROFILE();

	if( NULL == pFrame )
		return;

	if( NULL == m_pMotion )
		return;

	if( m_pXSkinAnim == NULL )
		return;

	if( 0.f == fRotationAngle )
		return;


	D3DXMATRIX matRotation;
	D3DXMatrixRotationAxis( &matRotation, &vRotationAxis, D3DX_PI / 180.f * fRotationAngle );
	pFrame->combineMatrix = matRotation * pFrame->combineMatrix;

	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameFirstChildClone = (CKTDXDeviceXSkinMesh::MultiAnimFrame*) pFrame->pFrameFirstChild;
	if( pFrameFirstChildClone != NULL )
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameFirstChildSrc = m_pMotion->FindFrame(pFrameFirstChildClone->Name);
		if( NULL != pFrameFirstChildSrc )
		{
			m_pXSkinAnim->UpdateAnimationFrame_Public( pFrameFirstChildSrc, &pFrame->combineMatrix, pFrameFirstChildClone );
		}
	}
}



void CX2GUUser::CommonCameraMove()
{
	KTDXPROFILE();

	// dynamic camera �ɼ��� �������� ���� �� �Լ��� ȣ��ȴ�.


	int iCameraIndex = m_FrameDataNow.stateParam.normalCamera;
	if( true == m_FrameDataNow.unitCondition.bHit )
	{
		iCameraIndex = m_FrameDataNow.stateParam.hitCamera;
	}


	g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = D3DXVECTOR3( 0, 0, 0 );


	if( m_LuaManager.BeginTable( "CAMERA", iCameraIndex ) == true )
	{
		if( 0.f > fPreWorldNear )
		{ // ���̳��� ī�޶� ���� �Ǹ� ĳ���Ͱ� ������� ������ �����ϱ� ���� , Near�� 10���� ����
			fPreWorldNear = g_pKTDXApp->GetDGManager()->GetNear();
			if( fPreWorldNear >= 50.f )
			{
				g_pKTDXApp->GetDGManager()->SetNear(10);

				g_pKTDXApp->GetDGManager()->SetProjection(
					g_pKTDXApp->GetDGManager()->GetNear(),
					g_pKTDXApp->GetDGManager()->GetFar(), true );
			}
			else
			{
				fPreWorldNear = -1.f;
			}
		}

		std::vector< float > vecSubCameraTiming;
		if( m_LuaManager.BeginTable( "SUB_CAMERA_TIMING" ) == true )
		{
			float fSubCameraTiming = 0.f;
			for( int i=1; ; i++ )
			{
				if( true == m_LuaManager.GetValue( i, fSubCameraTiming ) )
				{
					vecSubCameraTiming.push_back( fSubCameraTiming );
				}
				else
				{
					break;
				}
			}
			m_LuaManager.EndTable(); // SUB_CAMERA_TIMING
		}

		if( true == vecSubCameraTiming.empty() )
		{
			g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, 1 );
		}
		else
		{
			bool bDone = false;
			for( int i=0; i<(int)vecSubCameraTiming.size(); i++ )
			{
				float fTiming = vecSubCameraTiming[i];

				if( m_pXSkinAnim->GetNowAnimationTime() < fTiming )
				{
					char tableName[64] = "";
					StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", i );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
					bDone = true;
					break;
				}
			}

			if( false == bDone )
			{
				char tableName[64] = "";
				StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", (int)vecSubCameraTiming.size() );
				g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
			}
		}


		m_LuaManager.EndTable(); // CAMERA
	}
	else
	{
		if( 0.f < fPreWorldNear )
		{// ���̳��� ī�޶� ���� SetNear���� 10���� ���� �� ���¶�� ����
			g_pKTDXApp->GetDGManager()->SetNear(fPreWorldNear);
			fPreWorldNear = -1.f;

			g_pKTDXApp->GetDGManager()->SetProjection(
				g_pKTDXApp->GetDGManager()->GetNear(),
				g_pKTDXApp->GetDGManager()->GetFar(), true );
		}

		float eyeDistance = 0;






		const CKTDGLineMap::CameraData* pCameraData = g_pX2Game->GetWorld()->GetLineMap()->GetCameraData( m_FrameDataFuture.syncData.lastTouchLineIndex );
		if( NULL != pCameraData )
		{
			switch( pCameraData->m_eType )
			{
			case CKTDGLineMap::LCT_ABSOULTE_POSITION:
				{
					g_pX2Game->GetX2Camera()->GameLineCamera( this, pCameraData->m_vEye, pCameraData->m_vLookAt, pCameraData->m_bFocusUnit, 0.4f );
				} break;
			case CKTDGLineMap::LCT_RELATIE_POSITION:

				{
					g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = pCameraData->m_vRelativeEye;
					g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_fSpeed = pCameraData->m_fCameraRepositionSpeed;
					g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance(), 200, m_fAngleDegree, eyeDistance, 0.f, 0.3f );
				} break;

			default:
				{
					g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance(), 200, m_fAngleDegree, eyeDistance, 0.f, 0.3f );

				} break;
			}

		}
		else
		{
			if( m_GameCameraOffset.bEnable == true )
			{
				g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() + m_GameCameraOffset.fCameraDistance,
					200.f + m_GameCameraOffset.fHeight,
					m_fAngleDegree + m_GameCameraOffset.fAngleDegree,
					eyeDistance + m_GameCameraOffset.fEyeDistance,
					0.f + m_GameCameraOffset.fLookatDistance,
					0.3f + m_GameCameraOffset.fTrackingTime,
					0.0f + m_GameCameraOffset.fTrackingAtTime );

			}
			else
				g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance(), 200, m_fAngleDegree, eyeDistance, 0.f, 0.3f );
		}
	}
}






/*virtual*/ void CX2GUUser::DetonationEventProcess()
{
	if( true == GetRidingOn() )
		return;

	if( true == GetNoDetonation() )
		return;

	if( m_OrgInputData.pureZ == true || m_OrgInputData.pureX == true )
	{
		GetGageData()->SetCanChargeMpForDetonation( true );
	}
}

void CX2GUUser::CommonEventProcess()
{
	const int iSecretSkillStateId = m_ComboAndSkillChainSystem.GetTriggerSecretSkillState( this );
	if ( 0 != iSecretSkillStateId )
		StateChange( iSecretSkillStateId );

	SpecialAttackEventProcess( CX2SkillTree::ASUT_ANYTIME );		// �ƹ����� �� �� �ִ� �ʻ�⿡ ���� Ű �̺�Ʈ ó��


	if( m_InputData.oneSpace == true &&
		 true == GetUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB())
	{
		ToggleSkillSlotAB();
	}



	if( true == m_bSteppedJumpLinemap )
	{
		m_bSteppedJumpLinemap = false;


		if( m_fPepperRunTime > 0.f )
		{
			StateChange( USI_PEPPER_RUN_JUMP_UP );
		}
		else
		{
			/// ���� ���θ� ���� Ż�� ž�����̸� Ż�� ���� ������� ����
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_JUMP_UP );
			else
				StateChange( USI_JUMP_UP );
		}

		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
		m_PhysicParam.nowSpeed.x += m_vAddJumpSpeed.x;
		m_PhysicParam.nowSpeed.y = m_vAddJumpSpeed.y;
	}

	DetonationEventProcess();

	if( m_FrameDataFuture.syncData.fNowHP <= 0.0f && true == IsOnSomethingFuture() )
	{
		if( m_FrameDataNow.unitCondition.bAttackerFront == true )
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DIE );
			else
				 StateChange( USI_DIE_FRONT );
		}
		else
		{
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DIE );
			else
				StateChange( USI_DIE_BACK );
		}
	}
	else if( m_FrameDataFuture.syncData.fNowHP <= 0.0f && false == IsOnSomethingFuture() )
	{
		if ( true == GetRidingOn() )
		{
			StateChange( USI_JUMP_DOWN );
		}
	}
}

void CX2GUUser::CommonStateEndFuture()
{
	LUA_GET_VALUE( m_LuaManager, "TRANSITION", m_FrameDataFuture.unitCondition.bTransitionChange, true );

	bool bFlipDir = false;
	LUA_GET_VALUE( m_LuaManager, "FLIP_DIR_END", bFlipDir, false );
	if( bFlipDir == true )
	{
		m_FrameDataFuture.syncData.bIsRight			= !m_FrameDataFuture.syncData.bIsRight;
		m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
	}


	/// �ش� ������Ʈ������ ������ ����, ������Ʈ�� ������ ���� �������� ���ƺ�
	if ( true == m_bRightAtThisState )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_bRightAtThisState					= false;
	}



	//{{ kimhc // 2010.2.17 //
	m_bApplyMotionOffset = true;
	//}} kimhc // 2010.2.17 //

}


void CX2GUUser::CommonStateEnd()
{

	if ( false == m_bIsPlayAvatarEmotionSoundWithoutEmotion )
	{
		if( m_pAvatarEmotionSound != NULL )
		{
			m_pAvatarEmotionSound->Stop();
			SAFE_CLOSE( m_pAvatarEmotionSound );
		}
	}

	for( UINT i=0; i<m_vecpWeapon.size(); ++i )
	{
		m_vecpWeapon[i]->CommonStateEnd();
	}

	if( m_LuaManager.BeginTable( "ENABLE_ATTACK_BOX" ) == true )
	{
		int index = 1;
		wstring wname;
		while( m_LuaManager.GetValue( index, wname ) == true )
		{
			SetEnableAttackBox( wname.c_str(), false );

			index++;
		}

		m_LuaManager.EndTable();
	}

	if( m_LuaManager.BeginTable( "DISABLE_ATTACK_BOX" ) == true )
	{
		int index = 1;
		wstring wname;
		while( m_LuaManager.GetValue( index, wname ) == true )
		{
			SetEnableAttackBox( wname.c_str(), true );

			index++;
		}

		m_LuaManager.EndTable();
	}

	LUA_GET_VALUE( m_LuaManager, "TRANSITION", m_FrameDataNow.unitCondition.bTransitionChange, true );
	if ( NULL != g_pX2Game->GetWorld() )
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

	int stateStrID;
	if( m_LuaManager.GetValue( "STATE_STRING",	stateStrID ) == true )
	{
		g_pX2Game->SetStateString( L"" );
	}
	g_pX2Game->SetStateString( L"" );

	DeleteEffectSetOnStateEnd();

	//{{ kimhc // 2010.2.17 //
// #ifdef	APPLY_MOTION_OFFSET
// 	m_bApplyMotionOffset = true;
// #endif	APPLY_MOTION_OFFSET
	//}} kimhc // 2010.2.17 //


	// CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END ������ ���� ��, �ش� State �� ������ m_vecEffectSetToDeleteOnCustomState ������ ��� �����.
	bool bCustomStateDeleteEffectSetOnStateEnd = false;
	LUA_GET_VALUE( m_LuaManager, "CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END",	bCustomStateDeleteEffectSetOnStateEnd,	false );
	if(bCustomStateDeleteEffectSetOnStateEnd)
	{
		DeleteEffectSetOnCustomState();
	}

	m_OldStateId = m_NowStateData.stateID;




	DoCustomFunctionByBuffInCommonStateEnd();
}








void CX2GUUser::PhysicProcess()
{
	KTDXPROFILE();



//#ifdef TRANSFORMER_TEST
//
//	if( true == GetTransformed() &&
//		NULL != GetTransformer() )
//	{
//		m_FrameDataFuture.syncData.position = GetTransformer()->GetPos();
//		m_FrameDataFuture.syncData.lastTouchLineIndex = GetTransformer()->GetLastTouchLineIndex();
//
//
//		m_FrameDataFuture.unitCondition.dirDegree = GetTransformer()->GetRotateDegree();
//		m_FrameDataFuture.unitCondition.dirVector = GetTransformer()->GetDirVector();
//
//		m_FrameDataFuture.syncData.bIsRight = GetTransformer()->GetIsRight();
//
//		m_FrameDataFuture.unitCondition.bFootOnLine = GetTransformer()->GetUnitCondition().bFootOnLine;
//		//m_FrameDataFuture.unitCondition.bFootOnUnit = false;
//		m_FrameDataFuture.unitCondition.landPosition = GetTransformer()->GetLandPos();
//
//		return;
//	}
//
//#endif TRANSFORMER_TEST









	//���� �Ķ���� ���//////////////////////////////////////////////////////////
	D3DXVECTOR3 position;
	D3DXVECTOR3 positionOrg;
	D3DXVECTOR3 angleDegree;

	//���� ���� �޾ƿ���
	positionOrg = position = m_FrameDataFuture.syncData.position;
	angleDegree	= m_FrameDataFuture.unitCondition.dirDegree;


	if( g_pX2Game == NULL || g_pX2Game->GetWorld() == NULL )
	{
		ASSERT( !"user physic process world / linemap is NULL" );
		return;
	}


	CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	if( NULL == pLineMap )
	{
		ASSERT( !"user physic process linemap is NULL" );
		return;
	}



	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );

//{{ kimhc // 2010.7.7 // ���Ͱ� ������ ���θ�

	// ���Ͱ� ������ ���θ��ε� diable �� �����
	if ( pLineData != NULL && pLineData->m_bTempLineMap == true && pLineData->bEnable == false )
	{
		D3DXVECTOR3 outPos( 0.0f, 0.0f, 0.0f );
		int iLineIndex = -1;
		// �ٷ� ���� ���θ��� �ִ��� �˻�
        pLineMap->GetDownNearestWithBound( positionOrg, FLT_MAX, &outPos, &iLineIndex );
		pLineData = pLineMap->GetLineData( iLineIndex );

		// ������ ������ ������
		if ( pLineData != NULL )
		{
			m_FrameDataFuture.syncData.position.y	-= LINE_RADIUS * 1.5f;
			position	= m_FrameDataFuture.syncData.position;
			positionOrg = m_FrameDataFuture.syncData.position;
			angleDegree	= pLineData->dirDegree;

			m_FrameDataFuture.syncData.lastTouchLineIndex	= iLineIndex;
			m_FrameDataFuture.unitCondition.dirDegree		= pLineData->dirDegree;
			m_FrameDataFuture.unitCondition.bTempLineMap	= pLineData->m_bTempLineMap;
		}
	}

	bool bFindLine = false;
	if( pLineData == NULL ||
		( pLineData->bEnable == false && pLineData->m_bTempLineMap == false ) )
	{
		D3DXVECTOR3 outPos( 0.0f, 0.0f, 0.0f );
		int iLineIndex = -1;
		// �ٷ� ���� ���θ��� �ִ��� �˻�
        pLineMap->GetDownNearestWithBound( positionOrg, FLT_MAX, &outPos, &iLineIndex );
		pLineData = pLineMap->GetLineData( iLineIndex );

		// ������ ������ ������
		if ( pLineData != NULL )
		{
			m_FrameDataFuture.syncData.position.y	-= LINE_RADIUS * 1.5f;
			position	= m_FrameDataFuture.syncData.position;
			positionOrg = m_FrameDataFuture.syncData.position;
			angleDegree	= pLineData->dirDegree;

			m_FrameDataFuture.syncData.lastTouchLineIndex	= iLineIndex;
			m_FrameDataFuture.unitCondition.dirDegree		= pLineData->dirDegree;
			m_FrameDataFuture.unitCondition.bTempLineMap	= pLineData->m_bTempLineMap;
			bFindLine = true;
		}
	}
	if( pLineData == NULL ||
		( pLineData->bEnable == false && pLineData->m_bTempLineMap == false && bFindLine == false ) )
	{
		ErrorLog( XEM_ERROR121 );
		int lineIndex = pLineMap->GetNearestLine( positionOrg );

		if( lineIndex < 0 )
		{
			ErrorLog( XEM_ERROR122 );
		}
		pLineData = pLineMap->GetLineData( lineIndex );
		if( pLineData == NULL )
		{
			ErrorLog( XEM_ERROR123 );
			pLineData = pLineMap->GetAnyEnabledNormalLine();
			if( NULL == pLineData )
			{
				ErrorLogMsg( XEM_ERROR123, "line error" );
			}
		}

		if( NULL != pLineData )
		{
			positionOrg = position = m_FrameDataFuture.syncData.position = pLineData->startPos;
			m_FrameDataFuture.syncData.lastTouchLineIndex = lineIndex;
			angleDegree	= m_FrameDataFuture.unitCondition.dirDegree = pLineData->dirDegree;
			m_PhysicParam.nowSpeed.x = 0.0f;
			m_PhysicParam.nowSpeed.y = 0.0f;
		}
		else
		{
			//{{ dmlee 2008.11.8
			// ���� ������ �ɰ��ϴ�. �̷� ��Ȳ�� ����� �ȵǴµ�.
			// ���⿡ Ż���ϱ� �޴��� �ϳ� �߰��ع�����? �ϴ��� �� ���� ����ó��.
			ASSERT( !"user physic process linedata is NULL" );
			return;
			//}} dmlee 2008.11.8
		}
	}
//}} kimhc // 2010.7.7 // ���Ͱ� ������ ���θ�

	//X����
	bool positive;
	if( m_PhysicParam.nowSpeed.x >= 0.0f )
		positive = true;
	else
		positive = false;
	if( positive == true )
	{
		m_PhysicParam.nowSpeed.x -= m_PhysicParam.fReloadAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.x <= 0.0f )
			m_PhysicParam.nowSpeed.x = 0.0f;
	}
	else
	{
		m_PhysicParam.nowSpeed.x += m_PhysicParam.fReloadAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.x > 0.0f )
			m_PhysicParam.nowSpeed.x = 0.0f;
	}


	if( 0.f != m_fReverseGravitySpeedDeltaY )
	{

		if( m_PhysicParam.nowSpeed.y > 300.f )
		{
			m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
			if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed )
				m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
		}
		else if( m_PhysicParam.nowSpeed.y < -50.f )
		{
			float fScale = m_PhysicParam.nowSpeed.y / -50.f;
			m_PhysicParam.nowSpeed.y += m_fReverseGravitySpeedDeltaY * fScale;
		}
		else
		{
			m_PhysicParam.nowSpeed.y += m_fReverseGravitySpeedDeltaY;
		}

		if( m_PhysicParam.nowSpeed.y < -4000.f )
			m_PhysicParam.nowSpeed.y = -4000.f;

		if( m_PhysicParam.nowSpeed.y > 4000.f )
			m_PhysicParam.nowSpeed.y = 4000.f;



		if( m_FrameDataFuture.unitCondition.bFootOnLine == true )
		{
			position.y += LINE_RADIUS * 1.5f;
		}

		//{{ oasis907 : ����� [2011.6.29]
		if(m_vReverseGravityRotateSpeedDelta == D3DXVECTOR3(0.f, 0.f, 0.f))
		{
			m_vReverseGravityRotateSpeedDelta.x = 0.0f;
			m_vReverseGravityRotateSpeedDelta.y = (GetRandomFloat() * 4.f) - 2.f;
			m_vReverseGravityRotateSpeedDelta.z = 0.f;

			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_JUMP_DOWN );
			else
				StateChange( USI_JUMP_DOWN );
		}
		m_PhysicParam.nowSpeed.x = 0.f;

		D3DXVECTOR3 vAnimRotateDegree = m_pXSkinAnimFuture->GetMatrix().GetRotateDegree();
		vAnimRotateDegree.x += m_vReverseGravityRotateSpeedDelta.x;
		vAnimRotateDegree.y += m_vReverseGravityRotateSpeedDelta.y;
		vAnimRotateDegree.z += m_vReverseGravityRotateSpeedDelta.z;

		m_pXSkinAnimFuture->GetMatrix().RotateDegree( vAnimRotateDegree );


		m_pXSkinAnim->GetMatrix().RotateDegree( vAnimRotateDegree );



		D3DXVECTOR3 vRotateDegree = GetMatrix().GetRotateDegree();
		GetMatrix().RotateDegree(vRotateDegree);
		//}} oasis907 : ����� [2011.6.29]
	}
	else
	{
		m_vReverseGravityRotateSpeedDelta = D3DXVECTOR3(0.f, 0.f, 0.f);

		//Y����
		float fBuoyancy = 1.f;
		if( pLineData != NULL && pLineData->m_bUnderWater == true && GetPos().y <= pLineData->m_fWaterHeight )
		{
			fBuoyancy = pLineData->m_fBuoyancy;
		}


		//{{AFX
		if( m_FrameDataFuture.stateParam.bLandConnect == false &&
			m_bDisableGravity == false &&
			m_bDisableGravityInScript == false )
			//}}AFX

		{
			// m_bDisableGravity �̰� ���� ���°� Ȯ���Ǹ� ��� ����ҵ� ������ m_bRidingMachine ��κ��� �ӽ��� ~~
			if ( m_bRidingMachine == false  )
			{
				m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime * fBuoyancy;
				if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed * fBuoyancy )
					m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * fBuoyancy;

				m_PhysicParam.nowSpeed.y -= m_PhysicParam.fDownAccel * m_fElapsedTime;
				if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed * fBuoyancy )
					m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * fBuoyancy;
			}
		}
		else
		{
			if( m_bDisableGravity == false && m_bDisableGravityInScript == false )
			{
				if ( m_bRidingMachine == false )
				{
					m_PhysicParam.nowSpeed.y = 0.0f;
				}
			}
		}
	}



#if X2_DEV_TOOLS

#ifdef _DEBUG
	if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetPureKeyState(DIK_LSHIFT) == TRUE )
		{
			m_PhysicParam.nowSpeed.x *= 6.f;
		}
	}
#endif // _DEBUG

#endif // VILLAGE_FLASH_MAN_DEV_ONLY

	//X�̵�
	m_FrameDataFuture.unitCondition.dirVector = pLineData->dirVector;

	float fSlowFactor = 1.0f;
	if( pLineData != NULL && pLineData->m_bUnderWater == true && GetPos().y <= pLineData->m_fWaterHeight )
	{
		fSlowFactor *= pLineData->m_fResistance;
	}

	if( false == IsOnSomethingFuture() )
	{
		D3DXVECTOR3 dirVec = m_FrameDataFuture.unitCondition.dirVector;
		dirVec.y = 0.0f;

		if( m_FrameDataFuture.syncData.bIsRight == true )
			position += dirVec * m_PhysicParam.nowSpeed.x * fSlowFactor * m_fElapsedTime;
		else
			position -= dirVec * m_PhysicParam.nowSpeed.x * fSlowFactor * m_fElapsedTime;
	}
	else
	{
		if( m_FrameDataFuture.syncData.bIsRight == true )
			position += m_FrameDataFuture.unitCondition.dirVector * m_PhysicParam.nowSpeed.x * fSlowFactor * m_fElapsedTime;
		else
			position -= m_FrameDataFuture.unitCondition.dirVector * m_PhysicParam.nowSpeed.x * fSlowFactor * m_fElapsedTime;
	}

	//���� ���ǵ� ����
	m_FrameDataFuture.unitCondition.dirVector = pLineData->dirVector;
	if( true == m_FrameDataFuture.unitCondition.bFootOnLine )
	{
		position += pLineData->dirVector * pLineData->m_fSpeed * m_fElapsedTime;
	}



//#ifdef LINEMAP_SLOW_WIND_TEST
//
//	if( false == IsOnSomethingFuture() )
//	{
//		if( NULL != pLineData &&
//			0.f != pLineData->m_fWindRange )
//		{
//			float fDist = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
//			if( fDist < 0.f )
//				fDist = -fDist;
//
//			if( fDist < pLineData->m_fWindRange )
//			{
//				D3DXVECTOR3 vWindSpeed = pLineData->dirVector;
//				if( 0.f != vWindSpeed.y )
//				{
//					vWindSpeed.y = 0.f;
//					D3DXVec3Normalize( &vWindSpeed, &vWindSpeed );
//				}
//				vWindSpeed *= pLineData->m_vWindSpeed.x;
//				vWindSpeed.y = pLineData->m_vWindSpeed.y;
//
//				position += vWindSpeed * m_fElapsedTime;
//			}
//		}
//	}
//
//#endif LINEMAP_SLOW_WIND_TEST




	//��� �̵�
    if(
        m_bFutureApplyAnimMove == true

		//{{ kimhc // 2010.2.17 //
		&& m_bApplyMotionOffset == true
	//}} kimhc // 2010.2.17 //
		)
	{

		D3DXVECTOR3 vTranslationX = GetDirVector() * m_pXSkinAnimFuture->GetMotionOffset().x;
		D3DXVECTOR3 vTranslationZ = GetZVector() * m_pXSkinAnimFuture->GetMotionOffset().z;
		vTranslationX.y = 0.f;
		vTranslationZ.y = 0.f;

		if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			position += vTranslationX + vTranslationZ;
		}
		else
		{
			position -= vTranslationX + vTranslationZ;
		}
	}




//#ifdef STEP_ON_MONSTER_TEST
//	m_FrameDataFuture.unitCondition.bFootOnUnit = false;
//#endif STEP_ON_MONSTER_TEST

	//�ٸ� ĳ���Ϳ� ���� �и� Ȥ�� ����
	if( m_FrameDataFuture.stateParam.bCanPassUnit == false )
	{
        D3DXVECTOR2 vMyUnitMinMaxY( m_FrameDataFuture.syncData.position.y, m_FrameDataFuture.syncData.position.y + GetUnitHeight( true ) );
        D3DXVECTOR2 vMyUnitMinMaxYCur( m_FrameDataFuture.syncData.position.y, m_FrameDataFuture.syncData.position.y + GetUnitHeight( true ));


		const vector<CX2GameUnit*>& vecGameUnitList = g_pX2Game->GetUnitList();
		for( vector<CX2GameUnit*>::const_iterator cvItr = vecGameUnitList.begin();
            vecGameUnitList.end() != cvItr; ++cvItr )
		{
			CX2GameUnit* pGameUnit = *cvItr;
			if ( NULL != pGameUnit
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                && pGameUnit != this
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                )
			{
				if ( ( ( pGameUnit->GetTeam() != GetTeam() && false == pGameUnit->GetStateparam( true ).bCanPassUnit)
                || ( pGameUnit->GetTeam() == GetTeam() && pGameUnit->GetGameUnitType() == GUT_NPC
                && static_cast<CX2GUNPC*>( pGameUnit )->CanPassAllyUnit() == false )
				) && ( pGameUnit->GetAllyTeam() == CX2Room::TN_NONE || pGameUnit->GetAllyTeam() != static_cast<CX2Room::TEAM_NUM>(GetTeam()) ) && // ����� �Ʊ����� �����Ǿ����� �ʰų�, ����� �Ʊ����� �ƴϸ�
					pGameUnit->GetGameUnitState() != GUSI_DIE &&
					!( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && false == pGameUnit->DidReceiveFirstSyncPacket() ) )  /// ��Ʋ�ʵ忡�� ��ũ��Ŷ�� �ѹ��� �������� �ƴϸ�
				{
					// ���� �и� �� ����, ��밡 �и� �� �ִ� ���¶��
					if ( !( false == m_FrameDataFuture.stateParam.bCanPushUnit &&
						pGameUnit->GetStateparam( true ).bCanPushUnit ) )
                    {
                        vMyUnitMinMaxY.x = position.y;
                        vMyUnitMinMaxY.y = position.y + GetUnitHeight( true );
                        if ( pGameUnit->GetGameUnitType() == GUT_USER )
                        {
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                            int  iDirection = 0;
                            PhysicProcess_RetrieveRelativePosInfo( static_cast<CX2GUUser*>(pGameUnit), pGameUnit->GetUnitIndex(), iDirection );
                            if ( ProcessCanPushUnit( pGameUnit, vMyUnitMinMaxY, position, iDirection ) == false )
                            {
					            position = m_FrameDataFuture.syncData.position;
					            m_PhysicParam.nowSpeed.x = 0.0f;
                            }
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//                            if ( ProcessCanPushUserUnit( static_cast<CX2GUUser*>(pGameUnit), vMyUnitMinMaxY, vMyUnitMinMaxYCur, position ) == false )
//                            {
//					            position = m_FrameDataFuture.syncData.position;
//					            m_PhysicParam.nowSpeed.x = 0.0f;
//                            }
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                        }
                        else
                        {
                            CX2GUNPC* pNPC = static_cast<CX2GUNPC*>(pGameUnit);
                            int  iDirection = 0;
                            switch( pNPC->GetUserUnitRelativePos( GetUnitIndex() ) )
                            {
                            case 1:
                                iDirection = 2;
                                break;
                            case 2:
                                iDirection = 1;
                                break;
                            case 3:
                                iDirection = 4;
                                break;
                            case 4:
                                iDirection = 3;
                                break;
                            }
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                            if ( ProcessCanPushUnit( pGameUnit, vMyUnitMinMaxY, position, iDirection ) == false )
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//                            if ( ProcessCanPushNPCUnit( static_cast<CX2GUNPC*>(pGameUnit), vMyUnitMinMaxY, position, iDirection ) == false )
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                            {
					            position = m_FrameDataFuture.syncData.position;
					            m_PhysicParam.nowSpeed.x = 0.0f;
                            }
                        }
                        continue;
                    }

					/// ���� ����
                    if ( CheckPassUnit( pGameUnit, position ) == true )
                    {
					    position = m_FrameDataFuture.syncData.position;
					    m_PhysicParam.nowSpeed.x = 0.0f;
                    }
				}

			}
		}
	}

	//Y�̵�
	D3DXVECTOR3 vPos = ( true == GetRidingOn() && NULL != m_ptrRidingPet ) ? m_ptrRidingPet->GetPos() : m_FrameDataFuture.syncData.position;

	if( m_PhysicParam.nowSpeed.y > 0.f &&
		false == pLineMap->CanPassUp( vPos )
		)
	{
		position.y = m_FrameDataFuture.syncData.position.y;
		m_PhysicParam.nowSpeed.y = 0.f;
	}
	else
	{
		if( D3DXVECTOR3(0, 0, 0 ) == m_vDisplaceUnitPosition )
		{
			position.y += m_PhysicParam.nowSpeed.y * m_fElapsedTime;
		}
	}


	//// �ܺο��ο� ���� ĳ���� ��ġ��ȭ�� �ִٸ�
	//if( D3DXVECTOR3(0, 0, 0 ) != m_vFixedUnitPosition )
	//{
	//	m_PhysicParam.nowSpeed = D3DXVECTOR2( 0, 0 );
	//	m_PhysicParam.passiveSpeed = D3DXVECTOR2( -1, -1 );

	//	position = m_vFixedUnitPosition;
	//	ClearFixedUnitPosition();
	//	ClearDisplaceUnitPosition();
	//}
	//else
	if( D3DXVECTOR3( 0, 0, 0 ) != m_vDisplaceUnitPosition )
	{
		if( true == GetShowObject() &&
			GetNowHp() > 0.f )
		{

			ResetOnSomethingFuture();

			position += m_vDisplaceUnitPosition;

			float fDistSq = D3DXVec3LengthSq( &m_vDisplaceUnitPosition );
			if(fDistSq <= 81.f)
			{
                m_FrameDataFuture.stateParam.bCanPassUnit = m_bFutureCanPassUnit;
			}
		}
		//ClearFixedUnitPosition();
		ClearDisplaceUnitPosition();
	}




	// ���� �ε�������
	m_FrameDataFuture.unitCondition.bFootOnWall = false;
	bool bCanWallJump = false;
	if( true == pLineMap->CrossedWall( m_FrameDataFuture.syncData.position, position, bCanWallJump ) )
	{
		if( false == IsOnSomethingFuture() &&
			true == bCanWallJump )
		{
			m_FrameDataFuture.unitCondition.bFootOnWall = true;
		}

		position.x = positionOrg.x;
		position.z = positionOrg.z;
	}











	//���� ���////////////////////////////////////////////////////////////////////
	bool bTouch	= false;
	//m_FrameDataFuture.unitCondition.bFall	= false;
	int lastTouchLineIndex		= m_FrameDataFuture.syncData.lastTouchLineIndex;
	D3DXVECTOR3 landPosition	= m_FrameDataFuture.unitCondition.landPosition;

	if ( CX2Unit::UC_EVE_BATTLE_SERAPH == GetUnitClass() && CX2GUEve::ENSI_EBS_BOOST_DASH == m_NowStateData.stateID )
		bTouch = pLineMap->IsOnLine( position, 51.f, &landPosition, &lastTouchLineIndex );
// #ifdef RIDING_SYSTEM //JHKang
// 	else if ( GetRidingOn() )
// 		bTouch = pLineMap->IsOnLine( position, 51.f, &landPosition, &lastTouchLineIndex );
// #endif //RIDING_SYSTEM
	else
		bTouch = pLineMap->IsOnLine( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex );


	if( bTouch == true ) //�̵� ��ġ�� �� �����
	{
		if( m_PhysicParam.nowSpeed.y > 0.0f )
		{
			//��� ���̸� ���� ��ġ�� ���� ��ũ�Ѵ�.
			position.x = landPosition.x;
			position.z = landPosition.z;
		}
		else if( 0.f != m_vFutureIgnoreLineTime.y &&
			m_pXSkinAnimFuture->GetNowAnimationTime() >= m_vFutureIgnoreLineTime.x &&
			m_pXSkinAnimFuture->GetNowAnimationTime() < m_vFutureIgnoreLineTime.y )
		{
			position.x = landPosition.x;
			position.z = landPosition.z;
		}
		else
		{
			//�ϰ��̳� �������¸� ��ġ�� ���� ��ũ�Ѵ�.
			position = landPosition;

			//���� ������Ƿ� Y�ӵ��� ���ش�.
			m_PhysicParam.nowSpeed.y = 0.0f;
			m_PhysicParam.fDownAccel = 0.0f;
		}
	}
	else //�̵� ��ġ�� �� ���� �ƴ϶��
	{
		//���� �ݵ�� ���� ��� �־�� �ϴ� ��Ȳ�̶��
		if( m_FrameDataFuture.stateParam.bLandConnect == true
			&& pLineMap->IsOnLineConnect( position, &lastTouchLineIndex, LINE_RADIUS, &landPosition ) == true )

		{
			position = landPosition; //���� ��ġ�� ���� ��ũ�Ѵ�.

			//���� ������Ƿ� Y�ӵ��� ���ش�.
			m_PhysicParam.nowSpeed.y = 0.0f;
			m_PhysicParam.fDownAccel = 0.0f;
		}
		else //������ �ϰ��� ���� ���� �ʴ°��� ������ ��Ȳ�̶��
		{
			const CKTDGLineMap::LineData* pDwnLineData = pLineMap->GetLineData( lastTouchLineIndex );
			bool bMeOnStart = false;
			bool bMeOnEnd = false;

			if( pDwnLineData != NULL )
			{
				if( GetDistance(landPosition,pDwnLineData->startPos) < LINE_RADIUS && pDwnLineData->beforeLine == -2 )
					bMeOnStart = true;
				else if( GetDistance(landPosition,pDwnLineData->endPos) < LINE_RADIUS && pDwnLineData->nextLine == -2 )
					bMeOnEnd = true;

			//{{ kimhc // 2010.8.3 // WALL Ÿ���� ���θ� ó�� ����
			    if( pDwnLineData->lineType == CKTDGLineMap::LT_MONSTER_ROAD || pDwnLineData->lineType == CKTDGLineMap::LT_WALL )
			    //}} kimhc // 2010.8.3 // WALL Ÿ���� ���θ� ó�� ����
			    {
				    bMeOnStart = bMeOnEnd = false;
			    }
			}

			if( bMeOnStart == true )
			{
				if( position.y < landPosition.y )
					position.y = landPosition.y;

				//��� ����Ÿ ������ -2 ������ �հ� ������ ���󋚹��� ����, ���� �׽�Ʈ ������
				//position.x		= pDwnLineData->startPos.x;
				//position.z		= pDwnLineData->startPos.z;
				//landPosition.x	= pDwnLineData->startPos.x;
				//landPosition.z	= pDwnLineData->startPos.z;
				landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else if( bMeOnEnd == true )
			{
				if( position.y < landPosition.y )
					position.y = landPosition.y;

				//position.x		= pDwnLineData->endPos.x;
				//position.z		= pDwnLineData->endPos.z;
				//landPosition.x	= pDwnLineData->endPos.x;
				//landPosition.z	= pDwnLineData->endPos.z;
				landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else
			{
				bool bCanDown = pLineMap->CanDown( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, false, false, false );

				if( bCanDown == true ) //�Ʒ��ʿ� ������ �־ ������ ���� �ִٸ�
				{
					//���� ��ġ�� �Ʒ��� ���� ��ũ�Ѵ�.
					position.x = landPosition.x;
					position.z = landPosition.z;

					//if( m_PhysicParam.nowSpeed.y <= 0.0f )
					//{
					//	m_FrameDataFuture.unitCondition.bFall = true;
					//}

					if( position.y < landPosition.y )
						position.y = landPosition.y;
				}
				else //���̻� �Ʒ��ʿ� ������ ���� ������ ��Ȳ�̸�
				{


					//position = positionOrg;	//�̹� �������� �̵��� ĵ���Ѵ�.
					position.x = positionOrg.x;
					position.z = positionOrg.z;
					if( position.y < landPosition.y )
						position.y = landPosition.y;

                    if ( m_FrameDataFuture.unitCondition.bFootOnLine == true
                        && m_PhysicParam.nowSpeed.y < 0.f )
                    {
			            m_PhysicParam.nowSpeed.y = 0.0f;
			            m_PhysicParam.fDownAccel = 0.0f;
                    }
				}
			}
		}
	}


	if ( !m_vecNeverMovePtr.empty() && NULL != m_vecNeverMovePtr.back() )
	{
		D3DXVECTOR3 vPos = m_vecNeverMovePtr.back()->GetPos();
		if( m_vecNeverMovePtr.back()->GetGravitiApply())
		{
			position.x = vPos.x;
			position.z = vPos.z;
		}
		else
			position = vPos;
	}

	if( true == m_bEntangled )
	{
		//position = positionOrg;
		position = m_vEntangledPosition;
	}

	if( lastTouchLineIndex < 0 || pLineMap->GetLineData( lastTouchLineIndex )->bEnable == false )
	{
		lastTouchLineIndex = pLineMap->GetNearestLine( position, &landPosition );
	}


	m_FrameDataFuture.syncData.lastTouchLineIndex	= lastTouchLineIndex;
	m_FrameDataFuture.unitCondition.landPosition	= landPosition;


	//���������� ������ �´�
	pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );

	if( pLineData == NULL )
	{
		InitPosition( false, 0 );
		pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
	}
	else if( pLineData->bEnable == false )
	{
		int lineIndex = pLineMap->GetNearestLine( positionOrg );
		if( lineIndex != -1 )
		{
			pLineData = pLineMap->GetLineData( lineIndex );
			positionOrg = position = m_FrameDataFuture.syncData.position = pLineData->startPos;
			m_FrameDataFuture.syncData.lastTouchLineIndex = lineIndex;
			angleDegree	= m_FrameDataFuture.unitCondition.dirDegree = pLineData->dirDegree;
		}
		else
		{
			InitPosition( false, 0 );
		}
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}



	//��ũ�� ���� ������ �������ٸ� �ޱ� ������ �����.
	D3DXVECTOR3 dirDegree;
	INIT_VECTOR3( dirDegree, 0.0f, 0.0f, 0.0f );
	if( m_FrameDataFuture.syncData.bIsRight == true )
	{
		dirDegree.x = pLineData->dirDegree.x;
		dirDegree.y = pLineData->dirDegree.y;
		//angle.z = D3DXToRadian(lineData.dirDegree.z);
	}
	else
	{
		dirDegree.x = pLineData->dirDegree.x;
		dirDegree.y = pLineData->dirDegree.y + 180.0f;
		//angle.z = D3DXToRadian(-lineData.dirDegree.z);
	}









	//��ġ ������Ʈ////////////////////////////////////////////////////////////////
	m_FrameDataFuture.syncData.position			= position;
	m_FrameDataFuture.unitCondition.dirDegree	= dirDegree;

	const float MAGIC_LINE_TOUCH_BOUND = 1.f;
	if( fabs(m_FrameDataFuture.syncData.position.y-m_FrameDataFuture.unitCondition.landPosition.y) < MAGIC_LINE_TOUCH_BOUND &&
		m_PhysicParam.nowSpeed.y <= 0.f )
		m_FrameDataFuture.unitCondition.bFootOnLine = true;
	else
		m_FrameDataFuture.unitCondition.bFootOnLine = false;







	//PhysicProcess_FastUpwardWind(pLineMap);
	PhysicProcess_Jumping(pLineMap);
	PhysicProcess_Bungee(pLineMap);
	//PhysicProcess_Portal(pLineMap);

	if ( m_delegatePhysicProcessPortalByGameType )
		m_delegatePhysicProcessPortalByGameType( pLineMap );


}








//void CX2GUUser::PhysicProcess_FastUpwardWind( CKTDGLineMap* pLineMap )
//{
//#ifdef LINEMAP_FAST_WIND_TEST
//	if( true == m_bEnableUpsideWind )
//	{
//		if( GetNowHp() > 0.f )
//		{
//			const CKTDGLineMap::LineData* pLineData_Land = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
//			if( NULL != pLineData_Land &&
//				0.f != pLineData_Land->m_fUpsideWindAccelaration )
//			{
//				float fDist = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
//				if( fDist < 0.f )
//					fDist = -fDist;
//
//				float fSlope = -pLineData_Land->m_fUpsideWindAccelaration / pLineData_Land->m_fUpsideWindRange;
//				float fUpSideAccel = pLineData_Land->m_fUpsideWindAccelaration + fSlope * fDist;
//
//				if( fUpSideAccel > 0.f )
//				{
//					if( true == m_FrameDataFuture.unitCondition.bFootOnLine )
//					{
//						ResetOnSomethingFuture();
//
//						m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
//					}
//
//					m_PhysicParam.nowSpeed.y += fUpSideAccel * m_fElapsedTime;
//				}
//			}
//		}
//	}
//
//#endif LINEMAP_FAST_WIND_TEST
//}


void CX2GUUser::PhysicProcess_Jumping( CKTDGLineMap* pLineMap )
{
	if( true == IsOnSomethingFuture()  )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
		if( NULL != pLineData )
		{
			if ( 0.f < pLineData->m_fKeyStopTime )
				SetCanNotInputTime_LUA( pLineData->m_fKeyStopTime );

			switch( pLineData->lineType )
			{
			case CKTDGLineMap::LT_JUMP_UP_REL:
				{
					if( USI_DAMAGE_DOWN_FRONT	!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_DOWN_BACK	!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_AIR_FALL			!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_AIR_DOWN_LANDING	!= (int) m_NowStateData.stateID )
					{
						if( USI_JUMP_DOWN == (int) m_NowStateData.stateID &&
							true == m_InputData.pureDown &&
							true == pLineMap->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
						{
							m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.1f;
						}
						else
						{
							if ( CKTDGLineMap::LDO_RIGHT == pLineData->m_eDirection )
							{
								if ( false == GetIsRight() )
									m_FrameDataFuture.syncData.bIsRight = true;
							}
							else if ( CKTDGLineMap::LDO_LEFT == pLineData->m_eDirection )
							{
								if ( true == GetIsRight() )
									m_FrameDataFuture.syncData.bIsRight = false;
							}

							if ( 0.f < pLineData->m_fKeyStopTime )
								SetCanNotInputTime_LUA( pLineData->m_fKeyStopTime );

							m_bSteppedJumpLinemap = true;
							PlaySound( L"HighJump.ogg" );
							m_vAddJumpSpeed = pLineData->m_vJumpSpeed;
						}
					}
				} break;

			case CKTDGLineMap::LT_JUMP_UP_ABS:
				{
					if( USI_DAMAGE_DOWN_FRONT	!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_DOWN_BACK	!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_AIR_FALL			!= (int) m_NowStateData.stateID &&
						USI_DAMAGE_AIR_DOWN_LANDING	!= (int) m_NowStateData.stateID )
					{
						if( USI_JUMP_DOWN == (int) m_NowStateData.stateID &&
							true == m_InputData.pureDown &&
							true == pLineMap->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
						{
							m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.1f;
						}
						else
						{
							m_bSteppedJumpLinemap = true;

							PlaySound( L"HighJump.ogg" );


							if( true == m_FrameDataFuture.syncData.bIsRight )
							{
								m_vAddJumpSpeed = pLineData->m_vJumpSpeed;
							}
							else
							{
								m_vAddJumpSpeed = pLineData->m_vJumpSpeed;
								m_vAddJumpSpeed.x = -m_vAddJumpSpeed.x;
							}
						}
					}
				} break;
			}
		}
	}
}
void CX2GUUser::PhysicProcess_Bungee( CKTDGLineMap* pLineMap )
{
	const float MAGIC_REBIRTH_WAIT_TIME = 2.f;

	if( false == m_bWaitingForRebirthAfterBungee &&
		true == m_FrameDataFuture.unitCondition.bFootOnLine )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
		if( NULL != pLineData )
		{
			switch( pLineData->lineType )
			{
			case CKTDGLineMap::LT_BUNGEE:
				{
					m_bWaitingForRebirthAfterBungee = true;
					m_timerRebirthAfterBungee.restart();

					SetCanNotInputTime_ZXArrow( MAGIC_REBIRTH_WAIT_TIME );

				} break;
			}
		}
	}

	if( true == m_bWaitingForRebirthAfterBungee )
	{
		if( m_timerRebirthAfterBungee.elapsed() > MAGIC_REBIRTH_WAIT_TIME )
		{
			const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
			if( NULL != pLineData )
			{
				switch( pLineData->lineType )
				{
				case CKTDGLineMap::LT_BUNGEE:
					{
						m_bWaitingForRebirthAfterBungee = false;

						ClearRecieveSyncData();
						/// ���� ���θ� ���� Ż�� ž�����̸� Ż�� �ǰ� ������� ����
						if ( true == GetRidingOn() )
							StateChange( USI_RIDING_DAMAGE_FRONT, true );
						else
							StateChange( USI_DAMAGE_AIR_DOWN, true );

						// ���� ������ ���� 10%���� 15%�� ���� (��ȹ �̽¸��� ��û����)
						float fNowHP = GetNowHp() - GetMaxHp() * 0.15f;
						if( fNowHP <= 0.f )
							fNowHP = 1.f;
						SetNowHp( fNowHP );



						InitPosByLineData( pLineData->m_iTeleportLineIndex );

					} break;
				}
			}
		}
	}
}




void CX2GUUser::PhysicProcess_Portal( CKTDGLineMap* pLineMap )
{
	if( true == m_FrameDataFuture.unitCondition.bFootOnLine )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
		if( NULL != pLineData )
		{
			switch( pLineData->lineType )
			{
				//#note LT_POTAL �ٷ� ���� ��ġ �ؾ� ��. �ǵ������� break ���� ����.
				// default ������ LT_NORMAL ���� �߰��Ǿ�� ��
			case CKTDGLineMap::LT_NORMAL:
				{
					m_bEnterPortal = false;
					m_bLeavePortal = true;

					if( false == pLineData->m_bIsPortalWhenActiveRaid || // ��Ż�� ����Ǵ� ���θ��� �ƴϰų�
						false == CX2BossRaidManager::GetInstance()->IsActiveRaidPortal() )// ���� ����Ǵ� ������ �ƴ϶��
						break;
				}
			case CKTDGLineMap::LT_POTAL:
				{
					if( true == m_bLeavePortal )
					{
						if( false == m_bEnterPortal )
						{
							m_bEnterPortal = true;
							m_timerInPortal.restart();

							CreateTeleportEffect( ( pLineData->startPos + pLineData->endPos ) * 0.5f, pLineData->dirDegree );
						}
						else
						{
							if( m_timerInPortal.elapsed() > 0.7 )
							{
								m_bLeavePortal = false;
								m_timerInPortal.restart();

								SetCanNotInputTime_ZXArrow( 0.3f );

								ClearRecieveSyncData();
								InitPosByLineData( pLineData->m_iTeleportLineIndex );


								pLineData = pLineMap->GetLineData( pLineData->m_iTeleportLineIndex );
								if( NULL != pLineData )
								{
									CreateTeleportEffect( ( pLineData->startPos + pLineData->endPos ) * 0.5f, pLineData->dirDegree );
								}
								else
								{
									CreateTeleportEffect( GetPos(), GetRotateDegree() );
								}
							}
						}
					}
				} break;

			default:
				{
					// #NOTE default ������ LT_NORMAL ���� �߰��Ǿ�� ��
					m_bEnterPortal = false;
					m_bLeavePortal = true;

				} break;
			}
		}
	}

}

void CX2GUUser::PhysicProcess_PortalInBattleField( CKTDGLineMap* pLineMap )
{
	if( true == m_FrameDataFuture.unitCondition.bFootOnLine )
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_FrameDataFuture.syncData.lastTouchLineIndex );
		if( NULL != pLineData )
		{
			switch( pLineData->lineType )
			{
				//#note LT_POTAL �ٷ� ���� ��ġ �ؾ� ��. �ǵ������� break ���� ����.
				// default ������ LT_NORMAL ���� �߰��Ǿ�� ��
			case CKTDGLineMap::LT_NORMAL:
				{
					m_bEnterPortal = false;
					m_bLeavePortal = true;

					if( false == pLineData->m_bIsPortalWhenActiveRaid || // ��Ż�� ����Ǵ� ���θ��� �ƴϰų�
						false == CX2BossRaidManager::GetInstance()->IsActiveRaidPortal() )// ���� ����Ǵ� ������ �ƴ϶��
						break;
				}
			case CKTDGLineMap::LT_POTAL:
				{
					if( true == m_bLeavePortal )
					{
						if( false == m_bEnterPortal )
						{
							m_bEnterPortal = true;
							m_timerInPortal.restart();
						}
						else
						{
							if( m_timerInPortal.elapsed() > 2.3f )
							{
								m_bLeavePortal = false;
								m_timerInPortal.restart();


								ClearRecieveSyncData();

								pLineData = pLineMap->GetLineData( pLineData->m_iTeleportLineIndex );
								if( NULL != pLineData )
								{
									CreateTeleportEffect( ( pLineData->startPos + pLineData->endPos ) * 0.5f, pLineData->dirDegree );
								}
								else
								{
									CreateTeleportEffect( GetPos(), GetRotateDegree() );
								}
							}
						}
					}
				} break;

			default:
				{
					m_bEnterPortal = false;
					m_bLeavePortal = true;

				} break;
			}
		}
	}

}

bool CX2GUUser::CommonSpecialAttackEventProcess( IN const CX2UserSkillTree& cUserSkillTree_, OUT const CX2UserSkillTree::SkillSlotData* & pSkillSlotDataPressed_, OUT int& iSkillSlotIndexPressed_ )
{
	//������ // 2012-04-30 // ��ų���� ü���� ���� Ȯ�� ��ų ��� �� �� �ֵ��� ���� // iSkillSlotIndexPressed_���� 0~7�� ���� ������ ����
	// ���� ���ʷ���Ʈ ����Ȧ�� Ÿ���� �ް� �ִ� ���¶��
	if( true == IsThereEffectiveBlackHoleDamageEffect() )
	{
		return false;
	}

	// ���� ����� ������ ������� �������̸�
	if( true == m_bSpecialAttackEventProcessedAtThisFrame )
		return false;

	if( true == IsCanNotIntput() ) // Ű �Է� �Ұ� ���� �˻�
		return false;

	const bool bSkillSlotB = false;

	if( true == m_InputData.oneA )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 0, bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 0;
		}
		return true;
	}
	if( true == m_InputData.oneS )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 1, bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 1;
		}
		return true;
	}
	if( true == m_InputData.oneD )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 2, bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 2;
		}
		return true;
	}
	if( true == m_InputData.oneC )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 3, bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 3;
		}
		return true;
	}
//{{������ // 2012-04-30 // ��ų���� ü���� ���� Ȯ�� ��ų ��� �� �� �ֵ��� ����
	//��ų���� Ȯ�� ���°� �ƴ϶�� Ȯ�� ��ų ��� �� �� ������ ����
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() &&
	    false == g_pData->GetUIManager()->GetUISkillTree()->IsExpandSkillSlot() )
		return true;

	if( true == m_InputData.oneQ )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 0, !bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 4;
		}
		return true;
	}
	if( true == m_InputData.oneW )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 1, !bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 5;
		}
		return true;
	}
	if( true == m_InputData.oneE )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 2, !bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 6;
		}
		return true;
	}
	if( true == m_InputData.oneR )
	{
		pSkillSlotDataPressed_ = cUserSkillTree_.GetSkillSlot( 3, !bSkillSlotB );
		if( NULL != pSkillSlotDataPressed_ &&
			CX2SkillTree::SI_NONE != pSkillSlotDataPressed_->m_eID )
		{
			iSkillSlotIndexPressed_ = 7;
		}
		return true;
	}
//}}������ // 2012-04-30 // ��ų���� ü���� ���� Ȯ�� ��ų ��� �� �� �ֵ��� ����
	return true;
}

/** @function : CX2GetActualMPConsume
	@brief : ����� ��ų�� MP �Ҹ��� ����
	@param : ����� ��ų�� ID, ����� ��ų�� ����
	@return : �Ҹ�� MP ��ġ
*/
/*virtual*/ float CX2GUUser::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID_ );

	if( NULL == pSkillTemplet )
	{
		const float MAGIC_ERROR_MP_COST = 100.f;
		return MAGIC_ERROR_MP_COST;
	}
	else
	{
		if ( NULL == GetUnit()  )
			return 0.f;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		return pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel );
	}
}

/** @function : GetActualCoolTime
	@brief : ����� ��ų�� ��Ÿ���� ����
	@param : ����� ��ų�� ID, ����� ��ų�� ����
	@return : ��Ÿ�� ��ġ
*/
/*virtual*/ float CX2GUUser::GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const
{
	if( NULL == pSkillTemplet_ )
		return 0.f;

	const float fSkillCoolTime = pSkillTemplet_->GetSkillCoolTimeValue( iSkillLevel );

	return max( 0.f, fSkillCoolTime );
}

/** @function : CX2GetActualCBConsume
	@brief : ����� ��ų�� CB �Ҹ��� ����
	@param : ����� ��ų�� ID, ����� ��ų�� ����
	@return : �Ҹ�� CB ��ġ
*/
/*virtual*/ USHORT CX2GUUser::GetActualCBConsume( CX2SkillTree::SKILL_ID eSkill_ID_, int iSkillLevel_ )
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkill_ID_ );

	if( NULL == pSkillTemplet )
	{
		const USHORT MAGIC_ERROR_CB_COST = 1;
		return MAGIC_ERROR_CB_COST;
	}

	USHORT usCBConsume = pSkillTemplet->m_usCBConsumption;
	switch( eSkill_ID_ )
	{
		case CX2SkillTree::SI_A_CDC_RUMBLE_SHOT:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO13 ) == true )
			{
				usCBConsume = 2;
			}
		}
	default:
		break;
	}

	if( usCBConsume < 0 )
		usCBConsume = 0;

	return usCBConsume;
}

bool CX2GUUser::EventTimer( float fTime, bool bFuture )
{
	KTDXPROFILE();
	if( bFuture == true )
	{
		if( m_FrameDataFuture.unitCondition.fStateTimeBack < fTime
			&& m_FrameDataFuture.unitCondition.fStateTime >= fTime )
		{
            return  true;
		}
		else
			return false;
	}
	else
	{
		if( m_FrameDataNow.unitCondition.fStateTimeBack < fTime
			&& m_FrameDataNow.unitCondition.fStateTime >= fTime )
		{
            return  true;
		}
		else
			return false;
	}
}


//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
void CX2GUUser::PushFrameData( bool bSendForce )
{
	KTDXPROFILE();

	if( true == IsAvaliableActionDuringDamageReact(m_FrameDataFuture.syncData.nowAction ))
		bSendForce = true;

	m_SendSyncDataList.push_back( m_FrameDataFuture.syncData );
	if( bSendForce == true || (int)m_SendSyncDataList.size() >= m_FrameBufferNum )
	{
		//��Ʈ�� ����
		SendFrameData();
		m_SendSyncDataList.resize(0);
	}
}
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//void CX2GUUser::PushFrameData()
//{
//	KTDXPROFILE();
//
//	m_SendSyncDataList.push_back( m_FrameDataFuture.syncData );
//	if( (int)m_SendSyncDataList.size() >= m_FrameBufferNum )
//	{
//		//��Ʈ�� ����
//		SendFrameData();
//		m_SendSyncDataList.resize(0);
//	}
//}
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
bool CX2GUUser::PopFrameData( bool bPopOnce, OUT bool& bFrameBufferPass )
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//void CX2GUUser::PopFrameData()
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
{
	KTDXPROFILE();
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
    bool    bPopAgain = false;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

	//���ú� ����Ʈ�� ����Ÿ�� ���� �ִٸ�
	if( (int)m_ReceiveSyncDataList.size() > 0 )
	{
		m_FrameDataNowBefore						= m_FrameDataNow;
		m_FrameDataNow.syncData						= m_ReceiveSyncDataList.front();

		if(GetHold() == true)
		{
			m_FrameDataNow.syncData.position = m_vHold;
		}

		if (GetGrap() == true )
		{
			m_FrameDataNow.syncData.position = m_vGrap;
		}

		GetMatrix().Move( m_FrameDataNow.syncData.position );

		//�����̼� ���� ������Ʈ
		const CKTDGLineMap::LineData* pLineData = NULL;
		if( g_pX2Game != NULL && g_pX2Game->GetWorld() != NULL && g_pX2Game->GetWorld()->GetLineMap() != NULL )
			pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( m_FrameDataNow.syncData.lastTouchLineIndex );

		if ( pLineData != NULL )
		{
			m_FrameDataNow.unitCondition.dirVector		= pLineData->dirVector;
			m_FrameDataNow.unitCondition.dirDegree		= pLineData->dirDegree;
			m_FrameDataNow.unitCondition.dirDegree.z	= 0.0f;

			if( m_FrameDataNow.syncData.bIsRight == false )
			{
				m_FrameDataNow.unitCondition.dirDegree.y += 180.0f;
			}




			{
				if(m_fReverseGravitySpeedDeltaY == 0.f)
				{
					if ( true == GetRidingOn() && GetNowStateID() != USI_RIDING_DIE )
						GetMatrix().RotateDegree( GetSaddleDegree() );
					else
						GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );
				}


					//GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );

			}
		}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	    //�� ������ �ƴ� ��쿡�� ������.
	    if ( false == IsMyUnit() )
	    {
			//�ʱ��� FrameMoveCount �����̶��...
			if( m_dwLastRecvFrameMoveCount >= m_adwInitFrameMoveCount[0] && m_dwLastRecvFrameMoveCount <= m_adwInitFrameMoveCount[1] )
				m_dwLastRecvFrameMoveCount = 0;

		    if( m_dwLastRecvFrameMoveCount != 0 )
		    {
			    if( m_FrameDataNow.syncData.dwFrameMoveCount > m_dwLastRecvFrameMoveCount )
                {
        		    KOGGamePerformanceCheck::GetInstance()->GetUDPTransCheckMgr()->UpdateUserSyncPacketLossRate( m_FrameDataNow.syncData.dwFrameMoveCount - m_dwLastRecvFrameMoveCount, 1 );
                }
		    }

		    m_dwLastRecvFrameMoveCount = m_FrameDataNow.syncData.dwFrameMoveCount;
	    }
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

		m_ReceiveSyncDataList.erase( m_ReceiveSyncDataList.begin() );
        bFrameBufferPass = false;

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        if( (int)m_ReceiveSyncDataList.size() > m_FrameBufferNum * 2 )
        {
			bPopAgain = true;
        }
        else
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
		if( (int)m_ReceiveSyncDataList.size() > m_FrameBufferNum + m_AddFrameBuffer )
		{
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
            if ( m_FrameDataNow.syncData.nowState == USI_WAIT
                && m_ReceiveSyncDataList.front().nowState == USI_WAIT
                || bPopOnce == false )
                bPopAgain = true;
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//			m_bPopAgain = true;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
			/*
			m_fFrameSkipTime += m_fElapsedTime;
			if( m_fFrameSkipTime > m_fElapsedTime * 2.0f )
			{
			m_FrameSkipNum++;
			if( m_FrameSkipNum > 10 )
			{
			m_AddFrameBuffer--;
			if( m_AddFrameBuffer < 0 )
			m_AddFrameBuffer = 0;
			m_FrameSkipNum = 0;
			}
			}
			*/
		}
		else
		{
//#ifndef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//			m_bPopAgain = false;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
			//m_fFrameSkipTime = 0.0f;
		}

		//m_fFrameWaitTime = 0.0f;
	}
	else
	{
        bFrameBufferPass = true;
		/*
		m_fFrameWaitTime += m_fElapsedTime;
		if( m_fFrameWaitTime > m_fElapsedTime * 2.0f )
		{
		m_FrameWaitNum++;
		if( m_FrameWaitNum > 10 )
		{
		m_AddFrameBuffer++;
		if( m_AddFrameBuffer > m_FrameBufferNum )
		m_AddFrameBuffer = m_FrameBufferNum;
		m_FrameWaitNum = 0;
		}
		}

		m_fFrameSkipTime = 0.0f;
		*/
	}
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
    return  bPopAgain;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
}



void CX2GUUser::SendFrameData()
{
	KTDXPROFILE();

	if( NULL == g_pX2Game )
		return;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( m_SendSyncDataList.empty() == true && m_LastSendSyncDataList.empty() == true )
        return;

    if ( m_SendSyncDataList.empty() == false )
    {
        m_ReceiveSyncDataList.insert( m_ReceiveSyncDataList.end(), m_SendSyncDataList.begin(), m_SendSyncDataList.end() );
        _PostProcess_ReceiveFrameData();
    }

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if( true == m_SendSyncDataList.empty() )
//		return;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	if ( g_pData->GetGameUDP()->GetNonRelayUIDs().empty() == false
		|| g_pData->GetGameUDP()->GetRelayUIDs().empty() == false )
    {
        if ( g_pX2Game->GetFrameUDPPack().GetUnitUID() == GetUnitUID() )
        {
            KDYNAMIC_UNIT_USER_SYNC kLastSync;
            KDYNAMIC_UNIT_USER_SYNC kXPT_UNIT_USER_SYNC;
            BYTE    bySerialized[sizeof(KDYNAMIC_UNIT_USER_SYNC)];
//#ifndef X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//            kLastSync.Reset();
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
		    KTDXPROFILE_BEGIN( "COPY" );
            unsigned uSize = m_LastSendSyncDataList.size() + m_SendSyncDataList.size();
            const SyncData* pSyncData = NULL;
            for( unsigned u = 0; u != uSize; ++u )
		    {
			    //ù��° �׸��� framemovecount�� ����
                if ( u < m_LastSendSyncDataList.size() )
                    pSyncData = &m_LastSendSyncDataList[u];
                else
                    pSyncData = &m_SendSyncDataList[u-m_LastSendSyncDataList.size()];
                kXPT_UNIT_USER_SYNC.Reset();
                kXPT_UNIT_USER_SYNC.m_ucNowState = pSyncData->nowState;
                kXPT_UNIT_USER_SYNC.m_ucNowAction			= pSyncData->nowAction;
                kXPT_UNIT_USER_SYNC.m_cStateChangeNum			= pSyncData->stateChangeNum;
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                kXPT_UNIT_USER_SYNC.m_fPosY = pSyncData->position.y;
                _EncodeUserSyncPos( kXPT_UNIT_USER_SYNC.m_usPosX, kXPT_UNIT_USER_SYNC.m_usPosZ, pSyncData->position, pSyncData->lastTouchLineIndex );
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//                _EncodeUserSyncPos( kXPT_UNIT_USER_SYNC.m_usPosX, kXPT_UNIT_USER_SYNC.m_usPosY, kXPT_UNIT_USER_SYNC.m_usPosZ, pSyncData->position, pSyncData->lastTouchLineIndex );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                kXPT_UNIT_USER_SYNC.m_ucLastTouchLineIndex = pSyncData->lastTouchLineIndex;
				kXPT_UNIT_USER_SYNC.m_fNowHp = pSyncData->fNowHP;
				kXPT_UNIT_USER_SYNC.m_fNowMp = pSyncData->fNowMP;
			    kXPT_UNIT_USER_SYNC.m_cEncodedData = 0;
			    kXPT_UNIT_USER_SYNC.EncodeHyperModeCount( (char) pSyncData->m_HyperModeCount );
			    kXPT_UNIT_USER_SYNC.EncodeIsRight( pSyncData->bIsRight );
			    kXPT_UNIT_USER_SYNC.EncodeFrameStop( pSyncData->bFrameStop );
			    kXPT_UNIT_USER_SYNC.EncodeDirectChange( pSyncData->bDirectChange );
				kXPT_UNIT_USER_SYNC.m_sEncodedDataFromCannonBallCountAndEtc = 0;
			    kXPT_UNIT_USER_SYNC.EncodeCannonBallCount( static_cast<int>( pSyncData->m_CannonBallCount ) );
			    kXPT_UNIT_USER_SYNC.m_usRandomTableIndex = pSyncData->m_usRandomTableIndex;
                kXPT_UNIT_USER_SYNC.m_ucHitCount = pSyncData->ucHitCount;
			    kXPT_UNIT_USER_SYNC.m_ucHittedCount = pSyncData->ucHittedCount;
                kXPT_UNIT_USER_SYNC.m_ucNumOfDeBuff = pSyncData->ucNumOfDeBuff;
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                kXPT_UNIT_USER_SYNC.m_dwRelativePos = pSyncData->m_dwRelativePos;
                BYTE* pByBuffer = &bySerialized[0];
                kXPT_UNIT_USER_SYNC.Serialize( pByBuffer, &kLastSync );
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//                BYTE* pByBuffer = &bySerialized[0];
//                kXPT_UNIT_USER_SYNC.Serialize( pByBuffer, ( u == 0 ) ? NULL : &kLastSync );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
                if ( false == g_pX2Game->GetFrameUDPPack().AddFrameUDPPack_IDPack( XPT_UNIT_USER_SYNC_PACK, &bySerialized[0], (WORD)( pByBuffer-&bySerialized[0] ), true ) )
                {
                    BYTE* pByBuffer = &bySerialized[0];
                    kXPT_UNIT_USER_SYNC.Serialize( pByBuffer, NULL );
                    g_pX2Game->GetFrameUDPPack().AddFrameUDPPack_IDPack( XPT_UNIT_USER_SYNC_PACK, &bySerialized[0], (WORD)( pByBuffer-&bySerialized[0] ), false );
                }
                kLastSync = kXPT_UNIT_USER_SYNC;
		    }
		    KTDXPROFILE_END();
        }
    }

	if( g_pMain->IsEnableUdpPacketOverlap() )
		m_LastSendSyncDataList.swap( m_SendSyncDataList );
	else
		m_LastSendSyncDataList.clear();

    m_SendSyncDataList.resize(0);


//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//    {
//
//		m_kXPT_UNIT_USER_SYNC_PACK.m_vecUserSyncList.resize(0);
//		m_kXPT_UNIT_USER_SYNC_PACK.m_iUnitUID = m_UnitUID;
//
//		KTDXPROFILE_BEGIN( "COPY" );
//		for( int i = 0; i < (int)m_SendSyncDataList.size(); i++ )
//		{
//			KXPT_UNIT_USER_SYNC kXPT_UNIT_USER_SYNC;
//
//#ifdef OPTIMIZED_P2P
//			SyncData* pSyncData = &m_SendSyncDataList[i];
//			kXPT_UNIT_USER_SYNC.m_ucNowState = pSyncData->nowState;
//
//			kXPT_UNIT_USER_SYNC.nowAction			= pSyncData->nowAction;
//
//
//            if( !g_pX2Game )
//            {
//                break;
//            }
//
//            if( !g_pX2Game->GetLineMap() )
//            {
//                break;
//            }
//
//            D3DXVECTOR3 v3LandStart = D3DXVECTOR3( pSyncData->position.x, pSyncData->position.y + LINE_RADIUS, pSyncData->position.z );
//            D3DXVECTOR3 v3LandEnd = D3DXVECTOR3( pSyncData->position.x, 0.f, pSyncData->position.z );
//            D3DXVECTOR3 v3ContactPoint1, v3ContactPoint2;
//            const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )pSyncData->lastTouchLineIndex );
//            if( pLineData )
//            {
//                if( !g_pKTDXApp->GetCollision()->LineToLine( v3LandStart, v3LandEnd, pLineData->startPos, pLineData->endPos, LINE_RADIUS, v3ContactPoint1, v3ContactPoint2 ) )
//                {
//                    StateLog( L"��ġ �̻��ѵ�" );
//                    kXPT_UNIT_USER_SYNC.m_usPosX = floatToHalf( 0.f );
//                }
//                else
//                {
//                    kXPT_UNIT_USER_SYNC.m_usPosX = floatToHalf( D3DXVec3Length( &( v3ContactPoint2 - pLineData->startPos ) ) );
//                }
//                kXPT_UNIT_USER_SYNC.m_usPosY = floatToHalf( pSyncData->position.y - pLineData->startPos.y );
//            }
//            else
//            {
//                StateLog( L"���θ� ������ �̻�" );
//                kXPT_UNIT_USER_SYNC.m_usPosX = floatToHalf( 0.f );
//                kXPT_UNIT_USER_SYNC.m_usPosY = floatToHalf( 0.f );
//            }
//            kXPT_UNIT_USER_SYNC.m_ucLastTouchLineIndex = pSyncData->lastTouchLineIndex;
//
//            kXPT_UNIT_USER_SYNC.EncodeEncodedData( pSyncData->bIsRight, pSyncData->bFrameStop, pSyncData->bDirectChange, ( int )pSyncData->stateChangeNum, ( int )pSyncData->m_HyperModeCount );
//
//#ifdef NEW_RANDOM_TABLE_TEST
//			kXPT_UNIT_USER_SYNC.m_usRandomTableIndex = pSyncData->m_usRandomTableIndex;
//#else NEW_RANDOM_TABLE_TEST
////{{AFX
//			kXPT_UNIT_USER_SYNC.m_RandSeed			= pSyncData->m_RandSeed;
//			kXPT_UNIT_USER_SYNC.m_RandSeed2			= pSyncData->m_RandSeed2;
////}}AFX
//#endif NEW_RANDOM_TABLE_TEST
//
//            // �ϴ��� for loop �ȿ��� �̵����� ����. ���߿� ������ ���� ��.��
//            m_kXPT_UNIT_USER_SYNC_PACK.m_usNowHP = floatToHalf( pSyncData->fNowHP );
//            m_kXPT_UNIT_USER_SYNC_PACK.m_usNowMP = floatToHalf( pSyncData->fNowMP );
//
//#else OPTIMIZED_P2P
//
////{{AFX
//			SyncData* pSyncData						= &m_SendSyncDataList[i];
//			kXPT_UNIT_USER_SYNC.dwFrameMoveCount	= pSyncData->dwFrameMoveCount;
//			kXPT_UNIT_USER_SYNC.nowState			= pSyncData->nowState;
//			kXPT_UNIT_USER_SYNC.nowAction			= pSyncData->nowAction;
//			kXPT_UNIT_USER_SYNC.stateChangeNum		= pSyncData->stateChangeNum;
//			kXPT_UNIT_USER_SYNC.posX				= pSyncData->position.x;
//			kXPT_UNIT_USER_SYNC.posY				= pSyncData->position.y;
//			kXPT_UNIT_USER_SYNC.posZ				= pSyncData->position.z;
//			kXPT_UNIT_USER_SYNC.lastTouchLineIndex	= pSyncData->lastTouchLineIndex;
//
////{{ kimhc // 2010.12.7 // ����, ���� ��� ���� �Ǵ� ��Ƽ���� HP�� ���� �ʴ� ���� ����
//			// HP�� MP�� 2����Ʈ���� 4����Ʈ�� �þ, PosX,Y,Z�� 2����Ʈ �������� ���̴� ���� ������ ���� �ҵ�
//			kXPT_UNIT_USER_SYNC.fNowHP				= pSyncData->fNowHP;
//			kXPT_UNIT_USER_SYNC.fNowMP				= pSyncData->fNowMP;
////}} kimhc // 2010.12.7 // ����, ���� ��� ���� �Ǵ� ��Ƽ���� HP�� ���� �ʴ� ���� ����
//
//			kXPT_UNIT_USER_SYNC.m_EncodedData = 0;
//			kXPT_UNIT_USER_SYNC.EncodeHyperModeCount( (char) pSyncData->m_HyperModeCount );
//			kXPT_UNIT_USER_SYNC.EncodeIsRight( pSyncData->bIsRight );
//			kXPT_UNIT_USER_SYNC.EncodeFrameStop( pSyncData->bFrameStop );
//			kXPT_UNIT_USER_SYNC.EncodeDirectChange( pSyncData->bDirectChange );
//
////{{ kimhc // 2010.12.13 // 2010-12-23 New Character CHUNG
//#ifdef	NEW_CHARACTER_CHUNG
//			kXPT_UNIT_USER_SYNC.m_EncodedDataFromCannonBallCountAndEtc = 0;
//			kXPT_UNIT_USER_SYNC.EncodeCannonBallCount( static_cast<int>( pSyncData->m_CannonBallCount ) );
//#endif	NEW_CHARACTER_CHUNG
////}} kimhc // 2010.12.13 //  2010-12-23 New Character CHUNG
//
//#ifdef NEW_RANDOM_TABLE_TEST
//			kXPT_UNIT_USER_SYNC.m_usRandomTableIndex = pSyncData->m_usRandomTableIndex;
//#else NEW_RANDOM_TABLE_TEST
//			kXPT_UNIT_USER_SYNC.m_RandSeed			= pSyncData->m_RandSeed;
//			kXPT_UNIT_USER_SYNC.m_RandSeed2			= pSyncData->m_RandSeed2;
//#endif NEW_RANDOM_TABLE_TEST
////}}AFX
//
//#endif // OPTIMIZED_P2P
//
//			kXPT_UNIT_USER_SYNC.ucHitCount		= pSyncData->ucHitCount;
//			kXPT_UNIT_USER_SYNC.ucHittedCount	= pSyncData->ucHittedCount;
//			kXPT_UNIT_USER_SYNC.ucNumOfDeBuff	= pSyncData->ucNumOfDeBuff;
//
//			m_kXPT_UNIT_USER_SYNC_PACK.m_vecUserSyncList.push_back( kXPT_UNIT_USER_SYNC );
//		}
//		KTDXPROFILE_END();
//
//
//		KTDXPROFILE_BEGIN( "SERIALIZE_BROADCAST" );
//		KSerBuffer buff;
//		Serialize( &buff, &m_kXPT_UNIT_USER_SYNC_PACK );
//		g_pData->GetGameUDP()->BroadCast( g_pX2Game->GetVecUserUIDforSyncPacket(), XPT_UNIT_USER_SYNC_PACK, (char*)buff.GetData(), buff.GetLength() );
//		KTDXPROFILE_END();
//
//		m_SendSyncDataList.resize(0);
//    }
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}




//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


void CX2GUUser::_PostProcess_ReceiveFrameData()
{
	if( m_bFirstDataReceive == false )
	{

		CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarker    = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
//#ifdef TITLE_SYSTEM
		CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#else
//		CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#endif TITLE_SYSTEM

		if( NULL != pSeqHeadMarker )
			pSeqHeadMarker->SetShowObject(true);

		if( NULL != pSeqEmblem )
			pSeqEmblem->SetShowObject( true );

		if ( false == IsMyUnit() )	/// ù ��Ŷ�� ���� ������ �������� �ƴϰ�
		{
			/// ������ �ɸ� ����/������� ������
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() && !g_pX2Game->GetMyUnit()->IsBuffTempletListEmpty() )
				g_pX2Game->GetMyUnit()->SendBuffInfoNot( GetUnitUID() );	/// ��� ��Ŷ�� ���� �������� ����/����� ������ ������
		}
	    m_bFirstDataReceive = true;
	}

	m_bConnect			= true;
}

void CX2GUUser::ReceiveFrameData( DWORD dwFrameMoveCount, const KDYNAMIC_UNIT_USER_SYNC& kUserSync )
{
	KTDXPROFILE();

	//���ʷ� sync �����͸� ���� ������ ��� �����Ͱ� �Դ��� Ȯ���Ѵ�.
	if( kUserSync.m_usDataFieldFlag != KDYNAMIC_UNIT_USER_SYNC::eUnitUserSync_DataField_All )
    {
        return;
    }
    if ( dwFrameMoveCount <= m_FrameDataNow.syncData.dwFrameMoveCount )
    {
		return;
	}

	float fMaxHP = GetMaxHp();
	float fMaxMP = GetMaxMp();

	SyncData syncData;
	syncData.SetData( kUserSync, dwFrameMoveCount, fMaxHP, fMaxMP );
    if ( m_ReceiveSyncDataList.empty() == true )
    {
	    m_ReceiveSyncDataList.push_back( syncData );
    }
    else
    {
        if ( m_ReceiveSyncDataList.back().dwFrameMoveCount < syncData.dwFrameMoveCount )
            m_ReceiveSyncDataList.push_back( syncData );
        else
        {
            std::vector<SyncData>::iterator iter = std::lower_bound( m_ReceiveSyncDataList.begin(), m_ReceiveSyncDataList.end(), syncData.dwFrameMoveCount );
            if ( iter != m_ReceiveSyncDataList.end() && iter->dwFrameMoveCount == syncData.dwFrameMoveCount )
			{
				if( true == IsAvaliableActionDuringDamageReact( iter->nowAction ) )
				{
					syncData.nowAction = iter->nowAction;
				}
                *iter = syncData;
			}
            else
                m_ReceiveSyncDataList.insert( iter, syncData );
        }
    }

    _PostProcess_ReceiveFrameData();

}

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//void CX2GUUser::ReceiveFrameData( KXPT_UNIT_USER_SYNC_PACK* pKXPT_UNIT_USER_SYNC_PACK )
//{
//	KTDXPROFILE();
//
//    if( !pKXPT_UNIT_USER_SYNC_PACK )
//    {
//        return;
//    }
//
//
//
//#ifdef OPTIMIZED_P2P
//    std::vector< KXPT_UNIT_USER_SYNC >::iterator vit;
//	for( vit = pKXPT_UNIT_USER_SYNC_PACK->m_vecUserSyncList.begin();
//         vit != pKXPT_UNIT_USER_SYNC_PACK->m_vecUserSyncList.end();
//         vit++ )
//	{
//		SyncData syncData;
//		syncData.SetData( *vit, pKXPT_UNIT_USER_SYNC_PACK->m_usNowHP, pKXPT_UNIT_USER_SYNC_PACK->m_usNowMP );
//		m_ReceiveSyncDataList.push_back( syncData );
//	}
//#else
//    for( int i = 0; i < (int)pKXPT_UNIT_USER_SYNC_PACK->m_vecUserSyncList.size(); i++ )
//	{
//        KXPT_UNIT_USER_SYNC* pKXPT_UNIT_USER_SYNC = &pKXPT_UNIT_USER_SYNC_PACK->m_vecUserSyncList[i];
//		SyncData syncData;
//		syncData.SetData( pKXPT_UNIT_USER_SYNC );
//		m_ReceiveSyncDataList.push_back( syncData );
//	}
//#endif // OPTIMIZED_P2P
//
//// 	if ( !m_ReceiveSyncDataList.empty() )
//// 	{
//// 		auto SortFunc = []( const SyncData& lhs_, const SyncData& rhs_ ) {
//// 			return lhs_.dwFrameMoveCount < rhs_.dwFrameMoveCount;
//// 		};
////
//// 		sort( m_ReceiveSyncDataList.begin(), m_ReceiveSyncDataList.end(), SortFunc );
//// 	}
//
//	if( m_bFirstDataReceive == false )
//	{
//
//		CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarker    = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqHeadMarker );
//#ifdef TITLE_SYSTEM
//		CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#else
//		CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem		   = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#endif TITLE_SYSTEM
//
//		if( NULL != pSeqHeadMarker )
//			pSeqHeadMarker->SetShowObject(true);
//
//		if( NULL != pSeqEmblem )
//			pSeqEmblem->SetShowObject( true );
//
//// 		if ( !m_ReceiveSyncDataList.empty() )
//// 		{
//// 			SyncData& ReceiveSyncData = m_ReceiveSyncDataList.back();
//// 			SetFrameMoveCount( GetFrameMoveCount() + ReceiveSyncData.dwFrameMoveCount );
//// 			SetHitCount( GetHitCount() + ReceiveSyncData.ucHitCount );
//// 			SetHittedCount( GetHittedCount() + ReceiveSyncData.ucHittedCount );
//// 			SetNumOfDeBuff( GetNumOfDeBuff() + ReceiveSyncData.ucNumOfDeBuff );
//// 		}
//
//		if ( false == IsMyUnit() )	/// ù ��Ŷ�� ���� ������ �������� �ƴϰ�
//		{
//#ifdef FIX_OBSERVER_MODE_CRASH
//			/// ������ �ɸ� ����/������� ������
//			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() && !g_pX2Game->GetMyUnit()->IsBuffTempletListEmpty() )
//#else  FIX_OBSERVER_MODE_CRASH
//			if ( !g_pX2Game->GetMyUnit()->IsBuffTempletListEmpty() )	/// ������ �ɸ� ����/������� ������
//#endif FIX_OBSERVER_MODE_CRASH
//				g_pX2Game->GetMyUnit()->SendBuffInfoNot( GetUnitUID() );	/// ��� ��Ŷ�� ���� �������� ����/����� ������ ������
//		}
//	}
//
//	m_bFirstDataReceive = true;
//	m_bConnect			= true;
//
//}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


/*virtual*/ void CX2GUUser::RenderName()
{
	if( false == GetShowObject() )
		return;

	if( true == GetAbsoluteInvisibility() )
		return;

	if( NULL == g_pX2Game ||
		NULL == g_pX2Game->GetFontForUnitName() ||
		NULL == GetUnit()
		//{{ JHKang / ������ / 2010/12/08 / RenderName ���� ũ���� ����
		//}} JHKang / ������ / 2010/12/08 / RenderName ���� ũ���� ����
		)
	{
		return;
	}

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		return;

	if( g_pX2Game->GetMyUnit() != NULL &&
		g_pX2Game->GetMyUnit()->GetWhiteOut() == true )
	{
		return;
	}

	if( NULL != g_pData->GetPVPRoom() &&
		g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_SURVIVAL )
	{
		if ( g_pX2Game->GetMyUnit() == this )
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), GetUnit()->GetNickName(), 0xffffaa77,
				CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
		}
		else
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), GetUnit()->GetNickName(), 0xff54ddff,
				CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
		}
	}
	else
	{
		if( GetTeam() == CX2Room::TN_BLUE )
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), GetUnit()->GetNickName(), 0xff54ddff,
				CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
		}
		else
		{
			if ( ShouldAttackAllTeam() )
			{
				g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), GetUnit()->GetNickName(), 0xff54ddff,
					CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
			}
			else
			{
				g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), GetUnit()->GetNickName(), 0xffffaa77,
					CKTDGFontManager::FS_SHELL, 0xffff0000 , NULL, DT_CENTER  );
			}
		}
	}


	int iWidthUnitName = 0;
	int iFinalLeft = 0;
	int iFinalTop = 0;

	D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos(GetPos());

	iWidthUnitName = g_pX2Game->GetFontForUnitName()->GetWidth(GetUnit()->GetNickName());

	iFinalLeft = (int) finalPos.x + (iWidthUnitName / 2);
	iFinalTop = (int) finalPos.y;


	bool bPvpRank = g_pMain->GetGameOption().GetOptionList().m_bPvpRank;

	bool bNothing = g_pMain->GetGameOption().GetOptionList().m_bNothing;
	if( true == bNothing && true == IsMyUnit() )
	{
		bPvpRank = false;
	}

	if( true == bPvpRank )
	{
		int iFinalLeftRank = (int)finalPos.x - (iWidthUnitName / 2);
		RenderRank(iFinalLeftRank, iFinalTop);
	}

	//{{ ����� : [2009/10/13] //	����̸� ���
	if( GetUnit()->GetUnitData().m_wstrGuildName != L"" )
	{
		D3DXVECTOR3 vPos = GetPos();

		vPos.y -= 22.0f;

		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, (g_pData->GetGuildManager()->ConvertDisplayGuildName( GetUnit()->GetUnitData().m_wstrGuildName) ).c_str(),
			0xff96ff00, CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
	}
	//}} ����� : [2009/10/13] //	����̸� ���




	IF_EVENT_ENABLED( CEI_RELATIONSHIP_CODE_EVENT )
	{
		if( GetUnit()->GetUnitData().m_bCouple == true && GetUnit()->GetUnitData().m_wstrRelationTargetUserNickname != L"" )
		{
			D3DXVECTOR3 vPos = GetPos();

			if( m_pUnit->GetUnitData().m_wstrGuildName != L"" )
			{
				vPos.y -= 70.0f;
			}
			else
			{
				vPos.y -= 35.0f;
			}

			wstringstream adBuf;
			adBuf << GetUnit()->GetNickName();
			adBuf << L" �� ";
			adBuf << GetUnit()->GetUnitData().m_wstrRelationTargetUserNickname;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf.str().c_str(), 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
		}
	}


#if X2_DEV_TOOLS
	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		if( true == CX2Game::GetRenderNPCName() )
		{
			WCHAR adBuf[200] = {0,};
			D3DXVECTOR3 vPos( GetPos() );
			vPos.y -= 20.0f;


/// �޺� ��ġ ���
//////////////////////////////////////////////////////////////////////////
			int iComboNum = GetComboManager()->GetCombo();
			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Combo: %d", iComboNum );

			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
//
// 			if ( g_pX2Game->IsHost() )
// 			{
// 				vPos.y -= 40.0f;
// 				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, L"����",
// 					0xff96ff00, CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
// 			}
//

/// �ٿ� ��ġ ���
//////////////////////////////////////////////////////////////////////////
			float fForceDownValue = m_pGageData->GetNowForceDown();
			//StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"ForceDown: %d, UID: %lld", (int)fForceDownValue, GetUnitUID() );
			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"ForceDown: %d", static_cast<int>(fForceDownValue) );
			vPos.y -= 20.0f;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf, 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );

			//{{ ����� : [2009/8/4] //	�÷��̾� ü�� ǥ��
			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"HP: %d/%d", (int)GetNowHp(), (int)GetMaxHp() );
/// ü�� ���
//////////////////////////////////////////////////////////////////////////
			//{{ ����� : [2009/8/4] //	�÷��̾� ü�� ǥ��			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"HP: %d/%d", (int)GetNowHp(), (int)GetMaxHp() );

			vPos.y -= 40.0f;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );


			//}} ����� : [2009/8/4] //	�÷��̾� ü�� ǥ��


/// û ĳ���� ���
//////////////////////////////////////////////////////////////////////////
//{{ kimhc // 2010.12.18 // 2010-12-23 New Character CHUNG
// 			if ( CX2Unit::UT_CHUNG == GetUnit()->GetType() )
// 			{
// 				wstringstream wstreamCannonBall;
// 				wstreamCannonBall << L"ĳ����: ";
// 				wstreamCannonBall << GetGageManager()->GetCannonBallUIPtr()->GetCannonBallCount() << L" / ";
// 				wstreamCannonBall << GetGageManager()->GetCannonBallUIPtr()->GetMaxCannonBallCount();
//
// 				vPos.y -= 20.0f;
// 				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, wstreamCannonBall.str().c_str(),
// 					0xff77aaff, CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
// 			} // if
//}} kimhc // 2010.12.18 //  2010-12-23 New Character CHUNG


/// ���̺� �г� ������ ���
//////////////////////////////////////////////////////////////////////////
			if( CX2Unit::UT_RAVEN == GetUnit()->GetType() )
			{
				CX2GURaven *pRaven = (CX2GURaven*)this;

				WCHAR RageGuageBuf[200] = {0,};
				if( IsMyUnit() == true )
					StringCchPrintfW( RageGuageBuf, ARRAY_SIZE( RageGuageBuf ), L"�г������: %5.2f(%d)", pRaven->GetRageGuage(), pRaven->GetNasodBall() );
				else
					StringCchPrintfW( RageGuageBuf, ARRAY_SIZE( RageGuageBuf ), L"�г������: (%d)", pRaven->GetNasodBall() );

				vPos = GetPos() + D3DXVECTOR3( 0, -100, 0 );
				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, RageGuageBuf, 0xff77aaff, CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
			}


/// �̺� �⵿ ������ ���
//////////////////////////////////////////////////////////////////////////
			//{{ JHKang /  / 2010/11/25
/// ???
//////////////////////////////////////////////////////////////////////////
			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Given: %d", m_iTotalGivenDamamge );

			vPos.y -= 20.0f;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );


/// ???
//////////////////////////////////////////////////////////////////////////
			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"SlotABkill : %d, %d, %d, %d", m_iSkillSlotBCount[0], m_iSkillSlotBCount[1], m_iSkillSlotBCount[2], m_iSkillSlotBCount[3] );

			vPos.y -= 20.0f;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );


/// ��ҷ� ���
//////////////////////////////////////////////////////////////////////////
//}} JHKang /  / 2010/11/25
//#ifdef UNDERWATER_LINEMAP
//			StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"���: %d/%d", (int)m_fOxygenGage, (int)MAGIC_OXYGEN_GAGE );
//
//			vPos.y -= 20.0f;
//			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
//				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
//#endif
/// ���ҵ�, �����ý� ���� �� ��ġ ���
//////////////////////////////////////////////////////////////////////////
			if ( CX2Unit::UT_ELSWORD == GetUnit()->GetType()
				|| CX2Unit::UT_ELESIS == GetUnit()->GetType()
				)
			{
				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"WSP: %d", (int)GetWayOfSwordPoint() );

				vPos.y -= 20.0f;
				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
					0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
			}


/// ���� ��ġ ( �ֵ��  DP ��ġ ) ���
//////////////////////////////////////////////////////////////////////////
			if ( CX2Unit::UT_ADD == GetUnit()->GetType() )
			{
				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"DP: %d / %d",
								  static_cast<int>( static_cast<CX2AddGageData*>( m_pGageData )->GetDPValue() ),
								  static_cast<int>( static_cast<CX2AddGageData*>( m_pGageData )->GetMaxDPValue() ) );
			}
			else
				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Hyper Gage: %d / %d", static_cast<int>( GetNowSoul() ), static_cast<int>( GetMaxSoul() ) );

			vPos.y -= 20.0f;
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
				0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );

/// ���� ��ġ ���
//////////////////////////////////////////////////////////////////////////
			if ( CX2Unit::UT_ADD == GetUnit()->GetType() )
			{
				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Mutation Count: %d / 6",
					static_cast<int>( static_cast<CX2AddGageData*>( m_pGageData )->GetMutationCount() ) );

				vPos.y -= 20.0f;
				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
					0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
			}

/// �����̽ý� �ʵ� ���� ��ġ ǥ��
//////////////////////////////////////////////////////////////////////////
			if ( CX2Unit::UT_ADD == GetUnit()->GetType() )
			{
				CX2GUAdd *pAdd = static_cast<CX2GUAdd*>( this );

				if ( NULL != pAdd && true == pAdd->GetIsActiveStasisfield() )
				{
					StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Hit/ Damage: %f/ %f",
						pAdd->GetStasisfieldData().m_fHitValue, pAdd->GetStasisfieldData().m_fDamageValue );

					vPos.y -= 20.0f;
					g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
						0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
				}
			}

/// ĳ���� ����
//////////////////////////////////////////////////////////////////////////
			if ( CX2Unit::UT_CHUNG == GetUnit()->GetType() )
			{
				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Cannon Ball: %d / %d",
								  static_cast<int>( static_cast<CX2ChungGageData*>( m_pGageData )->GetNowCannonBallCount() ),
								  static_cast<int>( static_cast<CX2ChungGageData*>( m_pGageData )->GetMaxCannonBallCount() ) );

				vPos.y -= 20.0f;
				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
					0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
			}


/// ???
//////////////////////////////////////////////////////////////////////////
// #ifdef PVP_SEASON2
// 			{
// 				StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"KFactor: %f", GetUnit()->GetUnitData().m_fKFactor );
//
// 				vPos = GetPos() + D3DXVECTOR3( 0, -180, 0 );
// 				g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, adBuf,
// 					0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
// 			}
// #endif

		}



	}
#endif // _SERVICE_

}

void CX2GUUser::RenderRank(int iFinalLeft, int iFinalTop)
{
	if( NULL != g_pX2Game && CX2Game::GT_BATTLE_FIELD != g_pX2Game->GetGameType() )
		return;

	if(m_pTextureRank == NULL)
		return;

	if ( NULL == g_pMain->GetPVPEmblem() )
		return;

	wstring wstrRankKey = g_pMain->GetPVPEmblem()->GetPVPEmblemData( GetUnit()->GetPvpRank() )->m_TextureKey;

	MakeUpperCase( wstrRankKey );
	const CKTDXDeviceTexture::TEXTURE_UV*	pTexUV = m_pTextureRank->GetTexUV( wstrRankKey );
	if(pTexUV == NULL)
		return;

	float fFinalLeft, fFinalTop, fScaleX, fScaleY, fOffsetX, fOffsetY;

	fOffsetX	= 22.f / g_pKTDXApp->GetResolutionScaleX();
	fOffsetY	= 2.f / g_pKTDXApp->GetResolutionScaleY();
	fScaleX		= 20.f / g_pKTDXApp->GetResolutionScaleX();
	fScaleY		= 20.f; // / g_pKTDXApp->GetResolutionScaleY();
	fFinalLeft	= (iFinalLeft) / g_pKTDXApp->GetResolutionScaleX();
	fFinalTop	= (iFinalTop) / g_pKTDXApp->GetResolutionScaleY();

	fFinalLeft -= fOffsetX;
	fFinalTop -= fOffsetY;

	m_pTextureRank->Draw( fFinalLeft, fFinalTop, fScaleX, fScaleY,
		(int)pTexUV->leftTop.x, (int)pTexUV->leftTop.y, (int)pTexUV->rightBottom.x - (int)pTexUV->leftTop.x, (int)pTexUV->rightBottom.y - (int)pTexUV->leftTop.y );
}
//{{ oasis907 : ����� [2010.5.17] //
void CX2GUUser::RenderServer(int iFinalLeft, int iFinalTop)
{
	if(m_pTextureServer == NULL)
		return;


	const CKTDXDeviceTexture::TEXTURE_UV*	pTexUV = NULL;

#ifdef EXTEND_SERVER_GROUP_MASK
#else
	SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;

	eServerGroupID = (SERVER_GROUP_ID) g_pMain->ExtractServerGroupID(GetUnitUID());

	if( eServerGroupID == SGI_SOLES)
	{
		pTexUV = m_pTextureServer->GetTexUV( L"SOLES" );
	}
	else if( eServerGroupID == SGI_GAIA)
	{
		pTexUV = m_pTextureServer->GetTexUV( L"GAIA" );
	}
	else
#endif // EXTEND_SERVER_GROUP_MASK
	{
		pTexUV = NULL;
	}

	if(pTexUV == NULL)
		return;

	float fFinalLeft, fFinalTop, fScaleX, fScaleY, fOffsetX, fOffsetY;

	fOffsetX	= 2.f / g_pKTDXApp->GetResolutionScaleX();
	fOffsetY	= -3.f / g_pKTDXApp->GetResolutionScaleY();
	fScaleX		= 21.f ;// g_pKTDXApp->GetResolutionScaleX();
	fScaleY		= 23.f ;// g_pKTDXApp->GetResolutionScaleY();
	fFinalLeft	= (iFinalLeft) / g_pKTDXApp->GetResolutionScaleX();
	fFinalTop	= (iFinalTop) / g_pKTDXApp->GetResolutionScaleY();

	fFinalLeft += fOffsetX;
	fFinalTop += fOffsetY;

	m_pTextureServer->Draw( fFinalLeft, fFinalTop, fScaleX, fScaleY,
		(int)pTexUV->leftTop.x, (int)pTexUV->leftTop.y, (int)pTexUV->rightBottom.x - (int)pTexUV->leftTop.x, (int)pTexUV->rightBottom.y - (int)pTexUV->leftTop.y );
}
//}} oasis907 : ����� [2010.5.17] //


void CX2GUUser::Verify()
{
	CX2GameUnit::Verify();

	if ( IsMyUnit() )
	{
		m_FrameDataFuture.syncData.Verify();
		m_FrameDataNow.syncData.Verify();

		if( m_FrameDataFuture.stateParam.Verify()	== false
			|| m_FrameDataNow.stateParam.Verify()	== false )
		{
			g_pKTDXApp->SetFindHacking( true );
		}

		if ( NULL != GetUnit() &&
			GetUnit()->GetUnitData().m_UserSkillTree.VerifyEquippedSkillslot() == false )
		{
			if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER &&
				g_pKTDXApp->GetFindHacking() == false )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
			}


			g_pKTDXApp->SetFindHacking( true );
		}
	}
}

void CX2GUUser::SyncData::Verify()
{
	if( nowState.Verify()				== false
		|| nowSubState.Verify()			== false
		|| nowAction.Verify()			== false
		|| fNowHP.Verify()				== false
		|| fNowMP.Verify()				== false
		|| m_HyperModeCount.Verify()	== false
//{{ kimhc // 2010.12.13 // 2010-12-23 New Character CHUNG
		|| m_CannonBallCount.Verify()	== false
//}} kimhc // 2010.12.13 //  2010-12-23 New Character CHUNG
		)
	{
		g_pKTDXApp->SetFindHacking( true );
	}
}

int CX2GUUser::SyncData::IsActionActive( UINT uiActionIndex_ )
{
	return (nowAction & (1 << uiActionIndex_) );
}

bool CX2GUUser::IsActionActive( USER_ACTION_ID eUserActionID_ )
{
	UINT uiActionIndex = static_cast< UINT > ( eUserActionID_ );
	if( m_FrameDataNow.syncData.IsActionActive( uiActionIndex ) != 0 )
	{
		return true;
	}
	return false;
}

//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
/** @function : SetLockOn
	@brief : ���� Ÿ�Կ� ���� Ÿ���� ���� �� UID�� ������ �Լ�
	@param : CX2DamageEffect::CEffect* pCEffect, int randomOffset, CX2DamageEffect::LOCK_ON_TYPE eLockOnType_ = CX2DamageEffect::LOT_NONE
*/
void CX2GUUser::SetLockOn( CX2DamageEffect::CEffect* pCEffect, int randomOffset, CX2DamageEffect::LOCK_ON_TYPE eLockOnType_ /*= CX2DamageEffect::LOT_NONE*/, int iModulusFactor /*= 30*/ )
{
	if( pCEffect != NULL )
	{
		bool bUserUnit = false;
		UidType gameUnitUID = -1;

		switch ( eLockOnType_ )
		{
		case CX2DamageEffect::LOT_NEARST_UID_VECTOR:
			{
				CX2GameUnit* pOwnerUnit = pCEffect->GetOwnerUnit();
				// ����ó��
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();

				if( NULL != pOwnerUnit )
				{
					const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();
					gameUnitUID = g_pX2Game->GetLockOnNearstTarget( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vOwnerUnitPos, bUserUnit );
				}
			}
			break;

		case CX2DamageEffect::LOT_NEARST_UID_VECTOR_IN_RANGE:
			{
				CX2GameUnit* pOwnerUnit = pCEffect->GetOwnerUnit();
				// ����ó��
				if ( NULL == pOwnerUnit )
					pOwnerUnit = g_pX2Game->GetMyUnit();

				if( NULL != pOwnerUnit )
				{
					D3DXVECTOR2 vRange = (g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME) ? D3DXVECTOR2( 150.f, 300.f) : D3DXVECTOR2( 150.f, 750.f);
					const D3DXVECTOR3& vOwnerUnitPos = pOwnerUnit->GetPos();
					bool bUserUnit = false;

					gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vOwnerUnitPos, vRange, bUserUnit );
				}
			}
			break;

		default:
			{
				gameUnitUID = g_pX2Game->GetLockOnTarget( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), GetRandomInt() + randomOffset, bUserUnit, iModulusFactor );
				pCEffect->SetLockOnRandomSeed( GetRandomInt() + randomOffset );
			}
			break;
		}

		if( bUserUnit == true )
			pCEffect->SetLockOnUnitUID( gameUnitUID );
		else
			pCEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
	}
}

//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG


/** @function : ForceChangeHyperMode
	@brief : ������ ���� ���·� ����
	@param : int (������ ���� ���� ����)
*/
void CX2GUUser::ForceChangeHyperMode( int iAddBall_ )
{
	if( IsMyUnit() == true )
	{
		SetHyperModeCount( GetHyperModeCount() + iAddBall_ );
		StateChange( USI_HYPER_MODE );
	}
}

/** @function : ForceChangeHyperModeWithoutMotion
	@brief : ��� ������� ���� ��Ŵ
	@param : float fAddTime_( �߰� ���� �ð� )
*/
void CX2GUUser::ForceChangeHyperModeWithoutMotion( float fAddTime_ )
{
	ASSERT( NULL != g_pX2Game );
	if( NULL == g_pX2Game )
		return;

	IF_HYPER_MODE_STATE( m_NowStateData.stateID )
		return;

	ASSERT( NULL != g_pX2Game->GetWorld() );
	if( NULL == g_pX2Game->GetWorld() )
		return;

	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

	if ( GetShowSmallGageAndName() )
	{
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( GetPos() );
		CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

		CKTDGParticleSystem::CParticleEventSequence* pSeqSTR_ToKang = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STR_TOKANG ) );
		PlaySequenceByTriggerCount( pSeqSTR_ToKang, projPos.x, projPos.y, 0.0f, 100, 100, 1 );

		UpDownCrashCamera( 30.0f, 0.4f );
		g_pKTDXApp->GetDGManager()->ClearScreen();
	}

	// ���� ���� ���̸� �ð��� + ��Ų��
// 	const float fHyperModeTime
// 		= ( GetRemainHyperModeTime() > 0.0f ? GetRemainHyperModeTime() + fAddTime_ : fAddTime_ );

	SetForceHyperModeTime( fAddTime_ );
	ApplyHyperModeBuff();

	/// ���� ���� ���� �� ForceHyperModeTime �ʱ�ȭ
	SetForceHyperModeTime( 0.0f );
}


void CX2GUUser::AddEnchantResist( const CX2SocketItem::SocketData& kSocketData )
{
	// �Ӽ� ����
	//map< CX2EnchantItem::ENCHANT_TYPE, int >::iterator mit;
	for( int k= CX2EnchantItem::ET_NONE+1; k<CX2EnchantItem::ET_END; k++ )
	{
		if( 0 == kSocketData.m_aiResistEnchant[ k ] )
			continue;

		AddDefenceEnchantData( CX2EnchantItem::ENCHANT_TYPE(k), kSocketData.m_aiResistEnchant[ k ]);
	}

}

void CX2GUUser::AddEnchantAttack( const CX2SocketItem::SocketData& kSocketData)
{

	// �Ӽ� ����
	map< CX2EnchantItem::ENCHANT_TYPE, float >::iterator mit;
	for( int k= CX2EnchantItem::ET_NONE+1; k<CX2EnchantItem::ET_END; k++ )
	{
		if( 0 == kSocketData.m_afAttackEnchant[ k ] )
			continue;

		mit = m_mapAttackEnchantData.find( (CX2EnchantItem::ENCHANT_TYPE) k );
		if( mit != m_mapAttackEnchantData.end() )
		{
			mit->second = ( mit->second + kSocketData.m_afAttackEnchant[ k ] ) * kSocketData.m_fAllAttackEnchantRate;	//�Ӽ� �ߵ� Ȯ�� ����
		}
		else
		{
			m_mapAttackEnchantData.insert( std::make_pair( (CX2EnchantItem::ENCHANT_TYPE)k, kSocketData.m_afAttackEnchant[ k ] ) );
		}
	}
}

//{{ kimhc // 2011-07-19 // �ɼǵ���Ÿ ��ġȭ �۾�
// �ɼ� ��ġȭ�� ���� �߰� ������
/*virtual*/ float CX2GUUser::GetAdditionalAttackDamage( const CX2DamageManager::DamageData* pAttackDamageData )
{

	return CalcAdditionalAttackDamage( pAttackDamageData );


}

//}} kimhc // 2011-07-19 // �ɼǵ���Ÿ ��ġȭ �۾�

int CX2GUUser::GetSkillLevelUpNum( CX2SkillTree::SKILL_ID skillID ) const
{
// 	if( NULL != GetUnit()
// 	{
// 		if( false == GetUnit()->GetUnitData().m_UserSkillTree.DoIHaveThisSkill( skillID ) )
// 		{
// 			return 0;
// 		}
// 	}
	if( CX2SkillTree::SI_ETC_WS_COMMON_LOVE == skillID )
		return 0;

	int levelUpNum = 0;

	BOOST_TEST_FOREACH( const SkillLevelUpIDAndNum&, sSkillLevelUpIDAndNum, m_SocketData.m_vecSkillLevelUpID )
	{
		if( skillID == sSkillLevelUpIDAndNum.first )
		{
			levelUpNum += sSkillLevelUpIDAndNum.second;
		}
	}

	levelUpNum += m_SocketData.m_AllSkillLevelUp;
	levelUpNum += GetAddSkillLevelByBuff();	/// ��ų������ �����Ű�� ���� ȿ�� ����



	return levelUpNum;
}

/*virtual*/ void CX2GUUser::TransformScale( const PROTECT_VECTOR3& vScale_ )
{
	m_vTransScale = vScale_;
	//m_vTransScale += (m_vBasicScale - D3DXVECTOR3( 1.0f, 1.0f, 1.0f ));

	m_FrameDataFuture.unitCondition.fUnitWidth	= m_vOrgUnitSize.x * vScale_.x;
	m_FrameDataFuture.unitCondition.fUnitHeight = m_vOrgUnitSize.y * vScale_.y;


	m_FrameDataNow.unitCondition.fUnitWidth		= m_FrameDataFuture.unitCondition.fUnitWidth;
	m_FrameDataNow.unitCondition.fUnitHeight	= m_FrameDataFuture.unitCondition.fUnitHeight;
}

/*virtual*/ float CX2GUUser::GetCriticalPercent( BYTE byDamageType, float fExtraCritical )
{
	//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
	float fCriticalEffectiveRate = GetCriticalRate();
	//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�

	fCriticalEffectiveRate += fExtraCritical;

	fCriticalEffectiveRate += m_fPassiveCriticalRate;

	if( m_petCheer.m_fTime > 0.f && m_petCheer.m_fCriticalRate > 0.f )
	{
		fCriticalEffectiveRate += m_petCheer.m_fCriticalRate;
	}

	switch( byDamageType )
	{
	case CX2DamageManager::DT_MAGIC:
	case CX2DamageManager::DT_MIX:
	case CX2DamageManager::DT_WEAPON_MAGIC:
	case CX2DamageManager::DT_WEAPON_MIX:
		if( 0 < GetAddMagicAttackCriticalRate() )
			fCriticalEffectiveRate += GetAddMagicAttackCriticalRate();
			fCriticalEffectiveRate += m_fShadowOfDisasterRate;
	default:
		break;
	}

	return fCriticalEffectiveRate;	// kimhc // 2011-08-01 // �ɼ� ��ġȭ�� ���� ��ġ �����
}

void CX2GUUser::ChangeModelDetail( int detailPercent )
{
	if ( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->SetModelDetailPercent( detailPercent );
	}

	if ( m_pXSkinAnimFuture != NULL )
	{
		m_pXSkinAnimFuture->SetModelDetailPercent( detailPercent );
	}

	if ( m_pOrgXSkinAnim != NULL )
	{
		m_pOrgXSkinAnim->SetModelDetailPercent( detailPercent );
	}

	if ( m_pOrgXSkinAnimFuture != NULL )
	{
		m_pOrgXSkinAnimFuture->SetModelDetailPercent( detailPercent );
	}

}





bool CX2GUUser::IsImmuneToEnchant( CX2EnchantItem::ENCHANT_TYPE enchantType )
{
	if ( CX2GameUnit::IsImmuneToEnchant( enchantType ) == true )
		return true;

	if( GetRandomFloat( CKTDXRandomNumbers::SRO_RESIST_EXTRA_DAMAGE_ENCHANT ) < GetEnchantResist( enchantType ) / (float)CX2EnchantItem::EAR_MAX_VALUE )
		return true;

	return false;
}

void CX2GUUser::ShowSnatchCureEffect()
{
	D3DXVECTOR3 hpAndMpUpPos = D3DXVECTOR3(0,0,0);
	hpAndMpUpPos = GetBonePos( L"Bip01_Spine" );


	if( m_hSeqHPUp == INVALID_PARTICLE_SEQUENCE_HANDLE )
		m_hSeqHPUp = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Plus_Hp01", hpAndMpUpPos );

	if( m_hSeqMPUp == INVALID_PARTICLE_SEQUENCE_HANDLE )
		m_hSeqMPUp = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Plus_Mp01", hpAndMpUpPos );

	PlaySound( L"Drain_HPMP.ogg" );
}

float CX2GUUser::GetDamageUpPercentBySMA( int npcID )
{
	map< int, float >::iterator mit = m_mapNpcIdNDamageUpPercent.find( npcID );
	if ( mit != m_mapNpcIdNDamageUpPercent.end() )
	{
		return mit->second;
	}

	return 0.0f;
}

float CX2GUUser::GetEvadeUpPerBySMA( int npcID )
{
	map< int, float >::iterator mit = m_mapNpcIDNDEvadeUpPercent.find( npcID );
	if ( mit != m_mapNpcIDNDEvadeUpPercent.end() )
	{
		return mit->second;
	}

	return 0.0f;
}

/*virtual*/ float CX2GUUser::GetPowerRate()
{
	return m_NowStateData.m_fPowerRate;
}

/** @function : CX2GUUser::ActiveSkillShow
	@brief : ��ų���� ��½�̴� ����Ʈ��, ������ ���̵��� �ϴ� �Լ�
	@param : ��½�̴� ����Ʈ�� ���� ���̸�, ��ų���� ����ü�� ������, ������ ù��° �ε���(������ ���� �޶���), ���� ��ų�ΰ��� ����, ������������ ����, ���� ���̰� �Ұ����� ����
	@return : void
*/
void CX2GUUser::ActiveSkillShow( const WCHAR* pBoneName, const SkillCutInSet *s_SkillCutInSet, int iCutInSet, int iCutIn, bool bHyper, bool bOnlyLight)
{
	D3DXVECTOR3 bonePos = m_pXSkinAnim->GetCloneFramePosition( pBoneName );

	ActiveSkillShow( bonePos,
		s_SkillCutInSet[iCutInSet].cutIn[iCutIn].fileName.c_str(),
		s_SkillCutInSet[iCutInSet].cutIn[iCutIn].vSize,
		s_SkillCutInSet[iCutInSet].cutIn[iCutIn].vPosition,
		bHyper, bOnlyLight );
}

/** @function : CX2GUUser::ActiveSkillShow
@brief : ��ų���� ��½�̴� ����Ʈ��, ������ ���̵��� �ϴ� �Լ�
@param : ��½�̴� ����Ʈ�� ���� ���̸�, ��ų���� �ؽ��ĸ�, �ؽ��� ������, �ؽ��İ� ���� ��ġ, ������������ ����, ���� ���̰� �Ұ����� ����
@return : void
*/
void CX2GUUser::ActiveSkillShow( const WCHAR* pBoneName, const WCHAR* pBackFaceTexName, D3DXVECTOR2 size, D3DXVECTOR3 pos, bool bHyper, bool bOnlyLight )
{
	D3DXVECTOR3 bonePos = m_pXSkinAnim->GetCloneFramePosition( pBoneName );
	ActiveSkillShow( bonePos, pBackFaceTexName, size, pos, bHyper, bOnlyLight );
}

/** @function : CX2GUUser::ActiveSkillShow
@brief : ��ų���� ��½�̴� ����Ʈ��, ������ ���̵��� �ϴ� �Լ�
@param : ��½�̴� ����Ʈ�� ���� ����ġ, ��ų���� �ؽ��ĸ�, �ؽ��� ������, �ؽ��İ� ���� ��ġ, ������������ ����, ���� ���̰� �Ұ����� ����
@return : void
*/
void CX2GUUser::ActiveSkillShow( D3DXVECTOR3 bonePos, const WCHAR* pBackFaceTexName, D3DXVECTOR2 size, D3DXVECTOR3 pos, bool bHyper, bool bOnlyLight )
{
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		return;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	if( bOnlyLight == false )
	{
		pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"FocusLine", D3DXVECTOR3(0,0,0) );
		if( GetIsRight() == true )
		{
			pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"OutLine", D3DXVECTOR3(0,0,0) );
			CKTDGParticleSystem::CParticleEventSequence* pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Right", 0,0,0 );
			if( pPart != NULL )
			{
				pPart->ChangeTexForce( pBackFaceTexName );

				CKTDGParticleSystem::CParticleEventList::iterator iter =
					( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).begin();
				CKTDGParticleSystem::CParticleEventList::iterator iterEnd =
					( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).end();

				if ( iter == iterEnd )  goto out;
				iter++; if ( iter == iterEnd ) goto out;
				CKTDGParticleSystem::CParticleEvent_Size* pESize = (CKTDGParticleSystem::CParticleEvent_Size*) (*iter);
				pESize->SetSize( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(size.x,size.y,1), D3DXVECTOR3(size.x,size.y,1) ));

				iter++; if ( iter == iterEnd ) goto out;
				CKTDGParticleSystem::CParticleEvent_Position* pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(pos.x-size.x, pos.y, 0),D3DXVECTOR3(pos.x-size.x, pos.y, 0) ) );

				iter++; if ( iter == iterEnd ) goto out;
				pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(pos.x, pos.y, 0),D3DXVECTOR3(pos.x, pos.y, 0) ) );

				iter++; if ( iter == iterEnd ) goto out;
				pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024+size.x, pos.y, 0),D3DXVECTOR3(1024+size.x, pos.y, 0) ) );
			}

out:
			pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Talk_Right", 0,0,0 );
			if( pPart != NULL )
			{
				wstringstream str;
				str << GetCutInFileName(true) << L".dds";
				pPart->ChangeTexForce( str.str().c_str() );
			}
		}
		else
		{
			pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"OutLine_Left", D3DXVECTOR3(0,0,0) );
			CKTDGParticleSystem::CParticleEventSequence* pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Left", 0,0,0 );
			if( pPart != NULL )
			{
				pPart->ChangeTexForce( pBackFaceTexName );

				CKTDGParticleSystem::CParticleEventList::iterator iter =
					( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).begin();
				CKTDGParticleSystem::CParticleEventList::iterator iterEnd =
					( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).end();

				if ( iter == iterEnd )  goto out2;
				iter++; if ( iter == iterEnd ) goto out2;
				CKTDGParticleSystem::CParticleEvent_Size* pESize = (CKTDGParticleSystem::CParticleEvent_Size*) (*iter);
				pESize->SetSize( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(size.x,size.y,1), D3DXVECTOR3(size.x,size.y,1) ));

				iter++; if ( iter == iterEnd ) goto out2;
				CKTDGParticleSystem::CParticleEvent_Position* pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024+size.x, pos.y, 0),D3DXVECTOR3(1024+size.x, pos.y, 0) ) );

				iter++; if ( iter == iterEnd ) goto out2;
				pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024-pos.x, pos.y, 0),D3DXVECTOR3(1024-pos.x, pos.y, 0) ) );

				iter++; if ( iter == iterEnd ) goto out2;
				pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
				pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(-size.x, pos.y, 0),D3DXVECTOR3(-size.x, pos.y, 0) ) );
			}

out2:
			pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Talk_Left", 0,0,0 );
			if( pPart != NULL )
			{
				wstringstream str;
				str << GetCutInFileName(false) << L".dds";
				pPart->ChangeTexForce( str.str().c_str() );
			}
		}

	}
}

bool CX2GUUser::CheckDashAngleChangeTime()
{
	if( g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON && g_pX2Game->GetGameType() != CX2Game::GT_BATTLE_FIELD )
		return false;

	if ( GetUnit() != NULL )
	{

		if ( m_FrameDataNow.syncData.nowState == USI_DASH ||
			m_FrameDataNow.syncData.nowState == USI_DASH_JUMP ||
			m_FrameDataNow.syncData.nowState == USI_DASH_JUMP_LANDING ||
			m_FrameDataNow.syncData.nowState == USI_WALK )
		{
			return true;
		}

		//{{ 2009.01.19 ���¿� : �ڵ����� elseif -> switch
		switch( GetUnit()->GetUnitTemplet()->m_UnitType )
		{
		case CX2Unit::UT_ARME:
			{
				if ( m_FrameDataNow.syncData.nowState == CX2GUArme_VioletMage::AVSI_DASH_JUMP_LEVITATION )
				{
					return true;
				}
			} break;

		default:
			break;

		}//}} elseif -> switch
	}

	return false;
}

float CX2GUUser::GetDashCameraAngleDegree()
{
	if ( g_pMain->GetGameOption().GetCameraDistance() <= 900 )
	{
		return DASH_CAMERA_ANGLE;
	}
	else if ( g_pMain->GetGameOption().GetCameraDistance() <= 1200 && g_pMain->GetGameOption().GetCameraDistance() > 900 )
	{
		return DASH_CAMERA_ANGLE - 20;
	}
	else if ( g_pMain->GetGameOption().GetCameraDistance() > 1200 )
	{
		return DASH_CAMERA_ANGLE - 25;
	}

	return 0;
}

float CX2GUUser::GetBasicReducedDamagePercent()
{
	float fReducedDamagePer = 0;

	//��Ʈ �ɼ��� �������� ���ҵǴ� ������ �ɼ��� �ִ��� Ȯ��.
	for ( int optionIndex = 0; optionIndex < (int)m_vecSetItemOptions.size(); optionIndex++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecSetItemOptions[optionIndex] );
		if ( pSocketData != NULL )
		{
			if ( pSocketData->m_fDamageReduce > 0.0f && pSocketData->m_fPercentDamageReduce > GetRandomFloat() )
			{
				if ( pSocketData->m_bDungeonOnly == true )
				{
					if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
					{
						fReducedDamagePer += pSocketData->m_fDamageReduce;
						return fReducedDamagePer;
					}
				}
				else if ( pSocketData->m_bPVPOnly == true )
				{
					if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
					{
						fReducedDamagePer += pSocketData->m_fDamageReduce;
						return fReducedDamagePer;
					}
				}
				else
				{
					fReducedDamagePer += pSocketData->m_fDamageReduce;
					return fReducedDamagePer;
				}
			}
		}
	}

//#ifdef TITLE_SYSTEM
	for ( int optionIndex = 0; optionIndex < (int)m_vecTitleOptions.size(); optionIndex++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecTitleOptions[optionIndex] );
		if ( pSocketData != NULL )
		{
			if ( pSocketData->m_fDamageReduce > 0.0f && pSocketData->m_fPercentDamageReduce > GetRandomFloat() )
			{
				if ( pSocketData->m_bDungeonOnly == true )
				{
					if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
					{
						fReducedDamagePer += pSocketData->m_fDamageReduce;
						return fReducedDamagePer;
					}
				}
				else if ( pSocketData->m_bPVPOnly == true )
				{
					if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
					{
						fReducedDamagePer += pSocketData->m_fDamageReduce;
						return fReducedDamagePer;
					}
				}
				else
				{
					fReducedDamagePer += pSocketData->m_fDamageReduce;
					return fReducedDamagePer;
				}
			}
		}
	}
//#endif

	return fReducedDamagePer;
}


float CX2GUUser::GetReducedDamagePercentByMonsterAttack( int monsterID )
{
	float fReducedDamagePer = 0;


	//��Ʈ �ɼ��� �������� Ư�� ���Ϳ����� ���ҵǴ� ������ �ɼ��� �ִ��� Ȯ��.
	for ( int optionIndex = 0; optionIndex < (int)m_vecSetItemOptions.size(); optionIndex++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecSetItemOptions[optionIndex] );
		if ( pSocketData != NULL )
		{
			// Ư�� ���� ������ ���� �ɼ� ����ȵǴ� ���� ����
			//if ( pSocketData->m_fDamageReduceByMonsterAttack > 0.0f && pSocketData->m_fPercentDRByMonsterAttack <= GetRandomFloat() )
			if ( pSocketData->m_fDamageReduceByMonsterAttack > 0.0f && pSocketData->m_fPercentDRByMonsterAttack > GetRandomFloat() )
			{
				set<int>::iterator sit = pSocketData->m_setMonsterIDDamageReduced.find( monsterID );
				if ( sit != pSocketData->m_setMonsterIDDamageReduced.end() )
				{
					if ( pSocketData->m_bDungeonOnly == true )
					{
						if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
						{
							fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
							return fReducedDamagePer;
						}
					}
					else if ( pSocketData->m_bPVPOnly == true )
					{
						if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON )
						{
							fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
							return fReducedDamagePer;
						}
					}
					else
					{
						fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
						return fReducedDamagePer;
					}
				}
			}
		}
	}

//#ifdef TITLE_SYSTEM
	for ( int optionIndex = 0; optionIndex < (int)m_vecTitleOptions.size(); optionIndex++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecTitleOptions[optionIndex] );
		if ( pSocketData != NULL )
		{
			// Ư�� ���� ������ ���� �ɼ� ����ȵǴ� ���� ����
			//if ( pSocketData->m_fDamageReduceByMonsterAttack > 0.0f && pSocketData->m_fPercentDRByMonsterAttack <= GetRandomFloat() )
			if ( pSocketData->m_fDamageReduceByMonsterAttack > 0.0f && pSocketData->m_fPercentDRByMonsterAttack > GetRandomFloat() )
			{
				set<int>::iterator sit = pSocketData->m_setMonsterIDDamageReduced.find( monsterID );
				if ( sit != pSocketData->m_setMonsterIDDamageReduced.end() )
				{
					if ( pSocketData->m_bDungeonOnly == true )
					{
						if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
						{
							fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
							return fReducedDamagePer;
						}
					}
					else if ( pSocketData->m_bPVPOnly == true )
					{
						if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
						{
							fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
							return fReducedDamagePer;
						}
					}
					else
					{
						fReducedDamagePer += pSocketData->m_fDamageReduceByMonsterAttack;
						return fReducedDamagePer;
					}
				}
			}
		}
	}
//#endif


	return fReducedDamagePer;
}

/*virtual*/ float CX2GUUser::GetHyperModeTime() const
{
	float fTotalTime
		= ( GetForceHyperModeTime() > 0.0f ? GetForceHyperModeTime() : GetMaxHyperModeTime() * GetLastStateHyperModeCount() );
	// �⺻�����ð�(1�ʰ����ð�*������������) �� m_fHyperModeTime �ۼ�Ʈ ��ŭ �����ش�.
	fTotalTime += fTotalTime * m_SocketData.m_fHyperModeTime * 0.01f;	/// �簢������ ���� �ȵ�..

	return fTotalTime;
}






/*virtual*/ float CX2GUUser::GetDamageUpByAMADS()
{
	// 2010.01.13, dmlee - �ɼ��� ���� return �ϴ� �Ϳ���, �ɼ��߿��� �ְ����� return �ϴ� ������ ���� - ������
	float fMaxDamageUp = 0.f;

	float fDamageUp = 0.f;


	// fix!! ������ ���� �����̳� ������ ���� ���Ͽ� ���� �κ��� �ϴ� ��������. ���߿� �ʿ��ϸ� �߰��ؾ� ��





	//��Ʈ�����ۿ����� ��ų �ö󰡴� �༮�� �ִ��� �˻��غ���.
	for ( int i = 0; i < (int)m_vecSetItemOptions.size(); i++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecSetItemOptions[i] );
		if ( pSocketData != NULL )
		{
			if ( pSocketData->m_bDungeonOnly == true )
			{
				if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
				{
					fDamageUp += pSocketData->m_fDamageUpByAMADS;

					if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
					{
						fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
					}

				}
			}
			else if ( pSocketData->m_bPVPOnly == true )
			{
				if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
				{
					fDamageUp += pSocketData->m_fDamageUpByAMADS;

					if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
					{
						fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
					}

				}
			}
			else
			{
				fDamageUp += pSocketData->m_fDamageUpByAMADS;

				if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
				{
					fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
				}
			}
		}
	}

	for ( int i = 0; i < (int)m_vecTitleOptions.size(); i++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecTitleOptions[i] );
		if ( pSocketData != NULL )
		{
			if ( pSocketData->m_bDungeonOnly == true )
			{
				if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
				{
					fDamageUp += pSocketData->m_fDamageUpByAMADS;

					if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
					{
						fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
					}

				}
			}
			else if ( pSocketData->m_bPVPOnly == true )
			{
				if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
				{
					fDamageUp += pSocketData->m_fDamageUpByAMADS;


					if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
					{
						fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
					}

				}
			}
			else
			{
				fDamageUp += pSocketData->m_fDamageUpByAMADS;

				if( pSocketData->m_fDamageUpByAMADS > fMaxDamageUp )
				{
					fMaxDamageUp = pSocketData->m_fDamageUpByAMADS;
				}
			}
		}
	}


	//return fDamageUp;


	return fMaxDamageUp;

}







void CX2GUUser::GetDamageReduceByResistExtraDamage( CX2DamageManager::EXTRA_DAMAGE_TYPE damageType, float& fFinalDamage )
{
	float fResist = 0.0f;


	// ��Ʈ ������ ���� �ɼ�
	for ( int i = 0; i < (int)m_vecSetItemOptions.size(); i++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecSetItemOptions[i] );
		if( NULL == pSocketData )
			continue;

		if( pSocketData->m_bDungeonOnly == true &&
			( g_pX2Game == NULL || ( NULL != g_pX2Game && ( g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON && g_pX2Game->GetGameType() != CX2Game::GT_BATTLE_FIELD ) ) ) )
			continue;


		if( pSocketData->m_bPVPOnly == true &&
			( g_pX2Game == NULL || ( NULL != g_pX2Game && g_pX2Game->GetGameType() != CX2Game::GT_PVP ) ) )
			continue;


		if( pSocketData->m_ResistExtraDamageType != damageType )
		{
			continue;
		}

		fResist += pSocketData->m_fResistExtraDamage;
	}

//#ifdef TITLE_SYSTEM
	// Īȣ ���Ͽɼ�
	for ( int i = 0; i < (int)m_vecTitleOptions.size(); i++ )
	{
		const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( m_vecTitleOptions[i] );

		if( NULL == pSocketData )
			continue;

		if( pSocketData->m_bDungeonOnly == true &&
			( g_pX2Game == NULL || (NULL != g_pX2Game && ( g_pX2Game->GetGameType() != CX2Game::GT_DUNGEON && g_pX2Game->GetGameType() != CX2Game::GT_BATTLE_FIELD ) ) ) )
			continue;


		if( pSocketData->m_bPVPOnly == true &&
			( g_pX2Game == NULL || (NULL != g_pX2Game && g_pX2Game->GetGameType() != CX2Game::GT_PVP ) ) )
			continue;


		if( pSocketData->m_ResistExtraDamageType != damageType )
		{
			continue;
		}

		fResist += pSocketData->m_fResistExtraDamage;
	}
//#endif

	float fValue = (1.0f - fResist);
	if ( fValue <= 0.0f )
	{
		fFinalDamage = 1.0f;
	}
	else
	{
		fFinalDamage *= fValue;
	}
}



void CX2GUUser::AnimStop()
{
	if ( m_pXSkinAnimFuture != NULL )
		m_pXSkinAnimFuture->Wait();

	if ( m_pXSkinAnim != NULL )
		m_pXSkinAnim->Wait();

	m_FrameDataNow.stateParam.bSuperArmor = true;
	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}

void CX2GUUser::AnimPlay()
{
	if ( m_pXSkinAnimFuture != NULL )
		m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_LOOP );

	if ( m_pXSkinAnim != NULL )
		m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );

	m_FrameDataNow.stateParam.bSuperArmor = false;
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}



void CX2GUUser::ProcessEnchantedWeaponEffectAtHand( CX2Unit::UNIT_TYPE unitType )
{
	KTDXPROFILE();


	if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hSeqWeaponCommon1 )	// �� ��ƼŬ�� ������ �տ� �ٴ� ��ƼŬ�� ���ٰ� ���� ��
		return;

	if( m_pXSkinAnim == NULL )
		return;

	D3DXVECTOR3 vHandPos( 0, 0, 0 );
	float aEmitRate[4] = { 0.f, };
	switch( unitType )
	{
	case CX2Unit::UT_ELSWORD:
	case CX2Unit::UT_ARME:
	case CX2Unit::UT_RAVEN:
		//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
	case CX2Unit::UT_CHUNG:
		//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG
		{
			vHandPos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );
			aEmitRate[0] = 10.f;
			aEmitRate[1] = 30.f;
			aEmitRate[2] = 60.f;
			aEmitRate[3] = 25.f;
		} break;

	case CX2Unit::UT_LIRE:
		{
			vHandPos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			aEmitRate[0] = 15.f;
			aEmitRate[1] = 30.f;
			aEmitRate[2] = 60.f;
			aEmitRate[3] = 25.f;
		} break;

	case CX2Unit::UT_EVE:
		{
			vHandPos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			aEmitRate[0] = 10.f;
			aEmitRate[1] = 10.f;
			aEmitRate[2] = 10.f;
			aEmitRate[3] = 25.f;
		} break;

	case CX2Unit::UT_ARA:
	case CX2Unit::UT_ELESIS:
		{
			vHandPos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy3_Weapon" );
			aEmitRate[0] = 15.f;
			aEmitRate[1] = 30.f;
			aEmitRate[2] = 60.f;
			aEmitRate[3] = 25.f;
		} break;
	case CX2Unit::UT_ADD:
		{
			vHandPos = m_pXSkinAnim->GetCloneFramePosition( L"Weapon02" );
			aEmitRate[0] = 15.f;
			aEmitRate[1] = 30.f;
			aEmitRate[2] = 60.f;
			aEmitRate[3] = 25.f;
		} break;
	}


	CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire1 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon1 );
	CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire2 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon2 );
	CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire3 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon3 );
	CKTDGParticleSystem::CParticleEventSequence* pSeqWeaponFire4 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchantedWeaponCommon4 );


	/// Ż���� Ÿ�� ������, ���� ��þƮ ȿ�� ��� ������.
	if ( NULL != CX2RidingPetManager::GetInstance() && true != CX2RidingPetManager::GetInstance()->GetRidingOn() )
	{
		if ( pSeqWeaponFire1 != NULL )
		{
			pSeqWeaponFire1->SetPosition( vHandPos );
			pSeqWeaponFire1->SetEmitRate( aEmitRate[0], aEmitRate[0] );
		}

		if ( pSeqWeaponFire2 != NULL )
		{
			pSeqWeaponFire2->SetPosition( vHandPos );
			pSeqWeaponFire2->SetEmitRate( aEmitRate[1], aEmitRate[1] );
		}

		if ( pSeqWeaponFire3 != NULL )
		{
			pSeqWeaponFire3->SetPosition( vHandPos );
			pSeqWeaponFire3->SetEmitRate( aEmitRate[2], aEmitRate[2] );
		}

		if ( pSeqWeaponFire4 != NULL )
		{
			pSeqWeaponFire4->SetPosition( vHandPos );
			pSeqWeaponFire4->SetEmitRate( aEmitRate[3], aEmitRate[3] );
		}
	}
	else
	{
		if ( pSeqWeaponFire1 != NULL )
		{
			pSeqWeaponFire1->ClearAllParticle();
			pSeqWeaponFire1->SetEmitRate( 0, 0 );
		}

		if ( pSeqWeaponFire2 != NULL )
		{
			pSeqWeaponFire2->ClearAllParticle();
			pSeqWeaponFire2->SetEmitRate( 0, 0 );
		}

		if ( pSeqWeaponFire3 != NULL )
		{
			pSeqWeaponFire3->ClearAllParticle();
			pSeqWeaponFire3->SetEmitRate( 0, 0 );
		}

		if ( pSeqWeaponFire4 != NULL )
		{
			pSeqWeaponFire4->ClearAllParticle();
			pSeqWeaponFire4->SetEmitRate( 0, 0 );
		}
	}

}


/*static*/
void	CX2GUUser::AppendEnchantedWeaponEffectAtHandToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{

	switch( eDamageType_ )
	{
	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Wind01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Wind01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Wind01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Triple_Weapon_Property_Wind01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Light01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Light01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Light01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Triple_Weapon_Property_Light01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Curse01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Curse01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		{
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property02_M_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property03_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Weapon_Elsword_Property04_Curse01" );
			g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"Triple_Weapon_Property_Curse01"  );
		} break;
	}
}



void CX2GUUser::InitEnchantedWeaponEffectAtHand()
{
 	// attribute postfix�� ���� ����Ʈ ����

	switch( GetEnchantExtraDamageType() )
	{
	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_Wind01",
				L"Weapon_Elsword_Property03_Wind01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Wind01",
				L"Weapon_Elsword_Property03_Wind01",
				L"Weapon_Elsword_Property04_Wind01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Wind01",
				L"Weapon_Elsword_Property03_Wind01",
				L"Weapon_Elsword_Property04_Wind01",
				L"Triple_Weapon_Property_Wind01" );
		} break;



	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_Light01",
				L"Weapon_Elsword_Property03_Light01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Light01",
				L"Weapon_Elsword_Property03_Light01",
				L"Weapon_Elsword_Property04_Light01" );

		} break;


	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SHOCK:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Light01",
				L"Weapon_Elsword_Property03_Light01",
				L"Weapon_Elsword_Property04_Light01",
				L"Triple_Weapon_Property_Light01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_Curse01",
				L"Weapon_Elsword_Property03_Curse01" );
		} break;

	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Curse01",
				L"Weapon_Elsword_Property03_Curse01",
				L"Weapon_Elsword_Property04_Curse01" );
		} break;


	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_SNATCH:
		{
			CreateEnchantedWeaponParticleAtHand( L"Weapon_Elsword_Property02_M_Curse01",
				L"Weapon_Elsword_Property03_Curse01",
				L"Weapon_Elsword_Property04_Curse01",
				L"Triple_Weapon_Property_Curse01" );
		} break;
	}
}






void CX2GUUser::SetShowEnchantedWeaponEffectAtHand( bool bShow )
{
	CKTDGParticleSystem::CParticleEventSequence* pSeqWeapon = NULL;
	pSeqWeapon = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon1 );
	if( pSeqWeapon != NULL )
		pSeqWeapon->SetShowObject(bShow);
	pSeqWeapon = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon2 );
	if( pSeqWeapon != NULL )
		pSeqWeapon->SetShowObject(bShow);
	pSeqWeapon = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqWeaponCommon3 );
	if( pSeqWeapon != NULL )
		pSeqWeapon->SetShowObject(bShow);
	pSeqWeapon = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchantedWeaponCommon4 );
	if( pSeqWeapon != NULL )
		pSeqWeapon->SetShowObject(bShow);
}


bool CX2GUUser::IsImmuneBySocketOption( CX2DamageManager::EXTRA_DAMAGE_TYPE exDamageType )
{
	map< CX2DamageManager::EXTRA_DAMAGE_TYPE, float >::iterator mit = m_mapExtraDamageNImmunePer.find( exDamageType );
	if ( mit != m_mapExtraDamageNImmunePer.end() )
	{
		if ( mit->second > GetRandomFloat( CKTDXRandomNumbers::SRO_IMMUNE_EXTRA_DAMAGE_SCRIPTED ) )
		{
			return true;
		}
	}

	return false;
}

void CX2GUUser::PEPPER_RUN_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.3f )
	{
		StateChange( USI_PEPPER_RUN );
		m_TimerPepperRun.restart();
	}

	CommonEventProcess();
}

void CX2GUUser::PEPPER_RUN_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
	}

	m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.5f;

	CommonFrameMoveFuture();
}

void CX2GUUser::PEPPER_RUN_EventProcess()
{
	if( 0.f != m_fPepperRunTime &&
		m_TimerPepperRun.elapsed() > m_fPepperRunTime )
	{
		m_fPepperRunTime = 0.f;
		StateChange( USI_PEPPER_RUN_END );
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( USI_PEPPER_RUN_JUMP_UP );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_PEPPER_RUN_JUMP_DOWN );
	}

	CommonEventProcess();
}


void CX2GUUser::PEPPER_RUN_END_FrameMove()
{
	CommonFrameMove();
}

void CX2GUUser::PEPPER_RUN_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
		{
			StateChange( m_CommonState.m_Wait );
		}
	}

	CommonEventProcess();
}

void CX2GUUser::PEPPER_RUN_JUMP_UP_StateStartFuture()
{
	switch( GetUnit()->GetType() )
	{
	default:
		{
			m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
		} break;

	case CX2Unit::UT_EVE:
		{
			m_PhysicParam.nowSpeed.y = 2300.f;
		} break;
	}

	m_FrameDataFuture.syncData.position.y += LINE_RADIUS;

}

void CX2GUUser::PEPPER_RUN_JUMP_UP_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
	}

	m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.5f;

	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::PEPPER_RUN_JUMP_UP_EventProcess()
{
	if( 0.f != m_fPepperRunTime &&
		m_TimerPepperRun.elapsed() > m_fPepperRunTime )
	{
		m_fPepperRunTime = 0.f;
		StateChange( USI_PEPPER_RUN_END );
	}
	else if( m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		StateChange( USI_PEPPER_RUN_JUMP_DOWN );
	}

	CommonEventProcess();
}

void CX2GUUser::PEPPER_RUN_JUMP_DOWN_StateStartFuture()
{
	CommonStateStartFuture();

	m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

}

void CX2GUUser::PEPPER_RUN_JUMP_DOWN_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
	}

	m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.5f;

	if( m_InputData.pureDown == true  && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::PEPPER_RUN_JUMP_DOWN_EventProcess()
{
	if( 0.f != m_fPepperRunTime &&
		m_TimerPepperRun.elapsed() > m_fPepperRunTime )
	{
		m_fPepperRunTime = 0.f;
		StateChange( USI_PEPPER_RUN_END );
	}
	else if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_PEPPER_RUN );
	}
	else if( m_PhysicParam.nowSpeed.y >= 0.0f )
	{
		StateChange( USI_PEPPER_RUN_JUMP_UP );
	}

	CommonEventProcess();
}


void CX2GUUser::StartPepperRun( float fTime )
{
	m_fPepperRunTime *= 1.1f;
	m_fPepperRunTime += fTime;

	StateChange( USI_PEPPER_RUN_READY, true );
}

/*virtual*/ void CX2GUUser::DAMAGE_GROGGY_EventProcess()
{
	if ( m_vecStunPtr.empty() )
	{
		StateChange( m_CommonState.m_Wait );
	}

	CommonEventProcess();
}

void CX2GUUser::THROW_ITEM_StateStart()
{
	m_bThrowSuccess = false;
	CommonStateStart();
}

void CX2GUUser::THROW_ITEM_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUUser::THROW_ITEM_CameraMove()
{
	CommonCameraMove();
}

void CX2GUUser::THROW_ITEM_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_CommonState.m_Wait );
	}
	CommonEventProcess();
}

void CX2GUUser::THROW_ITEM_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fThrowStartTime ) == true )
	{
		D3DXVECTOR3 vEffectPos = ( CX2Item::SST_CHILDRENSDAY_SHPERE_03 != m_iThrowItemValue ? GetPos() : GetLandPosition() );


		D3DXVECTOR3 vRotate = GetRotateDegree();

		//vEffectPos = GetPos();
		vEffectPos.y += m_vThrowStartPosOffset.y;

		if(GetIsRight() == true)
			vEffectPos += ( m_vThrowStartPosOffset.x * GetDirVector() );
		else
			vEffectPos -= ( m_vThrowStartPosOffset.x * GetDirVector() );

		CX2DamageEffect::CEffect *pEffect = NULL;

		m_bThrowSuccess = true;
		switch( m_iThrowItemValue )
		{
		case CX2Item::SST_WOODEN_SPEAR:
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WOODEN_SPEAR", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_LIGHT_SPEAR:
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_LIGHT_SPEAR", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_FIRE_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_FIRE_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_WATER_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WATER_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_NATURE_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_NATURE_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_WIND_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WIND_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_LIGHT_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_LIGHT_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_DARK_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_DARK_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;

		case CX2Item::SST_CHILDRENSDAY_SHPERE_01:	/// �г��� ���ź
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_FIRE_SPHERE_CHILDRENS_DAY", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_CHILDRENSDAY_SHPERE_02:	/// ������ ���ź
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WATER_SPHERE_CHILDRENS_DAY", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_CHILDRENSDAY_SHPERE_03:	/// ����� ���ź
			{
				vEffectPos.y -= 100;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_NUCLEAR_SPHERE_ITEM_CHILDRENS_DAY", this, NULL, false, GetPowerRate(), -1.f, D3DXVECTOR3( 1, 1, 1 ),
					true, vEffectPos );
			} break;

		case CX2Item::SST_CHILDRENSDAY_SHPERE_04:	/// �ູ�� ���ź
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_LIGHT_SPHERE_CHILDRENS_DAY", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;

		case CX2Item::SST_ANGER_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_ANGER_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_CHINA_EVENT_SPEAR:
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVENT_SPECIAL_ITEM_LIGHT_SPEAR", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_FIRE_CRACKER:
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_FIRE_CRACKER", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_GOOD_FIRE_CRACKER:
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_GOOD_FIRE_CRACKER", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		default:
			break;
		}

		if( pEffect != NULL )
		{
			m_vThrowStartVelocity.x = m_fWalkCancelAfter;
			m_vThrowStartVelocity.y = m_fDashCancelAfter;
            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
			    pMeshInstance->SetVelocity( m_vThrowStartVelocity );
		}

	}

	CommonFrameMove();
}

void CX2GUUser::THROW_ITEM_StateEnd()
{
	if( m_bThrowSuccess == false )
	{
		D3DXVECTOR3 vEffectPos;
		D3DXVECTOR3 vRotate;

		vEffectPos = GetPos();
		vEffectPos.y += m_vThrowStartPosOffset.y;

		if(GetIsRight() == true)
			vEffectPos += ( m_vThrowStartPosOffset.x * GetDirVector() );
		else
			vEffectPos -= ( m_vThrowStartPosOffset.x * GetDirVector() );

		vRotate = GetRotateDegree();

		CX2DamageEffect::CEffect *pEffect = NULL;
		switch( m_iThrowItemValue )
		{
		case CX2Item::SST_FIRE_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_FIRE_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_WATER_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WATER_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_NATURE_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_NATURE_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_WIND_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_WIND_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_LIGHT_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_LIGHT_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_DARK_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_DARK_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		case CX2Item::SST_ANGER_SPHERE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECIAL_ITEM_ANGER_SPHERE", GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
			break;
		default:
			break;
		}

		if( pEffect != NULL )
		{
			m_vThrowStartVelocity.x = 0.f;
			m_vThrowStartVelocity.y = 200.f;
            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
			    pMeshInstance->SetVelocity( m_vThrowStartVelocity );
		}
	}

	CommonStateEnd();
}



//MACHINE_GUN_RIDE
void CX2GUUser::MACHINE_GUN_RIDE_StartFuture()
{

	m_pOrgMotionFuture		= m_pMotionFuture;

	m_pOrgXSkinAnimFuture	= m_pXSkinAnimFuture;


	m_pOrgXSkinAnimFuture->SetShowObject( false );


	m_pMotionFuture		= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Motion_BunkerCannon.x" );


	m_pXSkinAnimFuture	= CKTDGXSkinAnim::CreateSkinAnim();


	m_pXSkinAnimFuture->SetAnimXSkinMesh( m_pMotionFuture );
	m_pXSkinAnimFuture->AddModelXSkinMesh( m_pMotionFuture, NULL, NULL, NULL );
	m_pXSkinAnimFuture->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

	CommonStateStartFuture();

}

void CX2GUUser::MACHINE_GUN_RIDE_Start()
{

	m_pOrgMotion			= m_pMotion;

	m_pOrgXSkinAnim			= m_pXSkinAnim;


	m_pOrgXSkinAnim->SetShowObject( false );


	m_pMotion			= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Motion_BunkerCannon.x" );

	m_pXSkinAnim		= CKTDGXSkinAnim::CreateSkinAnimPtr();

	m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
	m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, NULL, NULL, NULL );
	m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );


	m_CollisionListSet.clear();
	m_AttackListSet.clear();

	m_CollisionListSet.insert( &m_pXSkinAnim->GetCollisionDataList() );
	m_AttackListSet.insert( &m_pXSkinAnim->GetAttackDataList() );

	for( int i =0; i < (int)m_ViewEqipList.size(); i++ )
	{
		//CKTDGXSkinAnim* pAnim = NULL;
		CX2EqipPtr pEqip = m_ViewEqipList[i];
		if ( pEqip == NULL )
			continue;

		//{{ robobeg : 2008-10-18
		//pEqip->SetShow( false );
		pEqip->SetShowObject( false );
		//}} robobeg : 2008-10-18
	}

	m_pFrame_Bip01_Head			= m_pXSkinAnim->GetCloneFrame( L"Cannon_Head" );


	m_bRidingMachine = true;

	CommonStateStart();

}

void CX2GUUser::MACHINE_GUN_RIDE_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_RIDE_FrameMove()
{
	CommonFrameMove();
}

void CX2GUUser::MACHINE_GUN_RIDE_CameraMove()
{
	CommonCameraMove();
}

void CX2GUUser::MACHINE_GUN_RIDE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_MachineGunWaitState );
	}
	CommonEventProcess();
}

//MACHINE_GUN_LEAVE
void CX2GUUser::MACHINE_GUN_LEAVE_StartFuture()
{


	SAFE_CLOSE( m_pMotionFuture );

	CKTDGXSkinAnim::DeleteKTDGObject( m_pXSkinAnimFuture );
	m_pXSkinAnimFuture = NULL;


	m_pMotionFuture		= m_pOrgMotionFuture;
	m_pOrgMotionFuture = NULL;

	m_pXSkinAnimFuture	= m_pOrgXSkinAnimFuture;
	m_pOrgXSkinAnimFuture = NULL;

	m_pXSkinAnimFuture->SetShowObject( true );

	CommonStateStartFuture();
}

void CX2GUUser::MACHINE_GUN_LEAVE_Start()
{
	SAFE_CLOSE( m_pMotion );

	//CKTDGXSkinAnim::DeleteKTDGObject( m_pXSkinAnim );
	//m_pXSkinAnim = NULL;
	m_pXSkinAnim.reset();


	m_pMotion		= m_pOrgMotion;
	m_pOrgMotion = NULL;

	m_pXSkinAnim	= m_pOrgXSkinAnim;
	//m_pOrgXSkinAnim = NULL;


	m_pXSkinAnim->SetShowObject( true );


	//{{ kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
	UpdateEquipCollisionData( m_ViewEqipList );
	//}} kimhc // 2010.12.10 //  2010-12-23 New Character CHUNG

	for( int i =0; i < (int)m_ViewEqipList.size(); i++ )
	{
		//CKTDGXSkinAnim* pAnim = NULL;
		CX2EqipPtr pEqip = m_ViewEqipList[i];
		if ( pEqip == NULL )
			continue;
		//{{ robobeg : 2008-10-18
		//pEqip->SetShow( true );
		pEqip->SetShowObject( true );
		//}} robobeg : 2008-10-18
	}


	m_pFrame_Bip01_Head			= m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );

	m_bRidingMachine = false;

	CommonStateStart();
}

void CX2GUUser::MACHINE_GUN_LEAVE_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_LEAVE_FrameMove()
{
	CommonFrameMove();
}

void CX2GUUser::MACHINE_GUN_LEAVE_CameraMove()
{
	CommonCameraMove();
}

void CX2GUUser::MACHINE_GUN_LEAVE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_CommonState.m_Wait );
	}
	CommonEventProcess();
}

//MACHINE_GUN_WAIT
void CX2GUUser::MACHINE_GUN_WAIT_StartFuture()
{
	CommonStateStartFuture();
}

void CX2GUUser::MACHINE_GUN_WAIT_Start()
{
	CommonStateStart();
}

void CX2GUUser::MACHINE_GUN_WAIT_FrameMoveFuture()
{
	//GetGageManager()->GetAttackDelayGage()->fNow -= 10.0f * m_fElapsedTime;
	//if( GetGageManager()->GetAttackDelayGage()->fNow < 0.0f )
	//	GetGageManager()->GetAttackDelayGage()->fNow = 0.0f;

	//GetGageManager()->GetForceDownGage()->fNow -= 10.0f * m_fElapsedTime;
	//if( GetGageManager()->GetForceDownGage()->fNow < 0.0f )
	//	GetGageManager()->GetForceDownGage()->fNow = 0.0f;

	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_WAIT_EventProcess()
{
	/*
	if( false == IsOnSomethingFuture() )
	{
	m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
	//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_WALK );
	}
	else */if( m_InputData.pureUp == true )
	{
		//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_UP );
		//m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		//m_FrameDataFuture.syncData.position.y += LINE_RADIUS;

		m_PhysicParam.nowSpeed.y = GetDashSpeed();
	}
	else if( m_InputData.oneZ == true  )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_ATTACK );
	}
	else if( /*m_InputData.oneDown*/ m_InputData.pureDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{

		//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_DOWN );
		if ( true == IsOnSomethingFuture() )
		{
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}


		m_PhysicParam.nowSpeed.y = -( GetDashSpeed() );
	}

	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
	}

	if ( m_InputData.pureUp == false && m_InputData.pureDown == false )
	{
		m_PhysicParam.nowSpeed.y = 0;
	}

	CommonEventProcess();
}

//MACHINE_GUN_WALK
void CX2GUUser::MACHINE_GUN_WALK_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetJumpSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetJumpSpeed();
	}
	else
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_WALK_EventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( m_InputData.oneZ == true  )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_ATTACK );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	CommonEventProcess();
}

//MACHINE_GUN_ATTACK
void CX2GUUser::MACHINE_GUN_ATTACK_FrameMoveFuture()
{
	//m_PhysicParam.nowSpeed.x = 0.0f;
	//m_PhysicParam.nowSpeed.y = 0.0f;

	/*
	if( m_InputData.pureRight == true )
	{
	m_FrameDataFuture.syncData.bIsRight = true;
	m_PhysicParam.nowSpeed.x = GetJumpSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
	m_FrameDataFuture.syncData.bIsRight = false;
	m_PhysicParam.nowSpeed.x = GetJumpSpeed();
	}
	else
	{
	m_PhysicParam.nowSpeed.x = 0.0f;
	}
	*/


	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_ATTACK_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.433f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.933f ) == true )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance_LUA( this, "MACHINE_GUN_STRAIGHT", GetBonePos( L"Dummy2_Cannon" ), GetLandPosition().y );


	}




    if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) == true  )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance_LUA( this, "MACHINE_GUN_STRAIGHT", GetBonePos( L"Dummy1_Cannon" ), GetLandPosition().y );

		//g_pX2Game->GetDamageEffect()->CreateInstance_LUA( this, "MORTAR_BOMB", GetBonePos( L"Cannon_Head" ), GetLandPosition().y );


		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance_LUA2( this, "WALLY_8TH_GUIDED_MISSILE", GetBonePos( L"Cannon_Head" ), GetLandPosition().y, GetRotateDegree() );

		SetLockOn( pEffect, 0 );

		D3DXVECTOR3 rotateValue = GetRotateDegree();
		rotateValue.y += 180.0f;

		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance_LUA2( this, "WALLY_8TH_GUIDED_MISSILE", GetBonePos( L"Cannon_Head" ), GetLandPosition().y, rotateValue );
		SetLockOn( pEffect, 1 );
		//g_pX2Game->GetDamageEffect()->CreateInstance_LUA2( this, "MORTAR_BOMB", GetBonePos( L"Cannon_Head" ), GetLandPosition().y, rotateValue );
	}

	CommonFrameMove();
}

void CX2GUUser::MACHINE_GUN_ATTACK_EventProcess()
{


	/*

	if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
	m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
	m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else
	{
	m_PhysicParam.nowSpeed.x = 0.0f;
	}

	if ( m_InputData.pureUp == false && m_InputData.pureDown == false )
	{
	m_PhysicParam.nowSpeed.y = 0;
	}

	*/

	/*
	if( false == IsOnSomethingFuture() )
	{
	m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
	//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_WALK );
	}
	else */if( m_InputData.pureUp == true )
	{
		//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_UP );
		//m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		//m_FrameDataFuture.syncData.position.y += LINE_RADIUS;

		m_PhysicParam.nowSpeed.y = GetDashSpeed();
	}
	else if( m_InputData.oneZ == true  )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_ATTACK );
	}
	else if( /*m_InputData.oneDown*/ m_InputData.pureDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{

		//StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_DOWN );
		if ( true == IsOnSomethingFuture() )
		{
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}


		m_PhysicParam.nowSpeed.y = -( GetDashSpeed() );
	}

	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
	}

	if ( m_InputData.pureUp == false && m_InputData.pureDown == false )
	{
		m_PhysicParam.nowSpeed.y = 0;
	}


	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.2f;
		StateChange( m_MachineGunWaitState );
	}

	CommonEventProcess();
}

void CX2GUUser::MACHINE_GUN_JUMP_UP_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_JUMP_UP_EventProcess()
{
	if( m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		StateChange( CX2GUElsword_SwordMan::ESSI_MACHINE_GUN_JUMP_DOWN );
	}

	CommonEventProcess();
}


void CX2GUUser::MACHINE_GUN_JUMP_DOWN_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( m_InputData.pureDown == true  && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}
	CommonFrameMoveFuture();
}

void CX2GUUser::MACHINE_GUN_JUMP_DOWN_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( m_MachineGunWaitState );
	}



	CommonEventProcess();
}

//SUMMON_MAGIC_SPEAR
void CX2GUUser::SUMMON_MAGIC_SPEAR_FrameMove()
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

	/*
	if( m_pXSkinAnim->EventTimer( 0.1f ) == true && EventCheck( 0.1f, false ) == true )
	{
	ActiveSkillShow( L"Dummy2_Lhand", L"HQ_CutIn_Elsword_2.dds", D3DXVECTOR2(670.0f,670.0f), D3DXVECTOR3(275.0f,471.0f,0.0f), false );
	}
	else*/
    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) == true )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LizardMan_Older_PowerBooster01", GetPos() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) == true )
	{
		D3DXVECTOR3 landPos = GetLandPosition();
		landPos.x = landPos.x + GetDirVector().x * 250.0f;
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ENT_SPIKE_BOARD", GetPowerRate(), landPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUUser::SUMMON_MAGIC_SPEAR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_CommonState.m_Wait );
	}
	CommonEventProcess();
}

/** @function : COMMON_BUFF_EventProcess
	@brief : ���������� ���� ���� ��ų�� EventProcess
*/
void CX2GUUser::COMMON_BUFF_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : StartEventProcess
	@brief : Start ������Ʈ���� ���� �ð��� ������ Wait ���·� ����
*/
void CX2GUUser::StartEventProcess()
{
	CommonEventProcess();

	const float EVENT_TIME = 4.5f;

	if( EVENT_TIME <= m_FrameDataFuture.unitCondition.fStateTime )
	{
		StateChange( USI_WAIT );
	}
}

/** @function : StartEnd
	@brief : �ڱ� ������ ��쿡�� �����ߴٴ� ��ƼŬ�� �����ְ�, ���� �ʱ�ȭ ��
*/
void CX2GUUser::StartEnd()
{
	CommonStateEnd();

	if( IsMyUnit() == true )
	{


		PlaySound( L"Fight.ogg" ,false);

		if( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
			g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameFight", 0,0,0, 1000, 1000, 1, 1 );
		else if( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON )
		{
			if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
			{
				CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);

				if( pDungeonGame != NULL &&
					pDungeonGame->GetDungeon() != NULL &&
					pDungeonGame->GetDungeon()->GetDungeonData() != NULL )
				{
					int iNowDungeonID = pDungeonGame->GetDungeon()->GetDungeonData()->m_DungeonID;

					/// ����� ���� �ƴ� ���� Start ���
					if( false == g_pData->GetDungeonManager()->IsDefenceDungeon(iNowDungeonID))
						g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"GameStart", 0,0,0, 1000, 1000, 1, 1 );

					g_pKTDXApp->GetDGManager()->ClearScreen( 10 );
				}
			}
		}

	}


	CommonStartStateEnd();

	//{{ �ӱԼ� �ӱԼ� ���� ��ŸƮ ǥ�� ������ �� , ���� ���� ���� �Ŀ� ������ ��� ���ϵ��� ����
	m_bStarted = true;
	//}}
}

/** @function : StartIntrudeStart
	@brief : ������ ���, ���Խ��� ������Ʈ���� ó������� �Ұ͵� ó��
*/
void CX2GUUser::StartIntrudeStart()
{
	CommonStateStart();

	CKTDGXMeshPlayer* pMinorMeshPlayer = g_pX2Game->GetMinorXMeshPlayer();

	pMinorMeshPlayer->CreateInstance( (CKTDGObject*) this,  L"RespawnLanding01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	pMinorMeshPlayer->CreateInstance( (CKTDGObject*) this,  L"RespawnLanding02", GetPos(), GetRotateDegree(), GetRotateDegree() );

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	pMinorParticleSystem->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P01", GetPos() );
	pMinorParticleSystem->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P02", GetPos() );

	D3DXVECTOR3 vDirVector = GetDirVector();
	pMinorParticleSystem->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P03", GetPos() + vDirVector * 10.f );

}


/** @function : StartIntrudeFrameMove
	@brief : ���Խ� ��ƼŬ ���
*/
void CX2GUUser::StartIntrudeFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.1f ) )
	{
		D3DXVECTOR3 vDirVector = GetDirVector();
		D3DXVECTOR3 vUpVector(0, 1, 0);

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", GetPos() + vDirVector * 10.f + vUpVector * 50.f );
	}


	if( true == m_pXSkinAnim->EventTimer( 0.26f ) )
	{
		D3DXVECTOR3 vDirVector = GetDirVector();
		D3DXVECTOR3 vUpVector(0, 1, 0);

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", GetPos() + vDirVector * 10.f + vUpVector * 100.f );
	}


	if( true == m_pXSkinAnim->EventTimer( 0.39f ) )
	{
		D3DXVECTOR3 vDirVector = GetDirVector();
		D3DXVECTOR3 vUpVector(0, 1, 0);

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", GetPos() + vDirVector * 10.f + vUpVector * 150.f );
	}


	if( true == m_pXSkinAnim->EventTimer( 0.52f ) )
	{
		D3DXVECTOR3 vDirVector = GetDirVector();
		D3DXVECTOR3 vUpVector(0, 1, 0);

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", GetPos() + vDirVector * 10.f + vUpVector * 200.f );
	}

	CommonFrameMove();
}

/** @function : StartIntrudeEventProcess
	@brief : ���� �ִϸ��̼��� ����Ǹ� Wait���� ���� ����
*/
void CX2GUUser::StartIntrudeEventProcess()
{
	CommonEventProcess();

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
}

/** @function : DieFrameMove
	@brief : front, back�� ������� �״� ������Ʈ�� FrameMove���� ���������� ����Ǿ�� �ϴ� �Լ�
	@param : float fTimeCreateStepDust_(�ٴڿ� ���⸦ �߻���Ű�� �ִϸ��̼� Ÿ��), float TimeSacrificeOfHero_(������ ����� ����Ǵ� �ִϸ��̼� Ÿ��)
*/
void CX2GUUser::DieFrameMove( float fTimeCreateStepDust_, float fTimeSacrificeOfHero_ )
{
	if( m_pXSkinAnim->IsAnimationEnd() == true )
	{
		// �׾����ϱ�, ���� ������ ��� ������ Ǯ����
		SetRemainHyperModeTime( 0.0f );
		SetAlphaObject( true );
		m_RenderParam.bAlphaBlend	= true;

		if( m_RenderParam.color.a > 0.0f )
		{
			m_RenderParam.color.a	-= 1.5f * m_fElapsedTime;
		}
		else
		{
			//{{ robobeg : 2008-10-21
			if ( GetShowObject() == true )
				//}} robobeg : 2008-10-21
			{
				//{{ robobeg : 2008-10-21
				SetShowObject( false );
				//}} robobeg : 2008-10-21
				SetAlphaObject( false );
				m_RenderParam.bAlphaBlend	= false;

				//{{ kimhc // 2009-12-29 // Die�� ���� �����尡 �������� ������ ��� RT_CARTOON_FADE_NONTEX�� ����
				m_bHaveSacrificeOfHero = false;
				m_RenderParam.renderType = CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
				//}} kimhc // 2009-12-29 // Die�� ���� �����尡 �������� ������ ��� RT_CARTOON_FADE_NONTEX�� ����
			}
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( fTimeCreateStepDust_ ) == true )
	{
		CreateStepDust();
	}

	//{{ kimhc // 2009-12-28 // ��� ��ų, ������ ���, ������ �ֺ��� ���Ḧ HP�� ������ ä����
    if ( m_pXSkinAnim->EventTimerOneshot( fTimeSacrificeOfHero_ ) == true )
	{
		ProcessSacrificeOfHero();
	}
	//}} kimhc // 2009-12-28 // ��� ��ų, ������ ���, ������ �ֺ��� ���Ḧ HP�� ������ ä����

	//{{ mauntain : ����ȯ [2012.06.14] ���� ī�� ��ȯ ��� - �ش� User ��Ż �� ���� ó��
	int SummonMonsterUID = GetSummonMonsterCardData().GetSummonMonsterUID();

	if( -1 != SummonMonsterUID )
	{
		CX2GUNPC* pNPCUnit = g_pX2Game->GetNPCUnitByUID( SummonMonsterUID );


		if( NULL != pNPCUnit )
		{
			pNPCUnit->SetNowHp( 0.f );


			if ( false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE0 ) &&
				 false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE1 ) &&
				 false == pNPCUnit->IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE2 ) )
				AccessSummonMonsterCardData().init();
		}
	}
	//}}

	if ( true == GetRidingOn() && NULL != CX2RidingPetManager::GetInstance() &&
		 NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() && true == m_ptrRidingPet->GetXSkinAnimPtr()->IsAnimationEnd() )
	{
		CX2RidingPetManager::GetInstance()->Handler_EGS_UNSUMMON_RIDING_PET_REQ();
	}


	CommonFrameMove();	// �� ĳ���Ͱ� override �� CommonFrameMove ����
}

/** @function : PlayAnimationWaitStart
	@brief : WaitStart ���¿��� ���ǿ� ���� �ִϸ��̼��� ���� ��Ŵ
*/
void CX2GUUser::PlayAnimationWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ )
{
	if( true == PlaySitReadyWaitStart(pXSkinAnim_) )
		return;

	CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType;
	LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", playType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );

	// FATAL_HP_PERCENT ���� HP�� ������ fatal �ִϸ��̼��� ����
	const float FATAL_HP_PERCENT = 0.25f;
	if( GetMaxHp() * FATAL_HP_PERCENT >= refFrameData_.syncData.fNowHP )
	{
		bool bTransition = true;
		LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );

		if( false == refFrameData_.unitCondition.bTransitionChange || false == bTransition )
			pXSkinAnim_->ChangeAnim( L"Fatal", false );
		else
			pXSkinAnim_->ChangeAnim( L"Fatal", true );
	}
	else
	{
		bool bTransition = true;
		LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
		if( false == refFrameData_.unitCondition.bTransitionChange || false == bTransition )
			pXSkinAnim_->ChangeAnim( L"Wait", false );
		else
			pXSkinAnim_->ChangeAnim( L"Wait", true );
	}

	pXSkinAnim_->Play( playType );
}
bool CX2GUUser::PlaySitReadyWaitStart( CKTDGXSkinAnim* pXSkinAnim_ )
{
	if( false == m_bReserveSitReadyEmotion )
		return false;

	wstring wstrName = L"Emotion_SitReady";

	if ( true == GetRidingOn() && NULL != GetRidingPet() && false == m_wstrRidingMotionName.empty() )
	{
		wstrName = m_wstrRidingMotionName;
		GetRidingPet()->StateChange( GetRidingPet()->m_SitReadyState );
	}

	pXSkinAnim_->ChangeAnim( wstrName.c_str(), true );
	pXSkinAnim_->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_ePlayedEmotion = CX2Unit::ET_SITREADY;

	m_bReserveSitReadyEmotion = false;
	return true;
}

/** @function : WalkFrameMoveFuture
	@brief : Walk ������ FrameMoveFuture �� Ű�Է¿� ���� �ȴ� �ӵ��� �̵�
*/
void CX2GUUser::WalkFrameMoveFuture()
{
	if( true == m_InputData.pureRight )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( true == m_InputData.pureLeft )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
	}

	CommonFrameMoveFuture();	// override �� �Լ� ����
}

/** @function : JumpFrameMoveFuture
	@brief : JumpUp, JumpDown ���¿��� �̵��� �����Ͽ� �������� ó���Ǵ� �κ�
*/
void CX2GUUser::JumpFrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( m_InputData.pureDown == true  && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}

	CommonFrameMoveFuture();
}

/** @function : JumpLandingStart
	@brief : ���� ������ Start �Լ� (���� ��ƼŬ ���)
*/
void CX2GUUser::JumpLandingStart()
{
	CommonStateStart();
	CreateStepDust();
}

void CX2GUUser::DashJumpLandingStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_JumpLandingState );

	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x *= 0.5f;
}

/** @function : DashStartFuture
	@brief : ��� ���� �ʿ��� �� �ʱ�ȭ
*/
void CX2GUUser::DashStartFuture()
{
	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_DashState );

	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = 0.0f;
}

/** @function : DashStart
	@brief : ���� ��ƼŬ ���
*/
void CX2GUUser::DashStart()
{
	CommonStateStart();
	CreateStepDust();
}

/** @function : DashFrameMoveFuture
	@brief : ��� ������ ���ǵ� �� ����
*/
void CX2GUUser::DashFrameMoveFuture()
{
	// ���� �Ǵ� ������ ����Ű�� �ι� ������ ���� ���� �󶧸�
	if( ( true == m_InputData.pureRight && true == m_BeforeInputData.pureRight ) ||
		( true == m_InputData.pureLeft  && true == m_BeforeInputData.pureLeft  ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

/** @function : DamageStandUpEventProcess
	@brief : �Ѿ��� �� �Ͼ� ���� ���� EventProcess
*/
void CX2GUUser::DamageStandUpEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		const float MAGIC_NUMBER_MULTIPLY_LINE_RADIUS = 1.5f;
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : DamageStandUpEventProcess
	@brief : �Ѿ��� �� �Ͼ� ���� ���� End
*/
void CX2GUUser::DamageStandUpEnd()
{
	CommonStateEnd();

	const float INVINCIBLE_TIME = 0.3f;

	SetForceInvincible( INVINCIBLE_TIME );
	SetShowInvincible( INVINCIBLE_TIME );
}

/** @function : DamageAirSmallStartFuture
	@brief : ���߿� �ٿ��� �ƴ� Ÿ���� �޾��� ���� StartFuture
*/
void CX2GUUser::DamageAirSmallStartFuture()
{
	CommonStateStartFuture();

	const float ADD_Y_SPEED = 700.f;
	m_PhysicParam.nowSpeed.y		+= ADD_Y_SPEED;

	const float DECREASE_WALK_SPEED_FACTOR = 0.33f;

	if( true == m_FrameDataNow.unitCondition.bAttackerFront )
		m_PhysicParam.nowSpeed.x	-= GetWalkSpeed() * DECREASE_WALK_SPEED_FACTOR;
	else
		m_PhysicParam.nowSpeed.x	+= GetWalkSpeed() * DECREASE_WALK_SPEED_FACTOR;
}

/** @function : DamageAirSmallEventProcess
	@brief : ���߿� �ٿ��� �ƴ� Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUUser::DamageAirSmallEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}

/** @function : DamageAirDownEventProcess
	@brief : ���߿� �ٿ� Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUUser::DamageAirDownEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_DOWN_LANDING );
	}

	CommonEventProcess();
}

/** @function : DamageAirDownInvincibleEventProcess
	@brief : ���߿� �ٿ� Ÿ���� �޾��� �� ���� ������ EventProcess
*/
void CX2GUUser::DamageAirDownInvincibleEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_DOWN_LANDING );
	}

	CommonEventProcess();
}

/** @function : DamageAirDownLandingStart
	@brief : ���߿��� Ÿ���� �޾� �ٿ� �� ���� Landing ������ Start
*/
void CX2GUUser::DamageAirDownLandingStart()
{
	if ( GetGrap() == true )
		SetGrap( false );


	CommonStateStart();

	UpDownCrashCameraNoReset( 20.0f, 0.3f );

	g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( this,  L"DropDown",
		m_FrameDataNow.syncData.position, m_FrameDataNow.unitCondition.dirDegree,
		m_FrameDataNow.unitCondition.dirDegree,	XL_EFFECT_0 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
	PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqAirDownTick = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_AIR_DOWN_TICK ) );

	if( NULL != pSeqAirDownTick )
	{
		PlaySequenceByTriggerCount( pSeqAirDownTick, m_FrameDataNow.syncData.position,  200.f, 200.f, 10 );
		pSeqAirDownTick->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
	}
}

/** @function : DamageAirDownLandingFrameMove
	@brief : ���߿��� Ÿ���� �޾� �ٿ� �� ���� Landing ������ FrameMove
*/
void CX2GUUser::DamageAirDownLandingFrameMove()
{
	if( true == m_pXSkinAnim->EventTimerOneshot( 0.029f ) || true == m_pXSkinAnim->EventTimerOneshot( 0.46f ) )
	{
        CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_DOWN_SMOKE ) );
		PlaySequenceByTriggerCount( pSeq, m_FrameDataNow.unitCondition.landPosition, 100, 100, 7 );
	}

	CommonFrameMove();
}

/** @function : DamageAirFallEventProcess
	@brief : UP Type�� ���ݿ� ���� �������� �ö󰬴ٰ� �������� ������Ʈ
*/
void CX2GUUser::DamageAirFallEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	m_bEnableUpsideWind = false;
//#endif LINEMAP_FAST_WIND_TEST

	if( 0.f < m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_DAMAGE_AIR_UP );
	}
	else if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_DOWN_LANDING );
	}

	CommonEventProcess();
}

/** @function : DamageAirUpEventProcess
	@brief : UP Type�� ������ ���� ������ EventProcess
*/
void CX2GUUser::DamageAirUpEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST

	if( 0.f > m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
	}
	else if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_DOWN_LANDING );
	}

	CommonEventProcess();
}

/** @function : DamageAirFlyEventProcess
	@brief : Fly Type�� ������ ���� ������ EventProcess
*/
void CX2GUUser::DamageAirFlyEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST

	if( true == IsOnSomethingFuture() )
	{
		//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
		if ( false == GetHold() )
		//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

		if ( false == GetGrap() )

		{
			/// Ż�Ϳ� ž�����̶��, ž�¹��� ������Ʈ�� �����Ѵ�.
			if ( true == GetRidingOn() )
				StateChange( USI_RIDING_DAMAGE_FRONT );
			else
				StateChange( USI_DAMAGE_DOWN_FRONT );
		}
	}

	CommonEventProcess();
}

/** @function : DamageRevengeStart
	@brief : ���ҵ��� �ݰ� ��ų�� ������ ���� ������Ʈ
*/
void CX2GUUser::DamageRevengeStart()
{
	CommonStateStart();

	CKTDGParticleSystem::CParticleEventSequence* pSeqExclamation = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hExclamationMark );
	if( NULL != pSeqExclamation )
	{
		pSeqExclamation->CreateNewParticle( pSeqExclamation->GetPosition() );
	}
}

/** @function : DamageRevengeCameraMove
	@brief : �ݰ� ��ų�� ������ ���� ī�޶� �̵�
*/
void CX2GUUser::DamageRevengeCameraMove()
{
	CommonCameraMove();

	if( false == m_FrameDataNow.stateParam.bEventFlagList[0] )
	{
		switch( m_FrameDataNow.stateParam.normalCamera )
		{
		case 0:
			break;
		case 1:
			{
				if( true == m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			break;

		case 2:
			{
				if( true == m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			break;

		default:
			ASSERT( !L"WorngPath" );
			break;
		}
	}

	m_FrameDataNow.stateParam.bEventFlagList[0] = true;
}

/** @function : DamageRevengeEventProcess
	@brief : �ݰ� ��ų�� ������ ���� EventProcess
*/
void CX2GUUser::DamageRevengeEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == IsOnSomethingFuture() )
			StateChange( USI_WAIT );
		else
			StateChange( USI_JUMP_DOWN );
	}

	CommonEventProcess();
}

/** @function : WallLandingEventProcess
	@brief : Wall �Ӽ��� ���� ���θʿ��� Jump Down ���� �� ���� ���� Ű���带 ������ ���� �Ǵ� WallLanding ������ EventProcess\n
			 ( ������ ���� �Ǵ����� �� �𸣰��� )
*/

void CX2GUUser::WallLandingEventProcess()
{
	if( true == m_FrameDataFuture.stateParam.bEventFlagList[0] &&
		0.15f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == m_InputData.pureUp )
		{
			m_bDisableGravity = false;
			m_FrameDataFuture.unitCondition.bFootOnWall = false;
			StateChange( USI_JUMP_UP );
			m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		}
		else
		{
			StateChange( USI_JUMP_DOWN );
		}
	}
	else if( 0.05f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonEventProcess();
}

/** @function : WalllandingEndFuture
	@brief : Wall �Ӽ��� ���� ���θʿ��� Jump Down ���� �� ���� ���� Ű���带 ������ ���� �Ǵ� WallLanding ������ EndProcess\n
			 ( ������ ���� �Ǵ����� �� �𸣰��� )
*/
void CX2GUUser::WallLandingEndFuture()
{
	m_bDisableGravity = false;
	m_FrameDataFuture.unitCondition.bFootOnWall = false;
	CommonStateEndFuture();
}


/** @function : HyperModeStart
	@brief : ������ ��带 ����Ѵٴ� �÷��� ����
*/
void CX2GUUser::HyperModeStart()
{
	CommonStateStart();
	m_bHyperModeUsed = true;
	m_iLastStateHyperModeCount = m_FrameDataNow.syncData.m_HyperModeCount;
	m_iHyperModeCountUsed = m_FrameDataNow.syncData.m_HyperModeCount;

	if ( GetRemainHyperModeTime() > 0.0f )
	{
		if( true == GetCanChangeHyperModeInHyperModeState() )/// ������ �簢�� ó��(����)
		{
			SetHyperModeCount( max( GetLastStateHyperModeCount() - 1, 0 ) );
		}
	}
	else
		SetHyperModeCount( 0 );

	m_FrameDataNow.syncData.m_HyperModeCount = 0;
	m_bReserveHyperModeZero = false;

}
void CX2GUUser::HyperModeStartFuture()
{
	CommonStateStartFuture();
	m_bReserveHyperModeZero = true;
}

void CX2GUUser::HyperModeEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
// #ifdef SUPERPOSITION_HYPER_MODE_BUG
// 	else if ( GetRemainHyperModeTime() > 0.0f &&
// 		false == GetCanChangeHyperModeInHyperModeState() )
// 	{
// 		// ���� �� ������ �Ұ��� �ѵ�, ���� ��ƾ ���� �ִٸ� ��� ���·� ���� ����
// 		if( false == IsOnSomethingFuture() )
// 			StateChange( USI_JUMP_DOWN );
// 		else
// 			StateChange( USI_WAIT );
// 	}
// #endif // SUPERPOSITION_HYPER_MODE_BUG

	CommonEventProcess();
}
void CX2GUUser::HyperModeEnd()
{
	CommonStateEnd();
	m_iLastStateHyperModeCount = 0;
}

/** @function : ShowMinorParticleHyperModeChange
	@brief : ���� ���·� �����ϴ� ��ƼŬ�� ������
*/
void CX2GUUser::ShowMinorParticleHyperModeChange( const wstring& wstrBoneName /*= L"Dummy1_Rhand"*/ )
{
	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	D3DXVECTOR3 pos = GetBonePos( wstrBoneName.c_str() );

	CKTDGParticleSystem::CParticleEventSequence* pSeqHyperModeChangeCenter = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_CENTER ) );
	PlaySequenceByTriggerCount( pSeqHyperModeChangeCenter, pos,  20, 40,  10 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqHyperModeChangeLine   = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_LINE ) );
	PlaySequenceByTriggerCount( pSeqHyperModeChangeLine, pos,  40, 80, 10 );

	if( NULL != pSeqHyperModeChangeLine )
	{
		pSeqHyperModeChangeLine->SetBlackHolePosition( pos );
		pSeqHyperModeChangeLine->UseLookPoint( true );
		pSeqHyperModeChangeLine->SetLookPoint( pos );
	}
}

/** @function : ShowMinorParticleHyperModeShock
	@brief : ���� ���·� �����ϴ� ��ƼŬ�� ������
*/
void CX2GUUser::ShowMinorParticleHyperModeShock( const wstring& wstrBoneName /*= L"Dummy1_Rhand"*/ )
{
	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	D3DXVECTOR3 pos = GetBonePos( wstrBoneName.c_str() );

	CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
	PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  3 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqWhiteShockWave = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_WHITE_SHOCK_WAVE ) );
	PlaySequenceByTriggerCount( pSeqWhiteShockWave, pos, 10, 10,  1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTick = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_IMPACT_TICK ) );
	PlaySequenceByTriggerCount( pSeqImpactTick, pos,  200, 200,  30 );

	if ( GetShowSmallGageAndName() )
	{
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( m_FrameDataNow.syncData.position );
		CKTDGParticleSystem::CParticleEventSequence* pSeqSTR_ToKang = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STR_TOKANG ) );
		PlaySequenceByTriggerCount( pSeqSTR_ToKang, projPos.x, projPos.y, 0.0f, 100, 100, 1 );
	}
	ShowSpecialEffectHyperMode();
}

/** @function : ShowMinorParticleHyperModeShock
	@brief : ���� ���¿��� ĳ������ ���� �پ� �ٴϴ� ��ƼŬ�� ������
*/
void CX2GUUser::ShowMinorParticleHyperModeTrace()
{
	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = pMinorParticleSystem->GetInstanceSequence( m_hHyperBoostRFoot );
	if( pSeq_RFoot != NULL )
	{
		pSeq_RFoot->SetPosition( GetBonePos( L"Bip01_R_Foot" ) );
		pSeq_RFoot->SetEmitRate( 30, 40 );
		pSeq_RFoot->SetTrace( true );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = pMinorParticleSystem->GetInstanceSequence( m_hHyperBoostLFoot );
	if( pSeq_LFoot != NULL )
	{
		pSeq_LFoot->SetPosition( GetBonePos( L"Bip01_L_Foot" ) );
		pSeq_LFoot->SetEmitRate( 30, 40 );
		pSeq_LFoot->SetTrace( true );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = pMinorParticleSystem->GetInstanceSequence( m_hHyperBoostRArm );
	if( pSeq_RArm != NULL )
	{
		pSeq_RArm->SetPosition( GetBonePos( L"Bip01_R_UpperArm" ) );
		pSeq_RArm->SetEmitRate( 30, 40 );
		pSeq_RArm->SetTrace( true );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = pMinorParticleSystem->GetInstanceSequence( m_hHyperBoostLArm );
	if( pSeq_LArm != NULL )
	{
		pSeq_LArm->SetPosition( GetBonePos( L"Bip01_L_UpperArm" ) );
		pSeq_LArm->SetEmitRate( 30, 40 );
		pSeq_LArm->SetTrace( true );
	}
}

// �޺� ������ Ÿ�ݿ� �� �� �ִ� �Ϲ����� eventprocess
void CX2GUUser::GenericGroundComboEndEventProcess()
{

	float fSkipTime = -1.f;
	LUA_GET_VALUE( m_LuaManager, "SKIP_TIME",	fSkipTime,		-1.f );


	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_CommonState.m_Wait );
	}
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( fSkipTime )
	{
		StateChange( m_CommonState.m_Wait );
	}

	CommonEventProcess();

}



void CX2GUUser::GenericSpecialActiveSkillEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( m_CommonState.m_Wait );
	}

	CommonEventProcess();
}



void CX2GUUser::GenericActiveSkillEventProcess()
{

	float fSkipTime = -1.f;
	LUA_GET_VALUE( m_LuaManager, "SKIP_TIME",	fSkipTime,		-1.f );

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( m_CommonState.m_Wait );
	}
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( fSkipTime )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( m_CommonState.m_Wait );
	}

	CommonEventProcess();

}






// ���θʿ��� ��Ż Ÿ�� �ٸ� �������� �̵��� �� ���̴� ����Ʈ �����ϴ� �Լ�
	void CX2GUUser::CreateTeleportEffect( const D3DXVECTOR3& vPos_, const D3DXVECTOR3& vRotDegree_ )
	{
	PlaySound( L"Nasod_Element_Teleport.ogg" );

	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P01", vPos_ );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P02", vPos_ );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P03", vPos_ );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", vPos_ + D3DXVECTOR3( 0,  50, 0 ) * 0.8f );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", vPos_ + D3DXVECTOR3( 0, 100, 0 ) * 0.8f );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", vPos_ + D3DXVECTOR3( 0, 200, 0 ) * 0.8f );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", vPos_ + D3DXVECTOR3( 0, 300, 0 ) * 0.8f );
	g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RespawnLanding_P04", vPos_ + D3DXVECTOR3( 0, 450, 0 ) * 0.8f );

	g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"RespawnLanding01", vPos_, vRotDegree_, vRotDegree_ );
	g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"RespawnLanding02", vPos_, vRotDegree_, vRotDegree_ );

}


// @bSpeedUpByItem: ������ ���� �ɼǿ� ���� �ִϸ��̼� �ӵ� ���� �������� ����
void CX2GUUser::ResetAnimSpeed( float fAnimSpeed, bool bSpeedUpByItem, bool bFuture, float fExtraSpeedRate /*= 1.f*/ )
{

	if( 1.f != fExtraSpeedRate )
	{
		fAnimSpeed *= fExtraSpeedRate;
	}

	if( true == bSpeedUpByItem )
	{
		float fSpeedUpRate = GetAnimSpeedValue();

		//{{ kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����
		if ( NULL != g_pData->GetWorldMissionManager() && CX2Game::GT_PVP != g_pX2Game->GetGameType() )
		{
			fSpeedUpRate += g_pData->GetWorldMissionManager()->GetDragonBreathAnimSpeedRateIncreased() * 0.01f;

			//{{ kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
			//}} kimhc // 2011-07-05 // �ɼǵ���Ÿ ��ġȭ �۾�
		}
		//}} kimhc // 2011.4.24 // ������� 2�ܰ�, �巡���� ����


		fAnimSpeed *= fSpeedUpRate;

		if( CX2Unit::UT_ELSWORD == GetUnit()->GetType() )
		{
			if( GetWayOfSwordState() == CX2GUElsword_SwordMan::WSS_VIGOR )
			{
				CX2GUElsword_SwordMan* pElsword = static_cast< CX2GUElsword_SwordMan* >( this );
				fAnimSpeed *= pElsword->GetAnimSpeedUpByVigor();
			}
		}
	}

	int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_INTENSE_SHOWTIME, true );

	if( iSkillLevel > 0 )
	{
		if ( true == IsFullHyperMode() )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CDC_INTENSE_SHOWTIME );
			if( NULL != pSkillTemplet )
			{
				const float fAniSpeedUp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevel );
				fAnimSpeed *= fAniSpeedUp;
			}
		}
	}

	//�Ĺ���� - ����� ���
	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LCR_AGILE_MOVEMENT, true );
	if( iSkillLevel > 0 && ( CX2Unit::UC_LIRE_COMBAT_RANGER == GetUnitClass() || CX2Unit::UC_LIRE_WIND_SNEAKER == GetUnitClass() ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LCR_AGILE_MOVEMENT );
		if( NULL != pSkillTemplet )
		{
			const float fAniSpeedUp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevel );
			fAnimSpeed *= fAniSpeedUp;
		}
	}

	// �Ӽ� ���ݵ ���� �ִϸ��̼� �ӵ��� ���ߴ���
	if( m_fAnimSpeedRate < 0.1f )
	{
		m_fAnimSpeedRate = 0.1f;
	}
	fAnimSpeed *= m_fAnimSpeedRate;



	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		1.f != g_pX2Game->GetWorld()->GetUnitAnimSpeedRate() )
	{
		fAnimSpeed *= g_pX2Game->GetWorld()->GetUnitAnimSpeedRate();
	}





	if( true == bFuture )
	{
		if( NULL != m_pXSkinAnimFuture )
		{
			m_pXSkinAnimFuture->SetPlaySpeed( fAnimSpeed );
		}
	}
	else
	{
		if( m_pXSkinAnim != NULL )
		{
			m_pXSkinAnim->SetPlaySpeed( fAnimSpeed );
		}
	}
}


float CX2GUUser::GetStopTime()
{
	return m_FrameDataFuture.unitCondition.fStopTime;

}

void CX2GUUser::SetStopTime( float fStopTime )
{
	m_FrameDataFuture.unitCondition.fStopTime = fStopTime;

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		g_pX2Game->GetWorld()->GetLineMap()->SetStopTime( max( g_pX2Game->GetWorld()->GetLineMap()->GetStopTime(), fStopTime ) );
	}



}

void CX2GUUser::SetStop2Time( float fStopTime )
{
	m_FrameDataFuture.unitCondition.fStop2Time = fStopTime;

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		g_pX2Game->GetWorld()->GetLineMap()->SetStopTime( max( g_pX2Game->GetWorld()->GetLineMap()->GetStopTime(), fStopTime ) );
	}



}

void CX2GUUser::EmotionFrameMove()
{
	if(m_ePlayedEmotion == CX2Unit::ET_NONE)
		return;

	if(m_pXSkinAnim != NULL && m_pXSkinAnim->IsAnimationEnd() == true)
	{
		switch(m_ePlayedEmotion)
		{
		case CX2Unit::ET_SITREADY:
			{
				wstring wstrEmotionName = L"Emotion_SitWait";

				if ( true == GetRidingOn() && NULL != GetRidingPet() && false == m_wstrRidingMotionName.empty() &&
					 NULL != GetRidingPet()->GetXSkinAnimPtr() && true == GetRidingPet()->GetXSkinAnimPtr()->IsAnimationEnd() )
				{
					wstrEmotionName = m_wstrRidingMotionName;
					GetRidingPet()->StateChange( GetRidingPet()->m_SitWaitState );
				}

				PlayEmotion(CX2Unit::ET_SITWAIT, wstrEmotionName);
				if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() &&
					CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )
				{//�ɱ� �̸�� ��� �� ü�� ȸ�� ����('������ �޽�' ����)
					SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_CALM_REST ) );
				}
			}
			break;
		case CX2Unit::ET_SITWAIT:
			break;
		default:
			{
				StateChange(GetWaitStateID(), true);
			}
			break;
		}
	}
}

bool CX2GUUser::PlayEmotion(CX2Unit::EMOTION_TYPE eEmotionId, wstring &wstrEmotionType)
{
	if( CX2Unit::IsAvatarEmotion( eEmotionId ) )
	{
		wstring wstrEmotionName = L"";
		CX2Unit::EMOTION_TYPE eEmotion = CX2Unit::ET_NONE;
		if ( true == IsPlayAvatarEmotionSoundWithouEmotion ( eEmotionId ) )
			return true;
		bool bMixEmotion = GetUnit()->GetAvatarEmotion(wstrEmotionName, eEmotion);
		if( bMixEmotion == false )
			return false;

		if( eEmotionId != eEmotion )
			return false;


		if( bMixEmotion == true && wstrEmotionName.empty() != true &&
			( (eEmotion == CX2Unit::ET_EMOTION_AVATAR1 && m_bMixedEmotion[0] == false) ||
			(eEmotion == CX2Unit::ET_EMOTION_AVATAR2 && m_bMixedEmotion[1] == false)  ||
			(eEmotion == CX2Unit::ET_EMOTION_AVATAR3 && m_bMixedEmotion[2] == false) ||
			(eEmotion == CX2Unit::ET_EMOTION_AVATAR4 && m_bMixedEmotion[3] == false) ||
			(eEmotion == CX2Unit::ET_EMOTION_AVATAR5 && m_bMixedEmotion[4] == false)
			|| (eEmotion == CX2Unit::ET_EMOTION_AVATAR6 && m_bMixedEmotion[5] == false)
			)

			)
		//if( bMixEmotion == true && m_bMixedEmotion == false && wstrEmotionName.empty() != true )
		{
			CKTDXDeviceXSkinMesh *pMixMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( wstrEmotionName );
			if( pMixMotion != NULL )
			{
				m_pXSkinAnim->MixAnim( pMixMotion );
				//m_bMixedEmotion = true;
				switch( eEmotion )
				{
				case CX2Unit::ET_EMOTION_AVATAR1:
					m_bMixedEmotion[0] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR2:
					m_bMixedEmotion[1] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR3:
					m_bMixedEmotion[2] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR4:
					m_bMixedEmotion[3] = true;
					break;

				case CX2Unit::ET_EMOTION_AVATAR5:
					m_bMixedEmotion[4] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR6:
					m_bMixedEmotion[5] = true;
					break;
				}
				SAFE_CLOSE( pMixMotion );
			}
		}
	}


	int iWaitStateId = GetNowStateID();

	if ( true == GetRidingOn() )
	{
		if ( iWaitStateId != GetRidingWaitStateID() )
			return false;
	}
	else
	{
		if( iWaitStateId != GetWaitStateID() )
			return false;
	}

	if( eEmotionId == CX2Unit::ET_SITREADY )
	{
		switch( GetUnit()->GetType() )
		{
		case CX2Unit::UT_LIRE:
			{
				if( CX2GULire_ElvenRanger::LESI_CHARGE_MP == GetNowStateID() )
				{
					StateChange( GetWaitStateID(), true );
					m_bReserveSitReadyEmotion = true;
				}
			}break;
		case CX2Unit::UT_ARME:
			{
				if( CX2GUArme_VioletMage::AVSI_CHARGE_MP == GetNowStateID() )
				{
					StateChange( GetWaitStateID(), true );
					m_bReserveSitReadyEmotion = true;
				}
			}break;
		case CX2Unit::UT_CHUNG:
			{
				if( CX2GUChung::CSI_CHARGE_CANNON_BALL == GetNowStateID() )
				{
					StateChange( GetWaitStateID(), true );
					m_bReserveSitReadyEmotion = true;
				}
			}break;
		}
	}
	if( true == GetRidingOn() )
	{
		if ( m_ePlayedEmotion == CX2Unit::ET_NONE && iWaitStateId != GetRidingWaitStateID() )
			return false;

		if(m_pXSkinAnim != NULL)
		{
			if(eEmotionId == CX2Unit::ET_SITWAIT)
				m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
			else
				m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		}

		if ( ( eEmotionId != CX2Unit::ET_SITREADY && eEmotionId != CX2Unit::ET_STANDUP ) && iWaitStateId == GetRidingWaitStateID() )
			return false;
	}
	else
	{
		if ( m_ePlayedEmotion == CX2Unit::ET_NONE && iWaitStateId != GetWaitStateID() )
		return false;
	}

	if(eEmotionId == CX2Unit::ET_NONE || wstrEmotionType.empty() == true)
		return false;

	if( eEmotionId == CX2Unit::ET_SITREADY &&
		(m_ePlayedEmotion == CX2Unit::ET_SITREADY || m_ePlayedEmotion == CX2Unit::ET_SITWAIT) )
		return false;

	if( eEmotionId == CX2Unit::ET_STANDUP && m_ePlayedEmotion != CX2Unit::ET_SITWAIT )
		return false;

	if(m_pXSkinAnim != NULL)
	{
		if( CX2Unit::IsAvatarEmotion( eEmotionId ) )
			m_pXSkinAnim->ChangeAnim( wstrEmotionType.c_str(), false );
		else
			m_pXSkinAnim->ChangeAnim( wstrEmotionType.c_str(), true );

		if(eEmotionId == CX2Unit::ET_SITWAIT)
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
		else
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );

		m_ePlayedEmotion = eEmotionId;

		if( eEmotionId == CX2Unit::ET_EMOTION_AVATAR1 || eEmotionId == CX2Unit::ET_EMOTION_AVATAR3 ||
			eEmotionId == CX2Unit::ET_EMOTION_AVATAR4 || eEmotionId == CX2Unit::ET_EMOTION_AVATAR5
			|| eEmotionId == CX2Unit::ET_EMOTION_AVATAR6

			|| eEmotionId == CX2Unit::ET_EMOTION_AVATAR7
			)
		{
			wstring wstrEmotionSoundName = L"";
			switch( eEmotionId )
			{
			case CX2Unit::ET_EMOTION_AVATAR1:
				wstrEmotionSoundName = L"Emotion_BigBangDance.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR3:
				wstrEmotionSoundName = L"Emotion_APinkDance.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR4:
				wstrEmotionSoundName = L"Emotion_APink_Love.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR5:
				wstrEmotionSoundName = L"DancingQueen_Music.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR6:
				wstrEmotionSoundName = L"Emotion_Bbabbabba.ogg";
				break;


			default:
				break;
			}

			if( m_pAvatarEmotionSound != NULL )
			{
				m_pAvatarEmotionSound->Stop();
				SAFE_CLOSE( m_pAvatarEmotionSound );
			}
			const float MAGIC_NUMBER = 500.f;
			const float fMaxDist = g_pKTDXApp->GetDGManager()->GetCamera().GetCameraDistance() + MAGIC_NUMBER;
			m_pAvatarEmotionSound = g_pKTDXApp->GetDeviceManager()->OpenSound( wstrEmotionSoundName, 10, true, fMaxDist );
			if( m_pAvatarEmotionSound != NULL )
			{
				if( CX2Main::XS_SQUARE_GAME != g_pMain->GetNowStateID() &&			// note!! �ŷ����忡���� �ϴ� 2D �����
					CX2Main::XS_PVP_ROOM	!= g_pMain->GetNowStateID() &&
					CX2Main::XS_WEDDING_GAME != g_pMain->GetNowStateID() &&
					true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
					true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() &&
					true == g_pMain->GetGameOption().GetOptionList().m_bEnable3DSound )
				{
					m_pAvatarEmotionSound->Set3DPosition( GetPos() );
					m_pAvatarEmotionSound->Play( false );
				}
				else
				{
					m_pAvatarEmotionSound->Play( false, false );
				}
			}
		}
		else
		{
			if( NULL != g_pChatBox )
			{
				g_pChatBox->PlayEmotionSound( GetUnit()->GetType(), eEmotionId, GetPos(), true );
			}
		}
		if ( eEmotionId == CX2Unit::ET_EMOTION_AVATAR4 )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Head" );
			g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL, L"Emotion_APINK_LOVE_P01", vBonePos );
		}

		return true;
	}

	return false;
}








void CX2GUUser::ResetMaxHP()
{
	SetMaxHp( GetStat().GetStat().m_fBaseHP );


//#ifdef PVP_BOSS_COMBAT_TEST
//
//	if( NULL != g_pX2Room )
//	{
//		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( GetUnitUID() );
//		if( NULL != pSlotData )
//		{
//			if( true == pSlotData->GetIsBoss() )
//			{
//				SetMaxHp( GetStat().GetStat().m_fBaseHP * 2.f );
//			}
//		}
//	}
//
//#endif PVP_BOSS_COMBAT_TEST
//
}


void CX2GUUser::ResetMaxMP()
{
	//SetMaxMP( ONE_CHARGE * 3.f );

	const float fMaxMp = ONE_CHARGE * 3.0f;

	m_AdditionalMaxMp.SetBasicStatValue( fMaxMp );

	float fResultMaxMp = GetUnit()->ResetMaxMp( fMaxMp );


	fResultMaxMp += ( fResultMaxMp * m_SocketData.m_Stat.m_ExtraStat.m_fIncreaseMPRate + m_SocketData.m_iAddMPValue );

//{{ ����� 2010.10.29
//}} ����� 2010.10.29

	//SetMaxMp( fResultMaxMp );

	m_AdditionalMaxMp.SetOptionValue( fResultMaxMp - fMaxMp );

	m_AdditionalMaxMp.CheckAndUpdateResultValue();

	SetMaxMp( m_AdditionalMaxMp.GetResultValue() );
}



void CX2GUUser::ResetMPChangeRate( float fMPChangeRate )
{
	SetChangeRateMp( fMPChangeRate );

	//{{ dmlee 2008.7.13 play channel
	if( KPVPChannelInfo::PCC_PLAY == g_pMain->GetPVPChannelClass( g_pMain->GetConnectedChannelID() ) &&
		CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID() )
	{
		const float MAGIC_PLAY_CHANNEL_MP_CHANGE_RATE_SCALE = 7.f;
		SetChangeRateMp( GetChangeRateMp() * MAGIC_PLAY_CHANNEL_MP_CHANGE_RATE_SCALE );
	}
	//}} dmlee 2008.7.13 play channel
}



//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� Īȣ ȹ�� �� ��ü ���� �κ� �߰�
void CX2GUUser::UpdateSocketDataAndEnchantData()
{
	m_SocketData.ReInit();
	m_vecTitleOptions.resize( 0 );
	m_mapDefenceEnchantData.clear();
	m_mapNpcIdNDamageUpPercent.clear();
	m_mapNpcIDNDEvadeUpPercent.clear();
	m_mapExtraDamageNImmunePer.clear();

//{{ kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)
	m_mapSocketItemEffectEx.clear();
//}} kimhc // 2010.4.2 // ��д��� �۾�(��Ʈ������ȿ��)

	m_mapAttackEnchantData.clear();

	m_vecSummonNpcSocket.clear();

	m_vecBuffFactorPtr.resize( 0 );

	ClearAllDelegateProcess();

	InitSocketData();
	InitEnchantData();
	ResetPhysicParam();
}

/** @function : UpdatePassiveAndActiveSkillState
	@brief : �нú�, ��Ƽ�� ��ų ������Ʈ
*/
void CX2GUUser::UpdatePassiveAndActiveSkillState()
{
	InitAdditionalPassive();
	InitPassiveSkillState();
	InitEquippedActiveSkillState( true );
	ResetMaxMP();

	if ( IsMyUnit() )
	{
		g_pX2Game->UpdateSkillSlotUI(true);
	}
}

void CX2GUUser::UpdateEquippedEmblem()
{
	if( NULL != GetUnit() )
	{
		int iTitleId			= GetUnit()->GetTitleId();
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqEmblem );
		m_hSeqEmblem			= INVALID_PARTICLE_SEQUENCE_HANDLE;
		// m_pPart_Emblem_200�� m_hSeqEmblem�� Particle �����Ϳ� ���� ������ NULL ����� ��
        m_hPart_Emblem_200		= INVALID_PARTICLE_HANDLE;

		if ( iTitleId > 0)
		{
			wstring titleName = g_pData->GetTitleManager()->GetTitleModel(iTitleId);
			m_hSeqEmblem = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  titleName.c_str(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

			CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );

			if( pSeqEmblem != NULL )
			{
                m_hPart_Emblem_200 = pSeqEmblem->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
				pSeqEmblem->SetShowObject( false );
			}
		}
	}
}

//{{ kimhc // 2010.12.10 // 2010-12-23 New Character CHUNG
CX2EqipPtr	CX2GUUser::GetEquipPtrFromEquipViewListByEquipPosition( const CX2Unit::EQIP_POSITION eEquipPosition )
{
	BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquipPtr, m_ViewEqipList )
	{
		if ( eEquipPosition == pViewEquipPtr->GetEqipPos() )
		{
			return pViewEquipPtr;
		}
	}

	return CX2EqipPtr();
}

/** @function : UpdateEquipViewListInfo
	@brief : ����� ����, Ż���� ���� ViewList�� ������
	@param :  CX2Item* pItemToAddInViewList(�߰��� ���), CX2Item* const pItemToRemoveInViewList(Ż���� ���)
	@return : �Լ��� ���� ����
*/
bool CX2GUUser::UpdateEquipViewListInfo( CX2Item* pItemToAddInViewList, CX2Item* const pItemToRemoveInViewList )
{
	{	// kimhc // �� �߰�ȣ�� �� �־�� �մϴ�.
		CX2EqipPtr pEquipPtrToRemove;
		int iIndexInViewListForRemove_ = 0;
		if ( false == GetAddItemAndRemoveItemToViewList( pEquipPtrToRemove, &pItemToAddInViewList, pItemToRemoveInViewList, iIndexInViewListForRemove_ ) )
			return false;

		if ( pEquipPtrToRemove.get() != NULL )
		{
			RemoveEquipFromViewList( pEquipPtrToRemove, iIndexInViewListForRemove_ );
		}
	}

	if ( pItemToAddInViewList != NULL )
	{
		AddEquipToViewList( pItemToAddInViewList );
	}
	return true;
}

/** @function : SetShowViewList
	@brief : ViewList���� �ִ� �͵��� ������� ������ ����
	@param : bool bShow_ = true
*/
/*virtual*/ void CX2GUUser::SetShowViewList( bool bShow_ )
{
	BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquipPtr, m_ViewEqipList )
	{
		if ( pViewEquipPtr != NULL )
		{
			pViewEquipPtr->SetShowObject( bShow_ );
		}
	}
}

//}} kimhc // 2010.12.10 //  2010-12-23 New Character CHUNG

/** @function : UpdateWeaponEnchantEffectAndExtraDamageType
	@brief : ���⿡ �ٴ� ����Ʈ, Ʈ���̽�, ExtraDamageType�� ����
*/
/*virtual*/ void CX2GUUser::UpdateWeaponEnchantEffectAndExtraDamageType()
{
	UpdateSlashTraceTextureName();
	DeleteEnchantedWeaponEffectAtHand();
	DeleteEnchantWeaponEffectUnitSpecific( 0 );
	CalcWeaponEnchantExtraDamageType();
	InitEnchantedWeaponEffectAtHand();
	InitEnchantWeaponEffectUnitSpecific();
	UpdateSlashTrace();

}

void CX2GUUser::AddEquipToViewList( CX2Item* const pItem )
{
	if ( pItem == NULL || pItem->GetItemTemplet() == NULL )
		return;

	CX2EqipPtr	pCX2EquipPtr;
	int			enchantLevel = 0;

	enchantLevel = pItem->GetItemData().m_EnchantLevel;

	bool bBasicEquip = GetUnit()->IsBasicEquip(
        pItem->GetItemTemplet()->GetItemID()
        );

	if( pItem->GetItemTemplet()->GetUseCondition() == CX2Item::UC_ANYONE )
	{
		pCX2EquipPtr = CX2Eqip::CreateEquip( pItem, m_pXSkinAnim, enchantLevel, bBasicEquip, true, GetUnit()->GetType() );
	}
	else
	{
		pCX2EquipPtr = CX2Eqip::CreateEquip( pItem, m_pXSkinAnim, enchantLevel, bBasicEquip, true );
	}

	pCX2EquipPtr->SetManagerObject( this );
	// ���ο� ��� Ư���ɷ��� �پ� �־��ٸ� Ư���ɷ� �ο��� �ֱ�

	if ( IsRecursivelyManagedByDGManager() )
		g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2EquipPtr.get() );

	m_ViewEqipList.push_back( pCX2EquipPtr );

    const CX2Item::ItemTemplet*	pItemTemplet	= pItem->GetItemTemplet();
	int						weaponIndex		= 0;

	switch ( pItemTemplet->GetEqipPosition() )
	{
	case CX2Unit::EP_WEAPON_HAND:
		weaponIndex = 0;				// InitEqip �κп��� 0�� 1�� ���еǾ� ����
		break;

	case CX2Unit::EP_WEAPON_SECOND:
		weaponIndex = 1;
		break;
	}

	// ���ο� ��� Weapon �� ���
	switch ( pItemTemplet->GetEqipPosition() )
	{
	case CX2Unit::EP_WEAPON_HAND:
	case CX2Unit::EP_WEAPON_SECOND:
		{
			Weapon* pWeapon = Weapon::CreateWeapon( *this );
			ASSERT( pWeapon != NULL );

			pWeapon->m_iWeaponIndex			= weaponIndex;
			pWeapon->m_pUserWeaponEquip		= pCX2EquipPtr;
			pWeapon->m_bUseSlashTrace		= true;


			if( NULL != GetUnit() )
			{
				pItemTemplet->GetSlashTraceColors( pWeapon->m_coSlashTrace, pWeapon->m_coSlashTraceTip
					, pWeapon->m_coSlashTraceHyper, pWeapon->m_coSlashTraceTipHyper, GetUnit()->GetType() );
			}


			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorParticle() )
			{
                unsigned uNumParticle = pItemTemplet->GetNumAttachedParticleData();
				for( size_t numParticle = 0; numParticle < uNumParticle; numParticle++ )
				{

                    const wchar_t*  pwszParticleName = L"";
                    const wchar_t*  pwszBoneName = L"";
                    pItemTemplet->GetAttachedParticleData( numParticle, &pwszParticleName, &pwszBoneName );
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, pwszParticleName, D3DXVECTOR3( 0, 0, 0 ) );
					if( NULL != pSeq )
					{
						AttachWeaponParticle( pWeapon->m_iWeaponIndex, true, pwszBoneName, pSeq->GetHandle() );
					}
				}
			}


			m_vecpWeapon.push_back( pWeapon );
		}
		break;	// switch ( pItemTemplet->GetEqipPosition() )
	}
}
void CX2GUUser::RemoveEquipFromViewList( CX2EqipPtr pEquipPtrToRemove, int index )
{
	CX2Unit::EQIP_POSITION	equipPosition	= pEquipPtrToRemove->GetEqipPos();
	int						weaponIndex		= 0;


// 	pEquipPtrToRemove->SetManagerObject( NULL );
// 	pEquipPtrToRemove.reset();
	m_ViewEqipList.erase( m_ViewEqipList.begin() + index );

	switch ( equipPosition )
	{
	case CX2Unit::EP_WEAPON_HAND:
		weaponIndex = 0;				// InitEqip �κп��� 0�� 1�� ���еǾ� ����
		break;

	case CX2Unit::EP_WEAPON_SECOND:
		weaponIndex = 1;
		break;
	}

	switch ( equipPosition )
	{
	case CX2Unit::EP_WEAPON_HAND:		// pWeapon->m_iWeaponIndex�� 0�ΰ�
	case CX2Unit::EP_WEAPON_SECOND:

		for ( size_t numWeapon = 0; numWeapon < m_vecpWeapon.size(); numWeapon++ )
		{
			Weapon* pWeapon	= m_vecpWeapon[numWeapon];

			if ( pWeapon->m_iWeaponIndex == weaponIndex )
			{
				m_vecpWeapon.erase( m_vecpWeapon.begin() + numWeapon );
				Weapon::DeleteKTDGObject( pWeapon );

				--numWeapon;	// added by wonpok, 20100318
			}
		}

		break;	// switch
	}
}

void CX2GUUser::DeleteEnchantedWeaponEffectAtHand()
{
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWeaponCommon1 );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWeaponCommon2 );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqWeaponCommon3 );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqEnchantedWeaponCommon4 );
}





// note!! ��ȯ �ʻ�� ������ ������ ���� �������� ���� ������ �ȵǹǷ� �ʻ�� ����� �ȵǵ��� ���� ���ؼ� �ϵ��ڵ�,
// ��ȯ �ʻ�� �߰��Ǹ� �߰������ ��
bool CX2GUUser::IsSummonSpecialAttack( CX2SkillTree::SKILL_ID iSkillID )
{
	switch( iSkillID )
	{
		case CX2SkillTree::SI_A_EN_DIMENSION_LINK:
	case CX2SkillTree::SI_SA_EN_DIMENSION_LINK_BLADER:
	case CX2SkillTree::SI_SA_EN_DIMENSION_LINK_GUARDIAN:
		case CX2SkillTree::SI_A_EAT_SURFACE_CUTING:
		case CX2SkillTree::SI_A_RNW_ANGER_OF_ELF:	//��â�� ��������� �г� �߰�
	case CX2SkillTree::SI_SA_EAT_SONIC_WAVE:
	case CX2SkillTree::SI_A_LSR_ENTANGLE:

	case CX2SkillTree::SI_A_ADM_HELL_DROP:
		case CX2SkillTree::SI_A_AEM_STONE_WALL:

		//{{ JHKang / ������ / 2010/12/06 / ����� ����
	case CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD:
		//}} JHKang / ������ / 2010/12/06 / ����� ����

		case CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE:

		case CX2SkillTree::SI_A_LTR_EVOKE:

	case CX2SkillTree::SI_A_CTT_AUTOMATIC_MORTAR: // �ڰ���
	case CX2SkillTree::SI_A_ADW_DISTORTION:// ���� �ְ�
	case CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA:
	case CX2SkillTree::SI_SA_AN_PARTICLE_PRISM:
		{
			return true;
		} break;
	}

	return false;
}

/** @function : CheckSummonSpecialAttack
	@brief : ��ȯ �ʻ�� ������ ��� �� �� �ִ� �������� üũ
	@param : iSkillID_ : üũ�� ��ų ID
	@return : ��� ���� true, ��� �Ұ��� false
*/
bool CX2GUUser::CheckSummonSpecialAttack( CX2SkillTree::SKILL_ID iSkillID_ )
{
	//��ȯ�� ��ų�� �ƴ϶�� ��� ����
	if( false == IsSummonSpecialAttack( iSkillID_ ) )
		return true;

	bool bCheck = true;

	if( NULL == g_pX2Game )
		return false;

	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_BATTLE_FIELD:
		{
			//�ʵ峻 ���Ͱ� �������� �ʾ��� ���� ��ų ��� �� �� ������.
			if( false == static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetIsNpcLoadComplete() )
				bCheck = false;

			//��Ż �������� ��ų ��� �� �� ������.
			if( true == static_cast<CX2BattleFieldGame*>(g_pX2Game)->IsNearPortalLineMap()	)
				bCheck = false;
		} break;
	default:
		{
			if( true == g_pX2Game->GetGameEnd() )
				bCheck = false;

			if( true == g_pX2Game->GetLastKillCheck() )
				bCheck = false;

			if( true == true == g_pX2Game->GetLastKillCheck() )
				bCheck = false;
		} break;
	}

	if( false == bCheck )
	{
		g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD,
			GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
	}

	return bCheck;
}




//#ifdef PVP_BOSS_COMBAT_TEST
//	void CX2GUUser::Frozen_StateStart()
//	{
//		CommonStateStart();
//		m_TeamNum = 9;
//		m_iFriendlyHittedCountAtFrozenState = 0;
//	}
//
//	void CX2GUUser::Frozen_StateEnd()
//	{
//		m_TeamNum = m_TeamNumOriginal;
//		m_iFriendlyHittedCountAtFrozenState = 0;
//		CommonStateEnd();
//	}
//
//	void CX2GUUser::Frozen_EventProcess()
//	{
//		const float MAGIC_FROZEN_TIME = 10.f;
//		if( m_FrameDataFuture.unitCondition.fStateTime > MAGIC_FROZEN_TIME )
//		{
//			SetNowHp( GetMaxHp() * 0.5f );
//			StateChange( USI_DAMAGE_AIR_FALL );
//		}
//		else if( true == m_FrameDataFuture.syncData.bIsRight && true == m_InputData.pureLeft )
//		{
//			m_FrameDataFuture.unitCondition.fStateTime += g_pKTDXApp->GetElapsedTime();
//			//m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
//		}
//		else if( false == m_FrameDataFuture.syncData.bIsRight && true == m_InputData.pureRight )
//		{
//			m_FrameDataFuture.unitCondition.fStateTime += g_pKTDXApp->GetElapsedTime();
//			//m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
//		}
//	}
//#endif PVP_BOSS_COMBAT_TEST



//}} kimhc // �ǽð� ���ҵ� �� �ǽð� Īȣ ȹ�� �� ��ü ���� �κ� �߰�

//#ifndef NOT_USE_DICE_ROLL
////////////////////////////////////////////////////////////////////////////
//CX2GUUser::DiceRoll::DiceRoll( CX2GUUser* pGUUser )
//{
//	m_pGUUser = pGUUser;
//
//	m_iNowRollingDiceCount = -1;
//	m_iNowDiceSeqIndex = -1;
//	m_pParticle = NULL;
//	m_hSeqDiceBalloon = INVALID_PARTICLE_SEQUENCE_HANDLE;
//
//
//	//CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
//	WCHAR wszText[64] = L"";
//	for( int i=0; i<6; i++ )
//	{
//		m_hSeqRollingDice[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
//		m_hSeqDice[i]			= INVALID_PARTICLE_SEQUENCE_HANDLE;
//
//		swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L"Rolling_Dice_%d", i+1 );
//#ifdef FIX_DICEROLL_CRASH
//		m_hSeqRollingDice[i] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  wszText, D3DXVECTOR3(0, 0, 0), 0.f, 0.f );
//#else
//		m_hSeqRollingDice[i] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wszText, D3DXVECTOR3(0, 0, 0), 0.f, 0.f );
//#endif //FIX_DICEROLL_CRASH
//		//pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqRollingDice[i] );
//		//if( NULL != pSeq )
//		//{
//		//	pSeq->SetTriggerWait( true );
//		//}
//
//		swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L"Dice_%d", i+1 );
//#ifdef FIX_DICEROLL_CRASH
//		m_hSeqDice[i] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  wszText, D3DXVECTOR3(0, 0, 0), 0.f, 0.f );
//#else
//		m_hSeqDice[i] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wszText, D3DXVECTOR3(0, 0, 0), 0.f, 0.f );
//#endif //FIX_DICEROLL_CRASH
//		//pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDice[i] );
//		//if( NULL != pSeq )
//		//{
//		//	pSeq->SetTriggerWait( true );
//		//}
//	}
//}
//
//
//CX2GUUser::DiceRoll::~DiceRoll()
//{
//	for( int i=0; i<6; i++ )
//	{
//		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqRollingDice[i] );
//		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqDice[i] );
//
//		m_hSeqRollingDice[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
//		m_hSeqDice[i]			= INVALID_PARTICLE_SEQUENCE_HANDLE;
//	}
//}
//
//
//
//
//void CX2GUUser::DiceRoll::OnFrameMove( double fTime, float fElapsedTime )
//{
//	const int MAGIC_ROLLING_COUNT = 8;
//
//
//	if( -1 == m_iNowRollingDiceCount )
//	{
//		if( m_vecDiceRoll.size() > 0 )
//		{
//			m_iNowRollingDiceCount = 0;
//
//			if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqRollingDice[ m_iNowRollingDiceCount%6 ] )
//			{
//#ifdef FIX_DICEROLL_CRASH
//				m_hSeqDiceBalloon = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,  L"Dice_Balloon", GetParticlePos() );
//#else
//				m_hSeqDiceBalloon = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Dice_Balloon", GetParticlePos() );
//#endif //FIX_DICEROLL_CRASH
//			}
//		}
//		return;
//	}
//
//	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqDiceBalloon )
//	{
//		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDiceBalloon );
//		if( NULL != pSeq )
//		{
//			pSeq->SetPosition( GetParticlePos() );
//		}
//		else
//		{
//			m_hSeqDiceBalloon = INVALID_PARTICLE_SEQUENCE_HANDLE;
//		}
//	}
//
//
//	if( m_iNowRollingDiceCount > MAGIC_ROLLING_COUNT )
//	{
//		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqDice[ m_iNowDiceSeqIndex ] )
//		{
//			CKTDGParticleSystem::CParticleEventSequence* pSeq =
//				g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDice[ m_iNowDiceSeqIndex ] );
//			if( NULL != pSeq )
//			{
//				if( 0 == pSeq->GetLiveParticleNum() )
//				{
//					m_pParticle = NULL;
//					m_iNowRollingDiceCount = -1;
//
//					if( m_vecDiceRoll.size() > 0 )
//					{
//						m_vecDiceRoll.erase( m_vecDiceRoll.begin() );
//					}
//				}
//				else
//				{
//					m_pParticle->SetPos( GetParticlePos() );
//				}
//			}
//		}
//	}
//	else
//	{
//		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqRollingDice[ m_iNowRollingDiceCount%6 ] )
//		{
//			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqRollingDice[ m_iNowRollingDiceCount%6 ] );
//			if( NULL != pSeq )
//			{
//				if( 0 == pSeq->GetLiveParticleNum() )
//				{
//					m_pParticle = NULL;
//					m_iNowRollingDiceCount += 1;
//
//					if( m_iNowRollingDiceCount > MAGIC_ROLLING_COUNT )
//					{
//						if( m_vecDiceRoll.size() > 0 )
//						{
//							// 6
//							if( true == m_vecDiceRoll[0] )
//							{
//								CKTDGParticleSystem::CParticleEventSequence* pSeqDice = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDice[ 5 ] );
//								if( NULL != pSeqDice )
//								{
//									m_pParticle = pSeqDice->CreateNewParticle( GetParticlePos() );
//									m_iNowDiceSeqIndex = 5;
//								}
//							}
//							else // 6�� ������ ������ ����
//							{
//								int iRandomDice = rand()%5;
//								CKTDGParticleSystem::CParticleEventSequence* pSeqDice = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDice[ iRandomDice ] );
//								if( NULL != pSeqDice )
//								{
//									m_pParticle = pSeqDice->CreateNewParticle( GetParticlePos() );
//									m_iNowDiceSeqIndex = iRandomDice;
//								}
//							}
//						}
//					}
//					else
//					{
//						CKTDGParticleSystem::CParticleEventSequence* pSeqDice = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqRollingDice[ m_iNowRollingDiceCount%6 ] );
//						if( NULL != pSeqDice )
//						{
//							m_pParticle = pSeqDice->CreateNewParticle( GetParticlePos() );
//						}
//					}
//				}
//				else
//				{
//					m_pParticle->SetPos( GetParticlePos() );
//				}
//			}
//		}
//	}
//}
//
//D3DXVECTOR3 CX2GUUser::DiceRoll::GetParticlePos()
//{
//	if( NULL == m_pGUUser )
//		return D3DXVECTOR3(0, 0, 0);
//
//	D3DXVECTOR3 vPos = m_pGUUser->GetBonePos( L"Bip01_Head" );
//	vPos.y += 100.f;
//
//	D3DXVECTOR3 vDirVector = m_pGUUser->GetDirVector();
//	D3DXVECTOR3 vZVector = GetZVector( vDirVector );
//	vPos += vZVector * (-100.f);
//
//	return vPos;
//}
//
//D3DXVECTOR3 CX2GUUser::DiceRoll::GetZVector( D3DXVECTOR3& vDirVector )
//{
//	D3DXVECTOR3 vZVector;
//
//	vZVector.y = 0.f;
//	vZVector.z = vDirVector.x;
//	vZVector.x = -vDirVector.z;
//
//	D3DXVec3Normalize( &vZVector, &vZVector );
//
//	return vZVector;
//}
//#endif //NOT_USE_DICE_ROLL

//{{ lucidash // 2010.12.14 // Ÿ����� ���.
void CX2GUUser::SetGrap(bool val)
{
	if ( m_iGrapShakeCount <= 0 ||
		false == val )
	{
		m_bDisableGravity = false;
		m_bGrap = false;
		return;
	}

	if( GetNowHp() > 0.f )
	{
		StateChange( GetGrapStateID(), true );
		m_PhysicParam.nowSpeed = D3DXVECTOR2( 0, 0 );
		m_bGrap = val;
	}

}

void CX2GUUser::SetGrappedPosition( D3DXVECTOR3 vPos )
{
	m_vGrap = vPos;
	return;
}
void CX2GUUser::SetBeforeGrapPosition( D3DXVECTOR3 vPos )
{
	m_vBeforeGrapPos = vPos;
	return;
}
void CX2GUUser::ResetToBeforeGrapPos()
{
	m_FrameDataFuture.syncData.position				= m_vBeforeGrapPos;
	m_FrameDataNow.syncData.position				= m_vBeforeGrapPos;
	return;
}

//}} lucidash // 2010.12.14 // Ÿ����� ���.



void CX2GUUser::SetHold(bool val)
{
	if( GetUnit() != NULL && GetUnit()->GetType() == CX2Unit::UT_RAVEN )
	{
		CX2GURaven *pRaven = (CX2GURaven*)this;
		if( pRaven != NULL &&
			GetRemainHyperModeTime() > 0.f &&
			( pRaven->CanParryingState() == true || pRaven->GetParrying() == true ) &&
			( pRaven->GetNasodBall() > 0 || pRaven->CanParrying() == true ) &&
			IsSuperArmor() != true &&
			m_FrameDataFuture.unitCondition.bAttackerFront == true )
		{
				return;
		}
	}

	if( GetHold() == val )
		return;
	m_bHold = val;

//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
	if(GetHold() == true)
	{
		/// Ż�� ž�½� Ż�� ������Ʈ�� ����
		if ( true == GetRidingOn() )
			StateChange( USI_RIDING_DAMAGE_FRONT, true );
		else
			StateChange( GetHoldStateID(), true );
	}
//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

}

void CX2GUUser::SetPos( D3DXVECTOR3 vPos )
{
	m_vHold = vPos;

	m_vGrap = vPos;

	if( NULL == g_pX2Game ||
		NULL == g_pX2Game->GetWorld() ||
		NULL == g_pX2Game->GetWorld()->GetLineMap() )
	{
		ASSERT( !"CX2GUUser::SetPosition, null world" );
		return;
	}

	CKTDGLineMap*	pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	D3DXVECTOR3		startPos;

	startPos = vPos;
	int lineIndex = -1;
	D3DXVECTOR3	landPos = vPos;
	landPos = pLineMap->GetLandPosition( startPos, LINE_RADIUS, &lineIndex );
	//pLineMap->IsOnLineConnect( startPos, &lineIndex, LINE_RADIUS, &landPos );
	pLineMap->IsOnLine( startPos, LINE_RADIUS, &startPos, &lineIndex, false );

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( lineIndex );
	if( NULL == pLineData )
	{
		ASSERT( !"CX2GUUser::SetPosition, null linedata" );
		return;
	}
	//startPos = landPos;

	//���� ������ ����Ÿ�� ������Ʈ �Ѵ�.

//	THEMIDA_VM_START
	//m_FrameDataFuture.syncData.position				= startPos;
	m_FrameDataFuture.unitCondition.dirDegree			= pLineData->dirDegree;
	m_FrameDataFuture.unitCondition.landPosition		= landPos;
	m_FrameDataFuture.syncData.lastTouchLineIndex		= lineIndex;
	//if( m_FrameDataFuture.syncData.bIsRight == false )
	//	m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;

	//m_FrameDataNow.syncData.position = vPos;
	m_FrameDataNow.syncData.lastTouchLineIndex = lineIndex;

//	THEMIDA_VM_END

	//m_PhysicParam.nowSpeed.x = 0.0f;
	//m_PhysicParam.nowSpeed.y = 0.0f;
}









void CX2GUUser::SetPositionOnLine( const D3DXVECTOR3& vPosition, const int iLineIndex )
{
#if X2_DEV_TOOLS
	ASSERT( NULL != g_pX2Game );
	ASSERT( NULL != g_pX2Game->GetWorld() );
	ASSERT( NULL != g_pX2Game->GetWorld()->GetLineMap() );

	if( NULL == g_pX2Game )
		return;

	if( NULL == g_pX2Game->GetWorld() )
		return;

	if( NULL == g_pX2Game->GetWorld()->GetLineMap() )
		return;



	const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( iLineIndex );
	ASSERT( NULL != pLineData );
	if( NULL == pLineData )
		return;


	// �������� ĳ������ ��ġ�� ���θ� ���� �Ű��ش�
	D3DXVECTOR3 vPlaneNormal;
	D3DXVec3Cross( &vPlaneNormal, &pLineData->dirVector, &Y_AXIS );
	//D3DXVec3Normalize( &vPlaneNormal, &vPlaneNormal );
	D3DXVECTOR3 vPlaneVertex = pLineData->startPos;

	D3DXVECTOR3 vRayStart = vPosition - vPlaneNormal * 100000.f;

	float fDistance = 0.f;
	D3DXVECTOR3 vIntersection( 0, 0, 0 );
	bool bIntesected = g_pKTDXApp->GetCollision()->RayToPlane( fDistance, vIntersection, vRayStart, vPlaneNormal, vPlaneVertex, vPlaneNormal );
	ASSERT( true == bIntesected );
	if( false == bIntesected )
		return;

	m_FrameDataFuture.syncData.position = vIntersection;
	m_FrameDataFuture.syncData.lastTouchLineIndex = iLineIndex;
	m_FrameDataFuture.unitCondition.dirDegree = pLineData->dirDegree;
	m_FrameDataFuture.unitCondition.dirVector = pLineData->dirVector;
	m_FrameDataFuture.unitCondition.landPosition = vIntersection;


	m_FrameDataNow.syncData.position = vIntersection;
	m_FrameDataNow.syncData.lastTouchLineIndex = iLineIndex;
	m_FrameDataNow.unitCondition.dirDegree = pLineData->dirDegree;
	m_FrameDataNow.unitCondition.dirVector = pLineData->dirVector;
	m_FrameDataNow.unitCondition.landPosition = vIntersection;


	BOOST_TEST_FOREACH( SyncData&, syncData, m_ReceiveSyncDataList )
	{
		syncData.position = vIntersection;
		syncData.lastTouchLineIndex = iLineIndex;
	}

	BOOST_TEST_FOREACH( SyncData&, syncData, m_SendSyncDataList )
	{
		syncData.position = vIntersection;
		syncData.lastTouchLineIndex = iLineIndex;
	}
#endif // ENABLE_SET_UNIT_POSITION_TEST
}



void CX2GUUser::SetEnableDash( bool bEnableDash,  CX2GameUnit::ENABLE_DASH_STATE eState /*= EDS_END*/ )
{
	CX2GameUnit::SetEnableDash( bEnableDash, eState );

	if( false == bEnableDash )
	{
		if( USI_DASH == GetGameUnitState()
			|| USI_DASH == GetGameUnitStateFuture()
			)
		{
			StateChange( USI_WALK, true );
		}
	}
}

void CX2GUUser::StateChangeDashIfPossible()
{
	if( true == GetEnableDash() )
	{
		if ( true == GetRidingOn() )
			StateChange( USI_RIDING_DASH );
		else
			StateChange( USI_DASH );
		++m_iDashCount;
	}
	else
	{
		StateChange( USI_WALK );
	}
}

void CX2GUUser::SetInputData( InputData* pInputData )
{
	m_OrgInputData		= *pInputData;

	if( m_fCanNotAttackTime > 0.0f )
	{
		pInputData->oneX			= false;
		pInputData->pureX			= false;
		pInputData->oneDoubleX		= false;
		pInputData->pureDoubleX		= false;

		pInputData->oneZ			= false;
		pInputData->pureZ			= false;
		pInputData->oneDoubleZ		= false;
		pInputData->pureDoubleZ		= false;

	}

	if ( false == m_vecAttackImpossiblePtr.empty() ||
		false == m_vecZXImpossiblePtr.empty() )
	{
		pInputData->oneX			= false;
		pInputData->pureX			= false;
		pInputData->oneDoubleX		= false;
		pInputData->pureDoubleX		= false;

		pInputData->oneZ			= false;
		pInputData->pureZ			= false;
		pInputData->oneDoubleZ		= false;
		pInputData->pureDoubleZ		= false;
	}

	// Z ���� �Ұ�, X ���� �Ұ� ���� �߰�
	if ( false == m_vecZImpossiblePtr.empty() )
	{
		pInputData->oneZ			= false;
		pInputData->pureZ			= false;
		pInputData->oneDoubleZ		= false;
		pInputData->pureDoubleZ		= false;
	}

	if ( false == m_vecXImpossiblePtr.empty() )
	{
		pInputData->oneX			= false;
		pInputData->pureX			= false;
		pInputData->oneDoubleX		= false;
		pInputData->pureDoubleX		= false;
	}

	if( false == m_vecAttackImpossiblePtr.empty() ||
		false == m_vecSkillImpossiblePtr.empty() )
	{
		pInputData->oneA			= false;
		pInputData->pureA			= false;
		pInputData->oneDoubleA		= false;
		pInputData->pureDoubleA		= false;

		pInputData->oneS			= false;
		pInputData->pureS			= false;
		pInputData->oneDoubleS		= false;
		pInputData->pureDoubleS		= false;

		pInputData->oneD			= false;
		pInputData->pureD			= false;
		pInputData->oneDoubleD		= false;
		pInputData->pureDoubleD		= false;

		pInputData->oneC			= false;
		pInputData->pureC			= false;
		pInputData->oneDoubleC		= false;
		pInputData->pureDoubleC		= false;

		pInputData->oneC			= false;
		pInputData->pureC			= false;
		pInputData->oneDoubleC		= false;
		pInputData->pureDoubleC		= false;

		pInputData->oneQ			= false;
		pInputData->pureQ			= false;
		pInputData->oneDoubleQ		= false;
		pInputData->pureDoubleQ		= false;

		pInputData->oneW			= false;
		pInputData->pureW			= false;
		pInputData->oneDoubleW		= false;
		pInputData->pureDoubleW		= false;

		pInputData->oneE			= false;
		pInputData->pureE			= false;
		pInputData->oneDoubleE		= false;
		pInputData->pureDoubleE		= false;

		pInputData->oneR			= false;
		pInputData->pureR			= false;
		pInputData->oneDoubleR		= false;
		pInputData->pureDoubleR		= false;
	}

	if ( !m_vecDownImpossiblePtr.empty() )
	{
		pInputData->oneDown			= false;
		pInputData->pureDown		= false;
		pInputData->oneDoubleDown	= false;
		pInputData->pureDoubleDown	= false;
	}

	if( m_fCanNotJumpTime > 0.0f )
	{
		pInputData->oneUp			= false;
		pInputData->pureUp			= false;
		pInputData->oneDoubleUp		= false;
		pInputData->pureDoubleUp	= false;
	}

	if( !m_vecDashImpossiblePtr.empty() )
	{
		pInputData->oneDoubleRight	= false;
		pInputData->oneDoubleLeft	= false;
		pInputData->pureDoubleRight	= false;
		pInputData->pureDoubleLeft	= false;
	}

	if( m_ExtraDamagePack.m_Slow.m_fTime > 0.0f )
	{
		pInputData->oneDoubleRight	= false;
		pInputData->oneDoubleLeft	= false;
		pInputData->pureDoubleRight	= false;
		pInputData->pureDoubleLeft	= false;
	}

	if ( !m_vecReverseLeftRightPtr.empty() )
	{
		if ( m_vecReverseLeftRightPtr[0]->GetReverse() )
		{
			std::swap( pInputData->pureLeft,		pInputData->pureRight );
			std::swap( pInputData->oneLeft,			pInputData->oneRight );
			std::swap( pInputData->pureDoubleLeft,  pInputData->pureDoubleRight );
			std::swap( pInputData->oneDoubleLeft,	pInputData->oneDoubleRight );
		}
	}

	//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
	if ( 0.0f < m_ExtraDamagePack.m_Panic.m_fTime  )
	{
		m_ExtraDamagePack.m_Panic.m_fElapsedTimeToEffectDamage += m_fElapsedTime;

		if ( 2.0f < m_ExtraDamagePack.m_Panic.m_fElapsedTimeToEffectDamage )
		{
			m_ExtraDamagePack.m_Panic.m_fElapsedTimeToEffectDamage = 0.0f;
			m_ExtraDamagePack.m_Panic.m_bCorrectDirection = !( m_ExtraDamagePack.m_Panic.m_bCorrectDirection );
		}

		if ( false == m_ExtraDamagePack.m_Panic.m_bCorrectDirection )
		{
			std::swap( pInputData->pureLeft,		pInputData->pureRight );
			std::swap( pInputData->oneLeft,			pInputData->oneRight );
			std::swap( pInputData->pureDoubleLeft,  pInputData->pureDoubleRight );
			std::swap( pInputData->oneDoubleLeft,	pInputData->oneDoubleRight );
		}
	}
	//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

	m_BeforeInputData	= m_InputData;
	m_InputData			= *pInputData;	// kimhc // m_InputData�� GUUser�� InputData, pInputData�� ����Ű�� m_InputData�� X2Game�� InputData �̴�.

	InitInpuDataProcess();

//#ifdef TRANSFORMER_TEST
//
//	if( true == GetTransformed() &&
//		NULL != GetTransformer() )
//	{
//		GetTransformer()->SetInputData( &m_InputData );
//
//		m_InputData.Init();
//
//	}
//
//#endif TRANSFORMER_TEST

}




// commmoncameramove �Լ��� �ٰ� ������
//void CX2GUUser::PlayLuaSubCamera( int iCameraTableIndex )
//{
//	std::vector< float > vecSubCameraTiming;
//
//	if( m_LuaManager.BeginTable( "CAMERA", iCameraTableIndex ) == true )
//	{
//		if( m_LuaManager.BeginTable( "SUB_CAMERA_TIMING" ) == true )
//		{
//			float fSubCameraTiming = 0.f;
//			for( int i=1; ; i++ )
//			{
//				if( true == m_LuaManager.GetValue( i, fSubCameraTiming ) )
//				{
//					vecSubCameraTiming.push_back( fSubCameraTiming );
//				}
//				else
//				{
//					break;
//				}
//			}
//			m_LuaManager.EndTable(); // SUB_CAMERA_TIMING
//		}
//
//		if( false == vecSubCameraTiming.empty() )
//		{
//			bool bDone = false;
//			for( int i=0; i<(int)vecSubCameraTiming.size(); i++ )
//			{
//				float fTiming = vecSubCameraTiming[i];
//
//				if( m_pXSkinAnim->GetNowAnimationTime() < fTiming )
//				{
//					char tableName[64] = L"";
//					StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", i );
//					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
//					bDone = true;
//					break;
//				}
//			}
//
//			if( false == bDone )
//			{
//				char tableName[64] = L"";
//				StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", (int)vecSubCameraTiming.size() );
//				g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
//			}
//		}
//
//		m_LuaManager.EndTable(); // CAMERA
//	}
//}
//
//

void CX2GUUser::InitStateCommonRandom( OUT std::string &tableNameStartUTF8, OUT std::string &tableNameWinUTF8, OUT std::string &tableNameLoseUTF8 )
{

	int iRandomPercent = rand() % 100;
	int iAccumulatedPercent = 0;
	BOOST_TEST_FOREACH( const CommonRandomState&, commonRandomState, m_vecRandomStartState )
	{
		iAccumulatedPercent += commonRandomState.m_iPercent;
		if( iRandomPercent < iAccumulatedPercent )
		{
			tableNameStartUTF8 = commonRandomState.m_StateName;
			break;
		}
	}
	ASSERT( false == tableNameStartUTF8.empty() );




	iRandomPercent = rand() % 100;
	iAccumulatedPercent = 0;
	BOOST_TEST_FOREACH( const CommonRandomState&, commonRandomState, m_vecRandomWinState )
	{
		iAccumulatedPercent += commonRandomState.m_iPercent;
		if( iRandomPercent < iAccumulatedPercent )
		{
			tableNameWinUTF8 = commonRandomState.m_StateName;
			break;
		}
	}
	ASSERT( false == tableNameWinUTF8.empty() );



	iRandomPercent = rand() % 100;
	iAccumulatedPercent = 0;
	BOOST_TEST_FOREACH( const CommonRandomState&, commonRandomState, m_vecRandomLoseState )
	{
		iAccumulatedPercent += commonRandomState.m_iPercent;
		if( iRandomPercent < iAccumulatedPercent )
		{
			tableNameLoseUTF8 = commonRandomState.m_StateName;
			break;
		}
	}
	ASSERT( false == tableNameLoseUTF8.empty() );
}

bool CX2GUUser::DoScaleHeadBone()
{
	return false;
}

void CX2GUUser::ScaleHead()
{
	if( m_pXSkinAnim != NULL )
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame(L"Bip01_Head");
		if( pFrame != NULL )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
			SetFrameScale(&vScale, false, pFrame);
		}

		pFrame = m_pXSkinAnim->GetCloneFrame(L"COLLISION_SPHERE1_Face");
		if(pFrame != NULL )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
			SetFrameScale(&vScale, false, pFrame);
		}
	}
}

void CX2GUUser::RestoreHead()
{
	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame(L"COLLISION_SPHERE1_Face");
	if(pFrame != NULL)
	{
		SetFrameScaleRestore(pFrame);
		//D3DXVECTOR3 vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		//SetFrameScale(&vScale, false, pFrame);
	}

	pFrame = m_pXSkinAnim->GetCloneFrame(L"Bip01_Head");
	if(pFrame != NULL)
	{
		SetFrameScaleRestore(pFrame);
	}

	SetUnitReForm();
}



/** @function	: ScaleHand
	@brief		: ����� �� ��� ȿ���� ���� �� �� ���� ������ ����
*/
void CX2GUUser::ScaleHand( D3DXVECTOR3 vChangeScale )
{
	if( m_pXSkinAnim != NULL )
	{
		const wstring wstrBoneName[] = { L"Bip01_R_Hand", L"Bip01_L_Hand" };

		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;

		for ( int i = 0; i < ARRAY_SIZE( wstrBoneName ); ++i )
		{
			pFrame = m_pXSkinAnim->GetCloneFrame( wstrBoneName[i].c_str() );

			if(pFrame != NULL)
				SetFrameScale(&vChangeScale, true, pFrame);
		}

		/// �̺�� ���� ��� ũ�⸦ �������� ����
		if ( NULL != GetUnit() && CX2Unit::UT_EVE == GetUnit()->GetType() )
		{
			pFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );

			if(pFrame != NULL)
				SetFrameScale(&vChangeScale, true, pFrame);

			pFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );

			if(pFrame != NULL)
				SetFrameScale(&vChangeScale, true, pFrame);
		}
	}
}

/** @function	: RestoreHand
	@brief		: ����� �� ��� ȿ���� ���Ͽ� ������ �� �� ���� ������ ����
*/
void CX2GUUser::RestoreHand()
{
	const wstring wstrBoneName[] = { L"Bip01_R_Hand", L"Bip01_L_Hand" };

	CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = NULL;

	for ( int i = 0; i < ARRAY_SIZE( wstrBoneName ); ++i )
	{
		pFrame = m_pXSkinAnim->GetCloneFrame( wstrBoneName[i].c_str() );

		if(pFrame != NULL)
			SetFrameScaleRestore(pFrame);
	}

	/// �̺��� Ŀ�� ��� ũ�⸦ ���� ��Ű��
	if ( NULL != GetUnit() && CX2Unit::UT_EVE == GetUnit()->GetType() )
	{
		pFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );

		if(pFrame != NULL)
			SetFrameScaleRestore(pFrame);

		pFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );

		if(pFrame != NULL)
			SetFrameScaleRestore(pFrame);
	}

	SetUnitReForm();
}

void CX2GUUser::SetUnitReForm()
{
	if( m_pXSkinAnim == NULL )
		return;

	float fMoveBip01 = 10.f;
	float fMoveFootStep = -15.f;
	D3DXVECTOR3 vHead = D3DXVECTOR3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vThigh = D3DXVECTOR3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vCalf = D3DXVECTOR3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vFoot = D3DXVECTOR3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vHand = D3DXVECTOR3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vSpine = D3DXVECTOR3(1.f, 1.f, 1.f);

#if X2_DEV_TOOLS
	// �ǽð� �׽�Ʈ��
	FILE *fp = fopen("reform.txt", "r");
	if(fp != NULL)
	{
		fscanf(fp, "%f%f", &fMoveBip01, &fMoveFootStep);
		fscanf(fp, "%f%f%f", &vHead.x, &vHead.y, &vHead.z);
		fscanf(fp, "%f%f%f", &vThigh.x, &vThigh.y, &vThigh.z);
		fscanf(fp, "%f%f%f", &vCalf.x, &vCalf.y, &vCalf.z);
		fscanf(fp, "%f%f%f", &vFoot.x, &vFoot.y, &vFoot.z);
		fscanf(fp, "%f%f%f", &vHand.x, &vHand.y, &vHand.z);
		fscanf(fp, "%f%f%f", &vSpine.x, &vSpine.y, &vSpine.z);

		fclose(fp);
	}
#endif

	if ( m_pUnit != NULL )
	{
		switch ( m_pUnit->GetType() )
		{
		case CX2Unit::UT_ELSWORD:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 10.f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -10.f );

				D3DXVECTOR3 vHead	= D3DXVECTOR3( 0.93f, 0.90f, 0.93f );
				D3DXVECTOR3 vThigh	= D3DXVECTOR3( 1.08f, 1.f, 0.98f );
				D3DXVECTOR3 vCalf	= D3DXVECTOR3( 1.12f, 0.97f, 0.94f );
				D3DXVECTOR3 vFoot	= D3DXVECTOR3( 0.895f, 0.94f, 0.94f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Thigh" );
				if ( pFrame_R_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_R_Leg );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Thigh" );
				if ( pFrame_L_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_L_Leg );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );
			} break;

		case CX2Unit::UT_ARME:
			{
				if( IsTransformed() )
				{
					SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame(L"Bip01"), 5.f);
					SetFrameMoveBoneY( m_pXSkinAnimFuture->GetCloneFrame(L"Bip01"), 5.f);
				}
				else
				{
					SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 4.f );
					SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -4.f );
				}

				D3DXVECTOR3 vHead		= D3DXVECTOR3( 0.95f, 0.93f, 0.93f );
				D3DXVECTOR3 vThigh		= D3DXVECTOR3( 1.f, 1.f, 1.f );
				D3DXVECTOR3 vCalf		= D3DXVECTOR3( 1.1f, 1.f, 1.f );
				D3DXVECTOR3 vHand		= D3DXVECTOR3( 0.9f, 0.93f, 0.93f );
				D3DXVECTOR3 vFoot		= D3DXVECTOR3( 0.9f, 1.f, 1.f );
				D3DXVECTOR3 vDummyHand	= D3DXVECTOR3( 1.f, 1.f, 1.f );

				m_fBasicBoneScale = 1.f;

				if( IsTransformed() )
				{
					m_fBasicBoneScale = 1.1f;
					vHead = D3DXVECTOR3( 0.93f, 0.88f, 0.9f );
					vThigh = D3DXVECTOR3( 1.04f, 1.f, 1.f );
					vCalf = D3DXVECTOR3( 1.1f, 1.f, 1.f );
					vFoot = D3DXVECTOR3( 0.9f, 1.f, 0.9f );
					vHand = D3DXVECTOR3( 0.8f, 0.86f, 0.86f );
				}

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Thigh" );
				if ( pFrame_R_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_R_Leg );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Thigh" );
				if ( pFrame_L_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_L_Leg );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f / vHand.x;
				vDummyHand.y = 1.f / vHand.y;
				vDummyHand.z = 1.f / vHand.z;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			}

		case CX2Unit::UT_LIRE:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 1.0f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -1.0f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.94f, 0.92f, 0.90f );
				D3DXVECTOR3 vThigh = D3DXVECTOR3( 1.02f, 1.f, 1.f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.02f, 1.f, 1.f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 0.93f, 0.93f, 0.93f );
				D3DXVECTOR3 vHand = D3DXVECTOR3( 0.9f, 0.9f, 0.9f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Thigh" );
				if ( pFrame_R_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_R_Leg );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Thigh" );
				if ( pFrame_L_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_L_Leg );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame(L"Bip01_R_Hand");
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f / vHand.x;
				vDummyHand.y = 1.f / vHand.y;
				vDummyHand.z = 1.f / vHand.z;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			} break;

		case CX2Unit::UT_RAVEN:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 5.0f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -5.0f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.88f, 0.9f, 0.92f );
				D3DXVECTOR3 vThigh = D3DXVECTOR3( 1.05f, 0.94f, 0.92f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.065f, 0.93f, 0.9f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 0.93f, 0.85f, 0.83f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );
				D3DXVECTOR3 vSpine = D3DXVECTOR3( 0.94f, 0.97f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Thigh" );
				if ( pFrame_R_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_R_Leg );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Thigh" );
				if ( pFrame_L_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_L_Leg );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameSpine = m_pXSkinAnim->GetCloneFrame( L"Bip01_Spine" );
				if ( pFrameSpine != NULL )
					SetFrameScale( &vSpine, false, pFrameSpine );
			} break;

		case CX2Unit::UT_EVE:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), -0.8f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), 0.8f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.94f, 0.97f, 0.98f );
				D3DXVECTOR3 vThigh = D3DXVECTOR3( 1.0f, 0.98f, 0.98f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.0f, 0.98f, 0.98f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 0.95f, 0.95f, 0.95f );
				D3DXVECTOR3 vHand = D3DXVECTOR3( 0.9f, 0.96f, 0.96f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Thigh" );
				if ( pFrame_R_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_R_Leg );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Leg = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Thigh" );
				if ( pFrame_L_Leg != NULL )
					SetFrameScale( &vThigh, true, pFrame_L_Leg );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame(L"Bip01_R_Foot");
				if(pFrame_R_Foot != NULL)
					SetFrameScale(&vFoot, true, pFrame_R_Foot);
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame(L"Bip01_L_Foot");
				if(pFrame_L_Foot != NULL)
					SetFrameScale(&vFoot, true, pFrame_L_Foot);

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f;
				vDummyHand.y = 1.f;
				vDummyHand.z = 1.f;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			} break;

		case CX2Unit::UT_CHUNG:
			{
				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.93f, 0.93f, 0.93f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 0.92f, 0.92f, 0.92f );
				m_fBasicBoneScale = 1.08f;
				float fScale = m_fBasicBoneScale;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				vDummyHand = vDummyHand / fScale;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );

				m_pXSkinAnim->GetMatrix().Scale( D3DXVECTOR3( fScale, fScale, fScale ) );

				pFrameHead = m_pXSkinAnimFuture->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );
				pFrame_R_Dummy = m_pXSkinAnimFuture->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				pFrame_L_Dummy = m_pXSkinAnimFuture->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );

				m_pXSkinAnimFuture->GetMatrix().Scale( D3DXVECTOR3( fScale, fScale, fScale ) );
			} break;

		case CX2Unit::UT_ARA:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 2.0f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -2.0f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.96f, 0.96f, 0.96f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.02f, 1.f, 1.f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 1.0f, 0.96f, 0.96f );
				D3DXVECTOR3 vHand = D3DXVECTOR3( 0.96f, 0.96f, 0.96f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame(L"Bip01_R_Hand");
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f / vHand.x;
				vDummyHand.y = 1.f / vHand.y;
				vDummyHand.z = 1.f / vHand.z;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			} break;

		case CX2Unit::UT_ELESIS:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 2.0f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -2.0f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.96f, 0.96f, 0.96f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.02f, 1.f, 1.f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 1.0f, 0.96f, 0.96f );
				D3DXVECTOR3 vHand = D3DXVECTOR3( 0.96f, 0.96f, 0.96f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame(L"Bip01_R_Hand");
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f / vHand.x;
				vDummyHand.y = 1.f / vHand.y;
				vDummyHand.z = 1.f / vHand.z;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			} break;

		case CX2Unit::UT_ADD:
			{
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01" ), 2.0f );
				SetFrameMoveBoneY( m_pXSkinAnim->GetCloneFrame( L"Bip01_Footsteps" ), -2.0f );

				D3DXVECTOR3 vHead = D3DXVECTOR3( 0.93f, 0.93f, 0.93f );
				D3DXVECTOR3 vCalf = D3DXVECTOR3( 1.02f, 1.f, 1.f );
				D3DXVECTOR3 vFoot = D3DXVECTOR3( 1.0f, 0.96f, 0.96f );
				D3DXVECTOR3 vHand = D3DXVECTOR3( 0.96f, 0.96f, 0.96f );
				D3DXVECTOR3 vDummyHand = D3DXVECTOR3( 1.f, 1.f, 1.f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrameHead = m_pXSkinAnim->GetCloneFrame( L"Bip01_Head" );
				if ( pFrameHead != NULL )
					SetFrameScale( &vHead, true, pFrameHead );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Calf" );
				if ( pFrame_R_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_R_Calf );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Calf = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Calf" );
				if ( pFrame_L_Calf != NULL )
					SetFrameScale( &vCalf, true, pFrame_L_Calf );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
				if ( pFrame_R_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_R_Foot );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Foot = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
				if ( pFrame_L_Foot != NULL )
					SetFrameScale( &vFoot, true, pFrame_L_Foot );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Hand = m_pXSkinAnim->GetCloneFrame(L"Bip01_R_Hand");
				if ( pFrame_R_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_R_Hand );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Hand = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
				if ( pFrame_L_Hand != NULL )
					SetFrameScale( &vHand, true, pFrame_L_Hand );

				vDummyHand.x = 1.f / vHand.x;
				vDummyHand.y = 1.f / vHand.y;
				vDummyHand.z = 1.f / vHand.z;

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_R_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				if ( pFrame_R_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_R_Dummy );
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame_L_Dummy = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
				if ( pFrame_L_Dummy != NULL )
					SetFrameScale( &vDummyHand, true, pFrame_L_Dummy );
			} break;

		default:
			break;
		}
	}
}



//{{ kimhc // 2009-11-20 // ��� ��ų 3, 4����
// Ƚ���� �ð� ������
void CX2GUUser::GuildSkillProcess( CX2SkillTree::SKILL_ID eSkillID_ )
{
	switch ( eSkillID_ )
	{
	case CX2SkillTree::SI_GP_COMMON_CHANCE_TO_REVERSE:
		{
			if ( m_GuildSkillData.m_bLearnChanceToReverse )
				DoGuildSkillChanceToReverse();

// 			if ( m_GuildSkillData.m_ChangeToReverse.m_bProcessingBuff == false &&
// 				 GetNowStateID()	!= GUSI_DIE	)
// 			{
// 				m_GuildSkillData.m_ChangeToReverse.m_bProcessingBuff	= true;
// 				m_GuildSkillData.m_ChangeToReverse.m_byCountLeft		= (BYTE)(int)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT );
// 				m_GuildSkillData.m_ChangeToReverse.m_fTimeLeft			= 30.0f;
//
// 				if ( m_GuildSkillData.m_ChangeToReverse.m_hEffectSet == INVALID_EFFECTSET_HANDLE )
// 				{
// 					m_GuildSkillData.m_ChangeToReverse.m_hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ChanceToReverse", this, this );
// 					PlaySound( L"GS_Chance_To_Reverse01.ogg" );
// 				}
// 			}
		}
		break;

	case CX2SkillTree::SI_GP_COMMON_INFINITE_STRENGTH:
		{
			const CX2Unit::UnitData*	pUnitData	=	&GetUnit()->GetUnitData();

			const CX2SkillTree::SkillTemplet* pSkillTemplet =
				pUnitData->m_UserSkillTree.GetUserSkillTemplet( eSkillID_, pUnitData->m_byMemberShipGrade );

			if ( pSkillTemplet == NULL )
				return;

			const CX2UserSkillTree& userSkillTree = pUnitData->m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

			if ( m_GuildSkillData.m_InfiniteStrength.m_byCountLeft == 0 &&
				m_GuildSkillData.m_InfiniteStrength.m_hSeqEffect == INVALID_PARTICLE_SEQUENCE_HANDLE &&
				GetNowStateID() != GUSI_DIE &&
				GetRandomFloat() < pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillTempletLevel ) )
			{
				m_GuildSkillData.m_InfiniteStrength.m_byCountLeft = 3; // ������ 3
			}
		}

	default:
		// ������ ����� ������ �׾��ٴ� ��Ŷ�� ���� ��� ����, CX2DungeonGame, X2Game�� UserUnitDieNot ���� �����Ǿ�����
		break;
	}
}

void CX2GUUser::ProcessSacrificeOfHero()		// ������ ��� ��彺ų
{
	if ( g_pX2Game == NULL )
	{
		ASSERT( !L"g_pX2Game is NULL" );
		return;
	}

	m_bHaveSacrificeOfHero = true;
	m_RenderParam.renderType = CKTDGXRenderer::RT_CARTOON_FADE_NONTEX;

	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

	const CX2SkillTree::SkillTemplet* pSkillTemplet =
		pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_SACRIFICE_OF_HERO, pUnitData->m_byMemberShipGrade );

	if ( pSkillTemplet != NULL )
	{
		const CX2UserSkillTree& userSkillTree = pUnitData->m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

		float fPercent = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_HP_GAIN_REL_MAX_HP, iSkillTempletLevel );

		if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
			fPercent *= 0.5f;

		// ����Ʈ ����!!
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SacrificeOfHero", this );

		PlaySound( L"GS_Sacrifice_of_Hero.ogg" );

		for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit *pGameUnit = g_pX2Game->GetUnit( i );
			if ( pGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
			{
				CX2GUUser* pPartyUser = (CX2GUUser*)g_pX2Game->GetUnit( i );

				if ( pPartyUser != NULL )
				{
					if ( GetDistance( GetPos(), pPartyUser->GetPos() ) < 1500.0f &&
						GetTeam() == pPartyUser->GetTeam() &&
						pPartyUser->GetNowStateID() != GUSI_DIE &&
						GetUnitUID() != pPartyUser->GetUnitUID() &&
						( g_pX2Game->GetGameType() == CX2Game::GT_PVP ||
						( NULL != g_pData && NULL != g_pData->GetPartyManager() && true == g_pData->GetPartyManager()->IsMyPartyMember( pPartyUser->GetUnitUID() ) ) ) )
					{
							float fHpToBeUp = pPartyUser->GetMaxHp() * fPercent;

							pPartyUser->UpNowHp( fHpToBeUp );

							std::wstringstream strStream;
							strStream << L"HP+ " << static_cast< int >( fHpToBeUp );
							D3DXVECTOR3 pos = pPartyUser->GetPos();
							pos.y += 50.0f;
							if( NULL != g_pData->GetPicCharGreen() )
								g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pPartyUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
					}
				}
			}
		}
	}
}


//}} kimhc // 2009-11-20 // ��� ��ų 3, 4����


//{{ kimhc // 2010.4.1 // ��д��� �۾�(��Ʈ������ȿ��)
void CX2GUUser::Bone_Shield::DestroyParticle()
{
	// ����Ʈ�� ����
	if ( m_hEffectSet != INVALID_EFFECTSET_HANDLE )
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectSet );
}

float CX2GUUser::GetOriginalMPChangeRate()
{
	float m_fMpRate = m_fOriginalMPChangeRate;

	// �巡���� ��Ʈȿ���� ���� ���� ���̸�
	if ( GetManaRecoveryTime() > 0.0f )
		m_fMpRate += GetManaRecoveryVal();

	//{{ mauntain : ����ȯ [2012.07.26] Ŭ��ŷ �޸� ������ ��ٸ� ���� ���� ( �۾��� : ���ҵ� �븸 ������ )
	return m_fMpRate + m_fAdditionalMPChangeRateByPassive;
	//}}
}

float	CX2GUUser::GetNowMPChange()
{
	float fMpChageRate = GetChangeRateMp();


	//{{ dmlee 2008.7.13 play channel
	const float MAGIC_PLAY_CHANNEL_MP_CHANGE_RATE_SCALE = 7.f;
	if( CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID()
		&& KPVPChannelInfo::PCC_PLAY == g_pMain->GetPVPChannelClass( g_pMain->GetConnectedChannelID() ) )
	{
		fMpChageRate /= MAGIC_PLAY_CHANNEL_MP_CHANGE_RATE_SCALE;
	}
	//}} dmlee 2008.7.13 play channel

	return fMpChageRate;
}

bool CX2GUUser::InitSocketEffectEx( const CX2SocketItem::SocketData& kSocketData )
{
	switch ( kSocketData.m_eTypeSocketItemEffectEx )		// ��д����� �߰��� �ɼ��� �˻�
	{
	default:
	case CX2SocketItem::SIEE_NONE:
		return false;
		break;

	case CX2SocketItem::SIEE_STAT_UP:
	case CX2SocketItem::SIEE_UP_MANA_RECOVERY_VALUE:
	case CX2SocketItem::SIEE_CURE_DEBUFF:
	case CX2SocketItem::SIEE_DEBUFF_ATTACKER:
	case CX2SocketItem::SIEE_BONE_SHIELD:
	case CX2SocketItem::SIEE_ATTACK_SPEED_UP:

	//{{ kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage
	case CX2SocketItem::SIEE_ICE_NOVA:
	//}} kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage

	case CX2SocketItem::SIEE_POISON_BURST:
	case CX2SocketItem::SIEE_ABSORB_HP:

	case CX2SocketItem::SIEE_ABSORB_HP_NO_REACT:	/// �ǰ� �� HP ���� ��Ʈ ȿ�� ( ���� ���� )
	case CX2SocketItem::SIEE_ACTIVE_BUFF:			/// �ǰ� �� ���� ���� ȿ��
		break;
	}

	SOCKET_EFFECT_EX_MAP::iterator mItr = m_mapSocketItemEffectEx.find( kSocketData.m_eTypeSocketItemEffectEx );

	// �ش� ����ȿ�� Ÿ���� ���� �ʿ� ������
	if ( mItr == m_mapSocketItemEffectEx.end() )
	{
		vector<int> tempVec;
		tempVec.push_back( kSocketData.m_ID );
		m_mapSocketItemEffectEx.insert( make_pair( kSocketData.m_eTypeSocketItemEffectEx, tempVec ) );
	}
	else		// �ش� ����ȿ�� Ÿ���� �ʿ� ������
	{
		mItr->second.push_back( kSocketData.m_ID );
	}

	return true;

}

CX2SocketItem::SOCKET_ITEM_EFFECT_EX CX2GUUser::ProcessSocketEffectEx( const CX2SocketItem::SOCKET_ITEM_EFFECT_EX eEx_, CX2GameUnit* pGameUnit_ )
{
	SOCKET_EFFECT_EX_MAP::iterator mItr = m_mapSocketItemEffectEx.find( eEx_ );
	CX2SocketItem::SOCKET_ITEM_EFFECT_EX eReturnVal = CX2SocketItem::SIEE_NONE;

	if ( mItr == m_mapSocketItemEffectEx.end() )
		return eReturnVal;

	switch ( eEx_ )
	{
	case CX2SocketItem::SIEE_STAT_UP:
		{
			// ���� ���� ���� ������ ������ ���� ����
			if ( m_StatUpBufferBySocketItem.m_fBufferTime > 0.0f )
				return eReturnVal;

			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_STAT_PERCENT_UP ) )
				{
					// �ش� ������ ���� ��Ŵ
					SetStatUpBufferBySocketItem( pSocketData->m_fIncOrDecVal, pSocketData->m_fDurationTime );
					TRACE( L"Socket On\n" );
					m_StatUpType = pSocketData->m_eStatUpType;
					eReturnVal = CX2SocketItem::SIEE_STAT_UP;
					break;
				}
			}

		} break;

	case CX2SocketItem::SIEE_UP_MANA_RECOVERY_VALUE:
		{
			// ���� ���� ���� ������ ������ ���� ����
			if ( GetManaRecoveryTime() > 0.0f )
				return eReturnVal;

			vector<int> &vecOptionID = mItr->second;

			const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( vecOptionID[0] );
            if ( pSocketData != NULL )
            {
    			SetManaRecoveryVal( pSocketData->m_fIncOrDecVal );
	    		SetManaRecoveryTime( pSocketData->m_fDurationTime );
		    	ResetMPChangeRate( GetNowMPChange() + GetManaRecoveryVal() );
    			eReturnVal = CX2SocketItem::SIEE_UP_MANA_RECOVERY_VALUE;
            }

		} break;

	case CX2SocketItem::SIEE_CURE_DEBUFF:
		{
			// ���͹� �ð��� ������ �ʾ����� ȿ�� ���� ���� ����
			if ( m_fIntervalTimeForCureDebuff > 0.0f )
				return eReturnVal;

			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_CURE_DEBUFF ) )
				{
					CureExtraDamage( pSocketData->m_iCount );
					m_fIntervalTimeForCureDebuff = pSocketData->m_fIntervalTime;
					eReturnVal = CX2SocketItem::SIEE_CURE_DEBUFF;
					break;
				}
			}

		} break;

	case CX2SocketItem::SIEE_DEBUFF_ATTACKER:		// �ڽ��� ������ ���ֿ��� �����̻��� ����
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_DEBUFF_ATTACKER ) )
				{
					if ( NULL != pSocketData->m_ptrBuffFactor )
					{
						CX2BuffFactorPtr ptrBuffFactor = pSocketData->m_ptrBuffFactor->GetClonePtr( 0 );
						if ( NULL != ptrBuffFactor )
						{
							ptrBuffFactor->SetGameUnitBuffFactor( this );
							pGameUnit_->SetBuffFactorToGameUnit( *ptrBuffFactor );
							break;
						}
					}
					//CreateDamageDataForDebuffAttacker( pSocketData->m_SocketExtraDamage );
					//eReturnVal = CX2SocketItem::SIEE_DEBUFF_ATTACKER;
					break;
				}
			}
		} break;

	case CX2SocketItem::SIEE_BONE_SHIELD:
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_BONE_SHIELD ) )
				{
					CreateBoneShield( *pSocketData );
					eReturnVal = CX2SocketItem::SIEE_BONE_SHIELD;
					break;
				}
			}
		} break;

	case CX2SocketItem::SIEE_ATTACK_SPEED_UP:
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_SPEED_UP ) )
				{
					//SetAnimSpeedFactor( pSocketData->m_fIncOrDecRate, pSocketData->m_fDurationTime, SFI_SPEED_2, SFST_ONLY );
					SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_MECHANIZATION_SPEED_UP );
					break;
				}
			}
		} break;

	//{{ kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage
	case CX2SocketItem::SIEE_ICE_NOVA:
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_ICE_NOVA ) )
				{
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SIEE_ICE_NOVA", this );

					eReturnVal = CX2SocketItem::SIEE_ICE_NOVA;
					break;
				}
			}
		} break;
//}} kimhc // 2010.12.31 // �ϸ� ���� �߰��� ���� ExtraDamage

	case CX2SocketItem::SIEE_POISON_BURST:
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_POISON_BURST ) )
				{
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SIEE_POISON_BURST", this );

					eReturnVal = CX2SocketItem::SIEE_POISON_BURST;
					break;
				}
			}
		} break;
	case CX2SocketItem::SIEE_ABSORB_HP:
		{
			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_ABSORB_HP ) )
				{
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SIEE_ABSORB_HP", this );

					eReturnVal = CX2SocketItem::SIEE_ABSORB_HP;
					break;
				}
			}
		} break;

	case CX2SocketItem::SIEE_ABSORB_HP_NO_REACT:	/// �ǰ� �� HP ���� ��Ʈ ȿ�� ( ���� ���� )
		{

			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_ABSORB_HP ) )
				{
					D3DXVECTOR3 vPos = GetBonePos(L"Bip01");	/// ���� ��ġ

					/// ���� ������ ����Ʈ ����
					CX2DamageEffect::CEffect* pCEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
													L"DAMAGE_EFFECT_SIEE_ABSORB_HP_NO_REACT", GetPowerRate(),
													vPos, GetDirVector(), GetDirVector(),
													m_FrameDataNow.unitCondition.landPosition.y );
					/// Ÿ�ݼ� ����
					if ( NULL != pCEffect )
						pCEffect->SetDamageTime( static_cast<int>( pSocketData->m_fEffectiveValue ) );

					// ������ // 2013-08-28 // DamageTime�� 0�̵Ǹ� DamageEffect�� ���ŵǱ� ������ ������ ���� ����Ʈ�� �߰�
					if( NULL != g_pX2Game->GetEffectSet() )
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Siee_Absorb_Hp", this );

					eReturnVal = CX2SocketItem::SIEE_ABSORB_HP_NO_REACT;


					break;
				}
			}
		} break;

	case CX2SocketItem::SIEE_ACTIVE_BUFF:			/// �ǰ� �� ���� ���� ȿ��
		{

			BOOST_TEST_FOREACH( int, val, mItr->second )
			{
				const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( val );

				/// Ȯ�� ���ǿ� �����ǰ�, ������ ������ ���� ��
				if ( pSocketData != NULL && pSocketData->m_fProbability > GetRandomFloat( CKTDXRandomNumbers::SRO_ACTIVE_BUFF ) &&
					NULL != pSocketData->m_ptrBuffFactorCustomUse )
				{
					/// Ŭ�� ���� ����
					CX2BuffFactorPtr pCloneBuffFactorPtr = pSocketData->m_ptrBuffFactorCustomUse->GetClonePtr( 0 );

					/// ���� ����
					if ( NULL != pCloneBuffFactorPtr )
						SetBuffFactorToGameUnit( *pCloneBuffFactorPtr );

					eReturnVal = CX2SocketItem::SIEE_ACTIVE_BUFF;


					break;
				}
			}
		} break;

	default:
		break;
	}

	return eReturnVal;
}

void CX2GUUser::CheckNChangeManaRecoveryValueNTime( float fElpasedTime )
{
	if ( GetManaRecoveryTime() > 0.0f )
	{
		SetManaRecoveryTime( GetManaRecoveryTime() - fElpasedTime );

		if ( GetManaRecoveryTime() <= 0.0f )
		{
			SetManaRecoveryTime( 0.0f );
			ResetMPChangeRate( GetNowMPChange() - GetManaRecoveryVal() );
			SetManaRecoveryVal( 0.0f );
		}
	}
}

void CX2GUUser::CheckNChangeIntervalTimeForCureDebuff( float fElpasedTime )
{
	if ( GetIntervalTimeForCureDebuff() > 0.0f )
	{
		SetIntervalTimeForCureDebuff( GetIntervalTimeForCureDebuff() - fElpasedTime );

		if ( GetIntervalTimeForCureDebuff() <= 0.0f )
			SetIntervalTimeForCureDebuff( 0.0f );
	}
}

void CX2GUUser::CreateBoneShield( const CX2SocketItem::SocketData& kSocketData )
{
//{{ robobeg : 2013-11-04
    SAFE_DELETE( m_pBoneShieldData );
//}} robobeg : 2013-11-04

	m_pBoneShieldData = new Bone_Shield;

	m_pBoneShieldData->m_fIntervalTime	= kSocketData.m_fIntervalTime;
	m_pBoneShieldData->m_fTimeLeft		= kSocketData.m_fDurationTime;
	m_pBoneShieldData->m_byCountLeft	= kSocketData.m_iCount;
	m_pBoneShieldData->m_fReducePercent	= kSocketData.m_fIncOrDecRate;

	if ( kSocketData.m_vecEffectName.size() != 2 )
	{
		ASSERT( !L"m_vecEffectName size is fault" );
		m_pBoneShieldData->m_hEffectSet		= g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_BesmaHell_Set_Barrier_Wait", this, this );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_BesmaHell_Set_Barrier", this, this );
	}
	else
	{
		m_pBoneShieldData->m_hEffectSet		= g_pX2Game->GetEffectSet()->PlayEffectSet( kSocketData.m_vecEffectName[0], this, this );
		m_pBoneShieldData->m_wstrEffectName = kSocketData.m_vecEffectName[1];		// Ÿ�� ������ �� ���� ����Ʈ �� ��
	}

	g_pX2Game->GetEffectSet()->PlayEffectSet( m_pBoneShieldData->m_wstrEffectName, this, this );
}

void CX2GUUser::CheckAndDeleteBoneShield( float fElpasedTime )
{
	if ( m_pBoneShieldData != NULL )
	{
		m_pBoneShieldData->m_fTimeLeft		-= fElpasedTime;
		m_pBoneShieldData->m_fIntervalTime	-= fElpasedTime;

		if ( m_pBoneShieldData->m_hEffectSet != INVALID_EFFECTSET_HANDLE
			 && ( m_pBoneShieldData->m_fTimeLeft <= 0.0f || m_pBoneShieldData->m_byCountLeft <= 0 ) )
			m_pBoneShieldData->DestroyParticle();

		if ( m_pBoneShieldData->m_fIntervalTime <= 0.0f && m_pBoneShieldData->m_hEffectSet == INVALID_EFFECTSET_HANDLE )
			DeleteBoneShield();
	}
}

// kimhc // 2010-11-01 GameUnit���� �ű�
// bool CX2GUUser::GreaterExtraDamageTime( CX2DamageManager::ExtraDamageData* pFirst, CX2DamageManager::ExtraDamageData *pSecond )
// {
// 	return pFirst->m_fTime > pSecond->m_fTime;
// }


//}} kimhc // 2010.4.1 // ��д��� �۾�(��Ʈ������ȿ��)



//{{ kimhc // 2010.6.16
// 1. GameUnit::Weapon::SetEnchantParticleShow() �� GUUser::SetShowEnchantedWeaponEffectAtHand()��
// ���� ���� ��찡 �����Ƿ� �ѵ� ����
// 2. NotifyShowObjectChanged()������ ���� ���� ȣ���ϵ��� �Ǿ��ִ°��� �Ѱ������� ȣ���ϵ��� ����
// 3. virtual�� ���� GameUnit������ GameUnit::Weapon::SetEnchantParticleShow()�� ȣ���ϰ�
// GUUser������ SetShowEnchantedWeaponEffectAtHand()�� ȣ���ϵ��� ����
/*virtual*/ void CX2GUUser::SetShowEnchantWeaponParticle( bool bShow )
{
	CX2GameUnit::SetShowEnchantWeaponParticle( bShow );
	SetShowEnchantedWeaponEffectAtHand( bShow );
}

void CX2GUUser::CommonStartStateEnd()
{
// 	CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
// 	if( pPet != NULL )
// 	{
// 		pPet->InitGamePet();
// 	}
}

void CX2GUUser::WinStateStart()
{
	CommonStateStart();

	if( g_pX2Game != NULL && g_pX2Game->IsHost() == true )
	{
		CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
		if( pPet != NULL )
		{
			int iRand = rand() % 2;
			if( iRand == 0 )
				pPet->StateChange( pPet->m_Happy1State, true, true );
			else
				pPet->StateChange( pPet->m_Happy2State, true, true );
		}
	}
}
void CX2GUUser::LoseStateStart()
{
	CommonStateStart();

	if( g_pX2Game != NULL && g_pX2Game->IsHost() == true )
	{
		CX2PET *pPet = g_pData->GetPetManager()->GetPet( GetUnitUID() );
		if( pPet != NULL )
		{
			pPet->StateChange( pPet->m_SadState, true, true );
		}
	}
}


/** @function : CreateStepDust
	@brief : ��� ���� ���¿��� ���θ��� eDustType�� ���� �ٸ� ��ƼŬ�� �����ֱ� ���� �Լ�
	@param : void
	@return : void
*/
/*virtual*/ void CX2GUUser::CreateStepDust()
{
	CKTDGParticleSystem*	pMinorParticleSystem = g_pX2Game->GetMinorParticle();
	CX2World*				pWorld = g_pX2Game->GetWorld();

	if( NULL != pWorld &&
		NULL != pWorld->GetLineMap() )
	{
		const CKTDGLineMap::LineData* pLineData = pWorld->GetLineMap()->GetLineData( m_FrameDataNow.syncData.lastTouchLineIndex );

		if( NULL != pLineData )
		{
			switch( pLineData->eDustType )
			{
			default:
			case CKTDGLineMap::LDT_DEFALT_DUST:
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STEP_SMOKE ) );
					PlaySequenceByTriggerCount( pSeq, m_FrameDataNow.unitCondition.landPosition, 100, 100, 5 );
				} break;

			case CKTDGLineMap::LDT_WATER_SPLASH:
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STEP_WATER ) );
					PlaySequenceByTriggerCount( pSeq, m_FrameDataNow.unitCondition.landPosition, 100, 100, 12 );

				} break;
			}
		}
	}
	else	// World or LineMap�� ������
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = pMinorParticleSystem->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STEP_SMOKE ) );
		PlaySequenceByTriggerCount( pSeq, m_FrameDataNow.unitCondition.landPosition, 100, 100, 5 );
	}
}

//{{ kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��
void CX2GUUser::SetHoldStateByReactType( CX2DamageManager::REACT_TYPE eReactType /*= CX2DamageManager::RT_DOWN */ )
{
	switch ( eReactType )
	{
	default:
	case CX2DamageManager::RT_DOWN:
		SetHoldStateID( USI_DAMAGE_AIR_FALL );
		break;
	}
}
//}} kimhc // 2010.7.12 // �������� ������ HOLD �� �� �ֵ��� ��

/** @function : PlayCommonBuffMinorParticle
	@brief : CommonBuffFrame���� ���̴� ��ƼŬ ���
*/
void CX2GUUser::PlayCommonBuffMinorParticle()
{
	D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );

	CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
	PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  3 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqWhiteShockWave  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_WHITE_SHOCK_WAVE ) );
	PlaySequenceByTriggerCount( pSeqWhiteShockWave, pos, 10, 10,  1 );

	CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTick		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_IMPACT_TICK ) );
	PlaySequenceByTriggerCount( pSeqImpactTick, pos,  200, 200,  30 );

	if( pSeqImpactTick != NULL )
	{
		pSeqImpactTick->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
	}
}

/** @function : GetSkillSlotIndexUsed
	@brief : ���� ������ �ε��� �� B���� ��뿩�θ� ���� ( �� Ŭ������ COMMON_BUFF_FrameMove�� �ִ� ���� �Űܿ� )
*/
bool CX2GUUser::GetSkillSlotIndexUsed( OUT int& iSkillSlotIndex_, OUT bool& bSlotB_ )
{
	//const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

	//���� ���� ��ų�� ���� �˾Ƴ���
	switch( m_NowStateData.stateID )
	{
	case USI_SPECIAL_ATTACK_1:
	case USI_SPECIAL_ATTACK_HYPER_1:
		{
			iSkillSlotIndex_ = 0;
		} break;
	case USI_SPECIAL_ATTACK_2:
	case USI_SPECIAL_ATTACK_HYPER_2:
		{
			iSkillSlotIndex_ = 1;
		} break;
	case USI_SPECIAL_ATTACK_3:
	case USI_SPECIAL_ATTACK_HYPER_3:
		{
			iSkillSlotIndex_ = 2;
		} break;
	case USI_SPECIAL_ATTACK_4:
	case USI_SPECIAL_ATTACK_HYPER_4:
		{
			iSkillSlotIndex_ = 3;
		} break;

	case USI_SPECIAL_ATTACK_SLOTB_1:
	case USI_SPECIAL_ATTACK_SLOTB_HYPER_1:
		{
			iSkillSlotIndex_ = 0;
			bSlotB_ = true;
		} break;
	case USI_SPECIAL_ATTACK_SLOTB_2:
	case USI_SPECIAL_ATTACK_SLOTB_HYPER_2:
		{
			iSkillSlotIndex_ = 1;
			bSlotB_ = true;
		} break;
	case USI_SPECIAL_ATTACK_SLOTB_3:
	case USI_SPECIAL_ATTACK_SLOTB_HYPER_3:
		{
			iSkillSlotIndex_ = 2;
			bSlotB_ = true;
		} break;
	case USI_SPECIAL_ATTACK_SLOTB_4:
	case USI_SPECIAL_ATTACK_SLOTB_HYPER_4:
		{
			iSkillSlotIndex_ = 3;
			bSlotB_ = true;
		} break;

	default:
		ASSERT( !L"Wrong Path" );
		return false;
		break;
	}

	return true;
}


//{{ kimhc // 2010.12.6 // 2010-12-23 New Character CHUNG
/** @function : SetDamageData
	@brief : �� ������Ʈ�� FrameMove �߰��� DAMAGE_DATA Ÿ���� �����ϱ� ���� �Լ�
	@param :  const wstring& wstrTableName_ (������ ������ ���̺� ����)
*/            //<ADD_MEMO_1ST_CLASS>  const bool IsEqippedMemo_ (�޸� ������� ����)
bool CX2GUUser::SetDamageData( const char* pszTableNameUTF8_, const bool IsEqippedMemo_ /*= false*/ )
{
	bool bIsSetDamageData = g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_DamageData, m_LuaManager, pszTableNameUTF8_, GetPowerRate(), IsEqippedMemo_ );

	if( m_DamageData.attackType != CX2DamageManager::AT_SPECIAL &&
		false == IsSamef(m_DamageData.fForceDownValue, 0.f) )
	{
		// �нú� ��ų�� ���� �����ٿ� ��ġ ����
		if( 1.f != m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack() )
		{
			m_DamageData.fForceDownValue *= m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack();//+= CalulateIncreaseingRate( m_SkillRelatedData.m_fForceDownValueRateOfMeleeAttack );
		}
	}
	ChangeDamageData ();				  // �нú꿡 ������ �޴� Ŀ�ǵ� ��, �������� �Ͼ�� ���� ���� ó���� ��ũ��Ʈ, �� ĳ���� �ҽ� �ڵ� ������ ó�� �� �� ���� ��, �ϵ� �ڵ�

	return bIsSetDamageData;
}

/** @function : CreateWeapon
	@brief : ������ ���ø��� EquipPtr�� ����Ͽ� Weapon ��ü�� ����,
	@param :  IN const CX2Item::ItemTemplet* pItemTemplet (���� �������� ���ø�)
*/
void CX2GUUser::CreateWeapon(
    IN const CX2Item::ItemTemplet* pItemTemplet_,
    CX2EqipPtr pEquipWeaponPtr_ )
{
	if ( pEquipWeaponPtr_ == NULL ||
        NULL == pItemTemplet_
        )
		return;

	int iWeaponIndex = 0;
	switch ( pItemTemplet_->GetEqipPosition() )
	{
	case CX2Unit::EP_WEAPON_HAND:
		iWeaponIndex = 0;
		break;

	case CX2Unit::EP_WEAPON_SECOND:
		iWeaponIndex = 1;
		break;

	default:
		return;
		break;
	}

	//{{ robobeg : 2008-10-28
	CX2GameUnit::Weapon* pWeapon = CX2GameUnit::Weapon::CreateWeapon( *this, pItemTemplet_ );
	ASSERT( pWeapon != NULL );
	//}} robobeg : 2008-10-28
	pWeapon->m_iWeaponIndex			= iWeaponIndex;
	pWeapon->m_pUserWeaponEquip		= pEquipWeaponPtr_;
	pWeapon->m_bUseSlashTrace		= true;

	if( NULL != GetUnit() )
	{
		pItemTemplet_->GetSlashTraceColors( pWeapon->m_coSlashTrace, pWeapon->m_coSlashTraceTip
			, pWeapon->m_coSlashTraceHyper, pWeapon->m_coSlashTraceTipHyper, GetUnit()->GetType() );
	}

	m_vecpWeapon.push_back( pWeapon );

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetMajorParticle() )
	{
        DWORD   dwNumParticles = pItemTemplet_->GetNumAttachedParticleData();
		for( UINT j=0; j<dwNumParticles; j++ )
		{
            const wchar_t*  pwszParticleName = L"";
            const wchar_t*  pwszBoneName = L"";
            pItemTemplet_->GetAttachedParticleData( j, &pwszParticleName, &pwszBoneName );
			CKTDGParticleSystem::CParticleEventSequence* pSeq =
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwszParticleName, D3DXVECTOR3( 0, 0, 0 ) );
			if( NULL != pSeq )
			{
				AttachWeaponParticle( pWeapon->m_iWeaponIndex, true, pwszBoneName, pSeq->GetHandle() );
			}
		}
	}
}

/** @function : UpdateNotRenderingOtherEquipList
	@brief : AC_FACE1 �Ǽ��縮�� ���� ������ ���� ���ƾ��� �����۵��� m_ViewEqipList���� false ������
*/
void CX2GUUser::UpdateNotRenderingOtherEquipList()
{
	SetHideNoRenderable(false);

	m_pHairXSkinMesh = NULL;

	// oasis907 : ����� [2011.8.22] �ѹ� ���� �ƹ�Ÿ
	bool bOnePieceAvatarCheck = false;

	CX2EqipPtr pNoRenderFactorEquipBody = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_ONEPIECE_FASHION );

	if( pNoRenderFactorEquipBody != NULL )
	{
		if ( IsOnePieceAvatarRenderHair( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				if( IsOnePieceAvatarWithNotRenderHairItem( pViewEquip->GetItemID() ) == true )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE2:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
				else
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
		}
		if ( IsOnePieceAvatar( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_DEFENCE_BODY:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
					}
					break;

				default:
					break;
				}
			}
		}
		if ( IsOnePieceAvatarNotRenderFace1( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_AC_FACE1:
				case CX2Unit::EP_DEFENCE_BODY:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
					}
					break;

				default:
					break;
				}
			}
		}
		if ( IsOnePieceAvatarNotRenderFace1Face2( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_AC_FACE1:
				case CX2Unit::EP_AC_FACE2:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
					}
					break;

				default:
					break;
				}
			}
		}
		if ( IsOnePieceAvatarNotRenderFace( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			SetHideNoRenderable(true);
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_FACE:
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_AC_FACE2:
				case CX2Unit::EP_AC_FACE3:
				case CX2Unit::EP_DEFENCE_BODY:
					{
						pViewEquip->SetShowObject( false );
						bOnePieceAvatarCheck = true;
					}
					break;

				default:
					break;
				}
			}
		}
		/// �ѹ� �ƹ�Ÿ �ܿ� ���� ��� �Ǽ������� ���� �ϴ� ������
		if ( IsOnePieceAvatarNotRenderHairAndAllAccessory( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						bOnePieceAvatarCheck = true;
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_AC_FACE1:
				case CX2Unit::EP_AC_FACE2:
				//case CX2Unit::EP_AC_FACE3:	/// ��û���� ���� �� (��) ����
				case CX2Unit::EP_AC_BODY:
				case CX2Unit::EP_AC_LEG:
				case CX2Unit::EP_AC_ARM:
				case CX2Unit::EP_AC_NECKLESS:
				case CX2Unit::EP_DEFENCE_BODY:
					pViewEquip->SetShowObject( false );
					bOnePieceAvatarCheck = true;
					break;

				default:
					break;
				}
			}
		}
		if ( IsOnePieceAvatarRenderingHairAndFace1( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
					{
						pViewEquip->SetShowObject( false );
					}
					break;

				default:
					break;
				}
			}
		}

		// 2012.12.02 darkstarbt_������ // �ѹ� �ƹ�Ÿ �ε� ���Ƕ� ���ǰ� ��� �־ ���� �԰� �ִ� �ƹ�Ÿ ���� ���;� �ϴ� ���
		if ( IsOnePieceAvatarWithRenderBodyItem( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						bOnePieceAvatarCheck = true;
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_AC_FACE1:
				case CX2Unit::EP_AC_FACE2:
				case CX2Unit::EP_AC_NECKLESS:
					{
						bOnePieceAvatarCheck = true;
						pViewEquip->SetShowObject( false );
					}
					break;

				default:
					break;
				}
			}
		}
		if ( IsOnePieceAvatarOnlyRenderFace1Face2Face3Equip( pNoRenderFactorEquipBody->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_DEFENCE_LEG:
				case CX2Unit::EP_DEFENCE_HAND:
				case CX2Unit::EP_DEFENCE_FOOT:
				case CX2Unit::EP_AC_BODY:
				case CX2Unit::EP_AC_LEG:
				case CX2Unit::EP_AC_ARM:
				case CX2Unit::EP_AC_NECKLESS:
					pViewEquip->SetShowObject( false );
					bOnePieceAvatarCheck = true;
					break;

				default:
					break;
				}
			}
		}

	}

	CX2EqipPtr pNoRenderFactorEquipACArm = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_ARM );
	if( pNoRenderFactorEquipACArm != NULL )
	{
		if ( IsNoRenderHandEquip( pNoRenderFactorEquipACArm->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAND:
					{
						pViewEquip->SetShowObject( false );
					}
					break;

				default:
					break;
				}
			}
		}
	}

	//CX2Item* pItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( CX2Unit::EP_AC_FACE1, false );
	// EP_AC_FACE1 ������ġ�� �ִ� �������� id�� ����
	CX2EqipPtr pNoRenderFactorEquip = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_FACE1 );

	if ( NULL != pNoRenderFactorEquip )
	{
		// kimhc // �� �̻� �Ⱥ����� �� �з��� �޶� ���ٸ�
		// �Ⱥ������� ���� �� �׷� ����� ������ �ִ� �������� ��ũ��Ʈ�� �����ҵ�
		// ����� �� �Ⱥ��̰� �ϴ� �����۰� ���, �� �Ǽ�(��,��)�� �Ⱥ��̰� �ϴ� �������� ���� ��ġ �̹Ƿ�
		// �Ѱ��� ���� �� �� ����
		if ( IsNoRenderHairFace2Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_AC_FACE2:
				case CX2Unit::EP_AC_FACE3:
					pViewEquip->SetShowObject( false );
					break;

				default:
					break;
				}
			}
		}
		else if ( IsNoRenderHairFace2Equip( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
			{
				switch ( pViewEquip->GetEqipPos() )
				{
				case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;
				case CX2Unit::EP_AC_FACE2:
					pViewEquip->SetShowObject( false );
					break;

				default:
					break;
				}
			}
		}
		else if ( IsNoRenderHair( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			if(bOnePieceAvatarCheck == true)
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE2:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}
			else
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					{
						pViewEquip->SetShowObject( false );
						if( NULL == m_pHairXSkinMesh )
						{
							m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
						}
					} break;

					default:
						break;
					}
				}
			}
		}
		else if ( IsNoRenderHairFaceFace2Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			if(bOnePieceAvatarCheck == true)
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE1:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}
			else
			{
				SetHideNoRenderable(true);
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
						{
							pViewEquip->SetShowObject( false );
							if( NULL == m_pHairXSkinMesh )
							{
								m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
							}
						} break;
					case CX2Unit::EP_DEFENCE_FACE:
					case CX2Unit::EP_AC_FACE2:
					case CX2Unit::EP_AC_FACE3:
						pViewEquip->SetShowObject( false );
						break;
					default:
						break;
					}
				}
			}
		}
	}

	pNoRenderFactorEquip = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_FACE2 );

	if ( NULL != pNoRenderFactorEquip )
	{
		if ( IsNoRenderHairFaceFace1Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			if(bOnePieceAvatarCheck == true)
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE2:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}
			else
			{
				SetHideNoRenderable(true);
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
						{
							pViewEquip->SetShowObject( false );
							if( NULL == m_pHairXSkinMesh )
							{
								m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
							}
						} break;
					case CX2Unit::EP_DEFENCE_FACE:
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE3:
						pViewEquip->SetShowObject( false );
						break;
					default:
						break;
					}
				}
			}
		}
		else if ( IsNoRenderHair( pNoRenderFactorEquip->GetItemID() ) == true )
		{
			if(bOnePieceAvatarCheck == true)
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE2:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}
			else
			{
				BOOST_TEST_FOREACH( CX2EqipPtr, pViewEquip, m_ViewEqipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
						{
							pViewEquip->SetShowObject( false );
							if( NULL == m_pHairXSkinMesh )
							{
								m_pHairXSkinMesh = pViewEquip->GetNormalModel(0);
							}
						} break;

					default:
						break;
					}
				}
			}
		}
	}

	if( NULL != m_pAfterImage )
	{
		m_pAfterImage->SetpHairSkinMesh( m_pHairXSkinMesh );
	}
}

//{{ kimhc // 2010.12.10 // ��ĳ �۾��ϸ鼭 �߰���
/** @function : GetAddItemAndRemoveItemToViewList
	@brief : ��� �����ϰų� ��ü�� ���� Ż�� �� ���(EquipPtr)�� ������, �߰��ؾ� �� ���(pItemToAddInViewList)�� ������\n
	��� Ż���� �� ���� Ż�� �� ���(EquipPtr), �߰��Ǿ�� �� ���(pItemToAddInViewList)�� ������, Ż�� �� ���(pItemToRemoveInViewList) �� ������\n
	��� Ż���� �� �� �߰��Ǿ��� ���� �ƹ�Ÿ ���� Ż������ �� ������ �� �⺻��� or normal ������ ���Ѵ�.
	iIndexInViewListForRemove_�� ���������� ����� ViewList�������� index�� ����
	@param : OUT CX2EqipPtr& pEquipPtrToRemove_, OUT pItemToAddInViewList_, IN CX2Item* const pItemToRemoveInViewList_, OUT int& iIndexInViewListForRemove_
	@return : bool(�Լ��� ���� ����)
*/
bool CX2GUUser::GetAddItemAndRemoveItemToViewList( OUT CX2EqipPtr& pEquipPtrToRemove_, IN OUT CX2Item** ppItemToAddInViewList_, IN CX2Item* const pItemToRemoveInViewList_, OUT int& iIndexInViewListForRemove_ )
{
	if (*ppItemToAddInViewList_ != NULL )		// ������et ��� �ִ� ���
	{
		const CX2Item::ItemTemplet* pItemTempletForItemToAdd = (*ppItemToAddInViewList_)->GetItemTemplet();
		if ( pItemTempletForItemToAdd == NULL )
		{
			ASSERT( false );
			return false;
		}

		for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_ViewEqipList.size(); numViewEquipInUser++ )
		{
			if ( pItemTempletForItemToAdd->GetEqipPosition() == m_ViewEqipList[numViewEquipInUser]->GetEqipPos() )
			{
				// ������ ��� Normal�̰� �ش� ��ġ�� �̹� Fashion ���� �ִ� ��쿡�� ������ �������� View���� ���� ���� �߰��� ���� ����
				if (  pItemTempletForItemToAdd->GetFashion() == false && GetUnit()->IsFashionEquipInThePosition( pItemTempletForItemToAdd->GetEqipPosition() ) == true )
				{
					*ppItemToAddInViewList_ = NULL;
				}
				else
				{
					pEquipPtrToRemove_ = m_ViewEqipList[numViewEquipInUser];
					// �̺κп��� remove �ؾ��ҵ�
				}
				iIndexInViewListForRemove_ = numViewEquipInUser;
				break;
			}
		}
	}
	else if ( pItemToRemoveInViewList_ != NULL )		// ��� ������ ���� ���
	{
		const CX2Item::ItemTemplet* pItemTempletForItemToRemove = pItemToRemoveInViewList_->GetItemTemplet();
		if ( pItemTempletForItemToRemove == NULL)
		{
			ASSERT( 0 );
			return false;
		}
		for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_ViewEqipList.size(); numViewEquipInUser++ )
		{
			if ( pItemTempletForItemToRemove->GetEqipPosition() == m_ViewEqipList[numViewEquipInUser]->GetEqipPos() )
			{
				switch ( pItemTempletForItemToRemove->GetFashion() )
				{
				case true:	// ���� �� ��� �м� ���� ���
					{
						pEquipPtrToRemove_ = m_ViewEqipList[numViewEquipInUser];	// ������ ���

						// ������ ���� �븻 ��� �ִ� ���
						if ( GetUnit()->IsNormalEquipInThePosition( pItemTempletForItemToRemove->GetEqipPosition() ) == true )
						{
							CX2Item* pNormalEquip	= NULL;

							pNormalEquip	= GetUnit()->GetInventory().GetEquippingItemByEquipPos( pItemTempletForItemToRemove->GetEqipPosition(), false );

							if ( pNormalEquip == NULL )
								return false;

							if ( pNormalEquip->GetItemData().m_PeriodType != CX2Item::PT_ENDURANCE		// �������� ���� ���ų�
								|| pNormalEquip->GetItemData().m_Endurance > 0)		// �������� 0���ϰ� �ƴ϶�� �븻 ��� �־���
								*ppItemToAddInViewList_ = pNormalEquip;
							else
								*ppItemToAddInViewList_ = GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );

						}
						else	// ������ ���� �븻 ��� ���� ���
						{
							*ppItemToAddInViewList_ = GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );
						}
					}
					break;

				case false: // ���� �� �������� �븻 ���� ���
					{
						// ������ ���� �м� �۵� ���� ���
						if (  GetUnit()->IsFashionEquipInThePosition( pItemTempletForItemToRemove->GetEqipPosition() ) == false )
						{
							pEquipPtrToRemove_		= m_ViewEqipList[numViewEquipInUser];	// ������ ���
							*ppItemToAddInViewList_	= GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );
						}
					}
					break;
				}

				iIndexInViewListForRemove_ = numViewEquipInUser;
				break;	// for���� ����
			}
		} // for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_ViewEqipList.size(); numViewEquipInUser++ )
	}

	return true;

}
//}} kimhc // 2010.12.10 // ��ĳ �۾��ϸ鼭 �߰���

//}} kimhc // 2010.12.6 //  2010-12-23 New Character CHUNG

//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
/** @function	GetUsingSkillCount
	@retrun		m_iUsingSkillCount : ���� ��ų ��� Ƚ��
	@brief		����ڰ� ����� ��ų�� Ƚ���� �ѱ��. �ٸ� 2ȸ �̻� ����� ��ų�� ��츸 Ƚ���� ���Եȴ�.
*/
int CX2GUUser::GetUsingSkillCount( void )
{
	for ( int i = 0; i < 4; ++i )
	{
		if ( m_iSkillSlotACount[i] > 2 )
			m_iUsingSkillCount++;

		if ( m_iSkillSlotBCount[i] > 2 )
			m_iUsingSkillCount++;
	}

	return m_iUsingSkillCount;
}

/** @function	CountUsedSkill
	@param		iSlotNumber : ���� ��ȣ(A,S,D,C)
	@param		bSlotB_ : ���� B ���� Ȯ��
	@brief		A, B ���� �����Ͽ� ��ų ��� �� ��� Ƚ���� �����Ѵ�.
	*/
void CX2GUUser::CountUsedSkill( int iSlotNumber_ )
{
	bool bSlotB_ = (iSlotNumber_ > 3) ? true : false;
	iSlotNumber_ = (iSlotNumber_ > 3) ? iSlotNumber_-4 : iSlotNumber_;
	if ( true == bSlotB_ )
		m_iSkillSlotBCount[iSlotNumber_]++;
	else
		m_iSkillSlotACount[iSlotNumber_]++;
}
//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����



void CX2GUUser::EndHenirBuff()
{

	/// ���� ��ϸ� ������ �ɷ��ִٸ�, ����������
	EraseBuffTempletFromGameUnit( BTI_BUFF_HENIR_FIRE );
	EraseBuffTempletFromGameUnit( BTI_BUFF_HENIR_WATER );
	EraseBuffTempletFromGameUnit( BTI_BUFF_HENIR_NATURE );
	EraseBuffTempletFromGameUnit( BTI_BUFF_HENIR_WIND );
	EraseBuffTempletFromGameUnit( BTI_BUFF_HENIR_LIGHT );

	return;
}

void CX2GUUser::StartHenirBuffDark()
{
	if(GetNowHp() <= 0.0f)
		return;

	const float MAGIC_HENIR_DARK_HP_PERCENT_UP = 20.f;
	const float MAGIC_HENIR_DARK_MP_PERCENT_UP = 20.f;

	float fHenirDarkHpPercentUp = 0.f;
	float fHenirDarkMpPercentUp = 0.f;

	fHenirDarkHpPercentUp = MAGIC_HENIR_DARK_HP_PERCENT_UP / 100.f;
	fHenirDarkMpPercentUp = MAGIC_HENIR_DARK_MP_PERCENT_UP / 100.f;

	float fHPUpRate = GetMaxHp() * fHenirDarkHpPercentUp * GetHpUpRateBySkill();
	UpNowHp( fHPUpRate );

	g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_Heal", (CX2GameUnit*)this );
	PlaySound( L"Nasod_Healer_Heal.ogg" );

	float fMPUpRate = GetMaxMp() * fHenirDarkMpPercentUp * GetMpUpRateBySkill();;
	UpNowMp( fMPUpRate );

	if( IsLocalUnit() == true )
		g_pChatBox->AddChatLog( GET_STRING(STR_ID_13781), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );

	return;
}

void CX2GUUser::StartHenirBuffFirst()
{
	if(GetNowHp() <= 0.0f)
		return;

	const float MAGIC_HENIR_FIRST_HP_PERCENT_UP = 70.f;
	const float MAGIC_HENIR_FIRST_MP_PERCENT_UP = 70.f;

	float fHenirFirstHpPercentUp = 0.f;
	float fHenirFirstMpPercentUp = 0.f;

	if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
	{
		CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
		if( NULL != pDungeonGame &&
			NULL != pDungeonGame->GetDungeon() )
		{
			int iCurrentHenirStageIndex = pDungeonGame->GetDungeon()->GetNowStageIndex();
			fHenirFirstHpPercentUp = (MAGIC_HENIR_FIRST_HP_PERCENT_UP - (float)iCurrentHenirStageIndex) / 100.f;
			fHenirFirstMpPercentUp = (MAGIC_HENIR_FIRST_MP_PERCENT_UP - (float)iCurrentHenirStageIndex) / 100.f;
		}
	}

	float fHPUpRate = GetMaxHp() * fHenirFirstHpPercentUp * GetHpUpRateBySkill();
	UpNowHp( fHPUpRate );

	g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_Heal", (CX2GameUnit*)this );
	PlaySound( L"Nasod_Healer_Heal.ogg" );

	float fMPUpRate = GetMaxMp() * fHenirFirstMpPercentUp * GetMpUpRateBySkill();;
	UpNowMp( fMPUpRate );

	if( IsLocalUnit() == true )
		g_pChatBox->AddChatLog( GET_STRING(STR_ID_13775), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );

	return;
}

float CX2GUUser::GetHpUpRateBySkill()
{
	float fIncreaseRateFromMasterOfAlchemistSkill = 1.f;
	if ( GetUnit() != NULL )
	{
		BYTE byMemberShipGrade = GetUnit()->GetUnitData().m_byMemberShipGrade;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_MASTER_OF_ALCHEMIST, byMemberShipGrade);
		if ( pSkillTemplet != NULL )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

			fIncreaseRateFromMasterOfAlchemistSkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillTempletLevel );
		}
	}

	float fIncreaseRateMetabolism = 1.f;
	if ( CX2Unit::UC_CHUNG_FURY_GUARDIAN == GetUnitClass() ||
		CX2Unit::UC_CHUNG_IRON_PALADIN == GetUnitClass() )
	{
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

		if ( 0 < iSkillLv )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet =
				g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

			if ( NULL != pSkillTemplet )
			{
				fIncreaseRateMetabolism	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );
			}
		}
	}
	return fIncreaseRateFromMasterOfAlchemistSkill * fIncreaseRateMetabolism;
}

float CX2GUUser::GetMpUpRateBySkill()
{
	float fIncreaseRateFromMasterOfAlchemistSkill = 1.f;
	if ( GetUnit() != NULL )
	{
		BYTE byMemberShipGrade = GetUnit()->GetUnitData().m_byMemberShipGrade;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_MASTER_OF_ALCHEMIST, byMemberShipGrade);
		if ( pSkillTemplet != NULL )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

			fIncreaseRateFromMasterOfAlchemistSkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillTempletLevel );
		}
	}
	return fIncreaseRateFromMasterOfAlchemistSkill;
}

/*virtual*/ int CX2GUUser::GetWayOfSwordState() const
{
	return 0;
}
/*virtual*/ float CX2GUUser::GetWayOfSwordPoint() const
{
	return 0.f;
}
/*virtual*/ void CX2GUUser::SetWayOfSwordPoint( float fWayOfSwordPoint_ )
{
	return;
}


void CX2GUUser::StartSpecialItemBuffWind()
{
	m_fSpecialItemBuff_Wind = 30.f;

	const float MAGIC_ANIM_SPEED_FACTOR = 1.2f;
	const float MAGIC_MOVE_SPEED_FACTOR = 1.2f;
	const float MAGIC_JUMP_SPEED_FACTOR = 1.2f;

	SetAnimSpeedFactor( MAGIC_ANIM_SPEED_FACTOR,	m_fSpecialItemBuff_Wind, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);
	SetVecMoveSpeedFactor( MAGIC_MOVE_SPEED_FACTOR,	m_fSpecialItemBuff_Wind, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);
	SetVecJumpSpeedFactor( MAGIC_JUMP_SPEED_FACTOR,	m_fSpecialItemBuff_Wind, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);


	if( INVALID_EFFECTSET_HANDLE != m_hSpecialItemBuff_Wind )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hSpecialItemBuff_Wind );
		m_hSpecialItemBuff_Wind = INVALID_EFFECTSET_HANDLE;
	}

	if( INVALID_EFFECTSET_HANDLE == m_hSpecialItemBuff_Wind )
	{
		m_hSpecialItemBuff_Wind = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SPECIALITEM_BUFF_WIND", this );
		m_vecEffectSetToDeleteOnDie.push_back( m_hSpecialItemBuff_Wind );
	}
	if( IsLocalUnit() == true )// ���� �ɷȴٴ� ä�� ����. �̸��� �ٲ� ��� �Ѵ�.
		g_pChatBox->AddChatLog( GET_STRING(STR_ID_15018), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );

	return;
}

void CX2GUUser::EndSpecialItemBuff()
{
	if(m_fSpecialItemBuff_Wind > 0.f)
		m_fSpecialItemBuff_Wind = 0.001f;

	return;
}

void CX2GUUser::StopSpecialItemBuffWind()
{
	if(m_hSpecialItemBuff_Wind == INVALID_EFFECTSET_HANDLE)
		return;

	m_fSpecialItemBuff_Wind = 0.f;

	SetAnimSpeedFactor( 1.f,	0.f, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);
	SetVecMoveSpeedFactor( 1.f,	0.f, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);
	SetVecJumpSpeedFactor( 1.f,	0.f, CX2GameUnit::SFI_SPEED_11, CX2GameUnit::SFST_UPDATE);

	//if( INVALID_EFFECTSET_HANDLE != m_hSpecialItemBuff_Wind )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hSpecialItemBuff_Wind );
		m_hSpecialItemBuff_Wind = INVALID_EFFECTSET_HANDLE;
	}
	if( IsLocalUnit() == true ) // �̰� �״�� �ص� �ɵ�??
		g_pChatBox->AddChatLog( GET_STRING(STR_ID_13782), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );

}



//////////////////////////////////////////////////////////////////////////

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
void CX2GUUser::SyncData::SetData( const KDYNAMIC_UNIT_USER_SYNC& SyncPacket, DWORD dwRealFrameMoveCount, float fMaxHP, float fMaxMP )
{
	dwFrameMoveCount	= dwRealFrameMoveCount;

	nowState				= SyncPacket.m_ucNowState;
	nowSubState				= -1;
	nowAction				= SyncPacket.m_ucNowAction;
	bDirectChange			= SyncPacket.DecodeDirectChange();
	stateChangeNum			= SyncPacket.m_cStateChangeNum;

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
	_DecodeUserSyncPos( position, SyncPacket.m_usPosX, SyncPacket.m_usPosZ, SyncPacket.m_ucLastTouchLineIndex );
    position.y              = SyncPacket.m_fPosY;
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//	_DecodeUserSyncPos( position, SyncPacket.m_usPosX, SyncPacket.m_usPosY, SyncPacket.m_usPosZ, SyncPacket.m_ucLastTouchLineIndex );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

	lastTouchLineIndex		= SyncPacket.m_ucLastTouchLineIndex;

	bIsRight				= SyncPacket.DecodeIsRight();
	bFrameStop				= SyncPacket.DecodeFrameStop();

	fNowHP					=  SyncPacket.m_fNowHp;
	fNowMP					=  SyncPacket.m_fNowMp;

	m_HyperModeCount		= SyncPacket.DecodeHyperModeCount();

	m_CannonBallCount		= SyncPacket.DecodeCannonBallCount();

	m_usRandomTableIndex	= SyncPacket.m_usRandomTableIndex;

	ucHitCount				= SyncPacket.m_ucHitCount;
	ucHittedCount			= SyncPacket.m_ucHittedCount;
	ucNumOfDeBuff			= SyncPacket.m_ucNumOfDeBuff;
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
    m_dwRelativePos         = SyncPacket.m_dwRelativePos;
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
}

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
void CX2GUUser::SyncData::_DecodeUserSyncPos( D3DXVECTOR3& vPosition, USHORT usPosX, USHORT usPosZ, unsigned char ucLastTouchLineIndex )
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//void CX2GUUser::SyncData::_DecodeUserSyncPos( D3DXVECTOR3& vPosition, USHORT usPosX, USHORT usPosY, USHORT usPosZ, unsigned char ucLastTouchLineIndex )
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
{
    const CKTDGLineMap::LineData* pLineData = NULL;
	if( !g_pX2Game || !g_pX2Game->GetLineMap()
        || ( pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )ucLastTouchLineIndex ) ) == NULL )
    {
		StateLog( L"���� ������ �̻�." );
	    vPosition.x = halfToFloat( usPosX );
//#ifndef X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//        vPosition.y = halfToFloat( usPosY );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
	    vPosition.z = halfToFloat( usPosZ );
		return;
    }

	vPosition.x = pLineData->startPos.x + halfToFloat( usPosX );
//#ifndef X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//    vPosition.y = pLineData->startPos.y + halfToFloat( usPosY );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
	vPosition.z = pLineData->startPos.z + halfToFloat( usPosZ );
}

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#ifdef OPTIMIZED_P2P
//void CX2GUUser::SyncData::SetData( KXPT_UNIT_USER_SYNC& SyncPacket, USHORT usHP, USHORT usMP )
//{
//    nowState				= SyncPacket.m_ucNowState;
//	nowSubState				= -1;
//	nowAction				= SyncPacket.nowAction;
//    bDirectChange			= SyncPacket.DecodeDirectChange();
//    stateChangeNum			= SyncPacket.DecodeStateChangeNum();
//
//    const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )SyncPacket.m_ucLastTouchLineIndex );
//    if( pLineData )
//    {
//        float fValue = halfToFloat( SyncPacket.m_usPosX );
//        position = pLineData->startPos + fValue * pLineData->dirVector;
//        position.y = pLineData->startPos.y + halfToFloat( SyncPacket.m_usPosY );
//    }
//    else
//    {
//        StateLog( L"���� ������ �̻�." );
//    }
//
//    lastTouchLineIndex		= SyncPacket.m_ucLastTouchLineIndex;
//
//    bIsRight				= SyncPacket.DecodeIsRight();
//    bFrameStop				= SyncPacket.DecodeFrameStop();
//
//    fNowHP					= halfToFloat( usHP );
//    fNowMP					= halfToFloat( usMP );
//
//    m_HyperModeCount		= SyncPacket.DecodeHyperModeCount();
//
//#ifdef NEW_RANDOM_TABLE_TEST
//	m_usRandomTableIndex	= SyncPacket.m_usRandomTableIndex;
//#else NEW_RANDOM_TABLE_TEST
////{{AFX
//	m_RandSeed				= SyncPacket.m_RandSeed;
//	m_RandSeed2				= SyncPacket.m_RandSeed2;
////}}AFX
//#endif NEW_RANDOM_TABLE_TEST
//}
//#else
////{{AFX
//void CX2GUUser::SyncData::SetData( KXPT_UNIT_USER_SYNC* pSyncPacket )
//{
//	dwFrameMoveCount		= pSyncPacket->dwFrameMoveCount;
//    nowState				= pSyncPacket->nowState;
//	nowSubState				= -1;
//    nowAction				= pSyncPacket->nowAction;
//    bDirectChange			= pSyncPacket->DecodeDirectChange();
//    stateChangeNum			= pSyncPacket->stateChangeNum;
//    position.x				= pSyncPacket->posX;
//    position.y				= pSyncPacket->posY;
//    position.z				= pSyncPacket->posZ;
//
//    lastTouchLineIndex		= pSyncPacket->lastTouchLineIndex;
//
//    bIsRight				= pSyncPacket->DecodeIsRight();
//    bFrameStop				= pSyncPacket->DecodeFrameStop();
//
//    //{{ kimhc // 2010.12.7 // ����, ���� ��� ���� �Ǵ� ��Ƽ���� HP�� ���� �ʴ� ���� ����
//	fNowHP					= pSyncPacket->fNowHP;
//	fNowMP					= pSyncPacket->fNowMP;
//	//}} kimhc // 2010.12.7 // ����, ���� ��� ���� �Ǵ� ��Ƽ���� HP�� ���� �ʴ� ���� ����
//
//    m_HyperModeCount		= pSyncPacket->DecodeHyperModeCount();
//
////{{ kimhc // 2010.12.13 // 2010-12-23 New Character CHUNG
//#ifdef	NEW_CHARACTER_CHUNG
//	m_CannonBallCount		= pSyncPacket->DecodeCannonBallCount();
//#endif	NEW_CHARACTER_CHUNG
////}} kimhc // 2010.12.13 //  2010-12-23 New Character CHUNG
//
//#ifdef NEW_RANDOM_TABLE_TEST
//	m_usRandomTableIndex	= pSyncPacket->m_usRandomTableIndex;
//#else NEW_RANDOM_TABLE_TEST
////{{AFX
//    m_RandSeed				= pSyncPacket->m_RandSeed;
//    m_RandSeed2				= pSyncPacket->m_RandSeed2;
////}}AFX
//#endif NEW_RANDOM_TABLE_TEST
//
//	ucHitCount				= pSyncPacket->ucHitCount;
//	ucHittedCount			= pSyncPacket->ucHittedCount;
//	ucNumOfDeBuff			= pSyncPacket->ucNumOfDeBuff;
//}
////}}AFX
//#endif // OPTIMIZED_P2P
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

void CX2GUUser::SetSuperArmorSocket()
{
	// �ǰݽ� ����Ȯ���� ���۾Ƹ�
	float fRandomValueForSuperArmor = GetRandomFloat( CKTDXRandomNumbers::SRO_SUPER_ARMOR_WHEN_ATTACKED );

	if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
	{
		if( fRandomValueForSuperArmor <= m_SocketData.m_fPercentSuperArmorDungeon && IsSuperArmor() == false)
		{
			m_fSocketSuperArmor = m_SocketData.m_fTimeSuperArmorDungeon;
			m_bSocketSuperArmor = true;
		}
	}
	else if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
	{
		if(fRandomValueForSuperArmor <= m_SocketData.m_fPercentSuperArmorPVP && IsSuperArmor() == false)
		{
			m_fSocketSuperArmor = m_SocketData.m_fTimeSuperArmorPVP;
			m_bSocketSuperArmor = true;
		}
	}
	else
	{
		if(fRandomValueForSuperArmor <= m_SocketData.m_fPercentSuperArmorDungeon && IsSuperArmor() == false)
		{
			m_fSocketSuperArmor = m_SocketData.m_fTimeSuperArmorDungeon;
			m_bSocketSuperArmor = true;
		}
	}
}

void CX2GUUser::ProcessVPP()
{
	if( m_delegateVitalPointPiercing )
		m_delegateVitalPointPiercing();
}

/*virtual*/ void CX2GUUser::DeleteMinorParticle()
{
	CX2GameUnit::DeleteMinorParticle();

	DeleteGameUnitMinorParticle();
}

bool CX2GUUser::GetShowActiveSkillShow() const
{
	bool bShowActiveSkillShow = false;
	switch( g_pMain->GetGameOption().GetStateCutinShowState() )
	{
	case CX2GameOption::SCSS_ONESELF:
		{
			if( true == IsMyUnit() )
				bShowActiveSkillShow = true;
		}break;
	case CX2GameOption::SCSS_PARTY_MEMBER:
		{
			if( true == IsMyUnit() ||
				g_pData->GetPartyManager()->IsMyPartyMember(m_UnitUID) )
				bShowActiveSkillShow = true;
		} break;
	default:
		break;
	}

	return bShowActiveSkillShow;
}

bool CX2GUUser::GetShowCutInAndChangeWorldColor()
{
	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_BATTLE_FIELD:	// �Ÿ� 800 �̳� �� ���
		if ( IsMyUnit() || GetDistance3Sq( GetPos(), g_pX2Game->GetMyUnit()->GetPos() ) < LIMIT_DISTANCE_SQ )
			return true;
		else
			return false;
		break;

	case CX2Game::GT_DUNGEON:
	case CX2Game::GT_PVP:
		return true;
		break;

	default:
		ASSERT( !L"wrong path" );
		return false;
		break;
	}
}
/** @function : ChangeWorldColorByHyperMode
	@brief : ���� ���� ���ο� ���� ����Į�� �ٸ��� ����
*/
void CX2GUUser::ChangeWorldColorByHyperMode()
{
	if ( GetShowCutInAndChangeWorldColor() )
	{
		if ( 0.0f < GetRemainHyperModeTime() )
			g_pX2Game->GetWorld()->SetWorldColor( 0xffff0000 );
		else
			g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
	}
}

void CX2GUUser::SetEquippedSkillFunc( const CX2Unit::UnitData& kUnitData_, const bool bSlotB_ )
{
	for( int i = 0; i< EQUIPPED_SKILL_SLOT_COUNT; i++ )
	{
		const CX2UserSkillTree::SkillSlotData* pSkillSlotData = kUnitData_.m_UserSkillTree.GetSkillSlot( i, bSlotB_ );
		ASSERT( pSkillSlotData != NULL );
		if( NULL != pSkillSlotData )
			SetEquippedSkillFuncBySkillSlotIndex( pSkillSlotData->m_eID, i, bSlotB_ );
	}
}

/*virtual*/ bool CX2GUUser::ShouldResetNotBusyTimer()
{
	// ���� ���´� ��� �����̰�
	 if ( GetWaitStateID() == m_FrameDataFuture.syncData.nowState || GetChargeMpStateID() == m_FrameDataFuture.syncData.nowState
		 || USI_RIDING_WAIT == m_FrameDataFuture.syncData.nowState || USI_RIDING_WAIT_HABIT == m_FrameDataFuture.syncData.nowState )
	{
		return false;
	}
	else	// ���� ���°� �ٻ� ���¸�
		return true;	// ���� �ؾ���
}

void CX2GUUser::ResetNotBusyTimerOnSpecificState()
{
	if ( ShouldResetNotBusyTimer() )
	{
		//TRACE( L"Before StateID: %d, Now StateID: %d\n", m_FrameDataNowBefore.syncData.nowState, m_FrameDataNow.syncData.nowState );
		m_ElapsedTimeNotBusyState.ResetSumOfElapsedTime();
	}
}

void CX2GUUser::InitByGameType()
{
	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_BATTLE_FIELD:
		{
			m_delegatePhysicProcessPortalByGameType = DelegatePhysicProcessPortalByGameType::from_method<CX2GUUser, &CX2GUUser::PhysicProcess_PortalInBattleField>( this );
		} break;

	case CX2Game::GT_DUNGEON:
		{
			if( NULL != g_pMain && false == g_pMain->GetIsPlayingTutorial() )
			{
				m_delegatePhysicProcessPortalByGameType = DelegatePhysicProcessPortalByGameType::from_method<CX2GUUser, &CX2GUUser::PhysicProcess_Portal>( this );
			}
			else
			{
				m_delegatePhysicProcessPortalByGameType = DelegatePhysicProcessPortalByGameType::from_method<CX2GUUser, &CX2GUUser::PhysicProcess_PortalInBattleField>( this );
			}
		} break;

	case CX2Game::GT_PVP:
		{
			m_delegatePhysicProcessPortalByGameType = DelegatePhysicProcessPortalByGameType::from_method<CX2GUUser, &CX2GUUser::PhysicProcess_Portal>( this );
		} break;

	default:
		break;
	}
}

void CX2GUUser::UpdateBuffInfo( const KStat& kStat_, const vector<KBuffInfo>& vecSetBuffInfo_, const vector<int>& vecEraseBuffInfo_ )
{
	//CX2Unit* pUnit = GetUnit();		/// �� ���� �ý���
	//if ( NULL != pUnit )
	//{
	//	CX2Unit::UnitData* pUnitData = &GetUnit()->AccessUnitData();

	//	pUnitData->m_GameStat.SetKStat( kStat_ );
	//	if( kStat_.m_iBaseHP > 0.f )
	//		SetGameStat( kStat_ );

	//	UpdateBuffEffect( vecSetBuffInfo_, vecEraseBuffInfo_ );	//���� ����Ʈ ����

	//	pUnitData->ErasePremiumBuffInfo( vecEraseBuffInfo_ );	//���� ���� ���̵�
	//	pUnitData->SetPremiumBuffInfo( vecSetBuffInfo_ );		//�߰��� ���� ���̵�

	//	UpdateSocketDataAndEnchantData();
	//	UpdatePassiveAndActiveSkillState();

	//	//HP, MP ���� ������ ����
	//	ResetMaxHP();
	//	ResetMaxMP();
	//}
}

void CX2GUUser::UpdateBuffEffect( const vector<KBuffInfo>& vecSetBuffInfo_, const vector<int>& vecEraseBuffInfo_ )
{
	////������� �� ���� ����Ʈ ���� �� ����
	//BOOST_FOREACH( const int& iBuffID, vecEraseBuffInfo_ )
	//{
	//	switch( iBuffID )
	//	{
	//	case CX2PremiumBuffTemplet::BI_VITALITY_OF_EL:
	//		{
	//			if( m_hEffectCrystalOfElBuff != INVALID_EFFECTSET_HANDLE)
	//			{
	//				g_pX2Game->GetEffectSet()->StopEffectSet(m_hEffectCrystalOfElBuff);
	//			}
	//		}break;
	//	default:
	//		{
	//			return;
	//		}break;
	//	}
	//}

	////���� �Ǿ�� �� ���� ����Ʈ ���� �� �߰�
	//BOOST_FOREACH( const KBuffInfo& kBuffInfo, vecSetBuffInfo_ )
	//{
	//	switch( kBuffInfo.m_iBuffID )
	//	{
	//	case CX2PremiumBuffTemplet::BI_VITALITY_OF_EL:
	//		{
	//			if( m_hEffectCrystalOfElBuff != INVALID_EFFECTSET_HANDLE)
	//			{
	//				g_pX2Game->GetEffectSet()->StopEffectSet(m_hEffectCrystalOfElBuff);
	//			}

	//			m_hEffectCrystalOfElBuff = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Crystal_Of_El_Buff", (CX2GameUnit*) this );
	//		}break;
	//	default:
	//		{
	//			return;
	//		}break;
	//	}
	//}
}

//10�ʴ� HP ȸ�� ����
void CX2GUUser::SetHPChangeRatePerSecond( float fElapsedTime )
{
	float fSocketHPIncreasePerSecond = GetSocketData().m_fHPRelativeChangeRate;	// HP ���� ȸ�� ����

	//������ ����� ������ ��, 10�ʴ� ���� ��ġ ��ŭ ȸ�� �Ѵ�.
	if( fSocketHPIncreasePerSecond > 0.f )
	{
		float fValueLimit	= GetMaxHp() - GetNowHp();								//ȸ������ ��Ȯ�� ǥ�� �ǵ��� ȸ�� �Ѱ�ġ ����

		if( fValueLimit  <= 0.f )													//���� ü���� �����̸� ���� ����
		{
			m_fSocketHPIncreasePerSecondTime = 0.f;									//���� �̻��̸� �ð��� 0���� �ʱ�ȭ

			return;
		}

		m_fSocketHPIncreasePerSecondTime += fElapsedTime;							//���ǰ� �ƴҶ��� �ð� ����

		if( m_fSocketHPIncreasePerSecondTime > 10.f )								//�ð��� 10�� �̻��� ��
		{
			float fValue = GetMaxHp() * fSocketHPIncreasePerSecond;					//ȸ����

			m_fSocketHPIncreasePerSecondTime = 0.f;									//�ð��� 10�ʰ� �Ѿ��� ��, ������ �ð��� �ʱ�ȭ

			if ( fValueLimit < fValue )												//ä���� �� ��ġ��ŭ�� �����Ѵ�. - ( HP 100/ 90, ȸ���� +20 �϶�,		 ȭ��� HP +10 �� �������� )
			{
				fValue = fValueLimit;
			}

			UpNowHp( fValue );

			WCHAR wszIncreaseHP[128] = L"";
			StringCchPrintfW( wszIncreaseHP, ARRAY_SIZE(wszIncreaseHP), L"HP+ %d", static_cast< int >( fValue ) );
			D3DXVECTOR3 pos = GetPos();
			pos.y += 50.0f;
			if( NULL != g_pData->GetPicCharGreen() )
				g_pData->GetPicCharGreen()->DrawText( wszIncreaseHP, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}
	}
	else																			//������ ���� ���� ���� �� �ð� �ʱ�ȭ
	{
		m_fSocketHPIncreasePerSecondTime = 0.f;
	}
}

//MP ���� ȸ�� ����
void CX2GUUser::SetMPChangeRateValue( float fElapsedTime )
{
	int iSocketMPChangeRate = GetSocketData().m_iMPChangeValue;		// MP ���� ȸ���� ���� ����

	SetNowMp( GetNowMp() + ( static_cast<float>( iSocketMPChangeRate ) * fElapsedTime ) );
}

#ifdef PVP_ZOOM_CAMERA
void CX2GUUser::SetSubZoomCameraTimer()
{
	if( IsMyUnit() == true )
		g_pMain->GetGameOption().SetSubZoomCameraTimer(2.f);
}
#endif

void CX2GUUser::UpdateStartOfDelayedFiring( bool bInsertUnit )
{
	if( CX2Unit::UC_LIRE_NIGHT_WATCHER == GetUnitClass() )
	{
		CX2GULire_ElvenRanger* pRena = static_cast<CX2GULire_ElvenRanger*>(this);
		if( pRena != NULL )
		{
			pRena->UpdateStartOfDelayedFiring( bInsertUnit );
		}
	}
}

float CX2GUUser::GetEDTBleedingTime()
{
	if( CX2Unit::UC_RAVEN_VETERAN_COMMANDER == GetUnitClass() )
	{
		CX2GURaven* pRaven = static_cast<CX2GURaven*>(this);
		if( pRaven != NULL )
		{
			return pRaven->GetEDTBleedingTime();
		}
	}

	return 0.f;
}

/*virtual*/ void CX2GUUser::ChargeMpAndEtcInTrainingGame()
{
	SetNowMp( GetMaxMp() );

	if ( GetHyperModeCount() == GetFrameData().syncData.m_HyperModeCount )
	{
		int iHyperModeCount = GetHyperModeCount();
		if ( iHyperModeCount < 1 )
		{
			if ( static_cast<int>( CX2GageUI::PGUWO_ORB3 ) == ++iHyperModeCount )
				iHyperModeCount = static_cast<int>( CX2GageUI::PGUWO_ORB3 );

			SetHyperModeCount( iHyperModeCount );
		}
	}
}

/** @function : SendBuffInfoNot
	@brief : �ڽ��� ���� ������ ������ ���� �ٸ� �������� ������ �Լ�
	@param : ���۹��� ������ UID(uidGameUnitToSendNot_)
*/
void CX2GUUser::SendBuffInfoNot( const UidType uidGameUnitToSendNot_ )
{
	KEGS_UPDATE_USER_UNIT_BUFF_INFO_NOT kPacket;
	kPacket.m_iToUnitUID = uidGameUnitToSendNot_;
	kPacket.m_vecBuffFactor.reserve( m_vecBuffTempletPtr.size() );

	GetBuffFactor( kPacket.m_vecBuffFactor );

	g_pData->GetServerProtocol()->SendPacket( EGS_UPDATE_USER_UNIT_BUFF_INFO_NOT, kPacket );
}

/** @function : HyperModeBuffEffectStart
	@brief : ���� �� ���;��ϴ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUUser::HyperModeBuffEffectStart()
{
	BOOST_TEST_FOREACH( CX2EqipPtr, pEquipPtr, m_ViewEqipList )
	{
		pEquipPtr->ChangeHyperMode();
	}
}

/** @function : HyperModeBuffEffectEnd
	@brief : ���� ������ �� ����Ʈ ����
*/
/*virtual*/ void CX2GUUser::HyperModeBuffEffectEnd()
{
	CX2GameUnit::HyperModeBuffEffectEnd();
	BOOST_TEST_FOREACH( CX2EqipPtr, pEquipPtr, m_ViewEqipList )
	{
		pEquipPtr->ChangeNormalMode();
	}

	m_iHyperModeCountUsed = 0;
}

/** @function : CreateAndInsertRenderParamByBuffUnitType
	@brief : ������ Ÿ�Ժ��� ������ ���� RenderParam ������ �����ϴ� �Լ�
	@param : ������������(BuffIdentity_), �����Ķ����� map ��������(mapStBuffRenderParamPtr_)
*/
/*virtual*/ void CX2GUUser::CreateAndInsertRenderParamByBuffUnitType( const KBuffIdentity& BuffIdentity_, const map<CX2Unit::UNIT_TYPE, StBuffRenderParamPtr>& mapStBuffRenderParamPtr_ )
{
	if ( NULL != GetUnit() )
	{
		CX2Unit::UNIT_TYPE eUnitType = GetUnit()->GetType();

		/// �ڽ��� ���� Ÿ�Կ� �´� RenderParam ���� �ִ��� �˻�
		map<CX2Unit::UNIT_TYPE, StBuffRenderParamPtr>::const_iterator mItrByUnitType
			= mapStBuffRenderParamPtr_.find( eUnitType );

		if ( mapStBuffRenderParamPtr_.end() != mItrByUnitType && NULL != mItrByUnitType->second)
		{
			CreateAndInsertRenderParamByBuff( BuffIdentity_, *(mItrByUnitType->second) );
		}
		else	/// �ڽŸ��� RenderParam�� ���ٸ�
		{
			/// �������� ����ϴ� �����Ķ� ã��
			map<CX2Unit::UNIT_TYPE, StBuffRenderParamPtr>::const_iterator mItrCommonType
				= mapStBuffRenderParamPtr_.find( CX2Unit::UT_NONE );

			if ( mapStBuffRenderParamPtr_.end() != mItrCommonType && NULL != mItrCommonType->second)
				CreateAndInsertRenderParamByBuff( BuffIdentity_, *(mItrCommonType->second) );
		}
	}
}

/** @function : IsSuperArmor
	@brief : ���� ���۾Ƹ��ΰ�?
	@return : ���۾Ƹӻ���(true), �ƴ�(false)
*/
/*virtual*/ bool CX2GUUser::IsSuperArmor() const
{
	if ( !m_vecSuperArmorPtr.empty() )	/// ������ ���� ����� ���۾ƸӰ� ������
		return true;	/// ���۾Ƹ�
	else	/// ������ ���� ���۾ƸӰ� ������
		return m_FrameDataNow.stateParam.bSuperArmor;
}

/** @function : ResetLeftSkillCoolTimeBySkillType
	@brief : ���ڷ� ���޹��� ��ų Ÿ�Կ� �ش��ϴ� ��ų���� ��Ÿ���� �ʱ�ȭ ��Ų��.
	@param : ��Ÿ���� �ʱ�ȭ ��ų ��ųŸ��(eSkillType_)
*/

/*virtual*/ void CX2GUUser::ResetLeftSkillCoolTimeBySkillType( const CX2SkillTree::SKILL_TYPE eSkillType_, const float fCoolTime /*= 0.f*/, bool bForceSet /*= true*/, BUFF_CHANGE_TYPE eChangeType /*= BCT_RELATION_VALUE*/, const float fCoolTimeRate /*= 1.f */, const bool bIsRelativeAllSkill /*= false */ )

{
	if ( NULL != GetUnit() )
	{
		CX2Unit::UnitData* pUnitData = &GetUnit()->AccessUnitData();
		{
			pUnitData->m_UserSkillTree.ResetLeftSkillCoolTimeBySkillType( eSkillType_, fCoolTime, bForceSet, eChangeType, fCoolTimeRate, bIsRelativeAllSkill );
		}
	}
}

/** @function : DoGuildSkillChanceToReverse
	@brief : ��彺ų ������ ��ȸ ����
*/
void CX2GUUser::DoGuildSkillChanceToReverse()
{
	const CX2Unit::UnitData*	pUnitData = ( NULL != GetUnit() ) ? &GetUnit()->GetUnitData() : NULL;

	if  ( NULL != pUnitData )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet =
			pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_CHANCE_TO_REVERSE, pUnitData->m_byMemberShipGrade );
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}
}

/** @function : SetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
void CX2GUUser::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	switch ( eBuffTempletId_ )
	{
	case BTI_BUFF_HP_UP_SOUP:
		{
			GetGageData()->SetNowHp( GetMaxHp() );
		} break;
	case BTI_BUFF_MP_UP_SOUP:
		{
			ForceChangeHyperMode( 3 );
			SetForceHyperModeTime( 90.0 );
			GetGageData()->SetNowMp( GetMaxMp() );
		} break;
	case BTI_DEBUFF_SIDE_EFFECT_MEDICINE:
	case BTI_BUFF_BIG_HEAD_SOUP:
		{
			ScaleHead();
		} break;

	case BTI_BUFF_HENIR_FIRE:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13776), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;
	case BTI_BUFF_HENIR_WATER:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13777), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;
	case BTI_BUFF_HENIR_NATURE:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13778), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;
	case BTI_BUFF_HENIR_WIND:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13779), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;
	case BTI_BUFF_HENIR_LIGHT:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13780), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;


	case BTI_BUFF_ELIXIR_BIG_HAND_POTION:
		{
			ScaleHand( D3DXVECTOR3( 1.5f, 1.5f, 1.5f ) );
		} break;


	default:
		break;
	}
}

/** @function : UnSetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
void CX2GUUser::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	switch ( eBuffTempletId_ )
	{
	case BTI_DEBUFF_SIDE_EFFECT_MEDICINE:
	case BTI_BUFF_BIG_HEAD_SOUP:
		{
			RestoreHead();
			PlaySound( L"Pressed_Return.ogg" );
		} break;

	case BTI_BUFF_HENIR_FIRE:
	case BTI_BUFF_HENIR_WATER:
	case BTI_BUFF_HENIR_NATURE:
	case BTI_BUFF_HENIR_WIND:
	case BTI_BUFF_HENIR_LIGHT:
		{
			if( IsLocalUnit() == true )
				g_pChatBox->AddChatLog( GET_STRING(STR_ID_13782), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(0.92549f, 0.92549f, 0.53333f, 1.f), L"#CECEC88" );
		} break;


	case BTI_BUFF_ELIXIR_BIG_HAND_POTION:
		{
			RestoreHand();
		} break;



	case BTI_BUFF_WATER_SHIELD :		// �� ��ȣ���� ����Ǹ� ����Ʈ ���� �Ѹ�
		{
			if ( NULL != g_pX2Game &&
				 NULL != g_pX2Game->GetEffectSet() )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SHASHA_STAGE0_AttackA_Buff_Hit", this, this );
			}
		} break;

	case BTI_HEALTH_POINT_IMMEDIATELY_CHANGE_ONCE :		// �� ��ȣ���� ����Ǹ� ����Ʈ ���� �Ѹ�
		{
			if ( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetEffectSet() )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SHASHA_STAGE0_AttackB_Heal", this, this );
			}
		} break;



	default:
		break;
	}
}

void CX2GUUser::ToggleSkillSlotAB()
{
	if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->SwapSkillSlot(!g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap());
	}
}





//#ifndef X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//
///*FORCEINLINE*/ bool CX2GUUser::ProcessCanPushUserUnit( CX2GUUser* pGameUnit_, const D3DXVECTOR2& vMyUnitMinMaxY_, const D3DXVECTOR2& vMyUnitMinMaxYCur_, OUT D3DXVECTOR3& vMyPos_ )
//{
//	KTDXPROFILE_BEGIN("Push");
//
//	D3DXVECTOR3 vPosFutureOtherGameUnit	= pGameUnit_->GetPos( true );
//    D3DXVECTOR3 vPosNowOtherGameUnit = pGameUnit_->GetPositionBackup();
//
//	const float fGameUnitYMin = vPosFutureOtherGameUnit.y;
//	const float fGameUnitYMax = fGameUnitYMin + pGameUnit_->GetUnitHeight( true );
//
//	// ���̰� ��ĥ�� �ִ� �������� ���� �׽�Ʈ
//	// vMyUnitMinMaxY_���� x�� Min ��, y�� Max ��
//	if( (fGameUnitYMin >= vMyUnitMinMaxY_.x && fGameUnitYMin <= vMyUnitMinMaxY_.y) ||
//		(fGameUnitYMax >= vMyUnitMinMaxY_.x && fGameUnitYMax <= vMyUnitMinMaxY_.y) ||
//		(fGameUnitYMin <= vMyUnitMinMaxY_.x && fGameUnitYMax >= vMyUnitMinMaxY_.y) )
//	{
//		vPosFutureOtherGameUnit.y = vMyPos_.y;
//		const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
//		float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;
//
//		if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// ���� �����Ͽ� �پ�������
//		{
//            float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
//            float fMarginMax = FLT_MAX;
//
//			//{{ dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��
//			// Ư�� ĳ���� Ȥ�� ������ ũ�Ⱑ Ŭ����, �ӵ��� �������� ���� �ʹ� �ָ� �и��� �Ǽ� ��Ÿ������ �ȵǴ� ������ ����
//			// �̸� �������ֱ� ���� ����ó��
//			if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
//				pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
//			{
//				const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
//				fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
//				if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
//					fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
//			}
//			//}} dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��
//
//			//��� ������ ���� ���� ����
//            D3DXVECTOR3 vTestPos;
//            D3DXVECTOR3 vDirVectorFromMeToOther;
//            const float fGameUnitYMinCur = vPosNowOtherGameUnit.y;
//            const float fGameUnitYMaxCur = fGameUnitYMinCur + pGameUnit_->GetUnitHeight( false );
//	        if( (fGameUnitYMinCur >= vMyUnitMinMaxYCur_.x && fGameUnitYMinCur <= vMyUnitMinMaxYCur_.y) ||
//		        (fGameUnitYMaxCur >= vMyUnitMinMaxYCur_.x && fGameUnitYMaxCur <= vMyUnitMinMaxYCur_.y) ||
//		        (fGameUnitYMinCur <= vMyUnitMinMaxYCur_.x && fGameUnitYMaxCur >= vMyUnitMinMaxYCur_.y) )
//            {
//                vDirVectorFromMeToOther = vPosNowOtherGameUnit - m_FrameDataFuture.syncData.position;
//                vDirVectorFromMeToOther.y = 0.f;
//			    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
//			    {
//                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
//                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
//                    {
//                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
//                        vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                        vTestPos.y = vMyPos_.y;
//                        vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    }
//                    else
//                    {
//                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
//                        vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                        vTestPos.y = vMyPos_.y;
//                        vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    }
//                }
//                else
//                {
//                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
//                    vDirVectorFromMeToOther.y = 0.f;
//                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
//                    {
//                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
//                        vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                        vTestPos.y = vMyPos_.y;
//                        vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    }
//                    else
//                    {
//                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
//                        vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                        vTestPos.y = vMyPos_.y;
//                        vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    }
//                }
//                vMyPos_ = vTestPos;
//                return true;
//            }
//            else
//            {
//                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
//                vDirVectorFromMeToOther.y = 0.f;
//                float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
//			    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
//			    {
//                    vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                    vTestPos.y = vMyPos_.y;
//                    vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    if ( CheckPassUnit( pGameUnit_, vTestPos) == false )
//                    {
//                        vMyPos_ = vTestPos;
//                        return true;
//                    }
//                }
//                else
//                {
//                    vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
//                    vTestPos.y = vMyPos_.y;
//                    vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
//                    if ( CheckPassUnit( pGameUnit_, vTestPos) == false )
//                    {
//                        vMyPos_ = vTestPos;
//                        return true;
//                    }
//                }
//                return false;
//            }
//		} // if
//	} // if
//
//    if ( CheckPassUnit( pGameUnit_, vMyPos_) == false )
//        return true;
//
//	KTDXPROFILE_END();
//
//    return  false;
//}
//
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
/*FORCEINLINE*/ bool CX2GUUser::ProcessCanPushUnit( CX2GameUnit* pGameUnit_, const D3DXVECTOR2& vMyUnitMinMaxY_, OUT D3DXVECTOR3& vMyPos_, int iDirection )
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
///*FORCEINLINE*/ bool CX2GUUser::ProcessCanPushNPCUnit( CX2GUNPC* pGameUnit_, const D3DXVECTOR2& vMyUnitMinMaxY_, OUT D3DXVECTOR3& vMyPos_, int iDirection )
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
{
	KTDXPROFILE_BEGIN("Push");

	D3DXVECTOR3 vPosFutureOtherGameUnit = pGameUnit_->GetPos( true );
	const float fGameUnitYMin = vPosFutureOtherGameUnit.y;
	const float fGameUnitYMax = fGameUnitYMin + pGameUnit_->GetUnitHeight( true );

	if( (fGameUnitYMin >= vMyUnitMinMaxY_.x && fGameUnitYMin <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMax >= vMyUnitMinMaxY_.x && fGameUnitYMax <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMin <= vMyUnitMinMaxY_.x && fGameUnitYMax >= vMyUnitMinMaxY_.y) )
    {
        if ( iDirection == 1 || iDirection == 2 )
        {
	        // ���̰� ��ĥ�� �ִ� �������� ���� �׽�Ʈ
	        // vMyUnitMinMaxY_���� x�� Min ��, y�� Max ��
		    vPosFutureOtherGameUnit.y = vMyPos_.y;
		    const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
     	    float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;

		    if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// ���� �����Ͽ� �پ�������
		    {
                float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
                float fMarginMax = FLT_MAX;

			    //{{ dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��
			    // Ư�� ĳ���� Ȥ�� ������ ũ�Ⱑ Ŭ����, �ӵ��� �������� ���� �ʹ� �ָ� �и��� �Ǽ� ��Ÿ������ �ȵǴ� ������ ����
			    // �̸� �������ֱ� ���� ����ó��
			    if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
				    pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
			    {
				    const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
				    fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
				    if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
					    fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
			    }
			    //}} dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��

                D3DXVECTOR3 vTestPos;
                D3DXVECTOR3 vDirVectorFromMeToOther;
			    if ( iDirection == 2 )
			    {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                else
                {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                vMyPos_ = vTestPos;
                return true;
		    }
            else
            {
                D3DXVECTOR3 vDirVectorFromMeToOther;
                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                vDirVectorFromMeToOther.y = 0.f;

                if ( iDirection == 2 )
                {
                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                        return true;
                }
                else
                {
                    if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
                        return true;
                }
                if ( CheckPassUnit( pGameUnit_, vMyPos_ ) == false )
                    return true;
                return false;
            }
        }
        else
        {
		    vPosFutureOtherGameUnit.y = vMyPos_.y;
		    const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
     	    float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;

		    if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// ���� �����Ͽ� �پ�������
		    {
                float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
                float fMarginMax = FLT_MAX;

			    //{{ dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��
			    // Ư�� ĳ���� Ȥ�� ������ ũ�Ⱑ Ŭ����, �ӵ��� �������� ���� �ʹ� �ָ� �и��� �Ǽ� ��Ÿ������ �ȵǴ� ������ ����
			    // �̸� �������ֱ� ���� ����ó��
			    if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
				    pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
			    {
				    const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
				    fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
				    if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
					    fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
			    }
			    //}} dmlee 2008.11.6 ���̺� �Ŀ����Ʈó�� �ӵ��� ������ �а� ������ ������ ��쿡 ����ó��

                D3DXVECTOR3 vTestPos;
                D3DXVECTOR3 vDirVectorFromMeToOther;
                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                vDirVectorFromMeToOther.y = 0.f;
                float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                {
                    vTestPos.x = vMyPos_.x - m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z - m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( iDirection == 0 || CheckPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return true;
                    }
                }
                else
                {
                    vTestPos.x = vMyPos_.x + m_FrameDataFuture.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z + m_FrameDataFuture.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( iDirection == 0 || CheckPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return true;
                    }
                }
                return false;
		    }
            else
            {
                if ( iDirection == 0 || CheckPassUnit( pGameUnit_, vMyPos_ ) == false )
                    return true;
            }//if.. else..
        }
    }
    else
    {
        if ( iDirection == 0 )
            return true;

        if ( fGameUnitYMin >= vMyUnitMinMaxY_.y )
        {
            if ( iDirection == 3 )
                return true;
        }
        else
        {
            if ( iDirection == 4 )
                return true;
        }
        if ( CheckPassUnit( pGameUnit_, vMyPos_ ) == false )
            return true;
    }


	KTDXPROFILE_END();

    return false;
}



/*FORCEINLINE*/ bool CX2GUUser::CheckPassUnit( const CX2GameUnit* pGameUnit_, OUT D3DXVECTOR3& vPos_ )
{
	KTDXPROFILE_BEGIN("Pass");

	const D3DXVECTOR3& vMyOriginalPos = m_FrameDataFuture.syncData.position;

	if( vPos_ == vMyOriginalPos )
    {
        return false;
    }

    const D3DXVECTOR3& vPosFutureOtherGameUnit = pGameUnit_->GetPos( true );
    D3DXVECTOR3 vPosBackup = const_cast<CX2GameUnit*>(pGameUnit_)->GetPositionBackup();

	D3DXVECTOR3 vDirFromPosFutureToUnit		= vPosFutureOtherGameUnit - vPos_;
	vDirFromPosFutureToUnit.y = 0.f;

    D3DXVECTOR3 vDirFromOrgPosToUnit	= vPosBackup - vMyOriginalPos;
	vDirFromOrgPosToUnit.y = 0.f;

    if ( D3DXVec3Dot( &vDirFromPosFutureToUnit, &vDirFromOrgPosToUnit ) > 0 )
    {
        return false;
    }

    vDirFromOrgPosToUnit	= vPosFutureOtherGameUnit - vMyOriginalPos;
	vDirFromOrgPosToUnit.y = 0.f;

	if ( D3DXVec3Dot( &vDirFromPosFutureToUnit, &vDirFromOrgPosToUnit ) < 0 )
	{
		const float fProjObjDistance3Sq
			= GetDistance3Sq( D3DXVECTOR3( vPosFutureOtherGameUnit.x, vMyOriginalPos.y, vPosFutureOtherGameUnit.z ), vMyOriginalPos );

		const float fProjMoveDistance3Sq
			= GetDistance3Sq( D3DXVECTOR3( vPos_.x, vMyOriginalPos.y, vPos_.z ), vMyOriginalPos );

		if ( fProjMoveDistance3Sq >= fProjObjDistance3Sq )
		{
			D3DXVECTOR3 moveDir = vPos_ - vMyOriginalPos;
			D3DXVec3Normalize( &moveDir, &moveDir );

			const float fCollisionPointFirstY = vMyOriginalPos.y + moveDir.y * sqrt( fProjObjDistance3Sq );
			const float fCollisionPointSecondY = fCollisionPointFirstY + GetUnitHeight( true );

			const float fGameUnitYMin = vPosFutureOtherGameUnit.y;
			const float fGameUnitYMax = vPosFutureOtherGameUnit.y + pGameUnit_->GetUnitHeight( true );

			if( ( fCollisionPointFirstY >= fGameUnitYMin && fCollisionPointFirstY <= fGameUnitYMax ) ||
				( fCollisionPointSecondY >= fGameUnitYMin && fCollisionPointSecondY <= fGameUnitYMax ) )
                //|| ( fCollisionPointFirstY <= fGameUnitYMin && fCollisionPointSecondY >= fGameUnitYMax ) )
			{
                return true;
			}
		}
	}

	KTDXPROFILE_END();
    return false;
}

/** @function : SetVisitCashShop
	@brief : ĳ������ �� ������ ���� ó��
			(����, �̸�Ƽ�� �߰�)
*/
void CX2GUUser::SetEnterCashShop(bool bVal)
{
	m_bEnterCashShop = bVal;

	if( true == bVal )
	{//ĳ���� ����
		wstring wstrEmotionName = L"Emotion_SitWait";
		if ( true == GetRidingOn() && NULL != GetRidingPet() && false == m_wstrRidingMotionName.empty() )
		{
			wstrEmotionName = m_wstrRidingMotionName;
			GetRidingPet()->StateChange( GetRidingPet()->m_SitWaitState );
		}
		PlayEmotion(CX2Unit::ET_SITWAIT, wstrEmotionName);

		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hEnterCashShop )
		{
			m_hEnterCashShop =
				g_pX2Game->GetMajorParticle()->CreateSequenceHandle( static_cast<CKTDGObject*>(this),  L"EnterCashShop", 0,0,0 );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hEnterCashShop );
		if( NULL != pSeq )
		{
			pSeq->SetShowObject(true);

			D3DXVECTOR3 vPos = GetPos();
			vPos.y += 200.f;
			vPos.x += 50.f;
			pSeq->SetPosition( vPos );
		}
	}
	else
	{//ĳ���� ����
		StateChange(GetWaitStateID(), true);

		if(m_pXSkinAnim != NULL)
		{
			if ( true == GetRidingOn() && NULL != GetRidingPet() && false == m_wstrRidingMotionName.empty() )
			{
				m_pXSkinAnim->ChangeAnim( m_wstrRidingMotionName.c_str() , true );
				GetRidingPet()->StateChange( GetRidingPet()->m_SitWaitState );
			}
			else
			{
				m_pXSkinAnim->ChangeAnim( L"Emotion_StandUp" , true );
			}
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		}

		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hEnterCashShop )
		{
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle(m_hEnterCashShop);
		}
	}
}

/** @function : CanApplySocketDataByGameType
	@brief : ���� Ÿ�Կ� ���� ���Ͽɼ��� ���� �� �� �ִ��� ����
	@param : SocketData�� ������
	@return : ������ �Ǿ� �ϸ� true, �ȵǾ� �ϸ� false
*/
bool CX2GUUser::CanApplySocketDataByGameType( const CX2SocketItem::SocketData& kSocketData_ )
{
	if ( NULL != g_pX2Game )
	{
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_BATTLE_FIELD:
		case CX2Game::GT_DUNGEON:
			{
				if ( kSocketData_.m_bPVPOnly )
					return false;
			} break;

		default:	/// pvp
			{
				if ( kSocketData_.m_bDungeonOnly )
					return false;
			}break;
		}
	}

	return true;
}
void CX2GUUser::SetDamageExceptionProcess( CX2DamageManager::DamageData* pDamageData_, float& fPowerRate)
{
	CX2GameUnit::SetDamageExceptionProcess(pDamageData_, fPowerRate);

	if( null != pDamageData_->optrAttackerGameUnit != NULL && pDamageData_->optrAttackerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser *pUser = static_cast<CX2GUUser*>( pDamageData_->optrAttackerGameUnit.GetObservable() );
		CX2DamageManager::DAMAGE_TYPE eDamageType;
		float fDamageRel;

		pUser->SetHitNasodWeapon(false);

		bool bChange = pUser->GetChangeDamageType(eDamageType, fDamageRel);
		if( bChange == true
			&& eDamageType == CX2DamageManager::DT_MAGIC
			)
		{
			float fDamageRelOri = 1.f;

			switch(pDamageData_->damageType)
			{
			case CX2DamageManager::DT_PHYSIC:
				fDamageRelOri = pDamageData_->damage.fPhysic;
				break;
			case CX2DamageManager::DT_MAGIC:
				fDamageRelOri = pDamageData_->damage.fMagic;
				break;
			default:
				break;
			}

			pDamageData_->damageType = eDamageType;
			switch(eDamageType)
			{
			case CX2DamageManager::DT_PHYSIC:
				pDamageData_->damage.fPhysic = fDamageRelOri * fDamageRel;
				break;
			case CX2DamageManager::DT_MAGIC:
				pDamageData_->damage.fMagic = fDamageRelOri * fDamageRel;
				break;

				//{{ kimhc // 2010.7.30 // ������ ������ ���⸸�� ������
			case CX2DamageManager::DT_WEAPON_PHYSIC:
				pDamageData_->damage.fPhysic = fDamageRelOri * fDamageRel;
				break;
			case CX2DamageManager::DT_WEAPON_MAGIC:
				pDamageData_->damage.fMagic = fDamageRelOri * fDamageRel;
				break;
				//}} kimhc // 2010.7.30 // ������ ������ ���⸸�� ������
			default:
				break;
			}
		}
	}
}


#if X2_DEV_TOOLS
void CX2GUUser::SetScriptFileName()
{
	if( NULL == GetUnit() )
		return;

	switch ( GetUnit()->GetType() )
	{
	case CX2Unit::UT_ELSWORD:	m_ScriptFileName = L"ELSWORD_SWORDMAN.lua"; break;
	case CX2Unit::UT_ARME:		m_ScriptFileName = L"ARME_VIOLETMAGE.lua"; break;
	case CX2Unit::UT_LIRE:		m_ScriptFileName = L"LIRE_ELVENRANGER.lua"; break;
	case CX2Unit::UT_RAVEN:		m_ScriptFileName = L"RAVEN_FIGHTER.lua"; break;
	case CX2Unit::UT_EVE:		m_ScriptFileName = L"EVE_NASOD.lua"; break;
	case CX2Unit::UT_CHUNG:		m_ScriptFileName = L"CHUNG_IRON_CANNON.lua"; break;
	case CX2Unit::UT_ARA:		m_ScriptFileName = L"ARA_MARTIAL_ARTIST.lua";	break;
	case CX2Unit::UT_ELESIS:	m_ScriptFileName = L"ELESIS_KNIGHT.lua";	break;
	case CX2Unit::UT_ADD:		m_ScriptFileName = L"ADD_NASODRULER.lua";	break;
	default:
		break;
	}
}

#endif //REALTIME_SCRIPT_PATCH

const CX2GUUser::UserUnitStateData& CX2GUUser::GetPressedSkillState( const int iPressedIndex_ )
{
	if( GetRemainHyperModeTime() <= 0.f )
	{
		if( iPressedIndex_ > 3 ) //���� B�϶�
		{
			return m_StateList[_CONST_GUUSER_::aNormalSkillStateIDSlotB[iPressedIndex_-4]];
		}
		else
		{
			return m_StateList[_CONST_GUUSER_::aNormalSkillStateID[iPressedIndex_]];
		}
	}
	else
	{
		if( iPressedIndex_ > 3 ) //���� B�϶�
		{
			return m_StateList[_CONST_GUUSER_::aHyperSkillStateIDSlotB[iPressedIndex_-4]];
		}
		else
		{
			return m_StateList[_CONST_GUUSER_::aHyperSkillStateID[iPressedIndex_]];
		}
	}
}


D3DXVECTOR3	CX2GUUser::GetBonePos( const WCHAR* pBoneName ) const
{

	return m_pXSkinAnim->GetCloneFramePosition( pBoneName );
}

bool CX2GUUser::GetBonePos( const WCHAR* pBoneName, OUT D3DXVECTOR3& pos_ ) const
{

	return m_pXSkinAnim->GetCloneFramePosition( pBoneName, pos_ );
};

// ��ȯ�� ���� ĳ���� ��ġ�� �����´�
void CX2GUUser::CallSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/ )
{
	if( NULL == g_pX2Game )
		return;

	for( int i=0; i < g_pX2Game->GetUnitNum(); ++i )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( NULL == pUnit )
			continue;

		if( CX2GameUnit::GUT_NPC != pUnit->GetGameUnitType() )
			continue;

		CX2GUNPC *pNPC = static_cast<CX2GUNPC*>(pUnit);
		if( NULL == pNPC )
			continue;

		if( CX2UnitManager::NUI_NONE != eNPCID &&
			eNPCID != pNPC->GetNPCTemplet().m_nNPCUnitID )
		{
			continue;
		}

		if( pUnit->GetNowHp() <= 0.f )
			continue;

		if( NULL != pNPC->GetNPCAI() &&
			CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )
		{
			CX2AllyNPCAI* pAllyAI = static_cast<CX2AllyNPCAI*>(pNPC->GetNPCAI());
			if( GetUnitUID() == pAllyAI->GetAllyUnitUID() )
			{
				pNPC->SetPosition( GetPos(), GetIsRight() );
			}
		}
	}
}

void CX2GUUser::COMMON_RELATIONSHIP_SKILL_FrameMove()
{
	// ������ ����� ��� �ϱ� ������ m_fThrowStartTime ���� Ÿ�̹� ����.
    if( m_pXSkinAnim->EventTimerOneshot( m_fThrowStartTime ) == true )
	{
		D3DXVECTOR3 vEffectPos	= GetPos();
		D3DXVECTOR3 vRotate		= GetRotateDegree();

		vEffectPos.y += m_vThrowStartPosOffset.y;

		vEffectPos += (GetIsRight() == true) ? m_vThrowStartPosOffset.x * GetDirVector() : -m_vThrowStartPosOffset.x * GetDirVector();


		wstring wstrDamageEffectName = L"";


		const CX2Unit::UnitData*	pUnitData = ( NULL != GetUnit() ) ? &GetUnit()->GetUnitData() : NULL;
		if( NULL != pUnitData )
		{
			const CX2UserSkillTree::SkillSlotData* pSkillSlot =
				pUnitData->m_UserSkillTree.GetSkillSlot(CX2SkillTree::SI_ETC_WS_COMMON_LOVE );;

			if( NULL != pSkillSlot )
			{
				switch( pSkillSlot->m_iSkillLevel )
				{
					case 1: wstrDamageEffectName = L"RELATIONSHIP_SKILL_THROW_LV1"; break;
					case 2: wstrDamageEffectName = L"RELATIONSHIP_SKILL_THROW_LV2"; break;
					default:
						ASSERT(!"RelationShip Skill Level Error");
						break;
				}
			}
		}
		if( false == wstrDamageEffectName.empty() )
		{
			CX2DamageEffect::CEffect *pDamageEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate(), vEffectPos, vRotate, vRotate, m_FrameDataNow.unitCondition.landPosition.y );
		}
	}
	CommonFrameMove();
}
void CX2GUUser::COMMON_RELATIONSHIP_SKILL_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//#pragma region RindingStart
void CX2GUUser::RidingStartStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_StartState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingStartStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
	{
		m_PhysicParam.fWalkSpeed = m_ptrRidingPet->GetPhysicParam().fWalkSpeed;
		m_PhysicParam.fRunSpeed= m_ptrRidingPet->GetPhysicParam().fRunSpeed;
		m_PhysicParam.fJumpSpeed = m_ptrRidingPet->GetPhysicParam().fJumpSpeed;
		m_PhysicParam.fDashJumpSpeed = m_ptrRidingPet->GetPhysicParam().fDashJumpSpeed;

		InitAddtionalDashAndJumpBasicStatValue();

		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_StartState );
	}
}

void CX2GUUser::RidingStartEventProcess()
{
	CommonEventProcess();

	const float EVENT_TIME = 4.5f;

	if( EVENT_TIME <= m_FrameDataFuture.unitCondition.fStateTime )
	{
		StateChange( USI_RIDING_WAIT );
	}
}
//#pragma endregion

//#pragma region RidingOn
void CX2GUUser::RidingOnStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_StartState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingOnStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_StartState );

	if ( NULL != CX2RidingPetManager::GetInstance() )
	{
// 		if ( NULL == m_ptrRidingPet )
// 			m_ptrRidingPet = CX2RidingPetPtr();
//
// 		if ( NULL == m_ptrRidingPet )
// 			m_ptrRidingPet = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( GetUnitUID() );

		if ( NULL != m_ptrRidingPet )
		{
			m_FrameDataFuture.unitCondition.fUnitWidth = m_ptrRidingPet->GetUnitWidth();
			m_FrameDataFuture.unitCondition.fUnitHeight = m_ptrRidingPet->GetUnitHeight();

			m_PhysicParam.fWalkSpeed = m_ptrRidingPet->GetPhysicParam().fWalkSpeed;
			m_PhysicParam.fRunSpeed= m_ptrRidingPet->GetPhysicParam().fRunSpeed;
			m_PhysicParam.fJumpSpeed = m_ptrRidingPet->GetPhysicParam().fJumpSpeed;
			m_PhysicParam.fDashJumpSpeed = m_ptrRidingPet->GetPhysicParam().fDashJumpSpeed;

			InitAddtionalDashAndJumpBasicStatValue();

			m_ElapsedTimeRidingWaitHabit.ResetSumOfElapsedTime();
			m_ElapsedTimeRidingWaitHabit.SetTargetTime( static_cast<float>( GetRandomInt() % 6 + 3 ) );

		}
	}
}

void CX2GUUser::RidingOnEventProcess()
{
	CommonEventProcess();

	if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	else if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS + 60.f;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
}
void CX2GUUser::RidingOnEnd()
{
	CommonStateEnd();
}
//#pragma endregion

//#pragma region RidingOff
void CX2GUUser::RidingOffEventProcess()
{
	CommonEventProcess();

	StateChange( USI_WAIT );
}
void CX2GUUser::RidingOffEnd()
{
	CommonStateEnd();

	m_FrameDataFuture.unitCondition.fUnitWidth = m_vOrgUnitSize.x;
	m_FrameDataFuture.unitCondition.fUnitHeight = m_vOrgUnitSize.y;

	m_PhysicParam = m_OrgPhysicParam;
	m_bPassDash = false;

	InitAddtionalDashAndJumpBasicStatValue();
	SetNoDetonation( false );
}
//#pragma endregion

//#pragma region RidingWait
void CX2GUUser::RidingWaitStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WaitState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingWaitStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WaitState );
}

void CX2GUUser::RidingWaitEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( m_ElapsedTimeRidingWaitHabit.CheckElapsedTime() &&
		NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT_HABIT );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE(USI_RIDING_HYPER_MODE);


	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingWalk
void CX2GUUser::RidingWalkStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_WalkState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingWalkStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WalkState );

	TRACE( L"RidingWalkStart Start\n" );
}

void CX2GUUser::RidingWalkEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
		TRACE( L"Walk RidingPetSpecialAttackEventProcess" );
	}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		TRACE( L"Walk m_InputData.pureDoubleRight m_InputData.pureDoubleLeft" );
		StateChangeDashIfPossible();
	}
	else if ( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		TRACE( L"Walk m_InputData.pureRight m_InputData.pureLeft" );
		StateChange( USI_RIDING_WAIT );

		TRACE( L"RidingWalkStart Wait\n" );
	}
	else if ( true == m_InputData.pureUp )
	{
		TRACE( L"Walk m_InputData.pureUp" );
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE(USI_RIDING_HYPER_MODE);

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingJumpUp
void CX2GUUser::RidingJumpUpStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_JumpUpState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingJumpUpStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_JumpUpState );
}

void CX2GUUser::RidingJumpUpEventProcess()
{
	if ( true == RidingPetSpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( 0.0f >= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
	}
	else if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		if ( NULL != m_ptrRidingPet && m_iAirDashCount < 1 )
		{
			if ( CX2RidingPet::RSM_AIR_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove ||
				 CX2RidingPet::RSM_PASS_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove )
					StateChange( USI_RIDING_SPECIAL_MOVE );
		}
	}

	if ( true == m_InputData.oneUp )
	{
		if ( NULL != m_ptrRidingPet && CX2RidingPet::RSM_DOUBLE_JUMP == m_ptrRidingPet->GetInfo().m_eSpecialMove && m_iDoubleJumpCount < 1 )
			StateChange( USI_RIDING_SPECIAL_MOVE );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingJumpDown
void CX2GUUser::RidingJumpDownStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_JumpDownState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingJumpDownStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_JumpDownState );
}

void CX2GUUser::RidingJumpDownEventProcess()
{
	if( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if ( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			if( m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else if( m_InputData.pureRight == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_RIDING_JUMP_LANDING );
			}
		}
		else
		StateChange( USI_RIDING_JUMP_LANDING );
		m_bDownForce = false;
	}
	else if ( true == RidingPetSpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_RIDING_JUMP_UP );
	}
	else if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		if ( NULL != m_ptrRidingPet && m_iAirDashCount < 1 )
		{
			if ( CX2RidingPet::RSM_AIR_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove ||
				CX2RidingPet::RSM_PASS_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove )
				StateChange( USI_RIDING_SPECIAL_MOVE );
		}
	}
	if ( true == m_InputData.oneUp )
	{
		if ( NULL != m_ptrRidingPet && CX2RidingPet::RSM_DOUBLE_JUMP == m_ptrRidingPet->GetInfo().m_eSpecialMove && m_iDoubleJumpCount < 1 )
			StateChange( USI_RIDING_SPECIAL_MOVE );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingJumpLanding
void CX2GUUser::RidingJumpLandingStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_JumpLandingState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingJumpLandingStart()
{
	CommonStateStart();
	CreateStepDust();

	if ( NULL != m_ptrRidingPet )
	{
		m_iAirDashCount = 0;
		m_iDoubleJumpCount = 0;
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_JumpLandingState );
	}
}

void CX2GUUser::RidingJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE(USI_RIDING_HYPER_MODE);

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingDash
void CX2GUUser::RidingDashStart()
{
	CommonStateStart();
	CreateStepDust();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DashState );
}

void CX2GUUser::RidingDashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if ( NULL != m_ptrRidingPet && m_iAirDashCount < 1 )
		{
			if ( CX2RidingPet::RSM_AIR_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove ||
				CX2RidingPet::RSM_PASS_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove )
				StateChange( USI_RIDING_SPECIAL_MOVE );
		}
		else
		{
			StateChange( USI_RIDING_DASH_JUMP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_RIDING_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	else if( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_RIDING_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	if ( NULL != m_ptrRidingPet && CX2RidingPet::RSM_BOOST_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove &&
		1.0f <= m_FrameDataFuture.unitCondition.fStateTime )
	{
		StateChange( USI_RIDING_SPECIAL_MOVE );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE(USI_RIDING_HYPER_MODE);

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingDashEnd
void CX2GUUser::RidingDashEndStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_DashEndState );

	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = GetDashSpeed();
	m_bApplyMotionOffset = false;
}

void CX2GUUser::RidingDashEndStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DashEndState );
}



void CX2GUUser::RidingDashEndEventProcess()
{
	const float TRIGGER_TIME_FACTOR_DASH_ATTACK = 0.3f;

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_RIDING_JUMP_UP );
	}
	else if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE(USI_RIDING_HYPER_MODE);

	CommonEventProcess();
}

void CX2GUUser::RidingDashEndFrameMove()
{
	if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && true == m_ptrRidingPet->GetXSkinAnimFuturePtr()->EventTimer( 0.4f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}
//#pragma endregion

//#pragma region RidingDashJump
void CX2GUUser::RidingDashJumpStartFuture()
{

	CommonStateStartFuture();

	const float MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL = 1.2f;

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL;
	m_bDownForce					= false;
}

void CX2GUUser::RidingDashJumpStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
		CreateStepDust();

	/// Now ���� Future �� �̵�
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DashJumpUpState );
}



void CX2GUUser::RidingDashJumpEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

	if ( false == IsOnSomethingFuture() )
	{
		if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
		{
			if ( NULL != m_ptrRidingPet && m_iAirDashCount < 1 )
			{
				if ( CX2RidingPet::RSM_AIR_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove ||
					 CX2RidingPet::RSM_PASS_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove )
						StateChange( USI_RIDING_SPECIAL_MOVE );
			}
		}
	}

	if( true == IsOnSomethingFuture() )
	{
		if ( true == m_bDownForce )
		{
			StateChange( USI_RIDING_DASH_JUMP_LANDING );
			m_bDownForce = false;
		}
		else
		{
			if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
			{
				StateChangeDashIfPossible();
			}
			else
				StateChange( USI_RIDING_DASH_JUMP_LANDING );

			m_bDownForce = false;
		}
	}
	else if ( true == RidingPetSpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( USI_RIDING_DASH_JUMP_LANDING );
	}
	else if( true == m_InputData.pureRight )
	{
		if( false == m_FrameDataFuture.syncData.bIsRight )
		{
			if( GetJumpSpeed() < m_PhysicParam.nowSpeed.y )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( 0.0f < m_PhysicParam.nowSpeed.y )
				StateChange( USI_RIDING_JUMP_UP );
			else
				StateChange( USI_RIDING_JUMP_DOWN );
		}
	}
	else if( true == m_InputData.pureLeft )
	{
		if( true == m_FrameDataFuture.syncData.bIsRight )
		{
			if( GetJumpSpeed() < m_PhysicParam.nowSpeed.y )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( 0.0f < m_PhysicParam.nowSpeed.y )
				StateChange( USI_RIDING_JUMP_UP );
			else
				StateChange( USI_RIDING_JUMP_DOWN );
		}
	}

	if ( true == m_InputData.oneUp )
	{
		if ( NULL != m_ptrRidingPet && CX2RidingPet::RSM_DOUBLE_JUMP == m_ptrRidingPet->GetInfo().m_eSpecialMove && m_iDoubleJumpCount < 1 )
			StateChange( USI_RIDING_SPECIAL_MOVE );
	}

	if( true == m_InputData.oneUp  && 0.0f >= m_PhysicParam.nowSpeed.y  )
	{
		bCanDashJump = true;
	}
	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
		bCanDashJump = false;
	}

	CommonEventProcess();
}

void CX2GUUser::RidingDashJumpFrameMoveFuture()
{
	const float MAGIC_NUMBER_MULTIPLY_DASH_SPEED = 1.6f;

	if( true == m_InputData.pureRight && true == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}
	else if( true == m_InputData.pureLeft && false == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::RidingDashJumpEndFuture()
{
	CommonStateEndFuture();
	m_PhysicParam.fDownAccel	= 0.0f;
}
//#pragma endregion

//#pragma region RidingDashJumpLanding
void CX2GUUser::RidingDashJumpLandingStart()
{
	CommonStateStart();
	CreateStepDust();

	if ( NULL != m_ptrRidingPet )
	{
		m_iAirDashCount = 0;
		m_iDoubleJumpCount = 0;
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DashJumpLandingState );
	}
}

void CX2GUUser::RidingDashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_RIDING_JUMP_UP );
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingDamageFront
void CX2GUUser::RidingDamageFrontStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_DamageFrontState );

	CommonStateStartFuture();

	// ���������� �ǰ� �޾��� �� ��ũ��Ʈ ������ �Ľ����� ���ϴ� ��찡 �߻��ؼ� ó�� ��.
	m_FrameDataFuture.stateParam.bLandConnect = false;
}

void CX2GUUser::RidingDamageFrontStart()
{
	CommonStateStart();
	CreateStepDust();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DamageFrontState );
}

void CX2GUUser::RidingDamageFrontEventProcess()
{
	if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_RIDING_JUMP_DOWN );
		}
		else if ( false == GetRidingOn() )
		{
			StateChange( USI_RIDING_OFF );
		}
		else
			StateChange( USI_RIDING_WAIT );
	}
	else if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingDamageBack
void CX2GUUser::RidingDamageBackStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_DamageBackState );

	CommonStateStartFuture();
	// ���������� �ǰ� �޾��� �� ��ũ��Ʈ ������ �Ľ����� ���ϴ� ��찡 �߻��ؼ� ó�� ��.
	m_FrameDataFuture.stateParam.bLandConnect = false;
}

void CX2GUUser::RidingDamageBackStart()
{
	CommonStateStart();
	CreateStepDust();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_DamageBackState );
}

void CX2GUUser::RidingDamageBackEventProcess()
{

	if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_RIDING_JUMP_DOWN );
		}
		else if ( false == GetRidingOn() )
		{
			StateChange( USI_RIDING_OFF );
		}
		else
			StateChange( USI_RIDING_WAIT );
	}
	else if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingAttackZ
void CX2GUUser::RidingAttackZStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_AttackZState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingAttackZStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
	{
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_AttackZState );

		switch ( m_ptrRidingPet->GetInfo().m_Id )
		{
		case CX2RidingPetManager::RPUI_SCORPION_TYPE_R:
			{
				if ( HaveThisBuff( BTI_RIDING_SCORPION_ENERGY_SWORD ) )
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Riding_Nasod_Guardian_Special_Attack_Z", this );
				else
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Riding_Nasod_Guardian_Attack_Z", this );
			} break;

		default:
			break;
		}
	}
}

void CX2GUUser::RidingAttackZEventProcess()
{
	if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	RIDING_SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	RIDING_DASH_CANCEL_AFTER( m_fDashCancelAfter )
	RIDING_WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingJumpAttackZ
void CX2GUUser::RidingJumpAttackZStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_JumpAttackZState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingJumpAttackZStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_JumpAttackZState );
}

void CX2GUUser::RidingJumpAttackZFrameMoveFuture()
{
	if( true == m_InputData.pureDown && false == m_InputData.pureUp )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::RidingJumpAttackZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		bool &bPureDown = m_FrameDataFuture.stateParam.bEventFlagList[0];

		if ( true == bPureDown )
		{
			m_fCanNotJumpTime = 0.2f;
		}

		StateChange( USI_RIDING_JUMP_LANDING );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingAttackX
void CX2GUUser::RidingAttackXStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_AttackXState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingAttackXStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_AttackXState );
}

void CX2GUUser::RidingAttackXEventProcess()
{
	if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}

// 	if ( false == IsOnSomethingFuture() )
// 	{
// 		StateChange( USI_RIDING_JUMP_DOWN );
// 		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
// 	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	RIDING_SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	RIDING_DASH_CANCEL_AFTER( m_fDashCancelAfter )
	RIDING_WALK_CANCEL_AFTER( m_fWalkCancelAfter )


	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingSpecialAttack
void CX2GUUser::RidingSpecialAttackStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_SpecialAttackState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingSpecialAttackStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_SpecialAttackState );

	TRACE( L"RidingSpecialAttack Start\n" );
}

void CX2GUUser::RidingSpecialAttackFrameMove()
{
	if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
	{
		switch ( m_ptrRidingPet->GetInfo().m_Id )
		{
		case CX2RidingPetManager::RPUI_SCORPION_TYPE_R:
			{
                if( m_ptrRidingPet->GetXSkinAnimPtr()->EventTimerOneshot( 1.000f ) == true  )
					SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_RIDING_SCORPION_ENERGY_SWORD ) );
			} break;

		default:
			break;
		}
	}

	CommonFrameMove();
}

void CX2GUUser::RidingSpecialAttackEventProcess()
{
	if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
		TRACE( L"RidingSpecialAttack Wait\n" );
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	RIDING_SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	RIDING_DASH_CANCEL_AFTER( m_fDashCancelAfter )
	RIDING_WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingWaitHabit
void CX2GUUser::RidingWaitHabitStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_WaitHabitState );

	CommonStateStartFuture();
}

void CX2GUUser::RidingWaitHabitStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
	{
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WaitHabitState );
		m_ElapsedTimeRidingWaitHabit.ResetSumOfElapsedTime();
		m_ElapsedTimeRidingWaitHabit.SetTargetTime( static_cast<float>( GetRandomInt() % 6 + 3 ) );
	}
}

void CX2GUUser::RidingWaitHabitEventProcess()
{
	if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	else if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS + 60.f;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region RidingSpecialMove
void CX2GUUser::RidingSpecialMoveStartFuture()
{
	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_SpecialMoveState );

	CommonStateStartFuture();

	if ( NULL != m_ptrRidingPet )
	{
		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_AIR_DASH:
			{
				m_bDownForce = false;
				m_bDisableGravity = true;
				m_fAirDashTime = 0.f;
				++m_iAirDashCount;
			} break;
		case CX2RidingPet::RSM_PASS_DASH:
			{
				m_bDownForce = false;
				m_bDisableGravity = true;
				m_fAirDashTime = 0.f;
				++m_iAirDashCount;
				m_FrameDataFuture.stateParam.bCanPassUnit = true;
			} break;
		case CX2RidingPet::RSM_BOOST_DASH:
			m_iRidingBoost = 0;
			break;
		case CX2RidingPet::RSM_DOUBLE_JUMP:
			++m_iDoubleJumpCount;
			break;
		default:
			break;
		}
	}
}

void CX2GUUser::RidingSpecialMoveStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
	{
		// ��ƼŬ ó��
		CX2EffectSet::Handle hEffectSetInst = INVALID_EFFECTSET_HANDLE;

		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_AIR_DASH:
			break;

		case CX2RidingPet::RSM_PASS_DASH:
			{
				hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Riding_N_G_DashJump", this );
				m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );

				SetAlphaObject( true );

				if ( false == IsMyUnit() && false == IsNullGageUI() )
					m_pGageUI->SetAlpha( 0.0f );

				m_RenderParam.renderType	= CKTDGXRenderer::RT_CARTOON;
				m_RenderParam.bAlphaBlend	= true;
				m_RenderParam.color.a		= 0.5f;
				m_bPassDash = true;
//#ifdef  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX
                m_FrameDataNow.stateParam.bCanPassUnit = true;
//#endif  X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX
			} break;

		case CX2RidingPet::RSM_BOOST_DASH:
			{
				hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Riding_Mobi_Booster", this );
				m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );
			} break;

		case CX2RidingPet::RSM_DOUBLE_JUMP:
			{
				m_PhysicParam.nowSpeed.y = GetJumpSpeed() * 0.9f;
			} break;

		default:
			break;
		}

		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_SpecialMoveState );
	}
}

void CX2GUUser::RidingSpecialMoveFrameMoveFuture()
{
	if ( NULL != m_ptrRidingPet )
	{
		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_AIR_DASH:
		case CX2RidingPet::RSM_PASS_DASH:
			{
				m_fAirDashTime += m_fElapsedTime;

				if( false == m_InputData.pureDown && true == m_InputData.pureRight && true == m_FrameDataNow.syncData.bIsRight )
				{
					m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;
					m_PhysicParam.nowSpeed.y = max( 200.f - ( m_fAirDashTime * 500), -1200.f );
				}
				else if( false == m_InputData.pureDown && true == m_InputData.pureLeft && false == m_FrameDataNow.syncData.bIsRight )
				{
					m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;
					m_PhysicParam.nowSpeed.y = max( 200.f - ( m_fAirDashTime * 500), -1200.f );
				}
				else
				{
					StateChange( USI_RIDING_DASH_JUMP );
				}
			}
			break;
		case CX2RidingPet::RSM_BOOST_DASH:
			{
				if( ( true == m_InputData.pureRight && true == m_BeforeInputData.pureRight ) ||
					( true == m_InputData.pureLeft  && true == m_BeforeInputData.pureLeft  ) )
				{
					switch ( m_iRidingBoost )
					{
					case 1:
						m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * 0.1f;
						break;
					case 2:
						m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * 0.2f;
						break;
					case 3:
						m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * 0.3f;
						break;
					default:
						m_PhysicParam.nowSpeed.x = GetDashSpeed();
						break;
					}
				}
			} break;
		case CX2RidingPet::RSM_DOUBLE_JUMP:
			break;
		default:
			break;
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUUser::RidingSpecialMoveFrameMove()
{
	CommonFrameMove();
}

void CX2GUUser::RidingSpecialMoveEventProcess()
{
	if ( NULL != m_ptrRidingPet )
	{
		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_AIR_DASH:
		case CX2RidingPet::RSM_PASS_DASH:
			{
				if( true == IsOnSomethingFuture() )
				{
					if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
					{
						StateChange( USI_RIDING_DASH_JUMP );
						m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
						m_bDownForce = false;
					}
					else
					{
						StateChange( USI_RIDING_DASH );
						m_bDownForce = false;
					}
				}
				else if ( false == GetRidingOn() )
				{
					StateChange( USI_RIDING_OFF );
				}
				else if ( true == RidingPetSpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
				{}
				if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
				{
					m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
				}
				if( m_InputData.oneDown == true )
				{
					m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
					m_bDownForce = true;
					m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
				}
			}
			break;
		case CX2RidingPet::RSM_BOOST_DASH:
			{
				if ( false == IsOnSomethingFuture() )
				{
					StateChange( USI_RIDING_DASH_JUMP );
					m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
				}
				else if ( false == GetRidingOn() )
				{
					StateChange( USI_RIDING_OFF );
				}
				else if ( true == RidingPetSpecialAttackEventProcess() )
				{}
				else if ( true == m_InputData.oneUp )
				{
					StateChange( USI_RIDING_DASH_JUMP );
					m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
				}
				else if( m_InputData.pureRight == false && m_InputData.pureLeft == false )
				{
					StateChange( USI_RIDING_DASH_END );
				}
				else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
				{
					StateChange( USI_RIDING_DASH_END );
				}
				else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
				{
					StateChange( USI_RIDING_WALK );
				}
				else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
				{
					StateChange( USI_RIDING_WALK );
				}
				else if( true == m_InputData.oneDown &&
					true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
					&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
				{
					StateChange( USI_RIDING_JUMP_DOWN );
					m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
				}

				if ( 1.f <= m_FrameDataFuture.unitCondition.fStateTime && 0 == m_iRidingBoost )
				{
					++m_iRidingBoost;
					CX2EffectSet::Handle hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Riding_Mobi_Booster", this );
					m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );
				}
				else if ( 2.f <= m_FrameDataFuture.unitCondition.fStateTime && 1 == m_iRidingBoost )
				{
					++m_iRidingBoost;
					CX2EffectSet::Handle hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Riding_Mobi_Booster", this );
					m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );
				}
				else if ( 3.f <= m_FrameDataFuture.unitCondition.fStateTime && 2 == m_iRidingBoost )
				{
					++m_iRidingBoost;
				}
			}
			break;
		case CX2RidingPet::RSM_DOUBLE_JUMP:
			{
				if ( true == RidingPetSpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
				{}
				else if ( true == IsOnSomethingFuture() )
				{
					StateChange( USI_RIDING_JUMP_LANDING );
				}
				else
				{
					if( m_InputData.pureRight == true )
					{
						m_FrameDataFuture.syncData.bIsRight = true;
						m_PhysicParam.nowSpeed.x = GetWalkSpeed();
					}
					else if( m_InputData.pureLeft == true )
					{
						m_FrameDataFuture.syncData.bIsRight = false;
						m_PhysicParam.nowSpeed.x = GetWalkSpeed();
					}
				}

				if( true == m_InputData.oneDown )
				{
					m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
					m_bDownForce = true;
				}
			} break;
		default:
			break;
		}
	}

	CommonEventProcess();
}

void CX2GUUser::RidingSpecialMoveEndFutrue()
{
	CommonStateEndFuture();

	if ( NULL != m_ptrRidingPet )
	{
		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_AIR_DASH:
		case CX2RidingPet::RSM_PASS_DASH:
			m_bDisableGravity = false;
			m_fAirDashTime = 0.f;
			m_iAirDashCount = 0;
			break;
		case CX2RidingPet::RSM_DOUBLE_JUMP:
			break;
		default:
			break;
		}
	}
}

void CX2GUUser::RidingSpecialMoveEnd()
{
	CommonStateEnd();

	if ( NULL != m_ptrRidingPet )
	{
		switch ( m_ptrRidingPet->GetInfo().m_eSpecialMove )
		{
		case CX2RidingPet::RSM_BOOST_DASH:
			m_PhysicParam.nowSpeed.x = GetDashSpeed();
			break;

		case CX2RidingPet::RSM_DOUBLE_JUMP:
			break;

		case CX2RidingPet::RSM_PASS_DASH:
			{
				SetAlphaObject( false );
				if ( false == IsMyUnit() && false == IsNullGageUI() )
					m_pGageUI->SetAlpha( 255.0f );

				m_RenderParam.bAlphaBlend	= false;
				m_RenderParam.color.a		= 1.f;
				m_bPassDash = false;
			} break;

		default:
			break;
		}
	}
}
//#pragma endregion

void CX2GUUser::ProcessRidingOn( IN bool bVal_, IN const KRidingPetInfo& _Info )
{
	if ( NULL != CX2RidingPetManager::GetInstance() )
	{
		if ( true == bVal_ )
		{
			m_ptrRidingPet
				= CX2RidingPetManager::GetInstance()->CreateGameRidingPet( GetUnitUID(), _Info );

			// ž�� ����
			//// ������ ������ ������ ���ؼ� ���̵����� ���ø��� ������
			CX2RidingPetManager::RidingPetTemplet* pMyRidingPetTemplet
				= CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( _Info.m_usRindingPetID ) );

			/// ���̵� �� ���� ����
			if ( NULL != pMyRidingPetTemplet )
			{
				BOOST_FOREACH( const CX2RidingPetManager::RidingPetBuffInfo& buffInfo, pMyRidingPetTemplet->m_vecBuff )
				{
					/// ������ �����Ǿ� ������ ����
					if ( buffInfo.m_eBuffFactorID != BFI_NONE )
						SetBuffFactorToGameUnitByBuffFactorID( buffInfo.m_eBuffFactorID );
				}
			}

			if( GetNowHp() > 0 )
			{
				if( false == IsOnSomethingFuture() )
					StateChange( USI_JUMP_DOWN, true );
				else
					StateChange( USI_WAIT, true );
			}
		}
		else
		{
			SetRidingOn( bVal_ );
			if( GetNowHp() > 0 )
			{
				StateChange( USI_RIDING_OFF, true );
			}

			CX2RidingPetManager::GetInstance()->RemoveRidingPet( GetUnitUID() );
		}

		/// �� ������ ��쿡�� ó��
		if ( IsMyUnit() && NULL != g_pData->GetUIManager() )
		{
			g_pData->GetUIManager()->SetShowSkillSlot( !bVal_ );
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot( bVal_ );
		}

		/// ���� Ÿ�� ���� �� ���� �ɼ� �� ���
		UpdateSocketDataAndEnchantData();
		UpdatePassiveAndActiveSkillState();
	}
}

bool CX2GUUser::RidingPetSpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_GROUND*/ )
{
	if ( NULL == CX2RidingPetManager::GetInstance() || NULL == m_ptrRidingPet )
		return false;

	CX2RidingPetManager* pRidingPetManager = CX2RidingPetManager::GetInstance();

	CX2RidingPetManager::RidingPetTemplet* pRidingPetTemplet = pRidingPetManager->GetRidingPetTemplet(
		static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( m_ptrRidingPet->GetInfo().m_Id ) );

	if ( NULL == pRidingPetTemplet )
		return false;

	if( true == IsThereEffectiveBlackHoleDamageEffect() )
		return false;

	if( true == m_bSpecialAttackEventProcessedAtThisFrame )
		return false;

	UINT iSkillNum = -1;
	float fMPConsume = 0.f;
	float fCoolTime = 0.f;

	if( true == m_InputData.oneZ )
	{
		iSkillNum = 0;
		fMPConsume = pRidingPetTemplet->m_vecAttackSkill.at(0).m_fConsumeMana;
	}
	else if( true == m_InputData.oneX && CX2SkillTree::ASUT_GROUND == eActiveSkillUseCondition )	/// X ������ ������ �������� �����Ѵ�.
	{
		iSkillNum = 1;
		fMPConsume = pRidingPetTemplet->m_vecAttackSkill.at(1).m_fConsumeMana;
	}
	else if( true == m_InputData.oneC && CX2SkillTree::ASUT_GROUND == eActiveSkillUseCondition )	/// C ������ ������ �������� �����Ѵ�.
	{
		iSkillNum = 2;
		fMPConsume = pRidingPetTemplet->m_vecAttackSkill.at(2).m_fConsumeMana;
	}
	else
	{
		return false;
	}

	fCoolTime = pRidingPetManager->GetSkillSlot()->GetSkillCoolTime( iSkillNum );

#if X2_DEV_TOOLS
	if( false == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
#endif //_IN_HOUSE_
	{
		if( 0.f < fCoolTime )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}

		if ( GetNowMp() < fMPConsume )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}
	}

	/// ��ųâ���� ��ų �������̸�, ��ų ��� �Ұ�
	if (	NULL != g_pData &&
			NULL != g_pData->GetUIManager() &&
			NULL != g_pData->GetUIManager()->GetUISkillTree() &&
			true == g_pData->GetUIManager()->GetUISkillTree()->GetNowLearnSkill() )
	{
		g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_25110 ), D3DXCOLOR(1,1,1,1),
			D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
		return false;
	}

	UpNowMp( -fMPConsume );

	switch ( iSkillNum )
	{
	case 0:
		pRidingPetManager->GetSkillSlot()->SetSkillSlotCoolTime( iSkillNum, pRidingPetTemplet->m_vecAttackSkill.at(0).m_fCoolTime );
		pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, true );

		/// �Ϲ� Z ������ ��� ��Ȳ���� �ߵ� �Ǿ��°��� ����, �޶�����.
		if ( CX2SkillTree::ASUT_AIR == eActiveSkillUseCondition )
			StateChange( USI_RIDING_JUMP_ATTACK_Z );	/// ���� Z ����
		else
			StateChange( USI_RIDING_ATTACK_Z );			/// �Ϲ� Z ����

		if ( pRidingPetTemplet->m_vecAttackSkill.at(0).m_fCoolTime <= 0.f )
			pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, false );
		break;

	case 1:
		pRidingPetManager->GetSkillSlot()->SetSkillSlotCoolTime( iSkillNum, pRidingPetTemplet->m_vecAttackSkill.at(1).m_fCoolTime );
		pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, true );
		StateChange( USI_RIDING_ATTACK_X );
		if ( pRidingPetTemplet->m_vecAttackSkill.at(1).m_fCoolTime <= 0.f )
			pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, false );
		break;

	case 2:
		pRidingPetManager->GetSkillSlot()->SetSkillSlotCoolTime( iSkillNum, pRidingPetTemplet->m_vecAttackSkill.at(2).m_fCoolTime );
		pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, true );
		StateChange( USI_RIDING_SPECIAL_ATTACK );
		if ( pRidingPetTemplet->m_vecAttackSkill.at(2).m_fCoolTime <= 0.f )
			pRidingPetManager->GetSkillSlot()->SetBlindSkillSlot( iSkillNum, false );
		break;

	default:
		return false;
	}

	return true;
}

/*virtual*/ bool CX2GUUser::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		 GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		 GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		 GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		 GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END )
		return true;

	return false;
}


void CX2GUUser::StopTime_StateStart()
{

	int index = 0;
	if( IsMyUnit() == false )
	{
		m_StopAllList.clear();
		while( m_LuaManager.BeginTable( "STOP_ALL_UNIT", index ) == true )
		{
			D3DXVECTOR3 stopTime;
			LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
			LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
			LUA_GET_VALUE( m_LuaManager, 3, stopTime.z, -1.0f );
			if( stopTime.x != -1.0f )
			{
				stopTime.x -= g_pKTDXApp->GetElapsedTime() * (m_FrameBufferNum-1);
				if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
				{
					switch ( g_pX2Game->GetGameType() )
					{
					case CX2Game::GT_DUNGEON:
					case CX2Game::GT_BATTLE_FIELD:
						{
							g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
							g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
						} break;

					case CX2Game::GT_PVP:
						{
							g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
							g_pX2Game->StopAllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
						} break;

					default:
						ASSERT( !L"wrong path" );
						break;
					}
				}
				else
					m_StopAllList.push_back( stopTime );
			}

			index++;
			m_LuaManager.EndTable();
		}

		index = 0;
		m_Stop2AllList.clear();
		while( m_LuaManager.BeginTable( "STOP2_ALL_UNIT", index ) == true )
		{
			D3DXVECTOR3 stopTime;
			LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
			LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
			LUA_GET_VALUE( m_LuaManager, 3, stopTime.z, -1.0f );
			if( stopTime.x != -1.0f )
			{
				stopTime.x -= g_pKTDXApp->GetElapsedTime() * (m_FrameBufferNum-1);
				if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
				{
					switch ( g_pX2Game->GetGameType() )
					{
					case CX2Game::GT_DUNGEON:
					case CX2Game::GT_BATTLE_FIELD:
						{
							g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f, this, stopTime.z );
							g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 700.0f, this, stopTime.z );
						} break;

					case CX2Game::GT_PVP:
						{
							g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f, this, stopTime.z );
							g_pX2Game->Stop2AllUnit( stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f, this, stopTime.z );
						} break;

					default:
						ASSERT( !L"wrong path" );
						break;
					}
				}
				else
					m_Stop2AllList.push_back( stopTime );
			}
			index++;
			m_LuaManager.EndTable();
		}
	}
	index = 0;
	m_StopOtherList.clear();
	while( m_LuaManager.BeginTable( "STOP_OTHER_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		if( stopTime.x != -1.0f )
		{
			stopTime.x -= g_pKTDXApp->GetElapsedTime() * (m_FrameBufferNum-1);
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
			{
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					{
						g_pX2Game->StopOtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.0f );
						g_pX2Game->StopOtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 700.0f );
					} break;

				case CX2Game::GT_PVP:
					{
						g_pX2Game->StopOtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f );
						g_pX2Game->StopOtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f );
					} break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}
			}
			else
				m_StopOtherList.push_back( stopTime );
		}

		index++;
		m_LuaManager.EndTable();
	}

	index = 0;
	m_Stop2OtherList.clear();
	while( m_LuaManager.BeginTable( "STOP2_OTHER_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( m_LuaManager, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( m_LuaManager, 2, stopTime.y, -1.0f );
		if( stopTime.x != -1.0f )
		{
			stopTime.x -= g_pKTDXApp->GetElapsedTime() * (m_FrameBufferNum-1);
			if( stopTime.x <= g_pKTDXApp->GetElapsedTime() )
			{
				switch ( g_pX2Game->GetGameType() )
				{
				case CX2Game::GT_DUNGEON:
				case CX2Game::GT_BATTLE_FIELD:
					{
						g_pX2Game->Stop2OtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 800.f );
						g_pX2Game->Stop2OtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 700.0f );
					} break;

				case CX2Game::GT_PVP:
					{
						g_pX2Game->Stop2OtherUnit( this, stopTime.y / 2.0f, &m_FrameDataNow.syncData.position, 2000.0f );
						g_pX2Game->Stop2OtherUnit( this, stopTime.y, &m_FrameDataNow.syncData.position, 1300.0f );
					} break;

				default:
					ASSERT( !L"wrong path" );
					break;
				}
			}
			else
				m_Stop2OtherList.push_back( stopTime );
		}

		index++;
		m_LuaManager.EndTable();
	}
}
int CX2GUUser::GetMyUnitSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_ )
{
	if( NULL == GetUnit()  )
		return 0;

	return GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID_ );
}

void CX2GUUser::Send_SKILL_USE_REQ( CX2SkillTree::SKILL_ID eSkillID )
{
	// ��ų ��� ī��Ʈ�� ����, ����, ��Ʋ�ʵ� ������ by ������
	if( CX2Main::XS_DUNGEON_GAME != g_pMain->GetNowStateID() &&
		CX2Main::XS_PVP_GAME != g_pMain->GetNowStateID() &&
		CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() )
		return;

	// �ش� ��ų ��������Ʈ�� ���� ��쿡�� ������?
	if( false == g_pData->GetQuestManager()->HasSkillUseSubQuest( static_cast< int >( eSkillID ) ) )
		return;

	KEGS_SKILL_USE_REQ kPacket;
	kPacket.m_iSkillID		= (int)eSkillID;

	g_pData->GetServerProtocol()->SendPacket( EGS_SKILL_USE_REQ, kPacket );
	//g_pMain->AddServerPacket( EGS_SKILL_USE_ACK, 60.f );
}

void CX2GUUser::ForceDownAndDetonationProcess()
{
	switch ( m_NowStateData.stateID )
	{
	case USI_DAMAGE_DOWN_FRONT:
	case USI_DAMAGE_DOWN_BACK:
	case USI_DAMAGE_AIR_DOWN_INVINCIBLE:
	case USI_RIDING_DAMAGE_FRONT:
	case USI_RIDING_DAMAGE_BACK:
		{
			bool bApplyForceDownORDetonation = false;
			if( true == m_pGageData->UpdateDamageFlushMP() )
			{
				bApplyForceDownORDetonation = true;
			}

			if( true == m_pGageData->UpdateFlushForceDown() )
			{
				bApplyForceDownORDetonation = true;
			}

			// ����, �ٿ��ġ �� �� �ϳ��� ���� �Ǿ��ٸ�
			// �����̻� �ʱ�ȭ��Ű�� �Ѿ������� �ϱ�
			if( true == bApplyForceDownORDetonation )
			{
				m_vecStunPtr.clear();
				m_vecFreezePtr.clear();
				m_ExtraDamagePack.m_Stun.Init();
				SetChangeGroggyState(false);
			}

		} break;
	default:
		break;
	}
}

/*virtual*/void CX2GUUser::DamageDataChangeProcess()
{
	if( -1.f == m_fCommonDamageChangeTime )
		return;

    if( true == m_pXSkinAnim->EventTimerOneshot( m_fCommonDamageChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType			= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit	= this;
			if( m_bApplyMemo == true )
			{
				if( false == SetDamageData( "DAMAGE_DATA_NEXT_MEMO", m_bApplyMemo ) )
				{
					SetDamageData( "DAMAGE_DATA_NEXT" );
				}
			}
			else
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
}
/** @function : ParsingBuffFactorID
	@brief : ��ũ��Ʈ���� ������ ������Ʈ��, ������ �ð���
		     �ڱ��ڽſ��� �����ϴ� ���� ���� ���� �Ľ�
*/
/*virtual*/ void CX2GUUser::ParsingBuffFactorID( KLuaManager& luaManager_, const bool IsEqippedMemo_ /*= false*/ )
{
	// ���� ������Ʈ�� ��ų ���� �ʱ�ȭ
	m_uiNowStateSkillLevel = 0;

	// BuffFactorID, ���� �ð�
	m_mapStateInsertBuffFactor.clear();

	if( true == IsEqippedMemo_ )
	{
		if( true == luaManager_.BeginTable( "BUFF_FACTOR_ID_AND_APPLY_TIME_MEMO" ) )
		{
			int iTableIndex = 1;
			while( true == luaManager_.BeginTable( iTableIndex++  ) )
			{
				UINT uiBuffFactorID = 0;
				float fStartAnimationTime = 0.f;

				LUA_GET_VALUE( luaManager_, 1, uiBuffFactorID, 0);

				LUA_GET_VALUE( luaManager_, 2, fStartAnimationTime, 0.f);

				luaManager_.EndTable();

				if( 0 == uiBuffFactorID )
					break;

#if X2_DEV_TOOLS
				if( m_mapStateInsertBuffFactor.end() != m_mapStateInsertBuffFactor.find( uiBuffFactorID ) )
				{
					DISPLAY_ERROR(L"BuffFactor Duplication Error!");
				}
#endif //_IN_HOUSE_

				m_mapStateInsertBuffFactor.insert( std::make_pair( uiBuffFactorID, fStartAnimationTime ) );
			}

			luaManager_.EndTable(); //BUFF_FACTOR_ID
		}
	}
	else
	if( true == luaManager_.BeginTable( "BUFF_FACTOR_ID_AND_APPLY_TIME" ) )
	{
		int iTableIndex = 1;
		while( true == luaManager_.BeginTable( iTableIndex++  ) )
		{
			UINT uiBuffFactorID = 0;
			float fStartAnimationTime = 0.f;

			LUA_GET_VALUE( luaManager_, 1, uiBuffFactorID, 0);

			LUA_GET_VALUE( luaManager_, 2, fStartAnimationTime, 0.f);

			luaManager_.EndTable();

			if( 0 == uiBuffFactorID )
				break;

#if X2_DEV_TOOLS
			if( m_mapStateInsertBuffFactor.end() != m_mapStateInsertBuffFactor.find( uiBuffFactorID ) )
			{
				DISPLAY_ERROR(L"BuffFactor Duplication Error!");
			}
#endif //_IN_HOUSE_

			m_mapStateInsertBuffFactor.insert( std::make_pair( uiBuffFactorID, fStartAnimationTime ) );
		}

		luaManager_.EndTable(); //BUFF_FACTOR_ID
	}

	if( false == m_mapStateInsertBuffFactor.empty() )
	{
		// ���� ���� ���� ���� �� ���
		{
			// BuffFactor ���� ���� ��ų ID �Ľ�
			CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE;
			LUA_GET_VALUE_ENUM( luaManager_, "BUFF_FACTOR_RELATIVE_SKILL_ID", eSkillID,	CX2SkillTree::SKILL_ID,	CX2SkillTree::SI_INVALID );

			bool bIsNowStateSkill = false;
			// ������Ʈ���� ������ ��ų�� ���ٸ� ���� ������Ʈ ��ų ���
			if( CX2SkillTree::SI_INVALID == eSkillID )
			{
				eSkillID = GetNowStateSkillID();
				bIsNowStateSkill = true;
			}

			// ���� ������Ʈ or ������Ʈ���� ������ ��ų�� �ִٸ�
			if( CX2SkillTree::SI_NONE != eSkillID )
			{
				if( NULL != GetUnit() )
				{
					// ��ųƮ���� ���� üũ.( ��ų ���� ���� )
					m_uiNowStateSkillLevel = static_cast<UINT>( GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID ) );
				}
			}
		}
		//������ �ش� skill�� ��������� üũ�ϴ� �����Դϴ�.
		//������ �ش� skill�� ����� �ʾҴٸ� buff�� 1�����̶� �������� �ʽ��ϴ�.
		bool CheckSkillLevel = false;
		LUA_GET_VALUE( luaManager_, "RELATIVE_SKILL_LEARN_CHECK", CheckSkillLevel,	false );

		//skill�� üũ ���� true�̰� ��ų�� ����� �ʾҴٸ� ������ �����Ű�� �ʰ� return ��ŵ�ϴ�.
		if( true == CheckSkillLevel && 1 > m_uiNowStateSkillLevel )
		{
			m_mapStateInsertBuffFactor.clear();
			return;
		}

		map< UINT, float >::iterator it = m_mapStateInsertBuffFactor.begin();
		while( it != m_mapStateInsertBuffFactor.end() )
		{
			if( 0.f == it->second )
			{
				SetBuffFactorToGameUnitByBuffFactorID( it->first, max( m_uiNowStateSkillLevel, 1 ) );
				m_mapStateInsertBuffFactor.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}
/** @function : CommonFrameMove_InsertBuffFactor
	@brief : ��ũ��Ʈ���� ������ ������Ʈ��, ������ �ð���
			�ڱ��ڽſ��� �����ϴ� ���� ���� ����
*/
void CX2GUUser::CommonFrameMove_InsertBuffFactor()
{
	if( NULL == m_pXSkinAnim )
		return;

	map< UINT, float >::iterator it = m_mapStateInsertBuffFactor.begin();
	while( it != m_mapStateInsertBuffFactor.end() )
	{
		// �ִϸ��̼� �ð��� ������ �ð� ���Ķ��
		if( m_pXSkinAnim->GetNowAnimationTime() >= it->second )
		{
			SetBuffFactorToGameUnitByBuffFactorID( it->first, max( m_uiNowStateSkillLevel, 1 ) );
			m_mapStateInsertBuffFactor.erase(it++);
		}
		else
		{
			++it;
		}
	}
}


/// �����̰� ������ ��ų Ű �Է� ���� ��� ����
void CX2GUUser::CurrentSkillKeyManager::Init()
{
	iSkillSlotIndex = -1;
	m_bIsSkillSLotSwap = false;
}
/** @function : CheckSkillSlotSwap
	@brief : ��ų���� ü���� ���ο� ���� ���� �ε��� ������Ʈ.
				�� ����ü�� ����Ϸ��� EventProcess���� ������ ȣ���ؾ� ��.
*/
void CX2GUUser::CurrentSkillKeyManager::CheckSkillSlotSwap()
{
	// ��ų���� �����ߴٸ� Index�� �������ֱ�.
	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() &&
		m_bIsSkillSLotSwap != g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() )
	{
		iSkillSlotIndex += ( true == m_bIsSkillSLotSwap ) ? 4 : -4;
		m_bIsSkillSLotSwap = !m_bIsSkillSLotSwap;
	}
}
void CX2GUUser::CurrentSkillKeyManager::InitCurrentSkillKeyManager( int iSkillSlotIndex_ )
{
	iSkillSlotIndex = iSkillSlotIndex_;
	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		m_bIsSkillSLotSwap = g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap();
	}
}
/** @function : IsPressedCurrentSkillKey
	@brief : ���� ��ų�� �Էµ� ��ųŰ�� �� �Է� �Ǿ����� üũ
*/
bool CX2GUUser::CurrentSkillKeyManager::IsPressedCurrentSkillKey( const CX2GUUser::InputData& InputData_ )
{
	bool bIsKeyPress = false;
	switch( iSkillSlotIndex )
	{
	case 0: bIsKeyPress = InputData_.oneA; break;
	case 1: bIsKeyPress = InputData_.oneS; break;
	case 2: bIsKeyPress = InputData_.oneD; break;
	case 3: bIsKeyPress = InputData_.oneC; break;
	case 4: bIsKeyPress = InputData_.oneQ; break;
	case 5: bIsKeyPress = InputData_.oneW; break;
	case 6: bIsKeyPress = InputData_.oneE; break;
	case 7: bIsKeyPress = InputData_.oneR; break;
	default :
		ASSERT("!DodgeAndSlash SkillIndex Error");
		break;
	}

	return bIsKeyPress;
}
/** @function : IsPressedCurrentSkillKey
	@brief : ���� ��ų�� �Էµ� ��ųŰ�� �ȴ����� �������� üũ
*/
bool CX2GUUser::CurrentSkillKeyManager::IsUnpressedCurrentSkillKey( const CX2GUUser::InputData& InputData_ )
{
	bool bIsKeyPress = false;
	switch( iSkillSlotIndex )
	{
	case 0: bIsKeyPress = InputData_.pureA; break;
	case 1: bIsKeyPress = InputData_.pureS; break;
	case 2: bIsKeyPress = InputData_.pureD; break;
	case 3: bIsKeyPress = InputData_.pureC; break;
	case 4: bIsKeyPress = InputData_.pureQ; break;
	case 5: bIsKeyPress = InputData_.pureW; break;
	case 6: bIsKeyPress = InputData_.pureE; break;
	case 7: bIsKeyPress = InputData_.pureR; break;
	default :
		ASSERT("!DodgeAndSlash SkillIndex Error");
		break;
	}

	return !bIsKeyPress;
}


void CX2GUUser::ResetLinkOverChargeIllusion()
{
	CX2GUEve* pEve = static_cast<CX2GUEve*>(this);
	if( NULL != pEve )
		pEve->ResetLinkOverChargeIllusion();
}

/** @function : HyperModeStart
	@brief : ������ ��带 ����Ѵٴ� �÷��� ����
*/
void CX2GUUser::RidingHyperModeStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WaitState );

	m_bHyperModeUsed = true;
	m_iLastStateHyperModeCount = m_FrameDataNow.syncData.m_HyperModeCount;
	m_iHyperModeCountUsed = m_FrameDataNow.syncData.m_HyperModeCount;

	if ( GetRemainHyperModeTime() > 0.0f )
		SetHyperModeCount( max( GetLastStateHyperModeCount() - 1, 0 ) );	 /// ������ �簢�� ó��(����)
	else
		SetHyperModeCount( 0 );

	m_FrameDataNow.syncData.m_HyperModeCount = 0;
	m_bReserveHyperModeZero = false;

}
void CX2GUUser::RidingHyperModeEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_RIDING_JUMP_DOWN );
		else
			StateChange( USI_RIDING_WAIT );
	}
	CommonEventProcess();
}

	void CX2GUUser::RidingHyperModeEnd()
	{
		CommonStateEnd();
		m_iLastStateHyperModeCount = 0;
	}

	void CX2GUUser::RidingHyperModeStartFuture()
	{
		CommonStateStartFuture();
		m_bReserveHyperModeZero = true;
	}


/** @function	: ApplyBuffByUseHyperMode
	@brief		: ������ ������ ������ �����ϴ� �Լ�
*/
void CX2GUUser::ApplyBuffByUseHyperMode()
{
	if ( NULL == g_pMain )
		return;

	/// ������ ������ ���� ���� ��ȸ
	BOOST_FOREACH( CX2SocketItem::InfoByUseHyperMode sInfoByUseHyperMode, m_SocketData.m_vecInfoByUseHyperMode )
	{
		/// ������ ����� �� �ִ� ��ġ ( ���� �� ���� )
		const CX2SocketItem::POSSIBLE_USE_PLACE ePossible_Use_Place =  sInfoByUseHyperMode.m_ePossibleUsePlace;

		switch( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_TRAINING_GAME:
			{
				/// ���� �����ε� ���� ������ �ƴϸ�, ���� ����
				if ( CX2SocketItem::PUP_PVP_ONLY == ePossible_Use_Place )
					continue;
			} break;
		case CX2Main::XS_PVP_GAME:
			{
				/// ���� �����ε� ���� ������ �ƴϸ�, ���� ����
				if ( CX2SocketItem::PUP_DUNGEON_ONLY == ePossible_Use_Place )
					continue;
			}
		}

		switch ( sInfoByUseHyperMode.m_eHyperModeEffectiveType )
		{
		case CX2SocketItem::HMET_USE_BUFF:								/// ������ ������ ����
			{
				if ( NULL != sInfoByUseHyperMode.m_pBuffFactorPtr )
				{
					CX2BuffFactorPtr ptrBuffFactorClone = sInfoByUseHyperMode.m_pBuffFactorPtr->GetClonePtr( 0 );

					if ( NULL != ptrBuffFactorClone )
						SetBuffFactorToGameUnit( *ptrBuffFactorClone );

				}
			} break;
		case CX2SocketItem::HMET_ERASE_DEBUFF_AND_ADD_MP:		///  (define ADJUST_SECRET_ITEM_OPTION) ����� ����, ���� ������ ������� mp ȸ������ ���� <- �ɷ��ִ� ������� �����, ���� ������ŭ MP ȸ��
			{
				EraseDebuffAndAddMP( sInfoByUseHyperMode.m_fEffectiveValue );
			} break;
		case CX2SocketItem::HMET_ADD_HP_IMMEDIATELY:			/// HP 5% ��� ȸ�� �� �������� HP ȸ��
			{
				const float fAddHPValue = GetMaxHp() * sInfoByUseHyperMode.m_fEffectiveValue;	/// ü�� ȸ����

				UpNowHp( fAddHPValue );

				/// ȸ���� ǥ�� ( Percent )
				if ( NULL != g_pX2Game )
					g_pX2Game->DisplayAddHPInformation( sInfoByUseHyperMode.m_fEffectiveValue * 100.f, GetPos(), GetDirVector() );

			}break;
		default:
			{
				/// ������ ����� ȿ�� ������ �̻��ϳ׿�!!!
				ASSERT( !"Unknown Hyper Mode Effective!!!" );
			}
		}
	}
}

/** @function	: EraseDebuffAndAddMP
	@brief		:  (define ADJUST_SECRET_ITEM_OPTION) ����� ����, ���� ������ ������� mp ȸ������ ���� <- �ɷ��ִ� ������� �����, ���� ������ŭ MP ȸ��
	@param		: ȸ�� MP��
*/
void CX2GUUser::EraseDebuffAndAddMP( IN const float fMPValue_ )
{
	if ( NULL != g_pX2Game )
	{
		int iEraseCount = 0;	/// ���� ���� ��

		/// �طο� ȿ����� ���ǵ� �������� ����� ��� ��ȯ
		BOOST_FOREACH( BUFF_TEMPLET_ID eBuffTempletID , g_pX2Game->GetOfficiallyDebuffList() )
		{
			auto FindFunc
				= [eBuffTempletID]( CX2BuffTempletPtr ptrBuffTemplet_ ) {
					return eBuffTempletID == ptrBuffTemplet_->GetBuffTempletID();
			};

			vector<CX2BuffTempletPtr>::iterator vItr
				= std::find_if( m_vecBuffTempletPtr.begin(), m_vecBuffTempletPtr.end(), FindFunc );

			/// �ش� ������� �ɷ� ���� ��
			if ( m_vecBuffTempletPtr.end() != vItr )
			{
				EraseBuffTempletFromGameUnitByIterator( vItr );		/// ����� ����

				++iEraseCount;										/// ���� ī��Ʈ ����
			}
		}

		// ���� �Ӽ��� Ǯ �� �ֵ��� �߰�
		if( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.f )
		{
			m_ExtraDamagePack.m_EnchantBlaze.m_fTime = 0.f;
			++iEraseCount;
		}
		if( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.f)
		{
			m_ExtraDamagePack.m_EnchantFrozen.m_fTime = 0.f;
			++iEraseCount;
		}
		if( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.f)
		{
			m_ExtraDamagePack.m_EnchantPoison.m_fTime = 0.f;
			++iEraseCount;
		}
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f)
		{
			m_ExtraDamagePack.m_EnchantShock.m_fTime = 0.f;
			++iEraseCount;
		}

		/// ���� ���� �� ��ŭ MP ȸ��
		if ( 0 < iEraseCount )
		{
			const float fAddMP = fMPValue_ * static_cast<float>( iEraseCount );
			UpNowMp( fAddMP );

			/// ȸ���� ǥ��
			g_pX2Game->DisplayAddMPInformation( fAddMP, GetPos(), GetDirVector() );
		}
	}
}

void CX2GUUser::ApplyWorldLightColor( OUT CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if( g_pX2Game->GetWorld() != NULL && m_bUseWorldColor == true )
	{
 		pRenderParam_->color.r	= g_pX2Game->GetWorld()->GetLightColor().r;
 		pRenderParam_->color.g	= g_pX2Game->GetWorld()->GetLightColor().g;
 		pRenderParam_->color.b	= g_pX2Game->GetWorld()->GetLightColor().b;
	}
}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
void CX2GUUser::_EncodeUserSyncPos( USHORT& usPosX, USHORT& usPosZ, const D3DXVECTOR3& position, unsigned char lastTouchLineIndex )
//#else   X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//void CX2GUUser::_EncodeUserSyncPos( USHORT& usPosX, USHORT& usPosY, USHORT& usPosZ, const D3DXVECTOR3& position, unsigned char lastTouchLineIndex )
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
{
    const CKTDGLineMap::LineData* pLineData = NULL;
	if( !g_pX2Game || !g_pX2Game->GetLineMap()
        || ( pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )lastTouchLineIndex ) ) == NULL )
    {
		StateLog( L"lastTouchLineIndex�� �̻��ѵ�" );
    	usPosX = floatToHalf( position.x );
//#ifndef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//        usPosY = floatToHalf( position.y );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
	    usPosZ = floatToHalf( position.z );
		return;
    }

	usPosX = floatToHalf( position.x - pLineData->startPos.x );
//#ifndef X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
//    usPosY = floatToHalf( position.y - pLineData->startPos.y );
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
	usPosZ = floatToHalf( position.z - pLineData->startPos.z );
}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC

void    CX2GUUser::ResetLocalAndRelativePosAndTimers()
{
    ZeroMemory( m_acLocalRelativePos, sizeof(m_acLocalRelativePos) );
    m_FrameDataFuture.syncData.m_dwRelativePos = 0;
    m_FrameDataNow.syncData.m_dwRelativePos = 0;
    for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
    {
        m_afLocalRelativePosTimers[ i ] = 0.f;
    }
}

void    CX2GUUser::IncrementLocalRelativePosTimers(  float fElapsedTime )
{
    if ( fElapsedTime > 0.f )
    {
        for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
        {
            m_afLocalRelativePosTimers[ i ] += fElapsedTime;
        }
    }//if
}

void    CX2GUUser::PhysicProcess_RetrieveRelativePosInfo( CX2GUUser* pGameUnit, int iUnitIndex, OUT int& iDirection )
{
    iDirection = 0;

    int iMyUnitIndex = GetUnitIndex();
    if ( iUnitIndex >= 0 && iUnitIndex < MAX_GAME_USER_UNIT_NUM )
    {
        iDirection = m_acLocalRelativePos[ iUnitIndex ];
        if ( iDirection != 1 && iDirection != 2 )
            return;

        int iUnitDirection = pGameUnit->GetRelativePos( iMyUnitIndex );
        if ( iUnitDirection == 1 || iUnitDirection == 2 )
        {
            if ( iUnitDirection == 1 )
                iUnitDirection = 2;
            else
                iUnitDirection = 1;

            if ( const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( iUnitIndex ) )
            {
                float fToleranceTime = 0.f;
                fToleranceTime = pSlotData->m_fAvgPingTime + pSlotData->m_fAvgPingStd + 0.2f;

                if ( m_afLocalRelativePosTimers[ iUnitIndex ] < fToleranceTime )
                    return;

                if ( iUnitDirection == iDirection )
                    return;

                if ( iMyUnitIndex < iUnitIndex )
                    return;

                iDirection = iUnitDirection;
            }//if
        }
    }//if
}


void    CX2GUUser::PhysicProcess_UpdateRelativePos()
{
    if ( IsMyUnit() == false )
        return;

    if ( m_FrameDataFuture.stateParam.bCanPassUnit == true )
    {
        ResetLocalAndRelativePosAndTimers();
        return;
    }

    D3DXVECTOR2 vMyUnitMinMaxY( m_FrameDataFuture.syncData.position.y, m_FrameDataFuture.syncData.position.y + GetUnitHeight( true ) );
    int  iUserUnitSize = __min( g_pX2Game->GetUserUnitListSize(), MAX_GAME_USER_UNIT_NUM );
    DWORD   dwNewRelativePos = 0;
    int iMyUnitIndex = GetUnitIndex();
    for( int i = 0; i < iUserUnitSize; i++ )
    {
        CX2GUUser* pGameUnit = g_pX2Game->GetUserUnit( i );
        if ( pGameUnit == NULL || pGameUnit == this || pGameUnit->GetGameUnitState() == GUSI_DIE )
        {
            m_acLocalRelativePos[i] = 0;
            m_afLocalRelativePosTimers[ i ] = 0.f;
            continue;
        }
        const StateParam* pGameUnitStateParam = &pGameUnit->GetStateparam( true );
        if ( false == ( ( ( pGameUnit->GetTeam() != GetTeam() && false == pGameUnitStateParam->bCanPassUnit )

             )
             && ( pGameUnit->GetAllyTeam() == CX2Room::TN_NONE || pGameUnit->GetAllyTeam() != static_cast<CX2Room::TEAM_NUM>(GetTeam()) ) // ����� �Ʊ����� �����Ǿ����� �ʰų�, ����� �Ʊ����� �ƴϸ�
	        && !( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && false == pGameUnit->DidReceiveFirstSyncPacket() ) )  ) /// ��Ʋ�ʵ忡�� ��ũ��Ŷ�� �ѹ��� �������� �ƴϸ�
        {
            m_acLocalRelativePos[i] = 0;
            m_afLocalRelativePosTimers[ i ] = 0.f;
            continue;
        }
        int iOldLocalDirection = m_acLocalRelativePos[i];
        int iNewLocalDirection = iOldLocalDirection;
        D3DXVECTOR3 vPosFuture = pGameUnit->GetPos( true );
        D3DXVECTOR3 vDirVectorFromMeToOther;
	    const float fGameUnitYMin = vPosFuture.y;
	    const float fGameUnitYMax = fGameUnitYMin + pGameUnit->GetUnitHeight( true );
	    if( (fGameUnitYMin >= vMyUnitMinMaxY.x && fGameUnitYMin <= vMyUnitMinMaxY.y) ||
		    (fGameUnitYMax >= vMyUnitMinMaxY.x && fGameUnitYMax <= vMyUnitMinMaxY.y) ||
		    (fGameUnitYMin <= vMyUnitMinMaxY.x && fGameUnitYMax >= vMyUnitMinMaxY.y) )
        {
            vDirVectorFromMeToOther = vPosFuture - m_FrameDataFuture.syncData.position;
            vDirVectorFromMeToOther.y = 0.f;
            int iHostDirection = pGameUnit->GetRelativePos( iMyUnitIndex );
            switch( iHostDirection )
            {
            case 1:
                iHostDirection = 2;
                break;
            case 2:
                iHostDirection = 1;
                break;
            default:
                iHostDirection = 0;
            }//switch
            if ( D3DXVec3Dot( &m_FrameDataFuture.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                iNewLocalDirection = 2;
            else
                iNewLocalDirection = 1;
            if ( iOldLocalDirection == 0
                && iNewLocalDirection != 0
                && iNewLocalDirection != iHostDirection )
            {
                m_afLocalRelativePosTimers[ i ] = 0.f;
            }
        }
        else
        {
            if ( fGameUnitYMin >= vMyUnitMinMaxY.y )
                iNewLocalDirection = 3;
            else
                iNewLocalDirection = 4;
            m_afLocalRelativePosTimers[ i ] = 0.f;
        }
        m_acLocalRelativePos[i] = iNewLocalDirection;
        if ( iNewLocalDirection == 1 || iNewLocalDirection == 2 )
            dwNewRelativePos |= iNewLocalDirection << ( i * 2 );
    }
    m_FrameDataFuture.syncData.m_dwRelativePos = dwNewRelativePos;
}
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC


/** @function	: ProcessSkillAtDie
	@brief		: ��� ������ �ߵ��Ǵ� ��ų �˻�
	@param		: �԰� �� ������, ������ ������
	@return		: ��ų �ߵ� ����
*/
bool CX2GUUser::ProcessSkillAtDie( IN const float fFinalDamage_, IN CX2DamageManager::DamageData* pDamageData_ /*= NULL*/ )
{
	bool bActiveSkill = false;		/// ��ų �ߵ� ����

	/// �׿����� ���������
	if ( fFinalDamage_ > GetNowHp() )
	{
		/// �ƶ� 2�� ���� ���� - ȸ������-------------------------------------------------------------------------------------------
		int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AYR_RESURRECTION, true );

		if( iSkillLevel > 0 )
		{
			CX2GUAra* pAra = static_cast<CX2GUAra*>( this );

			/// �ƶ��� ��, ȸ������ ����
			if ( NULL != pAra &&
				 NULL != GetUnit() &&
				 CX2Unit::UT_ARA == GetUnit()->GetType() &&
				 true == pAra->ProcessResurrection( iSkillLevel ) )
			{
				SetForceInvincible( 1.f );								/// �����ð� ���� ����

				/// ������ �Ѿ�߷� ������
				if ( NULL != pDamageData_ )
					pDamageData_->reActResult = CX2DamageManager::RT_DOWN;
				else		/// ������ �����Ͱ� ���ٸ�, �׳� ������Ʈ �ٲ� ������
					StateChange( USI_DAMAGE_DOWN_FRONT );

				bActiveSkill = true;
			}
		}
		/// ------------------------------------------------------------------------------------------------------------------------

		/// ���� 1�� ���� �Ĺ� ������ - ������ ���-------------------------------------------------------------------------------
		if( m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY <= 0.f )
		{
			int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LCR_SOFTBODY, true );

			if( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LCR_SOFTBODY );

				if( NULL != pSkillTemplet )
				{
					if( GetRandomFloat() < pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel ) )
					{
						SetNowHp( fFinalDamage_ + 1.f );	/// ü�� 1�� ������ ����

						m_SkillRelatedData.m_fSkillCoolTimeLeft_SI_P_COMMON_SOFTBODY = pSkillTemplet->GetSkillCoolTimeValue( iSkillLevel );
						SetBuffFactorToGameUnit( pSkillTemplet, 0 );

						/// ������ �Ѿ�߷� ������
						if ( NULL != pDamageData_ )
							pDamageData_->reActResult = CX2DamageManager::RT_DOWN;
						else		/// ������ �����Ͱ� ���ٸ�, �׳� ������Ʈ �ٲ� ������
							StateChange( USI_DAMAGE_DOWN_FRONT );

						bActiveSkill = true;
					}
				}
			}
		}
		/// ------------------------------------------------------------------------------------------------------------------------
	}

	if ( true == bActiveSkill )
		return true;

	return false;
}

/** @function : IsAvaliableActionDuringDamageReact
	@brief :  �ǰ� �� ��밡���� Action���� �˻��ϴ� �Լ�.
			  ������ ����� ���� ������ �����Լ� ������� �ʾ���.
*/
bool CX2GUUser::IsAvaliableActionDuringDamageReact( const UCHAR ucNowAction_ ) const
{
	switch( ucNowAction_ )
	{
	case UAI_MANA_SHIELD:		/// �����ǵ�
	case UAI_ENDURANCE_POWER:	/// �ر� - ��(˭)
	case UAI_ACTIVE_STASIS_FIELD:/// �����̽� �ʵ�
		{
			return true;
		} break;
	default:
		return false;
		break;
	}
}
/** @function : GetActionSyncData
	@brief : SyncData �����̳ʷκ��� �ǰ��� ��� ������ Action���� ���
*/
void CX2GUUser::GetActionSyncData( OUT map< int, UCHAR >& mapActionSyncData_, IN const vector<SyncData>& ReceiveSyncDataList_ ) const
{
	// int : �ε��� , UCHAR : NowAction
	mapActionSyncData_.clear();
	int iIndex = 0;
	BOOST_FOREACH( const SyncData& ReceiveSyncData, ReceiveSyncDataList_ )
	{
		if( true == IsAvaliableActionDuringDamageReact(ReceiveSyncData.nowAction) )
		{
			mapActionSyncData_.insert( std::make_pair(iIndex, ReceiveSyncData.nowAction) );
		}
		++iIndex;
	}
}
/** @function : SetActionSyncData
	@brief : ����� action������ SyncData�� �����ϱ�
*/
void CX2GUUser::SetActionSyncData( OUT vector<SyncData>& ReceiveSyncDataList_, IN const map< int, UCHAR >& mapActionSyncData_ ) const
{
	if( false == mapActionSyncData_.empty() )
	{
		int iIndex = 0;
		BOOST_FOREACH( SyncData& ReceiveSyncData, ReceiveSyncDataList_ )
		{
			map< int, UCHAR >::const_iterator mitActionSyncData = mapActionSyncData_.find( iIndex );
			if( mitActionSyncData != mapActionSyncData_.end() )
			{
				ReceiveSyncData.nowAction = mitActionSyncData->second;
			}
			++iIndex;
		}
	}
}


void CX2GUUser::LoadEffectSetFromScript( IN KLuaManager& luaManager, IN const bool IsEqippedMemo_/* = false */)
{
	m_vecEffectSetToPlay.resize(0);


	bool bIsEffectSetMemo = false;

	if( true == IsEqippedMemo_ )
	{
		if( true == luaManager.BeginTable( "EFFECT_SET_LIST_MEMO" ) )
		{
			std::wstring wstrEffectSetName = L"";
			float fStartAnimationTime = 0.f;

			for( int iValueIndex = 1; iValueIndex < 100; iValueIndex += 2 )
			{
				if( false == luaManager.GetValue( iValueIndex, wstrEffectSetName ) )
					break;

				if( false == luaManager.GetValue( iValueIndex+1, fStartAnimationTime ) )
					break;

				m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrEffectSetName, fStartAnimationTime ) );
			}


			luaManager.EndTable(); //EFFECT_SET_LIST_MEMO

			bIsEffectSetMemo = true;
		}
	}

	//APPLY_MEMO�� ���ǵǾ� ���� �ʰų�
	//APPLY_MEMO�� ���ǵǾ� ������ EFFECT_SET_LIST_MEMO�� ���ǵǾ� ���� ���� ��� EFFECT_SET_LIST_MEMO�� �Ľ��Ѵ�.(DAMAGE_DATA_MEMO�� ���� APPLY_MEMO�� ������ ���)
	if( false == IsEqippedMemo_ || ( true == IsEqippedMemo_ && false == bIsEffectSetMemo ) )
	{
		if( true == luaManager.BeginTable( "EFFECT_SET_LIST" ) )
		{
			std::wstring wstrEffectSetName = L"";
			float fStartAnimationTime = 0.f;

		for( int iValueIndex = 1; iValueIndex < 100; iValueIndex += 2 )
		{
			if( false == luaManager.GetValue( iValueIndex, wstrEffectSetName ) )
				break;

			if( false == luaManager.GetValue( iValueIndex+1, fStartAnimationTime ) )
				break;

				m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrEffectSetName, fStartAnimationTime ) );
			}


			luaManager.EndTable(); //EFFECT_SET_LIST
		}
	}


	bIsEffectSetMemo = false;
	if( true == IsEqippedMemo_ )
	{
		if( true == luaManager.BeginTable( "EFFECT_SET_LIST_EX_MEMO" ) )
		{
			std::wstring wstrEffectSetName = L"";
			float fStartAnimationTime = 0.f;




			vector<DELETE_CONDITION_EFFECT_SET> vecDeleteCondition;		// ���� ���� ����

			int iTableIndex = 1;

			while( true == luaManager.BeginTable( iTableIndex++  ) )
			{
				LUA_GET_VALUE( luaManager, 1, wstrEffectSetName,	 L"" );
				if( L"" == wstrEffectSetName )
					break;

				LUA_GET_VALUE( luaManager, 2, fStartAnimationTime, 0.f);


				int iTableCount = 3;			// ���̺� ī����, 3�� ���̺����� �о���� ������
				UINT uiDeleteConditionType = 0;	// ���� ����
				vecDeleteCondition.clear();

				while ( luaManager.GetValue( iTableCount, uiDeleteConditionType ) == true )
				{	// ���� �ִٸ�, ���̺� ī���� ������Ű�� ���� ������ ���Ϳ� �־��ش�.
					// ���� ���� ������ ����ٸ�, ASSERT �� ����.
#if X2_DEV_TOOLS
					// �系 �� ���, �ش� ���� ������ �ߺ��Ǵ��� üũ�Ѵ�.
					ASSERT( vecDeleteCondition.end() == find ( vecDeleteCondition.begin(), vecDeleteCondition.end(), static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) ) );
#endif // _IN_HOUSE_

					iTableCount++;
					vecDeleteCondition.push_back( static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) );
				}

				luaManager.EndTable();


				m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrEffectSetName, fStartAnimationTime, vecDeleteCondition ) );

			}


			luaManager.EndTable(); //EFFECT_SET_LIST_EX_MEMO

			bIsEffectSetMemo = true;
		}
	}

	//APPLY_MEMO�� ���ǵǾ� ���� �ʰų�
	//APPLY_MEMO�� ���ǵǾ� ������ EFFECT_SET_LIST_EX_MEMO�� ���ǵǾ� ���� ���� ��� EFFECT_SET_LIST_EX�� �Ľ��Ѵ�.(DAMAGE_DATA_MEMO�� ���� APPLY_MEMO�� ������ ���)
	if( false == IsEqippedMemo_ || ( true == IsEqippedMemo_ && false == bIsEffectSetMemo ) )
	{
		if( true == luaManager.BeginTable( "EFFECT_SET_LIST_EX" ) )
		{
			std::wstring wstrEffectSetName = L"";
			float fStartAnimationTime = 0.f;


		vector<DELETE_CONDITION_EFFECT_SET> vecDeleteCondition;		// ���� ���� ����

		int iTableIndex = 1;
		while( true == luaManager.BeginTable( iTableIndex++  ) )
		{
			LUA_GET_VALUE( luaManager, 1, wstrEffectSetName,	 L"" );
			if( L"" == wstrEffectSetName )
				break;

			LUA_GET_VALUE( luaManager, 2, fStartAnimationTime, 0.f);
			int iTableCount = 3;			// ���̺� ī����, 3�� ���̺����� �о���� ������
			UINT uiDeleteConditionType = 0;	// ���� ����
			vecDeleteCondition.clear();

			while ( luaManager.GetValue( iTableCount, uiDeleteConditionType ) == true )
			{	// ���� �ִٸ�, ���̺� ī���� ������Ű�� ���� ������ ���Ϳ� �־��ش�.
				// ���� ���� ������ ����ٸ�, ASSERT �� ����.
#if X2_DEV_TOOLS
				// �系 �� ���, �ش� ���� ������ �ߺ��Ǵ��� üũ�Ѵ�.
				ASSERT( vecDeleteCondition.end() == find ( vecDeleteCondition.begin(), vecDeleteCondition.end(), static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) ) );
#endif // _IN_HOUSE_

				iTableCount++;
				vecDeleteCondition.push_back( static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) );
			}

			luaManager.EndTable();


			m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrEffectSetName, fStartAnimationTime, vecDeleteCondition ) );
		}

			luaManager.EndTable(); //EFFECT_SET_LIST
		}
	}

	LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_STATE_END",		m_bDeleteEffectSetOnStateEnd,		false );
	LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DAMAGE_REACT",	m_bDeleteEffectSetOnDamageReact,	false );

	LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DIE",	m_bDeleteEffectSetOnDie,	false );

	LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_CUSTOM_STATE",	m_bDeleteEffectSetOnCustomState,	false );

	LUA_GET_VALUE( m_LuaManager, "CUSTOM_STATE_DELETE_EFFECT_SET_ON_DAMAGE_REACT",	m_bIsCustomStateDeleteEffectOnDamageReact,	false );

	LUA_GET_VALUE( m_LuaManager, "HYPER_EFFECT_SET",		m_bHyperEffectSet,		false );
	LUA_GET_VALUE( m_LuaManager, "EFFECT_SET_LIFE_TIME",	m_fEffectSetLifeTime,	-1.f );

}

void CX2GUUser::CommonFrameMove_EffectSet( float fNowAnimationTime )
{

	if( true == m_vecEffectSetToPlay.empty() )
		return;

	for( int i=0; i<(int)m_vecEffectSetToPlay.size(); ++i )
	{
		EffectSetToPlay& effectSetToPlay = m_vecEffectSetToPlay[i];
		if( 0.f == effectSetToPlay.m_fStartAnimationTime ||
			fNowAnimationTime >= effectSetToPlay.m_fStartAnimationTime )
		{
			CX2EffectSet::Handle hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( effectSetToPlay.m_wstrEffectSetName, this, NULL, m_bHyperEffectSet, GetPowerRate(), m_fEffectSetLifeTime);

			// ������ �ִϸ��̼� �ð��� �������� ����Ʈ�� �����ϴ� ���� effectSetToPlay.m_fStartAnimationTime�� �߰��Ǹ鼭 �ִϸ��̼��� �������� ���� �ʵ��� �������ش�
			if( 0.f != effectSetToPlay.m_fStartAnimationTime )
			{
				if ( CX2EffectSet::EffectSetInstance* pEffectSetInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSetInst ) )
				{
					pEffectSetInst->SetUseAnimationTiming( false );
				}
			}


			if( true == m_bDeleteEffectSetOnDamageReact )
			{
				m_vecEffectSetToDeleteOnDamageReact.push_back( hEffectSetInst );
			}

			if( true == m_bDeleteEffectSetOnStateEnd )
			{
				m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );
			}

			if( true == m_bDeleteEffectSetOnDie )
			{
				m_vecEffectSetToDeleteOnDie.push_back( hEffectSetInst );
			}

			if( true == m_bDeleteEffectSetOnCustomState )
			{
				m_vecEffectSetToDeleteOnCustomState.push_back( hEffectSetInst );
			}

			AddEffectSetDeleteCondition( hEffectSetInst, effectSetToPlay.m_vecDeleteCondition );

			m_vecEffectSetToPlay.erase( m_vecEffectSetToPlay.begin() + i );
			--i;
		}
	}
}

bool    CX2GUUser::FindNowAddSlashTraceWithWeaponIndex( int iWeaponIndex, OUT D3DXVECTOR3& vSlaceTraceTime ) const
{
    std::map< int, D3DXVECTOR3 >::const_iterator iter = m_mapNowAddSlashTrace.find( iWeaponIndex );
    if ( iter != m_mapNowAddSlashTrace.end() )
    {
        vSlaceTraceTime = iter->second;
        return true;
    }
    return false;
}

//{{ kimhc // 2010.12.07 // �Ž�ų
/** @function : LoadImmunityAtThisStateFromScript
	@brief : ���� ������Ʈ������ Immune ��ų ExtraDamage�� ��ũ��Ʈ���� �о���̴� �Լ�
*/
void CX2GUUser::LoadImmunityAtThisStateFromScript( IN KLuaManager& luaManager_ )
{
	m_vecImmunityAtThisState.resize( 0 );

	if ( true == luaManager_.BeginTable( "IMMUNITY_LIST_AT_THIS_STATE" ) )
	{

		int iIndex = 1;
		UINT eImmuneBuffOrDebuffType = 0;

		while ( luaManager_.GetValue( iIndex, eImmuneBuffOrDebuffType ) )
		{
			m_vecImmunityAtThisState.push_back( static_cast<BUFF_TEMPLET_ID>( eImmuneBuffOrDebuffType ) );
			++iIndex;
		}

		luaManager_.EndTable();
	}
}

//}} kimhc // 2010.12.07 // �Ž�ų

bool    CX2GUUser::FindNowSlashTraceTimeWithWeaponIndex( int iWeaponIndex, OUT D3DXVECTOR3& vSlashTraceTime )
{
    std::map< int, D3DXVECTOR3 >::const_iterator it;
	it = m_mapNowAddSlashTrace.find( iWeaponIndex );
	if( it != m_mapNowAddSlashTrace.end() )
	{
		vSlashTraceTime = it->second;
        return true;
	}
    return false;
}



void    CX2GUUser::ClearAnimEventTimerOneshotNow()
{
    if ( m_pXSkinAnim != NULL )
	    m_pXSkinAnim->ResetOneshotPerformed();
    if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimPtr() )
        m_ptrRidingPet->GetXSkinAnimPtr()->ResetOneshotPerformed();
}

void    CX2GUUser::ClearAnimEventTimerOneshotFuture()
{
    if ( m_pXSkinAnimFuture != NULL )
	    m_pXSkinAnimFuture->ResetOneshotPerformed();
    if ( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() )
        m_ptrRidingPet->GetXSkinAnimFuturePtr()->ResetOneshotPerformed();
}



/** @function	: GetSpecialAbilityValue
	@brief		: ��ų ���̵� �ش��ϴ� SA���� �޾ƿ��� �Լ�
	@param		: SA Ÿ��, ��ų ���̵�
	@return		: SA Value
*/
const float CX2GUUser::GetSpecialAbilityValue( IN const CX2SkillTree::SKILL_ABILITY_TYPE eSAValue, IN const CX2SkillTree::SKILL_ID eSkillID ) const
{
	float fReturnValue = 1.f;

	if ( NULL != g_pData &&
		 NULL != g_pData->GetSkillTree() &&
		 NULL != GetUnit() )
	{
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );

		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

			if( NULL != pSkillTemplet )
			{
				fReturnValue = pSkillTemplet->GetSkillAbilityValue( eSAValue, iSkillLevel );
			}
		}
	}

	return fReturnValue;
}

/** @function	: CreateNotEnoughMPEffect
	@brief		: MP�� ���� �� ������ ���� ����Ʈ ����
	@param		: ���� ��ġ, ���� ����
*/
void CX2GUUser::CreateNotEnoughMPEffect( IN const D3DXVECTOR3 vPos, IN const float fDegreeX, IN const float fDegreeY, IN const float fDegreeZ )
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_MAGIC_FAIL ) );
	if( NULL != pSeq )
	{
		PlaySequenceByTriggerCount( pSeq, vPos, 100, 100, 1 );
		D3DXVECTOR3 vAngle = GetRotateDegree();
		vAngle += D3DXVECTOR3( fDegreeX, fDegreeY, fDegreeZ );

		pSeq->SetAxisAngle( vAngle );
		pSeq->SetAddRotate( vAngle );
	}
}

/** @function	: IsEnoughMPForAttack
	@brief		: ���ݿ� �ʿ��� MP�� ����ϸ� true, ������� ������ false�� �Բ� ���� ����Ʈ �߻�
	@param		: MP �Һ�, �߻� ��ġ, �߻� Offset ��ġ, �߻� ����
	@return		: ���� ���� ����
*/
bool CX2GUUser::IsEnoughMPForAttack( IN const float fConsumeMP, IN const D3DXVECTOR3 vPos, IN const float fOffsetPos /*= 0.f;*/,
									 IN const D3DXVECTOR3 vRotDegree /*= D3DXVECTOR3( 0.f, 0.f, 0.f )*/ )
{
	if( FlushMp( fConsumeMP ) == true )
		return true;
	else	/// MP�� �����ϸ�, ���� ����Ʈ �߻�
	{
		D3DXVECTOR3 vFirePos = vPos;

		/// Offset ��ġ ����
		if ( false == IsSamef( 0.f, fOffsetPos ) )
			vFirePos += ( true == GetIsRight() ) ? GetDirVector() * 100.f : GetDirVector() * 100.f;

		CreateNotEnoughMPEffect( vFirePos, vRotDegree.x, vRotDegree.y, vRotDegree.z );
	}

	return false;
}

/*virtual*/void CX2GUUser::InitInpuDataProcess()
{
	if( m_fCanNotInputTimeZXArrow > 0.f )
	{
		m_InputData.Init_ZXArrowOnly();
	}

	if( true == IsCanNotIntput() )
	{
		m_InputData.Init();
	}
}

void CX2GUUser::PlaySoundRevive()
{
	const CX2Unit::UNIT_TYPE eUnitType = GetUnitType();

	if( g_pKTDXApp->GetDeviceManager() == NULL )
		return;

	if( eUnitType <= CX2Unit::UT_NONE || eUnitType >= CX2Unit::UT_END )
		return;

	wstring wstrSoundFile = L"";

	switch (eUnitType)
	{
	case CX2Unit::UT_ELSWORD:
		{
			wstrSoundFile = L"ElswordVoice_Revive0";
		} break;

	case CX2Unit::UT_ARME:
		{
			wstrSoundFile = L"AishaVoice_Revive0";
		} break;

	case CX2Unit::UT_LIRE:
		{
			wstrSoundFile = L"LenaVoice_Revive0";
		} break;

	case CX2Unit::UT_RAVEN:
		{
			wstrSoundFile = L"RavenVoice_Revive0";
		} break;

	case CX2Unit::UT_EVE:
		{
			wstrSoundFile = L"EveVoice_Revive0";
		} break;

	case CX2Unit::UT_CHUNG:
		{
			wstrSoundFile = L"ChungVoice_Revive0";
		} break;

	case CX2Unit::UT_ARA:
		{
			wstrSoundFile = L"AraVoice_Revive0";
		} break;

	case CX2Unit::UT_ELESIS:
		{
			wstrSoundFile = L"ElesisVoice_Revive0";
		} break;

	case CX2Unit::UT_ADD:
		{
			wstrSoundFile = L"AddVoice_Revive0";
		} break;

	default:
		break;
	}

	WCHAR buff[6];
	int iRandomSelect = RandomInt() % 2 + 1;
	StringCchPrintf( buff, ARRAY_SIZE(buff), L"%d.ogg ", iRandomSelect );
	wstrSoundFile += std::wstring(buff);

	if( wstrSoundFile.compare(L"") != 0 )
		g_pKTDXApp->GetDeviceManager()->PlaySound(wstrSoundFile.c_str(), false, false );
}


void CX2GUUser::InitPhaseShift()
{
	CX2GUAdd* pAdd = static_cast<CX2GUAdd*>(this);
	if( NULL != pAdd )
		pAdd->InitPhaseShift();
}

bool CX2GUUser::IsPlayAvatarEmotionSoundWithouEmotion  ( CX2Unit::EMOTION_TYPE eEmotionId )
{
	wstring wstrEmotionSoundName = L"";
	bool	bCheckHaveItem = false;
	switch ( eEmotionId )
	{
		case CX2Unit::ET_EMOTION_AVATAR7:
			for( int i=0; i<ARRAY_SIZE( ITEM_ID_EMOTION_AVATAR1 ); ++i )
			{
				if( NULL != GetUnit() )
				{
					if ( true == GetUnit()->GetInventory().CheckEquippingItem( ITEM_ID_EMOTION_AVATAR1[i] ) )
					{
						m_bIsPlayAvatarEmotionSoundWithoutEmotion = true;
						wstrEmotionSoundName = L"Emotion_Crayonpop_NoMotion.ogg";
						bCheckHaveItem = true;
					}
				}
			}
			break;
		default :
			return false;
	}
	if ( false == bCheckHaveItem )
		return false;

	if ( wstrEmotionSoundName != L"" )
	{
		if( m_pAvatarEmotionSound != NULL )
		{
			m_pAvatarEmotionSound->Stop();
			SAFE_CLOSE( m_pAvatarEmotionSound );
		}
		const float MAGIC_NUMBER = 500.f;
		const float fMaxDist = g_pKTDXApp->GetDGManager()->GetCamera().GetCameraDistance() + MAGIC_NUMBER;
		m_pAvatarEmotionSound = g_pKTDXApp->GetDeviceManager()->OpenSound( wstrEmotionSoundName, 10, true, fMaxDist );
		if( m_pAvatarEmotionSound != NULL )
		{
			if( CX2Main::XS_SQUARE_GAME != g_pMain->GetNowStateID() &&			// note!! �ŷ����忡���� �ϴ� 2D �����
				CX2Main::XS_PVP_ROOM	!= g_pMain->GetNowStateID() &&
				CX2Main::XS_WEDDING_GAME != g_pMain->GetNowStateID() &&
				true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
				true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() &&
				true == g_pMain->GetGameOption().GetOptionList().m_bEnable3DSound )
			{
				m_pAvatarEmotionSound->Set3DPosition( GetPos() );
				m_pAvatarEmotionSound->Play( false );
			}
			else
			{
				m_pAvatarEmotionSound->Play( false, false );
			}
		}
	}
	return true;
}


/** @function	: GetSkillLevelValue
	@brief		: ��ų ���̵� �ش��ϴ� ��ų ������ �޾ƿ��� �Լ�
	@param		: ��ų ���̵�
	@return		: ��ų ����
*/
const int CX2GUUser::GetSkillLevelValue( IN const CX2SkillTree::SKILL_ID eSkillID_ )
{
	if ( NULL == GetUnit() )
		return 1;

	const CX2UserSkillTree& userSkillTree	= GetUnit()->GetUnitData().m_UserSkillTree;
	const int				iSkillLevel		= userSkillTree.GetSkillLevel( eSkillID_ );

	return iSkillLevel;
}

/** @function	: SetEffectSoundParticle
	@brief		: ȿ���� ����Ʈ ��� �Լ�
	@param		: ���� �� ����, ���� ����Ʈ ����
*/
void CX2GUUser::SetEffectSoundParticle( IN const D3DXVECTOR3& vPos_, IN const wstring& wstrEffectName_ )
{
	if ( GetShowSmallGageAndName() && NULL != g_pX2Game && NULL != g_pX2Game->GetMinorParticle() )
	{
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( vPos_ );

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  wstrEffectName_.c_str(), projPos.x, projPos.y, 0.0f, 100, 100, 1, 1 );
	}
}
