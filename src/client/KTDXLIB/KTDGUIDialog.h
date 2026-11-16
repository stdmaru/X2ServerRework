#pragma once
//{{ seojt // 2009-8-19, 15:42
#include "KTDGUIDialogHandle.h"
//}} seojt // 2009-8-19, 15:42


class   CKTDGUIDialogManager;
class   CKTDGUIDialog;
class   CKTDGUIControl;
typedef std::list<CKTDGUIDialogType>   CKTDGUIDialogList; // moved to end of this file by jintaeks on 2009-08-19, 15:20
typedef std::set<CKTDGUIDialogType>    CKTDGUIDialogSet; // moved to end of this file by jintaeks on 2009-08-19, 15:20
typedef std::map<std::wstring, CKTDGUIDialogType>    CKTDGUIDialogMap; // moved to end of this file by jintaeks on 2009-08-19, 15:20



//class   CKTDGUIDialog : private x2::NonCopyable
//{{ seojt // 2009-8-18, 17:53, qff
class CKTDGUIDialog
    : private x2::NonCopyable
    , public KJohnObjMan<CKTDGUIDialog>
//}} seojt // 2009-8-18, 17:53
{
	public:
		struct DialogMoveItem
		{
			D3DXVECTOR2 pos;
			D3DXCOLOR	color;
			float		fChangeTime;
			bool		bIsAutoDelete;

			DialogMoveItem()
			{
				INIT_VECTOR2(pos, 0, 0);
				color = D3DXCOLOR(0xffffffff);
				fChangeTime = 0.0f;
				bIsAutoDelete = false;
			}

			DialogMoveItem( const DialogMoveItem& ) = default;
			DialogMoveItem& operator=(const DialogMoveItem& rhs)
			{
				if ( this == &rhs )
				{
					return *this;
				}

				pos = rhs.pos;
				color = rhs.color;
				fChangeTime = rhs.fChangeTime;
				bIsAutoDelete = rhs.bIsAutoDelete;

				return *this;
			}
		};


	public:
		CKTDGUIDialog( CKTDXStage* pStage, const WCHAR* wstrScriptFileName = NULL, float fMoveTime = 0.07f, int iLayer = 0 );
		virtual	~CKTDGUIDialog(void);

        /** UI dialog를 handle로 사용하도록 하는 작업을 진행하면서,
            operator new의 overloading에 문제가 있어서 서진택이 주석처리하다.
            - jintaeks on 2009-08-20, 11:03 */
		//void* operator new(size_t s);
		//void operator delete(void *p);

		bool	OpenScriptFile( const WCHAR* pFilename );

		HRESULT	OnFrameMove( double fTime, float fElapsedTime );
		HRESULT	OnFrameRender( bool bForceDraw = false );
		HRESULT	OnResetDevice();
		HRESULT	OnLostDevice();
		bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		CKTDGUIControl::CPictureData* CreatePicture()
		{
			CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
			return pPicture;
		}

// #ifdef MOVIE_TEST	 	// 13-11-11, 진입 구조 개편, kimjh, MOVIE_TEST 중 사용에 필요한 Define 을 MOVIE_TEST_BASE 로 변경
		CKTDGUIControl::CMovieData* CreateMovie()
		{
			CKTDGUIControl::CMovieData* pMovieData = new CKTDGUIControl::CMovieData();
			return pMovieData;
		}
// #endif //  MOVIE_TEST	// 13-11-11, 진입 구조 개편, kimjh, MOVIE_TEST 중 사용에 필요한 Define 을 MOVIE_TEST_BASE 로 변경





		CKTDGUIButton*		CreateButton()		{ CKTDGUIButton*		pControl; pControl = new CKTDGUIButton();		return pControl; }
		CKTDGUICheckBox*	CreateCheckBox()	{ CKTDGUICheckBox*		pControl; pControl = new CKTDGUICheckBox();		return pControl; }
		CKTDGUIEditBox*		CreateEditBox()		{ CKTDGUIEditBox*		pControl; pControl = new CKTDGUIEditBox();		return pControl; }
		CKTDGUIIMEEditBox*	CreateIMEEditBox()	{ CKTDGUIIMEEditBox*	pControl; pControl = new CKTDGUIIMEEditBox();	return pControl; }
		CKTDGUIStatic*		CreateStatic()		{ CKTDGUIStatic*		pControl; pControl = new CKTDGUIStatic();		return pControl; }
		CKTDGUIRadioButton*	CreateRadioButton()	{ CKTDGUIRadioButton*	pControl; pControl = new CKTDGUIRadioButton();	return pControl; }
		CKTDGUIScrollBar*	CreateScrollBar()	{ CKTDGUIScrollBar*		pControl; pControl = new CKTDGUIScrollBar();	return pControl; }
		CKTDGUIComboBox*	CreateComboBox()	{ CKTDGUIComboBox*		pControl; pControl = new CKTDGUIComboBox();		return pControl; }
		CKTDGUIListBox*		CreateListBox()		{ CKTDGUIListBox*		pControl; pControl = new CKTDGUIListBox();		return pControl; }
		CKTDGUISlider*		CreateSlider()		{ CKTDGUISlider*		pControl; pControl = new CKTDGUISlider();		return pControl; }
		CKTDGUINavigation*	CreateNavigation()	{ CKTDGUINavigation*	pControl; pControl = new CKTDGUINavigation();	return pControl; }
		CKTDGUIContolList*	CreateControlList() { CKTDGUIContolList*	pControl; pControl = new CKTDGUIContolList();	return pControl; }
		CKTDGUISlot*		CreateSlot()		{ CKTDGUISlot*			pControl; pControl = new CKTDGUISlot();			return pControl; }

		CKTDGUIItem*		CreateItem()		{ CKTDGUIItem*	pControl; pControl = new CKTDGUIItem(); return pControl; }

		void			AddControl( CKTDGUIControl* pControl );
		void			AddControl_LUA( CKTDGUIControl* pControl );	// scripts' AddControl: noted for the UI editor
#ifdef DLL_BUILD
		void			AddControl_Front( CKTDGUIControl* pControl );
#endif
		bool			DeleteControl( const WCHAR* pControlName );

		bool			ChangeSequence( CKTDGUIControl* pControlToChange, bool bFront ); // 가장 앞으로 옮길것이냐 가장 뒤로 옮길것이냐?
		void			ChangeSequenceByName_LUA( const char* pControlName, bool bFront );
		void			ChangeSequence( CKTDGUIControl* pControlToChange, CKTDGUIControl* pPivotControl, bool bFront );
		void			ChangeSequenceByNameAndPivot_LUA( const char* pControlNameToChange, const char* pPivotControlName, bool bFront );

		bool			CheckControl( CKTDGUIControl* pControl );
		bool			CheckControl( const WCHAR* wszControlName );

		//CKTDGUIControl*	GetControl_LUA( const char* pControlName );

		const CKTDGUIControl*   _GetControl( const WCHAR* pControlName ) const;
		CKTDGUIControl*	        GetControl( const WCHAR* pControlName );
		CKTDGUIControl*	        GetControl( const int index );
		//{{ seojt // 2009-8-19, 17:17
		const CKTDGUIControl*   GetControl( const WCHAR* pControlName ) const;
		const CKTDGUIControl*   GetControl( const int index ) const;
		//}} seojt // 2009-8-19, 17:17

		int				GetControlSize() { return (int)m_ControlsList.size(); }

		vector<CKTDGUIControl*>& GetControlList() { return m_ControlsList; }

		bool			ReNameControl( CKTDGUIControl* pControl );

		void					RequestFocus( CKTDGUIControl* pControl );
		static void				ClearFocus();
		static CKTDGUIControl*	GetControlFocus(){ return s_pControlFocus; }

		void			SetShowEnableControlsWithDummyInt( IN int iIndex, IN int iValue, IN bool bShow, IN bool bEnable );
		CKTDXStage*	GetStage()						{ return m_pStage; }
		void		SetStage( CKTDXStage* pStage ) { m_pStage = pStage; }

		void			SetName_LUA( const char* pName );
		void			SetName( const WCHAR* pName );

		const WCHAR*	GetName() const         { return m_Name.c_str(); }

		void		SetPos( const D3DXVECTOR2& pos );
		void		SetPos_LUA( float x, float y );

		D3DXVECTOR2 GetPos() const              { return m_Pos; }

		// Horizontal anchor (T44): from UIAnchors.lua, or the script's own SetAnchor, which wins. GetPos/SetPos are
		// in layout units under the anchor (layout positions); GetCanvasPos/SetCanvasPos are where the dialog is drawn
		// (positions derived from other windows or the mouse).
		void			SetAnchor( UIAnchor eAnchor )	{ m_eAnchor = eAnchor; m_bAnchorDeclared = true; }
		bool			SetAnchor_LUA( const char* pszAnchor );
		UIAnchor		GetAnchor() const				{ return m_eAnchor; }
		bool			IsAnchorDeclared() const		{ return m_bAnchorDeclared; }
		// Declared by the script itself (SetAnchor in Lua), which UIAnchors.lua does not override.
		bool			IsAnchorFromScript() const		{ return m_bAnchorFromScript; }
		// Back to no declared anchor (centered), for the UI editor when an entry of UIAnchors.lua goes.
		void			ClearAnchor()					{ m_eAnchor = UIAnchor::Center; m_bAnchorDeclared = false; m_bAnchorFromScript = false; }
		float			GetAnchorShift() const;
		D3DXVECTOR2		GetCanvasPos() const			{ return D3DXVECTOR2( m_Pos.x + GetAnchorShift(), m_Pos.y ); }
		void			SetCanvasPos( const D3DXVECTOR2& pos ) { SetPos( D3DXVECTOR2( pos.x - GetAnchorShift(), pos.y ) ); }
		// How far a control with its own anchor is drawn from where the dialog's anchor would put it, in layout units.
		float			GetControlAnchorShift( const CKTDGUIControl* pControl ) const;
		// The mouse, relative to the dialog in pixels, in a control's layout units (relative to its offset), with the
		// control's anchor or stretch.
		POINT			ToControlPoint( POINT ptDialog, CKTDGUIControl* pControl ) const;
		// A layout x of a control (with its anchor or stretch) as a layout x under the dialog's anchor, for what the
		// dialog draws next to it (its guide description).
		float			ControlXToDialogX( const CKTDGUIControl* pControl, float fX ) const;
		// Where STRETCH controls grow (UIAnchors.lua STRETCH_ZONES); none: they are scaled whole.
		CKTDGUIStretch&	GetStretch()					{ return m_kStretch; }

		// Named placements (T52): the spots the code shows the dialog at, declared in its own script so the UI editor
		// edits them, instead of positions in the game code:
		//
		//     g_pUIDialog:AddPlacement({ NAME = "MIDDLE", POS = D3DXVECTOR2(250, 300) })
		//     g_pUIDialog:AddPlacement({ NAME = "QUEST_MENU", TARGET = "DLG_UI_Menu.lua", CONTROL = "Button_Quest",
		//                                POS = D3DXVECTOR2(10, 20) })
		//
		// A fixed placement puts the dialog at POS, under ANCHOR when it has one (else the dialog's own anchor). A
		// placement with a TARGET follows a shown dialog loaded from that script (the topmost), and the CONTROL of it
		// when given: POS is then in the target's layout units, from its dialog's position plus the control's offset,
		// so at 4:3 the dialog lands where POS plus those would put it, and on wider screens it takes the target's
		// anchor and stretch. It is resolved each step; while the target is not shown the dialog stays where it is.
		// Moves the code makes on top (Move animations, SetPos) follow the target too.
		struct Placement
		{
			std::wstring	wstrName;
			D3DXVECTOR2		vPos = D3DXVECTOR2( 0.f, 0.f );
			bool			bHasAnchor = false;
			UIAnchor		eAnchor = UIAnchor::Center;
			std::wstring	wstrTarget;			// script key (UIScriptKey); empty: a fixed placement
			std::wstring	wstrTargetControl;	// empty: the target dialog itself
			int				iScriptCall = -1;	// its AddPlacement in GetScriptCalls() (X2_DEV_TOOLS, while a tool tracks)
		};
		void			AddPlacement_LUA();
		void			AddPlacement( const Placement& kPlacement );	// replaces one of the same name
		// Puts the dialog at the named placement; false (and nothing changes) when the script declares none by that name.
		bool			ApplyPlacement( const wchar_t* pwszName );
		// Stops following a target and gives the dialog back its own anchor; it stays where it is.
		void			ClearPlacement();
		const std::vector<Placement>& GetPlacements() const	{ return m_vecPlacements; }
		const Placement*	GetPlacementInEffect() const;
		// Where a placement puts the dialog now (its GetPos), false when its target is not shown.
		bool			ResolvePlacement( const Placement& kPlacement, D3DXVECTOR2& vPos ) const;
		// Whether a script by this key (UIScriptKey) was loaded into the dialog.
		bool			HasScript( const std::wstring& wstrKey ) const;
		// The topmost shown dialog the dialog manager has with that script, or null.
		static CKTDGUIDialog*	FindShownDialog( const std::wstring& wstrKey );

		// Drawing between simulation steps (T41): the dialog manager notes the position when a step begins and ends;
		// OnFrameRender draws at the position between the two, when nothing moved the dialog since (dragging does).
		void		NoteTickBegin( int iTick )  { m_vTickPosBegin = m_Pos; m_iTickPos = iTick; }
		void		NoteTickEnd()               { m_vTickPosEnd = m_Pos; }
		D3DXCOLOR	GetColor() const            { return m_Color; }
		void		SetColor( D3DXCOLOR color ) { m_Color = color; }

		void		SetModal( bool bModal )		{ m_bModal = bModal; }
		bool		GetModal() const            { return m_bModal; }

		void		SetEnable( bool bEnable );
		bool		GetEnable() const           { return m_bEnable; }
		void		SetShow( bool bShow );
		bool		GetShow() const             { return m_bShow; }

		void		SetShowEnable( bool bShow, bool bEnable ) { SetShow( bShow ); SetEnable( bEnable ); }

//{{ robobeg : 2009-01-07
		//void		UpdateDockPos();
//}} robobeg : 2009-01-07
		void		UpdateRects();

		bool		GetFront()					{ return m_bFront; }
		void		SetFront( bool bFront );

		bool		GetIsAutoDeleteEnd()		{ return m_bCheckAutoDeleteEnd; }

		void		Move( D3DXVECTOR2 pos, D3DXCOLOR color, float fChangeTime, bool bDirect = false,
						bool bAutoDelete = false,  bool bPingPong = false, int repeatNum = 1
						, bool bFadeInOut = true
						);

		void		Move_LUA();
		void		MoveStop();

		void		SetEnableMoveByDrag_LUA( bool bCheck ) { m_bEnableMoveByDrag = bCheck; }
		bool		GetEnableMoveByDrag() { return m_bEnableMoveByDrag; }

		void		AddDummyPos( D3DXVECTOR3 pos ) { m_DummyPosList.push_back( pos ); }
		__forceinline D3DXVECTOR3 GetDummyPos( int index )
		{
			if ( index < 0 || index >= (int)m_DummyPosList.size() )
				return D3DXVECTOR3( 0, 0, 0 );

			return m_DummyPosList[index];
		}

		void		AddDummyString_LUA( int iDummyString );
		wstring		GetDummyString( int index ) { return m_DummyStringList[index]; }

		void		AddDummyInt( int dummy ) { m_DummyIntList.push_back(dummy); }
		__forceinline int GetDummyInt( int index )
		{
			if ( index < 0 || index >= (int)m_DummyIntList.size() )
				return 0;

			return m_DummyIntList[index];
		}
		void		ClearDummyInt() { m_DummyIntList.clear(); }



		CKTDGUIButton*		GetButton_LUA( const char* pControlName );
		CKTDGUICheckBox*	GetCheckBox_LUA( const char* pControlName );
		CKTDGUIComboBox*	GetComboBox_LUA( const char* pControlName );
		CKTDGUIEditBox*		GetEditBox_LUA( const char* pControlName );
		CKTDGUIIMEEditBox*	GetIMEEdieBox_LUA( const char* pControlName );
		CKTDGUIListBox*		GetListBox_LUA( const char* pControlName );
		CKTDGUIRadioButton*	GetRadioButton_LUA( const char* pControlName );
		CKTDGUIStatic*		GetStatic_LUA( const char* pControlName );
		CKTDGUISlot*		GetSlot_LUA( const char* pControlName );
		CKTDGUISlot*		GetSlotByID(int ID);		// ID는 사용자가 설정해주는 것으로, 겹칠 수 있으므로 주의해야 한다.

		void			OnMouseLeave();
		bool			GetIsMoving();
		bool			GetIsMouseOver() const { return m_bIsMouseOver; }

		bool			GetIsSuccessInitOpenScript() { return m_bSuccessInitOpenScript; }

		int				GetLayer() { return m_iLayer; }
		void			SetLayer( int iLayer );

		void 			SetID(int id) { m_ID = id; }
		int				GetID() { return m_ID; }

		CKTDGUIRadioButton::RadioButtonGroupInfo* GetRadioButtonGroupInfo( int iGroupID )
		{
			map< int, CKTDGUIRadioButton::RadioButtonGroupInfo* >::iterator it;
			it = m_mapRadioButtonGroupInfo.find( iGroupID );
			if( it != m_mapRadioButtonGroupInfo.end() )
				return (CKTDGUIRadioButton::RadioButtonGroupInfo*) it->second;
			else
				return NULL;
		}

		void AddRadioButtonGroupInfo( CKTDGUIRadioButton::RadioButtonGroupInfo* pInfo )
		{
			m_mapRadioButtonGroupInfo[pInfo->m_iGroupID] = pInfo;
		}

		void SetMouseDownForDrag( bool bCheck ) { m_bMouseDownForDrag = bCheck; }
		bool GetMouseDownForDrag() const { return m_bMouseDownForDrag; }

//{{ robobeg : 2009-01-07
		// Docking
		//int GetDockDialogKeyID() { return m_iDockDialogKeyID; }
		//void SetDockDialogKeyID( int val ) { m_iDockDialogKeyID = val; }
//}} robobeg : 2009-01-07

//{{ robobeg : 2009-01-07
		//int GetKeyID() { return m_KeyID; }
//}} robobeg : 2009-01-07

		// Minimize / Maximize
		bool GetMaximize() { return m_bMaximize; }
		void SetMaximize( bool val );

		// Close On Focus Out
		bool GetCloseOnFocusOut() { return m_bCloseOnFocusOut; }
		void SetCloseOnFocusOut( bool val ) { m_bCloseOnFocusOut = val; }
		void SetCloseOnFocusOutEvent( int eventID ) { m_CustomMsgFocusOutEventID = eventID; }

		void SetCloseCustomUIEventID( int eventID ) { m_CloseCustomUIEventID = eventID; }
		int GetCloseCustomUIEventID() { return m_CloseCustomUIEventID; }
		void SendCloseCustomUIEventID();

		// dialog dragging 범위를 결정하는데 사용된다
		D3DXVECTOR2 GetSize() const { return m_vSize; }
		void SetSize( float fWidth, float fHeight ) { m_vSize = D3DXVECTOR2( fWidth, fHeight ); }


		bool GetEnableDraggingRange() const { return m_bEnableDraggingRange; }
		void SetEnableDraggingRange(bool val) { m_bEnableDraggingRange = val; }
		D3DXVECTOR2 GetDraggingRangeCenter() const { return m_vDraggingRangeCenter; }
		void SetDraggingRangeCenter( float x, float y ) { m_vDraggingRangeCenter = D3DXVECTOR2(x, y ); }
		// The dragging range on this screen. A dialog larger than the 1024x768 layout is a picture panned over the
		// screen (the field and world maps), whose script gives the range as its size less 1024x768, the travel at
		// 4:3: on a wider (or taller) screen, the travel shrinks by what the canvas adds, so the picture covers the
		// screen at both ends (none when the screen is wider than the picture: it stays centered). 2026-11-14.
		D3DXVECTOR2 GetDraggingRange() const;
		// Moves the dialog back into its dragging range now (OnFrameMove does it every step), for code that places
		// it from elsewhere (the world map's navigation box).
		void KeepInDraggingRange();
		// Such a picture narrower (or shorter) than the screen: black bars fill the screen beside it, as the letterbox
		// around the game (T48), instead of what lies behind showing through (21:9, 2026-11-14).
		void DrawPannedPictureBars() const;
		// After a backdrop control (CKTDGUIControl::IsBackdrop) draws, under its anchor: black beside its pictures where
		// the screen is wider, and their outer edges faded into it.
		void DrawBackdropEdges( CKTDGUIControl& kControl ) const;
		void SetDraggingRange( float fWidth, float fHeight ) { m_vDraggingRange = D3DXVECTOR2(fWidth, fHeight); }

		void ShowGuideDesc( CKTDGUIControl* pControl );
		CKTDGUIControl* GetGuideControl() { return m_pStaticGuideDesc; }

		void SetOKAndCancelMsgBox( bool bCheck ) { m_bOKAndCancelMsgBox = bCheck; }
		bool GetOKAndCancelMsgBox() { return m_bOKAndCancelMsgBox; }

		void ClearRadioButtonGroup( int iButtonGroup );


		void SetCustomMsgMouseOut( int msg ) { m_CustomMsgMouseOut = msg; }
		void SetCustomMsgMouseOver( int msg ){ m_CustomMsgMouseOver = msg; }
		void SetCustomMsgStopDragging(int val) { m_CustomMsgStopDragging = val; }
		void SetCustomMsgStartDragging(int val) { m_CustomMsgStartDragging = val; }
		void SetCustomMsgRMouseUp(int msg) { m_CustomMsgRMouseUp = msg; }


//{{ robobeg : 2009-01-07
		//int GetUniqueIndex() const { return m_iUniqueIndex; }
		//void SetUniqueIndex(int val) { m_iUniqueIndex = val; }
//}} robobeg : 2009-01-07

        bool GetHasSlot() { return m_bHasSlot; }
        void SetHasSlot(bool val) { m_bHasSlot = val; }
		bool GetHasUnit() { return m_bHasUnit; }
		void SetHasUnit(CKTDGObject *pUnit);

		void ScaleControls( float fx, float fy );
		void MoveControls( float fx, float fy );

        CKTDGUIDialogManager*   GetUIDialogManager() { return m_pkUIDialogManager; }

        class   CKTDGUIDialogManagerAccess
        {
            friend class    CKTDGUIDialogManager;
            static void SetUIDialogManager( CKTDGUIDialogType kInDialog_, CKTDGUIDialogManager* pkInManager_ )
                {
                    if( kInDialog_ != NULL )
                    {
                        kInDialog_->m_pkUIDialogManager = pkInManager_;
                    }//if
                }
//{{ robobeg : 2009-01-07
            //static void SetKeyID( CKTDGUIDialog& kInDialog_, int val )      { kInDialog_.m_KeyID = val; }
//}} robobeg : 2009-01-07
            static void SetLayer( CKTDGUIDialogType kInDialog_, int iLayer )
            {
                if( kInDialog_ != NULL )
                {
                    kInDialog_->m_iLayer = iLayer;
                }//if
            }
            static void SetFront( CKTDGUIDialogType kInDialog_, bool bInFront_ )
            {
                if( kInDialog_ != NULL )
                {
                    kInDialog_->m_bFront = bInFront_;
                }//if
            }
        };//class   CKTDGUIDialogManagerAccess

		bool GetIsOverControl();
		void SetIsOverControl(bool val);
		bool GetDisableUnderWindow () { return m_bDisableUnderWindow; }
		void SetDisableUnderWindow (bool val) { m_bDisableUnderWindow = val; }

		void SetMouseOverShowDlg(bool val) { m_bMouseOverShowDlg = val; }


		CKTDGUIDialog* Get() { return this; }

		int GetDialogListChangeCount() const { return m_iDialogListChangeCount; }
		void SetDialogListChangeCount(const int val) { m_iDialogListChangeCount = val; }


		float GetTimeForSelfDelete() const { return m_fTimeForSelfDelete; }
		void SetTimeForSelfDelete( float val) { m_fTimeForSelfDelete = val; }

		CKTDGUIControl* GetControlMouseOver() const;

		bool GetHasParticle() { return m_bHasParticle; }
		void AddParticle(CKTDGObject *pObj);
		void ClearParticleList();

		void SetCustomButtonMouseOverSndFile_LUA( const char* pCustomSndFileName );
		void SetCustomButtonMouseUpSndFile_LUA( const char* pCustomSndFileName );
		void SetDownStateToAllButtonType ();


	protected:

		CKTDGUIControl* GetControlAtPoint( POINT pt );
		CKTDGUIControl* GetControlAtPointForStatic( POINT pt );
		CKTDGUIControl*	GetControlAtPointExceptStatic( POINT pt );
		CKTDGUIControl* GetControlAtPointAbsolute( POINT pt );


		void			OnMouseMove( POINT pt );
		bool			HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );	//포커스 잡힌 컨트롤부터 처리해주고 처리 안되면 다른 컨트롤들 순서대로 처리해주다 걸리면 return;

		void			CreateGuideDesc();

		bool			SendInternelEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:

		wstring						m_Name;
		//string						m_Name_MB;

		D3DXVECTOR2					m_Pos;
		D3DXVECTOR2					m_vTickPosBegin{ 0.f, 0.f };	// m_Pos when step m_iTickPos began and ended
		D3DXVECTOR2					m_vTickPosEnd{ 0.f, 0.f };
		int							m_iTickPos = -1;
		D3DXCOLOR					m_Color;
		float						m_fAnimateTime;

		D3DXVECTOR2					m_GoalPos;
		D3DXCOLOR					m_GoalColor;
		float						m_fChangeTime;

		CKTDXStage*					m_pStage;
		bool						m_bModal;
		bool						m_bEnable;
		bool						m_bShow;
		bool						m_bMaximize;
		bool						m_bCloseOnFocusOut;
		int							m_CloseCustomUIEventID;

