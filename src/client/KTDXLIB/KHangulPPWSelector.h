#pragma once

//#include <stdio.h>
#include "x2/platform/win32.h"

bool KHanSungDivProcess( WORD kUnicode, char* pcResultChar );
bool KHanSungDivWideChar( char* pcBuff, char* pcResultChar );
bool KHanSungDivUnicode( WCHAR* pcWord, char* pcResultChar );

bool KHanSungPostWordWideChar( const char* pcBuff_ );
bool KHanSungPostWordUnicode( const WCHAR* pcWord_ );

int KHanSungPostWordUnicodeWrapper( const WCHAR* pcWord_, int eTrueReturn, int eFalseReturn );
