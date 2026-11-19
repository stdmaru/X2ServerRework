#pragma once

// How a player's unit moves in villages, squares and wedding halls, for the servers to validate movement (decision
// T53). It comes from the client's own unit script, SQUARE_Unit.lua (every class uses it;
// CX2SquareUnit::GetSquareUnitScriptFileName), read as CX2SquareUnit reads it:
//   INIT_PHYSIC            speeds and gravity, in units per second;
//   INIT_STATE[1..n]       the states; a state's ID is its index, as the client sends it (u_char), and a state the
//                          list does not name (the client's map gives 0) is ID 0;
//   <STATE> = { ... }      SPEED_X/SPEED_Y set when the state starts, PASSIVE_SPEED_X/_Y while a key is held,
//                          LAND_CONNECT (default true) keeps the unit on its line.
// The state functions set the same speeds again (SetXSpeed_LUA) and are not run. A line can replace the horizontal
// speed (SPEED) or launch a jump (LT_JUMP_UP_REL, JUMP_SPEED): those come from the line map.

#include <KNCSingleton.h>

#include <map>
#include <string>
#include <vector>

class KSquareUnitPhysics
{
	DeclareSingleton( KSquareUnitPhysics );

public:
	struct Physic
	{
		float fWalkSpeed	= 0.f;
		float fRunSpeed		= 0.f;
		float fJumpSpeed	= 0.f;
		float fDashJumpSpeed= 0.f;
		float fGAccel		= 0.f;
		float fMaxGSpeed	= 0.f;	// negative: the fastest fall
		float fReloadAccel	= 0.f;
	};

	struct State
	{
		std::string	strName;
		// The fastest speed the state sets, the larger of SPEED_* and PASSIVE_SPEED_* in absolute value; 0 when it
		// sets none (the unit keeps the speed it had, as when an emotion starts).
		float		fMaxSpeedX		= 0.f;
		float		fMaxSpeedY		= 0.f;
		bool		bSetsSpeedX		= false;
		bool		bSetsSpeedY		= false;
		bool		bLandConnect	= true;
	};

	KSquareUnitPhysics();
	~KSquareUnitPhysics();

	// Loads SQUARE_Unit.lua; replaces what was loaded. False when the script, INIT_PHYSIC or INIT_STATE is missing.
	bool Load();

	const Physic& GetPhysic() const { return m_kPhysic; }

	// nullptr for an ID outside 0..GetNumStates() (0 is a state the list does not name: it stands on its line).
	const State* GetState( int iStateID ) const;
	int GetStateID( const std::string& strName ) const;	// 0 when the list does not name it
	int GetNumStates() const { return static_cast<int>( m_vecState.size() ) - 1; }

	// The fastest a unit moves in any state: horizontally, and upwards when a jump starts.
	float GetMaxSpeedX() const { return m_fMaxSpeedX; }
	float GetMaxSpeedY() const { return m_fMaxSpeedY; }

private:
	Physic					m_kPhysic;
	std::vector< State >	m_vecState;		// [0] is the unnamed state
	std::map< std::string, int > m_mapStateID;
	float					m_fMaxSpeedX = 0.f;
	float					m_fMaxSpeedY = 0.f;
};

DefSingletonInline( KSquareUnitPhysics );
