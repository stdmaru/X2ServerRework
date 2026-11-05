#include "stdafx.h"
#include "./X2UserSkillTree.h"


/** @function	CX2userSkillTree
	@brief		������, ���� ��ų �ʱ�ȭ, ���Ŭ���� �ʱ�ȭ
	*/
CX2UserSkillTree::CX2UserSkillTree(void)
	: m_eSkillSlotBExpirationState( SSBES_EXPIRED )
	, m_wstrSkillSlotBEndDate( L"" )
	, m_iUnitClass( CX2Unit::UC_NONE )
	, m_usUsingPage( 0 )
	, m_usOpenedPage( 0 )
	, m_usSelectPage( 0 )
{
	BOOST_STATIC_ASSERT( ARRAY_SIZE( m_aEquippedSkill ) == ARRAY_SIZE( m_aEquippedSkillSlotB ) );
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		m_aEquippedSkill[i].Init();
		m_aEquippedSkillSlotB[i].Init();
	}

	m_iGuildClass = 0;

	ZeroMemory( m_arrSkillPoint, sizeof(m_arrSkillPoint) );
	ZeroMemory( m_arrCashSkillPoint, sizeof(m_arrCashSkillPoint) );

}

/** @function	~CX2UserSkillTree
	@brief		�Ҹ���
*/
CX2UserSkillTree::~CX2UserSkillTree(void)
{
}

/** @function	OnFrameMove
	@brief		������ ���� ��, ��ų ��Ÿ�� ó��
	@param[in]	fTime : ���� �ð�
	@param[in]	fElapsedTime : ��� �ð�
*/
void CX2UserSkillTree::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();




	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		SkillSlotData& skillData = m_aEquippedSkill[i];
		if( skillData.m_fCoolTimeLeft > 0.f )
		{
			skillData.m_fCoolTimeLeft -= fElapsedTime;
			if( skillData.m_fCoolTimeLeft < 0.f )
			{
				skillData.m_fCoolTimeLeft = 0.f;
			}

			pGageManager->SetMySkillCoolTimeList( i, skillData.m_fCoolTimeLeft );
		}
	}

	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); i++ )
	{
		SkillSlotData& skillData = m_aEquippedSkillSlotB[i];
		if( skillData.m_fCoolTimeLeft > 0.f )
		{
			skillData.m_fCoolTimeLeft -= fElapsedTime;
			if( skillData.m_fCoolTimeLeft < 0.f )
			{
				skillData.m_fCoolTimeLeft = 0.f;
			}

			pGageManager->SetMySkillCoolTimeList( i + EQUIPPED_SKILL_SLOT_COUNT, skillData.m_fCoolTimeLeft );
		}
	}




	if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		int iCurrentSP = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSPoint + g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint;
		static int iOldSP = 0;

		if( iCurrentSP != iOldSP )
		{
			if( NULL != g_pData->GetPlayGuide() )
			{
				if( iCurrentSP > iOldSP )
				{	//SP�� �������� �þ��� �� SP ȹ�� ���̵� ���
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_OBTAIN_SKILL_POINT, true, iCurrentSP);
				}
				iOldSP = iCurrentSP;
			}

			if( iCurrentSP > 0 )
			{	//SP�� 0�ʰ� �� �� SP ���� ���̵� ���

				if( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
				{
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_HAS_SKILL_POINT, true, g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_SKILL_TREE ) );
				}
			}
			else
			{
				g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_HAS_SKILL_POINT, false);
			}
		}
	}

	/// ��ų ���� ���� ���� ��ȿ�� �˻�
	if ( g_pKTDXApp->GetIsNowVeryfy() == true
		&& g_pInstanceData != NULL
		&& g_pInstanceData->GetVerifyGageManagerTimer() <= 0.f
		&& true == IsEnableSkillAcquiredPage()
		)
	{
		SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].begin();

		for ( mit; mit != m_vecSkillDataMap[m_usUsingPage].end(); ++mit )
		{
			if( mit->second.m_iSkillLevel.Verify() == false ||
				mit->second.m_iSkillCSPoint.Verify() == false ||
				mit->second.m_iIncreaseSkillLevelByBuff.Verify() == false ||
				mit->second.m_iIncreaseSkillLevelBySocket.Verify() == false )
			{
				if( NULL != g_pData &&
					NULL != g_pData->GetMyUser() &&
					NULL != g_pData->GetServerProtocol() )
				{
					if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER &&
						g_pKTDXApp->GetFindHacking() == false )
					{
						g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
					}

					return;
				}
			}
		}

		SkillDataMap::const_iterator mitGuild = m_mapGuildSkillAcquired.begin();

		for ( mitGuild; mitGuild != m_mapGuildSkillAcquired.end(); ++mitGuild )
		{
			if( mitGuild->second.m_iSkillLevel.Verify() == false ||
				mitGuild->second.m_iSkillCSPoint.Verify() == false ||
				mitGuild->second.m_iIncreaseSkillLevelByBuff.Verify() == false ||
				mitGuild->second.m_iIncreaseSkillLevelBySocket.Verify() == false )
			{
				if( NULL != g_pData &&
					NULL != g_pData->GetMyUser() &&
					NULL != g_pData->GetServerProtocol() )
				{
					if( g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER &&
						g_pKTDXApp->GetFindHacking() == false )
					{
						g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
					}

					return;
				}
			}
		}
	}
}

/** @function	Reset
	@brief		�缳��, ���� ��ų(A, B), ��ų Ʈ��, ���� ��ų �ʱ�ȭ
	@param[in]	bResetSkillTree : ��ų Ʈ��
	@param[in]	bResetEquippedSkill : ���� ��ų
	@param[in]	bResetUnsealedSkill : ���� ������ ��ų
	@param[in]	bResetSkillSlotB : B ����
*/
void CX2UserSkillTree::Reset( bool bResetSkillTree, bool bResetEquippedSkill, bool bResetUnsealedSkill, bool bResetSkillSlotB )
{
	if( true == bResetEquippedSkill )
	{
		for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
		{
			m_aEquippedSkill[i].Init();
			m_aEquippedSkillSlotB[i].Init();
		}
	}

	if( true == bResetSkillTree )
	{
		if ( m_vecSkillDataMap.size() > 0 )
			m_vecSkillDataMap[m_usUsingPage].clear();
	}

	if( true == bResetUnsealedSkill )
	{
		m_setUnsealedSkillID.clear();
	}

	if( bResetSkillSlotB )
	{
		SetSkillSlotBExpirationState( SSBES_EXPIRED );
	}
}

/** @function	Reset
	@brief		���� �Լ� ��� ��ų �ʱ�ȭ �߰�
	@param[in]	bResetSkillTree : ��ų Ʈ��
	@param[in]	bResetGuildSkillTree : ��� ��ų Ʈ��
	@param[in]	bResetEquippedSkill : ���� ��ų
	@param[in]	bResetUnsealedSkill : ���� ������ ��ų
	@param[in]	bResetSkillSlotB : B ����
*/
//{{ oasis907 : ����� //// 2009-11-12 ////
void CX2UserSkillTree::Reset( bool bResetSkillTree, bool bResetGuildSkillTree, bool bResetEquippedSkill, bool bResetUnsealedSkill, bool bResetSkillSlotB )
{
	if( true == bResetEquippedSkill )
	{
		for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
		{
			m_aEquippedSkill[i].Init();
			m_aEquippedSkillSlotB[i].Init();
		}
	}

	if( true == bResetSkillTree )
	{
		BOOST_FOREACH( SkillDataMap& userSkillPageData, m_vecSkillDataMap )
		{
			userSkillPageData.clear();
		}

		m_vecSkillDataMap.clear();
	}

	if( true == bResetGuildSkillTree )
	{
		m_mapGuildSkillAcquired.clear();
	}

	if( true == bResetUnsealedSkill )
	{
		m_setUnsealedSkillID.clear();
	}

	if( bResetSkillSlotB )
	{
		SetSkillSlotBExpirationState( SSBES_EXPIRED );
	}
}
//}} oasis907 : ����� //// 2009-11-12 ////

/** @function	ConstructSkillTreeNotMyUnit
	@brief		����, ���� ���ӿ��� �� ĳ���Ͱ� �ƴ� �ٸ� ĳ���Ͱ� � passive skill�� ������� ������ �� ���Ǵ� �Լ�
	@param[in]	vecSkillData : ��ų ����
	@param[out]	aEquippedSkill[] : A Slot
	@param[out]	aEquippedSkillSlotB[] : B Slot
*/
void CX2UserSkillTree::ConstructSkillTreeNotMyUnit( const std::vector<KSkillData>& vecSkillData, const KSkillData aEquippedSkill[], const KSkillData aEquippedSkillSlotB[] )
{
	// ������ //
	// �߰� �� m_iIncreaseSkillLevelBySocket���� �ʱ�ȭ ��Ű�� �ʱ� ���� false�� ����
	BOOST_FOREACH( SkillDataMap& userSkillPageData, m_vecSkillDataMap )
	{
		userSkillPageData.clear();
	}
	m_vecSkillDataMap.clear();

	SkillDataMap EmptySkillData;
	m_vecSkillDataMap.push_back( EmptySkillData );

	Reset( false, false, false, false );

	for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT; i++ )
	{
		SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) aEquippedSkill[i].m_iSkillID, (int) aEquippedSkill[i].m_cSkillLevel, 0 );
	}

	for( int i=0; i<EQUIPPED_SKILL_SLOT_COUNT; i++ )
	{
		SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) aEquippedSkillSlotB[i].m_iSkillID, (int) aEquippedSkillSlotB[i].m_cSkillLevel, 0 );
	}

	BOOST_TEST_FOREACH( const KSkillData&, kSkillData, vecSkillData )
	{
		SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kSkillData.m_iSkillID, (int) kSkillData.m_cSkillLevel, 0 );
	}
}

//{{ oasis907 : ����� //// 2009-11-18 ////
/** @function	ConstructGuildSkillTreeNotMyUnit
	@brief		�����κ��� ���� ��� ��ų�� �����ϴ� �Լ�, ���߿� ���� �Լ��� ��ü X
				��� ��ų�� ������Ʈ�ϴ� �Լ� (���� ��� ������)
				����, �������ӿ��� �� ĳ���Ͱ� �ƴ� �ٸ� ĳ���Ͱ� � passive skill�� ������� ������ �� ���Ǵ� �Լ�
	@param[in]	vecGuildSkillData : ��� ��ų ����
*/
void CX2UserSkillTree::ConstructGuildSkillTreeNotMyUnit( const std::vector<KSkillData>& vecGuildSkillData )
{
	Reset( false, true, false, false, false );

	// ��� ��ų
	BOOST_TEST_FOREACH( const KSkillData&, kSkillData, vecGuildSkillData )
	{
		SetSkillLevelAndCSP( (CX2SkillTree::SKILL_ID) kSkillData.m_iSkillID, (int) kSkillData.m_cSkillLevel, 0 );
	}
}
//}} oasis907 : ����� //// 2009-11-18 ////

