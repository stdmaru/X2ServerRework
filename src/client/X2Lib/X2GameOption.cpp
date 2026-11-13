#include "stdafx.h"
#include "./X2GameOption.h"
#include "x2/platform/process.h"
#include "x2/platform/unicode.h"

#include <cstdarg>
#include <cwchar>

#include "KBenchmark_SUM.h"
#include "KBenchmark_LINPACK.h"
#include "KBenchmark_MXM.h"

CX2GameOption::CX2GameOption(void):
m_bChangeFieldOption(false)
{
	//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현
	//}} JHKang // 2010.8.27 // 창 모드 전체 화면 구현

#ifdef ADD_MUSIC_SUB_VOLUME
	m_fMusicSubVolume = 0.f;
#endif

#ifdef PVP_ZOOM_CAMERA
	ResetAutoCamera();
#endif

	m_bFriendOld		= false;
	m_bPartyOld			= false;
	m_eFieldLevelOld	= FL_HIGH;
}

CX2GameOption::~CX2GameOption(void)
{
}

bool CX2GameOption::SetAutoOption( int index )
{
	if( index < 0 || index >= (int)m_vecOptionListPreset.size() )
		return false;

	OptionList& optionList = m_vecOptionListPreset[index];
	m_OptionList.m_iAutoSetLevel = index;

	SetUnitDetail( optionList.m_UnitDetail );

	SetMapDetail( optionList.m_MapDetail );
	SetEffectDetail( optionList.m_eEffect );


	SetFieldDetail( optionList.m_FieldDetail );


	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_PVP_GAME:
	case CX2Main::XS_TRAINING_GAME:
		{
		} break;

	default:
		{
			SetTexDetail( optionList.m_TexDetail );
		} break;
	}


	return true;
}

bool CX2GameOption::UpAutoOption()
{
	return SetAutoOption( m_OptionList.m_iAutoSetLevel+1 );
}

bool CX2GameOption::DownAutoOption()
{
	return SetAutoOption( m_OptionList.m_iAutoSetLevel-1 );
}

void CX2GameOption::SetTexDetail( OptionLevel optionLevel, bool bForce /*= false*/ )
{
	if( optionLevel < m_OptionList.m_MaxTexDetail )
		optionLevel = m_OptionList.m_MaxTexDetail;

	if( false == bForce )
	{
		if( m_OptionList.m_TexDetail == optionLevel )
			return;
	}


	m_OptionList.m_TexDetail = optionLevel;

	switch( m_OptionList.m_TexDetail )
	{
	case OL_HIGH:
		{
			g_pKTDXApp->GetDeviceManager()->SetTextureDetailLevel( CKTDXDeviceTexture::DL_HIGH );
		}
		break;

	case OL_MEDIUM:
		{
			g_pKTDXApp->GetDeviceManager()->SetTextureDetailLevel( CKTDXDeviceTexture::DL_MEDIUM );
		}
		break;

	case OL_LOW:
		{
			g_pKTDXApp->GetDeviceManager()->SetTextureDetailLevel( CKTDXDeviceTexture::DL_LOW );
		}
		break;
	}
}

