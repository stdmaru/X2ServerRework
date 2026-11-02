#include "./RoomListManager.h"

#include "PVPList.h"
#include "SquareList.h"

#include "NetError.h"
#include "GSSimLayer.h"

#include "GameEvent/GameEventManager.h"

ImplementSingleton( KRoomListManager );

KRoomListManager::KRoomListManager(void) :
m_nStartPVPID( 0 )
{
	lua_tinker::class_add<KRoomListManager>( g_pLua, "KRoomListManager" );
	lua_tinker::class_def<KRoomListManager>( g_pLua, "AddRoomListTemplet",		&KRoomListManager::AddRoomListTemplet_LUA );
	lua_tinker::class_def<KRoomListManager>( g_pLua, "SetStartPVPID",			&KRoomListManager::SetStartPVPID_LUA );
	//{{ 2011. 07. 04	최육사	대전 개편
	lua_tinker::class_def<KRoomListManager>( g_pLua, "SetPvpOffcialRoomListID",	&KRoomListManager::SetPvpOffcialRoomListID_LUA );
	//}}
	lua_tinker::class_def<KRoomListManager>( g_pLua, "SetTournamentPVPID",		&KRoomListManager::SetTournamentPVPID_LUA );

	lua_tinker::class_def<KRoomListManager>( g_pLua, "dump",					&KRoomListManager::Dump );

	lua_tinker::decl( g_pLua, "g_pRoomListManager", this );
}

KRoomListManager::~KRoomListManager(void)
{
}

ImplToStringW( KRoomListManager )
{
	stm_ << L"KRoomListManager dump." << std::endl
		 << L"Pvp Room Num : " << m_mapRoomList.size() << std::endl
		 << L"Square   Num : " << m_mapSquareInfoList.size() << std::endl
		 ;

	//std::map< UINT, KRoomListPtr >::const_iterator mit;
	//for( mit = m_mapRoomList.begin(); mit != m_mapRoomList.end(); ++mit )
	//{
	//	stm_ << *(mit->second);
	//}

	return stm_;
}

bool KRoomListManager::OpenScriptFile( const char* pFileName )
{
	_JIF( 0 == LUA_DOFILE( g_pLua, pFileName ), return false );

	return true;
}

