#include "stdafx.h"

/**
	@file	X2RidingPet.cpp
	@author	JHKang
	@brief	�ƶ� Ŭ���� ������ ����
	@since  2013-03-26
*/

#include "X2RidingPet.h"

//#pragma region CX2RidingPet
/** @class : CX2RidingPet
	@brief : ������
	@date  : 2013/03/27
*/
CX2RidingPet::CX2RidingPet(void)
	: m_wstrMotionName( L"" ), m_wstrChangeTex( L"" ), m_wstrAniTex( L"" ), m_wstrRidingMotion( L"" )
	, m_pMotion( NULL ), m_pAniXET( NULL ), m_pChangeXET( NULL )
	, m_bInit( false ), m_bMine( false ), m_bLoaded( false ), m_bSetData( false ), m_bInitPos( false ), m_bForceColor( false )
	, m_bForceColorChange( false ), m_bUnderWaterHead( false ), m_bIgnoreLineSpeed( false ), m_bGame( false )
	, m_fScale( 1.f ), m_fElapsedTime( 0.f ), m_fNowStateTimer( 0.f ), m_cColor( 0.f, 0.f, 0.f, 0.f )
	, m_cLineUnitColor( 0.f, 0.f, 0.f, 0.f )
{
	m_SyncData.Init();
	m_ConditionData.Init();
	m_Info.Init();

	m_StateBiMap.clear();
	m_StateDataMap.clear();

	ClearState();
}
//#pragma endregion ������

//#pragma region ~CX2RidingPet
/** @class : ~CX2RidingPet
	@brief : �Ҹ���
	@date  : 2013/03/27
*/
CX2RidingPet::~CX2RidingPet(void)
{
	if ( true == m_bLoaded )
	{
		SAFE_CLOSE( m_pMotion );
	}
	SAFE_CLOSE( m_pAniXET );
	SAFE_CLOSE( m_pChangeXET );
}
//#pragma endregion �Ҹ���

//#pragma region OnFrameMove
/*virtual*/ HRESULT CX2RidingPet::OnFrameMove( IN double fTime_, IN float fElapsedTime_ )
{
	//#pragma region �ִϸ��̼� ���� �� �ε�
	if ( false == m_bLoaded && NULL != m_pMotion )
	{
		const CKTDXDevice::EDeviceState eStateXMesh = m_pMotion->GetDeviceState();

		if ( CKTDXDevice::DEVICE_STATE_LOADED == eStateXMesh )
		{
			m_pXSkinAnimFuture = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnimFuture->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnimFuture->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnimFuture->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

			m_pXSkinAnimFuture->GetMatrix().Move( GetPos() );
			m_pXSkinAnimFuture->GetMatrix().Scale( D3DXVECTOR3( m_fScale, m_fScale, m_fScale ) );
			m_pXSkinAnimFuture->SetShowObject( true );

			SetCenter( m_pXSkinAnimFuture->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnimFuture->GetAnimXSkinMesh()->GetBoundingRadius() );

			m_pXSkinAnimFuture->OnFrameMove( g_NowTime, 0.f );

			m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

			SetShowObject( true );

			m_pXSkinAnim->GetMatrix().Move( GetPos() );
			m_pXSkinAnim->GetMatrix().Scale( D3DXVECTOR3( m_fScale, m_fScale, m_fScale ) );
			m_pXSkinAnim->SetShowObject( true );

			SetCenter( m_pXSkinAnim->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnim->GetAnimXSkinMesh()->GetBoundingRadius() );

			m_pXSkinAnim->OnFrameMove( g_NowTime, 0.f );

			m_bLoaded = true;
		}
	}
	//#pragma endregion

	if ( NULL == m_pMotion || NULL == m_pXSkinAnim )
		return S_OK;

	if ( false == m_bLoaded )
		return S_OK;

	//#pragma region ��ġ �� �ð� ����
	//if ( false == m_bInitPos )
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
		bool bIsRight = true;
		vPos.y += 30.f;

		GetUserInfo( vPos, bIsRight );
		SetPos( vPos, bIsRight );
		m_bInitPos = true;
	}

	m_fElapsedTime = fElapsedTime_;
	m_fNowStateTimer += fElapsedTime_;
	//#pragma endregion

	//#pragma region ��ġ ����
	if ( true == m_bLoaded && NULL != m_pMotion && NULL != m_pXSkinAnim && ( NULL != g_pX2Game || NULL != g_pTFieldGame ) )
	{
		GetMatrix().Move( m_SyncData.m_vPos );
		GetMatrix().RotateDegree( m_ConditionData.m_vDirDegree );

		if ( true == m_bForceBillBoard )
		{
			m_pXSkinAnim->SetBillBoardType( CKTDGMatrix::BT_ALL );
		}
		else
		{
			m_pXSkinAnim->GetMatrix().RotateDegree( m_ConditionData.m_vDirDegree );
		}

		m_pXSkinAnim->GetMatrix().Move( m_SyncData.m_vPos );
		m_pXSkinAnim->OnFrameMove( fTime_, fElapsedTime_ );
	}
	else
	{
		SetShowObject( false );
        if ( m_pXSkinAnim != NULL )
        {
            m_pXSkinAnim->UpdateBeforeAnimationTime();
        }
	}
	//#pragma endregion

	return S_OK;
}
//#pragma endregion �� ������ ó���� ����

//#pragma region OnFrameRender_Prepare
/*virtual*/ RENDER_HINT CX2RidingPet::OnFrameRender_Prepare()
{
	if ( false == m_bLoaded || NULL == m_pXSkinAnim )
		return RENDER_HINT_NORENDER;

	m_pXSkinAnim->SetInstantShow( false );
	if ( !m_pXSkinAnim->GetShowObject() )
		return RENDER_HINT_NORENDER;

	CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
	CKTDGXRenderer::RenderParam* pUserRenderParam = NULL;

	CX2World* pWorld = NULL;

	if ( NULL != g_pX2Game )
	{
		pWorld = g_pX2Game->GetWorld();

		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
		if ( NULL != pUser && NULL != pUser->GetSkinAnimPtr() && NULL != pUser->GetSkinAnimPtr()->GetRenderParam() )
		{
			pUserRenderParam = pUser->GetSkinAnimPtr()->GetRenderParam();
		}
	}
	else if ( NULL != g_pTFieldGame )
	{
		CX2SquareUnit* pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );

		if ( NULL != pUser )
		{
			if ( true == pUser->GetPlanRender() )
				return RENDER_HINT_NORENDER;
		}

		pWorld = g_pTFieldGame->GetWorld();
	}

	if ( NULL != pWorld )
		pRenderParam->lightPos = pWorld->GetLightPos();

	pRenderParam->color = m_cColor;

	if ( false == m_bForceColor && ( true == m_bUnderWaterHead || true == m_bForceColorChange ) )
	{
		pRenderParam->color.r -= m_cLineUnitColor.r;
		pRenderParam->color.g -= m_cLineUnitColor.b;
		pRenderParam->color.b -= m_cLineUnitColor.b;
	}

	pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON_BLACK_EDGE;

	if ( CX2GameOption::OL_MEDIUM == g_pMain->GetGameOption().GetOptionList().m_UnitDetail )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_ADD_CARTOON;
	}
	else if( CX2GameOption::OL_LOW == g_pMain->GetGameOption().GetOptionList().m_UnitDetail )
	{
		pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
	}

	if ( 700.f < GetDistanceToCamera() )
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 1000.f * 1.f;
	}
	else
	{
		pRenderParam->fOutLineWide = GetDistanceToCamera() / 500.f * 1.f;
	}

	if ( NULL != g_pX2Game && NULL != pUserRenderParam )
	{
		pRenderParam->cartoonTexType	= pUserRenderParam->cartoonTexType;
		pRenderParam->renderType		= pUserRenderParam->renderType;
		pRenderParam->fOutLineWide		= pUserRenderParam->fOutLineWide;
		pRenderParam->bAlphaBlend		= pUserRenderParam->bAlphaBlend;
		pRenderParam->cullMode			= pUserRenderParam->cullMode;
		pRenderParam->bZEnable			= pUserRenderParam->bZEnable;
		pRenderParam->outLineColor		= pUserRenderParam->outLineColor;
		pRenderParam->color				= pUserRenderParam->color;
	}
	else
	{
		pRenderParam->cartoonTexType	= m_RenderParam.cartoonTexType;
		pRenderParam->renderType		= m_RenderParam.renderType;
		pRenderParam->fOutLineWide		= m_RenderParam.fOutLineWide;
		pRenderParam->bAlphaBlend		= m_RenderParam.bAlphaBlend;
		pRenderParam->cullMode			= m_RenderParam.cullMode;
		pRenderParam->bZEnable			= m_RenderParam.bZEnable;
	}

	RENDER_HINT renderHindResult = m_pXSkinAnim->OnFrameRender_Prepare();

	if ( renderHindResult.m_eHint != RENDER_HINT::NORENDER )
		m_pXSkinAnim->SetInstantShow( true );

	return renderHindResult;
}
//#pragma endregion �� ������ ������ �ɼ� ����

//#pragma region OnFrameRender_Draw
/*virtual*/ void CX2RidingPet::OnFrameRender_Draw()
{
	KTDXPROFILE();

	if( m_pXSkinAnim == NULL )
		return;

	if( m_pXSkinAnim != NULL && m_pXSkinAnim->GetShowObject() && m_pXSkinAnim->IsInstantShow() )
	{
		m_pXSkinAnim->OnFrameRender_Draw();
	}
}
//#pragma endregion �� ������ Animation �׸���

//#pragma region OnResetDevice
/*virtual*/ HRESULT CX2RidingPet::OnResetDevice()
{
	return S_OK;
}
//#pragma endregion ��ġ �缳��

//#pragma region OnLostDevice
/*virtual*/ HRESULT CX2RidingPet::OnLostDevice()
{
	return S_OK;
}
//#pragma endregion ��ġ ������

//#pragma region static InitInit
/*static*/ void CX2RidingPet::InitInit( InitData& OutInit_, KLuaManager& luaManager_ )
{
	InitDevice( OutInit_.m_device, luaManager_ );
	InitMotion( OutInit_.m_device, luaManager_ );
}
//#pragma endregion

//#pragma region InitData::AppendToDeviceList
void CX2RidingPet::InitData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	m_device.AppendToDeviceList( listInOut_ );
}
//#pragma endregion

//#pragma region InitDeviceData::AppendToDeviceList
void CX2RidingPet::InitDeviceData::AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const
{
	// Texture
	for( const wstring& strTexture : m_vecstrReadyTexture )
	{
		g_pKTDXApp->GetDeviceManager()->AppendTexture( listInOut_, strTexture );
	}

	// skinMesh
	for( const wstring& strXSkinMesh : m_vecstrReadyXSkinMesh )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, strXSkinMesh );
	}

	// xmesh
	for( const wstring& strXMesh : m_vecstrReadyXMesh )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXMesh( listInOut_, strXMesh );
	}

	// xet
	for( const wstring& strXET : m_vecstrReadyXET )
	{
		g_pKTDXApp->GetDeviceManager()->AppendXET( listInOut_, strXET );
	}
}
//#pragma endregion

