#include "GameEventManager.h"
#include <algorithm>

#include <KncLua.h>
#include <lua_tinker.h>
#include "Lua/KLuaManager.h"
#include "BaseServer.h"
#include "Socket/NetCommon.h"
#include "X2Data/XSLDungeon.h"

//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )


//{{ 2011. 7.29 조효진 반복 퀘스트 및 일일 퀘스트도 퀘스트 이벤트로 등록가능하도록 수정
#include "../X2Data/XSLQuestManager.h"


	#include "GameEventScriptManager.h"
//}}

//}}

//{{ 2012. 06. 05	박세훈	매일매일 선물 상자
//}}

ImplementSingleton( KGameEventManager );

KGameEventManager::KGameEventManager()
{
	m_eServerType		 = ST_NONE;
}

KGameEventManager::~KGameEventManager()
{
	for( int idx = 0; idx < KGameEvent::GET_MAX; ++idx )
	{
		std::map< int, KGameEvent* >::iterator mit;
		for( mit = m_mapIngEventData[idx].begin(); mit != m_mapIngEventData[idx].end(); ++mit )
		{
			SAFE_DELETE( mit->second );
		}
		m_mapIngEventData[idx].clear();
	}
}

ImplToStringW( KGameEventManager )
{
	return stm_;
}

void KGameEventManager::Tick()
{
	TickCheckEvent();


	//////////////////////////////////////////////////////////////////////////
	// 10초 마다 Enable체크
	if( m_TimerEnableCheck.elapsed() < 10.0 )
		return;

	m_TimerEnableCheck.restart();

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	BEGIN_CHECK_TICK_LATENCY;
	//}}

	//이벤트의 시작과 종료를 진행시킴..

	CTime tCurrentTime = CTime::GetCurrentTime();

	for( int iGET = 0; iGET < KGameEvent::GET_MAX; ++iGET )
	{
		std::map< int, KGameEvent* >::iterator mit;
		for( mit = m_mapIngEventData[iGET].begin(); mit != m_mapIngEventData[iGET].end(); ++mit )
		{
			if( mit->second == NULL )
				continue;

			//현재 진행되는 이벤트중..
			if( mit->second->IsEnable() == true )
			{
				//종료시간이 현재시간보다 낮으면 종료.
				if( mit->second->GetEndDate() < tCurrentTime
					|| mit->second->IsOn() == false
					)
				{
					mit->second->SetEnable( false );

					// 새로 이벤트의 상태가 변경 된 후 이후 작업을 진행하기 위한 코드
					++m_iAppliedEventTick;
					CStringW strBegin(mit->second->GetBeginDate().Format(_T("%Y-%m-%d %H:%M:%S")));
					std::wstring wstrEventBegin(strBegin.GetString());
					CStringW strEnd(mit->second->GetEndDate().Format(_T("%Y-%m-%d %H:%M:%S")));
					std::wstring wstrEventEnd(strEnd.GetString());
					// Port: KSMS management tool notification removed (out of scope)

					START_LOG( cout2, L"Event ended." )
						<< BUILD_LOG( mit->second->GetUID() )
						<< BUILD_LOG( mit->second->GetEventName() )
						<< BUILD_LOG( mit->second->GetType() )
//#ifdef SERV_NEW_EVENT_TYPES
						<< BUILD_LOG( m_iAppliedEventTick )
//#endif SERV_NEW_EVENT_TYPES
						;

					//추후에 종료된 이벤트 리스트를 삭제하자.ㅡㅡ;
					//vecEraseEvent.push_back( mit );
				}
			}
			//아직 시작되지 않은 이벤트중..
			else
			{
				if( mit->second->GetType() == KGameEvent::GET_NOTIFY_MSG  &&  GetServerType() == ST_CENTER )
				{
					KGameEventNotifyMSG* pEvent = static_cast<KGameEventNotifyMSG*>(mit->second);

					if( pEvent == NULL )
						continue;

					if( pEvent->GetType() != KGameEvent::GET_NOTIFY_MSG )
						continue;

					if( pEvent->IsSend() == true )
						continue;

					if( pEvent->GetBeginDate() <= tCurrentTime
						&& mit->second->IsOn() == true
						)
					{
						pEvent->SetIsSend( true );

						//{{ 2008. 7. 11  최육사  GM 이벤트 공지
						//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
						if( SiKGameEventScriptManager()->IsNotifyMSGEvent() == true )
						//}}
						{
							KEGS_NOTIFY_MSG_NOT kPacket;
							kPacket.m_Count = 1;
							kPacket.m_wstrMSG = pEvent->GetNotifyMSG();

							//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합
							if( pEvent->IsAllServers() )
							{
								kPacket.m_cNotifyType = KEGS_NOTIFY_MSG_REQ::NT_GM_NOTIFY_ALL_SERVERS;

								UidType anTrace[2] = { 0, -1 };
								KncSend( PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, ECN_NOTIFY_MSG_NOT, kPacket );
							}
							else
							{
								kPacket.m_cNotifyType = KEGS_NOTIFY_MSG_REQ::NT_GM_NOTIFY_SERVER_GROUP;

								UidType anTrace[2] = { 0, -1 };
								KncSend( PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER_SAME, 0 , anTrace, ECN_NOTIFY_MSG_NOT, kPacket );
							}
//#else
//							UidType anTrace[2] = { 0, -1 };
//							KncSend( PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, ECN_NOTIFY_MSG_NOT, kPacket );
							//}}
						}
						//}}
					}
				}
				else
				{
					//진행이 시작되었으면
					if( mit->second->GetBeginDate() <= tCurrentTime && mit->second->GetEndDate() >= tCurrentTime
						&& mit->second->IsOn() == true
						)
					{
						mit->second->SetEnable( true );

						// 새로 이벤트의 상태가 변경 된 후 이후 작업을 진행하기 위한 코드
						++m_iAppliedEventTick;
						CStringW strBegin = mit->second->GetBeginDate().Format(_T("%Y-%m-%d %H:%M:%S"));
						std::wstring wstrEventBegin = strBegin.GetString();
						CStringW strEnd = mit->second->GetEndDate().Format(_T("%Y-%m-%d %H:%M:%S"));
						std::wstring wstrEventEnd = strEnd.GetString();
						// Port: KSMS management tool notification removed (out of scope)
						START_LOG( cout2, L"Event started." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetEventName() )
							<< BUILD_LOG( mit->second->GetType() )
//#ifdef SERV_NEW_EVENT_TYPES
							<< BUILD_LOG( m_iAppliedEventTick )
//#endif SERV_NEW_EVENT_TYPES
							;
					}
				}
			}
		}
	}

	//for( int i = 0; i < (int)vecEraseEvent.size(); ++i )
	//{
	//	if( vecEraseEvent[i]->second != NULL )
	//		delete vecEraseEvent[i]->second;

	//	m_mapIngEventData.erase( vecEraseEvent[i] );
	//}
	//vecEraseEvent.clear();

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	END_CHECK_TICK_LATENCY;
	//}}
}

void KGameEventManager::Init()
{
	InitEventReleaseTick();


	// 이벤트 정보 얻기
	SendToLogDB( DBE_EVENT_UPDATE_REQ );
}

void KGameEventManager::SetEvent( IN const std::vector< KEventInfo >& vecEventList )
{
	std::vector< KEventInfo >::const_iterator pNewEvent;
	for( pNewEvent = vecEventList.begin(); pNewEvent != vecEventList.end(); ++pNewEvent )
	{
		for( int iEventType = KGameEvent::GET_QUEST; iEventType < KGameEvent::GET_MAX; ++iEventType )
		{
			std::map< int, KGameEvent* >::iterator mitIng = m_mapIngEventData[iEventType].find( pNewEvent->m_iEventUID );

			// 돌아가고 있던 이벤트 중에서 이번에 넣은 이벤트를 찾았다.
			if( mitIng != m_mapIngEventData[iEventType].end() )
			{
				KGameEvent* pIngEvent = mitIng->second;

				// 켜져 있던게 off가 되면서 꺼진 경우
				if( pIngEvent->IsEnable() == true && pNewEvent->m_bOn == false )
				{
					// 성욱씨 코드. 켜진 이벤트라도 꺼줍니다.
					if ( iEventType == KGameEvent::GET_CONNECT_TIME )
					{
						KGameEventConnectTime* pEvent = static_cast<KGameEventConnectTime*>( pIngEvent );
						if ( pEvent != NULL )
							pEvent->SetDelEndEvent(true);

					}

					pIngEvent->SetOn(false); // 2013.01.22 lygan_조성욱 // DB에서 이벤트를 끄면 현재 돌아가고 있는 이벤트도 강제로 꺼준다
				}
			}
		}
	}

	for( const KEventInfo& kEventInfo : vecEventList )
	{
		switch( kEventInfo.m_iEventType )
		{
		case KGameEvent::GET_QUEST:
			AddQuestEvent( kEventInfo );
			break;

			//{{ 2009. 5. 22  최육사	드롭률이벤트
		case KGameEvent::GET_ITEM_DROP:
			AddItemDropEvent( kEventInfo );
			break;
			//}}

		case KGameEvent::GET_REWARD:
			AddRewardEvent( kEventInfo );
			break;

			//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템
		case KGameEvent::GET_MONSTER:
			AddMonsterEvent( kEventInfo );
			break;
			//}}

		case KGameEvent::GET_TOURNAMENT:
			AddTournamentEvent( kEventInfo );
			break;

		case KGameEvent::GET_SPIRIT:
			AddSpiritEvent( kEventInfo );
			break;

		case KGameEvent::GET_CONNECT_TIME:
			AddConnectTimeEvent( kEventInfo );
			break;

		case KGameEvent::GET_SECRET_DUNGEON:
			AddSecretDungeonEvent( kEventInfo );
			break;

		case KGameEvent::GET_NOTIFY_MSG:
			AddNotifyMSGEvent( kEventInfo );
			break;

			//{{ 2009. 12. 7  최육사	누적시간이벤트
		case KGameEvent::GET_CUMULATIVE_TIME:
			AddCumulativeTimeEvent( kEventInfo );
			break;
			//}}
			//{{ 2011. 08. 12   김민성      헤니르 개편
		case KGameEvent::GET_HENIR_REWARD:
			AddHenirRewardCountEvent( kEventInfo );
			break;
			//}}

		case KGameEvent::GET_ADVERTISEMENT_EVENT:
			AddAdvertisementEvent( kEventInfo );
			break;

		case KGameEvent::GET_DUNGEON:
			AddDungeonEvent( kEventInfo );
			break;
		case KGameEvent::GET_MAX_LEVEL:
			AddMaxLevelEvent( kEventInfo );
			break;
		case KGameEvent::GET_CONTENTS:
			AddContents( kEventInfo );
			break;

		case KGameEvent::GET_CODE:
			AddCodeEvent( kEventInfo );
			break;

			//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
		case KGameEvent::GET_QUEST_ITEM_DROP:
			AddQuestItemDropEvent( kEventInfo );
			break;
			//}

		case KGameEvent::GET_DEFENSE_DUNGEON:
			AddDefenseDungeonOpenRateEvent( kEventInfo );
			break;



		default:
			{
				START_LOG( cerr, L"Unknown event type or unimplemented event feature." )
					<< BUILD_LOG( kEventInfo.m_iEventUID )
					<< BUILD_LOG( kEventInfo.m_iEventType )
					<< BUILD_LOG( kEventInfo.m_wstrEventName )
					<< BUILD_LOG( kEventInfo.m_wstrBeginDate )
					<< BUILD_LOG( kEventInfo.m_wstrEndDate )
					<< BUILD_LOG( kEventInfo.m_iScriptID )
					<< BUILD_LOG( (int)kEventInfo.m_bEnable )
					<< END_LOG;
			}
		}
	}
}

//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합

//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
void KGameEventManager::GetRewardEvent( IN OUT KRoomUserInfo& kRoomUserInfo
									  , IN const int iDungeonID
									  , IN const int iUnitLevel
									  , IN const char cUnitClass
									  , IN const int iPartyNum
									  , IN const bool isPcBang


									  )
{
	SGameEventBonusRate kBonusRateInfo;

	GetRewardEvent( kBonusRateInfo
		, iDungeonID
		, iUnitLevel
		, cUnitClass
		, iPartyNum
		, isPcBang
		);

	if( kBonusRateInfo.m_fEXPRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_EXP, kBonusRateInfo.m_fEXPRate );
	}
	if( kBonusRateInfo.m_fVPRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_VP, kBonusRateInfo.m_fVPRate );
	}
	if( kBonusRateInfo.m_fEDRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_ED, kBonusRateInfo.m_fEDRate );
	}
	if( kBonusRateInfo.m_fPartyEXPRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_PARTY_EXP, kBonusRateInfo.m_fPartyEXPRate );
	}
	if( kBonusRateInfo.m_fPartyVPRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_PARTY_VP, kBonusRateInfo.m_fPartyVPRate );
	}
	if( kBonusRateInfo.m_fPartyEDRate > 0.0f )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_PARTY_ED, kBonusRateInfo.m_fPartyEDRate );
	}
	//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
	// 작업날짜: 2014-01-08 // 김현철 // 특정 캐릭터와 플레이시 경험치 증가 이벤트가 적용 될 때 다른 경험치 증가가 적용되지 않는 문제 수정
	//}}

	if( 0.0f < kBonusRateInfo.m_fRankingPointRate )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_RANKING_POINT, kBonusRateInfo.m_fRankingPointRate );
	}

	if( 0.0f < kBonusRateInfo.m_fPartyRankingPointRate )
	{
		kRoomUserInfo.AddBonusRate( KRoomUserInfo::BT_GEV_PARTY_RANKING_POINT, kBonusRateInfo.m_fPartyRankingPointRate );
	}
}
//}}

//}}

