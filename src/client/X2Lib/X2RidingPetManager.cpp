#include "stdafx.h"

/**
	@file	X2RidingPetManager.h
	@author	JHKang
	@brief	Ż �� ���� Ŭ����
	@since  2013-04-02
*/

#include "X2RidingPetManager.h"

CX2RidingPetManager* CX2RidingPetManager::m_pRidingPetManager = NULL;

//#pragma region ~RidingPetTemplet
CX2RidingPetManager::RidingPetTemplet::~RidingPetTemplet()
{
	SAFE_DELETE( m_InitLuaTemplet.m_pInitTemplet );
	SAFE_DELETE( m_InitLuaTemplet.m_pLuaManager );
}
//#pragma endregion

//#pragma region CX2RidingPetManager
CX2RidingPetManager::CX2RidingPetManager(void)
	: m_bShow( false )
	, m_ElapsedTimeDecrease( 2.4f )
	, m_ElapsedTimeRecovery( 7.2f )
	, m_ElapsedCanPushSummonButton( 1.0f )
	, m_fEnableStamina( 0.0f )
{
	m_mapRidingPet.clear();
	m_vecRemoveRidingPet.clear();

	m_RidingPetUid = -1;

	m_pSkillSlot = new CX2RidingPetSkillSlot;

	m_bIsDirectSummon = false;
	m_DirectSummonUid = -1;

	m_fUpdateNowStamina = 0.0f;
}
//#pragma endregion ������

//#pragma region ~CX2RidingPetManager
CX2RidingPetManager::~CX2RidingPetManager(void)
{
	RemoveAllRidingPet();

	RidingPetTempletMap::iterator iter;
	for ( iter = m_mapRidingPetTemplet.begin(); iter != m_mapRidingPetTemplet.end(); ++iter )
	{
		RidingPetTemplet *pRidingPetTemplet = iter->second;
		SAFE_DELETE( pRidingPetTemplet );
	}
	m_mapRidingPetTemplet.clear();

	SAFE_DELETE( m_pSkillSlot );
}
//#pragma endregion �Ҹ���

//#pragma region GetInstance
/*static*/ CX2RidingPetManager* CX2RidingPetManager::GetInstance()
{
	if ( NULL == m_pRidingPetManager )
		m_pRidingPetManager = new CX2RidingPetManager();

	return m_pRidingPetManager;
}
//#pragma endregion �̱��� ��ü ����

//#pragma region DestroyInstance
/*static*/ void CX2RidingPetManager::DestroyInstance()
{
	SAFE_DELETE( m_pRidingPetManager );
}
//#pragma endregion �̱��� ��ü ����

//#pragma region OnFrameMove
HRESULT CX2RidingPetManager::OnFrameMove( IN double fTime_, IN float fElapsedTime_ )
{
	int iIndex = 0;
	RidingPetMap::iterator iter;

	if ( m_pSkillSlot )
		m_pSkillSlot->OnFrameMove( fTime_, fElapsedTime_ );

	if ( m_mapRidingPet.size() > 0 )
	{
		for ( iter = m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); )
		{
			CX2RidingPetPtr pRidingPet = iter->second;

			if ( NULL == pRidingPet )
			{
				iter = m_mapRidingPet.erase( iter );
				continue;
			}

			if ( NULL != g_pX2Game )
			{
				RidingPetTemplet* pRidingPetTemplet = GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(
					pRidingPet->GetInfo().m_Id ) );

				if ( NULL == pRidingPetTemplet )
					return false;

				CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( pRidingPet->GetUserUid() );

				if( NULL == pUser )
				{
					iter = RemoveRidingPet( pRidingPet->GetUserUid() );
					continue;
				}
				else if ( NULL != pUser && NULL != pRidingPet )
				{
					bool bShow = pUser->GetShowObject();
					pRidingPet->SetShowObject( bShow );
					pRidingPet->SetInfo( pRidingPet->GetInfo(), pRidingPetTemplet->m_eSpecialMove );
				}

				if( g_pKTDXApp->GetIsNowVeryfy() == true )
				{
					pRidingPet->VerifyRindingPetInfo();

					std::vector<RidingPetSkillInfo>::iterator iter;

					for ( iter = pRidingPetTemplet->m_vecAttackSkill.begin(); iter != pRidingPetTemplet->m_vecAttackSkill.begin(); ++iter )
						iter->Verify();
				}
			}
			else if ( NULL != g_pTFieldGame )
			{
				CX2SquareUnit* pUser = g_pTFieldGame->GetSquareUnitByUID( pRidingPet->GetUserUid() );

				if ( NULL == pUser )
				{
					iter = RemoveRidingPet( pRidingPet->GetUserUid() );
					continue;
				}
				else if ( NULL != pUser && NULL != pRidingPet )
				{
					bool bShow = pUser->GetShowObject();
					pRidingPet->SetShowObject( bShow );
					pRidingPet->SetInfo( pRidingPet->GetInfo(), CX2RidingPet::RSM_NONE );
				}
			}

			pRidingPet->OnFrameMove( fTime_, fElapsedTime_ );

			++iter;
		}
	}

	m_ElapsedTimeDecrease.OnFrameMove( fElapsedTime_ );
	m_ElapsedTimeRecovery.OnFrameMove( fElapsedTime_ );
	m_ElapsedCanPushSummonButton.OnFrameMove( fElapsedTime_ );
	UpdateRidingPetGage();

	UnsummonRidingPet();
	return S_OK;
}
//#pragma endregion

//#pragma region OnFrameRender
HRESULT CX2RidingPetManager::OnFrameRender()
{
	if ( m_vecRemoveRidingPet.size() > 0 )
		RemoveRidingPetList();

	return S_OK;
}
//#pragma endregion

//#pragma region SetAllShow
void CX2RidingPetManager::SetShowAll( IN bool bVal_ )
{
	m_bShow = bVal_;

	RidingPetMap::iterator iter;

	for ( iter == m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); ++iter )
	{
		CX2RidingPetPtr pRidingPet = iter->second;
		pRidingPet->SetShowObject( m_bShow );
	}
}
//#pragma endregion

//#pragma region SetShow
void CX2RidingPetManager::SetShow( IN UidType uUserUid_, IN bool bVal_ )
{
	CX2RidingPet* pRidingPet = GetRidingPet( uUserUid_ );

	if ( NULL != pRidingPet )
		pRidingPet->SetShowObject( bVal_ );
}
//#pragma endregion

//#pragma region GetShow
bool CX2RidingPetManager::GetShow( IN UidType uUserUid_ )
{
	CX2RidingPet* pRidingPet = GetRidingPet( uUserUid_ );

	if ( NULL != pRidingPet )
		return pRidingPet->GetShowObject();

	return false;
}
//#pragma endregion

//#pragma region NotifyShowObjectChanged
void CX2RidingPetManager::NotifyShowObjectChanged( IN UidType uUserUid_, IN bool bVal_ )
{
	CX2RidingPet* pRidingPet = GetRidingPet( uUserUid_ );

	if ( NULL != pRidingPet && true == bVal_ )
		pRidingPet->InitPos();

	if ( NULL != pRidingPet )
		pRidingPet->SetShowObject( bVal_ );
}
//#pragma endregion

//#pragma region InitPos
void CX2RidingPetManager::InitPos()
{
	RidingPetMap::iterator iter;

	for ( iter = m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); ++iter )
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		if ( NULL != pRidingPet )
		{
			pRidingPet->InitPos();
		}
	}
}
//#pragma endregion

//#pragma region UICustomEventProc
bool CX2RidingPetManager::UICustomEventProc( IN HWND /*hWnd_*/, IN UINT /*uMsg_*/, IN WPARAM /*wParam_*/, IN LPARAM /*lParam_*/ )
{
	return false;
}
//#pragma endregion

