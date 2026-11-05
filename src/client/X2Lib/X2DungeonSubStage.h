#pragma once

class CX2DungeonSubStage
{
	public:
		enum CLEAR_TYPE
		{
			CT_SUB_STAGE = 0,
			CT_STAGE,
			CT_STAGE_FORCE,
			CT_STAGE_SECRET,
			CT_GAME,
			CT_FAILURE = 5, //해외팀 클리어 조건을 달성하면 지게 되는 클리어 타입 2013.07.03 김창한
		};

		enum CLEAR_CONDITION
		{
			CC_KILL_ALL_ACTIVE_NPC,
			CC_KILL_KEYCODE_NPC,
			CC_KILL_KEYCODE_NPC_ANY,
			CC_KILL_NPC_TYPE,
			CC_KILL_KEYCODE_NPC_COUNT,
			CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC,
			CC_TIME,
		};

		enum GO_TYPE
		{
			GT_NONE = 0,
			GT_RIGHT,
			GT_LEFT,
			GT_UP,
			GT_DOWN,
		};

		enum SECRET_STAGE_PAD
		{
			SSP_NONE	= 0,
			SSP_SECRET	= 1,
			SSP_ELITE	= 2,
			SSP_NORMAL	= 3,
		};


		struct NextStageData
		{
			CLEAR_TYPE m_eClearType;
			int m_iStageIndex;
			int m_iSubStageIndex;
			int m_iRate;

			int m_iDungeonEndingSpeechTableIndex;	// clear type == CT_GAME인 경우에만 유효
		};


		struct ClearCondtionData
		{
			struct MonsterCount
			{
				int m_iKey;				// could be npckeycode, or npcid
				int m_iOriginalCount;	// substage 시작할때 몬스터의 수
				int m_iObjectiveCount;	// 목표 킬 수 - note!! 일단 지금은 CC_KILL_KEYCODE_NPC_COUNT인 경우에만 사용

				MonsterCount()
				{
					m_iKey = 0;
					m_iOriginalCount = 0;
					m_iObjectiveCount = 0;
				}
			};

			bool						m_bEnable;
			bool						m_bChecked;				// 이 clear cond check가 성공해서 이미 처리되었다
			CLEAR_CONDITION				m_eClearCondition;
			std::vector<MonsterCount>	m_vecNPCKeyCode;
			std::vector<MonsterCount>	m_vecNPCID;
			std::vector<int>			m_vecPortalLineIndex;
			bool						m_bCreatedPortalEffect;		// CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC인 경우에 몬스터 다 잡으면 포탈 위치 표시하기 위해서
			float						m_fClearTime;
			std::vector<NextStageData>	m_vecNextStage;
			float						m_fDelayTimeClearSubStage;

		public:
			ClearCondtionData()
			{
				m_bEnable = true;
				m_bChecked = false;
				m_bCreatedPortalEffect = false;
				m_fClearTime = -1.f;
				m_eClearCondition = CC_KILL_ALL_ACTIVE_NPC;
				m_fDelayTimeClearSubStage = 0.0f;
			}
		};

		struct CurtainData
		{
			int			m_LineIndex;
			bool		m_bAtStart;
			bool		m_bLookLeft;
		};

		struct NPCData
		{
			int								m_UID;
			CX2UnitManager::NPC_UNIT_ID		m_UnitID;
			int								m_KeyCode;
			bool							m_bFocusCamera;

			std::vector<int>				m_vecStartPos;
			int								m_StartPos;

			bool							m_bActive;
			bool							m_bShowGage;
			D3DXVECTOR3						m_AddPos;

			int								m_Level;
			//{{ 2007. 9. 6  최육사
			int								m_Rate;
			//}}

			bool							m_bHasBossGage;
			bool							m_bShowBossName;
			bool							m_bShowSubBossName;
			bool							m_bSiegeMode;
			//{{ 2007. 10. 26  최육사
			bool							m_bIsRight;
			//}}

			//{{ 2007. 9. 6  최육사
			bool							m_bSubNpc;
			//}}

			bool							m_bNoDrop;
			float							m_fUnitScale;

			//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
			u_short							m_usBossGaugeHPLines;
			//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
			//{{ 2010. 03. 29  손영준	몬스터 팀 설정
			bool							m_bAllyNpc;
			//}}
			float							m_fShowBossNameDelayTime;
			bool							m_bStopAtStartState;



		};

		struct Trigger
		{
			bool		m_bHost;
			bool		m_bOneTime;
			bool		m_bReacted;
			float		m_Interval;
			float		m_NowInterval;
			string		m_ConditionFunc;
			string		m_ReactFunc;

		};