//{{ 2011. 07. 19	최육사	대전 개편
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool KRoomListManager::OpenPVPRoomList( KGSUserPtr spUser_, KEGS_OPEN_PVP_ROOM_LIST_ACK& kInfo )
{
	kInfo.m_iConnectChannelID = 0;
	kInfo.m_iOK	= NetError::NET_OK;

	KRoomListPtr spRoomList;
	int	nMin = 0;
	int nMax = 0;

	// 대전 입장 가능레벨 체크
	if( spUser_->GetAuthLevel() < SEnum::UAL_GM )
	{
		if( spUser_->GetLevel() < SEnum::PE_PVP_LIMIT_LEVEL )
		{
			kInfo.m_iOK = NetError::ERR_ROOM_LIST_03;
			return false;
		}
	}

	for( UINT ui = m_nStartPVPID; ui < 100; ++ui )
	{
		// 첫번째 대전 룸리스트
		spRoomList = Get( ui );
		if( spRoomList == NULL )
			continue;

		if( ( spRoomList->GetType() != KRoomList::RLT_PVP )  &&  ( spRoomList->GetType() != KRoomList::RLT_FREE_PVP ) )
		{
			kInfo.m_iOK	= NetError::ERR_CHANNEL_00;
			return false;
		}

		// 개편된 대전에서는 연습 채널만 넣습니다.
		switch( spRoomList->GetPVPChannelClass() )
		{
		case KPVPChannelInfo::PCC_TOURNAMENT:
			{
				// 대회채널이 열렸을때만 정보를 준다!
				if( SiKGameEventManager()->CheckTournamentEvent() == true )
				{
					spRoomList->GetVSPoint( nMin, nMax );

					KPVPChannelInfo	kPVPChannelInfo;
					kPVPChannelInfo.m_iChannelID	= spRoomList->GetID();
					kPVPChannelInfo.m_iMinRating	= nMin;
					kPVPChannelInfo.m_iMaxRating	= nMax;
					kPVPChannelInfo.m_cPVPChannelClass = static_cast<char>(spRoomList->GetPVPChannelClass());

					kInfo.m_vecChannel.push_back( kPVPChannelInfo );
				}
			}
			break;

		case KPVPChannelInfo::PCC_PLAY:
			{
				spRoomList->GetVSPoint( nMin, nMax );

				KPVPChannelInfo	kPVPChannelInfo;
				kPVPChannelInfo.m_iChannelID	= spRoomList->GetID();
				kPVPChannelInfo.m_iMinRating	= nMin;
				kPVPChannelInfo.m_iMaxRating	= nMax;
				kPVPChannelInfo.m_cPVPChannelClass = static_cast<char>(spRoomList->GetPVPChannelClass());

				kInfo.m_vecChannel.push_back( kPVPChannelInfo );
			}
			break;
		}
	}

	{ //////////////////////////////////////////////////////////////////////////
		//demiahun : 07.12.20 현재 기획상 대전채널을 순차적으로 열기위해서 모든 채널을 열지 않기 때문에
		//           자유채널을 제외한 다른채널을 못들어갈경우 자유채널에 넣어준다.
		KRoomListPtr spFreePvP = GetPVPFreeChannel();
		if( spFreePvP == NULL )
		{
			kInfo.m_iOK	= NetError::ERR_CHANNEL_07;
			return false;
		}

		spFreePvP->GetVSPoint( nMin, nMax );

		// (자유채널)
		KPVPChannelInfo	kPVPChannelInfo;

		kPVPChannelInfo.m_iChannelID = spFreePvP->GetID();
		kPVPChannelInfo.m_iMinRating = nMin;
		kPVPChannelInfo.m_iMaxRating = nMax;
		kPVPChannelInfo.m_cPVPChannelClass = static_cast<char>(spFreePvP->GetPVPChannelClass());
		kInfo.m_iConnectChannelID = spFreePvP->GetID();
		spUser_->SetRoomListID( kInfo.m_iConnectChannelID );
		//{{ 2009. 5. 3  최육사		대전유저리스트
		spUser_->SendUpdatePVPUserInfo( KCommunityUserInfo::US_ENTER_LOBBY );
		//}}

		kInfo.m_vecChannel.push_back( kPVPChannelInfo );
	} //////////////////////////////////////////////////////////////////////////


	if( kInfo.m_vecChannel.size() > 0 )
	{
		return true;
	}

	kInfo.m_iOK	= NetError::ERR_CHANNEL_00;

	START_LOG( cerr, L"Failed to enter the PvP channel." )
		<< BUILD_LOG( spUser_->GetCharName() )
		<< BUILD_LOG( spUser_->GetName() )
		<< BUILD_LOG( kInfo.m_iConnectChannelID )
		<< BUILD_LOG( NetError::GetErrStr( kInfo.m_iOK ) )
		<< END_LOG;

	return false;
}

