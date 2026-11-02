#include "GSSimLayer.h"		// 해외팀 추가 의도적으로 디파인 걸지않음
#include "GameServer.h"
#include "ChannelManager.h"


ImplementSingleton( KChannelManager );

KChannelManager::KChannelManager()
:
m_iGameServChannelID( 0 ),
m_iCurPartyCount( 0 ),
m_iPlayGamePartyCount( 0 ),
//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
m_fMaxWaitingTime( 0.0f ),
//}}
m_iMaxUser( 0 )
{
}

KChannelManager::~KChannelManager()
{
}

ImplToStringW( KChannelManager )
{
	return stm_;
}

void KChannelManager::Tick()
{
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	BEGIN_CHECK_TICK_LATENCY;
	//}}

	std::vector< UidType > vecDeleteList;

	std::map< UidType, KChannelChangeUserInfo >::iterator mit;
	for( mit = m_mapChannelChangeUserList.begin(); mit != m_mapChannelChangeUserList.end(); ++mit )
	{
		double fWaitingTime = mit->second.m_tWaitingTimer.elapsed();
		if( fWaitingTime > 15.f )
		{
			vecDeleteList.push_back( mit->first );

			// 가장 오래걸린 채널이동시간을 저장
			if( m_fMaxWaitingTime < fWaitingTime )
			{
				m_fMaxWaitingTime = fWaitingTime;

				START_LOG( cout, L"Channel transfer reservation took over 15 seconds. WaitingTime: " << m_fMaxWaitingTime );
			}
		}
	}

	std::vector< UidType >::const_iterator vit;
	for( vit = vecDeleteList.begin(); vit != vecDeleteList.end(); ++vit )
	{
		m_mapChannelChangeUserList.erase( *vit );
	}

	//{{ 2009. 2. 13  최육사	채널리스트
	if( m_kChannelListRefreshTimer.elapsed() > 60.0 )
	{
		KDBE_CHANNEL_LIST_REQ kPacketToDB;
		kPacketToDB.m_iServerGroupID = m_iServerGroupID;
		SendToLogDB( DBE_CHANNEL_LIST_REQ, kPacketToDB );

		// 서버군이 2개 국가에서만 동작
		if ( GetKGSSimLayer()->GetServerGroupNum() == KGSSimLayer::DOUBLE_SERVER_GROUP )
		{
			KDBE_CHANNEL_LIST_REQ kPacketToDB2nd;
			kPacketToDB2nd.m_iServerGroupID = ( m_iServerGroupID == SEnum::SGI_SOLES ? SEnum::SGI_GAIA : SEnum::SGI_SOLES );
			SendToLogDB2nd( DBE_CHANNEL_LIST_REQ, kPacketToDB2nd );
		}

		m_kChannelListRefreshTimer.restart();
	}
	//}}

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	END_CHECK_TICK_LATENCY;
	//}}
}

void KChannelManager::Init( int iServerGroupID, int iChannelID, const KChannelBonusInfo& kInfo )
{
	m_iServerGroupID = iServerGroupID;
	m_iGameServChannelID = iChannelID;

	START_LOG( cout, L"GameServer channel ID configured!" )
		<< BUILD_LOG( m_iServerGroupID )
		<< BUILD_LOG( m_iGameServChannelID );

	if( kInfo.m_bEnable )
	{
		m_kBonusInfo = kInfo;

		START_LOG( cout, L"Applied the GameServer channel bonus!" )
			<< BUILD_LOG( m_kBonusInfo.m_iBeginLv )
			<< BUILD_LOG( m_kBonusInfo.m_iEndLv )
			<< BUILD_LOG( m_kBonusInfo.m_iPerExp )
			<< BUILD_LOG( m_kBonusInfo.m_iPerED );
	}
}

void KChannelManager::UpdateChannelList( const std::map< int, KChannelInfo >& mapChannelList )
{
	m_mapChannelList.clear();
	m_vecChannelList.clear();

	m_vecChannelInfo.clear();

	m_iCurPartyCount = 0;
	m_iPlayGamePartyCount = 0;
	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	const UidType iServerUID = KBaseServer::GetKObj()->GetUID();
	bool bUpdatedMaxUser = false;
	//}}

	m_mapChannelList = mapChannelList;

	std::map< int, KChannelInfo >::iterator mit;
	for( mit = m_mapChannelList.begin(); mit != m_mapChannelList.end(); ++mit )
	{
		//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
		if( mit->second.m_iServerUID == iServerUID )
		{
			bUpdatedMaxUser = true;

			if( GetMaxUser() != mit->second.m_iMaxUser )
			{
				START_LOG( cout, L"The channel maximum-user setting has changed!" )
					<< BUILD_LOG( iServerUID )
					<< BUILD_LOG( GetMaxUser() )
					<< BUILD_LOG( mit->second.m_iMaxUser );

                SetMaxUser( mit->second.m_iMaxUser );
			}
		}
		//}}

		// 채널 정보
		KChannelSimpleInfo kInfo;
		kInfo.m_iChannelID = mit->second.m_iChannelID;
		kInfo.m_iCurrentUser = mit->second.m_iCurrentUser;
		kInfo.m_iMaxUser = mit->second.m_iMaxUser;
		m_vecChannelList.push_back( kInfo );

		// 현재 파티 인원 저장
		m_iCurPartyCount += mit->second.m_iCurPartyCount;
		m_iPlayGamePartyCount += mit->second.m_iPlayGamePartyCount;

		mit->second.m_iCurPartyCount = 0;
		mit->second.m_iPlayGamePartyCount = 0;

		// 2012.09.11 lygan_조성욱 // 미니맵 옆 UI 실시간 갱신용 정보
		m_vecChannelInfo.push_back(mit->second);

	}

	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	if( bUpdatedMaxUser == false )
	{
		START_LOG( cerr, L"The current channel is missing from the channel list!" )
			<< BUILD_LOG( iServerUID )
			<< BUILD_LOG( mapChannelList.size() )
			<< END_LOG;
	}
	//}}
}

