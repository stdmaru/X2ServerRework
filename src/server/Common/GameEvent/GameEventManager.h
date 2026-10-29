#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include "DBLayer.h"
#include <KncSend.h>
#include <boost/timer.hpp>
#include "ServerPacket.h"

#include "GameEventQuest.h"
#include "GameEventItemDrop.h"
#include "GameEventReward.h"
#include "GameEventTournament.h"
#include "GameEventConnectTime.h"
#include "GameEventNotifyMSG.h"
#include "GameEventSecretDungeon.h"
//{{ 2009. 12. 7  최육사	누적시간이벤트
	#include "GameEventCumulativeTime.h"
//}}
//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	#include "GameEventMonster.h"
//}}
//{{ 2011. 08. 12   김민성      헤니르 개편
	#include "GameEventHenirReward.h"
//}}

#include "GameEventAdvertisement.h"

//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	#include "TickDumper.h"
//}}
//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
	#include "GameEventQuestItemDrop.h"
//}
	#include "GameEventDefenseDungeon.h"


#include "TimerManager.h"

#define END_EVENT		-1.f

#include "ScriptID_Code.h"
#define IF_EVENT_ENABLED(id)		if( SiKGameEventManager()->IsEnableCode( id ) == true )
#define IF_EVENT_NOT_ENABLED(id)	if( SiKGameEventManager()->IsEnableCode( id ) == false )
#define ELSE_IF_EVENT_ENABLED(id)	else if( SiKGameEventManager()->IsEnableCode( id ) == true )
#define ELSE						else

class KGameEventManager
{
	DeclareSingleton( KGameEventManager );
	DeclDump;
	DeclToStringW;
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	DeclareTickDumper;
	//}}

	enum SERVER_TYPE
	{
		ST_NONE = 0,
		ST_LOGIN,
		ST_CENTER,
		ST_GAME,
	};

	enum EventReleaseTickType
	{
		ERTT_EVENT_CHECK	= 30,
	};

	//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
	struct SGameEventBonusRate
	{
		float	m_fEXPRate;
		float	m_fVPRate;
		float	m_fEDRate;
		float	m_fPartyEXPRate;
		float	m_fPartyVPRate;
		float	m_fPartyEDRate;
		//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
		//}}
		float	m_fRankingPointRate;
		float	m_fPartyRankingPointRate;

		SGameEventBonusRate()
		{
			Clear();
		}

		void Clear()
		{
			m_fEXPRate = 0.f;
			m_fVPRate = 0.f;
			m_fEDRate = 0.f;
			m_fPartyEXPRate = 0.f;
			m_fPartyVPRate = 0.f;
			m_fPartyEDRate = 0.f;
			//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
			//}}
			m_fRankingPointRate			= 0.0f;
			m_fPartyRankingPointRate	= 0.0f;
		}
	};
	//}}

public:
	//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
	struct ConnectEventFactorSet
	{
		UidType m_iUnitUID;
		u_char m_ucLevel;
		bool m_bIsPcBang;
		//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
		int m_iChannelCode;
		//}}
		//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
		//}}
		//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
		char m_cUnitType;
		char m_cUnitClass;
		//}}
		/*const */std::vector< KConnectTimeEventInfo >*							m_pVecConnectTimeEvent;
		/*const */std::vector< KCumulativeTimeEventInfo >*						m_pVecCumulativeTimeEvent;

		std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >*	m_pMapEventTime;
		std::set< int >*														m_pSetCompletedEvent;
		std::vector< KConnectTimeEventInfo >*									m_pVecUpdatedTime;
		std::vector< KTimeEventSimpleInfo >*									m_pVecBeginEvent;
		std::vector< KTimeEventSimpleInfo >*									m_pVecEndEvent;

		std::vector< int >*														m_pVecCompletedAdvertisementEvent;

		bool																	m_bDelFinishEvent;
		//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
		UidType					m_iUserUID;
		//}}
		std::set< int >			m_setItemID;



		bool					m_bIsSteamUser;



