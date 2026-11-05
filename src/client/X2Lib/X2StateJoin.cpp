#include "stdafx.h"


#include "./X2StateJoin.h"


CX2StateJoin::CX2StateJoin(void)
{
	m_pDLGJoinBack		= new CKTDGUIDialog( this, L"DLG_Join_Back.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGJoinBack );

	m_pDLGJoinFront		= new CKTDGUIDialog( this, L"DLG_Join_Front.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGJoinFront );

}

CX2StateJoin::~CX2StateJoin(void)
{
	SAFE_DELETE_DIALOG( m_pDLGJoinBack );
	SAFE_DELETE_DIALOG( m_pDLGJoinFront );

	g_pData->DeleteServerProtocol();
}

bool CX2StateJoin::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{
		case SJUCM_MEMBER_JOIN:
			{
				return ServerConnectReq( hWnd, uMsg, wParam, lParam );
			}
			break;

		case SJUCM_BACK_TO_LOGIN:
			{
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_LOGIN, NULL, false );
				return true;
			}
			break;
	}

	return false;
}

bool CX2StateJoin::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
		case EGS_CONNECT_ACK:
			{
				return ServerConnectAck( hWnd, uMsg, wParam, lParam );
			}
			break;

		case EGS_NEW_USER_JOIN_ACK:
			{
				return NewUserJoinAck( hWnd, uMsg, wParam, lParam );
			}
			break;
	}
	return false;
}
/*

bool CX2StateJoin::UIServerTimeOutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
		case EGS_CONNECT_ACK:
			{
				return ServerConnectTimeOutAck( hWnd, uMsg, wParam, lParam );
			}
			break;

		case EGS_NEW_USER_JOIN_ACK:
			{
				return NewUserJoinTimeOutAck( hWnd, uMsg, wParam, lParam );
			}
			break;
	}
	return false;
}
*/



bool CX2StateJoin::ServerConnectReq( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CKTDGUIIMEEditBox*	pIDBox			= NULL;
	CKTDGUIEditBox*		pPasswordBox	= NULL;
	CKTDGUIIMEEditBox*	pNameBox		= NULL;

	CKTDGUIIMEEditBox*	pNickNameBox	= NULL;
	pIDBox			= (CKTDGUIIMEEditBox*)m_pDLGJoinBack->GetControl( L"IMEEditBoxJoinID" );
	pPasswordBox	= (CKTDGUIEditBox*)m_pDLGJoinBack->GetControl( L"EditBoxJoinPassword" );
	pNameBox		= (CKTDGUIIMEEditBox*)m_pDLGJoinBack->GetControl( L"IMEEditBoxJoinName" );
	pNickNameBox	= (CKTDGUIIMEEditBox*)m_pDLGJoinBack->GetControl( L"IMEEditBoxJoinNickName" );

	m_UserID			= pIDBox->GetText();
	m_UserPassword		= pPasswordBox->GetText();
	m_UserName			= pNameBox->GetText();
	//m_UserNickName		= pNickNameBox->GetText();

	if( JoinErrorCheck() == true )
	{
		//접속시도
		g_pData->DeleteServerProtocol();
		g_pData->ResetServerProtocol();

//#ifdef X2OPTIMIZE_TCP_RELAY_TEST
//		g_pData->GetGameUDP()->SetServerProtocol( g_pData->GetServerProtocol() );
//#endif//X2OPTIMIZE_TCP_RELAY_TEST

// 09.07.03 태완 : 아이디 생성 사내에서는 가능하게 수정
#if X2_DEV_TOOLS
// 		KEVENT_X2_SERVER_CONNECT_REQ kEVENT_X2_SERVER_CONNECT_REQ;
// 		kEVENT_X2_SERVER_CONNECT_REQ.gameMainVersion	= 0;
// 		kEVENT_X2_SERVER_CONNECT_REQ.gameSubVersion		= 0;
// 		kEVENT_X2_SERVER_CONNECT_REQ.gamePatchVersion	= 0;

		//{{ dmlee 2009.5.13 fix!! 채널서버 접속 방식으로 바뀌면서 임시로 막아놓음, 나중에 수정해야함
		if( g_pData->GetServerProtocol()->ConnectedToGameServer( g_pMain->GetGameServerIPForCreateID().c_str(), g_pMain->GetGameServerPortForCreateID() ) == true )
		{
			KEGS_CONNECT_REQ kPacket;
			kPacket.m_wstrVersion = g_pMain->GetClientVersion();

			g_pData->GetServerProtocol()->SendPacket( EGS_CONNECT_REQ, kPacket );
			g_pMain->AddServerPacket( EGS_CONNECT_ACK );

			m_pDLGMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_647 ), this );
		}
		else
		{
			m_pDLGMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_648 ), this );
		}
		//}} dmlee 2009.5.13 fix!! 채널서버 접속 방식으로 바뀌면서 임시로 막아놓음, 나중에 수정해야함
