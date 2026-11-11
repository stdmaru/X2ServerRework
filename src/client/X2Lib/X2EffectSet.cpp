#include "stdafx.h"
#include "./X2EffectSet.h"




#pragma DMLEE_NOTE( "effect set에 사운드 작업할 수 있게 기능 추가해야 함" )



CX2EffectSet::CX2EffectSet(
    unsigned char ucSystemID
    )
: m_coInstanceHandleList( LIST_NUM, 64 )
{
    m_ucSystemID = ucSystemID & 0x3;
}

CX2EffectSet::~CX2EffectSet(void)
{
	ClearTempletAndInstance();
}




void CX2EffectSet::ClearTempletAndInstance()
{
    StopEffectSetAll();


	for( EffectSetDataMap::value_type& value : m_mapEffectSetTemplet )
	{
		SAFE_DELETE( value.second );
	}
	m_mapEffectSetTemplet.clear();

}


void CX2EffectSet::OnFrameMove( double fTime, float fElapsedTime )
{

	KTDXPROFILE();
	OnFrameMove_CreateEffect(fTime, fElapsedTime);
	OnFrameMove_DeleteEffect();
	OnFrameMove_UpdateEffect();
	OnFrameMove_StartCameraShake();

}

void CX2EffectSet::OnFrameMove_CreateEffect( double fTime, float fElapsedTime )
{

	KTDXPROFILE();

	// 이펙트 생성 및 effectsetinstance framemove
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
	{
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
		pEffectSetInstance->OnFrameMove( fTime, fElapsedTime );

		bool bTraceTargetDead = false;
		for( UINT i=0; i<pEffectSetInstance->m_vecEffectInstanceCreated.size(); i++ )
		{
			if( true == pEffectSetInstance->m_vecEffectInstanceCreated[i] )
				continue;

			EffectData* pEffectData = pEffectSetInstance->m_pEffectSetData->m_vecpEffectData[i];
			if( NULL == pEffectData )
				continue;




			if( false == bTraceTargetDead &&
				true == pEffectData->m_bStartWhenTraceTargetDead &&
				null != pEffectSetInstance->m_optrTraceTargetGameUnit &&
				NULL != g_pX2Game &&
				pEffectSetInstance->m_optrTraceTargetGameUnit->GetNowHp() <= 0.f )						// trace target이 죽으면 이펙트 바로 생성
			{
				bTraceTargetDead = true;
			}



			if( false == bTraceTargetDead &&
				0.f != pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime ) // 0초에 시작이면 무조건 바로 생성
			{
				EFFECT_SET_TIMER_TYPE eTimingType = pEffectSetInstance->m_pEffectSetData->m_eEventTimerType;
				if( false == pEffectSetInstance->m_bUseAnimationTiming &&
					ESTT_ANIM == eTimingType )
				{
					eTimingType = ESTT_CUSTOM;
				}
				switch( eTimingType )
				{
				default:
				case ESTT_ANIM:
					{
						if( pEffectSetInstance->m_pPet != NULL )
						{
							if( pEffectSetInstance->m_pPet->GetXSkinAnim() != NULL &&
								pEffectSetInstance->m_pPet->GetXSkinAnim()->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
						else if ( NULL != pEffectSetInstance->GetOwnerUnit() &&
								  NULL != pEffectSetInstance->GetOwnerUnit()->GetRidingPet() &&
								  true == pEffectSetInstance->GetOwnerUnit()->GetRidingOn() )
						{
							if( pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr() != NULL &&
								pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr()->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
						else
						{
							if(
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
                                pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
					} break;

				case ESTT_CUSTOM:
					{
						if( pEffectSetInstance->m_fElapsedTime < pEffectData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
						{
							continue;
						}
					} break;
				}
			}

			pEffectSetInstance->m_vecEffectInstanceCreated[i] = true;

			if( true == bTraceTargetDead &&
				false == pEffectData->m_bStartWhenTraceTargetDead )
			{
				continue;
			}

			// 거리 제한이 지정되어 있다면
			if ( pEffectData->m_fLimitDistanceSq > 0.f &&
				NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMyUnit() )
			{
#if X2_DEV_TOOLS
				if( ET_DAMAGE_EFFECT == pEffectData->m_eEffectType )
					DISPLAY_ERROR(L" DamageEffect에는 LIMIT_DISTANCE를 설정 하지 마세요! ");
#endif // _IN_HOUSE_

				// 거리 제한에 걸리면 만든 것으로 체크하고 실제로 만들지는 않도록 설정
				const D3DXVECTOR3& vMyUserPos = g_pX2Game->GetMyUnit()->GetPos();
				if( GetDistance3Sq( vMyUserPos, pEffectSetInstance->m_vPosition ) > pEffectData->m_fLimitDistanceSq )
				{
					pEffectSetInstance->m_vecEffectInstanceCreated[i] = true;
					continue;
				}
			}


			const D3DXVECTOR3 vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );
			const D3DXVECTOR3 vEffectRotation = pEffectSetInstance->GetEffectRotation( pEffectData );
			const D3DXVECTOR3 vEffectScale = pEffectSetInstance->GetEffectScale( pEffectData );

			CX2DamageEffect::CEffect* pEffect = NULL;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
			CKTDGParticleSystem::CParticleEventSequence* pParticleSequence = NULL;

			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
					CX2GameUnit* pUnit = pEffectSetInstance->GetOwnerUnit();
					bool bPetDamageEffect = ( NULL != pEffectSetInstance->m_pPet ? true : false );

					float fPowerRate = pEffectSetInstance->m_fPowerRate;
					if( fPowerRate < 0.f )
					{
						if( NULL != pUnit )
						{
							if( bPetDamageEffect == true )
							{
								if( pUnit->GetRemainHyperModeTime() > 0.f )
									fPowerRate = 1.2f;
								else
									fPowerRate = 1.f;
							}
							else
							{
								fPowerRate = pUnit->GetPowerRate();
							}
						}
						else
						{
							ASSERT( !"effectset owner invalid" );
							pUnit = NULL;
							pEffectSetInstance->m_optrGameUnit.Reset();
						}
					}

					fPowerRate *= pEffectSetInstance->m_fPowerRateScale;

					if ( NULL != g_pX2Game )
					{
						if( bPetDamageEffect == true )
						{
						float fPowerRatePet = 1.f;
						// 펫 친밀도가 70%이상일 경우 배율 증가
						if( pEffectSetInstance->m_pPet != NULL && pEffectSetInstance->m_pPet->GetNowIntimacy() >= 0.7f )
							fPowerRatePet = ( pEffectSetInstance->m_pPet->GetNowIntimacy() - 0.6f + 1.f );
						pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectName.c_str(),
								fPowerRate * fPowerRatePet, vEffectPos, vEffectRotation, vEffectRotation );
						}
						else
						{
							if( true == pEffectSetInstance->m_bHyper &&
								false == pEffectData->m_EffectNameHyper.empty() )
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectNameHyper.c_str(),
									fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
							}
							else
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pUnit, pEffectData->m_EffectName.c_str(),
									fPowerRate, vEffectPos, vEffectRotation, vEffectRotation );
							}
						}
					}

					if( NULL != pEffect )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pEffect->SetScale( vScaleAtTraceMatrix );
						}
						else
							pEffect->SetScale( vEffectScale );
						//if ( NULL != pEffect->GetLockOnData() )
						{
							if ( NULL != pUnit && CX2GameUnit::GUT_USER == pUnit->GetGameUnitType() )
							{
								CX2GUUser* pUser = static_cast<CX2GUUser*>(pUnit);
								pUser->SetLockOn( pEffect, 0, pEffect->GetLockOnData().m_LockOnType );


							}
						}
					}

				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
				{

					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos,
							vEffectRotation, vEffectRotation );
					}
					else
					{
						pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos,
							vEffectRotation, vEffectRotation );
					}

					if( NULL != pMeshInstance )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pMeshInstance->SetScale( vScaleAtTraceMatrix );
						}
						else
							pMeshInstance->SetScale( vEffectScale );

						if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
						}
					}

				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos,
							vEffectRotation, vEffectRotation );
					}
					else
					{
						pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->CreateInstance( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos,
							vEffectRotation, vEffectRotation );
					}


					if( NULL != pMeshInstance )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pMeshInstance->SetScale( vScaleAtTraceMatrix );
						}
						else
							pMeshInstance->SetScale( vEffectScale );

						if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pMeshInstance->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}

						}
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos,
								vEffectRotation, vEffectRotation );
						}
						else
						{
							pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos,
								vEffectRotation, vEffectRotation );
						}

						if( NULL != pMeshInstance )
						{
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pMeshInstance->SetScale( vScaleAtTraceMatrix );
							}
							else
								pMeshInstance->SetScale( vEffectScale );

							if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
							{
								if( NULL != pOwnerGameUnit )
								{
									pMeshInstance->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}
				} break;

			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos,
								vEffectRotation, vEffectRotation );
						}
						else
						{
							pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos,
								vEffectRotation, vEffectRotation );
						}

						if( NULL != pMeshInstance )
						{
							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pMeshInstance->SetScale( vScaleAtTraceMatrix );
							}
							else
								pMeshInstance->SetScale( vEffectScale );

							if( true == pMeshInstance->GetIsSetLandHeightOnStart() )
							{
								if( NULL != pOwnerGameUnit )
								{
									pMeshInstance->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}

				} break;

			case ET_PARTICLE_UI_MAJOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pParticleSequence = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
					}
					else
					{
						pParticleSequence = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos );
					}

					if( pParticleSequence != NULL )
					{
						if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
						{
							pParticleSequence->SetAxisAngle( vEffectRotation );
							pParticleSequence->SetAddRotate( vEffectRotation );
						}

						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
						}
						else
							pParticleSequence->SetScaleFactor( vEffectScale );

						/// 파티클에 대하여 Set Over UI 기능 추가
						if ( true == pEffectData->m_bSetOverUI )
							pParticleSequence->SetOverUI( true );

						if( pParticleSequence->GetUseLand() == true )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
						}
					}
				} break;

			case ET_PARTICLE_UI_MINOR:
				{
					if( true == pEffectSetInstance->m_bHyper &&
						false == pEffectData->m_EffectNameHyper.empty() )
					{
						pParticleSequence = g_pData->GetUIMinorParticle()->CreateSequence( NULL,  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
					}
					else
					{
						pParticleSequence = g_pData->GetUIMinorParticle()->CreateSequence( NULL,  pEffectData->m_EffectName.c_str(), vEffectPos );
					}


					if( pParticleSequence != NULL )
					{
						if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
						{
							pParticleSequence->SetAxisAngle( vEffectRotation );
							pParticleSequence->SetAddRotate( vEffectRotation );
						}

						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
						if ( true == pEffectData->m_bTraceMatrix && NULL != pOwnerGameUnit )
						{
							const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
															       vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

							pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
						}
						else
							pParticleSequence->SetScaleFactor( vEffectScale );


						/// 파티클에 대하여 Set Over UI 기능 추가
						if ( true == pEffectData->m_bSetOverUI )
							pParticleSequence->SetOverUI( true );

						if( pParticleSequence->GetUseLand() == true )
						{
							if( NULL != g_pX2Game &&
								null != pEffectSetInstance->m_optrGameUnit )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_optrGameUnit->GetLandPos().y );
							}
							else if( pEffectSetInstance->m_pSquareUnit != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pSquareUnit->GetLandPosition().y );
							}
							else if( pEffectSetInstance->m_pPet != NULL )
							{
								pParticleSequence->SetLandPosition( pEffectSetInstance->m_pPet->GetLandPosition_LUA().y );
							}
						}
					}
				} break;

			case ET_PARTICLE_GAME_MAJOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pParticleSequence = g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
						}
						else
						{
							pParticleSequence = g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos );
						}

						if ( pParticleSequence != NULL )
						{
							if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
							{
								pParticleSequence->SetAxisAngle( vEffectRotation );
								pParticleSequence->SetAddRotate( vEffectRotation );
							}

							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
																	   vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
																	   vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
							}
							else
								pParticleSequence->SetScaleFactor( vEffectScale );

							/// 파티클에 대하여 Set Over UI 기능 추가
							if ( true == pEffectData->m_bSetOverUI )
								pParticleSequence->SetOverUI( true );

							if( pParticleSequence->GetUseLand() == true )
							{
								if( NULL != pOwnerGameUnit )
								{
									pParticleSequence->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}

				} break;

			case ET_PARTICLE_GAME_MINOR:
				{
					if ( NULL != g_pX2Game )
					{
						CX2GameUnit* pOwnerGameUnit = pEffectSetInstance->GetOwnerUnit();

						if( true == pEffectSetInstance->m_bHyper &&
							false == pEffectData->m_EffectNameHyper.empty() )
						{
							pParticleSequence = g_pX2Game->GetMinorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectNameHyper.c_str(), vEffectPos );
						}
						else
						{
							pParticleSequence = g_pX2Game->GetMinorParticle()->CreateSequence( static_cast<CKTDGObject*>( pOwnerGameUnit ),  pEffectData->m_EffectName.c_str(), vEffectPos );
						}

						if( pParticleSequence != NULL )
						{
							if( D3DXVECTOR3(0, 0, 0) != vEffectRotation )
							{
								pParticleSequence->SetAxisAngle( vEffectRotation );
								pParticleSequence->SetAddRotate( vEffectRotation );
							}

							/// 행렬 안에 UnitScale 값이 들어 있기 때문에, 행렬 Trace 일때는 UnitScale 제외해주자.
							if ( true == pEffectData->m_bTraceMatrix )
							{
								const D3DXVECTOR3 vScaleAtTraceMatrix( vEffectScale.x / pOwnerGameUnit->GetScaleByUnit().x / pEffectSetInstance->m_vBoneTraceTargetMeshScale.x,
									vEffectScale.y / pOwnerGameUnit->GetScaleByUnit().y / pEffectSetInstance->m_vBoneTraceTargetMeshScale.y,
									vEffectScale.z / pOwnerGameUnit->GetScaleByUnit().z / pEffectSetInstance->m_vBoneTraceTargetMeshScale.z );

								pParticleSequence->SetScaleFactor( vScaleAtTraceMatrix );
							}
							else
								pParticleSequence->SetScaleFactor( vEffectScale );

							/// 파티클에 대하여 Set Over UI 기능 추가
							if ( true == pEffectData->m_bSetOverUI )
								pParticleSequence->SetOverUI( true );

							if( pParticleSequence->GetUseLand() == true )
							{
								if( NULL != pOwnerGameUnit )
								{
									pParticleSequence->SetLandPosition( pOwnerGameUnit->GetLandPos().y );
								}
							}
						}
					}
				} break;
			default:
				break;
			}


			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
					if( NULL != pEffect )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
                        pEffectInstance->m_hDamageEffect		= pEffect->GetHandle();
						pEffectInstance->m_hMeshPlayer			= INVALID_MESH_INSTANCE_HANDLE;
						pEffectInstance->m_hParticleSequence	= INVALID_PARTICLE_SEQUENCE_HANDLE;
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );
						if( true == pEffectData->m_bBoneTraceTargetMesh &&
							INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
						{
							pEffectSetInstance->m_hTargetMesh = pEffect->GetMainEffectHandle();
							pEffectSetInstance->m_eTargetMeshEffectType = ET_MESH_PLAYER_GAME_MAJOR;
							pEffectSetInstance->m_bIsTargetMesh = true;
							if ( null != pEffectSetInstance->m_optrGameUnit )
							{
								/// 유닛의 IsRight 값으로 타겟 매시의 방향을 설정
								if ( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
									pEffectSetInstance->m_bIsTargetMeshRight = true;

								/// 유닛의 방향 벡터 값으로 타겟 매시의 방향 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
								/// 유닛의 Z 벡터 값으로 타겟 매시의 Z 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
							}

							/// 행렬 Trace일 때 BoneTraceTargetMesh의 스케일 값 빼주기 위하여, 스케일 값 저장
							if ( true == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = D3DXVECTOR3( 1.f, 1.f, 1.f );
							else
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = pEffectData->m_vScale;
						}
					}
				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
			case ET_MESH_PLAYER_UI_MINOR:
			case ET_MESH_PLAYER_GAME_MAJOR:
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( NULL != pMeshInstance )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
                        pEffectInstance->m_hDamageEffect		= INVALID_DAMAGE_EFFECT_HANDLE;
						pEffectInstance->m_hMeshPlayer			= pMeshInstance->GetHandle();
						pEffectInstance->m_hParticleSequence	= INVALID_PARTICLE_SEQUENCE_HANDLE;
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );

						if ( pEffectData->m_bUseSubAttackListSet && null != pEffectSetInstance->m_optrGameUnit )
							pEffectSetInstance->m_optrGameUnit->InsertPairSubAttackListSet( pEffectInstance->m_hMeshPlayer, pMeshInstance->GetXSkinAnim() );

#if X2_DEV_TOOLS
						//{{AFX
						if( true == pEffectInstance->m_pEffectData->m_bApplyUnitRotation &&
							CKTDGMatrix::BT_NONE != pMeshInstance->GetTempletData()->billBoardType )
						{
							WCHAR wszMsg[1024] = L"";
							StringCchPrintfW( wszMsg, ARRAY_SIZE(wszMsg), L"%s, %s, %s, %d",
								pEffectSetInstance->m_pEffectSetData->m_wstrEffectSetName.c_str(),
								pEffectInstance->m_pEffectData->m_EffectName.c_str(),
								pMeshInstance->GetTempletData()->templetName.c_str(),
								(int) pMeshInstance->GetTempletData()->billBoardType );
							StateLog( wszMsg );

						}
						//}}AFX
#endif // _IN_HOUSE_
						if( true == pEffectData->m_bBoneTraceTargetMesh &&
							INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
						{
							pEffectSetInstance->m_hTargetMesh = pMeshInstance->GetHandle();
							pEffectSetInstance->m_eTargetMeshEffectType = pEffectData->m_eEffectType;
							pEffectSetInstance->m_bIsTargetMesh = true;
							/// 유닛의 IsRight 값으로 타겟 매시의 방향을 설정
							if ( null != pEffectSetInstance->m_optrGameUnit )
							{
								if ( true == pEffectSetInstance->m_optrGameUnit->GetIsRight() )
									pEffectSetInstance->m_bIsTargetMeshRight = true;

								/// 유닛의 방향 벡터 값으로 타겟 매시의 방향 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshDirVector	= pEffectSetInstance->m_optrGameUnit->GetDirVector();
								/// 유닛의 Z 벡터 값으로 타겟 매시의 Z 벡터를 설정
								pEffectSetInstance->m_vecTargetMeshZVector		= pEffectSetInstance->m_optrGameUnit->GetZVector();
							}

							/// 행렬 Trace일 때 BoneTraceTargetMesh의 스케일 값 빼주기 위하여, 스케일 값 저장
							if ( true == IsSamef( 0.f, pEffectData->m_vScale.x ) )
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = D3DXVECTOR3( 1.f, 1.f, 1.f );
							else
								pEffectSetInstance->m_vBoneTraceTargetMeshScale = pEffectData->m_vScale;
						}


					}
				} break;

			case ET_PARTICLE_UI_MAJOR:
			case ET_PARTICLE_UI_MINOR:
			case ET_PARTICLE_GAME_MAJOR:
			case ET_PARTICLE_GAME_MINOR:
				{
					if( NULL != pParticleSequence )
					{
						EffectInstance* pEffectInstance = new EffectInstance;
						pEffectInstance->m_pEffectData			= pEffectData;
                        pEffectInstance->m_hDamageEffect		= INVALID_DAMAGE_EFFECT_HANDLE;
						pEffectInstance->m_hMeshPlayer			= INVALID_MESH_INSTANCE_HANDLE;
						pEffectInstance->m_hParticleSequence	= pParticleSequence->GetHandle();
						pEffectInstance->m_optrGameUnit			= pEffectSetInstance->m_optrGameUnit;

						pEffectSetInstance->m_vecpEffectInstance.push_back( pEffectInstance );
					}
				} break;
			default:
				break;
			}
		}
	}
}

