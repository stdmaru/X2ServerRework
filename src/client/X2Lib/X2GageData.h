#pragma once

namespace _CONST_ELESIS_DETONATION_
{// �����ý� ���� �濡 ���Ǵ� ���
	const float MAX_CHARGE_DETONATION_VALUE = 2000.f; /// �ִ� ���� ������
	const float	CHARGE_RATE_FOR_DETONATION = 3000.f;  /// �ʴ� ���� ������
}

class CX2GageData;
typedef x2::intrusive_ptr<CX2GageData> CX2GageDataPtr;

class CX2ElswordGageData;
class CX2GageData {

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

	// TODO: Gage ����ü�� GageManager Ŭ������ �����ϰ��� ���� ������ �� ���� �� ����
	struct Gage
	{
		bool					bShow;

		KProtectedType<float>	fNow;
		KProtectedType<float>	fMax;
		KProtectedType<float>	fChangeRate;

		std::vector<GageFactor> vecFactor;

	public:
		Gage()
		{
			Init();
		}

		Gage( const Gage *pGage)
		{
			if( NULL == pGage )
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

		Gage( const Gage& rhs_ )
		{
			bShow = rhs_.bShow;
			fNow = rhs_.fNow;
			fMax = rhs_.fMax;
			fChangeRate = rhs_.fChangeRate;
			vecFactor.clear();
			vecFactor = rhs_.vecFactor;
		}

		void operator=( const Gage& rhs_ )
		{
			bShow = rhs_.bShow;
			fNow = rhs_.fNow;
			fMax = rhs_.fMax;
			fChangeRate = rhs_.fChangeRate;
			vecFactor.clear();
			vecFactor = rhs_.vecFactor;
		}

		void Init()
		{
			bShow		= true;
			fNow		= 0.0f;
			fMax		= 1.0f;
			fChangeRate	= 1.0f;
			vecFactor.clear();
		}

		void AddFactor( const float fFactor_, const float fTime_ )
		{
			GageFactor gageFactor;
			gageFactor.fFactor = fFactor_;
			gageFactor.fTime = fTime_;

			vecFactor.push_back(gageFactor);
		}

		void ClearFactor()
		{
			vecFactor.clear();
		}
		void Increase( const float fIncrement, const float fMinimum = 0.f );

	};

	CX2GageData() : m_uRefCount(0), m_pHpGage( NULL ), m_pMpGage( NULL ),
		m_ForceDownGage(), m_ChargeMpGageForDetonation(), m_fSwapGageTime( 0.0f ),
		m_fChangeRateByItem( 0.0f ), m_fChangeRateTimeByItem( 0.0f ), m_fHyperModeRemainTime( 0.0f ),
		m_byHyperModeCount( 0 ), m_bEnable( false ), m_bCanChargeMpForDetonation( false ), m_bHpRelativeChangeRate( false )
		,m_bUpdateMpPoint(true)
		, m_fAddMaxMP( 0.f )
		, m_bIsActivateForceDown(false)
		, m_bIsActivateDetonation(false)
        , m_fNowChargeMpForDetonation(0.f)
	{
		Init();
		m_SoulGage.fMax = 100.0f;
	}

	CX2GageData( const CX2GageData& rhs_ ) :
		m_uRefCount(0), m_pHpGage( new Gage( rhs_.m_pHpGage ) ), m_pMpGage( new Gage( rhs_.m_pMpGage ) ),
		m_SoulGage( rhs_.m_SoulGage ),
		m_ForceDownGage( rhs_.m_ForceDownGage ), m_ChargeMpGageForDetonation( rhs_.m_ChargeMpGageForDetonation ),
		m_fSwapGageTime( rhs_.m_fSwapGageTime ), m_fChangeRateByItem( rhs_.m_fChangeRateByItem ),
		m_fChangeRateTimeByItem( rhs_.m_fChangeRateTimeByItem ), m_fHyperModeRemainTime( rhs_.m_fHyperModeRemainTime ),
		m_byHyperModeCount( rhs_.m_byHyperModeCount ), m_bEnable( rhs_.m_bEnable ),
		m_bCanChargeMpForDetonation( rhs_.m_bCanChargeMpForDetonation ), m_bHpRelativeChangeRate( rhs_.m_bHpRelativeChangeRate )
		,m_bUpdateMpPoint( rhs_.m_bUpdateMpPoint )
		,m_fAddMaxMP( rhs_.m_fAddMaxMP )
		,m_bIsActivateForceDown( rhs_.m_bIsActivateForceDown )
		,m_bIsActivateDetonation( rhs_.m_bIsActivateDetonation )
        , m_fNowChargeMpForDetonation( rhs_.m_fNowChargeMpForDetonation )
	{
	}

	virtual ~CX2GageData()
	{
		SAFE_DELETE( m_pHpGage );
		SAFE_DELETE( m_pMpGage );
	}

