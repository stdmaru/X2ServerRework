#pragma once


#if X2_DEV_TOOLS

class ItemViewListDialog
{

public:
	ItemViewListDialog()
		: m_pDLGItemViewList( NULL )
		, m_bSortItemViewList( false )
	{
	}

	~ItemViewListDialog()
	{
		SAFE_DELETE_DIALOG( m_pDLGItemViewList );
	}

	bool IsOpen()
	{
		if( m_pDLGItemViewList == NULL )
			return false;

		if( false == m_pDLGItemViewList->GetShow() )
			return false;
		else
			return true;
	}

	void Close()
	{
		if( m_pDLGItemViewList != NULL )
		{
			m_pDLGItemViewList->SetShowEnable( false, false );
		}
	}

	void Open( CKTDXStage* pStage )
	{
		if( m_pDLGItemViewList == NULL )
		{
			m_pDLGItemViewList = new CKTDGUIDialog( pStage, L"DLG_Monster_List_Window.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGItemViewList );

			Update( m_bSortItemViewList );
		}

		if( m_pDLGItemViewList != NULL )
		{
			m_pDLGItemViewList->SetStage( pStage );
			m_pDLGItemViewList->SetShowEnable( true, true );
		}
	}

	void Update( bool bSortAsscending );

	void KeyProcess( CKTDXStage* pStage );
	void OnCommand( const int& ItemID );
	void OnChangeSorting()
	{
		m_bSortItemViewList = !m_bSortItemViewList;
		Update( m_bSortItemViewList );
	}

	CKTDGUIDialogType GetDialog() { return m_pDLGItemViewList; };

protected:
	CKTDGUIDialogType m_pDLGItemViewList;

	bool m_bSortItemViewList;


};

#endif // ITEM_VIEW_CHEAT




#if X2_DEV_TOOLS
class CreateMonsterListDialog
{
public:
	CreateMonsterListDialog()
		: m_pDLGCreateMonsterList( NULL )
		, m_bSortMonsterList( false )
	{
	}

	~CreateMonsterListDialog()
	{
		SAFE_DELETE_DIALOG( m_pDLGCreateMonsterList );
	}

	bool IsOpen()
	{
		if( m_pDLGCreateMonsterList == NULL )
			return false;

		if( false == m_pDLGCreateMonsterList->GetShow() )
			return false;
		else
			return true;
	}

	void Close()
	{
		if( m_pDLGCreateMonsterList != NULL )
		{
			m_pDLGCreateMonsterList->SetShowEnable( false, false );
		}
	}

