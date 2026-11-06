#pragma once

#define D3DFVF_XYZ_DIFFUSE          ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
#define D3DFVF_XYZ_DIFFUSE_TEX1     ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_XYZ_TEX1             ( D3DFVF_XYZ | D3DFVF_TEX1 )
#define D3DFVF_XYZRHW_DIFFUSE_TEX1  ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


// The engine's dynamic vertex data. The original chose, by Windows version, between a managed vertex buffer, a ring
// of dynamic vertex buffers (with or without a try/catch around the lock) and DrawPrimitiveUP; the device's
// DrawPrimitiveUP is itself a ring buffer uploaded per frame (T38), so every mode is DrawPrimitiveUP now.
class   CKTDGDynamicVBManager : public CKTDXStage
{
public:

    enum    EDYNAMIC_VB_MODE
    {
                        DYNAMIC_VB_MODE_MANAGED,
                        DYNAMIC_VB_MODE_PRIMITIVEUP,
                        DYNAMIC_VB_MODE_DYNAMICVB,
                        DYNAMIC_VB_MODE_DYNAMICVB_WITH_TRY_CATCH,
                        DYNAMIC_VB_MODE_NUMS
    };//enum    EDynamicVBOptMode

    enum    EDYNAMIC_VB_TYPE
    {
                        DVB_TYPE_XYZ_DIFFUSE,
                        DVB_TYPE_XYZ_DIFFUSE_TEX1,
                        DVB_TYPE_XYZ_TEX1,
                        DVB_TYPE_XYZRHW_DIFFUSE_TEX1,
                        DVB_END
    };//enum EDynamicVBType

public:

    CKTDGDynamicVBManager();
    virtual ~CKTDGDynamicVBManager();

    // uNumVertices: the most vertices one draw takes; longer lists and strips are drawn in parts, as the original
    // did when they did not fit its vertex buffer. iVBNum (the original's ring of buffers) is not used.
	bool                RegisterVBType( EDYNAMIC_VB_TYPE eType, DWORD dwFVF, unsigned uNumVertices, int iVBNum );
    // uNumDraws: the same vertices drawn that many times (multipass).
    void                DrawPrimitive( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws = 1 );
    // The first uNumData1 vertices come from pData1, the rest from pData2.
    void                DrawPrimitive_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
                            unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws = 1 );

    EDYNAMIC_VB_MODE    GetDynamicVBMode() const { return DYNAMIC_VB_MODE_PRIMITIVEUP; }

private:

    struct  DynamicVBInfo
    {
        unsigned        m_uNumVertices = 0;     // the most vertices of one draw
        DWORD           m_dwFVF = 0;
        unsigned        m_dwVertexStride = 0;

        bool            IsRegistered() const { return m_uNumVertices != 0; }
    };//struct  DynamicVBInfo

    void                _Draw( const DynamicVBInfo& dvbInfo, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const BYTE* pData, unsigned uNumDraws );

    std::array<DynamicVBInfo, DVB_END>
                        m_aDynamicVBInfo;
    std::vector<BYTE>   m_vecSplitData;         // DrawPrimitive_SplitData's vertices, joined
};//class   CKTDGDynamicVBManager
