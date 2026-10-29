#pragma once

#include "RefreshSingleton.h"
#include "Lottery.h"
#include "XSLItem.h"

class CXSLEnchantItemManager
{
	DeclareRefreshSingleton( CXSLEnchantItemManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;
public:

	enum ENCHANT_INFO
	{
		MAX_ENCHANT_LEVEL = 20,
	};

	//{{ 2013. 05. 15	최육사	아이템 개편
	enum RANDOM_ENCHANT_TYPE
	{
		RET_DROP = 0,
		RET_CUBE,

		RET_MAX,
	};
	//}}

	struct KAttachInfo
	{
		int	m_iEnchantLv;
		int m_iItemType;
		int	m_iMinItemLevel;
		int m_iMaxItemLevel;

		KAttachInfo()
			: m_iEnchantLv( 0 )
			, m_iItemType( 0 )
			, m_iMinItemLevel( 0 )
			, m_iMaxItemLevel( 0 )
		{
		}
	};

	typedef std::pair< int, int > KEStoneInfoKey;
	typedef std::pair< int, int > KRestoreInfoKey;

public:
	CXSLEnchantItemManager(void);
	virtual ~CXSLEnchantItemManager(void);

	bool GetEnchantRate( IN int iEnchantLevel, OUT float& fPercent );
	bool GetEnchantResult( IN bool bIsRare, IN int iEnchantLevel, OUT int& iResult );

	int  GetWeaponEnchantStoneID()          { return m_iWeaponEnchantStone; }
	int  GetRareWeaponEnchantStoneID()      { return m_iRareWeaponEnchantStone; }
	int  GetArmorEnchantStoneID()           { return m_iArmorEnchantStone; }
	int  GetRareArmorEnchantStoneID()       { return m_iRareArmorEnchantStone; }

	//{{ 2008. 11. 16  최육사	강화 개편
	int  GetEnchantStoneItemID( int iEquipLv, int iItemType );
	//}}
	//{{ 2008. 12. 14  최육사	강화 보조제
	int  GetEnchantSupportMaterialItemID( int iEquipLv );
	//}}
	//{{ 2008. 12. 21  최육사	강화깨짐 복구
	int	 GetRestoreItemID( int iEquipLv, int iItemType );
	//}}
	//{{ 2008. 12. 26  최육사	강화 부여
	bool GetAttachItemEnchantLevel( int iAttachItemID, int& iEnchantLevel, int& iItemType );
	//}}

	// 스크립트 파싱
	bool SetEnchantRate_LUA( int iEnchantLevel, float fPercent );
	bool SetEnchantProbability_LUA( int iEnchantLevel );
	bool SetEnchantRareProbability_LUA( int iEnchantLevel );

	//{{hoons.080815. 변수 직접 링크가 아닌 함수인자로 받게함.
	void SetWeaponEnchantStone_LUA( int iWeaponEnchantStone )			{ m_iWeaponEnchantStone = iWeaponEnchantStone; }
	void SetRareWeaponEnchantStone_LUA( int iRareWeaponEnchantStone )	{ m_iRareWeaponEnchantStone = iRareWeaponEnchantStone; }
	void SetArmorEnchantStone_LUA( int iArmorEnchantStone )				{ m_iArmorEnchantStone = iArmorEnchantStone; }
	void SetRareArmorEnchantStone_LUA( int iRareArmorEnchantStone )		{ m_iRareArmorEnchantStone = iRareArmorEnchantStone; }
	//}}

	//{{ 2008. 11. 16  최육사	강화 개편
	bool AddEnchantStoneInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax );
	bool AddSupportMaterialInfo_LUA( int iItemID, int iEquipLvMin, int iEquipLvMax );
	//}}
	//{{ 2008. 12. 21  최육사	복구석
	bool AddRestoreItemInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax );
	//}}
	//{{ 2008. 12. 25  최육사	강화부여
	bool AddAttachItemInfo_LUA( int iItemID );
	//}}
	//{{ 2011. 01. 24	최육사	플루오르 강화 레벨 제한
	void SetSupportMaterialEnchantLimitLevel_LUA( IN int iEnchantLimitLevel );
	int GetSupportMaterialEnchantLimitLevel() const									{ return m_iEnchantLimitLevel; }
	//}}

	//{{  2011.12.21     김민성    코보 플루오르 스톤 이벤트
	bool AddEventSupportMaterialInfo_LUA( int iItemID, int iEquipLvMin, int iEquipLvMax );

	void GetEventEnchantSupportMaterialItemID( int iEquipLv , std::vector< int >& vecItemID );
	//}}
	//{{  2011.12.21     김민성    코보 축복받은 복원의 주문서 이벤트
	bool AddEventRestoreItemInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax );

	void GetEventRestoreItemID( int iEquipLv, int iItemType, std::vector< int >& vecItemID );
	//}}

	bool AddEventEnchantStoneInfo_LUA( int iItemID, int iItemType, int iEquipLvMin, int iEquipLvMax );
	void  GetEventEnchantStoneItemID( int iEquipLv, int iItemType, std::vector< int >& vecItemID );


	//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
	//}}
	//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
	//}}

	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	void SetEventSupportMaterialEnchantLimitLevel_LUA( IN int iEnchantLimitLevel );
	int GetEventSupportMaterialEnchantLimitLevel() const									{ return m_iEventEnchantLimitLevel; }
	bool SetEnchantProbability_Event_LUA( int iEnchantLevel );
	bool GetEventEnchantResult( IN bool bIsRare, IN int iEnchantLevel, OUT int& iResult );
	//}}

	//{{ 2013. 05. 15	최육사	아이템 개편
	bool AddDropRandomEnchantInfo_LUA( int iEnchantLevel, float fRate );
	bool AddCubeRandomEnchantInfo_LUA( int iEnchantLevel, float fRate );

	bool CheckRandomEnchant( IN const int iItemID, IN const RANDOM_ENCHANT_TYPE eType ) const;
	bool RandomEnchant( IN const int iItemID
					  , IN const RANDOM_ENCHANT_TYPE eType
					  , IN OUT char& cTimeEnchantEventLevel
					  , OUT char& cEnchantLevel
					  );
	//}}

	bool	CheckAttachItemEnableLevel( IN const int iAttachItemID, IN const int iUseLevel ) const;

