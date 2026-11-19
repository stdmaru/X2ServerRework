// KFieldMoveSync (decision T53): what a field sends about village movement. Accepted states reach the units' viewers
// once per tick, a unit newly in view is sent even if it did not move, rejected ones are corrected and counted, and
// too many violations kick.
#include "LineMap/FieldMoveSync.h"
#include "LineMap/SquareUnitPhysics.h"
#include "LineMap/WorldLineMaps.h"
#include "LineMap/XSLLineMap.h"

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <memory>

#ifdef X2_VILLAGE_SYNC

namespace fs = std::filesystem;
using Reason = KEGS_FIELD_UNIT_CORRECTION_NOT;

namespace
{

enum : u_char { WAIT = 1, WALK = 2 };

struct Fixture
{
	KSquareUnitPhysics				kPhysics;
	std::shared_ptr< CXSLLineMap >	spLineMap;
};

const Fixture& GetFixture()
{
	static std::unique_ptr< Fixture > s_spFixture;
	if( s_spFixture )
		return *s_spFixture;

	const fs::path root = fs::temp_directory_path() / "x2_field_move_sync_test";
	fs::remove_all( root );
	fs::create_directories( root );
	std::ofstream( root / "Enum.lua" ) << "FALSE = 0\nTRUE = 1\nLINE_TYPE = { LT_NORMAL = 0 }\n";
	std::ofstream( root / "SQUARE_Unit.lua" ) << R"(
INIT_PHYSIC = { RUN_SPEED = 700, RELOAD_ACCEL = 2000, DASH_JUMP_SPEED = 1700, WALK_SPEED = 500, G_ACCEL = 4000,
	MAX_G_SPEED = -2000, JUMP_SPEED = 1500 }
INIT_STATE = { "WAIT", "WALK" }
WAIT = { SPEED_X = 0, LAND_CONNECT = TRUE }
WALK = { LAND_CONNECT = TRUE, PASSIVE_SPEED_X = INIT_PHYSIC.WALK_SPEED }
)";
	std::ofstream( root / "Floor_LineMap.lua" ) << R"(
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 0, 0, 0 ), END_POS = D3DXVECTOR3( 3000, 0, 0 ) })
)";

	const fs::path oldCurrent = fs::current_path();
	fs::current_path( root );
	auto spFixture = std::make_unique< Fixture >();
	const bool bPhysics = spFixture->kPhysics.Load();
	spFixture->spLineMap = KWorldLineMaps::LoadLineMap( "Floor_LineMap.lua" );
	fs::current_path( oldCurrent );
	fs::remove_all( root );
	REQUIRE( bPhysics );
	REQUIRE( spFixture->spLineMap != nullptr );
	s_spFixture = std::move( spFixture );
	return *s_spFixture;
}

KEGS_FIELD_UNIT_STATE_NOT StateNot( uint32_t uiTick, float x, u_char ucStateID = WALK )
{
	KEGS_FIELD_UNIT_STATE_NOT kNot;
	kNot.m_uiTick = uiTick;
	kNot.m_kState.m_vPos = VECTOR3( x, 0.f, 0.f );
	kNot.m_kState.m_ucStateID = ucStateID;
	return kNot;
}

// everyone sees everyone
KFieldMoveSync::Destinations All( const std::vector< UidType >& vecUnit )
{
	return [vecUnit]( UidType, std::vector< UidType >& vecViewer ) { vecViewer = vecUnit; };
}

KFieldMoveSync MakeSync( const KFieldMoveSync::Config& kConfig = KFieldMoveSync::Config() )
{
	return KFieldMoveSync( GetFixture().spLineMap.get(), GetFixture().kPhysics, kConfig );
}

}	// namespace

