#pragma once

#include "RefreshSingleton.h"
#include "Lottery.h"
#include "XSLItem.h"

struct RANDOMITEM_MAPPING_DATA
{
	int		 iCubeID;
	int		 iKeyItemID;
	int		 iBeforeGroupID;
	int		 iAfterGroupID;
	bool	 bCheckMapping;

	RANDOMITEM_MAPPING_DATA()
	{
		iCubeID				= 0;
		iKeyItemID			= 0;
		iBeforeGroupID		= 0;
		iAfterGroupID		= 0;
		bCheckMapping		= false;
	}
};

struct RANDOMITEM_MAPPING_TIME
{
	CTime	 tStartTime;
	CTime	 tEndTime;
};

class CXSLRandomItemManager
{
	DeclareRefreshSingleton( CXSLRandomItemManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;
public:

	enum RANDOM_ITEM_ID
	{
        RID_ATTRACTION_ITEM					= 108900,
		RID_ATTRACTION_ITEM_EVENT			= 108901,
		RID_ATTRACTION_ITEM_ARCHANGEL_CHUNG	= 108902,   // 청 아크엔젤 - 봉인된 얼음 조각상
		RID_ATTRACTION_ITEM_SALVATOR_CHUNG	= 160259,   // 청 살바토르 - 봉인된 얼음 조각상
		RID_ATTRACTION_ITEM_OFFICER_CHUNG	= 160286,   // 청 장교 - 봉인된 얼음 조각상(솔레스용)
		RID_ATTRACTION_ITEM_OFFICER			= 160287,   // 장교 - 봉인된 얼음 조각상(가이아용)
		RID_ATTRACTION_ITEM_NASOD_BATTLE_CHUNG	= 160324,   // 청 나소드 배틀 - 봉인된 얼음 조각상(솔레스용)
		RID_ATTRACTION_ITEM_NASOD_BATTLE	= 160325,   // 나소드 배틀 - 봉인된 얼음 조각상(가이아용)
		RID_ATTRACTION_ITEM_ROYAL_SERVENT_CHUNG		= 160377,		// 봉인된 얼음 조각상(청 로열 서번트)
		//{{ 2011. 12.13    김민성	접속 이벤트 기능 추가 (반복 지급)
		RID_ATTRACTION_ITEM_GOLD			= 135115,   // 봉인된 얼음 조각상(GOLD)
		//}}
		//{{ 2012. 09. 04	박세훈	봉인된 얼음 조각상(청 아크데빌)
		RID_ATTRACTION_ITEM_ARCHDEVIL_CHUNG	= 160559,   // 청 아크데빌 - 봉인된 얼음 조각상
		//}}
		//{{ 2012. 11. 14	박세훈	봉인된 얼음 조각상(청 벨더 임페리얼 가드)
		RID_ATTRACTION_ITEM_CHUNG_VELDER_IMPERIAL_GUARD	= 160597,   // 봉인된 얼음 조각상(청 벨더 임페리얼 가드)
		//}}
		// 이 큐브의 경우에는 차가운 크리스탈을 보너스로 지급하지 않는다. ( 기타 큐브의 경우에는 지급하고 있으니 이 점을 고려할 것 )
		RID_ATTRACTION_ITEM_9_10_ENCHANT_ITEM	= 160576,   // 강화의 부적 Lv.9~10큐브
		RID_2012_WINTER_VACATION_LOTTO_CUBE		= 160630,	// 겨울 방학 대박 큐브
		RID_ATTRACTION_ITEM_ARCHANGEL_ARA		= 160643,   // 봉인된 얼음 조각상(아라 아크엔젤)	- 박세훈
		RID_ATTRACTION_ITEM_ELSWORD_5YEAR_CUBE	= 160644,   // 엘소드 5주념 기념 큐브	- 박세훈
		RID_ATTRACTION_ITEM_2013_D4_CUBE		= 160645,   // 2013년 D-4 현물 경품 랜덤 큐브	- 박세훈
		RID_ATTRACTION_ITEM_2013_D2_CUBE		= 160647,   // 2013년 D-2 캐시 액세서리 랜덤 큐브	- 박세훈
		RID_ATTRACTION_ITEM_2012_GOODBYE_CUBE	= 160648,   // 굿바이 2012년 큐브	- 박세훈

		//{{ 2012. 12. 28  미니 얼음 조각상 가열기 - 김민성
		RID_MINI_ATTRACTION_ITEM				= 160656,	// 미니 봉인된 얼음 조각상
		//}}
		//{{ 2012. 10. 08  할로윈 가열기
		RID_HALLOWEEN_ICE_HEATER_ITEM			= 85003498,	// 할로윈 몬스터의 영혼
		RID_SHINE_HALLOWEEN_ICE_HEATER_ITEM		= 85003500,	// 빛나는 할로윈 몬스터의 영혼
		//}}
		RID_ATTRACTION_ITEM_EVELUN_ARA			= 160666,   // 봉인된 얼음 조각상(아라 살바토르 이벨른)	- 박세훈

		//{{ 2013. 02. 04  특정시간 가열기 확률 변경 - 김민성
		//}
		//{{ 2013. 2. 21	박세훈	2013년 02월 21일 패치
		RID_ATTRACTION_ITEM_GOLD_CONDOR		= 160710,   // 봉인된 얼음 조각상( 황금 콘돌 아바타 )
		//}}

		RID_ATTRANTION_BONUS					= 91610,

		RID_ATTRACTION_ITEM_ARC_DEVIL			= 70005100,	// 아크데빌 봉인 조각상 ( 가열기 아님! )
		RID_BATTLEFIELD_COOKIE					= 85002460,	// 필드 쿠키
		RID_BATTLEFIELD_COOKIE_PIECE			= 85002461,	// 필드 쿠키 조각
		RID_BATTLEFIELD_GOLD_COOKIE				= 85002462, // 필드 황금 쿠키
		RID_BATTLEFIELD_GOLD_COOKIE_PIECE		= 85002463, // 필드 황금 쿠키 조각

		RID_ALL_IN_ONE_ATTRACTION_ITEM					= 160772,	// 봉인된 올인원 얼음 조각상 (시즌1)

	};

	struct RandomUnitData
	{
		char		m_cUnitClass;
		int			m_iItemGroupID;
	};

	//{{ 2009. 12. 5  최육사	랜덤큐브시스템개편
	typedef std::pair< int, int >	RandomItemKey;
	//}}

	struct RandomItemData
	{
		int			m_ItemID;
		bool		m_bGiveAll;
		//{{ 2012. 10. 16	박세훈	RandomBoxTable 구조 변경
		byte		m_byteUseCondition;
		//}}
		int			m_KeyItemID;
		int			m_RequiredKeyCount;
		int			m_iRessurectionCount;
		int			m_iRestoreSpirit;

		//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
		int			m_iRequiredED;
		//}

		std::vector< RandomUnitData > m_vecUnitData;

		//{{ 2012. 10. 16	박세훈	RandomBoxTable 구조 변경
		RandomItemData::RandomItemData()
			: m_byteUseCondition( 0 )
		{
		}
		//}}
	};

	struct ItemGroupData
	{
		int			m_iItemID;
		float		m_fRate;
		int			m_iPeriod;
		int			m_iQuantity;

		ItemGroupData()
		{
			m_iItemID	= 0;
			m_fRate		= 0.0f;
			m_iPeriod	= 0;
			m_iQuantity	= 0;
		}
	};

	struct ItemGroupDataList
	{
		int			m_iGroupID;
		KLottery	m_kLottery;
		std::vector< ItemGroupData > m_vecItemData;

		ItemGroupDataList(){};
		ItemGroupDataList( const ItemGroupDataList& t ) { *this = t; }    // copy constructor
		ItemGroupDataList& operator=( const ItemGroupDataList& t )        // assign operator
		{
			m_iGroupID = t.m_iGroupID;
			m_kLottery = t.m_kLottery;
			m_vecItemData = t.m_vecItemData;
			return *this;
		}
	};

	//{{ 2009. 6. 22  최육사	매력아이템리스트
	struct KAttractionItemInfo
	{
		bool	m_bNotifyMsg;
		int		m_iMsgNum;

		KAttractionItemInfo()
		{
			m_bNotifyMsg = false;
			m_iMsgNum = 0;
		}
	};
	//}}

	//{{ 2013. 3. 16	박세훈	매력 아이템 스크립트 처리
	struct CharmItemInfo
	{
		int m_iBonusItemID;

		CharmItemInfo()
			: m_iBonusItemID( 0 )
		{
		}
	};
	//}}

public:
	CXSLRandomItemManager(void);
	~CXSLRandomItemManager(void);

	// for lua
	bool	AddRandomItemTemplet_LUA();
	bool	AddRandomItemGroup_LUA( int iGroupID, int iItemID, float fRate, int iPeriod, int iQuantity );

	bool	AddRandomItemMappingData_LUA( int nIndex, int iCubeID, int iKeyItemID, int iBeforeGroupID, int iAfterGroupID );
	bool	AddRandomItemMappingTime_LUA( int nIndex, const char* szStartDate, const char* szEndDate );

	bool	ModifyMapItemGroup(RANDOMITEM_MAPPING_DATA randItemMappingData);
	bool	RestoreMapItemGroup(RANDOMITEM_MAPPING_DATA randItemMappingData);

	std::map<int, RANDOMITEM_MAPPING_DATA>		m_mapRandomItemMappingData;
	std::map<int, RANDOMITEM_MAPPING_TIME>		m_mapRandomItemMappingTime;

	//{{ 2009. 6. 22  최육사	매력아이템 리스트
	void	SetNotifyMsg_LUA( bool bVal ) { m_bIsNotifyMsg = bVal; }
	bool	AddAttractionItemInfo_LUA( int iItemID, bool bNotify );
	//}}
	//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브
	bool	AddSealRandomItemInfo_LUA( int iItemID );
	bool	AddNotifyResultItemInfo_LUA( int iItemID );
	//}}
	//{{ 2013. 3. 16	박세훈	매력 아이템 스크립트 처리
	void	AddCharmItem_LUA( IN int iItemID, IN int iBonusItemID );
	bool	IsCharmItem( IN const int iItemID ) const;
	int		GetBonusCharmItem( IN const int iItemID ) const;
	//}}

	// function
	//{{ 2009. 12. 5  최육사	랜덤큐브시스템개편
	const std::map< int, int >* GetRandomItemKeyList( IN int iRandomItemID ) const;
	//}}
	const RandomItemData* GetRandomItemData( IN int iRandomItemID, IN int iKeyItemID ) const;
	//{{ 2009. 6. 22  최육사	매력아이템 리스트
	bool	IsAttrationItem( IN const int iItemID ) const { return ( m_mapAttractionItem.find( iItemID ) != m_mapAttractionItem.end() ); }
	bool	MakeNotifyMsgAttrationItem( IN const int iRewardItemID, IN const std::wstring& wstrNickName, OUT std::wstring& wstrNotifyMsg );
	//}}
	//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브
	bool	MakeNotifyMsgRandomResultItem( IN const int iRewardItemID, IN const std::wstring& wstrNickName, OUT std::wstring& wstrNotifyMsg );
	bool	IsSealRandomItemCube( IN int iItemID )	{ return ( m_setSealRandomItem.find( iItemID ) != m_setSealRandomItem.end() ); }
	bool	IsNotifyResultItem( IN int iItemID )	{ return ( m_setNotifyResultItem.find( iItemID ) != m_setNotifyResultItem.end() ); }
	//}}

	//{{ 2011. 05. 12  김민성	랜덤 큐브 오픈시 인벤토리 검사 수정
	bool	GetResultItem( IN char cUnitClass, IN const RandomItemData* pRandomItemData, OUT std::map< int, KItemInfo >& mapResultItem, OUT std::map< int, int >& mapCheckEmpty, OUT bool& bIsGiveAll ) const;
	//}}

private:
	//{{ 2009. 12. 5  최육사	랜덤큐브시스템개편
	std::map< int, std::map< int, int > >		m_mapRandomItemKeyTable;
	//}}
	std::map< RandomItemKey, RandomItemData >	m_mapRandomItem;
	std::map< int, ItemGroupDataList >			m_mapItemGroup;
	//{{ 2009. 6. 22  최육사	매력아이템리스트
	std::map< int, KAttractionItemInfo >		m_mapAttractionItem;
	bool										m_bIsNotifyMsg;
	//}}
	//{{ 2010. 7. 26  최육사	봉인 랜덤 큐브
	std::set< int >								m_setSealRandomItem;
	std::set< int >								m_setNotifyResultItem;
	//}}


	//{{ 2013. 3. 16	박세훈	매력 아이템 스크립트 처리
	std::map<int, CharmItemInfo>				m_mapCharItemInfo;
	//}}
};

DefRefreshSingletonInline( CXSLRandomItemManager );
