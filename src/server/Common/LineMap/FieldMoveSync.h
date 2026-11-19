#pragma once

#ifdef X2_VILLAGE_SYNC

// The movement of the village units in one field or room (decision T53): a KVillageMoveValidator per unit, the last
// state the server accepted for each, and, once per tick (Config::uiTickMs), a batch for each viewer with the units
// that changed among those it sees. It does no I/O: the GameServer's KField (and later the CenterServer's rooms) send
// what it returns, and tell it which units each viewer sees (the sectors, the area of interest).

#include "LineMap/FieldUnitSnapshots.h"
#include "LineMap/VillageMoveValidator.h"

#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <vector>

class KFieldMoveSync
{
public:
	struct Config
	{
		KVillageMoveValidator::Config	kValidator;
		uint32_t	uiTickMs			= 100;		// a batch to each viewer at most this often
		float		fFastMoveFactor		= 6.f;		// X2_DEBUG operators holding left Shift (CX2SquareUnit)
		int			iKickViolations		= 20;		// this many violations within uiViolationWindowMs: kick; 0 never
		uint32_t	uiViolationWindowMs	= 60000;
	};

	struct Result
	{
		bool							bAccepted	= false;
		bool							bCorrect	= false;	// send kCorrection to the unit
		KEGS_FIELD_UNIT_CORRECTION_NOT	kCorrection;
		bool							bKick		= false;	// too many violations
		int								iViolations	= 0;		// within the window, this one included
	};

	using Destinations = std::function< void( UidType iUnitUID, std::vector< UidType >& vecViewer ) >;

	// pLineMap may be null (a world without one).
	KFieldMoveSync( const CXSLLineMap* pLineMap, const KSquareUnitPhysics& kPhysics, const Config& kConfig );

	// A unit enters at a position its client chose (EGS_FIELD_LOADING_COMPLETE_REQ): one with no line below is moved
	// to the nearest line, and bCorrect asks to tell the client. bFastMove: an X2_DEBUG operator.
	Result AddUnit( UidType iUnitUID, const VECTOR3& vPos, bool bFastMove, uint32_t uiServerNow );
	void RemoveUnit( UidType iUnitUID );
	bool HasUnit( UidType iUnitUID ) const { return m_mapUnit.count( iUnitUID ) > 0; }

	// A position the server sets: the next update is judged from it, and viewers get it in the next batch.
	void SetUnitState( UidType iUnitUID, const KFieldUnitState& kState, uint32_t uiServerNow );

	Result OnState( UidType iUnitUID, const KEGS_FIELD_UNIT_STATE_NOT& kNot, uint32_t uiServerNow );

	// iViewer starts to see iUnitUID (a sector change, entering the field): it gets its state in the next batch even
	// if it did not change.
	void ShowTo( UidType iViewer, UidType iUnitUID );
	void ShowAllTo( UidType iViewer );		// every unit here (entering a room where all see all)

	void SetKickViolations( int iViolations ) { m_kConfig.iKickViolations = iViolations; }

	bool IsTickDue( uint32_t uiServerNow ) const;

	// The batches of this tick, one per viewer, and the tick starts over. fnDestinations gives who sees a unit.
	void BuildBatches( uint32_t uiServerNow, const Destinations& fnDestinations, std::map< UidType, KEGS_FIELD_UNIT_STATE_BATCH_NOT >& mapBatch );

	// The same for a room where every unit sees every other (a square): kShared has every unit that changed, for all
	// (each client skips its own), and mapExtra, per viewer, the units it starts to see that kShared does not have.
	void BuildSharedBatch( uint32_t uiServerNow, KEGS_FIELD_UNIT_STATE_BATCH_NOT& kShared,
						   std::map< UidType, KEGS_FIELD_UNIT_STATE_BATCH_NOT >& mapExtra );

	const KFieldUnitState* GetState( UidType iUnitUID ) const;

	// The nearest point on a walkable line, or vPos without a line map or lines.
	static VECTOR3 NearestOnLine( const CXSLLineMap* pLineMap, const VECTOR3& vPos );

private:
	struct Unit
	{
		KVillageMoveValidator		kValidator;
		KFieldUnitState				kState;
		uint32_t					uiStateTick	= 0;		// server time the client made it (kClientClock)
		KFieldServerClock			kClientClock;			// the client's ticks on the server's clock
		bool						bChanged	= false;
		std::vector< uint32_t >		vecViolation;			// server times, within the window

		Unit( const CXSLLineMap* pLineMap, const KSquareUnitPhysics& kPhysics, const KVillageMoveValidator::Config& kConfig )
			: kValidator( pLineMap, kPhysics, kConfig ) {}
	};

	int CountViolation( Unit& kUnit, uint32_t uiServerNow ) const;
	static void AddEntry( KEGS_FIELD_UNIT_STATE_BATCH_NOT& kBatch, uint32_t uiServerNow, UidType iUnitUID, const Unit& kUnit );

	const CXSLLineMap*					m_pLineMap;
	const KSquareUnitPhysics&			m_kPhysics;
	Config								m_kConfig;
	std::map< UidType, Unit >			m_mapUnit;
	std::map< UidType, std::set< UidType > >	m_mapShow;			// viewer -> units to send whether changed or not
	bool								m_bTicked		= false;
	uint32_t							m_uiLastTick	= 0;
};

#endif // X2_VILLAGE_SYNC
