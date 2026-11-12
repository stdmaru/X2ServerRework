#include "stdafx.h"
#include "./X2InformerManager.h"

CX2InformerManager::CX2InformerManager(void)
{
	CX2InformerSkill* pInformerSkill = new CX2InformerSkill();
	m_vecInformer.push_back( pInformerSkill );
	CX2InformerQuest* pInformerQuest = new CX2InformerQuest();
	m_vecInformer.push_back( pInformerQuest );
	CX2InformerInven* pInformerInven = new CX2InformerInven();
	m_vecInformer.push_back( pInformerInven );
	CX2InformerCashShop* pInformerCashShop = new CX2InformerCashShop();
	m_vecInformer.push_back( pInformerCashShop );

	//{{ kimhc // 2009-11-02 // 길드 추가
	CX2InformerGuildExp* pInformerGuildExp = new CX2InformerGuildExp();
	m_vecInformer.push_back( pInformerGuildExp );
	//}} kimhc // 2009-11-02 // 길드 추가
}

CX2InformerManager::~CX2InformerManager(void)
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		SAFE_DELETE( pInformer );
	}
	m_vecInformer.clear();
}

//{{ kimhc // 2009-11-02 // 길드 추가
CX2InformerGuildExp* CX2InformerManager::GetGuildInformer()
{
	CX2InformerGuildExp* pInformer = NULL;
	if ( IsCreatedInformerType( CX2Informer::XIT_GUILD_EXP ) == true )
	{
		pInformer = static_cast< CX2InformerGuildExp* >( GetInformerByType( CX2Informer::XIT_GUILD_EXP ) );
	}
	else
	{
		CX2InformerGuildExp* pInformer = new CX2InformerGuildExp();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
}
//}} kimhc // 2009-11-02 // 길드 추가

CX2InformerSkill* CX2InformerManager::GetSkillInformer()
{
	CX2InformerSkill* pInformer = NULL;
	if ( IsCreatedInformerType( CX2Informer::XIT_SKILL ) == true )
	{
		pInformer = (CX2InformerSkill*)GetInformerByType( CX2Informer::XIT_SKILL );
	}
	else
	{
		CX2InformerSkill* pInformer = new CX2InformerSkill();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;

}

CX2InformerQuest* CX2InformerManager::GetQuestInformer()
{
	CX2InformerQuest* pInformer = NULL;
	if ( IsCreatedInformerType( CX2Informer::XIT_QUEST ) == true )
	{
		pInformer = (CX2InformerQuest*)GetInformerByType( CX2Informer::XIT_QUEST );
	}
	else
	{
		CX2InformerQuest* pInformer = new CX2InformerQuest();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
}

CX2InformerInven* CX2InformerManager::GetInvenInformer()
{
	CX2InformerInven* pInformer = NULL;
	if ( IsCreatedInformerType( CX2Informer::XIT_INVEN ) == true )
	{
		pInformer = (CX2InformerInven*)GetInformerByType( CX2Informer::XIT_INVEN );
	}
	else
	{
		CX2InformerInven* pInformer = new CX2InformerInven();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
}

CX2InformerCashShop* CX2InformerManager::GetCashShopInformer()
{
	CX2InformerCashShop* pInformer = NULL;
	if ( IsCreatedInformerType( CX2Informer::XIT_CASH_SHOP ) == true )
	{
		pInformer = (CX2InformerCashShop*)GetInformerByType( CX2Informer::XIT_CASH_SHOP );
	}
	else
	{
		CX2InformerCashShop* pInformer = new CX2InformerCashShop();
		m_vecInformer.push_back( pInformer );

	}

	return pInformer;
}

void CX2InformerManager::Reset()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		pInformer->Reset();
	}
}

void CX2InformerManager::CheckInfo( bool /*bStateChange*/ )
{
}

void CX2InformerManager::InvalidInfo()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
			pInformer->InvalidInfo();
	}
}

void CX2InformerManager::ClearAllInfo()
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
			pInformer->ClearAllInfo();
	}
}


bool CX2InformerManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	for( int i = (int)m_vecInformer.size()-1; i >= 0; i-- )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
				return true;
		}
	}

	return false;
}

HRESULT CX2InformerManager::OnFrameMove( double fTime, float fElapsedTime )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			pInformer->OnFrameMove( fTime, fElapsedTime );
		}
	}

	return S_OK;
}

bool CX2InformerManager::IsCreatedInformerType( CX2Informer::X2_INFORMER_TYPE informerType )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->GetType() == informerType )
				return true;
		}
	}

	return false;
}

CX2Informer* CX2InformerManager::GetInformerByType( CX2Informer::X2_INFORMER_TYPE informerType )
{
	for ( int i = 0; i < (int)m_vecInformer.size(); i++ )
	{
		CX2Informer* pInformer = m_vecInformer[i];
		if ( pInformer != NULL )
		{
			if ( pInformer->GetType() == informerType )
				return pInformer;
		}
	}

	return NULL;
}