//{{ robobeg : 2009-01-07
		//int							m_iDockDialogKeyID;
//}} robobeg : 2009-01-07

		bool						m_bAutoDelete;
		bool						m_bCheckAutoDeleteEnd;

		bool						m_bEnableMoveByDrag;
		bool						m_bMouseDownForDrag;
		CKTDGUIControl*				m_pControlMouseOver;
		CKTDGUIControl*				m_pControlMouseOverShowDlg;
		static CKTDGUIControl*		s_pControlFocus;

		CKTDGUIControl*				m_pControlMouseOverStatic;

		vector<CKTDGUIControl*>								m_ControlsList;

		map<std::wstring, CKTDGUIControl*>					m_ControlsMap;
		//map<std::string, CKTDGUIControl*>					m_ControlsMap_MB;

		std::queue< CKTDGUIDialog::DialogMoveItem >	m_queMoveItems;

		vector< D3DXVECTOR3 >		m_DummyPosList;
		vector< wstring	>			m_DummyStringList;
		vector< int >				m_DummyIntList;

		bool						m_bSuccessInitOpenScript;

		int							m_ID;				// 같은 dialog를 여러개 한번에 만들때 구분하기 위한 ID, 보통 그냥 index

//{{ robobeg : 2009-01-07
		//int							m_KeyID;			// 다이얼로그 유니크한 ID