bool KRoomListManager::ChangePVPRoomList( IN KGSUserPtr spUser_, IN UINT uiRoomListID )
{
	SET_ERROR( NET_OK );

	if( GetRoomListType( uiRoomListID ) != KRoomList::RLT_PVP  &&  GetRoomListType( uiRoomListID ) != KRoomList::RLT_FREE_PVP )
	{
		SET_ERROR( ERR_CHANNEL_04 );
		return false;
	}

	KRoomListPtr spRoomList = Get( uiRoomListID );
	if( spRoomList == NULL )
	{
		SET_ERROR( ERR_CHANNEL_04 );
		return false;
	}

	if( spRoomList->GetPVPChannelClass() != KPVPChannelInfo::PCC_FREE  &&
		spRoomList->GetPVPChannelClass() != KPVPChannelInfo::PCC_PLAY  &&
		spRoomList->GetPVPChannelClass() != KPVPChannelInfo::PCC_TOURNAMENT )
	{
		START_LOG( cerr, L"This channel is unavailable after the PvP rework!" )
			<< BUILD_LOG( uiRoomListID )
			<< BUILD_LOG( spRoomList->GetPVPChannelClass() )
			<< END_LOG;

		SET_ERROR( ERR_CHANNEL_04 );
		return false;
	}

	KRoomListPtr spRoomListNow = Get( spUser_->GetRoomListID() );
	if( spRoomListNow == NULL )
	{
		START_LOG( cerr, L"Channel does not exist!" )
			<< BUILD_LOG( spUser_->GetCharUID() )
			<< BUILD_LOG( spUser_->GetCharName() )
			<< END_LOG;

		SET_ERROR( ERR_CHANNEL_02 );
		return false;
	}

	if( spRoomList->GetPVPChannelClass() == KPVPChannelInfo::PCC_TOURNAMENT )
	{
		if( SiKGameEventManager()->CheckTournamentEvent() == false )
		{
			SET_ERROR( ERR_CHANNEL_09 );
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// [이벤트] (2010-07-29 ~ 2010-08-11) 대회채널 입장 조건
		//////////////////////////////////////////////////////////////////////////
	}

	spUser_->SetRoomListID( spRoomList->GetID() );
	//{{ 2009. 5. 3  최육사		대전유저리스트
	spUser_->SendUpdatePVPUserInfo( KCommunityUserInfo::US_PVP_LOBBY );
	//}}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//}}

KRoomList::ROOM_LIST_TYPE KRoomListManager::GetRoomListType( IN UINT uiRoomListID )
{
	KRoomListPtr spRoomList = Get( uiRoomListID );

	if( spRoomList == NULL )
	{
		START_LOG( cerr, L"Invalid destination channel pointer!" )
			<< BUILD_LOG( uiRoomListID )
			<< END_LOG;

		return KRoomList::RLT_NONE;
	}

	return spRoomList->GetType();
}

int KRoomListManager::GetPVPChannelClassByRoomListID( IN UINT uiRoomListID )
{
	switch( GetRoomListType( uiRoomListID ) )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KRoomListPtr spRoomList = Get( uiRoomListID );

			//GetRoomListType함수 안에서 해당채널 존재확인을 하였으므로 2차확인은 하지 않는다.
			return spRoomList->GetPVPChannelClass();
		}
		break;

	default:
		{
			START_LOG( cerr, L"Requested a PvP class function outside a PvP channel!" )
				<< BUILD_LOG( uiRoomListID )
				<< END_LOG;
		}
	}

	return KPVPChannelInfo::PCC_NONE;
}

int	KRoomListManager::GetPVPChannelClassByRoomUID( IN UidType iRoomUID )
{
	KRoomInfoPtr spRoomInfo = GetRoomInfo( iRoomUID );
	if( spRoomInfo == NULL )
	{
		START_LOG( cwarn, L"Room does not exist." ) // 시간차로 생긴 문제일 확률이 높다
			<< BUILD_LOG( iRoomUID )
			<< END_LOG;

		return KPVPChannelInfo::PCC_NONE;
	}

	if( spRoomInfo->m_RoomType != CXSLRoom::RT_PVP )
	{
		START_LOG( cerr, L"Requested PVP Channel Class outside a PvP room!" )
			<< BUILD_LOG( iRoomUID )
			<< BUILD_LOGc( spRoomInfo->m_RoomType )
			<< END_LOG;

		return KPVPChannelInfo::PCC_NONE;
	}

	return spRoomInfo->m_iPVPChannelClass;
}