//#pragma region static InitDevice
/*static*/ void CX2RidingPet::InitDevice( IN InitDeviceData& OutDevice_, IN KLuaManager& luaManager_ )
{
	wstring wstrFileName;

	if( luaManager_.BeginTable( "INIT_DEVICE" ) == true )
	{
		OutDevice_.m_vecstrReadyTexture.resize( 0 );
		if( luaManager_.BeginTable( "READY_TEXTURE" ) == true )
		{
			int index = 1;
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyTexture.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadyXMesh.resize( 0 );
		if( luaManager_.BeginTable( "READY_XMESH" ) == true )
		{
			int index = 1;
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyXMesh.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadyXSkinMesh.resize( 0 );
		if( luaManager_.BeginTable( "READY_XSKIN_MESH" ) == true )
		{
			int index = 1;
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadyXSkinMesh.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}

		OutDevice_.m_vecstrReadySound.resize( 0 );
		if( luaManager_.BeginTable( "READY_SOUND" ) == true )
		{
			int index = 1;
			while( luaManager_.GetValue( index, wstrFileName ) == true )
			{
				OutDevice_.m_vecstrReadySound.push_back( wstrFileName );
				index++;
			}
			luaManager_.EndTable();
		}
		luaManager_.EndTable(); // INIT_DEVICE
	}
}
//#pragma endregion

//#pragma region SetRidingPet
void CX2RidingPet::SetRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ )
{
	if ( NULL != pPetLuaState_ )
		m_LuaManager.AssignNewLuaState( pPetLuaState_, 0, false );
	else
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	m_UserUid = uUserUid_;

	if ( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit()->GetUID() == m_UserUid )
	{
		m_bMine = true;
	}

	InitSystem();
	InitDevice();
	InitMotion();
	InitPhysic();
	InitState();

	if ( false == m_bLoaded && NULL == m_pXSkinAnim )
	{
		m_pMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMeshInBackground( m_wstrMotionName.c_str() );
	}

	m_bSetData = false;
}
//#pragma endregion

//#pragma region SetGameRidingPet
void CX2RidingPet::SetGameRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ )
{
	m_bGame = true;

	if ( pPetLuaState_ != NULL )
		m_LuaManager.AssignNewLuaState( pPetLuaState_, 0, false );
	else
		m_LuaManager.AssignNewLuaState( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	m_UserUid = uUserUid_;

	if ( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		 g_pData->GetMyUser()->GetSelectUnit()->GetUID() == m_UserUid )
	{
		m_bMine = true;
	}

	m_ConditionData.Init();
	m_SyncData.Init();

	InitSystem();
	InitMotion();
	InitPhysic();
	InitState();

	if ( false == m_bLoaded && NULL == m_pXSkinAnim )
	{
		if ( NULL == m_pMotion && NULL == m_pXSkinAnim )
		{
			m_pMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( m_wstrMotionName.c_str() );
			ASSERT( m_pMotion );
			if ( NULL == m_pMotion )
			{
				StateLog( m_wstrMotionName.c_str() );
			}
		}

		if ( NULL != m_pMotion )
		{
			m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnimPtr();
			m_pXSkinAnim->SetAnimXSkinMesh( m_pMotion );
			m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, m_pAniXET, NULL, m_pChangeXET );
			m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );
			m_pXSkinAnim->SetShowObject( false );

			SetCenter( m_pXSkinAnim->GetAnimXSkinMesh()->GetCenter() );
			SetBoundingRadius( m_pXSkinAnim->GetAnimXSkinMesh()->GetBoundingRadius() );
		}
	}

	m_bSetData = false;
}
//#pragma endregion ���� / �ʵ忡�� Ż �� ����

//#pragma region SetInfo
void CX2RidingPet::SetInfo( IN const RidingPetInfo& ridingPetInfo_, IN RIDING_SPECIAL_MOVE eSpecialMove_ )
{
	m_Info = ridingPetInfo_;
	m_Info.m_eSpecialMove = eSpecialMove_;
	m_bSetData = true;
}
//#pragma endregion Ż �� ���� �� Ư�� �̵��� �� ����

//#pragma region InitDevice
void CX2RidingPet::InitDevice()
{
	wstring fileName = L"";

	if( m_LuaManager.BeginTable( "INIT_DEVICE" ) == true )
	{
		if( m_LuaManager.BeginTable( "READY_TEXTURE" ) == true )
		{
			wstring texName;
			int index = 1;
			while( m_LuaManager.GetValue( index, texName ) == true )
			{
				TextureReadyInBackground( texName.c_str() );
				index++;
			}
			m_LuaManager.EndTable();
		}

		if( m_LuaManager.BeginTable( "READY_XMESH" ) == true )
		{
			wstring xmeshName;
			int index = 1;
			while( m_LuaManager.GetValue( index, xmeshName ) == true )
			{
				XMeshReadyInBackground( xmeshName.c_str() );
				index++;
			}
			m_LuaManager.EndTable();
		}


		if( m_LuaManager.BeginTable( "READY_XSKIN_MESH" ) == true )
		{
			wstring xskinmeshName;
			int index = 1;
			while( m_LuaManager.GetValue( index, xskinmeshName ) == true )
			{
				XSkinMeshReadyInBackground( xskinmeshName.c_str() );
				index++;
			}
			m_LuaManager.EndTable();
		}

		if( m_LuaManager.BeginTable( "READY_SOUND" ) == true )
		{
			wstring soundName;
			int index = 1;
			while( m_LuaManager.GetValue( index, soundName ) == true )
			{
				SoundReadyInBackground( soundName.c_str() );
				index++;
			}
			m_LuaManager.EndTable();
		}

		m_LuaManager.EndTable(); // INIT_DEVICE
	}
}
//#pragma endregion ���, �ؽ�ó, ���� ���� ��׶��� �ε� �� �ε�

//#pragma region InitSystem
void CX2RidingPet::InitSystem()
{
	m_RenderParam.Init();
	m_bForceBillBoard = false;

	if( m_LuaManager.BeginTable( "INIT_SYSTEM" ) == true )
	{
		int iLayer = 0;
		LUA_GET_VALUE( m_LuaManager, "UNIT_WIDTH", m_ConditionData.m_fUnitWidth, 300.f );
		LUA_GET_VALUE( m_LuaManager, "UNIT_HEIGHT", m_ConditionData.m_fUnitHeight, 300.f );
		m_LuaManager.GetValue( "UNIT_LAYER", iLayer );
		SetLayer( iLayer );

		LUA_GET_VALUE( m_LuaManager, "UNIT_SCALE", m_fScale, 1.f );

		LUA_GET_VALUE_ENUM( m_LuaManager, "RENDER_PARAM", m_RenderParam.renderType, CKTDGXRenderer::RENDER_TYPE, CKTDGXRenderer::RT_CARTOON_BLACK_EDGE );
		LUA_GET_VALUE( m_LuaManager, "OUT_LINE_WIDTH_SCALE", m_RenderParam.fOutLineWide, CARTOON_OUTLINE_WIDTH );

		LUA_GET_VALUE( m_LuaManager, "ALPHA_BLEND", m_RenderParam.bAlphaBlend, false );
		if( m_RenderParam.bAlphaBlend == true )
		{
			SetAlphaObject( true );
		}

		bool bCullMode;
		LUA_GET_VALUE( m_LuaManager, "CULL_MODE", bCullMode, true );
		if( bCullMode == false )
		{
			m_RenderParam.cullMode = D3DCULL_NONE;
		}

		LUA_GET_VALUE( m_LuaManager, "Z_ENABLE", m_RenderParam.bZEnable, true );
		LUA_GET_VALUE( m_LuaManager, "Z_WRITE_ENABLE", m_RenderParam.bZWriteEnable, true );
		LUA_GET_VALUE( m_LuaManager, "FORCE_BILLBOARD", m_bForceBillBoard, false );

		CKTDGXRenderer::CARTOON_TEX_TYPE cartoonTexType = CKTDGXRenderer::CTT_NORMAL;
		LUA_GET_VALUE_ENUM( m_LuaManager, "CARTOON_TEX_TYPE", cartoonTexType, CKTDGXRenderer::CARTOON_TEX_TYPE, CKTDGXRenderer::CTT_NORMAL );
		m_RenderParam.cartoonTexType = cartoonTexType;

		D3DBLEND srcBlend = D3DBLEND_SRCALPHA;
		D3DBLEND destBlend = D3DBLEND_INVSRCALPHA;
		LUA_GET_VALUE_ENUM( m_LuaManager, "SRC_BLEND", srcBlend, D3DBLEND, D3DBLEND_SRCALPHA );
		LUA_GET_VALUE_ENUM( m_LuaManager, "DEST_BLEND", destBlend, D3DBLEND, D3DBLEND_INVSRCALPHA );
		m_RenderParam.srcBlend = srcBlend;
		m_RenderParam.destBlend = destBlend;

		LUA_GET_VALUE( m_LuaManager, "NOT_CHANGE_COLOR", m_bForceColor, false );
		float fColor = 255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_A", fColor, 255.f );
		m_cColor.a = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_R", fColor, 255.f );
		m_cColor.r = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_G", fColor, 255.f );
		m_cColor.g = fColor/255.f;
		LUA_GET_VALUE( m_LuaManager, "COLOR_B", fColor, 255.f );
		m_cColor.b = fColor/255.f;

		m_LuaManager.EndTable();
	}
}
//#pragma endregion Ż �� �⺻ ���� �ʱ�ȭ

