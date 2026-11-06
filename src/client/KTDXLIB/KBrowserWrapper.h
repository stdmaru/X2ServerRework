// KBrowserWrapper: the web pages the game opens (the cash charge page, the server's advertisement pages), in the system
// browser. The original embedded Internet Explorer in a Win32 dialog over the game (KPCHWebBrowser, Browser.c); the
// port opens the page with SDL_OpenURL and keeps the interface its callers use. docs/porting/client-ktdxlib.md#web-pages
//
//   wrapper.Create( rect, hWnd );        // "opens the window": IsClosed() is false (rect and parent are unused)
//   wrapper.Navigate( url, NULL );       // opens url in the system browser; the wrapper counts as closed at once
//   ...                                  // the game window gets the focus back: OnFocusGained()
//   if ( !wrapper.GetCheckClosed() )     // as after the dialog closed: CX2State asks for the cash balance
//
// The original cleared GetCheckClosed when its dialog closed; with no dialog, that happens when the game window gets the
// focus back after a page was opened (the player is back from the browser).
#pragma once

#include "x2/platform/win32.h"

class KBrowserWrapper
{
public:
	typedef bool (*OpenUrlFunction)( const char* strURL );

	KBrowserWrapper( void );

	BOOL Init( void ) { return TRUE; }
	BOOL UnInit( void ) { return TRUE; }

	BOOL Create( RECT& rect, HWND hWndParent );
	void CloseBrowser( void );

	// Opens strURL in the system browser. Only GET: no caller posts data, and a POST string makes it fail.
	BOOL Navigate( const char* strURL, const char* strPostString_ );

	bool IsClosed( void ) const { return m_bClosed; }
	bool GetCheckClosed( void ) const { return m_bCheckClosed; }
	void SetCheckClosed( bool bCheck ) { m_bCheckClosed = bCheck; }

	// The game window got the focus. True when a page had been opened since: GetCheckClosed is now false.
	bool OnFocusGained( void );

	// SDL_OpenURL by default; tests replace it.
	static void SetOpenUrlFunction( OpenUrlFunction pFunction );

private:
	bool			m_bClosed;
	bool			m_bCheckClosed;
	bool			m_bPageOpened;	// opened in the system browser, the player not back yet
};
