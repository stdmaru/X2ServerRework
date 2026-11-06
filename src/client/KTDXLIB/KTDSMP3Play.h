#pragma once


#include "KTDSSoundSystem.h"





class CKTDSMP3Play
{
public:
	~CKTDSMP3Play(void);
	CKTDSMP3Play( KTDSSoundSystem* pSystem );

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

	KTDSSoundSystem*	m_pSystem;
};
