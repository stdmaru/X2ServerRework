#include "stdafx.h"
#include "./X2GageData.h"

void CX2GageData::Gage::Increase( const float fIncrement, const float fMinimum /*= 0.f */ )
{

		fNow += fIncrement;

	if( fNow > fMax )
		fNow = fMax;

	if( fNow < fMinimum )
		fNow = fMinimum;

}
/*virtual*/ void CX2GageData::DetonationFrameMove( double /*fTime*/, float fElapsedTime )
{
	// ���� ����
	if( GetCanChargeMpForDetonation() == true )
	{
		SetMaxChargeMpForDetonation( GetNowMp() );
		UpNowChargeMpForDetonation( GetChangeRateChargeMpForDetonation() * fElapsedTime );

		const float fNowChargeMp = GetNowChargeMpForDetonation();

		if( fNowChargeMp > THREE_CHARGE )
			SetNowChargeMpForDetonation( THREE_CHARGE );
		else if( fNowChargeMp > GetMaxChargeMpForDetonation() )
			SetNowChargeMpForDetonation( GetMaxChargeMpForDetonation() );
		else if( fNowChargeMp < 0.0f )
			SetNowChargeMpForDetonation( 0.0f );

		SetCanChargeMpForDetonation( false );
	}
	else
	{
		SetNowChargeMpForDetonation( 0.0f );
		SetCanChargeMpForDetonation( false );
	}
}
/*virtual*/ void CX2GageData::OnFrameMove( double fTime, float fElapsedTime )
{
	DetonationFrameMove(fTime, fElapsedTime);

	if ( GetNowHp() > 0 )
	{
		if( GetEnableHPRelativeChangeRate() )
			UpNowHp( GetChangeRateHp() * GetMaxHp() * fElapsedTime );
		else
			UpNowHp( GetChangeRateHp() * fElapsedTime );
	}

	//}} oasis : ����� // 2009-10-6 //

	if( GetChangeRateTimeByItem() > 0.f )
	{
		SetChangeRateTimeByItem( GetChangeRateTimeByItem() - fElapsedTime );
		if( GetChangeRateTimeByItem() <= 0.f )
		{
			SetChangeRateTimeByItem( 0.0f );
			SetChangeRateByItem( 0.0f );
		}
	}

	const float fMpToBeUp = ( GetChangeRateMp() + GetChangeRateByItem() ) * fElapsedTime;
	UpNowMp( fMpToBeUp );

	if( GetNowHp() <= 0)
		m_pMpGage->ClearFactor();

	for ( UINT i = 0; i < m_pMpGage->vecFactor.size(); ++i )
	{
		GageFactor &gageFactor = m_pMpGage->vecFactor[i];
		if( gageFactor.fTime > 0.f )
		{
			m_pMpGage->fNow += gageFactor.fFactor * fElapsedTime;
			gageFactor.fTime -= fElapsedTime;

			if( gageFactor.fTime <= 0.f )
			{
				m_pMpGage->vecFactor.erase( m_pMpGage->vecFactor.begin() + i );
				--i;
			}
		}
	}

	if ( g_pKTDXApp->GetIsNowVeryfy() == true
		&& g_pInstanceData != NULL
		&& g_pInstanceData->GetVerifyGageManagerTimer() <= 0.f )
		Veryfy();
}

bool CX2GageData::FlushMp( const float fFlushMp_ )
{
	if( NULL == m_pMpGage || fFlushMp_ > m_pMpGage->fNow )
		return false;
	else
	{
		UpNowMp( -fFlushMp_ );
		return true;
	}
}

/*virtual*/ bool CX2GageData::DamageFlushMp()
{
	// ���� : ���� ��ŭ ������ ����
	if( GetNowChargeMpForDetonation() >= ONE_CHARGE )
	{
		SetActivateDetonation( true, GetNowChargeMpForDetonation() );

		return true;
	}
	else
		return false;
}


bool CX2GageData::FlushForceDown()
{
	int iNowForceDown = static_cast<int>( GetNowForceDown() );
	int iMaxForceDown = static_cast<int>( GetMaxForceDown() );

	WCHAR buf[100] = { 0, };

	swprintf_s( buf, L"NowForceDown: %d, MaxForceDown: %d\n", iNowForceDown, iMaxForceDown );

	DXUTTRACE( buf );

	if( GetNowForceDown() < GetMaxForceDown() )
		return false;
	else
	{
		SetActivateForceDown( true );

		return true;
	}
}
bool CX2GageData::UpdateDamageFlushMP()
{
	if( true == GetActivateDetonation() )
	{
		// MP �Ҹ�
		UpNowMp( m_fNowChargeMpForDetonation * -1 );
		SetNowChargeMpForDetonation( 0.f );

		// ���� Ȱ��ȭ ���� �ʱ�ȭ
		SetActivateDetonation(false, 0.f);

		return true;
	}

	return false;
}
bool CX2GageData::UpdateFlushForceDown()
{
	// �ٿ��ġ Ȱ��ȭ ���¶�� �ٿ��ġ �ʱ�ȭ
	if( true == GetActivateForceDown() )
	{
		// �ٿ��ġ �ʱ�ȭ
		SetNowForceDown( 0.0f );

		// Ȱ��ȭ ���� �ʱ�ȭ
		SetActivateForceDown( false );

		return true;
	}

	return false;
}


