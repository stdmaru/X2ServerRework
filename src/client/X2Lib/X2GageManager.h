#pragma once

class CX2Game;
class CX2GameUnit;
class   CX2GUUser;
class   CX2GUNPC;

/////////////////////////////////////////////////////////////////////////////////////////////////
class CX2GageManager
{
public:

// #ifdef SERV_DUNGEON_FORCED_EXIT_SYSTEM
	enum GAGE_UI_CUSTOM_MSG
	{
		GUCM_BAN_FAULTY_PLAYER_BUTTON_CLICKED = 54000,		// 강퇴하기 버튼이 눌렸음!
//#ifdef ERASE_BUFF_CHEAT
		GUCM_ERASE_BUFF_CHEAT				  = 54001,		// 버프 제거 치트
//#endif // ERASE_BUFF_CHEAT
	};
// #endif // SERV_DUNGEON_FORCED_EXIT_SYSTEM

	class CX2GageSet
    : private x2::NonCopyable
	{
		public:
			CX2GageSet( CX2GageUI* pGageUI_, CX2GageData* pGageData_, const UidType uidGameUnit_ )
				: m_pGageUI( pGageUI_ ), m_pGageData( pGageData_ ), m_optrGameUnit(), m_uidGameUnit( uidGameUnit_ ), m_fNowHpPercent( 1.0f ), m_fNowMpPercent ( 1.0f )
                , m_uRefCount(0)

			{}

		~CX2GageSet()
		{
			SAFE_DELETE( m_pGageUI );
			SAFE_DELETE( m_pGageData );
		}

		bool GetShow() const { return m_pGageUI->GetShow(); }
		void SetShow( const bool bShow_ ) { m_pGageUI->SetShow( bShow_ ); }

        void OnFrameMove( float fElapsedTime )	{ m_pGageUI->OnFrameMove( fElapsedTime );}
		void OnFrameRender() { m_pGageUI->OnFrameRender(); }



		void SetOwnerGameUnit( CX2GameUnit* pOwnerGameUnit_ ) { m_optrGameUnit = pOwnerGameUnit_; }

		UidType GetUid() const { return m_uidGameUnit; }

		void SetMaxHp( const float fMaxHp_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetMaxHp( fMaxHp_ );
		}

		void SetNowHp( const float fNowHp_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetNowHp( fNowHp_ );
		}

		void SetMaxMp( const float fMaxMp_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetMaxMp( fMaxMp_ );
		}

		void SetNowMp( const float fNowMp_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetNowMp( fNowMp_ );
		}

		void SetMaxSoul( const float fMaxSoul_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetMaxSoul( fMaxSoul_ );

		}

		void SetNowSoul( const float fNowSoul_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetNowSoul( fNowSoul_ );
		}

		void SetHyperModeCount( const BYTE byHyperCount_ )
		{
			if ( NULL != m_pGageData )
				m_pGageData->SetHyperModeCount( byHyperCount_ );
		}

		void InitMyGageWhenMovingToVillage()
		{
			if ( NULL != m_pGageData )
				m_pGageData->InitWhenGameIsOver();
		}

		const CX2GageData*	GetGageData() const;


		CX2GageUI*			GetGageUI() const { return m_pGageUI; }
		void UpdateGageDataFromGameUnit();

		void CrashMyStateGageUI( const D3DXVECTOR2& min_, const D3DXVECTOR2& max_, D3DXCOLOR changeColor_ )
		{
			if ( NULL != m_pGageUI )
				m_pGageUI->CrashMyStateGageUI( min_, max_, changeColor_ );
		}

		void SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

		void UpNowHpAndMpInVillage();

		void UpdateNowHpPercent() { SetNowHpPercent( m_pGageData->GetNowHp() / m_pGageData->GetMaxHp() ); }
		float GetNowHpPercent() const { return m_fNowHpPercent; }
		void SetNowHpPercent( const float fHpPercent_ ) { m_fNowHpPercent = fHpPercent_; }

		void UpdateNowMpPercent() { SetNowMpPercent( m_pGageData->GetNowMp() / m_pGageData->GetMaxMp() ); }
		float GetNowMpPercent() const { return m_fNowMpPercent; }
		void SetNowMpPercent( const float fMpPercent_ ) { m_fNowMpPercent = fMpPercent_; }

		void UpdateNowHpFromPercent( const float fPercent_ ) { m_pGageData->SetNowHp( m_pGageData->GetMaxHp() * fPercent_ ); }
		void UpdateNowMpFromPercent( const float fPercent_ ) { m_pGageData->SetNowMp( m_pGageData->GetMaxMp() * fPercent_ ); }

        void    AddRef()    {   ++m_uRefCount; }
        void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

	private:

        unsigned                                        m_uRefCount;

		CX2GageUI*			m_pGageUI;
		CX2GageData*		m_pGageData;
		CX2GameUnitoPtr		m_optrGameUnit;
		UidType				m_uidGameUnit;

		KProtectedType<float>	m_fNowHpPercent;	// 1레벨 보정던전등이 있기 때문에 HP의 절대량이 아닌 Max값 대비 %도 저장 (0.0~1.0 사이값)
		KProtectedType<float>	m_fNowMpPercent;	// 패시브로 인한 최대 MP 증가량을 마을에서 확인 할 수 없기 때문에 Max값 대비 %도 저장 (0.0~1.0 사이값)
	};