	void Open( CKTDXStage* pStage )
	{
		if( m_pDLGCreateMonsterList == NULL )
		{
			m_pDLGCreateMonsterList = new CKTDGUIDialog( pStage, L"DLG_Monster_List_Window.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCreateMonsterList );

			Update( m_bSortMonsterList );
		}

		if( m_pDLGCreateMonsterList != NULL )
		{
			m_pDLGCreateMonsterList->SetStage( pStage );
			m_pDLGCreateMonsterList->SetShowEnable( true, true );
		}
	}

	void Update( bool bSortAlphabetical );

	void KeyProcess( CKTDXStage* pStage );
	void OnCommand( const std::wstring& monsterName );
	void OnChangeSorting()
	{
		m_bSortMonsterList = !m_bSortMonsterList;
		Update( m_bSortMonsterList );
	}

	CKTDGUIDialogType GetDialog() { return m_pDLGCreateMonsterList; }

protected:
	CKTDGUIDialogType m_pDLGCreateMonsterList;

	bool m_bSortMonsterList;

}; // class CreateMonsterListDialog

#endif // CREATE_MONSTER_LIST_TEST




#if X2_DEV_TOOLS
// class MonsterStateListDialog
// {
// public:
// 	MonsterStateListDialog()
// 		: m_pDLGMonsterStateList( NULL )
// 		, m_NPCID( CX2UnitManager::NUI_NONE )
// 	{
// 		m_vecStateName.reserve( 1024 );
// 	}
//
// 	~MonsterStateListDialog()
// 	{
// 		SAFE_DELETE_DIALOG( m_pDLGMonsterStateList );
// 	}
//
// 	bool IsOpen()
// 	{
// 		if( m_pDLGMonsterStateList == NULL )
// 			return false;
//
// 		if( false == m_pDLGMonsterStateList->GetShow() )
// 			return false;
// 		else
// 			return true;
// 	}
//
// 	void Close()
// 	{
// 		if( m_pDLGMonsterStateList != NULL )
// 		{
// 			m_pDLGMonsterStateList->SetShowEnable( false, false );
// 		}
// 	}
//
// 	void Open( CKTDXStage* pStage )
// 	{
// 		if( m_pDLGMonsterStateList == NULL )
// 		{
// 			m_pDLGMonsterStateList = new CKTDGUIDialog( pStage, L"DLG_Monster_List_Window.lua" );
// 			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMonsterStateList );
// 		}
//
// 		if( m_pDLGMonsterStateList != NULL )
// 		{
// 			m_pDLGMonsterStateList->SetStage( pStage );
// 			m_pDLGMonsterStateList->SetShowEnable( true, true );
//
// 			Update();
// 		}
// 	}
//
// 	void Update();
//
// 	void KeyProcess( CKTDXStage* pStage );
//
// 	void OnCommand( const std::wstring& monsterName );
//
//
// 	CKTDGUIDialogType GetDialog() { return m_pDLGMonsterStateList; }
//
//
//
// protected:
// 	CKTDGUIDialogType m_pDLGMonsterStateList;
// 	std::vector< std::wstring > m_vecStateName;
// 	CX2UnitManager::NPC_UNIT_ID m_NPCID;
//
// }; // class MonsterStateListDialog
//
#endif // MONSTER_STATE_LIST_TEST



/** @class  BadAttitudeUserWarningDialog
    @brief  불량 유저에 대한 경고 메시지 다이알로그

    @desc   Instance 로 제작되어 있다.
			DungeonGame 상태에서만 적용되므로 StateDungeonGame 에 위치해있다.
			DLG 가 항상 등장하는 것이 아니니 차라리 동적으로 할당할 수 있게 제작할까 고민 중이다.

    @date   김종훈 on 2013-01-24, 14:32
*/





class CX2StateDungeonGame : public CX2StateMenu
{
public:

	enum DUNGEON_GAME_UI_CUSTOM_MSG
	{
		DGUCM_GAME_LOADING = 0,
		DGUCM_GAME_START,
		DGUCM_GAME_EXIT_OK,

		DGUCM_GAME_EXIT_CANCEL,

		DGUCM_SUMMON_SELECTED_MONSTER_DEBUG,
		DGUCM_SORT_MONSTER_LIST_DEBUG,

		DGUCM_GOTO_FIRST_DUNGEON_OK,
		DGUCM_GOTO_FIRST_DUNGEON_CANCEL,

		DGUCM_TOTORIAL_EXIT,

		DGUCM_WANT_TO_ENTER_SECRET_STAGE_OK,
		DGUCM_WANT_TO_ENTER_SECRET_STAGE_CANCEL,

		DGUCM_CLOSE_BAD_ATTITUDE_WARNING_DIALOG,

		DGUCM_AGREE_ENTER_GATE_OF_DARKNESS,		/// 어둠의 문 입장 동의
		DGUCM_NOT_AGREE_ENTER_GATE_OF_DARKNESS,	/// 어둠의 문 입장 거부

	};
// 스크립트와 인덱스 맞춰야 함. DLG_PVP_GAME_LOADING_STAT_USER.lua
	enum LOADING_USER_INFO
	{
		LUI_ELSWORD_COLOR = 0,
		LUI_ARME_COLOR,
		LUI_LIRE_COLOR,
		LUI_RAVEN_COLOR,
		LUI_EVE_COLOR,
		LUI_CHUNG_COLOR,
		LUI_ARA_COLOR,


		LUI_ELSWORD_BLACK,
		LUI_ARME_BLACK,
		LUI_LIRE_BLACK,
		LUI_RAVEN_BLACK,
		LUI_EVE_BLACK,
		LUI_CHUNG_BLACK,
		LUI_ARA_BLACK,
//#ifdef NEW_CHARACTER_EL
		LUI_EL_COLOR,
		LUI_EL_BLACK,
//#endif // NEW_CHARACTER_EL
		LUI_ADD_COLOR,
		LUI_ADD_BLACK,
		LUI_EMPTY,
	};

	struct DungeonMapUI
	{
	public:
		struct SubStageNode
		{
			vector< std::pair<int, int> >		m_vecNextSubStageIndex;
		};

