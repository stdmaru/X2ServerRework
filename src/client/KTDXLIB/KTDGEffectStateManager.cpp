#include <stdafx.h>

#include    "KTDGEffectStateManager.h"


CKTDGEffectStateManager::CKTDGEffectStateManager()
{
    m_iStackPointer = 0;
    m_bInEffect = false;

    m_bd3dVertexShader = false;
    m_pd3dVertexShader = NULL;
    m_bd3dPixelShader = false;
    m_pd3dPixelShader = NULL;
    m_bFVF = false;
    m_pd3dVertexDeclaration = NULL;
    m_bd3dMaterial = false;
    ZeroMemory( &m_d3dMaterial, sizeof( m_d3dMaterial ) );

    m_iStateManagerStackTop = 0;

    //ZeroMemory( m_apd3dTextures, sizeof( m_apd3dTextures ) );

	m_uiNumFloatVSConstants = x2::d3d9::kVertexShaderConstants;

	m_uiFirstDirtyFloatVSReg = m_uiNumFloatVSConstants;
	m_uiFirstCleanFloatVSReg = 0;
    m_pfFloatVSConstantData = NULL;
	if (m_uiNumFloatVSConstants > 0)
	{
		unsigned int uiNumElements = m_uiNumFloatVSConstants * 4;
		m_pfFloatVSConstantData = new float[uiNumElements];
		memset(m_pfFloatVSConstantData, 0, uiNumElements * sizeof(*m_pfFloatVSConstantData));
	}
}//CKTDGEffectStateManager::CKTDGEffectStateManager()


CKTDGEffectStateManager::~CKTDGEffectStateManager()
{
    ASSERT( !m_bInEffect );
    if ( m_bInEffect )
        RestoreEffectState();

	SAFE_DELETE_ARRAY( m_pfFloatVSConstantData );
}//CKTDGEffectStateManager::~CKTDGEffectStateManager()


HRESULT     CKTDGEffectStateManager::PrepareEffectState()
{
    if ( m_bInEffect )
        return E_FAIL;
    ASSERT( m_iStackPointer == 0 );
    ASSERT( !m_bitsetRenderState.any() );
    ASSERT( !m_bitsetTextureStageState.any() );
    ASSERT( !m_bitsetSamplerState.any() );
    ASSERT( !m_bd3dVertexShader );
    ASSERT( !m_bd3dPixelShader );
    ASSERT( !m_bFVF );
    //ASSERT( !m_bitsetTextures.any() );

    m_iStackPointer = 0;
    m_bInEffect = true;

    m_bd3dVertexShader = false;
    m_pd3dVertexShader = NULL;
    m_bd3dPixelShader = false;
    m_pd3dPixelShader = NULL;
    m_bFVF = false;
    m_pd3dVertexDeclaration = NULL;
    m_bd3dMaterial = false;
    //ZeroMemory( m_apd3dTextures, sizeof( m_apd3dTextures ) );

    m_iStateManagerStackTop = CKTDGStateManager::GetStackTop();

    return S_OK;
}//CKTDGEffectStateManager::PrepareEffectState()



