#include "stdafx.h"

#include "./X2SceneLoading.h"

namespace
{
	// The bar's picture spans the 1024 reference units of the layout, as in CX2StateLoading.
	const float	BAR_FULL_LENGTH = 1024.f;
	// At least this long on screen, so a quick entry does not flash.
	const float	MIN_SHOW_TIME = 0.5f;
	// Everything must stay ready this long and this many frames of the scene be drawn behind the screen, so the
	// uploads were submitted and the scene's pipelines built before it shows.
	const float	READY_HOLD_TIME = 0.15f;
	const int	READY_FRAMES = 3;
	// Closes anyway after this long (a load that never ends, an ACK lost), with a log line.
	const float	MAX_WAIT_TIME = 30.f;
	const float	FADE_OUT_TIME = 0.3f;
	// Share of the bar for the background loads; the rest fills when the server confirmed the entry.
	const float	LOADS_SHARE = 0.85f;

	// A destination's background: two 512x768 halves of a 4:3 image (the original's art), or one image when pRight
	// is NULL, 4:3 (drawn over the 1024 column) or 16:9 (T51: 1365.33 units wide centered on it; its sides fall off
	// narrower screens).
	struct Background
	{
		const WCHAR*	pLeft;
		const WCHAR*	pRight;
		bool			b16x9;
	};

	// The pictures of the original's warp and channel change screen (CX2TFieldGame::LoadFieldImage) and of the raid
	// fields' (CX2BossRaidManager::GetLoadingUIFileName, the same picture for every raid).
	const Background* FindVillageBackground( SEnum::VILLAGE_MAP_ID eVillage )
	{
		static const struct { SEnum::VILLAGE_MAP_ID eVillage; Background kBackground; } s_aVillages[] =
		{
			{ SEnum::VMI_RUBEN,		{ L"HQ_LoadingVillage_RUBEN1.TGA",			L"HQ_LoadingVillage_RUBEN2.TGA", false } },
			{ SEnum::VMI_ELDER,		{ L"HQ_LoadingVillage_ELDER1.TGA",			L"HQ_LoadingVillage_ELDER2.TGA", false } },
			{ SEnum::VMI_BESMA,		{ L"HQ_LoadingVillage_BESMA1.TGA",			L"HQ_LoadingVillage_BESMA2.TGA", false } },
			{ SEnum::VMI_ALTERA,	{ L"HQ_LoadingVillage_ALTERA1.TGA",			L"HQ_LoadingVillage_ALTERA2.TGA", false } },
			{ SEnum::VMI_VELDER,	{ L"HQ_LoadingVillage_VELDER1.TGA",			L"HQ_LoadingVillage_VELDER2.TGA", false } },
			{ SEnum::VMI_HAMEL,		{ L"HQ_LoadingVillage_HAMEL1.TGA",			L"HQ_LoadingVillage_HAMEL2.TGA", false } },
			{ SEnum::VMI_SANDER,	{ L"HQ_LoadingVillage_SANDER_01.TGA",		L"HQ_LoadingVillage_SANDER_02.TGA", false } },
			{ SEnum::VMI_PEITA,		{ L"HQ_LoadingDungeonGate_FEITA1.TGA",		L"HQ_LoadingDungeonGate_FEITA2.TGA", false } },
			{ SEnum::VMI_BATTLE_FIELD_ELDER_REST_00,	{ L"HQ_Loading_ELDER_REST_01.TGA",	L"HQ_Loading_ELDER_REST_02.TGA", false } },
			{ SEnum::VMI_BATTLE_FIELD_BESMA_REST_00,	{ L"HQ_Loading_BESMA_REST_01.tga",	L"HQ_Loading_BESMA_REST_02.tga", false } },
			{ SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00,	{ L"HQ_Loading_ALTERA_REST_01.tga",	L"HQ_Loading_ALTERA_REST_02.tga", false } },
			{ SEnum::VMI_BATTLE_FIELD_VELDER_REST_00,	{ L"HQ_Loading_VELDER_REST_01.tga",	L"HQ_Loading_VELDER_REST_02.tga", false } },
			{ SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00,	{ L"HQ_Loading_HAMEL_REST_01.tga",	L"HQ_Loading_HAMEL_REST_02.tga", false } },
			{ SEnum::VMI_BATTLE_FIELD_SANDER_REST_00,	{ L"HQ_Loading_SANDER_REST_01.tga",	L"HQ_Loading_SANDER_REST_02.tga", false } },
		};
		for( const auto& village : s_aVillages )
			if( village.eVillage == eVillage )
				return &village.kBackground;
		return NULL;
	}
	const Background s_kChannelChange	= { L"Channel_A.tga",						L"Channel_B.tga",	false };
	const Background s_kBossRaidField	= { L"HQ_LoadingField_Boss_Gate_01.tga",	L"HQ_LoadingField_Boss_Gate_02.tga",	false };

