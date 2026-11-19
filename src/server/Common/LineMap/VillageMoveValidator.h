#pragma once

#ifdef X2_VILLAGE_SYNC

// Checks the movement a client reports for its village unit (decision T53), one validator per unit. It knows only the
// world's line map, how the unit script lets a unit move (KSquareUnitPhysics) and the last state it accepted; it does
// no I/O, so the GameServer and the CenterServer decide what to send and log.
//
// Time is the client's tick (EGS_FIELD_UNIT_STATE_NOT::m_uiTick, ms), so updates that TCP delivers in a bunch are
// judged by when they were made; the client's clock may not run ahead of the server's by more than
// Config::uiMaxClockLeadMs, which keeps a fast clock from buying distance. An update is rejected when:
//   R_RATE      more updates than the token bucket allows, or a tick that does not advance;
//   R_STATE     a state the unit script does not have;
//   R_SPEED     it moved farther horizontally than the fastest speed allows in the time (the line's own speed and a
//               jump line's launch count), or its clock ran ahead;
//   R_OFF_LINE  it is away from every line in a state that keeps it on one (LAND_CONNECT), unless a jump line threw it;
//   R_RISE      it is higher above the ground it left than a jump can carry it at that time (a unit in the air that
//               now rises may have landed and jumped again between two updates: on the highest line below it that the
//               last update's height and vertical speed bring it down onto within the time);
//   R_WALL      it went where CX2SquareUnit stops a unit: over no line (the unit cannot move where it has no line
//               below, on the ground or in the air: the edges of Elder), past an end of the line it last stood on
//               that is closed (BEFORE_LINE/NEXT_LINE -2: Altera, Besma), or across an LT_WALL line (none in villages
//               and rooms, 2026-11-19). The path between two updates is sampled, so a narrow gap cannot be skipped.

#include "ClientPacket.h"

#include <array>
#include <cstdint>
#include <string>

class CXSLLineMap;
class KSquareUnitPhysics;

class KVillageMoveValidator
{
public:
	struct Config
	{
		float		fSpeedTolerance		= 1.2f;		// times the fastest speed
		float		fDistanceSlack		= 50.f;		// units, per update
		float		fLineRadius			= 40.f;		// on a line within this distance (the client uses 30)
		float		fHeightSlack		= 60.f;		// units above the jump's curve
		float		fWallRadius			= 60.f;		// a line below within this distance horizontally; the path between
												// two updates is a chord of the real one (corners of line chains)
		float		fSpeedFactor		= 1.f;		// X2_DEBUG operators run 6 times faster
		uint32_t	uiMaxClockLeadMs	= 500;
		uint32_t	uiMaxClockLagMs		= 2000;		// a client this far behind (late packets) is caught up
		float		fUpdatesPerSecond	= 20.f;
		float		fUpdateBurst		= 10.f;
		uint32_t	uiCorrectionGraceMs	= 1000;		// rejections this soon after a correction are not new
		uint32_t	uiLandingSlackMs	= 30;		// a landing between two updates may come this much later
	};

	struct Verdict
	{
		u_char	ucReason		= KEGS_FIELD_UNIT_CORRECTION_NOT::R_NONE;
		// A rejection outside the grace after the last correction: send a correction and count a violation. Other
		// rejections are updates the client made before it applied the correction, and are dropped.
		bool	bNewViolation	= false;

		bool IsValid() const { return ucReason == KEGS_FIELD_UNIT_CORRECTION_NOT::R_NONE; }
	};

	// pLineMap may be null (a world without one): then only the state, the rate and the speed are checked.
	KVillageMoveValidator( const CXSLLineMap* pLineMap, const KSquareUnitPhysics& kPhysics, const Config& kConfig );

	// A position the server sets (entering the world, fast travel, a teleport): the next update is judged from it.
	void Reset( const KFieldUnitState& kState, uint32_t uiServerNow );

	Verdict Check( uint32_t uiClientTick, const KFieldUnitState& kState, uint32_t uiServerNow );

	const KFieldUnitState& GetLastValid() const { return m_kLast; }

