#pragma once
#include <boost/timer.hpp>


//{{ 2013. 02. 	최육사	Tick퍼포먼스 DB로그 기록


class KSimLayerTickChecker
{
public:
	KSimLayerTickChecker();
	virtual ~KSimLayerTickChecker();

	void Reset();
	DWORD GetSimLayerMaxTick() const				{ return m_dwMaxTick; }
	const std::wstring GetSimLayerMaxTickRegDate() const;
	void CheckSimLayerTick();

private:
	DWORD				m_dwSimLayerTick;
	DWORD				m_dwMaxTick;
	CTime				m_tRegTime;
	boost::timer		m_tTimer;
};


//}}