//}} robobeg : 2009-01-07

		map< int, CKTDGUIRadioButton::RadioButtonGroupInfo* > m_mapRadioButtonGroupInfo;

		D3DXVECTOR2					m_vDragGrab;	// the pointer relative to the dialog when a drag starts (layout units)


		D3DXVECTOR2					m_vSize;

		bool						m_bEnableDraggingRange;
		D3DXVECTOR2					m_vDraggingRangeCenter;
		D3DXVECTOR2					m_vDraggingRange;			// width, height

		CKTDGUIDialog*				m_pDialogGuideDesc;
		CKTDGUIStatic*				m_pStaticGuideDesc;

		bool						m_bOKAndCancelMsgBox;
		bool						m_bIsMouseOver;

		int 						m_CustomMsgMouseOut;
		int 						m_CustomMsgMouseOver;
		int							m_CustomMsgStopDragging;
		int							m_CustomMsgStartDragging;

		int							m_CustomMsgFocusOutEventID;


		bool						m_bEnableShortCutEnter;

//{{ robobeg : 2009-01-07
		//int							m_iUniqueIndex;
//}} robobeg : 2009-01-07

        bool                        m_bHasSlot;

		// m_pUnitViewer 외부에서 관리한다. 내부에서 지우거나 하지 않는다.
		// 외부에서 OnFrameMove를 처리하지만, OnFrameRender는 처리하지 않는다.
		// 내부에서 OnFrameRender 처리한다.
		CKTDGObject*                m_pUnitViewer;
		bool						m_bHasUnit;
		int							m_CustomMsgRMouseUp;

		//다이얼로그가 파티클의 레어이를 관리 하도록 다이얼로그 내부에서 OnFrameRender 처리
		vector<CKTDGObject*>		m_ParticleList;
		bool						m_bHasParticle;

