// KVillageMoveValidator (decision T53) on a fixture line map and the real unit script's numbers: walking, jumping
// onto a platform within reach and jump lines are accepted; running too fast, a fast clock, standing in the air,
// hovering and flying are not.
#include "LineMap/VillageMoveValidator.h"
#include "LineMap/SquareUnitPhysics.h"
#include "LineMap/WorldLineMaps.h"
#include "LineMap/XSLLineMap.h"

#include <doctest/doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <memory>

#ifdef X2_VILLAGE_SYNC

namespace fs = std::filesystem;
using Reason = KEGS_FIELD_UNIT_CORRECTION_NOT;

namespace
{

// state IDs: their index in INIT_STATE below
enum : u_char { WAIT = 1, WALK = 2, JUMP_UP = 3, JUMP_DOWN = 4, DASH = 5, DASH_JUMP_UP = 6 };

struct Fixture
{
	KSquareUnitPhysics					kPhysics;
	std::shared_ptr< CXSLLineMap >		spLineMap;
};

const Fixture& GetFixture()
{
	static std::unique_ptr< Fixture > s_spFixture;
	if( s_spFixture )
		return *s_spFixture;

	const fs::path root = fs::temp_directory_path() / "x2_village_move_validator_test";
	fs::remove_all( root );
	fs::create_directories( root );
	std::ofstream( root / "Enum.lua" ) << R"(
FALSE = 0
TRUE = 1
LINE_TYPE = { LT_NORMAL = 0, LT_WALL = 1, LT_POTAL = 2, LT_JUMP_UP_REL = 4 }
)";
	// the real script's numbers (2026-11-19)
	std::ofstream( root / "SQUARE_Unit.lua" ) << R"(
INIT_PHYSIC = { RUN_SPEED = 700, RELOAD_ACCEL = 2000, DASH_JUMP_SPEED = 1700, WALK_SPEED = 500, G_ACCEL = 4000,
	MAX_G_SPEED = -2000, JUMP_SPEED = 1500 }
INIT_STATE = { "WAIT", "WALK", "JUMP_UP", "JUMP_DOWN", "DASH", "DASH_JUMP_UP" }
WAIT = { SPEED_X = 0, LAND_CONNECT = TRUE }
WALK = { LAND_CONNECT = TRUE, PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED }
JUMP_UP = { LAND_CONNECT = FALSE, PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED, SPEED_Y = INIT_PHYSIC.JUMP_SPEED }
JUMP_DOWN = { LAND_CONNECT = FALSE, PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED }
DASH = { LAND_CONNECT = TRUE, PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED }
DASH_JUMP_UP = { LAND_CONNECT = FALSE, PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED, SPEED_Y = INIT_PHYSIC.JUMP_SPEED * 1.1 }
)";
	// 0 floor, 1 platform within a dash jump (300 < 340), 2 platform out of reach, 3 jump line, 4 moving walkway,
	// 5 floor past the jump line, ending at 9000, 6 a wall on it, 7 a ledge above it whose end at 7500 is closed
	std::ofstream( root / "Test_LineMap.lua" ) << R"(
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 0, 0, 0 ), END_POS = D3DXVECTOR3( 3000, 0, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 500, 300, 0 ), END_POS = D3DXVECTOR3( 900, 300, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 1200, 1000, 0 ), END_POS = D3DXVECTOR3( 1500, 1000, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_JUMP_UP_REL, START_POS = D3DXVECTOR3( 3000, 0, 0 ), END_POS = D3DXVECTOR3( 3200, 0, 0 ),
	JUMP_SPEED_X = 800, JUMP_SPEED_Y = 3000 })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( -3000, 0, 0 ), END_POS = D3DXVECTOR3( -1000, 0, 0 ),
	SPEED = 5000 })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 3200, 0, 0 ), END_POS = D3DXVECTOR3( 9000, 0, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_WALL, START_POS = D3DXVECTOR3( 6000, 0, 0 ), END_POS = D3DXVECTOR3( 6000, 300, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 7000, 200, 0 ), END_POS = D3DXVECTOR3( 7500, 200, 0 ),
	NEXT_LINE = -2 })
)";

	const fs::path oldCurrent = fs::current_path();
	fs::current_path( root );	// searched first: only the fixture's scripts are found
	auto spFixture = std::make_unique< Fixture >();
	const bool bPhysics = spFixture->kPhysics.Load();
	spFixture->spLineMap = KWorldLineMaps::LoadLineMap( "Test_LineMap.lua" );
	fs::current_path( oldCurrent );
	fs::remove_all( root );
	REQUIRE( bPhysics );
	REQUIRE( spFixture->spLineMap != nullptr );
	REQUIRE( spFixture->spLineMap->GetNumLineData() == 7 );	// the wall is kept apart
	REQUIRE( spFixture->spLineMap->GetWallList().size() == 1 );
	s_spFixture = std::move( spFixture );
	return *s_spFixture;
}