TEST_CASE( "KFieldMoveSync: entering" )
{
	KFieldMoveSync kSync = MakeSync();
	CHECK( kSync.AddUnit( 1, VECTOR3( 100, 0, 0 ), false, 1000 ).bAccepted );
	CHECK( kSync.AddUnit( 2, VECTOR3( 200, 400, 0 ), false, 1000 ).bAccepted );	// above the floor: it falls

	// a client entering where there is nothing is put on the nearest line
	const KFieldMoveSync::Result kOutside = kSync.AddUnit( 3, VECTOR3( 9000, 500, 0 ), false, 1000 );
	CHECK_FALSE( kOutside.bAccepted );
	CHECK( kOutside.bCorrect );
	CHECK( kOutside.kCorrection.m_ucReason == Reason::R_OFF_LINE );
	CHECK( kOutside.kCorrection.m_kState.m_vPos.x == doctest::Approx( 3000.f ) );
	CHECK( kOutside.kCorrection.m_kState.m_vPos.y == doctest::Approx( 0.f ) );
	REQUIRE( kSync.GetState( 3 ) != nullptr );
	CHECK( kSync.GetState( 3 )->m_vPos.x == doctest::Approx( 3000.f ) );

	kSync.RemoveUnit( 3 );
	CHECK_FALSE( kSync.HasUnit( 3 ) );
	CHECK( kSync.GetState( 3 ) == nullptr );
}

TEST_CASE( "KFieldMoveSync: batches" )
{
	KFieldMoveSync kSync = MakeSync();
	kSync.AddUnit( 1, VECTOR3( 100, 0, 0 ), false, 1000 );
	kSync.AddUnit( 2, VECTOR3( 500, 0, 0 ), false, 1000 );

	std::map< UidType, KEGS_FIELD_UNIT_STATE_BATCH_NOT > mapBatch;
	// the first tick sends who entered; 99 is a viewer the sync does not know (still loading): nothing for it
	CHECK( kSync.IsTickDue( 1000 ) );
	kSync.BuildBatches( 1000, All( { 1, 2, 99 } ), mapBatch );
	REQUIRE( mapBatch.size() == 2 );
	REQUIRE( mapBatch[ 1 ].m_vecUnit.size() == 1 );
	CHECK( mapBatch[ 1 ].m_vecUnit[ 0 ].m_UnitUID == 2 );		// never itself
	CHECK( mapBatch[ 1 ].m_uiTick == 1000u );

	CHECK_FALSE( kSync.IsTickDue( 1050 ) );
	CHECK( kSync.IsTickDue( 1100 ) );

	// nothing changed: nothing to send
	kSync.BuildBatches( 1100, All( { 1, 2 } ), mapBatch );
	CHECK( mapBatch.empty() );

	// unit 1 walks: unit 2 gets it, with the time the server accepted it
	CHECK( kSync.OnState( 1, StateNot( 50000, 140 ), 1150 ).bAccepted );
	CHECK( kSync.OnState( 1, StateNot( 50050, 165 ), 1180 ).bAccepted );
	kSync.BuildBatches( 1200, All( { 1, 2 } ), mapBatch );
	REQUIRE( mapBatch.size() == 1 );
	REQUIRE( mapBatch[ 2 ].m_vecUnit.size() == 1 );		// the latest state only
	CHECK( mapBatch[ 2 ].m_vecUnit[ 0 ].m_kState.m_vPos.x == doctest::Approx( 165.f ) );
	CHECK( mapBatch[ 2 ].m_vecUnit[ 0 ].m_uiTick == 1180u );

	// a unit coming into view is sent once, though it did not move
	kSync.ShowTo( 2, 1 );
	kSync.ShowTo( 1, 1 );		// not to itself
	kSync.BuildBatches( 1300, All( { 1, 2 } ), mapBatch );
	REQUIRE( mapBatch.size() == 1 );
	REQUIRE( mapBatch[ 2 ].m_vecUnit.size() == 1 );
	CHECK( mapBatch[ 2 ].m_vecUnit[ 0 ].m_UnitUID == 1 );
	kSync.BuildBatches( 1400, All( { 1, 2 } ), mapBatch );
	CHECK( mapBatch.empty() );

	// only to the viewers the field names
	CHECK( kSync.OnState( 2, StateNot( 70000, 520 ), 1450 ).bAccepted );
	kSync.BuildBatches( 1500, []( UidType, std::vector< UidType >& vecViewer ) { vecViewer.clear(); }, mapBatch );
	CHECK( mapBatch.empty() );

	// the server moves a unit: its viewers get it and it is judged from there
	KFieldUnitState kMoved;
	kMoved.m_vPos = VECTOR3( 2500, 0, 0 );
	kMoved.m_ucStateID = WAIT;
	kSync.SetUnitState( 2, kMoved, 1550 );
	kSync.BuildBatches( 1600, All( { 1, 2 } ), mapBatch );
	REQUIRE( mapBatch[ 1 ].m_vecUnit.size() == 1 );
	CHECK( mapBatch[ 1 ].m_vecUnit[ 0 ].m_kState.m_vPos.x == doctest::Approx( 2500.f ) );
	CHECK( kSync.OnState( 2, StateNot( 70100, 2540 ), 1650 ).bAccepted );
}