	// The last updates checked, oldest first, one per line, with the ground each was judged from: logged with a
	// rejection, so a rollback in game can be traced to the update that misled the validator.
	std::wstring DescribeHistory() const;

	// The highest a unit launched at fSpeedY rises above the ground after fTime seconds, when it left the ground
	// at any moment of the fWindow seconds before (the last sample on the ground and the first in the air).
	static float MaxHeight( float fSpeedY, float fGAccel, float fMaxFallSpeed, float fTime, float fWindow );

	// The nearest walkable line within fRadius of vPos, or -1.
	static int FindLine( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius );

	// Whether a walkable line is at or below vPos within fRadius horizontally (CKTDGLineMap::CanDown).
	static bool HasLineBelow( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius );

	// The highest walkable line under vFrom or vTo (within fRadius horizontally) that a unit at vFrom moving up at
	// fSpeedY (negative: down) lands on within fMaxTime seconds, and its height (fY); -1 if none. The unit lands once
	// it is not rising and within fCatch of the line (CX2SquareUnit: LINE_RADIUS, 30).
	static int FindLanding( const CXSLLineMap& kLineMap, const VECTOR3& vFrom, const VECTOR3& vTo, float fSpeedY, float fGAccel,
							float fMaxTime, float fRadius, float fCatch, float& fY );

	// The highest such line and its height (fY) below vPos, or -1.
	static int FindGroundBelow( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius, float& fY );

	// Whether the move from vFrom to vTo is one CX2SquareUnit stops: see R_WALL. iLastLine is the line the unit last
	// stood on (-1 none) and bOnLine whether vTo is on a line.
	static bool HitsWall( const CXSLLineMap& kLineMap, const VECTOR3& vFrom, const VECTOR3& vTo, int iLastLine, bool bOnLine,
						  float fRadius );

private:
	struct Sample
	{
		uint32_t	uiTick			= 0;
		KFieldUnitState	kState;
		u_char		ucReason		= 0;
		bool		bAir			= false;	// as judged (after a landing between updates was found)
		float		fGroundY		= 0.f;
		int			iGroundLine		= -1;
		uint32_t	uiGroundTick	= 0;
		float		fLaunchSpeedY	= 0.f;
		float		fMaxHeight		= 0.f;		// above the ground, R_RISE's limit before the slack; 0 not checked
	};

	Verdict Reject( u_char ucReason, uint32_t uiServerNow );
	void Record( uint32_t uiTick, const KFieldUnitState& kState, u_char ucReason, bool bAir, float fGroundY, int iGroundLine,
				 uint32_t uiGroundTick, float fLaunchSpeedY, float fMaxHeight );
	void SetGround( const VECTOR3& vPos, int iLine, uint32_t uiTick );

	const CXSLLineMap*			m_pLineMap;
	const KSquareUnitPhysics&	m_kPhysics;
	Config						m_kConfig;

	KFieldUnitState	m_kLast;
	uint32_t		m_uiLastTick		= 0;
	uint32_t		m_uiLastServer		= 0;
	bool			m_bHasClientClock	= false;
	uint32_t		m_uiBaseTick		= 0;		// client tick and server time at the same moment
	uint32_t		m_uiBaseServer		= 0;

	float			m_fTokens			= 0.f;
	uint32_t		m_uiTokenServer		= 0;
	bool			m_bCorrected		= false;
	uint32_t		m_uiCorrectionServer = 0;

	bool			m_bAir				= false;
	float			m_fGroundY			= 0.f;
	int				m_iGroundLine		= -1;
	uint32_t		m_uiGroundTick		= 0;		// last sample on the ground
	uint32_t		m_uiTakeoffWindowMs	= 0;
	float			m_fLaunchSpeedX		= 0.f;
	float			m_fLaunchSpeedY		= 0.f;
	bool			m_bThrownByLine		= false;

	std::array< Sample, 12 >	m_arHistory;
	size_t			m_uiHistoryNext		= 0;
	size_t			m_uiHistorySize		= 0;
	// the update being checked, for Reject to record
	uint32_t		m_uiCheckTick		= 0;
	KFieldUnitState	m_kCheckState;
	bool			m_bCheckRecorded	= false;
};

#endif // X2_VILLAGE_SYNC
