#pragma once

#include "GameEvent.h"

class KGameEventItemDrop : public KGameEvent
{
public:
	KGameEventItemDrop(void);
	virtual ~KGameEventItemDrop(void);

	//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장
	void	SetDropRate( IN const float fDropRate )	{ m_fDropRate = fDropRate; }
	float	GetDropRate( void ) const	{ return m_fDropRate; }

	void	SetWithPlayPcBang( bool bVal )			{ m_bWithPlayPcBang = bVal; }
	bool	IsWithPlayPcBang()						{ return m_bWithPlayPcBang; }

	//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )

private:
	float	m_fDropRate;
	bool	m_bWithPlayPcBang;

	//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )
	//}}
};
