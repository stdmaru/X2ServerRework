#include "stdafx.h"
#include "./KTDGManager.h"

//{{ robobeg : 2008-10-13
static const CKTDGStateManager::KState akStates[] =
{
	KRenderState( D3DRS_LIGHTING,			FALSE ),
	KRenderState( D3DRS_SPECULARENABLE,	FALSE ),
	KRenderState( D3DRS_ALPHABLENDENABLE,	FALSE ),
	KRenderState( D3DRS_ALPHATESTENABLE,	TRUE ),
	//KRenderState( D3DRS_ALPHAREF,			0x00000001 ),
	//KRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER ),
	KRenderState( D3DRS_ZENABLE,			TRUE ),
	KRenderState( D3DRS_ZWRITEENABLE,		TRUE ),
	KRenderState( D3DRS_CULLMODE,			D3DCULL_CCW ),
	KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
	KRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA ),
	KSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ),
	KSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ),
	KSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR ),
};//akStates[]


static const CKTDGStateManager::KState akRSICKTDXDeviceTexture[] =
{
	KRenderState( D3DRS_ALPHATESTENABLE,	TRUE ),
	KRenderState( D3DRS_ALPHAFUNC,			D3DCMP_NOTEQUAL ),
	KRenderState( D3DRS_ALPHAREF,			0 ),

	KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
	KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
	KRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA ),

	KRenderState( D3DRS_ZENABLE,			FALSE ),
	KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
	KRenderState( D3DRS_CULLMODE,			D3DCULL_CW ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE ),
};//


static const CKTDGStateManager::KState akRSICKTDGSlashTrace[] =
{
	KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
	KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
	KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),
	KRenderState( D3DRS_DESTBLEND,			D3DBLEND_DESTALPHA ),
	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_ADDSIGNED ),
};//





static const CKTDGStateManager::KState akRSICKTDGTexturedSlashTrace[] =
{

	KRenderState( D3DRS_ALPHATESTENABLE, FALSE ),
	KRenderState( D3DRS_ZENABLE, TRUE ),
	KRenderState( D3DRS_ZWRITEENABLE, FALSE ),
	KRenderState( D3DRS_CULLMODE, D3DCULL_NONE ),


	KRenderState( D3DRS_ALPHABLENDENABLE, TRUE ),
	KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
	//KRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA ),
	KRenderState( D3DRS_DESTBLEND,			D3DBLEND_DESTALPHA ),


	KTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 ),
	KTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE ),
	//KTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1, 	D3DTA_DIFFUSE ),
	//KTextureStageState( 0, D3DTSS_ALPHAARG2, 	D3DTA_TEXTURE ),


};





static const CKTDGStateManager::KState akRSICKTDGLineMap[] =
{
	KRenderState( D3DRS_LIGHTING,			FALSE ),
	//KRenderState( D3DRS_ZENABLE,			FALSE ),
	//KRenderState( D3DRS_ZWRITEENABLE,	FALSE ),
	//KRenderState( D3DRS_CULLMODE,		D3DCULL_CW ),

	KRenderState( D3DRS_ALPHABLENDENABLE,	FALSE ),
	//KRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA ),
	//KRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG2 ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE ),

	KTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 ),
	KTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE ),
	KTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE ),
};

static const CKTDGStateManager::KState akRSICKTDGPlanShadow[] =
{
	KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
	KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
};

static const CKTDGStateManager::KState akRSICKTDGUIDialog[] =
{
	KRenderState( D3DRS_ALPHABLENDENABLE, TRUE ),
	KRenderState( D3DRS_ALPHATESTENABLE, FALSE ),
	KRenderState( D3DRS_ZENABLE, FALSE ),
	KRenderState( D3DRS_ZWRITEENABLE, FALSE ),

	KTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ),
	KTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ),

	KTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ),
};


// Normal
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer0[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE ),
};//

// 덧셈: 광원/빛 효과 (밝아짐)
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer1[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE ),
};//

// 뺄셈: (어두움)
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer2[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_BLENDOP,			D3DBLENDOP_REVSUBTRACT ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),

	KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE ),
	KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE ),
};//

// 곱셈: 밝은곳이 투명해짐 (시야 출력)
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer3[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_BLENDOP,            D3DBLENDOP_MIN ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_ZERO ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),
};//

// 제곱: 밝은곳은 더 밝게 어두운곳은 더 어둡게
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer4[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_ZERO ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_DESTCOLOR ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),
};//

