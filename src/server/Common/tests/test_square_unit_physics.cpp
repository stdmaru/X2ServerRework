// KSquareUnitPhysics reads SQUARE_Unit.lua as CX2SquareUnit does (decision T53): INIT_PHYSIC, and the states of
// INIT_STATE numbered by their index.
#include "LineMap/SquareUnitPhysics.h"

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST_CASE( "KSquareUnitPhysics reads the village unit script" )
{
	const fs::path root = fs::temp_directory_path() / "x2_square_unit_physics_test";
	fs::remove_all( root );
	fs::create_directories( root );

	std::ofstream( root / "Enum.lua" ) << "FALSE = 0\nTRUE = 1\nXSKIN_ANIM_PLAYTYPE = { XAP_LOOP = 0 }\n";
	// a state function is defined, not run; EMOTION_SAD is not in the list; MISSING has no table
	std::ofstream( root / "SQUARE_Unit.lua" ) << R"(
INIT_PHYSIC = { RUN_SPEED = 700, RELOAD_ACCEL = 2000, DASH_JUMP_SPEED = 1700, WALK_SPEED = 500, G_ACCEL = 4000,
	MAX_G_SPEED = -2000, JUMP_SPEED = 1500 }
INIT_STATE = { WAIT = "WAIT", "WAIT", "WALK", "DASH_JUMP_UP", "RIDING_DASH", "MISSING" }
WAIT = { SPEED_X = 0, PLAY_TYPE = XSKIN_ANIM_PLAYTYPE.XAP_LOOP, LAND_CONNECT = TRUE }
WALK = { PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED, FUNC_FRAME_MOVE = "FUNC_FRAME_MOVE_WALK" }
function FUNC_FRAME_MOVE_WALK( p0, p1, p2, p3 ) p3:SetXSpeed_LUA( INIT_PHYSIC.WALK_SPEED ) end
DASH_JUMP_UP = { LAND_CONNECT = FALSE, PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED, SPEED_Y = INIT_PHYSIC.JUMP_SPEED * 1.1 }
RIDING_DASH = { LAND_CONNECT = TRUE, PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED * 1.3 }
EMOTION_SAD = { SPEED_X = 0 }
)";

	// the working folder and its subfolders are searched first: the fixture's, so that no other script is found
	const fs::path oldCurrent = fs::current_path();
	fs::current_path( root );
	KSquareUnitPhysics kPhysics;
	const bool bLoaded = kPhysics.Load();
	fs::current_path( oldCurrent );
	REQUIRE( bLoaded );

	CHECK( kPhysics.GetPhysic().fWalkSpeed == doctest::Approx( 500.f ) );
	CHECK( kPhysics.GetPhysic().fMaxGSpeed == doctest::Approx( -2000.f ) );
	CHECK( kPhysics.GetPhysic().fGAccel == doctest::Approx( 4000.f ) );

	CHECK( kPhysics.GetNumStates() == 5 );
	CHECK( kPhysics.GetStateID( "WAIT" ) == 1 );
	CHECK( kPhysics.GetStateID( "RIDING_DASH" ) == 4 );
	CHECK( kPhysics.GetStateID( "EMOTION_SAD" ) == 0 );

	const KSquareUnitPhysics::State* pWait = kPhysics.GetState( 1 );
	REQUIRE( pWait != nullptr );
	CHECK( pWait->bSetsSpeedX );
	CHECK( pWait->fMaxSpeedX == doctest::Approx( 0.f ) );
	CHECK( !pWait->bSetsSpeedY );
	CHECK( pWait->bLandConnect );

	const KSquareUnitPhysics::State* pDashJump = kPhysics.GetState( kPhysics.GetStateID( "DASH_JUMP_UP" ) );
	REQUIRE( pDashJump != nullptr );
	CHECK( pDashJump->fMaxSpeedX == doctest::Approx( 700.f ) );
	CHECK( pDashJump->fMaxSpeedY == doctest::Approx( 1650.f ) );
	CHECK( !pDashJump->bLandConnect );

	// a state with no table keeps the defaults; the unnamed state stands
	const KSquareUnitPhysics::State* pMissing = kPhysics.GetState( 5 );
	REQUIRE( pMissing != nullptr );
	CHECK( pMissing->strName == "MISSING" );
	CHECK( !pMissing->bSetsSpeedX );
	CHECK( pMissing->bLandConnect );
	REQUIRE( kPhysics.GetState( 0 ) != nullptr );
	CHECK( kPhysics.GetState( 0 )->bLandConnect );
	CHECK( kPhysics.GetState( 6 ) == nullptr );
	CHECK( kPhysics.GetState( -1 ) == nullptr );

	CHECK( kPhysics.GetMaxSpeedX() == doctest::Approx( 910.f ) );
	CHECK( kPhysics.GetMaxSpeedY() == doctest::Approx( 1650.f ) );

	fs::remove_all( root );
}