	// Destinations without art of their own draw one of the generic loading pictures, at random (user, 2026-11-16).
	// They are 1024x512 textures drawn stretched to 1024x768, as the dungeon loading screen does; HQ_LoadingBasic and
	// _2 are copies of _1, _111_2 of _111_1.
	const Background s_aGeneric[] =
	{
		{ L"HQ_LoadingBasic_1.tga",		NULL, false },	{ L"HQ_LoadingBasic_3.tga",		NULL, false },
		{ L"HQ_LoadingBasic_111.tga",	NULL, false },	{ L"HQ_LoadingBasic_111_1.tga",	NULL, false },
		{ L"HQ_LoadingBasic_222.tga",	NULL, false },	{ L"HQ_LoadingBasic_333.tga",	NULL, false },
		{ L"HQ_LoadingBasic_444.tga",	NULL, false },	{ L"HQ_LoadingBasic_555.tga",	NULL, false },
		{ L"HQ_LoadingBasic_666.tga",	NULL, false },	{ L"HQ_LoadingBasic_777.tga",	NULL, false },
		{ L"HQ_LoadingBasic_888.tga",	NULL, false },	{ L"HQ_LoadingBasic_999.tga",	NULL, false },
	};
}

CX2SceneLoading::CX2SceneLoading()
	: m_eStep( STEP_IDLE )
	, m_pDialog( NULL )
	, m_pTextureBlack( NULL )
	, m_uFinishedAtBegin( 0 )
	, m_uLastQueued( 0 )
	, m_fTime( 0.f )
	, m_fReadyTime( 0.f )
	, m_iReadyFrames( 0 )
	, m_fShownProgress( 0.f )
	, m_fAlpha( 1.f )
{
}

CX2SceneLoading::~CX2SceneLoading()
{
	Close();
	SAFE_CLOSE( m_pTextureBlack );
}

void CX2SceneLoading::CreateDialog()
{
	if( m_pDialog != NULL )
		SAFE_DELETE_DIALOG_HANDLE( m_pDialog );
	m_pDialog = new CKTDGUIDialog( this, L"DLG_First_Loading_Front.lua", 0.0f );
	if( m_pDialog == NULL )
		return;
	// the first login's client version is not repeated here
	if( CKTDGUIControl* pVersion = m_pDialog->GetControl( L"Static_Client_Version" ) )
		pVersion->SetShow( false );
}

void CX2SceneLoading::Begin()
{
	// a fresh dialog each time, so a background set for the last destination does not stay
	Close();
	CreateDialog();
	if( m_pDialog == NULL )
		return;
	SetBackground();
	if( m_pTextureBlack == NULL )
		m_pTextureBlack = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"FadeInOut.dds" );

	uint32_t uQueued = 0;
	g_pKTDXApp->GetDeviceManager()->GetPendingLoadCounts( uQueued, m_uFinishedAtBegin );
	m_uLastQueued = uQueued;

	m_eStep = STEP_LOADING;
	m_fTime = 0.f;
	m_fReadyTime = 0.f;
	m_iReadyFrames = 0;
	m_fShownProgress = 0.f;
	m_fAlpha = 1.f;
	SetProgress( 0.f );

	// Before the caller's synchronous loading: otherwise the last frame of the previous scene stays up meanwhile.
	g_pKTDXApp->RenderStageNow( this );
}

