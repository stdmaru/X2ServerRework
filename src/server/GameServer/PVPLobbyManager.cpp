#include "./PVPLobbyManager.h"

#include "NetError.h"

ImplementSingleton( KPVPLobbyManager );

KPVPLobbyManager::KPVPLobbyManager(void)
{
}

KPVPLobbyManager::~KPVPLobbyManager(void)
{
}

ImplToStringW( KPVPLobbyManager )
{
	return stm_;
}

bool KPVPLobbyManager::AddPVPUser( IN KCommunityUserInfo& kInfo_, IN UidType nGSUID )
{
	// 유령유저가 있는지 검사
	if( IsExist( kInfo_.m_iUnitUID ) )
	{
		START_LOG( cerr, L"Cannot add PvP user: user already exists! Is it a ghost user?" )
			<< BUILD_LOG( kInfo_.m_iUnitUID )
			<< END_LOG;

		// 삭제처리!
		RemovePVPUser( kInfo_.m_iUnitUID );
	}

	// 현재 상태를 로비로 변경!
	kInfo_.m_cState = static_cast<char>( KCommunityUserInfo::US_PVP_LOBBY );

	// pvp user 생성
	KPVPUserPtr spAddUser = KPVPUserPtr( new KPVPUser( kInfo_, nGSUID ) );
	if( spAddUser == NULL )
	{
		START_LOG( cerr, L"Failed to create PvP user!" )
			<< BUILD_LOG( kInfo_.m_iUnitUID )
			<< BUILD_LOG( nGSUID )
			<< END_LOG;

		return false;
	}

	// map에 추가
	std::pair< std::map< UidType, KPVPUserPtr >::iterator, bool > pairResult;
	pairResult = m_mapPVPUserList.insert( std::make_pair( kInfo_.m_iUnitUID, spAddUser ) );

	if( !pairResult.second )
	{
		START_LOG( cerr, L"Failed to register PvP user; this should never happen!" )
			<< BUILD_LOG( kInfo_.m_iUnitUID )
			<< BUILD_LOG( nGSUID )
			<< END_LOG;

		return false;
	}

	// vector에 추가
	m_vecPVPUserList.push_back( spAddUser );

	return true;
}