// 반전: 화면색 반전
/*static*/
static const CKTDGStateManager::KState akRSICKTDXDeviceLayer5[] =
{
    KRenderState( D3DRS_ALPHATESTENABLE,	FALSE ),

    KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
    KRenderState( D3DRS_SRCBLEND,			D3DBLEND_INVDESTCOLOR ),
    KRenderState( D3DRS_DESTBLEND,			D3DBLEND_ZERO ),

    KRenderState( D3DRS_ZENABLE,			FALSE ),
    KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
    KRenderState( D3DRS_CULLMODE,			D3DCULL_NONE ),
};//
//}} robobeg : 2008-10-13



CKTDGManager::CKTDGManager( HWND hWnd, LPDIRECT3DDEVICE9 /*pd3dDevice*/ )
: m_Near( 0.f )
, m_Far( 0.f )
, m_kCamera( g_pKTDXApp->GetDevice() )
, m_kFrustum( g_pKTDXApp->GetDevice() )
, m_uNonAlphaObjectNum( 0 )
{
	m_hWnd				= hWnd;

	m_ClearScreen		= 0;
	m_ClearScreenColor	= 0x00ffffff;
	m_ClearColor		= 0x32323232;
	m_Near				= 1.0f;
	m_Far				= 10000.0f;
	m_bPerspective		= true;


	m_fOrthoScale		= 1.f;


//{{ robobeg : 2008-10-13
//	InitializeCriticalSection( &m_csRenderObjectChainLock );
//}} robobeg : 2008-10-13

//{{ robobeg : 2008-10-13
	//CKTDGStateManager::Init();
	//m_BaseRenderStateID = CKTDGStateManager::RecordStateBegin();
	//{
	//	CKTDGStateManager::CaptureRenderState( D3DRS_LIGHTING,			FALSE );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_SPECULARENABLE,	FALSE );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	//	//CKTDGStateManager::CaptureRenderState( D3DRS_ALPHAREF,			0x00000001 );
	//	//CKTDGStateManager::CaptureRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_ZENABLE,			TRUE );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	//	CKTDGStateManager::CaptureRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );


	//	CKTDGStateManager::CaptureSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//	CKTDGStateManager::CaptureSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	//	CKTDGStateManager::CaptureSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	//}
	//CKTDGStateManager::RecordStateEnd();
//}} robobeg : 2008-10-13

//{{ robobeg : 2008-10-13
	//m_RSICKTDXDeviceTexture		= 0;
	//m_RSICKTDGSlashTrace		= 0;
	//m_RSICKTDGLineMap			= 0;
	//m_RSICKTDGPlanShadow		= 0;
	//m_RSICKTDGUIDialog			= 0;

 //   for(int i=0; i<6; ++i)
 //       m_RSICKTDXDeviceLayer[i]       = 0;
//}} robobeg : 2008-10-13

	BuildRenderStateID();

	//m_pCamera				= new CKTDGCamera( g_pKTDXApp->GetDevice() );
	//m_pFrustum				= new CKTDGFrustum( g_pKTDXApp->GetDevice() );
	m_pDialogManager		= new CKTDGUIDialogManager();
	//m_pGlow					= new CKTDGGlow( g_pKTDXApp->GetDevice() );
	//m_pMotionBlur			= new CKTDGMotionBlur( g_pKTDXApp->GetDevice() );
	m_pXRenderer			= new CKTDGXRenderer( g_pKTDXApp->GetDevice() );
	m_pFontManager			= new CKTDGFontManager( g_pKTDXApp->GetDevice() );
	//m_pGrayScale			= new CKTDGGrayScale();

//{{ robobeg : 2008-10-13
	//m_RenderObjectChain.reserve( 4096 );
	//m_AlphaObjectChain.reserve( 4096 );
	//m_NonAlphaObjectChain.reserve( 4096 );
//}} robobeg : 2008-10-13

	m_InnerUIChain.reserve( 512 );
	m_OverUIChain.reserve( 512 );
	m_OverUIAlphaChain.reserve( 512 );

	m_bFading						= false;
	m_bFadeOut						= true;
	m_fTimeFade						= 0.0f;
	m_fTimeFadeMax					= 0.0f;
	m_fFadeAlpha					= 0.0f;
	m_pTextureFadeInOut				= NULL;



//{{ robobeg : 2008-10-18
    m_uAlphaObjectNum = 0;
    m_nCull = 0;
//}} robobeg : 2008-10-18

	m_fPerspective = 4.f;


}