UidType KRoomListManager::GetWaitingRoomUID( IN UINT uiRoomListID, IN KEGS_QUICK_JOIN_REQ& kReq )
{
	switch( GetRoomListType( uiRoomListID ) )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KRoomListPtr spRoomList = Get( uiRoomListID );

			//GetRoomListType함수 안에서 해당채널 존재확인을 하였으므로 2차확인은 하지 않는다.
			return spRoomList->GetWaitingRoomUID( kReq );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Requested a waiting-room function outside a PvP or dungeon channel!" )
				<< BUILD_LOG( uiRoomListID )
				<< BUILD_LOG( kReq.m_iOption )
				<< BUILD_LOG( static_cast<int>(kReq.m_bIsItemMode) )
				<< END_LOG;
		}
	}

	return 0;
}

bool KRoomListManager::GetRoomInfoPage( IN UINT uiRoomListID, IN KEGS_ROOM_LIST_REQ& kReq, OUT KEGS_ROOM_LIST_ACK& kAck )
{
	switch( GetRoomListType( uiRoomListID ) )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KRoomListPtr spRoomList = Get( uiRoomListID );

			//GetRoomListType함수 안에서 해당채널 존재확인을 하였으므로 2차확인은 하지 않는다.
			return spRoomList->GetRoomInfoPage( kReq, kAck );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Requested the room list outside a PvP or dungeon room!" )
				<< BUILD_LOG( uiRoomListID )
				<< END_LOG;
		}
	}

	return false;
}

