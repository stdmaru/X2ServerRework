#include "stdafx.h"
#include "./KTDXDeviceSound.h"

// The original created an FMOD sound from the file in memory (FMOD_OPENMEMORY, decompressed into memory) and played it
// on a free channel; here the file is decoded into a clip and played on a voice of the sound system (KTDSSoundSystem.h).
// The background loading path of the original (X2OPTIMIZE_SOUND_BACKGROUND_LOAD) kept the file and created the sound on
// the main thread; decoding touches no shared state, so _Load decodes on whichever thread loads.

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

	if( NULL == m_pSystem )
		return S_OK;

	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
	Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( m_DeviceID );
	if( Info == NULL )
	{
		ErrorLogMsg( KEM_ERROR85, m_DeviceID.c_str() );
		return E_FAIL;
	}

	std::string error;
	x2::audio::ClipPtr pClip = x2::audio::DecodeClip(
		std::span<const std::byte>( reinterpret_cast<const std::byte*>( Info->pRealData ), static_cast<size_t>( Info->size ) ), error );
	if( pClip == NULL )
	{
		ErrorLogMsg( KEM_ERROR87, ( m_DeviceID + L", " + x2::platform::Utf8ToWide( error ) ).c_str() );
		return E_FAIL;
	}

	CSLock  locker( m_csDeviceState );
	if ( bSkipStateCheck == false )
	{
		switch( m_eDeviceState )
		{
		case DEVICE_STATE_INIT:
		default:
			ASSERT( 0 );
			[[fallthrough]];
		case DEVICE_STATE_LOADING:
		case DEVICE_STATE_FAILED:
			m_pClip = pClip;
			m_Size = Info->size;
			break;
		case DEVICE_STATE_LOADED:
			break;
		}
		m_eDeviceState = DEVICE_STATE_LOADED;
	}
	else
	{
		m_pClip = pClip;
		m_Size = Info->size;
	}

	return S_OK;
}

HRESULT CKTDXDeviceSound::_UnLoad()
{
	// Releasing an FMOD sound stopped every channel playing it.
	if( m_pSystem != NULL && m_pClip != NULL )
		m_pSystem->StopClip( m_pClip.get() );
	m_pClip.reset();
	m_Voice = 0;
	return S_OK;
}

void CKTDXDeviceSound::Play( bool loop /*= false */, bool b3DSound /*= true*/ )
{
	KTDXPROFILE();

	CKTDSManager* pManager = g_pKTDXApp->GetDSManager();
	if( pManager->GetInit() == false ||
		pManager->GetSoundMute() == true )
		return;

	if( false == IsLoaded() )
		return;

	if( NULL == m_pSystem || NULL == m_pClip )
		return;

	x2::audio::PlayParams params;
	params.loop = loop;
	params.volume = x2::audio::GainFromLevel( pManager->GetSoundVolume() );
	params.spatial = pManager->GetCapable3DSound() && pManager->GetEnable3DSound() && m_b3DSound && b3DSound;
	if( params.spatial )
	{
		const float fUnit = pManager->GetDistanceUnit();
		params.position = { m_vPos.x, m_vPos.y, m_vPos.z };
		params.minDistance = 0.1f * fUnit;
		params.maxDistance = ( m_fMaxDistance < 0.f ) ? 6.f * fUnit : m_fMaxDistance;
	}
	m_Voice = m_pSystem->Play( m_pClip, params );
}

void CKTDXDeviceSound::Wait()
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false || NULL == m_pSystem )
		return;

	m_pSystem->SetPaused( m_Voice, true );
}

void CKTDXDeviceSound::Stop()
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false || NULL == m_pSystem )
		return;

	m_pSystem->Stop( m_Voice );
	m_Voice = 0;
}

bool CKTDXDeviceSound::IsPlaying()
{
	KTDXPROFILE();

	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return false;

	if( IsLoaded() == false || NULL == m_pSystem )
		return false;

	return m_pSystem->IsPlaying( m_Voice );
}

// As in the original, the position is kept for the next play: a voice that is playing stays where it started.
void CKTDXDeviceSound::Set3DPosition( const D3DXVECTOR3& pos )
{
	m_vPos = pos;
}

void CKTDXDeviceSound::SetMax3DDistance( float fMaxDistance )
{
	m_fMaxDistance = fMaxDistance;
}

void CKTDXDeviceSound::SetVolume( float fVolume )
{
	KTDXPROFILE();

	if( false == IsPlaying() )
		return;

	m_pSystem->SetVolume( m_Voice, fVolume );
}