	void operator=( const CX2GageData& rhs_ )
	{
		m_bCanChargeMpForDetonation = rhs_.m_bCanChargeMpForDetonation;
		m_bEnable = rhs_.m_bEnable;
		m_bHpRelativeChangeRate = rhs_.m_bHpRelativeChangeRate;
		m_byHyperModeCount = rhs_.m_byHyperModeCount;
		m_ChargeMpGageForDetonation = rhs_.m_ChargeMpGageForDetonation;
		m_fChangeRateByItem = rhs_.m_fChangeRateByItem;
		m_fChangeRateTimeByItem = rhs_.m_fChangeRateTimeByItem;
		m_fHyperModeRemainTime = rhs_.m_fHyperModeRemainTime;
		m_ForceDownGage = rhs_.m_ForceDownGage;
		m_fSwapGageTime = rhs_.m_fSwapGageTime;
		*(m_pHpGage) = *(rhs_.m_pHpGage);
		*(m_pMpGage) = *(rhs_.m_pMpGage);
		m_SoulGage = rhs_.m_SoulGage;
		m_bUpdateMpPoint = rhs_.m_bUpdateMpPoint;
		m_fAddMaxMP = rhs_.m_fAddMaxMP;
		m_bIsActivateForceDown = rhs_.m_bIsActivateForceDown;
		m_bIsActivateDetonation = rhs_.m_bIsActivateDetonation;
		m_fNowChargeMpForDetonation = rhs_.m_fNowChargeMpForDetonation;
	}

	void			Init()
	{
		SAFE_DELETE ( m_pHpGage );
		m_pHpGage = new Gage();

		SAFE_DELETE ( m_pMpGage );
		m_pMpGage = new Gage();
	}

	void SetShow( const bool /*bShow_*/ ) { ASSERT( !L"Gage Data SetShow" ); }
	bool GetShow() const { ASSERT( !L"Gage Data GetShow" ); return false; }
	////////////////// Hp ���� Get/Set ///////////////////////////////////////////
	/////// NowHp
	float			GetNowHp() const { return ( NULL != m_pHpGage ? static_cast<float>( m_pHpGage->fNow ) : 0.0f );	}
	void			SetNowHp( const float fNowHp_ )
	{
		if ( NULL != m_pHpGage )
		{
			if ( 0 > fNowHp_ )
				m_pHpGage->fNow = fNowHp_;
			else if ( fNowHp_ > GetMaxHp() )
				m_pHpGage->fNow = GetMaxHp();
			else
				m_pHpGage->fNow = fNowHp_;
		}
	}

	void			UpNowHp( const float fChangeOfNowHp_, const float fMinimum_ = 0.0f )
	{
		if ( NULL != m_pHpGage )
			m_pHpGage->Increase( fChangeOfNowHp_, fMinimum_ );
	}

	bool			VerifyNowHp()
	{
		if ( NULL != m_pHpGage )
			return m_pHpGage->fNow.Verify();
		else
			return false;
	}

	////// MaxHp
	float			GetMaxHp() const { return ( NULL != m_pHpGage ? static_cast<float>( m_pHpGage->fMax ) : 0.0f );	}
	void			SetMaxHp( const float fMaxHp_ )
	{
		if ( NULL != m_pHpGage )
			m_pHpGage->fMax = fMaxHp_;
	}
	void			UpMaxHp( const float fChangeofMaxHp_ )
	{
		if ( NULL != m_pHpGage )
			m_pHpGage->fMax += fChangeofMaxHp_;
	}

	/////// ChangeRateHp
	float			GetChangeRateHp() const { return ( NULL != m_pHpGage ? static_cast<float>( m_pHpGage->fChangeRate ) : 0.0f );	}
	void			SetChangeRateHp( const float fChangeRateHp_ )
	{
		if ( NULL != m_pHpGage )
			m_pHpGage->fChangeRate = fChangeRateHp_;
	}
	void			UpChangeRateHp( const float fChangeofChangeRateHp_ )
	{
		if ( NULL != m_pHpGage )
			m_pHpGage->fChangeRate += fChangeofChangeRateHp_;
	}

	////////////////// Mp ���� Get/Set ///////////////////////////////////////////
	////// NowMp
	float			GetNowMp() const { return ( NULL != m_pMpGage ? static_cast<float>( m_pMpGage->fNow ) : 0.0f );	}
	void			SetNowMp( const float fNowMp_ )
	{
		if ( NULL != m_pMpGage )
		{
			if ( 0 > fNowMp_ )
				m_pMpGage->fNow = fNowMp_;
			else if ( fNowMp_ > GetMaxMp() )
				m_pMpGage->fNow = GetMaxMp();
			else
				m_pMpGage->fNow = fNowMp_;
		}
	}

	void			UpNowMp( const float fChangeOfNowMp_ )
	{
		if ( NULL != m_pMpGage )
			m_pMpGage->Increase( fChangeOfNowMp_ );
	}