//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
void KGameEventManager::GetRewardEvent( OUT SGameEventBonusRate& kBonusRateInfo
									  , IN const int iDungeonID
									  , IN const int iUnitLevel
									  , IN const char cUnitClass
									  , IN const int iPartyNum
									  , IN const bool isPcBang
									  )
{
	kBonusRateInfo.Clear();
	const char cUnitType = static_cast<char>( CXSLUnit::GetUnitClassToUnitType( static_cast<CXSLUnit::UNIT_CLASS>(cUnitClass) ) );

	//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
	float fNonAccumulateEXPRate_Solo = 0.f;
	float fNonAccumulateEXPRate_party = 0.f;
	//}}

	//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합
	if( GetServerType() != ST_GAME )
		return;
	//}}

	//이벤트 진행시간 체크를 위해 현재 시간을 받아온다.

	std::map< int, KGameEvent* >::iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_REWARD].begin(); mit != m_mapIngEventData[KGameEvent::GET_REWARD].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		KGameEventReward* pEvent = static_cast<KGameEventReward*>(mit->second);

		//#1. 보상 이벤트인지 체크..
		if( pEvent->GetType() != KGameEvent::GET_REWARD )
			continue;

		if ( pEvent->IsOn() == false )
			continue;

		//#2. 종료된 이벤트인지 체크
		if( pEvent->IsEnable() == false )
			continue;

		//#3. 유닛레벨 검사(이건 있던없던 기본값이 0이상이기때문에 무조건 검사해도 상관없음.)
		if( pEvent->GetUnitLevel() != 0  &&  pEvent->GetUnitLevel() < iUnitLevel )
			continue;

		//#4. 던전아이디검사(0이 아닌 설정된 값이 있을때문 체크)
		if( pEvent->GetDungeonID() != 0  &&  pEvent->GetDungeonID() != iDungeonID )
			continue;

		//#5. 유닛타입 검사
		if( pEvent->GetUnitType() != CXSLUnit::UT_NONE  &&  pEvent->GetUnitType() != cUnitType )
			continue;

		//#6. 유닛클레스 검사
		if( pEvent->GetUnitClass() != CXSLUnit::UC_NONE  &&  pEvent->GetUnitClass() != cUnitClass )
			continue;

		//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
		//#7. 서버군 검사

#ifdef EXTEND_SERVER_GROUP_MASK
		if( pEvent->GetServerGroupID() != -1 && pEvent->GetServerGroupID() != KBaseServer::GetKObj()->GetServerGroupID() )
#else
		if( pEvent->GetServerGroupID() != SEnum::SGI_INVALID  &&  pEvent->GetServerGroupID() != KBaseServer::GetKObj()->GetServerGroupID() )
#endif // EXTEND_SERVER_GROUP_MASK
			continue;
		//}}

		//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
		//}}

		if( ( pEvent->IsPcBangEvent() == true ) && ( isPcBang == false ) )
		{
			continue;
		}

		//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
		// 제일 마지막이어야 함

		//#Last. 중첩 여부
		if( pEvent->IsAccumulateEXP() == false )
		{
			//#Last-1. 중첩 불가인데 개인 추가 경험치
			if( pEvent->GetPartyNum() == 0 )
			{
				if( fNonAccumulateEXPRate_Solo < pEvent->GetEXPRate() )
				{
					fNonAccumulateEXPRate_Solo = pEvent->GetEXPRate();
					continue;
				}
			}
			//#Last-2. 중첩 불가인데 파티 추가 경험치
			else if( pEvent->GetPartyNum() <= iPartyNum )
			{
				if( fNonAccumulateEXPRate_party < pEvent->GetEXPRate() )
				{
					fNonAccumulateEXPRate_party = pEvent->GetEXPRate();
					continue;
				}
			}
		}
		//}}

		if( pEvent->GetPartyNum() == 0 )
		{
			//적용할 확률데이터를 있는 만큼 더해준다.
			kBonusRateInfo.m_fEXPRate			+= pEvent->GetEXPRate();
			kBonusRateInfo.m_fVPRate			+= pEvent->GetVPRate();
			kBonusRateInfo.m_fEDRate			+= pEvent->GetEDRate();
			kBonusRateInfo.m_fRankingPointRate	+= pEvent->GetRankingPointRate();
		}
		else if( pEvent->GetPartyNum() <= iPartyNum )
		{
			//적용할 확률데이터를 있는 만큼 더해준다.
			kBonusRateInfo.m_fPartyEXPRate			+= pEvent->GetEXPRate();
			kBonusRateInfo.m_fPartyVPRate			+= pEvent->GetVPRate();
			kBonusRateInfo.m_fPartyEDRate			+= pEvent->GetEDRate();
			kBonusRateInfo.m_fPartyRankingPointRate	+= pEvent->GetRankingPointRate();
		}
	}

	//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
	if( kBonusRateInfo.m_fEXPRate < fNonAccumulateEXPRate_Solo )
	{
		kBonusRateInfo.m_fEXPRate = fNonAccumulateEXPRate_Solo;
	}
	if( kBonusRateInfo.m_fPartyEXPRate < fNonAccumulateEXPRate_party )
	{
		kBonusRateInfo.m_fPartyEXPRate = fNonAccumulateEXPRate_party;
	}
	//}}
}
//}}

//{{ 2009. 5. 22  최육사	드롭률이벤트
//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )

bool KGameEventManager::CheckItemDropProbEvent( OUT float& fDropRate, OUT bool& bWithPlayPcBang )

{
	if( GetServerType() != ST_CENTER )
		return false;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_ITEM_DROP].begin(); mit != m_mapIngEventData[KGameEvent::GET_ITEM_DROP].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_ITEM_DROP )
			continue;

		//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )
		KGameEventItemDrop* pEvent = static_cast<KGameEventItemDrop*>(mit->second);

		// 진행중인 드롭률 이벤트
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )

			fDropRate = pEvent->GetDropRate();

			bWithPlayPcBang = pEvent->IsWithPlayPcBang();
			return true;
		}
	}

	return false;
}

//}}

//{{ 2012. 12. 16  드롭 이벤트 - 김민성
bool KGameEventManager::CheckItemDropProbEvent( OUT float& fDropRate )
{
	if( GetServerType() != ST_CENTER )
		return false;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_ITEM_DROP].begin(); mit != m_mapIngEventData[KGameEvent::GET_ITEM_DROP].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_ITEM_DROP )
			continue;

		KGameEventItemDrop* pEvent = static_cast<KGameEventItemDrop*>(mit->second);

		// 진행중인 드롭률 이벤트
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			fDropRate = pEvent->GetDropRate();

			return true;
		}
	}

	return false;
}
//}}

bool KGameEventManager::CheckTournamentEvent()
{
	if( GetServerType() != ST_GAME  &&
		GetServerType() != ST_CENTER )
	{
		return false;
	}

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_TOURNAMENT].begin(); mit != m_mapIngEventData[KGameEvent::GET_TOURNAMENT].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_TOURNAMENT )
			continue;

		//. 진행중인 대회가 있으면..
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			return true;
		}
	}

	return false;
}

bool KGameEventManager::CheckSpiritEvent()
{
	if( GetServerType() != ST_GAME )
		return false;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_SPIRIT].begin(); mit != m_mapIngEventData[KGameEvent::GET_SPIRIT].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_SPIRIT )
			continue;

		//. 진행중인 대회가 있으면..
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			return true;
		}
	}

	return false;
}

bool KGameEventManager::CheckSecretDungeonEvent( int iDungeonID, int iDungeonMode )
{
	if( GetServerType() != ST_GAME )
		return false;

	if( CXSLDungeon::IsSecretDungeon( iDungeonID ) == false  &&
		CXSLDungeon::IsEventDungeon( iDungeonID ) == false  &&
		CXSLDungeon::IsHenirDungeon( iDungeonID ) == false )
	{
		// 비밀던전 또는 헤니르 시공 또는 이벤트던전이 아니면 리턴!
		return false;
	}

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_SECRET_DUNGEON].begin(); mit != m_mapIngEventData[KGameEvent::GET_SECRET_DUNGEON].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_SECRET_DUNGEON )
			continue;

		//{{ 2010. 10. 19	최육사	비밀던전 이벤트 업데이트
		KGameEventSecretDungeon* pEvent = static_cast<KGameEventSecretDungeon*>(mit->second);

		if( pEvent->GetDungeonID() != 0 )
		{
			if( pEvent->GetDungeonID() != iDungeonID )
				continue;
		}

		//{{ 2011. 09. 28  임규수		할로윈 던전
		if( CXSLDungeon::IsEventDungeon( iDungeonID ) == false )  // 추가
		{
			if( iDungeonMode != CXSLDungeon::DM_INVALID )
			{
				if( pEvent->GetDungeonMode() != iDungeonMode )
					continue;
			}
		}
		//}}

		//}}

		//. 진행중인 비밀던전 입장 이벤트가 있으면..
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			return true;
		}
	}

	return false;
}

//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템

void KGameEventManager::CheckEventMonster( OUT std::vector< int >& vecEventID )
{
	vecEventID.clear();

	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_MONSTER].begin(); mit != m_mapIngEventData[KGameEvent::GET_MONSTER].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_MONSTER )
			continue;

		if (mit->second->IsOn() == false)
			continue;

		KGameEventMonster* pEvent = static_cast<KGameEventMonster*>(mit->second);

		// 진행중인 몬스터 출현 이벤트가 있으면..
		if( mit->second->IsEnable() )
		{
			vecEventID.push_back( pEvent->GetEventID() );
		}
	}
}

//}}

//{{ 2012. 12. 30	박세훈	필드 이벤트 몬스터
void KGameEventManager::CheckEventMonster( OUT std::set<int>& setEventID )
{
	setEventID.clear();

	if( GetServerType() != ST_CENTER )
		return;

	for( std::map<int, KGameEvent*>::const_iterator it=m_mapIngEventData[KGameEvent::GET_MONSTER].begin(); it != m_mapIngEventData[KGameEvent::GET_MONSTER].end(); ++it )
	{
		if( it->second == NULL )
			continue;

		if( it->second->GetType() != KGameEvent::GET_MONSTER )
			continue;

		if (it->second->IsOn() == false)
			continue;

		KGameEventMonster* pEvent = static_cast<KGameEventMonster*>( it->second );

		// 진행중인 몬스터 출현 이벤트가 있으면..
		if( it->second->IsEnable() )
		{
			setEventID.insert( pEvent->GetEventID() );
		}
	}
}
//}}

//{{ 2010. 02. 09  최육사	일일 이벤트 퀘스트

//{{ 2010. 04. 02  최육사	일일퀘스트
void KGameEventManager::CheckEnableEventQuest( IN OUT std::map< int, KEventQuestInfo >& mapEventQuest,
											  IN const std::map< int, KCompleteQuestInfo >& mapCompeletedQuest,
											  OUT std::vector< int >& vecBeginQuest,
											  OUT std::vector< int >& vecEndQuest )
//}}
{
	if( GetServerType() != ST_GAME )
		return;

	vecBeginQuest.clear();
	vecEndQuest.clear();


	std::map< int, KGameEvent* >::const_iterator mit;
	std::map< int, KEventQuestInfo >::iterator mitET;

	std::map< int, std::set< int > > mapQuestTurnOn;
	std::set< int > setQuestTurnOff;

	// 새로 시작되는 이벤트가 있으면 추가하고 종료되는 이벤트가 있으면 삭제
	for( mit = m_mapIngEventData[KGameEvent::GET_QUEST].begin(); mit != m_mapIngEventData[KGameEvent::GET_QUEST].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_QUEST )
			continue;

		const int iEventUID = mit->first;
		KGameEventQuest* pEvent = static_cast<KGameEventQuest*>(mit->second);

		{
			const CXSLQuestManager::QuestTemplet* pQuestTemplet = SiCXSLQuestManager()->GetQuestTemplet( pEvent->GetQuestID() );
			if( pQuestTemplet != NULL && pQuestTemplet->m_bIsTimeEvent == false )
			{
				continue;
			}
		}

		// 3. 이벤트가 진행중인지..
		if( mit->second->IsEnable()
			&& mit->second->IsOn() == true
			)
		{
			mapQuestTurnOn[ pEvent->GetQuestID() ].insert( iEventUID );
		}
		else
		{
			// 4-2. 종료된 이벤트를 유저의 이벤트 리스트에서 찾아서 삭제한다.
			mitET = mapEventQuest.find( iEventUID );
			if( mitET != mapEventQuest.end() || mit->second->IsOn() == false )
			{
				setQuestTurnOff.insert( pEvent->GetQuestID() );
			}
			if( mitET != mapEventQuest.end() )
			{
				// 진행중인 이벤트 리스트에서 삭제!
				mapEventQuest.erase( iEventUID );

			}
		}
	}

	std::map< int, std::set< int > >::iterator mitQuestTurnOn = mapQuestTurnOn.begin();
	for( ; mitQuestTurnOn != mapQuestTurnOn.end(); ++mitQuestTurnOn )
	{
		std::set< int >& setQuestUIDTurnOn = mitQuestTurnOn->second;

		// 현재 상황에선 발생하지 않을 것이지만, 후일 set이 비어 있는 경우가 있을까봐 처리해 둡니다. by 박진웅
		if( setQuestUIDTurnOn.empty() == true )
			continue;

		int iQuestID = mitQuestTurnOn->first;

		//{{ 2011. 7.29 조효진 반복 퀘스트 및 일일 퀘스트도 퀘스트 이벤트로 등록가능하도록 수정
		std::map< int, KCompleteQuestInfo >::const_iterator const_it = mapCompeletedQuest.find( iQuestID );

		// 완료한 정보가 있는 이벤트라면 반복 가능 여부를 체크한다.
		if( const_it != mapCompeletedQuest.end() )
		{
			const CXSLQuestManager::QuestTemplet* pQuestTemplet = SiCXSLQuestManager()->GetQuestTemplet( iQuestID );

			if( pQuestTemplet == NULL )
				continue;

			// 반복 가능 여부 체크 //
			// 일반 퀘스트는 단 한번 가능
			if( pQuestTemplet->m_eRepeatType == CXSLQuestManager::QRT_NORMAL )
			{
				continue;
			}
			else if( pQuestTemplet->m_eRepeatType == CXSLQuestManager::QRT_DAY )
			{
				const KCompleteQuestInfo tmpCompletQuestInfo = const_it->second;

				if( tmpCompletQuestInfo.m_tCompleteDate == 0 )
				{
					START_LOG( cerr, L"Quest is marked complete, but its completion date is invalid." )
						<< BUILD_LOG( iQuestID )
						<< END_LOG;

					continue;
				}

				// 이미 완료한 퀘스트라면 클리어한 날짜를 확인하여 오늘 수행 가능한지 체크한다.
				CTime tCompleteDate( tmpCompletQuestInfo.m_tCompleteDate );
				CTime tCurTime = CTime::GetCurrentTime();

				CTime tCheckTime = CTime( tCompleteDate.GetYear(), tCompleteDate.GetMonth(), tCompleteDate.GetDay(), CXSLQuestManager::QE_DAILY_REPEAT_HOUR, 0, 0 );
				if( tCompleteDate.GetHour() >= CXSLQuestManager::QE_DAILY_REPEAT_HOUR )
				{
					// 6시보다 넘는 시각이라면 다음날로 세팅
					tCheckTime += CTimeSpan( 1, 0, 0, 0 );
				}

				// 오늘 수행 가능한가요?
				if( tCheckTime > tCurTime )
				{
					continue;
				}
			}
		}
		//}}

		std::set< int >::iterator sitQuestUIDTurnOn = setQuestUIDTurnOn.begin();
		for( ; sitQuestUIDTurnOn != setQuestUIDTurnOn.end(); ++sitQuestUIDTurnOn )
		{
			int iEventUID = *sitQuestUIDTurnOn;

			// 4-1. 진행중인 이벤트라면 유저의 이벤트 리스트를 확인 후 없는 이벤트라면 채워준다.
			mitET = mapEventQuest.find( iEventUID );
			if( mitET == mapEventQuest.end() )
			{
				//////////////////////////////////////////////////////////////////////////
				// 이벤트 새로 시작!
				KEventQuestInfo kInfo;
				kInfo.m_bInit = true;
				kInfo.m_iEventUID = iEventUID;
				kInfo.m_iQuestID = iQuestID;
				mapEventQuest.insert( std::make_pair( iEventUID, kInfo ) );

				// 클라이언트에게 시작되는 이벤트를 알리기 위해서 QuestID 저장
				vecBeginQuest.push_back( iQuestID );
			}
			else
			{
				// 캐릭터를 처음 선택한 상황이라 이벤트 데이터가 초기화 되어 있지 않다면..?
				if( mitET->second.m_bInit == false )
				{
					mitET->second.m_bInit = true;
					mitET->second.m_iEventUID = iEventUID;
					mitET->second.m_iQuestID = iQuestID;
				}
			}
		}
	}

	std::set< int >::iterator sitQuestTurnOff = setQuestTurnOff.begin();
	for( ; sitQuestTurnOff != setQuestTurnOff.end(); ++sitQuestTurnOff )
	{
		int iQuestID = *sitQuestTurnOff;
		if( mapQuestTurnOn.find( iQuestID ) != mapQuestTurnOn.end() )
			continue;

		//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
		vecEndQuest.push_back( iQuestID );		// 2013.06.10 lygan_조성욱 // 퀘스트의 경우 Enable 이 꺼지면 리스트에서 빼줘야 한다.
		//}}
	}
}

