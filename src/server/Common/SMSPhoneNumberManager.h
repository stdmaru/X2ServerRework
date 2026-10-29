#pragma once

#include "RefreshSingleton.h"
#include <vector>
#include "CommonPacket.h"


//{{ 2012. 09. 26	최육사		우편 복사 버그 수정


class KSMSPhoneNumberManager
{
	DeclareRefreshSingleton( KSMSPhoneNumberManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	KSMSPhoneNumberManager(void);
	~KSMSPhoneNumberManager(void);

	enum PHONE_NUMBER_LIST_TYPE
	{
		PNLT_ELSWORD_DEV_TEAM			= 0,
		//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//#ifdef SERV_SMS_TOTAL_MANAGER
		PNLT_ELSWORD_QA_TEAM			= 1,
		PNLT_ELSWORD_MOR_TEAM			= 2,
		PNLT_NEXON						= 3,
//#endif SERV_SMS_TOTAL_MANAGER
		//}}

//#ifdef SERV_MORNITORING_ABUSER_NOTIFY_SMS// 작업날짜: 2013-05-22	// 박세훈
		PNLT_ELSWORD_DEV_TEAM_SERVER	= 4,
//#endif // SERV_MORNITORING_ABUSER_NOTIFY_SMS

		PNLT_MAX,
	};

	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}

public:

	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	// for lua
	void AddPhoneNumber_LUA( int iType, const char* pPhoneNum );

	// function
	void GetPhoneNumberList( IN const PHONE_NUMBER_LIST_TYPE eType, OUT std::vector< std::wstring >& vecPhoneNumList ) const;
	//}}

private:
	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	std::vector< std::wstring >				m_vecPhoneNumList[PNLT_MAX];
	//}}
};

DefRefreshSingletonInline( KSMSPhoneNumberManager );


//}}
