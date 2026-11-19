#include "BaseServer.h"

#ifdef X2_VILLAGE_SYNC

#include "LineMap/VillageMoveValidator.h"
#include "LineMap/SquareUnitPhysics.h"
#include "LineMap/XSLLineMap.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace
{

using Reason = KEGS_FIELD_UNIT_CORRECTION_NOT;

float DistanceToSegment( const VECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b )
{
	const float abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
	const float apx = p.x - a.x, apy = p.y - a.y, apz = p.z - a.z;
	const float fLength2 = abx * abx + aby * aby + abz * abz;
	float t = fLength2 > 0.f ? ( apx * abx + apy * aby + apz * abz ) / fLength2 : 0.f;
	t = std::clamp( t, 0.f, 1.f );
	const float dx = apx - abx * t, dy = apy - aby * t, dz = apz - abz * t;
	return std::sqrt( dx * dx + dy * dy + dz * dz );
}

bool IsWalkable( const CXSLLineMap::LineData& kLine )
{
	return kLine.bEnable && kLine.lineType != CXSLLineMap::LT_WALL && kLine.lineType != CXSLLineMap::LT_MONSTER_ROAD;
}

// Where the segment a-b is nearest to p horizontally, from 0 (a) to 1 (b), unclamped; 0 for a vertical segment.
float HorizontalParameter( const VECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b )
{
	const float abx = b.x - a.x, abz = b.z - a.z;
	const float fLength2 = abx * abx + abz * abz;
	return fLength2 > 0.f ? ( ( p.x - a.x ) * abx + ( p.z - a.z ) * abz ) / fLength2 : 0.f;
}

// The distance between the segments p1-p2 and q1-q2 (CKTDXCollision::LineToLine, which the client tests against
// walls with a radius of 1).
float SegmentDistance( const VECTOR3& p1, const VECTOR3& p2, const D3DXVECTOR3& q1, const D3DXVECTOR3& q2 )
{
	const D3DXVECTOR3 a( p1.x, p1.y, p1.z ), b( p2.x, p2.y, p2.z );
	const D3DXVECTOR3 d1 = b - a, d2 = q2 - q1, r = a - q1;
	const auto Dot = []( const D3DXVECTOR3& u, const D3DXVECTOR3& v ) { return u.x * v.x + u.y * v.y + u.z * v.z; };
	const float aa = Dot( d1, d1 ), ee = Dot( d2, d2 ), ff = Dot( d2, r );
	float s = 0.f, t = 0.f;
	if( aa <= 1e-6f && ee <= 1e-6f )
		;
	else if( aa <= 1e-6f )
		t = std::clamp( ff / ee, 0.f, 1.f );
	else
	{
		const float cc = Dot( d1, r );
		if( ee <= 1e-6f )
			s = std::clamp( -cc / aa, 0.f, 1.f );
		else
		{
			const float bb = Dot( d1, d2 ), fDenominator = aa * ee - bb * bb;
			s = fDenominator > 1e-6f ? std::clamp( ( bb * ff - cc * ee ) / fDenominator, 0.f, 1.f ) : 0.f;
			t = ( bb * s + ff ) / ee;
			if( t < 0.f )
			{
				t = 0.f;
				s = std::clamp( -cc / aa, 0.f, 1.f );
			}
			else if( t > 1.f )
			{
				t = 1.f;
				s = std::clamp( ( bb - cc ) / aa, 0.f, 1.f );
			}
		}
	}
	const D3DXVECTOR3 vDelta = ( a + d1 * s ) - ( q1 + d2 * t );
	return std::sqrt( Dot( vDelta, vDelta ) );
}

}	// namespace

KVillageMoveValidator::KVillageMoveValidator( const CXSLLineMap* pLineMap, const KSquareUnitPhysics& kPhysics, const Config& kConfig )
	: m_pLineMap( pLineMap )
	, m_kPhysics( kPhysics )
	, m_kConfig( kConfig )
{
	Reset( KFieldUnitState(), 0 );
}