//#pragma region UIServerEventProc
bool CX2RidingPetManager::UIServerEventProc( IN HWND hWnd_, IN UINT uMsg_, IN WPARAM wParam_, IN LPARAM lParam_ )
{
	switch( wParam_ )
	{
	case EGS_CREATE_RIDING_PET_ACK:
		return Handler_EGS_CREATE_RIDING_PET_ACK( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_SUMMON_RIDING_PET_ACK:
		return Handler_EGS_SUMMON_RIDING_PET_ACK( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_SUMMON_RIDING_PET_NOT:
		return Handler_EGS_SUMMON_RIDING_PET_NOT( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_UNSUMMON_RIDING_PET_ACK:
		return Handler_EGS_UNSUMMON_RIDING_PET_ACK( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_UNSUMMON_RIDING_PET_NOT:
		return Handler_EGS_UNSUMMON_RIDING_PET_NOT( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_RELEASE_RIDING_PET_ACK:
		return Handler_EGS_RELEASE_RIDING_PET_ACK( hWnd_, uMsg_, wParam_, lParam_ );

	case EGS_RELEASE_RIDING_PET_NOT:
		return Handler_EGS_RELEASE_RIDING_PET_NOT( hWnd_, uMsg_, wParam_, lParam_ );
	default:
		break;
	}

	return false;
}
//#pragma endregion

//#pragma region CreateRidingPet
CX2RidingPetPtr CX2RidingPetManager::CreateRidingPet( IN UidType uUserUid_, IN const CX2RidingPet::RidingPetInfo& RidingPetInfo_, IN D3DXVECTOR3 /*vInitPos_*/ )
{
	RidingPetTemplet* pRidingPetTemplet = GetRidingPetTemplet( (CX2RidingPetManager::RIDING_PET_UNIT_ID) RidingPetInfo_.m_Id );

	if( g_pMain->GetNowStateID() == CX2Main::XS_PVP_LOBBY ||
		g_pMain->GetNowStateID() == CX2Main::XS_PVP_ROOM ||
		g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME ||
		g_pMain->GetNowStateID() == CX2Main::XS_PVP_RESULT ||
		g_pMain->GetNowStateID() == CX2Main::XS_SQUARE_GAME ||
		g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_RESULT )
	{
		return CX2RidingPetPtr();
	}

	if( g_pX2Room != NULL )
	{
		CX2PVPRoom *pPvpRoom = (CX2PVPRoom*)g_pX2Room;
		if( pPvpRoom->IsOfficialPvp() == true )
			return CX2RidingPetPtr();
	}

	if( pRidingPetTemplet == NULL )
		return CX2RidingPetPtr();

	KLuaManager* pLuaManager = pRidingPetTemplet->m_InitLuaTemplet.m_pLuaManager;

	if ( NULL == pLuaManager )
		return CX2RidingPetPtr();

	CX2RidingPetPtr pRidingPet = CX2RidingPet::CreateRidingPetPtr();

	pRidingPet->SetRidingPet( pLuaManager->GetLuaState(), uUserUid_ );

	pRidingPet->SetInfo( RidingPetInfo_, pRidingPetTemplet->m_eSpecialMove );

	g_pKTDXApp->GetDGManager()->AddObjectChain( pRidingPet.get() );

	ChangeRidingPet( pRidingPet );

	return pRidingPet;
}
//#pragma endregion

//#pragma region CreateGameRidingPet
CX2RidingPetPtr CX2RidingPetManager::CreateGameRidingPet( IN UidType uUserUid_, IN const CX2RidingPet::RidingPetInfo& RidingPetInfo_ )
{
	if ( CX2Main::XS_PVP_LOBBY == g_pMain->GetNowStateID() || CX2Main::XS_PVP_ROOM == g_pMain->GetNowStateID() ||
		 CX2Main::XS_PVP_GAME == g_pMain->GetNowStateID() || CX2Main::XS_PVP_RESULT == g_pMain->GetNowStateID() ||
		 CX2Main::XS_SQUARE_GAME == g_pMain->GetNowStateID() )
	{
		return CX2RidingPetPtr();
	}

	RidingPetTemplet* pRidingPetTempet = GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(RidingPetInfo_.m_Id) );

	if ( NULL == pRidingPetTempet )
		return CX2RidingPetPtr();

	if ( NULL == pRidingPetTempet->m_InitLuaTemplet.m_pInitTemplet )
		return CX2RidingPetPtr();

	KLuaManager* pLuaManager = pRidingPetTempet->m_InitLuaTemplet.m_pLuaManager;
	if ( NULL == pLuaManager )
		return CX2RidingPetPtr();

	CX2RidingPetPtr pRidingPet = CX2RidingPet::CreateRidingPetPtr();
	pRidingPet->SetGameRidingPet( pLuaManager->GetLuaState(), uUserUid_ );
	pRidingPet->SetInfo( RidingPetInfo_, pRidingPetTempet->m_eSpecialMove );
	g_pKTDXApp->GetDGManager()->AddObjectChain( pRidingPet.get() );
	pRidingPet->OnFrameMove( g_NowTime, 0.f );

	ChangeRidingPet( pRidingPet );

	return pRidingPet;
}
//#pragma endregion

//#pragma region AddRidingPet
void CX2RidingPetManager::AddRidingPet( IN CX2RidingPetPtr pRidingPet_ )
{
	if ( NULL == pRidingPet_ )
		return;

	RidingPetMap::iterator iter;

	iter = m_mapRidingPet.find( pRidingPet_->GetUserUid() );

	if ( iter == m_mapRidingPet.end() )
	{
		m_mapRidingPet.insert( std::make_pair( pRidingPet_->GetUserUid(), pRidingPet_ ) );
	}
}
//#pragma endregion

//#pragma region ChangeRidingPet
void CX2RidingPetManager::ChangeRidingPet( IN CX2RidingPetPtr pRidingPet_ )
{
	if ( NULL == pRidingPet_ )
		return;

	RidingPetMap::iterator iter;

	iter = m_mapRidingPet.find( pRidingPet_->GetUserUid() );
	if ( iter == m_mapRidingPet.end() )
	{
		m_mapRidingPet.insert( std::make_pair( pRidingPet_->GetUserUid(), pRidingPet_ ) );
	}
	else
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		pRidingPet->SetShowObject( false );
		RemoveRidingPet( pRidingPet->GetUserUid() );

		m_mapRidingPet.insert( std::make_pair( pRidingPet->GetUserUid(), pRidingPet_ ) );
	}
}
//#pragma endregion

//#pragma region RemoveRidingPet
CX2RidingPetManager::RidingPetMap::iterator CX2RidingPetManager::RemoveRidingPet( IN UidType uUserUid_ )
{
	RidingPetMap::iterator iter = m_mapRidingPet.find( uUserUid_ );


	if ( iter != m_mapRidingPet.end() )
	{
		bool bFind = false;

		CX2RidingPetPtr pRidingPet1 = iter->second;

		for ( UINT i = 0; i < m_vecRemoveRidingPet.size(); ++i )
		{
			CX2RidingPetPtr pRidingPet2 = m_vecRemoveRidingPet[i];

			if ( pRidingPet2 == pRidingPet1 )
			{
				bFind = true;
				break;
			}
		}

		pRidingPet1->SetShowObject( false );

		if ( bFind == false )
			m_vecRemoveRidingPet.push_back( pRidingPet1 );
		iter = m_mapRidingPet.erase( iter );
	}

	return iter;
}
//#pragma endregion

//#pragma region RemoveAllRidingPet
void CX2RidingPetManager::RemoveAllRidingPet()
{
	/*RidingPetMap::iterator iter;

	for ( iter = m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); ++iter )
	{
		CX2RidingPetPtr pRidingPet = iter->second;
	}*/
	m_mapRidingPet.clear();

	RemoveRidingPetList();
}
//#pragma endregion

//#pragma region RemoveRidingPetList
void CX2RidingPetManager::RemoveRidingPetList()
{
	for ( UINT i = 0; i < m_vecRemoveRidingPet.size(); ++i )
	{
		CX2RidingPetPtr pRidingPet = m_vecRemoveRidingPet[i];
		pRidingPet.reset();
	}

	m_vecRemoveRidingPet.clear();
}
//#pragma endregion

//#pragma region GetMyRidingPet
CX2RidingPet* CX2RidingPetManager::GetMyRidingPet()
{
	UidType uUserUid = -1;

	if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
		uUserUid = g_pData->GetMyUser()->GetSelectUnit()->GetUID();

	return GetRidingPet( uUserUid );
}
//#pragma endregion

//#pragma region GetRidingPet
CX2RidingPet* CX2RidingPetManager::GetRidingPet( IN UidType uUserUid_ )
{
	RidingPetMap::iterator iter;

	iter = m_mapRidingPet.find( uUserUid_ );
	if ( iter == m_mapRidingPet.end() )
	{
		return NULL;
	}
	else
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		return pRidingPet.get();
	}

	return NULL;
}
//#pragma endregion

//#pragma region GetRidingPetPtr
CX2RidingPetPtr CX2RidingPetManager::GetRidingPetPtr( IN UidType uUserUid_ )
{
	RidingPetMap::iterator iter;

	iter = m_mapRidingPet.find( uUserUid_ );
	if ( iter == m_mapRidingPet.end() )
	{
		return CX2RidingPetPtr();
	}
	else
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		return pRidingPet;
	}

	return CX2RidingPetPtr();
}
//#pragma endregion

//#pragma region GetRidingPetIndex
CX2RidingPet* CX2RidingPetManager::GetRidingPetIndex( IN int iIndex_ )
{
	int iIndex = 0;
	RidingPetMap::iterator iter;

	for ( iter == m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); ++iter )
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		if ( iIndex == iIndex_ )
			return pRidingPet.get();

		++iIndex;
	}

	return NULL;
}
//#pragma endregion