	public:
        DungeonMapUI( CX2Dungeon* pDungeon, CX2State* pNowState )
		{
			m_bShow = false;

			m_pDungeon = pDungeon;
			CreatStageGraph( pDungeon );

			m_pNowState = pNowState;
			m_pDLGDungeonMap		= NULL;
			m_pDLGDungeonMap		= new CKTDGUIDialog( (CKTDXStage*) m_pNowState, L"DLG_DungeonMap_UI.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGDungeonMap );
			m_vPos = m_pDLGDungeonMap->GetPos();

			SetShow( m_bShow );
		}

		~DungeonMapUI()
		{
            SAFE_DELETE_DIALOG( m_pDLGDungeonMap );
		}

		bool GetShow() const { return m_bShow; }
		void SetShow( bool bShow )
		{
			m_bShow = bShow;

			if( m_pDLGDungeonMap != NULL )
			{
				m_pDLGDungeonMap->SetShowEnable( m_bShow, m_bShow );
			}
		}

		void CreatStageGraph( CX2Dungeon* pDungeon );
		void CreateDungeonMapUI();
		void ResizeBackground();
		void OnFirstSubStage();

		void SetToOriginalPos()
		{
			if( m_pDLGDungeonMap == NULL )
				return;

			m_pDLGDungeonMap->Move( m_vPos, D3DXCOLOR(1, 1, 1, 1), 0.01f, true );
		}

	private:
		CKTDGUIDialogType			m_pDLGDungeonMap;
		CX2State*				m_pNowState;
		CX2Dungeon*				m_pDungeon;

		map< std::pair<int, int>, SubStageNode> m_mapStageGraph;

		bool					m_bShow;

		D3DXVECTOR2				m_vPos;
	};


	struct DungeonEndingEvent
	{
		DungeonEndingEvent();
		~DungeonEndingEvent();

		void OnFrameMove( double fTime, float fElapsedTime );

		//{{ kimhc // 2010.12.13 // 2010-12-23 New Character CHUNG
		void SetDungeonClearTexture( CKTDGUIStatic* pStaticClearTexture_ );
		//}} kimhc // 2010.12.13 //  2010-12-23 New Character CHUNG

		void Start()
		{
			m_bStarted = true;
			m_fElapsedTime = 0.f;
			m_bEnd = false;
		}

		void StartChapter2()
		{
			m_bStartChapter2 = true;
		}

	public:
		CKTDGUIDialogType m_pDLGSpeechBalloon;
		CKTDGUIDialogType m_pDLGCharacterPicture;

		float		m_fElapsedTime;
		bool		m_bStarted;
		bool		m_bEnd;

		bool		m_bEndChapter1;
		bool		m_bStartChapter2;
		float		m_fSpeechElapsedTime;

		int			m_iIndexSpeech1;
		int			m_iIndexSpeech2;


		float		m_fElapsedTime_ShowClearEffect;
		float		m_fElapsedTime_WinEffect;

		std::vector< wstring > m_vecSpeech1;
		std::vector< wstring > m_vecSpeech2;

	};




	struct SecretStageEnteringEvent
	{
		SecretStageEnteringEvent();
		~SecretStageEnteringEvent();

		void OnFrameMove( double fTime, float fElapsedTime );

		void Start()
		{
			SAFE_DELETE_DIALOG( m_pDLGSpeechBalloon );
			SAFE_DELETE_DIALOG( m_pDLGCharacterPicture );

			m_bStarted = true;
			m_fElapsedTime = 0.f;
			m_bEnd = false;
			m_bCreatedChallengeParticle = false;
		}

		void End()
		{
			m_fElapsedTime = 0.f;
			m_bStarted = false;
			m_bEnd = true;

			if( m_pDLGCharacterPicture != NULL )
				m_pDLGCharacterPicture->Move( D3DXVECTOR2(0, 0), D3DXCOLOR(1, 1, 1, 0), 0.25f, false );
			if( m_pDLGSpeechBalloon != NULL )
				m_pDLGSpeechBalloon->Move( D3DXVECTOR2(0, 0), D3DXCOLOR(1, 1, 1, 0), 0.25f, false );
		}

	public:
		CKTDGUIDialogType m_pDLGSpeechBalloon;
		CKTDGUIDialogType m_pDLGCharacterPicture;