void KVillageMoveValidator::Reset( const KFieldUnitState& kState, uint32_t uiServerNow )
{
	m_kLast				= kState;
	m_uiLastServer		= uiServerNow;
	m_bHasClientClock	= false;
	m_fTokens			= m_kConfig.fUpdateBurst;
	m_uiTokenServer		= uiServerNow;
	m_bCorrected		= false;
	m_bAir				= false;
	m_bThrownByLine		= false;
	m_fGroundY			= kState.m_vPos.y;
	m_iGroundLine		= m_pLineMap != nullptr ? FindLine( *m_pLineMap, kState.m_vPos, m_kConfig.fLineRadius ) : -1;
	m_uiTakeoffWindowMs	= 0;
	m_fLaunchSpeedX		= 0.f;
	m_fLaunchSpeedY		= 0.f;
}

KVillageMoveValidator::Verdict KVillageMoveValidator::Check( uint32_t uiClientTick, const KFieldUnitState& kState, uint32_t uiServerNow )
{
	// rate: a token bucket on the server's clock
	m_fTokens = std::min( m_kConfig.fUpdateBurst,
						  m_fTokens + static_cast<float>( uiServerNow - m_uiTokenServer ) / 1000.f * m_kConfig.fUpdatesPerSecond );
	m_uiTokenServer = uiServerNow;
	if( m_fTokens < 1.f )
		return Reject( Reason::R_RATE, uiServerNow );
	m_fTokens -= 1.f;

	const KSquareUnitPhysics::State* pState = m_kPhysics.GetState( kState.m_ucStateID );
	if( pState == nullptr )
		return Reject( Reason::R_STATE, uiServerNow );

	// the client's clock: it must advance, and not run ahead of the server's
	uint32_t uiLastTick = m_uiLastTick;
	uint32_t uiGroundTick = m_uiGroundTick;
	uint32_t uiBaseTick = m_uiBaseTick;
	uint32_t uiBaseServer = m_uiBaseServer;
	if( !m_bHasClientClock )
	{
		// first update since a reset: judged by the server's time since it
		const uint32_t uiElapsed = std::max< uint32_t >( 1, uiServerNow - m_uiLastServer );
		uiLastTick = uiClientTick - uiElapsed;
		if( !m_bAir )
			uiGroundTick = uiLastTick;
		uiBaseTick = uiClientTick;
		uiBaseServer = uiServerNow;
	}
	else
	{
		if( static_cast< int32_t >( uiClientTick - uiLastTick ) <= 0 )
			return Reject( Reason::R_RATE, uiServerNow );

		const uint32_t uiClientElapsed = uiClientTick - uiBaseTick;
		const uint32_t uiServerElapsed = uiServerNow - uiBaseServer;
		if( uiClientElapsed > uiServerElapsed + m_kConfig.uiMaxClockLeadMs )
			return Reject( Reason::R_SPEED, uiServerNow );
		if( uiServerElapsed > uiClientElapsed + m_kConfig.uiMaxClockLagMs )
			uiBaseServer = uiServerNow - ( uiClientElapsed + m_kConfig.uiMaxClockLagMs );	// late packets buy nothing later
	}
	const float fDeltaTime = static_cast< float >( uiClientTick - uiLastTick ) / 1000.f;

	bool bAir = m_bAir;
	float fGroundY = m_fGroundY;
	int iGroundLine = m_iGroundLine;
	if( bAir && m_bHasClientClock && m_pLineMap != nullptr && kState.m_sSpeedY > 0 && m_kLast.m_sSpeedY <= 0 )
	{
		// falling at the last update and rising at this one: it landed and jumped again in between (jumping again
		// at once lands between two updates), from the line below the last update, if it could fall that far
		const KSquareUnitPhysics::Physic& kPhysic = m_kPhysics.GetPhysic();
		const float fMaxFallSpeed = std::fabs( kPhysic.fMaxGSpeed );
		const float fFallSpeed = std::min( static_cast< float >( -m_kLast.m_sSpeedY ), fMaxFallSpeed );
		const float fMaxFall = std::min( fFallSpeed * fDeltaTime + 0.5f * kPhysic.fGAccel * fDeltaTime * fDeltaTime,
										 fMaxFallSpeed * fDeltaTime );
		float fLandY = 0.f;
		const int iLandLine = FindGroundBelow( *m_pLineMap, m_kLast.m_vPos, m_kConfig.fWallRadius, fLandY );
		if( iLandLine >= 0 && m_kLast.m_vPos.y - fLandY <= fMaxFall + m_kConfig.fHeightSlack )
		{
			bAir		= false;
			fGroundY	= fLandY;
			iGroundLine	= iLandLine;
			uiGroundTick = uiLastTick;
		}
	}

	const CXSLLineMap::LineData* pGroundLine = m_pLineMap != nullptr ? m_pLineMap->GetLineData( iGroundLine ) : nullptr;
	const int iLine = m_pLineMap != nullptr ? FindLine( *m_pLineMap, kState.m_vPos, m_kConfig.fLineRadius ) : -1;
	const CXSLLineMap::LineData* pLine = m_pLineMap != nullptr ? m_pLineMap->GetLineData( iLine ) : nullptr;

	// on the ground: on a line, or (no line map) in a state that keeps it on one; an air state touching a line while
	// not rising (landing, or the first sample of a jump) is on the ground too
	const bool bGround = m_pLineMap != nullptr
		? iLine >= 0 && ( pState->bLandConnect || kState.m_sSpeedY <= 0 || !bAir )
		: pState->bLandConnect;

	bool bThrownByLine = m_bThrownByLine;
	uint32_t uiTakeoffWindowMs = m_uiTakeoffWindowMs;
	float fLaunchSpeedX = m_fLaunchSpeedX;
	float fLaunchSpeedY = m_fLaunchSpeedY;
	if( !bGround && !bAir )
	{
		// takeoff, somewhere between the last sample on the ground and this one
		bAir = true;
		uiTakeoffWindowMs = uiClientTick - uiGroundTick;
		fLaunchSpeedX = m_kPhysics.GetMaxSpeedX() * m_kConfig.fSpeedFactor;
		fLaunchSpeedY = m_kPhysics.GetMaxSpeedY();
		bThrownByLine = false;
		if( pGroundLine != nullptr )
		{
			fLaunchSpeedX = std::max( fLaunchSpeedX, std::fabs( pGroundLine->m_fSpeed ) );
			if( pGroundLine->m_vJumpSpeed.y > 0.f )
			{
				fLaunchSpeedX = std::max( fLaunchSpeedX, std::fabs( pGroundLine->m_vJumpSpeed.x ) );
				fLaunchSpeedY = std::max( fLaunchSpeedY, pGroundLine->m_vJumpSpeed.y );
				bThrownByLine = true;
			}
		}
	}

	// horizontal distance
	float fMaxSpeedX = m_kPhysics.GetMaxSpeedX() * m_kConfig.fSpeedFactor;
	if( pGroundLine != nullptr )
		fMaxSpeedX = std::max( fMaxSpeedX, std::fabs( pGroundLine->m_fSpeed ) );
	if( pLine != nullptr )
		fMaxSpeedX = std::max( fMaxSpeedX, std::fabs( pLine->m_fSpeed ) );
	if( bAir )
		fMaxSpeedX = std::max( fMaxSpeedX, fLaunchSpeedX );
	const float fDistance = std::hypot( kState.m_vPos.x - m_kLast.m_vPos.x, kState.m_vPos.z - m_kLast.m_vPos.z );
	if( fDistance > fMaxSpeedX * m_kConfig.fSpeedTolerance * fDeltaTime + m_kConfig.fDistanceSlack )
		return Reject( Reason::R_SPEED, uiServerNow );

	if( m_pLineMap != nullptr && HitsWall( *m_pLineMap, m_kLast.m_vPos, kState.m_vPos, iGroundLine, iLine >= 0, m_kConfig.fWallRadius ) )
		return Reject( Reason::R_WALL, uiServerNow );

	if( !bGround )
	{
		if( pState->bLandConnect && m_pLineMap != nullptr && !bThrownByLine )
			return Reject( Reason::R_OFF_LINE, uiServerNow );

		const float fTime = static_cast< float >( uiClientTick - uiGroundTick ) / 1000.f;
		const float fMaxHeight = MaxHeight( fLaunchSpeedY, m_kPhysics.GetPhysic().fGAccel, std::fabs( m_kPhysics.GetPhysic().fMaxGSpeed ),
											fTime, static_cast< float >( uiTakeoffWindowMs ) / 1000.f );
		if( kState.m_vPos.y - fGroundY > fMaxHeight + m_kConfig.fHeightSlack )
			return Reject( Reason::R_RISE, uiServerNow );
	}

	// accepted
	m_kLast				= kState;
	m_uiLastTick		= uiClientTick;
	m_uiLastServer		= uiServerNow;
	m_bHasClientClock	= true;
	m_uiBaseTick		= uiBaseTick;
	m_uiBaseServer		= uiBaseServer;
	m_bCorrected		= false;
	m_uiGroundTick		= uiGroundTick;
	m_bAir				= bAir;
	m_bThrownByLine		= bThrownByLine;
	m_uiTakeoffWindowMs	= uiTakeoffWindowMs;
	m_fLaunchSpeedX		= fLaunchSpeedX;
	m_fLaunchSpeedY		= fLaunchSpeedY;
	m_fGroundY			= fGroundY;
	m_iGroundLine		= iGroundLine;
	if( bGround )
		SetGround( kState.m_vPos, iLine, uiClientTick );
	return Verdict();
}