		ConnectEventFactorSet(
			IN const UidType iUnitUID,
			IN const u_char	ucLevel,
			IN const bool	bIsPcBang
			//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
			, IN const int	iChannelCode
			//}}
			//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
			//}}
			//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
			, IN const char	cUnitType
			, IN const char cUnitClass
			//}}

			, IN const bool bDelFinishEvent
			//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
			//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
			, IN const UidType iUserUID
		//}}
			, IN const std::set< int >& setItemID
			, IN const bool bIsSteamUser
			)
			:	m_iUnitUID( iUnitUID ),
			m_ucLevel( ucLevel ),
			m_bIsPcBang( bIsPcBang )
			//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
			, m_iChannelCode( iChannelCode )
			//}}
			//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
			//}}
			//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
			, m_cUnitType( cUnitType )
			, m_cUnitClass( cUnitClass )
			//}}

			, m_bDelFinishEvent( bDelFinishEvent )
			//{{ 2012. 06. 07	박세훈	매일매일 선물 상자
			//}}
			//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
			//}}
			//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
			, m_iUserUID( iUserUID )
			//}}
			, m_setItemID( setItemID )
			, m_bIsSteamUser( bIsSteamUser )
		{
			m_pVecConnectTimeEvent		= NULL;
			m_pVecCumulativeTimeEvent	= NULL;
			m_pMapEventTime				= NULL;
			m_pSetCompletedEvent		= NULL;
			m_pVecUpdatedTime			= NULL;
			m_pVecBeginEvent			= NULL;
			m_pVecEndEvent				= NULL;
			m_pVecCompletedAdvertisementEvent = NULL;
		}
	};
	//}}

	KGameEventManager();
	virtual ~KGameEventManager();

	void		SetServerType( KGameEventManager::SERVER_TYPE eServerType ){ m_eServerType = eServerType; }
	SERVER_TYPE	GetServerType() const{ return m_eServerType; }

	virtual	void	Tick();

	void	Init();

	// DB에서 이벤트 패킷이 올경우..
	void	SetEvent( IN const std::vector< KEventInfo >& vecEventList );

	// 던전 보상 이벤트
	//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
	void	GetRewardEvent( OUT SGameEventBonusRate& kBonusRateInfo
						  , IN const int iDungeonID
						  , IN const int iUnitLevel
						  , IN const char cUnitClass
						  , IN const int iPartyNum
						  , IN const bool isPcBang
							);
	//}}

	//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합
	void	GetRewardEvent( IN OUT KRoomUserInfo& kRoomUserInfo
						  , IN const int iDungeonID
						  , IN const int iUnitLevel
						  , IN const char cUnitClass
						  , IN const int iPartyNum
						  , IN const bool isPcBang



						  );
	//}}

	//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장
	//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )

	bool	CheckItemDropProbEvent( OUT float& fDropRate, OUT bool& bWithPlayPcBang );	// 드롭률 이벤트

	//}}

	//{{ 2012. 12. 16  드롭 이벤트 - 김민성
	bool	CheckItemDropProbEvent( OUT float& fDropRate );	// 드롭률 이벤트
	//}}

	bool	CheckTournamentEvent();														// 대회 이벤트
	bool	CheckSpiritEvent();															// 근성도 무제한 이벤트
	bool	CheckSecretDungeonEvent( int iDungeonID, int iDungeonMode );				// 비밀던전 무료 입장 이벤트

	//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	void	CheckEventMonster( OUT std::vector< int >& vecEventID );
	//}}
	//{{ 2012. 12. 30	박세훈	필드 이벤트 몬스터
	void	CheckEventMonster( OUT std::set<int>& setEventID );
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2010. 02. 09  최육사	일일 이벤트 퀘스트

	//{{ 2010. 04. 02  최육사	일일퀘스트

	void	CheckEnableEventQuest( IN OUT std::map< int, KEventQuestInfo >& mapEventQuest,
								   IN const std::map< int, KCompleteQuestInfo >& mapCompeletedQuest,
								   OUT std::vector< int >& vecBeginQuest,
								   OUT std::vector< int >& vecEndQuest
								   );

	//}}

