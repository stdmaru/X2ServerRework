#pragma once

/* 오현빈 // 2013-05-20
-공존이 축제, 어둠의 문 시작 했을 때 등
 화면 가운데 나타나는 엠블럼을 관리 하는 매니져 클래스입니다.

-기존 코드는 순서를 관리 할 수 없어서,
 같은 시간대에 출력 했을 때, 엠블럼이 겹치는 문제가 있었으며
 상기 문제를 해결 하기 위해 작성한 클래스입니다.
*/
class CX2EmblemManager
{
public:
	enum EMBLEM_ID
	{
		EI_NONE	= -1,
		EI_FESTIVAL = 0,	// 공존의 축제
		EI_DEFENCE_START,	// 어문 침공 시작
		EI_DEFENCE_SUCCESS,	// 성공
		EI_DEFENEC_FAIL,	// 실패
		EI_EVENT_CRAYONPOP,	// 크래용팝 이벤트
		EI_EVENT_ELESIS,	// 엘리시스 이벤트
		EI_WEEKEND_BURNING_EVENT, // 주말 버닝 이벤트
		EI_OPEN_FIELD_BOSS_RAID, // 보스 레이드 오픈
		EI_CLOSE_FIELD_BOSS_RAID,// 보스 레이드 클로즈
// #ifdef WORLD_BUFF_2013_CHRISTMAS 	// 김종훈, 2013년 크리스마스용 월드 버프
		EI_EVENT_2013_CHRISTMAS,
// #endif // WORLD_BUFF_2013_CHRISTMAS	// 김종훈, 2013년 크리스마스용 월드 버프

// #ifdef ADD_2013_CHARACTER_ADD_EVENT		// 2013 애드 추가 기념 버닝 이벤트
		EI_EVENT_BURNING_NEW_CHARACTER_TRIPLE,
		EI_EVENT_BURNING_NEW_CHARACTER_DOUBLE,
// #endif // ADD_2013_CHARACTER_ADD_EVENT	// 2013 애드 추가 기념 버닝 이벤트

// #ifdef FIX_REFORM_ENTRY_POINT_7TH		// 김종훈, 귀환자 엠블렘 출력 부분 Emblem Manager 로 변경
		EI_RETURN_OF_HERO,
// #endif FIX_REFORM_ENTRY_POINT_7TH		// 김종훈, 귀환자 엠블렘 출력 부분 Emblem Manager 로 변경

		EI_NAVER_EVENT,						// 네이버 채널링 이벤트
	};

	enum EMBLEM_POPUP_SOUND_TYPE
	{
		EPST_NONE					= 0,		// 일반 출력
		EPST_RELATIVE_UNIT_TYPE_RANDOM	= 1,	// 클래스에 맞게 출력
	};

	struct EmblemPopupSound
	{

		wstring					m_wstrSoundFileName;
		float					m_fStartTime;
		EMBLEM_POPUP_SOUND_TYPE m_eEmblemPopupSoundType;
		CX2Unit::UNIT_TYPE		m_eUnitType;
		bool					m_bIsPlaying;
		EmblemPopupSound () :
			m_wstrSoundFileName ( L"" ),
			m_fStartTime ( -1.f ),
			m_eEmblemPopupSoundType ( EPST_NONE ),
			m_eUnitType ( CX2Unit::UT_NONE ),
			m_bIsPlaying ( false )
		{ }

		EmblemPopupSound ( wstring wstrSoundFileName, float fStartTime, EMBLEM_POPUP_SOUND_TYPE eEmblemPopupSoundType, CX2Unit::UNIT_TYPE eUnitType ) :
			m_wstrSoundFileName ( wstrSoundFileName ),
			m_fStartTime ( fStartTime ),
			m_eEmblemPopupSoundType ( eEmblemPopupSoundType ),
			m_eUnitType ( eUnitType ),
			m_bIsPlaying ( false )
		{ }

	};





	struct EmblemTemplet
	{
		EMBLEM_ID	m_eEmblemID;			// ID

		bool		m_bIsFirstOnly;			// 매회 첫 캐릭터 접속 시에만 보여 줄 것인가?
		bool		m_bCheckCondition;		// 조건 체크 해서 보여 줄 것인가?(FALSE = 무조건 출력)
		bool		m_bIsPassCondition;		// 조건 체크 통과 여부

		UINT		m_uiShowOrder;			// 출력 순서( 상대적인 순서, 0부터 시작 )