//}}

void KGameEventManager::InitTimeEventForCharInit( IN const std::vector< KConnectTimeEventInfo >& vecEventTime,
												  IN const std::vector< KCumulativeTimeEventInfo >& vecCumulativeEventTime,
												  OUT std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime,
												  OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
												  OUT std::set< int >& setCompletedEvent
												  )
{
	CTime tEventTime;
	CTime tCurTime = CTime::GetCurrentTime();

	// 누적 시간 이벤트 정보
	for( u_int ui = 0; ui < vecCumulativeEventTime.size(); ++ui )
	{
		std::pair< KUserCumulativeTimeEventInfo, KTimerManager > kUserCumulativeEvent;
		kUserCumulativeEvent.first.m_iEventUID = vecCumulativeEventTime[ui].m_iEventUID;
		kUserCumulativeEvent.second.restart();
		kUserCumulativeEvent.second.SetAddTime( static_cast<double>( vecCumulativeEventTime[ui].m_iCumulativeTime * 60 ) );
		mapCumulativeEventTime.insert( std::make_pair( vecCumulativeEventTime[ui].m_iEventUID, kUserCumulativeEvent ) );
	}

	// 접속 시간 이벤트 정보
	for( u_int ui = 0; ui < vecEventTime.size(); ++ui )
	{
		if( !KncUtil::ConvertStringToCTime( vecEventTime[ui].m_wstrEventTime, tEventTime ) )
		{
			START_LOG( cerr, L"Failed to convert the time string." )
				<< BUILD_LOG( vecEventTime[ui].m_wstrEventTime )
				<< END_LOG;

			// 문자열 변환 실패시 보상 주지 않는것으로 처리
			tEventTime = CTime( 2050, tCurTime.GetMonth(), tCurTime.GetDay(), 23, 59, 59 );
		}

		// 이미 이벤트 보상을 받았는가?
		if( tCurTime <= tEventTime )
		{
			// 완료 리스트에 넣는다.
			setCompletedEvent.insert( vecEventTime[ui].m_iEventUID );
			continue;
		}

		// 시간 이벤트 리스트에서 누적 시간 이벤트가 아닌것만 찾아서 저장한다.
		if( mapCumulativeEventTime.find( vecEventTime[ui].m_iEventUID ) == mapCumulativeEventTime.end() )
		{
			std::pair< KUserConnectTimeEventInfo, KTimerManager > kUserConnectEvent;
			kUserConnectEvent.first.m_fEventTime = END_EVENT; // 기본값으로 종료된 이벤트값을 넣는다
			kUserConnectEvent.first.m_kCTime		= tEventTime.GetTime();
			kUserConnectEvent.first.m_bGiveOnce	= true;
			kUserConnectEvent.second.restart(); // 타이머 초기화
			mapConnectEventTime.insert( std::make_pair( vecEventTime[ui].m_iEventUID, kUserConnectEvent ) );
		}
	}
}

void KGameEventManager::GetEventTimerInfo( IN const std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime, OUT std::map< int, double >& mapEventTimer )
{
	CTime tCurrentTime = CTime::GetCurrentTime();

	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = mapConnectEventTime.begin(); mit != mapConnectEventTime.end(); ++mit )
	{
		const KUserConnectTimeEventInfo& kConnectEventInfo = mit->second.first;
		const KTimerManager& kEventTimer = mit->second.second;

		// 1. 이미 보상을 받았는지 검사
		if( tCurrentTime.GetTime() <= kConnectEventInfo.m_kCTime )
			continue;

		// 2. END_EVENT값은 이미 '끝나버린 이벤트'임.
		if( kConnectEventInfo.m_fEventTime <= END_EVENT )
			continue;

		// 3. 현재 까지 진행한 이벤트 타이머 얻기
		mapEventTimer.insert( std::make_pair( mit->first, kEventTimer.elapsed() ) );
	}
}

void KGameEventManager::SetEventTimerInfo( OUT std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime, IN const std::map< int, double >& mapEventTimer )
{
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::iterator mitFind;
	std::map< int, double >::const_iterator mit;
	for( mit = mapEventTimer.begin(); mit != mapEventTimer.end(); ++mit )
	{
		mitFind = mapConnectEventTime.find( mit->first );
		if( mitFind == mapConnectEventTime.end() )
			continue;

		KTimerManager& kEventTimer = mitFind->second.second;

		// 채널이동으로 인한 이벤트 타이머 연장
		kEventTimer.SetAddTime( mit->second );
	}
}

//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
bool KGameEventManager::GetConnectTimeEventReward( IN const u_char ucLevel, IN const int iEventUID, OUT int& iRewardID )
//}}
{
	std::map< int, KGameEvent* >::const_iterator mit = m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].find( iEventUID );
	if( mit == m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].end() )
		return false;

	if( mit->second == NULL )
		return false;

	if( mit->second->GetType() != KGameEvent::GET_CONNECT_TIME )
		return false;

	KGameEventConnectTime* pEvent = static_cast<KGameEventConnectTime*>(mit->second);

	//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
	if( pEvent->IsMinMaxReward() == true )
	{
        iRewardID = pEvent->GetMinMaxRewardID( ucLevel );
	}
	else
	{
		iRewardID = pEvent->GetRewardID();
	}
	//}}

	return true;
}

//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
void KGameEventManager::CheckEnableConnectTimeEventForCharInit( IN OUT ConnectEventFactorSet& kFactor )
{
	if( ( kFactor.m_pMapEventTime == NULL ) ||
		( kFactor.m_pSetCompletedEvent == NULL ) )
	{
		START_LOG( cerr, L"Invalid login-event argument. This error should not occur; please investigate immediately." )
			<< BUILD_LOG( kFactor.m_pMapEventTime )
			<< BUILD_LOG( kFactor.m_pSetCompletedEvent )
			<< END_LOG;
		return;
	}

	std::vector< KTimeEventSimpleInfo > vecDummy1;
	std::vector< KTimeEventSimpleInfo > vecDummy2;

	kFactor.m_pVecBeginEvent	= &vecDummy1;
	kFactor.m_pVecEndEvent		= &vecDummy2;

	CheckEnableConnectTimeEvent( kFactor );

	// 종료된 이벤트 또는 보상받은 이벤트라면 삭제 처리 하자!
	std::vector< int > vecEndEvent;
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = kFactor.m_pMapEventTime->begin(); mit != kFactor.m_pMapEventTime->end(); ++mit )
	{
		const KUserConnectTimeEventInfo& kEventInfo = mit->second.first;

		// 이벤트 시간정보는 있지만 활성화되지 않은 이벤트라면..
		if( kEventInfo.m_fEventTime <= END_EVENT )
		{
			vecEndEvent.push_back( mit->first );
		}
	}

	// 종료된 이벤트들은 삭제
	std::vector< int >::const_iterator vit;
	for( vit = vecEndEvent.begin(); vit != vecEndEvent.end(); ++vit )
	{
		kFactor.m_pMapEventTime->erase( *vit );
	}
}