	//}}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 접속 시간 이벤트 초기화 함수
	void	InitTimeEventForCharInit( IN const std::vector< KConnectTimeEventInfo >& vecEventTime,
									  IN const std::vector< KCumulativeTimeEventInfo >& vecCumulativeEventTime,
									  OUT std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime,
									  OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
									  OUT std::set< int >& setCompletedEvent
									  );

	void	GetEventTimerInfo( IN const std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime, OUT std::map< int, double >& mapEventTimer );	// 이벤트 타이머 정보 얻기
	void	SetEventTimerInfo( OUT std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime, IN const std::map< int, double >& mapEventTimer );	// 이벤트 타이머 정보 저장

	//////////////////////////////////////////////////////////////////////////
	// 접속 시간 이벤트
	//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
	bool	GetConnectTimeEventReward( IN const u_char ucLevel, IN const int iEventUID, OUT int& iRewardID );
	//}}

	//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
	//{{ 2008. 6. 19  최육사  개선된 아이템 이벤트
	void	CheckEnableConnectTimeEventForCharInit( IN OUT ConnectEventFactorSet& kFactor );
	void	CheckEnableConnectTimeEvent( IN OUT ConnectEventFactorSet& kFactor );
	void	CheckConnectTimeEvent( IN OUT ConnectEventFactorSet& kFactor );
	//}}

	//}}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	bool	GetCumulativeTimeEventReward( IN const int iEventUID, OUT int& iRewardID );

	void	CheckEnableCumulativeTimeEventForCharInit( IN const u_char ucLevel,
													   IN const bool bIsPcBang,
													   //{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
													   IN const int iChannelCode,
													   //}}
													   //{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
													   //}}
													   IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapEventTime,
													   IN OUT std::set< int >& setCompleteEvent
													   );

	void	CheckEnableAndCompleteCumulativeTimeEvent( IN const u_char ucLevel,
													   IN const bool bIsPcBang,
													   //{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
													   IN const int iChannelCode,
													   //}}
													   //{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
													   //}}
													   IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapEventTime,
													   IN OUT std::set< int >& setCompleteEvent,
													   OUT std::vector< KUserCumulativeTimeEventInfo >& vecCompleteEventInfo,
													   OUT std::vector< KTimeEventSimpleInfo >& vecBeginEvent,
													   OUT std::vector< KTimeEventSimpleInfo >& vecEndEvent
													   );

	void	CheckCumulativeTimeEvent( IN const u_char ucLevel,
									  IN const bool bIsPcBang,
									  //{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
									  IN const int iChannelCode,
									  //}}
									  //{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
									  //}}
									  IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapEventTime,
									  IN OUT std::set< int >& setCompleteEvent,
									  IN OUT std::set< int >& setDirectRewardEvent,
									  OUT std::vector< KCumulativeTimeEventInfo >& vecUpdatedTime,
									  OUT std::vector< KTimeEventSimpleInfo >& vecBeginEvent,
									  OUT std::vector< KTimeEventSimpleInfo >& vecEndEvent
									  );

	void	GetDBUpdateInfo( IN const std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
							 IN const std::set< int >& setCompletedEvent,
							 OUT std::vector< KCumulativeTimeEventInfo >& vecEventTime );

	void	GetNowProcessingTimeEvent( IN const std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime,
									   IN const std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
									   IN const std::set< int >& setCompletedEvent,
									   OUT std::vector< KTimeEventSimpleInfo >& vecTimeEvent,
									   IN bool bAccountTimeEvent );

	void	DeleteDirectRewardAndGetDBUpdateInfo( IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
												  IN OUT std::set< int >& setCompletedEvent,
												  IN OUT std::set< int >& setDirectRewardEvent,
												  OUT std::vector< KCumulativeTimeEventInfo >& vecCompleteEvent );

