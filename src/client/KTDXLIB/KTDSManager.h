#pragma once


#include "KTDSMP3Play.h"

#include "KTDSSoundSystem.h"



class CKTDSManager : public CKTDXStage
{
	public:
		CKTDSManager( HWND hWnd, LPDIRECT3DDEVICE9 pd3dDevice );
		~CKTDSManager(void);

		CKTDSMP3Play*	GetMP3Play(){ return m_pMP3Play; }

		bool			GetInit(){ return m_bInit; }

		void			SetSoundVolume( float fVolume ){ m_fSoundVolume = fVolume; }
		float			GetSoundVolume(){ return m_fSoundVolume; }

		void			SetSoundMute( bool bMute ){ m_bSoundMute = bMute; }
		bool			GetSoundMute(){ return m_bSoundMute; }


		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );

		KTDSSoundSystem* GetSystem() const { return m_pSystem; }

		bool GetEnable3DSound() const { return m_bEnable3DSound; }
		void SetEnable3DSound(bool val) { m_bEnable3DSound = val; }

		bool GetCapable3DSound() const { return m_bCapable3DSound; }
		void SetCapable3DSound(bool val) { m_bCapable3DSound = val; }


		float GetDistanceUnit() const { return m_fDistanceUnit; }

		void SetListenerData( D3DXVECTOR3 pos, D3DXVECTOR3 frontVec = D3DXVECTOR3(0, 0, 1), D3DXVECTOR3 upVec = D3DXVECTOR3( 0, 1, 0) );



	private:
		bool					m_bInit;
		bool					m_bSoundMute;
		float					m_fSoundVolume;

		CKTDSMP3Play*			m_pMP3Play = NULL;	// NULL only when the sound system could not start (m_bInit false)


		std::unique_ptr<KTDSSoundSystem>	m_pOwnedSystem;
		KTDSSoundSystem*			m_pSystem;
		bool					m_bEnable3DSound;		// whether 3D sound is used (the options)
		bool					m_bCapable3DSound;		// whether the system supports 3D sound


		float					m_fDistanceUnit;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

};