CKTDGManager::~CKTDGManager(void)
{
	//SAFE_DELETE( m_pGrayScale );
	//SAFE_DELETE( m_pCamera );
	//SAFE_DELETE( m_pFrustum );
	SAFE_DELETE( m_pDialogManager );
	//SAFE_DELETE( m_pGlow );
	//SAFE_DELETE( m_pMotionBlur );
	SAFE_DELETE( m_pXRenderer );
	SAFE_DELETE( m_pFontManager );
	SAFE_CLOSE( m_pTextureFadeInOut );


//{{ robobeg : 2008-10-13
	//CKTDGStateManager::Free();
//}} robobeg : 2008-10-13

//{{ robobeg : 2008-10-13
	//EnterCriticalSection( &m_csRenderObjectChainLock );
//}} robobeg : 2008-10-13
	m_RenderObjectChain.clear();
	//m_AlphaObjectChain.clear();
	//m_NonAlphaObjectChain.clear();
//{{ robobeg : 2008-10-13
	//LeaveCriticalSection( &m_csRenderObjectChainLock );
	//DeleteCriticalSection( &m_csRenderObjectChainLock );
//}} robobeg : 2008-10-13

}

void CKTDGManager::BuildRenderStateID()
{
	m_RSICKTDXDeviceTexture = akRSICKTDXDeviceTexture;
	m_RSICKTDGSlashTrace = akRSICKTDGSlashTrace;
	m_RSICKTDGTexturedSlashTrace = akRSICKTDGTexturedSlashTrace;
	m_RSICKTDGLineMap = akRSICKTDGLineMap;
	m_RSICKTDGPlanShadow = akRSICKTDGPlanShadow;
	m_RSICKTDGUIDialog = akRSICKTDGUIDialog;


    // Normal
    m_RSICKTDXDeviceLayer[0] = akRSICKTDXDeviceLayer0;
    // 덧셈: 광원/빛 효과 (밝아짐)
    m_RSICKTDXDeviceLayer[1] = akRSICKTDXDeviceLayer1;

    // 뺄셈: (어두움)
    m_RSICKTDXDeviceLayer[2] = akRSICKTDXDeviceLayer2;

    // 곱셈: 밝은곳이 투명해짐 (시야 출력)
    m_RSICKTDXDeviceLayer[3] = akRSICKTDXDeviceLayer3;

    // 제곱: 밝은곳은 더 밝게 어두운곳은 더 어둡게
    m_RSICKTDXDeviceLayer[4] = akRSICKTDXDeviceLayer4;

    // 반전: 화면색 반전
    m_RSICKTDXDeviceLayer[5] = akRSICKTDXDeviceLayer5;

}

bool CKTDGManager::BeginRender()
{
	KTDXPROFILE();

	if( m_ClearScreen > 0 )
	{
		m_ClearScreen--;

		const float MAGIC_CLEAR_SCREEN_GAP = 0.2f;
		if( m_ClearScreenTimer.elapsed() > MAGIC_CLEAR_SCREEN_GAP )
		{
			g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearScreenColor, 1.0f, 0);
			m_ClearScreenTimer.restart();
			return false;
		}
	}

	//if( m_pMotionBlur->GetBlurEnable() == true )
	//	m_pMotionBlur->BeginRender();
	//else
		g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearColor, 1.0f, 0);

	//m_pGlow->BeginRender();

	return true;
}

void CKTDGManager::EndRender()
{
	KTDXPROFILE();

	//m_pGlow->EndRender();

	//if( m_pMotionBlur->GetBlurEnable() == true )
	//{
	//	m_pMotionBlur->EndRender();
	//}
}



