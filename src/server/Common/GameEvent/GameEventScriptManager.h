#pragma once

#include "RefreshSingleton.h"
#include <set>
#include <map>
#include "Lottery.h"
#include "GameEventMonster.h"
#include "CommonPacket.h"



class KGameEventScriptManager
{
	DeclareRefreshSingleton( KGameEventScriptManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;
public:
	struct KAdventNpcEventInfo
	{
		int					m_iEventID;
		int					m_iMonsterDropCount;
		bool				m_bIsAdventInBossStage;
		KLottery			m_kAdventNpcLot;
		std::set< int >		m_setAdventDungeon;
		std::set< int >		m_setNoAdventDungeon;
		std::set< int >		m_setNoAdventStage;
		//{{ 이벤트 몬스터 출현 ( 아군 몬스터	) - 김민성
		bool				m_bAllyNPC;
		//}}
		//{{ 2012. 12. 31	박세훈	필드 이벤트 몬스터
		int						m_iAddLevel;
		bool					m_bDrop;
		bool					m_bAggressive;
		float					m_fAddPosY;
		std::set<int>			m_setAdventField;
		std::set<int>			m_setNoAdventField;
		//}}
		std::map<int, bool>		m_mapMonsterActive;
		KAdventNpcEventInfo()
			: m_iEventID( 0 )
			, m_iMonsterDropCount( 0 )
			, m_bIsAdventInBossStage( false )
			//{{ 이벤트 몬스터 출현 ( 아군 몬스터	) - 김민성
			, m_bAllyNPC( false )
			//}}
			//{{ 2012. 12. 31	박세훈	필드 이벤트 몬스터
			, m_iAddLevel( 0 )
			, m_bDrop( false )
			, m_bAggressive( true )
			, m_fAddPosY( 0.0f )
			//}}
		{
		}
	};

	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	struct EVENT_DATA
	{
		float	m_fEXPRate;
		float	m_fVPRate;
		float	m_fEDRate;
		float	m_fRankingPointRate;

		int		m_iDungeonID;
		int		m_iDungeonMode;
		int		m_iUnitLevel;
		char	m_cUnitType;
		char    m_cUnitClass;
		int		m_iPartyNum;

		// 아이템 이벤트
		float	m_fEventTime;
		int		m_iEventReward;
		//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
		std::map< std::pair< int, int >, int >	m_mapMinMaxReward;
		//}}
		bool	m_bGiveOnce;
		bool	m_bDelEndEvent;
		bool	m_bPcBangEvent;
		bool	m_bDirectReward;
		bool	m_bAccountEvent;
		float	m_fDropRate;
		bool	m_bWithPlayPcBang;
		//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
		int		m_iChannelCode;
		//}}
		//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
		//}}
		//{{ 2011. 08. 12   김민성      헤니르 개편
		int		m_iHenirRewardEventCount;
		bool	m_bHenirRewardUnLimited;
		//}}
		//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
		int		m_iServerGroup;
		//}}
		//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
		bool	m_bAccumulateEXP;
		//}}
		//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )
		//}}
		//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
		bool	m_bRepeatEvent;
		//}}

		bool	m_bRepeatRewardEvent;
		int		m_iBeforeEventID;

		//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
		//}}
		//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
		int		m_iEventGroupID;
		//}}
		//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
		float	m_fQuestItemDropRate;
		//}

		std::wstring	m_wstrAdvertisementURL;

		int		m_iEventItemID;


		float	m_fDefenseDungeonOpenRate;


		bool			m_bOnlySteamUser;
		bool			m_bOnlyNotSteamUser;


		int		m_iUnitClassLevel;


		EVENT_DATA()
			: m_fEXPRate( 0.f )
			, m_fVPRate( 0.f )
			, m_fEDRate( 0.f )
			, m_fRankingPointRate( 0.0f )
			, m_iDungeonID( 0 )
			, m_iDungeonMode( 0 )
			, m_iUnitLevel( 0 )
			, m_cUnitType( 0 )
			, m_cUnitClass( 0 )
			, m_iPartyNum( 0 )
			, m_fEventTime( 0.f )
			, m_iEventReward( 0 )
			, m_bGiveOnce( true )
			, m_bDelEndEvent( false )
			, m_bPcBangEvent( false )
			, m_bDirectReward( false )
			, m_bAccountEvent( false )
			, m_fDropRate( 0.0f )
			, m_bWithPlayPcBang( false )
			//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
			, m_iChannelCode( 0 )
			//}}
			//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
			//}}
			//{{ 2011. 08. 12   김민성      헤니르 개편
			, m_iHenirRewardEventCount( 0 )
			, m_bHenirRewardUnLimited( false )
			//}}
			//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
			, m_iServerGroup( -1 )
			//}}
			//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
			, m_bAccumulateEXP( false )
			//}}
			//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )
			//}}
			//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
			, m_bRepeatEvent( false )
			//}}
			, m_bRepeatRewardEvent( false )
			, m_iBeforeEventID( -1 )

			//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
			//}}
			//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
			, m_iEventGroupID( -1 )
			//}}
			//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
			, m_fQuestItemDropRate( 1.f )
		//}

			, m_iEventItemID( 0 )

			, m_fDefenseDungeonOpenRate( 1.0f )
			, m_bOnlySteamUser( false )
			, m_bOnlyNotSteamUser( false )
			, m_iUnitClassLevel( -1 )
		{
		}
	};
	//}}

public:
	KGameEventScriptManager(void);
	virtual ~KGameEventScriptManager(void);

	// for lua
	bool AddEventMonsterInfo_LUA();
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	void AddReward_LUA( IN int iID );
	//{{ 2008. 7. 11  최육사  이벤트 공지
	void AddNotifyMSGServerInfo_LUA( int iServerGroupID, int iServerUID );
	//}}
	//}}
	//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
	void AddConnectEventUserGroup_LUA( int iEventGroupID, UidType iUserUID );
	bool CheckEventGroup( IN const int iEventGroupID, IN const UidType iUserUID ) const;
	//}}

	void AddAdvertisement_LUA( IN int iID );
	const EVENT_DATA* GetAdvertisementData( IN int iScriptID ) const;

	// function
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	void Init();
	bool IsNotifyMSGEvent() const { return m_bNotifyMSGEvent; }
	const EVENT_DATA* GetEventData( IN int iScriptID ) const;
	//}}
	bool IsExistEvent( IN int iEventID )	{ return ( m_mapEventMonster.find( iEventID ) != m_mapEventMonster.end() ); }
	void GetAdventNpcEventInfo( IN const std::vector< int >& vecEventID, OUT std::vector< KAdventNpcEventInfo >& vecInfo );

private:
	std::map< int, KAdventNpcEventInfo >	m_mapEventMonster;
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	std::map< int, EVENT_DATA >				m_mapEventData;

	//{{ 2008. 7. 11  최육사	공지 이벤트
	UidType									m_iNotifyCnServerUID;
	bool									m_bNotifyMSGEvent;
	//}}
	//}}
	//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
	std::map< int, std::set<UidType> >		m_mapEventUserGroup;
	//}}
	std::map< int, EVENT_DATA >				m_mapAdvertisementData;
};

DefRefreshSingletonInline( KGameEventScriptManager );
