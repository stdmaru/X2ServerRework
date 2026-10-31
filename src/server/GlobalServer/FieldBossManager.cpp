#include "FieldBossManager.h"

#include <Windows.h>

#include <intsafe.h>

ImplementSingleton( KFieldBossManager );

KFieldBossManager::KFieldBossManager( void )
	: m_tCheckTerm( 0 )
	, m_tPortalAppearanceTime( 0 )
	, m_iPortalAppearanceMap( 0 )
	, m_iPortalDestination( 0 )
	, m_byteState( 0 )
{
}

KFieldBossManager::~KFieldBossManager( void )
{
}

void KFieldBossManager::UpdateTotalDangerousValue( IN const int iPortalAppearanceMap, IN const byte byteValue )
{
	TYPE_MAP_TOTAL_DANGEROUS_VALUE_FIELD::iterator it = m_mapTotalDangerousValue.find( iPortalAppearanceMap );
	if( it == m_mapTotalDangerousValue.end() )
	{
		m_mapTotalDangerousValue.insert( TYPE_MAP_TOTAL_DANGEROUS_VALUE_FIELD::value_type( iPortalAppearanceMap, byteValue ) );
		return;
	}

	it->second = byteValue;
}

byte KFieldBossManager::GetTotalDangerousValue( IN const int iPortalAppearanceMap ) const
{
	TYPE_MAP_TOTAL_DANGEROUS_VALUE_FIELD::const_iterator it = m_mapTotalDangerousValue.find( iPortalAppearanceMap );
	if( it == m_mapTotalDangerousValue.end() )
	{
		return 0;
	}

	return it->second;
}

byte KFieldBossManager::GetTotalDangerousValue( void ) const
{
	byte byteTotalDangerousValueSum = 0;

	for( TYPE_MAP_TOTAL_DANGEROUS_VALUE_FIELD::const_iterator it = m_mapTotalDangerousValue.begin(); it != m_mapTotalDangerousValue.end(); ++it )
	{
		byteTotalDangerousValueSum += it->second;

		if( byteTotalDangerousValueSum < it->second )
			return BYTE_MAX;
	}

	return byteTotalDangerousValueSum;
}

void KFieldBossManager::OpenPortal( IN const int iPortalAppearanceMap, IN const int iPortalDestination, IN const __time64_t tPortalAppearanceTime )
{
	m_byteState				|= FBS_PORTAL_OPEN;
	m_iPortalAppearanceMap	= iPortalAppearanceMap;
	m_iPortalDestination	= iPortalDestination;
	m_tPortalAppearanceTime	= tPortalAppearanceTime;

	SetCoolTimeState( true );
}

bool KFieldBossManager::CheckTerm( IN const __time64_t tCurrentTime )
{
	// 1분마다 체크하자
	if( ( tCurrentTime - m_tCheckTerm ) <= 60 )
	{
		return false;
	}

	m_tCheckTerm = tCurrentTime;
	return true;
}