		float		m_fElapsedTime;
		bool		m_bStarted;
		bool		m_bEnd;

		float		m_fSpeechElapsedTime;
		int			m_iIndexSpeech1;

		float		m_fElapsedTime_ShowClearEffect;

		std::vector< wstring > m_vecSpeech1;
		CX2UnitManager::NPC_UNIT_ID m_eBossNPCID;

		bool m_bCreatedChallengeParticle;
	};


	struct DRAWFACE_RHW_VERTEX
	{
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

	struct DUNGEON_LOADING_DATA
	{
		int iLoadingPercent;
		UidType uidUnitUID;

		DUNGEON_LOADING_DATA()
		{
			iLoadingPercent = 0;
			uidUnitUID = -1;
		}
	};

public:
	CX2StateDungeonGame(void);
	~CX2StateDungeonGame(void);

	virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
	virtual HRESULT OnFrameRender();

	virtual HRESULT OnResetDevice();
	virtual HRESULT OnLostDevice();

	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	virtual void SetShowStateDLG( bool /*bShow*/ ) {};


	bool DungeonStageLoadReq( int stageNum );
	bool EndDungeonGameReq( CX2Room::TEAM_NUM winTeam );		// 게임을 정상적으로 마친경우


	void OpenExitDLG( const WCHAR* wszMsg );

	virtual void PopTalkBox( UidType iUnitUID_, const WCHAR* pWstrMsg_,
		const D3DXCOLOR& coTextColor_, const D3DXCOLOR& coOutColor_, const D3DXCOLOR& coBackColor_ );

	DungeonMapUI* GetDungeonMapUI() const { return m_pDungeonMapUI; }

	CX2StateDungeonGame::DungeonEndingEvent* GetDungeonEndingEvent() { return &m_DungeonEndingEvent; }



	CX2StateDungeonGame::SecretStageEnteringEvent* GetSecretStageEnteringEvent() { return &m_SecretStageEnteringEvent; }

#ifdef OLD_COMBO_LIST_UI
	void CreateComboListForTutorial();
#endif // OLD_COMBO_LIST_UI


	// 09.03.23 태완 : public으로 이동
	void ToggleDungeonMapUI();


	bool Handler_EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_REQ();
	bool Handler_EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Send_EGS_DUNGEON_SECRET_STAGE_ENTER_SELECT_NOT( bool bWantToEnterSecretStage );
	bool Handler_EGS_DUNGEON_SECRET_STAGE_ENTER_RESULT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	int GetStartSecretStageEnteringEvent(){ return m_iStartSecretStageEnteringEvent; }
	void InitStartSecretStageEnteringEvent(){ m_iStartSecretStageEnteringEvent = 0; }
	bool SecretDungeonStageLoadReq(int iVal);

	bool Handler_EGS_DUNGEON_KILLALLNPC_CHECK_REQ();
	bool Handler_EGS_DUNGEON_KILLALLNPC_CHECK_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



	void				AllocateFaultyPlayerWarningDlg ();							// DLG 할당
	void				DeallocateFaultyPlayerWarningDlg ();						// DLG 삭제
	void				ReservingWarningDlgDelete ();								// DLG 삭제 예약

	bool Handler_EGS_DECISION_ENTER_DEFENCE_DUNGEON_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_UPDATE_INVENTORY_SLOT_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_LEAVE_ROOM_BEFORE_DEFENCE_DUNGEON_START_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_ENTER_DEFENCE_DUNGEON_POPUP_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void Sned_Handler_EGS_ENTER_DEFENCE_DUNGEON_POPUP_REPLY_NOT( bool bAgree );

	/// 어둠의 문 시작까지의 대기 시간
	float GetWaitDefenceDungeonStartTime() const { return m_fWaitDefenceDungeonStartTime; }
	void SetWaitDefenceDungeonStartTime(float val) { m_fWaitDefenceDungeonStartTime = val; }

	void WaitDefenceDungeonStart( const float fElapsedTime );		/// 특정 시간 이후 어둠의 문 스테이트 변경을 처리하는 함수

	bool Handler_EGS_DUNGEON_KILLALLNPC_CHECK_NOT();


protected:

	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool UIP2PEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 09.03.30 태완
	virtual void SetShowCommonBG(){};