	//}}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	void	RefreshEventScript();
	//}}

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}

	//{{ 2011. 08. 12   김민성      헤니르 개편
	void	AddHenirRewardCountEvent( IN const KEventInfo& kInfo );

	bool	GetHenirRewardCountEvent( OUT bool& bUnLimited, OUT int& iEventMax  );

	//}}

	void	CheckEnableAdvertisementEvent( IN OUT std::set< int >& setAdvertisementEvent, OUT std::vector< KTimeEventSimpleInfo >& wstrBeginEventURL, OUT std::vector< KTimeEventSimpleInfo >& wstrEndEventURL );
	void	AddAdvertisementEvent( IN const KEventInfo& kInfo );
	void	GetAdvertisementURL( OUT std::vector< std::wstring >& vecAdvertisementURL );

		bool TickCheckEvent();

	void InitEventReleaseTick();
	int GetEventReleaseTick() { return m_iEventReleaseTick; }
	void SetEventReleaseTick( int iReleaseTick ) { m_iEventReleaseTick = iReleaseTick; }


	int		GetMaxLevel();
	bool	IsEnableDungeon( int nDunegonID, bool bEventDungeon );
	void	CheckEnableEventDungeon( IN OUT std::map< int, bool >& mapCheckDungeon );
	int		GetAppliedEventTick() { return m_iAppliedEventTick; }

	bool	IsEnableCode( IN int iEventID );
	void	CheckEnableCode( OUT std::map< int, bool >& mapCheckCode );

	//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
	void	AddQuestItemDropEvent( IN const KEventInfo& kInfo );
	bool	CheckQuestItemDropProbEvent( OUT float& fRate );
	//}

	bool	IsEnableEventQuest( IN int iQuestID ); // 2013.06.10 lygan_조성욱 // DB에서 값 on/off 부분 추가 작업 // 국내 필드 들어가고 퀘스트 자동 수락 들어가면서 해당 기능 추가

	void GetWarningEventQuest( OUT KEGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT& kPacket_ );

	void	AddDefenseDungeonOpenRateEvent( IN const KEventInfo& kInfo );
	bool	GetDefenseDungeonOpenRate( OUT float& fDefenseDungeonOpenRate ) const;






protected:
	bool	RefreshEventTime( IN const KEventInfo& kInfo, OUT KGameEvent* pkGameEvent );
	bool	SetPeriodEventTime( IN const KEventInfo& kInfo, OUT CTime& ctBegin, OUT CTime& ctEnd );

	//## DB에서 이벤트 데이터를 받아 셋팅하는 함수들..
	void	AddQuestEvent( IN const KEventInfo& kInfo );
	void	AddItemDropEvent( IN const KEventInfo& kInfo );
	void	AddRewardEvent( IN const KEventInfo& kInfo );
	//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
	void	AddMonsterEvent( IN const KEventInfo& kInfo );
	//}}
	void	AddTournamentEvent( IN const KEventInfo& kInfo );
	void	AddSpiritEvent( IN const KEventInfo& kInfo );
	void	AddConnectTimeEvent( IN const KEventInfo& kInfo );
	void	AddSecretDungeonEvent( IN const KEventInfo& kInfo );
	void	AddNotifyMSGEvent( IN const KEventInfo& kInfo );
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	void	AddCumulativeTimeEvent( IN const KEventInfo& kInfo );
	//}}

	void	AddDungeonEvent( IN const KEventInfo& kInfo );
	void	AddMaxLevelEvent( IN const KEventInfo& kInfo );
	void	AddContents( IN const KEventInfo& kInfo );

	void	AddCodeEvent( IN const KEventInfo& kInfo );

	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}


protected:
	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	void	SendToLogDB( unsigned short usEventID ){ SendToLogDB( usEventID, char() ); }


protected:
	SERVER_TYPE						m_eServerType;

	std::map< int, KGameEvent* >	m_mapIngEventData[KGameEvent::GET_MAX];	// [EventUID, Event Struct]
	boost::timer					m_TimerEnableCheck;						// 일정 시간마다 이벤트 진행 상태를 변경하기 위해..
	boost::timer					m_TimerRefreshEvent;					// 일정 시간마다 DB에서 이벤트를 받아오기 위해..

	boost::timer					m_tEventCheckTimer;
	int								m_iEventReleaseTick;

	int								m_iAppliedEventTick;

};

template < class T >
void KGameEventManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { 0, -1 };

	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PC_LOG_DB, anTrace, usEventID, data );
	LIF( spEvent->m_kDestPerformer.AddUID( 0 ) );

	SiKDBLayer()->QueueingEvent( spEvent );
}


DefSingletonInline( KGameEventManager );
