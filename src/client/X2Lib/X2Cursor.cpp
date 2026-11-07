#include "stdafx.h"
#include "../X2/resource.h"
#include "./X2Cursor.h"


//{{ robobeg : 2008-10-13
static const CKTDGStateManager::KState s_akStates[] =
{
		KRenderState( D3DRS_ALPHATESTENABLE,	TRUE ),
		KRenderState( D3DRS_ALPHAFUNC,			D3DCMP_NOTEQUAL ),
		KRenderState( D3DRS_ALPHAREF,			0 ),

		KRenderState( D3DRS_ALPHABLENDENABLE,	TRUE ),
		KRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA ),
		KRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA ),

		KRenderState( D3DRS_ZENABLE,			FALSE ),
		KRenderState( D3DRS_ZWRITEENABLE,		FALSE ),
		KRenderState( D3DRS_CULLMODE,			D3DCULL_CW ),

		KTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE ),
		KTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE ),
		KTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE ),
};//s_akStates[]
//}} robobeg : 2008-10-13


CX2Cursor::CX2Cursor(void)
{
	m_hCursor = LoadCursor( DXUTGetHINSTANCE(), MAKEINTRESOURCE(IDC_CURSOR1) );

	m_pCursorDraggingTex = NULL;
	m_pCursorDraggingTex = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Drag_Cursor.tga" );

	m_pCursorSellTex = NULL;
	m_pCursorSellTex = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Sell_Cursor.tga" );

	m_pCursorRepairTex = NULL;
	m_pCursorRepairTex = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Repair_Cursor.tga" );

	m_pCursorDecompositionTex = NULL;
	m_pCursorDecompositionTex = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Broken_Cursor.tga" );

	m_pCursorEnchant = NULL;
	m_pCursorEnchant = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Upgrade_Cursor.tga" );

	m_pCursorSocket = NULL;
	m_pCursorSocket = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Addsoket_Cursor.tga" );

	m_pCursorRemoveSkill = NULL;
	m_pCursorRemoveSkill = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Reset_Skill_Cursor.tga" );


	m_pCursorAtrrib = NULL;
	m_pCursorAtrrib = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"Cursor_Attrib.tga" );


	m_pCursorItemExchange = NULL;
	m_pCursorItemExchange = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"Item_Change_Cursor.tga" );

	m_pCursorAttachItem = NULL;
	m_pCursorAttachItem = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Upgrade_Cursor.tga" );
	m_pCursorDeleteItem = NULL;
	m_pCursorDeleteItem = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Waste_Item_Cursor.tga" );

	//{{ kimhc // 2009-08-19 // 아이템 봉인
	m_pCursorSealItem	= NULL;
	m_pCursorSealItem	= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Seal_Item_Cursor.tga" );	// 임시
	//}} kimhc // 2009-08-19 // 아이템 봉인

	m_pCursorRegisterMemo	= NULL;
	m_pCursorRegisterMemo	= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Write_Cursor.tga" );

	m_pCursorEventScene	= NULL;
	m_pCursorEventScene	= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Eventscene_Cursor.dds" );

	m_pCursorPetFood	= NULL;
	m_pCursorPetFood	= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Pat_Cursor.tga" );


	m_pCursorRestoreItemEvalutation = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"Evaulation_Cursor.tga" );
	m_pCursorExchangeNewItem = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Change_Cursor.tga" );
	m_pCursorAddSocketSlot = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_AddSocketSlot_Cursor.tga" );

//{{ robobeg : 2008-10-13
	m_RenderStateID = s_akStates;
//}} robobeg : 2008-10-13


	m_Pos = D3DXVECTOR2(0, 0);
	m_Size = D3DXVECTOR2(50, 50);

	POINT point;
	GetCursorPos( &point );
	ScreenToClient( g_pKTDXApp->GetHWND(), &point );


	m_Pos.x = (float)point.x;
	m_Pos.y = (float)point.y;

	SetShow( true );

	m_CursorState = CX2Cursor::XCS_NORMAL;

//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//	m_pVB = NULL;
//	HRESULT hr = g_pKTDXApp->GetDevice()->CreateVertexBuffer( 4 * sizeof(DRAWFACE_RHW_VERTEX),
//		D3DUSAGE_WRITEONLY, D3DFVF_DRAWFACE_RHW_VERTEX,
//		D3DPOOL_MANAGED, &m_pVB, NULL );
//	ASSERT( SUCCEEDED( hr ) );
//
//	m_PosVB = D3DXVECTOR2(0, 0);
//	m_SizeVB = D3DXVECTOR2(0, 0);
//	m_d3dColorVB = 0x00000000;
//#endif

}

