#pragma once

// Disconnection reasons of a session, kept by KSocketObject (SetDisconnectReason) and reported in the statistics
// (KStatistics derives from this, so KStatistics::eSIColDR_* still names them). Moved out of Statistics.h so the
// network layer (Socket/), which the client shares, does not include every packet header (decision T34).
struct KDisconnectReason
{
	enum ESIColumn_Disconnect_Reason
	{
		eSIColDR_No_Reason							=  0,	// no reason
		eSIColDR_Unknown							=  1,	// unknown
		eSIColDR_HackShield_Auth_Fail				=  2,	// HackShield authentication failed (no client reply)
		eSIColDR_PCBang_Auth_Delay					=  3,	// PC cafe authentication delayed
		eSIColDR_Diff_Main_Ver						=  4,	// different main version
		eSIColDR_Limit_User_Num						=  5,	// user limit exceeded
		eSIColDR_Nexon_Auth_Fail					=  6,	// Nexon authentication failed
		eSIColDR_Same_Server_Connect				=  7,	// double login on the same server
		eSIColDR_Account_Block						=  8,	// account blocked
		eSIColDR_IP_Block							=  9,	// IP restricted
		eSIColDR_NProtect_Auth_First				= 10,	// first nProtect authentication
		eSIColDR_PCBang_Auth_Fail					= 11,	// PC cafe authentication failed
		eSIColDR_PCBang_Auth_Wrong					= 12,	// wrong PC cafe authentication result
		eSIColDR_Internal_Account					= 13,	// in-house account created
		eSIColDR_Normal_Disconnect					= 14,	// normal logout
		eSIColDR_Client_Crash						= 15,	// client crash
		eSIColDR_Client_Hacking						= 16,	// client hacking
		eSIColDR_HackShield_Wrong_Ack				= 17,	// HackShield authentication failed (wrong reply)
		eSIColDR_PCBang_Auth_Server					= 18,	// shutdown requested by the PC cafe authentication server
		eSIColDR_Server_Shutdown					= 19,	// server shutdown
		eSIColDR_Session_Close						= 20,	// connection closed (Session)
		eSIColDR_WSAECONNRESET						= 21,	// WSAECONNRESET (socket object)
		eSIColDR_WSARecvFailed						= 22,	// WSARecv failed (socket object)
		eSIColDR_WSASendFailed_OnSendComp			= 23,	// WSASend failed (socket object), OnSendCompleted()
		eSIColDR_SendBufferFull						= 24,	// send buffer full (socket object)
		eSIColDR_WSASendFailed						= 25,	// WSASend failed (socket object), SendData()
		eSIColDR_GM_Kick_User						= 26,	// kicked by an operator
		eSIColDR_Diff_Server_Connect				= 27,	// double login across servers
		eSIColDR_User_Reg_Error						= 28,	// error while registering the user
		eSIColDR_Heart_Beat							= 29,	// heartbeat
		eSIColDR_IOCP_GetStatus						= 30,	// IOCP GetStatus: closed by the local machine
		eSIColDR_nProtect_Hacking					= 31,	// nProtect detected hacking
		eSIColDR_Channel_Change						= 32,	// disconnected to change channel
		eSIColDR_Channel_Change_Fail				= 33,	// disconnected after a failed channel change
		eSIColDR_Traffic_Attack						= 34,	// disconnected for causing traffic (2009-10-22)
		eSIColDR_GameServer_Account_Block			= 35,	// account blocked by the game server (2010-06-22)
		eSIColDR_MachineID_Block					= 36,	// machine ID blocked (2011-08-09)
		eSIColDR_Same_MachineID_Connect				= 37,	// double login from one machine ID (2011-08-17)
		eSIColDR_Policy_Shutdown_Disconnect			= 38,	// login refused by the shutdown policy (2011-09-29)
		eSlColDR_Wrong_Account_or_Password			= 39,	// wrong account or password (TW)
		eSlColDR_Wrong_SHA_Checksum					= 40,	// wrong SHA checksum (TW, 2011-02-16)

		eSIColDR_Account_Invalid_Selling_Kill		= 41,	// kicked for selling an unsellable item (TW)
		eSIColDR_Clear_Time_Hack_User				= 42,	// clear time under 5 seconds, suspected hack (TW)

		// HProtect (2011-03-20)
		eSIColDR_Error_Check_Alive1					= 43,	// A type heartbeat validation error
		eSIColDR_Error_Check_Alive2					= 44,	// B type heartbeat validation error
		eSIColDR_Error_Wait_Check_Time_Out			= 45,	// the client did not respond to a packet for too long
		eSIColDR_Error_Client_Speed					= 46,	// abnormal client speed
		eSIColDR_Error_Client_Scan_Result			= 47,	// wParam points to TOOLSSCANRESULT
		eSIColDR_Error_Client_Version				= 48,	// wParam: smallest allowed version, lParam: client version
		eSIColDR_Error_Server_Lost_File				= 49,	// wParam points to the file path
		eSIColDR_Error_Client_Protocol				= 50,	// data packets failed the check
		eSIColDR_Error_Client_Sign_Hash				= 51,	// error in the client signature file
		eSIColDR_Error_Wait_Client_Version_Time_Out = 52,	// no client version within 2 minutes; protection not initialized
		eSIColDR_Check_ServerSN_In_GameServer		= 53,	// kicked by the game server's ServerSN check (2012-01-16)
		eSIColDR_RecvBufferFull						= 54,	// received packet longer than the receive buffer (2012-02-01)
		eSIColDR_PeaceOfMindLogin					= 55,	// blocked by Nexon's location-based login (2012-02-14)
		eSIColDR_ImpossibleSellItem					= 56,	// tried to sell an unsellable item (2012-05-23)
		eSIColDR_nProtect_Error                     = 57,	// nProtect error
		eSIColDR_MovDataQue2Buf_Failed              = 58,	// abnormal buffer size in MovDataQue2Buf
		eSIColDR_LanBugOutDisconnect				= 59,	// ignored the LAN-lag check and was disconnected (2013-05-06)
		eSIColDR_FieldWorkingCheck					= 60,	// field farming check (2013-05-06)
		eSIColDR_InvlideCheckKomScript				= 61,	// CheckKom.xml parse error (2013-09-04)
		eSIColDR_InvaildKomDetect					= 62,	// tampered .kom detected (2013-11-06)

		eSIColDR_Num,
	};
};