//{{ robobeg : 2008-10-13
void    CKTDGManager::_RenderObjectRenderHintVector( const CKTDGManager::KObjectRenderHintVector& vecIn_, bool bInLayerExclusive_ )
{


    RENDER_HINT   kRenderHint;
    int               iRenderHintLayer = 0;

    HRESULT hr;
    int iStart = 0;
    int iIndex = 0;
    int iCount = 0;
    int iIndex2;


    KD3DBEGIN()

        CKTDGStateManager::KStateID stateID;
        int     iStackTop = CKTDGStateManager::GetStackTop();

        iStart = 0;
        iIndex = 0;
        iCount = 0;

        for( const KObjectRenderHint& kObjectRenderHint : vecIn_ )
        {
            ASSERT( kObjectRenderHint.m_pObject != NULL );
			if( kObjectRenderHint.m_pObject == NULL )
			{
				ErrorLogMsg( KEM_ERROR422, "RenderHint pObject Is NULL" );
				continue;
			}


                if ( iCount > 0
                    && ( !kRenderHint.IsSameCategory( kObjectRenderHint.m_kRenderHint ) || ( bInLayerExclusive_ && iRenderHintLayer != kObjectRenderHint.m_pObject->GetLayer() ) ) )
                {
                    ASSERT( kRenderHint.m_eHint == RENDER_HINT::XMESH
                        || kRenderHint.m_eHint == RENDER_HINT::XSKINMESH );

                    if ( iCount == 1 )
                    {
                        const KObjectRenderHint& prev = vecIn_[ iStart ];

                        if( prev.m_pObject->GetRenderStateID() != stateID )
			            {
                            CKTDGStateManager::PopStates( iStackTop );
                            stateID = prev.m_pObject->GetRenderStateID();
                            CKTDGStateManager::PushStates( stateID );
			            }//if
                        m_pXRenderer->SetRenderPassFogAndMaxSkinningVertices( prev.m_kRenderHint.m_bFog, prev.m_kRenderHint.m_uMaxSkinningVertices );
                        prev.m_pObject->OnFrameRender_Draw();
                    }
                    else
                    {

                        if ( stateID != CKTDGStateManager::KStateID() )
                        {
                            CKTDGStateManager::PopStates( iStackTop );
                            stateID = CKTDGStateManager::KStateID();
                        }//if

                        for( hr = m_pXRenderer->BeginRenderGroup( kRenderHint.m_eRenderType ); SUCCEEDED( hr );
                            hr = m_pXRenderer->NextRenderGroup() )
                        {
                            for( iIndex2 = iStart; iIndex2 < iIndex; iIndex2++ )
                            {
                                const KObjectRenderHint& prev = vecIn_[ iIndex2 ];

			                        if( prev.m_pObject->GetRenderStateID() != stateID )
			                        {
                                        CKTDGStateManager::PopStates( iStackTop );
                                        stateID = prev.m_pObject->GetRenderStateID();
                                        CKTDGStateManager::PushStates( stateID );
			                        }//if
                                    m_pXRenderer->SetRenderPassFogAndMaxSkinningVertices( prev.m_kRenderHint.m_bFog, prev.m_kRenderHint.m_uMaxSkinningVertices );
                                    prev.m_pObject->OnFrameRender_Draw();


                            }//for
                        }//for

                        stateID = CKTDGStateManager::KStateID();
                    }//if.. else..
                    iCount = 0;
                }//if
                if ( kObjectRenderHint.m_kRenderHint.m_eHint == RENDER_HINT::XMESH
                    || kObjectRenderHint.m_kRenderHint.m_eHint == RENDER_HINT::XSKINMESH )
                {
                    if ( iCount == 0 )
                    {
                        kRenderHint = kObjectRenderHint.m_kRenderHint;
                        iRenderHintLayer = kObjectRenderHint.m_pObject->GetLayer();
                        iCount = 1;
                        iStart = iIndex;
                    }
                    else
                    {
                        ASSERT( kRenderHint.IsSameCategory( kObjectRenderHint.m_kRenderHint ) );
                        iCount++;
                    }//if.. else..
                }
                else
                {
                    ASSERT( iCount == 0 );
			        if( kObjectRenderHint.m_pObject->GetRenderStateID() != stateID )
			        {
                        CKTDGStateManager::PopStates( iStackTop );
                        stateID = kObjectRenderHint.m_pObject->GetRenderStateID();
                        CKTDGStateManager::PushStates( stateID );
			        }//if
                    kObjectRenderHint.m_pObject->OnFrameRender_Draw();
                }//if.. else..


            iIndex++;
        }//BOOST_TEST_FOREACH()


        if ( iCount > 0 )
        {
			ASSERT( kRenderHint.m_eHint == RENDER_HINT::XMESH
                || kRenderHint.m_eHint == RENDER_HINT::XSKINMESH );

            if ( iCount == 1 )
            {
                const KObjectRenderHint& prev = vecIn_[ iStart ];
			    if( prev.m_pObject->GetRenderStateID() != stateID )
			    {
                    CKTDGStateManager::PopStates( iStackTop );
                    stateID = prev.m_pObject->GetRenderStateID();
                    CKTDGStateManager::PushStates( stateID );
			    }//if
                m_pXRenderer->SetRenderPassFogAndMaxSkinningVertices( prev.m_kRenderHint.m_bFog, prev.m_kRenderHint.m_uMaxSkinningVertices );
                prev.m_pObject->OnFrameRender_Draw();
            }
            else
            {



                if ( stateID != CKTDGStateManager::KStateID() )
                {
                    CKTDGStateManager::PopStates( iStackTop );
                    stateID = CKTDGStateManager::KStateID();
                }//if

                for( hr = m_pXRenderer->BeginRenderGroup( kRenderHint.m_eRenderType ); SUCCEEDED( hr );
                    hr = m_pXRenderer->NextRenderGroup() )
                {
                    for( iIndex2 = iStart; iIndex2 < iIndex; iIndex2++ )
                    {
                        const KObjectRenderHint& prev = vecIn_[ iIndex2 ];


			                if( prev.m_pObject->GetRenderStateID() != stateID )
			                {
                                CKTDGStateManager::PopStates( iStackTop );
                                stateID = prev.m_pObject->GetRenderStateID();
                                CKTDGStateManager::PushStates( stateID );
			                }//if
                            m_pXRenderer->SetRenderPassFogAndMaxSkinningVertices( prev.m_kRenderHint.m_bFog, prev.m_kRenderHint.m_uMaxSkinningVertices );
                            prev.m_pObject->OnFrameRender_Draw();

                    }//for
                }//for

                stateID = CKTDGStateManager::KStateID();

            }//if.. else..
            iCount = 0;
        }//if

    KD3DEND()

}//CKTDGManager::_RenderObjectRenderHintVector()
//}} robobeg : 2008-10-13



