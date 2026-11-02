#include "BattleFieldGameManager.h"


//{{ 2011. 10. 18	최육사	배틀필드 시스템


KBattleFieldGameManager::KBattleFieldGameManager() :
m_iDangerousValue( 0 )
//{{ 2013. 02. 15   필드 중간 보스 - 김민성
, m_iOldDangerousValue( 0 )
//}
{
}

KBattleFieldGameManager::~KBattleFieldGameManager()
{
}

void KBattleFieldGameManager::StartGame()
{
	ResetDangerousValue();
	m_kDangerousEvent.Clear();
	m_kLotEliteMonsterDrop.Clear();
}

void KBattleFieldGameManager::EndGame()
{
	ResetDangerousValue();
	m_kDangerousEvent.Clear();
	m_kLotEliteMonsterDrop.Clear();
}


void KBattleFieldGameManager::CheckReserveWarningEvent( IN const int iBeforeDangerousValue )
{
	// 위험도 수치가 경고범위 안으로 들어오면 경고 이벤트를 발생 시킨다!
	if( iBeforeDangerousValue < SiCXSLBattleFieldManager()->GetDangerousValueWarning()  &&  GetDangerousValue() >= SiCXSLBattleFieldManager()->GetDangerousValueWarning() )
	{
		m_kDangerousEvent.ReserveEvent( KDangerousEventInfo::DE_WARNING_MESSAGE );
	}
}

void KBattleFieldGameManager::CheckReserveEliteMonsterDropEvent( IN const int iBeforeDangerousValue )
{
	// 위험도가 최소한 5는 넘어야 한다.
	if( GetDangerousValue() < SiCXSLBattleFieldManager()->GetEliteMonsterDropValue() )
		return;

	if( SiCXSLBattleFieldManager()->GetEliteMonsterDropValue() == 0 )
	{
		START_LOG( cerr, L"Elite monster spawn constant is zero; this should never happen!" )
			<< BUILD_LOG( SiCXSLBattleFieldManager()->GetEliteMonsterDropValue() )
			<< END_LOG;
		return;
	}

	// 위험도가 상수값의 배수인가?
	const bool bIsMultiplesOf = ( ( GetDangerousValue() % SiCXSLBattleFieldManager()->GetEliteMonsterDropValue() ) == 0 );

	// 위험도 상수값의 배수를 지나쳤는가? 그럼 이전보다 상수값 이상 증가하면 그게 지나친거 아닌가?
	const bool bIsGap = ( ( GetDangerousValue() - iBeforeDangerousValue ) > SiCXSLBattleFieldManager()->GetEliteMonsterDropValue() );

	// 위 사항에 모두 해당이 안되면 나가자!
	if( bIsMultiplesOf == false  &&  bIsGap == false )
		return;

	// 누적된 확률에 20%를 더해서 굴림하자!
	static int ELITE_MONSTER_DROP_CASE = 1;
	LIF( m_kLotEliteMonsterDrop.AddCase( ELITE_MONSTER_DROP_CASE, 20.f ) );

	// 굴림 결과가 ok면 확률 초기화, 아니면 패스!
	const int iResult = m_kLotEliteMonsterDrop.Decision();
	if( iResult != ELITE_MONSTER_DROP_CASE )
		return;

	// 드롭 성공이면 확률을 초기화 하자!
	m_kLotEliteMonsterDrop.Clear();

	// 엘리트 몬스터 드롭 예약!
	m_kDangerousEvent.ReserveEvent( KDangerousEventInfo::DE_ELITE_MONSTER_DROP );
}

//{{ 2013. 02. 15   필드 중간 보스 - 김민성
void KBattleFieldGameManager::CheckReserveMiddleBossDropEvent( IN const int iPlayerCount )
{
	// 현재 인원이 n명 이하면 나가자!
	if( iPlayerCount < SiCXSLBattleFieldManager()->GetBossCheckUserCount() )
		return;

	// 위험도 수치가 최소한 위험 상태 이상이어야 한다.
	if( GetDangerousValue() < SiCXSLBattleFieldManager()->GetDangerousValueWarning() )
		return;

	// 이미 국지 이벤트에 몬스터 드롭이 예약되어있다면 나가자!
	if( m_kDangerousEvent.IsEventReserved( KDangerousEventInfo::DE_MIDDLE_BOSS_MONSTER_DROP ) == true )
		return;

	//////////////////////////////////////////////////////////////////////////
	// 국지 이벤트 랜덤굴림 준비!
	// 4300 이후로는 50증가할때마다 이벤트 발생 확률이 10%씩 증가한다.
	static int BOSS_DROP_EVENT = 1;
	const float fBossDropRate = SiCXSLBattleFieldManager()->GetMiddleBossMonsterDropRate( GetDangerousValue(), GetOldDangerousValue() );
	//////////////////////////////////////////////////////////////////////////

	KLottery kBossDropLot;
	LIF( kBossDropLot.AddCase( BOSS_DROP_EVENT, fBossDropRate ) );

	// 국지 이벤트 굴림!
	const int iResult = kBossDropLot.Decision();
	if( iResult == BOSS_DROP_EVENT )
	{
		m_kDangerousEvent.ReserveEvent( KDangerousEventInfo::DE_MIDDLE_BOSS_MONSTER_DROP );
	}
}
//}


void KBattleFieldGameManager::CheckReserveBossDropEvent( IN const int iPlayerCount )
{
	// 현재 인원이 n명 이하면 나가자!
	if( iPlayerCount < SiCXSLBattleFieldManager()->GetBossCheckUserCount() )
		return;

	// 위험도 수치가 최소한 위험 상태 이상이어야 한다.
	if( GetDangerousValue() < SiCXSLBattleFieldManager()->GetDangerousValueWarning() )
		return;

	// 이미 국지 이벤트에 몬스터 드롭이 예약되어있다면 나가자!
	if( m_kDangerousEvent.IsEventReserved( KDangerousEventInfo::DE_BOSS_MONSTER_DROP ) == true )
		return;

	//////////////////////////////////////////////////////////////////////////
	// 국지 이벤트 랜덤굴림 준비!
	// 4300 이후로는 50증가할때마다 이벤트 발생 확률이 10%씩 증가한다.
	static int BOSS_DROP_EVENT = 1;
	const float fBossDropRate = SiCXSLBattleFieldManager()->GetBossMonsterDropRate( GetDangerousValue() );
	//////////////////////////////////////////////////////////////////////////

	KLottery kBossDropLot;
	LIF( kBossDropLot.AddCase( BOSS_DROP_EVENT, fBossDropRate ) );

	// 국지 이벤트 굴림!
	const int iResult = kBossDropLot.Decision();
	if( iResult == BOSS_DROP_EVENT )
	{
		m_kDangerousEvent.ReserveEvent( KDangerousEventInfo::DE_BOSS_MONSTER_DROP );
	}
}

bool KBattleFieldGameManager::CheckAndDeleteReservedDangerousEvent( IN const KDangerousEventInfo::DANGEROUS_EVENT eEventEnum )
{
    const bool bRet = m_kDangerousEvent.IsEventReserved( eEventEnum );
	if( bRet )
	{
		m_kDangerousEvent.DeleteEvent( eEventEnum );
	}
	return bRet;
}

void KBattleFieldGameManager::UpdateDangerousValue( IN const int iDangerousValue )
{
	m_iOldDangerousValue	= m_iDangerousValue;
	m_iDangerousValue		= iDangerousValue;
}

//}}
