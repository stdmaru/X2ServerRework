#include "stdafx.h"
#include "./KTDGFontManager.h"

CKTDGFontManager::CKTDGFontManager( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int error = FT_Init_FreeType( &m_FreeTypeLibrary );
	if( error )
	{
		wstringstream strstm;
		strstm << "Font Init Failed : returns " << error;
		MessageBoxW( NULL, strstm.str().c_str(), L"Error!", NULL );
		ErrorLogMsg( KEM_ERROR426, strstm.str().c_str() );
	}
}

CKTDGFontManager::~CKTDGFontManager(void)
{
	FT_Done_FreeType( m_FreeTypeLibrary );

	for( int i = 0; i < (int)m_UKFontList.size(); i++ )
	{
		SAFE_DELETE( m_UKFontList[i] );
	}
	m_UKFontList.clear();

#ifdef DLL_BUILD
	for ( int i = 0; i < (int)m_UseFontNameList.size(); i++ )
	{
		wstring fontName = m_UseFontNameList[i];
		//RemoveFontResourceEx( fontName.c_str(), FR_PRIVATE, NULL );
		if( RemoveFontResource( fontName.c_str() ) == false )
		{
			int i = 0;
		}
	}
#endif // DLL_BUILD
}



void CKTDGFontManager::OnFrameMove( float fTime, float fElapsedTime )
{
	KLagCheck( eKnown_LagCheckType_KTDGFontManager_FrameMove );


	for( int i = 0; i < (int) m_UKFontList.size(); i++ )
	{
		CUKFont* pFont = m_UKFontList[i];
		if ( pFont == NULL )
			continue;
		pFont->OnFrameMove( fElapsedTime );
	}
}

void CKTDGFontManager::OnFrameRender()
{

	for( int i = 0; i < (int) m_UKFontList.size(); i++ )
	{
		CUKFont* pFont = m_UKFontList[i];
		if ( pFont == NULL )
			continue;
		pFont->OnFrameRender();
	}
}



HRESULT	CKTDGFontManager::OnResetDevice()
{

	for( int i = 0; i < (int)m_UKFontList.size(); i++ )
	{
		if ( m_UKFontList[i] != NULL )
			m_UKFontList[i]->OnResetDevice();
	}

	return S_OK;
}

HRESULT	CKTDGFontManager::OnLostDevice()
{

	for( int i = 0; i < (int)m_UKFontList.size(); i++ )
	{
		if ( m_UKFontList[i] != NULL )
			m_UKFontList[i]->OnLostDevice();
	}

	return S_OK;
}



void CKTDGFontManager::CreateNewUIFont_LUA( int fontID, const char* pFontName, int Height, int Weight )
{
}

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
void CKTDGFontManager::CreateNewUIUKFont_LUA( int fontID, const char* pFontName, int Height, int Weight, int enlargeNum, int iOutlineSize, bool bNoRes )
//#else
//void CKTDGFontManager::CreateNewUIUKFont_LUA( int fontID, const char* pFontName, int Height, int Weight, int enlargeNum )
//#endif
{
	wstring fontName;
	ConvertUtf8ToWCHAR( fontName, pFontName );
	CUKFont* pFont;

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	pFont = CreateNewUKFont( fontName.c_str(), Height, iOutlineSize, true, Weight, enlargeNum, bNoRes );
//#else
//	pFont = CreateNewUKFont( fontName.c_str(), Height, 1, true, Weight, enlargeNum );
//#endif

	g_pKTDXApp->GetDGManager()->GetDialogManager()->SetUKFont( fontID, pFont );
}


void CKTDGFontManager::MapUIFontToUKFont( int fontID, int ukFontID )
{
	g_pKTDXApp->GetDGManager()->GetDialogManager()->MapFontToUKFont( fontID, ukFontID );
}//CKTDGFontManager::MapUIFontToUKFont()