bool CX2GageData::FlushSoul( const float fFlushSoul_ )
{
	if( GetNowSoul() >= fFlushSoul_ )
	{
		UpNowSoul( -fFlushSoul_ );
		return true;
	}
	else
		return false;
}

/*virtual*/ void CX2GageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	if( NULL != pGageData_ )
	{
		SetNowHp( pGageData_->GetNowHp() );
		SetMaxHp( pGageData_->GetMaxHp() );

		SetNowMp( pGageData_->GetNowMp() );
		SetMaxMp( pGageData_->GetMaxMp() );

		SetNowSoul( pGageData_->GetNowSoul() );
		SetMaxSoul( pGageData_->GetMaxSoul() );

		SetHyperModeCount( pGageData_->GetHyperModeCount() );
		SetHyperModeRemainTime( pGageData_->GetHyperModeRemainTime() );

		SetNowChargeMpForDetonation( pGageData_->GetNowChargeMpForDetonation() );
	}
}

/*virtual*/ void CX2GageData::InitWhenGameIsOver()
{
	SetHyperModeRemainTime( 0.0f );
}

/*virtual*/ bool CX2GageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus_ ) const
{
	if ( Veryfy() )
	{
		kGamePlayStatus_.m_cCurHyperCount	= max( static_cast<char>( 0 ), static_cast<char>( GetHyperModeCount() ) );
		kGamePlayStatus_.m_iCurHP			= max( static_cast<int>( 0 ), static_cast<int>( GetNowHp() ) );
		kGamePlayStatus_.m_iCurHyperGage	= max( static_cast<int>( 0 ), static_cast<int>( GetNowSoul() ) );
		kGamePlayStatus_.m_iCurMP			= max( static_cast<int>( 0 ), static_cast<int>( GetNowMp() ) );
		kGamePlayStatus_.m_iMaxHP			= max( static_cast<int>( 0 ), static_cast<int>( GetMaxHp() ) );
		kGamePlayStatus_.m_iMaxMP			= max( static_cast<int>( 0 ), static_cast<int>( GetMaxMp() ) );

		return true;
	}
	else
		return false;
}

/*virtual*/ void CX2GageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus_ )
{
	SetHyperModeCount( static_cast<BYTE>( kGamePlayStatus_.m_cCurHyperCount ) );
	SetMaxHp( static_cast<float>( kGamePlayStatus_.m_iMaxHP ) );
	SetMaxMp( static_cast<float>( kGamePlayStatus_.m_iMaxMP ) );
	SetNowHp( static_cast<float>( kGamePlayStatus_.m_iCurHP ) );
	SetNowMp( static_cast<float>( kGamePlayStatus_.m_iCurMP ) );
	SetMaxSoul( 100.0f );
	SetNowSoul( static_cast<float>( kGamePlayStatus_.m_iCurHyperGage ) );

}