void CX2UserSkillTree::SetEquippedSkill( const int iSlotIndex_, const bool bSlotB_, const KSkillData& equippedSkillData_ )
{
	if ( bSlotB_ )
	{
		if ( iSlotIndex_ >= 0 && iSlotIndex_ < ARRAY_SIZE( m_aEquippedSkillSlotB ) )
		{
			m_aEquippedSkillSlotB[iSlotIndex_].Init();
			m_aEquippedSkillSlotB[iSlotIndex_].m_eID			= static_cast<CX2SkillTree::SKILL_ID>( equippedSkillData_.m_iSkillID );
			m_aEquippedSkillSlotB[iSlotIndex_].m_iSkillLevel	= static_cast<int>( equippedSkillData_.m_cSkillLevel );
		}
	}
	else
	{
		if ( iSlotIndex_ >= 0 && iSlotIndex_ < ARRAY_SIZE( m_aEquippedSkill ) )
		{
			m_aEquippedSkill[iSlotIndex_].Init();
			m_aEquippedSkill[iSlotIndex_].m_eID			= static_cast<CX2SkillTree::SKILL_ID>( equippedSkillData_.m_iSkillID );
			m_aEquippedSkill[iSlotIndex_].m_iSkillLevel	= static_cast<int>( equippedSkillData_.m_cSkillLevel );
		}
	}
}

/** @function	SetEquippedSkill
	@brief		���� ��ų ����
	@param[out]	aEquippedSkill[] : A Slot
	@param[out]	aEquippedSkillSlotB[] : B Slot
*/
void CX2UserSkillTree::SetEquippedSkill( const KSkillData aEquippedSkill[], const KSkillData aEquippedSkillSlotB[] )
{
	/// �ʱ�ȭ
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		m_aEquippedSkill[i].Init();
		m_aEquippedSkillSlotB[i].Init();
	}


	/// A ����(ID, ����)
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		m_aEquippedSkill[i].m_eID			= (CX2SkillTree::SKILL_ID) aEquippedSkill[i].m_iSkillID;
		m_aEquippedSkill[i].m_iSkillLevel	= (int) aEquippedSkill[i].m_cSkillLevel;
	}

	/// B ����(ID, ����)
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); i++ )
	{
		m_aEquippedSkillSlotB[i].m_eID			= (CX2SkillTree::SKILL_ID) aEquippedSkillSlotB[i].m_iSkillID;
		m_aEquippedSkillSlotB[i].m_iSkillLevel	= (int) aEquippedSkillSlotB[i].m_cSkillLevel;
	}
}

void CX2UserSkillTree::SetEquippedSkillLevelPlusBySlotIndex( const int iEquippedSkillLevelPlus_, const int iSlotIndex_, const bool bIsSlotB_ )
{
	if ( bIsSlotB_ )
	{
		if ( iSlotIndex_ >= 0 && iSlotIndex_ < ARRAY_SIZE( m_aEquippedSkillSlotB ) )
			m_aEquippedSkillSlotB[iSlotIndex_].m_iSkillLevelPlus = iEquippedSkillLevelPlus_;
	}
	else
	{
		if ( iSlotIndex_ >= 0 && iSlotIndex_ < ARRAY_SIZE( m_aEquippedSkill ) )
			m_aEquippedSkill[iSlotIndex_].m_iSkillLevelPlus = iEquippedSkillLevelPlus_;
	}
}

/** @function	SetEquippedSkillLevelPlus
	@brief		���� ��ų ���� ����(�����ۿ� ����)
	@param[in]	aEquippedSkillLevelPlus[] : A Slot ��ų ���� ���� ��
	@param[in]	aEquippedSkillLevelPlusSlotB[] : B Slot ��ų ���� ���� ��
*/
void CX2UserSkillTree::SetEquippedSkillLevelPlus( const int aEquippedSkillLevelPlus[], const int aEquippedSkillLevelPlusSlotB[] )
{
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		m_aEquippedSkill[i].m_iSkillLevelPlus	= aEquippedSkillLevelPlus[i];
	}

	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); ++i )
	{
		m_aEquippedSkillSlotB[i].m_iSkillLevelPlus	= aEquippedSkillLevelPlusSlotB[i];
	}
}


/** @function	SetAcquiredSkill
	@brief		����� ��ų ����
	@param[in]	��ų ����Ʈ
*/
void CX2UserSkillTree::SetAcquiredSkill( const std::vector<KUserSkillPageData>& vecSkillList )
{
	BOOST_FOREACH( SkillDataMap& userSkillPageData, m_vecSkillDataMap )
	{
		userSkillPageData.clear();
	}
	m_vecSkillDataMap.clear();

	for ( UINT i = 0; i < m_usOpenedPage; ++i )
	{
		SkillDataMap EmptySkillData;
		m_vecSkillDataMap.push_back( EmptySkillData );
	}

	int iPage = 0;

	BOOST_FOREACH( const KUserSkillPageData& userSkillPageData, vecSkillList )
	{
		BOOST_FOREACH( const KUserSkillData& userSkillData, userSkillPageData.m_vecUserSkillData )
		{
			if ( true == m_vecSkillDataMap.empty() || iPage >= m_vecSkillDataMap.size() )
				continue;

			SkillDataMap::iterator mit;
			mit = m_vecSkillDataMap[iPage].find( (CX2SkillTree::SKILL_ID) userSkillData.m_iSkillID );
			ASSERT( mit == m_vecSkillDataMap[iPage].end() );
			if( mit == m_vecSkillDataMap[iPage].end() )
			{
				// �̹� �����ϸ� ���Ե��� �ʰ� �������� ������ ���Եȴ�.
				m_vecSkillDataMap[iPage][ (CX2SkillTree::SKILL_ID)userSkillData.m_iSkillID ] = UserSkillData( (int)userSkillData.m_cSkillLevel, (int)userSkillData.m_cSkillCSPoint );
			}
		}
		m_arrSkillPoint[iPage] = userSkillPageData.m_usSkillPoint;
		m_arrCashSkillPoint[iPage] = userSkillPageData.m_usCashSkillPoint;
		++iPage;
	}
}

void CX2UserSkillTree::AddSkillPage()
{
	SkillDataMap EmptySkillData;
	m_vecSkillDataMap.push_back( EmptySkillData );
}

//{{ oasis907 : ����� //// 2009-11-12 ////
/** @function	SetAcquiredGuildSkill
	@brief		����� ��� ��ų ����
	@param[in]	��� ��ų ����Ʈ
*/
void CX2UserSkillTree::SetAcquiredGuildSkill( const std::vector<KGuildSkillData>& vecSkillList )
{
	m_mapGuildSkillAcquired.clear();
	BOOST_TEST_FOREACH( const KGuildSkillData&, userSkillData, vecSkillList )
	{
		SkillDataMap::iterator mit;
		mit = m_mapGuildSkillAcquired.find( (CX2SkillTree::SKILL_ID) userSkillData.m_iSkillID );
		ASSERT( mit == m_mapGuildSkillAcquired.end() );
		if( mit == m_mapGuildSkillAcquired.end() )
		{
			// �̹� �����ϸ� ���Ե��� �ʰ� �������� ������ ���Եȴ�.
			m_mapGuildSkillAcquired[ (CX2SkillTree::SKILL_ID)userSkillData.m_iSkillID ] = UserSkillData( (int)userSkillData.m_cSkillLevel, (int)userSkillData.m_cSkillCSPoint );
		}
	}
}
//}} oasis907 : ����� //// 2009-11-12 ////
/** @function	SetUnsealedSkill
	@brief		���� ������ ��ų ����
	@param[in]	���� ������ ��ų ����Ʈ
*/
void CX2UserSkillTree::SetUnsealedSkill( const std::vector<short>& vecUnsealedSkillList )
{
	//  ���������� ��ų ���
	m_setUnsealedSkillID.clear();
	for( UINT i=0; i<vecUnsealedSkillList.size(); i++ )
	{
		m_setUnsealedSkillID.insert( (CX2SkillTree::SKILL_ID) vecUnsealedSkillList[i] );
	}
}

/** @function	GetSkillLevel
	@brief		��ų ���� ���
	@param[in]	��ų ID, �߰��� ��ų ȿ��( ĳ�� ����Ʈ, ����, ���� ) ���� ����
	@return		��ų ����
*/
int CX2UserSkillTree::GetSkillLevel( CX2SkillTree::SKILL_ID eSkillID, bool bBaslcLevel /*= false*/, USHORT usPageNumber_ /*= 0*/ ) const
{
	KTDXPROFILE();

	//{{ oasis907 : ����� //// 2009-11-16 ////
	if(IsGuildSkill(eSkillID))
	{
		SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.find( eSkillID );
		if( mit != m_mapGuildSkillAcquired.end() )
		{
			const UserSkillData& userSkillData = mit->second;

			const int iSkillLevel = true == bBaslcLevel ? userSkillData.m_iSkillLevel : userSkillData.GetTotalSkillLevel();

			return min( iSkillLevel, MAX_LIMITED_SKILL_LEVEL );
		}
		return 0;
	}
	//}} oasis907 : ����� //// 2009-11-16 ////

	if( m_vecSkillDataMap.empty() == true )
		return 0;

	USHORT usSkillPage = m_usUsingPage;

	if ( usPageNumber_ > 0 )
		usSkillPage = usPageNumber_ - 1;

	if( m_vecSkillDataMap.size() <= usSkillPage )
		usSkillPage = max( 0, m_vecSkillDataMap.size() - 1 );

	SkillDataMap::const_iterator mit = m_vecSkillDataMap[usSkillPage].find( eSkillID );

	if( mit != m_vecSkillDataMap[usSkillPage].end() )
	{
		const UserSkillData& userSkillData = mit->second;
		const int iSkillLevel = true == bBaslcLevel ? userSkillData.m_iSkillLevel : userSkillData.GetTotalSkillLevel();

		return min( iSkillLevel, MAX_LIMITED_SKILL_LEVEL );
	}
	return 0;
}

