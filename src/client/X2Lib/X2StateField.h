#pragma once

using namespace std;

class CX2StateField : public CX2StateMenu
{
public:
	friend class CX2StateAutoChanger;

	struct DRAWFACE_RHW_VERTEX
	{
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

public:
	enum FIELD_QUERY
	{
		FQ_DUNGEON_ACCEPT = 15200,
		FQ_DUNGEON_CANCEL,
		FQ_PARTY_JOIN_REQ,
		FQ_PARTY_JOIN_CANCEL,
	};

	enum GUIDE_UI_CUSTOM_MESSAGE
	{
		GUI_EPIC_EXIT	= 42001,
		GUI_ITEM_EXIT,
	};

	CX2StateField(void);
	virtual ~CX2StateField(void);

	virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
	virtual HRESULT OnFrameRender();

	virtual HRESULT OnResetDevice();
	virtual HRESULT OnLostDevice();

	bool OnMouseWheel( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void PopTalkBox( UidType iUnitUID_, const WCHAR* pWstrMsg_,
		const D3DXCOLOR& coTextColor_, const D3DXCOLOR& coOutColor_, const D3DXCOLOR& coBackColor_ );

private:
	void SetVillageName();
	void ResourcePreLoad();

protected:
	//virtual bool ClickMenuBackButton(void) { return false; }

	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetShowStateDLG(bool);
	void SetShowCommonBG();

	bool GetNowMovingToBattleField() const { return m_bNowMovingToBattleField; }
	void SetNowMovingToBattleField( const bool bNowMovingToBattleField_ ) { m_bNowMovingToBattleField = bNowMovingToBattleField_; }

	bool MoveFromVillageToBattleField( const int iVillageStartPosIdToBattleField_ ) const;
    bool MoveToOtherPlace( float fElapsedTime );
	void DrawMovingSmallBar();
	void CreateMovingSmallBar();
	void DrawFace( const float fX_, const float fY_, const CKTDGUIControl::UITextureData& texData_, D3DCOLOR color_ /* = 0xffffffff */, const float fWidthPercent_ = 1.0f );

	virtual void UserAndPetListPopUpMenuProcess();

public:

	void JoinSector( KFieldUserInfo *pKFieldUserInfo);
	void LeaveSector( UidType iUnitUID );


	bool Handler_EGS_STATE_CHANGE_FIELD_REQ();
	bool Handler_EGS_STATE_CHANGE_FIELD_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_FIELD_LOADING_COMPLETE_REQ();
	bool Handler_EGS_FIELD_LOADING_COMPLETE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//bool Handler_EGS_LEAVE_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // 섹터에서 나간유저가 누군지 알려줌
	//bool Handler_EGS_JOIN_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // 섹터에서 들어온유저가 누군지 알려줌

	//bool Handler_EGS_DEL_USER_LIST_IN_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // 섹터 이동한 이후 렌더링 안해도 되는 캐릭터 리스트
	//bool Handler_EGS_NEW_USER_LIST_IN_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // 섹터 이동한 이후 추가로 렌더링 해야되는 캐릭터 리스트
	bool Handler_EGS_CHANGE_USER_LIST_IN_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 1초 싱크받음
	bool Handler_EGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_CHANGE_EQUIPPED_ITEM_IN_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 던전으로 바로 이동
	void Handler_EGS_CREATE_TUTORIAL_ROOM_REQ(int iDungeonId);
	bool Handler_EGS_CREATE_TUTORIAL_ROOM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 거래광장 진입
	bool Handler_EGS_JOIN_SQUARE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	// 파티정보 (방장 파티박스)
	//bool Handler_EGS_PARTY_TALKBOX_INFO_UPDATE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	// 던전 라운지에서 파티원 이탈 처리
	bool Handler_EGS_LEAVE_ROOM_REQ();
	bool Handler_EGS_LEAVE_ROOM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_LEAVE_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 필드내 유저들의 파티정보
	bool Handler_EGS_UPDATE_PARTY_UID_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_TOGGLE_INVISIBLE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



	virtual bool Handler_EGS_CHAR_LEVEL_UP_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



	/// mauntain // 2013-05-03 // 결혼 시스템 배우자 소환 기능
	virtual bool Handler_EGS_CALL_MY_LOVER_STATE_CHANGE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	CKTDGUIDialogType		m_pDLGJoinParty;
	CKTDGUIDialogType		m_pDLGExitMsgBox;
	CKTDGUIDialogType		m_pDLGSquareTitle;

	D3DXVECTOR3			m_vOldEyePt;
	D3DXVECTOR3			m_vOldLookAtPt;

	bool				m_bNowMovingToBattleField;	/// 현재 필드로 이동 중이라면

	float				m_fLoadCompleteTimer;
	bool				m_bSendLoadCompleteReq;
	bool				m_bLoadCompleteReq;

	UidType				m_TalkBoxunitUIDToFind;	// 삭제
	UidType				m_TalkBoxunitUID;

	int					m_iTalkNpcIndex;

	CKTDXCheckElapsedTime				m_TimerWaitingPortal;			// 이동 대기 시간 계산
	CKTDGUIControl::UITextureData		m_TexDataMovingGageBG;
	CKTDGUIControl::UITextureData		m_TexDataMovingGage;
	CKTDGStateManager::KStateID			m_RenderStateID;	/// 인터페이스 출력을 위한 2D 출력함수



	SHORT						m_SumDelta;		/// 마우스 휠 저장값
};