KFieldUnitState State( float x, float y, u_char ucStateID, int16_t sSpeedY = 0 )
{
	KFieldUnitState kState;
	kState.m_vPos = VECTOR3( x, y, 0.f );
	kState.m_ucStateID = ucStateID;
	kState.m_sSpeedY = sSpeedY;
	return kState;
}

// A unit and its clocks: the client's tick and the server's time advance together unless told otherwise.
struct Unit
{
	KVillageMoveValidator	kValidator;
	uint32_t				uiTick		= 100000;
	uint32_t				uiServer	= 5000;

	explicit Unit( const KFieldUnitState& kStart, const KVillageMoveValidator::Config& kConfig = KVillageMoveValidator::Config() )
		: kValidator( GetFixture().spLineMap.get(), GetFixture().kPhysics, kConfig )
	{
		kValidator.Reset( kStart, uiServer );
	}

	KVillageMoveValidator::Verdict Send( const KFieldUnitState& kState, uint32_t uiStepMs = 100, uint32_t uiServerStepMs = UINT32_MAX )
	{
		uiTick += uiStepMs;
		uiServer += uiServerStepMs == UINT32_MAX ? uiStepMs : uiServerStepMs;
		return kValidator.Check( uiTick, kState, uiServer );
	}

	u_char Reason( const KFieldUnitState& kState, uint32_t uiStepMs = 100 ) { return Send( kState, uiStepMs ).ucReason; }
};

// Samples a jump from (x, y) with the unit script's physics every 50 ms until it lands back at y, and returns the
// highest reason (R_NONE when every sample is accepted).
u_char Jump( Unit& kUnit, float x, float y, float fSpeedY, float fSpeedX, u_char ucStateID )
{
	const float g = 4000.f;
	for( int i = 1; ; ++i )
	{
		const float t = 0.05f * static_cast< float >( i );
		const float h = fSpeedY * t - 0.5f * g * t * t;
		if( h <= 0.f )
			break;
		const u_char ucReason = kUnit.Reason( State( x + fSpeedX * t, y + h, ucStateID, static_cast< int16_t >( fSpeedY - g * t ) ), 50 );
		if( ucReason != Reason::R_NONE )
			return ucReason;
	}
	return Reason::R_NONE;
}

}	// namespace

TEST_CASE( "KVillageMoveValidator: walking and running" )
{
	Unit kUnit( State( 100, 0, WAIT ) );
	for( int i = 1; i <= 10; ++i )
		CHECK( kUnit.Reason( State( 100.f + 50.f * i, 0, WALK ) ) == Reason::R_NONE );		// 500 u/s
	for( int i = 1; i <= 10; ++i )
		CHECK( kUnit.Reason( State( 600.f + 70.f * i, 0, DASH ) ) == Reason::R_NONE );		// 700 u/s
	CHECK( kUnit.Reason( State( 1600, 0, DASH ) ) == Reason::R_SPEED );						// 2,900 u/s
	// a rejected update changes nothing: the unit goes on from the last valid position
	CHECK( kUnit.kValidator.GetLastValid().m_vPos.x == doctest::Approx( 1300.f ) );
	CHECK( kUnit.Reason( State( 1370, 0, DASH ) ) == Reason::R_NONE );
}

