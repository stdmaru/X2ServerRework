
#include "ChannelUser.h"

		#include <dbg/dbg.hpp>

		//{{ 2012. 07. 02 Choi Yuksa Queue System
			#include "CHFSM.h"

		#include "DBLayer.h"
		#include "NetError.h"
		#include "ChannelSimLayer.h"
		#include "ChannelServer.h"

		//{{ 2009. 12. 16	Choi Yuk-sa’s	adjacent tool
		#include "CCUToolManager.h"

		//{{ 2011. 01. 20	Minseong Kim	Concurrent monitoring tool DBary Injection
			#include "ODBC/Odbc.h"

		#include <boost/format.hpp>

		//{{ 2011. 09. 14	Minseong Kim	Hacking process list delivered - Step before game loading

		//{{ 2011. 09. 26	Minseong Kim	Machine ID check - Step before game loading
			#include "GameSysVal/GameSysVal.h"

		//{{ 2011.11.02		Minseong Kim	Issue and validate SN on server
		//}}
		//{{ 2012. 07. 02	Choi Yuksa		Queue System
			#include "WaitQueueManager.h"

		//{{ 2012. 07. 18	Choi Yuksa	reconnection logic

		#include "../Common/OnlyGlobal/AuthAndBilling/DEBAuthManager.h"	// Manager (Debelzak Custom Auth)

		void KChannelUser::RequestPublisherLogin(const KECH_VERIFY_ACCOUNT_REQ& kPacket_)
		{
			START_LOG( clog, L"[DEB] Authentication Request." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kPacket_.m_wstrID )
				<< BUILD_LOG( kPacket_.m_wstrPassword )
				<< BUILD_LOG( kPacket_.m_bLogin )
				<< END_LOG;

			// Authentication Logic
			KEPUBLISHER_AUTHENTICATION_REQ kPacketReq;

			kPacketReq.m_iActorUID = GetUID();
			kPacketReq.m_wstrIP = KncUtil::toWideString( GetIPStr() );;
			kPacketReq.m_wstrServiceAccountID = kPacket_.m_wstrID;
			kPacketReq.m_wstrServicePassword = kPacket_.m_wstrPassword;

				kPacketReq.m_bServerUseKogOTP = KSimLayer::GetKObj()->GetUseKogOTP();


			KEventPtr spEvent( new KEvent );
			UidType anTrace[2] = { GetUID(), -1 };
			spEvent->SetData(PI_NULL, anTrace, EPUBLISHER_AUTHENTICATION_REQ, kPacketReq );
			SiKDEBAuthManager()->QueueingEvent( spEvent );
		}

		void KChannelUser::HandlePublisherLoginAck(const KEPUBLISHER_AUTHENTICATION_ACK& kPacket_, OUT bool& bDoNotDisconnect)
		{
			if( kPacket_.m_iOK == NetError::NOT_LEAVE_ROOM_REASON_01)
			{
				bDoNotDisconnect = false;
			}
			else
			{
				bDoNotDisconnect = true;
			}
		}

		void KChannelUser::RequestKOGOTPLogin(const KECH_VERIFY_ACCOUNT_REQ& kPacket_)
		{
			// Since Giant authentication has already been completed in X2StateLogin, replace it with KOG OTP authentication
			KDBE_CH_USER_KOGOTP_LOGIN_REQ kPacketReq;
			kPacketReq.m_wstrServiceAccountID = kPacket_.m_wstrID;
			kPacketReq.m_wstrOTP = kPacket_.m_wstrPassword;
			kPacketReq.m_wstrIP = KncUtil::toWideString(KSession::GetIPStr());

				kPacketReq.m_wstrPurchaseTok = L"";

				kPacketReq.m_iChannelingCode = kPacket_.m_iChannelingCode;

			SendToAccountDB( DBE_CH_USER_KOGOTP_LOGIN_REQ, kPacketReq );
		}

		void KChannelUser::HandleKOGOTPLoginAck()
		{
			// In the case of the United States, there is nothing special to do here.
		}

		void KChannelUser::RequestPublisherSecurityAuth(const KECH_SECURITY_AUTH_REQ& kPacket_)
		{
			if( kPacket_.m_iSecurityAuthType == KECH_SECURITY_AUTH_REQ::SAT_CHK_SECOND_SECURITY )
			{
				// Authentication Logic
				KEPUBLISHER_SECURITY_AUTH_REQ kPacketReq;

				kPacketReq.m_iSecurityAuthType = kPacket_.m_iSecurityAuthType;
				kPacketReq.m_iUserUID = GetUID();
				kPacketReq.m_uiPublisherUID = (unsigned int)(_wtoi( kPacket_.m_wstrUserID.c_str() ));

				START_LOG( clog, L"[DEB] Security Authentication request." )
					<< BUILD_LOG( kPacketReq.m_uiPublisherUID )
					<< END_LOG;

				KEventPtr spEvent( new KEvent );
				UidType anTrace[2] = { GetUID(), -1 };
				spEvent->SetData(PI_NULL, anTrace, EPUBLISHER_SECURITY_AUTH_REQ, kPacketReq );

				SiKDEBAuthManager()->QueueingEvent( spEvent );
			}

		}

		void KChannelUser::HandlePublisherSecurityAuthAck(const KEPUBLISHER_SECURITY_AUTH_ACK& kPacket_)
		{
			KECH_SECURITY_AUTH_ACK kAck;
			kAck.m_iOK	= kPacket_.m_iOK;
			kAck.m_strSecondSecurityURL = kPacket_.m_strSecondSecurityURL;	// secondary related URL

			SendPacket( ECH_SECURITY_AUTH_ACK, kAck );
		}

		void KChannelUser::HandlePublisherSecurityInfoNot(const KEPUBLISHER_SECURITY_INFO_NOT& kPacket_)
		{

		}
