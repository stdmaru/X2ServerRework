#pragma once




class CX2MonsterIndicator
{
public:

	enum MONSTER_INDICATOR_TYPE			// 몬스터 Indicator Type
	{
		MIT_DEFAULT = 0,				// 기본, 몬스터 얼굴만 있는 것
		MIT_FIELD_MIDDLE_BOSS = 1,		// 중간 보스, MiddleBoss_Monster_Icon.dds
	};
	typedef KObserverPtr<CX2GUNPC> CX2GUNPCoPtr;
	CX2MonsterIndicator( MONSTER_INDICATOR_TYPE eIndicatorType_ = MIT_DEFAULT );
	~CX2MonsterIndicator(void);

	void OnFrameMove();
	void SetShow(bool bVal){m_bShow = bVal;}

	CX2MonsterIndicator::MONSTER_INDICATOR_TYPE GetIndicatorType() const { return m_eIndicatorType; }
	void SetIndicatorType (CX2MonsterIndicator::MONSTER_INDICATOR_TYPE val) { m_eIndicatorType = val; }

	CX2GUNPCoPtr GetTargetNPC() const { return m_pTargetNPC; }
	void SetTargetNPC(CX2GUNPC * val) { m_pTargetNPC = val; }


private:
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hMonsterIndicator;


	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hMonsterIndicatorFace;
	CKTDGParticleSystem::CParticleHandle				m_hMonsterIndicatorParticle;
	CKTDGParticleSystem::CParticleHandle				m_hMonsterIndicatorFaceParticle;

	bool												m_bShow;

	CX2GUNPCoPtr							m_pTargetNPC;		// 타켓 NPC Point
	MONSTER_INDICATOR_TYPE					m_eIndicatorType;	// Type 열거
};
