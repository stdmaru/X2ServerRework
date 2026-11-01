#include "ActorManager.h"
#include "DEBAuth.h"

	#include "DEBAuthManager.h"
	#include "NetError.h"

	#define CLASS_TYPE KDEBAuth

	_ImplementException( KDEBAuth );

	KDEBAuth::KDEBAuth(std::string strWsdlPath)
		: m_kSoapClient( std::move( strWsdlPath ) )
	{
	}

	KDEBAuth::~KDEBAuth()
	{
		// Timeout
		End( 1500 );
	}

	// 2010.10.24 Jo Hyo-jin.
	// Since the current GameForgeAuth manager is registered in m_vecpThreadMgr of KChannelSimLayer
	// Due to its structure, Lua's AddWebMethod is performed faster than KGameForgeAuth::Run().
	void KDEBAuth::Run()     // derived from KThread
	{
		std::cout << "KSubThread::Run(), thread start." << std::endl;
		START_LOG( clog2, L"[DEB] KDEBAuth Started" ) << END_LOG;

		srand( (unsigned) time( NULL ) );

		testfunction();

		while( true )
		{
			DWORD ret = ::WaitForSingleObject( m_hKillEvent, 1 );

			if( ret == WAIT_OBJECT_0 ) break;

			if( ret == WAIT_TIMEOUT ) Loop();

			else std::cout << "*** WaitForSingleObject() - return :" << ret << std::endl;
		}
	}

	//=========================================================================//
	//			Process Event handle
	//=========================================================================//
	void KDEBAuth::ProcessEvent( const KEventPtr& spEvent_ )
	{
		DWORD dwElapTime = ::GetTickCount();

		switch( spEvent_->m_usEventID )
		{
			CASE( EPUBLISHER_AUTHENTICATION_REQ );
			CASE( EPUBLISHER_SECURITY_AUTH_REQ );

			default: {
				START_LOG( cerr, "[DEB] Event for which no valid handler was specified." )
					<< BUILD_LOG( spEvent_->m_usEventID )
					<< BUILD_LOG( spEvent_->GetIDStr() )
					<< END_LOG;
			}
		}

		dwElapTime = ::GetTickCount() - dwElapTime;
		if( dwElapTime > 3000 )
		{
			START_LOG( cwarn, L"[DEB] Event processing time was too long." )
				<< BUILD_LOG( spEvent_->GetIDStr() )
				<< BUILD_LOG( spEvent_->m_usEventID )
				<< BUILD_LOG( dwElapTime );
		}

		return;
	}

	void KDEBAuth::testfunction()
	{
		// IMPLEMENT_ME_IF_NEEDED //
		std::string username = "test";
		std::string password = "test";
		std::string ipAddress = "192.168.71.45";
		int iResult = 0;

		if( iResult == 0 )	// Passed
		{
			START_LOG( clog2, L"[DEB] Authentication protocol is working properly!!" )
				<< BUILD_LOG( iResult )
				<< END_LOG;
			return;
		}
		else	// Stub
		{
			START_LOG( cerr, L"[DEB] Authentication system has an INITIAL TEST ERROR !" )
				<< END_LOG;
			return;
		}
	}

	//////////////////////////////////////////////////////
	IMPL_ON_FUNC( EPUBLISHER_AUTHENTICATION_REQ )
	{
		// kPacket_m_wstrIP, kPacket_m_wstrServiceAccountID, kPacket_m_wstrServicePassword : input
		// ------------ Packets you should/could fill before sending [ (*) is mandatory ] ------------
		//  (*) [int]		kPacketAck.m_iOK				Operation result code [NetError]
		//	(*) [wstring]	kPacketAck.m_wstrID				Username/Email/Whatever you used as username to login.
		//  (*) [uint]		kPacketAck.m_uiPublisherUID		Unique ID linked to the account issued by the publisher
		//  (*) [int]		kPacketAck.m_iChannelingCode	Integer to identify user region, if you like
		//  	[string]	kPacketAck.m_strAgreementURL	Related to terms of service (???)
		//  	[wstring]	kPacketAck.wstrPurchTok			Purchase token (???)
		//  	[uint]		kPacketAck.m_uiAccountType		Used when the anti-addiction system is used (#ifdef SERV_ANTI_ADDICTION_SYSTEM)
		// NetErrors:
		// 		NetError::ERR_VERIFY_21				A system error has occurred in the process of authorization.
		// 		NetError::ERR_GAMEFORGE_00			Passwords do not match.
		// 		NetError::ERR_GAMEFORGE_01			No such account exists.
		// 		NetError::ERR_GAMEFORGE_02			The account has not been validated.
		//		NetError::ERR_ACCOUNT_BLOCK_04		Your account has been blocked(LE4).Please visit our website and contact Customer Support.
		// 		NetError::ERR_VERIFY_11				The account has been blocked.
		// 		NetError::ERR_K3RCOMBO_AUTH_01		We're sorry but Elsword Online is not available in your area. Service is restricted to North America and Oceania.
		// 		NetError::ERR_K3RCOMBO_AUTH_02		OUR TERMS OF USE HAVE CHANGED\nClick 'Accept' and re - login to the game.
		//

		KEPUBLISHER_AUTHENTICATION_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_VERIFY_21;

		//{{ 2010. 11. 25. Jo Hyo-jin If nothing is entered, an error is processed.
		if( kPacket_.m_wstrServiceAccountID == L"" )
		{
			kPacketAck.m_iOK = NetError::ERR_GAMEFORGE_01;
			KEventPtr spEvent( new KEvent );
			spEvent->SetData( PI_CHANNEL_USER, NULL, EPUBLISHER_AUTHENTICATION_ACK, kPacketAck );
			KActorManager::GetKObj()->QueueingTo( kPacket_.m_iActorUID, spEvent );
			return;
		}
		else if ( kPacket_.m_wstrServicePassword == L"" )
		{
			kPacketAck.m_iOK = NetError::ERR_BUY_CASH_ITEM_25;
			KEventPtr spEvent( new KEvent );
			spEvent->SetData( PI_CHANNEL_USER, NULL, EPUBLISHER_AUTHENTICATION_ACK, kPacketAck );
			KActorManager::GetKObj()->QueueingTo( kPacket_.m_iActorUID, spEvent );
			return;

		}

		const x2::deb::SoapFields request = {
			{ "strUsername", KncUtil::toNarrowString( kPacket_.m_wstrServiceAccountID ) },
			{ "strPassword", KncUtil::toNarrowString( kPacket_.m_wstrServicePassword ) },
			{ "strIPAddress", KncUtil::toNarrowString( kPacket_.m_wstrIP ) },
		};
		std::string strError;
		const auto response = m_kSoapClient.Call( "LoginRequest", request, strError );

		if( response ) {
			kPacketAck.m_iOK = static_cast<int>( response->Int( "iResult" ) );

			if( kPacketAck.m_iOK == NetError::NET_OK ) { // Success
				if( !response->Text( "strID" ).empty() )
					kPacketAck.m_wstrID = KncUtil::toWideString( response->Text( "strID" ) );

				kPacketAck.m_uiPublisherUID = static_cast<unsigned int>( response->Int( "iPublisherSN" ) );
				kPacketAck.m_iChannelingCode = static_cast<int>( response->Int( "iChannelingCode" ) );

				// Port: the original tested empty() here, so the token was only copied when there was none.
				if( !response->Text( "strPurchaseToken" ).empty() )
					kPacketAck.wstrPurchTok = KncUtil::toWideString( response->Text( "strPurchaseToken" ) );
			}
		} else {
			START_LOG( cerr, L"[DEB] Fatal error AuthServer request." )
				<< BUILD_LOG( m_kSoapClient.Endpoint() )
				<< BUILD_LOG( strError )
				<< END_LOG;
		}

		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_CHANNEL_USER, NULL, EPUBLISHER_AUTHENTICATION_ACK, kPacketAck );
		KActorManager::GetKObj()->QueueingTo( kPacket_.m_iActorUID, spEvent );

		START_LOG( clog, L"[DEB] Authentication result" )
			<< BUILD_LOG( kPacket_.m_iActorUID )
			<< BUILD_LOG( kPacket_.m_wstrServiceAccountID )
			<< BUILD_LOG( kPacketAck.m_iOK )
			<< END_LOG;
	}


	IMPL_ON_FUNC( EPUBLISHER_SECURITY_AUTH_REQ )
	{
		UidType iActorUID = kPacket_.m_iUserUID;

		KEPUBLISHER_AUTHENTICATION_ACK kPacketAck;
		kPacketAck.m_iOK = NetError::ERR_GAMEFORGE_01;
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_CHANNEL_USER, NULL, EPUBLISHER_AUTHENTICATION_ACK, kPacketAck );
		KActorManager::GetKObj()->QueueingTo( iActorUID, spEvent );

		START_LOG( clog, L"[DEB] Security Auth Needs implementation" )
			<< BUILD_LOG( iActorUID )
			<< BUILD_LOG( kPacketAck.m_iOK )
			<< END_LOG;

		return;
	}