void KGameEventManager::CheckEnableConnectTimeEvent( IN OUT ConnectEventFactorSet& kFactor )
{
	if( GetServerType() != ST_GAME )
		return;

	if( ( kFactor.m_pVecBeginEvent == NULL  ) ||
		( kFactor.m_pVecEndEvent == NULL  ) ||
		( kFactor.m_pMapEventTime == NULL  ) ||
		( kFactor.m_pSetCompletedEvent == NULL  ) )
	{
		START_LOG( cerr, L"Invalid login-event argument. This error should not occur; please investigate immediately." )
			<< BUILD_LOG( kFactor.m_iUnitUID )
			<< BUILD_LOG( kFactor.m_pVecBeginEvent )
			<< BUILD_LOG( kFactor.m_pVecEndEvent )
			<< BUILD_LOG( kFactor.m_pMapEventTime )
			<< BUILD_LOG( kFactor.m_pSetCompletedEvent )
			<< END_LOG;
		return;
	}

	kFactor.m_pVecBeginEvent->clear();
	kFactor.m_pVecEndEvent->clear();

	CTime tCurrentTime = CTime::GetCurrentTime();

	std::map< int, KGameEvent* >::const_iterator mit;
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::iterator mitET;

	// 새로 시작되는 이벤트가 있으면 추가하고 종료되는 이벤트가 있으면 삭제
	for( mit = m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].begin(); mit != m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_CONNECT_TIME )
			continue;

		const int iEventUID = mit->first;
		KGameEventConnectTime* pEvent = static_cast<KGameEventConnectTime*>(mit->second);

		// 3. 이벤트가 진행중인지..
		if( mit->second->IsEnable() )
		{
			//{{ 2009. 10. 26  최육사	PC방이벤트
			if( pEvent->IsPcBangEvent() == true  &&  kFactor.m_bIsPcBang == false )
				continue;
			//}}

			//{{ 2009. 12. 16  최육사	이벤트개편
			if( pEvent->GetUnitLevel() != 0  &&  pEvent->GetUnitLevel() > static_cast<int>( kFactor.m_ucLevel ) )
				continue;
			//}}

			//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
			if( pEvent->GetChannelCode() != KNexonAccountInfo::CE_NONE )
			{
				if( pEvent->GetChannelCode() != kFactor.m_iChannelCode )
					continue;
			}
			//}}

			//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
			//}}

			//{{ 2011. 10. 12	김민성	접속 이벤트 레벨 체크
			// 보상 지급 가능한 레벨대가 아니라면 넘어간다!
			if( pEvent->IsMinMaxReward() == true )
			{
				if( pEvent->GetMinMaxRewardID( static_cast<int>( kFactor.m_ucLevel ) ) == 0 )
				{
					START_LOG( clog, L"Excluded from the event because the character is not at an eligible reward level." )
						<< END_LOG;
					continue;
				}
			}
			//}}

			//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
			if( pEvent->GetUnitType() != 0  &&  pEvent->GetUnitType() != kFactor.m_cUnitType )
				continue;

			if( pEvent->GetUnitClass() != 0  &&  pEvent->GetUnitClass() != kFactor.m_cUnitClass )
				continue;
			//}}
			if( pEvent->GetUnitClassLevel() == 0 && CXSLUnit::IsInitNormalJob(static_cast<CXSLUnit::UNIT_CLASS>(kFactor.m_cUnitClass)) == false )
				continue;

			if( pEvent->GetUnitClassLevel() == 1 && CXSLUnit::IsFirstChangeJob(static_cast<CXSLUnit::UNIT_CLASS>(kFactor.m_cUnitClass)) == false )
				continue;

			if( pEvent->GetUnitClassLevel() == 2 && CXSLUnit::IsSecondChangeJob(static_cast<CXSLUnit::UNIT_CLASS>(kFactor.m_cUnitClass)) == false )
				continue;

			if( pEvent->GetEventItemID() != 0 )
			{
				std::set< int >::iterator sitItemIDInInventory = kFactor.m_setItemID.find( pEvent->GetEventItemID() );
				if( sitItemIDInInventory == kFactor.m_setItemID.end() )
					continue;
			}



			// 이미 완료한 이벤트라면 넘어간다!
			if( kFactor.m_pSetCompletedEvent->find( iEventUID ) != kFactor.m_pSetCompletedEvent->end() )
				continue;

			//{{ 2012. 06. 05	박세훈	매일매일 선물 상자
			//}}

			//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
			//}}
			//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
			if( SiKGameEventScriptManager()->CheckEventGroup( pEvent->GetEventGroupID(), kFactor.m_iUserUID ) == false )
			{
				continue;
			}
			//}}


			if( pEvent->IsOnlySteamUserEvent() == true && kFactor.m_bIsSteamUser == false )
			{
				START_LOG( clog, L"[TEST] Skipping: this event is for Steam users, but the user is not on Steam." )
					<< END_LOG;
				continue;
			}

			if( pEvent->IsOnlyNotSteamUserEvent() == true && kFactor.m_bIsSteamUser == true )
			{
				START_LOG( clog, L"[TEST] Skipping: this event is for non-Steam users, but the user is on Steam." )
					<< END_LOG;
				continue;
			}

			if ( pEvent->IsRepeatRewardEvent() == true && ( pEvent->GetBeforeEventID())!= 0 ) // 2013.01.23 lygan_조성욱 // 자동 연계 접속 이벤트인데 선행 이벤트 가 0 이 아니면 이전 선행 이벤트를 완료 했는지 체크 해야 한다.
			{

				std::set< int >::const_iterator  mitSetComplet = kFactor.m_pSetCompletedEvent->begin();
				bool bCheckNoStartRewardEvent = true;

				for ( ; mitSetComplet != kFactor.m_pSetCompletedEvent->end() ; ++mitSetComplet )
				{
					std::map< int, KGameEvent* >::const_iterator mit2 = m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].find( *mitSetComplet );

					if ( mit2 != m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].end() )
					{
						KGameEventConnectTime* pEventOld = static_cast<KGameEventConnectTime*>(mit2->second);

						if ( pEventOld->IsRepeatRewardEvent() == true && pEventOld->GetScriptID() == pEvent->GetBeforeEventID() && pEventOld->GetBeginDate() == pEvent->GetBeginDate() )
						{
							bCheckNoStartRewardEvent = false;

						}

					}

				}

				if ( bCheckNoStartRewardEvent == true )
					continue;

			}



			// 4-1. 진행중인 이벤트라면 유저의 이벤트 리스트를 확인 후 없는 이벤트라면 채워준다.
			mitET = kFactor.m_pMapEventTime->find( iEventUID );
			if( mitET == kFactor.m_pMapEventTime->end() )
			{
				//////////////////////////////////////////////////////////////////////////
				// 이벤트 새로 시작!
				CTime tSetTime = CTime( tCurrentTime.GetYear(), tCurrentTime.GetMonth(), tCurrentTime.GetDay(), 0, 0, 0 );

				std::pair< KUserConnectTimeEventInfo, KTimerManager > kEventTime;
				kEventTime.first.m_bInit			= true;
				kEventTime.first.m_iEventID			= pEvent->GetEventID();
				kEventTime.first.m_wstrEventName	= pEvent->GetEventName();
				kEventTime.first.m_fEventTime		= pEvent->GetEventTime();
				kEventTime.first.m_kCTime			= tSetTime.GetTime();
				kEventTime.first.m_bGiveOnce		= pEvent->IsGiveOnce();
				//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
				kEventTime.first.m_bAccountEvent	= pEvent->IsAccountEvent();
				//}}
				//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
				kEventTime.first.m_bRepeatEvent	= pEvent->GetRepeatEvent();
				//}}

				//{{ 2012. 08. 14	박세훈	대천사의 주화 이벤트 가이드 문구 출력
				//}}

				kEventTime.first.m_bRepeatRewardEvent = pEvent->IsRepeatRewardEvent();
				kEventTime.first.m_iBeforeEventID = pEvent->GetBeforeEventID();


				kEventTime.first.m_bOnlySteamUser = pEvent->IsOnlySteamUserEvent();
				kEventTime.first.m_bOnlyNotSteamUser = pEvent->IsOnlyNotSteamUserEvent();

				kEventTime.second.restart(); // 타이머 초기화
				kFactor.m_pMapEventTime->insert( std::make_pair( iEventUID, kEventTime ) );

				//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
				KTimeEventSimpleInfo kBeginEventInfo;
				kBeginEventInfo.m_iEventUID		= iEventUID;
				kBeginEventInfo.m_iEventID		= pEvent->GetEventID();
				kBeginEventInfo.m_wstrEventName = pEvent->GetEventName();
				kBeginEventInfo.m_fEventTime	= pEvent->GetEventTime();
				kFactor.m_pVecBeginEvent->push_back( kBeginEventInfo );
				//}}
			}
			else
			{
				// 캐릭터를 처음 선택한 상황이라 이벤트 데이터가 초기화 되어 있지 않다면..?
				if( mitET->second.first.m_bInit == false )
				{
					mitET->second.first.m_bInit			= true;
					mitET->second.first.m_wstrEventName	= pEvent->GetEventName();
					mitET->second.first.m_iEventID		= pEvent->GetEventID();
					mitET->second.first.m_fEventTime	= pEvent->GetEventTime();
					mitET->second.first.m_bGiveOnce		= pEvent->IsGiveOnce();
					//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
					mitET->second.first.m_bAccountEvent	= pEvent->IsAccountEvent();
					//}}
					//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
					mitET->second.first.m_bRepeatEvent	= pEvent->GetRepeatEvent();
					//}}

					mitET->second.first.m_bRepeatRewardEvent = pEvent->IsRepeatRewardEvent();
					mitET->second.first.m_iBeforeEventID = pEvent->GetBeforeEventID();


					mitET->second.first.m_bOnlySteamUser = pEvent->IsOnlySteamUserEvent();
					mitET->second.first.m_bOnlyNotSteamUser = pEvent->IsOnlyNotSteamUserEvent();

				}
			}
		}
		else
		{
			// 4-2. 이벤트 기간은 종료되었지만 유저의 이벤트는 계속 진행시킬건지 체크한다.
			if( !kFactor.m_bDelFinishEvent  &&  !pEvent->IsDelEndEvent() )
				continue;

			// 4-3. 종료된 이벤트를 유저의 이벤트 리스트에서 찾아서 삭제한다.
			mitET = kFactor.m_pMapEventTime->find( iEventUID );
			if( mitET != kFactor.m_pMapEventTime->end() )
			{
				// 진행중인 이벤트 리스트에서 삭제!
				kFactor.m_pMapEventTime->erase( iEventUID );
				if ( pEvent->IsOn() == true  )
				{
					kFactor.m_pSetCompletedEvent->insert( iEventUID );
				}

				//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
				kFactor.m_pVecEndEvent->push_back( KTimeEventSimpleInfo( iEventUID, pEvent->GetEventID() ) );
				//}}
			}
		}
	}
}

void KGameEventManager::CheckConnectTimeEvent( IN OUT ConnectEventFactorSet& kFactor )
{
	if( GetServerType() != ST_GAME )
		return;

	if( ( kFactor.m_pVecUpdatedTime == NULL  ) ||
		( kFactor.m_pVecBeginEvent == NULL  ) ||
		( kFactor.m_pVecEndEvent == NULL  ) ||
		( kFactor.m_pMapEventTime == NULL  ) ||
		( kFactor.m_pSetCompletedEvent == NULL  ) )
	{
		START_LOG( cerr, L"Invalid login-event argument. This error should not occur; please investigate immediately." )
			<< BUILD_LOG( kFactor.m_iUnitUID )
			<< BUILD_LOG( kFactor.m_pVecUpdatedTime )
			<< BUILD_LOG( kFactor.m_pVecBeginEvent )
			<< BUILD_LOG( kFactor.m_pVecEndEvent )
			<< BUILD_LOG( kFactor.m_pMapEventTime )
			<< BUILD_LOG( kFactor.m_pSetCompletedEvent )
			<< END_LOG;
		return;
	}

	kFactor.m_pVecUpdatedTime->clear();
	kFactor.m_pVecBeginEvent->clear();
	kFactor.m_pVecEndEvent->clear();

	CheckEnableConnectTimeEvent( kFactor );

	CTime tCurrentTime = CTime::GetCurrentTime();

	std::vector< int > vecCompletedEvent;
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::iterator mitET;

	for( mitET = kFactor.m_pMapEventTime->begin(); mitET != kFactor.m_pMapEventTime->end(); ++mitET )
	{
		const int iEventUID = mitET->first;
		KUserConnectTimeEventInfo& kEventInfo = mitET->second.first;
		KTimerManager& kEventTimer = mitET->second.second;

		// 1. 이미 보상을 받았는지 검사
		if( tCurrentTime.GetTime() <= kEventInfo.m_kCTime )
		{
			vecCompletedEvent.push_back( iEventUID );
			continue;
		}

		// 2. END_EVENT값은 이미 '끝나버린 이벤트'임.
		if( kEventInfo.m_fEventTime <= END_EVENT )
		{
			vecCompletedEvent.push_back( iEventUID );
			continue;
		}

		// 3. 경과 시간이 지났는지..
		if( kEventTimer.elapsed() < ( kEventInfo.m_fEventTime * 60.0 ) )
			continue;

		kEventTimer.restart();

		// 4. 보상 시간 변경
		CTime tUpdateTime;
		if( kEventInfo.m_bGiveOnce )
		{
			tUpdateTime = tCurrentTime;
			tUpdateTime += CTimeSpan( 18250, 0, 0, 0 ); // 한번만 줘야 하므로 50년을 더한다.
		}
		else
		{
			tUpdateTime = CTime( tCurrentTime.GetYear(), tCurrentTime.GetMonth(), tCurrentTime.GetDay(), 23, 59, 59 ); // 다음날 되기전에는 받지 못함.
		}

		//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
		if( kEventInfo.m_bRepeatEvent )
		{
			tUpdateTime = CTime( tCurrentTime.GetYear(), tCurrentTime.GetMonth(), tCurrentTime.GetDay(), tCurrentTime.GetHour(), tCurrentTime.GetMinute(), 0 ); // 바로 받을 수 있다
		}
		//}}

		kEventInfo.m_kCTime = tUpdateTime.GetTime();

		// 5. DB에 업데이트
		KConnectTimeEventInfo kConnectTimeEvent;
		kConnectTimeEvent.m_iEventUID		= iEventUID;
		kConnectTimeEvent.m_wstrEventTime	= ( CStringW )tUpdateTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) );
		//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
		kConnectTimeEvent.m_bAccountEvent	= kEventInfo.m_bAccountEvent;
		//}}

		//{{ 2012. 08. 14	박세훈	대천사의 주화 이벤트 가이드 문구 출력
		//}}

		kFactor.m_pVecUpdatedTime->push_back( kConnectTimeEvent );

		// 6. 클라이언트에 종료되는 이벤트 보내기
		kFactor.m_pVecEndEvent->push_back( KTimeEventSimpleInfo( iEventUID, kEventInfo.m_iEventID ) );

		// 7. 완료 이벤트 리스트에 넣기
		//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
		if( kEventInfo.m_bRepeatEvent == false )
		{
			vecCompletedEvent.push_back( iEventUID );
		}
		else
		{
			KTimeEventSimpleInfo kRepeatEvent;
			kRepeatEvent.m_iEventUID = iEventUID;
			kRepeatEvent.m_iEventID = kEventInfo.m_iEventID;
			kRepeatEvent.m_wstrEventName = kEventInfo.m_wstrEventName;
			kRepeatEvent.m_fEventTime = kEventInfo.m_fEventTime;
			kFactor.m_pVecBeginEvent->push_back( kRepeatEvent );

			mitET->second.second.restart();
		}
		//}}
	}

	std::vector< int >::const_iterator vit;
	for( vit = vecCompletedEvent.begin(); vit != vecCompletedEvent.end(); ++vit )
	{
		// 진행중인 이벤트 리스트에서 삭제
		kFactor.m_pMapEventTime->erase( *vit );

		// 완료 이벤트 리스트에 추가
		kFactor.m_pSetCompletedEvent->insert( *vit );
	}
}
//}}
//}}

//{{ 2009. 12. 7  최육사	누적시간이벤트

bool KGameEventManager::GetCumulativeTimeEventReward( IN const int iEventUID, OUT int& iRewardID )
{
	std::map< int, KGameEvent* >::const_iterator mit = m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].find( iEventUID );
	if( mit == m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].end() )
		return false;

	if( mit->second == NULL )
		return false;

	if( mit->second->GetType() != KGameEvent::GET_CUMULATIVE_TIME )
		return false;

	KGameEventCumulativeTime* pEvent = static_cast<KGameEventCumulativeTime*>(mit->second);

	iRewardID = pEvent->GetRewardID();
	return true;
}

void KGameEventManager::CheckEnableCumulativeTimeEventForCharInit( IN const u_char ucLevel,
																   IN const bool bIsPcBang,
																   //{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
																   IN const int iChannelCode,
																   //}}
																   //{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
																   //}}
																   IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapEventTime,
																   IN OUT std::set< int >& setCompleteEvent )
{
	std::vector< KTimeEventSimpleInfo > vecDummy1;
	std::vector< KTimeEventSimpleInfo > vecDummy2;
	std::vector< KUserCumulativeTimeEventInfo > vecCompleteEventInfo;

	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	CheckEnableAndCompleteCumulativeTimeEvent( ucLevel, bIsPcBang, iChannelCode, mapEventTime, setCompleteEvent, vecCompleteEventInfo, vecDummy1, vecDummy2 );
	//}}
	//}}

	// 초기화 되지 못한 이벤트는 이미 종료된 이벤트로 판단하여 삭제처리한다.
	std::vector< int > vecEndEvent;

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = mapEventTime.begin(); mit != mapEventTime.end(); ++mit )
	{
		if( mit->second.first.m_bInit == false )
		{
			// 삭제할 리스트에 저장
			vecEndEvent.push_back( mit->first );
		}
	}

	std::vector< int >::const_iterator vit;
	for( vit = vecEndEvent.begin(); vit != vecEndEvent.end(); ++vit )
	{
		mapEventTime.erase( *vit );
	}
}