bool CX2GageData::Veryfy() const
{
	bool bIsVerify = true;
	bool bFindHacking = false;
	string strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_Verify;

	if( m_pHpGage->fNow.Verify()						== false ||
		m_pHpGage->fMax.Verify()						== false )
	{
		bIsVerify = false;
		bFindHacking = true;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_Hp_Verify;
	}
	else if( m_pMpGage->fNow.Verify()					== false ||
		m_pMpGage->fMax.Verify()					== false )
	{
		bIsVerify = false;
		bFindHacking = true;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_Mp_Verify;
	}
	else if( m_ForceDownGage.fNow.Verify()			== false ||
		m_ForceDownGage.fMax.Verify()				== false )
	{
		bIsVerify = false;


		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_DownGage_Verify;
	}
	else if( m_pHpGage->fChangeRate.Verify()			== false )
	{
		bIsVerify = false;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_Hp_ChangeRate_Verify;
	}
	else if( m_pMpGage->fChangeRate.Verify()			== false )
	{
		bIsVerify = false;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_Mp_ChangeRate_Verify;
	}
	else if( m_ChargeMpGageForDetonation.fNow.Verify()			== false ||
		m_ChargeMpGageForDetonation.fMax.Verify()			== false )
	{
		bIsVerify = false;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_MpChargeGage_Verify;
	}
	else if( m_ChargeMpGageForDetonation.fChangeRate.Verify()	== false )
	{
		bIsVerify = false;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_MpChangeGage_ChangeRate_Verify;
	}
	else if( m_SoulGage.fNow.Verify()				== false ||
		m_SoulGage.fMax.Verify()				== false )
	{
		bIsVerify = false;
		bFindHacking = true;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_SoulGage_Verify;
	}
	else if( m_SoulGage.fChangeRate.Verify()		== false)
	{
		bIsVerify = false;
		strAntiHackingDesc = ANTI_HACK_STRING_AntiHacking_GageManager_SoulGage_ChangeRate_Verify;
	}

	if( bIsVerify == false )
	{
		if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER &&
			g_pKTDXApp->GetFindHacking() == false )
		{
			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
		}


		if ( g_pInstanceData != NULL &&
			g_pInstanceData->GetVerifyGageManagerTimer() <= 0.f )
		{


			g_pMain->SendHackMail_DamageHistory(strAntiHackingDesc.c_str());

			g_pInstanceData->SetRemainedTimeByForceQuitGame( REMAINED_TIME_BY_FORCE_QUIT_GAME );

			if( bFindHacking == true )
			{
				g_pKTDXApp->SetFindHacking( true );
			}
		}
	}


	if( m_fChangeRateByItem.Verify() == false )
	{
		if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

	}
	if( m_fChangeRateTimeByItem.Verify() == false )
	{
		if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

	}
	if( m_fHyperModeRemainTime.Verify() == false )
	{
		if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

	}

	return bIsVerify;
}

void CX2GageData::UseSpecialAbilityInVillage( const CX2Item::ItemTemplet* pItemTemplet_ )
{
	const CX2Unit* pX2Unit = g_pData->GetMyUser()->GetSelectUnit();
	const CX2Unit::UnitData* pUnitData = &pX2Unit->GetUnitData();
	const CX2Unit::UNIT_CLASS eUnitClass = pX2Unit->GetClass();

	//��� ��ų �� ���ݼ��� ����(����, ����, ���� ȸ�� �������� ȿ���� X%����)
	float	fIncreaseRateFromMasterOfAlchemistSkill							= 1.0f;
	bool	bDoIHaveMasterOfAlchemistSkill			= false;
	int		iSkillLvMasterOfAlchemist				= 0;

	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pUnitData->m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_MASTER_OF_ALCHEMIST, pUnitData->m_byMemberShipGrade );

		if ( pSkillTemplet != NULL )
		{
			bDoIHaveMasterOfAlchemistSkill  = true;

			const CX2UserSkillTree& userSkillTree = pUnitData->m_UserSkillTree;

			const int iSkillLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

			fIncreaseRateFromMasterOfAlchemistSkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLevel );
		}

	}

    unsigned uNumSA = pItemTemplet_->GetNumSpecialAbility();
    for( unsigned u = 0; u != uNumSA; ++u )
	{
        const CX2Item::SpecialAbility& specialAbility = pItemTemplet_->GetSpecialAbility(u);

		switch ( specialAbility.GetType() )
		{
		case CX2Item::SAT_HP_UP:
			{
				float fIncreateRateHpUp = fIncreaseRateFromMasterOfAlchemistSkill;
				float fValue = static_cast< float >( specialAbility.m_Value1 );

				// ǻ������� ������� ����
				switch ( eUnitClass )
				{
				case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
				case CX2Unit::UC_CHUNG_IRON_PALADIN:
					{
						const CX2UserSkillTree& userSkillTree = pUnitData->m_UserSkillTree;

						int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

						if ( 0 < iSkillLv )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet =
								g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );
							float fIncreaseRateMetabolism = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );
							fIncreaseRateMetabolism = CalculateIncreasingRate( fIncreaseRateMetabolism );

							fIncreateRateHpUp += fIncreaseRateMetabolism;
						} // if
					} break;

				default:
					break;
				}


				// ��� ��ų �� ���ݼ��� ����(����, ����, ���� ȸ�� �������� ȿ���� X%����)
				fValue *= fIncreateRateHpUp;

				UpNowHp( fValue );

