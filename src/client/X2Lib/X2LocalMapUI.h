#pragma once




class CX2LocalMapUI : public CKTDXDeviceHolder
{
public:
	enum LOCAL_MAP_UI_CUSTOM_MSG
	{
		LMUCM_DUNGEON_CLICKED = 18000,
		LMUCM_DUNGEON_GATE_SELECTED,
		LMUCM_CLOSE_WINDOW,
		LMUCM_DUNGEON_MOUSE_OVER,
		LMUCM_DUNGEON_MOUSE_OUT,
	};

public:
	CX2LocalMapUI(void);
	virtual ~CX2LocalMapUI(void);

	virtual void OnFrameMove( double fTime, float fElapsedTime );
	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool GetShowLocalMap() { return m_bShowLocalMap; }
	CX2LocationManager::LOCAL_MAP_ID GetPickedLocalMapID() const { return m_ePickedLocalMapID; }
	void ResetLocalMapID(){m_ePickedLocalMapID = CX2LocationManager::LMI_INVALID;}

private:
	CKTDXStage*			m_pStage;

	CX2LocationManager::LOCAL_MAP_ID							m_ePickedLocalMapID;
	map< CX2LocationManager::LOCAL_MAP_ID, CKTDGUIDialogType >		m_mapLocalDLG;

	bool m_bShowLocalMap;

	CKTDGUIDialogType		m_pDLGQuestDesc;
	map< int, wstring >		m_mapLocalMapQuestDesc;

	CKTDGUIDialogType		m_pDlgHeroRecruit;
public:
	void SetStage( CKTDXStage* pStage );

	void OpenLocalMapDLG( bool bOpen, CX2LocationManager::LOCAL_MAP_ID eLocalMapID = CX2LocationManager::LMI_INVALID );
	void UpdateLocalMapDLG();
	void ClearLocalMapDLGs();
	CKTDGUIDialogType GetLocalMapDLG( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );
	CKTDGUIDialogType CreateLocalMapDLG( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );

	void UpdateHeroRecruitButton();

	void UpdateDungeonButtons( CX2LocationManager::LOCAL_MAP_ID eLocalMapID );

	void CreateDungeonClearStars( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID );
	void UpdateDungeonClearStars( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID, bool bShow );

	void DropDownDungeonButton( CKTDGUIControl* pControl );
	void CreateNewDungeonParticle( CKTDGUIControl* pControl, bool bEventDungeon = false );
	//{{ oasis907 : 김상윤 [2010.7.1] // 에픽 퀘스트
	void CreateLocalQuestPicture( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID );
	void DrawLocalMapQuestDesc(D3DXVECTOR2 DrawPos, int iDungeonID);
	void UdateLocalMapQuestDesc( CX2LocationManager::LOCAL_MAP_ID eLocalMapID, int dungeonID, bool bShow );
	//}}

	//{{ kimhc // 2011-04-27 // 디펜스 던전 시간에 따라, 던전 아이콘 비/활성화 여부 결정
	bool UpdateDefenceDungeonIcon( CKTDGUIControl* pControlDefenceDungeon_, const bool bActiveDefenceDungeon_ );
	//}} kimhc // 2011-04-27 // 디펜스 던전 시간에 따라, 던전 아이콘 비/활성화 여부 결정
	void GetHenirDungeonIcnoDesc( OUT wstringstream& wstrStreamDungeonIconDesc_ );


	void CloseOtherUI();
};