void CX2EffectSet::OnFrameMove_DeleteEffect()
{
	KTDXPROFILE();

	// 이펙트 삭제
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
	{
        iterNext = iter;    ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
        {
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
            continue;
        }

		if( true == pEffectSetInstance->m_bIsTargetMesh &&
			INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )
		{
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}

		if( true == pEffectSetInstance->m_bCreateEffectSetTraceUnit &&
			true == pEffectSetInstance->m_bTraceUnitDieDeleteEffectSet &&
			( null == pEffectSetInstance->m_optrTraceTargetGameUnit ||
			0 >= pEffectSetInstance->m_optrTraceTargetGameUnit->GetNowHp() ))
		{
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}

		// game에 없는 user의 포인터를 가지고 있다면 지운다
		if( ESM_GAME == pEffectSetInstance->m_eMode )
		{
			if( NULL != g_pX2Game && null == pEffectSetInstance->m_optrGameUnit )
			{
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}

		if( pEffectSetInstance->m_pPet != NULL )
		{
			if( g_pData->GetPetManager()->IsValidPet(pEffectSetInstance->m_pPet) == false )
			{
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}

		if( pEffectSetInstance->m_pEffectSetData->m_iDeleteShakeCount > 0 )
		{
			if( pEffectSetInstance->m_iCurrDeleteShakeCount >= pEffectSetInstance->m_pEffectSetData->m_iDeleteShakeCount )
			{
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
			else
			{
				if( null != pEffectSetInstance->m_optrTraceTargetGameUnit &&
					NULL != g_pX2Game )
				{

					switch( pEffectSetInstance->m_optrTraceTargetGameUnit->GetGameUnitType() )
					{
					case CX2GameUnit::GUT_USER:
						{
							CX2GUUser* pCX2GUUser = static_cast<CX2GUUser*>( pEffectSetInstance->m_optrTraceTargetGameUnit.GetObservable() );
							if( pCX2GUUser != NULL )
							{
								if( pCX2GUUser->GetIsRight() != pCX2GUUser->GetIsRightBefore() )
								{
									++pEffectSetInstance->m_iCurrDeleteShakeCount;
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
			}
		}

		if( pEffectSetInstance->m_fLifeTime > 0.f )
		{
			if( pEffectSetInstance->m_fElapsedTime > pEffectSetInstance->m_fLifeTime )
			{
                iter->m_pInstance = NULL;
                m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			    SAFE_DELETE( pEffectSetInstance );
				continue;
			}
		}

		bool bEffectSetInstanceAlive = false;
		for( UINT i=0; i<pEffectSetInstance->m_vecEffectInstanceCreated.size(); i++ )
		{
			if( false == pEffectSetInstance->m_vecEffectInstanceCreated[i] )
			{
				bEffectSetInstanceAlive = true;
				break;
			}
		}

		for( UINT i=0; i<pEffectSetInstance->m_vecpEffectInstance.size(); i++ )
		{
			EffectInstance* pEffectInstance = pEffectSetInstance->m_vecpEffectInstance[i];
			if( false == pEffectInstance->m_bAlive )
				continue;

			EffectData* pEffectData = pEffectInstance->m_pEffectData;
			switch( pEffectData->m_eEffectType )
			{
			default:
				{
					ASSERT( !"no type effect" );
					pEffectInstance->m_bAlive = false;
				} break;
			case ET_DAMAGE_EFFECT:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetDamageEffect()->IsLiveInstanceHandle( pEffectInstance->m_hDamageEffect ) )
					{
						pEffectInstance->m_bAlive = false;
                        pEffectInstance->m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
					}
				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
				{
					if( false == g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( false == g_pData->GetUIMinorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMajorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMinorXMeshPlayer()->IsLiveInstanceHandle( pEffectInstance->m_hMeshPlayer ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hMeshPlayer = INVALID_MESH_INSTANCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_UI_MAJOR:
				{
					if( false == g_pData->GetUIMajorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_UI_MINOR:
				{
					if( false == g_pData->GetUIMinorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_GAME_MAJOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMajorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			case ET_PARTICLE_GAME_MINOR:
				{
					if( NULL != g_pX2Game && false == g_pX2Game->GetMinorParticle()->IsLiveInstanceHandle( pEffectInstance->m_hParticleSequence ) )
					{
						pEffectInstance->m_bAlive = false;
						pEffectInstance->m_hParticleSequence = INVALID_PARTICLE_SEQUENCE_HANDLE;
					}
				} break;
			}

			if( true == pEffectInstance->m_bAlive &&
				false == pEffectData->m_bPassiveEffect )
			{
				bEffectSetInstanceAlive = true;
			}
		}


		if( false == bEffectSetInstanceAlive )
		{
            iter->m_pInstance = NULL;
            m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), iter );
			SAFE_DELETE( pEffectSetInstance );
			continue;
		}
	}
}



void CX2EffectSet::OnFrameMove_UpdateEffect()
{

	KTDXPROFILE();

	// 이펙트 위치 갱신
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
	{
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
		for( UINT i=0; i<pEffectSetInstance->m_vecpEffectInstance.size(); i++ )
		{
			EffectInstance* pEffectInstance = pEffectSetInstance->m_vecpEffectInstance[i];
			EffectData* pEffectData = pEffectInstance->m_pEffectData;

			if( false == pEffectData->m_bTrace
				&& false == pEffectData->m_bTraceMore
				//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
				&& false == pEffectData->m_bTraceMatrix
				//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
				)
				continue;

			if( false == pEffectInstance->m_bAlive )
				continue;


			D3DXVECTOR3 vEffectPos = pEffectSetInstance->GetEffectPosition( pEffectData );


			// 타겟 메시가 사라지면 더이상 위치 갱신 하지 않기
			if( true == pEffectSetInstance->m_bIsTargetMesh &&
				false == pEffectData->m_bTraceUserTargetForce &&
				INVALID_MESH_INSTANCE_HANDLE == pEffectSetInstance->m_hTargetMesh )

				break;


			switch( pEffectData->m_eEffectType )
			{
			case ET_DAMAGE_EFFECT:
				{
					if ( NULL != g_pX2Game &&
						NULL != g_pX2Game->GetDamageEffect() )
                    {
                        if ( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect ) )
					    {
						    pEffect->SetPos( vEffectPos );
					    }
                    }

				} break;

			case ET_MESH_PLAYER_UI_MAJOR:
				{
					if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
						INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( NULL != pInstance )
						{
							pInstance->SetPos( vEffectPos );
						}
					}
				} break;
			case ET_MESH_PLAYER_UI_MINOR:
				{
					if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
						INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( NULL != pInstance )
						{
							pInstance->SetPos( vEffectPos );
						}
					}
				} break;
			case ET_MESH_PLAYER_GAME_MAJOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( pInstance != NULL )
						{
							pInstance->SetPos(vEffectPos);
						}
					}
				}
				break;
			case ET_MESH_PLAYER_GAME_MINOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
						if( pInstance != NULL )
						{
							pInstance->SetPos(vEffectPos);
						}
					}
				} break;
			case ET_PARTICLE_UI_MAJOR:
				{
					if( g_pData->GetUIMajorParticle() != NULL &&
						pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
						}
					}
				} break;

			case ET_PARTICLE_UI_MINOR:
				{
					if( g_pData->GetUIMinorParticle() != NULL &&
						pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
						}
					}
				} break;

			case ET_PARTICLE_GAME_MAJOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
						}
					}
				}
				break;

			case ET_PARTICLE_GAME_MINOR:
				{
					if( g_pX2Game != NULL && g_pX2Game->GetMinorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
						if( pSeq != NULL )
						{
							pSeq->SetPosition( vEffectPos, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
						}
					}
				}
				break;
			default:
				break;
			}

			//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
			if ( true == pEffectData->m_bTraceMatrix )
			{
				const D3DXMATRIX* pCombineMatrix = pEffectSetInstance->GetCombineMatrix( pEffectData );

				if ( NULL != pCombineMatrix )
				{
					switch( pEffectData->m_eEffectType )
					{
					case ET_DAMAGE_EFFECT:
						{
							if( NULL != g_pX2Game &&
								NULL != g_pX2Game->GetDamageEffect() )
                            {
                                if ( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect ) )
							    {
								    CKTDGXMeshPlayer::CXMeshInstance* pInstance = pEffect->GetMainEffect();
								    if( NULL != pInstance )
								    {
									    pInstance->SetUseDXMatrix( *pCombineMatrix );
								    }
							    }
                            }
						}
						break;

					case ET_MESH_PLAYER_UI_MAJOR:
						{
							if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( NULL != pInstance )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						} break;
					case ET_MESH_PLAYER_UI_MINOR:
						{
							if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
								INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( NULL != pInstance )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						} break;
					case ET_MESH_PLAYER_GAME_MAJOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( pInstance != NULL )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						}
						break;
					case ET_MESH_PLAYER_GAME_MINOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
							{
								CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
								if( pInstance != NULL )
								{
									pInstance->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						} break;
					case ET_PARTICLE_GAME_MAJOR:
						{
							if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
							{
								CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
								if( pSeq != NULL )
								{
									pSeq->SetUseDXMatrix( *pCombineMatrix );
								}
							}
						} break;

					default:
						break;
					}
				} // if
			} // if
			//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

			if ( true == pEffectData->m_bReverseY )
			{
				switch( pEffectData->m_eEffectType )
				{
				case ET_PARTICLE_GAME_MAJOR:
					{
						if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );

							if( pSeq != NULL )
							{
								CKTDGMatrix::BILLBOARD_TYPE eBillboardType = pSeq->GetBillBoardType();

								if ( CKTDGMatrix::BT_NONE != eBillboardType )
								{
									/// 타겟 매시가 설정되어 있다면, 타겟 매시의 IsRight 값을 사용한다.
									if ( INVALID_MESH_INSTANCE_HANDLE != pEffectSetInstance->m_hTargetMesh &&
										 false == pEffectData->m_bTraceUserTargetForce )
										pSeq->SetReverseY( !( pEffectSetInstance->m_bIsTargetMeshRight ) );
									/// 타겟 매시가 설정되어 있지 않다면, 유저의 IsRight 값을 사용한다.
									else if ( null != pEffectInstance->m_optrGameUnit )
										pSeq->SetReverseY( !( pEffectInstance->m_optrGameUnit->GetIsRight() ) );
								}
							}
						}
					} break;
				default:
					break;
				}
			}
		}
	}
}