// 				std::wstringstream strStream;
// 				strStream << L"HP+ " << static_cast< int >( fValue );
// 				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
// 				pos.y += 50.0f + (i * 30.0f);
// 				if( NULL != g_pData->GetPicCharGreen() )
// 					g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER
			} break;

		case CX2Item::SAT_MP_UP:
			{
				float fIncreaseRateMpUp = fIncreaseRateFromMasterOfAlchemistSkill;
				float fValue = static_cast< float >( specialAbility.m_Value1 );
				// ��� ��ų �� ���ݼ��� ����(����, ����, ���� ȸ�� �������� ȿ���� X%����)
				fValue *= fIncreaseRateMpUp;

				UpNowMp( fValue );

// 				std::wstringstream strStream;
// 				strStream << L"MP+ " << static_cast< int >( fValue );
// 				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
// 				pos.y += 50.0f + (i * 30.0f);
// 				if( NULL != g_pData->GetPicCharBlue() )
// 					g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );

			} break;

		case CX2Item::SAT_SOUL_GAGE_UP:
			{
				UpNowSoul( static_cast<float>( specialAbility.m_Value1 ) );

// 				std::wstringstream strStream;
// 				strStream << L"SOUL+ " << pSa->m_Value1;
// 				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
// 				pos.y += 50.0f + (i * 30.0f);
// 				if( NULL != g_pData->GetPicCharRed() )
// 					g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
			} break;

		case CX2Item::SAT_HP_PERCENT_UP:
			{
				float fIncreateRateHpPercentUp = fIncreaseRateFromMasterOfAlchemistSkill;
				float fValue = specialAbility.m_Value1 / 100.0f;

				// ǻ������� ������� ����
				if ( CX2Unit::UC_CHUNG_FURY_GUARDIAN == eUnitClass )
				{
					const CX2UserSkillTree& userSkillTree = pUnitData->m_UserSkillTree;

					int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

					if ( 0 < iSkillLv )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet =
							g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

						float fIncreaseRateMetabolism = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );
						fIncreaseRateMetabolism = CalculateIncreasingRate( fIncreaseRateMetabolism );

						fIncreateRateHpPercentUp += fIncreaseRateMetabolism;
					} // if
				}
				// ��� ��ų �� ���ݼ��� ����(����, ����, ���� ȸ�� �������� ȿ���� X%����)
				if ( true == bDoIHaveMasterOfAlchemistSkill &&
                    EGG_ITEM_ID == pItemTemplet_->GetItemID()
                    )
					fValue += ( iSkillLvMasterOfAlchemist / 100.0f );

				fValue *= fIncreateRateHpPercentUp;

				UpNowHp( GetMaxHp() * fValue );

				std::wstringstream strStream;
				strStream << L"HP+ " << static_cast< int >( fValue * 100 ) << L" PERCENT";
// 				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
// 				pos.y += 50.0f + (i * 30.0f);
// 				if( NULL != g_pData->GetPicCharGreen() )
// 					g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );

			} break;

		case CX2Item::SAT_MP_PERCENT_UP:
			{
				float fValue = specialAbility.m_Value1 / 100.0f;
				if ( bDoIHaveMasterOfAlchemistSkill == true )
					fValue *= fIncreaseRateFromMasterOfAlchemistSkill;

				UpNowMp( GetMaxMp() * fValue );

// 				std::wstringstream strStream;
// 				strStream << L"MP+ " << static_cast< int >( pSa->m_Value1 * fIncreaseRateFromMasterOfAlchemistSkill ) << L" PERCENT";
// 				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
// 				pos.y += 50.0f + (i * 30.0f);
// 				if( NULL != g_pData->GetPicCharGreen() )
// 					g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
			} break;

		default:
			break;
		}
	}

}

/*virtual*/ CX2GageData* CX2GageData::GetCloneGageData()
{
	return new CX2GageData( *this );
}

/*virtual*/ void CX2GageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *pGageData_;
}

/*virtual*/ void CX2ChungGageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	if( NULL == pGageData_ )
		return;


	CX2GageData::UpdateDataFromGameUnit( pGageData_ );

	// ĳ���� ��!!
	const CX2ChungGageData* pChungGageData = static_cast<const CX2ChungGageData*>( pGageData_ );

	if( NULL != pChungGageData )
	{
		SetNowCannonBallCount( pChungGageData->GetNowCannonBallCount() );
		SetMaxCannonBallCount( pChungGageData->GetMaxCannonBallCount() );

		SetNowCannonBallCountEx( pChungGageData->GetNowCannonBallCountEx() );
		SetMaxCannonBallCountEx( pChungGageData->GetMaxCannonBallCountEx() );

		SetBerserkMode( pChungGageData->GetBerserkMode() );
		SetBerserkModeChanged( pChungGageData->GetBerserkModeChanged() );
		SetCannonBallChanged( pChungGageData->GetCannonBallChanged() );
	}
}

/*virtual*/ void CX2ChungGageData::InitWhenGameIsOver()
{
	CX2GageData::InitWhenGameIsOver();
	SetBerserkMode( false );
	SetBerserkModeChanged( true );
}

/*virtual*/ bool CX2ChungGageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const
{
	/// EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE(7)�� ������ ������ bitmask ó�� �����ϸ� �ȴ�.
	kGamePlayStatus.m_iCharAbilCount = static_cast<char>( GetNowCannonBallCountEx() * EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE + GetNowCannonBallCount() ); // û

	return CX2GageData::GetMyPlayStatusToPacket( kGamePlayStatus );
}

