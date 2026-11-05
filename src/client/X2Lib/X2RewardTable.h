#pragma once

#include <map>
#include <vector>

struct KItemInfo;
struct KPostItemInfo;
class CX2RewardTable
{
public:
	struct KRewardData
	{
		int				m_iRewardItemID;
		int				m_iQuantity;
		short			m_sPeriod;

		KRewardData(){
			m_iRewardItemID	= -1;
			m_iQuantity		= 0;
			m_sPeriod		= 0;
		};
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
	CX2RewardTable(void);
	~CX2RewardTable(void);

	bool OpenScriptFile( const wchar_t* pFileName );
	bool AddRewardData_LUA( int iRewardID, int iItemID, int iQuantity, short sPeriod );
	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{ 2011. 10.26    김민성	캐릭터 레벨업 이벤트
	//}}

	void MakeSystemLetterTitle( IN const std::wstring& wstrNickName, IN OUT KPostItemTitleInfo& kPostTitle );
	void MakeSystemLetter( IN const std::wstring& wstrNickName, IN OUT KPostItemInfo& kPostItem );

	void AssignStringFromRewardTable( IN const int iRewardID, OUT std::wstring& wstrMessage, IN const bool bOnlyItemName = false );

	void GetRewardItem( IN const KPostItemInfo& kPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const;
	void GetRewardItem( IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const;
	bool GetRewardInfo( IN int iRewardID, OUT std::map< int, int >& mapRewardInfo ) const;
	void GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, int >& mapRewardInfo ) const;
	void GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::vector< std::wstring >& vecMessage ) const;

private:
	std::map< int, std::vector< KRewardData > > m_mapRewardData;
};
