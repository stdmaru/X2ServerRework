#include "ChannelSimLayer.h"
#include <iomanip>  // std::setiosflags, std::resetiosflags
#include "ActorManager.h"
#include "DBLayer.h"
#include "ChannelServer.h"
#include "KAutoPath.h"


		#include "../Common/OnlyGlobal/AuthAndBilling/DEBAuthManager.h"

	#include "../Common/OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"
#include "ChannelUser.h"
#include "DBLayer.h"
#include "NetError.h"

//#include "GASHAuthManager.h"

//{{ 2010. 02. 16	Choi Yuksa		hacking tool list
	#include "HackingProtectionManager.h"

//{{ 2011. 09. 26	Minseong Kim Machine		ID check - Step before game loading
	#include "GameSysVal/GameSysVal.h"

//{{ 2012. 07. 04	Choi Yuksa		Queue System
	#include "WaitQueueManager.h"

//{{ 2012. 07. 18	Yuksa Choi		reconnection logic

//{{ 2012. 10. 15	Park Se-hoon	SMS phone number integrated management

#include <lua_tinker.h>
using namespace lua_tinker;

#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

NiImplementRTTI( KChannelSimLayer, KSimLayer );

KChannelSimLayer::KChannelSimLayer(void)
{
	//{{ 2011. 08. 24	Choi Yuk-sa		channel server maintenance mode
		m_eCheckIPMode = CIM_SERVICE_MODE;

	//{{ 2011.11.02     Minseong Kim    Issue and validate SN on server

}

KChannelSimLayer::~KChannelSimLayer(void)
{

}

ImplToStringW( KChannelSimLayer )
{
    	return START_TOSTRING_PARENTW( KSimLayer );
}

ImplOstmOperatorW2A( KChannelSimLayer );

void KChannelSimLayer::RegToLua()
{
    KSimLayer::RegToLua();

	lua_tinker::class_add<KChannelSimLayer>( g_pLua, "KChannelSimLayer" );
	lua_tinker::class_inh<KChannelSimLayer, KSimLayer>( g_pLua );
	lua_tinker::class_def<KChannelSimLayer>( g_pLua, "dump",					&KChannelSimLayer::Dump );
	lua_tinker::class_def<KChannelSimLayer>( g_pLua, "AddIP",					&KChannelSimLayer::AddIP );
	lua_tinker::class_def<KChannelSimLayer>( g_pLua, "SetCheckIPType",			&KChannelSimLayer::SetCheckIPType );
	//{{ 2011. 08. 24	Choi Yuksa		Channel server check mode
		lua_tinker::class_def<KChannelSimLayer>( g_pLua, "AddCheckModeIP",			&KChannelSimLayer::AddCheckModeIP );
		lua_tinker::class_def<KChannelSimLayer>( g_pLua, "SetCheckModeCheckIPType",	&KChannelSimLayer::SetCheckModeCheckIPType );
		lua_tinker::class_def<KChannelSimLayer>( g_pLua, "SetCheckIPMode",			&KChannelSimLayer::SetCheckIPMode_LUA );
	//}}

	lua_tinker::decl( g_pLua, "SimLayer", this );
	lua_tinker::decl( g_pLua, "CIT_ALL_IP_ALLOW",		KChannelSimLayer::CIT_ALL_IP_ALLOW );
	lua_tinker::decl( g_pLua, "CIT_CHECK_IP_ALLOW",		KChannelSimLayer::CIT_CHECK_IP_ALLOW );
	lua_tinker::decl( g_pLua, "CIT_CHECK_IP_BLOCK",		KChannelSimLayer::CIT_CHECK_IP_BLOCK );

	//{{ 2011. 08. 24	Choi Yuksa		Channel server check mode
		lua_tinker::decl( g_pLua, "CIM_SERVICE_MODE",		KChannelSimLayer::CIM_SERVICE_MODE );
		lua_tinker::decl( g_pLua, "CIM_CHECK_MODE",			KChannelSimLayer::CIM_CHECK_MODE );

	//{{ 2013.03.21 Jo	Hyo-jin		By country AuthManager Variation
			SiKDEBAuthManager()->RegToLua();
}


