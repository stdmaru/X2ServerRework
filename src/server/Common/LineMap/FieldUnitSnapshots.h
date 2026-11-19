#pragma once

#ifdef X2_VILLAGE_SYNC

// How the client draws the other village units (decision T53): the states the server accepted
// (EGS_FIELD_UNIT_STATE_BATCH_NOT) are kept per unit and drawn a little in the past, interpolating between the two
// around that moment, so a unit moves smoothly however the packets arrive. Header only and free of the client's
// classes, so the client includes it and the server's tests check it.
//
// Time is the server's clock (ms): every entry carries when the server accepted it and every batch when the server
// sent it. KFieldServerClock maps the client's clock to the server's; the client draws at its estimate of the server's
// time minus Config::uiDelayMs.

#include "ClientPacket.h"

#include <cmath>
#include <cstdint>
#include <deque>

// The server's clock seen from the client (and, on the server, the client's clock seen from the server: OnBatch with
// the client's tick and the server's time). A batch's tick, less the client's time when it arrives, is the offset
// between the clocks minus the batch's delay; the largest offset seen is the one with the least delay, so it is kept,
// and it follows smaller offsets by 1 ms per batch, in case the client's clock runs faster than the server's. Faster
// than that, the time drawn would jump ahead at every batch that came quickly and slow down between them.
class KFieldServerClock
{
public:
	void OnBatch( uint32_t uiServerTick, uint32_t uiLocalMs )
	{
		const int32_t iOffset = static_cast< int32_t >( uiServerTick - uiLocalMs );
		if( !m_bHasOffset )
		{
			m_iOffset		= iOffset;
			m_bHasOffset	= true;
			return;
		}
		if( iOffset > m_iOffset )
			m_iOffset = iOffset;
		else if( iOffset < m_iOffset )
			--m_iOffset;
	}

	bool HasOffset() const { return m_bHasOffset; }
	uint32_t ToServer( uint32_t uiLocalMs ) const { return uiLocalMs + static_cast< uint32_t >( m_iOffset ); }
	// the local time of a server tick: when it would have arrived with the least delay
	uint32_t ToLocal( uint32_t uiServerTick ) const { return uiServerTick - static_cast< uint32_t >( m_iOffset ); }

private:
	bool	m_bHasOffset	= false;
	int32_t	m_iOffset		= 0;
};

// The states of one remote unit, oldest first.
class KFieldUnitSnapshots
{
public:
	struct Config
	{
		// The server sends a unit's newest state once per tick (100 ms) and the client updates it at that rate, so
		// the newest state is up to two ticks old when it arrives: drawing that far behind leaves a later state to
		// interpolate toward nearly always.
		uint32_t	uiDelayMs			= 200;
		uint32_t	uiTickMs			= 100;		// the sender's rate (KFieldMoveSync::Config::uiTickMs)
		uint32_t	uiMaxExtrapolateMs	= 150;		// a late state: go on moving this long at most
		float		fSnapDistance		= 300.f;	// farther apart than this in one step: a jump, not a walk
		size_t		uiMaxSnapshots		= 32;
	};

	struct Pose
	{
		VECTOR3			vPos;
		KFieldUnitState	kState;		// the newest state at or before the moment drawn (the first one before it)
	};

	KFieldUnitSnapshots() = default;
	explicit KFieldUnitSnapshots( const Config& kConfig ) : m_kConfig( kConfig ) {}

	const Config& GetConfig() const { return m_kConfig; }
	bool IsEmpty() const { return m_deqSnapshot.empty(); }
	size_t GetSize() const { return m_deqSnapshot.size(); }
	void Clear() { m_deqSnapshot.clear(); }

	// A state the server accepted at uiServerTick; one not newer than the newest is ignored.
	void Push( uint32_t uiServerTick, const KFieldUnitState& kState )
	{
		if( !m_deqSnapshot.empty() && static_cast< int32_t >( uiServerTick - m_deqSnapshot.back().uiTick ) <= 0 )
			return;
		m_deqSnapshot.push_back( Snapshot{ uiServerTick, kState } );
		while( m_deqSnapshot.size() > m_kConfig.uiMaxSnapshots )
			m_deqSnapshot.pop_front();
	}

