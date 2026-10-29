#include "./KAbuserLogManager.h"

#include <dbg/dbg.hpp>
#include "BaseServer.h"
#include "NetError.h"
#include <iomanip>


ImplementRefreshSingleton( KAbuserLogManager );



KAbuserLogManager::KAbuserLogManager(void)
{
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	// 릴리즈틱 초기화
	for( int iIdx = 0; iIdx < AT_MAX; ++iIdx )
	{
		m_arrReleaseTick[iIdx] = 0;
	}
	//}}

	m_bAbuserEventLogEnable = false;

	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	m_iGetItemCountMin = INT_MAX;
	m_iGetItemCountMax = INT_MAX;
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	m_bAbuserCheckLevelUp = false;
	//}}
}

KAbuserLogManager::~KAbuserLogManager(void)
{
	if( m_wfsAbuserEventLog.is_open() )
		m_wfsAbuserEventLog.close();
}

ImplToStringW( KAbuserLogManager )
{
	std::wstring wstrAbuserLogPath = KncUtil::toWideString( m_strAbuserLogPath );

	stm_	<< L"----------[ Abuser Log Manager ]----------" << std::endl
			//std::set< UidType >				m_setAbuserList[AT_MAX];
			//int								m_arrReleaseTick[AT_MAX];
			<< TOSTRINGW( m_vecPhoneNumList.size() )
			<< TOSTRINGW( m_bAbuserEventLogEnable )
			<< TOSTRINGW( m_vecAbuserEventLog.size() )
			<< TOSTRINGW( wstrAbuserLogPath )
			//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
			<< TOSTRINGW( m_vecIP.size() )
			//}}
			//{{ 2010. 11. 17	최육사	오토핵 검증 기능
			<< TOSTRINGW( m_iGetItemCountMin )
			<< TOSTRINGW( m_iGetItemCountMax )
			//}}
			;

	return stm_;
}

ImplementLuaScriptParser( KAbuserLogManager )
{
	lua_tinker::class_add<KAbuserLogManager>( GetLuaState(), "KAbuserLogManager" );
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "SetAbuserLogPath",			&KAbuserLogManager::SetAbuserLogPath_LUA );
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "AddAbuserNotifyPhoneNum",		&KAbuserLogManager::AddAbuserNotifyPhoneNum_LUA );
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "SetAbuserEventLogEnable",		&KAbuserLogManager::SetAbuserEventLogEnable_LUA );
	//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "AddJoinAccountCheckIP",		&KAbuserLogManager::AddJoinAccountCheckIP_LUA );
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "SetGetItemCountMinMax",		&KAbuserLogManager::SetGetItemCountMinMax_LUA );
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "SetAbuserCheckLevelUp",		&KAbuserLogManager::SetAbuserCheckLevelUp_LUA );
	//}}


	lua_tinker::class_def<KAbuserLogManager>( GetLuaState(), "dump",						&KAbuserLogManager::Dump );

	lua_tinker::decl( GetLuaState(), "AbuserLogManager", this );
}

void KAbuserLogManager::SetAbuserLogPath_LUA( const char* pLogPath )
{
	m_strAbuserLogPath = pLogPath;
}

void KAbuserLogManager::SetAbuserEventLogEnable_LUA( bool bEnable )
{
    m_bAbuserEventLogEnable = bEnable;

	START_LOG( cout, L"어뷰저 이벤트 로그 동작 여부!" )
		<< BUILD_LOG( m_bAbuserEventLogEnable );
}

//{{ 2010. 11. 17	최육사	오토핵 검증 기능
void KAbuserLogManager::SetAbuserCheckLevelUp_LUA( bool bEnable )
{
	m_bAbuserCheckLevelUp = bEnable;

	START_LOG( cout, L"레벨업 간격 체크 동작 여부!" )
		<< BUILD_LOG( m_bAbuserCheckLevelUp );
}
//}}

void KAbuserLogManager::AddAbuserNotifyPhoneNum_LUA( const char* pPhoneNum )
{
	std::wstring wstrPhoneNum = KncUtil::toWideString( pPhoneNum, strlen( pPhoneNum ) );

	m_vecPhoneNumList.push_back( wstrPhoneNum );
}

