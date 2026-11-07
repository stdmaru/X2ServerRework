#pragma once

#define D3DFVF_DRAWFACE_RHW_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define ONE_CHARGE 100.0f
#define TWO_CHARGE 200.0f
#define THREE_CHARGE 300.0f
#define FULL_HYPER_COUNT 3	/// 3������ �� �� �ִ� ���� ���� ����


class CX2GameUnit;
class CX2RidingPet;
class CX2GageUI;
typedef boost::intrusive_ptr<CX2GageUI> CX2GageUIPtr;

class CX2GageUI
    : private x2::NonCopyable
{

public:

	// ���� �ϳ��� set(1) �� ���¸� UI�� ������ �ʵ��� ��
// 	enum GAGE_UI_FACTOR_SHOW	/// UI�� ���̰ų� �Ⱥ��̰� �ϴ� ���
// 	{
// 		GUFS_ENABLE_SCREEN_SHOT_MODE = 0,	/// ���� ��忡 ���� ������
// 		GUFS_STATE_CHANGE,					/// ����->�ʵ� �� ���º��濡 ���� �������� ���
// 		GUFS_FAR_DISTANCE,					/// �Ÿ��� �־ �Ⱥ��̴� ���
// 		GUFS_EVE_FLASH_BANG,				/// �̺� �Ϸ�Ʈ�� ��ų (���� �÷���)
// 		GUFS_END,							/// ��ü ���� �� ���� ��Ÿ��
// 	};

	enum PVP_GAME_UI_BAR
	{
		PGUB_MY_MP = 0,
		PGUB_MY_ACTIVE_MP,	/// Detonation(����)
		PGUB_MY_SOUL,		/// HyperGage(����)
	};

	enum PVP_GAME_UI_WAKE_ORB
	{
		PGUWO_ORB1 = 0,
		PGUWO_ORB2,
		PGUWO_ORB3,
		PGUWO_ORB_CHUNG,
		PGUWO_ORB_RAVEN,
		PGUWO_ORB_ARA,
	};

	enum WAY_OF_SWORD_DESTRUCTION_PICTURE
	{
		WOSDP_BG = 0,			// ���
		WOSDP_POINT_BAR,		// ���Ǳ� ��ġ
		WOSDP_DETONATION_CHARGE,// ���� ��ġ
		WOSDP_GAGE_EFFECT,		// ������ ����Ʈ
		WOSDP_EMPTY_BEAD,		// �� ����
		WOSDP_FULL_BEAD,		// �� �� ����
	};




	struct BuffIcon
	{
		BUFF_TEMPLET_ID					eBuffID;
		wstring							szTextureFileName;
		wstring							szTextureKeyName;
		int								iBuffName;
		int								iBuffDesc;
		bool							bNew;

		BuffIcon( BUFF_TEMPLET_ID eBuffID_, const wstring& FileName_, const wstring& KeyName_, const int BuffName_, const int BuffDesc_ ):
		eBuffID(eBuffID_), szTextureFileName(FileName_), szTextureKeyName(KeyName_), iBuffName(BuffName_), iBuffDesc(BuffDesc_), bNew(true)
		{}
	};

public:
	CX2GageUI() : m_fAlpha( 0.0f ), m_bShow( true )
        , m_uRefCount(0)
	{}

	virtual ~CX2GageUI();

	virtual void			InitUI(){};
    virtual void			OnFrameMove( float fElapsedTime_ ) = NULL;
	virtual	void			OnFrameRender() {};

	virtual void			CrashMyStateGageUI( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor ) { ASSERT( !L"Crash Base Function is not available" ); }
	virtual void			SetHpTexture( const WCHAR* pTexName_ ) { ASSERT( !L"Hp Base function is not available"); }
	virtual void			SetMpTexture( const WCHAR* pTexName_ ) { ASSERT( !L"Mp Base function is not available"); }

	virtual bool			GetShow() const { return m_bShow; }
	virtual void			SetShow( const bool bShow_ ) { m_bShow = bShow_; }

	virtual float			GetAlpha() const { return m_fAlpha; }
	virtual void			SetAlpha( const float fAlpha_ ) { m_fAlpha = fAlpha_; }

	//void					SetOwnerGameUnit( CX2GameUnit* pOwnerGameUnit_ ) { m_optrGameUnit = pOwnerGameUnit_; }

	virtual	void			PushBuff( const BuffIcon& BuffIconInfo_, bool bIsDeBuff_){};
	virtual	void			EraseBuff(BUFF_TEMPLET_ID eBuffID_, bool bIsDeBuff_){};
	virtual	void			ClearBuffList(){};
	virtual	void			UpdateBuffIcon(){};
	virtual void			NotifyDurationTime5sec(BUFF_TEMPLET_ID eBuffID_){};
	virtual void			InitBuffIconFlicker(BUFF_TEMPLET_ID eBuffID_) {};

	virtual void SetDurationTime( BUFF_TEMPLET_ID eBuffID_, int iTime_){}

    void    AddRef()    {   ++m_uRefCount; }
    void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

private:

    unsigned                                        m_uRefCount;
	float					m_fAlpha;			/// HP�� MP�� �������� ���(�������, �� �������� ��)
	bool					m_bShow;

	//std::bitset<GUFS_END>		m_bitsetGageUiFactorShow;	/// �Ѱ��� set(1)�� ��Ʈ�� ������ UI�� ������ �ʵ��� ��(ó���� ��� 0���� ����)

};

