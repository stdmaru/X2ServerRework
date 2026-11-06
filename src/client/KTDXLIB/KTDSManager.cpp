#include "stdafx.h"
#include "./KTDSManager.h"

// Silent backend (KTDSSoundSystem.h): the original created and configured an FMOD Ex system here (m_bInit false when it
// failed). The silent manager is always initialized, has no system object and no 3D sound, and its music player is
// always there, so the options and the states keep their bookkeeping.

CKTDSManager::CKTDSManager( HWND /*hWnd*/, LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
{
	m_bInit				= true;

	m_pSystem			= NULL;
	m_bCapable3DSound	= false;
	m_bEnable3DSound	= false;

	m_fDistanceUnit = 800.f; // units per meter (FMOD's distance factor: Doppler and 3D roll-off)

	m_bSoundMute		= false;
	m_fSoundVolume		= 0.f;

	m_pMP3Play			= new CKTDSMP3Play( m_pSystem );
}

CKTDSManager::~CKTDSManager(void)
{
	SAFE_DELETE( m_pMP3Play );
}



HRESULT CKTDSManager::OnFrameMove( double /*fTime*/, float /*fElapsedTime*/ )
{
	KLagCheck( eKnown_LagCheckType_KTDSManager_FrameMove );

	return S_OK;
}


void CKTDSManager::SetListenerData( D3DXVECTOR3 /*pos*/, D3DXVECTOR3 /*frontVec*/, D3DXVECTOR3 /*upVec*/ )
{
}