    typedef boost::intrusive_ptr<CX2GageSet> CX2GageSetPtr;

	static CX2GageManager* GetInstance();
	static void DestroyInstance();
	static void StateChange( const int iX2State_ );


	void SetShow( const bool bShow_ );
	void CreateMyGageUI( const CX2Unit::UNIT_CLASS eGameUnitClass_, const UidType uid_ );
	void InitMyGageWhenGameIsOver();

	void InsertPartyMemberUI( const KPartyUserInfo& kPartyUserInfo_, CX2GameUnit* pGameUnitPartyMember_ );
	void RemovePartyMemberUIByUserUid( const UidType uid_ );
	void ClearPartyMemberUI();

	bool UICustomEventProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	// Gage UI 의 메시지를 처리하는 곳
	bool Handler_EGS_FORCED_EXIT_VOTE_ACK ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_FORCED_EXIT_VOTE_REQ ( const bool IsWantBan_, const UidType UserUID_ );			// 강퇴 버튼을 눌렀음!

	void ChangeButtonType ( const UidType UserUID_ );
	void SetAllPartyMemberVoteButtonType ( const CX2PartyMemberGageUI::BAN_VOTE_BUTTON_TYPE ButtonType_ ); // 모든 파티원의 투표 버튼 타입을 통일함
	void VoteAllFaultyPlayer ();						// 불량 유저가 되면 불량 유저에게 자동 투표

	CX2PartyMemberGageUI * GetPartyMemberDataByNickName ( const wstring & wstrName_ ) const;		// ID 로 PartyMemberData 를 받아옴
	CX2PartyMemberGageUI * GetPartyMemberDataByUID ( const UidType uIdType_ ) const;				// UID 로 PartyMemberData 를 받아옴
	CX2PartyMemberGageUI * GetPartyMemberDataByIndex ( const int Index_ ) const;					// Index 로 PartyMemberData 를 받아옴

	bool				   GetMyUserIsFaultyPlayer ( ) const { return m_bMyUserIsFaultyPlayer; }
	void				   SetMyUserIsFaultyPlayer ( const bool bMyUserIsFaultyPlayer_ ) 		{ m_bMyUserIsFaultyPlayer = bMyUserIsFaultyPlayer_; }




	void UpdatePartyMemberUIPosition( bool bForce = false );
	void ResetOtherPlayerUIPosition();

	bool CanThisMemberInsert( const UidType uidGameUnit_ );


	static void OnFrameMoveInSpecificX2State( float fElapsedTime_ );
    void OnFrameMove( float fElapsedTime_ );
	void OnFrameRender();


	void SetMyGameUnit( CX2GameUnit* pMyGameUnit_ );
	void SetPartyMemberGameUnit( const UINT uiIndex_, CX2GameUnit* pParyMemberGameUnit_ );
	void SetPartyMemberGameUnit( CX2GameUnit* pParyMemberGameUnit_ );

	void UpdateGageDataFromGameUnit();

	void SetMaxHp( const UidType uid_, const float fMaxHp_ );
	void SetNowHp( const UidType uid_, const float fNowHp_ );

	void SetMaxMp( const UidType uid_, const float fMaxMp_ );
	void SetNowMp( const UidType uid_, const float fNowMp_ );

