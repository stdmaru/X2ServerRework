#pragma once

#include "KTDSSoundSystem.h"


// A sound effect. Silent backend (KTDSSoundSystem.h): loading reads nothing and playing plays nothing.
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

	bool Update3DPosition();
	virtual HRESULT _Load( bool bSkipStateCheck = false
				, bool bBackgroundQueueing = false
		);


	virtual HRESULT _UnLoad();


protected:



	KTDSSoundSystem*		m_pSystem;

	bool				m_b3DSound;
    float               m_fMaxDistance;

};