void CKTDGManager::BackUIRender()
{
    _RenderObjectRenderHintVector( m_InnerUIChain );

	//후경 UI그리기
//{{ robobeg : 2008-10-13
    KD3DBEGIN()
	    m_pDialogManager->OnFrameRender( false );
    KD3DEND()
//}} robobeg : 2008-10-13
}


void CKTDGManager::ObjectChainNonAlphaRender()
{
	KTDXPROFILE();

	BackUIRender();


	for(int iLayer=0; iLayer<m_ciNumLayerGroups; ++iLayer)
	{
		_RenderObjectRenderHintVector( m_NonAlphaGroup[iLayer].m_layerObject, true );
	}//for
}

void CKTDGManager::ObjectChainAlphaRender()
{
	KTDXPROFILE();


    for(int iLayer=0; iLayer<m_ciNumLayerGroups; ++iLayer)
    {
        _RenderObjectRenderHintVector( m_AlphaGroup[iLayer].m_layerObject, true );
    }//for
}

void CKTDGManager::FrontUIRender()
{
	//전경 UI그리기
//{{ robobeg : 2008-10-13
    KD3DBEGIN()
	    m_pDialogManager->OnFrameRender( true );
    KD3DEND()
//}} robobeg : 2008-10-13

	// fade in, out effect
	if( m_fFadeAlpha > 0.0f )
	{
		D3DXCOLOR color = 0xffffffff;
		color.a = m_fFadeAlpha;
		if( m_pTextureFadeInOut != NULL )
			m_pTextureFadeInOut->Draw( 0, 0, 1024, 768, color );
	}

    _RenderObjectRenderHintVector( m_OverUIChain );
	_RenderObjectRenderHintVector( m_OverUIAlphaChain );

//}} robobeg : 2008-10-13
}




HRESULT CKTDGManager::OnResetDevice()
{
	////CKTDXThread::CLocker locker( m_csRenderObjectChainLock );

	CKTDGStateManager::RestoreDeviceObjects();
//{{ robobeg : 2008-10-13
    CKTDGStateManager::SetStates( akStates );
//}} robobeg : 2008-10-13

//{{ robobeg : 2008-10-13
	//for( int i = 0; i < (int)m_RenderObjectChain.size(); i++ )
	//{
	//	if ( m_RenderObjectChain[i] != NULL )
	//		m_RenderObjectChain[i]->OnResetDevice();
	//}
    for( CKTDGObject* pObject : m_RenderObjectChain )
    {
        if ( pObject != NULL )
            pObject->OnResetDevice();
    }//
//}} robobeg : 2008-10-13
	if ( m_pXRenderer )
		m_pXRenderer->OnResetDevice();

	if ( m_pFontManager != NULL )
		m_pFontManager->OnResetDevice();
	if ( m_pDialogManager != NULL )
		m_pDialogManager->OnResetDevice();
	//m_pGlow->OnResetDevice();
	//m_pMotionBlur->OnResetDevice();

	//m_pGrayScale->OnResetDevice();

	m_pTextureFadeInOut = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"FadeInOut.dds" );

	return S_OK;
}

