#include "./RecommendUserTable.h"


ImplementRefreshSingleton( KRecommendUserTable );

KRecommendUserTable::KRecommendUserTable(void)
{
}

KRecommendUserTable::~KRecommendUserTable(void)
{
}

ImplToStringW( KRecommendUserTable )
{
	stm_	<< L"----------[ Recommend User Table ]----------" << std::endl
			<< TOSTRINGW( m_mapRecommendData.size() )
		;

	return stm_;
}

ImplementLuaScriptParser( KRecommendUserTable )
{
	lua_tinker::class_add<KRecommendUserTable>( GetLuaState(), "KRecommendUserTable" );
	lua_tinker::class_def<KRecommendUserTable>( GetLuaState(), "AddRecommendUserData",	&KRecommendUserTable::AddRecommendUserData_LUA );
	lua_tinker::class_def<KRecommendUserTable>( GetLuaState(), "dump",					&KRecommendUserTable::Dump );

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
	//}

	lua_tinker::decl( GetLuaState(), "g_pKRecommendUserTable", this );
}

int KRecommendUserTable::GetRecommendUserReward( IN u_char ucLevel ) const
{
	std::map< u_char, int >::const_iterator mit;
	mit = m_mapRecommendData.find( ucLevel );
	if( mit == m_mapRecommendData.end() )
		return 0;

	return mit->second;
}

bool KRecommendUserTable::AddRecommendUserData_LUA( u_char ucLevel, int iRewardID )
{
	if( ucLevel <= 1 || iRewardID <= 0 )
	{
		START_LOG( cerr, L"Failed to parse recommender table information!" )
			<< BUILD_LOG( ucLevel )
			<< BUILD_LOG( iRewardID )
			<< END_LOG;

		return false;
	}

	m_mapRecommendData.insert( std::make_pair( ucLevel, iRewardID ) );
	return true;
}

//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
//}
