#pragma once

class CX2StateLogin : public CX2State
{
	public:
		enum STATE_LOGIN_UI_CUSTOM_MSG
		{
			SLUCM_LOGIN = 0,
			SLUCM_CHANGE_STATE_MEMBER_JOIN,

			SLUCM_CONNECT_SERVER,				// fix!! 사용하는 곳이 없네요

			SLUCM_AUTO_LOGIN,
			SLUCM_NOT_AUTO_LOGIN,

			SLUCM_NEXON_KOREA_LOGIN,
			SLUCM_NEXON_KOREA_LOGIN_COMPLETE,
//#ifdef _HOMEPAGE_MOVE_
			SLUCM_HOMEPAGE_MOVE,
			SLUCM_REGISTER_MOVE,
			SLUCM_FACEBOOK_MOVE,
			SLUCM_STEAM_COMMUNITY_MOVE,
//#endif _HOMEPAGE_MOVE_
//#ifdef RULE_AGREEMENT
			SLUCM_RULE_AGREEMENT,
//#ifdef NEW_RULE_AGREEMENT
			SLUCM_NEW_RULE_AGREEMENT,
//#endif NEW_RULE_AGREEMENT
//#endif RULE_AGREEMENT
//#ifdef SAVE_LOGIN_ID
			SLUCM_REMEMBER_ID_CHECK_ON,
			SLUCM_REMEMBER_ID_CHECK_OFF,
//#endif SAVE_LOGIN_ID
//#ifdef SERV_PUBLISHER_MATRIXKEY
			SLUCM_MATRIX_NUMBER_MOUSE_OVER,
			SLUCM_MATRIX_NUMBER_MOUSE_OUT,
			SLUCM_MATRIX_NUMBER_MOUSE_UP,
			SLUCM_MATRIX_OK,
			SLUCM_MATRIX_CLEAR,
//#endif SERV_PUBLISHER_MATRIXKEY
//#ifdef SERV_PUBLISHER_OTP
			SLUCM_OTP_ON,
			SLUCM_OTP_OFF,
//#endif SERV_PUBLISHER_OTP
//#ifdef LOGIN_KEYBOARD_SECURITY
			SLUCM_KEYBOARD,
			SLUCM_KEYBOARD_KEY,
			SLUCM_KEYBOARD_ENTER,
			SLUCM_KEYBOARD_SHIFT,
			SLUCM_KEYBOARD_CTRL,
			SLUCM_KEYBOARD_BACK,
			SLUCM_KEYBOARD_EDIT_FOCUSOUT,
			SLUCM_KEYBOARD_CLOSE,
//#endif LOGIN_KEYBOARD_SECURITY
//#ifdef USE_ACCOUNT_DOMAIN
			SLUCM_DOMAIN_TYPE_PLAYID		=	30,
			SLUCM_DOMAIN_TYPE_TCG			=	31,
//#endif USE_ACCOUNT_DOMAIN
		};

	public:
		CX2StateLogin(void);
		virtual ~CX2StateLogin(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual HRESULT OnFrameRender();


	protected:
		bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void RetrieveUserIDnPassword();
		bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		void StateChangeLoading();
		void InitManualLogin();

//{{ kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능
#if X2_DEV_TOOLS
		void ReadIDAndPassword();
#endif // AUTO_LOGIN_IN_HOUSE
//}} kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능

		CKTDGUIDialogType	m_pDLGLoginBack;
		CKTDGUIDialogType	m_pDLGLoginFront;

		bool OpenScriptServerGroupFile();
		bool TryVerifyAccount();
		bool ConnectToChannelServer();
		bool Handler_ECH_VERIFY_ACCOUNT_REQ();
		bool Handler_ECH_VERIFY_ACCOUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_ECH_SECURITY_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_ECH_SECURITY_AUTH_REQ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


//{{ 2013.03.13 조효진	회원가입 웹페이지를 버튼을 누르면 띄워줌
//}}

};