//#pragma region GetRidingPetByUid
CX2RidingPet* CX2RidingPetManager::GetRidingPetByUid( IN UidType iUid_ )
{
	RidingPetMap::iterator iter;

	for ( iter = m_mapRidingPet.begin(); iter != m_mapRidingPet.end(); ++iter )
	{
		CX2RidingPetPtr pRidingPet = iter->second;

		if ( pRidingPet->GetInfo().m_Uid == iUid_ )
			return pRidingPet.get();
	}

	return NULL;
}
//#pragma endregion

//#pragma region OpenScriptFile
bool CX2RidingPetManager::OpenScriptFile(  IN const WCHAR* pFileName_ )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pRidingPetManager", this );


    if ( g_pKTDXApp->LoadLuaTinker( pFileName_ ) == false )
    {
		ErrorLogMsg( XEM_ERROR69, pFileName_ );

		return false;
    }


	LoadInitPetData();

	m_ElapsedTimeRecovery.SetTargetTime( GetRecoveryTimePerOne() );
	m_ElapsedTimeRecovery.ResetSumOfElapsedTime();
	return true;
}
//#pragma endregion

//#pragma region LoadInitPetData
void CX2RidingPetManager::LoadInitPetData()
{
	for ( RidingPetTempletMap::const_iterator iter = m_mapRidingPetTemplet.begin(); iter != m_mapRidingPetTemplet.end(); ++iter )
	{
		RidingPetTemplet* pRidingPetTemplet = iter->second;
		if ( NULL != pRidingPetTemplet )
		{
			pRidingPetTemplet->m_InitLuaTemplet.m_pInitTemplet = new RPInitTemplet;
			pRidingPetTemplet->m_InitLuaTemplet.m_pLuaManager
				= new KLuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

			if ( g_pKTDXApp->LoadAndDoMemory(
				pRidingPetTemplet->m_InitLuaTemplet.m_pLuaManager, pRidingPetTemplet->m_wstrScripName.c_str() ) == true )
			{
				CX2RidingPet::InitInit( pRidingPetTemplet->m_InitLuaTemplet.m_pInitTemplet->m_init,
					*pRidingPetTemplet->m_InitLuaTemplet.m_pLuaManager );
			}
		}
	}
}
//#pragma endregion