void KChannelManager::UpdateChannelList2nd( const std::map< int, KChannelInfo >& mapChannelList_ )
{
    m_mapChannelList2nd.clear();
    m_mapChannelList2nd = mapChannelList_;

    std::map< int, KChannelInfo >::iterator mit;
    for( mit = m_mapChannelList2nd.begin(); mit != m_mapChannelList2nd.end(); ++mit )
    {
        KChannelSimpleInfo kInfo;
        kInfo.m_iChannelID = mit->second.m_iChannelID;
        kInfo.m_iCurrentUser = mit->second.m_iCurrentUser;
        kInfo.m_iMaxUser = mit->second.m_iMaxUser;
        m_vecChannelList2nd.push_back( kInfo );

    }

}




//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
void KChannelManager::UpdateChannelBonusList( const std::map< int, KChannelBonusInfo >& mapChannelBonusList )
{
	m_mapChannelBonusList.clear();

	m_mapChannelBonusList = mapChannelBonusList;

	std::map< int, KChannelBonusInfo >::const_iterator mit;
	mit = mapChannelBonusList.find( m_iGameServChannelID );

	if( mit == mapChannelBonusList.end() )
	{
		START_LOG(cerr, L"Cannot change the GameServer channel bonus.")
			<< BUILD_LOG( m_iGameServChannelID )
			<< BUILD_LOG( m_mapChannelBonusList.size() )
			<< END_LOG;
		return;
	}
	else
	{
		m_kBonusInfo = mit->second;
	}

}

//}}

void KChannelManager::UpdateChannelBonusList2nd( const std::map< int, KChannelBonusInfo >& mapChannelBonusList_ )
{
    m_mapChannelBonusList2nd.clear();

    m_mapChannelBonusList2nd = mapChannelBonusList_;

}

bool KChannelManager::GetServerUIDByChannelID( int iChannelID, UidType& iServerUID )
{
	std::map< int, KChannelInfo >::const_iterator mit;
	mit = m_mapChannelList.find( iChannelID );
	if( mit == m_mapChannelList.end() )
		return false;

	iServerUID = mit->second.m_iServerUID;
	return true;
}

bool KChannelManager::NewChannelChangeUser( UidType iUnitUID, const KChannelChangeInfo& kInfo )
{
	if( IsExistChannelChangeUser( iUnitUID ) )
	{
		START_LOG( cerr, L"User already has a channel transfer scheduled." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	KChannelChangeUserInfo kReserveInfo( kInfo );
	m_mapChannelChangeUserList.insert( std::make_pair( iUnitUID, kReserveInfo ) );
	return true;
}

bool KChannelManager::DelChannelChangeUser( UidType iUnitUID, KChannelChangeInfo& kInfo )
{
	std::map< UidType, KChannelChangeUserInfo >::iterator mit = m_mapChannelChangeUserList.find( iUnitUID );
	if( mit == m_mapChannelChangeUserList.end() )
	{
		START_LOG( cerr, L"Attempted to delete information for an unregistered user." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	// 가장 오래걸린 채널이동시간을 저장
	double fWaitingTime = mit->second.m_tWaitingTimer.elapsed();
	if( fWaitingTime > 5.f )
	{
		if( m_fMaxWaitingTime < fWaitingTime )
		{
			m_fMaxWaitingTime = fWaitingTime;

			START_LOG( cout, L"Channel transfer took over 5 seconds; updating maximum duration. WaitingTime: " << m_fMaxWaitingTime );
		}
	}

	// 저장해놓았던 채널이동시 가져가는 정보 얻기
	kInfo = mit->second.m_kChannelChangeInfo;

	// 예약 정보 삭제
	m_mapChannelChangeUserList.erase( mit );
	return true;
}

bool KChannelManager::DelChannelChangeUser( UidType iUnitUID )
{
	KChannelChangeInfo kInfo;
    return DelChannelChangeUser( iUnitUID, kInfo );
}

void KChannelManager::GetChannelBonus( u_char ucLevel, float& fAddExpRate, float& fAddEDRate )
{
	fAddExpRate = 0.0f;
	fAddEDRate = 0.0f;

	if( m_kBonusInfo.m_bEnable == false )
		return;

	if( static_cast<int>(ucLevel) < m_kBonusInfo.m_iBeginLv  ||  static_cast<int>(ucLevel) > m_kBonusInfo.m_iEndLv )
		return;

	fAddExpRate = ( static_cast<float>(m_kBonusInfo.m_iPerExp) / 100.f );
	fAddEDRate = ( static_cast<float>(m_kBonusInfo.m_iPerED) / 100.f );
}

void KChannelManager::SendToLogDB( unsigned short usEventID )
{
	SendToLogDB( usEventID, char() );
}

//{{ 2013. 04. 01	 인연 시스템 - 김민성
void KChannelManager::GetServerUIDByChannelName( OUT std::wstring& wstrName )
{
	std::map< int, KChannelInfo >::iterator mit = m_mapChannelList.find( m_iGameServChannelID );
	if( mit != m_mapChannelList.end() )
	{
		wstrName = mit->second.m_wstrChannelName;
	}
}
//}