//#pragma region InitPhysic
void CX2RidingPet::InitPhysic()
{
	if( m_LuaManager.BeginTable( "INIT_PHYSIC" ) == true )
	{
		m_LuaManager.GetValue( "RELOAD_ACCEL",		m_PhysicParam.fReloadAccel );
		m_LuaManager.GetValue( "G_ACCEL",			m_PhysicParam.fGAccel );
		m_LuaManager.GetValue( "MAX_G_SPEED",		m_PhysicParam.fMaxGSpeed );

		m_LuaManager.GetValue( "WALK_SPEED",		m_PhysicParam.fWalkSpeed );
		m_LuaManager.GetValue( "RUN_SPEED",			m_PhysicParam.fRunSpeed );
		m_LuaManager.GetValue( "JUMP_SPEED",		m_PhysicParam.fJumpSpeed );
		m_LuaManager.GetValue( "DASH_JUMP_SPEED",	m_PhysicParam.fDashJumpSpeed );

		m_LuaManager.EndTable();
	}
}
//#pragma endregion ���� ��ġ �ʱ�ȭ

//#pragma region InitState
void CX2RidingPet::InitState()
{
	if( m_LuaManager.BeginTable( "INIT_STATE" ) == true )
	{
		RidingPetStateData stateData;
		m_StateBiMap.clear();
		m_StateDataMap.clear();

		m_StateDataMap.insert( std::make_pair( RSI_NONE, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_NONE, "NONE") );

		m_StateDataMap.insert( std::make_pair( RSI_LOADING, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_LOADING, "LOADING") );

		m_StateDataMap.insert( std::make_pair( RSI_READY, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_READY, "READY") );

		m_StateDataMap.insert( std::make_pair( RSI_PLAY, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_PLAY, "PLAY") );

		m_StateDataMap.insert( std::make_pair( RSI_END, stateData ) );
		m_StateBiMap.insert( BiMapValue( RSI_END, "END") );

		int iIndex = 1;
		while ( true == m_LuaManager.BeginTable( iIndex ) )
		{
			RidingPetStateData stateData;
			std::string strStateTableName;
			LUA_GET_VALUE_UTF8( m_LuaManager, "STATE_NAME", strStateTableName, "" );
			stateData.m_StateID = RSI_END + iIndex;

			m_StateDataMap.insert( std::make_pair( stateData.m_StateID, stateData ) );
			m_StateBiMap.insert( BiMapValue( stateData.m_StateID, strStateTableName ) );

			++iIndex;
			m_LuaManager.EndTable();
		}

		std::string strStateName = "";
		LUA_GET_VALUE_UTF8( m_LuaManager, "START_STATE", strStateName, "" );
		m_StartState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "WAIT_STATE", strStateName, "" );
		m_WaitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "HABIT_STATE", strStateName, "" );
		m_WaitHabitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "WALK_STATE", strStateName, "" );
		m_WalkState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_UP_STATE", strStateName, "" );
		m_JumpUpState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_DOWN_STATE", strStateName, "" );
		m_JumpDownState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_LANDING_STATE", strStateName, "" );
		m_JumpLandingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_STATE", strStateName, "" );
		m_DashState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_END_STATE", strStateName, "" );
		m_DashEndState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_UP_STATE", strStateName, "" );
		m_DashJumpUpState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_LANDING_STATE", strStateName, "" );
		m_DashJumpLandingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DAMAGE_FRONT_STATE", strStateName, "" );
		m_DamageFrontState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DAMAGE_BACK_STATE", strStateName, "" );
		m_DamageBackState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "DYING_STATE_STATE", strStateName, "" );
		m_DyingState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_Z_STATE", strStateName, "" );
		m_AttackZState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_ATTACK_Z_STATE", strStateName, "" );
		m_JumpAttackZState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_X_STATE", strStateName, "" );
		m_AttackXState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "ATTACK_SPECIAL_STATE", strStateName, "" );
		m_SpecialAttackState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "SIT_READY_STATE", strStateName, "" );
		m_SitReadyState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "SIT_WAIT_STATE", strStateName, "" );
		m_SitWaitState = GetStateID( strStateName );
		LUA_GET_VALUE_UTF8( m_LuaManager, "STAND_UP_STATE", strStateName, "" );
		m_StandUpState = GetStateID( strStateName );

		LUA_GET_VALUE_UTF8( m_LuaManager, "SPECIAL_MOVE_STATE", strStateName, "" );
		m_SpecialMoveState = GetStateID( strStateName );

		m_LuaManager.EndTable();
	}

	for ( StateBiMap::left_const_iterator leftIter = m_StateBiMap.left.begin(); leftIter != m_StateBiMap.left.end(); ++leftIter )
	{
		m_LuaManager.MakeTableReference( leftIter->second.c_str(), leftIter->first );
	}
}
//#pragma endregion ���� ���̺� �ʱ�ȭ

