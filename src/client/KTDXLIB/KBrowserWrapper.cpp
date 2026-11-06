#include "KBrowserWrapper.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_misc.h>

namespace {

bool SdlOpenUrl( const char* strURL )
{
	if ( SDL_OpenURL( strURL ) )
		return true;
	SDL_Log( "KBrowserWrapper: cannot open %s: %s", strURL, SDL_GetError() );
	return false;
}

KBrowserWrapper::OpenUrlFunction g_pOpenUrl = &SdlOpenUrl;

}  // namespace

void KBrowserWrapper::SetOpenUrlFunction( OpenUrlFunction pFunction )
{
	g_pOpenUrl = ( pFunction != NULL ) ? pFunction : &SdlOpenUrl;
}

KBrowserWrapper::KBrowserWrapper( void )
	: m_bClosed( true )
	, m_bCheckClosed( true )
	, m_bPageOpened( false )
{
}

BOOL KBrowserWrapper::Create( RECT& /*rect*/, HWND /*hWndParent*/ )
{
	m_bClosed = false;
	m_bCheckClosed = true;
	return TRUE;
}

void KBrowserWrapper::CloseBrowser( void )
{
	m_bClosed = true;
	m_bCheckClosed = false;
}

BOOL KBrowserWrapper::Navigate( const char* strURL, const char* strPostString_ )
{
	if ( m_bClosed || strURL == NULL || strURL[0] == '\0' || ( strPostString_ != NULL && strPostString_[0] != '\0' ) )
		return FALSE;

	if ( !g_pOpenUrl( strURL ) )
	{
		CloseBrowser();
		return FALSE;
	}

	// Nothing stays open in the game; the closing is noticed when the player comes back (OnFocusGained).
	m_bClosed = true;
	m_bPageOpened = true;
	return TRUE;
}

bool KBrowserWrapper::OnFocusGained( void )
{
	if ( !m_bPageOpened )
		return false;
	m_bPageOpened = false;
	m_bCheckClosed = false;
	return true;
}
