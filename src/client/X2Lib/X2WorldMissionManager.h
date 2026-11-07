#pragma once


#define MAX_CRYSTAL_COUNT	5

class CX2UIWorldMission;

class CX2WorldMissionManager
{
public:
	~CX2WorldMissionManager();
	CX2WorldMissionManager();

	void Init();

	bool IsActiveDefenceDungeon() const { return m_bActiveDefenceDungeon; }
	void SetIsActiveDefenceDungeon( bool bActiveDefenceDungeon_ ) { m_bActiveDefenceDungeon = bActiveDefenceDungeon_; }
	//bool IsEXPBonus()		{ return m_bIsEXPBonus; }

//	int GetDungeonTargetCount() { return m_iDungeonTargetCount; }
	int GetNumOfWorldCrystalsProtected() const { return m_iNumOfWorldCrystalsProtected; }
	void SetNumOfWorldCrystalsProtected( int iNumOfWorldCrystalsProtected_ ) { m_iNumOfWorldCrystalsProtected = iNumOfWorldCrystalsProtected_; }

	USHORT GetRemainTimeWorldBuff() const { return m_usRemainTimeWorldBuff; }
	void SetRemainTimeWorldBuff( USHORT usRemainTimeWorldBuff_ ) { m_usRemainTimeWorldBuff = usRemainTimeWorldBuff_; }

	USHORT GetWorldBuffStepNum() const { return m_usWorldBuffStepNum; }
	void SetWorldBuffStepNum( USHORT usWorldBuffStepNum_ ) { m_usWorldBuffStepNum = usWorldBuffStepNum_; }

//	char GetNeedPartyNum() { return m_cNeedPartyNum; }

	void RenderMiniMapTexture( CKTDXDeviceRenderTargetTexture* 	pRenderTargetTexture );
	void RenderCrystal( int iPosX_, int iPosY_, const CX2GUNPC* pCrystal_, CKTDXTimer& tTimer_ );

	CX2UIWorldMission *GetUIWorldMission() const { return m_pUIWorldMission; }

	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	HRESULT OnFrameMove( double fTime, float fElapsedTime );

	bool Handler_EGS_WORLD_MISSION_UPDATE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_UPDATE_WORLD_BUFF_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	CKTDXDeviceTexture* GetTextureDefenceDungeonMiniMap() const { return m_pTextureDefenceDungeonMiniMap; }
	void SetTextureDefenceDungeonMiniMap( IN CKTDXDeviceTexture* pTextureDefenceDungeonMiniMap_ ) { m_pTextureDefenceDungeonMiniMap = pTextureDefenceDungeonMiniMap_; }

	const CKTDXDeviceTexture::TEXTURE_UV* GetUVDefenceDungeonMiniMap() const { return m_pUVDefenceDungeonMiniMap; }
	void SetUVDefenceDungeonMiniMap( const CKTDXDeviceTexture::TEXTURE_UV* pUVDefenceDungeonMiniMap_ ) { m_pUVDefenceDungeonMiniMap = pUVDefenceDungeonMiniMap_; }

	void SetDefenceDungeonMiniMap( const wstring& wstrTextureName, const wstring& wstrKeyName );

	int GetNumOfCrystals() const { return m_iNumOfCrystals; }
	void SetNumOfCrystals( int iNumOfCrystals_ ) { m_iNumOfCrystals = iNumOfCrystals_; }

	// 드래곤의 숨결
	bool GetDragonBreathEnchantStatIncreased( OUT const CX2Stat::EnchantStat*& pEnchantStatInreased_ ) const;
	int GetDragonBreathAnimSpeedRateIncreased() const;
	float GetDragonBreathAttackStatRateIncreased() const;

	float GetEnchantStatFromDrgonBreath( int iEnchantType ) const;
	bool IsNullWorldBuffDragonBreath() const { return (NULL == m_pWorldBuffDragonBreath ? true : false); }

	// 엘의 축복
	int GetElBlessCriticalRateIncreased() const;
	float GetElBlessDefenceStatRateIncreased() const;

	bool IsNULLWorldBuffElBless() const { return (NULL == m_pWorldBuffElBless ? true : false); }

	int GetRemainTimeToFinish() const { return m_iRemainTimeToFinish; }
	void SetRemainTimeToFinish( const int iRemainTime_ ) { m_iRemainTimeToFinish = iRemainTime_; }

	int GetRemainTimeMissionStart() const { return m_iRemainTimeMissionStart; }
	void SetRemainTimeMissionStart( const int iRemainTimeMissionStart_ ) { m_iRemainTimeMissionStart = iRemainTimeMissionStart_; }