TEST_CASE( "KVillageMoveValidator: states, ticks, rate and clock" )
{
	Unit kUnit( State( 100, 0, WAIT ) );
	CHECK( kUnit.Reason( State( 100, 0, 50 ) ) == Reason::R_STATE );
	CHECK( kUnit.Reason( State( 100, 0, WAIT ) ) == Reason::R_NONE );
	CHECK( kUnit.Reason( State( 100, 0, WAIT ), 0 ) == Reason::R_RATE );		// the tick does not advance

	// more updates than the bucket holds at once
	Unit kBurst( State( 100, 0, WAIT ) );
	int iAccepted = 0;
	for( int i = 0; i < 30; ++i )
		iAccepted += kBurst.Send( State( 100, 0, WAIT ), 1, 0 ).IsValid() ? 1 : 0;
	CHECK( iAccepted == 10 );

	// a client clock running twice as fast: rejected once it is more than 500 ms ahead of where it was at the first
	// update (the clocks' base)
	Unit kFast( State( 100, 0, WAIT ) );
	int iFirstRejected = 0;
	for( int i = 1; i <= 20 && iFirstRejected == 0; ++i )
		if( kFast.Send( State( 100, 0, WAIT ), 100, 50 ).ucReason == Reason::R_SPEED )
			iFirstRejected = i;
	CHECK( iFirstRejected == 12 );
}

TEST_CASE( "KVillageMoveValidator: off the line and in the air" )
{
	Unit kUnit( State( 100, 0, WAIT ) );
	CHECK( kUnit.Reason( State( 100, 200, WAIT ) ) == Reason::R_OFF_LINE );		// standing on nothing
	CHECK( kUnit.Reason( State( 100, 15, WALK ) ) == Reason::R_NONE );			// within the line radius

	// a jump that follows the script's physics, landing back on the floor
	CHECK( Jump( kUnit, 100, 0, 1500.f, 500.f, JUMP_UP ) == Reason::R_NONE );
	CHECK( kUnit.Reason( State( 480, 0, WAIT ) ) == Reason::R_NONE );

	// flying straight up
	Unit kFly( State( 100, 0, WAIT ) );
	CHECK( kFly.Reason( State( 100, 120, JUMP_UP, 1500 ) ) == Reason::R_NONE );
	CHECK( kFly.Reason( State( 100, 600, JUMP_UP, 1500 ) ) == Reason::R_RISE );

	// rising to a reachable height and hovering there: fine near the apex, rejected once a real jump would have
	// fallen (the fastest launch, 1650, is above 210 until 0.67 s, plus the 0.1 s takeoff window)
	Unit kHover( State( 100, 0, WAIT ) );
	CHECK( kHover.Reason( State( 100, 130, JUMP_UP, 1100 ) ) == Reason::R_NONE );
	CHECK( kHover.Reason( State( 100, 220, JUMP_UP, 700 ) ) == Reason::R_NONE );
	CHECK( kHover.Reason( State( 100, 270, JUMP_UP, 300 ) ) == Reason::R_NONE );
	int iRejected = 0;
	for( int i = 1; i <= 20 && iRejected == 0; ++i )
		if( kHover.Send( State( 100, 270, JUMP_UP, 0 ) ).ucReason == Reason::R_RISE )
			iRejected = i;
	CHECK( iRejected == 5 );		// at 0.8 s
}

