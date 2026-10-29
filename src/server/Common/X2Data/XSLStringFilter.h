#pragma once

#include "XSLStat.h"

//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	#include "RefreshSingleton.h"
//}}

class CXSLStringFilter
{
	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	DeclareRefreshSingleton( CXSLStringFilter );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;
	//}}

public:

	enum FILTER_TYPE
	{
		//FT_WORD,		//해당 String속에 문제의 단어가 있는가? --"이런 개새끼!!"
		//FT_SENTENCE,	//해당 String을 하나의 단어로 보고 문제가 있는가? -- "운영자"
		FT_NICKNAME = 0,
		FT_CHAT,
//#ifdef SERV_POST_BAN_WORD_FILTER
		FT_POST,
//#endif //SERV_POST_BAN_WORD_FILTER
	};


	CXSLStringFilter();
	virtual ~CXSLStringFilter(void);

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	//}}

	bool CheckIsValidString( FILTER_TYPE filterType, std::wstring& wstringForCheck );		//유효한 스트링인가?
	//std::wstring FilteringChatString( const WCHAR* pString, WCHAR wcharForReplace ); -- 서버에서는 안쓰는코드

	void	AddNickNameFilter_LUA( char* szFilter );
	//{{ 2009. 2. 17  최육사	닉네임 포함 문자
	void	AddNickNameWordFilter_LUA( char* szFilter );
	//}}
	void	AddWordFilter_LUA( char* szFilter );

	void	AddPostWordFilter_LUA( char* szFilter );


protected:

	inline void MakeUpperCase(std::wstring &str)
	{
		for( std::wstring::iterator i = str.begin(); i != str.end(); i++ )
			*i = (WCHAR)towupper(*i);
	}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	//}}

	std::vector< std::wstring > m_BanNickNameList;
	//{{ 2009. 2. 17  최육사	닉네임 포함 문자
	std::vector< std::wstring > m_BanNickNameWordList;
	//}}
	std::vector< std::wstring > m_BanWordList;
	std::vector< std::wstring > m_BanPostWordList;
};

//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	DefRefreshSingletonInline( CXSLStringFilter );
//}}