		wstring		m_wstrTextureFileName;	// 출력 텍스쳐 파일 명
		D3DXVECTOR3 m_vecTime;	// x : 출력 지속 시간(페이드 시간 포함)
								// y : 시작 후 알파값 1이 될 때 까지 소요되는 시간
								// z : 종료 전 알파값 0이 될 때 까지 소요되는 시간


		// 엠블럼이 2개의 이펙트로 조합 되는 경우
		wstring		m_wstrTextureFileName2nd;	// 출력 텍스쳐 파일 명

		D3DXVECTOR2	m_vTexture1stCostomPos;
		D3DXVECTOR2	m_vTexture2ndCostomPos;

		bool		m_bShowVilleage;
		bool		m_bShowBattleFIeld;
		bool		m_bShowDungeon;

		bool		m_bIsAlreadyShow;

		vector<EmblemPopupSound>		m_vecPlayEmblemSoundTemplet;		// 첫번째 엠블렘 등장 시, 사운드 파일 이름



		EmblemTemplet():
		m_bIsFirstOnly(false),
		m_bCheckCondition(false),
		m_bIsPassCondition(false),
		m_uiShowOrder(0),
		m_vTexture1stCostomPos(-1.f, -1.f),
		m_vTexture2ndCostomPos(-1.f, -1.f),
		m_bShowVilleage(false),
		m_bShowBattleFIeld(false),
		m_bShowDungeon(false),
		m_bIsAlreadyShow(false)
		{}

		void InitShowState()
		{
			m_bIsAlreadyShow = false;
		}
		void InitIsPassCondition()
		{// 조건 없다면 무조건 true
			if( false == m_bCheckCondition )
				m_bIsPassCondition = true;
			else
			{
				if( true == m_bIsAlreadyShow )
					m_bIsPassCondition = false;
			}
		}
		void GetTexture1stPos( OUT D3DXVECTOR3& vPos) const
		{
			if( m_vTexture1stCostomPos.x > -1.f && m_vTexture1stCostomPos.y > -1.f )
				vPos = D3DXVECTOR3(m_vTexture1stCostomPos.x,m_vTexture1stCostomPos.y,0);
			else
				vPos = D3DXVECTOR3(512,300,0);
		}
		void GetTexture2ndPos( OUT D3DXVECTOR3& vPos) const
		{
			if( m_vTexture2ndCostomPos.x > -1.f && m_vTexture2ndCostomPos.y > -1.f )
				vPos = D3DXVECTOR3(m_vTexture2ndCostomPos.x,m_vTexture2ndCostomPos.y,0);
			else
				vPos = D3DXVECTOR3(512,300,0);
		}
	};

public:
	static CX2EmblemManager* GetInstance();
	static void ResetEmblemManager();
	static void DestroyInstance();

	void OnFrameMove( float fElpaseTime );
	void InitEmbelmPlayInfo( bool bCharChange = false );	// 엠블럼 진행 관련 정보 초기화

	void PlayEmblem( EMBLEM_ID eEmblemID_, bool bReShow_ = false );
	void EndEmblem( EMBLEM_ID eEmblemID_ );
	bool HasEmblem( EMBLEM_ID eEmblemID_ );

	void SetTexture2ndName( EMBLEM_ID eEmblemID_, const wstring& wstrTextureName );


private:
	// 파싱
	bool OpenScriptFile( IN const WCHAR* pFileName_ );
	void ParsingEmblemTemplet( KLuaManager& luaManager );

	// 이펙트 생성
	CKTDGParticleSystem::CParticleEventSequence* CreateEventSequence( const WCHAR* wstrTextureFile_,
		const D3DXVECTOR3& vTime);
	void PlayEmblemEffect( const EmblemTemplet& sEmblemTemplet_ );
	void PlayEmblemOtherProcess( const EmblemTemplet& sEmblemTemplet_ );
	void CheckNextPlayingEmblem();

private:
	CX2EmblemManager();
	~CX2EmblemManager();

	static CX2EmblemManager* m_pEmblemManager;
	std::vector<EmblemTemplet*>	m_vecEmblemTemplet;

	bool m_bHasNextPlayEmblem;	// 다음에 출력 할 엠블럼이 존재 여부
	float m_fWaitTime; // 출력 전 대기 시간
	CKTDGParticleSystem::CParticleEventSequenceHandle m_hCurrentEmblem;
	CKTDGParticleSystem::CParticleEventSequenceHandle m_hCurrentEmblem2nd;
	float m_fSoundPlayingCheckTime;
	vector<EmblemPopupSound>	m_vecNowPlayEmblemSound;
	vector<CKTDXDeviceSound *>	m_vecPlayingEmblemSound;

};
