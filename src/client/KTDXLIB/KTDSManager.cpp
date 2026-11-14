#include "stdafx.h"
#include "./KTDSManager.h"

// The original created and configured an FMOD Ex system here: the driver's speaker mode, 64 channels (128 with
// FIX_STOLEN_MUSIC_CHANNEL), and the 3D settings (Doppler 1, distance factor 800, roll-off 1). The sound system
// (KTDSSoundSystem.h) has 128 voices for effects and the music on a stream of its own, so the music cannot lose its
// channel. When no audio device can be opened the system still runs without output, so the client behaves the same,
// silent; m_bInit is false only when the mixer itself cannot start.

CKTDSManager::CKTDSManager( HWND /*hWnd*/, LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
{
	m_bInit				= true;

	m_pSystem			= NULL;
	m_bCapable3DSound	= true;
	m_bEnable3DSound	= true;

	m_fDistanceUnit = 800.f; // units per meter (FMOD's distance factor: Doppler and 3D roll-off)

	m_bSoundMute		= false;
	m_fSoundVolume		= 0.f;

	std::string error;
	x2::audio::SoundSystem::Options options;
	m_pOwnedSystem = x2::audio::SoundSystem::Create( options, error );
	if( m_pOwnedSystem == nullptr )
	{
		ErrorLogMsg( KEM_ERROR360, x2::platform::Utf8ToWide( error ).c_str() );
		m_bInit = false;
		m_bCapable3DSound = false;
		m_bEnable3DSound = false;
		return;
	}
	if( false == error.empty() )
		ErrorLogMsg( KEM_ERROR366, x2::platform::Utf8ToWide( error ).c_str() );
	m_pSystem = m_pOwnedSystem.get();

	m_pMP3Play = new CKTDSMP3Play( m_pSystem );
}

CKTDSManager::~CKTDSManager(void)
{
	SAFE_DELETE( m_pMP3Play );
	m_pSystem = NULL;
	m_pOwnedSystem.reset();
}



HRESULT CKTDSManager::OnFrameMove( double /*fTime*/, float /*fElapsedTime*/ )
{
	KLagCheck( eKnown_LagCheckType_KTDSManager_FrameMove );

	if( NULL != m_pSystem && true == m_bInit )
		m_pSystem->Update();

	return S_OK;
}


void CKTDSManager::SetListenerData( D3DXVECTOR3 pos, D3DXVECTOR3 frontVec /*= D3DXVECTOR3(0, 0, 1)*/, D3DXVECTOR3 upVec /*= D3DXVECTOR3( 0, 1, 0)*/ )
{
	if( false == m_bInit || NULL == m_pSystem )
		return;

	if( false == m_bCapable3DSound ||
		false == m_bEnable3DSound )
		return;

	m_pSystem->SetListener( { pos.x, pos.y, pos.z }, { frontVec.x, frontVec.y, frontVec.z }, { upVec.x, upVec.y, upVec.z } );
}
