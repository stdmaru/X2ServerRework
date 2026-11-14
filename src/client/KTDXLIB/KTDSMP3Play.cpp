#include "stdafx.h"
#include "./KTDSMP3Play.h"

#include <fstream>

// The background music: one track, found among the data directories (data/music/*.ogg) and streamed by the sound
// system (KTDSSoundSystem.h), as FMOD streamed it from the file in the original. The file is read into memory once and
// decoded as it plays.

namespace
{
	bool ReadWholeFile( const WCHAR* pPath, std::vector<std::byte>& data )
	{
		std::ifstream file( std::filesystem::path( std::wstring( pPath ) ), std::ios::binary | std::ios::ate );
		if( !file )
			return false;
		const std::streamoff size = file.tellg();
		if( size <= 0 )
			return false;
		data.resize( static_cast<size_t>( size ) );
		file.seekg( 0 );
		return static_cast<bool>( file.read( reinterpret_cast<char*>( data.data() ), size ) );
	}
}

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

	WCHAR wcsFileName[MAX_PATH] = L"";
	wcsncpy( wcsFileName, pFileName, MAX_PATH - 1 );
	if( g_pKTDXApp->GetDeviceManager()->GetPullPath( wcsFileName ) == NULL )
		return;

	m_FileName	= pFileName;
	m_bLoop		= loop;

	if( NULL == m_pSystem )
		return;

	Clear();

	std::vector<std::byte> data;
	if( false == ReadWholeFile( wcsFileName, data ) ||
		false == m_pSystem->PlayMusic( std::move( data ), m_bLoop, x2::audio::GainFromLevel( static_cast<float>( m_Volume ) ), m_bPaused ) )
	{
		ErrorLog( KEM_ERROR383 );
		return;
	}
	m_bLoaded = true;
}

void CKTDSMP3Play::Stop()
{
	Clear();
}

void CKTDSMP3Play::Clear()
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return;

	if( m_bLoaded && NULL != m_pSystem )
		m_pSystem->StopMusic();
	m_bLoaded = false;
}

void CKTDSMP3Play::SetVolume( int volume )
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return;

	m_Volume = volume;

	if( m_bLoaded && NULL != m_pSystem )
		m_pSystem->SetMusicVolume( x2::audio::GainFromLevel( static_cast<float>( m_Volume ) ) );
}

void CKTDSMP3Play::SetPaused( bool bPause )
{
	if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		return;

	if( m_bPaused == bPause )
		return;

	m_bPaused = bPause;

	// Nothing loaded yet (the file was given while paused, or failed): resuming starts it.
	if( false == m_bLoaded )
	{
		if( false == bPause )
			Play( m_FileName.c_str(), m_bLoop );
		return;
	}

	m_pSystem->SetMusicPaused( bPause );
}