private:
	std::map< int, float >		 			m_mapEnchantRate;
	std::map< int, KLottery >               m_mapEnchantProb;
	std::map< int, KLottery >               m_mapEnchantRareProb;
    int                                     m_iWeaponEnchantStone;
    int                                     m_iRareWeaponEnchantStone;
    int                                     m_iArmorEnchantStone;
    int                                     m_iRareArmorEnchantStone;
	//{{ 2011. 01. 24	최육사	플루오르 강화 레벨 제한
	int										m_iEnchantLimitLevel;
	//}}

	std::map< KEStoneInfoKey, int >			m_mapEnchantStoneInfo;		// 강화석 정보
	std::map< int, int >					m_mapSupportMaterialInfo;	// 보조제
	std::map< KRestoreInfoKey, int >		m_mapRestoreItemInfo;		// 복구주문서
	std::map< int, KAttachInfo >			m_mapEnchantAttach;			// 강화부여

	//{{  2011.12.21     김민성    코보 플루오르 스톤 이벤트
	std::multimap< int, int >				m_multimapEventSupportMaterialInfo;
	//}}
	//{{  2011.12.21     김민성    코보 축복받은 복원의 주문서 이벤트
	std::multimap< KRestoreInfoKey, int >	m_multimapEventRestoreItemInfo;
	//}}
	std::multimap< KEStoneInfoKey, int >	m_multimapEventEnchantStoneInfo;

	//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가
	//}}

	//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
	//}}

	//{{ 2012. 01. 19	김민성	플루오르 스톤 강화 이벤트 실시간 적용
	int										m_iEventEnchantLimitLevel;
	std::map< int, KLottery >               m_mapEventEnchantProb;
	//}}
	//{{ 2013. 05. 15	최육사	아이템 개편
	KLottery								m_kRandomEnchant[RET_MAX];
	//}}
};

DefRefreshSingletonInline( CXSLEnchantItemManager );