// The destination is known at Begin: the location manager has the village before CX2StateField is built, the field
// manager the field before CX2StateBattleField is (and before a field-to-field move), and the channel change flag is
// spent only while the village is built. A destination without art gets a generic picture; if a texture is missing,
// the dialog keeps its own (the first login's).
void CX2SceneLoading::SetBackground()
{
	const Background* pBackground = NULL;
	if( g_pInstanceData != NULL && g_pInstanceData->IsChangeChannel() )
		pBackground = &s_kChannelChange;
	else if( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
		pBackground = FindVillageBackground( g_pData->GetLocationManager()->GetCurrentVillageID() );
	else if( g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD )
	{
		const CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
		if( battleFieldManager.GetIsBossRaidFieldByFieldID( battleFieldManager.GetBattleFieldIdWhereIam() ) )
			pBackground = &s_kBossRaidField;
	}
	if( pBackground == NULL )
	{
		static std::mt19937 s_random( std::random_device{}() );
		pBackground = &s_aGeneric[ std::uniform_int_distribution<size_t>( 0, ARRAY_SIZE( s_aGeneric ) - 1 )( s_random ) ];
	}

	CKTDGUIStatic* pBack = m_pDialog->GetStatic_LUA( "First_Loading_Back" );
	CKTDGUIControl::CPictureData* pLeft = pBack != NULL ? pBack->GetPicture( 0 ) : NULL;
	CKTDGUIControl::CPictureData* pRight = pBack != NULL ? pBack->GetPicture( 1 ) : NULL;	// none once it is 16:9
	if( pLeft == NULL )
		return;

	bool bSet = false;
	if( pBackground->pRight == NULL )
	{
		bSet = pLeft->SetTex( pBackground->pLeft );
		if( bSet )
		{
			const float fWidth = pBackground->b16x9 ? 768.f * 16.f / 9.f : 1024.f;
			pLeft->SetPos( D3DXVECTOR2( ( 1024.f - fWidth ) / 2.f, 0.f ) );
			pLeft->SetSize( D3DXVECTOR2( fWidth, 768.f ) );
			if( pRight != NULL )
				pRight->SetShow( false );
		}
	}
	else if( pRight != NULL )
		bSet = pLeft->SetTex( pBackground->pLeft ) && pRight->SetTex( pBackground->pRight );

	// a missing picture: the first login's background whole, not half of each
	if( bSet == false )
		CreateDialog();
}

bool CX2SceneLoading::IsCoveredState() const
{
	return g_pMain != NULL
		&& ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP || g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD );
}

// The state's own entry is done: the village sent EGS_FIELD_LOADING_COMPLETE_REQ (on its first frame) and got the
// ACK; the field got EGS_GAME_LOADING_ACK.
bool CX2SceneLoading::IsStateReady() const
{
	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		return g_pTFieldGame != NULL && g_pTFieldGame->GetLoadComplete()
			&& g_pMain->IsWaitingServerPacket( EGS_FIELD_LOADING_COMPLETE_ACK ) == false;
	case CX2Main::XS_BATTLE_FIELD:
		return g_pX2Game != NULL && g_pMain->IsWaitingServerPacket( EGS_GAME_LOADING_ACK ) == false;
	default:
		return true;
	}
}

