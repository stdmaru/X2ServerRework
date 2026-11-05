#pragma once

class CX2MemoryHolder
{
	public:

		enum X2_MEMORY_TYPE
		{
			XMT_VILLAGE = 0,
			XMT_ROOM_USER_EQUIP,
			XMT_END,
		};

	public:

		CX2MemoryHolder(void);
		virtual ~CX2MemoryHolder(void);


		CKTDGUIDialogType GetDungeonRoomBackDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetDungeonRoomFrontDLG( CKTDXStage* pStage );

		CKTDGUIDialogType GetPVPRoomTeamBackDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetPVPRoomSurvivorBackDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetPVPRoomFrontDLG( CKTDXStage* pStage );

		CKTDGUIDialogType GetPVPLobbyBack( CKTDXStage* pStage );
		CKTDGUIDialogType GetPVPLobbyFront( CKTDXStage* pStage );

		CKTDGUIDialogType GetVillageMapBackDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetVillageMapFrontDLG( CKTDXStage* pStage );
		void ReleaseVillageMapDLG();
		bool GetIsParsedVillageMapDLG() { return m_bIsParsedVillageMapDLG; }
		void SetIsParsedVillageMapDLG( bool bCheck ) { m_bIsParsedVillageMapDLG = bCheck; }

		CKTDGUIDialogType GetLocalMapBackDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetLocalMapFrontDLG( CKTDXStage* pStage );
		void ReleaseLocalMapDLG();
		bool GetIsParsedLocalMapDLG() { return m_bIsParsedLocalMapDLG; }
		void SetIsParsedLocalMapDLG( bool bCheck ) { m_bIsParsedLocalMapDLG = bCheck; }

		CKTDGUIDialogType GetMenuDLG( CKTDXStage* pStage );
		CKTDGUIDialogType GetMenuDLG() { return m_pMenuDLG; }

		CKTDGUIDialogType GetCommonDLG( CKTDXStage* pStage );
		void RelaseCommonDLG();


		CKTDGUIDialogType GetOptionDLG( CKTDXStage* pStage );




		void SetShowNewYear2014Event( bool bShow );
		void SetShowNewYear2014EventBoard( bool bShow );
		bool GetShowNewYear2014EventBoard() const;
		CKTDGUIDialogType GetNewYear2014EventDLG( CKTDXStage* pStage );
		void UpdateNewYear2014Event();





		void LoadVillageMemory( int villageID );
		void ReleaseVillageMemory();

		void LoadEquip( const WCHAR* pDeviceID );
		void ClearEquip();

	protected:

		void DefualtSettingDLG( CKTDGUIDialogType pDialog, CKTDXStage* pStage );

		CKTDXDeviceXMesh* XMeshReady( const WCHAR* pFileName );
		CKTDXDeviceXSkinMesh* XSkinMeshReady( const WCHAR* pFileName );
		CKTDXDeviceTexture* TextureReady( const WCHAR* pFileName );


		CKTDGUIDialogType m_pDungeonRoomBackDLG;
		CKTDGUIDialogType m_pDungeonRoomFrontDLG;

		CKTDGUIDialogType m_pPVPRoomTeamBackDLG;
		CKTDGUIDialogType m_pPVPRoomSurvivorBackDLG;
		CKTDGUIDialogType m_pPVPRoomFrontDLG;

		CKTDGUIDialogType m_pPVPLobbyBackDLG;
		CKTDGUIDialogType m_pPVPLobbyFrontDLG;

		CKTDGUIDialogType m_pVillageMapBackDLG;
		CKTDGUIDialogType m_pVillageMapFrontDLG;
		bool m_bIsParsedVillageMapDLG;

		CKTDGUIDialogType m_pLocalMapBackDLG;
		CKTDGUIDialogType m_pLocalMapFrontDLG;
		bool m_bIsParsedLocalMapDLG;

		CKTDGUIDialogType m_pCommonDLG;
		CKTDGUIDialogType m_pMenuDLG;


		CKTDGUIDialogType	m_pDLGOptionWindow;




		CKTDGUIDialogType		m_pNewYear2014EventDLG;
		bool					m_bShowNewYear2014Event;
		bool					m_bShowNewYear2014EventBoard;





		int										m_VillageID;
		vector<CKTDXDevice*>					m_vecVillageDevice;
		vector<CKTDXDevice*>					m_EqipDeviceList;
};
