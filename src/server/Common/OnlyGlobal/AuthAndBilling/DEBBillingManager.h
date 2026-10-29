#pragma once

#include <KNCSingleton.h>
#include "ThreadManager.h"


#include "Event.h"

class KDEBBillingManager : public KThreadManager
{
	DeclDump;
	DeclToStringW;
	DeclPfID;
	DeclareSingleton( KDEBBillingManager );

public:
	KDEBBillingManager(void);
	~KDEBBillingManager(void);
	void RegToLua();
	void Init( int nThreadNum );
	void AddWebServiceAddress( const char* szWSDL );
	virtual KThread* CreateThread();

    // Nothing to do at the moment. Redefined to prevent KPerformer Tick from being called and messing with the event queue.
	virtual void Tick() { };

protected:
	std::string m_strWsdlFile;
};

DefSingletonInline( KDEBBillingManager );