//#pragma region AddRidingPetTemplet_LUA
bool CX2RidingPetManager::AddRidingPetTemplet_LUA()
{
	RidingPetTemplet* pRidingPetTemplet = new RidingPetTemplet();
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	LUA_GET_VALUE_RETURN_ENUM( luaManager, "RIDING_PET_ID",	pRidingPetTemplet->m_Uid,	RIDING_PET_UNIT_ID,	RPUI_NONE,
							   SAFE_DELETE(pRidingPetTemplet); return false; );

	int iStringIndex;
	LUA_GET_VALUE( luaManager,	"COMMON_NAME",	iStringIndex,	STR_ID_EMPTY );
	pRidingPetTemplet->m_wstrName = GET_STRING( iStringIndex );

	LUA_GET_VALUE( luaManager,	"STAMINA_INCREASE_RATE",	pRidingPetTemplet->m_fIncreaseRate,	1.f	);
	if( pRidingPetTemplet->m_fIncreaseRate <= 0.f)
	{
		ASSERT(!"Increase Rate Error");
	}

	pRidingPetTemplet->m_fMaxStamina = static_cast<USHORT>( 100.f * pRidingPetTemplet->m_fIncreaseRate );

	LUA_GET_VALUE( luaManager,	"STAMINA_RECOVERY_RATE",	pRidingPetTemplet->m_fRecoveryRate,	1.f	);
	if( pRidingPetTemplet->m_fRecoveryRate <= 0.f)
	{
		ASSERT(!"Recovery Rate Error");
	}

	LUA_GET_VALUE( luaManager,	"SCRIPTNAME",	pRidingPetTemplet->m_wstrScripName,	L"" );

	float fInitSize = 1.f;
	LUA_GET_VALUE( luaManager,	"VIEWER_SIZE_X",	pRidingPetTemplet->m_fViewerSizeX, fInitSize );
	LUA_GET_VALUE( luaManager,	"VIEWER_SIZE_Y",	pRidingPetTemplet->m_fViewerSizeY, fInitSize );
	LUA_GET_VALUE( luaManager,	"VIEWER_SIZE_Z",	pRidingPetTemplet->m_fViewerSizeZ, fInitSize );

	int iDescription = STR_ID_27;
	LUA_GET_VALUE( luaManager,	"DESCRIPTION", iDescription, STR_ID_27 );
	pRidingPetTemplet->m_wstrDescription = GET_STRING( iDescription );

	if ( true == luaManager.BeginTable( "FACE_IMAGE" ) )
	{
		LUA_GET_VALUE( luaManager,	1,	pRidingPetTemplet->m_FaceImage.m_wstrImageName,	L"" );
		LUA_GET_VALUE( luaManager,	2,	pRidingPetTemplet->m_FaceImage.m_wstrKeyName,	L"" );

		luaManager.EndTable();
	}

	if ( true == luaManager.BeginTable( "ATTACK_SKILL_INFO" ) )
	{
		int iIndex = 1;

		while ( true == luaManager.BeginTable( iIndex ))
		{
			RidingPetSkillInfo ridingPetSkillInfo;

			int iSkillDesc = STR_ID_27;
			LUA_GET_VALUE( luaManager,	1,	ridingPetSkillInfo.m_wstrImageName,	L"" );
			LUA_GET_VALUE( luaManager,	2,	ridingPetSkillInfo.m_wstrKeyName,	L"" );
			LUA_GET_VALUE( luaManager,	3,	iSkillDesc,	STR_ID_27 );
			ridingPetSkillInfo.m_wstrSkillDesc = GET_STRING( iSkillDesc );
			LUA_GET_VALUE( luaManager,	4,	iSkillDesc,	STR_ID_27 );
			ridingPetSkillInfo.m_wstrSkillDesc += L"\n\n";
			ridingPetSkillInfo.m_wstrSkillDesc += GET_STRING( iSkillDesc );
			ridingPetSkillInfo.m_wstrSkillDesc += L"\n";
			float fValue = 0.f;
			LUA_GET_VALUE( luaManager, 5, fValue, 0.f );
			ridingPetSkillInfo.m_fConsumeMana = fValue;
			fValue = 0.f;
			LUA_GET_VALUE( luaManager, 6, fValue, 0.f );
			ridingPetSkillInfo.m_fCoolTime = fValue;

			pRidingPetTemplet->m_vecAttackSkill.push_back( ridingPetSkillInfo );
			luaManager.EndTable();

			++iIndex;
		}
		luaManager.EndTable();
	}

	if ( true == luaManager.BeginTable( "BUFF_SKILL_INFO" ) )
	{
		int iIndex = 1;

		while ( true == luaManager.BeginTable( iIndex ))
		{
			RidingPetBuffInfo ridingPetBuffInfo;

			int iSkillDesc = STR_ID_27;
			LUA_GET_VALUE( luaManager,	1,	ridingPetBuffInfo.m_wstrImageName,	L"" );
			LUA_GET_VALUE( luaManager,	2,	ridingPetBuffInfo.m_wstrKeyName,	L"" );
			LUA_GET_VALUE( luaManager,	3,	iSkillDesc,	STR_ID_27 );
			ridingPetBuffInfo.m_wstrSkillDesc = GET_STRING( iSkillDesc );
			LUA_GET_VALUE( luaManager,	4,	iSkillDesc,	STR_ID_27 );
			ridingPetBuffInfo.m_wstrSkillDesc += L"\n\n";
			ridingPetBuffInfo.m_wstrSkillDesc += GET_STRING( iSkillDesc );
			ridingPetBuffInfo.m_wstrSkillDesc += L"\n";

			/// ���� ���� ID �Է� ����
			LUA_GET_VALUE_ENUM( luaManager, 5, ridingPetBuffInfo.m_eBuffFactorID, BUFF_FACTOR_ID, BFI_NONE );

			pRidingPetTemplet->m_vecBuff.push_back( ridingPetBuffInfo );
			luaManager.EndTable();

			++iIndex;
		}
		luaManager.EndTable();
	}

	LUA_GET_VALUE_ENUM( luaManager, "SPECIAL_MOVE", pRidingPetTemplet->m_eSpecialMove, CX2RidingPet::RIDING_SPECIAL_MOVE, CX2RidingPet::RSM_NONE );

	if( luaManager.BeginTable( "SOCKET_OPTION" ) == true )
	{
		int iIndex = 1;
		int iBuffSillOptionId = 0;

		while( luaManager.GetValue( iIndex, iBuffSillOptionId  ) == true )
		{
			pRidingPetTemplet->m_vecSocketOption.push_back( iBuffSillOptionId );
			iIndex++;
		}
		luaManager.EndTable();
	}

	if( m_mapRidingPetTemplet.find( pRidingPetTemplet->m_Uid ) != m_mapRidingPetTemplet.end() )
	{
		SAFE_DELETE( pRidingPetTemplet );
		return false;
	}
	else
	{
		m_mapRidingPetTemplet.insert( std::make_pair( pRidingPetTemplet->m_Uid, pRidingPetTemplet ) );
	}

	return true;
}
//#pragma endregion

//#pragma region GetRidingTemplet
CX2RidingPetManager::RidingPetTemplet* CX2RidingPetManager::GetRidingPetTemplet( IN RIDING_PET_UNIT_ID eId_ )
{
	RidingPetTempletMap::iterator iter;

	iter = m_mapRidingPetTemplet.find( eId_ );

	if ( iter == m_mapRidingPetTemplet.end() )
		return NULL;

	RidingPetTemplet* pRidingPetTemplet = iter->second;

	return pRidingPetTemplet;
}
//#pragma endregion

//#pragma region Handler_EGS_SUMMON_RIDING_PET
void CX2RidingPetManager::Handler_EGS_SUMMON_RIDING_PET_REQ( UidType petUid )
{
	if( NULL != g_pInstanceData &&
		true == g_pInstanceData->GetSendGameStartReqPacket() )
		return;

	if( petUid < 0)
		return;

	if( false == CheckRidingStamina( petUid ) )
		return;

	if ( g_pData->GetServerProtocol() == NULL ||
#ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
        g_pData->GetServerProtocol()->IsUserProxyValid() == false
#else
		g_pData->GetServerProtocol()->GetUserProxy() == NULL
#endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
        )
		return;

	KEGS_SUMMON_RIDING_PET_REQ kPacket;
	kPacket.m_iRidingPetUID = petUid;

	g_pData->GetServerProtocol()->SendPacket( EGS_SUMMON_RIDING_PET_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_SUMMON_RIDING_PET_ACK, 60.f );
}

bool CX2RidingPetManager::Handler_EGS_SUMMON_RIDING_PET_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_SUMMON_RIDING_PET_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bIsDirectSummon = false;

	if( true == g_pMain->DeleteServerPacket( EGS_SUMMON_RIDING_PET_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
				{
					CX2SquareUnit* pMyUnit = NULL;
					if( NULL != g_pTFieldGame )
						pMyUnit = g_pTFieldGame->GetMyUnit();

					if( NULL != pMyUnit && NULL != pMyUnit->GetUnit() )
					{
						pMyUnit->ProcessRidingOn( true, kEvent.m_kRidingPetInfo );
						pMyUnit->GetUnit()->SetFullRidingPetInfo( kEvent.m_kRidingPetInfo );
						m_RidingPetUid = kEvent.m_kRidingPetInfo.m_iRidingPetUID;

						if ( NULL != m_pSkillSlot )
						{
							m_pSkillSlot->SetShowRidingSkillSlot(true);
						}
					}
				} break;

			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_TRAINING_GAME:
				{
					CX2GUUser* pMyGUUser = NULL;
					if( NULL != g_pX2Game )
						pMyGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetMyUnit() );
					if( NULL != pMyGUUser && NULL != pMyGUUser->GetUnit() )
					{
						pMyGUUser->ProcessRidingOn( true, kEvent.m_kRidingPetInfo );
						pMyGUUser->GetUnit()->SetFullRidingPetInfo( kEvent.m_kRidingPetInfo );
						m_RidingPetUid = kEvent.m_kRidingPetInfo.m_iRidingPetUID;

						if ( NULL != m_pSkillSlot )		// ��ų ��Ÿ��
						{
							RidingPetTemplet* pRidingPetTemplet = GetRidingPetTemplet( kEvent.m_kRidingPetInfo.m_usRindingPetID );
							if ( NULL != pRidingPetTemplet )
							{
								int iSlotNum = 0;
								for ( const RidingPetSkillInfo& SkilInfo : pRidingPetTemplet->m_vecAttackSkill )
								{
									m_pSkillSlot->SetSkillSlotCoolTime( iSlotNum++, SkilInfo.m_fCoolTime );
								}
							}
							m_pSkillSlot->SetShowRidingSkillSlot(true);
						}
					}
				} break;

			default:
				break;
			}

			// ��ȯ�� ���̵� ���� ���̵��� ����Ʈ �ֻ����� �ΰ� ������.
			for( sRidingPetInfo& kInfo : m_vecRidingPetList )
			{
				if( kInfo.m_iRidingPetUID == kEvent.m_kRidingPetInfo.m_iRidingPetUID )
				{
					kInfo.m_iPetListOrder = -1;
					SortRidingPetList();
					break;
				}
			}

			if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetUIManager()->GetUIPetList() )
			{
				CX2UIPetInfo* pPetInfo = g_pData->GetUIManager()->GetUIPetList();

				if( true == pPetInfo->GetShowRPInfo() )
				{
					if( m_RidingPetUid == pPetInfo->GetViewRidingPetID() )
						pPetInfo->SelectKindRideButton( CX2UIPetInfo::BUTTON_RIDE_OFF );
				}
				else if( true == pPetInfo->GetShowRPList() )
					pPetInfo->Select_Tab(CX2UIPetInfo::RPIT_LIST);
			}



			return true;
		}
	}

	return false;
}

