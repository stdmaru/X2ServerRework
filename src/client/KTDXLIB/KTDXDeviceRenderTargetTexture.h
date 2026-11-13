#pragma once

#define D3DFVF_RENDERTARGET_RHW_VERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
#define D3DFVF_RENDERTARGET_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

class CKTDXDeviceRenderTargetTexture
	: public CKTDXDeviceBaseTexture
{
	private:
		struct RENDERTARGET_RHW_VERTEX
		{
			float x, y, z, rhw;
			DWORD color;
			float u, v;
		};

		struct RENDERTARGET_VERTEX
		{
			D3DXVECTOR3	vPos;
			//DWORD		color;
			D3DXVECTOR2 vTex;
		};

	public:
		CKTDXDeviceRenderTargetTexture( LPDIRECT3DDEVICE9 pd3dDevice, wstring deviceID, int width, int height, D3DFORMAT fmt = D3DFMT_UNKNOWN );


		virtual HRESULT OnResetDevice();
		virtual HRESULT OnLostDevice();

		void Draw( int nX, int nY, int nWidth, int nHeight, D3DCOLOR color = 0xffffffff );
		void RenderQuadRHW( D3DCOLOR color = 0xffffffff, float fX = 0.f, float fY = 0.f, float fWidth = -1.f, float fHeight = -1.f );
		void RenderQuad( D3DCOLOR color = 0xffffffff );

		void BeginRender( bool bClear = true );
		void EndRender();

		void Clear( D3DCOLOR color = 0x00000000 );

		void SetDeviceTexture( int stage = 0 );
		// StretchRect copies between this target, another one and the back buffer: not on the device yet, they fail
		// (docs/porting/client-ktdxlib.md#render-targets).
		HRESULT CopySurface( LPDIRECT3DTEXTURE9 pDestSurface );
		HRESULT CopySurfaceToBackBuffer();
		HRESULT CopyBackBufferToSurface();

		// The device has no surfaces (T38): a render target is its texture.
		LPDIRECT3DTEXTURE9 GetSurface(){ return m_pRenderTargetTexture; }
		LPDIRECT3DTEXTURE9 GetTexture(){ return m_pRenderTargetTexture; }
		int GetWidth(){ return m_Width; }
		int GetHeight(){ return m_Height; }


		bool IsOk()
		{
			if( m_pRenderTargetTexture == NULL )
			{
				return false;
			}
			else
			{
				return true;
			}
		}

    protected:

		virtual ~CKTDXDeviceRenderTargetTexture(void);

	private:



		int						m_Width;
		int						m_Height;

		LPDIRECT3DTEXTURE9		m_pRenderTargetTexture;	// its depth buffer is the device's, sized to the target
		LPDIRECT3DTEXTURE9		m_pBackupTarget;		// the target BeginRender replaced (null: the back buffer)
		D3DFORMAT				m_D3DFormat;
		D3DXVECTOR2				m_vBackupUIOrigin{ 0.f, 0.f };	// the UI origin BeginRender replaced
		float					m_fBackupUIAnchorShift = 0.f;	// and the anchor shift in effect with it

};
