#include "stdafx.h"
#include "./KTDSMP3Play.h"

// Silent backend (KTDSSoundSystem.h): the original streamed the file through FMOD; the player keeps its state (the file
// it was asked to play, loop, volume, pause) and plays nothing.

CKTDSMP3Play::CKTDSMP3Play( KTDSSoundSystem* pSystem )
{
	m_bLoop			= true;
	m_Volume		= 0;
	m_bPaused		= false;

	m_pSystem = pSystem;
}


CKTDSMP3Play::~CKTDSMP3Play(void)
{
	Clear();
}

void CKTDSMP3Play::Play( const WCHAR* pFileName, bool loop /*= true*/ )
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return;

	if( NULL == pFileName )
		return;

	m_FileName	= pFileName;
	m_bLoop		= loop;
}

void CKTDSMP3Play::Stop()
{
}

void CKTDSMP3Play::Clear()
{
}

void CKTDSMP3Play::SetVolume( int volume )
{
	m_Volume = volume;
}

void CKTDSMP3Play::SetPaused( bool bPause )
{
	m_bPaused = bPause;
}