//{{ oasis907 : ����� //// 2009-11-18 ////
/** @function	GetUserSkillTemplet
	@brief		���� ��ų ���ø� ���
	@param[in]	��ų ID
	@return		��ų ���ø�
*/
const CX2SkillTree::SkillTemplet* CX2UserSkillTree::GetUserSkillTemplet( CX2SkillTree::SKILL_ID eSkillID) const
{
	KTDXPROFILE();

	//// passive skill ���� ����ؾ� �Ѵ�. active skill�� getequippedactiveskill()�Լ��� �����
	//ASSERT( NULL == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 ) ||
	//	CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 )->m_eType ||
	//	CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 )->m_eType ||
	//	CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 )->m_eType ||
	//	CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 )->m_eType ||
	//	CX2SkillTree::ST_PASSIVE == g_pData->GetSkillTree()->GetSkillTemplet( eSkillID, 1 )->m_eType );


	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return NULL;

	SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].find( eSkillID );

	if( mit == m_vecSkillDataMap[m_usUsingPage].end() )
	{
		return NULL;
	}

	const UserSkillData& userSkillData = mit->second;
	/////////////////////////////////////////////
	return g_pData->GetSkillTree()->GetSkillTemplet(eSkillID);
}
//}} oasis907 : ����� //// 2009-11-18 ////

//{{ oasis907 : ����� //// 2009-11-18 ////
/** @function	GetSkillLevel
	@brief		��� ��ų ���� ���
	@param[out]	int ��� ��ų ����
*/
int CX2UserSkillTree::GetSkillLevel( CX2SkillTree::SKILL_ID eSkillID, int iGuildMemberGrade) const
{
	KTDXPROFILE();
	if(iGuildMemberGrade == CX2GuildManager::GUG_INVALID ||
		iGuildMemberGrade >= g_pData->GetGuildManager()->GetLimitGuildGrade()) // ������ ĿƮ���� ��� ��� ���
	{
		return 0;      // ����� ���� ��� ��ų ���� �ȵɶ�
	}

	SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.find( eSkillID );
	if( mit != m_mapGuildSkillAcquired.end() )
	{
		const UserSkillData& userSkillData = mit->second;
		return userSkillData.m_iSkillLevel;
	}

	return 0;
}
//}} oasis907 : ����� //// 2009-11-18 ////

//{{ oasis907 : ����� //// 2009-11-18 ////
/** @function	GetUserSkillTemplet
	@brief		��� ���� ��ų ���ø� ���
	@param[in]	��ų ID
	@param[in]	��� ��� ���
	@return		��ų ���ø�
*/
const CX2SkillTree::SkillTemplet* CX2UserSkillTree::GetUserSkillTemplet( CX2SkillTree::SKILL_ID eSkillID, int iGuildMemberGrade) const
{
	KTDXPROFILE();

	if(iGuildMemberGrade == CX2GuildManager::GUG_INVALID ||
		iGuildMemberGrade >= g_pData->GetGuildManager()->GetLimitGuildGrade())  // ������ ĿƮ���� ��� ��� ���
	{
		return NULL;	// ����� ���� ��� ��ų ���� �ȵɶ�
	}

	SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.find( eSkillID );

	if( mit == m_mapGuildSkillAcquired.end() )
	{
		return NULL;
	}

	const UserSkillData& userSkillData = mit->second;
	/////////////////////////////////////////////

	return g_pData->GetSkillTree()->GetSkillTemplet(eSkillID );
}
//}} oasis907 : ����� //// 2009-11-18 ////

/** @function	GetSkillLevelAndCSP
	@brief		��ų ����, ���� ����Ʈ ���
	@param[in]	��ų ID
	@param[out]	��ų ����
	@param[out]	��ų Spell Point
*/
bool CX2UserSkillTree::GetSkillLevelAndCSP( IN CX2SkillTree::SKILL_ID eSkillID, OUT int& iSkillLevel, OUT int& iSkillCSPoint ) const
{
	//{{ oasis907 : ����� //// 2009-11-16 ////
	if(IsGuildSkill(eSkillID))
	{
		SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.find( eSkillID );
		if( mit != m_mapGuildSkillAcquired.end() )
		{
			const UserSkillData& userSkillData = mit->second;
			iSkillLevel		= userSkillData.m_iSkillLevel;
			iSkillCSPoint	= userSkillData.m_iSkillCSPoint;
			return true;
		}
		return false;
	}
	//}} oasis907 : ����� //// 2009-11-16 ////

	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return false;

	SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].find( eSkillID );

	if( mit == m_vecSkillDataMap[m_usUsingPage].end() )
	{
		const UserSkillData& userSkillData = mit->second;
		iSkillLevel		= userSkillData.m_iSkillLevel;
		iSkillCSPoint	= userSkillData.m_iSkillCSPoint;
		return true;
	}

	return false;
}


/** @function	SetSkillLevelAndCSP
	@brief		�����Ͱ� ������ ���� �����ϰ�, ������ �߰��Ѵ�, skill level�� 0���� ������ ����
	@param[in]	��ų ID
	@param[in]	��ų ����
	@param[in]	��ų Spell Point
	@return		false : ���� ����
	@return		true : ���� �Ϸ�
*/
bool CX2UserSkillTree::SetSkillLevelAndCSP( CX2SkillTree::SKILL_ID eSkillID, int iSkillLevel, int iSkillCSPoint, USHORT usSkillPage_ )
{
	//{{ oasis907 : ����� //// 2009-11-16 ////
	if(IsGuildSkill(eSkillID))
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
		if( NULL == pSkillTemplet )
			return false;

		if( iSkillLevel > g_pData->GetSkillTree()->GetMaxGuildSkillLevel( (int)m_iGuildClass, eSkillID ) )
			return false;



		SkillDataMap::iterator mit;
		mit = m_mapGuildSkillAcquired.find( eSkillID );
		if( mit != m_mapGuildSkillAcquired.end() )
		{
			if( iSkillLevel > 0 )
			{
				UserSkillData& userSkillData = mit->second;
				userSkillData.m_iSkillLevel		= iSkillLevel;
				userSkillData.m_iSkillCSPoint	= iSkillCSPoint;
			}
			else
			{
				m_mapGuildSkillAcquired.erase( mit );
			}
		}
		else
		{
			if( iSkillLevel > 0 )
			{
				m_mapGuildSkillAcquired[ eSkillID ] = UserSkillData( iSkillLevel, iSkillCSPoint );
			}
		}
		return true;

	}
	//}} oasis907 : ����� //// 2009-11-16 ////

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
		return false;

	if( iSkillLevel > g_pData->GetSkillTree()->GetMaxSkillLevel( (int)m_iUnitClass, eSkillID ) )
		return false;



	SkillDataMap::iterator mit;

	if ( m_vecSkillDataMap.size() <= usSkillPage_ )
		return false;

	USHORT usPageNumber = m_usUsingPage;

	if ( usSkillPage_ > 0 )
		usPageNumber = usSkillPage_;

	mit = m_vecSkillDataMap[usPageNumber].find( eSkillID );
	if( mit != m_vecSkillDataMap[usPageNumber].end() )
	{
		if( iSkillLevel > 0 )
		{
			UserSkillData& userSkillData = mit->second;
			userSkillData.m_iSkillLevel		= iSkillLevel;
			userSkillData.m_iSkillCSPoint	= iSkillCSPoint;
		}
		else
		{
			m_vecSkillDataMap[usPageNumber].erase( mit );
		}
	}
	else
	{
		if( iSkillLevel > 0 )
		{
			m_vecSkillDataMap[usPageNumber][ eSkillID ] = UserSkillData( iSkillLevel, iSkillCSPoint );
		}
	}
	return true;
}

/** @function	IsMaxSkillLevel
	@brief		��ų ���� Ȯ��(�ְ�ġ)
	@param[in]	��ų ID
	@return		�ְ�ġ ���� �ƴ��� ����
*/
bool CX2UserSkillTree::IsMaxSkillLevel( IN CX2SkillTree::SKILL_ID eSkillID ) const
{
	//{{ oasis907 : ����� //// 2009-11-16 ////
	if(IsGuildSkill(eSkillID))
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
		if( NULL == pSkillTemplet )
			return false;

		SkillDataMap::const_iterator mit;
		mit = m_mapGuildSkillAcquired.find( eSkillID );
		if( mit != m_mapGuildSkillAcquired.end() )
		{
			const UserSkillData& userSkillData = mit->second;
			if( userSkillData.m_iSkillLevel >= g_pData->GetSkillTree()->GetMaxGuildSkillLevel( (int)m_iGuildClass, eSkillID ) )
				return true;
			else
				return false;
		}

		return false;
	}
	//}} oasis907 : ����� //// 2009-11-16 ////

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );
	if( NULL == pSkillTemplet )
		return false;

	SkillDataMap::const_iterator mit;
	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return false;

	mit = m_vecSkillDataMap[m_usUsingPage].find( eSkillID );
	if( mit != m_vecSkillDataMap[m_usUsingPage].end() )
	{
		const UserSkillData& userSkillData = mit->second;
		if( userSkillData.m_iSkillLevel >= g_pData->GetSkillTree()->GetMaxSkillLevel( (int)m_iUnitClass, eSkillID ) )
			return true;
		else
			return false;
	}

	return false;
}

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
/** @function	SetSkillSlotInfo
	@brief		��ų ���� ���� ����(ĳ���� �߰��� ���� ����)
	@param[in]	��ų ���� ID
	@param[in]	��ų ID
*/
void CX2UserSkillTree::SetSkillSlotInfo( int iSkillSlotID, CX2SkillTree::SKILL_ID eSkillID )
{
	// ĳ���� �߰�
	switch( iSkillSlotID )
	{
	case CX2UserSkillTree::SKILL_SLOT_A1:
	case CX2UserSkillTree::SKILL_SLOT_A2:
	case CX2UserSkillTree::SKILL_SLOT_A3:
	case CX2UserSkillTree::SKILL_SLOT_A4:
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUserSkillTemplet( eSkillID);
			if(pSkillTemplet != NULL)
			{

				if ( NULL == g_pData ||
					 NULL == g_pData->GetMyUser() ||
					 NULL == g_pData->GetMyUser()->GetSelectUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				SetSkillSlot( iSkillSlotID, eSkillID, pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel ), pSkillTemplet->m_usCBConsumption,
					pSkillTemplet->m_usFPConsumtion, false );

			}
			else
			{
				SetSkillSlot( iSkillSlotID, eSkillID, 0, 0, 0, false );
			}

		} break;

	case CX2UserSkillTree::SKILL_SLOT_B1:
	case CX2UserSkillTree::SKILL_SLOT_B2:
	case CX2UserSkillTree::SKILL_SLOT_B3:
	case CX2UserSkillTree::SKILL_SLOT_B4:
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUserSkillTemplet( eSkillID);
			if(pSkillTemplet != NULL)
			{

				if ( NULL == g_pData ||
					NULL == g_pData->GetMyUser() ||
					NULL == g_pData->GetMyUser()->GetSelectUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				SetSkillSlot( iSkillSlotID-(int)CX2UserSkillTree::SKILL_SLOT_B1, eSkillID, pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel ),
					pSkillTemplet->m_usCBConsumption, pSkillTemplet->m_usFPConsumtion, true );

			}
			else
			{
				SetSkillSlot( iSkillSlotID-(int)CX2UserSkillTree::SKILL_SLOT_B1, eSkillID, 0, 0, 0, true );
			}
		} break;
	}
}