/*virtual*/ void CX2ChungGageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus )
{
	CX2GageData::SetMyPlayStatusFromPacket( kGamePlayStatus );

	/// EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE(7) �κ� bitmask ���� ó�� �����ϸ� �ȴ�.
	const int iCannonBallCount = kGamePlayStatus.GetCannonBallCount() % EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE;
	const int iExtraCannonBallCount = (kGamePlayStatus.GetCannonBallCount() - iCannonBallCount) / EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE;

	// ���⼭ MaxCount�� m_cCannonBallCount�� ������ ������ MaxCount ���� ������ 0�̾ NowCannonBallCount�� ����� ���� ���� �ʱ� ����
	SetMaxCannonBallCount( iCannonBallCount );
	SetNowCannonBallCount( iCannonBallCount );

	SetNowCannonBallCountEx( iExtraCannonBallCount );
	SetMaxCannonBallCountEx( iExtraCannonBallCount );

	SetCannonBallChanged( true );
}

/*virtual*/ CX2GageData* CX2ChungGageData::GetCloneGageData()
{
	return new CX2ChungGageData( *this );
}

void CX2ChungGageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *( static_cast<CX2ChungGageData*>( pGageData_ ) );
	SetCannonBallChanged( true );
}




/*virtual*/ void CX2ElswordGageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	if ( NULL == pGageData_ )
		return;

	CX2GageData::UpdateDataFromGameUnit( pGageData_ );

	// ���� ��
	const CX2ElswordGageData* pElswordGageData = static_cast<const CX2ElswordGageData*>( pGageData_ );

	if( NULL != pElswordGageData )
	{
		SetWayOfSwordState( pElswordGageData->GetWayOfSwordState() );
		SetWayOfSwordPoint( pElswordGageData->GetWayOfSwordPoint() );
	}
}

/*virtual*/ void CX2ElswordGageData::InitWhenGameIsOver()
{
	CX2GageData::InitWhenGameIsOver();
	SetWayOfSwordState( 0 );
	SetChangedWayOfSwordState( true );
}

bool CX2ElswordGageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const
{
	kGamePlayStatus.m_iCharAbilCount = static_cast<int>( GetWayOfSwordPoint() );	// ���ҵ�
	return CX2GageData::GetMyPlayStatusToPacket( kGamePlayStatus );
}

/*virtual*/ void CX2ElswordGageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus )
{
	CX2GageData::SetMyPlayStatusFromPacket( kGamePlayStatus );
	SetWayOfSwordPoint( static_cast<float>( kGamePlayStatus.GetWSP() ) );
}

CX2GageData* CX2ElswordGageData::GetCloneGageData()
{
	return new CX2ElswordGageData( *this );
}

void CX2ElswordGageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *( static_cast<CX2ElswordGageData*>( pGageData_ ) );
	SetChangedWayOfSwordState( true );
}

//#pragma region CX2AraGageData

//#pragma region GetBerserkMode
/** @function 	: GetBerserkMode
	@brief 		: ����ȭ ��� ���� ���
*/
bool CX2AraGageData::GetBerserkMode() const
{
	return m_bBerserkMode;
}
//#pragma endregion ����ȭ ��� Ȯ��

//#pragma region SetBerserkMode
/** @function 	: SetBerserkMode
	@brief 		: ����ȭ ��� ����
*/
void CX2AraGageData::SetBerserkMode( const bool bBerserkMode_ )
{
	m_bBerserkMode = bBerserkMode_;
}
//#pragma endregion ����ȭ ��� ����

//#pragma region GetBerserkModeChanged
/** @function 	: GetBerserkModeChanged
	@brief 		: ����ȭ ���� ����Ǿ����� Ȯ��
*/
bool CX2AraGageData::GetBerserkModeChanged() const
{
	return m_bBerserkModeChanged;
}
//#pragma endregion ����ȭ ���� ����Ǿ����� Ȯ��

//#pragma region SetBerserkModeChanged
/** @function 	: SetBerserkModeChanged
	@brief 		: ����ȭ ���� ����Ǿ����� ����
*/
void CX2AraGageData::SetBerserkModeChanged( bool bBerserkModeChanged_ )
{
	m_bBerserkModeChanged = bBerserkModeChanged_;
}
//#pragma endregion ����ȭ ���� ����Ǿ����� ����

//#pragma region GetForcePowerChanged
/** @function 	: GetForcePowerChanged
	@brief 		: ��� ���� Ȯ��
*/
bool CX2AraGageData::GetForcePowerChanged() const
{
	return m_bForcePowerChanged;
}
//#pragma endregion ��� ���� Ȯ��

//#pragma region SetForcePowerChanged
/** @function 	: SetForcePowerChanged
	@brief 		: ��� ���� ����
*/
void CX2AraGageData::SetForcePowerChanged( const bool bForcePowerChanged_ )
{
	m_bForcePowerChanged = bForcePowerChanged_;
}
//#pragma endregion ��� ���� ����

//#pragma region GetNowForcePower
/** @function 	: GetNowForcePower
	@brief 		: ���� ��� Ȯ��
*/
int CX2AraGageData::GetNowForcePower() const
{
	return m_iNowForcePowerCount;
}
//#pragma endregion ���� ��� Ȯ��

//#pragma region SetNowForcePower
/** @function 	: SetNowForcePower
	@brief 		: ���� ��� ����
*/
void CX2AraGageData::SetNowForcePower( const int iNowForcePowerCount_ )
{
	if ( GetMaxForcePower() < iNowForcePowerCount_ )
		m_iNowForcePowerCount = GetMaxForcePower();
	else if ( 0 > iNowForcePowerCount_ )
		m_iNowForcePowerCount = 0;
	else
		m_iNowForcePowerCount = iNowForcePowerCount_;
}
//#pragma endregion ���� ��� ����

//#pragma region GetMaxForcePower
/** @function 	: GetMaxForcePower
	@brief 		: �ִ� ��� Ȯ��
*/
int CX2AraGageData::GetMaxForcePower() const
{
	return m_iMaxForcePowerCount;
}
//#pragma endregion �ִ� ��� Ȯ��

//#pragma region SetMaxForcePower
/** @function 	: SetMaxForcePower
	@brief 		: �ִ� ��� ����
*/
void CX2AraGageData::SetMaxForcePower( const int iMaxForcePowerCount_ )
{
	m_iMaxForcePowerCount = iMaxForcePowerCount_;
}
//#pragma endregion �ִ� ��� ����

//#pragma region IsFullForcePower
/** @function 	: IsFullForcePower
	@brief 		: ����� �ִ�ġ���� Ȯ��
*/
bool CX2AraGageData::IsFullForcePower() const
{
	return GetNowForcePower() >= GetMaxForcePower();
}
//#pragma endregion ����� �ִ�ġ���� Ȯ��

//#pragma region IsEmptyForcePower
/** @function 	: IsEmptyForcePower
	@brief 		: ����� ������ Ȯ��
*/
bool CX2AraGageData::IsEmptyForcePower() const
{
	return GetNowForcePower() <= 0;
}
//#pragma endregion ����� ������ Ȯ��

//#pragma region UpdateDataFromGameUnit
/** @function 	: UpdateDataFromGameUnit
	@brief 		: GageDate ����
*/
/*virtual*/ void CX2AraGageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	CX2GageData::UpdateDataFromGameUnit( pGageData_ );
	const CX2AraGageData* pAraGageData = static_cast<const CX2AraGageData*>( pGageData_ );

	if( NULL != pAraGageData )
	{
		SetNowForcePower( pAraGageData->GetNowForcePower() );
		SetMaxForcePower( pAraGageData->GetMaxForcePower() );

		SetBerserkMode( pAraGageData->GetBerserkMode() );
		SetBerserkModeChanged( pAraGageData->GetBerserkModeChanged() );
		SetForcePowerChanged( pAraGageData->GetForcePowerChanged() );
	}
}
//#pragma endregion GageDate ����

//#pragma region InitWhenGameIsOver
/** @function 	: InitWhenGameIsOver
	@brief 		: ������ ������ �� �ʱ�ȭ
*/
/*virtual*/ void CX2AraGageData::InitWhenGameIsOver()
{
	CX2GageData::InitWhenGameIsOver();
	SetBerserkMode( false );
	SetBerserkModeChanged( true );
}
//#pragma endregion ������ ������ �� �ʱ�ȭ

//#pragma region GetMyPlayStatusToPacket
/** @function 	: GetMyPlayStatusToPacket
	@brief 		: ��Ŷ���� ���� �÷��� ���� ���
*/
/*virtual*/ bool CX2AraGageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const
{
	kGamePlayStatus.m_iCharAbilCount = static_cast<char>( GetNowForcePower() );

	return CX2GageData::GetMyPlayStatusToPacket( kGamePlayStatus );
}
//#pragma endregion ��Ŷ���� ���� �÷��� ���� ���

//#pragma region SetMyPlayStatusFromPacket
/** @function 	: SetMyPlayStatusFromPacket
	@brief 		: ��Ŷ�� ���� �÷��� ���� ����
*/
/*virtual*/ void CX2AraGageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus )
{
	CX2GageData::SetMyPlayStatusFromPacket( kGamePlayStatus );

	const int iForcePower = kGamePlayStatus.GetForcePower();
	const int iLevel = g_pData->GetMyUser()->GetSelectUnit()->GetPrevLevel();

	SetMaxForcePower( min( iLevel / 10, 6 ) + 4 );
	SetNowForcePower( iForcePower );
	SetForcePowerChanged( true );
}
//#pragma endregion ��Ŷ�� ���� �÷��� ���� ����

//#pragma region GetCloneGageData
/** @function 	: GetCloneGageData
	@brief 		: Gage ���� ������ ���
*/
/*virtual*/ CX2GageData* CX2AraGageData::GetCloneGageData()
{
	return new CX2AraGageData( *this );
}
//#pragma endregion Gage ���� ������ ���

//#pragma region CopyGageData
/** @function 	: CopyGageData
	@brief 		: GageDate ����
*/
void CX2AraGageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *( static_cast<CX2AraGageData*>( pGageData_ ) );
	SetForcePowerChanged( true );
}
//#pragma endregion GageDate ����

//#pragma endregion Ŭ����


/*virtual*/ void CX2ElesisGageData::DetonationFrameMove( double /*fTime*/, float fElapsedTime )
{
	// ���� ����
	if( GetCanChargeMpForDetonation() == true )
	{
		// ���� ���¿����� ���� ������ �̿��� ���� ó��
		if( WSS_DESTRUCTION == GetWayOfSwordState() )
		{
			// ���� �ִ� ��ġ ����
			SetMaxChargeMpForDetonation( _CONST_ELESIS_DETONATION_::MAX_CHARGE_DETONATION_VALUE );

			// ���� ���� ����
			UpNowChargeMpForDetonation( _CONST_ELESIS_DETONATION_::CHARGE_RATE_FOR_DETONATION * fElapsedTime );

			// ���� �ִ� ��ġ�� ����
			const float fNowChargeMp = GetNowChargeMpForDetonation();
			if( fNowChargeMp > GetMaxChargeMpForDetonation() )
				SetNowChargeMpForDetonation( GetMaxChargeMpForDetonation() );

			SetCanChargeMpForDetonation( false );
		}
		else
		{
			SetMaxChargeMpForDetonation( GetNowMp() );
			UpNowChargeMpForDetonation( GetChangeRateChargeMpForDetonation() * fElapsedTime );

			const float fNowChargeMp = GetNowChargeMpForDetonation();

			if( fNowChargeMp > THREE_CHARGE )
				SetNowChargeMpForDetonation( THREE_CHARGE );
			else if( fNowChargeMp > GetMaxChargeMpForDetonation() )
				SetNowChargeMpForDetonation( GetMaxChargeMpForDetonation() );
			else if( fNowChargeMp < 0.0f )
				SetNowChargeMpForDetonation( 0.0f );

			SetCanChargeMpForDetonation( false );
		}
	}
	else
	{
		SetNowChargeMpForDetonation( 0.0f );
		SetCanChargeMpForDetonation( false );
	}
}

/*virtual*/ void CX2ElesisGageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	if( NULL == pGageData_ )
		return;

	CX2GageData::UpdateDataFromGameUnit( pGageData_ );

	// ���� ��
	const CX2ElesisGageData* pElesisGageData = static_cast<const CX2ElesisGageData*>( pGageData_ );

	if( NULL != pElesisGageData )
	{
		SetWayOfSwordState( pElesisGageData->GetWayOfSwordState() );
		SetWayOfSwordPoint( pElesisGageData->GetWayOfSwordPoint() );
	}
}

/*virtual*/ void CX2ElesisGageData::InitWhenGameIsOver()
{
	CX2GageData::InitWhenGameIsOver();
	SetWayOfSwordState( 0 );
	SetChangedWayOfSwordState( true );
}

bool CX2ElesisGageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const
{
	kGamePlayStatus.m_iCharAbilCount = static_cast<int>( GetWayOfSwordPoint() );	// ���ҵ�
	return CX2GageData::GetMyPlayStatusToPacket( kGamePlayStatus );
}

/*virtual*/ void CX2ElesisGageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus )
{
	CX2GageData::SetMyPlayStatusFromPacket( kGamePlayStatus );
	SetWayOfSwordPoint( static_cast<float>( kGamePlayStatus.GetWSP() ) );
}

CX2GageData* CX2ElesisGageData::GetCloneGageData()
{
	return new CX2ElesisGageData( *this );
}