CX2Cursor::~CX2Cursor(void)
{
	SAFE_CLOSE( m_pCursorDraggingTex );
	SAFE_CLOSE( m_pCursorSellTex );
	SAFE_CLOSE( m_pCursorRepairTex );
	SAFE_CLOSE( m_pCursorDecompositionTex );
	SAFE_CLOSE( m_pCursorEnchant );
	SAFE_CLOSE( m_pCursorSocket );
	SAFE_CLOSE( m_pCursorRemoveSkill );
	SAFE_CLOSE( m_pCursorAtrrib );
	SAFE_CLOSE( m_pCursorItemExchange );
	SAFE_CLOSE( m_pCursorAttachItem );
	SAFE_CLOSE( m_pCursorDeleteItem );

	//{{ kimhc // 2009-08-19 // 아이템 봉인
	SAFE_CLOSE( m_pCursorSealItem );
	//}} kimhc // 2009-08-19 // 아이템 봉인

	SAFE_CLOSE( m_pCursorRegisterMemo );
	SAFE_CLOSE( m_pCursorEventScene );
	SAFE_CLOSE( m_pCursorPetFood );
	SAFE_CLOSE( m_pCursorRestoreItemEvalutation );
	SAFE_CLOSE( m_pCursorExchangeNewItem );
//#ifndef DYNAMIC_VERTEX_BUFFER_OPT
//	SAFE_RELEASE( m_pVB );
//#endif
	SAFE_CLOSE( m_pCursorAddSocketSlot );
}



HRESULT CX2Cursor::OnFrameMove( double fTime, float fElapsedTime )
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( g_pKTDXApp->GetHWND(), &point );

	if ( m_Pos.x == (float)point.x && m_Pos.y == (float)point.y )
	{
		m_fTimeForHide -= fElapsedTime;
	}
	else
	{
		m_Pos.x = (float)point.x;
		m_Pos.y = (float)point.y;

		m_fTimeForHide = 5.0f;
	}

	if ( m_fTimeForHide <= 0.0f )
	{
		SetShow( false );
		SetCursor( NULL );
		m_fTimeForHide = 0.0f;
	}
	else
	{
		m_bShow = true;
	}

	return S_OK;
}



HRESULT CX2Cursor::OnFrameRender()
{
	if ( m_bShow == true )
    {
//{{ robobeg : 2008-10-24
        //g_pKTDXApp->SetNULLTexture( 0 );
        CKTDGStateManager::SetTexture( 0, NULL );
//}} robobeg : 2008-10-24

		if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->IsDraggingDialog() )
		{
			if( NULL != m_pCursorDraggingTex )
			{
				SetCursor( NULL );
				m_pCursorDraggingTex->SetDeviceTexture();
				DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y);
			}
		}
		else
		{
			//{{ 2009.01.19 김태완 : 코드정리
			switch(m_CursorState)
			{
			case CX2Cursor::XCS_NORMAL:
				{
					if ( GetCursor() == NULL )
					{
						SetCursor( m_hCursor );
					}
				} break;
			case CX2Cursor::XCS_REPAIR:
				{
					if( NULL != m_pCursorRepairTex)
						m_pCursorRepairTex->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y);

				} break;
			case CX2Cursor::XCS_SELL:
				{
					if( NULL != m_pCursorSellTex)
						m_pCursorSellTex->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y);
				} break;
			case CX2Cursor::XCS_DECOMPOSITION:
				{
					if( NULL != m_pCursorDecompositionTex)
						m_pCursorDecompositionTex->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ENCHANT:
				{
					if( NULL != m_pCursorEnchant)
						m_pCursorEnchant->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_SOCKET:
				{
					if( NULL != m_pCursorSocket)
						m_pCursorSocket->SetDeviceTexture();
					DrawFace( m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_REMOVE_SKILL:
				{
					if( NULL != m_pCursorRemoveSkill)
						m_pCursorRemoveSkill->SetDeviceTexture();
					DrawFace( m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ATTRIB_ENCHANT:
				{
					if( NULL != m_pCursorAtrrib)
						m_pCursorAtrrib->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ITEM_EXCHANGE:
				{
					if( NULL != m_pCursorItemExchange)
						m_pCursorItemExchange->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ATTACH_ITEM:
				{
					if( NULL != m_pCursorAttachItem)
						m_pCursorAttachItem->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ATTACH_ATTRIB_ITEM:
				{
					if( NULL != m_pCursorAtrrib)
						m_pCursorAtrrib->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_DELETE_ITEM:
				{
					if( NULL != m_pCursorDeleteItem)
						m_pCursorDeleteItem->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;

				//{{ kimhc // 2009-08-19 // 아이템 봉인
			case CX2Cursor::XCS_SEAL_ITEM:
				{
					if( NULL != m_pCursorSealItem)
						m_pCursorSealItem->SetDeviceTexture();
					DrawFace( m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
				//}} kimhc // 2009-08-19 // 아이템 봉인

			case CX2Cursor::XCS_REGISTER_MEMO:
				{
					if( NULL != m_pCursorRegisterMemo)
						m_pCursorRegisterMemo->SetDeviceTexture();
					DrawFace( m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				}
				break;
			case CX2Cursor::XCS_EVENT_SCENE:
				{
					if( NULL != m_pCursorEventScene)
						m_pCursorEventScene->SetDeviceTexture();
					DrawFace( m_Pos.x - 40, m_Pos.y - 40, 128.f, 128.f );
				}
				break;
			case CX2Cursor::XCS_PET_FOOD:
				{
					if( NULL != m_pCursorPetFood)
						m_pCursorPetFood->SetDeviceTexture();
					DrawFace( m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ITEM_EVALUTE:
				{
					if( NULL != m_pCursorRestoreItemEvalutation )
						m_pCursorRestoreItemEvalutation->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_EXCHANGE_NEW_ITEM:
				{
					if( NULL != m_pCursorExchangeNewItem )
						m_pCursorExchangeNewItem->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;
			case CX2Cursor::XCS_ADD_SOCKET_SLOT:
				{
					if( NULL != m_pCursorAddSocketSlot )
						m_pCursorAddSocketSlot->SetDeviceTexture();
					DrawFace(m_Pos.x, m_Pos.y, m_Size.x, m_Size.y );
				} break;

			default:
				break;
			}//}} elseif -> switch
		}

//{{ robobeg : 2008-10-24
		//g_pKTDXApp->SetNULLTexture( 0 );
        CKTDGStateManager::SetTexture( 0, NULL );
//}} robobeg : 2008-10-24
	}
	return S_OK;
}


void CX2Cursor::SetShow( bool bShow )
{
	m_bShow = bShow;

	if ( m_bShow == true )
		m_fTimeForHide = 5.0f;
}

bool CX2Cursor::GetShowHardwareCursor()
{
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->IsDraggingDialog() )
	{
		if( NULL != m_pCursorDraggingTex )
		{
			return false;
		}
	}
	else
	{
		if ( m_CursorState == CX2Cursor::XCS_NORMAL )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if ( m_bShow == false )
		return false;

	return true;
}


void CX2Cursor::ChangeCursorState( X2_CURSOR_STATE cursorState )
{
	if( g_pMain != NULL && m_CursorState == CX2Cursor::XCS_PET_FOOD && cursorState != CX2Cursor::XCS_PET_FOOD )
	{
		g_pMain->DeleteInfoBox();
	}

	if ( m_CursorState == CX2Cursor::XCS_NORMAL )
		m_CursorState = cursorState;
	else if ( m_CursorState != cursorState )
		m_CursorState = cursorState;
	else
		m_CursorState = CX2Cursor::XCS_NORMAL;

	//으흠..
	if ( m_CursorState == CX2Cursor::XCS_NORMAL )
	{
		SetCursor( m_hCursor );
	}
	else
	{
		SetCursor( NULL );
	}
}

void CX2Cursor::DrawFace( float nX, float nY, float nWidth, float nHeight, D3DCOLOR color /* = 0xffffffff  */)
{
    KD3DPUSH( m_RenderStateID )

	DRAWFACE_RHW_VERTEX vertex[4];
	ZeroMemory( vertex, sizeof(DRAWFACE_RHW_VERTEX) * 4 );

	float X1 = (float)nX - 0.5f;
	float Y1 = (float)nY - 0.5f;
	float X2 = (float)(nX + nWidth) - 0.5f;
	float Y2 = (float)(nY + nHeight) - 0.5f;

	float temp =  1.0f / nHeight;

	float UMax = 1.0f;
	float VMax = 1.0f - temp;

	//Set up the 4 corners of a small square
	vertex[0].x = X1;     vertex[0].y = Y1;
	vertex[0].z = 1.0f;   vertex[0].rhw = 1.0f;
	vertex[0].u = 0.0f;   vertex[0].v = temp;

	vertex[1].x = X1;     vertex[1].y = Y2;
	vertex[1].u = 0.0f;   vertex[1].v = VMax;
	vertex[1].z = 1.0f;   vertex[1].rhw = 1.0f;

	vertex[2].x = X2;     vertex[2].y = Y1;
	vertex[2].u = UMax;   vertex[2].v = temp;
	vertex[2].z = 1.0f;   vertex[2].rhw = 1.0f;

	vertex[3].x = X2;     vertex[3].y = Y2;
	vertex[3].u = UMax;   vertex[3].v = VMax;
	vertex[3].z = 1.0f;   vertex[3].rhw = 1.0f;

	vertex[0].color = vertex[1].color =
		vertex[2].color = vertex[3].color = color;

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	static_assert( D3DFVF_DRAWFACE_RHW_VERTEX == D3DFVF_XYZRHW_DIFFUSE_TEX1 , "");
	g_pKTDXApp->GetDVBManager()->DrawPrimitive( CKTDGDynamicVBManager::DVB_TYPE_XYZRHW_DIFFUSE_TEX1
		, D3DPT_TRIANGLESTRIP, 2, vertex );
//#else
//	g_pKTDXApp->GetDevice()->SetFVF( D3DFVF_DRAWFACE_RHW_VERTEX );
//	g_pKTDXApp->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(DRAWFACE_RHW_VERTEX) );
//#endif

    KD3DEND()
}