#endif // !_SERVICE_ || SERV_JOIN_IN_CLIENT_FOR_TW_TEST_SERVER

	}
	return true;
}

bool CX2StateJoin::ServerConnectAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KPacketOK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_CONNECT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//g_pData->GetServerProtocol()->SetConnectAccept();
			NewUserJoinReq( NULL,NULL,NULL,NULL );
		}
		else
		{


			g_pData->DeleteServerProtocol();
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_648 ), this );
		}
	}
	return true;
}
/*
bool CX2StateJoin::ServerConnectTimeOutAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	g_pData->DeleteServerProtocol();

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_648 ), this );
	return true;
}
*/

bool CX2StateJoin::NewUserJoinReq( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KEGS_NEW_USER_JOIN_REQ	kEGS_NEW_USER_JOIN_REQ;
	kEGS_NEW_USER_JOIN_REQ.m_wstrID			= m_UserID;
	kEGS_NEW_USER_JOIN_REQ.m_wstrPassword	= m_UserPassword;
	kEGS_NEW_USER_JOIN_REQ.m_wstrName		= m_UserName;
//	kEGS_NEW_USER_JOIN_REQ.m_wstrNickName	= m_UserNickName;

	g_pData->GetServerProtocol()->SendPacket( EGS_NEW_USER_JOIN_REQ, kEGS_NEW_USER_JOIN_REQ );
	g_pMain->AddServerPacket( EGS_NEW_USER_JOIN_ACK );
	m_pDLGMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_649 ), this );
	return true;
}

bool CX2StateJoin::NewUserJoinAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_NEW_USER_JOIN_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_NEW_USER_JOIN_ACK ) == true )
	{
		if(  g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_650 ), this );

		}

	}
//	g_pData->DeleteServerProtocol();
	return true;
}

//bool CX2StateJoin::NewUserJoinTimeOutAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"서버에 응답이 없습니다", this );
//	g_pData->DeleteServerProtocol();
//	return true;
//}











bool CX2StateJoin::JoinErrorCheck()
{
	string multyByteUserID;
	ConvertWCHARToChar( multyByteUserID, m_UserID.c_str() );

	string multyByteUserPassword;
	ConvertWCHARToChar( multyByteUserPassword, m_UserPassword.c_str() );

	string multyByteUserName;
	ConvertWCHARToChar( multyByteUserName, m_UserName.c_str() );

	//string multyByteUserNickName;
	//ConvertWCHARToChar( multyByteUserNickName, m_UserNickName.c_str() );

	if( multyByteUserID.size() < 4 || multyByteUserID.size() > 20 )
	{
		//ID를 4자 미만으로 입력한 경우
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_651 ), this );
	}
	else if( multyByteUserPassword.size() < 4 || multyByteUserPassword.size() > 20 )
	{
		//Password를 4자 미만으로 입력한 경우
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_652 ), this );
	}
	else if( multyByteUserName.size() < 1 || multyByteUserName.size() > 16 )
	{
		//Password를 4자 미만으로 입력한 경우
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_653 ), this );
	}
	//else if( multyByteUserNickName.size() < 1 || multyByteUserNickName.size() > 16 )
	//{
	//	//Password를 4자 미만으로 입력한 경우
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_654 ), this );
	//}
	else
	{
		return true;
	}
	return false;
}
