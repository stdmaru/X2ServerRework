
#pragma once

class CX2UIManager
{
public:
// Comment : 08.12.12 김태완
// 	UIManager에 의해 관리되는 UI들의 제작시 제약 사항
//	void UIClosed(UI_MENU menuInx);
// 	void UIOpened(UI_MENU menuInx);
// 	실제로 UI가 열리고 닫히는 시점에 UIOpened 또는 UIClosed를 호출해 줘야 합니다.
// 	UI 레이어 설정 함수가 있어야 합니다. ( SortUIByIndex 내부에서 호출하게 연결 )
// 	왼쪽 위에 위치해서 위치가 조절되어야 하는 UI들은 위치 조절 함수가 구현되어야 합니다. ( SetDLGPosition에서 호출 )
//
// 	UI 추가시 주의해야 하는 점
// 	-새 UI를 enum CX2UIManager::UI_MENU에 추가 (순서 조심할 것)
// 	- UIManager의 msgproc, uicustomeventproc, uiservereventproc에 추가
// 	- ToggleUI, SortUIbyIndex, GetShow에 추가
// 	-  왼쪽 위에 위치해서 우선순위에 따라 위치가 조절되어야 하는 UI들은
// 	- GetDLGSize, SetDLGPosition에 추가
// 내가 왜 그냥 UI 클래스 하나 만들지 않았을까 나는 한없이 어리석었도다 ...


	// enum의 순서를 메뉴 우선순위 순으로 정렬
    enum UI_MENU
    {
		UI_MENU_HIGHEST = 0,

		UI_MENU_QUICK_SLOT,
		UI_SKILL_SLOT,

		UI_MENU_UPPER,

		UI_MENU_PERSONAL_TRADE,
		//UI_MENU_PARTY_LIST,
		//UI_MENU_LOCAL_MAP,
		UI_MENU_QUEST_RECEIVE,
		UI_MENU_SHOP,
		UI_MENU_MANUFACTURE,

		UI_MENU_POSTBOX_NOTICE,

		// oasis907 : 김상윤 [2010.3.12] //  개인 상점 검색 시스템
		UI_MENU_PERSONAL_SHOP_BOARD,
		// oasis907 : 김상윤 [2009.11.24] // 길드 게시판
		UI_MENU_GUILD_BOARD,

		//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
		UI_MENU_RANKING_INFO,
		//}} kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업

		//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
		UI_MENU_ITEM_EXCHANGE_SHOP,
		//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

		//{{ kimhc // 2009-08-04 // 캐릭터별 은행
		UI_MENU_PRIVATE_BANK,
		//}} kimhc // 2009-08-04 // 캐릭터별 은행

		UI_MENU_PERSONAL_SHOP,
        UI_MENU_CHARINFO,
		UI_MENU_PET_LIST,
		UI_MENU_SKILL,
		UI_MENU_COMBO_TREE,
		UI_MENU_QUEST_NEW,
        UI_MENU_QUEST,
		UI_MENU_WATCH,

		UI_MENU_LU,	// 왼쪽 위에 떠야 하는 하위창들(파티, 캐릭터..)등은 이것보다 위로 그 외는 아래로.
					// UI_MENU_UPPER ~ UI_MENU_LU 사이에 있는 UI들은 열고 닫힐 때 위치가 정렬된다.
					// UI_MENU_LU 아래쪽에 있는 UI들은 위치정렬은 되지 않는다. (상하관계 정렬은 된다)

		//{{ kimhc // 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리

		UI_TEMP_INVENTORY,		// enum.lua에는 없음(DLG_Item_Box.lua)
								// MENU라 하기에 애매해서 MENU 단어를 앞에 붙이지 않았음
		//}} kimhc // 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리

		UI_SKILL_NOTE,

		UI_MENU_INVEN,
        UI_MENU_COMMUNITY,
        UI_MENU_OPTION,

		UI_MENU_LOWEST,

		UI_MENU_PARTY,
		UI_MENU_GUILD,	//길드창과 커뮤니티창을 분리할 예정이므로 버튼 먼저 분리

		//{{ 2011.03.16   임규수 아바타 분해 시스템
		//}}
		//{{ 2011.05.04   임규수 아바타 합성 시스템
		//}}

		UI_MENU_RELATION,


		UI_MENU_HERO,


		UI_RIDING_SKILL_SLOT,

        UI_MENU_END,
    };


public:
    CX2UIManager();
	void ResetUIManager();
    virtual ~CX2UIManager();

    virtual bool    UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
    virtual HRESULT OnFrameRender();

	void OnStateChange();
	void OnFieldChange();

