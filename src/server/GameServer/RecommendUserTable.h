#pragma once

#include "ServerDefine.h"
#include "RefreshSingleton.h"
#include <map>


class KRecommendUserTable
{
private:
	DeclareRefreshSingleton( KRecommendUserTable );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclToStringW;
	DeclDump;

public:
	KRecommendUserTable(void);
	~KRecommendUserTable(void);

	// for lua
	bool AddRecommendUserData_LUA( u_char ucLevel, int iRewardID );

	// result
	int GetRecommendUserReward( IN u_char ucLevel ) const;

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
	//}

private:
	std::map< u_char, int > m_mapRecommendData;

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
	//}
};

DefRefreshSingletonInline( KRecommendUserTable );