	////// MaxMp
	float			GetMaxMp() const { return ( NULL != m_pMpGage ? static_cast<float>( m_pMpGage->fMax ) : 0.0f ); }
	void			SetMaxMp( const float fMaxMp_ )
	{
		if ( NULL != m_pMpGage )
		{
			if( m_pMpGage->fMax != fMaxMp_ )
			{
				m_pMpGage->fMax = fMaxMp_;
				m_bUpdateMpPoint = true;
			}
		}
	}
	void			UpMaxMp( const float fChangeOfMaxMp_ )
	{
		if ( NULL != m_pMpGage )
		{
			m_pMpGage->fMax += fChangeOfMaxMp_;
			m_bUpdateMpPoint = true;
		}
	}
	/** @function : SetAddMaxMP
		@brief : �ִ� MP���� �߰� ��
				 ��ø �߰� ���� �ʰ� 1ȸ�� �߰� �� �� �ֵ��� ������
		@param : fAddMaxMP_ : �߰��� MP��
	*/
	void SetAddMaxMP( const float fAddMaxMP_ )
	{
		if ( fAddMaxMP_ == m_fAddMaxMP )
			return;

		//������ �߰��ߴ� �ִ� MP���� ���� �� ���ο� �߰� MP������ ����
		float fNowMaxMp = GetMaxMp() - m_fAddMaxMP + fAddMaxMP_;
		SetMaxMp( fNowMaxMp );

		//��ȸ MP �߰����� ���
		m_fAddMaxMP = fAddMaxMP_;
	}

	/////// ChangeRateMp
	float			GetChangeRateMp() const { return ( NULL != m_pMpGage ? static_cast<float>( m_pMpGage->fChangeRate ) : 0.0f );	}
	void			SetChangeRateMp( const float fChangeRateMp_ )
	{
		if ( NULL != m_pMpGage )
			m_pMpGage->fChangeRate = fChangeRateMp_;
	}
	void			UpChangeRateMp( const float fChangeofChangeRateMp_ )
	{
		if ( NULL != m_pMpGage )
			m_pMpGage->fChangeRate += fChangeofChangeRateMp_;
	}

	////////////////// Soul(����) ���� Get/Set ///////////////////////////////////////////
	////// NowSoul
	float			GetNowSoul() const { return m_SoulGage.fNow; }
	void			SetNowSoul( const float fNowSoul_ ) { m_SoulGage.fNow = fNowSoul_; }

	void			UpNowSoul( const float fChangeOfNowSoul_ )
	{
		m_SoulGage.Increase( fChangeOfNowSoul_ );
	}

	////// MaxSoul
	float			GetMaxSoul() const { return m_SoulGage.fMax; }
	void			SetMaxSoul( const float fMaxSoul_ ) { m_SoulGage.fMax = fMaxSoul_; }
	void			UpMaxSoul( const float fChangeOfMaxSoul_ ) { m_SoulGage.fMax += fChangeOfMaxSoul_; }

	/////// ChangeRateSoul
	float			GetChangeRateSoul() const { return m_SoulGage.fChangeRate;	}
	void			SetChangeRateSoul( const float fChangeRateSoul_ ) { m_SoulGage.fChangeRate = fChangeRateSoul_; }
	void			UpChangeRateSoul( const float fChangeofChangeRateSoul_ ) { m_SoulGage.fChangeRate += fChangeofChangeRateSoul_; }

	////////////////// ForceDown ��ġ ���� Get/Set ///////////////////////////////////////////
	////// NowForceDown
	float			GetNowForceDown() const { return m_ForceDownGage.fNow;	}
	void			SetNowForceDown( const float fNowForceDown_ ) { m_ForceDownGage.fNow = fNowForceDown_; }
	void			UpNowForceDown( const float fChangeOfNowForceDown_ )
	{
		m_ForceDownGage.Increase( fChangeOfNowForceDown_ );
	}

	////// MaxForceDown
	float			GetMaxForceDown() const { return m_ForceDownGage.fMax; }
	void			SetMaxForceDown( const float fMaxForceDown_ ) { m_ForceDownGage.fMax = fMaxForceDown_; }
	void			UpMaxForceDown( const float fChangeOfMaxForceDown_ ) { m_ForceDownGage.fMax += fChangeOfMaxForceDown_; }

	/////// ChangeRateForceDown
	float			GetChangeRateForceDown() const { return m_ForceDownGage.fChangeRate; }
	void			SetChangeRateForceDown( const float fChangeRateForceDown_ ) { m_ForceDownGage.fChangeRate = fChangeRateForceDown_; }
	void			UpChangeRateForceDown( const float fChangeofChangeRateForceDown_ ) { m_ForceDownGage.fChangeRate += fChangeofChangeRateForceDown_; }

	////////////////// ChargeMpGageForDetonation(����) ��ġ ���� Get/Set ///////////////////////////////////////////
	////// ChargeMpForDetonation
	float			GetNowChargeMpForDetonation() const { return m_ChargeMpGageForDetonation.fNow; }
	void			SetNowChargeMpForDetonation( const float fChargeMpForDetonation_ ) {
		m_ChargeMpGageForDetonation.fNow = fChargeMpForDetonation_;
	}
	void			UpNowChargeMpForDetonation( const float fChangeOfChargeMpForDetonation_ ) {
		m_ChargeMpGageForDetonation.fNow += fChangeOfChargeMpForDetonation_;
	}