private:

//{{ robobeg : 2009-01-07
        CKTDGUIDialogManager*       m_pkUIDialogManager;
//}} robobeg : 2009-01-07

		bool						m_bFront;
		int							m_iLayer;





		bool						m_bDisableUnderWindow;
		bool						m_bIsOverControl;

		bool						m_bMouseOverShowDlg;


		int							m_iDialogListChangeCount; // dialog가 dialog list에 추가될때, list가 변경된 회수를 저장. dialog list에 추가된 이후에 dialog list가 변동이 있었는지 확인하기 위해 사용.

		bool						m_bOneClickJoyVectorButton;

		CKTDXTimer					m_TimerToSelfDelete;
		float						m_fTimeForSelfDelete;
		wstring		m_wstrCustomButtonMouseOverSndFileName;
		wstring		m_wstrCustomButtonMouseUpSndFileName;

		UIAnchor	m_eAnchor = UIAnchor::Center;
		bool		m_bAnchorDeclared = false;
		bool		m_bAnchorFromScript = false;
		CKTDGUIStretch	m_kStretch;

		// Named placements (AddPlacement); the one in effect, by index (-1: none).
		void			FollowPlacementTarget();
		void			ShiftPlaced( const D3DXVECTOR2& vDelta );
		std::vector<Placement>	m_vecPlacements;
		std::vector<std::wstring>	m_vecScriptKeys;	// every script loaded into the dialog (UIScriptKey)
		int				m_iPlacement = -1;
		bool			m_bPlacementResolved = false;
		D3DXVECTOR2		m_vPlacementLast = D3DXVECTOR2( 0.f, 0.f );
		bool			m_bPlacementAnchor = false;			// a placement's ANCHOR is in effect over the dialog's own
		UIAnchor		m_eOwnAnchor = UIAnchor::Center;
		bool			m_bOwnAnchorDeclared = false;