HRESULT CKTDGManager::OnLostDevice()
{
	////CKTDXThread::CLocker locker( m_csRenderObjectChainLock );

//{{ robobeg : 2008-10-13
	//CKTDGStateManager::EndStateBlock( m_BaseRenderStateID );
//}} robobeg : 2008-10-13
	CKTDGStateManager::InvalidateDeviceObjects();

//{{ robobeg : 2008-10-13
	//for( int i = 0; i < (int)m_RenderObjectChain.size(); i++ )
	//{
	//	if ( m_RenderObjectChain[i] != NULL )
	//		m_RenderObjectChain[i]->OnLostDevice();
	//}
    for( CKTDGObject* pObject : m_RenderObjectChain )
    {
        if ( pObject != NULL )
            pObject->OnLostDevice();
    }//
//}} robobeg : 2008-10-13


	if ( m_pDialogManager != NULL )
		m_pDialogManager->OnLostDevice();
	if ( m_pXRenderer != NULL )
		m_pXRenderer->OnLostDevice();
	if ( m_pFontManager != NULL )
		m_pFontManager->OnLostDevice();
	//m_pGrayScale->OnLostDevice();

	return S_OK;
}

HRESULT CKTDGManager::OnFrameMove( double fTime, float fElapsedTime )
{
	m_pFontManager->OnFrameMove( (float)fTime, fElapsedTime );
	//m_pGrayScale->OnFrameMove( fElapsedTime );


	if( m_bFading == true )
	{
		if( m_bFadeOut == true )
		{
			m_fTimeFade += fElapsedTime;

			if( m_fTimeFade > m_fTimeFadeMax )
			{
				m_bFading			= false;
				m_bFadeOut			= false;
				m_fTimeFadeMax		= 0.0f;
				m_fTimeFade			= 0.0f;
				m_fFadeAlpha		= 1.0f;
			}
			else
			{
				m_fFadeAlpha		= m_fTimeFade / m_fTimeFadeMax;
			}
		}
		else
		{
			m_fTimeFade -= fElapsedTime;

			if( m_fTimeFade < 0.0f )
			{
				m_bFading			= false;
				m_bFadeOut			= false;
				m_fTimeFadeMax		= 0.0f;
				m_fTimeFade			= 0.0f;
				m_fFadeAlpha		= 0.0f;
			}
			else
			{
				m_fFadeAlpha		= m_fTimeFade / m_fTimeFadeMax;
			}
		}
	}


	return S_OK;
}

HRESULT CKTDGManager::OnFrameRender()
{
	//m_pFontManager->OnFrameRender();

	return S_OK;
}
#ifdef CONVERSION_VS
HRESULT CKTDGManager::AddObjectChain( CKTDGObject* object )
{
	KTDXPROFILE();
	if ( object == NULL )
		return E_FAIL;
	ASSERT( CKTDGObject::CKTDGManagerAccess::_GetDGManagerHandle( *object ) == GetRenderObjectChainEnd() );
	if ( CKTDGObject::CKTDGManagerAccess::_GetDGManagerHandle( *object ) != GetRenderObjectChainEnd() )
		return E_FAIL;
	CKTDGObject::CKTDGManagerAccess::_SetDGManagerHandle( *object, m_RenderObjectChain.insert( m_RenderObjectChain.end(), object ) );
	object->NotifyDGManagerChanged( *this );
	return S_OK;
}
HRESULT CKTDGManager::RemoveObjectChain( CKTDGObject* object )
{
	KTDXPROFILE();

	ASSERT( object != NULL );
	if ( object == NULL )
		return E_FAIL;
	if ( CKTDGObject::CKTDGManagerAccess::_GetDGManagerHandle( *object ) == GetRenderObjectChainEnd() )
		return E_FAIL;
	m_RenderObjectChain.erase( CKTDGObject::CKTDGManagerAccess::_GetDGManagerHandle( *object ) );
	CKTDGObject::CKTDGManagerAccess::_SetDGManagerHandle( *object, GetRenderObjectChainEnd() );
	object->NotifyDGManagerChanged( *this );
	return S_OK;
}
#endif // CONVERSION_VS

	void CKTDGManager::SetProjection( float Near, float Far, bool bPerspective /*= true*/, float fOrthoScale /*= 1.f*/ )
{
	RECT WindowRect;

	const D3DSURFACE_DESC* pbackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	if( pbackBufferSurfaceDesc )
	{
		WindowRect.left = 0;
		WindowRect.right = pbackBufferSurfaceDesc->Width;
		WindowRect.top = 0;
		WindowRect.bottom = pbackBufferSurfaceDesc->Height;
	}
	else
	{
		GetClientRect(m_hWnd, &WindowRect);
	}

	m_Near			= Near;
	m_Far			= Far;
	m_bPerspective	= bPerspective;

	float fWidth	= (float)(WindowRect.right - WindowRect.left);
	float fHeight	= (float)(WindowRect.bottom - WindowRect.top);
	if( fWidth <= 0 )
		fWidth = 1024;
	if( fHeight <= 0 )
		fHeight = 768;


	if( bPerspective == true )
	{
		//Create 3D Projection Matrix
		D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI / GetPerspectiveValue(),
			fWidth / fHeight,
			m_Near, m_Far );
	}
	else
	{
		m_fOrthoScale = fOrthoScale;
		D3DXMatrixOrthoLH( &m_matProjection, 1024 * fOrthoScale, 768 * fOrthoScale,	m_Near, m_Far );
	}

	g_pKTDXApp->SetProjectionTransform( &m_matProjection );
}