//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG

/** @function : GetCBConsumption
	@brief : ���� �ε����� ����A,B �� ���� CB�Ҹ��� ����
	@param : const int iSlotIndex_, const bool bSlotB_
	@return : USHORT (CB �Ҹ�)
*/
USHORT CX2UserSkillTree::GetCBConsumption( const int iSlotIndex_, const bool bSlotB_ ) const
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return 0;

	if( true == bSlotB_ )
	{
		return m_aEquippedSkillSlotB[iSlotIndex_].m_usCBConsumption;
	}
	else
	{
		return m_aEquippedSkill[iSlotIndex_].m_usCBConsumption;
	}
}

/** @function : SetCBConsumption
	@brief : ���� ���Կ� �����Ǿ��ִ� CB �Ҹ��� Set��
	@param : int iSlotIndex_, bool bSlotB_, int iCBConsumption_
*/
void CX2UserSkillTree::SetCBConsumption( int iSlotIndex_, bool bSlotB_, USHORT usCBConsumption_ )
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return;

	if( true == bSlotB_ )
	{
		m_aEquippedSkillSlotB[iSlotIndex_].m_usCBConsumption = usCBConsumption_;
	}
	else
	{
		m_aEquippedSkill[iSlotIndex_].m_usCBConsumption = usCBConsumption_;
	}
}

//#pragma region ForcePower
/** @function 	: GetFPConsumption
	@brief 		: �ش� ������ ��� ��뷮 Ȯ��
*/
USHORT CX2UserSkillTree::GetFPConsumption( const int iSlotIndex_, const bool bSlotB_ ) const
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return 0;

	if( true == bSlotB_ )
	{
		return m_aEquippedSkillSlotB[iSlotIndex_].m_usFPConsumption;
	}
	else
	{
		return m_aEquippedSkill[iSlotIndex_].m_usFPConsumption;
	}
}

/** @function 	: SetFPConsumption
	@brief 		: �ش� ������ ��� ��뷮 ����
*/
void CX2UserSkillTree::SetFPConsumption( int iSlotIndex_, bool bSlotB_, USHORT usFPConsumption_ )
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return;

	if( true == bSlotB_ )
	{
		m_aEquippedSkillSlotB[iSlotIndex_].m_usFPConsumption = usFPConsumption_;
	}
	else
	{
		m_aEquippedSkill[iSlotIndex_].m_usFPConsumption = usFPConsumption_;
	}
}

/** @function 	: GetFPConsumption
	@brief 		: �ش� ������ ��� ��뷮 Ȯ��
*/
USHORT CX2UserSkillTree::GetFPGainCount( const int iSlotIndex_, const bool bSlotB_ ) const
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return 0;

	if( true == bSlotB_ )
	{
		return m_aEquippedSkillSlotB[iSlotIndex_].m_usFPGainCount;
	}
	else
	{
		return m_aEquippedSkill[iSlotIndex_].m_usFPGainCount;
	}
}

/** @function 	: SetFPConsumption
	@brief 		: �ش� ������ ��� ��뷮 ����
*/
void CX2UserSkillTree::SetFPGainCount( int iSlotIndex_, bool bSlotB_, USHORT usFPGainCount_ )
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return;

	if( true == bSlotB_ )
	{
		m_aEquippedSkillSlotB[iSlotIndex_].m_usFPGainCount = usFPGainCount_;
	}
	else
	{
		m_aEquippedSkill[iSlotIndex_].m_usFPGainCount = usFPGainCount_;
	}
}
//#pragma endregion ���


/** @function	SetSkillSlot
	@brief		��ų ���� ����
	@param[in]	���� �ε���
	@param[in]	��ų ID
	@param[in]	�Ҹ� ����
	@param[in]	�Ҹ� ĳ����
	@param[in]	�Ҹ� ���
	@param[in]	B ���� ���� �ƴ���
*/
bool CX2UserSkillTree::SetSkillSlot( int iSlotIndex_, CX2SkillTree::SKILL_ID eSkillID_, float fMPConsume_, USHORT usCBConsume_,
									 USHORT usFPConsume, bool bSlotB_ )
{
	if( iSlotIndex_ < 0 ||
		iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) - 1 )
	{
		return false;
	}

	//if( true == bSlotB &&
	//	false == GetEnabledSkillSlotB() )
	//{
	//	return false;
	//}

	if( true == bSlotB_ )
	{
		m_aEquippedSkillSlotB[iSlotIndex_].m_eID = eSkillID_;
		m_aEquippedSkillSlotB[iSlotIndex_].m_fCoolTimeLeft = 0.f;
		m_aEquippedSkillSlotB[iSlotIndex_].m_fMPConsumption = fMPConsume_;
		m_aEquippedSkillSlotB[iSlotIndex_].m_usCBConsumption = usCBConsume_;
		m_aEquippedSkillSlotB[iSlotIndex_].m_usFPConsumption = usFPConsume;
	}
	else
	{
		m_aEquippedSkill[iSlotIndex_].m_eID = eSkillID_;
		m_aEquippedSkill[iSlotIndex_].m_fCoolTimeLeft = 0.f;
		m_aEquippedSkill[iSlotIndex_].m_fMPConsumption = fMPConsume_;
		m_aEquippedSkill[iSlotIndex_].m_usCBConsumption = usCBConsume_;
		m_aEquippedSkillSlotB[iSlotIndex_].m_usFPConsumption = usFPConsume;
	}

	return true;
}


//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

/** @function	GetSkillSlot
	@brief		��ų ���� ���
	@param[in]	���� �ε���
	@param[in]	B �������� �ƴ���
*/
const CX2UserSkillTree::SkillSlotData* CX2UserSkillTree::GetSkillSlot( int iSlotIndex, bool bSlotB ) const
{
	if( iSlotIndex < 0 ||
		iSlotIndex > ARRAY_SIZE( m_aEquippedSkill ) -1 )
	{
		return NULL;
	}

	if( iSlotIndex >= 4 )
	{
		bSlotB = true;
		iSlotIndex -= 4;
	}

	if( true == bSlotB )
	{
		return &m_aEquippedSkillSlotB[iSlotIndex];
	}
	else
	{
		return &m_aEquippedSkill[iSlotIndex];
	}
}

/** @function : GetSkillSlot
	@brief : ��ų���Ե������� �����͸� ��� �Լ�
	@param : ��ųID(eSkillID_)
	@return : ��ų���Ե����� ������
*/
const CX2UserSkillTree::SkillSlotData* CX2UserSkillTree::GetSkillSlot( const CX2SkillTree::SKILL_ID eSkillID_ ) const
{
	const UINT uiArraySize = ARRAY_SIZE( m_aEquippedSkill );

	for ( UINT uiIndex = 0; uiIndex < uiArraySize; ++uiIndex )
	{
		if ( eSkillID_ == m_aEquippedSkill[uiIndex].m_eID )
			return &m_aEquippedSkill[uiIndex];
		else if ( eSkillID_ == m_aEquippedSkillSlotB[uiIndex].m_eID )
			return &m_aEquippedSkillSlotB[uiIndex];
	}

	return NULL;
}

/** @function : ChangeMpConsumptionPercent
	@brief : Mp �Ҹ��� ����
	@param : Mp �Ҹ����� ������ ��ųID, ���� ����(fMultiplier)
*/
void CX2UserSkillTree::ChangeMpConsumptionPercent( const CX2SkillTree::SKILL_ID eSkillID_, const float fMultiplier_ )
{
	const UINT uiArraySize = ARRAY_SIZE( m_aEquippedSkill );

	for ( UINT uiIndex = 0; uiIndex < uiArraySize; ++uiIndex )
	{
		if ( eSkillID_ == m_aEquippedSkill[uiIndex].m_eID )
		{
			m_aEquippedSkill[uiIndex].m_fMPConsumption *= fMultiplier_;
			break;
		}
		else if ( eSkillID_ == m_aEquippedSkillSlotB[uiIndex].m_eID )
		{
			m_aEquippedSkillSlotB[uiIndex].m_fMPConsumption *= fMultiplier_;
			break;
		}
	}
}

/** @function	SetMpConsumption
	@brief		���� �Ҹ� ����
	@param[in]	���� �ε���
	@param[in]	B ����?
	@param[in]	���� �Ҹ�
*/
void CX2UserSkillTree::SetMpConsumption( int iSlotIndex_, bool bSlotB_, float fMpConsumption_ )
{
	if( iSlotIndex_ < 0 || iSlotIndex_ > ARRAY_SIZE( m_aEquippedSkill ) -1 )
		return;

	if( true == bSlotB_ )
	{
		m_aEquippedSkillSlotB[iSlotIndex_].m_fMPConsumption = fMpConsumption_;
	}
	else
	{
		m_aEquippedSkill[iSlotIndex_].m_fMPConsumption = fMpConsumption_;
	}
}

/** @function	GetSkillStat
	@brief		��ų Stat ���
	@return		CX2Stat::Stat
*/
CX2Stat::Stat CX2UserSkillTree::GetSkillStat() const
{
	CX2Stat::Stat retStat;

	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return retStat;

	BOOST_TEST_FOREACH( const SkillDataMap::value_type&, value, m_vecSkillDataMap[m_usUsingPage] )
	{
		const UserSkillData& userSkillData = value.second;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( value.first );

		if( pSkillTemplet == NULL )
			continue;

		switch( pSkillTemplet->m_eType )
		{
		case CX2SkillTree::ST_PASSIVE_PHYSIC_ATTACK:
		case CX2SkillTree::ST_PASSIVE_MAGIC_ATTACK:
		case CX2SkillTree::ST_PASSIVE_MAGIC_DEFENCE:
		case CX2SkillTree::ST_PASSIVE_PHYSIC_DEFENCE:
			{
				retStat.AddStat( pSkillTemplet->GetSkillStatValue( userSkillData.m_iSkillLevel ), false );
			} break;
		}
	}

	return retStat;
}

//{{ oasis907 : ����� //// 2009-11-12 ////
/** @function	GetGuildSkillStat
	@brief		��� ��ų ���� ���
	@return		CX2Stat:Stat
*/
CX2Stat::Stat CX2UserSkillTree::GetGuildSkillStat(int iGuildMemberGrade) const
{
	CX2Stat::Stat retStat;

	// oasis907 : ����� [2009.12.8] // ��� ���
	if(iGuildMemberGrade == CX2GuildManager::GUG_INVALID ||
		iGuildMemberGrade >= g_pData->GetGuildManager()->GetLimitGuildGrade()) // ������ ĿƮ���� ��� ��� ���
	{
		return retStat;
	}

	BOOST_TEST_FOREACH( const SkillDataMap::value_type&, value, m_mapGuildSkillAcquired )
	{
		const UserSkillData& userSkillData = value.second;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( value.first );

		if( pSkillTemplet == NULL )
			continue;

		switch( pSkillTemplet->m_eType )
		{
		case CX2SkillTree::ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE:
		case CX2SkillTree::ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE:
			{
				retStat.AddStat( pSkillTemplet->GetSkillStatValue( userSkillData.m_iSkillLevel ), false );
			} break;
		}
	}

	return retStat;
}
//}} oasis907 : ����� //// 2009-11-12 ////