	void GetDefenceDungeonIcnoDesc( OUT wstringstream& wstrStreamDungeonIconDesc_ );
	//const wstring& GetDefenceDungeonIconDesc() const;
	//void LoadScript();

	//현재 어둠의 문 난이도
	int GetNowDefenceDungeonLevel() const { return m_iNowDefenceDungeonLevel; }
	void SetNowDefenceDungeonLevel( const int iNowDefenceDungeonLevel_ ) { m_iNowDefenceDungeonLevel = iNowDefenceDungeonLevel_; }
	void ChangeDefenceDungeonLevel();	//지켜낸 수정의 갯수에 따라서 어둠의 문 난이도 값을 설정한다.




protected:
	void CreateWorldBuffDragonBreath();
	void DeleteWorldBuffDragonBreath();

	void CreateWorldBuffElBless();
	void DeleteWorldBuffElBless();

private:
	// 월드버프 2단계, 드래곤의 숨결
	struct WORLD_BUFF_DRAGON_BREATH
	{
		CX2Stat::EnchantStat	m_EnchantStatIncreased;
		int						m_iAnimSpeedRateIncreased;
		float					m_fAttackStatRateIncreased;

		WORLD_BUFF_DRAGON_BREATH( const float fEnchantStatIncreased_, const int iAnimSpeedRateIncreased_, const float fAttackStatRateIncreased_ )
			: m_EnchantStatIncreased( fEnchantStatIncreased_ ), m_iAnimSpeedRateIncreased( iAnimSpeedRateIncreased_ ), m_fAttackStatRateIncreased( fAttackStatRateIncreased_ )
		{}
	};

	// 월드버프 3단계, 엘의 축복
	struct WORLD_BUFF_EL_BLESS
	{
		int						m_iCriticalRateIncreased;
		float					m_fDefenceStatRateIncreased;

		WORLD_BUFF_EL_BLESS( const int iCriticalRateIncreased_, const float fDefenceStatRateIncreased_ )
			: m_iCriticalRateIncreased( iCriticalRateIncreased_ ), m_fDefenceStatRateIncreased( fDefenceStatRateIncreased_ )
		{}
	};

	bool			m_bActiveDefenceDungeon;			///	현재 던전 개방되어 있는지 여부

	int				m_iNumOfWorldCrystalsProtected;		///	월드 내에서 지켜진 엘의 갯수

	int				m_iRemainTimeToFinish;				///	남은 시간
	USHORT			m_usRemainTimeWorldBuff;			/// 월드 버프 남은 시간
	USHORT			m_usWorldBuffStepNum;				/// 현재 진행 중인 월드 버프 단계
	int				m_iRemainTimeMissionStart;			/// 미션 시작시 까지 남은 시간 (분 단위, 오늘 열릴 예정이 더이상 없는 경우 -1)

	CKTDXTimer		m_timerMissionToFinish;				///	미션 진행시간
	CKTDXTimer		m_timerWorldBuff;					///	월드 버프 진행 시간
	CKTDXTimer		m_timerMissionToStart;				/// 미션이 시작될 때 까지의 남은 시간

	//bool			m_bIsEXPBonus;						///	보너스 경험치 여부

	CKTDXTimer		m_timerDamaged[MAX_CRYSTAL_COUNT];

	CX2UIWorldMission	*m_pUIWorldMission;

	// 아래 두개를 CX2UIWorldMission으로 넣는 방법도 고려해 보자
	CKTDXDeviceTexture*	m_pTextureDefenceDungeonMiniMap;			/// 디펜스 던전 미니맵 텍스쳐 포인터
	const CKTDXDeviceTexture::TEXTURE_UV* m_pUVDefenceDungeonMiniMap;		/// 디펜스 던전 미니맵 텍스쳐 UV 포인터

	const int m_iStartKeyNumberForCrystal;				/// 엘 크리스탈이 가지고 있는 키코드 중 제일 첫번째 KeyNumber
	int m_iNumOfCrystals;								/// 난이도에 따라 달라지는 크리스탈의 갯수

	WORLD_BUFF_DRAGON_BREATH*	m_pWorldBuffDragonBreath;
	WORLD_BUFF_EL_BLESS*		m_pWorldBuffElBless;

	int							m_iNowDefenceDungeonLevel;	/// 현재 어둠의 문 난이도


	// TEST
// 	int MAGIC_X_LEFT;
// 	int MAGIC_X_RIGHT;
// 	int MAGIC_Z_UP;
// 	int MAGIC_Z_DOWN;
//
// 	int MAGIC_POS_X;
// 	int MAGIC_POS_Y;
//
// 	int MAGIC_WIDTH;
// 	int MAGIC_HEIGHT;
//
// 	int MAGIC_EL_WIDTH;
// 	int MAGIC_EL_HEIGHT;
};