void KChannelSimLayer::Init()
{
	//{{ 2013.03.21 Jo	Hyo-jin		By country AuthManager Variation
			m_vecpThreadMgr.push_back( SiKDEBAuthManager()->GetInstance() );

	//{{ 2011. 09. 26  Minseong Kim		Machine ID check - step before game loading
		KAutoPath kAutoPath;
		std::string	strFile;
		bool bRVal = true;

		{
			LoadingTimer lt( L"Enum.lua" );

			//If the data loading done in �� fails later, the server must be shut down.
			strFile = "Enum.lua";
			kAutoPath.GetPullPath( strFile );
			if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
			{
				START_LOG( cerr, L"Failed to load enum information.!" )
					<< BUILD_LOG( KncUtil::toWideString( strFile ) );
				KBaseServer::GetKObj()->AddFailScriptFileName( L"Enum.lua" );
			}
			else
			{
				START_LOG( cout, L"Enum information loading successfully!" );
			}
		}

		{
			LoadingTimer lt( L"DungeonEnum.lua" );

			//If the data loading done in �� fails later, the server must be shut down.
			strFile = "DungeonEnum.lua";
			kAutoPath.GetPullPath( strFile );
			if( 0 != LUA_DOFILE( g_pLua, strFile.c_str() ) )
			{
				START_LOG( cerr, L"DungeonEnum Failed to load information!" )
					<< BUILD_LOG( KncUtil::toWideString( strFile ) );
				KBaseServer::GetKObj()->AddFailScriptFileName( L"DungeonEnum.lua" );
			}
			else
			{
				START_LOG( cout, L"DungeonEnum Information loaded successfully!" );
			}
		}

		{
			KGameSysVal::RegScriptName( "GameSysValTable.lua" );
			OPEN_SCRIPT_FILE( KGameSysVal );
		}

	//{{ 2012. 07. 04	Choi Yuksa		Queue System
		{
			KWaitQueueManager::RegScriptName( "WaitQueueData.lua" );
			OPEN_SCRIPT_FILE( KWaitQueueManager );
		}

	//{{ 2012. 10. 15	Park Se-hoon	SMS phone number integrated management

	//{{ 2010. 02. 16	Choi Yuksa		hacking tool list
		SiKHackingProtectionManager();

    KSimLayer::Init();


}

void KChannelSimLayer::Tick()
{
    KSimLayer::Tick();

	//{{ 2013.03.21 Jo	Hyo-jin		By country AuthManager Variation


	//{{ 2010. 02. 16		Choi Yuksa		hacking tool list
		SiKHackingProtectionManager()->Tick();

	//{{ 2012. 07. 04	Choi Yuksa	Queue System
		SiKWaitQueueManager()->Tick();

	//{{ 2012. 07. 18	Choi Yuksa	reconnection logic
}

void KChannelSimLayer::ShutDown()
{
    KSimLayer::ShutDown();

	//{{ 2013.03.21 Jo	Hyo-jin		By country AuthManager Variation
			SiKDEBAuthManager()->ReleaseInstance();


	//{{ 2010. 02. 16	Choi Yuksa	hacking tool list
		KHackingProtectionManager::ReleaseInstance();

	//{{ 2012. 07. 04	Choi Yuksa	Queue System
		KWaitQueueManager::ReleaseInstance();

	//{{ 2012. 11. 22	Park Se-hoon	SMS phone number integrated management
}

