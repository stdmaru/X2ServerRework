#pragma once
#include "RefreshSingleton.h"
#include <map>
#include <vector>

#include "ClientPacket.h"


struct KItemInfo;
struct KPostItemInfo;
class KRewardTable
{
	DeclareRefreshSingleton( KRewardTable );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclToStringW;
	DeclDump;

public:

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	enum EVENT_REWARD_ID
	{
		// 해외팀 각국가 전용 디파인
		ERI_SECOND_SECURITY = 10135,

		//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		ERI_COME_BACK_USER_TITLE				= 20150,	// 타이틀ID  - 다시 깨어난 자(휴면유저 칭호)
		ERI_COME_BACK_USER_ITEM_CUBE_ONE		= 10143,	// reward
		ERI_COME_BACK_USER_ITEM_CUBE_TWO		= 10144,	// reward
		ERI_COME_BACK_USER_ITEM_CUBE_THREE		= 10145,	// reward
		//}}


		ERI_CREATE_NEW_CHARACTER_REWARD_TITLE					= 35240, // 새로운 영웅의 탄생

	};
	//}}

	struct KRewardData
	{
		int				m_iRewardItemID;
		int				m_iQuantity;
		short			m_sPeriod;

		KRewardData(){};
		KRewardData( const KRewardData& t ) { *this = t; }    // copy constructor
		KRewardData& operator=( const KRewardData& t )       // assign operator
		{
			m_iRewardItemID = t.m_iRewardItemID;
			m_iQuantity = t.m_iQuantity;
			m_sPeriod = t.m_sPeriod;

			return *this;
		}
	};


public:
	KRewardTable(void);
	~KRewardTable(void);

	// for lua
	bool AddRewardData_LUA( int iRewardID, int iItemID, int iQuantity, short sPeriod );

	void GetRewardItem( IN const KPostItemInfo& kPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const;
	void GetRewardItem( IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const;
	bool GetRewardInfo( IN int iRewardID, OUT std::map< int, int >& mapRewardInfo ) const;
	void GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, int >& mapRewardInfo ) const;
	void GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::vector< std::wstring >& vecMessage ) const;

	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
	//}}


private:
	std::map< int, std::vector< KRewardData > >		m_mapRewardData;


	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
	//}}
};

DefRefreshSingletonInline( KRewardTable );