	////// MaxChargeMpforDetonation
	float			GetMaxChargeMpForDetonation() const { return m_ChargeMpGageForDetonation.fMax; }
	void			SetMaxChargeMpForDetonation( const float fMaxForceDown_ ) {
		m_ChargeMpGageForDetonation.fMax = fMaxForceDown_;
	}
	void			UpMaxChargeMpforDetonation( const float fChangeOfMaxForceDown_ ) {
		m_ChargeMpGageForDetonation.fMax += fChangeOfMaxForceDown_;
	}

	/////// ChangeRateChargeMpForDetonation
	float			GetChangeRateChargeMpForDetonation() const { return m_ChargeMpGageForDetonation.fChangeRate; }
	void			SetChangeRateChargeMpForDetonation( const float fChangeRateChargeMpForDetonation_ ) {
		m_ChargeMpGageForDetonation.fChangeRate = fChangeRateChargeMpForDetonation_;
	}
	void			UpChangeRateChargeMpForDetonation( const float fChangeofChangeRateChargeMpForDetonation_ ) {
		m_ChargeMpGageForDetonation.fChangeRate += fChangeofChangeRateChargeMpForDetonation_;
	}

	bool			GetEnable() const { return m_bEnable; }
	void			SetEnable( const bool bEnable_ ) { m_bEnable = bEnable_; }

	// Ű �Է��� ���� ���� ���� ���� ����
	bool			GetCanChargeMpForDetonation() const { return m_bCanChargeMpForDetonation; }
	void			SetCanChargeMpForDetonation( const bool bCanChargeMpForDetonation_ ) { m_bCanChargeMpForDetonation = bCanChargeMpForDetonation_; }

	float			GetChangeRateByItem() const { return m_fChangeRateByItem; }
	void			SetChangeRateByItem( const float fChangeRateByItem_ ) { m_fChangeRateByItem = fChangeRateByItem_; }

	float			GetChangeRateTimeByItem() const { return m_fChangeRateTimeByItem; }
	void			SetChangeRateTimeByItem( const float fChangeRateTimeByItem_ ) { m_fChangeRateTimeByItem = fChangeRateTimeByItem_; }

	bool			GetEnableHPRelativeChangeRate() const { return m_bHpRelativeChangeRate; }
	void			SetEnableHPRelativeChangeRate( const bool bHpRelativeChangeRate_ ) { m_bHpRelativeChangeRate = bHpRelativeChangeRate_; }

	void			AddMPFactor( const float fFactor_, const float fTime_ ) { m_pMpGage->AddFactor( fFactor_, fTime_ ); }
	void			ClearMPFactor() { m_pMpGage->ClearFactor(); }

	bool			FlushMp( const float fFlushMp_ );

	// ������ // �����ý� ���� ���� ������ ���� �����Լ��� ����
	virtual bool	DamageFlushMp();
	bool			FlushForceDown();

	bool			FlushSoul( const float fFlushSoul_ );

	void			SwapGage()
	{
		Gage *pGage = new Gage(m_pHpGage);
		SAFE_DELETE(m_pHpGage);
		m_pHpGage = pGage;

		pGage = new Gage(m_pMpGage);
		SAFE_DELETE(m_pMpGage);
		m_pMpGage = pGage;
		m_fSwapGageTime = 0.f;
	}

	float GetHyperModeRemainTime() const { return m_fHyperModeRemainTime; }
	void SetHyperModeRemainTime( const float fHyperModeRemainTime_ ) { m_fHyperModeRemainTime = fHyperModeRemainTime_; }
	bool VerifyHyperModeRemainTime() const { return m_fHyperModeRemainTime.Verify(); }

	BYTE GetHyperModeCount() const { return m_byHyperModeCount; }
	void SetHyperModeCount( BYTE byHyperModeCount_ ) { m_byHyperModeCount = byHyperModeCount_; }

	///////dummy

	void SetBossGaugeNumber( IN u_short /*usNumber_*/ ) { ASSERT( !L"Not Exist" ); }
	//////////////dummy
	virtual void	OnFrameMove( double fTime, float fElapsedTime );
	virtual void	DetonationFrameMove( double fTime, float fElapsedTime );

	virtual void	UpdateDataFromGameUnit( const CX2GageData* pGageData_ );
	virtual void	InitWhenGameIsOver();

	virtual bool GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus_ ) const;
	virtual void SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus_ );

	bool Veryfy() const;

	void UseSpecialAbilityInVillage( const CX2Item::ItemTemplet* pItemTemplet_ );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� �������� ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );

	bool GetUpdateMpPoint()const{return m_bUpdateMpPoint;}
	void SetUpdateMpPoint(bool bVal){m_bUpdateMpPoint = bVal;}

	// �ٿ��ġ
	bool GetActivateForceDown() const { return m_bIsActivateForceDown; }
	void SetActivateForceDown(bool val) { m_bIsActivateForceDown = val; }
	bool UpdateFlushForceDown();

	// ����
	bool GetActivateDetonation() const { return m_bIsActivateDetonation; }
	void SetActivateDetonation(bool val, float fNowCharge ) { m_bIsActivateDetonation = val; m_fNowChargeMpForDetonation = fNowCharge; }
	bool UpdateDamageFlushMP();

    void    AddRef()    {   ++m_uRefCount; }
    void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