bool KRoomListManager::GetSquareInfoPage( IN KEGS_SQUARE_LIST_REQ& kReq, OUT KEGS_SQUARE_LIST_ACK& kAck )
{
	if( m_spSquare == NULL )
	{
		START_LOG( cerr, L"Invalid square channel pointer!" )
			<< BUILD_LOG( m_spSquare )
			<< END_LOG;

		kAck.m_iOK	= NetError::ERR_CHANNEL_06;
		return false;
	}

	if( GetRoomListType( m_spSquare->GetID() ) == KRoomList::RLT_SQUARE )
	{
		KRoomListPtr spRoomList = Get( m_spSquare->GetID() );

		return spRoomList->GetSquareInfoPage( kReq, kAck );
	}

	START_LOG( cerr, L"Requested the plaza list from a non-plaza channel!" )
		<< END_LOG;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//방관리
bool KRoomListManager::AddRoomInfo( const KRoomInfo& kRoomInfo )
{
	std::map< UidType, KRoomInfoPtr >::iterator mit;
	mit = m_mapRoomInfoList.find( kRoomInfo.m_RoomUID );

	if( mit != m_mapRoomInfoList.end() )
	{
		START_LOG( cwarn, L"Attempted to add room information for an existing ID; possible server hang?" )
			<< BUILD_LOG( kRoomInfo.m_RoomUID )
			<< BUILD_LOG( kRoomInfo.m_RoomName )
			<< END_LOG;

		DeleteRoomInfo( kRoomInfo.m_uiRoomListID, mit->second );

		m_mapRoomInfoList.erase( mit );
	}

	KRoomInfoPtr spRoomInfo = KRoomInfoPtr( new KRoomInfo( kRoomInfo ) );

	if( InsertRoomInfo( kRoomInfo.m_uiRoomListID, spRoomInfo ) == false )
	{
		return false;
	}
	else
	{
		//채널에 먼저 방정보 추가가 성공해야 매니저에 넣는다.
		m_mapRoomInfoList.insert( std::make_pair( kRoomInfo.m_RoomUID, spRoomInfo ) );
	}

	switch( kRoomInfo.m_RoomType )
	{
	case CXSLRoom::RT_PVP:
		ChangeNumPvPRoom( kRoomInfo.m_PVPGameType, 1 );
		break;

	default:
		break;
	}

	return true;
}

bool KRoomListManager::ChangeRoomInfo( const KRoomInfo& kRoomInfo )
{
	std::map< UidType, KRoomInfoPtr >::iterator mit;
	mit = m_mapRoomInfoList.find( kRoomInfo.m_RoomUID );

	if( mit != m_mapRoomInfoList.end() )
	{
		*( mit->second ) = kRoomInfo;

		return true;
	}

	START_LOG( cwarn, L"Room information is missing during update (RoomUID is missing)." )
		<< BUILD_LOG( kRoomInfo.m_RoomUID )
		<< BUILD_LOG( kRoomInfo.m_RoomName );

	// 070604. florist. 수정할 방 정보가 존재하지 않는 경우 바로 추가.
	return AddRoomInfo( kRoomInfo );
}

KRoomInfoPtr KRoomListManager::GetRoomInfo( const UidType nRoomUID )
{
	std::map< UidType, KRoomInfoPtr >::const_iterator mit;
	mit = m_mapRoomInfoList.find( nRoomUID );

	if( mit == m_mapRoomInfoList.end() )
	{
		// [참고] 시간차로 방이 존재하지 않을 수 있다.
		START_LOG( cwarn, L"No room information is available to read." )
			<< BUILD_LOG( nRoomUID )
			<< END_LOG;

		return KRoomInfoPtr();
	}

	return mit->second;
}

bool KRoomListManager::DelRoomInfo( const KRoomInfo& kRoomInfo )
{
	std::map< UidType, KRoomInfoPtr >::iterator mit;
	mit = m_mapRoomInfoList.find( kRoomInfo.m_RoomUID );

	if( mit == m_mapRoomInfoList.end() )
	{
		START_LOG( cerr, L"Could not find the room to delete." )
			<< BUILD_LOG( kRoomInfo.m_RoomUID )
			<< END_LOG;

		return false;
	}

	if( DeleteRoomInfo( kRoomInfo.m_uiRoomListID, mit->second ) == false )
	{
		START_LOG( cerr, L"Invalid channel information while deleting room information." )
			<< BUILD_LOG( kRoomInfo.m_RoomUID )
			<< BUILD_LOG( kRoomInfo.m_uiRoomListID )
			<< BUILD_LOGc( kRoomInfo.m_RoomType )
			<< BUILD_LOGc( kRoomInfo.m_RoomState )
			<< END_LOG;
	}

	m_mapRoomInfoList.erase( mit );
	return true;
}

int KRoomListManager::DelRoomInfo( const UidType nCenterUID )
{
	int nDelCount = 0;

	std::map< UidType, KRoomInfoPtr >::iterator	mit;
	for( mit = m_mapRoomInfoList.begin(); mit != m_mapRoomInfoList.end();/*mit++*/)
	{
		if( KncUid::ExtractServerID( mit->first ) == nCenterUID )	// 해당 센터에서 만들어졌던 방인 경우
		{
			DeleteRoomInfo( mit->second->m_uiRoomListID, mit->second );

			mit = m_mapRoomInfoList.erase( mit );	// 방 정보를 삭제하고

			++nDelCount;	// 삭제된 방의 갯수를 센다.
		}
		else
		{
			mit++;
		}
	}

	return nDelCount;
}

void KRoomListManager::GetNumPvPRoom( std::map< int, int > &mapNumPvPRoom )
{
	mapNumPvPRoom = m_mapNumPvPRoom;
}

void KRoomListManager::ChangeNumPvPRoom( int iGameMode, int iChange )
{
	std::map< int, int >::iterator mit;
	mit = m_mapNumPvPRoom.find( iGameMode );

	int iTotalValue;
	if( mit != m_mapNumPvPRoom.end() )
	{
		mit->second += iChange;
		iTotalValue = mit->second;
	}
	else
	{
		m_mapNumPvPRoom.insert( std::make_pair( iGameMode, iChange ) );
		iTotalValue = iChange;
	}

	if( iTotalValue < 0 )
	{
		START_LOG( cerr, L"Invalid room count." )
			<< BUILD_LOG( iGameMode )
			<< BUILD_LOG( iChange )
			<< END_LOG;
	}
}

//////////////////////////////////////////////////////////////////////////
//광장 관리
bool KRoomListManager::AddSquareInfo( const KSquareInfo& kSquareInfo )
{
	std::map< UidType, KSquareInfoPtr >::iterator mit;
	mit = m_mapSquareInfoList.find( kSquareInfo.m_iSquareUID );

	if( mit != m_mapSquareInfoList.end() )
	{
		START_LOG( cwarn, L"Attempted to add existing plaza information; possible server hang?" )
			<< BUILD_LOG( kSquareInfo.m_iSquareUID )
			<< BUILD_LOG( kSquareInfo.m_wstrRoomName )
			<< END_LOG;

		*(mit->second) = kSquareInfo;

		return false;
	}

	KSquareInfoPtr spSquareInfo = KSquareInfoPtr( new KSquareInfo( kSquareInfo ) );

	if( m_spSquare == NULL )
	{
		START_LOG( cerr, L"Invalid square channel pointer!" )
			<< BUILD_LOG( m_spSquare )
			<< END_LOG;

		return false;
	}

	m_mapSquareInfoList.insert( std::make_pair( kSquareInfo.m_iSquareUID, spSquareInfo ) );

	return m_spSquare->AddSquareInfo( spSquareInfo );
}

bool KRoomListManager::ChangeSquareInfo( const KSquareInfo& kSquareInfo )
{
	std::map< UidType, KSquareInfoPtr >::iterator mit;
	mit = m_mapSquareInfoList.find( kSquareInfo.m_iSquareUID );

	if( mit != m_mapSquareInfoList.end() )
	{
		*( mit->second ) = kSquareInfo;

		return true;
	}


	START_LOG( cwarn, L"Room information is missing while updating plaza data (iSquareUID is missing)." )
		<< BUILD_LOG( kSquareInfo.m_iSquareUID )
		<< BUILD_LOG( kSquareInfo.m_wstrRoomName );

	// 070626. 수정할 광장 정보가 존재하지 않는 경우 바로 추가.
	return AddSquareInfo( kSquareInfo );
}

KSquareInfoPtr KRoomListManager::GetSquareInfo( const UidType iSquareUID )
{
	std::map< UidType, KSquareInfoPtr >::iterator mit;
	mit = m_mapSquareInfoList.find( iSquareUID );

	if( mit == m_mapSquareInfoList.end() )
	{
		START_LOG( cerr, L"Square information is missing!" )
			<< BUILD_LOG( iSquareUID )
			<< END_LOG;

		return KSquareInfoPtr();
	}

	return mit->second;
}

bool KRoomListManager::DelSquareInfo( const KSquareInfo& kSquareInfo )
{
	std::map< UidType, KSquareInfoPtr >::iterator mit;
	mit = m_mapSquareInfoList.find( kSquareInfo.m_iSquareUID );

	if( mit == m_mapSquareInfoList.end() )
	{
		START_LOG( cerr, L"Plaza information to delete does not exist." )
			<< BUILD_LOG( kSquareInfo.m_iSquareUID )
			<< BUILD_LOG( kSquareInfo.m_wstrRoomName )
			<< END_LOG;

		return false;
	}

	if( m_spSquare == NULL )
	{
		START_LOG( cerr, L"Invalid square channel pointer!" )
			<< BUILD_LOG( m_spSquare )
			<< END_LOG;

		return false;
	}

	m_spSquare->DelSquareInfo( mit->second );
	m_mapSquareInfoList.erase( mit );

	return true;
}

int KRoomListManager::DelSquareInfo( const UidType nCenterUID )
{
	int nDelCount = 0;

	std::map< UidType, KSquareInfoPtr >::iterator mit;
	for( mit = m_mapSquareInfoList.begin(); mit != m_mapSquareInfoList.end();/*mit++*/)
	{
		if( KncUid::ExtractServerID( mit->first ) == nCenterUID )	// 해당 센터에서 만들어졌던 광장인 경우
		{
			if( mit->second != NULL )
			{
				m_spSquare->DelSquareInfo( mit->second );
			}

			mit = m_mapSquareInfoList.erase( mit );	// 광장 정보를 삭제하고

			++nDelCount;	// 삭제된 방의 갯수를 센다.
		}
		else
		{
			mit++;
		}
	}

	return nDelCount;
}

KRoomListPtr KRoomListManager::Get( UINT uiRoomListID )
{
	std::map< UINT, KRoomListPtr >::iterator mit;
	mit = m_mapRoomList.find( uiRoomListID );

	if( mit == m_mapRoomList.end() )
	{
		START_LOG( cwarn, L"Channel does not exist" )
			<< BUILD_LOG( uiRoomListID )
			<< BUILD_LOG( m_mapRoomList.size() );

		return KRoomListPtr();
	}

	return mit->second;
}

KRoomListPtr KRoomListManager::GetPVPFreeChannel()
{
	std::map< UINT, KRoomListPtr >::iterator mit;

	for( UINT i = m_nStartPVPID; i < 100; ++i )
	{
		mit = m_mapRoomList.find( i );

		if( mit->second != NULL )
		{
			if( mit->second->GetPVPChannelClass() == KPVPChannelInfo::PCC_FREE )
			{
				return mit->second;
			}
		}
		else
			break;

		if( mit == m_mapRoomList.end() )
		{
			break;
		}
	}

	START_LOG( cerr, L"Invalid free-channel pointer!" )
		<< END_LOG;

	return KRoomListPtr();
}

bool KRoomListManager::AddRoomListTemplet_LUA()
{
	// 채널생성 함수바인딩시
	// 던전의 마을 아이디 할당을 위해 클라이언트 스크립인
	// DLG_Map_Enum.lua 파싱후 호출한다.

	KLuaManager luaManager( g_pLua );
	TableBind( &luaManager );

	UINT	nTemp01 = 0;
	UINT	nTemp02 = 0;
	int		nTemp03 = 0;
	int		nTemp04 = 0;

	LUA_GET_VALUE( luaManager, "ROOM_LIST_ID", nTemp01, 0 );

	if( nTemp01 == 0 )
	{
		START_LOG( cerr, L"Failed to parse channel ID!" )
			<< BUILD_LOG( nTemp01 )
			<< END_LOG;

		return false;
	}

	LUA_GET_VALUE( luaManager, "ROOM_LIST_TYPE", nTemp02, 0 );

	if( KRoomList::RLT_NONE >= nTemp02 || KRoomList::RLT_END <= nTemp02 )
	{
		START_LOG( cerr, L"Invalid channel type!" )
			<< BUILD_LOG( nTemp01 )
			<< BUILD_LOG( nTemp02 )
			<< END_LOG;

		return false;
	}

	switch( nTemp02 )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KPVPListPtr spPVPList = KPVPListPtr( new KPVPList( nTemp01, (KRoomList::ROOM_LIST_TYPE)nTemp02, PVP_PAGE_NUM ) );

			if( spPVPList == NULL )
			{
				START_LOG( cerr, L"Failed to create room list!" )
					<< L"ROOM_LIST_ID"	<< BUILD_LOG( nTemp01 )
					<< L"ROOM_LIST_TYPE"	<< BUILD_LOG( nTemp02 )
					<< END_LOG;

				return false;
			}

			LUA_GET_VALUE( luaManager, "MIN_VP", nTemp03, 0 );
			LUA_GET_VALUE( luaManager, "MAX_VP", nTemp04, 0 );

			if( nTemp03 == 0 && nTemp04 == 0 )
			{
				START_LOG( cerr, L"Failed to set PvP channel VS pointer!" )
					<< BUILD_LOG ( nTemp03 )
					<< BUILD_LOG ( nTemp04 )
					<< END_LOG;
			}
			spPVPList->SetVSPoint( nTemp03, nTemp04 );

			LUA_GET_VALUE( luaManager, "PVP_CLASS", nTemp04, 0 );

			if( nTemp01 == 0 )
			{
				START_LOG( cerr, L"Invalid PvP channel class value!" )
					<< BUILD_LOG( nTemp04 )
					<< END_LOG;
			}

			//if( nTemp02 == KChannel::CT_FREE_PVP && nTemp04 != KPVPChannelInfo::PCC_TOURNAMENT )
			//{
			//	m_spFreePvP = spPVPChannel;
			//}
			//else if( nTemp02 == KChannel::CT_FREE_PVP && nTemp04 == KPVPChannelInfo::PCC_TOURNAMENT )
			//{
			//	m_spTournamentPvP = spPVPChannel;
			//}

			spPVPList->SetPVPChannelClass( nTemp04 );

			m_mapRoomList.insert( std::make_pair( spPVPList->GetID(), spPVPList ) );
		}
		break;

	case KRoomList::RLT_SQUARE:
		{
			KSquareListPtr spSquareList = KSquareListPtr( new KSquareList( nTemp01, (KRoomList::ROOM_LIST_TYPE)nTemp02, SQUARE_PAGE_NUM ) );

			if( spSquareList == NULL )
			{
				START_LOG( cerr, L"Failed to create plaza list!" )
					<< L"ROOM_LIST_ID"	<< BUILD_LOG( nTemp01 )
					<< L"ROOM_LIST_TYPE"	<< BUILD_LOG( nTemp02 )
					<< END_LOG;

				return false;
			}

			m_spSquare = spSquareList;

			m_mapRoomList.insert( std::make_pair( spSquareList->GetID(), spSquareList ) );
		}
		break;
	}

	return true;
}