//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
CKTDGFontManager::CUKFont*	CKTDGFontManager::CreateNewUKFont( const WCHAR* wstrFontName, int iFontSize, int iOutLineSize, bool bRHW, int fontWeight /*= FW_NORMAL*/, int enlargeNum /*= 1*/, bool bNoRes )
//#else
//CKTDGFontManager::CUKFont*	CKTDGFontManager::CreateNewUKFont( const WCHAR* wstrFontName, int iFontSize, int iOutLineSize, bool bRHW, int fontWeight /*= FW_NORMAL*/, int enlargeNum /*= 1*/ )
//#endif
{
//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	CUKFont* pFont = new CUKFont( wstrFontName, iFontSize, iOutLineSize, bRHW, fontWeight, enlargeNum, bNoRes );
//#else
//	CUKFont* pFont = new CUKFont( wstrFontName, iFontSize, iOutLineSize, bRHW, fontWeight, enlargeNum );
//#endif
	m_UKFontList.push_back( pFont );

	//m_mapUKFontList.insert(map<string, string>::value_type( fontIndex, pFont );

	return pFont;
}

bool CKTDGFontManager::DestroyUKFont( CUKFont* pFont )
{
	for( int i = 0; i < (int)m_UKFontList.size(); i++ )
	{
		if( pFont == m_UKFontList[i] )
		{
			SAFE_DELETE( pFont );
			m_UKFontList.erase( m_UKFontList.begin() + i );
			return true;
		}
	}
	return false;
}






//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
CKTDGFontManager::CUKFont::CUKFont( const WCHAR* wstrFontName, int iFontSize, int iOutLineSize,
	bool bRHW, int fontWeight/* = FW_NORMAL */, int enlargeNum /* = 1 */, bool bNoRes )
//#else
//CKTDGFontManager::CUKFont::CUKFont( const WCHAR* wstrFontName, int iFontSize, int iOutLineSize,
//								   bool bRHW, int fontWeight/* = FW_NORMAL */, int enlargeNum /* = 1 */ )
//#endif
{
	string strFontName;
	ConvertWCHARToChar( strFontName, wstrFontName );

	m_pUKFont = new CKTDGFreeTypeFont( strFontName, iFontSize, iOutLineSize, bRHW, fontWeight, enlargeNum, bNoRes );
	m_pUKFont->Load();

	D3DXMatrixIdentity( &m_matIdentity );

	m_TextID = 0;
}

CKTDGFontManager::CUKFont::~CUKFont(void)
{
	Clear2DFont();

	m_pUKFont->UnLoad();
	SAFE_DELETE( m_pUKFont );
}

void CKTDGFontManager::CUKFont::OnResetDevice()
{
	if ( m_pUKFont != NULL )
		m_pUKFont->OnResetDevice();
}
void CKTDGFontManager::CUKFont::OnLostDevice()
{
	if ( m_pUKFont != NULL )
		m_pUKFont->OnLostDevice();
}

bool CKTDGFontManager::CUKFont::Load()
{
	return m_pUKFont->Load();
}
void CKTDGFontManager::CUKFont::UnLoad(bool bComplete /*= true */)
{
	m_pUKFont->UnLoad( bComplete );
}

std::string CKTDGFontManager::CUKFont::MakeFontID( std::string strFontName, int iFontSize, int iOutLineSize, bool bRHW, int fontWeight/* = FW_NORMAL*/ )
{
	return "";
}

int CKTDGFontManager::CUKFont::GetWidth( WCHAR wLetter ) const
{
	return m_pUKFont->GetWidth( wLetter );
}
int CKTDGFontManager::CUKFont::GetHeight( WCHAR wLetter ) const
{
	return m_pUKFont->GetHeight( wLetter );
}
int CKTDGFontManager::CUKFont::GetWidth( const WCHAR* wszText ) const
{
	return m_pUKFont->GetWidth( wszText );
}
int CKTDGFontManager::CUKFont::GetHeight( const WCHAR* wszText ) const
{
	KTDXPROFILE();

	int enterNum = 1;

	if( NULL != wszText )
	{
		int stringIndex = 0;
		wstring stringData = wszText;


		while( 1 )
		{
			stringIndex = stringData.find( L"\n", stringIndex );
			if ( stringIndex == -1 )
				break;

			stringIndex++;
			enterNum++;
		}
	}

	if( NULL == m_pUKFont )
	{
		// ERROR; m_pUKFONT shouldn't be NULL
		return 1;
	}

	int stringHeight = enterNum * m_pUKFont->GetHeight();
	return stringHeight;
}

