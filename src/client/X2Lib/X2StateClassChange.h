#pragma once

//class CX2StateClassChange : public CX2StateCommonBG
//{
//	public:
//
//		enum STATE_CLASS_CHANGE_UI_CUSTOM_MSG
//		{
//			SCCUCM_STATE_CHANGE_VILLAGE = 0,
//		};
//
//	public:
//
//		CX2StateClassChange(void);
//		virtual ~CX2StateClassChange(void);
//
//		HRESULT OnFrameMove( double fTime, float fElapsedTime );
//
//	protected:
//		bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//		bool LastMsgByESC();
//		void LastMsgPopOk();
//
//	private:
//		CX2UnitViewerUI*	m_pUnitViewerUI;
//		CKTDGUIDialogType	m_pDLGClassChange;
//};



class CX2ClassChangePopup
{
public:
	enum CLASS_CHANGE_UI_CUSTOM_MSG
	{
		CCUCM_STATE_CHANGE_VILLAGE = 35200,
	};

	enum HELP_POPUP_TYPE
	{
		HPT_SHARE_BANK = 1,
	};

public:
	CX2ClassChangePopup(void);
	virtual ~CX2ClassChangePopup(void);

	HRESULT OnFrameMove( double fTime, float fElapsedTime );

	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetShow( bool bShow );
	bool SetShow( bool bShow, int iType );
	bool GetShow() { return m_bShow; }

private:
	bool m_bShow;
	CX2UnitViewerUI*	m_pUnitViewerUI;
	CKTDGUIDialogType	m_pDLGClassChange;

	void PlaySoundChangeClass( CX2Unit::UNIT_TYPE eUnitType );
};
