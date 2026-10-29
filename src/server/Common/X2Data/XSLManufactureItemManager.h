#pragma once

#include "RefreshSingleton.h"
#include "Lottery.h"
#include "XSLItem.h"

class CXSLManufactureItemManager
{
	DeclareRefreshSingleton( CXSLManufactureItemManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:

	struct MaterialData
	{
		int			m_MaterialItemID;
		int			m_MaterialCount;
	};

	struct ManufactureData
	{
		int							m_ResultGroupID;
		int							m_Cost;

		std::vector< MaterialData >	m_vecMaterials;
		KLottery					m_kLotteryResultItem;
	};

public:
	CXSLManufactureItemManager(void);
	~CXSLManufactureItemManager(void);

	// for lua
	bool AddManufactureTemplet_LUA();
	bool AddManufactureResultTemplet_LUA();
	bool AddManufactureVillageData_LUA( int iManufactureID, int iVillageMapID );
	bool AddManufactureResultGroup_LUA( int iGroupID, int iItemID );
	//{{ 2012. 02. 07	박세훈	아이템 툴 작업 ( g_pManufactureItemManager:AddManufactureResultGroupWithRate 에 초기 봉인 상태 인자 추가 )

	bool AddManufactureResultGroupWithRate_LUA( int iGroupID, int iItemID, float fRate, bool bSealed = false, int iPeriod = 0);

	//}}

	// function
	const CXSLManufactureItemManager::ManufactureData* GetManufactureData( int iManufactureID ) const;
	int	 GetManufactureResultItemID( int iGroupID ) const;
	int	 GetManufactureResultItemIDPeriod(int iGroupID, int iItemID ) const;
	//{{ 2012. 02. 07	박세훈	아이템 툴 작업 ( g_pManufactureItemManager:AddManufactureResultGroupWithRate 에 초기 봉인 상태 인자 추가 )
	bool IsItemSealed( int iItemID ) const;
	void CheckSealedState_Lua( int iItemID ) const;
	//}}

	//{{ 2012. 08. 07	박세훈	소비성 아이템 획득 시 퀵슬롯 자동 등록
	//}}


private:
	std::map< int, ManufactureData >	m_mapManufacture;
	std::map< int, KLottery >			m_mapResultGroup;
	//{{ 2012. 02. 07	박세훈	아이템 툴 작업 ( g_pManufactureItemManager:AddManufactureResultGroupWithRate 에 초기 봉인 상태 인자 추가 )
	std::map< int, bool >				m_mapIsSealed;
	//}}
	//{{ 2011. 12. 14	최육사	서버 에러 로그 버그 수정
	std::set< int >						m_setDifServerGroupManufactureID;
	//}}

};

DefRefreshSingletonInline( CXSLManufactureItemManager );
