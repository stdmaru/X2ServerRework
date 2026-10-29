#include "GameEventHenirReward.h"

//{{ 2011. 08. 12   김민성      헤니르 개편

KGameEventHenirReward::KGameEventHenirReward(void)
	: m_iHenirRewardEventCount(0)
//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
, m_iServerGroupID(-1)
//}}
, m_bHenirRewardUnLimited( false )
{
}

KGameEventHenirReward::~KGameEventHenirReward(void)
{
}

//}}
