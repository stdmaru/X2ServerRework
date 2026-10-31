#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include <map>
#include "x2/platform/timer.h"
#include "KncSend.h"


//////////////////////////////////////////////////////////////////////////
// Server CCU Manager : 동접 정보 관리

class KGlobalServerCCUManager
{
	DeclareSingleton( KGlobalServerCCUManager );
	DeclDump;
	DeclToStringW;

	enum ENUM_TIMER
	{
		TM_CCU_UPDATE = 0,

		TM_TOTAL_NUM,
	};

public:
	KGlobalServerCCUManager(void);
	virtual ~KGlobalServerCCUManager(void);

	void		Tick();
	void		UpdateCCUInfo( IN const KUpdateCCUInfo& kInfo );
	int			GetNowTotalCCU();


private:
	std::map< UidType, KUpdateCCUInfo >		m_mapCCUInfo;
};

DefSingletonInline( KGlobalServerCCUManager );
