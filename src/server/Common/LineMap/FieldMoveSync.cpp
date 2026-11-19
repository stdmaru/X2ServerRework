#include "BaseServer.h"

#ifdef X2_VILLAGE_SYNC

#include "LineMap/FieldMoveSync.h"
#include "LineMap/XSLLineMap.h"

#include <algorithm>
#include <cfloat>
#include <cmath>

KFieldMoveSync::KFieldMoveSync( const CXSLLineMap* pLineMap, const KSquareUnitPhysics& kPhysics, const Config& kConfig )
	: m_pLineMap( pLineMap )
	, m_kPhysics( kPhysics )
	, m_kConfig( kConfig )
{
}

KFieldMoveSync::Result KFieldMoveSync::AddUnit( UidType iUnitUID, const VECTOR3& vPos, bool bFastMove, uint32_t uiServerNow )
{
	KVillageMoveValidator::Config kValidatorConfig = m_kConfig.kValidator;
	if( bFastMove )
		kValidatorConfig.fSpeedFactor *= m_kConfig.fFastMoveFactor;

	m_mapUnit.erase( iUnitUID );
	Unit& kUnit = m_mapUnit.emplace( iUnitUID, Unit( m_pLineMap, m_kPhysics, kValidatorConfig ) ).first->second;

	Result kResult;
	kResult.bAccepted = true;
	kUnit.kState.m_vPos = vPos;
	if( m_pLineMap != nullptr && !KVillageMoveValidator::HasLineBelow( *m_pLineMap, vPos, kValidatorConfig.fWallRadius ) )
	{
		kUnit.kState.m_vPos = NearestOnLine( m_pLineMap, vPos );
		kResult.bAccepted = false;
		kResult.bCorrect = true;
		kResult.kCorrection.m_ucReason = KEGS_FIELD_UNIT_CORRECTION_NOT::R_OFF_LINE;
	}
	kUnit.kValidator.Reset( kUnit.kState, uiServerNow );
	kUnit.uiStateTick = uiServerNow;
	kUnit.bChanged = true;
	kResult.kCorrection.m_kState = kUnit.kState;
	return kResult;
}

void KFieldMoveSync::RemoveUnit( UidType iUnitUID )
{
	m_mapUnit.erase( iUnitUID );
	m_mapShow.erase( iUnitUID );
	for( auto& kShow : m_mapShow )
		kShow.second.erase( iUnitUID );
}

void KFieldMoveSync::SetUnitState( UidType iUnitUID, const KFieldUnitState& kState, uint32_t uiServerNow )
{
	auto it = m_mapUnit.find( iUnitUID );
	if( it == m_mapUnit.end() )
		return;
	it->second.kState = kState;
	it->second.uiStateTick = uiServerNow;
	it->second.bChanged = true;
	it->second.kValidator.Reset( kState, uiServerNow );
}

KFieldMoveSync::Result KFieldMoveSync::OnState( UidType iUnitUID, const KEGS_FIELD_UNIT_STATE_NOT& kNot, uint32_t uiServerNow )
{
	Result kResult;
	auto it = m_mapUnit.find( iUnitUID );
	if( it == m_mapUnit.end() )
		return kResult;
	Unit& kUnit = it->second;

	const KVillageMoveValidator::Verdict kVerdict = kUnit.kValidator.Check( kNot.m_uiTick, kNot.m_kState, uiServerNow );
	if( kVerdict.IsValid() )
	{
		kUnit.kState = kNot.m_kState;
		kUnit.uiStateTick = uiServerNow;
		kUnit.bChanged = true;
		kResult.bAccepted = true;
		return kResult;
	}

	// a rejection soon after a correction is an update the client made before it applied it: dropped
	if( !kVerdict.bNewViolation )
		return kResult;

	kResult.bCorrect = true;
	kResult.kCorrection.m_uiTick = kNot.m_uiTick;
	kResult.kCorrection.m_kState = kUnit.kValidator.GetLastValid();
	kResult.kCorrection.m_ucReason = kVerdict.ucReason;
	kResult.iViolations = CountViolation( kUnit, uiServerNow );
	kResult.bKick = m_kConfig.iKickViolations > 0 && kResult.iViolations >= m_kConfig.iKickViolations;
	return kResult;
}

void KFieldMoveSync::ShowTo( UidType iViewer, UidType iUnitUID )
{
	if( iViewer != iUnitUID )
		m_mapShow[ iViewer ].insert( iUnitUID );
}

