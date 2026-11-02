#include "DungeonMonsterManager.h"
#include "X2Data/XSLUnitManager.h"


//{{ 2011. 10. 18	최육사	배틀필드 시스템


KDungeonMonsterManager::KDungeonMonsterManager()
{
}

KDungeonMonsterManager::~KDungeonMonsterManager()
{
}

void KDungeonMonsterManager::StartGame()
{
	KRoomMonsterManager::StartGame();

	m_kNpcDieCount.Clear();			// 몬스터 죽은 카운트 초기화
	m_mapSubStageNpcInfo.clear();	// 서브 스테이지 몬스터 초기화
}

void KDungeonMonsterManager::EndGame()
{
	KRoomMonsterManager::EndGame();

	m_kNpcDieCount.Clear();			// 몬스터 죽은 카운트 초기화
	m_mapSubStageNpcInfo.clear();	// 서브 스테이지 몬스터 초기화
}

void KDungeonMonsterManager::OnCloseRoom()
{
	KRoomMonsterManager::OnCloseRoom();

	m_kNpcDieCount.Clear();
	m_mapSubStageNpcInfo.clear();
}

void KDungeonMonsterManager::IncreaseMonsterDieCount( IN const char cMonsterTypeFator )
{
	switch( cMonsterTypeFator )
	{
	case CXSLUnitManager::MTF_NORMAL_NPC:		++m_kNpcDieCount.m_iNormalNpcDieCount;		break;
	case CXSLUnitManager::MTF_LOW_ELITE_NPC:	++m_kNpcDieCount.m_iLowEliteNpcDieCount;	break;
	case CXSLUnitManager::MTF_HIGH_ELITE_NPC:	++m_kNpcDieCount.m_iHighEliteNpcDieCount;	break;
	case CXSLUnitManager::MTF_MIDDLE_BOSS_NPC:	++m_kNpcDieCount.m_iMiddleBossDieCount;		break;
	case CXSLUnitManager::MTF_BOSS_NPC:			++m_kNpcDieCount.m_iBossDieCount;			break;
	default:
		START_LOG( cerr, L"Unexpected monster type!" )
			<< BUILD_LOGc( cMonsterTypeFator )
			<< END_LOG;
		return;
	}
}

bool KDungeonMonsterManager::InsertSubStageNpcList( IN const int iStageID, IN const int iSubStageID, IN const std::vector< KNPCUnitReq >& vecSubStageNpcList )
{
	SUB_STAGE_NPC_INFO sSubStageNpcInfo;

	// 몬스터 등급별로 구분해서 리스트를 만들자!
	for( const KNPCUnitReq& kNpcInfo : vecSubStageNpcList )
	{
		if( kNpcInfo.m_cMonsterGrade == CXSLUnitManager::MG_BOSS_NPC )
		{
			sSubStageNpcInfo.m_setBossNpcUID.insert( kNpcInfo.m_UID );
		}
		else if( kNpcInfo.m_cMonsterGrade == CXSLUnitManager::MG_MIDDLE_BOSS_NPC )
		{
			sSubStageNpcInfo.m_setMiddleBossNpcUID.insert( kNpcInfo.m_UID );
		}
		else if( kNpcInfo.m_bActive )
		{
			sSubStageNpcInfo.m_setNormalNpcUID.insert( kNpcInfo.m_UID );
		}
	}

	// 만든 리스트를 저장하자!
	KSubStageKey kSubStageKey( iStageID, iSubStageID );
	if( m_mapSubStageNpcInfo.find( kSubStageKey ) != m_mapSubStageNpcInfo.end() )
	{
		START_LOG( cerr, L"Substage spawn NPC information is already registered." )
			<< BUILD_LOG( iStageID )
			<< BUILD_LOG( iSubStageID )
			<< BUILD_LOG( m_mapSubStageNpcInfo.size() )
			<< END_LOG;

		// 이미 있는것은 지우자!
		m_mapSubStageNpcInfo.erase( kSubStageKey );
	}

	m_mapSubStageNpcInfo.insert( std::make_pair( kSubStageKey, sSubStageNpcInfo ) );
	return true;
}

