#include "stdafx.h"
#include "./X2GameEdit.h"



CX2GameEdit::CX2GameEdit(void)
#if X2_DEV_TOOLS
:m_eShowEffetLogLevel(ELL_NONE)
#endif // EFFECT_USE_LOG

{
	m_bShow		= false;
	m_bEnable	= false;

	m_pDLGGameEdit	= new CKTDGUIDialog( g_pMain, L"DLG_GameEdit.lua", 0.07f, XDL_GAME_EDIT );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGGameEdit );

	CX2ChatWindow::ChatWindowData chatWindowData;
	chatWindowData.pos.x = 18;
	chatWindowData.pos.y = 660;
	chatWindowData.size.x = 400;
	chatWindowData.size.y = 0;
	chatWindowData.fontSize.x = 0;
	chatWindowData.fontSize.y = 15;
	chatWindowData.maxChatDataNum = 100;
	chatWindowData.fChatTime = 99999;
	chatWindowData.fVanishTime = 1;

	m_pChatWindow = new CX2ChatWindow( chatWindowData );

	m_pState = NULL;

	m_iLastCreatedMonster = -1;

	m_wstrDevHelp			= L"";
	m_wstrOperatorHelp		= L"";

	SetShow( false );
	SetEnable( false );

	if ( OpenScriptFile( L"GameEditCommandList.lua" ) == false )
	{
		ErrorLog( XEM_ERROR78 );
	}
}

CX2GameEdit::~CX2GameEdit(void)
{
	SAFE_DELETE( m_pChatWindow );
	SAFE_DELETE_DIALOG( m_pDLGGameEdit );
}

HRESULT CX2GameEdit::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_bEnable == false )
	{
		HotKeyProcess();
		return S_OK;
	}

	if ( m_pChatWindow != NULL )
		m_pChatWindow->OnFrameMove( fTime, fElapsedTime );

	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_PGDN ) == TRUE )
	{
		Reset();
	}

	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_UPARROW ) == TRUE )
	{
		int tempNowHistoryCommandIndex = m_NowCommandHistoryIndex;
		tempNowHistoryCommandIndex -= 1;

		SetCommandByUpDownArrow( tempNowHistoryCommandIndex );

	}

	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_DOWNARROW ) == TRUE )
	{
		int tempNowHistoryCommandIndex = m_NowCommandHistoryIndex;
		tempNowHistoryCommandIndex += 1;

		SetCommandByUpDownArrow( tempNowHistoryCommandIndex );
	}

	return S_OK;
}

HRESULT CX2GameEdit::OnFrameRender()
{
	if ( m_bShow == false )
		return S_OK;

	if ( m_pChatWindow != NULL )
		m_pChatWindow->OnFrameRender();

	return S_OK;
}

void CX2GameEdit::SetShow( bool bShow )
{
	m_bShow = bShow;

	if ( m_pDLGGameEdit != NULL )
	{
		m_pDLGGameEdit->SetShow( m_bShow );
		CKTDGUIControl* pControl = m_pDLGGameEdit->GetControl( L"IMEEditBoxGameEditCommand" );
		if ( pControl != NULL )
		{
			if ( m_bShow == true )
			{
				pControl->RequestFocus();
			}
			else
			{
				m_pDLGGameEdit->ClearFocus();
			}
		}

	}
}

void CX2GameEdit::SetEnable( bool bEnable )
{
	m_bEnable = bEnable;
	m_pDLGGameEdit->SetModal( !m_bEnable );

	if ( m_pDLGGameEdit != NULL )
		m_pDLGGameEdit->SetEnable( m_bEnable );
}

void CX2GameEdit::SetNowStage( CX2State* pStage )
{
	if ( pStage == NULL )
		return;

	m_pState = pStage;

	m_pDLGGameEdit->SetStage( m_pState );
}

bool CX2GameEdit::ExecCommand( const WCHAR* pCommandString )
{


	if( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_SPECIAL_USER )
	{
		return false;
	}

	wstring commandWString = pCommandString;
	string commandString;
	ConvertWCHARToChar( commandString, pCommandString );


	if ( commandWString.empty() == true )
	{
		return false;
	}

	AddCommandHistory( commandWString.c_str() );

    KLuaManagerProxy    kLuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	WCHAR tempChar = commandWString[0];
	if ( tempChar == '/' )
	{
		string slashCommandString = "Slash_";
		int prevStringIndex = 0;
		int nowStringIndex = 0;

		nowStringIndex = commandString.find( " " );
		if ( nowStringIndex == -1 )
		{
			nowStringIndex = commandString.size();
		}

		string tempString;
		tempString.assign( commandString.begin() + 1, commandString.begin() + nowStringIndex );

		if ( m_mapCommandChange.find( tempString ) != m_mapCommandChange.end() )
		{
			string changeCommand = m_mapCommandChange[tempString];
			tempString = changeCommand;
		}

		slashCommandString += tempString;

        std::vector<KLuaManagerProxyLuaValue> vecArg;

		if ( tempString == "n" ||
			//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
			tempString == "na" ||
			//}}
			//{{ oasis907 : 김상윤 [2010.5.24] //
			tempString == "msc" ||
			//}}
			//tempString == "cm" ||
			tempString == "tf"
			|| tempString == "pa"
			|| tempString == "obs"		/// 영웅 대전 공지

			|| tempString == "afd"		/// 영웅 대전 공지

			|| tempString == "couple"		/// 영웅 대전 공지
			)


		{
			nowStringIndex += 1;
			string noticeMsg;
			noticeMsg.assign( commandString.begin() + nowStringIndex, commandString.end() );
            vecArg.push_back( KLuaManagerProxyLuaValue( noticeMsg ) );
		}
		else if( tempString == "obsw" )		/// 영웅 대전 귓속말 공지 ( 특정 인자에 ""를 씌우기 위해 분류 )
		{
			if( false == SetValueByHeroMatchWisperNotice(
                kLuaManager, vecArg,
                commandString, nowStringIndex ) )
				return false;
		}
		else if( tempString == "obsdel" )	/// 영웅 대전 그룹 제거 기능 ( 특정 인자에 ""를 씌우기 위해 분류 )
		{
			if( false == SetValueByHeroMatchDeleteGroupUser(
                kLuaManager, vecArg,
                commandString, nowStringIndex ) )
				return false;
		}
		else
		{
			prevStringIndex = nowStringIndex + 1;


			while(1)
			{
				if ( prevStringIndex >= (int)commandString.size() )
					break;


				WCHAR tempChar = commandWString[prevStringIndex];
				if ( tempChar == '"' ) //변수가 스트링일 경우
				{
					nowStringIndex = commandString.find( "\"", prevStringIndex + 1 );

					if ( nowStringIndex == -1 )
					{
						AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
						return false;
					}

					string tempString;
					tempString.assign( commandString.begin() + prevStringIndex + 1, commandString.begin() + nowStringIndex );
                    vecArg.push_back( KLuaManagerProxyLuaValue( tempString ) );

					prevStringIndex = nowStringIndex + 2;
				}
				else
				{
					nowStringIndex = commandString.find( " ", prevStringIndex );

					if ( nowStringIndex == -1 )
					{
						nowStringIndex = (int)commandString.size();
					}
					string tempString;
					tempString.assign( commandString.begin() + prevStringIndex, commandString.begin() + nowStringIndex );
                    KLuaManagerProxyLuaValue    luaValue;
                    switch( kLuaManager.LuaParse( tempString, true, luaValue ) )
                    {
                    case KLuaManagerProxy::LUA_PARSE_VALUE:
                        vecArg.push_back( luaValue );
                        break;
                    default:
						AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
						return false;
                    }//switch

					prevStringIndex = nowStringIndex + 1;
				}
			}
		}


		RegisterLuaBind();

        if ( kLuaManager.LuaCall( slashCommandString.c_str(), vecArg ) == false )
        {
			AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
			return false;
        }

#if !X2_DEV_TOOLS
// 		if( NULL != g_pData &&
// 			NULL != g_pData->GetMyUser() )
// 		{
// 			char Buf1[256];
// 			StringCchPrintfA( Buf1, 256, "사용 유저 UID : %lld, 권한 : %d \n 사용 치트 : ", g_pData->GetMyUser()->GetUID(), g_pData->GetMyUser()->GetAuthLevel());
// 			CX2MailLogManager::GetInstance()->AddMailLog( CX2MailLogManager::MLI_USE_GAME_EDIT, Buf1 );
// 		}
//
// 		CX2MailLogManager::GetInstance()->AddMailLog( CX2MailLogManager::MLI_USE_GAME_EDIT, slashCommandString.c_str() );
#endif // defined( NEW_MAIL_LOG ) || defined(_SERVICE_) //
	}
	else
	{

		AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
		return false;

	}

	AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_218 ) );



	return true;
}



#if X2_DEV_TOOLS
void CX2GameEdit::ExecuteLastCommand()
{
	if( m_CommandHistory.empty() )
		return;

	std::wstring& lastCommand = m_CommandHistory.back();
	ExecCommand( lastCommand.c_str() );
}
#endif // CHEAT_COMMAND_SHORT_CUT_KEY_TEST



void CX2GameEdit::AddString( CX2ChatWindow::CHAT_TYPE chatType, const WCHAR* pString )
{
	CX2ChatWindow::ChatData* pChatData	= new CX2ChatWindow::ChatData();
	pChatData->chatType					= chatType;
	pChatData->message					= pString;
	m_pChatWindow->PushChatData( pChatData );
}

void CX2GameEdit::AddString_LUA( int chatType, const char* pString )
{
	wstring stringMsg;
	ConvertUtf8ToWCHAR( stringMsg, pString );
	AddString( (CX2ChatWindow::CHAT_TYPE)chatType, stringMsg.c_str() );
}

void CX2GameEdit::MonsterStateChange(const char* pString )
{

	//wstring wstrMonsterStateName;
	//ConvertUtf8ToWCHAR( wstrMonsterStateName, pString );

    string  strMonsterStateName = ( pString ) ? pString : "";
	MakeUpperCase(strMonsterStateName);

	int iNpcID = GetLastCreatedMonster();

	if(iNpcID == -1)
	{
		AddString(CX2ChatWindow::CT_KILL, L"마지막으로 생성된 몬스터가 존재하지 않습니다.");
		return;
	}

	CX2GUNPC* pNPCUnit = g_pX2Game->GetNPCUnitByType(iNpcID);

	if(pNPCUnit == NULL)
	{
		AddString(CX2ChatWindow::CT_KILL, L"존재하지 않는 몬스터 입니다.");
		return;
	}

	int stateID = pNPCUnit->GetStateIDByKeyword( strMonsterStateName );

	if (stateID != CX2GUNPC::GUSI_NONE)
	{
        std::wstring    wstrTemp;
		AddString(CX2ChatWindow::CT_KILL, ConvertUtf8ToWCHAR( wstrTemp, pNPCUnit->GetStateNameByStateID( stateID ) ).c_str() );
		pNPCUnit->StateChangeCheat( stateID );
	}
	else
	{
		AddString(CX2ChatWindow::CT_KILL, L"존재하지 않는 몬스터 스테이트입니다.");
	}
	return;
}

//{{ 2010. 8. 10	최육사	펫 시스템
void CX2GameEdit::PetCheat_LUA( int iCheatType, int iValue )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}


	KEGS_ADMIN_CHANGE_PET_INFO_NOT kPacket;
	kPacket.m_iCheatType = iCheatType;
	kPacket.m_iValue = iValue;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANGE_PET_INFO_NOT, kPacket );
}

void CX2GameEdit::PetActionCheat_LUA( const char *motionName )
{

	if ( motionName == NULL || motionName[0] == NULL )
		return;

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		CX2PET *pPet = g_pData->GetPetManager()->GetMyPet();
		if( pPet != NULL && pPet->GetXSkinAnim() != NULL )
		{
			//wstring wstrMotionName;
			//ConvertUtf8ToWCHAR( wstrMotionName, motionName );

			if( strcmp( motionName, "true" ) == 0 )
			{
				pPet->SetCheat(true);
			}
			else if( strcmp( motionName, "false" ) == 0 )
			{
				pPet->SetCheat(false);
			}
			else
			{
				int stateId = pPet->GetStateID( motionName );
				pPet->StateChange( stateId, true, false );
			}
		}
	}
}

//}}

//{{ 2010. 9. 29	최육사	ED 모니터링 치트
void CX2GameEdit::UserEdCheat_LUA( const char* nickName )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}


	wstring wstrNickName;
	ConvertCharToWCHAR( wstrNickName, nickName );

	KEGS_ADMIN_SHOW_ED_CHEAT_REQ kPacket;
	kPacket.m_wstrNickName = wstrNickName;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_SHOW_ED_CHEAT_REQ, kPacket );
}
//}}

//{{ 2011. 04. 16	최육사	대리 상인
//#ifdef SERV_PSHOP_AGENCY
void CX2GameEdit::ShopCheat_LUA( int iPeriod )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	KEGS_ADMIN_PSHOP_AGENCY_CHEAT_REQ kPacket;
	kPacket.m_sPeriod = static_cast<short>(iPeriod);
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_PSHOP_AGENCY_CHEAT_REQ, kPacket );
}
//#endif SERV_PSHOP_AGENCY
//}}

//{{ 2011. 08. 11	최육사	투니 랜드 채널링
void CX2GameEdit::ChannelingCodeCheat_LUA( int iChannelingCode )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	KEGS_ADMIN_CHANNELING_CODE_CHEAT_NOT kPacket;
	kPacket.m_iChannelingCode = iChannelingCode;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANNELING_CODE_CHEAT_NOT, kPacket );
}
//}}

//{{ 2011. 08. 12   김민성      헤니르 개편
void CX2GameEdit::HenirRewardCountCheat_LUA( int iHenirReward )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	KEGS_ADMIN_HENIR_REWARD_CHEAT_NOT kPacket;
	kPacket.m_iHenirRewardNormalCnt = iHenirReward;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_HENIR_REWARD_CHEAT_NOT, kPacket );
}
//}}
void CX2GameEdit::HenirBuffCheat_LUA( int iHenirBuff )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if ( g_pX2Game != NULL && g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON && g_pX2Game->GetMyUnit() != NULL)
	{
		switch(iHenirBuff)
		{
		case 0:
			g_pX2Game->GetMyUnit()->EndHenirBuff();
			break;
		case 1:
			g_pX2Game->GetMyUnit()->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_HENIR_FIRE ) );
			break;
		case 2:
			g_pX2Game->GetMyUnit()->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_HENIR_WATER ) );
			break;
		case 3:
			g_pX2Game->GetMyUnit()->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_HENIR_NATURE ) );
			break;
		case 4:
			g_pX2Game->GetMyUnit()->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_HENIR_WIND ) );
			break;
		case 5:
			g_pX2Game->GetMyUnit()->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_HENIR_LIGHT ) );
			break;
		case 6:
			g_pX2Game->GetMyUnit()->StartHenirBuffDark();
			break;
		case 7:
			g_pX2Game->GetMyUnit()->StartHenirBuffFirst();
			break;
		default:
			g_pX2Game->GetMyUnit()->EndHenirBuff();
			break;
		}
	}
}

void CX2GameEdit::ElswordWSPCheat_LUA( int iWSP )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if ( g_pX2Game != NULL && g_pX2Game->GetMyUnit() != NULL)
	{
		g_pX2Game->GetMyUnit()->SetWayOfSwordPoint( (float)iWSP );
	}
}


//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
void CX2GameEdit::EDMonitoringModeCheat_LUA( bool bMode )
{
	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		KEGS_ADMIN_ED_MONITORING_CHEAT_NOT kNot;
		kNot.m_bEDMonitoringMode = bMode;

		if( g_pData != NULL )
			g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_ED_MONITORING_CHEAT_NOT, kNot );

		if( g_pInstanceData != NULL )
			g_pInstanceData->SetMoneyMonitoring(bMode);
	}
}
//}}

void CX2GameEdit::RefreshDailyQuest()
{
	if( g_pData != NULL &&
		g_pData->GetQuestManager() != NULL)
	{
		g_pData->GetQuestManager()->RefreshDailyQuest();
	}
}

void CX2GameEdit::AddCommandChange( const char* pOrgCommand, const char* pChangeCommand )
{
	wstring temp;
	ConvertUtf8ToWCHAR( temp, pOrgCommand );
	string orgCommand;
	ConvertWCHARToChar( orgCommand, temp.c_str() );

	ConvertUtf8ToWCHAR( temp, pChangeCommand );
	string changeCommand;
	ConvertWCHARToChar( changeCommand, temp.c_str() );

	m_mapCommandChange.insert( map<string, string>::value_type(orgCommand, changeCommand) );
}
void CX2GameEdit::AddHelperText( const char* pHelperText, CX2User::X2_USER_AUTH_LEVEL userLevel )
{
	wstring helperText;
	ConvertUtf8ToWCHAR( helperText, pHelperText );

	switch( userLevel )
	{
	case CX2User::XUAL_DEV:
		{
			CKTDGUIListBox* pListBox = (CKTDGUIListBox*) m_pDLGGameEdit->GetControl( L"GameEdit_Help_Dev" );
			if( NULL != pListBox )
			{
				pListBox->AddItem(helperText.c_str(), NULL);
			}
		} break;
	case CX2User::XUAL_OPERATOR:
		{
			CKTDGUIListBox* pListBox = (CKTDGUIListBox*) m_pDLGGameEdit->GetControl( L"GameEdit_Help_Operator" );
			if( NULL != pListBox )
			{
				pListBox->AddItem(helperText.c_str(), NULL);
			}
		} break;

	case CX2User::XUAL_SPECIAL_USER:
	case CX2User::XUAL_NORMAL_USER:
	default:
		{
			return;
		} break;
	}
}

void CX2GameEdit::ToggleCommandList()
{
	CX2User::X2_USER_AUTH_LEVEL userLevel = g_pData->GetMyUser()->GetAuthLevel();
	CKTDGUIListBox* pListBox = NULL;
	switch( userLevel )
	{
	case CX2User::XUAL_DEV:
		{
			pListBox = (CKTDGUIListBox*) m_pDLGGameEdit->GetControl( L"GameEdit_Help_Dev" );
		} break;
	case CX2User::XUAL_OPERATOR:
		{
			pListBox = (CKTDGUIListBox*) m_pDLGGameEdit->GetControl( L"GameEdit_Help_Operator" );
		} break;

	case CX2User::XUAL_SPECIAL_USER:
	case CX2User::XUAL_NORMAL_USER:
	default:
		{
		} break;
	}

	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGGameEdit->GetControl( L"StaticGameEditCommandBG" );
	if( pStatic != NULL )
	{
		pStatic->SetShow( !pStatic->GetShow() );
		pStatic->SetEnable( !pStatic->GetEnable() );
	}

	if( NULL != pListBox )
	{
		pListBox->SetShow( pStatic->GetShow() );
		pListBox->SetEnable( pStatic->GetShow() );
	}
}

bool CX2GameEdit::Reset()
{
	if ( OpenScriptFile( L"GameEditCommandList.lua" ) == true )
	{
		AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_219 ) );
		return true;
	}

	AddString( CX2ChatWindow::CT_KILL, GET_STRING( STR_ID_220 ) );
	return false;
}

void CX2GameEdit::RegisterLuaBind()
{
	//바인더에 등록
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pGameEdit", this );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pX2Game", g_pX2Game);
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pState", m_pState );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pData", g_pData );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pMain", g_pMain );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pKTDXApp", g_pKTDXApp );

	// g_pSquareGame / g_pTFieldGame 상황에 따라 정의
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pSquareGame", g_pSquareGame);

}

bool CX2GameEdit::OpenScriptFile( const WCHAR* pFilename )
{
	RegisterLuaBind();


    if ( g_pKTDXApp->LoadLuaTinker( pFilename ) == false )
    {
		return false;
    }

	return true;
}



void CX2GameEdit::AddCommandHistory( const WCHAR* pCommandMsg )
{
	if ( pCommandMsg == NULL )
		return;

	wstring commandMsg = pCommandMsg;
	if ( m_CommandHistory.size() >= 10 )
	{
		m_CommandHistory.erase( m_CommandHistory.begin() );
	}
	m_CommandHistory.push_back( commandMsg );
	m_NowCommandHistoryIndex = m_CommandHistory.size();
}

bool CX2GameEdit::SetCommandByUpDownArrow( int tempCommandHistoryIndex )
{
	if ( (int)m_CommandHistory.size() == 1 && tempCommandHistoryIndex == 0 )
	{

	}
	else
	{
		if ( tempCommandHistoryIndex < 0 || tempCommandHistoryIndex >= (int)m_CommandHistory.size() )
		{
			return true;
		}
	}

	m_NowCommandHistoryIndex = tempCommandHistoryIndex;

	CKTDGUIIMEEditBox* pIMEEditBoxCommand = (CKTDGUIIMEEditBox*)m_pDLGGameEdit->GetControl(L"IMEEditBoxGameEditCommand");
	if ( pIMEEditBoxCommand != NULL )
	{
		wstring nowHistoryCommand = m_CommandHistory[m_NowCommandHistoryIndex];
		pIMEEditBoxCommand->SetText( nowHistoryCommand.c_str() );
	}
	else
	{
		return false;
	}

	return true;
}

//{{ 2007. 11. 12  최육사  현재 로드된 디바이스들을 파일로 출력
void CX2GameEdit::SaveFileLoadedDevices()
{
	std::vector< std::pair< std::wstring, std::pair< int, int > > > vecLoadedDeviceInfo;
	g_pKTDXApp->GetDeviceManager()->GetDeviceNameAndRefCount( vecLoadedDeviceInfo );

	std::ofstream ofs( "LoadedDeviceInfo.txt" );
	if( ofs.is_open() == false )
	{
		ofs.close();
		return;
	}

	ofs << "<<< Loaded Devices List >>>" << std::endl << std::endl;

	std::string strTemp;
	std::vector< std::pair< std::wstring, std::pair< int, int > > >::iterator vit = vecLoadedDeviceInfo.begin();
	for( ; vit != vecLoadedDeviceInfo.end(); vit++ )
	{
		ConvertWCHARToChar( strTemp, vit->first.c_str() );
		ofs << "RefCount : " << vit->second.first << "       Size : " << vit->second.second << "       DeviceName : " << strTemp <<   std::endl;
	}

	ofs.close();
}
//}}
//{{ 2009-11-30  최육사
void CX2GameEdit::AdminGetGuildSPoint_LUA( int iCount )
{

	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}


	CX2State* pState = (CX2State*) g_pMain->GetNowState();
	pState->Handler_EGS_ADMIN_GET_GUILD_SKILL_POINT_REQ( iCount );
}
//}}




//{{ lucidash : 2010.11.24 . 드롭뷰어 치트.
#if X2_DEV_TOOLS

void CX2GameEdit::ItemViewCheat( int iItemID )
{
	if ( NULL == g_pX2Game ||
		 NULL == g_pX2Game->GetMyUnit() )
	{
		 return;
	}

    const CX2Item::ItemTemplet* pTemplet = g_pData->GetItemManager()->GetItemTemplet( iItemID );

	D3DXVECTOR3 pos = g_pX2Game->GetMyUnit()->GetPos() + D3DXVECTOR3( 100 , 100 , 0 );
	CKTDGParticleSystem::CParticleEventSequenceHandle hSeq = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( NULL,
        pTemplet->GetDropViewer(),
        pos, 0 , 0 );

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( hSeq );

	if( NULL != pSeq )
	{
		pSeq->SetAlphaObject( false );
		pSeq->SetLandPosition( pos.y );

		pSeq->CreateNewParticle( pos );
		int iSpeedY = ((int) RandomInt( 1 , 1000 ))%9 + 12;
		pSeq->SetLifetime( CMinMax<float>( 5.0f, 5.0f ) );
		pSeq->SetTriggerTime( 5.0f );
		pSeq->SetTriggerCount( -1 );
		pSeq->SetVelocity(D3DXVECTOR3( 0.f, (float)iSpeedY * 43.f, 0.f ));
	}
}

#endif // ITEM_VIEW_CHEAT
//}} lucidash : 2010.11.24  .  드롭뷰어 치트.


/** @function : RequireGetDangerousValue_LUA
	@breif : 필드 위험도 확인 치트
	*/
void CX2GameEdit::RequireGetDangerousValue_LUA()
{
	if( CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() )
		return;

	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		g_pData->GetServerProtocol()->SendID( EGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT );
	}
}



void CX2GameEdit::IncreaseFieldDangerousValue_LUA( int iAddValue_)
{
	if( CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() )
		return;

	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		KEGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_REQ kPacket;
		kPacket.m_iDanger = iAddValue_;
		g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_REQ, kPacket );
		g_pMain->AddServerPacket( EGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_ACK );

	}
}


void CX2GameEdit::IncreaseLoverDate_LUA( int iAddValue_)
{
	if ( NULL != g_pData->GetRelationshipManager() &&
		 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
	{
		if ( SEnum::RT_SOLO == g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType )
		{
			AddString( CX2ChatWindow::CT_NORMAL, L"이 치트는 커플, 결혼 일때만 사용 할 수 있습니다.");
		}
		else
		{
			if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
			{
				KEGS_ADMIN_CHANGE_COUPLE_DATE_REQ kPacket;
				kPacket.m_iAddHour = iAddValue_;
				g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANGE_COUPLE_DATE_REQ, kPacket );
				g_pMain->AddServerPacket( EGS_ADMIN_CHANGE_COUPLE_DATE_ACK );

			}

		}

	}


}



//{{ kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드
/** @function : AveragePingTime_LUA
	@brief : 주기적으로 호스트를 변경하는 기준이 되는 핑값을 테스트 값으로 변경함
	@param : 변경할 핑값( 0이면 실제 평균 핑값을 사용하고 0이 아니면 테스트 값을 사용함)
*/
void CX2GameEdit::AveragePingTime_LUA( DWORD dwAveragePingTime_ )
{
	if ( NULL != g_pX2Room )
		g_pX2Room->SetAveragePingTimeForGameEdit( dwAveragePingTime_ );
}
//}} kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드

bool CX2GameEdit::SetValueByHeroMatchWisperNotice( KLuaManagerProxy& kLuaManager, std::vector<KLuaManagerProxyLuaValue>& vec_, string wstrCommandString, int iNowStringIndex_ )
{
	const int iMaxValueNum		= 2;
	int		  iValueIndex		= 0;
	int		  iPrevStringIndex	= 0;

	while(1)
	{
		iPrevStringIndex = iNowStringIndex_ + 1;

		iNowStringIndex_	= wstrCommandString.find( " ", iPrevStringIndex );

		if( iMaxValueNum > iValueIndex && iNowStringIndex_ == -1 )	/// 만약 인자 중 입력되지 않은 부분이 있다면, 탈출
		{
			AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
			return false;
		}

		if( iMaxValueNum <= iValueIndex )		/// 마지막 인자인 공지 문구 입력할 땐 문장 끝까지 받아오자. ( 띄어쓰기 포함 )
		{
			iNowStringIndex_ = (int)wstrCommandString.size();
		}

		if( 1 == iValueIndex || 2 == iValueIndex )		/// 쌍따옴표를 추가해 주어야 할 인자 ( 문자 )
		{
			string noticeMsg;
			noticeMsg.assign( wstrCommandString.begin() + iPrevStringIndex, wstrCommandString.begin() + iNowStringIndex_ );
            vec_.push_back( KLuaManagerProxyLuaValue( noticeMsg ) );
		}
		else	/// 그냥 인자
		{
			string noticeMsg;
			noticeMsg.assign( wstrCommandString.begin() + iPrevStringIndex, wstrCommandString.begin() + iNowStringIndex_ );
            KLuaManagerProxyLuaValue    luaValue;
            switch( kLuaManager.LuaParse( noticeMsg, true, luaValue ) )
            {
            case KLuaManagerProxy::LUA_PARSE_VALUE:
                vec_.push_back( luaValue );
                break;
            default:
			    AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
                return false;
            }//switch
		}

		if( iMaxValueNum > iValueIndex )	/// 최대 인자 수를 넘지 않았다면 쉼표 추가
        {
        }
		else								/// 최대 인자 수를 넘었다면, 탈출
			break;

		++iValueIndex;
	}

	return true;
}

bool CX2GameEdit::SetValueByHeroMatchDeleteGroupUser( KLuaManagerProxy& kLuaManager, std::vector<KLuaManagerProxyLuaValue>& vec_, string wstrCommandString, int iNowStringIndex_ )
{
	const int iMaxValueNum		= 1;
	int		  iValueIndex		= 0;
	int		  iPrevStringIndex	= 0;

	while(1)
	{
		iPrevStringIndex = iNowStringIndex_ + 1;

		iNowStringIndex_	= wstrCommandString.find( " ", iPrevStringIndex );

		if( iMaxValueNum > iValueIndex && iNowStringIndex_ == -1 )	/// 만약 인자 중 입력되지 않은 부분이 있다면, 탈출
		{
			AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
			return false;
		}

		if( iMaxValueNum <= iValueIndex )		/// 마지막 인자인 공지 문구 입력할 땐 문장 끝까지 받아오자. ( 띄어쓰기 포함 )
		{
			iNowStringIndex_ = (int)wstrCommandString.size();
		}

		if( 1 == iValueIndex )		/// 쌍따옴표를 추가해 주어야 할 인자 ( 문자 )
		{
			string noticeMsg;
			noticeMsg.assign( wstrCommandString.begin() + iPrevStringIndex, wstrCommandString.begin() + iNowStringIndex_ );
            vec_.push_back( KLuaManagerProxyLuaValue( noticeMsg ) );
		}
		else	/// 그냥 인자
		{
			string noticeMsg;
			noticeMsg.assign( wstrCommandString.begin() + iPrevStringIndex, wstrCommandString.begin() + iNowStringIndex_ );
            KLuaManagerProxyLuaValue    luaValue;
            switch( kLuaManager.LuaParse( noticeMsg, true, luaValue ) )
            {
            case KLuaManagerProxy::LUA_PARSE_VALUE:
                vec_.push_back( luaValue );
                break;
            default:
			    AddString( CX2ChatWindow::CT_NORMAL, GET_STRING( STR_ID_217 ) );
                return false;
            }//switch
		}

		if( iMaxValueNum > iValueIndex )	/// 최대 인자 수를 넘지 않았다면 쉼표 추가
        {
        }
		else								/// 최대 인자 수를 넘었다면, 탈출
			break;

		++iValueIndex;
	}

	return true;
}


bool CX2GameEdit::DeleteGUildAdd_LUA( int iGuildUID )
{
	KEGS_ADMIN_DELETE_GUILD_ADD_NOT kPacket;
	kPacket.m_iGuildUID = iGuildUID;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_DELETE_GUILD_ADD_NOT, kPacket );

	return true;
}



//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )

bool CX2GameEdit::DeleteHenirRankingRank_LUA( int nRankingType, int nRank )
{
	KEGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT kPacket;
	kPacket.m_iRankingType = nRankingType;
	kPacket.m_iRank = nRank;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_DELETE_HENIR_RANKING_RANK_NOT, kPacket );

	return true;
}

bool CX2GameEdit::DeleteHenirRankingNickName_LUA( int nRankingType, const char* szNickName )
{
	wstring wstrNickName;
	ConvertCharToWCHAR( wstrNickName, szNickName );

	KEGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT kPacket;
	kPacket.m_iRankingType = nRankingType;
	kPacket.m_wstrNickName = wstrNickName;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_DELETE_HENIR_RANKING_NICKNAME_NOT, kPacket );

	return true;

}

bool CX2GameEdit::DeleteHenirRankingUnitUID_LUA( int nRankingType, UidType iUnitUID )
{
	KEGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT kPacket;
	kPacket.m_iRankingType = nRankingType;
	kPacket.m_iUnitUID = iUnitUID;
	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_DELETE_HENIR_RANKING_UNITUID_NOT, kPacket );

	return true;
}
//}}

void CX2GameEdit::ToggleInvisible_LUA()
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	CX2State* pState = (CX2State*) g_pMain->GetNowState();
	pState->Handler_EGS_TOGGLE_INVISIBLE_REQ();
}


void CX2GameEdit::FullChargeRidingStamina( int iValue_ )
{
	if ( NULL != CX2RidingPetManager::GetInstance() )
		CX2RidingPetManager::GetInstance()->FullChargeRidingStamina( iValue_ );
}

#if X2_DEV_TOOLS //JHKang
void CX2GameEdit::ToggleClassChangeCheat()
{
	m_bShowClassChangeCheat = !m_bShowClassChangeCheat;

	if( true == m_bShowClassChangeCheat )
	{
		m_pDLGClassChangeCheat = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Change_Class_Cheat.lua", 0.07f, XDL_GAME_EDIT );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGClassChangeCheat );

		if ( NULL != m_pDLGClassChangeCheat )
		{
			CKTDGUIListBox* pListBox = (CKTDGUIListBox*) m_pDLGClassChangeCheat->GetControl( L"Class_List" );

			if ( NULL != pListBox )
			{
				pListBox->SetShowAllItems( true );

				for ( int i = 1; i < CX2Unit::UC_END; ++i )
				{
					wstring wClassName = g_pKTDXApp->GetEnumToString( L"UNIT_CLASS", i );
					if ( wClassName.empty() != true )
						pListBox->AddItem( wClassName.c_str(), NULL, i, L"OO" );
				}
			}
		}
	}
	else
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGClassChangeCheat, NULL, false );
	}
}
#endif //CHEAT_CLASS_CHANGE

void CX2GameEdit::HotKeyProcess()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		CX2User::XUAL_DEV > g_pData->GetMyUser()->GetAuthLevel()  )
		return;

	if( true == g_pChatBox->GetChatEditBox() )
		return;

	HotKeyProcess_BossRaid();
#if X2_DEV_TOOLS
	HotKeyProcess_FreeTraining();
#endif // SUMMON_MONSTER_BY_PRESS_KEY_AT_TRAINING_GAME
}
#if X2_DEV_TOOLS
void CX2GameEdit::HotKeyProcess_FreeTraining()
{
	if( NULL == g_pX2Game )
		return;

	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		CX2User::XUAL_DEV > g_pData->GetMyUser()->GetAuthLevel()  )
		return;

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetWorldData() &&
	    g_pX2Game->GetWorld()->GetWorldData()->worldID == CX2World::WI_TRAINING_CENTER_FREESTAGE )
	{
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_1) == TRUE )
		{
			g_pX2Game->Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA( CX2UnitManager::NUI_DUMMY_MOVE, 70, g_pX2Game->GetMyUnit()->GetPos(), true, 0, 0, 0, 0 );
			g_pChatBox->AddChatLog( L"NUI_DUMMY_MOVE 소환", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_2) == TRUE )
		{
			g_pX2Game->Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA( CX2UnitManager::NUI_DUMMY_WAIT, 70, g_pX2Game->GetMyUnit()->GetPos(), true, 0, 0, 0, 0 );
			g_pChatBox->AddChatLog( L"NUI_DUMMY_WAIT 소환", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_3) == TRUE )
		{
			g_pX2Game->Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA( CX2UnitManager::NUI_DUMMY_BIG_MOVE, 70, g_pX2Game->GetMyUnit()->GetPos(), true, 0, 0, 0, 0 );
			g_pChatBox->AddChatLog( L"NUI_DUMMY_BIG_MOVE 소환", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_4) == TRUE )
		{
			g_pX2Game->Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA( CX2UnitManager::NUI_DUMMY_BIG_WAIT, 70, g_pX2Game->GetMyUnit()->GetPos(), true, 0, 0, 0, 0 );
			g_pChatBox->AddChatLog( L"NUI_DUMMY_BIG_WAIT 소환", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
	}
}
#endif // SUMMON_MONSTER_BY_PRESS_KEY_AT_TRAINING_GAME
void CX2GameEdit::HotKeyProcess_BossRaid()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		CX2User::XUAL_DEV > g_pData->GetMyUser()->GetAuthLevel()  )
		return;

	if( true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
	{
		CX2UnitManager::NPC_UNIT_ID eNpcID =
			CX2BossRaidManager::GetInstance()->GetBossNPCIDByMapID(static_cast<SEnum::VILLAGE_MAP_ID>(g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam()));

		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_4) == TRUE )
		{
			CX2BossRaidManager::GetInstance()->Handler_EGS_BOSS_FIELD_INTRUDE_RESTRICTION_REQ_Cheat();
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_30361 ), g_pMain->GetNowState() );
		}
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetDoubleKeyState(DIK_5) == TRUE )
		{
			SetLastCreatedMonster( eNpcID );
		}
	}
}
void CX2GameEdit::OpenRaidGate()
{
	CX2BossRaidManager::GetInstance()->Handler_EGS_ADMIN_BOSS_FIELD_GATE_OPEN_REQ();
}
void CX2GameEdit::CloseRateGate()
{
	CX2BossRaidManager::GetInstance()->Send_EGS_ADMIN_BOSS_FIELD_GATE_CLOSE_NOT();
}
void CX2GameEdit::RequireGetTotalDangerousValue()
{
	CX2BossRaidManager::GetInstance()->Handler_EGS_ADMIN_GET_TOTAL_DANGEROUS_VALUE_REQ();
}
void CX2GameEdit::IncreaseTotalDangerousValue(int iValue_)
{
	CX2BossRaidManager::GetInstance()->Handler_EGS_ADMIN_SET_TOTAL_DANGEROUS_VALUE_REQ(iValue_);
}
