#include "UserDungeonManager.h"
//{{ 2012. 04. 23	최육사	배틀필드 시스템
	#include "X2Data/XSLDungeonManager.h"
	#include "GSAutoPartyManager.h"
	#include "X2Data/XSLMapData.h"
//}}

	#include "GameSysVal/GameSysVal.h"

//{{ 2011. 08. 12   김민성      헤니르 개편


KUserDungeonManager::KUserDungeonManager(void)
{
	Clear();
}

KUserDungeonManager::~KUserDungeonManager(void)
{
}

void KUserDungeonManager::Clear()
{
	m_iHenirRewardCount				= 0;				// 헤니르 보상 가능 횟수 (일반)
	m_iHenirRewardPremiumCount		= 0;				// 헤니르 보상 가능 횟수 (PC방)
	m_iHenirRewardEventCount		= 0;				// 헤니르 보상 가능 횟수 (이벤트) 횟수

	m_iHenirRewardEventLimitCount	= 0;				// 헤니르 보상 가능 횟수 (이벤트) 제한
	m_bUnLimitedHenirRewardEvnet	= false;			// 헤니르 보상 가능 횟수 (이벤트) 무제한

	m_bIsPcBang						= false;
	m_iPcBangType					= -1;
	//{{ 2012. 02. 08	최육사	배틀필드 시스템
	m_iAutoPartyWaitNumber			= 0;
	m_iAutoPartyUID					= 0;
	m_kDungeonGameInfo.Clear();
	//}}
}

void KUserDungeonManager::Init( IN const bool bUnlimitedEvent			// 무제한 인가?
							  , IN const int iLimitEvent				// Event 추가 횟수 MAX
							  , IN const int iNormalCnt					// 보상 받은 횟수 일반
							  , IN const int iPremiumCnt				// 보상 받은 횟수 pc방
							  , IN const int iEventCnt					// 보상 받은 횟수 이벤트
							  , IN const bool bIsPcBang					// PC방인지 정보 넣기
								, IN const int iPcBangType				// PC방 타입
							  )
{
	if( ( iLimitEvent < 0 )
		|| ( iNormalCnt < 0 )
		|| ( iPremiumCnt < 0 )
		|| ( iEventCnt < 0 )
		)
	{
		START_LOG( cerr, L"Henir reward acquisition initialized to a value below zero?")
			<< BUILD_LOG( bUnlimitedEvent )
			<< BUILD_LOG( iLimitEvent )
			<< BUILD_LOG( iNormalCnt )
			<< BUILD_LOG( iPremiumCnt )
			<< BUILD_LOG( iEventCnt )
			<< END_LOG;
	}

	m_iHenirRewardCount				= iNormalCnt;
	m_iHenirRewardPremiumCount		= iPremiumCnt;
	m_iHenirRewardEventCount		= iEventCnt;

	m_iHenirRewardEventLimitCount	= iLimitEvent;
	m_bUnLimitedHenirRewardEvnet	= bUnlimitedEvent;

	m_bIsPcBang						= bIsPcBang;
	m_iPcBangType					= iPcBangType;
}

bool KUserDungeonManager::IncreaseHenirRewardCount()
{
	if( m_bUnLimitedHenirRewardEvnet == true )		// 헤니르 무제한 이벤트 중이면
	{
		return true;
	}

	// 보상 획득 감소 순서
	// PC방 > Event > Normal

	// 1. PC 방
	if( IsPremiumUser()== true )
	{
		int iAdditionalHenirRewardCount = SiKGameSysVal()->GetAdditionalHenirRewardCount( GetPcBangType() );
		if( m_iHenirRewardPremiumCount < iAdditionalHenirRewardCount )
		{
			m_iHenirRewardPremiumCount += 1;

			return true;
		}
	}

	// 2. Event
	if( m_iHenirRewardEventCount < m_iHenirRewardEventLimitCount )
	{
		m_iHenirRewardEventCount += 1;

		return true;
	}

	// 3. Normal
	if( m_iHenirRewardCount < HRLC_NORMAL )
	{
		m_iHenirRewardCount += 1;

		return true;
	}

	return false;
}

void KUserDungeonManager::GetHenirRewardCountInfo( IN KEGS_HENIR_REWARD_COUNT_NOT& kNot )
{
	kNot.m_bUnLimited	= m_bUnLimitedHenirRewardEvnet;
	kNot.m_iEvent		= GetPossibleHenirRewardEventCount();
	kNot.m_iPremium		= GetPossibleHenirRewardPremiumCount();
	kNot.m_iNormal		= GetPossibleHenirRewardNormalCount();
	kNot.m_iEventMAX	= m_iHenirRewardEventLimitCount;

	int iAdditionalHenirRewardCount = SiKGameSysVal()->GetAdditionalHenirRewardCount( GetPcBangType() );
	kNot.m_iPremiumMAX	= ( IsPremiumUser() == true )  ? iAdditionalHenirRewardCount : 0;
}

int KUserDungeonManager::GetPossibleHenirRewardNormalCount()
{
	if( (HRLC_NORMAL - m_iHenirRewardCount) < 0 )
	{
		return 0;
	}

	return HRLC_NORMAL - m_iHenirRewardCount;
}