void CX2EffectSet::OnFrameMove_StartCameraShake()
{
	KTDXPROFILE();

	if ( NULL == g_pX2Game )
		return;

	//camera shake
    KInstanceHandleList::iterator iterNext;
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
	{
        iterNext = iter; ++iterNext;
        EffectSetInstance* pEffectSetInstance = iter->m_pInstance;
        if ( pEffectSetInstance == NULL )
            continue;
		EffectSetData* pEffectSetData = pEffectSetInstance->m_pEffectSetData;

		for ( UINT i=0; i<pEffectSetData->m_vecpCameraShakeData.size(); i++ )
		{
			if( true == pEffectSetInstance->m_vecCameraShakeDone[i] )
				continue;

			CameraShakeData* pCameraShakeData = pEffectSetData->m_vecpCameraShakeData[i];


			if( 0.f != pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime ) // 0초에 시작이면 무조건 바로 생성
			{
				switch( pEffectSetInstance->m_pEffectSetData->m_eEventTimerType )
				{
				default:
				case ESTT_ANIM:
					{
						if( pEffectSetInstance->m_pPet != NULL )
						{
							if( pEffectSetInstance->m_pPet->GetXSkinAnim() != NULL &&
								pEffectSetInstance->m_pPet->GetXSkinAnim()->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
						else if ( NULL != pEffectSetInstance->GetOwnerUnit() && NULL != pEffectSetInstance->GetOwnerUnit()->GetRidingPet() )
						{
							if( pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr() != NULL &&
								pEffectSetInstance->GetOwnerUnit()->GetRidingPet()->GetXSkinAnimPtr()->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
						else
						{
							if(
                                pEffectSetInstance->m_pXSkinAnim != NULL &&
                                pEffectSetInstance->m_pXSkinAnim->GetNowAnimationTime() < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
								continue;
						}
					} break;

				case ESTT_CUSTOM:
					{
						//if( pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime <= pEffectSetInstance->m_fPrevElapsedTime ||
						//	pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime > pEffectSetInstance->m_fElapsedTime )
						if( pEffectSetInstance->m_fElapsedTime < pCameraShakeData->m_fStartAnimTime + pEffectSetInstance->m_fDelayTime )
						{
							continue;
						}
					} break;
				}
			}


			pEffectSetInstance->m_vecCameraShakeDone[i] = true;
			if( ( NULL != g_pX2Game->GetMyUnit() && GetDistance3Sq( g_pX2Game->GetMyUnit()->GetPos(), pEffectSetInstance->m_vPosition ) < pCameraShakeData->m_iLimitDistanceSq ) || IGNORE_DISTANCE == pCameraShakeData->m_iLimitDistanceSq )
			{
				switch( pCameraShakeData->m_eShakeType )
				{
				case CKTDGCamera::DECT_UP:
					{
						g_pX2Game->GetX2Camera()->GetCamera().UpCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;
				case CKTDGCamera::DECT_DOWN:
					{
						g_pX2Game->GetX2Camera()->GetCamera().DownCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;
				case CKTDGCamera::DECT_UP_DOWN:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeUpDown( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime, pCameraShakeData->m_fTwoDirTimeGap );
					} break;
				case CKTDGCamera::DECT_UP_DOWN_NO_RESET:
					{
						g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCameraNoReset( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime );
					} break;

				case CKTDGCamera::DECT_LEFT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().LeftCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;

				case CKTDGCamera::DECT_RIGHT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().RightCrashCamera( pCameraShakeData->m_fOneDirSpeed, pCameraShakeData->m_fOneDirAccel );
					} break;

				case CKTDGCamera::DECT_LEFT_RIGHT:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeLeftRight( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime, pCameraShakeData->m_fTwoDirTimeGap );
					} break;
				case CKTDGCamera::DECT_LEFT_RIGHT_NO_RESET:
					{
						g_pX2Game->GetX2Camera()->GetCamera().LeftRightCrashCameraNoReset( pCameraShakeData->m_fTwoDirGap, pCameraShakeData->m_fTwoDirTime );
					} break;



				case CKTDGCamera::DECT_RANDOM:
					{
						g_pX2Game->GetX2Camera()->GetCamera().ShakeRandom( pCameraShakeData->m_iEyeShakeCount, pCameraShakeData->m_EyeMoveSpeed.m_Min, pCameraShakeData->m_EyeMoveSpeed.m_Max,
							pCameraShakeData->m_EyeMoveRange.m_Min, pCameraShakeData->m_EyeMoveRange.m_Max,
							pCameraShakeData->m_iLookAtShakeCount, pCameraShakeData->m_LookAtMoveSpeed.m_Min, pCameraShakeData->m_LookAtMoveSpeed.m_Max,
							pCameraShakeData->m_LookAtMoveRange.m_Min, pCameraShakeData->m_LookAtMoveRange.m_Max,
							pCameraShakeData->m_bFixedLookVec );
					} break;
				default:
					break;
				}
			}
		}

	}
}

CX2EffectSet::EffectSetInstance*   CX2EffectSet::_CreateEffectSetInstance(
    EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr skinAnimPtr, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit,
    CX2PET *pPet
    )
{
    if ( pEffectSetData == NULL )
        return NULL;

    KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
    if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
    {
        if ( m_coInstanceHandleList.storage_size() >= 0x10000 )
        {
            return NULL;
        }
        m_coInstanceHandleList.push_back_default(LIST_FREE);
        iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
    }


	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, pSquareUnit
        , pPet
        );
    ASSERT( pEffectSetInstance != NULL );
    m_coInstanceHandleList.splice( m_coInstanceHandleList.end(LIST_LIVE), iterEmpty );
    iterEmpty->m_pInstance = pEffectSetInstance;

    WORD   wIndex = (WORD) iterEmpty.GetIndex();
    Handle  handle;
    DWORD   dwHandle = 0;
    do
    {
        ++iterEmpty->m_wStamp;
        dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
        handle.SetValue( (int) dwHandle );
    } while ( handle == INVALID_EFFECTSET_HANDLE );
    pEffectSetInstance->SetHandle( handle );

    return  pEffectSetInstance;
}


CX2EffectSet::Handle CX2EffectSet::PlayEffectSet( const wstring& wstrEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit /*= NULL*/,
												 bool bHyper /*= false*/, float fPowerRate /*= -1.f*/, float fLifeTime /*= -1.f */,
												 D3DXVECTOR3 vScale /*= D3DXVECTOR3( 1, 1, 1 )*/,
												 bool bCustomPosition /*= false*/, const D3DXVECTOR3& vPosition/* = D3DXVECTOR3(0, 0, 0)*/,
												 const D3DXVECTOR3& vRotationDegree /*= D3DXVECTOR3(0, 0, 0)*/, const D3DXVECTOR3& vDirVector /*= D3DXVECTOR3(0, 0, 0)*/)
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( NULL == pGameUnit )
		return INVALID_EFFECTSET_HANDLE;



	CKTDGXSkinAnimPtr skinAnimPtr;
	switch( pGameUnit->GetGameUnitType() )
	{
	case CX2GameUnit::GUT_USER:
		{
			CX2GUUser* pUser = (CX2GUUser*) pGameUnit;
			skinAnimPtr = pUser->GetXSkinAnimPtr();
		} break;

	case CX2GameUnit::GUT_NPC:
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) pGameUnit;
			skinAnimPtr = pNPC->GetXSkinAnimPtr();
		} break;
	}

	if( skinAnimPtr == NULL )
		return INVALID_EFFECTSET_HANDLE;


    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, skinAnimPtr, pGameUnit, pTraceTargetGameUnit, CX2SquareUnitPtr() );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;