//#pragma region IsValidUser
bool CX2RidingPet::IsValidUser()
{
	void* pUser = NULL;

	if ( NULL != g_pX2Game )
	{
		pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );

		if ( false == static_cast<CX2GUUser*>( pUser )->IsLocalUnit() )
			return false;
	}
	else if ( NULL != g_pTFieldGame )
	{
		pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );
	}

	if ( NULL == pUser )
		return false;

	return true;
}
//#pragma endregion ���� Ȯ��

//#pragma region StateChangeFutrue
void CX2RidingPet::StateChangeFuture( IN int iStateID_ )
{
	KTDXPROFILE();

	StateBiMap::left_const_iterator leftIter;
	leftIter = m_StateBiMap.left.find( iStateID_ );

	if ( leftIter != m_StateBiMap.left.end() )
	{
		string stateName = leftIter->second;
		StateChangeFuture( stateName.c_str() );
	}
}

void CX2RidingPet::StateChangeFuture( IN const char* stateName_ )
{
	// 	if ( false == IsValidUser() )
	// 		return;

	bool bTableOpen = false;
	std::string stateName = stateName_;
	StateBiMap::right_const_iterator rightIter = m_StateBiMap.right.find( stateName );

	if ( rightIter != m_StateBiMap.right.end() )
	{
		char cID = rightIter->second;

		m_StateData.m_StateID = m_SyncData.m_nowState = cID;
		++m_SyncData.m_StateChangeNum;

		bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
		{
			wstring animName;
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

			CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE ePlayType;
			LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", ePlayType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
			bool bTransition = true;
			LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );

			if( m_pXSkinAnimFuture != NULL )
			{
				if( bTransition == false )
					m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), false );
				else
					m_pXSkinAnimFuture->ChangeAnim( animName.c_str(), true );

				m_pXSkinAnimFuture->Play( ePlayType );
			}

			float fAnimSpeed = 1.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", fAnimSpeed, 1.f );
			if( m_pXSkinAnimFuture != NULL )
				m_pXSkinAnimFuture->SetPlaySpeed( fAnimSpeed );

			if ( NULL != g_pX2Game )
			{
				CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
				if ( NULL != pUser )
				{
					CX2GUUser::FrameData& kFutureFrameData = pUser->AccessFutureFrameData();
					//CX2GUUser::PhysicParam& pPhysicParam = pUser->GetPhysicParam();
					//std::vector<CX2GUUser::TIME_SPEED>& vecSpeedFactor = pUser->GetSpeedFatorVector();
					//std::vector<CX2GUUser::SoundPlayData>& vecSoundData = pUser->GetSoundPlayData();

					//if ( NULL != pFrameData )
					{
						LUA_GET_VALUE( m_LuaManager, "LAND_CONNECT", kFutureFrameData.stateParam.bLandConnect, true );
						m_ConditionData.m_bLandConnection = kFutureFrameData.stateParam.bLandConnect;
						bool bCanPushUnit = true;
						LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT", bCanPushUnit, true );
						kFutureFrameData.stateParam.bCanPushUnit = bCanPushUnit;
						bool bCanPassUnit = false;
						LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
						kFutureFrameData.stateParam.bCanPassUnit = bCanPassUnit;
					}
				}
			}

		}
		if( bTableOpen == true )
			m_LuaManager.EndTable();
	}
}
//#pragma endregion ���� ����_��� ������ ��

//#pragma region StateChange
void CX2RidingPet::StateChange( IN int iStateID_ )
{
	KTDXPROFILE();

	StateBiMap::left_const_iterator leftIter;
	leftIter = m_StateBiMap.left.find( iStateID_ );

	if ( leftIter != m_StateBiMap.left.end() )
	{
		string stateName = leftIter->second;
		StateChange( stateName.c_str() );
	}
}

