#pragma once

#define SLASH_TRACE_MAX_SPLIT	50

#define D3DFVF_SLASH_TRACE (D3DFVF_XYZ|D3DFVF_DIFFUSE)

	static const DWORD D3DFVF_SLASH_TRACE_TEXTURED = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;


class CKTDGSlashTrace : public CKTDGObject
{
	public:
		enum CURVE_TYPE
		{
			CT_SMOOTH_CURVE,		// 완전히 부드러운 곡선, 그렇지만 실제 칼보다 조금 늦게 따라옴
			CT_TOUGH_CURVE,			// 조금 덜 부드러운 곡선, 그렇지만 실제 칼에 바짝 붙어서 따라옴
			CT_STRAIGHT_LINE,		// 완전 직선형, 칼에 붙어서 따라옴
		};

		enum SLASH_TRACE_TYPE
		{
			STT_DEFAULT,
			STT_CONSTANT_WIDTH,
			STT_CIRCULAR,
//#ifdef ARA_CHARACTER_BASE
			STT_DOUBLE_TIP,
//#endif ARA_CHARACTER_BASE
//#ifdef CUSTOM_SLASH_TRACE_TEXTURE
			STT_CUSTOM_TEXTURE,
//#endif // CUSTOM_SLASH_TRACE_TEXTURE
			STT_NONE,
		};

		struct VERTEX_SLASH_TRACE
		{
			D3DXVECTOR3 position;
			D3DCOLOR    color;
		};

		struct VERTEX_SLASH_TRACE_TEXTURED
		{
			D3DXVECTOR3 position;
			D3DCOLOR    color;
			D3DXVECTOR2 texUV;
		};


	private:
		CKTDGSlashTrace( int vertexNum, bool bTexturedSlashTrace = false, const float fTextureStride = 0.f, const wstring& slashTraceTextureName = L"" );
		virtual ~CKTDGSlashTrace(void);

	public:
		static CKTDGSlashTrace* CreateSlashTrace( const int vertexNum, bool bTexturedSlashTrace = false, const float fTextureStride = 0.f, const wstring& slashTraceTextureName = L"" )
		{
			return new CKTDGSlashTrace(vertexNum, bTexturedSlashTrace, fTextureStride, slashTraceTextureName );
		}


		using CKTDGObject::OnFrameMove;
		virtual HRESULT OnFrameMove( float fElapsedTime );
//{{ robobeg : 2008-10-24
		//virtual HRESULT OnFrameRender();
        virtual RENDER_HINT   OnFrameRender_Prepare();
        virtual void          OnFrameRender_Draw();
//}} robobeg : 2008-10-24

//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//		virtual HRESULT OnResetDevice();
//		virtual HRESULT OnLostDevice();
//#endif

		void SetDisableTime( float fTime ){ m_DisableTime = fTime; }

		// point-to-point slash trace
		void AddSlashData( D3DXVECTOR3 posUp, D3DXVECTOR3 posDown, D3DXCOLOR color, bool bColorGradation = true, const float fTextureStride = 0.f, const wstring& wstrTextureName = L"" );
		// 두께가 일정한 slash trace
		void AddSlashDataConstantWidth( D3DXVECTOR3 posUp, D3DXVECTOR3 posDown, const D3DXCOLOR& color,
			const D3DXVECTOR3& vNormalizedLineDir, bool bColorGradation = false );
		// 중심과 반지름이 있는 원형 slash trace
		void AddSlashDataCircular( D3DXVECTOR3 vCircleCenter, D3DXVECTOR3 posUp, D3DXVECTOR3 posDown, const D3DXCOLOR& color,
			const D3DXVECTOR3& vNormalizedLineDir, const float fThreshold = 50.f, bool bColorGradation = true );

		int		GetDrawCount()
		{
			return m_DrawCount;
		}
		void	SetDrawCount( int drawCount )
		{
			m_DrawCount = drawCount;
		}

		void	SetHighLight( bool bHighLight )
		{
			m_bHighLight = bHighLight;
		}

		CKTDGSlashTrace::CURVE_TYPE GetCurveType() const
		{
			return m_eCurveType;
		}
		void SetCurveType(const CKTDGSlashTrace::CURVE_TYPE val)
		{
			m_eCurveType = val;
		}


		void SetDefaultSlashTraceTextureName(const wstring val)
		{
			m_DefaultSlashTraceTextureName = val;
		}
		bool GetEnabledSlashTraceTexture() const { return m_bEnabledSlashTraceTexture; }



protected:
		void	PushRenderBuffer( D3DXVECTOR3 posUp, D3DXVECTOR3 posDown, D3DXCOLOR color, bool bColorGradation, bool bTexturedSlashTrace = false, float fTextureStride = 0.f );


		CKTDXDeviceTexture* GetSlashTraceTexture( const wstring& wstrTextureName );






private:
		int						m_VertexNum;
//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//		LPDIRECT3DVERTEXBUFFER9	m_pSlashVB;
//#endif
		VERTEX_SLASH_TRACE*		m_pSlashVertexList;
        int                     m_iSlashVertexList_StartIndex;
		bool					m_bHasVisibleVertex;

		bool							m_bEnabledSlashTraceTexture;
//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//		LPDIRECT3DVERTEXBUFFER9			m_pSlashTexturedVB;
//#endif
		VERTEX_SLASH_TRACE_TEXTURED*	m_pSlashVertexTexturedList;
        int                             m_iSlashVertexTexturedList_StartIndex;
		bool							m_bHasVisibleVertexTextured;

		wstring							m_DefaultSlashTraceTextureName;
		typedef std::map<wstring, CKTDXDeviceTexture*> SlashTraceTextureMap;
		SlashTraceTextureMap			m_mapSlashTraceTexture;
		CKTDXDeviceTexture*				m_pCurrSlashTraceTexture;
		float							m_fTextureStride;



		float					m_DisableTime;
		int						m_DrawCount;
		bool					m_bHighLight;

		D3DXVECTOR3				m_SplineBufUp1;
		D3DXVECTOR3				m_SplineBufUp2;
		D3DXVECTOR3				m_SplineBufUp3;
		D3DXVECTOR3				m_SplineBufUp4;

		D3DXVECTOR3				m_SplineBufDown1;
		D3DXVECTOR3				m_SplineBufDown2;
		D3DXVECTOR3				m_SplineBufDown3;
		D3DXVECTOR3				m_SplineBufDown4;

//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//		bool					m_bUseDynamic;
//#endif


		D3DXVECTOR3				m_vOldCenter;
		float					m_fOldCircleRadius;



		float m_TexturedSlashTraceOpaqueAlphaThreshold;
		float m_TexturedSlashTraceInvisibleAlphaThreshold;


		CURVE_TYPE m_eCurveType;

};