#if X2_DEV_TOOLS
	if( NULL != g_pMain->GetGameEdit() &&
		CX2GameEdit::ELL_EFFECT_LOG_1 <= g_pMain->GetGameEdit()->GetEffectLogLevel() )
	{
		dbg::clog << L"(이펙트셋)" << wstrEffectSetName << dbg::endl;
		KLOG("EffectLog.txt")  << L"EffectSet : " << wstrEffectSetName.c_str() << fileout;
	}
#endif // EFFECT_USE_LOG

	pEffectSetInstance->m_bHyper = bHyper;
	pEffectSetInstance->m_fPowerRate = fPowerRate;
	pEffectSetInstance->m_fLifeTime = fLifeTime;
	pEffectSetInstance->m_vScale = vScale;

/// 각 파티클 클래스 내에서 유닛 스케일을 적용해 주므로, 중복 적용 구문 제거



	pEffectSetInstance->m_bCustomPosition = bCustomPosition;
	if( true == bCustomPosition )
	{
		pEffectSetInstance->m_vPosition			= vPosition;
		pEffectSetInstance->m_vRotationDegree	= vRotationDegree;
		pEffectSetInstance->m_vDirVector		= vDirVector;
		pEffectSetInstance->m_vZVector			= CX2EffectSet::GetZVector( vDirVector );
	}
	else
		pEffectSetInstance->m_vPosition = pGameUnit->GetPos();	// 커스텀포지션이 아닌 경우 이펙트를 발생 시킨 유저의 위치를 넣어줌

	return pEffectSetInstance->m_hHandle;
}


CX2EffectSet::Handle CX2EffectSet::PlayEffectSet( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( pSquareUnit == NULL )
		return INVALID_EFFECTSET_HANDLE;

	if( pSquareUnit->GetUnitViewer() == NULL )
		return INVALID_EFFECTSET_HANDLE;



	CKTDGXSkinAnimPtr skinAnimPtr = pSquareUnit->GetUnitViewer()->GetXSkinAnimPtr();
	if( skinAnimPtr == NULL )
		return INVALID_EFFECTSET_HANDLE;

    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, skinAnimPtr, NULL, NULL, pSquareUnit );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;

#if X2_DEV_TOOLS
	if( NULL != g_pMain->GetGameEdit() &&
		CX2GameEdit::ELL_EFFECT_LOG_1 <= g_pMain->GetGameEdit()->GetEffectLogLevel() )
	{
		dbg::clog << L"(이펙트셋)" << wstrEffectSetName << dbg::endl;
		KLOG("EffectLog.txt")  << L"EffectSet : " << wstrEffectSetName.c_str() << fileout;
	}
#endif // EFFECT_USE_LOG

	return pEffectSetInstance->m_hHandle;
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetCustomPos( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, const D3DXVECTOR3& vPos )
{
	CX2EffectSet::Handle hEffectset = PlayEffectSet(wstrEffectSetName, pSquareUnit );
	if( INVALID_EFFECTSET_HANDLE != hEffectset )
	{
		EffectSetInstance* pEffect = g_pData->GetGameEffectSet()->GetEffectSetInstance( hEffectset );
        if( NULL != pEffect )
			pEffect->SetEffectPosition( vPos );
	}

	return hEffectset;
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByPet_LUA( const char* strEffectSetName, CX2PET* pPet )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, strEffectSetName );

	return PlayEffectSetByPet( wstrEffectSetName, pPet );
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByPet( const wstring& wstrEffectSetName, CX2PET* pPet )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( pPet == NULL )
		return INVALID_EFFECTSET_HANDLE;

	CKTDGXSkinAnim* skinAnim = pPet->GetXSkinAnim();
	if( skinAnim == NULL )
		return INVALID_EFFECTSET_HANDLE;

	CKTDGXSkinAnimPtr tempPtr;


    EffectSetInstance* pEffectSetInstance = _CreateEffectSetInstance( pEffectSetData, tempPtr, NULL, NULL, CX2SquareUnitPtr(), pPet );
    if ( pEffectSetInstance == NULL )
        return INVALID_EFFECTSET_HANDLE;

	pEffectSetInstance->m_bHyper = false;
	pEffectSetInstance->m_fPowerRate = -1.f;
	pEffectSetInstance->m_fLifeTime = -1.f;
	pEffectSetInstance->m_vScale = D3DXVECTOR3( 1.f, 1.f, 1.f );

	return pEffectSetInstance->m_hHandle;
}