	void SetMaxSoul( const UidType uid_, const float fMaxSoul_ );
	void SetNowSoul( const UidType uid_, const float fNowSoul_ );

	void SetHyperModeCount( const UidType uid_, const BYTE byHyperCount_ );

	void CrashMyStateGageUI( const D3DXVECTOR2& min_, const D3DXVECTOR2& max_, D3DXCOLOR changeColor_ )
	{
		if ( NULL != m_ptrMyGageSet )
			m_ptrMyGageSet->CrashMyStateGageUI( min_, max_, changeColor_ );
	}

	void UpdateNowHpPercent()
	{
		if ( NULL != m_ptrMyGageSet )
			m_ptrMyGageSet->UpdateNowHpPercent();
	}

	void UpdateNowMpPercent()
	{
		if ( NULL != m_ptrMyGageSet )
			m_ptrMyGageSet->UpdateNowMpPercent();
	}

	float GetMyNowHpPercent() const
	{
		if ( NULL != m_ptrMyGageSet )
			return m_ptrMyGageSet->GetNowHpPercent();
		else
			return 1.0f;
	}

	float GetMyNowMpPercent() const
	{
		if ( NULL != m_ptrMyGageSet )
			return m_ptrMyGageSet->GetNowMpPercent();
		else
			return 1.0f;
	}

	bool GetShowMyGageUI() const
	{
		if( NULL == m_ptrMyGageSet )
			return false;
		else
			return m_ptrMyGageSet->GetShow();
	}

	void CreateAndInsertBossGageUI( CX2GameUnit* pBossGameUnit_, const WCHAR* wszFaceTexName_, const WCHAR* wszFaceTexPieceName_ );
	void ShowBossGageUIByUIDAndNotShowOtherBossGageUI( const UidType uidBoss_, bool bShow_ = true);
	void ShowBossGageUI( bool bShow_ );
	void ClearBossGageUIList();
	void DeleteBossGageUIByUID( const UidType uidBoss_ );

	const CX2GageData*	GetMyGageData() const;
	CX2GageData* GetMyGageData();

	const CX2MyGageUI* GetMyGageUI() const;
	void SetCharacterImage( const CX2Unit::UNIT_CLASS eGameUnitClass_ ){ static_cast<CX2MyGageUI*>(m_ptrMyGageSet->GetGageUI())->SetCharacterImage(eGameUnitClass_);}
	void ResetGageUIEtc( const CX2Unit::UNIT_CLASS eGameUnitClass_ ) { static_cast<CX2MyGageUI*>(m_ptrMyGageSet->GetGageUI())->ResetGageUIEtc(eGameUnitClass_); }

