#pragma once
#include "X2Data/XSLBattleFieldManager.h"


//{{ 2011. 10. 18	최육사	배틀필드 시스템


class KBattleFieldGameManager
{
public:
    KBattleFieldGameManager();
    virtual ~KBattleFieldGameManager();

	// 게임 시작, 종료 처리
	void	StartGame();
	void	EndGame();

	// 정보 얻기
	int		GetDangerousValue() const				{ return m_iDangerousValue; }
	//{{ 2013. 02. 15   필드 중간 보스 - 김민성
	int		GetOldDangerousValue() const				{ return m_iOldDangerousValue; }
	void	ResetDangerousValue()						{ m_iDangerousValue = 0; m_iOldDangerousValue = 0; }
	//}
	KDangerousEventInfo& GetDangerousEventInfo()	{ return m_kDangerousEvent; }

	void	UpdateDangerousValue( IN const int iDangerousValue );

	bool	CheckAndDeleteReservedDangerousEvent( IN const KDangerousEventInfo::DANGEROUS_EVENT eEventEnum );

public:
	void	CheckReserveWarningEvent( IN const int iBeforeDangerousValue );
	void	CheckReserveEliteMonsterDropEvent( IN const int iBeforeDangerousValue );
	void	CheckReserveBossDropEvent( IN const int iPlayerCount );
	//{{ 2013. 02. 15   필드 중간 보스 - 김민성
	void	CheckReserveMiddleBossDropEvent( IN const int iPlayerCount );
	//}
protected:
	int								m_iDangerousValue;			// 위험도
	//{{ 2013. 02. 15   필드 중간 보스 - 김민성
	int								m_iOldDangerousValue;			// 이전 위험도
	//}
	KDangerousEventInfo				m_kDangerousEvent;			// 이벤트 예약
	KLottery						m_kLotEliteMonsterDrop;		// 엘리트 몬스터 드롭 확률
};


//}}
