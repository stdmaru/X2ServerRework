#include    "stdafx.h"
#include    "KTDGDynamicVBManager.h"

// See KTDGDynamicVBManager.h: everything is drawn with the device's DrawPrimitiveUP.

namespace
{

unsigned    NumVertices( D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives )
{
    if ( uNumPrimitives == 0 )
        return 0;
    switch( d3dPrimitiveType )
    {
    case D3DPT_POINTLIST:       return uNumPrimitives;
    case D3DPT_LINELIST:        return uNumPrimitives * 2;
    case D3DPT_LINESTRIP:       return uNumPrimitives + 1;
    case D3DPT_TRIANGLELIST:    return uNumPrimitives * 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:     return uNumPrimitives + 2;
    default:                    return 0;
    }//switch
}

unsigned    NumPrimitives( D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumVertices )
{
    switch( d3dPrimitiveType )
    {
    case D3DPT_POINTLIST:       return uNumVertices;
    case D3DPT_LINELIST:        return uNumVertices / 2;
    case D3DPT_LINESTRIP:       return uNumVertices > 1 ? uNumVertices - 1 : 0;
    case D3DPT_TRIANGLELIST:    return uNumVertices / 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:     return uNumVertices > 2 ? uNumVertices - 2 : 0;
    default:                    return 0;
    }//switch
}

}//namespace


CKTDGDynamicVBManager::CKTDGDynamicVBManager()
{
    // the original's sizes (vertices of one buffer)
    RegisterVBType( DVB_TYPE_XYZ_DIFFUSE, D3DFVF_XYZ_DIFFUSE, 1024, 1 );
    RegisterVBType( DVB_TYPE_XYZ_DIFFUSE_TEX1, D3DFVF_XYZ_DIFFUSE_TEX1, 1024 * 4, 1 );
    RegisterVBType( DVB_TYPE_XYZ_TEX1, D3DFVF_XYZ_TEX1, 1024, 1 );
    RegisterVBType( DVB_TYPE_XYZRHW_DIFFUSE_TEX1, D3DFVF_XYZRHW_DIFFUSE_TEX1, 1024 * 40, 1 );
}//CKTDGDynamicVBManager::CKTDGDynamicVBManager()

/*virtual*/
CKTDGDynamicVBManager::~CKTDGDynamicVBManager()
{
}//CKTDGDynamicVBManager::~CKTDGDynamicVBManager()


bool        CKTDGDynamicVBManager::RegisterVBType( EDYNAMIC_VB_TYPE eType, DWORD dwFVF, unsigned uNumVertices, int /*iVBNum*/ )
{
    if ( eType < EDYNAMIC_VB_TYPE( 0 ) || eType >= DVB_END )
        return false;
    if ( dwFVF == 0 || uNumVertices == 0 )
        return false;

    const DWORD dwVertexStride = D3DXGetFVFVertexSize( dwFVF );
    ASSERT( dwVertexStride != 0 );
    if ( dwVertexStride == 0 )
        return false;

    DynamicVBInfo& dvbInfo = m_aDynamicVBInfo[ eType ];
    if ( dvbInfo.IsRegistered() == true )
        return false;

    dvbInfo.m_uNumVertices = uNumVertices;
    dvbInfo.m_dwFVF = dwFVF;
    dvbInfo.m_dwVertexStride = dwVertexStride;
	return true;
}//CKTDGDynamicVBManager::RegisterVBType()


void        CKTDGDynamicVBManager::DrawPrimitive( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws )
{
    if ( eType < EDYNAMIC_VB_TYPE( 0 ) || eType >= DVB_END || uNumPrimitives == 0 || pData == NULL || uNumDraws == 0 )
        return;
    const DynamicVBInfo& dvbInfo = m_aDynamicVBInfo[ eType ];
    if ( dvbInfo.IsRegistered() == false )
        return;
    _Draw( dvbInfo, d3dPrimitiveType, uNumPrimitives, static_cast<const BYTE*>( pData ), uNumDraws );
}//CKTDGDynamicVBManager::DrawPrimitive()


