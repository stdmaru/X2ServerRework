#include "stdafx.h"
#include "./X2GameLoadingTip.h"

CX2GameLoadingTip::CX2GameLoadingTip()
{
	m_pDLGLoadingTip = NULL;


	m_pDLGLoadingTip = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Game_Loading_Tip.lua", 0.0f );

	Init();
// 2009.01.30 김태완 : 로딩시 한줄팁 나오게 하기
}

CX2GameLoadingTip::~CX2GameLoadingTip(void)
{
    SAFE_DELETE_DIALOG_HANDLE( m_pDLGLoadingTip );

	for ( UINT i = 0; i < m_vecTipGroup.size(); i++ )
	{
		CX2GameLoadingTip::TipGroup* pTipGroup = m_vecTipGroup[i];
		SAFE_DELETE( pTipGroup );
	}
	m_vecTipGroup.clear();

}


HRESULT CX2GameLoadingTip::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_pDLGLoadingTip != NULL )
		m_pDLGLoadingTip->OnFrameMove( fTime, fElapsedTime );

	return S_OK;
}
HRESULT CX2GameLoadingTip::OnFrameRender()
{
	if ( m_pDLGLoadingTip != NULL )
		m_pDLGLoadingTip->OnFrameRender();

	return S_OK;
}

bool CX2GameLoadingTip::Init()
{
//{{ robobeg : 2008-10-28
	//KLuaManager kLuamanager;
    KLuaManager kLuamanager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//}} robobeg : 2008-10-28

	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;

//{{ robobeg : 2008-10-28
	//Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile( "Enum.lua" );
	//if( Info == NULL )
	//{
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"enum.lua\n파일을 찾을 수 없습니다.", g_pMain->GetNowState() );
	//	return false;
	//}

	//if( kLuamanager.DoMemory( Info->pRealData, Info->size ) == false )
	//{
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"enum.lua - 구문 오류?", g_pMain->GetNowState() );
	//	return false;
	//}
//}} robobeg : 2008-10-28

    if ( g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"Game_Loading_Tip.lua" ) == false )
    {
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_222 ), g_pMain->GetNowState() );
		return false;
    }

	if ( kLuamanager.BeginTable( "LoadingTip" ) == true )
	{
		for ( int tipGrouptableIndex = 1; ; tipGrouptableIndex++ )
		{
			if ( kLuamanager.BeginTable( tipGrouptableIndex ) == true )
			{
				CX2GameLoadingTip::TipGroup* pTipGroup = new CX2GameLoadingTip::TipGroup();

				LUA_GET_VALUE( kLuamanager, "MIN_LEVEL", pTipGroup->m_MinLevel, 0 );
				LUA_GET_VALUE( kLuamanager, "MAX_LEVEL", pTipGroup->m_MaxLevel, 0 );

				if ( kLuamanager.BeginTable( "TEXTURES" ) == true )
				{
					int tableIndex = 1;
					wstring textureName;
					while( kLuamanager.GetValue( tableIndex, textureName ) == true )
					{
						pTipGroup->m_vecTextureName.push_back( textureName );
						tableIndex++;
					}
					kLuamanager.EndTable();
				}

				m_vecTipGroup.push_back( pTipGroup );

				kLuamanager.EndTable();
			}
			else
			{
				break;
			}
		}

		kLuamanager.EndTable();
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_223 ), g_pMain->GetNowState() );
		return false;
	}

//{{ 2009.01.30 김태완 : 던전로딩시 한줄팁 나오게 하기
	if ( kLuamanager.BeginTable( "OneLineTip" ) == true )
	{
		for ( int tipGrouptableIndex = 1; ; tipGrouptableIndex++ )
		{
			if ( kLuamanager.BeginTable( tipGrouptableIndex ) == true )
			{
				CX2GameLoadingTip::TipGroupStrID pTipGroup;

				LUA_GET_VALUE( kLuamanager, "MIN_LEVEL", pTipGroup.m_MinLevel, 0 );
				LUA_GET_VALUE( kLuamanager, "MAX_LEVEL", pTipGroup.m_MaxLevel, 0 );

				if ( kLuamanager.BeginTable( "STR_ID" ) == true )
				{
					int tableIndex = 1;
					int OneLineTipStrID = -1;
					while( kLuamanager.GetValue( tableIndex, OneLineTipStrID ) == true )
					{
						pTipGroup.m_vecOneLineTip.push_back( OneLineTipStrID );
						tableIndex++;
					}
					kLuamanager.EndTable();
				}

				m_vecOneLineTipGroup.push_back( pTipGroup );
				kLuamanager.EndTable();
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_224 ), g_pMain->GetNowState() );
		return false;
	}

//}}

	return true;
}

