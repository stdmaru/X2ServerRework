#pragma once

#include "GameEvent.h"

//{{ 2011. 08. 12   김민성      헤니르 개편

class KGameEventHenirReward : public KGameEvent
{
public:
	KGameEventHenirReward(void);
	virtual ~KGameEventHenirReward(void);

	void	SetEventID( int iEventID )						{ m_iEventID = iEventID; }
	int		GetEventID()									{ return m_iEventID; }

	void	SetHenirRewardEventCount( int iEventCount )		{ m_iHenirRewardEventCount = iEventCount; }
	int		GetHenirRewardEventCount()						{ return m_iHenirRewardEventCount; }

	void	SetHenirRewardUnLimited( bool bUnLimited )		{ m_bHenirRewardUnLimited = bUnLimited; }
	bool	GetHenirRewardUnLimited()						{ return m_bHenirRewardUnLimited; }

	//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	void	SetServerGroupID( int iServerGroupID )			{ m_iServerGroupID = iServerGroupID; }
	int		GetServerGroupID() const						{ return m_iServerGroupID; }
	//}}


private:

	int						m_iEventID;

	int						m_iHenirRewardEventCount;		//  event 추가 보상 횟수
	bool					m_bHenirRewardUnLimited;		//  무제한
	//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	int						m_iServerGroupID;
	//}}

};

//}}
