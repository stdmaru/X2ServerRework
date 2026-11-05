#include "stdafx.h"
#include "./X2User.h"

CX2User::CX2User( const UserData& kUserData )
    : m_UserData( kUserData )
{
	//m_pUserData		= pUserData;
	m_pSelectUnit	= NULL;

	m_Cash = 0;

	m_UnitList.reserve( 16 );

	m_bHideSetDesc = true;

	m_bSharingBankOpen = false;

	m_ulGlobalTotalCash = 0;

}

CX2User::~CX2User(void)
{
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2Unit* pTempUnit = m_UnitList[i];
		SAFE_DELETE( pTempUnit );
	}
	m_UnitList.clear();

	//SAFE_DELETE( m_pUserData );
}

bool CX2User::AddUnit( CX2Unit* pUnit )
{
	m_UnitList.push_back( pUnit );
	return true;
}

CX2Unit* CX2User::GetUnitByUID( UidType unitUID )
{
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2Unit* pUnit = m_UnitList[i];
		if( pUnit->GetUID() == unitUID )
			return pUnit;
	}
	return NULL;
}

CX2Unit* CX2User::GetUnitByIndex( int index )
{
	if( index >= (int)m_UnitList.size() )
		return NULL;

	CX2Unit* pUnit = m_UnitList[index];
	return pUnit;
}

CX2Unit* CX2User::GetUnitByNickName( const WCHAR* pNickName )
{
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2Unit* pUnit = m_UnitList[i];
		if( wcscmp( pUnit->GetNickName(), pNickName ) == 0 )
			return pUnit;
	}
	return NULL;
}

bool CX2User::DeleteUnitByUID( UidType unitUID )
{
	vector< CX2Unit* >::iterator i;

	for( i = m_UnitList.begin(); i != m_UnitList.end(); i++ )
	{
		CX2Unit* pUnit = (*i);
		if( pUnit->GetUID() == unitUID )
		{
			if ( pUnit == m_pSelectUnit )
				m_pSelectUnit = NULL;

			SAFE_DELETE( pUnit );
			m_UnitList.erase(i);
			return true;
		}
	}

	return false;
}

void CX2User::DeleteAllUnit()
{
	vector< CX2Unit* >::iterator i;

	for( i = m_UnitList.begin(); i != m_UnitList.end(); i++ )
	{
		CX2Unit* pUnit = (*i);
		SAFE_DELETE( pUnit );
	}

	m_UnitList.clear();

	m_pSelectUnit = NULL;
}

bool CX2User::IsMyUnitByUnitUID( UidType unitUid )
{
	vector< CX2Unit* >::iterator i;

	for( i = m_UnitList.begin(); i != m_UnitList.end(); i++ )
	{
		CX2Unit* pUnit = (*i);
		if( pUnit->GetUID() == unitUid )
		{
			return true;
		}
	}

	return false;
}

void CX2User::SetGlobalCash(KGlobalCashInfo globalCashInfo)
{
	m_GlobalCashInfo = globalCashInfo;

	// Get Total Cash //
	m_ulGlobalTotalCash = 0; // init
	for (int i=0; i < KGlobalCashInfo::GCT_TOTAL_NUM; i++)
	{
		m_ulGlobalTotalCash += globalCashInfo.m_ulCash[i];
	}
}

void CX2User::SetGlobalTypeCash(int iCashType, int ulCash)
{
	if (iCashType < 0 || iCashType >= KGlobalCashInfo::GCT_TOTAL_NUM)
		return;

	m_GlobalCashInfo.m_ulCash[iCashType] = ulCash;

	// Get Total Cash //
	m_ulGlobalTotalCash = 0; // init
	for (int i=0; i < KGlobalCashInfo::GCT_TOTAL_NUM; i++)
	{
		m_ulGlobalTotalCash += m_GlobalCashInfo.m_ulCash[i];
	}
}

unsigned long CX2User::GetGlobalCash(int iCashType)
{
	if (iCashType < 0 || iCashType >= KGlobalCashInfo::GCT_TOTAL_NUM)
		return 0;

	return m_GlobalCashInfo.m_ulCash[iCashType];
}
unsigned long CX2User::GetGlobalCashForRescurrection()
{
	int iCashType = KGlobalCashInfo::GCT_PUBLISHER_CASH;


	if (iCashType < 0 || iCashType >= KGlobalCashInfo::GCT_TOTAL_NUM)
		return 0;

	return m_GlobalCashInfo.m_ulCash[iCashType];
}

unsigned long CX2User::GetGlobalMainCash()
{
	int iCashType = KGlobalCashInfo::GCT_PUBLISHER_CASH;


	if (iCashType < 0 || iCashType >= KGlobalCashInfo::GCT_TOTAL_NUM)
		return 0;

	return m_GlobalCashInfo.m_ulCash[iCashType];
}

bool CX2User::SortFirstByUID(UidType aUnitUID)
{
	auto FirstByUidFunc = [aUnitUID]( const CX2Unit* pUnit_ ) {
		return pUnit_->GetUID() == aUnitUID;
	};

	vector<CX2Unit*>::iterator it = find_if(m_UnitList.begin(), m_UnitList.end(), FirstByUidFunc );
	if( m_UnitList.end() == it )
		return false;
	else
	{
		CX2Unit* pSelectUnit = *it;
		m_UnitList.erase(it);
		m_UnitList.insert(m_UnitList.begin(), pSelectUnit);
		return true;
	}
}

void CX2User::ChangeEquippedSkillState( const int iSkillSlotId_, const int iSkillId_ )
{
	if ( NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

	CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

	// iSkillSlotId_가 B슬롯인지 아닌지..
	bool bSlotB = false;
	// B슬롯이면 B슬롯의 시작 슬롯을 현재 인자로 넘겨진 아이디에서 빼서 (0~3) 사이의 슬롯 Index를 구함
	int iSlotIndex = 0;

	CX2UserSkillTree::GetSlotIndexAndSlotB( IN iSkillSlotId_, OUT iSlotIndex, OUT bSlotB );
	const CX2SkillTree::SKILL_ID eSkillId = static_cast<CX2SkillTree::SKILL_ID>( iSkillId_ );
	const int iSkillLevel = pUnitData->m_UserSkillTree.UpdateEquippedSkillLevelFromAcqureidMap( eSkillId, iSlotIndex, bSlotB );

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillId );
	if ( NULL != pSkillTemplet )
	{
		pUnitData->m_UserSkillTree.SetSkillCoolTimeLeft( eSkillId, pSkillTemplet->GetSkillCoolTimeValue( iSkillLevel ) );
	}
}

int CX2User::GetIndexByUID( UidType unitUID )
{
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2Unit* pUnit = m_UnitList[i];
		if( pUnit->GetUID() == unitUID )
			return i;
	}
	return -1;
}
