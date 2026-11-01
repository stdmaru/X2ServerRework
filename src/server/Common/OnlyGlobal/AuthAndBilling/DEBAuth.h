#pragma once
#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"
#include "SubThread.h"
#include "KncException.h"
#include "DEB/SoapClient.h"

	class KDEBAuth :public KSubThread
	{
		_DeclareException;

		public:
			#undef _ENUM
			#define _ENUM( id ) id,
			enum
			{
				_ENUM( DEB_AUTH )
				_ENUM( DEB_AUTH_SECOND_SECURITY )
				_ENUM( DEB_NUM )
			};

			KDEBAuth(std::string WsdlFile);
			virtual ~KDEBAuth();
			virtual void Run();
			virtual void ProcessEvent( const KEventPtr& spEvent );

			DECL_ON_FUNC( EPUBLISHER_AUTHENTICATION_REQ );
			DECL_ON_FUNC( EPUBLISHER_SECURITY_AUTH_REQ );

			void testfunction();

		protected:
			x2::deb::SoapClient	m_kSoapClient;	// Port: own SOAP/HTTP client in place of gSOAP (decision T31)
	};