void KGameEventManager::CheckEnableAndCompleteCumulativeTimeEvent( IN const u_char /*ucLevel*/,
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
									   							   )
{
	if( GetServerType() != ST_GAME )
		return;

	vecCompleteEventInfo.clear();
	vecBeginEvent.clear();
	vecEndEvent.clear();


	std::map< int, KGameEvent* >::const_iterator mit;
	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::iterator mitET;

	// 새로 시작되는 이벤트가 있으면 추가하고 종료되는 이벤트가 있으면 삭제
	for( mit = m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].begin(); mit != m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_CUMULATIVE_TIME )
			continue;

		const int iEventUID = mit->first;
		KGameEventCumulativeTime* pEvent = static_cast<KGameEventCumulativeTime*>(mit->second);

		// 3. 이벤트가 진행중인지..
		if( mit->second->IsEnable()
			&& mit->second->IsOn() == true
			)
		{
			//{{ 2009. 10. 26  최육사	PC방이벤트
			if( pEvent->IsPcBangEvent() == true  &&  bIsPcBang == false )
				continue;
			//}}

			//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
			if( pEvent->GetChannelCode() != KNexonAccountInfo::CE_NONE )
			{
				if( pEvent->GetChannelCode() != iChannelCode )
					continue;
			}
			//}}

			//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
			//}}

			// 이미 완료한 이벤트 인지 검사!
			if( setCompleteEvent.find( iEventUID ) != setCompleteEvent.end() )
				continue;

			//{{ 2009. 12. 9  최육사	접속시간이벤트
			//if( ucLevel < 10 ) // 10레벨보다 낮으면 이벤트 적용안됨
			//	continue;
			//}}

			// 4-1. 진행중인 이벤트라면 유저의 이벤트 리스트를 확인 후 없는 이벤트라면 채워준다.
			mitET = mapEventTime.find( iEventUID );
			if( mitET == mapEventTime.end() )
			{
				std::pair< KUserCumulativeTimeEventInfo, KTimerManager > kEventTime;
				kEventTime.first.m_bInit			= true; // 새로 넣는것이기 때문에 초기화한것으로 한다.
				kEventTime.first.m_iEventUID		= iEventUID;
				kEventTime.first.m_iEventID			= pEvent->GetEventID();
				kEventTime.first.m_wstrEventName	= pEvent->GetEventName();
				kEventTime.first.m_fEventTime		= pEvent->GetEventTime();
				kEventTime.first.m_bDirectReward	= pEvent->IsDirectReward();
				//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
				kEventTime.first.m_bAccountEvent	= pEvent->IsAccountEvent();
				//}}
				kEventTime.second.restart(); // 타이머 초기화
				mapEventTime.insert( std::make_pair( iEventUID, kEventTime ) );

				//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
				KTimeEventSimpleInfo kBeginEventInfo;
				kBeginEventInfo.m_iEventUID		= iEventUID;
				kBeginEventInfo.m_iEventID		= pEvent->GetEventID();
				kBeginEventInfo.m_wstrEventName = pEvent->GetEventName();
				kBeginEventInfo.m_fEventTime	= pEvent->GetEventTime();
				vecBeginEvent.push_back( kBeginEventInfo );
				//}}
			}
			else
			{
				// 캐릭터를 처음 선택한 상황이라 이벤트 데이터가 초기화 되어 있지 않다면..?
				if( mitET->second.first.m_bInit == false )
				{
					mitET->second.first.m_bInit			= true;
					mitET->second.first.m_iEventUID		= iEventUID;
					mitET->second.first.m_iEventID		= pEvent->GetEventID();
					mitET->second.first.m_wstrEventName	= pEvent->GetEventName();
					mitET->second.first.m_fEventTime	= pEvent->GetEventTime();
					mitET->second.first.m_bDirectReward	= pEvent->IsDirectReward();
					//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
					mitET->second.first.m_bAccountEvent	= pEvent->IsAccountEvent();
					//}}
				}
				else
				{
					// 이벤트가 완료 되었나 체크!
					if( static_cast<double>(mitET->second.first.m_fEventTime * 60.0 ) < mitET->second.second.elapsed() )
					{
						vecCompleteEventInfo.push_back( mitET->second.first );
					}
				}
			}
		}
		else
		{
			// 4-3. 종료된 이벤트를 유저의 이벤트 리스트에서 찾아서 삭제한다.
			mitET = mapEventTime.find( iEventUID );
			if( mitET != mapEventTime.end() )
			{
				mapEventTime.erase( iEventUID );

				if ( pEvent->IsOn() == true ) // 2013.06.11 lygan_조성욱 // 접속 이벤트가 돌아가다가 문제 생겨서 바로 끌 경우 DB에서 off 처리 된건 완료쪽에 넣지 않는다. 문제 수정후 해당 이벤트를 다시 켜줘야 하기 때문. 수정 후 신규 등록하면 한번 더 받을수 있다.
				{
					// 완료된 이벤트에 추가
					setCompleteEvent.insert( iEventUID );
				}


				//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
				vecEndEvent.push_back( KTimeEventSimpleInfo( iEventUID, pEvent->GetEventID() ) );
				//}}
			}
		}
	}
}

void KGameEventManager::CheckCumulativeTimeEvent( IN const u_char ucLevel,
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
								 				  )
{
	if( GetServerType() != ST_GAME )
		return;

	vecUpdatedTime.clear();
	vecBeginEvent.clear();
	vecEndEvent.clear();

	std::vector< KUserCumulativeTimeEventInfo > vecCompleteEventInfo;

	// 새로 시작되는 이벤트가 있으면 추가하고 종료되는 이벤트가 있으면 삭제
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	CheckEnableAndCompleteCumulativeTimeEvent(
		ucLevel
		, bIsPcBang
		, iChannelCode
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}
		, mapEventTime
		, setCompleteEvent
		, vecCompleteEventInfo
		, vecBeginEvent
		, vecEndEvent
		);
	//}}

	std::vector< KUserCumulativeTimeEventInfo >::const_iterator vit;
	for( vit = vecCompleteEventInfo.begin(); vit != vecCompleteEventInfo.end(); ++vit )
	{
		// 직접 보상받는 이벤트인지 확인한다
		if( vit->m_bDirectReward )
		{
			// 이미 있으면 통과
			if( setDirectRewardEvent.find( vit->m_iEventUID ) != setDirectRewardEvent.end() )
				continue;

			setDirectRewardEvent.insert( vit->m_iEventUID );

			// 클라이언트에 이벤트 종결을 알리기 위해 담는다.
			vecEndEvent.push_back( KTimeEventSimpleInfo( vit->m_iEventUID, vit->m_iEventID ) );
			continue;
		}

		// 보상 받을 정보
		KCumulativeTimeEventInfo kCumulativeTimeEvent;
		kCumulativeTimeEvent.m_iEventUID		= vit->m_iEventUID;
		kCumulativeTimeEvent.m_iCumulativeTime	= static_cast<int>(vit->m_fEventTime); // 보상받는 타임을 DB에 업데이트
		//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
		kCumulativeTimeEvent.m_bAccountEvent	= vit->m_bAccountEvent;
		//}}
		vecUpdatedTime.push_back( kCumulativeTimeEvent );

		// 이벤트 삭제!
		mapEventTime.erase( vit->m_iEventUID );

		// 완료된 이벤트에 추가
		setCompleteEvent.insert( vit->m_iEventUID );

		//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
		vecEndEvent.push_back( KTimeEventSimpleInfo( vit->m_iEventUID, vit->m_iEventID ) );
		//}}
	}
}

void KGameEventManager::GetDBUpdateInfo( IN const std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
										 IN const std::set< int >& setCompletedEvent,
										 OUT std::vector< KCumulativeTimeEventInfo >& vecEventTime )
{
	vecEventTime.clear();

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = mapCumulativeEventTime.begin(); mit != mapCumulativeEventTime.end(); ++mit )
	{
		// 초기화가 되지 않은 이벤트는 이미 종료된 이벤트이므로 DB에 업데이트 할 필요가 없다.
		if( mit->second.first.m_bInit == false )
			continue;

		// 완료한 이벤트는 DB에 업데이트할 필요가 없다.
		if( setCompletedEvent.find( mit->first ) != setCompletedEvent.end() )
			continue;

		const KTimerManager& kEventTimer = mit->second.second;

		KCumulativeTimeEventInfo kInfo;
		kInfo.m_iEventUID = mit->first;
		kInfo.m_iCumulativeTime = static_cast<int>( kEventTimer.elapsed() / 60.0 );
		vecEventTime.push_back( kInfo );
	}
}

void KGameEventManager::GetNowProcessingTimeEvent( IN const std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >& mapConnectEventTime,
												   IN const std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
												   IN const std::set< int >& setCompletedEvent,
												   OUT std::vector< KTimeEventSimpleInfo >& vecTimeEvent,
												   IN bool /*bAccountTimeEvent*/ )
{
	vecTimeEvent.clear();

	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = mapConnectEventTime.begin(); mit != mapConnectEventTime.end(); ++mit )
	{
		const KUserConnectTimeEventInfo& kConnectEventInfo = mit->second.first;

		KTimeEventSimpleInfo kInfo;
		kInfo.m_iEventUID	  = mit->first;
		kInfo.m_iEventID	  = kConnectEventInfo.m_iEventID;
		kInfo.m_wstrEventName = kConnectEventInfo.m_wstrEventName;
		kInfo.m_fEventTime	  = kConnectEventInfo.m_fEventTime;
		vecTimeEvent.push_back( kInfo );
	}

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::const_iterator mitCT;
	for( mitCT = mapCumulativeEventTime.begin(); mitCT != mapCumulativeEventTime.end(); ++mitCT )
	{
		// 완료한 이벤트는 클라이언트에 줄필요 없다.
		if( setCompletedEvent.find( mitCT->first ) != setCompletedEvent.end() )
			continue;

		const KUserCumulativeTimeEventInfo& kCumulativeEventInfo = mitCT->second.first;
		const KTimerManager& kEventTimer = mitCT->second.second;

		// 초기화 되지 않은 이벤트는 클라이언트에 줄필요 없다.
		if( !kCumulativeEventInfo.m_bInit )
			continue;

		KTimeEventSimpleInfo kInfo;
		kInfo.m_iEventUID	  = mitCT->first;
		kInfo.m_iEventID	  = kCumulativeEventInfo.m_iEventID;
		kInfo.m_wstrEventName = kCumulativeEventInfo.m_wstrEventName;
		kInfo.m_fEventTime	  = kCumulativeEventInfo.m_fEventTime - static_cast<float>( kEventTimer.elapsed() / 60.0 );
		if( kInfo.m_fEventTime < 0.0f )
			kInfo.m_fEventTime = 0.0f;
		vecTimeEvent.push_back( kInfo );
	}
}

void KGameEventManager::DeleteDirectRewardAndGetDBUpdateInfo( IN OUT std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >& mapCumulativeEventTime,
										 					  IN OUT std::set< int >& setCompletedEvent,
															  IN OUT std::set< int >& setDirectRewardEvent,
															  OUT std::vector< KCumulativeTimeEventInfo >& vecCompleteEvent )
{
	vecCompleteEvent.clear();

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::iterator mit;
	std::set< int >::const_iterator sit;
	for( sit = setDirectRewardEvent.begin(); sit != setDirectRewardEvent.end(); ++sit )
	{
		mit = mapCumulativeEventTime.find( *sit );
		if( mit == mapCumulativeEventTime.end() )
			continue;

		KUserCumulativeTimeEventInfo& kCumulativeEventInfo = mit->second.first;

		// DB에 업데이트할 정보 저장
		KCumulativeTimeEventInfo kInfo;
		kInfo.m_iEventUID = kCumulativeEventInfo.m_iEventUID;
		kInfo.m_iCumulativeTime = static_cast<int>(kCumulativeEventInfo.m_fEventTime);
		vecCompleteEvent.push_back( kInfo );

		// 진행중인 이벤트에서 삭제
		mapCumulativeEventTime.erase( mit );

		// 완료 이벤트에 추가
		setCompletedEvent.insert( *sit );
	}

	// DirectReward 초기화
	setDirectRewardEvent.clear();
}

//}}