//{{ 2011. 08. 24	Choi Yuk-sa		channel server maintenance mode
	void KChannelSimLayer::SetCheckIPMode_LUA( int iVal )
	{
		m_eCheckIPMode = static_cast<CHECK_IP_MODE>(iVal);

		switch( m_eCheckIPMode )
		{
			case CIM_SERVICE_MODE:
			{
				START_LOG( cout, L"------------ Channel server normal mode! ------------" );
					SiKProcessCommunicationManager()->QueueingProcessWrite(boost::str(boost::wformat(L"%1%_%2%") % 0 %L"Channel Server Normal Mode!"));
			}
			break;

			case CIM_CHECK_MODE:
			{
				START_LOG( cout, L"------------ Channel server maintenance mode! ------------" );
					SiKProcessCommunicationManager()->QueueingProcessWrite(boost::str(boost::wformat(L"%1%_%2%") % 0 %L"Channel server check mode!"));
			}
			break;

			default:
			{
				START_LOG( cerr, L"Invalid channel server mode setting!" )
					<< BUILD_LOG( iVal )
					<< END_LOG;

				// Service mode applied by default
				m_eCheckIPMode = CIM_SERVICE_MODE;
			}
			break;
		}
	}

void KChannelSimLayer::AddIP( const char* szAddIP )
{
	if( szAddIP == NULL )
	{
		START_LOG( cerr, L"IP string anormal." )
			<< END_LOG;

		return;
	}

	std::string strIP = szAddIP;
	if( strIP.size() < 1 || strIP.size() > 15 )
	{
		START_LOG( cerr, L"IP a ser adicionado anormal." )
			<< BUILD_LOG( strIP.size() )
			<< END_LOG;

		return;
	}

	START_LOG( cout, L"=== ADDED IP ===" )
		<< BUILD_LOG( strIP.c_str() )
		<< BUILD_LOG( strIP.size() )
		<< L"================"
		<< dbg::endl;

	//{{ 2011. 08. 24	Choi Yuk-sa		channel server maintenance mode
		m_kConnectIpCheck[CIM_SERVICE_MODE].m_vecIP.push_back( strIP );
}

