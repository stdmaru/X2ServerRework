// KFieldServerClock and KFieldUnitSnapshots (decision T53): how the client draws the other village units, in the past,
// between the states the server accepted.
#include "LineMap/FieldUnitSnapshots.h"

#include <doctest/doctest.h>

#ifdef X2_VILLAGE_SYNC

namespace
{

KFieldUnitState At( float x, u_char ucStateID = 2 )
{
	KFieldUnitState kState;
	kState.m_vPos = VECTOR3( x, 0.f, 0.f );
	kState.m_ucStateID = ucStateID;
	return kState;
}

float X( const KFieldUnitSnapshots& kSnapshots, uint32_t uiTick )
{
	KFieldUnitSnapshots::Pose kPose;
	REQUIRE( kSnapshots.Sample( uiTick, kPose ) );
	return kPose.vPos.x;
}

}	// namespace

TEST_CASE( "KFieldServerClock" )
{
	KFieldServerClock kClock;
	CHECK_FALSE( kClock.HasOffset() );
	kClock.OnBatch( 50000, 1000 );
	REQUIRE( kClock.HasOffset() );
	CHECK( kClock.ToServer( 1100 ) == 50100u );

	// a batch that took longer moves it by 1 ms; one that took less is taken at once
	kClock.OnBatch( 50100, 1180 );
	CHECK( kClock.ToServer( 1200 ) == 50199u );
	CHECK( kClock.ToLocal( 50199 ) == 1200u );
	kClock.OnBatch( 50200, 1150 );
	CHECK( kClock.ToServer( 1200 ) == 50250u );

	// either clock may wrap
	KFieldServerClock kWrap;
	kWrap.OnBatch( 10, 0xFFFFFFF0u );
	CHECK( kWrap.ToServer( 0xFFFFFFF0u ) == 10u );
	CHECK( kWrap.ToServer( 0x10u ) == 42u );
}

TEST_CASE( "KFieldUnitSnapshots: interpolation" )
{
	KFieldUnitSnapshots kSnapshots;
	KFieldUnitSnapshots::Pose kPose;
	CHECK_FALSE( kSnapshots.Sample( 1000, kPose ) );

	kSnapshots.Push( 1000, At( 100 ) );
	CHECK( X( kSnapshots, 900 ) == doctest::Approx( 100.f ) );		// before the first: the first
	CHECK( X( kSnapshots, 1500 ) == doctest::Approx( 100.f ) );		// a single state: it stays

	kSnapshots.Push( 1100, At( 150 ) );
	kSnapshots.Push( 1200, At( 200, 5 ) );
	kSnapshots.Push( 1150, At( 999 ) );								// older than the newest: ignored
	CHECK( kSnapshots.GetSize() == 3 );
	CHECK( X( kSnapshots, 1050 ) == doctest::Approx( 125.f ) );
	CHECK( X( kSnapshots, 1175 ) == doctest::Approx( 187.5f ) );
	REQUIRE( kSnapshots.Sample( 1175, kPose ) );
	CHECK( kPose.kState.m_ucStateID == 2 );							// the state reached so far
	REQUIRE( kSnapshots.Sample( 1200, kPose ) );
	CHECK( kPose.kState.m_ucStateID == 5 );

	// late: a moving unit goes on, for a while
	CHECK( X( kSnapshots, 1250 ) == doctest::Approx( 225.f ) );
	CHECK( X( kSnapshots, 1500 ) == doctest::Approx( 275.f ) );		// 150 ms at most

	// it stopped: the sender's last update repeats the position
	kSnapshots.Push( 1300, At( 210 ) );
	kSnapshots.Push( 1400, At( 210 ) );
	CHECK( X( kSnapshots, 1600 ) == doctest::Approx( 210.f ) );

	kSnapshots.Prune( 1350 );
	CHECK( kSnapshots.GetSize() == 2 );
	CHECK( X( kSnapshots, 1350 ) == doctest::Approx( 210.f ) );
}

TEST_CASE( "KFieldUnitSnapshots: standing, teleports and limits" )
{
	KFieldUnitSnapshots kSnapshots;

	// stood at 100 from 1000, then a tick of walking reached 150 at 5000: it starts moving at 4900
	kSnapshots.Push( 1000, At( 100 ) );
	kSnapshots.Push( 5000, At( 150 ) );
	CHECK( X( kSnapshots, 3000 ) == doctest::Approx( 100.f ) );
	CHECK( X( kSnapshots, 4900 ) == doctest::Approx( 100.f ) );
	CHECK( X( kSnapshots, 4950 ) == doctest::Approx( 125.f ) );
	// nor does it go on past a state that came after a stand
	CHECK( X( kSnapshots, 5100 ) == doctest::Approx( 150.f ) );

	// far in one step: it jumps there instead of sliding
	kSnapshots.Push( 5100, At( 2000 ) );
	CHECK( X( kSnapshots, 5050 ) == doctest::Approx( 150.f ) );
	CHECK( X( kSnapshots, 5100 ) == doctest::Approx( 2000.f ) );
	CHECK( X( kSnapshots, 5200 ) == doctest::Approx( 2000.f ) );

	KFieldUnitSnapshots::Config kConfig;
	kConfig.uiMaxSnapshots = 4;
	KFieldUnitSnapshots kFew( kConfig );
	for( uint32_t i = 0; i < 10; ++i )
		kFew.Push( 1000 + i * 100, At( static_cast< float >( i ) ) );
	CHECK( kFew.GetSize() == 4 );
	CHECK( X( kFew, 0 ) == doctest::Approx( 6.f ) );
}

#endif // X2_VILLAGE_SYNC
