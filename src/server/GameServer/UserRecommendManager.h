#pragma once
#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"

class KUserRecommendManager
{
public:
	enum RECOMMEND_ENUM
	{
		PAGE_PER_COUNT = 8,
	};

	KUserRecommendManager(void);
	~KUserRecommendManager(void);

	void Clear();

	void SetIsRecommend( bool bVal )	{ m_bIsRecommend = bVal; }
	bool IsRecommend()					{ return m_bIsRecommend; }

	void SetRecommendedUnitUID( UidType iUnitUID ) { m_iRecommendedUnitUID = iUnitUID; }
	UidType GetRecommendedUnitUID()		{ return m_iRecommendedUnitUID; }

	bool IsInitRecommendUserList()		{ return m_bInitRecommendUserList; }
	void SetRecommendUserList( IN std::vector< KRecommendUserInfo >& vecUserList );
	void GetRecommendUserList( IN OUT u_int& uiViewPage, OUT u_int& uiTotalPage, OUT std::vector< KRecommendUserInfo >& vecUserList );

	void AddRecommendUser( IN const KRecommendUserInfo& kInfo );
	void ModifyRecommendUser( IN const KELG_RECOMMEND_USER_INFO_NOT& kInfo );

private:
	bool								m_bIsRecommend;				// 추천을 할수 있는 상태인지 여부
	UidType								m_iRecommendedUnitUID;		// 내가 추천한 유저 UnitUID

	bool								m_bInitRecommendUserList;	// 추천인 리스트 초기화
	std::vector< KRecommendUserInfo >	m_vecRecommendUserList;		// 추천인 리스트

};