bool CX2RidingPetManager::Handler_EGS_SUMMON_RIDING_PET_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SUMMON_RIDING_PET_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		 kEvent.m_iUnitUID != g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		KRidingPetInfo kRidingPetInfo;
		kRidingPetInfo.m_iRidingPetUID = kEvent.m_iRidingPetUID;
		kRidingPetInfo.m_usRindingPetID = kEvent.m_usRidingPetID;


		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP:
			{
				CreateRidingPet( kEvent.m_iUnitUID, kRidingPetInfo, D3DXVECTOR3( 0, 0, 0) );
			} break;

		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_TRAINING_GAME:
			{
				if ( NULL != g_pX2Game )
				{
					CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( kEvent.m_iUnitUID );
					if ( NULL != pUser )
					{
						pUser->ProcessRidingOn( true, kRidingPetInfo );
						pUser->GetUnit()->SetFullRidingPetInfo( kRidingPetInfo );
					}
				}
			} break;

		default:
			break;
		}
	}

	return true;
}
//#pragma endregion

//#pragma region Handler_EGS_CREATE_RIDING_PET
void CX2RidingPetManager::Handler_EGS_CREATE_RIDING_PET_REQ( const UidType _uId )
{
	KEGS_CREATE_RIDING_PET_REQ kPacket;
	kPacket.m_iItemUID = _uId;

	g_pData->GetServerProtocol()->SendPacket( EGS_CREATE_RIDING_PET_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CREATE_RIDING_PET_ACK, 60.f );
}

bool CX2RidingPetManager::Handler_EGS_CREATE_RIDING_PET_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_CREATE_RIDING_PET_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == g_pMain->DeleteServerPacket( EGS_CREATE_RIDING_PET_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		{

			if( NULL != g_pData )
			{
				// ���Ľ� �ֻ����� ����
				kEvent.m_kCreatedPetInfo.m_iLastUnSummonDate = -1;

				sRidingPetInfo sInfo = kEvent.m_kCreatedPetInfo;
				//����Ʈ�� �߰�
				m_vecRidingPetList.push_back( sInfo );
				SortRidingPetList();

				//Ȯ�� �˾�
				if( NULL != CX2RidingPetManager::GetInstance() )
				{
					CX2RidingPetManager::RidingPetTemplet *pTemplet = CX2RidingPetManager::GetInstance()->GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(kEvent.m_kCreatedPetInfo.m_usRindingPetID) );
					if( NULL != pTemplet)
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999.f, -999.f ), GET_REPLACED_STRING( ( STR_ID_24444, "L", pTemplet->m_wstrName )), g_pMain->GetNowState() );
				}


				//����� ������ ����
				if( NULL != g_pData->GetMyUser() &&
					NULL != g_pData->GetMyUser()->GetSelectUnit() )
					g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

				// �����ִ��� Ȯ�� �� ������Ʈ
				if( NULL != g_pData->GetUIManager() )
				{
					if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIInventory() &&
						g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_INVEN ) == true )
					{
						g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );
					}

					if( NULL != g_pData->GetUIManager()->GetUIPetList() &&
						true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_PET_LIST ) )
					{
						g_pData->GetUIManager()->GetUIPetList()->Select_Tab( CX2UIPetInfo::RPIT_LIST );
					}
				}
			}

			return true;
		}
	}

	return false;
}
//#pragma endregion

//#pragma region Handler_EGS_UNSUMMON_RIDING_PET
void CX2RidingPetManager::Handler_EGS_UNSUMMON_RIDING_PET_REQ( bool bIsDirect_ /*= false*/, UidType uidDirectPetId_ /*= -1*/ )
{
	if( NULL != g_pInstanceData &&
		true == g_pInstanceData->GetSendGameStartReqPacket() )
		return;

	if( NULL == GetMyRidingPet() )
		return;

	KEGS_UNSUMMON_RIDING_PET_REQ kPacket;

	sRidingPetInfo kInfo;
	for ( UINT i = 0; i < m_vecRidingPetList.size(); ++i ) //boost_foreach�� itrater�� �ٲ���
	{
		if( m_vecRidingPetList[i].m_iRidingPetUID == m_RidingPetUid )
		{
			kInfo = m_vecRidingPetList[i];
			break;
		}
	}

	kPacket.m_iRidingPetUID = kInfo.m_iRidingPetUID;
	kPacket.m_fStamina	= kInfo.m_fProtectStamina;

	if( false == m_bIsDirectSummon ) //ckdgksdl : if���� ���� ���� �ٲ���.
	{
		g_pData->GetServerProtocol()->SendPacket( EGS_UNSUMMON_RIDING_PET_REQ, kPacket );
		g_pMain->AddServerPacket( EGS_UNSUMMON_RIDING_PET_ACK, 60.f );

		m_bIsDirectSummon = bIsDirect_;
		m_DirectSummonUid = uidDirectPetId_;
	}
}

bool CX2RidingPetManager::Handler_EGS_UNSUMMON_RIDING_PET_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_UNSUMMON_RIDING_PET_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == g_pMain->DeleteServerPacket( EGS_UNSUMMON_RIDING_PET_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		{
			if ( NULL != m_pSkillSlot )
				m_pSkillSlot->SetShowRidingSkillSlot(false);

			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
				{
					CX2SquareUnit* pMyUnit = NULL;
					if( NULL != g_pTFieldGame )
						pMyUnit = g_pTFieldGame->GetMyUnit();

					if( NULL != pMyUnit && NULL != pMyUnit->GetUnit() )
					{
						pMyUnit->ProcessRidingOn( false );
						pMyUnit->GetUnit()->ClearRidingPetInfo();
					}
				}
				break;

			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_TRAINING_GAME:
				{
					CX2GUUser* pMyGUUser = NULL;
					if( NULL != g_pX2Game )
						pMyGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetMyUnit() );
					if( NULL != pMyGUUser && NULL != pMyGUUser->GetUnit() )
					{
						pMyGUUser->ProcessRidingOn( false );
						pMyGUUser->GetUnit()->ClearRidingPetInfo();
					}
				}
				break;

			default:
				break;
			}


			// ��ȯ������ ���̵� ���� ���̵��� ����Ʈ �ֻ����� �ΰ� ������.
			for( sRidingPetInfo& kInfo : m_vecRidingPetList )
			{
				if( kInfo.m_iRidingPetUID == m_RidingPetUid )
				{
					kInfo.m_iPetListOrder = -1;
					SortRidingPetList();
					break;
				}
			}

			if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetUIManager()->GetUIPetList() )
			{
				CX2UIPetInfo* pPetInfo = g_pData->GetUIManager()->GetUIPetList();

				if( true == pPetInfo->GetShowRPInfo() )
				{
					if( m_RidingPetUid == pPetInfo->GetViewRidingPetID() )
						pPetInfo->SelectKindRideButton( CX2UIPetInfo::BUTTON_RIDE_ON );
				}
				else if( true == pPetInfo->GetShowRPList() )
					pPetInfo->Select_Tab(CX2UIPetInfo::RPIT_LIST);
			}


			m_RidingPetUid = -1;

			if( true == m_bIsDirectSummon && 0 < m_DirectSummonUid )
			{
				Handler_EGS_SUMMON_RIDING_PET_REQ( m_DirectSummonUid );
			}

			return true;
		}
	}

	return false;
}

