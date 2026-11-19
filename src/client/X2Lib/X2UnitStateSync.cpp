#include "stdafx.h"

#ifdef X2_VILLAGE_SYNC

#include "./X2UnitStateSync.h"

#include <chrono>

/*static*/ uint32_t CX2UnitStateSync::GetClockMs()
{
	return static_cast< uint32_t >( std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::steady_clock::now().time_since_epoch() ).count() );
}

uint32_t CX2UnitStateSync::GetRenderTick() const
{
	return m_kServerClock.ToServer( GetClockMs() ) - KFieldUnitSnapshots::Config().uiDelayMs;
}

void CX2UnitStateSync::FrameMove( float fElapsedTime, CX2SquareUnit* pMyUnit, bool bCanSend )
{
	m_fSendTime += fElapsedTime;
	if( m_fSendTime < 0.1f )
		return;
	m_fSendTime = 0.f;

	if( pMyUnit == NULL || bCanSend == false )
		return;

	const KFieldUnitState kState = pMyUnit->GetFieldUnitState();
	const bool bChanged = !m_bSentState
		|| kState.m_vPos.x != m_kSentState.m_vPos.x || kState.m_vPos.y != m_kSentState.m_vPos.y
		|| kState.m_vPos.z != m_kSentState.m_vPos.z || kState.m_sSpeedX != m_kSentState.m_sSpeedX
		|| kState.m_sSpeedY != m_kSentState.m_sSpeedY || kState.m_sLineIndex != m_kSentState.m_sLineIndex
		|| kState.m_ucStateID != m_kSentState.m_ucStateID || kState.m_ucStateSeq != m_kSentState.m_ucStateSeq
		|| kState.m_bIsRight != m_kSentState.m_bIsRight;
	// standing still sends nothing, but once: the others see it stopped where it stopped
	if( !bChanged && !m_bSentChange )
		return;

	KEGS_FIELD_UNIT_STATE_NOT kPacket;
	kPacket.m_uiTick = GetClockMs();
	if( m_bSentState && static_cast< int32_t >( kPacket.m_uiTick - m_uiSentTick ) <= 0 )
		kPacket.m_uiTick = m_uiSentTick + 1;	// the server drops a tick that does not advance
	kPacket.m_kState = kState;
	g_pData->GetServerProtocol()->SendPacket( EGS_FIELD_UNIT_STATE_NOT, kPacket );

	m_bSentState	= true;
	m_bSentChange	= bChanged;
	m_kSentState	= kState;
	m_uiSentTick	= kPacket.m_uiTick;
}

void CX2UnitStateSync::ReceiveBatch( const KEGS_FIELD_UNIT_STATE_BATCH_NOT& kBatch, UidType iMyUnitUID,
									 const std::function< CX2SquareUnit* ( UidType ) >& fnFindUnit )
{
	KTDXPROFILE();

	m_kServerClock.OnBatch( kBatch.m_uiTick, GetClockMs() );

	// units that never got created (left before they loaded, or not drawn at all)
	for( std::map< UidType, PendingState >::iterator it = m_mapPendingState.begin(); it != m_mapPendingState.end(); )
	{
		if( kBatch.m_uiTick - it->second.uiTick > 60000 )
			it = m_mapPendingState.erase( it );
		else
			++it;
	}

	for( const KFieldUnitStateEntry& kEntry : kBatch.m_vecUnit )
	{
		if( kEntry.m_UnitUID == iMyUnitUID )
			continue;

		CX2SquareUnit* pUnit = fnFindUnit( kEntry.m_UnitUID );
		if( pUnit != NULL )
		{
			pUnit->PushFieldUnitState( kEntry.m_uiTick, kEntry.m_kState );
			continue;
		}

		// a unit that came into view is created once its models load; its state comes at once, and a unit
		// standing still sends no other
		m_mapPendingState[ kEntry.m_UnitUID ] = PendingState{ kEntry.m_uiTick, kEntry.m_kState };

		// not drawn here: a party member's position still goes to the party list, as the original's sync did
		if( g_pData != NULL && g_pData->GetPartyManager()->DoIHaveParty() == true
			&& g_pData->GetPartyManager()->IsMyPartyMember( kEntry.m_UnitUID ) == true )
		{
			const VECTOR3& vPos = kEntry.m_kState.m_vPos;
			g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( kEntry.m_UnitUID )->m_vPosition = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		}
	}
}

void CX2UnitStateSync::ReceiveCorrection( CX2SquareUnit* pMyUnit, const KEGS_FIELD_UNIT_CORRECTION_NOT& kCorrection )
{
	if( pMyUnit == NULL )
		return;

	dbg::clog << L"Movement corrected by the server, reason " << static_cast< int >( kCorrection.m_ucReason ) << dbg::endl;
	pMyUnit->ApplyFieldCorrection( kCorrection.m_kState );
	m_bSentChange = true;	// tell the server where the unit is now, even if it stays
}

void CX2UnitStateSync::OnUnitAdded( CX2SquareUnit* pUnit, bool bMyUnit )
{
	if( pUnit == NULL )
		return;

	pUnit->SetStateSync( true );
	std::map< UidType, PendingState >::iterator it = m_mapPendingState.find( pUnit->GetUID() );
	if( it == m_mapPendingState.end() )
		return;
	if( bMyUnit == false )
		pUnit->PushFieldUnitState( it->second.uiTick, it->second.kState );
	m_mapPendingState.erase( it );
}

#endif // X2_VILLAGE_SYNC