KVillageMoveValidator::Verdict KVillageMoveValidator::Reject( u_char ucReason, uint32_t uiServerNow )
{
	Verdict kVerdict;
	kVerdict.ucReason = ucReason;
	kVerdict.bNewViolation = !m_bCorrected || uiServerNow - m_uiCorrectionServer >= m_kConfig.uiCorrectionGraceMs;
	if( kVerdict.bNewViolation )
	{
		m_bCorrected = true;
		m_uiCorrectionServer = uiServerNow;
	}
	return kVerdict;
}

void KVillageMoveValidator::SetGround( const VECTOR3& vPos, int iLine, uint32_t uiTick )
{
	m_bAir			= false;
	m_bThrownByLine	= false;
	m_fGroundY		= vPos.y;
	m_iGroundLine	= iLine;
	m_uiGroundTick	= uiTick;
}

float KVillageMoveValidator::MaxHeight( float fSpeedY, float fGAccel, float fMaxFallSpeed, float fTime, float fWindow )
{
	if( fGAccel <= 0.f )
		return FLT_MAX;

	const float fApexTime = fSpeedY / fGAccel;
	const float fApexHeight = fSpeedY * fSpeedY / ( 2.f * fGAccel );
	const float fTerminalTime = fMaxFallSpeed > 0.f ? fMaxFallSpeed / fGAccel : FLT_MAX;
	const auto Height = [&]( float t )
	{
		if( t <= 0.f )
			return 0.f;
		if( t <= fApexTime )
			return fSpeedY * t - 0.5f * fGAccel * t * t;
		const float fFall = t - fApexTime;
		const float fDrop = fFall <= fTerminalTime
			? 0.5f * fGAccel * fFall * fFall
			: 0.5f * fGAccel * fTerminalTime * fTerminalTime + fMaxFallSpeed * ( fFall - fTerminalTime );
		return fApexHeight - fDrop;
	};

	// the unit left the ground between fTime - fWindow and fTime seconds ago: the highest of those curves
	const float fEarliest = std::max( 0.f, fTime - fWindow );
	if( fEarliest <= fApexTime && fApexTime <= fTime )
		return fApexHeight;
	if( fTime < fApexTime )
		return Height( fTime );
	return Height( fEarliest );
}