bool CX2RidingPetManager::Handler_EGS_UNSUMMON_RIDING_PET_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_SUMMON_RIDING_PET_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( 0 > kEvent.m_iUnitUID )
		return false;

	const UidType uidUser = kEvent.m_iUnitUID;

	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		uidUser != g_pData->GetMyUser()->GetSelectUnit()->GetUID() )
	{
		switch( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP:
			{
				CX2SquareUnit* pSquareUnit = NULL;

				if( NULL != g_pTFieldGame )
					pSquareUnit = g_pTFieldGame->GetSquareUnitByUID( uidUser );

				if( NULL != pSquareUnit && NULL != pSquareUnit->GetUnit() )
				{
					pSquareUnit->ProcessRidingOn( false );
					pSquareUnit->GetUnit()->ClearRidingPetInfo();
				}
			}
			break;

		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_TRAINING_GAME:
			{
				CX2GUUser* pGUUser = NULL;
				if( NULL != g_pX2Game )
					pGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( uidUser ) );

				if( NULL != pGUUser && NULL != pGUUser->GetUnit() )
				{
					pGUUser->ProcessRidingOn( false );
					pGUUser->GetUnit()->ClearRidingPetInfo();
				}
			}
			break;

		default:
			break;
		}
	}
	return true;
}
//#pragma endregion

//#pragma region Handler_EGS_RELEASE_RIDING_PET
void CX2RidingPetManager::Handler_EGS_RELEASE_RIDING_PET_REQ( UidType _uId )
{
	KEGS_RELEASE_RIDING_PET_REQ kPacket;
	kPacket.m_iRidingPetUID = _uId;

	// �⺻�� ����
	kPacket.m_bCEventQuest = false;

	// ���̵� �� ����Ʈ���� ���� �� ����Ʈ ����
	sRidingPetInfo kInfo;
	for ( UINT i = 0; i < m_vecRidingPetList.size(); ++i ) //boost_foreach�� itrater�� �ٲ���
	{
		if( m_vecRidingPetList[i].m_iRidingPetUID == _uId )
		{
			kInfo = m_vecRidingPetList[i];
			break;
		}
	}
	bool bCEventQuest1 = true;
	bool bCEventQuest2 = true;

	// �̺�Ʈ�� �� ���� Ȯ��
	if(wcscmp(kInfo.m_wstrDestroyDate.c_str(), L"") == 0)
		bCEventQuest1 = false;
	else
		bCEventQuest1 = true;

	// ����Ʈ ���� ���� Ȯ�� (questID : 74590)
	CX2QuestManager::QuestInst* pQuestInst = NULL;
	pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( _CONST_AEVENT_RIDING_WITH_SUB_QUEST::iHasEventQuestID );

	if ( pQuestInst == NULL )
	{
		bCEventQuest2 = false;
	}
	else
	{
		bCEventQuest2 = true;
	}

	if( (bCEventQuest1 == true) && (bCEventQuest2 == true) )
	{
		kPacket.m_bCEventQuest = true;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_RELEASE_RIDING_PET_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RELEASE_RIDING_PET_ACK, 60.f );
}

bool CX2RidingPetManager::Handler_EGS_RELEASE_RIDING_PET_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_RELEASE_RIDING_PET_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == g_pMain->DeleteServerPacket( EGS_RELEASE_RIDING_PET_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
		{

			std::vector<sRidingPetInfo>::iterator itr;
			for( itr = m_vecRidingPetList.begin(); itr != m_vecRidingPetList.end(); )
			{
				if( itr->m_iRidingPetUID == kEvent.m_iRidingPetUID )
				{
					itr = m_vecRidingPetList.erase(itr);
					break;
				}
				else
					++itr;
			}

			if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetUIManager()->GetUIPetList() )
				g_pData->GetUIManager()->GetUIPetList()->Select_Tab( CX2UIPetInfo::RPIT_LIST );

			return true;
		}
	}

	return false;
}

bool CX2RidingPetManager::Handler_EGS_RELEASE_RIDING_PET_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>( lParam );
	KEGS_RELEASE_RIDING_PET_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	RidingPetTemplet* pRidingPetTemplet = GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>( kEvent.m_usRindingPetID ) );
	if( NULL != pRidingPetTemplet )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_195, "L", pRidingPetTemplet->m_wstrName ) ), g_pMain->GetNowState() );
	}


	if( m_RidingPetUid == kEvent.m_iRidingPetUID )
	{
		if ( NULL != g_pData && NULL != g_pData->GetUIManager() )
		{
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot(false);
			g_pData->GetUIManager()->SetShowSkillSlot(true);
		}

		switch( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP:
			{
				CX2SquareUnit* pMyUnit = NULL;
				if( NULL != g_pTFieldGame )
					pMyUnit = g_pTFieldGame->GetMyUnit();

				if( NULL != pMyUnit && NULL != pMyUnit->GetUnit() )
				{
					pMyUnit->ProcessRidingOn( false );
					pMyUnit->GetUnit()->ClearRidingPetInfo();
				}
			}
			break;

		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_TRAINING_GAME:
			{
				CX2GUUser* pMyGUUser = NULL;
				if( NULL != g_pX2Game )
					pMyGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetMyUnit() );
				if( NULL != pMyGUUser && NULL != pMyGUUser->GetUnit() )
				{
					pMyGUUser->ProcessRidingOn( false );
					pMyGUUser->GetUnit()->ClearRidingPetInfo();
				}
			}
			break;

		default:
			break;
		}
	}

	DeleteRidingPetOnList( kEvent.m_iRidingPetUID );

	if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUIPetList() &&
		true == g_pData->GetUIManager()->GetUIPetList()->GetShow() )
		g_pData->GetUIManager()->GetUIPetList()->Select_Tab( CX2UIPetInfo::RPIT_LIST );

	return true;
}

