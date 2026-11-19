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

	const CXSLLineMap::LineData* pGroundLine = m_pLineMap != nullptr ? m_pLineMap->GetLineData( m_iGroundLine ) : nullptr;
	const int iLine = m_pLineMap != nullptr ? FindLine( *m_pLineMap, kState.m_vPos, m_kConfig.fLineRadius ) : -1;
	const CXSLLineMap::LineData* pLine = m_pLineMap != nullptr ? m_pLineMap->GetLineData( iLine ) : nullptr;

	// on the ground: on a line, or (no line map) in a state that keeps it on one; an air state touching a line while
	// not rising (landing, or the first sample of a jump) is on the ground too
	const bool bGround = m_pLineMap != nullptr
		? iLine >= 0 && ( pState->bLandConnect || kState.m_sSpeedY <= 0 || !m_bAir )
		: pState->bLandConnect;

	bool bAir = m_bAir;
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

	if( !bGround )
	{
		if( pState->bLandConnect && m_pLineMap != nullptr && !bThrownByLine )
			return Reject( Reason::R_OFF_LINE, uiServerNow );

		const float fTime = static_cast< float >( uiClientTick - uiGroundTick ) / 1000.f;
		const float fMaxHeight = MaxHeight( fLaunchSpeedY, m_kPhysics.GetPhysic().fGAccel, std::fabs( m_kPhysics.GetPhysic().fMaxGSpeed ),
											fTime, static_cast< float >( uiTakeoffWindowMs ) / 1000.f );
		if( kState.m_vPos.y - m_fGroundY > fMaxHeight + m_kConfig.fHeightSlack )
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

#endif // X2_VILLAGE_SYNC