void CKTDGManager::SetProjectionFov( float Near, float Far, float fFov )
{
	RECT WindowRect;
	GetClientRect(m_hWnd, &WindowRect);

	m_Near			= Near;
	m_Far			= Far;
	m_bPerspective	= true;

	float fWidth	= (float)(WindowRect.right - WindowRect.left);
	float fHeight	= (float)(WindowRect.bottom - WindowRect.top);
	if( fWidth <= 0 )
		fWidth = 1024;
	if( fHeight <= 0 )
		fHeight = 768;

	//Create 3D Projection Matrix
	D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/fFov,
		fWidth / fHeight,
		m_Near, m_Far );

	g_pKTDXApp->SetProjectionTransform( &m_matProjection );
}

struct CKTDGObjectUISort
{
   bool operator()( const CKTDGManager::KObjectRenderHint& _Left, const CKTDGManager::KObjectRenderHint& _Right ) const
   {
       return ( _Left.m_kRenderHint < _Right.m_kRenderHint
                || ( !(_Right.m_kRenderHint < _Left.m_kRenderHint)
                     && _Left.m_pObject->GetDistanceToCamera() < _Right.m_pObject->GetDistanceToCamera() ) );
   };
};//struct CKTDGObjectUISort


struct CKTDGObjectNonAlphaSort
{
   bool operator()( const CKTDGManager::KObjectRenderHint& _Left, const CKTDGManager::KObjectRenderHint& _Right ) const
   {
       return _Left.m_pObject->GetLayer() < _Right.m_pObject->GetLayer()
           || ( _Left.m_pObject->GetLayer() == _Right.m_pObject->GetLayer()
                && ( _Left.m_kRenderHint < _Right.m_kRenderHint
                     || ( !(_Right.m_kRenderHint < _Left.m_kRenderHint)
                          && _Left.m_pObject->GetDistanceToCamera() < _Right.m_pObject->GetDistanceToCamera() ) ) );
   };
};//struct CKTDGObjectNonAlphaSort


struct CKTDGObjectAlphaSort
{
   bool operator()( const CKTDGManager::KObjectRenderHint& _Left, const CKTDGManager::KObjectRenderHint& _Right ) const
   {
       return   _Left.m_pObject->GetLayer() < _Right.m_pObject->GetLayer()
           || ( _Left.m_pObject->GetLayer() == _Right.m_pObject->GetLayer()
                && _Left.m_pObject->GetDistanceToCamera() > _Right.m_pObject->GetDistanceToCamera() );
   };
};//struct CKTDGObjectAlphaSort


//{{ robobeg : 2008-10-18