	bool GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus_ ) const;
	void SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus_ );

	void InitMySkillCoolTimeList( const CX2UserSkillTree& userSkillTree_ );
	void SetMySkillCoolTimeList( const map<int, int>& mapSkillCoolTime_ );
	void SetMySkillCoolTimeList(  const UINT uiSlotIndex_, const float fRemainCoolTime_ );
	void GetMySkillCoolTimeListToPacket( OUT map<int, int>& mapSkillCoolTime_ ) const;

	const vector<float>& GetSkillCoolTimeList() const { return m_vecSkillCoolTime; }

	//void InitMyQuickSlotCoolTimeList( const UINT uiSizeofQuickSlot_ );
	void SetMyQuickSlotCoolTimeList( const map<int, int>& mapQuickSlotCoolTime_ );
	void SetMyQuickSlotCoolTimeList( const map<int, CKTDXTimer>& mapQuickSlotCoolTime_ );
	void GetMyQuickCoolTimeListToPacket( OUT map<int, int>& mapQuickCoolTime_ ) const;

	const map<int, int>& GetQuickSlotCoolTimeList() const { return m_mapQuickSlotCoolTime; }

	void SetMyRidingSkillCoolTimeList( const map<int, int>& mapSkillCoolTime_ );
	void SetMyRidingSkillCoolTimeList(  const UINT uiSlotIndex_, const float fRemainCoolTime_ );
	void GetMyRidingSkillCoolTimeListToPacket( OUT map<int, int>& mapSkillCoolTime_ ) const;

	void SaveGageData();
	void RestoreGageData();

	void UpdatePartyMemberGageData( const UidType uidPartyMember_, const KPartyMemberStatus& kPartyMemberStatus_ );

	void PushBuff( UidType uiUnitUID, const CX2GageUI::BuffIcon& BuffIconInfo_, bool bIsDeBuff );
	void EraseBuff( UidType uiUnitUID, BUFF_TEMPLET_ID eBuffID_, bool bIsDeBuff);
	void ClearBuffList(UidType uiUnitUID);
	void NotifyDurationTime5sec(UidType uiUnitUID, BUFF_TEMPLET_ID eBuffID_);
	void SetShowBuffIcon( bool bIsShow_ );
	void InitBuffIconFlicker( UidType uiUnitUID, BUFF_TEMPLET_ID eBuffID_ );
	void SetDurationTime( UidType uiUnitUID, BUFF_TEMPLET_ID eBuffID_, int iTime_);

	void GetBuffFactorFromGameUnit( const CX2GameUnit* pGameUnit_ );
	const vector<KBuffFactor> GetBuffFactor() const { return m_vecTempBuffFactor; }
	void ClearTempBuffFactor() { m_vecTempBuffFactor.clear(); }

	void UpdatePartyMemberLevel( const UidType uiUnitUID_, const UINT uiLevel_ );

	void InsertPvpMemberUI( const CX2Room::RoomNpcSlot& npcSlotData_, CX2GUNPC* pGameUnitPartyMember_ );
	void InsertPvpMemberUI( const CX2Room::SlotData& slotData_, CX2GUUser* pGameUnitPartyMember_ );
	void RemovePvpMemberUIByUserUid( const UidType uid_ );
	void ClearPvpMemberUI();

	void UpdatePvpMemberUIPosition(  bool bForce_ = false );
	void UpdatePvpMemberGageData( const UidType uidPartyMember_, CX2GameUnit* pGameUnitPartyMember_ );

	const UidType GetPartyGageUIPositionIndexByUserList( int iIndex ) const;

	bool GetIsExsitancePartyMemberHPLessThanPer( float fHPRate_ );  //체력 @1 % 이하인 파티원이 존재 여부 체크

	bool IsNotUseNotifyTimeBuff( BUFF_TEMPLET_ID eBuffID_ );
protected:

	CX2GageManager() : m_ElapsedTimeCheckVillageBuff( 1.0f ), m_pGageDataForRestorationToPvpGame( NULL )
	, m_bMyUserIsFaultyPlayer( false )	//// Player 가 불량 유저라면 true, 정상 유저라면 false
 {}
	~CX2GageManager();
private:
	static CX2GageManager*	m_pGageManager;
	CX2GageSetPtr			m_ptrMyGageSet;
	vector<CX2GageSetPtr>	m_vecGageSetPartyMember;
	vector<CX2BossGageUIPtr> m_vecBossGageUIList;

	vector<CX2GageSetPtr>	m_vecGageSetPvpMyTeam;
	vector<CX2GageSetPtr>	m_vecGageSetPvpOtherTeam;

	vector<float>			m_vecSkillCoolTime;
	map<int, int>			m_mapQuickSlotCoolTime;
	vector<float>			m_vecRidingPetSkillCoolTime;
	CKTDXCheckElapsedTime	m_ElapsedTimeCheckVillageBuff;

	CX2GageData*			m_pGageDataForRestorationToPvpGame;		/// PVP 게임 입장 시 HP 등을 초기화 하는데, 다시 필드나 마을로 입장했을 때 원래의 Status를 복구시켜 주기 위한 용도
	vector<float>			m_vecTempSkillCoolTime;
	vector<KBuffFactor>		m_vecTempBuffFactor;

	bool						m_bMyUserIsFaultyPlayer;		// Player 가 불량 유저라면 true, 정상 유저라면 false

};










//////////////////////////////////////////////////////////////////////////////////////////////////
class CX2OldGageManager
{
	public:

		enum GAGE_UI_TYPE
		{
			GUT_NONE		= 0,
			GUT_SMALL_GAGE,
			GUT_MY_STATUS_GAGE,
		};

		enum PVP_GAME_UI_STAR
		{
			PGUS_WHITE = 0,
			PGUS_YELLOW,
			PGUS_BLUE,
			PGUS_RED,
			PGUS_RAINBOW,
		};