bool KPVPLobbyManager::RemovePVPUser( IN UidType iUnitUID )
{
	KPVPUserPtr spPVPUser = Get( iUnitUID );
	if( spPVPUser == NULL )
	{
		START_LOG( cerr, L"Cannot remove PvP user: user does not exist!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		// 삭제성공으로..
		return true;
	}

	// vector에서 지우자!
	std::vector< KPVPUserPtr >::iterator vitRemove;
	for( vitRemove = m_vecPVPUserList.begin(); vitRemove != m_vecPVPUserList.end(); ++vitRemove )
	{
		if( *vitRemove == NULL )
		{
            START_LOG( cerr, L"Attempted to delete user, but pointer is null!" )
				<< BUILD_LOG( iUnitUID )
				<< END_LOG;
			continue;
		}

		// 찾았다!
        if( (*vitRemove)->m_kUserInfo.m_iUnitUID == iUnitUID )
			break;
	}

	if( vitRemove != m_vecPVPUserList.end() )
	{
        m_vecPVPUserList.erase( vitRemove );
	}
	else
	{
		START_LOG( cerr, L"Could not find the user in the vector, even though it is in the map?" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;
	}

	// map에서 지우자!
	m_mapPVPUserList.erase( iUnitUID );
	return true;
}

bool KPVPLobbyManager::UpdatePVPUserInfo( IN UidType iUnitUID, IN char cState, IN int iRoomListID, IN UidType iRoomUID )
{
	KPVPUserPtr spPVPUser = Get( iUnitUID );
	if( spPVPUser == NULL )
	{
		// 시간차로 발생가능
		START_LOG( cwarn, L"Cannot update PvP user information: user does not exist!" )
			<< BUILD_LOG( iUnitUID )
			<< BUILD_LOGc( cState )
			<< END_LOG;

		return false;
	}

	// 상태 정보 갱신
	spPVPUser->m_kUserInfo.m_cState		= cState;
	if( iRoomListID > 0 )
	{
		// 유효한 RoomListID만 업데이트한다.
		spPVPUser->m_kUserInfo.m_iStateCode = iRoomListID;
	}
	spPVPUser->m_kUserInfo.m_iRoomUID	= iRoomUID;

	return true;
}

bool KPVPLobbyManager::GetPVPUserList( IN UINT& uiViewPage, IN UINT& uiTotalPage, OUT std::vector< KCommunityUserInfo >& vecUserList )
{
	vecUserList.clear();

	_JIF( uiViewPage > 0, return false; );

	if( m_vecPVPUserList.empty() )
	{
		uiViewPage = 1;
		uiTotalPage = 1;
		return true;
	}

	uiTotalPage = m_vecPVPUserList.size() / PAGE_PER_PVP_USER;
	if( ( m_vecPVPUserList.size() % PAGE_PER_PVP_USER ) > 0 )
	{
		++uiTotalPage;
	}

	u_int uiBeginIndex = ( uiViewPage * PAGE_PER_PVP_USER ) - PAGE_PER_PVP_USER;
	if( m_vecPVPUserList.size() <= uiBeginIndex )
	{
		uiViewPage = uiTotalPage; // 마지막 페이지
		uiBeginIndex = ( uiViewPage * PAGE_PER_PVP_USER ) - PAGE_PER_PVP_USER;

		if( m_vecPVPUserList.size() <= uiBeginIndex )
		{
			START_LOG( cerr, L"This should never happen!" )
				<< BUILD_LOG( uiViewPage )
				<< BUILD_LOG( uiTotalPage )
				<< BUILD_LOG( uiBeginIndex )
				<< BUILD_LOG( m_vecPVPUserList.size() )
				<< END_LOG;
			return false;
		}
	}

	std::vector< KPVPUserPtr >::reverse_iterator vit = m_vecPVPUserList.rbegin();
	vit += uiBeginIndex;

	while( vit != m_vecPVPUserList.rend() )
	{
		KPVPUserPtr spPVPUser = *vit;
		if( spPVPUser == NULL )
		{
			START_LOG( cerr, L"Invalid PvP user pointer." )
				<< END_LOG;
		}
		else
		{
			vecUserList.push_back( spPVPUser->m_kUserInfo );

			if( vecUserList.size() >= PAGE_PER_PVP_USER )
				break;
		}

		++vit;
	}

	return true;
}

void KPVPLobbyManager::RemovePVPUserDisconnectGS( IN UidType nGSUID )
{
	std::vector< UidType >::const_iterator vitRemove;
	std::vector< UidType > vecRemoveUserList;

	// 연결이 끊어진 게임서버의 유저들을 찾는다
	std::vector< KPVPUserPtr >::const_iterator vit;
	for( vit = m_vecPVPUserList.begin(); vit != m_vecPVPUserList.end(); ++vit )
	{
		if( *vit == NULL )
		{
			START_LOG( cerr, L"Invalid PvP user pointer!" )
				<< BUILD_LOG( nGSUID )
				<< END_LOG;
			continue;
		}

		if( (*vit)->m_nGSUID == nGSUID )
		{
			vecRemoveUserList.push_back( (*vit)->m_kUserInfo.m_iUnitUID );
		}
	}

	// 찾은 유저들을 리스트에서 삭제한다!
	for( vitRemove = vecRemoveUserList.begin(); vitRemove != vecRemoveUserList.end(); ++vitRemove )
	{
		if( !RemovePVPUser( *vitRemove ) )
		{
			START_LOG( cerr, L"Failed to delete user!" )
				<< BUILD_LOG( *vitRemove )
				<< END_LOG;
		}
	}
}

KPVPUserPtr KPVPLobbyManager::Get( IN UidType iUnitUID )
{
	std::map< UidType, KPVPUserPtr >::iterator mit;
	mit = m_mapPVPUserList.find( iUnitUID );
	if( mit == m_mapPVPUserList.end() )
	{
		START_LOG( cwarn, L"Could not find PvP user." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return KPVPUserPtr();
	}

	return mit->second;
}