	bool IsOpenedWindow();
	bool GetShow(UI_MENU menuInx);
	bool ToggleUI(CX2UIManager::UI_MENU menuInx, UidType arg = 0);
    bool ToggleUI(CX2UIManager::UI_MENU menuInx, bool forOpen, UidType arg = 0);
	void CloseAll();
	void CloseAllLUDlg();		// LU : 왼쪽 위
	void CloseAllNPCDlg();
	//bool CloseByEsc();			// 위에서부터 하나씩 닫는다.  창이 하나라도 닫혔으면 true, 더 닫힐 창이 없는 상태면 false
	bool GetShowNpcShop();

	bool GetDLGFlag(){return m_bFlag;}
	void SetDLGFlag(bool val){m_bFlag = val;}


	//bool IsStateMenu();

	// 특정 UI가 열렸다/닫혔다를 신호해 줘서 위치관계를 재정렬하게 한다.
	// UIManager를 거치지 않고 열고 닫는 경우(REQ/ACK 등 단순히 ToggleUI->SetShow path를 사용하지 못하는 경우) 에 활용하자.
	void UIClosed(UI_MENU menuInx);
	void UIOpened(UI_MENU menuInx);

	// Toggle이 동작하지 않게 해 주는 Method!
	bool ToggleEnableCondition(UI_MENU menuInx, bool forOpen);

	// UI 정렬을 위한 내부 method
	void SetUILayerIndex(UI_MENU menuInx, bool bAdd /* true면 추가, false면 삭제 */ );
    void SortUIbyLayerIndex();

	// 특정상황에서 특정 UI를 diasble 시킨다 : 파티창 작동 안하게 하려고
	void DisableDialogOnCondition();


	void CreateUICharInfo();
	CX2UICharInfo* GetUICharInfo();

	void CreateUIInventory();
	CX2UIInventory* GetUIInventory();

	void CreateUIPersonalShop();
	CX2UIPersonalShop* GetUIPersonalShop();

	void DestroyPeronalShopUI();

	void CreateUIPersonalTrade();
	CX2UIPersonalTrade* GetUIPersonalTrade();

	void CreateUIShop();
	CX2UIShop* GetUIShop();

	void CreateUIManufacture();
	CX2UIManufacture* GetUIManufacture();

	void CreateUIQuickSlot();
	CX2UIQuickSlot* GetUIQuickSlot();

	void CreateUIQuestNew();
	CX2UIQuestNew* GetUIQuestNew();
	void CreateUIWatch();
	CX2UIWatch*	GetUIWatch();

	void CreatePetList();
	CX2UIPetInfo* GetUIPetList();

	void CreateUISkillTree();

		CX2UISkillTreeNew* GetUISkillTree();


	void CreateUIComboTree();
	CX2UIComboTree* GetUIComboTree();

	void CreateUIHeroMatch();
	CX2UIHeroMatch* GetUIHeroMatch();

	//{{ kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리
	void CreateUITempInventory();
	CX2UITempInventory* GetUITempInventory() const;
	void DestoryUITempInventory();
	//}} kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리


	// oasis907 : 김상윤 [2010.3.12] // 개인 거래 상점 검색 시스템
	void CreateUIPostBoxWarning();
	CKTDGUIDialog* GetUIPostBoxWarning() const;

	// oasis907 : 김상윤 [2010.3.12] // 개인 거래 상점 검색 시스템
	void CreateUIPersonalShopBoard();
	CX2UIPersonalShopBoard* GetUIPersonalShopBoard() const;
	// oasis907 : 김상윤 [2009.11.24] // 길드 게시판
	void CreateUIGuildBoard();
	CX2UIGuildBoard* GetUIGuildBoard() const;

	//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
	void CreateUIRankingInfo();
	CX2UIRankingInfo* GetUIRankingInfo() const;
		//}} kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업

	//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
	void						CreateUIItemExchangeShop();
	CX2UIItemExchangeShop*		GetUIItemExchangeShop() const;
		//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

	//{{ kimhc // 2009-08-04 // 캐릭터별 은행
	void						CreateUIPrivateBank();
	CX2UIPrivateBank*			GetUIPrivateBank() const;
		//}} kimhc // 2009-08-04 // 캐릭터별 은행