bool KFieldMoveSync::IsTickDue( uint32_t uiServerNow ) const
{
	return !m_bTicked || uiServerNow - m_uiLastTick >= m_kConfig.uiTickMs;
}

void KFieldMoveSync::BuildBatches( uint32_t uiServerNow, const Destinations& fnDestinations,
								   std::map< UidType, KEGS_FIELD_UNIT_STATE_BATCH_NOT >& mapBatch )
{
	mapBatch.clear();
	m_bTicked = true;
	m_uiLastTick = uiServerNow;

	const auto Add = [&]( UidType iViewer, UidType iUnitUID, const Unit& kUnit )
	{
		KEGS_FIELD_UNIT_STATE_BATCH_NOT& kBatch = mapBatch[ iViewer ];
		kBatch.m_uiTick = uiServerNow;
		KFieldUnitStateEntry kEntry;
		kEntry.m_UnitUID = iUnitUID;
		kEntry.m_uiTick = kUnit.uiStateTick;
		kEntry.m_kState = kUnit.kState;
		kBatch.m_vecUnit.push_back( kEntry );
	};

	std::vector< UidType > vecViewer;
	for( auto& [iUnitUID, kUnit] : m_mapUnit )
	{
		if( !kUnit.bChanged )
			continue;
		kUnit.bChanged = false;
		vecViewer.clear();
		fnDestinations( iUnitUID, vecViewer );
		for( UidType iViewer : vecViewer )
		{
			if( iViewer == iUnitUID || !HasUnit( iViewer ) )
				continue;
			Add( iViewer, iUnitUID, kUnit );
			auto itShow = m_mapShow.find( iViewer );
			if( itShow != m_mapShow.end() )
				itShow->second.erase( iUnitUID );	// already in its batch
		}
	}

	for( const auto& [iViewer, setUnit] : m_mapShow )
	{
		if( !HasUnit( iViewer ) )
			continue;
		for( UidType iUnitUID : setUnit )
		{
			auto it = m_mapUnit.find( iUnitUID );
			if( it != m_mapUnit.end() )
				Add( iViewer, iUnitUID, it->second );
		}
	}
	m_mapShow.clear();
}

const KFieldUnitState* KFieldMoveSync::GetState( UidType iUnitUID ) const
{
	auto it = m_mapUnit.find( iUnitUID );
	return it != m_mapUnit.end() ? &it->second.kState : nullptr;
}

int KFieldMoveSync::CountViolation( Unit& kUnit, uint32_t uiServerNow ) const
{
	auto& vecViolation = kUnit.vecViolation;
	vecViolation.erase( std::remove_if( vecViolation.begin(), vecViolation.end(),
		[&]( uint32_t uiTime ) { return uiServerNow - uiTime >= m_kConfig.uiViolationWindowMs; } ), vecViolation.end() );
	vecViolation.push_back( uiServerNow );
	return static_cast< int >( vecViolation.size() );
}

VECTOR3 KFieldMoveSync::NearestOnLine( const CXSLLineMap* pLineMap, const VECTOR3& vPos )
{
	if( pLineMap == nullptr )
		return vPos;
	VECTOR3 vNearest = vPos;
	float fNearest = FLT_MAX;
	for( int i = 0; i < pLineMap->GetNumLineData(); ++i )
	{
		const CXSLLineMap::LineData* pLine = pLineMap->GetLineData( i );
		if( pLine == nullptr || !pLine->bEnable || pLine->lineType == CXSLLineMap::LT_MONSTER_ROAD )
			continue;
		const D3DXVECTOR3 d = pLine->endPos - pLine->startPos;
		const float fLength2 = d.x * d.x + d.y * d.y + d.z * d.z;
		float t = fLength2 > 0.f
			? ( ( vPos.x - pLine->startPos.x ) * d.x + ( vPos.y - pLine->startPos.y ) * d.y + ( vPos.z - pLine->startPos.z ) * d.z ) / fLength2
			: 0.f;
		t = std::clamp( t, 0.f, 1.f );
		const D3DXVECTOR3 p = pLine->startPos + d * t;
		const float fDistance = ( p.x - vPos.x ) * ( p.x - vPos.x ) + ( p.y - vPos.y ) * ( p.y - vPos.y ) + ( p.z - vPos.z ) * ( p.z - vPos.z );
		if( fDistance < fNearest )
		{
			fNearest = fDistance;
			vNearest = VECTOR3( p.x, p.y, p.z );
		}
	}
	return vNearest;
}

#endif // X2_VILLAGE_SYNC