IMPLEMENT_INTRUSIVE_PTR( CX2GageUI )

#define KIND_OF_HP_GAGE_BAR_COLOR 7
#define MAX_NUM_OF_POST_FIX_COLOR 5

class CX2BossGageUI;
typedef boost::intrusive_ptr<CX2BossGageUI> CX2BossGageUIPtr;

class CX2BossGageUI : public CX2GageUI
{
public:
	CX2BossGageUI( CX2GameUnit* pGameUnit_ );
	virtual ~CX2BossGageUI();

	virtual void			InitUI();
	void					SetBossGageTexture( const WCHAR* wszFaceTexName_, const WCHAR* wszFaceTexPieceName_ );
	void					ResetBossUIPos();
    virtual void			OnFrameMove( float fElapsedTime_ );

	virtual void			SetShow( const bool bShow_ )
	{
		if ( NULL != m_pDLGBossGage )
		{
			CX2GageUI::SetShow( bShow_ );
			m_pDLGBossGage->SetShow( bShow_ );
		}
	}

	USHORT GetNumOfGage() const { return m_usNumOfGage; }
	void SetNumOfGage( const USHORT usNumOfGage_ ) { m_usNumOfGage = usNumOfGage_; }
	void SetNumberIcon( CKTDGUIControl::CPictureData* pPictureData_, const int iNum_ );

	bool IsOwnerGameUnit( const UidType uidBoss_ );
// 	{
// 		if ( null != m_optrGameUnit || uidBoss_ != m_optrGameUnit->GetUnitUID() )
// 			return false;
// 		else
// 			return true;
// 	}
public:
	// �⺻ ����
	const static float MAGIC_LEFT_HP_BAR_LENGTH;// = 11.f;
	const static float MAGIC_RIGHT_HP_BAR_LENGTH;// = 16.f;
	const static float MAGIC_HP_BAR_LENGTH;// = 626.f + MAGIC_LEFT_HP_BAR_LENGTH + MAGIC_RIGHT_HP_BAR_LENGTH;

	const static WCHAR WCHAR_HP_BAR_POST_FIX[KIND_OF_HP_GAGE_BAR_COLOR][MAX_NUM_OF_POST_FIX_COLOR];// = { L"_V", L"_PB" };

private:
	CX2GameUnitoPtr		m_optrGameUnit;
	CKTDGUIDialogType	m_pDLGBossGage;
	vector<wstring>		m_vecHpBarColor;
	//wstring				m_wstrHPBar[KIND_OF_HP_GAGE_BAR_COLOR];		/// HP Bar ���� Ű ��

	USHORT				m_usNumOfGage;		/// �������� ����
	bool				m_bSkillSlotUITypeA;
};

class CX2SmallGageUI : public CX2GageUI
{

public:
	struct DRAWFACE_RHW_VERTEX
	{
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

	CX2SmallGageUI( CX2GameUnit* pGameUnit_ );
	virtual ~CX2SmallGageUI();

    virtual void			OnFrameMove( float fElapsedTime_ );
	virtual	void			OnFrameRender();

	void					SetHpTexture( const WCHAR* pTexName_ );
	void					SetMpTexture( const WCHAR* pTexName_ );

protected:
	void					DrawFace( float fX_, float fY_, float fWidth_, float fHeight_, D3DCOLOR color_ /* = 0xffffffff */);

private:
	void					DrawSmallGage();
	void					DrawHpGage( const D3DXVECTOR3& vOut_ );
	void					DrawMpGage( const D3DXVECTOR3& vOut_ );

	CX2GameUnitoPtr					m_optrGameUnit;
	CKTDGStateManager::KStateID		m_RenderStateID;	/// �������̽� ����� ���� 2D ����Լ�
	CKTDXDeviceTexture*				m_pHPSmallBar;
	CKTDXDeviceTexture*				m_pMPSmallBar;
	float							m_fMPGageAlpha;		/// MP �������� Fade In, Out�� ���
};

class CX2MySmallGageUI : public CX2GageUI
{
public:
	CX2MySmallGageUI( CX2GameUnit* pGameUnit_ );
	virtual ~CX2MySmallGageUI();

	virtual void			InitUI();
    virtual void			OnFrameMove( float fElapsedTime_ );

	virtual bool			GetShow() const;
	virtual void			SetShow( const bool bShow_ );

	void					CreateAirGage();
	void					DestroyAirGage();
	void					UpdateAirGage();

private:
	CX2GameUnitoPtr					m_optrGameUnit;
	CKTDGUIDialogType				m_pDlgAirGage;
};

class CX2StatusGageUI : public CX2GageUI
 {
public:
	CX2StatusGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ )
		: CX2GageUI(), m_pGageData( pGageData_ ), m_eOwnerGameUnitClass( eOwnerUnitClass_ )
	{}

	virtual ~CX2StatusGageUI();

	//virtual void			CrashMyStateGageBar( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor );
	CX2Unit::UNIT_CLASS		GetOwnerUnitClass() const { return m_eOwnerGameUnitClass; }
	void					SetOwnerUnitClass( CX2Unit::UNIT_CLASS eOwnerGameUnitClass_ ) { m_eOwnerGameUnitClass = eOwnerGameUnitClass_; }

	virtual	void			PushBuff( const BuffIcon& BuffIconInfo_, bool bIsDeBuff_);
	virtual	void			EraseBuff(BUFF_TEMPLET_ID eBuffID_, bool bIsDeBuff_);
	virtual	void			ClearBuffList();
	virtual	void			UpdateBuffIcon(){}
	virtual void			NotifyDurationTime5sec(BUFF_TEMPLET_ID eBuffID_);
	virtual void			InitBuffIconFlicker(BUFF_TEMPLET_ID eBuffID_);

protected:
	virtual void			SetCharacterImage( const CX2Unit::UNIT_CLASS eGameUnitClass_ ) = NULL;
	void	UpdateGageForUV( IN CKTDGUIStatic* pStaticBar_, IN int iPicNum, IN const WCHAR* pUVName_,
							 IN float fPercent_, IN bool isWidth_ );

