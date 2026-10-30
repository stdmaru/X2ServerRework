#include "BaseServer.h"
#include "GSWeddingHall.h"
#include "Enum/Enum.h"



//{{ 2013. 04. 01	 인연 시스템 - 김민성

KGSWeddingHall::KGSWeddingHall( const KWeddingHallInfo& kInfo )
{
	std::set< UidType > setUserListDumy;
	UpdateRoomInfo( kInfo, setUserListDumy );
}

KGSWeddingHall::~KGSWeddingHall(void)
{
}

void KGSWeddingHall::UpdateRoomInfo( IN const KGSWeddingHall& kInfo )
{
	m_kHallInfo = kInfo.m_kHallInfo;
	m_setJoinUnitList = kInfo.m_setJoinUnitList;
}

void KGSWeddingHall::UpdateRoomInfo( IN const KWeddingHallInfo& kInfo, IN std::set< UidType >& setList )
{
	m_kHallInfo = kInfo;
	m_setJoinUnitList = setList;
}
//}