		//{{ kimhc // 2010.12.3 // 2010-12-23 New Character CHUNG
		enum PVP_GAME_UI_CANNON_BALL
		{
			PGUCB_STACK_01 = 0, // 가장 아래에 있는 스택
			PGUCB_STACK_02,
			PGUCB_STACK_03,
			PGUCB_STACK_04,
			PGUCB_STACK_05,
			PGUCB_STACK_06,		// 가장 위에 있는 스택
		};
//}} kimhc // 2010.12.3 //  2010-12-23 New Character CHUNG

	public:
		struct GageFactor
		{
			float	fTime;
			float	fFactor;

			GageFactor()
			{
				fTime = 0.f;
				fFactor = 0.f;
			}
		};

		// TODO: Gage 구조체를 GageManager 클래스를 제외하고는 직접 참조할 수 없게 다 막자
		struct Gage
		{

			bool					bShow;

			KProtectedType<float>	fNow;
			KProtectedType<float>	fMax;

		public:
			KProtectedType<float>	fChangeRate;

			std::vector<GageFactor> vecFactor;



		public:
			void Init()
			{
				bShow		= true;
				fNow		= 0.0f;
				fMax		= 0.0f;
				fChangeRate	= 0.0f;
				vecFactor.clear();
			}

			Gage()
			{
				Init();
			}
			Gage(Gage *pGage)
			{
				if( pGage == NULL )
				{
					Init();
				}
				else
				{
					bShow = pGage->bShow;
					fNow = pGage->fNow;
					fMax = pGage->fMax;
					fChangeRate = pGage->fChangeRate;
					vecFactor.clear();
					vecFactor = pGage->vecFactor;
				}
			}

			void AddFactor(float fFactor, float fTime)
			{
				GageFactor gageFactor;
				gageFactor.fFactor = fFactor;
				gageFactor.fTime = fTime;

				vecFactor.push_back(gageFactor);
			}

			void ClearFactor()
			{
				vecFactor.clear();
			}




			void Increase( const float increment, const float minimum = 0.f );

		};

		/*
		//{{ kimhc // 2010.12.12 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG

		class CCannonBallUI
		{
		private:
			CCannonBallUI()
				: NUM_OF_CANNON_BALL_PER_VERTICAL(6), m_byNumOfVerticalCannonBallUI( 0 ),
				m_iCannonBallCount(0), m_iMaxCannonBallCount(0), m_bCannonBallChanged(false),
				m_bBerserkModeChanged(false), m_bBerserkMode( false )
			{}

		private:
			CKTDGUIDialogType		m_pDLGMyUnit;						/// GageManager의 m_pDlgMyUnit
			const int				NUM_OF_CANNON_BALL_PER_VERTICAL;	/// 캐논볼 UI 한개의 세로줄(Vertical) 당 6개의 캐논볼을 가지고 있음
			BYTE					m_byNumOfVerticalCannonBallUI;		/// 캐논볼UI Vertical의 갯수

			KProtectedType<int>		m_iCannonBallCount;					/// 청의 캐논볼이 차지된 양
			int						m_iMaxCannonBallCount;				/// 최대로 차지되는 캐논볼의 양

			bool					m_bCannonBallChanged;				/// 캐논볼의 양이 변화되었으면 true
			bool					m_bBerserkModeChanged;				/// 광폭화 모드로 또는 광폭화 모드에서 변했는가?
			bool					m_bBerserkMode;						/// 광폭화 모드인가?

		public:
			static std::shared_ptr<CCannonBallUI> CreateCannonBallUIPtr()
			{
				std::shared_ptr<CCannonBallUI> pCannonBallPtr( new CCannonBallUI );
				return pCannonBallPtr;
			}

			CKTDGUIDialogType GetDLGMyUnit() const { return m_pDLGMyUnit; }
			void SetDLGMyUnit( CKTDGUIDialogType pDLGMyUnit_ ) { m_pDLGMyUnit = pDLGMyUnit_; }

			int GetCannonBallCount() const { return m_iCannonBallCount; }
			void SetCannonBallCount( int iCannonBallCount_ )
			{
				if ( m_iMaxCannonBallCount < iCannonBallCount_ )
					m_iCannonBallCount = m_iMaxCannonBallCount;
				else if ( 0 > iCannonBallCount_ )
					m_iCannonBallCount = 0;
				else
					m_iCannonBallCount = iCannonBallCount_;

				SetCannonBallChanged( true );
			}

			void SetMaxCannonBallCount( int iMaxCannonBallCount_ ) { m_iMaxCannonBallCount = iMaxCannonBallCount_; }
			int	 GetMaxCannonBallCount() const { return m_iMaxCannonBallCount; }

			bool IsFullCannonBallCount() const { return GetCannonBallCount() == m_iMaxCannonBallCount; }
			bool IsEmptyCannonBallCount() const { return GetCannonBallCount() == 0; }

			// 캐논볼 Verticla(6개 묵음)의 갯수
			BYTE GetNumOfVerticalCannonBallUI() const { return m_byNumOfVerticalCannonBallUI; }
			void SetNumOfVerticalCannonBallUI( BYTE byNumOfVerticalCannonBallUI_ ) { m_byNumOfVerticalCannonBallUI = byNumOfVerticalCannonBallUI_; }

			bool GetCannonBallChanged() const { return m_bCannonBallChanged; }
			void SetCannonBallChanged(bool bCannonBallChanged_) { m_bCannonBallChanged = bCannonBallChanged_; }

			CX2Unit::UNIT_TYPE GetUnitType();

			void UpdateNumOfVerticalCannonBallUI();
			void InitializeCannonBallUI();
			void CannonBallUIFrameMove();
			void CannonBallUIPerVerticalUIFrameMove( CKTDGUIStatic* pStaticVertical_, int iStaticIndex_ );

			bool IsBerserkMode() const { return m_bBerserkMode; }
			void SetBerserkMode( bool bBerserkMode_ ) { m_bBerserkMode = bBerserkMode_; }

			bool GetBerserkModeChanged() const { return m_bBerserkModeChanged; }
			void SetBerserkModeChanged( bool bBerserkModeChanged_ ) { m_bBerserkModeChanged = bBerserkModeChanged_; }

		};

		typedef std::shared_ptr<CCannonBallUI> CCannonBallUIPtr;
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.12 //  2010-12-23 New Character CHUNG
		*/
	public:
		CX2OldGageManager();
		~CX2OldGageManager(void);

		void			Init( CX2GameUnit* pGameUnit, const GAGE_UI_TYPE eGageUiType_ );
		void			SetHPTexture( const WCHAR* pTexName_ );
		void			SetMPTexture( const WCHAR* pTexName_ );

		void			SetEnable( bool bEnable ){ m_bEnable = bEnable; }
		void			SetShow( bool bShow );

		void			SetAlpha( const float fAlpha_ )
		{
			if ( NULL != m_pGageUI )
				m_pGageUI->SetAlpha( fAlpha_ );
		}

		static void		ResetTeamIndex( int myTeamIndex = 0, int otherTeamIndex = 3 ) { 	m_MyTeamIndex = myTeamIndex;
									m_OtherTeamIndex = otherTeamIndex; }		//모든 게이지 매니져 렌더 for문 돌리기 이전 셋팅해줘야함..
																				//기본 pvp대전에서 4:4할 때.. 디폴드 값 설정
		virtual void	OnFrameMove( double fTime, float fElapsedTime );
		virtual void	OnFrameRender();

		int				GetIndex() { return m_Index; } // 게이지매니져에서 따로 관리하는 인덱스..
		bool			IsMyUnit();

		void			SetCanChargeMP( const bool bCanChargeMp_ ) { m_bCanChargeMP = bCanChargeMp_; }
		bool			DamageFlush();
		bool			FlushMP( float fMP, bool bForce = true );

		void			UpHP( float fHP, const float hpMinimum = 0.f );
		void			UpMP( float fMP ); // TODO: CX2GameUnit 클래스외에 이 함수를 직접 호출하는 부분을 모두 CX2GameUnit::UpMP()를 호출하도록 수정하기
		void			UpSoul( float fSoul );
		bool			FlushSoul( float fSoul );

		void			AddMPFactor( const float factor, const float time ) { m_pMPGage->AddFactor( factor, time ); }
		void			ClearMPFactor() { m_pMPGage->ClearFactor(); }

		void			SetNowHP( float fHPNow ) { m_pHPGage->fNow = fHPNow; }
		void			SetNowMP( float fMPNow ) { m_pMPGage->fNow = fMPNow; }
		void			SetNowMPCharge( float fMPChargeNow ) { m_MPChargeGage.fNow = fMPChargeNow; }

		void			SetHPChangeRate( const float hpChangeRate ) { m_pHPGage->fChangeRate = hpChangeRate; }
		void			SetMPChangeRate( const float mpChangeRate ) { m_pMPGage->fChangeRate = mpChangeRate; }
		float			GetMPChangeRate() { return m_pMPGage->fChangeRate; }

		void			SetMPChargeChageRate( const float mpChargeChangeRate ) { m_MPChargeGage.fChangeRate = mpChargeChangeRate; }

		float			GetNowHp() const { return m_pHPGage->fNow; }
		float			GetNowMP() const{ return m_pMPGage->fNow; }



		void			SetNowSoul( const float soul ) { m_SoulGage.fNow = soul; }
		float			GetNowSoul(){ return m_SoulGage.fNow; }

		void			SetMaxHP( float fHPMax ) { m_pHPGage->fMax = fHPMax; }
		float			GetMaxHp() const { return m_pHPGage->fMax; }

		void			SetMaxMP( const float maxMP ) { m_pMPGage->fMax = maxMP; }
		float			GetMaxMp() const { return m_pMPGage->fMax; }

		void			SetShowMPGage( bool bShow ) { m_pMPGage->bShow = bShow; }
		bool			GetShowMPGage() { return m_pMPGage->bShow; }


		float			GetMaxSoul(){ return m_SoulGage.fMax; }
		void			SetMaxSoul( const float maxSoul ) { m_SoulGage.fMax = maxSoul; }


		Gage*			GetHPGage(){ return m_pHPGage; }
		Gage*			GetMPGage(){ return m_pMPGage; }
		Gage*			GetMPChargeGage(){ return &m_MPChargeGage; }
		Gage*			GetSoulGage(){ return &m_SoulGage; }


		Gage*			GetForceDownGage(){ return &m_ForceDownGage; }
		void			AddForceDownGage( float fDelay ) // TODO : rename 하자, UpHP() 등의 함수와 이름을 일관되게
		{
			m_ForceDownGage.fNow += fDelay;
			if( m_ForceDownGage.fNow > m_ForceDownGage.fMax )
				m_ForceDownGage.fNow = m_ForceDownGage.fMax;
			if( m_ForceDownGage.fNow < 0.f )
				m_ForceDownGage.fNow = 0.f;
		}

		float			GetNowForceDownGage(){ return m_ForceDownGage.fNow; }
		void			SetNowForceDownGage( const float value ) { m_ForceDownGage.fNow = value; }

		void			SetMaxForceDownGage( const float value ) { m_ForceDownGage.fMax = value; }


		bool			FlushForceDownGage()
		{
			if( m_ForceDownGage.fNow == m_ForceDownGage.fMax )
			{
				m_ForceDownGage.fNow = 0.0f;
				return true;
			}
			return false;
		}

		void			DamagedFaceDraw( int unitClass );
		bool			OpenScript( const WCHAR* pFileName );

		void			UIFrameMove( double fTime, float fElapsedTime );

		void			SetHasBossGage( bool bHasBossGage ) { m_bHasBossGage = bHasBossGage; }
		void			CreateBossGage( const WCHAR* wszFaceTexName, const WCHAR* wszFaceTexPieceName );
		bool			GetHasBossGage() { return m_bHasBossGage; }
		float			GetRemainBossGageTime() { return m_fRemainBossGageTime; }
		void			SetRemainBossGageTime( float fRemainTime ) { m_fRemainBossGageTime = fRemainTime; }

		void			CrashMyStateGageUI( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor );


		void			SetPingLevel( int iLevel );

		void			TwinkleMPGage( float fTime );

		void			UpdateMyManaGuageUI();		// 최대 마나량이 변할 수 있으므로 마나 100 위치에 점을 찍어주는 역할을 한다
		void			SetChangeRateByItem(float fRate, float fTime)
		{
			m_fChangeRateByItem = fRate;
			m_fChangeRateTimeByItem = fTime;
		}
		void			SetEnableHPRelativeChangeRate( bool bVal ){ m_bHPRelativeChangeRate = bVal; }

/*
//{{ kimhc // 2010.12.3 // 2010-12-23 New Character CHUNG
#ifdef	NEW_CHARACTER_CHUNG
		CCannonBallUIPtr GetCannonBallUIPtr() { return m_pCannonBallUIPtr; }
		void CreateCannonBallUI();
		void CannonBallUIFrameMove( double fTime_, float fElapsedTime_ );
#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.3 //  2010-12-23 New Character CHUNG
*/
		void GetBerserkModeMyStateImage( OUT wstring& wstrFileName, OUT wstring& wstrPieceName, IN const CX2Unit::UNIT_CLASS eClassType );

		//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
		void SetBossGaugeNumber( IN u_short usNumber_ ) { m_usGaugeNumber = usNumber_; }
		//}} JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
		void StopUIEffectElswordWS();
		void PlayUIEffectDestruction();
		void PlayUIEffectVigor();
		void InitUIElswordWSP( bool bEnable_ );
		void UIFrameMoveElswordWSP();
		void UpdateUIElswordWSP(int iWayOfSwordState ) {};

		void SwapGage()
		{
			Gage *pGage = new Gage(m_pHPGage);
			SAFE_DELETE(m_pHPGage);
			m_pHPGage = pGage;
			pGage = new Gage(m_pMPGage);
			SAFE_DELETE(m_pMPGage);
			m_pMPGage = pGage;

			m_fSwapGageTime = 0.f;
		}

	private:
		void					CheckMaxHP();
		void					CheckMaxMP();

		void					CreateGageUI( const GAGE_UI_TYPE eGageUiType_ );
		//int						GetMyGagePictureIndexByClass( CX2Unit::UNIT_CLASS eUnitClass );

		bool					m_bEnable;
		bool					m_bShow;
		float					m_fElapsedTime;

		Gage					*m_pHPGage;
		Gage					*m_pMPGage;
		Gage					m_MPChargeGage;
		Gage					m_SoulGage;

		Gage					m_ForceDownGage;

		bool					m_bCanChargeMP;			// 기폭

		CKTDGUIDialogType			m_pDLGOtherUnit;
		CKTDGUIDialogType			m_pDLGBossGage;


		CX2GameUnit*			m_pGameUnit;

		static int				m_MyTeamIndex;				// UI 용
		static int				m_OtherTeamIndex;			// UI 용

		int						m_Index;					// UI 용

		bool					m_bInit;						// UI 용


		bool					m_bHasBossGage;					// UI 용
		float					m_fRemainBossGageTime;			// UI 용


		bool					m_bMPLevel1;
		bool					m_bMPLevel2;
		bool					m_bMPLevel3;

		// 밑의 5개는 핵 방지용
		KProtectedType<int>		m_OneSkillPlayCount;
		KProtectedType<int>		m_TwoSkillPlayCount;
		KProtectedType<int>		m_ThreeSkillPlayCount;
		CKTDXTimer				m_TimerCheckSkillPlayCount;
		static bool				m_sbSendReportHackUserPacket; // 소스 합칠 때 편하게 하기 위해 임시로 넣은 코드임 x2data나 x2main쪽으로 빼야함.

		KProtectedType<float>	m_fChangeRateByItem;		/// 소인의 비약 인듯.
		KProtectedType<float>	m_fChangeRateTimeByItem;

		bool					m_bHPRelativeChangeRate;	/// NPC 중에 ??

/*
		//{{ kimhc // 2010.12.3 // 2010-12-23 New Character CHUNG
	#ifdef	NEW_CHARACTER_CHUNG
		CCannonBallUIPtr		m_pCannonBallUIPtr;
	#endif	NEW_CHARACTER_CHUNG
		//}} kimhc // 2010.12.3 //  2010-12-23 New Character CHUNG
*/

		CKTDGUIDialogType			m_pDlgAirGage;

		//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP Bar 여러 개로 구현
		u_short	m_usGaugeNumber;	/// 서버에서 받는 HP Bar 개수

		wstring m_wstrHPBar[7];		/// HP Bar 색상 키 값
		//}} JHKang / 강정훈 / 2011/01/26 / 보스 HP Bar 여러 개로 구현

		float	m_fSwapGageTime;

		CX2GageUI*				m_pGageUI;

	private:
		void SetNumberIcon( CKTDGUIControl::CPictureData* const pPictureData, int num );
		CKTDGUIStatic* m_pStaticElswordVigor;
		CKTDGUIStatic* m_pStaticElswordDest;


};


IMPLEMENT_INTRUSIVE_PTR( CX2GageManager::CX2GageSet );
