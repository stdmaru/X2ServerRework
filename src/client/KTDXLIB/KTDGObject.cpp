#include "stdafx.h"
#include "./KTDGObject.h"
#include "./KTDXTickInterpolation.h"


// constructor.
CKTDGObject::CKTDGObject( CKTDGObject* pManagerObject_ )
: m_eObjectType( OT_UNKNOWN )
, m_uRefCounter( 0 )
, m_vScaleByUnit( 1, 1, 1 )

{

    m_pManagerObject        = pManagerObject_;
    if ( pManagerObject_ == NULL )
    {
        m_pMatrix           = new CKTDGMatrix( g_pKTDXApp->GetDevice() );
        ASSERT( m_pMatrix != NULL );
    }
    else
    {
        m_pMatrix = NULL;
    }//if


    m_bAlphaObject      = false;
    m_fDistanceToCamera = 0.0f;
    m_Sphere.fRadius    = 0.0f;
    m_Sphere.center     = D3DXVECTOR3( 0,0,0 );
    m_bForceLayer       = false;
    m_Layer             = 0;
//{{ robobeg : 2008-10-13
    //m_RenderStateID       = 0;
//}} robobeg : 2008-10-13

    m_bOverUI           = false;
    m_bInnerUI          = false;
    m_bShowObject       = true;

//{{ robobeg : 2009-01-10
    m_bInstantShow = false;
//}} robobeg : 2009-01-10


    m_bCulled           = false;
    //{{ seojt: 2008.10.23
    //m_dwLastAccessTime  = 0.0;
    //}} seojt

//{{ robobeg : 2008-10-13
#ifdef CONVERSION_VS
	m_hKTDGManagerHandle = g_pKTDXApp->GetDGManager()->GetRenderObjectChainEnd();
#else
    m_hKTDGManagerHandle = NULL;
#endif // CONVERSION_VS
//}} robobeg : 2008-10-13
}//CKTDGObject::CKTDGObject()


CKTDGObject::~CKTDGObject(void)
{
    SAFE_DELETE( m_pMatrix );
}//CKTDGObject::~CKTDGObject(void)


//{{ robobeg : 2008-10-17
void CKTDGObject::SetManagerObject( CKTDGObject* pManagerObject_ )
{
    //ASSERT( IsManagedByDGManager() == false );

    m_pManagerObject = pManagerObject_;
    if ( pManagerObject_ == NULL )
    {
        if ( m_pMatrix == NULL )
        {
            m_pMatrix = new CKTDGMatrix( g_pKTDXApp->GetDevice() );
            ASSERT( m_pMatrix != NULL );
        }//if
    }
    else
    {
        SAFE_DELETE( m_pMatrix );
    }//if.. else..
}//CKTDGObject::SetRefMatrix()
//}} robobeg : 2008-10-17



/*static*/ void CKTDGObject::DeleteKTDGObject( CKTDGObject* pObject_ )
{
    //ASSERT( pObject_ != NULL );
    if( pObject_ == NULL )
    {
        return;
    }//if

    if( pObject_->IsManagedByDGManager() )
    {
        if( g_pKTDXApp != NULL && g_pKTDXApp->GetDeviceManager() != NULL )
        {
		    g_pKTDXApp->GetDGManager()->RemoveObjectChain( pObject_ );
        }//if

    }//if

	pObject_->SetManagerObject( NULL );
    delete pObject_;
}//CKTDGObject::DeleteKTDGObject()
#ifdef CONVERSION_VS
bool CKTDGObject::IsManagedByDGManager() const
{
	return m_hKTDGManagerHandle != g_pKTDXApp->GetDGManager()->GetRenderObjectChainEnd();
}

bool CKTDGObject::IsRecursivelyManagedByDGManager() const
{
	return ( m_hKTDGManagerHandle != g_pKTDXApp->GetDGManager()->GetRenderObjectChainEnd() ) ? true :
		( m_pManagerObject != NULL ? m_pManagerObject->IsRecursivelyManagedByDGManager() : false );
}
#endif // CONVERSION_VS


void CKTDGObject::NoteTickMatrix( int iTick )
{
    if ( m_pManagerObject == NULL && m_pMatrix == NULL )
        return;
    CKTDGMatrix& kMatrix = GetMatrix();
    m_vTickPos = kMatrix.GetPos();
    m_vTickRotate = kMatrix.GetRotate();
    m_vTickScale = kMatrix.GetScale();
    m_iTickMatrix = iTick;
}

// Whether the object is drawn between the last two steps this frame: what it moved and turned in the last step, and the
// fraction of a step to draw it at. Not when the last step did not note it, nor on a jump or a turn over 30 degrees.
bool CKTDGObject::GetRenderStep( D3DXVECTOR3& vMoved, D3DXVECTOR3& vTurned, float& t )
{
    if ( m_pManagerObject == NULL && m_pMatrix == NULL )
        return false;
    CKTDGMatrix& kMatrix = GetMatrix();
    t = g_pKTDXApp->GetTickInterpolation();
    if ( m_iTickMatrix < 0 || m_iTickMatrix != g_pKTDXApp->GetFrameMoveCount() - 1 || t >= 1.f )
        return false;
    vMoved = kMatrix.GetPos() - m_vTickPos;
    vTurned = kMatrix.GetRotate() - m_vTickRotate;
    constexpr float fSnapRadians = 0.5236f;   // 30 degrees in one step: a turn, not a motion
    return D3DXVec3Dot( &vMoved, &vMoved ) <= x2::ktdx::kSnapDistance * x2::ktdx::kSnapDistance
        && fabsf( vTurned.x ) <= fSnapRadians && fabsf( vTurned.y ) <= fSnapRadians && fabsf( vTurned.z ) <= fSnapRadians;
}

D3DXMATRIX CKTDGObject::GetRenderMatrix( CKTDGMatrix::BILLBOARD_TYPE billboardType )
{
    CKTDGMatrix& kMatrix = GetMatrix();
    D3DXVECTOR3 vMoved, vTurned;
    float t = 1.f;
    if ( !GetRenderStep( vMoved, vTurned, t ) )
        return kMatrix.GetMatrix( billboardType );
    CKTDGMatrix kBetween( NULL );
    kBetween.Move( m_vTickPos + vMoved * t );
    kBetween.Rotate( m_vTickRotate + vTurned * t );
    kBetween.Scale( m_vTickScale + ( kMatrix.GetScale() - m_vTickScale ) * t );
    return kBetween.GetMatrix( billboardType );
}

D3DXVECTOR3 CKTDGObject::GetRenderOffset()
{
    D3DXVECTOR3 vMoved, vTurned;
    float t = 1.f;
    if ( !GetRenderStep( vMoved, vTurned, t ) )
        return D3DXVECTOR3( 0.f, 0.f, 0.f );
    return vMoved * ( t - 1.f );
}