/** @function	CalcUsedSPointAndCSPoint
	@brief		���� Skill Point�� ĳ�� ��ų ����Ʈ �� ���
	@param[out]	iSPont : ��ų ����Ʈ
	@param[out]	iCSPoint : ĳ�� ��ų ����Ʈ
*/
void CX2UserSkillTree::CalcUsedSPointAndCSPoint( OUT int& iSPoint, OUT int& iCSPoint ) const
{
	if ( NULL == g_pData || NULL == g_pData->GetSkillTree() )
		return;

	iSPoint		= 0;
	iCSPoint	= 0;

	if ( m_vecSkillDataMap.size() < m_usUsingPage )
		return;

	for( SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].begin(); mit != m_vecSkillDataMap[m_usUsingPage].end(); ++mit )
	{
		/// ��ȥ ��ų�̶��, ��ų ����Ʈ ���� ����
		CX2SkillTree::SKILL_ID eSkillID = mit->first;

		if ( CX2SkillTree::SI_ETC_WS_COMMON_LOVE == eSkillID )
			continue;

		/// ��ų ���ø� ��ȯ
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

		if ( NULL != pSkillTemplet )
		{
			const UserSkillData& userSkillData = mit->second;

			/// ����� ��ų ����Ʈ ���� ( ĳ�� ��ų ����Ʈ�� ���� )
			for( int i = 0 ; i < userSkillData.m_iSkillLevel ; ++i )
			{
				if( i == 0 )	/// ������ 1�� ��
				{
					if( true == g_pData->GetSkillTree()->isDefaultSkill( eSkillID ) )	/// �⺻ ��ų�̸� ����
						continue;

					iSPoint += pSkillTemplet->m_iRequireLearnSkillPoint;	/// ���� ����Ʈ�� ����
				}
				else
				{
					iSPoint += pSkillTemplet->m_iRequireUpgradeSkillPoint;	/// ��ȭ ����Ʈ�� ����
				}
			}

			iCSPoint	+= userSkillData.m_iSkillCSPoint;	/// ĳ�� ��ų ����Ʈ�� ����
			iSPoint		-= userSkillData.m_iSkillCSPoint;	/// ����� ĳ�� ��ų ����Ʈ�� ���ܽ���, �Ϲ� ��ų ����Ʈ�� ����

		}
	}

}

/** @function	CalcCumulativeUsedSPointOnEachTier
	@brief		tier���� �Ҹ�� ���� SP+CSP�� ������ش�
	@param[out]	vecTierSPoint :
*/
void CX2UserSkillTree::CalcCumulativeUsedSPointOnEachTier( OUT std::vector<int>& vecTierSPoint ) const
{

	int iMaxTierIndex = 0;
	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return;

	for( SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].begin(); mit != m_vecSkillDataMap[m_usUsingPage].end(); ++mit )
	{
		const UserSkillData& userSkillData = mit->second;
		if( userSkillData.m_iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( (int)m_iUnitClass, mit->first );
			if( NULL != pSkillTreeTemplet )
			{
				if( iMaxTierIndex < pSkillTreeTemplet->m_iTier )
					iMaxTierIndex = pSkillTreeTemplet->m_iTier;
			}
		}
	}

	vecTierSPoint.resize(0);
	for( int i=0; i<= iMaxTierIndex; i++ )
	{
		vecTierSPoint.push_back( 0 );
	}



	for( SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].begin(); mit != m_vecSkillDataMap[m_usUsingPage].end(); ++mit )
	{
		const UserSkillData& userSkillData = mit->second;
		if( userSkillData.m_iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( (int)m_iUnitClass, mit->first );
			if( NULL != pSkillTreeTemplet )
			{
				vecTierSPoint[ pSkillTreeTemplet->m_iTier ] += userSkillData.m_iSkillLevel;

//{{ kimhc // 2010.8.17 // ��ų �ǵ����Ⱑ �Ǹ� �ȵǴ� ��Ȳ���� �ǵ����Ⱑ �Ǿ� ���������� ��ųƮ���� �Ǵ� ������ �ذ��� ���Դϴ�.
				vecTierSPoint[ pSkillTreeTemplet->m_iTier ] -= userSkillData.m_iSkillCSPoint;
//}} kimhc // 2010.8.17 // ��ų �ǵ����Ⱑ �Ǹ� �ȵǴ� ��Ȳ���� �ǵ����Ⱑ �Ǿ� ���������� ��ųƮ���� �Ǵ� ������ �ذ��� ���Դϴ�.
			}
		}
	}


	int iCumulativeTierSPoint = 0;
	BOOST_TEST_FOREACH( int&, iTierSPoint, vecTierSPoint )
	{
		iCumulativeTierSPoint += iTierSPoint;
		iTierSPoint = iCumulativeTierSPoint;
	}
}


//{{ oasis907 : ����� //// 2009-11-12 ////
void CX2UserSkillTree::CalcUsedGuildSPointAndCSPoint( OUT int& iSPoint, OUT int& iCSPoint ) const
{
	iSPoint = 0;
	iCSPoint = 0;

	for( SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.begin(); mit != m_mapGuildSkillAcquired.end(); ++mit )
	{
		const UserSkillData& userSkillData = mit->second;

		if( userSkillData.m_iSkillLevel > 0 )
		{
			iSPoint		+= userSkillData.m_iSkillLevel - userSkillData.m_iSkillCSPoint;
			iCSPoint	+= userSkillData.m_iSkillCSPoint;
		}

	}
}



// tier���� �Ҹ�� ���� SP+CSP�� ������ش�
void CX2UserSkillTree::CalcCumulativeUsedGuildSPointOnEachTier( OUT std::vector<int>& vecTierSPoint ) const
{

	int iMaxTierIndex = 0;
	for( SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.begin(); mit != m_mapGuildSkillAcquired.end(); ++mit )
	{
		const UserSkillData& userSkillData = mit->second;
		if( userSkillData.m_iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetGuildSkillTreeTemplet( (int)m_iGuildClass, mit->first );
			if( NULL != pSkillTreeTemplet )
			{
				if( iMaxTierIndex < pSkillTreeTemplet->m_iTier )
					iMaxTierIndex = pSkillTreeTemplet->m_iTier;
			}
		}
	}

	vecTierSPoint.resize(0);
	for( int i=0; i<= iMaxTierIndex; i++ )
	{
		vecTierSPoint.push_back( 0 );
	}



	for( SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.begin(); mit != m_mapGuildSkillAcquired.end(); ++mit )
	{
		const UserSkillData& userSkillData = mit->second;
		if( userSkillData.m_iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetGuildSkillTreeTemplet( (int)m_iGuildClass, mit->first );
			if( NULL != pSkillTreeTemplet )
			{
				vecTierSPoint[ pSkillTreeTemplet->m_iTier ] += userSkillData.m_iSkillLevel;
				//{{ kimhc // 2010.8.17 // ��ų �ǵ����Ⱑ �Ǹ� �ȵǴ� ��Ȳ���� �ǵ����Ⱑ �Ǿ� ���������� ��ųƮ���� �Ǵ� ������ �ذ��� ���Դϴ�.
				vecTierSPoint[ pSkillTreeTemplet->m_iTier ] -= userSkillData.m_iSkillCSPoint;
				//}} kimhc // 2010.8.17 // ��ų �ǵ����Ⱑ �Ǹ� �ȵǴ� ��Ȳ���� �ǵ����Ⱑ �Ǿ� ���������� ��ųƮ���� �Ǵ� ������ �ذ��� ���Դϴ�.
			}
		}
	}


	int iCumulativeTierSPoint = 0;
	BOOST_TEST_FOREACH( int&, iTierSPoint, vecTierSPoint )
	{
		iCumulativeTierSPoint += iTierSPoint;
		iTierSPoint = iCumulativeTierSPoint;
	}
}
//}} oasis907 : ����� //// 2009-11-12 ////


// cash skill point�� ���� ��ų�� ������ �����Ѵ�
void CX2UserSkillTree::ExpireCashSkillPoint( OUT const std::map<int, int> mapHaveSKill_ )
{

	/// ��� ��ų ��� �ʱ�ȭ
	Reset( true, false, false, false );

	/// �׳�ý� ���� ������ ����� ��ų��� ����
	std::map<int, int>::const_iterator it = mapHaveSKill_.begin();

	for ( it; it != mapHaveSKill_.end(); ++it  )
	{
		const CX2SkillTree:: SKILL_ID	eSkillID	= static_cast<CX2SkillTree::SKILL_ID>( it->first );
		const int						eSkillLevel	= it->second;

		SetSkillLevelAndCSP( eSkillID, eSkillLevel, 0 );
	}



	// ������ ��ų �߿��� skill level�� 0������ ��ų�� ������ Ż����Ų��.
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		SkillSlotData& skillSlotData = m_aEquippedSkill[i];
		if( false == DoIHaveThisSkill( skillSlotData.m_eID ) )
		{
//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
			SetSkillSlot( i, CX2SkillTree::SI_NONE, 0, 0, 0, false );
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
		}
	}


	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); i++ )
	{
		SkillSlotData& skillSlotData = m_aEquippedSkillSlotB[i];
		if( false == DoIHaveThisSkill( skillSlotData.m_eID ) )
		{
			//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
			SetSkillSlot( i, CX2SkillTree::SI_NONE, 0, 0, 0, true );
			//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
		}
	}
}
//{{ oasis907 : ����� [2009.11.27] //
// cash skill point�� ���� ��ų�� ������ �����Ѵ�
void CX2UserSkillTree::ExpireGuildCashSkillPoint()
{
	for( SkillDataMap::iterator mit = m_mapGuildSkillAcquired.begin(); mit != m_mapGuildSkillAcquired.end(); ++mit )
	{
		UserSkillData& userSkillData = mit->second;

		if( userSkillData.m_iSkillCSPoint > 0 )
		{
			userSkillData.m_iSkillLevel		-= userSkillData.m_iSkillCSPoint;
			userSkillData.m_iSkillCSPoint	= 0;
		}
	}
    // oasis907 : ����� [2009.11.27] // ��� ��ų�� ��Ƽ�갡 �����Ƿ� �ʿ����
/*
	// ������ ��ų �߿��� skill level�� 0������ ��ų�� ������ Ż����Ų��.
	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); i++ )
	{
		SkillSlotData& skillSlotData = m_aEquippedSkill[i];
		if( false == DoIHaveThisSkill( skillSlotData.m_eID ) )
		{
			SetSkillSlot( i, CX2SkillTree::SI_NONE, 0, false );
		}
	}


	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); i++ )
	{
		SkillSlotData& skillSlotData = m_aEquippedSkillSlotB[i];
		if( false == DoIHaveThisSkill( skillSlotData.m_eID ) )
		{
			SetSkillSlot( i, CX2SkillTree::SI_NONE, 0, false );
		}
	}
*/
}




