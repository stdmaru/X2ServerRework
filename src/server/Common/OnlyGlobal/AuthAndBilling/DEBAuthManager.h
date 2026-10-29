#pragma once

#include <KNCSingleton.h>
#include "ThreadManager.h"

#include "Event.h"
#include "CommonPacket.h"

	class KDEBAuthManager : public KThreadManager
	{
		DeclDump;
		DeclToStringW;
		DeclPfID;
		DeclareSingleton( KDEBAuthManager );

		public:
			KDEBAuthManager();
			virtual ~KDEBAuthManager();

			void RegToLua();
			void Init( int nThreadNum );
			void AddAuthServiceAddress( const char* szURL );
			//void AddWebMethod( int iSoapAction, const char* szMethod );

			// derived from KThreadManager
			virtual KThread* CreateThread();

			// Currently there is nothing to do. KPerformer Tick is redefined to prevent it from being called and touching the event queue //
			// Comment out SiKDEBAuthManager()->Tick(); in the original ChannelSimLayer Tick()
			virtual void Tick();


		protected:
			std::string m_strWsdlPath;

	};

	DefSingletonInline( KDEBAuthManager );
