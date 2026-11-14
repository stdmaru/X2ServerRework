#pragma once

#include "KTDSSoundSystem.h"


// A sound effect: the file decoded once when the device loads, played on a voice of the sound system each time. As
// with FMOD's channel in the original, the device follows only the voice of its last Play: Wait, Stop, IsPlaying and
// SetVolume act on it, while earlier plays run to their end.
class CKTDXDeviceSound : public CKTDXDevice
{
public:
	CKTDXDeviceSound( KTDSSoundSystem* pSystem, wstring fileName, bool bUse3D = true );



	void Play( bool loop = false, bool b3DSound = true );


	void Wait();
	void Stop();

	bool IsPlaying();
	void Set3DPosition( const D3DXVECTOR3& pos );
	void SetMax3DDistance( float fMaxDistance );
	void SetVolume( float fVolume );

protected:

	virtual ~CKTDXDeviceSound(void);

	virtual HRESULT _Load( bool bSkipStateCheck = false
				, bool bBackgroundQueueing = false
		);


	virtual HRESULT _UnLoad();


protected:



	KTDSSoundSystem*		m_pSystem;
	x2::audio::ClipPtr		m_pClip;	// set under m_csDeviceState when loaded
	x2::audio::VoiceId		m_Voice = 0;
	D3DXVECTOR3				m_vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );	// where the next 3D play starts

	bool				m_b3DSound;
    float               m_fMaxDistance;	// the 3D maximum distance; -1: the default (6 distance units)

};