	void						CreateUISkillNote();
	CX2UISkillNote*				GetUISkillNote() const;

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}
	bool PetInventoryOpenCondition();
	bool PetInventoryCloseCondition(UI_MENU menuInx);
	bool PetInventoryForceOpenCondition();
	void PetInventoryForceCloseOtherDlg();

	void SetShowUI(bool bVal, bool bForce = false);//, bool AllowReset = true);
	bool GetShowUI() const {return m_bShowUI;}

	void Handler_EGS_WATCH_UNIT_REQ(UidType uid);
	bool Handler_EGS_WATCH_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );




	void OpenWatchUIByOtherServerGroupUnitUid(UidType UnitUid_);
	bool IsSameServerGroupByUnitUid( UidType UnitUid_ );

	//스킬슬롯, 퀵슬롯, 메뉴버튼을 각각 따로 관리하기 위해 추가
	void SetShowSkillSlot(bool val);
	bool GetShowSkillslot() const { return m_bShowSkillSlot; }

	void SetShowQuickSlot(bool val) { m_bShowQuickSlot = val;}
	bool GetShowQuickSlot() const { return m_bShowQuickSlot; }

	void SetShowRidingPetSkillSlot(bool val);
	bool GetShowRidingPetSkillslot() const { return m_bShowRidingPetSkillSlot; }


	CX2UIPersonalShop* GetPersonalShop() { return m_pPersonalShop; }
private:
	//void SetPosLUDialog(UI_MENU openDLG, bool forOpen);
	void SetPosLUDialogOnOpen(UI_MENU openDLG);
	void SetPosLUDialogOnClose(UI_MENU closeDLG);
	D3DXVECTOR2 GetDLGSize(UI_MENU DLG);
	void SetDLGPosition(UI_MENU DLG, D3DXVECTOR2 vec);
	void SetNowDragItemReturn();

	void SetShowMinimapUI(bool bVal);
	void SetShowGageUI(bool bVal);
	void SetShowQucikQuest(bool bVal);
	void SetShowPlayGuide(bool bVal);
	void SetShowFeverUI(bool bVal);
	void SetShowCinematicUI(bool bVal, float fTime_ = 0.0f );
	void SetShowResurrectionStone(bool bVal);
	void SetShowMenu(bool bVal);

	public :
		void SetShowSkillDesc(bool bVal);

	private :
	void SetShowPartyMenu(bool bVal);
	void SetShowMonsterIndicator(bool bVal);
	bool					m_bFlag;

	CX2UICharInfo*			m_pCharInfo;
	CX2UIInventory*			m_pInventory;
	CX2UIWatch*				m_pWatch;
	CX2UIPersonalShop*		m_pPersonalShop;
	CX2UIPersonalTrade*		m_pPersonalTrade;
	CX2UIShop*				m_pShop;
	CX2UIManufacture*		m_pManufacture;
	CX2UIQuickSlot*			m_pQuickSlot;

	CX2UIQuestNew*			m_pQuestNew;

	CX2UIPetInfo*			m_pPetList;


		CX2UISkillTreeNew*		m_pSkillTree;


	CX2UIComboTree*			m_pComboTreeUI;

	//{{ kimhc // 실시간 엘소드 중 실시간 아이템 획득 관련
	CX2UITempInventory*		m_pTempInventory;	// 던전 내에서 인벤토리가 가득 찼을 경우 생기는 임시 공간
	//}} kimhc // 실시간 엘소드 중 실시간 아이템 획득 관련

	CKTDGUIDialog* m_pPostBoxWarning;
	CX2UIPersonalShopBoard* m_pPersonalShopBoard;
	// oasis907 : 김상윤 [2009.11.24] // 길드 게시판
	CX2UIGuildBoard*		m_pGuildBoard;

	//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
	CX2UIRankingInfo*		m_pRankingInfo;		// 게시판, BILLBOARD(헤니르 시공, 대전, 레벨 랭킹)
	//}} kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업

	//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
	CX2UIItemExchangeShop*	m_pItemExchangeShop;	// 아이템 교환 (헤니르 시공 업데이트)
	//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

	//{{ kimhc // 2009-08-04 // 캐릭터별 은행
	CX2UIPrivateBank*		m_pPrivateBank;			// 개인 은행
		//}} kimhc // 2009-08-04 // 캐릭터별 은행

	CX2UISkillNote*			m_pSkillNote;

	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}

	CX2UIHeroMatch*			m_pHeroMatch;


	CX2UIManager::UI_MENU				m_LUMenu1st;
	CX2UIManager::UI_MENU				m_LUMenu2nd;

	vector<CX2UIManager::UI_MENU>		m_vLayer;
	bool								m_bShowUI;


	//스킬슬롯, 퀵슬롯, 메뉴버튼을 각각 따로 관리하기 위해 추가
	bool								m_bShowSkillSlot;
	bool								m_bShowQuickSlot;

	bool								m_bShowRidingPetSkillSlot;

};
