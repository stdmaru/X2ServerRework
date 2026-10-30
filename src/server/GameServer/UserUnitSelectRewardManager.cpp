#include "./UserUnitSelectRewardManager.h"
#include "GameServer.h"


//{{ 2011. 05. 02  김민성	2차 보안 시스템


KUserUnitSelectRewardManager::KUserUnitSelectRewardManager(void)
{
	Clear();
}

KUserUnitSelectRewardManager::~KUserUnitSelectRewardManager(void)
{
}

void KUserUnitSelectRewardManager::Clear()
{
	m_vecAccessItem.clear();
}

void KUserUnitSelectRewardManager::AddRewardItemInfo( IN const int iAddRewardID )
{
	// 중복 되는 아이템은 지급 하지 않는다
	for( const int iRewardID : m_vecAccessItem )
	{
		if( iRewardID == iAddRewardID )
		{
			START_LOG( clog, L"중복 되는 아이템을 지급하려 합니다." )
				<< BUILD_LOG( iAddRewardID )
				<< BUILD_LOG( iRewardID )
				<< END_LOG;
			return;
		}
	}

	m_vecAccessItem.push_back( iAddRewardID );
}

void KUserUnitSelectRewardManager::GetRewardAndClear( OUT std::vector< int >& vecRewardInfo )
{
	// 보상 정보를 얻는다!
	vecRewardInfo = m_vecAccessItem;

	// 보상 정보를 넘겨주었으면 초기화 시킨다!
	Clear();
}

//{{ 2013. 01. 21  대규모 이벤트 보상 시 최초 선택 캐릭터에게 지급하는 시스템 - 김민성
//}

//}}