void CKTDGManager::ObjectChainSort()
{
	KTDXPROFILE();

	////CKTDXThread::CLocker locker( m_csRenderObjectChainLock );

	m_kFrustum.Construct( GetFar() );

    m_OverUIChain.resize( 0 );
	m_OverUIAlphaChain.resize( 0 );
    m_InnerUIChain.resize( 0 );

    //m_NonAlphaObjectChain.resize( 0 );

    for(int i=0; i<m_ciNumLayerGroups; ++i)
    {
        m_NonAlphaGroup[i].m_layerObject.resize( 0 );
        m_AlphaGroup[i].m_layerObject.resize( 0 );
    }

    m_nCull = 0;
    m_uAlphaObjectNum = 0;

    KObjectRenderHint   kObjectRenderHint;
    for( CKTDGObject* object : m_RenderObjectChain )
    {
        if(object == NULL)
            continue;

        object->SetInstantShow( false );


        if ( IsInFrustum( object ) == false )
        {
            ++m_nCull;
            continue;
        }

        //{{ seojt
        // frustum안에 있으면 마지막 접근 시간을 갱신한다. - jintaeks on 2008-10-22, 16:39
        //object->SetLastAccessTime( g_NowTime );
        //}} seojt

        kObjectRenderHint.m_pObject = object;
        kObjectRenderHint.m_kRenderHint = object->OnFrameRender_Prepare();
        if ( kObjectRenderHint.m_kRenderHint.m_eHint == RENDER_HINT::NORENDER )
            continue;

        object->SetInstantShow( true );

		//논알파, 알파 분류
        if( object->GetOverUI() == true )
		{
			if( object->IsAlphaObject() == false
				|| object->GetForceLayer() == true )
			{
				m_OverUIChain.push_back( kObjectRenderHint );
			}
			else
			{
				m_OverUIAlphaChain.push_back( kObjectRenderHint );
			}
		}
        else if ( object->GetInnerUI() == true )
        {
            m_InnerUIChain.push_back( kObjectRenderHint );
        }
        else
		{
            int iLayer = MapLayerGroup( object->GetLayer() );

            if( iLayer < 0)
                continue;
            ASSERT( iLayer < m_ciNumLayerGroups );
			if( object->IsAlphaObject() == false
				|| object->GetForceLayer() == true )
			{
				m_NonAlphaGroup[iLayer].m_layerObject.push_back(kObjectRenderHint);
				m_uNonAlphaObjectNum++;
			}
			else
			{
				m_AlphaGroup[iLayer].m_layerObject.push_back(kObjectRenderHint);
				m_uAlphaObjectNum++;
			}
		}
    }//BOOST_TEST_FOREACH()


    std::sort( m_OverUIChain.begin(), m_OverUIChain.end(), CKTDGObjectUISort() );
	std::sort( m_OverUIAlphaChain.begin(), m_OverUIAlphaChain.end(), CKTDGObjectUISort() );
    std::sort( m_InnerUIChain.begin(), m_InnerUIChain.end(), CKTDGObjectUISort() );

    for( int iLayer = 0; iLayer<m_ciNumLayerGroups; ++iLayer)
    {
        std::sort( m_NonAlphaGroup[iLayer].m_layerObject.begin(), m_NonAlphaGroup[iLayer].m_layerObject.end(), CKTDGObjectNonAlphaSort() );
        std::sort( m_AlphaGroup[iLayer].m_layerObject.begin(), m_AlphaGroup[iLayer].m_layerObject.end(), CKTDGObjectAlphaSort() );
    }//for
    //}
//#endif
}

//}} robobeg : 2008-10-18


void CKTDGManager::StartFadeIn( float fTime )
{
	m_bFading			= true;
	m_bFadeOut			= false;
	m_fTimeFadeMax		= fTime;
	m_fTimeFade			= fTime;
	m_fFadeAlpha		= 1.0f;
}

void CKTDGManager::StartFadeOut( float fTime )
{
	m_bFading			= true;
	m_bFadeOut			= true;
	m_fTimeFadeMax		= fTime;
	m_fTimeFade			= 0.0f;
	m_fFadeAlpha		= 0.0f;
}



//{{ robobeg : 2008-10-17

bool CKTDGManager::IsInFrustum( CKTDGObject* object )
{
	if ( object == NULL )
		return false;

	return object->IsInFrustum( GetCamera().GetEye(), m_kFrustum );
}

bool CKTDGManager::IsInFrustum( CKTDGObject* object, float& fFrameMoveUpdateTime )
{
	if ( object == NULL )
		return false;

	if( object->GetShowObject() == false )
		return false;


    D3DXVECTOR3 center;


	object->GetTransformCenter( &center );
	object->SetDistanceToCamera( GetDistance( GetCamera().GetEye(), center ) );

    if ( object->GetBoundingRadius() <= 0 )
    {
	    fFrameMoveUpdateTime = 0.0f;
        return true;
    }//if

	//컬링
    float   fScale = object->GetMaxScale();
    float fScaledBoundingRadius = object->GetBoundingRadius() * fScale;

	float fNeedDistance = 0.0f;
	if( m_kFrustum.CheckSphere( center, fScaledBoundingRadius, fNeedDistance ) == false )
	{
		//...... -_-a I want to use this code but............ T^T
		object->SetIsCulled( true );

		float fFrameMoveUpdateTime = 1/g_pKTDXApp->GetFrameMoveFPS(); //max frame rate!!
		int distanceTimes = (int)(fNeedDistance / object->GetBoundingRadius() * fScale); // 몇배일까?

		//최고 40배까지
		if ( distanceTimes >= 40 )
		{
			fFrameMoveUpdateTime = 999999.0f;
		}
		else if ( distanceTimes > 0 )
		{
			fFrameMoveUpdateTime = fFrameMoveUpdateTime * ( 1.0f + ( distanceTimes * 0.6f ) );
		}
		return false;
	}
	else
	{
		object->SetIsCulled( false );
	}

	fFrameMoveUpdateTime = 0.0f;
	return true;
}

//}} robobeg : 2008-10-17