//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
void KGameEventManager::RefreshEventScript()
{
	START_LOG( cout, L"Applying live event-script patch." );

	for( int iGET = 0; iGET < KGameEvent::GET_MAX; ++iGET )
	{
		switch( iGET )
		{
		case KGameEvent::GET_ITEM_DROP:
		case KGameEvent::GET_REWARD:
		case KGameEvent::GET_CONNECT_TIME:
		case KGameEvent::GET_CUMULATIVE_TIME:
			//{{ 2011. 08. 12   김민성      헤니르 개편
		case KGameEvent::GET_HENIR_REWARD:
			//}}
			//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
		case KGameEvent::GET_QUEST_ITEM_DROP:
			//}
		case KGameEvent::GET_DEFENSE_DUNGEON:


			break;

		default:
			continue;
		}


		std::map< int, KGameEvent* >::iterator mit;
		for( mit = m_mapIngEventData[iGET].begin(); mit != m_mapIngEventData[iGET].end(); ++mit )
		{
			if( mit->second == NULL )
				continue;

			switch( mit->second->GetType() )
			{
				//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장
			case KGameEvent::GET_ITEM_DROP:
				{
					KGameEventItemDrop* pEvent = static_cast<KGameEventItemDrop*>(mit->second);

					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );

					//}}
					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetDropRate() != pEventData->m_fDropRate  ||
						pEvent->IsWithPlayPcBang() != pEventData->m_bWithPlayPcBang )
					{
						pEvent->SetDropRate( pEventData->m_fDropRate );
						pEvent->SetWithPlayPcBang( pEventData->m_bWithPlayPcBang );

						START_LOG( cout, L"Drop event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetDropRate() )
							<< BUILD_LOG( pEvent->IsWithPlayPcBang() );
					}
				}
				break;
				//}}

			case KGameEvent::GET_REWARD:
				{
					KGameEventReward* pEvent = static_cast<KGameEventReward*>(mit->second);

					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );

					//}}
					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetEXPRate()			!= pEventData->m_fEXPRate			||
						pEvent->GetVPRate()				!= pEventData->m_fVPRate			||
						pEvent->GetEDRate()				!= pEventData->m_fEDRate			||
						pEvent->GetRankingPointRate()	!= pEventData->m_fRankingPointRate  ||
						pEvent->GetDungeonID()			!= pEventData->m_iDungeonID			||
						pEvent->GetUnitLevel()			!= pEventData->m_iUnitLevel			||
						pEvent->GetUnitType()			!= pEventData->m_cUnitType			||
						pEvent->GetUnitClass()			!= pEventData->m_cUnitClass			||
						//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
						pEvent->GetServerGroupID()		!= pEventData->m_iServerGroup		||
						//}}
						pEvent->GetPartyNum()			!= pEventData->m_iPartyNum

						|| pEvent->IsPcBangEvent()		!= pEventData->m_bPcBangEvent
						)
					{
						//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
						pEvent->SetEXPRate( pEventData->m_fEXPRate );
						pEvent->SetVPRate( pEventData->m_fVPRate );
						pEvent->SetEDRate( pEventData->m_fEDRate );
						pEvent->SetRankingPointRate( pEventData->m_fRankingPointRate );
						pEvent->SetDungeonID( pEventData->m_iDungeonID );
						pEvent->SetUnitLevel( pEventData->m_iUnitLevel );
						pEvent->SetUnitType( pEventData->m_cUnitType );
						pEvent->SetUnitClass( pEventData->m_cUnitClass );
						pEvent->SetPartyNum( pEventData->m_iPartyNum );
						pEvent->SetServerGroupID( pEventData->m_iServerGroup );
						//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
						pEvent->SetAccumulateEXP( pEventData->m_bAccumulateEXP );
						//}}

						pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );

						pEvent->Dump();

						START_LOG( cout, L"Dungeon reward event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetEXPRate() )
							<< BUILD_LOG( pEvent->GetVPRate() )
							<< BUILD_LOG( pEvent->GetEDRate() )
							<< BUILD_LOG( pEvent->GetRankingPointRate() )
							<< BUILD_LOG( pEvent->GetDungeonID() )
							<< BUILD_LOG( pEvent->GetUnitLevel() )
							<< BUILD_LOGc( pEvent->GetUnitType() )
							<< BUILD_LOGc( pEvent->GetUnitClass() )
							<< BUILD_LOG( pEvent->GetPartyNum() )
							<< BUILD_LOG( pEvent->GetServerGroupID() )
							//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
							<< BUILD_LOG( pEvent->IsAccumulateEXP() )
							//}}
							<< BUILD_LOG( pEvent->IsPcBangEvent() )
							;
						//}}
					}
				}
				break;

			case KGameEvent::GET_CONNECT_TIME:
				{
					KGameEventConnectTime* pEvent = static_cast<KGameEventConnectTime*>(mit->second);

					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );

					//}}
					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetEventTime() != pEventData->m_fEventTime  ||
						pEvent->GetRewardID() != pEventData->m_iEventReward  ||
						pEvent->IsGiveOnce() != pEventData->m_bGiveOnce  ||
						pEvent->IsDelEndEvent() != pEventData->m_bDelEndEvent  ||
						pEvent->IsPcBangEvent() != pEventData->m_bPcBangEvent
						//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
						|| pEvent->IsAccountEvent() != pEventData->m_bAccountEvent
						//}}
						//{{ 2009. 12. 16  최육사	캐릭터 레벨도 조건 추가
						|| pEvent->GetUnitLevel() != pEventData->m_iUnitLevel
						//}}
						//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
						|| pEvent->GetRepeatEvent() != pEventData->m_bRepeatEvent
						//}}
						//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
						|| pEvent->GetMinMaxRewardSize() != static_cast<int>( pEventData->m_mapMinMaxReward.size() )
						//}}
						|| pEvent->IsRepeatRewardEvent() != pEventData->m_bRepeatRewardEvent
						|| pEvent->GetBeforeEventID() != ( pEventData->m_iBeforeEventID )
						|| pEvent->GetEventItemID() != pEventData->m_iEventItemID
						|| pEvent->IsOnlySteamUserEvent() != ( pEventData->m_bOnlySteamUser )
						|| pEvent->IsOnlyNotSteamUserEvent() != ( pEventData->m_bOnlyNotSteamUser )
						|| ( pEvent->GetUnitClassLevel() != pEventData->m_iUnitClassLevel )
						)
					{
						//이벤트 정보셋팅..
						pEvent->SetEventTime( pEventData->m_fEventTime );
						pEvent->SetRewardID( pEventData->m_iEventReward );
						pEvent->SetGiveOnce( pEventData->m_bGiveOnce );
						pEvent->SetDelEndEvent( pEventData->m_bDelEndEvent );
						//{{ 2009. 10. 26  최육사	PC방이벤트
						pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );
						//}}
						//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
						pEvent->SetAccountEvent( pEventData->m_bAccountEvent );
						//}}
						//{{ 2009. 12. 16  최육사	캐릭터 레벨도 조건 추가
						pEvent->SetUnitLevel( pEventData->m_iUnitLevel );
						//}}
						//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
						pEvent->SetMinMaxReward( pEventData->m_mapMinMaxReward );
						//}}
						//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
						pEvent->SetRepeatEvent( pEventData->m_bRepeatEvent );
						//}}
						//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
						pEvent->SetEventGroupID( pEventData->m_iEventGroupID );
						//}}
						pEvent->SetRepeatRewardEvent(pEventData->m_bRepeatRewardEvent);
						pEvent->SetBeforeEventID(pEventData->m_iBeforeEventID );
						pEvent->SetEventItemID( pEventData->m_iEventItemID );
						pEvent->SetOnlySteamUserEvent( pEventData->m_bOnlySteamUser );
						pEvent->SetOnlyNotSteamUserEvent( pEventData->m_bOnlyNotSteamUser );
						pEvent->SetUnitClassLevel( pEventData->m_iUnitClassLevel );

						START_LOG( cout, L"Login-time event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetEventTime() )
							<< BUILD_LOG( pEvent->GetRewardID() )
							<< BUILD_LOG( pEvent->IsGiveOnce() )
							<< BUILD_LOG( pEvent->IsDelEndEvent() )
							<< BUILD_LOG( pEvent->IsPcBangEvent() )
							<< BUILD_LOG( pEvent->IsAccountEvent() )
							<< BUILD_LOG( pEvent->GetUnitLevel() )
							//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
							<< BUILD_LOG( pEvent->GetMinMaxRewardSize() )
							//}}
							//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
							<< BUILD_LOG( pEvent->GetRepeatEvent() )
							//}}
							//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
							<< BUILD_LOG( pEvent->GetEventGroupID() )
							//}}
							<< BUILD_LOG( pEvent->IsRepeatRewardEvent() )
							<< BUILD_LOG( pEvent->GetBeforeEventID() )
							<< BUILD_LOG( pEvent->GetEventItemID() )
							<< BUILD_LOG( pEvent->IsOnlySteamUserEvent() )
							<< BUILD_LOG( pEvent->IsOnlyNotSteamUserEvent() )
							<< BUILD_LOG( pEvent->GetUnitClassLevel() )

							<< END_LOG;
							;
					}
				}
				break;

				//{{ 2009. 12. 7  최육사	누적시간이벤트
			case KGameEvent::GET_CUMULATIVE_TIME:
				{
					KGameEventCumulativeTime* pEvent = static_cast<KGameEventCumulativeTime*>(mit->second);

					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )

					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );

					//}}
					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetEventTime() != pEventData->m_fEventTime  ||
						pEvent->GetRewardID() != pEventData->m_iEventReward  ||
						pEvent->IsPcBangEvent() != pEventData->m_bPcBangEvent  ||
						pEvent->IsDirectReward() != pEventData->m_bDirectReward
						//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
						|| pEvent->IsAccountEvent() != pEventData->m_bAccountEvent
						//}}
						)
					{
						//이벤트 정보셋팅..
						pEvent->SetEventTime( pEventData->m_fEventTime );
						pEvent->SetRewardID( pEventData->m_iEventReward );
						//{{ 2009. 10. 26  최육사	PC방이벤트
						pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );
						//}}
						//{{ 2009. 12. 8  최육사	접속시간이벤트
						pEvent->SetDirectReward( pEventData->m_bDirectReward );
						//}}
						//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
						pEvent->SetAccountEvent( pEventData->m_bAccountEvent );
						//}}

						START_LOG( cout, L"Cumulative-time event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetEventTime() )
							<< BUILD_LOG( pEvent->GetRewardID() )
							<< BUILD_LOG( pEvent->IsDirectReward() )
							<< BUILD_LOG( pEvent->IsPcBangEvent() )
							<< BUILD_LOG( pEvent->IsAccountEvent() )
							<< END_LOG;
					}
				}
				break;
				//}}

				//{{ 2011. 08. 12   김민성      헤니르 개편
			case KGameEvent::GET_HENIR_REWARD:
				{
					KGameEventHenirReward* pEvent = static_cast<KGameEventHenirReward*>(mit->second);
					//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )

					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );

					//}}
					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetHenirRewardEventCount() != pEventData->m_iHenirRewardEventCount ||
						//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
						pEvent->GetServerGroupID() != pEventData->m_iServerGroup  ||
						//}}
						pEvent->GetHenirRewardUnLimited() != pEventData->m_bHenirRewardUnLimited
						)
					{
						//이벤트 정보셋팅..
						pEvent->SetHenirRewardEventCount( pEventData->m_iHenirRewardEventCount );
						pEvent->SetHenirRewardUnLimited( pEventData->m_bHenirRewardUnLimited );
						//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
						pEvent->SetServerGroupID( pEventData->m_iServerGroup );
						//}}


						START_LOG( cout, L"Cumulative-time event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetHenirRewardEventCount() )
							<< BUILD_LOG( pEvent->GetHenirRewardUnLimited() )
							//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
							<< BUILD_LOG( pEvent->GetServerGroupID() )
							//}}
							;
					}
				}
				break;
				//}}

				//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
			case KGameEvent::GET_QUEST_ITEM_DROP:
				{
					KGameEventQuestItemDrop* pEvent = static_cast<KGameEventQuestItemDrop*>(mit->second);

					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );


					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetQuestItemDropRate() != pEventData->m_fQuestItemDropRate )
					{
						pEvent->SetQuestItemDropRate( pEventData->m_fQuestItemDropRate );

						START_LOG( cout, L"Drop event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetQuestItemDropRate() );
					}
				}
				break;
				//}

			case KGameEvent::GET_DEFENSE_DUNGEON:
				{
					KGameEventDefenseDungeon* pEvent = static_cast<KGameEventDefenseDungeon*>( mit->second );
					const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( pEvent->GetScriptID() );


					if( pEventData == NULL )
					{
						START_LOG( cerr, L"Event script information is missing." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( mit->second->GetType() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< END_LOG;
						break;
					}

					if( pEvent->GetDefenseDungeonOpenRate() != pEventData->m_fDefenseDungeonOpenRate )
					{
						pEvent->SetDefenseDungeonOpenRate( pEventData->m_fDefenseDungeonOpenRate );

						START_LOG( cout, L"Gate of Darkness opening-rate event script updated." )
							<< BUILD_LOG( mit->second->GetUID() )
							<< BUILD_LOG( pEvent->GetEventName() )
							<< BUILD_LOG( pEvent->GetScriptID() )
							<< BUILD_LOG( pEvent->GetDefenseDungeonOpenRate() );
					}
				}
				break;

			default:
				break;
			}
		}
	}
}
//}}
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}
bool KGameEventManager::RefreshEventTime( IN const KEventInfo& kInfo, OUT KGameEvent* pkGameEvent )
{
	if( pkGameEvent == NULL )
	{
		START_LOG( cerr, L"Invalid event object pointer." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< END_LOG;

		return false;
	}

	pkGameEvent->SetEventName( kInfo.m_wstrEventName );

	CTime ctBegin, ctEnd;

	if( KncUtil::ConvertStringToCTime( kInfo.m_wstrBeginDate, ctBegin ) == false )
	{
		START_LOG( cerr, L"Failed to assemble the time string." )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< END_LOG;

		return false;
	}

	if( KncUtil::ConvertStringToCTime( kInfo.m_wstrEndDate, ctEnd ) == false )
	{
		START_LOG( cerr, L"Failed to assemble the time string." )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< END_LOG;

		return false;
	}

	std::wstring wstrTime;

	//이벤트 시작시간 수정
	wstrTime = ( CStringW )( pkGameEvent->GetBeginDate().Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	if( pkGameEvent->GetBeginDate() != ctBegin )
	{
		START_LOG( cout2, L"Event start time updated." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( wstrTime )
			;

		pkGameEvent->SetBeginDate( ctBegin );
	}

	//이벤트 종료시간 수정.
	wstrTime = ( CStringW )( pkGameEvent->GetEndDate().Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	if( pkGameEvent->GetEndDate() != ctEnd )
	{
		START_LOG( cout2, L"Event end time updated." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( wstrTime )
			;

		pkGameEvent->SetEndDate( ctEnd );
	}

	pkGameEvent->SetOn(kInfo.m_bOn);

	return true;
}

bool KGameEventManager::SetPeriodEventTime( IN const KEventInfo& kInfo, OUT CTime& ctBegin, OUT CTime& ctEnd )
{
	if( KncUtil::ConvertStringToCTime( kInfo.m_wstrBeginDate, ctBegin ) == false )
	{
		START_LOG( cerr, L"Failed to assemble the time string." )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< END_LOG;

		return false;
	}

	if( KncUtil::ConvertStringToCTime( kInfo.m_wstrEndDate, ctEnd ) == false )
	{
		START_LOG( cerr, L"Failed to assemble the time string." )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< END_LOG;

		return false;
	}

	return true;
}

void KGameEventManager::AddQuestEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_QUEST].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_QUEST].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEventQuest* pEvent = new KGameEventQuest();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 02. 05  최육사	일일 이벤트 퀘스트
		pEvent->SetQuestID( kInfo.m_iScriptID );
		//}}
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_QUEST].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		pEvent->SetScriptID(kInfo.m_iScriptID);

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//{{ 2009. 5. 22  최육사	드롭률이벤트
void KGameEventManager::AddItemDropEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_ITEM_DROP].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_ITEM_DROP].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}


		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );

		if( pEventData == NULL )
		//}}
		//}}
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventItemDrop* pEvent = new KGameEventItemDrop();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 07. 09  최육사	드롭률 이벤트 확장

		pEvent->SetDropRate( pEventData->m_fDropRate );

		pEvent->SetWithPlayPcBang( pEventData->m_bWithPlayPcBang );
		//}}

		//{{ 2012. 03. 28	박세훈	드롭률 설정 이벤트 개편 Merge 작업 ( 2012.03.20 lygan_조성욱 )

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_ITEM_DROP].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}
//}}

