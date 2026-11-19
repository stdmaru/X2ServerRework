#pragma once

#ifdef X2_VILLAGE_SYNC

// Village and square movement sync on the client (decision T53), shared by CX2TFieldGame and CX2SquareGame: the own
// unit's state goes to the server about 10 times per second, the other units are drawn from the states the server
// accepted (KFieldUnitSnapshots, in CX2SquareUnit), and a correction puts the own unit back where the server says.

#include "LineMap/FieldUnitSnapshots.h"

#include <cstdint>
#include <functional>
#include <map>

class CX2SquareUnit;

class CX2UnitStateSync
{
public:
	// milliseconds, steady; GetTickCount steps by 10 to 16 ms on Windows
	static uint32_t GetClockMs();

	// the server's time at which the other units are drawn
	uint32_t GetRenderTick() const;

	// Every frame: sends the own unit's state at most every 0.1 s, only when it changed, and once after it stops.
	void FrameMove( float fElapsedTime, CX2SquareUnit* pMyUnit, bool bCanSend );

	// fnFindUnit gives the drawn unit of a UID, or NULL (not here, or still loading).
	void ReceiveBatch( const KEGS_FIELD_UNIT_STATE_BATCH_NOT& kBatch, UidType iMyUnitUID,
					   const std::function< CX2SquareUnit* ( UidType ) >& fnFindUnit );
	void ReceiveCorrection( CX2SquareUnit* pMyUnit, const KEGS_FIELD_UNIT_CORRECTION_NOT& kCorrection );

	// A unit was created (its models loaded): it gets the state that came meanwhile.
	void OnUnitAdded( CX2SquareUnit* pUnit, bool bMyUnit );
	void OnUnitRemoved( UidType iUnitUID ) { m_mapPendingState.erase( iUnitUID ); }

private:
	struct PendingState
	{
		uint32_t		uiTick;
		KFieldUnitState	kState;
	};

	KFieldServerClock					m_kServerClock;
	float								m_fSendTime		= 0.f;
	bool								m_bSentState	= false;	// m_kSentState holds what was sent last
	bool								m_bSentChange	= false;	// the last update was a change: send once more
	KFieldUnitState						m_kSentState;
	uint32_t							m_uiSentTick	= 0;
	std::map< UidType, PendingState >	m_mapPendingState;			// units still loading: their newest state
};

#endif // X2_VILLAGE_SYNC
