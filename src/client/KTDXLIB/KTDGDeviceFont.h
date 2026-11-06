#pragma once

// The font cache types shared by CKTDGFreeTypeFont. The original also declared CKTDGDeviceFont here, a font drawn
// with GDI that nothing used any more (CKTDGFontManager creates CKTDGFreeTypeFont); it was removed with its .cpp.

#include "x2/platform/d3dx_math.h"
#include "Device.h"
#include <vector>
#include "KTDGFontVertex.h"
#include "KTDGDynamicTexture.h"


//#define MAX_FONT_SIZE				(256)
//#define FONT_CACHE_SIZE				(512)
#define HASH_BUCKET_SIZE			65536

#define MAX_FONT_SIZE				(128)
#define FONT_CACHE_SIZE				(256)
#define FONT_CACHE_PAGE				(4)
#define VIRTUAL_FONT_LINE_MAX		(3000)

//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//	#define	MAX_CHAR_IN_PAGE			(((int)FONT_CACHE_SIZE/12)*((int)FONT_CACHE_SIZE/12))		//12 X 12를 최소로 생각함
//	#define MAX_VERTEX_IN_PAGE			(MAX_CHAR_IN_PAGE*6)
//#endif

#define DT_NO_COLOR					(0xF0000000)

//#define _16BIT_FONT
#ifdef _16BIT_FONT
//D3DCOLOR_A1R5G5B5
#define MYD3DCOLOR_ARGB(a,r,g,b)	(WORD)( ((a&0x1) << 15) | ((r&0x1F)<<10) | ((g&0x1F) << 5) | (b&0x1F) )
#define MYFONTCOLORFMT				(D3DFMT_A1R5G5B5)
#define MYFONTCOLOR_PTR				WORD*
#define MYFONTCOLOR_ALPHA(col)		( ( col & (0x1 << 15) ) >> 15 )
#else
#define MYD3DCOLOR_ARGB(a,r,g,b)	D3DCOLOR_ARGB(a,r,g,b)
#define MYFONTCOLORFMT				(D3DFMT_A8R8G8B8)
#define MYFONTCOLOR_PTR				DWORD*
#define MYFONTCOLOR_ALPHA(col)		( ( col & 0xff000000 ) >> 24 )
#endif

// DT_HVCENTER 은 DT_CENTER(이진수 0001)와 DT_VCENTER(이진수 0100) 비트 연산자 OR 로 구성된 값으로 5(0101)
// DT_HVCENTER 는 DT_CENTER와 DT_VCENTER를 개별로 비트 연산자 | 하면 true 나와야 하는 값으로 구성해야 함
#define DT_HVCENTER		( DT_CENTER + DT_VCENTER )

struct SGCFontCache
{
	static float PixelToTex;

	int iLeft, iTop;
	WCHAR _Char;
	DWORD dwLastUse;
	int iWidth,iHeight;
	int	iPage;

	float _texLeft;
	float _texRight;
	float _texTop;
	float _texBottom;

    SGCFontCache()
        : iLeft( 0 )
        , iTop( 0 )
        , _Char( 0 )
        , dwLastUse( 0 )
        , iWidth( 0 )
        , iHeight( 0 )
        , iPage( 0 )
		, _texLeft( 0.f )
    	, _texRight( 0.f )
    	, _texTop( 0.f )
		, _texBottom( 0.f )
	{
	}

	void SetPos(int x, int y, int iFontSize)
	{
		iLeft = x * (iFontSize);
		iTop  = y * iFontSize;

		_texLeft = PixelToTex * ((float)iLeft + 0.5f);
		_texTop = PixelToTex * ((float)iTop + 0.5f);
	}

	void _SetRect(int width, int height)
	{
		iWidth = width;
		iHeight = height;

		_texRight = PixelToTex * ((float)(iLeft + iWidth) + 0.5f);
		_texBottom = PixelToTex * ((float)(iTop + iHeight) + 0.5f);
	}
};