//}} oasis907 : ����� [2009.11.27] //

bool CX2UserSkillTree::IsSkillUnsealed( CX2SkillTree::SKILL_ID eSkillID ) const
{
	std::set< CX2SkillTree::SKILL_ID >::const_iterator it = m_setUnsealedSkillID.find( eSkillID );
	if( it != m_setUnsealedSkillID.end() )
	{
		return true;
	}

	return false;
}



void CX2UserSkillTree::AddSkillUnsealed( CX2SkillTree::SKILL_ID eSkillID )
{
	if( true == IsSkillUnsealed( eSkillID ) )
		return;

	m_setUnsealedSkillID.insert( eSkillID );
}


bool CX2UserSkillTree::DoIHaveThisSkill( CX2SkillTree::SKILL_ID eSkillID ) const
{

	/// ���� ��ų ����Ʈ�� ���ڵ� ��ų���� �˻�
	if( GetSkillLevel(eSkillID) > 0 )
		return true;
	/// �ƴ϶��, ��ųâ���� ��ų ������ �ø��� �ִ� ��ų���� �˻�
	else if (	NULL != g_pData &&
				NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetSkillTree() &&
				NULL != g_pData->GetUIManager()->GetUISkillTree() &&
				true == g_pData->GetUIManager()->GetUISkillTree()->GetNowLearnSkill() )
	{
		/// �ø� ��Ͽ� �ش� ��ų�� ������, ���������� ó��
		const std::map<int, KGetSkillInfo> mapSkillInfo = g_pData->GetSkillTree()->GetMapSkillInfo();

		const std::map<int, KGetSkillInfo>::const_iterator it = mapSkillInfo.find( static_cast<int>( eSkillID ) );

		if ( it != mapSkillInfo.end() )
			return true;
	}

	return false;
}


bool CX2UserSkillTree::CanIResetThisSkill( CX2SkillTree::SKILL_ID eSkillID ) const
{
//{{ oasis907 : ����� //// 2009-11-17 ////
	if(IsGuildSkill(eSkillID))
	{
		if( true == g_pData->GetSkillTree()->IsUnitTypeDefaultSkill( eSkillID ) && GetSkillLevel( eSkillID, true ) <= 1)	// �¿� ���� : 1���������� ���� �� �ְ�..
			return false;

		int iMySkillLevel = GetSkillLevel( eSkillID );
		if( iMySkillLevel <= 0 )
			return false;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

		if( NULL == pSkillTemplet )
			return false;


		const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetGuildSkillTreeTemplet( (int)m_iGuildClass, eSkillID );
		if( NULL == pSkillTreeTemplet )
			return false;


		CX2SkillTree::SKILL_ID eFollowingSkillID = static_cast<CX2SkillTree::SKILL_ID>(pSkillTreeTemplet->m_iFollowingSkill);
		if( true == DoIHaveThisSkill( eFollowingSkillID ) )
			return false;


		// ��ų �ǵ����⸦ �ϰ� �� �Ŀ� ���� tier�� ��ų�� �� ��� �Ǵ� ��� ��ų ������ �� �� ����
		std::vector<int> vecCumulativeTierSPoint;
		CalcCumulativeUsedGuildSPointOnEachTier( vecCumulativeTierSPoint );


		const int MAGIC_SKILL_POINT_PER_TIER = 5;
		for( UINT i=pSkillTreeTemplet->m_iTier+1; i<vecCumulativeTierSPoint.size(); i++ )
		{
			if( vecCumulativeTierSPoint[i-1]-1 < (int)i * MAGIC_SKILL_POINT_PER_TIER )
			{
				return false;
			}
		}

		return true;
	}
//}} oasis907 : ����� //// 2009-11-17 ////

	if( true == g_pData->GetSkillTree()->IsUnitTypeDefaultSkill( eSkillID ) && GetSkillLevel( eSkillID, true ) <= 1)	// �¿� ���� : 1���������� ���� �� �ְ�..
		return false;


	int iMySkillLevel = GetSkillLevel( eSkillID );
	if( iMySkillLevel <= 0 )
		return false;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

	if( NULL == pSkillTemplet )
		return false;


	const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( (int)m_iUnitClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return false;


		CX2SkillTree::SKILL_ID eFollowingSkillID = static_cast<CX2SkillTree::SKILL_ID>( pSkillTreeTemplet->m_iFollowingSkill );
		if( true == DoIHaveThisSkill( eFollowingSkillID ) )
			return false;



	// ��ų �ǵ����⸦ �ϰ� �� �Ŀ� ���� tier�� ��ų�� �� ��� �Ǵ� ��� ��ų ������ �� �� ����
	std::vector<int> vecCumulativeTierSPoint;
	CalcCumulativeUsedSPointOnEachTier( vecCumulativeTierSPoint );


	const int MAGIC_SKILL_POINT_PER_TIER = 5;
	for( UINT i=pSkillTreeTemplet->m_iTier+1; i<vecCumulativeTierSPoint.size(); i++ )
	{
		if( vecCumulativeTierSPoint[i-1]-1 < (int)i * MAGIC_SKILL_POINT_PER_TIER )
		{
			return false;
		}
	}

	return true;
}


bool CX2UserSkillTree::CanILearnThisSkill( CX2SkillTree::SKILL_ID eSkillID, bool bCheckSP /*= true*/ ) const
{
//{{ oasis907 : ����� //// 2009-11-17 ////
	if(IsGuildSkill(eSkillID))
	{
		if( NULL == g_pData ||
			NULL == g_pData->GetMyUser() ||
			NULL == g_pData->GetMyUser()->GetSelectUnit() )
		{
			return false;
		}

		const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

		if(bCheckSP == true)
		{
			if( pUnitData->m_iGuildSPoint + pUnitData->m_iGuildCSPoint < 1 )
				return false;
		}

		if( true == IsMaxSkillLevel( eSkillID ) )
			return false;

		int iSkillLevelToLearn = GetSkillLevel( eSkillID ) + 1;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

		if( NULL == pSkillTemplet )
			return false;



		// ������������ ���� ��ų�� ȹ���� �� ����
		if( true == pSkillTemplet->m_bBornSealed )
		{
			if( false == IsSkillUnsealed( eSkillID ) )
			{
				return false;
			}
		}

		const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetGuildSkillTreeTemplet( (int)m_iGuildClass, eSkillID );
		if( NULL == pSkillTreeTemplet )
			return false;


		const int MAGIC_SKILL_POINT_PER_TIER = 5;
		int iUsedSPoint = 0;
		int iUsedCSPoint = 0;
		CalcUsedGuildSPointAndCSPoint( iUsedSPoint, iUsedCSPoint );
		if( iUsedSPoint + iUsedCSPoint < pSkillTreeTemplet->m_iTier * MAGIC_SKILL_POINT_PER_TIER )
		{
			return false;
		}



		CX2SkillTree::SKILL_ID ePrecedingSkillID = static_cast<CX2SkillTree::SKILL_ID>( pSkillTreeTemplet->m_iPrecedingSkill );

		if( CX2SkillTree::SI_NONE != ePrecedingSkillID &&false == IsMaxSkillLevel( ePrecedingSkillID ) )
			return false;

		return true;

	}
//}} oasis907 : ����� //// 2009-11-17 ////

	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return false;
	}

	const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

	if(bCheckSP == true)
	{
		if( pUnitData->m_iSPoint + pUnitData->m_iCSPoint < 1 )
			return false;
	}

	if( true == IsMaxSkillLevel( eSkillID ) )
		return false;

	int iSkillLevelToLearn = GetSkillLevel( eSkillID ) + 1;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID );

	if( NULL == pSkillTemplet )
		return false;



	// ������������ ���� ��ų�� ȹ���� �� ����
	if( true == pSkillTemplet->m_bBornSealed )
	{
		if( false == IsSkillUnsealed( eSkillID ) )
		{
			return false;
		}
	}


	const CX2SkillTree::SkillTreeTemplet* pSkillTreeTemplet = g_pData->GetSkillTree()->GetSkillTreeTemplet( (int)m_iUnitClass, eSkillID );
	if( NULL == pSkillTreeTemplet )
		return false;


	const int MAGIC_SKILL_POINT_PER_TIER = 5;
	int iUsedSPoint = 0;
	int iUsedCSPoint = 0;
	CalcUsedSPointAndCSPoint( iUsedSPoint, iUsedCSPoint );
	if( iUsedSPoint + iUsedCSPoint < pSkillTreeTemplet->m_iTier * MAGIC_SKILL_POINT_PER_TIER )
	{
		return false;
	}



	CX2SkillTree::SKILL_ID ePrecedingSkillID = static_cast<CX2SkillTree::SKILL_ID>( pSkillTreeTemplet->m_iFollowingSkill );
	if( false == IsMaxSkillLevel( ePrecedingSkillID ) )
		return false;

	return true;
}


bool CX2UserSkillTree::CanIInitSkillTree() const
{
	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return false;

	for( SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].begin(); mit != m_vecSkillDataMap[m_usUsingPage].end(); ++mit )
	{
		CX2SkillTree::SKILL_ID eSkillID = mit->first;
		const UserSkillData& userSkillData = mit->second;
		if( false == g_pData->GetSkillTree()->IsUnitTypeDefaultSkill( eSkillID ) )
		{
			return true;
		}
		else if( GetSkillLevel( eSkillID ) > 1 )
		{
			return true;
		}
	}

	return false;

}
//{{ oasis907 : ����� //// 2009-11-12 ////
bool CX2UserSkillTree::CanIInitGuildSkillTree() const
{
	for(SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.begin(); mit != m_mapGuildSkillAcquired.end(); ++mit)
	{
		CX2SkillTree::SKILL_ID eSkillID = mit->first;
		const UserSkillData& userSkillData = mit->second;
		if( false == g_pData->GetSkillTree()->IsUnitTypeDefaultSkill( eSkillID ) )
		{
			return true;
		}
		else if( GetSkillLevel( eSkillID ) > 1 )
		{
			return true;
		}
	}

	return false;

}
//}} oasis907 : ����� //// 2009-11-12 ////