int CKTDGFontManager::CUKFont::GetPaddedHeight( const WCHAR* wszText, int iAddHeight ) const
{
	KTDXPROFILE();

	int enterNum = 1;

	if( NULL != wszText )
	{
		int stringIndex = 0;
		wstring stringData = wszText;


		while( 1 )
		{
			stringIndex = stringData.find( L"\n", stringIndex );
			if ( stringIndex == -1 )
				break;

			stringIndex++;
			enterNum++;
		}
	}

	if( NULL == m_pUKFont )
	{
		// ERROR; m_pUKFONT shouldn't be NULL
		return 1;
	}

	int stringHeight = enterNum * ( m_pUKFont->GetHeight() + iAddHeight );
	return stringHeight;
}


//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
void CKTDGFontManager::CUKFont::OutTextXY( const int& iLeft, const int& iTop, const WCHAR* wszText, D3DCOLOR color /*= 0xffffffff*/, CKTDGFontManager::FONT_STYLE fontStyle /* = FS_NONE */, D3DCOLOR colorOutLine /*= 0xff000000*/, RECT* pRt /* = NULL */, DWORD dwFlag /* = DT_LEFT|DT_TOP  */, int nCount /*= -1 */, float fScaleX, float fScaleY )
//#else
//void CKTDGFontManager::CUKFont::OutTextXY( const int& iLeft, const int& iTop, const WCHAR* wszText, D3DCOLOR color /*= 0xffffffff*/,
//										  CKTDGFontManager::FONT_STYLE fontStyle /* = FS_NONE */, D3DCOLOR colorOutLine /*= 0xff000000*/,
//										  RECT* pRt /* = NULL */, DWORD dwFlag /* = DT_LEFT|DT_TOP  */)
//#endif
{
	KTDXPROFILE();

	if ( fontStyle == FS_NONE || fontStyle == FS_SHADOW )
		colorOutLine = D3DXCOLOR( 0, 0, 0, 0 );

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	RECT    rtScale;

	if ( pRt != NULL )
	{
		rtScale.left = (int) ( pRt->left * g_pKTDXApp->GetResolutionScaleX() );
		rtScale.top  = (int) ( pRt->top * g_pKTDXApp->GetResolutionScaleY() );
		rtScale.right = (int) ( pRt->right * g_pKTDXApp->GetResolutionScaleX() );
		rtScale.bottom = (int) ( pRt->bottom * g_pKTDXApp->GetResolutionScaleY() );
		pRt = &rtScale;
	}//if

	m_pUKFont->OutTextXY( (int)( iLeft * g_pKTDXApp->GetResolutionScaleX() )
		, (int)( iTop * g_pKTDXApp->GetResolutionScaleY() ), wszText, color, colorOutLine, pRt, dwFlag, nCount
		, fScaleX, fScaleY );
//#else
//	m_pUKFont->OutTextXY( (int)( iLeft * g_pKTDXApp->GetResolutionScaleX() ), (int)( iTop * g_pKTDXApp->GetResolutionScaleY() ), wszText, color, colorOutLine, pRt, dwFlag );
//#endif
}

