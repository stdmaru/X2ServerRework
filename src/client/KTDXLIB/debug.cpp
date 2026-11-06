#include "stdafx.h"
#include <stdarg.h>
#include "x2/platform/win32.h"
#include <stdio.h>

int debugPrintf( const char *fmt, ... ) {
	 va_list l;
	 va_start( l, fmt );
	 char buf[1024];
	 vsnprintf( buf, sizeof(buf), fmt, l );
	 va_end( l );
	 OutputDebugStringA( buf );
	 return 0;
}

int debugWPrintf( const WCHAR *fmt, ... )
{
	va_list l;
	va_start( l, fmt );
	WCHAR buf[1024];
#if defined(_WIN32)
	vswprintf( buf, _countof(buf), fmt, l );
#else
	vswprintf_ms( buf, _countof(buf), fmt, l );   // MSVC's format semantics (%s is wide)
#endif
	va_end( l );
	OutputDebugStringW( buf );
	return 0;
}