int CX2UserSkillTree::GetSkillEquippedSlotIndex( CX2SkillTree::SKILL_ID eSkillID, bool bSlotB ) const
{
	if( false == bSlotB )
	{
		for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
		{
			if( m_aEquippedSkill[i].m_eID == eSkillID )
				return i;
		}

	}
	else
	{
		for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); ++i )
		{
			if( m_aEquippedSkillSlotB[i].m_eID == eSkillID )
				return i;
		}
	}

	return -1;

}

bool CX2UserSkillTree::GetEnabledSkillSlotB() const
{
	switch( m_eSkillSlotBExpirationState )
	{
	case SSBES_EXPIRED:
		{
			return false;
		} break;

	case SSBES_NOT_EXPIRED:
	case SSBES_PERMANENT:
		{
			return true;
		} break;
	}

	return false;
}



void CX2UserSkillTree::SetSkillCoolTimeLeft( CX2SkillTree::SKILL_ID eSkillID, float fCoolTimeLeft )
{
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();

	for( int i=0; i<ARRAY_SIZE( m_aEquippedSkill ); ++i )
	{
		SkillSlotData& skillSlot = m_aEquippedSkill[ i ];

		if( eSkillID == skillSlot.m_eID )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUserSkillTemplet( skillSlot.m_eID );

			if( pSkillTemplet != NULL && ( pSkillTemplet->m_eType == CX2SkillTree::ST_RELATIONSHIP_SKILL
				|| ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && skillSlot.m_eID != CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION ) ) )
			{
				const int iSkillTempletLevel = max( 1, GetSkillLevel( pSkillTemplet->m_eID ) );
				const float fTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel );

				skillSlot.m_fCoolTimeLeft = fTime;
				pGageManager->SetMySkillCoolTimeList( i, fTime );
			}
			else
			{
				skillSlot.m_fCoolTimeLeft = fCoolTimeLeft;
				pGageManager->SetMySkillCoolTimeList( i, fCoolTimeLeft );
			}

			float fDecreaseRate = 1.0f;
			if( NULL != pSkillTemplet && NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
				fDecreaseRate =	g_pData->GetMyUser()->GetSelectUnit()->GetSkillCoolTimeDecreaseRate(eSkillID, pSkillTemplet->m_eType);

			if( 1.0f != fDecreaseRate )
			{
				float fCoolTimeLeftOld = skillSlot.m_fCoolTimeLeft;
				skillSlot.m_fCoolTimeLeft = fCoolTimeLeftOld * fDecreaseRate;
				pGageManager->SetMySkillCoolTimeList( i, fCoolTimeLeftOld * fDecreaseRate );
			}
		}
	}

	if( true == GetEnabledSkillSlotB() )
	{
		for( int i=0; i<ARRAY_SIZE( m_aEquippedSkillSlotB ); ++i )
		{
			SkillSlotData& skillSlot = m_aEquippedSkillSlotB[ i ];

			if( eSkillID == skillSlot.m_eID )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUserSkillTemplet( skillSlot.m_eID );

				if( pSkillTemplet != NULL && ( pSkillTemplet->m_eType == CX2SkillTree::ST_RELATIONSHIP_SKILL
					|| ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && skillSlot.m_eID != CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION ) ) )
				{
					const int iSkillTempletLevel = max( 1, GetSkillLevel( pSkillTemplet->m_eID ) );
					const float fTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel );

					skillSlot.m_fCoolTimeLeft = fTime;
					pGageManager->SetMySkillCoolTimeList( i + 4, fTime );
				}
				else
				{
					skillSlot.m_fCoolTimeLeft = fCoolTimeLeft;
					pGageManager->SetMySkillCoolTimeList( i + 4, fCoolTimeLeft );
				}

				float fDecreaseRate = 1.0f;
				if( NULL != pSkillTemplet && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
					fDecreaseRate =	g_pData->GetMyUser()->GetSelectUnit()->GetSkillCoolTimeDecreaseRate(eSkillID, pSkillTemplet->m_eType);

				if( 1.0f != fDecreaseRate )
				{
					float fCoolTimeLeftOld = skillSlot.m_fCoolTimeLeft;
					skillSlot.m_fCoolTimeLeft = fCoolTimeLeftOld * fDecreaseRate;
					pGageManager->SetMySkillCoolTimeList( i, fCoolTimeLeftOld * fDecreaseRate );
				}
			}
		}
	}
}

//{{ kimhc // 2010.11.2 // �������� - ���� ������

void CX2UserSkillTree::ResetLeftSkillCoolTimeAll()
{
	// ���� a�� ���� b�� ����� ���ٰ� �����Ͽ���
	const int iSizeOfIndex = ARRAY_SIZE( m_aEquippedSkill );
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();

	for ( int iCurrentIndex = 0; iCurrentIndex < iSizeOfIndex; ++iCurrentIndex )
	{
		if( NULL != g_pMain && g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
		{
			m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft = 0.0f;
			m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft = 0.0f;

			pGageManager->SetMySkillCoolTimeList( iCurrentIndex, 0.0f );
			pGageManager->SetMySkillCoolTimeList( iCurrentIndex + 4, 0.0f );
		}
		else
		{
			const CX2SkillTree::SkillTemplet* pSkillTempletA = GetUserSkillTemplet( m_aEquippedSkill[iCurrentIndex].m_eID );

			if ( pSkillTempletA != NULL && ( pSkillTempletA->m_eType != CX2SkillTree::ST_HYPER_ACTIVE_SKILL &&
				 pSkillTempletA->m_eType != CX2SkillTree::ST_RELATIONSHIP_SKILL ) )
			{
				m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft = 0.0f;
				pGageManager->SetMySkillCoolTimeList( iCurrentIndex, 0.0f );
			}

			const CX2SkillTree::SkillTemplet* pSkillTempletB = GetUserSkillTemplet( m_aEquippedSkillSlotB[iCurrentIndex].m_eID );

			if ( pSkillTempletB != NULL && ( pSkillTempletB->m_eType != CX2SkillTree::ST_HYPER_ACTIVE_SKILL &&
				pSkillTempletB->m_eType != CX2SkillTree::ST_RELATIONSHIP_SKILL ) )
			{
				m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft = 0.0f;
				pGageManager->SetMySkillCoolTimeList( iCurrentIndex + 4, 0.0f );
			}
		}
	}
}


void CX2UserSkillTree::ResetLeftSkillCoolTimeBySkillType( const CX2SkillTree::SKILL_TYPE eSkillType, const float fCoolTime /*= 0.f*/, bool bForceSet /*= true*/, BUFF_CHANGE_TYPE eChangeType /*= BCT_RELATION_VALUE*/, const float fCoolTimeRate /* = 1.f */, const bool bIsRelativeAllSkill /* = false */ )


{
	// ���� a�� ���� b�� ����� ���ٰ� �����Ͽ���
	const int iSizeOfIndex = ARRAY_SIZE( m_aEquippedSkill );
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();

	for ( int iCurrentIndex = 0; iCurrentIndex < iSizeOfIndex; ++iCurrentIndex )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletSlotA = GetUserSkillTemplet( m_aEquippedSkill[iCurrentIndex].m_eID );

		if ( NULL != pSkillTempletSlotA && ( eSkillType == pSkillTempletSlotA->m_eType
			|| bIsRelativeAllSkill == true								// ���� ������ ��ų�� ��� �����ϴ� ��ų Ÿ��
			) )
		{

			float fNowCoolTime = m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft;		/// �ش� ��ų�� ���� ��Ÿ��

			switch ( eChangeType )
			{
			case BCT_FIX_VALUE:
				{
					/// ���� ��Ÿ�� ���� ���� ������ �߰ų� ���� ��Ÿ���� ������ ��Ÿ�Ӻ��� �ξ� Ŭ��, ���� ��Ÿ���� ������ ��Ÿ������ �����Ѵ�.
					if ( true == bForceSet || fNowCoolTime > fCoolTime )
					{
						m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft = fCoolTime;
						pGageManager->SetMySkillCoolTimeList( iCurrentIndex, fCoolTime );
					}
				} break;

			case BCT_RELATION_VALUE:
				{
					fNowCoolTime += fCoolTime;
					fNowCoolTime = max( 0, fNowCoolTime );

					m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft = fNowCoolTime;
					pGageManager->SetMySkillCoolTimeList( iCurrentIndex, fNowCoolTime );
				} break;
			case BCT_PERCENT:
				{
					int iSkillLevel = GetSkillLevel ( pSkillTempletSlotA->m_eID );
					float fMaxCoolTime = pSkillTempletSlotA->GetSkillCoolTimeValue( iSkillLevel );
					fNowCoolTime = fMaxCoolTime * fCoolTimeRate;

					m_aEquippedSkill[iCurrentIndex].m_fCoolTimeLeft = fNowCoolTime;
					pGageManager->SetMySkillCoolTimeList( iCurrentIndex, fNowCoolTime);
				} break;

			default:
				break;
			}



		}

		const CX2SkillTree::SkillTemplet* pSkillTempletSlotB = GetUserSkillTemplet( m_aEquippedSkillSlotB[iCurrentIndex].m_eID );

		if ( NULL != pSkillTempletSlotB && ( eSkillType == pSkillTempletSlotB->m_eType
			|| bIsRelativeAllSkill == true								// ���� ������ ��ų�� ��� �����ϴ� ��ų Ÿ��

			) )
		{

			float fNowCoolTime = m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft;	/// �ش� ��ų�� ���� ��Ÿ��

			switch( eChangeType )
			{
			case BCT_FIX_VALUE:
				{
					/// ���� ��Ÿ�� ���� ���� ������ �߰ų� ���� ��Ÿ���� ������ ��Ÿ�Ӻ��� �ξ� Ŭ��, ���� ��Ÿ���� ������ ��Ÿ������ �����Ѵ�.
					if ( true == bForceSet || fNowCoolTime > fCoolTime )
					{
						m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft = fCoolTime;
						pGageManager->SetMySkillCoolTimeList( iCurrentIndex + 4, fCoolTime );
					}
				} break;

			case BCT_RELATION_VALUE:
				{
					fNowCoolTime += fCoolTime;
					fNowCoolTime = max( 0, fNowCoolTime );

					m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft = fNowCoolTime;
					pGageManager->SetMySkillCoolTimeList( iCurrentIndex + 4, fNowCoolTime );
				} break;


			case BCT_PERCENT:
				{
					int iSkillLevel = GetSkillLevel ( pSkillTempletSlotB->m_eID );
					float fMaxCoolTime = pSkillTempletSlotB->GetSkillCoolTimeValue( iSkillLevel );
					fNowCoolTime = fMaxCoolTime * fCoolTimeRate;

					m_aEquippedSkillSlotB[iCurrentIndex].m_fCoolTimeLeft = fNowCoolTime;
					pGageManager->SetMySkillCoolTimeList( iCurrentIndex, fNowCoolTime);
				} break;


			default:
				break;
			}


		}
	}
}