void CKTDGFontManager::CUKFont::OutProjectionText( const D3DXVECTOR3& pos, const WCHAR* wszText, D3DCOLOR color /*= 0xffffffff*/,
												  CKTDGFontManager::FONT_STYLE fontStyle /* = FS_NONE */, D3DCOLOR colorOutLine /*= 0xff000000*/,
												  RECT* pRt /* = NULL */, DWORD dwFlag /* = DT_LEFT|DT_TOP  */ )
{
	KTDXPROFILE();

	D3DXVECTOR3 screenPos;

	KTDXPROFILE_BEGIN( "projection to screen" );

	D3DXMatrixIdentity( &m_matIdentity );
	g_pKTDXApp->SetWorldTransform( &m_matIdentity );
	g_pKTDXApp->GetProjectionTransform( &m_matProj );
	g_pKTDXApp->GetViewTransform( &m_matView );

	D3DVIEWPORT9 kViewPort = g_pKTDXApp->GetViewport();

	// Process
	D3DXVec3Project( &screenPos, &pos, &kViewPort, &m_matProj, &m_matView, &m_matIdentity );

	// Cull
	if ( 0.f > screenPos.z || screenPos.z > 1.f )
	{
		//KDbgStm << "Text Out Cull" << trace;
		return;
	}

	if ( fontStyle == FS_NONE || fontStyle == FS_SHADOW )
		colorOutLine = D3DXCOLOR( 0, 0, 0, 0 );
	KTDXPROFILE_END();


	KTDXPROFILE_BEGIN( "m_pUKFont->OutTextXY" );
	m_pUKFont->OutTextXY( (int)screenPos.x, (int)screenPos.y, wszText, color, colorOutLine, pRt, dwFlag );
	KTDXPROFILE_END();
}

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
void CKTDGFontManager::CUKFont::OutTextMultiline( const int& iLeft, const int& iTop, const WCHAR* wszText, D3DCOLOR color /*= 0xffffffff*/, CKTDGFontManager::FONT_STYLE fontStyle /* = FS_NONE */, D3DCOLOR colorOutLine /*= 0xff000000*/, float fLineSpace /* = 1.0f */, RECT* pRt /* = NULL */, DWORD dwFlag /* = DT_LEFT|DT_TOP */, int nCount /* = -1 */, float fScaleX, float fScaleY )
//#else
//void CKTDGFontManager::CUKFont::OutTextMultiline( const int& iLeft, const int& iTop, const WCHAR* wszText, D3DCOLOR color /*= 0xffffffff*/,
//												 CKTDGFontManager::FONT_STYLE fontStyle /* = FS_NONE */, D3DCOLOR colorOutLine /*= 0xff000000*/,
//												 float fLineSpace /* = 1.0f */, RECT* pRt /* = NULL */, DWORD dwFlag /* = DT_LEFT|DT_TOP */)
//#endif
{
	KTDXPROFILE();

	if ( fontStyle == FS_NONE || fontStyle == FS_SHADOW )
		colorOutLine = D3DXCOLOR( 0, 0, 0, 0 );

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	RECT    rtScale;
	if ( pRt != NULL )
	{
		rtScale.left = (int) ( pRt->left * g_pKTDXApp->GetResolutionScaleX() );
		rtScale.top  = (int) ( pRt->top * g_pKTDXApp->GetResolutionScaleY() );
		rtScale.right = (int) ( pRt->right * g_pKTDXApp->GetResolutionScaleX() );
		rtScale.bottom = (int) ( pRt->bottom * g_pKTDXApp->GetResolutionScaleY() );
		pRt = &rtScale;
	}//if

	m_pUKFont->OutTextMultiline( (int)( iLeft * g_pKTDXApp->GetResolutionScaleX() ),
								(int)( iTop * g_pKTDXApp->GetResolutionScaleY() ),
								wszText, color, colorOutLine, fLineSpace, pRt, dwFlag, nCount,
								fScaleX, fScaleY );
//#else
//	m_pUKFont->OutTextMultiline( (int)( iLeft * g_pKTDXApp->GetResolutionScaleX() ), (int)( iTop * g_pKTDXApp->GetResolutionScaleY() ), wszText, color, colorOutLine, fLineSpace, pRt, dwFlag );
//#endif
}

void CKTDGFontManager::CUKFont::DrawTextFont( const WCHAR* strMsg, RECT* pos, int nCount, D3DXCOLOR color, bool bOutline, D3DXCOLOR outlineColor, DWORD sortFlag )
{
	if ( pos == NULL )
		return;

	if ( strMsg == NULL || strMsg[0] == NULL || nCount == 0 )
		return;

	if ( bOutline == true )
		DrawFont( strMsg, *pos, sortFlag, color, outlineColor, FS_SHELL, nCount );
	else
		DrawFont( strMsg, *pos, sortFlag, color, 0xff000000, FS_NONE, nCount );
}

