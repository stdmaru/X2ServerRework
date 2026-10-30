#include "./RewardTable.h"
#include "X2Data/XSLItemManager.h"


ImplementRefreshSingleton( KRewardTable );

KRewardTable::KRewardTable(void)
{
}

KRewardTable::~KRewardTable(void)
{
}

ImplToStringW( KRewardTable )
{
	stm_	<< L"----------[ Reward Table ]----------" << std::endl
			<< TOSTRINGW( m_mapRewardData.size() )
		;

	return stm_;
}

ImplementLuaScriptParser( KRewardTable )
{
	lua_tinker::class_add<KRewardTable>( GetLuaState(), "KRewardTable" );
	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	lua_tinker::class_def<KRewardTable>( GetLuaState(), "AddRewardData",	&KRewardTable::AddRewardData_LUA );
	lua_tinker::class_def<KRewardTable>( GetLuaState(), "dump",				&KRewardTable::Dump );

	//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
	//}}

	lua_tinker::decl( GetLuaState(), "g_pKRewardTable", this );
}

bool KRewardTable::AddRewardData_LUA( int iRewardID, int iItemID, int iQuantity, short sPeriod )
{
	if( iRewardID <= 0  ||  iItemID <= 0  ||  iQuantity <= 0  ||  sPeriod < 0 )
	{
		START_LOG( cerr, L"잘못된 스크립트 정보입니다." )
			<< BUILD_LOG( iRewardID )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( iQuantity )
			<< BUILD_LOG( sPeriod )
			<< END_LOG;
		return false;
	}

	KRewardData kRewardData;
	kRewardData.m_iRewardItemID = iItemID;
	kRewardData.m_iQuantity		= iQuantity;
	kRewardData.m_sPeriod		= sPeriod;

	std::map< int, std::vector< KRewardData > >::iterator mit;
	mit = m_mapRewardData.find( iRewardID );
	if( mit == m_mapRewardData.end() )
	{
		// 없다면..
		std::vector< KRewardData > vecRewardData;
		vecRewardData.push_back( kRewardData );
		m_mapRewardData.insert( std::make_pair( iRewardID, vecRewardData ) );
	}
	else
	{
		mit->second.push_back( kRewardData );
	}

	return true;;
}

void KRewardTable::GetRewardItem( IN const KPostItemInfo& kPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const
{
	// 편지와 공지 메시지의 경우 통과
	//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
	if( KPostItemInfo::IsItemIDLetter( kPostItemInfo.m_cScriptType ) == true )
		return;
	//}}

	std::map< int, std::vector< KRewardData > >::const_iterator mit;

	mit = m_mapRewardData.find( kPostItemInfo.m_iScriptIndex );
	if( mit == m_mapRewardData.end() )

	{
		START_LOG( cerr, L"존재하지 않는 RewardID입니다." )
			<< BUILD_LOG( kPostItemInfo.m_iScriptIndex )
			<< END_LOG;
		return;
	}

	BOOST_TEST_FOREACH( const KRewardData&, kReward, mit->second )
	{
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kReward.m_iRewardItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"아이템 템플릿 정보를 찾을 수 없습니다." )
				<< BUILD_LOG( kReward.m_iRewardItemID )
				<< END_LOG;
			continue;
		}

		// 보상 아이템
		KItemInfo kRewardItemInfo;
		kRewardItemInfo.m_iItemID		= kReward.m_iRewardItemID;
		kRewardItemInfo.m_iQuantity		= kReward.m_iQuantity;
		kRewardItemInfo.m_cUsageType	= pItemTemplet->m_PeriodType;
		kRewardItemInfo.m_sEndurance	= pItemTemplet->m_Endurance;

		// 기간제 아이템의 경우 기간 설정
		if( pItemTemplet->m_PeriodType == CXSLItem::PT_INFINITY )
		{
			kRewardItemInfo.m_sPeriod	= kReward.m_sPeriod;
		}

		// 동일한 ItemID가 있을 시에는 수량만 올려준다.
		std::map< int, KItemInfo >::iterator mitFindSame;
		mitFindSame = mapRewardItem.find( kRewardItemInfo.m_iItemID );
		if( mitFindSame == mapRewardItem.end() )
		{
			mapRewardItem.insert( std::make_pair( kRewardItemInfo.m_iItemID, kRewardItemInfo ) );
		}
		else
		{
			mitFindSame->second.m_iQuantity += kRewardItemInfo.m_iQuantity;
		}
	}
}

void KRewardTable::GetRewardItem( IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, KItemInfo >& mapRewardItem ) const
{
	mapRewardItem.clear();

	BOOST_TEST_FOREACH( const KPostItemInfo&, kPostItem, vecPostItemInfo )
	{
		GetRewardItem( kPostItem, mapRewardItem );
	}
}

bool KRewardTable::GetRewardInfo( IN int iRewardID, OUT std::map< int, int >& mapRewardInfo ) const
{
	std::map< int, std::vector< KRewardData > >::const_iterator mit;

	mit = m_mapRewardData.find( iRewardID );
	if( mit == m_mapRewardData.end() )
	{
		START_LOG( cerr, L"존재하지 않는 RewardID입니다." )
			<< BUILD_LOG( iRewardID )
			<< END_LOG;
		return false;
	}

	BOOST_TEST_FOREACH( const KRewardData&, kReward, mit->second )
	{
		std::map< int, int >::iterator mitFindSame;
		mitFindSame = mapRewardInfo.find( kReward.m_iRewardItemID );
		if( mitFindSame == mapRewardInfo.end() )
		{
			mapRewardInfo.insert( std::make_pair( kReward.m_iRewardItemID, kReward.m_iQuantity ) );
		}
		else
		{
			mitFindSame->second += kReward.m_iQuantity;
		}
	}

	return true;
}

void KRewardTable::GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::map< int, int >& mapRewardInfo ) const
{
	mapRewardInfo.clear();

	BOOST_TEST_FOREACH( const KPostItemInfo&, kPostItem, vecPostItemInfo )
	{
		if( kPostItem.m_cScriptType != static_cast<char>(iRewardType) )
			continue;

		if( GetRewardInfo( kPostItem.m_iScriptIndex, mapRewardInfo ) == false )
			continue;
	}
}

void KRewardTable::GetRewardInfo( IN int iRewardType, IN const std::vector< KPostItemInfo >& vecPostItemInfo, OUT std::vector< std::wstring >& vecMessage ) const
{
	vecMessage.clear();

	BOOST_TEST_FOREACH( const KPostItemInfo&, kPostItem, vecPostItemInfo )
	{
		if( kPostItem.m_cScriptType != static_cast<char>(iRewardType) )
			continue;

		vecMessage.push_back( kPostItem.m_wstrMessage );
	}
}

//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//}}

//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
//}}
