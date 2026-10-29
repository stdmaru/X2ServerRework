#pragma once

#include "GameEvent.h"

class KGameEventDefenseDungeon : public KGameEvent
{
public:
	KGameEventDefenseDungeon();
	virtual ~KGameEventDefenseDungeon();

	void	SetDefenseDungeonOpenRate( const float fDefenseDungeonOpenRate )	{ m_fDefenseDungeonOpenRate = fDefenseDungeonOpenRate; }
	float	GetDefenseDungeonOpenRate( void ) const{	return m_fDefenseDungeonOpenRate;	}

private:
	float	m_fDefenseDungeonOpenRate;
};