void CX2ElesisGageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *( static_cast<CX2ElesisGageData*>( pGageData_ ) );
	SetChangedWayOfSwordState( true );
}
/*virtual*/ bool CX2ElesisGageData::DamageFlushMp()
{
	if( WSS_DESTRUCTION == GetWayOfSwordState() )
	{
		if( GetNowChargeMpForDetonation() >= _CONST_ELESIS_DETONATION_::MAX_CHARGE_DETONATION_VALUE )
		{
			const float fConsumeWspForDetonation = g_pData->GetDamageManager()->GetDestRageWSPGain_Elesis();
			float fWayOfSwordPoint = GetWayOfSwordPoint();
			// �⺻ ��ġ�� �������̱� ������ �Ҹ��Ű���� + ���� �ʿ�
			fWayOfSwordPoint += fConsumeWspForDetonation;

			// �Ҹ�� ���� ��ǳ ��ġ�� ������ �ʵ��� �ִ밪 ����
			SetWayOfSwordPoint( min( 0.f, fWayOfSwordPoint ) );
			return true;
		}
		else
			return false;
	}
	else
	{
		return CX2GageData::DamageFlushMp();
	}

	return false;
}


CX2GageData* CX2AddGageData::GetCloneGageData()
{
	return new CX2AddGageData( *this );
}

void CX2AddGageData::CopyGageData( IN CX2GageData* pGageData_ )
{
	ASSERT( NULL != pGageData_ );
	*this = *( static_cast<CX2AddGageData*>( pGageData_ ) );

	SetChangedMutationCount( true );	/// ���� ������ ����
}

/*virtual*/ void CX2AddGageData::UpdateDataFromGameUnit( const CX2GageData* pGageData_ )
{
	if( NULL == pGageData_ )
		return;

	CX2GageData::UpdateDataFromGameUnit( pGageData_ );

	const CX2AddGageData* pAddrGageData = static_cast<const CX2AddGageData*>( pGageData_ );

	if( NULL != pAddrGageData )
	{
		SetIsFormationMode( pAddrGageData->GetIsFormationMode() );
		SetChangedMutationCount( pAddrGageData->GetChangedMutationCount() );
		SetMutationCount( pAddrGageData->GetMutationCount() );
		SetDPValue( pAddrGageData->GetDPValue() );
		SetChangeFormationCoolTime( pAddrGageData->GetChangeFormationCoolTime() );
		SetMaxDPValue( MAX_DP_GAGE_VALUE );
	}
}

/*virtual*/ bool CX2AddGageData::GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const
{
	if ( Veryfy() )
	{
		CX2GageData::GetMyPlayStatusToPacket( kGamePlayStatus );

		int iSyncData = ( true == GetIsFormationMode() ) ? 1 : 0;				/// ���� ��� ���� ���� ����

		iSyncData = iSyncData << 4;												/// ���� ��� ��ȯ ��Ÿ���� �ֱ� ����, 4��Ʈ�� ������ �ű���

		iSyncData |= static_cast<int>( GetChangeFormationCoolTime() ) & 0x000F;	/// ���� ��� ��ȯ ��Ÿ�� ����

		iSyncData = iSyncData << 4;												/// ���� ��ġ�� �ֱ� ����, 4��Ʈ�� ������ �ű���

		iSyncData |= GetMutationCount() & 0x000F;								/// ���� ��ġ ����

		iSyncData =  iSyncData << 16;											/// DP ��ġ�� �ֱ� ����, 16��Ʈ�� ������ ����

		iSyncData |= static_cast<int>( GetDPValue() ) & 0xFFFF;					/// DP ��ġ ����

		kGamePlayStatus.m_iCharAbilCount = iSyncData;

	return true;
	}
	else
		return false;
}

/*virtual*/ void CX2AddGageData::SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus )
{
	CX2GageData::SetMyPlayStatusFromPacket( kGamePlayStatus );

	int iSyncData = kGamePlayStatus.GetNPAndMutationAndFormationMode();

	SetIsFormationMode( ( ( ( iSyncData >> 24 ) & 0x000F ) == 1 ) ? true : false  );		/// ���� ��� ���� ���� ����

	SetChangeFormationCoolTime( static_cast<float>( ( iSyncData  >> 20 ) & 0x000F ) );	/// ���� ��� ��ȯ ��Ÿ�� ����

	SetMutationCount( ( iSyncData  >> 16 ) & 0x000F  );									/// ���� ��ġ ����

	SetDPValue( static_cast<float>( iSyncData & 0xFFFF ) );								/// DP ��ġ ����

	SetMaxDPValue( MAX_DP_GAGE_VALUE );													/// DP �ִ� ��ġ ����

	SetChangedMutationCount( true );													/// ����
}

/*virtual*/ void CX2AddGageData::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2GageData::OnFrameMove(fTime, fElapsedTime);

	/// ���� ��Ÿ�� ����
	if ( 0.f < m_fChangeFormationCoolTime )
	{
		m_fChangeFormationCoolTime -= fElapsedTime;

		if ( 0.f >= m_fChangeFormationCoolTime )
			m_fChangeFormationCoolTime = 0.f;
	}
}
