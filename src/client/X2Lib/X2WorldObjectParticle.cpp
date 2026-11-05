#include "stdafx.h"
#include "./X2WorldObjectParticle.h"



CX2WorldObjectParticle::~CX2WorldObjectParticle(void)
{
	if( m_pParticleSystem != NULL )
	{
		m_pParticleSystem->DestroyInstanceHandle( m_hParticleEventSequence );
	}
}

CX2WorldObjectParticle::CX2WorldObjectParticle( CKTDGParticleSystem* pParticleSystem, const char* pszSequenceName, float fDelayTime )
{
	m_ObjectType = CX2WorldObject::OT_PARTICLE;
	m_bCanHide = false;
	m_pParticleSystem			= NULL;
	m_hParticleEventSequence	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_bCanHide		= false;

	m_wstrSequenceName = L"";

	m_fDelayTime = fDelayTime;
	m_bCullCheck = false;
	m_vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );

	if( m_pParticleSystem != NULL &&
		INVALID_PARTICLE_SEQUENCE_HANDLE != m_hParticleEventSequence )
	{
		m_pParticleSystem->DestroyInstanceHandle( m_hParticleEventSequence );
	}

	if( pParticleSystem == NULL )
		return;

	m_pParticleSystem = pParticleSystem;
	wstring wstrSequenceName;
	ConvertCharToWCHAR(wstrSequenceName, pszSequenceName);


	if( m_fDelayTime > 0.f )
	{
		m_wstrSequenceName = wstrSequenceName;
		return;
	}


	D3DXVECTOR3 vPosition = GetMatrix().GetPos();
	m_hParticleEventSequence = m_pParticleSystem->CreateSequenceHandle( NULL,  wstrSequenceName.c_str(), vPosition.x, vPosition.y, vPosition.z,
		-1, -1, // emit rate
		1, -1, -1, // draw count, trigger
		false, 0.f );

	//m_pParticleEventSequence->SetLifetime( CMinMax<float>(10000, 10000) );

}


HRESULT CX2WorldObjectParticle::OnFrameMove( double fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;


	if( m_fDelayTime > 0.f )
	{
		m_fDelayTime -= fElapsedTime;
		if(m_fDelayTime < 0.f)
		{
			m_fDelayTime = 0.f;

			if( m_hParticleEventSequence == INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				D3DXVECTOR3 vPosition = GetMatrix().GetPos();
				m_hParticleEventSequence = m_pParticleSystem->CreateSequenceHandle( NULL,  m_wstrSequenceName.c_str(), vPosition.x, vPosition.y, vPosition.z,
					-1, -1, // emit rate
					1, -1, -1, // draw count, trigger
					false, 0.f );

				SetParticlePos( m_vPos );
				SetCullCheck( m_bCullCheck );
			}
		}
	}


	if( m_pParticleSystem != NULL && m_hParticleEventSequence != INVALID_PARTICLE_SEQUENCE_HANDLE &&
		(m_fHideDistance > 0.f || m_fHideNearDistance > 0.f) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pParticle = m_pParticleSystem->GetInstanceSequence( m_hParticleEventSequence );

		if(pParticle != NULL)
		{
			float fDistance = GetDistance( g_pKTDXApp->GetDGManager()->GetCamera().GetEye(), pParticle->GetPosition() );
			if(m_fHideDistance > 0.f)
			{
				if( fDistance > m_fHideDistance )
				{
					pParticle->SetShowObject(false);
					return hr;
				}
			}
			if(m_fHideNearDistance > 0.f)
			{
				if( fDistance < m_fHideNearDistance )
				{
					pParticle->SetShowObject(false);
					return hr;
				}
			}

			pParticle->SetShowObject(true);
		}
	}

	return hr;
}

//{{ robobeg : 2008-10-28

/*virtual*/
RENDER_HINT CX2WorldObjectParticle::OnFrameRender_Prepare()
{
    //__super::SetLastAccessTime( g_NowTime );

    return RENDER_HINT_NORENDER;
}//CX2WorldObjectParticle::OnFrameRender_Prepare()


//}} robobeg : 2008-10-28


/*
CX2WorldObjectParticle::CX2WorldObjectParticle( ParticleObjectData* pParticleObjectData )
						:CX2WorldObject( pParticleObjectData )
{
	m_pParticle = g_pKTDXApp->GetDGManager()->GetParticleSystem()->CreateSequence( NULL,
							pParticleObjectData->name.c_str(),
							pParticleObjectData->pos.x,
							pParticleObjectData->pos.y,
							pParticleObjectData->pos.z,
							pParticleObjectData->fEmitMax,
							pParticleObjectData->fEmitMin,
							0,
							pParticleObjectData->drawCount );

	m_pParticle->SetBillBoardType( pParticleObjectData->billboardType );
}

*/
