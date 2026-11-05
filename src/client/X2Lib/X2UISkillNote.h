
#pragma once
class CX2UISkillNote
{
public:
	enum UI_SKILL_NOTE_MSG
	{
		USNM_EXIT = 39000,
		USNM_REGISTER_VIEW_SLOT,
		USNM_SLOT_PAGE_LEFT,
		USNM_SLOT_PAGE_RIGHT,
		USNM_REGISTER_OK,
		USNM_REGISTER_CANCEL,
		USNM_PAGE_PREV,
		USNM_PAGE_NEXT,

		USNM_USE_MEMO,						// 메모 사용
		USNM_BUY_MEMO,						// 메모 구입
		USNM_HAS_MEMO_MOUSE_OVER,			// 보유 메모 설명
		USNM_NOT_HAS_MEMO_MOUSE_OVER,		// 미보유 메모 설명
	};

public:

	CX2UISkillNote( CKTDXStage* pNowStage, const WCHAR* pFileName );
	virtual ~CX2UISkillNote(void);

	virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetShow(bool val);
	bool GetShow(){ return m_bShow; }

	bool OpenDialog();

	void SetLayer(X2_DIALOG_LAYER layer);

	void RegisterMemo(UidType iMemoId);
	void SetRegisterMemo(bool bVal) { m_bRegisterMemo = bVal; }
	bool GetRegisterMemo() { return m_bRegisterMemo; }

	void UpdateMemoSlot(int iPage = 0);
	void UpdateMemo(bool bVal, char cPage);

	char GetUpdatePage() { return m_iUpdatePage; }
	bool GetUpdateMemo() { return m_bUpdateMemo; }
	float GetWriteMemoTime() { return m_fWriteMemoTime; }
	void ResetSkillNote();



private:
	void ViewMemo(int iPage);
	void Handler_EGS_REG_SKILL_NOTE_MEMO_REQ(char iPage);
	void CheckRegisterMemo();
	void UpdateMemoEffect();
    wstring ReplaceDesc_(const wchar_t* pwstrOri, const wchar_t* pwstrPattern, const wchar_t* pwstrReplace);


private:
	char					m_iRegisterPage;
	char					m_iCurrentPage;
	char					m_iUpdatePage;
	bool					m_bShow;
	bool					m_bRegisterMemo;
	UidType					m_iRegisterMemo;
	int						m_iViewPage;

	CKTDGUIDialogType		m_pDLGSkillNote;
	CKTDGUIDialogType		m_pDLGQuery;
	CKTDGUIDialogType		m_pDLGInfoBox;
	CKTDGUIDialogType		m_pDLGWrittingNote;
	float					m_fWriteMemoTime;
	bool					m_bUpdateMemo;
	std::vector<wstring>	m_vecSlotLv;

	CKTDGParticleSystem::CParticleEventSequenceHandle m_hParticle1;
	CKTDGParticleSystem::CParticleEventSequenceHandle m_hParticle2;


};