	void	SetBuffIconStatic( const BuffIcon& BuffIconInfo_, int iIndex_, bool bIsDebuff_, const D3DXVECTOR2& vSize_ );

protected:
	CX2GageData*				m_pGageData;
	CX2Unit::UNIT_CLASS			m_eOwnerGameUnitClass;
	CKTDGUIDialogType			m_pDlgBuffIcon;		//����, ����� ������

	//��� ������ �������ֱ� ���� vector�� �۾�
	vector<BuffIcon>			m_vecBuffList;		//���� �ɷ��ִ� ���� ����Ʈ
	vector<BuffIcon>			m_vecDebuffList;	//���� �ɷ��ִ� ����� ����Ʈ
};

class CX2MyGageUI : public CX2StatusGageUI
{
public:

public:
	CX2MyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );
	virtual ~CX2MyGageUI();

	virtual void			SetShow( const bool bShow_ );
	virtual void			InitUI();
	void					InitStatusUI();
	virtual void			InitWakeOrb();

    virtual void			OnFrameMove( float fElapsedTime_ );
	virtual void			CrashMyStateGageUI( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor );
	void					SetOnPopup(bool bVal){m_bOnPopUp = bVal;}
	bool					GetOnPopup() const { return m_bOnPopUp; }
	virtual	void			UpdateBuffIcon();
	void					SetBuffIconGuideDesc();
	virtual void			SetCharacterImage( const CX2Unit::UNIT_CLASS eGameUnitClass_ );
	virtual void			ResetGageUIEtc( const CX2Unit::UNIT_CLASS eGameUnitClass_ ){}

	virtual void SetDurationTime( BUFF_TEMPLET_ID eBuffID_, int iTime_);
protected:
	// ������ // �����ý� ���� ���� ������ ���� �����Լ��� ����
	virtual void			UpdateMpAndHyperAndDetonationGage();
	virtual void			UpdateDetonationGage( CKTDGUIStatic* pStaticStateBar_ );	/// ���� ������

	void					UpdateHyperModeOrb();
	void					UpdateHpGage();
	void					UpdateMpGageAndString( CKTDGUIStatic* pStaticStateBar_ );
	void					UpdateMpGuidePoint();
	virtual void			UpdateHyperGage( CKTDGUIStatic* pStaticStateBar_ );			/// �ֵ��� DP �ý����� ����, ���� �Լ��� ����


	void					UpdateHyperGageString( const float fNowHyper_, const float fMaxHyper_ );
	virtual void			UpdateHyperModeRemainTime();								/// �ֵ��� DP �ý����� ����, ���� �Լ��� ����
	void					UpdatePartyLeaderUI();
	bool					IsMyPlayerFaulty () const { return m_bIsMyPlayerFaulty; }
	void					IsMyPlayerFaulty (bool val_) { m_bIsMyPlayerFaulty = val_; }



	void					UpdateInfoString();

	void				    UpdateTradeBlockDesc( std::wstring& wstrBuffDesc );

protected:
	CKTDGUIDialogType			m_pDLGMyUnit;
	bool	m_bOnPopUp;
	bool	m_bIsShow;
	bool	m_bIsMyPlayerFaulty;

};

class CX2ChungMyGageUI : public CX2MyGageUI
{
public:
	CX2ChungMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );

	virtual void			InitUI();
	virtual void			InitWakeOrb();
	void					InitCannonBallUI();

    virtual void			OnFrameMove( float fElapsedTime_ );

	void					UpdateCharacterImage();
	void					UpdateCannonBallCountUI();

	void					InitNumOfVerticalOfCannonBall( const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );

	bool					GetUseExtraCannonBallGage() const { return m_bUseExtraCannonBallGage; }
	void					SetUseExtraCannonBallGage( const bool bUseExtraCannonBallGage_ ) { m_bUseExtraCannonBallGage = bUseExtraCannonBallGage_; }

	virtual void			ResetGageUIEtc( const CX2Unit::UNIT_CLASS eGameUnitClass_ ){ InitNumOfVerticalOfCannonBall(eGameUnitClass_); InitCannonBallUI();}