	// The unit at uiRenderTick (server time, already less the delay). False with no state yet.
	bool Sample( uint32_t uiRenderTick, Pose& kPose ) const
	{
		if( m_deqSnapshot.empty() )
			return false;

		const Snapshot& kFirst = m_deqSnapshot.front();
		if( Before( uiRenderTick, kFirst.uiTick ) )
		{
			kPose.vPos		= kFirst.kState.m_vPos;
			kPose.kState	= kFirst.kState;
			return true;
		}

		for( size_t i = 0; i + 1 < m_deqSnapshot.size(); ++i )
		{
			const Snapshot& a = m_deqSnapshot[ i ];
			const Snapshot& b = m_deqSnapshot[ i + 1 ];
			if( Before( uiRenderTick, b.uiTick ) )
			{
				kPose.kState	= a.kState;
				kPose.vPos		= Between( a, b, uiRenderTick );
				return true;
			}
		}

		// past the newest state: a unit that was moving goes on for a while, one that stopped stays
		const Snapshot& kLast = m_deqSnapshot.back();
		kPose.kState	= kLast.kState;
		kPose.vPos		= kLast.kState.m_vPos;
		if( m_deqSnapshot.size() >= 2 )
		{
			const Snapshot& kPrev = m_deqSnapshot[ m_deqSnapshot.size() - 2 ];
			const uint32_t uiGap = kLast.uiTick - kPrev.uiTick;
			if( uiGap > 0 && ( uiGap <= HoldMs() || IsMoving( kPrev.kState ) ) && Distance( kPrev.kState.m_vPos, kLast.kState.m_vPos ) <= m_kConfig.fSnapDistance )
			{
				uint32_t uiAhead = uiRenderTick - kLast.uiTick;
				if( uiAhead > m_kConfig.uiMaxExtrapolateMs )
					uiAhead = m_kConfig.uiMaxExtrapolateMs;
				const float t = static_cast< float >( uiAhead ) / static_cast< float >( uiGap );
				kPose.vPos = Lerp( kPrev.kState.m_vPos, kLast.kState.m_vPos, 1.f + t );
			}
		}
		return true;
	}

	// Forgets the states no longer needed to draw uiRenderTick and later: all but the last one at or before it.
	void Prune( uint32_t uiRenderTick )
	{
		while( m_deqSnapshot.size() >= 2 && !Before( uiRenderTick, m_deqSnapshot[ 1 ].uiTick ) )
			m_deqSnapshot.pop_front();
	}

	static float Distance( const VECTOR3& a, const VECTOR3& b )
	{
		const float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z;
		return std::sqrt( x * x + y * y + z * z );
	}

private:
	struct Snapshot
	{
		uint32_t		uiTick;
		KFieldUnitState	kState;
	};

	static bool IsMoving( const KFieldUnitState& kState ) { return kState.m_sSpeedX != 0 || kState.m_sSpeedY != 0; }

	// a state that comes this long after a standing one: the unit stood still between them
	uint32_t HoldMs() const { return m_kConfig.uiTickMs + m_kConfig.uiTickMs / 2; }

	static bool Before( uint32_t uiTick, uint32_t uiOther ) { return static_cast< int32_t >( uiTick - uiOther ) < 0; }

	static VECTOR3 Lerp( const VECTOR3& a, const VECTOR3& b, float t )
	{
		return VECTOR3( a.x + ( b.x - a.x ) * t, a.y + ( b.y - a.y ) * t, a.z + ( b.z - a.z ) * t );
	}

	VECTOR3 Between( const Snapshot& a, const Snapshot& b, uint32_t uiRenderTick ) const
	{
		// a teleport (fast travel, a correction): no sliding across the map
		if( Distance( a.kState.m_vPos, b.kState.m_vPos ) > m_kConfig.fSnapDistance )
			return a.kState.m_vPos;

		// the server sends nothing while a unit stands, so a long gap after a standing state is standing still and then
		// one tick of moving; after a moving one it is a state that did not reach us (two in one batch), and the unit
		// went on moving
		uint32_t uiStart = a.uiTick;
		if( b.uiTick - a.uiTick > HoldMs() && !IsMoving( a.kState ) )
			uiStart = b.uiTick - m_kConfig.uiTickMs;
		if( Before( uiRenderTick, uiStart ) )
			return a.kState.m_vPos;
		const float t = static_cast< float >( uiRenderTick - uiStart ) / static_cast< float >( b.uiTick - uiStart );
		return Lerp( a.kState.m_vPos, b.kState.m_vPos, t );
	}

	Config					m_kConfig;
	std::deque< Snapshot >	m_deqSnapshot;
};

#endif // X2_VILLAGE_SYNC