void CX2EffectSet::StopEffectSet( CX2EffectSet::Handle& hHandle )
{
	if( INVALID_EFFECTSET_HANDLE == hHandle )
		return;


    DWORD   dwHandle = hHandle.GetValue();
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    if ( DecomposeHandle( dwHandle, wIndex, wStamp ) == false )
    {
        ASSERT( 0 );
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
    {
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    if ( info.m_pInstance == NULL || info.m_wStamp != wStamp )
    {
        hHandle = INVALID_EFFECTSET_HANDLE;
        return;
    }
    EffectSetInstance* pEffectSetInstance = info.m_pInstance;
    info.m_pInstance = NULL;
    m_coInstanceHandleList.splice( m_coInstanceHandleList.begin( LIST_FREE ), wIndex );
    hHandle = INVALID_EFFECTSET_HANDLE;
    SAFE_DELETE( pEffectSetInstance );

}

void CX2EffectSet::StopEffectSetAll()
{
    KInstanceHandleList::iterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    KInstanceHandleList::iterator iterNext;
    for( KInstanceHandleList::iterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        iter = iterNext )
    {
        iterNext = iter; ++iterNext;
        KInstanceHandleInfo& info = *iter;
        EffectSetInstance* pESInstance = info.m_pInstance;
        info.m_pInstance = NULL;
        SAFE_DELETE( pESInstance );
    }
    m_coInstanceHandleList.splice_list( m_coInstanceHandleList.begin( LIST_FREE ), LIST_LIVE );
}






CX2EffectSet::EffectSetInstance* CX2EffectSet::GetEffectSetInstance( CX2EffectSet::Handle hHandle )
{
	if( INVALID_EFFECTSET_HANDLE == hHandle )
		return NULL;


    DWORD   dwHandle = hHandle.GetValue();
    WORD    wIndex = 0;
    WORD    wStamp = 0;
    if ( DecomposeHandle( dwHandle, wIndex, wStamp ) == false )
    {
        ASSERT( 0 );
        return NULL;
    }
    if ( wIndex >= m_coInstanceHandleList.storage_size() )
        return NULL;
    KInstanceHandleInfo& info = m_coInstanceHandleList.data( wIndex );
    EffectSetInstance* pInstance = info.m_pInstance;
    if ( pInstance == NULL || info.m_wStamp != wStamp )
        return NULL;
    return  pInstance;

}









CX2EffectSet::EffectSetData* CX2EffectSet::GetEffectSetTemplet( const wstring& wstrEffectSetName )
{
	if( true == wstrEffectSetName.empty() )
		return NULL;

	EffectSetDataMap::iterator it = m_mapEffectSetTemplet.find( wstrEffectSetName );
	if( it != m_mapEffectSetTemplet.end() )
	{
		return it->second;
	}

	return NULL;
}



void CX2EffectSet::RegisterLuaBind()
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pEffectSet",		this );
}


bool CX2EffectSet::OpenScriptFile( const wstring& wstrFileName )
{
	RegisterLuaBind();

	if( true == wstrFileName.empty() )
		return false;

	return g_pKTDXApp->LoadLuaTinker( wstrFileName.c_str() );
}

bool CX2EffectSet::AddEffectSetTemplet_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


	EffectSetData* pEffectSetData = new EffectSetData;
	LUA_GET_VALUE( luaManager,		"NAME",			pEffectSetData->m_wstrEffectSetName,			L""		);

	ASSERT( false == pEffectSetData->m_wstrEffectSetName.empty() );
	if( true == pEffectSetData->m_wstrEffectSetName.empty() )
		goto effect_set_error_proc;


	ASSERT( NULL == GetEffectSetTemplet( pEffectSetData->m_wstrEffectSetName ) );
	if( NULL != GetEffectSetTemplet( pEffectSetData->m_wstrEffectSetName ) )
		goto effect_set_error_proc;



	LUA_GET_VALUE_ENUM( luaManager,		"EVENT_TIMER_TYPE",			pEffectSetData->m_eEventTimerType,			EFFECT_SET_TIMER_TYPE,	ESTT_ANIM );
	LUA_GET_VALUE( luaManager,		"AFFECTED_BY_UNIT_SCALE",		pEffectSetData->m_bAffectedByUnitScale,		true );

	//LUA_GET_VALUE( luaManager,		"DELETE_WHEN_TRACE_TARGET_DEAD",	pEffectSetData->m_bDeleteWhenTraceTargetDead,				false	);
	LUA_GET_VALUE( luaManager,		"DELETE_SHAKE_COUNT",				pEffectSetData->m_iDeleteShakeCount,						-1	);



	// damageeffect, meshplayer, particle
	int iEffectTableIndex;	// split: a goto jumps over it
	iEffectTableIndex = 0;
	while( true == luaManager.BeginTable( "EFFECT", iEffectTableIndex ) )
	{
		EffectData* pEffectData = new EffectData;

		// 공통 스크립트
		LUA_GET_VALUE_ENUM( luaManager, "EFFECT_TYPE",				pEffectData->m_eEffectType,					EFFECT_TYPE,	ET_INVALID		);
		if( ET_INVALID == pEffectData->m_eEffectType )
			goto effect_error_proc;

		LUA_GET_VALUE( luaManager,		"NAME",						pEffectData->m_EffectName,					L""		);
		ASSERT( false == pEffectData->m_EffectName.empty() );
		if( true == pEffectData->m_EffectName.empty() )
			goto effect_error_proc;

		LUA_GET_VALUE( luaManager,		"HYPER_NAME",				pEffectData->m_EffectNameHyper,				L""		);



		LUA_GET_VALUE( luaManager,		"START_TIME",				pEffectData->m_fStartAnimTime,				0.f		);
		LUA_GET_VALUE( luaManager,		"BONE_NAME",				pEffectData->m_PositionBoneName,			L""		);

		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_NAME",			pEffectData->m_PositionWeaponBoneName,		L""		);
		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_WEAPON_INDEX",	pEffectData->m_iWeaponIndex,				0		);
		LUA_GET_VALUE( luaManager,		"WEAPON_BONE_MODEL_INDEX",	pEffectData->m_iModelIndex,					0		);

		LUA_GET_VALUE( luaManager,		"RIDING_PET_BONE_NAME",		pEffectData->m_PosRidingPetBoneName,		L""		);

		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_X",		pEffectData->m_vPositionOffset.x,			0.f		);
		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_Y",		pEffectData->m_vPositionOffset.y,			0.f		);
		LUA_GET_VALUE( luaManager,		"POSITION_OFFSET_Z",		pEffectData->m_vPositionOffset.z,			0.f		);

		LUA_GET_VALUE( luaManager,		"TRACE",					pEffectData->m_bTrace,						false	);
		LUA_GET_VALUE( luaManager,		"TRACE_MORE",				pEffectData->m_bTraceMore,					false	);

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		LUA_GET_VALUE( luaManager,		"TRACE_MATRIX",				pEffectData->m_bTraceMatrix,					false	);
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

		LUA_GET_VALUE( luaManager,		"REVERSE_Y",				pEffectData->m_bReverseY,					false	);


		LUA_GET_VALUE( luaManager,		"USE_LAND_POSITION",		pEffectData->m_bLandPosition,				false	);
		LUA_GET_VALUE( luaManager,		"ROTATE_X",					pEffectData->m_vRotateRel.x,				0.f		);
		LUA_GET_VALUE( luaManager,		"ROTATE_Y",					pEffectData->m_vRotateRel.y,				0.f		);
		LUA_GET_VALUE( luaManager,		"ROTATE_Z",					pEffectData->m_vRotateRel.z,				0.f		);
		LUA_GET_VALUE( luaManager,		"APPLY_UNIT_ROTATION",		pEffectData->m_bApplyUnitRotation,			true	);

		/// 회전값 설정시 좌 우 완벽한 대칭을 이루기 위해, 방향에 따른 x회전 값 반전 기능 추가
		LUA_GET_VALUE( luaManager,		"APPLY_UNIT_ROTATION_MIRROR",	pEffectData->m_bApplyUnitRotationMirror, false );

		/// 위의 기능이 설정되어 있다면, APPLY_UNIT_ROTATION은 해제
		if ( true == pEffectData->m_bApplyUnitRotationMirror )
			pEffectData->m_bApplyUnitRotation = false;


		LUA_GET_VALUE( luaManager,		"APPLY_INSTANCE_SCALE",		pEffectData->m_bApplyInstanceScale,			true	);	// 주의!! templet의 scale을 적용할지 말지 정하는 것이 아니라, instance에 지정된 scale을 적용할지 말지 결정하는 변수 입니다

		LUA_GET_VALUE( luaManager,		"SCALE_X",					pEffectData->m_vScale.x,					0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Y",					pEffectData->m_vScale.y,					0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Z",					pEffectData->m_vScale.z,					0.f		);


		LUA_GET_VALUE( luaManager,		"SCALE_X_ON_LEFT",			pEffectData->m_vScaleOnLeft.x,				0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Y_ON_LEFT",			pEffectData->m_vScaleOnLeft.y,				0.f		);
		LUA_GET_VALUE( luaManager,		"SCALE_Z_ON_LEFT",			pEffectData->m_vScaleOnLeft.z,				0.f		);




		LUA_GET_VALUE( luaManager,		"START_WHEN_TRACE_TARGET_DEAD",		pEffectData->m_bStartWhenTraceTargetDead,		false	);
		LUA_GET_VALUE( luaManager,		"PASSIVE_EFFECT",		pEffectData->m_bPassiveEffect,		false	);

		LUA_GET_VALUE( luaManager,		"USE_SUB_ATTACK_LIST_SET",		pEffectData->m_bUseSubAttackListSet,		false	);

		/// 파티클에 대하여 Set Over UI 기능 추가
		LUA_GET_VALUE( luaManager,		"SET_OVER_UI",					pEffectData->m_bSetOverUI,					false	);

		/*
			BONE_TRACE_TARGET_MESH로 설정되어 있는 EffectSet은 절대로 TRACE_MATRIX를 사용할 수 없다.
			다만, TRACE_USER_TARGET_FORCE가 설정되어 있다면 위의 두 기능을 사용할 수 있다.
			BONE_TRACE_TARGET_MESH를 설정하면 해당 이펙트 셋은 무조건 타겟 매시를 Trace 하는데,
			TRACE_USER_TARGET_FORCE 를 통해 해당 유닛을 Trace 할 수 있게 된다.
		*/

		LUA_GET_VALUE( luaManager,		"BONE_TRACE_TARGET_MESH",		pEffectData->m_bBoneTraceTargetMesh,		false	);

		LUA_GET_VALUE( luaManager,		"TRACE_USER_TARGET_FORCE",		pEffectData->m_bTraceUserTargetForce,		false	);

		/// BONE_TRACE_TARGET_MESH와 TRACE_MATRIX가 설정 되어 있는데, TRACE_USER_TARGET_FORCE가 설정 되어 있지 않다면 경고 발생
		if ( false  == pEffectData->m_bBoneTraceTargetMesh && true == pEffectData->m_bTraceUserTargetForce )
		{
			/// TRACE_USER_TARGET_FORCE는 BONE_TRACE_TARGET_MESH 에서만 쓸 수 있어요!
			//ASSERT( !"TRACE_USER_TARGET_FORCE is Only Can Use BONE_TRACE_TARGET_MESH !!!" );

		}

		/// BONE_TRACE_TARGET_MESH와 TRACE_MATRIX가 설정 되어 있는데, TRACE_USER_TARGET_FORCE가 설정 되어 있지 않다면 경고 발생
		if ( true  == pEffectData->m_bBoneTraceTargetMesh && true == pEffectData->m_bTraceMatrix &&
			 false == pEffectData->m_bTraceUserTargetForce )
		{
			/// BONE_TRACE_TARGET_MESH는 TRACE_MATRIX를 사용할 수 없어요!
			//ASSERT( !"BONE_TRACE_TARGET_MESH is Do Not Use TRACE_MATRIX !!!" );
		}

		float fLimitDistance;	// split: a goto jumps over it
		fLimitDistance = -1.f;
		LUA_GET_VALUE( luaManager,		"LIMIT_DISTANCE",				fLimitDistance,				-1.f	);
		if( 0.f < fLimitDistance )
		{
			pEffectData->m_fLimitDistanceSq = fLimitDistance * fLimitDistance;
		}

		// 이펙트 종류별 script
		switch( pEffectData->m_eEffectType )
		{
		case ET_DAMAGE_EFFECT:
			{
				ASSERT( !pEffectData->m_bUseSubAttackListSet );	/// MeshPlayer만 사용가능
				/// 파티클에 대하여 Set Over UI 기능 추가
				ASSERT( !pEffectData->m_bSetOverUI );			/// Particle만 사용가능

			} break;
		case ET_MESH_PLAYER_UI_MAJOR:
		case ET_MESH_PLAYER_UI_MINOR:
		case ET_MESH_PLAYER_GAME_MAJOR:
		case ET_MESH_PLAYER_GAME_MINOR:
			{
				/// 파티클에 대하여 Set Over UI 기능 추가
				ASSERT( !pEffectData->m_bSetOverUI );			/// Particle만 사용가능
			} break;
		case ET_PARTICLE_UI_MAJOR:
		case ET_PARTICLE_UI_MINOR:
		case ET_PARTICLE_GAME_MAJOR:
		case ET_PARTICLE_GAME_MINOR:
			{
				ASSERT( !pEffectData->m_bUseSubAttackListSet );	/// MeshPlayer만 사용가능
				ASSERT( !pEffectData->m_bBoneTraceTargetMesh );
			} break;
		default:
			break;
		}


		pEffectSetData->m_vecpEffectData.push_back( pEffectData );
		luaManager.EndTable();		// EFFECT(iEffectTableIndex)
		++iEffectTableIndex;
		continue;

effect_error_proc:
		SAFE_DELETE( pEffectData );
		luaManager.EndTable();		// EFFECT(iEffectTableIndex)
		++iEffectTableIndex;
	}

	// camera shake
	int iCameraShakeTableIndex;	// split: a goto jumps over it
	iCameraShakeTableIndex = 0;
	while( true == luaManager.BeginTable( "CAMERA_SHAKE", iCameraShakeTableIndex ) )
	{
		CameraShakeData* pCameraShakeData = new CameraShakeData;

		LUA_GET_VALUE_ENUM( luaManager, "SHAKE_TYPE",				pCameraShakeData->m_eShakeType,				CKTDGCamera::SHAKE_TYPE,	CKTDGCamera::DECT_INVALID	);
		if( CKTDGCamera::DECT_INVALID == pCameraShakeData->m_eShakeType )
			goto camera_shake_error_proc;

		LUA_GET_VALUE( luaManager,		"START_TIME",				pCameraShakeData->m_fStartAnimTime,				0.f		);

		/// 카메라 크래시 발생 거리제한
		int iLimitDistance;	// split: a goto jumps over it
		iLimitDistance = 0;
		LUA_GET_VALUE( luaManager,		"LIMIT_DISTANCE",			iLimitDistance,			0		);
		if ( IGNORE_DISTANCE == iLimitDistance )	//ignore인 경우에 해당 값을 그대로 대입
			pCameraShakeData->m_iLimitDistanceSq = IGNORE_DISTANCE;
		else if ( 0 != iLimitDistance )	// 입력 값이 있는 경우만 해당 값을 제곱하여 대입
			pCameraShakeData->m_iLimitDistanceSq = iLimitDistance * iLimitDistance;

		LUA_GET_VALUE( luaManager,		"ONE_DIR_SPEED",			pCameraShakeData->m_fOneDirSpeed,				0.f		);
		LUA_GET_VALUE( luaManager,		"ONE_DIR_ACCEL",			pCameraShakeData->m_fOneDirAccel,				0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_GAP",				pCameraShakeData->m_fTwoDirGap,					0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_TIME",				pCameraShakeData->m_fTwoDirTime,				0.f		);
		LUA_GET_VALUE( luaManager,		"TWO_DIR_TIME_GAP",			pCameraShakeData->m_fTwoDirTimeGap,				0.05f	);


		LUA_GET_VALUE( luaManager,		"FIXED_LOOK_VEC",			pCameraShakeData->m_bFixedLookVec,				false	);
		LUA_GET_VALUE( luaManager,		"EYE_SHAKE_COUNT",			pCameraShakeData->m_iEyeShakeCount,				0 );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_SPEED_MIN",		pCameraShakeData->m_EyeMoveSpeed.m_Min,			0.f );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_SPEED_MAX",		pCameraShakeData->m_EyeMoveSpeed.m_Max,			0.f );

		LUA_GET_VALUE( luaManager,		"EYE_MOVE_RANGE_MIN",		pCameraShakeData->m_EyeMoveRange.m_Min,			0.f );
		LUA_GET_VALUE( luaManager,		"EYE_MOVE_RANGE_MAX",		pCameraShakeData->m_EyeMoveRange.m_Max,			0.f );

		LUA_GET_VALUE( luaManager,		"LOOK_AT_SHAKE_COUNT",		pCameraShakeData->m_iLookAtShakeCount,			0 );
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_SPEED_MIN",	pCameraShakeData->m_LookAtMoveSpeed.m_Min,		0.f	);
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_SPEED_MAX",	pCameraShakeData->m_LookAtMoveSpeed.m_Max,		0.f	);

		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_RANGE_MIN",	pCameraShakeData->m_LookAtMoveRange.m_Min,		0.f	);
		LUA_GET_VALUE( luaManager,		"LOOK_AT_MOVE_RANGE_MAX",	pCameraShakeData->m_LookAtMoveRange.m_Max,		0.f	);

		pEffectSetData->m_vecpCameraShakeData.push_back( pCameraShakeData );
		luaManager.EndTable();		// EFFECT(iCameraShakeTableIndex)
		++iCameraShakeTableIndex;
		continue;

camera_shake_error_proc:
		SAFE_DELETE( pCameraShakeData );
		luaManager.EndTable();		// EFFECT(iCameraShakeTableIndex)
		++iCameraShakeTableIndex;
	}




	m_mapEffectSetTemplet[ pEffectSetData->m_wstrEffectSetName ] = pEffectSetData;

	return true;

effect_set_error_proc:
	SAFE_DELETE( pEffectSetData );
	return false;
}






