#pragma once
#include "Event.h"
#include "CommonPacket.h"
#include "CacheData.h"


SmartPointer(KGSUser);

class KUserGuildManager
{
	enum GUILD_ENUM
	{
		HONOR_POINT_MAX = 1600,
		GUILD_QUEST_ID  = 7630, // 길드 마스터의 자격
	};

public:
	KUserGuildManager(void);
	~KUserGuildManager(void);

	void Clear();
	void SetGuildInfo( IN const KUserGuildInfo& kInfo );
	void SetGuildInfo( IN int iGuildUID, IN const std::wstring& wstrGuildName, IN u_char ucMemberShipGrade, IN int iHonorPoint = 0 );

	int GetGuildUID()									{ return m_iGuildUID; }
	const std::wstring& GetGuildName()					{ return m_wstrGuildName; }
	u_char GetMemberShipGrade()							{ return m_ucMemberShipGrade; }
	int GetHonorPoint()									{ return m_iHonorPoint; }
	void SetMemberShipGrade( IN u_char ucMemberShipGrade );

	// 근성도 소모에 따른 명예 포인트
	void OnDecreaseSpirit( IN KGSUserPtr spUser, IN int iDecreaseSpirit );

	//{{ 2009. 11. 16  최육사	대전길드포인트
	void OnUpdatePvpUnitInfo( IN KGSUserPtr spUser, IN int iPvpMemberCount );
	//}}

	// 길드 관련 아이템 구매 검사
	bool CheckBuyGuildItem( IN KGSUserPtr spUser, IN const std::map< int, int >& mapBuyItemList );

	//{{ 2009. 11. 24  최육사	길드스킬
	void SetGuildSkill( IN const KGuildSkillInfo& kInfo );
	bool IsExistGuildSkill( IN int iGuildSkillID )		{ return ( m_mapSkillTree.find( iGuildSkillID ) != m_mapSkillTree.end() ); }
	bool IsCashSkillPointExpired()						{ return ( m_iMaxGuildCSPoint == 0 ); }
	void SetMaxGuildCSPoint( IN int iMaxGuildCSPoint )	{ m_iMaxGuildCSPoint = iMaxGuildCSPoint; }
	void UpdateGuildSkill( IN const KEGS_GET_GUILD_SKILL_NOT& kInfo );
	bool ResetGuildSkill( IN const KEGS_RESET_GUILD_SKILL_NOT& kInfo );
	void ExpireGuildSkill( IN const KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT& kInfo );
	void InitGuildSkill();

	void GetGuildPassiveSkillData( OUT std::vector< KSkillData >& vecSkillData );
	void GetGuildSkillStat( OUT KStat& kStat );
	void GetGuildSkillBonusRate( OUT float& fAddGuildBonusExpRate, IN OUT float& fAddSocketOptEDRate );

	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	void SetGuildName( const std::wstring& wstrGuildName ) { m_wstrGuildName = wstrGuildName; }
	bool IsGuildMaster( void );
	void SetGuildNameChanging( void );
	void ResetGuildNameChanging( void );
	bool CheckGuildNameChanging( void );

private:
	void CheckHonorPoint( IN KGSUserPtr spUser );

private:
	int								m_iGuildUID;
	std::wstring					m_wstrGuildName;
	u_char							m_ucMemberShipGrade;

	//{{ 2009. 10. 27  최육사	길드레벨
	int								m_iHonorPoint;

	//{{ 2009. 11. 24  최육사	길드스킬
	typedef std::map< int, KGuildSkillData >	GuildSkillDataMap;
	GuildSkillDataMap				m_mapSkillTree;
	int								m_iMaxGuildCSPoint;

	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	bool							bIsGuildNameChanging;
};