void CKTDGFontManager::CUKFont::CalcTextRect( const WCHAR* strMsg, RECT* pos, int nCount, DWORD format )
{
	if ( pos == NULL )
		return;

	if ( strMsg == NULL || strMsg[0] == NULL || nCount == 0 )
	{
		pos->left = pos->top = pos->right = pos->bottom = 0;
		return;
	}//if

	if ( format & DT_SINGLELINE )
	{
		pos->bottom = pos->top + m_pUKFont->GetHeight();
		pos->right = pos->left + m_pUKFont->GetWidth( strMsg, nCount );
	}
	else
	{
		pos->right = pos->left + m_pUKFont->GetWidth( strMsg, nCount );
		pos->bottom = pos->top + m_pUKFont->GetHeight( strMsg, nCount );
	}//if.. else..
}

void CKTDGFontManager::CUKFont::DrawFont( const WCHAR* strMsg, RECT rt, DWORD sortFlag, D3DXCOLOR color, D3DXCOLOR outlineColor, FONT_STYLE fontStyle, int nCount /* = -1  */)
{
	KTDXPROFILE();

	if( NULL == strMsg || strMsg[0] == NULL || nCount == 0 )
		return;

	switch( sortFlag )
	{
		case DT_CENTER:
			{
				CalcTextRect( strMsg, &rt, nCount, 0 );
				int width	= rt.right - rt.left;
				rt.left		-= width/2;
				rt.right	-= width/2;
			}
			break;

		case DT_RIGHT:
			{
				CalcTextRect( strMsg, &rt, nCount, 0 );
				int width	= rt.right - rt.left;
				rt.left		-= width;
				rt.right	-= width;
			}
			break;

		case DT_VCENTER:
			{
				int height	= rt.bottom - rt.top;
				int fontHeight = m_pUKFont->GetHeight( strMsg, nCount );
				rt.top		= rt.top + height/2 - fontHeight/2;
				rt.bottom	= rt.bottom - height/2 + fontHeight/2;
			}
			break;
		case DT_HVCENTER:
			{
				CalcTextRect( strMsg, &rt, nCount, 0 );
				int width	= rt.right - rt.left;
				rt.left		-= width/2;
				rt.right	-= width/2;

				int height	= rt.bottom - rt.top;
				int fontHeight = m_pUKFont->GetHeight( strMsg, nCount );
				rt.top		= rt.top + height/2 - fontHeight/2;
				rt.bottom	= rt.bottom - height/2 + fontHeight/2;
			}
			break;
	}

    RECT    rtScale;
    rtScale.left = (int) ( rt.left * g_pKTDXApp->GetResolutionScaleX() );
    rtScale.top  = (int) ( rt.top * g_pKTDXApp->GetResolutionScaleY() );
    rtScale.right = (int) ( rt.right * g_pKTDXApp->GetResolutionScaleX() );
    rtScale.bottom = (int) ( rt.bottom * g_pKTDXApp->GetResolutionScaleY() );

    if ( fontStyle != FS_NONE )
    {
        DrawFontStyle( strMsg, rt, DT_LEFT | DT_NOCLIP, outlineColor, fontStyle, nCount );
        m_pUKFont->OutTextMultiline( rtScale.left
            , rtScale.top
            , strMsg, color, 0xff000000, 1.0f, &rtScale, sortFlag, nCount
			);
    }
    else
    {
        m_pUKFont->OutTextMultiline( rtScale.left
            , rtScale.top
			, strMsg, color, 0xff000000, 1.0f, &rtScale, sortFlag, nCount
			);
    }//if.. else..
}

