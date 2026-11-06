/** @file   CKTDGStateManager.h
	@desc   interface of CKTDGStateManager
	@author robobeg@kogstudios.com
    @since  2003-1-1

	@date   seojt on 2005-8-26, 11:26:53
            - example comment added
*/

#pragma once

// The engine's cache of device states, with a stack for push/pop (decision T38: on KTDXLIB's own device,
// docs/porting/client-d3d9.md). The Xbox and Direct3D 8 branches of the original are gone.

#define DEVICE_STATE_STACK_SIZE     512

#define TEXTURE_STAGE_NUM   8
#define SAMPLER_STAGE_NUM   16
#define MAX_TEXTURE_NUM     SAMPLER_STAGE_NUM

#define D3DRS_MAX           (D3DRS_BLENDOPALPHA+1)
#define D3DTSS_MAX          (D3DTSS_CONSTANT+1)
#define D3DSSS_MAX          (D3DSAMP_DMAPOFFSET+1)

typedef IDirect3DDevice9        IDirect3DDevice;
typedef LPDIRECT3DDEVICE9       LPDIRECT3DDEVICE;
typedef D3DVIEWPORT9            D3DVIEWPORT;

struct  KRenderState
{
    D3DRENDERSTATETYPE m_eType;
    DWORD   m_dwValue;

    KRenderState( D3DRENDERSTATETYPE Type, DWORD Value )
        : m_eType( Type ), m_dwValue( Value ) {}
};//struct  KRenderState

struct  KTextureStageState
{
    DWORD   m_dwStage;
    D3DTEXTURESTAGESTATETYPE m_eType;
    DWORD   m_dwValue;

    KTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value )
        : m_dwStage(Stage), m_eType(Type), m_dwValue(Value) {}
};//struct  KTextureStageState

struct  KSamplerState
{
    DWORD   m_dwStage;
    D3DSAMPLERSTATETYPE m_eType;
    DWORD   m_dwValue;

    KSamplerState( DWORD Stage, D3DSAMPLERSTATETYPE Type, DWORD Value )
        : m_dwStage(Stage), m_eType(Type), m_dwValue( Value ) {}
};//struct  KSamplerState

class   CKTDGStateManager
{
	public:

    enum    EStateType
    {
        STATE_TYPE_INVALID = -1,
        STATE_TYPE_RENDER = 0,
        STATE_TYPE_TEXTURE_STAGE = 1,
        STATE_TYPE_SAMPLER = 2,
    };//enum    EStackType

    struct  KState
    {
        EStateType  m_eType;
        DWORD       m_dwStage;
        DWORD       m_dwType;
        DWORD       m_dwValue;

        KState()
            : m_eType( STATE_TYPE_INVALID ), m_dwStage( 0 ), m_dwType( 0 ), m_dwValue( 0 ) {}
        KState( const KRenderState& kIn_ )
            : m_eType( STATE_TYPE_RENDER ), m_dwStage( 0), m_dwType( kIn_.m_eType ), m_dwValue( kIn_.m_dwValue ) {}
        KState& operator = ( const KRenderState& kIn_ )
        {
            m_eType = STATE_TYPE_RENDER;
            m_dwStage = 0;
            m_dwType = kIn_.m_eType;
            m_dwValue = kIn_.m_dwValue;
            return *this;
        }//operator = ()
        KState( const KTextureStageState& kIn_ )
            : m_eType( STATE_TYPE_TEXTURE_STAGE ), m_dwStage( kIn_.m_dwStage ), m_dwType( kIn_.m_eType ), m_dwValue( kIn_.m_dwValue ) {}
        KState& operator = ( const KTextureStageState& kIn_ )
        {
            m_eType = STATE_TYPE_TEXTURE_STAGE;
            m_dwStage = kIn_.m_dwStage;
            m_dwType = kIn_.m_eType;
            m_dwValue = kIn_.m_dwValue;
            return *this;
        }//operator = ()
        KState( const KSamplerState& kIn_ )
            : m_eType( STATE_TYPE_SAMPLER ), m_dwStage( kIn_.m_dwStage ), m_dwType( kIn_.m_eType ), m_dwValue( kIn_.m_dwValue ) {}
        KState& operator = ( const KSamplerState& kIn_ )
        {
            m_eType = STATE_TYPE_SAMPLER;
            m_dwStage = kIn_.m_dwStage;
            m_dwType = kIn_.m_eType;
            m_dwValue = kIn_.m_dwValue;
            return *this;
        }//operator = ()
    };//struct  KState


    struct  KStateID
    {
        const KState*   m_pkStates;
        unsigned        m_uNumStates;

