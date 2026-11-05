#pragma once

class CX2State;
class CX2GameEdit
{
	public:

		enum UI_GAME_EDIT_CUSTOM_MSG
		{
//#ifdef MODIFY_GAME_EDIT_CAMERA
			UGECM_CAMERA_EDIT_CLOSE = 55000,			/// 카메라 치트 UI 닫기
			UGECM_CAMERA_EDIT_FREE_CAMERA_ON,			/// /fc true
			UGECM_CAMERA_EDIT_FREE_CAMERA_OFF,			/// /fc false
			UGECM_CAMERA_EDIT_MENUAL_CAMERA_ON,			/// /mc true
			UGECM_CAMERA_EDIT_MENUAL_CAMERA_OFF,		/// /mc false
			UGECM_CAMERA_EDIT_SLOW_FRAME_01,			/// /sf 0.1 100
			UGECM_CAMERA_EDIT_SLOW_FRAME_02,			/// /sf 0.5 100
			UGECM_CAMERA_EDIT_SLOW_FRAME_03,			/// /sf 1
//#endif //MODIFY_GAME_EDIT_CAMERA
		};

#if X2_DEV_TOOLS //JHKang
		enum UI_CHANGE_CLASS_CHEAT_MSG
		{
			UCCCM_CLOSE					= 60000,
			UCCCM_DOUBLE_CLICK_CLASS,
		};

#endif //CHEAT_CLASS_CHANGE

#if X2_DEV_TOOLS
		enum EFFECT_LOG_LEVEL
		{
			ELL_NONE,
			ELL_EFFECT_LOG_1,	// 데미지 이펙트, 이펙트셋 로그
			ELL_EFFECT_LOG_2,	// 1레벨 로그 + 파티클, 메시 로그(미구현)
		};
#endif // EFFECT_USE_LOG
	public:

		CX2GameEdit();
		virtual ~CX2GameEdit(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual HRESULT OnFrameRender();

		void HotKeyProcess();
		void HotKeyProcess_BossRaid();
#if X2_DEV_TOOLS
		void HotKeyProcess_FreeTraining();
#endif // SUMMON_MONSTER_BY_PRESS_KEY_AT_TRAINING_GAME


		void SetShow( bool bShow );
		bool GetShow() { return m_bShow; }
		void SetEnable( bool bEnable );
		bool GetEnable() { return m_bEnable; }

		void SetNowStage( CX2State* pStage );


		bool ExecCommand( const WCHAR* pCommandString );
#if X2_DEV_TOOLS
		void ExecuteLastCommand();
#endif // CHEAT_COMMAND_SHORT_CUT_KEY_TEST


		void AddString( CX2ChatWindow::CHAT_TYPE chatType, const WCHAR* pString );
		void AddString_LUA( int chatType, const char* pString );

		void AddCommandChange( const char* pOrgCommand, const char* pChangeCommand );
		void AddHelperText( const char* pHelperText, CX2User::X2_USER_AUTH_LEVEL userLevel );

		void ToggleCommandList();
		//{{ 2007. 11. 12  최육사  현재 로드된 디바이스들을 파일로 출력
		void SaveFileLoadedDevices();
		//}}
		//{{ 2009-11-30  최육사
		void AdminGetGuildSPoint_LUA( int iCount );
		//}}

		void SetLastCreatedMonster(int val){ m_iLastCreatedMonster = val; }
		int GetLastCreatedMonster(){ return m_iLastCreatedMonster; }
		void MonsterStateChange(const char* pString );

		//{{ 2010. 8. 10	최육사	펫 시스템
		void PetCheat_LUA( int iCheatType, int iValue );
		void PetActionCheat_LUA( const char *motionName );
		//}}
		//{{ 2010. 9. 29	최육사	ED 모니터링 치트
		void UserEdCheat_LUA( const char* nickName );
		//}}
		//{{ 2011. 04. 16	최육사	대리 상인
//#ifdef SERV_PSHOP_AGENCY
		void ShopCheat_LUA( int iPeriod );
//#endif SERV_PSHOP_AGENCY
		//}}
		//{{ 2011. 08. 11	최육사	투니 랜드 채널링
		void ChannelingCodeCheat_LUA( int iChannelingCode );
		//}}
		//{{ 2011. 08. 12   김민성      헤니르 개편
		void HenirRewardCountCheat_LUA( int iHenirReward );
		//}}
		void HenirBuffCheat_LUA( int iHenirBuff );

		void ElswordWSPCheat_LUA( int iWSP );

		//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
		void EDMonitoringModeCheat_LUA( bool bMode );
		//}}

		void RefreshDailyQuest();

#if X2_DEV_TOOLS
		void ItemViewCheat( int itemID );
#endif // ITEM_VIEW_CHEAT


		void RequireGetDangerousValue_LUA();

		void IncreaseFieldDangerousValue_LUA (int iAddValue_);


		void IncreaseLoverDate_LUA (int iAddValue_);


		//{{ kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드
		void AveragePingTime_LUA( DWORD dwAveragePingTime_ );
		//}} kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드

		bool SetValueByHeroMatchWisperNotice( KLuaManagerProxy& kLuaManager_, std::vector<KLuaManagerProxyLuaValue>& vec_, string wstrCommandString, int iNowStringIndex_ );	/// 귓속말 공지
		bool SetValueByHeroMatchDeleteGroupUser( KLuaManagerProxy& kLuaManager_, std::vector<KLuaManagerProxyLuaValue>& vec_, string wstrCommandString, int iNowStringIndex_ );	/// 그룹에서 특정 유저 제거

		CKTDGUIDialogType GetEditChatWindow(){ return m_pDLGGameEdit; }


		bool DeleteGUildAdd_LUA( int iGuildUID );

		//{{ 2011.03.04  임규수 헤니르 랭킹 삭제 치트 ( 운영자,개발자 계정 )
		bool DeleteHenirRankingRank_LUA( int nRankingType, int nRank );
		bool DeleteHenirRankingNickName_LUA( int nRankingType, const char* pNickName );
		bool DeleteHenirRankingUnitUID_LUA( int nRankingType, UidType iUnitUID );
		//}}

		void ToggleInvisible_LUA();


		void FullChargeRidingStamina( int iValue_ );

#if X2_DEV_TOOLS
		void SetEffectLogLevel( EFFECT_LOG_LEVEL eLogLevel ) { m_eShowEffetLogLevel = eLogLevel; }
		EFFECT_LOG_LEVEL GetEffectLogLevel() const { return m_eShowEffetLogLevel; }
#endif // EFFECT_USE_LOG

#if X2_DEV_TOOLS //JHKang
		void ToggleClassChangeCheat();
#endif //CHEAT_CLASS_CHANGE

		void OpenRaidGate();
		void CloseRateGate();
		void RequireGetTotalDangerousValue();
		void IncreaseTotalDangerousValue(int iValue_);

	protected:

		bool Reset();

		void RegisterLuaBind();
		bool OpenScriptFile( const WCHAR* pFilename );

		void AddCommandHistory( const WCHAR* pCommandMsg );
		bool SetCommandByUpDownArrow( int tempCommandHistoryIndex );

		bool							m_bShow;
		bool							m_bEnable;

		CKTDGUIDialogType					m_pDLGGameEdit;
		CX2ChatWindow*					m_pChatWindow;

		CX2State*						m_pState;

		map< string, string >			m_mapCommandChange; //     /ㅜ -> /n 이런식으로 변경

		vector< wstring >				m_CommandHistory;
		int								m_NowCommandHistoryIndex;

		int							m_iLastCreatedMonster;


		wstring							m_wstrDevHelp;
		wstring							m_wstrOperatorHelp;


#if X2_DEV_TOOLS
		EFFECT_LOG_LEVEL				m_eShowEffetLogLevel;
#endif // EFFECT_USE_LOG

#if X2_DEV_TOOLS //JHKang
		CKTDGUIDialogType				m_pDLGClassChangeCheat;
		bool							m_bShowClassChangeCheat;
#endif //CHEAT_CLASS_CHANGE
};