protected:

	float GetSwapGageTime() const { return m_fSwapGageTime; }
	void SetSwapGageTime( const float fSwapGageTime_ ) { m_fSwapGageTime = fSwapGageTime_; }
	void UpSwapGageTime( const float fChangeOfSwapGageTime_ ) { m_fSwapGageTime += fChangeOfSwapGageTime_; }

private:

    unsigned                                        m_uRefCount;

	Gage*					m_pHpGage;
	Gage*					m_pMpGage;
	Gage					m_SoulGage;
	Gage					m_ForceDownGage;
	Gage					m_ChargeMpGageForDetonation;

	float					m_fSwapGageTime;

	KProtectedType<float>	m_fChangeRateByItem;		/// ������ ��� �ε�.
	KProtectedType<float>	m_fChangeRateTimeByItem;

	KProtectedType<float>	m_fHyperModeRemainTime;		/// ���� ���� �ð�

	BYTE					m_byHyperModeCount;						// ���� ���� ����
	bool					m_bEnable;
	bool					m_bCanChargeMpForDetonation;			// ����

	bool					m_bHpRelativeChangeRate;	/// NPC �߿� ??
	bool					m_bUpdateMpPoint;

	//������//2013-01-23 //
	//���� �нú� ������ ������뿡�� ����ϱ� ���� GUUser��ü�� �ִ� m_bLimitManaManagement ������
	//���� ��ü ����/�Ҹ꿡 ������� �����ð����� ���� ��Ű�� ���� GageDataŬ������ �̵�
	float					m_fAddMaxMP;				/// �ִ� ������ ���� ��

	bool					m_bIsActivateForceDown;		// �ٿ��ġ Ȱ��ȭ ����
	bool					m_bIsActivateDetonation;	// ���� Ȱ��ȭ ����
	float					m_fNowChargeMpForDetonation;// ���� Ȱ��ȭ �Ǿ��� �� ���� ��
};

IMPLEMENT_INTRUSIVE_PTR( CX2GageData );

class CX2ChungGageData : public CX2GageData
{
public:
	CX2ChungGageData() : CX2GageData(),
		m_iNowCannonBallCount( 0 ), m_iMaxCannonBallCount( 0 ),
		m_iNowCannonBallCountEx( 0 ), m_iMaxCannonBallCountEx( 0 ),
		m_bBerserkMode( false ), m_bBerserkModeChanged( false ), m_bCannonBallChanged( false )
	{}

	CX2ChungGageData( const CX2ChungGageData& rhs_ ) : CX2GageData( rhs_ ),
		m_iNowCannonBallCount( rhs_.m_iNowCannonBallCount ), m_iMaxCannonBallCount( rhs_.m_iMaxCannonBallCount ),
		m_iNowCannonBallCountEx( rhs_.m_iNowCannonBallCountEx ), m_iMaxCannonBallCountEx( rhs_.m_iMaxCannonBallCountEx ),
		m_bBerserkMode( rhs_.m_bBerserkMode ), m_bBerserkModeChanged( rhs_.m_bBerserkModeChanged ),
		m_bCannonBallChanged( rhs_.m_bCannonBallChanged )
	{}

	void operator=( const CX2ChungGageData& rhs_ )
	{
		CX2GageData::operator =( rhs_ );
		m_iNowCannonBallCount = rhs_.m_iNowCannonBallCount;
		m_iMaxCannonBallCount = rhs_.m_iMaxCannonBallCount;
		m_iNowCannonBallCountEx = rhs_.m_iNowCannonBallCountEx;
		m_iMaxCannonBallCountEx = rhs_.m_iMaxCannonBallCountEx;
		m_bBerserkMode = rhs_.m_bBerserkMode;
		m_bBerserkModeChanged = rhs_.m_bBerserkModeChanged;
		m_bCannonBallChanged = rhs_.m_bCannonBallChanged;
	}

	int GetNowCannonBallCount() const { return m_iNowCannonBallCount; }
	void SetNowCannonBallCount( const int iNowCannonBallCount_ )
	{
		if ( GetMaxCannonBallCount() < iNowCannonBallCount_ )
			m_iNowCannonBallCount = GetMaxCannonBallCount();
		else if ( 0 > iNowCannonBallCount_ )
			m_iNowCannonBallCount = 0;
		else
			m_iNowCannonBallCount = iNowCannonBallCount_;
	}

	int GetMaxCannonBallCount() const { return m_iMaxCannonBallCount; }
	void SetMaxCannonBallCount( const int iMaxCannonBallCount_ ) { m_iMaxCannonBallCount = iMaxCannonBallCount_; }