        KStateID() : m_pkStates( NULL ), m_uNumStates( 0 ) {}

        template<size_t n>
        KStateID( const KState (&state)[n] )
            : m_pkStates( &state[0] ), m_uNumStates( n ) {}

        bool operator == ( const KStateID& kIn_ ) const { return m_pkStates == kIn_.m_pkStates && m_uNumStates == kIn_.m_uNumStates; }
        bool operator != ( const KStateID& kIn_ ) const { return m_pkStates != kIn_.m_pkStates || m_uNumStates != kIn_.m_uNumStates; }

        template<size_t n>
        KStateID& operator = ( const KState (&state)[n] )
        {
            m_pkStates = &state[0];
            m_uNumStates = n;
            return *this;
        }//

    };//struct  KStateID


private:

    static BOOL                         ms_bRestored;

    static DWORD                        ms_adwRenderState[D3DRS_MAX];
    static DWORD                        ms_adwTextureStageState[TEXTURE_STAGE_NUM][D3DTSS_MAX];
    static DWORD                        ms_adwSamplerState[SAMPLER_STAGE_NUM][D3DSSS_MAX];

    static KState                       ms_akStack[DEVICE_STATE_STACK_SIZE];
    static int                          ms_iStackPointer;

    static D3DVIEWPORT                  ms_d3dBackBufferViewport;
    static LPDIRECT3DTEXTURE9           ms_pd3dRenderTarget;
    static D3DVIEWPORT                  ms_d3dRenderTargetViewport;


private:

    static __forceinline KState&   _GetStackTop()
    {
        ASSERT( ms_iStackPointer < DEVICE_STATE_STACK_SIZE );
        return ms_akStack[ ms_iStackPointer ];
    }//


public:

    static void            InvalidateDeviceObjects();
    // Resets the caches and the device to D3D9's default states and binds the back buffer. The original took the
    // adapter and device type for its format checks, which are gone (no callers).
    static void            RestoreDeviceObjects();

    static __forceinline HRESULT SetRenderState(D3DRENDERSTATETYPE Type, DWORD Value)
    {
        extern LPDIRECT3DDEVICE9        g_pd3dDevice;
        ASSERT( Type >= D3DRENDERSTATETYPE(0) && Type < D3DRS_MAX );

        if ( ms_adwRenderState[Type] != Value )
        {
            g_pd3dDevice->SetRenderState( Type, Value );
            ms_adwRenderState[Type] = Value;
        }//if

        return S_OK;
    }//SetRenderState()


    static __forceinline  void  PushRenderState( D3DRENDERSTATETYPE Type )
    {
        ASSERT( Type >= D3DRENDERSTATETYPE(0) && Type < D3DRS_MAX );
        ASSERT( ms_iStackPointer < DEVICE_STATE_STACK_SIZE );
        ms_akStack[ ms_iStackPointer++ ] = KRenderState( Type, ms_adwRenderState[Type] );
    }//PushRenderState()

    static __forceinline  void    PushRenderState(D3DRENDERSTATETYPE Type, DWORD Value)
    {
        PushRenderState( Type );
        SetRenderState( Type, Value );
    }//PushRenderState()

    static __forceinline void     PopRenderState([[maybe_unused]] D3DRENDERSTATETYPE Type)
    {
        ASSERT( ms_iStackPointer > 0 );
        ms_iStackPointer--;
        const KState& kTop = _GetStackTop();
        ASSERT( kTop.m_eType == STATE_TYPE_RENDER && (D3DRENDERSTATETYPE) kTop.m_dwType == Type );
        SetRenderState( (D3DRENDERSTATETYPE) kTop.m_dwType, kTop.m_dwValue );
    }//PopRenderState()

    static __forceinline void    GetRenderState(D3DRENDERSTATETYPE Type, DWORD*  pValue)
    {
        ASSERT( Type >= D3DRENDERSTATETYPE(0) && Type < D3DRS_MAX );
        ASSERT( pValue != NULL );
        *pValue = ms_adwRenderState[Type];
    }//GetRenderState()
    static __forceinline DWORD    GetRenderState(D3DRENDERSTATETYPE Type)
    {
        ASSERT( Type >= D3DRENDERSTATETYPE(0) && Type < D3DRS_MAX );
        return  ms_adwRenderState[Type];
    }//GetRenderState()


