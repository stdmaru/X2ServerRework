#include "stdafx.h"
#include "./X2WorldLayer.h"

CX2WorldLayer::CX2WorldLayer( HWND hWnd )
{
	m_hWnd			= hWnd;

    m_color         = D3DXCOLOR(1, 1, 1, 1);


	m_FadeColor		= D3DXCOLOR( 1, 1, 1, 1 );
	m_fFadeTime		= 0.f;


	m_pLayerTex1 = NULL;
    SetLayer( XL_LENS_FLARE );

    SetAlphaObject( true );

    //SetTex("WorldLayer_Alpha.tga");
    //m_pLayerTex1	= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"WorldLayer_Alpha.tga" );


    SetPos(0.0f, 0.0f, 1.0f, 1.0f);

    m_SpeedUV = D3DXVECTOR2(0.0f, 0.0f);
    m_minTexUV = D3DXVECTOR2(0.01f, 0.01f);
    m_maxTexUV = D3DXVECTOR2(0.99f, 0.99f);
    m_feedbakUV = D3DXVECTOR2(0.0f, 0.0f);  // 1300

    //m_TexName = NULL;
	m_iLayerId = -1;
	m_bInitShow = true;

	m_BlendType = 0;

	//if ( DXUTGetDeviceObjectReset() == true )
	//{
		OnResetDevice();
	//}//if
}

CX2WorldLayer::~CX2WorldLayer(void)
{
	SAFE_CLOSE( m_pLayerTex1 );

    //if(m_TexName)
    //    delete m_TexName;   // fix!! delete_array()로 해야하지 않을까...
}

void CX2WorldLayer::SetTex( const char *texName )
{
    //if(m_TexName)
    //    delete m_TexName;

    //wstring wstrTexName;
	ConvertUtf8ToWCHAR( m_wstrTexName, texName );
    //string strTexName;
    //ConvertWCHARToChar( strTexName, wstrTexName.c_str() );
    //m_TexName = new char[strlen(strTexName.c_str())+1];
    //strcpy(m_TexName, strTexName.c_str());

	SAFE_CLOSE( m_pLayerTex1 );
    m_pLayerTex1 = g_pKTDXApp->GetDeviceManager()->OpenTexture( m_wstrTexName );
}

HRESULT CX2WorldLayer::OnFrameMove( double fTime, float fElapsedTime )
{
	if( GetShowObject() == false )
		return S_OK;

    CX2WorldObject::OnFrameMove( fTime, fElapsedTime );

    RECT WindowRect;
    GetClientRect( m_hWnd, &WindowRect );
    float xPos = (WindowRect.right - WindowRect.left) * m_PosX;
    float yPos = (WindowRect.bottom - WindowRect.top) * m_PosY;
    float Width	= (WindowRect.right - WindowRect.left) * m_fWidth;
    float Height	= (WindowRect.bottom - WindowRect.top) * m_fHeight;

    m_x = xPos;
    m_y = yPos;
    m_w = Width;
    m_h = Height;


    D3DXVECTOR3 nowPos = g_pKTDXApp->GetDGManager()->GetCamera().GetLookAt(); // GetEye();


    float speedU = m_SpeedUV.x * m_fElapsedTime;
    float speedV = m_SpeedUV.y * m_fElapsedTime;

    float stepU, stepV;


    m_minTexUV.x += speedU;
    m_minTexUV.y += speedV;
    m_maxTexUV.x += speedU;
    m_maxTexUV.y += speedV;

    stepU = stepV = 0.0f;
    if(m_feedbakUV.x != 0.0f)
    {
        stepU = nowPos.x - m_prePos.x;
        stepU = stepU / m_feedbakUV.x;

        m_minTexUV.x += stepU;
        m_maxTexUV.x += stepU;
    }

    if(m_feedbakUV.y != 0.0f)
    {
        stepV = nowPos.y - m_prePos.y;
        stepV = stepV / m_feedbakUV.y;

        m_minTexUV.y -= stepV;
        m_maxTexUV.y -= stepV;
    }


    if(m_maxTexUV.y < 0.0001f || m_minTexUV.y > 0.9999f)
    {
        m_minTexUV.y = 0.01f;
        m_maxTexUV.y = 0.99f;
    }

    if(m_maxTexUV.x < 0.0001f || m_minTexUV.x > 0.9999f)
    {
        m_minTexUV.x = 0.01f;
        m_maxTexUV.x = 0.99f;
    }

    m_prePos = nowPos;




	// fade color
	if( m_fFadeTime > 0.f )
	{
		m_fFadeTime -= fElapsedTime;
		if( m_fFadeTime <= 0.f )
		{
			m_fFadeTime = 0.f;
			m_color = m_FadeColor;
		}
		else
		{
			D3DXCOLOR remainColor = m_FadeColor - m_color;
			remainColor /= m_fFadeTime;
			m_color += remainColor * fElapsedTime;
		}
	}


    return S_OK;
}

void CX2WorldLayer::SetPos(float x, float y, float wx, float wy)
{
    m_PosX = x;
    m_PosY = y;
    m_fWidth = wx;
    m_fHeight = wy;

    RECT WindowRect;
    GetClientRect( m_hWnd, &WindowRect );
    float xPos = (WindowRect.right - WindowRect.left) * m_PosX;
    float yPos = (WindowRect.bottom - WindowRect.top) * m_PosY;
    float Width	= (WindowRect.right - WindowRect.left) * m_fWidth;
    float Height	= (WindowRect.bottom - WindowRect.top) * m_fHeight;

    m_x = xPos;
    m_y = yPos;
    m_w = Width;
    m_h = Height;


    m_prePos = g_pKTDXApp->GetDGManager()->GetCamera().GetLookAt();
}


//{{ robobeg : 2008-10-28
//HRESULT CX2WorldLayer::OnFrameRender()
/*virtual*/
void    CX2WorldLayer::OnFrameRender_Draw()
//}} robobeg : 2008-10-28
{
	if( GetShowObject() == false )
		return;


	if( NULL != m_pLayerTex1 )
	{
		m_pLayerTex1->DrawLayer(m_x, m_y, m_w, m_h, m_color, m_minTexUV, m_maxTexUV, m_BlendType);
	}
//	return S_OK;
}


HRESULT CX2WorldLayer::OnResetDevice()
{
    return S_OK;
}

HRESULT CX2WorldLayer::OnLostDevice()
{
	return S_OK;
}
