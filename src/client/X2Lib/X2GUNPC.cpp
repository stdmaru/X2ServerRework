#include "stdafx.h"

#include "./X2GUNPC.h"
#include "./X2GUNPCFunc.h"
#include "./X2NPCAI.h"

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
#define X2GUNPC_RECEIVE_FRAME_COUNT_MAX_GAB     (70*10)
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
#define X2GUNPC_NONHOST_SIM_MINIMUM_DELAY         (0.1f)
#define X2GUNPC_NONHOST_SIM_MAXIMUM_DELAY         (2.0f)
#define X2GUNPC_NONHOST_SIM_MAXIMUM_RESPONSE_DELAY (2.5f)
#define X2GUNPC_NONHOST_SIM_XSPEED_TOLERANCE       (100.f)
#define X2GUNPC_NONHOST_SIM_MAX_NUM_REACTION_SIMULATION 6



static_assert( ( MAX_GAME_USER_UNIT_NUM * 2 ) <= ( sizeof(DWORD) * 8 ) , "");

/*static*/
unsigned    CX2GUNPC::KNonHostReaction::ms_uReactionSimulationCounter = 0;

/*static*/
inline bool     CX2GUNPC::KNonHostReaction::_IsNumReactionSimulationMax()
{ return ms_uReactionSimulationCounter >= X2GUNPC_NONHOST_SIM_MAX_NUM_REACTION_SIMULATION; }



struct  KStateEnumType
{
    const char*             m_pszName;
    CX2GUNPC::EStateType    m_eStateType;
};

static const KStateEnumType   s_aStateTypes[] =
{
        { "START_STATE", CX2GUNPC::STATE_TYPE_START_STATE },
        { "REBIRTH_STATE", CX2GUNPC::STATE_TYPE_REBIRTH_STATE },
        { "WAIT_STATE", CX2GUNPC::STATE_TYPE_WAIT_STATE },
        { "RAGE_STATE", CX2GUNPC::STATE_TYPE_RAGE_STATE },
        { "SIEGE_STATE", CX2GUNPC::STATE_TYPE_SIEGE_STATE },
        { "DISABLED_STATE", CX2GUNPC::STATE_TYPE_DISABLED_STATE },

        { "SMALL_DAMAGE_LAND_FRONT", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_LAND_FRONT },
        { "SMALL_DAMAGE_LAND_BACK", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_LAND_BACK },
        { "SMALL_DAMAGE_AIR", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_AIR },
        { "SMALL_DAMAGE_AIR_FRONT", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_AIR_FRONT },
        { "SMALL_DAMAGE_AIR_BACK", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_AIR_BACK },
        { "BIG_DAMAGE_LAND_FRONT", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_LAND_FRONT },
        { "BIG_DAMAGE_LAND_BACK", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_LAND_BACK },
        { "BIG_DAMAGE_AIR", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_AIR },
        { "BIG_DAMAGE_AIR_FRONT", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_AIR_FRONT },
        { "BIG_DAMAGE_AIR_BACK", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_AIR_BACK },
        { "DOWN_DAMAGE_LAND_FRONT", CX2GUNPC::STATE_TYPE_DOWN_DAMAGE_LAND_FRONT },
        { "DOWN_DAMAGE_LAND_BACK", CX2GUNPC::STATE_TYPE_DOWN_DAMAGE_LAND_BACK },
        { "DOWN_DAMAGE_AIR", CX2GUNPC::STATE_TYPE_DOWN_DAMAGE_AIR },
        { "DOWN_DAMAGE_AIR_LANDING", CX2GUNPC::STATE_TYPE_DOWN_DAMAGE_AIR_LANDING },
        { "UP_DAMAGE", CX2GUNPC::STATE_TYPE_UP_DAMAGE },
        { "FLY_DAMAGE_FRONT", CX2GUNPC::STATE_TYPE_FLY_DAMAGE_FRONT },
        { "FLY_DAMAGE_BACK",  CX2GUNPC::STATE_TYPE_FLY_DAMAGE_BACK },
        { "RIDE_DAMAGE_FRONT", CX2GUNPC::STATE_TYPE_RIDE_DAMAGE_FRONT },
        { "RIDE_DAMAGE_BACK", CX2GUNPC::STATE_TYPE_RIDE_DAMAGE_BACK },
        { "DAMAGE_FLUSH_LAND_FRONT", CX2GUNPC::STATE_TYPE_DAMAGE_FLUSH_LAND_FRONT },
        { "DAMAGE_FLUSH_LAND_BACK", CX2GUNPC::STATE_TYPE_DAMAGE_FLUSH_LAND_BACK },
        { "DAMAGE_FLUSH_AIR", CX2GUNPC::STATE_TYPE_DAMAGE_FLUSH_AIR },
        { "DAMAGE_REVENGE",  CX2GUNPC::STATE_TYPE_DAMAGE_REVENGE},
        { "DEFENCE_STATE", CX2GUNPC::STATE_TYPE_DEFENCE_STATE },
        { "SMALL_DAMAGE_LAND_GUARD", CX2GUNPC::STATE_TYPE_SMALL_DAMAGE_LAND_GUARD  },
        { "BIG_DAMAGE_LAND_GUARD", CX2GUNPC::STATE_TYPE_BIG_DAMAGE_LAND_GUARD },
        { "DOWN_DAMAGE_LAND_GUARD", CX2GUNPC::STATE_TYPE_DOWN_DAMAGE_LAND_GUARD },
        { "REVENGE_ATTACK", CX2GUNPC::STATE_TYPE_REVENGE_ATTACK },
        { "DYING_LAND_FRONT", CX2GUNPC::STATE_TYPE_DYING_LAND_FRONT },
        { "DYING_LAND_BACK", CX2GUNPC::STATE_TYPE_DYING_LAND_BACK },
        { "DYING_SKY", CX2GUNPC::STATE_TYPE_DYING_SKY },
        { "DYING_FLY", CX2GUNPC::STATE_TYPE_DYING_FLY },


        { "SELF_DESTRUCTION_STATE", CX2GUNPC::STATE_TYPE_SELF_DESTRUCTION_STATE },
        { "IMMUNE_SKILL_STATE", CX2GUNPC::STATE_TYPE_IMMUNE_SKILL_STATE },
        { "AIR_FALL", CX2GUNPC::STATE_TYPE_AIR_FALL },
        { "RIDE_STATE", CX2GUNPC::STATE_TYPE_RIDE_STATE },
        { "WIN_STATE", CX2GUNPC::STATE_TYPE_WIN_STATE },
        { "LOSE_STATE", CX2GUNPC::STATE_TYPE_LOSE_STATE },
        { "SUMMON_END_STATE", CX2GUNPC::STATE_TYPE_SUMMON_END_STATE },
};

static_assert( ARRAY_SIZE( s_aStateTypes ) == CX2GUNPC::STATE_TYPE_NUMS , "");


CX2GUNPC::KNonHostReaction::KNonHostReaction()
: m_eNowStateReaction( REACTION_STATE_NONE )
, m_eNowSubstateReaction( REACTION_SUBSTATE_NORMAL )
{
    m_cbufNonHostSimState.set_capacity( 16 );
    int iFrameRate = (int) ( 1.f / g_pKTDXApp->GetElapsedTime() );
    iFrameRate = __min( __max( iFrameRate, 70 ), 100 );
    m_cbuffPositionHistory.set_capacity( iFrameRate );

    Init();
}

CX2GUNPC::KNonHostReaction::~KNonHostReaction()
{
    _UpdateNowStateReaction( REACTION_STATE_NONE, REACTION_SUBSTATE_NORMAL );
}

void    CX2GUNPC::KNonHostReaction::Init()
{
    m_eNextStateReaction = REACTION_STATE_NONE;
    m_bNextStateRelaxReactionCheck = false;
    m_bNextStateReactionWait = false;
    m_iNextStateReactionUnitIndex = -1;
    m_v2NextStateReactionYMinMax = D3DXVECTOR2(0,0);
    _UpdateNowStateReaction( REACTION_STATE_NONE, REACTION_SUBSTATE_NORMAL );
    m_bNowStateRelaxReactionCheck = false;
    m_iNowStateReactionUnitIndex = -1;
    m_cbufNonHostSimState.resize(0);
    m_bNowStateStartSpeedX = false;
    m_fNowStateStartSpeedX = 0.f;
    m_dwNowReactionStartFrame = 0;
    m_fNonHostSimulationTolerance = 0.f;
    m_cbuffPositionHistory.resize(0);
    m_bReservedReceivePos = false;
    m_vReservedReceivePos = D3DXVECTOR3(0,0,0);
    m_iReservedReceiveLineIndex = -1;
    m_bReservedReceiveFootOnLine = false;

    ResetRelativePosAndTimers();
    ResetLocalRelativePosAndTimers();

    m_dwLastSentRelativePos = 0;
    m_iLastSentTargetUnitIndex = -1;
    m_uLastSentMiniSyncCount = 0;

    m_usDefenceRandomTableIndex = 0;
    m_iTargetUnitIndex = -1;
}


/*static*/ bool CX2GUNPC::KNonHostReaction::_CheckCompatibleReactionCondition( float fSpeedX1, float fSpeedX2 )
{
    if ( fSpeedX1 < -X2GUNPC_NONHOST_SIM_XSPEED_TOLERANCE && fSpeedX2 > X2GUNPC_NONHOST_SIM_XSPEED_TOLERANCE
        || fSpeedX1 > X2GUNPC_NONHOST_SIM_XSPEED_TOLERANCE && fSpeedX2 < -X2GUNPC_NONHOST_SIM_XSPEED_TOLERANCE )
        return false;
    return true;
}

/*static*/ bool    CX2GUNPC::KNonHostReaction::CheckCompatibleReactionState( char state1, char state2, const std::vector<StateData>& vecStateData )
{
    if ( state1 == state2 )
        return true;

    if ( state1 <= 0 || state1 >= (char) vecStateData.size()
        || state2 <= 0 || state2 >= (char) vecStateData.size() )
        return false;

    return vecStateData[ state1 ].m_cEquivalentReactionState == state2
        || vecStateData[ state2 ].m_cEquivalentReactionState == state1;
}

/*static*/ bool    CX2GUNPC::KNonHostReaction::_CheckInvincibleState(
    char stateID, float fTolerance, float fAnimSpeedAdjustFactor, const std::vector<StateData>& vecStateData )
{
    if ( stateID < 0 || stateID >= (int) vecStateData.size() )
        return false;
    const StateData& nowStateData = vecStateData[ stateID ];
    if ( NOWSTATE_FLAG( SET_SUPER_ARMOR_VALUE ) == true )
        return true;
    if ( nowStateData.m_fSetForceInvincibleTimeValue > 0.f )
        return true;
    if ( nowStateData.m_fSetGlobalSuperArmorTimeValue > 0.f )
        return true;

    if ( fTolerance >= nowStateData.m_fSetAnimWaitTimeValue )
        fTolerance -= nowStateData.m_fSetAnimWaitTimeValue;

    if ( fTolerance > 0.f )
    {
	    /// m_fTimedEventAnimSpeed가 주어져 있으면 m_fTimedEventAnimSpeed를 사용 아니면 m_fAnimSpeed를 사용
	    fTolerance *= nowStateData.m_fSetAnimSpeedValue * fAnimSpeedAdjustFactor;
    }//if

    if ( fTolerance < 0.f )
        return false;

    {
        float fMin = __max( nowStateData.m_v2SetInvincibleTimeValue.x, 0.f );
        float fMax = __min( nowStateData.m_v2SetInvincibleTimeValue.y, fTolerance );
        if ( fMin <= fMax )
            return true;
    }

    if ( nowStateData.m_vecSuperArmorTime.empty() == false )
    {
        for( unsigned u = 0; u < nowStateData.m_vecSuperArmorTime.size(); u++ )
        {
            const D3DXVECTOR2& v2Time = nowStateData.m_vecSuperArmorTime[ u ];
            float fMin = __max( v2Time.x, 0.f );
            float fMax = __min( v2Time.y, fTolerance );
            if ( fMin <= fMax )
                return true;
        }
    }


    return false;
}



void    CX2GUNPC::KNonHostReaction::DoStateEndStart( bool bLocalUnit, const NPCFrameData& m_NPCFrameData, const PhysicParam& m_PhysicParam )
{
    bool bPrevReactionState = m_eNowStateReaction != REACTION_STATE_NONE;
    bool bNextReactionState = m_eNextStateReaction != REACTION_STATE_NONE;

    if ( bNextReactionState == true )
    {
        if ( bLocalUnit == false )
        {
            KNonHostSimState    kSimState;
            kSimState.m_kStateChangeParts = m_NPCFrameData.syncData.stateChangeParts;
            kSimState.m_cState = m_NPCFrameData.syncData.nowState;
            kSimState.m_eReactionStateType = ( GetNowStateReactionWait() == true ) ? REACTION_STATE_NONE : m_eNowStateReaction;
            kSimState.m_iReactionStateUnitIndex = m_iNowStateReactionUnitIndex;
            kSimState.m_fStartSpeedX = ( m_bNowStateStartSpeedX == true ) ? m_bNowStateStartSpeedX
                : ( ( m_NPCFrameData.syncData.bIsRight == true ) ? m_PhysicParam.nowSpeed.x : -m_PhysicParam.nowSpeed.x );
            kSimState.m_dwReactionStartFrame = m_dwNowReactionStartFrame;
            switch( m_eNextStateReaction )
            {
            case REACTION_STATE_FIRST:
                {
                    m_dwNowReactionStartFrame = g_pX2Game->GetFrameUDPPack().GetFrameMoveCount();
                    if ( kSimState.m_eReactionStateType == REACTION_STATE_NONE )
                        kSimState.m_dwReactionStartFrame = m_dwNowReactionStartFrame;
                    kSimState.m_v2ReactionYMinMax = m_v2NextStateReactionYMinMax;
                    if ( m_eNowStateReaction == REACTION_STATE_NONE )
                    {
                        m_cbufNonHostSimState.resize(0);
                    }//if
                    _UpdateNowStateReaction( REACTION_STATE_FIRST, REACTION_SUBSTATE_NORMAL );
                    m_bNowStateRelaxReactionCheck = m_bNextStateRelaxReactionCheck;
                    if ( m_cbufNonHostSimState.full() == true )
                        m_cbufNonHostSimState.set_capacity( m_cbufNonHostSimState.capacity() + 1 );
                    m_cbufNonHostSimState.push_back( kSimState );
                }
                break;
            case REACTION_STATE_CONTINUE:
                {
                    if ( m_cbufNonHostSimState.empty() == true )
                        kSimState.m_v2ReactionYMinMax = D3DXVECTOR2(0,0);
                    else
                        kSimState.m_v2ReactionYMinMax = m_cbufNonHostSimState.back().m_v2ReactionYMinMax;
                    _UpdateNowStateReaction( ( m_eNowStateReaction == REACTION_STATE_FIRST ) ? REACTION_STATE_FIRST : REACTION_STATE_CONTINUE
                        , ( m_bNextStateReactionWait == true ) ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                    //m_bNowStateRelaxReactionCheck = m_bNowStateRelaxReactionCheck;
                    if ( m_cbufNonHostSimState.full() == true )
                        m_cbufNonHostSimState.set_capacity( m_cbufNonHostSimState.capacity() + 1 );
                    m_cbufNonHostSimState.push_back( kSimState );
                }//if.. else..
                break;
            case REACTION_STATE_HOST_FIRST:
                {
                    m_dwNowReactionStartFrame = g_pX2Game->GetFrameUDPPack().GetFrameMoveCount();
                    m_cbufNonHostSimState.resize( 0 );
                    _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
                    m_bNowStateRelaxReactionCheck = m_bNextStateRelaxReactionCheck;
                }//if.. else..
                break;
            case REACTION_STATE_HOST_CONTINUE:
                {
                    if ( m_cbufNonHostSimState.empty() == true )
                        kSimState.m_v2ReactionYMinMax = D3DXVECTOR2(0,0);
                    else
                        kSimState.m_v2ReactionYMinMax = m_cbufNonHostSimState.back().m_v2ReactionYMinMax;
                    _UpdateNowStateReaction( REACTION_STATE_HOST_CONTINUE
                         , ( m_bNextStateReactionWait == true ) ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                    //m_bNowStateRelaxReactionCheck = m_bNowStateRelaxReactionCheck;
                    if ( m_cbufNonHostSimState.full() == true )
                        m_cbufNonHostSimState.set_capacity( m_cbufNonHostSimState.capacity() + 1 );
                    m_cbufNonHostSimState.push_back( kSimState );
                }//if.. else..
                break;
            }
        }
        else
        {
            m_dwNowReactionStartFrame = g_pX2Game->GetFrameUDPPack().GetFrameMoveCount();
            m_cbufNonHostSimState.resize( 0 );
            _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
            if ( m_eNextStateReaction == REACTION_STATE_FIRST || m_eNextStateReaction == REACTION_STATE_HOST_FIRST )
            {
                m_bNowStateRelaxReactionCheck = m_bNextStateRelaxReactionCheck;
            }
            else
            {
                //m_bNowStateRelaxReactionCheck = m_bNowStateRelaxReactionCheck;
            }
        }
        m_iNowStateReactionUnitIndex = m_iNextStateReactionUnitIndex;
        m_eNextStateReaction = REACTION_STATE_NONE;
        m_bNextStateReactionWait = false;
        m_bNextStateRelaxReactionCheck = false;
        m_iNextStateReactionUnitIndex = -1;
        m_bNowStateStartSpeedX = false;
    }
    else
    {
        m_eNextStateReaction = REACTION_STATE_NONE;
        m_bNextStateReactionWait = false;
        _UpdateNowStateReaction( REACTION_STATE_NONE, REACTION_SUBSTATE_NORMAL );
        m_bNextStateRelaxReactionCheck = false;
        m_bNowStateRelaxReactionCheck = false;
        m_cbufNonHostSimState.resize( 0 );
        m_bNowStateStartSpeedX = false;
        m_iNextStateReactionUnitIndex = -1;
        m_iNowStateReactionUnitIndex = -1;
        m_dwNowReactionStartFrame = 0;
    }

    if ( bLocalUnit == true && bPrevReactionState == true && bNextReactionState == false )
    {
        m_usDefenceRandomTableIndex = (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );
    }
}


void    CX2GUNPC::KNonHostReaction::FixStatesForLocalUnitOrReaction_NONE( bool bLocalUnit )
{
    if ( bLocalUnit == true || m_eNowStateReaction == REACTION_STATE_NONE )
    {
        m_cbufNonHostSimState.resize( 0 );
        m_eNowSubstateReaction = REACTION_SUBSTATE_NORMAL;
        if ( m_eNowStateReaction == REACTION_STATE_NONE )
            m_bNowStateRelaxReactionCheck = false;
        if ( bLocalUnit == true )
        {
            if ( m_eNowStateReaction != REACTION_STATE_NONE )
            {
                _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
            }
            ResetReservedReceivePos();
        }
    }
}

void    CX2GUNPC::KNonHostReaction::FixStatesForReaction_NOT_FIRST( bool bLocalUnit )
{
    if ( bLocalUnit == true )
    {
        FixStatesForLocalUnitOrReaction_NONE( true );
    }
    else
    {
        if ( m_eNowStateReaction == REACTION_STATE_FIRST )
        {
            _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
        }
    }
}



void    CX2GUNPC::KNonHostReaction::UpdateNowStateStartSpeed( const NPCFrameData& m_NPCFrameData, const PhysicParam& m_PhysicParam )
{
    if ( m_eNowStateReaction != REACTION_STATE_NONE )
    {
        if ( m_bNowStateStartSpeedX == false )
        {
            m_fNowStateStartSpeedX = m_PhysicParam.nowSpeed.x;
            if ( m_NPCFrameData.syncData.bIsRight == false )
                m_fNowStateStartSpeedX = -m_fNowStateStartSpeedX;
            m_bNowStateStartSpeedX = true;
        }//if
    }
    else
    {
        m_bNowStateStartSpeedX = false;
    }
}


void    CX2GUNPC::KNonHostReaction::DoFrameMove_SimulateReaction_FIRST(
    NPCFrameData& m_NPCFrameData, PhysicParam& m_PhysicParam,
    KCircularBuffer< NPCFrameSync >& m_cbufReceiveDynamicPacket,
    DWORD& m_dwLastProcessedFrameMoveCount, bool& bUpdatePositionBackup,
    float& fNowHP, float& fNowMP,
    bool bSuperArmorFactor, float fAnimSpeedAdjustFactor, float fUnitHeight, const std::vector<StateData>& vecStateData
    )
{
    if ( m_eNowStateReaction == REACTION_STATE_FIRST )
    {
        if ( m_cbufNonHostSimState.empty() == true )
        {
            _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
            return;
        }//if
        char            cPrevState = 0;
        unsigned char   cPrevStateChangeNum = 0;
        for( ; m_cbufReceiveDynamicPacket.empty() == false; m_cbufReceiveDynamicPacket.pop_front() )
        {
            const NPCFrameSync& frameSync = m_cbufReceiveDynamicPacket.front();
            const KDYNAMIC_UNIT_NPC_SYNC& xPT_UNIT_NPC_SYNC = frameSync.m_sync;
            m_dwLastProcessedFrameMoveCount = frameSync.m_dwFrameMoveCount;
            if ( frameSync.m_bMiniSync == true )
            {
                UpdateRelativePosAndTimers( xPT_UNIT_NPC_SYNC.m_dwRelativePos );
                SetTargetUnitIndex( xPT_UNIT_NPC_SYNC.m_cTargetUnitIndex );
                continue;
            }//if
            // 필드에서 NCP 가 원래 위치보다 너무 떨어졌을 때 come back state 조건이 발동한 경우이다.
            // come back state 에서는 충돌 판정이 일어나지 않는다.
            bool bComeBackState = xPT_UNIT_NPC_SYNC.ExtractIsComeBackState();
            if ( bComeBackState == true )
            {
                m_cbufNonHostSimState.resize( 0 );
                _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
//printf( "NPC State comback %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                bUpdatePositionBackup = true;
                return;
            }
            if ( cPrevStateChangeNum == xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum
                && cPrevState == xPT_UNIT_NPC_SYNC.m_cNowState )
            {
                goto continue_common;
            }
            bool bReactionState = xPT_UNIT_NPC_SYNC.ExtractReactionState();
            if ( bReactionState == false )
            {
                KCircularBuffer<KNonHostSimState>::iterator iterHist = m_cbufNonHostSimState.begin();
                for( ;iterHist != m_cbufNonHostSimState.end(); ++iterHist )
                {
                    if ( iterHist->m_eReactionStateType == REACTION_STATE_NONE )
                        break;
                }
                if ( iterHist == m_cbufNonHostSimState.end() )
                {
                    m_cbufNonHostSimState.resize( 0 );
                    _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
//printf( "NPC State not reaction %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//        (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                    bUpdatePositionBackup = true;
                    return;
                }//if
                KNonHostSimState& kSimState = *iterHist;
                LONG lFrameDiff = (LONG) ( g_pX2Game->GetFrameUDPPack().GetFrameMoveCount() - kSimState.m_dwReactionStartFrame );
                if ( lFrameDiff < 0 )
                    lFrameDiff = 0;
                float   fNonHostSimElapsedTime = lFrameDiff * g_pKTDXApp->GetElapsedTime();
                float   fTimeTolerance = m_fNonHostSimulationTolerance - fNonHostSimElapsedTime;
                bool    bBroken = false;
                if ( fTimeTolerance < 0.f || bSuperArmorFactor == true
                    || _CheckInvincibleState( xPT_UNIT_NPC_SYNC.m_cNowState, fTimeTolerance, fAnimSpeedAdjustFactor, vecStateData ) == true )
                {
                    bBroken = true;
//if ( fTimeTolerance < 0.f )
//printf( "NPC State timeout %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
//else
//printf( "NPC State invincible %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                }
                else
                {
			        float fY = NPCSyncData::_DecodeNpcSyncPosY( xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                    float   fSpeedY = halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedY );
                    if ( fY + fUnitHeight < kSimState.m_v2ReactionYMinMax.x
                        || fY > kSimState.m_v2ReactionYMinMax.y
                        || ( fSpeedY > 0.f && fY + fSpeedY * fTimeTolerance > kSimState.m_v2ReactionYMinMax.y
                                || fSpeedY < 0.f && fY + fUnitHeight + fSpeedY * fTimeTolerance < kSimState.m_v2ReactionYMinMax.x ) )
                    {
                        bBroken = true;
//printf( "NPC State outofrange %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                    }
                }
                if ( bBroken == true )
                {
                    m_cbufNonHostSimState.resize( 0 );
                    _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                    bUpdatePositionBackup = true;
                    return;
                }//if
                unsigned char stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum & 0xf;
                for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist; iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                {
                    iterHist2->m_kStateChangeParts.stateChangeNum = stateChangeNum;
                    stateChangeNum = ( stateChangeNum + 1 ) & 0xf;
                }
                m_NPCFrameData.syncData.stateChangeParts.stateChangeNum = stateChangeNum;
                m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = stateChangeNum;
                m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                goto continue_common;
            }
            else
            {
                float   fSpeedX = halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedX );
                if ( xPT_UNIT_NPC_SYNC.ExtractIsRight() == false )
                    fSpeedX = -fSpeedX;
                if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex == m_iNowStateReactionUnitIndex
                    && xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum == m_NPCFrameData.syncData.stateChangeParts.stateChangeNum
                    && CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, m_NPCFrameData.syncData.nowState, vecStateData ) == true )
                {
                    m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                    m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                    m_cbufNonHostSimState.resize(0);
                    _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
//printf( "NPC State skip1 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                    return;
                }

                for( KCircularBuffer<KNonHostSimState>::iterator iterHist = m_cbufNonHostSimState.begin();
                    iterHist != m_cbufNonHostSimState.end(); ++iterHist )
                {
                    KNonHostSimState& kSimState = *iterHist;
                    if ( kSimState.m_eReactionStateType == REACTION_STATE_NONE )
                        continue;
                    if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex != kSimState.m_iReactionStateUnitIndex
                        || xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum != kSimState.m_kStateChangeParts.stateChangeNum
                        || CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, kSimState.m_cState, vecStateData ) == false )
                        continue;
                    if ( kSimState.m_dwReactionStartFrame < m_dwNowReactionStartFrame )
                    {
//printf( "NPC State skip2 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        if ( kSimState.m_eReactionStateType == REACTION_STATE_FIRST )
                        {
                            for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                                iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                            {
                                KNonHostSimState& kSimState2 = *iterHist2;
                                if ( kSimState2.m_dwReactionStartFrame != kSimState.m_dwReactionStartFrame )
                                    break;
                                if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                                    kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                            }//for
                        }
                        m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                        goto continue_common;
                    }
                    else if ( _CheckCompatibleReactionCondition( fSpeedX, kSimState.m_fStartSpeedX ) == true )
                    {
//printf( "NPC State skip3 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                        for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                            iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                        {
                            KNonHostSimState& kSimState2 = *iterHist2;
                            kSimState2.m_dwReactionStartFrame = m_dwNowReactionStartFrame;
                            if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                                kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                        }//for
                        m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                        return;
                    }
                    else
                    {
//printf( "NPC State not compatible1  %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        m_cbufNonHostSimState.resize(0);
                        _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                        return;
                    }
                }
                for( KCircularBuffer<KNonHostSimState>::iterator iterHist = m_cbufNonHostSimState.begin();
                    iterHist != m_cbufNonHostSimState.end(); ++iterHist )
                {
                    KNonHostSimState& kSimState = *iterHist;
                    if ( kSimState.m_eReactionStateType != REACTION_STATE_FIRST )
                        continue;
                    if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex != kSimState.m_iReactionStateUnitIndex
                        || CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, kSimState.m_cState, vecStateData ) == false )
                        continue;
                    if ( kSimState.m_dwReactionStartFrame < m_dwNowReactionStartFrame )
                    {
//printf( "NPC State skip4 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        unsigned char stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum & 0xf;
                        for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                            iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                        {
                            KNonHostSimState& kSimState2 = *iterHist2;
                            kSimState2.m_kStateChangeParts.stateChangeNum = stateChangeNum;
                            stateChangeNum = ( stateChangeNum + 1 ) & 0xf;
                            if ( kSimState2.m_dwReactionStartFrame == kSimState.m_dwReactionStartFrame )
                            {
                                if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                                    kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                            }
                        }//for
                        m_NPCFrameData.syncData.stateChangeParts.stateChangeNum = stateChangeNum;
                        m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = stateChangeNum;
                        m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                        goto continue_common;
                    }
                    else if ( _CheckCompatibleReactionCondition( fSpeedX, kSimState.m_fStartSpeedX ) == true )
                    {
//printf( "NPC State skip5 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                        unsigned char stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum & 0xf;
                        for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                            iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                        {
                            KNonHostSimState& kSimState2 = *iterHist2;
                            kSimState2.m_kStateChangeParts.stateChangeNum = stateChangeNum;
                            stateChangeNum = ( stateChangeNum + 1 ) & 0xf;
                            kSimState2.m_dwReactionStartFrame = m_dwNowReactionStartFrame;
                            if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                                kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                        }//for
                        m_NPCFrameData.syncData.stateChangeParts.stateChangeNum = stateChangeNum;
                        m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = stateChangeNum;
                        m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                        return;
                    }
                }
                if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex == m_iNowStateReactionUnitIndex
                    && CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, m_NPCFrameData.syncData.nowState, vecStateData ) == true )
                {
                    m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                    m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                    m_cbufNonHostSimState.resize( 0 );
                    _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
//printf( "NPC State skip6 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );

                    return;
                }
//printf( "NPC State notfound %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                m_cbufNonHostSimState.resize(0);
                _UpdateNowStateReaction( REACTION_STATE_CONTINUE, GetNowStateReactionWait() ? REACTION_SUBSTATE_WAIT : REACTION_SUBSTATE_NORMAL );
                return;
            }//if.. else..
continue_common:
            fNowHP = xPT_UNIT_NPC_SYNC.m_fNowHP;
            fNowMP = halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowMP );
            m_NPCFrameData.syncData.mindFlag = (char)xPT_UNIT_NPC_SYNC.ExtractMindFlag();
            SetTargetUnitIndex( xPT_UNIT_NPC_SYNC.m_cTargetUnitIndex );
            UpdateRelativePosAndTimers( xPT_UNIT_NPC_SYNC.m_dwRelativePos );
            cPrevState = xPT_UNIT_NPC_SYNC.m_cNowState;
            cPrevStateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum;
        }//for
    }//if
}


bool    CX2GUNPC::KNonHostReaction::DoFrameMove_SimulateReaction(
    NPCFrameData& m_NPCFrameData, PhysicParam& m_PhysicParam,
    bool& bUpdatePositionBackup, bool& bStateChange, bool& bForceModifyConfig, bool& bNotUpdateDefenceRandomTableIndex,
    const KDYNAMIC_UNIT_NPC_SYNC& xPT_UNIT_NPC_SYNC, const std::vector<StateData>& vecStateData )

{
    if ( xPT_UNIT_NPC_SYNC.ExtractIsComeBackState() == true )
    {
//printf( "NPC State comback2 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
        m_eNextStateReaction = REACTION_STATE_NONE;
        m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
        m_NPCFrameData.unitCondition.nextState = xPT_UNIT_NPC_SYNC.m_cNowState;
        bUpdatePositionBackup = true;
        bStateChange = true;
        return false;
    }
    bool bReactionState = xPT_UNIT_NPC_SYNC.ExtractReactionState();
    if ( bReactionState == false )
    {
        if ( GetNowStateReactionWait() == true
            && m_NPCFrameData.syncData.nowState == xPT_UNIT_NPC_SYNC.m_cNowState )
        {
            m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            _UpdateNowStateReaction( REACTION_STATE_NONE, REACTION_SUBSTATE_NORMAL );
            m_bNowStateRelaxReactionCheck = false;
            m_cbufNonHostSimState.resize( 0 );
            m_bNowStateStartSpeedX = false;
            m_iNowStateReactionUnitIndex = -1;
            m_dwNowReactionStartFrame = 0;
            return false;
        }
//printf( "NPC State notreact2 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
        m_eNextStateReaction = REACTION_STATE_NONE;
        m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
        m_NPCFrameData.unitCondition.nextState = xPT_UNIT_NPC_SYNC.m_cNowState;
        bStateChange = true;
        bUpdatePositionBackup = true;
        return false;
    }
    else
    {
        m_bNowStateRelaxReactionCheck = xPT_UNIT_NPC_SYNC.ExtractRelaxReactionStateCheck();
        float   fSpeedX = halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedX );
        if ( xPT_UNIT_NPC_SYNC.ExtractIsRight() == false )
            fSpeedX = -fSpeedX;
        if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex == m_iNowStateReactionUnitIndex
            && xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum == m_NPCFrameData.syncData.stateChangeParts.stateChangeNum
            && CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, m_NPCFrameData.syncData.nowState, vecStateData ) == true )
        {
            if ( m_eNowStateReaction == REACTION_STATE_HOST_FIRST || m_eNowStateReaction == REACTION_STATE_HOST_CONTINUE )
            {
                if ( m_eNowStateReaction == REACTION_STATE_HOST_CONTINUE )
                    _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
                else
                    m_eNowSubstateReaction = REACTION_SUBSTATE_NORMAL;
            }
            else // REACTION_STATE_CONTINUE
            if ( m_eNowSubstateReaction != REACTION_SUBSTATE_CHECKED )
            {
                if ( _CheckCompatibleReactionCondition( fSpeedX, ( m_bNowStateStartSpeedX == true ) ? m_fNowStateStartSpeedX
                    : ( ( m_NPCFrameData.syncData.bIsRight == true ) ? m_PhysicParam.nowSpeed.x : -m_PhysicParam.nowSpeed.x ) ) == true )
                {
                }
                else
                {
                    bForceModifyConfig = true;
                }
                if ( _IsNumReactionSimulationMax() == true )
                    _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
                else
                    m_eNowSubstateReaction = REACTION_SUBSTATE_CHECKED;
            }
            m_cbufNonHostSimState.resize(0);
            m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            if ( bForceModifyConfig == true )
            {
//printf( "NPC State direction change %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                return false;
            }
            else
            {
//printf( "NPC State skip7 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                D3DXVECTOR3 vReservedReceivePos;
                NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                ReserveReceivePos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex, xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                return true;
            }
        }
        for( KCircularBuffer<KNonHostSimState>::iterator iterHist = m_cbufNonHostSimState.begin();
            iterHist != m_cbufNonHostSimState.end(); ++iterHist )
        {
            KNonHostSimState& kSimState = *iterHist;
            if ( kSimState.m_eReactionStateType == REACTION_STATE_NONE )
                continue;
            if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex != kSimState.m_iReactionStateUnitIndex
                || xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum != kSimState.m_kStateChangeParts.stateChangeNum
                || CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, kSimState.m_cState, vecStateData ) == false )
                continue;
            if ( kSimState.m_dwReactionStartFrame < m_dwNowReactionStartFrame )
            {
//printf( "NPC State skip8 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                if ( kSimState.m_eReactionStateType == REACTION_STATE_FIRST )
                {
                    for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                        iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                    {
                        KNonHostSimState& kSimState2 = *iterHist2;
                        if ( kSimState2.m_dwReactionStartFrame != kSimState.m_dwReactionStartFrame )
                            break;
                        if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                            kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                    }//for
                }
                m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(),iterHist );
				ResetReservedReceivePos();
                bNotUpdateDefenceRandomTableIndex = true;
                return true;
            }
            else
            {
                if ( kSimState.m_eReactionStateType == REACTION_STATE_FIRST )
                {
                    if ( _CheckCompatibleReactionCondition( fSpeedX, kSimState.m_fStartSpeedX ) == true )
                    {
//printf( "NPC State skip9 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                        for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                            iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                        {
                            KNonHostSimState& kSimState2 = *iterHist2;
                            if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                                kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                        }//for
                        m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(),iterHist );
                        D3DXVECTOR3 vReservedReceivePos;
                        NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                        ReserveReceivePos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex, xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                        return true;
                    }
                    else
                    {
                        goto match_failed;
                    }
                }
                else
                {
//printf( "NPC State skip10 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                    m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(),iterHist );
                    D3DXVECTOR3 vReservedReceivePos;
                    NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                    ReserveReceivePos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex, xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                    return true;
                }
            }
        }
        if ( m_eNowStateReaction == REACTION_STATE_HOST_FIRST || m_eNowStateReaction == REACTION_STATE_HOST_CONTINUE )
        {
            goto match_complete_failed;
        }
        for( KCircularBuffer<KNonHostSimState>::iterator iterHist = m_cbufNonHostSimState.begin();
            iterHist != m_cbufNonHostSimState.end(); ++iterHist )
        {
            KNonHostSimState& kSimState = *iterHist;
            if ( kSimState.m_eReactionStateType != REACTION_STATE_FIRST )
                continue;
            if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex != kSimState.m_iReactionStateUnitIndex
                || CheckCompatibleReactionState( xPT_UNIT_NPC_SYNC.m_cNowState, kSimState.m_cState, vecStateData ) == false )
                continue;
            if ( kSimState.m_dwReactionStartFrame < m_dwNowReactionStartFrame )
            {
//printf( "NPC State skip11 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
                unsigned char stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum & 0xf;
                for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                    iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                {
                    KNonHostSimState& kSimState2 = *iterHist2;
                    kSimState2.m_kStateChangeParts.stateChangeNum = stateChangeNum;
                    stateChangeNum = ( stateChangeNum + 1 ) & 0xf;
                    if ( kSimState2.m_dwReactionStartFrame == kSimState.m_dwReactionStartFrame )
                    {
                        if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                            kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                    }
                }//for
                m_NPCFrameData.syncData.stateChangeParts.stateChangeNum = stateChangeNum;
                m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = stateChangeNum;
                m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
				ResetReservedReceivePos();
                bNotUpdateDefenceRandomTableIndex = true;
                return true;
            }
            else
            {
                if ( _CheckCompatibleReactionCondition( fSpeedX, kSimState.m_fStartSpeedX ) == true )
                {
                    unsigned char stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum & 0xf;
                    for( KCircularBuffer<KNonHostSimState>::iterator iterHist2 = iterHist;
                        iterHist2 != m_cbufNonHostSimState.end(); ++iterHist2 )
                    {
                        KNonHostSimState& kSimState2 = *iterHist2;
                        kSimState2.m_kStateChangeParts.stateChangeNum = stateChangeNum;
                        stateChangeNum = ( stateChangeNum + 1 ) & 0xf;
                        kSimState2.m_dwReactionStartFrame = m_dwNowReactionStartFrame;
                        if ( kSimState2.m_eReactionStateType == REACTION_STATE_FIRST )
                            kSimState2.m_eReactionStateType = REACTION_STATE_CONTINUE;
                    }//for
                    m_NPCFrameData.syncData.stateChangeParts.stateChangeNum = stateChangeNum;
                    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = stateChangeNum;
                    m_cbufNonHostSimState.rerase( m_cbufNonHostSimState.begin(), iterHist );
                    D3DXVECTOR3 vReservedReceivePos;
                    NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                    ReserveReceivePos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex, xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                    return true;
                }
            }
        }
        if ( xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex == m_iNowStateReactionUnitIndex
            && CheckCompatibleReactionState(xPT_UNIT_NPC_SYNC.m_cNowState, m_NPCFrameData.syncData.nowState, vecStateData ) == true )
        {
//printf( "NPC State skip12 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
            if ( m_eNowSubstateReaction != REACTION_SUBSTATE_CHECKED )
            {
                if ( _CheckCompatibleReactionCondition( fSpeedX, ( m_bNowStateStartSpeedX == true ) ? m_fNowStateStartSpeedX
                    : ( ( m_NPCFrameData.syncData.bIsRight == true ) ? m_PhysicParam.nowSpeed.x : -m_PhysicParam.nowSpeed.x ) ) == true )
                {
                    m_eNowSubstateReaction = REACTION_SUBSTATE_CHECKED;
                }
            }
            if ( m_eNowSubstateReaction == REACTION_SUBSTATE_CHECKED )
            {
                m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
                if ( _IsNumReactionSimulationMax() == true )
                    _UpdateNowStateReaction( REACTION_STATE_HOST_FIRST, REACTION_SUBSTATE_NORMAL );
                m_cbufNonHostSimState.resize(0);
                D3DXVECTOR3 vReservedReceivePos;
                NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                ReserveReceivePos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex, xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                return true;
            }
        }
match_failed:
//printf( "NPC State notfound2 %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
        if ( m_bNowStateRelaxReactionCheck == true && m_iNowStateReactionUnitIndex == xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex
            && GetNowStateReactionWait() == false )
        {
            m_NPCFrameData.syncData.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
            m_cbufNonHostSimState.resize(0);
            ResetReservedReceivePos();
            bNotUpdateDefenceRandomTableIndex = true;
            return true;
        }//if
match_complete_failed:
        m_eNextStateReaction = REACTION_STATE_HOST_FIRST;
        m_bNextStateRelaxReactionCheck = m_bNowStateRelaxReactionCheck;
        m_bNextStateReactionWait = false;
        m_iNextStateReactionUnitIndex = xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex;
        m_cbufNonHostSimState.resize(0);
        m_NPCFrameData.unitCondition.stateChangeParts = xPT_UNIT_NPC_SYNC.m_stateChangeParts;
        m_NPCFrameData.unitCondition.nextState = xPT_UNIT_NPC_SYNC.m_cNowState;
        bStateChange = true;
        return false;
    }

    return false;
}


void    CX2GUNPC::KNonHostReaction::DoFrameMove_StateChangeByHost( bool& bUpdatePositionBackup, const KDYNAMIC_UNIT_NPC_SYNC& xPT_UNIT_NPC_SYNC )
{
    if ( xPT_UNIT_NPC_SYNC.ExtractIsComeBackState() == true )
    {
        m_eNextStateReaction = REACTION_STATE_NONE;
        bUpdatePositionBackup = true;
    }
    else if ( xPT_UNIT_NPC_SYNC.ExtractReactionState() == true )
    {
//printf( "NPC State by host %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//    (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
        m_eNextStateReaction = REACTION_STATE_HOST_FIRST;
        m_bNextStateReactionWait = false;
        m_bNowStateRelaxReactionCheck = xPT_UNIT_NPC_SYNC.ExtractRelaxReactionStateCheck();
        m_iNextStateReactionUnitIndex = xPT_UNIT_NPC_SYNC.m_cReactionStateUnitIndex;
    }
    else
    {
        m_eNextStateReaction = REACTION_STATE_NONE;
    }
}


void    CX2GUNPC::KNonHostReaction::ReserveReceivePos( const D3DXVECTOR3& vReservedReceivePos, int iLastTouchLineIndex, bool bFootOnLine )
{
    m_bReservedReceivePos = true;
    m_vReservedReceivePos = vReservedReceivePos;
    m_iReservedReceiveLineIndex = iLastTouchLineIndex;
    m_bReservedReceiveFootOnLine = bFootOnLine;
}




void    CX2GUNPC::KNonHostReaction::SetNextStateReaction_NONE()
{
    m_eNextStateReaction = REACTION_STATE_NONE;
}

void    CX2GUNPC::KNonHostReaction::SetNextStateReaction_FIRST(
    bool bRelaxReactionCheck,
    int  iAttackerUnitIndex,
    const D3DXVECTOR2& vReactionYMinMax )
{
    m_eNextStateReaction = REACTION_STATE_FIRST;
    m_bNextStateReactionWait = false;
    m_bNextStateRelaxReactionCheck = bRelaxReactionCheck;
    m_iNextStateReactionUnitIndex = iAttackerUnitIndex;
    m_v2NextStateReactionYMinMax = vReactionYMinMax;
}


void    CX2GUNPC::KNonHostReaction::SetNextStateReaction_CONTINUE( bool bWait )
{
    m_eNextStateReaction = ( m_eNowStateReaction == REACTION_STATE_HOST_FIRST || m_eNowStateReaction == REACTION_STATE_HOST_CONTINUE )
        ? REACTION_STATE_HOST_CONTINUE : REACTION_STATE_CONTINUE;
    m_bNextStateReactionWait = bWait;
    m_iNextStateReactionUnitIndex = m_iNowStateReactionUnitIndex;
}



void    CX2GUNPC::KNonHostReaction::BuffPositionHistory( bool bLocalUnit, const D3DXVECTOR3& vPosition )
{
    if ( bLocalUnit == true )
        m_cbuffPositionHistory.push_back( vPosition );
    else
        m_cbuffPositionHistory.resize(0);
}


void    CX2GUNPC::KNonHostReaction::UpdateSimulationTolerance()
{
    CX2GUUser* pHostUser = g_pX2Game->GetHostGameUnit();
    if ( pHostUser != NULL && g_pX2Room != NULL )
    {
        float   fAvgPingTime = 0.f;
        const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( pHostUser->GetUnitIndex() );
        if ( pSlotData != NULL )
        {
            fAvgPingTime = pSlotData->m_fAvgPingTime ;
            fAvgPingTime +=pSlotData->m_fAvgPingStd;
        }
        if ( fAvgPingTime <= 0.1f )
        {
            m_fNonHostSimulationTolerance = fAvgPingTime + 0.1f;
        }
        else if ( fAvgPingTime <= 0.5f )
        {
            float   fRate = ( fAvgPingTime - 0.1f ) / 0.4f;
            m_fNonHostSimulationTolerance = fAvgPingTime + ( ( 1.f - fRate ) * 0.1f + fRate * 0.2f );
        }
        else
        {
            m_fNonHostSimulationTolerance = fAvgPingTime + 0.2f;
            m_fNonHostSimulationTolerance = __min( m_fNonHostSimulationTolerance, X2GUNPC_NONHOST_SIM_MAXIMUM_DELAY );
        }
    }
}

void    CX2GUNPC::KNonHostReaction::AdjustDamageReactionSpeed( NPCFrameData& m_NPCFrameData, PhysicParam& m_PhysicParam,
    bool bIsRightBackup, float fWalkSpeed, float fLagLatency )
{
    float fDiff = 0.f;
    D3DXVECTOR3 vDiff(0,0,0);
    if ( m_cbuffPositionHistory.empty() == false )
    {
        int iLagHistory = (int) ( ( fLagLatency * 0.5f ) / g_pKTDXApp->GetElapsedTime() );
        if ( iLagHistory < 0 )
            iLagHistory = 0;
        else if ( iLagHistory >= (int) m_cbuffPositionHistory.size() )
            iLagHistory = m_cbuffPositionHistory.size() - 1;
        vDiff = m_NPCFrameData.syncData.position - m_cbuffPositionHistory[ iLagHistory ];
        vDiff.y = 0.f;
    }
    fDiff = D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDiff );
    if ( bIsRightBackup == false )
        fDiff = -fDiff;
    if ( fWalkSpeed <= 0.f )
        fDiff = 0.f;
    if ( m_NPCFrameData.unitCondition.bAttackerFront == false && m_PhysicParam.nowSpeed.x > 0.f && fDiff > 0.f )
    {
        float fAum = m_PhysicParam.nowSpeed.x * m_PhysicParam.nowSpeed.x - fDiff * m_PhysicParam.fReloadAccel * ( 2.f / fWalkSpeed );
        if ( fAum > 0.f )
            m_PhysicParam.nowSpeed.x = sqrt( fAum );
        else
            m_PhysicParam.nowSpeed.x = 0.f;
    }
    else if ( m_NPCFrameData.unitCondition.bAttackerFront == true && m_PhysicParam.nowSpeed.x < 0.f && fDiff < 0.f )
    {
        float fAum = m_PhysicParam.nowSpeed.x * m_PhysicParam.nowSpeed.x - (-fDiff) * m_PhysicParam.fReloadAccel * ( 2.f / fWalkSpeed );
        if ( fAum > 0.f )
            m_PhysicParam.nowSpeed.x = -sqrt( fAum );
        else
            m_PhysicParam.nowSpeed.x = 0.f;
    }
}


void    CX2GUNPC::KNonHostReaction::ResetRelativePosAndTimers()
{
    m_dwRelativePos = 0;
    for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
    {
        m_afRelativePosTimers[ i ] = 0.f;
    }
}

void    CX2GUNPC::KNonHostReaction::UpdateRelativePosAndTimers( DWORD dwRelativePos )
{
    DWORD   dwOldRelativePos = m_dwRelativePos;
    DWORD   dwNewRelativePos = dwRelativePos;
    for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
    {
        if ( ( dwOldRelativePos & 0x3 ) != ( dwNewRelativePos & 0x3 ) )
            m_afRelativePosTimers[ i ] = 0.f;
        dwOldRelativePos >>= 2;
        dwNewRelativePos >>= 2;
    }
    m_dwRelativePos = dwRelativePos;
}//CX2GUNPC::_UpdateRelativePosAndTimers()

void    CX2GUNPC::KNonHostReaction::UpdateRelativePosAndTimer( int iUnitIndex, int iPos )
{
    if ( iUnitIndex >= 0 && iUnitIndex < MAX_GAME_USER_UNIT_NUM
        && iPos >= 0 && iPos <= 3 && iPos != GetRelativePos( iUnitIndex ) )
    {
        m_afRelativePosTimers[ iUnitIndex ] = 0.f;
        if ( iPos == 0 )
            m_dwRelativePos &= ~( 0x3 << ( iUnitIndex * 2 ) );
        else
            m_dwRelativePos |= iPos << ( iUnitIndex * 2 );
    }
}

void    CX2GUNPC::KNonHostReaction::ResetLocalRelativePos()
{
    ZeroMemory( m_acLocalRelativePos, sizeof(m_acLocalRelativePos) );
}


void    CX2GUNPC::KNonHostReaction::ResetLocalRelativePosAndTimers()
{
    ResetLocalRelativePos();
    for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
    {
        m_afLocalRelativePosTimers[ i ] = 0.f;
    }
}

void    CX2GUNPC::KNonHostReaction::IncrementLocalAndRelativePosTimers( float  fElapsedTime )
{
    if ( fElapsedTime > 0.f )
    {
        for( int i = 0; i < MAX_GAME_USER_UNIT_NUM; i++ )
        {
            m_afRelativePosTimers[ i ] += fElapsedTime;
            m_afLocalRelativePosTimers[ i ] += fElapsedTime;
        }
    }//if
}


void    CX2GUNPC::KNonHostReaction::PhysicProcess_RetrieveRelativePosInfo( bool bLocalUnit, int iUnitIndex, bool bMyUnit,
                       int iNumAccumFrames,
    OUT int& iLocalDirection, OUT int& iDirection, OUT bool& bReservePosReliable )
{
    iLocalDirection = 0;
    iDirection = 0;
    bReservePosReliable = false;

    if ( iUnitIndex >= 0 && iUnitIndex < MAX_GAME_USER_UNIT_NUM )
    {
        iLocalDirection = m_acLocalRelativePos[ iUnitIndex ];
        if ( bLocalUnit == false )
        {
            iDirection = GetRelativePos( iUnitIndex );
            if ( iDirection != 1 && iDirection != 2 )
                iDirection = 0;
            else
            {
                switch( iLocalDirection )
                {
                case 1:
                case 2:
                    break;
                case 3:
                case 4:
                    iDirection = 0;
					break;
                default: // case 0:
                    break;
                }
            }//if.. else..
            if ( m_eNowStateReaction != REACTION_STATE_FIRST && m_eNowStateReaction != REACTION_STATE_CONTINUE
                        || bMyUnit == true && m_eNowStateReaction == REACTION_STATE_CONTINUE )
            {
                bReservePosReliable = true;
                if ( iDirection != 0 )
                {
                    float fToleranceTime = GetSimulationTolerance();
                    if ( bMyUnit == false )
                    {
                        const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( iUnitIndex );
                        if ( pSlotData != NULL )
                        {
                            fToleranceTime = __max( fToleranceTime, ( fToleranceTime + pSlotData->m_fAvgPingTime
                                    + pSlotData->m_fAvgPingStd ) * 0.5f + 0.2f );
                        }//if
                    }//if
                    if ( iNumAccumFrames > 1 )
                    {
                        fToleranceTime += ( iNumAccumFrames - 1 ) * g_pKTDXApp->GetElapsedTime();
                    }
                    if ( m_afLocalRelativePosTimers[ iUnitIndex ] < fToleranceTime )
                    {
                        bReservePosReliable = false;
                        iDirection = 0;
                    }
                }
            }
            else
            {
                iDirection = 0;
            }
        }//if
    }//if
}


void    CX2GUNPC::KNonHostReaction::PhysicProcess_UpdateRelativePos( bool bLocalUnit, const NPCFrameData& m_NPCFrameData, float fUnitHeight
    , int iTeam, CX2Room::TEAM_NUM eAllyTeam )
{
    if ( bLocalUnit == true )
    {
        D3DXVECTOR2 vMyUnitMinMaxY( m_NPCFrameData.syncData.position.y, m_NPCFrameData.syncData.position.y + fUnitHeight );
        int  iUserUnitSize = g_pX2Game->GetUserUnitListSize();
        ResetRelativePosAndTimers();
        ResetLocalRelativePosAndTimers();
        D3DXVECTOR3 vPosFuture;
        D3DXVECTOR3 vDirVectorFromMeToOther;
        bool bCanPass = m_NPCFrameData.stateParam.bCanPassUnit;
        for( int i = 0; i < __min( iUserUnitSize, MAX_GAME_USER_UNIT_NUM ); i++ )
        {
            CX2GUUser* pX2Unit = g_pX2Game->GetUserUnit( i );
			if ( NULL == pX2Unit ||
                pX2Unit->GetTeam() == iTeam ||
					pX2Unit->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
					( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && eAllyTeam != CX2Room::TN_NONE ) || /// 배틀필드에서 소환된 몬스터와 유저가 부딪힌게 아니라면
                    ( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && pX2Unit->IsMyUnit() == false && pX2Unit->DidReceiveFirstSyncPacketOfUser() == false ) )
                continue;
            bool bUnitCanPass = pX2Unit->GetStateparam( true ).bCanPassUnit;
            D3DXVECTOR3 vPosFuture = pX2Unit->GetPos( true );
	        const float fGameUnitYMin = vPosFuture.y;
	        const float fGameUnitYMax = fGameUnitYMin + pX2Unit->GetUnitHeight( true );
	        if( (fGameUnitYMin >= vMyUnitMinMaxY.x && fGameUnitYMin <= vMyUnitMinMaxY.y) ||
		        (fGameUnitYMax >= vMyUnitMinMaxY.x && fGameUnitYMax <= vMyUnitMinMaxY.y) ||
		        (fGameUnitYMin <= vMyUnitMinMaxY.x && fGameUnitYMax >= vMyUnitMinMaxY.y) )
            {
                vDirVectorFromMeToOther = vPosFuture - m_NPCFrameData.syncData.position;
                vDirVectorFromMeToOther.y = 0.f;
                if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                {
                    UpdateRelativePosAndTimer( i, 2 );
                    if ( bCanPass == false && bUnitCanPass == false )
                         m_acLocalRelativePos[i] = 2;
                }
                else
                {
                    UpdateRelativePosAndTimer( i, 1 );
                    if ( bCanPass == false && bUnitCanPass == false )
                        m_acLocalRelativePos[i] = 1;
                }//if.. else..
            }
            else
            {
                if ( bCanPass == false && bUnitCanPass == false )
                {
                    if ( fGameUnitYMin >= vMyUnitMinMaxY.y )
                        m_acLocalRelativePos[i] = 3;
                    else
                        m_acLocalRelativePos[i] = 4;
                }
            }
        }//for
    }
    else if( m_NPCFrameData.stateParam.bCanPassUnit == false )
    {
        D3DXVECTOR2 vMyUnitMinMaxY( m_NPCFrameData.syncData.position.y, m_NPCFrameData.syncData.position.y + fUnitHeight );
        int  iUserUnitSize = __min( g_pX2Game->GetUserUnitListSize(), MAX_GAME_USER_UNIT_NUM );
        D3DXVECTOR3 vPosFuture;
        D3DXVECTOR3 vDirVectorFromMeToOther;
        for( int i = 0; i < iUserUnitSize; i++ )
        {
            int iOldLocalDirection = m_acLocalRelativePos[i];
            int iNewLocalDirection = iOldLocalDirection;
            CX2GUUser* pX2Unit = g_pX2Game->GetUserUnit( i );
			if ( NULL == pX2Unit || pX2Unit->GetTeam() == iTeam ||
					pX2Unit->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
					( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && eAllyTeam != CX2Room::TN_NONE ) || /// 배틀필드에서 소환된 몬스터와 유저가 부딪힌게 아니라면
                    ( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && pX2Unit->IsMyUnit() == false && pX2Unit->DidReceiveFirstSyncPacketOfUser() == false ) )
            {
                iNewLocalDirection = 0;
                UpdateRelativePosAndTimer( i, 0 );
                m_afLocalRelativePosTimers[ i ] = 0.f;
            }
            else if ( pX2Unit->GetStateparam( true ).bCanPassUnit == true )
            {
                iNewLocalDirection = 0;
                m_afLocalRelativePosTimers[ i ] = 0.f;
            }
            else
            {
                D3DXVECTOR3 vPosFuture = pX2Unit->GetPos( true );
	            const float fGameUnitYMin = vPosFuture.y;
	            const float fGameUnitYMax = fGameUnitYMin + pX2Unit->GetUnitHeight( true );
	            if( (fGameUnitYMin >= vMyUnitMinMaxY.x && fGameUnitYMin <= vMyUnitMinMaxY.y) ||
		            (fGameUnitYMax >= vMyUnitMinMaxY.x && fGameUnitYMax <= vMyUnitMinMaxY.y) ||
		            (fGameUnitYMin <= vMyUnitMinMaxY.x && fGameUnitYMax >= vMyUnitMinMaxY.y) )
                {
                    vDirVectorFromMeToOther = vPosFuture - m_NPCFrameData.syncData.position;
                    vDirVectorFromMeToOther.y = 0.f;
                    int iHostDirection = GetRelativePos( i );
                    int iDirection = 0;
                    float   fDotProduct = D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther );
                    if ( fDotProduct >= 0 )
                        iNewLocalDirection = 2;
                    else if ( fDotProduct < 0 )
                        iNewLocalDirection = 1;
                    else
                    {
                        if ( iOldLocalDirection == 1 || iOldLocalDirection == 2 )
                            iNewLocalDirection = iOldLocalDirection;
                        else if ( iHostDirection == 1 || iHostDirection == 2 )
                            iNewLocalDirection = iHostDirection;
                        else
                            iNewLocalDirection = 2;
                    }
                    if ( iOldLocalDirection == 0
                        && iNewLocalDirection != 0
                        && iNewLocalDirection != iHostDirection )
                    {
                        m_afLocalRelativePosTimers[ i ] = 0.f;
                    }
                }
                else
                {
                    if ( fGameUnitYMin >= vMyUnitMinMaxY.y )
                        iNewLocalDirection = 3;
                    else
                        iNewLocalDirection = 4;
                    UpdateRelativePosAndTimer( i, 0 );
                    m_afLocalRelativePosTimers[ i ] = 0.f;
                }
            }
            m_acLocalRelativePos[i] = iNewLocalDirection;
        }//for
    }
    else
    {
        ResetLocalRelativePosAndTimers();
    }
}


unsigned CX2GUNPC::KNonHostReaction::UpdateLastSentMiniInfo( int iTargetUnitIndex )
{
    if ( m_uLastSentMiniSyncCount > 0 )
        m_uLastSentMiniSyncCount--;
    if ( m_dwLastSentRelativePos != m_dwRelativePos
        || m_iLastSentTargetUnitIndex != iTargetUnitIndex )
        m_uLastSentMiniSyncCount = 2;
    m_dwLastSentRelativePos = m_dwRelativePos;
    m_iLastSentTargetUnitIndex = iTargetUnitIndex;
    return  m_uLastSentMiniSyncCount;
}

bool    CX2GUNPC::KNonHostReaction::CheckLastSentMiniInfo()
{
    return  ( m_dwLastSentRelativePos != m_dwRelativePos );
}


int     CX2GUNPC::KNonHostReaction::EvaluateConditionRandomRate( int iTableIndex, int iStateChangeNum, const ConditionTableData& conditionTableData,
                        const vector<std::pair<D3DXVECTOR3,int> >& m_EventIntervalList )
{
    int iSeed = m_usDefenceRandomTableIndex + iTableIndex + iStateChangeNum;
    if ( conditionTableData.m_iAnimPlayCount != -1 )
        iSeed += conditionTableData.m_iAnimPlayCount;
    if ( conditionTableData.m_fAnimTimerFactor != -1.f )
        iSeed += (int) ( conditionTableData.m_fAnimTimerFactor * 70 );
    if ( conditionTableData.m_iIntervalID >= 0 &&
        conditionTableData.m_iIntervalID < (int) m_EventIntervalList.size() )
        iSeed += m_EventIntervalList[ conditionTableData.m_iIntervalID ].second;
    return CKTDXRandomNumbers::GetRandomInt( iSeed ) % 100;
}



bool    CX2GUNPC::_IsReactionState( char StateID )
{
    if ( StateID <= 0 || StateID >= (char) GetInitStateData().m_vecStateData.size() )
        return false;
    return GetInitStateData().m_vecStateData[StateID].m_bitsetFlags.test( StateData::FLAG_REACTION_STATE );
}


bool    CX2GUNPC::_IsReactionWaitState( char StateID )
{
    if ( StateID <= 0 || StateID >= (char) GetInitStateData().m_vecStateData.size() )
        return false;
    return GetInitStateData().m_vecStateData[StateID].m_bitsetFlags.test( StateData::FLAG_WAIT_STATE );
}

bool    CX2GUNPC::_IsDyingState( char StateID )
{
    if ( StateID == 0 )
        return false;
    return StateID == m_DyingLandFront
        || StateID == m_DyingLandBack
        || StateID == m_DyingSky
        || StateID == m_DyingFly
        ;
}


void    CX2GUNPC::AddExtraAbilityID( CX2UnitManager::NPC_ABILITY_ID npcAbilityID )
{
    m_vecExtraAbilityID.push_back( g_pData->GetUnitManager()->GetNPCExtraAbility( npcAbilityID ) );
}







// dmlee 2008.10.13 -  guard collision box 관련 변경 내용
//USE_GUARD_COLLISION_BOX = FALSE, 	<-- INIT_COMPONENT 안에, GUARD_* 로 이름지어진 collision 박스를 사용여부
//사용하지 않으면 앞에서 때렸는지 뒤에서 때렸는지만 구분한다, default = FALSE
//
//GUARD_DEFENCE           = 80,		<-- 각 개별 state안에, 방어 percent, 20% 데미지만 받는다
//GUARD_DEFENCE_FRONT	= TRUE,		<-- 앞에서 때린 것만 막는다, default = TRUE
//GUARD_DEFENCE_BACK	= TRUE,		<-- 뒤에서 때린 것만 막는다, default = TRUE
// 예) NASOD_GUARD, GLITTER_SHIELD 몬스터 스크립트 참고.



//#pragma NOTE( "박스를 재밌게 만들기 위해서 박스를 띄울 수 있도록 만들어봤습니다. 패치전에 몬스터 스크립트 수정하고 패치해야합니다." )


//static float s_fSpeed = 0.f;
//static float s_fRange = 0.f;
//static float s_fRetrieveAccel = 0.f;
//static float s_fDampingRate = 0.f;
//static int s_BounceCount = 0;
//

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
lua_State*	CX2GUNPC::GetFunctionLuaState()
{
	return	( m_pNPCUnitInfo->m_pLuaState != NULL ) ? m_pNPCUnitInfo->m_pLuaState : g_pKTDXApp->GetLuaBinder()->GetLuaState();
}
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING



//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
CX2GUNPC::CX2GUNPC( int unitIndex, int teamNum, int uid, const CX2UnitManager::NPCUnitInfo* pNPCUnitInfo,
			int eAIType, CX2GameUnit* pOwnerGameUnit_, float fUnitScale )
: CX2GameUnit( unitIndex, teamNum, ( pNPCUnitInfo->m_pLuaState != NULL ) ? NULL : pNPCUnitInfo->m_templet.m_LuaFileName.c_str() )
//, m_bLuaShared( false )
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//CX2GUNPC::CX2GUNPC( int unitIndex, int teamNum, const WCHAR* pScriptFileName, int uid, const CX2UnitManager::NPCUnitTemplet* pNPCTemplet,
//	int eAIType, CX2GameUnit* pOwnerGameUnit_ /*= NULL*/, float fUnitScale /*= -1.f*/ )
//: CX2GameUnit( unitIndex, teamNum, pScriptFileName )
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
, m_usSoundSet( 0 )
, m_bUnitFog( false )
{

	//if( NULL == pNPCTemplet )
    if ( pNPCUnitInfo == NULL || pNPCUnitInfo->m_pInitData == NULL )
		return;

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	if ( pNPCUnitInfo->m_pLuaState != NULL )
	{
		m_LuaManager.AssignNewLuaState( pNPCUnitInfo->m_pLuaState, 0, true );
		//m_bLuaShared = true;
	}
	else
	{
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
		//m_bLuaShared = false;
	}
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING

	m_UnitUID					= uid;
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
    m_pNPCUnitInfo               = pNPCUnitInfo;
    m_pInitData = &pNPCUnitInfo->m_pInitData->m_init;
//#else   X2OPTIMIZE_NPC_LUASPACE_SHARING
//	m_pNPCTemplet				= pNPCTemplet;
//#endif  X2OPTIMIZE_NPC_LUASPACE_SHARING

	//{{ 허상형 : [2011/2/27/] //	월드 미션(항상 Ally 상태인 NPC 처리)
	if( m_UnitUID != 0 && IsAllyNPCID( GetNPCTemplet().m_nNPCUnitID ) == true )
	{
		eAIType = CX2NPCAI::NAT_ALLY;

		m_TeamNum = CX2Room::TN_RED;
	}
	//}} 허상형 : [2011/2/27/] //	월드 미션(항상 Ally 상태인 NPC 처리)


	switch( eAIType )
	{
	default:
	case CX2NPCAI::NAT_NORMAL:
		{
			m_pAI = new CX2GUNPCAI( this );
		}break;

	case CX2NPCAI::NAT_ALLY:
		{
			CX2AllyNPCAI* pAllyAI = new CX2AllyNPCAI( this );
			pAllyAI->SetAllyGameUnit( pOwnerGameUnit_ );
			m_pAI = pAllyAI;

			//{{ kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)
			// 유저에 의해 소환된 몹이면
			SetRenderLv( false );
			//}} kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)

		} break;

//#ifdef TRANSFORMER_TEST
//	case CX2NPCAI::NAT_NO_BRAIN:
//		{
//			m_pAI = NULL;
//		} break;
//#endif TRANSFORMER_TEST
	}

	m_fScale = fUnitScale;
	CommonConstructor();

	//{{ kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)
	// 몬스터 생성 시에 비밀던전 이면Lv이 보이도록 함
	// 던전 타입으로 비교하지 않고 모드로 비교한 이유는 타입으로 비교하기 위해선 던전 id를 가지고 매번 map을 뒤져야 하기 때문임
	//if ( CX2UnitManager::NCT_BASIC == m_pNPCTemplet->m_ClassType )
	if ( CX2UnitManager::NCT_BASIC == GetNPCTemplet().m_ClassType )
	{
		const CX2PartyManager::PartyData* pPartyData = g_pData->GetPartyManager()->GetMyPartyData();
		if ( NULL != pPartyData &&
			( pPartyData->m_iDungeonMode == CX2Dungeon::DM_SECRET_NORMAL ||
			pPartyData->m_iDungeonMode == CX2Dungeon::DM_SECRET_HELL
			) )
			SetRenderLv( true );
	}
	//}} kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)

}

/** @function : 필드용 Constructor
	@brief : 배틀필드에서 생성될 NPC의 생성자
	@param : const KNPCUnitReq& kNpcUnitData_(서버로 부터 받은 패킷의 구조체), const CX2UnitManager::NPCUnitTemplet* pNPCTemplet
*/
CX2GUNPC::CX2GUNPC( const CNpcCreatedInfoPtr NpcCreatedInfoPtr_
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
    ,  const CX2UnitManager::NPCUnitInfo* pNPCUnitInfo
//#else   X2OPTIMIZE_NPC_LUASPACE_SHARING
//    , const CX2UnitManager::NPCUnitTemplet* pNPCTemplet
//#endif  X2OPTIMIZE_NPC_LUASPACE_SHARING
	)
	: CX2GameUnit( 0, NpcCreatedInfoPtr_->GetTeamNum(),
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
		( pNPCUnitInfo->m_pLuaState != NULL ) ? NULL : pNPCUnitInfo->m_templet.m_LuaFileName.c_str()
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	    pNPCTemplet->m_LuaFileName.c_str()
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
	)
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
//, m_bLuaShared( false )
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
, m_bUnitFog( false )
{
    if ( pNPCUnitInfo == NULL || pNPCUnitInfo->m_pInitData == NULL )
		return;

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	if ( pNPCUnitInfo->m_pLuaState != NULL )
	{
		m_LuaManager.AssignNewLuaState(  pNPCUnitInfo->m_pLuaState , 0, true );
		//m_bLuaShared = true;
	}
	else
	{
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
		//m_bLuaShared = false;
	}
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING

	m_pAI		= new CX2BattleFieldNpcAi( this, NpcCreatedInfoPtr_ );
	m_UnitUID	= NpcCreatedInfoPtr_->GetNpcUid();
	m_fScale	= NpcCreatedInfoPtr_->GetUnitScale();
	//m_pNPCTemplet = pNPCTemplet;
    m_pNPCUnitInfo = pNPCUnitInfo;
    m_pInitData = &pNPCUnitInfo->m_pInitData->m_init;
	CommonConstructor();
	m_KeyCode = NpcCreatedInfoPtr_->GetKeyCode();

	SetMonsterGrade( static_cast<MONSTER_GRADE>(NpcCreatedInfoPtr_->GetMonsterGrade()) );
}

CX2GUNPC::~CX2GUNPC(void)
{
	// 자신을 참조하는 몬스터들에게 더이상 나를 참조하지 말라고 설정한다.
	for(UINT i=0; i<m_vecConsultedNpc.size(); ++i)
	{
		if(m_vecConsultedNpc[i] != NULL)
			m_vecConsultedNpc[i]->ClearConsultNpc(this);
	}
	for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
	{
		if(m_vecConsultNpc[i] != NULL)
			m_vecConsultNpc[i]->ClearConsultedNpc(this);
	}
	SetUserGrapReset();

	for( CX2SubEquip* pSubEquip : m_vecAttachMesh )
	{
		SAFE_DELETE_KTDGOBJECT( pSubEquip );
	}
	m_vecAttachMesh.clear();

	//m_pNPCTemplet = NULL;
    m_pNPCUnitInfo = NULL;
    m_pInitData = NULL;
    m_vecStateRuntime.clear();

	SAFE_CLOSE( m_pMotion );
	SAFE_CLOSE( m_pAniXET );
	SAFE_CLOSE( m_pChangeTexXET );
	SAFE_CLOSE( m_pMultiTexXET );

    ////{{ seojt // 2009-1-12, 16:38
    //bool bTryDelete_m_pOrgXSkinAnim = false;
    //if( m_pXSkinAnim != m_pOrgXSkinAnim )
    //{
    //    bTryDelete_m_pOrgXSkinAnim = true;
    //}//if
    ////}} seojt // 2009-1-12, 16:38

	//SAFE_DELETE_KTDGOBJECT( m_pXSkinAnim );



	SAFE_DELETE( m_pAI );

    ////{{ seojt // 2009-1-12, 16:38
    //bool bTryDelete_m_pOrgAfterImage = false;
    //if( m_pAfterImage != m_pOrgAfterImage )
    //{
    //    bTryDelete_m_pOrgAfterImage = true;
    //}//if
    ////}} seojt // 2009-1-12, 16:38


	//SAFE_DELETE_KTDGOBJECT( m_pAfterImage );



	//if ( m_bChangedAppearance == true )
	//{
	//	SAFE_CLOSE( m_pOrgMotion );

 //       // 같은 포인터를 2번 지워서 crash될 가능성을 막는다.
 //       // - jintaeks on 2009-01-12, 16:40
 //       if( bTryDelete_m_pOrgXSkinAnim == true )
 //       {
	//	    CKTDGXSkinAnim::DeleteKTDGObject( m_pOrgXSkinAnim );
 //       }//if

 //       // 같은 포인터를 2번 지워서 crash될 가능성을 막는다.
 //       // - jintaeks on 2009-01-12, 16:40
 //       if( bTryDelete_m_pOrgAfterImage == true )
 //       {
	//	    CKTDGXSkinAfterImage::DeleteKTDGObject( m_pOrgAfterImage );
 //       }//if
	//	m_pOrgXSkinAnim  = NULL;
	//	m_pOrgAfterImage = NULL;
	//}

	//
	//m_pXSkinAnim.reset();

	DeleteArmagedonBlade();


	for( UINT i=0; i<m_vecpAttachedParticle.size(); i++ )
	{
		AttachedParticle* pAttachedParticle = m_vecpAttachedParticle[i];
		SAFE_DELETE( pAttachedParticle );
	}
	m_vecpAttachedParticle.clear();


	for( int i = 0; i < 10; i++ )
	{
		if( m_hMajorSeqList[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hMajorSeqList[i] );

		if( m_hMinorSeqList[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hMinorSeqList[i] );

		if( m_ahDamageEffect[i] != INVALID_DAMAGE_EFFECT_HANDLE )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_ahDamageEffect[i] );

		//{{ kimhc // 2010.4.19 // 비밀던전 작업
		if ( m_hEffectSetHandle[i] != INVALID_EFFECTSET_HANDLE )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectSetHandle[i] );
		//}} kimhc // 2010.4.19 // 비밀던전 작업
	}



	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqStickShake );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqStandUpAttackMark );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqAttribEnchant_Fire );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqAttribEnchant_Wind );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqSpecialAbility_Attack );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqSpecialAbility_Defend );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqSpecialAbility_Attack_Background );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqSpecialAbility_Defend_Background );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqQuestMonster );

	m_vecSoundPlayLoop.clear();
	for(int iSound=0; iSound<(int)m_vecLoopSound.size(); ++iSound)
	{
		CKTDXDeviceSound *pSound = m_vecLoopSound[iSound];
		if( pSound != NULL )
		{
			pSound->Stop();
		}
	}
	m_vecLoopSound.clear();

//{{ kimhc // 2010.7.7 // 몬스터가 생성한 라인맵
	m_vecMonsterLineMapIndex.clear();
//}} kimhc // 2010.7.7 // 몬스터가 생성한 라인맵


//	CX2GageManager::GetInstance()->DeleteBossGageUIByUID( GetUnitUID() );
	if( NULL != g_pX2Game &&
		null != m_optrOwnerGameUnit &&
		true == m_optrOwnerGameUnit->IsMyUnit())
	{
		g_pX2Game->InitFocusUnit();
	}

	for( int i =0; i < (int)m_ParticleEffData.size(); ++i )
	{
		ParticleEffectData* pParticleEffectData = m_ParticleEffData[i];
        ParticleEffectData::Destroy( pParticleEffectData );
	}
	m_ParticleEffData.clear();

    for( int i = 0; i < (int) m_MeshPlayerEffData.size(); ++i )
    {
        MeshPlayerEffectData* pMeshPlayerEffectData = m_MeshPlayerEffData[i];
        MeshPlayerEffectData::Destroy( pMeshPlayerEffectData );
    }
    m_MeshPlayerEffData.clear();

}

void CX2GUNPC::CommonConstructor()
{
    m_iFrameAccumCounter = 0;
    m_iFrameMoveIntervalCounter = 0;
    m_fOneShotElapsedTime = 0.f;


    m_v2ReflectMagic = D3DXVECTOR2(0,0);

    m_eDoubleAttack = DOUBLE_ATTACK_NONE;


    m_pFrame_Bip01_Head_Real = NULL;

	//m_bGlobalSuperArmor	= false;
	m_fGlobalTimeSuperArmor	= 0.f;

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_cbufReceiveDynamicPacket.resize(0);
    m_cbufReceiveDynamicPacket.set_capacity( 16 );
    m_pidHostUID = 0;
    m_dwLastProcessedFrameMoveCount = 0;

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	m_kLastStateChangeParts_Recv.Init();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


    m_cMindFlagBackup = 0;
    m_iLua_CommonCameraMoveRefID = LUA_REFNIL;
    m_iLua_CommonFrameMoveRefID = LUA_REFNIL;
    m_iLua_AttackedFuncRefID = LUA_REFNIL;

    m_kNonHostReaction.Init();





	m_fArmagedonBladeTime		= 0.f;
	m_hArmagedonBlade			= INVALID_MESH_INSTANCE_HANDLE;

	m_bBonusDrop				= false;

	m_bAfterImageAlways			= false;
	m_bAlwaysAlphaObject		= false;
	m_fDefaultPowerRate			= 1.f;

	m_bRenderLv					= false;

	m_bHoldOnlyOneUser			= true;
	m_bUnHoldWhenRevenged		= false;

	m_bDamageOutScreen			= true;


	//{{ JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)
	m_hSeqDrainHPEffect = INVALID_PARTICLE_SEQUENCE_HANDLE;
	//}}

	//{{ JHKang / 강정훈 / 2011/02/21 / 소환된 유닛이 NPC(몬스터)에게 준 데미지
	m_optrOwnerGameUnit.Reset();
	//}}

	m_iInvincibleToNPCKeyCode = 0;

	m_bAbsorbMagicState = false;
	m_bAbsorbMagicMonster = false;
	m_bAbsorbMagicSpecial = false;
	m_HyperModeColor = 0xffff5511;
	m_vecAttachMesh.clear();

	m_pTextureServer = NULL;
	if( m_UnitUID < -1 && g_pX2Room->IsNpcSlot(m_UnitUID) == true )
	{
		m_bPvpBot = true;
		m_pTextureServer = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_UI_Common_Texture11.tga" );
	}

	m_iDangerAlarm = -1;

	m_bISMonsterCard = false;
	m_fSummonTime	 = 0.f;				//InitComponent에서 받아온 해당 NPC의 소환 시간
	m_SummonEndState = 0;				//소환 해제 State


	m_bDeleteNPCBySubStageEnd = false;	/// 서브 스테이지 종료시 해당 NPC 삭제 여부

	m_bImmunityTimeStop	= false;		//타임 스톱 면역 기능 추가

	m_bStopAtStartState   = false;
	m_fShowBossNameDelayTime = -1.f;
	m_bIsApplyCinematicUI = true;

	m_colorName = 0xffffffff;
	m_colorOutLine = 0x00000000;

	m_iRelatedSkillLevel = 0;

	m_bCreatedBySyncNot			= false;
	m_bReceiveP2P				= false;
	m_bIsComeBackState			= false;
	m_bHasBossGage				= false;
	m_usNumOfGage				= 0;
	m_bFocusCamera				= false;
	m_wstrHyperBoostRightParticleName	= L"";
	m_wstrHyperBoostLeftParticleName	= L"";

	m_eObjectType = OT_CX2GUNPC;


	m_fTimePauseNPCAI			= 0.f;
	m_GameUnitType				= GUT_NPC;
	m_StartState				= 0;
	m_RebirthState				= 0;
	m_SelfDestructionState		= 0;
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_iSendReserveCount = 0;
	m_dwSendPacketFrame = 0;
//#else//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_bSendReserve				= false;
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_bImmediate = false;

	//유닛 컴포넌트
	m_pMotion					= NULL;
	m_pAniXET					= NULL;
	m_pChangeTexXET				= NULL;
	m_pMultiTexXET				= NULL;
	m_TexStage0Speed			= D3DXVECTOR2(0,0);
	m_TexStage1Speed			= D3DXVECTOR2(0,0);
	m_TexStage2Speed			= D3DXVECTOR2(0,0);
	m_ORGPos					= D3DXVECTOR3(0,0,0);

	m_AIMessage					= SCT_NO_CHANGE;
	m_UnUseAIMessage			= SCT_NO_CHANGE;

	m_bAIEnabled				= true;
	m_bDyingInSky				= false;
	m_bNeverMove				= false;

	m_bEnableCondtionTableEventProcess = true;
	m_bEnableEventProcess		= true;
	m_bIsActiveMonster			= true;
	m_AddRotateDegree			= D3DXVECTOR3(0,0,0);
	m_AddPos					= D3DXVECTOR3(0,0,0);

	m_fCoolTime					= 0.0f;
	m_KeyCode					= 0;
	m_HardLevel					= 0;

    m_eNPCUnitDieState = eNPCUnitDieState_NONE;
    m_fNPCUnitDieStateTimer = 0.f;
    m_bClientHoldPosition = false;

	m_fOutLineWidthScale		= 1.f;
	m_fMindFlagHeight			= 0.f;
	m_bDamagedThisFrameMove		= false;
	m_eDamageTypeThisFrameMove  = CX2DamageManager::RT_NO_DAMAGE;
	m_bShowOnMiniMap			= true;

	m_HyperModeCount			= 0;

	for( int i = 0; i < 10; i++ )
	{
		m_FlagList[i]		= false;
		m_IntList[i]		= 0;
//{{ robobeg : 2013-01-25
		INIT_VECTOR3( m_VecList[i], 0, 0, 0 );
//}} robobeg : 2013-01-25
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
		m_NumberList[i]     = 0.;
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
		m_hMajorSeqList[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hMinorSeqList[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

        m_ahDamageEffect[i]	= INVALID_DAMAGE_EFFECT_HANDLE;

		//{{ kimhc // 2010.4.19 // 비밀던전 작업
		m_hEffectSetHandle[i] = INVALID_EFFECTSET_HANDLE;
		//}} kimhc // 2010.4.19 // 비밀던전 작업

	}
	m_hSeqDieEffect					= INVALID_PARTICLE_SEQUENCE_HANDLE;

	//{{ megagame : 박교현 : [2010-04-06]
	m_HitCount					= 0;
	//}} megagame : 박교현 : [2010-04-06]


	m_RageCountMax					= 0;
	m_fRageTimeMax					= 0.0f;
	m_RageCount						= 0;
	m_fRageTime						= 0.0f;

//{{ oasis907 : 김상윤 [2010.4.21] //
	m_iHPRageCount					= 0;
	m_iRageHPPercent				= 0;
//}}

	m_fElapsedTimeLastGuide			= 1000.f;

	m_bSiegeMode					= false;
	m_bAddForceDownGageThisState	= false;
	m_fLastDamagePassTime			= 0.0f;
	m_fMaxDownForce = -9999.f;

	m_RageModeColor					= D3DXCOLOR( 1, 1, 1, 1 );
	m_bRageModeColorToRed			= true;
	m_fRageModeColorChangeTime		= 0.0f;

	m_bUseGuardCollisionBox			= false;
	m_bFallDown						= false;
    m_bSkyDie						= false;
	m_PacketSendCount				= 0;
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_bReceiveFirstSyncPacket			= false;
//#else//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_PacketRecvCount				= 0;
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	m_bFollowHitterLost				= false;
	m_iFollowHitter					= 0;
	m_HitterUID						= 0;
	m_eHitterUnitType				= CX2GameUnit::GUT_USER;


	m_bForceBillBoard				= false;
	m_hSeqStickShake				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqStandUpAttackMark			= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_fAnimWaitTime					= 0.0f;

	m_bShowBossName					= false;
	m_bShowSubBossName				= false;
	m_wstrBossNameTexture			= L"";

	m_fPrevHPGage					= 0.0f;
	m_DefenceRate					= 0;

	m_bSelfResurrection				= false;
	m_fSelfResurrectionTime			= 0.f;
	m_iSelfResurrectionCount		= 0;
	m_iSelfResurrectionHPPercent	= 0;

	m_bSelfDestruction				= false;
	m_fSelfDestructionTime			= -1.f;


	m_fDefaultAnimSpeed				= 1.f;
	m_fBornAnimSpeedRate			= 1.f;
	m_fAnimSpeedRate				= 1.f;
	m_fAnimSpeed					= 1.f;
	m_fTimedEventAnimSpeed			= 0.0f;		/// 0.0f 인 경우 사용 안함

	m_bColdSlowApplied				= false;
	m_bFrozenSlowApplied			= false;

    m_bGuard                        = false;

	m_bGgobong						= false;
	m_fTimeGgobong					= 0.0f;
	m_bOneTimeCheckGgobong			= true;

	m_RiderNpcUID					= 0;
	m_RidingNPCUID					= 0;

    float fx, fy, fz;
    fx = (float)(rand() % 2 + 1) + ((float)(rand() % 10) / 10.f);
    fy = 2.f + ((float)(rand() % 10) / 10.f);
    fz = (float)(rand() % 2 + 1) + ((float)(rand() % 10) / 10.f);

    if(rand()%2 == 0)
        fx *= -1.f;
    if(rand()%2 == 0)
        fz *= -1.f;

    m_vDieFly = D3DXVECTOR3(fx, fy, fz);
    m_bDieFly = false;

    D3DXVECTOR3 flyDir;
    D3DXVec3Normalize( &flyDir, &m_vDieFly );

    D3DXVECTOR3 u, v;
    u = D3DXVECTOR3(-1.f, 0.f, 0.f);
    v = flyDir;
    v.y= 0.f;
    D3DXVec3Normalize(&v, &v);
    fy = acos(D3DXVec3Dot(&v, &u));
    if(v.z < 0.f)
        fy *= -1.f;

    m_flyAngle = D3DXToDegree(fy);

	m_bAttributeEmotionFlag			= true;

	m_hSeqAttribEnchant_Fire		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqAttribEnchant_Wind		= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hSeqSpecialAbility_Attack		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqSpecialAbility_Defend		= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hSeqSpecialAbility_Attack_Background		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqSpecialAbility_Defend_Background		= INVALID_PARTICLE_SEQUENCE_HANDLE;


	m_bUnitColor					= false;

	m_bUnitFade						= false;
	m_fFadeEndTime					= 0.f;
	m_cFadeColor					= D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	m_bFadeScale					= false;
	m_fFadeScaleEndTime				= 0.f;
	m_vFadeScale					= D3DXVECTOR3(1.f, 1.f, 1.f);
	m_vFinalScale					= D3DXVECTOR3(1.f, 1.f, 1.f);

	m_vecConsultNpc.clear();
	m_vecConsultedNpc.clear();




//#ifdef MONSTER_DIE_PARTICLE_TEST
//	m_bCreatedDieParticle			= false;
//#endif MONSTER_DIE_PARTICLE_TEST

	m_fFlyHeight					= 0.f;

	m_bHold							= false;
	m_wstrHoldBone					= L"";

	m_vHoldOffSet					= D3DXVECTOR3( 0.f, 0.f, 0.f );


	m_bShowName						= true;

	m_fEntangleTrapAttackDuration	= 0.f;
	m_hSeqDrainParticle				= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_vNowSpeedBackup				= D3DXVECTOR2(0.f, 0.f);
	m_vpassiveSpeedBackup			= D3DXVECTOR2(0.f, 0.f);
	m_bDoneBackupSpeed				= false;

	m_strErrorMonsterStateName		= "";

	m_eRideState					=  CX2GUNPC::RS_NOT_AVAILABLE;
	m_optrRidingGameUnit.Reset();


	m_vecGameUnitoPtr.clear();
	m_bUserGrap						= false;

	m_vGrapPosition					= D3DXVECTOR3(0.f, 0.f, 0.f);


	//{{ JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)
	m_hSeqDrainHPEffect				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	//}}

	//{{ JHKang / 강정훈 / 2011/02/21 / 소환된 유닛이 NPC(몬스터)에게 준 데미지
	m_optrOwnerGameUnit.Reset();
	//}}

	m_iInvincibleToNPCKeyCode		= 0;

	m_bAbsorbMagicState				= false;
	m_bAbsorbMagicMonster			= false;
	m_HyperModeColor = 0xffff5511;
	m_vecAttachMesh.clear();

	m_pTextureServer = NULL;
	if( m_UnitUID < -1 && g_pX2Room->IsNpcSlot(m_UnitUID) == true )
	{
		m_bPvpBot = true;
		m_pTextureServer = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_UI_Common_Texture11.tga" );
	}

	m_iDangerAlarm = -1;

	m_colorName						= 0xffffffff;
	m_colorOutLine					= 0x00000000;

	m_iRelatedSkillLevel			= 0;

	m_bStopAtStartState				= false;
	m_fShowBossNameDelayTime		= -1.f;

	m_hSeqQuestMonster				= INVALID_PARTICLE_SEQUENCE_HANDLE; // 퀘스트 몬스터 표시 이펙트
	m_bIsQuestMonster				= false;
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuestNew() )
	{
		m_bIsQuestMonster = g_pData->GetUIManager()->GetUIQuestNew()->GetIsQuestMonster(GetNPCTemplet().m_nNPCUnitID);
	}

//#ifdef FIX_NPC_SHOW_OBJECT
//	m_bShowObjectCurrentState		= true;
//#endif //FIX_NPC_SHOW_OBJECT

	m_eNPCCreateType				= NCT_NONE;
	m_bNoRenderName					= false;
	m_bChangeRenderNameColor		= false;
	m_colorRenderName				= D3DXCOLOR( 0.f, 0.f, 0.f, 0.f );
	m_colorRenderNameOutLine		= D3DXCOLOR( 0.f, 0.f, 0.f, 0.f );


	m_bIsRemainingNpcWhenStageCleard = false;

	m_wstrEnterMonsterName = L"";

	m_eMonsterGrade = MG_NORMAL_NPC;


    m_eSoundSetType = SOUND_SET_DEFUALT;
    m_iEffectSetToPlayProgress = 0;
    m_iSpeedFactorProgress = 0;
    m_iCameraCrashProgress = 0;
    m_iStateInsertBuffFactorProgress = 0;
}

void CX2GUNPC::Init( bool bUseTeamPos, int startPosIndex )
{

	InitSystem();
	InitAI();
	InitDevice();
	InitMotion();
	InitPosition( bUseTeamPos, startPosIndex );
	//InitStat(); <- 빈 함수임
	InitPhysic();

	InitComponent();
	InitEffect();
	InitState();
	InitEtc();

	SetCenter( m_pMotion->GetCenter() );



	if(!m_bNotCull)
	{
		SetBoundingRadius( m_pMotion->GetBoundingRadius() );
	}

	g_pX2Game->GetNPCFunc()->SetNPC( this );
    StateChangeForce( m_StartState );


	m_bWaitingForRebirthAfterBungee = false;

	DeleteArmagedonBlade();

	InitAdditionalBasicStatValue();
	InitAdditionalStatOptionValue();
}


// 이 함수는 아이샤 헬드롭 스킬에만 사용하기 위해 만들었습니다
void CX2GUNPC::ScaleMoveSpeed( float fScale )
{
	ASSERT( fScale > 0.f );

	ASSERT( CX2UnitManager::NUI_UFO_AISHA == GetNPCTemplet().m_nNPCUnitID );
	if( CX2UnitManager::NUI_UFO_AISHA != GetNPCTemplet().m_nNPCUnitID )
		return;


	m_PhysicParam.fWalkSpeed      *= fScale;
	m_PhysicParam.fRunSpeed       *= fScale;
	m_PhysicParam.fJumpSpeed      *= fScale;
	m_PhysicParam.fDashJumpSpeed  *= fScale;
}


//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
void                        CX2GUNPC::ReInitExtraWork()
{
	m_iSendReserveCount = 0;
	m_bImmediate = false;
	m_dwSendPacketFrame = 0;
    m_dwLastProcessedFrameMoveCount = 0;
    m_cbufReceiveDynamicPacket.resize( 0 );
    m_pidHostUID  = 0;
    m_bClientHoldPosition = false;
    m_eNPCUnitDieState = eNPCUnitDieState_NONE;
    m_fNPCUnitDieStateTimer = 0.f;


    m_kNonHostReaction.Init();

    m_bPositionBackup = false;



    m_iFrameAccumCounter = 0;
    m_iFrameMoveIntervalCounter = 0;
    m_fOneShotElapsedTime = 0.f;

}
//#endif SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK




void CX2GUNPC::ReInit( bool bUseTeamPos, int startPosIndex )
{
	CX2GameUnit::ReInit( bUseTeamPos, startPosIndex );

	m_vecSoundPlayLoop.resize( 0 );
	for(int iSound=0; iSound<(int)m_vecLoopSound.size(); ++iSound)
	{
		CKTDXDeviceSound *pSound = m_vecLoopSound[iSound];
		if( pSound != NULL )
		{
			pSound->Stop();
		}
	}
	m_vecLoopSound.resize( 0 );

    ReInitExtraWork();

	m_fTimePauseNPCAI = 0.f;

	//유닛 컴포넌트
	m_ORGPos					= D3DXVECTOR3(0,0,0);

	m_AIMessage					= SCT_NO_CHANGE;
	m_UnUseAIMessage			= SCT_NO_CHANGE;
	m_bAIEnabled				= true;
	m_bDyingInSky				= false;
	m_bNeverMove				= false;

	m_bEnableEventProcess		= true;

	m_fCoolTime					= 0.0f;

	m_KeyCode					= 0;
	for( int i = 0; i < 10; i++ )
	{
		m_FlagList[i]		= false;
		m_IntList[i]		= 0;
		m_hMajorSeqList[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hMinorSeqList[i]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

		//{{ kimhc // 2010.4.19 // 비밀던전 작업
		m_hEffectSetHandle[i] = INVALID_EFFECTSET_HANDLE;
		//}} kimhc // 2010.4.19 // 비밀던전 작업

        m_ahDamageEffect[i]	= INVALID_DAMAGE_EFFECT_HANDLE;
	}

	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_NPCFrameData.unitCondition.fUnitWidth, 35.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_NPCFrameData.unitCondition.fUnitHeight, 150.f );

 		m_vOrgUnitSize.x = m_NPCFrameData.unitCondition.fUnitWidth;
 		m_vOrgUnitSize.y = m_NPCFrameData.unitCondition.fUnitHeight;

		m_LuaManager.EndTable();
	}

	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		float fBuffer = 0.0f;
		m_LuaManager.GetValue( "MAX_HP",				fBuffer );
		SetMaxHp( fBuffer );

		//{{ oasis : 김상윤 // 2009-10-6 ////////////////
		// 몬스터 HP 리젠 //
		m_LuaManager.GetValue( "HP_CHANGE_RATE",		fBuffer );
		SetChangeRateHp( fBuffer );
		//}} oasis : 김상윤 // 2009-10-6 ////////////////

		LUA_GET_VALUE(m_LuaManager, "HP_RELATIVE_CHANGE_RATE", fBuffer, 0.0f );
		if( fBuffer != 0.0f)
		{
			SetChangeRateHp( fBuffer );
			SetEnableHPRelativeChangeRate(true);
		}

		m_LuaManager.GetValue( "MP_CHANGE_RATE",		fBuffer );
		SetChangeRateMp( fBuffer );

		m_LuaManager.GetValue( "MP_CHARGE_RATE",		fBuffer );
		SetChangeRateChargeMpForDetonation( fBuffer );


		SetNowHp( GetMaxHp() );
		SetMaxMp( ONE_CHARGE * 3.f );

		m_fPrevHPGage = GetNowHp();

		GetGageData()->SetEnable( true );

		if( m_pAfterImage != NULL )
			m_pAfterImage->SetAfterImageColor( 0xffcccccc );

		m_LuaManager.EndTable();
	}
	m_pXSkinAnim->SetPlaySpeed( 1.f );


	m_bWaitingForRebirthAfterBungee = false;


	DeleteArmagedonBlade();
}


//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
/*static*/
void	CX2GUNPC::InitSystem( InitData& OutInit_, KLuaManager& luaManager_ )
{
	if( luaManager_.BeginTable( "INIT_SYSTEM" ) == true )
	{
		OutInit_.m_bLuaShareable = false;
		LUA_GET_VALUE( luaManager_, "LUA_SHAREABLE", OutInit_.m_bLuaShareable, false );

		OutInit_.m_bHeuristicSimulation = false;
		LUA_GET_VALUE( luaManager_, "HEURISTIC_SIMULATION", OutInit_.m_bHeuristicSimulation, false );

		OutInit_.m_bViewDepthHeuristic = true;
		LUA_GET_VALUE( luaManager_, "VIEWDEPTH_HEURISTIC", OutInit_.m_bViewDepthHeuristic, true );

		luaManager_.EndTable();
	}
}
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING


void CX2GUNPC::InitSystem()
{
	CX2GameUnit::InitSystem();

	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_NPCFrameData.unitCondition.fUnitWidth, 70.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_NPCFrameData.unitCondition.fUnitHeight, 150.f );

		m_vOrgUnitSize.x = m_NPCFrameData.unitCondition.fUnitWidth;
		m_vOrgUnitSize.y = m_NPCFrameData.unitCondition.fUnitHeight;

		m_NPCFrameData.syncData.nowState = GUSI_LOADING;

		if( -1.f == m_fScale )
		{
			LUA_GET_VALUE( m_LuaManager, "UNIT_SCALE", m_fScale, 1.f );
		}

		LUA_GET_VALUE( m_LuaManager, "FORCE_BILLBOARD", m_bForceBillBoard, false );

		LUA_GET_VALUE( m_LuaManager, "APPLY_EFFECTSET_SCALE", m_bScaleEffectSetByNpc, false );

		m_LuaManager.EndTable();
	}


}

void CX2GUNPC::InitDevice()
{
	CX2GameUnit::InitDevice();
}

void CX2GUNPC::InitAI()
{
	if( NULL != m_pAI )
	{
		m_pAI->LoadAIDataFromLUA( m_LuaManager );
	}
}

/*static*/
void CX2GUNPC::InitMotion( InitData& OutInit_, KLuaManager& luaManager_ )
{
	std::wstring	wstrName;



	if( luaManager_.BeginTable( "INIT_MOTION" ) == true )
	{
		if ( luaManager_.GetValue( "MOTION_FILE_NAME", wstrName ) == true )
			OutInit_.m_device.m_vecstrReadyXSkinMesh.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_FILE_NAME_ENEMY", wstrName ) == true )
			OutInit_.m_device.m_vecstrReadyXSkinMesh.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_CHANGE_TEX_XET", wstrName ) == true )
			OutInit_.m_device.m_vecstrReadyXET.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_ANI_TEX_XET", wstrName ) == true )
			OutInit_.m_device.m_vecstrReadyXET.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_MULTI_TEX_XET", wstrName ) == true )
			OutInit_.m_device.m_vecstrReadyXET.push_back( wstrName );

		luaManager_.EndTable();

	}
}


void CX2GUNPC::InitMotion()
{
    // HEAD_BONE_NAME 을 INIT_COMPONENT 에서 지정하는 문제를 일단은 이렇게 해결
    if ( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
    {
        LUA_GET_VALUE( m_LuaManager, "HEAD_BONE_NAME",	m_HeadBoneName, L"Bip01_Head" );
		m_LuaManager.EndTable();
    }


	if( m_LuaManager.BeginTable( "INIT_MOTION" ) == true )
	{
		wstring motion;
		LUA_GET_VALUE( m_LuaManager, "MOTION_FILE_NAME", motion, L"" );

		wstring motionEnemy;
		LUA_GET_VALUE( m_LuaManager, "MOTION_FILE_NAME_ENEMY", motionEnemy, L"" );
		if( motionEnemy != L"" && g_pX2Game->GetMyUnit() != NULL && g_pX2Game->GetMyUnit()->GetTeam() != GetTeam() )
		{
			m_pMotion			= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motionEnemy.c_str() );
		}
		else
		{
			m_pMotion			= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( motion.c_str() );
		}

        ASSERT( m_pMotion );
		if ( m_pMotion == NULL )
		{
			StateLog( m_ScriptFileName.c_str() );
			StateLog( motion.c_str() );
			//MessageBox( g_pKTDXApp->GetHWND(), motion.c_str(), L"모션 파일 읽어오기 실패!", MB_OK );
		}
		//m_pOrgMotion = m_pMotion;



		wstring changeTex;
		LUA_GET_VALUE( m_LuaManager, "MOTION_CHANGE_TEX_XET", changeTex, L"" );
		SAFE_CLOSE( m_pChangeTexXET );
		m_pChangeTexXET		= g_pKTDXApp->GetDeviceManager()->OpenXET( changeTex.c_str() );

		wstring aniTex;
		LUA_GET_VALUE( m_LuaManager, "MOTION_ANI_TEX_XET", aniTex, L"" );
		SAFE_CLOSE( m_pAniXET );
		m_pAniXET		= g_pKTDXApp->GetDeviceManager()->OpenXET( aniTex.c_str() );

		wstring multiTex;
		LUA_GET_VALUE( m_LuaManager, "MOTION_MULTI_TEX_XET", multiTex, L"" );
		SAFE_CLOSE( m_pMultiTexXET );
		m_pMultiTexXET		= g_pKTDXApp->GetDeviceManager()->OpenXET( multiTex.c_str() );

		if( m_LuaManager.BeginTable( "MULTI_TEX_SPEED" ) == true )
		{
			int stage;
			if( m_LuaManager.GetValue( 1, stage ) == true )
			{
				switch( stage )
				{
					case 0:
						{
							m_LuaManager.GetValue( 2, m_TexStage0Speed.x );
							m_LuaManager.GetValue( 3, m_TexStage0Speed.y );
						}
						break;

					case 1:
						{
							m_LuaManager.GetValue( 2, m_TexStage1Speed.x );
							m_LuaManager.GetValue( 3, m_TexStage1Speed.y );
						}
						break;

					case 2:
						{
							m_LuaManager.GetValue( 2, m_TexStage2Speed.x );
							m_LuaManager.GetValue( 3, m_TexStage2Speed.y );
						}
						break;
				}
			}

			m_LuaManager.EndTable();
		}

		LUA_GET_VALUE( m_LuaManager, "ADD_ROTATE_X", m_AddRotateDegree.x, 0.0f );
		LUA_GET_VALUE( m_LuaManager, "ADD_ROTATE_Y", m_AddRotateDegree.y, 0.0f );
		LUA_GET_VALUE( m_LuaManager, "ADD_ROTATE_Z", m_AddRotateDegree.z, 0.0f );

		CreateAndSetAinmXSkinMesh();



		m_vBasicScale = D3DXVECTOR3( m_fScale, m_fScale, m_fScale );


		m_vBasicScale.x *= GetScaleByUnit().x;
		m_vBasicScale.y *= GetScaleByUnit().y;
		m_vBasicScale.z *= GetScaleByUnit().z;



		m_vTransScale = m_vBasicScale;

		//FieldFix: 이게 모하는 짓인가...



		m_LuaManager.EndTable();
	}
}

void CX2GUNPC::InitEnchantData()
{
	if( m_LuaManager.BeginTable( "INIT_ENCHANT" ) == true )
	{
		m_mapDefenceEnchantData.clear();

		int resistPercent = 0;
		LUA_GET_VALUE( m_LuaManager, "ET_BLAZE",	resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_BLAZE, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_BLAZE, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "ET_WATER",	resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_WATER, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_WATER, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "ET_NATURE",	resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_NATURE, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_NATURE, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "ET_WIND",		resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_WIND, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_WIND, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "ET_LIGHT",	resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_LIGHT, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair(CX2EnchantItem::ET_LIGHT, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "ET_DARK",		resistPercent, 0 );
		if ( resistPercent > 0 )
		{
			if ( resistPercent == 100 )
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_DARK, CX2EnchantItem::EAR_MAX_VALUE ) );
			else
			{
				m_mapDefenceEnchantData.insert( std::make_pair( CX2EnchantItem::ET_DARK, (int)((float)resistPercent / 100.0f * (float)CX2EnchantItem::EAR_MAX_VALUE) ) );
			}
		}

        m_LuaManager.EndTable();
	}
}

void CX2GUNPC::InitEffect()
{
	CX2GameUnit::InitEffect();
}

void CX2GUNPC::DeleteMinorParticle()
{
	CX2GameUnit::DeleteMinorParticle();
}


bool CX2GUNPC::IsImmuneToEnchant( CX2EnchantItem::ENCHANT_TYPE enchantType )
{
	if( true == GetIsImmuneAllEDT() )
		return true;

	if ( CX2GameUnit::IsImmuneToEnchant( enchantType ) == true )
		return true;

	if( GetRandomFloat( CKTDXRandomNumbers::SRO_RESIST_EXTRA_DAMAGE_ENCHANT ) < GetEnchantResist( enchantType ) / (float)CX2EnchantItem::EAR_MAX_VALUE )
		return true;

	return false;
}


/*static*/
void    CX2GUNPC::AppendEnchantMonsterResourcesToDeviceList( CKTDXDeviceDataList& listInOut_, const KAttribEnchantNpcInfo& kAttribEnchantNpcInfo_  )
{
	if ( kAttribEnchantNpcInfo_.IsValidInfo() == false )
		return;

	vector< CX2UnitManager::NPC_ABILITY_ID >	vecExtraAbilityID;
	vector< CX2EnchantItem::ENCHANT_TYPE >		vecEnchantType;

	if ( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility1 != 0 )
		vecExtraAbilityID.push_back( static_cast<CX2UnitManager::NPC_ABILITY_ID>( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility1 ) );

	if ( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility2 != 0 )
		vecExtraAbilityID.push_back( static_cast<CX2UnitManager::NPC_ABILITY_ID>( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility2 ) );

	if ( kAttribEnchantNpcInfo_.m_cAttribEnchant1 != 0 )
		vecEnchantType.push_back( static_cast<CX2EnchantItem::ENCHANT_TYPE>( kAttribEnchantNpcInfo_.m_cAttribEnchant1 ) );

	if ( kAttribEnchantNpcInfo_.m_cAttribEnchant2 != 0 )
		vecEnchantType.push_back( static_cast<CX2EnchantItem::ENCHANT_TYPE>( kAttribEnchantNpcInfo_.m_cAttribEnchant2 ) );


	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;
	for ( int i = 0; i < (int)vecExtraAbilityID.size(); i++ )
	{
		 pNPCExtraAbility = g_pData->GetUnitManager()->GetNPCExtraAbility( vecExtraAbilityID[i] );
		if( NULL == pNPCExtraAbility )
			continue;
		g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Defend_BG" );
		if ( pNPCExtraAbility->m_fPhysicDefensePercentUp != 0 )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Defend_A" );
		if ( pNPCExtraAbility->m_fMagicDefensePercentUp != 0 )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Defend_B" );
		if ( pNPCExtraAbility->m_bHeavy == true )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Defend_D" );
		if ( pNPCExtraAbility->m_bAlwaysSuperArmor == true )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Defend_C" );
		g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Attack_BG" );
		if ( pNPCExtraAbility->m_fPhysicAttackPercentUp != 0 )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Attack_A" );
		if ( pNPCExtraAbility->m_fMagicAttackPercentUp != 0 )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Attack_C" );
		if ( pNPCExtraAbility->m_fCriticalPercent > 0.0f )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Attack_B" );
		if ( pNPCExtraAbility->m_fAllSpeedUpPercentUp > 0.0f )
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Special_Ability_Attack_D" );
	}
	// 속성 인챈트
	CX2DamageManager::EXTRA_DAMAGE_TYPE eWeaponEnchantExtraDamageType = g_pData->GetEnchantItem()->GetExtraDamageType( vecEnchantType );


	// 첫번째 이펙트 파티클
	switch(eWeaponEnchantExtraDamageType)
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark01_Fire" );
		} break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark01_Ice" );
		} break;
	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark01_Poison" );
		} break;
	default:
		break;
	}

	// 두번째 이펙트 파티클
	switch(eWeaponEnchantExtraDamageType)
	{
	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark02_Wind" );
		} break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark02_Light" );
		} break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
		{
			g_pX2Game->GetMajorParticle()->AppendToDeviceList( listInOut_, L"Monster_Prop_Mark02_Curse" );
		} break;
	default:
		break;
	}

}//CX2GUNPC::InitComponentData::AppendToDeviceList()



void CX2GUNPC::ApplyEnchantMonster( const KAttribEnchantNpcInfo& kAttribEnchantNpcInfo_ )
{
	if ( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility1 != 0 )
		AddExtraAbilityID( static_cast<CX2UnitManager::NPC_ABILITY_ID>( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility1 ) );

	if ( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility2 != 0 )
		AddExtraAbilityID( static_cast<CX2UnitManager::NPC_ABILITY_ID>( kAttribEnchantNpcInfo_.m_cExtraSpecialAbility2 ) );

	if ( kAttribEnchantNpcInfo_.m_cAttribEnchant1 != 0 )
		AddEnchantType( static_cast<CX2EnchantItem::ENCHANT_TYPE>( kAttribEnchantNpcInfo_.m_cAttribEnchant1 ) );

	if ( kAttribEnchantNpcInfo_.m_cAttribEnchant2 != 0 )
		AddEnchantType( static_cast<CX2EnchantItem::ENCHANT_TYPE>( kAttribEnchantNpcInfo_.m_cAttribEnchant2 ) );

	m_fBornAnimSpeedRate += GetActionSpeedUpValue();

	if ( (int)m_vecExtraAbilityID.size() >= 2 || (int)m_vecEnchantType.size() >= 2 )
	{
		//상급 엘리트, HP를 3배로
		float maxHP = GetMaxHp();
		SetMaxHp( maxHP * 3.f );
		SetNowHp( maxHP * 3.f );
	}
	else if ( (int)m_vecExtraAbilityID.size() >= 1 || (int)m_vecEnchantType.size() >= 1  )
	{
		//하급 엘리트, HP를 2배로
		float maxHP = GetMaxHp();
		SetMaxHp( maxHP * 2.f );
		SetNowHp( maxHP * 2.f );
	}

	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;

	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
	{
        pNPCExtraAbility = m_vecExtraAbilityID[ i ];
		if( NULL == pNPCExtraAbility )
			continue;


		if(m_hSeqSpecialAbility_Defend_Background == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqSpecialAbility_Defend_Background = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Defend_BG", 0, 0, 0);
		}

		if ( pNPCExtraAbility->m_fPhysicDefensePercentUp != 0 )
		{
			AccessStat().AccessStat().m_fDefPhysic *= (1.0f + pNPCExtraAbility->m_fPhysicDefensePercentUp );
			m_AdditionalPhysicDefence.SetBasicStatValue( GetStat().GetStat().m_fDefPhysic );

			if ( m_hSeqSpecialAbility_Defend == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Defend = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Defend_A", 0, 0, 0 );
			}
		}

		if ( pNPCExtraAbility->m_fMagicDefensePercentUp != 0 )
		{
			AccessStat().AccessStat().m_fDefMagic *= (1.0f + pNPCExtraAbility->m_fMagicDefensePercentUp );
			m_AdditionalMagicDefence.SetBasicStatValue( GetStat().GetStat().m_fDefMagic );

			if ( m_hSeqSpecialAbility_Defend == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Defend = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Defend_B", 0, 0, 0 );
			}
		}
		if ( pNPCExtraAbility->m_bHeavy == true )
		{
			if ( m_hSeqSpecialAbility_Defend == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Defend = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Defend_D", 0, 0, 0 );
			}
		}


		if ( pNPCExtraAbility->m_bAlwaysSuperArmor == true )
		{
			if ( m_hSeqSpecialAbility_Defend == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Defend = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Defend_C", 0, 0, 0 );
			}
		}
		if(m_hSeqSpecialAbility_Attack_Background == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			m_hSeqSpecialAbility_Attack_Background = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Attack_BG", 0, 0, 0);
		}

		if ( pNPCExtraAbility->m_fPhysicAttackPercentUp != 0 )
		{
			//AccessStat().AccessStat().m_fAtkPhysic += GetStat().GetStat().m_fAtkPhysic * (1.0f + pNPCExtraAbility->m_fPhysicAttackPercentUp );
            AccessStat().AccessStat().m_fAtkPhysic *= 2.0f + pNPCExtraAbility->m_fPhysicAttackPercentUp;
			m_AdditionalPhysicAttack.SetBasicStatValue( GetStat().GetStat().m_fAtkPhysic );

			if ( m_hSeqSpecialAbility_Attack == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Attack = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Attack_A", 0, 0, 0 );
			}
		}

		if ( pNPCExtraAbility->m_fMagicAttackPercentUp != 0 )
		{
			//AccessStat().AccessStat().m_fAtkMagic += GetStat().GetStat().m_fAtkMagic * (1.0f + pNPCExtraAbility->m_fMagicAttackPercentUp );
            AccessStat().AccessStat().m_fAtkMagic *= 2.0f + pNPCExtraAbility->m_fMagicAttackPercentUp;
			m_AdditionalMagicAttack.SetBasicStatValue( GetStat().GetStat().m_fAtkMagic );

			if ( m_hSeqSpecialAbility_Attack == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Attack = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Attack_C", 0, 0, 0 );
			}
		}

		if ( pNPCExtraAbility->m_fCriticalPercent > 0.0f )
		{
			/// 옵션 수치화로 적용
			const float fOptionCritical = pNPCExtraAbility->m_fCriticalPercent * ( 1.5f * (GetUnitLevel() -1) + 9.5f );
			m_AdditionalCriticalRate.SetOptionValue( fOptionCritical );

			if ( m_hSeqSpecialAbility_Attack == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Attack = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Attack_B", 0, 0, 0 );
			}
		}

		if ( pNPCExtraAbility->m_fAllSpeedUpPercentUp > 0.0f )
		{
			if ( m_hSeqSpecialAbility_Attack == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				m_hSeqSpecialAbility_Attack = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Special_Ability_Attack_D", 0, 0, 0 );
			}
		}
	}


	// 속성 인챈트
	m_eWeaponEnchantExtraDamageType = g_pData->GetEnchantItem()->GetExtraDamageType( m_vecEnchantType );


	// 첫번째 이펙트 파티클
	switch(m_eWeaponEnchantExtraDamageType)
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
		{
			m_hSeqAttribEnchant_Fire = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark01_Fire", 0, 0, 0 );
		} break;
	case CX2DamageManager::EDT_ENCHANT_FROZEN:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
		{
			m_hSeqAttribEnchant_Fire = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark01_Ice", 0, 0, 0 );
		} break;
	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
		{
			m_hSeqAttribEnchant_Fire = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark01_Poison", 0, 0, 0 );
		} break;
	default:
		break;
	}




	// 두번째 이펙트 파티클
	switch(m_eWeaponEnchantExtraDamageType)
	{
	case CX2DamageManager::EDT_ENCHANT_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:
		{
			m_hSeqAttribEnchant_Wind = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark02_Wind", 0, 0, 0 );
		} break;
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK:
		{
			m_hSeqAttribEnchant_Wind = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark02_Light", 0, 0, 0 );
		} break;
	case CX2DamageManager::EDT_ENCHANT_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:
	case CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH:
		{
			m_hSeqAttribEnchant_Wind = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Monster_Prop_Mark02_Curse", 0, 0, 0 );
		} break;
	default:
		break;
	}


}

void CX2GUNPC::ShowSnatchCureEffect()
{
	D3DXVECTOR3 hpAndMpUpPos = GetPos();
	hpAndMpUpPos.y += 80.0f;

	if( m_hSeqHPUp == INVALID_PARTICLE_SEQUENCE_HANDLE )
		m_hSeqHPUp = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Plus_Hp01", hpAndMpUpPos );

	if( m_hSeqMPUp == INVALID_PARTICLE_SEQUENCE_HANDLE )
		m_hSeqMPUp = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Plus_Mp01", hpAndMpUpPos );

	PlaySound( L"Drain_HPMP.ogg" );
}


void CX2GUNPC::SetPositionForce( const D3DXVECTOR3& vPosition )
{
	KTDXPROFILE();

	m_ORGPos = m_NPCFrameData.syncData.position	= vPosition;
	m_NPCFrameData.unitCondition.landPosition	= vPosition;

	BackupPosition();
}



bool CX2GUNPC::SetPosition( const D3DXVECTOR3& vPos, bool bIsRight )
{
	KTDXPROFILE();

	if( NULL == g_pX2Game ||
		NULL == g_pX2Game->GetWorld() ||
		NULL == g_pX2Game->GetWorld()->GetLineMap() )
	{
		ASSERT( !"CX2GUNPC::SetPosition, null world" );
		return false;
	}

	CKTDGLineMap*	pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	D3DXVECTOR3		startPos;

	startPos = vPos;

	//정확한 라인 보정위치와 라인 인덱스를 구한다
	int lineIndex = -1;
	D3DXVECTOR3	landPos = startPos;
	landPos = pLineMap->GetLandPosition( startPos, LINE_RADIUS, &lineIndex );
	//pLineMap->IsOnLineConnect( startPos, &lineIndex, LINE_RADIUS, &landPos );
	pLineMap->IsOnLine( startPos, LINE_RADIUS, &startPos, &lineIndex, true );

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( lineIndex );
	if( NULL == pLineData )
	{
		ASSERT( !"CX2GUNPC::SetPosition, null linedata" );
		return false;
	}
	//startPos = landPos;

	//최초 프레임 데이타에 업데이트 한다.
	m_NPCFrameData.syncData.position				= startPos;
    m_NPCFrameData.syncData.bIsRight                = true;
	m_NPCFrameData.unitCondition.dirDegree			= pLineData->dirDegree;
	m_NPCFrameData.unitCondition.dirVector			= pLineData->dirVector;
	m_NPCFrameData.unitCondition.landPosition		= landPos;
	m_NPCFrameData.syncData.lastTouchLineIndex		= lineIndex;
    SetIsRightNow( bIsRight );

	return true;
}





void CX2GUNPC::SetPositionOnLine( const D3DXVECTOR3& vPosition, const int iLineIndex )
{
	ASSERT( NULL != g_pX2Game );
	ASSERT( NULL != g_pX2Game->GetWorld() );
	ASSERT( NULL != g_pX2Game->GetWorld()->GetLineMap() );

	if( NULL == g_pX2Game )
		return;

	if( NULL == g_pX2Game->GetWorld() )
		return;

	if( NULL == g_pX2Game->GetWorld()->GetLineMap() )
		return;



	const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( iLineIndex );
	ASSERT( NULL != pLineData );
	if( NULL == pLineData )
		return;


	// 던져지는 캐릭터의 위치를 라인맵 위로 옮겨준다
	D3DXVECTOR3 vPlaneNormal;
	D3DXVec3Cross( &vPlaneNormal, &pLineData->dirVector, &Y_AXIS );
	//D3DXVec3Normalize( &vPlaneNormal, &vPlaneNormal );
	D3DXVECTOR3 vPlaneVertex = pLineData->startPos;

	D3DXVECTOR3 vRayStart = vPosition - vPlaneNormal * 100000.f;

	float fDistance = 0.f;
	D3DXVECTOR3 vIntersection( 0, 0, 0 );
	bool bIntesected = g_pKTDXApp->GetCollision()->RayToPlane( fDistance, vIntersection, vRayStart, vPlaneNormal, vPlaneVertex, vPlaneNormal );
	ASSERT( true == bIntesected );
	if( false == bIntesected )
		return;

    bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
	m_NPCFrameData.syncData.position = vIntersection;
	m_NPCFrameData.syncData.lastTouchLineIndex = iLineIndex;
    m_NPCFrameData.syncData.bIsRight = true;
	m_NPCFrameData.unitCondition.dirDegree = pLineData->dirDegree;
	m_NPCFrameData.unitCondition.dirVector = pLineData->dirVector;
	m_NPCFrameData.unitCondition.landPosition = vIntersection;

    SetIsRightNow( bOldIsRight );
}




void CX2GUNPC::SetRotateDegree( const D3DXVECTOR3& vDegree )
{
	m_NPCFrameData.unitCondition.dirDegree		= vDegree;

	GetMatrix().RotateDegree( vDegree );

	if ( NULL != m_pXSkinAnim )
		m_pXSkinAnim->GetMatrix().RotateDegree( vDegree );
}

void CX2GUNPC::InitPosition( bool bUseTeamPos, int startPosIndex )
{
	KTDXPROFILE();

    m_kNonHostReaction.ResetReservedReceivePos();
    m_bPositionBackup = false;
    m_kNonHostReaction.ResetRelativePosAndTimers();
    m_kNonHostReaction.ResetLocalRelativePosAndTimers();
    m_kNonHostReaction.ClearBuffPositionHistory();

	if( NULL == g_pX2Game->GetWorld() ||
		NULL == g_pX2Game->GetWorld()->GetLineMap() )
	{
		//최초 프레임 데이타에 업데이트 한다.
		m_NPCFrameData.syncData.position			= D3DXVECTOR3(0,0,0);
		m_NPCFrameData.unitCondition.dirDegree		= D3DXVECTOR3(0,0,0);
		m_NPCFrameData.unitCondition.landPosition	= D3DXVECTOR3(0,0,0);
		m_NPCFrameData.syncData.lastTouchLineIndex	= 0;
		m_NPCFrameData.syncData.bIsRight			= true;
		m_NPCFrameData.unitCondition.dirDegree		= D3DXVECTOR3(0,0,0);
		return;
	}




	CKTDGLineMap*	pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	D3DXVECTOR3		startPos;
	int lineIndex = -1;


	if ( bUseTeamPos == true )
	{
		if ( startPosIndex == -1 )	//직접 어디에서 태어날지 정해주지 않은경우
		{
			if( m_TeamNum == CX2Room::TN_RED )
			{
				startPos								= pLineMap->GetRedTeamStartPosition( m_UnitIndex );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetRedTeamStartRight( m_UnitIndex );
				lineIndex								= pLineMap->GetRedTeamStartLineIndex( m_UnitIndex );
			}
			else if( m_TeamNum == CX2Room::TN_BLUE )
			{
				startPos								= pLineMap->GetBlueTeamStartPosition( m_UnitIndex - 4 );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetBlueTeamStartRight( m_UnitIndex - 4 );
				lineIndex								= pLineMap->GetBlueTeamStartLineIndex( m_UnitIndex - 4 );
			}
			else
			{
				startPos								= pLineMap->GetStartPosition( m_UnitIndex );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetStartRight( m_UnitIndex );
				lineIndex								= pLineMap->GetStartLineIndex( m_UnitIndex );
			}
		}
		else
		{
			if( m_TeamNum == CX2Room::TN_RED )
			{
				startPos								= pLineMap->GetRedTeamStartPosition( startPosIndex );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetRedTeamStartRight( startPosIndex );
				lineIndex								= pLineMap->GetRedTeamStartLineIndex( startPosIndex );
			}
			else if( m_TeamNum == CX2Room::TN_BLUE )
			{
				startPos								= pLineMap->GetBlueTeamStartPosition( startPosIndex );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetBlueTeamStartRight( startPosIndex );
				lineIndex								= pLineMap->GetBlueTeamStartLineIndex( startPosIndex );
			}
			else
			{
				startPos								= pLineMap->GetStartPosition( startPosIndex );
				m_NPCFrameData.syncData.bIsRight		= pLineMap->GetStartRight( startPosIndex );
				lineIndex								= pLineMap->GetStartLineIndex( startPosIndex );
			}
		}
	}
	else
	{
		if ( startPosIndex != -1 )
		{
			startPos								= pLineMap->GetStartPosition( startPosIndex );
			m_NPCFrameData.syncData.bIsRight		= pLineMap->GetStartRight( startPosIndex );
			lineIndex								= pLineMap->GetStartLineIndex( startPosIndex );
		}
		else
		{
            int index = pLineMap->GetRandomStartPosIndex();
			startPos								= pLineMap->GetStartPosition( index );
			m_NPCFrameData.syncData.bIsRight		= pLineMap->GetStartRight( index );
			lineIndex								= pLineMap->GetStartLineIndex( index );
		}
	}

	// 시작 위치를 찾을 수 없다면
	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( lineIndex );
	if( NULL == pLineData )
	{
		D3DXVECTOR3 vPos = pLineMap->GetRandomPosition( NULL, 99999.f, false );
		int iLineIndex = 0;
		vPos = pLineMap->GetLandPosition( vPos, LINE_RADIUS, &iLineIndex );
		const CKTDGLineMap::LineData* pTempLineData = pLineMap->GetLineData( iLineIndex );

		if( NULL != pTempLineData )
		{
			//최초 프레임 데이타에 업데이트 한다.
            bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
			m_NPCFrameData.syncData.position				= vPos;
            m_NPCFrameData.syncData.bIsRight = true;
			m_NPCFrameData.unitCondition.dirDegree			= pTempLineData->dirDegree;
			m_NPCFrameData.unitCondition.landPosition		= vPos;
			m_NPCFrameData.syncData.lastTouchLineIndex		= iLineIndex;
            SetIsRightNow( bOldIsRight );
		}
		else
		{
			// 그래도 위치 정보를 초기화 할 수 없다면, 외부에서 위치를 설정해줄 것으로 여기고, 여기에서 위치정보를 update하지 않는다.
			m_NPCFrameData.syncData.position				= D3DXVECTOR3( 0, 0, 0 );
			m_NPCFrameData.unitCondition.dirDegree			= D3DXVECTOR3( 0, 0, 0 );
			m_NPCFrameData.unitCondition.landPosition		= D3DXVECTOR3( 0, 0, 0 );
			m_NPCFrameData.syncData.lastTouchLineIndex		= 0;
		}
	}
	else
	{
		D3DXVECTOR3	landPos = startPos;
		startPos = landPos;
		startPos += m_AddPos;

		//최초 프레임 데이타에 업데이트 한다.
        bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
		m_NPCFrameData.syncData.position				= startPos;
        m_NPCFrameData.syncData.bIsRight                = true;
		m_NPCFrameData.unitCondition.dirDegree			= pLineData->dirDegree;
		m_NPCFrameData.unitCondition.landPosition		= startPos;
		m_NPCFrameData.syncData.lastTouchLineIndex		= lineIndex;
        SetIsRightNow( bOldIsRight );
	}
}

/*static*/
void CX2GUNPC::InitComponent( InitData& OutInit_, KLuaManager& luaManager_ )
{
	CX2GameUnit::InitComponent( OutInit_.m_device, luaManager_ );

	if( luaManager_.BeginTable( "INIT_COMPONENT" ) == true )
	{
		LUA_GET_VALUE( luaManager_, "DEFAULT_ANIM_SPEED", OutInit_.m_component.m_fDefaultAnimSpeed, 1.f );

		//OutComponent_.m_vecWeapon.reserve( luaManager_.GetNumIndexedTables( "WEAPON" ) );
		int iWeaponIndex = 0;
		while( true == luaManager_.BeginTable( "WEAPON", iWeaponIndex ) )
		{
			CX2GameUnit::InitWeaponDataForNPC( OutInit_.m_device, luaManager_ );
			luaManager_.EndTable(); // WEAPON%d
			iWeaponIndex += 1;
		}

		luaManager_.EndTable();
	}
}


void CX2GUNPC::InitComponent()
{
	CX2GameUnit::InitComponent();

	//능력치 세팅
    const CX2UnitManager::NPCUnitStat& kNPCUnitStat = GetNPCUnitStat();
    //if ( GetNPCTemplet() != NULL )
	{
        //const CX2UnitManager::NPCUnitStat* pNPCUnitStat = g_pData->GetUnitManager()->GetNPCUnitStat( GetNPCTemplet().m_nNPCUnitID );
		//if( pNPCUnitStat != NULL )
		{
			// megagame / 박교현 / 2010.04.30 / NPC 스탯 추가
			SetNPCStat(kNPCUnitStat);
		}
		//else
		//{
		//	// npcstat이 없네. 오류다!!
		//	AccessStat().AccessStat().m_fBaseHP		= 1.f;
		//	SetMaxHp( 1.f );
		//}
	}

	if( IsPvpBot() == true )
	{
		CX2Room::RoomNpcSlot *pNpcSlotData = g_pX2Room->GetNpcSlotData( GetUID() );
		if( pNpcSlotData != NULL )
			SetNPCStat( pNpcSlotData->m_fBaseHP, pNpcSlotData->m_fAtkPhysic, pNpcSlotData->m_fAtkMagic, pNpcSlotData->m_fDefPhysic, pNpcSlotData->m_fDefMagic );
	}

	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		float fBuffer = 0.0f;

		LUA_GET_VALUE( m_LuaManager, "DEFAULT_ANIM_SPEED", m_fDefaultAnimSpeed, 1.f );



		m_vecImmuneSkill.resize(0);
		if( m_LuaManager.BeginTable( "IMMUNE_SKILL_LIST" ) == true )
		{
			int index = 1;
			int iSkillID = -1;

			while( m_LuaManager.GetValue( index, iSkillID ) == true )
			{
				if( iSkillID > 0 )
				{
					m_vecImmuneSkill.push_back( (CX2SkillTree::SKILL_ID) iSkillID );
				}
				index++;
			}

			m_LuaManager.EndTable(); // IMMUNE_SKILL_LIST
		}
		//{{ oasis : 김상윤 // 2009-10-6 ////////////////
		// 몬스터 HP 리젠 //
		m_LuaManager.GetValue( "HP_CHANGE_RATE",		fBuffer );
		SetChangeRateHp( fBuffer );
		//}} oasis : 김상윤 // 2009-10-6 ////////////////

		LUA_GET_VALUE(m_LuaManager, "HP_RELATIVE_CHANGE_RATE", fBuffer, 0.0f );
		if( fBuffer != 0.0f)
		{
			SetChangeRateHp( fBuffer );
			SetEnableHPRelativeChangeRate(true);
		}

		m_LuaManager.GetValue( "MP_CHANGE_RATE",		fBuffer );
		SetChangeRateMp( fBuffer );

		m_LuaManager.GetValue( "MP_CHARGE_RATE",		fBuffer );
		SetChangeRateChargeMpForDetonation( fBuffer );

		bool bShowMPGageBar = false;
		LUA_GET_VALUE( m_LuaManager, "DRAW_SMALL_MP_BAR", bShowMPGageBar, false );


		if( true == bShowMPGageBar )
		{
			SetMpTexture( L"Small_HP_bar_Blue.TGA" );
		}

		SetNowHp( GetMaxHp() );
		SetMaxMp( ONE_CHARGE * 3.f );

		m_fPrevHPGage = GetNowHp();

		LUA_GET_VALUE( m_LuaManager, "FORCE_DOWN_GAGE_MAX", fBuffer, 130.0f );
		SetMaxForceDown( fBuffer );
		GetGageData()->SetEnable( true );

		LUA_GET_VALUE( m_LuaManager, "SHOW_ON_MINIMAP", m_bShowOnMiniMap, true );



		//LUA_GET_VALUE( m_LuaManager, L"CAMERA_SHAKE_SPEED", s_fSpeed, 100.f );
		//LUA_GET_VALUE( m_LuaManager, L"CAMERA_SHAKE_RANGE", s_fRange, 10.f );
		//LUA_GET_VALUE( m_LuaManager, L"CAMERA_SHAKE_ACCEL", s_fRetrieveAccel, 10.f );
		//LUA_GET_VALUE( m_LuaManager, L"CAMERA_SHAKE_DAMPING_RATE", s_fDampingRate, 10.f );
		//LUA_GET_VALUE( m_LuaManager, L"CAMERA_SHAKE_COUNT", s_BounceCount, 1 );



		LUA_GET_VALUE( m_LuaManager, "OUT_LINE_WIDTH_SCALE", m_fOutLineWidthScale, 1.f );


		LUA_GET_VALUE( m_LuaManager, "MIND_FLAG_HEIGHT", m_fMindFlagHeight, 170.f );


		bool bUseAfterImage = false;
		LUA_GET_VALUE( m_LuaManager, "USE_AFTER_IMAGE", bUseAfterImage, false );
		if( true == bUseAfterImage )
		{
			D3DXCOLOR coAfterImage = 0xffcccccc;
			LUA_GET_VALUE( m_LuaManager, "AFTER_IMAGE_COLOR_R", coAfterImage.r, 0.797f );
			LUA_GET_VALUE( m_LuaManager, "AFTER_IMAGE_COLOR_G", coAfterImage.g, 0.797f );
			LUA_GET_VALUE( m_LuaManager, "AFTER_IMAGE_COLOR_B", coAfterImage.b, 0.797f );
			LUA_GET_VALUE( m_LuaManager, "AFTER_IMAGE_COLOR_A", coAfterImage.a, 1.f );

			X2_LAYER eAfterImageLayer = XL_EFFECT_0;
			LUA_GET_VALUE_ENUM( m_LuaManager, "AFTER_IMAGE_LAYER", eAfterImageLayer, X2_LAYER, XL_EFFECT_0 );


			m_pAfterImage = CKTDGXSkinAfterImage::CreateSkinAfterImage( m_pXSkinAnim.get(), 5, eAfterImageLayer );
			if( m_pAfterImage != NULL )
			{
				m_pAfterImage->SetAfterImageColor( coAfterImage );
				m_pAfterImage->AddMesh( m_pMotion );
				//m_pAfterImage->AddMesh( g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_pMotion->GetDeviceID() ) );
			}
		}

		LUA_GET_VALUE( m_LuaManager, "AFTER_IMAGE_ALWAYS", m_bAfterImageAlways, false );

		bool bAlwaysAlphaBlend = false;
		LUA_GET_VALUE( m_LuaManager, "ALPHA_BLEND_ALWAYS", bAlwaysAlphaBlend, false );

		if ( bAlwaysAlphaBlend )	/// true일때만 적용
		{
			m_stOriginBuffRenderParam.m_bAlphaBlend = bAlwaysAlphaBlend;
			SetAlphaObject( bAlwaysAlphaBlend );
		}


//         wstring lightFlowName;
//         LUA_GET_VALUE( m_LuaManager, "LIGHT_FLOW", lightFlowName, L"" );
//         GetXSkinAnimPtr()->SetLightFlow(lightFlowName);
        LUA_GET_VALUE( m_LuaManager, "UNIT_FOG", m_bFog, false );

        LUA_GET_VALUE( m_LuaManager, "NOT_EXTRA_DAMAGE", m_bImmuneToExtraDamage, false );

		LUA_GET_VALUE( m_LuaManager, "INVINCIBLE_TO_NPC_KEYCODE", m_iInvincibleToNPCKeyCode, 0 );

#ifdef CHECK_HAS_NOT_HEAD_BONE
		if( NULL == m_pFrame_Bip01_Head && true == GetNPCTemplet().m_bIsNPC )
		{
			switch( GetNPCTemplet().m_nNPCUnitID )
			{
			case CX2UnitManager::NUI_PD_MONSTER:
			case CX2UnitManager::NUI_DUNGEON_CHECKER1:
			case CX2UnitManager::NUI_DUNGEON_CHECKER2:
			case CX2UnitManager::NUI_DUNGEON_CHECKER3:
			case CX2UnitManager::NUI_DUNGEON_CHECKER4:
			case CX2UnitManager::NUI_DUNGEON_CHECKER5:
				break;
			default:
				{
					WCHAR wBuf[101];
					//if( NULL != GetNPCTemplet() )
					{
						StringCchPrintfW( wBuf, 100, L"HasNotHeadBone NPC ID %d \nLuaFileName : %s",GetNPCTemplet().m_nNPCUnitID, GetNPCTemplet().m_LuaFileName );
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wBuf, g_pMain->GetNowState() );
					}
				} break;
			}
		}
#endif //CHECK_HAS_NOT_HEAD_BONE

		LUA_GET_VALUE( m_LuaManager, "RAGE_COUNT_MAX",		m_RageCountMax, 0		);
		LUA_GET_VALUE( m_LuaManager, "RAGE_TIME_MAX",		m_fRageTimeMax, 0.0f	);

//{{ oasis907 : 김상윤 [2010.4.21] //
		LUA_GET_VALUE( m_LuaManager, "RAGE_HP_PERCENT",		m_iRageHPPercent, 0		);

		if( m_iRageHPPercent < 0 || m_iRageHPPercent > 100 )
		{
			m_iRageHPPercent = 0;
		}
//}}

//{{ oasis907 : 김상윤 [2010.4.22] //
		LUA_GET_VALUE( m_LuaManager, "NOT_CULL",		m_bNotCull, false		);

//}}


		LUA_GET_VALUE( m_LuaManager, "ENABLE_SELF_RESURRECTION",		m_bSelfResurrection,			false );
		LUA_GET_VALUE( m_LuaManager, "SELF_RESURRECTION_TIME",			m_fSelfResurrectionTime,		0.f );
		LUA_GET_VALUE( m_LuaManager, "SELF_RESURRECTION_COUNT",			m_iSelfResurrectionCount,		0 );
		LUA_GET_VALUE( m_LuaManager, "SELF_RESURRECTION_HP_PERCENT",	m_iSelfResurrectionHPPercent,	0 );

//{{ oasis907 : 김상윤 [2010.5.6] //
		LUA_GET_VALUE( m_LuaManager, "BOSS_KILL_EFFECT",				m_bPlayBossKillEffect,			false );
//}}

		LUA_GET_VALUE( m_LuaManager, "SET_HAS_BOSS_GAGE",				m_bHasBossGage,			m_bHasBossGage	);	/// 보스 체력 게이지 설정
		LUA_GET_VALUE( m_LuaManager, "SET_NUM_OF_GAGE",					m_usNumOfGage,			m_usNumOfGage	);	/// 보스 체력 게이지 바 갯수 설정

		if ( GetHasBossGage() )
		{
			std::wstring wstrBossGageTexName = L"";
			std::wstring wstrBossGageTexPieceName = L"";
			LUA_GET_VALUE( m_LuaManager, "BOSS_GAGE_FACE_TEX",				wstrBossGageTexName,		L""		);
			LUA_GET_VALUE( m_LuaManager, "BOSS_GAGE_FACE_TEX_PIECE",		wstrBossGageTexPieceName,	L""		);

			if( wstrBossGageTexName.length() > 0 && wstrBossGageTexPieceName.length() > 0 )
			{
				CX2GageManager::GetInstance()->CreateAndInsertBossGageUI( this, wstrBossGageTexName.c_str(), wstrBossGageTexPieceName.c_str() );
			}
			LUA_GET_VALUE( m_LuaManager, "BOSS_NAME_TEX",					m_wstrBossNameTexture,		L""		);
		}

		LUA_GET_VALUE( m_LuaManager, "HYPER_MODE_COUNT",				m_HyperModeCount,			0		);
		m_HyperModeColor = 0xffff5511;
		LUA_GET_VALUE( m_LuaManager, "HYPER_MODE_COLOR_R",				m_HyperModeColor.r,			1.f	);
		LUA_GET_VALUE( m_LuaManager, "HYPER_MODE_COLOR_G",				m_HyperModeColor.g,			0.333f	);
		LUA_GET_VALUE( m_LuaManager, "HYPER_MODE_COLOR_B",				m_HyperModeColor.b,			0.066f	);
		LUA_GET_VALUE( m_LuaManager, "HYPER_MODE_COLOR_A",				m_HyperModeColor.a,			1.f	);

		LUA_GET_VALUE( m_LuaManager, "HYPER_BOOST_RIGHT",				m_wstrHyperBoostRightParticleName,		L""		);
		LUA_GET_VALUE( m_LuaManager, "HYPER_BOOST_LEFT",				m_wstrHyperBoostLeftParticleName,			L""		);

		LUA_GET_VALUE( m_LuaManager, "USE_GUARD_COLLISION_BOX",				m_bUseGuardCollisionBox,			false	);

		LUA_GET_VALUE( m_LuaManager, "FALL_DOWN",				m_bFallDown,			false	);
        LUA_GET_VALUE( m_LuaManager, "DAMAGE_DOWN",				m_bDamageDown,			true	);

        LUA_GET_VALUE( m_LuaManager, "SKY_DIE",				    m_bSkyDie,			    false	);
		LUA_GET_VALUE( m_LuaManager, "DEFENCE_RATE",			m_DefenceRate,			0		);
        LUA_GET_VALUE( m_LuaManager, "DIE_FLY",				    m_SpeedDyingFly,		550	    );

		LUA_GET_VALUE( m_LuaManager, "ATTRIBUTE_EMOTION_FLAG",	m_bAttributeEmotionFlag,	true	    );

#ifdef ADD_BACKATTACK
		LUA_GET_VALUE( m_LuaManager, "BACK_ATTACK",	m_bApplyBackAttack,		true	    );
#endif

		if( m_LuaManager.BeginTable( "INVISIBLE" ) == true )
		{
			m_Invisible.m_bInvisible	= true;
			m_Invisible.m_bAdd			= true;
			LUA_GET_VALUE( m_LuaManager, "MAX_ALPHA",		m_Invisible.m_fMaxAlpha,	1.0f	);
			LUA_GET_VALUE( m_LuaManager, "MIN_ALPHA",		m_Invisible.m_fMinAlpha,	0.0f	);
			LUA_GET_VALUE( m_LuaManager, "SPEED",			m_Invisible.m_fSpeed,		1.0f	);

			LUA_GET_VALUE( m_LuaManager, "ALPHA_LOWER_BOUND",	m_Invisible.m_fAlphaLowerBound,	m_Invisible.m_fMinAlpha	);

			LUA_GET_VALUE( m_LuaManager, "CLOAKING_UNIT",		m_Invisible.m_bCloakingUnit,	false	);

			LUA_GET_VALUE( m_LuaManager, "ALWAYS_INVISIBLE",	m_Invisible.m_bAlwaysInvisible,	true );

			SetAlphaObject( true );

			m_LuaManager.EndTable();
		}

		RemoveAllWeaponAttackCollisionBoxForNPC();


		int iWeaponIndex = 0;
		while( true == m_LuaManager.BeginTable( "WEAPON", iWeaponIndex ) )
		{
			//{{ robobeg : 2008-10-28
			Weapon* pWeapon = Weapon::CreateWeapon( *this );
			ASSERT( pWeapon != NULL );

			if ( IsRecursivelyManagedByDGManager() )
				g_pKTDXApp->GetDGManager()->AddObjectChain( pWeapon );
			//}} robobeg : 2008-10-28
			if( true == pWeapon->LoadDataForNPC( m_LuaManager ) )
			{
				pWeapon->m_iWeaponIndex = iWeaponIndex; // note: slash_trace 만들때 몇 번 무기에 만들지 결정하기 위해서 필요
				AddWeaponAttackCollisionBoxForNPC( pWeapon );
				m_vecpWeapon.push_back( pWeapon );
			}
			else
			{
				SAFE_DELETE_KTDGOBJECT( pWeapon );
			}

			m_LuaManager.EndTable(); // WEAPON%d
			iWeaponIndex += 1;
		}

	//{{ oasis907 : 김상윤 [2010.4.19] //



		if( m_LuaManager.BeginTable( "REFLECT_DAMAGE" ) == true )
		{
			if( m_LuaManager.BeginTable( "CONDITION_REFLECT_DAMAGE" ) == true )
			{
				LUA_GET_VALUE( m_LuaManager, "MY_HP_LESS_THAN_PERCENT",	m_ConditionReflectDamage.m_fMyHPLessThanPercentFactor,	-1.0f );
				LUA_GET_VALUE( m_LuaManager, "ON_PHYSICAL_DAMAGE",		m_ConditionReflectDamage.m_bOnPhysicalDamage,	false );
				LUA_GET_VALUE( m_LuaManager, "ON_MAGICAL_DAMAGE",		m_ConditionReflectDamage.m_bOnMagicalDamage,	false );
				m_LuaManager.EndTable();
			}

			m_ReflectDamageData.SimpleInit();
			m_ReflectDamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_ReflectDamageData.optrAttackerGameUnit		= this;
			g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_ReflectDamageData, m_LuaManager, "REFLECT_DAMAGE_DATA", GetPowerRate() );

			m_LuaManager.EndTable();
		}
	//}}

		bool bRideMonster;
		LUA_GET_VALUE( m_LuaManager, "RIDE_MONSTER", bRideMonster, false );
		if(bRideMonster)
		{
			m_eRideState =  CX2GUNPC::RS_READY;

			LUA_GET_VALUE( m_LuaManager, "RIDE_TARGET_MONSTER", m_RideTargetNPCID, 0);
			if(m_RideTargetNPCID != 0)
			{
				m_eRideType = CX2GUNPC::RT_RIDER;
			}
			else
			{
				m_eRideType = CX2GUNPC::RT_RIDING;
			}
			LUA_GET_VALUE( m_LuaManager, "SEARCH_RADIUS", m_fSearchRadius, 10000.f);
		}

		//{{ kimhc // 2010.8.7 // 무조건 NPC에게 카메라가 가도록 하는 기능
		LUA_GET_VALUE( m_LuaManager, "FOCUS_CAMERA_FORCE", m_bFocusCameraForce, false );
		//}} kimhc // 2010.8.7 // 무조건 NPC에게 카메라가 가도록 하는 기능

		int iAttachIndex = 0;
		while( true == m_LuaManager.BeginTable( "ATTACH_MESH", iAttachIndex ) )
		{
			wstring wstrAttachedMeshName = L"";
			wstring wstrAttachedBoneName = L"";
			float fScale = 1.f;
			D3DXVECTOR3 vOffset = D3DXVECTOR3(0.f, 0.f, 0.f);
			D3DXVECTOR3 vRotate = D3DXVECTOR3(0.f, 0.f, 0.f);

			LUA_GET_VALUE( m_LuaManager, "ATTACH_MESH_NAME",	wstrAttachedMeshName,	L"" );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_BONE_NAME",	wstrAttachedBoneName,	L"" );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_SCALE",		fScale,					1.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_OFFSET_X",		vOffset.x,				0.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_OFFSET_Y",		vOffset.y,				0.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_OFFSET_Z",		vOffset.z,				0.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_ROTATE_X",		vRotate.x,				0.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_ROTATE_Y",		vRotate.y,				0.f );
			LUA_GET_VALUE( m_LuaManager, "ATTACH_ROTATE_Z",		vRotate.z,				0.f );

			CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( wstrAttachedBoneName.c_str() );
			if( pFrame != NULL )
			{
				CX2SubEquip* pSubEquip = CX2SubEquip::CreateSubEquip( &pFrame->combineMatrix, wstrAttachedMeshName, false );
				if( pSubEquip != NULL )
				{
					m_vecAttachMesh.push_back( pSubEquip );

					pSubEquip->SetManagerObject( this );
					g_pKTDXApp->GetDGManager()->AddObjectChain( pSubEquip );

					pSubEquip->SetLayer( GetLayer() );
					pSubEquip->SetScale( fScale );
					pSubEquip->SetOffset( vOffset );
					pSubEquip->SetRotate( vRotate );
				}
			}

			m_LuaManager.EndTable();
			++iAttachIndex;
		}
		LUA_GET_VALUE( m_LuaManager, "ABSORB_MAGIC_MONSTER", m_bAbsorbMagicMonster, false );

		LUA_GET_VALUE( m_LuaManager, "ABSORB_MAGIC_SPECIAL",	m_bAbsorbMagicSpecial,	false );

		LUA_GET_VALUE( m_LuaManager, "SUMMON_TIME",			 m_fSummonTime,			0.f );		//해당 NPC의 소환 시간

		LUA_GET_VALUE( m_LuaManager, "DELETE_NPC_BY_SUB_STAGE_END",			m_bDeleteNPCBySubStageEnd,		false	);		/// 서브 스테이지 종료시 해당 NPC 삭제 여부

		LUA_GET_VALUE( m_LuaManager, "IMMUNITY_TIME_STOP",	m_bImmunityTimeStop, false	);//타임 스톱 면역 기능 추가


		// Stage, Substage 가 클리어 될 때에도 해당 NPC 를 남길 것인가
		LUA_GET_VALUE( m_LuaManager, "REMAINING_WHEN_SUB_STAGE_CLEAR", m_bIsRemainingNpcWhenStageCleard, false );


		LUA_GET_VALUE( m_LuaManager, "NO_RENDER_NAME",		m_bNoRenderName,	 false	);		/// 몬스터 이름 미출력 여부 설정

		if( m_LuaManager.BeginTable( "SET_RENDER_NAME_COLOR" ) == true )						/// 몬스터 이름 색상 변경
		{
			m_bChangeRenderNameColor = true;	/// 이름 색상 변경 설정

			LUA_GET_VALUE( m_LuaManager, "COLOR_R",				m_colorRenderName.r,			0.f	);
			LUA_GET_VALUE( m_LuaManager, "COLOR_G",				m_colorRenderName.g,			0.f	);
			LUA_GET_VALUE( m_LuaManager, "COLOR_B",				m_colorRenderName.b,			0.f	);
			LUA_GET_VALUE( m_LuaManager, "COLOR_A",				m_colorRenderName.a,			0.f	);

			LUA_GET_VALUE( m_LuaManager, "OUTLINE_COLOR_R",		m_colorRenderNameOutLine.r,		0.f );
			LUA_GET_VALUE( m_LuaManager, "OUTLINE_COLOR_G",		m_colorRenderNameOutLine.g,		0.f	);
			LUA_GET_VALUE( m_LuaManager, "OUTLINE_COLOR_B",		m_colorRenderNameOutLine.b,		0.f );
			LUA_GET_VALUE( m_LuaManager, "OUTLINE_COLOR_A",		m_colorRenderNameOutLine.a,		0.f );

			m_LuaManager.EndTable(); // SET_RENDER_NAME_COLOR
		}

		bool bIsBossRaidNPC = false;
		LUA_GET_VALUE( m_LuaManager, "BOSS_RAID_NPC",				bIsBossRaidNPC,	 false	);
		m_eMonsterGrade = (true == bIsBossRaidNPC ) ? MG_BOSS_NPC : m_eMonsterGrade;

		m_LuaManager.EndTable();
	}

	m_bAddForceDownGageThisState	= false;
	m_fLastDamagePassTime			= 0.0f;
	m_fMaxDownForce = -9999.f;

}






/*static*/
void    CX2GUNPC::InitState( InitData& OutInit_, KLuaManager& luaManager_
     )
{
    InitStateData& initState = OutInit_.m_state;
    initState.m_vecStateData.resize( 0 );
    initState.m_vecStateData.resize( 1 );
    initState.m_mapStateNameID.clear();
    ZeroMemory( initState.m_acStateType, sizeof(initState.m_acStateType) );

	if( luaManager_.BeginTable( "INIT_STATE" ) == true )
	{
        int index = 1;
        while( luaManager_.BeginTable( index ) == true )
        {
			index++;
			luaManager_.EndTable();
        }
        initState.m_vecStateData.resize( GUSI_END + 1 + (index - 1 ) );
        initState.m_mapStateNameID.insert( std::make_pair( "NONE", GUSI_NONE ) );
        initState.m_mapStateNameID.insert( std::make_pair( "LOADING", GUSI_LOADING ) );
        initState.m_mapStateNameID.insert( std::make_pair( "READY", GUSI_READY ) );
        initState.m_mapStateNameID.insert( std::make_pair( "PLAY", GUSI_PLAY ) );
        initState.m_mapStateNameID.insert( std::make_pair( "DIE", GUSI_DIE ) );
        initState.m_mapStateNameID.insert( std::make_pair( "END", GUSI_END ) );

		index = 1;
        string stateName;
        int iStateID;
		while( luaManager_.BeginTable( index ) == true )
		{
            iStateID = GUSI_END + 1 + (index - 1);
			StateData& stateData = initState.m_vecStateData[ iStateID ];
            stateName.resize( 0 );
            LUA_GET_VALUE_UTF8( luaManager_, "STATE_NAME", stateName, "" );

			LUA_GET_VALUE_UTF8( luaManager_, "LUA_STATE_START_FUNC",	stateData.m_strLua_StateStart,	"" );
			LUA_GET_VALUE_UTF8( luaManager_, "LUA_FRAME_MOVE_FUNC",	stateData.m_strLua_FrameMove,	"" );
			LUA_GET_VALUE_UTF8( luaManager_, "LUA_CAMERA_MOVE_FUNC",	stateData.m_strLua_CameraMove,	"" );
			LUA_GET_VALUE_UTF8( luaManager_, "LUA_STATE_END_FUNC",		stateData.m_strLua_StateEnd,		"" );

			LUA_GET_VALUE( luaManager_, "STATE_COOL_TIME",		stateData.m_fStateCoolTime,	0.0f );

//{{ kimhc // 2010.4.16 // 비밀던전 작업(보스 몬스터 작업)
			// 쿨타임을 주었을 경우 처음에는 무조건 쿨타임의 시간만큼 해당 스테이트가 실행이 안되는
			// 경우를 유동적으로 조절 할 수 있게 하기 위해, 처음에 stateData.fStateElpasedTime을
			// 셋팅 할 수 있게 함
			LUA_GET_VALUE( luaManager_, "INITIALIZE_STATE_ELAPSED_TIME",	stateData.m_fInitializeStateElapsedTime, 0.0f );
//}} kimhc // 2010.4.16 // 비밀던전 작업(보스 몬스터 작업)

//{{ robobeg : 2008-10-28
			//m_StateListByName.insert( std::make_pair( stateData.stateTable.c_str(), stateData ) );
            initState.m_mapStateNameID.insert( std::make_pair( stateName, iStateID ) );
//}} robobeg : 2008-10-28

			index++;
			luaManager_.EndTable();
		}//while

//{{ kimhc // 2010.8.8 // 무조건 NPC에게 카메라가 가도록 하는 기능
		LUA_GET_VALUE_UTF8( luaManager_, "COMMON_CAMERA_MOVE_FUNC",		initState.m_strCommonCameraMove,		"" );
//}} kimhc // 2010.8.8 // 무조건 NPC에게 카메라가 가도록 하는 기능
        // added by wonpok.
        // 모든 스테이트에 대한 공통 FrameMove Function 정의
        LUA_GET_VALUE_UTF8( luaManager_, "COMMON_FRAME_FUNC",		    initState.m_strCommonFrameMove,		"" );

        initState.m_iHintTimerEventID = -1;
        initState.m_vecHintTimerEventTrigger.resize(0);
        float   fTemp = 0.f;
        if ( luaManager_.BeginTable( "HINT_TIMER_EVENT_TRIGGER" ) == true )
        {
            int iNum = luaManager_.GetNumIndexedValues();
            if ( iNum <= 1 )
                goto out_hint;
            int iID = -1;
            if ( luaManager_.GetValue( 1, iID ) == false )
                goto out_hint;
            if ( iID < 0 || iID >= 10 )
                goto out_hint;
            initState.m_iHintTimerEventID = iID;
            initState.m_vecHintTimerEventTrigger.reserve( iNum - 1 );
            index = 2;
		    while ( luaManager_.GetValue( index, fTemp ) == true )
		    {
                initState.m_vecHintTimerEventTrigger.push_back( fTemp );
			    ++index;
		    }
            std::stable_sort( initState.m_vecHintTimerEventTrigger.begin(), initState.m_vecHintTimerEventTrigger.end() );
            std::vector<float>::iterator iterEnd = std::unique( initState.m_vecHintTimerEventTrigger.begin(), initState.m_vecHintTimerEventTrigger.end() );
            initState.m_vecHintTimerEventTrigger.resize( std::distance( initState.m_vecHintTimerEventTrigger.begin(), iterEnd ) );
            luaManager_.EndTable();
        }//if
out_hint:

        // 공격 명중시 실행할 Function 정의
        LUA_GET_VALUE_UTF8( luaManager_, "ATTACKED_FUNC",		        initState.m_strAttackedFunc,		"" );

        std::string stateName2;

        for( unsigned i = 0; i < ARRAY_SIZE( s_aStateTypes ); i++ )
        {
            stateName.resize( 0 );
            ASSERT( s_aStateTypes[i].m_eStateType == EStateType( i ) );
            LUA_GET_VALUE_UTF8( luaManager_, s_aStateTypes[i].m_pszName, stateName, "" );
            int idStateID = 0;
            if ( stateName.empty() == false )
                idStateID = initState.GetStateID( stateName );
            initState.m_acStateType[ i ] = idStateID;
            if ( i == STATE_TYPE_WAIT_STATE )
            {
                if ( idStateID != 0 )
                {
                    initState.m_vecStateData[ idStateID ].m_bitsetFlags.set( StateData::FLAG_WAIT_STATE, true );
                }
                else if ( initState.m_acStateType[ STATE_TYPE_START_STATE ] != 0 )
                {
                    initState.m_vecStateData[ initState.m_acStateType[ STATE_TYPE_START_STATE ] ].m_bitsetFlags.set( StateData::FLAG_WAIT_STATE, true );
                }
            }
            if ( i >= STATE_TYPE_REACTION_STATE_BEGIN && i < STATE_TYPE_REACTION_STATE_NUMS && idStateID != 0 )
            {
			    StateData& nowStateData = initState.m_vecStateData[ idStateID ];
                if ( NOWSTATE_FLAG( REACTION_STATE ) == false && NOWSTATE_FLAG( WAIT_STATE ) == false )
                {
                    NOWSTATE_SET_FLAG( REACTION_STATE, true );
                    if ( stateName.length() >= 5 && _stricmp( stateName.c_str() + stateName.length() - 5, "_BACK" ) == 0 )
                    {
                        stateName2.assign( stateName.begin(), stateName.end() - 5 );
                        stateName2 += "_FRONT";
                        int idStateID2 = initState.GetStateID( stateName2 );
                        if ( idStateID2 != 0 )
                            nowStateData.m_cEquivalentReactionState = (char) idStateID2;
                    }
                    else if ( stateName.length() >= 6 && _stricmp( stateName.c_str() + stateName.length() - 6, "_FRONT" ) == 0 )
                    {
                        stateName2.assign( stateName.begin(), stateName.end() - 6 );
                        stateName2 += "_BACK";
                        int idStateID2 = initState.GetStateID( stateName2 );
                        if ( idStateID2 != 0 )
                            nowStateData.m_cEquivalentReactionState = (char) idStateID2;
                    }
                }
            }
        }

	    if( luaManager_.BeginTable( "DAMAGE_EXTRA_STATES" ) == true )
        {
            int index = 1;
            stateName.resize( 0 );
            while( luaManager_.GetValueUtf8( index++, stateName ) )
            {
                int idStateID = initState.GetStateID( stateName );
                if ( idStateID != 0 )
                {
			        StateData& nowStateData = initState.m_vecStateData[ idStateID ];
                    if ( NOWSTATE_FLAG( REACTION_STATE ) == false )
                    {
                        NOWSTATE_SET_FLAG( REACTION_STATE, true );
                        if ( stateName.length() >= 5 && _stricmp( stateName.c_str() + stateName.length() - 5, "_BACK" ) == 0 )
                        {
                            stateName2.assign( stateName.begin(), stateName.end() - 5 );
                            stateName2 += "_FRONT";
                            int idStateID2 = initState.GetStateID( stateName2 );
                            if ( idStateID2 != 0 )
                                nowStateData.m_cEquivalentReactionState = (char) idStateID2;
                        }
                        else if ( stateName.length() >= 6 && _stricmp( stateName.c_str() + stateName.length() - 6, "_FRONT" ) == 0 )
                        {
                            stateName2.assign( stateName.begin(), stateName.end() - 6 );
                            stateName2 += "_BACK";
                            int idStateID2 = initState.GetStateID( stateName2 );
                            if ( idStateID2 != 0 )
                                nowStateData.m_cEquivalentReactionState = (char) idStateID2;
                        }
                    }
                }
            }
            luaManager_.EndTable();
        }

	    if( luaManager_.BeginTable( "WAIT_STATES" ) == true )
        {
            int index = 1;
            stateName.resize( 0 );
            while( luaManager_.GetValueUtf8( index++, stateName ) )
            {
                int idStateID = initState.GetStateID( stateName );
                if ( idStateID != 0 )
                {
			        StateData& nowStateData = initState.m_vecStateData[ idStateID ];
                    NOWSTATE_SET_FLAG( WAIT_STATE, true );
                }
            }
            luaManager_.EndTable();
        }

        luaManager_.EndTable();
    }//if


//{{ robobeg : 2008-10-28

    // state table 을 state ID 로 참조하기 위해 luaManager_ 에 reference 로 등록한다.
    // state table 이 global table에 있기 때문에 위의 BeginTable, EndTable 블럭을 빠져나와서 등록함.




    for( std::map<string,char> ::const_iterator iter = initState.m_mapStateNameID.begin(); iter != initState.m_mapStateNameID.end(); iter++ )
    {
        if ( iter->second <= GUSI_END )
            continue;
        StateData& nowStateData = initState.m_vecStateData[ iter->second ];
        if ( luaManager_.BeginTable( iter->first.c_str() ) == true )
        {
            LoadStateData( nowStateData, OutInit_, luaManager_
                );
            luaManager_.EndTable();
        }
    }//
//}} robobeg : 2008-10-28
}

/*static*/
void    CX2GUNPC::LoadStateData( StateData& nowStateData, InitData& OutInit_, KLuaManager& luaManager_
    )
{
	bool    bTemp = false;
    int     iTemp = -1;
    float   fTemp = 0.f;
    int     index = 0;
    std::wstring wstrTemp;
    std::string strTemp;
    char    aszTemp[100] = {0,};


	nowStateData.m_vecImmunityAtThisState.resize( 0 );
	if ( true == luaManager_.BeginTable( "IMMUNITY_LIST_AT_THIS_STATE" ) )
	{
		index = 1;
		UINT eImmuneBuffOrDebuffType = 0;
        nowStateData.m_vecImmunityAtThisState.reserve( luaManager_.GetNumIndexedValues() );
		while ( luaManager_.GetValue( index, eImmuneBuffOrDebuffType ) )
		{
			nowStateData.m_vecImmunityAtThisState.push_back( static_cast<BUFF_TEMPLET_ID>( eImmuneBuffOrDebuffType ) );
			++index;
		}
		luaManager_.EndTable();
	}

	LUA_GET_VALUE( luaManager_, "ALLOW_DIR_CHANGE", bTemp, false );
    NOWSTATE_SET_FLAG( ALLOW_DIR_CHANGE, bTemp );

	LUA_GET_VALUE( luaManager_, "IMMADIATE_PACKET_SEND", bTemp, false );
    NOWSTATE_SET_FLAG( IMMEDIATE_PACKET_SEND, bTemp );

	LUA_GET_VALUE( luaManager_, "VIEW_TARGET", bTemp, false );
    NOWSTATE_SET_FLAG( VIEW_TARGET, bTemp );

    bTemp = false;
	if( luaManager_.GetValue( "RIGHT", bTemp ) == true )
	{
        NOWSTATE_SET_FLAG( SET_DIRECTION, true );
        NOWSTATE_SET_FLAG( SET_DIRECTION_VALUE, bTemp );
	}

    iTemp = -1;
	if( luaManager_.GetValue( "MIND_FLAG", iTemp ) == true )
    {
        NOWSTATE_SET_FLAG( SET_MIND_FLAG, true );
        nowStateData.m_cSetMindFlagValue = (char) iTemp;
    }//if

	LUA_GET_VALUE( luaManager_, "LAND_CONNECT",	bTemp,	true );
    NOWSTATE_SET_FLAG( SET_LAND_CONNECT_VALUE, bTemp );

    LUA_GET_VALUE( luaManager_, "NOTUSE_LANDCONNECT",	bTemp,	false );
    NOWSTATE_SET_FLAG( NOT_USE_LAND_CONNECT, bTemp );

	LUA_GET_VALUE( luaManager_, "CAN_PUSH_UNIT",	bTemp,	true );
	NOWSTATE_SET_FLAG( SET_CAN_PUSH_UNIT_VALUE, bTemp );
	LUA_GET_VALUE( luaManager_, "CAN_PASS_UNIT",	bTemp,	false );
    NOWSTATE_SET_FLAG( SET_CAN_PASS_UNIT_VALUE, bTemp );

	LUA_GET_VALUE( luaManager_, "CAN_PASS_ALLY_UNIT",	bTemp,	true );
    NOWSTATE_SET_FLAG( CAN_PASS_ALLY_UNIT, bTemp );

	LUA_GET_VALUE( luaManager_, "APPLY_ANIM_MOVE", bTemp, true );
    NOWSTATE_SET_FLAG( APPLY_ANIM_MOVE, bTemp );

	LUA_GET_VALUE( luaManager_, "IGNORE_LINE_TIME_START", nowStateData.m_v2IgnoreLineTime.x, 0.f );
	LUA_GET_VALUE( luaManager_, "IGNORE_LINE_TIME_END", nowStateData.m_v2IgnoreLineTime.y, 0.f );
	ASSERT( nowStateData.m_v2IgnoreLineTime.x <= nowStateData.m_v2IgnoreLineTime.y );

	LUA_GET_VALUE( luaManager_, "SUPER_ARMOR", bTemp,			false );
    NOWSTATE_SET_FLAG( SET_SUPER_ARMOR_VALUE, bTemp );

    LUA_GET_VALUE( luaManager_, "SUPER_ARMOR_NOT_RED",			bTemp,			false );
    NOWSTATE_SET_FLAG( SET_SUPER_ARMOR_NOT_RED_VALUE, bTemp );

    LUA_GET_VALUE( luaManager_, "CANNOT_RAGE",			bTemp,			false );
    NOWSTATE_SET_FLAG( CANNOT_RAGE, bTemp );

	LUA_GET_VALUE( luaManager_, "SLASH_TRACE_TIP_WIDE",	nowStateData.m_fSlashTraceTipWide,	4.0f );

	LUA_GET_VALUE( luaManager_, "FOLLOW_HITTER", nowStateData.m_iSetFollowHitterValue,		0 );

	LUA_GET_VALUE( luaManager_, "ATTACK_FOLLOW_HITTER",	bTemp,	false );
    NOWSTATE_SET_FLAG( ATTACK_FOLLOW_HITTER, bTemp );

	//{{ JHKang / 강정훈 / 2010/12/15 / 몬스터를 캐릭터 얼굴 앞에 붙이기
	LUA_GET_VALUE( luaManager_, "ATTACK_FOLLOW_HITTER_POS_FRONT",	bTemp,	false );
    NOWSTATE_SET_FLAG( ATTACK_FOLLOW_HITTER_POS_FRONT, bTemp );
	//}} JHKang / 강정훈 / 2010/12/15 / 몬스터를 캐릭터 얼굴 앞에 붙이기

	LUA_GET_VALUE( luaManager_, "APPLY_MOVEBONE",	bTemp,	true );
    NOWSTATE_SET_FLAG( APPLY_MOVEBONE, bTemp );

	LUA_GET_VALUE( luaManager_, "GUARD_DEFENCE_BACK",		bTemp,			true );
    NOWSTATE_SET_FLAG( GUARD_DEFENCE_BACK, bTemp );
    LUA_GET_VALUE( luaManager_, "GUARD_DEFENCE_FRONT",		bTemp,			true );
    NOWSTATE_SET_FLAG( GUARD_DEFENCE_FRONT, bTemp );

    LUA_GET_VALUE( luaManager_, "GUARD_DEFENCE",			nowStateData.m_fSetGuardDefenceValue,			0 );

    LUA_GET_VALUE( luaManager_, "DAMAGE_DOWN", bTemp,			true );
    NOWSTATE_SET_FLAG( DAMAGE_DOWN, bTemp );

	LUA_GET_VALUE( luaManager_, "TRANSITION", bTemp, true );
    NOWSTATE_SET_FLAG( TRANSITION, bTemp );

    nowStateData.m_vecEffectSetToPlay.resize( 0 );
    {
        int iNumEffectSetList = 0;
	    if( true == luaManager_.BeginTable( "EFFECT_SET_LIST" ) )
	    {
            iNumEffectSetList = luaManager_.GetNumIndexedValues() / 2;
            luaManager_.EndTable();
        }
        if ( true == luaManager_.BeginTable( "EFFECT_SET_LIST_EX" ) )
        {
            iNumEffectSetList += luaManager_.GetNumIndexedTables();
            luaManager_.EndTable();
        }
        nowStateData.m_vecEffectSetToPlay.reserve( iNumEffectSetList );
	    if( true == luaManager_.BeginTable( "EFFECT_SET_LIST" ) )
	    {
            int iValueIndex = 1;
            while( 1 )
		    {
                wstrTemp.resize( 0 );
			    if ( luaManager_.GetValue( iValueIndex, wstrTemp) == false )
                    break;
                iValueIndex++;
                fTemp = 0.f;
                if ( luaManager_.GetValue( iValueIndex, fTemp ) == false )
                    break;
                iValueIndex++;
                if ( wstrTemp.empty() == false && fTemp >= 0.f )
			        nowStateData.m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrTemp, fTemp ) );
		    }
		    luaManager_.EndTable(); //EFFECT_SET_LIST
	    }
	    if( true == luaManager_.BeginTable( "EFFECT_SET_LIST_EX" ) )
	    {
		    vector<DELETE_CONDITION_EFFECT_SET> vecDeleteCondition;		// 종료 조건 벡터
            index = 1;
            while( true == luaManager_.BeginTable( index++  ) )
		    {
			    LUA_GET_VALUE( luaManager_, 1, wstrTemp,	 L"" );
			    LUA_GET_VALUE( luaManager_, 2, fTemp, 0.f);
			    int iTableCount = 3;			// 테이블 카운터, 3번 테이블부터 읽어오기 시작함
			    UINT uiDeleteConditionType = 0;	// 종료 조건
			    vecDeleteCondition.clear();

			    while ( luaManager_.GetValue( iTableCount, uiDeleteConditionType ) == true )
			    {	// 값이 있다면, 테이블 카운터 증가시키고 종료 조건을 벡터에 넣어준다.
				// 얻어온 값이 범위에 벗어난다면, ASSERT 를 띄운다.
#if X2_DEV_TOOLS
				// 사내 일 경우, 해당 종료 조건이 중복되는지 체크한다.
				    ASSERT( vecDeleteCondition.end() == find ( vecDeleteCondition.begin(), vecDeleteCondition.end(), static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) ) );
#endif // _IN_HOUSE_

				    iTableCount++;
				    vecDeleteCondition.push_back( static_cast<CX2GameUnit::DELETE_CONDITION_EFFECT_SET> (uiDeleteConditionType) );
			    }
			    luaManager_.EndTable();
                if ( wstrTemp.empty() == false && fTemp >= 0.f )
                {
			        nowStateData.m_vecEffectSetToPlay.push_back( EffectSetToPlay( wstrTemp, fTemp, vecDeleteCondition ) );
                }//if
		    }

		    luaManager_.EndTable(); //EFFECT_SET_LIST
	    }
        std::stable_sort( nowStateData.m_vecEffectSetToPlay.begin(), nowStateData.m_vecEffectSetToPlay.end() );
    }

	LUA_GET_VALUE( luaManager_, "DELETE_EFFECT_SET_ON_STATE_END",		bTemp,		false );
    NOWSTATE_SET_FLAG( DELETE_EFFECT_SET_ON_STATE_END, bTemp );
	LUA_GET_VALUE( luaManager_, "DELETE_EFFECT_SET_ON_DAMAGE_REACT",	bTemp,	false );
    NOWSTATE_SET_FLAG( DELETE_EFFECT_SET_ON_DAMAGE_REACT, bTemp );
	LUA_GET_VALUE( luaManager_, "DELETE_EFFECT_SET_ON_DIE",	bTemp,	false );
    NOWSTATE_SET_FLAG( DELETE_EFFECT_SET_ON_DIE, bTemp );
	LUA_GET_VALUE( luaManager_, "HYPER_EFFECT_SET",		bTemp,		false );
    NOWSTATE_SET_FLAG( HYPER_EFFECT_SET, bTemp );
	LUA_GET_VALUE( luaManager_, "EFFECT_SET_LIFE_TIME",	nowStateData.m_fEffectSetLifeTime,	-1.f );

	LUA_GET_VALUE( luaManager_, "DELETE_EFFECT_SET_ON_CUSTOM_STATE",	bTemp,	false );
    NOWSTATE_SET_FLAG( DELETE_EFFECT_SET_ON_CUSTOM_STATE, bTemp );
	LUA_GET_VALUE( luaManager_, "CUSTOM_STATE_DELETE_EFFECT_SET", bTemp,	false );
    NOWSTATE_SET_FLAG( CUSTOM_STATE_DELETE_EFFECT_SET, bTemp );
    LUA_GET_VALUE( luaManager_, "CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END",	bTemp,	false );
    NOWSTATE_SET_FLAG( CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END, bTemp );

    LUA_GET_VALUE( luaManager_, "SWAP_UNIT_WIDTH_HEIGHT", bTemp, false );
    NOWSTATE_SET_FLAG( SWAP_UNIT_WIDTH_HEIGHT, bTemp );

	LUA_GET_VALUE( luaManager_, "SHOW_STICK_SHAKE",	bTemp, false );
    NOWSTATE_SET_FLAG( SHOW_STICK_SHAKE, bTemp );

	LUA_GET_VALUE( luaManager_, "NEVER_MOVE", bTemp,	false );
    NOWSTATE_SET_FLAG( SET_NEVER_MOVE_VALUE, bTemp );

	LUA_GET_VALUE( luaManager_, "DYING_END", bTemp, false );
    NOWSTATE_SET_FLAG( DYING_END, bTemp );
	// 09.05.19 태완
	LUA_GET_VALUE( luaManager_, "ESCAPE_END", bTemp, false );
    NOWSTATE_SET_FLAG( ESCAPE_END, bTemp );

	LUA_GET_VALUE( luaManager_, "DYING_SPEED", nowStateData.m_fDyingSpeed, 0.5f );

	LUA_GET_VALUE( luaManager_, "USE_MIND_FLAG", bTemp, true );
    NOWSTATE_SET_FLAG( USE_MIND_FLAG, bTemp );

	LUA_GET_VALUE( luaManager_, "FALL_DOWN", bTemp, true );
    NOWSTATE_SET_FLAG( FALL_DOWN, bTemp );


	// attack box
    nowStateData.m_vecwstrEnableAttackBox.resize( 0 );
	if( luaManager_.BeginTable( "ENABLE_ATTACK_BOX" ) == true )
	{
		index = 1;
        nowStateData.m_vecwstrEnableAttackBox.reserve( luaManager_.GetNumIndexedValues() );
		while( luaManager_.GetValue( index, wstrTemp ) == true )
		{
            nowStateData.m_vecwstrEnableAttackBox.push_back( wstrTemp );
			index++;
		}
		luaManager_.EndTable();
	}

    nowStateData.m_vecwstrDisableAttackBox.resize( 0 );
	if( luaManager_.BeginTable( "DISABLE_ATTACK_BOX" ) == true )
	{
		index = 1;
        nowStateData.m_vecwstrDisableAttackBox.reserve( luaManager_.GetNumIndexedValues() );
		while( luaManager_.GetValue( index, wstrTemp ) == true )
		{
			nowStateData.m_vecwstrDisableAttackBox.push_back( wstrTemp );
			index++;
		}
		luaManager_.EndTable();
	}

	// collision box
    nowStateData.m_vecwstrEnableCollisionBox.resize( 0 );
	if( luaManager_.BeginTable( "ENABLE_COLLISION_BOX" ) == true )
	{
		index = 1;
        nowStateData.m_vecwstrEnableCollisionBox.reserve( luaManager_.GetNumIndexedValues() );
		while( luaManager_.GetValue( index, wstrTemp ) == true )
		{
            nowStateData.m_vecwstrEnableCollisionBox.push_back( wstrTemp );
			index++;
		}
		luaManager_.EndTable();
	}

    nowStateData.m_vecwstrDisableCollisionBox.resize( 0 );
	if( luaManager_.BeginTable( "DISABLE_COLLISION_BOX" ) == true )
	{
		index = 1;
        nowStateData.m_vecwstrDisableCollisionBox.reserve( luaManager_.GetNumIndexedValues() );
		while( luaManager_.GetValue( index, wstrTemp ) == true )
		{
            nowStateData.m_vecwstrDisableCollisionBox.push_back( wstrTemp );
			index++;
		}
		luaManager_.EndTable();
	}


	bTemp = true;
	if( true == luaManager_.GetValue( "SHOW", bTemp ) )
	{
        NOWSTATE_SET_FLAG( SET_SHOW, true );
        NOWSTATE_SET_FLAG( SET_SHOW_VALUE, bTemp );
		//GetGageManager()->SetShow( bShow );
	}
//#ifdef FIX_NPC_SHOW_OBJECT
//	m_bShowObjectCurrentState = bShow;
//#endif //FIX_NPC_SHOW_OBJECT

    bTemp = true;
	if( true == luaManager_.GetValue( "SHOW_NAME", bTemp ) )
	{
        NOWSTATE_SET_FLAG( SET_SHOW_NAME, true );
        NOWSTATE_SET_FLAG( SET_SHOW_NAME_VALUE, bTemp );
	}

    bTemp = false;
	if( true == luaManager_.GetValue( "INVISIBLE_TO_ENEMY", bTemp ) )
	{
        NOWSTATE_SET_FLAG( SET_INVISIBLE_TO_ENEMY, true );
        NOWSTATE_SET_FLAG( SET_INVISIBLE_TO_ENEMY_VALUE, bTemp );
    }

	LUA_GET_VALUE( luaManager_, "DYING_END_IMMEDIATE", bTemp, false );
    NOWSTATE_SET_FLAG( DYING_END_IMMEDIATE, bTemp );

    nowStateData.m_vecTalkBox.resize( 0 );
	if( luaManager_.BeginTable( "TALK_BOX" ) == true )
	{
		int iRate = 0;

		index = 1;
        nowStateData.m_vecTalkBox.reserve( luaManager_.GetNumIndexedValues() );
		while( luaManager_.BeginTable( index ) == true )
		{
            nowStateData.m_vecTalkBox.resize( nowStateData.m_vecTalkBox.size() + 1 );
            TalkBox& talkBox = nowStateData.m_vecTalkBox.back();

			LUA_GET_VALUE( luaManager_, "RATE",		talkBox.m_iRate,			0 );
			LUA_GET_VALUE( luaManager_, "MESSAGE",	talkBox.m_iMessage,	STR_ID_EMPTY );

			LUA_GET_VALUE( luaManager_, "DELAY_TIME",	talkBox.m_fDelayTime,		-1.f );
			luaManager_.EndTable();
			index++;
		}
		luaManager_.EndTable(); // TALK_BOX
	}

    nowStateData.m_veciEnableWeapon.resize( 0 );
    if( luaManager_.BeginTable( "ENABLE_WEAPON" ) == true )
    {
        index = 1;
        nowStateData.m_veciEnableWeapon.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, iTemp ) == true )
        {
            nowStateData.m_veciEnableWeapon.push_back( iTemp );
            index++;
        }
        luaManager_.EndTable();
    }
    nowStateData.m_veciDisableWeapon.resize( 0 );
    if( luaManager_.BeginTable( "DISABLE_WEAPON" ) == true )
    {
        index = 1;
        nowStateData.m_veciDisableWeapon.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, iTemp ) == true )
        {
            nowStateData.m_veciDisableWeapon.push_back( iTemp );
            index++;
        }
        luaManager_.EndTable();
    }


	bTemp = false;
	if( luaManager_.GetValue( "FLY_AI", bTemp ) == true)
	{
        NOWSTATE_SET_FLAG( SET_FLY_AI, true );
        NOWSTATE_SET_FLAG( SET_FLY_AI_VALUE, bTemp );
	}
	LUA_GET_VALUE( luaManager_, "ENABLE_HEIGHT_FIX",		bTemp,		false );
    NOWSTATE_SET_FLAG( ENABLE_HEIGHT_FIX, bTemp );

#if X2_DEV_TOOLS
	LUA_GET_VALUE( luaManager_, "ZERO_DAMAGE",				bTemp,			false );
    NOWSTATE_SET_FLAG( SET_ZERO_DAMAGE, bTemp );
#endif // DAMAGE_ZERO

	LUA_GET_VALUE( luaManager_, "NEVER_DIE_THIS_STATE",	bTemp,			false );
	NOWSTATE_SET_FLAG( NEVER_DIE_THIS_STATE, bTemp );

	LUA_GET_VALUE( luaManager_, "ABSORB_MAGIC",		bTemp,		false );
    NOWSTATE_SET_FLAG( SET_ABSORB_MAGIC_VALUE, bTemp );

	//{{ 오현빈 // 2012-08-01 // 해당 스테이트가 유지되는 동안 유저의 입력 제한과, NPC의 AI를 동작하지 않게 하는 기능
	LUA_GET_VALUE( luaManager_, "STOP_AT_CURRENT_STATE",			bTemp,			false );
    NOWSTATE_SET_FLAG( STOP_AT_CURRENT_STATE, bTemp );

	//{{ 오현빈 // 2012-08-01 // 해당 스테이트가 유지되는 동안 NPC에게 카메라가 가도록 하는 기능
	LUA_GET_VALUE( luaManager_, "FOCUS_CAMERA_AT_CURRENT_STATE",	bTemp,	false);
	NOWSTATE_SET_FLAG( FOCUS_CAMERA_AT_CURRENT_STATE, bTemp );

	LUA_GET_VALUE( luaManager_, "INVALID_LAST_TOUCH_INDEX",			bTemp,			false );
	NOWSTATE_SET_FLAG( INVALID_LAST_TOUCH_INDEX, bTemp );


	LUA_GET_VALUE( luaManager_, "FLIP_DIR_END", bTemp, false );
    NOWSTATE_SET_FLAG( FLIP_DIR_END, bTemp );

    if ( luaManager_.GetValue( "COOL_TIME", nowStateData.m_fSetCoolTimeValue ) == true )
    {
        NOWSTATE_SET_FLAG( SET_COOL_TIME, true );
    }

	if( luaManager_.BeginTable( "DEFENCE" ) == true )
	{
        NOWSTATE_SET_FLAG( SET_DEFENCE, true );
		fTemp = -1.0f;
		LUA_GET_VALUE( luaManager_, 1,	fTemp,	-1.0f );
		nowStateData.m_vDefenceTime.x = fTemp;
		fTemp = -1.0f;
		LUA_GET_VALUE( luaManager_, 2,	fTemp,	-1.0f );
		nowStateData.m_vDefenceTime.y = fTemp;
		fTemp = 0.0f;
		LUA_GET_VALUE( luaManager_, 3,	fTemp,	-1.0f );
		nowStateData.m_vDefenceTime.z = fTemp;

		luaManager_.EndTable();
	}
	else
	{
        NOWSTATE_SET_FLAG( SET_DEFENCE, false );
		nowStateData.m_vDefenceTime.x	= -1.0f;
		nowStateData.m_vDefenceTime.y	= -1.0f;
		nowStateData.m_vDefenceTime.z	= 0.0f;
	}

	if( luaManager_.BeginTable( "REVENGE" ) == true )
	{
        fTemp = -1.f;
		LUA_GET_VALUE( luaManager_, 1,	fTemp,	-1.0f );
		nowStateData.m_v2SetRevengeTimeValue.x = fTemp;
        fTemp = -1.f;
		LUA_GET_VALUE( luaManager_, 2,	fTemp,		-1.0f );
		nowStateData.m_v2SetRevengeTimeValue.y = fTemp;

		luaManager_.EndTable();
	}
	else
	{
		nowStateData.m_v2SetRevengeTimeValue.x	= -1.0f;
		nowStateData.m_v2SetRevengeTimeValue.y	= -1.0f;
	}

	if( luaManager_.BeginTable( "REFLECT_MAGIC" ) == true )
	{
        NOWSTATE_SET_FLAG( SET_REFLECT_MAGIC, true );
		fTemp = 0.f;
		LUA_GET_VALUE( luaManager_, 1,	fTemp,	0.f );	// 발동시작 시간
		nowStateData.m_v2SetReflectMagicValue.x = fTemp;
        fTemp = 0.f;
		LUA_GET_VALUE( luaManager_, 2,	fTemp,	0.f );	// 유지 시간
		nowStateData.m_v2SetReflectMagicValue.y = fTemp;
		//LUA_GET_VALUE( m_LuaManager, 3,	fReflect,	0.f );	// extra value
		//m_NPCFrameData.stateParam.m_vReflectMagic.z = fReflect;

		luaManager_.EndTable();
	}

	nowStateData.m_mapAddSlashTrace.clear();

	if( luaManager_.BeginTable( "SLASH_TRACE" ) == true )
	{
		D3DXVECTOR3 vSlashTraceTime;

		LUA_GET_VALUE( luaManager_, 1,	vSlashTraceTime.x,	-1.0f );
		LUA_GET_VALUE( luaManager_, 2,	vSlashTraceTime.y,	-1.0f );
		LUA_GET_VALUE( luaManager_, 4, vSlashTraceTime.z, 0.f );			// slash trace type

		nowStateData.m_mapAddSlashTrace[ 0 ] = vSlashTraceTime;

		luaManager_.EndTable();
	}

	int iTableIndex = 0;
	while( true == luaManager_.BeginTable( "SLASH_TRACE", iTableIndex ) )
	{
		int iWeaponIndex = 0;
		D3DXVECTOR3 vSlashTraceTime;

		LUA_GET_VALUE( luaManager_, 1,	vSlashTraceTime.x,	-1.0f );
		LUA_GET_VALUE( luaManager_, 2,	vSlashTraceTime.y,	-1.0f );
		LUA_GET_VALUE( luaManager_, 3, iWeaponIndex, 0 );
		LUA_GET_VALUE( luaManager_, 4, vSlashTraceTime.z, 0.f );			// slash trace type

		nowStateData.m_mapAddSlashTrace[ iWeaponIndex ] = vSlashTraceTime;

		luaManager_.EndTable(); // SLASH_TRACE%d
		iTableIndex++;
	}

	nowStateData.m_wstrAnimName.resize( 0 );
	LUA_GET_VALUE( luaManager_, "ANIM_NAME", nowStateData.m_wstrAnimName, L"" );

	LUA_GET_VALUE_ENUM ( luaManager_, "PLAY_TYPE", nowStateData.m_ePlayType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );

	//{{ kimhc // 2011.1.21 // 청 1차 전직
	// 스테이트 별 HittedType 지정
	LUA_GET_VALUE_ENUM( luaManager_, "HITTED_TYPE_AT_STATE", nowStateData.m_eHittedTypeAtState, CX2DamageManager::HITTED_TYPE, CX2DamageManager::HTD_NO_SOUND );
	//}} kimhc // 2011.1.21 // 청 1차 전직


    LUA_GET_VALUE( luaManager_, "ADD_POS_X", nowStateData.m_vAddPos.x, 0.f );
    LUA_GET_VALUE( luaManager_, "ADD_POS_Y", nowStateData.m_vAddPos.y, 0.f );
    LUA_GET_VALUE( luaManager_, "ADD_POS_Z", nowStateData.m_vAddPos.z, 0.f );

	LUA_GET_VALUE( luaManager_, "ADD_ROTATE_X", fTemp, 0.f );
    nowStateData.m_vAddRotateRadian.x = D3DXToRadian( fTemp );
	LUA_GET_VALUE( luaManager_, "ADD_ROTATE_Y", fTemp, 0.f );
    nowStateData.m_vAddRotateRadian.y = D3DXToRadian( fTemp );
	LUA_GET_VALUE( luaManager_, "ADD_ROTATE_Z", fTemp, 0.f );
    nowStateData.m_vAddRotateRadian.z = D3DXToRadian( fTemp );


	LUA_GET_VALUE( luaManager_, "GRAP_SHAKE_COUNT",	 nowStateData.m_iGrapShakeCount,  0 );




	index = 0;
	nowStateData.m_vecSpeedFactor.resize( 0 );
    nowStateData.m_vecSpeedFactor.reserve( luaManager_.GetNumIndexedTables( "SPEED_TIME" ) );
	while( luaManager_.BeginTable( "SPEED_TIME", index ) == true )
	{
		TIME_SPEED timeSpeed;

		LUA_GET_VALUE( luaManager_, 1, timeSpeed.vSpeed.x, 0.0f );		/// X축 속도
		LUA_GET_VALUE( luaManager_, 2, timeSpeed.vSpeed.y, 0.0f );		/// Y축 속도
		LUA_GET_VALUE( luaManager_, 3, timeSpeed.vSpeed.z, -1.0f );	/// 적용될 애니메이션 타임
		LUA_GET_VALUE( luaManager_, 4, timeSpeed.iFlag, 0 );			/// m_PhysicParam.nowSpeed, 1이면 +=, 2이면 = 연산을 수행함

        if ( timeSpeed.vSpeed.z >= 0.f && ( timeSpeed.iFlag == 1 || timeSpeed.iFlag == 2 ) )
        {
            nowStateData.m_vecSpeedFactor.push_back( timeSpeed );
        }

		index++;
		luaManager_.EndTable();
	}
    std::stable_sort( nowStateData.m_vecSpeedFactor.begin(), nowStateData.m_vecSpeedFactor.end() );



	if( true == luaManager_.GetValue( "G_ACCEL", nowStateData.m_fSetGAccelValue ) )
	{
        NOWSTATE_SET_FLAG( SET_G_ACCEL, true );
	}
    if ( true == luaManager_.GetValue( "DOWN_ACCEL", nowStateData.m_fSetDownAccelValue ) )
    {
        NOWSTATE_SET_FLAG( SET_DOWN_ACCEL, true );
    }
    if ( true == luaManager_.GetValue( "SPEED_X", nowStateData.m_v2SetSpeedValue.x ) )
    {
        NOWSTATE_SET_FLAG( SET_SPEED_X, true );
    }
    if ( true == luaManager_.GetValue( "SPEED_Y", nowStateData.m_v2SetSpeedValue.y ) )
    {
        NOWSTATE_SET_FLAG( SET_SPEED_Y, true );
    }
	LUA_GET_VALUE( luaManager_, "PASSIVE_SPEED_X", nowStateData.m_v2SetPassiveSpeedValue.x, -1.0f );
	LUA_GET_VALUE( luaManager_, "PASSIVE_SPEED_Y", nowStateData.m_v2SetPassiveSpeedValue.y, -1.0f );


	if( luaManager_.BeginTable( "AFTER_IMAGE" ) == true )
	{
		LUA_GET_VALUE( luaManager_, 1,	nowStateData.m_v2AfterImageTime.x,	-1.0f );
		LUA_GET_VALUE( luaManager_, 2,	nowStateData.m_v2AfterImageTime.y,	-1.0f );
		luaManager_.EndTable();
	}
	else
	{
		nowStateData.m_v2AfterImageTime.x	= -1.0f;
		nowStateData.m_v2AfterImageTime.y	= -1.0f;
	}

	if( luaManager_.BeginTable( "INVISIBLE_STATE" ) == true )
	{
		LUA_GET_VALUE( luaManager_, 1,	nowStateData.m_v2InvisibleTime.x,	-1.0f );
		LUA_GET_VALUE( luaManager_, 2,	nowStateData.m_v2InvisibleTime.y,	-1.0f );

		luaManager_.EndTable();
	}
	else
	{
		nowStateData.m_v2InvisibleTime.x	= -1.0f;
		nowStateData.m_v2InvisibleTime.y	= -1.0f;
	}

	index = 0;
    nowStateData.m_vecfEventIntervalTime.resize(0);
	while( true )
	{
		sprintf( aszTemp, "EVENT_INTERVAL_TIME%d", index );
        float   fInterval;
		if( luaManager_.GetValue( aszTemp, fInterval ) == false )
			break;
        nowStateData.m_vecfEventIntervalTime.push_back( fInterval );
		index++;
	}

	index = 0;
    nowStateData.m_vecAttackTime.resize( 0 );
    nowStateData.m_vecAttackTime.reserve( luaManager_.GetNumIndexedTables( "ATTACK_TIME" ) );
	while( luaManager_.BeginTable( "ATTACK_TIME", index ) == true )
	{
        nowStateData.m_vecAttackTime.resize( nowStateData.m_vecAttackTime.size() + 1 );
        KAttackTimeProjSeqName& kNode = nowStateData.m_vecAttackTime.back();

		LUA_GET_VALUE( luaManager_, 1, kNode.m_v2AttackTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, kNode.m_v2AttackTime.y, -1.0f );
        kNode.m_wstrProjSeqName.resize( 0 );
		luaManager_.GetValue( 3, kNode.m_wstrProjSeqName );
		index++;
		luaManager_.EndTable();
	}

    for( int i = 0; i < SOUND_SET_NUMS; ++i )
    {
        nowStateData.m_avecSoundPlayData[i].resize(0);
    }

	index = 0;
    {
        std::vector<SoundPlayData>& vecSoundPlayData = nowStateData.m_avecSoundPlayData[ SOUND_SET_DEFUALT ];
        vecSoundPlayData.reserve( luaManager_.GetNumIndexedTables( "SOUND_PLAY" ) );
	    while( luaManager_.BeginTable( "SOUND_PLAY", index ) == true )
	    {
            vecSoundPlayData.resize( vecSoundPlayData.size() + 1 );
		    CX2GameUnit::SoundPlayData& sndPlayData = vecSoundPlayData.back();

		    LUA_GET_VALUE( luaManager_, 1, sndPlayData.m_SoundPlayTime,		-1.0f	);
		    LUA_GET_VALUE( luaManager_, 2, sndPlayData.m_SoundPlayName,		L""		);
		    LUA_GET_VALUE( luaManager_, 3, sndPlayData.m_SoundPlayRate,		100		);
		    LUA_GET_VALUE( luaManager_, 4, sndPlayData.m_bOnlyIfMyUnit,		false	);
		    LUA_GET_VALUE( luaManager_, 5, sndPlayData.m_fMaxSoundDistance,	-1.f	);
		    LUA_GET_VALUE( luaManager_, 6, sndPlayData.m_b3DSound,				true	);

		    index++;
		    luaManager_.EndTable();
	    }
    }

    nowStateData.m_vecSetSoundPlayLoopValue.resize( 0 );
    bTemp = false;
    LUA_GET_VALUE( luaManager_, "STOP_LOOP_SOUND",	bTemp,	false );
    if ( bTemp == true )
    {
        NOWSTATE_SET_FLAG( STOP_LOOP_SOUND, true );
    }
    else
    {
	    index = 0;
        nowStateData.m_vecSetSoundPlayLoopValue.resize(  luaManager_.GetNumIndexedTables( "SOUND_PLAY_LOOP" ) );
	    while( luaManager_.BeginTable( "SOUND_PLAY_LOOP", index ) == true )
	    {
            nowStateData.m_vecSetSoundPlayLoopValue.resize( nowStateData.m_vecSetSoundPlayLoopValue.size() + 1 );
		    CX2GameUnit::SoundPlayData& sndPlayData = nowStateData.m_vecSetSoundPlayLoopValue.back();

		    LUA_GET_VALUE( luaManager_, 1, sndPlayData.m_SoundPlayTime,		-1.0f	);
		    LUA_GET_VALUE( luaManager_, 2, sndPlayData.m_SoundPlayName,		L""		);
		    LUA_GET_VALUE( luaManager_, 3, sndPlayData.m_SoundPlayRate,		100		);
		    LUA_GET_VALUE( luaManager_, 4, sndPlayData.m_bOnlyIfMyUnit,		false	);
		    LUA_GET_VALUE( luaManager_, 5, sndPlayData.m_fMaxSoundDistance,	-1.f	);
		    LUA_GET_VALUE( luaManager_, 6, sndPlayData.m_b3DSound,				true	);
		    MakeUpperCase( sndPlayData.m_SoundPlayName );
		    index++;
		    luaManager_.EndTable();
	    }
    }


	LUA_GET_VALUE( luaManager_, "SOUND_SET0_RATE", nowStateData.m_iSoundSet0Rate, 0 );
	LUA_GET_VALUE( luaManager_, "SOUND_SET1_RATE", nowStateData.m_iSoundSet1Rate, 0 );
	LUA_GET_VALUE( luaManager_, "SOUND_SET2_RATE", nowStateData.m_iSoundSet2Rate, 0 );

    for( int iSet = 0; iSet <= 2; ++iSet )
    {
	    if ( luaManager_.BeginTable( "SOUND_SET", iSet ) == true )
	    {
		    bool bUseHyperSound = false;
		    bool bUseBerserkSound = false;
		    LUA_GET_VALUE( luaManager_, "USE_HYPER_SOUND", bUseHyperSound, false );
		    LUA_GET_VALUE( luaManager_, "USE_BERSERK_SOUND", bUseBerserkSound, false );
            switch( iSet )
            {
            case 0:
                NOWSTATE_SET_FLAG( SOUND_SET0, true );
                NOWSTATE_SET_FLAG( SOUND_SET0_USE_HYPER_SOUND, bUseHyperSound );
                NOWSTATE_SET_FLAG( SOUND_SET0_USE_BERSERK_SOUND, bUseBerserkSound );
                break;
            case 1:
                NOWSTATE_SET_FLAG( SOUND_SET1, true );
                NOWSTATE_SET_FLAG( SOUND_SET1_USE_HYPER_SOUND, bUseHyperSound );
                NOWSTATE_SET_FLAG( SOUND_SET1_USE_BERSERK_SOUND, bUseBerserkSound );
                break;
            case 2:
                NOWSTATE_SET_FLAG( SOUND_SET2, true );
                NOWSTATE_SET_FLAG( SOUND_SET2_USE_HYPER_SOUND, bUseHyperSound );
                NOWSTATE_SET_FLAG( SOUND_SET2_USE_BERSERK_SOUND, bUseBerserkSound );
                break;
            };//

            static const char* s_apszMode[3] = { "NORMAL", "HYPER", "BERSERK" };
            for( int iMode = 0; iMode < 3; ++iMode )
            {
                int iSoundDataIndex = SOUND_SET0_NORMAL + iSet * 3 + iMode;
                std::vector<SoundPlayData>& vecSoundPlayData = nowStateData.m_avecSoundPlayData[iSoundDataIndex];
                vecSoundPlayData.resize( 0 );
		        if( luaManager_.BeginTable( s_apszMode[iMode] ) == true )
		        {
			        index = 0;
                    vecSoundPlayData.reserve( luaManager_.GetNumIndexedTables( "SOUND_PLAY" ) );
			        while( luaManager_.BeginTable( "SOUND_PLAY", index ) == true )
			        {
                        vecSoundPlayData.resize( vecSoundPlayData.size() + 1 );
		                CX2GameUnit::SoundPlayData& sndPlayData = vecSoundPlayData.back();
				        LUA_GET_VALUE( luaManager_, 1, sndPlayData.m_SoundPlayTime,	-1.0f	);
				        LUA_GET_VALUE( luaManager_, 2, sndPlayData.m_SoundPlayName,	L""		);
				        LUA_GET_VALUE( luaManager_, 3, sndPlayData.m_SoundPlayRate,	100		);
				        LUA_GET_VALUE( luaManager_, 4, sndPlayData.m_bOnlyIfMyUnit,	false	);
				        LUA_GET_VALUE( luaManager_, 5, sndPlayData.m_fMaxSoundDistance,	-1.f	);
				        LUA_GET_VALUE( luaManager_, 6, sndPlayData.m_b3DSound,				true	);
				        index++;
				        luaManager_.EndTable();	// SOUND_PLAY#
			        }
		            luaManager_.EndTable();	// 이름없는 Table
		        }
            }
		    luaManager_.EndTable();	// SOUND_SET
	    }
    }

    ParseStopTime( nowStateData, luaManager_ );

	//{{ JHKang / 강정훈 / 2011/01/13 / 몬스터 State당 렌더링 방식 변경
    iTemp = 0;
    if ( luaManager_.GetValue( "RENDER_PARAM", iTemp ) == true )
    {
        NOWSTATE_SET_FLAG( SET_RENDER_PARAM, true );
        nowStateData.m_eSetRenderParamValue = (CKTDGXRenderer::RENDER_TYPE) iTemp;
    }
	//}} JHKang / 강정훈 / 2011/01/13

	LUA_GET_VALUE( luaManager_, "DAMAGE_DATA_CHANGE_TIME",			nowStateData.m_fCommonDamageChangeTime,		-1.f );



    nowStateData.m_vecSuperArmorTime.resize(0);
	int indexSuperArmorTimeTable = 0;
	while( luaManager_.BeginTable( "SUPER_ARMOR_TIME", indexSuperArmorTimeTable ) == true )
	{
		indexSuperArmorTimeTable++;
		D3DXVECTOR2 superArmorTime;
		LUA_GET_VALUE( luaManager_, 1, superArmorTime.x, 0.f );
		LUA_GET_VALUE( luaManager_, 2, superArmorTime.y, 0.f );
		bool bGlobalSuperArmor;
		LUA_GET_VALUE( luaManager_, 3, bGlobalSuperArmor, false );
		if ( true == bGlobalSuperArmor )
			nowStateData.m_fSetGlobalSuperArmorTimeValue = superArmorTime.y;
        else
		    nowStateData.m_vecSuperArmorTime.push_back(superArmorTime);
		luaManager_.EndTable();
	}

	if( luaManager_.BeginTable( "INVINCIBLE" ) == true )
	{
		float fInvin = -1.0f;
		LUA_GET_VALUE( luaManager_, 1,	fInvin,	-1.0f );
		nowStateData.m_v2SetInvincibleTimeValue.x = fInvin;
		fInvin = -1.0f;
		LUA_GET_VALUE( luaManager_, 2,	fInvin,	-1.0f );
		nowStateData.m_v2SetInvincibleTimeValue.y = fInvin;
		fInvin = -1.f;
		LUA_GET_VALUE( luaManager_, 3,	fInvin,	-1.0f );
        nowStateData.m_fSetForceInvincibleTimeValue = fInvin;
		fInvin = -1.f;
        LUA_GET_VALUE( luaManager_, 4,	fInvin,	-1.f );
        nowStateData.m_fSetShowInvincibleTimeValue = fInvin;
		luaManager_.EndTable();
	}
	else
	{
		nowStateData.m_v2SetInvincibleTimeValue.x	= -1.0f;
		nowStateData.m_v2SetInvincibleTimeValue.y	= -1.0f;
        nowStateData.m_fSetForceInvincibleTimeValue = -1.0f;
        nowStateData.m_fSetShowInvincibleTimeValue = -1.0f;
	}


	LUA_GET_VALUE( luaManager_, "ANIM_WAIT_TIME", nowStateData.m_fSetAnimWaitTimeValue, 0.0f );
    if ( luaManager_.GetValue( "ANIM_SPEED", nowStateData.m_fSetAnimSpeedValue ) == true )
    {
        NOWSTATE_SET_FLAG( SET_ANIM_SPEED, true );
    }
    else
    {
        nowStateData.m_fSetAnimSpeedValue = OutInit_.m_component.m_fDefaultAnimSpeed;
    }

    nowStateData.m_vecfHintAnimTimer.resize( 0 );


    nowStateData.m_vecNPCEventProcessData.resize( 0 );
	if( luaManager_.BeginTable( "EVENT_PROCESS" ) == true )
	{
		int index = 1;
        while( luaManager_.BeginTable( index ) == true )
        {
            index++;
		    luaManager_.EndTable();
        }//while
        nowStateData.m_vecNPCEventProcessData.resize( index - 1 );
        index = 1;
        string strNextState;
		while( luaManager_.BeginTable( index ) == true )
		{
			NPCEventProcessData* pEventProcessData = &nowStateData.m_vecNPCEventProcessData[ index - 1 ];

			LUA_GET_VALUE_ENUM( luaManager_,	1,	pEventProcessData->m_StateChangeType,		STATE_CHANGE_TYPE, SCT_NO_CHANGE );
//{{ robobeg : 2008-10-28
			//LUA_GET_VALUE( luaManager_,		2,	pEventProcessData->m_wstrNextState,			L"" );
            strNextState.resize(0);
            LUA_GET_VALUE_UTF8( luaManager_,		2,	strNextState,			"" );
            pEventProcessData->m_NextStateID = OutInit_.m_state.GetStateID( strNextState );
            //ASSERT( pEventProcessData->m_NextStateID > 0 );

//}} robobeg : 2008-10-28

            LUA_GET_VALUE_UTF8( luaManager_,		3,	strTemp,	"" );

            switch ( pEventProcessData->m_StateChangeType )
            {
            case SCT_NO_CHANGE:
            case SCT_MOTION_END:
            case SCT_FOOT_ON_LINE_FALSE_DOWN:
            case SCT_FOOT_ON_LINE_TRUE:
            case SCT_POSITIVE_Y_SPEED:
            case SCT_NEGATIVE_Y_SPEED:
                break;
            case SCT_CONDITION_FUNCTION:
                {
                    pEventProcessData->m_strCustomCondition = strTemp;

                }
                break;
            case SCT_CONDITION_TABLE:
			    {
				    luaManager_.EndTable(); // index
				    luaManager_.EndTable(); // EVENT_PROCESS

				    if( strTemp.empty() == false
                        && luaManager_.BeginTable( strTemp.c_str() ) == true )
				    {
					    ConditionTableData* pConditionTableData = new ConditionTableData;

                        LUA_GET_VALUE_UTF8( luaManager_, "CONDITION_FUNCTION",			pEventProcessData->m_strCustomCondition,			"" );

					    LUA_GET_VALUE( luaManager_, "EVENT_INTERVAL_ID",			pConditionTableData->m_iIntervalID,					-1 );
					    LUA_GET_VALUE( luaManager_, "DISTANCE_TO_TARGET_NEAR",		pConditionTableData->m_fDistanceToTarget,			-1.0f );
					    LUA_GET_VALUE( luaManager_, "DISTANCE_OVER_TARGET_NEAR",	pConditionTableData->m_fDistanceOverTarget,			-1.0f );
					    LUA_GET_VALUE( luaManager_, "TARGET_ABOVE_ME",				pConditionTableData->m_bTargetAboveMe,				false );
					    LUA_GET_VALUE( luaManager_, "TARGET_BELOW_ME",				pConditionTableData->m_bTargetBelowMe,				false );
					    LUA_GET_VALUE( luaManager_, "ANIM_PLAY_COUNT",				pConditionTableData->m_iAnimPlayCount,				-1 );
					    LUA_GET_VALUE( luaManager_, "RATE",						pConditionTableData->m_iRate,						-1 );

                        if ( pConditionTableData->m_iRate >= 100 )
                            pConditionTableData->m_iRate = -1;

					    LUA_GET_VALUE( luaManager_, "SMALL_THEN_Y_SPEED",			pConditionTableData->m_fSmallThenYSpeedFactor,		-1.0f );
					    LUA_GET_VALUE( luaManager_, "STATE_TIME_OVER",				pConditionTableData->m_fStateTimeOverFactor,		-1.0f );
					    LUA_GET_VALUE( luaManager_, "ANIM_TIMER",			        pConditionTableData->m_fAnimTimerFactor,		-1.0f );
                        if ( luaManager_.GetValue( "ANIM_EVENT_TIMER", fTemp ) == true )
                            pConditionTableData->m_fAnimTimerFactor = fTemp;
                        if ( pConditionTableData->m_fAnimTimerFactor != -1.0f )
                            nowStateData.m_vecfHintAnimTimer.push_back( pConditionTableData->m_fAnimTimerFactor );

					    LUA_GET_VALUE( luaManager_, "MY_HP_LESS_THAN_PERCENT",		pConditionTableData->m_fMyHPLessThanPercentFactor,	-1.0f );
					    LUA_GET_VALUE( luaManager_, "MY_HP_MORE_THAN_PERCENT",		pConditionTableData->m_fMyHPMoreThanPercentFactor,	-1.0f );
					    LUA_GET_VALUE( luaManager_, "MY_MP_LESS_THAN_PERCENT",		pConditionTableData->m_fMyMPLessThanPercentFactor,	-1.0f );
					    LUA_GET_VALUE( luaManager_, "MY_MP_MORE_THAN_PERCENT",		pConditionTableData->m_fMyMPMoreThanPercentFactor,	-1.0f );
						LUA_GET_VALUE( luaManager_, "TARGET_HP_LESS_THAN_PERCENT",		pConditionTableData->m_fTargetHPLessThanPercentFactor,	-1.f );
						LUA_GET_VALUE( luaManager_, "TARGET_HP_MORE_THAN_PERCENT",		pConditionTableData->m_fTargetHPMoreThanPercentFactor,	-1.f );
					    LUA_GET_VALUE( luaManager_, "MY_MP_MORE_THAN",				pConditionTableData->m_fMyMPMoreThanFactor,	-1.0f );
					    LUA_GET_VALUE( luaManager_, "ATTACK_SUCCESS",				pConditionTableData->m_bAttackSuccessFactor,		false );
					    LUA_GET_VALUE( luaManager_, "SAME_LINE_WITH_TARGET",		pConditionTableData->m_bSameLineWithTargetFactor,	false );
					    LUA_GET_VALUE( luaManager_, "FLAG_TRUE",					pConditionTableData->m_iFlagTrue,					-1 );
					    LUA_GET_VALUE( luaManager_, "FLAG_FALSE",					pConditionTableData->m_iFlagFalse,					-1 );
					    //{{oasis:김상윤////2009-10-7////
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_TRUE0",		pConditionTableData->m_iGroupAICommandTrue[0],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_TRUE1",		pConditionTableData->m_iGroupAICommandTrue[1],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_TRUE2",		pConditionTableData->m_iGroupAICommandTrue[2],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_TRUE3",		pConditionTableData->m_iGroupAICommandTrue[3],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );

					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_FALSE0",		pConditionTableData->m_iGroupAICommandFalse[0],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_FALSE1",		pConditionTableData->m_iGroupAICommandFalse[1],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_FALSE2",		pConditionTableData->m_iGroupAICommandFalse[2],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );
					    LUA_GET_VALUE_ENUM( luaManager_, "COMMAND_FALSE3",		pConditionTableData->m_iGroupAICommandFalse[3],	CX2GUNPC::GROUP_AI_COMMAND,	CX2GUNPC::GAI_CMD_OFF );

					    //}}oasis:김상윤////2009-10-7////
					    LUA_GET_VALUE( luaManager_, "ESCAPE_CHECK",				pConditionTableData->m_bEscapeCheck,				false );  // 도망성공여부 체크
					    LUA_GET_VALUE( luaManager_, "HAVE_TARGET",					pConditionTableData->m_iHaveTarget,					-1 );
                        LUA_GET_VALUE( luaManager_, "HIGHER_THAN_POS_Y",			pConditionTableData->m_fHigherThanPosY,				-1.0f );
					    if ( luaManager_.GetValue( "HIGHER_THEN_POS_Y", fTemp ) == true )
                            pConditionTableData->m_fHigherThanPosY = fTemp;

					    LUA_GET_VALUE( luaManager_, "ALIVE_USER_GEQ",		pConditionTableData->m_iAliveUserUnitGeq,		-1 );
					    LUA_GET_VALUE( luaManager_, "ALIVE_USER_LEQ",		pConditionTableData->m_iAliveUserUnitLeq,		-1 );
					    LUA_GET_VALUE( luaManager_, "USER_GEQ",				pConditionTableData->m_iUserUnitGeq,			-1 );
					    LUA_GET_VALUE( luaManager_, "USER_LEQ",				pConditionTableData->m_iUserUnitLeq,			-1 );
                        if ( luaManager_.GetValue( "ALIVE_USER_MORE_THAN", iTemp ) == true )
                            pConditionTableData->m_iAliveUserUnitGeq = iTemp;
                        if ( luaManager_.GetValue( "ALIVE_USER_LESS_THAN", iTemp ) == true )
                            pConditionTableData->m_iAliveUserUnitLeq = iTemp;
                        if ( luaManager_.GetValue( "USER_MORE_THAN", iTemp ) == true )
                            pConditionTableData->m_iUserUnitGeq = iTemp;
                        if ( luaManager_.GetValue( "USER_LESS_THAN", iTemp ) == true )
                            pConditionTableData->m_iUserUnitLeq = iTemp;


					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED0",				pConditionTableData->m_fTimerElapsed[0],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED1",				pConditionTableData->m_fTimerElapsed[1],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED2",				pConditionTableData->m_fTimerElapsed[2],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED3",				pConditionTableData->m_fTimerElapsed[3],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED4",				pConditionTableData->m_fTimerElapsed[4],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED5",				pConditionTableData->m_fTimerElapsed[5],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED6",				pConditionTableData->m_fTimerElapsed[6],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED7",				pConditionTableData->m_fTimerElapsed[7],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED8",				pConditionTableData->m_fTimerElapsed[8],			-1.f );
					    LUA_GET_VALUE( luaManager_, "TIMER_ELAPSED9",				pConditionTableData->m_fTimerElapsed[9],			-1.f );

                        LUA_GET_VALUE( luaManager_, "GE_KEYCODE",				    pConditionTableData->m_iGEKeyCode,			-1 );

					    LUA_GET_VALUE( luaManager_, "CHECK_TARGET_DIRECTION",		pConditionTableData->m_bDirTarget,			false );
						LUA_GET_VALUE( luaManager_, "VIEW_TARGET_DIRECTION",		pConditionTableData->m_bViewTargetDir,		false );
					    LUA_GET_VALUE( luaManager_, "TARGET_RIGHT",				pConditionTableData->m_bRightTarget,		false );


					    //{{ oasis907 : 김상윤 [2010.8.1] //
					    LUA_GET_VALUE( luaManager_, "FOOT_ON_LINE",					pConditionTableData->m_bFootOnLine,		false );
					    //}} oasis907 : 김상윤 [2010.8.1] //
					    LUA_GET_VALUE( luaManager_, "IS_RAGEMODE",					pConditionTableData->m_bRageMode,		false );

					    LUA_GET_VALUE( luaManager_, "IS_ANOTHER_TEAM",					pConditionTableData->m_bCheckAnotherTeam,		false );

                        pConditionTableData->m_bNPCNonHostSimulatable = false == (
                            pEventProcessData->m_strCustomCondition.empty() == false ||

                            ( pConditionTableData->m_iRate != -1 && pConditionTableData->m_iRate <= 0 )
                            || ( pConditionTableData->m_iRate > 0 && pConditionTableData->m_iRate < 100
                            && pConditionTableData->m_iAnimPlayCount == -1 && pConditionTableData->m_fAnimTimerFactor == -1
                            && pConditionTableData->m_iIntervalID == -1 )
                            || pConditionTableData->m_bEscapeCheck == true
                            || pConditionTableData->m_iFlagTrue != -1
                            || pConditionTableData->m_iFlagFalse != -1
                            || pConditionTableData->m_iGroupAICommandTrue[0] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandTrue[1] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandTrue[2] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandTrue[3] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandFalse[0] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandFalse[1] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandFalse[2] != CX2GUNPC::GAI_CMD_OFF
                            || pConditionTableData->m_iGroupAICommandFalse[3] != CX2GUNPC::GAI_CMD_OFF );

                        pConditionTableData->m_bRequireTarget = ( pConditionTableData->m_fDistanceToTarget != -1.0f
                            || pConditionTableData->m_fDistanceOverTarget != -1.0f
                            || pConditionTableData->m_bTargetAboveMe != false
                            || pConditionTableData->m_bTargetBelowMe != false
                            || pConditionTableData->m_bSameLineWithTargetFactor != false
                            || pConditionTableData->m_iHaveTarget != -1
                            || pConditionTableData->m_bDirTarget == true
                            || pConditionTableData->m_bCheckAnotherTeam == true
							|| pConditionTableData->m_bViewTargetDir == true
							|| pConditionTableData->m_fTargetHPLessThanPercentFactor != -1.f
							|| pConditionTableData->m_fTargetHPMoreThanPercentFactor != -1.f
							);

					    pEventProcessData->m_pConditionTableData = pConditionTableData;

                        if ( OutInit_.m_state.m_bSyncTargetInfo == false
                            && NOWSTATE_FLAG( REACTION_STATE ) == true
                            && pConditionTableData->m_bNPCNonHostSimulatable == true
                            && pConditionTableData->m_bRequireTarget == true )
                        {
                            OutInit_.m_state.m_bSyncTargetInfo = true;
                        }

					    luaManager_.EndTable(); //pEventProcessData.m_strCustomCondition.c_str()
				    }

				    luaManager_.BeginTable( "EVENT_PROCESS" );
				    luaManager_.BeginTable( index );
			    }
                break;
            default:
                break;
            }//switch
			luaManager_.EndTable(); // index
			index++;
		}
		luaManager_.EndTable(); // EVENT_PROCESS
	}


    NOWSTATE_SET_FLAG( HINT_NO_PERFRAME_EVENT, true );

	LUA_GET_VALUE( luaManager_, "HINT_DAMAGED_EVENT", bTemp, false );
    NOWSTATE_SET_FLAG( HINT_DAMAGED_EVENT, bTemp );

    nowStateData.m_vecfHintAnimEventTimer.resize( 0 );
	if ( luaManager_.BeginTable( "HINT_ANIM_EVENT_TIMER" ) == true )
    {
        index = 1;
        nowStateData.m_vecfHintAnimEventTimer.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, fTemp ) == true )
        {
            nowStateData.m_vecfHintAnimEventTimer.push_back( fTemp );
            index++;
        }//while
        luaManager_.EndTable();
        std::stable_sort( nowStateData.m_vecfHintAnimEventTimer.begin(), nowStateData.m_vecfHintAnimEventTimer.end() );
        std::vector<float>::iterator iterEnd = std::unique( nowStateData.m_vecfHintAnimEventTimer.begin(), nowStateData.m_vecfHintAnimEventTimer.end() );
        nowStateData.m_vecfHintAnimEventTimer.resize( std::distance( nowStateData.m_vecfHintAnimEventTimer.begin(), iterEnd ) );
    }//if

    nowStateData.m_vecfHintVisualAnimEventTimer.resize( 0 );
	if ( luaManager_.BeginTable( "HINT_VISUAL_ANIM_EVENT_TIMER" ) == true )
    {
        index = 1;
        nowStateData.m_vecfHintVisualAnimEventTimer.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, fTemp ) == true )
        {
            nowStateData.m_vecfHintVisualAnimEventTimer.push_back( fTemp );
            index++;
        }//while
        luaManager_.EndTable();
        std::stable_sort( nowStateData.m_vecfHintVisualAnimEventTimer.begin(), nowStateData.m_vecfHintVisualAnimEventTimer.end() );
        std::vector<float>::iterator iterEnd = std::unique( nowStateData.m_vecfHintVisualAnimEventTimer.begin(), nowStateData.m_vecfHintVisualAnimEventTimer.end() );
        nowStateData.m_vecfHintVisualAnimEventTimer.resize( std::distance( nowStateData.m_vecfHintVisualAnimEventTimer.begin(), iterEnd ) );
    }//if

    //nowStateData.m_vecfHintAnimTimer.resize( 0 );
	if ( luaManager_.BeginTable( "HINT_ANIM_TIMER" ) == true )
    {
        index = 1;
        nowStateData.m_vecfHintAnimTimer.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, fTemp ) == true )
        {
            nowStateData.m_vecfHintAnimTimer.push_back( fTemp );
            index++;
        }//while
        luaManager_.EndTable();
    }
    if ( nowStateData.m_vecfHintAnimTimer.empty() == false )
    {
        std::stable_sort( nowStateData.m_vecfHintAnimTimer.begin(), nowStateData.m_vecfHintAnimTimer.end() );
        std::vector<float>::iterator iterEnd = std::unique( nowStateData.m_vecfHintAnimTimer.begin(), nowStateData.m_vecfHintAnimTimer.end() );
        nowStateData.m_vecfHintAnimTimer.resize( std::distance( nowStateData.m_vecfHintAnimTimer.begin(), iterEnd ) );
    }//if

    nowStateData.m_vecfHintVisualAnimTimer.resize( 0 );
	if ( luaManager_.BeginTable( "HINT_VISUAL_ANIM_TIMER" ) == true )
    {
        index = 1;
        nowStateData.m_vecfHintVisualAnimTimer.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, fTemp ) == true )
        {
            nowStateData.m_vecfHintVisualAnimTimer.push_back( fTemp );
            index++;
        }//while
        luaManager_.EndTable();
        std::stable_sort( nowStateData.m_vecfHintVisualAnimTimer.begin(), nowStateData.m_vecfHintVisualAnimTimer.end() );
        std::vector<float>::iterator iterEnd = std::unique( nowStateData.m_vecfHintVisualAnimTimer.begin(), nowStateData.m_vecfHintVisualAnimTimer.end() );
        nowStateData.m_vecfHintVisualAnimTimer.resize( std::distance( nowStateData.m_vecfHintVisualAnimTimer.begin(), iterEnd ) );
    }//if

    nowStateData.m_vecfHintEventTimer.resize( 0 );
    if ( luaManager_.BeginTable( "HINT_EVENT_TIMER" ) == true )
    {
        index = 1;
        nowStateData.m_vecfHintEventTimer.reserve( luaManager_.GetNumIndexedValues() );
        while( luaManager_.GetValue( index, fTemp ) == true )
        {
            nowStateData.m_vecfHintEventTimer.push_back( fTemp );
            index++;
        }//while
        luaManager_.EndTable();
        std::stable_sort(nowStateData.m_vecfHintEventTimer.begin(), nowStateData.m_vecfHintEventTimer.end() );
        std::vector<float>::iterator iterEnd = std::unique( nowStateData.m_vecfHintEventTimer.begin(), nowStateData.m_vecfHintEventTimer.end() );
        nowStateData.m_vecfHintEventTimer.resize( std::distance( nowStateData.m_vecfHintEventTimer.begin(), iterEnd ) );
    }//if

    if ( luaManager_.GetValue( "HINT_NO_PERFRAME_EVENT", bTemp ) == true )
    {
        NOWSTATE_SET_FLAG( HINT_NO_PERFRAME_EVENT, bTemp );
    }//if



//#ifdef TRANSFORMER_TEST
//
//    nowStateData.m_vecKeyEventProcessData.resize(0);
//	if( true == luaManager_.BeginTable( "KEYBOARD_EVENT_PROCESS" ) )
//	{
//		wstring wstrKeyEventFailNextStateID = L"";
//		LUA_GET_VALUE( luaManager_, "DEFAULT_NEXT_STATE_ID", wstrKeyEventFailNextStateID, L"" );
//		nowStateData.m_cKeyEventFailNextStateID = GetStateID( wstrKeyEventFailNextStateID );
//
//		int index = 1;
//        while( luaManager_.BeginTable( index ) == true )
//        {
//            index++;
//		    luaManager_.EndTable();
//        }//while
//        nowStateData.m_vecKeyEventProcessData.resize( index - 1 );
//		for( int iEventTableIndex = 1; true == luaManager_.BeginTable( iEventTableIndex ); ++iEventTableIndex )
//		{
//			KeyEventProcessData& keyEventProcessData = nowStateData.m_vecKeyEventProcessData[ iEventTableIndex-1 ];
//
//			for( int iKeyTableIndex = 1; true == luaManager_.BeginTable( iKeyTableIndex ) ; ++iKeyTableIndex )
//			{
//				for( int iValueIndex = 1; ; iValueIndex += 2 )
//				{
//					KeyCombinationData keyCombinationdata;
//
//					LUA_GET_VALUE_ENUM( luaManager_, iValueIndex,	keyCombinationdata.eType, KEYBOARD_EVENT_TYPE, KET_NONE );
//					LUA_GET_VALUE_RETURN( luaManager_, iValueIndex+1,	keyCombinationdata.bPressed, true, break; );
//
//					if( KET_NONE == keyCombinationdata.eType )
//						break;
//
//					keyEventProcessData.m_vecKeyCombinationData.push_back( keyCombinationdata );
//				}
//
//				luaManager_.EndTable(); // iKeyTableIndex
//			}
//
//			wstring wstrNextStateID = L"";
//			LUA_GET_VALUE( luaManager_, "NEXT_STATE_ID", wstrNextStateID, L"" );
//			keyEventProcessData.m_cNextStateID = OutInit_.m_state.GetStateID( wstrNextStateID );
//			//ASSERT( false == wstrNextStateID.empty() );
//			//ASSERT( keyEventProcessData.m_cNextStateID >= 0 );
//
//
//			int iIsRight = -1;
//			LUA_GET_VALUE( luaManager_, "IS_RIGHT", iIsRight, -1 );
//			keyEventProcessData.m_cIsRight = (char) iIsRight;
//
//
//
//			LUA_GET_VALUE( luaManager_, "DISPLACE_X", keyEventProcessData.m_vDisplace.x, 0.f );
//			LUA_GET_VALUE( luaManager_, "DISPLACE_Y", keyEventProcessData.m_vDisplace.y, 0.f );
//			LUA_GET_VALUE( luaManager_, "DISPLACE_Z", keyEventProcessData.m_vDisplace.z, 0.f );
//
//
//			luaManager_.EndTable(); // iEventTableIndex
//		}
//
//		luaManager_.EndTable(); // KEYBOARD_EVENT_PROCESS
//	}
//
//
//#endif TRANSFORMER_TEST




	nowStateData.m_vecAnimSpeed.resize(0);
	if( luaManager_.BeginTable( "ANIM_SPEED_SET" ) == true )
	{
		int index = 1;
        nowStateData.m_vecParticleSeq.reserve( luaManager_.GetNumIndexedTables() );
		while( luaManager_.BeginTable( index ) == true )
		{
			AnimSpeed animSpeed;
			LUA_GET_VALUE( luaManager_, 1,	animSpeed.m_fStartTime,		-1.f );
			LUA_GET_VALUE( luaManager_, 2,	animSpeed.m_fEndTime,		-1.f );
			LUA_GET_VALUE( luaManager_, 3, animSpeed.m_fAnimSpeed,		1.f );

			nowStateData.m_vecAnimSpeed.push_back( animSpeed );
			luaManager_.EndTable(); // index
			index++;
		}

		luaManager_.EndTable(); // ANIM_SPEED_SET
	}

    nowStateData.m_vecParticleSeq.resize( 0 );
	if( luaManager_.BeginTable( "PARTICLE_SEQ" ) == true )
	{
        nowStateData.m_vecParticleSeq.reserve( luaManager_.GetNumIndexedTables() );
		int index = 1;
		while( luaManager_.BeginTable( index ) == true )
		{
			nowStateData.m_vecParticleSeq.resize( nowStateData.m_vecParticleSeq.size() + 1 );
            ParticleEffectData& particleEffectData = nowStateData.m_vecParticleSeq.back();

			LUA_GET_VALUE( luaManager_, 1, particleEffectData.m_bMajor,				false	);
			LUA_GET_VALUE( luaManager_, 2, particleEffectData.m_fTime,				0.0f	);
			LUA_GET_VALUE( luaManager_, 3, particleEffectData.m_Name,				L""		);
			LUA_GET_VALUE( luaManager_, 4, particleEffectData.m_bWeapon,				false	);
			LUA_GET_VALUE( luaManager_, 5, particleEffectData.m_Pos,					L""		);
			LUA_GET_VALUE( luaManager_, 6, particleEffectData.m_StateEndDelete,		false	);

			LUA_GET_VALUE( luaManager_, 7, particleEffectData.m_bTrace,					true );
			LUA_GET_VALUE( luaManager_, 8, particleEffectData.m_vOffset.x,				0.f );
			LUA_GET_VALUE( luaManager_, 9, particleEffectData.m_vOffset.y,				0.f );
			LUA_GET_VALUE( luaManager_, 10, particleEffectData.m_vOffset.z,				0.f );

			LUA_GET_VALUE( luaManager_, 11, particleEffectData.m_bApplyUnitRotation,		false );
			LUA_GET_VALUE( luaManager_, 12, particleEffectData.m_vRotation.x,			0.f );
			LUA_GET_VALUE( luaManager_, 13, particleEffectData.m_vRotation.y,			0.f );
			LUA_GET_VALUE( luaManager_, 14, particleEffectData.m_vRotation.z,			0.f );

            LUA_GET_VALUE( luaManager_, 15, particleEffectData.m_vScale.x,			    1.f );
            LUA_GET_VALUE( luaManager_, 16, particleEffectData.m_vScale.y,			    1.f );
            LUA_GET_VALUE( luaManager_, 17, particleEffectData.m_vScale.z,			    1.f );

			particleEffectData.m_bUnitScale = false;
			if(particleEffectData.m_vScale.x >= 999.f)
				particleEffectData.m_bUnitScale = true;

			luaManager_.EndTable();
			index++;
		}
		luaManager_.EndTable();
	}

	//메시플레이어
    nowStateData.m_vecMeshPlayer.resize( 0 );
	if( luaManager_.BeginTable( "MESH_PLAYER" ) == true )
	{
        nowStateData.m_vecMeshPlayer.reserve( luaManager_.GetNumIndexedTables() );
		int index = 1;
		while( luaManager_.BeginTable( index ) == true )
		{
            nowStateData.m_vecMeshPlayer.resize( nowStateData.m_vecMeshPlayer.size() + 1 );
            MeshPlayerEffectData& meshPlayerEffectData = nowStateData.m_vecMeshPlayer.back();
			LUA_GET_VALUE( luaManager_, 1, meshPlayerEffectData.m_bMajor,			false	);
			LUA_GET_VALUE( luaManager_, 2, meshPlayerEffectData.m_fTime,				0.0f	);
			LUA_GET_VALUE( luaManager_, 3, meshPlayerEffectData.m_Name,				L""		);
			LUA_GET_VALUE( luaManager_, 4, meshPlayerEffectData.m_bWeapon,			false	);
			LUA_GET_VALUE( luaManager_, 5, meshPlayerEffectData.m_Pos,				L""		);
			LUA_GET_VALUE( luaManager_, 6, meshPlayerEffectData.m_StateEndDelete,	false	);
			LUA_GET_VALUE( luaManager_, 7, meshPlayerEffectData.m_Trace,				false	);

			luaManager_.EndTable();
			index++;
		}
		luaManager_.EndTable();
	}

	//카메라
    nowStateData.m_vecCameraCrash.resize( 0 );
	if( luaManager_.BeginTable( "CAMERA_CRASH" ) == true )
	{
        nowStateData.m_vecCameraCrash.reserve( luaManager_.GetNumIndexedTables() );
		int index = 1;
		while( luaManager_.BeginTable( index ) == true )
		{
            CameraCrashData cameraCrashData;

			LUA_GET_VALUE( luaManager_, 1, cameraCrashData.m_fTime,			0.0f	);
			LUA_GET_VALUE( luaManager_, 2, cameraCrashData.m_fGap,			0.0f	);
			LUA_GET_VALUE( luaManager_, 3, cameraCrashData.m_CrashTime,		0.0f	);
			LUA_GET_VALUE( luaManager_, 4, cameraCrashData.m_bOnlyFocus,		false	);

            if ( cameraCrashData.m_fTime >= 0.f )
                nowStateData.m_vecCameraCrash.push_back( cameraCrashData );

			luaManager_.EndTable();
			index++;
		}
		luaManager_.EndTable();
        std::stable_sort( nowStateData.m_vecCameraCrash.begin(), nowStateData.m_vecCameraCrash.end() );
	}

    ParsingBuffFactorID( nowStateData, luaManager_ );


}



void CX2GUNPC::InitState()
{

    const InitStateData& initStateData = GetInitStateData();

    m_vecStateRuntime.resize( 0 );
    unsigned uStateSize = initStateData.m_vecStateData.size();
    m_vecStateRuntime.resize( __max( uStateSize, 1 ) );
    for( unsigned u = 0; u < uStateSize; u++ )
    {
        const StateData& nowStateData = initStateData.m_vecStateData[ u ];
        StateRuntime& stateRuntime = m_vecStateRuntime[ u ];
        stateRuntime.m_fStateElapsedTime = nowStateData.m_fInitializeStateElapsedTime;

        if ( nowStateData.m_strLua_StateStart.empty() == false )
            stateRuntime.m_iLua_StateStartRefID = m_LuaManager.MakeFunctionReference( nowStateData.m_strLua_StateStart.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
        if ( nowStateData.m_strLua_FrameMove.empty() == false )
            stateRuntime.m_iLua_FrameMoveRefID = m_LuaManager.MakeFunctionReference( nowStateData.m_strLua_FrameMove.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
        if ( nowStateData.m_strLua_CameraMove.empty() == false )
            stateRuntime.m_iLua_CameraMoveRefID = m_LuaManager.MakeFunctionReference( nowStateData.m_strLua_CameraMove.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
        if ( nowStateData.m_strLua_StateEnd.empty() == false )
            stateRuntime.m_iLua_StateEndRefID = m_LuaManager.MakeFunctionReference( nowStateData.m_strLua_StateEnd.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
        stateRuntime.m_vecNPCEventProcessRuntime.resize( nowStateData.m_vecNPCEventProcessData.size() );
        for( unsigned ui = 0; ui < nowStateData.m_vecNPCEventProcessData.size(); ++ui )
        {
            const NPCEventProcessData& npcEventProcessData = nowStateData.m_vecNPCEventProcessData[ ui ];
            NPCEventProcessRuntime& npcEventProcessRuntime = stateRuntime.m_vecNPCEventProcessRuntime[ ui ];
            npcEventProcessRuntime.m_iLua_CustomConditionRefID = LUA_REFNIL;
            if ( ( npcEventProcessData.m_StateChangeType == SCT_CONDITION_FUNCTION || npcEventProcessData.m_StateChangeType == SCT_CONDITION_TABLE )
                && npcEventProcessData.m_strCustomCondition.empty() == false )
            {
                npcEventProcessRuntime.m_iLua_CustomConditionRefID = m_LuaManager.MakeFunctionReference( npcEventProcessData.m_strCustomCondition.c_str()
                    , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
            }
        }

    }//for
//{{ kimhc // 2010.8.8 // 무조건 NPC에게 카메라가 가도록 하는 기능
    if ( initStateData.m_strCommonCameraMove.empty() == false )
        m_iLua_CommonCameraMoveRefID = m_LuaManager.MakeFunctionReference( initStateData.m_strCommonCameraMove.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
//}} kimhc // 2010.8.8 // 무조건 NPC에게 카메라가 가도록 하는 기능
        // added by wonpok.
        // 모든 스테이트에 대한 공통 FrameMove Function 정의
    if ( initStateData.m_strCommonFrameMove.empty() == false )
        m_iLua_CommonFrameMoveRefID = m_LuaManager.MakeFunctionReference( initStateData.m_strCommonFrameMove.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );
        // 공격 명중시 실행할 Function 정의
    if ( initStateData.m_strAttackedFunc.empty() == false )
        m_iLua_AttackedFuncRefID = m_LuaManager.MakeFunctionReference( initStateData.m_strAttackedFunc.c_str()
                , ( m_pNPCUnitInfo->m_pLuaState != NULL ) ? false : true );

    m_SmallDamageLandFront = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_LAND_FRONT ];
	m_SmallDamageLandBack = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_LAND_BACK ];
    m_SmallDamageAir = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_AIR ];
    m_SmallDamageAirFront = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_AIR_FRONT ];
    m_SmallDamageAirBack = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_AIR_BACK ];
    m_BigDamageLandFront = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_LAND_FRONT ];
    m_BigDamageLandBack = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_LAND_BACK ];
    m_BigDamageAir = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_AIR ];
    m_BigDamageAirFront = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_AIR_FRONT ];
    m_BigDamageAirBack = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_AIR_BACK ];
    m_CommonState.m_DownDamageLandFront = initStateData.m_acStateType[ STATE_TYPE_DOWN_DAMAGE_LAND_FRONT ];
    m_CommonState.m_DownDamageLandBack = initStateData.m_acStateType[ STATE_TYPE_DOWN_DAMAGE_LAND_BACK ];
    m_CommonState.m_DownDamageAir = initStateData.m_acStateType[ STATE_TYPE_DOWN_DAMAGE_AIR ];
    //m_DownDamageAirLanding = initStateData.m_acStateType[ STATE_TYPE_DOWN_DAMAGE_AIR_LANDING ];
    m_UpDamage = initStateData.m_acStateType[ STATE_TYPE_UP_DAMAGE ];
    m_FlyDamageFront = initStateData.m_acStateType[ STATE_TYPE_FLY_DAMAGE_FRONT ];
    m_FlyDamageBack = initStateData.m_acStateType[ STATE_TYPE_FLY_DAMAGE_BACK ];
    //{{ oasis907 : 김상윤 [2010.6.1] // 라이딩 반응 스테이트 정의
    m_RideDamageFront = initStateData.m_acStateType[ STATE_TYPE_RIDE_DAMAGE_FRONT ];
	if(m_RideDamageFront == GUSI_NONE )
		m_RideDamageFront = m_SmallDamageLandFront;
    m_RideDamageBack = initStateData.m_acStateType[ STATE_TYPE_RIDE_DAMAGE_BACK ];
	if(m_RideDamageBack == GUSI_NONE )
		m_RideDamageBack = m_SmallDamageLandBack;
    m_AirFallState = initStateData.m_acStateType[ STATE_TYPE_AIR_FALL ];
    m_RevengeAttack = initStateData.m_acStateType[ STATE_TYPE_REVENGE_ATTACK ];
    m_DamageFlushLandFront = initStateData.m_acStateType[ STATE_TYPE_DAMAGE_FLUSH_LAND_FRONT ];
    m_DamageFlushLandBack = initStateData.m_acStateType[ STATE_TYPE_DAMAGE_FLUSH_LAND_BACK ];
    m_DamageFlushAir = initStateData.m_acStateType[ STATE_TYPE_DAMAGE_FLUSH_AIR ];
    m_DamageRevenge = initStateData.m_acStateType[ STATE_TYPE_DAMAGE_REVENGE ];
    m_DyingLandFront = initStateData.m_acStateType[ STATE_TYPE_DYING_LAND_FRONT ];
    m_DyingLandBack = initStateData.m_acStateType[ STATE_TYPE_DYING_LAND_BACK ];
    m_DyingSky = initStateData.m_acStateType[ STATE_TYPE_DYING_SKY ];
    m_DyingFly = initStateData.m_acStateType[ STATE_TYPE_DYING_FLY ];
    if ( m_DyingFly == GUSI_NONE )
        m_DyingFly = m_DyingSky;
    m_StartState = initStateData.m_acStateType[ STATE_TYPE_START_STATE ];
    m_RebirthState = initStateData.m_acStateType[ STATE_TYPE_REBIRTH_STATE ];
    m_CommonState.m_Wait = initStateData.m_acStateType[ STATE_TYPE_WAIT_STATE ];
    if ( m_CommonState.m_Wait == GUSI_NONE )
        m_CommonState.m_Wait = m_StartState;
    if ( m_DamageRevenge == GUSI_NONE )
        m_DamageRevenge = m_CommonState.m_Wait;
    m_RageState = initStateData.m_acStateType[ STATE_TYPE_RAGE_STATE ];
    if ( m_RageState == GUSI_NONE )
        m_RageState = m_CommonState.m_Wait;
    m_WinState = initStateData.m_acStateType[ STATE_TYPE_WIN_STATE ];
    if ( m_WinState == GUSI_NONE )
        m_WinState = m_CommonState.m_Wait;
    m_LoseState = initStateData.m_acStateType[ STATE_TYPE_LOSE_STATE ];
    if ( m_LoseState == GUSI_NONE )
        m_LoseState = m_CommonState.m_Wait;
    m_RideState = initStateData.m_acStateType[ STATE_TYPE_RIDE_STATE ];
    if ( m_RideState == GUSI_NONE )
        m_RideState = m_CommonState.m_Wait;
    m_SiegeState = initStateData.m_acStateType[ STATE_TYPE_SIEGE_STATE ];
    m_DefenceState = initStateData.m_acStateType[ STATE_TYPE_DEFENCE_STATE ];
    m_DisabledState = initStateData.m_acStateType[ STATE_TYPE_DISABLED_STATE ];
    m_SelfDestructionState = initStateData.m_acStateType[ STATE_TYPE_SELF_DESTRUCTION_STATE ];
    m_SmallDamageLandGuard = initStateData.m_acStateType[ STATE_TYPE_SMALL_DAMAGE_LAND_GUARD ];
    m_BigDamageLandGuard = initStateData.m_acStateType[ STATE_TYPE_BIG_DAMAGE_LAND_GUARD ];
    m_DownDamageLandGuard = initStateData.m_acStateType[ STATE_TYPE_DOWN_DAMAGE_LAND_GUARD ];
    m_ImmuneSkillState = initStateData.m_acStateType[ STATE_TYPE_IMMUNE_SKILL_STATE ];
    m_SummonEndState = initStateData.m_acStateType[ STATE_TYPE_SUMMON_END_STATE ];




//{{ robobeg : 2008-10-28

    // state table 을 state ID 로 참조하기 위해 m_LuaManager 에 reference 로 등록한다.
    // state table 이 global table에 있기 때문에 위의 BeginTable, EndTable 블럭을 빠져나와서 등록함.

    for( std::map<string,char>::const_iterator iter = GetInitStateData().m_mapStateNameID.begin(); iter != GetInitStateData().m_mapStateNameID.end(); iter++ )
    {
        m_LuaManager.MakeTableReference( iter->first.c_str(), iter->second );
    }//
//}} robobeg : 2008-10-28

	// 필드에서 난입한 유저가 방장으로 부터 Npc의 Sync 패킷을 받기 전에 방장이 나가버리는 경우
	// 정의되지 않은 State로 작동하는 것을 방지 하기 위한 코드
	StateChangeForce( GetWaitStateID() );
}

/*static*/
void    CX2GUNPC::InitInit( InitData& OutInit_, KLuaManager& luaManager_ )
{
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	InitSystem( OutInit_, luaManager_ );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
	CX2GameUnit::InitDevice( OutInit_.m_device, luaManager_ );
	InitComponent( OutInit_, luaManager_ );
	InitMotion( OutInit_, luaManager_ );
    InitState( OutInit_, luaManager_ );

}//CX2GUNPC::InitInit()

void    CX2GUNPC::InitData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	m_device.AppendToDeviceList( listInOut_ );
}//CX2GUNPC::InitData::AppendToDeviceList()

void CX2GUNPC::SetHardLevel( int hardLevel, bool bResetHP, float fHPScale /*= 1.f*/ )
{
	m_HardLevel = hardLevel;

	//능력치 세팅
	//if( GetNPCTemplet() != NULL )
	{
		//const CX2UnitManager::NPCUnitStat* pNPCUnitStat = g_pData->GetUnitManager()->GetNPCUnitStat( GetNPCTemplet().m_nNPCUnitID );
        const CX2UnitManager::NPCUnitStat& kNPCUnitStat = GetNPCUnitStat();
		//if( pNPCUnitStat != NULL )
		{
			float fCorrectionStat = 1.f;
			if( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_PVP )
			{
				if( g_pMain->GetPVPChannelClass( g_pMain->GetConnectedChannelID() ) == KPVPChannelInfo::PCC_FREE ||
					g_pMain->GetPVPChannelClass( g_pMain->GetConnectedChannelID() ) == KPVPChannelInfo::PCC_TOURNAMENT )
				{
					// 자유채널, 대회채널에서는 소환몬스터 기본 스탯으로 보정한다.
					fCorrectionStat = 0.f;
				}
				else
				{
					// 그외 채널은 1/10 으로 보정한다.
					fCorrectionStat = 0.1f;
				}
			}

			const float fMultiplier = m_HardLevel * fCorrectionStat;
			const CX2UnitManager::NPCUnitStat::HardLevel& hardLevel = kNPCUnitStat.m_HardLevel;

			SetNPCStat( ( kNPCUnitStat.m_fMaxHP + (hardLevel.m_fHP * fMultiplier) ) * fHPScale,
				kNPCUnitStat.m_fAtkPhysic + (hardLevel.m_fAtkPhysic * fMultiplier),
				kNPCUnitStat.m_fAtkMagic + (hardLevel.m_fAtkMagic * fMultiplier),
				kNPCUnitStat.m_fDefPhysic + (hardLevel.m_fDefPhysic * fMultiplier),
				kNPCUnitStat.m_fDefMagic + (hardLevel.m_fDefMagic * fMultiplier)
				);
		}
	}

	if( bResetHP == true )
	{
		SetNowHp( GetMaxHp() );
		m_fPrevHPGage = GetNowHp();
	}
}

// npc stat을 직접 변경하는 경우
void CX2GUNPC::SetNPCStat( float fHP, float fAtkPhysic, float fAtkMagic, float fDefPhysic, float fDefMagic, bool bUpdateMaxHp /*= true*/ )
{
	if( bUpdateMaxHp == true )
		AccessStat().AccessStat().m_fBaseHP		= fHP;

	AccessStat().AccessStat().m_fAtkPhysic 	= fAtkPhysic;
	AccessStat().AccessStat().m_fAtkMagic	 	= fAtkMagic;
	AccessStat().AccessStat().m_fDefPhysic 	= fDefPhysic;
	AccessStat().AccessStat().m_fDefMagic	 	= fDefMagic;

	if( bUpdateMaxHp == true )
		SetMaxHp( fHP );

	SetNowHp( fHP );
	m_fPrevHPGage = fHP;

	SetGameStat( GetStat().GetStat() );
}

//{{ megagame / 박교현 / 2010.04.30 / NPC 스탯 추가
void CX2GUNPC::GetNPCStat( CX2UnitManager::NPCUnitStat& NpcStat )
{
	//능력치 세팅
	NpcStat.m_fMaxHP			= GetStat().GetStat().m_fBaseHP;
	NpcStat.m_fAtkPhysic		= GetStat().GetStat().m_fAtkPhysic;
	NpcStat.m_fAtkMagic			= GetStat().GetStat().m_fAtkMagic;
	NpcStat.m_fDefPhysic		= GetStat().GetStat().m_fDefPhysic;
	NpcStat.m_fDefMagic			= GetStat().GetStat().m_fDefMagic;

	NpcStat.m_fMaxHP			= GetMaxHp();


	// 명중, 회피 설정
	NpcStat.m_fAccuracy			= GetAccuracyPercent();
	NpcStat.m_fAvoidance		= GetAvoidancePercent();

	NpcStat.m_fPercentCritical	= GetCriticalRate();

	if( m_pAI != NULL &&
		CX2NPCAI::NAT_NORMAL == m_pAI->GetAIType() )
	{
		// 클러킹 스캔 설정
		CX2GUNPCAI *pAI = (CX2GUNPCAI*)m_pAI;
		NpcStat.m_fScanRate			= pAI->GetScanCloaking();
		NpcStat.m_fScanNearRange	= pAI->GetScanNearRange();
	}
}

void CX2GUNPC::SetNPCStat( const CX2UnitManager::NPCUnitStat& NpcStat )
{
	//능력치 세팅
	AccessStat().AccessStat().m_fBaseHP		= NpcStat.m_fMaxHP;
	AccessStat().AccessStat().m_fAtkPhysic	= NpcStat.m_fAtkPhysic;
	AccessStat().AccessStat().m_fAtkMagic		= NpcStat.m_fAtkMagic;
	AccessStat().AccessStat().m_fDefPhysic	= NpcStat.m_fDefPhysic;
	AccessStat().AccessStat().m_fDefMagic		= NpcStat.m_fDefMagic;

	SetMaxHp( NpcStat.m_fMaxHP );

	SetGameStat( GetStat().GetStat() );


	// 명중, 회피, 크리티컬 확률 설정
	m_AdditionalAccuracy.SetBasicStatValue( NpcStat.m_fAccuracy );
	m_AdditionalAvoidance.SetBasicStatValue( NpcStat.m_fAvoidance );
	m_AdditionalCriticalRate.SetBasicStatValue( NpcStat.m_fPercentCritical );

	if( m_pAI != NULL &&
		CX2NPCAI::NAT_NORMAL == m_pAI->GetAIType() )
	{
		// 클러킹 스캔 설정
		CX2GUNPCAI *pAI = (CX2GUNPCAI*)m_pAI;
		pAI->SetScanCloaking( NpcStat.m_fScanRate );
		pAI->SetScanNearRange( NpcStat.m_fScanNearRange );
	}
}
//}} megagame / 박교현 / 2010.04.30 / NPC 스탯 추가

void CX2GUNPC::DoStateEndStart()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();

	//이전 프레임의 스테이트를 종료한다
	bool bTableOpen = false;
    bTableOpen = m_LuaManager.BeginTableByReference( m_NPCFrameData.syncData.nowState );
	{
        m_NPCFrameData.unitCondition.bTransitionChange = NOWSTATE_FLAG( TRANSITION );


		if( IsLocalUnit() == true )
		{
			CommonStateEndHost();
		}

		CommonStateEnd();
	}
	if( bTableOpen == true )
		m_LuaManager.EndTable();

	//스테이트를 바꾼다


    m_kNonHostReaction.DoStateEndStart( IsLocalUnit(), m_NPCFrameData, m_PhysicParam );


	m_NPCFrameData.syncData.nowState		= m_NPCFrameData.unitCondition.nextState;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_NPCFrameData.syncData.stateChangeParts.stateChangeNum	= m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum;
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_NPCFrameData.syncData.bStateChange	= false;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//{{ robobeg : 2008-10-28
	//m_StateTableName	= m_NPCStateData.stateTable;
//}} robobeg : 2008-10-28

    m_bClientHoldPosition = false;

	//다음 스테이트를 시작한다
    bTableOpen = m_LuaManager.BeginTableByReference( m_NPCFrameData.syncData.nowState );
	{

		if( IsLocalUnit() == true )
		{
			CommonStateStartHost();
		}

		CommonStateStart();
	}
	if( bTableOpen == true )
		m_LuaManager.EndTable();
}

void CX2GUNPC::DoFrameMove( bool bNoFrameMove )
{
	KTDXPROFILE();

	bool bTableOpen = false;
    bool    bLocalUnit = IsLocalUnit();

    m_cMindFlagBackup = m_NPCFrameData.syncData.mindFlag;


//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


    m_kNonHostReaction.FixStatesForLocalUnitOrReaction_NONE( bLocalUnit );


    //if ( GetNowStateID() == GUSI_DIE )
    //    goto out_state_change;

    if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
    {
		DoStateEndStart();


        m_kNonHostReaction.FixStatesForLocalUnitOrReaction_NONE( bLocalUnit );
        m_kNonHostReaction.UpdateNowStateStartSpeed( m_NPCFrameData, m_PhysicParam );


    }//if

    if ( GetNowStateID() == GUSI_DIE )
        goto out_state_change;

	if( ( m_pidHostUID == 0 || m_pidHostUID != g_pX2Game->GetHostSlotUID() )
        || bLocalUnit == true )
    {
		//Host가 변경된다면 지금까지 받은패킷 모두 정리
		m_cbufReceiveDynamicPacket.resize( 0 );
		m_dwLastProcessedFrameMoveCount = 0;
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		m_kLastStateChangeParts_Recv.Init();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

        m_pidHostUID = 0;
        m_kNonHostReaction.ClearBuffNonHostSimState();
    }//if

    bool    bUpdatePositionBackup = false;


    if ( bLocalUnit == false && m_kNonHostReaction.IsNowStateReaction_FIRST() == true )
    {
        float   fAnimSpeedAdjustFactor = m_fBornAnimSpeedRate * GetAnimSpeedValue();
        float   fOldHP = GetNowHp();
        float   fOldMP = GetNowMp();
        float   fNewHP = fOldHP, fNewMP = fOldMP;
        m_kNonHostReaction.DoFrameMove_SimulateReaction_FIRST(
            m_NPCFrameData, m_PhysicParam, m_cbufReceiveDynamicPacket, m_dwLastProcessedFrameMoveCount, bUpdatePositionBackup, fNewHP, fNewMP,
            m_vecSuperArmorPtr.empty() == false, fAnimSpeedAdjustFactor, GetUnitHeight( true ), GetInitStateData().m_vecStateData );
        if ( fNewHP != fOldHP )
            SetNowHp( fNewHP );
        if ( fNewMP != fOldMP )
            SetNowMp( fNewMP );
    }

    if ( bNoFrameMove == true )
    {
        goto out_exit;
    }//if


	for( ; m_cbufReceiveDynamicPacket.empty() == false; m_cbufReceiveDynamicPacket.pop_front() )
	{
        const NPCFrameSync& frameSync = m_cbufReceiveDynamicPacket.front();
        const KDYNAMIC_UNIT_NPC_SYNC& xPT_UNIT_NPC_SYNC = frameSync.m_sync;
        m_dwLastProcessedFrameMoveCount = frameSync.m_dwFrameMoveCount;


        m_kNonHostReaction.FixStatesForReaction_NOT_FIRST( bLocalUnit );

        if ( frameSync.m_bMiniSync == true )
        {
            m_kNonHostReaction.UpdateRelativePosAndTimers( xPT_UNIT_NPC_SYNC.m_dwRelativePos );
            m_kNonHostReaction.SetTargetUnitIndex( xPT_UNIT_NPC_SYNC.m_cTargetUnitIndex );
            continue;
        }//if

		SetReceiveP2P( true );

        m_bClientHoldPosition = false;

        bool    bNotUpdateDefenceRandomTableIndex = false;
        {
            bool bStateChange = false;

            bool    bForceModifyConfig = false;

            if ( bLocalUnit == false && m_kNonHostReaction.IsNowStateReaction_NONE() == false )
            {
                bool bSkipStateChangeOrUpdate = m_kNonHostReaction.DoFrameMove_SimulateReaction(
                    m_NPCFrameData, m_PhysicParam, bUpdatePositionBackup, bStateChange, bForceModifyConfig, bNotUpdateDefenceRandomTableIndex,
                    xPT_UNIT_NPC_SYNC, GetInitStateData().m_vecStateData );
                if ( bSkipStateChangeOrUpdate == true )
                {
                    goto out;
                }
            }
            else
            {
                if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum
                    || xPT_UNIT_NPC_SYNC.m_cNowState != m_NPCFrameData.syncData.nowState )
                {
                    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum;
                    m_NPCFrameData.unitCondition.nextState = xPT_UNIT_NPC_SYNC.m_cNowState;
                    m_kNonHostReaction.DoFrameMove_StateChangeByHost( bUpdatePositionBackup, xPT_UNIT_NPC_SYNC );
                    bStateChange = true;
                }
            }

		    //이번 프레임에 스테이트 변경이 예약되어 있으면
		    if( bStateChange == true )
		    {
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//if ( bNowStateReaction == true )
//{
//    printf( "NPC State Change By Server %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//        (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
//}//if
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
			    DoStateEndStart();

                {
                    bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
                    m_NPCFrameData.syncData.SetData( xPT_UNIT_NPC_SYNC
                        , _IsDyingState( m_NPCFrameData.syncData.nowState )
                        );
                    bool bNewIsRight = m_NPCFrameData.syncData.bIsRight;
                    m_NPCFrameData.syncData.bIsRight = bOldIsRight;
                    SetIsRightNow( bNewIsRight );
		            m_PhysicParam.nowSpeed.x					= halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedX );
		            m_PhysicParam.nowSpeed.y					= halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedY );

                    m_NPCFrameData.unitCondition.bFootOnLine = xPT_UNIT_NPC_SYNC.ExtractFootOnLine();
                    m_kNonHostReaction.ReserveReceivePos( m_NPCFrameData.syncData.position
                        , m_NPCFrameData.syncData.lastTouchLineIndex
                        , m_NPCFrameData.unitCondition.bFootOnLine );
                }
		    }
            else
            {
                bool bIsRight = xPT_UNIT_NPC_SYNC.ExtractIsRight();
                float fNewSpeedX = halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedX );
                if (
                     bForceModifyConfig == true
                    || ( ( m_kNonHostReaction.IsNowStateReaction_NONE() == false )
                           ? m_kNonHostReaction.CheckCompatibleReactionState( m_NPCFrameData.syncData.nowState, xPT_UNIT_NPC_SYNC.m_cNowState,
                                GetInitStateData().m_vecStateData ) == false
                           : ( m_NPCFrameData.syncData.stateChangeParts.stateModifyNum != xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateModifyNum
                               || m_NPCFrameData.syncData.bIsRight != bIsRight ) )
                  )
                {
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//if ( bNowStateReaction == true )
//{
//    printf( "NPC State Update By Server %S(%d)\n", GetStateNameByStateID( xPT_UNIT_NPC_SYNC.m_cNowState ).c_str(),
//        (int) xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateChangeNum );
//}//if
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION

                    bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
                    m_NPCFrameData.syncData.SetData( xPT_UNIT_NPC_SYNC
                        , _IsDyingState( m_NPCFrameData.syncData.nowState )
                        );
                    bool bNewIsRight = m_NPCFrameData.syncData.bIsRight;
                    m_NPCFrameData.syncData.bIsRight = bOldIsRight;
                    SetIsRightNow( bNewIsRight );
		            m_PhysicParam.nowSpeed.x					= fNewSpeedX;
		            m_PhysicParam.nowSpeed.y					= halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowSpeedY );

                    m_NPCFrameData.unitCondition.bFootOnLine = xPT_UNIT_NPC_SYNC.ExtractFootOnLine();
                    m_kNonHostReaction.ReserveReceivePos( m_NPCFrameData.syncData.position
                        , m_NPCFrameData.syncData.lastTouchLineIndex
                        , m_NPCFrameData.unitCondition.bFootOnLine );
                }
                else
                {
                    D3DXVECTOR3 vReservedReceivePos;
                    NPCSyncData::_DecodeNpcSyncPos( vReservedReceivePos, xPT_UNIT_NPC_SYNC.m_usPosX, xPT_UNIT_NPC_SYNC.m_usPosY, xPT_UNIT_NPC_SYNC.m_usPosZ, xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex );
                    m_kNonHostReaction.ReserveReceivePos( vReservedReceivePos
                        , xPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex
                        , xPT_UNIT_NPC_SYNC.ExtractFootOnLine() );
                }
            }
            m_kNonHostReaction.UpdateNowStateStartSpeed( m_NPCFrameData, m_PhysicParam );
        }
out:

        m_NPCFrameData.syncData.stateChangeParts.stateModifyNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateModifyNum;
        m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum = xPT_UNIT_NPC_SYNC.m_stateChangeParts.stateModifyNum;
        m_NPCFrameData.syncData.m_usRandomTableIndex = xPT_UNIT_NPC_SYNC.m_usRandomTableIndex;
        SetNowHp( xPT_UNIT_NPC_SYNC.m_fNowHP );
        SetNowMp( halfToFloat( xPT_UNIT_NPC_SYNC.m_usNowMP ) );
        m_NPCFrameData.syncData.mindFlag = (char)xPT_UNIT_NPC_SYNC.ExtractMindFlag();
        m_kNonHostReaction.SetTargetUnitIndex( xPT_UNIT_NPC_SYNC.m_cTargetUnitIndex );
        m_kNonHostReaction.UpdateRelativePosAndTimers( xPT_UNIT_NPC_SYNC.m_dwRelativePos );
        if ( bNotUpdateDefenceRandomTableIndex == false )
        {
            m_kNonHostReaction.SetDefenceRandomTableIndex( xPT_UNIT_NPC_SYNC.m_usDefenceRandomTableIndex );
        }

        if ( bUpdatePositionBackup == true )
        {
            BackupPosition();
            bUpdatePositionBackup = false;
            m_kNonHostReaction.ResetLocalRelativePos();
        }//if
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		if( m_kLastStateChangeParts_Recv.IsValid() )
		{
            unsigned uSent = 0;
            unsigned uReceived = 0;
            unsigned uDiffStateModify = ( m_NPCFrameData.syncData.stateChangeParts.stateModifyNum - m_kLastStateChangeParts_Recv.GetValue().stateModifyNum ) & 0xf;
			if( uDiffStateModify != 0 )
            {
                uSent = uDiffStateModify;
                uReceived = 1;
            }//if

			//NPC SYNC Packet 로스율 측정
            if ( uSent != 0 )
			    KOGGamePerformanceCheck::GetInstance()->GetUDPTransCheckMgr()->UpdateNpcSyncPacketLossRate( uSent, uReceived );
		}

		//
		m_kLastStateChangeParts_Recv.SetValue( m_NPCFrameData.syncData.stateChangeParts );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	}
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	while( (int)m_ReceivePacketBufList.size() > 0 )
//	{
//#ifdef	BATTLE_FIELD_TEST
//		SetReceiveP2P( true );
//#endif	BATTLE_FIELD_TEST
//
//		KXPT_UNIT_NPC_SYNC xPT_UNIT_NPC_SYNC = m_ReceivePacketBufList[0];
//		m_ReceivePacketBufList.erase( m_ReceivePacketBufList.begin() );
//
//		m_NPCFrameData.unitCondition.nextState		= xPT_UNIT_NPC_SYNC.nextState;
//
//#ifdef NEW_RANDOM_TABLE_TEST
//		m_NPCFrameData.syncData.m_usRandomTableIndex	= xPT_UNIT_NPC_SYNC.m_usRandomTableIndex;
//#else NEW_RANDOM_TABLE_TEST
////{{AFX
//		m_NPCFrameData.syncData.m_StateRandomSeed	= xPT_UNIT_NPC_SYNC.m_StateRandomSeed;
//		m_NPCFrameData.syncData.m_StateRandomSeed2	= xPT_UNIT_NPC_SYNC.m_StateRandomSeed2;
////}}AFX
//#endif NEW_RANDOM_TABLE_TEST
//		m_PhysicParam.nowSpeed.x					= xPT_UNIT_NPC_SYNC.nowSpeedX;
//		m_PhysicParam.nowSpeed.y					= xPT_UNIT_NPC_SYNC.nowSpeedY;
//		SetNowHp( xPT_UNIT_NPC_SYNC.fNowHP );
//		SetNowMp( halfToFloat(xPT_UNIT_NPC_SYNC.fNowMP) );
//
//		if( m_NPCFrameData.syncData.nowState != xPT_UNIT_NPC_SYNC.nowState )
//		{
//			//xPT_UNIT_NPC_SYNC.bStateChange = true;
//
//			char tempData = 1;
//			xPT_UNIT_NPC_SYNC.mindFlag	|= (tempData << 6);
//		}
//
//		m_NPCFrameData.syncData.SetData( xPT_UNIT_NPC_SYNC );
//
//		//이번 프레임에 스테이트 변경이 예약되어 있으면
//		if( m_NPCFrameData.syncData.bStateChange == true )
//		{
//			DoStateEndStart();
//		}
//	}
//
//	/// kimhc // 이건 방장만 돌아도 될것 같은데...
//	//이번 프레임에 스테이트 변경이 예약되어 있으면
//	if( m_NPCFrameData.syncData.bStateChange == true )
//	{
//		DoStateEndStart();
//	}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
out_state_change:

    if ( bNoFrameMove == true )
    {
        goto out_exit;
    }

//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    {

        bTableOpen = m_LuaManager.BeginTableByReference( m_NPCFrameData.syncData.nowState );
	    {

            if( true == IsLocalUnit() )
		    {
			    CommonFrameMoveHost();
		    }

		    CommonFrameMove();

		    if( true == m_bEnableEventProcess )
		    {
			    if( IsLocalUnit() == true )
			    {
				    CommonEventProcess();
			    }
                else
                {
                    CommonEventProcessNonHost();
                }
		    }
	    }
	    if( bTableOpen == true )
		    m_LuaManager.EndTable();

    }




out_exit:

    m_kNonHostReaction.UpdateNowStateStartSpeed( m_NPCFrameData, m_PhysicParam );
    m_kNonHostReaction.BuffPositionHistory( IsLocalUnit(),  m_NPCFrameData.syncData.position );


    m_iFrameAccumCounter = 0;
}

HRESULT CX2GUNPC::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	//필드에서 비호스트가 최초 NPC SYNC 패밋을 못 받았으면 바로 리턴한다.
	if ( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && DidReceiveFirstSyncPacket() == false )
		return S_OK;
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK





    bool    bLocalUnit = IsLocalUnit();

    {
        if ( bLocalUnit == true )
        {
            m_kNonHostReaction.ResetRelativePosAndTimers();
        }
        else
        {
            m_kNonHostReaction.IncrementLocalAndRelativePosTimers( g_pKTDXApp->GetElapsedTime() );
        }//if.. else..
    }


    m_fOneShotElapsedTime = fElapsedTime;

    if ( m_pInitData != NULL && m_pInitData->m_bHeuristicSimulation == false )
    {
        m_iFrameMoveIntervalCounter = 1;
        m_iFrameAccumCounter = 1;
    }
    else
    {
        if ( m_iFrameMoveIntervalCounter >= g_pMain->GetFrameBufferNum() )
        {
            m_iFrameMoveIntervalCounter = 1;
            m_iFrameAccumCounter = 1;
            fElapsedTime = m_fOneShotElapsedTime * m_iFrameAccumCounter;
        }
        else
        {
            if ( m_iFrameMoveIntervalCounter == 0 )
            {
                m_iFrameMoveIntervalCounter = 1;
                m_iFrameAccumCounter = 1;
            }
            else
            {
                m_iFrameAccumCounter++;
	            fElapsedTime	= m_fOneShotElapsedTime * m_iFrameAccumCounter;
                m_iFrameMoveIntervalCounter++;

            }//if.. else..

	        AdjustFrameSkipRate( fElapsedTime );

            if ( m_iFrameMoveIntervalCounter >= g_pMain->GetFrameBufferNum() )
            {
                m_iFrameMoveIntervalCounter = 0;
            }
            else
            {
                return E_FAIL;
            }//if.. else..
        }
    }


    {
        m_kNonHostReaction.ResetReservedReceivePos();
        if ( bLocalUnit == true )
        {
            BackupPosition();
            m_kNonHostReaction.ResetSimulationTolerance();
        }
        else
        {
            m_kNonHostReaction.UpdateSimulationTolerance();
        }//if.. else..
    }


// 	if ( NULL == m_pXSkinAnim )
// 	{
// 		if ( CKTDXDevice::DEVICE_STATE_LOADED == m_pMotion->GetDeviceState() )
// 			CreateAndSetAinmXSkinMesh();
// 		else
// 			return S_OK;
// 	}






	if( GetNPCAI() != NULL &&
		GetNPCAI()->GetAIType() == CX2NPCAI::NAT_NORMAL &&
		m_bDamageHistory == true &&
		g_pX2Game != NULL && bLocalUnit == true &&
		g_pInstanceData != NULL &&
		g_pInstanceData->GetVerifyNpcHp() == false &&
		VerifyNowHp() == false
		)
	{
		g_pInstanceData->SetVerifyNpcHp(true);

		if( g_pData != NULL && g_pData->GetServerProtocol() != NULL &&
			g_pKTDXApp->GetFindHacking() == false )
		{
			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

			if( g_pData != NULL && g_pData->GetMyUser() != NULL )
				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
		}

		g_pMain->SendHackMail_DamageHistory( ANTI_HACK_STRING_AntiHacking_VERIFY_NPC_HP );
		g_pKTDXApp->SetFindHacking( true );
	}




	CX2GameUnit::OnFrameMove( fTime, fElapsedTime );

	bool bTableOpen = false;

	//m_pComboManager->OnFrameMove(fTime, fElapsedTime);

// 	float fNowHPGage = GetNowHp();
// 	if( m_fPrevHPGage != fNowHPGage &&
// 		true == GetGageManager()->GetHasBossGage() )
// 	{
// 		m_fPrevHPGage = GetNowHp();
//
// 		for( int i=0; i<g_pX2Game->GetNPCUnitListSize(); i++ )
// 		{
// 			CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnit( i );
// 			if( NULL == pGUNPC )
// 				continue;
//
// 			if( true == pGUNPC->GetGageManager()->GetHasBossGage() )
// 			{
// 				pGUNPC->GetGageManager()->SetRemainBossGageTime( 0.f );
// 			}
// 		}
//
// 		const float MAGIC_BOSS_GAGE_TIME = 2.f;
// 		GetGageManager()->SetRemainBossGageTime( MAGIC_BOSS_GAGE_TIME );
// 	}

	if( GetNowHp() < 0.f )
		m_fDieAfterTime += fElapsedTime;

	// Unit Fade
	if(m_bFadeScale == true)
	{
		if(m_fFadeScaleEndTime > 0.f)
		{
			m_fFadeScaleEndTime -= fElapsedTime;
			m_vFinalScale += m_vFadeScale * ( (float) ( m_iFrameAccumCounter ) );

			if(m_vFinalScale.x <= 0.7f)
				m_vFinalScale.x = 0.7f;
			if(m_vFinalScale.y >= 2.0f)
				m_vFinalScale.y = 2.0f;
			if(m_vFinalScale.z <= 0.7f)
				m_vFinalScale.z = 0.7f;
		}
		else
		{
			m_bFadeScale = false;
			m_fFadeScaleEndTime = 0.f;
		}

		if ( NULL != m_pXSkinAnim )
			m_pXSkinAnim->GetMatrix().Scale(m_vFinalScale);
	}

	if(m_bUnitFade == true)
	{
		if(m_fFadeEndTime > 0.f)
		{
			m_fFadeEndTime -= fElapsedTime;
            m_RenderParam.color += m_cFadeColor * ( (float)  m_iFrameAccumCounter );
			if(m_RenderParam.bAlphaBlend == true)
			{
				SetAlphaObject(true);
			}
		}
		else
		{
			m_bUnitFade = false;
			m_fFadeEndTime = 0.f;
			//m_cFadeColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

			m_bUnitColor = false;
			m_RenderParam.bAlphaBlend = false;
			m_RenderParam.color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

			m_RenderParam.renderType = CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;

			SetAlphaObject(false);
		}
	}

	//프레임을 돌린다
	if( m_NPCFrameData.unitCondition.fStopTime <= 0.0f &&
		m_NPCFrameData.unitCondition.fStop2Time <= 0.0f )
	{
        D3DXVECTOR3 vPosBackup = m_NPCFrameData.syncData.position;

		DoFrameMove();

		ShowGateGuideFrameMove( fTime, fElapsedTime );

		//{{ kimhc // 2010.7.7 // 몬스터가 생성한 라인맵
		// 몬스터에 의해 생성된 라인맵이 있으면 몬스터 이동시 라인맵도 같이 움직이도록 구현
		MoveMonsterLineMap( vPosBackup );
		//}} kimhc // 2010.7.7 // 몬스터가 생성한 라인맵
	}
	else
	{
		m_NPCFrameData.unitCondition.fStopTime -= fElapsedTime;
		if( m_NPCFrameData.unitCondition.fStopTime < 0.0f )
			m_NPCFrameData.unitCondition.fStopTime = 0.0f;
		m_NPCFrameData.unitCondition.fStop2Time -= fElapsedTime;
		if( m_NPCFrameData.unitCondition.fStop2Time < 0.0f )
			m_NPCFrameData.unitCondition.fStop2Time = 0.0f;

        DoFrameMove( true );
	}

	// weapon
	for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	{
		Weapon* pWeapon = m_vecpWeapon[i];

		pWeapon->OnFrameMove( fTime, fElapsedTime );
	}

    unsigned uSize = GetInitStateData().m_vecStateData.size();
    ASSERT( uSize == m_vecStateRuntime.size() );
    for( unsigned u = 0; u != uSize; u++ )
    {
		const StateData& stateData = GetInitStateData().m_vecStateData[ u ];
        StateRuntime& stateRuntime = m_vecStateRuntime[ u ];
		if ( stateData.m_fStateCoolTime > 0 )
			stateRuntime.m_fStateElapsedTime += fElapsedTime;
    }

//{{ robobeg : 2008-10-28
	//map<wstring,NPCStateData>::iterator iter2;
	//for ( iter2 = m_StateListByName.begin(); iter2 != m_StateListByName.end(); iter2++ )
	//{
	//	NPCStateData& npcStateData = (iter2->second);

	//	if ( npcStateData.fStateCoolTime > 0 )
	//		npcStateData.fStateElapsedTime += fElapsedTime;
	//}
//}} robobeg : 2008-10-28

	if ( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->GetMatrix().Scale( m_vTransScale );
	}


	// 더블어택 처리
	m_fDoubleAttackTime += fElapsedTime;
	if(m_fDoubleAttackTime >= 0.2f
        && m_eDoubleAttack != DOUBLE_ATTACK_NONE
        )
	{
		DamageDoubleAttack();
	}

#if X2_DEV_TOOLS
	if( m_pXSkinAnim != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV && g_pInstanceData->GetFrameScale() == true)
	{
        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pFrame_Bip01_Head_Real;
		if(pFrame != NULL && pFrame->m_bScale == false)
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
			SetFrameScale(&vScale, true, pFrame);
		}
	}
	else if( m_pXSkinAnim != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV && g_pInstanceData->GetFrameScale() == false)
	{
        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pFrame_Bip01_Head_Real;
		if(pFrame != NULL && pFrame->m_bScale == true)
		{
			SetFrameScaleRestore(pFrame);
		}
	}
#endif

	// 강제다운수치 관련 수정
	if( m_fMaxDownForce > -9999.f
		&& false == IsSuperArmor() )
	{
		m_fLastDamagePassTime = 0.0f;
		AddForceDownGage( m_fMaxDownForce );

		if ( GetUID() == 2 )
		{
			int iForceDown = static_cast<int>( GetGageData()->GetNowForceDown() );

			WCHAR buf[100] = { 0, };

			wsprintf( buf, L"NowForceDown: %d\n", iForceDown );

			DXUTTRACE( buf );
		}

	}
	m_fMaxDownForce	= -9999.f;

	return S_OK;
}

HRESULT	CX2GUNPC::OnCameraMove()
{
	KTDXPROFILE();

    if ( true == m_LuaManager.BeginTableByReference( m_NPCFrameData.syncData.nowState ) )
	{
		//{{ robobeg : 2008-10-28
		//if( m_NPCStateData.CameraMove != NULL )
		//	m_NPCStateData.CameraMove();
		//else
		//}} robobeg : 2008-10-28
		CommonCameraMove();

		m_LuaManager.EndTable();
	}

	return S_OK;
}

void    CX2GUNPC::OnFrameRender_Draw()
{
	KTDXPROFILE();

	// 캐시샵이 열려있으면 처리 하지 않도록 설정
	if( NULL != g_pData && NULL != g_pData->GetCashShop() && true == g_pData->GetCashShop()->GetOpen() )
		return;

	if ( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && false == DidReceiveFirstSyncPacket() )
		return;
    if ( true == m_LuaManager.BeginTableByReference( m_NPCFrameData.syncData.nowState ) )
	{
		//{{ robobeg : 2008-10-28
		//if( m_NPCStateData.Render != NULL )
		//	m_NPCStateData.Render();
		//else
		//}} robobeg : 2008-10-28
		CommonRender_Draw();

		m_LuaManager.EndTable();
	}

}

void CX2GUNPC::StateChangeCheat( int state )
{
    if ( false == ( state >= 0 && state < (int) m_vecStateRuntime.size() ) )
        return;
    ASSERT( m_vecStateRuntime.size() == GetInitStateData().m_vecStateData.size() );
    const StateData& stateData = GetInitStateData().m_vecStateData[ state ];
    StateRuntime& stateRuntime = m_vecStateRuntime[ state ];
    if ( stateRuntime.m_fStateElapsedTime >= stateData.m_fStateCoolTime )
    {
        stateRuntime.m_fStateElapsedTime = 0.0f;
    }

	m_NPCFrameData.unitCondition.nextState		= state;
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum = m_NPCFrameData.syncData.stateChangeParts.stateChangeNum;
    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum++;
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_NPCFrameData.syncData.bStateChange		= true;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_kNonHostReaction.SetNextStateReaction_NONE();

}

bool CX2GUNPC::StateChange_LUA( const char* stateName, bool bDirectChange )
{
	if( NULL == stateName )
		return false;

	//wstring wstrStateName = L"";
	//ConvertUtf8ToWCHAR( wstrStateName, stateName );
//{{ robobeg : 2008-10-28
	//StateChange( wstrStateName.c_str(), bDirectChange );
    return StateChange( GetStateID( stateName ), bDirectChange );
//}} robobeg : 2008-10-28
}





bool    CX2GUNPC::StateChangeForce( int state, bool bDirectChange )
{
    if ( false == ( state >= 0 && state < (int) m_vecStateRuntime.size() ) )
        return false;
    ASSERT( m_vecStateRuntime.size() == GetInitStateData().m_vecStateData.size() );
    const StateData& stateData = GetInitStateData().m_vecStateData[ state ];
    StateRuntime& stateRuntime = m_vecStateRuntime[ state ];
    if ( stateRuntime.m_fStateElapsedTime >= stateData.m_fStateCoolTime )
    {
        stateRuntime.m_fStateElapsedTime = 0.0f;
    }
    else
    {
        return false;
    }

	m_NPCFrameData.unitCondition.nextState		= state;
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum	= m_NPCFrameData.syncData.stateChangeParts.stateChangeNum;
    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum++;
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_NPCFrameData.syncData.bStateChange		= true;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_kNonHostReaction.SetNextStateReaction_NONE();

    return true;

}


void    CX2GUNPC::StateChangeToDieForce()
{
    if ( GetNowStateID() != GUSI_DIE )
    {
	    m_NPCFrameData.unitCondition.nextState		= GUSI_DIE;
        m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum	= m_NPCFrameData.syncData.stateChangeParts.stateChangeNum;
        m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum++;
        m_kNonHostReaction.SetNextStateReaction_NONE();

    }//if
}



int CX2GUNPC::StateChange_CheckOnly( int state, bool bDirectChange )
{
	if( state <= 0 )
		return 0;

    if ( false == ( state >= 0 && state < (int) m_vecStateRuntime.size() ) )
        return 0;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( GetNowStateID() == GUSI_DIE )
        return 0;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	if( GetNowHp() > 0.f ) // 죽은 몬스터의 경우에 석화나 얼음이 동작하지 않기 때문에 살아있는 몬스터에 대해서만 처리
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			return 0;
		}

		if( HaveThisBuff(BTI_DEBUFF_FROZEN) )
			return 0;
	}

    return state;
}



bool CX2GUNPC::StateChange( int state, bool bDirectChange )
{
	KTDXPROFILE();
	if( IsLocalUnit() == false )
		return false;

	if( state <= 0 )
		return false;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( GetNowStateID() == GUSI_DIE )
        return false;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	if( GetNowHp() > 0.f ) // 죽은 몬스터의 경우에 석화나 얼음이 동작하지 않기 때문에 살아있는 몬스터에 대해서만 처리
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			return false;
		}

		if( HaveThisBuff(BTI_DEBUFF_FROZEN) )
			return false;
	}

	bool bStateChanged = StateChangeForce( state, bDirectChange );

	//호스트는 샌드패킷
//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//    if ( bStateChanged == true )
//        SendReserve();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

    return  bStateChanged;
}

void CX2GUNPC::UnDoStateChange_Reaction()
{
    if ( m_NPCFrameData.unitCondition.nextState == GUSI_DIE
        || m_kNonHostReaction.IsNextStateRection_NONE() == true )
        return;
	m_NPCFrameData.unitCondition.nextState		= m_NPCFrameData.syncData.nowState;
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum	= m_NPCFrameData.syncData.stateChangeParts.stateChangeNum;
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_NPCFrameData.syncData.bStateChange		= false;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_kNonHostReaction.SetNextStateReaction_NONE();
}


bool CX2GUNPC::StateChange_Reaction_FIRST( int state, int iAttackerUnitIndex, const D3DXVECTOR3& vImpact, bool bRelaxReactionCheck )
{
	KTDXPROFILE();

	if( state <= 0 )
		return false;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( GetNowStateID() == GUSI_DIE )
        return false;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


	if( GetNowHp() > 0.f ) // 죽은 몬스터의 경우에 석화나 얼음이 동작하지 않기 때문에 살아있는 몬스터에 대해서만 처리
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			return false;
		}

		if( HaveThisBuff(BTI_DEBUFF_FROZEN) )
			return false;
	}

	if ( StateChangeForce( state, true ) == true )
    {
        float fY = m_NPCFrameData.syncData.position.y;
        float fHeight = GetUnitHeight( false );
        D3DXVECTOR2 vReactionYMinMax;
        vReactionYMinMax.x = __min( fY, vImpact.y );
        vReactionYMinMax.y = __max( fY + fHeight, vImpact.y );
        m_kNonHostReaction.SetNextStateReaction_FIRST(
            bRelaxReactionCheck,
            iAttackerUnitIndex,
            vReactionYMinMax );

//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	    //호스트는 샌드패킷
//	    if( IsLocalUnit() == true )
//	    {
//		    SendReserve();
//	    }
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        return true;
    }
    return false;
}

bool CX2GUNPC::StateChange_Reaction_Continue( int state, bool bWait )
{
	KTDXPROFILE();


	if( state <= 0 )
		return false;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( GetNowStateID() == GUSI_DIE )
        return false;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


	if( GetNowHp() > 0.f ) // 죽은 몬스터의 경우에 석화나 얼음이 동작하지 않기 때문에 살아있는 몬스터에 대해서만 처리
	{
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
			m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			return false;
		}

		if( HaveThisBuff(BTI_DEBUFF_FROZEN) )
			return false;
	}

	if ( StateChangeForce( state, true ) == true )
    {
        m_kNonHostReaction.SetNextStateReaction_CONTINUE( bWait );

//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	    //호스트는 샌드패킷
//	    if( IsLocalUnit() == true )
//	    {
//		    SendReserve();
//	    }
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

        return true;
    }

    return false;
}



//{{ robobeg : 2008-10-28
bool CX2GUNPC::StateChange( const char* state, bool bDirectChange )
{
	KTDXPROFILE();
    return StateChange( GetStateID( state ), bDirectChange );
}

/*virtual*/ int CX2GUNPC::GetNowStateID() const
{
    return  m_NPCFrameData.syncData.nowState;
}

void CX2GUNPC::DamageReact( CX2DamageManager::DamageData* pDamageData )
{

#if !X2_DEV_TOOLS
#endif
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();


	m_NPCFrameData.syncData.m_usRandomTableIndex	= ( m_NPCFrameData.syncData.m_usRandomTableIndex + 1234 ) % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT;

	m_bHitedDrainMana = false;
    m_eDoubleAttack = DOUBLE_ATTACK_NONE;
	m_NPCFrameData.unitCondition.bAttackerFront = true;

	if( pDamageData->bForceFly == true )
	{
		m_FlyCount = 0;
	}
	else
	{
		switch( pDamageData->reActResult )
		{
		case CX2DamageManager::RT_UP:
		case CX2DamageManager::RT_FLY:

//{{ kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격
		case CX2DamageManager::RT_DRAG_UP:
//}} kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격
			{
				m_FlyCount++;
			} break;
		}
	}



	CX2GameUnit*	pAttackerUnit = NULL;
	D3DXVECTOR3		dirUnitToAttacker = D3DXVECTOR3(0,0,0);

	if( null != pDamageData->optrAttackerGameUnit )
	{
		pAttackerUnit		= pDamageData->optrAttackerGameUnit.GetObservable();
	}

    CX2DamageEffect::CEffect*   pAttackerEffect = g_pX2Game->GetDamageEffect()->GetInstance( pDamageData->hAttackerEffect );

    bool        bLocalUnit = IsLocalUnit();
    bool        bAttackedByMe = false;
    UidType     uidAttackerUID = 0;
    int         iAttackerUnitIndex = -1;
    bool        bLagConsideration = false;
    float       fLagLatency = 0.f;

	if( pAttackerUnit != NULL )
	{

        if ( pAttackerUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
        {
            uidAttackerUID = pAttackerUnit->GetUnitUID();
            iAttackerUnitIndex = pAttackerUnit->GetUnitIndex();
            bAttackedByMe = pAttackerUnit->IsMyUnit();
        }
		switch(pDamageData->attackerType)
		{
		case CX2DamageManager::AT_UNIT:
			{
				dirUnitToAttacker	= pAttackerUnit->GetPos() - GetPos();
                if ( bLocalUnit == true
                    && uidAttackerUID != 0 && bAttackedByMe == false
                    && m_kNonHostReaction.IsNowStateReaction_NONE() == true )
                {
                    bLagConsideration = true;
                    const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( pAttackerUnit->GetUnitIndex() );
                    if ( pSlotData != NULL )
                    {
                        fLagLatency = pSlotData->m_fAvgPingTime;
                        fLagLatency += pSlotData->m_fAvgPingStd;
                    }//if
                }//if
			} break;
		case CX2DamageManager::AT_MESH:
			{
				dirUnitToAttacker	= pDamageData->pAttackerAnim->GetMatrix().GetPos() - GetPos();
			} break;
		case CX2DamageManager::AT_MANUAL_COLLISION_DATA:
			{
				dirUnitToAttacker	= pDamageData->pCollisionData->GetPointStart() - GetPos();
			} break;
		case CX2DamageManager::AT_EFFECT:
			{
                if ( pAttackerEffect != NULL )
                {
                    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pAttackerEffect->GetMainEffect() )
				        dirUnitToAttacker	= pMeshInstance->GetMatrix().GetPos() - GetPos();
                }
			} break;
		default:
			{
				ASSERT( !"Unexpected Attacker Type" );
			}
			break;
		}
	}
    else if ( pDamageData->attackerType == CX2DamageManager::AT_EFFECT )
    {
        if ( pAttackerEffect != NULL && pAttackerEffect->GetOwnerUnit() != NULL && pAttackerEffect->GetOwnerUnit()->GetGameUnitType() == CX2GameUnit::GUT_USER )
        {
            uidAttackerUID = pAttackerEffect->GetOwnerUnit()->GetUnitUID();
            iAttackerUnitIndex = pAttackerEffect->GetOwnerUnit()->GetUnitIndex();
            bAttackedByMe = pAttackerEffect->GetOwnerUnit()->IsMyUnit();
        }
    }

    bool    bDamageReactStateChange = false;
    if ( bLocalUnit == true )
        bDamageReactStateChange = true;
    else if ( bAttackedByMe == true )
    {
        CX2GUUser* pHostUser = g_pX2Game->GetHostGameUnit();
        if ( pHostUser != NULL && g_pX2Room != NULL )
        {
            const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( pHostUser->GetUnitIndex() );
            if ( pSlotData != NULL && pSlotData->m_fAvgPingTime
                + pSlotData->m_fAvgPingStd
                >= X2GUNPC_NONHOST_SIM_MINIMUM_DELAY )
                bDamageReactStateChange = true;
        }//if
    }


	//{{ oasis907 : 김상윤 [2010.4.20] //
	Process_ReflectDamage(pDamageData, pAttackerUnit);
	//}}

	//{{ oasis907 : 김상윤 [2010.2.11] //
	bool IsDamageDirRight = false;
	if ( pDamageData->bArrangedFly == true )
	{
		if( null != pDamageData->optrAttackerGameUnit )
		{
			switch(pDamageData->attackerType)
			{
			case CX2DamageManager::AT_UNIT:
				{
					IsDamageDirRight = pDamageData->optrAttackerGameUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_MESH:
				{
					IsDamageDirRight = pDamageData->optrAttackerGameUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_MANUAL_COLLISION_DATA:
				{
					IsDamageDirRight = pDamageData->optrAttackerGameUnit->GetIsRight();
				} break;
			case CX2DamageManager::AT_EFFECT:
				{
                    if ( pAttackerEffect != NULL )
                    {
					    IsDamageDirRight = pAttackerEffect->GetFollowLineRight();
                    }
				} break;
			default:
				{
					ASSERT( !"Unexpected Attacker Type" );
				} break;
			}
		}
	}
	//}} oasis907 : 김상윤 [2010.2.11] //





	if( GetNowHp() <= 0.f )
	{
		pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
		return;
	}

	if( m_NPCFrameData.stateParam.bInvincible == true &&
		pDamageData->m_fHpPercentUp == 0.f &&
		pDamageData->m_fMpPercentUp == 0.f &&
		CX2DamageManager::RT_THROW != pDamageData->reActType )
	{
		pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
		return;
	}


	if( m_iInvincibleToNPCKeyCode != 0 &&
		pAttackerUnit != NULL &&
		pAttackerUnit->GetGameUnitType() == GUT_NPC )
	{
		if( m_iInvincibleToNPCKeyCode == ((CX2GUNPC*)pAttackerUnit)->GetKeyCode() )
		{
			pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
			return;
		}
	}

	if( true == m_bAbsorbMagicMonster )
	{
		if ( true == m_bAbsorbMagicSpecial &&
			 CX2DamageManager::AT_EFFECT == pDamageData->attackerType &&
			 CX2DamageManager::AT_NORMAL == pDamageData->attackType )
		{
			if ( pAttackerEffect != NULL && pAttackerEffect->GetDamageTime() < 20 )
			{
				pDamageData->reActResult = CX2DamageManager::RT_DUMMY_DAMAGE;
				pDamageData->m_bNoDamage = true;
				m_NPCFrameData.unitCondition.bHit	= true;
				pAttackerEffect->SetDamageTime( 0 );

				if( false == GetAbsorbMagicState() )
				{
					m_bAbsorbMagicState = true;
					if( m_RevengeAttack != 0 )
						StateChange( m_RevengeAttack, true );
				}
				return;
			}
		}
		else if ( true == pDamageData->bCanReflexMagic )
		{
			pDamageData->reActResult = CX2DamageManager::RT_DUMMY_DAMAGE;
			pDamageData->m_bNoDamage = true;
			m_NPCFrameData.unitCondition.bHit	= true;


			float fRestoreHp = GetMaxHp() * 0.1f;

			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
			{
				std::wstringstream strStream;
				strStream << L"HP+ " << (int)fRestoreHp;
				D3DXVECTOR3 pos = GetPos();
				pos.y += 50.0f;
				if( NULL != g_pData->GetPicCharGreen() )
					g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}

			UpNowHp( fRestoreHp );

			if(false == GetAbsorbMagicState())
			{
				m_bAbsorbMagicState = true;
				if( m_RevengeAttack != 0 )
					StateChange( m_RevengeAttack, true );
			}
			return;
		}
	}



	switch( pDamageData->reActType )
	{
	case CX2DamageManager::RT_CREATE_DAMAGE_EFFECT:
	case CX2DamageManager::RT_CREATE_EFFECT_SET:
		{
			return;
		} break;
	}

	bool bRevenge = false;
	bool bReflexMagic = false;

	// 반격여부검사
	if( NULL != m_pXSkinAnim && m_NPCFrameData.stateParam.fRevengeStartTime < m_pXSkinAnim->GetNowAnimationTime()
		&& m_NPCFrameData.stateParam.fRevengeEndTime > m_pXSkinAnim->GetNowAnimationTime()
		&& m_NPCFrameData.unitCondition.bAttackerFront == true )
	{
		bRevenge = true;
	}

	// 마법반사여부 검사
    if( m_v2ReflectMagic.y > 0.f && m_v2ReflectMagic.x <= -1.0f )
	{
		bReflexMagic = true;
	}

	// 반격에 의해 피격음이 변경 된 이후, 이전 피격음으로 복구 시켜주기 위한 임시 저장 값
	CX2DamageManager::HIT_TYPE eOldHitType = ( NULL != pDamageData) ? pDamageData->hitType : CX2DamageManager::HT_NO_SOUND;

	/// 버프로 인한 마법 증가 효과 검사
	if ( false == m_vecReflectMagicByBuffPtr.empty() )
		bReflexMagic = true;

	if( bRevenge == true || bReflexMagic == true )
	{
		if( (bRevenge == true && pDamageData->bCanRevenge == true) ||
			(bReflexMagic == true && pDamageData->bCanReflexMagic == true) )
		{
			/// 버프로 인한 마법 반사가 적용중이라면
			if ( false == m_vecReflectMagicByBuffPtr.empty() )
			{
				vector<CX2ReflectMagicByBuffPtr>::iterator vit = m_vecReflectMagicByBuffPtr.begin();

				if ( NULL != vit->get() )
				{
					/// 버프로부터 반사체 데미지 증가값 반환
					const float fPowerRate = vit->get()->GetValue();

					/// 공격자의 데미지 이펙트에 적용하여 생성
					if(pAttackerEffect != NULL)
						pAttackerEffect->SetPowerRate( pAttackerEffect->GetPowerRate() * fPowerRate );
				}
			}

			m_fRevengeAddDamage					= pDamageData->damage.GetAllDamage();
			pDamageData->reActResult			= CX2DamageManager::RT_REVENGE;

			switch( pDamageData->hitType )
			{
			default:
			case CX2DamageManager::HT_SWORD_SLASH:
			case CX2DamageManager::HT_SWORD_SLASH2:
			case CX2DamageManager::HT_SWORD_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_SWORD_CRASH;
				break;

			case CX2DamageManager::HT_ROD_SLASH:
			case CX2DamageManager::HT_ROD_SLASH2:
			case CX2DamageManager::HT_ROD_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_ROD_CRASH;
				break;

			case CX2DamageManager::HT_PUNCH_HIT:
			case CX2DamageManager::HT_PUNCH_HIT2:
				pDamageData->hitType				= CX2DamageManager::HT_PUNCH_CRASH;
				break;

			case CX2DamageManager::HT_KICK_SLASH:
			case CX2DamageManager::HT_KICK_SLASH2:
			case CX2DamageManager::HT_KICK_HIT:
				pDamageData->hitType				= CX2DamageManager::HT_KICK_CRASH;
				break;

			case CX2DamageManager::HT_ARROW_HIT:
			case CX2DamageManager::HT_ARROW_HIT2:
				pDamageData->hitType				= CX2DamageManager::HT_ARROW_CRASH;
				break;
			}

			m_NPCFrameData.unitCondition.bHit	= true;
		}
	}

	// 자신을 공격한 유닛을 AI에 알려준다
	if( NULL != m_pAI &&
		pAttackerUnit != NULL
		// kimhc // 2011-02-23 // AttackOurTeam 플래그를 이용해서 같은 팀의 몬스터 끼리 버프를 걸어주는 경우(Shadow_defender), Traget을 같은 팀으로 잡는 버그가 발생하여 수정함
		&& false == pDamageData->m_bAttackOurTeam ) // 같은 팀의 공격인 경우 타겟으로 잡지 않음
	{
		m_pAI->SetAttackerGameUnit( pAttackerUnit );
	}


	m_bDamagedThisFrameMove	= true;
	m_eDamageTypeThisFrameMove = pDamageData->reActType;
	m_optrLastGameUnitWhoAttackToMe = pAttackerUnit;

	if( pAttackerUnit != NULL && pDamageData->m_bDrainHpParticle == true )
	{
		if( CX2GameUnit::GUT_USER == pAttackerUnit->GetGameUnitType() )
		{
			if( m_hSeqDrainParticle == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				D3DXVECTOR3 pos = GetPos();
				pos.y = pos.y + 100.0f;

				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GameUnitCreateSequence_LUA( this, "DrainLight",	pos, D3DXVECTOR2(-1,-1), D3DXVECTOR2(3,-1) );
				if( pSeq != NULL )
				{
					D3DXVECTOR3 pos = pAttackerUnit->GetWeaponBonePos( 0, L"TRACE_END0", 0 );

					pSeq->SetLandPosition( GetLandPosition_LUA().y );
					pSeq->SetBlackHolePosition( pos );
					m_hSeqDrainParticle = pSeq->GetHandle();
				}
			}
		}
	}

	if ( AbsorbEffectAttackProcess( pDamageData ) )
		return;

	//적이 전방에 있는가 후방에 있는가
    dirUnitToAttacker.y = 0.f;
	if( D3DXVec3Dot( &GetDirVector(), &dirUnitToAttacker ) >= 0 )
		m_NPCFrameData.unitCondition.bAttackerFront = true;
	else
		m_NPCFrameData.unitCondition.bAttackerFront = false;

	if( m_NPCFrameData.syncData.bIsRight == false )
	{
		m_NPCFrameData.unitCondition.bAttackerFront = !m_NPCFrameData.unitCondition.bAttackerFront;
	}

	bool bViewDamageDir = false;
	if ( pDamageData->bArrangedFly == true )
	{
		// 데미지의 방향성과 캐릭터의 방향이 일치하는지
		if( IsDamageDirRight == GetIsRight() )
		{
			bViewDamageDir = true;
			m_NPCFrameData.unitCondition.bAttackerFront = false;
		}
		else
		{
			bViewDamageDir = false;
			m_NPCFrameData.unitCondition.bAttackerFront = true;
		}
	}

#ifdef ADD_BACKATTACK
	bool bCriticalHit = false;
	bool bBackAttack = false;
	if( m_bApplyBackAttack == true &&
		pDamageData->m_bApplyBackAttack == true &&
		IsSuperArmor() == false &&
		m_NPCFrameData.unitCondition.bAttackerFront == false )
	{
		bBackAttack = true;
	}
#endif

    // 가드여부 판정
	if( true == m_bUseGuardCollisionBox )
	{
		if( pDamageData->m_bGuard == true && m_DamageGuardPercent != 0 )
		{
			// 가드박스에 충돌
            if ( NOWSTATE_FLAG( GUARD_DEFENCE_FRONT ) == true )
			{
				m_bGuard = m_NPCFrameData.unitCondition.bAttackerFront;
			}
			else
			{
				m_bGuard = true;
			}
		}
		else
		{
			m_bGuard = false;
		}
	}
	else
	{
		if( m_DamageGuardPercent != 0 )
		{
			if(
                NOWSTATE_FLAG( GUARD_DEFENCE_FRONT ) == true
				&& true == m_NPCFrameData.unitCondition.bAttackerFront )
			{
				m_bGuard = true;
				pDamageData->m_bGuard = true;
			}
			else if(
                NOWSTATE_FLAG( GUARD_DEFENCE_BACK ) == true
                && false == m_NPCFrameData.unitCondition.bAttackerFront )
			{
				m_bGuard = true;
				pDamageData->m_bGuard = true;
			}
			else
			{
				m_bGuard = false;
				pDamageData->m_bGuard = false;
			}
		}
		else
		{
			m_bGuard = false;
		}

	}


    float fPrevNowSpeedX = m_PhysicParam.nowSpeed.x;

	if( IsSuperArmor() != true && m_bGuard != true && m_ExtraDamagePack.m_Frozen.m_fTime <= 0.f )	// 빙결시 움직이지 않도록 수정
	{
	    if( bLocalUnit == true || bDamageReactStateChange == true )
        {

			switch( pDamageData->reActType )
			{
			case CX2DamageManager::RT_FLIP_SAME_DIR:
				{
					// 앞 또는 뒤쪽 으로만
					m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
				} break;
			case CX2DamageManager::RT_NO_DAMAGE:
			case CX2DamageManager::RT_DUMMY_DAMAGE:
				{

				} break;
			default:
				{
					//{{ oasis907 : 김상윤 [2010.2.11] //
					if ( pDamageData->bArrangedFly == true )
					{
						if( bViewDamageDir == true )
						{
							m_PhysicParam.nowSpeed.x = pDamageData->backSpeed.x;
						}
						else
						{
							m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
						}
					}
					else
						//}} oasis907 : 김상윤 [2010.2.11] //
					{
						if( m_NPCFrameData.unitCondition.bAttackerFront == true )
						{
							m_PhysicParam.nowSpeed.x = -pDamageData->backSpeed.x;
						}
						else
						{
							m_PhysicParam.nowSpeed.x = pDamageData->backSpeed.x;
						}
					}
				} break;
			}
        }
	}

#if X2_DEV_TOOLS
	bool bShowDamageReact = true;
	// 2013-01-30 // 오현빈
	// 옵션에서 내가 준 데미지만 보여 주도록 켜져 있는데
	// 현재 데미지의 optrAttackerGameUnit이 내가 아니라면
	// 데미지를 보여주지 않기
	if( NULL != g_pMain &&
		true == g_pMain->GetGameOption().GetShowOnlyMyDamage() &&
		false == pDamageData->optrAttackerGameUnit->IsMyUnit()
		)
	{
		bShowDamageReact = false;
	}
#endif //SHOW_ONLY_MY_DAMAGE

	//타격 방식에 따른 반응(사운드 등)
	wstring wstrHitSound = L"";
	if( pDamageData->reActResult != CX2DamageManager::RT_NO_DAMAGE
        && pDamageData->reActResult != CX2DamageManager::RT_SENSOR
		&& m_bGuard != true )
	{
		const WCHAR* wszSoundName = NULL;
	//{{ kimhc // 2011.1.21 // 청 1차 전직
		if ( CX2DamageManager::HTD_NO_SOUND != nowStateData.m_eHittedTypeAtState)
			wszSoundName = g_pData->GetDamageManager()->GetDamageSound( pDamageData->hitType, nowStateData.m_eHittedTypeAtState );
		else
	//}} kimhc // 2011.1.21 // 청 1차 전직
			wszSoundName = g_pData->GetDamageManager()->GetDamageSound( pDamageData->hitType, m_HittedType );

		if( NULL != wszSoundName )
		{
			PlaySound( wszSoundName );
			wstrHitSound = wszSoundName;
		}
		if( NULL != pDamageData &&
			CX2DamageManager::HT_NO_SOUND != eOldHitType )
		{
			pDamageData->hitType = eOldHitType;
		}
	}

    // 마지막 타격이 무엇인지에 따라 죽을때 날릴지 어떨지 결정
    if(pDamageData->reActResult == CX2DamageManager::RT_UP || pDamageData->reActResult == CX2DamageManager::RT_FLY
		//{{ kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격
		|| pDamageData->reActResult == CX2DamageManager::RT_DRAG_UP
		//}} kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격
		)
    {
        m_bDieFly = true;
    }
    else
    {
        m_bDieFly = false;
    }





	if( true == IsDamagedReaction( pDamageData ) )
	{


		if( (m_bGuard != true || (m_bGuard == true && pDamageData->m_bDrainMana == true)) &&
			(m_bImmuneToExtraDamage == false) )
		{
			// 버프 데미지도 같이 처리한다
			if( pDamageData->m_BufExtraDamage.m_ExtraDamageType != CX2DamageManager::EDT_DULL ||
				pDamageData->m_ExtraDamage.m_ExtraDamageType != CX2DamageManager::EDT_DULL ||
				 GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC )
			{
				DamageReact_ExtraDamage( pDamageData, true
                    , bAttackedByMe
                    );
			}

			// 공격자의 속성 인챈트 extra_damage
			const float fRandomValue1 = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_ENCHANT );
			const float fRandomValue2 = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_ENCHANT + 1 );
			g_pData->GetEnchantItem()->ProcessEnchant( this, pDamageData, fRandomValue1, fRandomValue2 );

			DoCustomFunctionByBuffInDamageReact( *pDamageData );
		}

		ApplyBuffToGameUnitInDamageReact( pDamageData );
		PlayEffectSetHittedByBuff();
		DoDelegateProcessInDamageReact();
		DoDelegateProcessWithDamageDataInDamageReact( *pDamageData );
	}


	// modified by wonpok. 20090821. dummy damage에 대해서는 StopTime을 풀지 않는다.
	//if( m_NPCFrameData.stateParam.bSuperArmor != true )
	if( IsSuperArmor() != true && pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE )
	{
		if(pDamageData->reActResult != CX2DamageManager::RT_NO_DAMAGE)
		{
			SetStopTime( pDamageData->fStopTimeDef );
			SetStop2Time( 0.0f );
			m_fCoolTime = 0.0f;
		}
	}

	m_DamageLightTime = 0.2f;



	if( bDamageReactStateChange == true )
    {
        bool bIsRightBackup = m_NPCFrameData.syncData.bIsRight;
		bool bStateChanged = DamageReactStateChange( pDamageData, iAttackerUnitIndex );
        if ( bStateChanged == true )
        {
        }
        if ( bStateChanged == false && bLocalUnit == false )
        {
            m_PhysicParam.nowSpeed.x = fPrevNowSpeedX;
        }
        else if ( bLagConsideration == true &&
            m_NPCFrameData.syncData.bIsRight == bIsRightBackup &&
            m_PhysicParam.fReloadAccel > 0.f
			&& pDamageData->bArrangedFly == false
            )
        {
            m_kNonHostReaction.AdjustDamageReactionSpeed( m_NPCFrameData, m_PhysicParam, bIsRightBackup, GetWalkSpeed(), fLagLatency );
        }
    }

	if( true == IsDamagedReaction( pDamageData ) &&
		false == pDamageData->m_bNoDamage
		//{{ kimhc // 2010.6.29 // 피격 모션등은 실행되고, 받는 데미지는 모두 0가 되게하는 기능
#if X2_DEV_TOOLS
			&& false == GetZeroDamage()
#endif // DAMAGE_ZERO
	//}} kimhc // 2010.6.29 // 피격 모션등은 실행되고, 받는 데미지는 모두 0가 되게하는 기능
		)
	{
		if ( GetHasBossGage() )
			CX2GageManager::GetInstance()->ShowBossGageUIByUIDAndNotShowOtherBossGageUI( GetUnitUID() );

		float fFinalDamage = GetFinalDamage( pDamageData );

		// 데미지 강제 보정(알테라 평원 보스 예외처리)
		if( g_pX2Game != NULL && GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_NASOD_ELEMENT_ICE )
		{
			for( int i=0; i < g_pX2Game->GetNPCUnitListSize(); ++i)
			{
				CX2GUNPC *pUnit = g_pX2Game->GetNPCUnit(i);
				if( pUnit != NULL && pUnit->IsUnitID( CX2UnitManager::NUI_BARRIERMACHINE_ICE ) == true &&
					pUnit->GetFlag_LUA(3) == true )
				{
					 fFinalDamage = 1.f;
					 break;
				}
			}
		}
		else if( g_pX2Game != NULL && GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_NASOD_ELEMENT_FIRE )
		{
			for( int i=0; i < g_pX2Game->GetNPCUnitListSize(); ++i)
			{
				CX2GUNPC *pUnit = g_pX2Game->GetNPCUnit(i);
				if( pUnit != NULL && pUnit->IsUnitID( CX2UnitManager::NUI_BARRIERMACHINE_FIRE ) == true &&
					pUnit->GetFlag_LUA(3) == true )
				{
					fFinalDamage = 1.f;
					break;
				}
			}
		}



		if( (CX2EnchantItem::ENCHANT_TYPE)pDamageData->m_ResistType != CX2EnchantItem::ET_NONE )
		{
			ApplyEnchantResist( (CX2EnchantItem::ENCHANT_TYPE)pDamageData->m_ResistType, fFinalDamage );
		}
		D3DXVECTOR3 numPos;
		if( true == GetIsBosRaidNPC() )
		{
			numPos = pDamageData->impactPoint;
		}
		else
		{
			GetFramePos( &numPos, m_pFrame_Bip01_Head );
			if( numPos.x == 0.0f && numPos.y == 0.0f && numPos.z == 0.0f )
			{
				numPos = GetPos();
				numPos.y += 150.0f;
			}
		}
		numPos.y += 75.0f;

		D3DXVECTOR3 criticalNumPos = numPos;

		criticalNumPos.y += 40.0f;

		//{{ dmlee 2009.5.13 공격형태에 관계없이 크리티컬 터질수 있게 수정, 아이샤 라이트닝볼트와 구체연타, 엘소드 필살기 중 일부가 AT_MESH, AT_MANUAL_COLLISION_DATA를 사용하고 있음
		//if ( ( pDamageData->attackerType == CX2DamageManager::AT_UNIT ||
		//	pDamageData->attackerType == CX2DamageManager::AT_EFFECT ) &&
		//	pDamageData->pAttackerUnit != NULL )
		//{

		bool bShowRedPicture = false;	// 감소된 HP를 빨간색으로 보여줘야 하는지 여부
		if( null != pDamageData->optrAttackerGameUnit )
		{
		//}} dmlee 2009.5.13 공격형태에 관계없이 크리티컬 터질수 있게 수정, 아이샤 라이트닝볼트와 구체연타, 엘소드 필살기 중 일부가 AT_MESH, AT_MANUAL_COLLISION_DATA를 사용하고 있음

			//{{ 2011-04 에 패치될 청 메모 헤드샷
			// 헤드샷이 적용될 것인지 판단 후 수행함
			ApplyHeadShotDamage( &fFinalDamage );
			//}} 2011-04 에 패치될 청 메모 헤드샷

		float fCriticalRate = pDamageData->optrAttackerGameUnit->GetCriticalPercent( pDamageData->damageType, pDamageData->m_fCriticalRate );

		pDamageData->optrAttackerGameUnit->SetCriticalPercent_WayOfSword( IN pDamageData->m_WayofSwordType,
																		OUT fCriticalRate );

#ifdef ADD_BACKATTACK
			if( bBackAttack == true )
			{
				fCriticalRate *= 1.5f;
			}
#endif
			if( 0.f < m_fAntiCriticalRate )
			{
				fCriticalRate -= m_fAntiCriticalRate;
				if(fCriticalRate < 0.f)
					fCriticalRate = 0.f;
			}
			//{{ 2011-04 에 패치될 청 캐시 스킬
			if( GetCriticalValueFromDefender() <= fCriticalRate )
			//}} 2011-04 에 패치될 청 캐시 스킬
			{
				bShowRedPicture = true;
				float fDefenderCriticalValue = 0.f;

				// 레나 낙인의 사격에 피해를 받은 상태인 경우에 받는 크리티컬 데미지가 증가한다
				if( m_ExtraDamagePack.m_StigmaDebuff.m_fTime > 0.f )
				{
					fDefenderCriticalValue = m_ExtraDamagePack.m_StigmaDebuff.m_DamagePerSec;	// 크리티컬 데미지 증가 비율을 곱한다
				}

				// 마법 공격에만 적용되는 크리티컬 데미지 배율 설정
				if( CX2GUUser::GUT_USER	== pAttackerUnit->GetGameUnitType() )
				{
					CX2GUUser* pGUUser = static_cast<CX2GUUser*>(pAttackerUnit);
					if( NULL != pGUUser )
					{
						switch( pDamageData->damageType )
						{
						case CX2DamageManager::DT_MAGIC:
						case CX2DamageManager::DT_MIX:
						case CX2DamageManager::DT_WEAPON_MAGIC:
						case CX2DamageManager::DT_WEAPON_MIX:
							{
								// 기본 크리티컬 공격 배율 + 마법 공격 크리티컬 배율
								float fCriticalDamageRate = pGUUser->GetCriticalValue( fDefenderCriticalValue );
								fCriticalDamageRate += pGUUser->GetAddMagicAttackCriticalDamageRate();

								fCriticalDamageRate += pDamageData->m_fApplyCriticalDamage ;

								// 크리티컬 공격 방어력 적용
								fCriticalDamageRate *= GetCriticalDefence();

								// FinalDamage 에 크리티컬 공격 적용
								fFinalDamage *= fCriticalDamageRate;
							} break;
						default:
							{
								float fCritical = pGUUser->GetCriticalValue( fDefenderCriticalValue );

								fCritical += pDamageData->m_fApplyCriticalDamage ;

								// 크리티컬 공격 방어력 적용
								fCritical *= GetCriticalDefence();

								if ( fCritical >= 1.0f )
									fFinalDamage *= fCritical;
							}
							break;
						}
					}
				}
				else
				{
					float fCritical = pDamageData->optrAttackerGameUnit->GetCriticalValue( fDefenderCriticalValue ) * GetCriticalDefence();
					if ( fCritical >= 1.0f )
						fFinalDamage *= fCritical;
				}

#if X2_DEV_TOOLS
				if( true == bShowDamageReact )
#endif //SHOW_ONLY_MY_DAMAGE
				{
					//크리티컬~~
					D3DXVECTOR3 criticalEffPos = criticalNumPos;
					criticalEffPos.y += 20.f;

					g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"CriticalHit01", criticalEffPos );

#ifdef ADD_BACKATTACK
					g_pX2Game->GetX2Camera()->GetCamera().ImpactForward( 25.f, 150.f, 15.f, 0.6f, 1 );
#else
					UpDownCrashCamera(40.f, 0.3f);
#endif
					//pDamageData->fCameraCrashGap = 40.0f;
					//pDamageData->fCameraCrashTime = 0.3f;

					PlaySound( L"Critical_Hit.wav" );

#ifdef ADD_BACKATTACK
					bCriticalHit = true;
#endif
				}
			}
		}

#ifdef ADD_BACKATTACK
		if( bBackAttack == true && bCriticalHit == false )
		{
			bShowRedPicture = true;
			fFinalDamage *= 1.1f;
			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera(50.f, 0.3f);

			PlaySound( L"HitKingNasod.ogg", true, false );

			D3DXVECTOR3 vBackAttackPos = GetHeadBonePos();
			vBackAttackPos.y += 110.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"damage_backAttack_P01", GetHeadBonePos() );
		}
#endif
		/// 더블 어택 디버프 발동 여부 연산
		if(  CX2DamageManager::AT_NORMAL ==  pDamageData->attackType )
		{
			if( IsDoubleAttackByBuff() )
            {
                m_eDoubleAttack = ( bAttackedByMe == true ) ? DOUBLE_ATTACK_BY_ME : DOUBLE_ATTACK_BY_OTHER;
            }
		}

		// 더블어택은 데미지를 2배로 주지만, UI에 표시하기는 데미지를 반반씩 2번 보여준다. (UI 분리는 뒷부분에..)
        if ( m_eDoubleAttack != DOUBLE_ATTACK_NONE )
		{
			fFinalDamage *= 2.f;	// 실제 데미지만 증가
			bShowRedPicture = true; // 더블어택의 경우 빨간색으로 표시!!
		}

		bool bCanDrainHp = false;

		CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
		if( NULL != pAttackerUnit )
		{
			pAttackerUnit->GetDrainHpBuffValue( pDamageData );

		}

		if( pDamageData->m_fDrainHpRate > 0.f && pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
		{
			if( GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC )
				bCanDrainHp = true;
		}

		if( bCanDrainHp == true
#if X2_DEV_TOOLS
			&& true == bShowDamageReact
#endif //SHOW_ONLY_MY_DAMAGE
			)
		{
			if( pAttackerUnit != NULL && pAttackerUnit->GetGageData() != NULL )
			{
				float fDrainDamage = fFinalDamage * pDamageData->m_fDrainHpRate;

				if( fDrainDamage > 0.f )
				{
					if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || pAttackerUnit->IsMyUnit() == true )
					{
						std::wstringstream strStream;
						strStream << L"HP+ " << (int)fDrainDamage;
						D3DXVECTOR3 pos = pAttackerUnit->GetPos();
						pos.y += 50.0f;
						if( NULL != g_pData->GetPicCharGreen() )
							g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pAttackerUnit->GetDirVector(), CKTDGPicChar::AT_CENTER );
					}

					pAttackerUnit->UpNowHp( fDrainDamage );
				}
			}
		}

		if( pDamageData->m_fDrainMpByReact > 0.f
#if X2_DEV_TOOLS
			&& true == bShowDamageReact
#endif //SHOW_ONLY_MY_DAMAGE
			)
		{
			if( pAttackerUnit != NULL && pAttackerUnit->GetGageData() != NULL )
			{
				pAttackerUnit->UpNowMp( pDamageData->m_fDrainMpByReact * pDamageData->m_fDrainMpRate );

				if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || pAttackerUnit->IsMyUnit() == true )
				{
					std::wstringstream strStream;
					strStream << L"MP+ " << (int)(pDamageData->m_fDrainMpByReact * pDamageData->m_fDrainMpRate);
					D3DXVECTOR3 pos = pAttackerUnit->GetPos();
					pos.y += 140.0f;
					if( NULL != g_pData->GetPicCharGreen() )
						g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pAttackerUnit->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}

				UpNowMp( -1.f * pDamageData->m_fDrainMpByReact );

				if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || pAttackerUnit->IsMyUnit() == true )
				{
					std::wstringstream strStreamDamage;
					strStreamDamage << L"MP -" << (int)pDamageData->m_fDrainMpByReact;
					D3DXVECTOR3 posTarget = GetPos();
					posTarget.y += 70.f;
					if( NULL != g_pData->GetPicCharRed() )
						g_pData->GetPicCharRed()->DrawText( strStreamDamage.str().c_str(), posTarget, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		}

		////////////////////////// fFinalDamage의 변경은 이 위로 해주세요!!! (옵션 수치화 제외)///////////////////////////////////

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		if( pDamageData->m_bLvDamage == false )
		{
			// 모든 계산을 거친 최종 대미지에 옵션 수치화의 데미지 감소를 적용 (snatch에 영향을 주지 않음)
			if ( 0.0f < GetAdditionalDefence() )
			{
				fFinalDamage -= min( fFinalDamage * GetAdditionalDefence(), fFinalDamage );
			}

			// 모든 계산을 거친 최종 대미지에 옵션 수치화의 데미지 감소를 적용 (snatch에 영향을 주지 않음)
			if ( null != pDamageData->optrAttackerGameUnit )
			{
				const float fAdditionalAttackValue = pDamageData->optrAttackerGameUnit->GetAdditionalAttackDamage( pDamageData );


				if ( 0.0f < fAdditionalAttackValue )
				{
					fFinalDamage += fAdditionalAttackValue;

					// 더블 어택의 경우 한번 더 더해준다.
                	if ( m_eDoubleAttack != DOUBLE_ATTACK_NONE )
                	{
						fFinalDamage += fAdditionalAttackValue;
                	}
				}

				if ( GUT_USER == pDamageData->optrAttackerGameUnit.GetObservable()->GetGameUnitType() )
				{
					CX2GUUser* pUser = static_cast< CX2GUUser* >( pDamageData->optrAttackerGameUnit.GetObservable() );

					if ( NULL != pUser && NULL != pUser->GetUnit() && CX2Unit::UT_ADD == pUser->GetUnit()->GetType() )
					{
						/// 스테이시스 필드용 타격 수치 연산해 주자
						pUser->SetStasisfieldHitValue( fFinalDamage );
					}
				}
			}
		}
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

		// 데미지가 보여야 하는 경우 (아토믹 실드에 맞은 경우에는 보이지 않아야 함)
		if ( true == m_bDamageOutScreen
#if X2_DEV_TOOLS
			&& true == bShowDamageReact
#endif //SHOW_ONLY_MY_DAMAGE
			)
		{
			// UI상 타격된 데미지를 보여주기 위한 용도
			float fFinalDamageToShow = fFinalDamage;

			// 더블 어택의 경우
            if ( m_eDoubleAttack != DOUBLE_ATTACK_NONE )
			{
				// 보여주는 UI상의 수치를 1/2로 하여 두번 보여준다.
				fFinalDamageToShow *= 0.5f;
				SetDoubleAttack(wstrHitSound, fFinalDamageToShow, pDamageData->impactPoint);	// 한번 더 보여줌
			}

			// 크리티컬이거나 더블 어택이면 빨간색으로 표시
			if ( true == bShowRedPicture )
			{
				/// 데미지가 0 이면
				if( ( 0.0f < fFinalDamageToShow) && NULL != g_pData->GetPicCharRed() )
					if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
						g_pData->GetPicCharRed()->DrawText( static_cast<int>( fFinalDamageToShow ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
			else	// 그렇지 않으면 일반 노란색
			{
				if( ( 0.0f < fFinalDamageToShow) && NULL != g_pData->GetPicChar() )
					if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
						g_pData->GetPicChar()->DrawText( static_cast<int>( fFinalDamageToShow ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}

			if( bLocalUnit == true && g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
			{
				// 자유훈련소에는 나혼자 밖에 없으므로 공격자가 누군지에 대한 정보가 필요없음.
				CX2GUUser *pMyUser = g_pX2Game->GetMyUnit();

				if( pMyUser != NULL && g_pX2Game->IsValidUnit(pMyUser) == true )
				{
					int iComboCount = pMyUser->GetComboManager()->GetCombo();
					if( iComboCount <= 0 )
					{
						pMyUser->GetComboManager()->ResetMyComboDamage();
					}

					pMyUser->GetComboManager()->AddMyComboDamage( fFinalDamage );
				}
			}
		}


		if( bLocalUnit == true && m_bDamageHistory == true )
		{
			*m_fDamageHistory += fFinalDamage;
		}
		if( NULL != pAttackerUnit )
		{
			if ( CX2GameUnit::GUT_USER == pAttackerUnit->GetGameUnitType() )
				AddToDamagedMap( pAttackerUnit, fFinalDamage );
			else	/// NPC 이면
			{
				CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pAttackerUnit );
				if ( NULL != pNPC->GetNPCAI() && CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )	// 유저가 소환한 몬스터 이면, 유저가 때린걸로 카운트 한다
				{
					CX2AllyNPCAI* pAllyAI = static_cast<CX2AllyNPCAI*>( pNPC->GetNPCAI() );
					if ( CX2GameUnit::GUT_USER == pAllyAI->GetAllyUnitType() )
						AddToDamagedMap( pAllyAI->GetAllyGameUnit(), fFinalDamage );
				}
			}
		}





        if( true == bLocalUnit )
		{
			SetHittedCount( GetHittedCount() + 1 );
			UpNowHp( -fFinalDamage );


			UpNowMp( 3.0f );
			UpNowSoul( 2.0f );

			float fForceDownValue = pDamageData->fForceDownValue;
			fForceDownValue -= pDamageData->m_fDecreaseForceDown;
			if(pDamageData->m_bPenetrate == true)
			{
				if(fForceDownValue < 0.f)
					fForceDownValue -= 10.f;
				else
					fForceDownValue = -10.f;
			}
			AddForceDownDelay( fForceDownValue );

		}

			if( pAttackerUnit != NULL )
			{
				if( pAttackerUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					SetGameUnitWhoAttackedMe( pAttackerUnit );

					//{{ JHKang / 강정훈 / 2011/02/21 / 유저가 NPC(몬스터)에게 준 데미지
					CX2GUUser *pMyUnit = g_pX2Game->GetMyUnit();

				if ( NULL != pMyUnit && pMyUnit->GetUnitUID() == pAttackerUnit->GetUnitUID() )
				{
					pMyUnit->AddGivenDamamge( static_cast<int>(fFinalDamage) );
					pMyUnit->SetCurrentDamage( fFinalDamage );
				}
				//}}
			}
			else
			{
				// fix!! 이브의 소환 몬스터에 의해 타격이 된경우에 유저의 타격 점수로 계산되어야 한다.

				//{{ JHKang / 강정훈 / 2011/02/21 / 소환된 유닛이 NPC(몬스터)에게 준 데미지
					if ( null != pDamageData->optrAttackerGameUnit )
					{
						SetGameUnitWhoAttackedMe( pAttackerUnit );

					CX2GUNPC*		pAttackerNpc = static_cast<CX2GUNPC*>( pDamageData->optrAttackerGameUnit.GetObservable() );
					CX2GameUnit*	pOwnerGameUnitOfAttackerNpc = pAttackerNpc->GetOwnerGameUnit();
					if ( NULL != pOwnerGameUnitOfAttackerNpc && pOwnerGameUnitOfAttackerNpc->IsMyUnit() )
					{
						CX2GUUser *pMyUnit = g_pX2Game->GetMyUnit();

						if ( NULL != pMyUnit )
						{
							pMyUnit->AddGivenDamamge( static_cast<int>(fFinalDamage) );
							pMyUnit->SetCurrentDamage( fFinalDamage );


						}
					}
				}
				//}}
			}
		}
		else	// 방장이 아닐 경우 콤보를 위한 final Damage 전달
		{
			if( pAttackerUnit != NULL )
			{
				if( pAttackerUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					CX2GUUser *pMyUnit = g_pX2Game->GetMyUnit();

					if ( NULL != pMyUnit && pMyUnit->GetUnitUID() == pAttackerUnit->GetUnitUID() )
					{
						pMyUnit->SetCurrentDamage( fFinalDamage );
					}
				}
				else
				{
					if ( null != pDamageData->optrAttackerGameUnit )
					{
						CX2GUNPC*		pAttackerNpc = static_cast<CX2GUNPC*>( pDamageData->optrAttackerGameUnit.GetObservable() );
						CX2GameUnit*	pOwnerGameUnitOfAttackerNpc = pAttackerNpc->GetOwnerGameUnit();
						if ( NULL != pOwnerGameUnitOfAttackerNpc && pOwnerGameUnitOfAttackerNpc->IsMyUnit() )
						{
							CX2GUUser *pMyUnit = g_pX2Game->GetMyUnit();

							if ( NULL != pMyUnit )
							{
								pMyUnit->SetCurrentDamage( fFinalDamage );

								if( NULL != pMyUnit->GetComboManager() )
									pMyUnit->GetComboManager()->AddCombo();
							}
						}
					}
				}
			}
		}

	}


	//{{ JHKang / 강정훈 / 2011.3.25 / 애니메이션 속도 증가 버프
	//}}

	if(pDamageData->m_fHpPercentUp > 0.f)
	{
		float fUpRate = GetMaxHp() * pDamageData->m_fHpPercentUp;
		float fHpUp = GetNowHp() + fUpRate;
		SetNowHp( fHpUp );
	}

	if(pDamageData->m_fMpPercentUp > 0.f)
	{
		float fUpRate = GetMaxMp() * pDamageData->m_fMpPercentUp;
		float fMpUp = GetNowMp() + fUpRate;
		SetNowMp( fMpUp );
	}

	if( pDamageData->m_fHpPercentUp > 0.f || pDamageData->m_fMpPercentUp > 0.f )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet(L"EffectSet_Heal", (CX2GameUnit*)this );
		PlaySound( L"Nasod_Healer_Heal.ogg" );
	}
	// 타격받았을경우 이펙트 종료를 위해
	// DELETE_EFFECT_SET_ON_DAMAGE_REACT가 TRUE인 경우 등록된 현재 스테이트의 effectset들을 지운다.
	if( IsSuperArmor() == false &&
		m_NPCFrameData.stateParam.bInvincible == false &&
		CX2DamageManager::RT_DUMMY_DAMAGE != pDamageData->reActResult
		&& CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE != pDamageData->reActResult
		)
	{
		DeleteEffectSetOnDamageReact();
	}



	//{{ megagame : 박교현 : [2010-04-06]
	DamageReact_HitCount( pDamageData );
	//}} megagame : 박교현 : [2010-04-06]

	DamageReact_Rage( pDamageData );

	PostProcessEnchantDamage( pDamageData );

#if !X2_DEV_TOOLS
#endif
}

void CX2GUNPC::DamageDoubleAttack()
{
    m_eDoubleAttack = DOUBLE_ATTACK_NONE;
	m_fDoubleAttackTime = 0.f;

	if( m_wstrHitSound.empty() == false )
	{
		PlaySound( m_wstrHitSound.c_str() );
	}

	if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH
        || m_eDoubleAttack == DOUBLE_ATTACK_BY_ME )
	{
		D3DXVECTOR3 numPos;
		GetFramePos( &numPos, m_pFrame_Bip01_Head );
		if( numPos.x == 0.0f && numPos.y == 0.0f && numPos.z == 0.0f )
		{
			numPos = GetPos();
			numPos.y += 150.0f;
		}
		numPos.y += 75.0f;

		if( g_pData->GetPicCharRed() != NULL )
			g_pData->GetPicCharRed()->DrawText( (int)m_fFinalDamage, numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
	}

    m_eDoubleAttack = DOUBLE_ATTACK_NONE;

	int randVal = rand() % 1001;
	int iRandX = randVal % 21 - 10;
	int iRandZ = (randVal / 100) % 21 - 10;
	int iRandY = (rand() % 21 - 10);
	m_vImpactPoint.x += (float)iRandX;
	m_vImpactPoint.y += (float)iRandY;
	m_vImpactPoint.z += (float)iRandZ;

	CKTDGParticleSystem::CParticleEventSequence* pSeq1 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpact", m_vImpactPoint, 50, 50, 1, 1 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq2 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpact", m_vImpactPoint, 100, 200, 2, 10 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq3 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpactCore", m_vImpactPoint, 100, 200, 2, 10 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq4 = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DamageImpactSlash", m_vImpactPoint, 50, 50, 2, 2 );

	if(pSeq1 != NULL)
		pSeq1->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq2 != NULL)
		pSeq2->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq3 != NULL)
		pSeq3->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
	if(pSeq4 != NULL)
		pSeq4->SetScaleFactor(D3DXVECTOR3(1.5f, 1.5f, 1.5f));


	UpDownCrashCamera(20.f);
}



// @bApplyExtraDamageOfAttacker: pDamageData->pAttackerUnit의 damagedata를 적용할지 말지
void CX2GUNPC::DamageReact_ExtraDamage( CX2DamageManager::DamageData* pDamageData, bool bApplyExtraDamageOfAttacker
                    , bool bAttackedByMe
                    )
{

	KTDXPROFILE();

#if !X2_DEV_TOOLS
#endif

	if( NULL == pDamageData )
		return;

	if( pDamageData->m_bDrainMana == true && GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
		return;


	// oasis907 : 김상윤 [2010.3.22] // 함수 하나로 묶었습니다
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_ExtraDamage);
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_BufExtraDamage);
	LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_SocketExtraDamage);
	// oasis907 : 김상윤 [2010.10.11] // 중복 가능한 EDT가 빠져서 추가합니다.
	for( int i=0; i<(int)pDamageData->m_MultipleExtraDamage.size(); i++ )
	{
		LevelCorrectionExtraDamage(pDamageData, &pDamageData->m_MultipleExtraDamage[i]);
	}
	//}}


	float randVal = GetRandomFloat();
	// 스크립트에 있는 extra damage
	const CX2DamageManager::ExtraDamageData& extraDamageData = pDamageData->m_ExtraDamage;
	if( CX2DamageManager::EDT_NONE != extraDamageData.m_ExtraDamageType )
	{
		ASSERT( extraDamageData.m_fRate >= 0.f && extraDamageData.m_fRate <= 1.f );
		if( extraDamageData.m_fRate > (float)randVal )
		{
			Process_ExtraDamage( pDamageData, extraDamageData
                , bAttackedByMe
                );
		}
	}

	// 버프용 extra damage
	if( pDamageData != NULL && null != pDamageData->optrAttackerGameUnit
        //&& pDamageData->optrAttackerGameUnit->GetDamageData() != NULL
        )
	{
		const CX2DamageManager::ExtraDamageData& bufExtraDamageData = pDamageData->optrAttackerGameUnit->GetDamageData().m_BufExtraDamage;
		if( CX2DamageManager::EDT_NONE != bufExtraDamageData.m_ExtraDamageType &&
			bufExtraDamageData.m_fRate > (float)randVal )
		{
			if( pDamageData->attackerType != CX2DamageManager::AT_EFFECT ||
				pDamageData->optrAttackerGameUnit->GetDamageData().m_bApplyExtraDamage == true )
			{
				if( pDamageData->m_bNoBuff == false && pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
					Process_ExtraDamage( pDamageData, bufExtraDamageData
                        , bAttackedByMe
                    );
			}
		}
	}

	if( pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
	{
		// 소켓 옵션에 의한 extra damage
		const CX2DamageManager::ExtraDamageData& socketExtraDamageData = pDamageData->m_SocketExtraDamage;
		if( CX2DamageManager::EDT_NONE != socketExtraDamageData.m_ExtraDamageType )
		{
			ASSERT( socketExtraDamageData.m_fRate >= 0.f && socketExtraDamageData.m_fRate <= 1.f );
			if( socketExtraDamageData.m_fRate > (float)randVal )
			{
				Process_ExtraDamage( pDamageData, socketExtraDamageData
                        , bAttackedByMe
                    );
			}
		}

		// 중첩가능한 extra damage
		for( int i=0; i<(int)pDamageData->m_MultipleExtraDamage.size(); i++ )
		{
			const CX2DamageManager::ExtraDamageData& multipleExtraDamageData = pDamageData->m_MultipleExtraDamage[i];
			//ASSERT( multipleExtraDamageData.m_fRate >= 0.f && multipleExtraDamageData.m_fRate <= 1.f );

			float r = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_MULTIPLE + i );
			if( r < multipleExtraDamageData.m_fRate )
			{
				Process_ExtraDamage( pDamageData, multipleExtraDamageData
                        , bAttackedByMe
                    );
			}
		}

		// 공격 형태가 AT_EFFECT이면 (예를들면, 화살이나 파이어볼) 공격자의 소켓옵션 extra_damage도 적용한다.
		if( true == bApplyExtraDamageOfAttacker &&
			CX2DamageManager::AT_EFFECT == pDamageData->attackerType &&
			null != pDamageData->optrAttackerGameUnit )
		{
			pDamageData->optrAttackerGameUnit->GetDamageData().optrAttackerGameUnit = pDamageData->optrAttackerGameUnit;
			//{{ oasis907 : 김상윤 [2010.3.23] //
			pDamageData->optrAttackerGameUnit->GetDamageData().optrDefenderGameUnit = pDamageData->optrDefenderGameUnit;
			//}}
			pDamageData->optrAttackerGameUnit->GetDamageData().impactPoint = pDamageData->impactPoint;

			{
//{{ 오현빈 // 2012-06-28 // AT_EFFECT 공격에 의도하지 않은 공격자의 익스트라데미지가 적용되는 문제 수정
				//소켓옵션은 적용 받을 수 있도록 m_SocketExtraDamage, m_MultipleExtraDamage 만 처리하도록 수정
				const CX2DamageManager::ExtraDamageData& SocketExtraDamage = pDamageData->optrAttackerGameUnit->GetDamageData().m_SocketExtraDamage;
				if( CX2DamageManager::EDT_NONE != SocketExtraDamage.m_ExtraDamageType &&
					SocketExtraDamage.m_fRate > (float)randVal )
				{
					LevelCorrectionExtraDamage( pDamageData, &pDamageData->optrAttackerGameUnit->GetDamageData().m_SocketExtraDamage );
					Process_ExtraDamage( pDamageData, SocketExtraDamage
                        , bAttackedByMe
                        );
				}

				for( int i = 0; i < (int)pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage.size(); ++i )
				{
					const CX2DamageManager::ExtraDamageData& multipleExtraDamageData = pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage[i];
					LevelCorrectionExtraDamage( pDamageData, &pDamageData->optrAttackerGameUnit->GetDamageData().m_MultipleExtraDamage[i] );

					float r = GetRandomFloat( CKTDXRandomNumbers::SRO_ATTACK_EXTRA_DAMAGE_MULTIPLE + i );
					if( r < multipleExtraDamageData.m_fRate )
					{
						Process_ExtraDamage( pDamageData, multipleExtraDamageData
                        , bAttackedByMe
                            );
					}
				}
//}} 오현빈 // 2012-06-28 // AT_EFFECT 공격에 의도하지 않은 공격자의 익스트라데미지가 적용되는 문제 수정
			}
		}
	}

#if !X2_DEV_TOOLS
#endif
}


//{{ oasis907 : 김상윤 [2010.3.22] // 소켓 아이템 ExtraDamage 레벨 보정
void CX2GUNPC::LevelCorrectionExtraDamage( const CX2DamageManager::DamageData* pDamageData, CX2DamageManager::ExtraDamageData* pExtraDamageData )
{
	int iLevel = pExtraDamageData->m_iLevel;
	if( iLevel == 0 )
	{
		if( pExtraDamageData->m_bDefenderLevel == false && null != pDamageData->optrAttackerGameUnit )
			iLevel = pDamageData->optrAttackerGameUnit->GetUnitLevel();
		else if( pExtraDamageData->m_bDefenderLevel == true && null != pDamageData->optrDefenderGameUnit )
			iLevel = pDamageData->optrDefenderGameUnit->GetUnitLevel();
	}

	if( g_pData != NULL && g_pData->GetDungeonRoom() != NULL && g_pData->GetPartyManager() != NULL && g_pData->GetPartyManager()->GetMyPartyData() != NULL )
	{
		switch(g_pData->GetDungeonRoom()->GetDungeonID())
		{
		case SEnum::DI_ELDER_HENIR_SPACE:
// 		case SEnum::DI_BESMA_HENIR_SPACE:
// 		case SEnum::DI_ALTERA_HENIR_SPACE:
// 		case SEnum::DI_FEITA_HENIR_SPACE:
// 			// kimhc // 벨더 헤니르 // 2009-10-27
// 		case SEnum::DI_VELDER_HENIR_SPACE:
// 		case SEnum::DI_HAMEL_HENIR_SPACE:
			{
				if( (CX2Dungeon::DUNGEON_MODE) g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode == CX2Dungeon::DM_HENIR_CHALLENGE )
				{
					iLevel = 1;
				}
			} break;

		case SEnum::DI_EVENT_TREE_DAY_ELDER:
		case SEnum::DI_EVENT_TREE_DAY_BESMA:
		case SEnum::DI_EVENT_TREE_DAY_ALTERA:
		case SEnum::DI_EVENT_TREE_DAY_PEITA:
		case SEnum::DI_EVENT_TREE_DAY_VELDER:
		case SEnum::DI_EVENT_TREE_DAY_HAMEL:
//{{ oasis907 : 김상윤 [2010.10.21] //
		case SEnum::DI_ELDER_HALLOWEEN_NORMAL:
		case SEnum::DI_ELDER_HALLOWEEN_HARD:
		case SEnum::DI_ELDER_HALLOWEEN_EXPERT:
//}} oasis907 : 김상윤 [2010.10.21] //
		case SEnum::DI_EVENT_VALENTINE_DAY:
			//{{ 허상형 : [2011/3/3/] //	월드 미션
		CASE_DEFENCE_DUNGEON
			//}} 허상형 : [2011/3/3/] //	월드 미션
			iLevel = 1;
			break;
		}
	}

	if( iLevel >= 0 )
	{
		CX2EnchantItem::ENCHANT_TYPE eEnchantType = GetEnchantTypeFromExtraDamageType( pExtraDamageData->m_ExtraDamageType );
		float fResist = GetEnchantResist(eEnchantType); // 피격자 속성 저항치

		pExtraDamageData->m_DamagePerSec = pExtraDamageData->GetCalcPerDamage(iLevel, fResist);
		float fRate = pExtraDamageData->m_fRate;
		float fPureRate = pExtraDamageData->m_fPureRate;

		if( pExtraDamageData->m_bIgnoreResist == true )
		{
			fRate = 1.f;
		}
		else if(pExtraDamageData->m_iLevel >= 0)
		{
			// ( ( (공격자 레벨 - 피격자 레벨) * 0.025f ) + 0.5f ) * (1.f - (피격자 속성저항치 / 500.f))
			int attackerLv = pDamageData->optrAttackerGameUnit->GetUnitLevel();
			int defencerLv = 0;

			ASSERT( null != pDamageData->optrDefenderGameUnit );
			if ( null != pDamageData->optrDefenderGameUnit )
				defencerLv = pDamageData->optrDefenderGameUnit->GetUnitLevel();

			fRate = ( ( (attackerLv - defencerLv) * 0.025f ) + 0.5f ) * (1.f - (fResist / CX2EnchantItem::EAR_MAX_VALUE));
			if(fPureRate >= 1.f) // oasis907 : 김상윤 [2011.5.25] 스크립트 파싱시 디폴트 fRate는 1.f,
			{					// 외부에서 Rate설정을 따로해주지 않으면 최대 Rate를 0.5로 한정
				if( fRate > 0.5f )
					fRate = 0.5f;
			}
			else
			{
				if( fRate > fPureRate )
					fRate = fPureRate;
			}
			//{{ 엑스트라데미지 확률이 음수일 때 0으로 처리 (ASSERT 제거)
			if( fRate < 0.f )
				fRate = 0.f;
			//}} 엑스트라데미지 확률이 음수일 때 0으로 처리 (ASSERT 제거)
		}

		pExtraDamageData->m_fRate = fRate;
	}

	// oasis907 : 김상윤 [2010.10.5] // 소켓 옵션 발동 확률 조정
	if( pExtraDamageData->m_bIgnoreResist == false )
	{
		{
			float fRateModifier = 1.f;
			if( iLevel >= 0 )
			{
				fRateModifier = pExtraDamageData->m_fRate;
			}
			else
			{
				fRateModifier = pExtraDamageData->m_fPureRate;
			}
			pExtraDamageData->m_fRate = fRateModifier * pDamageData->m_fRateModifier;
		}
	}

	return;
}
//}} oasis907 : 김상윤 [2010.3.22] //

//{{ oasis907 : 김상윤 [2010.4.19] //
void CX2GUNPC::Process_ReflectDamage(CX2DamageManager::DamageData* pDamageData, CX2GameUnit* pAttackerUnit)
{
	bool bConditionCheck = false;
	if( m_ConditionReflectDamage.m_fMyHPLessThanPercentFactor >  100*GetNowHp()/GetMaxHp() )
	{
		bConditionCheck = true;
	}
	else if(m_ConditionReflectDamage.m_bOnPhysicalDamage)
	{
		if(pDamageData->damageType ==  CX2DamageManager::DT_PHYSIC)
		{
			bConditionCheck = true;
		}
	}
	else if(m_ConditionReflectDamage.m_bOnMagicalDamage)
	{
		if(pDamageData->damageType ==  CX2DamageManager::DT_MAGIC)
		{
			bConditionCheck = true;
		}
	}

	if(bConditionCheck)
	{
		if(pAttackerUnit != NULL &&
			pAttackerUnit->GetInvincible() == false &&
			pAttackerUnit->GetNowHp() > 0.f &&
			GetInvincible() == false &&
			GetNowHp() > 0.f &&
			m_ReflectDamageData.reActType != CX2DamageManager::RT_NO_DAMAGE &&
			pDamageData->reActType != CX2DamageManager::RT_NO_DAMAGE &&
			pDamageData->attackType != CX2DamageManager::AT_SPECIAL)
		{
			m_ReflectDamageData.impactPoint	= pAttackerUnit->GetPos();
			m_ReflectDamageData.optrDefenderGameUnit = pAttackerUnit;
			m_ReflectDamageData.reActResult	= m_ReflectDamageData.reActType;
			pAttackerUnit->DamageReact(&m_ReflectDamageData);

		}
	}
	return;
}

void CX2GUNPC::SetReflectDamageOnPhysicalDamage( bool bVal )
{
	m_ConditionReflectDamage.m_bOnPhysicalDamage = bVal;
	return;
}
void CX2GUNPC::SetReflectDamageOnMagicalDamage( bool bVal )
{
	m_ConditionReflectDamage.m_bOnMagicalDamage = bVal;
	return;
}

//}} oasis907 : 김상윤 [2010.4.19] //



// @pDamageData : 기타 필요한 정보를 얻어 오기 위한
// @extraDamageData : 실제 처리해야할 extra damage
void CX2GUNPC::Process_ExtraDamage( const CX2DamageManager::DamageData* pDamageData, const CX2DamageManager::ExtraDamageData &extraDamageData
            , bool bAttackedByMe
    )
{
	KTDXPROFILE();

	ASSERT( pDamageData != NULL );

	switch( extraDamageData.m_ExtraDamageType )
	{
	case CX2DamageManager::EDT_POISON:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Poison = extraDamageData;
				if(extraDamageData.m_iLevel < 0.f)
				{
					float fDuration = extraDamageData.m_fTime;
					ApplyEnchantResist( CX2EnchantItem::ET_NATURE, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
					m_ExtraDamagePack.m_Poison.m_fTime = fDuration;
				}
				if( extraDamageData.m_fRunJumpRate > 0.f && m_ExtraDamagePack.m_Poison.m_fTime > 0.f )
				{
					SetMoveSpeedFactor(extraDamageData.m_fRunJumpRate, m_ExtraDamagePack.m_Poison.m_fTime);
					SetJumpSpeedFactor(extraDamageData.m_fRunJumpRate, m_ExtraDamagePack.m_Poison.m_fTime);

					SetPoisonDeBuff( m_ExtraDamagePack.m_Poison.m_fTime );
				}
			}
		} break;

	case CX2DamageManager::EDT_SLOW:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Slow = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Slow.m_fTime = fDuration;
			}
		} break;

	case CX2DamageManager::EDT_FIRE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				bool bNewExtra = true;
				if( m_ExtraDamagePack.m_Fire.m_fTime > 0.f )
				{
					if( m_ExtraDamagePack.m_Fire.m_DamagePerSec > extraDamageData.m_DamagePerSec )
					{
						bNewExtra = false;
					}
					else if( m_ExtraDamagePack.m_Fire.m_DamagePerSec < extraDamageData.m_DamagePerSec )
					{
						bNewExtra = true;
					}
					else
					{
						if( m_ExtraDamagePack.m_Fire.m_fTime >  extraDamageData.m_fTime )
						{
							bNewExtra = false;
						}
						else
						{
							bNewExtra = true;
						}
					}
				}

				if( bNewExtra == true )
				{
					m_ExtraDamagePack.m_Fire = extraDamageData;
					if(extraDamageData.m_iLevel < 0.f)
					{
						float fDuration = extraDamageData.m_fTime;
						ApplyEnchantResist( CX2EnchantItem::ET_BLAZE, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
						m_ExtraDamagePack.m_Fire.m_fTime = fDuration;
					}
				}
			}
		} break;

	case CX2DamageManager::EDT_CURSE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Curse = extraDamageData;

				float fDuration = extraDamageData.m_fTime;
				ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Curse.m_fTime = fDuration;
			}
		} break;

	case CX2DamageManager::EDT_LIGHTNING:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Lightning = extraDamageData;

				float fDuration = extraDamageData.m_fTime;
				ApplyEnchantResist( CX2EnchantItem::ET_WIND, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Lightning.m_fTime = fDuration;
			}
		} break;

	case CX2DamageManager::EDT_COLD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Cold = extraDamageData;

				float fDuration = extraDamageData.m_fTime;
				ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Cold.m_fTime = fDuration;
			}
		} break;

	case CX2DamageManager::EDT_MANA_DAMAGE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				//{{ JHKang / 강정훈 / 2010/11/05 / 루나 블레이드, 현재 마나의 비율로 감소 추가
				if ( extraDamageData.m_fDamageRate > 0.f && extraDamageData.m_Damage < 0.f )
				{
					SetNowMp( GetNowMp() * extraDamageData.m_fDamageRate );

					if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
					{
						std::wstringstream strStream;
						strStream << L"MP -" << (1.f - extraDamageData.m_fDamageRate) * 100.f << L"%";
						D3DXVECTOR3 pos = GetPos();
						pos.y += 50.0f + (0 * 30.0f);
						if( NULL != g_pData->GetPicCharRed() )
							g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

					}

					m_bHitedDrainMana = false;
				}
				else
				{
					UpNowMp(-1 * extraDamageData.m_Damage);

					if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
					{
						std::wstringstream strStream;
						strStream << L"MP -" << (int)extraDamageData.m_Damage;
						D3DXVECTOR3 pos = GetPos();
						pos.y += 50.0f + (0 * 30.0f);
						if( NULL != g_pData->GetPicCharRed() )
							g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
					}

					m_bHitedDrainMana = true;
				}
				//}} JHKang / 강정훈 / 2010/11/05 / 루나 블레이드, 현재 마나의 비율로 감소 추가
			}
		} break;

	case CX2DamageManager::EDT_FIRE_SMASH:
		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Fire_Smash_Smoke_FireWave04_Nasod_Element_Fire", pDamageData->impactPoint );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Fire_Smash_Flare_FireWave05_Nasod_Element_Fire", pDamageData->impactPoint );

			UpDownCrashCamera( 20.0f, 0.5f );
		} break;

	case CX2DamageManager::EDT_COLD_SMASH:
		{
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Cold_Smash_Smoke_IceWave04_Nasod_Element_Ice", pDamageData->impactPoint );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Cold_Smash_Flare_IceWave05_Nasod_Element_Ice", pDamageData->impactPoint );

			UpDownCrashCamera( 20.0f, 0.5f );
		} break;


	case CX2DamageManager::EDT_EVENT_SMASH:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_NPCFrameData.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_CoupleHell", vPos );

			UpDownCrashCamera( 30.0f, 0.5f );
		} break;
	case CX2DamageManager::EDT_CAMILLA_SMASH:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_NPCFrameData.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_FightKing", vPos );

			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 30.0f, 0.5f );
		} break;
	case CX2DamageManager::EDT_EVENT_SMASH_3:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_NPCFrameData.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence(  this,  L"STR_EventSmash3", vPos );

			g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 30.0f, 0.5f );
		} break;
	case CX2DamageManager::EDT_EVENT_SMASH_2:
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += m_NPCFrameData.unitCondition.fUnitHeight + 120.f;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_EventSmash2", vPos );

			UpDownCrashCamera( 30.0f, 0.5f );
		} break;


	case CX2DamageManager::EDT_POISON_CLOUD:
		{
			const float MAGIC_POWER_RATE = 1.f;
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pDamageData->optrAttackerGameUnit.GetObservable(), L"EXTRA_DAMAGE_POISON_CLOUD", MAGIC_POWER_RATE,
				pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition_LUA().y );

			UpDownCrashCamera( 20.0f, 0.5f );
		} break;

	case CX2DamageManager::EDT_DOUBLE_ATTACK:
		{
			if( pDamageData->attackType != CX2DamageManager::AT_SPECIAL )
            {
                m_eDoubleAttack = ( bAttackedByMe == true ) ? DOUBLE_ATTACK_BY_ME : DOUBLE_ATTACK_BY_OTHER;
            }
			//g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 20.0f, 0.5f );
		} break;



	case CX2DamageManager::EDT_FROZEN:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Frozen = extraDamageData;

				float fDuration = extraDamageData.m_fTime;
				ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Frozen.m_fTime = fDuration;

				if( fDuration > 0.f )
				{
					AnimStop();
				}

				if( m_bDoneBackupSpeed == false )
				{
					m_bDoneBackupSpeed = true;
					m_vNowSpeedBackup = m_PhysicParam.nowSpeed;
					m_vpassiveSpeedBackup = m_PhysicParam.passiveSpeed;
				}
				m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
				m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );


				UpDownCrashCamera( 20.0f, 0.5f );
			}

		} break;

	case CX2DamageManager::EDT_ENTANGLE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Entangled = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				//ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_Entangled.m_fTime = fDuration;

			}
		} break;

	case CX2DamageManager::EDT_TIME_BOMB:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_TimeBomb = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				//ApplyEnchantResist( CX2EnchantItem::ET_DARK, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_TimeBomb.m_fTime = fDuration;
			}
		} break;


	case CX2DamageManager::EDT_DAMAGE_RATE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				float fNowHp = GetNowHp();
				float fDamage = fNowHp * extraDamageData.m_Damage;

                UpNowHp( -fDamage );

				*m_fDamageHistory += fDamage;

				if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || bAttackedByMe == true )
				{
					std::wstringstream strStream;
					strStream << L"HP -" << (int)fDamage;
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f + (0 * 30.0f);
					//{{ JHKang / 강정훈 / 2010/12/06 / 아토믹 쉴드가 받은 데미지 숫자 표시 제한
					if( NULL != g_pData->GetPicCharRed() && true == m_bDamageOutScreen )
						//}} JHKang / 강정훈 / 2010/12/06 / 아토믹 쉴드가 받은 데미지 숫자 표시 제한
						g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		} break;

	case CX2DamageManager::EDT_STIGMA_DEBUFF:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_StigmaDebuff = extraDamageData;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Rena_Stigma_NPC", this );
			}
		} break;

	case CX2DamageManager::EDT_LIGHTNING_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"LIGHTNING_SMASH", pAttackerUnit->GetPowerRate(),
					GetLandPos(), GetRotateDegree(), GetRotateDegree() );
			}
		} break;

		//{{ oasis907 : 김상윤 [2010.11.8] // 로드 나이트 - 샌드 스톰 - EDT_BLIND
	case CX2DamageManager::EDT_BLIND:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Blind = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				m_ExtraDamagePack.m_Blind.m_fTime = fDuration;
				float fEffectiveRate = extraDamageData.m_fDamageRate;
				m_ExtraDamagePack.m_Blind.m_fDamageRate = fEffectiveRate;
			}
		} break;
		//}} oasis907 : 김상윤 [2010.11.8] // 로드 나이트 - 샌드 스톰 - EDT_BLIND

	//{{ kimhc // 2010.12.29 // 하멜 마을 추가시 들어가는 ExtraDamage
 	case CX2DamageManager::EDT_ICE_STING:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_ICE_STING", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;

	case CX2DamageManager::EDT_FIRE_BALL:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_FIRE_BALL", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;
//}} kimhc // 2010.12.29 // 하멜 마을 추가시 들어가는 ExtraDamage


		//{{ lucidash : 김상훈 [2011.1.4] // 시즌3 몬스터 라구즈 마법 공격
	case CX2DamageManager::EDT_WATER_HOLD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_WaterHold = extraDamageData;

				float fDuration = extraDamageData.m_fTime;
				//ApplyEnchantResist( CX2EnchantItem::ET_WATER, fDuration );	// 속성 저항이 있으면 지속시간을 줄여준다
				m_ExtraDamagePack.m_WaterHold.m_fTime = fDuration;

				if( fDuration > 0.f )
				{
					AnimStop();
				}

				if( m_bDoneBackupSpeed == false )
				{
					m_bDoneBackupSpeed = true;
					m_vNowSpeedBackup = m_PhysicParam.nowSpeed;
					m_vpassiveSpeedBackup = m_PhysicParam.passiveSpeed;
				}
				m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
				m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );


				UpDownCrashCamera( 20.0f, 0.5f );
			}
		} break;
		//}} lucidash : 김상훈 [2011.1.4] // 시즌3 몬스터 라구즈 마법 공격

		//{{ 2011-04 에 패치될 청 캐시 스킬
	case CX2DamageManager::EDT_PANIC:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_Panic = extraDamageData;
				UpDownCrashCamera( 20.0f, 0.5f );
			}
		} break;

	case CX2DamageManager::EDT_PAIN:
		{
			//if ( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				// Accumulation에 아무것도 없으면 (지금 들어온 extraDamage가 처음 시작된 PAIN 이면)
				if ( 0 == extraDamageData.m_Accumulation )
				{
					// 현재 캐릭터에게 걸린 디버프 갯수를 체크한다.

					vector<KBuffFactor> VecDebuffFactor;
					GetBuffFactorOnlyDebuff(VecDebuffFactor);

					int iNumOfExtraDamage = VecDebuffFactor.size();
					if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.f )
						++iNumOfExtraDamage;
					if ( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f )
						++iNumOfExtraDamage;
					// 이전에 걸려있던 Pain이 없거나, 있어도 현재 새로 걸릴 Pain보다 전염횟수가 적으면 적용
					if ( iNumOfExtraDamage > m_ExtraDamagePack.m_Pain.m_Accumulation )
					{
						m_ExtraDamagePack.m_Pain = extraDamageData;
						m_ExtraDamagePack.m_Pain.m_Accumulation = iNumOfExtraDamage;
						m_ExtraDamagePack.m_Pain.m_optrGameUnit	= pDamageData->optrAttackerGameUnit;
					}
				}
				// Accumulation이 0이 아니면 (다른 유저로 부터 전염 된 것이면)
				else
				{
					// 기존에 EDT_PAIN이 걸려 있었다면, 전염 횟수가 더 많이 남은 것을 선택 함
					if ( extraDamageData.m_Accumulation > m_ExtraDamagePack.m_Pain.m_Accumulation)
					{
						m_ExtraDamagePack.m_Pain = extraDamageData;
						m_ExtraDamagePack.m_Pain.m_optrGameUnit	= pDamageData->optrAttackerGameUnit;
					}
				}
			}
		} break;

	case CX2DamageManager::EDT_HEAD_SHOT:
		{
			SetHittedExtraDamageHeadShot( true );
		} break;
		//}} 2011-04 에 패치될 청 캐시 스킬

	case CX2DamageManager::EDT_BIGHEAD:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				float fDuration = extraDamageData.m_fTime;

				m_ExtraDamagePack.m_Aging = extraDamageData;
				m_ExtraDamagePack.m_Aging.m_fTime = fDuration;

				// 공격력 감소
// #ifdef SKILL_CASH_10_TEST
// 				SetAttackBuffRate( extraDamageData.m_fDamageRate );
// 				SetAttackBuffTime( fDuration );
// #endif

				m_fExtraBigHead = fDuration;
			}

		} break;

		// oasis907 : 김상윤 [2011.6.16] 청 2차 전직 - 아이언 팔라딘
	case CX2DamageManager::EDT_TRANSIT_DEBUFF:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				pAttackerUnit->ProcessTransitDebuff( pDamageData, this );
			}
			//bGetExtraDamage = true; extradamage아님
		} break;

	case CX2DamageManager::EDT_WIND_STORM:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_WIND_STORM", pAttackerUnit, NULL, false,
					-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, pDamageData->impactPoint,
					pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
			} // if
		} break;

	case CX2DamageManager::EDT_EARTH_QUAKE:
		{
			//if( it_immune == m_mapImmunity.end() || it_immune->second == false )
			{
				m_ExtraDamagePack.m_EarthQuake = extraDamageData;
				float fDuration = extraDamageData.m_fTime;
				m_ExtraDamagePack.m_EarthQuake.m_fTime = fDuration;

				SetMoveSpeedFactor(0.5f, m_ExtraDamagePack.m_EarthQuake.m_fTime);
				SetAnimSpeedFactor(0.75f, m_ExtraDamagePack.m_EarthQuake.m_fTime);
			}
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 outPos( 0.0f, 0.0f, 0.0f );
				int iLineIndex = -1;

				CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
				if( NULL != pLineMap )
				{
					if( true == pLineMap->GetDownNearestWithBound( GetPos(), 100.f, &outPos, &iLineIndex ) )
					{
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_EARTH_QUAKE", pAttackerUnit, NULL, false,
							-1.f, -1.f, pAttackerUnit->GetVec3ScaleByUnit(), true, outPos,
							pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetDirVector() );
					}
				}
			} // if
		} break;

	case CX2DamageManager::EDT_CURSE_CLOUD:
		{
			const float MAGIC_POWER_RATE = 1.f;
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pDamageData->optrAttackerGameUnit.GetObservable(), L"EXTRA_DAMAGE_CURSE_CLOUD", MAGIC_POWER_RATE,
				pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition_LUA().y );

			UpDownCrashCamera( 20.0f, 0.5f );
		} break;

	case CX2DamageManager::EDT_RUNJUMP_SPEED:
		{
			float fDuration = extraDamageData.m_fTime;

			SetVecMoveSpeedFactor(extraDamageData.m_fRunJumpRate, fDuration, CX2GameUnit::SFI_SPEED_9, CX2GameUnit::SFST_UPDATE);
			SetVecJumpSpeedFactor(extraDamageData.m_fRunJumpRate, fDuration, CX2GameUnit::SFI_SPEED_9, CX2GameUnit::SFST_UPDATE);
		}
		break;

	case CX2DamageManager::EDT_FLASH_BANG:
		{
			m_ExtraDamagePack.m_FlashBang = extraDamageData;
			float fDuration = extraDamageData.m_fTime;
			m_ExtraDamagePack.m_FlashBang.m_fTime = fDuration;

            if ( CX2DamageEffect::CEffect*	pAttackerEffect = g_pX2Game->GetDamageEffect()->GetInstance( pDamageData->hAttackerEffect ) )
			{
				const float CONST_FLASH_BANG_HIGH = 400.f;
				const float CONST_FLASH_BANG_LOW = 800.f;

				D3DXVECTOR3 vFlashCenterPos = pAttackerEffect->GetPos();
				float fDistanceFromCenter3Sq = D3DXVec3LengthSq( &(vFlashCenterPos - GetPos()) );

				if ( fDistanceFromCenter3Sq > CONST_FLASH_BANG_LOW * CONST_FLASH_BANG_LOW )
					fDuration *= 0.25f;
				else if( fDistanceFromCenter3Sq > CONST_FLASH_BANG_HIGH * CONST_FLASH_BANG_LOW )
					fDuration *= 0.5f;
				else
					fDuration *= 1.f;


				m_ExtraDamagePack.m_FlashBang.m_fTime = fDuration;
			}
		} break;

	case CX2DamageManager::EDT_MARK_OF_COMMANDER:
		{
			m_ExtraDamagePack.m_MarkOfCommander = extraDamageData;
			float fDuration = extraDamageData.m_fTime;
			m_ExtraDamagePack.m_MarkOfCommander.m_fTime = fDuration;
			m_ExtraDamagePack.m_MarkOfCommander.m_optrGameUnit	= pDamageData->optrAttackerGameUnit.GetObservable();
			m_ExtraDamagePack.m_MarkOfCommander.m_fElapsedTimeToEffectDamage = 1.5f;
		} break;

	case CX2DamageManager::EDT_FLAME_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FLAME_SMASH", pAttackerUnit->GetPowerRate(),
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;

	case CX2DamageManager::EDT_FIRE_WRAITH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				if ( true == pAttackerUnit->GetIsRight() )
				{
					g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FIRE_WRAITH", pAttackerUnit->GetPowerRate(),
						pDamageData->impactPoint, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree() );
				}
				else
				{
					g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_FIRE_WRAITH_INVERSE", pAttackerUnit->GetPowerRate(),
						pDamageData->impactPoint, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree() );
				}
			}
		} break;

	case CX2DamageManager::EDT_START_OF_DELAYED_FIRING:
		{
			float	fDuration	= extraDamageData.m_fTime;		/// 유지 시간
			int		iArrowType	= extraDamageData.m_iLevel;		/// 화살 타입( 0 = 폭발의 화살, 1 = 지연의 신호탄 )

			CX2DamageManager::StartOfDelayedFiringData* pData = NULL;

			if( iArrowType == 0 )	/// 폭발의 화살
				pData = new CX2DamageManager::StartOfDelayedFiringData( CX2DamageEffect::DET_ARROW_OF_EXPLOSION_DELAY,
					pDamageData->optrAttackerGameUnit->GetUnitIndex(), 100, -1, fDuration );
			else					/// 지연의 신호탄
				pData = new CX2DamageManager::StartOfDelayedFiringData( CX2DamageEffect::DET_START_OF_DELAYED_FIRING,
					pDamageData->optrAttackerGameUnit->GetUnitIndex(), -1, -1, fDuration );

			if( true == GetStartOfDelayedFiringData().empty() )		/// 지연의 신호탄 EDT 적용중 확인 이펙트
				m_hActiveStartOfDelayedFiringEDT = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Rena_Night_Watcher_ExplosionArrow_Active_EDT", (CX2GameUnit*) this );

			if( NULL != pData )		/// 지연의 신호탄 추가
				AccessStartOfDelayedFiringData().push_back( pData );

			if( iArrowType == 0 )	/// 폭발의 화살일 때만 화살 갯수 연산
			{
				if( null != pDamageData->optrAttackerGameUnit &&
					CX2GameUnit::GUT_USER == pDamageData->optrAttackerGameUnit->GetGameUnitType() )
				{
					CX2GUUser* pAttacker = static_cast<CX2GUUser*>( pDamageData->optrAttackerGameUnit.GetObservable() );

					if( NULL != pAttacker )
						pAttacker->UpdateStartOfDelayedFiring( true );
				}
			}

			SetStartOfDelayedFiringEmoticon();		/// 이모티콘 갱신
		}break;


	case CX2DamageManager::EDT_STARLIGHT_SMASH:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_STARLIGHT_SMASH", pAttackerUnit->GetPowerRate(),
					pDamageData->impactPoint, GetRotateDegree(), GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;
	case CX2DamageManager::EDT_FALLING_ROCK:/// 샌더 셋트 효과 - 바위 소환
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				vPos.y += 300.f;

				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EQUIP_FALLING_ROCK", pAttackerUnit->GetPowerRate(),
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;

	case CX2DamageManager::EDT_SANDSTORM:/// 샌더 셋트 효과 - 모래돌풍 소환
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				vPos.y = GetLandPosition_LUA().y;

				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EQUIP_SANDSTORM", pAttackerUnit->GetPowerRate(),
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;
	case CX2DamageManager::EDT_WIND_VIBRATION:/// 샌더 칭호 진동 타격
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_EDT_WIND_VIBRATION", pAttackerUnit->GetPowerRate(),
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;

	case CX2DamageManager::EDT_BREEZE_ACCEL:///샌더 칭호 산들바람의 기운 생성
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pAttackerUnit->GetBonePos(L"Bip01");
				g_pX2Game->GetDamageEffect()->CreateInstance( pAttackerUnit, L"DAMAGE_EFFECT_BUFF_BREEZE_ACCEL", pAttackerUnit->GetPowerRate(),
					vPos, pAttackerUnit->GetRotateDegree(), pAttackerUnit->GetRotateDegree(), GetLandPosition_LUA().y );
			}
		} break;
	case CX2DamageManager::EDT_EVIL_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_EVIL_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;
	case CX2DamageManager::EDT_SPIRIT_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pDamageData->impactPoint;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_SPIRIT_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;
	case CX2DamageManager::EDT_GHOST_SWORD:
		{
			CX2GameUnit* pAttackerUnit = pDamageData->optrAttackerGameUnit.GetObservable();
			if ( NULL != pAttackerUnit )
			{
				D3DXVECTOR3 vPos = pAttackerUnit->GetBonePos(L"Bip01");
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EDT_GHOST_SWORD", pAttackerUnit, NULL, false, -1.0f, -1.0f,
					D3DXVECTOR3(1,1,1), true, vPos, pAttackerUnit->GetRotateDegree() );
			}
		} break;

	case CX2DamageManager::EDT_ACTIVE_DEBUFF:	/// 타격시 특정 디버프를 적용
		{
			CX2GameUnit* pDefenderUnit = pDamageData->optrDefenderGameUnit.GetObservable();

			if ( NULL != pDefenderUnit && NULL != extraDamageData.m_ptrBuffFactor )
			{
				CX2BuffFactorPtr pCloneBuffFactorPtr =  extraDamageData.m_ptrBuffFactor->GetClonePtr( 0 );

				if ( NULL != pCloneBuffFactorPtr )
				{
					pCloneBuffFactorPtr->SetGameUnitBuffFactor( this );
					pDefenderUnit->SetBuffFactorToGameUnit( *pCloneBuffFactorPtr );
				}
			}
		} break;

	default:
		{
			ASSERT( !"EDT_NONE extra damage" );
		} break;
	}
}


//{{ megagame : 박교현 : [2010-04-06]

void CX2GUNPC::DamageReact_HitCount( CX2DamageManager::DamageData* pDamageData )
{
	if( IsSuperArmor() )
		return;

	if( pDamageData->reActType == CX2DamageManager::RT_NO_DAMAGE )
		return;

	++m_HitCount;
}

int	CX2GUNPC::GetStateID_LUA( const char* stateName )
{
	if ( stateName == NULL )
		return GUSI_NONE;

	//wstring wstrStateName = L"";
	//ConvertUtf8ToWCHAR( wstrStateName, stateName );
	return GetStateID( stateName );
}

int CX2GUNPC::GetNextStateID_LUA() const
{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	if( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum == m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if(!m_NPCFrameData.syncData.bStateChange)
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


		return GUSI_NONE;

	return m_NPCFrameData.unitCondition.nextState;
}

//{{ kimhc // 2010-04-16 // 공격력, 방어력 등 핸들링 할 수 있도록 함
void CX2GUNPC::ReInitAtkAndDef_LUA()
{
	//const CX2UnitManager::NPCUnitStat* pNPCUnitStat = g_pData->GetUnitManager()->GetNPCUnitStat( GetNPCTemplet().m_nNPCUnitID );
    const CX2UnitManager::NPCUnitStat& kNPCUnitStat = GetNPCUnitStat();
	//if( pNPCUnitStat != NULL )
	{
		SetNPCStat( GetNowHp(),
			kNPCUnitStat.m_fAtkPhysic + (kNPCUnitStat.m_HardLevel.m_fAtkPhysic * m_HardLevel),
			kNPCUnitStat.m_fAtkMagic + (kNPCUnitStat.m_HardLevel.m_fAtkMagic * m_HardLevel),
			kNPCUnitStat.m_fDefPhysic + (kNPCUnitStat.m_HardLevel.m_fDefPhysic * m_HardLevel),
			kNPCUnitStat.m_fDefMagic + (kNPCUnitStat.m_HardLevel.m_fDefMagic * m_HardLevel)
			, false );
	}
}
//}} kimhc // 2010-04-16 // 공격력, 방어력 등 핸들링 할 수 있도록 함
//}} megagame : 박교현 : [2010-04-06]


void CX2GUNPC::DoStateRage()
{

	if( m_RageState != m_CommonState.m_Wait )
	{
		m_NPCFrameData.stateParam.bSuperArmor = true;
		StateChange( m_RageState );
	}
	else
	{
		StateChange( m_CommonState.m_Wait );
	}


	D3DXVECTOR3 pos = m_NPCFrameData.syncData.position;
	float fRadious = GetXSkinAnimPtr()->GetAnimXSkinMesh()->GetBoundingRadius();
	pos.y += (fRadious*0.5f);
	g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MonsterAngryWind", pos);
	//g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MonsterAngryWind2", pos);
	g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Counter_Particle01", pos );


	if( m_RageState == m_CommonState.m_Wait )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MONSTER_ANGRY_WIND", GetPowerRate(), pos,
			GetRotateDegree(), GetRotateDegree(), m_NPCFrameData.unitCondition.landPosition.y );
	}


	SetStop2Time( 0.7f );

	UpDownCrashCamera( 30.0f, 0.4f );

	return;
}


void CX2GUNPC::DamageReact_Rage( CX2DamageManager::DamageData* pDamageData )
{
    const StateData& nowStateData = GetNowStateData();
    if ( NOWSTATE_FLAG( CANNOT_RAGE) == true )
        return;

	//{{ oasis907 : 김상윤 [2010.4.21] //
	if( m_iRageHPPercent != 0)
	{
		if(GetNowHp() <= 0.f)
		{
			return;
		}
		if( pDamageData->reActType == CX2DamageManager::RT_NO_DAMAGE )
			return;


		if(m_fRageTime > 0.f)
			return;


		float fDiffHPThreshold = GetMaxHp() * (float)m_iRageHPPercent / 100;

		float fNowHPThreshold = GetMaxHp() - fDiffHPThreshold * (m_iHPRageCount + 1);


		if( GetNowHp() < fNowHPThreshold)
		{
			while(GetNowHp() < fNowHPThreshold)
			{
				m_iHPRageCount++;
				fNowHPThreshold = GetMaxHp() - fDiffHPThreshold * (m_iHPRageCount + 1);
			}

			m_fRageTime = m_fRageTimeMax;

			DoStateRage();
			pDamageData->reActResult = CX2DamageManager::RT_DUMMY_DAMAGE;
			return;

		}

	}
//}} oasis907 : 김상윤 [2010.4.21] //



	if( IsSuperArmor() )
		return;

	if( m_RageCountMax <= 0 )
		return;

	if( GetNowHp() <= 0.f )
	{
		m_fRageTime = 0.f;
		m_RageCount = 0;
		return;
	}

	if( pDamageData->reActType == CX2DamageManager::RT_NO_DAMAGE )
		return;

	m_RageCount++;

	if( m_RageCount >= m_RageCountMax )
	{
		m_fRageTime = m_fRageTimeMax;
		m_RageCount = 0;

		// oasis907 : 김상윤 [2010.4.21] //
		DoStateRage();
		pDamageData->reActResult = CX2DamageManager::RT_DUMMY_DAMAGE;

	}

}

bool CX2GUNPC::DamageReactStateChange( CX2DamageManager::DamageData* pDamageData, int iAttackerUnitIndex )
{
    bool    bStateChanged = false;
    bool    bTmpStateChanged = false;
    bool bRelaxReactionCheck = pDamageData->m_bRelaxNPCReactionStateCheck;

	if ( m_RidingNPCUID != 0 )
    {
        return false;
    }

	if( m_DefenceRate > 0 )
	{
        m_kNonHostReaction.SetDefenceRandomTableIndex( m_kNonHostReaction.GetShuffledDefenceRandomTableIndex() );
        if ( ( CKTDXRandomNumbers::GetRandomInt( m_kNonHostReaction.GetDefenceRandomTableIndex() ) % 100 ) < m_DefenceRate )
		{
			if( pDamageData->reActResult					!= CX2DamageManager::RT_NO_DAMAGE
                && pDamageData->reActResult					!= CX2DamageManager::RT_SENSOR
				&& pDamageData->reActResult					!= CX2DamageManager::RT_REVENGE
				&& IsSuperArmor()	== false
				&& m_NPCFrameData.stateParam.bInvincible	== false
				&& m_NPCFrameData.syncData.nowState			!= m_SmallDamageLandFront
				&& m_NPCFrameData.syncData.nowState			!= m_SmallDamageLandBack
				&& m_NPCFrameData.syncData.nowState			!= m_SmallDamageAir
				&& m_NPCFrameData.syncData.nowState			!= m_SmallDamageAirFront
				&& m_NPCFrameData.syncData.nowState			!= m_SmallDamageAirBack
				&& m_NPCFrameData.syncData.nowState			!= m_BigDamageLandFront
				&& m_NPCFrameData.syncData.nowState			!= m_BigDamageLandBack
				&& m_NPCFrameData.syncData.nowState			!= m_BigDamageAir
				&& m_NPCFrameData.syncData.nowState			!= m_BigDamageAirFront
				&& m_NPCFrameData.syncData.nowState			!= m_BigDamageAirBack
				&& m_NPCFrameData.syncData.nowState			!= m_CommonState.m_DownDamageLandFront
				&& m_NPCFrameData.syncData.nowState			!= m_CommonState.m_DownDamageLandBack
				&& m_NPCFrameData.syncData.nowState			!= m_CommonState.m_DownDamageAir
				&& m_NPCFrameData.syncData.nowState			!= m_UpDamage
				&& m_NPCFrameData.syncData.nowState			!= m_FlyDamageFront
				&& m_NPCFrameData.syncData.nowState			!= m_FlyDamageBack
				&& m_NPCFrameData.syncData.nowState			!= m_RevengeAttack
				&& m_NPCFrameData.syncData.nowState			!= m_DamageFlushLandFront
				&& m_NPCFrameData.syncData.nowState			!= m_DamageFlushLandBack
				&& m_NPCFrameData.syncData.nowState			!= m_DamageFlushAir
				&& m_NPCFrameData.syncData.nowState			!= m_DamageRevenge
				&& m_NPCFrameData.syncData.nowState			!= m_DefenceState
				&& m_NPCFrameData.syncData.nowState			!= m_DyingLandFront
				&& m_NPCFrameData.syncData.nowState			!= m_DyingLandBack
				&& m_NPCFrameData.syncData.nowState			!= m_DyingSky
                && m_NPCFrameData.syncData.nowState			!= m_DyingFly
                && m_NPCFrameData.syncData.nowState			!= m_SmallDamageLandGuard
                && m_NPCFrameData.syncData.nowState			!= m_BigDamageLandGuard
                && m_NPCFrameData.syncData.nowState			!= m_DownDamageLandGuard )
			{
				//디펜스 모드로 이행한다
				pDamageData->reActResult = CX2DamageManager::RT_NO_DAMAGE;
                {
                    bTmpStateChanged = StateChange_Reaction_FIRST( m_DefenceState, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                     bStateChanged = bStateChanged || bTmpStateChanged;
                }
			}
		}
	}

	CX2GameUnit*	pAttackerUnit = NULL;

	if( null != pDamageData->optrAttackerGameUnit )
	{
		pAttackerUnit		= pDamageData->optrAttackerGameUnit.GetObservable();
	}


	// 라이딩 유닛만 여기서, 일반적인 유닛들은 기존처럼 아래에서 검사
	if(m_eRideState ==  CX2GUNPC::RS_ON_RIDING && IsSuperArmor() != true )
	{
		if( m_NPCFrameData.syncData.position.y - m_NPCFrameData.unitCondition.landPosition.y < LINE_RADIUS * 7.0f
			|| (pAttackerUnit != NULL && pDamageData->attackerType == CX2DamageManager::AT_UNIT && pAttackerUnit->GetPos().y - pAttackerUnit->GetUnitCondition().landPosition.y < LINE_RADIUS * 3.0f) )
		{
			if( m_PhysicParam.nowSpeed.y > 0.0f )
				m_PhysicParam.nowSpeed.y = 0.0f;

			if( m_bGuard == true && m_SmallDamageLandGuard != 0 )
			{
                bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			else
			{
				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_RideDamageFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_RideDamageBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
		}
	}
	else
	switch( pDamageData->reActResult )
	{

		case CX2DamageManager::RT_DUMMY_DAMAGE:
		case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
			break;

		case CX2DamageManager::RT_SMALL_DAMAGE:
//{{ kimhc // 2011.1.8 // 청 1차 전직
		case CX2DamageManager::RT_FLIP_SAME_DIR:
//}} kimhc // 2011.1.8 // 청 1차 전직
		case CX2DamageManager::RT_FLIP_DIFF_DIR :

			if( IsSuperArmor() )
				break;

			if( m_NPCFrameData.syncData.position.y - m_NPCFrameData.unitCondition.landPosition.y < LINE_RADIUS * 7.0f
				|| (pAttackerUnit != NULL && pDamageData->attackerType == CX2DamageManager::AT_UNIT && pAttackerUnit->GetPos().y - pAttackerUnit->GetUnitCondition().landPosition.y < LINE_RADIUS * 3.0f) )
			{
				if( m_PhysicParam.nowSpeed.y > 0.0f )
					m_PhysicParam.nowSpeed.y = 0.0f;

                if( m_bGuard == true && m_SmallDamageLandGuard != 0 )
                {
                    bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }
                else
                {
                    if( m_NPCFrameData.unitCondition.bAttackerFront == true )
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageLandFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    else
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageLandBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }

			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

				m_PhysicParam.nowSpeed.y		+= 700.0f;

				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					m_PhysicParam.nowSpeed.x	-= GetWalkSpeed()/3.0f;
				else
					m_PhysicParam.nowSpeed.x	+= GetWalkSpeed()/3.0f;

				if( m_SmallDamageAir != 0 )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageAir, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageAirFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_SmallDamageAirBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			break;

		case CX2DamageManager::RT_BIG_DAMAGE:
			if( IsSuperArmor() )
				break;

			if( m_NPCFrameData.syncData.position.y - m_NPCFrameData.unitCondition.landPosition.y < LINE_RADIUS * 7.0f
				|| (pAttackerUnit != NULL && pDamageData->attackerType == CX2DamageManager::AT_UNIT && pAttackerUnit->GetPos().y - pAttackerUnit->GetUnitCondition().landPosition.y < LINE_RADIUS * 3.0f) )
			{
				if( m_PhysicParam.nowSpeed.y > 0.0f )
					m_PhysicParam.nowSpeed.y = 0.0f;

                if( m_bGuard == true && m_BigDamageLandGuard != 0)
                {
                    bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }
                else
                {
                    if( m_NPCFrameData.unitCondition.bAttackerFront == true )
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageLandFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    else
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageLandBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }

			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

				if( m_BigDamageAir != 0 )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageAir, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageAirFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_BigDamageAirBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			break;

		case CX2DamageManager::RT_DOWN:
			if( IsSuperArmor() )
				break;

			if( IsOnSomething() == true )
			{
				if( m_PhysicParam.nowSpeed.y > 0.0f )
					m_PhysicParam.nowSpeed.y = 0.0f;

                if( m_bGuard && m_BigDamageLandGuard != 0)
                {
                    bTmpStateChanged = StateChange_Reaction_FIRST( m_DownDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }
                else
                {
                    if( m_NPCFrameData.unitCondition.bAttackerFront == true )
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageLandFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    else
                        bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageLandBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
                }
			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
                bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageAir, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}

			break;

		case CX2DamageManager::RT_UP:
			if( IsSuperArmor() )
				break;

            if( m_bGuard == true && m_DownDamageLandGuard != 0)
            {
                bTmpStateChanged = StateChange_Reaction_FIRST( m_DownDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
                break;
            }

			if( m_FlyCount > 2 )
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y / (m_FlyCount - 1);
			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
			}
            bTmpStateChanged = StateChange_Reaction_FIRST( m_UpDamage, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
            bStateChanged = bStateChanged || bTmpStateChanged;
			break;

		case CX2DamageManager::RT_FLY:
			if( IsSuperArmor() )
				break;

            if( m_bGuard == true && m_DownDamageLandGuard != 0)
            {
                bTmpStateChanged = StateChange_Reaction_FIRST( m_DownDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
                break;
            }

			if( m_FlyCount > 2 )
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y / (m_FlyCount - 1);
			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
			}
			if( m_NPCFrameData.unitCondition.bAttackerFront == true )
				bTmpStateChanged = StateChange_Reaction_FIRST( m_FlyDamageFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
			else
				bTmpStateChanged = StateChange_Reaction_FIRST( m_FlyDamageBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
            bStateChanged = bStateChanged || bTmpStateChanged;
			break;

			//{{ kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격
		case CX2DamageManager::RT_DRAG_UP:
			{
				if( IsSuperArmor() )
					break;

				if( m_bGuard == true && m_DownDamageLandGuard != 0)
				{
                    bTmpStateChanged = StateChange_Reaction_FIRST( m_DownDamageLandGuard, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                    bStateChanged = bStateChanged || bTmpStateChanged;
					break;
				}

				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;

				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_FlyDamageFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_FlyDamageBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			break;
			//}} kimhc // 2010.2.8 // 위치보정 없이 위로 계속 끌어 올리는 공격

		case CX2DamageManager::RT_REVENGE:
			    //m_PhysicParam.nowSpeed.x = 0.0f;
			    //m_PhysicParam.nowSpeed.y = 0.0f;


			/// 마법 반사 버프가 적용중 이라면, 피격 모션을 없앤다.
			if ( false == m_vecReflectMagicByBuffPtr.empty() )
			{
				if ( bStateChanged == true )
					break;
				return false;
			}
			else
			{
				if( m_RevengeAttack != 0 )
	            {
	                if ( IsLocalUnit() == false )
	                {
	                    int iStateChange = StateChange_CheckOnly( m_RevengeAttack, true );
	                    if ( iStateChange != 0 )
	                    {
	                        bStateChanged = false;
	                        UnDoStateChange_Reaction();
	                    }
	                }
	                else
	                {
				        m_PhysicParam.nowSpeed.x = 0.0f;
				        m_PhysicParam.nowSpeed.y = 0.0f;
					    bTmpStateChanged = StateChange( m_RevengeAttack, true );
	                    bStateChanged = bStateChanged || bTmpStateChanged;
	                }

	            }

			}

			break;





	}




	// 빰맞고 돌아서기
	switch( pDamageData->reActResult )
	{
	case CX2DamageManager::RT_SMALL_DAMAGE:
	case CX2DamageManager::RT_BIG_DAMAGE:
		{
			if( true == pDamageData->m_bFlipDirOnHit &&
				false == m_bNeverMove &&
				false == IsSuperArmor() &&
				false == GetShowBossName() &&
				false == GetShowSubBossName() )
			{
				//if( NULL != GetNPCTemplet() )
				{
					switch( GetNPCTemplet().m_ClassType )
					{
					case CX2UnitManager::NCT_BASIC:
						{
                            SetIsRightNow( !m_NPCFrameData.syncData.bIsRight );
						} break;
					}
				}
			}
		} break;

		//{{ kimhc // 2011.1.8 // 청 1차 전직
	case CX2DamageManager::RT_FLIP_SAME_DIR:	// 상대방을 유저
		{
			if ( false == m_bNeverMove &&
				 false == IsSuperArmor() &&
				 false == GetShowBossName() &&
				 false == GetShowSubBossName() )
			{
				bool bIsAttackerUnitInRightDirection = false; // 유저의 방향

				if ( null != pDamageData->optrAttackerGameUnit )
				{
					bIsAttackerUnitInRightDirection = pDamageData->optrAttackerGameUnit->GetIsRight();
				} // if

				//if ( NULL != GetNPCTemplet() )
				{
					switch ( GetNPCTemplet().m_ClassType )
					{
					case CX2UnitManager::NCT_BASIC:
						{
							// 유저의 방향과 NPC의 방향이 반대이면
							if ( bIsAttackerUnitInRightDirection != m_NPCFrameData.syncData.bIsRight )
							{
                                SetIsRightNow( !m_NPCFrameData.syncData.bIsRight );
							} // if
						} break;
					} // switch
				} // if


			} // if
		} break;
		//}} kimhc // 2011.1.8 // 청 1차 전직

	case CX2DamageManager::RT_FLIP_DIFF_DIR:	// 상대방을 유저와 다른 방향을 바라보도록 ( 마주 보도록 ) 한다.
	case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
		{
			if ( false == m_bNeverMove &&
				 false == IsSuperArmor() &&
				 false == GetShowBossName() &&
				 false == GetShowSubBossName() )
			{
				bool bIsAttackerUnitInRightDirection = false; // 유저의 방향

				if ( null != pDamageData->optrAttackerGameUnit )
				{
					bIsAttackerUnitInRightDirection = pDamageData->optrAttackerGameUnit->GetIsRight();
				} // if

				//if ( NULL != GetNPCTemplet() )
				{
					switch ( GetNPCTemplet().m_ClassType )
					{
					case CX2UnitManager::NCT_BASIC:
						{
							// 유저의 방향과 NPC의 방향이 같다면
							if ( bIsAttackerUnitInRightDirection == m_NPCFrameData.syncData.bIsRight )
							{
								SetIsRightNow( !m_NPCFrameData.syncData.bIsRight );
							} // if
						} break;
					} // switch
				} // if
			} // if
		} break;


	}




	if( true == IsDamagedReaction( pDamageData ) )
	{

		if( DamageFlushMp() == true )
		{
			if( IsOnSomething() == true )
			{
				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_DamageFlushLandFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_DamageFlushLandBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			else
			{
				m_PhysicParam.nowSpeed.y = pDamageData->backSpeed.y;
                bTmpStateChanged = StateChange_Reaction_FIRST( m_DamageFlushAir, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}

			SetStopTime( 0.7f );

			g_pKTDXApp->GetDGManager()->ClearScreen( 1 );

			CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
			pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpactTick",
				pDamageData->impactPoint,
				200, 200, 2, 20 );

			if( pSeq != NULL )
			{
				pSeq->SetLandPosition( m_NPCFrameData.unitCondition.landPosition.y );
			}
		}
		else if( FlushForceDown() == true )
		{
			if( GetRideState() == RS_ON_RIDING && GetRideType() == RT_RIDER ) // 라이더
			{
                if ( IsLocalUnit() == false )
                {
                    bStateChanged = false;
                    UnDoStateChange_Reaction();
                }
                else
                {
				    m_PhysicParam.nowSpeed.y = 0.0f;
				    //StateChange( m_AirFallState, true );
				    RideOff();
                    bStateChanged = true;
                }

			}
			else
			if( IsOnSomething() == true )
			{
				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
					bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageLandFront, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
				else
					bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageLandBack, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}
			else
			{
				m_PhysicParam.nowSpeed.y = 0.0f;
                bTmpStateChanged = StateChange_Reaction_FIRST( m_CommonState.m_DownDamageAir, iAttackerUnitIndex,  pDamageData->impactPoint, bRelaxReactionCheck );
                bStateChanged = bStateChanged || bTmpStateChanged;
			}

		}
	}

	if( NULL != m_pAI )
		m_pAI->SetPrevAIMessage( SCT_NO_CHANGE ); // note: damage에 의해 state가 바뀌면 ai에 알려준다


    return  bStateChanged;
}


void CX2GUNPC::AttackResult()
{
#if !X2_DEV_TOOLS
#endif

	KTDXPROFILE();
	if( m_DamageData.reActResult == CX2DamageManager::RT_NO_DAMAGE || m_DamageData.reActResult == CX2DamageManager::RT_SENSOR)
		return;

    const StateData& nowStateData = GetNowStateData();


	if( m_DamageData.defenderType == CX2DamageManager::AT_UNIT
		&& null != m_DamageData.optrDefenderGameUnit
		&& m_DamageData.optrDefenderGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser* pCX2GUUser = static_cast<CX2GUUser*>( m_DamageData.optrDefenderGameUnit.GetObservable() );
		m_HitterUID = pCX2GUUser->GetUnitUID();

		if ( GetUserGrap() == true && pCX2GUUser->GetGrap() == false )  // 잡는 공격이고, 타격대상은 잡히지 않은 상태일 때
		{
			if ( pCX2GUUser != NULL )
			{
				pCX2GUUser->SetGrap( true );
                pCX2GUUser->SetGrapShakeCount( nowStateData.m_iGrapShakeCount );
				SetUserGrap( true, m_vGrapPosition); // kimhc // grap: 이걸 다시 해줘야 할 필요가 있나??

				D3DXVECTOR3 vBeforeGrapPos = pCX2GUUser->GetPos();
				pCX2GUUser->SetBeforeGrapPosition(vBeforeGrapPos);

				// oasis907 : 김상윤 [2011.1.25] // Grapped 위치 결정, 유저의 위치를 Grap의 중심으로 조금 뭉치게
				//D3DXVECTOR3 vGrappedPosition = (pCX2GUUser->GetPos() * 0.3f) +  (m_vGrapPosition * 0.7f);

				// oasis907 : 김상윤 [2011.6.8] // 여전한 라인맵 이탈 버그 발생으로 수정
				D3DXVECTOR3 vGrappedPosition = pCX2GUUser->GetPos();

				pCX2GUUser->SetGrappedPosition(vGrappedPosition);	// kimhc // grap: 레퍼런스로 넘기는게 좋을 듯

				m_vecGameUnitoPtr.push_back( CX2GameUnitoPtr( pCX2GUUser ) );
				if ( pCX2GUUser->IsMyUnit() )
				{
					if( g_pX2Game->GetMyUnit() != NULL && m_hSeqStickShake == INVALID_PARTICLE_SEQUENCE_HANDLE && m_HitterUID == g_pX2Game->GetMyUnit()->GetUnitUID() )
					{
						D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( g_pX2Game->GetMyUnit()->GetPos() );
						projPos.y -= 40.0f;
						m_hSeqStickShake = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"StickShake", projPos.x, projPos.y, 0.0f );
					}
				}
			}
		}

//{{ kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
		if ( GetUserHold() == true && pCX2GUUser->GetHold() == false )
		{
			bool bRevenge = false;

			// 반격시 Hold를 풀어야 한다면
			if ( GetUnHoldWhenRevenged() == true )
			{
				// 현재 반격 중인지를 검사 한 후
				if ( pCX2GUUser->GetFrameData().stateParam.fRevengeStartTime < pCX2GUUser->GetXSkinAnim()->GetNowAnimationTime()
					 && pCX2GUUser->GetFrameData().stateParam.fRevengeEndTime > pCX2GUUser->GetXSkinAnim()->GetNowAnimationTime() )
				{
                    // 몬스터가 앞에서 공격했는지를 검사 함
					D3DXVECTOR3 dirUnitToMonster( 0, 0, 0 );
					dirUnitToMonster = GetPos() - pCX2GUUser->GetPos();

					// 몬스터가 앞에서 공격했으면
                    dirUnitToMonster.y = 0.f;
					if ( D3DXVec3Dot( &(pCX2GUUser->GetDirVector()), &dirUnitToMonster ) >= 0 )
						bRevenge = true;
				}
			}

			if ( bRevenge == false
				 && ( m_vecHoldUserUID.empty() == true
					  || GetHoldOnlyOneUser() == false ) )
			{
				pCX2GUUser->SetHold( true );
				back_inserter( m_vecHoldUserUID ) = m_HitterUID;
			}
		}
//}} kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함

		//{{ JHKang / 강정훈 / 2010.12.17 / NPC Monster가 Monset Hold 시 문제 발생
		m_eHitterUnitType = pCX2GUUser->GetGameUnitType();
		//}} JHKang / 강정훈 / 2010.12.17 / NPC Monster가 Monset Hold 시 문제 발생
	}

	//{{ JHKang / 강정훈 / 2010.12.17 / NPC Monster가 Monset Hold 시 문제 발생
	//{{ JHKang / 강정훈 / 2010.12.17 / NPC Monster가 Monset Hold 시 문제 발생


	if( NULL != m_pAI &&
		CX2NPCAI::NAT_ALLY == m_pAI->GetAIType() )
	{
		CX2AllyNPCAI* pNPCAI = (CX2AllyNPCAI*) m_pAI;

		CX2GameUnit *pGameUnit = pNPCAI->GetAllyGameUnit();

		if( NULL != pGameUnit )
		{
			pGameUnit->UpNowMp( m_DamageData.fHitAddMP );
			if( pGameUnit->GetComboManager() != NULL )
				pGameUnit->GetComboManager()->AddCombo();
			DoDelegateProcessInAttackResult( m_DamageData );
		}

		UpDownCrashCamera( m_DamageData.fCameraCrashGap, m_DamageData.fCameraCrashTime );
	}



	UpNowMp( 2.0f );
	UpNowSoul( 1.0f );
	SetStopTime( m_DamageData.fStopTimeAtt );

	m_DamageLightTime = 0.2f;
	SetNowForceDown( 0.f );



	CX2GameUnit *pDefender = m_DamageData.optrDefenderGameUnit.GetObservable();
	switch( m_DamageData.reActResult )
	{
	case CX2DamageManager::RT_DUMMY_DAMAGE:
	case CX2DamageManager::RT_GRAPPLE:
	case CX2DamageManager::RT_THROW:

	case CX2DamageManager::RT_FLIP_DIFF_DIR:
	case CX2DamageManager::RT_FLIP_DIFF_DIR_DUMMY_DAMAGE :
		break;

	case CX2DamageManager::RT_REVENGE:
		{
			//
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH ||	pDefender->IsMyUnit() )
			{
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpactSlashCW",
					m_DamageData.impactPoint,
					50, 50, 1, 3 );
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpactSlashCCW",
					m_DamageData.impactPoint,
					50, 50, 1, 3 );
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpact",
					m_DamageData.impactPoint,
					50, 50, 1, 1 );
				CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
				pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"RevengeImpactTick",
					m_DamageData.impactPoint,
					200, 200, 2, 20 );

				if( pSeq != NULL )
					pSeq->SetLandPosition( m_NPCFrameData.unitCondition.landPosition.y );
			}

			if ( NULL != g_pX2Game->GetMyUnit() && GetDistance3Sq( GetPos(), g_pX2Game->GetMyUnit()->GetPos() ) < LIMIT_DISTANCE_SQ )
			{
				g_pX2Game->GetWorld()->SetWorldColor( 0xff000000 );
				g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );
			}

			m_PhysicParam.nowSpeed.x = 0.0f;
			m_PhysicParam.nowSpeed.y = 0.0f;

			if( IsLocalUnit() == true )
			{
				AttackResultStateChange();
			}
		}
		break;

	case CX2DamageManager::RT_SMALL_DAMAGE:
	//{{ kimhc // 2011.1.8 // 청 1차 전직
	case CX2DamageManager::RT_FLIP_SAME_DIR:
	//}} kimhc // 2011.1.8 // 청 1차 전직
		{
			//
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH ||	pDefender->IsMyUnit() )
			{
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpact",
					m_DamageData.impactPoint,
					100, 200, 2, 10 );
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactCore",
					m_DamageData.impactPoint,
					100, 200, 2, 10 );
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlash",
					m_DamageData.impactPoint,
					50, 50, 2, 2 );

				PlayEffectSetImpactPointByBuff();
			}
		} break;

	case CX2DamageManager::RT_CREATE_DAMAGE_EFFECT:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetDamageEffect() )
			{
				// fix!! damage effect이름과 powerrate를 입력 받을 수 있게
				g_pX2Game->GetDamageEffect()->CreateInstance( this, m_DamageData.m_wstrCreateDamageEffectName.c_str(), GetPowerRate(),
					m_DamageData.impactPoint, GetRotateDegree(), GetRotateDegree(), m_NPCFrameData.unitCondition.landPosition.y );
			}
		} break;




	case CX2DamageManager::RT_CREATE_EFFECT_SET:
		{

				if( NULL != g_pX2Game )
				{
					CX2GameUnit* pTraceUnit = NULL;
					if( true == m_DamageData.m_bCreateEffectSetTraceUnit )
					{
						pTraceUnit = m_DamageData.optrDefenderGameUnit.GetObservable();
					}

					CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( m_DamageData.m_wstrCreateEffectSetName.c_str(), this, pTraceUnit, false, GetPowerRate() );
					if ( CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSet ) )
					{
						pInstance->m_bTraceUnitDieDeleteEffectSet = m_DamageData.m_bTraceUnitDieDeleteEffectSet;
						pInstance->m_bCreateEffectSetTraceUnit = m_DamageData.m_bCreateEffectSetTraceUnit;
					}
				}

		} break;


	default:
		{
			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH || pDefender->IsMyUnit() )
			{
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactRed",
					m_DamageData.impactPoint,
					100, 200, 2, 10 );

				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactRingRed",
					m_DamageData.impactPoint,
					100, 200, 1, 1 );

				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactCoreRed",
					m_DamageData.impactPoint,
					100, 200, 1, 1 );

				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed",
					m_DamageData.impactPoint,
					50, 50, 2, 1 );

				PlayEffectSetImpactPointByBuff();
			}
		} break;
	}

	if ( CX2DamageManager::RT_DUMMY_DAMAGE != m_DamageData.reActResult &&
		 IsLocalUnit() )
		SetHitCount( GetHitCount() + 1 );

	if( IsLocalUnit() == true && m_DamageData.m_bDrain == true && NULL != pDefender)
	{
		//CX2DamageManager::Damage damage = SetAttackDamage( &m_DamageData );
		//float fFinalDamage = SetDefenceDamage( &m_DamageData, damage );
//		ModifyFinalDamageByGameUnitType( &m_DamageData );

		float fFinalDamage = pDefender->GetFinalDamage( &m_DamageData );
		UpNowHp( fFinalDamage );
	}

#if !X2_DEV_TOOLS
#endif

		/// 일반 공격 타격시 이펙트 셋 생성하는 함수 검사
		PlayEffectSetNormalAttackByBuff( m_DamageData.attackType );
}

void CX2GUNPC::AttackResultStateChange()
{
	StateChange( m_DamageRevenge, true );
}

void CX2GUNPC::SetAttackedUnit()
{
    if ( m_iLua_AttackedFuncRefID != LUA_REFNIL )
    {
        lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), m_iLua_AttackedFuncRefID, g_pKTDXApp, g_pX2Game, this );
    }
}

void CX2GUNPC::ChangeModelDetail( int detailPercent )
{
	if ( m_pXSkinAnim != NULL )
	{
		m_pXSkinAnim->SetModelDetailPercent( detailPercent );
	}

	//if ( m_pOrgXSkinAnim != NULL )
	//{
	//	m_pOrgXSkinAnim->SetModelDetailPercent( detailPercent );
	//}
}

bool CX2GUNPC::GetIsFallDowning()
{
    if ( m_pXSkinAnim != NULL &&
        m_pXSkinAnim->GetNowAnimName().compare( L"DamageAirDownLanding" ) == 0 )
        return true;

    const StateData& nowStateData = GetNowStateData();
    if ( NOWSTATE_FLAG( DAMAGE_DOWN ) == false )
        return false;
    if( GetIsDamageDown() == false)
        return false;

	if( m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageLandFront
		|| m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageLandBack )
		//|| m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageAir )
	{
		return true;
	}

	return false;
}

//{{ kimhc // 2011.1.17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
/** @function : GetCombineMatrixFromBoneName
	@brief : 본 이름에 해당하는 CombineMatrix값을 얻어옴
	@param : const wstring& wstrBoneName
	@return : const D3DXMATRIX*
*/
/*virtual*/ const D3DXMATRIX*	CX2GUNPC::GetCombineMatrixFromBoneName( const wstring& wstrBoneName ) const
{
	if ( true == wstrBoneName.empty() )
	{
		return NULL;
	} // if

	CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( wstrBoneName.c_str() );
	if ( NULL == pMultiAnimFrame )
	{
		return NULL;
	} // if

	return &(pMultiAnimFrame->combineMatrix);
}
//}} kimhc // 2011.1.17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

//{{ robobeg : 2008-10-21
//void CX2GUNPC::SetShow( bool bShow )
/*virtual*/
void CX2GUNPC::NotifyShowObjectChanged()
{
//{{ robobeg : 2008-10-21
	//CX2GameUnit::SetShow( bShow );
    CX2GameUnit::NotifyShowObjectChanged();

    bool bShow = GetShowObject();

	if( m_pUnitShadow != NULL )
		m_pUnitShadow->SetShowObject( bShow );

	if( false == bShow )
		DisableAfterImage( true );

	if ( false == IsNullGageUI() )
		SetShowGage( bShow );

	for( UINT i=0; i<m_vecAttachMesh.size(); i++ )
	{
		m_vecAttachMesh[i]->SetShowObject( GetShowObject() );
	}
}
//}} robobeg : 2008-10-21


// X2GUNPC 가 KTDGManager 에 등록, 삭제될 때 장착 부품들도 같이 등록, 삭제하기 위해 사용됨
/*virtual*/
void CX2GUNPC::NotifyDGManagerChanged( CKTDGManager& refInManager_ )
{
    if ( IsRecursivelyManagedByDGManager() )
    {
        for( CX2GameUnit::Weapon* pWeapon : m_vecpWeapon )
        {
            if ( pWeapon != NULL )
                refInManager_.AddObjectChain( pWeapon );
        }//BOOST_TEST_FOREACH()
    }
    else
    {
        for( CX2GameUnit::Weapon* pWeapon : m_vecpWeapon )
        {
            if ( pWeapon != NULL )
                refInManager_.RemoveObjectChain( pWeapon );
        }//BOOST_TEST_FOREACH()
    }//if.. else..
}//CX2GUNPC::NotifyDGManagerChanged()
//}} robobeg : 2008-10-17



bool CX2GUNPC::IsMyUnit() const
{
	return false;
}

bool CX2GUNPC::IsLocalUnit() const
{
//#ifdef TRANSFORMER_TEST
//
//	if( true == GetTransformed() )
//	{
//		if( NULL != g_pX2Game->GetMyUnit() )
//		{
//			if( GetTransformerOwnerUID() == g_pX2Game->GetMyUnit()->GetUnitUID() )
//			{
//				return true;
//			}
//			else
//			{
//				return false;
//			}
//		}
//		else
//		{
//			ASSERT( "my unit can't be NULL" );
//			return g_pX2Game->IsHost();
//		}
//	}
//	else
//	{
//		return g_pX2Game->IsHost();
//	}
//
//#else TRANSFORMER_TEST
	//{{AFX
	return g_pX2Game->IsHost();
	//}}AFX
//#endif TRANSFORMER_TEST

}

bool CX2GUNPC::IsFocusUnit() const
{
	return false;
}

void CX2GUNPC::DyingStart()
{
#if !X2_DEV_TOOLS
#endif

        const StateData& nowStateData = GetNowStateData();



		if( GetNPCAI() != NULL &&
			GetNPCAI()->GetAIType() == CX2NPCAI::NAT_NORMAL &&
			GetNowHp() <= 0.f &&
			m_bDamageHistory == true &&
			*m_fDamageHistory <= 0.f &&
			IsLocalUnit() == true )
		{
			if( g_pInstanceData != NULL && g_pInstanceData->GetSendHackUser() == false && g_pKTDXApp->GetFindHacking() == false )
			{
				g_pInstanceData->SetSendHackUser(true);
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;

				if( g_pKTDXApp->GetFindHacking() == false )
				{
					g_pMain->SendHackMail_DamageHistory( ANTI_HACK_STRING_NPC_Dying_Check );
					//g_pInstanceData->SetSendHackMail(true);
				}

				//g_pX2Game->Handler_EGS_LEAVE_ROOM_REQ( NetError::NOT_LEAVE_ROOM_REASON_25 );
				//g_pX2Game->SetSendedLeaveRoom(true);
				//g_pKTDXApp->SetFindHacking( true );
			}

			return;
		}


	if( m_bDyingStart == true )
		return;



#if !X2_DEV_TOOLS
#endif






//#ifdef TRANSFORMER_TEST
//	if( true == GetTransformed() )
//	{
//		CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( GetTransformerOwnerUID() );
//		if( NULL != pUser )
//		{
//			pUser->TransformIntoUser();
//		}
//	}
//
//#endif TRANSFORMER_TEST





	CX2GameUnit::DyingStart();


	DeleteArmagedonBlade();


	if ( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD )		// 	필드에 존재한 NPC 가 Boss Group Data 를 갖고 있다면
	{
		if ( GetBossGroupID() > 0 && NULL != g_pX2Game )
		{
			// 해당 Boss Group 에 관계된 다른 GUNPC 를 찾는다.
			CX2GUNPC * pTargetNPC = static_cast<CX2BattleFieldGame*>(g_pX2Game)->GetChainedBossGroup(this);
			if ( NULL != pTargetNPC )
			{
				// 다른 GUNPC 객체가 존재한다면, 해당 몬스터에게 DamagedMap 데이터를 전달하고
				// 이 NPC 의 Boss Group ID 를 해제한다.
				for ( map<UidType,float>::iterator _itDamagedMap = m_DamagedMap.begin();
					_itDamagedMap != m_DamagedMap.end(); ++_itDamagedMap )
				{
					pTargetNPC->AddToDamagedMap ( _itDamagedMap->first, _itDamagedMap->second );
				}
				m_DamagedMap.clear();

				static_cast<CX2BattleFieldGame*>(g_pX2Game)->SetIndicatorToNpc ( pTargetNPC );
				SetBossGroupID( 0 );
				SetIsMainBoss ( false );
			}
			else
			{
				// 다른 GUNPC 객체가 존재하지 않는다면, 해당 그룹 몬스터가 모두 사망하였다는 것이므로
				// 해당 연출을 뿌려준다.
				static_cast<CX2BattleFieldGame*>(g_pX2Game)->UpdateCreateUnitDisplayEffect ( this, false );
			}
		}
	}


	if( IsLocalUnit() == true )
	{


        if ( NOWSTATE_FLAG( ESCAPE_END ) == true )
		{
			KEGS_NPC_UNIT_DIE_REQ kPacket( GetUID(), -1, -1, -1, KEGS_NPC_UNIT_DIE_REQ::NDS_ESCAPE, static_cast<int>( GetMaxHp() ) );
			AddDamagedMapAndPositionToDiePacket( kPacket );
            m_eNPCUnitDieState = eNPCUnitDieState_REQ;
			g_pX2Game->NPCUnitDieReq( kPacket );

			//g_pX2Game->NPCUnitDieReq( -1, GetUID(), -1, -1, m_NPCFrameData.unitCondition.landPosition, KEGS_NPC_UNIT_DIE_REQ::NDS_ESCAPE );
			return;
		}

		if( false == m_bSelfResurrection || m_iSelfResurrectionCount <= 0 )
		{
			// 죽은 몬스터가 이브의 소환 몬스터인지, 그냥 적 몬스터인지 구분 해줘야 한다. 서버에서 구분이 필요.


			//서버로 유닛 사망 데이터를 보낸다
            CX2GameUnit* pGameUnitWhoAttackedMe = GetGameUnitWhoAttackedMe();
			if( NULL == pGameUnitWhoAttackedMe )
			{
				// 몬스터 혼자서 죽은 경우

				KEGS_NPC_UNIT_DIE_REQ kPacket( GetUID(), -1, -1, -1, KEGS_NPC_UNIT_DIE_REQ::NDS_KILL_SELF, static_cast<int>( GetMaxHp() ) );
				AddDamagedMapAndPositionToDiePacket( kPacket );
				g_pX2Game->NPCUnitDieReq( kPacket );

				//g_pX2Game->NPCUnitDieReq( -1, GetUID(), -1, -1, m_NPCFrameData.unitCondition.landPosition, KEGS_NPC_UNIT_DIE_REQ::NDS_KILL_SELF );
                m_eNPCUnitDieState = eNPCUnitDieState_REQ;
			}
			else
			{
				enum KEGS_NPC_UNIT_DIE_REQ::NPC_DIE_STATE eNpcDieState = KEGS_NPC_UNIT_DIE_REQ::NDS_KILL_BY_USER;

				if( NULL != GetNPCAI() &&
					CX2NPCAI::NAT_ALLY == GetNPCAI()->GetAIType() )
					eNpcDieState = KEGS_NPC_UNIT_DIE_REQ::NDS_ALLY;
				else if ( true == m_bBonusDrop )
					eNpcDieState = KEGS_NPC_UNIT_DIE_REQ::NDS_BONUS_DROP;

				/// 데미지를 제일 많이 준 유저UID
				UidType fMaxDamageKillerUserUnitUID = GetGameUnitUIDMaxDamage();
				//if ( NULL != pGameUnitWhoAttackedMe )
				{
                    if ( pGameUnitWhoAttackedMe->GetGameUnitType() == CX2GameUnit::GUT_USER )
					{


						//pGameUnitWhoAttackedMe->DoProcessWhenKillEnemy( this );

						KEGS_NPC_UNIT_DIE_REQ kPacket( GetUID(), static_cast<int>( CX2GameUnit::GUT_USER ), pGameUnitWhoAttackedMe->GetUnitUID(), fMaxDamageKillerUserUnitUID,
							static_cast<char>( eNpcDieState ), static_cast<int>( GetMaxHp() ) );
						AddDamagedMapAndPositionToDiePacket( kPacket );
						//필드 보스 레이드 로그 작업.
						AddFieldBossRaidLog();

						g_pX2Game->NPCUnitDieReq( kPacket );

                        m_eNPCUnitDieState = eNPCUnitDieState_REQ;
					}
					else
					{
						CX2GameUnit::GAME_UNIT_TYPE eAttackerUnitType = CX2GameUnit::GUT_NPC;
						UidType uidAttackerUID = pGameUnitWhoAttackedMe->GetUnitUID();

						if( NULL != g_pX2Game )
						{
							// 이브의 소환 몬스터에 적 몬스터가 죽은 경우, User가 죽인 것으로 처리해줘야 한다.
							CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pGameUnitWhoAttackedMe );
							if( NULL != pNPC &&
								NULL != pNPC->GetNPCAI() )
							{
								switch( pNPC->GetNPCAI()->GetAIType() )
								{
								case CX2NPCAI::NAT_ALLY:
									{
										CX2AllyNPCAI* pNPCAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
										uidAttackerUID		= pNPCAI->GetAllyUnitUID();
										eAttackerUnitType	= CX2GameUnit::GUT_USER;
									} break;
								}
							}
						}

							KEGS_NPC_UNIT_DIE_REQ kPacket( GetUID(), static_cast<int>( eAttackerUnitType ), uidAttackerUID, fMaxDamageKillerUserUnitUID,
							static_cast<char>( eNpcDieState ), static_cast<int>( GetMaxHp() ) );
						AddDamagedMapAndPositionToDiePacket( kPacket );
						g_pX2Game->NPCUnitDieReq( kPacket );
                        m_eNPCUnitDieState = eNPCUnitDieState_REQ;
					}
				}
			}
		}
	}








#if !X2_DEV_TOOLS
#endif


	if( g_pX2Game != NULL && g_pX2Game->IsHost() == true && g_pX2Game->GetUserUnitNum() == 1 &&
		m_NPCFrameData.syncData.nowState != m_DyingLandFront && m_NPCFrameData.syncData.nowState != m_DyingLandBack && m_NPCFrameData.syncData.nowState != m_DyingSky
        && m_NPCFrameData.syncData.nowState != m_DyingFly
        )

	{
		if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
		{
			if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
			}
		}

		//if( g_pKTDXApp->GetFindHacking() == false )
		//{
		//	g_pMain->SendHackMail_DamageHistory("DUNGEON_CHECK01");
		//}
		//g_pKTDXApp->SetFindHacking( true );

		// 강제크래쉬
		__asm
		{
			xor eax, eax
			call eax
		}
	}



#if !X2_DEV_TOOLS
#endif

		ClearStartOfDelayedFiringData();		/// 사망시 지연의 신호탄 적용 정보 삭제

	if ( GetHasBossGage() )
		CX2GageManager::GetInstance()->ShowBossGageUIByUIDAndNotShowOtherBossGageUI( GetUnitUID(), false );
}

void CX2GUNPC::AddForceDownDelay( float fDelay )
{
	if( m_bAddForceDownGageThisState == false )
	{
		m_fMaxDownForce = fDelay;
		m_bAddForceDownGageThisState = true;
	}
	else
	{
		m_fMaxDownForce = max(m_fMaxDownForce, fDelay);
	}
}

void CX2GUNPC::CommonStateStartHost()
{
    const StateData& nowStateData = GetNowStateData();

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_NPCFrameData.syncData.m_usRandomTableIndex = (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( NOWSTATE_FLAG( IMMEDIATE_PACKET_SEND ) == true )
        m_bImmediate = true;

    m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum = m_NPCFrameData.syncData.stateChangeParts.stateModifyNum;
    m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum++;
	if( g_pMain->IsEnableUdpPacketOverlap() )
		SendReserve( 2 );
	else
		SendReserve( 1 );
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	LUA_GET_VALUE( m_LuaManager, "IMMADIATE_PACKET_SEND", m_bImmediate, false );
//	if( m_bImmediate == true )
//		SendReserve();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

    if ( NOWSTATE_FLAG( VIEW_TARGET ) == true )
	{
//#ifdef MONSTER_BABO_TEST
//		if( m_pAI->IsTargetFront() == false )
//		{
//			if ( m_ExtraDamagePack.m_ReverseLeftRight.m_fTime <= 0.0f )
//				m_NPCFrameData.syncData.bIsRight = !m_NPCFrameData.syncData.bIsRight;
//		}
//		else
//		{
//			if ( m_ExtraDamagePack.m_ReverseLeftRight.m_fTime > 0.0f )
//				m_NPCFrameData.syncData.bIsRight = !m_NPCFrameData.syncData.bIsRight;
//		}
//#else //MONSTER_BABO_TEST

		if( NULL != m_pAI &&
			m_pAI->IsTargetFront() == false )
		{
            SetIsRightNow( !m_NPCFrameData.syncData.bIsRight );
		}

//#endif //MONSTER_BABO_TEST
	}


    if ( NOWSTATE_FLAG( SET_DIRECTION ) == true )
    {
        SetIsRightNow( NOWSTATE_FLAG( SET_DIRECTION_VALUE ) );
    }


	if( NULL != m_pAI &&
		true == GetAIEnabled() )
	{
		if( m_pAI->IsTargetAccquired() == true )
			m_NPCFrameData.syncData.mindFlag = MF_TARGET_TAKE;
		else
			m_NPCFrameData.syncData.mindFlag = MF_TARGET_LOST;
	}
	else
		m_NPCFrameData.syncData.mindFlag = MF_NO;

    if ( NOWSTATE_FLAG( SET_MIND_FLAG ) == true )
        m_NPCFrameData.syncData.mindFlag = nowStateData.m_cSetMindFlagValue;

}

void CX2GUNPC::CommonStateStart()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();
    int index = 0;

	if ( NULL == m_pXSkinAnim )
		return;



    if( m_NPCFrameData.syncData.nowState == m_DyingLandFront || m_NPCFrameData.syncData.nowState == m_DyingLandBack || m_NPCFrameData.syncData.nowState == m_DyingSky
        || m_NPCFrameData.syncData.nowState == m_DyingFly
        )

	{
		DyingStart();
	}
    if ( NOWSTATE_FLAG( ESCAPE_END ) == true )
	{
		DyingStart();
	}


	if( NULL != m_pUnitSlashTraceManager )
	{
		m_pUnitSlashTraceManager->OnStateStart();
	}

	if ( IsLocalUnit() )
    {
        // 방장이면 보낼 싱크 데이타에 셋팅
		m_NPCFrameData.syncData.bComeBackState = GetIsComeBackState();
    }
// 	else					// 방장이 아니면 받은 싱크 데이타로 셋팅
// 		SetIsComeBackState( m_NPCFrameData.syncData.bComeBackState );


	// 컴백 중인 상태이고, 죽거나 탈출(마나이터 등)하는 것이 아니라면
	if (
        NOWSTATE_FLAG( DYING_END ) == false   // 이전 m_bDyingEnd 설정 시점에 문제인듯
        && NOWSTATE_FLAG( ESCAPE_END ) == false
         )
	{
		if ( true == m_NPCFrameData.syncData.bComeBackState && false == m_bUnitColor)
			SetUnitFadeStart( m_RenderParam.renderType, m_RenderParam.color.r, m_RenderParam.color.g, m_RenderParam.color.b, 0.3f, true, true );
		else if ( false == m_NPCFrameData.syncData.bComeBackState && true == m_bUnitColor )
			SetUnitFadeStart( m_RenderParam.renderType, m_RenderParam.color.r, m_RenderParam.color.g, m_RenderParam.color.b, 1.0f, false, false );
	}

//#ifdef LINEMAP_FAST_WIND_TEST
//	m_bEnableUpsideWind = true;
//	m_bWaitInTheAir		= false;
//	m_fTimeInTheAir		= 0.f;
//#endif LINEMAP_FAST_WIND_TEST

	m_HitUnitList.resize(0);

    m_NPCFrameData.unitCondition.fStateTimeBack		= 0.0f;
	m_NPCFrameData.unitCondition.fStateTime			= 0.0f;
	m_NPCFrameData.unitCondition.bHit				= false;

    m_NPCFrameData.stateParam.bLandConnect = NOWSTATE_FLAG( SET_LAND_CONNECT_VALUE );


		if( NCT_MONSTER_CARD == GetNPCCreateType() && GetNowStateID() == m_StartState )
		m_NPCFrameData.stateParam.bLandConnect = false;




    m_NPCFrameData.stateParam.bCanPushUnit = NOWSTATE_FLAG( SET_CAN_PUSH_UNIT_VALUE );
    m_NPCFrameData.stateParam.bCanPassUnit = NOWSTATE_FLAG( SET_CAN_PASS_UNIT_VALUE );
    if ( true == m_NPCFrameData.syncData.bComeBackState )
    {
        m_NPCFrameData.stateParam.bCanPassUnit = true;
        m_DamagedMap.clear();
    }

    m_NPCFrameData.stateParam.bSuperArmor = NOWSTATE_FLAG( SET_SUPER_ARMOR_VALUE );
    m_NPCFrameData.stateParam.bSuperArmorNotRed = NOWSTATE_FLAG( SET_SUPER_ARMOR_NOT_RED_VALUE );



    if ( nowStateData.m_fSetGlobalSuperArmorTimeValue > 0.f )
    {
        m_fGlobalTimeSuperArmor = nowStateData.m_fSetGlobalSuperArmorTimeValue;
    }
    else if ( nowStateData.m_vecSuperArmorTime.empty() == false )
    {
        m_fGlobalTimeSuperArmor = 0.f;
    }



    m_iFollowHitter = nowStateData.m_iSetFollowHitterValue;

	if( m_pXSkinAnim != NULL )
		m_pXSkinAnim->SetApplyMoveBone( NOWSTATE_FLAG( APPLY_MOVEBONE ) );




	// 컴백 스테이트 인 경우 무조건 무적
	if ( true == m_NPCFrameData.syncData.bComeBackState )
	{
		m_NPCFrameData.stateParam.bInvincible = true;
	}
	else
	{
        m_NPCFrameData.stateParam.invincibleTime.m_fX = nowStateData.m_v2SetInvincibleTimeValue.x;
        m_NPCFrameData.stateParam.invincibleTime.m_fY = nowStateData.m_v2SetInvincibleTimeValue.y;
		if( m_NPCFrameData.stateParam.invincibleTime.m_fX <= 0.0f && m_NPCFrameData.stateParam.invincibleTime.m_fY > 0.0f )
			m_NPCFrameData.stateParam.bInvincible = true;
		else
			m_NPCFrameData.stateParam.bInvincible = false;
        SetForceInvincible( nowStateData.m_fSetForceInvincibleTimeValue, nowStateData.m_fSetShowInvincibleTimeValue );
	}





    if ( NOWSTATE_FLAG( SET_DEFENCE ) == false )
    {
        SetDamageReducePercent( 0 );
    }

    SetDamageGaurdPercent( (int) nowStateData.m_fSetGuardDefenceValue );

    m_NPCFrameData.stateParam.fRevengeStartTime = nowStateData.m_v2SetRevengeTimeValue.x;
    m_NPCFrameData.stateParam.fRevengeEndTime = nowStateData.m_v2SetRevengeTimeValue.y;

	if( NOWSTATE_FLAG( SET_REFLECT_MAGIC ) == true )
	{
        m_v2ReflectMagic = nowStateData.m_v2SetReflectMagicValue;
	}

    const wstring& animName = nowStateData.m_wstrAnimName;

    bool bTransition = NOWSTATE_FLAG( TRANSITION );


	bool bChangeAnim = false;
	if( m_NPCFrameData.unitCondition.bTransitionChange == false || bTransition == false )
		bChangeAnim = m_pXSkinAnim->ChangeAnim( animName.c_str(), false );
	else
		bChangeAnim = m_pXSkinAnim->ChangeAnim( animName.c_str(), true );

	// 오현빈 // 2012-12-12 // 없는 모션이 지정되어 있으면 알려주도록 ASSERT 추가
	if( false == animName.empty() && false == bChangeAnim )
	{
		//ASSERT( !"NPC Motion Error" );

		WCHAR wszText[1024] = L"";
		swprintf( wszText, ARRAY_SIZE(wszText), L"NPC Motion Error. LuaFileName : %s, Motion Name : %s ", GetNPCTemplet().m_LuaFileName.c_str(), animName.c_str() );
		StateLog(wszText);
#if X2_DEV_TOOLS
		g_pChatBox->AddChatLog( wszText,
			KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
#endif //_IN_HOUSE_

	}

    m_pXSkinAnim->Play( nowStateData.m_ePlayType );
    m_fAnimWaitTime = nowStateData.m_fSetAnimWaitTimeValue;

	m_pXSkinAnim->ResetOneshotPerformed();

    if ( NOWSTATE_FLAG( SET_ANIM_SPEED ) == true )
        m_fAnimSpeed = nowStateData.m_fSetAnimSpeedValue;
    else
        m_fAnimSpeed = m_fDefaultAnimSpeed;


	ResetAnimSpeed();


	// 현재스테이트에서 발생하는 effectset list를 담는다.

    if ( NOWSTATE_FLAG( CUSTOM_STATE_DELETE_EFFECT_SET ) == true )
	{
		DeleteEffectSetOnCustomState();
	}

    m_iEffectSetToPlayProgress = 0;

	m_vecEffectSetToDeleteOnDamageReact.resize(0);
	m_vecEffectSetToDeleteOnStateEnd.resize(0);




    m_NPCFrameData.syncData.position.x += nowStateData.m_vAddPos.x;
    if ( nowStateData.m_vAddPos.y != 0.f )
    {
        m_NPCFrameData.syncData.position.y +=
            ( true == m_NPCFrameData.syncData.bComeBackState )
              ? nowStateData.m_vAddPos.y * 4.f : nowStateData.m_vAddPos.y;
    }
    m_NPCFrameData.syncData.position.z += nowStateData.m_vAddPos.z;

    bool bSickShake = NOWSTATE_FLAG( SHOW_STICK_SHAKE );




	if( g_pX2Game->GetMyUnit() != NULL && bSickShake == true &&
		m_hSeqStickShake == INVALID_PARTICLE_SEQUENCE_HANDLE && m_HitterUID == g_pX2Game->GetMyUnit()->GetUnitUID() )
	{
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( g_pX2Game->GetMyUnit()->GetPos() );
		projPos.y -= 40.0f;
		m_hSeqStickShake = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"StickShake", projPos.x, projPos.y, 0.0f );
	}



    m_iSpeedFactorProgress = 0;




	float fWorldGravitySpeedScale = 1.f;
	float MAGIC_GRAVITY_SCALE = 1.f;
	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		1.f != g_pX2Game->GetWorld()->GetGravityAndSpeedRate() )
	{
		fWorldGravitySpeedScale = g_pX2Game->GetWorld()->GetGravityAndSpeedRate();
		MAGIC_GRAVITY_SCALE = 0.5f;
		if( fWorldGravitySpeedScale > 1.f )
		{
			MAGIC_GRAVITY_SCALE = 1.5f;
		}
	}


    if ( NOWSTATE_FLAG( SET_G_ACCEL ) == true )
        m_PhysicParam.fGAccel = nowStateData.m_fSetGAccelValue * fWorldGravitySpeedScale * MAGIC_GRAVITY_SCALE;
    if ( NOWSTATE_FLAG( SET_DOWN_ACCEL ) == true )
        m_PhysicParam.fDownAccel = nowStateData.m_fSetDownAccelValue * fWorldGravitySpeedScale * MAGIC_GRAVITY_SCALE;
    if ( NOWSTATE_FLAG( SET_SPEED_X ) == true )
        m_PhysicParam.nowSpeed.x = nowStateData.m_v2SetSpeedValue.x * fWorldGravitySpeedScale;
    if ( NOWSTATE_FLAG( SET_SPEED_Y ) == true )
        m_PhysicParam.nowSpeed.y = nowStateData.m_v2SetSpeedValue.y * fWorldGravitySpeedScale;
    m_PhysicParam.passiveSpeed = nowStateData.m_v2SetPassiveSpeedValue;


	if( -1.f != m_PhysicParam.passiveSpeed.x )
	{
		// 컴백 스테이트인 경우 이동 스피드 빠르게
		if ( true == m_NPCFrameData.syncData.bComeBackState )
			m_PhysicParam.passiveSpeed.x *= 4.0f;

		m_PhysicParam.passiveSpeed.x	*= fWorldGravitySpeedScale;
	}
	if( -1.f != m_PhysicParam.passiveSpeed.y )
	{
		m_PhysicParam.passiveSpeed.y	*= fWorldGravitySpeedScale;
	}

	if( GetIsHeavyMonster() == true && m_NPCFrameData.syncData.nowState == m_UpDamage
		&& false == m_NPCFrameData.syncData.bComeBackState )
	{
		m_PhysicParam.passiveSpeed.y = -1;
		m_PhysicParam.nowSpeed.y = GetJumpSpeed() * 0.6f;
	}



    m_bNeverMove = NOWSTATE_FLAG( SET_NEVER_MOVE_VALUE );

	//if( m_bNeverMove == true )
	{
		m_ORGPos = m_NPCFrameData.syncData.position;
	}

	if( m_bNeverMove == true )
	{
		m_PhysicParam.passiveSpeed.x = 0.f;
		m_PhysicParam.passiveSpeed.y = 0.f;
		m_PhysicParam.nowSpeed.x = 0.f;
		m_PhysicParam.nowSpeed.y = 0.f;
	}


	if( NOWSTATE_FLAG( DYING_END ) == true
        && GetNowHp() <= 0.f && nowStateData.m_fDyingSpeed != 1.0f )
    {
		m_pXSkinAnim->SetPlaySpeed( nowStateData.m_fDyingSpeed );
    }

    if ( nowStateData.m_v2AfterImageTime.x == -1.f
        && nowStateData.m_v2AfterImageTime.y == -1.f )
    {
        DisableAfterImage();
    }


	m_DamageData.SimpleInit();
	m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
	m_DamageData.optrAttackerGameUnit		= this;

	g_pData->GetDamageManager()->SetDamageDataFromLUA( &m_DamageData, m_LuaManager, "DAMAGE_DATA", GetPowerRate() );
	ZeroMemory( m_NPCFrameData.stateParam.bEventFlagList, sizeof(bool) * EVENT_FLAG_LIST_NUM );

    {
        const std::vector<float> vecfEventIntervalTime = nowStateData.m_vecfEventIntervalTime;
        m_EventIntervalList.resize( vecfEventIntervalTime.size() );
        {
            for( unsigned u = 0; u != m_EventIntervalList.size(); u++ )
            {
                std::pair<D3DXVECTOR3,int>& interval = m_EventIntervalList[ u ];
                interval.first.y = interval.first.x = vecfEventIntervalTime[ u ];
                interval.first.z = 0.0f;
                interval.second = 0;
            }//for
        }
    }

    m_eSoundSetType = SOUND_SET_DEFUALT;


    if ( NOWSTATE_FLAG( STOP_LOOP_SOUND ) == true )
    {
		m_vecSoundPlayLoop.resize( 0 );
        for( CKTDXDeviceSound *pSound : m_vecLoopSound )
		{
			if( pSound != NULL )
			{
				pSound->Stop();
			}
		}
		m_vecLoopSound.resize( 0 );
    }
    else
    {
        for( const CX2GameUnit::SoundPlayData& sndPlayData : nowStateData.m_vecSetSoundPlayLoopValue )
	    {
		    bool bExist = false;
            for( const CX2GameUnit::SoundPlayData& sndPlayDataLoop : m_vecSoundPlayLoop )
		    {
			    if( sndPlayDataLoop.m_SoundPlayName == sndPlayData.m_SoundPlayName )
			    {
				    bExist = true;
				    break;
			    }
		    }
		    if( bExist == false )
			    m_vecSoundPlayLoop.push_back( sndPlayData );
	    }
    }

    {
	    int iRand = rand()%100 + 1;
        ESoundSetType   eSoundSetType = SOUND_SET0_NORMAL;
        int iFlagIndex = StateData::FLAG_SOUND_SET0;
	    if( iRand < nowStateData.m_iSoundSet0Rate )
	    {
            eSoundSetType= SOUND_SET0_NORMAL;
            iFlagIndex = StateData::FLAG_SOUND_SET0;
	    }
	    else if( iRand < ( nowStateData.m_iSoundSet0Rate + nowStateData.m_iSoundSet1Rate ) )
	    {
            eSoundSetType = SOUND_SET1_NORMAL;
            iFlagIndex = StateData::FLAG_SOUND_SET1;
	    }
	    else if( iRand < ( nowStateData.m_iSoundSet0Rate + nowStateData.m_iSoundSet1Rate + nowStateData.m_iSoundSet2Rate ) )
	    {
            eSoundSetType = SOUND_SET2_NORMAL;
            iFlagIndex = StateData::FLAG_SOUND_SET2;
	    }
	    else
        {
            eSoundSetType = SOUND_SET0_NORMAL;
            iFlagIndex = StateData::FLAG_SOUND_SET0;
        }
        if ( nowStateData.m_bitsetFlags.test( iFlagIndex ) == true )
	    {
            m_eSoundSetType = eSoundSetType;
            int iFlagMode = 0;
		    if ( 0.f < GetRemainHyperModeTime() && true == nowStateData.m_bitsetFlags.test( iFlagIndex + 1 ) )
            {
                m_eSoundSetType = ESoundSetType( eSoundSetType + 1 );
            }
		    if ( true == IsFullHyperMode() && 0.f < GetRemainHyperModeTime() && true == nowStateData.m_bitsetFlags.test( iFlagIndex + 2 ) )
            {
                m_eSoundSetType = ESoundSetType( eSoundSetType + 2 );
            }
	    }
    }


    if ( nowStateData.m_vecwstrEnableAttackBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrEnableAttackBox )
        {
            SetEnableAttackBox( name.c_str(), true );
        }//BOOST_FOREACH()
    }//if

    if ( nowStateData.m_vecwstrDisableAttackBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrDisableAttackBox )
        {
            SetEnableAttackBox( name.c_str(), false );
        }//BOOST_FOREACH()
    }//if


    if ( nowStateData.m_vecwstrEnableCollisionBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrEnableCollisionBox )
        {
            SetEnableCollisionBox( name.c_str(), true );
        }//BOOST_FOREACH()
    }//if

    if ( nowStateData.m_vecwstrDisableCollisionBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrDisableCollisionBox )
        {
            SetEnableCollisionBox( name.c_str(), false );
        }//BOOST_FOREACH()
    }//if

	bool bShow = true;
    if ( NOWSTATE_FLAG( SET_SHOW ) == true )
    {
        bShow = NOWSTATE_FLAG( SET_SHOW_VALUE );
        SetShowObject( bShow );
    }

    if ( NOWSTATE_FLAG( SET_SHOW_NAME ) == true )
    {
        m_bShowName = NOWSTATE_FLAG( SET_SHOW_NAME_VALUE );
		m_bShowOnMiniMap = m_bShowName;
		if ( false == IsNullGageUI() )
			SetShowGage( m_bShowName );
    }



    if ( NOWSTATE_FLAG( SET_INVISIBLE_TO_ENEMY ) == true )
	{
        if ( NOWSTATE_FLAG( SET_INVISIBLE_TO_ENEMY_VALUE ) == true )
		{
			if( NULL != g_pX2Room->GetMySlot() )
			{
				if( g_pX2Room->GetMySlot()->GetTeamNum() == GetTeam() )
				{
					if( true == bShow )
					{
						SetShowObject( true );
					}

					if( true == bShow && true == m_bShowName )
					{
						if ( false == IsNullGageUI() )
							SetShowGage( true );
					}
				}
				else
				{
					SetShowObject( false );
					if ( false == IsNullGageUI() )
						SetShowGage( false );
				}
			}
		}
		else
		{
			if( true == bShow )
			{
				SetShowObject( true );
			}

			if( true == bShow && true == m_bShowName )
			{
				if ( false == IsNullGageUI() )
					SetShowGage( true );
			}
		}
	}


	//이펙트 데이타
	//파티클


    if ( nowStateData.m_vecParticleSeq.empty() == false )
    {
        const std::vector<ParticleEffectData>& vecsrc = nowStateData.m_vecParticleSeq;
        m_ParticleEffData.reserve( m_ParticleEffData.size() + vecsrc.size() );
        for( unsigned u = 0; u != vecsrc.size(); u++ )
        {
            ParticleEffectData* pParticleEffectData = new ParticleEffectData( vecsrc[u] );
            ASSERT( pParticleEffectData != NULL );
            m_ParticleEffData.push_back( pParticleEffectData );
        }//for
    }//if

    if ( nowStateData.m_vecMeshPlayer.empty() == false )
    {
        const std::vector<MeshPlayerEffectData>& vecsrc = nowStateData.m_vecMeshPlayer;
        m_MeshPlayerEffData.reserve( m_MeshPlayerEffData.size() + vecsrc.size() );
        for( unsigned u = 0; u != vecsrc.size(); u++ )
        {
            MeshPlayerEffectData* pMeshPlayerEffectData = new MeshPlayerEffectData( vecsrc[u] );
            ASSERT( pMeshPlayerEffectData != NULL );
            m_MeshPlayerEffData.push_back( pMeshPlayerEffectData );
        }//for
    }//if

    m_iCameraCrashProgress = 0;



	g_pX2Game->GetNPCFunc()->StartFunc( this );
    int iLua_StateStartRefID = GetNowStateRuntime().m_iLua_StateStartRefID;
    if ( iLua_StateStartRefID != LUA_REFNIL )
	{
		lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), iLua_StateStartRefID, g_pKTDXApp, g_pX2Game, this );
	}

//	if( false == GetNowStateData().m_strLua_StateStart.empty() )
//	{
//		string func;
//		ConvertWCHARToChar( func, GetNowStateData().m_strLua_StateStart.c_str() );
//#ifdef LEAVE_LAST_ERROR_LOG_TEST
//		LastErrorLog( func.c_str() );
//#endif LEAVE_LAST_ERROR_LOG_TEST
////#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		lua_tinker::call<void>( GetFunctionLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
////		lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	}


	bool bDyingEndImmadiate = false;
    bDyingEndImmadiate = NOWSTATE_FLAG( DYING_END_IMMEDIATE );
	if(
		true == bDyingEndImmadiate )
	{
		SetNowHp( 0.f );

        SetShowObject( false );
        StateChangeToDieForce();
	}



//#pragma DMLEE_NOTE( "몬스터 eventprocess data를 매번 파싱하지 않도록 수정해야 할 듯. 최소한 lua에서 파싱해오지는 않도록" )




	if( true == g_pX2Game->GetEnableNPCTalkBox() )
	{
		if( nowStateData.m_vecTalkBox.empty() == false )
		{
			int iRateLowerBound = 0, iRateUpperBound = 0;
			int randomNumber = rand()%100;

            for( const TalkBox& talkBoxData : nowStateData.m_vecTalkBox )
			{
				iRateUpperBound += talkBoxData.m_iRate;

				// 원형맵상의 먼거리에 있는 말은 랜더하지 않는다.
				if( iRateLowerBound < randomNumber && randomNumber <= iRateUpperBound && GetDistanceToCamera() < 3800)
				{
					CX2TalkBoxManager* pTalkBoxMgr = NULL;
					pTalkBoxMgr = g_pX2Game->GetTalkBoxManager();

					if( NULL != pTalkBoxMgr )
					{
						CX2TalkBoxManagerImp::TalkBox talkBox;
						talkBox.m_OwnerUnitUID		= this->GetUnitUID();
						talkBox.m_GameUnitType		= CX2GameUnit::GUT_NPC;
						talkBox.m_bTraceUnit		= true;
						talkBox.m_TalkBoxType		= CX2TalkBoxManagerImp::TBT_FROM_DOWN_LEFT;
						talkBox.m_wstrTalkContent	= GET_STRING( talkBoxData.m_iMessage );
						talkBox.m_fDelayTime		= talkBoxData.m_fDelayTime;
						pTalkBoxMgr->Push( talkBox );
						break;
					}
				}
				iRateLowerBound += talkBoxData.m_iRate;
			}
		}
	}

    if ( nowStateData.m_veciEnableWeapon.empty() == false )
    {
        for( int weaponIndex : nowStateData.m_veciEnableWeapon )
        {
            SetWeaponEnable(weaponIndex, true);
        }//
    }//if

    if ( nowStateData.m_veciDisableWeapon.empty() == false )
    {
        for( int weaponIndex : nowStateData.m_veciDisableWeapon )
        {
            SetWeaponEnable( weaponIndex, false);
        }//
    }//if

    if ( NOWSTATE_FLAG( SET_FLY_AI ) == true )
	{
		if(m_pAI != NULL)
		{
            m_pAI->SetAiFlyNowState( NOWSTATE_FLAG( SET_FLY_AI_VALUE ) );
		}
	}
	else
	{
		if(m_pAI != NULL)
		{
			m_pAI->SetAiFlyNowState( m_pAI->GetAIFly() );
		}
	}



    //{{ kimhc // 2010.6.29 // 피격 모션등은 실행되고, 받는 데미지는 모두 0가 되게하는 기능으로 몬스터 state table 내에서 사용
#if X2_DEV_TOOLS
    SetZeroDamage( NOWSTATE_FLAG( SET_ZERO_DAMAGE ) );
#endif // DAMAGE_ZERO
    //}} kimhc // 2010.6.29 // 피격 모션등은 실행되고, 받는 데미지는 모두 0가 되게하는 기능으로 몬스터 state table 내에서 사용

    m_bAbsorbMagicState = NOWSTATE_FLAG( SET_ABSORB_MAGIC_VALUE );

	//{{ JHKang / 강정훈 / 2011/01/13 / 몬스터 State당 렌더링 방식 변경
    if ( NOWSTATE_FLAG( SET_RENDER_PARAM ) == true )
        m_RenderParam.renderType = nowStateData.m_eSetRenderParamValue;
    else
        m_RenderParam.renderType = m_BasicRenderType;
	m_RenderParam.fOutLineWide	= 1.7f;
	if( g_pX2Game->GetWorld() != NULL )
		m_RenderParam.lightPos	= g_pX2Game->GetWorld()->GetLightPos();
	//}} JHKang / 강정훈 / 2011/01/13


	// 대전, 자유훈련소, 헤니르시공에서는 던전알람 동작하지 않도록 수정
	if( g_pX2Game->GetDungeonType() != (int)CX2Dungeon::DT_HENIR &&
		g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME &&
		g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME )
	{
		m_iDangerAlarm = g_pX2Game->GetDangerAlarm( GetUnitUID(), GetNowStateID() );
		if( m_iDangerAlarm < 0 )
		{
			m_iDangerAlarm = g_pX2Game->LoadDangerAlarm( m_LuaManager, GetUnitUID(), GetNowStateID() );
		}
		else if( g_pX2Game->GetAlarmRepeat(m_iDangerAlarm) == false )
		{
			m_iDangerAlarm = -1;
		}
	}



    if ( NOWSTATE_FLAG( SET_DIRECTION ) == true )
    {
        SetIsRightNow( NOWSTATE_FLAG( SET_DIRECTION_VALUE ) );
    }




	DoCustomFunctionByBuffInCommonStateStart();
	ApplyDrainHpBuff();



    m_iStateInsertBuffFactorProgress = 0;
    CommonFrameMove_InsertBuffFactor( 0.f );


}

void CX2GUNPC::CommonFrameMoveHost()
{
	KTDXPROFILE();

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	++m_dwFrameMoveCountFuture;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	//m_AIMessage = SCT_NO_CHANGE;


    if( m_NPCFrameData.syncData.nowState != m_ImmuneSkillState )
	{
		if( GetNowMp() > GetMaxMp() * 0.7f )
		{
			for( const CX2SkillTree::SKILL_ID eSkillID : m_vecImmuneSkill )
			{
				if( true == g_pX2Game->IsThereAnyUserUnitUsingSkill( eSkillID ) )
				{
					StateChange( m_ImmuneSkillState, true );
					UpNowMp( -GetMaxMp() * 0.7f );
				}
			}
		}
	}




	if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f &&
		m_ExtraDamagePack.m_Frozen.m_fTime <= 0.f )
	{
		if( NULL != m_pAI &&
			true == GetAIEnabled() )
		{
			m_pAI->OnFrameMove( m_fTime, m_fElapsedTime );
		}
	}



	m_bAddForceDownGageThisState	= false;

	m_fLastDamagePassTime += m_fElapsedTime;
	if( m_fLastDamagePassTime >= 2.0f )
	{
		AddForceDownGage( -10.f * m_fElapsedTime );
	}


	CommonFrameMoveHost_ExtraDamage();


	if( m_iFollowHitter > 0 )
	{

		switch( m_eHitterUnitType )
		{
		case CX2GameUnit::GUT_USER:
			{

				CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_HitterUID );
				if( pCX2GUUser != NULL )
				{
					if( pCX2GUUser->GetIsRight() != pCX2GUUser->GetIsRightBefore() )
					{
						m_iFollowHitter--;
					}
				}

			} break;

		case CX2GameUnit::GUT_NPC:
			{

				//CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( (int)m_HitterUID );
				//if( pNPC != NULL )
				//{
				//	if( pNPC->GetIsRight() != pNPC->GetIsRightBefore() )
				//	{
				//		m_iFollowHitter--;
				//	}
				//}

			} break;
		}

	}


	//{{ dmlee 2009.1.17
	// 몬스터의 상태가 바뀌지 않으면 random seed값이 바뀌지 않아서 크리티컬이 연속해서 터지는 문제를 수정하기 위해
	// 일정시간 간격으로 random seed를 바꾼다. 그러나 패킷 전송을 해서 sync를 맞추지 않기 때문에 각 클라이언트에서 서로 다른 값을 갖게 될 수 있다.
	// "석화" 되었을 경우에만 동작하도록 수정
	if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
		m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
	{
		if( m_TimerRandomSeedSync.elapsed() > CX2GameUnit::RANDOM_SEED_RESET_TIME * 3.f )
		{
			m_TimerRandomSeedSync.restart();
		}
		else if( m_TimerRandomSeedSync.elapsed() > CX2GameUnit::RANDOM_SEED_RESET_TIME )
		{
			m_TimerRandomSeedSync.restart();

			const int MAGIC_NUMBER = 17;
			m_NPCFrameData.syncData.m_usRandomTableIndex	= ( m_NPCFrameData.syncData.m_usRandomTableIndex + 1000 ) % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT; // fix!! random number를 변경시키는 방식을 바꿔야??
		}
	}
	//}} dmlee 2009.1.17
}

void CX2GUNPC::CommonFrameMoveHost_ExtraDamage()
{
	KTDXPROFILE();



	if( true == IsImmuneToExtraDamage() )
		return;







	//특수데미지 체크
	if( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
	{
		if ( m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage >= 1.0f )
		{
			m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage = 0.0f;

			float fPrevNow = GetNowHp();
			UpNowHp( -m_ExtraDamagePack.m_EnchantPoison.m_DamagePerSec);

			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH
                || m_ExtraDamagePack.m_EnchantPoison.m_bAttackedByMyUnit == true )
			{
				D3DXVECTOR3 numPos;
				GetFramePos( &numPos, m_pFrame_Bip01_Head );
				if( numPos.x == 0.0f && numPos.y == 0.0f && numPos.z == 0.0f )
				{
					numPos = GetPos();
					numPos.y += 150.0f;
				}
				numPos.y += 75.0f;

				//{{ JHKang / 강정훈 / 2010/12/06
				if( NULL != g_pData->GetPicChar() && m_bDyingStart == false  && true == m_bDamageOutScreen )
					//}} JHKang / 강정훈 / 2010/12/06
				{
					g_pData->GetPicChar()->DrawText( static_cast<int>( fPrevNow - GetNowHp() ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		}
	}



	if( m_ExtraDamagePack.m_Poison.m_fTime > 0.0f )
	{
		const float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Poison.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

	if( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
	{
		if ( m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage >= 1.0f )
		{
			m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage = 0.0f;

			// oasis907 : 김상윤 [2010.10.5] // 속성 블레이즈로 사망 가능
			const float fBeforeHP = GetNowHp();
			UpNowHp(-m_ExtraDamagePack.m_EnchantBlaze.m_DamagePerSec);

			if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH
                || m_ExtraDamagePack.m_EnchantBlaze.m_bAttackedByMyUnit == true )
			{
				D3DXVECTOR3 numPos;
				GetFramePos( &numPos, m_pFrame_Bip01_Head );
				if( numPos.x == 0.0f && numPos.y == 0.0f && numPos.z == 0.0f )
				{
					numPos = GetPos();
					numPos.y += 150.0f;
				}
				numPos.y += 75.0f;


				//{{ JHKang / 강정훈 / 2010/12/06
				if( NULL != g_pData->GetPicChar() && m_bDyingStart == false  && true == m_bDamageOutScreen )
					//}} JHKang / 강정훈 / 2010/12/06
				{
					g_pData->GetPicChar()->DrawText( static_cast<int>( fBeforeHP - GetNowHp() ), numPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		}
	}

	if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f )
	{
		const float fBeforeHP = GetNowHp();
		UpNowHp(-1.0f * m_ExtraDamagePack.m_Fire.m_DamagePerSec * m_fElapsedTime );

		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

	if( m_ExtraDamagePack.m_Lightning.m_fTime > 0.0f )
	{
		//자기꺼 닳고
		const float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime );
		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

	if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f )
	{
		const float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Cold.m_DamagePerSec * m_fElapsedTime );
		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

	if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f )
	{
		FlushMp( m_ExtraDamagePack.m_Curse.m_DamagePerSec * m_fElapsedTime );
	}




	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.0f )
	{
		const float fBeforeHP = GetNowHp();
		UpNowHp( -1.0f * m_ExtraDamagePack.m_Frozen.m_DamagePerSec * m_fElapsedTime );
		if( fBeforeHP > 0.0f && GetNowHp() < 1.0f )
			SetNowHp( 1.0f );
	}

}

void CX2GUNPC::CommonFrameMove()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();

//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	++m_dwFrameMoveCountNow;
//#endif	SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	if ( NULL == m_pXSkinAnim )
		return;

	DoDelegateProcessInCommonFrameMove();

	float fAnimTime = -1.f;
	if( m_pXSkinAnim != NULL )
	{
		fAnimTime = m_pXSkinAnim->GetNowAnimationTime();
	}

	// 스크립트 상에서 특정 시간에 특정스피드를 지정해 줄수 있도록한다.
    if ( m_pXSkinAnim != NULL )
    {
        for( ;m_iSpeedFactorProgress < (int) nowStateData.m_vecSpeedFactor.size(); ++m_iSpeedFactorProgress )
        {
            const TIME_SPEED& timeSpeed = nowStateData.m_vecSpeedFactor[m_iSpeedFactorProgress];
            if ( timeSpeed.vSpeed.z > fAnimTime )
                break;
			switch(timeSpeed.iFlag)
			{
			case 1:
				m_PhysicParam.nowSpeed.x += timeSpeed.vSpeed.x;
				m_PhysicParam.nowSpeed.y += timeSpeed.vSpeed.y;
				break;
			case 2:
				m_PhysicParam.nowSpeed.x = timeSpeed.vSpeed.x;
				m_PhysicParam.nowSpeed.y = timeSpeed.vSpeed.y;
				break;
			default:
				break;
			}
        }
    }

    const std::vector<D3DXVECTOR2>& vecSuperArmorTime = nowStateData.m_vecSuperArmorTime;
    if ( vecSuperArmorTime.empty() == false )
    {
        bool bSet = false;
	    for(UINT iSuperArmorTime=0; iSuperArmorTime < vecSuperArmorTime.size(); ++iSuperArmorTime)
	    {
		    D3DXVECTOR2 superArmorTime = vecSuperArmorTime[iSuperArmorTime];

		    if( fAnimTime >= superArmorTime.x && fAnimTime <= superArmorTime.y )
		    {
			    m_NPCFrameData.stateParam.bSuperArmor = true;
                bSet = true;
			    break;
		    }
	    }
        if ( bSet == false )
        {
            m_NPCFrameData.stateParam.bSuperArmor = m_vecSuperArmorPtr.empty() == false || NOWSTATE_FLAG( SET_SUPER_ARMOR_VALUE ) == true;
        }
    }

	//if ( true == m_bGlobalSuperArmor && m_fGlobalTimeSuperArmor > 0 )
    if ( m_fGlobalTimeSuperArmor > 0 )
	{
		m_NPCFrameData.stateParam.bSuperArmor = true;
		m_fGlobalTimeSuperArmor -= m_fElapsedTime;
	}
	/// m_bStateSuperArmor 에 블리언 값을 대입하지 않고 바로 적용시키는 슈퍼아머인 경우, 해당 구문에서 모두 꺼져 버린다. 그래서 제거
	/*else
	{
		m_NPCFrameData.stateParam.bSuperArmor = m_bStateSuperArmor;
	}*/

	CommonFrameMove_Particle();

	//{{ JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)
	CommonFrameMove_DrainHPParticle();
	//}} JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)

	CommonFrameMove_DieParticle();
	CommonFrameMove_EffectSet( GetNowAnimationTime() );


    {
        float&  fReflectMagicX = m_v2ReflectMagic.x;
        float&  fReflectMagicY = m_v2ReflectMagic.y;

	    if( fReflectMagicX >= 0.f &&
		    fReflectMagicY > 0.f &&
		    GetNowAnimationTime() > fReflectMagicX )
	    {
		    fReflectMagicX = -1.f;
	    }
	    if( fReflectMagicX <= -1.f &&
		    fReflectMagicY > 0.f )
	    {
		    fReflectMagicY -= m_fElapsedTime;
		    if( fReflectMagicY < 0.f )
		    {
			    fReflectMagicX = 0.f;
			    fReflectMagicY = 0.f;
		    }
	    }
    }


	if( m_NPCFrameData.syncData.nowState == GUSI_DIE )
		return;

	if( m_hSeqDrainParticle != INVALID_PARTICLE_SEQUENCE_HANDLE &&
		null != m_optrLastGameUnitWhoAttackToMe &&
		CX2GameUnit::GUT_USER == m_optrLastGameUnitWhoAttackToMe->GetGameUnitType() )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDrainParticle );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 pos = m_optrLastGameUnitWhoAttackToMe->GetWeaponBonePos( 0, L"TRACE_END0", 0 );
			pSeq->SetBlackHolePosition( pos );
		}
		else
		{
			m_hSeqDrainParticle = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}

	if( m_fTimePauseNPCAI > 0.f )
	{
		m_fTimePauseNPCAI -= m_fElapsedTime;
		SetAIEnable( false );	// 스턴등이 걸렸을경우 스테이트 변경하지 못하도록 ai를 Off.

		if( m_fTimePauseNPCAI <= 0.f )
		{
			m_fTimePauseNPCAI = 0.f;
			SetAIEnable( true ); // 스턴등이 풀렸렸을경우 스테이트 변경할 수 있도록 ai를 On.
		}

	}

	// 배틀필드에서 방장이 바뀌는 경우 새로운 방장의 AI 에서 몬스터들이 현재의 위치를 목적지라고 알도록 하기위해 추가
	if ( false == IsLocalUnit() && CX2NPCAI::NAT_FIELD == m_pAI->GetAIType() )
	{
		CX2BattleFieldNpcAi* pBattleFieldNpcAi = static_cast<CX2BattleFieldNpcAi*>( m_pAI );
		pBattleFieldNpcAi->SetFinalDestinationPosAndLineMapIndex( GetPos(), GetLastTouchLineIndex() );
	}



//{{oasis:김상윤////2009-10-7////
	if(GetGroupAICommand_LUA() == CX2GUNPC::GAI_CMD_FORCED_MOVE)
	{
		SetCanPassUnit(true);
	}
//}}oasis:김상윤////2009-10-7////
	if(m_eRideState ==  CX2GUNPC::RS_TRY_TO_RIDE)
	{
		SetCanPassUnit(true);
	}

//#ifdef MONSTER_DIE_PARTICLE_TEST
//
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//    if( m_NPCFrameData.syncData.nowState == m_DyingLandFront || m_NPCFrameData.syncData.nowState == m_DyingLandBack || m_NPCFrameData.syncData.nowState == m_DyingSky )
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//	if( m_NPCStateData.stateID == m_DyingLandFront || m_NPCStateData.stateID == m_DyingLandBack || m_NPCStateData.stateID == m_DyingSky )
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//	{
//		if( m_bCreatedDieParticle == false &&
//			m_pXSkinAnim->IsAnimationEnd() == true )
//		{
//			m_bCreatedDieParticle = true;
//
//			//WCHAR wszParticleName[64] = L"";
//			//StringCchPrintfW( wszParticleName, ARRAY_SIZE(wszParticleName), L"Die_Flare_TEST%d", rand()%3 );
//			//CKTDGParticleSystem::CParticleEventSequence* pSeqDie = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  wszParticleName, GetPos() );
//
//			CKTDGParticleSystem::CParticleEventSequence* pSeqDie = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Die_Flare_TEST3", GetPos() );
//
//
//			if( NULL != pSeqDie )
//			{
//				pSeqDie->SetLandPosition( GetLandPosition_LUA().y );
//
//				std::vector<D3DXVECTOR3> vecVertices;
//				vecVertices.reserve( 256 );
//
//				//m_pXSkinAnim->UpdateAnimationFrameStructure();
//				m_pXSkinAnim->OnFrameMove( g_pKTDXApp->GetTime(), g_pKTDXApp->GetElapsedTime() );
//				m_pXSkinAnim->GetAnimXSkinMesh()->GetSurfaceVertices( vecVertices );
//
//				for( int i=0; i< (int) vecVertices.size(); i++ )
//				{
//					pSeqDie->CreateNewParticle( vecVertices[i] );
//				}
//
//
//				WCHAR wszText[64] = L"";
//				StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"X %d", (int) vecVertices.size() );
//				if( NULL != g_pData->GetPicCharGreen() )
//					g_pData->GetPicCharGreen()->DrawText( wszText, GetPos(), GetDirVector(), CKTDGPicChar::AT_CENTER );
//			}
//		}
//	}
//
//#endif MONSTER_DIE_PARTICLE_TEST
//




//#ifdef LINEMAP_FAST_WIND_TEST
//	if( true == m_bEnableUpsideWind )
//	{
//		if( true == m_bWaitInTheAir )
//		{
//			if( m_fTimeInTheAir > 0.f )
//			{
//				m_fTimeInTheAir -= m_fElapsedTime;
//			}
//			else if( m_fTimeInTheAir < 0.f )
//			{
//				m_fTimeInTheAir = 0.f;
//			}
//		}
//	}
//#endif LINEMAP_FAST_WIND_TEST




    for( int i = 0; i < 10; i++ )
    {
        m_aTimerList[i].OnFrameMove( m_fElapsedTime );
    }


	m_NPCFrameData.unitCondition.fStateTimeBack	= m_NPCFrameData.unitCondition.fStateTime;
	m_NPCFrameData.unitCondition.fStateTime		+= m_fElapsedTime;


    // 죽을때 날리기..
    if( m_SpeedDyingFly > 0 && m_bDyingStart == true && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true )
    {
        if(m_vDieFly.x > 0.1f)
            m_vDieFly.x -= 0.02f;
        else if(m_vDieFly.x < -0.1f)
            m_vDieFly.x += 0.02f;

        if(m_vDieFly.z > 0.1f)
            m_vDieFly.z -= 0.02f;
        else if(m_vDieFly.z < -0.1f)
            m_vDieFly.z += 0.02f;

        m_vDieFly.y -= (0.04f);

        D3DXVECTOR3 flyPos = m_pXSkinAnim->GetMatrix().GetPos() + (m_vDieFly * (m_fElapsedTime * m_SpeedDyingFly));

        m_NPCFrameData.unitCondition.dirDegree.y = m_flyAngle;
        //SetRotateDegree(D3DXVECTOR3(D3DXToDegree(acos(oriDir.x / flyDir.x)), D3DXToDegree(acos(oriDir.y / flyDir.y)), D3DXToDegree(acos(oriDir.z / flyDir.z))));

        //GetMatrix().Rotate( acos(oriDir.x / flyDir.x), acos(oriDir.y / flyDir.y), acos(oriDir.z / flyDir.z) ); //m_NPCFrameData.unitCondition.dirDegree );
        GetMatrix().Move( flyPos ); //m_NPCFrameData.syncData.position );

        //m_pXSkinAnim->GetMatrix()->Rotate( , acos(oriDir.y / flyDir.y), acos(oriDir.z / flyDir.z) );
        m_pXSkinAnim->GetMatrix().Move( flyPos );


    }
    else
    {

        D3DXVECTOR3 vPosBackup = GetPositionBackup();
        GetMatrix().Move( vPosBackup );

		{
			GetMatrix().RotateDegree( m_NPCFrameData.unitCondition.dirDegree );
		}

        m_pXSkinAnim->GetMatrix().Move( vPosBackup );

    }



	if( m_bForceBillBoard == false )
	{
		{
			m_pXSkinAnim->GetMatrix().RotateDegree( m_NPCFrameData.unitCondition.dirDegree );
			m_pXSkinAnim->GetMatrix().RotateRel( D3DXToRadian(m_AddRotateDegree.x), D3DXToRadian(m_AddRotateDegree.y), D3DXToRadian(m_AddRotateDegree.z) );
            m_pXSkinAnim->GetMatrix().RotateRel( nowStateData.m_vAddRotateRadian.x, nowStateData.m_vAddRotateRadian.y, nowStateData.m_vAddRotateRadian.z );
		}
	}
	else
	{
		m_pXSkinAnim->SetBillBoardType( CKTDGMatrix::BT_ALL );
	}




	if ( IsLocalUnit() == false )
	{
		//float fUpdateTime = 0.0f;
		//m_pXSkinAnim->SetIsCulled( !g_pKTDXApp->GetDGManager()->IsInFrustum( this, fUpdateTime ) );
        m_pXSkinAnim->SetIsCulled( !g_pKTDXApp->GetDGManager()->IsInFrustum( this ) );

		//m_pXSkinAnim->SetUpdatePassedNeedTime( fUpdateTime );
	}
	else
    {
		m_pXSkinAnim->SetIsCulled( false );
    }


	//ResetAnimSpeed();
    bool    bXSkinAnimOnFrameMoveCalled = false;

    if( m_fAnimWaitTime > m_fElapsedTime )
	{
        m_pXSkinAnim->OnFrameMove( m_fTime, 0.00000f );
        bXSkinAnimOnFrameMoveCalled = true;
		m_fAnimWaitTime -= m_fElapsedTime;
	}
	else
	{
        float   fAnimElapsedTime = m_fElapsedTime - m_fAnimWaitTime;
        m_fAnimWaitTime = 0.f;
        if ( fAnimElapsedTime < 0.f )
            fAnimElapsedTime = 0.f;
		// 퍼포먼스 때문에 배경몬스터처럼 움직임이 거의 없는 몬스터는 위치가 변하지 않는다면 skin anim 갱신을 하지 않는다
		switch( GetNPCTemplet().m_ClassType )
		{
		case CX2UnitManager::NCT_THING_WORLD_BLOCK:
			{
				if( false == m_bNeverMove ||
					CKTDGXSkinAnim::XAS_WAIT != m_pXSkinAnim->GetState() )
				{
                    m_pXSkinAnim->OnFrameMove( m_fTime, fAnimElapsedTime );
                    bXSkinAnimOnFrameMoveCalled = true;
				}
			} break;

		default:
			{
                m_pXSkinAnim->OnFrameMove( m_fTime, fAnimElapsedTime );
                bXSkinAnimOnFrameMoveCalled = true;
			} break;
		}

		m_fAnimWaitTime = 0.0f;
	}
    if ( bXSkinAnimOnFrameMoveCalled == false )
    {
        m_pXSkinAnim->UpdateBeforeAnimationTime();
    }


	if( m_fCoolTime > 0.0f )
	{
		m_fCoolTime -= m_fElapsedTime;
		if( m_fCoolTime <= 0.0f )
			m_fCoolTime = 0.0f;
	}


	if( m_fArmagedonBladeTime > 0.f )
	{
		m_fArmagedonBladeTime -= m_fElapsedTime;
		if( m_fArmagedonBladeTime <= 0.f )
		{
			m_fArmagedonBladeTime = 0.f;
			DeleteArmagedonBlade();
		}

		if( INVALID_MESH_INSTANCE_HANDLE != m_hArmagedonBlade &&
			m_pXSkinAnim != NULL )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hArmagedonBlade );
			if( NULL != pMeshInst )
			{
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
			}
			else
			{
				m_hArmagedonBlade = INVALID_MESH_INSTANCE_HANDLE;
			}
		}
	}



	if( m_NPCFrameData.stateParam.invincibleTime.m_fX != -1.0f
		&& m_NPCFrameData.stateParam.invincibleTime.m_fY != -1.0f )
	{
		if( m_NPCFrameData.stateParam.invincibleTime.m_fX <= m_pXSkinAnim->GetNowAnimationTime()
			&& m_NPCFrameData.stateParam.invincibleTime.m_fY > m_pXSkinAnim->GetNowAnimationTime() )
			m_NPCFrameData.stateParam.bInvincible = true;
		else
			m_NPCFrameData.stateParam.bInvincible = false;
	}
	if( m_fShowInvincibleTime > 0.0f )
	{
		m_fShowInvincibleSwapTime -= m_fElapsedTime;
		if( m_fShowInvincibleSwapTime <= 0.0f )
		{
			m_fShowInvincibleSwapTime	= 0.05f;
			m_bShowInvincibleWhite		= !m_bShowInvincibleWhite;
		}

	}
	if( m_fShowInvincibleTime > 0.f )
	{
        m_fShowInvincibleTime -= m_fElapsedTime;
		if( m_fShowInvincibleTime <= 0.0f )
		{
			m_fShowInvincibleTime = 0.0f;
		}
	}
	if( m_fForceInvincibleTime > 0.f )
	{
		m_fForceInvincibleTime -= m_fElapsedTime;
		if( m_fForceInvincibleTime <= 0.0f )
			m_fForceInvincibleTime = 0.0f;
	}

	if( m_fForceInvincibleTime > 0.0f )
	{
		m_NPCFrameData.stateParam.bInvincible = true;
	}

    {
        const D3DXVECTOR3& vDefenceTime = nowStateData.m_vDefenceTime;
        if ( vDefenceTime.x != -1.0f
            && vDefenceTime.y != -1.0f )
	    {
		    if( vDefenceTime.x <= m_pXSkinAnim->GetNowAnimationTime()
			    && vDefenceTime.y > m_pXSkinAnim->GetNowAnimationTime() )
		    {
			    SetDamageReducePercent( (int)vDefenceTime.z );
		    }
		    else
		    {
			    SetDamageReducePercent( 0 );
		    }
        }
    }

    {
        const D3DXVECTOR2&  v2AfterImage = nowStateData.m_v2AfterImageTime;
	    if( v2AfterImage.x != -1.0f &&
		    v2AfterImage.y != -1.0f )
	    {
		    if( v2AfterImage.x <= m_pXSkinAnim->GetNowAnimationTime() &&
			    v2AfterImage.y > m_pXSkinAnim->GetNowAnimationTime() )
			    EnableAfterImage();
		    else
			    DisableAfterImage();
	    }
    }

	for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		pWeapon->CommonFrameMoveForNPC( m_fTime, m_fElapsedTime );
	}

	for(UINT i=0; i<m_vecAttachMesh.size(); ++i)
	{
		CX2SubEquip *pSubEquip = m_vecAttachMesh[i];
		if( pSubEquip != NULL )
		{
			pSubEquip->OnFrameMove(m_fTime, m_fElapsedTime);
		}
	}

    for( std::vector<AttachedParticle*>::iterator iter = m_vecpAttachedParticle.begin();
        iter != m_vecpAttachedParticle.end(); )
	{
		AttachedParticle* pAttachedParticle = *iter;

        CKTDGParticleSystem::CParticleEventSequence* pSeq = pAttachedParticle->GetInstanceSequence();
		if( NULL == pSeq )
		{
			//{{ kimhc // 2010.6.21 // 메모리해제 없이 erase 하는 부분에 메모리 해제 추가
			SAFE_DELETE( pAttachedParticle );
			//}} kimhc // 2010.6.21 // 메모리해제 없이 erase 하는 부분에 메모리 해제 추가
            iter = EraseUnorderlyUsingSwap( m_vecpAttachedParticle, iter );
			continue;
		}

		D3DXVECTOR3 vBonePos = GetPos();
		if( pAttachedParticle->m_wstrBoneName.empty() == false )
		{
			vBonePos = GetBonePos( pAttachedParticle->m_wstrBoneName.c_str() );
		}
		vBonePos += pAttachedParticle->m_vOffsetPos;

		pSeq->SetPosition( vBonePos );
		pSeq->SetAddRotate( GetRotateDegree() );
		pSeq->SetAxisAngle( GetRotateDegree() );
        ++iter;
	}


	GetGageData()->OnFrameMove( m_fTime, m_fElapsedTime );



	CKTDGParticleSystem::CParticleEventSequence* pSeq_Poison		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPoisonBody );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Fire1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody1 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Fire2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody2 );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Curse 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCurseBody );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_SlowRight 	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_SlowLeft  	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyLeft  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold_Head		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold_Head );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold1L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold2L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold1R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1_Right  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Cold2R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2_Right  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Pepper1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody1  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Pepper2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPepperBody2  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Stun 			= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqStun  );





	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Stun				= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Stun_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun_Second  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Blaze_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Blaze_Second  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Frozen_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Frozen_Second  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Poison_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Poison_Second  );

	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation2		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation2  );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_Exclamation3		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation3  );
	CKTDGParticleSystem::CParticleEventSequence* pSeqQuestMonster = NULL;

	/// EDT 이모티콘
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Enchant_ArrowOfExplosion  = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_ArrowOfExplosion );

	/// 중첩수 이모티콘
	CKTDGParticleSystem::CParticleEventSequence* pSeq_StartOfDelayedFiringOverlap  = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hStartOfDelayedFiringOverlap );

    if(m_bDyingStart == false)
    {

	D3DXVECTOR3 poisonBodyEffPos = GetPos();
	poisonBodyEffPos.y += 100.0f;
	if( pSeq_Poison != NULL )
	{
		pSeq_Poison->SetPosition( poisonBodyEffPos );
	}

	if( pSeq_Curse != NULL )
	{
		pSeq_Curse->SetPosition( poisonBodyEffPos );
	}

	D3DXVECTOR3 vEmoticonPos = GetPos();
	GetFramePos( &vEmoticonPos, m_pFrame_Bip01_Head );
	vEmoticonPos.y += (m_fMindFlagHeight);

	D3DXVECTOR3 vOverlapEmoticonPos = GetPos();		/// 중첩된 EDT 횟수 표시 이모티콘 위치
	GetFramePos( &vOverlapEmoticonPos, m_pFrame_Bip01_Head );
	vOverlapEmoticonPos.y += (m_fMindFlagHeight + 60.f);
	vOverlapEmoticonPos += 50.f * m_NPCFrameData.unitCondition.dirVector;

	//D3DXVECTOR3 vEmoticonPos = GetPos() + D3DXVECTOR3( 0, 250.f + m_fMindFlagHeight, 0 ) - 40.f * GetZVector();
	//vEmoticonPos.y -= 90.f;

	if( NULL != pSeq_Cold_Head )
	{
		pSeq_Cold_Head->SetPosition( vEmoticonPos );
	}



	if( NULL != pSeq_Enchant_Stun )
	{
		pSeq_Enchant_Stun->SetPosition( vEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Exclamation )
	{
		pSeq_Enchant_Exclamation->SetPosition( vOverlapEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Exclamation2 )
	{
		pSeq_Enchant_Exclamation2->SetPosition( vOverlapEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Exclamation3 )
	{
		pSeq_Enchant_Exclamation3->SetPosition( vOverlapEmoticonPos );
	}

	if ( m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation >= 2 ||
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 2 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 2 )
	{
		if( m_hSeqEnchant_Exclamation3 == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
        {
			m_hSeqEnchant_Exclamation3 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Overlap_EDT_Emoticon_3", 0,0,0 );
                pSeq_Enchant_Exclamation3		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation3  );
        }


		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->SetEmitRate( 20, 20 );
		}


		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}
	}
	else if ( m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation >= 1 ||
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 1 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 1 )
	{
		if( m_hSeqEnchant_Exclamation2 == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
        {
			m_hSeqEnchant_Exclamation2 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Overlap_EDT_Emoticon_2", 0,0,0 );
                pSeq_Enchant_Exclamation2		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation2  );
        }


		if( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->SetEmitRate( 20, 20 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

	}
	else if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0 ||
		m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0 ||
		m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0  )
	{
		if( m_hSeqEnchant_Exclamation == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
        {
			m_hSeqEnchant_Exclamation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( static_cast<CKTDGObject*>(this),  L"Overlap_EDT_Emoticon_1", 0,0,0 );
                pSeq_Enchant_Exclamation		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Exclamation  );
        }


		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->SetEmitRate( 20, 20 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}
	}
	else
	{
		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}
	}

	if( NULL != pSeq_Enchant_Blaze_Second )
	{
		pSeq_Enchant_Blaze_Second->SetPosition( vEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Frozen_Second )
	{
		pSeq_Enchant_Frozen_Second->SetPosition( vEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Poison_Second )
	{
		pSeq_Enchant_Poison_Second->SetPosition( vEmoticonPos );
	}

	if( NULL != pSeq_Enchant_Stun_Second )
	{
		pSeq_Enchant_Stun_Second->SetPosition( vEmoticonPos );
	}
	/// mauntain : 김태환 [2012.05.21] 레나 2차 전직 나이트 와쳐 - 지연의 신호탄
	if( NULL != pSeq_Enchant_ArrowOfExplosion )			/// EDT 이모티콘
	{
		pSeq_Enchant_ArrowOfExplosion->SetPosition( vEmoticonPos );

		if( false == pSeq_Enchant_ArrowOfExplosion->GetShowObject() )
			pSeq_Enchant_ArrowOfExplosion->SetShowObject( true );
	}

	if( NULL != pSeq_StartOfDelayedFiringOverlap )		/// 중첩 이모티콘
	{
		pSeq_StartOfDelayedFiringOverlap->SetPosition( vOverlapEmoticonPos );

		if( false == pSeq_StartOfDelayedFiringOverlap->GetShowObject() )
			pSeq_StartOfDelayedFiringOverlap->SetShowObject( true );
	}


	D3DXVECTOR3 fireBodyEffPos1 = GetPos();
	fireBodyEffPos1.y += 120.0f;
	fireBodyEffPos1.x += 30.0f;
	if( pSeq_Fire1 != NULL )
		pSeq_Fire1->SetPosition( fireBodyEffPos1 );

	D3DXVECTOR3 fireBodyEffPos2 = GetPos();
	fireBodyEffPos2.y += 80.0f;
	fireBodyEffPos2.x -= 20.0f;
	if( pSeq_Fire2 != NULL )
		pSeq_Fire2->SetPosition( fireBodyEffPos2 );

	D3DXVECTOR3 slowBodyEffPos1 = GetPos();
	slowBodyEffPos1.y += 20.0f;
	slowBodyEffPos1.x += 30.0f;
	if( pSeq_SlowRight != NULL )
		pSeq_SlowRight->SetPosition( slowBodyEffPos1 );

	if( pSeq_Cold1L != NULL )
	{
		pSeq_Cold1L->SetPosition( slowBodyEffPos1 );
	}
	if( pSeq_Cold2L != NULL )
	{
		pSeq_Cold2L->SetPosition( slowBodyEffPos1 );
	}


	D3DXVECTOR3 slowBodyEffPos2 = GetPos();
	slowBodyEffPos2.y += 20.0f;
	slowBodyEffPos2.x -= 20.0f;
	if( pSeq_SlowRight != NULL )
		pSeq_SlowRight->SetPosition( slowBodyEffPos2 );

	if( pSeq_Cold1R != NULL )
	{
		pSeq_Cold1R->SetPosition( slowBodyEffPos2 );
	}
	if( pSeq_Cold2R != NULL )
	{
		pSeq_Cold2R->SetPosition( slowBodyEffPos2 );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq_StickShake = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqStickShake );
	if( pSeq_StickShake != NULL && g_pX2Game->GetMyUnit() != NULL)
	{
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( g_pX2Game->GetMyUnit()->GetPos() );
		projPos.y -= 40.0f;
		pSeq_StickShake->SetPosition( D3DXVECTOR3( projPos.x, projPos.y, 0.0f ) );
	}

	if( true == m_bIsQuestMonster )
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hSeqQuestMonster )
		{
			m_hSeqQuestMonster = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( static_cast<CKTDGObject*>(this),  L"QuestMonster", 0,0,0 ); // 퀘스트 몬스터 표시 이펙트
		}

		pSeqQuestMonster = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqQuestMonster );
		if( NULL != pSeqQuestMonster )
		{
			pSeqQuestMonster->SetShowObject(true);
			pSeqQuestMonster->SetEmitRate( 20, 40 );
			pSeqQuestMonster->SetPosition( D3DXVECTOR3(vEmoticonPos.x + 116, vEmoticonPos.y - 42, vEmoticonPos.z) );
		}
	}
	else
	{
		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hSeqQuestMonster )
		{
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqQuestMonster );
		}
	}

    }
    else if(m_bDieFly == true && m_bDyingStart == true && m_SpeedDyingFly > 0)
    {
        if(pSeq_Poison)         pSeq_Poison->SetShowObject(false);
        if(pSeq_Fire1)          pSeq_Fire1->SetShowObject(false);
        if(pSeq_Fire2)          pSeq_Fire2->SetShowObject(false);
        if(pSeq_Curse)          pSeq_Curse->SetShowObject(false);
        if(pSeq_SlowRight)      pSeq_SlowRight->SetShowObject(false);
        if(pSeq_SlowLeft)       pSeq_SlowLeft->SetShowObject(false);

        if(pSeq_Cold_Head)      pSeq_Cold_Head->SetShowObject(false);
        if(pSeq_Cold1L)         pSeq_Cold1L->SetShowObject(false);
        if(pSeq_Cold2L)         pSeq_Cold2L->SetShowObject(false);
        if(pSeq_Cold1R)         pSeq_Cold1R->SetShowObject(false);
        if(pSeq_Cold2R)         pSeq_Cold2R->SetShowObject(false);

        if(pSeq_Pepper1)        pSeq_Pepper1->SetShowObject(false);
        if(pSeq_Pepper2)        pSeq_Pepper2->SetShowObject(false);

        if(pSeq_Stun)           pSeq_Stun->SetShowObject(false);

        if(pSeq_Enchant_Stun)           pSeq_Enchant_Stun->SetShowObject(false);
        if(pSeq_Enchant_Exclamation)    pSeq_Enchant_Exclamation->SetShowObject(false);
		if(pSeq_Enchant_Blaze_Second)   pSeq_Enchant_Blaze_Second->SetShowObject(false);
        if(pSeq_Enchant_Frozen_Second)  pSeq_Enchant_Frozen_Second->SetShowObject(false);
        if(pSeq_Enchant_Poison_Second)  pSeq_Enchant_Poison_Second->SetShowObject(false);
        if(pSeq_Enchant_Stun_Second)    pSeq_Enchant_Stun_Second->SetShowObject(false);

        if(pSeq_Enchant_Exclamation2)   pSeq_Enchant_Exclamation2->SetShowObject(false);
        if(pSeq_Enchant_Exclamation3)   pSeq_Enchant_Exclamation3->SetShowObject(false);

		if(pSeq_Enchant_ArrowOfExplosion)		pSeq_Enchant_ArrowOfExplosion->SetShowObject(false);

		if(pSeq_StartOfDelayedFiringOverlap)	pSeq_StartOfDelayedFiringOverlap->SetShowObject(false);

        if ( m_hSeqAttribEnchant_Fire != INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
            CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqAttribEnchant_Fire );
            if( NULL != pSeq )
            {
                pSeq->SetShowObject(false);
            }
        }

        if ( m_hSeqAttribEnchant_Wind != INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
            CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqAttribEnchant_Wind );
            if( NULL != pSeq )
            {
                pSeq->SetShowObject(false);
            }
        }

		if ( m_hSeqSpecialAbility_Attack != INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Attack );
			if( NULL != pSeq )
			{
				pSeq->SetShowObject(false);
			}
			pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Attack_Background );
			if( NULL != pSeq )
			{
				pSeq->SetShowObject(false);
			}
		}

		if ( m_hSeqSpecialAbility_Defend != INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Defend );
			if( NULL != pSeq )
			{
				pSeq->SetShowObject(false);
			}
			pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Defend_Background );
			if( NULL != pSeq )
			{
				pSeq->SetShowObject(false);
			}
		}

        if( m_pUnitShadow != NULL )
            m_pUnitShadow->SetShowObject( false );


    }


	// 넘어졌을 때 강제다운 초기화
	if( m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageLandFront
		|| m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageLandBack

		|| m_NPCFrameData.syncData.nowState == (char)m_CommonState.m_DownDamageAir )
	{
		SetNowForceDown( 0.f );
	}

	if ( m_delegateProcessReturnFromScaleChange )
		m_delegateProcessReturnFromScaleChange();


	float fRunJumpSpeedRate = 1.0f;
	float fAnimSpeedRate = 1.f;


	if( m_ExtraDamagePack.m_Poison.m_fTime  > 0.0f || m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
	{
		if( m_hSeqPoisonBody == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqPoisonBody = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitPoison", 0,0,0 );
                pSeq_Poison		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqPoisonBody );
        }


		if( pSeq_Poison != NULL )
			pSeq_Poison->SetEmitRate( 10,20 );

		m_ExtraDamagePack.m_Poison.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Poison.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Poison.m_fTime = 0.0f;

		if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
		{

			if( m_hSeqEnchant_Poison_Second == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
            {
				m_hSeqEnchant_Poison_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Poison_Emoticon_Second", 0,0,0 );
                    pSeq_Enchant_Poison_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Poison_Second  );
            }

			if ( pSeq_Enchant_Poison_Second != NULL )
			{
				pSeq_Enchant_Poison_Second->SetEmitRate( 20, 40 );
			}

			const CX2EnchantItem::EnchantData* pEnchantData = g_pData->GetEnchantItem()->GetEnchantData( CX2DamageManager::EDT_ENCHANT_POISON );
			if ( pEnchantData != NULL )
			{
				if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation == 0 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_FirstSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					// oasis907 : 김상윤 [2010.10.5] // 속성 포이즌 이동속도 -> 속도 디버프로 변경
					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;
				}
				else if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation == 1 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_SecondSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantPoison.m_Accumulation >= 2 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_ThirdSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_NATURE );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;

					if ( fAnimSpeedRate > fApplySlowFact + fAlpha )
						fAnimSpeedRate = fApplySlowFact + fAlpha;


				}
				if( fAnimSpeedRate >= 1.f )
				{
					fAnimSpeedRate = 1.f;
				}
			}
		}
		else
		{


			if( pSeq_Enchant_Poison_Second != NULL )
			{
				pSeq_Enchant_Poison_Second->ClearAllParticle( );
				pSeq_Enchant_Poison_Second->SetEmitRate( 0 ,0 );
			}
		}

		m_ExtraDamagePack.m_EnchantPoison.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantPoison.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantPoison.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage += m_fElapsedTime;
	}
	else
	{
		if( pSeq_Poison != NULL )
			pSeq_Poison->SetEmitRate( 0,0 );



		if( pSeq_Enchant_Poison_Second != NULL )
		{
			pSeq_Enchant_Poison_Second->ClearAllParticle( );
			pSeq_Enchant_Poison_Second->SetEmitRate( 0 ,0 );
		}
	}

	if ( m_ExtraDamagePack.m_EnchantPoison.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantPoison.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantPoison.m_Accumulation = 0;
	}

	if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f || m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
	{
		if( m_hSeqFireBody1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
		    m_hSeqFireBody1 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
            pSeq_Fire1 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody1 );
        }

		if( m_hSeqFireBody2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqFireBody2 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"SwordFireTip", 0,0,0 );
            pSeq_Fire2 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqFireBody2 );
        }

		if( pSeq_Fire1 != NULL )
			pSeq_Fire1->SetEmitRate( 20,40 );
		if( pSeq_Fire2 != NULL )
			pSeq_Fire2->SetEmitRate( 20,40 );


		m_ExtraDamagePack.m_Fire.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Fire.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Fire.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantBlaze.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantBlaze.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantBlaze.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage += m_fElapsedTime;

		if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
		{
			if( m_hSeqEnchant_Blaze_Second == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
            {
				m_hSeqEnchant_Blaze_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Blaze_Emoticon_Second", 0,0,0 );
                pSeq_Enchant_Blaze_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Blaze_Second  );
            }

			if ( pSeq_Enchant_Blaze_Second != NULL )
			{
				pSeq_Enchant_Blaze_Second->SetEmitRate( 20, 40 );
			}

		}
		else
		{
			if ( pSeq_Enchant_Blaze_Second != NULL )
			{
				pSeq_Enchant_Blaze_Second->ClearAllParticle();
				pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
			}
		}
	}
	else
	{
		if( pSeq_Fire1 != NULL )
			pSeq_Fire1->SetEmitRate( 0,0 );
		if( pSeq_Fire2 != NULL )
			pSeq_Fire2->SetEmitRate( 0,0 );



		if ( pSeq_Enchant_Blaze_Second != NULL )
		{
			pSeq_Enchant_Blaze_Second->ClearAllParticle();
			pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
		}
	}

	if ( m_ExtraDamagePack.m_EnchantBlaze.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantBlaze.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantBlaze.m_Accumulation = 0;
	}

	if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.0f )
	{

		m_ExtraDamagePack.m_EnchantShock.m_Accumulation = 1;
		m_NPCFrameData.stateParam.bSuperArmor = true;
		m_ExtraDamagePack.m_EnchantShock.m_fTime -= m_fElapsedTime;

		/*
		if ( m_ExtraDamagePack.m_EnchantShock.m_Accumulation >= 1 )
		{
			if ( m_pPart_Enchant_Stun != NULL )
			{
				m_pPart_Enchant_Stun->ClearAllParticle();
				m_pPart_Enchant_Stun->SetEmitRate( 0, 0 );
			}

			if ( m_pPart_Enchant_Stun_Second != NULL )
			{
				m_pPart_Enchant_Stun_Second->SetEmitRate( 20, 40 );
			}
		}
		else
		{
			if ( m_pPart_Enchant_Stun_Second != NULL )
			{
				m_pPart_Enchant_Stun_Second->ClearAllParticle();
				m_pPart_Enchant_Stun_Second->SetEmitRate( 0, 0 );
			}

			if ( m_pPart_Enchant_Stun != NULL )
			{
				m_pPart_Enchant_Stun->SetEmitRate( 20, 40 );
			}
		}
		*/
		if( m_hSeqEnchant_Stun == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
        {
			m_hSeqEnchant_Stun = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Stun_Emoticon", 0,0,0 );
            pSeq_Enchant_Stun				= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Stun  );
        }

		if ( pSeq_Enchant_Stun != NULL )
		{
			pSeq_Enchant_Stun->SetEmitRate( 20, 40 );
		}


		if( m_ExtraDamagePack.m_EnchantShock.m_fTime < 0.0f )
		{
			m_ExtraDamagePack.m_EnchantShock.m_fTime = 0.0f;
			if ( m_ExtraDamagePack.m_EnchantShock.m_Accumulation == 1 )
			{
				if ( GetNowHp() > 0.f )
				{
					if ( m_pXSkinAnim != NULL )
					{
#ifdef BALANCE_PATCH_201103
						m_pXSkinAnim->SetState( CKTDGXSkinAnim::XAS_PLAYING );

#else
						m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
						if( IsPvpBot() == true )
						{
							StateChange( m_CommonState.m_DownDamageLandFront, true );
						}
#endif

					}
				}


				m_ExtraDamagePack.m_EnchantShock.m_Accumulation = 0;
			}
		}
		else
		{
			if( GetNowHp() > 0.f )
				AnimStop();
		}
	}
	else
	{
		if ( pSeq_Enchant_Stun != NULL )
		{
			pSeq_Enchant_Stun->ClearAllParticle();
			pSeq_Enchant_Stun->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Stun_Second != NULL )
		{
			pSeq_Enchant_Stun_Second->ClearAllParticle();
			pSeq_Enchant_Stun_Second->SetEmitRate( 0, 0 );
		}
	}


	if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f )
	{
        if( m_hSeqCurseBody == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
            m_hSeqCurseBody = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitCurse", 0,0,0 );
            pSeq_Curse 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCurseBody );
        }


		if( pSeq_Curse != NULL )
			pSeq_Curse->SetEmitRate( 10,20 );


		m_ExtraDamagePack.m_Curse.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Curse.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Curse.m_fTime = 0.0f;
	}
	else
	{
		if( pSeq_Curse != NULL )
			pSeq_Curse->SetEmitRate( 0,0 );
	}



	if( m_ExtraDamagePack.m_Slow.m_fTime > 0.0f )
	{
		if( m_hSeqSlowBodyRight == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqSlowBodyRight = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitSlow", 0,0,0 );
            pSeq_SlowRight 	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyRight );
        }
		if( m_hSeqSlowBodyLeft == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqSlowBodyLeft = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitSlow", 0,0,0 );
            pSeq_SlowLeft  	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqSlowBodyLeft  );
        }

		if( pSeq_SlowRight != NULL )
			pSeq_SlowRight->SetEmitRate( 5,10 );
		if( pSeq_SlowLeft != NULL )
			pSeq_SlowLeft->SetEmitRate( 5,10 );


		m_ExtraDamagePack.m_Slow.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Slow.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Slow.m_fTime = 0.0f;
	}
	else
	{
		if( pSeq_SlowRight  != NULL )
			pSeq_SlowRight ->SetEmitRate( 0,0 );
		if( pSeq_SlowLeft   != NULL )
			pSeq_SlowLeft  ->SetEmitRate( 0,0 );
	}

	if( m_ExtraDamagePack.m_Cold.m_fTime  > 0.0f || m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.0f )
	{
		if ( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f )
		{
			if(m_ExtraDamagePack.m_Cold.m_iLevel < 0)
			{
				//감소율 = 0.2
				if ( fAnimSpeedRate > 0.8f )
					fAnimSpeedRate = 0.8f;
			}
			else
			{
				//감소율 = 0.2 + ( 0.0025 * EDT레벨 )
				float fAnimSpeedReduceRate = 0.2f + ( 0.0025f * (float)m_ExtraDamagePack.m_Cold.m_iLevel );
				if( fAnimSpeedRate > 1.f - fAnimSpeedReduceRate)
				{
					fAnimSpeedRate = 1.f - fAnimSpeedReduceRate;
				}
				if( fAnimSpeedRate >= 1.f )
				{
					fAnimSpeedRate = 1.f;
				}
			}

			if( false == m_bColdSlowApplied )
			{
				m_bColdSlowApplied = true;
			}
		}
		else
		{
			if( true == m_bColdSlowApplied )
			{
				m_bColdSlowApplied = false;
			}
		}


		if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0 )
		{
			m_bFrozenSlowApplied = true;

			const CX2EnchantItem::EnchantData* pEnchantData = g_pData->GetEnchantItem()->GetEnchantData( CX2DamageManager::EDT_ENCHANT_FROZEN );
			if ( pEnchantData != NULL )
			{
				if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation == 0 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_FirstSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;



					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation == 1 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_SecondSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;



					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;


				}
				else if ( m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation >= 2 )
				{
					float fApplySlowFact = 1.0f - ((float)pEnchantData->m_ThirdSlowPercent / 100.0f );

					float fAlpha = 1.0f - fApplySlowFact;

					float fResist = GetEnchantResist( CX2EnchantItem::ET_WATER );
					fAlpha = fAlpha * fResist / CX2EnchantItem::EAR_MAX_VALUE;


					if ( fRunJumpSpeedRate > fApplySlowFact + fAlpha )
						fRunJumpSpeedRate = fApplySlowFact + fAlpha;
				}
			}
		}

		if( m_hSeqCold1 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold1 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error01", 0, 0, 0 );
            pSeq_Cold1L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1  );
        }
		if( m_hSeqCold2 == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold2 = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error02", 0, 0, 0 );
            pSeq_Cold2L  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2  );
        }
		if( m_hSeqCold1_Right == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold1_Right = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error01", 0, 0, 0 );
            pSeq_Cold1R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold1_Right  );
        }
		if( m_hSeqCold2_Right == INVALID_PARTICLE_SEQUENCE_HANDLE )
        {
			m_hSeqCold2_Right = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ice_Status_Error02", 0, 0, 0 );
            pSeq_Cold2R  		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold2_Right  );
        }



		if( pSeq_Cold1L != NULL )
		{
			pSeq_Cold1L->SetEmitRate( 20,30 );
		}
		if( pSeq_Cold2L != NULL )
		{
			pSeq_Cold2L->SetEmitRate( 10,20 );
		}
		if( pSeq_Cold1R != NULL )
		{
			pSeq_Cold1R->SetEmitRate( 20,30 );
		}
		if( pSeq_Cold2R != NULL )
		{
			pSeq_Cold2R->SetEmitRate( 10,20 );
		}


		if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f )
		{
			if( m_hSeqCold_Head == INVALID_PARTICLE_SEQUENCE_HANDLE )
            {
				m_hSeqCold_Head = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"UnitCold", 0, 0, 0 );
                pSeq_Cold_Head		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqCold_Head );
            }

			if( pSeq_Cold_Head != NULL )
				pSeq_Cold_Head->SetEmitRate( 10, 20 );
		}
		else
		{
			if( pSeq_Cold_Head != NULL )
			{
				pSeq_Cold_Head->ClearAllParticle();
				pSeq_Cold_Head->SetEmitRate( 0, 0 );
			}
		}


		if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0 )
		{
			if( m_hSeqEnchant_Frozen_Second == INVALID_PARTICLE_SEQUENCE_HANDLE && m_bAttributeEmotionFlag == true )
            {
				m_hSeqEnchant_Frozen_Second = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Enchant_Frozen_Emoticon_Second", 0,0,0 );
                pSeq_Enchant_Frozen_Second		= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqEnchant_Frozen_Second  );
            }


			if ( pSeq_Enchant_Frozen_Second != NULL )
			{
				pSeq_Enchant_Frozen_Second->SetEmitRate( 20, 40 );
			}
		}
		else
		{
			if( pSeq_Enchant_Frozen_Second != NULL )
			{
				pSeq_Enchant_Frozen_Second->ClearAllParticle();
				pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
			}
		}

		m_ExtraDamagePack.m_Cold.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Cold.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Cold.m_fTime = 0.0f;

		m_ExtraDamagePack.m_EnchantFrozen.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_EnchantFrozen.m_fTime < 0.0f )
			m_ExtraDamagePack.m_EnchantFrozen.m_fTime = 0.0f;

	}
	else
	{
		if( true == m_bColdSlowApplied )
		{
			m_bColdSlowApplied = false;
		}

		if ( m_bFrozenSlowApplied == true )
		{
			m_bFrozenSlowApplied = false;
		}

		if( pSeq_Cold1L != NULL )
			pSeq_Cold1L->SetEmitRate( 0,0 );
		if( pSeq_Cold2L != NULL )
			pSeq_Cold2L->SetEmitRate( 0,0 );
		if( pSeq_Cold1R != NULL )
			pSeq_Cold1R->SetEmitRate( 0,0 );
		if( pSeq_Cold2R != NULL )
			pSeq_Cold2R->SetEmitRate( 0,0 );
		if( pSeq_Cold_Head != NULL )
		{
			pSeq_Cold_Head->ClearAllParticle();
			pSeq_Cold_Head->SetEmitRate( 0, 0 );
		}



		if( pSeq_Enchant_Frozen_Second != NULL )
		{
			pSeq_Enchant_Frozen_Second->ClearAllParticle();
			pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
		}
	}

	if( m_ExtraDamagePack.m_Aging.m_fTime > 0.f )
	{
		fAnimSpeedRate *= m_ExtraDamagePack.m_Aging.m_fAnimSpeedRate;
		fRunJumpSpeedRate *= m_ExtraDamagePack.m_Aging.m_fRunJumpRate;
	}

	if ( m_ExtraDamagePack.m_EnchantFrozen.m_fTime <= 0.0f )
	{
		m_ExtraDamagePack.m_EnchantFrozen.m_fElapsedTimeToEffectDamage = 0.0f;
		m_ExtraDamagePack.m_EnchantFrozen.m_Accumulation = 0;
	}

	if( m_ExtraDamagePack.m_Lightning.m_fTime > 0.0f )
	{
		//근거리 아군도 닳는다
		D3DXVECTOR3 pos = GetPos();
		for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
			if( pUnit != NULL
				&& pUnit != this
				&& pUnit->IsLocalUnit() == true
				&& pUnit->GetTeam() == GetTeam()
				&& pUnit->GetInvincible() == false
				&& pUnit->GetNowHp() > 0.0f
				&& GetDistance( pos, pUnit->GetPos() ) < 200.0f )
			{

				float fBeforeHP = pUnit->GetNowHp();

				//pUnit->GetGageManager()->GetHPGage()->fNow -= m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime;
				float fNowHp = pUnit->GetNowHp() - m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime;
				SetNowHp(fNowHp);

				if( fBeforeHP > 0.0f && pUnit->GetNowHp() < 1.0f )
				{
					pUnit->SetNowHp( 1.f );
				}

				*m_fDamageHistory += (m_ExtraDamagePack.m_Lightning.m_DamagePerSec * m_fElapsedTime);


			}
		}

		m_ExtraDamagePack.m_Lightning.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Lightning.m_fTime < 0.0f )
			m_ExtraDamagePack.m_Lightning.m_fTime = 0.0f;
	}


	// mauntain : 김태환 [2012.05.21] 레나 2차 전직 나이트 와쳐 - 지연의 신호탄
	if ( false == GetStartOfDelayedFiringData().empty() )	//자신에게 박힌 폭발의 화살이 있다면, 이모티콘 유지
	{
		if ( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 20, 40 );
		}

		if ( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 20, 40 );
		}
	}
	else
	{
		if( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->ClearAllParticle();
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 0, 0 );
		}

		if( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->ClearAllParticle();
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 0, 0 );
		}
	}

	CommonFrameMoveStateAbnormalityStartOfDelayedFiring();

	CommonFrameMoveStateAbnormality();

	//BBT_ATTACK_IMPOSSIBLE 설정되어있으면 AI 끄도록 변경

	if( false == m_vecAttackImpossiblePtr.empty() ||
		false == m_vecSkillImpossiblePtr.empty() ||
		false == m_vecZXImpossiblePtr.empty() )
	{
		SetTimePauseNPCAI( 0.2f );
	}

	if( m_vecStunPtr.empty() == false ||
		m_vecFreezePtr.empty() == false )
	{
		if( true == GetAIEnabled() )
		{
			SetTimePauseNPCAI( 0.1f );
		}
	}

	if( m_ExtraDamagePack.m_StigmaDebuff.m_fTime > 0.f )
	{
		fRunJumpSpeedRate *= m_ExtraDamagePack.m_StigmaDebuff.m_fRunJumpRate;
	}

	// 이동속도 변화 적용
	if( false == IsSamef( m_fRunJumpSpeedRate, fRunJumpSpeedRate ) )
		m_fRunJumpSpeedRate = fRunJumpSpeedRate;

	if( g_pX2Game != NULL && g_pX2Game->GetLineMap() != NULL )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
		if( pLineData != NULL && pLineData->m_bUnderWater == true && GetPos().y + GetUnitHeight(true) <= pLineData->m_fWaterHeight )
		{
			//fAnimSpeedRate *= pLineData->m_fAnimSpeed;

			m_AdditionalAnimSpeed.SetPassiveMultiple( 1.0f );	/// Fix: 어떻게 할지 생각해보자

			ChangeUnderWater(true);
			//m_bUnderWaterHead = true;
			float fMinY = min( pLineData->startPos.y, pLineData->endPos.y );
			float fColorRate = (pLineData->m_fWaterHeight - GetPos().y /*+ GetUnitHeight(true)*/) / (pLineData->m_fWaterHeight - fMinY);
			m_cLineUnitColor = D3DXCOLOR( pLineData->m_cLineColor.r * fColorRate, pLineData->m_cLineColor.g * fColorRate, pLineData->m_cLineColor.b * fColorRate, 0.f );
		}
		else if( pLineData != NULL && pLineData->m_bForceChangeColor == true )
		{
			m_bForceChagneColor = true;
			m_cLineUnitColor = D3DXCOLOR( pLineData->m_cLineColor.r, pLineData->m_cLineColor.g, pLineData->m_cLineColor.b, 0.f );
		}
		else
		{
			/// 옵션값 0.0f로 초기화
			m_AdditionalAnimSpeed.SetOptionValue( 0.0f );
			ChangeUnderWater(false);
			//m_bUnderWaterHead = false;

			m_bForceChagneColor = false;
			m_cLineUnitColor = D3DXCOLOR( 0.f, 0.f, 0.f, 0.f );
		}
	}

	// 애니메이션 속도 변화 적용
// 	if( m_fAnimSpeedRate != fAnimSpeedRate )
// 	{
// 		m_fAnimSpeedRate = fAnimSpeedRate;
// 		ResetAnimSpeed();
// 	}

	if( GetNowHp() <= 0.0f )
	{
		for(int iSound=0; iSound<(int)m_vecLoopSound.size(); ++iSound)
		{
			CKTDXDeviceSound *pSound = m_vecLoopSound[iSound];
			if( pSound != NULL )
			{
				pSound->Stop();
			}
		}
		m_vecLoopSound.clear();

		if( pSeq_Cold_Head != NULL )
		{
			pSeq_Cold_Head->ClearAllParticle();
			pSeq_Cold_Head->SetEmitRate( 0,0 );
		}

		if ( pSeq_Enchant_Stun != NULL )
		{
			pSeq_Enchant_Stun->ClearAllParticle();
			pSeq_Enchant_Stun->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation != NULL )
		{
			pSeq_Enchant_Exclamation->ClearAllParticle();
			pSeq_Enchant_Exclamation->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation2 != NULL )
		{
			pSeq_Enchant_Exclamation2->ClearAllParticle();
			pSeq_Enchant_Exclamation2->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Exclamation3 != NULL )
		{
			pSeq_Enchant_Exclamation3->ClearAllParticle();
			pSeq_Enchant_Exclamation3->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Frozen_Second != NULL )
		{
			pSeq_Enchant_Frozen_Second->ClearAllParticle();
			pSeq_Enchant_Frozen_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Blaze_Second != NULL )
		{
			pSeq_Enchant_Blaze_Second->ClearAllParticle();
			pSeq_Enchant_Blaze_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Poison_Second != NULL )
		{
			pSeq_Enchant_Poison_Second->ClearAllParticle();
			pSeq_Enchant_Poison_Second->SetEmitRate( 0, 0 );
		}

		if ( pSeq_Enchant_Stun_Second != NULL )
		{
			pSeq_Enchant_Stun_Second->ClearAllParticle();
			pSeq_Enchant_Stun_Second->SetEmitRate( 0, 0 );
		}

		if( pSeqQuestMonster != NULL)
		{
			pSeqQuestMonster->ClearAllParticle();
			pSeqQuestMonster->SetEmitRate( 0, 0 );
		}

		// mauntain : 김태환 [2012.05.21] 레나 2차 전직 나이트 와쳐 - 지연의 신호탄
		if ( pSeq_Enchant_ArrowOfExplosion != NULL )
		{
			pSeq_Enchant_ArrowOfExplosion->ClearAllParticle();
			pSeq_Enchant_ArrowOfExplosion->SetEmitRate( 0, 0 );
		}

		if ( pSeq_StartOfDelayedFiringOverlap != NULL )
		{
			pSeq_StartOfDelayedFiringOverlap->ClearAllParticle();
			pSeq_StartOfDelayedFiringOverlap->SetEmitRate( 0, 0 );
		}

	}

	//if( m_bNeverMove == false )
	{

		//{{AFX
		{
			PhysicProcess();
		}
		//}}AFX





		//임시 땜빵 코드
		if ( GetRidingNPCUID() != 0 )
		{
			CX2GUNPC* pRidingNPC = g_pX2Game->GetNPCUnitByUID( GetRidingNPCUID() );
			if ( pRidingNPC != NULL )
			{
				D3DXVECTOR3 bonePos = pRidingNPC->GetBonePos_LUA( "Bip01_Spine" );
                SetIsRightNow( pRidingNPC->GetIsRight() );
				m_NPCFrameData.syncData.position = bonePos;
			}
		}




		if( GetRideState() !=  CX2GUNPC::RS_NOT_AVAILABLE )
		{
			if( GetRideState() ==  CX2GUNPC::RS_READY )
			{
				if( GetRideType() == CX2GUNPC::RT_RIDER )
				{
					CallNearOneNPC((CX2UnitManager::NPC_UNIT_ID)m_RideTargetNPCID, m_fSearchRadius);
				}
				else
				{

				}
			}
			else if( GetRideState() ==  CX2GUNPC::RS_TRY_TO_RIDE )
			{
				if( GetRideType() == CX2GUNPC::RT_RIDER )
				{
					if(TryToRideTargetNPC() == true)
					{
						return;
					}
				}
				else
				{
					if( GetAIEnabled() == true )
					{
						SetAIEnable( false );
					}
				}
			}
			else // if(m_eRideState ==  CX2GUNPC::RS_ON_RIDING)
			{

			}
		}



		if( GetRideState() == RS_ON_RIDING )
		{
			if( GetRideType() == RT_RIDER ) // 라이더
			{
				//CX2GUNPC* pRidingNPC = g_pX2Game->GetNPCUnitByUID( m_RideTargetNPCUid );

				CX2GUNPC* pRidingNPC = GetRideUnit();

				if ( pRidingNPC != NULL )
				{
					D3DXVECTOR3 BonePos = pRidingNPC->GetBonePos_LUA( "Dummy_Rider" );
					//m_NPCFrameData.syncData.bIsRight = pRidingNPC->GetIsRight();


					const CX2GUNPC::NPCFrameData& ridingFrameData = pRidingNPC->GetNPCFrameData();
					m_NPCFrameData.syncData.lastTouchLineIndex = ridingFrameData.syncData.lastTouchLineIndex;
					m_NPCFrameData.syncData.position = BonePos;
				}
			}
			else // 라이딩 몬스터
			{
				CX2GUNPC* pRiderNPC = GetRideUnit();

				if ( pRiderNPC != NULL )
				{
					if( GetAIEnabled() == true )
					{
						SetAIEnable( false );
					}
				}
			}
		}

		if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f )
		{
			if( m_PhysicParam.passiveSpeed.x != -1.0f )
			{
				m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;// * m_PhysicParam.fSpeedFactor;
			}
			if( m_PhysicParam.passiveSpeed.y != -1.0f )
			{
				m_PhysicParam.nowSpeed.y = m_PhysicParam.passiveSpeed.y;// * m_PhysicParam.fSpeedFactor;
			}
		}
	}

	m_RenderParam.texOffsetStage0.x += m_TexStage0Speed.x * m_fElapsedTime;
	m_RenderParam.texOffsetStage0.y += m_TexStage0Speed.y * m_fElapsedTime;
	m_RenderParam.texOffsetStage1.x += m_TexStage1Speed.x * m_fElapsedTime;
	m_RenderParam.texOffsetStage1.y += m_TexStage1Speed.y * m_fElapsedTime;
	m_RenderParam.texOffsetStage2.x += m_TexStage2Speed.x * m_fElapsedTime;
	m_RenderParam.texOffsetStage2.y += m_TexStage2Speed.y * m_fElapsedTime;

	if( m_DamageData.bReAttack == true )
	{
		for( CX2DamageManager::HitUnitVector::iterator iter = m_DamageData.hitUnitList.begin();
            iter != m_DamageData.hitUnitList.end(); )
		{
			CX2DamageManager::HitUnit* pHitUnit = &(*iter);
			pHitUnit->fRemainGap -= m_fElapsedTime;
			if( pHitUnit->fRemainGap <= 0.0f )
			{
                iter = EraseUnorderlyUsingSwap( m_DamageData.hitUnitList, iter );
			}
            else
            {
                ++iter;
            }
		}
	}

	m_DamageLightTime -= m_fElapsedTime;
	if( m_DamageLightTime < 0.0f )
		m_DamageLightTime = 0.0f;

	if( g_pMain->GetGameOption().GetOptionList().m_eEffect == CX2GameOption::OL_HIGH )
	{
		if( m_pUnitShadow != NULL )
		{
			m_pUnitShadow->SetShowObject( true );
			m_pUnitShadow->UpdatePosition( m_NPCFrameData.syncData.position, m_NPCFrameData.unitCondition.landPosition, m_NPCFrameData.unitCondition.dirDegree );
		}
	}
	else
	{
		if( m_pUnitShadow != NULL )
			m_pUnitShadow->SetShowObject( false );
	}






	if( m_pAfterImage != NULL )
	{
		m_pAfterImage->OnFrameMove( m_fTime, m_fElapsedTime );
	}


	D3DXVECTOR3 markerPos;
	GetFramePos( &markerPos, m_pFrame_Bip01_Head );
	markerPos.y += m_fMindFlagHeight;






	/* 여기다가 집어넣어 볼까 */

	if ( m_hSeqAttribEnchant_Fire != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqAttribEnchant_Fire );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( m_NPCFrameData.unitCondition.landPosition );

			float fWidth = GetNPCFrameData().unitCondition.fUnitWidth;
			float fScale = fWidth / 80.0f;
			if ( fScale <= 1.0f )
				fScale = 1.0f;
			pSeq->SetScaleFactor( D3DXVECTOR3( fScale, fScale, fScale ) );

			//기본 80
		}
	}

	if ( m_hSeqAttribEnchant_Wind != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqAttribEnchant_Wind );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 headBonePos = GetBonePos( GetHeadBoneName().c_str() );
			D3DXVECTOR3 dirVec = headBonePos - g_pX2Game->GetX2Camera()->GetCamera().GetEye();
			D3DXVec3Normalize( &dirVec, &dirVec );
			dirVec.x = dirVec.x + 25;
			dirVec.y = dirVec.y + 25;
			dirVec.z = dirVec.z + 25;
			headBonePos = headBonePos + ( dirVec );
			pSeq->SetPosition( headBonePos );

			float fWidth = GetNPCFrameData().unitCondition.fUnitWidth;
			float fScale = fWidth / 80.0f;
			if ( fScale <= 1.0f )
				fScale = 1.0f;
			pSeq->SetScaleFactor( D3DXVECTOR3( fScale, fScale, fScale ) );


			//기본 80
		}
	}

	if ( m_hSeqSpecialAbility_Attack != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		D3DXVECTOR3 headBonePos = GetBonePos( GetHeadBoneName().c_str() );
		headBonePos.y += 140;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Attack );
		if( NULL != pSeq )
		{
			D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( headBonePos );
			pSeq->SetPosition( D3DXVECTOR3( finalPos.x, finalPos.y, 0 ) );
            /*
			float fWidth = GetUnitWidth(true);
			float fScale = fWidth / 80.0f;
			if ( fScale <= 1.0f )
				fScale = 1.0f;
			pSeq->SetScaleFactor( D3DXVECTOR3( fScale, fScale, fScale ) );
*/
		}

		pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Attack_Background );
		if( NULL != pSeq )
		{
			D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( headBonePos );
			pSeq->SetPosition( D3DXVECTOR3( finalPos.x, finalPos.y, 0 ) );

		}
	}

	if ( m_hSeqSpecialAbility_Defend != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		D3DXVECTOR3 headBonePos = GetBonePos( GetHeadBoneName().c_str() );
		headBonePos.y += 140;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Defend );
		if( NULL != pSeq )
		{
			D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( headBonePos );
			pSeq->SetPosition( D3DXVECTOR3( finalPos.x, finalPos.y, 0 ) );
/*
			float fWidth = GetUnitWidth(true);
			float fScale = fWidth / 80.0f;
			if ( fScale <= 1.0f )
				fScale = 1.0f;
			pSeq->SetScaleFactor( D3DXVECTOR3( fScale, fScale, fScale ) );
*/
		}

		pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqSpecialAbility_Defend_Background );
		if( NULL != pSeq )
		{
			D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( headBonePos );
			pSeq->SetPosition( D3DXVECTOR3( finalPos.x, finalPos.y, 0 ) );

		}
	}




	//if( m_pStandUpAttackMark != NULL && m_pStandUpAttackMark->GetLiveParticleNum() > 0 )
	//	m_pStandUpAttackMark->SetPosition( markerPos );

	if( m_NPCFrameData.stateParam.bInvincible == true || IsSuperArmor() )
		m_FlyCount = 0;



    if ( NOWSTATE_FLAG( DYING_END ) == true )
	{
		if( false == m_bSelfResurrection || m_iSelfResurrectionCount <= 0
             || m_eNPCUnitDieState != eNPCUnitDieState_NONE
            )
		{
			if( m_pXSkinAnim->IsAnimationEnd() == true )
			{
				//{{ mauntain : 김태환 [2012.06.14] 몬스터 카드 소환 기능 - 몬스터 사망시 소환 User의 소환 정보 초기화

					if( NCT_MONSTER_CARD == GetNPCCreateType() && NULL != GetOwnerGameUnit() )
					{
						CX2GUUser* pUser = static_cast<CX2GUUser*>( GetOwnerGameUnit() );
						if ( false == IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE0 ) &&
							false == IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE1 ) &&
							false == IsUnitID( CX2UnitManager::NUI_SHASHA_STAGE2 ) )
							pUser->AccessSummonMonsterCardData().init();		//해당 몬스터 사망시( 혹은 소환 유지 시간 종료 시 ) 소환 유저가 가지고 있는 소환 NPC 정보 초기화
					}

				//}}
					if( CX2UnitManager::NUI_SI_HA_FERDINAND == GetNPCTemplet().m_nNPCUnitID && NULL != GetOwnerGameUnit() )
					{
						CX2GUUser* pUser = static_cast<CX2GUUser*>( GetOwnerGameUnit() );
						if( NULL != pUser && CX2Unit::UC_EVE_CODE_EMPRESS == pUser->GetUnitClass() )
						{
							pUser->ResetLinkOverChargeIllusion();
						}
					}
				SetRemainHyperModeTime( 0.0f );
#ifndef DIE_NPC_TEST
				SetAlphaObject( true );
				m_RenderParam.bAlphaBlend	= true;

				if( m_RenderParam.color.a > 0.0f )
				{
					m_RenderParam.color.a	-= 1.5f * m_fElapsedTime;
				}
				else
				{
                    if ( GetShowObject() )
					{
                        SetShowObject( false );
						SetAlphaObject( false );
						m_RenderParam.bAlphaBlend = false;
					}
				}
#endif
				if( NULL != g_pX2Game &&
					CX2Game::GT_BATTLE_FIELD == g_pX2Game->GetGameType() &&
					true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
				{
					CX2GageManager::GetInstance()->ShowBossGageUIByUIDAndNotShowOtherBossGageUI( GetUnitUID(), false );
				}
			}
		}
	}

    else if ( NOWSTATE_FLAG( ESCAPE_END ) == true )
	{
		if( m_pXSkinAnim->IsAnimationEnd() == true )
		{
			SetRemainHyperModeTime( 0.0f );

			SetAlphaObject( true );
			m_RenderParam.bAlphaBlend	= true;

			if( m_RenderParam.color.a > 0.0f )
			{
				m_RenderParam.color.a	-= 1.5f * m_fElapsedTime;
			}
			else
			{
				if ( GetShowObject() )
				{
					SetShowObject( false );
					SetAlphaObject( false );
					m_RenderParam.bAlphaBlend = false;
				}
			}
		}
	}

//#ifdef MAKE_GGOBONG_TEST
//	m_fTimeGgobong -= m_fElapsedTime;
//	if ( m_fTimeGgobong <= 0.0f )
//	{
//		if ( m_bOneTimeCheckGgobong == false )
//		{
//			m_pAI->SetAITargetNPC( false );
//			m_pAI->SetTargetUnitType( CX2GameUnit::GUT_USER );
//			m_bOneTimeCheckGgobong = true;
//			m_TeamNum = CX2Room::TN_MONSTER;
//			m_MasterUserUnitUID = -1;
//		}
//
//		m_bGgobong = false;
//		m_fTimeGgobong = 0.0f;
//
//	}
//	else
//	{
//		m_TeamNum = CX2Room::TN_RED;
//		m_pAI->SetTargetUnitType( CX2GameUnit::GUT_NPC );
//	}
//#endif //MAKE_GGOBONG_TEST



	CommonFrameMove_MindFlagParticle();
	CommonFrameMove_Rage();
	CommonFrameMove_TimedEvent();
	CommonFrameMove_Invincible();


	if ( GetIsAlwaysSuperArmor() == true )
	{
		m_NPCFrameData.stateParam.bSuperArmor = true;
	}

	if( m_iFollowHitter > 0 )
	{
		switch( m_eHitterUnitType )
		{

		case CX2GameUnit::GUT_USER:
			{
				CX2GUUser* pCX2GUUser = g_pX2Game->GetUserUnitByUID( m_HitterUID );
				if( pCX2GUUser != NULL &&
					pCX2GUUser->GetNowHp() > 0.f )
				{
					m_NPCFrameData.syncData.lastTouchLineIndex	= pCX2GUUser->GetSyncData().lastTouchLineIndex;
					m_NPCFrameData.unitCondition.landPosition	= pCX2GUUser->GetUnitCondition().landPosition;

					//위치 업데이트////////////////////////////////////////////////////////////////

					m_NPCFrameData.syncData.position			= pCX2GUUser->GetPos();
					m_NPCFrameData.unitCondition.dirDegree		= pCX2GUUser->GetUnitCondition().dirDegree;
					m_NPCFrameData.unitCondition.dirVector		= pCX2GUUser->GetUnitCondition().dirVector;
					m_NPCFrameData.unitCondition.bFootOnLine	= pCX2GUUser->GetUnitCondition().bFootOnLine;

					//{{ JHKang / 강정훈 / 2010/12/15 / 몬스터를 캐릭터 얼굴 앞에 붙이기
                    if ( NOWSTATE_FLAG( ATTACK_FOLLOW_HITTER_POS_FRONT ) == true )
					{
						m_NPCFrameData.unitCondition.dirDegree.y += 180.f;

						if( pCX2GUUser->GetIsRight() ==true )
							m_NPCFrameData.syncData.position += 50.0f * pCX2GUUser->GetUnitCondition().dirVector;
						else
							m_NPCFrameData.syncData.position -= 50.0f * pCX2GUUser->GetUnitCondition().dirVector;
					}
					else
					{
						if( pCX2GUUser->GetIsRight() ==false )
							m_NPCFrameData.syncData.position += 70.0f * pCX2GUUser->GetUnitCondition().dirVector;
						else
							m_NPCFrameData.syncData.position -= 70.0f * pCX2GUUser->GetUnitCondition().dirVector;
					}
					//}} JHKang / 강정훈 / 2010/12/15 / 몬스터를 캐릭터 얼굴 앞에 붙이기
					m_PhysicParam.nowSpeed.x = 0.0f;
					m_PhysicParam.nowSpeed.y = 0.0f;
				}
				else
				{
					m_iFollowHitter = 0;
					m_bFollowHitterLost = true;
				}
			} break;


		case CX2GameUnit::GUT_NPC:
			{
				CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( (int)m_HitterUID );
				if( pNPC != NULL &&
					pNPC->GetNowHp() > 0.f )
				{
					m_NPCFrameData.syncData.lastTouchLineIndex	= pNPC->GetNPCSyncData().lastTouchLineIndex;
					m_NPCFrameData.unitCondition.landPosition	= pNPC->GetUnitCondition().landPosition;

					//위치 업데이트////////////////////////////////////////////////////////////////

					m_NPCFrameData.syncData.position			= pNPC->GetPos();
					m_NPCFrameData.unitCondition.dirDegree		= pNPC->GetUnitCondition().dirDegree;
					m_NPCFrameData.unitCondition.dirVector		= pNPC->GetUnitCondition().dirVector;
					m_NPCFrameData.unitCondition.bFootOnLine	= pNPC->GetUnitCondition().bFootOnLine;

					if( pNPC->GetIsRight() == false )
						m_NPCFrameData.syncData.position += 70.0f * pNPC->GetUnitCondition().dirVector;
					else
						m_NPCFrameData.syncData.position -= 70.0f * pNPC->GetUnitCondition().dirVector;

					m_PhysicParam.nowSpeed.x = 0.0f;
					m_PhysicParam.nowSpeed.y = 0.0f;
				}
				else
				{
					m_iFollowHitter = 0;
					m_bFollowHitterLost = true;
				}
			} break;
		}

	}


	if( m_bNeverMove == true )
	{
		m_NPCFrameData.syncData.position = m_ORGPos;

		//모션 이동
        if ( NOWSTATE_FLAG( APPLY_ANIM_MOVE ) == true )
		{

			D3DXVECTOR3 vTranslationX = GetDirVector() * m_pXSkinAnim->GetMotionOffset().x;
			D3DXVECTOR3 vTranslationZ = GetZVector() * m_pXSkinAnim->GetMotionOffset().z;
			vTranslationX.y = 0.f;
			vTranslationZ.y = 0.f;

			if( m_NPCFrameData.syncData.bIsRight == true )
			{
				m_NPCFrameData.syncData.position += vTranslationX + vTranslationZ;

			}
			else
			{
				m_NPCFrameData.syncData.position -= vTranslationX + vTranslationZ;
			}
		}
	}

	if ( !m_vecNeverMovePtr.empty() && NULL != m_vecNeverMovePtr.back() )
		m_NPCFrameData.syncData.position = m_vecNeverMovePtr.back()->GetPos();

	if( true == m_bEntangled )
	{
		//m_NPCFrameData.syncData.position = m_ORGPos;
		m_NPCFrameData.syncData.position = m_vEntangledPosition;
	}

	g_pX2Game->GetNPCFunc()->CommonFrameMoveFunc( this );
    // added by wonpok.
    if ( m_iLua_CommonFrameMoveRefID != LUA_REFNIL )
    {
        lua_tinker::call<void>( GetFunctionLuaState(),  m_LuaManager.GetLuaGeneralRef(), m_iLua_CommonFrameMoveRefID, g_pKTDXApp, g_pX2Game, this );
    }


	g_pX2Game->GetNPCFunc()->FrameMoveFunc( this );
    int iLua_FrameMoveRefID = GetNowStateRuntime().m_iLua_FrameMoveRefID;
	if( iLua_FrameMoveRefID != LUA_REFNIL )
	{

		lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), iLua_FrameMoveRefID, g_pKTDXApp, g_pX2Game, this );
	}
//	if( false == GetNowStateData().m_strLua_FrameMove.empty() )
//	{
//		string func;
//		ConvertWCHARToChar( func, GetNowStateData().m_strLua_FrameMove.c_str() );
//#ifdef LEAVE_LAST_ERROR_LOG_TEST
//		LastErrorLog( func.c_str() );
//#endif LEAVE_LAST_ERROR_LOG_TEST
////#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		lua_tinker::call<void>( GetFunctionLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
////		lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	}


	if(m_bHold == true)
	{
//{{ kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
		if ( m_vecHoldUserUID.empty() == false )
		{
//			vector<UidType> vecUnHoldUserUID;

			for( UidType& val : m_vecHoldUserUID )
			{
				CX2GUUser* pHoldUser = g_pX2Game->GetUserUnitByUID( val );

				if ( pHoldUser != NULL )
				{
					D3DXVECTOR3 vHoldPos = GetBonePos( m_wstrHoldBone.c_str() ) + m_vHoldOffSet;	/// 본 위치에 OffSet 추가

					pHoldUser->SetPos( vHoldPos );
				}
// 				else
// 					back_inserter( vecUnHoldUserUID ) = val;
			}
		}
//}} kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
	}

	FollowTargetUnitBone_FrameMove();



	if ( GetUserGrap() == true )
	{
		if ( !m_vecGameUnitoPtr.empty() )
		{
			for( CX2GameUnitoPtr oPtr : m_vecGameUnitoPtr )
			{
				if ( null != oPtr && oPtr->GetGameUnitType() == CX2GameUnit::GUT_USER )
				{
					CX2GUUser* pGrappledUser = static_cast<CX2GUUser*>( oPtr.GetObservable() );
					pGrappledUser->SetGrap( true );
					pGrappledUser->SetPos( pGrappledUser->GetGrappedPosition() );
				}
			}
		}
	}


	if( m_iDangerAlarm >= 0 )
	{
		if( g_pX2Game->GetShowAlarm(m_iDangerAlarm) == false && g_pX2Game->GetAlarmDelayTime(m_iDangerAlarm) <= GetStateTime() )
		{
			g_pX2Game->ShowDangerAlarm( m_iDangerAlarm );
			m_iDangerAlarm = -1;
		}
	}

	DamageDataChangeProcess();
	DoCustomFunctionByBuffInCommonFrameMove();
}




// enchant frozen 이 아니에요~
/*virtual*/ void CX2GUNPC::CommonFrameMoveStateAbnormalityFrozen()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
	{
		m_ExtraDamagePack.m_Frozen.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_Frozen.m_fTime < 0.0f )
		{
			m_ExtraDamagePack.m_Frozen.m_fTime = 0.0f;

			if( GetNowHp() > 0.f )
			{
				if ( m_pXSkinAnim != NULL )
				{
					m_pXSkinAnim->SetState( CKTDGXSkinAnim::XAS_PLAYING );
					if( IsPvpBot() == true )
					{
						StateChange( m_CommonState.m_DownDamageLandFront );
					}
					if( m_bDoneBackupSpeed == true )
					{
						m_bDoneBackupSpeed = false;
						m_PhysicParam.nowSpeed		= m_vNowSpeedBackup;
						m_PhysicParam.passiveSpeed	= m_vpassiveSpeedBackup;
					}

				}
			}
		}
	}





	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
	{
		if( INVALID_EFFECTSET_HANDLE == m_hEffSetStateAbnormalityFrozen )
		{
			m_hEffSetStateAbnormalityFrozen = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_State_Abnormality_Frozen", this ); /*, NULL, false, -1.f, m_ExtraDamagePack.m_Frozen.m_fTime );*/
		}

		//if( INVALID_EFFECTSET_HANDLE != m_hEffSetStateAbnormalityFrozen )
		//{
		//	CX2EffectSet::EffectSetInstance* pEffInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( m_hEffSetStateAbnormalityFrozen );
		//	if( NULL != pEffInst )
		//	{
		//		pEffInst->m_fLifeTime = m_ExtraDamagePack.m_Frozen.m_fTime;
		//	}
		//
		//}
	}
	else
	{
		if( INVALID_EFFECTSET_HANDLE != m_hEffSetStateAbnormalityFrozen )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffSetStateAbnormalityFrozen );
		}
	}

}





//{{ lucidash : 김상훈 [2010.1.4]
void CX2GUNPC::CommonFrameMoveStateAbnormalityWaterHold()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_WaterHold.m_fTime > 0.f )
	{
		m_ExtraDamagePack.m_WaterHold.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_WaterHold.m_fTime < 0.0f )
		{
			m_ExtraDamagePack.m_WaterHold.m_fTime = 0.0f;

			m_NPCFrameData.stateParam.bSuperArmor = true;
			m_NPCFrameData.stateParam.bSuperArmorNotRed = true;

			if( GetNowHp() > 0.f )
			{
				if ( m_pXSkinAnim != NULL )
				{
					m_pXSkinAnim->SetState( CKTDGXSkinAnim::XAS_PLAYING );

					if( m_bDoneBackupSpeed == true )
					{
						m_bDoneBackupSpeed = false;
						m_PhysicParam.nowSpeed		= m_vNowSpeedBackup;
						m_PhysicParam.passiveSpeed	= m_vpassiveSpeedBackup;
					}
				}
			}
		}
	}
}
//}} lucidash : 김상훈 [2010.1.4]

void CX2GUNPC::CommonFrameMoveStateAbnormalityFlashBang()
{
	KTDXPROFILE();

	if( m_ExtraDamagePack.m_FlashBang.m_fTime > 0.f )
	{
		m_ExtraDamagePack.m_FlashBang.m_fTime -= m_fElapsedTime;
		if( m_ExtraDamagePack.m_FlashBang.m_fTime < 0.0f )
		{
			m_ExtraDamagePack.m_FlashBang.m_fTime = 0.0f;
		}
	}
}

void CX2GUNPC::CommonFrameMove_TimedEvent()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();

    {
        const std::vector<AnimSpeed>&    vecAnimSpeed = nowStateData.m_vecAnimSpeed;
	    for( UINT i=0; i<vecAnimSpeed.size(); i++ )
	    {
		    const AnimSpeed& animSpeed = vecAnimSpeed[i];
		    if( true == m_pXSkinAnim->EventTimer( animSpeed.m_fEndTime ) )
		    {
			    m_fTimedEventAnimSpeed = 0.0f;
 			    ResetAnimSpeed();
		    }
		    else if( true == m_pXSkinAnim->EventTimer( animSpeed.m_fStartTime ) )
		    {
			    m_fTimedEventAnimSpeed = animSpeed.m_fAnimSpeed;
 			    ResetAnimSpeed();
		    }
	    }
    }


	//이펙트 데이타
	//파티클
    vector<ParticleEffectData*>::iterator iterPNext;
	for( vector<ParticleEffectData*>::iterator iter = m_ParticleEffData.begin();
        iter != m_ParticleEffData.end();
        iter = iterPNext )
	{
        iterPNext = iter;    ++iterPNext;

		ParticleEffectData* pParticleEffectData = *iter;
		if( pParticleEffectData->m_bUse == false &&
			pParticleEffectData->m_hSeq == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			if( m_pXSkinAnim->EventTimer( pParticleEffectData->m_fTime ) == true )
			{
				//파티클 생성
				pParticleEffectData->m_bUse = true;
				{
					D3DXVECTOR3 pos;
					if( pParticleEffectData->m_bWeapon == true )
					{
						pos = GetWeaponBonePos(0, pParticleEffectData->m_Pos.c_str(), 0 ); // fix!!! pParticleEffectData에 weapon index를 받도록 수정해야
					}
					else
					{
						if( pParticleEffectData->m_Pos.length() == 0 )
						{
							pos = GetPos();
						}
						else
						{
							pos = GetBonePos( pParticleEffectData->m_Pos.c_str() );
						}
					}

					// offset
					if( true == GetIsRight() )
					{
						pos += GetDirVector() * pParticleEffectData->m_vOffset.x;
						pos.y += pParticleEffectData->m_vOffset.y;
						pos += GetZVector() * pParticleEffectData->m_vOffset.z;
					}
					else
					{
						pos -= GetDirVector() * pParticleEffectData->m_vOffset.x;
						pos.y += pParticleEffectData->m_vOffset.y;
						pos -= GetZVector() * pParticleEffectData->m_vOffset.z;
					}
                    CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticleEffectData->CreateInstanceSequence( this, pos );

					// rotation
					D3DXVECTOR3 vRot(0, 0, 0);
					if( true == pParticleEffectData->m_bApplyUnitRotation )
					{
						vRot = GetRotateDegree();
					}
					vRot += pParticleEffectData->m_vRotation;

					if( NULL != pSeq )
					{
						pSeq->SetAddRotate( vRot );
						pSeq->SetAxisAngle( vRot );

						if(pParticleEffectData->m_bUnitScale == true)
						{
							CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( pParticleEffectData->m_Pos.c_str() );
							float sx = pFrame->baseMatrix._11;
							float sy = pFrame->baseMatrix._22;
							float sz = pFrame->baseMatrix._33;

							pParticleEffectData->m_vScale = D3DXVECTOR3(sx, sy, sz);
						}

						pSeq->SetScaleFactor(pParticleEffectData->m_vScale); // added by wonpok

					}

				}
			}
		}
		//메인터넌스
		if( pParticleEffectData->m_hSeq != INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			{
                CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticleEffectData->GetInstanceSequence();
                if ( pSeq == NULL )
				{
					pParticleEffectData->m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
                    ParticleEffectData::Destroy( pParticleEffectData );

                    iterPNext = EraseUnorderlyUsingSwap( m_ParticleEffData, iter );
				}
				else
				{
					if( true == pParticleEffectData->m_bTrace )
					{
						D3DXVECTOR3 pos;
						if( pParticleEffectData->m_bWeapon == true )
						{
							pos = GetWeaponBonePos(0, pParticleEffectData->m_Pos.c_str(), 0);
						}
						else
						{
							if( pParticleEffectData->m_Pos.length() == 0 )
							{
								pos = GetPos();
							}
							else
							{
								pos = GetBonePos( pParticleEffectData->m_Pos.c_str() );
							}
						}

						// offset
						if( true == GetIsRight() )
						{
							pos += GetDirVector() * pParticleEffectData->m_vOffset.x;
							pos.y += pParticleEffectData->m_vOffset.y;
							pos += GetZVector() * pParticleEffectData->m_vOffset.z;
						}
						else
						{
							pos -= GetDirVector() * pParticleEffectData->m_vOffset.x;
							pos.y += pParticleEffectData->m_vOffset.y;
							pos -= GetZVector() * pParticleEffectData->m_vOffset.z;
						}
						if( NULL != pSeq && pParticleEffectData->m_bUnitScale == true)
						{
							CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( pParticleEffectData->m_Pos.c_str() );
							float sx = pFrame->baseMatrix._11;
							float sy = pFrame->baseMatrix._22;
							float sz = pFrame->baseMatrix._33;

							pParticleEffectData->m_vScale = D3DXVECTOR3(sx, sy, sz);
							pSeq->SetScaleFactor(pParticleEffectData->m_vScale);
						}

						if( NULL != pSeq )
						{
							pSeq->SetPosition( pos );
						}


						// rotation
						D3DXVECTOR3 vRot(0, 0, 0);
						if( true == pParticleEffectData->m_bApplyUnitRotation )
						{
							vRot = GetRotateDegree();
						}
						vRot += pParticleEffectData->m_vRotation;

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( vRot );
							pSeq->SetAxisAngle( vRot );
						}
					}
				}
			}
		}
	}

	//메시 플레이어
    vector<MeshPlayerEffectData*>::iterator iterMNext;
	for( vector<MeshPlayerEffectData*>::iterator iter = m_MeshPlayerEffData.begin();
        iter != m_MeshPlayerEffData.end();
        iter = iterMNext )
	{
        iterMNext = iter;   ++iterMNext;
		MeshPlayerEffectData* pMeshPlayerEffectData = *iter;

		if( pMeshPlayerEffectData->m_bUse == false && pMeshPlayerEffectData->m_hMeshInst == INVALID_MESH_INSTANCE_HANDLE )
		{
			if( m_pXSkinAnim->EventTimer( pMeshPlayerEffectData->m_fTime ) == true )
			{
				// 생성
				pMeshPlayerEffectData->m_bUse = true;
				{
					D3DXVECTOR3 pos;
					if( pMeshPlayerEffectData->m_bWeapon == true )
					{
						pos = GetWeaponBonePos(0, pMeshPlayerEffectData->m_Pos.c_str(), 0 );
					}
					else
					{
						pos = GetBonePos( pMeshPlayerEffectData->m_Pos.c_str() );
					}
                    pMeshPlayerEffectData->CreateMeshInstance( this, pos, GetRotateDegree() );
				}
			}
		}

		//메인터넌스
		if( pMeshPlayerEffectData->m_hMeshInst != INVALID_MESH_INSTANCE_HANDLE && pMeshPlayerEffectData->m_Trace == true )
		{
			{
                CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pMeshPlayerEffectData->GetMeshInstance();
				if( NULL == pMeshInst )
				{
					pMeshPlayerEffectData->m_hMeshInst = INVALID_MESH_INSTANCE_HANDLE;
                    MeshPlayerEffectData::Destroy( pMeshPlayerEffectData );
                    iterMNext = EraseUnorderlyUsingSwap( m_MeshPlayerEffData, iter );
				}
				else
				{
					D3DXVECTOR3 pos;
					if( pMeshPlayerEffectData->m_bWeapon == true )
					{
						pos = GetWeaponBonePos(0, pMeshPlayerEffectData->m_Pos.c_str(), 0 );
					}
					else
					{
						pos = GetBonePos( pMeshPlayerEffectData->m_Pos.c_str() );
					}
					pMeshInst->SetPos( pos );
					pMeshInst->SetRotateDegree( GetRotateDegree() );
					pMeshInst->SetMoveAxisAngleDegree( GetRotateDegree() );
				}
			}
		}
	}


	for( ; m_iCameraCrashProgress < (int) nowStateData.m_vecCameraCrash.size(); m_iCameraCrashProgress++ )
	{
		const CameraCrashData& cameraCrashData = nowStateData.m_vecCameraCrash[ m_iCameraCrashProgress ];
		if( m_pXSkinAnim->EventTimer( cameraCrashData.m_fTime ) == true )
		{
			if( cameraCrashData.m_bOnlyFocus == true )
			{
				UpDownCrashCameraNoReset( cameraCrashData.m_fGap, cameraCrashData.m_CrashTime );
			}
			else
			{
				UpDownCrashCameraNoReset( cameraCrashData.m_fGap, cameraCrashData.m_CrashTime );
			}
        }
        else
        {
            break;
        }//if.. else..
	}


	for( int i = 0; i < (int)m_EventIntervalList.size(); i++ )
	{
        D3DXVECTOR3* pIntervel = &m_EventIntervalList[i].first;
		pIntervel->y += m_fElapsedTime;
		if( pIntervel->x < pIntervel->y )
		{
			pIntervel->z = 1.0f;
			pIntervel->y -= pIntervel->x;
            if ( pIntervel->y > pIntervel->x )
                pIntervel->y = pIntervel->x;
            m_EventIntervalList[i].second++;
		}
		else
		{
			pIntervel->z = 0.0f;
		}
	}

	//공격 체크
    for( int i = 0; i < (int)nowStateData.m_vecAttackTime.size(); i++ )
	{
        const KAttackTimeProjSeqName& kAttackTime = nowStateData.m_vecAttackTime[ i ];
        const D3DXVECTOR2& attackTime = kAttackTime.m_v2AttackTime;
		if( m_pXSkinAnim->GetNowAnimationTime() > attackTime.x && m_pXSkinAnim->GetNowAnimationTime() < attackTime.y )
		{
			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData,
                NOWSTATE_FLAG( ATTACK_FOLLOW_HITTER ),
                m_HitterUID ) == true )
			{
				if( m_NPCFrameData.unitCondition.bHit == false && GetShowSmallGageAndName() )
				{
					D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( m_DamageData.impactPoint );
					projPos.y -= 50.0f;
					projPos.x += 50.0f;
					g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,
                        kAttackTime.m_wstrProjSeqName.c_str(),
                        projPos.x, projPos.y, 0.0f,	100, 100, -1, 1 );
				}
				m_NPCFrameData.unitCondition.bHit = true;
			}
		}
	}

    {
        const std::vector<D3DXVECTOR3>& vecStopAllList = nowStateData.m_vecStopAllList;
	    for( const D3DXVECTOR3& stopTime : vecStopAllList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    g_pX2Game->StopAllUnit( stopTime.y / 2.0f, &m_NPCFrameData.syncData.position, 2000.0f, this, stopTime.z );
			    g_pX2Game->StopAllUnit( stopTime.y, &m_NPCFrameData.syncData.position, 1300.0f, this, stopTime.z );
		    }
	    }
        const std::vector<D3DXVECTOR3>& vecStop2AllList = nowStateData.m_vecStop2AllList;
	    for( const D3DXVECTOR3& stopTime : vecStop2AllList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    g_pX2Game->Stop2AllUnit( stopTime.y / 2.0f, &m_NPCFrameData.syncData.position, 2000.0f, this, stopTime.z );
			    g_pX2Game->Stop2AllUnit( stopTime.y, &m_NPCFrameData.syncData.position, 1300.0f, this, stopTime.z );
		    }
	    }
        const std::vector<D3DXVECTOR2>& vecStopOtherList = nowStateData.m_vecStopOtherList;
	    for( const D3DXVECTOR2& stopTime : vecStopOtherList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    g_pX2Game->StopOtherUnit( this, stopTime.y / 2.0f, &m_NPCFrameData.syncData.position, 2000.0f );
			    g_pX2Game->StopOtherUnit( this, stopTime.y, &m_NPCFrameData.syncData.position, 1300.0f );
		    }
	    }
        const std::vector<D3DXVECTOR2>& vecStop2OtherList = nowStateData.m_vecStop2OtherList;
	    for( const D3DXVECTOR2& stopTime : vecStop2OtherList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    g_pX2Game->Stop2OtherUnit( this, stopTime.y / 2.0f, &m_NPCFrameData.syncData.position, 2000.0f );
			    g_pX2Game->Stop2OtherUnit( this, stopTime.y, &m_NPCFrameData.syncData.position, 1300.0f );
		    }
	    }
        const std::vector<D3DXVECTOR2>& vecStopMyList = nowStateData.m_vecStopMyList;
	    for( const D3DXVECTOR2& stopTime : vecStopMyList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    SetStopTime( stopTime.y );
		    }
	    }
        const std::vector<D3DXVECTOR2>& vecStop2MyList = nowStateData.m_vecStop2MyList;
	    for( const D3DXVECTOR2& stopTime : vecStop2MyList )
	    {
            if( AnimEventTimer_LUA( stopTime.x ) == true )
		    {
			    SetStop2Time( stopTime.y );
		    }
	    }
    }

    {
        const std::vector<SoundPlayData>& vecSoundPlayData = nowStateData.m_avecSoundPlayData[
            ( m_eSoundSetType >= ESoundSetType(0) && m_eSoundSetType < SOUND_SET_NUMS ) ?  m_eSoundSetType : 0 ];
	    for( const SoundPlayData& soundPlayData : vecSoundPlayData )
	    {
		    float fTime = soundPlayData.m_SoundPlayTime;
		    int iRate	= soundPlayData.m_SoundPlayRate;
            if( AnimEventTimer_LUA( fTime ) == true )
		    {
			    if( rand()%100 < iRate )
			    {
				    CKTDXDeviceSound* pSound = PlaySound( soundPlayData.m_SoundPlayName.c_str(), soundPlayData.m_b3DSound );
				    if( NULL != pSound &&
					    -1 != soundPlayData.m_fMaxSoundDistance )
				    {
					    pSound->SetMax3DDistance( soundPlayData.m_fMaxSoundDistance );
				    }
			    }
		    }
	    }
    }
	for( int i=0; i< (int)m_vecSoundPlayLoop.size(); i++ )
	{
		bool bExist = false;
		for(int j=0; j<(int)m_vecLoopSound.size(); ++j)
		{
			CKTDXDeviceSound *pSound = m_vecLoopSound[j];
			if( pSound != NULL && pSound->GetDeviceID() == m_vecSoundPlayLoop[i].m_SoundPlayName )
			{
				bExist = true;
				break;
			}
		}

		if( bExist == false )
		{
			float fTime = m_vecSoundPlayLoop[i].m_SoundPlayTime;
			int iRate	= m_vecSoundPlayLoop[i].m_SoundPlayRate;
            if( AnimEventTimer_LUA( fTime ) == true )
			{
				if( rand()%100 < iRate )
				{
					CKTDXDeviceSound* pSound = PlaySound( m_vecSoundPlayLoop[i].m_SoundPlayName.c_str(), m_vecSoundPlayLoop[i].m_b3DSound, true );
					if( NULL != pSound &&
						-1 != m_vecSoundPlayLoop[i].m_fMaxSoundDistance )
					{
						pSound->SetMax3DDistance( m_vecSoundPlayLoop[i].m_fMaxSoundDistance );
					}

					m_vecLoopSound.push_back( pSound );
				}
			}
		}
	}

    if ( m_pXSkinAnim != NULL )
        CommonFrameMove_InsertBuffFactor( m_pXSkinAnim->GetNowAnimationTime() );
}

void CX2GUNPC::CommonFrameMove_ExtraDamage()
{

}



void CX2GUNPC::CommonFrameMove_Rage()
{
	KTDXPROFILE();
	// oasis907 : 김상윤 [2010.4.22] //
	if( m_fRageTime > 0.0f )
	{
		m_NPCFrameData.stateParam.bSuperArmor = true;


		m_fRageTime -= m_fElapsedTime;
		if( m_fRageTime < 0.0f )
		{
			m_fRageTime = 0.0f;

			m_bRageModeColorToRed = true;
			m_RageModeColor = D3DXCOLOR( 1, 1, 1, 1 );
			m_fRageModeColorChangeTime = 0.0f;
		}
		else
		{
			// rage 상태일 때 붉은색이 반짝반짝하게
			m_fRageModeColorChangeTime += m_fElapsedTime;

			float fChangeTime = 0.5f;
			float fRemainTime = fChangeTime - m_fRageModeColorChangeTime;

			if ( m_bRageModeColorToRed == true )
			{
				D3DXCOLOR remainColor = D3DXCOLOR( 1, 0, 0, 1 ) - m_RageModeColor;
				m_RageModeColor += (remainColor / fRemainTime)	* m_fElapsedTime;

				if ( m_fRageModeColorChangeTime > fChangeTime )
				{
					m_bRageModeColorToRed = false;
					m_fRageModeColorChangeTime = 0.0f;
				}
			}
			else
			{
				D3DXCOLOR remainColor = D3DXCOLOR( 1, 1, 1, 0.7f ) - m_RageModeColor;
				m_RageModeColor += (remainColor / fRemainTime)	* m_fElapsedTime;

				if ( m_fRageModeColorChangeTime > fChangeTime )
				{
					m_bRageModeColorToRed = true;
					m_fRageModeColorChangeTime = 0.0f;
				}
			}
		}
	}
}



void CX2GUNPC::CommonFrameMove_Invincible()
{

    const StateData& nowStateData = GetNowStateData();

	if(m_Invisible.m_bCloakingUnit == true && GetNowHp() > 0.0f)
	{
		SetAlphaObject( true );
		m_RenderParam.bAlphaBlend = true;

		bool bEnemyUnit = false;
		float fAlphaLowerBound = 0.f;
		if( g_pX2Room->GetMySlot()->GetTeamNum() == GetTeam() )
		{
			bEnemyUnit = false;
			fAlphaLowerBound = m_Invisible.m_fAlphaLowerBound;
		}
		else
		{
			bEnemyUnit = true;
			fAlphaLowerBound = m_Invisible.m_fMinAlpha;
		}

		if(m_Invisible.m_bCloaked == true)
		{
			if(m_Invisible.m_bCloaking == true)
			{
				SetShowObject( false );
				if ( false == IsNullGageUI() )
					SetShowGage( false );
			}
			else
			{
				SetShowObject( true );
				if( m_bShowName == true && false == IsNullGageUI() )
					SetShowGage( true );
			}
		}
		else if(m_Invisible.m_bCloaking == true)
		{
			SetShowObject( true );
			if ( m_bShowName == true && false == IsNullGageUI() )
				SetShowGage( true );

			m_Invisible.m_fNowAlpha -= m_fElapsedTime * m_Invisible.m_fSpeed;
			if( m_Invisible.m_fNowAlpha <= fAlphaLowerBound )
			{
				m_Invisible.m_fNowAlpha = fAlphaLowerBound;
				m_Invisible.m_bCloaked = true;
			}
		}
		else if(m_Invisible.m_bCloaking == false)
		{
			SetShowObject( true );
			if( m_bShowName == true && false == IsNullGageUI() )
				SetShowGage( true );

			m_Invisible.m_fNowAlpha += m_fElapsedTime * m_Invisible.m_fSpeed;
			if( m_Invisible.m_fNowAlpha >= m_Invisible.m_fMaxAlpha )
			{
				m_Invisible.m_fNowAlpha = m_Invisible.m_fMaxAlpha;
				m_Invisible.m_bCloaked = true;
			}
		}
		m_RenderParam.color.a = m_Invisible.m_fNowAlpha;
	}
	else

		if ( true == m_Invisible.m_bAlwaysInvisible )
		{
			if( m_Invisible.m_bInvisible == true && GetNowHp() > 0.0f )
			{
				SetAlphaObject( true );
				m_RenderParam.bAlphaBlend = true;


				if( m_Invisible.m_fNowAlpha < m_Invisible.m_fAlphaLowerBound )
				{
					m_RenderParam.color.a = m_Invisible.m_fAlphaLowerBound;
				}
				else
				{
					m_RenderParam.color.a = m_Invisible.m_fNowAlpha;
				}

				if( m_Invisible.m_bAdd == true )
				{
					m_Invisible.m_fNowAlpha += m_fElapsedTime * m_Invisible.m_fSpeed;
					if( m_Invisible.m_fNowAlpha >= m_Invisible.m_fMaxAlpha )
					{
						m_Invisible.m_fNowAlpha = m_Invisible.m_fMaxAlpha;
						m_Invisible.m_bAdd = false;
					}
				}
				else
				{
					m_Invisible.m_fNowAlpha -= m_fElapsedTime * m_Invisible.m_fSpeed;
					if( m_Invisible.m_fNowAlpha <= m_Invisible.m_fMinAlpha )
					{
						m_Invisible.m_fNowAlpha = m_Invisible.m_fMinAlpha;
						m_Invisible.m_bAdd = true;
					}
				}
			}
		}
		else
		{
			if( m_Invisible.m_bInvisible == true && GetNowHp() > 0.0f )
			{
                const D3DXVECTOR2&  v2InvisibleTime = nowStateData.m_v2InvisibleTime;
				if( v2InvisibleTime.x != -1.0f &&
					v2InvisibleTime.y != -1.0f )
				{
					if( v2InvisibleTime.x <= m_pXSkinAnim->GetNowAnimationTime() &&
						v2InvisibleTime.y > m_pXSkinAnim->GetNowAnimationTime() )
					{
						SetAlphaObject( true );
						m_RenderParam.bAlphaBlend = true;

						if( m_Invisible.m_fNowAlpha < m_Invisible.m_fAlphaLowerBound )
						{
							m_RenderParam.color.a = m_Invisible.m_fAlphaLowerBound;
						}
						else
						{
							m_RenderParam.color.a = m_Invisible.m_fNowAlpha;
						}

						if( m_Invisible.m_bAdd == true )
						{
							m_Invisible.m_fNowAlpha += m_fElapsedTime * m_Invisible.m_fSpeed;
							if( m_Invisible.m_fNowAlpha >= m_Invisible.m_fMaxAlpha )
							{
								m_Invisible.m_fNowAlpha = m_Invisible.m_fMaxAlpha;
								m_Invisible.m_bAdd = false;
							}
						}
						else
						{
							m_Invisible.m_fNowAlpha -= m_fElapsedTime * m_Invisible.m_fSpeed;
							if( m_Invisible.m_fNowAlpha <= m_Invisible.m_fMinAlpha )
							{
								m_Invisible.m_fNowAlpha = m_Invisible.m_fMinAlpha;
								m_Invisible.m_bAdd = true;
							}
						}
					}
				}
				else
				{
					SetAlphaObject( false );
					m_RenderParam.bAlphaBlend = false;
					m_Invisible.m_fNowAlpha = 1.0f;
					m_Invisible.m_bAdd = false;
				}
			}
		}
}


void CX2GUNPC::CommonFrameMove_Particle()
{
	D3DXVECTOR3 vEffectPos = GetPos();
	vEffectPos.y += 110.f;
	for( SeqHitParticleTraceUnitContainer::iterator lit = m_listSeqHitParticleTraceUnit.begin() ; lit != m_listSeqHitParticleTraceUnit.end(); )
	{
		CKTDGParticleSystem::CParticleEventSequenceHandle hParticle = *lit;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( hParticle );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( vEffectPos );
			++lit;
		}
		else
		{
            lit = EraseUnorderlyUsingSwap( m_listSeqHitParticleTraceUnit, lit );
		}

	}
}



void CX2GUNPC::CommonFrameMove_DieParticle()
{
	KTDXPROFILE();




//#ifdef MONSTER_DIE_PARTICLE_TEST
//
//	if( m_hSeqDieEffect != INVALID_PARTICLE_SEQUENCE_HANDLE )
//	{
//		g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqDieEffect );
//	}
//	return;
//
//#endif MONSTER_DIE_PARTICLE_TEST


	if( m_hSeqDieEffect == INVALID_PARTICLE_SEQUENCE_HANDLE )
		return;

	if( null == m_optrLastGameUnitWhoAttackToMe )
		return;


	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqDieEffect );
	if( NULL == pSeq )
	{
		m_hSeqDieEffect = INVALID_PARTICLE_SEQUENCE_HANDLE;
		return;
	}

	D3DXVECTOR3 pos = m_optrLastGameUnitWhoAttackToMe->GetPos();
	pos.y += 60.0f;
	pSeq->SetBlackHolePosition( pos );
	if( pSeq->GetTime() > 1.5f )
	{
		pSeq->SetGravity( CMinMax<D3DXVECTOR3>(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) );
		pSeq->SetUseLand( false );
	}
}

void CX2GUNPC::CommonFrameMove_MindFlagParticle()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();


    if ( NOWSTATE_FLAG( USE_MIND_FLAG ) == false )
        return;


	D3DXVECTOR3 markerPos(0, 0, 0);
	GetFramePos( &markerPos, m_pFrame_Bip01_Head );
	markerPos.y += m_fMindFlagHeight;


	if( m_cMindFlagBackup != MF_NO &&
		m_NPCFrameData.syncData.mindFlag != MF_NO &&
		m_NPCFrameData.syncData.mindFlag != m_cMindFlagBackup )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqQuestion		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hQuestionMark );
		CKTDGParticleSystem::CParticleEventSequence* pSeqExclamation	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hExclamationMark );
		CKTDGParticleSystem::CParticleEventSequence* pSeqAttack			= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqStandUpAttackMark );

		if( pSeqQuestion != NULL )
			pSeqQuestion->ClearAllParticle();
		if( pSeqExclamation != NULL )
			pSeqExclamation->ClearAllParticle();
		if( pSeqAttack != NULL )
			pSeqAttack->ClearAllParticle();

		switch( m_NPCFrameData.syncData.mindFlag )
		{
		case MF_TARGET_TAKE:
			{
				if( NULL != pSeqExclamation )
				{
					pSeqExclamation->SetPosition(markerPos, true );
					pSeqExclamation->CreateNewParticle( pSeqExclamation->GetPosition() );
				}
			}
			break;

		case MF_TARGET_LOST:
			{
				if( NULL != pSeqQuestion )
				{
					pSeqQuestion->SetPosition(markerPos, true );
					pSeqQuestion->CreateNewParticle( pSeqQuestion->GetPosition() );
				}
			}
			break;

		case MF_STAND_UP_ATTACK:
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hSeqStandUpAttackMark )
				{
					m_hSeqStandUpAttackMark	= g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"StandUpAttackMark", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
					pSeqAttack			= g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqStandUpAttackMark );
				}

				if( NULL != pSeqAttack )
				{
					pSeqAttack->SetPosition(markerPos, true );
					pSeqAttack->CreateNewParticle( pSeqAttack->GetPosition() );
				}
			}
			break;
		}
	}





	if( m_hQuestionMark != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hQuestionMark );
		if( NULL != pSeq && pSeq->GetLiveParticleNum() > 0 )
		{
			pSeq->SetPosition( markerPos );
		}
	}

	if( m_hExclamationMark != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hExclamationMark );
		if( NULL != pSeq && pSeq->GetLiveParticleNum() > 0 )
		{
			pSeq->SetPosition( markerPos );
		}
	}

}






void CX2GUNPC::CommonCameraMove()
{
	KTDXPROFILE();

    int iLua_CameraMoveRefID = GetNowStateRuntime().m_iLua_CameraMoveRefID;
    if ( iLua_CameraMoveRefID != LUA_REFNIL )
	{
		lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), iLua_CameraMoveRefID, g_pKTDXApp, g_pX2Game, this );
	}

//	if( false == GetNowStateData().m_strLua_CameraMove.empty() )
//	{
//		string func;
//		ConvertWCHARToChar( func, GetNowStateData().m_strLua_CameraMove.c_str() );
//#ifdef LEAVE_LAST_ERROR_LOG_TEST
//		LastErrorLog( func.c_str() );
//#endif LEAVE_LAST_ERROR_LOG_TEST
////#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		lua_tinker::call<void>( GetFunctionLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
////		lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	}


	else if ( m_LuaManager.BeginTable( "CAMERA" ) == true )
	{
		std::vector< float > vecSubCameraTiming;
		if( m_LuaManager.BeginTable( "SUB_CAMERA_TIMING" ) == true )
		{
			float fSubCameraTiming = 0.f;
			for( int i=1; ; i++ )
			{
				if( true == m_LuaManager.GetValue( i, fSubCameraTiming ) )
				{
					vecSubCameraTiming.push_back( fSubCameraTiming );
				}
				else
				{
					break;
				}
			}
			m_LuaManager.EndTable(); // SUB_CAMERA_TIMING
		}

		if( true == vecSubCameraTiming.empty() )
		{
			g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, 1 );
		}
		else
		{
			bool bDone = false;
			for( int i=0; i<(int)vecSubCameraTiming.size(); i++ )
			{
				float fTiming = vecSubCameraTiming[i];

				if( m_pXSkinAnim->GetNowAnimationTime() < fTiming )
				{
					char tableName[64] = "";
					StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", i );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
					bDone = true;
					break;
				}
			}
			if( false == bDone )
			{
				char tableName[64] = "";
				StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", (int)vecSubCameraTiming.size() );
				g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, tableName );
			}
		}

		m_LuaManager.EndTable(); // CAMERA
	}
    else if ( m_iLua_CommonCameraMoveRefID != LUA_REFNIL )
    {
        lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), m_iLua_CommonCameraMoveRefID, g_pKTDXApp, g_pX2Game, this );
    }
	else
	{
		g_pX2Game->GetX2Camera()->NomalTrackingCamera( this, g_pMain->GetGameOption().GetCameraDistance() );
	}
}

void    CX2GUNPC::CommonEventProcessNonHost()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();

	if( m_NPCFrameData.syncData.nowState == CX2GameUnit::GUSI_DIE )
		return;

    if ( m_eNPCUnitDieState != eNPCUnitDieState_NONE )
    {
        m_fNPCUnitDieStateTimer += m_fElapsedTime;
        if ( m_bDyingStart == true )
            m_fNPCUnitDieStateTimer = 0.f;
        if ( m_fNPCUnitDieStateTimer >= NPC_UNIT_DIE_STATE_TIME_LIMIT )
        {
            StateChangeToDieForce();
            return;
        }//if
    }//if

	if( GetNowHp() <= 0.0f
        && NOWSTATE_FLAG( DYING_END ) == false
		//{{ kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능
        && NOWSTATE_FLAG( NEVER_DIE_THIS_STATE ) == false
		//}} kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능
		)
	{
		if( IsOnSomething() == false )
		{
			//m_bDyingInSky	= true;
			m_bNeverMove	= false;
			m_NPCFrameData.stateParam.bLandConnect = false;			// 09.05.11 태완 : 슈퍼아머 + Land Connnet = true인 state에서 죽을 때 공중에서 굳는 버그 수정.
			m_PhysicParam.passiveSpeed.x = -1.0f;
			m_PhysicParam.passiveSpeed.y = -1.0f;
			m_PhysicParam.fGAccel = m_OrgPhysicParam.fGAccel;
		}
		return;
	}

    if ( NOWSTATE_FLAG( DYING_END ) == true )
	{
		if( false == m_bSelfResurrection
            //|| m_iSelfResurrectionCount <= 0
             || m_eNPCUnitDieState != eNPCUnitDieState_NONE
             )

		{
//{{ robobeg : 2008-10-21
			//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
            if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
//}} robobeg : 2008-10-21
			{
				StateChangeToDieForce();
			}
			return;
		}
	}
    else if ( NOWSTATE_FLAG( ESCAPE_END ) == true )
	{
		if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
		{
			StateChangeToDieForce();
		}
		return;
	}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if( m_NPCFrameData.syncData.bStateChange == true )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		return;

	if( m_NPCFrameData.syncData.nowState == m_CommonState.m_Wait && m_fCoolTime > 0.0f )
		return;


    const std::vector< NPCEventProcessData>& vecNPCEventProcessData = GetNowStateData().m_vecNPCEventProcessData;
	for( int i = 0; i < (int)vecNPCEventProcessData.size(); i++ )
	{
        const NPCEventProcessData* pEventProcessData = &vecNPCEventProcessData[i];
		if ( pEventProcessData == NULL )
			continue;

		switch( pEventProcessData->m_StateChangeType )
		{
		case SCT_MOTION_END:
			{
				if( m_pXSkinAnim->IsAnimationEnd() == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE_OR_WAIT() == false )
                    {
                        bool bReactionState = _IsReactionState( pEventProcessData->m_NextStateID );
                        if ( bReactionState == true || _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                        {
                            if ( bReactionState == true &&
                                m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == false )
                            {
                                m_bClientHoldPosition = true;
                                break;
                            }
                            StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, bReactionState == false );
                        }
                        break;
                    }//if
                    m_bClientHoldPosition = true;
				}
			}
			break;

		case SCT_FOOT_ON_LINE_FALSE_DOWN:
			{
				if( IsOnSomething() == false )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE_OR_WAIT() == false )
                    {
                        bool bReactionState = _IsReactionState( pEventProcessData->m_NextStateID );
                        if ( bReactionState == true || _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                        {
                            if ( bReactionState == true && m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == false )
                            {
                                m_bClientHoldPosition = true;
                                break;
                            }
                            m_NPCFrameData.syncData.position.y -= LINE_RADIUS * 1.5f;
                            StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, bReactionState == false );
                            break;
                        }
                    }//if
                    m_bClientHoldPosition = true;
				}
			}
			break;


		case SCT_FOOT_ON_LINE_TRUE:
			{
				if( IsOnSomething() == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE_OR_WAIT() == false )
                    {
                        bool bReactionState = _IsReactionState( pEventProcessData->m_NextStateID );
                        if ( bReactionState == true || _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                        {
                            if ( bReactionState == true && m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == false )
                            {
                                break;
                            }
                            StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, bReactionState == false );
                            break;
                        }
                    }
				}
			}
			break;

		case SCT_POSITIVE_Y_SPEED:
			{
				if( m_PhysicParam.nowSpeed.y > 0.0f == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE_OR_WAIT() == false )
                    {
                        bool bReactionState = _IsReactionState( pEventProcessData->m_NextStateID );
                        if ( bReactionState == true || _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                        {
                            if ( bReactionState == true && m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == false )
                            {
                                break;
                            }
                            StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, bReactionState == false );
                            break;
                        }
                    }
				}
			}
			break;

		case SCT_NEGATIVE_Y_SPEED:
			{
				if( m_PhysicParam.nowSpeed.y <= 0.0f == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE_OR_WAIT() == false )
                    {
                        bool bReactionState = _IsReactionState( pEventProcessData->m_NextStateID );
                        if ( bReactionState == true || _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                        {
                            if ( bReactionState == true && m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == false )
                            {
                                break;
                            }
                            StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, bReactionState == false );
                            break;
                        }//if
                    }
				}
			}
			break;

		case SCT_CONDITION_TABLE:
            {
				KTDXPROFILE_BEGIN("SCT_CONDITION_TABLE");

				if( false == m_bEnableCondtionTableEventProcess )
					break;
				ConditionTableData* pConditionTableData = pEventProcessData->m_pConditionTableData;
                if ( pConditionTableData == NULL )
                    break;
                if ( false == ( ( m_kNonHostReaction.IsNowStateReaction_FIRST_OR_CONTINUE() == true )
                                && m_kNonHostReaction.GetNowStateReactionWait() == false && pConditionTableData->m_bNPCNonHostSimulatable == true ) )
                    break;
                CX2GUUser* pTargetGameUnit = NULL;
                if ( pConditionTableData->m_bRequireTarget == true )
                {
                    pTargetGameUnit = g_pX2Game->GetUserUnit( m_kNonHostReaction.GetTargetUnitIndex() );
                }//if
				if( pConditionTableData->m_iIntervalID != -1 && pConditionTableData->m_iIntervalID < (int)m_EventIntervalList.size() )
				{
                    if ( m_EventIntervalList[pConditionTableData->m_iIntervalID].first.z == 0.0f )
                        break;
				}
				if( pConditionTableData->m_fDistanceToTarget != -1.0f )
				{
					if ( NULL != pTargetGameUnit )
					{
						float fDistanc3Sq = GetDistance3Sq( pTargetGameUnit->GetPos(), GetPos() );
						if( fDistanc3Sq > pConditionTableData->m_fDistanceToTarget * pConditionTableData->m_fDistanceToTarget )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				if( pConditionTableData->m_fDistanceOverTarget != -1.0f )
				{
					if ( NULL != pTargetGameUnit )
					{
						float fDistanc3Sq = GetDistance3Sq( pTargetGameUnit->GetPos(), GetPos() );
						if( fDistanc3Sq < pConditionTableData->m_fDistanceOverTarget * pConditionTableData->m_fDistanceOverTarget )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				if( pConditionTableData->m_bTargetAboveMe != false )
				{
					if ( NULL != pTargetGameUnit )
					{
						if( pTargetGameUnit->GetYPos() <= GetYPos() )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				if( pConditionTableData->m_bTargetBelowMe != false )
				{
					if ( NULL != pTargetGameUnit )
					{
						if( pTargetGameUnit->GetYPos() > GetYPos() )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				if( pConditionTableData->m_iAnimPlayCount != -1 )
				{
					if( m_pXSkinAnim->GetNowPlayCount() != pConditionTableData->m_iAnimPlayCount ||
						m_pXSkinAnim->IsAnimationEnd() != true )
						break;
				}

				if( pConditionTableData->m_fSmallThenYSpeedFactor != -1.0f )
				{
					if( m_PhysicParam.nowSpeed.y >= pConditionTableData->m_fSmallThenYSpeedFactor )
                        break;
				}
				if( pConditionTableData->m_fStateTimeOverFactor != -1.0f )
				{
					if( m_NPCFrameData.unitCondition.fStateTime <= pConditionTableData->m_fStateTimeOverFactor )
                        break;
				}
				if( pConditionTableData->m_fAnimTimerFactor != -1.0f )
				{
					if( m_pXSkinAnim->EventTimer( pConditionTableData->m_fAnimTimerFactor ) == false )
                        break;
				}
				if( pConditionTableData->m_fMyHPLessThanPercentFactor != -1.0f )
				{
					if( 100*GetNowHp()/GetMaxHp() > pConditionTableData->m_fMyHPLessThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyHPMoreThanPercentFactor != -1.0f )
				{
					if( 100*GetNowHp()/GetMaxHp() < pConditionTableData->m_fMyHPMoreThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyMPLessThanPercentFactor != -1.0f )
				{
					if( 100*GetNowMp()/GetMaxMp() > pConditionTableData->m_fMyMPLessThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyMPMoreThanPercentFactor != -1.0f )
				{
					if( 100*GetNowMp()/GetMaxMp() < pConditionTableData->m_fMyMPMoreThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fTargetHPLessThanPercentFactor != -1.0f )
				{
					if( NULL != pTargetGameUnit &&
						(100 * pTargetGameUnit->GetNowHp() / pTargetGameUnit->GetMaxHp()) > pConditionTableData->m_fTargetHPLessThanPercentFactor )
						break;
				}
				if( pConditionTableData->m_fTargetHPMoreThanPercentFactor != -1.0f )
				{
					if( NULL != pTargetGameUnit &&
						(100 * pTargetGameUnit->GetNowHp() / pTargetGameUnit->GetMaxHp()) < pConditionTableData->m_fTargetHPMoreThanPercentFactor )
						break;
				}
				if( pConditionTableData->m_fMyMPMoreThanFactor != -1.0f )
				{
					if( GetNowMp() < pConditionTableData->m_fMyMPMoreThanFactor )
                        break;
				}
				if( pConditionTableData->m_bAttackSuccessFactor != false )
				{
					if( GetUnitCondition().bHit == false )
                        break;
				}
				if( pConditionTableData->m_bSameLineWithTargetFactor != false )
				{
					if ( NULL != pTargetGameUnit )
					{
						if( pTargetGameUnit->GetLastTouchLineIndex() != GetLastTouchLineIndex() )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				//if( pConditionTableData->m_iFlagTrue != -1 )
				//{
				//	if( m_FlagList[pConditionTableData->m_iFlagTrue] == false )
				//		break;
				//}
				//if( pConditionTableData->m_iFlagFalse != -1 )
				//{
				//	if( m_FlagList[pConditionTableData->m_iFlagFalse] == true )
				//		break;
				//}
				//{{oasis:김상윤////2009-10-7////
				//{
    //                int i = 0;
				//	for( ; i<4; i++ )
				//	{
				//		if( pConditionTableData->m_iGroupAICommandTrue[i] != CX2GUNPC::GAI_CMD_OFF)
				//		{
				//			if( GetGroupAICommand_LUA() != pConditionTableData->m_iGroupAICommandTrue[i])
				//				break;
				//		}
				//	}
    //                if ( i < 4 )
    //                    break;
				//}
				//{
    //                int i = 0;
				//	for( ; i<4; i++ )
				//	{
				//		if( pConditionTableData->m_iGroupAICommandFalse[i] != CX2GUNPC::GAI_CMD_OFF )
				//		{
				//			if( GetGroupAICommand_LUA() == pConditionTableData->m_iGroupAICommandFalse[i] )
    //                            break;
				//		}
				//	}
    //                if ( i < 4 )
    //                    break;
				//}
			//}}oasis:김상윤////2009-10-7////
				// 도망가기 성공했는지를 검사
				if( pConditionTableData->m_bEscapeCheck == true )
				{
					//if( NULL == m_pAI )
					//{
					//	bConditionCheck = false;
					//}
					//else if( m_pAI->GetFleeing() == true )
					//{
						// 아직 도망가고 있는 중이라면 실패
				    break;
					//}
				}
				if( pConditionTableData->m_iHaveTarget != -1 )
				{
					if( (HasTarget() == true) != (pConditionTableData->m_iHaveTarget == 1) )
                        break;
				}
				if ( pConditionTableData->m_fHigherThanPosY != -1.0f )
				{
					if ( GetPos().y < pConditionTableData->m_fHigherThanPosY )
                        break;
				}

				if ( pConditionTableData->m_iAliveUserUnitGeq != -1 )
				{
					if( false == ( g_pX2Game->LiveUserUnitNum() >= pConditionTableData->m_iAliveUserUnitGeq ) )
                        break;
				}
				if ( pConditionTableData->m_iAliveUserUnitLeq != -1 )
				{
					if( false == ( g_pX2Game->LiveUserUnitNum() <= pConditionTableData->m_iAliveUserUnitLeq ) )
                        break;
				}
				if ( pConditionTableData->m_iUserUnitGeq != -1 )
				{
					if( false == ( g_pX2Room->GetNumOfPlayers() >= pConditionTableData->m_iUserUnitGeq ) )
                        break;
				}
				if ( pConditionTableData->m_iUserUnitLeq != -1 )
				{
                    if( false == ( g_pX2Room->GetNumOfPlayers() <= pConditionTableData->m_iUserUnitLeq ) )
                        break;
				}
				{
                    int i = 0;
					for( ; i<10; i++ )
					{
						if( pConditionTableData->m_fTimerElapsed[i] != -1.f )
						{
							if( m_aTimerList[i].GetTime() < pConditionTableData->m_fTimerElapsed[i] )
								break;
						}
					}
                    if ( i < 10 )
                        break;
				}
				if( pConditionTableData->m_iGEKeyCode != -1 )
				{
					if(pConditionTableData->m_iGEKeyCode > GetKeyCode())
                        break;
				}
				if( pConditionTableData->m_bDirTarget == true )
				{
					if ( NULL != pTargetGameUnit )
					{
						const bool bRight = IsRightTarget( pTargetGameUnit->GetPos() );
						if( bRight != pConditionTableData->m_bRightTarget )
							break;
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
				if( true == pConditionTableData->m_bViewTargetDir )
				{
					if ( NULL != pTargetGameUnit )
					{
						const bool bRight = IsRightTarget( pTargetGameUnit->GetPos() );
						if( true == bRight && true == GetIsRight() )
						{
							break;
						}
						else if( false == bRight && false == GetIsRight() )
						{
							break;
						}
						// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
					}
					else
						break;
				}
			    //{{ oasis907 : 김상윤 [2010.8.1] //
				if( pConditionTableData->m_bFootOnLine != false )
				{
					if(m_NPCFrameData.unitCondition.bFootOnLine == false)
                        break;
				}
				//}} oasis907 : 김상윤 [2010.8.1] //
				if( pConditionTableData->m_bRageMode == true )
				{
					if( m_fRageTime <= 0.f )
                        break;
				}
					//{{ mauntain : 김태환 [2012.06.19] 몬스터 카드 소환 기능 - 타겟팅된 대상의 팀이 자신의 팀과 다른지 검사
				if( pConditionTableData->m_bCheckAnotherTeam == true )
				{
					if ( NULL != pTargetGameUnit && GetTeam() == pTargetGameUnit->GetTeam() )
						break;
				}
					//}}

				if( pConditionTableData->m_iRate != -1 )
				{
                    if ( pConditionTableData->m_iRate <= 0 )
                        break;
                    else if ( pConditionTableData->m_iRate < 100 )
                    {
                        int randRate = m_kNonHostReaction.EvaluateConditionRandomRate(
                            i,
                            m_NPCFrameData.syncData.stateChangeParts.stateChangeNum,
                            *pConditionTableData,
                            m_EventIntervalList );

						if( pConditionTableData->m_iRate <= randRate )
							break;
                    }
				}

                if ( _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                    StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, false );
                else if ( _IsReactionWaitState( pEventProcessData->m_NextStateID ) == true )
                    StateChange_Reaction_Continue( pEventProcessData->m_NextStateID, true );

				KTDXPROFILE_END();
            }
			break;
        }//switch

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		if( m_NPCFrameData.syncData.bStateChange == true )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			break;
    }//for
}


void CX2GUNPC::CommonEventProcess()
{
	KTDXPROFILE();

	if ( NULL == m_pXSkinAnim )
		return;

	if( m_NPCFrameData.syncData.nowState == CX2GameUnit::GUSI_DIE )
		return;

    const StateData& nowStateData = GetNowStateData();



	if( GetNowHp() > 0.f )
	{
		if( true == m_bSelfDestruction && m_fSelfDestructionTime != -1.f )
		{
			if( m_SelfDestructionTimer.elapsed() > m_fSelfDestructionTime )
			{
				StateChange( m_SelfDestructionState );
			}
		}
	}


	if( GetNowHp() <= 0.0f
        && NOWSTATE_FLAG( DYING_END ) == false
		//{{ kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능
        && NOWSTATE_FLAG( NEVER_DIE_THIS_STATE ) == false
		//}} kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능
		)
	{
		if( IsOnSomething() == true )
		{
			if( m_bDyingInSky == false )
			{
				if( m_NPCFrameData.unitCondition.bAttackerFront == true )
                {
                    if(m_SpeedDyingFly > 0 && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true)
                    {
                        StateChange( m_DyingFly );
                    }
                    else
					StateChange( m_DyingLandFront );
                }
				else
                {
                    if(m_SpeedDyingFly > 0 && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true)
                    {
                        StateChange( m_DyingFly );
                    }
                    else
					StateChange( m_DyingLandBack );
                }
			}
			else
			{
                if(m_SpeedDyingFly > 0 && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true)
                {
                    StateChange( m_DyingFly );
                }
                else
				StateChange( m_DyingSky ); // m_DyingLandFront, m_DyingLandBack가 되어야 하지 않는가?
			}
		}
		else
		{
            if((m_SpeedDyingFly > 0 && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true) || m_bSkyDie == true)
            {
                if(m_SpeedDyingFly > 0 && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true)
                {
                    StateChange( m_DyingFly );
                }
                else
                StateChange( m_DyingSky );
            }

			m_bDyingInSky	= true;
			m_bNeverMove	= false;
			m_NPCFrameData.stateParam.bLandConnect = false;			// 09.05.11 태완 : 슈퍼아머 + Land Connnet = true인 state에서 죽을 때 공중에서 굳는 버그 수정.
			m_PhysicParam.passiveSpeed.x = -1.0f;
			m_PhysicParam.passiveSpeed.y = -1.0f;
			m_PhysicParam.fGAccel = m_OrgPhysicParam.fGAccel;
		}


		if( true == m_bSelfResurrection && m_iSelfResurrectionCount > 0 )
		{
			m_SelfResurrectionTimer.restart();
		}

		return;
	}

    if ( NOWSTATE_FLAG( DYING_END ) == true )
	{
		if( false == m_bSelfResurrection || m_iSelfResurrectionCount <= 0
             || m_eNPCUnitDieState != eNPCUnitDieState_NONE
            )
		{
			SetAIEnable( false );
//{{ robobeg : 2008-10-21
			//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
            if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
//}} robobeg : 2008-10-21
			{
                StateChangeToDieForce();
			}

			return;
		}

		if( true == m_bSelfResurrection && m_iSelfResurrectionCount > 0 )
		{
			if( m_SelfResurrectionTimer.elapsed() > m_fSelfResurrectionTime )
			{
				SetNowHp( GetMaxHp() * (float)m_iSelfResurrectionHPPercent / 100.f );

				StateChangeForce( m_RebirthState, true );
				if( m_pXSkinAnim != NULL )
				{
					m_pXSkinAnim->SetPlaySpeed( 1.f );
				}
				m_iSelfResurrectionCount--;
				m_bDyingStart = false;
			}
		}
	}
    else if ( NOWSTATE_FLAG( ESCAPE_END ) == true )
	{
		SetAIEnable( false );
		//{{ robobeg : 2008-10-21
		//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
		if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
			//}} robobeg : 2008-10-21
		{
            StateChangeToDieForce();
		}
		return;
	}




	if( m_AIMessage == CX2GUNPC::SCT_AI_DIR_CHANGE )
	{
        if( NOWSTATE_FLAG( ALLOW_DIR_CHANGE ) == true )
		{
			m_bImmediate = true;
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
            m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum = m_NPCFrameData.syncData.stateChangeParts.stateModifyNum;
            m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum++;
			if( g_pMain->IsEnableUdpPacketOverlap() )
				SendReserve( 2 );
			else
				SendReserve( 1 );
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//			SendReserve();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

            SetIsRightNow( m_bAIIsRight );
		}
	}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if( m_NPCFrameData.syncData.bStateChange == true )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		return;

	if( m_NPCFrameData.syncData.nowState == m_CommonState.m_Wait && m_fCoolTime > 0.0f )
		return;

    m_vecEventProcessAccum.resize( 0 );

    const std::vector< NPCEventProcessData>& vecNPCEventProcessData = GetNowStateData().m_vecNPCEventProcessData;
    const std::vector< NPCEventProcessRuntime >& vecNPCEventProcessRuntime = GetNowStateRuntime().m_vecNPCEventProcessRuntime;
	for( int i = 0; i < (int)vecNPCEventProcessData.size(); i++ )
	{
        const NPCEventProcessData* pEventProcessData = &vecNPCEventProcessData[i];

		if ( pEventProcessData == NULL )
			continue;

		switch( pEventProcessData->m_StateChangeType )
		{
		case SCT_MOTION_END:
			{
				if( m_pXSkinAnim->IsAnimationEnd() == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false
                        && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                        StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                    else
                        StateChange( pEventProcessData->m_NextStateID );

					//{{oasis:김상윤////2009-10-7////
					if( NULL != m_pAI )
						m_pAI->SetPrevAIMessage( SCT_NO_CHANGE );
					//}}oasis:김상윤////2009-10-7////

				}
			}
			break;

		case SCT_FOOT_ON_LINE_FALSE_DOWN:
			{
				if( IsOnSomething() == false )
				{
					m_NPCFrameData.syncData.position.y -= LINE_RADIUS * 1.5f;
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false
                        && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                        StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                    else
                        StateChange( pEventProcessData->m_NextStateID );

				}
			}
			break;

		case SCT_FOOT_ON_LINE_TRUE:
			{
				if( IsOnSomething() == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false
                        && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                        StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                    else
                        StateChange( pEventProcessData->m_NextStateID );
				}
			}
			break;

		case SCT_POSITIVE_Y_SPEED:
			{
				if( m_PhysicParam.nowSpeed.y > 0.0f == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false
                        && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                        StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                    else
                        StateChange( pEventProcessData->m_NextStateID );
				}
			}
			break;

		case SCT_NEGATIVE_Y_SPEED:
			{
				if( m_PhysicParam.nowSpeed.y <= 0.0f == true )
				{
                    if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false
                        && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                        StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                    else
                        StateChange( pEventProcessData->m_NextStateID );
				}
			}
			break;

		case SCT_AI_WAIT:
			{
				if( m_AIMessage == SCT_AI_WAIT || m_UnUseAIMessage == SCT_AI_WAIT )
				{
//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_WALK:
			{
				if( m_AIMessage == SCT_AI_WALK || m_UnUseAIMessage == SCT_AI_WALK )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					    StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_DASH:
			{
				if( m_AIMessage == SCT_AI_DASH || m_UnUseAIMessage == SCT_AI_DASH )
				{
                    SetIsRightNow( m_bAIIsRight );
//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_JUMP:
			{
				if( m_AIMessage == SCT_AI_JUMP || m_UnUseAIMessage == SCT_AI_JUMP )
				{
//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_DOWN:
			{
				if( m_AIMessage == SCT_AI_DOWN || m_UnUseAIMessage == SCT_AI_DOWN )
				{
					D3DXVECTOR3 vTempLandPosition = m_NPCFrameData.unitCondition.landPosition;
					int iTempLastLineIndex = m_NPCFrameData.syncData.lastTouchLineIndex;

					bool bCanDown = g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_NPCFrameData.syncData.position,
						LINE_RADIUS, &vTempLandPosition, &iTempLastLineIndex, true, true );

					if( true == bCanDown )
					{
						m_NPCFrameData.syncData.position.y -= LINE_RADIUS * 1.5f;
//{{ robobeg : 2008-10-28
					    //StateChange( pEventProcessData->m_wstrNextState.c_str() );
					    StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
						m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
					}
				}
			}
			break;

		case SCT_AI_JUMP_DIR:
			{
				if( m_AIMessage == SCT_AI_JUMP_DIR || m_UnUseAIMessage == SCT_AI_JUMP_DIR )
				{
                    SetIsRightNow( m_bAIIsRight );
//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_DOWN_DIR:
			{
				if( m_AIMessage == SCT_AI_DOWN_DIR || m_UnUseAIMessage == SCT_AI_DOWN_DIR )
				{
                    SetIsRightNow( m_bAIIsRight );
					m_NPCFrameData.syncData.position.y -= LINE_RADIUS * 1.5f;
//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

		case SCT_AI_FLY_FRONT:
			{
				if( m_AIMessage == SCT_AI_FLY_FRONT || m_UnUseAIMessage == SCT_AI_FLY_FRONT )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_FRONT_UP:
			{
				if( m_AIMessage == SCT_AI_FLY_FRONT_UP || m_UnUseAIMessage == SCT_AI_FLY_FRONT_UP )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_FRONT_DOWN:
			{
				if( m_AIMessage == SCT_AI_FLY_FRONT_DOWN || m_UnUseAIMessage == SCT_AI_FLY_FRONT_DOWN )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_BACK:
			{
				if( m_AIMessage == SCT_AI_FLY_BACK || m_UnUseAIMessage == SCT_AI_FLY_BACK )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_BACK_UP:
			{
				if( m_AIMessage == SCT_AI_FLY_BACK_UP || m_UnUseAIMessage == SCT_AI_FLY_BACK_UP )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_BACK_DOWN:
			{
				if( m_AIMessage == SCT_AI_FLY_BACK_DOWN || m_UnUseAIMessage == SCT_AI_FLY_BACK_DOWN )
				{
                    if ( pEventProcessData->m_NextStateID >= 0
                        && pEventProcessData->m_NextStateID < (int) m_vecStateRuntime.size()
                        && ( pEventProcessData->m_NextStateID != m_NPCFrameData.syncData.nowState || m_NPCFrameData.syncData.bIsRight != m_bAIIsRight ) )
					{
                        SetIsRightNow( m_bAIIsRight );
						//{{ robobeg : 2008-10-28
						//StateChange( pEventProcessData->m_wstrNextState.c_str() );
						StateChange( pEventProcessData->m_NextStateID );
						//}} robobeg : 2008-10-28
					}
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_UP:
			{
				if( m_AIMessage == SCT_AI_FLY_UP || m_UnUseAIMessage == SCT_AI_FLY_UP )
				{
                        SetIsRightNow( m_bAIIsRight );
					StateChange( pEventProcessData->m_NextStateID );
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;
		case SCT_AI_FLY_DOWN:
			{
				if( m_AIMessage == SCT_AI_FLY_DOWN || m_UnUseAIMessage == SCT_AI_FLY_DOWN )
				{
                        SetIsRightNow( m_bAIIsRight );
					StateChange( pEventProcessData->m_NextStateID );
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;



//{{oasis:김상윤////2009-10-7////

		case SCT_AI_SELF_COMMAND_TRANSIT:
			{
				if( m_AIMessage == SCT_AI_SELF_COMMAND_TRANSIT || m_UnUseAIMessage == SCT_AI_SELF_COMMAND_TRANSIT )
				{
					//{{ robobeg : 2008-10-28
					//StateChange( pEventProcessData->m_wstrNextState.c_str() );
					StateChange( pEventProcessData->m_NextStateID );
					//}} robobeg : 2008-10-28
					m_AIMessage = m_UnUseAIMessage = SCT_NO_CHANGE;
				}
			}
			break;

//}}oasis:김상윤////2009-10-7////








		case SCT_CONDITION_FUNCTION:
			{
                if ( i < (int) vecNPCEventProcessRuntime.size() )
                {
                    const NPCEventProcessRuntime& npcEventProcessRuntime = vecNPCEventProcessRuntime[ i ];
                    if ( npcEventProcessRuntime.m_iLua_CustomConditionRefID != LUA_REFNIL )
                    {
                        bool bRt = lua_tinker::call<bool>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef()
                            , npcEventProcessRuntime.m_iLua_CustomConditionRefID, g_pKTDXApp, g_pX2Game, this );
                        if ( bRt == true )
                        {
                            StateChange( pEventProcessData->m_NextStateID );
                        }
                    }
                }
			}
			break;

		case SCT_CONDITION_TABLE:
			{
				KTDXPROFILE_BEGIN("SCT_CONDITION_TABLE");

				if( false == m_bEnableCondtionTableEventProcess )
					continue;

                bool bReactState = false;
                bool bDamageReactState = false;

                if( NULL == pEventProcessData->m_pConditionTableData )
                    break;

				ConditionTableData* pConditionTableData = pEventProcessData->m_pConditionTableData;

                if ( m_kNonHostReaction.IsNowStateReaction_NONE() == false && pConditionTableData->m_bNPCNonHostSimulatable == true )
                    bReactState = true;

                bool    bOneshot = false;

				if( pConditionTableData->m_iIntervalID != -1 && pConditionTableData->m_iIntervalID < (int)m_EventIntervalList.size() )
				{
                    if ( m_EventIntervalList[pConditionTableData->m_iIntervalID].first.z == 0.0f )
                        break;
                    bOneshot = true;
				}
				if( pConditionTableData->m_fDistanceToTarget != -1.0f )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							float fDistanc3Sq = GetDistance3Sq( pTargetGameUnit->GetPos(), GetPos() );
							if( fDistanc3Sq > pConditionTableData->m_fDistanceToTarget * pConditionTableData->m_fDistanceToTarget )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( pConditionTableData->m_fDistanceOverTarget != -1.0f )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							float fDistanc3Sq = GetDistance3Sq( pTargetGameUnit->GetPos(), GetPos() );
							if( fDistanc3Sq < pConditionTableData->m_fDistanceOverTarget * pConditionTableData->m_fDistanceOverTarget )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( pConditionTableData->m_bTargetAboveMe != false )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							if( pTargetGameUnit->GetYPos() <= GetYPos() )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( pConditionTableData->m_bTargetBelowMe != false )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							if( pTargetGameUnit->GetYPos() > GetYPos() )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( pConditionTableData->m_iAnimPlayCount != -1 )
				{
					if( m_pXSkinAnim->GetNowPlayCount() != pConditionTableData->m_iAnimPlayCount ||
						m_pXSkinAnim->IsAnimationEnd() != true )
						break;
                    bOneshot = true;
				}
				if( pConditionTableData->m_fSmallThenYSpeedFactor != -1.0f )
				{
					if( m_PhysicParam.nowSpeed.y >= pConditionTableData->m_fSmallThenYSpeedFactor )
                        break;
				}
				if( pConditionTableData->m_fStateTimeOverFactor != -1.0f )
				{
					if( m_NPCFrameData.unitCondition.fStateTime <= pConditionTableData->m_fStateTimeOverFactor )
                        break;
				}
				if( pConditionTableData->m_fAnimTimerFactor != -1.0f )
				{
					if( m_pXSkinAnim->EventTimer( pConditionTableData->m_fAnimTimerFactor ) == false )
                        break;
                    bOneshot = true;
				}
				if( pConditionTableData->m_fMyHPLessThanPercentFactor != -1.0f )
				{
					if( 100*GetNowHp()/GetMaxHp() > pConditionTableData->m_fMyHPLessThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyHPMoreThanPercentFactor != -1.0f )
				{
					if( 100*GetNowHp()/GetMaxHp() < pConditionTableData->m_fMyHPMoreThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyMPLessThanPercentFactor != -1.0f )
				{
					if( 100*GetNowMp()/GetMaxMp() > pConditionTableData->m_fMyMPLessThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fMyMPMoreThanPercentFactor != -1.0f )
				{
					if( 100*GetNowMp()/GetMaxMp() < pConditionTableData->m_fMyMPMoreThanPercentFactor )
                        break;
				}
				if( pConditionTableData->m_fTargetHPLessThanPercentFactor != -1.0f )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if( NULL != pTargetGameUnit &&
							(100 * pTargetGameUnit->GetNowHp() / pTargetGameUnit->GetMaxHp()) > pConditionTableData->m_fTargetHPLessThanPercentFactor )
						{
							break;
						}
					}
				}

				if( pConditionTableData->m_fTargetHPMoreThanPercentFactor != -1.0f )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if( NULL != pTargetGameUnit &&
							(100 * pTargetGameUnit->GetNowHp() / pTargetGameUnit->GetMaxHp()) < pConditionTableData->m_fTargetHPMoreThanPercentFactor )
						{
							break;
						}
					}
				}

				if( pConditionTableData->m_fMyMPMoreThanFactor != -1.0f )
				{
					if( GetNowMp() < pConditionTableData->m_fMyMPMoreThanFactor )
                        break;
                }
				if( pConditionTableData->m_bAttackSuccessFactor != false )
				{
					if( GetUnitCondition().bHit == false )
                        break;
				}
				if( pConditionTableData->m_bSameLineWithTargetFactor != false )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							if( pTargetGameUnit->GetLastTouchLineIndex() != GetLastTouchLineIndex() )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( pConditionTableData->m_iFlagTrue != -1 )
				{
					if( m_FlagList[pConditionTableData->m_iFlagTrue] == false )
						break;
				}
				if( pConditionTableData->m_iFlagFalse != -1 )
				{
					if( m_FlagList[pConditionTableData->m_iFlagFalse] == true )
						break;
				}
				//{{oasis:김상윤////2009-10-7////
                {
                    int i=0;
				    for( ; i<4; i++ )
				    {
					    if( pConditionTableData->m_iGroupAICommandTrue[i] != CX2GUNPC::GAI_CMD_OFF)
					    {
						    if( GetGroupAICommand_LUA() != pConditionTableData->m_iGroupAICommandTrue[i])
							    break;
					    }
				    }
                    if ( i < 4 )
                        break;
                }
				{
                    int i = 0;
					for( ; i<4; i++ )
					{
						if( pConditionTableData->m_iGroupAICommandFalse[i] != CX2GUNPC::GAI_CMD_OFF )
						{
							if( GetGroupAICommand_LUA() == pConditionTableData->m_iGroupAICommandFalse[i] )
								break;
						}
					}
                    if ( i < 4 )
                        break;
				}
				//}}oasis:김상윤////2009-10-7////
					// 도망가기 성공했는지를 검사
				if( pConditionTableData->m_bEscapeCheck == true )
				{
                    // 아직 도망가고 있는 중이라면 실패
					if( NULL == m_pAI || m_pAI->GetFleeing() == true )
                        break;
				}
				if( pConditionTableData->m_iHaveTarget != -1 )
				{
					if( (HasTarget() == true) != (pConditionTableData->m_iHaveTarget == 1) )
                        break;
				}
				if ( pConditionTableData->m_fHigherThanPosY != -1.0f )
				{
					if ( GetPos().y < pConditionTableData->m_fHigherThanPosY )
                        break;
				}

				if ( pConditionTableData->m_iAliveUserUnitGeq != -1 )
				{
                    if( false == ( g_pX2Game->LiveUserUnitNum() >= pConditionTableData->m_iAliveUserUnitGeq ) )
                        break;
				}
				if ( pConditionTableData->m_iAliveUserUnitLeq != -1 )
				{
					if( false == ( g_pX2Game->LiveUserUnitNum() <= pConditionTableData->m_iAliveUserUnitLeq ) )
                        break;
				}
				if ( pConditionTableData->m_iUserUnitGeq != -1 )
				{
					if( false == ( g_pX2Room->GetNumOfPlayers() >= pConditionTableData->m_iUserUnitGeq ) )
                        break;
				}
				if ( pConditionTableData->m_iUserUnitLeq != -1 )
				{
					if( false == ( g_pX2Room->GetNumOfPlayers() <= pConditionTableData->m_iUserUnitLeq ) )
                        break;
				}

				{
                    int i = 0;
					for( ; i<10; i++ )
					{
						if( pConditionTableData->m_fTimerElapsed[i] != -1.f )
						{
                            if( m_aTimerList[i].GetTime() < pConditionTableData->m_fTimerElapsed[i] )
							{
								break;
							}
						}
					}
                    if ( i < 10 )
                        break;
				}
				if( pConditionTableData->m_iGEKeyCode != -1 )
				{
					if(pConditionTableData->m_iGEKeyCode > GetKeyCode())
                        break;
				}
				if( pConditionTableData->m_bDirTarget == true )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							const bool bRight = IsRightTarget( pTargetGameUnit->GetPos() );
							if( bRight != pConditionTableData->m_bRightTarget )
								break;
							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				if( true == pConditionTableData->m_bViewTargetDir )
				{
					if ( NULL != m_pAI )
					{
						CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
						if ( NULL != pTargetGameUnit )
						{
							const bool bRight = IsRightTarget( pTargetGameUnit->GetPos() );
							if( true == bRight && true == GetIsRight() )
								break;
							else if( false == bRight && false == GetIsRight() )
								break;

							// else 인 경우에만 bConditionCheck = false를 하지 않고 다음 판단으로 넘어감
						}
						else
							break;
					}
					else
						break;
				}
				//{{ oasis907 : 김상윤 [2010.8.1] //
				if( pConditionTableData->m_bFootOnLine != false )
				{
					if(m_NPCFrameData.unitCondition.bFootOnLine == false)
                        break;
				}
				//}} oasis907 : 김상윤 [2010.8.1] //
				if( pConditionTableData->m_bRageMode == true )
				{
					if( m_fRageTime <= 0.f )
                        break;
				}
				//{{ mauntain : 김태환 [2012.06.19] 몬스터 카드 소환 기능 - 타겟팅된 대상의 팀이 자신의 팀과 다른지 검사
				if( pConditionTableData->m_bCheckAnotherTeam == true )
				{
					CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
					if ( NULL != pTargetGameUnit && GetTeam() == pTargetGameUnit->GetTeam() )
						break;
				}
				//}}

                {
                    bool bRt = true;
                    if ( i < (int) vecNPCEventProcessRuntime.size() )
                    {
                        const NPCEventProcessRuntime& npcEventProcessRuntime = vecNPCEventProcessRuntime[ i ];
                        if ( npcEventProcessRuntime.m_iLua_CustomConditionRefID != LUA_REFNIL )
                        {
                            bRt = lua_tinker::call<bool>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), npcEventProcessRuntime.m_iLua_CustomConditionRefID, g_pKTDXApp, g_pX2Game, this );
                        }
                    }
                    if ( bRt == false )
                    {
                        break;
                    }
                }

				if( pConditionTableData->m_iRate != -1 )
				{
                    if ( pConditionTableData->m_iRate <= 0 )
                        break;
                    if ( pConditionTableData->m_iRate < 100 )
                    {
                        if ( bReactState == true )
                        {
                            int randRate = m_kNonHostReaction.EvaluateConditionRandomRate(
                                i,
                                m_NPCFrameData.syncData.stateChangeParts.stateChangeNum,
                                *pConditionTableData,
                                m_EventIntervalList );
						    if( pConditionTableData->m_iRate <= randRate )
							    break;
                        }
                        else
                        {
                            if ( EstimateFrameOneshotPercent( (float) pConditionTableData->m_iRate ) == false )
                            {
                                if ( bOneshot == false && m_iFrameAccumCounter > 1 )
                                {
                                    EVENT_PROCESS_ACCUM epaccum;
                                    epaccum.m_iRate = pConditionTableData->m_iRate;
                                    epaccum.m_iStateID = pEventProcessData->m_NextStateID;
                                    m_vecEventProcessAccum.push_back( epaccum );
                                }
                                break;
                            }
                        }
                    }
				}

                if ( bReactState == true && _IsReactionState( pEventProcessData->m_NextStateID ) == true )
                    StateChange_Reaction_Continue( pEventProcessData->m_NextStateID );
                else
                    StateChange( pEventProcessData->m_NextStateID );

				KTDXPROFILE_END();
			}
			break;


		default:
			if( m_AIMessage > SCT_AI_MESSAGE_BEGIN && m_AIMessage < SCT_AI_MESSAGE_END )
			{
				m_UnUseAIMessage = m_AIMessage;
			}
			break;

		} // end of switch( stateChangeType )

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		if( m_NPCFrameData.syncData.bStateChange == true )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			break;
	}

    if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum == m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum
        && m_vecEventProcessAccum.empty() == false )
    {
        for( int i = 1; i < m_iFrameAccumCounter; i++ )
        {
            for( const EVENT_PROCESS_ACCUM& epaccum : m_vecEventProcessAccum )
            {
                if ( EstimateFrameOneshotPercent( (float) epaccum.m_iRate ) == true )
                {
                    StateChange( epaccum.m_iStateID );
                    goto out;
                }//if
            }//BOOST_FOREACH()
        }//for
    }
out:
    m_vecEventProcessAccum.resize( 0 );



//#ifdef TRANSFORMER_TEST
//
//
//	if( false == m_NPCFrameData.syncData.bStateChange &&
//		true == GetTransformed() &&
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//        false == GetNowStateData().m_vecKeyEventProcessData.empty()
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//		false == m_NPCStateData.m_vecKeyEventProcessData.empty()
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//        )
//	{
//		bool bKeyProcessed = true;
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//        for( const KeyEventProcessData& keyEvent : GetNowStateData().m_vecKeyEventProcessData )
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//		for( KeyEventProcessData& keyEvent : m_NPCStateData.m_vecKeyEventProcessData )
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//		{
//			bKeyProcessed = true;
//			for( KeyCombinationData keyCombination : keyEvent.m_vecKeyCombinationData )
//			{
//				switch( keyCombination.eType )
//				{
//				case KET_PURELEFT:
//				case KET_ONELEFT:
//				case KET_PUREDOUBLELEFT:
//				case KET_ONEDOUBLELEFT:
//					{
//						if( m_InputData.pureLeft != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PURERIGHT:
//				case KET_ONERIGHT:
//				case KET_PUREDOUBLERIGHT:
//				case KET_ONEDOUBLERIGHT:
//					{
//						if( m_InputData.pureRight != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREUP:
//				case KET_ONEUP:
//				case KET_PUREDOUBLEUP:
//				case KET_ONEDOUBLEUP:
//					{
//						if( m_InputData.pureUp != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREDOWN:
//				case KET_ONEDOWN:
//				case KET_PUREDOUBLEDOWN:
//				case KET_ONEDOUBLEDOWN:
//					{
//						if( m_InputData.pureDown != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//
//				case KET_PUREZ:
//				case KET_ONEZ:
//				case KET_PUREDOUBLEZ:
//				case KET_ONEDOUBLEZ:
//					{
//						if( m_InputData.pureZ != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREX:
//				case KET_ONEX:
//				case KET_PUREDOUBLEX:
//				case KET_ONEDOUBLEX:
//					{
//						if( m_InputData.pureX != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREC:
//				case KET_ONEC:
//				case KET_PUREDOUBLEC:
//				case KET_ONEDOUBLEC:
//					{
//						if( m_InputData.pureC != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREA:
//				case KET_ONEA:
//				case KET_PUREDOUBLEA:
//				case KET_ONEDOUBLEA:
//					{
//						if( m_InputData.pureA != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PURES:
//				case KET_ONES:
//				case KET_PUREDOUBLES:
//				case KET_ONEDOUBLES:
//					{
//						if( m_InputData.pureS != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PURED:
//				case KET_ONED:
//				case KET_PUREDOUBLED:
//				case KET_ONEDOUBLED:
//					{
//						if( m_InputData.pureD != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//				case KET_PUREF:
//				case KET_ONEF:
//				case KET_PUREDOUBLEF:
//				case KET_ONEDOUBLEF:
//					{
//						if( m_InputData.pureF != keyCombination.bPressed )
//						{
//							bKeyProcessed = false;
//						}
//
//					} break;
//
//
//
//					//case KET_PURELSHIFT:
//					//case KET_ONELSHIFT:
//					//case KET_PUREDOUBLELSHIFT:
//					//case KET_ONEDOUBLELSHIFT:
//
//					//case KET_PURELALT:
//					//case KET_ONELALT:
//					//case KET_PUREDOUBLELALT:
//					//case KET_ONEDOUBLELALT:
//
//					//case KET_PURESPACE:
//					//case KET_ONESPACE:
//					//case KET_PUREDOUBLESPACE:
//					//case KET_ONEDOUBLESPACE:
//
//					//case KET_PURE_LEFT_CTRL:
//					//case KET_ONE_LEFT_CTRL:
//					//case KET_PUREDOUBLE_LEFT_CTRL:
//					//case KET_ONEDOUBLE_LEFT_CTRL:
//					//	{
//					//	} break;
//
//				}
//
//				if( false == bKeyProcessed )
//					break;
//			}
//
//			if( true == bKeyProcessed )
//			{
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//                if ( m_NPCFrameData.syncData.nowState <= 0
//                    || m_NPCFrameData.syncData.nowState != keyEvent.m_cNextStateID )
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//				if( m_NPCStateData.stateID <= 0 ||
//					m_NPCStateData.stateID != (int)keyEvent.m_cNextStateID )
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//				{
//					switch( keyEvent.m_cIsRight )
//					{
//					default:
//					case -1:
//						{
//						} break;
//
//					case 0: // left
//						{
//#ifdef  X2OPTIMIZE_ROBUST_NPC_ROTATION
//                            SetIsRightNow( false );
//#else   X2OPTIMIZE_ROBUST_NPC_ROTATION
//							m_NPCFrameData.syncData.bIsRight = false;
//#endif  X2OPTIMIZE_ROBUST_NPC_ROTATION
//						} break;
//
//					case 1: // right
//						{
//#ifdef  X2OPTIMIZE_ROBUST_NPC_ROTATION
//                            SetIsRightNow( true );
//#else   X2OPTIMIZE_ROBUST_NPC_ROTATION
//							m_NPCFrameData.syncData.bIsRight = true;
//#endif  X2OPTIMIZE_ROBUST_NPC_ROTATION
//						} break;
//					}
//				}
//
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//				if( keyEvent.m_cNextStateID > 0 &&
//					m_NPCFrameData.syncData.nowState != (int)keyEvent.m_cNextStateID )
//				{
//					StateChange( keyEvent.m_cNextStateID );
//					m_NPCFrameData.syncData.position += keyEvent.m_vDisplace;
//				}
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//				if( keyEvent.m_cNextStateID > 0 &&
//					m_NPCStateData.stateID != (int)keyEvent.m_cNextStateID )
//				{
//					StateChange( keyEvent.m_cNextStateID );
//					m_NPCFrameData.syncData.position += keyEvent.m_vDisplace;
//				}
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//
//				break; // for( KeyCombinationData keyCombination : keyEvent.m_vecKeyCombinationData )
//			}
//		} // for( KeyEventProcessData& keyEvent : m_NPCStateData.m_vecKeyEventProcessData )
//
//
//		if( false == bKeyProcessed )
//		{
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//            int iKeyEventFailNextStateID = GetNowStateData().m_cKeyEventFailNextStateID;
//            if ( iKeyEventFailNextStateID > 0 &&
//                iKeyEventFailNextStateID != m_NPCFrameData.syncData.nowState )
//            {
//                StateChange( iKeyEventFailNextStateID );
//            }
//#else   X2OPTIMIZE_NPC_NONHOST_SIMULATION
//			if( m_NPCStateData.m_cKeyEventFailNextStateID > 0 &&
//				m_NPCStateData.m_cKeyEventFailNextStateID != m_NPCStateData.stateID )
//			{
//				StateChange( m_NPCStateData.m_cKeyEventFailNextStateID );
//			}
//#endif X2OPTIMIZE_NPC_NONHOST_SIMULATION
//		}
//	}
//
//#endif TRANSFORMER_TEST


}

void CX2GUNPC::CommonStateEndHost()
{
	KTDXPROFILE();

}


void CX2GUNPC::CommonStateEnd()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();


    m_NPCFrameData.unitCondition.bTransitionChange = NOWSTATE_FLAG( TRANSITION );

	for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		pWeapon->CommonStateEnd();
	}


    if ( nowStateData.m_vecwstrEnableAttackBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrEnableAttackBox )
        {
            SetEnableAttackBox( name.c_str(), false );
        }//BOOST_FOREACH()
    }//if

    if ( nowStateData.m_vecwstrDisableAttackBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrDisableAttackBox )
        {
            SetEnableAttackBox( name.c_str(), true );
        }//BOOST_FOREACH()
    }//if


    if ( nowStateData.m_vecwstrEnableCollisionBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrEnableCollisionBox )
        {
            SetEnableCollisionBox( name.c_str(), false );
        }//BOOST_FOREACH()
    }//if

    if ( nowStateData.m_vecwstrDisableCollisionBox.empty() == false )
    {
        for( const std::wstring& name : nowStateData.m_vecwstrDisableCollisionBox )
        {
            SetEnableCollisionBox( name.c_str(), true );
        }//BOOST_FOREACH()
    }//if


	m_PhysicParam.fDownAccel = 0.f;

	g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

    if ( NOWSTATE_FLAG( SET_COOL_TIME ) == true )
    {
        m_fCoolTime = nowStateData.m_fSetCoolTimeValue;
    }

    if ( NOWSTATE_FLAG( FLIP_DIR_END ) == true )
	{
         SetIsRightNow( !m_NPCFrameData.syncData.bIsRight );
	}

	m_iFollowHitter	= 0;
	m_bFollowHitterLost = false;

	m_PhysicParam.fGAccel = m_OrgPhysicParam.fGAccel;

	//이펙트 데이타
	//파티클


    vector<ParticleEffectData*>::iterator iterPNext;
	for( vector<ParticleEffectData*>::iterator iter = m_ParticleEffData.begin();
        iter != m_ParticleEffData.end();
        iter = iterPNext )
	{
        iterPNext = iter;   ++iterPNext;
		ParticleEffectData* pParticleEffectData = *iter;

		if( GetNowHp() <= 0.f || GetGameUnitState() == GUSI_DIE )
		{
			pParticleEffectData->m_StateEndDelete = true;
		}


		if( pParticleEffectData->m_bUse == true &&
			pParticleEffectData->m_hSeq == INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
            ParticleEffectData::Destroy( pParticleEffectData );
            iterPNext = EraseUnorderlyUsingSwap( m_ParticleEffData, iter );
		}
		else if( pParticleEffectData->m_bUse == false )
		{
            ParticleEffectData::Destroy( pParticleEffectData );
            iterPNext = EraseUnorderlyUsingSwap( m_ParticleEffData, iter );
		}
		else if( pParticleEffectData->m_StateEndDelete == true )
		{
			if( INVALID_PARTICLE_SEQUENCE_HANDLE != pParticleEffectData->m_hSeq )
			{
                CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticleEffectData->GetInstanceSequence();
				if( NULL != pSeq )
				{
					pSeq->SetAutoDie();
					pParticleEffectData->m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
				}
			}
            ParticleEffectData::Destroy( pParticleEffectData );
            iterPNext = EraseUnorderlyUsingSwap( m_ParticleEffData, iter );
		}
	}


	//메시플레이어
    vector<MeshPlayerEffectData*>::iterator iterMNext;
	for( vector<MeshPlayerEffectData*>::iterator iter = m_MeshPlayerEffData.begin();
        iter != m_MeshPlayerEffData.end();
        iter = iterMNext )
	{
        iterMNext = iter;   ++iterMNext;
		MeshPlayerEffectData* pMeshPlayerEffectData = *iter;
		if( pMeshPlayerEffectData->m_bUse == true && pMeshPlayerEffectData->m_hMeshInst == INVALID_MESH_INSTANCE_HANDLE )
		{
            MeshPlayerEffectData::Destroy( pMeshPlayerEffectData );
            iterMNext = EraseUnorderlyUsingSwap( m_MeshPlayerEffData, iter );
		}
		else if( pMeshPlayerEffectData->m_bUse == false )
		{
            MeshPlayerEffectData::Destroy( pMeshPlayerEffectData );
            iterMNext = EraseUnorderlyUsingSwap( m_MeshPlayerEffData, iter );
		}
		else if( pMeshPlayerEffectData->m_StateEndDelete == true )
		{
            MeshPlayerEffectData::Destroy( pMeshPlayerEffectData );
            iterMNext = EraseUnorderlyUsingSwap( m_MeshPlayerEffData, iter );
		}
	}



	if( m_hSeqStickShake != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqStickShake );
	}

	g_pX2Game->GetNPCFunc()->EndFunc( this );

    int iLua_StateEndRefID = GetNowStateRuntime().m_iLua_StateEndRefID;
	if( iLua_StateEndRefID != LUA_REFNIL )
	{
		lua_tinker::call<void>( GetFunctionLuaState(), m_LuaManager.GetLuaGeneralRef(), iLua_StateEndRefID, g_pKTDXApp, g_pX2Game, this );
	}

//	if( false == GetNowStateData().m_strLua_StateEnd.empty() )
//	{
//		string func;
//		ConvertWCHARToChar( func, GetNowStateData().m_strLua_StateEnd.c_str() );
//
//#ifdef LEAVE_LAST_ERROR_LOG_TEST
//		LastErrorLog( func.c_str() );
//#endif LEAVE_LAST_ERROR_LOG_TEST
//
////#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		lua_tinker::call<void>( GetFunctionLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
////		lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), func.c_str(), g_pKTDXApp, g_pX2Game, this );
////#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	}



    if ( nowStateData.m_veciEnableWeapon.empty() == false )
    {
        for( int weaponIndex : nowStateData.m_veciEnableWeapon )
        {
            SetWeaponEnable(weaponIndex, true);
        }//
    }//if

    if ( nowStateData.m_veciDisableWeapon.empty() == false )
    {
        for( int weaponIndex : nowStateData.m_veciDisableWeapon )
        {
            SetWeaponEnable( weaponIndex, false);
        }//
    }//if


    if ( NOWSTATE_FLAG( STOP_AT_CURRENT_STATE ) == true )
	{
		g_pX2Game->CanNotInputAndDisalbeNPCAI(false);
	}
    if( m_NPCFrameData.syncData.nowState == m_StartState )
	{
		SetStopAtStartState(false);
		g_pX2Game->CanNotInputAndDisalbeNPCAI(false);
	}


	// CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END 구문이 있을 때, 해당 State 가 끝나면 m_vecEffectSetToDeleteOnCustomState 내용을 모두 지운다.
    if ( NOWSTATE_FLAG( CUSTOM_STATE_DELETE_EFFECT_SET_ON_STATE_END ) == true )
	{
		DeleteEffectSetOnCustomState();
	}

	DeleteEffectSetOnStateEnd();

	DoCustomFunctionByBuffInCommonStateEnd();
}





//{{ robobeg : 2008-10-28
/*virtual*/
RENDER_HINT CX2GUNPC::OnFrameRender_Prepare()
{
	// 애니메이션을 로딩 못했거나, 싱크 패킷을 한번도 받지 못했으면
	if ( NULL == m_pXSkinAnim ||
		( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && false == DidReceiveFirstSyncPacket() ) )
		return RENDER_HINT_NORENDER;

    if ( m_NPCFrameData.syncData.nowState == CX2GameUnit::GUSI_DIE || m_pXSkinAnim == NULL )
        return RENDER_HINT_NORENDER;

    m_pXSkinAnim->SetInstantShow( false );
    if ( !m_pXSkinAnim->GetShowObject() )
        return RENDER_HINT_NORENDER;

	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH && g_pX2Game->GetWorld() != NULL && m_bFog == true)
	{
		CX2World *pWorld = g_pX2Game->GetWorld();

		m_RenderParam.bFog      = pWorld->m_bFog;
		m_RenderParam.fogNearX  = pWorld->m_fNearX;
		m_RenderParam.fogNearY  = pWorld->m_fNearY;
		m_RenderParam.fogFarX   = pWorld->m_fFarX;
		m_RenderParam.fogFarY   = pWorld->m_fFarY;
		m_RenderParam.fogDensity = pWorld->m_fDensity;
		m_RenderParam.fogColor  = pWorld->m_FogColor;
	}
	else if ( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH && g_pX2Game->GetWorld() != NULL && m_bUnitFog == true )
	{
		m_RenderParam.bFog      = m_bUnitFog;
		m_RenderParam.fogNearX  = m_fFogNearX;
		m_RenderParam.fogNearY  = m_fFogNearY;
		m_RenderParam.fogFarX   = m_fFogFarX;
		m_RenderParam.fogFarY   = m_fFogFarY;
		m_RenderParam.fogDensity = m_fFogDensity;
		m_RenderParam.fogColor  = m_cFogColor;
	}
    else
    {
        m_RenderParam.bFog = false;
    }

	CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
	*pRenderParam = m_RenderParam;

	if( m_bUnitColor == false && g_pX2Game->GetWorld() != NULL && m_bUseWorldColor == true )
	{
		pRenderParam->lightPos = g_pX2Game->GetWorld()->GetLightPos();
	}
	ApplyRenderParam( pRenderParam );

	if( GetDistanceToCamera() > 700.0f )
	{
		pRenderParam->fOutLineWide = min(GetDistanceToCamera() / 1000.0f, 1.f) * m_fOutLineWidthScale;
	}
	else
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 500.0f * m_fOutLineWidthScale;
	}

	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_MEDIUM )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
	}
	else if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_LOW )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
	}


	if( IsSuperArmor() && m_NPCFrameData.stateParam.bSuperArmorNotRed == false)
	{
		if( pRenderParam->renderType == CKTDGXRenderer::RT_CARTOON_BLACK_EDGE )
		{
			pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
			pRenderParam->outLineColor = 0xffff0000;
		}
	}




	/*
	if( m_DamageLightTime > 0.0f && g_pMain->GetGameOption().GetOptionList().m_UnitDetail != CX2GameOption::OL_LOW )
		{
			if( m_NPCFrameData.unitCondition.bAttackerFront == true )
			{
				if( m_NPCFrameData.syncData.bIsRight == true )
				{
					pRenderParam->lightPos = GetPos() + GetDirVector() * 1000.0f;
					pRenderParam->lightPos -= GetPos();
				}
				else
				{
					pRenderParam->lightPos = GetPos() - GetDirVector() * 1000.0f;
					pRenderParam->lightPos -= GetPos();
				}
			}
			else
			{
				if( m_NPCFrameData.syncData.bIsRight == true )
				{
					pRenderParam->lightPos = GetPos() - GetDirVector() * 1000.0f;
					pRenderParam->lightPos -= GetPos();
				}
				else
				{
					pRenderParam->lightPos = GetPos() + GetDirVector() * 1000.0f;
					pRenderParam->lightPos -= GetPos();
				}
			}

			pRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_DAMAGE;

			switch( pRenderParam->renderType )
			{
				case CKTDGXRenderer::RT_CARTOON_BLACK_EDGE:
					pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_BLACK_EDGE;
					break;

				case CKTDGXRenderer::RT_CARTOON_COLOR_EDGE:
					pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_COLOR_EDGE;
					break;

				case CKTDGXRenderer::RT_CARTOON:
					pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON;
					break;
			}
		}*/


	if( m_RageCountMax > 0 && m_fRageTime > 0.0f )
	{
		pRenderParam->color				= m_RageModeColor;
	}

	//{{ 2011-04 에 패치될 청 캐시 스킬
	// EDT_PAIN이 걸려있으면 다른 EDT에 의한 색 변경은 일어나지 않는다.
	if ( m_ExtraDamagePack.m_Pain.m_Accumulation == 0 )
		//}} 2011-04 에 패치될 청 캐시 스킬
	{
		//특수데미지 체크
		if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
		{
			pRenderParam->renderType			= CKTDGXRenderer::RT_CARTOON_FADE;
			m_RenderParam.color = D3DXCOLOR( 160.f/255.f, 235.f/255.f, 1.f, 0.7f );
		}
		else if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f )
		{
			pRenderParam->renderType			= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
			pRenderParam->cartoonTexType		= CKTDGXRenderer::CTT_PETRIFIED;
			pRenderParam->outLineColor			= D3DXCOLOR( 0.7f, 0.7f, 0.7f, 1.f );
		}
		else if( m_ExtraDamagePack.m_Cold.m_fTime > 0.0f || m_ExtraDamagePack.m_EnchantFrozen.m_fTime > 0.0f )
		{
			pRenderParam->color = D3DXCOLOR(0.15f, 0.8f, 1.f, 1.f );
		}
		else if( m_ExtraDamagePack.m_Fire.m_fTime > 0.0f || m_ExtraDamagePack.m_EnchantBlaze.m_fTime > 0.0f )
		{
			pRenderParam->color = 0xffff0000;
		}
		else if( m_ExtraDamagePack.m_Poison.m_fTime  > 0.0f || m_ExtraDamagePack.m_EnchantPoison.m_fTime > 0.0f )
		{
			pRenderParam->color = 0xff00ff00;
		}
		else if( m_ExtraDamagePack.m_Curse.m_fTime > 0.0f )
		{
			pRenderParam->color = 0xff6600ff;
		}
	}

	if( m_fShowInvincibleTime > 0.0f && m_bShowInvincibleWhite == true )
	{
		pRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_INVINCIBLE;
		pRenderParam->renderType		= CKTDGXRenderer::RT_ADD_CARTOON;
	}

	if( m_bUnderWaterHead == true || m_bForceChagneColor == true )
	{
		pRenderParam->color.r -= m_cLineUnitColor.r;
		pRenderParam->color.g -= m_cLineUnitColor.g;
		pRenderParam->color.b -= m_cLineUnitColor.b;
	}


    pRenderParam->color.a = m_RenderParam.color.a;

    RENDER_HINT renderHintResult = m_pXSkinAnim->OnFrameRender_Prepare();

    if ( renderHintResult.m_eHint != RENDER_HINT::NORENDER )
        m_pXSkinAnim->SetInstantShow( true );

    if ( !IsRecursivelyManagedByDGManager() && !m_vecpWeapon.empty() )
    {
        RENDER_HINT renderHint;
        for( UINT i = 0; i < m_vecpWeapon.size(); i++ )
        {
		    Weapon* pWeapon = m_vecpWeapon[i];
            ASSERT( pWeapon != NULL && !pWeapon->IsManagedByDGManager() );
            if ( pWeapon == NULL || pWeapon->IsManagedByDGManager() )
                continue;
            pWeapon->SetInstantShow( false );
            if ( !pWeapon->GetShowObject() )
                continue;
            renderHint = pWeapon->OnFrameRender_Prepare();
            if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
            {
                pWeapon->SetInstantShow( true );
                renderHintResult.MergeRenderHint( renderHint );
            }//if
        }//for
    }//if

	for(UINT i=0; i<m_vecAttachMesh.size(); ++i)
	{
		CX2SubEquip *pSubEquip = m_vecAttachMesh[i];
		if( pSubEquip != NULL  )
		{
			if(  pSubEquip->GetIsSkinMesh() == true )
			{
				CKTDGXRenderer::RenderParam *pSubRenderParam = pSubEquip->GetRenderParam();
				*pSubRenderParam = *pRenderParam;

				RENDER_HINT renderHint;
				renderHint = pSubEquip->OnFrameRender_Prepare();
				renderHintResult.MergeRenderHint( renderHint );
			}
		}
	}

    return renderHintResult;

}//CX2GUNPC::OnFrameRender_Prepare()
//}} robobeg : 2008-10-28


void CX2GUNPC::CommonRender_Draw()
{
	KTDXPROFILE();

	if ( NULL == m_pXSkinAnim )
		return;

    if ( m_pXSkinAnim != NULL && m_pXSkinAnim->GetShowObject() && m_pXSkinAnim->IsInstantShow() )
	{
		m_pXSkinAnim->OnFrameRender_Draw();
	}

    if ( !IsRecursivelyManagedByDGManager() && !m_vecpWeapon.empty() )
    {
	    for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	    {
		    Weapon* pWeapon = m_vecpWeapon[i];
            ASSERT( pWeapon != NULL && !pWeapon->IsManagedByDGManager() );
            if ( pWeapon == NULL || pWeapon->IsManagedByDGManager() )
                continue;
            if ( pWeapon->GetShowObject() && pWeapon->IsInstantShow() )
                pWeapon->OnFrameRender_Draw();
	    }//for
    }//if

 	for(vector<CX2SubEquip*>::iterator it = m_vecAttachMesh.begin(); it<m_vecAttachMesh.end(); ++it)
	{
		CX2SubEquip* pSubEquip = *it;
		if( pSubEquip != NULL && pSubEquip->GetIsSkinMesh() == true )
			pSubEquip->OnFrameRender_Draw();
	}
}//CX2GUNPC::CommonRender_Draw()


/*virtual*/
HRESULT CX2GUNPC::OnResetDevice()
{
    if ( !IsRecursivelyManagedByDGManager() )
    {
	    for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	    {
		    Weapon* pWeapon = m_vecpWeapon[i];
            ASSERT( !pWeapon->IsManagedByDGManager() );
            pWeapon->OnResetDevice();
	    }//for
    }//if
    return S_OK;
}//CX2GUNPC::OnResetDevice()

/*virtual*/
HRESULT	CX2GUNPC::OnLostDevice()
{
    if ( !IsRecursivelyManagedByDGManager() )
    {
	    for( UINT i=0; i<m_vecpWeapon.size(); i++ )
	    {
		    Weapon* pWeapon = m_vecpWeapon[i];
            ASSERT( !pWeapon->IsManagedByDGManager() );
            pWeapon->OnLostDevice();
	    }//for
    }//if
    return S_OK;
}//CX2GUNPC::OnLostDevice()







void CX2GUNPC::NeverMovingPhysicProcess()
{
	KTDXPROFILE();

    const StateData& nowStateData = GetNowStateData();

	ASSERT( true == m_bNeverMove );
	//ASSERT( NULL != GetNPCTemplet() );
	ASSERT( CX2UnitManager::NCT_THING_WORLD_BLOCK == GetNPCTemplet().m_ClassType );


	//현재 정보 받아오기
	D3DXVECTOR3 position = m_NPCFrameData.syncData.position;


	CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	ASSERT( NULL != pLineMap );
	if( NULL == pLineMap )
		return;



	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
	ASSERT( NULL != pLineData );
	if( pLineData == NULL )
	{
		InitPosition( false, 0 );
		pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
	}


	ASSERT( NULL != pLineData );
	if( NULL == pLineData )
		return;



	m_NPCFrameData.unitCondition.dirVector = pLineData->dirVector;



	//모션 이동
	if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f )
	{
        if ( NOWSTATE_FLAG( APPLY_ANIM_MOVE ) == true )
		{
			if( m_NPCFrameData.syncData.bIsRight == true )
			{
				position.x += m_NPCFrameData.unitCondition.dirVector.x * m_pXSkinAnim->GetMotionOffset().x;
				position.z += m_NPCFrameData.unitCondition.dirVector.z * m_pXSkinAnim->GetMotionOffset().x;
			}
			else
			{
				position.x -= m_NPCFrameData.unitCondition.dirVector.x * m_pXSkinAnim->GetMotionOffset().x;
				position.z -= m_NPCFrameData.unitCondition.dirVector.z * m_pXSkinAnim->GetMotionOffset().x;
			}
		}
	}


	D3DXVECTOR3 dirDegree( 0, 0, 0 );
    if ( pLineData != NULL )
    {
        bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
        m_NPCFrameData.syncData.bIsRight = true;
	    m_NPCFrameData.unitCondition.dirDegree = pLineData->dirDegree;
        m_NPCFrameData.unitCondition.dirDegree.z = 0;
        SetIsRightNow( bOldIsRight );
        dirDegree = m_NPCFrameData.unitCondition.dirDegree;
    }
    m_NPCFrameData.unitCondition.dirDegree = dirDegree;



	if( m_NPCFrameData.syncData.position.y == m_NPCFrameData.unitCondition.landPosition.y )
		m_NPCFrameData.unitCondition.bFootOnLine = true;
	else
		m_NPCFrameData.unitCondition.bFootOnLine = false;





}





void CX2GUNPC::PhysicProcess()
{
	KTDXPROFILE();

    bool bLocalUnit = IsLocalUnit();
    const StateData&    nowStateData = GetNowStateData();

	//물리 파라메터 계산//////////////////////////////////////////////////////////
	D3DXVECTOR3 position;
	D3DXVECTOR3 positionOrg;
	//D3DXVECTOR3 angleDegree;


	//현재 정보 받아오기
	positionOrg = position = m_NPCFrameData.syncData.position;
	//angleDegree	= m_NPCFrameData.unitCondition.dirDegree;

	CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	if( NULL == pLineMap )
	{
		ASSERT( !"monster physic process linemap is NULL" );
		return;
	}

	if( GetRideState() == RS_ON_RIDING )
	{
		if( GetRideType() == RT_RIDER ) // 라이더
		{
			CX2GUNPC* pRidingNPC = GetRideUnit();
			if ( pRidingNPC != NULL )
			{
				const CX2GUNPC::NPCFrameData& ridingFrameData = pRidingNPC->GetNPCFrameData();
				m_NPCFrameData.syncData.lastTouchLineIndex = ridingFrameData.syncData.lastTouchLineIndex;
				m_NPCFrameData.unitCondition.landPosition = ridingFrameData.unitCondition.landPosition;
			}
		}
	}

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
	if( pLineData == NULL )
	{
		InitPosition( false, 0 );
		pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
	}


	ASSERT( NULL != pLineData );
	if( NULL == pLineData )
		return;

    positionOrg = GetPositionBackup();
	const CKTDGLineMap::LineData* pLineDataOrg = pLineData;
    if ( m_NPCFrameData.syncData.lastTouchLineIndex != m_iPositionBackup_LineIndex )
    {
        pLineDataOrg = pLineMap->GetLineData( m_iPositionBackup_LineIndex );
        if ( pLineDataOrg == NULL )
        {
            m_iPositionBackup_LineIndex = m_NPCFrameData.syncData.lastTouchLineIndex;
            pLineDataOrg = pLineData;
        }
    }//if
	const CKTDGLineMap::LineData* pLineDataReserved = pLineData;
    if ( m_kNonHostReaction.IsReceivePosReserved() == true && m_kNonHostReaction.GetReservedReceiveLineIndex() != m_NPCFrameData.syncData.lastTouchLineIndex )
    {
        pLineDataReserved = pLineMap->GetLineData( m_kNonHostReaction.GetReservedReceiveLineIndex() );
        if ( pLineDataReserved == NULL )
        {
            m_kNonHostReaction.ResetReservedReceivePos();
            pLineDataReserved = pLineData;
        }
    }//if



//#pragma NOTE( "움직이지 않는 몬스터의 경우에 여기 코드 실행되지 않도록 고치기, 코드 정리, 함수 나누기도 같이 하면 좋음" )
	//if( false == m_bNeverMove )
	//{

	KTDXPROFILE_BEGIN("Speed");
	//X감쇄

    float   fActiveNowSpeedXPos = 0.f;
    float   fActiveNowSpeedYPos = 0.f;

	bool positive;
	if( m_PhysicParam.nowSpeed.x >= 0.0f )
		positive = true;
	else
		positive = false;
    if ( m_iFrameAccumCounter > 0 )
    {
	    if( positive == true )
	    {
            if ( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f && m_PhysicParam.passiveSpeed.x != -1.0f )
            {
		        m_PhysicParam.nowSpeed.x -= m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
		        if( m_PhysicParam.nowSpeed.x <= 0.0f )
			        m_PhysicParam.nowSpeed.x = 0.0f;
                else
                    fActiveNowSpeedXPos = m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime;
                if ( m_iFrameAccumCounter > 1 )
                {
                    m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x - m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
                    if( m_PhysicParam.nowSpeed.x <= 0.0f )
			            m_PhysicParam.nowSpeed.x = 0.0f;
                    else
                        fActiveNowSpeedXPos += m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime * ( m_iFrameAccumCounter - 1 );
                }
            }
            else
            {
                for( int i = 0; i < m_iFrameAccumCounter; i++ )
                {
                    m_PhysicParam.nowSpeed.x -= m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
		            if( m_PhysicParam.nowSpeed.x <= 0.0f )
                    {
			            m_PhysicParam.nowSpeed.x = 0.0f;
                        break;
                    }
                    else
                        fActiveNowSpeedXPos += m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime;
                }//if
            }//if
	    }
	    else
	    {
            if ( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f && m_PhysicParam.passiveSpeed.x != -1.0f )
            {
		        m_PhysicParam.nowSpeed.x += m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
		        if( m_PhysicParam.nowSpeed.x >= 0.0f )
			        m_PhysicParam.nowSpeed.x = 0.0f;
                else
                    fActiveNowSpeedXPos = m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime;
                if ( m_iFrameAccumCounter > 1 )
                {
                    m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x + m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
		            if( m_PhysicParam.nowSpeed.x >= 0.0f )
			            m_PhysicParam.nowSpeed.x = 0.0f;
                    else
                        fActiveNowSpeedXPos += m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime * ( m_iFrameAccumCounter - 1 );
                }
            }
            else
            {
                for( int i = 0; i < m_iFrameAccumCounter; i++ )
                {
                    m_PhysicParam.nowSpeed.x += m_PhysicParam.fReloadAccel * m_fOneShotElapsedTime;
                    if ( m_PhysicParam.nowSpeed.x >= 0.0f )
                    {
                        m_PhysicParam.nowSpeed.x = 0.0f;
                        break;
                    }
                    else
                        fActiveNowSpeedXPos += m_PhysicParam.nowSpeed.x * m_fOneShotElapsedTime;
                }
            }
	    }
    }


	if( m_ExtraDamagePack.m_Frozen.m_fTime > 0.0f )
	{
		if(m_pAI != NULL && m_pAI->GetAIFly() == true)
		{
			if( m_bDoneBackupSpeed == false )
			{
				m_bDoneBackupSpeed = true;
				m_vNowSpeedBackup = m_PhysicParam.nowSpeed;
				m_vpassiveSpeedBackup = m_PhysicParam.passiveSpeed;
			}
			m_PhysicParam.nowSpeed.x = 0.f;
			m_PhysicParam.nowSpeed.y = 0.f;
			m_PhysicParam.passiveSpeed.y = 0.f;
            fActiveNowSpeedXPos = 0.f;
            fActiveNowSpeedYPos = 0.f;
		}
	}


    if ( m_iFrameAccumCounter > 0 )
    {
        bool bPassive = m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f && m_PhysicParam.passiveSpeed.y != -1.0f;

    m_fReverseGravitySpeedDeltaY = 0.f;

	if( true == g_pX2Game->IsInReverseGravityRegion(
        m_fOneShotElapsedTime,
        GetPos(true), m_fReverseGravitySpeedDeltaY, m_vReverseGravityRotateSpeedDelta, GetTeam() ) )
	{
        for( int i = 0; i < m_iFrameAccumCounter; i++ )
        {
            if ( i > 0 && bPassive == true )
                m_PhysicParam.nowSpeed.y = m_PhysicParam.passiveSpeed.y; // * m_PhysicParam.fSpeedFactor;
		    if( m_PhysicParam.nowSpeed.y > 200.f )
		    {
			    m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fOneShotElapsedTime;
			    if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed )
				    m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
		    }
		    else if( m_PhysicParam.nowSpeed.y < -200.f )
		    {
			    float fScale = m_PhysicParam.nowSpeed.y / -200.f;
			    m_PhysicParam.nowSpeed.y += m_fReverseGravitySpeedDeltaY * fScale;
		    }
		    else
		    {
			    m_PhysicParam.nowSpeed.y += m_fReverseGravitySpeedDeltaY;
		    }

		    if( m_PhysicParam.nowSpeed.y < -2000.f )
			    m_PhysicParam.nowSpeed.y = -2000.f;

		    if( m_PhysicParam.nowSpeed.y > 2000.f )
			    m_PhysicParam.nowSpeed.y = 2000.f;

            fActiveNowSpeedYPos += m_PhysicParam.nowSpeed.y * m_fOneShotElapsedTime;

		    if( i == 0 && m_NPCFrameData.unitCondition.bFootOnLine == true )
		    {
			    position.y += LINE_RADIUS * 1.5f;
			    StateChange( m_UpDamage ); // land connect constraints 가 작동하지 않게 막았기 때문에 상태바꿀 필요 없이 그냥 뜬다
                break;
		    }
        }
	}
	else

	{
//#pragma DMLEE_NOTE( "여기에 캐릭터 코드 참고해서 m_bDisableGravity, m_bDisableGravityInScript 작업해야함" )


		//Y감쇄

		float fBuoyancy = 1.f;
		if( pLineData != NULL && pLineData->m_bUnderWater == true &&
			GetPos().y <= pLineData->m_fWaterHeight )
		{
			fBuoyancy = pLineData->m_fBuoyancy;
		}

		if( m_NPCFrameData.stateParam.bLandConnect == false && NULL == m_pAI
		    || m_NPCFrameData.stateParam.bLandConnect == false && m_pAI != NULL && m_pAI->GetAIFlyNowState() == false
          )
		{
            for( int i = 0; i < m_iFrameAccumCounter; i++ )
            {
                if ( i > 0 && bPassive == true )
                    m_PhysicParam.nowSpeed.y = m_PhysicParam.passiveSpeed.y; // * m_PhysicParam.fSpeedFactor;
			    m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fOneShotElapsedTime * fBuoyancy;
			    if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed * fBuoyancy )
				    m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * fBuoyancy;
			    m_PhysicParam.nowSpeed.y -= m_PhysicParam.fDownAccel * m_fOneShotElapsedTime;
			    if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed * fBuoyancy )
				    m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * fBuoyancy;
                fActiveNowSpeedYPos += m_PhysicParam.nowSpeed.y * m_fOneShotElapsedTime;
            }

		}
		else
		{
			m_PhysicParam.nowSpeed.y = 0.0f;
            fActiveNowSpeedYPos = 0.f;
		}
	}
    }



	//X이동
	m_NPCFrameData.unitCondition.dirVector = pLineData->dirVector;

	float fSlowFactor = 1.0f;
	if( m_ExtraDamagePack.m_Slow.m_fTime > 0.0f )
	{
		fSlowFactor = 0.5f;
	}

	fSlowFactor *= m_PhysicParam.GetMoveSpeedPower();

	if( pLineData != NULL && pLineData->m_bUnderWater == true && GetPos().y <= pLineData->m_fWaterHeight )
	{
		fSlowFactor *= pLineData->m_fResistance;
	}

	if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f )
	{
		if( m_NPCFrameData.unitCondition.bFootOnLine == false )
		{
			D3DXVECTOR3 dirVec = m_NPCFrameData.unitCondition.dirVector;
			dirVec.y = 0.0f;
			if( m_NPCFrameData.syncData.bIsRight == true )
				position += dirVec * fActiveNowSpeedXPos * GetWalkSpeed();
			else
				position -= dirVec * fActiveNowSpeedXPos * GetWalkSpeed();
		}
		else
		{
			if( m_NPCFrameData.syncData.bIsRight == true )
				position += m_NPCFrameData.unitCondition.dirVector * fActiveNowSpeedXPos * GetWalkSpeed();
			else
				position -= m_NPCFrameData.unitCondition.dirVector * fActiveNowSpeedXPos * GetWalkSpeed();
		}
	}


	//라인 스피드 적용
	if( false == m_bIgnoreLineSpeed )
	{
		if( true == m_NPCFrameData.unitCondition.bFootOnLine )
		{
			position += pLineData->dirVector * pLineData->m_fSpeed * m_fElapsedTime;
		}
	}

//#ifdef LINEMAP_SLOW_WIND_TEST
//#ifdef AI_FLY
//	if( false == IsOnSomething() && m_pAI != NULL && m_pAI->GetAIFlyNowState() == false)
//#else
//	if( false == IsOnSomething() )
//#endif
//	{
//		if( NULL != pLineData &&
//			0.f != pLineData->m_fWindRange )
//		{
//			float fDist = m_NPCFrameData.syncData.position.y - m_NPCFrameData.unitCondition.landPosition.y;
//			if( fDist < 0.f )
//				fDist = -fDist;
//
//			if( fDist < pLineData->m_fWindRange )
//			{
//				D3DXVECTOR3 vWindSpeed = pLineData->dirVector;
//				if( 0.f != vWindSpeed.y )
//				{
//					vWindSpeed.y = 0.f;
//					D3DXVec3Normalize( &vWindSpeed, &vWindSpeed );
//				}
//				vWindSpeed *= pLineData->m_vWindSpeed.x;
//				vWindSpeed.y = pLineData->m_vWindSpeed.y;
//
//				position += vWindSpeed * m_fElapsedTime;
//			}
//		}
//	}
//
//#endif LINEMAP_SLOW_WIND_TEST






	//모션 이동
	if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f )
	{
        if ( NOWSTATE_FLAG( APPLY_ANIM_MOVE ) == true )
		{
			if( m_NPCFrameData.syncData.bIsRight == true )
			{
				position.x += m_NPCFrameData.unitCondition.dirVector.x * m_pXSkinAnim->GetMotionOffset().x;
				position.z += m_NPCFrameData.unitCondition.dirVector.z * m_pXSkinAnim->GetMotionOffset().x;
			}
			else
			{
				position.x -= m_NPCFrameData.unitCondition.dirVector.x * m_pXSkinAnim->GetMotionOffset().x;
				position.z -= m_NPCFrameData.unitCondition.dirVector.z * m_pXSkinAnim->GetMotionOffset().x;
			}
		}
	}



//#ifdef STEP_ON_MONSTER_TEST
//	m_NPCFrameData.unitCondition.bFootOnUnit = false;
//#endif STEP_ON_MONSTER_TEST



	//다른 캐릭터에 의해 밀림 혹은 막힘

    bool bCollisionOccurred = false;
    int iLineDataType = 1;
    const CKTDGLineMap::LineData* pLineDataCur = pLineData;
    int ilastTouchLineIndexCur = m_NPCFrameData.syncData.lastTouchLineIndex;

	if( m_NPCFrameData.stateParam.bCanPassUnit == false )
	{
        D3DXVECTOR2 vMyUnitMinMaxY( position.y, position.y + GetUnitHeight( true ) );
        D3DXVECTOR2 vMyUnitMinMaxYCur( GetPositionBackup().y, GetPositionBackup().y + GetUnitHeight( true ));


        int  iUnitSize = g_pX2Game->GetUnitNum();
        CX2GameUnit*    pMyUnit = NULL;
		for( int iIndex = 0; iIndex <= iUnitSize; iIndex++ )
		{
            CX2GameUnit*    pGameUnit = NULL;
            bool            bMyUnit = false;
            if ( iIndex < iUnitSize )
            {
                pGameUnit = g_pX2Game->GetUnit( iIndex );
                if ( pGameUnit != NULL && pGameUnit->GetGameUnitType() == GUT_USER && pGameUnit->IsMyUnit() == true )
                {
                    pMyUnit = pGameUnit;
                    pGameUnit = NULL;
                }
            }
            else
            {
                if ( pMyUnit != NULL )
                {
                    pGameUnit = pMyUnit;
                    bMyUnit = true;
                }
            }

            if ( pGameUnit == NULL )
                continue;

            bool bUserUnit = pGameUnit->GetGameUnitType() == GUT_USER;

			if ( pGameUnit->GetTeam() != GetTeam() &&
					pGameUnit->GetGameUnitState() != GUSI_DIE &&
					!( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && bUserUnit == true && GetAllyTeam() != CX2Room::TN_NONE ) && /// 배틀필드에서 소환된 몬스터와 유저가 부딪힌게 아니라면
                    !( g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD && bUserUnit == true && bMyUnit == false && pGameUnit->DidReceiveFirstSyncPacket() == false ) )
			{
				if (  false == pGameUnit->GetStateparam( true ).bCanPassUnit )
				{
                    int  iLocalDirection = 0;
                    int  iDirection = 0;
                    bool bReservePosReliable = false;
                    if ( bUserUnit == true )
                    {
                        m_kNonHostReaction.PhysicProcess_RetrieveRelativePosInfo( bLocalUnit, pGameUnit->GetUnitIndex(), bMyUnit,
                            m_iFrameAccumCounter,
                            OUT iLocalDirection, OUT iDirection, OUT bReservePosReliable );
                    }

					// 내가 밀릴 수 없고, 상대가 밀릴 수 있는 상태라면
					if ( !( false == m_NPCFrameData.stateParam.bCanPushUnit &&
						pGameUnit->GetStateparam( true ).bCanPushUnit ) )
                    {
                        vMyUnitMinMaxY.x = position.y;
                        vMyUnitMinMaxY.y = position.y + GetUnitHeight( true );
                        int iType = 0;
                        if ( bUserUnit == false )
                            iType = ProcessCanPushNPCUnit( static_cast<CX2GUNPC*>(pGameUnit), vMyUnitMinMaxY, vMyUnitMinMaxYCur, position );
                        else
                            iType = ProcessCanPushUserUnit( static_cast<CX2GUUser*>(pGameUnit), vMyUnitMinMaxY, position, iDirection, iLocalDirection, bReservePosReliable );
                        if ( ( iType & 0x8 ) != 0 )
                            bCollisionOccurred = true;
                        switch( iType & 0x7 )
                        {
                        case 1:
                            iLineDataType = 1;
                            continue;
                        case 2:
                            if ( iLineDataType != 2 )
                            {
                                iLineDataType = 2;
                                pLineData = pLineDataOrg;
                                m_NPCFrameData.unitCondition.dirVector = pLineDataOrg->dirVector;
                                m_NPCFrameData.syncData.lastTouchLineIndex = m_iPositionBackup_LineIndex;
                            }
                            continue;
                        case 3:
                            if ( m_kNonHostReaction.IsReceivePosReserved() == true && iLineDataType != 3 )
                            {
                                iLineDataType = 3;
                                pLineData = pLineDataReserved;
                                m_NPCFrameData.unitCondition.dirVector = pLineDataReserved->dirVector;
                                m_NPCFrameData.syncData.lastTouchLineIndex = m_kNonHostReaction.GetReservedReceiveLineIndex();
                            }
                            continue;
                        case 4:
                            {
                                iLineDataType = 4;
                            }
                            continue;
                        case 5:
                            if ( iLineDataType != 5 )
                            {
                                iLineDataType = 5;
                                pLineData = pLineDataCur;
                                m_NPCFrameData.unitCondition.dirVector = pLineDataCur->dirVector;
                                m_NPCFrameData.syncData.lastTouchLineIndex = ilastTouchLineIndexCur;
                            }
                            continue;
                        default:
                            break;
                        }
                    }
					/// 투과 방지
                    if ( ProcessCanPassUnit( pGameUnit, position ) == true )
                    {
                        position = positionOrg;
					    bCollisionOccurred = true;
                        if ( iLineDataType != 2 )
                        {
                            iLineDataType = 2;
                            pLineData = pLineDataOrg;
                            m_NPCFrameData.unitCondition.dirVector = pLineDataOrg->dirVector;
                            m_NPCFrameData.syncData.lastTouchLineIndex = m_iPositionBackup_LineIndex;
                        }
                    }
				}
			}
		}
	}
    if ( bCollisionOccurred == true )
    {
        m_PhysicParam.nowSpeed.x = 0.0f;
    }


	//Y이동
	if( NULL == m_pAI ||
		(m_pAI != NULL && m_pAI->GetAIFlyNowState() == false) )
	{
		if( m_PhysicParam.nowSpeed.y > 0.f &&
			false == pLineMap->CanPassUp( m_NPCFrameData.syncData.position ) )
		{
			position.y = m_NPCFrameData.syncData.position.y;
			m_PhysicParam.nowSpeed.y = 0.f;
            fActiveNowSpeedYPos = 0.f;
		}
		else
		{
			if( D3DXVECTOR3(0, 0, 0 ) == m_vDisplaceUnitPosition )
			{
				float fJumpSpeedPower = 1.f;

                if( fActiveNowSpeedYPos > 0.f )
					fJumpSpeedPower = GetJumpSpeed();

                position.y += fActiveNowSpeedYPos * fJumpSpeedPower;
			}
		}
	}

	// 외부요인에 의한 캐릭터 위치변화가 있다면
	if( D3DXVECTOR3(0, 0, 0 ) != m_vDisplaceUnitPosition )
	{
		if( true == GetShowObject() &&
			false == m_bNeverMove &&
			GetNowHp() > 0.f )
		{

			ResetOnSomething();

			position += m_vDisplaceUnitPosition;

			float fDistSq = D3DXVec3LengthSq( &m_vDisplaceUnitPosition );
			if(fDistSq <= 81.f)
			{
                m_NPCFrameData.stateParam.bCanPassUnit = ( m_NPCFrameData.syncData.bComeBackState == true )
                    ? true : NOWSTATE_FLAG( SET_CAN_PASS_UNIT_VALUE );
			}
		}
		ClearDisplaceUnitPosition();
	}


	KTDXPROFILE_END();




	// 벽에 부딪혔는지
	m_NPCFrameData.unitCondition.bFootOnWall = false;
	bool bCanWallJump = false;
    if( true == pLineMap->CrossedWall( positionOrg, position, bCanWallJump ) )
	{
		if( false == IsOnSomething() &&
			true == bCanWallJump )
		{
			m_NPCFrameData.unitCondition.bFootOnWall = true;
		}

		position.x = positionOrg.x;
		position.z = positionOrg.z;
        m_NPCFrameData.syncData.lastTouchLineIndex = m_iPositionBackup_LineIndex;
	}












	//지면 계산////////////////////////////////////////////////////////////////////
	bool bTouch	= false;
	int lastTouchLineIndex		= m_NPCFrameData.syncData.lastTouchLineIndex;
	D3DXVECTOR3 landPosition	= m_NPCFrameData.unitCondition.landPosition;

	CX2GUNPCAI* pNormalAI = NULL;
	if( m_pAI != NULL &&
		( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL || m_pAI->GetAIType() == m_pAI->CX2NPCAI::NAT_FIELD ) )
	{
		pNormalAI = (CX2GUNPCAI*) m_pAI;
	}

	if(pNormalAI != NULL && pNormalAI->GetAIFlyNowState() == true)
	{
		bTouch = pLineMap->IsOnLine( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, true );

		if(bTouch == true)
		{
			//pLineMap->IsOnLineConnect( position, &lastTouchLineIndex, LINE_RADIUS, &landPosition, true );
			position.x = landPosition.x;
			position.z = landPosition.z;
		}
		else
		{
            const CKTDGLineMap::LineData* pDwnLineData = pLineDataOrg;

			bool bMeOnStart = false;
			bool bMeOnEnd = false;
            if ( pDwnLineData != NULL )
            {
			    if( GetDistance(landPosition,pDwnLineData->startPos) < LINE_RADIUS && pDwnLineData->beforeLine == -2 && lastTouchLineIndex == m_iPositionBackup_LineIndex )
				    bMeOnStart = true;
			    else if( GetDistance(landPosition,pDwnLineData->endPos) < LINE_RADIUS && pDwnLineData->nextLine == -2 && lastTouchLineIndex == m_iPositionBackup_LineIndex )
				    bMeOnEnd = true;

				switch ( pDwnLineData->lineType )
				{
				case CKTDGLineMap::LT_UNIT_ROAD:
				case CKTDGLineMap::LT_JUMP_UP_ABS:
				case CKTDGLineMap::LT_JUMP_UP_REL:
					bMeOnStart = bMeOnEnd = false;
					break;

				default:
					break;
				}
            }

            if ( bMeOnStart == true )
            {
				//if( position.y < landPosition.y )
				//	position.y = landPosition.y;
				landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else if( bMeOnEnd == true )
			{
				//if( position.y < landPosition.y )
				//	position.y = landPosition.y;

				landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else
			{
				bool bCanDown = pLineMap->CanDown( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, true, true, false );

				if( bCanDown == true ) //아래쪽에 발판이 있어서 내려갈 수가 있다면
				{
					//수평 위치를 아래쪽 선에 싱크한다.
					position.x = landPosition.x;
					position.z = landPosition.z;

					//if( m_PhysicParam.nowSpeed.y <= 0.0f )
					//{
					//	m_NPCFrameData.unitCondition.bFall = true;
					//}
				}
				else //더이상 아래쪽에 발판이 없어 떨어질 상황이면
				{
					//position = positionOrg;	//이번 프레임의 이동을 캔슬한다.
					position.x = positionOrg.x;
					position.z = positionOrg.z;
					//if( position.y < landPosition.y )
					//	position.y = landPosition.y;
                    lastTouchLineIndex = m_iPositionBackup_LineIndex;
				}
			}
		}

		if(GetNowHp() > 0.f)
		{
            if ( NOWSTATE_FLAG( ENABLE_HEIGHT_FIX ) == false )
			{
				position.y += m_PhysicParam.passiveSpeed.y;
			}
		}
		else
		{
			if(pNormalAI != NULL)
				pNormalAI->SetAIFly(false);
		}
	}
	else
	{
		bTouch = pLineMap->IsOnLine( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, true );

		if( bTouch == true ) //이동 위치가 선 위라면
		{
			if( m_PhysicParam.nowSpeed.y > 0.0f )
			{
				//상승 중이면 수평 위치만 선에 싱크한다.
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else if(
                0.f != nowStateData.m_v2IgnoreLineTime.y &&
				m_pXSkinAnim->GetNowAnimationTime() >= nowStateData.m_v2IgnoreLineTime.x &&
				m_pXSkinAnim->GetNowAnimationTime() < nowStateData.m_v2IgnoreLineTime.y
                )
			{
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else
			{
				//하강이나 정지상태면 위치를 선에 싱크한다.
                if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
					position = landPosition;

				//땅을 밟았으므로 Y속도를 없앤다.
				m_PhysicParam.nowSpeed.y = 0.0f;
				m_PhysicParam.fDownAccel = 0.0f;
			}
		}
		else //이동 위치가 선 위가 아니라면
		{
			//지금 반드시 땅을 밟고 있어야 하는 상황이라면
			if( m_NPCFrameData.stateParam.bLandConnect == true
				&& pLineMap->IsOnLineConnect( position, &lastTouchLineIndex, LINE_RADIUS, &landPosition, true ) == true )
			{
                if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
					position = landPosition; //현재 위치를 땅에 싱크한다.

				//땅을 밟았으므로 Y속도를 없앤다.
				m_PhysicParam.nowSpeed.y = 0.0f;
				m_PhysicParam.fDownAccel = 0.0f;
			}
			else if( GetRideType() == CX2GUNPC::RT_RIDER && m_NPCFrameData.stateParam.bLandConnect == true
				&& pLineMap->IsOnLineConnect( position, &lastTouchLineIndex, 80.f, &landPosition, true ) == true )
			{
                if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
					position = landPosition; //현재 위치를 땅에 싱크한다.

				//땅을 밟았으므로 Y속도를 없앤다.
				m_PhysicParam.nowSpeed.y = 0.0f;
				m_PhysicParam.fDownAccel = 0.0f;
			}
			else //점프나 하강등 땅을 밟지 않는것이 정상인 상황이라면
			{
                const CKTDGLineMap::LineData* pDwnLineData = pLineDataOrg;

				bool bMeOnStart = false;
				bool bMeOnEnd = false;
				if ( NULL != pDwnLineData )
				{
					if( GetDistance(landPosition,pDwnLineData->startPos) < LINE_RADIUS && pDwnLineData->beforeLine == -2 && lastTouchLineIndex == m_iPositionBackup_LineIndex )
						bMeOnStart = true;
					else if( GetDistance(landPosition,pDwnLineData->endPos) < LINE_RADIUS && pDwnLineData->nextLine == -2  && lastTouchLineIndex == m_iPositionBackup_LineIndex )
						bMeOnEnd = true;


					switch ( pDwnLineData->lineType )
					{
					case CKTDGLineMap::LT_UNIT_ROAD:
					case CKTDGLineMap::LT_JUMP_UP_ABS:
					case CKTDGLineMap::LT_JUMP_UP_REL:
						bMeOnStart = bMeOnEnd = false;
						break;

					default:
						break;
					}
                }

				if( bMeOnStart == true )
				{
                    if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
					{
						if( position.y < landPosition.y )
							position.y = landPosition.y;
					}
					landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
					position.x = landPosition.x;
					position.z = landPosition.z;
				}
				else if( bMeOnEnd == true )
				{
                    if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
					{
						if( position.y < landPosition.y )
							position.y = landPosition.y;
					}
					landPosition = pLineMap->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
					position.x = landPosition.x;
					position.z = landPosition.z;
				}
				else
				{
					bool bCanDown = pLineMap->CanDown( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, true, true, false );

					if( bCanDown == true ) //아래쪽에 발판이 있어서 내려갈 수가 있다면
					{
						//수평 위치를 아래쪽 선에 싱크한다.
						position.x = landPosition.x;
						position.z = landPosition.z;

						//if( m_PhysicParam.nowSpeed.y <= 0.0f )
						//{
						//	m_NPCFrameData.unitCondition.bFall = true;
						//}
					}
					else //더이상 아래쪽에 발판이 없어 떨어질 상황이면
					{



						//position = positionOrg;	//이번 프레임의 이동을 캔슬한다.
						position.x = positionOrg.x;
						position.z = positionOrg.z;
                        lastTouchLineIndex = m_iPositionBackup_LineIndex;
                        if ( NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false )
						{
							if( position.y < landPosition.y )
								position.y = landPosition.y;
						}

                        if ( m_pInitData != NULL && m_pInitData->m_bHeuristicSimulation == true
                            && m_NPCFrameData.unitCondition.bFootOnLine == true
                            && m_PhysicParam.nowSpeed.y < 0.f )
                        {
			                m_PhysicParam.nowSpeed.y = 0.0f;
			                m_PhysicParam.fDownAccel = 0.0f;
                        }



					}
				}
			}
		}
	}







//{{ kimhc // 2010.7.7 // 몬스터가 생성한 라인맵
	pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );

	// 몬스터가 생성한 라인맵인데 diable 한 경우라면
	if ( pLineData != NULL && pLineData->bEnable == false && pLineData->m_bTempLineMap == true )
	{
		D3DXVECTOR3 outPos( 0.0f, 0.0f, 0.0f );
		int iLineIndex = -1;
        pLineMap->GetDownNearestWithBound( position, FLT_MAX, &outPos, &iLineIndex );
		pLineData = pLineMap->GetLineData( iLineIndex );

		if ( pLineData != NULL )
		{
			lastTouchLineIndex	= iLineIndex;
			landPosition		= outPos;
		}
	}

	// line data가 없거나(위의 과정을 거쳤을 경우도 포함), 몬스터가 생성한 라인맵이 아닌데 disable한 경우
	if( pLineData == NULL || lastTouchLineIndex < 0 ||
		( pLineData->bEnable == false && pLineData->m_bTempLineMap == false ) )
	{
		lastTouchLineIndex = pLineMap->GetNearestLine( position, &landPosition, true );
		//라인정보를 가지고 온다
		pLineData = pLineMap->GetLineData( lastTouchLineIndex );
	}

	m_NPCFrameData.syncData.lastTouchLineIndex	= lastTouchLineIndex;
	m_NPCFrameData.unitCondition.landPosition	= landPosition;
	m_NPCFrameData.unitCondition.bTempLineMap	= ( NULL != pLineData ? pLineData->m_bTempLineMap : false );

//}} kimhc // 2010.7.7 // 몬스터가 생성한 라인맵



	D3DXVECTOR3 dirDegree( 0, 0, 0 );
    if ( pLineData != NULL )
    {
        bool bOldIsRight = m_NPCFrameData.syncData.bIsRight;
        m_NPCFrameData.syncData.bIsRight = true;
	    m_NPCFrameData.unitCondition.dirDegree = pLineData->dirDegree;
        m_NPCFrameData.unitCondition.dirDegree.z = 0;
        SetIsRightNow( bOldIsRight );
        dirDegree = m_NPCFrameData.unitCondition.dirDegree;
        m_NPCFrameData.unitCondition.dirVector = pLineDataOrg->dirVector;
    }
    m_NPCFrameData.unitCondition.dirDegree = dirDegree;

	//위치 업데이트////////////////////////////////////////////////////////////////

    if ( m_bClientHoldPosition == true )
    {
        if ( IsLocalUnit() == true )
        {
            m_bClientHoldPosition = false;
            m_NPCFrameData.syncData.position			= position;
        }
    }
    else
    {
        m_NPCFrameData.syncData.position			= position;
    }


	if( m_NPCFrameData.syncData.position.y == m_NPCFrameData.unitCondition.landPosition.y )
		m_NPCFrameData.unitCondition.bFootOnLine = true;
	else
		m_NPCFrameData.unitCondition.bFootOnLine = false;



	//PhysicProcess_FastUpwardWind( pLineMap );
	PhysicProcess_Bungee( pLineMap );

	//}
}

/*virtual*/
void   CX2GUNPC::OnFrameMove_PostProcess()
{
    m_kNonHostReaction.PhysicProcess_UpdateRelativePos( IsLocalUnit(), m_NPCFrameData, GetUnitHeight( true ), GetTeam(), GetAllyTeam() );
    BackupPosition();
}


//void CX2GUNPC::PhysicProcess_FastUpwardWind( CKTDGLineMap* pLineMap )
//{
//#ifdef LINEMAP_FAST_WIND_TEST
//
//	if( true == m_bEnableUpsideWind )
//	{
//		if( GetNowHp() > 0.f &&
//			false == GetNeverMove() )
//		{
//			const CKTDGLineMap::LineData* pLineData_Land = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
//			if( NULL != pLineData_Land &&
//				0.f != pLineData_Land->m_fUpsideWindAccelaration )
//			{
//				float fDist = m_NPCFrameData.syncData.position.y - m_NPCFrameData.unitCondition.landPosition.y;
//				if( fDist < 0.f )
//					fDist = -fDist;
//
//				float fSlope = -pLineData_Land->m_fUpsideWindAccelaration / pLineData_Land->m_fUpsideWindRange;
//				float fUpSideAccel = pLineData_Land->m_fUpsideWindAccelaration + fSlope * fDist;
//
//				if( fUpSideAccel > 0.f )
//				{
//					if( true == m_NPCFrameData.unitCondition.bFootOnLine )
//					{
//						ResetOnSomething();
//
//						m_NPCFrameData.syncData.position.y += LINE_RADIUS;
//					}
//
//					m_PhysicParam.nowSpeed.y += fUpSideAccel * m_fElapsedTime;
//
//					if( 0.f == m_PhysicParam.nowSpeed.x )
//					{
//						const float MAGIC_X_SPEED = 400.f;
//						if( true == m_NPCFrameData.syncData.bIsRight )
//						{
//							m_NPCFrameData.syncData.position.x += MAGIC_X_SPEED * m_fElapsedTime;
//						}
//						else
//						{
//							m_NPCFrameData.syncData.position.x -= MAGIC_X_SPEED * m_fElapsedTime;
//						}
//					}
//				}
//			}
//		}
//	}
//
//#endif LINEMAP_FAST_WIND_TEST
//}



void CX2GUNPC::PhysicProcess_Bungee( CKTDGLineMap* pLineMap )
{
	CX2GUNPCAI* pNormalAI = NULL;
	if( m_pAI != NULL && m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
	{
		pNormalAI = (CX2GUNPCAI*) m_pAI;
	}



	const float MAGIC_REBIRTH_WAIT_TIME = 2.f;

	bool bAIFly = true;
	if( pNormalAI == NULL || (pNormalAI != NULL && pNormalAI->GetAIFly() == false) )
		bAIFly = false;

	if( false == m_bWaitingForRebirthAfterBungee &&
		true == m_NPCFrameData.unitCondition.bFootOnLine &&
		GetNowHp() > 0.f
		&& bAIFly == false
		)
	{
		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
		if( NULL != pLineData )
		{
			switch( pLineData->lineType )
			{
			case CKTDGLineMap::LT_BUNGEE:
				{
					m_bWaitingForRebirthAfterBungee = true;
					m_timerRebirthAfterBungee.restart();

					SetAIEnable( false );

				} break;
			}
		}
	}

	if( true == m_bWaitingForRebirthAfterBungee )
	{
		if( m_timerRebirthAfterBungee.elapsed() > MAGIC_REBIRTH_WAIT_TIME )
		{
			const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
			if( NULL != pLineData )
			{
				switch( pLineData->lineType )
				{
				case CKTDGLineMap::LT_BUNGEE:
					{

						SetDamageHistory( false );

						m_bWaitingForRebirthAfterBungee = false;

						//SetNowHP( GetNowHp() - GetMaxHp() * 0.1f );
						// note!! 몬스터는 일단 떨어지면 그냥 죽게

						//{{ mauntain : 김태환 [2012.06.14] 몬스터 카드 소환 기능 - 소환한 몬스터는 절벽으로 떨어져도 죽지 않도록 설정

						if( NCT_MONSTER_CARD == GetNPCCreateType()
							)
						{
							CX2GUUser* pOwnerUnit = static_cast<CX2GUUser*>( GetOwnerGameUnit() );

							if( NULL != pOwnerUnit )		//주인 있으면 데미지 입고, 주인에게 이동
							{
								SetPosition( pOwnerUnit->GetPos() , GetIsRight() );			//User의 위치로 이동

								float fNowHP = GetNowHp() - GetMaxHp() * 0.15f;				//User가 떨어졌을때와 똑같은 처리
								if( fNowHP <= 0.f )
									fNowHP = 1.f;
								SetNowHp( fNowHP );
							}
							else							//주인 없으면 떨어져 사망
							{
								SetNowHp( 0.f );
							}
						}
						else
						{
							SetNowHp( 0.f );
						}
						//}}

						ClearRecieveSyncData();
						StateChange( m_CommonState.m_DownDamageAir, true );

						//InitPosByLineData( pLineData->m_iTeleportLineIndex );
						SetAIEnable( true );
					} break;
				}
			}
		}
	}
}



bool CX2GUNPC::EventTimer( float fTime )
{
	KTDXPROFILE();
	return ( m_NPCFrameData.unitCondition.fStateTimeBack < fTime
		&& m_NPCFrameData.unitCondition.fStateTime >= fTime );
}


bool    CX2GUNPC::StateEventInterval_LUA( int iIntervalID )
{
    if ( iIntervalID >= 0 && iIntervalID < (int) m_EventIntervalList.size() )
    {
        return ( m_EventIntervalList[iIntervalID].first.z != 0.f );
    }
    return  false;
}

bool    CX2GUNPC::GetMotionEnd()
{
    return  NULL != m_pXSkinAnim && m_pXSkinAnim->IsAnimationEnd();
}


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


void    CX2GUNPC::ReceiveMiniSyncData( UidType uidUnitUID, DWORD dwFrameMoveCount, const KDYNAMIC_UNIT_NPC_MINISYNC& kMiniSync )
{
    if ( m_bReceiveFirstSyncPacket == false )
        return;
	if( IsLocalUnit() == true )
		return;
    if ( uidUnitUID == 0 || uidUnitUID != g_pX2Game->GetHostSlotUID() )
        return;
    if ( m_pidHostUID == 0 || m_pidHostUID != uidUnitUID )
        return;
    if ( dwFrameMoveCount <= m_dwLastProcessedFrameMoveCount )
        return;

    bool bPushBack = false;
    if ( m_cbufReceiveDynamicPacket.empty() == true
        || m_cbufReceiveDynamicPacket.back().m_dwFrameMoveCount < dwFrameMoveCount )
    {
        bPushBack = true;
    }
    else
    {
        KCircularBuffer< NPCFrameSync >::iterator iter = std::lower_bound( m_cbufReceiveDynamicPacket.begin(), m_cbufReceiveDynamicPacket.end(), dwFrameMoveCount );
        if ( iter != m_cbufReceiveDynamicPacket.end() && iter->m_dwFrameMoveCount == dwFrameMoveCount )
        {
            iter->m_sync.m_dwRelativePos = kMiniSync.m_dwRelativePos;
            iter->m_sync.m_cTargetUnitIndex = kMiniSync.m_cTargetUnitIndex;
        }
        else if ( iter == m_cbufReceiveDynamicPacket.end() )
        {
            bPushBack = true;
        }
        else
        {
            NPCFrameSync sync;
            sync.m_dwFrameMoveCount = dwFrameMoveCount;
            sync.m_sync.m_dwRelativePos = kMiniSync.m_dwRelativePos;
            sync.m_sync.m_cTargetUnitIndex = kMiniSync.m_cTargetUnitIndex;
            sync.m_bMiniSync = true;
            if ( m_cbufReceiveDynamicPacket.full() == true )
            {
                KCircularBuffer< NPCFrameSync >::difference_type   diff = iter - m_cbufReceiveDynamicPacket.begin();
                m_cbufReceiveDynamicPacket.set_capacity( m_cbufReceiveDynamicPacket.capacity() + 1 );
                iter = m_cbufReceiveDynamicPacket.begin() + diff;
            }
            m_cbufReceiveDynamicPacket.insert( iter, sync );
        }
    }
    if ( bPushBack == true )
    {
        m_cbufReceiveDynamicPacket.resize( m_cbufReceiveDynamicPacket.size() + 1 );
        NPCFrameSync& frameSync = m_cbufReceiveDynamicPacket.back();
        frameSync.m_dwFrameMoveCount = dwFrameMoveCount;
	    frameSync.m_sync.m_dwRelativePos = kMiniSync.m_dwRelativePos;
        frameSync.m_sync.m_cTargetUnitIndex = kMiniSync.m_cTargetUnitIndex;
        frameSync.m_bMiniSync = true;
    }
}


void CX2GUNPC::ReceiveFrameData( UidType uidUnitUID, DWORD dwFrameMoveCount, const KDYNAMIC_UNIT_NPC_SYNC& kXPT_UNIT_NPC_SYNC )
{
	KTDXPROFILE();

	if( IsLocalUnit() == true )
		return;
    if ( uidUnitUID == 0 || uidUnitUID != g_pX2Game->GetHostSlotUID() )
        return;

    if ( m_pidHostUID == 0 || m_pidHostUID != uidUnitUID )
    {
		//Host가 변경된다면 지금까지 받은패킷 모두 정리
		m_cbufReceiveDynamicPacket.resize( 0 );
		m_dwLastProcessedFrameMoveCount = 0;
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		m_kLastStateChangeParts_Recv.Init();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
    }
    m_pidHostUID = uidUnitUID;

    if ( dwFrameMoveCount <= m_dwLastProcessedFrameMoveCount )
    {
        // 필드를 빠져나간 유저가 다시 들어왔을 때 m_dwLastProcessedFrameMoveCount 를 리셋하기 위한 장치
        // 혹은 네트워크 오류로 잘못된 프레임 카운터가 넘어왔을 때를 대비
        if ( dwFrameMoveCount + X2GUNPC_RECEIVE_FRAME_COUNT_MAX_GAB <= m_dwLastProcessedFrameMoveCount )
        {
            m_dwLastProcessedFrameMoveCount = 0;
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			m_kLastStateChangeParts_Recv.Init();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
            m_cbufReceiveDynamicPacket.resize( 0 );
            m_kNonHostReaction.ClearBuffNonHostSimState();

        }
        else
        {
            return;
        }
	}

	m_bReceiveFirstSyncPacket = true;

    bool bPushBack = false;
    if ( m_cbufReceiveDynamicPacket.empty() == true
        || m_cbufReceiveDynamicPacket.back().m_dwFrameMoveCount < dwFrameMoveCount )
    {
        bPushBack = true;
    }
    else
    {
        KCircularBuffer< NPCFrameSync >::iterator iter = std::lower_bound( m_cbufReceiveDynamicPacket.begin(), m_cbufReceiveDynamicPacket.end(), dwFrameMoveCount );
        if ( iter != m_cbufReceiveDynamicPacket.end() && iter->m_dwFrameMoveCount == dwFrameMoveCount )
        {
            iter->m_sync = kXPT_UNIT_NPC_SYNC;
            iter->m_bMiniSync = false;
        }
        else if ( iter == m_cbufReceiveDynamicPacket.end() )
        {
            bPushBack = true;
        }
        else
        {
            NPCFrameSync sync;
            sync.m_dwFrameMoveCount = dwFrameMoveCount;
            sync.m_sync = kXPT_UNIT_NPC_SYNC;
            sync.m_bMiniSync = false;
            if ( m_cbufReceiveDynamicPacket.full() == true )
            {
                KCircularBuffer< NPCFrameSync >::difference_type   diff = iter - m_cbufReceiveDynamicPacket.begin();
                m_cbufReceiveDynamicPacket.set_capacity( m_cbufReceiveDynamicPacket.capacity() + 1 );
                iter = m_cbufReceiveDynamicPacket.begin() + diff;
            }
            m_cbufReceiveDynamicPacket.insert( iter, sync );
        }
    }
    if ( bPushBack == true )
    {
        m_cbufReceiveDynamicPacket.resize( m_cbufReceiveDynamicPacket.size() + 1 );
        NPCFrameSync& frameSync = m_cbufReceiveDynamicPacket.back();
        frameSync.m_dwFrameMoveCount = dwFrameMoveCount;
	    frameSync.m_sync = kXPT_UNIT_NPC_SYNC;
        frameSync.m_bMiniSync = false;
    }
}

bool    CX2GUNPC::SendPacket( CX2FrameUDPPack& kFrameUDPPack, bool bImmediateOnly )
{
	KTDXPROFILE();

	if( IsLocalUnit() == false )
	{
		return false;
	}

    if ( GetNowStateID() == GUSI_DIE )
        return false;

    if ( bImmediateOnly == true )
    {
	    if( m_iSendReserveCount <= 0 || m_bImmediate == false )
		    return false;
    }
    else
    {
	    if( m_iSendReserveCount <= 0 )
		    return false;
    }

	if( m_dwSendPacketFrame != kFrameUDPPack.GetFrameMoveCount() )
	{
        if ( m_iSendReserveCount > 0 )
		    m_iSendReserveCount--;
        m_bImmediate = false;
		m_dwSendPacketFrame = kFrameUDPPack.GetFrameMoveCount();
		bool bRet = SendPacketImmediateForce( kFrameUDPPack );
        return bRet;
	}
    return false;
}



bool    CX2GUNPC::SendMiniSyncPacket( CX2FrameUDPPack& kFrameUDPPack )
{
	if( IsLocalUnit() == false )
	{
		return false;
	}

    int  iTargetUnitIndex = -1;
    if ( GetInitStateData().m_bSyncTargetInfo == true )
    {
        CX2GUUser* pX2User = GetTargetUser();
        if ( pX2User != NULL )
        {
            iTargetUnitIndex = pX2User->GetUnitIndex();
        }//if
    }//if

    if ( m_kNonHostReaction.UpdateLastSentMiniInfo( iTargetUnitIndex ) == 0 )
        return false;

	KDYNAMIC_UNIT_NPC_MINISYNC kMiniSync;
	kMiniSync.m_iUnitUID				= (int)m_UnitUID;
    kMiniSync.m_dwRelativePos = m_kNonHostReaction.GetRelativePosFlags();
    kMiniSync.m_cTargetUnitIndex = iTargetUnitIndex;


    BYTE    byBuffer[sizeof(KDYNAMIC_UNIT_NPC_MINISYNC)];
    BYTE*    pbStart = &byBuffer[0];
    kMiniSync.Serialize( pbStart );
    kFrameUDPPack.AddFrameUDPPack_IDPack( XPT_UNIT_NPC_MINISYNC_PACK, &byBuffer[0], pbStart - &byBuffer[0] );
	return true;

}


bool CX2GUNPC::SendPacketImmediateForce( CX2FrameUDPPack& kFrameUDPPack )
{
	KTDXPROFILE();

	if( IsLocalUnit() == false )
	{
		return false;
	}

	//
	KDYNAMIC_UNIT_NPC_SYNC kXPT_UNIT_NPC_SYNC;

	kXPT_UNIT_NPC_SYNC.m_iUnitUID				= (int)m_UnitUID;


    kXPT_UNIT_NPC_SYNC.m_usNowSpeedX = floatToHalf( m_PhysicParam.nowSpeed.x );
    kXPT_UNIT_NPC_SYNC.m_usNowSpeedY = floatToHalf( m_PhysicParam.nowSpeed.y );
    kXPT_UNIT_NPC_SYNC.m_fNowHP = GetNowHp();
    kXPT_UNIT_NPC_SYNC.m_usNowMP = floatToHalf( GetNowMp() );
	kXPT_UNIT_NPC_SYNC.m_cNowState			= m_NPCFrameData.syncData.nowState;
    _EncodeNpcSyncPos( kXPT_UNIT_NPC_SYNC.m_usPosX, kXPT_UNIT_NPC_SYNC.m_usPosY, kXPT_UNIT_NPC_SYNC.m_usPosZ, m_NPCFrameData.syncData.position, m_NPCFrameData.syncData.lastTouchLineIndex );
	kXPT_UNIT_NPC_SYNC.m_ucLastTouchLineIndex			= m_NPCFrameData.syncData.lastTouchLineIndex;

    m_NPCFrameData.syncData.mindFlag = 0;
    kXPT_UNIT_NPC_SYNC.SetMindFlag( m_NPCFrameData.syncData.mindFlag );
    kXPT_UNIT_NPC_SYNC.SetIsRight( m_NPCFrameData.syncData.bIsRight );
    kXPT_UNIT_NPC_SYNC.SetIsComeBackState( m_NPCFrameData.syncData.bComeBackState );
    kXPT_UNIT_NPC_SYNC.m_usDefenceRandomTableIndex =  m_kNonHostReaction.GetDefenceRandomTableIndex();
    kXPT_UNIT_NPC_SYNC.SetReactionState(
        m_kNonHostReaction.IsNowStateReaction_NONE() == false,
        m_kNonHostReaction.GetNowStateReactionUnitIndex(),
        m_kNonHostReaction.GetNowStateRelaxReactionCheck() );
    kXPT_UNIT_NPC_SYNC.SetFootOnLine( m_NPCFrameData.unitCondition.bFootOnLine );
    int  iTargetUnitIndex = -1;
    if ( GetInitStateData().m_bSyncTargetInfo == true )
    {
        CX2GUUser* pX2User = GetTargetUser();
        if ( pX2User != NULL )
        {
            iTargetUnitIndex = pX2User->GetUnitIndex();
        }//if
    }//if
    kXPT_UNIT_NPC_SYNC.SetTargetUnit( iTargetUnitIndex );
    kXPT_UNIT_NPC_SYNC.m_dwRelativePos = m_kNonHostReaction.GetRelativePosFlags();

    m_kNonHostReaction.UpdateLastSentMiniInfo( iTargetUnitIndex );


    kXPT_UNIT_NPC_SYNC.m_cNowState = m_NPCFrameData.syncData.nowState;
    m_NPCFrameData.syncData.stateChangeParts.stateModifyNum = m_NPCFrameData.unitCondition.stateChangeParts.stateModifyNum;
    kXPT_UNIT_NPC_SYNC.m_stateChangeParts = m_NPCFrameData.syncData.stateChangeParts;

    kXPT_UNIT_NPC_SYNC.m_usRandomTableIndex = m_NPCFrameData.syncData.m_usRandomTableIndex;


	m_PacketSendCount++;

    BYTE    byBuffer[sizeof(KDYNAMIC_UNIT_NPC_SYNC)];
    BYTE*    pbStart = &byBuffer[0];
    kXPT_UNIT_NPC_SYNC.Serialize( pbStart );
    kFrameUDPPack.AddFrameUDPPack_IDPack( XPT_UNIT_NPC_SYNC_PACK, &byBuffer[0], pbStart - &byBuffer[0] );

	return true;
}
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//void CX2GUNPC::ReceiveFrameData( KXPT_UNIT_NPC_SYNC* pXPT_UNIT_NPC_SYNC )
//{
//	KTDXPROFILE();
//	if( IsLocalUnit() == true )
//		return;
//
//
//	m_PacketRecvCount++;
//
//	//m_ReceivePacketBufList.clear();
//	m_ReceivePacketBufList.push_back( *pXPT_UNIT_NPC_SYNC );
//}
//
//void CX2GUNPC::SendPacket( vector<KXPT_UNIT_NPC_SYNC>& syncList )
//{
//	KTDXPROFILE();
//
//
//
//
//#ifdef TRANSFORMER_TEST
//	if( false == IsLocalUnit() )
//	{
//		return;
//	}
//#else TRANSFORMER_TEST
//	//{{AFX
//	if( g_pX2Game->IsHost() == false )
//	{
//		return;
//	}
//	//}}AFX
//#endif TRANSFORMER_TEST
//
//
//
//	if( m_bSendReserve == false )
//		return;
//
//	SendPacketImmediateForce( syncList );
//
//	m_bSendReserve	= false;
//}
//
//void CX2GUNPC::SendPacketImmediate( vector<KXPT_UNIT_NPC_SYNC>& syncList )
//{
//	KTDXPROFILE();
//
//
//
//#ifdef TRANSFORMER_TEST
//	if( false == IsLocalUnit() )
//	{
//		return;
//	}
//#else TRANSFORMER_TEST
//	//{{AFX
//	if( g_pX2Game->IsHost() == false )
//	{
//		return;
//	}
//	//}}AFX
//#endif TRANSFORMER_TEST
//
//
//
//
//	if( m_bSendReserve == false || m_bImmediate == false )
//		return;
//
//	SendPacketImmediateForce( syncList );
//
//	m_bSendReserve	= false;
//	m_bImmediate	= false;
//}
//
//void CX2GUNPC::SendPacketImmediateForce( vector<KXPT_UNIT_NPC_SYNC>& syncList )
//{
//	KTDXPROFILE();
//
//
//
//#ifdef TRANSFORMER_TEST
//	if( false == IsLocalUnit() )
//	{
//		return;
//	}
//#else TRANSFORMER_TEST
//	//{{AFX
//	if( g_pX2Game->IsHost() == false )
//	{
//		return;
//	}
//	//}}AFX
//#endif TRANSFORMER_TEST
//
//
//
//	KXPT_UNIT_NPC_SYNC kXPT_UNIT_NPC_SYNC;
//
//	kXPT_UNIT_NPC_SYNC.unitUID				= (int)m_UnitUID;
//	kXPT_UNIT_NPC_SYNC.nextState			= m_NPCFrameData.unitCondition.nextState;
//	kXPT_UNIT_NPC_SYNC.nowSpeedX			= m_PhysicParam.nowSpeed.x;
//	kXPT_UNIT_NPC_SYNC.nowSpeedY			= m_PhysicParam.nowSpeed.y;
//	kXPT_UNIT_NPC_SYNC.fNowHP				= GetNowHp();
//	kXPT_UNIT_NPC_SYNC.fNowMP				= floatToHalf( GetNowMp() );
//	kXPT_UNIT_NPC_SYNC.nowState				= m_NPCFrameData.syncData.nowState;
//	kXPT_UNIT_NPC_SYNC.posX					= m_NPCFrameData.syncData.position.x;
//	kXPT_UNIT_NPC_SYNC.posY					= m_NPCFrameData.syncData.position.y;
//	kXPT_UNIT_NPC_SYNC.posZ					= m_NPCFrameData.syncData.position.z;
//	kXPT_UNIT_NPC_SYNC.lastTouchLineIndex	= m_NPCFrameData.syncData.lastTouchLineIndex;
//	//kXPT_UNIT_NPC_SYNC.bIsRight				= m_NPCFrameData.syncData.bIsRight;
//	//kXPT_UNIT_NPC_SYNC.bStateChange			= m_bSendReserveStateChange;
//	//kXPT_UNIT_NPC_SYNC.mindFlag				= m_NPCFrameData.syncData.mindFlag;
//
//	kXPT_UNIT_NPC_SYNC.mindFlag	= m_NPCFrameData.syncData.mindFlag;
//	char tempData = m_NPCFrameData.syncData.bIsRight;
//	kXPT_UNIT_NPC_SYNC.mindFlag	|= (tempData << 7);
//	tempData = m_bSendReserveStateChange;
//	kXPT_UNIT_NPC_SYNC.mindFlag	|= (tempData << 6);
//	tempData = m_NPCFrameData.syncData.bComeBackState;
//	kXPT_UNIT_NPC_SYNC.mindFlag	|= (tempData << 5);
//
//
//
//#ifdef NEW_RANDOM_TABLE_TEST
//
//	if( m_bSendReserveStateChange == true )
//	{
//		kXPT_UNIT_NPC_SYNC.m_usRandomTableIndex	= (USHORT) ( rand() % CKTDXRandomNumbers::RANDOM_NUMBER_COUNT );
//		m_NPCFrameData.syncData.m_usRandomTableIndex = kXPT_UNIT_NPC_SYNC.m_usRandomTableIndex;
//	}
//	else
//	{
//		kXPT_UNIT_NPC_SYNC.m_usRandomTableIndex	= m_NPCFrameData.syncData.m_usRandomTableIndex;
//	}
//#else NEW_RANDOM_TABLE_TEST
////{{AFX
//
//	if( m_bSendReserveStateChange == true )
//	{
//		kXPT_UNIT_NPC_SYNC.m_StateRandomSeed	= rand()%100;
//		kXPT_UNIT_NPC_SYNC.m_StateRandomSeed2	= rand()%100;
//		// 2008.11.13 김태완 : RandomSeed2번 전송 안 되는 문제 수정 // 코드 위치 변경
//		m_NPCFrameData.syncData.m_StateRandomSeed = kXPT_UNIT_NPC_SYNC.m_StateRandomSeed;
//		m_NPCFrameData.syncData.m_StateRandomSeed2 = kXPT_UNIT_NPC_SYNC.m_StateRandomSeed2;
//	}
//	else
//	{
//		kXPT_UNIT_NPC_SYNC.m_StateRandomSeed	= m_NPCFrameData.syncData.m_StateRandomSeed;
//		kXPT_UNIT_NPC_SYNC.m_StateRandomSeed2	= m_NPCFrameData.syncData.m_StateRandomSeed2;
//	}
////}}AFX
//#endif NEW_RANDOM_TABLE_TEST
//
//	m_PacketSendCount++;
//
//	syncList.push_back( kXPT_UNIT_NPC_SYNC );
//}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
void CX2GUNPC::SendReserve( int iMaxCount )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//void CX2GUNPC::SendReserve()
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
{
	KTDXPROFILE();

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( iMaxCount <= 0 )
        return;

	m_iSendReserveCount = __max( m_iSendReserveCount, iMaxCount );
//#else//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	m_bSendReserve = true;
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if( m_NPCFrameData.syncData.bStateChange == true )
//		m_bSendReserveStateChange = true;
//	else
//		m_bSendReserveStateChange = false;
//#endif   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

void CX2GUNPC::AnimStop()
{
	if ( m_pXSkinAnim != NULL )
		m_pXSkinAnim->Wait();
}


float CX2GUNPC::GetCriticalPercent( BYTE byAttackType, float fExtraCritical )
{


	float fCriticalPercent = GetCriticalRate();

// 	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;
// 	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
// 	{
// 		pNPCExtraAbility = g_pData->GetUnitManager()->GetNPCExtraAbility( m_vecExtraAbilityID[i] );
//
// 		if ( pNPCExtraAbility != NULL )
// 		{
// 			fCriticalPercent += pNPCExtraAbility->m_fCriticalPercent;
// 		}
// 	}
//
	fCriticalPercent += fExtraCritical;

	return fCriticalPercent;
}

float CX2GUNPC::GetActionSpeedUpValue()
{
	float fActionSpeedUpValue = 0.0f;

	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;

	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
	{
        pNPCExtraAbility = m_vecExtraAbilityID[ i ];

		if ( pNPCExtraAbility != NULL )
		{
			fActionSpeedUpValue += pNPCExtraAbility->m_fAllSpeedUpPercentUp;
		}
	}

	return fActionSpeedUpValue;
}

bool CX2GUNPC::GetIsAlwaysSuperArmor()
{
	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;

	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
	{
        pNPCExtraAbility = m_vecExtraAbilityID[i];

		if ( pNPCExtraAbility != NULL )
		{
			if ( pNPCExtraAbility->m_bAlwaysSuperArmor == true )
				return true;
		}
	}

	return false;
}

bool CX2GUNPC::GetIsHeavyMonster()
{
	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;

	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
	{
        pNPCExtraAbility = m_vecExtraAbilityID[i];

		if ( pNPCExtraAbility != NULL )
		{
			if ( pNPCExtraAbility->m_bHeavy == true )
				return true;
		}
	}

	return false;
}

wstring CX2GUNPC::GetExtraAbilityDesc( bool bFront )
{
	const CX2UnitManager::NPCExtraAbility* pNPCExtraAbility = NULL;

	for ( int i = 0; i < (int)m_vecExtraAbilityID.size(); i++ )
	{
        pNPCExtraAbility = m_vecExtraAbilityID[i];

		if ( pNPCExtraAbility != NULL )
		{
			if ( pNPCExtraAbility->m_bFront == bFront )
			{
				return pNPCExtraAbility->m_AbilityName;
			}
		}
	}

	return L"";
}

bool CX2GUNPC::GetIsEliteMonster()
{
	if ( (int)m_vecExtraAbilityID.size() >= 1 || (int)m_vecEnchantType.size() >= 1  )
	{
		return true;
	}
	return false;
}


float CX2GUNPC::GetEnchantResist( CX2EnchantItem::ENCHANT_TYPE enchantType )
{
	float fResistValue = 0.0f;

	for ( int i = 0; i < (int)m_vecEnchantType.size(); i++ )
	{
		if ( m_vecEnchantType[i] == enchantType )
		{
			fResistValue += CX2EnchantItem::EAR_VALUE_FOR_NPC;
		}
	}

	//const CX2UnitManager::NPCUnitStat* pNpcStat = g_pData->GetUnitManager()->GetNPCUnitStat( GetNPCTemplet().m_nNPCUnitID );
    const CX2UnitManager::NPCUnitStat& kNpcStat = GetNPCUnitStat();
	//if ( pNpcStat != NULL )
	{

		switch(enchantType)
		{
		case CX2EnchantItem::ET_BLAZE:
			{
				fResistValue += kNpcStat.m_DefenseRed;
			} break;
		case CX2EnchantItem::ET_WATER:
			{
				fResistValue += kNpcStat.m_DefenseBlue;
			} break;
		case CX2EnchantItem::ET_NATURE:
			{
				fResistValue += kNpcStat.m_DefenseGreen;
			} break;
		case CX2EnchantItem::ET_WIND:
			{
				fResistValue += kNpcStat.m_DefenseWind;
			} break;
		case CX2EnchantItem::ET_LIGHT:
			{
				fResistValue += kNpcStat.m_DefenseLight;
			} break;
		case CX2EnchantItem::ET_DARK:
			{
				fResistValue += kNpcStat.m_DefenseDark;
			} break;
		default:
			break;
		}

	}

	return fResistValue;
}

void CX2GUNPC::ClearRecieveSyncData()
{
	m_NPCFrameData.syncData.lastTouchLineIndex = 0;
}

void CX2GUNPC::SetAddPos( const D3DXVECTOR3& pos )
{
	m_AddPos = pos;
}


void CX2GUNPC::SetAIEnable( bool bEnable )
{
	m_bAIEnabled = bEnable;
	if( m_pAI != NULL )
	{
		m_pAI->SetEnabled( bEnable );
		m_pAI->ResetTarget();
	}

	if( false == m_bAIEnabled )
	{
		m_AIMessage = SCT_AI_WAIT;
		m_UnUseAIMessage = SCT_AI_WAIT;
	}
	else
	{
		m_AIMessage = SCT_AI_WAIT;
		m_pAI->SetPrevAIMessage( SCT_NO_CHANGE );
	}
}

bool CX2GUNPC::GetAIEnabled()
{
	if( NULL == m_pAI )
		return false;


	if( m_fTimePauseNPCAI > 0.f )
	{
		return false;
	}

	return m_bAIEnabled;
}

void CX2GUNPC::SetTimePauseNPCAI( const float val )
{
	m_fTimePauseNPCAI = val;

	if( val > 0.f )
	{
		if( m_pAI != NULL )
		{
			m_pAI->ResetTarget();
		}
		m_AIMessage = SCT_AI_WAIT;
		m_UnUseAIMessage = SCT_AI_WAIT;
	}

}



void CX2GUNPC::SetRallyToUnit( CX2GameUnit* pRallyGameUnit_ )
{
	if( NULL != m_pAI && true == GetAIEnabled() )
	{
		if( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
		{
			CX2GUNPCAI* pNormalAI = static_cast<CX2GUNPCAI*>( m_pAI );
			pNormalAI->SetRallyToUnit( pRallyGameUnit_ );
		}
	}
}


void CX2GUNPC::CallOtherNPC( CX2UnitManager::NPC_UNIT_ID npcUnitID )
{
	if( NULL == g_pX2Game )
		return;

	for( int i=0; i<g_pX2Game->GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnit( i );
		if( NULL == pGUNPC || pGUNPC == this ||
			//false == pGUNPC->IsActiveMonster() ||
			CX2GameUnit::GUSI_DIE == pGUNPC->GetGameUnitState() ||
			npcUnitID != pGUNPC->GetNPCTemplet().m_nNPCUnitID )
			continue;

		if( NULL == pGUNPC->GetNPCAI() ||
			pGUNPC->GetNPCAI()->GetAIType() != CX2NPCAI::NAT_NORMAL )
			continue;

		if( false == pGUNPC->GetNPCAI()->IsTargetAccquired() )
		{
			pGUNPC->SetRallyToUnit( this );
		}
	}
}

void CX2GUNPC::CallNearOneNPC( CX2UnitManager::NPC_UNIT_ID npcUnitID, float fMaxRange )
{
	if( NULL == g_pX2Game )
		return;

// 	if(GetNowStateID() != GetWaitStateID())
// 		return;

	CX2GUNPC* pGUNPC = g_pX2Game->GetNearestNpcInSpecificRangeByNpcId( GetPos(), npcUnitID, fMaxRange);

	if( NULL == pGUNPC || pGUNPC == this ||
		//false == pGUNPC->IsActiveMonster() ||
		CX2GameUnit::GUSI_DIE == pGUNPC->GetGameUnitState() ||
		GetNowHp() <= 0.f ||
		npcUnitID != pGUNPC->GetNPCTemplet().m_nNPCUnitID )
		return;

	if( NULL == pGUNPC->GetNPCAI() ||
		pGUNPC->GetNPCAI()->GetAIType() != CX2NPCAI::NAT_NORMAL )
		return;

	if(pGUNPC->GetRideState() != RS_READY)
		return;

	CX2GUNPCAI* pNormalAI = static_cast<CX2GUNPCAI*>( m_pAI );

	pNormalAI->SetRideToUnit( pGUNPC );

	//m_RideTargetNPCUid = NPCUid;

	SetRideState( CX2GUNPC::RS_TRY_TO_RIDE);
	pGUNPC->SetRideState( CX2GUNPC::RS_TRY_TO_RIDE);

	SetRideUnit(pGUNPC);
	pGUNPC->SetRideUnit(this);

	pGUNPC->SetAIEnable(false);
}

bool CX2GUNPC::TryToRideTargetNPC( )
{
	if( NULL == g_pX2Game )
		return false;

	//CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnitByUID(m_RideTargetNPCUid);

	CX2GUNPC* pGUNPC = GetRideUnit();

	if( NULL == pGUNPC || pGUNPC == this ||
		//false == pGUNPC->IsActiveMonster() ||
		(  pGUNPC != NULL && pGUNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE ) )
	{
		SetRideState( CX2GUNPC::RS_READY);
		StopRideToUnit();
		return false;
	}

	if(GetDistance(GetPos(), pGUNPC->GetPos()) < 180.f &&
		pGUNPC->GetNowStateID() == pGUNPC->GetWaitStateID()	)
	{
		StopRideToUnit();

		SetRideState( CX2GUNPC::RS_ON_RIDING);
		pGUNPC->SetRideState( CX2GUNPC::RS_ON_RIDING);
		//pGUNPC->SetAIEnable(true);

		StateChange(GetRideStateID());
		pGUNPC->StateChange(pGUNPC->GetRideStateID());

		//SetRideUnit(pGUNPC);
		//pGUNPC->SetRideUnit(this);


		pGUNPC->SetAIEnable(false);

		return true;
	}

	return false;

}
CX2GUNPC* CX2GUNPC::GetRideUnit()
{
	if( null != m_optrRidingGameUnit )
	{
		return static_cast<CX2GUNPC*>( m_optrRidingGameUnit.GetObservable() );
	}
	return NULL;
}
void CX2GUNPC::StopRideToUnit()
{
	CX2GUNPCAI* pNormalAI = (CX2GUNPCAI*) m_pAI;
	if(pNormalAI != NULL)
	{
		pNormalAI->StopRideToUnit();
	}
	return;
}

void CX2GUNPC::RideOff()
{
	CX2GUNPC* pGUNPC = GetRideUnit();

	if(GetRideType() == CX2GUNPC::RT_RIDER)
	{
		if(GetNowHp() > 0.f && GetRideState() ==  CX2GUNPC::RS_ON_RIDING)
		{
			StateChange(GetAirFallState());
		}
		SetRideState( CX2GUNPC::RS_READY);
		ResetRideUnit();
		StopRideToUnit();

		if( NULL != pGUNPC)
		{
			pGUNPC->SetAIEnable(true);
			pGUNPC->SetRideState( CX2GUNPC::RS_READY);
			pGUNPC->ResetRideUnit();
		}
	}
	else
	{
		SetRideState( CX2GUNPC::RS_READY);
		ResetRideUnit();

		if( NULL != pGUNPC)
		{
			if(pGUNPC->GetNowHp() > 0.f&& pGUNPC->GetRideState() ==  CX2GUNPC::RS_ON_RIDING)
			{
				pGUNPC->StateChange(pGUNPC->GetAirFallState());
			}
			pGUNPC->SetRideState( CX2GUNPC::RS_READY);
			pGUNPC->ResetRideUnit();
			pGUNPC->StopRideToUnit();
		}
		SetAIEnable(true);

	}

	return;
}

bool CX2GUNPC::IsMoveableState()
{
	bool bIsMovableState = false;
	if(GetNowStateID() == m_SmallDamageLandFront ||
		GetNowStateID() == m_SmallDamageLandBack ||
		GetNowStateID() == m_SmallDamageAir ||
		GetNowStateID() == m_SmallDamageAirFront ||
		GetNowStateID() == m_SmallDamageAirBack ||
		GetNowStateID() == m_BigDamageLandFront ||
		GetNowStateID() == m_BigDamageLandBack ||
		GetNowStateID() == m_BigDamageAir ||
		GetNowStateID() == m_BigDamageAirFront ||
		GetNowStateID() == m_BigDamageAirBack ||
		GetNowStateID() == m_UpDamage ||
		GetNowStateID() == m_FlyDamageFront ||
		GetNowStateID() == m_FlyDamageBack ||
		GetNowStateID() == m_CommonState.m_DownDamageLandFront ||
		GetNowStateID() == m_CommonState.m_DownDamageLandBack ||
		GetNowStateID() == m_CommonState.m_DownDamageAir ||
		GetNowStateID() == m_CommonState.m_DamageAirDownLanding)
	{
		bIsMovableState = false;
	}
	else
	{
		if(IsOnSomething() == true)
			bIsMovableState = true;
		else
			bIsMovableState = false;

	}
	return bIsMovableState;
}


void CX2GUNPC::JumpDownFromLinemap()
{
	m_NPCFrameData.syncData.position.y -= LINE_RADIUS * 1.5f;
}
void CX2GUNPC::SetRideToUnit( CX2GameUnit* pGameUnit_ )
{
	if( NULL != m_pAI && true == GetAIEnabled() )
	{
		if( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
		{
			CX2GUNPCAI* pNormalAI = static_cast<CX2GUNPCAI*>( m_pAI );
			pNormalAI->SetRideToUnit( pGameUnit_ );
		}
	}
}







void CX2GUNPC::RenderName()
{
    if ( false == GetShowObject() )
		return;

	if( true == GetAbsoluteInvisibility() )
		return;

	if( m_bShowName == false )
		return;

	if ( false == GetShowSmallGageAndName() )
		return;


    if( m_SpeedDyingFly > 0 && m_bDyingStart == true && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC && m_bDieFly == true )
        return;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		return;

	if( g_pX2Game->GetMyUnit() != NULL &&
		g_pX2Game->GetMyUnit()->GetWhiteOut() == true )
		return;

	if ( true == GetNoRenderName() )	/// 몬스터 이름 미출력 설정하였다면, 출력 제외
		return;

	D3DXVECTOR3 vPosName( GetPos() );
	vPosName.y -= 80;

	if ( m_wstrChampionName.empty() == false )
	{
		// 챔피언 타입을 출력하고
		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, m_wstrChampionName.c_str(),
			m_colorName, CKTDGFontManager::FS_SHELL, m_colorOutLine, NULL, DT_CENTER  );
		vPosName.y -= 20;	// 실제 이름 출력 하는 곳의 위치를 -20 만큼 더 해줌
	}

	if( IsPvpBot() == true )
	{
		if( GetTeam() == CX2Room::TN_BLUE )
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), m_wstrLvAndNameAndPostFix.c_str(),
				m_colorName, CKTDGFontManager::FS_SHELL, m_colorOutLine, NULL, DT_CENTER  );
		}
		else
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( GetPos(), m_wstrLvAndNameAndPostFix.c_str(),
				m_colorName,	CKTDGFontManager::FS_SHELL, m_colorOutLine , NULL, DT_CENTER  );
		}

	}
	else
	{

		if( NCT_MONSTER_CARD == GetNPCCreateType() )
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, m_wstrLvAndNameAndPostFix.c_str(),
				0xff0db46d, CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
		}
		else if( true == m_bChangeRenderNameColor)		/// 이름 색상을 변경 하였다면, 설정 색상 적용
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, m_wstrLvAndNameAndPostFix.c_str(),
				m_colorRenderName, CKTDGFontManager::FS_SHELL, m_colorRenderNameOutLine, NULL, DT_CENTER  );
		}
		else
		{
			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, m_wstrLvAndNameAndPostFix.c_str(),
				m_colorName/*0xffffaa77*/, CKTDGFontManager::FS_SHELL, m_colorOutLine, NULL, DT_CENTER  );
		}
	}

#if X2_DEV_TOOLS
	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
// 		WCHAR* wDest = new WCHAR[1000];
//
// 		ZeroMemory( wDest, 1000 );
// 		m_pAI->RenderStateParam( &wDest );
// 		vPosName.y -= 20.0f;
// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, wDest,
// 			0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );

		// 		ZeroMemory( wDest, 1000 );
		// 		swprintf( wDest, L"LastTouchedIndex: %d", GetLastTouchLineIndex() );
		// 		vPosName.y -= 20.0f;
		// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, wDest,
		// 			0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );

		// 		ZeroMemory( wDest, 1000 );
		// 		m_pAI->RenderPatrolLineIndex( &wDest );
		// 		vPosName.y -= 20.0f;
		// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, wDest,
		// 			0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
		//
		// 		ZeroMemory( wDest, 1000 );
		// 		m_pAI->RenderPlayLineIndex( &wDest );
		// 		vPosName.y -= 20.0f;
		// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, wDest,
		// 			0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );

		//delete wDest;
		//vPosName = GetPos() + D3DXVECTOR3( 0, -100, 0 );

		//*
		D3DCOLOR colorFont = 0xffffaa77;
		D3DCOLOR colorOutLine = 0xffff0000;
		WCHAR adBuf[200] = {0,};

// #ifdef	BATTLE_FIELD_TEST

// 		if ( g_pX2Game->IsHost() && false == GetReceiveP2P() )
// 		{
// 			colorFont = 0xff77aaff;
// 			colorOutLine = 0xff00ff00;
// 			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, L"NoCreatedBySyncNot",
// 				colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
// 		}
//
// 		else if ( GetCreatedBySyncNot() )
// 		{
// 			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, L"CreatedBySyncNot",
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
// 		}
// 		else
// 		{
// 			colorFont = 0xff77aaff;
// 			colorOutLine = 0xff0000ff;
// 			g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, L"NoCreatedBySyncNot",
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
// 		}
// #endif	BATTLE_FIELD_TEST

// 		swprintf( adBuf, L"SP: %d", m_PacketSendCount );
// 		vPosName.y -= 20.0f;
// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
//
// 		swprintf( adBuf, L"RP: %d", m_PacketRecvCount );
// 		vPosName.y -= 20.0f;
// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
//
// 		swprintf( adBuf, L"Lv.%d, UID: %d", m_HardLevel, GetUID() );
// 		vPosName.y -= 20.0f;
// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
		//
		const float fForceDownValue = GetGageData()->GetNowForceDown();
		StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"ForceDown: %d", (int)fForceDownValue );
		vPosName.y -= 20.0f;
		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );

		//LV표시
		StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"Level: %d", m_HardLevel );
		vPosName.y -= 25.0f;
		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );

		//{{ 허상형 : [2009/8/4] //	HP 표시
		StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"HP: %d/%d", (int)GetNowHp(), (int)GetMaxHp() );
		vPosName.y -= 25.0f;
		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );

// 		//}} 허상형 : [2009/8/4] //	HP 표시
//
// 		StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"AniSpeed:%f", m_PhysicParam.GetAnimSpeed() );
// 		vPosName.y -= 20.0f;
// 		g_pX2Game->GetFontForUnitName()->OutProjectionText( vPosName, adBuf,
// 			colorFont, CKTDGFontManager::FS_SHELL, colorOutLine, NULL, DT_CENTER  );
		//*/
	}
#endif // _SERVICE_
}

void CX2GUNPC::RenderCrazyMode()
{
	if ( m_bGgobong == false )
		return;

//{{ robobeg : 2008-10-21
	//if( false == GetShow() )
    if ( false == GetShowObject() )
//}} robobeg : 2008-10-21
		return;

	D3DXVECTOR3 vPos = GetPos() + D3DXVECTOR3( 0, -80 + 20, 0 );
	g_pX2Game->GetFontForUnitName()->OutProjectionText( vPos, GET_STRING( STR_ID_227 ),
		0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
}

bool CX2GUNPC::HasTarget()
{
	if( NULL == m_pAI )
		return false;

	return m_pAI->IsTargetAccquired();
}

void CX2GUNPC::ShowGateGuideFrameMove( double fTime, float fElapsedTime )
{
	if( this->GetGameUnitState() == CX2GameUnit::GUSI_DIE || this->GetNowHp() <= 0.f )
		return;

	if( CX2UnitManager::NCT_THING_GATE == GetNPCTemplet().m_ClassType && NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit())
	{
		CX2GUUser* pGUUser = g_pX2Game->GetMyUnit();
		if( NULL != pGUUser && pGUUser->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			if( g_pX2Game->LiveActiveNPCNum() == 0  && NULL != g_pData && NULL != g_pData->GetPlayGuide())
			{	//스테이지의 살아남은 액티브 몬스터가 없을 때, 스테이지 이동 가이드 출력
				g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_GO_NEXT_STAGE, true);
			}
		}
	}
}


void CX2GUNPC::SetLockOnTarget( CX2DamageEffect::CEffect* pCEffect )
{
	if ( pCEffect != NULL )
	{
		bool bUserUnit;
		UidType uid = g_pX2Game->GetLockOnTarget( (CX2Room::TEAM_NUM)GetTeam(), (int)GetRandomInt(), bUserUnit );

		if( bUserUnit == true )
			pCEffect->SetLockOnUnitUID( uid );
		else
			pCEffect->SetLockOnNPCUID( (int)uid );

		pCEffect->SetLockOnRandomSeed( (int)GetRandomInt() );

	}
}

void CX2GUNPC::SetLockOnNearTarget( CX2DamageEffect::CEffect* pCEffect )
{
	if( pCEffect != NULL )
	{
		bool bUserUnit;
		UidType uid = g_pX2Game->GetLockOnNearstTarget( (CX2Room::TEAM_NUM)GetTeam(), GetPos(), bUserUnit );

		if( bUserUnit == true )
			pCEffect->SetLockOnUnitUID( uid );
		else
			pCEffect->SetLockOnNPCUID( (int)uid );

		pCEffect->SetLockOnRandomSeed( (int)GetRandomInt() );
	}
}

//2013-08-29패치 : <김창한> uid를 지정해서 유닛을 타겟하도록 하는 함수. 하멜 비던에서 사용됨.
void CX2GUNPC::SetLockOnUnitUIDFix( CX2DamageEffect::CEffect* pCEffect, UidType uUID )
{
	if( pCEffect != NULL && uUID >= 0 )
	{
		pCEffect->SetLockOnUnitUID( uUID );
		pCEffect->SetLockOnRandomSeed( GetRandomInt() );
	}
}


void CX2GUNPC::ChangeHyperMode_LUA( )
{
	if ( GetHyperModeCount() <= 0 )
		return;

	if ( GetRemainHyperModeTime() > 0.0f )
		return;

	UpDownCrashCamera( 30.0f, 0.4f );
	g_pKTDXApp->GetDGManager()->ClearScreen();

	ApplyHyperModeBuff();
}




D3DXVECTOR3 CX2GUNPC::GetBoneRotateDegree_LUA( char* pBoneName )
{
	if( m_pXSkinAnim != NULL )
	{

		wstring wBoneName;
		ConvertUtf8ToWCHAR( wBoneName, pBoneName );
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( wBoneName.c_str() );
		if( NULL != pFrame )
		{
			D3DXVECTOR3 vRotateDegree;
			GetFrameRotateDegree( &vRotateDegree, pFrame );
			return vRotateDegree;
		}
	}

	return D3DXVECTOR3(0,0,0);
}

void CX2GUNPC::SetBoneScale_LUA(char* pBoneName, bool bScaleChild, float sx, float sy, float sz)
{
	if( m_pXSkinAnim != NULL )
	{
		wstring wBoneName;
		ConvertUtf8ToWCHAR( wBoneName, pBoneName );
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pXSkinAnim->GetCloneFrame( wBoneName.c_str() );
		if( NULL != pFrame )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(sx, sy, sz);

			SetFrameScale( &vScale, bScaleChild, pFrame );
		}
	}
}

void CX2GUNPC::SetLockOn( CX2DamageEffect::CEffect* pCEffect, int randomOffset )
{
	if( pCEffect != NULL )
	{
		bool bUserUnit;
		UidType uid = g_pX2Game->GetLockOnTarget( (CX2Room::TEAM_NUM)GetTeam(),
												(int)GetRandomInt() + randomOffset, bUserUnit );
		if( bUserUnit == true )
			pCEffect->SetLockOnUnitUID( uid );
		else
			pCEffect->SetLockOnNPCUID( (int)uid );

		pCEffect->SetLockOnRandomSeed( (int)GetRandomInt() + randomOffset );
	}
}

void CX2GUNPC::SetLockOnPos( CX2DamageEffect::CEffect* pCEffect, D3DXVECTOR3 vPos )
{
	if( pCEffect != NULL )
	{
		pCEffect->SetLockOnPos( vPos );
	}
}


void CX2GUNPC::SetSiegeMode( bool bSiege )
{
	if( true == m_bSiegeMode && false == bSiege )
	{
		StateChange( m_CommonState.m_Wait );
	}
	else if( false == m_bSiegeMode && true == bSiege )
	{
		StateChange( m_SiegeState );
	}

	m_bSiegeMode = bSiege;
}

void CX2GUNPC::SetSiegeModeForce( bool bSiege )
{
	if( true == m_bSiegeMode && false == bSiege )
	{
		StateChangeForce( m_CommonState.m_Wait );
	}
	else if( false == m_bSiegeMode && true == bSiege )
	{
		StateChangeForce( m_SiegeState );
	}

	m_bSiegeMode = bSiege;
}


void CX2GUNPC::ReleaseSiegeMode( float fRange )
{
	if( NULL == g_pX2Game )
		return;

	for( int i=0; i<g_pX2Game->GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnit( i );
		if( NULL == pGUNPC || pGUNPC == this ||
			/*false == pGUNPC->IsActiveMonster() || */		// fix!! active 인지 아닌지 구분이 필요한가?
			CX2GameUnit::GUSI_DIE == pGUNPC->GetGameUnitState() )
			continue;


		if( true == pGUNPC->GetSiegeMode() &&
			GetDistance( this->GetPos(), pGUNPC->GetPos() ) < fRange )
		{
			pGUNPC->SetSiegeMode( false );
		}
	}
}

D3DXVECTOR3 CX2GUNPC::GetLineGroupStartPos()
{
	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() )
    {
        if ( CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap() )
	    {
		    const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
		    if( pLineData != NULL )
		    {
                return  pLineMap->GetLineGroupStartPos( pLineData->m_lineGroupID );
		    }
	    }
    }
	return D3DXVECTOR3( 0, 0, 0 );
}

D3DXVECTOR3 CX2GUNPC::GetLineGroupEndPos()
{
	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() )
	{
        if ( CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap() )
	    {
		    const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( m_NPCFrameData.syncData.lastTouchLineIndex );
		    if( pLineData != NULL )
		    {
                return  pLineMap->GetLineGroupEndPos( pLineData->m_lineGroupID );
		    }
        }
	}

	return D3DXVECTOR3( 0, 0, 0 );
}


// warning!! ai target pos은 sync를 안하기 때문에 훈련소처럼 방장 1인플레이가 보장된 경우에만 사용
D3DXVECTOR3 CX2GUNPC::GetAITargetPos()
{
	if( NULL != m_pAI && m_pAI->IsTargetAccquired() )
	{
		if( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
		{
			CX2GUNPCAI* pNormalAI = (CX2GUNPCAI*) m_pAI;
			return pNormalAI->GetFinalDest();
		}
	}

	return D3DXVECTOR3( 0, 0, 0 );
}


D3DXVECTOR3 CX2GUNPC::GetNearestUserPos()
{
	D3DXVECTOR3 vPos(0, 0, 0 );
	if( NULL == g_pX2Game )
		return vPos;

	float fNearestDist = 500000.f;
	D3DXVECTOR3 vNPCPos = GetPos();

	for( int i=0; i<g_pX2Game->GetUserUnitListSize(); i++ )
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnit(i);
		if( NULL == pUser )
			continue;

		float fTempDist = GetDistance( vNPCPos, pUser->GetPos() );
		if( fTempDist < fNearestDist )
		{
			fNearestDist = fTempDist;
			vPos = pUser->GetPos();
		}
	}

	return vPos;
}

D3DXVECTOR3 CX2GUNPC::GetNearestUserLandPos()
{
	D3DXVECTOR3 vPos(0, 0, 0 );
	if( NULL == g_pX2Game )
		return vPos;

	float fNearestDist = 500000.f;
	D3DXVECTOR3 vNPCPos = GetPos();

	for( int i=0; i<g_pX2Game->GetUserUnitListSize(); i++ )
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnit(i);
		if( NULL == pUser )
			continue;

		float fTempDist = GetDistance( vNPCPos, pUser->GetLandPos() );
		if( fTempDist < fNearestDist )
		{
			fNearestDist = fTempDist;
			vPos = pUser->GetLandPos();
		}
	}

	return vPos;
}

D3DXVECTOR3 CX2GUNPC::GetNearestNPCPos()
{
	D3DXVECTOR3 vPos(0, 0, 0 );
	if( NULL == g_pX2Game )
		return vPos;

	float fNearestDist = 500000.f;
	D3DXVECTOR3 vNPCPos = GetPos();

	for( int i=0; i<g_pX2Game->GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnit(i);
		if( NULL == pNPC || pNPC == this )
			continue;

		if( true == pNPC->GetIsBosRaidNPC() )
			continue;

		float fTempDist = GetDistance( vNPCPos, pNPC->GetPos() );
		if( fTempDist < fNearestDist )
		{
			fNearestDist = fTempDist;
			vPos = pNPC->GetPos();
		}
	}

	return vPos;
}



bool CX2GUNPC::IsUnitID(CX2UnitManager::NPC_UNIT_ID unitID)
{
    //if( GetNPCTemplet() != NULL)
    {
        if( GetNPCTemplet().m_nNPCUnitID == unitID)
            return true;
    }

    return false;
}

BOOL CX2GUNPC::GetDirPos_LUA(D3DXVECTOR3 vPos)
{
    BOOL isRight = TRUE;
    D3DXVECTOR3 dirVector;
    float fDist1, fDist2;

    float yRot = GetYRotateDegree();

    float yRotPI = yRot / 180.0f;
    D3DXMATRIX rotMat;

    D3DXMatrixRotationY(&rotMat, D3DX_PI * yRotPI);
    D3DXVec3TransformCoord(&dirVector, &GetDirVector(), &rotMat);
    D3DXVECTOR3 unitPos = GetPos();

    fDist1 = D3DXVec3Length(&((unitPos - dirVector) - vPos));
    fDist2 = D3DXVec3Length(&((unitPos + dirVector) - vPos));

    if(yRot >= 180)
    {
        // left
        isRight = FALSE;
    }
    else
    {
        // right
        isRight = TRUE;
    }


    // 내위치가 vPos보다 오른쪽에 있다면 FALSE
    if(isRight)
    {
        if(fDist1 < fDist2)
            return FALSE;
        else
            return TRUE;
    }
    else
    {
        if(fDist1 > fDist2)
            return FALSE;
        else
            return TRUE;
    }
}




void CX2GUNPC::AttachParticle_LUA( bool bIsMajorParticle, char* particleSeqName, char* pBoneName, D3DXVECTOR3 vOffsetPos )
{
	if( NULL == particleSeqName )
		return;

	AttachedParticle* pAttachedParticle = new AttachedParticle;
	pAttachedParticle->m_bIsMajorParticle = bIsMajorParticle;
	pAttachedParticle->m_vOffsetPos = vOffsetPos;
	ConvertUtf8ToWCHAR( pAttachedParticle->m_wstrBoneName, pBoneName );


	wstring wstrSeqName = L"";
	ConvertUtf8ToWCHAR( wstrSeqName, particleSeqName );


	if( true == bIsMajorParticle )
	{
		pAttachedParticle->m_hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrSeqName.c_str(), D3DXVECTOR3( 0, 0, 0 ) );
	}
	else
	{
		pAttachedParticle->m_hSeq = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrSeqName.c_str(), D3DXVECTOR3( 0, 0, 0 ) );
	}

	if( INVALID_PARTICLE_SEQUENCE_HANDLE == pAttachedParticle->m_hSeq )
	{
		SAFE_DELETE( pAttachedParticle );
		return;
	}

	m_vecpAttachedParticle.push_back( pAttachedParticle );

}




void CX2GUNPC::SetEnableLuaTargetingFunc( bool val )
{
	if( NULL != m_pAI )
	{
		if( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
		{
			CX2GUNPCAI* pNormalAI = (CX2GUNPCAI*) m_pAI;
			pNormalAI->SetEnableLuaTargetingFunc( val );
		}
	}
}

void CX2GUNPC::SetLuaTargetingFunc( const string& val )
{
	if( NULL != m_pAI )
	{
		if( m_pAI->GetAIType() == CX2NPCAI::NAT_NORMAL )
		{
			CX2GUNPCAI* pNormalAI = (CX2GUNPCAI*) m_pAI;
			pNormalAI->SetLuaTargetingFunc( val );
		}
	}
}

void CX2GUNPC::SetTargetUnitType( int val )
{
	wstringstream wstream;
	wstream << L"SetTargetUnitType 사용 금지: ID: ";
	wstream << this->GetNPCTemplet().m_nNPCUnitID;

	ASSERT( !"SetTargetUnitType 사용 금지: StateLog를 확인해 보세요" );
	StateLog( wstream.str().c_str() );
// 	if( NULL != m_pAI )
// 		m_pAI->SetTargetUnitType( (CX2GameUnit::GAME_UNIT_TYPE) val );
}

void CX2GUNPC::SetTargetUnitUID( UidType tartgetUID )
{
	wstringstream wstream;
	wstream << L"SetTargetUnitUID 사용 금지: ID: ";
	wstream << this->GetNPCTemplet().m_nNPCUnitID;

	ASSERT( !"SetTargetUnitUID 사용 금지: StateLog를 확인해 보세요" );
	StateLog( wstream.str().c_str() );
// 	if( NULL != m_pAI )
// 		m_pAI->SetTargetUnitUID( tartgetUID );
}

void CX2GUNPC::SetTargetUnit( CX2GameUnit *pUnit )
{
	if( pUnit == NULL )
		return;

	if( m_pAI != NULL )
	{
		m_pAI->SetTargetGameUnit( pUnit );
	}
}

int CX2GUNPC::GetTargetUnitUID()
{
	wstringstream wstream;
	wstream << L"GetTargetUnitUID 사용 금지: ID: ";
	wstream << this->GetNPCTemplet().m_nNPCUnitID;

	ASSERT( !"GetTargetUnitUID 사용 금지: StateLog를 확인해 보세요" );
	StateLog( wstream.str().c_str() );
// 	if(m_pAI != NULL)
// 	{
// 		return (int)m_pAI->GetTargetUnitUID();
// 	}

	return -1;
}

CX2GUUser* CX2GUNPC::GetTargetUser()
{
	if(m_pAI != NULL)
	{
		CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
		if ( NULL != pTargetGameUnit && CX2GameUnit::GUT_USER == pTargetGameUnit->GetGameUnitType() )
			return static_cast<CX2GUUser*>( pTargetGameUnit );
// 		UidType uTaretUnitUid = m_pAI->GetTargetUnitUID();
//
// 		for ( int i = 0; i < g_pX2Game->GetUserUnitListSize(); ++i )
// 		{
// 			CX2GUUser* pGUUser = g_pX2Game->GetUserUnit(i);
// 			if ( pGUUser != NULL && pGUUser->GetUnitUID() == uTaretUnitUid )
// 			{
// 				return pGUUser;
// 			}
// 		}
	}

	return NULL;
}

CX2GUNPC* CX2GUNPC::GetTargetNPC()
{
	if(m_pAI != NULL)
	{
		CX2GameUnit* pTargetGameUnit = m_pAI->GetTargetGameUnit();
		if ( NULL != pTargetGameUnit && CX2GameUnit::GUT_NPC == pTargetGameUnit->GetGameUnitType() )
			return static_cast<CX2GUNPC*>( pTargetGameUnit );
	}

	return NULL;
}

void CX2GUNPC::SetTimerRestart( int iIndex )
{
	if( iIndex < 0 || iIndex >= 10 )
		return;
    m_aTimerList[iIndex].Restart();
}

float CX2GUNPC::GetTimerElapsedTime( int iIndex )
{
	if( iIndex < 0 || iIndex >= 10 )
		return -1;

    return m_aTimerList[iIndex].GetTime();
}

void CX2GUNPC::SetTimerPause( int iIndex )
{
	if( iIndex < 0 || iIndex >= 10 )
		return;

    m_aTimerList[iIndex].Pause();
}

void CX2GUNPC::SetTimerResume( int iIndex )
{
	if( iIndex < 0 || iIndex >= 10 )
		return;

    m_aTimerList[iIndex].Resume();
}

bool CX2GUNPC::GetTimerResumed( int iIndex )
{
	if( iIndex < 0 || iIndex >= 10 )
		return false;

    return m_aTimerList[iIndex].GetResumed();
}

bool CX2GUNPC::TimerEventTrigger( int iIndex, float fTime )
{
	if( iIndex < 0 || iIndex >= 10 )
		return false;

    return  m_aTimerList[iIndex].EventTimer( fTime );
}




void CX2GUNPC::SetUnitColor(float r, float g, float b, float a, bool bBlend, D3DBLEND srcBlend, D3DBLEND destBlend)
{
	m_bUnitColor = bBlend;
	m_RenderParam.color = D3DXCOLOR(r, g, b, a);

	//m_RenderParam.renderType = CKTDGXRenderer::RT_REAL_COLOR;

	//m_RenderParam.bAlphaBlend = bBlend;
	m_RenderParam.srcBlend	= srcBlend;
	m_RenderParam.destBlend = destBlend;
}

void CX2GUNPC::SetUnitFadeStart(CKTDGXRenderer::RENDER_TYPE blend, float r, float g, float b, float a, bool bBlend, bool bFade)
{
	m_bUnitColor = bFade;
	m_stOriginBuffRenderParam.m_bAlphaBlend = bBlend;
	m_stOriginBuffRenderParam.m_d3dxColor = D3DXCOLOR(r, g, b, a);

	if ( bFade == true )
	{
		m_stOriginBuffRenderParam.m_eRenderType = blend; //CKTDGXRenderer::RT_CARTOON_FADE;
	}
	else
	{
		m_stOriginBuffRenderParam.m_eRenderType = CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
	}
}

void CX2GUNPC::SetUnitFadeMove(float fEndTime, float r, float g, float b, float a)
{
	m_bUnitFade = true;
	m_fFadeEndTime = fEndTime;
	m_cFadeColor = D3DXCOLOR(r, g, b, a);
}

void CX2GUNPC::SetUnitFadeScale(float fEndTime, float x, float y, float z)
{
	m_bFadeScale = true;
	m_fFadeScaleEndTime = fEndTime;
	m_vFadeScale = D3DXVECTOR3(x, y, z);
}


void CX2GUNPC::ResetAnimSpeed()
{
	if( m_pXSkinAnim != NULL )
	{
		m_AdditionalAnimSpeed.UpdateResultValue();

		/// m_fTimedEventAnimSpeed가 주어져 있으면 m_fTimedEventAnimSpeed를 사용 아니면 m_fAnimSpeed를 사용
		float fAnimSpeedValue = ( IsSamef( m_fTimedEventAnimSpeed ) ? m_fAnimSpeed : m_fTimedEventAnimSpeed );
		fAnimSpeedValue *= m_fBornAnimSpeedRate;
		fAnimSpeedValue *= GetAnimSpeedValue();

		if ( m_NPCFrameData.syncData.bComeBackState )
			fAnimSpeedValue *= 4.0f;

		m_pXSkinAnim->SetPlaySpeed( fAnimSpeedValue );
	}
}




CKTDGParticleSystem::CParticleEventSequenceHandle 	CX2GUNPC::GetMajorParticleHandle( int index )
{
	return m_hMajorSeqList[index];
}


CKTDGParticleSystem::CParticleEventSequence* CX2GUNPC::GetMajorParticle_LUA( int index )
{
	return g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hMajorSeqList[index] );
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUNPC::GetMinorParticle_LUA( int index )
{
	return g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hMinorSeqList[index] );
}

//{{ kimhc // 2010.4.19 // 비밀던전 작업

void     CX2GUNPC::SetEffectSet_LUA( int iIndex, CX2EffectSet::Handle hHandle )
{
    m_hEffectSetHandle[iIndex] = hHandle;
}


CX2EffectSet::Handle CX2GUNPC::GetEffectSet_LUA( int iIndex )
{
	return m_hEffectSetHandle[iIndex];
}
//}} kimhc // 2010.4.19 // 비밀던전 작업

/*virtual*/ void CX2GUNPC::TransformScale( const PROTECT_VECTOR3& vScale_ )
{
	m_vTransScale = vScale_;
	//m_vTransScale += (m_vBasicScale - D3DXVECTOR3( 1.0f, 1.0f, 1.0f ));

	// 오현빈 // 2013-01-02 // 스케일 변경에 따라 기본 크기가 정상적으로 변경되도록 기본 스케일 값 나눠주기
	if( 0.f < m_vBasicScale.x )
	{
		m_NPCFrameData.unitCondition.fUnitWidth	= m_vOrgUnitSize.x * (vScale_.x / m_vBasicScale.x) ;
	}
	if( 0.f < m_vBasicScale.y )
	{
		m_NPCFrameData.unitCondition.fUnitHeight = m_vOrgUnitSize.y * (vScale_.y / m_vBasicScale.y) ;
	}
}

void CX2GUNPC::DeleteArmagedonBlade()
{
	m_SubAttackListSet.clear();

	m_fArmagedonBladeTime = 0.f;
	if( INVALID_MESH_INSTANCE_HANDLE != m_hArmagedonBlade )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hArmagedonBlade );
	}
}



void CX2GUNPC::CreateArmagedonBlade( float fTime )
{
	DeleteArmagedonBlade();

	if( fTime <= 0.f )
		return;


	m_fArmagedonBladeTime = fTime;
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmagedonBladePoison", 0,0,0, 0,0,0, 0,0,0 );
	if( NULL != pMeshInst )
	{
        pMeshInst->SetPerFrameSimulation( true );
		m_hArmagedonBlade = pMeshInst->GetHandle();
		pMeshInst->SetBoundingRadius( 0.0f );
		SetSubAttackList( pMeshInst->GetXSkinAnim() );
	}
}

void CX2GUNPC::CreateArmagedonBladeByBot( float fTime )
{
	DeleteArmagedonBlade();

	if( fTime <= 0.f )
		return;


	m_fArmagedonBladeTime = fTime;
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmagedonBlade", 0,0,0, 0,0,0, 0,0,0 );
	if( NULL != pMeshInst )
	{
        pMeshInst->SetPerFrameSimulation( true );
		m_hArmagedonBlade = pMeshInst->GetHandle();
		pMeshInst->SetBoundingRadius( 0.0f );
		SetSubAttackList( pMeshInst->GetXSkinAnim() );
	}
}

void CX2GUNPC::ActiveSkillShow_LUA( char* pBackFaceTexName, float fSizeX, float fSizeY, float fPosX, float fPosY, float fPosZ )
{
	wstring wstrBackFaceTexName;

	ConvertCharToWCHAR( wstrBackFaceTexName, pBackFaceTexName );

	ActiveSkillShow( wstrBackFaceTexName.c_str(), D3DXVECTOR2(fSizeX, fSizeY), D3DXVECTOR3(fPosX, fPosY, fPosZ) );
}

void CX2GUNPC::ActiveSkillShow( const WCHAR* pBackFaceTexName, D3DXVECTOR2 size, D3DXVECTOR3 pos )
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	if( GetIsRight() == true )
	{
		CKTDGParticleSystem::CParticleEventSequence* pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Right", 0,0,0 );
		if( pPart != NULL )
		{
			pPart->ChangeTexForce( pBackFaceTexName );

			CKTDGParticleSystem::CParticleEventList::iterator iter =
				( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).begin();
			CKTDGParticleSystem::CParticleEventList::iterator iterEnd =
				( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).end();

			if ( iter == iterEnd )  goto out;
			iter++; if ( iter == iterEnd ) goto out;
			CKTDGParticleSystem::CParticleEvent_Size* pESize = (CKTDGParticleSystem::CParticleEvent_Size*) (*iter);
			pESize->SetSize( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(size.x,size.y,1), D3DXVECTOR3(size.x,size.y,1) ));

			iter++; if ( iter == iterEnd ) goto out;
			CKTDGParticleSystem::CParticleEvent_Position* pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(pos.x-size.x, pos.y, 0),D3DXVECTOR3(pos.x-size.x, pos.y, 0) ) );

			iter++; if ( iter == iterEnd ) goto out;
			pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(pos.x, pos.y, 0),D3DXVECTOR3(pos.x, pos.y, 0) ) );

			iter++; if ( iter == iterEnd ) goto out;
			pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024+size.x, pos.y, 0),D3DXVECTOR3(1024+size.x, pos.y, 0) ) );
		}

out:
		pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Talk_Right", 0,0,0 );
		if( pPart != NULL )
		{
			int texID = rand() % 3;
			wstringstream str;
			str << L"HQ_CutIn_Talk_R_" << texID << L".dds";
			pPart->ChangeTexForce( str.str().c_str() );
			pPart->SetOverUI(true);
		}
	}
	else
	{
		CKTDGParticleSystem::CParticleEventSequence* pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Left", 0,0,0 );
		if( pPart != NULL )
		{
			pPart->ChangeTexForce( pBackFaceTexName );

			CKTDGParticleSystem::CParticleEventList::iterator iter =
				( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).begin();
			CKTDGParticleSystem::CParticleEventList::iterator iterEnd =
				( const_cast<CKTDGParticleSystem::CParticleEventList&>( pPart->GetTemplet()->m_EventList ) ).end();

			if ( iter == iterEnd )  goto out2;
			iter++; if ( iter == iterEnd ) goto out2;
			CKTDGParticleSystem::CParticleEvent_Size* pESize = (CKTDGParticleSystem::CParticleEvent_Size*) (*iter);
			pESize->SetSize( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(size.x,size.y,1), D3DXVECTOR3(size.x,size.y,1) ));

			iter++; if ( iter == iterEnd ) goto out2;
			CKTDGParticleSystem::CParticleEvent_Position* pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024+size.x, pos.y, 0),D3DXVECTOR3(1024+size.x, pos.y, 0) ) );

			iter++; if ( iter == iterEnd ) goto out2;
			pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(1024-pos.x, pos.y, 0),D3DXVECTOR3(1024-pos.x, pos.y, 0) ) );

			iter++; if ( iter == iterEnd ) goto out2;
			pEPos = (CKTDGParticleSystem::CParticleEvent_Position*) (*iter);
			pEPos->SetPosition( CMinMax<D3DXVECTOR3>( D3DXVECTOR3(-size.x, pos.y, 0),D3DXVECTOR3(-size.x, pos.y, 0) ) );
		}

out2:
		pPart = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"CutIn_Talk_Left", 0,0,0 );
		if( pPart != NULL )
		{
			int texID = rand() % 3;
			wstringstream str;
			str << L"HQ_CutIn_Talk_L_" << texID << L".dds";
			pPart->ChangeTexForce( str.str().c_str() );
			pPart->SetOverUI(true);
		}
	}

	g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"FocusLine", D3DXVECTOR3(0,0,0) );
	g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"OutLine", D3DXVECTOR3(0,0,0) );
}

float CX2GUNPC::GetStopTime()
{
	return m_NPCFrameData.unitCondition.fStopTime;

}










void CX2GUNPC::SetStopTime( float fStopTime )
{
	if( true == m_bImmunityTimeStop )
		return;

	m_NPCFrameData.unitCondition.fStopTime = fStopTime;

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		g_pX2Game->GetWorld()->GetLineMap()->SetStopTime( max( g_pX2Game->GetWorld()->GetLineMap()->GetStopTime(), fStopTime ) );
	}





}

void CX2GUNPC::SetStop2Time( float fStopTime )
{
	if( true == m_bImmunityTimeStop )
		return;

	m_NPCFrameData.unitCondition.fStop2Time = fStopTime;

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		g_pX2Game->GetWorld()->GetLineMap()->SetStopTime( max( g_pX2Game->GetWorld()->GetLineMap()->GetStopTime(), fStopTime ) );
	}




}

void CX2GUNPC::AddConsultedNpc(CX2GUNPC *pConsultedNpc)
{
	if(pConsultedNpc != NULL)
		m_vecConsultedNpc.push_back(pConsultedNpc);
}

void CX2GUNPC::AddConsultNpc(CX2GUNPC *pConsultNpc)
{
	if(pConsultNpc != NULL)
	{
		bool bAddConsultNpc = true;
		for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
		{
			if(m_vecConsultNpc[i] != NULL && m_vecConsultNpc[i] == pConsultNpc)
			{
				bAddConsultNpc = false;
				break;
			}
		}
		if(bAddConsultNpc == true)
		{
			m_vecConsultNpc.push_back(pConsultNpc);
			pConsultNpc->AddConsultedNpc(this);
		}
	}
}

void CX2GUNPC::ClearConsultNpc(CX2GUNPC *pConsultNpc)
{
	if(pConsultNpc != NULL)
	{
		for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
		{
			if(m_vecConsultNpc[i] != NULL && m_vecConsultNpc[i] == pConsultNpc)
			{
				m_vecConsultNpc.erase(m_vecConsultNpc.begin() + i);
				return;
			}
		}
	}
}

void CX2GUNPC::ClearConsultedNpc(CX2GUNPC *pConsultedNpc)
{
	if(pConsultedNpc != NULL)
	{
		for(UINT i=0; i<m_vecConsultedNpc.size(); ++i)
		{
			if(m_vecConsultedNpc[i] != NULL && m_vecConsultedNpc[i] == pConsultedNpc)
			{
				m_vecConsultedNpc.erase(m_vecConsultedNpc.begin() + i);
				return;
			}
		}
	}
}

CX2GUNPC *CX2GUNPC::GetConsultNpc(CX2UnitManager::NPC_UNIT_ID unitID)
{
	for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
	{
		if(m_vecConsultNpc[i] != NULL && m_vecConsultNpc[i]->IsUnitID(unitID) == true)
		{
			return m_vecConsultNpc[i];
		}
	}

	// 참고할 몬스터를 설정한다.
	CX2GUNPC *pUnit = g_pX2Game->GetNPCUnitByType( unitID );
	if( pUnit != NULL )
	{
		AddConsultNpc(pUnit);
		return pUnit;
	}

	return NULL;
}
//{{oasis:김상윤////2009-10-7////
void CX2GUNPC::SetAllFlag_LUA( bool bFlag )
{
	for(UINT i=0; i<10; i++)
	{
		m_FlagList[i] = bFlag;
	}
	return;
}


CX2GUNPC *CX2GUNPC::GetOnlyConsultNPC_LUA(CX2UnitManager::NPC_UNIT_ID unitID)
{
	for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
	{
		if(m_vecConsultNpc[i] != NULL && m_vecConsultNpc[i]->IsUnitID(unitID) == true)
		{
			return m_vecConsultNpc[i];
		}
	}
	return NULL;
}

CX2GUNPC *CX2GUNPC::GetOnlyConsultedNPC_LUA(CX2UnitManager::NPC_UNIT_ID unitID)
{
	for(UINT i=0; i<m_vecConsultedNpc.size(); ++i)
	{
		if(m_vecConsultedNpc[i] != NULL && m_vecConsultedNpc[i]->IsUnitID(unitID) == true)
		{
			return m_vecConsultedNpc[i];
		}
	}
	return NULL;
}



CX2GUNPC *CX2GUNPC::GetConsultNearNPC_LUA( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, float fMaxRange )
{
	for(UINT i=0; i<m_vecConsultNpc.size(); ++i)
	{
		if(m_vecConsultNpc[i] != NULL && m_vecConsultNpc[i]->IsUnitID(unitID) == true)
		{
			return m_vecConsultNpc[i];
		}
	}
	// 참고할 몬스터를 설정한다.
	CX2GUNPC *pUnit = g_pX2Game->GetNearNoneAINPCUnitUID(pos, unitID, fMaxRange);
	if( pUnit != NULL )
	{
		AddConsultNpc(pUnit);
		return pUnit;
	}
	return NULL;
}

bool CX2GUNPC::IsRightTarget_LUA(CX2GUNPC *pStartNpc, D3DXVECTOR3 pos)
{
	bool bRight = false;

	if(g_pX2Game == NULL || g_pX2Game->GetWorld() == NULL || g_pX2Game->GetWorld()->GetLineMap() == NULL)
		return true;

	const CKTDGLineMap::LineData* pCurrLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pStartNpc->GetNPCSyncData().lastTouchLineIndex ); //m_pMyUnit->GetLastTouchLineIndex() );

	// pCurrLineData가 NULL일경우 처리는....????
	if(pCurrLineData == NULL)
		return false;

	D3DXVECTOR3 vFinalDestDir = pos - pStartNpc->GetPos();
	float fFinalDestDist = D3DXVec3Length( &vFinalDestDir );
	D3DXVec3Normalize( &vFinalDestDir, &vFinalDestDir );
	const D3DXVECTOR3 vUpVec(0,1,0);
	D3DXVECTOR3 vRightVec = pCurrLineData->dirVector;
	D3DXVec3Normalize( &vRightVec, &vRightVec );

	float fIsUp    = D3DXVec3Dot( &vUpVec,    &vFinalDestDir );
	float fIsRight = D3DXVec3Dot( &vRightVec, &vFinalDestDir );
	if( fIsRight > 0.f )
		bRight = true;
	else
		bRight = false;

	return bRight;
}
//}}oasis:김상윤////2009-10-7////




/*virtual*/ float CX2GUNPC::GetPowerRate()
{
	return m_fDefaultPowerRate;

	// 나중에는 user처럼 각 상태별로 powerrate입력하도록 바꿔야
	//return 1.f;
}




CX2GameUnit* CX2GUNPC::GetHitter()
{
	switch( m_eHitterUnitType )
	{
	case CX2GameUnit::GUT_USER:
		{
			return (CX2GameUnit*)g_pX2Game->GetUserUnitByUID( m_HitterUID );
		} break;

	case CX2GameUnit::GUT_NPC:
		{
			return (CX2GameUnit*)g_pX2Game->GetNPCUnitByUID( (int)m_HitterUID );
		} break;
	}

	return NULL;
}


void CX2GUNPC::SetUserGrapReset()
{
	m_bUserGrap = false;

	if ( !m_vecGameUnitoPtr.empty() )
	{
		for( CX2GameUnitoPtr oPtr : m_vecGameUnitoPtr )
		{
			if ( null != oPtr && oPtr->GetGameUnitType() == CX2GameUnit::GUT_USER )
			{
				CX2GUUser* pGrappledUser = static_cast<CX2GUUser*>( oPtr.GetObservable() );
				pGrappledUser->SetGrap( false );
				pGrappledUser->ResetToBeforeGrapPos();
			}
		}
	}

	m_vecGameUnitoPtr.clear();
}


bool CX2GUNPC::IsLostGrap()
{
	if ( m_vecGameUnitoPtr.empty() )
		return true;
	else
	{
		for( CX2GameUnitoPtr oPtr : m_vecGameUnitoPtr )
		{
			if ( null != oPtr && oPtr->GetGameUnitType() == CX2GameUnit::GUT_USER )
			{
				CX2GUUser* pGrappledUser = static_cast<CX2GUUser*>( oPtr.GetObservable() );
				if ( !pGrappledUser->GetGrap() )
					return true;
			}
		}
	}

	return false;
}
//{{ oasis907 : 김상윤 [2011.1.25] //
void CX2GUNPC::SetUserGrap( bool val, D3DXVECTOR3 vGrapPosition )
{
	m_bUserGrap = val;
	m_vGrapPosition = vGrapPosition;
	return;
}
//}}


void CX2GUNPC::SetUserHoldReset()
{
	m_bHold = false;

	for ( int i = 0; i < g_pX2Game->GetUserUnitListSize(); i++ )
	{
		CX2GUUser* pGUUser = g_pX2Game->GetUserUnit(i);
		if ( pGUUser != NULL )
		{
			pGUUser->SetHold(m_bHold);
		}
	}

//{{ kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
	m_vecHoldUserUID.resize( 0 );
//}} kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함

	m_vHoldOffSet = D3DXVECTOR3( 0.f, 0.f, 0.f );
}

//{{ kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
	void CX2GUNPC::SetUserHold(bool val, const char* pBoneName, bool bHoldOnlyOneUser /*= true*/, D3DXVECTOR3 vHoldOffSet /*= D3DXVECTOR3( 0.f, 0.f, 0.f )*/ )
//}} kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함

{
	if(m_bHold == val)
		return;

	m_bHold = val;

//{{ kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함
	wstring wBoneName;
	ConvertUtf8ToWCHAR( m_wstrHoldBone, pBoneName );

	SetHoldOnlyOneUser( bHoldOnlyOneUser );
//}} kimhc // 2010.7.12 // 여러명의 유저를 HOLD 할 수 있도록 함

	m_vHoldOffSet = vHoldOffSet;

}

void CX2GUNPC::SetEnableDash( bool bEnableDash, CX2GameUnit::ENABLE_DASH_STATE eState /*= EDS_END*/ )
{
	CX2GameUnit::SetEnableDash( bEnableDash, eState );
}

int CX2GUNPC::GetUnitLevel() const
{
	if( IsPvpBot() == true )
	{
		CX2Room::RoomNpcSlot *pSlotData = g_pX2Room->GetNpcSlotData( GetUnitUID() );
		if( pSlotData != NULL )
		{
			return pSlotData->m_iLevel;
		}
	}
	return m_HardLevel;
}

void CX2GUNPC::PushAIMessage( STATE_CHANGE_TYPE message, bool bIsRight )
{
	if( false == GetEnableDash() &&
		SCT_AI_DASH == message )
	{
		message = SCT_AI_WALK;
	}

	m_AIMessage = message;
	m_bAIIsRight = bIsRight;
}
// oasis /
void CX2GUNPC::PushUnUseAIMessage( STATE_CHANGE_TYPE message)
{
	m_UnUseAIMessage = message;
	return;
}
//////////////////////////


//////////////////////////////////////////////////////////////////////////


CX2GUNPC::AttachedParticle::~AttachedParticle()
{
	if( true == m_bIsMajorParticle )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeq );
	}
	else
	{
		g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeq );
	}
}

CKTDGParticleSystem::CParticleEventSequence*    CX2GUNPC::AttachedParticle::GetInstanceSequence()
{
    if ( m_hSeq == INVALID_PARTICLE_SEQUENCE_HANDLE )
        return NULL;
    return ( m_bIsMajorParticle == true )
        ? g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeq )
        : g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeq );
}


//////////////////////////////////////////////////////////////////////////
CX2GUNPC::WeaponParticle::WeaponParticle()
{
	m_wstrBoneName = L"";
	m_bIsMajorParticle = true;
	m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
}

CX2GUNPC::WeaponParticle::~WeaponParticle()
{
	if( true == m_bIsMajorParticle )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeq );
	}
	else
	{
		g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeq );
	}
}



//{{oasis:김상윤////2009-10-7////
void CX2GUNPC::COMMAND_HOLD_LUA()
{
	SetGroupAICommand_LUA(CX2GUNPC::GAI_CMD_HOLD);
	return;
}

void CX2GUNPC::COMMAND_FORCED_MOVE_TARGET_LUA( CX2GameUnit* pGroupAiTargetGameUnit_ )
{
	SetGroupAICommand_LUA(CX2GUNPC::GAI_CMD_FORCED_MOVE);
	SetGroupAIGroundTarget( false );
	SetGroupAITargetGameUnit( pGroupAiTargetGameUnit_ );
	return;
}


void CX2GUNPC::COMMAND_FORCED_MOVE_GROUND_LUA(D3DXVECTOR3 targetPos)
{
	SetGroupAICommand_LUA(CX2GUNPC::GAI_CMD_FORCED_MOVE);
	SetGroupAIGroundTarget( true );
	SetGroupAITargetPos(targetPos);
	return;
}


void CX2GUNPC::COMMAND_ATTACK_MOVE_LUA( CX2GameUnit* pGroupAiTargetGameUnit_ )
{
	SetGroupAICommand_LUA(CX2GUNPC::GAI_CMD_ATTACK_MOVE);
	SetGroupAIGroundTarget( false );
	SetGroupAITargetGameUnit( pGroupAiTargetGameUnit_ );
	return;
}

void CX2GUNPC::COMMAND_SKILL_LUA( CX2GUNPC::GROUP_AI_COMMAND command, CX2GameUnit* pGroupAiTargetGameUnit_ )
{
	SetGroupAICommand_LUA( command );
	SetGroupAIGroundTarget( false );
	SetGroupAITargetGameUnit( pGroupAiTargetGameUnit_ );
	return;
}

int CX2GUNPC::GetTargetUnitUID_LUA()
{
	wstringstream wstream;
	wstream << L"GetTargetUnitUID_LUA 사용 금지: ID: ";
	wstream << this->GetNPCTemplet().m_nNPCUnitID;

	ASSERT( !"GetTargetUnitUID_LUA 사용 금지: StateLog를 확인해 보세요" );
	StateLog( wstream.str().c_str() );
// 	if(m_pAI != NULL)
// 	{
// 		return (int)m_pAI->GetTargetUnitUID();
// 	}

	return -1;
}


void CX2GUNPC::SetPrevAIMessage_LUA( CX2GUNPC::STATE_CHANGE_TYPE val)
{
	if( NULL == m_pAI )
		return;

	m_pAI->SetPrevAIMessage( val );
	return;
}


void CX2GUNPC::SetAItoMainAIData_LUA()
{
	CX2GUNPCAI* pNPCAI = (CX2GUNPCAI*)GetNPCAI();
	pNPCAI->SetAItoMainAIData();

	return;
}
void CX2GUNPC::SetAItoSubAIData_LUA()
{
	CX2GUNPCAI* pNPCAI = (CX2GUNPCAI*)GetNPCAI();
	pNPCAI->SetAItoSubAIData();
	return;
}


//}}oasis:김상윤////2009-10-7////

int CX2GUNPC::GetStateIDByKeyword( const std::string& strKeyword )
{
	std::map<string,char>::const_iterator iter;
	for( iter = GetInitStateData().m_mapStateNameID.begin(); iter != GetInitStateData().m_mapStateNameID.end(); ++iter )
	{
		if( NULL != StrStrA( iter->first.c_str(), strKeyword.c_str()) )
		{
			return iter->second;
		}
	}
	return GUSI_NONE;
}

const string& CX2GUNPC::GetStateNameByStateID( int iStateID )
{
	std::map<string,char>::const_iterator iter;
	for( iter = GetInitStateData().m_mapStateNameID.begin(); iter != GetInitStateData().m_mapStateNameID.end(); ++iter )
	{
		if( iter->second == iStateID )
		{
			return iter->first;
		}
	}
	return m_strErrorMonsterStateName;
}

//{{ kimhc // 2010.6.29 //스테이트들 중에서 특정 데미지 스테이트를 다른 스테이트로 변경

// small_front, big_back 등의 여러 데미지 스테이트 들 중에서 A라는 stateID를 가진것을 B라는 stateID로 변경
void CX2GUNPC::ChangeDamageStateID_LUA( char* lpstrSrcState, char* lpstrDestState )
{
	//wstring wstrSrcState, wstrDestState;

	//ConvertCharToWCHAR( wstrSrcState, lpstrSrcState );
	//ConvertCharToWCHAR( wstrDestState, lpstrDestState );

	int iSrcStateID		= GetStateID( lpstrSrcState );
	int iDestStateID	= GetStateID( lpstrDestState );

	ChangeStateID( &m_SmallDamageLandFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_SmallDamageLandBack, iSrcStateID, iDestStateID );
	ChangeStateID( &m_SmallDamageAir, iSrcStateID, iDestStateID );

	ChangeStateID( &m_SmallDamageAirFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_SmallDamageAirBack, iSrcStateID, iDestStateID );

	ChangeStateID( &m_BigDamageLandFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_BigDamageLandBack, iSrcStateID, iDestStateID );
	ChangeStateID( &m_BigDamageAir, iSrcStateID, iDestStateID );

	ChangeStateID( &m_BigDamageAirFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_BigDamageAirBack, iSrcStateID, iDestStateID );

	ChangeStateID( &m_UpDamage, iSrcStateID, iDestStateID );

	ChangeStateID( &m_FlyDamageFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_FlyDamageBack, iSrcStateID, iDestStateID );

	ChangeStateID( &m_CommonState.m_DownDamageLandFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_CommonState.m_DownDamageLandBack, iSrcStateID, iDestStateID );
	ChangeStateID( &m_CommonState.m_DownDamageAir, iSrcStateID, iDestStateID );

	ChangeStateID( &m_DamageFlushLandFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DamageFlushLandBack, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DamageFlushAir, iSrcStateID, iDestStateID );

	ChangeStateID( &m_DamageRevenge, iSrcStateID, iDestStateID );

	ChangeStateID( &m_SmallDamageLandGuard, iSrcStateID, iDestStateID );
	ChangeStateID( &m_BigDamageLandGuard, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DownDamageLandGuard, iSrcStateID, iDestStateID );
}

// dying_front, dying_back 등의 여러 dying 스테이드 들 중에서 A라는 stateID를 가진것을 B라는 stateID로 변경
void CX2GUNPC::ChangeDyingStateID_LUA( char* lpstrSrcState, char* lpstrDestState )
{
	//wstring wstrSrcState, wstrDestState;

	//ConvertCharToWCHAR( wstrSrcState, lpstrSrcState );
	//ConvertCharToWCHAR( wstrDestState, lpstrDestState );

	int iSrcStateID		= GetStateID( lpstrSrcState );
	int iDestStateID	= GetStateID( lpstrDestState );

	ChangeStateID( &m_DyingLandFront, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DyingLandBack, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DyingSky, iSrcStateID, iDestStateID );
	ChangeStateID( &m_DyingFly, iSrcStateID, iDestStateID );
}
//}} kimhc // 2010.6.29 //스테이트들 중에서 특정 데미지 스테이트를 다른 스테이트로 변경

//{{ kimhc // 2010.7.7 // 몬스터가 생성한 라인맵

void CX2GUNPC::MoveMonsterLineMap( const D3DXVECTOR3& vPosBackup )
{
	// 이 몬스터가 생성한 라인맵이 없으면 리턴함
	if ( m_vecMonsterLineMapIndex.size() == 0 )
		return;

	// 이동거리의 변화가 없으면 리턴함
    if ( m_NPCFrameData.syncData.position == vPosBackup )
 		return;

	//if ( m_PhysicParam.passiveSpeed.x == 0 && m_PhysicParam.passiveSpeed.y == 0 )
	//	return;

	//int iUserUnitSize	= g_pX2Game->GetUserUnitListSize();
	int iNpcUnitSize	= g_pX2Game->GetNPCUnitListSize();

	// 라인맵 인덱스 별로 분류한 유닛 포인터 리스트
// 	map<int, vector<CX2GUUser*> > mapUserList;	// map< 현재 위치해있는 라인맵인덱스, 유저 포인터 >
//
// 	for ( int i = 0; i < iUserUnitSize; i++ )
// 	{
// 		CX2GUUser* pUser = g_pX2Game->GetUserUnit( i );
//
// 		if ( pUser != NULL && pUser->IsMyUnit() == true && pUser->IsOnSomethingFuture() == true )
// 		{
// 			int iIndex = pUser->GetSyncData( true ).lastTouchLineIndex;
// 			map<int, vector<CX2GUUser*> >::iterator	mItrUserList = mapUserList.find( iIndex );
//
// 			// 해당 라인맵으로 된 key가 없으면
// 			if ( mItrUserList == mapUserList.end() )
// 			{
// 				vector<CX2GUUser*> vecUserList;
// 				vecUserList.push_back( pUser );
// 				mapUserList.insert( std::make_pair( iIndex, vecUserList ) );
// 			}
// 			else
// 				mItrUserList->second.push_back( pUser );
// 		}
// 	}

	//라인맵 인덱스 별로 분류한 NPC 포인터 리스트
	map<int, vector<CX2GUNPC*> > mapNpcList;	// map< 현재 위치해있는 라인맵인덱스, NPC 포인터 >

	for ( int i = 0; i < iNpcUnitSize; i++ )
	{
		CX2GUNPC* pNpc = g_pX2Game->GetNPCUnit( i );

		if ( pNpc != NULL && pNpc->IsOnSomething() == true && pNpc != this )
		{
			int iIndex = pNpc->m_NPCFrameData.syncData.lastTouchLineIndex;
			map<int, vector<CX2GUNPC*> >::iterator	mItrNpcList = mapNpcList.find( iIndex );

			// 해당 라인맵으로 된 key가 없으면
			if ( mItrNpcList == mapNpcList.end() )
			{
				vector<CX2GUNPC*> vecNpcList;
				vecNpcList.push_back( pNpc );
				mapNpcList.insert( std::make_pair( iIndex, vecNpcList ) );
			}
			else
				mItrNpcList->second.push_back( pNpc );
		}
	}

// 	float fSpeed = m_PhysicParam.passiveSpeed.x;
//
// 	if ( GetIsRight() == false )
// 		fSpeed *= -1;

	// 이동거리 벡터
	D3DXVECTOR3 vMoveDistance = m_NPCFrameData.syncData.position - vPosBackup;

	for( int& val : m_vecMonsterLineMapIndex )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( val );

		if ( pLineData != NULL && pLineData->m_bTempLineMap == true ) // enable은 체크 안함
		{
            D3DXVECTOR3 vStartPos = pLineData->startPos + vMoveDistance;
            D3DXVECTOR3 vEndPos = pLineData->endPos + vMoveDistance;
            g_pX2Game->GetLineMap()->UpdateLineData( val, vStartPos, vEndPos );

			//pLineData->m_fSpeed = fSpeed;//vMoveDistance.x;

			//// 유저들중에 해당 몬스터가 생성한 라인맵에 서있는 유저 검색
			//map<int, vector<CX2GUUser*> >::iterator mItrUserList = mapUserList.find( val );
			//if ( mItrUserList != mapUserList.end() )
			//{
			//	for( CX2GUUser* pUser : mItrUserList->second )
			//	{
			//		const_cast< CX2GUUser::FrameData* >( pUser->GetFrameData( true ) )->syncData.position += vMoveDistance;
			//		//pUser->SetPos( pUser->GetPos( true ) + vMoveDistance );
			//	}
			//}

			if ( NULL != g_pX2Game->GetMyUnit() && g_pX2Game->GetMyUnit()->IsOnSomethingFuture() == true &&
				 g_pX2Game->GetMyUnit()->GetSyncData( true ).lastTouchLineIndex == val )
				g_pX2Game->GetMyUnit()->AccessFutureFrameData().syncData.position += vMoveDistance;


			// NPC들 중에 해당 몬스터가 생성한 라인맵에 서있는 유저 검색
			map<int, vector<CX2GUNPC*> >::iterator mItrNpcList = mapNpcList.find( val );
			if ( mItrNpcList != mapNpcList.end() )
			{
				for( CX2GUNPC* pNpc : mItrNpcList->second )
				{
					pNpc->m_NPCFrameData.syncData.position += vMoveDistance;
					//pNpc->SetPosition( pNpc->GetPos() + vMoveDistance, pNpc->GetIsRight() );
				}
			}

		}
	}
}

//}} kimhc // 2010.7.7 // 몬스터가 생성한 라인맵


void CX2GUNPC::SetCloaking()
{
	m_Invisible.m_bCloaking = true;
	m_Invisible.m_bCloaked = false;
}
void CX2GUNPC::SetUnCloaking()
{
	m_Invisible.m_bCloaking = false;
	m_Invisible.m_bCloaked = false;
}




#if X2_DEV_TOOLS
void CX2GUNPC::EnumerateStateAndAnimationName( std::vector<std::string>& vecStateName, std::vector<std::wstring>& vecAnimationName )
{
	vecStateName.resize(0);

	std::map<string,char>::const_iterator it;
	for( it = GetInitStateData().m_mapStateNameID.begin(); it != GetInitStateData().m_mapStateNameID.end(); ++it )
	{
		vecStateName.push_back( it->first );

		if( true == m_LuaManager.BeginTableByReference( it->second ) )
		{
			std::wstring animationName( L"" );
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animationName, L"" );
			if( false == animationName.empty() )
			{
				vecAnimationName.push_back( animationName );
			}
			else
			{
				vecAnimationName.push_back( L"_noname" );
			}
			m_LuaManager.EndTable();
		}

		while( vecAnimationName.size() < vecStateName.size() )
		{
			vecAnimationName.push_back( L"_error" );
		}
	}
}



#endif // MONSTER_STATE_LIST_TEST




//#ifdef BOOST_SINGLETON_POOL_TEST
//	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CX2GUNPC::NPCEventProcessData)> CX2GUNPC_NPCEventProcessDataPool;
//	typedef boost::singleton_pool<boost::pool_allocator_tag, sizeof(CX2GUNPC::ConditionTableData)> CX2GUNPC_ConditionTableDataPool;
//
//
//
//
//	void* CX2GUNPC::NPCEventProcessData::operator new(size_t s)
//	{
//		return CX2GUNPC_NPCEventProcessDataPool::malloc();
//	}
//
//	void CX2GUNPC::NPCEventProcessData::operator delete(void *p)
//	{
//		CX2GUNPC_NPCEventProcessDataPool::free(p);
//	}
//
//
//
//
//
//	void* CX2GUNPC::ConditionTableData::operator new(size_t s)
//	{
//		return CX2GUNPC_ConditionTableDataPool::malloc();
//	}
//
//	void CX2GUNPC::ConditionTableData::operator delete(void *p)
//	{
//		CX2GUNPC_ConditionTableDataPool::free(p);
//	}
//
//
//#endif BOOST_SINGLETON_POOL_TEST




void    CX2GUNPC::MarkNPCUnitDieNotReceived()
{
    SetNowHp( 0.f );
    m_eNPCUnitDieState = eNPCUnitDieState_NOT;
}




// NOTE: 이 함수는 소스 코드에서는 사용하지 말아주세요~ ^^ 루아 스크립트에서만 사용해주세요
void CX2GUNPC::SetNowHP_LUA( float fHP )
{


	if( IsLocalUnit() == true )
	{

		*m_fDamageHistory = GetMaxHp() - fHP;


	}

	SetNowHp( fHP );


}


//{{ JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)
void CX2GUNPC::CommonFrameMove_DrainHPParticle()
{
	if( m_hSeqDrainHPEffect == INVALID_PARTICLE_SEQUENCE_HANDLE )
		return;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqDrainHPEffect );
	if( NULL == pSeq )
	{
		m_hSeqDrainHPEffect = INVALID_PARTICLE_SEQUENCE_HANDLE;
		return;
	}

	D3DXVECTOR3 pos = GetPos();
	pos.y += 60.0f;
	pSeq->SetBlackHolePosition( pos );
	if( pSeq->GetTime() > 1.5f )
	{
		pSeq->SetGravity( CMinMax<D3DXVECTOR3>(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)) );
		pSeq->SetUseLand( false );
	}
}

//}} JHKang / 강정훈 / 2010/12/20 / 피 흡수 이펙트(SetDie 참고)

void CX2GUNPC::SetApplyMotionOffset( bool bApply )
{
	if(m_pXSkinAnim != NULL)
		m_pXSkinAnim->SetApplyMotionOffset(bApply);

	return;
}

bool CX2GUNPC::DoScaleHeadBone()
{
	if( m_pXSkinAnim != NULL && m_fExtraBigHead > 0.f )
	{
        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pFrame_Bip01_Head_Real;
		if(pFrame != NULL && pFrame->m_bScale == false)
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
			SetFrameScale(&vScale, true, pFrame);

			return true;
		}
	}
	else if( m_pXSkinAnim != NULL )
	{
        CKTDXDeviceXSkinMesh::MultiAnimFrame* pFrame = m_pFrame_Bip01_Head_Real;
		if(pFrame != NULL && pFrame->m_bScale == true)
		{
			SetFrameScaleRestore(pFrame);
		}
	}

	return false;
}

//{{ 허상형 : [2011/2/27/] //	월드 미션(항상 Ally 상태인 NPC 처리)
bool CX2GUNPC::IsAllyNPCID( CX2UnitManager::NPC_UNIT_ID nNpcID )
{
	switch( nNpcID )
	{
	case CX2UnitManager::NUI_MOTOR:		//샌더 5번 던전 동력장치
	case CX2UnitManager::NUI_DEFENCE_CRYSTAL:
	case CX2UnitManager::NUI_VALENTINE_POISON_CUP_CAKE_KILLER:
	case CX2UnitManager::NUI_VALENTINE_BOX_MAKER:

	case CX2UnitManager::NUI_GOD_GATE_ALLY:
	case CX2UnitManager::NUI_GOD_DEFENDER_FOREST_GUARDIAN_EVENT:
	case CX2UnitManager::NUI_GOD_SWORDMAN_MONKEY_WILD_EVENT:
	case CX2UnitManager::NUI_GOD_ARCHER_CACTUS_EVENT:
	case CX2UnitManager::NUI_GOD_MAGICIAN_MONKEY_BOMB_EVENT:
	case CX2UnitManager::NUI_GOD_SPECIAL_RED_KNIGHT_EVENT:

	case CX2UnitManager::NUI_EVENT_CRAYON_ELRIN: // 이벤트 크레용 팝-엘린
		return true;

	default:
		return false;
	}

	return false;
}
//}} 허상형 : [2011/2/27/] //	월드 미션(항상 Ally 상태인 NPC 처리)


void CX2GUNPC::ChangeAfterImageColor(D3DXCOLOR _AfterImageColor)
{
	if(m_pAfterImage != NULL)
	{
		m_pAfterImage->SetAfterImageColor(_AfterImageColor);
	}
}

wstring CX2GUNPC::GetUnitName()
{
	if( IsPvpBot() == true && g_pX2Room != NULL && g_pX2Room->GetNpcSlotData( GetUnitUID() ) != NULL )
	{
		return g_pX2Room->GetNpcSlotData( GetUnitUID() )->m_wstrNpcName;
	}

	return GetNPCTemplet().m_Name;
}


void CX2GUNPC::CreateAllyNpcByMonster_LUA( CX2UnitManager::NPC_UNIT_ID eNpcID)
{
	if( CX2UnitManager::NUI_NONE == eNpcID )
		return;

	if( NULL != g_pX2Game )
	{
		//소환중인 몬스터가 없는 유저 찾기
		int iUserUnitNum  = g_pX2Game->GetUserUnitNum();
		CX2GUUser* pCX2GUUser = NULL;
		bool bHasNotSummonMonster = false;
		for( int i = 0; i < iUserUnitNum; ++i )
		{
			pCX2GUUser = g_pX2Game->GetUserUnit(i);
			if( NULL != pCX2GUUser )
			{
				int iNpcUID = pCX2GUUser->GetSummonMonsterCardData().GetSummonMonsterUID();
				if( -1 == iNpcUID )
				{
					bHasNotSummonMonster = true;
					break;
				}
			}
		}



		if ( eNpcID != CX2UnitManager::NUI_SHASHA_STAGE0 &&
			 eNpcID != CX2UnitManager::NUI_SHASHA_STAGE1 &&
			 eNpcID != CX2UnitManager::NUI_SHASHA_STAGE2 )
		{
			//모든 유저가 몬스터 카드를 소환했다면 1명의 몬스터카드 강제로 죽이기
			if( false == bHasNotSummonMonster )
			{
				pCX2GUUser = g_pX2Game->GetUserUnit(0);
				if( NULL != pCX2GUUser )
				{
					int iNpcUID = pCX2GUUser->GetSummonMonsterCardData().GetSummonMonsterUID();
					if( -1 != iNpcUID )
					{
						CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID(iNpcUID);
						if( NULL != pCX2GUNPC )
						{
							pCX2GUNPC->SetNowHp(0.f);
						}
					}
				}
			}
		}

		if( NULL != pCX2GUUser )
		{
			const int iMonsterLevel = pCX2GUUser->GetUnitLevel();
			const D3DXVECTOR3 vPos = GetPos();

			g_pX2Game->CreateNPCReq( eNpcID, iMonsterLevel, true,
				vPos, pCX2GUUser->GetIsRight(), 0.f, true, -1, CX2Room::TN_RED,
				CX2NPCAI::NAT_ALLY, pCX2GUUser->GetUnitUID(),false, CX2Room::TN_NONE, CX2GUNPC::NCT_MONSTER_CARD );
		}
	}

}
void CX2GUNPC::SetCoolTimeSummonCard_LUA()
{
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
	{
		g_pData->GetUIManager()->GetUIQuickSlot()->UpdateCoolTimeByGroupID(101);
	}
}

CX2GameUnit* CX2GUNPC::GetNearestUnitPosByOwnerUnit( float fAttackRange )		/// NPC를 소환한 유저와 가장 가까이 있는 적 유닛을 반환
{
	CX2GameUnit* pOwnerGameUnit = GetOwnerGameUnit();		/// NPC를 소환한 유저

	if( NULL == pOwnerGameUnit )
		return NULL;

	const float fAttackRangeSq = fAttackRange * fAttackRange;
	float	fDistanceSq   = fAttackRangeSq;				/// 가장 가까운 거리 저장용 변수 ( 최대 사정 거리로 초기화 )

	const vector<CX2GameUnit*>&	pUnitList   = g_pX2Game->GetUnitList();	/// 모든 유닛 리스트
	CX2GameUnit*			pTargetUnit = NULL;						/// 공격 범위로 삼을 유닛
	const D3DXVECTOR3&		vPos		= pOwnerGameUnit->GetPos();		/// NPC를 소환한 유저의 위치
	int						iTeam		= pOwnerGameUnit->GetTeam();		/// NPC를 소환한 유저의 팀

	for( UINT i = 0; i < pUnitList.size(); ++i )		/// 모든 유닛 검사
	{
		if( NULL == pUnitList[i] || iTeam == pUnitList[i]->GetTeam() )		/// 유닛이 없거나, 같은 팀이면 패스
			continue;

		if( 0 >= pUnitList[i]->GetNowHp() )				/// 사망한 적은 패스
			continue;

		if( pUnitList[i]->GetGameUnitType() == CX2GameUnit::GUT_NPC )	/// 일반적인 NPC에 한해서만 체크 ( 상자, 던전 내 더미 객체 등등은 제외 )
		{
			CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pUnitList[i] );
			if( NULL == pNPC ||
				CX2UnitManager::NCT_BASIC != pNPC->GetNPCTemplet().m_ClassType ||
				true == pNPC->m_NPCFrameData.stateParam.bInvincible
				)
				continue;

			if( true == pNPC->GetIsBosRaidNPC() )
				continue;
		}
		else	/// 유저일 경우 무적이 아닐 때만 체크
		{
			CX2GUUser* pUser = static_cast<CX2GUUser*>( pUnitList[i] );
			if ( NULL == pUser ||
				 true == pUser->GetFrameData().stateParam.bInvincible )
				 continue;
		}

		float fTempDistance3Sq = GetDistance3Sq( vPos, pUnitList[i]->GetPos() );			/// NPC를 소환한 유저와 대상의 거리

		if( fAttackRangeSq > fTempDistance3Sq )		/// 공격 범위 안에 있는지 검사
		{
			if( fDistanceSq > fTempDistance3Sq )		/// 지금까지 검사한 거리중 가장 가까운 거리인지 검사
			{
				fDistanceSq = fTempDistance3Sq;		/// 가장 가까운 거리 저장
				pTargetUnit = pUnitList[i];		/// 가장 가까운 유닛 저장
			}
		}
	}
	return pTargetUnit;
}

void CX2GUNPC::AttackResultByType( CX2DamageManager::DamageData &pDamageData )
{
	if( pDamageData.m_fHpPercentUp <= 0.f &&
		pDamageData.m_fIncreaseMP <= 0.f &&
		pDamageData.reActType != CX2DamageManager::RT_CREATE_DAMAGE_EFFECT &&
		pDamageData.reActType != CX2DamageManager::RT_NO_DAMAGE &&
		pDamageData.reActResult != CX2DamageManager::RT_DUMMY_DAMAGE &&
		pDamageData.reActResult != CX2DamageManager::RT_SENSOR &&
		pDamageData.hitType != CX2DamageManager::HT_GROUND_HIT &&
		pDamageData.m_eDamageTrigger != CX2DamageManager::DTT_MARK_OF_COMMANDER &&
		pDamageData.m_bNoDamage == false )
	{
		if( null != pDamageData.optrDefenderGameUnit &&
			pDamageData.optrDefenderGameUnit->GetExtraDamagePack().m_MarkOfCommander.m_fTime > 0.f &&
			pDamageData.optrDefenderGameUnit->GetExtraDamagePack().m_MarkOfCommander.m_fElapsedTimeToEffectDamage >= 1.5f )
		{
			CX2GUUser* pOwnerUnit = static_cast<CX2GUUser*>( GetOwnerGameUnit() );		/// NPC를 소환한 유저
			if( NULL == pOwnerUnit )
				return;

			int iProjectCount = (int) pDamageData.optrDefenderGameUnit->GetExtraDamagePack().m_MarkOfCommander.m_fDamageRate;

			if(iProjectCount > 5)
				iProjectCount = 5;

			D3DXVECTOR3 vConstBasic[] =
			{
				D3DXVECTOR3(	-1000,		1000,	0	),
				D3DXVECTOR3(	-1000+100,	1000,	0	),
				D3DXVECTOR3(	-1000-100,	1000,	0	),
				D3DXVECTOR3(	-1000+200,	1000,	0	),
				D3DXVECTOR3(	-1000-200,	1000,	0	),
			};

			for( int i=0; i< iProjectCount; ++i)
			{
				float fRandReservTime = 0.5;
				if( i > 0)
				{
					fRandReservTime += GetRandomFloat( i ) * 0.3f * i;
					fRandReservTime += GetRandomFloat( pDamageData.optrDefenderGameUnit->GetUnitIndex() ) * 0.3f;
				}
				//float fRandVarDistance = GetRandomFloat( pDamageData.pDefenderUnit->GetUnitIndex() ) * 500.f + i*150.f ;
				//if( i > 0)
				//	fRandVarDistance += GetRandomFloat(i) * 500.f;

				D3DXVECTOR3 vVelocity	= D3DXVECTOR3( 1500.f, -1500.f, 0.f );


				D3DXVECTOR3 vRotateDegree = GetRotateDegree();
				if( i > 0 )
					vRotateDegree.z += (GetRandomFloat( pDamageData.optrDefenderGameUnit->GetUnitIndex() + i) * 10.f - 5.f);


				const float fConstBasicX = vConstBasic[i].x;
				const float fConstBasicY = vConstBasic[i].y;


				const float fConstUnitHeight = 80.f;
				D3DXVECTOR3 vShellStartPos = pDamageData.optrDefenderGameUnit->GetPos();
				vShellStartPos.y += (fConstBasicY + fConstUnitHeight);
				D3DXVECTOR3 vDirectVectorZeroY = GetDirVector();
				vDirectVectorZeroY.y	= 0.f;

				if(GetIsRight() == true)
					vShellStartPos += ( fConstBasicX * vDirectVectorZeroY );
				else
					vShellStartPos -= ( fConstBasicX * vDirectVectorZeroY );

				float fShellPowerRate = pDamageData.damage.GetAllDamage() * pDamageData.m_fDungeonRate;

				if( pOwnerUnit->GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO17 ) == true && GetRandomFloat(i + 3) <= 0.2f )
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_MARK_OF_COMMANDER_SHELL_MEMO", fShellPowerRate, vShellStartPos, vRotateDegree, vRotateDegree,  GetLandPos().y, true, fRandReservTime );
				}
				else
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_MARK_OF_COMMANDER_SHELL", fShellPowerRate, vShellStartPos, vRotateDegree, vRotateDegree,  GetLandPos().y, true, fRandReservTime );
                    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
					    pMeshInstance->SetVelocity( vVelocity );
				}
			}
			pDamageData.optrDefenderGameUnit->AccessExtraDamagePack().m_MarkOfCommander.m_fElapsedTimeToEffectDamage	= 0.f;


		}
	}



	return;
}

D3DXVECTOR3 CX2GUNPC::GetNearestActiveNPCPos()		/// 현재 Active 중인 근처의 NPC의 위치를 반환
{
	D3DXVECTOR3 vPos(0, 0, 0 );
	if( NULL == g_pX2Game )
		return vPos;

	float fNearestDist = 500000.f;
	D3DXVECTOR3 vNPCPos = GetPos();

	for( int i=0; i<g_pX2Game->GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnit(i);
		if( NULL == pNPC )
			continue;

		if( false == pNPC->IsActiveMonster() )		/// 활성화 되지 않은 NPC는 제외
			continue;

		if( CX2UnitManager::NCT_BASIC != pNPC->GetNPCTemplet().m_ClassType )		/// 상자나 문 등은 제외
			continue;

		if( GetTeam() == pNPC->GetTeam() )		///  같은 팀이면 제외
			continue;

		if( true == pNPC->GetIsBosRaidNPC() )
			continue;

		float fTempDist = GetDistance( vNPCPos, pNPC->GetPos() );
		if( fTempDist < fNearestDist )
		{
			fNearestDist = fTempDist;
			vPos = pNPC->GetPos();
		}
	}

	return vPos;
}

CX2GUUser* CX2GUNPC::GetNearestGUUser()
{
	if ( null != m_optrNearestGUUser && m_optrNearestGUUser->GetGameUnitType() == GUT_USER )
		return static_cast<CX2GUUser*>( m_optrNearestGUUser.GetObservable() );
	else
		return NULL;
}

/*virtual*/ CX2GageUI* CX2GUNPC::CreateGageUI( const CX2GageData* pOwnerGageData_ )
{
	switch ( GetNPCTemplet().m_ClassType )
	{
	case CX2UnitManager::NCT_BASIC:
	case CX2UnitManager::NCT_THING_HOUSE:
		return new CX2SmallGageUI( this );
		break;

	default:
		return NULL;
		break;
	}
}

/*inline*/ void CX2GUNPC::AddDamagedMapAndPositionToDiePacket( OUT KEGS_NPC_UNIT_DIE_REQ& kPacket_ )
{
	kPacket_.m_mapDamageByUser	= m_DamagedMap;
	kPacket_.m_DiePos.x = m_NPCFrameData.unitCondition.landPosition.x;
	kPacket_.m_DiePos.y = m_NPCFrameData.unitCondition.landPosition.y;
	kPacket_.m_DiePos.z = m_NPCFrameData.unitCondition.landPosition.z;

}

/*virtual*/ void CX2GUNPC::SetSmallGage()
{
	if ( NULL != g_pX2Room )
	{
		switch ( g_pX2Room->GetRoomType() )
		{
		case CX2Room::RT_PVP:
			{
				CX2GameUnit::SetSmallGage();
			} break;

		case CX2Room::RT_DUNGEON:
		case CX2Room::RT_TRAININGCENTER:
			{
				CX2GameUnit::SetSmallGage();
			} break;

		case CX2Room::RT_BATTLE_FIELD:
			{
				if ( NULL != GetNPCAI() && CX2NPCAI::NAT_FIELD == GetNPCAI()->GetAIType() )
				{
					CX2BattleFieldNpcAi* pBattleNpcAI = static_cast<CX2BattleFieldNpcAi*>( GetNPCAI() );

					if ( pBattleNpcAI->IsAggressive() )
						SetHpTexture( L"Small_HP_bar_Yellow.TGA" );
					else
						SetHpTexture( L"Small_HP_bar_Green.tga");
				}
				else
					CX2GameUnit::SetSmallGage();

			} break;

		default:
			break;
		}
	}
}

void CX2GUNPC::InitFullName()
{
	m_wstrChampionName.clear();
	m_wstrLvAndNameAndPostFix.clear();

	// 챔피온 몬스터 인지
	if( CX2DamageManager::EDT_NONE != m_eWeaponEnchantExtraDamageType )
	{
		wstring enchantName = g_pData->GetDamageManager()->GetExtraDamageName( m_eWeaponEnchantExtraDamageType );

		if ( enchantName.empty() == false )
		{
			m_wstrChampionName += L"[";
			m_wstrChampionName += enchantName;
			m_wstrChampionName += L"]";
		}
	}

	//{{ kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)
//
//	if ( GetRenderLv() == true && GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC )
//	{
//		m_wstrLvAndNameAndPostFix += GET_REPLACED_STRING( ( STR_ID_667, "i", m_HardLevel ) );
//		m_wstrLvAndNameAndPostFix += L" ";
//	}
	//}} kimhc // 2010.5.1 // 비밀던전 작업(비밀던전 몬스터 레벨 표시)


	//접두어로 붙는거 있는지 보고.
	m_wstrLvAndNameAndPostFix += GetExtraAbilityDesc( true );

	//다음에 이름 붙이고.
	if ( m_wstrLvAndNameAndPostFix.empty() == false )
		m_wstrLvAndNameAndPostFix += L" ";

	m_wstrLvAndNameAndPostFix += GetUnitName();

	//다음에 접미어 붙여주는 센스.
	wstring tempEndEx = GetExtraAbilityDesc( false );
	if ( tempEndEx.empty() == false )
	{
		m_wstrLvAndNameAndPostFix += L" ";
		m_wstrLvAndNameAndPostFix += tempEndEx;
	}
}

void CX2GUNPC::RenderPvpNpcServerTexture()
{
	D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( GetPos() );
	int iWidthUnitName = g_pX2Game->GetFontForUnitName()->GetWidth( m_wstrLvAndNameAndPostFix.c_str() );
	int iFinalLeft = (int) finalPos.x + (iWidthUnitName / 2);
	int iFinalTop = (int) finalPos.y;

	if(m_pTextureServer == NULL)
		return;


	const CKTDXDeviceTexture::TEXTURE_UV*	pTexUV = NULL;
	pTexUV = m_pTextureServer->GetTexUV( L"SOLES" );
	if(pTexUV == NULL)
		return;

	float fFinalLeft, fFinalTop, fScaleX, fScaleY, fOffsetX, fOffsetY;

	fOffsetX	= 2.f / g_pKTDXApp->GetResolutionScaleX();
	fOffsetY	= -3.f / g_pKTDXApp->GetResolutionScaleY();
	fScaleX		= 21.f ;// g_pKTDXApp->GetResolutionScaleX();
	fScaleY		= 23.f ;// g_pKTDXApp->GetResolutionScaleY();
	fFinalLeft	= (iFinalLeft) / g_pKTDXApp->GetResolutionScaleX();
	fFinalTop	= (iFinalTop) / g_pKTDXApp->GetResolutionScaleY();

	fFinalLeft += fOffsetX;
	fFinalTop += fOffsetY;

	m_pTextureServer->Draw( fFinalLeft, fFinalTop, fScaleX, fScaleY,
		(int)pTexUV->leftTop.x, (int)pTexUV->leftTop.y, (int)pTexUV->rightBottom.x - (int)pTexUV->leftTop.x, (int)pTexUV->rightBottom.y - (int)pTexUV->leftTop.y );
}

void CX2GUNPC::InitNameColor( const int iMyLevel_ )
{
	const int iLevelDefference = GetUnitLevel() - iMyLevel_;

	if( IsPvpBot() == true )
	{
		if( GetTeam() == CX2Room::TN_BLUE )
			m_colorName = 0xff54ddff;
		else
			m_colorName = 0xffffaa77;
	}
	else
	{
		if ( 8 < iLevelDefference )
		{
			m_colorName = 0xff000000;
			m_colorOutLine = 0x00000000;
		}
		else if ( 2 < iLevelDefference )		// 빨강
		{
			m_colorName = 0xffff1100;
			m_colorOutLine = 0xff995555;
		}
		else if ( -4 < iLevelDefference )		// 주황생
		{
			m_colorName = 0xffff8800;
			m_colorOutLine = 0xff555555;
		}
		else if ( -10 < iLevelDefference )
		{
			m_colorName = 0xff96ff00;		// 길드명 색깔
			m_colorOutLine = 0xff062507;
		}
		else
		{
			m_colorName = 0xffcccccc;	// 회색
			m_colorOutLine = 0xff555555;
		}
	}
}

void CX2GUNPC::CreateAndSetAinmXSkinMesh()
{
// 	if ( NULL != m_pAniXET && CKTDXDevice::DEVICE_STATE_LOADED != m_pAniXET->GetDeviceState() )
// 		return;
//
// 	if ( NULL != m_pMultiTexXET && CKTDXDevice::DEVICE_STATE_LOADED != m_pMultiTexXET->GetDeviceState() )
// 		return;
//
// 	if ( NULL != m_pChangeTexXET && CKTDXDevice::DEVICE_STATE_LOADED != m_pChangeTexXET->GetDeviceState() )
// 		return;

	m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
	m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
	m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, m_pAniXET, m_pMultiTexXET, m_pChangeTexXET );
	m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

	m_pXSkinAnim->GetMatrix().Scale( D3DXVECTOR3( m_vTransScale.x, m_vTransScale.y, m_vTransScale.z ) );

	m_CollisionListSet.insert( &m_pXSkinAnim->GetCollisionDataList() );
	m_AttackListSet.insert( &m_pXSkinAnim->GetAttackDataList() );

	m_pFrame_Bip01				= m_pXSkinAnim->GetCloneFrame( L"Bip01" );

	m_pFrame_Bip01_R_Foot		= m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Foot" );
	m_pFrame_Bip01_L_Foot		= m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Foot" );
	m_pFrame_Bip01_R_UpperArm	= m_pXSkinAnim->GetCloneFrame( L"Bip01_R_UpperArm" );
	m_pFrame_Bip01_L_UpperArm	= m_pXSkinAnim->GetCloneFrame( L"Bip01_L_UpperArm" );

	m_pFrame_Bip01_Head			= m_pXSkinAnim->GetCloneFrame( m_HeadBoneName.c_str() ); // HEAD_BONE_NAME을 여기에서 읽어오므로init_motion에
    m_pFrame_Bip01_Head_Real    = m_pXSkinAnim->GetCloneFrame(  L"Bip01_Head" );

	if(m_bNotCull)
	{
		m_pXSkinAnim->SetNotCull();
	}
}

/** @function : GetBuffInfo
	@brief : NPC의 버프 정보를 vector에 담는 함수
	@param : NPC 버프의 정보를 담을 벡터
*/
void CX2GUNPC::GetBuffInfo( OUT vector<KNpcUnitBuffInfo>& vecNpcUnitBuffInfo_ ) const
{
	if ( !m_vecBuffTempletPtr.empty() )
	{
		vecNpcUnitBuffInfo_.push_back( KNpcUnitBuffInfo() );
		KNpcUnitBuffInfo& kNpcUnitBuffInfo = vecNpcUnitBuffInfo_.back();
		kNpcUnitBuffInfo.m_iNpcUID = GetUID();

		GetBuffFactor( kNpcUnitBuffInfo.m_vecBuffFactor );
	}
}

/** @function : HyperModeBuffEffectStart
	@brief : 각성 시 나와야하는 이펙트들 시작
*/
/*virtual*/ void CX2GUNPC::HyperModeBuffEffectStart()
{
	SetHyperModeCount( 0 );

	if ( m_wstrHyperBoostRightParticleName.empty() == false )
	{
		if ( m_hHyperBoostRFoot == INVALID_PARTICLE_SEQUENCE_HANDLE )
			m_hHyperBoostRFoot	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  m_wstrHyperBoostRightParticleName.c_str(),	0,0,0, 0, 0 );

		if ( m_hHyperBoostRArm == INVALID_PARTICLE_SEQUENCE_HANDLE )
			m_hHyperBoostRArm	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  m_wstrHyperBoostRightParticleName.c_str(),	0,0,0, 0, 0 );
	}

	if ( m_wstrHyperBoostLeftParticleName.empty() == false )
	{
		if ( m_hHyperBoostLFoot == INVALID_PARTICLE_SEQUENCE_HANDLE )
			m_hHyperBoostLFoot	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  m_wstrHyperBoostLeftParticleName.c_str(),		0,0,0, 0, 0 );

		if ( m_hHyperBoostLArm == INVALID_PARTICLE_SEQUENCE_HANDLE )
			m_hHyperBoostLArm	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  m_wstrHyperBoostLeftParticleName.c_str(),		0,0,0, 0, 0 );
	}
}

/** @function : CreateAndInsertRenderParamByBuffUnitType
	@brief : 유닛의 타입별로 버프에 의한 RenderParam 변경을 적용하는 함수
	@param : 버프종류구분(BuffIdentity_), 렌더파람정보 map 컨테이터(mapStBuffRenderParamPtr_)
*/
/*virtual*/ void CX2GUNPC::CreateAndInsertRenderParamByBuffUnitType( const KBuffIdentity& BuffIdentity_, const map<CX2Unit::UNIT_TYPE, StBuffRenderParamPtr>& mapStBuffRenderParamPtr_ )
{
	/// 공통으로 사용하는 렌더파람 찾기
	map<CX2Unit::UNIT_TYPE, StBuffRenderParamPtr>::const_iterator mItrCommonType
		= mapStBuffRenderParamPtr_.find( CX2Unit::UT_NONE );

	if ( mapStBuffRenderParamPtr_.end() != mItrCommonType && NULL != mItrCommonType->second)
		CreateAndInsertRenderParamByBuff( BuffIdentity_, *(mItrCommonType->second) );
}

/** @function : CanApplyBuffToGameUnit
	@brief : 버프에 걸릴수 있는 상태인지 알아내는 함수로 BASIC 타입이 아니면 false 리턴
	@return : 걸릴수 있으면 true, 없으면 false
*/
/*virtual*/ bool CX2GUNPC::CanApplyBuffToGameUnit() const
{
	if ( GetClassType() == CX2UnitManager::NCT_BASIC )
	{
		return CX2GameUnit::CanApplyBuffToGameUnit();
	}
	else
		return false;	/// NCT_BASIC 이 아니면 false 리턴
}

/** @function : IsSuperArmor
	@brief : 현재 수퍼아머인가?
	@return : 수퍼아머상태(true), 아님(false)
*/
/*virtual*/ bool CX2GUNPC::IsSuperArmor() const
{
	if ( !m_vecSuperArmorPtr.empty() )	/// 버프에 의해 적용된 수퍼아머가 있으면
		return true;	/// 수퍼아머
	else	/// 버프에 의한 수퍼아머가 없으면
		return m_NPCFrameData.stateParam.bSuperArmor;
}

/*virtual*/ void CX2GUNPC::InitAddtionalDashAndJumpBasicStatValue()
{
	/// NPC는 스피드로 스크립트의 INIT_PHYSIC 테이블 값을 사용하지 않고
	/// 대부분 x 값은 PassiveSpeed, y값은 Speed를 사용 하므로
	/// 실제 값을 셋팅하기보다 배율을 셋팅하기 위해
	/// PhysicParam의 값이 아닌 1.0으로 셋팅하였음
	m_AdditionalWalkSpeed.SetBasicStatValue( 1.0f );
	m_AdditionalDashSpeed.SetBasicStatValue( 1.0f );
	m_AdditionalJumpPower.SetBasicStatValue( 1.0f );
	m_AdditionalDashJumpPower.SetBasicStatValue( 1.0f );
}

/** @function : GetShowOnMiniMap
	@brief : 미니맵 상에서 보이는지 여부를 조사하는 함수
	@return : 보이면 true, 안보이면 false
*/
/*virtual*/ bool CX2GUNPC::GetShowOnMiniMap() const
{
	if ( IsCulled() == false &&
		GetNowHp() > 0.0f &&
		GetGameUnitState() != CX2GameUnit::GUSI_DIE &&
		GetShowObject() &&
		m_bShowOnMiniMap )
		return true;
	else
		return false;
}






/*FORCEINLINE*/ int CX2GUNPC::ProcessCanPushNPCUnit( CX2GUNPC* pGameUnit_, const D3DXVECTOR2& vMyUnitMinMaxY_, const D3DXVECTOR2& vMyUnitMinMaxYCur_,
    OUT D3DXVECTOR3& vMyPos_ )
{
	KTDXPROFILE_BEGIN("Push");

	D3DXVECTOR3 vPosFutureOtherGameUnit = pGameUnit_->GetPos( true );
	const float fGameUnitYMin = vPosFutureOtherGameUnit.y;
	const float fGameUnitYMax = fGameUnitYMin + pGameUnit_->GetUnitHeight( true );

	if( (fGameUnitYMin >= vMyUnitMinMaxY_.x && fGameUnitYMin <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMax >= vMyUnitMinMaxY_.x && fGameUnitYMax <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMin <= vMyUnitMinMaxY_.x && fGameUnitYMax >= vMyUnitMinMaxY_.y) )
    {
        D3DXVECTOR3 vPosNowOtherGameUnit = pGameUnit_->GetPositionBackup();
		vPosFutureOtherGameUnit.y = vMyPos_.y;
		const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
     	float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;

		if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// 서로 근접하여 붙어있으면
		{
            float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
            float fMarginMax = FLT_MAX;

			//{{ dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리
			// 특히 캐릭터 혹은 몬스터의 크기가 클수록, 속도가 빠를수록 서로 너무 멀리 밀리게 되서 연타공격이 안되는 문제가 있음
			// 이름 보정해주기 위한 예외처리
			if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
				pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
			{
				const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
				fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
				if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
					fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
			}
			//}} dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리

            D3DXVECTOR3 vTestPos;
            D3DXVECTOR3 vDirVectorFromMeToOther;
            const float fGameUnitYMinCur = vPosNowOtherGameUnit.y;
            const float fGameUnitYMaxCur = fGameUnitYMinCur + pGameUnit_->GetUnitHeight( false );
	        if( (fGameUnitYMinCur >= vMyUnitMinMaxYCur_.x && fGameUnitYMinCur <= vMyUnitMinMaxYCur_.y) ||
		        (fGameUnitYMaxCur >= vMyUnitMinMaxYCur_.x && fGameUnitYMaxCur <= vMyUnitMinMaxYCur_.y) ||
		        (fGameUnitYMinCur <= vMyUnitMinMaxYCur_.x && fGameUnitYMaxCur >= vMyUnitMinMaxYCur_.y) )
            {
                vDirVectorFromMeToOther = vPosNowOtherGameUnit - m_vPositionBackup;
                vDirVectorFromMeToOther.y = 0.f;
			    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
			    {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                else
                {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                vMyPos_ = vTestPos;
                return 1;
            }
            else
            {
                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                vDirVectorFromMeToOther.y = 0.f;
                float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                {
                    vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( ProcessCanPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return 1;
                    }
                }
                else
                {
                    vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( ProcessCanPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return 1;
                    }
                }
            }
            vMyPos_ = m_vPositionBackup;
            return ( 2 | 0x8 );
		} // if
    }

	KTDXPROFILE_END();

    return 0;
}

/*FORCEINLINE*/ int CX2GUNPC::ProcessCanPushUserUnit( CX2GUUser* pGameUnit_, const D3DXVECTOR2& vMyUnitMinMaxY_,
    OUT D3DXVECTOR3& vMyPos_, int iDirection, int iLocalDirection, bool bReservePosReliable )
{
	KTDXPROFILE_BEGIN("Push");

    const StateData&    nowStateData = GetNowStateData();

	D3DXVECTOR3 vPosFutureOtherGameUnit = pGameUnit_->GetPos( true );
	const float fGameUnitYMin = vPosFutureOtherGameUnit.y;
	const float fGameUnitYMax = fGameUnitYMin + pGameUnit_->GetUnitHeight( true );

    int iReliableDirection = iDirection;
    if ( iReliableDirection != 1 && iReliableDirection != 2 )
        iReliableDirection = iLocalDirection;

	if( (fGameUnitYMin >= vMyUnitMinMaxY_.x && fGameUnitYMin <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMax >= vMyUnitMinMaxY_.x && fGameUnitYMax <= vMyUnitMinMaxY_.y) ||
		(fGameUnitYMin <= vMyUnitMinMaxY_.x && fGameUnitYMax >= vMyUnitMinMaxY_.y) )
    {
        if ( iReliableDirection == 1 || iReliableDirection == 2 )
        {
	        // 높이가 겹칠수 있는 높이인지 먼저 테스트
	        // vMyUnitMinMaxY_에서 x는 Min 값, y는 Max 값
		    vPosFutureOtherGameUnit.y = vMyPos_.y;
		    const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
     	    float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;

		    if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// 서로 근접하여 붙어있으면
		    {
                float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
                float fMarginMax = FLT_MAX;

			    //{{ dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리
			    // 특히 캐릭터 혹은 몬스터의 크기가 클수록, 속도가 빠를수록 서로 너무 멀리 밀리게 되서 연타공격이 안되는 문제가 있음
			    // 이름 보정해주기 위한 예외처리
			    if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
				    pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
			    {
				    const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
				    fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
				    if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
					    fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
			    }
			    //}} dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리

                D3DXVECTOR3 vTestPos;
                D3DXVECTOR3 vDirVectorFromMeToOther;
			    if ( iReliableDirection == 2 )
			    {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                else
                {
                    vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                    vDirVectorFromMeToOther.y = 0.f;
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                        vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                    else
                    {
                        float fHalfSumMargin = __min( fHalfSumUnitWidth, fMarginMax ) + fDistanceHorizon;
                        vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                        vTestPos.y = vMyPos_.y;
                        vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    }
                }
                vMyPos_ = vTestPos;
                return 1;
		    }
            else
            {
                D3DXVECTOR3 vMoveDir;
                D3DXVECTOR3 vDirVectorFromMeToOther;
                D3DXVECTOR3 vDirVectorFromAltToOther;
                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                vDirVectorFromMeToOther.y = 0.f;

                if ( iReliableDirection == 2 )
                {
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                        return 1;
                    float   fUnitHeight = GetUnitHeight( true );
                    float   fProjObjDistanceSq = D3DXVec3LengthSq( &vDirVectorFromMeToOther );
                    if ( vMyPos_ != m_NPCFrameData.syncData.position )
                    {
                        vDirVectorFromAltToOther = vPosFutureOtherGameUnit - m_NPCFrameData.syncData.position;
                        vDirVectorFromAltToOther.y = 0.f;
                        if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromAltToOther ) >= 0
                            && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                        {
	                        const float fProjMoveDistance3Sq
		                        = GetDistance3Sq( D3DXVECTOR3( m_NPCFrameData.syncData.position.x, vMyPos_.y, m_NPCFrameData.syncData.position.z ), vMyPos_ );
                            if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                            {
                                vMyPos_ = m_NPCFrameData.syncData.position;
                                return 5;
                            }
                        }
                    }
                    if ( iLocalDirection == iReliableDirection && vMyPos_ != m_vPositionBackup && m_NPCFrameData.syncData.position != m_vPositionBackup )
                    {
                        vDirVectorFromAltToOther = vPosFutureOtherGameUnit - m_vPositionBackup;
                        vDirVectorFromAltToOther.y = 0.f;
                        if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromAltToOther ) >= 0
                            && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                        {
	                        const float fProjMoveDistance3Sq
		                        = GetDistance3Sq( D3DXVECTOR3( m_vPositionBackup.x, vMyPos_.y, m_vPositionBackup.z ), vMyPos_ );
                            if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                            {
                                vMyPos_ = m_vPositionBackup;
                                return 2;
                            }
                        }
                    }
                    if ( bReservePosReliable == true && m_kNonHostReaction.IsReceivePosReserved() == true &&
                        (
                        NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == true ||
                        m_NPCFrameData.unitCondition.bFootOnLine == m_kNonHostReaction.GetReservedReceiveFootOnLine() ) )
                    {
                        const D3DXVECTOR3&  vReservedReceivePos = m_kNonHostReaction.GetReservedReceivePos();
                        if ( vMyPos_ != vReservedReceivePos && m_NPCFrameData.syncData.position != vReservedReceivePos
                            && ( iLocalDirection != iReliableDirection || m_vPositionBackup != vReservedReceivePos ) )
                        {
                            vDirVectorFromAltToOther = vPosFutureOtherGameUnit - vReservedReceivePos;
                            vDirVectorFromAltToOther.y = 0.f;
                            if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0
                                && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                            {
	                            const float fProjMoveDistance3Sq
		                            = GetDistance3Sq( D3DXVECTOR3( vReservedReceivePos.x, vMyPos_.y, vReservedReceivePos.z ), vMyPos_ );
                                if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                                {
                                    vMyPos_ = vReservedReceivePos;
                                    return 3;
                                }
                            }
                        }
                        if ( pGameUnit_->IsMyUnit() == true
                            && NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false
                            && m_kNonHostReaction.GetReservedReceiveFootOnLine() == true
                            && ( GetRideState() != RS_ON_RIDING || GetRideType() != RT_RIDER )
                            )
                        {
		                    const CX2GUUser::FrameData& kFrameData = pGameUnit_->GetFrameData( true );
                            //ASSERT( pFrameData != NULL );
                            if ( kFrameData.unitCondition.bFootOnLine == true )
                            {
	                            CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
                                ASSERT( pLineMap != NULL );
                                if ( pLineMap->GetLineGroupIncludesLineData( m_kNonHostReaction.GetReservedReceiveLineIndex() )
                                    == pLineMap->GetLineGroupIncludesLineData( kFrameData.syncData.lastTouchLineIndex ) )
                                {
                                    m_NPCFrameData.syncData.lastTouchLineIndex = kFrameData.syncData.lastTouchLineIndex;
                                    m_NPCFrameData.unitCondition.dirVector = kFrameData.unitCondition.dirVector;
                                    vMyPos_ = kFrameData.syncData.position;
                                    vMyPos_.x -= m_NPCFrameData.unitCondition.dirVector.x * fHalfSumUnitWidth;
                                    vMyPos_.z -= m_NPCFrameData.unitCondition.dirVector.z * fHalfSumUnitWidth;
                                    return 4 | 0x8;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) <= 0 )
                        return 1;
                    float   fUnitHeight = GetUnitHeight( true );
                    float   fProjObjDistanceSq = D3DXVec3LengthSq( &vDirVectorFromMeToOther );
                    if ( vMyPos_ != m_NPCFrameData.syncData.position )
                    {
                        vDirVectorFromAltToOther = vPosFutureOtherGameUnit - m_NPCFrameData.syncData.position;
                        vDirVectorFromAltToOther.y = 0.f;
                        if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromAltToOther ) <= 0
                            && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                        {
	                        const float fProjMoveDistance3Sq
		                        = GetDistance3Sq( D3DXVECTOR3( m_NPCFrameData.syncData.position.x, vMyPos_.y, m_NPCFrameData.syncData.position.z ), vMyPos_ );
                            if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                            {
                                vMyPos_ = m_NPCFrameData.syncData.position;
                                return 5;
                            }
                        }
                    }
                    if ( iLocalDirection == iReliableDirection && vMyPos_ != m_vPositionBackup && m_NPCFrameData.syncData.position != m_vPositionBackup )
                    {
                        vDirVectorFromAltToOther = vPosFutureOtherGameUnit - m_vPositionBackup;
                        vDirVectorFromAltToOther.y = 0.f;
                        if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromAltToOther ) <= 0
                            && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                        {
	                        const float fProjMoveDistance3Sq
		                        = GetDistance3Sq( D3DXVECTOR3( m_vPositionBackup.x, vMyPos_.y, m_vPositionBackup.z ), vMyPos_ );
                            if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                            {
                                vMyPos_ = m_vPositionBackup;
                                return 2;
                            }
                        }
                    }
                    if ( bReservePosReliable == true && m_kNonHostReaction.IsReceivePosReserved() == true &&
                        (
                            NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == true ||
                            m_NPCFrameData.unitCondition.bFootOnLine == m_kNonHostReaction.GetReservedReceiveFootOnLine() ) )
                    {
                        const D3DXVECTOR3&  vReservedReceivePos = m_kNonHostReaction.GetReservedReceivePos();
                        if ( vMyPos_ != vReservedReceivePos && m_NPCFrameData.syncData.position != vReservedReceivePos
                            && ( iLocalDirection != iReliableDirection || m_vPositionBackup != vReservedReceivePos )  )
                        {
                            vDirVectorFromAltToOther = vPosFutureOtherGameUnit - vReservedReceivePos;
                            vDirVectorFromAltToOther.y = 0.f;
                            if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromAltToOther ) <= 0
                            && D3DXVec3Dot( &vDirVectorFromMeToOther, &vDirVectorFromAltToOther ) <= 0 )
                            {
	                            const float fProjMoveDistance3Sq
		                            = GetDistance3Sq( D3DXVECTOR3( vReservedReceivePos.x, vMyPos_.y, vReservedReceivePos.z ), vMyPos_ );
                                if ( fProjMoveDistance3Sq >= fProjObjDistanceSq )
                                {
                                    vMyPos_ = vReservedReceivePos;
                                    return 3;
                                }
                            }
                        }
                        if ( pGameUnit_->IsMyUnit() == true
                            && NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == false
                            && m_kNonHostReaction.GetReservedReceiveFootOnLine() == true
                            && ( GetRideState() != RS_ON_RIDING || GetRideType() != RT_RIDER )
                            )
                        {
		                    const CX2GUUser::FrameData& kFrameData = pGameUnit_->GetFrameData( true );
                            //ASSERT( pFrameData != NULL );
                            if ( kFrameData.unitCondition.bFootOnLine == true )
                            {
	                            CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
                                ASSERT( pLineMap != NULL );
                                if ( pLineMap->GetLineGroupIncludesLineData( m_kNonHostReaction.GetReservedReceiveLineIndex() )
                                    == pLineMap->GetLineGroupIncludesLineData( kFrameData.syncData.lastTouchLineIndex ) )
                                {
                                    m_NPCFrameData.syncData.lastTouchLineIndex = kFrameData.syncData.lastTouchLineIndex;
                                    m_NPCFrameData.unitCondition.dirVector = kFrameData.unitCondition.dirVector;
                                    vMyPos_ = kFrameData.syncData.position;
                                    vMyPos_.x += m_NPCFrameData.unitCondition.dirVector.x * fHalfSumUnitWidth;
                                    vMyPos_.z += m_NPCFrameData.unitCondition.dirVector.z * fHalfSumUnitWidth;
                                    return 4 | 0x8;
                                }
                            }
                        }
                    }
                }
                if ( iLocalDirection == iReliableDirection && ProcessCanPassUnit( pGameUnit_, vMyPos_ ) == true )
                    return 1 | 0x8;
                return  1;
            }
        }
        else
        {
		    vPosFutureOtherGameUnit.y = vMyPos_.y;
		    const float fDistanceHorizon3Sq = GetDistance3Sq( vPosFutureOtherGameUnit, vMyPos_ );
     	    float fHalfSumUnitWidth = ( GetUnitWidth( true ) + pGameUnit_->GetUnitWidth( true ) ) * 0.5f;

		    if( fDistanceHorizon3Sq < fHalfSumUnitWidth * fHalfSumUnitWidth )	/// 서로 근접하여 붙어있으면
		    {
                float fDistanceHorizon =  sqrt( fDistanceHorizon3Sq );
                float fMarginMax = FLT_MAX;

			    //{{ dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리
			    // 특히 캐릭터 혹은 몬스터의 크기가 클수록, 속도가 빠를수록 서로 너무 멀리 밀리게 되서 연타공격이 안되는 문제가 있음
			    // 이름 보정해주기 위한 예외처리
			    if ( pGameUnit_->GetPhysicParam().nowSpeed.x > MAGIC_SPEED_BOUND ||
				    pGameUnit_->GetPhysicParam().passiveSpeed.x > MAGIC_SPEED_BOUND )
			    {
				    const float fUnitWidthScale = GetUnitWidth( true ) / MAGIC_DEFAULT_UNIT_WIDTH;
				    fMarginMax = MAGIC_DEFALUT_UNIT_DISPLACE * fUnitWidthScale;
				    if( fMarginMax > MAGIC_MAX_UNIT_DISPLACE )
					    fMarginMax = MAGIC_MAX_UNIT_DISPLACE;
			    }
			    //}} dmlee 2008.11.6 레이븐 파워어썰트처럼 속도가 빠르고 밀고 나가는 공격인 경우에 예외처리

                D3DXVECTOR3 vTestPos;
                D3DXVECTOR3 vDirVectorFromMeToOther;
                vDirVectorFromMeToOther = vPosFutureOtherGameUnit - vMyPos_;
                vDirVectorFromMeToOther.y = 0.f;
                float fHalfSumMargin = __min( fHalfSumUnitWidth - fDistanceHorizon, fMarginMax );
                if ( D3DXVec3Dot( &m_NPCFrameData.unitCondition.dirVector, &vDirVectorFromMeToOther ) >= 0 )
                {
                    vTestPos.x = vMyPos_.x - m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z - m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( iLocalDirection == 0 || ProcessCanPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return  1;
                    }
                }
                else
                {
                    vTestPos.x = vMyPos_.x + m_NPCFrameData.unitCondition.dirVector.x * fHalfSumMargin;
                    vTestPos.y = vMyPos_.y;
                    vTestPos.z = vMyPos_.z + m_NPCFrameData.unitCondition.dirVector.z * fHalfSumMargin;
                    if ( iLocalDirection == 0 || ProcessCanPassUnit( pGameUnit_, vTestPos ) == false )
                    {
                        vMyPos_ = vTestPos;
                        return  1;
                    }
                }
                vMyPos_ = m_vPositionBackup;
                return 2 & 0x8;
		    }
            else
            {
                if ( iLocalDirection == 0 )
                    return 1;
                return 0;
            }//if.. else..
        }
    }
    else
    {
        if ( iLocalDirection == 0 )
        {
            return 1;
        }//if

        if ( fGameUnitYMin >= vMyUnitMinMaxY_.y )
        {
            if ( iLocalDirection == 3 )
                return 1;
        }
        else
        {
            if ( iLocalDirection == 4 )
                return 1;
        }

        if ( ProcessCanPassUnit( pGameUnit_, vMyPos_ ) == false )
            return  1;

        if ( bReservePosReliable == true && m_kNonHostReaction.IsReceivePosReserved() == true &&
            (
              NOWSTATE_FLAG( NOT_USE_LAND_CONNECT ) == true
                || m_NPCFrameData.unitCondition.bFootOnLine == m_kNonHostReaction.GetReservedReceiveFootOnLine() )
            && vMyPos_ != m_kNonHostReaction.GetReservedReceivePos() )
        {
            vMyPos_ = m_kNonHostReaction.GetReservedReceivePos();
            return 3 | 0x8;
        }

        vMyPos_ = m_vPositionBackup;
        return 2 & 0x8;
    }

	KTDXPROFILE_END();

    return 0;
}

bool    CX2GUNPC::ProcessCanPassUnit( const CX2GameUnit* pGameUnit_, const D3DXVECTOR3& vModifiedPos )
{
	if( vModifiedPos == m_vPositionBackup )
        return false;

    D3DXVECTOR3 vPosFuture = pGameUnit_->GetPos( true );
    D3DXVECTOR3 vPosBackup = const_cast<CX2GameUnit*>(pGameUnit_)->GetPositionBackup();

	D3DXVECTOR3 vDirFromPosFutureToUnit		= vPosFuture - vModifiedPos;
	vDirFromPosFutureToUnit.y = 0.f;

    D3DXVECTOR3 vDirFromOrgPosToUnit	= vPosBackup - m_vPositionBackup;
	vDirFromOrgPosToUnit.y = 0.f;

    if ( D3DXVec3Dot( &vDirFromPosFutureToUnit, &vDirFromOrgPosToUnit ) > 0 )
        return false;

    vDirFromOrgPosToUnit	= vPosFuture - m_vPositionBackup;
	vDirFromOrgPosToUnit.y = 0.f;

    if ( D3DXVec3Dot( &vDirFromPosFutureToUnit, &vDirFromOrgPosToUnit ) < 0 )
    {
	    const float fProjObjDistance3Sq
		    = GetDistance3Sq( D3DXVECTOR3( vPosFuture.x, m_vPositionBackup.y, vPosFuture.z ), m_vPositionBackup );

	    const float fProjMoveDistance3Sq
		    = GetDistance3Sq( D3DXVECTOR3( vModifiedPos.x, m_vPositionBackup.y, vModifiedPos.z ), m_vPositionBackup );

        if ( fProjMoveDistance3Sq >= fProjObjDistance3Sq )
        {
	        D3DXVECTOR3 moveDir = vModifiedPos - m_vPositionBackup;
	        D3DXVec3Normalize( &moveDir, &moveDir );

	        const float fCollisionPointFirstY = m_vPositionBackup.y + moveDir.y * sqrt( fProjObjDistance3Sq );
	        const float fCollisionPointSecondY = fCollisionPointFirstY + GetUnitHeight( true );

	        const float fGameUnitYMin = vPosFuture.y;
	        const float fGameUnitYMax = vPosFuture.y + pGameUnit_->GetUnitHeight( true );

	        if( ( fCollisionPointFirstY >= fGameUnitYMin && fCollisionPointFirstY <= fGameUnitYMax ) ||
		        ( fCollisionPointSecondY >= fGameUnitYMin && fCollisionPointSecondY <= fGameUnitYMax ) )
                //|| ( fCollisionPointFirstY <= fGameUnitYMin && fCollisionPointSecondY >= fGameUnitYMax ) )
	        {
                return true;
	        }
        }
    }

    return false;
}








/* virtual */ float CX2GUNPC::GetAdditionalAttackDamage ( const CX2DamageManager::DamageData* pAttackDamageData )
{
	if ( NULL != GetOwnerGameUnit() )					// 추가 대미지에 영향을 받는 객체는 소환물 밖에 없다.
	{
		return CalcAdditionalAttackDamage ( pAttackDamageData );
	}
	return 0.0f;
}












/*static*/ void CX2GUNPC::ParseStopTime( StateData& nowStateData, KLuaManager& luaManager_ )
{
	int index = 0;
	nowStateData.m_vecStopAllList.resize(0);
	while( luaManager_.BeginTable( "STOP_ALL_UNIT", index ) == true )
	{
		D3DXVECTOR3 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		LUA_GET_VALUE( luaManager_, 3, stopTime.z, -1.0f );
		nowStateData.m_vecStopAllList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}

	index = 0;
	nowStateData.m_vecStop2AllList.resize(0);
	while( luaManager_.BeginTable( "STOP2_ALL_UNIT", index ) == true )
	{
		D3DXVECTOR3 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		LUA_GET_VALUE( luaManager_, 3, stopTime.z, -1.0f );
		nowStateData.m_vecStop2AllList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}

	index = 0;
	nowStateData.m_vecStopOtherList.resize(0);
	while( luaManager_.BeginTable( "STOP_OTHER_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		nowStateData.m_vecStopOtherList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}

	index = 0;
	nowStateData.m_vecStop2OtherList.resize(0);
	while( luaManager_.BeginTable( "STOP2_OTHER_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		nowStateData.m_vecStop2OtherList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}

	index = 0;
	nowStateData.m_vecStopMyList.resize(0);
	while( luaManager_.BeginTable( "STOP_MY_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		nowStateData.m_vecStopMyList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}

	index = 0;
	nowStateData.m_vecStop2MyList.resize(0);
	while( luaManager_.BeginTable( "STOP2_MY_UNIT", index ) == true )
	{
		D3DXVECTOR2 stopTime;
		LUA_GET_VALUE( luaManager_, 1, stopTime.x, -1.0f );
		LUA_GET_VALUE( luaManager_, 2, stopTime.y, -1.0f );
		nowStateData.m_vecStop2MyList.push_back( stopTime );

		index++;
		luaManager_.EndTable();
	}
}


/*virtual*/void CX2GUNPC::DamageDataChangeProcess()
{
    const StateData& nowStateData = GetNowStateData();
    float   fCommonDamageChangeTime = nowStateData.m_fCommonDamageChangeTime;

	if( -1.f == fCommonDamageChangeTime )
		return;

	if( true == AnimEventTimer_LUA( fCommonDamageChangeTime ) )
	{
		ClearHitUnitList_LUA();
		SetDamageData_LUA( "DAMAGE_DATA_NEXT" );
	}
}
/** @function : ParsingBuffFactorID
	@brief : 스크립트에서 지정한 스테이트의, 지정한 시간에
		     자기자신에게 적용하는 버프 팩터 정보 파싱
*/


/*static*/ void CX2GUNPC::ParsingBuffFactorID( StateData& nowStateData, KLuaManager& luaManager_ )
{
	// BuffFactorID, 적용 시간
    std::vector<StateInsertBuffFactor>& vecStateInsertBuffFactor = nowStateData.m_vecStateInsertBuffFactor;
    vecStateInsertBuffFactor.resize( 0 );
	if( true == luaManager_.BeginTable( "BUFF_FACTOR_ID_AND_APPLY_TIME" ) )
	{
		int iTableIndex = 1;
        nowStateData.m_vecStateInsertBuffFactor.reserve( luaManager_.GetNumIndexedTables() );
		while( true == luaManager_.BeginTable( iTableIndex++  ) )
		{
			UINT uiBuffFactorID = 0;
			float fStartAnimationTime = 0.f;

			LUA_GET_VALUE( luaManager_, 1, uiBuffFactorID, 0);

			LUA_GET_VALUE( luaManager_, 2, fStartAnimationTime, 0.f);

			luaManager_.EndTable();

			if( 0 == uiBuffFactorID )
				break;

            if ( fStartAnimationTime < 0.f )
                continue;

#if X2_DEV_TOOLS
            for( unsigned u = 0; u < vecStateInsertBuffFactor.size(); ++u )
            {
                if ( vecStateInsertBuffFactor[ u ].m_uiBuffFactorID == uiBuffFactorID )
			    {
				    DISPLAY_ERROR(L"BuffFactor Duplication Error!");
			    }
            }
#endif //_IN_HOUSE_
            vecStateInsertBuffFactor.push_back( StateInsertBuffFactor( uiBuffFactorID, fStartAnimationTime ) );
		}
		luaManager_.EndTable(); //BUFF_FACTOR_ID
        std::stable_sort( vecStateInsertBuffFactor.begin(), vecStateInsertBuffFactor.end() );
	}
}

/** @function : CommonFrameMove_InsertBuffFactor
	@brief : 스크립트에서 지정한 스테이트의, 지정한 시간에
			자기자신에게 적용하는 버프 팩터 설정
*/
void CX2GUNPC::CommonFrameMove_InsertBuffFactor( float fNowAnimationTime )
{
    if ( m_iStateInsertBuffFactorProgress < 0 )
        return;

    const std::vector<StateInsertBuffFactor>& vecStateInsertBuffFactor = GetNowStateData().m_vecStateInsertBuffFactor;

    for( ; m_iStateInsertBuffFactorProgress < (int) vecStateInsertBuffFactor.size(); ++m_iStateInsertBuffFactorProgress )
	{
        const StateInsertBuffFactor& factor = vecStateInsertBuffFactor[ m_iStateInsertBuffFactorProgress ];
		// 애니메이션 시간이 지정한 시간 이후라면
		if( fNowAnimationTime >= factor.m_fStartAnimationTime )
		{
			SetBuffFactorToGameUnitByBuffFactorID( factor.m_uiBuffFactorID );
		}
		else
		{
            break;
		}
	}
}




void CX2GUNPC::ApplyWorldLightColor( OUT CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if( m_bUnitColor == false && g_pX2Game->GetWorld() != NULL && m_bUseWorldColor == true )
	{
		pRenderParam_->color.r	= g_pX2Game->GetWorld()->GetLightColor().r;
		pRenderParam_->color.g	= g_pX2Game->GetWorld()->GetLightColor().g;
		pRenderParam_->color.b	= g_pX2Game->GetWorld()->GetLightColor().b;
	}
}
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

void CX2GUNPC::_EncodeNpcSyncPos( USHORT& usPosX, USHORT& usPosY, USHORT& usPosZ, const D3DXVECTOR3& position, unsigned char lastTouchLineIndex )
{
    const CKTDGLineMap::LineData* pLineData = NULL;
	if( !g_pX2Game || !g_pX2Game->GetLineMap()
        || ( pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )lastTouchLineIndex ) ) == NULL )
    {
		StateLog( L"lastTouchLineIndex가 이상한듯" );
    	usPosX = floatToHalf( position.x );
	    usPosY = floatToHalf( position.y );
	    usPosZ = floatToHalf( position.z );
		return;
    }

	usPosX = floatToHalf( position.x - pLineData->startPos.x );
	usPosY = floatToHalf( position.y - pLineData->startPos.y );
	usPosZ = floatToHalf( position.z - pLineData->startPos.z );

}

/*static*/
void CX2GUNPC::NPCSyncData::_DecodeNpcSyncPos( D3DXVECTOR3& vPosition, USHORT usPosX, USHORT usPosY, USHORT usPosZ, unsigned char ucLastTouchLineIndex )
{
    const CKTDGLineMap::LineData* pLineData = NULL;
	if( !g_pX2Game || !g_pX2Game->GetLineMap()
        || ( pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )ucLastTouchLineIndex ) ) == NULL )
    {
		StateLog( L"라인 포인터 이상." );
	    vPosition.x = halfToFloat( usPosX );
	    vPosition.y = halfToFloat( usPosY );
	    vPosition.z = halfToFloat( usPosZ );
		return;
    }

	vPosition.x = pLineData->startPos.x + halfToFloat( usPosX );
	vPosition.y = pLineData->startPos.y + halfToFloat( usPosY );
	vPosition.z = pLineData->startPos.z + halfToFloat( usPosZ );
}

/*static*/
float CX2GUNPC::NPCSyncData::_DecodeNpcSyncPosY( USHORT usPosY, unsigned char ucLastTouchLineIndex )
{
    const CKTDGLineMap::LineData* pLineData = NULL;
	if( !g_pX2Game || !g_pX2Game->GetLineMap()
        || ( pLineData = g_pX2Game->GetLineMap()->GetLineData( ( int )ucLastTouchLineIndex ) ) == NULL )
    {
		StateLog( L"라인 포인터 이상." );
	    return halfToFloat( usPosY );
    }

	return pLineData->startPos.y + halfToFloat( usPosY );
}

//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK






CX2GUUser* CX2GUNPC::GetOwnerGameUnit()
{
    return  ( null != m_optrOwnerGameUnit ) ? static_cast<CX2GUUser*>(m_optrOwnerGameUnit.GetObservable()) : NULL;
}

void    CX2GUNPC::SetOwnerGameUnit( CX2GUUser* pOwnerGameUnit_ )
{
    if ( pOwnerGameUnit_ != NULL )
        m_optrOwnerGameUnit = pOwnerGameUnit_;
}



/** @function	: SetEntryMonsterMessage
	@brief		: 몬스터 등장시 화면 중앙에 출력하는 메시지 처리 ( 필드 중간 보스 처럼 )
	@param		: 몬스터 이름, 표시 시간
*/
void CX2GUNPC::SetEntryMonsterMessage( IN const char* pMonsterName_, IN float fVisibleTime_ )
{
	if ( NULL == g_pMain ||
		 NULL == g_pMain->GetNowState() )
		 return;

	CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );

	if ( NULL != pNowState )
	{
		m_wstrEnterMonsterName.clear();			/// 저장할 스트링 변수 초기화

		/// 인자로 받은 이름의 포맷 변환
		ConvertUtf8ToWCHAR( m_wstrEnterMonsterName, pMonsterName_ );

		/// @1이(가) 등장하였습니다.
		wstring wstrMessage = GET_REPLACED_STRING ( ( STR_ID_23588, "I", KHanSungPostWordUnicodeWrapper( m_wstrEnterMonsterName.c_str(), STR_ID_806, STR_ID_807 ) ) );

		m_wstrEnterMonsterName += wstrMessage;	/// 출력할 스트링 조합

		/// 화면 중앙에 메시지 출력
		pNowState->SetFadeInOutNotice( m_wstrEnterMonsterName.c_str(), fVisibleTime_ );
	}
}

/** @function : FollowTargetUnitBone_FrameMove
	@brief : 지정된 타겟 유닛의 본 위치로, 내(NPC) 위치를 이동 시키는 함수
*/
void CX2GUNPC::FollowTargetUnitBone_FrameMove()
{
	if( true == m_sFollowTargetUnitData.m_bUseFollowTargetUnitBone &&
		NULL != g_pX2Game )
	{
		CX2GameUnit* pTargetUnit = NULL;
		if( true == m_sFollowTargetUnitData.m_bIsUser )
			pTargetUnit = g_pX2Game->GetUserUnitByUID( m_sFollowTargetUnitData.m_TargetUnitUID );
		else
			pTargetUnit = g_pX2Game->GetNPCUnitByUID( static_cast<int>(m_sFollowTargetUnitData.m_TargetUnitUID) );

		if( NULL != pTargetUnit )
		{
			D3DXVECTOR3 vTargetBonePos;
			pTargetUnit->GetBonePos( m_sFollowTargetUnitData.m_TargetBoneName.c_str(), vTargetBonePos );

			SetPosition( vTargetBonePos, true );
		}
	}
}
void CX2GUNPC::SetFollowTargetUnitBone( UidType iTargetUnitUID_, const CHAR* pTargetBoneName_, bool bIsUser_ )
{
	if( NULL == pTargetBoneName_ )
		return;

	wstring wstrTargetBoneName = L"";
	ConvertUtf8ToWCHAR( wstrTargetBoneName, pTargetBoneName_);
	m_sFollowTargetUnitData.m_TargetBoneName = wstrTargetBoneName;
	m_sFollowTargetUnitData.m_TargetUnitUID = iTargetUnitUID_;
}
/** @function : CreateEltrionEvenlyLockonMissile
	@brief : -> 타겟 대상들을 골고루 타겟 하는 락온 미사일 생성
			 -> STL을 사용하기 위해 코드에서 처리 함
*/
void CX2GUNPC::CreateEltrionEvenlyLockonMissile()
{
	const D3DXVECTOR3 vBaseMissilePos( -8300.f, 6800.f, 9000.f );
	const int iMissileNum = 18;

	// # 미사일 데미지 이펙트 생성
	CX2DamageEffect::CEffect* arrayMissileDamageEffect[iMissileNum];
	D3DXVECTOR3 vCreatePos(vBaseMissilePos);
	for( int i=0; i<(iMissileNum/2); ++i )
	{
		vCreatePos.x += 2000.f;
		arrayMissileDamageEffect[i] =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Eltrion_S3_MOVE_TO_S4_Lock_Missile",
														GetPowerRate(), vCreatePos, GetRotateDegree(), GetRotateDegree() );
	}
	// 1초 후 미사일 데미지 이펙트 추가 생성
	vCreatePos = vBaseMissilePos;
	for( int i=(iMissileNum/2); i<iMissileNum; ++i )
	{
		vCreatePos.x += 2000.f;
		arrayMissileDamageEffect[i] =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Eltrion_S3_MOVE_TO_S4_Lock_Missile",
			GetPowerRate(), vCreatePos, GetRotateDegree(), GetRotateDegree() , 0, true, 1.f );
	}

	// # 가까이 있는 순서대로 1발씩 타겟 잡으며, 개체수보다 미사일이 많을 경우 첫 타겟 대상부터 다시 타겟팅
	std::map<float, UidType> mapNearUnitUid;
	g_pX2Game->GetNearUnitUidList( static_cast<CX2Room::TEAM_NUM>(GetTeam()), GetPos(), mapNearUnitUid, true);

	int iOffset = 0;
	std::map<float, UidType>::iterator it = mapNearUnitUid.begin();
	for( int i=0; i< iMissileNum; ++i )
	{
		//타겟 개체 수 보다, 미사일이 많으면, 첫 유닛부터 락온
		if( (i - iOffset) >= static_cast<int>(mapNearUnitUid.size()) )
		{
			iOffset = i;
			it = mapNearUnitUid.begin();
		}
		if ( arrayMissileDamageEffect[i] != NULL )
		{
			if( NULL != g_pX2Game->GetNPCUnitByUID(static_cast<int>(it->second)) )
				arrayMissileDamageEffect[i]->SetLockOnNPCUID( static_cast<int>(it->second) );
			else
				arrayMissileDamageEffect[i]->SetLockOnUnitUID( it->second );

			//타겟을 잃었을 때에는 가장 가까이 있는 타겟을 잡기 위해 락온타입 설정
			arrayMissileDamageEffect[i]->SetLockOnType(CX2DamageEffect::LOT_NEARST_UID_VECTOR);
		}
		++it;
	}

}



CX2DamageEffect::CEffect*	CX2GUNPC::GetDamageEffect_LUA( int index )
{
    if ( index < 0 || index >= ARRAY_SIZE( m_ahDamageEffect ) )
        return NULL;
    CX2DamageEffect::CEffect*   pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_ahDamageEffect[index] );
    if ( pEffect == NULL )
        m_ahDamageEffect[index] = INVALID_DAMAGE_EFFECT_HANDLE;
    return  pEffect;
}

void    CX2GUNPC::SetDamageEffect_LUA( int index, CX2DamageEffect::CEffect* val)
{
    if ( index < 0 || index >= ARRAY_SIZE( m_ahDamageEffect ) )
        return;
    if ( val == NULL || g_pX2Game->GetDamageEffect()->IsLiveInstance( val ) == false )
        m_ahDamageEffect[index] = INVALID_DAMAGE_EFFECT_HANDLE;
    else
        m_ahDamageEffect[index] = val->GetHandle();
}

void    CX2GUNPC::ClearDamageEffect_LUA( int index )
{
    if ( index < 0 || index >= ARRAY_SIZE( m_ahDamageEffect ) )
        return;
    m_ahDamageEffect[index] = INVALID_DAMAGE_EFFECT_HANDLE;
}


bool    CX2GUNPC::CanPassAllyUnit()
{
    const StateData& nowStateData = GetNowStateData();
    return  NOWSTATE_FLAG( CAN_PASS_ALLY_UNIT );
}



void    CX2GUNPC::CommonFrameMove_EffectSet( float fNowAnimationTime )
{
    const StateData& nowStateData = GetNowStateData();

	if( m_iEffectSetToPlayProgress < 0 || m_iEffectSetToPlayProgress >= (int) nowStateData.m_vecEffectSetToPlay.size() )
		return;

    bool    bDeleteEffectSetOnStateEnd = NOWSTATE_FLAG( DELETE_EFFECT_SET_ON_STATE_END );
    bool    bDeleteEffectSetOnDamageReact = NOWSTATE_FLAG( DELETE_EFFECT_SET_ON_DAMAGE_REACT );
    bool    bDeleteEffectSetOnDie = NOWSTATE_FLAG( DELETE_EFFECT_SET_ON_DIE );
    bool    bDeleteEffectSetOnCustomState = NOWSTATE_FLAG( DELETE_EFFECT_SET_ON_CUSTOM_STATE );
    bool    bHyperEffectSet = NOWSTATE_FLAG( HYPER_EFFECT_SET );

    for( ; m_iEffectSetToPlayProgress < (int) nowStateData.m_vecEffectSetToPlay.size(); ++m_iEffectSetToPlayProgress )
	{
		const EffectSetToPlay& effectSetToPlay = nowStateData.m_vecEffectSetToPlay[m_iEffectSetToPlayProgress];
		if( 0.f == effectSetToPlay.m_fStartAnimationTime || fNowAnimationTime >= effectSetToPlay.m_fStartAnimationTime )
		{
			CX2EffectSet::Handle hEffectSetInst = g_pX2Game->GetEffectSet()->PlayEffectSet( effectSetToPlay.m_wstrEffectSetName, this, NULL,
                bHyperEffectSet, GetPowerRate(), nowStateData.m_fEffectSetLifeTime );

			// 기존에 애니메이션 시간을 기준으로 이펙트를 생성하던 것을 effectSetToPlay.m_fStartAnimationTime이 추가되면서 애니메이션을 기준으로 하지 않도록 변경해준다
			if( 0.f != effectSetToPlay.m_fStartAnimationTime )
			{
				if ( CX2EffectSet::EffectSetInstance* pEffectSetInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSetInst ) )
				{
					pEffectSetInst->SetUseAnimationTiming( false );
				}
			}

			if( true == bDeleteEffectSetOnDamageReact )
			{
				m_vecEffectSetToDeleteOnDamageReact.push_back( hEffectSetInst );
			}

			if( true == bDeleteEffectSetOnStateEnd )
			{
				m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetInst );
			}

			if( true == bDeleteEffectSetOnDie )
			{
				m_vecEffectSetToDeleteOnDie.push_back( hEffectSetInst );
			}

			if( true == bDeleteEffectSetOnCustomState )
			{
				m_vecEffectSetToDeleteOnCustomState.push_back( hEffectSetInst );
			}

			AddEffectSetDeleteCondition( hEffectSetInst, effectSetToPlay.m_vecDeleteCondition );
		}
        else
        {
            break;
        }
	}
}


bool    CX2GUNPC::FindAddSlashTraceWithWeaponIndex( int iWeaponIndex, OUT D3DXVECTOR3& vAddSlashTrace ) const
{
    const StateData& nowStateData = GetNowStateData();
    std::map< int, D3DXVECTOR3 >::const_iterator iter = nowStateData.m_mapAddSlashTrace.find( iWeaponIndex );
    if ( iter != nowStateData.m_mapAddSlashTrace.end() )
    {
        vAddSlashTrace = iter->second;
        return true;
    }
    return false;
}



CKTDGParticleSystem::CParticleEventSequence*    CX2GUNPC::ParticleEffectData::CreateInstanceSequence( CX2GUNPC* pGUNPC, const D3DXVECTOR3& vPos )
{
    DestroyInstanceSequence();
    CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
    if ( m_bMajor == true )
    {
        pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) pGUNPC, m_Name.c_str(), vPos );
    }
    else
    {
        pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) pGUNPC, m_Name.c_str(), vPos );
    }
    if ( pSeq != NULL )
    {
	    m_hSeq = pSeq->GetHandle();
    }
    return  pSeq;
}

CKTDGParticleSystem::CParticleEventSequence*    CX2GUNPC::ParticleEffectData::GetInstanceSequence()
{
    if ( m_hSeq != INVALID_PARTICLE_SEQUENCE_HANDLE )
    {
        if ( m_bMajor == true )
            return g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeq );
        else
            return g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeq );
    }
    return  NULL;
}

void        CX2GUNPC::ParticleEffectData::DestroyInstanceSequence()
{
	if( m_hSeq != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		if( m_bMajor == true )
		{
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeq );
		}
		else
		{
			g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeq );
		}
        m_hSeq = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void    CX2GUNPC::ParticleEffectData::Destroy( ParticleEffectData* pParticleEffectData )
{
    if ( pParticleEffectData != NULL )
    {
        pParticleEffectData->DestroyInstanceSequence();
        delete pParticleEffectData;
    }
}

CKTDGXMeshPlayer::CXMeshInstance*           CX2GUNPC::MeshPlayerEffectData::CreateMeshInstance( CX2GUNPC* pGUNPC
    , const D3DXVECTOR3& vPos, const D3DXVECTOR3& vRotateDegree )
{
    DestroyMeshInstance();
    CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
    if ( m_bMajor == true )
    {
        pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) pGUNPC,  m_Name.c_str(), vPos, vRotateDegree, vRotateDegree );
    }
    else
    {
        pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) pGUNPC,  m_Name.c_str(), vPos, vRotateDegree, vRotateDegree );
    }
    if ( pMeshInst != NULL )
    {
        m_hMeshInst = pMeshInst->GetHandle();
    }
    return  pMeshInst;
}

CKTDGXMeshPlayer::CXMeshInstance*           CX2GUNPC::MeshPlayerEffectData::GetMeshInstance()
{
    if ( m_hMeshInst != INVALID_MESH_INSTANCE_HANDLE )
    {
        if ( m_bMajor == true )
            return g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMeshInst );
        else
            return g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hMeshInst );
    }
    return  NULL;
}

void        CX2GUNPC::MeshPlayerEffectData::DestroyMeshInstance()
{
	if( m_hMeshInst != INVALID_MESH_INSTANCE_HANDLE )
	{
		if( m_bMajor == true )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInst );
		}
		else
		{
			g_pX2Game->GetMinorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInst );
		}
        m_hMeshInst = INVALID_MESH_INSTANCE_HANDLE;
	}
}

/*static*/
void    CX2GUNPC::MeshPlayerEffectData::Destroy( MeshPlayerEffectData* pMeshPlayerEffectData )
{
    if ( pMeshPlayerEffectData != NULL )
    {
        pMeshPlayerEffectData->DestroyMeshInstance();
        delete  pMeshPlayerEffectData;
    }
}


void CX2GUNPC::AdjustFrameSkipRate( float fElapsedTime )
{
    int iFrameMoveInterval = g_pMain->GetFrameBufferNum();

    bool    bLocalUnit = IsLocalUnit();

    if ( m_iFrameMoveIntervalCounter >= iFrameMoveInterval )
        return;

    if ( m_NPCFrameData.syncData.stateChangeParts.stateChangeNum != m_NPCFrameData.unitCondition.stateChangeParts.stateChangeNum )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
        return;
    }

	if( GetNowStateID() == GUSI_DIE )
        return;

    if ( GetClassType() == CX2UnitManager::NCT_THING_WORLD_BLOCK
        && m_bNeverMove == true && m_pXSkinAnim->GetState() == CKTDGXSkinAnim::XAS_WAIT )
        return;

    const InitStateData& initStateData = GetInitStateData();
    const StateData& nowStateData = GetNowStateData();
    const StateRuntime& nowStateRuntime = GetNowStateRuntime();

    if ( NOWSTATE_FLAG( HINT_NO_PERFRAME_EVENT ) == false )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
		return;
    }

    if ( NOWSTATE_FLAG( HINT_DAMAGED_EVENT ) == true
        && m_bDamagedThisFrameMove == true )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
        return;
    }//if

    if ( bLocalUnit == false && m_cbufReceiveDynamicPacket.empty() == false )
    {
        if ( m_kNonHostReaction.IsNowStateReaction_FIRST() == true )
        {
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            return;
        }
        if ( m_NPCFrameData.unitCondition.fStopTime <= 0.0f &&
		            m_NPCFrameData.unitCondition.fStop2Time <= 0.0f )
        {
            if ( m_cbufReceiveDynamicPacket.size() > 1
                || m_cbufReceiveDynamicPacket.size() == 1
                    && m_cbufReceiveDynamicPacket.front().m_bMiniSync == false )
            {
		        m_iFrameMoveIntervalCounter = iFrameMoveInterval;
                return;
            }//if
        }//if
    }//if

    if ( initStateData.m_iHintTimerEventID >= 0 && initStateData.m_iHintTimerEventID < 10
        && m_aTimerList[ initStateData.m_iHintTimerEventID ].Predict_EventTimer(
            initStateData.m_vecHintTimerEventTrigger, fElapsedTime ) == true )
    {
    	m_iFrameMoveIntervalCounter = iFrameMoveInterval;
	    return;
    }//if

    bool    bHostSendPacket = bLocalUnit == true
        && ( g_pData->GetGameUDP()->GetNonRelayUIDs().empty() == false
				|| g_pData->GetGameUDP()->GetRelayUIDs().empty() == false );
    if ( bHostSendPacket == true )
    {
        if ( m_iSendReserveCount > 0 && ( m_bImmediate == true || g_pX2Game->IsPacketSendFrame() == true )
            || m_kNonHostReaction.CheckLastSentMiniInfo() == true && g_pX2Game->IsPacketSendFrame() == true )
        {
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
		    return;
        }
    }

	for( int i = 0; i < (int)m_EventIntervalList.size(); i++ )
	{
        D3DXVECTOR3* pIntervel = &m_EventIntervalList[i].first;
		if( pIntervel->x < pIntervel->y + fElapsedTime )
		{
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
		    return;
		}
	}

    float   fAnimElapsedTime = fElapsedTime;
    float   fAnimWaitTime = m_fAnimWaitTime;
    bool    bAnimWaitTimePassed = false;
	if( fAnimWaitTime > 0.0f )
	{
        fAnimWaitTime -= fAnimElapsedTime;
        bAnimWaitTimePassed = true;
	}
    if ( fAnimWaitTime > 0.0f )
    {
        fAnimElapsedTime = 0.f;
    }
    else
	{
        if ( bAnimWaitTimePassed == true )
            fAnimElapsedTime = -fAnimWaitTime;
        if ( fAnimElapsedTime < 0.f )
            fAnimElapsedTime = 0.f;
    }//if.. else..

    if ( m_pXSkinAnim->Predict_IsAnimationEnd( fAnimElapsedTime ) == true )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
		return;
    }//if

    if ( m_iEffectSetToPlayProgress >= 0 && m_iEffectSetToPlayProgress < (int) nowStateData.m_vecEffectSetToPlay.size() )
    {
		const EffectSetToPlay& effectSetToPlay = nowStateData.m_vecEffectSetToPlay[m_iEffectSetToPlayProgress];
        if ( effectSetToPlay.m_fStartAnimationTime <= m_pXSkinAnim->GetNowAnimationTime()
            + m_pXSkinAnim->GetNowAnimationSpeed() * fAnimElapsedTime )
        {
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
		    return;
        }
    }


	D3DXVECTOR3 vNPCPos = m_NPCFrameData.syncData.position;

    if ( IsCulled() == false )
    {
        if ( nowStateData.m_vecfHintVisualAnimEventTimer.empty() == false
            && m_pXSkinAnim->Predict_EventTimerOneshot( nowStateData.m_vecfHintVisualAnimEventTimer, fAnimElapsedTime ) == true )
        {
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            return;
        }//if

        if ( nowStateData.m_vecfHintVisualAnimTimer.empty() == false
            && m_pXSkinAnim->Predict_EventTimer( nowStateData.m_vecfHintVisualAnimTimer, fAnimElapsedTime ) == true )
        {
		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            return;
        }//if
    }

    if ( nowStateData.m_vecAttackTime.empty() == false )
	{
        for( const KAttackTimeProjSeqName& kATPSN : nowStateData.m_vecAttackTime )
	    {
		    if( m_pXSkinAnim->Predict_IntervalTimer( kATPSN.m_v2AttackTime.x, kATPSN.m_v2AttackTime.y, fAnimElapsedTime ) == true )
            {
		        m_iFrameMoveIntervalCounter = iFrameMoveInterval;
                return;
		    }
	    }
	}

    if ( nowStateData.m_vecfHintAnimEventTimer.empty() == false
        && m_pXSkinAnim->Predict_EventTimerOneshot( nowStateData.m_vecfHintAnimEventTimer, fAnimElapsedTime ) == true )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
        return;
    }//if

    if ( nowStateData.m_vecfHintAnimTimer.empty() == false
        && m_pXSkinAnim->Predict_EventTimer( nowStateData.m_vecfHintAnimTimer, fAnimElapsedTime ) == true )
    {
		m_iFrameMoveIntervalCounter = iFrameMoveInterval;
        return;
    }//if

    if ( nowStateData.m_vecfHintEventTimer.empty() == false )
    {
        std::vector<float>::const_iterator iterGreaterThanBeforeTime =
            std::upper_bound( nowStateData.m_vecfHintEventTimer.begin(), nowStateData.m_vecfHintEventTimer.end(), m_NPCFrameData.unitCondition.fStateTime );
        if ( false == ( iterGreaterThanBeforeTime == nowStateData.m_vecfHintEventTimer.end()
            || m_NPCFrameData.unitCondition.fStateTime + m_fElapsedTime < *iterGreaterThanBeforeTime ) )
        {
            m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            return;
        }//if
    }//if

	// 속도가 일정 이상
	if( m_PhysicParam.nowSpeed.x > 800.f
        || m_PhysicParam.nowSpeed.x < -800.f
        || m_PhysicParam.nowSpeed.y > 800.f
        || m_PhysicParam.nowSpeed.y < -800.f )
    {
		m_iFrameMoveIntervalCounter++;
        if ( m_iFrameMoveIntervalCounter >= iFrameMoveInterval )
            return;
    }//if

	// 공중에 떴나
 	if( m_NPCFrameData.unitCondition.bFootOnLine == false )
    {
 		m_iFrameMoveIntervalCounter++;
        if ( m_iFrameMoveIntervalCounter >= iFrameMoveInterval )
            return;
    }//if

    if ( IsCulled() == false && g_pKTDXApp->IsFinalFrameOfSimulationLoop() == true )
    {
        if ( m_pInitData->m_bViewDepthHeuristic == true )
        {
            D3DXVECTOR3 vEye = g_pX2Game->GetX2Camera()->GetCamera().GetEye();
		    D3DXVECTOR3 vLookVec = g_pX2Game->GetX2Camera()->GetCamera().GetLookVec();
		    D3DXVec3Normalize( &vLookVec, &vLookVec );
            D3DXVECTOR3 vDir = vNPCPos - vEye;
            float fDot = D3DXVec3Dot( &vLookVec, &vDir );
            if ( fDot < 3000.f )
    		    m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            else if ( fDot < 5000.f )
                m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 1 );
            else if ( fDot < 6333.f )
                m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 2 );
            else if ( fDot < 7221.f )
                m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 3 );
            else
                m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 4 );
            //else if ( fDot < 7813.f )
            //    m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 4 );
            //else if ( fDot < 5700.f )
            //    m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 5 );
        }
        else
        {
            m_iFrameMoveIntervalCounter = iFrameMoveInterval;
            return;
        }
    }

    if ( m_iFrameMoveIntervalCounter >= iFrameMoveInterval )
        return;

	// 가장 가까운 유저와의 거리는..?

	float DoubleFramemoveDistSq = 1000.f * 1000.f;

    int iUserUnitListSize = g_pX2Game->GetUserUnitListSize();
    float   fUnitWidth = GetUnitWidth( true );
    float   fYMin = vNPCPos.y;
    float   fYMax = fYMin + GetUnitHeight( true );
    float   fUserYMin;
    float   fUserYMax;
    D3DXVECTOR3 vUserPosFuture;
    float   fDistSq, fXZDistSq, fHalfSumSq;
	for( int i=0; i<iUserUnitListSize; i++ )
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnit(i);
		if( NULL == pUser || ( bHostSendPacket == false && pUser->IsMyUnit() == false ) )
			continue;
	    vUserPosFuture = pUser->GetPos( true );
        fUserYMin = vUserPosFuture.y;
        fUserYMax = fUserYMin + pUser->GetUnitHeight( true );
        if ( fUserYMax < fYMin )
            fDistSq = (fYMin - fUserYMax)*(fYMin - fUserYMax);
        else if ( fUserYMin > fYMax )
            fDistSq = (fUserYMin - fYMax)*(fUserYMin - fYMax);
        else
            fDistSq = 0.f;
        if ( fDistSq >= DoubleFramemoveDistSq )
            continue;
        vUserPosFuture.y = vNPCPos.y;
        fXZDistSq = GetDistance3Sq( vUserPosFuture, vNPCPos );
        fHalfSumSq = ( fUnitWidth + pUser->GetUnitWidth( true ) ) * 0.5f;
        fHalfSumSq *= fHalfSumSq;
        if ( fXZDistSq > fHalfSumSq )
            fDistSq += fXZDistSq - fHalfSumSq;
		if( fDistSq < DoubleFramemoveDistSq )
		{
            DoubleFramemoveDistSq = fDistSq;
		}
	}

    if ( DoubleFramemoveDistSq < 150.f * 150.f )
        m_iFrameMoveIntervalCounter = iFrameMoveInterval;
    else if ( DoubleFramemoveDistSq < 300.f * 300.f )
        m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 1 );
    else if ( DoubleFramemoveDistSq < 450.f * 450.f )
        m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 2 );
    else if ( DoubleFramemoveDistSq < 600.f * 600.f )
        m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 3 );
    else if ( DoubleFramemoveDistSq < 750.f * 750.f )
        m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 4 );
    else if ( DoubleFramemoveDistSq < 900.f * 900.f )
        m_iFrameMoveIntervalCounter = __max( m_iFrameMoveIntervalCounter, iFrameMoveInterval - 5 );

    if ( m_iFrameMoveIntervalCounter >= iFrameMoveInterval )
        return;

}

/** @function : SetMonsterGrade()
	@brief : m_eMonsterGrade 값은 중보에서만 쓰이도록 처리 됨
*/
void CX2GUNPC::SetMonsterGrade( MONSTER_GRADE eMonsterGrade_ )
{
	if( MG_MIDDLE_BOSS_NPC == eMonsterGrade_  )
		m_eMonsterGrade = eMonsterGrade_;

	return;
}

void CX2GUNPC::AddFieldBossRaidLog()
{
	if( NULL != g_pX2Game->GetMyUnit()
		&& CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID()
		&& g_pData->GetBattleFieldManager().GetIsBossRaidFieldByFieldID( g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam() )
		)
	{
		const UidType uMyUid = g_pX2Game->GetMyUnit()->GetUnitUID();
		const int iTotalDamage = g_pX2Game->GetMyUnit()->GetTotalAttackedDamage();
		const int iTotalAttackDamage = g_pX2Game->GetMyUnit()->GetTotalGivenDamamge();

		KEGS_BOSS_FIELD_LOG_NOT kPacket;
		kPacket.m_mapGivenDamage.insert( std::make_pair( uMyUid, iTotalAttackDamage ) );
		kPacket.m_mapAttackDamage.insert( std::make_pair( uMyUid, iTotalDamage ) );

		g_pData->GetServerProtocol()->SendPacket( EGS_BOSS_FIELD_LOG_NOT, kPacket );
	}
}