TEST_CASE( "KFieldMoveSync: corrections and kicks" )
{
	KFieldMoveSync::Config kConfig;
	kConfig.iKickViolations = 3;
	KFieldMoveSync kSync = MakeSync( kConfig );
	kSync.AddUnit( 1, VECTOR3( 100, 0, 0 ), false, 1000 );
	kSync.AddUnit( 2, VECTOR3( 500, 0, 0 ), false, 1000 );
	std::map< UidType, KEGS_FIELD_UNIT_STATE_BATCH_NOT > mapBatch;
	kSync.BuildBatches( 1000, All( { 1, 2 } ), mapBatch );

	// too fast: corrected to the last valid state, and the viewers get nothing
	const KFieldMoveSync::Result kFirst = kSync.OnState( 1, StateNot( 50000, 2000 ), 1100 );
	CHECK_FALSE( kFirst.bAccepted );
	CHECK( kFirst.bCorrect );
	CHECK( kFirst.kCorrection.m_ucReason == Reason::R_SPEED );
	CHECK( kFirst.kCorrection.m_uiTick == 50000u );
	CHECK( kFirst.kCorrection.m_kState.m_vPos.x == doctest::Approx( 100.f ) );
	CHECK( kFirst.iViolations == 1 );
	CHECK_FALSE( kFirst.bKick );
	kSync.BuildBatches( 1200, All( { 1, 2 } ), mapBatch );
	CHECK( mapBatch.empty() );

	// what the client sent before it applied the correction: dropped quietly
	const KFieldMoveSync::Result kLate = kSync.OnState( 1, StateNot( 50100, 2050 ), 1200 );
	CHECK_FALSE( kLate.bAccepted );
	CHECK_FALSE( kLate.bCorrect );

	CHECK( kSync.OnState( 1, StateNot( 52000, 9000 ), 3100 ).iViolations == 2 );
	const KFieldMoveSync::Result kThird = kSync.OnState( 1, StateNot( 54000, 9000 ), 5100 );
	CHECK( kThird.iViolations == 3 );
	CHECK( kThird.bKick );

	// violations older than the window are forgotten
	kConfig.uiViolationWindowMs = 3000;
	KFieldMoveSync kForgiving = MakeSync( kConfig );
	kForgiving.AddUnit( 1, VECTOR3( 100, 0, 0 ), false, 1000 );
	CHECK( kForgiving.OnState( 1, StateNot( 50000, 2000 ), 1100 ).iViolations == 1 );
	CHECK( kForgiving.OnState( 1, StateNot( 52000, 9000 ), 3100 ).iViolations == 2 );
	CHECK( kForgiving.OnState( 1, StateNot( 56000, 9000 ), 7100 ).iViolations == 1 );
}

TEST_CASE( "KFieldMoveSync: X2_DEBUG operators move faster" )
{
	KFieldMoveSync kSync = MakeSync();
	kSync.AddUnit( 1, VECTOR3( 100, 0, 0 ), true, 1000 );
	kSync.AddUnit( 2, VECTOR3( 100, 0, 0 ), false, 1000 );
	CHECK( kSync.OnState( 1, StateNot( 50000, 500 ), 1100 ).bAccepted );		// 4,000 u/s
	CHECK_FALSE( kSync.OnState( 2, StateNot( 50000, 500 ), 1100 ).bAccepted );
}

#endif // X2_VILLAGE_SYNC
