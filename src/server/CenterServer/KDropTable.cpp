#include "KDropTable.h"
#include "X2Data/XSLItemManager.h"
//{{ 2012. 02. 16	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
//}}
#include "X2Data/XSLDungeonManager.h"

ImplementRefreshSingleton( KDropTable );

KDropTable::KDropTable()
{
	//{{ 2009. 6. 17  최육사	대전드롭 - 일단 하드코딩
	m_kPvpDrop.AddCase( CXSLItem::EI_CAMILLAS_COIN, 17.f ); // 카밀라의 코인
	//}}

	//{{ 2009. 6. 29  최육사	드롭 획득 공지
	m_bNotifyDropItem = false;
	//}}

	::ZeroMemory( m_fPVPFailRate, sizeof(float) * PF_END );
	m_dfPVPItemChangeTime = 0;
	//{{ 2011. 03. 28	최육사	서버 상태 체크
	m_bIsDropManyEDEvent = false;
	//}}
}

KDropTable::~KDropTable()
{
}

ImplToStringW( KDropTable )
{
	stm_	<< L"----------[ Drop Table ]----------" << std::endl
			<< TOSTRINGW( m_mapMonsterDrop.size() )
			<< TOSTRINGW( m_mapDropGroup.size() )
			<< TOSTRINGW( m_mapStaticDrop.size() )
			<< TOSTRINGW( m_mapHenirNpcDrop.size() )
			//{{ 2010. 05. 02  최육사	비밀던전 헬모드
			<< TOSTRINGW( m_mapExtraStageNpcDrop.size() )
			//}}
			//{{ 2011. 11. 23	최육사	배틀필드
			<< TOSTRINGW( m_mapBattleFieldNpcDrop.size() )
			<< TOSTRINGW( m_mapBattleFieldStaticDrop.size() )
			//}}
			<< TOSTRINGW( m_vecTimeDrop.size() )
			<< TOSTRINGW( m_mapTimeDrop.size() )
			<< TOSTRINGW( m_kPvpDrop.GetTotalProb() )
			<< TOSTRINGW( m_setNotifyDropItem.size() )
			<< TOSTRINGW( m_bNotifyDropItem )
			<< TOSTRINGW( m_vecPVPDropItem.size() )
			<< TOSTRINGW( m_mapPVPDropItem.size() )
			<< TOSTRINGW( m_fPVPFailRate[PF_WINTEAM] )
			<< TOSTRINGW( m_fPVPFailRate[PF_LOSETEAM] )
			<< TOSTRINGW( m_fPVPFailRate[PF_INC_NUM] )
			<< TOSTRINGW( m_fPVPFailRate[PF_DEC_NUM] )
			<< TOSTRINGW( m_dfPVPItemChangeTime )
			<< TOSTRINGW( m_mapAttribNpcSPDrop.size() )
	//{{ 2009. 12. 18  최육사	던전경험치개편
			<< TOSTRINGW( m_mapNpcExpTable.size() )
	//}}
	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
			<< TOSTRINGW( m_mapComeBackBuffDropRate.size() )
	//}}
	//{{ 2011. 08. 23	최육사	헤니르 시공 개편
			<< TOSTRINGW( m_mapHenirSpecialNpcDrop.size() )
	//}}
		;

	return stm_;
}

ImplementLuaScriptParser( KDropTable )
{
	lua_tinker::class_add<KDropTable>( GetLuaState(), "KDropTable" );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddToGroup",						&KDropTable::AddToGroup_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddMonsterDropInfo",				&KDropTable::AddMonsterDropInfo_LUA );
	//{{ 2009. 7. 25  최육사	헤니르 시공
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddHenirMonsterDropInfo",		&KDropTable::AddHenirMonsterDropInfo_LUA );
	//}}
	//{{ 2010. 05. 02  최육사	비밀던전 헬모드
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddExtraStageMonsterDropInfo",	&KDropTable::AddExtraStageMonsterDropInfo_LUA );
	//}}
	//{{ 2009. 4. 21  최육사	전체드롭테이블
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddStaticDropInfo",				&KDropTable::AddStaticDropInfo_LUA );
	//}}
	//{{ 2011. 11. 23	최육사	배틀필드
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddBattleFieldNpcDropInfo",		&KDropTable::AddBattleFieldNpcDropInfo_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddBattleFieldStaticDropInfo",	&KDropTable::AddBattleFieldStaticDropInfo_LUA );
	//}}
	//{{ 2009. 6. 29  최육사	획득 공지
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddNotifyDropItemInfo",			&KDropTable::AddNotifyDropItemInfo_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "SetNotifyDropItem",				&KDropTable::SetNotifyDropItem_LUA );
	//}}
	lua_tinker::class_def<KDropTable>( GetLuaState(), "dump",							&KDropTable::Dump );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "DumpGroup",						&KDropTable::DumpGroup );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "DumpMonsterDrop",				&KDropTable::DumpMonsterDrop );

	lua_tinker::class_def<KDropTable>( GetLuaState(), "SetPVPFailRate",					&KDropTable::SetPVPFailRate_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "SetPVPItemChangeTime",			&KDropTable::SetPVPItemChangeTime_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddPVPDropInfo",					&KDropTable::AddPVPDropInfo_LUA );
	//{{ 2009. 12. 18  최육사	던전사용아이템
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddAttribNpcSpecialDrop",		&KDropTable::AddAttribNpcSpecialDrop_LUA );
	//}}
	//{{ 2009. 12. 18  최육사	던전경험치개편
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddNpcExpTable",					&KDropTable::AddNpcExpTable_LUA );
	//}}
	//{{ 2011. 03. 28	최육사	서버 상태 체크
	lua_tinker::class_def<KDropTable>( GetLuaState(), "SetManyEDDropNpcEvent",			&KDropTable::SetManyEDDropNpcEvent_LUA );
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddManyEDDropNpc",				&KDropTable::AddManyEDDropNpc_LUA );
	//}}

	//{{ 2011. 05. 04  김민성	던전 클리어시 아이템 지급 조건 추가
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddPaymentDungeon",				&KDropTable::AddPaymentDungeon_LUA );

	//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddUnPaymentCondition",			&KDropTable::AddUnPaymentCondition_LUA );
	//}}

	//}}
	//{{ 2011. 08. 23	최육사	헤니르 시공 개편
	lua_tinker::class_def<KDropTable>( GetLuaState(), "AddHenirSpecialNpcDropInfo",		&KDropTable::AddHenirSpecialNpcDropInfo_LUA );
	//}}
	//{{ 2013. 1. 14	박세훈	던전 및 필드 드랍 아이템에 강화 부여
	lua_tinker::class_def<KDropTable>( GetLuaState(), "SetEnchantItemDropEvent",		&KDropTable::SetEnchantItemDropEvent_LUA );
	//}}

	lua_tinker::decl( GetLuaState(), "DropTable", this );
}

void KDropTable::DumpGroup()
{
    std::wostringstream stm;

    stm << L"  -- Item Group -- " << std::endl;
    std::map<int,KLottery>::const_iterator mit;
    for( mit = m_mapDropGroup.begin(); mit != m_mapDropGroup.end(); ++mit )
    {
        stm << L"  Group ID : " << mit->first << std::endl;
        mit->second.ToString( stm );
    }

    dbg::cout << stm.str();
}

void KDropTable::DumpMonsterDrop()
{
    std::wostringstream stm;

    stm << std::endl << L"  -- Monster Drop Info -- " << std::endl;
    std::map<std::pair<int,int>,KMonsterDropInfo>::const_iterator mit;
    for( mit = m_mapMonsterDrop.begin(); mit != m_mapMonsterDrop.end(); ++mit )
    {
        stm << L"  Dungeon,Monster key : " << mit->first.first << L", " << mit->first.second << std::endl
            << TOSTRINGW( mit->second.m_iExp )
            << TOSTRINGW( mit->second.m_iED )
            << TOSTRINGW( mit->second.m_fEDProperty );

        mit->second.m_kLottery.ToString( stm );
    }

    dbg::cout << stm.str();
}

void KDropTable::SetPVPFailRate_LUA( IN int iEnumKey, IN float fRate )
{
	if( iEnumKey >= 0 && iEnumKey < PF_END )
	{
		m_fPVPFailRate[iEnumKey] = fRate;
	}
	else
	{
		START_LOG( cerr, L"Failed to set PvP item-acquisition failure probability." )
			<< BUILD_LOG( iEnumKey )
			<< BUILD_LOG( fRate )
			<< END_LOG;
	}
}

void KDropTable::SetPVPItemChangeTime_LUA( float fTime )
{
	m_dfPVPItemChangeTime = static_cast<double>(fTime);
}

void KDropTable::AddPVPDropInfo_LUA( IN int iGroup, IN int iItemID, IN float fDropRate )
{
	std::map< int, KLottery >::iterator mit;
	mit = m_mapPVPDropItem.find( iGroup );
	if( mit == m_mapPVPDropItem.end() )
	{
		KLottery kLottery;
		kLottery.AddCaseIntegerCast( iItemID, fDropRate );
		m_mapPVPDropItem.insert( std::make_pair(iGroup,kLottery) );
	}
	else
	{
		mit->second.AddCaseIntegerCast( iItemID, fDropRate );
	}

	m_vecPVPDropItem.push_back( iItemID );
}

bool KDropTable::DecisionbyAllItem( OUT int& iGroup, OUT int& iItemID )
{
	iGroup = 0;
	iItemID = 0;

	if( m_mapPVPDropItem.size() <= 0 )
	{
		START_LOG( cerr, L"PvP drop-item data is missing!" )
			<< BUILD_LOG( m_mapPVPDropItem.size() )
			<< END_LOG;

		return false;
	}

	int iSelGroup = rand() % static_cast<int>(m_mapPVPDropItem.size());

	std::map< int, KLottery >::iterator mit;
	mit = m_mapPVPDropItem.begin();
	for( int i = 0; i < static_cast<int>(m_mapPVPDropItem.size()); ++i )
	{
		if( i == iSelGroup )
		{
			int iDecision = mit->second.Decision();

			if( iDecision == KLottery::CASE_BLANK )
			{
				START_LOG( cerr, L"Invalid PvP drop-item probability setting!" )
					<< END_LOG;
				return 0;
			}

			START_LOG( clog, L"PvP drop item" )
				<< BUILD_LOG( iDecision )
				<< END_LOG;

			// 정상 동작..
			iGroup = mit->first;//iSelGroup;
			iItemID = iDecision;

			return true;
		}

		++mit;
	}

	START_LOG( cerr, L"PvP drop item failed!" )
		<< END_LOG;

	return false;
}

bool KDropTable::DecisionbyGroupItem( IN int iGroup, OUT int& iItemID )
{
	iItemID = 0;

	std::map< int, KLottery >::iterator mit;
	mit = m_mapPVPDropItem.find( iGroup );

	if( mit == m_mapPVPDropItem.end() )
	{
		START_LOG( cerr, L"Invalid dropped-item group ID!" )
			<< BUILD_LOG( iGroup )
			<< END_LOG;

		return false;
	}

	int iDecision = mit->second.Decision();

	if( iDecision == KLottery::CASE_BLANK )
	{
		START_LOG( cerr, L"Invalid PvP drop-item probability setting!" )
			<< END_LOG;
		return false;
	}

	START_LOG( clog, L"PvP drop item" )
		<< BUILD_LOG( iDecision )
		<< END_LOG;

	iItemID = iDecision;

	return true;
}

float KDropTable::GetPVPDropFailRate( PVP_FAILRATE eKey )
{
	if( eKey < 0 ||
		eKey >= PF_END )
	{
		START_LOG( cerr, L"PvP drop item: invalid key for failure probability." )
			<< BUILD_LOG( eKey )
			<< END_LOG;
		return 0.f;
	}

	return m_fPVPFailRate[eKey];

}

bool KDropTable::CheckPVPItem( int _iItemID )
{
	for( const int& iItemID : m_vecPVPDropItem )
	{
		if( _iItemID == iItemID )
			return true;
	}

	return false;
}

//{{ 2009. 12. 18  최육사	던전사용아이템
bool KDropTable::AddAttribNpcSpecialDrop_LUA( IN char cDifficulty, IN int iItemID, IN float fDropRate )
{
	if( cDifficulty < CXSLDungeon::DL_NORMAL  ||
		cDifficulty > CXSLDungeon::DL_EXPERT  ||
		iItemID <= 0  ||
		fDropRate <= 0.0f )
	{
		START_LOG( cerr, L"Invalid attribute-monster special-drop information." )
			<< BUILD_LOGc( cDifficulty )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( fDropRate )
			<< END_LOG;

		return false;
	}

	if( SiCXSLItemManager()->GetItemTemplet( iItemID ) == NULL )
	{
		START_LOG( cerr, L"Attempted to register a nonexistent item in the NPC special-drop table." )
			<< BUILD_LOGc( cDifficulty )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( fDropRate )
			<< END_LOG;

		return false;
	}

	std::map< char, KLottery >::iterator mit = m_mapAttribNpcSPDrop.find( cDifficulty );
	if( mit == m_mapAttribNpcSPDrop.end() )
	{
        KLottery kLot;
		kLot.AddCase( iItemID, fDropRate );
		m_mapAttribNpcSPDrop.insert( std::make_pair( cDifficulty, kLot ) );
	}
	else
	{
        LIF( mit->second.AddCase( iItemID, fDropRate ) );
	}

	return true;
}

//{{ 필드 드롭 개편 - 김민성
bool KDropTable::AttribNpcSpecialDrop( IN char cDifficulty, IN float fUserContribution ,OUT std::vector< int >& vecDropItem )
{
	std::map< char, KLottery >::const_iterator mit = m_mapAttribNpcSPDrop.find( cDifficulty );
	if( mit == m_mapAttribNpcSPDrop.end() )
		return false;

	if( fUserContribution <= 0.0f )
		return true;

	KLottery kLott = mit->second;
	kLott.AddMultiProbRate( fUserContribution );

	const int iResultItemID = kLott.Decision();
	if( iResultItemID == KLottery::CASE_BLANK )
		return true;

	vecDropItem.push_back( iResultItemID );
	return true;
}
//}}

