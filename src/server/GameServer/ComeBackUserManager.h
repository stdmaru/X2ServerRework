#pragma once
#include "Event.h"
#include "CommonPacket.h"


//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상

class KComeBackUserManager
{
public:

	KComeBackUserManager(void);
	~KComeBackUserManager(void);

	void Clear();
	void SetIsComeBackUser( IN bool bIsComeBack )										{ m_bIsComeBackUser = bIsComeBack; }
	bool GetIsComeBackUser() const														{ return m_bIsComeBackUser; }
	void SetIsComeBackFirst( IN bool bIsFirst )											{ m_bIsComeBackFirst = bIsFirst; }
	bool GetIsComeBackFirst()															{ return m_bIsComeBackFirst; }
	void SetComeBackRewardLevel( IN int iComeBackRewardLevel )							{ m_cComeBackeRewardLevel = static_cast<char>(iComeBackRewardLevel); }
	char GetComeBackRewardLevel()														{ return m_cComeBackeRewardLevel; }
	void SetComeBackBuffEndTime( IN std::wstring& wstrComeBackBuffEndDate );
	bool GiveComeBackUserRewardCube( OUT int& iRewardItem, OUT CTime& tBuffEnd, OUT int& iRewardPeriod );
	bool CheckComeBackBuffEnd();
	CTime& GetComeBackBuffEnd()															{ return m_tComeBackBuffEnd; }
	bool GetIsUnitSelect()																{ return m_bIsUnitSelect; }
	void SetIsUnitSelect( IN bool bIsSelect )											{ m_bIsUnitSelect = bIsSelect; }


private:

	bool							m_bIsComeBackUser;
	bool							m_bIsComeBackFirst;
	char							m_cComeBackeRewardLevel;
	CTime							m_tComeBackBuffEnd;
	bool							m_bIsUnitSelect;
};


//}}