void KChannelSimLayer::SetCheckIPType( int iVal )
{
	switch( iVal )
	{
		case CIT_ALL_IP_ALLOW: START_LOG( cout, L"Allow all IPs!" ); break;
		case CIT_CHECK_IP_ALLOW: START_LOG( cout, L"Only allow specific IPs!" ); break;
		case CIT_CHECK_IP_BLOCK: START_LOG( cout, L"Block only specific IPs!" ); break;

		default:
		{
			START_LOG( cerr, L"Undefined IP permission type!" )
				<< BUILD_LOG( iVal )
				<< END_LOG;
			return;
		}
	}

	//{{ 2011. 08. 24	Choi Yuk-sa		channel server maintenance mode
		m_kConnectIpCheck[CIM_SERVICE_MODE].m_eCheckIPType = static_cast<CHECK_IP_TYPE>(iVal);
}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 08. 24	Choi Yuksa		channel server maintenance mode
	void KChannelSimLayer::AddCheckModeIP( const char* szAddIP )
	{
		if( szAddIP == NULL )
		{
			START_LOG( cerr, L"[Maintenance Mode] IP is invalid." )
				<< END_LOG;

			return;
		}

		std::string strIP = szAddIP;
		if( strIP.size() < 1 || strIP.size() > 15 )
		{
			START_LOG( cerr, L"[Maintenance Mode] IP or more you want to add." )
				<< BUILD_LOG( strIP.size() )
				<< END_LOG;

			return;
		}

		START_LOG( cout, L"=== [Maintenance Mode] ADDED IP ===" )
			<< BUILD_LOG( strIP.c_str() )
			<< BUILD_LOG( strIP.size() )
			<< L"================"
			<< dbg::endl;

		m_kConnectIpCheck[CIM_CHECK_MODE].m_vecIP.push_back( strIP );
	}

	void KChannelSimLayer::SetCheckModeCheckIPType( int iVal )
	{
		switch( iVal )
		{
			case CIT_ALL_IP_ALLOW:
				START_LOG( cout, L"[Maintenance Mode] Allow all IPs!" );
				break;

			case CIT_CHECK_IP_ALLOW:
				START_LOG( cout, L"[Maintenance Mode] Allow only specific IPs!" );
				break;

			case CIT_CHECK_IP_BLOCK:
				START_LOG( cout, L"[Maintenance Mode] Block only specific IPs!" );
				break;

			default:
				START_LOG( cerr, L"[Maintenance Mode] Undefined IP permission type!" )
					<< BUILD_LOG( iVal )
					<< END_LOG;
				return;
		}

		m_kConnectIpCheck[CIM_CHECK_MODE].m_eCheckIPType = static_cast<CHECK_IP_TYPE>(iVal);
	}

	bool KChannelSimLayer::CheckIP( const char* szIP )
	{
		return CheckIP( m_eCheckIPMode, szIP );
	}

	bool KChannelSimLayer::CheckIP( IN const CHECK_IP_MODE eCheckIpMode, IN const char* szIP )
	{
		switch( eCheckIpMode )
		{
			case CIM_SERVICE_MODE:
			case CIM_CHECK_MODE:
				break;

			default:
				START_LOG( cerr, L"Invalid check mode!" )
					<< BUILD_LOG( eCheckIpMode )
					<< END_LOG;

				// Let's pass it for now!
				return true;
		}

		// If all IPs are allowed, pass!
		if( m_kConnectIpCheck[eCheckIpMode].m_eCheckIPType == CIT_ALL_IP_ALLOW )
			return true;

		// If the check IP list is empty, pass!
		if( m_kConnectIpCheck[eCheckIpMode].m_vecIP.empty() )
			return true;

		if( szIP == NULL )
		{
			START_LOG( cerr, L"IP or more you want to compare." )
				<< END_LOG;

			return false;
		}

		std::string strIP = szIP;
		if( strIP.size() < 1 || strIP.size() > 15 )
		{
			START_LOG( cerr, L"The IP length or more you want to compare." )
				<< BUILD_LOG( strIP.size() )
				<< END_LOG;

			return false;
		}

		// What if a specific IP is allowed?
		if( m_kConnectIpCheck[eCheckIpMode].m_eCheckIPType == CIT_CHECK_IP_ALLOW )
		{
			std::vector< std::string >::iterator vit;
			for( vit = m_kConnectIpCheck[eCheckIpMode].m_vecIP.begin(); vit != m_kConnectIpCheck[eCheckIpMode].m_vecIP.end(); ++vit )
			{
				if( vit->size() < 1 || vit->size() > 15 )
				{
					START_LOG( cerr, L"More than just the IP you want to compare." )
						<< BUILD_LOG( vit->size() )
						<< END_LOG;

					return false;
				}

				// Allowed as long as it is in the appropriate IP band!
				if( vit->compare( 0, vit->size(), strIP, 0, vit->size() ) == 0 )
				{
					return true;
				}
			}

			return false;
		}
		// What if a specific IP is blocked?
		else if( m_kConnectIpCheck[eCheckIpMode].m_eCheckIPType == CIT_CHECK_IP_BLOCK )
		{
			std::vector< std::string >::iterator vit;
			for( vit = m_kConnectIpCheck[eCheckIpMode].m_vecIP.begin(); vit != m_kConnectIpCheck[eCheckIpMode].m_vecIP.end(); ++vit )
			{
				if( vit->size() < 1 || vit->size() > 15 )
				{
					START_LOG( cerr, L"IP or more you want to compare." )
						<< BUILD_LOG( vit->size() )
						<< END_LOG;

					return false;
				}

				// If it is in that IP range, block it!
				if( vit->compare( 0, vit->size(), strIP, 0, vit->size() ) == 0 )
				{
					return false;
				}
			}

			return true;
		}

		START_LOG( cerr, L"Invalid IP check type." )
			<< BUILD_LOG( m_kConnectIpCheck[eCheckIpMode].m_eCheckIPType )
			<< END_LOG;

		return false;
	}