//}}


//{{ 2009. 12. 18  최육사	던전경험치개편

int KDropTable::GetNpcExp( IN char cNpcLv )
{
	std::map< int, int >::const_iterator mit = m_mapNpcExpTable.find( static_cast<int>(cNpcLv) );
	if( mit == m_mapNpcExpTable.end() )
	{
        START_LOG( cwarn, L"NPC level is not registered." )
			<< BUILD_LOGc( cNpcLv )
			<< END_LOG;

		return 0;
	}

	return mit->second;
}

//}}

void KDropTable::AddToGroup_LUA( int nGroupID, int nItemID, float fProbability )
{
    std::map<int,KLottery>::iterator mit;
    mit = m_mapDropGroup.find( nGroupID );

    if( mit == m_mapDropGroup.end() )  // 기존에 등록된 동일 그룹 데이터가 없음 - 새로 추가.
    {
        m_mapDropGroup.insert( std::make_pair( nGroupID, KLottery() ) );
        mit = m_mapDropGroup.find( nGroupID );
    }

    if( !mit->second.AddCaseIntegerCast( nItemID, fProbability ) )
	{
		START_LOG( cerr, L"Invalid drop-group probability!" )
			<< BUILD_LOG( nGroupID )
			<< BUILD_LOG( nItemID )
			<< BUILD_LOG( fProbability )
			<< BUILD_LOG( mit->second.GetTotalProb() )
			<< END_LOG;
	}
}

void KDropTable::AddMonsterDropInfo_LUA()
{
    KLuaManager luaMgr( GetLuaState() );

    KMonsterDropInfo kInfo;
    LUA_GET_VALUE( luaMgr, "DungeonID",     kInfo.m_nDungeonID, -1 );
    LUA_GET_VALUE( luaMgr, "MonsterID",     kInfo.m_nMonsterID, -1 );
    LUA_GET_VALUE( luaMgr, "Exp",           kInfo.m_iExp, -1 );
    LUA_GET_VALUE( luaMgr, "ED",            kInfo.m_iED, -1 );
    LUA_GET_VALUE( luaMgr, "EDProperty",    kInfo.m_fEDProperty, 0.0f );

	if( kInfo.m_nDungeonID == -1 && kInfo.m_nMonsterID == -1 && kInfo.m_nMonsterID == 0 )
    {
        START_LOG( cerr, L"Invalid key" )
            << BUILD_LOG( kInfo.m_nDungeonID )
            << BUILD_LOG( kInfo.m_nMonsterID );
        return;
    }

	//이벤트 테이블 중복 검사.
	if( kInfo.m_nDungeonID == 0 && kInfo.m_nMonsterID == 0 )
	{
		std::map<std::pair<int,int>,KMonsterDropInfo>::iterator mit;
		mit = m_mapMonsterDrop.find( std::make_pair( 0, 0 ) );

		if( mit != m_mapMonsterDrop.end() )
		{
			START_LOG( cerr, L"Duplicate event ID received." )
				<< END_LOG;

			return;
		}
	}

    // 단품 아이템 리스트와 아이템 그룹 리스트 로딩 처리가 매우 유사하게 되어있지만,
    // 스크립트에서의 magic number 난무 방지를 위해 이같이 처리하였다.

    if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
    {
        int nTableIndex = 1;
        while( luaMgr.BeginTable( nTableIndex ) == S_OK )
        {
            int nItemID         = -1;
            float fProperty     = 0.0f;

            LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
            LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

            if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

            luaMgr.EndTable();

            nTableIndex++;
        } // end while

        luaMgr.EndTable();
    }

    if( luaMgr.BeginTable( "DropGroupList" ) == S_OK ) // 아이템 그룹 리스트 확률 설정
    {
        int nTableIndex = 1;
        while( luaMgr.BeginTable( nTableIndex ) == S_OK )
        {
            int nGroupID    = -1;
            float fProperty = 0.0f;

            LUA_GET_VALUE( luaMgr, 1, nGroupID, -1 );
            LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( m_mapDropGroup.find( nGroupID ) == m_mapDropGroup.end() )
			{
				START_LOG( cerr, L"DropTable references a nonexistent DropGroupID." )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nGroupID )
					<< END_LOG;
			}
			//}}

            if( kInfo.m_kLottery.AddCase( nGroupID, fProperty, DCP_ITEM_GROUP ) == false )
			{
				START_LOG( cerr, L"Drop Group List" )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

            luaMgr.EndTable();

            nTableIndex++;
        } // end while

        luaMgr.EndTable();
    }

    // operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
    m_mapMonsterDrop[ std::make_pair( kInfo.m_nDungeonID, kInfo.m_nMonsterID ) ] = kInfo;
}

//{{ 2009. 7. 25  최육사	헤니르시공
void KDropTable::AddHenirMonsterDropInfo_LUA()
{
	KLuaManager luaMgr( GetLuaState() );

	KHenirNpcDropInfo kInfo;
	LUA_GET_VALUE( luaMgr, "HenirDungeonMode",	kInfo.m_iHenirDungeonMode, -1 );
	LUA_GET_VALUE( luaMgr, "MonsterID",			kInfo.m_nMonsterID, -1 );
	LUA_GET_VALUE( luaMgr, "Exp",				kInfo.m_iExp, -1 );
	LUA_GET_VALUE( luaMgr, "ED",				kInfo.m_iED, -1 );
	LUA_GET_VALUE( luaMgr, "EDProperty",		kInfo.m_fEDProperty, 0.0f );

	if( kInfo.m_iHenirDungeonMode == -1 && kInfo.m_nMonsterID == -1 && kInfo.m_nMonsterID == 0 )
	{
		START_LOG( cerr, L"Invalid key" )
			<< BUILD_LOG( kInfo.m_iHenirDungeonMode )
			<< BUILD_LOG( kInfo.m_nMonsterID );
		return;
	}

	//이벤트 테이블 중복 검사.
	if( kInfo.m_iHenirDungeonMode == 0 && kInfo.m_nMonsterID == 0 )
	{
		std::map< std::pair< int, int >, KHenirNpcDropInfo >::const_iterator mit;
		mit = m_mapHenirNpcDrop.find( std::make_pair( 0, 0 ) );

		if( mit != m_mapHenirNpcDrop.end() )
		{
			START_LOG( cerr, L"Duplicate event ID received." )
				<< END_LOG;

			return;
		}
	}

	// 단품 아이템 리스트와 아이템 그룹 리스트 로딩 처리가 매우 유사하게 되어있지만,
	// 스크립트에서의 magic number 난무 방지를 위해 이같이 처리하였다.

	if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nItemID         = -1;
			float fProperty     = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_iHenirDungeonMode )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_iHenirDungeonMode )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	if( luaMgr.BeginTable( "DropGroupList" ) == S_OK ) // 아이템 그룹 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nGroupID    = -1;
			float fProperty = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nGroupID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( m_mapDropGroup.find( nGroupID ) == m_mapDropGroup.end() )
			{
				START_LOG( cerr, L"DropTable references a nonexistent DropGroupID." )
					<< BUILD_LOG( kInfo.m_iHenirDungeonMode )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nGroupID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nGroupID, fProperty, DCP_ITEM_GROUP ) == false )
			{
				START_LOG( cerr, L"Drop Group List" )
					<< BUILD_LOG( kInfo.m_iHenirDungeonMode )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	// operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
	m_mapHenirNpcDrop[ std::make_pair( kInfo.m_iHenirDungeonMode, kInfo.m_nMonsterID ) ] = kInfo;
}
//}}

//{{ 2010. 05. 02  최육사	비밀던전 헬모드

void KDropTable::AddExtraStageMonsterDropInfo_LUA()
{
	KLuaManager luaMgr( GetLuaState() );

	KExtraStageNpcDropInfo kInfo;
	LUA_GET_VALUE( luaMgr, "MonsterID",			kInfo.m_nMonsterID, -1 );
	LUA_GET_VALUE( luaMgr, "Exp",				kInfo.m_iExp, -1 );
	LUA_GET_VALUE( luaMgr, "ED",				kInfo.m_iED, -1 );
	LUA_GET_VALUE( luaMgr, "EDProperty",		kInfo.m_fEDProperty, 0.0f );

	if( kInfo.m_nMonsterID == -1 || kInfo.m_nMonsterID == 0 )
	{
		START_LOG( cerr, L"Invalid key" )
			<< BUILD_LOG( kInfo.m_nMonsterID );
		return;
	}

	//이벤트 테이블 중복 검사.
	std::map< int, KExtraStageNpcDropInfo >::const_iterator mit;
	mit = m_mapExtraStageNpcDrop.find( kInfo.m_nMonsterID );
	if( mit != m_mapExtraStageNpcDrop.end() )
	{
		START_LOG( cerr, L"Duplicate monster ID received." )
			<< BUILD_LOG( kInfo.m_nMonsterID )
			<< END_LOG;
		return;
	}

	// 단품 아이템 리스트와 아이템 그룹 리스트 로딩 처리가 매우 유사하게 되어있지만,
	// 스크립트에서의 magic number 난무 방지를 위해 이같이 처리하였다.

	if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nItemID         = -1;
			float fProperty     = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	if( luaMgr.BeginTable( "DropGroupList" ) == S_OK ) // 아이템 그룹 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nGroupID    = -1;
			float fProperty = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nGroupID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( m_mapDropGroup.find( nGroupID ) == m_mapDropGroup.end() )
			{
				START_LOG( cerr, L"DropTable references a nonexistent DropGroupID." )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nGroupID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nGroupID, fProperty, DCP_ITEM_GROUP ) == false )
			{
				START_LOG( cerr, L"Drop Group List" )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	// operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
	m_mapExtraStageNpcDrop[ kInfo.m_nMonsterID ] = kInfo;
}

//}}

//{{ 2009. 4. 21  최육사	전체드롭테이블
void KDropTable::AddStaticDropInfo_LUA()
{
	KLuaManager luaMgr( GetLuaState() );

	bool bEnable = true;
	KStaticDropInfo kInfo;
	LUA_GET_VALUE( luaMgr, "DungeonID",     kInfo.m_nDungeonID, -1 );
	LUA_GET_VALUE( luaMgr, "Enable",	    bEnable,			true );

	if( !bEnable )
		return;

	if( kInfo.m_nDungeonID == -1 )
	{
		START_LOG( cerr, L"Invalid key" )
			<< BUILD_LOG( kInfo.m_nDungeonID )
			<< END_LOG;
		return;
	}

	// DungeonID 중복 검사.
	std::map< int, KStaticDropInfo >::const_iterator mit;
	mit = m_mapStaticDrop.find( kInfo.m_nDungeonID );
	if( mit != m_mapStaticDrop.end() )
	{
		START_LOG( cerr, L"Duplicate dungeon ID received." )
			<< BUILD_LOG( kInfo.m_nDungeonID )
			<< END_LOG;
		return;
	}

	// 드롭테이블
	if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nItemID         = -1;
			float fProperty     = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_nDungeonID )
					<< END_LOG;
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	// operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
	m_mapStaticDrop.insert( std::make_pair( kInfo.m_nDungeonID, kInfo ) );
}
//}}