//////////////////////////////////////////////////////////////////////////
CX2EffectSet::EffectInstance::~EffectInstance()
{
	switch( m_pEffectData->m_eEffectType )
	{
	default:
		{
			ASSERT( !"no type effect" );
		} break;

	case ET_DAMAGE_EFFECT:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetDamageEffect() )
			{
                if ( m_hDamageEffect != INVALID_DAMAGE_EFFECT_HANDLE )
                    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffect );
			}
		} break;

	case ET_MESH_PLAYER_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pData->GetUIMinorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_MESH_PLAYER_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorXMeshPlayer() )
			{
				if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshPlayer )
				{
					if ( m_pEffectData->m_bUseSubAttackListSet && null != m_optrGameUnit )
						m_optrGameUnit->ErasePairSubAttackListSet( m_hMeshPlayer );

					g_pX2Game->GetMinorXMeshPlayer()->DestroyInstanceHandle( m_hMeshPlayer );
				}
			}
		} break;

	case ET_PARTICLE_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pData->GetUIMinorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;

	case ET_PARTICLE_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorParticle() )
			{
				if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleSequence )
				{
					g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hParticleSequence );
				}
			}
		} break;
	}
}



//////////////////////////////////////////////////////////////////////////
CX2EffectSet::EffectSetInstance::~EffectSetInstance()
{

	//if( NULL != g_pX2Game )
	//{
	//	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqStickShake );
	//}

    m_hHandle = INVALID_EFFECTSET_HANDLE;

	for( EffectInstance* pEffectInstance : m_vecpEffectInstance )
	{
		SAFE_DELETE( pEffectInstance );
	}
	m_vecpEffectInstance.clear();
}

CX2EffectSet::EffectSetInstance::EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit, CX2PET *pPet)
: m_hHandle( INVALID_EFFECTSET_HANDLE )
, m_pXSkinAnim( pSkinAnim )
, m_optrGameUnit( pGameUnit )
, m_optrTraceTargetGameUnit( pTraceTargetGameUnit )
, m_pSquareUnit( pSquareUnit )
, m_bHyper( false )
, m_bCustomPosition( false )
, m_vPosition( 0, 0, 0 )
, m_vRotationDegree( 0, 0, 0 )
, m_vDirVector( 0, 0, 0 )
, m_vZVector( 0, 0, 0 )
, m_fDelayTime( 0.f )
, m_fPowerRate( 0.f )
, m_fPowerRateScale( 1.f )
, m_fLifeTime( 0.f )
, m_vScale( 1, 1, 1 )
, m_bDamageEffectScale( false )
, m_pEffectSetData( pEffectSetData )
, m_fPrevElapsedTime( 0.f )
, m_fElapsedTime( 0.f )
, m_bUseAnimationTiming( true )
, m_iCurrDeleteShakeCount( 0 )
, m_bCreateEffectSetTraceUnit( false )
, m_bTraceUnitDieDeleteEffectSet( true )
, m_pPet( pPet )
, m_vLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_hTargetMesh ( INVALID_MESH_INSTANCE_HANDLE )
, m_eTargetMeshEffectType(ET_INVALID)
, m_bIsTargetMesh( false )
, m_bIsTargetMeshRight( false )
, m_vecTargetMeshDirVector( D3DXVECTOR3(1.f, 0.f, 0.f) )
, m_vecTargetMeshZVector( D3DXVECTOR3(1.f, 0.f, 0.f) )
//, m_hSeqStickShake( INVALID_PARTICLE_SEQUENCE_HANDLE )
, m_vBoneTraceTargetMeshScale( 1.f, 1.f, 1.f )
{
//	if( NULL == pEffectSetData )
//		return;

// #ifdef SERV_PET_SYSTEM
// 	if( pPet == NULL && pSkinAnim == NULL )
// 		return;
// #else
// 	if( pSkinAnim == NULL )
// 		return;
// #endif

	if( NULL != pGameUnit )
	{
		m_eMode = ESM_GAME;
	}
	else if( pSquareUnit != NULL )
	{
		m_eMode = ESM_FIELD;
	}
	else if( pPet != NULL )
	{
		m_eMode = ESM_COMMON;
	}
	else
	{
		m_eMode = ESM_FIELD;
		//return;
	}


	m_vecEffectInstanceCreated.resize(0);
	for( UINT i=0; i<pEffectSetData->m_vecpEffectData.size(); i++ )
	{
		m_vecEffectInstanceCreated.push_back( false );
	}

	m_vecCameraShakeDone.resize(0);
	for( UINT i=0; i<pEffectSetData->m_vecpCameraShakeData.size(); i++ )
	{
		m_vecCameraShakeDone.push_back( false );
	}

    m_hHandle = INVALID_EFFECTSET_HANDLE;
}

D3DXVECTOR3 CX2EffectSet::GetEffectPosition_LUA( CX2EffectSet::Handle hHandle, int iEffectIndex )
{
	D3DXVECTOR3 vPos( 0, 0, 0 );

	EffectSetInstance* pEffectSetInstance = GetEffectSetInstance( hHandle );
	if( pEffectSetInstance == NULL )
		return vPos;

	EffectData* pEffectData = pEffectSetInstance->m_pEffectSetData->m_vecpEffectData[iEffectIndex];
	if( pEffectData == NULL )
		return vPos;

	vPos = pEffectSetInstance->GetEffectPosition( pEffectData );

	return vPos;
}

D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectPosition( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vPos(0, 0, 0);

	if( NULL == pEffectData )
		return vPos;

	if( true == GetEffectPositionByTargetMesh( vPos, pEffectData ) )
	{
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
		return vPos;
	}

	if( true == m_bCustomPosition )
	{
		vPos = m_vPosition;
		SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
		return vPos;
	}



	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return vPos;

			if( null != m_optrTraceTargetGameUnit )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					if( 0 == pEffectData->m_PositionBoneName.compare( L"_HEAD_BONE" ) )
					{
						vPos = m_optrTraceTargetGameUnit->GetHeadBonePos();
					}
					else
					{
						m_optrTraceTargetGameUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos );
					}
				}
				else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
				{
					vPos = m_optrTraceTargetGameUnit->GetWeaponBonePos( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
				}
				else
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_optrTraceTargetGameUnit->GetLandPos();
					}
					else
					{
						vPos = m_optrTraceTargetGameUnit->GetPos();
					}
				}

				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}
			else if( null != m_optrGameUnit )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					if( 0 == pEffectData->m_PositionBoneName.compare( L"_HEAD_BONE" ) )
					{
						vPos = m_optrGameUnit->GetHeadBonePos();
					}
					// 없는 본에 대한 참조를 시도 한다면 헤드본으로 연결 하도록 수정
					else if( false == m_optrGameUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos ) )
					{
						vPos = m_optrGameUnit->GetHeadBonePos();
					}
				}
				else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
				{
					vPos = m_optrGameUnit->GetWeaponBonePos( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
				}
				else if ( NULL != GetOwnerUnit() && NULL != GetOwnerUnit()->GetRidingPet() && false == pEffectData->m_PosRidingPetBoneName.empty() )
				{
					GetOwnerUnit()->GetRidingPet()->GetBonePos( pEffectData->m_PosRidingPetBoneName.c_str(), vPos ) ;
				}
				else
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_optrGameUnit->GetLandPos();
					}
					else
					{
						vPos = m_optrGameUnit->GetPos();
					}
				}

				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}
			else
			{
				return vPos;
			}
		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{

				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					vPos = m_pSquareUnit->GetBonePos( pEffectData->m_PositionBoneName.c_str() );
				}
				else
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_pSquareUnit->GetLandPosition();
					}
					else
					{
						vPos = m_pSquareUnit->GetPos();
					}
				}

				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}

		} break;

	case ESM_COMMON:
		{
			if( m_pPet != NULL )
			{
				if( false == pEffectData->m_PositionBoneName.empty() )
				{
					vPos = m_pPet->GetBonePos( pEffectData->m_PositionBoneName.c_str() );
				}
				else
				{
					if( true == pEffectData->m_bLandPosition )
					{
						vPos = m_pPet->GetLandPosition_LUA();
					}
					else
					{
						vPos = m_pPet->GetPos();
					}
				}

				SetEffectSetOffset( vPos, pEffectData->m_vPositionOffset, pEffectData );
			}
		}
		break;
	default:
		break;
	}

	return vPos;
}


