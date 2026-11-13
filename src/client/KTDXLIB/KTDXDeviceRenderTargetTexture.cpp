#include "stdafx.h"
#include "./KTDXDeviceRenderTargetTexture.h"


CKTDXDeviceRenderTargetTexture::CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 /*pd3dDevice*/, wstring deviceID, int width, int height, D3DFORMAT fmt /*= D3DFMT_UNKNOWN*/ )
	: CKTDXDeviceBaseTexture( deviceID, DT_RENDER_TARGET_TEXTURE )
{
	m_DeviceType				= DT_RENDER_TARGET_TEXTURE;

	m_D3DFormat					= fmt;
	m_Width						= width;
	m_Height					= height;

	m_pRenderTargetTexture		= NULL;
	m_pBackupTarget				= NULL;

//{{ robobeg : 2011-03-18
    m_bWithoutFile = true;
//}} robobeg : 2011-03-18

    // the device is never lost or reset (T38): the target is created at once when there is one
    if ( g_pKTDXApp != NULL && g_pKTDXApp->GetDevice() != NULL )
    {
	    OnResetDevice();
    }//if

}

CKTDXDeviceRenderTargetTexture::~CKTDXDeviceRenderTargetTexture(void)
{
	OnLostDevice();
}

HRESULT CKTDXDeviceRenderTargetTexture::OnResetDevice()
{
	OnLostDevice();

	// The original took the back buffer's format and the depth buffer's description from the device's surfaces and
	// created a depth surface of its own; the device keeps one depth buffer, sized to the target in use.
	if( D3DFMT_UNKNOWN == m_D3DFormat )
	{
		m_D3DFormat = D3DFMT_A8R8G8B8;
	}

	m_pRenderTargetTexture = g_pKTDXApp->GetDevice()->CreateTexture( m_Width, m_Height, 1, D3DUSAGE_RENDERTARGET,
		m_D3DFormat, D3DPOOL_DEFAULT ).release();
	if( m_pRenderTargetTexture == NULL )
	{
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR78, strName.c_str());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKTDXDeviceRenderTargetTexture::OnLostDevice()
{
	if( m_pRenderTargetTexture != NULL && g_pKTDXApp->GetDevice()->GetRenderTarget() == m_pRenderTargetTexture )
	{
		CKTDGStateManager::SetRenderTarget( m_pBackupTarget );
	}
	SAFE_DELETE( m_pRenderTargetTexture );
	m_pBackupTarget = NULL;

	return S_OK;
}

void CKTDXDeviceRenderTargetTexture::BeginRender( bool bClear )
{
	KTDXPROFILE();
	if( m_pRenderTargetTexture == NULL )
	{
		return;
	}

	m_pBackupTarget = g_pKTDXApp->GetDevice()->GetRenderTarget();
	CKTDGStateManager::SetRenderTarget( m_pRenderTargetTexture );
	// T44: what is drawn in layout units into the target starts at its top left, not at the screen's centered column.
	m_vBackupUIOrigin = g_pKTDXApp->GetUIOrigin();
	m_fBackupUIAnchorShift = g_pKTDXApp->GetUIAnchorShiftInEffect();
	m_kBackupUIStretch = g_pKTDXApp->GetUIStretch();
	g_pKTDXApp->SetUIOrigin( D3DXVECTOR2( 0.f, 0.f ) );
	g_pKTDXApp->SetUIStretch( CKTDXApp::UIStretchState() );

	if( bClear == true )
	{
		g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	}
}

void CKTDXDeviceRenderTargetTexture::EndRender()
{
	KTDXPROFILE();
	// back to the target in use before BeginRender (the back buffer in the original, which saved it at creation)
	CKTDGStateManager::SetRenderTarget( m_pBackupTarget );
	m_pBackupTarget = NULL;
	g_pKTDXApp->SetUIOrigin( m_vBackupUIOrigin, m_fBackupUIAnchorShift );
	g_pKTDXApp->SetUIStretch( m_kBackupUIStretch );
}

void CKTDXDeviceRenderTargetTexture::Clear( D3DCOLOR color )
{
	KTDXPROFILE();

	g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
}

void CKTDXDeviceRenderTargetTexture::Draw( int nX, int nY, int nWidth, int nHeight, D3DCOLOR color /* = 0xffffffff  */)
{
	KTDXPROFILE();

    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() )

	D3DXVECTOR2 pos		= g_pKTDXApp->ConvertByResolution( (float)nX, (float)nY );
	D3DXVECTOR2 size	= g_pKTDXApp->ConvertSizeByResolution( (float)nWidth, (float)nHeight );



	float X1 = (float)pos.x - 0.5f;
	float Y1 = (float)pos.y - 0.5f;
	float X2 = (float)(pos.x + size.x);
	float Y2 = (float)(pos.y + size.y);

	float UMax = 1.0f;
	float VMax = 1.0f;

//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2
	CKTDXDeviceTexture::TEXTURE_RHW_VERTEX vertex[4];
	ZeroMemory( vertex, sizeof(CKTDXDeviceTexture::TEXTURE_RHW_VERTEX) * 4 );

	//Set up the 4 corners of a small square
	vertex[0].x = X1;     vertex[0].y = Y1;
	vertex[0].z = 1.0f;   vertex[0].rhw = 1.0f;
	vertex[0].u = 0.0f;   vertex[0].v = 0.0f;

	vertex[1].x = X1;     vertex[1].y = Y2;
	vertex[1].u = 0.0f;   vertex[1].v = VMax;
	vertex[1].z = 1.0f;   vertex[1].rhw = 1.0f;

	vertex[2].x = X2;     vertex[2].y = Y1;
	vertex[2].u = UMax;   vertex[2].v = 0.0f;
	vertex[2].z = 1.0f;   vertex[2].rhw = 1.0f;

	vertex[3].x = X2;     vertex[3].y = Y2;
	vertex[3].u = UMax;   vertex[3].v = VMax;
	vertex[3].z = 1.0f;   vertex[3].rhw = 1.0f;

	vertex[0].color = vertex[1].color =
	vertex[2].color = vertex[3].color = color;
//#endif

	SetDeviceTexture();

//#endif //REMOVE_DRAW_PRIMITIVE_UP_2

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	static_assert( D3DFVF_TEXTURE_RHW_VERTEX == D3DFVF_XYZRHW_DIFFUSE_TEX1 , "");
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLESTRIP, 2, vertex );
//#else
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_TEXTURE_RHW_VERTEX );
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(CKTDXDeviceTexture::TEXTURE_RHW_VERTEX) );
//#endif DYNAMIC_VERTEX_BUFFER_OPT
	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::RenderQuadRHW( D3DCOLOR color, float fX, float fY, float fWidth, float fHeight )
{
	KTDXPROFILE();
    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() );


	D3DVIEWPORT9 Viewport;
	g_pKTDXApp->GetDevice()->GetViewport(&Viewport);

	float X1, Y1, X2, Y2;
	if( fWidth == -1.f || fHeight == -1.f )
	{
		X1 = (float)Viewport.X;
		Y1 = (float)Viewport.Y;
		X2 = (float)Viewport.X + Viewport.Width;
		Y2 = (float)Viewport.Y + Viewport.Height;
	}
	else
	{
		X1 = fX;
		Y1 = fY;
		X2 = X1 + fWidth;
		Y2 = Y1 + fHeight;
	}

	float UMax = 1.0f;
	float VMax = 1.0f;