//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그
void KAbuserLogManager::AddJoinAccountCheckIP_LUA( const char* pIP )
{
	if( pIP == NULL )
	{
		START_LOG( cerr, L"IP 스트링 이상." )
			<< END_LOG;

		return;
	}

	std::string strIP = pIP;
	if( strIP.size() < 1 || strIP.size() > 15 )
	{
		START_LOG( cerr, L"추가하려는 IP 이상." )
			<< BUILD_LOG( strIP.size() )
			<< END_LOG;

		return;
	}

	START_LOG( cout, L"=== ADD JOIN ACCOUNT CHECK IP ===" )
		<< BUILD_LOG( strIP.c_str() )
		<< BUILD_LOG( strIP.size() )
		<< L"================"
		<< dbg::endl;

	m_vecIP.push_back( strIP );
}
//}}

//{{ 2010. 11. 17	최육사	오토핵 검증 기능
void KAbuserLogManager::SetGetItemCountMinMax_LUA( int iGetItemCountMin, int iGetItemCountMax )
{
	m_iGetItemCountMin = iGetItemCountMin;
	m_iGetItemCountMax = iGetItemCountMax;

	START_LOG( cout, L"오토핵 유저 아이템 획득 카운트 체크!" )
		<< BUILD_LOG( m_iGetItemCountMin )
		<< BUILD_LOG( m_iGetItemCountMax );
}
//}}

void KAbuserLogManager::GetAbuserNotifyPhoneNumList( std::vector< std::wstring >& vecPhoneNumList ) const
{
	vecPhoneNumList.clear();
	vecPhoneNumList = m_vecPhoneNumList;
}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
void KAbuserLogManager::SendAbuserListReq()
{
	KDBE_ABUSER_LIST_REQ kPacketToDB;

	// 릴리즈틱 얻기
	for( int iIdx = 0; iIdx < AT_MAX; ++iIdx )
	{
		kPacketToDB.m_mapReleaseTick.insert( std::make_pair( iIdx, m_arrReleaseTick[iIdx] ) );
	}

	SendToLogDB( DBE_ABUSER_LIST_REQ, kPacketToDB );
}
//}}

void KAbuserLogManager::Tick()
{
	if( m_tRefreshTimer.elapsed() < 60.0 )
		return;

	m_tRefreshTimer.restart();

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	BEGIN_CHECK_TICK_LATENCY;
	//}}

	// DB로 어뷰저 리스트 요청
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	SendAbuserListReq();
	//}}

	// 세부로그 기록~!
	FlushAbuserEventLog();

	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	END_CHECK_TICK_LATENCY;
	//}}
}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
void KAbuserLogManager::UpdateAbuserList( const std::map< int, KAbuserList >& mapAbuserList )
{
	std::map< int, KAbuserList >::const_iterator mit;

	for( int iIdx = 0; iIdx < AT_MAX; ++iIdx )
	{
		mit = mapAbuserList.find( iIdx );
		if( mit == mapAbuserList.end() )
			continue;

		const KAbuserList& kAbuserList = mit->second;

		m_arrReleaseTick[iIdx] = kAbuserList.m_iReleaseTick;
		m_setAbuserList[iIdx].clear();

		std::vector< UidType >::const_iterator vit;
		for( vit = kAbuserList.m_vecAbuserList.begin(); vit != kAbuserList.m_vecAbuserList.end(); ++vit )
		{
			m_setAbuserList[iIdx].insert( *vit );
		}
	}
}

//}}
bool KAbuserLogManager::IsPvpAbuser( UidType iUnitUID ) const
{
	std::set< UidType >::const_iterator sit;
	sit = m_setAbuserList[AT_PVP_ABUSER].find( iUnitUID );
	if( sit != m_setAbuserList[AT_PVP_ABUSER].end() )
		return true;

	return false;
}

bool KAbuserLogManager::IsItemAbuser( UidType iUnitUID ) const
{
	std::set< UidType >::const_iterator sit;
	sit = m_setAbuserList[AT_ITEM_ABUSER].find( iUnitUID );
	if( sit != m_setAbuserList[AT_ITEM_ABUSER].end() )
		return true;

	return false;
}

bool KAbuserLogManager::IsEnchantAbuser( UidType iUnitUID ) const
{
	std::set< UidType >::const_iterator sit;
	sit = m_setAbuserList[AT_ENCHANT_ABUSER].find( iUnitUID );
	if( sit != m_setAbuserList[AT_ENCHANT_ABUSER].end() )
		return true;

	return false;
}

bool KAbuserLogManager::IsEDAbuser( UidType iUnitUID ) const
{
	std::set< UidType >::const_iterator sit;
	sit = m_setAbuserList[AT_ED_ABUSER].find( iUnitUID );
	if( sit != m_setAbuserList[AT_ED_ABUSER].end() )
		return true;

	return false;
}