HRESULT     CKTDGEffectStateManager::RestoreEffectState()
{
    if ( !m_bInEffect )
        return E_FAIL;

    if ( g_pd3dDevice != NULL )
    {
        ASSERT( m_iStateManagerStackTop <= CKTDGStateManager::GetStackTop() );
        CKTDGStateManager::PopStates( m_iStateManagerStackTop );
        m_iStateManagerStackTop = 0;

        while( m_iStackPointer > 0 )
        {
            m_iStackPointer--;
            const CKTDGStateManager::KState& kTop = _GetStackTop();
            switch( kTop.m_eType )
            {
            case STATE_TYPE_RENDER:
                m_bitsetRenderState.reset( kTop.m_dwType );
                CKTDGStateManager::SetRenderState( (D3DRENDERSTATETYPE) kTop.m_dwType, kTop.m_dwValue );
                break;
            case STATE_TYPE_TEXTURE_STAGE:
                m_bitsetTextureStageState.reset( kTop.m_dwStage * D3DTSS_MAX + kTop.m_dwType );
                CKTDGStateManager::SetTextureStageState( kTop.m_dwStage, (D3DTEXTURESTAGESTATETYPE) kTop.m_dwType, kTop.m_dwValue );
                break;
            case STATE_TYPE_SAMPLER:
                m_bitsetSamplerState.reset( kTop.m_dwStage * D3DSSS_MAX + kTop.m_dwType );
                CKTDGStateManager::SetSamplerState( kTop.m_dwStage, (D3DSAMPLERSTATETYPE) kTop.m_dwType, kTop.m_dwValue );
                break;
            default:
                ASSERT( !"invalid device state type" );
            }//switch
        }//while

        if ( m_bFVF )
        {
            g_pd3dDevice->SetVertexDeclaration( m_pd3dVertexDeclaration );
            SAFE_RELEASE( m_pd3dVertexDeclaration );
            m_bFVF = false;
        }//if

        if ( m_bd3dVertexShader )
        {
            g_pd3dDevice->SetVertexShader( m_pd3dVertexShader );
            SAFE_RELEASE( m_pd3dVertexShader );
            m_bd3dVertexShader = false;
        }//if

        if ( m_bd3dPixelShader )
        {
            g_pd3dDevice->SetPixelShader( m_pd3dPixelShader );
            SAFE_RELEASE( m_pd3dPixelShader );
            m_bd3dPixelShader = false;
        }//if

        if ( m_bd3dMaterial )
        {
            g_pd3dDevice->SetMaterial( &m_d3dMaterial );
            m_bd3dMaterial = false;
        }//if

    }
    else
    {
        m_iStackPointer = 0;
        m_bitsetRenderState.reset();
        m_bitsetTextureStageState.reset();
        m_bitsetSamplerState.reset();
        //m_bitsetTextures.reset();

        m_bd3dVertexShader = false;
        m_pd3dVertexShader = NULL;
        m_bd3dPixelShader = false;
        m_pd3dPixelShader = NULL;
        m_bFVF = false;
        m_pd3dVertexDeclaration = NULL;
        m_bd3dMaterial = false;
        //ZeroMemory( m_apd3dTextures, sizeof( m_apd3dTextures ) );

        m_iStateManagerStackTop = 0;
    }//if.. else..

    m_bInEffect = false;

    return S_OK;
}//CKTDGEffectStateManager::RestoreEffectState()


void CKTDGEffectStateManager::Release()
{
	delete this;
}//CKTDGEffectStateManager::Release()


// The following methods are called by the Effect when it wants to make
// the corresponding device call.  Note that:
// 1. Users manage the state and are therefore responsible for making the
//    the corresponding device calls themselves inside their callbacks.
// 2. Effects pay attention to the return values of the callbacks, and so
//    users must pay attention to what they return in their callbacks.

HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
    ASSERT( m_bInEffect );
    g_pd3dDevice->SetTransform( State, pMatrix );
    return  D3D_OK;
}//CKTDGEffectStateManager::SetTransform()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetMaterial(THIS_ CONST D3DMATERIAL9 *pMaterial)
{
    ASSERT( m_bInEffect );

    if ( !m_bd3dMaterial )
    {
        g_pd3dDevice->GetMaterial( &m_d3dMaterial );
        m_bd3dMaterial = true;
    }//if

    g_pd3dDevice->SetMaterial( pMaterial );
    return  D3D_OK;
}//CKTDGEffectStateManager::SetMaterial()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetLight(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight)
{
    ASSERT( m_bInEffect );
    return  g_pd3dDevice->SetLight( Index, pLight ) ? D3D_OK : D3DERR_INVALIDCALL;
}//CKTDGEffectStateManager::SetLight()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::LightEnable(THIS_ DWORD Index, BOOL Enable)
{
    ASSERT( m_bInEffect );
    return  g_pd3dDevice->LightEnable( Index, Enable ) ? D3D_OK : D3DERR_INVALIDCALL;
}//CKTDGEffectStateManager::LightEnable()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value)
{
    ASSERT( m_bInEffect );
    ASSERT( State >= D3DRENDERSTATETYPE(0) && State < D3DRS_MAX );
    if ( !( State >= D3DRENDERSTATETYPE(0) && State < D3DRS_MAX ) )
        return E_FAIL;
    if ( !m_bitsetRenderState.test( State ) )
    {
        _PushStackTop( KRenderState( State, CKTDGStateManager::GetRenderState( State ) ) );
        m_bitsetRenderState.set( State );
    }//if
    CKTDGStateManager::SetRenderState( State, Value );
    return S_OK;
}//CKTDGEffectStateManager::SetRenderState()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetTexture(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
{
    ASSERT( m_bInEffect );

    return  CKTDGStateManager::SetTexture( Stage, pTexture );
}//CKTDGEffectStateManager::SetTexture()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    ASSERT( m_bInEffect );
    ASSERT( Stage >= 0 && Stage < TEXTURE_STAGE_NUM && Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );
    if ( !( Stage >= 0 && Stage < TEXTURE_STAGE_NUM && Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX ) )
        return E_FAIL;
    size_t  bit = Stage * D3DTSS_MAX + Type;
    if ( !m_bitsetTextureStageState.test( bit ) )
    {
        _PushStackTop( KTextureStageState( Stage, Type, CKTDGStateManager::GetTextureStageState( Stage, Type ) ) );
        m_bitsetTextureStageState.set( bit );
    }//if
    CKTDGStateManager::SetTextureStageState( Stage, Type, Value );
    return S_OK;
}//CKTDGEffectStateManager::SetTextureStageState()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetSamplerState(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    ASSERT( m_bInEffect );
    ASSERT( Sampler >= 0 && Sampler < SAMPLER_STAGE_NUM && Type >= D3DSAMPLERSTATETYPE( 0 ) && Type < D3DSSS_MAX );
    if ( !( Sampler >= 0 && Sampler < SAMPLER_STAGE_NUM && Type >= D3DSAMPLERSTATETYPE( 0 ) && Type < D3DSSS_MAX ) )
        return E_FAIL;
    size_t  bit = Sampler * D3DSSS_MAX + Type;
    if ( !m_bitsetSamplerState.test( bit ) )
    {
        _PushStackTop( KSamplerState( Sampler, Type, CKTDGStateManager::GetSamplerState( Sampler, Type ) ) );
        m_bitsetSamplerState.set( bit );
    }//if
    CKTDGStateManager::SetSamplerState( Sampler, Type, Value );
    return S_OK;
}//CKTDGEffectStateManager::SetSamplerState()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetNPatchMode(THIS_ FLOAT NumSegments)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetNPatchMode()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetFVF(THIS_ DWORD FVF)
{
    ASSERT( m_bInEffect );

    if ( !m_bFVF )
    {
        g_pd3dDevice->GetVertexDeclaration( &m_pd3dVertexDeclaration );
        m_bFVF = true;
    }//if

    g_pd3dDevice->SetFVF( FVF );
    return  D3D_OK;
}//CKTDGEffectStateManager::SetFVF()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetVertexShader(THIS_ LPDIRECT3DVERTEXSHADER9 pShader)
{
    ASSERT( m_bInEffect );

    if ( !m_bd3dVertexShader )
    {
        g_pd3dDevice->GetVertexShader( &m_pd3dVertexShader );
        m_bd3dVertexShader = true;
    }//if

    g_pd3dDevice->SetVertexShader( pShader );
    return  D3D_OK;
}//CKTDGEffectStateManager::SetVertexShader()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetVertexShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetVertexShaderConstantI()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetVertexShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetVertexShaderConstantB()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetPixelShader(THIS_ LPDIRECT3DPIXELSHADER9 pShader)
{
    ASSERT( m_bInEffect );

    if ( !m_bd3dPixelShader )
    {
        g_pd3dDevice->GetPixelShader( &m_pd3dPixelShader );
        m_bd3dPixelShader = true;
    }//if

    g_pd3dDevice->SetPixelShader( pShader );
    return  D3D_OK;
}//CKTDGEffectStateManager::SetPixelShader()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetPixelShaderConstantF(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetPixelShaderConstantF()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetPixelShaderConstantI(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetPixelShaderConstantI()


HRESULT STDMETHODCALLTYPE CKTDGEffectStateManager::SetPixelShaderConstantB(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
    ASSERT( m_bInEffect );
    return  D3D_OK;   // ignored: no shaders read it until the effects are rewritten (T37)
}//CKTDGEffectStateManager::SetPixelShaderConstantB()
