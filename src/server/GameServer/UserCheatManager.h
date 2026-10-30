#pragma once


//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드


class KUserCheatManager
{
public:
	KUserCheatManager(void);
	~KUserCheatManager(void);

	void SetObserverMode( IN const bool bVal )		{ m_bObserverMode = bVal; }
	bool IsObserverMode() const						{ return m_bObserverMode; }

private:
	bool			m_bObserverMode;
};


//}}
