#include "BaseServer.h"
#include "./XSLPartyManager.h"


ImplementRefreshSingleton( CXSLPartyManager );

CXSLPartyManager::CXSLPartyManager(void) :
//{{ 2012. 11. 12	최육사	배틀필드 시스템
m_iActivateFeverPoint( 0 ),
m_fBasePercent( 0.f ),
//}}
//{{ 2012. 11. 16	최육사		헤니르 시공 비밀 스테이지 덤프
m_fPartyFeverFactor( 0.f ),
m_iEnterSuccessCount( 0 ),
//}}
m_iEnterFailCount( 0 )
{
	//{{ 2012. 11. 16	최육사		헤니르 시공 비밀 스테이지 덤프
	m_kHenirSecretEnterMinRate.m_fEnterSecretRate = 100.f;
	m_kHenirSecretEnterMaxRate.m_fEnterSecretRate = 0.f;
	//}}
}

CXSLPartyManager::~CXSLPartyManager(void)
{
}

ImplToStringW( CXSLPartyManager )
{
	stm_	<< L"----------[ Party Manager ]----------" << std::endl
		<< TOSTRINGW( m_mapMaxFeverPoint.size() )
		<< TOSTRINGW( m_mapIncreasePartyFever.size() )
		//{{ 2012. 11. 16	최육사		헤니르 시공 비밀 스테이지 덤프
		<< TOSTRINGW( m_iEnterSuccessCount )
		<< TOSTRINGW( m_iEnterFailCount )
		//}}
		;

	//{{ 2012. 11. 16	최육사		헤니르 시공 비밀 스테이지 덤프
	stm_	<< L"----------[ Henir Scret Enter Min Rate ]----------" << std::endl
		<< TOSTRINGW( m_kHenirSecretEnterMinRate.m_iFeverPoint )
		<< TOSTRINGW( m_kHenirSecretEnterMinRate.m_iNumMember )
		<< TOSTRINGW( m_kHenirSecretEnterMinRate.m_fEnterSecretRate )
		<< TOSTRINGW( m_kHenirSecretEnterMinRate.m_bEnterSuccess )
		;

	stm_	<< L"----------[ Henir Scret Enter Max Rate ]----------" << std::endl
		<< TOSTRINGW( m_kHenirSecretEnterMaxRate.m_iFeverPoint )
		<< TOSTRINGW( m_kHenirSecretEnterMaxRate.m_iNumMember )
		<< TOSTRINGW( m_kHenirSecretEnterMaxRate.m_fEnterSecretRate )
		<< TOSTRINGW( m_kHenirSecretEnterMaxRate.m_bEnterSuccess )
		;
	//}}

	return stm_;
}

ImplementLuaScriptParser( CXSLPartyManager )
{
	lua_tinker::class_add<CXSLPartyManager>( GetLuaState(), "CXSLRandomItemManager" );
	lua_tinker::class_def<CXSLPartyManager>( GetLuaState(), "SetActivateFeverPoint",			&CXSLPartyManager::SetActivateFeverPoint_LUA );
	lua_tinker::class_def<CXSLPartyManager>( GetLuaState(), "SetMaxFeverPointByNumMember",		&CXSLPartyManager::SetMaxFeverPointByNumMember_LUA );
	lua_tinker::class_def<CXSLPartyManager>( GetLuaState(), "SetIncreasePartyFeverByNumMember",	&CXSLPartyManager::SetIncreasePartyFeverByNumMember_LUA );
	//{{ 2012. 11. 12	최육사	배틀필드 시스템
	lua_tinker::class_def<CXSLPartyManager>( GetLuaState(), "SetDungeonSecretStageEnterFactor",	&CXSLPartyManager::SetDungeonSecretStageEnterFactor_LUA );
	//}}
	lua_tinker::class_def<CXSLPartyManager>( GetLuaState(), "dump",								&CXSLPartyManager::Dump );

	lua_tinker::decl( GetLuaState(), "g_pPartyManager", this );
}

void CXSLPartyManager::SetActivateFeverPoint_LUA( int iFever )
{
	if( iFever < 0 )
	{
		START_LOG( cerr, L"Invalid script value." )
			<< BUILD_LOG( iFever )
			<< END_LOG;
		return;
	}

	m_iActivateFeverPoint = iFever;

	START_LOG( cout, L"[Party] Fever activation points: " )
		<< BUILD_LOG( GetActivateFeverPoint() );
}

bool CXSLPartyManager::SetMaxFeverPointByNumMember_LUA( int iNumMember, int iMaxFever )
{
	if( iNumMember < 0  ||  iMaxFever < 0 )
	{
		START_LOG( cerr, L"Invalid script value." )
			<< BUILD_LOG( iNumMember )
			<< BUILD_LOG( iMaxFever )
			<< END_LOG;
		return false;
	}

	START_LOG( cout, L"[Party] Maximum fever by party size: " )
		<< BUILD_LOG( iNumMember )
		<< BUILD_LOG( iMaxFever );

	m_mapMaxFeverPoint.insert( std::make_pair( iNumMember, iMaxFever ) );
	return true;
}

