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

void CX2SceneLoading::Begin()
{
	if( m_pDialog == NULL )
	{
		m_pDialog = new CKTDGUIDialog( this, L"DLG_First_Loading_Front.lua", 0.0f );
		if( m_pDialog == NULL )
			return;
		// the first login's client version is not repeated here
		if( CKTDGUIControl* pVersion = m_pDialog->GetControl( L"Static_Client_Version" ) )
			pVersion->SetShow( false );
	}
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
