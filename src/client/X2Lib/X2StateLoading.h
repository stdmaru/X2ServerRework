#pragma once




class CX2StateLoading : public CX2State
{
	public:
		CX2StateLoading(void);
		virtual ~CX2StateLoading(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual HRESULT OnFrameRender();


		void SetLoadingGageBar( int iLoadingPercent );
		void SetLoadingString( const WCHAR* msg );
		void DropItemLoading();

	protected:
		bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );




//#ifdef	ADD_SERVER_GROUP
//		bool OpenScriptServerGroupFile();
//#endif
		bool ConnectToChannelServer();
		bool Handler_ECH_VERIFY_ACCOUNT_REQ();
		bool Handler_ECH_VERIFY_ACCOUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_ECH_DISCONNECT_REQ();
		bool Handler_ECH_DISCONNECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		//bool Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
		//bool Handler_EGS_STATE_CHANGE_SERVER_SELECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	private:
		CKTDGUIDialogType								m_pDLGLoadingFront;
		float											m_fLogoTime;

		//# KOG & Nexon 합병로고
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hCompanyLogo;
		//# 위의 로고 배경
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hLogoBack;


		//{{AFX
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hNexonLogo;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hKOGBack;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hKOGLogo;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hKOGLogoMini;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hNexonLogoMini;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hPPoruSurprise;
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hPPoruLogo;

		//CKTDGXMeshPlayer::CXMeshInstance*				m_pWillium;
		//CKTDGXMeshPlayer::CXMeshInstance*				m_pElSword;
		//CKTDGXMeshPlayer::CXMeshInstance*				m_pLena;
		//CKTDGXMeshPlayer::CXMeshInstance*				m_pAisha;
		//CKTDGXMeshPlayer::CXMeshInstance*				m_pBenders;

		//CKTDXDeviceTexture*								m_pTextureTestMark;
		//}}AFX





};
