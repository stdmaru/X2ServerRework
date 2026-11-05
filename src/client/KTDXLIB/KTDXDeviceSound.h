#pragma once

#include "FMOD/inc/fmod.h"
#include "FMOD/inc/fmod_errors.h"
#include "KGCMassFileBufferManager.h"


class CKTDXDeviceSound : public CKTDXDevice
{
public:
	CKTDXDeviceSound( FMOD_SYSTEM* pSystem, wstring fileName, bool bUse3D = true );



	void Play( bool loop = false, bool b3DSound = true );


	void Wait();
	void Stop();

	bool IsPlaying();
	void Set3DPosition( const D3DXVECTOR3& pos );
	void SetMax3DDistance( float fMaxDistance );
	void SetVolume( float fVolume );

protected:

	virtual ~CKTDXDeviceSound(void);

	bool Update3DPosition();
	virtual HRESULT _Load( bool bSkipStateCheck = false
				, bool bBackgroundQueueing = false
		);


	virtual HRESULT _UnLoad();
	void			_CheckLoadFMODSound();


protected:


	FMOD_VECTOR 		m_vFmodPos;

	FMOD_SYSTEM*		m_pSystem;
	FMOD_SOUND*			m_pSound;
	FMOD_CHANNEL*		m_pChannel;

	bool				m_b3DSound;
    float               m_fMaxDistance;

	KGCMassFileBufferPtr m_spMemoryBuffer;

};
