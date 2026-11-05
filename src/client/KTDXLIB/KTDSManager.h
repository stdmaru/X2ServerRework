#pragma once


#include "KTDSMP3Play.h"

#include "FMOD/inc/fmod.h"
#include "FMOD/inc/fmod_errors.h"



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

		//void ErrorCheck( FMOD_RESULT fr )
		//{
		//	if (fr != FMOD_OK)
		//	{
		//		//printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		//		//exit(-1);
		//	}
		//}

		FMOD_SYSTEM* GetSystem() const { return m_pSystem; }

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

		CKTDSMP3Play*			m_pMP3Play;


		FMOD_SYSTEM*			m_pSystem;
		bool					m_bEnable3DSound;		// 옵션 설정등에서 실제로 3D sound를 사용할지
		bool					m_bCapable3DSound;		// 시스템에서 3D sound를 지원하는지


		float					m_fDistanceUnit;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

};