void CX2RidingPet::StateChange( IN const char* stateName_ )
{
// 	if ( false == IsValidUser() )
// 		return;

	bool bTableOpen = false;
	std::string stateName = stateName_;
	StateBiMap::right_const_iterator rightIter = m_StateBiMap.right.find( stateName );

	if ( rightIter != m_StateBiMap.right.end() )
	{
		char cID = rightIter->second;

		m_StateData.m_StateID = m_SyncData.m_nowState = cID;
		++m_SyncData.m_StateChangeNum;

		bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
		{
			wstring animName;
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

			CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE ePlayType;
			LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", ePlayType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
			bool bTransition = true;
			LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );

			if( m_pXSkinAnim != NULL )
			{
				if( bTransition == false )
					m_pXSkinAnim->ChangeAnim( animName.c_str(), false );
				else
					m_pXSkinAnim->ChangeAnim( animName.c_str(), true );

				m_pXSkinAnim->Play( ePlayType );
			}

			float fAnimSpeed = 1.f;
			LUA_GET_VALUE( m_LuaManager, "ANIM_SPEED", fAnimSpeed, 1.f );
			if( m_pXSkinAnim != NULL )
				m_pXSkinAnim->SetPlaySpeed( fAnimSpeed );

			if ( NULL != g_pX2Game )
			{
				CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
				if ( NULL != pUser )
				{
					CX2GUUser::FrameData& kNowFrameData = pUser->AccessNowFrameData();
					CX2GUUser::PhysicParam& kPhysicParam = pUser->AccessPhysicParam();
					std::vector<CX2GUUser::TIME_SPEED>& vecSpeedFactor = pUser->AccessSpeedFatorVector();
					std::vector<CX2GUUser::SoundPlayData>& vecSoundData = pUser->AccessSoundPlayData();

					//if ( NULL != pFrameData )
					{
						bool bCanPushUnit = true;
						LUA_GET_VALUE( m_LuaManager, "CAN_PUSH_UNIT", bCanPushUnit, true );
						kNowFrameData.stateParam.bCanPushUnit = bCanPushUnit;
						bool bCanPassUnit = false;
						LUA_GET_VALUE( m_LuaManager, "CAN_PASS_UNIT",	bCanPassUnit,	false );
						kNowFrameData.stateParam.bCanPassUnit = bCanPassUnit;
					}

					D3DXVECTOR3 speed( 0.f, 0.f, 0.f );
					if( m_LuaManager.GetValue( "SPEED_X", speed.x ) == true )
						kPhysicParam.nowSpeed.x = speed.x;
					if( m_LuaManager.GetValue( "SPEED_Y", speed.y ) == true )
						kPhysicParam.nowSpeed.y = speed.y;

					LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_X", kPhysicParam.passiveSpeed.x, -1.f );
					LUA_GET_VALUE( m_LuaManager, "PASSIVE_SPEED_Y", kPhysicParam.passiveSpeed.y, -1.f );

					int index = 0;
					vecSpeedFactor.clear();
					while( m_LuaManager.BeginTable( "SPEED_TIME", index ) == true )
					{
						CX2GUUser::TIME_SPEED timeSpeed;

						LUA_GET_VALUE( m_LuaManager, 1, timeSpeed.vSpeed.x, 0.0f );		/// X�� �ӵ�
						LUA_GET_VALUE( m_LuaManager, 2, timeSpeed.vSpeed.y, 0.0f );		/// Y�� �ӵ�
						LUA_GET_VALUE( m_LuaManager, 3, timeSpeed.vSpeed.z, -1.0f );		/// ����� �ִϸ��̼� Ÿ��
						LUA_GET_VALUE( m_LuaManager, 4, timeSpeed.iFlag, 0 );			/// m_PhysicParam.nowSpeed, 1�̸� +=, 2�̸� = ������ ������, 3�̸� �нú� ���ǵ� �� ����

						vecSpeedFactor.push_back( timeSpeed );
						++index;
						m_LuaManager.EndTable();
					}

					if( m_LuaManager.BeginTable( "INVINCIBLE" ) == true )
					{
						float fInvin = -1.0f;
						LUA_GET_VALUE( m_LuaManager, 1,	fInvin,	-1.0f );
						kNowFrameData.stateParam.invincibleTime.m_fX = fInvin;
						fInvin = -1.0f;
						LUA_GET_VALUE( m_LuaManager, 2,	fInvin,	-1.0f );
						kNowFrameData.stateParam.invincibleTime.m_fY = fInvin;

						if( kNowFrameData.stateParam.invincibleTime.m_fX <= 0.0f && kNowFrameData.stateParam.invincibleTime.m_fY > 0.0f )
							kNowFrameData.stateParam.bInvincible = true;
						else
							kNowFrameData.stateParam.bInvincible = false;

						m_LuaManager.EndTable();
					}
					else
					{
						kNowFrameData.stateParam.invincibleTime.m_fX	= -1.0f;
						kNowFrameData.stateParam.invincibleTime.m_fY	= -1.0f;
						kNowFrameData.stateParam.bInvincible		= false;
					}

					bool bSuperArmor = false;
					LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR",			bSuperArmor,			false );
					kNowFrameData.stateParam.bSuperArmor			= bSuperArmor;

					bool bSuperArmorNotRed = true;
					LUA_GET_VALUE( m_LuaManager, "SUPER_ARMOR_NOT_RED", bSuperArmorNotRed,		true );
					kNowFrameData.stateParam.bSuperArmorNotRed		= bSuperArmorNotRed;

					pUser->SetStateSuperArmor( bSuperArmor );

                    std::vector<D3DXVECTOR2>& vecSuperArmorTime = pUser->AccessVecNowSuperArmorTime();
                    vecSuperArmorTime.clear();

					int indexTimeTable = 0;
					while( m_LuaManager.BeginTable( "SUPER_ARMOR_TIME", indexTimeTable ) == true )
					{
						D3DXVECTOR2 superArmorTime;
						LUA_GET_VALUE( m_LuaManager, 1, superArmorTime.x, 0.f );
						LUA_GET_VALUE( m_LuaManager, 2, superArmorTime.y, 0.f );
						vecSuperArmorTime.push_back(superArmorTime);

						indexTimeTable++;
						m_LuaManager.EndTable();
					}

					index = 0;
					vecSoundData.resize(0);
					while( m_LuaManager.BeginTable( "SOUND_PLAY", index ) == true )
					{
						CX2GUUser::SoundPlayData sndPlayData;

						LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,		-1.0f	);
						LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,		L""		);
						LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,		100		);
						LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,		false	);

						vecSoundData.push_back( sndPlayData );

						index++;
						m_LuaManager.EndTable();
					}

					if( pUser->IsFocusUnit() == true && kNowFrameData.stateParam.bResetCamera == true )
					{
						kNowFrameData.stateParam.hitCamera = 0;
						if( g_pX2Game->GetX2Camera()->GetCameraState() != CX2Camera::CS_NORMAL )
						{
							g_pX2Game->GetX2Camera()->NomalDirectCamera( pUser, g_pMain->GetGameOption().GetCameraDistance() );
						}

						int camera0Rate = 100;
						int camera1Rate = 100;
						int camera2Rate = 100;
						int ran = rand()%100 + 1;

						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA0_RATE", camera0Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA1_RATE", camera1Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "NORMAL_CAMERA2_RATE", camera2Rate, 0 );

						if( ran < camera0Rate )
						{
							kNowFrameData.stateParam.normalCamera = 0;
						}
						else if( ran < (camera0Rate + camera1Rate) )
						{
							kNowFrameData.stateParam.normalCamera = 1;
						}
						else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
						{
							kNowFrameData.stateParam.normalCamera = 2;
						}
						else
							kNowFrameData.stateParam.normalCamera = 0;

						ran = rand()%100 + 1;
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA0_RATE", camera0Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA1_RATE", camera1Rate, 0 );
						LUA_GET_VALUE( m_LuaManager, "HIT_CAMERA2_RATE", camera2Rate, 0 );
						if( ran < camera0Rate )
						{
							kNowFrameData.stateParam.hitCamera = 0;
						}
						else if( ran < (camera0Rate + camera1Rate) )
						{
							kNowFrameData.stateParam.hitCamera = 1;
						}
						else if( ran < (camera0Rate + camera1Rate + camera2Rate) )
						{
							kNowFrameData.stateParam.hitCamera = 2;
						}
						else
							kNowFrameData.stateParam.hitCamera = kNowFrameData.stateParam.normalCamera;
					}

					float fValue = 0.f;
					LUA_GET_VALUE(	m_LuaManager, "SKILL_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetSkillCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "BWALK_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetBWalkCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "WALK_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetWalkCancelAfter( fValue );
					LUA_GET_VALUE(	m_LuaManager, "DASH_CANCEL_AFTER",		fValue,	9999.f );
					pUser->SetDashCancelAfter( fValue );

					pUser->LoadEffectSetFromScript( m_LuaManager );

					bool bDeleteEffect = false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_STATE_END", bDeleteEffect, false );
					pUser->SetDeleteEffectSetOnStateEnd( bDeleteEffect );
					bDeleteEffect= false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DAMAGE_REACT", bDeleteEffect, false );
					pUser->SetDeleteEffectSetOnDamageReact( bDeleteEffect );
					bDeleteEffect= false;
					LUA_GET_VALUE( m_LuaManager, "DELETE_EFFECT_SET_ON_DIE", bDeleteEffect,	false );
					pUser->SetDeleteEffectSetOnDie( bDeleteEffect );

				}
			}
		}
		if( bTableOpen == true )
			m_LuaManager.EndTable();
	}
}

