#pragma once

#include "GameEvent.h"

//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
class KGameEventQuestItemDrop : public KGameEvent
{
public:
	KGameEventQuestItemDrop(void);
	virtual ~KGameEventQuestItemDrop(void);

	void	SetQuestItemDropRate( float fQuestItemDropRate )			{ m_fQuestItemDropRate = fQuestItemDropRate; }
	float	GetQuestItemDropRate()										{ return m_fQuestItemDropRate; }

private:
	float	m_fQuestItemDropRate;
};
//}