	bool GetBerserkMode() const { return m_bBerserkMode; }
	void SetBerserkMode( const bool bBerserkMode_ ) { m_bBerserkMode = bBerserkMode_; }

	bool GetBerserkModeChanged() const { return m_bBerserkModeChanged; }
	void SetBerserkModeChanged( bool bBerserkModeChanged_ ) { m_bBerserkModeChanged = bBerserkModeChanged_; }

	bool GetCannonBallChanged() const { return m_bCannonBallChanged; }
	void SetCannonBallChanged( const bool bCannonBallChanged_ ) { m_bCannonBallChanged = bCannonBallChanged_; }

	bool IsFullCannonBallCount() const { return GetNowCannonBallCount() >= GetMaxCannonBallCount(); }
	bool IsEmptyCannonBallCount() const { return GetNowCannonBallCount() <= 0; }

	/// ��ƽ�� ������
	int GetMaxCannonBallCountEx() const { return m_iMaxCannonBallCountEx; }
	void SetMaxCannonBallCountEx( const int iMaxCannonBallCountEx_ ) { m_iMaxCannonBallCountEx = iMaxCannonBallCountEx_; }

	int GetNowCannonBallCountEx() const { return m_iNowCannonBallCountEx; }
	void SetNowCannonBallCountEx( const int iNowCannonBallCountEx_ )
	{
		if ( GetMaxCannonBallCountEx() < iNowCannonBallCountEx_ )
			m_iNowCannonBallCountEx = GetMaxCannonBallCountEx();
		else if ( 0 > iNowCannonBallCountEx_ )
			m_iNowCannonBallCountEx = 0;
		else
			m_iNowCannonBallCountEx = iNowCannonBallCountEx_;
	}

	virtual void	UpdateDataFromGameUnit( const CX2GageData* pGageData_ );
	virtual void	InitWhenGameIsOver();

	virtual bool	GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const;
	virtual void	SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� ������������ ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );

private:
	const static int		EXTRA_CANNON_BALL_COUNT_BITMASK_VALUE = 7;
	KProtectedType<int>		m_iNowCannonBallCount;
	KProtectedType<int>		m_iMaxCannonBallCount;

	/// ��ƽ�������� ���� GageData Ŭ������ ����� �;�����, CX2GUChung ��ü�� Ŭ���� �ϳ��� �Ǿ� �ֱ� ������
	/// GageData�� �������� ������ �Ǹ� ��뿡 ������� �־ ������ �ʾ���
	KProtectedType<int>		m_iNowCannonBallCountEx;
	KProtectedType<int>		m_iMaxCannonBallCountEx;

	bool					m_bBerserkMode;
	bool					m_bBerserkModeChanged;
	bool					m_bCannonBallChanged;

};


class CX2ElswordGageData : public CX2GageData
{
public:
	enum WAY_OF_SWORD_STATE
	{
		WSS_DESTRUCTION				= -2,
		WSS_TOWARD_DESTRUCTION		= -1,
		WSS_CENTER					= 0,
		WSS_TOWARD_VIGOR			= 1,
		WSS_VIGOR					= 2,
	};

	CX2ElswordGageData() : CX2GageData(), m_bChangedWayOfSwordState( true ),
		m_eWayOfSwordState( 0/*WSS_CENTER*/ ), m_fWayOfSwordPoint( 0.0f )
	{}

	CX2ElswordGageData( const CX2ElswordGageData& rhs_ ) : CX2GageData( rhs_ ),
		m_bChangedWayOfSwordState( rhs_.m_bChangedWayOfSwordState ),
		m_eWayOfSwordState( rhs_.m_eWayOfSwordState ), m_fWayOfSwordPoint( rhs_.m_fWayOfSwordPoint )
	{}

	void operator=( const CX2ElswordGageData& rhs_ )
	{
		CX2GageData::operator =( rhs_ );
		m_bChangedWayOfSwordState = rhs_.m_bChangedWayOfSwordState;
		SetWayOfSwordState( rhs_.m_eWayOfSwordState );
		m_fWayOfSwordPoint = rhs_.m_fWayOfSwordPoint;
	}

	bool GetChangedWayOfSwordState() const { return m_bChangedWayOfSwordState; }
	void SetChangedWayOfSwordState( const bool bChangedWayOfSwordState_ ) { m_bChangedWayOfSwordState = bChangedWayOfSwordState_; }

	int GetWayOfSwordState() const { return m_eWayOfSwordState; }
	void SetWayOfSwordState(/*CX2ElswordGageData::WAY_OF_SWORD_STATE*/ int val)
	{
		if ( GetWayOfSwordState() != val )
			SetChangedWayOfSwordState( true );

		m_eWayOfSwordState = val;
	}

	float GetWayOfSwordPoint() const { return m_fWayOfSwordPoint; }
	void SetWayOfSwordPoint(float val) { m_fWayOfSwordPoint = val; }

	virtual void	UpdateDataFromGameUnit( const CX2GageData* pGageData_ );
	virtual void	InitWhenGameIsOver();
	virtual bool	GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const;
	virtual void	SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� ������������ ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );

private:
	bool						m_bChangedWayOfSwordState;	// ���� �� ������Ʈ�� ���� �Ǿ����� ����
	int/*WAY_OF_SWORD_STATE*/	m_eWayOfSwordState;
	float						m_fWayOfSwordPoint;

};

//#pragma region CX2AraGageData
/** @class : CX2AraGageData
	@brief : �ƶ� ��� ���� ������
	@date  : 2012/11/26
*/
class CX2AraGageData : public CX2GageData
{
public:
	CX2AraGageData() : CX2GageData(), m_bBerserkMode( false ), m_bBerserkModeChanged( false ), m_bForcePowerChanged( false )
		, m_iNowForcePowerCount( 0 ), m_iMaxForcePowerCount( 0 )
	{}

	CX2AraGageData( const CX2AraGageData& rhs_ ) : CX2GageData( rhs_ ), m_bBerserkMode( rhs_.m_bBerserkMode )
		, m_bBerserkModeChanged( rhs_.m_bBerserkModeChanged ), m_bForcePowerChanged( rhs_.m_bForcePowerChanged )
		, m_iNowForcePowerCount( rhs_.m_iNowForcePowerCount), m_iMaxForcePowerCount( rhs_.m_iMaxForcePowerCount )
	{}

	void operator=( const CX2AraGageData& rhs_ )
	{
		CX2GageData::operator =( rhs_ );
		m_bBerserkMode = rhs_.m_bBerserkMode;
		m_bBerserkModeChanged = rhs_.m_bBerserkModeChanged;
		m_bForcePowerChanged = rhs_.m_bForcePowerChanged;
		m_iNowForcePowerCount = rhs_.m_iNowForcePowerCount;
		m_iMaxForcePowerCount = rhs_.m_iMaxForcePowerCount;
	}

	bool GetBerserkMode() const;
	void SetBerserkMode( const bool bBerserkMode_ );

	bool GetBerserkModeChanged() const;
	void SetBerserkModeChanged( bool bBerserkModeChanged_ );

	bool GetForcePowerChanged() const;
	void SetForcePowerChanged( const bool bForcePowerChanged_ );

	int GetNowForcePower() const;
	void SetNowForcePower( const int iNowForcePowerCount_ );

	int GetMaxForcePower() const;
	void SetMaxForcePower( const int iMaxForcePowerCount_ );

	bool IsFullForcePower() const;
	bool IsEmptyForcePower() const;

	virtual void	UpdateDataFromGameUnit( const CX2GageData* pGageData_ );
	virtual void	InitWhenGameIsOver();

	virtual bool	GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const;
	virtual void	SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� ������������ ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );

private:
	bool	m_bBerserkMode;				/// 3���� ����ȭ �����ΰ�?
	bool	m_bBerserkModeChanged;		/// ����ȭ ���·� ��ȯ�Ǿ��°�?
	bool	m_bForcePowerChanged;		/// ����� ����Ǿ��°�?

	KProtectedType<int>	m_iNowForcePowerCount;	/// ���� ��� ��
	KProtectedType<int>	m_iMaxForcePowerCount;	/// �ִ� ��� ��
};
//#pragma endregion Ŭ����

class CX2ElesisGageData : public CX2GageData
{
public:
	enum WAY_OF_SWORD_STATE
	{
		WSS_DESTRUCTION				= -2,
		WSS_TOWARD_DESTRUCTION		= -1,
		WSS_CENTER					= 0,
		WSS_TOWARD_VIGOR			= 1,
		WSS_VIGOR					= 2,
	};

	CX2ElesisGageData() : CX2GageData(), m_bChangedWayOfSwordState( true ),
		m_eWayOfSwordState( 0/*WSS_CENTER*/ ), m_fWayOfSwordPoint( 0.0f )
	{}

	CX2ElesisGageData( const CX2ElesisGageData& rhs_ ) : CX2GageData( rhs_ ),
		m_bChangedWayOfSwordState( rhs_.m_bChangedWayOfSwordState ),
		m_eWayOfSwordState( rhs_.m_eWayOfSwordState ), m_fWayOfSwordPoint( rhs_.m_fWayOfSwordPoint )
	{}

	void operator=( const CX2ElesisGageData& rhs_ )
	{
		CX2GageData::operator =( rhs_ );
		m_bChangedWayOfSwordState = rhs_.m_bChangedWayOfSwordState;
		SetWayOfSwordState( rhs_.m_eWayOfSwordState );
		m_fWayOfSwordPoint = rhs_.m_fWayOfSwordPoint;
	}

	bool GetChangedWayOfSwordState() const { return m_bChangedWayOfSwordState; }
	void SetChangedWayOfSwordState( const bool bChangedWayOfSwordState_ ) { m_bChangedWayOfSwordState = bChangedWayOfSwordState_; }

	int GetWayOfSwordState() const { return m_eWayOfSwordState; }
	void SetWayOfSwordState(/*CX2ElesisGageData::WAY_OF_SWORD_STATE*/ int val)
	{
		if ( GetWayOfSwordState() != val )
			SetChangedWayOfSwordState( true );

		m_eWayOfSwordState = val;
	}

