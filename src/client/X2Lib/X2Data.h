#pragma once




//#ifdef USER_DEFINED_KEYBOARD_SETTING
//	// 'func' 기능을 하는 실제 키입력이 무엇인지 알아오는 매크로
//	#define KEY_MAP( func )	( CX2Data::s_KeyMappingLayer[1][ CX2Data::s_KeyMappingLayer[0][ func ] ] )
//#else
//	#define KEY_MAP( func )	( func )
//#endif USER_DEFINED_KEYBOARD_SETTING


#include "X2StateClassChange.h"


// 게임 동작중 변하지 않는 데이터를 담아두는 구조체, 게임 동작중 변하는 데이터는 CX2InstanceData에 담아주세요~
class CX2Data : public CKTDXStage
{
	public:
//#ifdef USER_DEFINED_KEYBOARD_SETTING
//		// 키입력에 의해 동작하는 기능의 나열
//		enum KEY_FUNC
//		{
//			KF_USE_QUICK_SLOT_ITEM_1,
//			KF_USE_QUICK_SLOT_ITEM_2,
//			KF_USE_QUICK_SLOT_ITEM_3,
//			KF_USE_QUICK_SLOT_ITEM_4,
//			KF_USE_QUICK_SLOT_ITEM_5,
//			KF_USE_QUICK_SLOT_ITEM_6,
//			KF_USE_QUICK_SLOT_ITEM_7,
//			KF_USE_QUICK_SLOT_ITEM_8,
//			KF_USE_QUICK_SLOT_ITEM_9,
//			KF_USE_QUICK_SLOT_ITEM_10,
//
//			...
//		};
//#endif USER_DEFINED_KEYBOARD_SETTING



	enum CHARACTER_IMAGE_TYPE
	{
		CIT_INVALID,
		CIT_DungeonClear,
		CIT_160by160,
		CIT_160by160Gray,
		CIT_50by50,
		CIT_20by20,
		CIT_ClassInitial,
		CIT_MyGage,
		CIT_Party,
// #ifdef REFORM_ENTRY_POINT	 	// 13-11-11, 진입 구조 개편, kimjh
		CIT_UnitSlot,
// #endif // REFORM_ENTRY_POINT	// 13-11-11, 진입 구조 개편, kimjh
		CIT_END,

	};



	public:
		CX2Data(void);
		virtual ~CX2Data(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );

		virtual HRESULT OnResetDevice();
		virtual HRESULT OnLostDevice();

		void LoadUserUnitMotion1();
		void LoadUserUnitMotion2();
		void LoadUserUnitMotion3();
		void LoadUserUnitMotion4();
		void LoadUserUnitMotion5();
		void LoadUserUnitMotion6();
		void LoadUserUnitMotion7();
		void LoadUserUnitMotion8();
		void LoadUserUnitMotion9();

		void LoadCommonDevice();

		void LoadPicChar1();
		void LoadPicChar2();
		void LoadPicChar3();
		void LoadPicChar4();
		void LoadPicChar5();
		void LoadPicChar6();
		void LoadPicChar7();
		void LoadPicChar8();
		void LoadPicChar9();
		void LoadPicChar10();
		void LoadPicCharAll_LUA();

		void LoadEXPTable();
		void LoadPVPResultTable();

		CKTDGPicChar*	GetPicChar(){ return m_pPicChar; }
		CKTDGPicChar*	GetPicCharRed(){ return m_pPicCharRed; }
		CKTDGPicChar*	GetPicCharBlue(){ return m_pPicCharBlue; }
		CKTDGPicChar*	GetPicCharGreen(){ return m_pPicCharGreen; }
		CKTDGPicChar*	GetPicCharYellow(){ return m_pPicCharYellow; }
		CKTDGPicChar*	GetPicCharGameScore(){ return m_pPicCharGameScore; }
		CKTDGPicChar*	GetPicCharTechPoint(){ return m_pPicCharTechPoint; }
		CKTDGPicChar*	GetScoreBonusPicChar(){ return m_pScoreBonusPicChar; }
		CKTDGPicChar*	GetComboPicChar(){ return m_pComboPicChar; }
		CKTDGPicChar*	GetComboPicCharBig(){ return m_pComboPicCharBig; }

		CX2EXPTable*	GetEXPTable(){ return &m_EXPTable; }
		CX2PVPResultTable* GetPVPResultTable() { return &m_PVPResultTable; }

		//{{ 임홍락 [2012.05.16] 클라이언트 단일화 // CLIENT_USE_NATION_FLAG 필수 포함임
		//}}

		void LoadItemManager();
		CX2ItemManager* ResetItemManager();
		CX2ItemManager* GetItemManager(){ return m_pItemManager; }
		void DeleteItemManager(){ SAFE_DELETE(m_pItemManager); }

		CX2UnitManager* ResetUnitManager( const WCHAR* pFileName, const WCHAR* pNPCFileName, const WCHAR* pNPCStatFileName );
		CX2UnitManager* GetUnitManager(){ return m_pUnitManager; }
		void DeletetUnitManager(){ SAFE_DELETE(m_pUnitManager); }

		CX2UIUnitManager* ResetUIUnitManager();
		CX2UIUnitManager* GetUIUnitManager(){ return m_pUIUnitManager; }
		void DeletetUIUnitManager(){ SAFE_DELETE(m_pUIUnitManager); }

		CX2WorldManager* ResetWorldManager( const WCHAR* pFileName );
		CX2WorldManager* GetWorldManager(){ return m_pWorldManager; }
		void DeletetWorldManager(){ SAFE_DELETE(m_pWorldManager); }

		CX2DungeonManager* ResetDungeonManager( const WCHAR* pFileName );
		CX2DungeonManager* GetDungeonManager() { return m_pDungeonManager; }
		void DeleteDungerManager() { SAFE_DELETE(m_pDungeonManager); }

		CX2ChannelManager* ResetChannelManager();
		CX2ChannelManager* GetChannelManager(){ return m_pChannelManager; }
		void DeleteChannelManager(){ SAFE_DELETE(m_pChannelManager); }

		CX2QuestManager* ResetQuestManager( const WCHAR* pQuestFileName, const WCHAR* pSubQuestFileName, const WCHAR* pAccountQuestFileName );
		void			OpenDayOfWeekQuestScript( const WCHAR* pQuestFileName_ );
		CX2QuestManager* GetQuestManager(){ return m_pQuestManager; }
		void DeleteQuestManager(){ SAFE_DELETE(m_pQuestManager); }

		CX2SkillTree* ResetSkillTree( const WCHAR* pFileName, const WCHAR* pSkillDataFileName );
		CX2SkillTree* GetSkillTree(){ return m_pSkillTree; }
		void DeleteSkillTree(){ SAFE_DELETE(m_pSkillTree); }


		CX2ComboTree* ResetComboTree( const wstring& filename );
		CX2ComboTree* GetComboTree(){ return m_pComboTree; }
		void DeleteComboTree(){ SAFE_DELETE(m_pComboTree); }



		CX2PartyManager* ResetPartyManager();
		CX2PartyManager* GetPartyManager() { return m_pPartyManager; }
		void DeletePartyManager() { SAFE_DELETE(m_pPartyManager); }

		CX2TrainingCenterTable* ResetTrainingCenterTable( const WCHAR* pFileName );
		CX2TrainingCenterTable* GetTrainingCenterTable() { return m_pTrainingCenterTable; }
		void DeleteTrainingCenterTable() { SAFE_DELETE(m_pTrainingCenterTable); }

		CX2ServerProtocol* ResetServerProtocol();
		CX2ServerProtocol* GetServerProtocol(){ return m_pServerProtocol; }
		void DeleteServerProtocol(){ SAFE_DELETE(m_pServerProtocol); }

		//CX2User* ResetMyUser( CX2User::UserData* pUserData );
        CX2User* ResetMyUser( const CX2User::UserData& kUserData );
		CX2User* GetMyUser(){ return m_pMyUser; }
		void DeleteMyUser(){ SAFE_DELETE(m_pMyUser); }

		CX2PVPRoom* ResetPVPRoom();
		CX2PVPRoom* GetPVPRoom(){ return m_pPVPRoom; }
		void DeletePVPRoom(){ SAFE_DELETE(m_pPVPRoom); }

		CX2DungeonRoom* ResetDungeonRoom();
		CX2DungeonRoom* GetDungeonRoom() { return m_pDungeonRoom; }
		void DeleteDungeonRoom() { SAFE_DELETE(m_pDungeonRoom); }

		CX2BattleFieldRoom* ResetBattleFieldRoom();
		CX2BattleFieldRoom* GetBattleFieldRoom() const { return m_pBattleFieldRoom; }
		void DeleteBattleFieldRoom() { SAFE_DELETE(m_pBattleFieldRoom); }

		void DeleteAllRooms()
		{
			DeletePVPRoom();
			DeleteDungeonRoom();
			DeleteBattleFieldRoom();
		}


		CKTDGParticleSystem* ResetUIMajorParticle( const WCHAR* pFileName );
		CKTDGParticleSystem* GetUIMajorParticle(){ return m_pUIMajorParticle; }
		void DeleteUIMajorParticle(){ SAFE_DELETE(m_pUIMajorParticle); }

		CKTDGParticleSystem* ResetUIMinorParticle( const WCHAR* pFileName );
		CKTDGParticleSystem* GetUIMinorParticle(){ return m_pUIMinorParticle; }
		void DeleteUIMinorParticle(){ SAFE_DELETE(m_pUIMinorParticle); }

		CKTDGParticleSystem* ResetGameMajorParticle( const WCHAR* pFileName );
		CKTDGParticleSystem* GetGameMajorParticle(){ return m_pGameMajorParticle; }
		void DeleteGameMajorParticle(){ SAFE_DELETE(m_pGameMajorParticle); }

		CKTDGParticleSystem* ResetGameMinorParticle( const WCHAR* pFileName );
		CKTDGParticleSystem* GetGameMinorParticle(){ return m_pGameMinorParticle; }
		void DeleteGameMinorParticle(){ SAFE_DELETE(m_pGameMinorParticle); }

		CKTDGXMeshPlayer* ResetUIMajorXMeshPlayer( const WCHAR* pFileName );
		CKTDGXMeshPlayer* GetUIMajorXMeshPlayer(){ return m_pUIMajorXMeshPlayer; }
		void DeleteUIMajorXMeshPlayer(){ SAFE_DELETE(m_pUIMajorXMeshPlayer); }

		CKTDGXMeshPlayer* ResetUIMinorXMeshPlayer( const WCHAR* pFileName );
		CKTDGXMeshPlayer* GetUIMinorXMeshPlayer(){ return m_pUIMinorXMeshPlayer; }
		void DeleteUIMinorXMeshPlayer(){ SAFE_DELETE(m_pUIMinorXMeshPlayer); }

		CKTDGXMeshPlayer* ResetGameMajorXMeshPlayer( const WCHAR* pFileName );
		CKTDGXMeshPlayer* GetGameMajorXMeshPlayer(){ return m_pGameMajorXMeshPlayer; }
		void DeleteGameMajorXMeshPlayer(){ SAFE_DELETE(m_pGameMajorXMeshPlayer); }

		CKTDGXMeshPlayer* ResetGameMinorXMeshPlayer( const WCHAR* pFileName );
		CKTDGXMeshPlayer* GetGameMinorXMeshPlayer(){ return m_pGameMinorXMeshPlayer; }
		void DeleteGameMinorXMeshPlayer(){ SAFE_DELETE(m_pGameMinorXMeshPlayer); }

		CX2EffectSet* ResetUIEffectSet( const WCHAR* pFileName );
		CX2EffectSet* GetUIEffectSet() { return &m_UIEffectSet; }
		void DeleteUIEffectSet() { m_UIEffectSet.ClearTempletAndInstance(); }

		CX2EffectSet* ResetGameEffectSet( const WCHAR* pFileName );
		CX2EffectSet* GetGameEffectSet() { return &m_GameEffectSet; }
		void DeleteGameEffectSet() { m_GameEffectSet.ClearTempletAndInstance(); }


		CKTDNUDP*	ResetGameUDP( int port );
		CKTDNUDP*	GetGameUDP(){ return m_pGameUDP; }
		void		DeleteGameUDP(){ SAFE_DELETE( m_pGameUDP ); }

