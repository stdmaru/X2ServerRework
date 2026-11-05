#pragma once


//{{ seojt // 2009-1-14, 23:16
class CX2WorldObject;
typedef boost::intrusive_ptr<CX2WorldObject>   CX2WorldObjectPtr;
//}} seojt // 2009-1-14, 23:16


class CX2WorldObject : public CKTDGObject
{
	public:
		enum OBJECT_TYPE
		{
			OT_INVALID,
			OT_STATIC_MESH,
			OT_SKIN_MESH,
			OT_PARTICLE,
			OT_LIGHT_FLOW,
			OT_REACT_MESH,
			OT_END,
		};

	protected:
		CX2WorldObject( bool bBackgroundLoad_ = false );
		virtual ~CX2WorldObject(void);



	public:
		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );

		bool			IsBackgroundLoad() const { return m_bBackgroundLoad; }
//{{ robobeg : 2008-10-28
		//virtual HRESULT OnFrameRender(){ return S_OK; }
        virtual void    OnFrameRender_Draw() {}
//}} robobeg : 2008-10-28

		virtual HRESULT OnResetDevice(){ return S_OK; }
		virtual HRESULT OnLostDevice(){ return S_OK; }
		virtual void	SetLOD( bool bLOD ){ m_bLOD = bLOD; }
		virtual bool	RayHit( D3DXVECTOR3 rayStart, D3DXVECTOR3 rayEnd, float* pHitDistance = NULL ){ return false; }

		virtual bool	RayHit( D3DXVECTOR3 rayStart, D3DXVECTOR3 rayEnd, float* pHitDistance, const D3DXMATRIX& transform ) { return false; }


		OBJECT_TYPE		GetObjectType() const { return m_ObjectType; }
		const WCHAR*	GetName() const { return m_wstrObjectName.c_str(); }
		void			SetLightPos( D3DXVECTOR3 lightPos ){ m_LightPos = lightPos; }
		void			SetScale( D3DXVECTOR3 vScale );
		bool 			GetUseWorldColor() const { return m_bUseWorldColor; }
		void 			SetUseWorldColor(bool val) { m_bUseWorldColor = val; }
		void			SetHideDistance(float fDist) { m_fHideDistance = fDist; }
		void			SetHideNearDistance(float fDist) { m_fHideNearDistance = fDist; }
		void			SetForceShow(bool val) { m_bForceShow = val; }
		bool			GetForceShow() { return m_bForceShow; }
//{{ robobeg : 2008-10-17
        D3DXVECTOR3     GetPos() { return GetMatrix().GetPos(); }
//}} robobeg : 2008-10-17
		//{{ 09.05.12 김태완
		void		SetOriginalColorFactor( D3DXCOLOR coColor ){ m_coOriginalColor = coColor; }
		D3DXCOLOR	GetOriginalColorFactor(){ return m_coOriginalColor; }
		void		SetColorFactor( D3DXCOLOR coColor )
		{
			m_coColor.r = coColor.r * m_coOriginalColor.r;
			m_coColor.g = coColor.g * m_coOriginalColor.g;
			m_coColor.b = coColor.b * m_coOriginalColor.b;
			m_coColor.a = coColor.a * m_coOriginalColor.a;
		}
		//}}
//{{ 09.03.02 김태완
//}}


		//LUA
		void SetName( const char* pName );
		void SetPos( D3DXVECTOR3 pos );
		void SetAngleDegree( D3DXVECTOR3 angleDegree );


	protected:
		bool							m_bBackgroundLoad;
		CX2WorldObject::OBJECT_TYPE		m_ObjectType;
		bool							m_bLOD;
		wstring							m_wstrObjectName;
//{{ robobeg : 2008-10-17
		//D3DXVECTOR3						m_Pos;
		//D3DXVECTOR3						m_AngleDegree;
		//D3DXVECTOR3						m_vScale;
//}} robobeg : 2008-10-17
		D3DXVECTOR3						m_LightPos;

		float							m_fElapsedTime;
		D3DXCOLOR						m_coColor;

// 09.05.12 태완
		D3DXCOLOR						m_coOriginalColor;

		bool							m_bUseWorldColor;
		float							m_fHideDistance;
		float							m_fHideNearDistance;

		bool							m_bForceShow;		// 옵션설정에 상관없이 무조건 출력


};