//{{ 2011. 11. 23	최육사	배틀필드
void KDropTable::AddBattleFieldNpcDropInfo_LUA()
{
	KLuaManager luaMgr( GetLuaState() );

	KBattleFieldNpcDropInfo kInfo;
	LUA_GET_VALUE( luaMgr, "BattleFieldID", kInfo.m_nBattleFieldID, -1 );
	LUA_GET_VALUE( luaMgr, "MonsterID",     kInfo.m_nMonsterID, -1 );
	LUA_GET_VALUE( luaMgr, "Exp",           kInfo.m_iExp, -1 );
	LUA_GET_VALUE( luaMgr, "ED",            kInfo.m_iED, -1 );
	LUA_GET_VALUE( luaMgr, "EDProperty",    kInfo.m_fEDProperty, 0.0f );

	if( kInfo.m_nBattleFieldID == -1 && kInfo.m_nMonsterID == -1 && kInfo.m_nMonsterID == 0 )
	{
		START_LOG( cerr, L"Invalid key" )
			<< BUILD_LOG( kInfo.m_nBattleFieldID )
			<< BUILD_LOG( kInfo.m_nMonsterID );
		return;
	}

	//이벤트 테이블 중복 검사.
	if( kInfo.m_nBattleFieldID == 0 && kInfo.m_nMonsterID == 0 )
	{
		std::map<std::pair<int,int>,KBattleFieldNpcDropInfo>::iterator mit;
		mit = m_mapBattleFieldNpcDrop.find( std::make_pair( 0, 0 ) );

		if( mit != m_mapBattleFieldNpcDrop.end() )
		{
			START_LOG( cerr, L"Duplicate event ID received." )
				<< END_LOG;

			return;
		}
	}

	// 단품 아이템 리스트와 아이템 그룹 리스트 로딩 처리가 매우 유사하게 되어있지만,
	// 스크립트에서의 magic number 난무 방지를 위해 이같이 처리하였다.

	if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nItemID         = -1;
			float fProperty     = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	if( luaMgr.BeginTable( "DropGroupList" ) == S_OK ) // 아이템 그룹 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nGroupID    = -1;
			float fProperty = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nGroupID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( m_mapDropGroup.find( nGroupID ) == m_mapDropGroup.end() )
			{
				START_LOG( cerr, L"DropTable references a nonexistent DropGroupID." )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< BUILD_LOG( kInfo.m_nMonsterID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nGroupID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nGroupID, fProperty, DCP_ITEM_GROUP ) == false )
			{
				START_LOG( cerr, L"Drop Group List" )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< BUILD_LOG( kInfo.m_nMonsterID );
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	// operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
	m_mapBattleFieldNpcDrop[ std::make_pair( kInfo.m_nBattleFieldID, kInfo.m_nMonsterID ) ] = kInfo;
}

void KDropTable::AddBattleFieldStaticDropInfo_LUA()
{
	KLuaManager luaMgr( GetLuaState() );

	bool bEnable = true;
	KBattleFieldStaticDropInfo kInfo;
	LUA_GET_VALUE( luaMgr, "BattleFieldID",     kInfo.m_nBattleFieldID, -1 );
	LUA_GET_VALUE( luaMgr, "Enable",	    bEnable,			true );

	if( !bEnable )
		return;

	if( kInfo.m_nBattleFieldID == -1 )
	{
		START_LOG( cerr, L"Invalid key" )
			<< BUILD_LOG( kInfo.m_nBattleFieldID )
			<< END_LOG;
		return;
	}

	// DungeonID 중복 검사.
	std::map< int, KBattleFieldStaticDropInfo >::const_iterator mit;
	mit = m_mapBattleFieldStaticDrop.find( kInfo.m_nBattleFieldID );
	if( mit != m_mapBattleFieldStaticDrop.end() )
	{
		START_LOG( cerr, L"Duplicate dungeon ID received." )
			<< BUILD_LOG( kInfo.m_nBattleFieldID )
			<< END_LOG;
		return;
	}

	// 드롭테이블
	if( luaMgr.BeginTable( "DropItemList" ) == S_OK )   // 단품 아이템 리스트 확률 설정
	{
		int nTableIndex = 1;
		while( luaMgr.BeginTable( nTableIndex ) == S_OK )
		{
			int nItemID         = -1;
			float fProperty     = 0.0f;

			LUA_GET_VALUE( luaMgr, 1, nItemID, -1 );
			LUA_GET_VALUE( luaMgr, 2, fProperty, 0.0f );

			//{{ 2008. 10. 24  최육사	드롭 그룹 체크
			if( SiCXSLItemManager()->GetItemTemplet( nItemID ) == NULL )
			{
				START_LOG( cerr, L"DropTable references a nonexistent ItemID." )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< BUILD_LOG( nTableIndex )
					<< BUILD_LOG( nItemID )
					<< END_LOG;
			}
			//}}

			if( kInfo.m_kLottery.AddCase( nItemID, fProperty, DCP_UNARY_ITEM ) == false )
			{
				START_LOG( cerr, L"Drop Item List" )
					<< BUILD_LOG( kInfo.m_nBattleFieldID )
					<< END_LOG;
			}

			luaMgr.EndTable();

			nTableIndex++;
		} // end while

		luaMgr.EndTable();
	}

	// operator[]의 의도적인 사용 - 기존에 있던 동일 데이터를 덮어써 버린다.
	m_mapBattleFieldStaticDrop.insert( std::make_pair( kInfo.m_nBattleFieldID, kInfo ) );
}
//}}

//{{ 2009. 6. 29  최육사	드롭공지 아이템
bool KDropTable::AddNotifyDropItemInfo_LUA( int iItemID )
{
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Attempted to register a nonexistent item as a drop-acquisition notice item!" )
			<< BUILD_LOG( iItemID )
			<< END_LOG;

		return false;
	}

	m_setNotifyDropItem.insert( iItemID );
	return true;
}
//}}

//{{ 2009. 12. 18  최육사	던전경험치개편
bool KDropTable::AddNpcExpTable_LUA( int iNpcLv, int iEXP )
{
	if( iNpcLv <= 0  ||  iEXP <= 0 )
	{
		START_LOG( cerr, L"Invalid NPC experience settings." )
			<< BUILD_LOG( iNpcLv )
			<< BUILD_LOG( iEXP )
			<< END_LOG;

		return false;
	}

	m_mapNpcExpTable.insert( std::make_pair( iNpcLv, iEXP ) );
	return true;
}
//}}

//{{ 2011. 03. 28	최육사	서버 상태 체크
void KDropTable::SetManyEDDropNpcEvent_LUA( bool bVal )
{
    m_bIsDropManyEDEvent = bVal;

	START_LOG( cout, L"Configuring the high-ED-drop NPC event: " )
		<< BUILD_LOG( m_bIsDropManyEDEvent );
}

bool KDropTable::AddManyEDDropNpc_LUA( int iNpcID, float fMultiplyDropED, float fMultiplyDropEDCount )
{
	if( iNpcID <= 0  ||  fMultiplyDropED <= 0  ||  fMultiplyDropEDCount <= 0 )
	{
		START_LOG( cerr, L"Invalid many-ED-drop settings." )
			<< BUILD_LOG( iNpcID )
			<< BUILD_LOG( fMultiplyDropED )
			<< BUILD_LOG( fMultiplyDropEDCount )
			<< END_LOG;

		return false;
	}

    KManyEDDropNpcInfo kInfo;
	kInfo.m_fMultiplyDropED = fMultiplyDropED;
	kInfo.m_fMultiplyDropEDCount = fMultiplyDropEDCount;
	m_mapManyEDDropTable.insert( std::make_pair( iNpcID, kInfo ) );
	return true;
}
//}}

int KDropTable::GetEDItemID( int nED ) const
{
    // 추후 이 함수만 동적 수정이 용이하도록 script 화 한다.

	int iEDItemID = 0;

	if( 0 < nED )	{	iEDItemID = CXSLItem::EDI_BRONZE_ED;	}
	if( 50 < nED )	{	iEDItemID = CXSLItem::EDI_SILVER_ED;	}
	if( 150 < nED )	{	iEDItemID = CXSLItem::EDI_GOLD_ED;	}
	if( 1999 < nED )	{	iEDItemID = CXSLItem::EDI_GOLD_BAR;	}

	return iEDItemID;
}

