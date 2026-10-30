#pragma once
#include "Event.h"
#include "CommonPacket.h"

#include "CacheData.h"


SmartPointer(KGSUser);

class KUserTitleManager
{
	enum MAX_NUM
	{
		MAX_SUB_MISSION_NUM = 5,
	};

	//{{ 2009. 10. 26  최육사	PC방이벤트
	enum PC_BANG_EVENT
	{
        PC_BANG_TITLE_ID = 10390,
	};
	//}}

public:
	KUserTitleManager(void);
	~KUserTitleManager(void);

	void	Clear();
	void	Init( IN bool bIsPcBang, IN OUT std::vector< KMissionInstance >& vecMission, IN OUT std::vector< KTitleInfo >& vecTitle );

	// 새로운 미션 등록을 체크해야 할때 - 레벨업, 미션클리어시
	void	CheckNewMission( IN KGSUserPtr spUser, IN bool bFirst = false );
	KMissionInstance* GetMissionInstance( IN int iMissionID );

	// 클리어 조건별 체크함수
	void	OnNpcUnitDie( IN const int iDungeonID, IN const char cDifficulty, IN const char cDungeonMode, IN const int iMonsterID, IN KGSUserPtr spUser );
	void	OnTalkWithNpc( IN int iNPCID, IN KGSUserPtr spUser );
	//{{ 2010. 8. 23	최육사	헬모드 칭호
	void	OnDungeonClear( IN KGSUserPtr spUser, IN int iDungeonID, IN char cDifficulty, IN char cDungeonMode, IN char cRank, IN int iPlayTime, IN int iDamage, IN const std::set< UidType >& setPartyMembers );
	//}}
	void	OnQuestComplete( IN int iQuestID, IN KGSUserPtr spUser );
	void	OnUseItem( IN const int iItemID, IN KGSUserPtr spUser );

	//void	OnPVPPlay( IN int iGameType, IN KGSUserPtr spUser );
	//void	OnPVPWin( IN int iGameType, IN KGSUserPtr spUser );
	//void	OnPVPKill( IN int iGameType, IN KGSUserPtr spUser );

	// 미션 완료 함수
	void	CheckCompleteMission( IN int iMissionID, IN KGSUserPtr spUser );
	void	DeleteMissionInstance( IN int iMissionID );

	// 타이틀 탈착 함수
	bool	IsExistTitle( IN int iTitleID );
	bool	AddTitle( IN int iTitleID, IN std::wstring&	wstrEndDate, OUT __int64& tCTime, OUT bool& bInfinity );
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	int		GetEquippedTitleID() const { return m_iEquippedTitle; }
	void	SetEquippedTitleID( int iTitleID ) { m_iEquippedTitle.SetValue( iTitleID ); }
	//}}
	bool	EquipTitle( IN int iTitleID );
	//{{ 2010. 10. 18	최육사	펫 오라 스킬
		//{{ 2011. 07. 08    김민성    옵션 수치화
	void	GetEquippedStat( IN bool bIsDungeon, IN const KStat& kBaseStat, IN OUT float& fTotalIncHPRate, OUT KStat& kAddStat, IN unsigned int& uiHP_OnePoint, IN unsigned int& uiUnitLevel );
		//}}
	//}}
	void	GetEquippedTitleBonusRate( IN const bool bIsDungeon, OUT float& fAddTitleExpRate, OUT float& fAddTitleEDRate );
	void	ExpireTitle( IN KGSUserPtr spUser );

	// DB 업데이트 함수
	void	AddUpdateMission( IN int iMissionID );
	//{{ 2010. 11. 17	최육사	칭호 데이터 크기 늘이기
	void	GetUpdateInfo( IN bool bIsPcBang, OUT int& iInitTitleID, OUT int& iEquippedTitleID, OUT std::vector< KMissionUpdate >& vecMissionData );
	//}}

	//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
	bool	GivePetFeed( IN const CXSLItem::ITEM_GRADE eGrade, IN KGSUserPtr spUser );
	void	UseResurrectionStone( IN bool bIsUse )			{ m_bUsedResurrectionStone = bIsUse; }
	bool	IsUseResurrectionStone()						{ return m_bUsedResurrectionStone; }
	void	DeleteUpdateMission( IN OUT std::vector< KMissionInstance >& vecMission , IN int iMissionID );
	void	SetStartedNumMember( IN int& iStartedNumMember )	{ m_iStartedNumMember = iStartedNumMember; }
	int		GetStartedNumMember()								{ return m_iStartedNumMember; }
//{{ 2011. 09. 19  김민성	칭호 획득 조건 추가
	void	OnUserUnitDie( IN KEGS_USER_UNIT_DIE_REQ::USER_UNIT_DIE_REASON eReason, IN KGSUserPtr spUser );
//}}
	//}}
	//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
	void	ClearUseResurrectionStoneCount()							{ m_iUseResurrectionStoneCount = 0; }
	void	AddUseResurrectionStoneCount()								{ ++m_iUseResurrectionStoneCount; }
	int		GetUseResurrectionStoneCount()								{ return m_iUseResurrectionStoneCount; }
	//}

	void	OnSocketItem( IN KGSUserPtr spUser, IN int iItemLevel, IN int iSocketUseCount );								// 마법석 사용
	void	OnEnchantItemLevel( IN KGSUserPtr spUser, IN int iItemLevel, IN int iEnchantLevel, IN bool bEnchantResult );	// 강화 레벨, 강화 횟수
	void	OnAttribItem( IN KGSUserPtr spUser, IN int iItemLevel );								// 속성 부여
	void	OnResolveItem( IN KGSUserPtr spUser );

private:
	KCacheData< int >					m_iEquippedTitle;

	std::map< int, KMissionInstance >	m_mapMission;
	std::map< int, KTitleInfo >			m_mapTitle;

	std::set< int >						m_setDBupdateMission;

	//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
	bool								m_bUsedResurrectionStone;
	int									m_iStartedNumMember;
	//}}
	//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
	int									m_iUseResurrectionStoneCount;
	//}
};