D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectScale( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vScale( 1, 1, 1 );

	if( 0.f != pEffectData->m_vScale.x )
	{
		vScale.x *= pEffectData->m_vScale.x;
		vScale.y *= pEffectData->m_vScale.y;
		vScale.z *= pEffectData->m_vScale.z;
	}

	if( 0.f != pEffectData->m_vScaleOnLeft.x &&
		false == IsUnitRight() )
	{
		vScale.x *= pEffectData->m_vScaleOnLeft.x;
		vScale.y *= pEffectData->m_vScaleOnLeft.y;
		vScale.z *= pEffectData->m_vScaleOnLeft.z;
	}

	//if( true == pEffectData->m_bApplyInstanceScale ) : 원본
	// UnitScale의 중복 곱을 피하기 위해 Effectset을 타고온 DamageEffect는 UnitScale을 적용하지 않는다.
	if( m_bDamageEffectScale == true || (true == pEffectData->m_bApplyInstanceScale && pEffectData->m_eEffectType != ET_DAMAGE_EFFECT) )
	{
		vScale.x *= m_vScale.x;
		vScale.y *= m_vScale.y;
		vScale.z *= m_vScale.z;
	}

	return vScale;
}



D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetEffectRotation( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vEffectRotation = pEffectData->m_vRotateRel;
	if( true == pEffectData->m_bApplyUnitRotation )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
			if( NULL != pMeshInstance )
			{
				vEffectRotation += pMeshInstance->GetRotateDegree();
			}
		}
		else
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}
	}
	/// 회전값 설정시 좌 우 완벽한 대칭을 이루기 위해, 방향에 따른 x회전 값 반전 기능 추가
	else if ( true == pEffectData->m_bApplyUnitRotationMirror )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
			if( NULL != pMeshInstance )
			{
				vEffectRotation += pMeshInstance->GetRotateDegree();
			}
		}
		else
		{
			vEffectRotation += GetUnitRotation( pEffectData );
		}

		if( false == IsUnitRight() )
		{
			vEffectRotation.x *= -1.f;
		}
	}

	vEffectRotation += m_vLocalRotateDegree;

	return vEffectRotation;
}

D3DXVECTOR3 CX2EffectSet::EffectSetInstance::GetUnitRotation( EffectData* pEffectData )
{
	KTDXPROFILE();

	D3DXVECTOR3 vRotation( 0, 0, 0 );

	if( NULL == m_pPet && NULL == pEffectData )
		return vRotation;


	if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
		if( NULL != pMeshInstance )
		{
			pMeshInstance->GetRotateDegree();
		}
	}


	if( true == m_bCustomPosition )
	{
		return m_vRotationDegree;
	}

	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return vRotation;

			if( null != m_optrTraceTargetGameUnit )
			{
				return m_optrTraceTargetGameUnit->GetRotateDegree();
			}
			else if ( null != m_optrGameUnit )
			{
				return m_optrGameUnit->GetRotateDegree();
			}
			else
				return vRotation;

		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{
				return m_pSquareUnit->GetDirDegree();
			}

		} break;
	case ESM_COMMON:
		{
			if( m_pPet != NULL )
			{
				return m_pPet->GetDirDegree();
			}
		} break;
	default:
		break;
	}

	return vRotation;
}


bool CX2EffectSet::EffectSetInstance::IsUnitRight()
{
	KTDXPROFILE();

	switch( m_eMode )
	{
	case ESM_GAME:
		{
			if( NULL == g_pX2Game )
				return false;

			if( null != m_optrTraceTargetGameUnit )
			{
				return m_optrTraceTargetGameUnit->GetIsRight();
			}
			else if( null != m_optrGameUnit )
			{
				return m_optrGameUnit->GetIsRight();
			}
			else
				return false;

		} break;

	case ESM_FIELD:
		{
			if( m_pSquareUnit != NULL )
			{
				if( NULL == g_pTFieldGame )
					return false;

				return m_pSquareUnit->GetIsRight();
			}
		} break;
	default:
		break;
	}

	return false;
}

//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
const D3DXMATRIX* CX2EffectSet::EffectSetInstance::GetCombineMatrix( const EffectData* pEffectData ) const
{
	if( ESM_GAME == m_eMode && null != m_optrGameUnit )
	{
		if( false == pEffectData->m_PositionBoneName.empty() )
		{
			/// BONE_TRACE_TARGET_MESH가 설정 되어 있다면, 해당 매시의 본을 사용
			if( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData->m_bTraceUserTargetForce &&
				 NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );

				if ( NULL != pMesh  )
				{
					CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = pMesh->GetCloneFrame( pEffectData->m_PositionBoneName.c_str() );

					if ( NULL != pMultiAnimFrame )
						return &pMultiAnimFrame->combineMatrix;
				}
			}
			else
				return m_optrGameUnit->GetCombineMatrixFromBoneName( pEffectData->m_PositionBoneName );
		}
		else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromWeaponBoneName( pEffectData->m_iWeaponIndex, pEffectData->m_PositionWeaponBoneName.c_str(), pEffectData->m_iModelIndex );
		}
		else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
		{
			return m_optrGameUnit->GetCombineMatrixFromRidingPetBoneName( pEffectData->m_PosRidingPetBoneName );
		}
	}

	return NULL;
}

//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

CX2GameUnit* CX2EffectSet::EffectSetInstance::GetOwnerUnit() const
{
	{
		if( null != m_optrGameUnit )
			return m_optrGameUnit.GetObservable();
		else if( NULL != m_pPet )
			return m_pPet->GetMaster();
		else
			return NULL;
	}
}

/*static*/ D3DXVECTOR3 CX2EffectSet::GetZVector( const D3DXVECTOR3& vDirVector )
{
	D3DXVECTOR3 vZVector = vDirVector;
	if( vZVector.y != 0.f )
	{
		vZVector.y = vZVector.z;
		vZVector.z = vZVector.x;
		vZVector.x = -vZVector.y;
		vZVector.y = 0.f;
		D3DXVec3Normalize( &vZVector, &vZVector );
	}
	else
	{
		vZVector.y = vZVector.z;
		vZVector.z = vZVector.x;
		vZVector.x = -vZVector.y;
		vZVector.y = 0.f;
	}
	return vZVector;
}


//{{ kimhc // 2010.4.19 // 비밀던전 작업
CX2EffectSet::Handle CX2EffectSet::PlayEffectSet_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit );
}
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos , D3DXVECTOR3 vRotationDegree)
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit, NULL, false, -1.0f, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
		true, vPos, vRotationDegree);
}

//{{ JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시
CX2EffectSet::Handle CX2EffectSet::PlayEffectSetTraceTarget_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit )
{
	wstring wstrEffectSetName;
	ConvertUtf8ToWCHAR( wstrEffectSetName, pEffectSetName );

	return PlayEffectSet( wstrEffectSetName, pGameUnit, pTraceTargetGameUnit );
}
//}} JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시

void CX2EffectSet::StopEffectSet_LUA( CX2EffectSet::Handle hHandle )
{
	StopEffectSet( hHandle );
}
//}} kimhc // 2010.4.19 // 비밀던전 작업


#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
bool CX2EffectSet::MergeEffectSetTemplet( EffectSetData* pNewEffectSetTemplet_, EffectSetData* pOrgEffectSetTemplet_ )
{
	if ( NULL != pNewEffectSetTemplet_ )
	{
		if ( NULL != pOrgEffectSetTemplet_ )
		{
			m_mapEffectSetTemplet.erase( pOrgEffectSetTemplet_->m_wstrEffectSetName );
			KLOG("DevScriptTable_Log.txt")  << L"Merge : " << pNewEffectSetTemplet_->m_wstrEffectSetName.c_str() << L"\t: EffectSetTemplet 을 변경 하였습니다." << fileout;
		}
		else
		{
			KLOG("DevScriptTable_Log.txt")  << L"Merge : " << pNewEffectSetTemplet_->m_wstrEffectSetName.c_str() << L"\t: EffectSetTemplet 을 추가 하였습니다." << fileout;
		}
		m_mapEffectSetTemplet[pNewEffectSetTemplet_->m_wstrEffectSetName] = pNewEffectSetTemplet_;
		return true;
	}
	return false;
}
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가

#if X2_DEV_TOOLS
// #ifdef EFFECT_TOOL

bool CX2EffectSet::CreateEffectSetTemplet( EffectSetData* pEffectSetData_ )
{
	// 중복 체크.
	EffectSetDataMap::iterator it = m_mapEffectSetTemplet.find( pEffectSetData_->m_wstrEffectSetName );
	if( it != m_mapEffectSetTemplet.end() )
	{
		return false;
	}

	EffectSetData * pNewEffectSetData = new EffectSetData();
	(*pNewEffectSetData ) = (*pEffectSetData_);
	// 맵 추가
	m_mapEffectSetTemplet[ pNewEffectSetData->m_wstrEffectSetName ] = pNewEffectSetData;
	// 벡터 추가.
	return true;
}

#endif // defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

	#if X2_DEV_TOOLS
	bool CX2EffectSet::DeleteEffectSetTemplet( EffectSetData* pEffectSetData_ )
	{
		// 존재 여부 체크.
		EffectSetDataMap::iterator mit = m_mapEffectSetTemplet.find( pEffectSetData_->m_wstrEffectSetName );
		if( mit == m_mapEffectSetTemplet.end() )
		{
			return false;
		}

		//맵 제거.
		m_mapEffectSetTemplet.erase( mit );

		return true;
	}
	#endif // EXPAND_DEVELOPER_SCRIPT

/** @function : SetEffectSetOffset
	@brief : 이펙트셋에 설정된 오프셋값을 출력 위치에 적용 시켜 주는 함수
	@param : vPos_  ( 현재 위치에 대한 위치값. OUT 인자)
	         vOffsetPos_ ( EffectSet에 담겨있는 오프셋 정보 )
*/