void CKTDGFontManager::CUKFont::DrawFontStyle(const WCHAR* strMsg, RECT rt, DWORD sortFlag, D3DXCOLOR outlineColor, FONT_STYLE fontStyle, int nCount /* = -1  */)
{
	KTDXPROFILE();

	if( NULL == strMsg || strMsg[0] == NULL || nCount == 0 )
		return;

	LONG	gap = 1;
	RECT	rect = rt;
	RECT rtScale;
	float   fXScale = g_pKTDXApp->GetResolutionScaleX();
	float   fYScale = g_pKTDXApp->GetResolutionScaleY();

	switch( fontStyle )
	{
	case FS_NONE:
		break;
	case FS_SHADOW:
		rect.left	+= gap;
		rect.top	+= gap;
		rect.right	+= gap;
		rect.bottom	+= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		break;
	case FS_SHELL:
		rect		= rt;
		rect.left	-= gap;
		rect.right	-= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.left	+= gap;
		rect.right	+= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.top	-= gap;
		rect.bottom	-= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.top	+= gap;
		rect.bottom	+= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);

		rect		= rt;
		rect.left	-= gap;
		rect.top	-= gap;
		rect.right	-= gap;
		rect.bottom	-= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.left	-= gap;
		rect.top	+= gap;
		rect.right	-= gap;
		rect.bottom	+= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.left	+= gap;
		rect.top	-= gap;
		rect.right	+= gap;
		rect.bottom	-= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		rect		= rt;
		rect.left	+= gap;
		rect.top	+= gap;
		rect.right	+= gap;
		rect.bottom	+= gap;
		rtScale.left = (int) ( rect.left * fXScale );
		rtScale.top  = (int) ( rect.top * fYScale );
		rtScale.right = (int) ( rect.right * fXScale );
		rtScale.bottom = (int) ( rect.bottom * fYScale );
		m_pUKFont->OutTextMultiline( rtScale.left, rtScale.top, strMsg, outlineColor, 0xff000000, 1.0f, &rtScale, DT_LEFT|DT_TOP, nCount
			);
		break;
	}
}

void CKTDGFontManager::CUKFont::Flush()
{
	int nFontNum = 0;
	int i = 0;

	nFontNum = (int)m_vecFont2D.size();

	//float   fFontScaleY = (float) m_pUKFont->GetAdjustedFontSize() / (float) m_pUKFont->GetOrgFontSize();
	//float   fAspectRatio = g_pKTDXApp->GetResolutionScaleX() / fFontScaleY;
	float   fScaleX = g_pKTDXApp->GetResolutionScaleX();
	float   fScaleY = g_pKTDXApp->GetResolutionScaleY();
	//float   fScaleX = 1.f;
	//float   fScaleY = 1.f;

	if( nFontNum != 0 )
	{
		for( i = 0; i < nFontNum; ++i )
		{
			FontArticle* pArticle	= m_vecFont2D[i];

			if ( pArticle == NULL )
				continue;

			pArticle->rect.left		= (int)pArticle->pos.x;
			pArticle->rect.top		= (int)pArticle->pos.y;


			if( true == pArticle->bSpread )
			{
				wstring wstrTemp = pArticle->strMsg.substr(0, pArticle->iSpreadCount );

				if ( pArticle->sortFlag & DT_SINGLELINE )
					OutTextXY( pArticle->rect.left, pArticle->rect.top, wstrTemp.c_str(), pArticle->color, pArticle->fontStyle
					, pArticle->outLineColor, &pArticle->rect, pArticle->sortFlag, wstrTemp.size(), fScaleX, fScaleY );
				else
					OutTextMultiline( pArticle->rect.left, pArticle->rect.top, wstrTemp.c_str(), pArticle->color, pArticle->fontStyle
					, pArticle->outLineColor, 1.f, &pArticle->rect, pArticle->sortFlag, wstrTemp.size(), fScaleX, fScaleY );

			}
			else
			{
				if ( pArticle->sortFlag & DT_SINGLELINE )
					OutTextXY( pArticle->rect.left, pArticle->rect.top, pArticle->strMsg.c_str(), pArticle->color, pArticle->fontStyle
					, pArticle->outLineColor, &pArticle->rect, pArticle->sortFlag, pArticle->strMsg.size(), fScaleX, fScaleY );
				else
					OutTextMultiline( pArticle->rect.left, pArticle->rect.top, pArticle->strMsg.c_str(), pArticle->color, pArticle->fontStyle
					, pArticle->outLineColor, 1.f, &pArticle->rect, pArticle->sortFlag, pArticle->strMsg.size(), fScaleX, fScaleY );
			}

			if( pArticle->bInstance == true )
			{
				SAFE_DELETE( pArticle );
				m_vecFont2D.erase( m_vecFont2D.begin() + i );
				--nFontNum;
				--i;
			}
		}

		nFontNum = 0;
	}
}