void CX2GameLoadingTip::CreateLoadingTipUI()
{
	if ( g_pData->GetMyUser() != NULL )
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
		if ( pUnit != NULL )
		{
			CX2GameLoadingTip::TipGroup* pTipGroup = GetTipGroup( pUnit->GetUnitData().m_Level );
			if ( pTipGroup != NULL )
			{
				wstring textureName = GetRandomTextureName( pTipGroup );

				CKTDGUIStatic* pStaticLoadingTip = new CKTDGUIStatic();
				CKTDGUIControl::CPictureData* pPictureData = new CKTDGUIControl::CPictureData();
				pPictureData->SetTex( textureName.c_str() );
				pPictureData->SetPoint();

				pStaticLoadingTip->AddPicture( pPictureData );
				m_pDLGLoadingTip->AddControl( pStaticLoadingTip );
			}
		}
	}
}

CX2GameLoadingTip::TipGroup* CX2GameLoadingTip::GetTipGroup( int level )
{
	CX2GameLoadingTip::TipGroup* pTipGroup = NULL;

	for ( UINT i = 0; i < m_vecTipGroup.size(); i++ )
	{
		pTipGroup = m_vecTipGroup[i];
		if ( pTipGroup == NULL )
			continue;

		if ( pTipGroup->m_MinLevel <= level && pTipGroup->m_MaxLevel >= level )
			return pTipGroup;
	}

	return pTipGroup;
}

wstring CX2GameLoadingTip::GetRandomTextureName( CX2GameLoadingTip::TipGroup* pTipGroup )
{
	if ( pTipGroup == NULL || (int)pTipGroup->m_vecTextureName.size() == 0 )
		return L"";

	int textureIndex = RandomNumber( 0, (int)pTipGroup->m_vecTextureName.size() - 1 );


	return pTipGroup->m_vecTextureName[ textureIndex ];
}

//{{ 2009.01.30 김태완 : 던전로딩시 한줄팁 나오게 하기
void CX2GameLoadingTip::CreateLoadingOneLineTip()
{
		int iLevel = -1; //캐릭터 레벨
		if( NULL != g_pData )
		{
			iLevel = g_pData->GetSelectUnitLevel();
		}

		//현재 레벨에 맞는 팁그룹 구하기
		vector<TipGroupStrID>::iterator it = m_vecOneLineTipGroup.begin();
		for( ; it != m_vecOneLineTipGroup.end(); ++it )
		{
			if( iLevel <= it->m_MaxLevel &&
				iLevel >= it->m_MinLevel)
			{
				//현재 그룹의 팁중 랜덤으로 1개 설정
				if( false == it->m_vecOneLineTip.empty() )
				{
					int RandomTipIndex = RandomNumber( 0, static_cast<int>(it->m_vecOneLineTip.size()) - 1 );
					int iStrID = it->m_vecOneLineTip[RandomTipIndex];
					SetOneLineTipString(iStrID);
				}
				break;
			}
		}

}

void CX2GameLoadingTip::SetOneLineTipString( int iStrID_)
{
	wstring OneLineTip = GET_STRING(iStrID_);
	if( true == OneLineTip.empty() )
		return;

	OneLineTip = GET_STRING( STR_ID_30362 ) + OneLineTip;

	CKTDGUIStatic* pStaticLoadingTip = (CKTDGUIStatic*) m_pDLGLoadingTip->GetControl( L"Static_OneLineTip" );

	if( NULL == pStaticLoadingTip )
		return;

	// 글자 자르기
	CKTDGFontManager::CUKFont* pFont
		= g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStaticLoadingTip->GetString(0)->fontIndex );
	if( NULL == pFont )
		return;

	float WindowLength = (float)(pStaticLoadingTip->GetDummyInt(0));
	WindowLength = g_pKTDXApp->ConvertByResolution( D3DXVECTOR2(WindowLength, 0) ).x;

	const int MAGIC_MAX_WIDTH = (int) WindowLength;
	CWordLineHandler::LineBreakInX2Main( OneLineTip, pFont, MAGIC_MAX_WIDTH, L"", true  );
	wstring wstrLineBuf = OneLineTip;
	pStaticLoadingTip->GetString(0)->msg = wstrLineBuf.c_str();

	pStaticLoadingTip->SetShow(true);
}
//}}



void CX2GameLoadingTip::SetVillageSetting()
{
	if( NULL != m_pDLGLoadingTip )
	{
		m_pDLGLoadingTip->SetPos( D3DXVECTOR2(512, 740) );
		CKTDGUIStatic* pStaticLoadingTip = (CKTDGUIStatic*) m_pDLGLoadingTip->GetControl( L"Static_OneLineTip" );
		if( NULL != pStaticLoadingTip && NULL != pStaticLoadingTip->GetString(0) )
		{
			pStaticLoadingTip->GetString(0)->sortFlag = 1;
		}
	}
}