		CX2PVPGame::PVPResultInfo* ResetPVPResultInfo( KEGS_END_GAME_PVP_RESULT_DATA_NOT& kPacket );
		CX2PVPGame::PVPResultInfo* GetPVPResultInfo() { return m_pPVPResultInfo; }
		void DeletePVPResultInfo() { SAFE_DELETE( m_pPVPResultInfo); }

		CX2DungeonGame::DungeonResultInfo* ResetDungeonResultInfo( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kPacket );
		CX2DungeonGame::DungeonResultInfo* GetDungeonResultInfo() { return m_pDungeonResultInfo; }
		void DeleteDungeonResultInfo() { SAFE_DELETE( m_pDungeonResultInfo); }

		CX2DamageManager* ResetDamageManager();
		CX2DamageManager* GetDamageManager(){ return m_pDamageManager; }
		void DeleteDamageManager(){ SAFE_DELETE( m_pDamageManager ); }


		CX2LocationManager*	ResetLocationManager();
		CX2LocationManager* GetLocationManager() { return m_pLocationManager; }
		void DeleteLocationManager() { SAFE_DELETE( m_pLocationManager ); }

		CX2BattleFieldManager& GetBattleFieldManager() { return m_BattleFieldManager; }
		void ResetBattleFieldManager( const wchar_t* szScriptFileName_ );

		CX2Community* ResetMessenger();
		CX2Community* GetMessenger() { return m_pMessenger; }
		void DeleteMessenger() { SAFE_DELETE( m_pMessenger ); }
		CX2EventScene* GetEventScene() { return m_pEventScene; }
		void DeleteEventScene() { SAFE_DELETE( m_pEventScene); }
		CX2EventScene* ResetEventScene(const WCHAR* pSceneObjectFileName, const WCHAR* pEventSceneFileName);

		CX2SlideShot* ResetSlideShot();
		CX2SlideShot* GetSlideShot(){ return m_pSlideShot; }
		void DeleteSlideShot() { SAFE_DELETE( m_pSlideShot); }

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업
		CX2SocketItem* ResetSocketItem( const WCHAR* pFileName, const WCHAR* pFormulaFileName_ );
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업


		CX2SocketItem* GetSocketItem(){ return m_pCX2SocketItem; }
		void DeleteSocketItem(){ SAFE_DELETE( m_pCX2SocketItem); }

		CX2EnchantItem* ResetEnchantItem();
		CX2EnchantItem* GetEnchantItem(){ return m_pCX2EnchantItem; }
		void DeleteEnchantItem(){ SAFE_DELETE( m_pCX2EnchantItem); }

		//{{ 2008. 10. 1  최육사
//#ifdef TITLE_SYSTEM
		CX2TitleManager* ResetTitleManager();
		CX2TitleManager* GetTitleManager(){ return m_pCX2TitleManager; }
		void DeleteTitleManager(){ SAFE_DELETE( m_pCX2TitleManager ); }
//#endif
		//}}

		//{{ 2009. 6. 24  최육사	보상테이블
		CX2RewardTable* ResetRewardTable();
		CX2RewardTable* GetRewardTable(){ return m_pRewardTable; }
		void DeleteRewardTable(){ SAFE_DELETE( m_pRewardTable ); }
		//}}

		//{{ 허상형 : [2009/9/16] //	길드 매니저
		CX2GuildManager* ResetGuildManager();
		CX2GuildManager* GetGuildManager(){ return m_pGuildManager; }
		//}} 허상형 : [2009/9/16] //	길드 매니저

		//{{ kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장
		const std::wstring&		GetSavedServerGroupFileName() const { return m_wstrServerGroupFileName; }
		//}} kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장
		CX2CashShop*	ResetCashShop( CKTDXStage* pNowStage, const WCHAR* wszFileName );
		CX2CashShop*	GetCashShop() { return m_pCashShop; }
		void DeleteCashShop() { SAFE_DELETE(m_pCashShop); }


		CX2ClassChangePopup& GetClassChangePopup() { return m_ClassChangePopup; }

		static bool GetCharacterImageName( OUT wstring& fileName, OUT wstring& pieceName, const CX2Unit::UNIT_CLASS eUnitClass, const CHARACTER_IMAGE_TYPE eImageType );
		static bool GetPvpNpcImageName( OUT wstring& fileName, OUT wstring& pieceName, CX2UnitManager::NPC_UNIT_ID iNpcId );

		void ReadyCharacterMotion();

		wstring GetComboString( CX2DungeonRoom::RANK_TYPE rankType );

		void PlayLobbyBGM( const WCHAR* wszName, bool bPlay );

		//void ReadyUnitDevice( CX2Unit::UNIT_CLASS unitClass );

		__time64_t	GetServerCurrentTime();
		void SetServerCurrentTime( wstring wstrTime );
		__int64	GetServerCurrentTime64();
		void SetServerCurrentTimeInt64( __int64 tTime );

		int GetArrivedPresentNum() { return m_ArrivedPresentNum; }
		void SetArrivedPresentNum( int presentNum ) { m_ArrivedPresentNum = presentNum; }

		void SetPostItemMsg( vector<wstring>& vecPostItemMsg ) { m_vecPostItemMsg = vecPostItemMsg; }
		void GetPostItemMsg( vector<wstring>& vecPostItemMsg ) { vecPostItemMsg = m_vecPostItemMsg; }
		void DeletePostItemMsg()
		{
			m_vecPostItemMsg.resize(0);
		}

		int	GetModelDetailPercent() { return m_ModelDetailPercent; }
		void SetModelDetailPercent( int detailPercent ) { m_ModelDetailPercent = detailPercent; }
		void			DeleteUIManager() { SAFE_DELETE( m_pUIManager ); }
        CX2UIManager*   ResetUIManager();
        CX2UIManager*   GetUIManager() { return m_pUIManager; }
		CX2SlotItem::CX2Slot**		GetSlotBeforeDragging(){ return &m_pSlotBeforeDragging; }
		UidType*		GetpDraggingItemUID(){ return &m_DraggingItemUID; }
		//void			SetSlotBeforeDragging(CX2SlotItem::CX2Slot* pSlot){ m_pSlotBeforeDragging = pSlot; }

		CX2CharPopupMenu* ResetUserMenu();
		CX2CharPopupMenu* GetUserMenu() { return m_pCharPopUpMenu; }


		CX2UserListPopUp* ResetUserListPopUp();
		CX2UserListPopUp* GetUserListPopUp() { return m_pUserListPopUp; }


		void ToggleRenderEtc() { m_bToggleRenderEtc = !m_bToggleRenderEtc; }
		bool GetRenderEtc() { return m_bToggleRenderEtc; }

		void SetStateArg(int val) { m_StateArg = val; }
		int	 GetStateArg() { return m_StateArg; }

		CX2PetManager *ResetPetManager();
		CX2PetManager *GetPetManager() { return m_pPetManager; }

		CX2RidingPetManager *ResetRidingPetManager();

		//{{ 허상형 : [2011/3/14/] //	월드 미션
		CX2WorldMissionManager *ResetWorldMissionManager();
		CX2WorldMissionManager *GetWorldMissionManager() { return m_pWorldMissionManager; }
		//}} 허상형 : [2011/3/14/] //	월드 미션

		//{{ JHKang / 강정훈 / 2011.6.3 / 휴면 복귀 유저 보상 및 특수 보상에 대한 UI 및 버프 처리
		CX2PremiumBuffManager	*ResetPremiumBuffManager();
		CX2PremiumBuffManager	*GetPremiumBuffManager() { return m_pPremiumBuffManager; }
		//}}


		//{{ 오현빈 // 2012-05-15 // 플레이 가이드 Class
		CX2PlayGuide			*ResetPlayGuide();
		CX2PlayGuide			*GetPlayGuide(){ return m_pPlayGuide; }
		//}}

		//오현빈 // 2012-12-14 // 널체크 코드 길이 줄이기 위해 추가 했습니다.
		int				GetSelectUnitLevel() const;
		CX2Unit::UNIT_CLASS		GetSelectUnitClass() const;
		CX2Unit::UNIT_TYPE		GetSelectUnitType() const;
		SEnum::DUNGEON_ID		GetCurrentDungeonID();
		BYTE					GetSelectUnitMemberShipGrade() const;
		SEnum::SERVER_GROUP_ID	GetSelectUnitServerGroupID () const;

		void					SetSelectUnitED( int iED_ );

		CX2CubeOpenImageManager* ResetCubeOpenImageManager();
		CX2CubeOpenImageManager* GetCubeOpenImageManager() { return m_pCubeOpenImageManager; }



		CX2RelationshipManager* ResetRelationshipManager();
		CX2RelationshipManager* GetRelationshipManager() { return m_pRelationshipManager; }

		CX2RelationshipEffectManager* GetRelationshipEffectManager () {return m_pRelationshipEffectManager; }
		CX2RelationshipEffectManager* ResetRelationshipEffectManager();

		CX2ItemStatCalculator& GetItemStatCalculator(){ return m_ItemStatCalculator; }
		void ResetItemStatCalculator(){ m_ItemStatCalculator.ResetItemStatCalculator(); }

		float RotateYSelectCharacterType( CX2Unit::UNIT_CLASS eClass_ );
		//{{ 2012.09.08	임홍락	글로벌 미션 매니저
		CX2GlobalMissionManager* ResetGlobalMissionManager();
		CX2GlobalMissionManager* GetGlobalMissionManager() { return m_pGlobalMissionManager; }
		//}} 2012.09.08	임홍락	글로벌 미션 매니저
	public:
#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
		vector<CX2EffectSet*>		m_vecGameEFfectSet;
		vector<CKTDGXMeshPlayer*>	m_vecGameMajorXMeshPlayer;
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가
		//vector<KServerSetData>	m_ServerSetList; // 채널서버 접속 방식으로 바뀌면서, 사용안되는 변수임. 삭제해야함.

//#ifdef USER_DEFINED_KEYBOARD_SETTING
//		//{{ dmlee 2008.11.27 실제 키보드 입력과 실제 동작을 맵핑 해주는 테이블
//		// 실제 키입력 -> layer[0] -> ... -> layer[1] 실제 기능, 동작
//		// 유저가 설정할 수 있는 테이블은 layer[0]
//		// 시스템 내부적으로 변경되는 keymapping은 일단 layer[1]에, 필요하면 layer[2] 추가
//		static unsigned char	s_KeyMappingLayer[2][256];
//		static const int		KEY_MAPPING_LAYER_USER		= 1;
//		static const int		KEY_MAPPING_LAYER_SYSTEM	= 0;
//		//}} dmlee 2008.11.27 실제 키보드 입력과 실제 동작을 맵핑 해주는 테이블
//#endif USER_DEFINED_KEYBOARD_SETTING

	private:
		CX2ItemManager*			m_pItemManager;
		CX2UnitManager*			m_pUnitManager;
		CX2UIUnitManager*		m_pUIUnitManager;
		CX2WorldManager*		m_pWorldManager;
		CX2DungeonManager*		m_pDungeonManager;
		CX2ChannelManager*		m_pChannelManager;
		CX2QuestManager*		m_pQuestManager;
		CX2SkillTree*			m_pSkillTree;

		CX2ComboTree*			m_pComboTree;

		CX2DamageManager*		m_pDamageManager;
		CX2PartyManager*		m_pPartyManager;
		//{{ 2009. 6. 24  최육사	보상테이블
		CX2RewardTable*			m_pRewardTable;
		//}}

		CX2ServerProtocol*		m_pServerProtocol;
		CX2User*				m_pMyUser;

		CX2PVPRoom*				m_pPVPRoom;
		CX2DungeonRoom*			m_pDungeonRoom;
		CX2BattleFieldRoom*		m_pBattleFieldRoom;

		CKTDGParticleSystem*	m_pUIMajorParticle;
		CKTDGParticleSystem*	m_pUIMinorParticle;

		CKTDGParticleSystem*	m_pGameMajorParticle;
		CKTDGParticleSystem*	m_pGameMinorParticle;

		CKTDGXMeshPlayer*		m_pUIMajorXMeshPlayer;
		CKTDGXMeshPlayer*		m_pUIMinorXMeshPlayer;