void CKTDGFontManager::CUKFont::OnFrameMove( float fElapsedTime )
{
	int nFontNum = 0;
	int i = 0;

	nFontNum = (int)m_vecFont2D.size();

	for( i = 0; i < nFontNum; i++ )
	{
		FontArticle* pArticle = m_vecFont2D[i];
		if( false == pArticle->bSpread )
			continue;

		pArticle->fNowSpreadTime += fElapsedTime;
		if( pArticle->fNowSpreadTime < pArticle->fSpreadTimeGap )
			continue;

		pArticle->fNowSpreadTime = 0.f;

		if( pArticle->iSpreadCount < (int) pArticle->strMsg.length() )
			pArticle->iSpreadCount++;
	}
}

void CKTDGFontManager::CUKFont::OnFrameRender()
{
	Flush();
}

int CKTDGFontManager::CUKFont::Push2DText( const WCHAR* strMsg, int xPos, int yPos, D3DXCOLOR color, D3DXCOLOR outlineColor, DWORD sortFlag, FONT_STYLE fontStyle, bool bInstance, bool bSpread, float fSpreadTimeGap )
{
	if( bSpread == true )
	{
		FontArticle* pFontArticle = new FontArticle;

		m_TextID++;

		pFontArticle->bSpread		 = true;
		pFontArticle->fSpreadTimeGap = fSpreadTimeGap;
		pFontArticle->fNowSpreadTime = 0.f;
		pFontArticle->iSpreadCount	 = 0;

		pFontArticle->color			= color;
		pFontArticle->outLineColor	= outlineColor;
		pFontArticle->pos.x			= (float)xPos;
		pFontArticle->pos.y			= (float)yPos;
		pFontArticle->strMsg		= strMsg;
		pFontArticle->bInstance		= bInstance;
		pFontArticle->sortFlag		= sortFlag;
		pFontArticle->ID			= m_TextID;
		pFontArticle->fontStyle		= fontStyle;

		m_vecFont2D.push_back( pFontArticle );

		return m_TextID;
	}
	else
	{
		FontArticle* pFontArticle = new FontArticle;

		m_TextID++;

		pFontArticle->color			= color;
		pFontArticle->outLineColor	= outlineColor;
		pFontArticle->pos.x			= (float)xPos;
		pFontArticle->pos.y			= (float)yPos;
		pFontArticle->strMsg		= strMsg;
		pFontArticle->bInstance		= bInstance;
		pFontArticle->sortFlag		= sortFlag;
		pFontArticle->ID			= m_TextID;
		pFontArticle->fontStyle		= fontStyle;

		m_vecFont2D.push_back( pFontArticle );

		return m_TextID;
	}
}

void CKTDGFontManager::CUKFont::Draw2DText( const WCHAR* strMsg, int xPos, int yPos, D3DXCOLOR color, D3DXCOLOR outlineColor, DWORD sortFlag, FONT_STYLE fontStyle, int nCount )
{
	KTDXPROFILE();

	if( NULL == strMsg )
		return;

	RECT rect;
	rect.left		= xPos;
	rect.top		= yPos;
	rect.right		= xPos;
	rect.bottom		= yPos;

	//float   fFontScaleY = (float) m_pUKFont->GetAdjustedFontSize() / (float) m_pUKFont->GetOrgFontSize();
	//float   fAspectRatio = g_pKTDXApp->GetResolutionScaleX() / fFontScaleY;

	if ( sortFlag & DT_SINGLELINE )
		OutTextXY( rect.left, rect.top, strMsg, color, fontStyle
		, outlineColor, &rect, sortFlag, nCount
		);
	else
		OutTextMultiline( rect.left, rect.top, strMsg, color, fontStyle
		, outlineColor, 1.0f, &rect, sortFlag, nCount
		);

}

CKTDGFontManager::FontArticle* CKTDGFontManager::CUKFont::Get2DArticle( const int ID )
{
	for( int i = 0; i < (int)m_vecFont2D.size(); ++i )
	{
		if( m_vecFont2D[i]->ID == ID )
		{
			return m_vecFont2D[i];
		}
	}
	return NULL;
}