void CX2EffectSet::EffectSetInstance::SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_, IN const EffectData* pEffectData_ )
{
	if ( NULL == pEffectData_ )
		return;

	if( null != m_optrGameUnit ) // ESM_GAME
	{
		/// 타겟 매시가 설정되어 있다면, 타겟 매시의 방향에 따라 OffSet 위치를 적용한다.
		if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh &&
			  false == pEffectData_->m_bTraceUserTargetForce )
		{
			if ( false == IsSamef( vOffsetPos_.x, 0.f ) )
			{
				/// 방향벡터의 Y 값을 제거
				D3DXVECTOR3 vDirVector = m_vecTargetMeshDirVector;

				if( 0.f != vDirVector.y )
				{
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}

				/// 방향에 따른 OffSet X 설정
				vPos_ += m_bIsTargetMeshRight ? ( vOffsetPos_.x * vDirVector ) : ( -vOffsetPos_.x * vDirVector );
			}

			/// 방향에 따른 OffSet Y 설정
			if ( false == IsSamef( vOffsetPos_.y, 0.f ) )
				vPos_.y += vOffsetPos_.y;

			/// 방향에 따른 OffSet Z 설정
			if ( false == IsSamef( vOffsetPos_.z, 0.f ) )
				vPos_ += vOffsetPos_.z * m_vecTargetMeshZVector;
		}
		else
		{
			// X축 오프셋 적용
			if( 0.f != vOffsetPos_.x )
			{
				D3DXVECTOR3 vDirVector = m_optrGameUnit->GetDirVector();
				if( 0.f != vDirVector.y )
				{
					vDirVector.y = 0.f;
					D3DXVec3Normalize( &vDirVector, &vDirVector );
				}

				if( true == m_optrGameUnit->GetIsRight() )
					vPos_ += vDirVector * vOffsetPos_.x;
				else
					vPos_ -= vDirVector * vOffsetPos_.x;
			}

			// Y축 오프셋 적용
			if( 0.f != vOffsetPos_.y )
			{
				vPos_.y += vOffsetPos_.y;
			}

			// Z축 오프셋 적용
			if( 0.f != vOffsetPos_.z )
			{
				vPos_ += m_optrGameUnit->GetZVector() * vOffsetPos_.z;
			}
		}
	}
	else if ( null != m_pSquareUnit ) // ESM_FIELD
	{
		// X축 오프셋 적용
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pSquareUnit->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pSquareUnit->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		// Y축 오프셋 적용
		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		// Z축 오프셋 적용
		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pSquareUnit->GetZVector() * vOffsetPos_.z;
		}
	}
	else if( NULL != m_pPet ) // ESM_COMMON
	{
		if( 0.f != vOffsetPos_.x )
		{
			D3DXVECTOR3 vDirVector = m_pPet->GetDirVector();
			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == m_pPet->GetIsRight() )
				vPos_ += vDirVector * vOffsetPos_.x;
			else
				vPos_ -= vDirVector * vOffsetPos_.x;
		}

		if( 0.f != vOffsetPos_.y )
		{
			vPos_.y += vOffsetPos_.y;
		}

		if( 0.f != vOffsetPos_.z )
		{
			vPos_ += m_pPet->GetZVector() * vOffsetPos_.z;
		}
	}

	else if ( INVALID_MESH_INSTANCE_HANDLE != m_hTargetMesh && false == pEffectData_->m_bTraceUserTargetForce )
	{
		vPos_.x += (0.f != vOffsetPos_.x) ? vOffsetPos_.x : 0.f;
		vPos_.y += (0.f != vOffsetPos_.y) ? vOffsetPos_.y : 0.f;
		vPos_.z += (0.f != vOffsetPos_.z) ? vOffsetPos_.z : 0.f;
	}

}


/** @function	: PlayEffectSetByMeshPlayer
	@brief		: 매시에 이펙트 셋을 설정하는 함수
	@param		: 이펫트셋 이름(wstrEffectSetName). 매시 플레이어 객체 핸들(hTargetMesh), 메쉬 이펙트 타입(eTargetMeshType)
	@return		: 이펙트셋 핸들 값
*/

CX2EffectSet::Handle CX2EffectSet::PlayEffectSetByMeshPlayer( const WCHAR* wstrEffectSetName,
															CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh, EFFECT_TYPE	eTargetMeshType )
{
	EffectSetData* pEffectSetData = GetEffectSetTemplet( wstrEffectSetName );
	if( NULL == pEffectSetData )
		return INVALID_EFFECTSET_HANDLE;

	if( INVALID_MESH_INSTANCE_HANDLE == hTargetMesh)
		return INVALID_EFFECTSET_HANDLE;

    KInstanceHandleList::iterator iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
    if ( iterEmpty == m_coInstanceHandleList.end(LIST_FREE) )
    {
        if ( m_coInstanceHandleList.storage_size() >= 0x10000 )
        {
            return INVALID_EFFECTSET_HANDLE;
        }
        m_coInstanceHandleList.push_back_default(LIST_FREE);
        iterEmpty = m_coInstanceHandleList.begin(LIST_FREE);
        ASSERT( iterEmpty != m_coInstanceHandleList.end(LIST_FREE) );
    }

	EffectSetInstance* pEffectSetInstance = new EffectSetInstance( pEffectSetData, CKTDGXSkinAnimPtr(), NULL, NULL, CX2SquareUnitPtr(), NULL );

	// 타겟 메쉬 정보 설정
	pEffectSetInstance->m_hTargetMesh = hTargetMesh;
	pEffectSetInstance->m_eTargetMeshEffectType = eTargetMeshType;
	pEffectSetInstance->m_bIsTargetMesh = true;

    m_coInstanceHandleList.splice( m_coInstanceHandleList.end(LIST_LIVE), iterEmpty );
    iterEmpty->m_pInstance = pEffectSetInstance;
    WORD   wIndex = (WORD) iterEmpty.GetIndex();
    Handle handle;
    DWORD   dwHandle = 0;
    do
    {
        ++iterEmpty->m_wStamp;
        dwHandle = ComposeHandle( wIndex, iterEmpty->m_wStamp );
        handle.SetValue( (int) dwHandle );
    } while ( handle == INVALID_EFFECTSET_HANDLE );
    pEffectSetInstance->SetHandle( handle );
	return pEffectSetInstance->m_hHandle;
}



/** @function : GetMeshInstanceByTargetMesh
	@brief : TargetMesh 의 메쉬 인스턴스 객체 포인터 얻기
			 (타입에 따라 얻어오는 방법이 다르기 때문에 함수로 뺐음)
*/
CKTDGXMeshPlayer::CXMeshInstance* CX2EffectSet::EffectSetInstance::GetMeshInstanceByTargetMesh()
{
	if( INVALID_MESH_INSTANCE_HANDLE == m_hTargetMesh ||
		ET_INVALID == m_eTargetMeshEffectType )
		return NULL;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
	switch( m_eTargetMeshEffectType )
	{
	case ET_MESH_PLAYER_UI_MAJOR:
		{
			if( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				pMeshInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_UI_MINOR:
		{
			if( NULL != g_pData->GetUIMinorXMeshPlayer() )
			{
				pMeshInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_GAME_MAJOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorXMeshPlayer() )
			{
				pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	case ET_MESH_PLAYER_GAME_MINOR:
		{
			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMinorXMeshPlayer() )
			{
				pMeshInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hTargetMesh );
			}
		} break;
	default:
		break;
	}

	return pMeshInstance;
}
bool CX2EffectSet::EffectSetInstance::GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData )
{
	if( INVALID_MESH_INSTANCE_HANDLE == m_hTargetMesh ||
		true == pEffectData->m_bTraceUserTargetForce ||
		ET_INVALID == m_eTargetMeshEffectType )
		return false;

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = GetMeshInstanceByTargetMesh();
	if( NULL != pMeshInstance )
	{
		if( false == pEffectData->m_PositionBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PositionBoneName.c_str(), vPos ) ;
		}
		else if( false == pEffectData->m_PositionWeaponBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PositionWeaponBoneName.c_str(), vPos ) ;
		}
		else if ( false == pEffectData->m_PosRidingPetBoneName.empty() )
		{
			pMeshInstance->GetBonePos( pEffectData->m_PosRidingPetBoneName.c_str() );
		}
		else
		{
			vPos = pMeshInstance->GetPos();
		}
	}
	else
	{
		m_hTargetMesh = INVALID_MESH_INSTANCE_HANDLE;
		m_eTargetMeshEffectType = ET_INVALID;
		return false;
	}

	return true;
}

/** @function	: SetPos
	@brief		: 이펙트 셋의 위치를 강제로 바꾸는 함수 ( Trace가 설정되어 있다면, 바꿔도 원상태로 돌아옴 )
	@param		: 변경 위치
*/
void CX2EffectSet::EffectSetInstance::SetPos( IN const D3DXVECTOR3& vOffsetPos_ )
{
	for( EffectInstance* pEffectInstance : m_vecpEffectInstance )
	{
		if ( NULL == pEffectInstance )
			continue;

		EffectData* pEffectData = pEffectInstance->m_pEffectData;

		if( NULL == pEffectData )
			continue;

		/// Trace가 설정되어 있다면, 동작 시키지 말자.
		if( true == pEffectData->m_bTrace ||
			true == pEffectData->m_bTraceMore ||
			true == pEffectData->m_bTraceMatrix )
		{
			ASSERT( ! "Can Not Change Position!!! This EffectSet is Setting Trace" );
			continue;
		}

		if( false == pEffectInstance->m_bAlive )
			continue;

		switch( pEffectData->m_eEffectType )
		{
		case ET_DAMAGE_EFFECT:
			{
				if ( NULL != g_pX2Game &&
					 NULL != g_pX2Game->GetDamageEffect() )
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pEffectInstance->m_hDamageEffect );

					if ( NULL != pEffect )
					{
						pEffect->SetPos( vOffsetPos_ );
					}
				}

			} break;

		case ET_MESH_PLAYER_UI_MAJOR:
			{
				if( NULL != g_pData->GetUIMajorXMeshPlayer() &&
					INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( NULL != pInstance )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_MESH_PLAYER_UI_MINOR:
			{
				if( NULL != g_pData->GetUIMinorXMeshPlayer() &&
					INVALID_MESH_INSTANCE_HANDLE != pEffectInstance->m_hMeshPlayer )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pData->GetUIMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( NULL != pInstance )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_MESH_PLAYER_GAME_MAJOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMajorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( pInstance != NULL )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			}
			break;
		case ET_MESH_PLAYER_GAME_MINOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMinorXMeshPlayer() != NULL && pEffectInstance->m_hMeshPlayer != INVALID_MESH_INSTANCE_HANDLE )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pInstance = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance(pEffectInstance->m_hMeshPlayer);
					if( pInstance != NULL )
					{
						pInstance->SetPos( vOffsetPos_ );
					}
				}
			} break;
		case ET_PARTICLE_UI_MAJOR:
			{
				if( g_pData->GetUIMajorParticle() != NULL &&
					pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			} break;

		case ET_PARTICLE_UI_MINOR:
			{
				if( g_pData->GetUIMinorParticle() != NULL &&
					pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			} break;

		case ET_PARTICLE_GAME_MAJOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMajorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			}
			break;

		case ET_PARTICLE_GAME_MINOR:
			{
				if( g_pX2Game != NULL && g_pX2Game->GetMinorParticle() != NULL && pEffectInstance->m_hParticleSequence != INVALID_PARTICLE_SEQUENCE_HANDLE )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( pEffectInstance->m_hParticleSequence );
					if( pSeq != NULL )
					{
						pSeq->SetPosition( vOffsetPos_, pEffectData->m_bTraceMore, pEffectData->m_bTraceMore );
					}
				}
			}
			break;
		default:
			break;
		}
	}
}