//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2
	RENDERTARGET_RHW_VERTEX vertex[4];
	ZeroMemory( vertex, sizeof(RENDERTARGET_RHW_VERTEX) * 4 );

	//Set up the 4 corners of a small square
	vertex[0].x = X1;     vertex[0].y = Y1;
	vertex[0].z = 1.0f;   vertex[0].rhw = 1.0f;
	vertex[0].u = 0.0f;   vertex[0].v = 0.0f;

	vertex[2].x = X1;     vertex[2].y = Y2;
	vertex[2].z = 1.0f;   vertex[2].rhw = 1.0f;
	vertex[2].u = 0.0f;   vertex[2].v = VMax;

	vertex[1].x = X2;     vertex[1].y = Y1;
	vertex[1].z = 1.0f;   vertex[1].rhw = 1.0f;
	vertex[1].u = UMax;   vertex[1].v = 0.0f;

	vertex[3].x = X2;     vertex[3].y = Y2;
	vertex[3].z = 1.0f;   vertex[3].rhw = 1.0f;
	vertex[3].u = UMax;   vertex[3].v = VMax;

	vertex[0].color = vertex[1].color =
		vertex[2].color = vertex[3].color = color;

//#endif

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	static_assert( D3DFVF_RENDERTARGET_RHW_VERTEX == D3DFVF_XYZRHW_DIFFUSE_TEX1 , "");
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLESTRIP, 2, vertex );
//#else   DYNAMIC_VERTEX_BUFFER_OPT
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_RENDERTARGET_RHW_VERTEX );
//
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(RENDERTARGET_RHW_VERTEX) );
//#endif  DYNAMIC_VERTEX_BUFFER_OPT

	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::RenderQuad( D3DCOLOR /*color*/ )
{
	KTDXPROFILE();
    KD3DPUSH( g_pKTDXApp->GetDGManager()->GetRSICKTDXDeviceTexture() )


	//HRESULT hr;

//#ifdef REMOVE_DRAW_PRIMITIVE_UP_2

	RENDERTARGET_VERTEX vertex[6];
	//ZeroMemory( vertex, sizeof(RENDERTARGET_VERTEX) * 6 );

	vertex[0].vPos = D3DXVECTOR3(-1,1,0);
	vertex[0].vTex = D3DXVECTOR2(0,0);

	vertex[1].vPos = D3DXVECTOR3(1,1,0);
	vertex[1].vTex = D3DXVECTOR2(1,0);

	vertex[2].vPos = D3DXVECTOR3(-1,-1,0);
	vertex[2].vTex = D3DXVECTOR2(0,1);

	vertex[3].vPos = D3DXVECTOR3(1,-1,0);
	vertex[3].vTex = D3DXVECTOR2(1,1);

	vertex[4].vPos = D3DXVECTOR3(-1,-1,0);
	vertex[4].vTex = D3DXVECTOR2(0,1);

	vertex[5].vPos = D3DXVECTOR3(1,1,0);
	vertex[5].vTex = D3DXVECTOR2(1,0);

	/*
	vertex[0].color = vertex[1].color =
		vertex[2].color = vertex[3].color =
			vertex[4].color = vertex[5].color = color;
			*/
//#endif //REMOVE_DRAW_PRIMITIVE_UP_2

//#ifdef  DYNAMIC_VERTEX_BUFFER_OPT
	static_assert( D3DFVF_RENDERTARGET_VERTEX == D3DFVF_XYZ_TEX1 , "");
	// DVB_TYPE_XYZRHW_DIFFUSE_TEX1 in the original, which read these XYZ_TEX1 vertices with the wrong layout
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZ_TEX1
		, D3DPT_TRIANGLELIST, 2, vertex );
