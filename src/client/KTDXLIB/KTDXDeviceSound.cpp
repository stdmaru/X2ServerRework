#include "stdafx.h"
#include "./KTDXDeviceSound.h"

// Silent backend (KTDSSoundSystem.h): the original created an FMOD sound from the file in memory and played it on a
// channel; here the device loads at once without reading the file, and never plays.

CKTDXDeviceSound::CKTDXDeviceSound( KTDSSoundSystem* pSystem, wstring fileName, bool bUse3D )
: CKTDXDevice( fileName, DT_SOUND )
{
	m_pSystem = pSystem;

	m_b3DSound = bUse3D;
    m_fMaxDistance = -1;
}

CKTDXDeviceSound::~CKTDXDeviceSound(void)
{
	_UnLoad();
}

HRESULT CKTDXDeviceSound::_Load( bool bSkipStateCheck, bool /*bBackgroundQueueing*/ )
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return S_OK;

	if ( bSkipStateCheck == false )
	{
		CSLock  locker( m_csDeviceState );
		m_eDeviceState = DEVICE_STATE_LOADED;
	}

	return S_OK;
}

HRESULT CKTDXDeviceSound::_UnLoad()
{
	return S_OK;
}

void CKTDXDeviceSound::Play( bool /*loop*/, bool /*b3DSound*/ )
{
}

void CKTDXDeviceSound::Wait()
{
}

void CKTDXDeviceSound::Stop()
{
}

bool CKTDXDeviceSound::IsPlaying()
{
	return false;
}

void CKTDXDeviceSound::Set3DPosition( const D3DXVECTOR3& /*pos*/ )
{
}

void CKTDXDeviceSound::SetMax3DDistance( float fMaxDistance )
{
	m_fMaxDistance = fMaxDistance;
}

void CKTDXDeviceSound::SetVolume( float /*fVolume*/ )
{
}

bool CKTDXDeviceSound::Update3DPosition()
{
	return true;
}