TEST_CASE( "KVillageMoveValidator: jumping again at once" )
{
	// jumps one after another, sampled every 100 ms: the landings (every 0.75 s) fall between updates
	const float g = 4000.f, fSpeedY = 1500.f, fPeriod = 2.f * fSpeedY / g;
	const auto Sample = [&]( float t )
	{
		const float tJump = std::fmod( t, fPeriod );
		return State( 100, fSpeedY * tJump - 0.5f * g * tJump * tJump, JUMP_UP, static_cast< int16_t >( fSpeedY - g * tJump ) );
	};
	Unit kUnit( State( 100, 0, WAIT ) );
	for( int i = 1; i <= 50; ++i )
		CHECK( kUnit.Reason( Sample( 0.1f * static_cast< float >( i ) ) ) == Reason::R_NONE );

	// falling slowly high above the floor and then rising: it cannot have landed in between
	Unit kHover( State( 100, 0, WAIT ) );
	CHECK( kHover.Reason( State( 100, 130, JUMP_UP, 1100 ) ) == Reason::R_NONE );
	CHECK( kHover.Reason( State( 100, 220, JUMP_UP, 700 ) ) == Reason::R_NONE );
	CHECK( kHover.Reason( State( 100, 270, JUMP_UP, 300 ) ) == Reason::R_NONE );
	int iRejected = 0;
	for( int i = 1; i <= 20 && iRejected == 0; ++i )
		if( kHover.Send( State( 100, 270, JUMP_UP, static_cast< int16_t >( i % 2 == 0 ? 100 : -1 ) ) ).ucReason == Reason::R_RISE )
			iRejected = i;
	CHECK( iRejected == 5 );		// as when it hovers without pretending to land
}

TEST_CASE( "KVillageMoveValidator: platforms" )
{
	// a dash jump reaches the platform at 300 and lands on it
	Unit kUnit( State( 500, 0, WAIT ) );
	for( int i = 1; i <= 4; ++i )
	{
		const float t = 0.05f * i;
		CHECK( kUnit.Reason( State( 500.f + 700.f * t, 1650.f * t - 2000.f * t * t, DASH_JUMP_UP, 1000 ), 50 ) == Reason::R_NONE );
	}
	CHECK( kUnit.Reason( State( 660, 300, JUMP_DOWN, -100 ), 50 ) == Reason::R_NONE );	// lands
	CHECK( kUnit.Reason( State( 680, 300, WAIT ), 50 ) == Reason::R_NONE );

	// from it, the platform at 1000 is out of reach
	CHECK( kUnit.Reason( State( 700, 400, DASH_JUMP_UP, 1650 ), 100 ) == Reason::R_NONE );
	CHECK( kUnit.Reason( State( 760, 700, DASH_JUMP_UP, 1000 ), 100 ) == Reason::R_RISE );
}

TEST_CASE( "KVillageMoveValidator: jump lines and moving walkways" )
{
	// a jump line throws the unit higher and farther, even in a state that keeps it on a line (the client does not
	// change its state)
	Unit kPad( State( 3100, 0, WAIT ) );
	CHECK( Jump( kPad, 3100, 0, 3000.f, 800.f, WALK ) == Reason::R_NONE );

	// the same without the jump line is standing in the air
	Unit kFloor( State( 2000, 0, WAIT ) );
	CHECK( Jump( kFloor, 2000, 0, 3000.f, 800.f, WALK ) == Reason::R_OFF_LINE );

	// a moving walkway carries the unit at its speed
	Unit kWalkway( State( -2900, 0, WAIT ) );
	for( int i = 1; i <= 3; ++i )
		CHECK( kWalkway.Reason( State( -2900.f + 450.f * i, 0, WAIT ) ) == Reason::R_NONE );
	Unit kNoWalkway( State( 100, 0, WAIT ) );
	CHECK( kNoWalkway.Reason( State( 550, 0, WAIT ) ) == Reason::R_SPEED );
}