//#else   DYNAMIC_VERTEX_BUFFER_OPT
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_RENDERTARGET_VERTEX );
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, (void*)vertex, sizeof(RENDERTARGET_VERTEX) );
//
//#endif  DYNAMIC_VERTEX_BUFFER_OPT
	KD3DEND()
}

void CKTDXDeviceRenderTargetTexture::SetDeviceTexture( int stage )
{
//{{ robobeg : 2008-10-22
	//if( g_pKTDXApp->IsSameBeforeTexture( stage, m_pRenderTargetTexture ) == false )
	//{
	//	if ( FAILED(g_pKTDXApp->GetDevice()->SetTexture( stage, m_pRenderTargetTexture ) ) )
	//	{
	//		string strName;
	//		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
	//		ErrorLogMsg( KEM_ERROR81, strName.c_str() );
	//		return;
	//	}
	//	g_pKTDXApp->SetBeforeTexture( stage, m_pRenderTargetTexture );
	//}
    if ( FAILED( CKTDGStateManager::SetTexture( stage, m_pRenderTargetTexture ) ) )
    {
		string strName;
		ConvertWCHARToChar( strName, m_DeviceID.c_str() );
		ErrorLogMsg( KEM_ERROR81, strName.c_str() );
    }//if
//}} robobeg : 2008-10-22
}

// StretchRect: x2::render has no blit yet and the back buffer is a swapchain image that cannot be read, so these
// fail quietly (they run every frame), as on the devices where the original set m_bCannotStretchRect.
HRESULT CKTDXDeviceRenderTargetTexture::CopySurface( LPDIRECT3DTEXTURE9 /*pDestSurface*/ )
{
	return E_NOTIMPL;
}

HRESULT CKTDXDeviceRenderTargetTexture::CopySurfaceToBackBuffer()
{
	return E_NOTIMPL;
}

HRESULT CKTDXDeviceRenderTargetTexture::CopyBackBufferToSurface()
{
	return E_NOTIMPL;
}
