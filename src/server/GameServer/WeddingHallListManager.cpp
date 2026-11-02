#include "NetError.h"
#include "GSSimLayer.h"
#include "WeddingHallListManager.h"


//{{ 2013. 04. 01	 인연 시스템 - 김민성


ImplementSingleton( KWeddingHallListManager );

KWeddingHallListManager::KWeddingHallListManager(void)
{
// 	lua_tinker::class_add<KWeddingHallListManager>( g_pLua, "CXSLItemManager" );
// 	lua_tinker::class_def<KWeddingHallListManager>( g_pLua, "dump",						&KWeddingHallListManager::Dump );
// 	lua_tinker::decl( g_pLua, "BattleFieldListManager", this );
}

KWeddingHallListManager::~KWeddingHallListManager(void)
{
}

ImplToStringW( KWeddingHallListManager )
{
	stm_	<< L"----------[ Wedding Hall List Manager ]----------" << std::endl
			<< TOSTRINGW( m_mapWeddingHallList.size() )
			;

	return stm_;
}

KGSWeddingHallPtr KWeddingHallListManager::GetWeddingHallInfo( IN const int iWeddingUid )
{
	std::map< int, KGSWeddingHallPtr >::iterator mit;
	mit = m_mapWeddingHallList.find( iWeddingUid );
	if( mit == m_mapWeddingHallList.end() )
	{
		START_LOG( cerr, L"The specified wedding hall information does not exist!" )
			<< BUILD_LOG( iWeddingUid )
			<< END_LOG;

		return KGSWeddingHallPtr();
	}

	return mit->second;
}

void KWeddingHallListManager::AddWeddingHallInfo( IN const KGSWeddingHall& kHallInfo )
{
	if( IsExist( kHallInfo.GetWeddingHallUID() ) == true )
	{
		// 존재하는 결혼식장 입니다.
		START_LOG( cerr, L"Wedding hall already exists." )
			<< BUILD_LOG( kHallInfo.GetWeddingHallUID() )
			<< BUILD_LOG( kHallInfo.GetRoomUID() )
			<< END_LOG;

		return;
	}

	KGSWeddingHallPtr spHallInfo( new KGSWeddingHall( kHallInfo ) );
	m_mapWeddingHallList.insert( std::make_pair( kHallInfo.GetWeddingHallUID(), spHallInfo ) );
}

void KWeddingHallListManager::UpdateWeddingHallInfo( IN const KGSWeddingHall& kHallInfo )
{
	std::map< int, KGSWeddingHallPtr >::iterator mit;
	mit = m_mapWeddingHallList.find( kHallInfo.GetWeddingHallUID() );
	if( mit == m_mapWeddingHallList.end() )
	{
		START_LOG( cerr, L"The specified wedding hall information does not exist!" )
			<< BUILD_LOG( kHallInfo.GetWeddingHallUID() )
			<< END_LOG;

		return;
	}

	mit->second->UpdateRoomInfo( kHallInfo );
}

void KWeddingHallListManager::UpdateWeddingHallInfo( IN const KWeddingHallInfo& kInfo, IN std::vector< UidType >& vecUnitList )
{
	std::map< int, KGSWeddingHallPtr >::iterator mit;
	mit = m_mapWeddingHallList.find( kInfo.m_iWeddingUID );
	if( mit == m_mapWeddingHallList.end() )
	{
		START_LOG( cerr, L"The specified wedding hall information does not exist!" )
			<< BUILD_LOG( kInfo.m_iWeddingUID )
			<< END_LOG;

		return;
	}

	std::set< UidType > setUnitList;
	for( UidType iUnitUID : vecUnitList )
	{
		setUnitList.insert( iUnitUID );
	}

	mit->second->UpdateRoomInfo( kInfo, setUnitList );
}

void KWeddingHallListManager::DeleteWeddingHallInfo( IN const KGSWeddingHall& kHallInfo )
{
	std::map< int, KGSWeddingHallPtr >::iterator mit;
	mit = m_mapWeddingHallList.find( kHallInfo.GetWeddingHallUID() );
	if( mit == m_mapWeddingHallList.end() )
	{
		START_LOG( cerr, L"The specified wedding hall information does not exist!" )
			<< BUILD_LOG( kHallInfo.GetWeddingHallUID() )
			<< END_LOG;

		return;
	}

	m_mapWeddingHallList.erase( kHallInfo.GetWeddingHallUID() );
}

void KWeddingHallListManager::DeleteWeddingHallInfo( IN const int iWeddingUID )
{
	std::map< int, KGSWeddingHallPtr >::iterator mit;
	mit = m_mapWeddingHallList.find( iWeddingUID );
	if( mit == m_mapWeddingHallList.end() )
	{
		START_LOG( cerr, L"The specified wedding hall information does not exist!" )
			<< BUILD_LOG( iWeddingUID )
			<< END_LOG;

		return;
	}

	m_mapWeddingHallList.erase( iWeddingUID );
}

void KWeddingHallListManager::DelRoomInfoByCnServerUID( IN const UidType iCnServerUID )
{
	std::vector< int > vecWeddingHallUID;

	std::map< int, KGSWeddingHallPtr >::iterator mit;
	for( mit = m_mapWeddingHallList.begin(); mit != m_mapWeddingHallList.end(); ++mit )
	{
		KGSWeddingHallPtr spHallInfo = mit->second;
		if( IS_NULL( spHallInfo ) )
		{
			START_LOG( cerr, L"The pointer is invalid!" )
				<< BUILD_LOG( mit->first )
				<< END_LOG;
			continue;
		}

		if( KncUid::ExtractServerID( spHallInfo->GetRoomUID() ) == iCnServerUID )
		{
			vecWeddingHallUID.push_back( spHallInfo->GetWeddingHallUID() );
		}
	}

	for( int iWeddingHallUID : vecWeddingHallUID )
	{
		m_mapWeddingHallList.erase( iWeddingHallUID );
	}
}
//}