void KGameEventManager::AddRewardEvent( IN const KEventInfo& kInfo )
{
	//{{ 2010. 05. 11  최육사	대전 던전 서버군 통합
	if( GetServerType() != ST_GAME )
		return;
	//}}

	std::map<int,KGameEvent*>::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_REWARD].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_REWARD].end() )
	{
        if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면..
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventReward* pEvent = new KGameEventReward();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
		pEvent->SetEXPRate( pEventData->m_fEXPRate );
		pEvent->SetVPRate( pEventData->m_fVPRate );
		pEvent->SetEDRate( pEventData->m_fEDRate );
		pEvent->SetRankingPointRate( pEventData->m_fRankingPointRate );
		pEvent->SetDungeonID( pEventData->m_iDungeonID );
		pEvent->SetUnitLevel( pEventData->m_iUnitLevel );
		pEvent->SetUnitType( pEventData->m_cUnitType );
		pEvent->SetUnitClass( pEventData->m_cUnitClass );
		pEvent->SetPartyNum( pEventData->m_iPartyNum );
		pEvent->SetServerGroupID( pEventData->m_iServerGroup );
		//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
		pEvent->SetAccumulateEXP( pEventData->m_bAccumulateEXP );
		//}}
		//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
		//}}

		pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );

		pEvent->Dump();
		//}}

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_REWARD].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
            << BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//{{ 2010. 07. 16  최육사	이벤트 몬스터 시스템

void KGameEventManager::AddMonsterEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_MONSTER].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_MONSTER].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEventMonster* pEvent = new KGameEventMonster();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetEventID( kInfo.m_iScriptID );

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_MONSTER].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		pEvent->SetScriptID( kInfo.m_iScriptID );
		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//}}

void KGameEventManager::AddTournamentEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME && GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_TOURNAMENT].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_TOURNAMENT].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEventTournament* pEvent = new KGameEventTournament();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_TOURNAMENT].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

void KGameEventManager::AddSpiritEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_SPIRIT].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_SPIRIT].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEventTournament* pEvent = new KGameEventTournament();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_SPIRIT].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)
		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//{{ 2008. 6. 9  최육사  아이템 이벤트
void KGameEventManager::AddConnectTimeEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면..
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventConnectTime* pEvent = new KGameEventConnectTime();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetEventID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetEventTime( pEventData->m_fEventTime );
		pEvent->SetRewardID( pEventData->m_iEventReward );
		pEvent->SetGiveOnce( pEventData->m_bGiveOnce );
		pEvent->SetDelEndEvent( pEventData->m_bDelEndEvent );
		//{{ 2009. 10. 26  최육사	PC방이벤트
		pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );
		//}}
		//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
		pEvent->SetAccountEvent( pEventData->m_bAccountEvent );
		//}}
		//{{ 2009. 12. 16  최육사	캐릭터 레벨도 조건 추가
        pEvent->SetUnitLevel( pEventData->m_iUnitLevel );
		//}}
		// 최육사	접속 이벤트 레벨 체크
		pEvent->SetMinMaxReward( pEventData->m_mapMinMaxReward );
		//}}
		//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
		pEvent->SetChannelCode( pEventData->m_iChannelCode );
		//}}
		//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
		//}}
		//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
		pEvent->SetUnitType( pEventData->m_cUnitType );
		pEvent->SetUnitClass( pEventData->m_cUnitClass );
		//}}
		//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
		pEvent->SetRepeatEvent( pEventData->m_bRepeatEvent );
		//}}
		//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
		pEvent->SetEventGroupID( pEventData->m_iEventGroupID );
		//}}

		pEvent->SetOn(kInfo.m_bOn);

		pEvent->SetRepeatRewardEvent(pEventData->m_bRepeatRewardEvent);
		pEvent->SetBeforeEventID(pEventData->m_iBeforeEventID );

		pEvent->SetEventItemID( pEventData->m_iEventItemID );


		pEvent->SetOnlySteamUserEvent( pEventData->m_bOnlySteamUser );
		pEvent->SetOnlyNotSteamUserEvent( pEventData->m_bOnlyNotSteamUser );


		pEvent->SetUnitClassLevel( pEventData->m_iUnitClassLevel );

		m_mapIngEventData[KGameEvent::GET_CONNECT_TIME].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)


		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( pEventData->m_iEventReward )
			<< BUILD_LOG( pEventData->m_fEventTime )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}
//}}

void KGameEventManager::AddSecretDungeonEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_SECRET_DUNGEON].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_SECRET_DUNGEON].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventSecretDungeon* pEvent = new KGameEventSecretDungeon();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 10. 19	최육사	비밀던전 이벤트 업데이트
		pEvent->SetDungeonID( pEventData->m_iDungeonID );
		pEvent->SetDungeonMode( pEventData->m_iDungeonMode );
		//}}

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_SECRET_DUNGEON].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		pEvent->SetScriptID(kInfo.m_iScriptID);
		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

void KGameEventManager::AddNotifyMSGEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_NOTIFY_MSG].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_NOTIFY_MSG].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEventNotifyMSG* pEvent = new KGameEventNotifyMSG();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );

		//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합
		pEvent->SetIsAllServers( ( kInfo.m_iScriptID == 1 ) );
		//}}

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_NOTIFY_MSG].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//{{ 2009. 12. 7  최육사	누적시간이벤트

void KGameEventManager::AddCumulativeTimeEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면..
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventCumulativeTime* pEvent = new KGameEventCumulativeTime();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetEventID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetEventTime( pEventData->m_fEventTime );
		pEvent->SetRewardID( pEventData->m_iEventReward );
		//{{ 2009. 10. 26  최육사	PC방이벤트
		pEvent->SetPcBangEvent( pEventData->m_bPcBangEvent );
		//}}
		//{{ 2009. 12. 8  최육사	접속시간이벤트
		pEvent->SetDirectReward( pEventData->m_bDirectReward );
		//}}
		//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
		pEvent->SetAccountEvent( pEventData->m_bAccountEvent );
		//}}
		//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
		pEvent->SetChannelCode( pEventData->m_iChannelCode );
		//}}

		pEvent->SetOn(kInfo.m_bOn);


		m_mapIngEventData[KGameEvent::GET_CUMULATIVE_TIME].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

//}}
//{{ 2011. 08. 12   김민성      헤니르 개편
void KGameEventManager::AddHenirRewardCountEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_HENIR_REWARD].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_HENIR_REWARD].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면..
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventHenirReward* pEvent = new KGameEventHenirReward();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetEventID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );

		pEvent->SetHenirRewardUnLimited( pEventData->m_bHenirRewardUnLimited );
		pEvent->SetHenirRewardEventCount( pEventData->m_iHenirRewardEventCount );
		//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
		pEvent->SetServerGroupID( pEventData->m_iServerGroup );
		//}}

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_HENIR_REWARD].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			<< END_LOG;
	}
}

bool KGameEventManager::GetHenirRewardCountEvent( OUT bool& bUnLimited, OUT int& iEventMax  )
{
	bUnLimited = false;
	iEventMax = 0;

	if( GetServerType() != ST_GAME )
		return false;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_HENIR_REWARD].begin(); mit != m_mapIngEventData[KGameEvent::GET_HENIR_REWARD].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_HENIR_REWARD )
			continue;

		KGameEventHenirReward* pEvent = static_cast<KGameEventHenirReward*>(mit->second);

		// 진행중인 드롭률 이벤트
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
#ifdef EXTEND_SERVER_GROUP_MASK
			if( pEvent->GetServerGroupID() != -1 && pEvent->GetServerGroupID() != KBaseServer::GetKObj()->GetServerGroupID() )
#else
			if( pEvent->GetServerGroupID() != SEnum::SGI_INVALID  &&  pEvent->GetServerGroupID() != KBaseServer::GetKObj()->GetServerGroupID() )
#endif // EXTEND_SERVER_GROUP_MASK
				continue;
			//}}

			//{{ 2013. 04. 14	최육사	헤니르 시공 보상 횟수 이벤트 버그 수정
			if( bUnLimited == false )
				bUnLimited = pEvent->GetHenirRewardUnLimited();

			if( iEventMax == 0 )
				iEventMax = pEvent->GetHenirRewardEventCount();
			//}}
		}
	}

	//{{ 2013. 04. 14	최육사	헤니르 시공 보상 횟수 이벤트 버그 수정
	return true;
	//}}
}
//}}

//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
//}}

//{{ 2013. 03. 18	 퀘스트 드롭 확률 증가 이벤트 - 김민성
void KGameEventManager::AddQuestItemDropEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_QUEST_ITEM_DROP].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_QUEST_ITEM_DROP].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}


		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventQuestItemDrop* pEvent = new KGameEventQuestItemDrop();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetScriptID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetQuestItemDropRate( pEventData->m_fQuestItemDropRate );

		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_QUEST_ITEM_DROP].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

bool KGameEventManager::CheckQuestItemDropProbEvent( OUT float& fRate )
{
	if( GetServerType() != ST_CENTER )
		return false;

	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_QUEST_ITEM_DROP].begin(); mit != m_mapIngEventData[KGameEvent::GET_QUEST_ITEM_DROP].end(); ++mit )
	{
		if( mit->second == NULL )
			continue;

		if( mit->second->GetType() != KGameEvent::GET_QUEST_ITEM_DROP )
			continue;

		KGameEventQuestItemDrop* pEvent = static_cast<KGameEventQuestItemDrop*>(mit->second);

		// 진행중인 드롭률 이벤트
		if( mit->second->IsEnable() == true
			&& mit->second->IsOn() == true
			)
		{
			fRate = pEvent->GetQuestItemDropRate();

			return true;
		}
	}

	return false;
}
//}

void KGameEventManager::AddAdvertisementEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_GAME )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면..
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetAdvertisementData( kInfo.m_iScriptID );
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Advertisement script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventAdvertisement* pEvent = new KGameEventAdvertisement();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetEventID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetAdvertisementURL( pEventData->m_wstrAdvertisementURL );
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

void KGameEventManager::CheckEnableAdvertisementEvent( IN OUT std::set< int >& setAdvertisementEvent, OUT std::vector< KTimeEventSimpleInfo >& vecBeginEvent, OUT std::vector< KTimeEventSimpleInfo >& vecEndEvent )
{
	if( GetServerType() != ST_GAME )
		return;

	vecBeginEvent.clear();
	vecEndEvent.clear();

	std::map< int, KGameEvent* >::const_iterator mit;

	// 새로 시작되는 이벤트가 있으면 추가하고 종료되는 이벤트가 있으면 삭제
	for( mit = m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].begin(); mit != m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_ADVERTISEMENT_EVENT )
			continue;

		const int iEventUID = mit->first;
		KGameEventAdvertisement* pEvent = static_cast<KGameEventAdvertisement*>(mit->second);

		// 3. 이벤트가 진행중인지..
		if( mit->second->IsEnable()
			&& mit->second->IsOn() == true
			)
		{
			// 이미 완료한 이벤트라면 넘어간다!
			if( setAdvertisementEvent.find( iEventUID ) != setAdvertisementEvent.end() )
				continue;

			// 4-1. 진행중인 이벤트라면 유저의 이벤트 리스트를 확인 후 없는 이벤트라면 채워준다.
			//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
			KTimeEventSimpleInfo kBeginEventInfo;
			kBeginEventInfo.m_iEventUID		= iEventUID;
			kBeginEventInfo.m_iEventID		= pEvent->GetEventID();
			kBeginEventInfo.m_wstrEventName = pEvent->GetEventName();
			kBeginEventInfo.m_wstrAdvertisementURL = pEvent->GetAdvertisementURL();
			vecBeginEvent.push_back( kBeginEventInfo );
			//}}
		}
		else
		{
			if (  mit->second->IsOn() == true )
			{
				// 완료 이벤트에 추가
				setAdvertisementEvent.insert( iEventUID );
			}

			vecEndEvent.push_back( KTimeEventSimpleInfo( iEventUID, pEvent->GetEventID() ));
		}
	}
}

void KGameEventManager::GetAdvertisementURL( OUT std::vector< std::wstring >& vecAdvertisementURL )
{
	std::map< int, KGameEvent* >::const_iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].begin(); mit != m_mapIngEventData[KGameEvent::GET_ADVERTISEMENT_EVENT].end(); ++mit )
	{
		if( mit->second->IsEnable()
			&& mit->second->IsOn() == true
			)
		{
			KGameEventAdvertisement* pEvent = static_cast<KGameEventAdvertisement*>(mit->second);
			vecAdvertisementURL.push_back( pEvent->GetAdvertisementURL() );
		}
	}
}

int KGameEventManager::GetMaxLevel()
{
	int iMaxLevel = -1;
	// maxlevel 이벤트는 가장 높은걸 이용하도록 합니다.
	std::map< int, KGameEvent* >::iterator mitIngEventData = m_mapIngEventData[KGameEvent::GET_MAX_LEVEL].begin();
	for( ; mitIngEventData != m_mapIngEventData[KGameEvent::GET_MAX_LEVEL].end(); ++mitIngEventData )
	{
		// 꺼져 있는 이벤트는 의미 없다.
		if( mitIngEventData->second->IsEnable() == false )
			continue;

		if( mitIngEventData->second->IsOn() == false )
			continue;

		iMaxLevel = std::max( mitIngEventData->second->GetScriptID(), iMaxLevel );
	}

	return iMaxLevel;
}