void CX2RidingPet::StateChange_LUA( IN const char* stateName_ )
{
	//wstring stateName;
	//ConvertUtf8ToWCHAR( stateName, stateName_ );
	StateChange( stateName_ );
}
//#pragma endregion ���� ����

//#pragma region static InitMotion
/*static*/
void CX2RidingPet::InitMotion( InitDeviceData& OutDevice_, KLuaManager& luaManager_ )
{
	if( luaManager_.BeginTable( "INIT_MOTION" ) == true )
	{
		std::wstring	wstrName;

		if ( luaManager_.GetValue( "MOTION_FILE_NAME", wstrName ) == true )
			OutDevice_.m_vecstrReadyXSkinMesh.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_CHANGE_TEX_XET", wstrName ) == true )
			OutDevice_.m_vecstrReadyXET.push_back( wstrName );
		if ( luaManager_.GetValue( "MOTION_ANI_TEX_XET", wstrName ) == true )
			OutDevice_.m_vecstrReadyXET.push_back( wstrName );

		luaManager_.EndTable();
	}
}
//#pragma endregion

//#pragma region InitMotion
void CX2RidingPet::InitMotion()
{
	if( m_LuaManager.BeginTable( "INIT_MOTION" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "MOTION_FILE_NAME", m_wstrMotionName, L"" );
		LUA_GET_VALUE( m_LuaManager, "MOTION_CHANGE_TEX_XET", m_wstrChangeTex, L"" );
		LUA_GET_VALUE( m_LuaManager, "MOTION_ANI_TEX_XET", m_wstrAniTex, L"" );
		LUA_GET_VALUE( m_LuaManager, "RIDING_MOTION", m_wstrRidingMotion, L"" );

		m_LuaManager.EndTable();
	}

	SAFE_CLOSE( m_pChangeXET );
	m_pChangeXET = g_pKTDXApp->GetDeviceManager()->OpenXET( m_wstrChangeTex.c_str() );
	SAFE_CLOSE( m_pAniXET );
	m_pAniXET = g_pKTDXApp->GetDeviceManager()->OpenXET( m_wstrAniTex.c_str() );

}
//#pragma endregion  ��� ����

//#pragma region ClearState
void CX2RidingPet::ClearState()
{
	m_StartState = m_WaitHabitState = m_WaitState = m_WalkState = m_JumpUpState = m_JumpDownState = m_JumpLandingState =
		m_DashState = m_DashEndState = m_DashJumpUpState = m_DashJumpLandingState = m_DamageFrontState = m_DamageBackState =
		m_DyingState = m_AttackZState = m_JumpAttackZState = m_AttackXState = m_SpecialAttackState = m_SitReadyState =
		m_SitWaitState = m_StandUpState = m_SpecialMoveState = RSI_NONE;
}
//#pragma endregion �⺻ ���µ� �� �ʱ�ȭ

//#pragma region GetUserInfo
void CX2RidingPet::GetUserInfo( IN D3DXVECTOR3 &vPos, IN bool &bIsRight_ )
{
	vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
	bIsRight_ = true;

	if ( -1 >= m_UserUid )
		return;

	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );

			m_pXSkinAnim->GetMatrix().Scale( pUser->GetScaleByUnit() );
		}
	}
	else if ( NULL != g_pTFieldGame )
	{
		CX2SquareUnit* pUser = static_cast<CX2SquareUnit*>( g_pTFieldGame->GetSquareUnitByUID( m_UserUid ) );

		if ( NULL != pUser && NULL != m_pXSkinAnim )
		{
			D3DXVECTOR3 vRot;
			pUser->SetSaddlePos( GetBonePos( L"Saddle_Dummy01" ) );
			GetFrameRotateDegree( &vRot, m_pXSkinAnim->GetCloneFrame( L"Saddle_Dummy01" ) );
			pUser->SetSaddleDegree( vRot );
			pUser->SetRidingMotionName( m_wstrRidingMotion );

			vPos = pUser->GetPos();
			bIsRight_ = pUser->GetIsRight();
			if ( true == m_bLoaded )
				pUser->SetRidingOn( true );
		}
	}
}
//#pragma endregion ���� ��ġ ����, ���� ��ġ ���ϱ�

//#pragma region SetPos
void CX2RidingPet::SetPos( IN D3DXVECTOR3& vPos_, IN bool bIsRight_ )
{
	KTDXPROFILE();

	CKTDGLineMap* pLineMap = NULL;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetWorld() )
	{
		pLineMap = g_pX2Game->GetWorld()->GetLineMap();
	}
	else if ( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetWorld() )
	{
		pLineMap = g_pTFieldGame->GetWorld()->GetLineMap();
	}

	if ( NULL == pLineMap )
		return;

	if ( false == pLineMap->GetAnyEnabledLine() )
		return;

	D3DXVECTOR3 startPos;
	D3DXVECTOR3 dirVec( 0.f, 0.f, 0.f );

	startPos = vPos_;
	m_SyncData.m_bIsRight = bIsRight_;

	int iLineIndex = -1;
	D3DXVECTOR3 vLandPos = startPos;
	vLandPos = pLineMap->GetLandPosition( startPos, LINE_RADIUS, &iLineIndex );
	pLineMap->IsOnLine( startPos, LINE_RADIUS + 10, &startPos, &iLineIndex, bIsRight_ );

	const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );

	if ( NULL == pLineData )
	{
		ASSERT( !"CX2RidingPet::SetPos, null linedata" );
		return;
	}

	if ( true == m_SyncData.m_bIsRight )
	{
		dirVec.x = pLineData->dirDegree.x;
		dirVec.y = pLineData->dirDegree.y;
	}
	else
	{
		dirVec.x = pLineData->dirDegree.x;
		dirVec.y = pLineData->dirDegree.y + 180.f;
	}

	//m_SyncData.m_vPos			= startPos;
	m_SyncData.m_vPos			= vPos_;
	m_ConditionData.m_vDirDegree	= dirVec;
	m_ConditionData.m_vDir			= pLineData->dirVector;
	m_ConditionData.m_vLandPosition	= vLandPos;
	m_SyncData.m_ucLastTouchLineIndex	= iLineIndex;
}
//#pragma endregion ���� ��ġ �� ȸ�� �� ���

//#pragma region InitPos
bool CX2RidingPet::InitPos()
{
	if ( true == m_bInitPos )
		return true;

	D3DXVECTOR3 vPos;
	bool bIsRight;
	GetUserInfo( vPos, bIsRight );

	if ( false == m_bInit )
	{
		CKTDGLineMap* pLineMap = NULL;

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetWorld() )
		{
			pLineMap = g_pX2Game->GetWorld()->GetLineMap();
		}
		else if ( NULL != g_pTFieldGame )
		{
			pLineMap = g_pTFieldGame->GetWorld()->GetLineMap();
		}

		if ( NULL == pLineMap )
			return false;

		if ( false == pLineMap->GetAnyEnabledLine() )
			return false;

		int iLineIndex = -1;
		D3DXVECTOR3 vStartPos = vPos;
		D3DXVECTOR3 vLandPos = vStartPos;

		vLandPos = pLineMap->GetLandPosition( vStartPos, LINE_RADIUS, &iLineIndex );
		pLineMap->IsOnLine( vStartPos, LINE_RADIUS, &vStartPos, &iLineIndex, true );

		const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( iLineIndex );

		if ( NULL == pLineData )
			return false;
	}

	vPos.y += 30.f;
	SetPos( vPos, bIsRight );
	m_bInitPos = true;

	return true;
}
//#pragma endregion ��ġ �ʱ�ȭ

//#pragma region CommonCameraFrameMove
void CX2RidingPet::CommonCameraFrameMove()
{
	KTDXPROFILE();

	if ( NULL != g_pX2Game )
	{
		CX2GUUser* pUser = static_cast<CX2GUUser*>( g_pX2Game->GetUserUnitByUID( m_UserUid ) );
		if ( NULL != pUser )
		{
			CX2GUUser::FrameData& kNowFrameData = pUser->AccessNowFrameData();
			CX2GUUser::FrameData& kFutureFrameData = pUser->AccessFutureFrameData();

			//if ( NULL == pFrameData || NULL == pFrameDataFuture )
			//	return;
			//
			int iCameraIndex = kNowFrameData.stateParam.normalCamera;

			if( true == kNowFrameData.unitCondition.bHit )
				iCameraIndex = kNowFrameData.stateParam.hitCamera;

			g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = D3DXVECTOR3( 0, 0, 0 );

			bool bTableOpen = false;

			bTableOpen = m_LuaManager.BeginTableByReference( m_StateData.m_StateID );
			{
				if( this->m_LuaManager.BeginTable( "CAMERA", iCameraIndex ) == true )
				{
					std::vector< float > vecSubCameraTiming;
					if( this->m_LuaManager.BeginTable( "SUB_CAMERA_TIMING" ) == true )
					{
						float fSubCameraTiming = 0.f;
						for( int i=1; ; i++ )
						{
							if( true == this->m_LuaManager.GetValue( i, fSubCameraTiming ) )
							{
								vecSubCameraTiming.push_back( fSubCameraTiming );
							}
							else
							{
								break;
							}
						}
						this->m_LuaManager.EndTable(); // SUB_CAMERA_TIMING
					}

					if( true == vecSubCameraTiming.empty() )
					{
						g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, this->m_LuaManager, 1 );
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
								g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, this->m_LuaManager, tableName );
								bDone = true;
								break;
							}
						}

						if( false == bDone )
						{
							char tableName[64] = "";
							StringCchPrintfA( tableName, ARRAY_SIZE(tableName), "SUB_CAMERA%d", (int)vecSubCameraTiming.size() );
							g_pX2Game->GetX2Camera()->PlayLuaCamera( pUser, m_LuaManager, tableName );
						}
					}

					this->m_LuaManager.EndTable(); // CAMERA
				}
				else
				{

				float eyeDistance = 0;
				const CKTDGLineMap::CameraData* pCameraData = g_pX2Game->GetWorld()->GetLineMap()->GetCameraData(
					kFutureFrameData.syncData.lastTouchLineIndex );

				if( NULL != pCameraData )
				{
					switch( pCameraData->m_eType )
					{
					case CKTDGLineMap::LCT_ABSOULTE_POSITION:
						{
							g_pX2Game->GetX2Camera()->GameLineCamera( pUser, pCameraData->m_vEye, pCameraData->m_vLookAt,
								pCameraData->m_bFocusUnit, 0.4f );
						} break;
					case CKTDGLineMap::LCT_RELATIE_POSITION:
						{
							g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_vFinalRelativeEyePosition = pCameraData->m_vRelativeEye;
							g_pX2Game->GetX2Camera()->GetLineScriptedCameraData().m_fSpeed = pCameraData->m_fCameraRepositionSpeed;
							g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(), 200,
								pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );
						} break;

					default:
						{
							g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(),
								200, pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );

						} break;
					}
				}
				else
				{
					if( pUser->GetGameCameraOffset().bEnable == true )
					{
						g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance() +
							pUser->GetGameCameraOffset().fCameraDistance,
							200.f + pUser->GetGameCameraOffset().fHeight,
							pUser->GetCameraAngleDegree() + pUser->GetGameCameraOffset().fAngleDegree,
							eyeDistance + pUser->GetGameCameraOffset().fEyeDistance,
							0.f + pUser->GetGameCameraOffset().fLookatDistance,
							0.3f + pUser->GetGameCameraOffset().fTrackingTime,
							0.0f + pUser->GetGameCameraOffset().fTrackingAtTime );

					}
					else
						g_pX2Game->GetX2Camera()->NomalTrackingCamera( pUser, g_pMain->GetGameOption().GetCameraDistance(), 200,
						pUser->GetCameraAngleDegree(), eyeDistance, 0.f, 0.3f );
					}
				}
			}
			if( bTableOpen == true )
				m_LuaManager.EndTable();
		}
	}
}
//#pragma endregion ī�޶� �Ľ� �� ����
