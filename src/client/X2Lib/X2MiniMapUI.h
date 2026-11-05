#pragma once


#include "./X2MiniMap.h"
#include "./X2FieldMiniMap.h"

#include "./X2WorldMapUI.h"


class CX2MiniMapUI
{
public:
	enum MINI_MAP_UI_CUSTOM_MSG
	{
		MMUCM_DUNGEON_ZOOM_IN		= 22000,
		MMUCM_DUNGEON_ZOOM_OUT,

		MMUCM_FIELD_ZOOM_IN,
		MMUCM_FIELD_ZOOM_OUT,
		MMUCM_FIELD_OPEN_WORLD_MAP,

		MMUCM_FIELD_OPEN_NEW_QUEST_MAP,

		MMUCM_ZOOM_BUTTON,

		MMUCM_ENDURANCE_BUTTON_OVER,
		MMUCM_ENDURANCE_BUTTON_OUT,

		MMUCM_CHANGE_CHANNEL,
		MMUCM_CHANGE_CHANNEL_ACCEPT,
		MMUCM_CHANGE_CHANNEL_CANCEL,
		MMUCM_CHANGE_CHANNEL_OPEN,
		MMUCM_CHANGE_CHANNEL_CLOSE,
	};

	enum MINI_MAP_TYPE
	{
		MMT_DUNGEON,		// 미니맵 부분에 라인맵 그리는 방식. 던전, 거래광장에서 쓰고 있다. MMT_LINEMAP으로 이름 바꿀까?
		MMT_FIELD,			// 미니맵 부분에 마을 그림을 그리는 방식. 마을에서 쓰고 있다.
	};

	//{{ kimhc // 2009-12-15 // 서버군 추가 작업
	struct CHANNEL_INFO_IN_MINIMAP
	{
		int				m_iChannelID;
		std::wstring	m_wstrChannelName;

		CHANNEL_INFO_IN_MINIMAP()
		{
			m_iChannelID = 0;
		}

		CHANNEL_INFO_IN_MINIMAP( int iChannelID, const std::wstring& wstrChannelName )
		{
			m_iChannelID		= iChannelID;
			m_wstrChannelName	= wstrChannelName;
		}

		bool operator<( const CHANNEL_INFO_IN_MINIMAP& rhs ) const
		{
			return ( m_iChannelID < rhs.m_iChannelID ? true : false );
		}

		int GetNumRelatedWithChannelName() const;
	};
	//}}  kimhc // 2009-12-15 // 서버군 추가 작업

public:
	CX2MiniMapUI(void);
	virtual ~CX2MiniMapUI(void);


	virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual HRESULT	OnFrameMove( double fTime, float fElapsedTime );


	CX2FieldMiniMap* GetFieldMiniMap() const { return m_pFieldMiniMap; }
	CX2MiniMap* GetDungeonMiniMap() const {	return m_pDungeonMiniMap; }
	CX2WorldMapUI* GetWorldMapUI();

	void CreateDungeonMiniMap()
	{
		CX2MiniMap::DeleteKTDGObject( m_pDungeonMiniMap );
		m_pDungeonMiniMap = CX2MiniMap::CreateMiniMap( this );
		g_pKTDXApp->GetDGManager()->AddObjectChain( (CKTDGObject*) m_pDungeonMiniMap );
	}

	void DeleteDungeonMiniMap()
	{
		CX2MiniMap::DeleteKTDGObject( m_pDungeonMiniMap );
		m_pDungeonMiniMap = NULL;
	}

	void CreateFieldMiniMap()
	{
		CX2FieldMiniMap::DeleteKTDGObject( m_pFieldMiniMap );
		m_pFieldMiniMap = CX2FieldMiniMap::CreateFieldMiniMap( this );

		g_pKTDXApp->GetDGManager()->AddObjectChain( (CKTDGObject*) m_pFieldMiniMap );
	}

	void DeleteFieldMiniMap()
	{
		CX2FieldMiniMap::DeleteKTDGObject( m_pFieldMiniMap );
		m_pFieldMiniMap = NULL;
	}

	void SetShowMiniMap( MINI_MAP_TYPE eMiniMapType, bool bShow );
	void SetStage( CKTDXStage* pStage );




	//void SetTitle( const wstring& minimapTitle )
	//{
	//	if( NULL != m_pDungeonMiniMap )
	//	{
	//		m_pDungeonMiniMap->SetTitle( minimapTitle*);
	//	}
	//}


	void SetVillageMapID( int iMapID );
	void SetTitle(const WCHAR* val);

	void UpdateMailNotice( std::vector<KPostSmallTitleInfo> &vecUnReadMailTitle );
	void UpdateQuestNotice( int iQuestNum, int TopQuestID, int VID );

	int GetTopQuestID(){ return m_TopQuestID; }
	int GetQuestVillageMapID(){ return m_QuestVillageMapID; }

	void ZoomDLGSetting();

	void CreateNewQuestEffect();
	void CreateNewMailEffect();

	void SetMonsterNum( bool bShow, int iNum = 0 );
	void SetNoExpAtThisDungeon(bool val);

//{{ kimhc // 2010.5.2 // 비밀던전 작업(경험치)
	void SetWarningForGettingExp( bool bVal );		// 비밀던전에서 경험치를 얻지 못할 수 도 있는 경우에 true
//}} kimhc // 2010.5.2 // 비밀던전 작업(경험치)

	void UpdateEnduranceNotice();
	void SetShowEnduranceUI( bool bShow );

	void UpdateEventNotice();
	void UpdateChannelInfo();


	void UpdateChannelIndex();

	//{{ kimhc // 2009-12-16 // 채널 콤보박스 설정
	void SetChannelComboBox();
	int	GetListIndexFromChannelID( int iChannelID );
	//}} kimhc // 2009-12-16 // 채널 콤보박스 설정

	MINI_MAP_TYPE GetCurrentMiniMapType() { return m_eCurrentMiniMapType; }

	//{{ 허상형 : [2011/3/14/] //	월드 미션
	bool			IsOpenDefenceMinimap() { return m_bIsOpenDefenceMinimap; }
		//}} 허상형 : [2011/3/14/] //	월드 미션




	bool	GetShow()const {return m_bShow;}

	void CloseWorldMapUI() {if( NULL != m_pWorldMapUI )m_pWorldMapUI->CloseWorldMapUI();}

	void SetEnableChannelComboBox( bool bVal_ );

private:
	CX2MiniMap*			m_pDungeonMiniMap;
	CX2FieldMiniMap*	m_pFieldMiniMap;
	CX2WorldMapUI*		m_pWorldMapUI;

	bool			m_bShow;

	CKTDGUIDialogType	m_pDLGFrameWindow;
	int				m_TopQuestID;			// 퀘스트 버튼을 눌렀을 때 표시해줄 퀘스트의 ID
	int				m_QuestVillageMapID;		// 퀘 버튼 눌렀을때 표시해줄 마을 맵 ID
	bool			m_bCreateNewQuestEffectWhenOpen;
	bool			m_bCreateNewMailEffectWhenOpen;

	int				m_iNewMail;

	MINI_MAP_TYPE	m_eCurrentMiniMapType;

	CX2UIEnduranceChecker	m_UIEnduranceChecker;

	int				m_iChangeChannelId;

	CKTDGUIDialogType	m_pDLGQuery;
	float			m_fChannelInfoTime;
	float			m_fChangeChannelTime;

	//{{ 허상형 : [2011/3/14/] //	월드 미션
	bool			m_bIsOpenDefenceMinimap;
		//}} 허상형 : [2011/3/14/] //	월드 미션


	//{{ kimhc // 2009-12-16 // 서버군 추가 작업
	std::vector< CHANNEL_INFO_IN_MINIMAP > m_vecChannelInfoInMinimap;
	//}} kimhc // 2009-12-16 // 서버군 추가 작업




};