//{{ 필드 드롭 개편 - 김민성
bool KDropTable::NpcDropItem( IN const int iNpcID,
							 IN const int iDropEXP,
							 IN const float fEDProperty,
							 IN const int iDropED,
							 IN const KLottery& kLot,
							 IN const float fLevelFactor,
							 IN const float fDungeonFactor,
							 IN const float fContribution,
							 IN const int iPartyUserCount_,
							 IN const float /*fPartyDropBonus*/,
							 //{{ 2012. 12. 16  드롭 이벤트 - 김민성
							 IN const float fDropRate,
							 //}}
							 OUT DROP_DATA& sDropData,
							 IN const bool bIsBoss )
{
	//////////////////////////////////////////////////////////////////////////
	// X = 유저당 준데미지 / 몬스터HP (X 값은 최고 1.5를 넘지 않는다)
	// Y = NPC가 원래 주는 EXP
	// Z = NPC가 원래 주는 ED
	// B = 파티 보너스 = NPC설정 경험치 / ‘현재 지역에 있는’ 파티원 수 * ‘현재 지역에 있는’ 파티원 수에 따른 파티원 상수 * ‘매우어려움 고정’ 난이도 상수
	// A = 필드 상수 = SiCXSLBattleFieldManager()->GetBattleFieldFactor()
	// F = 레벨 보정 = 몬스터와의 레벨차가 +-10레벨 이상이면, 보상을 획득할 수 없다
	// T = 드롭 테이블 확률
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// EXP, ED

	// EXP = Y * X * A * F
	// ED =  Z * X * A * F
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 드롭 아이템

	// 아이템 드롭(획득) 확률 = T * X * A * ( B * 0.5 ) * F
	//////////////////////////////////////////////////////////////////////////

	sDropData.Clear();

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 EXP
	sDropData.m_iExp = (int)(iDropEXP * fContribution * fDungeonFactor * fLevelFactor);	// fDungeonFactor -> 몬스터상수(일반,엘리트....보스)

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 ED
	//{{ 2012. 12. 16  드롭 이벤트 - 김민성
	int iGetTotalED = static_cast<int>( iDropED * fContribution * fDungeonFactor * fLevelFactor * fDropRate );
	//}}

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 ED
	if( ( (float)rand() / static_cast<float>( RAND_MAX ) ) < fEDProperty )
	{
		if( true == bIsBoss )
		{
			sDropData.m_nGPNum		= ( rand() % 11 + 10 ); // [10,20]
			sDropData.m_iED			= ( sDropData.m_nGPNum > 0 ? iGetTotalED / sDropData.m_nGPNum : 0 );
			sDropData.m_iGPItemID	= GetEDItemID( sDropData.m_iED );
		}
		else
		{
			sDropData.m_nGPNum      = rand() % 2 + 2;    // [2,3]
			sDropData.m_iED			= ( sDropData.m_nGPNum > 0 ? iGetTotalED / sDropData.m_nGPNum : 0 );
			sDropData.m_iGPItemID   = GetEDItemID( sDropData.m_iED );
		}

		KManyEDDropNpcInfo kManyDropInfo;
		if( GetManyEDDropNpcInfo( iNpcID, kManyDropInfo ) == true )
		{
			sDropData.m_nGPNum		= static_cast<int>(static_cast<float>(sDropData.m_nGPNum) * kManyDropInfo.m_fMultiplyDropEDCount);
			sDropData.m_iED			= static_cast<int>(static_cast<float>(sDropData.m_iED) * kManyDropInfo.m_fMultiplyDropED);
			sDropData.m_iGPItemID	= GetEDItemID( sDropData.m_iED );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 다음은 아이템.
	// 	T = 드롭테이블 확률
	// 	X = 해당유저가 준데미지 / 몬스터HP (X 값은 최고 1.5를 넘지 않는다)
	// 	A = 필드 상수
	// 	C = ‘현재 지역에 있는’ 파티원 수에 따른 파티원 상수 (0, 0.5, 1, 1.5)
	// 	F = 레벨보정상수
	// 	아이템 드롭(획득) 확률 = T * X * A * ( 1 + C ) * F

	if( fContribution <= 0.0f )
		return true;

	// 아이템 드롭은 레벨 차에 상관 없이 드롭하기로 변경
	//if( fLevelFactor <= 0.0f )
	//	return true;

	KLottery kDropInfoCopy = kLot;
	kDropInfoCopy.AddMultiProbRate( fContribution * iPartyUserCount_ );
//	kDropInfoCopy.AddMultiProbRate( fPartyDropBonus );

	//{{ 2012. 12. 16  드롭 이벤트 - 김민성

	{
		float fDropRateCopy = fDropRate;

		while( 0.0f < fDropRateCopy )
		{
			if( fDropRateCopy < 1.0f )
			{
				kDropInfoCopy.AddMultiProbRate( fDropRateCopy );
				fDropRateCopy = 0.0f;
			}
			else
			{
				fDropRateCopy -= 1.0f;
			}

			const int nCaseID = kDropInfoCopy.Decision();
			if( nCaseID == KLottery::CASE_BLANK )   // 획득한 아이템 없음.
				continue;

			const int nCaseParam = kDropInfoCopy.GetParam1( nCaseID );

			switch( nCaseParam )    // 단품 아이템, 아이템 그룹, 혹은 예상 못한 에러.
			{
			case DCP_UNARY_ITEM:    // 단품 아이템
				{
					sDropData.m_vecItemID.push_back( nCaseID );
				}
				break;
			case DCP_ITEM_GROUP:    // 아이템 그룹.
				{
					std::map< int, KLottery >::const_iterator mit;
					mit = m_mapDropGroup.find( nCaseID );
					if( mit == m_mapDropGroup.end() )
					{
						START_LOG( cerr, L"Invalid item-group information." )
							<< BUILD_LOG( nCaseID )
							<< BUILD_LOG( nCaseParam )
							<< BUILD_LOG( m_mapDropGroup.size() );
						continue;
					}

					// 아이템 그룹 중에서 실제 아이템 하나 선택.
					const int iResultItemID = mit->second.Decision();
					if( iResultItemID == KLottery::CASE_BLANK )   // 획득한 아이템 없음.
						continue;

					sDropData.m_vecItemID.push_back( iResultItemID );
				}
				break;
			default:
				{
					START_LOG( cerr, L"Never Get Here ~" )
						<< BUILD_LOG( nCaseParam )
						<< BUILD_LOG( nCaseID )
						<< BUILD_LOG( kDropInfoCopy.GetCaseNum() )
						<< BUILD_LOG( kDropInfoCopy.GetTotalProb() )
						<< END_LOG;
					continue;
				}
			}
		}
	}

	//}}

	return true;
}

//}}

//{{ 필드 드롭 개편 - 김민성
bool KDropTable::NormalNpcDropItem( IN const int nDungeonID,
								    IN const int iNpcID,
									IN const char cNpcLevel,
								    IN const float fLevelFactor,
								    IN const float fDungeonFactor,
								    IN const float fContribution,
								    IN const float fPartyDropBonus,
									IN const int iPartyUserCount,
									//{{ 2012. 12. 16  드롭 이벤트 - 김민성
									IN const float fDropRate,
									//}}
									OUT bool& bDecreaseEndurance,
								    OUT DROP_DATA& sDropData,
								    IN const bool bIsBoss )
{
	sDropData.Clear();

	// 우선 해당 DungeonID + MonsterID에 해당하는 드롭 정보을 검색한다.
	std::map< std::pair< int, int >, KMonsterDropInfo >::const_iterator mit;
	mit = m_mapMonsterDrop.find( std::make_pair( nDungeonID, iNpcID ) );

	// 해당 경우에 대한 설정이 존재하지 않음.
	if( mit == m_mapMonsterDrop.end() )
	{
		// 던전 아이디가 정해져 있지 않은 설정이 존재하는지 다시 한번 확인.
		mit = m_mapMonsterDrop.find( std::make_pair( 0, iNpcID ) );

		// 그것마저 없다면?
		if( mit == m_mapMonsterDrop.end() )
		{
			// 성공적인 종료. 에러일때만 false.
			return true;
		}
	}

	KMonsterDropInfo kDropInfo = mit->second;

	// 드롭 테이블에 경험치 정보가 세팅된 몬스터만 경험치를 준다.
	if( kDropInfo.m_iExp > 0 )
	{
		kDropInfo.m_iExp = GetNpcExp( cNpcLevel );

		// 드롭 테이블에 경험치 정보가 세팅된 몬스터는 유저의 내구도를 감소 시킨다!
		bDecreaseEndurance = true;
	}

	if( kDropInfo.m_iED > 0 )
	{
		kDropInfo.m_iED = (10 + ( kDropInfo.m_iED * cNpcLevel ));
	}

	// 보정 던전 이라면 fLevelFactor 를 1.0f 로 고정된다.
	float fCheckLevelFactor = fLevelFactor;
	if( CXSLDungeon::IsEventDungeon( nDungeonID ) == true )
	{
		fCheckLevelFactor = 1.0f;
	}

	// NPC 드롭 처리
	//{{ 2012. 12. 16  드롭 이벤트 - 김민성

	return NpcDropItem( iNpcID, kDropInfo.m_iExp, kDropInfo.m_fEDProperty, kDropInfo.m_iED, kDropInfo.m_kLottery, fCheckLevelFactor, fDungeonFactor, fContribution, iPartyUserCount, fPartyDropBonus, fDropRate, sDropData, bIsBoss );

	//}}
}

bool KDropTable::HenirNpcDropItem( IN const CXSLDungeon::DUNGEON_MODE eDungeonMode,
							 	   IN const int iNpcID,
								   IN const char cNpcLevel,
								   IN const float fDungeonFactor,
								   IN const float fContribution,
								   IN const float fPartyDropBonus,
								   IN const int iPartyUserCount,
								   //{{ 2012. 12. 16  드롭 이벤트 - 김민성
								   IN const float fDropRate,
								   //}}
								   OUT bool& bDecreaseEndurance,
								   OUT DROP_DATA& sDropData,
								   IN const bool bIsBoss )
{
	sDropData.Clear();

	// 우선 해당 DungeonMode와 NpcID에 해당하는 드롭 정보를 검색한다.
	std::map< std::pair< int, int >, KHenirNpcDropInfo >::const_iterator mit;
	mit = m_mapHenirNpcDrop.find( std::make_pair( static_cast<int>( eDungeonMode ), iNpcID ) );

	// 해당 경우에 대한 설정이 존재하지 않음.
	if( mit == m_mapHenirNpcDrop.end() )
	{
		// 던전 아이디가 정해져 있지 않은 설정이 존재하는지 다시 한번 확인.
		mit = m_mapHenirNpcDrop.find( std::make_pair( 0, iNpcID ) );

		// 그것마저 없다면?
		if( mit == m_mapHenirNpcDrop.end() )
		{
			// 성공적인 종료. 에러일때만 false.
			return true;
		}
	}

	KHenirNpcDropInfo kDropInfo = mit->second;

	// 경험치 세팅된 몬스터만 경험치를 주자!
	if( kDropInfo.m_iExp > 0 )
	{
		kDropInfo.m_iExp = GetNpcExp( cNpcLevel );

		// 경험치가 세팅된 몬스터는 내구도를 깎자!
		bDecreaseEndurance = true;
	}

	if( kDropInfo.m_iED > 0 )
	{
		kDropInfo.m_iED = (10 + ( kDropInfo.m_iED * cNpcLevel ));
	}

	// NPC 드롭 처리
	// 헤니르는 무조건 LevelFactor 는 1.0f 로 한다.
	//{{ 2012. 12. 16  드롭 이벤트 - 김민성
	return NpcDropItem( iNpcID, kDropInfo.m_iExp, kDropInfo.m_fEDProperty, kDropInfo.m_iED, kDropInfo.m_kLottery, 1.0f, fDungeonFactor, fContribution, iPartyUserCount, fPartyDropBonus, fDropRate, sDropData, bIsBoss );
	//}}
}

bool KDropTable::ExtraStageNpcDropItem( IN const int iNpcID,
									    IN const char cNpcLevel,
									    IN const float fLevelFactor,
									    IN const float fDungeonFactor,
									    IN const float fContribution,
									    IN const float fPartyDropBonus,
										IN const int iPartyUserCount,
										//{{ 2012. 12. 16  드롭 이벤트 - 김민성
										IN const float fDropRate,
										//}}
										OUT bool& bDecreaseEndurance,
									    OUT DROP_DATA& sDropData,
									    IN const bool bIsBoss /*= false*/ )
{
	sDropData.Clear();

	// 우선 해당 MonsterID와 NpcLevel에 해당하는 드롭 정보를 검색한다.
	std::map< int, KExtraStageNpcDropInfo >::const_iterator mit;
	mit = m_mapExtraStageNpcDrop.find( iNpcID );
	if( mit == m_mapExtraStageNpcDrop.end() )
	{
		// 성공적인 종료. 에러일때만 false.
		return true;
	}

	KExtraStageNpcDropInfo kDropInfo = mit->second;

	// 경험치 세팅된 몬스터만 경험치를 주자!
	if( kDropInfo.m_iExp > 0 )
	{
		kDropInfo.m_iExp = GetNpcExp( cNpcLevel );

		// 경험치가 세팅된 몬스터는 내구도를 깎자!
		bDecreaseEndurance = true;
	}

	if( kDropInfo.m_iED > 0 )
	{
		kDropInfo.m_iED = (10 + ( kDropInfo.m_iED * cNpcLevel ));
	}

	//{{ 2012. 12. 16  드롭 이벤트 - 김민성

	return NpcDropItem( iNpcID, kDropInfo.m_iExp, kDropInfo.m_fEDProperty, kDropInfo.m_iED, kDropInfo.m_kLottery, fLevelFactor, fDungeonFactor, fContribution, iPartyUserCount, fPartyDropBonus, fDropRate, sDropData, bIsBoss );

	//}}
}

//}}

//{{ 2011. 11. 22	최육사	배틀필드 시스템
//#ifdef SERV_BATTLE_FIELD_SYSTEM
//bool KDropTable::BattleFieldNpcDropItem( IN const CXSLBattleField::BATTLE_FIELD_ID eBattleFieldID, IN const int iNpcID ) const
//{
//    return true;
//}
//#endif SERV_BATTLE_FIELD_SYSTEM
//}}

//{{ 2012. 11. 12	최육사	배틀필드 시스템
void KDropTable::NpcSpecialDropItem( IN const int iNpcID, IN const char cMonsterGrade, IN const char cNpcDieState, IN OUT std::vector< int >& vecItemID )
{
	switch( iNpcID )
	{
	case CXSLUnitManager::NUI_MANA_EATER:
	case CXSLUnitManager::NUI_HALLOWEEN_MANA_EATER:
	case CXSLUnitManager::NUI_DEFENCE_MANA_EATER:
	case CXSLUnitManager::NUI_MANA_EATER_FD:
		{
			if( cNpcDieState == KEGS_NPC_UNIT_DIE_REQ::NDS_BONUS_DROP )
			{
				// 드롭 테이블을 확장해서 'Special Drop'기능으로 대체할예정
				vecItemID.push_back( 70055 ); // 70055
			}
		}
		break;

	case CXSLUnitManager::NUI_BLOOD_EATER:
	case CXSLUnitManager::NUI_DEFENCE_BLOOD_EATER:
	case CXSLUnitManager::NUI_BLOOD_EATER_FD:
		{
			if( cNpcDieState == KEGS_NPC_UNIT_DIE_REQ::NDS_BONUS_DROP )
			{
				// 드롭 테이블을 확장해서 'Special Drop'기능으로 대체할예정
				vecItemID.push_back( 70056 ); // 70055
			}
		}
		break;
	}
	//}}

	//{{ 보스, 중보 잡을 때 아이템 드롭 되는 기능(MonsterGrade 기준) - 김민성
	switch( cMonsterGrade )
	{
	case CXSLUnitManager::MG_MIDDLE_BOSS_NPC:
		{
			// 드롭 테이블을 확장해서 'Special Drop'기능으로 대체할예정
			if( rand() % 100 < 50 )	// 중보스는 50% 확률로만 드랍
			{
				vecItemID.push_back( 70057 ); // 70057
			}
		}
		break;

	case CXSLUnitManager::MG_BOSS_NPC:
		{
			// 드롭 테이블을 확장해서 'Special Drop'기능으로 대체할예정
			vecItemID.push_back( 70058 ); // 70057
		}
		break;

	case CXSLUnitManager::MG_DEFINITELY_DROP_HP_BALL:
		vecItemID.push_back( 70057 ); // 70057
		break;
	}
	//}
}
//}}

//{{ 2009. 5. 13  최육사	서브스테이지별 경험치 주는 npc수
bool KDropTable::IsExpDropNpc( IN int nDungeonID, IN int nMonsterID )
{
	std::map< std::pair< int, int >, KMonsterDropInfo >::const_iterator mit;
	mit = m_mapMonsterDrop.find( std::make_pair( nDungeonID, nMonsterID ) );
	if( mit == m_mapMonsterDrop.end() )
	{
		mit = m_mapMonsterDrop.find( std::make_pair( 0, nMonsterID ) );

		if( mit == m_mapMonsterDrop.end() )
			return false;
	}

	return ( mit->second.m_iExp > 0 );
}
//}}

//{{ 2010. 01. 05  최육사	내구도개편
bool KDropTable::IsHenirExpDropNpc( IN CXSLDungeon::DUNGEON_MODE eDungeonMode, IN int nMonsterID )
{
	std::map< std::pair< int, int >, KHenirNpcDropInfo >::const_iterator mit;
	mit = m_mapHenirNpcDrop.find( std::make_pair( static_cast<int>( eDungeonMode ), nMonsterID ) );
	if( mit == m_mapHenirNpcDrop.end() )
	{
		mit = m_mapHenirNpcDrop.find( std::make_pair( 0, nMonsterID ) );

		if( mit == m_mapHenirNpcDrop.end() )
			return false;
	}

	return ( mit->second.m_iExp > 0 );
}
//}}

//{{ 2009. 4. 22  최육사	전체드롭
//{{ 필드 드롭 개편 - 김민성
bool KDropTable::StaticDropItem( IN int nDungeonID, OUT DROP_DATA& sDropData, IN float fUserContribution )
{
	sDropData.m_vecItemID.clear();
	sDropData.m_iExp        = -1;
	sDropData.m_iGPItemID   = -1;
	sDropData.m_iED         = -1;
	sDropData.m_nGPNum      = -1;

	std::map< int, KStaticDropInfo >::const_iterator mit;
	mit = m_mapStaticDrop.find( nDungeonID );
	if( mit == m_mapStaticDrop.end() )
	{
		START_LOG( clog, L"No global-drop settings exist for this dungeon." )
			<< BUILD_LOG( nDungeonID );
		return true;
	}

	if( fUserContribution <= 0.0f )
		return true;

	KLottery kLott = mit->second.m_kLottery;
	kLott.AddMultiProbRate( fUserContribution );

	int iResultItemID = kLott.Decision();
	if( iResultItemID != KLottery::CASE_BLANK )
	{
		// 아이템 드롭!
		sDropData.m_vecItemID.push_back( iResultItemID );
	}

	return true;
}

bool KDropTable::EventDropItem( IN float fUserContribution, OUT DROP_DATA& sDropData )
{
	return StaticDropItem( 0, sDropData, fUserContribution );
}

//}}
//}}

bool KDropTable::DropGroupItem( IN int iDropGroupID, OUT int& iDropItemID )
{
	std::map< int, KLottery >::const_iterator mit;
	mit = m_mapDropGroup.find( iDropGroupID );
	if( mit == m_mapDropGroup.end() )
		return false;

	iDropItemID = mit->second.Decision();
	return true;
}

//{{ 2009. 6. 17  최육사	대전드롭
bool KDropTable::PvpDropItem( IN int /*iItemDropNum*/, OUT DROP_DATA& sDropData )
{
	sDropData.m_vecItemID.clear();
	sDropData.m_iExp        = -1;
	sDropData.m_iGPItemID   = -1;
	sDropData.m_iED         = -1;
	sDropData.m_nGPNum      = -1;

	int iResultItemID = m_kPvpDrop.Decision();
	if( iResultItemID != KLottery::CASE_BLANK )
	{
		sDropData.m_vecItemID.push_back( iResultItemID );
	}
	return true;
}
//}}

//{{ 2009. 5. 23  최육사	시간 드롭
void KDropTable::SetTimeDropInfo( IN const std::vector< KTimeDropInfo >& vecTimeDropInfo, IN const std::wstring& wstrServerIP )
{
	CTime tCurTime = CTime::GetCurrentTime();

	for( const KTimeDropInfo& kTimeDropInfo : vecTimeDropInfo )
	{
		CTime tTimeDrop;
		std::wstring wstrDropTime = kTimeDropInfo.m_wstrDropTime;
		if( KncUtil::ConvertStringToCTime( wstrDropTime, tTimeDrop ) == false )
		{
			START_LOG( cerr, L"Failed to parse drop time!" )
				<< BUILD_LOG( kTimeDropInfo.m_iTimeDropID )
				<< BUILD_LOG( kTimeDropInfo.m_wstrDropTime )
				<< END_LOG;

			continue;
		}

		// 정상적인 아이템인지 검사
		const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( kTimeDropInfo.m_iDropItemID );
		if( pItemTemplet == NULL )
		{
			START_LOG( cerr, L"A nonexistent ItemID is registered." )
				<< BUILD_LOG( kTimeDropInfo.m_iTimeDropID )
				<< BUILD_LOG( kTimeDropInfo.m_iDropItemID )
				<< END_LOG;

			continue;
		}

		// 지났으면 업데이트 안함
        if( tTimeDrop <= tCurTime )
		{
			START_LOG( clog, L"Expired event." )
				<< BUILD_LOG( kTimeDropInfo.m_iTimeDropID )
				<< BUILD_LOG( kTimeDropInfo.m_wstrDropTime );

			continue;
		}

		// 해당 서버인지 검사
		if( kTimeDropInfo.m_wstrServerIP != wstrServerIP )
			continue;

        KTimeDropItemInfo kInfo;
		kInfo.m_iTimeDropID	 = kTimeDropInfo.m_iTimeDropID;
		kInfo.m_iItemID		 = kTimeDropInfo.m_iDropItemID;
		kInfo.m_wstrItemName = pItemTemplet->m_Name;
		kInfo.m_kDropTime	 = tTimeDrop;
		kInfo.m_bIsDrop		 = false;
		kInfo.m_bNoticeMsg	 = kTimeDropInfo.m_bNoticeMsg;

		m_vecTimeDrop.push_back( kInfo );
		m_mapTimeDrop.insert( std::make_pair( kInfo.m_iTimeDropID, kInfo ) );

		START_LOG( cout, L"Registering timed-drop event!" )
			<< BUILD_LOG( kInfo.m_iTimeDropID )
			<< BUILD_LOG( kInfo.m_wstrItemName )
			<< BUILD_LOG( kTimeDropInfo.m_wstrDropTime )
			<< BUILD_LOG( kInfo.m_bNoticeMsg );
	}
}

bool KDropTable::CheckTimeDrop( OUT int& iTimeDropID, OUT int& iDropItemID )
{
	iDropItemID = 0;

	if( m_vecTimeDrop.empty() )
		return false;

	CTime tCurTime = CTime::GetCurrentTime();

	std::vector< KTimeDropItemInfo >::iterator vit;
	for( vit = m_vecTimeDrop.begin(); vit != m_vecTimeDrop.end(); ++vit )
	{
		// 아직 드롭할 시간이 아니다
		if( vit->m_kDropTime > tCurTime )
			return false;

		// 이벤트 적용!
		iTimeDropID = vit->m_iTimeDropID;
		iDropItemID = vit->m_iItemID;

		START_LOG( clog2, L"Timed drop succeeded!" )
			<< BUILD_LOG( iTimeDropID )
			<< BUILD_LOG( iDropItemID );

		break;
	}

	if( vit != m_vecTimeDrop.end() )
	{
		// 이벤트 리스트에서 삭제
        m_vecTimeDrop.erase( vit );
		return true;
	}

	return false;
}

bool KDropTable::RollbackTimeDrop( IN int iTimeDropID )
{
	std::map< int, KTimeDropItemInfo >::const_iterator mit;
	mit = m_mapTimeDrop.find( iTimeDropID );
	if( mit == m_mapTimeDrop.end() )
	{
		START_LOG( cerr, L"Timed-drop information does not exist." )
			<< BUILD_LOG( iTimeDropID )
			<< END_LOG;

		return false;
	}

	// 롤백!
	m_vecTimeDrop.insert( m_vecTimeDrop.begin(), mit->second );

	START_LOG( clog2, L"Rolled back timed-drop items." )
		<< BUILD_LOG( iTimeDropID );

	return true;
}
//}}

//{{ 2009. 6. 29  최육사	드롭 획득 공지
bool KDropTable::IsNotifyDropItem( IN int iItemID )
{
	if( !m_bNotifyDropItem )
		return false;

    return ( m_setNotifyDropItem.find( iItemID ) != m_setNotifyDropItem.end() );
}
//}}

//{{ 2011. 03. 28	최육사	서버 상태 체크
bool KDropTable::GetManyEDDropNpcInfo( IN const int iNpcID, OUT KManyEDDropNpcInfo& kInfo )
{
	if( IsManyEDDropNpcEvent() == false )
		return false;

	std::map< int, KManyEDDropNpcInfo >::const_iterator mit;
	mit = m_mapManyEDDropTable.find( iNpcID );
	if( mit == m_mapManyEDDropTable.end() )
		return false;

    kInfo = mit->second;
	return true;
}
//}}

//{{ 2011. 05. 04  김민성	던전 클리어시 아이템 지급 조건 추가
bool KDropTable::AddPaymentDungeon_LUA( IN int iDungeonID )
{

	KLuaManager luaMgr( GetLuaState() );
	DUNGEON_CLEAR_REWARD sData;

	// 경험치,VP,ED 보너스 이벤트

	//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
	sData.m_iDungeonID	= iDungeonID;
	//}}

	// AddUnPaymentCondition_LUA 부분의 초기화 정보와 일치시킬 수 있도록 수정하는 것이 좋습니다.
	LUA_GET_VALUE( luaMgr,		"iItemID",			sData.m_iItemID,		0 );
	LUA_GET_VALUE( luaMgr,		"fRate",			sData.m_fRate,			0.f );
	LUA_GET_VALUE( luaMgr,		"iQuestID",			sData.m_iQuestID,		0 );
	LUA_GET_VALUE( luaMgr,		"iGoingQuestID",	sData.m_iGoingQuestID,		0 );
	LUA_GET_VALUE( luaMgr,		"iMinLv",			sData.m_iMinLv,			0 );
	LUA_GET_VALUE( luaMgr,		"iMaxLv",			sData.m_iMaxLv,			99 );
	LUA_GET_VALUE( luaMgr,		"iQuantity",		sData.m_iQuantity,		1 );
	LUA_GET_VALUE( luaMgr,		"cUnitClass",				sData.m_cUnitClass,			CXSLUnit::UC_NONE );
	LUA_GET_VALUE( luaMgr,		"fPcBangUserRate",			sData.m_fPcBangUserRate,	0.f );

	LUA_GET_VALUE( luaMgr,		"iTitleID1",			sData.m_iTitleID,			0 );
	LUA_GET_VALUE( luaMgr,		"iTitleID2",			sData.m_iTitleID2,			0 );
	LUA_GET_VALUE_ENUM( luaMgr,		"eUnitType",		sData.m_eUnitType,		CXSLUnit::UNIT_TYPE,	CXSLUnit::UT_NONE);
	LUA_GET_VALUE( luaMgr,		"bPartyPay",			sData.m_bPartyPay,		false);
	LUA_GET_VALUE( luaMgr,		"bComebak",				sData.m_bComeback,		false);
	LUA_GET_VALUE( luaMgr,		"iEquippedItemID1",		sData.m_iEquippedItemID1,		0 );
	LUA_GET_VALUE( luaMgr,		"iEquippedItemID2",		sData.m_iEquippedItemID2,		0);
	LUA_GET_VALUE( luaMgr,		"iUseItemID",			sData.m_iUseItemID,		0 );
	LUA_GET_VALUE( luaMgr,		"bMustParty",			sData.m_bMustParty,		false );

	LUA_GET_VALUE( luaMgr,		"iConsumingItemID",		sData.m_iConsumingItemID,		0 );

	if( sData.m_bPartyPay == true && sData.m_iConsumingItemID > 0 )
	{
		START_LOG( cerr, L"Party rewards and item consumption cannot be used together!" )
			<< BUILD_LOG( sData.m_bPartyPay )
			<< BUILD_LOG( sData.m_iConsumingItemID )
			<< END_LOG;

		return false;
	}

	LUA_GET_VALUE( luaMgr,		"iAllyNPCID",			sData.m_iAllyNPCID,		0 );

	//{{ 2012. 11. 05	박세훈	PaymentItem 기간 조건 추가
	std::wstring wstrDate;
	LUA_GET_VALUE( luaMgr,		L"wstrBeginDate",	wstrDate,				L"" );
	if( wstrDate.empty() == false )
	{
		if( KncUtil::ConvertStringToCTime( wstrDate, sData.m_tBeginDate ) == false )
		{
			START_LOG( cerr, L"Time conversion failed." )
				<< BUILD_LOG( wstrDate )
				<< BUILD_LOG( iDungeonID )
				<< BUILD_LOG( sData.m_iItemID )
				<< BUILD_LOG( sData.m_fRate )
				<< END_LOG;
			return false;
		}
	}
	else
	{
		sData.m_tBeginDate = 0;
	}

	LUA_GET_VALUE( luaMgr,		L"wstrEndDate",	wstrDate,				L"" );
	if( wstrDate.empty() == false )
	{
		if( KncUtil::ConvertStringToCTime( wstrDate, sData.m_tEndDate ) == false )
		{
			START_LOG( cerr, L"Time conversion failed." )
				<< BUILD_LOG( wstrDate )
				<< BUILD_LOG( iDungeonID )
				<< BUILD_LOG( sData.m_iItemID )
				<< BUILD_LOG( sData.m_fRate )
				<< END_LOG;
			return false;
		}
	}
	else
	{
		sData.m_tEndDate = 0;
	}
	//}}

	// 입력 값 검사(필수값들만 검사)
	if( ( iDungeonID < 0 ) || ( sData.m_iItemID <=0 ) || ( sData.m_fRate > 100.f ) || ( sData.m_iQuantity < 1 ) || ( sData.m_fPcBangUserRate > 100.f ))
	{
		START_LOG( cerr, L"Invalid item-reward settings for dungeon clear." )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( sData.m_iItemID )
			<< BUILD_LOG( sData.m_fRate )
			<< BUILD_LOG( sData.m_iQuestID )
			<< BUILD_LOG( sData.m_iGoingQuestID )
			<< BUILD_LOG( sData.m_iMinLv )
			<< BUILD_LOG( sData.m_iMaxLv )
			<< BUILD_LOG( sData.m_iQuantity )
			<< BUILD_LOGc( sData.m_cUnitClass )
			<< BUILD_LOG( sData.m_fPcBangUserRate )
			<< BUILD_LOG( sData.m_iTitleID )
			<< BUILD_LOG( sData.m_iTitleID2 )
			<< BUILD_LOG( sData.m_eUnitType )
			<< BUILD_LOG( sData.m_bPartyPay )
			<< BUILD_LOG( sData.m_bComeback )
			<< BUILD_LOG( sData.m_iEquippedItemID1 )
			<< BUILD_LOG( sData.m_iEquippedItemID2 )
			<< BUILD_LOG( sData.m_iUseItemID )
			<< BUILD_LOG( sData.m_bMustParty )
			<< BUILD_LOG( sData.m_iConsumingItemID )
			<< BUILD_LOG( sData.m_iAllyNPCID )
			<< END_LOG;

		return false;
	}

	std::map< int, std::vector< DUNGEON_CLEAR_REWARD > >::iterator mit = m_mapPaymentItemTable.find( iDungeonID );
	if( mit == m_mapPaymentItemTable.end() )  // 해당 DungeonID 의 데이터가 없다.
	{
		std::vector< DUNGEON_CLEAR_REWARD > vecNewData;
		vecNewData.push_back( sData );

		m_mapPaymentItemTable.insert( std::make_pair( iDungeonID, vecNewData ) );
	}
	else  // 해당 DungeonID 의 데이터가 있다.
	{
		mit->second.push_back( sData );

		//std::vector< DUNGEON_CLEAR_REWARD >::iterator vit = mit->second.begin();
		//if( vit == mit->second.end() )  // 해당 조건이 없다? 있을 수 있는가?
		//{
		//	mit->second.push_back( sData );
		//}
		//else  // 중복되는 조건이 있다면 입력하지 말자
		//{
		//	for( ; vit != mit->second.end() ; ++vit )
		//	{
		//		// 조건을 찾는다   퀘스트, 경험치받는던전, 레벨
		//		if( (sData.m_iItemID == vit->m_iItemID)
		//		 && (sData.m_iQuestID == vit->m_iQuestID)
		//		 && sData.m_cUnitClass == vit->m_cUnitClass
		//		 && ((sData.m_iMaxLv >= vit->m_iMinLv) || (vit->m_iMaxLv >= sData.m_iMinLv))  )
		//		{
		//			START_LOG( cerr, L"던전 클리어시 아이템 지급 관련 중복 데이터가 존재한다!!" )
		//				<< BUILD_LOG( iDungeonID )
		//				<< BUILD_LOG( sData.m_iItemID )
		//				<< BUILD_LOG( sData.m_fRate )
		//				<< BUILD_LOG( sData.m_iQuestID )
		//				<< BUILD_LOG( sData.m_iMinLv )
		//				<< BUILD_LOG( sData.m_iMaxLv )
		//				<< BUILD_LOG( sData.m_iQuantity )
		//				<< BUILD_LOGc( sData.m_cUnitClass )
		//				<< BUILD_LOG( sData.m_fPcBangUserRate )
		//				<< END_LOG;

		//			return false;
		//		}
		//	}
		//
		//	mit->second.push_back( sData );

		//	START_LOG( clog, L"던전 클리어시 아이템 지급 관련 추가 성공!!" )
		//		<< BUILD_LOG( iDungeonID )
		//		<< BUILD_LOG( sData.m_iItemID )
		//		<< BUILD_LOG( sData.m_fRate )
		//		<< BUILD_LOG( sData.m_iQuestID )
		//		<< BUILD_LOG( sData.m_iMinLv )
		//		<< BUILD_LOG( sData.m_iMaxLv )
		//		<< BUILD_LOG( sData.m_iQuantity )
		//		<< BUILD_LOGc( sData.m_cUnitClass )
		//		<< BUILD_LOG( sData.m_fPcBangUserRate )
		//		<< END_LOG;
		//}
	}

	return true;
}


bool KDropTable::CheckDungeonClearPaymentItem( IN int iDungeonID, IN char cDifficulty, IN bool bExp, KRoomUserPtr spRoomUser,
											  KRoomUserManagerPtr spRoomUserManager,
											  OUT std::map< int, KItemInfo >& DungeonClearPaymentItem, IN bool bAraEvent )

{
	std::map< int, std::vector< DUNGEON_CLEAR_REWARD > >::iterator mit;

	// 1. 모든 던전 조건
	mit = m_mapPaymentItemTable.find( 0 );
	if( mit != m_mapPaymentItemTable.end() )
	{
		ProcessPaymentItem( mit,
			//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
			iDungeonID,
			cDifficulty,
			//}}
			spRoomUser,
			spRoomUserManager,
			DungeonClearPaymentItem
			, bAraEvent
			);
	}

	// 2. 경험치를 받는 던전 조건
	mit = m_mapPaymentItemTable.find( 1 );
	if( mit != m_mapPaymentItemTable.end() && bExp == true )
	{
		ProcessPaymentItem( mit,
			//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
			iDungeonID,
			cDifficulty,
			//}}
			spRoomUser,
			spRoomUserManager,
			DungeonClearPaymentItem
			);
	}

	// 3. 적정레벨 조건이 맞는 던전 조건
	int iDungeonMinLevel = SiCXSLDungeonManager()->GetDungeonMinLevel( iDungeonID + static_cast<int>(cDifficulty) );
	int iDungeonMaxLevel = SiCXSLDungeonManager()->GetDungeonMaxLevel( iDungeonID + static_cast<int>(cDifficulty) );

	if( (iDungeonMinLevel - DUNGEON_CLEAR_REWARD::TL_MIN_LEVEL) <= spRoomUser->GetLevel()  && spRoomUser->GetLevel() <= (iDungeonMaxLevel + DUNGEON_CLEAR_REWARD::TL_MAX_LEVEL) )
	{
		mit = m_mapPaymentItemTable.find( 2 );
		if( mit != m_mapPaymentItemTable.end() && bExp == true )
		{
			ProcessPaymentItem( mit,
				//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
				iDungeonID,
				cDifficulty,
				//}}
				spRoomUser,
				spRoomUserManager,
				DungeonClearPaymentItem
				);
		}
	}

	// 4. 특정 던전, 특정 난이도 일 때
	int iDungeon = iDungeonID + static_cast<int>( cDifficulty );
	mit = m_mapPaymentItemTable.find( iDungeon );
	if( mit != m_mapPaymentItemTable.end() )
	{
		ProcessPaymentItem( mit,
			//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
			iDungeonID,
			cDifficulty,
			//}}
			spRoomUser,
			spRoomUserManager,
			DungeonClearPaymentItem
			);
	}

	return true;
}

bool KDropTable::GetPaymentItemInfo( IN const DUNGEON_CLEAR_REWARD& sData, IN bool bIsPcBang, OUT KItemInfo& kResultItemInfo ) const
{
	// 확률 계산
	KLottery Cal;
	float fRate = sData.m_fRate;

	if( bIsPcBang == true )
	{
		if( sData.m_fPcBangUserRate > fRate )
		{
			fRate = sData.m_fPcBangUserRate;
		}
	}

	Cal.AddCase( 1, fRate );

	if( Cal.Decision() == KLottery::CASE_BLANK )
	{
		START_LOG( clog, L"Acquisition probability calculation: failed to grant due to insufficient grant probability." )
			<< BUILD_LOG( sData.m_iItemID )
			<< BUILD_LOG( sData.m_fRate )
			<< END_LOG;
		return false;
	}

	// 아이템 지급
	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( sData.m_iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Item information is missing; failed to grant item on dungeon clear!!" )
			<< BUILD_LOG( sData.m_iItemID )
			<< END_LOG;
		return false;
	}
	else
	{
		kResultItemInfo.m_iItemID	 = pItemTemplet->m_ItemID;
		kResultItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
		kResultItemInfo.m_iQuantity	 = sData.m_iQuantity;
		kResultItemInfo.m_sEndurance = pItemTemplet->m_Endurance;
	}

	return true;
}

void KDropTable::ProcessPaymentItem( IN const std::map< int, std::vector< DUNGEON_CLEAR_REWARD > >::iterator& mit,
									//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
									IN const int iDungeonID,
									IN const char cDifficulty,
									//}}
									IN const KRoomUserPtr spRoomUser,
									IN const KRoomUserManagerPtr spRoomUserManager,
									OUT std::map< int, KItemInfo >& DungeonClearPaymentItem
									, IN const bool bAraEvent
									) const
{
	KRoomUserInfo sRoomUserInfo;
	spRoomUser->GetRoomUserInfo( sRoomUserInfo );

	std::vector< DUNGEON_CLEAR_REWARD >::iterator vit;
	vit = mit->second.begin();
	for( ; vit != mit->second.end() ; ++vit )
	{
		//{{ 2012. 11. 05	박세훈	PaymentItem 기간 조건 추가
		if( ( 0 < vit->m_tBeginDate.GetTime() ) || ( 0 < vit->m_tEndDate.GetTime() ) )
		{
			CTime tCurrentTime = CTime::GetCurrentTime();

			if( ( 0 < vit->m_tBeginDate.GetTime() ) &&
				( tCurrentTime < vit->m_tBeginDate )
				)
			{
				continue;
			}

			if( ( 0 < vit->m_tEndDate.GetTime() ) &&
				( vit->m_tEndDate <= tCurrentTime )
				)
			{
				continue;
			}
		}
		//}}

		//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
		DUNGEON_CLEAR_REWARD kCheckValue(	iDungeonID + static_cast<int>( cDifficulty ),
			vit->m_iItemID,
			vit->m_fRate,
			vit->m_iQuestID,
			vit->m_iGoingQuestID,
			vit->m_iMinLv,
			vit->m_iMaxLv
			, vit->m_iQuantity
			, vit->m_cUnitClass
			, vit->m_fPcBangUserRate
			, vit->m_iTitleID
			, vit->m_iTitleID2
			, vit->m_eUnitType
			, vit->m_bPartyPay
			, vit->m_bComeback
			, vit->m_iEquippedItemID1
			, vit->m_iEquippedItemID2
			, vit->m_iUseItemID
			, vit->m_bMustParty
			, vit->m_iConsumingItemID
			, vit->m_iAllyNPCID
			);

		if( CheckUnPaymentCondition( kCheckValue ) == true )
		{
			continue;
		}
		//}}

		if( vit->m_iQuestID == 63750 && bAraEvent == false )
			continue;

		if(_CheckPaymentCondition( *vit, spRoomUser, spRoomUserManager))
		{
			// 아이템 지급
			KItemInfo kResultItemInfo;
			if( GetPaymentItemInfo( *vit, spRoomUser->IsGameBang(), kResultItemInfo ) == true )
			{
				std::map< int, KItemInfo >::iterator mit = DungeonClearPaymentItem.find( kResultItemInfo.m_iItemID );
				if( mit != DungeonClearPaymentItem.end() )
				{
					// 아이템을 또 얻었으면 기존 수량에 더해준다.
					// 최대 수량보다 많아지면?? 글쌔...
					mit->second.m_iQuantity += kResultItemInfo.m_iQuantity;
				}
				else
					DungeonClearPaymentItem.insert( std::make_pair(kResultItemInfo.m_iItemID, kResultItemInfo ) );
				spRoomUser->AddItem( kResultItemInfo.m_iItemID, kResultItemInfo.m_iQuantity );

				// 클리어 보상이 지급되고, 해당 클리어 보상을 위해 소모해야 할 아이템이 있을 때 소모합니다.
				if( spRoomUser->HasSpecialItem( vit->m_iConsumingItemID ) == true )
				{
					const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( vit->m_iConsumingItemID );
					if( pItemTemplet == NULL )
					{
						START_LOG( cerr, L"Item information is missing; failed to consume item on dungeon clear!!" )
							<< BUILD_LOG( vit->m_iConsumingItemID )
							<< END_LOG;
					}
					else
					{
						KItemInfo kConsumingItemInfo;
						kConsumingItemInfo.m_iItemID	 = pItemTemplet->m_ItemID;
						kConsumingItemInfo.m_cUsageType = pItemTemplet->m_PeriodType;
						kConsumingItemInfo.m_iQuantity	 = -1;
						kConsumingItemInfo.m_sEndurance = pItemTemplet->m_Endurance;

						START_LOG( clog, L"Item to consume" )
							<< BUILD_LOG( kConsumingItemInfo.m_iItemID )
							<< BUILD_LOG( kConsumingItemInfo.m_iQuantity );

						//spRoomUser->AddItem( kConsumingItemInfo.m_iItemID, kConsumingItemInfo.m_iQuantity );
						DungeonClearPaymentItem.insert( std::make_pair( kConsumingItemInfo.m_iItemID, kConsumingItemInfo ) );
					}
				}
			}
		}
	}
}

//{{ 2012. 10. 17	박세훈	PaymentItem 금지 조건 추가
bool KDropTable::AddUnPaymentCondition_LUA( IN int iDungeonID )
{

	KLuaManager luaMgr( GetLuaState() );
	DUNGEON_CLEAR_REWARD sData;

	// 경험치,VP,ED 보너스 이벤트

	sData.m_iDungeonID	= iDungeonID;

	// AddPaymentDungeon_LUA 부분의 초기화 정보와 일치시킬 수 있도록 수정하는 것이 좋습니다.
	LUA_GET_VALUE( luaMgr,		"iItemID",			sData.m_iItemID,		0 );
	LUA_GET_VALUE( luaMgr,		"fRate",			sData.m_fRate,			0.f );
	LUA_GET_VALUE( luaMgr,		"iQuestID",			sData.m_iQuestID,		0 );
	LUA_GET_VALUE( luaMgr,		"iMinLv",			sData.m_iMinLv,			0 );
	LUA_GET_VALUE( luaMgr,		"iMaxLv",			sData.m_iMaxLv,			0 );
	LUA_GET_VALUE( luaMgr,		"iQuantity",		sData.m_iQuantity,		1 );
	LUA_GET_VALUE( luaMgr,		"cUnitClass",				sData.m_cUnitClass,			CXSLUnit::UC_NONE );
	LUA_GET_VALUE( luaMgr,		"fPcBangUserRate",			sData.m_fPcBangUserRate,	0.f );

	// 입력 값 검사(필수값들만 검사)
	//{{ 2012. 05. 9	박세훈	어둠의 문 개편
	if( ( iDungeonID < 0 ) || ( sData.m_iItemID <=0 ) || ( sData.m_fRate > 100.f ) || ( sData.m_iQuantity < 1 ) || ( sData.m_fPcBangUserRate > 100.f ))
	{
		START_LOG( cerr, L"Invalid settings for [limited] item rewards on dungeon clear." )
			<< BUILD_LOG( iDungeonID )
			<< BUILD_LOG( sData.m_iItemID )
			<< BUILD_LOG( sData.m_fRate )
			<< BUILD_LOG( sData.m_iQuestID )
			<< BUILD_LOG( sData.m_iMinLv )
			<< BUILD_LOG( sData.m_iMaxLv )
			<< BUILD_LOG( sData.m_iQuantity )
			<< BUILD_LOGc( sData.m_cUnitClass )
			<< BUILD_LOG( sData.m_fPcBangUserRate )
			<< END_LOG;

		return false;
	}

	m_setUnPaymentCondition.insert( sData );

	return true;
}
//}}

bool KDropTable::_CheckPaymentCondition(IN const DUNGEON_CLEAR_REWARD& dcr, KRoomUserPtr spRoomUser, IN KRoomUserManagerPtr spRoomUserManager ) const
{
	if( dcr.m_bMustParty == true && spRoomUserManager->GetNumMember() == 1 )
	{
		START_LOG(clog, L"Dungeon-clear rewards: no reward for solo play!")
			<< END_LOG;
		return false;
	}

	KRoomUserInfo sRoomUserInfo;
	// 개인 지급
	if(dcr.m_bPartyPay == false)
	{
		START_LOG(clog, L"Dungeon-clear rewards: starting individual reward checks.")
			<< END_LOG;

		spRoomUser->GetRoomUserInfo( sRoomUserInfo );

		if( dcr.m_iAllyNPCID > 0 )
		{
			if( spRoomUser->IsAllyNPC( dcr.m_iAllyNPCID ) == false )
			{
				START_LOG( cwarn, L"No NPC companion is available." )
					<< BUILD_LOGc( dcr.m_iAllyNPCID )
					<< END_LOG;

				return false;
			}
		}

		if( dcr.m_iConsumingItemID != 0 )
		{
			if( spRoomUser->HasSpecialItem( dcr.m_iConsumingItemID ) == false )
			{
				START_LOG( cwarn, L"No item to consume." )
					<< BUILD_LOGc( dcr.m_iConsumingItemID )
					<< END_LOG;

				return false;
			}
		}
		return _CheckPaymentConditionDetail(dcr, sRoomUserInfo, spRoomUserManager);
	}
	else// 파티 지급
	{
		START_LOG(clog, L"Dungeon-clear rewards: starting party reward checks.")
			<< END_LOG;
		for( int i = 0; i < spRoomUserManager->GetNumMember(); ++i )
		{
			KRoomUserPtr spRoomUserForParty = spRoomUserManager->GetUser( i );// NULL 체크는 앞에서 미리 하니까 필요 없어요~
			spRoomUserForParty->GetRoomUserInfo( sRoomUserInfo );
			if( _CheckPaymentConditionDetail(dcr, sRoomUserInfo, spRoomUserManager) == true)
				return true;
		}
	}

	return false;
}

bool KDropTable::_CheckPaymentConditionDetail(IN const DUNGEON_CLEAR_REWARD& dcr, IN KRoomUserInfo& sRoomUserInfo, IN KRoomUserManagerPtr spRoomUserManager ) const
{
	// 1. 타이틀 체크
	if( _CheckTitlePass( sRoomUserInfo, spRoomUserManager, dcr ) == false )
		return false;

	START_LOG(clog2, L"Dungeon-clear rewards: check 1 passed - title requirement.")
		<< END_LOG;

	// 2. 퀘스트 체크
	if(dcr.m_iQuestID != 0)
	{
		std::set< int >::iterator qsit = sRoomUserInfo.m_setQuestInfo.find( dcr.m_iQuestID );
		if( qsit == sRoomUserInfo.m_setQuestInfo.end() ) // 퀘스트를 소유하고 있지 않다
			return false;
	}

	if( dcr.m_iGoingQuestID != 0 )
	{
		std::set< int >::iterator qsit = sRoomUserInfo.m_setGoingQuestInfo.find( dcr.m_iGoingQuestID );
		if( qsit == sRoomUserInfo.m_setGoingQuestInfo.end() ) // 퀘스트를 소유하고 있지 않다
			return false;
	}

	START_LOG(clog2, L"Dungeon-clear rewards: check 2 passed - quest requirement.")
		<< END_LOG;

	// 3. 레벨 체크
	if((dcr.m_iMinLv <= sRoomUserInfo.m_ucLevel && dcr.m_iMaxLv >= sRoomUserInfo.m_ucLevel) == false)
		return false;

	START_LOG(clog2, L"Dungeon-clear rewards: check 3 passed - level requirement.")
		<< END_LOG;

	if(dcr.m_eUnitType != CXSLUnit::UT_NONE)
	{
		// 4. 캐릭터 체크
		if(dcr.m_eUnitType != CXSLUnit::GetUnitClassToUnitType(static_cast<CXSLUnit::UNIT_CLASS>(sRoomUserInfo.m_cUnitClass)))
			return false;
	}

	START_LOG(clog2, L"Dungeon-clear rewards: check 4 passed - character requirement.")
		<< END_LOG;

	if(dcr.m_cUnitClass != CXSLUnit::UC_NONE && dcr.m_cUnitClass != sRoomUserInfo.m_cUnitClass)
	{
		return false;
	}

	START_LOG(clog2, L"Dungeon-clear rewards: check 4 passed - class requirement.")
		<< END_LOG;

	// 5. 복귀유저 체크
	if(dcr.m_bComeback != false)
	{
		if(sRoomUserInfo.m_bComeBackUser != true)
			return false;
	}
	START_LOG(clog2, L"Dungeon-clear rewards: check 5 passed - returning-player requirement.")
		<< END_LOG;

	// 착용 악세사리 체크
	if( _CheckEquippedItemPass( sRoomUserInfo, spRoomUserManager, dcr ) == false)
		return false;

	// 6. 사용 아이템 체크
	if(dcr.m_iUseItemID != 0)
	{
		if(sRoomUserInfo.m_bUseItem != true)
			return false;
	}
	START_LOG(clog2, L"Dungeon-clear rewards: check 6 passed - item-use requirement.")
		<< END_LOG;

	START_LOG(clog2, L"Dungeon-clear rewards: all checks passed!!")
		<< END_LOG;

	return true;
}

bool KDropTable::_CheckEquippedItem(IN KRoomUserInfo& sRoomUserInfo, IN const int iItemID) const
{
	std::map< int, KInventoryItemInfo >::const_iterator mit = sRoomUserInfo.m_mapEquippedItem.begin();
	while(mit != sRoomUserInfo.m_mapEquippedItem.end())
	{
		if(mit->second.m_kItemInfo.m_iItemID == iItemID)
			return true;
		mit++;
	}
	return false;
}
// 사용 아이템 체크 확인용
bool KDropTable::CheckPaymentUseItem(IN const int iItemID )
{
	std::map< int, std::vector< DUNGEON_CLEAR_REWARD > >::iterator mit;

	mit = m_mapPaymentItemTable.begin();
	for(; mit != m_mapPaymentItemTable.end(); ++mit)
	{
		if( _CheckPaymentUseItemDetail(mit, iItemID ) == true)
			return true;
	}

	return false;
}

bool KDropTable::_CheckPaymentUseItemDetail( std::map< int, std::vector< DUNGEON_CLEAR_REWARD > >::iterator& mit, IN const int iItemID) const
{
	std::vector< DUNGEON_CLEAR_REWARD >::const_iterator vit;
	vit = mit->second.begin();
	for( ; vit != mit->second.end() ; ++vit )
	{
		if( vit->m_iUseItemID == iItemID )
			return true;
	}

	return false;
}

bool KDropTable::_CheckEquippedItemPass( IN KRoomUserInfo& sRoomUserInfo, IN KRoomUserManagerPtr spRoomUserManager, IN const DUNGEON_CLEAR_REWARD& dcr ) const
{
	bool bPassEquippedItem =  false;
	if(dcr.m_iEquippedItemID1 != 0)
	{
		if(_CheckEquippedItem(sRoomUserInfo,  dcr.m_iEquippedItemID1) == true)
		{
			if( dcr.m_iEquippedItemID2 == 0 )
			{
				// 아이템 조건이 1개일 경우 본인이 착용하고 있으면 OK
				START_LOG(clog2, L"Dungeon-clear rewards: check 5 passed - solo item-equipped requirement.")
					<< END_LOG;
				bPassEquippedItem = true;
			}
			else
			{
				// 다른 유저들 착용했는지 찾자.
				for( int i = 0; i < spRoomUserManager->GetNumMember(); ++i )
				{
					KRoomUserInfo tempRoomUserInfo;
					KRoomUserPtr spRoomUserForParty = spRoomUserManager->GetUser( i );
					spRoomUserForParty->GetRoomUserInfo( tempRoomUserInfo );
					if(_CheckEquippedItem(tempRoomUserInfo, dcr.m_iEquippedItemID2 ) == true)
					{
						START_LOG(clog2, L"Dungeon-clear rewards: check 5 passed - both users have the item equipped.")
							<< END_LOG;
						bPassEquippedItem = true;
						break;
					}
				}
			}
		}
		else if( _CheckEquippedItem(sRoomUserInfo,  dcr.m_iEquippedItemID2) == true )
		{
			if( dcr.m_iEquippedItemID1 == 0 )
			{
				START_LOG(cerr, L"Dungeon-clear rewards: second item is configured, but the first ItemID is missing.")
					<< END_LOG;
			}
			else
			{
				// 다른 유저들 착용했는지 찾자.
				for( int i = 0; i < spRoomUserManager->GetNumMember(); ++i )
				{
					KRoomUserInfo tempRoomUserInfo;
					KRoomUserPtr spRoomUserForParty = spRoomUserManager->GetUser( i );
					spRoomUserForParty->GetRoomUserInfo( tempRoomUserInfo );
					if(_CheckEquippedItem(tempRoomUserInfo, dcr.m_iEquippedItemID1 ) == true)
					{
						START_LOG(clog2, L"Dungeon-clear rewards: check 5 passed - both users have the item equipped.")
							<< END_LOG;
						bPassEquippedItem = true;
						break;
					}
				}
			}
		}
	}
	else
		bPassEquippedItem = true;

	return bPassEquippedItem;
}

bool KDropTable::_CheckTitlePass( IN KRoomUserInfo& sRoomUserInfo, IN KRoomUserManagerPtr spRoomUserManager, IN const DUNGEON_CLEAR_REWARD& dcr ) const
{
	bool bPassTitle =  false;
	if(dcr.m_iTitleID != 0)
	{
		if( dcr.m_iTitleID == sRoomUserInfo.m_iTitleID )
		{
			if( dcr.m_iTitleID2 == 0 )
			{
				// 타이틀 조건이 1개일 경우 본인이 착용하고 있으면 OK
				START_LOG(clog2, L"Dungeon-clear rewards: check 1 passed - solo title requirement.")
					<< END_LOG;
				bPassTitle = true;
			}
			else
			{
				// 다른 유저들 착용했는지 찾자.
				for( int i = 0; i < spRoomUserManager->GetNumMember(); ++i )
				{
					KRoomUserInfo tempRoomUserInfo;
					KRoomUserPtr spRoomUserForParty = spRoomUserManager->GetUser( i );
					spRoomUserForParty->GetRoomUserInfo( tempRoomUserInfo );
					if( dcr.m_iTitleID2 == tempRoomUserInfo.m_iTitleID )
					{
						START_LOG(clog2, L"Dungeon-clear rewards: check 1 passed - both users meet the title requirement.")
							<< END_LOG;
						bPassTitle = true;
						break;
					}
				}
			}
		}
		else if( dcr.m_iTitleID2 != 0 && dcr.m_iTitleID2 == sRoomUserInfo.m_iTitleID )
		{
			if( dcr.m_iTitleID == 0 )
			{
				START_LOG(cerr, L"Dungeon-clear rewards: second title is configured, but the first title ID is missing.")
					<< END_LOG;
			}
			else
			{
				// 다른 유저들 착용했는지 찾자.
				for( int i = 0; i < spRoomUserManager->GetNumMember(); ++i )
				{
					KRoomUserInfo tempRoomUserInfo;
					KRoomUserPtr spRoomUserForParty = spRoomUserManager->GetUser( i );
					spRoomUserForParty->GetRoomUserInfo( tempRoomUserInfo );
					if( dcr.m_iTitleID == tempRoomUserInfo.m_iTitleID )
					{
						START_LOG(clog2, L"Dungeon-clear rewards: check 1 passed - both users have the title equipped.")
							<< END_LOG;
						bPassTitle = true;
						break;
					}
				}
			}
		}
	}
	else
		bPassTitle = true;

	return bPassTitle;
}

//}}

//{{ 2011. 08. 23	최육사	헤니르 시공 개편
bool KDropTable::AddHenirSpecialNpcDropInfo_LUA( int iNpcID, int iItemID, int iQuantity )
{
    if( iNpcID < 0  ||  iItemID <= 0  ||  iQuantity < 0 )
	{
        START_LOG( cerr, L"Invalid Henir NPC special-drop information." )
			<< BUILD_LOG( iNpcID )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( iQuantity )
			<< END_LOG;
		return false;
	}

	std::map< int, SHenirSpecialNpcDropInfo >::iterator mit;
	mit = m_mapHenirSpecialNpcDrop.find( iNpcID );
	if( mit == m_mapHenirSpecialNpcDrop.end() )
	{
		SHenirSpecialNpcDropInfo kInfo;
		kInfo.m_mapDropInfo.insert( std::make_pair( iItemID, iQuantity ) );
		m_mapHenirSpecialNpcDrop.insert( std::make_pair( iNpcID, kInfo ) );
	}
	else
	{
		if( mit->second.m_mapDropInfo.find( iItemID ) != mit->second.m_mapDropInfo.end() )
		{
			START_LOG( cerr, L"Henir NPC special-drop information is already registered!" )
				<< BUILD_LOG( iNpcID )
				<< BUILD_LOG( iItemID )
				<< BUILD_LOG( iQuantity )
				<< END_LOG;
			return false;
		}

		mit->second.m_mapDropInfo.insert( std::make_pair( iItemID, iQuantity ) );
	}

	return true;
}

//{{ 필드 드롭 개편 - 김민성
void KDropTable::GetHenirSpecialNpcDropInfo( IN const int iNpcID,IN float fTotalDropRate, OUT DROP_DATA& sDropDataHenir )
//}}
{
	if( fTotalDropRate <= 0.f)
		return;

	std::map< int, SHenirSpecialNpcDropInfo >::const_iterator mit;
	mit = m_mapHenirSpecialNpcDrop.find( iNpcID );
	if( mit == m_mapHenirSpecialNpcDrop.end() )
	{
		START_LOG( clog, L"No special-drop information for the specified monster!" )
			<< BUILD_LOG( iNpcID )
			<< END_LOG;
		return;
	}

	std::map< int, int >::const_iterator mitSD;
	for( mitSD = mit->second.m_mapDropInfo.begin(); mitSD != mit->second.m_mapDropInfo.end(); ++mitSD )
	{
		const int iDropItemID = mitSD->first;
		const int iDropCnt = mitSD->second;

		for( int iCnt = 0; iCnt < iDropCnt; ++iCnt )
		{
			sDropDataHenir.m_vecItemID.push_back( iDropItemID );
		}
	}
}

//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
bool KDropTable::GetComeBackBuffDropCount( IN const int& iRoomMember )
{
	// 확률 계산
	float fRate = 0.0f;

	switch( iRoomMember )
	{
	case 1: fRate = 1.3f;  break;
	case 2: fRate = 2.2f;  break;
	case 3: fRate = 3.1f;  break;
	case 4: fRate = 4.0f;  break;
	default:
		{
			START_LOG( cerr, L"Invalid party member count." )
				<< BUILD_LOG( iRoomMember )
				<< BUILD_LOG( fRate )
				<< END_LOG;
			return false;
		}
	}

	KLottery Cal;
	Cal.AddCase( 1, fRate );

	if( Cal.Decision() == KLottery::CASE_BLANK )
	{
		START_LOG( clog, L"Acquisition probability calculation: failed to grant due to insufficient grant probability." )
			<< BUILD_LOG( iRoomMember )
			<< BUILD_LOG( fRate )
			<< END_LOG;

		return false;
	}

	return true;	// 하나 지급
}
//}}

//{{ 2011. 11. 22	최육사	배틀필드 시스템
//{{ 필드 드롭 개편 - 김민성
//#ifdef SERV_REFORM_ITEM_DROP
bool KDropTable::GetBattleFieldNpcDropInfo( IN const SEnum::BATTLE_FIELD_ID eBattleFieldID, IN const int iNpcID, OUT KBattleFieldNpcDropInfo& kDropInfo )
{
	// 우선 해당 BattleFieldID + MonsterID에 해당하는 드롭 정보을 검색한다.
	std::map< std::pair< int, int >, KBattleFieldNpcDropInfo >::const_iterator mit;
	mit = m_mapBattleFieldNpcDrop.find( std::make_pair( eBattleFieldID, iNpcID ) );

	// 해당 경우에 대한 설정이 존재하지 않음.
	if( mit == m_mapBattleFieldNpcDrop.end() )
	{
		// 던전 아이디가 정해져 있지 않은 설정이 존재하는지 다시 한번 확인.
		mit = m_mapBattleFieldNpcDrop.find( std::make_pair( 0, iNpcID ) );

		// 그것마저 없다면?
		if( mit == m_mapBattleFieldNpcDrop.end() )
		{
			return false;
		}
	}

	kDropInfo = mit->second;
    return true;
}

bool KDropTable::BattleFieldNpcDropItem( IN const KBattleFieldNpcDropInfo& kDropInfo
										//{{ 2013. 03. 26	 필드 ED, EXP 팩터 추가 - 김민성
									   , IN const float fFieldEDFactor
									   , IN const float fFieldEXPFactor
										//}
									   , IN const float fLevelFactor
									   , IN const float fUserContribution
									   , IN const int iPartyCount_	// kimhc // 김현철 // 2013-12-16 // 기여도 변경 작업
									   , IN const float /*fPartyDropBonus*/
										//{{ 2012. 12. 16  드롭 이벤트 - 김민성
									   , IN const float fDropRate
										//}}
									   , IN const bool bIsBoss
									   , IN const float fBattleFieldBossFactor
									   , OUT DROP_DATA& sDropData
									   )
{
	//////////////////////////////////////////////////////////////////////////
	// X = 유저당 준데미지 / 몬스터HP (X 값은 최고 1.5를 넘지 않는다)
	// Y = NPC가 원래 주는 EXP
	// Z = NPC가 원래 주는 ED
	// B = 파티 보너스 = NPC설정 경험치 / ‘현재 지역에 있는’ 파티원 수 * ‘현재 지역에 있는’ 파티원 수에 따른 파티원 상수 * ‘매우어려움 고정’ 난이도 상수
	// A = 필드 상수 = SiCXSLBattleFieldManager()->GetBattleFieldFactor()
	// F = 레벨 보정 = 몬스터와의 레벨차가 +-10레벨 이상이면, 보상을 획득할 수 없다
	// T = 드롭 테이블 확률
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// EXP, ED

	// EXP = Y * X * A * F
	// ED =  Z * X * A * F
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 드롭 아이템

	// 아이템 드롭(획득) 확률 = T * X * A * ( B * 0.5 ) * F
	//////////////////////////////////////////////////////////////////////////

	sDropData.Clear();

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 EXP
	float fTemp;

	fTemp = kDropInfo.m_iExp * fUserContribution * fLevelFactor;

	//{{ 2013. 03. 26	 필드 ED, EXP 팩터 추가 - 김민성
	fTemp *= fFieldEXPFactor;
	//}

	fTemp *= fBattleFieldBossFactor;

	sDropData.m_iExp = static_cast<int>( fTemp );

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 ED
	fTemp = kDropInfo.m_iED * fUserContribution * fLevelFactor;

	//{{ 2013. 03. 26	 필드 ED, EXP 팩터 추가 - 김민성
	fTemp *= fFieldEDFactor;
	//}

	//{{ 2012. 12. 16  드롭 이벤트 - 김민성
	fTemp *= fDropRate;
	//}}

	int iGetTotalED = static_cast<int>( fTemp );

	//////////////////////////////////////////////////////////////////////////
	// NPC로부터 얻게되는 ED
	if( ( (float)rand() / static_cast<float>( RAND_MAX ) ) < kDropInfo.m_fEDProperty )
	{
		if( true == bIsBoss )
		{
			sDropData.m_nGPNum		= ( rand() % 11 + 10 ); // [10,20]
			sDropData.m_iED			= ( sDropData.m_nGPNum > 0 ? iGetTotalED / sDropData.m_nGPNum : 0 );
			sDropData.m_iGPItemID	= GetEDItemID( sDropData.m_iED );
		}
		else
		{
			sDropData.m_nGPNum      = rand() % 2 + 2;    // [2,3]
			sDropData.m_iED			= ( sDropData.m_nGPNum > 0 ? iGetTotalED / sDropData.m_nGPNum : 0 );
			sDropData.m_iGPItemID   = GetEDItemID( sDropData.m_iED );
		}

		KManyEDDropNpcInfo kManyDropInfo;
		if( GetManyEDDropNpcInfo( kDropInfo.m_nMonsterID, kManyDropInfo ) == true )
		{
			sDropData.m_nGPNum		= static_cast<int>(static_cast<float>(sDropData.m_nGPNum) * kManyDropInfo.m_fMultiplyDropEDCount);
			sDropData.m_iED			= static_cast<int>(static_cast<float>(sDropData.m_iED) * kManyDropInfo.m_fMultiplyDropED);
			sDropData.m_iGPItemID	= GetEDItemID( sDropData.m_iED );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 다음은 아이템.
	if( fUserContribution <= 0.0f )
		return true;

	// 아이템은 레벨 차이에 상관없이 드롭도록 변경
	//if( fLevelFactor <= 0.0f )
	//	return true;

	KLottery kDropInfoCopy = kDropInfo.m_kLottery;
	kDropInfoCopy.AddMultiProbRate( fUserContribution * iPartyCount_ );
	//{{ 2013. 03. 26	 필드 ED, EXP 팩터 추가 - 김민성
	kDropInfoCopy.AddMultiProbRate( fFieldEXPFactor );
	//}
//	kDropInfoCopy.AddMultiProbRate( fPartyDropBonus );

	//{{ 2012. 12. 16  드롭 이벤트 - 김민성

	{
		float fDropRateCopy = fDropRate;

		while( 0.0f < fDropRateCopy )
		{
			if( fDropRateCopy < 1.0f )
			{
				kDropInfoCopy.AddMultiProbRate( fDropRateCopy );
				fDropRateCopy = 0.0f;
			}
			else
			{
				fDropRateCopy -= 1.0f;
			}

			const int nCaseID = kDropInfoCopy.Decision();
			if( nCaseID == KLottery::CASE_BLANK )   // 획득한 아이템 없음.
				continue;

			const int nCaseParam = kDropInfoCopy.GetParam1( nCaseID );

			switch( nCaseParam )    // 단품 아이템, 아이템 그룹, 혹은 예상 못한 에러.
			{
			case DCP_UNARY_ITEM:    // 단품 아이템
				{
					sDropData.m_vecItemID.push_back( nCaseID );
				}
				break;
			case DCP_ITEM_GROUP:    // 아이템 그룹.
				{
					std::map< int, KLottery >::const_iterator mit;
					mit = m_mapDropGroup.find( nCaseID );
					if( mit == m_mapDropGroup.end() )
					{
						START_LOG( cerr, L"Invalid item-group information." )
							<< BUILD_LOG( nCaseID )
							<< BUILD_LOG( nCaseParam )
							<< BUILD_LOG( m_mapDropGroup.size() );
						continue;
					}

					// 아이템 그룹 중에서 실제 아이템 하나 선택.
					const int iResultItemID = mit->second.Decision();
					if( iResultItemID == KLottery::CASE_BLANK )   // 획득한 아이템 없음.
						continue;

					sDropData.m_vecItemID.push_back( iResultItemID );
				}
				break;
			default:
				{
					START_LOG( cerr, L"Never Get Here ~" )
						<< BUILD_LOG( nCaseParam )
						<< BUILD_LOG( nCaseID )
						<< BUILD_LOG( kDropInfo.m_kLottery.GetCaseNum() )
						<< BUILD_LOG( kDropInfo.m_kLottery.GetTotalProb() )
						<< END_LOG;
					continue;
				}
			}
		}
	}

	//}}

	return true;
}

bool KDropTable::BattleFieldStaticDropItem( IN const SEnum::BATTLE_FIELD_ID eBattleFieldID, OUT DROP_DATA& sDropData, const float fUserContribution )
{
	sDropData.m_vecItemID.clear();
	sDropData.m_iExp        = -1;
	sDropData.m_iGPItemID   = -1;
	sDropData.m_iED         = -1;
	sDropData.m_nGPNum      = -1;

	std::map< int, KBattleFieldStaticDropInfo >::const_iterator mit;
	mit = m_mapBattleFieldStaticDrop.find( eBattleFieldID );
	if( mit == m_mapBattleFieldStaticDrop.end() )
	{
		START_LOG( clog, L"No global-drop settings exist for this dungeon." )
			<< BUILD_LOG( eBattleFieldID );
		return true;
	}

	if( fUserContribution <= 0.0f )
		return true;

	KLottery kDropInfoCopy = mit->second.m_kLottery;
	kDropInfoCopy.AddMultiProbRate( fUserContribution );

	int iResultItemID = kDropInfoCopy.Decision();
	if( iResultItemID != KLottery::CASE_BLANK )
	{
		// 아이템 드롭!
		sDropData.m_vecItemID.push_back( iResultItemID );
	}

	return true;
}

//}}

//{{ 2012. 07. 17	박세훈	2012 대전 시즌2
float KDropTable::CalculateRebirthTime( IN const int& iMyTeamKillNum
									, IN const int& iEnemyTeamKillNum
									, IN const bool& bDidIHaveARebirthRing )
{
	return lua_tinker::call<float>( GetLuaState(), "CALCULATE_REBIRTH_TIME", iMyTeamKillNum, iEnemyTeamKillNum, bDidIHaveARebirthRing );
}

bool KDropTable::SuccessfullyGetTheItem( IN const CXSLRoom::PVP_GAME_TYPE& ePvpGameType, IN const int& iMyTeamKillNum, IN const int& iEnemyTeamKillNum )
{
	return lua_tinker::call<bool>( GetLuaState(), "SUCCESSFULLY_GET_THE_ITEM", ePvpGameType, iMyTeamKillNum, iEnemyTeamKillNum );
}
//}}

//{{ 2013. 1. 14	박세훈	던전 및 필드 드랍 아이템에 강화 부여
void KDropTable::SetEnchantItemDropEvent_LUA( void )
{
	if( m_kEnchantItemDropEventInfo.empty() == false )
	{
		START_LOG( cerr, L"Enhancement-item drop event is already configured.")
			<< BUILD_LOG( m_kEnchantItemDropEventInfo.m_vecEIDESettingInfo.size() )
			<< END_LOG;
		return;
	}

	KLuaManager luaManager( GetLuaState() );

	std::wstring wstrBeginDate;
	LUA_GET_VALUE_RETURN( luaManager,	L"m_wstrBeginDate",	wstrBeginDate,	L"",	return );
	if( KncUtil::ConvertStringToCTime( wstrBeginDate, m_kEnchantItemDropEventInfo.m_tBeginDate ) == false )
	{
		START_LOG( cerr, L"Failed to convert enhancement-item drop-event start time.")
			<< BUILD_LOG( wstrBeginDate )
			<< END_LOG;
		return;
	}

	std::wstring wstrEndDate;
	LUA_GET_VALUE_RETURN( luaManager,	L"m_wstrEndDate",	wstrEndDate,	L"",	return );
	if( KncUtil::ConvertStringToCTime( wstrEndDate, m_kEnchantItemDropEventInfo.m_tEndDate ) == false )
	{
		START_LOG( cerr, L"Failed to convert enhancement-item drop-event end time.")
			<< BUILD_LOG( wstrEndDate )
			<< END_LOG;
		return;
	}

	if( m_kEnchantItemDropEventInfo.m_tEndDate <= m_kEnchantItemDropEventInfo.m_tBeginDate )
	{
		START_LOG( cerr, L"Invalid enhancement-item drop-event time information.")
			<< BUILD_LOG( wstrBeginDate )
			<< BUILD_LOG( wstrEndDate )
			<< END_LOG;
		return;
	}

	for( int i=1; luaManager.BeginTable( i ) == S_OK; ++i )
	{
		EIDESettingInfo m_kEIDESettingInfo;
		LUA_GET_VALUE( luaManager,	"iMinLevel",			m_kEIDESettingInfo.m_iMinLevel,			0 );
		LUA_GET_VALUE( luaManager,	"iMaxLevel",			m_kEIDESettingInfo.m_iMaxLevel,			0 );

		for( int i=1; luaManager.BeginTable( i ) == S_OK; ++i )
		{
			char	cEnchantLevel;
			float	fRate;
			bool	bNotify;

			LUA_GET_VALUE_RETURN( luaManager,	"m_cEnchantLevel",		cEnchantLevel,		0,		goto end_proc );
			LUA_GET_VALUE_RETURN( luaManager,	"m_fRate",				fRate,				0.0f,	goto end_proc );
			LUA_GET_VALUE( luaManager,	"m_bNotify",			bNotify,			false );

			LIF( m_kEIDESettingInfo.m_kLottery.AddCase( cEnchantLevel, fRate ) );

			if( bNotify == true )
			{
				m_kEIDESettingInfo.m_setNotify.insert( cEnchantLevel );
			}
			luaManager.EndTable();
		}
		luaManager.EndTable();

		m_kEnchantItemDropEventInfo.m_vecEIDESettingInfo.push_back( m_kEIDESettingInfo );

		START_LOG( cout, L"Configuring Sage equipment event." )
			<< BUILD_LOG( m_kEIDESettingInfo.m_iMinLevel )
			<< BUILD_LOG( m_kEIDESettingInfo.m_iMaxLevel )
			<< BUILD_LOG( m_kEIDESettingInfo.m_kLottery.GetTotalProb() )
			<< BUILD_LOG( m_kEIDESettingInfo.m_setNotify.size() )
			<< END_LOG;
	}

	START_LOG( cout, L"Sage equipment event configured." )
		<< BUILD_LOG( wstrBeginDate )
		<< BUILD_LOG( wstrEndDate )
		<< BUILD_LOG( m_kEnchantItemDropEventInfo.m_vecEIDESettingInfo.size() )
		<< END_LOG;

	return;

end_proc:
	// 실패 처리
	m_kEnchantItemDropEventInfo.m_vecEIDESettingInfo.clear();

	START_LOG( cerr, L"Failed to read enchantment-level and probability settings." )
		<< END_LOG;
}

void KDropTable::EnchantItemDropEventProcess( IN const int iItemID, OUT char& cEnchantLevel, OUT bool& bNotify )
{
	// 이벤트 동작 시간인지 체크
	CTime tCurrentTime = CTime::GetCurrentTime();
	if(		( tCurrentTime < m_kEnchantItemDropEventInfo.m_tBeginDate )
		||	( m_kEnchantItemDropEventInfo.m_tEndDate <= tCurrentTime )
		)
	{
		return;
	}

	const CXSLItem::ItemTemplet* pItemTemplet = SiCXSLItemManager()->GetItemTemplet( iItemID );
	if( pItemTemplet == NULL )
	{
		START_LOG( cerr, L"Item does not exist." )
			<< BUILD_LOG( iItemID )
			<< END_LOG;
		return;
	}

	// 인챈트 불가 아이템은 넘어가자
	if( pItemTemplet->m_bCanEnchant == false )
	{
		return;
	}

	for( const EIDESettingInfo& kEIDESettingInfo : m_kEnchantItemDropEventInfo.m_vecEIDESettingInfo )
	{
		if( ( kEIDESettingInfo.m_iMinLevel != 0 ) && ( pItemTemplet->m_UseLevel < kEIDESettingInfo.m_iMinLevel ) )
		{
			continue;
		}

		if( ( kEIDESettingInfo.m_iMaxLevel != 0 ) && ( kEIDESettingInfo.m_iMaxLevel < pItemTemplet->m_UseLevel ) )
		{
			continue;
		}

		cEnchantLevel = static_cast<char>( kEIDESettingInfo.m_kLottery.Decision() );
		if( cEnchantLevel < 0 )
		{
			cEnchantLevel = 0;
		}

		bNotify = kEIDESettingInfo.m_setNotify.find( cEnchantLevel ) != kEIDESettingInfo.m_setNotify.end();
		break;
	}
}
//}}