bool KGameEventManager::IsEnableDungeon( int nDunegonID, bool bEventDungeon )
{
	// 이벤트 던전이 아니면 무조건 켜짐
	if( bEventDungeon == false )
		return true;

	std::map< int, KGameEvent* >::iterator mitIngEventData = m_mapIngEventData[KGameEvent::GET_DUNGEON].begin();
	for( ; mitIngEventData != m_mapIngEventData[KGameEvent::GET_DUNGEON].end(); ++mitIngEventData )
	{
		// 꺼져 있는 이벤트는 의미 없다.
		if( mitIngEventData->second->IsEnable() == false )
			continue;

		// 2013.03.28 lygan_조성욱 // DB에서 기능 on / off 사항 바로 적용 되게
		if ( mitIngEventData->second->IsOn() == false )
			continue;

		if( mitIngEventData->second->GetScriptID() == nDunegonID )
		{
			START_LOG( clog, L"Enabled dungeon" )
				<< BUILD_LOG( nDunegonID )
				<< BUILD_LOG( bEventDungeon )
				;

			return true;
		}
	}

	START_LOG( clog, L"Disabled dungeon" )
		<< BUILD_LOG( nDunegonID )
		<< BUILD_LOG( bEventDungeon )
		;

	return false;
}

void KGameEventManager::CheckEnableEventDungeon( IN OUT std::map< int, bool >& mapCheckDungeon )
{
	// on/off 될 던전의 on/off 값을 리턴해 주어야 하는데...

	std::map< int, bool >::iterator mitCheckDungeon;
	for( mitCheckDungeon = mapCheckDungeon.begin(); mitCheckDungeon != mapCheckDungeon.end(); ++mitCheckDungeon )
	{
		// 우선 꺼놓고
		mitCheckDungeon->second = false;
	}

	std::map< int, KGameEvent* >::iterator mitIngEventData = m_mapIngEventData[KGameEvent::GET_DUNGEON].begin();
	for( ; mitIngEventData != m_mapIngEventData[KGameEvent::GET_DUNGEON].end(); ++mitIngEventData )
	{
		// 꺼져 있는 이벤트는 의미 없다.
		if( mitIngEventData->second->IsEnable() == false )
			continue;

		// 2013.03.28 lygan_조성욱 // DB에서 기능 on / off 사항 바로 적용 되게
		if ( mitIngEventData->second->IsOn() == false )
			continue;

		// 이벤트 진행중인 던전ID
		int iIngDungeonID = mitIngEventData->second->GetScriptID();

		// 진행 중인 던전인 경우 켜준다.
		mitCheckDungeon = mapCheckDungeon.find( iIngDungeonID );

		if( mitCheckDungeon != mapCheckDungeon.end() )
			mitCheckDungeon->second = true;

		START_LOG( clog, L"Enabled dungeon" )
			<< BUILD_LOG( iIngDungeonID )
			;
	}

}


void KGameEventManager::AddDungeonEvent( IN const KEventInfo& kInfo )
{
	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_DUNGEON].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_DUNGEON].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEvent* pEvent = new KGameEvent();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 02. 05  최육사	일일 이벤트 퀘스트
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_DUNGEON].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}
void KGameEventManager::AddMaxLevelEvent( IN const KEventInfo& kInfo )
{
	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_MAX_LEVEL].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_MAX_LEVEL].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEvent* pEvent = new KGameEvent();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 02. 05  최육사	일일 이벤트 퀘스트
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_MAX_LEVEL].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );


		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}
void KGameEventManager::AddContents( IN const KEventInfo& kInfo )
{
	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_CONTENTS].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_CONTENTS].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEvent* pEvent = new KGameEvent();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 02. 05  최육사	일일 이벤트 퀘스트
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_CONTENTS].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}


void KGameEventManager::InitEventReleaseTick()
{
	m_iEventReleaseTick = 0;
	m_iAppliedEventTick = 0;
}

bool KGameEventManager::TickCheckEvent()
{

	if( m_tEventCheckTimer.elapsed() > 60.0f )
	{
		int iReleaseTickCount = 0;

		iReleaseTickCount = GetEventReleaseTick();
		if ( iReleaseTickCount == -1 )
		{

			m_tEventCheckTimer.restart();

			return false;
		}

		SendToLogDB( DBE_CHECK_EVENT_UPDATE_REQ );

		m_tEventCheckTimer.restart();

	}

	return true;
}


void KGameEventManager::AddCodeEvent( IN const KEventInfo& kInfo )
{
	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_CODE].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_CODE].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		KGameEvent* pEvent = new KGameEvent();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		//{{ 2010. 02. 05  최육사	일일 이벤트 퀘스트
		pEvent->SetScriptID( kInfo.m_iScriptID );
		//}}
		pEvent->SetOn(kInfo.m_bOn);

		m_mapIngEventData[KGameEvent::GET_CODE].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		CStringW strBegin = ctBegin.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventBegin = strBegin.GetString();
		CStringW strEnd = ctEnd.Format(_T("%Y-%m-%d %H:%M:%S"));
		std::wstring wstrEventEnd = strEnd.GetString();
		// Port: KSMS management tool notification removed (out of scope)

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

bool KGameEventManager::IsEnableCode( IN int iEventID )
{
	std::map< int, KGameEvent* >::iterator mitIngEventData = m_mapIngEventData[KGameEvent::GET_CODE].begin();
	for( ; mitIngEventData != m_mapIngEventData[KGameEvent::GET_CODE].end(); ++mitIngEventData )
	{
		// 꺼져 있는 이벤트는 의미 없다.
		if( mitIngEventData->second->IsEnable() == false )
			continue;

		// 2013.03.28 lygan_조성욱 // DB에서 기능 on / off 사항 바로 적용 되게
		if ( mitIngEventData->second->IsOn() == false)
			continue;


		if( mitIngEventData->second->GetScriptID() == iEventID )
			return true;
	}

	return false;
}

void KGameEventManager::CheckEnableCode( OUT std::map< int, bool >& mapCheckCode )
{
	// 우선 꺼놓고
	mapCheckCode.clear();
	for( int iCodeEventID = 1; iCodeEventID < CEI_END; ++iCodeEventID )
	{
		mapCheckCode[iCodeEventID] = false;
	}

	std::map< int, KGameEvent* >::iterator mitIngEventData = m_mapIngEventData[KGameEvent::GET_CODE].begin();
	for( ; mitIngEventData != m_mapIngEventData[KGameEvent::GET_CODE].end(); ++mitIngEventData )
	{
		// 꺼져 있는 이벤트는 의미 없다.
		if( mitIngEventData->second->IsEnable() == false )
			continue;

		// 2013.03.28 lygan_조성욱 // DB에서 기능 on / off 사항 바로 적용 되게
		if ( mitIngEventData->second->IsOn() == false)
			continue;

		// 이벤트 진행중인 던전ID
		int iIngCodeID = mitIngEventData->second->GetScriptID();

		// 진행 중인 던전인 경우 켜준다.
		std::map< int, bool >::iterator mitCheckCode = mapCheckCode.find( iIngCodeID );

		if( mitCheckCode != mapCheckCode.end() )
			mitCheckCode->second = true;

		START_LOG( clog, L"Enabled code enum" )
			<< BUILD_LOG( iIngCodeID )
			;
	}
}



bool KGameEventManager::IsEnableEventQuest( IN int iQuestID )
{
	if( GetServerType() != ST_GAME )
		return false;


	const CXSLQuestManager::QuestTemplet* pQuestTemplet = SiCXSLQuestManager()->GetQuestTemplet( iQuestID );

	if( pQuestTemplet == NULL )
		return false;

	// 타임퀘스트가 아닌경우
	if ( pQuestTemplet->m_bIsTimeEvent == false )
		return true;

	// 타임퀘스트인 경우
	std::map< int, KGameEvent* >::const_iterator mit;
	for( mit = m_mapIngEventData[KGameEvent::GET_QUEST].begin(); mit != m_mapIngEventData[KGameEvent::GET_QUEST].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_QUEST )
			continue;

		KGameEventQuest* pEvent = static_cast<KGameEventQuest*>(mit->second);

		const int iEnableQuestID = pEvent->GetQuestID();

		if ( iEnableQuestID == iQuestID )
		{
			// 3. 이벤트가 진행중인지..
			if( mit->second->IsEnable() && mit->second->IsOn() == true )
			{
				return true;
			}
		}
	}

	// 타임퀘스트인데 현재 해당 이벤트가 돌아가지 않고 있다
	return false;
}


void KGameEventManager::GetWarningEventQuest( OUT KEGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT& kPacket_ )
{
	if( GetServerType() != ST_GAME )
		return;

	kPacket_.m_vecWarningQuestID.clear();

	std::wstringstream wstrMsg; // 에러 로그 용
	std::map< int, KGameEvent* >::iterator mit;

	for( mit = m_mapIngEventData[KGameEvent::GET_QUEST].begin(); mit != m_mapIngEventData[KGameEvent::GET_QUEST].end(); ++mit )
	{
		// 1. NULL 체크
		if( mit->second == NULL )
			continue;

		// 2. 이벤트 타입 체크
		if( mit->second->GetType() != KGameEvent::GET_QUEST )
			continue;

		// 3. On Off 체크
		if( false == mit->second->IsOn() )
			continue;

		// 4. Enable 체크
		if( false == mit->second->IsEnable() )
			continue;

		KGameEventQuest* pEvent = static_cast<KGameEventQuest*>(mit->second);

		const int iQuestID = pEvent->GetQuestID();
		const CXSLQuestManager::QuestTemplet* pQuestTemplet = SiCXSLQuestManager()->GetQuestTemplet( iQuestID );

		if( pQuestTemplet == NULL )
		{
			START_LOG( cout, L"[Test] A nonexistent quest ID was registered as an event." )
				<< BUILD_LOG( iQuestID );
		}
		else
		{
			if( false == pQuestTemplet->m_bIsTimeEvent )
			{
				kPacket_.m_vecWarningQuestID.push_back( iQuestID );
				wstrMsg << iQuestID << L", ";
			}
		}
	}

	if( kPacket_.m_vecWarningQuestID.empty() == false )
	{
		START_LOG( cout, L"[Test] A quest without a TimeEvent setting is registered in the database. Please check." )
			<< BUILD_LOG( wstrMsg.str().c_str() );
	}
}
//}}

void KGameEventManager::AddDefenseDungeonOpenRateEvent( IN const KEventInfo& kInfo )
{
	if( GetServerType() != ST_CENTER )
		return;

	std::map< int, KGameEvent* >::iterator mit;
	mit = m_mapIngEventData[KGameEvent::GET_DEFENSE_DUNGEON].find( kInfo.m_iEventUID );

	//이미 있는 이벤트라면..
	if( mit != m_mapIngEventData[KGameEvent::GET_DEFENSE_DUNGEON].end() )
	{
		if( RefreshEventTime( kInfo, mit->second ) == false )
		{
			START_LOG( cerr, L"Failed to update the event duration." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< END_LOG;
		}
	}
	//없는 이벤트라면 신규추가.
	else
	{
		CTime ctBegin, ctEnd;

		if( SetPeriodEventTime( kInfo, ctBegin, ctEnd ) == false )
		{
			START_LOG( cerr, L"Failed to set the event period." )
				<< BUILD_LOG( kInfo.m_iEventType )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< END_LOG;

			return;
		}

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		const KGameEventScriptManager::EVENT_DATA* pEventData = SiKGameEventScriptManager()->GetEventData( kInfo.m_iScriptID );
		//}}
		if( pEventData == NULL )
		{
			START_LOG( cerr, L"Reward event script is missing." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( kInfo.m_wstrEventName )
				<< BUILD_LOG( kInfo.m_iScriptID )
				<< END_LOG;
			return;
		}

		KGameEventDefenseDungeon* pEvent = new KGameEventDefenseDungeon();

		//이벤트 정보셋팅..
		pEvent->SetUID( kInfo.m_iEventUID );
		pEvent->SetScriptID( kInfo.m_iScriptID );
		pEvent->SetType( (KGameEvent::GAME_EVENT_TYPE)kInfo.m_iEventType );
		pEvent->SetEventName( kInfo.m_wstrEventName );
		pEvent->SetBeginDate( ctBegin );
		pEvent->SetEndDate( ctEnd );
		pEvent->SetDefenseDungeonOpenRate( pEventData->m_fDefenseDungeonOpenRate );

		m_mapIngEventData[KGameEvent::GET_DEFENSE_DUNGEON].insert( std::make_pair( kInfo.m_iEventUID, pEvent ) );

		START_LOG( cout2, L"New event registered." )
			<< BUILD_LOG( kInfo.m_iEventUID )
			<< BUILD_LOG( kInfo.m_iEventType )
			<< BUILD_LOG( kInfo.m_wstrEventName )
			<< BUILD_LOG( kInfo.m_wstrBeginDate )
			<< BUILD_LOG( kInfo.m_wstrEndDate )
			<< BUILD_LOG( kInfo.m_iScriptID )
			<< BUILD_LOG( (int)kInfo.m_bEnable )
			;
	}
}

bool KGameEventManager::GetDefenseDungeonOpenRate( OUT float& fDefenseDungeonOpenRate ) const
{
	fDefenseDungeonOpenRate = 0.0f;

	if( GetServerType() != ST_CENTER )
		return false;

	std::map<int, KGameEvent*>::const_iterator it;
	for( it = m_mapIngEventData[KGameEvent::GET_DEFENSE_DUNGEON].begin(); it != m_mapIngEventData[KGameEvent::GET_DEFENSE_DUNGEON].end(); ++it )
	{
		if( it->second == NULL )
			continue;

		if( it->second->GetType() != KGameEvent::GET_DEFENSE_DUNGEON )
			continue;

		KGameEventDefenseDungeon* pEvent = static_cast<KGameEventDefenseDungeon*>( it->second );

		// 진행중인 드롭률 이벤트
		if( it->second->IsEnable() == true )
		{
			float fTemp = pEvent->GetDefenseDungeonOpenRate();
			if( fDefenseDungeonOpenRate < fTemp )
			{
				fDefenseDungeonOpenRate = fTemp;
			}
		}
	}

	return ( 0.0f < fDefenseDungeonOpenRate );
}
