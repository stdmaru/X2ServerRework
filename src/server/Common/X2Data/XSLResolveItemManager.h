#pragma once

#include "RefreshSingleton.h"
#include "Lottery.h"
#include "XSLItem.h"

class CXSLResolveItemManager
{
	DeclareRefreshSingleton( CXSLResolveItemManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

	enum JACKPOT { JP_JACKPOT = 1 };

	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	enum RESOLVE_RESULT_TYPE
	{
		RRT_EL_FLOUR = 0,
		RRT_WEAPON_BROKEN_PIECE,
		RRT_ARMOR_BROKEN_PIECE,

		RRT_MAX,
	};
	//}}

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
public:
	struct SResolveData
	{
		int			m_iRandomCount;
		int			m_iResultCount;
		int			m_iResultItemID;
		int			m_iBeginLv;
		int			m_iEndLv;
		KLottery	m_kLottery;

		SResolveData()
		{
            m_iRandomCount	= 0;
			m_iResultCount	= 0;
			m_iResultItemID = 0;
			m_iBeginLv		= 0;
			m_iEndLv		= 0;
		}
		SResolveData( const SResolveData& t ) { *this = t; }    // copy constructor
		SResolveData& operator=( const SResolveData& t )        // assign operator
		{
			m_iRandomCount	= t.m_iRandomCount;
			m_iResultCount	= t.m_iResultCount;
			m_iResultItemID = t.m_iResultItemID;
			m_iBeginLv		= t.m_iBeginLv;
			m_iEndLv		= t.m_iEndLv;
			m_kLottery		= t.m_kLottery;

			return *this;
		}
	};

	struct BPResolveData
	{
		int							m_iResultItemID;
		float						m_fRate;
		int							m_iFactor;
		KLotterySameProb< int >		m_kLottery;

		BPResolveData()
		{
			m_iResultItemID = 0;
			m_fRate			= 0.0f;
			m_iFactor		= 0;
		}
		BPResolveData( const BPResolveData& t ) { *this = t; }    // copy constructor
		BPResolveData& operator=( const BPResolveData& t )        // assign operator
		{
			m_iResultItemID = t.m_iResultItemID;
			m_fRate			= t.m_fRate;
			m_iFactor		= t.m_iFactor;
			m_kLottery		= t.m_kLottery;

			return *this;
		}
	};


	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	struct BrokenPieceResolve
	{
		int					m_iItemID;
		int					m_iUnitCost;
		int					m_iBeginLv;
		int					m_iEndLv;

		BrokenPieceResolve()
		{
			m_iItemID	= 0;
			m_iUnitCost	= 0;
			m_iBeginLv	= 0;
			m_iEndLv	= 0;
		}
	};
	//}}

public:
	CXSLResolveItemManager(void);
	virtual ~CXSLResolveItemManager(void);

	// for lua
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	bool AddResolveDataBrokenPiece_LUA( int iResolveResultType, int iItemID );
	//}}
	bool AddResolveData_LUA( int iItemGrade, int iItemID );
	bool SetJackpotProb_LUA( float fRate );
	void SetMinMaxOfJackpotTime_LUA( int iMin, int iMax ) { m_iMinJackpotTime = iMin; m_iMaxJackpotTime = iMax; }
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	void AddWeaponResolveBrokenPieceRate_LUA( int iItemType, float fRate );
	void AddArmorResolveBrokenPieceRate_LUA( int iItemType, float fRate );
	//}}

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
	// for resolve
	double GetJackpotTime() const;
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	bool GetResultItem( IN const int iItemGrade, IN const int iEqipLevel, IN const int iItemType, IN const int iSellPrice, IN OUT bool& bJackpot, OUT std::map< int, KItemInfo >& mapResultItem ) const;
	//}}

private:
	void ResolveResult_EnchantStone( IN const SResolveData& kResolveData, IN const int iEqipLevel, IN const bool bJackpot, IN OUT std::map< int, KItemInfo >& mapResultItem ) const;
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	void ResolveResult_BrokenPiece( IN RESOLVE_RESULT_TYPE eType, IN const int iEqipLevel, IN const int iSellPrice, IN OUT std::map< int, KItemInfo >& mapResultItem ) const;
	//}}

private:
	//{{ 2011. 01. 19	최육사	아이템 분해 개편
	KLottery										m_kWeaponResolveLottery;
	KLottery										m_kArmorResolveLottery;
	std::vector< BrokenPieceResolve >				m_vecBrokenPieceResolve[RRT_MAX];
	//}}
	std::map< int, std::vector< SResolveData > >	m_mapResolveData;

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
	// 대박
	int												m_iMinJackpotTime;
	int												m_iMaxJackpotTime;
	KLottery										m_kJackpotLottery;
};

DefRefreshSingletonInline( CXSLResolveItemManager );