#ifdef X2_DEBUG
		wstring		m_strScriptFileName;
		wstring		m_strMouseOverStaticName;
		bool		m_bShowScriptName;

	public:
		bool GetShowScriptName() { return m_bShowScriptName; }
		void SetShowScriptName( bool bValue_ ) { m_bShowScriptName = bValue_; }
		wstring GetScriptFileName() { return m_strScriptFileName; }
		// Moved by code after its script loaded (a floating window, or one placed next to something): for the anchor
		// recorder (CKTDGUIAnchorRecorder).
		bool GetPlacedByCode() const { return m_bPlacedByCode; }
		// Every script loaded into the dialog, in order: the one it was created with, then those code added
		// (OpenScriptFile), for the live UI snapshot (CKTDGUIAnchorRecorder::NoteLive).
		const std::vector<wstring>& GetOpenedScripts() const { return m_vecOpenedScripts; }
	private:
		std::vector<wstring> m_vecOpenedScripts;
		bool		m_bLoadingScript = false;
		bool		m_bPlacedByCode = false;
	public:
		wstring GetMouseOverStaticName() { return m_strMouseOverStaticName; }
#endif //X2_DEBUG

#ifdef X2_DEV_TOOLS
	public:
		// Where the dialog's scripts set what the UI editor edits (tools/ui-editor): each setter call a script made while
		// it loaded into the dialog, with the object it set and the call's line. Noted only while a tool tracks
		// (CKTDGUIAnchorRecorder::SetToolTracking).
		enum class ScriptCallKind : unsigned char
		{
			DialogPos,		// g_pUIDialog:SetPos(x, y); the object is the dialog
			Point,			// a point table (SetPoint, SetNormalPoint, ...); the object is the picture or control it was set on
			String,			// AddString; the object is the static
			Texture,		// a picture's SetTex(file, key); the object is the picture
			AddPicture,		// a static's AddPicture(picture); the object is the picture
			AddControl,		// g_pUIDialog:AddControl(control); the object is the control
			PicturePos,		// a picture's SetPos(D3DXVECTOR2(x, y)); the object is the picture
			Placement,		// g_pUIDialog:AddPlacement({ ... }); the object is the dialog
		};
		struct ScriptCall
		{
			const void*		pObject = nullptr;
			ScriptCallKind	eKind = ScriptCallKind::Point;
			unsigned		uScript = 0;	// index into GetOpenedScripts()
			int				iLine = 0;		// 1-based, the line of the call's '('
		};
		const std::vector<ScriptCall>& GetScriptCalls() const { return m_vecScriptCalls; }
		// Called by the setters a script reaches through lua_tinker: notes the running Lua call into the dialog whose
		// script is loading, with the object at argument `iObject` (1: its `self`).
		static void NoteScriptCall( ScriptCallKind eKind, int iObject = 1 );
	private:
		std::vector<ScriptCall> m_vecScriptCalls;
		unsigned m_uLoadingScript = 0;		// the script running now: a script may load another (OpenScriptFile)
		int m_iLoadingDepth = 0;			// CKTDXApp::s_iScriptDepth while it runs
		static inline CKTDGUIDialog* s_pLoadingDialog = nullptr;
	public:
#endif

		bool						m_bFadeInOut;

};//class CKTDGUIDialog