bool CKTDGFontManager::CUKFont::Delete2DFont( const int ID )
{
	for( int i = 0; i < (int)m_vecFont2D.size(); ++i )
	{
		if( m_vecFont2D[i]->ID == ID )
		{
			SAFE_DELETE( m_vecFont2D[i] );
			m_vecFont2D.erase( m_vecFont2D.begin() + i );
			return true;
		}
	}
	return false;
}

void CKTDGFontManager::CUKFont::Clear2DFont()
{
	int i = 0;
	int nFontNum = 0;

	nFontNum = (int)m_vecFont2D.size();
	for( int i = 0; i < nFontNum; ++i )
	{
		SAFE_DELETE( m_vecFont2D[i] );
	}
	m_vecFont2D.clear();
}

#ifdef DLL_BUILD
bool CKTDGFontManager::InstallFont( const char* pValueName, const char* pFontFileName )
{
	WCHAR FontPath[MAX_PATH] = {0};
	WCHAR tempFontFileName[MAX_PATH] = {0};
	WCHAR FontFileName[MAX_PATH] = {0};
	WCHAR FilePath[MAX_PATH] = {0};

	wstring fontFileName;
	wstring realFontFileName;
	ConvertUtf8ToWCHAR( fontFileName, pFontFileName );
	ConvertUtf8ToWCHAR( realFontFileName, pFontFileName );

	StringCchPrintf( FontFileName, MAX_PATH ,  L"%s", fontFileName.c_str() );
	StringCchPrintf( tempFontFileName, MAX_PATH , L"%s", fontFileName.c_str() );
	/*wsprintf( FontFileName, L"%s", fontFileName.c_str() );
	wsprintf( tempFontFileName, L"%s", fontFileName.c_str() );*/

	if ( g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( tempFontFileName ) == NULL )
		return false;

	StringCchPrintf( FilePath, MAX_PATH , g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( FontFileName ) );
	//wsprintf( FilePath, g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( FontFileName ) );

	if ( AddFontResource( FilePath ) == FALSE )
	{
		return false;
	}

	m_UseFontNameList.push_back( FilePath );

	return true;

}
#endif // DLL_BUILD

void CKTDGFontManager::InstallFont( string strFontName, string strFontPath )
{
    if ( strFontName.empty() == true || strFontPath.empty() == true )
        return;
    MakeUpperCase( strFontName );
    MakeUpperCase( strFontPath );

	if( m_mapFontNameFile.find( strFontName ) != m_mapFontNameFile.end() )
		return;
    m_mapFontNameFile[ strFontName ] = strFontPath;

    FontFileMemoryMap::const_iterator iter = m_mapFontFileMemory.find( strFontPath );
    if ( iter != m_mapFontFileMemory.end() )
        return;

	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER  info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( strFontPath );
	if( info->pRealData == NULL || info->size <= 0 )
	{
		ErrorLogMsg( KEM_ERROR16, strFontPath.c_str() );
		MessageBoxA( NULL, strFontPath.c_str(), "Font Error!", NULL );
		return;
	}
    auto ib = m_mapFontFileMemory.insert( FontFileMemoryMap::value_type( strFontPath, std::vector<BYTE>() ) );
    ib.first->second.reserve( info->size );
    ib.first->second.assign( (const BYTE*) info->pRealData, (const BYTE*) ( info->pRealData + info->size ) );

}

const std::vector<BYTE>& CKTDGFontManager::GetFontMemory( string strFontName ) const
{
    static std::vector<BYTE>    s_vecDummy;

    if ( strFontName.empty() == true )
        return s_vecDummy;
    MakeUpperCase( strFontName );

	FontNameFileMap::const_iterator iter = m_mapFontNameFile.find( strFontName );
    if ( iter == m_mapFontNameFile.end() )
        return  s_vecDummy;

	FontFileMemoryMap::const_iterator mitUseFontMemory = m_mapFontFileMemory.find( iter->second );
	if( mitUseFontMemory == m_mapFontFileMemory.end() )
	{
		return s_vecDummy;
	}
    return  mitUseFontMemory->second;
}