int KVillageMoveValidator::FindLine( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius )
{
	int iNearest = -1;
	float fNearest = fRadius;
	for( int i = 0; i < kLineMap.GetNumLineData(); ++i )
	{
		const CXSLLineMap::LineData* pLine = kLineMap.GetLineData( i );
		if( pLine == nullptr || !IsWalkable( *pLine ) )
			continue;
		const float fDistance = DistanceToSegment( vPos, pLine->startPos, pLine->endPos );
		if( fDistance <= fNearest )
		{
			fNearest = fDistance;
			iNearest = i;
		}
	}
	return iNearest;
}

bool KVillageMoveValidator::HasLineBelow( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius )
{
	float fY = 0.f;
	return FindGroundBelow( kLineMap, vPos, fRadius, fY ) >= 0;
}

int KVillageMoveValidator::FindGroundBelow( const CXSLLineMap& kLineMap, const VECTOR3& vPos, float fRadius, float& fY )
{
	int iHighest = -1;
	for( int i = 0; i < kLineMap.GetNumLineData(); ++i )
	{
		const CXSLLineMap::LineData* pLine = kLineMap.GetLineData( i );
		if( pLine == nullptr || !IsWalkable( *pLine ) )
			continue;
		const float t = std::clamp( HorizontalParameter( vPos, pLine->startPos, pLine->endPos ), 0.f, 1.f );
		const D3DXVECTOR3 vNearest = pLine->startPos + ( pLine->endPos - pLine->startPos ) * t;
		if( std::hypot( vPos.x - vNearest.x, vPos.z - vNearest.z ) <= fRadius && vNearest.y <= vPos.y + fRadius
			&& ( iHighest < 0 || vNearest.y > fY ) )
		{
			iHighest = i;
			fY = vNearest.y;
		}
	}
	return iHighest;
}