	//방 정보 변경 관련
	bool Handler_EGS_LEAVE_ROOM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_LEAVE_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_GAME_LOADING_TIME_OUT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_STATE_CHANGE_RESULT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_STATE_CHANGE_RESULT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	bool GameLoadingReq( int percent );
	bool GameLoadingAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool GameLoadingNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool GameLoadingAllUnitOkNot();

	bool PlayStartNot();

	bool BroadCastDungeonRoomSlotInfoNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_END_GAME_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool Handler_EGS_END_GAME_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	virtual bool StateChangeFieldReq();
	virtual bool StateChangeLocalMapReq();
	bool StateChangeLocalMapAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	bool StateChangeResultReq();
	bool StateChangeResultAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool StateChangeResultNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool LoadNextStageOrSubStageReq( string nextStageID = "" );
	bool LoadNextStageOrSubStageAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool LoadNextStageOrSubStageNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	bool StageLoadingReq();
	bool StageLoadingAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool StageLoadingCompleteAllUnitNot();

	bool NPCUnitCreateAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool NPCUnitCreateNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//bool NPCUnitDieReq();
	bool NPCUnitDieAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool NPCUnitDieNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool DungeonStageLoadAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool DungeonStageLoadNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool DungeonStageLoadCompleteReq( bool bIsSecretStage );
	bool DungeonStageLoadCompleteAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool DungeonStageLoadCompleteNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool DungeonStageStartNot( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DUNGEON_SUB_STAGE_OPEN_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DUNGEON_SUB_STAGE_OPEN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DUNGEON_SUB_STAGE_START_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_REMAINING_PLAY_TIME_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_MY_USER_UNIT_INFO_TO_SERVER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_END_GAME_DUNGEON_RESULT_DATA_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_UPDATE_UNIT_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//bool Handler_EGS_NOTIFY_MSG_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



	bool Handler_EGS_CHANGE_PITIN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	bool Handler_EGS_DUNGEON_EFFECT_REMAINING_TIME_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_DUNGEON_EFFECT_TIME_OUT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_PLAY_TIME_OUT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//{{ kimhc // 2011-08-08 // 투니 랜드 채널링 이벤트
	//}} kimhc // 2011-08-08 // 투니 랜드 채널링 이벤트

	bool	Handler_EGS_BAD_ATTITUDE_USER_MSG_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DUNGEON_SUB_STAGE_CLEAR_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_CATCH_HACKUSER_INFO_NOT( KEGS_CATCH_HACKUSER_INFO_NOT& kEGS_CATCH_HACKUSER_INFO_NOT );

	void LoadUI();
	void CreateGame();
	void GameLoadingStart();


	virtual bool QuitGame();


	// tutorial 관련
	void ClearTutorialInputTreeParticle();
	HRESULT TutorialFrameMove( double fTime, float fElapsedTime );

	void TextOutSkillList( const char unitStateID );
	void CreateSkillSuccessEffect() { m_bCreateCommandSuccessSplash = true; }

	virtual bool ShortCutKeyProcess();

	virtual void ProcessShowMiniMap( const bool bHide_ );



    void					MoveToRubenVillage( float fElapsedTime );
	void					DrawMovingSmallBar();
	void					CreateMovingSmallBar();
	void					DestroyMovingSmallBar();
	void					DrawFace( const float fX_, const float fY_,
									const CKTDGUIControl::UITextureData& texData_,
									D3DCOLOR color_ /* = 0xffffffff */, const float fWidthPercent_ = 1.0f );


	/// 어둠의 문 입장 처리 여부
	bool GetEnterDefenceDungeon() const { return m_bEnterDefenceDungeon; }
	void SetEnterDefenceDungeon(bool val) { m_bEnterDefenceDungeon = val; }

	bool OnMouseWheel( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetLoadingGageBar( int iLoadingPercent );

private:
	CKTDGUIDialogType					m_pDLGExitMsgBox;
	CKTDGUIDialogType					m_pDLGLoadingState;
	CKTDGUIDialogType					m_pDLGTutorialInputControl;

	CKTDGUIDialogType					m_pDLGGoToFirstDungeon;
	CKTDGUIDialogType					m_pDLGInputUI;
	CKTDGUIDialogType					m_pDLGTutorialExitButton;

	//CKTDGUIDialogType					m_pDLGF1Help;
	CKTDGUIDialogType					m_pDLGF1Help2;

	CKTDGUIDialogType					m_pDLGArcadeFrame;




	CX2DungeonGame*					m_pDungeonGame;
	bool							m_bLoadingOK;
	CX2GameLoadingTip*				m_pCX2GameLoadingTip;

	wstring							m_HostIP;
	int								m_HostPort;


	bool							m_bSubStageLoading;
	int								m_SubStageLoadingRenderCount;

	bool							m_bResultStateReq;
	bool							m_bGameEnd;
	float							m_fGameEndWaitTime;



	CKTDXDeviceTexture*				m_pLoadingOutLineTex;
	CKTDXDeviceTexture*				m_pLoadingBasicTex;


	int								m_LoadingPercent;
	int								m_LoadingRenderCount;
	CX2Game::CONNECT_TYPE			m_ConnectType;

	int								m_NextStageNum;
	int								m_RenderCount;

	CKTDGUIDialogType				m_pDLGLoadingFront;

	// tutorial 관련
	char							m_TutorialBeforeUserStateID;
	vector< CKTDGParticleSystem::CParticleEventSequenceHandle  >	m_vecTutorialInputTreeParticle;
	D3DXVECTOR3						m_vTutorialParticlePos;
	bool							m_bShowSkilList;
	CKTDGFontManager::CUKFont*		m_pFontForSkillList;


	int 							m_HighLightSkillID;
	float							m_fTimeLeftForHighlight;
	D3DXCOLOR						m_coHighlight;
	D3DXCOLOR						m_coHighLightIncrement;
	int								m_iTutorialMsgFlashCount;

	bool							m_bCreateCommandSuccessSplash;

	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hTutorialMissionSplashParticle;

	map< CX2Unit::UNIT_CLASS, map<char, wstring> > m_mapUnitCommandList;


	DungeonMapUI*					m_pDungeonMapUI;

	bool							m_bWasQuestUIMinimized;
	bool							m_bWasMiniMapMinimized;




	bool							m_bSend_KEGS_END_GAME_REQ;

	bool							m_bStateChangeLocalMap;

	bool							m_bSend_KEGS_LEAVE_ROOM_REQ;
	bool							m_bReceive_KEGS_LEAVE_ROOM_ACK;
	bool							m_bReceive_KEGS_UNIT_INFO_UPDATE;

	bool							m_bLeaveRoomAtTutorial;

	DungeonEndingEvent				m_DungeonEndingEvent;
	SecretStageEnteringEvent		m_SecretStageEnteringEvent;
	int								m_iStartSecretStageEnteringEvent;
	CKTDGUIDialogType					m_pDLGArcadeMission;
	float							m_fTimeShowArcadeMission;
	bool							m_bAutoShowOffArcadeMission;


#if X2_DEV_TOOLS
	CreateMonsterListDialog	m_CreateMonsterListDialog;
#endif // CREATE_MONSTER_LIST_TEST

#if X2_DEV_TOOLS
	MonsterStateListDialog m_MonsterStateListDialog;
#endif // MONSTER_STATE_LIST_TEST


	CKTDXCheckElapsedTime				m_TimerWaitingPortal;
	CKTDGUIControl::UITextureData		m_TexDataMovingGageBG;
	CKTDGUIControl::UITextureData		m_TexDataMovingGage;
	CKTDGStateManager::KStateID			m_RenderStateID;	/// 인터페이스 출력을 위한 2D 출력함수

	CKTDGUIDialogType						m_pDLGFaultyPlayerWarning;
	bool									m_bFaultyPlayerWarningDlgExitButtonPressed;	// DLG 가 삭제 되어야 하는가?


	bool								m_bEnterDefenceDungeon;				/// 어둠의 문 입장 처리 여부
	float								m_fWaitDefenceDungeonStartTime;		/// 어둠의 문 시작까지의 대기 시간

	SHORT								m_SumDelta;		/// 마우스 휠 저장값

	vector<DUNGEON_LOADING_DATA>		m_vecDungeonLoadingData; //던전 로딩 데이터. 던전 로딩형태가 게이지바 하나로 변화되며
																 //그 퍼센트 기준점이 제일 로딩이 느린 유저가 되야해서 추가한 구조체 벡터. (서버 작업 없이)

};