//{{ 2011. 07. 04	최육사	대전 개편
void KRoomListManager::SetPvpOffcialRoomListID_LUA( UINT nID )
{
    m_nPvpOffcialRoomListID = nID;

	START_LOG( cout, L"Set official PvP channel RoomListID: " << nID );
}
//}}

bool KRoomListManager::InsertRoomInfo( IN UINT uiRoomListID, IN KRoomInfoPtr spRoomInfo )
{
	switch( GetRoomListType( uiRoomListID ) )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KRoomListPtr spRoomList = Get( uiRoomListID );

			//GetRoomListType함수 안에서 해당채널 존재확인을 하였으므로 2차확인은 하지 않는다.
			return spRoomList->AddRoomInfo( spRoomInfo );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Attempted to add room information to a non-room channel." )
				<< BUILD_LOG( uiRoomListID )
				<< BUILD_LOG( spRoomInfo->m_RoomUID )
				<< BUILD_LOG( spRoomInfo->m_uiRoomListID )
				<< END_LOG;
		}
	}

	return false;
}

bool KRoomListManager::DeleteRoomInfo( IN UINT uiRoomListID, IN KRoomInfoPtr spRoomInfo )
{
	bool bRet = false;

	switch( GetRoomListType( uiRoomListID ) )
	{
	case KRoomList::RLT_PVP:
	case KRoomList::RLT_FREE_PVP:
		{
			KRoomListPtr spRoomList = Get( uiRoomListID );

			//GetRoomListType함수 안에서 해당채널 존재확인을 하였으므로 2차확인은 하지 않는다.
			bRet = spRoomList->DelRoomInfo( spRoomInfo );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Attempted to add room information to a non-room channel." )
				<< BUILD_LOG( uiRoomListID )
				<< BUILD_LOG( spRoomInfo->m_RoomUID )
				<< BUILD_LOG( spRoomInfo->m_uiRoomListID )
				<< END_LOG;
		}
	}

	switch( spRoomInfo->m_RoomType )
	{
	case CXSLRoom::RT_PVP:
		ChangeNumPvPRoom( spRoomInfo->m_PVPGameType, -1 );
		break;

	default:
		break;
	}

	return bRet;
}