	float GetWayOfSwordPoint() const { return m_fWayOfSwordPoint; }
	void SetWayOfSwordPoint(float val) { m_fWayOfSwordPoint = val; }

	virtual void	DetonationFrameMove( double fTime, float fElapsedTime );
	virtual void	UpdateDataFromGameUnit( const CX2GageData* pGageData_ );
	virtual void	InitWhenGameIsOver();
	virtual bool	GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const;
	virtual void	SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� ������������ ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );
	virtual bool DamageFlushMp();
private:
	bool						m_bChangedWayOfSwordState;	// ���� �� ������Ʈ�� ���� �Ǿ����� ����
	int/*WAY_OF_SWORD_STATE*/	m_eWayOfSwordState;
	float						m_fWayOfSwordPoint;

};

class CX2AddGageData : public CX2GageData
{
public:
	/// �⺻ �Լ���
	CX2AddGageData() : CX2GageData(),
		m_bIsFormationMode( false ),
		m_bChangedMutationCount( false ),
		m_iMutationCount( 0 ),
		m_fDPValue( 0.f ),
		m_fMaxDPValue( MAX_DP_GAGE_VALUE ),
		m_fChangeFormationCoolTime( 0.f )
	{}

	CX2AddGageData( const CX2AddGageData& rhs_ ) : CX2GageData( rhs_ ),
		m_bIsFormationMode( rhs_.m_bIsFormationMode ),
		m_bChangedMutationCount( rhs_.m_bChangedMutationCount ),
		m_iMutationCount( rhs_.m_iMutationCount ),
		m_fDPValue( rhs_.m_fDPValue ),
		m_fMaxDPValue( rhs_.m_fMaxDPValue ),
		m_fChangeFormationCoolTime( rhs_.m_fChangeFormationCoolTime )
	{}

	void operator=( const CX2AddGageData& rhs_ )
	{
		CX2GageData::operator		= ( rhs_ );
		m_bIsFormationMode			= rhs_.m_bIsFormationMode;
		m_bChangedMutationCount		= rhs_.m_bChangedMutationCount;
		m_iMutationCount			= rhs_.m_iMutationCount;
		m_fDPValue					= rhs_.m_fDPValue;
		m_fMaxDPValue				= rhs_.m_fMaxDPValue;
		m_fChangeFormationCoolTime	= rhs_.m_fChangeFormationCoolTime;
	}

	virtual void UpdateDataFromGameUnit( const CX2GageData* pGageData_ );

	virtual bool GetMyPlayStatusToPacket( OUT KGamePlayStatus& kGamePlayStatus ) const;
	virtual void SetMyPlayStatusFromPacket( IN const KGamePlayStatus& kGamePlayStatus );

	/// ���� GageData�� ���纻�� ���� (�Ļ� Ŭ���� ���� ������ ���纻�� ���� �ϵ��� �ϱ� ���� ������������ ����)
	virtual CX2GageData* GetCloneGageData();
	virtual void CopyGageData( IN CX2GageData* pGageData_ );


	/// �߰� �Լ���
	bool GetIsFormationMode() const { return m_bIsFormationMode; }
	void SetIsFormationMode( bool val_ ) { m_bIsFormationMode = val_; }

	bool GetChangedMutationCount() const { return m_bChangedMutationCount; }
	void SetChangedMutationCount( bool val_ ) { m_bChangedMutationCount = val_; }

	int GetMutationCount() const { return std::min( std::max( m_iMutationCount, 0), MAX_MUTATION_COUNT_VALUE ); }
	void SetMutationCount( int val_ ) { m_iMutationCount = std::min( std::max( val_, 0), MAX_MUTATION_COUNT_VALUE ); }

	float GetDPValue() const { return std::min( std::max( m_fDPValue, 0.f ), MAX_DP_GAGE_VALUE ); }
	void SetDPValue( float val_ ) { m_fDPValue = std::min( std::max( val_, 0.f ), MAX_DP_GAGE_VALUE ); }

	float GetMaxDPValue() const { return m_fMaxDPValue; }
	void SetMaxDPValue( float val_ ) { m_fMaxDPValue = val_; }		/// �ִ�ġ�� X2Define.h �� ���� �Ǿ� �־��!

	float GetChangeFormationCoolTime() const { return m_fChangeFormationCoolTime; }
	void SetChangeFormationCoolTime( float val_ ) { m_fChangeFormationCoolTime = val_; }

	virtual void OnFrameMove( double fTime, float fElapsedTime );
private:
	bool						m_bIsFormationMode;			/// ���� ��� ���� ����
	bool						m_bChangedMutationCount;	/// ���� ��ġ ���� ����
	int							m_iMutationCount;			/// ���� ��ġ
	float						m_fDPValue;					/// DP ��ġ
	float						m_fMaxDPValue;				/// DP �ִ� ��ġ
	float						m_fChangeFormationCoolTime;	/// ���� ��� ��ȯ ��Ÿ��
};