    static __forceinline void     SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
    {
        extern LPDIRECT3DDEVICE9        g_pd3dDevice;
        ASSERT( Stage < TEXTURE_STAGE_NUM );
        ASSERT( Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );
        if ( ms_adwTextureStageState[Stage][Type] != Value )
        {
            g_pd3dDevice->SetTextureStageState( Stage, Type, Value );
            ms_adwTextureStageState[Stage][Type] = Value;
        }//if
    }//SetTextureStageState()

    static __forceinline void   GetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD*  pValue )
    {
        ASSERT( Stage < TEXTURE_STAGE_NUM );
        ASSERT( Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );
        ASSERT( pValue != NULL );
        *pValue = ms_adwTextureStageState[Stage][Type];
    }//GetTextureStageState()

    static __forceinline DWORD    GetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type )
    {
        ASSERT( Stage < TEXTURE_STAGE_NUM );
        ASSERT( Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );
        return  ms_adwTextureStageState[Stage][Type];
    }//GetTextureStageState()

    static __forceinline  void    PushTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type )
    {
        ASSERT( Stage < TEXTURE_STAGE_NUM );
        ASSERT( Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );

        ASSERT( ms_iStackPointer < DEVICE_STATE_STACK_SIZE );
        ms_akStack[ ms_iStackPointer++ ] = KTextureStageState( Stage, Type, ms_adwTextureStageState[Stage][Type] );
    }//PushTextureStageState()

    static __forceinline  void    PushTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
    {
        PushTextureStageState( Stage, Type );
        SetTextureStageState( Stage, Type, Value );
    }//PushTextureStageState()

    //static __forceinline void     PushTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, bool bInValue_ )
    //{
    //    PushTextureStageState( Stage, Type, (DWORD)( (bInValue_) ? TRUE : FALSE ) );
    //}//

    static __forceinline void     PopTextureStageState([[maybe_unused]] DWORD Stage, [[maybe_unused]] D3DTEXTURESTAGESTATETYPE Type)
    {
        ASSERT( Stage < TEXTURE_STAGE_NUM );
        ASSERT( Type >= D3DTEXTURESTAGESTATETYPE(0) && Type < D3DTSS_MAX );

        ASSERT( ms_iStackPointer > 0 );
        ms_iStackPointer--;
        const KState& kTop = _GetStackTop();
        ASSERT( kTop.m_eType == STATE_TYPE_TEXTURE_STAGE && kTop.m_dwStage == Stage && (D3DTEXTURESTAGESTATETYPE) kTop.m_dwType == Type );

        SetTextureStageState( kTop.m_dwStage, (D3DTEXTURESTAGESTATETYPE) kTop.m_dwType, kTop.m_dwValue );
    }//PopRenderState()


    static __forceinline void     SetSamplerState(DWORD Stage, D3DSAMPLERSTATETYPE Type, DWORD Value)
    {
        extern LPDIRECT3DDEVICE9        g_pd3dDevice;
        ASSERT( Stage < SAMPLER_STAGE_NUM );
        ASSERT( Type >= D3DSAMPLERSTATETYPE(0) && Type < D3DSSS_MAX );
        if ( ms_adwSamplerState[Stage][Type] != Value )
        {
            g_pd3dDevice->SetSamplerState( Stage, Type, Value );
            ms_adwSamplerState[Stage][Type] = Value;
        }//if
    }//SetSamplerState()


    static __forceinline void   GetSamplerState(DWORD Stage, D3DSAMPLERSTATETYPE Type, DWORD*  pValue )
    {
        ASSERT( Stage < SAMPLER_STAGE_NUM );
        ASSERT( Type >= D3DSAMPLERSTATETYPE(0) && Type < D3DSSS_MAX );
        ASSERT( pValue != NULL );
        *pValue = ms_adwSamplerState[Stage][Type];
    }//GetSamplerState()


    static __forceinline DWORD  GetSamplerState(DWORD Stage, D3DSAMPLERSTATETYPE Type)
    {
        ASSERT( Stage < SAMPLER_STAGE_NUM );
        ASSERT( Type >= D3DSAMPLERSTATETYPE(0) && Type < D3DSSS_MAX );
        return  ms_adwSamplerState[Stage][Type];
    }//GetSamplerState()


    static __forceinline  void    PushSamplerState(DWORD Stage, D3DSAMPLERSTATETYPE Type )
    {
        ASSERT( Stage < SAMPLER_STAGE_NUM );
        ASSERT( Type >= D3DSAMPLERSTATETYPE(0) && Type < D3DTSS_MAX );

        ASSERT( ms_iStackPointer < DEVICE_STATE_STACK_SIZE );
        ms_akStack[ ms_iStackPointer++ ] = KSamplerState( Stage, Type, ms_adwSamplerState[Stage][Type] );

    }//PushSamplerState()

    static __forceinline  void    PushSamplerState(DWORD Stage, D3DSAMPLERSTATETYPE Type, DWORD Value)
    {
        PushSamplerState( Stage, Type );
        SetSamplerState(Stage, Type, Value);
    }//PushSamplerState()

    //static __forceinline void     PushSamplerState( DWORD Stage, D3DSAMPLERSTATETYPE Type, bool bInValue_ )
    //{
    //    PushSamplerState( Stage, Type, (DWORD)( (bInValue_) ? TRUE : FALSE ) );
    //}//

    static __forceinline void     PopSamplerState([[maybe_unused]] DWORD Stage, [[maybe_unused]] D3DSAMPLERSTATETYPE Type)
    {
        ASSERT( Stage < SAMPLER_STAGE_NUM );
        ASSERT( Type >= D3DSAMPLERSTATETYPE(0) && Type < D3DTSS_MAX );

        ASSERT( ms_iStackPointer > 0 );
        ms_iStackPointer--;
        const KState& kTop = _GetStackTop();
        ASSERT( kTop.m_eType == STATE_TYPE_SAMPLER && kTop.m_dwStage == Stage && (D3DSAMPLERSTATETYPE) kTop.m_dwType == Type );

        SetSamplerState( kTop.m_dwStage, (D3DSAMPLERSTATETYPE) kTop.m_dwType, kTop.m_dwValue );
    }//PopRenderState()


    static __forceinline void     SetState( const KState& kInState_ )
    {
        switch( kInState_.m_eType )
        {
        case STATE_TYPE_RENDER:
            SetRenderState( (D3DRENDERSTATETYPE) kInState_.m_dwType, kInState_.m_dwValue );
            break;
        case STATE_TYPE_TEXTURE_STAGE:
            SetTextureStageState( kInState_.m_dwStage, (D3DTEXTURESTAGESTATETYPE) kInState_.m_dwType, kInState_.m_dwValue );
            break;
        case STATE_TYPE_SAMPLER:
            SetSamplerState( kInState_.m_dwStage, (D3DSAMPLERSTATETYPE) kInState_.m_dwType, kInState_.m_dwValue );
            break;
        default:
            ASSERT( !"invalid direct3d device state type" );
        }//switch
    }//SetState()

    static __forceinline void   PushState( const KState& kInState_ )
    {
        switch( kInState_.m_eType )
        {
        case STATE_TYPE_RENDER:
            PushRenderState( D3DRENDERSTATETYPE(kInState_.m_dwType), kInState_.m_dwValue );
            break;
        case STATE_TYPE_TEXTURE_STAGE:
            PushTextureStageState( kInState_.m_dwStage, D3DTEXTURESTAGESTATETYPE( kInState_.m_dwType ), kInState_.m_dwValue );
            break;
        case STATE_TYPE_SAMPLER:
            PushSamplerState( kInState_.m_dwStage, D3DSAMPLERSTATETYPE( kInState_.m_dwType ), kInState_.m_dwValue );
            break;
        default:
            ASSERT( !"invalid direct3d device state type" );
        }//switch
    }//PushState()



    static __forceinline void   PopState()
    {
        ASSERT( ms_iStackPointer > 0 );
        ms_iStackPointer--;
        SetState( _GetStackTop() );
    }//PopState()


    static __forceinline int    GetStackTop() { return ms_iStackPointer; }

    static __forceinline void   SetStates( const KState akInStateValues[], unsigned uInNumValues_ )
    {
        for( unsigned u = 0; u < uInNumValues_; u++ )
        {
            ASSERT( akInStateValues != NULL );
            SetState( akInStateValues[ u ] );
        }//if
    }//SetStates()

    static __forceinline int    PushStates( const KState akInStateValues[], unsigned uInNumValues_ )
    {
        int iOldStackTop = ms_iStackPointer;
        for( unsigned u = 0; u < uInNumValues_; u++ )
        {
            ASSERT( akInStateValues != NULL );
            PushState( akInStateValues[ u ] );
        }//if
        return iOldStackTop;
    }//PushStates()

    static __forceinline void   SetStates( const KStateID& kIn_ )
    {
        return SetStates( kIn_.m_pkStates, kIn_.m_uNumStates );
    }//PushStates()


    static __forceinline int    PushStates( const KStateID& kIn_ )
    {
        return PushStates( kIn_.m_pkStates, kIn_.m_uNumStates );
    }//PushStates()

    template<size_t n>
    static __forceinline void   SetStates( const KState (&state)[n] )
    {
        SetStates( state, n );
    }//PushStates()

    template<size_t n>
    static __forceinline int    PushStates( const KState (&state)[n] )
    {
        return PushStates( state, n );
    }//PushStates()


    static __forceinline void   PopStates( int iInOldStackTop_ )
    {
        while( ms_iStackPointer > iInOldStackTop_ )
            PopState();
    }//PopStates()


    // The device keeps the bindings (and unbinds a texture destroyed while bound), so there is no cache here: the
    // original's cache held a reference to each texture, which KTDXLIB's device has no counts for.
    static __forceinline HRESULT SetTexture( DWORD dwInIndex_, LPDIRECT3DBASETEXTURE9 pd3dTexture )
    {
        extern LPDIRECT3DDEVICE   g_pd3dDevice;
        g_pd3dDevice->SetTexture( dwInIndex_, pd3dTexture );
        return S_OK;
    }//SetTexture()


    static __forceinline void SetViewport(const D3DVIEWPORT *pViewport)
    {
        extern LPDIRECT3DDEVICE   g_pd3dDevice;
        g_pd3dDevice->SetViewport( pViewport );
        CopyMemory( &ms_d3dRenderTargetViewport, pViewport, sizeof(D3DVIEWPORT) );
    }//SetViewport()

    static __forceinline void     GetViewport(D3DVIEWPORT *pViewport)
    {
        CopyMemory( pViewport, &ms_d3dRenderTargetViewport, sizeof(D3DVIEWPORT) );
    }//GetViewport()

    static __forceinline const D3DVIEWPORT* GetViewport() { return  &ms_d3dRenderTargetViewport; }
    static __forceinline const D3DVIEWPORT* GetBackBufferViewport() { return &ms_d3dBackBufferViewport; }

    // A render target texture (D3DUSAGE_RENDERTARGET), or NULL for the back buffer. The original took a surface and
    // a depth-stencil surface; the device attaches its depth-stencil buffer to targets of the back buffer's size.
    // The viewport becomes the whole target, as in D3D9.
    static void SetRenderTarget( LPDIRECT3DTEXTURE9 pRenderTarget );
    static void RestoreRenderTarget() { SetRenderTarget( NULL ); }
    static __forceinline LPDIRECT3DTEXTURE9  GetRenderTarget() { return ms_pd3dRenderTarget; }


    __forceinline static HRESULT   SetFVF( DWORD dwFVF )
    {
        extern LPDIRECT3DDEVICE   g_pd3dDevice;
        g_pd3dDevice->SetFVF( dwFVF );
        return S_OK;
    }// SetFVF()

};//class   CKTDGStateManager



