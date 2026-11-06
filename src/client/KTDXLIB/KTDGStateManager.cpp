#include "stdafx.h"
/** @file   CKTDGStateManager.cpp
	@desc   implementation of CKTDGStateManager
*/

#include "KTDGStateManager.h"

extern LPDIRECT3DDEVICE9        g_pd3dDevice;

BOOL                        CKTDGStateManager::ms_bRestored = FALSE;

DWORD                       CKTDGStateManager::ms_adwRenderState[D3DRS_MAX];
DWORD                       CKTDGStateManager::ms_adwTextureStageState[TEXTURE_STAGE_NUM][D3DTSS_MAX];
DWORD                       CKTDGStateManager::ms_adwSamplerState[SAMPLER_STAGE_NUM][D3DSSS_MAX];

CKTDGStateManager::KState   CKTDGStateManager::ms_akStack[DEVICE_STATE_STACK_SIZE];
int                         CKTDGStateManager::ms_iStackPointer = 0;

D3DVIEWPORT                 CKTDGStateManager::ms_d3dBackBufferViewport;
LPDIRECT3DTEXTURE9          CKTDGStateManager::ms_pd3dRenderTarget = NULL;
D3DVIEWPORT                 CKTDGStateManager::ms_d3dRenderTargetViewport;


void    CKTDGStateManager::InvalidateDeviceObjects()
{
    if ( !ms_bRestored )
        return;

    ms_iStackPointer = 0;
    ms_pd3dRenderTarget = NULL;
    ms_bRestored = FALSE;

    for( DWORD i = 0; i < MAX_TEXTURE_NUM; i++ )
        g_pd3dDevice->SetTexture( i, NULL );
}//InvalidateDeviceObjects()


namespace
{
    DWORD   FloatBits( float f ) { DWORD dw; memcpy( &dw, &f, sizeof dw ); return dw; }

    // D3D9's default value of a texture stage state, for every stage (the device runs fewer stages than the eight
    // cached here).
    DWORD   DefaultTextureStageState( DWORD dwStage, DWORD dwType )
    {
        switch( dwType )
        {
        case D3DTSS_COLOROP:        return dwStage == 0 ? D3DTOP_MODULATE : D3DTOP_DISABLE;
        case D3DTSS_ALPHAOP:        return dwStage == 0 ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE;
        case D3DTSS_COLORARG1:
        case D3DTSS_ALPHAARG1:      return D3DTA_TEXTURE;
        case D3DTSS_COLORARG0:
        case D3DTSS_COLORARG2:
        case D3DTSS_ALPHAARG0:
        case D3DTSS_ALPHAARG2:
        case D3DTSS_RESULTARG:      return D3DTA_CURRENT;
        case D3DTSS_TEXCOORDINDEX:  return dwStage;
        default:                    return 0;   // the bump-mapping matrix and offsets (0.0f), TTFF_DISABLE, CONSTANT
        }//switch
    }

    DWORD   DefaultSamplerState( DWORD dwType )
    {
        switch( dwType )
        {
        case D3DSAMP_ADDRESSU:
        case D3DSAMP_ADDRESSV:
        case D3DSAMP_ADDRESSW:      return D3DTADDRESS_WRAP;
        case D3DSAMP_MAGFILTER:
        case D3DSAMP_MINFILTER:     return D3DTEXF_POINT;
        case D3DSAMP_MAXANISOTROPY: return 1;
        case D3DSAMP_MIPMAPLODBIAS: return FloatBits( 0.0f );
        default:                    return 0;   // border color 0, MIPFILTER NONE, MAXMIPLEVEL 0, SRGB off...
        }//switch
    }
}


void    CKTDGStateManager::RestoreDeviceObjects()
{
    if ( ms_bRestored )
        return;

    ASSERT( g_pd3dDevice != NULL );
    ASSERT( ms_iStackPointer == 0 );

    // D3D9's defaults, as the original set them (its table had the device's default values). The render states come
    // from KTDXLIB's device, which is created with them.
    for( DWORD i = 0; i < D3DRS_MAX; i++ )
        ms_adwRenderState[ i ] = g_pd3dDevice->GetRenderState( D3DRENDERSTATETYPE( i ) );

    for( DWORD i = 0; i < TEXTURE_STAGE_NUM; i++ )
    {
        for( DWORD j = 0; j < D3DTSS_MAX; j++ )
        {
            ms_adwTextureStageState[ i ][ j ] = DefaultTextureStageState( i, j );
            g_pd3dDevice->SetTextureStageState( i, D3DTEXTURESTAGESTATETYPE( j ), ms_adwTextureStageState[ i ][ j ] );
        }//for
    }//for

    for( DWORD i = 0; i < SAMPLER_STAGE_NUM; i++ )
    {
        for( DWORD j = 0; j < D3DSSS_MAX; j++ )
        {
            ms_adwSamplerState[ i ][ j ] = DefaultSamplerState( j );
            g_pd3dDevice->SetSamplerState( i, D3DSAMPLERSTATETYPE( j ), ms_adwSamplerState[ i ][ j ] );
        }//for
    }//for

    for( DWORD i = 0; i < MAX_TEXTURE_NUM; i++ )
        g_pd3dDevice->SetTexture( i, NULL );

    ms_d3dBackBufferViewport.X = 0;
    ms_d3dBackBufferViewport.Y = 0;
    ms_d3dBackBufferViewport.Width = g_pd3dDevice->BackBufferWidth();
    ms_d3dBackBufferViewport.Height = g_pd3dDevice->BackBufferHeight();
    ms_d3dBackBufferViewport.MinZ = 0.f;
    ms_d3dBackBufferViewport.MaxZ = 1.f;

    SetRenderTarget( NULL );

    ms_bRestored = TRUE;
}//RestoreDeviceObjects()


void    CKTDGStateManager::SetRenderTarget( LPDIRECT3DTEXTURE9 pRenderTarget )
{
    g_pd3dDevice->SetRenderTarget( 0, pRenderTarget );
    ms_pd3dRenderTarget = pRenderTarget;

    if ( pRenderTarget == NULL )
    {
        // The back buffer's size follows the window.
        ms_d3dBackBufferViewport.Width = g_pd3dDevice->BackBufferWidth();
        ms_d3dBackBufferViewport.Height = g_pd3dDevice->BackBufferHeight();
        SetViewport( &ms_d3dBackBufferViewport );
        return;
    }//if

    D3DVIEWPORT viewport;
    viewport.X = viewport.Y = 0;
    viewport.Width = pRenderTarget->Width();
    viewport.Height = pRenderTarget->Height();
    viewport.MinZ = 0.f;
    viewport.MaxZ = 1.f;
    SetViewport( &viewport );
}//SetRenderTarget()