		CKTDGXMeshPlayer*		m_pGameMajorXMeshPlayer;
		CKTDGXMeshPlayer*		m_pGameMinorXMeshPlayer;

		CX2EffectSet			m_UIEffectSet;
		CX2EffectSet			m_GameEffectSet;

		CX2BattleFieldManager	m_BattleFieldManager;

		CKTDNUDP*				m_pGameUDP;

		CX2PVPGame::PVPResultInfo*			m_pPVPResultInfo;
		CX2DungeonGame::DungeonResultInfo*	m_pDungeonResultInfo;


		CKTDGPicChar*			m_pPicChar;
		CKTDGPicChar*			m_pPicCharRed;
		CKTDGPicChar*			m_pPicCharBlue;
		CKTDGPicChar*			m_pPicCharGreen;
		CKTDGPicChar*			m_pPicCharYellow;
		CKTDGPicChar*			m_pPicCharGameScore;
		CKTDGPicChar*			m_pPicCharTechPoint;
		CKTDGPicChar*			m_pScoreBonusPicChar;
		CKTDGPicChar*			m_pComboPicChar;
		CKTDGPicChar*			m_pComboPicCharBig;

		CX2EXPTable				m_EXPTable;
		CX2PVPResultTable		m_PVPResultTable;

		CX2LocationManager*		m_pLocationManager;

		CX2Community*			m_pMessenger;
		CX2TrainingCenterTable*	m_pTrainingCenterTable;

		CX2EventScene*			m_pEventScene;
		CX2SlideShot*			m_pSlideShot;
		CX2SocketItem*			m_pCX2SocketItem;
		CX2EnchantItem*			m_pCX2EnchantItem;

		//{{ 2008. 10. 1  최육사
//#ifdef TITLE_SYSTEM
		CX2TitleManager*		m_pCX2TitleManager;
//#endif
		//}}

		CX2CashShop*			m_pCashShop;

		CTime					m_ServerCurrentTime;
		__int64					m_tServerCurrent64Time;
		DWORD					m_TickCountAtGetTime;

		int						m_ArrivedPresentNum;

		vector< wstring >		m_vecPostItemMsg;

		//KEGS_OPEN_ARCADE_ROOM_LIST_ACK m_ArcadeChannelData;

		int						m_ModelDetailPercent;

        CX2UIManager            *m_pUIManager;
		CX2SlotItem::CX2Slot*	m_pSlotBeforeDragging;
		UidType					m_DraggingItemUID;

		CX2CharPopupMenu*		m_pCharPopUpMenu;

		bool m_bToggleRenderEtc;
		int  m_StateArg;

	//{{ 허상형 : [2009/9/16] //	길드 매니저 객체
		CX2GuildManager*		m_pGuildManager;
	//}} 허상형 : [2009/9/16] //	길드 매니저 객체

	//{{ kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장
		const std::wstring			m_wstrServerGroupFileName;
	//}} kimhc // 2009-12-15 // 이전에 플레이 했던 서버군 저장

		CX2ClassChangePopup		m_ClassChangePopup;

		CX2PetManager			*m_pPetManager;

		CX2UserListPopUp		*m_pUserListPopUp;

//{{ 허상형 : [2011/3/14/] //	월드 미션
		CX2WorldMissionManager	*m_pWorldMissionManager;
//}} 허상형 : [2011/3/14/] //	월드 미션

		//{{ JHKang / 강정훈 / 2011.6.3 / 휴면 복귀 유저 보상 및 특수 보상에 대한 UI 및 버프 처리
		CX2PremiumBuffManager	*m_pPremiumBuffManager;
		//}}


		//{{ 오현빈 // 2012-05-15 // 플레이 가이드 Class
		CX2PlayGuide			*m_pPlayGuide;
		//}}

		CX2CubeOpenImageManager*	m_pCubeOpenImageManager;




		CX2RelationshipManager *	   m_pRelationshipManager;
		CX2RelationshipEffectManager * m_pRelationshipEffectManager;

		CX2ItemStatCalculator		m_ItemStatCalculator;

		CX2GlobalMissionManager*		m_pGlobalMissionManager;
};