//{{ QUEST 개편 - 김민성
void KDungeonMonsterManager::GetSubStageNpcList( IN int iStageID, IN int iSubStageID, OUT std::vector< int >& vecSubStageNpcList )
{
	vecSubStageNpcList.clear();

	KSubStageKey kSubStageKey( iStageID, iSubStageID );
	std::map< KSubStageKey, SUB_STAGE_NPC_INFO >::const_iterator mit;
	mit = m_mapSubStageNpcInfo.find( kSubStageKey );
	if( mit == m_mapSubStageNpcInfo.end() )
	{
		START_LOG( clog, L"This stage has no NPCs; not an error. Change to clog later." )
			<< BUILD_LOG( iStageID )
			<< BUILD_LOG( iSubStageID )
			<< END_LOG;
		return;
	}

	// 일반 npc 정보 얻기
	{
		std::set< int >::const_iterator sit = mit->second.m_setNormalNpcUID.begin();
		for( ; sit != mit->second.m_setNormalNpcUID.end() ; ++sit )
		{
			NPC_DATA kData;
			if( GetNpcData( *sit, kData ) == true )
			{
				vecSubStageNpcList.push_back( kData.m_iNPCID );
			}
		}
	}

	// 중보 npc 정보 얻기
	{
		std::set< int >::const_iterator sit = mit->second.m_setMiddleBossNpcUID.begin();
		for( ; sit != mit->second.m_setMiddleBossNpcUID.end() ; ++sit )
		{
			NPC_DATA kData;
			if( GetNpcData( *sit, kData ) == true )
			{
				vecSubStageNpcList.push_back( kData.m_iNPCID );
			}
		}
	}

	// 보스 npc 정보 얻기
	{
		std::set< int >::const_iterator sit = mit->second.m_setBossNpcUID.begin();
		for( ; sit != mit->second.m_setBossNpcUID.end() ; ++sit )
		{
			NPC_DATA kData;
			if( GetNpcData( *sit, kData ) == true )
			{
				vecSubStageNpcList.push_back( kData.m_iNPCID );
			}
		}
	}
}
//}}

bool KDungeonMonsterManager::IsAllSubStageNpcDie( IN const int iStageID, IN const int iSubStageID )
{
	// 모든 NPC를 죽여달라는 요청이 들어온 스테이지가 보스가 있는 서브 스테이지인지 확인하자!
	KSubStageKey kSubStageKey( iStageID, iSubStageID );

	std::map< KSubStageKey, SUB_STAGE_NPC_INFO >::const_iterator mit;
	mit = m_mapSubStageNpcInfo.find( kSubStageKey );
	if( mit == m_mapSubStageNpcInfo.end() )
	{
		START_LOG( cerr, L"Received a request to kill all NPCs on a stage with no NPCs; possible hack attempt." )
			<< BUILD_LOG( iStageID )
			<< BUILD_LOG( iSubStageID )
			<< END_LOG;
		return false;
	}

	const SUB_STAGE_NPC_INFO& sSubStageNpcInfo = mit->second;

	// 보스 스테이지 인지 일반 스테이지 인지 확인
	if( sSubStageNpcInfo.m_setBossNpcUID.empty() )
	{
		// 해당 서브 스테이지에 모든 일반 몬스터가 죽었는지 확인!
		for( const int iNpcUID : sSubStageNpcInfo.m_setNormalNpcUID )
		{
			bool bIsDie = false;
			if( IsExistMonster( iNpcUID, bIsDie ) == false )
			{
				START_LOG( cerr, L"NPC_DATA is missing for the NpcUID in the normal NPC list." )
					<< BUILD_LOG( iStageID )
					<< BUILD_LOG( iSubStageID )
					<< BUILD_LOG( iNpcUID )
					<< END_LOG;
				continue;
			}

			// npc가 죽지 않았다면?
			if( bIsDie == false )
			{
				START_LOG( cerr, L"Received a kill-all-monsters request although not all NPCs are dead; possible hack attempt." )
					<< BUILD_LOG( iStageID )
					<< BUILD_LOG( iSubStageID )
					<< BUILD_LOG( iNpcUID )
					<< END_LOG;
				return false;
			}
		}
	}
	else
	{
		// 해당 서브 스테이지에 모든 보스 몬스터가 죽었는지 확인!
		for( const int iNpcUID : sSubStageNpcInfo.m_setBossNpcUID )
		{
			bool bIsDie = false;
			if( IsExistMonster( iNpcUID, bIsDie ) == false )
			{
				START_LOG( cerr, L"NPC_DATA is missing for the NpcUID in the boss list." )
					<< BUILD_LOG( iStageID )
					<< BUILD_LOG( iSubStageID )
					<< BUILD_LOG( iNpcUID )
					<< END_LOG;
				continue;
			}

			// 보스가 죽지 않았다면?
			if( bIsDie == false )
			{
				START_LOG( cerr, L"Received a kill-all-monsters request while the boss is still alive; possible hack attempt." )
					<< BUILD_LOG( iStageID )
					<< BUILD_LOG( iSubStageID )
					<< BUILD_LOG( iNpcUID )
					<< END_LOG;
				return false;
			}
		}
	}

	return true;
}


//}}
