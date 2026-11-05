#pragma once

class CKTDGUIDialogManager : public CKTDXStage
{
	public:
		CKTDGUIDialogManager(void);
		~CKTDGUIDialogManager(void);

		bool	GetShow() { return m_bShow; }
		void	ToggleShow() { SetShow(!m_bShow);}
		void	SetShow( bool bShow ) { m_bShow = bShow; }

		void	RegisterLuaBind();

        bool	AddDlg( CKTDGUIDialogType pDialog );
		//bool	AddDlg( CKTDGUIDialogHandle hDialog_ );
//{{ robobeg : 2008-01-07
        bool    UnmanageDlg( CKTDGUIDialogType pDialog );
//}} robobeg : 2008-01-07

        bool	DeleteDlg( CKTDGUIDialogType pDialog );
		//{{ seojt // 2009-8-18, 18:00
		//bool	DeleteDlg( CKTDGUIDialogHandle hDialog_ );
		//}} seojt // 2009-8-18, 18:00
//{{ robobeg : 2008-01-07
		//bool	DeleteDlgByName( const WCHAR* pName );
//}} robobeg : 2008-01-07
		void	ClearAllDlg();
		bool	ClearAllOKAndCancelMsgDlg();

//{{ robobeg : 2009-01-07
		//std::vector<CKTDGUIDialogType>& GetDialogList() { return m_vecDialog; }
//}} robobeg : 2009-01-07

		bool	CheckDlg( CKTDGUIDialogType pDialog );
		bool	CheckModalDlg();
		bool	CheckFrontModalDlg( CKTDGUIDialogType pPivotDialog ); //해당 다이얼로그 앞에 모달 다이얼로그가 있는지 확인
		CKTDGUIDialogType GetFirstFrontModalDlg();		//현제 맨앞에서 모달 역할을 하는 다이얼로그 가져오기
		bool	CheckMostFrontDlg( CKTDGUIDialogType pDialog ); //이 다이얼로그가 맨 앞에 보여지는 다이얼로그인지 확인

		void	CountModalDialog( CKTDGUIDialog* pDialog = NULL );


		bool	ChangeLayer( CKTDGUIDialogType pDialog, int layerToChange );
        bool    ChangeFront( CKTDGUIDialogType pDialog, bool bFront ); ///< front, rear 변경
        bool	ChangeSequence( CKTDGUIDialogType pDialog, bool bTop ); //속해있는 레이어에서 가장 앞으로 옮길것이냐 가장 뒤로 옮길것이냐?
		//{{ seojt // 2009-8-18, 18:12, qff
		//bool	ChangeSequence( CKTDGUIDialogHandle hDialog_, bool bTop ); //속해있는 레이어에서 가장 앞으로 옮길것이냐 가장 뒤로 옮길것이냐?
		//}} seojt // 2009-8-18, 18:12
		bool	ChangeSequence( CKTDGUIDialogType pDialog, CKTDGUIDialogType pPivotDialog, bool bOver, bool bChangeLayer = false );

		CKTDGUIDialog* GetDialog_LUA( const char* pName );
		CKTDGUIDialogType GetDialog( const WCHAR* pName );

		bool ExistDialogAtMousePos();


		virtual	HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual	HRESULT OnFrameRender( bool bFront = false );

		virtual HRESULT OnFrameRenderTopModal();

		virtual	bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual	HRESULT OnResetDevice();
		virtual	HRESULT OnLostDevice();


		void	SetUKFont( int uiUKFontID, CKTDGFontManager::CUKFont* pFont );
		CKTDGFontManager::CUKFont* GetUKFont( int fontID ){ return m_mapUKFontList[fontID]; }

		void    MapFontToUKFont( int fontID, int uiUKFontID )
		{
			if ( m_mapUKFontList.find( uiUKFontID ) != m_mapUKFontList.end() )
			{
				m_mapFontToUKFont[ fontID ] = uiUKFontID;
			}//if
		}

		CKTDGFontManager::CUKFont* GetMappedUKFont( int fontID )
		{
			std::map< int, int >::const_iterator iter = m_mapFontToUKFont.find( fontID );
			if ( iter == m_mapFontToUKFont.end() )
				return  NULL;
			return  GetUKFont( iter->second );
		}

//{{ robobeg : 2009-01-07
		//CKTDGUIDialogType GetDialogByKeyID( int iKeyID );
//}} robobeg : 2009-01-07

		bool IsDraggingDialog();

		CKTDGUIDialogType GetCloseDialogByESC();

		void SetStage( CKTDXStage* pStage );

		bool GetFocusInEditBox();	// imeedit or editbox에 포커스가 있는지 검사한다.

		bool GetHideDialog();
		void SetCanHide(bool bVal) { m_bCanHide = bVal; }

private:

        void    _InsertIntoDialogListTop( CKTDGUIDialogType pkInDialog_ );
        void    _InsertIntoDialogListBottom( CKTDGUIDialogType pkInDialog_ );
        bool    _RemoveFromDialogList( CKTDGUIDialogType pkInDialog_ );

		inline void	IncreaseDialogListChangeCount()
		{
			m_iDialogListChangeCount += 1;
			if( m_iDialogListChangeCount > INT_MAX - 10 )
			{
				m_iDialogListChangeCount = 1;
			}
		}

private:

		bool										m_bShow;

//{{ robobeg : 2009-01-07
		CKTDGUIDialogMap							m_mapDialog;			// dialog name으로 dialog handle을 검색할 때 사용
		CKTDGUIDialogSet							m_setDialog;			// dialog handle로 dialog가 있는지 없는지 검사할 때 사용
        CKTDGUIDialogList                           m_listDialog;			// dialog를 순차적으로 traverse할 때 사용
        std::vector<CKTDGUIDialogList::iterator>    m_vecIterator;
        std::vector<CKTDGUIDialogList::iterator>    m_vecReverseIterator;
//}} robobeg : 2009-01-07



		map< int, CKTDGFontManager::CUKFont* >		m_mapUKFontList;

		std::map< int, int >                        m_mapFontToUKFont;


		int m_iModalDialogCount;		// 현재 dialog list에 포함 되어있는 modal dialog 개수
		int m_iDialogListChangeCount;	// dialog가 dialog list에 추가될때, list가 변경된 회수를 저장. dialog list에 추가된 이후에 dialog list가 변동이 있었는지 확인하기 위해 사용.
		std::map< std::wstring, int > m_mapDialogListChangeCountWhenLastGetDialog; // < name, dialog_list_change_count> // 마지막으로 GetDialog(name) 함수를 호출했을때 dialog list change count

		bool m_bCanHide;
};