		struct SubStageData
		{
			int						m_StartLineSet;
			int						m_EndLineSet;
			int						m_MainLineSet;
			vector<CurtainData*>	m_CurtainDataList;
			vector<NPCData*>		m_NPCDataList;
			vector<Trigger*>		m_TriggerList;
			//vector<CX2GUNPC*>		m_NPCList;

			CX2DungeonSubStage::GO_TYPE	m_GoType;
			vector<ClearCondtionData>	m_vecClearCondData;

			map < int, string >	m_mapKeyCodeNStartState;


			typedef std::vector< wstring > SecretStageEnteringSpeech;
			SecretStageEnteringSpeech m_SecretStageEnteringSpeech;

			CX2UnitManager::NPC_UNIT_ID m_eSecretStageNPCID;

//{{ kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용
			bool					m_bUsePosOfMonsterForRebirth;
			int						m_iKeyCodeOfMonsterForRebirth;
//}} kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용

			SubStageData()
				: m_StartLineSet( -1 )
				, m_EndLineSet( -1 )
				, m_MainLineSet( 0 )
				, m_CurtainDataList()
				, m_NPCDataList()
				, m_TriggerList()
				//, m_NPCList()
				, m_GoType( GT_NONE )
				, m_vecClearCondData()
				, m_mapKeyCodeNStartState()
				, m_SecretStageEnteringSpeech()
				, m_eSecretStageNPCID( CX2UnitManager::NUI_NONE )
				, m_bUsePosOfMonsterForRebirth( false )
				, m_iKeyCodeOfMonsterForRebirth( -1 )
			{
			}

			~SubStageData()
			{
				for( int i = 0; i < (int)m_CurtainDataList.size(); i++ )
				{
					CurtainData* pCurtainData = m_CurtainDataList[i];
					SAFE_DELETE( pCurtainData );
				}
				m_CurtainDataList.clear();

				for( int i = 0; i < (int)m_NPCDataList.size(); i++ )
				{
					NPCData* pNPCData = m_NPCDataList[i];
					SAFE_DELETE( pNPCData );
				}
				m_NPCDataList.clear();

				for( int i = 0; i < (int)m_TriggerList.size(); i++ )
				{
					Trigger* pTrigger = m_TriggerList[i];
					SAFE_DELETE( pTrigger );
				}
				m_TriggerList.clear();

// 				for( int i = 0; i < (int)m_NPCList.size(); i++ )
// 				{
// 					CX2GUNPC* pCX2GUNPC = m_NPCList[i];
// 					SAFE_DELETE_KTDGOBJECT( pCX2GUNPC );
// 				}
// 				m_NPCList.clear();
			}

			bool LoadData( KLuaManager& luaManager, bool bIsNpcLoad = false );
			//void CreateNPC();
			void AddNPCData( KNPCUnitReq& kNPCUnitReq );

			//{{ kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용
			bool GetUsePosOfMonsterForRebirth() const { return m_bUsePosOfMonsterForRebirth; }
			void SetUsePosOfMonsterForRebirth(bool val) { m_bUsePosOfMonsterForRebirth = val; }
			int	 GetKeyCodeOfMonsterForRebirth() const { return m_iKeyCodeOfMonsterForRebirth; }
			void SetKeyCodeOfMonsterForRebirth(int val) { m_iKeyCodeOfMonsterForRebirth = val; }
			//}} kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용

		private:
			void LoadCurtain( KLuaManager& luaManager );
			void LoadNPCData( KLuaManager& luaManager );
			void LoadTrigger( KLuaManager& luaManager );
			void LoadSecretStageEnteringSpeech( KLuaManager& luaManager );


			void FetchNPCData( KLuaManager& luaManager, CX2DungeonSubStage::NPCData* pNPCData );

		};

	public:
		CX2DungeonSubStage( int subStageIndex, SubStageData* pSubStageData );
		~CX2DungeonSubStage(void);

		void				Start()
		{
			m_bStart = true;
			ToggleStartLineMapSet(true);
			ToggleMainLineMapSet(true);
			ToggleEndLineMapSet(true);

			dbg::clog << L"SubStageStart! Index: " << m_SubStageIndex << dbg::endl;
		}

		void				OnFrameMove( double fTime, float fElapsedTime );


		void				CurtainFrameMove( double fTime, float fElapsedTime );

		void				ToggleStartLineMapSet( bool bToggle );
		void				ToggleEndLineMapSet( bool bToggle );
		void				ToggleMainLineMapSet( bool bToggle );

		bool				TouchStartLineMapSet( int lineNum );
		bool				TouchEndLineMapSet( int lineNum );
		bool				TouchMainLineMapSet( int lineNum );

		SubStageData*		GetSubStageData(){ return m_pSubStageData; }
		int					GetSubStageIndex(){ return m_SubStageIndex; }

		void				SetMonsterCountForSubStageClearCheck();
		void				PrepareClearSubStage( int iClearConditionIndex );
		void				ClearSubStage( int clearType, int nextStageNum, int nextSubStageNum );
		void				CheckIfSubStageCleared();
		void				CheckIfSubStageCleared_NotHost();
		float				GetRemainTimeInClearCondition();


		void				DeleteAllNPCUnit();

		static void			DeleteAllNPCUnit( const vector<NPCData*>& vecNPCData_ );

		void				SwapToGameNPCUnit();
		bool				IsNPCLoadComplete();

		void				CreateCurtain();
		void				DeleteCurtain();
		void				BreakCurtain();
		void				OpenCurtain();

		CX2Curtain*			GetCurtain( int index ) { return m_CurtainList[index]; }
		int					GetCurtainNum() { return (int)m_CurtainList.size(); }

		bool 				GetWinWhenTimeOut() const { return m_bWinWhenTimeOut; }
		void 				SetWinWhenTimeOut(bool val) { m_bWinWhenTimeOut = val; }

		bool				GetStarted() const { return m_bStart; }

		float				GetEndGameDelay() const { return m_fEndGameDelay; }
		void				ShowBossNameAtStart( float fElapsedTimeSubStage );
		float				GetSubStageTime(){ return m_fElapsedTimeSubStage; }

		bool				GetIsBossStage() const { return m_bIsBossStage; }
		bool				GetBossStageHaveManyNextStage() const { return m_bBossStageHaveManyNextStage; }

		bool				GetCheckedClearSubStage() const { return m_bCheckedClearSubStage; }

		void				SetEnableClearCondition( int iIndex, bool bEnable );

		bool				GetDoneClearSubStage() const { return m_bDoneClearSubStage; }


		int GetNextSecretStageIndex() const { return m_iNextSecretStageIndex; }
		int GetNextNormalStageIndex() const { return m_iNextNormalStageIndex; }
		int GetNextEliteStageIndex() const { return m_iNextEliteStageIndex; }
		void DeleteNPCBySubStageEnd();


		void				ClearDungeonGame();

		static void CreateAndAppendGUNPC( std::vector<CX2GUNPC*>& vecInOutNPCs_, const vector<NPCData*>& vecNPCData_ );

		static CX2GUNPC*    CreateGUNPC( const NPCData* pNpcData_ );

		void				ProcessAfterSubStageClear_NotHost();
	protected:
		void				ProcessTrigger();

		void				ChangeLineTypeAfterStageClear();



		SubStageData*		m_pSubStageData;
		int					m_SubStageIndex;
		float				m_fElapsedTime;

		bool				m_bStart;

		int					m_LoadNPCIndex;

		vector<CX2Curtain*>	m_CurtainList;

		float				m_fEndGameDelay;

		bool				m_bWinWhenTimeOut;			// 훈련소 던전용
		bool				m_bBossNameWasShown;

		float				m_fElapsedTimeSubStage;		// substage가 시작된 후로 흐른 시간
		bool				m_bDoneClearSubStage;		// ClearSubStage() 함수를 호출했는지

		bool				m_bIsBossStage;
		bool				m_bBossStageHaveManyNextStage;	// 2개 이상의 next stage data가 존재한다

		bool				m_bCheckedClearSubStage;		// clear condition 체크 완료하고 다음 스테이지로 넘어가기 위해 잠시 대기
		CKTDXTimer			m_TimerDelayClearSubstage;		// clear condition을 만족했지만 잠시 연출을 위해서 substage 혹은 stage 넘어가는 것을 기다린다.
		float				m_fDelayTimeClearSubstage;

		bool				m_bCheckedClearSubStage_NotHost;	// 방장 아닌 클라이언트가 이펙트 표시를 위해서 던전 스테이지 클리어 처리 되었는지 체크하도록 하기 위해서


		int m_iNextSecretStageIndex;	// 비밀 스테이지가 있는 경우에 스테이지 번호를 저장하기 위한 변수
		int m_iNextNormalStageIndex;
		int m_iNextEliteStageIndex;


		CKTDGParticleSystem::CParticleEventSequenceHandle m_hBossDanger;
		CKTDGParticleSystem::CParticleEventSequenceHandle m_hBossDangerAdd;

};