void CX2GameOption::UpTexDetail()
{
	if( m_OptionList.m_TexDetail == OL_HIGH )
		return;

	switch( m_OptionList.m_TexDetail )
	{
	case OL_MEDIUM:
		{
			SetTexDetail( OL_HIGH );
		}
		break;

	case OL_LOW:
		{
			SetTexDetail( OL_MEDIUM );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::DownTexDetail()
{
	if( m_OptionList.m_TexDetail == OL_LOW )
		return;

	switch( m_OptionList.m_TexDetail )
	{
	case OL_HIGH:
		{
			SetTexDetail( OL_MEDIUM );
		}
		break;

	case OL_MEDIUM:
		{
			SetTexDetail( OL_LOW );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::SetUnitDetail( OptionLevel optionLevel, bool bForce /*= false*/  )
{

	if( m_OptionList.m_MaxUnitDetail == OL_HIGH )
	{
		// depth buffer format이 16bit이면 unit의 외곽선을 그리지 않도록~
		DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
		if( deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D16_LOCKABLE ||
			deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D16 ||
			deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D15S1 )
		{
			m_OptionList.m_MaxUnitDetail = OL_MEDIUM;
		}
	}

	if( optionLevel < m_OptionList.m_MaxUnitDetail )
		optionLevel = m_OptionList.m_MaxUnitDetail;

	if( false == bForce )
	{
		if( m_OptionList.m_UnitDetail == optionLevel )
			return;
	}

	m_OptionList.m_UnitDetail = optionLevel;
}

void CX2GameOption::UpUnitDetail()
{
	if( m_OptionList.m_UnitDetail == OL_HIGH )
		return;

	switch( m_OptionList.m_UnitDetail )
	{
	case OL_MEDIUM:
		{
			SetUnitDetail( OL_HIGH );
		}
		break;

	case OL_LOW:
		{
			SetUnitDetail( OL_MEDIUM );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::DownUnitDetail()
{
	if( m_OptionList.m_UnitDetail == OL_LOW )
		return;

	switch( m_OptionList.m_UnitDetail )
	{
	case OL_HIGH:
		{
			SetUnitDetail( OL_MEDIUM );
		}
		break;

	case OL_MEDIUM:
		{
			SetUnitDetail( OL_LOW );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::SetMapDetail( OptionLevel optionLevel, bool bForce /*= false*/ )
{
	if( optionLevel < m_OptionList.m_MaxMapDetail )
		optionLevel = m_OptionList.m_MaxMapDetail;

	if( false == bForce )
	{
		if( m_OptionList.m_MapDetail == optionLevel )
			return;
	}

	m_OptionList.m_MapDetail = optionLevel;

	if( g_pX2Game != NULL )
	{
		if( g_pX2Game->GetWorld() != NULL )
		{
			g_pX2Game->GetWorld()->SetMapDetail( m_OptionList.m_MapDetail );
		}
	}
	if( g_pSquareGame != NULL )
	{
		if( g_pSquareGame->GetWorld() != NULL )
		{
			g_pSquareGame->GetWorld()->SetMapDetail( m_OptionList.m_MapDetail );
		}
	}
	else if(g_pTFieldGame != NULL)
	{
		if( g_pTFieldGame->GetWorld() != NULL )
		{
			g_pTFieldGame->GetWorld()->SetMapDetail( m_OptionList.m_MapDetail );
		}
	}
}

void CX2GameOption::UpMapDetail()
{
	if( m_OptionList.m_MapDetail == OL_HIGH )
		return;

	switch( m_OptionList.m_MapDetail )
	{
	case OL_MEDIUM:
		{
			SetMapDetail( OL_HIGH );
		}
		break;

	case OL_LOW:
		{
			SetMapDetail( OL_MEDIUM );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::DownMapDetail()
{
	if( m_OptionList.m_MapDetail == OL_LOW )
		return;

	switch( m_OptionList.m_MapDetail )
	{
	case OL_HIGH:
		{
			SetMapDetail( OL_MEDIUM );
		}
		break;

	case OL_MEDIUM:
		{
			SetMapDetail( OL_LOW );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::SetEffectDetail( OptionLevel optionLevel, bool bForce /*= false*/ )
{
	if( false == bForce )
	{
		if( m_OptionList.m_eEffect == optionLevel )
			return;
	}

	m_OptionList.m_eEffect = optionLevel;

	if( g_pData != NULL )
	{
		if( g_pData->GetGameMinorParticle() != NULL )
			g_pData->GetGameMinorParticle()->SetEnable( m_OptionList.m_eEffect != OL_LOW );

		if( g_pData->GetGameMinorXMeshPlayer() != NULL )
		{
			g_pData->GetGameMinorXMeshPlayer()->SetEnable( m_OptionList.m_eEffect != OL_LOW );
			g_pData->GetGameMinorXMeshPlayer()->SetEnableSlashTrace( m_OptionList.m_eEffect != OL_LOW );
		}

        if ( g_pX2Game != NULL )
        {
		    for( int i = 0; i < (int) g_pX2Game->GetUnitNum(); i++ )
		    {
			    CX2GameUnit* pCX2GameUnit = g_pX2Game->GetUnit(i);
			    //if( true == g_pX2Game->IsValidUnit( pCX2GameUnit ) )
			    if ( NULL != pCX2GameUnit )
			    {
				    pCX2GameUnit->InitEffect();
                    if( m_OptionList.m_eEffect == OL_LOW )
				    {
					    pCX2GameUnit->DeleteMinorParticle();
				    }
			    }
		    }
        }
//#ifdef X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
//		//if( g_pData->GetGameMajorXMeshPlayer() != NULL )
//			//g_pData->GetGameMajorXMeshPlayer()->SetEnableSlashTrace( m_OptionList.m_eEffect != OL_LOW );
//#else//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
//		if( g_pData->GetGameMajorXMeshPlayer() != NULL )
//			g_pData->GetGameMajorXMeshPlayer()->SetEnableSlashTrace( m_OptionList.m_bEffect );
//#endif//X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION
	}
}

void CX2GameOption::UpEffectDetail()
{
	if( m_OptionList.m_eEffect == OL_HIGH )
		return;

	switch( m_OptionList.m_eEffect )
	{
	case OL_MEDIUM:
		{
			SetEffectDetail( OL_HIGH );
		}
		break;

	case OL_LOW:
		{
			SetEffectDetail( OL_MEDIUM );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::DownEffectDetail()
{
	if( m_OptionList.m_eEffect == OL_LOW )
		return;

	switch( m_OptionList.m_eEffect )
	{
	case OL_HIGH:
		{
			SetEffectDetail( OL_MEDIUM );
		}
		break;

	case OL_MEDIUM:
		{
			SetEffectDetail( OL_LOW );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::SetFieldDetail( FieldLevel optionLevel, bool bForce /*= false*/ )
{
	if( optionLevel < m_OptionList.m_MaxFieldDetail )
		optionLevel = m_OptionList.m_MaxFieldDetail;

	if( false == bForce )
	{
		if( m_OptionList.m_FieldDetail == optionLevel )
			return;
	}

	m_OptionList.m_FieldDetail = optionLevel;

	// 서버로 변경된 필드옵션을 보낸다.
	SendFieldOption();
	m_bChangeFieldOption = true;
}

void CX2GameOption::UpFieldDetail()
{
	if( m_OptionList.m_FieldDetail == FL_HIGH )
		return;

	switch( m_OptionList.m_FieldDetail )
	{
	case FL_MEDIUM:
		{
			SetFieldDetail( FL_HIGH );
		}
		break;

	case FL_LOW:
		{
			SetFieldDetail( FL_MEDIUM );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::DownFieldDetail()
{
	if( m_OptionList.m_FieldDetail == FL_LOW )
		return;

	switch( m_OptionList.m_FieldDetail )
	{
	case FL_HIGH:
		{
			SetFieldDetail( FL_MEDIUM );
		}
		break;

	case FL_MEDIUM:
		{
			SetFieldDetail( FL_LOW );
		}
		break;
	default:
		break;
	}
}

void CX2GameOption::SetFieldParty( bool bParty )
{
	m_OptionList.m_bParty = bParty;

	// 서버로 변경된 옵션설정을 보낸다.
	//SendFieldOption();
	m_bChangeFieldOption = true;



	if ( NULL != g_pData->GetRelationshipEffectManager() )
	{
		if ( TRUE == GetFieldSD() )
		{
			g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect ( CX2RelationshipEffectManager::REST_HIDE_ALL );
		}
		else
		{
			if ( TRUE == GetFieldParty() )
				g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect( CX2RelationshipEffectManager::REST_HIDE_NOT_PARTY_ONLY );
			else
				g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect( CX2RelationshipEffectManager::REST_SHOW_ALL );
		}
	}
}

void CX2GameOption::SetFieldFriend( bool /*bFriend*/ )
{
	//m_OptionList.m_bFriend = bFriend;

	// 서버로 변경된 옵션설정을 보낸다.
	//SendFieldOption();
	m_bChangeFieldOption = true;
}

void CX2GameOption::SetFieldSD( bool bSD )
{
	m_OptionList.m_bSD = bSD;

	if ( NULL != g_pData->GetRelationshipEffectManager() )
	{
		if ( TRUE == GetFieldSD() )
		{
			g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect ( CX2RelationshipEffectManager::REST_HIDE_ALL );
		}
		else
		{
			if ( TRUE == GetFieldParty() )
				g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect( CX2RelationshipEffectManager::REST_SHOW_PARTY_ONLY );
			else
				g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect( CX2RelationshipEffectManager::REST_SHOW_ALL );
		}
	}
}
void CX2GameOption::SetEnable3DSound( bool bEnable3DSound )
{
	m_OptionList.m_bEnable3DSound = bEnable3DSound;
	if( g_pKTDXApp->GetDSManager() != NULL &&
		g_pKTDXApp->GetDSManager()->GetInit() == true )
	{
		g_pKTDXApp->GetDSManager()->SetEnable3DSound( bEnable3DSound );
	}
}

void CX2GameOption::SetMusic( bool bMusic )
{
	m_OptionList.m_bMusic = bMusic;
	if( g_pKTDXApp->GetDSManager() != NULL &&
		g_pKTDXApp->GetDSManager()->GetMP3Play() != NULL &&
		g_pKTDXApp->GetDSManager()->GetInit() == true )
	{
		g_pKTDXApp->GetDSManager()->GetMP3Play()->SetPaused( !bMusic );
	}
}

void CX2GameOption::SetSound( bool bSound )
{
	m_OptionList.m_bSound = bSound;
	g_pKTDXApp->GetDSManager()->SetSoundMute( !bSound );


	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() )
	{
		g_pX2Game->GetWorld()->PlayEffectBGM( bSound );
		g_pX2Game->GetWorld()->Play3DEffectBGM( bSound );
	}

	if( NULL != g_pTFieldGame &&
		NULL != g_pTFieldGame->GetWorld() )
	{
		g_pTFieldGame->GetWorld()->PlayEffectBGM( bSound );
		g_pTFieldGame->GetWorld()->Play3DEffectBGM( bSound );
	}

	//if( NULL != g_pSquareGame &&
	//	NULL != g_pSquareGame->GetWorld() )
	//{
	//	g_pSquareGame->GetWorld()->PlayEffectBGM( bSound );
	//}
}

void CX2GameOption::SetMusicVolume( float fMusicVolume )
{
#ifdef ADD_MUSIC_SUB_VOLUME
	if( fMusicVolume >= 0.f )
		fMusicVolume = -1.f;

	float fNewMusicVolume = fMusicVolume;
	fNewMusicVolume += m_fMusicSubVolume;
	if( fNewMusicVolume >= 0.f )
		fNewMusicVolume = -1.f;
	if( fNewMusicVolume < -3000.f )
		fNewMusicVolume = -3000.f;

	m_OptionList.m_fMusicVolume = fMusicVolume;
	if( g_pKTDXApp->GetDSManager() != NULL && g_pKTDXApp->GetDSManager()->GetMP3Play() != NULL && g_pKTDXApp->GetDSManager()->GetInit() == true )
		g_pKTDXApp->GetDSManager()->GetMP3Play()->SetVolume( (int)fNewMusicVolume );
#else

	if( fMusicVolume >= 0.f )
		fMusicVolume = -1.f;

	m_OptionList.m_fMusicVolume = fMusicVolume;
	if( g_pKTDXApp->GetDSManager() != NULL && g_pKTDXApp->GetDSManager()->GetMP3Play() != NULL && g_pKTDXApp->GetDSManager()->GetInit() == true )
		g_pKTDXApp->GetDSManager()->GetMP3Play()->SetVolume( (int)fMusicVolume );

#endif
}

void CX2GameOption::SetSoundVolume( float fSoundVolume )
{
	m_OptionList.m_fSoundVolume = fSoundVolume;
	g_pKTDXApp->GetDSManager()->SetSoundVolume( fSoundVolume );
}

void CX2GameOption::SetResolution( DWORD dwWidth, DWORD dwHeight, int iColorBit /*=32*/ )
{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Pause();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	float fNear = g_pKTDXApp->GetDGManager()->GetNear();
	float fFar = g_pKTDXApp->GetDGManager()->GetFar();
	float fPerspective = g_pKTDXApp->GetDGManager()->GetPerspectiveValue();
	bool bPerspective = g_pKTDXApp->GetDGManager()->GetPerspective();

	//{{ JHKang // 2010.8.30 // 창 모드 전체 화면 구현
	DXUTDeviceSettings deviceSettings	= DXUTGetDeviceSettings();

	deviceSettings.pp.BackBufferWidth	= dwWidth;
	deviceSettings.pp.BackBufferHeight	= dwHeight;

	if( iColorBit == 16 )
	{
		deviceSettings.AdapterFormat		= D3DFMT_R5G6B5;
		deviceSettings.pp.BackBufferFormat	= D3DFMT_R5G6B5;
	}

	if( deviceSettings.pp.MultiSampleType != D3DMULTISAMPLE_NONE )
	{
		deviceSettings.pp.Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}

	if( S_OK == DXUTCreateDeviceFromSettings( &deviceSettings ) )
	{
		DXUTSetFullscreenClientRectAtModeChange( (int)dwWidth, (int)dwHeight );
		DXUTSetWindowClientRectAtModeChange( (int)dwWidth, (int)dwHeight );

		m_OptionList.m_vResolution.x = (float)dwWidth;
		m_OptionList.m_vResolution.y = (float)dwHeight;
	}
	else
	{
		// 해상도 변경에 실패하면 게임옵션파일에 default 값으로 저장한다
		m_OptionList.m_vResolution = D3DXVECTOR2( 1024, 768 );
		SaveScriptFile();
	}
	//{{ JHKang // 2010.8.30 // 창 모드 전체 화면 구현

	g_pKTDXApp->GetDGManager()->SetProjection( fNear, fFar, bPerspective );
	g_pKTDXApp->GetDGManager()->SetPerspectiveValue(fPerspective);

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

    if ( g_pData->GetGameUDP() != NULL )
        g_pData->GetGameUDP()->RemoveAllPendingPingSends();
}

bool CX2GameOption::OpenScriptFile()
{
    const WCHAR pFileName[256] = L"GameOption.txt";			// X2/dat 폴더에 있는 default script
	const WCHAR pSaveFileName[256] = L"GameOptions.lua";	// 사용자가 설정을 바꾸고 게임을 끝냈을때 저장되는 script

	// gameoptions.lua 파일을 읽어서 UTF-8 포맷이 아니면 변환한다


	bool bDoneParsingUserGameOptionFile = true;


    KLuaManagerProxy luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );
    {
        if ( g_pKTDXApp->LoadAndDoMemory_LocalFile( &luaManager, pSaveFileName ) == false )
            bDoneParsingUserGameOptionFile = false;
        if ( bDoneParsingUserGameOptionFile == false )
        {
            if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, pFileName ) == false )
            {
                return false;
            }
        }
    }



	if( true == luaManager.BeginTable( "GAME_OPTION_CURRENT" ) )
	{
		LUA_GET_VALUE( luaManager, "AUTOSET_LEVEL",				m_OptionList.m_iAutoSetLevel,		3 );
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_UNIT", 			m_OptionList.m_UnitDetail,			OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_TEXTURE",			m_OptionList.m_TexDetail,			OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_MAP",				m_OptionList.m_MapDetail,			OptionLevel,		OL_HIGH	);
		//기존의 GRAPHIC_EFFECT를 GRAPHIC_EFFECT_VER2로 변경 시도
		bool bEffect;
		if( luaManager.GetValue( "GRAPHIC_EFFECT", bEffect ) == true )
			m_OptionList.m_eEffect = ( bEffect ) ? CX2GameOption::OL_HIGH : CX2GameOption::OL_LOW;
		else
			LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_EFFECT_VER2",	m_OptionList.m_eEffect,		OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE( luaManager, "RESOLUTION_X",					m_OptionList.m_vResolution.x,		1024 );
		LUA_GET_VALUE( luaManager, "RESOLUTION_Y",					m_OptionList.m_vResolution.y,		768 );
		if( false == bDoneParsingUserGameOptionFile )
			m_OptionList.m_vResolution = D3DXVECTOR2( GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ) );

		LUA_GET_VALUE( luaManager, "ENABLE_3D_SOUND",				m_OptionList.m_bEnable3DSound,		true );
		LUA_GET_VALUE( luaManager, "SOUND",						m_OptionList.m_bSound,				true );
		LUA_GET_VALUE( luaManager, "MUSIC",						m_OptionList.m_bMusic,				true );
		LUA_GET_VALUE( luaManager, "SOUND_VOLUME",					m_OptionList.m_fSoundVolume,		-800.f );
		LUA_GET_VALUE( luaManager, "MUSIC_VOLUME",					m_OptionList.m_fMusicVolume,		-2000.f );

		LUA_GET_VALUE( luaManager, "DYNAMIC_CAMERA",				m_OptionList.m_bDynamicCamera,		true );
		LUA_GET_VALUE( luaManager, "CAMERA_ZOOM_LEVEL",			m_OptionList.m_iZoomLevel,			0 );

		LUA_GET_VALUE_ENUM( luaManager, "FIELD_LEVEL",				m_OptionList.m_FieldDetail,			FieldLevel,			FL_HIGH );
		LUA_GET_VALUE( luaManager, "FIELD_PARTY",					m_OptionList.m_bParty,				false );
		LUA_GET_VALUE( luaManager, "FIELD_SD",						m_OptionList.m_bSD,					false );


		LUA_GET_VALUE( luaManager, "CHARINFO_LEVEL",				m_OptionList.m_bLevel,				true );
		LUA_GET_VALUE( luaManager, "CHARINFO_PVP",					m_OptionList.m_bPvpRank,			true );
		LUA_GET_VALUE( luaManager, "CHARINFO_GUILD",				m_OptionList.m_bGuild,				true );
		LUA_GET_VALUE( luaManager, "CHARINFO_NOTHING",				m_OptionList.m_bNothing,			true );
		//LUA_GET_VALUE( luaManager, "FIELD_FRIEND",					m_OptionList.m_bFriend,				true );

		LUA_GET_VALUE( luaManager, "FULLSCREEN",					m_OptionList.m_bFullScreen,			false );

		LUA_GET_VALUE( luaManager, "FLASHEFFECT",					m_OptionList.m_bFlashEffect,		false );

		//{{ kimhc // 2010.3.12 //	채팅창 개편
		LUA_GET_VALUE( luaManager, "DISPLAY_OBTAINING_ED",			m_OptionList.m_bDisplayObtainingED,		true );
		LUA_GET_VALUE( luaManager, "DISPLAY_OBTAINING_EXP",		m_OptionList.m_bDisplayObtainingEXP,	true );
		//}} kimhc // 2010.3.12 //	채팅창 개편

		LUA_GET_VALUE( luaManager, "JOY_ENABLE",					m_OptionList.m_bJoyEnable,			false );


		LUA_GET_VALUE( luaManager, "VERTICALSYNC",					m_OptionList.m_bVerticalSync,		false );
		LUA_GET_VALUE( luaManager, "FRAME_LIMIT",					m_OptionList.m_iFrameLimit,			0 );
		LUA_GET_VALUE( luaManager, "MULTISAMPLE",					m_OptionList.m_iMultisample,		1 );
		LUA_GET_VALUE( luaManager, "ANISOTROPY",					m_OptionList.m_iAnisotropy,			1 );
		LUA_GET_VALUE( luaManager, "SUPERSAMPLE",					m_OptionList.m_iSupersample,		1 );
		LUA_GET_VALUE_ENUM( luaManager, "SKILL_CUTIN_SHOW", 		m_OptionList.m_eSkillCutinShowState,	SKILL_CUTIN_SHOW_STATE,		SCSS_ONESELF );

		LUA_GET_VALUE( luaManager, "SKILL_SLOT_UI_TYPE_A",			m_OptionList.m_bIsSkillUITypeA,		true );

		m_OptionList.m_vecPvpMap.clear();
		if( luaManager.BeginTable( "PVP_MAP" ) == true )
		{
			for( int tableIndex = 1, iPvpMapId = -1; tableIndex <= 2 ; ++tableIndex )
			{
				if ( luaManager.GetValue( tableIndex, iPvpMapId ) == true )
					m_OptionList.m_vecPvpMap.push_back( iPvpMapId );
			}

			luaManager.EndTable();
		}


		int iServerGroupID = 0;
		LUA_GET_VALUE( luaManager, "SERVER_GROUP_ID",		iServerGroupID,	0 );
		g_pInstanceData->SetServerGroupID( iServerGroupID );


		luaManager.EndTable();
	}

	if( false == bDoneParsingUserGameOptionFile )
	{
		//"최적의 게임 옵션을 자동으로 설정하려면 "예"를 그렇지 않으면 "아니요"를 눌러주세요." 메세지 출력
		if( MessageBoxW( g_pKTDXApp->GetHWND(), GET_STRING( STR_ID_29006 ), GET_STRING( STR_ID_29042 ), MB_YESNO | MB_TOPMOST ) == IDYES )
		{
			_DecideProperGraphicOption();
		}
		// Saved now, so the question is asked once (the original saved only after a login or an option change).
		SaveScriptFile();
	}

	m_eFieldLevelOld = m_OptionList.m_FieldDetail;
	m_bPartyOld = m_OptionList.m_bParty;
	//m_bFriendOld = m_OptionList.m_bFriend;

	//시스템 메모리가 256 이하이거나 비디오 램이 64 이하이면
	//텍스쳐, 이펙트 설정을 최하로 낮추고 해상도를 800/600 이하로 낮춘다.
	DWORD totalTextureMem		= 0;
	DWORD availableTextureMem	= 0;
	g_pKTDXApp->GetVideoRam( totalTextureMem, availableTextureMem );
	totalTextureMem = totalTextureMem/1024/1024;

	DWORD totalPhysicMem		= 0;
	DWORD availablePhysicMem	= 0;
	g_pKTDXApp->GetPhysicRam( totalPhysicMem, availablePhysicMem );
	totalPhysicMem = totalPhysicMem/1024/1024;

	if( totalTextureMem <= 60 || totalPhysicMem < 300 )
	{
		// 비디오 메모리가 일정 크기 이하이면 unit설정을 medium을 최대로
		m_OptionList.m_MaxUnitDetail	= OL_MEDIUM;
		// 텍스쳐 설정도 최하로 바꾸도록 하자
		m_OptionList.m_MaxTexDetail		= OL_MEDIUM;
		m_OptionList.m_MaxMapDetail		= OL_MEDIUM;
		m_OptionList.m_MaxFieldDetail	= FL_MEDIUM;
	}

	// depth buffer format이 16bit이면 unit의 외곽선을 그리지 않도록~
	DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
	if( deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D16_LOCKABLE ||
		deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D16 ||
		deviceSettings.pp.AutoDepthStencilFormat == D3DFMT_D15S1 )
	{
		m_OptionList.m_MaxUnitDetail	= OL_MEDIUM;
	}

	//ApplyAllOption();

    const WCHAR pPresetFileName[256] = L"GameOptionPreset.txt";

//{{ robobeg : 2008-10-28
    {
        if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, pPresetFileName ) == false )
        {
            return false;
        }
    }
//}} robobeg : 2008-10-28

	m_vecOptionListPreset.clear();
	int iTableIndex = 0;
	while( true == luaManager.BeginTable( "GAME_OPTION_PRESET", iTableIndex ) )
	{
		OptionList optionList;
		LUA_GET_VALUE( luaManager, "AUTOSET_LEVEL",				optionList.m_iAutoSetLevel,		3 );
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_UNIT", 			optionList.m_UnitDetail,		OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_TEXTURE",			optionList.m_TexDetail,			OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_MAP",				optionList.m_MapDetail,			OptionLevel,		OL_HIGH	);
		LUA_GET_VALUE_ENUM( luaManager, "GRAPHIC_EFFECT_VER2",	optionList.m_eEffect,			OptionLevel,		OL_HIGH );

		//LUA_GET_VALUE( luaManager, "RESOLUTION_X",					optionList.m_vResolution.x,		640 );
		//LUA_GET_VALUE( luaManager, "RESOLUTION_Y",					optionList.m_vResolution.y,		480 );

		m_vecOptionListPreset.push_back( optionList );

		luaManager.EndTable();
		iTableIndex++;
	}



	return true;
}




namespace
{
	// The file is UTF-8 with a BOM. fwprintf after fputc writes nothing on glibc (a stream is either byte or wide
	// oriented), so each line is formatted as wide text and written as UTF-8 bytes.
	void FilePrintf( FILE* file, const wchar_t* format, ... )
	{
		wchar_t buffer[512];
		va_list args;
		va_start( args, format );
		const int length = vswprintf( buffer, std::size( buffer ), format, args );
		va_end( args );
		if ( length > 0 )
		{
			const std::string utf8 = x2::platform::WideToUtf8( std::wstring( buffer, static_cast<size_t>( length ) ) );
			fwrite( utf8.data(), 1, utf8.size(), file );
		}
	}
}

bool CX2GameOption::SaveScriptFile()
{

	const string strFileName = "GameOptions.lua";


	FILE* file = NULL;
	file = fopen( strFileName.c_str(), "w" );
	if( NULL == file )
	{
		ErrorLogMsg( XEM_ERROR7, strFileName.c_str() );
		return false;
	}

    fputc( 0xEF, file );
    fputc( 0xBB, file );
    fputc( 0xBF, file );

	FilePrintf( file, L"  \n" );
	FilePrintf( file, L"  \n" );
	FilePrintf( file, L"GAME_OPTION_CURRENT = \n" );
	FilePrintf( file, L"{ \n" );


	FilePrintf( file, L"\t-- GRAPHIC OPTION \n" );
	FilePrintf( file, L"\tAUTOSET_LEVEL		= %d, \n", m_OptionList.m_iAutoSetLevel );

	switch( m_OptionList.m_UnitDetail )
	{
	case OL_HIGH:
		FilePrintf( file, L"\tGRAPHIC_UNIT		= OPTION_LEVEL[\"OL_HIGH\"], \n" );
		break;
	case OL_MEDIUM:
		FilePrintf( file, L"\tGRAPHIC_UNIT		= OPTION_LEVEL[\"OL_MEDIUM\"], \n" );
		break;
	case OL_LOW:
		FilePrintf( file, L"\tGRAPHIC_UNIT		= OPTION_LEVEL[\"OL_LOW\"], \n" );
		break;
	}

	switch( m_OptionList.m_TexDetail )
	{
	case OL_HIGH:
		FilePrintf( file, L"\tGRAPHIC_TEXTURE		= OPTION_LEVEL[\"OL_HIGH\"], \n" );
		break;
	case OL_MEDIUM:
		FilePrintf( file, L"\tGRAPHIC_TEXTURE		= OPTION_LEVEL[\"OL_MEDIUM\"], \n" );
		break;
	case OL_LOW:
		FilePrintf( file, L"\tGRAPHIC_TEXTURE		= OPTION_LEVEL[\"OL_LOW\"], \n" );
		break;
	}

	switch( m_OptionList.m_MapDetail )
	{
	case OL_HIGH:
		FilePrintf( file, L"\tGRAPHIC_MAP			= OPTION_LEVEL[\"OL_HIGH\"], \n" );
		break;
	case OL_MEDIUM:
		FilePrintf( file, L"\tGRAPHIC_MAP			= OPTION_LEVEL[\"OL_MEDIUM\"], \n" );
		break;
	case OL_LOW:
		FilePrintf( file, L"\tGRAPHIC_MAP			= OPTION_LEVEL[\"OL_LOW\"], \n" );
		break;
	}

	switch( m_OptionList.m_eEffect )
	{
		case OL_HIGH:
			FilePrintf( file, L"\tGRAPHIC_EFFECT_VER2	= OPTION_LEVEL[\"OL_HIGH\"], \n" );
			break;
		case OL_MEDIUM:
			FilePrintf( file, L"\tGRAPHIC_EFFECT_VER2	= OPTION_LEVEL[\"OL_MEDIUM\"], \n" );
			break;
		case OL_LOW:
			FilePrintf( file, L"\tGRAPHIC_EFFECT_VER2	= OPTION_LEVEL[\"OL_LOW\"], \n" );
			break;
	}


	FilePrintf( file, L"\tRESOLUTION_X		= %d, \n", (int)m_OptionList.m_vResolution.x );
	FilePrintf( file, L"\tRESOLUTION_Y		= %d, \n", (int)m_OptionList.m_vResolution.y );

	//{{ kimhc // 2010.3.12 //	채팅창 개편
	if( true == m_OptionList.m_bFullScreen )
		FilePrintf( file, L"\tFULLSCREEN			= TRUE, \n" );
	else
		FilePrintf( file, L"\tFULLSCREEN			= FALSE, \n" );
	//}} kimhc // 2010.3.12 //	채팅창 개편

	if( true == m_OptionList.m_bJoyEnable )
		FilePrintf( file, L"\tJOY_ENABLE			= TRUE, \n" );
	else
		FilePrintf( file, L"\tJOY_ENABLE			= FALSE, \n" );

	if ( true == m_OptionList.m_bFlashEffect )
		FilePrintf( file, L"\tFLASHEFFECT			= TRUE, \n" );
	else
		FilePrintf( file, L"\tFLASHEFFECT			= FALSE, \n" );


	FilePrintf( file, L"\n\n\n" );
	FilePrintf( file, L"\t-- SOUND OPTION \n" );


	if( true == m_OptionList.m_bSound )
		FilePrintf( file, L"\tSOUND			= TRUE, \n" );
	else
		FilePrintf( file, L"\tSOUND			= FALSE, \n" );

	if( true == m_OptionList.m_bMusic )
		FilePrintf( file, L"\tMUSIC			= TRUE, \n" );
	else
		FilePrintf( file, L"\tMUSIC			= FALSE, \n" );



	if( true == m_OptionList.m_bEnable3DSound )
		FilePrintf( file, L"\tENABLE_3D_SOUND			= TRUE, \n" );
	else
		FilePrintf( file, L"\tENABLE_3D_SOUND			= FALSE, \n" );




	FilePrintf( file, L"\tSOUND_VOLUME		= %d, \n", (int)m_OptionList.m_fSoundVolume );
	FilePrintf( file, L"\tMUSIC_VOLUME		= %d, \n", (int)m_OptionList.m_fMusicVolume );




	FilePrintf( file, L"\n\n\n" );
	FilePrintf( file, L"\t-- OTHER OPTION \n" );


//{{ kimhc // 2010.3.12 //	채팅창 개편
//}} kimhc // 2010.3.12 //	채팅창 개편


	if( true == m_OptionList.m_bDynamicCamera )
		FilePrintf( file, L"\tDYNAMIC_CAMERA		= TRUE, \n" );
	else
		FilePrintf( file, L"\tDYNAMIC_CAMERA		= FALSE, \n" );

	FilePrintf( file, L"\tCAMERA_ZOOM_LEVEL		= %d, \n", m_OptionList.m_iZoomLevel );


	if ( true == m_OptionList.m_bVerticalSync )
		FilePrintf( file, L"\tVERTICALSYNC			= TRUE, \n" );
	else
		FilePrintf( file, L"\tVERTICALSYNC			= FALSE, \n" );
	FilePrintf( file, L"\tFRAME_LIMIT			= %d, \n", m_OptionList.m_iFrameLimit );
	FilePrintf( file, L"\tMULTISAMPLE			= %d, \n", m_OptionList.m_iMultisample );
	FilePrintf( file, L"\tANISOTROPY			= %d, \n", m_OptionList.m_iAnisotropy );
	FilePrintf( file, L"\tSUPERSAMPLE			= %d, \n", m_OptionList.m_iSupersample );


	switch( m_OptionList.m_eSkillCutinShowState )
	{
	case SCSS_ONESELF:
		FilePrintf( file, L"\tSKILL_CUTIN_SHOW		= SKILL_CUTIN_SHOW_STATE[\"SCSS_ONESELF\"], \n" );
		break;
	case SCSS_PARTY_MEMBER:
		FilePrintf( file, L"\tSKILL_CUTIN_SHOW		= SKILL_CUTIN_SHOW_STATE[\"SCSS_PARTY_MEMBER\"], \n" );
		break;
	case SCSS_HIDE:
		FilePrintf( file, L"\tSKILL_CUTIN_SHOW		= SKILL_CUTIN_SHOW_STATE[\"SCSS_HIDE\"], \n" );
		break;
	}

	if( true == m_OptionList.m_bIsSkillUITypeA )
		FilePrintf( file, L"\tSKILL_SLOT_UI_TYPE_A		= TRUE, \n" );
	else
		FilePrintf( file, L"\tSKILL_SLOT_UI_TYPE_A		= FALSE, \n" );




	FilePrintf( file, L"\n\n\n" );
	FilePrintf( file, L"\t-- FIELD OPTION \n" );
	switch( m_OptionList.m_FieldDetail )
	{
	case FL_HIGH:
		FilePrintf( file, L"\tFIELD_LEVEL			= FIELD_LEVEL[\"FL_HIGH\"], \n" );
		break;
	case FL_MEDIUM:
		FilePrintf( file, L"\tFIELD_LEVEL			= FIELD_LEVEL[\"FL_MEDIUM\"], \n" );
		break;
	case FL_LOW:
		FilePrintf( file, L"\tFIELD_LEVEL			= FIELD_LEVEL[\"FL_LOW\"], \n" );
		break;
	}
	FilePrintf( file, L"\tFIELD_PARTY		= %d, \n", m_OptionList.m_bParty );
	FilePrintf( file, L"\tFIELD_SD		= %d, \n", m_OptionList.m_bSD );
	//FilePrintf( file, L"\tFIELD_FRIEND		= %d, \n", m_OptionList.m_bFriend );

	// 캐릭터 정보 저장
	FilePrintf( file, L"\tCHARINFO_LEVEL		= %d, \n", m_OptionList.m_bLevel );
	FilePrintf( file, L"\tCHARINFO_PVP		= %d, \n", m_OptionList.m_bPvpRank );
	FilePrintf( file, L"\tCHARINFO_GUILD		= %d, \n", m_OptionList.m_bGuild );
	FilePrintf( file, L"\tCHARINFO_NOTHING	= %d, \n", m_OptionList.m_bNothing );

	// 대전 맵 설정
	if( m_OptionList.m_vecPvpMap.size() > 0 )
	{
		FilePrintf( file, L"\tPVP_MAP = { " );
		for(unsigned int i=0; i<m_OptionList.m_vecPvpMap.size(); ++i)
		{
			FilePrintf( file, L"%d, ", m_OptionList.m_vecPvpMap[i] );
		}
		FilePrintf( file, L"},\n" );
	}

	FilePrintf( file, L"\tSERVER_GROUP_ID		= %d, \n", g_pInstanceData->GetServerGroupID() );


	FilePrintf( file, L"} \n" );

	fclose(file);

	return true;
}

void CX2GameOption::ApplyAllOption( bool bForce /*= false*/ )
{
	SetVerticalSync( m_OptionList.m_bVerticalSync );
	SetFrameLimit( m_OptionList.m_iFrameLimit );
	SetMultisample( m_OptionList.m_iMultisample );
	SetAnisotropy( m_OptionList.m_iAnisotropy );
	SetSupersample( m_OptionList.m_iSupersample );

	// GRAPHIC
	SetUnitDetail( m_OptionList.m_UnitDetail, bForce );
	SetTexDetail( m_OptionList.m_TexDetail, bForce );
	SetMapDetail( m_OptionList.m_MapDetail, bForce );
	SetEffectDetail( m_OptionList.m_eEffect, bForce);
	SetFieldDetail( m_OptionList.m_FieldDetail, bForce );

	SetResolution( (DWORD)m_OptionList.m_vResolution.x, (DWORD)m_OptionList.m_vResolution.y );

	SetFlashEffect( m_OptionList.m_bFlashEffect );

	// SOUND
	SetSound( m_OptionList.m_bSound );
	SetMusic( m_OptionList.m_bMusic );
	SetSoundVolume( m_OptionList.m_fSoundVolume );
	SetMusicVolume( m_OptionList.m_fMusicVolume );
	SetEnable3DSound( m_OptionList.m_bEnable3DSound );

	// OTHER
	SetDynamicCamera( m_OptionList.m_bDynamicCamera );
	SetPlayGuide( m_OptionList.m_bPlayGuide, false );
	CameraZoomIn( 0 );
	SetFullScreen( m_OptionList.m_bFullScreen );
	SetStateCutinShowState(m_OptionList.m_eSkillCutinShowState);
	SetIsSkillUITypeA(m_OptionList.m_bIsSkillUITypeA);

	SetJoyEnable( m_OptionList.m_bJoyEnable );
}



void CX2GameOption::SetPlayGuide( bool bGuide, bool bUpdateDB /* = true */ )
{
	if( m_OptionList.m_bPlayGuide == bGuide )
		return;

	m_OptionList.m_bPlayGuide = bGuide;

	if( bUpdateDB == true )
	{
		if( g_pMain != NULL )
		{
			CX2State* pNowState = (CX2State*) g_pMain->GetNowState();
			if( pNowState != NULL )
			{
				pNowState->Handler_EGS_CHANGE_OPTION_PLAY_GUIDE_REQ();
			}
		}
	}
}

void CX2GameOption::SendFieldOption()
{
	if( m_OptionList.m_FieldDetail == m_eFieldLevelOld &&
		m_OptionList.m_bParty == m_bPartyOld )
		return;

	m_eFieldLevelOld = m_OptionList.m_FieldDetail;
	m_bPartyOld = m_OptionList.m_bParty;

	if( g_pMain != NULL )
	{
		CX2State* pNowState = (CX2State*) g_pMain->GetNowState();
		if( pNowState != NULL )
		{
			pNowState->Handler_EGS_OPTION_UPDATE_REQ();
		}
	}
}



void CX2GameOption::SetRefuseFriend(CX2GameOption::BlackListDenyState val)
{
	switch(val)
	{
	case BL_ON:
		SetRefuseFriend(true);
		break;
	case BL_OFF:
	default:
		SetRefuseFriend(false);
		break;
	}
}

void CX2GameOption::SetRefuseParty(CX2GameOption::BlackListDenyState val)
{
	switch(val)
	{
	case BL_ON:
		SetRefuseParty(true);
		break;
	case BL_OFF:
	default:
		SetRefuseParty(false);
		break;
	}
}

void CX2GameOption::SetRefusePersonalTrade(CX2GameOption::BlackListDenyState val)
{
	switch(val)
	{
	case BL_ON:
		SetRefusePersonalTrade(true);
		break;
	case BL_OFF:
	default:
		SetRefusePersonalTrade(false);
		break;
	}
}

void CX2GameOption::SetRefuseParty(bool val)
{
	m_OptionList.m_bRefuseParty = val;
}

void CX2GameOption::SetRefuseFriend(bool val)
{
	m_OptionList.m_bRefuseFriend = val;
}

void CX2GameOption::SetRefusePersonalTrade(bool val)
{
	m_OptionList.m_bRefusePersonalTrade = val;
}

//{{ kimhc // 2009-10-09 // 길드초대 거부 추가
void CX2GameOption::SetRefuseGuildInvitation( CX2GameOption::BlackListDenyState val )
{
	switch ( val )
	{
	case BL_ON:
		SetRefuseGuildInvitation( true );
		break;

	case BL_OFF:
	default:
		SetRefuseGuildInvitation( false );
		break;
	}
}

void CX2GameOption::SetRefuseGuildInvitation( bool val )
{
	m_OptionList.m_bRefuseGuildInvitation = val;
}
//}} kimhc // 2009-10-09 // 길드초대 거부 추가

void CX2GameOption::SetRefuseRequestCouple(CX2GameOption::BlackListDenyState val)
{
	switch(val)
	{
	case BL_ON:
		SetRefuseRequestCouple(true);
		break;
	case BL_OFF:
	default:
		SetRefuseRequestCouple(false);
		break;
	}
}

void CX2GameOption::SetRefuseRequestCouple(bool val)
{
	m_OptionList.m_bRefuseRequestCouple = val;
}


void CX2GameOption::SetRefuseInvitePVP(CX2GameOption::BlackListDenyState val)
{
	switch(val)
	{
	case BL_ON:
		SetRefuseInvitePVP(true);
		break;
	case BL_OFF:
	default:
		SetRefuseInvitePVP(false);
		break;
	}
}

void CX2GameOption::SetRefuseInvitePVP(bool val)
{
	m_OptionList.m_bRefuseInvitePVP = val;
}








void CX2GameOption::SetFullScreen( bool bFullScreen )
{

	m_OptionList.m_bFullScreen = bFullScreen;

	//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현
	if( true == bFullScreen && true == DXUTIsWindowed() )
	{
		DXUTToggleFullScreen();
		::SetFocus( g_pKTDXApp->GetHWND() );
	}
	else if( false == bFullScreen && false == DXUTIsWindowed() )
	{
		DXUTToggleFullScreen();
		::SetFocus( g_pKTDXApp->GetHWND() );
	}
	//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현


}

//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현
//}} JHKang // 2010.8.27 // 창 모드 전체 화면 구현

void CX2GameOption::SetFlashEffect( IN bool bVal_ )
{
	m_OptionList.m_bFlashEffect = bVal_;
}

void CX2GameOption::SetFrameLimit( IN int iFps_ )
{
	m_OptionList.m_iFrameLimit = iFps_ > 0 ? iFps_ : 0;
	DXUTSetFrameLimit( static_cast<float>( m_OptionList.m_iFrameLimit ) );
}

// Anti-aliasing, supersampling and anisotropic filtering (2026-11-12, client/msaa): new options, applied by the D3D9 device from
// the next frame.
void CX2GameOption::SetMultisample( IN int iSamples_ )
{
	m_OptionList.m_iMultisample = iSamples_ >= 8 ? 8 : iSamples_ >= 4 ? 4 : iSamples_ >= 2 ? 2 : 1;
	if( IDirect3DDevice9* pDevice = DXUTGetD3DDevice() )
		pDevice->SetMultisample( static_cast<UINT>( m_OptionList.m_iMultisample ) );
}

void CX2GameOption::SetSupersample( IN int iFactor_ )
{
	m_OptionList.m_iSupersample = iFactor_ >= 2 ? 2 : 1;
	if( IDirect3DDevice9* pDevice = DXUTGetD3DDevice() )
		pDevice->SetSupersample( static_cast<UINT>( m_OptionList.m_iSupersample ) );
}

void CX2GameOption::SetAnisotropy( IN int iLevel_ )
{
	m_OptionList.m_iAnisotropy = iLevel_ >= 16 ? 16 : iLevel_ >= 8 ? 8 : iLevel_ >= 4 ? 4 : iLevel_ >= 2 ? 2 : 1;
	if( IDirect3DDevice9* pDevice = DXUTGetD3DDevice() )
		pDevice->SetAnisotropy( static_cast<UINT>( m_OptionList.m_iAnisotropy ) );
}

void CX2GameOption::SetVerticalSync( IN bool bVal_ )
{
	m_OptionList.m_bVerticalSync = bVal_;

	if( true == bVal_ && static_cast<UINT>( D3DPRESENT_INTERVAL_IMMEDIATE ) == DXUTGetVerticalSync() )
	{
		DXUTSetVerticalSync( 0 );
		::SetFocus( g_pKTDXApp->GetHWND() );
	}
	else if( false == bVal_ && D3DPRESENT_INTERVAL_DEFAULT == DXUTGetVerticalSync() )
	{
		DXUTSetVerticalSync( 1 );
		::SetFocus( g_pKTDXApp->GetHWND() );
	}
}

float CX2GameOption::GetCameraDistance()
{
#ifdef PVP_ZOOM_CAMERA
	if( m_OptionList.m_bAutoCamera == true && g_pX2Game != NULL )
	{
		float fBaseDist = m_fPvpZoom2;
		float fZoomOutDist = m_fPvpZoom3;
		if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			fBaseDist = m_fDungeonZoom2;
			fZoomOutDist = m_fDungeonZoom3;
		}

		float fDistAct = 1.f;
		if( GetSubZoomCameraTimer() > 0.f )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
				fDistAct = m_fDungeonZoom1;
			else
				fDistAct = m_fPvpZoom1;
		}

		switch( GetSubZoomCameraLv() )
		{
		case 0:
			return fBaseDist * fDistAct;
		case 1:
			return (fBaseDist + fZoomOutDist) * fDistAct;
		default:
			return fBaseDist * fDistAct;
		}
	}
#endif

	float fCameraDistance = 1200.f;
	switch( m_OptionList.m_iZoomLevel )
	{
	default:
	case 0:
		{
			fCameraDistance = 1100.f;
		} break;

	case 1:
		{
			fCameraDistance = 600.f;
		} break;
	case 2:
		{
			fCameraDistance = 200.f;
		} break;
	case -1:
		{
			fCameraDistance = 1500.f;
		} break;
	}

	float fBaseCameraDistance = 0.f;
	float fStepCameraDistance = 0.f;
	if( g_pX2Game != NULL )
	{
		if( g_pX2Game->GetWorld() != NULL )
		{
			fBaseCameraDistance = g_pX2Game->GetWorld()->GetBaseCameraDistance();

			//월드에서 지정하는 카메라는 '0, 1, 2' 3단계로 유지
			int iGetGamerStepDistance = (m_OptionList.m_iZoomLevel > 1 ) ? 1: m_OptionList.m_iZoomLevel;
			iGetGamerStepDistance  = max(-1, iGetGamerStepDistance );

			fStepCameraDistance = g_pX2Game->GetWorld()->GetCameraStepDistacne( iGetGamerStepDistance + 1 );
		}
	}

	return fBaseCameraDistance + fCameraDistance + fStepCameraDistance;
}

#ifdef PVP_ZOOM_CAMERA
void CX2GameOption::ResetAutoCamera()
{
#if X2_DEV_TOOLS
	FILE *fp = fopen("ZoomCamera.ini", "r");
	if( fp != NULL )
	{
		fscanf(fp, "%f%f%f%f%f%f", &m_fDungeonZoom1, &m_fDungeonZoom2, &m_fDungeonZoom3, &m_fPvpZoom1, &m_fPvpZoom2, &m_fPvpZoom3 );

		fclose(fp);
	}
	else
#endif
	{
		m_iSubZoomCameraLv = 0;
		m_fSubZoomCameraTimer = 0.f;

		m_fDungeonZoom1 = 0.8f;
		m_fDungeonZoom2 = 1300.f;
		m_fDungeonZoom3 = 400.f;
		m_fPvpZoom1 = 0.8f;
		m_fPvpZoom2 = 1500.f;
		m_fPvpZoom3 = 500.f;
	}

}
#endif
void CX2GameOption::SetIsSkillUITypeA( bool bIsSkillUITypeA_, bool bShowBuffIcon_/* = true */)
{
	m_OptionList.m_bIsSkillUITypeA = bIsSkillUITypeA_;

	//스킬 위치 변경
	if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillTree() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->SetSkillUIType(m_OptionList.m_bIsSkillUITypeA);
	}

	//펫 스킬 슬롯 위치 변경
	if( NULL != CX2RidingPetManager::GetInstance() && NULL != CX2RidingPetManager::GetInstance()->GetSkillSlot() )
	{
		CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetSkillUIType( m_OptionList.m_bIsSkillUITypeA );
	}

	//파티 게이지 위치 변경
	CX2GageManager* pGageManager = CX2GageManager::GetInstance();
	if( NULL != pGageManager )
	{
		pGageManager->ResetOtherPlayerUIPosition();
		pGageManager->SetShowBuffIcon( bShowBuffIcon_ );
	}

}

void CX2GameOption::_DecideProperGraphicOption()
{
	int iCpuScore = 0;
	int iSystemMemScore = 0;
	int iGpuMemScore = 0;

	//Cpu Score 계산
	iCpuScore = _CalcCpuScore();

	//System Memory Score 계산
	std::size_t totalPhys = 0, availablePhys = 0;
	x2::platform::PhysicalMemory( totalPhys, availablePhys );
	UINT uiTotalSystemMem = (UINT)( (float)totalPhys / 1024.0f / 1024.0f );//MB로...

	//100점으로 환산...
	iSystemMemScore = (int)( ( 100.0f / 4096.0f ) * (float)uiTotalSystemMem );

	if( iSystemMemScore > 100 )
		iSystemMemScore = 100;

	//GPU Memory Score 계산(공유메모리 포함한 용량임)
	UINT uiAvailableTextureMem = (UINT)( (float)g_pKTDXApp->GetDevice()->GetAvailableTextureMem() / 1024.0f / 1024.0f );//MB로...

	//100점으로 환산...
	iGpuMemScore = (int)( ( 100.0f / 4096.0f ) * (float)uiAvailableTextureMem );

	if( iGpuMemScore > 100 )
		iGpuMemScore = 100;

	//FILE* fp = fopen( "AutoSetupGameOptionLog.txt", "w+" );
	//fprintf( fp, "iCpuScore = %d, iSystemMemScore=%d, iGpuMemScore=%d\n", iCpuScore, iSystemMemScore, iGpuMemScore );
	//fclose( fp );

	//현재 pc에 맞는 가장 적합한 그래픽옵션 설정
	_CalcBestGraphicOption( ( iCpuScore + iSystemMemScore ) / 2, iGpuMemScore );
}

int CX2GameOption::_CalcCpuScore()
{
	return ( KBenchMark_SUM::exec() + KBenchMark_LINPACK::exec() + KBenchMark_MXM::exec() ) / 3;
}

void CX2GameOption::_CalcBestGraphicOption( int iCpuTotalScore, int iGpuTotalScore )
{
	// Read AutoSetupGameOptionWeightConfig.xml
	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
	Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( "AutoSetupGameOptionWeightConfig.xml" );
	if( Info == NULL )
		return;

	const std::optional<x2::platform::XmlElement> kRoot = x2::platform::ParseXml( std::string_view( Info->pRealData, Info->size ) );
	if( !kRoot || kRoot->name != "AutoSetupGameOptionWeightConfig" )
		return;

	for( const x2::platform::XmlElement& kElement : kRoot->children )
	{
		const std::string& name = kElement.name;
		if ( name == "GRAPHIC_UNIT" )
			m_OptionList.m_UnitDetail = _CalcHighMiddleLow( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "GRAPHIC_TEXTURE" )
			m_OptionList.m_TexDetail = _CalcHighMiddleLow( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "GRAPHIC_MAP" )
			m_OptionList.m_MapDetail = _CalcHighMiddleLow( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "GRAPHIC_EFFECT" )
			m_OptionList.m_eEffect = _CalcHighMiddleLow( kElement, iCpuTotalScore, iGpuTotalScore );
		// GRAPHIC_VIEWDISTANCE was not applied in the original either.
		else if ( name == "GRAPHIC_FIELD" )
		{
			switch( _CalcHighMiddleLow( kElement, iCpuTotalScore, iGpuTotalScore ) )
			{
			case OL_HIGH:	m_OptionList.m_FieldDetail = FL_HIGH;	break;
			case OL_MEDIUM:	m_OptionList.m_FieldDetail = FL_MEDIUM;	break;
			case OL_LOW:	m_OptionList.m_FieldDetail = FL_LOW;	break;
			default:	break;
			}
		}
		else if ( name == "ShowAllPlayer" )
			m_OptionList.m_bParty = !_CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "NormalViewPlayer" )
			m_OptionList.m_bSD = !_CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "NormalResolution" )
		{
			if( _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore ) == false )
			{
				m_OptionList.m_vResolution.x = 800;
				m_OptionList.m_vResolution.y = 600;
			}
		}
		else if ( name == "ShowLevel" )
			m_OptionList.m_bLevel = _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "ShowPvPRank" )
			m_OptionList.m_bPvpRank = _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "ShowGuild" )
			m_OptionList.m_bGuild = _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "ShowMyInfo" )
			m_OptionList.m_bNothing = !_CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "ThreeDimensionSound" )
			m_OptionList.m_bEnable3DSound = _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "FLASHEFFECT" )
			m_OptionList.m_bFlashEffect = !_CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
		else if ( name == "DYNAMIC_CAMERA" )
			m_OptionList.m_bDynamicCamera = _CalcOnOff( kElement, iCpuTotalScore, iGpuTotalScore );
	}
}

namespace
{
	// The GpuWeight attribute of an option element (1 when absent), and the score of its child `level` (100 when absent).
	float GpuWeight( const x2::platform::XmlElement& kElement )
	{
		const std::string* pValue = kElement.Attribute( "GpuWeight" );
		return pValue != nullptr ? static_cast<float>( std::atof( pValue->c_str() ) ) : 1.0f;
	}

	int LevelScore( const x2::platform::XmlElement& kElement, std::string_view level )
	{
		const x2::platform::XmlElement* pChild = kElement.Child( level );
		const std::string* pValue = pChild != nullptr ? pChild->Attribute( "score" ) : nullptr;
		return pValue != nullptr ? std::atoi( pValue->c_str() ) : 100;
	}

	// The user's score for an option: the CPU and GPU scores, weighted.
	int OptionScore( const x2::platform::XmlElement& kElement, int iCpuTotalScore, int iGpuTotalScore )
	{
		const float fGpuWeight = GpuWeight( kElement );
		return static_cast<int>( iCpuTotalScore * ( 1.0f - fGpuWeight ) + iGpuTotalScore * fGpuWeight );
	}
}

CX2GameOption::OptionLevel CX2GameOption::_CalcHighMiddleLow( const x2::platform::XmlElement& kElement, int iCpuTotalScore, int iGpuTotalScore )
{
	const int iMyScore = OptionScore( kElement, iCpuTotalScore, iGpuTotalScore );
	if( iMyScore >= LevelScore( kElement, "High" ) )
		return CX2GameOption::OL_HIGH;
	else if( iMyScore >= LevelScore( kElement, "Middle" ) )
		return CX2GameOption::OL_MEDIUM;
	return CX2GameOption::OL_LOW;
}

bool CX2GameOption::_CalcOnOff( const x2::platform::XmlElement& kElement, int iCpuTotalScore, int iGpuTotalScore )
{
	return OptionScore( kElement, iCpuTotalScore, iGpuTotalScore ) >= LevelScore( kElement, "On" );
}

void CX2GameOption::SetJoyEnable( bool bEnable )
{
	m_OptionList.m_bJoyEnable = bEnable;
	g_pKTDXApp->GetDIManager()->SetEnableJoyStick(bEnable);

}
