#include "BaseServer.h"
#include "LineMap/SquareUnitPhysics.h"
#include "LineMap/ClientScriptState.h"

#include <KncLua.h>

#include <algorithm>
#include <cmath>

ImplementSingleton( KSquareUnitPhysics );

namespace
{

const char* const kUnitScript = "SQUARE_Unit.lua";

// The number in a field of the table on top of the stack.
bool GetNumber( lua_State* L, const char* szName, float& fValue )
{
	lua_getfield( L, -1, szName );
	const bool bNumber = lua_type( L, -1 ) == LUA_TNUMBER;
	if( bNumber )
		fValue = static_cast<float>( lua_tonumber( L, -1 ) );
	lua_pop( L, 1 );
	return bNumber;
}

// A flag the client reads as a number (TRUE = 1, FALSE = 0) or a boolean.
bool GetFlag( lua_State* L, const char* szName, bool bDefault )
{
	lua_getfield( L, -1, szName );
	bool bValue = bDefault;
	if( lua_type( L, -1 ) == LUA_TNUMBER )
		bValue = lua_tonumber( L, -1 ) != 0;
	else if( lua_type( L, -1 ) == LUA_TBOOLEAN )
		bValue = lua_toboolean( L, -1 ) != 0;
	lua_pop( L, 1 );
	return bValue;
}

// The larger of a state's speed and its passive speed on one axis.
bool GetMaxSpeed( lua_State* L, const char* szSpeed, const char* szPassiveSpeed, float& fMaxSpeed )
{
	float fSpeed = 0.f;
	float fPassiveSpeed = 0.f;
	const bool bSpeed = GetNumber( L, szSpeed, fSpeed );
	const bool bPassiveSpeed = GetNumber( L, szPassiveSpeed, fPassiveSpeed );
	fMaxSpeed = std::max( std::fabs( fSpeed ), std::fabs( fPassiveSpeed ) );
	return bSpeed || bPassiveSpeed;
}

}	// namespace

KSquareUnitPhysics::KSquareUnitPhysics() : m_vecState( 1 ) {}
KSquareUnitPhysics::~KSquareUnitPhysics() = default;

bool KSquareUnitPhysics::Load()
{
	m_kPhysic = Physic();
	m_vecState.assign( 1, State() );
	m_mapStateID.clear();
	m_fMaxSpeedX = 0.f;
	m_fMaxSpeedY = 0.f;

	KClientScriptState kLua;
	lua_State* L = kLua.Get();
	if( !kLua.Run( "Enum.lua", true ) || !kLua.Run( kUnitScript, true ) )
		return false;

	lua_getglobal( L, "INIT_PHYSIC" );
	if( !lua_istable( L, -1 ) )
	{
		lua_pop( L, 1 );
		START_LOG( cerr, L"The unit script has no INIT_PHYSIC." )
			<< BUILD_LOG( kUnitScript )
			<< END_LOG;
		return false;
	}
	GetNumber( L, "WALK_SPEED",		m_kPhysic.fWalkSpeed );
	GetNumber( L, "RUN_SPEED",			m_kPhysic.fRunSpeed );
	GetNumber( L, "JUMP_SPEED",		m_kPhysic.fJumpSpeed );
	GetNumber( L, "DASH_JUMP_SPEED",	m_kPhysic.fDashJumpSpeed );
	GetNumber( L, "G_ACCEL",			m_kPhysic.fGAccel );
	GetNumber( L, "MAX_G_SPEED",		m_kPhysic.fMaxGSpeed );
	GetNumber( L, "RELOAD_ACCEL",		m_kPhysic.fReloadAccel );
	lua_pop( L, 1 );

	lua_getglobal( L, "INIT_STATE" );
	if( !lua_istable( L, -1 ) )
	{
		lua_pop( L, 1 );
		START_LOG( cerr, L"The unit script has no INIT_STATE." )
			<< BUILD_LOG( kUnitScript )
			<< END_LOG;
		return false;
	}
	for( int i = 1; ; ++i )
	{
		lua_rawgeti( L, -1, i );
		const bool bName = lua_type( L, -1 ) == LUA_TSTRING;
		State kState;
		if( bName )
			kState.strName = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		if( !bName )
			break;

		lua_getglobal( L, kState.strName.c_str() );
		if( lua_istable( L, -1 ) )
		{
			kState.bSetsSpeedX = GetMaxSpeed( L, "SPEED_X", "PASSIVE_SPEED_X", kState.fMaxSpeedX );
			kState.bSetsSpeedY = GetMaxSpeed( L, "SPEED_Y", "PASSIVE_SPEED_Y", kState.fMaxSpeedY );
			kState.bLandConnect = GetFlag( L, "LAND_CONNECT", true );
		}
		else
		{
			START_LOG( cwarn, L"A unit state has no table." )
				<< BUILD_LOG( kState.strName )
				<< END_LOG;
		}
		lua_pop( L, 1 );

		m_fMaxSpeedX = std::max( m_fMaxSpeedX, kState.fMaxSpeedX );
		m_fMaxSpeedY = std::max( m_fMaxSpeedY, kState.fMaxSpeedY );
		m_mapStateID.emplace( kState.strName, i );
		m_vecState.push_back( std::move( kState ) );
	}
	lua_pop( L, 1 );

	if( GetNumStates() == 0 )
	{
		START_LOG( cerr, L"The unit script names no state." )
			<< BUILD_LOG( kUnitScript )
			<< END_LOG;
		return false;
	}

	START_LOG( cout, L"Village unit physics loaded." )
		<< BUILD_LOG( GetNumStates() )
		<< BUILD_LOG( m_kPhysic.fWalkSpeed )
		<< BUILD_LOG( m_kPhysic.fRunSpeed )
		<< BUILD_LOG( m_fMaxSpeedX )
		<< BUILD_LOG( m_fMaxSpeedY )
		<< END_LOG;
	return true;
}

const KSquareUnitPhysics::State* KSquareUnitPhysics::GetState( int iStateID ) const
{
	return 0 <= iStateID && iStateID < static_cast<int>( m_vecState.size() ) ? &m_vecState[ iStateID ] : nullptr;
}

int KSquareUnitPhysics::GetStateID( const std::string& strName ) const
{
	const auto mit = m_mapStateID.find( strName );
	return mit != m_mapStateID.end() ? mit->second : 0;
}
