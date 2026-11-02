#include "GSUser.h"
#include "./UserRankingManager.h"
#include "GSRankingManager.h"


KUserRankingManager::KUserRankingManager(void) :
m_bHenirRankingInited( false )
, m_bHenirRankingChanged( false )
{
}

KUserRankingManager::~KUserRankingManager(void)
{
}

void KUserRankingManager::clear()
{
	m_bHenirRankingInited = false;
	m_bHenirRankingChanged = false;
}

void KUserRankingManager::Init( IN const std::vector< KHenirRankingInfo >& vecHenirRanking
	)
{
	std::vector< KHenirRankingInfo >::const_iterator vit = vecHenirRanking.begin();
	if( vit != vecHenirRanking.end() )
	{
        m_kHenirRanking = *vit;
        m_bHenirRankingInited = true;
	}

}

void KUserRankingManager::GetUpdateInfo( OUT std::vector< KHenirRankingInfo >& vecHenirRanking )
{
	vecHenirRanking.clear();

	if( m_bHenirRankingInited  &&  m_bHenirRankingChanged )
	{
		vecHenirRanking.push_back( m_kHenirRanking );
	}
}

void KUserRankingManager::GetRankingInfo( IN KGSUserPtr spUser, IN const KEGS_GET_RANKING_INFO_REQ& kReq, OUT KEGS_GET_RANKING_INFO_ACK& kAck )
{
	switch( kReq.m_iRankingType )
	{
	case SEnum::RT_DAY_RANKING:
	case SEnum::RT_WEEK_RANKING:
	case SEnum::RT_MONTH_RANKING:
		{
			if( m_bHenirRankingInited )
			{
                kAck.m_vecMyHenirRanking.push_back( m_kHenirRanking );
			}
		}
		break;

	case SEnum::RT_DUNGEON_RANKING:
		{
			KDungeonRankingInfo kRankingInfo;
			if( SiKGSRankingManager()->GetDungeonRankingByUnitUID( spUser->GetCharUID(), kRankingInfo ) == true )
			{
				kAck.m_vecMyDungeonRanking.push_back( kRankingInfo );
			}
		}
		break;

	case SEnum::RT_PVP_RANKING:
		{
			KPvpRankingInfo kRankingInfo;
			if( SiKGSRankingManager()->GetPvpRankingByUnitUID( spUser->GetCharUID(), kRankingInfo ) == true )
			{
				kAck.m_vecMyPvpRanking.push_back( kRankingInfo );
			}
		}
		break;


	default:
		{
			START_LOG( cerr, L"RankingType does not exist." )
				<< BUILD_LOG( kReq.m_iRankingType )
				<< END_LOG;
		}
	}
}

void KUserRankingManager::SetRankingInfo( IN const KHenirRankingInfo& kInfo )
{
	m_kHenirRanking = kInfo;
	m_bHenirRankingInited = true;
	m_bHenirRankingChanged = true;
}
