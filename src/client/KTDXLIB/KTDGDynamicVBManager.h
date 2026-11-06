#pragma once

#define D3DFVF_XYZ_DIFFUSE          ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
#define D3DFVF_XYZ_DIFFUSE_TEX1     ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_XYZ_TEX1             ( D3DFVF_XYZ | D3DFVF_TEX1 )
#define D3DFVF_XYZRHW_DIFFUSE_TEX1  ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


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

private:

    void                CustomInitialize( bool bInConstructor_ );
    virtual void        CustomFinalize( bool bInDestructor_ );

public:

    virtual HRESULT     OnFrameMove( float fElapsedTime );

    virtual HRESULT     OnResetDevice();
    virtual HRESULT     OnLostDevice();

	bool                RegisterVBType( EDYNAMIC_VB_TYPE eType, DWORD dwFVF, unsigned uNumVertices, int iVBNum );
    void                DrawPrimitive( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws = 1 ) throw()
                        {
                            if ( m_pDrawPrimitiveFn != NULL )
                                (this->*m_pDrawPrimitiveFn)( eType, d3dPrimitiveType, uNumPrimitives, pData, uNumDraws );
                        }//CKTDGDynamicVBManager::DrawPrimitive()
    void                DrawPrimitive_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
                            unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws = 1 ) throw()
                        {
                            if ( uNumData1 == 0 )
                            {
                                if ( m_pDrawPrimitiveFn != NULL )
                                    (this->*m_pDrawPrimitiveFn)( eType, d3dPrimitiveType, uNumPrimitives, pData2, uNumDraws );
                            }
                            if ( m_pDrawPrimitiveFn_SplitData != NULL )
                                (this->*m_pDrawPrimitiveFn_SplitData)( eType, d3dPrimitiveType, uNumPrimitives, uNumData1, pData1, pData2, uNumDraws );
                        }//CKTDGDynamicVBManager::DrawPrimitive()

    static              void    GetOSVersion( DWORD& dwOutMajor, DWORD& dwOutMinor );

    bool                SetDynamicVBMode( EDYNAMIC_VB_MODE eDynamicVBMode );
    EDYNAMIC_VB_MODE    GetDynamicVBMode() const { return m_eDynamicVBMode; }

    void                ResetNumExceptionsOccurred() { m_uNumExceptionsOccurred = 0; }


private:

    void                _DrawPrimitiveManaged( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws ) throw() ;
    void                _DrawPrimitiveUP( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveDynamicVB( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveDynamicVBWithTryCatch( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveManaged_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
        unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveUP_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
        unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveDynamicVB_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
        unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws ) throw();
    void                _DrawPrimitiveDynamicVBWithTryCatch_SplitData( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
        unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws ) throw();
private:

    struct  DynamicVBInfo
    {
        unsigned        m_uNumVertices;			/// 정점 개수
        DWORD           m_dwFVF;				/// 정점 정보
        unsigned        m_dwVertexStride;		/// 스트림에 연결하고자하는 버텍스 버퍼 내 각 요소의 바이트 수
		struct KVBInfo
		{
			KVBInfo()
				:m_uCurVertexIndex( 0 )
				, m_pVB( NULL )
			{}

			~KVBInfo()
			{
				SAFE_RELEASE( m_pVB );
			}

			void Reset()
			{
				m_uCurVertexIndex = 0;
				SAFE_RELEASE( m_pVB );
			}

			void OnLostDevice()
			{
				m_uCurVertexIndex = 0;
				SAFE_RELEASE( m_pVB );
			}

			unsigned        m_uCurVertexIndex;		/// 현재 정점 인덱스
			LPDIRECT3DVERTEXBUFFER9 m_pVB;					/// DX 버텍스 버퍼
		};

		int m_iCurVBInfoIndex;
		std::vector< KVBInfo > m_vecVBInfo;

        DynamicVBInfo()
                        : m_uNumVertices( 0 )
                        , m_dwFVF( 0 )
                        , m_dwVertexStride( 0 )
                        {
                        }

        ~DynamicVBInfo();

        bool            IsRegistered() const { return m_uNumVertices != 0; }

        void            Reset()
                        {
                            m_uNumVertices = 0;
                            m_dwFVF = 0;
                            m_dwVertexStride = 0;
							std::vector< KVBInfo >::iterator it;
							std::vector< KVBInfo >::iterator iend = m_vecVBInfo.end();
							for( it = m_vecVBInfo.begin(); it != iend; ++it )
							{
								it->Reset();
							}

							m_iCurVBInfoIndex = 0;
                        }

        void            OnResetDevice( EDYNAMIC_VB_MODE eMode );
        void            OnLostDevice( EDYNAMIC_VB_MODE eMode );

    };//struct  DynamicVBInfo

    std::array<DynamicVBInfo, DVB_END>
                        m_aDynamicVBInfo;

    unsigned            m_uNumMaxPrimitiveCount;
    EDYNAMIC_VB_MODE    m_eDynamicVBMode;
    unsigned            m_uNumExceptionsOccurred;

    void                (CKTDGDynamicVBManager::*m_pDrawPrimitiveFn)( EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives, const void* pData, unsigned uNumDraws ) throw() ;
    void                (CKTDGDynamicVBManager::*m_pDrawPrimitiveFn_SplitData)(
        EDYNAMIC_VB_TYPE eType, D3DPRIMITIVETYPE d3dPrimitiveType, unsigned uNumPrimitives,
        unsigned uNumData1, const void* pData1, const void* pData2, unsigned uNumDraws ) throw() ;
};//class   CKTDGDynamicVBManager