TEST_CASE( "KVillageMoveValidator: walls" )
{
	// the end of a line with no other below (the edges of Elder): the client stops the unit there
	Unit kEdge( State( 8900, 0, WAIT ) );
	CHECK( kEdge.Reason( State( 8960, 0, WALK ) ) == Reason::R_NONE );
	CHECK( kEdge.Reason( State( 9010, 0, WALK ) ) == Reason::R_NONE );		// within the wall radius
	CHECK( kEdge.Reason( State( 9070, 0, WALK ) ) == Reason::R_WALL );
	CHECK( kEdge.Reason( State( 9070, 100, JUMP_UP, 1000 ) ) == Reason::R_WALL );	// in the air too

	// a narrow gap cannot be skipped between two updates: the floor ends at -1000 and starts again at 0
	Unit kGap( State( -1100, 0, WAIT ) );
	CHECK( kGap.Reason( State( 50, 0, WAIT ), 300 ) == Reason::R_WALL );		// from the walkway, fast enough to cross it

	// a closed end (NEXT_LINE -2, Altera and Besma): the unit cannot leave the ledge past it, though the floor is below
	Unit kLedge( State( 7450, 200, WAIT ) );
	CHECK( kLedge.Reason( State( 7540, 190, JUMP_DOWN, -200 ) ) == Reason::R_NONE );	// 40 past it
	CHECK( kLedge.Reason( State( 7580, 160, JUMP_DOWN, -400 ) ) == Reason::R_WALL );
	// its open start is a way down
	Unit kDown( State( 7050, 200, WAIT ) );
	CHECK( kDown.Reason( State( 6950, 180, JUMP_DOWN, -200 ) ) == Reason::R_NONE );

	// an LT_WALL line
	Unit kWall( State( 5960, 0, WAIT ) );
	CHECK( kWall.Reason( State( 6030, 0, DASH ) ) == Reason::R_WALL );
	CHECK( kWall.Reason( State( 6000, 0, DASH ) ) == Reason::R_WALL );		// touching it
	CHECK( kWall.Reason( State( 5990, 0, DASH ) ) == Reason::R_NONE );
	// above it
	Unit kOver( State( 5900, 400, JUMP_DOWN, 0 ) );
	CHECK( kOver.Reason( State( 5960, 395, JUMP_DOWN, -100 ) ) == Reason::R_NONE );
}

TEST_CASE( "KVillageMoveValidator: corrections, resets and operators" )
{
	Unit kUnit( State( 100, 0, WAIT ) );
	const KVillageMoveValidator::Verdict kFirst = kUnit.Send( State( 1000, 0, WALK ) );
	CHECK( kFirst.ucReason == Reason::R_SPEED );
	CHECK( kFirst.bNewViolation );
	// what the client sent before it applied the correction
	CHECK_FALSE( kUnit.Send( State( 1050, 0, WALK ) ).bNewViolation );
	CHECK_FALSE( kUnit.Send( State( 1100, 0, WALK ) ).bNewViolation );
	// still wrong a second later: a new violation
	CHECK( kUnit.Send( State( 2000, 0, WALK ), 1000 ).bNewViolation );

	// the server moves the unit: judged from there
	kUnit.kValidator.Reset( State( 2500, 0, WAIT ), kUnit.uiServer );
	CHECK( kUnit.Reason( State( 2520, 0, WALK ) ) == Reason::R_NONE );

	// X2_DEBUG operators run 6 times faster
	KVillageMoveValidator::Config kConfig;
	kConfig.fSpeedFactor = 6.f;
	Unit kOperator( State( 100, 0, WAIT ), kConfig );
	CHECK( kOperator.Reason( State( 500, 0, DASH ) ) == Reason::R_NONE );		// 4,000 u/s
}

TEST_CASE( "KVillageMoveValidator::MaxHeight follows the jump" )
{
	// JUMP_SPEED 1500, G_ACCEL 4000: apex 281.25 at 0.375 s
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 0.375f, 0 ) == doctest::Approx( 281.25f ) );
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 0.1f, 0 ) == doctest::Approx( 130.f ) );
	// falling: 0.5 s after the apex the unit reaches MAX_G_SPEED, then falls at 2000 u/s
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 0.875f, 0 ) == doctest::Approx( 281.25f - 500.f ) );
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 1.375f, 0 ) == doctest::Approx( 281.25f - 1500.f ) );
	// the takeoff may have been up to fWindow later than the last sample on the ground
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 1.f, 0.7f ) == doctest::Approx( 281.25f ) );
	CHECK( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 1.f, 0.1f ) == doctest::Approx( KVillageMoveValidator::MaxHeight( 1500, 4000, 2000, 0.9f, 0 ) ) );
}

#endif // X2_VILLAGE_SYNC