class  KD3DStack
{
public:

    KD3DStack() : m_iStackTop( CKTDGStateManager::GetStackTop() ) {}

    template<size_t n>
    KD3DStack( const CKTDGStateManager::KState (&state)[n] )
      : m_iStackTop( CKTDGStateManager::PushStates( state, n ) ) {}

    KD3DStack( const CKTDGStateManager::KStateID& kInID_ )
      : m_iStackTop( CKTDGStateManager::PushStates( kInID_ ) ) {}

    ~KD3DStack() { CKTDGStateManager::PopStates( m_iStackTop ); }

private:

    KD3DStack( const KD3DStack& );
    KD3DStack& operator = ( const KD3DStack& );

private:

    int m_iStackTop;
};//class  _KD3DSTACK


#define KD3DBEGIN()                { KD3DStack _kD3DStack;

#define KD3DPUSH( kStateArray )    { KD3DStack _kD3DStack( kStateArray );

#define KD3DEND()      }




/** @example    class CKTDGStateManager

  1) OnResetDevice():

    CKTDGStateManager::RestoreDeviceObjects();

    // Setup render state
    CKTDGStateManager::SetRenderState( D3DRS_LIGHTING, TRUE );
    CKTDGStateManager::SetRenderState( D3DRS_DITHERENABLE, TRUE );
    CKTDGStateManager::SetRenderState( D3DRS_ZENABLE, TRUE );
    CKTDGStateManager::SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    CKTDGStateManager::SetRenderState( D3DRS_AMBIENT, 0x33333333 );
    CKTDGStateManager::SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    CKTDGStateManager::SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    CKTDGStateManager::SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

  2) OnFrameRender():

    CKTDGStateManager::PushRenderState( D3DRS_LIGHTING, FALSE );
	g_stdMesh.DrawMesh();
	CKTDGStateManager::PopRenderState( D3DRS_LIGHTING );

	2) OnLostDevice():

	CKTDGStateManager::InvalidateDeviceObjects();

	*/