bool KAbuserLogManager::IsMornitoringAbuser( UidType iUserUID ) const
{
	std::set< UidType >::const_iterator sit;
	sit = m_setAbuserList[AT_ABUSER_MONITORING].find( iUserUID );
	if( sit != m_setAbuserList[AT_ABUSER_MONITORING].end() )
		return true;

	return false;
}

//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
//}}

int KAbuserLogManager::ChangeEnchantAbuserResult( int iEnchantResult )
{
	switch( iEnchantResult )
	{
	case NetError::ERR_ENCHANT_RESULT_00: return ERS_SUCCESS;
	case NetError::ERR_ENCHANT_RESULT_01: return ERS_NO_CHANGE;
	case NetError::ERR_ENCHANT_RESULT_02: return ERS_LEVEL_DOWN;
	case NetError::ERR_ENCHANT_RESULT_03: return ERS_RESET;
	case NetError::ERR_ENCHANT_RESULT_04: return ERS_BROKEN;
	default:
		{
			START_LOG( cerr, L"정의 되지 않은 강화 결과입니다." )
				<< BUILD_LOG( iEnchantResult )
				<< END_LOG;
		}
		break;
	}
	return ERS_NONE;
}

//{{ 2008. 11. 12  최육사	세부로그
//{{ 2013. 05. 29	최육사	패킷 모니터링 근성도 항목 추가
void KAbuserLogManager::InsertAbuserEventLog( IN const UidType iUserUID, IN const UidType iUnitUID, IN const int iED, IN const int iGetItemCount, IN const int iSpirit, IN const unsigned short usEventID )
//}}
{
	CTime tCurTime = CTime::GetCurrentTime();

	KAbuserEventLog kEventLog;
	kEventLog.m_iUserUID		= iUserUID;
	kEventLog.m_iUnitUID		= iUnitUID;
	kEventLog.m_iED				= iED;
	kEventLog.m_iGetItemCount	= iGetItemCount;
	//{{ 2013. 05. 29	최육사	패킷 모니터링 근성도 항목 추가
	kEventLog.m_iSpirit			= iSpirit;
	//}}
	kEventLog.m_iRegDate		= tCurTime.GetTime();
	kEventLog.m_usEventID		= usEventID;

	m_vecAbuserEventLog.push_back( kEventLog );
}

#define KLocAlign std::setw(8) << std::setiosflags( std::ios::left )

//{{ 2010. 11. 17	최육사	오토핵 검증 기능

void KAbuserLogManager::FlushAbuserEventLog()
{
	if( m_vecAbuserEventLog.empty() )
		return;

	// 로그 기록
	if( m_bAbuserEventLogEnable )
	{
		KE_LOCAL_LOG_ABUSER_MORNITORING_NOT kPacketNot;
		kPacketNot.m_vecAbuserEventLog = m_vecAbuserEventLog;
		KSIManager.QueueingEvent( E_LOCAL_LOG_ABUSER_MORNITORING_NOT, kPacketNot );
	}

	m_vecAbuserEventLog.clear();
}

//}}

//{{ 2010. 11. 2	최육사	IP주소 계정 가입 로그

bool KAbuserLogManager::JoinAccountCheckIP( const std::wstring& wstrIP ) const
{
	// 체크 IP리스트가 비어있으면 통과!
	if( m_vecIP.empty() )
		return false;

	std::string strIP = KncUtil::toNarrowString( wstrIP );
	if( strIP.size() < 1 || strIP.size() > 15 )
	{
		START_LOG( cerr, L"비교하려는 IP 길이 이상." )
			<< BUILD_LOG( strIP.size() )
			<< END_LOG;

		return false;
	}

	// 특정 IP허용이라면..?
	std::vector< std::string >::const_iterator vit;
	for( vit = m_vecIP.begin(); vit != m_vecIP.end(); ++vit )
	{
		if( vit->size() < 1 || vit->size() > 15 )
		{
			START_LOG( cerr, L"비교하려는 IP 이상." )
				<< BUILD_LOG( vit->size() )
				<< END_LOG;

			return false;
		}

		// 해당 IP 대역이라면 허용!
		if( vit->compare( 0, vit->size(), strIP, 0, vit->size() ) == 0 )
		{
			return true;
		}
	}

	return false;
}

//}}

void KAbuserLogManager::SendToLogDB( unsigned short usEventID )
{
	SendToLogDB( usEventID, char() );
}