//}} kimhc // 2010.11.2 // �������� - ���� ������

//{{ oasis907 : ����� //// 2009-11-16 ////
bool CX2UserSkillTree::IsGuildSkill( CX2SkillTree::SKILL_ID eSkillID ) const
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet(eSkillID );

	if(pSkillTemplet == NULL)
	{
		return false;
	}
	if(pSkillTemplet->m_bGuildSkill)
	{
		return true;
	}
	return false;

}
//}} oasis907 : ����� //// 2009-11-16 ////


bool CX2UserSkillTree::GetEqipSkillMemo(int iSkillMemo) const
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

	for(UINT i=0; i<m_vecSkillNote.size(); ++i)
	{
		if(m_vecSkillNote[i] == iSkillMemo)
		{
			return true;
		}
	}

	return false;
}

//���� B�� ���� �� �� �������� �������� �ʴ� ���� ����
int CX2UserSkillTree::UpdateEquippedSkillLevelFromAcqureidMap( const CX2SkillTree::SKILL_ID eSkillID, const int iSlotIndex_, bool bSlotB )
{
	if ( m_usUsingPage < m_vecSkillDataMap.size() && !m_vecSkillDataMap[m_usUsingPage].empty() )
	{
		SkillDataMap::const_iterator cmItr = m_vecSkillDataMap[m_usUsingPage].find( eSkillID );
		if ( m_vecSkillDataMap[m_usUsingPage].end() != cmItr )
		{
			if ( iSlotIndex_ >= 0 && iSlotIndex_ < ARRAY_SIZE( m_aEquippedSkill ) )
			{
				if( true == bSlotB )
				{
					m_aEquippedSkillSlotB[iSlotIndex_].m_iSkillLevel = cmItr->second.m_iSkillLevel;
					return m_aEquippedSkillSlotB[iSlotIndex_].m_iSkillLevel;
				}
				else
				{
					m_aEquippedSkill[iSlotIndex_].m_iSkillLevel = cmItr->second.m_iSkillLevel;
					return m_aEquippedSkill[iSlotIndex_].m_iSkillLevel;
				}
			}
		}
	}

	return 0;
}

void CX2UserSkillTree::InitSkillCoolTimeFromGageManager( const vector<float>& vecSkillCoolTime_ )
{
	// ���� a�� ���� b�� ����� ���ٰ� �����Ͽ���
	const int iSizeOfIndex = ARRAY_SIZE( m_aEquippedSkill );

	for ( UINT uiSlotIndex = 0; uiSlotIndex < vecSkillCoolTime_.size(); ++uiSlotIndex )
	{
		if ( iSizeOfIndex > uiSlotIndex )
			m_aEquippedSkill[uiSlotIndex].m_fCoolTimeLeft = vecSkillCoolTime_[uiSlotIndex];
		else
			m_aEquippedSkillSlotB[uiSlotIndex - iSizeOfIndex].m_fCoolTimeLeft = vecSkillCoolTime_[uiSlotIndex];
	}
}

/*static*/ void CX2UserSkillTree::GetSlotIndexAndSlotB( IN const int iSkillSlotId_, OUT int& iSlotIndex_, OUT bool& bSlotB_ )
{
	// SlotB�� ù��° ���� ID int������ ����
	const int iSlotBStartId = static_cast<int>( CX2UserSkillTree::SKILL_SLOT_B1 );

	// iSkillSlotId_�� B�������� �ƴ���..
	bSlotB_ = ( iSkillSlotId_ >= iSlotBStartId ? true : false );

	// B�����̸� B������ ���� ������ ���� ���ڷ� �Ѱ��� ���̵𿡼� ���� (0~3) ������ ���� Index�� ����
	iSlotIndex_ = ( bSlotB_ == true ? iSkillSlotId_ - iSlotBStartId : iSkillSlotId_ );
}






/** @function	: SetDefaultSkill
	@brief		: �� ������ �⺻ ��ų ���� �Լ� ( SkillData.lua )
*/
void CX2UserSkillTree::SetDefaultSkill( USHORT usPageIndex_, bool bCanPassIfChecked_ /* = false */ )
{
	/// �� ������ �⺻ ��ų ����
	if ( NULL == g_pData  ||
		 NULL == g_pData->GetSkillTree() ||
		 NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	const int iUnitClass = static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() );	/// �ش� ������ Ŭ����
	const std::map<int, std::vector<int>>					mapDefaultSkill		= g_pData->GetSkillTree()->GetMapDefaultSkill();	/// �⺻ ��ų ����

	const std::map<int, std::vector<int>>::const_iterator	it					= mapDefaultSkill.find( iUnitClass );


	if ( it != mapDefaultSkill.end() )	/// �ش� Ŭ������ �⺻ ��ų ������ �ִٸ�, ����
	{
		std::vector<int> vecDefaultSkillID = it->second;

		BOOST_FOREACH( int iValue, vecDefaultSkillID )
		{
			if ( true == bCanPassIfChecked_ &&
				 0 < g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( static_cast<CX2SkillTree::SKILL_ID> ( iValue ) ) )
			{
				continue;
			}
			SetSkillLevelAndCSP( static_cast<CX2SkillTree::SKILL_ID>( iValue ), 1, 0, usPageIndex_ );
		}
	}
}

/** @function	GetIncreaseSkillLevel
	@brief		��ų ���� ���� ȿ��ġ ���
	@param[in]	��ų ID
	@return		int : ��ų ����
*/
int CX2UserSkillTree::GetIncreaseSkillLevel( CX2SkillTree::SKILL_ID eSkillID ) const
{
	KTDXPROFILE();

	if(IsGuildSkill(eSkillID))
	{
		SkillDataMap::const_iterator mit = m_mapGuildSkillAcquired.find( eSkillID );
		if( mit != m_mapGuildSkillAcquired.end() )
		{
			const UserSkillData& userSkillData = mit->second;
			return userSkillData.GetIncreaseSkillLevel();
		}
		return 0;
	}

	if ( true == m_vecSkillDataMap.empty() || m_usUsingPage >= m_vecSkillDataMap.size() )
		return 0;

	SkillDataMap::const_iterator mit = m_vecSkillDataMap[m_usUsingPage].find( eSkillID );
	if( mit != m_vecSkillDataMap[m_usUsingPage].end() )
	{
		const UserSkillData& userSkillData = mit->second;
		return userSkillData.GetIncreaseSkillLevel();
	}
	return 0;
}


/// ��ų ���� ID�� ���� �Ǿ��ִ� ��ų ���̵� ��ȯ
const CX2SkillTree::SKILL_ID CX2UserSkillTree::GetSkillIDByEquipSlotID( IN const int iStateID_ ) const
{
	int  iSkillSlotIndex	= -1;
	bool bSlotB				= false;

	switch ( CX2GUUser::USER_STATE_ID( iStateID_ ) )
	{
	case CX2GUUser::USI_SPECIAL_ATTACK_1:
	case CX2GUUser::USI_SPECIAL_ATTACK_HYPER_1:
		{
			iSkillSlotIndex = 0;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_2:
	case CX2GUUser::USI_SPECIAL_ATTACK_HYPER_2:
		{
			iSkillSlotIndex = 1;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_3:
	case CX2GUUser::USI_SPECIAL_ATTACK_HYPER_3:
		{
			iSkillSlotIndex = 2;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_4:
	case CX2GUUser::USI_SPECIAL_ATTACK_HYPER_4:
		{
			iSkillSlotIndex = 3;
		} break;

	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_1:
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_HYPER_1:
		{
			iSkillSlotIndex = 0;
			bSlotB = true;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_2:
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_HYPER_2:
		{
			iSkillSlotIndex = 1;
			bSlotB = true;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_3:
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_HYPER_3:
		{
			iSkillSlotIndex = 2;
			bSlotB = true;
		} break;
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_4:
	case CX2GUUser::USI_SPECIAL_ATTACK_SLOTB_HYPER_4:
		{
			iSkillSlotIndex = 3;
			bSlotB = true;
		} break;
	}

	const SkillSlotData* pSkillSlotData = GetSkillSlot( iSkillSlotIndex, bSlotB );

	if ( NULL != pSkillSlotData )
		return pSkillSlotData->m_eID;

	return CX2SkillTree::SI_NONE;
}

/** @function : GetIncreaseGainSoulRateByHit()
	@brief : 'Ÿ�� ��' ���� �ϴ� '���������ӵ�'�� ���� ����
			 -> ����� ��� ��ų�� ���ؼ��� ���� ��
*/
const float CX2UserSkillTree::GetIncreaseGainSoulRateByHit() const
{
	float fIncreaseSoulRate = 1.f;

	/// �г� �ع�
	const CX2SkillTree::SkillTemplet* pSkillTemplet =
		GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_LIBERATION_OF_ANGER, g_pData->GetSelectUnitMemberShipGrade() );

	if( NULL != pSkillTemplet )
	{
		// ������
		//TODO : ��� ��ų ������ Basic Level�� �޾� ���� �ִµ�, ���� ȿ���� ���� ���� �� �ֵ��� �ؾ� ��.
		//     : ���� ��� ��� ��ų�� true�� �����Ǿ� �־ true ���� ��
		const int iSkillTempletLevel = max( 1, GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����
		float fRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_ON_HIT_REL, iSkillTempletLevel );
		fIncreaseSoulRate += CalculateIncreasingRate( fRate );
	}

	return fIncreaseSoulRate;
}
/** @function : GetIncreaseGainSoulRateByDamage()
	@brief : '�ǰ� ��' ���� �ϴ� '���������ӵ�'�� ���� ����
			 -> ����� ��� ��ų�� ���ؼ��� ���� ��
*/
const float CX2UserSkillTree::GetIncreaseGainSoulRateByDamage() const
{
	float fIncreaseSoulRate = 1.f;

	/// �ݳ�
	const CX2SkillTree::SkillTemplet* pSkillTemplet =
		GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_VIOLENT, g_pData->GetSelectUnitMemberShipGrade() );
	if( NULL != pSkillTemplet )
	{
		// ������
		//TODO : ��� ��ų ������ Basic Level�� �޾� ���� �ִµ�, ���� ȿ���� ���� ���� �� �ֵ��� �ؾ� ��.
		//     : ���� ��� ��� ��ų�� true�� �����Ǿ� �־ true ���� ��
		const int iSkillTempletLevel = max( 1, GetSkillLevel( pSkillTemplet->m_eID, true) );	/// ��ų ����
		float fRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SOUL_GAIN_GET_HIT_REL, iSkillTempletLevel );
		fIncreaseSoulRate += CalculateIncreasingRate( fRate );
	}

	return fIncreaseSoulRate;
}