bool KVillageMoveValidator::HitsWall( const CXSLLineMap& kLineMap, const VECTOR3& vFrom, const VECTOR3& vTo, int iLastLine,
									  bool bOnLine, float fRadius )
{
	for( const CXSLLineMap::LineData* pWall : kLineMap.GetWallList() )
	{
		if( pWall->bEnable && SegmentDistance( vFrom, vTo, pWall->startPos, pWall->endPos ) <= 1.f )
			return true;
	}
	// a closed end of the line it last stood on: the client keeps a unit off its line within that end
	const CXSLLineMap::LineData* pLast = bOnLine ? nullptr : kLineMap.GetLineData( iLastLine );
	if( pLast != nullptr )
	{
		const float t = HorizontalParameter( vTo, pLast->startPos, pLast->endPos );
		const float fLength = std::hypot( pLast->endPos.x - pLast->startPos.x, pLast->endPos.z - pLast->startPos.z );
		const float fBeyond = fLength > 0.f ? ( t < 0.f ? -t : t - 1.f ) * fLength : 0.f;
		if( fBeyond > fRadius && ( ( t < 0.f && pLast->beforeLine == -2 ) || ( t > 1.f && pLast->nextLine == -2 ) ) )
			return true;
	}

	// no line below somewhere on the way
	const float fDistance = std::hypot( vTo.x - vFrom.x, vTo.z - vFrom.z );
	const int iSteps = std::max( 1, static_cast< int >( std::ceil( fDistance / fRadius ) ) );
	for( int iStep = 1; iStep <= iSteps; ++iStep )
	{
		const float f = static_cast< float >( iStep ) / static_cast< float >( iSteps );
		const VECTOR3 vPos( vFrom.x + ( vTo.x - vFrom.x ) * f, vFrom.y + ( vTo.y - vFrom.y ) * f, vFrom.z + ( vTo.z - vFrom.z ) * f );
		if( !HasLineBelow( kLineMap, vPos, fRadius ) )
			return true;
	}
	return false;
}

#endif // X2_VILLAGE_SYNC