//#pragma endregion
UidType CX2RidingPetManager::GetFirstRidingPetUID()
{
	if( false == GetHasRidingPet() )
		return -1;

	SortRidingPetList();

	// 0�� ��ȿ������ ���� üũ�� GetHasRidingPet()���� ó����
	return m_vecRidingPetList[0].m_iRidingPetUID;
}
/** @function : SortRidingPetList
	@brief : �� ����Ʈ ����
			 �ֱ� ���� ������ ����
*/
void CX2RidingPetManager::SortRidingPetList()
{
	// UnSummonData�� ����� 1�� ����
	// ���� ���� ������ ������ ����
	auto UnSummonDateSortFunc = [](const sRidingPetInfo& lhs_, const sRidingPetInfo& rhs_ )->bool
	{
		return rhs_.m_iPetListOrder > lhs_.m_iPetListOrder ;
	};

	std::vector< sRidingPetInfo > vecTempRidingPetList(m_vecRidingPetList);
	std::sort( vecTempRidingPetList.begin(), vecTempRidingPetList.end(), UnSummonDateSortFunc );

	// Date���� Index�� ����
	int iOrder = 0;
	for( sRidingPetInfo& RidingPet : vecTempRidingPetList )
	{
		RidingPet.m_iPetListOrder = iOrder++;
	}

	m_vecRidingPetList.clear();
	// Ȱ��ȭ ������ ���� ���� �켱������ ����
	for( sRidingPetInfo RidingPet : vecTempRidingPetList )
	{
		if( RidingPet.m_fProtectStamina >= m_fEnableStamina )
			m_vecRidingPetList.push_back( RidingPet );
	}

	// ��� Ȱ��ȭ ���¶�� ��Ȱ��ȭ üũ ���� �ʱ�
	if ( m_vecRidingPetList.size() == vecTempRidingPetList.size() )
		return;

	for( sRidingPetInfo RidingPet : vecTempRidingPetList )
	{
		if( RidingPet.m_fProtectStamina < m_fEnableStamina )
			m_vecRidingPetList.push_back( RidingPet );
	}
}
/** @function : FullChargeRidingStamina
	@brief : ���̵� �� ���׹̳� ���� �Լ�.
			 ������ ġƮ�� �Լ�
*/
void CX2RidingPetManager::FullChargeRidingStamina( int iValue_ )
{
	if( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_DEV )
	{
		return;
	}

	for( sRidingPetInfo& RidingPet : m_vecRidingPetList )
	{
		if( iValue_ > 0 )
			RidingPet.m_fProtectStamina = static_cast<float>(iValue_);
		else
		{
			RidingPetTempletMap::iterator iterTempletMap =	m_mapRidingPetTemplet.find( RidingPet.m_usRindingPetID );
			if ( iterTempletMap != m_mapRidingPetTemplet.end() )
			{
				RidingPet.m_fProtectStamina = iterTempletMap->second->m_fMaxStamina;
			}
		}
	}
}

void CX2RidingPetManager::UpdateRidingPetGage()
{
	if ( m_ElapsedTimeDecrease.CheckElapsedTime() && ( g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD ||
		 g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ) )
	{
		vector<sRidingPetInfo>::iterator iter;

		for ( iter = m_vecRidingPetList.begin(); iter != m_vecRidingPetList.end(); ++iter )
		{
			iter->Verify();
			float fMaxStamina = 0.f;
			float fRecoveryValue = 1.f;
			RidingPetTempletMap::iterator iterTempletMap;
			iterTempletMap = m_mapRidingPetTemplet.find( iter->m_usRindingPetID );
			if ( iterTempletMap != m_mapRidingPetTemplet.end() )
			{
				fMaxStamina = iterTempletMap->second->m_fMaxStamina;
				fRecoveryValue *= iterTempletMap->second->m_fRecoveryRate;
			}

			if ( m_RidingPetUid > -1 && iter->m_iRidingPetUID == m_RidingPetUid ) // ������ �Ҹ�
			{
				CX2GUUser* pUser = NULL;
				if( NULL != g_pX2Game )
					pUser = g_pX2Game->GetMyUnit();

				if( NULL != pUser )
				{
					// �޽����̶�� ���׹̳� ȸ��
					if( true == pUser->IsApplyBuffByBuffTempletID( BTI_CALM_REST ))
					{
						if ( iter->m_fProtectStamina < fMaxStamina )
							iter->m_fProtectStamina += 1 * fRecoveryValue;

						if ( iter->m_fProtectStamina >= fMaxStamina
							|| iter->m_fProtectStamina < 0.0f )	/// ����ö // 2013-07-11 // ���̳ʽ��� ���� ����ó�� �߰�
							iter->m_fProtectStamina = fMaxStamina;
					}
					else
					{
						iter->m_fProtectStamina -= 1;
						if ( iter->m_fProtectStamina < 0 )
							iter->m_fProtectStamina = 0;
					}

					if( m_fUpdateNowStamina > 0.0f )
					{
						// ��Ȯ�� ��ġ ������ ���ؼ�
						iter->m_fProtectStamina = static_cast<int>(m_fUpdateNowStamina);
						m_fUpdateNowStamina = 0.0f;
					}
					// ���¹̳� ���� ����
					m_pSkillSlot->UpdateStamina( iter->m_fProtectStamina, fMaxStamina );
					iter->m_fStamina = iter->m_fProtectStamina ;

					// ���¹̳ʰ� �����Ǹ� UNSUMMON
					if ( iter->m_fProtectStamina <= 0 || pUser->GetNowHp() <= 0.f )
						Handler_EGS_UNSUMMON_RIDING_PET_REQ();
				}
			}
		}
		m_ElapsedTimeDecrease.ResetSumOfElapsedTime();
	}
	else if ( m_ElapsedTimeRecovery.CheckElapsedTime() )
	{
		vector<sRidingPetInfo>::iterator iter;
		for ( iter = m_vecRidingPetList.begin(); iter != m_vecRidingPetList.end(); ++iter )
		{
			iter->Verify();
			float fMaxStamina = 0;
			float fRecoveryValue = 1.f;
			RidingPetTempletMap::iterator iterTempletMap;
			iterTempletMap = m_mapRidingPetTemplet.find( iter->m_usRindingPetID );
			if ( iterTempletMap != m_mapRidingPetTemplet.end() )
			{
				fMaxStamina = iterTempletMap->second->m_fMaxStamina;
				fRecoveryValue *= iterTempletMap->second->m_fRecoveryRate;
			}

			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
				{
					if ( iter->m_fProtectStamina < fMaxStamina )
					{
						iter->m_fProtectStamina += 1 * fRecoveryValue;
					}

					if ( iter->m_fProtectStamina >= fMaxStamina
						|| iter->m_fProtectStamina < 0.0f )	/// ����ö // 2013-07-11 // ���̳ʽ��� ���� ����ó�� �߰�
						iter->m_fProtectStamina = fMaxStamina;

					iter->m_fStamina = iter->m_fProtectStamina ;
				} break;
			default:
				{
					if ( iter->m_iRidingPetUID != m_RidingPetUid ) // ������ ȸ��
					{
						if ( iter->m_fProtectStamina < fMaxStamina )
							iter->m_fProtectStamina += 1 * fRecoveryValue;

						if ( iter->m_fProtectStamina >= fMaxStamina
							|| iter->m_fProtectStamina < 0.0f )		/// ����ö // 2013-07-11 // ���̳ʽ��� ���� ����ó�� �߰�
							iter->m_fProtectStamina = fMaxStamina;

						iter->m_fStamina = iter->m_fProtectStamina ;
					}
				}
				break;
			}

			if( iter->m_iRidingPetUID == m_RidingPetUid ) //��ų ���� ����ȭ
			{
				m_pSkillSlot->UpdateStamina( iter->m_fProtectStamina, fMaxStamina );
			}
		}
		m_ElapsedTimeRecovery.ResetSumOfElapsedTime();
	}
}

bool CX2RidingPetManager::GetRidingOn()
{
	bool Is_Ride = false;

	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		{
			CX2SquareUnit* pMyUnit = NULL;
			if( NULL != g_pTFieldGame )
				pMyUnit = g_pTFieldGame->GetMyUnit();

			if( NULL != pMyUnit && NULL != pMyUnit->GetUnit() )
			{
				Is_Ride = pMyUnit->GetRidingOn();
			}
		}
		break;

	case CX2Main::XS_BATTLE_FIELD:
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_TRAINING_GAME:
		{
			CX2GUUser* pMyGUUser = NULL;
			if( NULL != g_pX2Game )
				pMyGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetMyUnit() );
			if( NULL != pMyGUUser && NULL != pMyGUUser->GetUnit() )
			{
				Is_Ride = pMyGUUser->GetRidingOn();
			}
		}
		break;
	default:
		break;
	}

	return Is_Ride;
}