protected:
	int						GetNumOfVerticalOfCannonBall() const { return m_iNumOfVerticalOfCannonBall; }
	void					SetNumOfVerticalOfCannonBall( const int iNumOfVerticalOfCannonBall_ ) { m_iNumOfVerticalOfCannonBall = iNumOfVerticalOfCannonBall_; }

	void					GetBerserkModeMyStateImage( OUT wstring& wstrFileName, OUT wstring& wstrPieceName, IN const CX2Unit::UNIT_CLASS eClassType );

private:
	int						m_iNumOfVerticalOfCannonBall;
	bool					m_bUseExtraCannonBallGage;

};

class CX2ElswordMyGageUI : public CX2MyGageUI
{
public:
	CX2ElswordMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ )
		: CX2MyGageUI( pGageData_, eOwnerUnitClass_ ),
		m_pStaticElswordVigor( NULL ), m_pStaticElswordDest( NULL ), m_bShowWspParticle( true )
	{
		m_hSeqVigorEffect[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqVigorEffect[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqVigorEffect[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[0]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[1]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[2]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	}

	virtual ~CX2ElswordMyGageUI();

	virtual void			InitUI();
	virtual void			SetShow( const bool bShow_ );
    virtual void			OnFrameMove( float fElapsedTime_ );

	void InitUIElswordWSP();
	void StopUIEffectElswordWS();
	void PlayUIEffectDestruction();
	void PlayUIEffectVigor();
	void UIFrameMoveElswordWSP();
	void UpdateUIElswordWSP();

protected:
	bool GetShowWspParticle() const { return m_bShowWspParticle; }
	void SetShowWspParticle( const bool bShowWspParticle_ );

private:
	CKTDGUIStatic*										m_pStaticElswordVigor;
	CKTDGUIStatic*										m_pStaticElswordDest;
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqVigorEffect[3];
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqDestEffect[3];

	bool												m_bShowWspParticle;

};

class CX2RavenMyGageUI : public CX2MyGageUI
{
public:
	CX2RavenMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );
	virtual ~CX2RavenMyGageUI();

	virtual void			InitWakeOrb();
    virtual void			OnFrameMove( float fElapsedTime_ );
	void					UpdateHyperOrb();

private:
	CKTDGParticleSystem::CParticleEventSequenceHandle m_hSeqHyperBall;
};

class CX2PartyMemberGageUI : public CX2StatusGageUI
{


public :
	enum BAN_VOTE_BUTTON_TYPE	// ��ǥ ��ư Ÿ��
	{
		BVBT_NONE = 0,  		// ���� ���� ��,
		BVBT_NEED_VOTE,			// ��ǥ �ؾ��ϴ� ����
		BVBT_COMPLETE_VOTE,		// ��ǥ �Ϸ�
//		BVBT_FAULTY_USER_VOTE,	// �ҷ� ���� ����
	};





public:
	CX2PartyMemberGageUI( const KPartyUserInfo& kPartyUserInfo_, CX2GageData* pGageData_, const UINT uiPositionIndex_ )
		: CX2StatusGageUI( pGageData_, static_cast<CX2Unit::UNIT_CLASS>( kPartyUserInfo_.m_cUnitClass ) ),
		m_pDLGOtherUnit( NULL ), m_wstrNickName( kPartyUserInfo_.m_wstrNickName ),
		m_uiLevel( kPartyUserInfo_.m_ucLevel ), m_uidGameUnit( kPartyUserInfo_.m_iUnitUID ),
		m_uiPositionIndex( uiPositionIndex_ ),
		m_iVoteButtonType ( BVBT_NONE ),		// PartyMemberGageUI �� ���� ��ư Ÿ��
		m_pButtonVotePlayer ( NULL ),			// �ش� ��ư�� Dlg ������
		m_pButtonVoteComplete ( NULL ),			// �ش� ��ư�� Dlg ������

		m_bOnPopUp( false )
		, m_bIsSkillUITypeA ( false )
	{}

	virtual ~CX2PartyMemberGageUI();

	virtual void			InitUI();
	void					SetLevelString( const UINT uiLevel_ );
	void					SetNickNameString( const wstring& wstrNickName_ );
	void					SetPosition( const UINT uiPositionIndex_ );

	virtual void			SetShow( const bool bShow_ );

    virtual void			OnFrameMove( float fElapsedTime_ );

	virtual void			CrashMyStateGageUI( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor ) { ASSERT( !L"Crash Base Function is not available" ); }
	virtual void			SetHpTexture( const WCHAR* pTexName_ ) { ASSERT( !L"Hp Base function is not available"); }
	virtual void			SetMpTexture( const WCHAR* pTexName_ ) { ASSERT( !L"Mp Base function is not available"); }

	void					UpdateGageDataFromGameUnit();
	void					UpdateResurrectShortKey();
	void					UpdatePartyLeaderUI();

	UINT					GetPositionIndex() const { return m_uiPositionIndex; }
	void					SetPositionIndex( const UINT uiPosIndex_ )
	{
		m_uiPositionIndex = uiPosIndex_;
		if( NULL != m_pButtonVotePlayer )
		{
			if( false == m_pButtonVotePlayer->SetDummyInt(0, m_uiPositionIndex ) )
				m_pButtonVotePlayer->AddDummyInt( m_uiPositionIndex );
		}
	}

	//CX2Unit::UNIT_CLASS GetOwnerUnitClass() const { return m_eOwnerGameUnitClass; }
	//void SetOwnerUnitClass( CX2Unit::UNIT_CLASS eOwnerGameUnitClass_ ) { m_eOwnerGameUnitClass = eOwnerGameUnitClass_; }

	bool					GetOnPopup() const { return m_bOnPopUp; }

	virtual		void		UpdateBuffIcon();

	void					UpdateFaultyPlayerUI ( const bool b_faultyValue_ );											// ��ǥ ��ư�� UI �� Update ��
	void					SetShowRelationVoteButtons ( const bool bVote_, const bool bComplete_ );		// ��ǥ ��ư�� ��ǥ �Ϸ� ��ư�� �����ֱ� ������ ��
	BAN_VOTE_BUTTON_TYPE	GetVoteButtonType () const { return m_iVoteButtonType; }				// ��ǥ ��ư�� ���¸� ��ȯ
	void					SetVoteButtonType ( const BAN_VOTE_BUTTON_TYPE iVoteButtonType_ ) { m_iVoteButtonType = iVoteButtonType_; }	// ��ǥ ��ư�� ���¸� ����
	CKTDGUIButton *			GetVoteReadyButton () const;				// ��ǥ �غ� ��ư�� ��ȯ
	CKTDGUIButton *			GetVoteCompleteButton () const { return m_pButtonVoteComplete; }		// ��ǥ �Ϸ� ��ư�� ��ȯ
	const wstring &			GetNicknameThisPartyMemeber() const { return m_wstrNickName; }		// �ش� GageUI �� ���� �ִ� �������� ���̵� ��ȯ��
	const UidType &			GetUidTypeThisPartyMember () const { return m_uidGameUnit; }			// �ش� GageUI �� ���� �ִ� �������� UID �� ��ȯ��


protected:
	virtual		void CX2PartyMemberGageUI::SetCharacterImage( const CX2Unit::UNIT_CLASS eGameUnitClass_ );



protected:
	CKTDGUIDialogType		m_pDLGOtherUnit;

private:
	wstring					m_wstrNickName;
	UINT					m_uiLevel;
	UidType					m_uidGameUnit;
	UINT					m_uiPositionIndex;

	bool	m_bOnPopUp;
	bool					m_bIsSkillUITypeA;

	CKTDGUIButton *			m_pButtonVotePlayer;			// �����ϱ� ��ư
	CKTDGUIButton *			m_pButtonVoteComplete;			// ����Ϸ� ��ư
	BAN_VOTE_BUTTON_TYPE	m_iVoteButtonType;				// ���� ��ư�� Ÿ��, 0 = �ƹ� ���µ� �ƴ�, 1 = ���� ��ư �Է� �޴� ��, 2 = �Է� �ޱ� �Ϸ�, 3 = �ҷ� ���� �Ǻ�


};

class CX2PVPPlayerGageUI : public CX2StatusGageUI
{
public:
	CX2PVPPlayerGageUI( const CX2Room::SlotData& pPvpSlot_, CX2GageData* pGageData_, const UINT uiPositionIndex_, char cRank_ )
		: CX2StatusGageUI( pGageData_, pPvpSlot_.m_pUnit->GetClass() ), m_wstrNickName( pPvpSlot_.m_pUnit->GetNickName() ),
		m_uiLevel( pPvpSlot_.m_pUnit->GetPrevLevel() ), m_uiPositionIndex( uiPositionIndex_ ), m_uiMyTeam( pPvpSlot_.m_TeamNum ),
		m_cRank(cRank_), m_iNpcId(-1)
		, m_bIsSkillUITypeA ( false )
	{}


	CX2PVPPlayerGageUI( const CX2Room::RoomNpcSlot& npcSlotData_, CX2GageData* pGageData_, const UINT uiPositionIndex_, char cRank_ )
		: CX2StatusGageUI( pGageData_, CX2Unit::UC_NONE ), m_wstrNickName( npcSlotData_.m_wstrNpcName ),
		m_uiLevel( -1 ), m_uiPositionIndex( uiPositionIndex_ ), m_uiMyTeam(  CX2Room::TN_BLUE ),
		m_cRank(cRank_), m_iNpcId( npcSlotData_.m_iNpcId )
		, m_bIsSkillUITypeA ( false )
	{}

	virtual ~CX2PVPPlayerGageUI();

	virtual void	InitUI();
	void			SetLevelString( IN const UINT uiLevel_ );
	void			SetNickNameString( IN const wstring& wstrNickName );
	void			SetPosition( IN const UINT uiPositionIndex_ );
	virtual void	SetShow( const bool bShow_ );

    virtual void			OnFrameMove( float fElapsedTime_ );
	void			UpdatePvpRank();

	virtual void	CrashMyStateGageUI( const D3DXVECTOR2& _min, const D3DXVECTOR2& _max, D3DXCOLOR changeColor )
	{
		ASSERT( !L"Crash Base Function is not available" );
	}
	virtual void	SetHpTexture( const WCHAR* pTexName_ )
	{
		ASSERT( !L"Hp Base function is not available");
	}
	virtual void	SetMpTexture( const WCHAR* pTexName_ )
	{
		ASSERT( !L"Mp Base function is not available");
	}

	void	UpdateGageDataFromGameUnit();
	UINT	GetPositionIndex() const
	{
		return m_uiPositionIndex;
	}
	void	SetPositionIndex( const UINT uiPosIndex_ )
	{
		m_uiPositionIndex	= uiPosIndex_;
	}

	void	SetRankImage();

	virtual	void			UpdateBuffIcon();

protected:
	virtual void CX2PVPPlayerGageUI::SetCharacterImage( const CX2Unit::UNIT_CLASS eGameUnitClass_ );


protected:
	CKTDGUIDialogType		m_pDLGOtherUnit;

private:
	wstring		m_wstrNickName;		/// ĳ���͸�
	UINT		m_uiLevel;			/// ����
	UINT		m_uiPositionIndex;	/// ������ ���� �� ī���� ����
	UINT		m_uiMyTeam;			/// RED, BLUE, NONE �� ����
	char		m_cRank;
	int			m_iNpcId;			/// ����NPC�ʻ�ȭ ������ ���� NPCUid
	bool					m_bIsSkillUITypeA;
};

//#pragma region CX2AraMyGageUI
/** @class : CX2AraMyGageUI
	@brief : �ƶ��� ��� UI
	@date  : 2012/11/30
*/
class CX2AraMyGageUI : public CX2MyGageUI
{
public:
	CX2AraMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );

	virtual void	InitUI();
	virtual void	InitWakeOrb();
    virtual void			OnFrameMove( float fElapsedTime_ );

	void	InitForcePowerUI();
	void	UpdateCharacterImage();
	void	UpdateForcePowerUI();

protected:
	void	GetBerserkModeMyStateImage( OUT wstring& wstrFileName, OUT wstring& wstrPieceName, IN const CX2Unit::UNIT_CLASS eClassType );
};
//#pragma endregion Ŭ����

class CX2ElesisMyGageUI : public CX2MyGageUI
{
public:
	CX2ElesisMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ )
		: CX2MyGageUI( pGageData_, eOwnerUnitClass_ ),
		m_pStaticElswordVigor( NULL ), m_pStaticElswordDest( NULL ), m_bShowWspParticle( true )
	{
		m_hSeqVigorEffect[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqVigorEffect[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqVigorEffect[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[0]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[1]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hSeqDestEffect[2]		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	}

	virtual ~CX2ElesisMyGageUI();

	virtual void			InitUI();
	virtual void			SetShow( const bool bShow_ );
    virtual void			OnFrameMove( float fElapsedTime_ );

	void InitUIElswordWSP();
	void StopUIEffectElswordWS();
	void PlayUIEffectDestruction();
	void PlayUIEffectVigor();
	void UIFrameMoveElswordWSP();
	void UpdateUIElswordWSP();

protected:
	bool GetShowWspParticle() const { return m_bShowWspParticle; }
	void SetShowWspParticle( const bool bShowWspParticle_ );

	virtual void UpdateMpAndHyperAndDetonationGage();
	virtual void UpdateDetonationGage( CKTDGUIStatic* pStaticStateBar_ );	/// ���� ������

private:
	CKTDGUIStatic*										m_pStaticElswordVigor;
	CKTDGUIStatic*										m_pStaticElswordDest;
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqVigorEffect[3];
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqDestEffect[3];

	bool												m_bShowWspParticle;

};

class CX2AddMyGageUI : public CX2MyGageUI
{
public:
	CX2AddMyGageUI( CX2GageData* pGageData_, const CX2Unit::UNIT_CLASS eOwnerUnitClass_ );

	virtual void			InitUI();
	virtual void			InitWakeOrb();				/// ���� ���� UI �ʱ�ȭ
	void					InitDPGageUI();				/// DP ������ UI �ʱ�ȭ
	void					InitMutationCountUI();		/// ���� ��ġ UI �ʱ�ȭ
	void					InitFormationModeConsumeDPGuide();	/// ���� ��� ��ȯ�� ǥ�� ���̵�

	virtual void			OnFrameMove( float fElapsedTime_ );

	void					UpdateMutationCountUI();	/// ���� ��ġ UI ����
	virtual void			UpdateHyperGage( CKTDGUIStatic* pStaticStateBar_ );			/// �ֵ��� DP �ý����� ����, ���� �Լ��� ����
	virtual void			UpdateMpAndHyperAndDetonationGage();

	virtual void			ResetGageUIEtc( const CX2Unit::UNIT_CLASS eGameUnitClass_ )
	{
		InitMutationCountUI();
		InitDPGageUI();
	}

protected:
	/// ������ ���� ������ �ʻ�ȭ �̹��� ����
	void					UpdateDPGage();
	void					UpdateDPGuidePoint();

	virtual	void			UpdateHyperModeRemainTime();
};