HRESULT CX2SceneLoading::OnFrameMove( double fTime, float fElapsedTime )
{
	if( m_eStep == STEP_IDLE )
		return S_OK;

	// Left for a screen it does not cover (a disconnection, a dungeon): nothing to wait for.
	if( IsCoveredState() == false )
	{
		Close();
		return S_OK;
	}

	m_fTime += fElapsedTime;

	if( m_eStep == STEP_LOADING )
	{
		uint32_t uQueued = 0, uFinished = 0;
		g_pKTDXApp->GetDeviceManager()->GetPendingLoadCounts( uQueued, uFinished );
		const bool bLoadsIdle = uQueued == uFinished && uQueued == m_uLastQueued;
		m_uLastQueued = uQueued;
		const bool bStateReady = IsStateReady();

		const uint32_t uTotal = uQueued - m_uFinishedAtBegin;
		const float fLoads = uTotal > 0 ? static_cast<float>( uFinished - m_uFinishedAtBegin ) / uTotal : 1.f;
		float fTarget = LOADS_SHARE * fLoads + ( bStateReady ? 1.f - LOADS_SHARE : 0.f );

		if( bLoadsIdle && bStateReady && m_fTime >= MIN_SHOW_TIME )
		{
			m_fReadyTime += fElapsedTime;
			fTarget = 1.f;
		}
		else
		{
			m_fReadyTime = 0.f;
			m_iReadyFrames = 0;
			fTarget = std::min( fTarget, 0.99f );
		}

		if( fTarget > m_fShownProgress )
			m_fShownProgress += ( fTarget - m_fShownProgress ) * std::min( 1.f, fElapsedTime * 8.f );
		SetProgress( m_fReadyTime > 0.f ? 1.f : m_fShownProgress );

		if( m_fReadyTime >= READY_HOLD_TIME && m_iReadyFrames >= READY_FRAMES )
		{
			m_eStep = STEP_FADE_OUT;
		}
		else if( m_fTime >= MAX_WAIT_TIME )
		{
			dbg::clog << L"Scene loading: closed after " << MAX_WAIT_TIME << L" s (loads " << uFinished - m_uFinishedAtBegin
				<< L"/" << uTotal << L", state " << ( bStateReady ? L"ready" : L"not ready" ) << L")" << dbg::endl;
			m_eStep = STEP_FADE_OUT;
		}
	}
	else if( m_eStep == STEP_FADE_OUT )
	{
		m_fAlpha -= fElapsedTime / FADE_OUT_TIME;
		if( m_fAlpha <= 0.f )
		{
			Close();
			return S_OK;
		}
	}

	if( m_pDialog != NULL )
	{
		m_pDialog->SetColor( D3DXCOLOR( 1.f, 1.f, 1.f, m_fAlpha ) );
		m_pDialog->OnFrameMove( fTime, fElapsedTime );
	}
	return S_OK;
}

HRESULT CX2SceneLoading::OnFrameRender()
{
	if( m_eStep == STEP_IDLE )
		return S_OK;

	if( m_eStep == STEP_LOADING && m_fReadyTime > 0.f )
		m_iReadyFrames++;

	KD3DBEGIN()
		if( m_pTextureBlack != NULL )
		{
			int iX, iY, iWidth, iHeight;
			g_pKTDXApp->GetUICanvasRect( iX, iY, iWidth, iHeight );
			m_pTextureBlack->Draw( iX, iY, iWidth, iHeight, D3DXCOLOR( 1.f, 1.f, 1.f, m_fAlpha ) );
		}
		if( m_pDialog != NULL )
			m_pDialog->OnFrameRender();
	KD3DEND()
	return S_OK;
}

void CX2SceneLoading::SetProgress( float fProgress )
{
	if( m_pDialog == NULL )
		return;
	CKTDGUIStatic* pStatic = m_pDialog->GetStatic_LUA( "First_Loading" );
	if( pStatic == NULL )
		return;

	const int iPercent = static_cast<int>( fProgress * 100.f + 0.5f );
	if( CKTDGUIControl::CPictureData* pBar = pStatic->GetPicture( 0 ) )
		pBar->SetSizeX( BAR_FULL_LENGTH * iPercent / 100.f );
	if( CKTDGUIControl::UIStringData* pString = pStatic->GetString( 0 ) )
		pString->msg = GET_REPLACED_STRING( ( STR_ID_655, "i", iPercent ) );
}

void CX2SceneLoading::Close()
{
	m_eStep = STEP_IDLE;
	if( m_pDialog != NULL )
		SAFE_DELETE_DIALOG_HANDLE( m_pDialog );
}