bool CX2RidingPetManager::GetRidingOnState()
{
	bool Is_Riding_State = false;

	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		{
			CX2SquareUnit* pMyUnit = NULL;
			if( NULL != g_pTFieldGame )
				pMyUnit = g_pTFieldGame->GetMyUnit();

			if( NULL != pMyUnit && NULL != pMyUnit->GetUnit() )
			{
				if ( pMyUnit->GetNowState() == pMyUnit->GetStateID( "WAIT") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "WALK") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "DASH") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "RIDING_WAIT") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "RIDING_WAIT_HABIT") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "RIDING_WALK") ||
					 pMyUnit->GetNowState() == pMyUnit->GetStateID( "RIDING_DASH") )
					Is_Riding_State = true;
			}
		}
		break;

	case CX2Main::XS_BATTLE_FIELD:
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_TRAINING_GAME:
		{
			CX2GUUser* pMyGUUser = NULL;
			if( NULL != g_pX2Game )
				pMyGUUser = static_cast<CX2GUUser*>( g_pX2Game->GetMyUnit() );
			if( NULL != pMyGUUser && NULL != pMyGUUser->GetUnit() )
			{
				if ( true == pMyGUUser->CanRidingState() )
					Is_Riding_State = true;
			}
		}
		break;
	default:
		break;
	}

	return Is_Riding_State;
}

void CX2RidingPetManager::DeleteRidingPetOnList( UidType deleteUid_ )
{
	vector<sRidingPetInfo>::iterator iter;
	for ( iter = m_vecRidingPetList.begin(); iter != m_vecRidingPetList.end();)
	{
		if( iter->m_iRidingPetUID == deleteUid_ )
		{
			iter = m_vecRidingPetList.erase(iter);
			break;
		}
		else
			++iter;
	}

	//SortRidingPetList();
}

bool CX2RidingPetManager::CheckRidingStamina( UidType RidingPetUid_ )
{
	bool bAvailableToRiding = false;

	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_BATTLE_FIELD:
	case CX2Main::XS_DUNGEON_GAME:
		{
			//�ش� Ż���� ���׹̳� üũ
			vector<sRidingPetInfo>::iterator iter;
			for ( iter = m_vecRidingPetList.begin(); iter != m_vecRidingPetList.end(); ++iter )
			{
				if ( RidingPetUid_ == iter->m_iRidingPetUID )
				{
					if( iter->m_fProtectStamina >= m_fEnableStamina )
						bAvailableToRiding = true;
					else
					{
						/// ���� ���̵� ���� ���̵� �� ���ø� ��ȯ
						CX2RidingPetManager::RidingPetTemplet* pRidingPetTemplet =
							GetRidingPetTemplet( static_cast<CX2RidingPetManager::RIDING_PET_UNIT_ID>(iter->m_usRindingPetID) );
						if ( NULL != pRidingPetTemplet )
						{
							/// ���׹̳��� ��� Ż �� ���ٴ� �޽��� ���
							if( NULL != g_pX2Game )
							{
								const wstring wstrRidingPetName = GET_REPLACED_STRING( ( STR_ID_24529, "L", pRidingPetTemplet->m_wstrName ) );	/// ���̵� �� �̸�
								g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, wstrRidingPetName.c_str(), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1),
									DT_CENTER, 1.f, 1.f );
							}
						}
					}
				}
			}
		} break;
	case CX2Main::XS_TRAINING_GAME:
	case CX2Main::XS_VILLAGE_MAP:
		{// ����, �����Ʒ����� üũ�����ʰ� ������ ok
			bAvailableToRiding = true;
		}
		break;
	default:
		break;
	}

	return bAvailableToRiding;
}

void CX2RidingPetManager::SetEnableStamina_LUA( float _EnableStamina )
{
	m_fEnableStamina = _EnableStamina;
}

bool CX2RidingPetManager::GetRidingPetInfoByUID( IN UidType uiRidingPetUID, OUT sRidingPetInfo& RidingPetInfo_ )
{
	for( sRidingPetInfo sPetInfo : m_vecRidingPetList )
	{
		if( sPetInfo.m_iRidingPetUID == uiRidingPetUID )
		{
			RidingPetInfo_ = sPetInfo;
			return true;
			break;
		}
	}

	return  false;
}
float CX2RidingPetManager::GetRecoveryTimePerOne()
{
	return lua_tinker::call<float>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), "GET_RECOVERY_TIME_PER_ONE" );
}

#if X2_DEV_TOOLS
void CX2RidingPetManager::RefreshRidingPetScript()
{
	GetInstance()->OpenScriptFile( L"RidingPetTemplet.lua" );
}
#endif // REALTIME_SCRIPT_PATCH


/** @function : IsValideRiding
	@brief : Ż �� ž�� ���°� ��ȿ������ ���� �˻�
	@param :
	@return : false : ž�� ���°� �ƴϰų�, ž�� ���� ��ȿ �� ��
			  true  : ��ȿ���� ���� ��
			  */
bool CX2RidingPetManager::IsInvalideRiding() const
{
	if( NULL == g_pX2Game )
		return false;

	if( NULL != g_pX2Game->GetMyUnit() &&
		true == g_pX2Game->GetMyUnit()->GetRidingOn() )
	{
		if( -1 == m_RidingPetUid )
			return true;

		if( true == m_vecRidingPetList.empty() )
			return true;
	}

	return false;
}
/** @function : UnsummonRidingPet
	@brief : Ż �� ������ ó��
*/
void CX2RidingPetManager::UnsummonRidingPet()
{
	// ���� ž�� ���°� ���������� ���� ��쿡 ���� ó��
	if( true == IsInvalideRiding() )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetMyUnit() &&
			true == g_pX2Game->GetMyUnit()->GetRidingOn() )
		{
			if( -1 != m_RidingPetUid )
				Handler_EGS_UNSUMMON_RIDING_PET_REQ();
			else
			{
				g_pX2Game->GetMyUnit()->ProcessRidingOn( false );
				if( NULL != g_pX2Game->GetMyUnit()->GetUnit() )
					g_pX2Game->GetMyUnit()->GetUnit()->ClearRidingPetInfo();
			}
		}
	}
}

/** @function : SetRidingPetStaminaPercentUP
	@brief : Ż�� ���� ���¹̳� �۾� (%)
*/
bool CX2RidingPetManager::SetRidingPetStaminaPercentUP(int iValue_ )
{
	std::vector< sRidingPetInfo > vecRidingPetList;
	GetRidingPetList( vecRidingPetList );
	std::vector<sRidingPetInfo>::iterator iter;

	for ( iter = vecRidingPetList.begin(); iter != vecRidingPetList.end(); ++iter )
	{
		if ( m_RidingPetUid > -1 && iter->m_iRidingPetUID == m_RidingPetUid ) // ������ �Ҹ�
		{
			iter->Verify();
			float fMaxStamina = 0.f;
			float fNowStamina = 0.f;
			float fValue = 1.f;

			RidingPetTempletMap::iterator iterTempletMap;
			iterTempletMap = m_mapRidingPetTemplet.find( iter->m_usRindingPetID );

			fValue = static_cast<float>(iValue_) / 100.0f;
			fNowStamina = iter->m_fProtectStamina;
			fMaxStamina = iterTempletMap->second->m_fMaxStamina;
			fNowStamina += static_cast<int>(fMaxStamina * fValue);

			// ���׹̳� ����
			if( fNowStamina > fMaxStamina || fNowStamina < 0.0f)
			{
				fNowStamina = fMaxStamina;
			}

			iter->m_fProtectStamina = fNowStamina;
			// ���¹̳� �� ���� ����
			m_fUpdateNowStamina = fNowStamina;

			return true;
		}
	}
	return false;
}
