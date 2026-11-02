#include "NetError.h"
#include "GSSimLayer.h"
#include "./BattleFieldListManager.h"


//{{ 2011. 10. 27	최육사	배틀필드 시스템


ImplementSingleton( KBattleFieldListManager );

KBattleFieldListManager::KBattleFieldListManager(void)
{
	lua_tinker::class_add<KBattleFieldListManager>( g_pLua, "CXSLItemManager" );
	lua_tinker::class_def<KBattleFieldListManager>( g_pLua, "dump",						&KBattleFieldListManager::Dump );
	lua_tinker::decl( g_pLua, "BattleFieldListManager", this );
}

KBattleFieldListManager::~KBattleFieldListManager(void)
{
}

ImplToStringW( KBattleFieldListManager )
{
	stm_	<< L"----------[ Battle Field List Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapBattleFieldList.size() )
			;

	return stm_;
}

KBattleFieldListPtr KBattleFieldListManager::GetBattleFieldRoomList( IN const int iBattleFieldID )
{
	std::map< int, KBattleFieldListPtr >::iterator mit;
	mit = m_mapBattleFieldList.find( iBattleFieldID );
	if( mit == m_mapBattleFieldList.end() )
	{
		START_LOG( cerr, L"Battlefield list information does not exist!" )
			<< BUILD_LOG( iBattleFieldID )
			<< END_LOG;

		return KBattleFieldListPtr();
	}

	return mit->second;
}

void KBattleFieldListManager::UpdateBattleFieldRoomInfo( IN const KBattleFieldRoomInfo& kRoomInfo )
{
	if( IsExist( kRoomInfo.m_iBattleFieldID ) == false )
	{
		KBattleFieldListPtr spRoomList( new KBattleFieldList );
		spRoomList->AddRoomInfo( kRoomInfo );
		m_mapBattleFieldList.insert( std::make_pair( kRoomInfo.m_iBattleFieldID, spRoomList ) );
		return;
	}

	KBattleFieldListPtr spRoomList = GetBattleFieldRoomList( kRoomInfo.m_iBattleFieldID );
	if( spRoomList == NULL )
	{
		START_LOG( cerr, L"Invalid room list information pointer!" )
			<< BUILD_LOG( kRoomInfo.m_iBattleFieldID )
			<< END_LOG;
		return;
	}

	if( spRoomList->IsExist( kRoomInfo.m_RoomUID ) == false )
	{
		spRoomList->AddRoomInfo( kRoomInfo );
	}
	else
	{
		spRoomList->UpdateRoomInfo( kRoomInfo );
	}
}

void KBattleFieldListManager::DeleteBattleFieldRoomInfo( IN const KBattleFieldRoomInfo& kRoomInfo )
{
	if( IsExist( kRoomInfo.m_iBattleFieldID ) == false )
	{
		START_LOG( cerr, L"Room list for this battlefield does not exist!" )
			<< BUILD_LOG( kRoomInfo.m_iBattleFieldID )
			<< END_LOG;
		return;
	}

	KBattleFieldListPtr spRoomList = GetBattleFieldRoomList( kRoomInfo.m_iBattleFieldID );
	if( spRoomList == NULL )
	{
		START_LOG( cerr, L"Invalid room list information pointer!" )
			<< BUILD_LOG( kRoomInfo.m_iBattleFieldID )
			<< END_LOG;
		return;
	}

	LIF( spRoomList->DelRoomInfo( kRoomInfo.m_RoomUID ) );
}

//{{ 2012. 11. 20	최육사		배틀필드 방 정리 기능 추가
void KBattleFieldListManager::DeleteBattleFieldRoomInfo( IN const UidType iCnServerUID )
{
	[[maybe_unused]] int nDelCount = 0;

	std::map< int, KBattleFieldListPtr >::iterator mit;
	for( mit = m_mapBattleFieldList.begin(); mit != m_mapBattleFieldList.end(); ++mit )
	{
		KBattleFieldListPtr spBattleFieldList = mit->second;
		if( IS_NULL( spBattleFieldList ) )
		{
			START_LOG( cerr, L"The pointer is invalid!" )
				<< BUILD_LOG( mit->first )
				<< END_LOG;
			continue;
		}

		spBattleFieldList->DelRoomInfoByCnServerUID( iCnServerUID );
	}
}
//}}

bool KBattleFieldListManager::GetRoomUIDForJoinBattleField( IN const int iBattleFieldID, IN const UidType iUnitUID, IN const UidType iTargetRoomUID, IN const UidType iPartyUID, OUT UidType& iJoinBFRoomUID )
{
	SET_ERROR( NET_OK );

	iJoinBFRoomUID = 0;

	if( IsExist( iBattleFieldID ) == false )
	{
		// 이런 경우 새로운 배틀필드방을 만들어서 입장해야하므로 true를 리턴한다. ( 오류가 아니다 )
		return true;
	}

	KBattleFieldListPtr spRoomList = GetBattleFieldRoomList( iBattleFieldID );
	if( IS_NULL( spRoomList ) )
	{
		START_LOG( cerr, L"Invalid pointer to nonexistent battlefield room list information!" )
			<< BUILD_LOG( iBattleFieldID )
			<< END_LOG;

		SET_ERROR( ERR_BATTLEFIELD_04 );
		return false;
	}

	return spRoomList->GetRoomUIDForJoinBattleField( iUnitUID, iTargetRoomUID, iPartyUID, iJoinBFRoomUID );
}


//}}