void        CKTDGDynamicVBManager::DrawPrimitive_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
    unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws )
{
    if ( uNumData1 == 0 )
    {
        DrawPrimitive( eType, d3dPrimitiveType, uNumPrimitives, pData2, uNumDraws );
        return;
    }
    if ( eType < EDYNAMIC_VB_TYPE( 0 ) || eType >= DVB_END || uNumPrimitives == 0 || pData1 == NULL || pData2 == NULL || uNumDraws == 0 )
        return;
    const DynamicVBInfo& dvbInfo = m_aDynamicVBInfo[ eType ];
    if ( dvbInfo.IsRegistered() == false )
        return;

    const unsigned uNumVertices = NumVertices( d3dPrimitiveType, uNumPrimitives );
    if ( uNumVertices == 0 )
        return;
    const unsigned uFirst = std::min( uNumData1, uNumVertices );
    m_vecSplitData.resize( std::size_t( uNumVertices ) * dvbInfo.m_dwVertexStride );
    memcpy( m_vecSplitData.data(), pData1, std::size_t( uFirst ) * dvbInfo.m_dwVertexStride );
    memcpy( m_vecSplitData.data() + std::size_t( uFirst ) * dvbInfo.m_dwVertexStride, pData2,
        std::size_t( uNumVertices - uFirst ) * dvbInfo.m_dwVertexStride );
    _Draw( dvbInfo, d3dPrimitiveType, uNumPrimitives, m_vecSplitData.data(), uNumDraws );
}//CKTDGDynamicVBManager::DrawPrimitive_SplitData()


void        CKTDGDynamicVBManager::_Draw( const DynamicVBInfo& dvbInfo, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const BYTE* pData, unsigned uNumDraws )
{
    if ( g_pKTDXApp == NULL || g_pKTDXApp->GetDevice() == NULL )
        return;
    LPDIRECT3DDEVICE9 pDevice = g_pKTDXApp->GetDevice();

    // as the original: a fan is cut to what one buffer holds, and lists and strips longer than that are drawn in
    // parts of whole primitives (an even count for strips, so the winding stays the same)
    unsigned uNumPrimsPossible = NumPrimitives( d3dPrimitiveType, dvbInfo.m_uNumVertices );
    if ( d3dPrimitiveType == D3DPT_TRIANGLEFAN && uNumPrimitives > uNumPrimsPossible )
        uNumPrimitives = uNumPrimsPossible;
    if ( d3dPrimitiveType == D3DPT_TRIANGLESTRIP && uNumPrimsPossible < uNumPrimitives )
        uNumPrimsPossible = ( uNumPrimsPossible / 2 ) * 2;
    if ( uNumPrimsPossible == 0 )
        return;

    pDevice->SetFVF( dvbInfo.m_dwFVF );
    while ( uNumPrimitives > 0 )
    {
        const unsigned uNumPrims = std::min( uNumPrimitives, uNumPrimsPossible );
        for( unsigned u = 0; u != uNumDraws; u++ )
            pDevice->DrawPrimitiveUP( d3dPrimitiveType, uNumPrims, pData, dvbInfo.m_dwVertexStride );

        uNumPrimitives -= uNumPrims;
        // the next part starts at the first vertex not yet used (strips and line strips share the last ones)
        unsigned uStep = 0;
        switch( d3dPrimitiveType )
        {
        case D3DPT_POINTLIST:
        case D3DPT_LINESTRIP:
        case D3DPT_TRIANGLESTRIP:   uStep = uNumPrims; break;
        case D3DPT_LINELIST:        uStep = uNumPrims * 2; break;
        case D3DPT_TRIANGLELIST:    uStep = uNumPrims * 3; break;
        default:                    return;
        }//switch
        pData += std::size_t( uStep ) * dvbInfo.m_dwVertexStride;
    }//while
}//CKTDGDynamicVBManager::_Draw()