bool CXSLPartyManager::SetIncreasePartyFeverByNumMember_LUA( int iNumMember, int iMin, int iMax )
{
	if( iNumMember < 0  ||  iMin < 0 || iMax < 0 )
	{
		START_LOG( cerr, L"Invalid script value." )
			<< BUILD_LOG( iNumMember )
			<< BUILD_LOG( iMin )
			<< BUILD_LOG( iMax )
			<< END_LOG;
		return false;
	}

	START_LOG( cout, L"[Party] Fever increase by party size: " )
		<< BUILD_LOG( iNumMember )
		<< BUILD_LOG( iMin )
		<< BUILD_LOG( iMax );

	// 최대값에서 최소값을 뺀것을 저장한다.
	SIncreasePartyFeverRange sData;
	sData.m_iMin = iMin;
	sData.m_iMax = iMax;
	sData.m_iMinMaxGap = iMax - iMin;
	m_mapIncreasePartyFever.insert( std::make_pair( iNumMember, sData ) );
	return true;
}

//{{ 2012. 11. 12	최육사	배틀필드 시스템
void CXSLPartyManager::SetDungeonSecretStageEnterFactor_LUA( float fBasePercent, float fPartyFeverFactor )
{
	if( fBasePercent < 0.f  ||  fPartyFeverFactor < 0.f )
	{
		START_LOG( cerr, L"Invalid script value." )
			<< BUILD_LOG( fBasePercent )
			<< BUILD_LOG( fPartyFeverFactor )
			<< END_LOG;
		return;
	}

	m_fBasePercent = fBasePercent;
	m_fPartyFeverFactor = fPartyFeverFactor;

	START_LOG( cout, L"[Party] Henir and Secret Dungeon entry constants: " )
		<< BUILD_LOG( fBasePercent )
		<< BUILD_LOG( fPartyFeverFactor );
}

float CXSLPartyManager::GetDungeonSecretStageEnterRate( IN const int iPartyFeverPoint, IN const int iPartyNumMember )
{
	float fEnterSecretRate = 0.0f;

	if( iPartyNumMember > 1 )
	{
		fEnterSecretRate = ( m_fBasePercent + ( static_cast<float>(iPartyFeverPoint) * m_fPartyFeverFactor ) ) * static_cast<float>( iPartyNumMember -1 );
	}
	else
	{
		fEnterSecretRate = 0.0f;
	}

	// 100%는 넘지 않도록!
	if( fEnterSecretRate > 100.f )
		fEnterSecretRate = 100.f;

	return fEnterSecretRate;
}
//}}

short CXSLPartyManager::GetMaxFeverPointByNumMember( IN const short sPartyFever, IN const int iPartyNumMember )
{
	short sPartyFeverResult = sPartyFever;

	// 멤버수 별로 최대값 제한
	std::map< int, int >::const_iterator mit;
	mit = m_mapMaxFeverPoint.find( iPartyNumMember );
	if( mit == m_mapMaxFeverPoint.end() )
	{
		START_LOG( clog, L"Undefined values are treated as zero." )
			<< BUILD_LOG( iPartyNumMember )
			<< BUILD_LOG( sPartyFever );
		return 0;
	}

	if( sPartyFeverResult > mit->second )
		sPartyFeverResult = static_cast<short>(mit->second);

	return sPartyFeverResult;
}

//{{ 2012. 11. 15	최육사		피버 증가 버그 수정
int	CXSLPartyManager::GetIncreasePartyFever( IN const int iNumMember )
{
	std::map< int, SIncreasePartyFeverRange >::const_iterator mit;
	mit = m_mapIncreasePartyFever.find( iNumMember );
	if( mit == m_mapIncreasePartyFever.end() )
	{
		START_LOG( clog, L"No fever increase value; returning zero." )
			<< BUILD_LOG( iNumMember );
		return 0;
	}

	int iResult = 0;
	const int iMinMaxGap = mit->second.m_iMinMaxGap;
	if( iMinMaxGap > 0 )
	{
		iResult = ( rand() % ( iMinMaxGap + 1 ) ) + mit->second.m_iMin;
	}
	else
	{
		iResult = mit->second.m_iMin;
	}

	return iResult;
}
//}}

//{{ 2012. 11. 16	최육사		헤니르 시공 비밀 스테이지 덤프
void CXSLPartyManager::SetHenirSecretStageEnterRateDump( IN const int iFeverPoint, IN const int iNumMember, IN const float fRate, IN const bool bEnterSuccess )
{
	// 최소값 저장
	if( fRate < m_kHenirSecretEnterMinRate.m_fEnterSecretRate )
	{
		m_kHenirSecretEnterMinRate.m_iFeverPoint	  = iFeverPoint;
		m_kHenirSecretEnterMinRate.m_iNumMember		  = iNumMember;
		m_kHenirSecretEnterMinRate.m_fEnterSecretRate = fRate;
		m_kHenirSecretEnterMinRate.m_bEnterSuccess	  = bEnterSuccess;
	}

	// 최대값 저장
	if( fRate > m_kHenirSecretEnterMaxRate.m_fEnterSecretRate )
	{
		m_kHenirSecretEnterMaxRate.m_iFeverPoint	  = iFeverPoint;
		m_kHenirSecretEnterMaxRate.m_iNumMember		  = iNumMember;
		m_kHenirSecretEnterMaxRate.m_fEnterSecretRate = fRate;
		m_kHenirSecretEnterMaxRate.m_bEnterSuccess	  = bEnterSuccess;
	}

	// 성공 실패 카운트
	if( bEnterSuccess )
	{
		++m_iEnterSuccessCount;
	}
	else
	{
		++m_iEnterFailCount;
	}
}
//}}