int KUserDungeonManager::GetPossibleHenirRewardEventCount()
{
	if( (m_iHenirRewardEventLimitCount - m_iHenirRewardEventCount) < 0 )
	{
		return 0;
	}

	return m_iHenirRewardEventLimitCount - m_iHenirRewardEventCount;
}

int KUserDungeonManager::GetPossibleHenirRewardPremiumCount()
{
	if( IsPremiumUser() == true )
	{
		int iAdditionalHenirRewardCount = SiKGameSysVal()->GetAdditionalHenirRewardCount( GetPcBangType() );
		if( (iAdditionalHenirRewardCount - m_iHenirRewardPremiumCount) < 0 )
		{
			return 0;
		}

		return iAdditionalHenirRewardCount - m_iHenirRewardPremiumCount;
	}

	return 0;
}


//}}

//{{ 2012. 04. 23	최육사	배틀필드 시스템
void KUserDungeonManager::SetDungeonGameInfo( IN const int iDungeonID, IN const char cDifficulty, IN const char cDungeonMode )
{
    m_kDungeonGameInfo.m_iDungeonID = iDungeonID;
	m_kDungeonGameInfo.m_cDifficulty = cDifficulty;
	m_kDungeonGameInfo.m_cDungeonMode = cDungeonMode;
}

bool KUserDungeonManager::IsPlayingNowHenirChallengeMode() const
{
	if( m_kDungeonGameInfo.m_iDungeonID == 0 )
		return false;

	const CXSLDungeon::DUNGEON_TYPE eDungeonType = SiCXSLDungeonManager()->GetDungeonType( m_kDungeonGameInfo.GetDungeonIDWithDif() );
	if( eDungeonType != CXSLDungeon::DT_HENIR )
	{
		// 헤니르 시공이 아니면 false
		return false;
	}

	//CXSLDungeon::HENIR_DUNGEON_MODE eHenirDungeonMode = CXSLDungeon::HDM_INVALID;
	//if( !SiKPartyManager()->GetHenirDungeonMode( GetPartyUID(), eHenirDungeonMode ) )
	//{
	//	START_LOG( cerr, L"던전 모드를 얻을 수 없습니다! 있을수 없는 에러!" )
	//		<< BUILD_LOG( GetCharUID() )
	//		<< BUILD_LOG( GetPartyUID() )
	//		<< END_LOG;

	//	return false;
	//}

	// 헤니르 시공 도전모드 여부
	return ( (CXSLDungeon::DUNGEON_MODE)m_kDungeonGameInfo.m_cDungeonMode == CXSLDungeon::DM_HENIR_CHALLENGE );
}

void KUserDungeonManager::GetAutoPartyBonusDungeonList( IN const int iVillageMapID, OUT std::map< int, KAutoPartyDungeonBonusInfo >& mapAutoPartyBonusList )
{
	const CXSLMapData::SLocalMapInfo* pLocalMapIDList = SiCXSLMapData()->GetLocalMapList( iVillageMapID );
	if( IS_NULL( pLocalMapIDList ) )
	{
		START_LOG( cerr, L"Could not retrieve local map list information!" )
			<< BUILD_LOG( iVillageMapID )
			<< END_LOG;
		return;
	}

	for( const KDungeonGameSimpleInfo& kInfo : pLocalMapIDList->m_vecDungeonInfo )
	{
		KAutoPartyDungeonBonusInfo kAutoPartyBonusInfo;
		kAutoPartyBonusInfo.m_cDungeonMode = kInfo.m_cDungeonMode;
		kAutoPartyBonusInfo.m_iDungeonID = kInfo.m_iDungeonID;
		kAutoPartyBonusInfo.m_bActivatedBonus = SiKGSAutoPartyManager()->IsAutoPartyBonus( kInfo.m_cDungeonMode, kInfo.m_iDungeonID );
		mapAutoPartyBonusList.insert( std::make_pair( kInfo.m_iDungeonID, kAutoPartyBonusInfo ) );
	}
}

void KUserDungeonManager::GetAutoPartyBonusDungeonList( IN const std::vector< KDungeonGameSimpleInfo >& vecDungeonList, OUT std::map< int, KAutoPartyDungeonBonusInfo >& mapAutoPartyBonusList )
{
	for( const KDungeonGameSimpleInfo& kInfo : vecDungeonList )
	{
		KAutoPartyDungeonBonusInfo kAutoPartyBonusInfo;
		kAutoPartyBonusInfo.m_cDungeonMode = kInfo.m_cDungeonMode;
		kAutoPartyBonusInfo.m_iDungeonID = kInfo.m_iDungeonID;
		kAutoPartyBonusInfo.m_bActivatedBonus = SiKGSAutoPartyManager()->IsAutoPartyBonus( kInfo.m_cDungeonMode, kInfo.m_iDungeonID );
		mapAutoPartyBonusList.insert( std::make_pair( kInfo.m_iDungeonID, kAutoPartyBonusInfo ) );
	}
}
//}}
