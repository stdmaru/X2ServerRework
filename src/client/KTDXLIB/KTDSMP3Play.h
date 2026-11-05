#pragma once


#include "FMOD/inc/fmod.h"
#include "FMOD/inc/fmod_errors.h"





class CKTDSMP3Play
{
public:
	~CKTDSMP3Play(void);
	CKTDSMP3Play( FMOD_SYSTEM* pSystem );

	void Play( const WCHAR* pFileName, bool loop = true );
	void Stop();
	void Clear();

	void SetVolume( int volume );
	void SetPaused( bool bPause );

	wstring GetPlayingFileName() { return m_FileName; }

private:
	wstring			m_FileName;
	bool			m_bLoop;
	int				m_Volume;
	bool			m_bPaused;

	FMOD_SYSTEM*	m_pSystem;
	FMOD_SOUND*		m_pSound;
	FMOD_CHANNEL*	m_pChannel;
};
