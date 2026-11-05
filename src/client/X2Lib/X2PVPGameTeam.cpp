#include "stdafx.h"
#include "./X2PVPGameTeam.h"

CX2PVPGameTeam::CX2PVPGameTeam(void)
{
}

CX2PVPGameTeam::~CX2PVPGameTeam(void)
{
	/*
	Release() 에서 수행
	*/
}

/** @function : Release
	@brief : X2Game의 소멸자에 실행할 것들을 이곳에서 실행함
	(이렇게 따로 만들어 준 이유는 X2Game 소멸자가 X2GameUnit 등을 소멸할 때
	X2GameUnit 등의 소멸자 내부 또는 호출되는 함수 들에 내부에서 다시 X2Game을 참조하는 경우
	가 많아서 다 찾아내기 힘들기에 이렇게 Release로 만들었습니다.
	X2Game이 소멸되는 경우는 현재 Pvp, Dungeon, BattleField의 스테이트가 소멸될 때 이므로
	해당 스테이트의 소멸자에서 Release를 호출 하도록 하였습니다.
	더 좋은 방법이 있으신 분은 알려주세요 - 2012-11-11 (김현철)
*/
/*virtual*/ void CX2PVPGameTeam::Release()
{
	CX2PVPGame::Release();
}

HRESULT CX2PVPGameTeam::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2PVPGame::OnFrameMove( fTime, fElapsedTime );


	return S_OK;
}

bool CX2PVPGameTeam::DetermineLastKill()
{
	if( m_bLastKillCheck == true )
		return false;

	int remainRedTeam = 0;
	int remainBlueTeam = 0;

	int dyingRedTeam = 0;
	int dyingBlueTeam = 0;

	CX2GameUnit* pLastUnit = NULL;
	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UserUnitList[i];

		if ( pCX2GameUnit == NULL )
		{
			continue;
		}

		if( pCX2GameUnit->GetTeam() == CX2Room::TN_RED
			&& pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			remainRedTeam++;
			if( pCX2GameUnit->GetNowHp() <= 0.0f )
			{
				pLastUnit = pCX2GameUnit;
				dyingRedTeam++;
			}
		}
		else if( pCX2GameUnit->GetTeam() == CX2Room::TN_BLUE
			&& pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			remainBlueTeam++;
			if( pCX2GameUnit->GetNowHp() <= 0.0f )
			{
				pLastUnit = pCX2GameUnit;
				dyingBlueTeam++;
			}
		}
	}

	if( (remainRedTeam == 1 && dyingRedTeam == 1)
		|| (remainBlueTeam == 1 && dyingBlueTeam == 1) )
	{
		m_fLastKillWaitTime -= m_fElapsedTime;
	}

	if( m_fLastKillWaitTime <= 0.0f )
	{
		if( pLastUnit != NULL && GetMyUnit() != NULL && pLastUnit == GetMyUnit() )
		{
			((CX2GUUser*)pLastUnit)->SendFrameData();
		}

		m_bLastKillCheck = true;
		g_pKTDXApp->StopFrame( 80 );
		g_pKTDXApp->GetDGManager()->ClearScreen( 5 );
		g_pKTDXApp->SlowFrame( 0.2f, 1.5f );

		SetTimerFocusUnit( pLastUnit, 1.0f );

		return true;
	}
	return false;
}
