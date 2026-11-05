#pragma once

class CX2GUEve : public CX2GUUser
{
public:
	enum EVE_STATE_ID
	{
		ENSI_BASE						= USI_END + 1,

		ENSI_DASH_JUMP_POWER_LANDING,

		ENSI_JUMP_FLY_UP,
		ENSI_JUMP_FLY_DOWN,
		ENSI_JUMP_DASH,

		ENSI_STANDUP_ROLLING_FRONT_FRONT,
		ENSI_STANDUP_ROLLING_FRONT_BACK,
		ENSI_STANDUP_ROLLING_BACK_FRONT,
		ENSI_STANDUP_ROLLING_BACK_BACK,

		ENSI_STANDUP_ATTACK_FRONT,
		ENSI_STANDUP_ATTACK_BACK,

		ENSI_INVISIBILITY_ON,


		ENSI_COMBO_Z,
		ENSI_COMBO_ZZ,
		ENSI_COMBO_ZZZ,
		ENSI_COMBO_ZZZZ,

		ENSI_COMBO_ZZZX,
		ENSI_COMBO_ZZZdownZ,
		ENSI_COMBO_ZZZdownZZ,



		ENSI_COMBO_X,
		ENSI_COMBO_XX,
		ENSI_COMBO_XXX,

		ENSI_COMBO_XXfrontX,
		ENSI_COMBO_XXfrontXX,

		ENSI_COMBO_XXdownX,
		ENSI_COMBO_XXdownXX,




		ENSI_DASH_COMBO_Z,
		ENSI_DASH_COMBO_X,

		ENSI_JUMP_COMBO_Z,
		ENSI_JUMP_COMBO_X,
		ENSI_JUMP_COMBO_X_LANDING,

		ENSI_DASH_JUMP_COMBO_Z,
		ENSI_DASH_JUMP_COMBO_ZZ,
		ENSI_DASH_JUMP_COMBO_ZZZ,

		ENSI_DASH_JUMP_COMBO_X,


//#ifdef WALL_JUMP_TEST
		ENSI_WALL_LANDING,
//#endif WALL_JUMP_TEST

		ENSI_COMBO_XXZ4,
		ENSI_DASH_COMBO_ZZ,
		ENSI_DASH_COMBO_ZZZ,
		ENSI_DASH_COMBO_XZ,
		ENSI_DASH_JUMP_COMBO_ZX,
		ENSI_DASH_JUMP_COMBO_XZ,


//{{ kimhc // 2010.1.25 // �̺� 2�� ���� ������Ʈ ID

		ENSI_NS_DASH_COMBO_ZX,
		ENSI_NS_DASH_COMBO_ZXX,

		ENSI_NS_COMBO_XXZ,
		ENSI_NS_COMBO_XXZZ8,		// �� ZŰ 8���� �Է��� �޴� ������Ʈ
		ENSI_NS_COMBO_XXZZ8Z,

		ENSI_EP_DASH_COMBO_ZZ,
		ENSI_EP_DASH_COMBO_ZZZ,
		ENSI_EP_JUMP_COMBO_ZX,
		ENSI_EP_COMBO_XXdownX,

		ENSI_A_ENS_BURST_SPEAR_Landing,
		ENSI_A_ENS_BURST_SPEAR_Landing_HYPER,

//}} kimhc // 2010.1.25 // �̺� 2�� ���� ������Ʈ ID

		ENSI_EPR_COMBO_ZZZfrontZ,
		ENSI_EPR_COMBO_XXZ,
		ENSI_EPR_DASH_COMBO_ZZ,
		ENSI_EPR_DASH_COMBO_ZZZ,
		ENSI_SA_EPR_SWEEP_ROLLING_CHARGE,
		ENSI_SA_EPR_SWEEP_ROLLING_FIRE,
		ENSI_SA_EPR_SWEEP_ROLLING_END,

		ENSI_EBS_BOOST_DASH_START,		/// �ν�Ʈ �뽬 ����
		ENSI_EBS_BOOST_DASH,			/// �ν�Ʈ �뽬

		ENSI_EBS_BOOST_DASH_AIR_START,	/// ���� �ν�Ʈ �뽬 ����
		ENSI_EBS_BOOST_DASH_AIR,		/// ���� �ν�Ʈ �뽬

		ENSI_EBS_DASH_JUMP_COMBO_XZ,	/// �뽬 ���� �޺� X(Push)Z

		ENSI_EBS_DASH_JUMP_COMBO_ZFrontX,	/// �뽬 ���� �޺� ZXXX, (����) �ν�Ʈ �뽬 XXX Front
		ENSI_EBS_DASH_JUMP_COMBO_ZUpX,		/// �뽬 ���� �޺� ZXXX, (����) �ν�Ʈ �뽬 XXX Up
		ENSI_EBS_DASH_JUMP_COMBO_ZDownX,	/// �뽬 ���� �޺� ZXXX, (����) �ν�Ʈ �뽬 XXX Down

		ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_FIRE,
		ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END,






		//////////////////////////////////////////////////////////////////////////
		// ���ʿ� �߰����ּ���~ �� �Ʒ��� �׽�Ʈ�� �߰��� enum �Դϴ�


		ENSI_TRANSFORMED,

//#ifdef PVP_BOSS_COMBAT_TEST
//		ENSI_FROZEN,
//#endif PVP_BOSS_COMBAT_TEST

	};

	enum MajorMeshInstanceID
	{
		MajorMID_DashComboZ,
		MajorMID_DashComboZZ,
		MajorMID_DashComboZZZ,
		MajorMID_ComboZZZdownZ,
		MajorMID_ComboZZZdownZ_EG,

		MajorMID_IllusionStinger01,
		MajorMID_IllusionStinger02,

		MajorMID_GenBlackHole01,
		MajorMID_GenBlackHole02,
		MajorMID_GenBlackHole03,
		MajorMID_DashJumpComboZX,
		MajorMID_DashJumpComboXZ,

		MajorMID_ExplosionImpact01,

		MajorMID_AT_SP3,
		MajorMID_EG_SP3_Wing,

		MajorMID_EG_SP3_Combo_Z1,
		MajorMID_EG_SP3_Combo_Z2,
		MajorMID_EG_SP3_Combo_Z3,
		MajorMID_EG_SP3_Combo_Z4_01,
		MajorMID_EG_SP3_Combo_Z4_02,

		MajorMID_EG_SP3_Jump_Attack_Z_01,
		MajorMID_EG_SP3_Jump_Attack_Z_02,
		MajorMID_EG_SP3_Jump_Attack_X_01,
		MajorMID_EG_SP3_Jump_Attack_X_02,

		MajorMID_EG_SP3_Dash_Jump_Combo_Z_01,
		MajorMID_EG_SP3_Dash_Jump_Combo_Z_02,
		MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01,
		MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02,
		MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01,
		MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02,

		//{{ kimhc // 2010-03-09 // effect���� effectset���� �ٲ�
		// 		MajorMID_AT_Start,
		// 		MajorMID_AT_Win,
		// 		MajorMID_AT_Win1,
		// 		MajorMID_AT_Lose,
		// 		MajorMID_AT_Lose1,
		// 		MajorMID_EG_Win1,
		//}} kimhc // 2010-03-09 // effect���� effectset���� �ٲ�
		MajorMID_AT_Overon_Guard_01,
		MajorMID_AT_Overon_Guard_02,

		MajorMID_EG_Metal_Dust_Aura_01,
		MajorMID_EG_Metal_Dust_Aura_02,

		//{{ kimhc // 2010.2.3 //	�̺� 2�� ����
		MajorPID_SPEAR_BURST01,			/// ���Ǿ� ����Ʈ 01
		MajorPID_SPEAR_BURST02,			/// ���Ǿ� ����Ʈ 02
		MajorPID_SPEAR_BURST03,			/// ���Ǿ� ����Ʈ 03
		MajorPID_SPEAR_BURST_DUMMY,		/// ���Ǿ� ����Ʈ ���� ���ùڽ�

		MajorPID_ATOMIC_BLASTER_GUN,	/// ����� �������� ���� ��
		MajorPID_ATOMIC_BLASTER_DUMMY,	/// Attack Box
		MajorPID_ATOMIC_BLASTER_BULLET,	/// �Ѿ� �������� mesh
		//}} kimhc // 2010.2.3 //	�̺� 2�� ����

		MajorMID_END,
	};

	enum MinorMeshInstanceID
	{
		MinorMID_TEST1,

		MinorMID_END,
	};

	enum EVE_MAJOR_PARTICLE_INSTANCE_ID
	{
		EVE_MAJOR_PII_ILLUSION_STRIKE_01,
		EVE_MAJOR_PII_ILLUSION_STRIKE_02,

		EVE_MAJOR_PII_ILLUSION_STINGER_01,
        EVE_MAJOR_PII_ILLUSION_STINGER_02,

		EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01,
		EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02,

		EVE_MAJOR_PII_OVERON_GUARD_01,
		EVE_MAJOR_PII_OVERON_GUARD_02,

		EVE_MAJOR_PII_END,
	};

	enum EVE_MINOR_PARTICLE_INSTANCE_ID
	{
		EVE_MINOR_PII_BASE,
		EVE_MINOR_PII_END,
	};

	// ������ ��Ƽ�� ��ų�� ���� ȣ��Ǵ� üũ�����͸� �̸� ��Ƶα� ����..
	enum EquippedActiveSkillValID
	{
		EASI_NONE = 0,
		//EASI_MEGA_EBALL_SLOT_ID,

		EASI_END,
	};







	enum EveElectraEffectType
	{
		EEET_LASER,		/// ������
		EEET_EFFECT,	/// ����Ʈ
		EEET_LOCKON,	/// ����
	};

	enum PlaneFormationType
	{
		PFT_AMPLIFIER,	/// ����
		PFT_SPECTRUM,	/// �б�
//#ifdef SERV_EVE_BATTLE_SERAPH
		PFT_INDUCTION,	/// ����
		PFT_FUSION,		/// ����
		PFT_NONE,		/// ����
//#endif
	};

	struct EveElectraSystem;
	struct EveElectraLaserData
	{
        vector<CX2DamageEffect::CEffectHandle>	    m_vecDamageEffect;		/// ����Ʈ ������
		D3DXVECTOR3					m_vLaserStartPos;				/// ������ ���� ��ġ
		D3DXVECTOR3					m_vDirVector;					/// ������ ���� ����
		D3DXVECTOR3					m_vInitRotateLocalDegree;		/// ȸ����
		float						m_fVelocity;					/// �ӵ�
		float						m_fLaserPropagateTime;			/// ���� �ð�
		float						m_fElapsedTime;					/// ��� �ð�
		float						m_fPenetrateTime;				/// ���� �ð�
		float						m_fLaserWidthScale;				/// ������ ���� ����
		bool						m_bPlaneFront;					/// ��� �� �� ����
		bool						m_bPenetrate;					/// ���� ����
		bool						m_bPenetrateGiga[4];			///
		wstring						m_TraceBone;					/// ����
		bool						m_bDie;							/// ����Ʈ���� laser�� ���� ���� �Ǵ�
		bool						m_bApplyLaserRangePassive;		/// �Ÿ� ���� �нú� ����
		wstring						m_AmplifyDamageEffectName;		/// ������ ������ ����Ʈ �̸�
		wstring						m_SpectrumDamageEffectName;		/// �б��� ������ ����Ʈ �̸�
		wstring						m_InductionDamageEffectName;	/// ������ ������ ����Ʈ �̸�
		int							m_iFireCount;					/// ������ ����ź �߻� ��
		EveElectraSystem*			m_pEveElectraSystem;			/// �Ϸ�Ʈ�� �ý��� ����Ʈ
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hParticleLaserCross;			/// ������ �浹 ��ġ ����Ʈ
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hParticleGigaLaserCross[4];	/// �Ⱑ ��Ʈ���� ���

		EveElectraLaserData( EveElectraSystem* _pEveElectraSystem );
		void OnFrameMove( float fElapsedTime );
		void Delete();

	};

	struct EveElectraEffectData
	{
        CX2DamageEffect::CEffectHandle       m_hDamageEffect;
		D3DXVECTOR3					m_vPlaneIntersectPoint;			/// ������ ��ġ
		bool						m_bPlaneFront;					/// �帷 ��, ��
		bool						m_bDie;							/// �Ҹ� Ȯ��
		float						m_fElapsedTime;					/// ��� �ð�
		wstring						m_AmplifyDamageEffectName;		/// ������ ������ ����Ʈ �̸�
		wstring						m_SpectrumDamageEffectName;		/// �б��� ������ ����Ʈ �̸�
		wstring						m_InductionDamageEffectName;	/// ������ ������ ����Ʈ �̸�
		wstring						m_FusionDamageEffectName;		/// ������ ������ ����Ʈ �̸�
		EveElectraSystem*			m_pEveElectraSystem;			/// �Ϸ�Ʈ�� �ý��� ����Ʈ

		EveElectraEffectData( EveElectraSystem* _pEveElectraSystem );
		void OnFrameMove( float fElapsedTime );
	};

	struct EveElectraLockonData
	{
        CX2DamageEffect::CEffectHandle       m_hDamageEffect;
		D3DXVECTOR3					m_vPlaneIntersectPoint;			/// ������ ��ġ
		bool						m_bPlaneFront;					/// �帷 ��, ��
		bool						m_bDie;							/// �Ҹ� Ȯ��
		float						m_fElapsedTime;					///	��� �ð�
		wstring						m_AmplifyDamageEffectName;		/// ������ ������ ����Ʈ �̸�
		wstring						m_SpectrumDamageEffectName;		/// �б��� ������ ����Ʈ �̸�
		wstring						m_InductionDamageEffectName;	/// ������ ������ ����Ʈ �̸�
		EveElectraSystem*			m_pEveElectraSystem;			/// �Ϸ�Ʈ�� �ý��� ����Ʈ

		EveElectraLockonData( EveElectraSystem* _pEveElectraSystem );
		void OnFrameMove( float fElapsedTime );
	};

	struct EveElectraSystem
	{
		bool						m_bSystemEnable;			/// �б������� ��� ������ ����
		float						m_fSystemOperationTime;		/// �б����� ���� �ð�
		float						m_fFormationSetElapsedTime;	/// �������� �ٴ� �б����� ����Ʈ�� ���ʷ� ������ �ֱ� ���� ����ϴ� ��� �ð�
		PlaneFormationType			m_ePlaneFormationType;		/// ������, �б��� �� �б����� Ÿ��
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectPlane;		/// �޽� ����Ʈ
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectCrystal1;	/// �޽� ����Ʈ
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectCrystal2;	/// �޽� ����Ʈ
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectCrystal3;	/// �޽� ����Ʈ

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hParticlePlane;		/// ��� �޽�
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleCrystal[3];	/// ũ����Ż �޽�

		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleFormationMark[3];	/// ���� ǥ�� ��ƼŬ
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleFormationMsg[3];		/// ���� �޽��� ��ƼŬ

		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleThousandStarFormation;	/// õ�� ������ ��ƼŬ

		bool						m_bPlaneEnable;				/// ���� Ȱ��ȭ
		bool						m_bSpectrumDynamicsEnable;	/// �б� ���� Ȱ��ȭ
		float						m_fLaserRangeScale;			/// Laser �Ÿ� ����
		float						m_fLaserEndOffsetX;			/// X ��ġ ����
		float						m_fLaserEndOffsetY;			/// Y ��ġ ����

		D3DXPLANE					m_PrimaryPlane;			/// ���
		D3DXVECTOR3					m_vPlaneNormal;			/// ��� ����
		D3DXVECTOR3					m_vPlanePoint;			/// ��� ��
		float						m_fPlaneZAngle;			/// ��� Z�� ����
		float						m_fLastPlaneZAngle;		/// ��� �ֱ� Z�� ����

		D3DXPLANE					m_PlaneCenter;			/// �߾� ���
		D3DXPLANE					m_PlaneUpper;			/// �� ���
		D3DXPLANE					m_PlaneLower;			/// �Ʒ� ���
		D3DXVECTOR3					m_vPlaneNormalCenter;	/// �߾� ��� ����
		D3DXVECTOR3					m_vPlaneNormalUpper;	/// �� ��� ����
		D3DXVECTOR3					m_vPlaneNormalLower;	/// �Ʒ� ��� ����

		D3DXVECTOR3					m_vPlaneRotateDegreeCenter;		/// �߾� ��� ȸ�� ����
		D3DXVECTOR3					m_vPlaneRotateDegreeUpper;		/// �� ��� ȸ�� ����
		D3DXVECTOR3					m_vPlaneRotateDegreeLower;		/// �Ʒ� ��� ȸ�� ����

//////////// GIGA STREAM ////////////////////////////////////////////////////////{{

		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectGigaPlane;			/// �Ⱑ ��� �޽�
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hEffectGigaCrystal[5];	/// �Ⱑ ũ����Ż �޽�

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hParticleGigaPlane;		/// �Ⱑ ��� ��ƼŬ
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleGigaCrystal[5];	/// �Ⱑ ũ����Ż ��ƼŬ

		bool						m_bGigaPlaneEnable;		/// �Ⱑ ��� Ȱ��ȭ ����
		D3DXPLANE					m_GigaStreamPlane;		///	�Ⱑ ���
		D3DXVECTOR3					m_vGigaPlaneNormal;		/// �Ⱑ ��� ����
		D3DXVECTOR3					m_vGigaPlanePoint;		/// �Ⱑ ��� ��
		float						m_fGigaPlaneZAngle;		/// �Ⱑ ��� Z�� ����

//////////// GIGA STREAM ////////////////////////////////////////////////////////}}

		int							m_iPriorEffectID;		/// �켱�ϴ� ����Ʈ ID
		int							m_iPosteriorEffectID;	/// �޺κ� ����Ʈ ID

		CX2GUEve*					m_pGUEve;				/// �̺� ����Ʈ
		//CX2DamageEffect::CEffect*	m_pEffectPlane;			/// ����Ʈ ���

		std::list<EveElectraLaserData>		m_EveElectraLaserDataList;		/// ������ ������ ����Ʈ
		std::list<EveElectraEffectData>		m_EveElectraEffectDataList;		/// ����Ʈ ������ ����Ʈ
		std::list<EveElectraLockonData>		m_EveElectraLockonDataList;		/// ���� ������ ����Ʈ

		bool	m_bHyperOpticalResearch;		/// �ʱ��� ����
		float	m_fHyperOpticalResearchScale;	/// �ʱ��� ���� ������ �߰� ����

		bool	m_bExceptionSystem;						/// ������ƽ ��Ʈ �ߵ�
		float	m_fExceptionSystemOperationTime;		/// ������ƽ ��Ʈ ���� �ð�
		float	m_fExceptionSystemRemainTime;			/// ������ƽ ��Ʈ ���� �ð�
		float	m_fExceptionFormationSetElapsedTime;	/// ������ƽ ��Ʈ ���� ��� �ð� ����
		PlaneFormationType m_ePrevSpectro;				/// ������ ���� ���� ���� ����

		float	m_fEveZAngle;		/// Dash Jump X(Push)X �������� ���� �̺��� �ʱⰢ ��

		int			m_iLaserGroupNum;		/// ���Ͼ� ����̴� ������ �׷� ������

		DelegateProcess				m_delegateChangePlaneFormation;		/// �Ϸ�Ʈ��, ��Ʋ ����Ʈ�� ChangePlaneFormation �� Delegate

		EveElectraSystem( CX2GUEve* _pGUEve );
        void ProcessSystem( float fElapsedTime );
		void ProcessPlane( float _fElapsedTime );
		void ProcessGigaPlane( float _fElapsedTime );
		void ProcessFormation( float _fElapsedTime );

		void StartFormationEffect();

		void AddLaserToSystem( CX2DamageEffect::CEffect* _pDamageEffect, wstring _TraceBone, D3DXVECTOR3& _vLaserStartPos, D3DXVECTOR3&_vDirVector,
							   float _fVelocity, float _fLaserPropagateTime = 0.3f, float _fLaserWidthScale = 1.f,
							   wstring AmplifyDamageEffectName = L"", wstring SpectrumDamageEffectName = L"", wstring InductionDamageEffectName = L"",
							   bool _bApplyLaserRangePassive = true,  float _fRadiusScale = 1.f );

		void AddEffectToSystem( CX2DamageEffect::CEffect* _pDamageEffect, wstring AmplifyDamageEffectName = L"",
								wstring SpectrumDamageEffectName = L"", wstring InductionDamageEffectName = L"", wstring FusionDamageEffectName = L"" );

		void AddLockonToSystem( CX2DamageEffect::CEffect* _pDamageEffect, wstring AmplifyDamageEffectName = L"",
								wstring SpectrumDamageEffectName = L"", wstring InductionDamageEffectName = L"" );

		void DeleteLaserFromSystem();

		void LaserAttackLineSet( CX2DamageEffect::CEffect* _pDamageEffect, float _fRadiusScale = 1.0f );


		void CreatePlane( float _fZAngle = 0.f, float _LifeTime = 2.f, float _fDistance = 300.f, float _fHeight = 130.f, wstring PlaneMeshName = L"" );
		void CreateGigaPlane( float _fZAngle = 0.f, float _LifeTime = 5.f, float _fDistance = 500.f, float _fHeight = 130.f, float _fScale = 1.0f );

		void SetPlaneFormation( PlaneFormationType _ePlaneFormationType );
		void ChangePlaneFormation();

		PlaneFormationType GetPlaneFormation(){ return m_ePlaneFormationType; }
		BUFF_TEMPLET_ID GetBuffTempletIdByPlaneFormation( const PlaneFormationType ePlaneFormationType_ ) const;

		bool GetEnableSystem(){ return m_bSystemEnable; }
		void SetEnableSystem( bool _bVal ){ m_bSystemEnable = _bVal; }

		bool GetEnablePlane(){ return m_bPlaneEnable; }
		void SetEnablePlane(bool val) { m_bPlaneEnable = val; }

		bool GetEnableGigaPlane(){ return m_bGigaPlaneEnable; }
		void SetEnableGigaPlane(bool val) { m_bGigaPlaneEnable = val; }

		bool GetEnableSpectrumDynamics(){ return m_bSpectrumDynamicsEnable; }
		void SetEnableSpectrumDynamics( bool _bVal ){ m_bSpectrumDynamicsEnable = _bVal; }

		float GetSystemOperationTime() const { return m_fSystemOperationTime; }
		void SetSystemOperationTime(float val) { m_fSystemOperationTime = val; }

		void DestroyFormationMarkAndMsgParticle();
		void DestroyPlaneAndCrystalEffect();
		void DestroyGigaPlaneAndCrystalEffect();

		bool GetEnableHyperOpticalResearch(){ return m_bHyperOpticalResearch; }
		void SetEnableHyperOpticalResearch( bool _bVal ){ m_bHyperOpticalResearch = _bVal; }

		void SetExceptionSystemTime( float _fVal );
		void SetEnableExceptionSystem( bool _bVal ){ m_bExceptionSystem = _bVal; }
		bool GetEnableExceptionSystem() { return m_bExceptionSystem; }

	};

	struct FlyingImpactData
	{
		D3DXVECTOR3					m_vOffsetPos;
		D3DXVECTOR3					m_vOffsetRotate;
        CX2DamageEffect::CEffectHandle       m_hEffect;

		FlyingImpactData();
		~FlyingImpactData();

//{{ robobeg : 2013-08-30
        CX2DamageEffect::CEffect*   GetLiveDamageEffect();
//}} robobeg : 2013-08-30
	};





	struct EG_QueensThroneData
	{
		double	m_dMaxLifeTime;		/// �ִ� ����ð�.


		CKTDXTimer		m_TimerElapsedTime;	/// ����ð�.
		bool			m_bEnable;			/// ��뿩��.
		float			m_fPowerRate;		/// ���� ����

		// � �پ��ִ� �޽�
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_ahMajorMeshInstance[4];

		enum PEH_POS
		{
			PP_WING,
			PP_BODY,
			PP_END,
		};
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahMajorParticleInstance[PP_END];

		std::wstring	m_wstrBoneName[5];
		int				m_iSelectBoneNameID;

		EG_QueensThroneData()
		:
		m_dMaxLifeTime(30.0),
		m_bEnable(false),
		m_iSelectBoneNameID(0),
		m_fPowerRate(0.f)
		{
			for( int i = 0; i < ARRAY_SIZE(m_ahMajorMeshInstance); ++i )
			{
				m_ahMajorMeshInstance[i] = INVALID_MESH_INSTANCE_HANDLE;
			}
			for( int i = 0; i < ARRAY_SIZE(m_ahMajorParticleInstance); ++i )
			{
				m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
			m_wstrBoneName[0] = L"Bip01";
			m_wstrBoneName[1] = L"Bip01_L_UpperArm";
			m_wstrBoneName[2] = L"Bip01_R_UpperArm";
			m_wstrBoneName[3] = L"Bip01_L_Foot";
			m_wstrBoneName[4] = L"Bip01_R_Foot";
		}

		void Reset();

		int	 GetBoneNameID()
		{
			if( m_iSelectBoneNameID > 4 || m_iSelectBoneNameID < 0 )
				m_iSelectBoneNameID = 0;

			return m_iSelectBoneNameID;
		}
	};

	struct ATOveronGuardData
	{
		CKTDXTimer		m_TimerElapsedTime;	/// ����ð�.
		bool			m_bEnable;			/// ��뿩��.

		double			m_dEffectiveTime;	/// �����ð�.
		float			m_fProjectileSize;	/// �߻�ü ũ��.

		ATOveronGuardData(){ Reset(); }
		void	Reset(){ m_bEnable = false; m_dEffectiveTime = 0.f; m_fProjectileSize = 1.f; }

		void	SetEnable( bool bEnable ){ m_bEnable = bEnable; }
		bool	GetEnable(){ return m_bEnable; }
		bool	IsLive(){ return ((GetEffectiveTime() - m_TimerElapsedTime.elapsed() > 0.0) ? true : false); }
		void	SetEffectiveTime( float fEffectiveTime ){ m_dEffectiveTime = static_cast<double>(fEffectiveTime); }
		double	GetEffectiveTime(){ return m_dEffectiveTime; }
		void	SetProjectileSize( float fProjectileSize ){ m_fProjectileSize = fProjectileSize; }
		float	GetProjectileSize(){ return m_fProjectileSize; }
	};


	// �ް��Ϸ�Ʈ�к� ����
	//#1. InitEquippedActiveSkillState() ���� ���� ����.
	//#2. CommonEventProcess() �Է�üũ.
	//#3. CommonFrameMove() ���� ����Ʈ ó�� �� [**] ������ �ٲ��� ����ó�� [**].
	//#4. SpecialAttackEventProcess() �߻�ó��.
	enum CHARGE_RANK
	{
		CR_RANK_1 = 1,
		CR_RANK_2,
		CR_RANK_3,
	};
	struct MEGA_ELECTRONBALL_Data
	{
		double			m_dMaxTime;
		CKTDXTimer		m_timerElapsedTime;

		bool			m_bEnable;			/// ���� ������.
		bool			m_bIsSucCharge;		/// ������ �Ϸ�Ǿ�����.
		CHARGE_RANK		m_ChargeRank;		/// �������� ���Ǿ��� ��ũ�� �״�� ����Ѵ�.
		bool			m_bSkillSlotSwap;	/// ���� ü���� ���� ���


		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahMajorParticleInstance[4];

		MEGA_ELECTRONBALL_Data()
		:
		m_dMaxTime(0.),
		m_bEnable(false),
		m_iSkillSlotID(-1),
		m_ChargeRank(CR_RANK_1),
		m_bIsSucCharge(false),
		m_bSkillSlotSwap( false )
		{
			for( int i = 0; i < ARRAY_SIZE(m_ahMajorParticleInstance); ++i )
			{
				m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}

		void	Reset( bool bIsChargeTrue = true, FrameData* pFrameDataFuture = NULL ); // ����Ÿ�̹��� ���� �����ϰ��. pvp ��ũ�� ����������.
		void	ResetParticle();
		void	SetSlotID( const int iSlotID )// ��ų���� B�ϰ�� 4~7���� ����.
		{
			m_iSkillSlotID = iSlotID;
		}
		int		GetSkillSlotID()
		{
			return m_iSkillSlotID;
		}
		void	SetSkillSlotSwap( bool bSkillSlotSwap ){ m_bSkillSlotSwap = bSkillSlotSwap; }
		bool	GetSkillSlotSwap(){ return m_bSkillSlotSwap; }

	private:
		int				m_iSkillSlotID;		// -1 �� �ƴϸ� �ش罽�Կ� ��ϵǾ��ִٴ� �ǹ�.
	};

	// ������ �ʶ��� �����Ͱ� �߻� ���� ����Ǳ� ������ �߻� �� ���̴� ������ �߰�
	struct TASER_PILUM_TinyData
	{
		CKTDXTimer		m_timerElapsedTime;
		CHARGE_RANK		m_ChargeRank;
		int				m_iAddDamageTime;

		TASER_PILUM_TinyData():
		m_ChargeRank(CR_RANK_1),
		m_iAddDamageTime(0)
		{}

		void reset()
		{
			m_ChargeRank = CR_RANK_1;
			m_iAddDamageTime = 0;
			m_timerElapsedTime.ResetStartTime();
		}
	};


	struct TASER_PILUM_Data
	{
		enum SLOT_TYPE
		{
			ST_A = 0,
			ST_B,

			ST_END,
		};

		double			m_dMaxTime;
		CKTDXTimer		m_timerElapsedTime;

		bool			m_bEnable;			/// ���� ������.
		bool			m_bIsSucCharge;		/// ������ �Ϸ�Ǿ�����.
		CHARGE_RANK		m_ChargeRank;		/// �������� ���Ǿ��� ��ũ�� �״�� ����Ѵ�.
		bool			m_bSkillSlotSwap;	/// ���� ü���� ���� ���

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahMajorParticleInstance[4];

		TASER_PILUM_Data()
			:
		m_dMaxTime(0.),
			m_bEnable(false),
			m_bSelectedSlot(true),
			m_ChargeRank(CR_RANK_1),
			m_bIsSucCharge(false),
			m_bSkillSlotSwap(false)
		{
			m_iSkillSlotID[ST_A] = -1;
			m_iSkillSlotID[ST_B] = -1;

			for( int i = 0; i < ARRAY_SIZE(m_ahMajorParticleInstance); ++i )
			{
				m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}

		void	Reset( bool bIsChargeTrue = true, FrameData* pFrameDataFuture = NULL ); // ����Ÿ�̹��� ���� �����ϰ��. pvp ��ũ�� ����������.
		void	ResetParticle();

		void	SetSlotID( SLOT_TYPE eSlotType, const int iSlotID )//const int iSlotID, const bool bSlotB )
		{
			m_iSkillSlotID[eSlotType] = iSlotID;
			//m_bSkillSlotB	= bSlotB;
		}
		int		GetSkillSlotID()
		{
			if( m_bSelectedSlot == true )
				return m_iSkillSlotID[ST_A];
			else
				return m_iSkillSlotID[ST_B];
		}
		//bool	GetSkillSlotB(){return m_bSkillSlotB;}
		void	SetSelectedSlot( bool bSelectedSlot ){ m_bSelectedSlot = bSelectedSlot; }
		bool	GetSelectedSlot(){ return m_bSelectedSlot; }
		void	SetSkillSlotSwap( bool bSkillSlotSwap ){ m_bSkillSlotSwap = bSkillSlotSwap; }
		bool	GetSkillSlotSwap(){ return m_bSkillSlotSwap; }

	private:
		int				m_iSkillSlotID[ST_END];	// -1 �� �ƴϸ� �ش罽�Կ� ��ϵǾ��ִٴ� �ǹ�.
		bool			m_bSelectedSlot;	// A : true | B : false
	};

	enum BOOST_DASH_STATE
	{
		BDS_NONE	= 0,	/// �ν�Ʈ �뽬�� �ƴ�
		BDS_RIGHT	= 1,	/// �ν�Ʈ �뽬 ������
		BDS_LEFT	= 2,	/// �ν�Ʈ �뽬 ����
	};

	struct ENERGY_NEEDLES_Data
	{
		bool	m_bEnable;			/// ������ �ϵ齺 �߻�
		float	m_fMaxTime;			/// ������ �ϵ齺 �߻� ���� �ð�, SA_EFFECTIVE_TIME ���� ����
		float	m_fIntervalTime;	/// ������ �ϵ齺 �߻� ����( 0.33���� �� �� )
		float	m_fPowerRate;		/// ������ �ϵ齺 PowerRate ��ġ

		D3DXVECTOR3 m_vEffectPos[6];

		ENERGY_NEEDLES_Data( )
			: m_bEnable( false )
			, m_fMaxTime( 0.f )
			, m_fIntervalTime( 0.33f )
			, m_fPowerRate( 1.f )
		{
			m_vEffectPos[0] = D3DXVECTOR3( 120,	-40,  20 );
			m_vEffectPos[1] = D3DXVECTOR3( 135,	-20, -20 );
			m_vEffectPos[2] = D3DXVECTOR3( 105,	 60,   0 );
			m_vEffectPos[3] = D3DXVECTOR3( 140,  75,   0 );
			m_vEffectPos[4] = D3DXVECTOR3( 110,	-60,   0 );
			m_vEffectPos[5] = D3DXVECTOR3( 100,	-75,  40 );
		}

		void OnFrameMove( IN CX2GUEve* pGUEve, IN float fElapsedTime_ );
	};

	struct SKugelBlitz_Data
	{
		enum SLOT_TYPE
		{
			ST_A = 0,
			ST_B,

			ST_END,
		};

		double			m_dMaxTime;
		CKTDXTimer		m_timerElapsedTime;

		bool			m_bEnable;			/// ���� ������.
		bool			m_bIsSucCharge;		/// ������ �Ϸ�Ǿ�����.
		CHARGE_RANK		m_ChargeRank;		/// �������� ���Ǿ��� ��ũ�� �״�� ����Ѵ�.
		bool			m_bSkillSlotSwap;	/// ���� ü���� ���� ���

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahMajorParticleInstance[4];

		SKugelBlitz_Data()
			: m_dMaxTime(0.)
			, m_bEnable(false)
			, m_bSelectedSlot(true)
			, m_ChargeRank(CR_RANK_1)
			, m_bIsSucCharge(false)
			, m_bSkillSlotSwap(false)
		{
			m_iSkillSlotID[ST_A] = -1;
			m_iSkillSlotID[ST_B] = -1;

			for( int i = 0; i < ARRAY_SIZE(m_ahMajorParticleInstance); ++i )
			{
				m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
			}
		}

		void	Reset( bool bIsChargeTrue = true, FrameData* pFrameDataFuture = NULL ); // ����Ÿ�̹��� ���� �����ϰ��. pvp ��ũ�� ����������.
		void	ResetParticle();

		void	SetSlotID( SLOT_TYPE eSlotType, const int iSlotID )//const int iSlotID, const bool bSlotB )
		{
			m_iSkillSlotID[eSlotType] = iSlotID;
			//m_bSkillSlotB	= bSlotB;
		}
		int		GetSkillSlotID()
		{
			if( m_bSelectedSlot == true )
				return m_iSkillSlotID[ST_A];
			else
				return m_iSkillSlotID[ST_B];
		}

		void	SetSelectedSlot( bool bSelectedSlot ){ m_bSelectedSlot = bSelectedSlot; }
		bool	GetSelectedSlot(){ return m_bSelectedSlot; }
		void	SetSkillSlotSwap( bool bSkillSlotSwap ){ m_bSkillSlotSwap = bSkillSlotSwap; }
		bool	GetSkillSlotSwap(){ return m_bSkillSlotSwap; }

	private:
		int				m_iSkillSlotID[ST_END];	// -1 �� �ƴϸ� �ش罽�Կ� ��ϵǾ��ִٴ� �ǹ�.
		bool			m_bSelectedSlot;	// A : true | B : false
	};

	struct SKugelBlitz_TinyData
	{
		CKTDXTimer		m_timerElapsedTime;
		CHARGE_RANK		m_ChargeRank;
		int				m_iAddDamageTime;

		SKugelBlitz_TinyData()
			: m_ChargeRank(CR_RANK_1), m_iAddDamageTime(0)
		{}

		void reset()
		{
			m_ChargeRank = CR_RANK_1;
			m_iAddDamageTime = 0;
			m_timerElapsedTime.ResetStartTime();
		}
	};




	private:
		CX2GUEve( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit );
		virtual ~CX2GUEve(void);

		bool IsUpSkillSlotMegaElectronBall(); // �ް��Ϸ�Ʈ�к� �������� Ű�� ���ȴ��� üũ..
		const CX2UserSkillTree::SkillSlotData* GetSkillSlotDataMegaElectronBall( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ );

		bool IsUpSkillSlotTaserPilum();
		const CX2UserSkillTree::SkillSlotData* GetSkillSlotDataTaserPilum( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ );

		bool IsUpSkillSlotKugelBlitz();
		const CX2UserSkillTree::SkillSlotData* GetSkillSlotDataKugelBlitz( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ );
		int	GetKugelBlitzAddDamageTime() const {return m_sKugelBlitz_TinyData.m_iAddDamageTime;}
		void ResetKugelBlitzTinyData() { m_sKugelBlitz_TinyData.reset(); }

//{{ kimhc // 2010.11.2 // �������� - ���� ������
		bool CanUseChargingBooster( CX2SkillTree::SKILL_TYPE eSkillType_, const IN CX2UserSkillTree& cUserSkillTree_ );
		void UseChargingBooster( CX2UserSkillTree& cUserSkillTree_ );
//}} kimhc // 2010.11.2 // �������� - ���� ������



	public:
		static CX2GUEve* CreateGUEve( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit )
		{
			return new CX2GUEve( unitIndex, teamNum,
				pScriptLuaState,
				frameBufferNum, pUnit );
		};


	static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );


	virtual void		ReInit( bool bRandom = false, int startPosIndex = -1 );

	virtual void		DamageReact( CX2DamageManager::DamageData* pDamageData );
	virtual void		AttackResultPlayEffect();

	virtual void		Win();
	virtual void		Lose();
	virtual void		Draw();

	virtual void		InitEffect();
	virtual void		InitEnchantWeaponEffectUnitSpecific();


	void				InitializeEveMajorParticleArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
	virtual void		CreateMajorParticleArray();		// ������ ��ƼŬ �迭�� ���� // kimhc // 2010.11.5
	static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetEveMajorParticleByEnum( EVE_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5

	ParticleEventSequenceHandle	GetHandleEveMajorParticleByEnum( EVE_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
	{
		ASSERT( EVE_MAJOR_PII_END > eVal_ && EVE_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahEveMajorParticleInstance[eVal_];
	}

	ParticleEventSequenceHandle& GetHandleEveMajorParticleByEnum( EVE_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
	{
		ASSERT( EVE_MAJOR_PII_END > eVal_ && EVE_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahEveMajorParticleInstance[eVal_];
	}

	void				SetHandleEveMajorParticleByEnum( EVE_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ ) // ĳ���͸� ���� ������ ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������ // kimhc // 2010.11.5
	{
		ASSERT( EVE_MAJOR_PII_END > eVal_ && EVE_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		m_ahEveMajorParticleInstance[eVal_] = hHandle_;
	}
	void				DeleteEveMajorParticle();


	void				InitializeEveMinorParticleArray();	// ���������� ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
	virtual void		CreateMinorParticleArray();		// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
	static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetEveMinorParticleByEnum( EVE_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5

	ParticleEventSequenceHandle	GetHandleEveMinorParticleByEnum( EVE_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const	// ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
	{
		ASSERT( EVE_MINOR_PII_END > eVal_ && EVE_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahEveMinorParticleInstance[eVal_];
	}

	ParticleEventSequenceHandle& GetHandleEveMinorParticleByEnum( EVE_MINOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
	{
		ASSERT( EVE_MINOR_PII_END > eVal_ && EVE_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahEveMinorParticleInstance[eVal_];
	}

	void				SetHandleEveMinorParticleByEnum( EVE_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )	// ĳ���͸� ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������	// kimhc // 2010.11.5
	{
		ASSERT( EVE_MINOR_PII_END > eVal_ && EVE_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		m_ahEveMinorParticleInstance[eVal_] = hHandle_;
	}
	void				DeleteEveMinorParticle();
	virtual	void		DeleteMinorParticle();

	void 				InitMajorParticle();
	void 				InitMinorParticle();

	void				CallSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID = CX2UnitManager::NUI_NONE );
	void				ResetMegaEletronCharge();

	virtual bool		SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet );
	virtual float		GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

	virtual	float		GetAdditionalAccelBuffTime( const CX2SkillTree::SKILL_ID eSkillID_ ) const;
	virtual void		ApplyAuraAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ );
	CX2BuffFactorPtr	GetEnergyOfConcentrationBuffFactorClonePtr() const;

	bool				GetStartSpectroElCrystal() { return m_bStartSpectroElCrystal; }
	void				SetStartSpectroElCrystal( bool bStartSpectroElCrystal_ ) { m_bStartSpectroElCrystal = bStartSpectroElCrystal_; }

	bool				GetEnableElectraSystem(){ return m_EveElectraSystem.GetEnableSystem(); }
	PlaneFormationType	GetElectraFormationType(){ return m_EveElectraSystem.GetPlaneFormation(); }
	void				ClearThousandStar();
	void				CreateThousandStar();
	void				DoFrameMoveThousandStar();
	void				EntireAttackThousandStar();
	void				AttackThousandStar();
	int					GetThousandStarCount(){ return m_iThousandStarCount; }
	bool				GetThousandStarEntireFired(){ return m_bEntireFireMode; }

	virtual void		AttackResultByType( CX2DamageManager::DamageData &pDamageData );

	void				ResetTaserPilumCharge();
	void				ResetKugelBlitzCharge();
	//�������� �ñر� - ��ũ ���� ���� ��� ��Ʈ�� �����ߴ��� üũ
	const bool GetActiveLinkOverChargeIllusion() const { return m_bStartLinkOverChargeIllusion; }
	void SetActiveLinkOverChargeIllusion( const bool bActive ) { m_bStartLinkOverChargeIllusion = bActive; }

	void SaveFerdinandNPCUID( const int npcUid ) { m_iFerdinandNPCUID = npcUid; }
	void ResetLinkOverChargeIllusion();

	virtual void HyperModeBuffEffectStart();
	virtual void AddUnitSlashData( const CX2UnitSlashTraceManager::SLASH_TRACE_CONDITION eSlashTraceCondition_ );

	/// �� ���ָ��� Ư�� ������ ���� �Ǵ� ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ��...)
	virtual void SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );

	virtual void ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ );
	virtual void ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ );

	void		ChangePlaneForamtionSystem( const PlaneFormationType ePlaneFormatinoSystem_ );

	virtual int GetComboZStateID() const { return ENSI_COMBO_Z; }
	virtual int GetComboXStateID() const { return ENSI_COMBO_X; }



protected:
	virtual void ParseCommonRandomState();
	void				InitState();
	virtual void		InitComponent();
	void	InitStateID();
	void	InitStateCommon();



	virtual void		InitEquippedActiveSkillState( bool bOnlySkillLevel = false );

	virtual void		InitPassiveSkillState();
	virtual void		SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ );
	virtual void		SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ );



	virtual void		CommonStateStartFuture();
	virtual void		CommonStateStart();
	virtual void		CommonFrameMoveFuture();
	virtual void		CommonFrameMove();
	virtual void		CommonEventProcess();
	virtual void		CommonStateEndFuture();
	virtual void		CommonStateEnd();
	//{{ robobeg : 2008-10-28
	//virtual void		CommonRender();
	virtual RENDER_HINT CommonRender_Prepare();
	//}} robobeg : 2008-10-28

	virtual bool		SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
	virtual void		NoStateChangeActionFrameMove();


	virtual	void		CreateStepDust();

	void				CancelComboWhenChangedDirection( int iFlagCount );
	float				CalcInvisibilityAlphaValue();
	void				SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID = CX2UnitManager::NUI_NONE );

	//{{ kimhc // 2010.2.10 //	�̺� ���Ǿ� ����Ʈ�� ���� ����� �ĺ� ��
	void				CreateSpearBurst();
	//}} kimhc // 2010.2.10 //	�̺� ���Ǿ� ����Ʈ�� ���� ����� �ĺ� ��

	void StateChangeJumpDashIfPossible();

	virtual void ModifyFinalDamageByGameUnitType( OUT CX2DamageManager::DAMAGE_TYPE& damageType_, OUT float& fAttackerPhysicDamage_, OUT float& fAttackerMagicDamage_, OUT float& fForceDownValue_ );

	void StartElCrystalSystem();
	void EndElCrystalSystem( bool bEnergeticHeartEnd_= false );

	void ChangeThousandsOfStarsFormation();
	int	GetTaserPilumAddDamageTime() const {return m_sTaserPilumTinyData.m_iAddDamageTime;}
	void ResetTaserPilumTinyData() { m_sTaserPilumTinyData.reset(); }

	void OnFrameMove_EndElCrystalSystem();

	virtual void InitInpuDataProcess();
private:

	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahEveMajorParticleInstance[EVE_MAJOR_PII_END];
	CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahEveMinorParticleInstance[EVE_MINOR_PII_END];

protected:
	bool								m_bReAttackZ1Right;
	float								m_fReAttackZ1Time;
	float								m_fReDashJumpXTime;
	//bool								m_bZZZX;


	int			m_iJumpFlyCount;
	int			m_iDashJumpCount;
	int			m_iDashJumpUpCount;
	bool				m_bAfterJumpDash;		/// ���� �뽬 �̵��� ���
	bool				m_bEnableBoostDash;		/// ���� �뽬 �̵��� ���� �������� �̵��� ���
	BOOST_DASH_STATE	m_eBoostDashState;		/// �ν�Ʈ �뽬 ����

	//int			m_iJumpDashCount;
	float		m_fCurrDashJumpTime;		// dash_jump Ȥ�� jump_dash


	float		m_fInvisibilityTime;		// ����ȭ ���� �ð�
	CKTDXTimer	m_timerInvisibility;
	float		m_fInvisibilityAlpha;		// ����ȭ ���� �� ���� ���İ�





	CKTDXTimer	m_timerSummoned;
	float		m_fSummonedLifeTime;		// ��ȯ�� ���͵� ����ִ� �ð�

//{{ kimhc // 2010.11.2 // �������� - ���� ������
	// ���Ŀ� map or managerŬ���� ���� �����, damageManager�� �����ϴ� ����� ������ ������
	CSkillDataBasePtr	m_ChargingBoosterSkillDataBasePtr;	/// ����: ���������� ��ų ��� ������
//}} kimhc // 2010.11.2 // �������� - ���� ������


	CKTDGXMeshPlayer::CXMeshInstanceHandle				m_ahMajorMeshInstance[MajorMID_END];
	CKTDGXMeshPlayer::CXMeshInstanceHandle				m_ahMinorMeshInstance[MinorMID_END];

	int			m_iSuccessiveHit_OneZ;
	D3DXVECTOR3 m_vSummonedUnitBonePos;
	EG_QueensThroneData	m_sEGQueensThrone;
	CKTDXTimer			m_TimerEGQTRefreshMsg;

	CX2EffectSet::Handle	m_hOberonGuard;

	MEGA_ELECTRONBALL_Data	m_sMegaElectronBallData;
	float					m_fEquippedActiveSkill[EASI_END];	//������ ��Ƽ�� ��ų�� üũ������ ��������.
	float					m_fElectronBallRangeRate;			//�Ϸ�Ʈ�к� ������ ���� �Ÿ������� ����.
	float					m_fMPConsumeRateElectronBall;		//�Ϸ�Ʈ�к� ���� �����Ҹ�

//#ifdef EVE_SECOND_CLASS_CHANGE
	// oasis907 : ����� [2010.1.29] // �������� �нú� ��ų - �Ϸ�Ʈ�к� ������
	float					m_fAttackPowerRateElectronBall;		// ������ ����
	float					m_fForceDownValueRateElectronBall;  // �ٿ� ��ġ ����

//#endif EVE_SECOND_CLASS_CHANGE

	ATOveronGuardData		m_sATOveronGuardData;

	bool					m_bIsWinMotion2;
	bool					m_bIsLoseMotion2;

//{{ kimhc // 2010.2.10  // �̺� 2������
	bool					m_bDoneSpearBurst;	//	�̺� ���Ǿ� ����Ʈ�� ���� ����� �ĺ� ��
//}} kimhc // 2010.2.10 //	�̺� 2������

	//{{ JHKang / ������ / 2010/11/22 // ����� ���� ����
	CKTDXTimer					m_timerAtomicShieldSummoned;	/// ����� ���� Ÿ�̸�
	float						m_fAtomicShieldLifeTime;		/// ����� ���� ���� �ð�
	//}} JHKang /  / 2010/11/22 // ����� ���� ����

	EveElectraSystem			m_EveElectraSystem;
	float						m_fPlaneZAngle;

	bool						m_bBlinkInvisibility;
	float						m_fBlinkInvisibilityAlpha;

	float						m_iPhotonBoosterAdditionalDashJumpCount;
	float						m_fPhotonBlinkBackSpeedX;
	float						m_fPhotonBlinkDummyLifeTime;

	CX2DamageEffect::CEffectHandle   	m_hEffectSweepParadeTriangle;
	CX2DamageEffect::CEffectHandle	    m_hEffectSweepParadeElectric;
	CX2DamageEffect::CEffectHandle	    m_hEffectSweepParadeLight;
	CX2DamageEffect::CEffectHandle	    m_hEffectElectraDashComboZ;
	CX2DamageEffect::CEffectHandle	    m_hEffectElectraDashComboZZ;
	CX2DamageEffect::CEffectHandle	    m_hEffectElectraLaserZZZfrontZ;
	CX2DamageEffect::CEffectHandle	    m_hEffectElectraLaserXXZ;
	D3DXVECTOR3					m_vInitScaleByUnitSweepParade;
	float						m_fInitDamageSweepParade;
	float						m_fSweepParadeTriangleLIfeTime;

	bool						m_bXXZLaserEnd;
	bool						m_bNotEnoughMPZZZfrontZLaser;
	bool						m_bNotEnoughMPXXZLaser;
	bool						m_bNotEnoughMPDashZZZ;


	CKTDGXMeshPlayer::CXMeshInstanceHandle  m_hElectraDashComboZMesh;
	//CKTDGXMeshPlayer::CXMeshInstanceHandle  m_hElectraDashComboZMeshZ;



	CKTDGParticleSystem::CParticleEventSequenceHandle	m_hCountDownThousandStar;

	float						m_fElapsedTimeThousandStartCreated;

	bool						m_bLockOnNpc;
	UidType						m_iLockOnUid;
	bool						m_bDelayAttackThousandStar;
	float						m_fDelayAttackTime;

	bool						m_bEntireFireMode;
	float						m_fDelayAttackTimeEntireFire;

	int							m_iThousandStarCount;

	float						m_fThousandStarPowerRate;


	float						m_fThousandStarAttackTime;
	FlyingImpactData*			m_pThousandStar;

	bool						m_bDashJumpXXLaserEnd;				/// ������ ����
	bool						m_bNotEnoughMPDashJumpXZLaser;		/// MP �����
	bool						m_bExceptionPlane;					/// �������� ������ ��� Ȯ��
	float						m_fExceptioncPlaneZAngle;			/// �������� ������ ������
    CX2DamageEffect::CEffectHandle	    m_hEffectElectraLaserDashJumpXZ;		/// �뽬 ���� XX ������

	CKTDGParticleSystem::CParticleEventSequenceHandle	m_hBoostDashParticle;		/// �ν�Ʈ �뽬 ��ƼŬ ����Ʈ
	//CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hBoostDashMesh;			/// �ν�Ʈ �뽬 �޽� ����Ʈ

	TASER_PILUM_Data	m_sTaserPilumData;				/// ������ �ʶ� ������, ������ ��ų( �ް� �Ϸ�Ʈ�к� ���� )
	bool				m_bHighPoweredElectrics;		/// ����� ����ȸ�� �нú�
	float				m_fHighPoweredElectricsSpeed;	/// ����� ����ȸ�� �нú� ��ų �ν�Ʈ ��� �ӵ� ���� ��
	int					m_iAccumulateAccel;				/// �ν�Ʈ �뽬 1�� �ӵ� ���� Count( 1�� : 0.05%, 2�� : 0.1%, 3�� : 0.15% )

	TASER_PILUM_TinyData m_sTaserPilumTinyData;			/// ������ �ʶ��� �����Ͱ� �߻� ���� ����Ǳ� ������ �߻� �� ���̴� ������ �߰�

	SKugelBlitz_Data			m_sKugelBlitz_Data;
	SKugelBlitz_TinyData		m_sKugelBlitz_TinyData;

	float		m_fEffectStartTime;
	float		m_fAddSpectrumTime;
	float		m_fAddEnergeticHeartTime;
	D3DXVECTOR3	m_vSonicGunRotate;

	ENERGY_NEEDLES_Data	m_EnergyNiddleData;		/// ������ �ϵ齺 ó��

	bool	m_bNotEnoughMPDashJumpZX;	/// �ν�Ʈ ���� ���� MP
	short	m_iBoostAttackCount;		/// �ν�Ʈ ���� ���� Ƚ��
	bool	m_bBoostUpAttack;			/// false �� ���� Up ������ ������

	int		m_iAddBoostAttack;		/// �ν�Ʈ �߰� ���� Ƚ��
	bool	m_bNotEnoughMPDashZX;	/// �ν�Ʈ ���� ����ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_End

	CX2EffectSet::Handle	m_hEffectEnergeticHeart;	/// ������ƽ ��Ʈ ����Ʈ

	bool	m_bStartSpectroElCrystal;	/// �б������� ��ų ����� ���� ���� �Ǿ����� ����

	bool		m_bStartLinkOverChargeIllusion; //��ũ �������� �Ϸ��� ��ų�� �����Ͽ����� ����
	int			m_iFerdinandNPCUID;	//��ũ �������� �Ϸ��� npc �丣�𳭵�
	CKTDXTimer	m_timerFerdinandSummoned;	/// ��ũ �������� �Ϸ��� npc �丣�𳭵� ��ȯ Ÿ�̸�
	float		m_fFerdinandLifeTime;		/// ��ũ �������� �Ϸ��� npc �丣�𳭵� ���� �ð�

	BUFF_TEMPLET_ID m_eReserveEndElCrystalBuffID;	// ���� ���� ������ ���� ID
	bool			m_bIsCheckEndElCrystal;			// ���� ���� üũ ����

	float										m_fDecraseInvisibilityMP;		// Ŭ��ŷ �ʴ� ���� ���ҷ�, SA ������ ���̳ʽ� ������ ǥ��


	D3DXVECTOR3 m_vecBoneILLUSIONSTINGER1;
	D3DXVECTOR3 m_vecBoneILLUSIONSTINGER2;



protected:
	//ENSI_DIE_FRONT
	void ENSI_DIE_FRONT_StartFuture();
	void ENSI_DIE_FRONT_Start();
	void ENSI_DIE_FRONT_FrameMove();
	void ENSI_DIE_FRONT_EventProcess();

	//ENSI_DIE_BACK
	void ENSI_DIE_BACK_StartFuture();
	void ENSI_DIE_BACK_Start();
	void ENSI_DIE_BACK_FrameMove();
	void ENSI_DIE_BACK_EventProcess();

	//ENSI_WAIT
	void ENSI_WAIT_StartFuture();
	void ENSI_WAIT_Start();
	void ENSI_WAIT_EventProcess();

	//ENSI_WALK
	void ENSI_WALK_EventProcess();

	//ENSI_JUMP_READY
	void ENSI_JUMP_READY_EventProcess();

	//ENSI_JUMP_UP
	void ENSI_JUMP_UP_EventProcess();

	//ENSI_JUMP_DOWN
	void ENSI_JUMP_DOWN_EventProcess();

	//ENSI_JUMP_LANDING
	void ENSI_JUMP_LANDING_StartFuture();
	void ENSI_JUMP_LANDING_EventProcess();

	//ENSI_DASH
	virtual void DashStartFuture();
	void ENSI_DASH_Start();
	void ENSI_DASH_FrameMoveFuture();
	void ENSI_DASH_EventProcess();

	//ENSI_DASH_END
	void ENSI_DASH_END_StartFuture();
	void ENSI_DASH_END_Start();
	void ENSI_DASH_END_FrameMove();
	void ENSI_DASH_END_EventProcess();

	//ENSI_DASH_JUMP
	void ENSI_DASH_JUMP_StartFuture();
	void ENSI_DASH_JUMP_Start();
	void ENSI_DASH_JUMP_FrameMoveFuture();
	void ENSI_DASH_JUMP_EventProcess();
	void ENSI_DASH_JUMP_EndFuture();

	//ENSI_DASH_JUMP_LANDING
	void ENSI_DASH_JUMP_LANDING_StartFuture();
	void ENSI_DASH_JUMP_LANDING_Start();
	void ENSI_DASH_JUMP_LANDING_EventProcess();

	//ENSI_DASH_JUMP_POWER_LANDING
	void ENSI_DASH_JUMP_POWER_LANDING_StartFuture();
	void ENSI_DASH_JUMP_POWER_LANDING_Start();
	void ENSI_DASH_JUMP_POWER_LANDING_EventProcess();




	void ENSI_JUMP_FLY_UP_StartFuture();
	void ENSI_JUMP_FLY_UP_Start();
	void ENSI_JUMP_FLY_UP_FrameMoveFuture();
	void ENSI_JUMP_FLY_UP_EventProcess();
	void ENSI_JUMP_FLY_UP_EndFuture();



	void ENSI_JUMP_FLY_DOWN_StartFuture();
	void ENSI_JUMP_FLY_DOWN_FrameMoveFuture();
	void ENSI_JUMP_FLY_DOWN_EventProcess();
	void ENSI_JUMP_FLY_DOWN_EndFuture();



	void ENSI_JUMP_DASH_StartFuture();
	void ENSI_JUMP_DASH_Start();
	void ENSI_JUMP_DASH_FrameMoveFuture();
	void ENSI_JUMP_DASH_EventProcess();
	void ENSI_JUMP_DASH_EndFuture();





	//ENSI_HYPER_MODE
	void HyperModeFrameMove();
	void RidingHyperModeFrameMove();
	void CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ = false );

	//ENSI_DAMAGE_GROGGY
	//void ENSI_DAMAGE_GROGGY_FrameMoveFuture();
	//void ENSI_DAMAGE_GROGGY_EventProcess();

	//ENSI_DAMAGE_SMALL_FRONT
	void ENSI_DAMAGE_SMALL_FRONT_EventProcess();

	//ENSI_DAMAGE_SMALL_BACK
	void ENSI_DAMAGE_SMALL_BACK_EventProcess();

	//ENSI_DAMAGE_BIG_FRONT
	void ENSI_DAMAGE_BIG_FRONT_Start();
	void ENSI_DAMAGE_BIG_FRONT_EventProcess();

	//ENSI_DAMAGE_BIG_BACK
	void ENSI_DAMAGE_BIG_BACK_Start();
	void ENSI_DAMAGE_BIG_BACK_EventProcess();

	//ENSI_DAMAGE_DOWN_FRONT
	void ENSI_DAMAGE_DOWN_FRONT_FrameMove();
	void ENSI_DAMAGE_DOWN_FRONT_EventProcess();

	//ENSI_DAMAGE_DOWN_BACK
	void ENSI_DAMAGE_DOWN_BACK_FrameMove();
	void ENSI_DAMAGE_DOWN_BACK_EventProcess();

	//ENSI_DAMAGE_AIR_DOWN_LANDING
	void ENSI_DAMAGE_AIR_DOWN_LANDING_EventProcess();

	//ENSI_STANDUP_ROLLING_FRONT_FRONT
	void ENSI_STANDUP_ROLLING_FRONT_FRONT_EventProcess();
	void ENSI_STANDUP_ROLLING_FRONT_FRONT_End();

	//ENSI_STANDUP_ROLLING_FRONT_BACK
	void ENSI_STANDUP_ROLLING_FRONT_BACK_EventProcess();
	void ENSI_STANDUP_ROLLING_FRONT_BACK_End();

	//ENSI_STANDUP_ROLLING_BACK_FRONT
	void ENSI_STANDUP_ROLLING_BACK_FRONT_EventProcess();
	void ENSI_STANDUP_ROLLING_BACK_FRONT_End();

	//ENSI_STANDUP_ROLLING_BACK_BACK
	void ENSI_STANDUP_ROLLING_BACK_BACK_EventProcess();
	void ENSI_STANDUP_ROLLING_BACK_BACK_End();

	//ENSI_STANDUP_ATTACK_FRONT
	void ENSI_STANDUP_ATTACK_FRONT_FrameMove();
	void ENSI_STANDUP_ATTACK_FRONT_EventProcess();
	void ENSI_STANDUP_ATTACK_FRONT_End();

	//ENSI_STANDUP_ATTACK_BACK
	void ENSI_STANDUP_ATTACK_BACK_FrameMove();
	void ENSI_STANDUP_ATTACK_BACK_EventProcess();
	void ENSI_STANDUP_ATTACK_BACK_End();




	void ENSI_COMBO_Z_Start();
	void ENSI_COMBO_Z_End();
	void ENSI_COMBO_Z_FrameMove();
	void ENSI_COMBO_Z_EventProcess();

	void ENSI_COMBO_ZZ_Start();
	void ENSI_COMBO_ZZ_End();
	void ENSI_COMBO_ZZ_FrameMove();
	void ENSI_COMBO_ZZ_EventProcess();

	void ENSI_COMBO_ZZZ_Start();
	void ENSI_COMBO_ZZZ_End();
	void ENSI_COMBO_ZZZ_FrameMove();
	void ENSI_COMBO_ZZZ_EventProcess();

	void ENSI_COMBO_ZZZZ_Start();
	void ENSI_COMBO_ZZZZ_End();
	void ENSI_COMBO_ZZZZ_FrameMove();
	void ENSI_COMBO_ZZZZ_EventProcess();



	void ENSI_COMBO_ZZZX_FrameMove();
	void ENSI_COMBO_ZZZX_EventProcess();



	void ENSI_COMBO_ZZZdownZ_StartFuture();
	void ENSI_COMBO_ZZZdownZ_Start();
	void ENSI_COMBO_ZZZdownZ_End();
	void ENSI_COMBO_ZZZdownZ_FrameMove();
	void ENSI_COMBO_ZZZdownZ_EventProcess();

	//{{ 09.01.21.hoons. ������ �Լ��� ���Ǿ���.
	// : ���� �̺� �׽�Ʈ �ڵ�� �����־ �̸���ü�� ���� ��������.
	void ENSI_COMBO_ZZZdownZZ_Init();
	void ENSI_COMBO_ZZZdownZZ_StartFuture();
	void ENSI_COMBO_ZZZdownZZ_Start();
	void ENSI_COMBO_ZZZdownZZ_End();
	void ENSI_COMBO_ZZZdownZZ_FrameMove();
	void ENSI_COMBO_ZZZdownZZ_EventProcess();
	//}}




	void ENSI_COMBO_X_FrameMove();
	void ENSI_COMBO_X_EventProcess();
	void ENSI_COMBO_XX_FrameMove();
	void ENSI_COMBO_XX_EventProcess();
	void ENSI_COMBO_XXZ4_Init();
	void ENSI_COMBO_XXZ4_FrameMove();
	void ENSI_COMBO_XXZ4_EventProcess();
	void ENSI_COMBO_XXX_FrameMove();
	void ENSI_COMBO_XXX_EventProcess();
	//void ENSI_COMBO_XXfrontX_FrameMove();
	void ENSI_COMBO_XXfrontX_EventProcess();
	void ENSI_COMBO_XXfrontXX_FrameMove();
	void ENSI_COMBO_XXfrontXX_EventProcess();

	//void ENSI_COMBO_XXdownX_FrameMove();
	void ENSI_COMBO_XXdownX_EventProcess();
	void ENSI_COMBO_XXdownXX_FrameMove();
	void ENSI_COMBO_XXdownXX_EventProcess();

	void ENSI_DASH_COMBO_Z_Start();
	void ENSI_DASH_COMBO_Z_End();
	void ENSI_DASH_COMBO_Z_FrameMove();
	void ENSI_DASH_COMBO_Z_EventProcess();



	void ENSI_DASH_COMBO_ZZ_Init();
	void ENSI_DASH_COMBO_ZZ_StartFuture();
	void ENSI_DASH_COMBO_ZZ_Start();
	void ENSI_DASH_COMBO_ZZ_End();
	void ENSI_DASH_COMBO_ZZ_FrameMove();
	void ENSI_DASH_COMBO_ZZ_EventProcess();

	void ENSI_DASH_COMBO_ZZZ_StartFuture();
	void ENSI_DASH_COMBO_ZZZ_Start();
	void ENSI_DASH_COMBO_ZZZ_End();
	void ENSI_DASH_COMBO_ZZZ_FrameMove();
	void ENSI_DASH_COMBO_ZZZ_EventProcess();



	void ENSI_DASH_COMBO_X_FrameMove();
	void ENSI_DASH_COMBO_X_EventProcess();

	void ENSI_DASH_COMBO_XZ_FrameMove();
	void ENSI_DASH_COMBO_XZ_EventProcess();



	void ENSI_JUMP_COMBO_Z_Start();
	void ENSI_JUMP_COMBO_Z_End();
	void ENSI_JUMP_COMBO_Z_FrameMove();
	void ENSI_JUMP_COMBO_Z_EventProcess();

	void ENSI_JUMP_COMBO_X_Start();
	void ENSI_JUMP_COMBO_X_End();
	void ENSI_JUMP_COMBO_X_FrameMove();
	void ENSI_JUMP_COMBO_X_EventProcess();



	void ENSI_JUMP_COMBO_X_LANDING_StartFuture();
	void ENSI_JUMP_COMBO_X_LANDING_Start();
	void ENSI_JUMP_COMBO_X_LANDING_EventProcess();

	void ENSI_DASH_JUMP_COMBO_Z_Start();
	void ENSI_DASH_JUMP_COMBO_Z_End();
	void ENSI_DASH_JUMP_COMBO_Z_FrameMove();
	void ENSI_DASH_JUMP_COMBO_Z_EventProcess();

	void ENSI_DASH_JUMP_COMBO_ZZ_Start();
	void ENSI_DASH_JUMP_COMBO_ZZ_End();
	void ENSI_DASH_JUMP_COMBO_ZZ_FrameMove();
	void ENSI_DASH_JUMP_COMBO_ZZ_EventProcess();

	void ENSI_DASH_JUMP_COMBO_ZZZ_Start();
	void ENSI_DASH_JUMP_COMBO_ZZZ_End();
	void ENSI_DASH_JUMP_COMBO_ZZZ_FrameMove();
	void ENSI_DASH_JUMP_COMBO_ZZZ_EventProcess();

	void ENSI_DASH_JUMP_COMBO_ZX_FrameMove();
	void ENSI_DASH_JUMP_COMBO_ZX_EventProcess();
	void ENSI_DASH_JUMP_COMBO_ZX_End();




	void ENSI_DASH_JUMP_COMBO_X_Start();
	void ENSI_DASH_JUMP_COMBO_X_End();
	void ENSI_DASH_JUMP_COMBO_X_FrameMove();
	void ENSI_DASH_JUMP_COMBO_X_EventProcess();

	void ENSI_DASH_JUMP_COMBO_XZ_Init();
	void ENSI_DASH_JUMP_COMBO_XZ_FrameMove();
	void ENSI_DASH_JUMP_COMBO_XZ_EventProcess();
	void ENSI_DASH_JUMP_COMBO_XZ_End();

//{{ kimhc // 2010.1.25 //	�̺� 2�� ���� ��ų �� �޺�

	// �׸޽ý� �޺�
	void ENSI_NS_DASH_COMBO_ZX_Init();
	void ENSI_NS_DASH_COMBO_ZX_Start();
	void ENSI_NS_DASH_COMBO_ZX_FrameMove();
	void ENSI_NS_DASH_COMBO_ZX_EventProcess();


	void ENSI_NS_DASH_COMBO_ZXX_Init();
	void ENSI_NS_DASH_COMBO_ZXX_FrameMove();
	void ENSI_NS_DASH_COMBO_ZXX_EventProcess();

	void ENSI_NS_COMBO_XXZ_FrameMove();
	void ENSI_NS_COMBO_XXZ_EventProcess();

	void ENSI_NS_COMBO_XXZZ8_FrameMove();
	void ENSI_NS_COMBO_XXZZ8_EventProcess();

	void ENSI_NS_COMBO_XXZZ8Z_FrameMove();
	void ENSI_NS_COMBO_XXZZ8Z_EventProcess();

	// �������� �޺�
	void ENSI_EP_DASH_COMBO_ZZ_Init();
	void ENSI_EP_DASH_COMBO_ZZ_FrameMove();
	void ENSI_EP_DASH_COMBO_ZZ_EventProcess();

	void ENSI_EP_DASH_COMBO_ZZZ_Init();
	void ENSI_EP_DASH_COMBO_ZZZ_FrameMove();
	void ENSI_EP_DASH_COMBO_ZZZ_EventProcess();

	void ENSI_EP_JUMP_COMBO_ZX_Init();
	void ENSI_EP_JUMP_COMBO_ZX_FrameMove();
	void ENSI_EP_JUMP_COMBO_ZX_EventProcess();

	void ENSI_EP_COMBO_XXdownX_Init();
	void ENSI_EP_COMBO_XXdownX_FrameMove();
	void ENSI_EP_COMBO_XXdownX_EventProcess();

	// �׸޽ý� ��ų
	void ENSI_SA_ENS_ATOMIC_BLASTER_Init();
	void ENSI_SA_ENS_ATOMIC_BLASTER_Start();
	void ENSI_SA_ENS_ATOMIC_BLASTER_FrameMove();
	void ENSI_SA_ENS_ATOMIC_BLASTER_EventProcess();
	void ENSI_SA_ENS_ATOMIC_BLASTER_End();


	void ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_Init();
	void ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_Start();
	void ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_FrameMove();
	void ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_EventProcess();
	void ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_End();

	void ENSI_A_ENS_BURST_SPEAR_Init();
	void ENSI_A_ENS_BURST_SPEAR_Start();
	void ENSI_A_ENS_BURST_SPEAR_FrameMove();
	void ENSI_A_ENS_BURST_SPEAR_EventProcess();
	void ENSI_A_ENS_BURST_SPEAR_End();

	void ENSI_A_ENS_BURST_SPEAR_Hyper_Init();
	void ENSI_A_ENS_BURST_SPEAR_Hyper_Start();
	void ENSI_A_ENS_BURST_SPEAR_Hyper_FrameMove();
	void ENSI_A_ENS_BURST_SPEAR_Hyper_EventProcess();
	void ENSI_A_ENS_BURST_SPEAR_Hyper_End();

	void ENSI_A_ENS_BURST_SPEAR_Landing_FrameMove();
	void ENSI_A_ENS_BURST_SPEAR_Landing_EventProcess();

	void ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_FrameMove();
	void ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_EventProcess();



	// oasis907 : ����� [2010.1.28] //  �������� ��ų
	void ENSI_SA_EEP_ELECTRONIC_FIELD_Init();
	void ENSI_SA_EEP_ELECTRONIC_FIELD_FrameMove();
	void ENSI_SA_EEP_ELECTRONIC_FIELD_EventProcess();

	void ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_Init();
	void ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_FrameMove();
	void ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_EventProcess();

	void ENSI_A_EEP_SPIT_FIRE_Init();
	void ENSI_A_EEP_SPIT_FIRE_FrameMove();
	void ENSI_A_EEP_SPIT_FIRE_EventProcess();

	void ENSI_A_EEP_SPIT_FIRE_Hyper_Init();
	void ENSI_A_EEP_SPIT_FIRE_Hyper_FrameMove();
	void ENSI_A_EEP_SPIT_FIRE_Hyper_EventProcess();


//}} kimhc // 2010.1.25 //  �̺� 2�� ���� ��ų �� �޺�
	void ENSI_INVISIBILITY_ON_Start();
	void ENSI_INVISIBILITY_ON_FrameMove();
	void ENSI_INVISIBILITY_ON_EventProcess();
	void ENSI_INVISIBILITY_ON_End();





	// ���⼭ ���� �ʻ��
	void ENSI_SI1_ILLUSION_STRIKE_Init();
	//void ENSI_SI1_ILLUSION_STRIKE_CameraMove();
	void ENSI_SI1_ILLUSION_STRIKE_FrameMove();
	void ENSI_SI1_ILLUSION_STRIKE_EventProcess();
	void ENSI_SI1_ILLUSION_STRIKE_HYPER_FrameMove();
	void ENSI_SI1_ILLUSION_STRIKE_HYPER_EventProcess();


	void ENSI_SI2_ILLUSION_STINGER_Init();
	void ENSI_SI2_ILLUSION_STINGER_Start();
	void ENSI_SI2_ILLUSION_STINGER_End();
	//void ENSI_SI2_ILLUSION_STINGER_CameraMove();
	void ENSI_SI2_ILLUSION_STINGER_FrameMove();
	void ENSI_SI2_ILLUSION_STINGER_EventProcess();

	void ENSI_SI2_ILLUSION_STINGER_HYPER_Start();
	void ENSI_SI2_ILLUSION_STINGER_HYPER_End();
	void ENSI_SI2_ILLUSION_STINGER_HYPER_FrameMove();
	void ENSI_SI2_ILLUSION_STINGER_HYPER_EventProcess();


	void ENSI_SI3_GENERATE_BLACKHOLE_Init();
	//void ENSI_SI3_GENERATE_BLACKHOLE_CameraMove();
	void ENSI_SI3_GENERATE_BLACKHOLE_FrameMove();
	void ENSI_SI3_GENERATE_BLACKHOLE_EventProcess();
	void ENSI_SI3_GENERATE_BLACKHOLE_HYPER_FrameMove();
	void ENSI_SI3_GENERATE_BLACKHOLE_HYPER_EventProcess();







	void ENSI_SI1_DIMENSION_LINK_Init();
	//void ENSI_SI1_DIMENSION_LINK_CameraMove();
	void ENSI_SI1_DIMENSION_LINK_FrameMove();
	void ENSI_SI1_DIMENSION_LINK_EventProcess();
	void ENSI_SI1_DIMENSION_LINK_HYPER_FrameMove();
	void ENSI_SI1_DIMENSION_LINK_HYPER_EventProcess();



	void ENSI_SI2_DIMENSION_LINK_BLADER_Init();
	//void ENSI_SI2_DIMENSION_LINK_BLADER_CameraMove();
	void ENSI_SI2_DIMENSION_LINK_BLADER_FrameMove();
	void ENSI_SI2_DIMENSION_LINK_BLADER_EventProcess();
	void ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_FrameMove();
	void ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_EventProcess();



	void ENSI_SI3_DIMENSION_LINK_GUARDIAN_Init();
	//void ENSI_SI3_DIMENSION_LINK_GUARDIAN_CameraMove();
	void ENSI_SI3_DIMENSION_LINK_GUARDIAN_FrameMove();
	void ENSI_SI3_DIMENSION_LINK_GUARDIAN_EventProcess();
	void ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_FrameMove();
	void ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_EventProcess();

	//COMMON_BUFF
	virtual void COMMON_BUFF_FrameMove();

	// ARCHITECTURE
	void ENSI_SI1_AT_SURFACE_CUTING_Init();
	void ENSI_SI1_AT_SURFACE_CUTING_FrameMove();
	void ENSI_SI1_AT_SURFACE_CUTING_EventProcess();
	void ENSI_SI1_AT_SURFACE_CUTING_HYPER_FrameMove();
	void ENSI_SI1_AT_SURFACE_CUTING_HYPER_EventProcess();

	void ENSI_SI2_AT_SONIC_WAVE_Init();
	void ENSI_SI2_AT_SONIC_WAVE_FrameMove();
	void ENSI_SI2_AT_SONIC_WAVE_EventProcess();
	void ENSI_SI2_AT_SONIC_WAVE_HYPER_FrameMove();
	void ENSI_SI2_AT_SONIC_WAVE_HYPER_EventProcess();

	void ENSI_SI3_AT_GENOCIDE_RIPPER_Init();
	void ENSI_SI3_AT_GENOCIDE_RIPPER_Start();
	void ENSI_SI3_AT_GENOCIDE_RIPPER_FrameMove();
	void ENSI_SI3_AT_GENOCIDE_RIPPER_EventProcess();
	void ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_FrameMove();
	void ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_EventProcess();

	// EXOTIC GEAR
	void ENSI_SI1_EG_HORNET_STING_Init();
	void ENSI_SI1_EG_HORNET_STING_FrameMove();
	void ENSI_SI1_EG_HORNET_STING_EventProcess();
	void ENSI_SI1_EG_HORNET_STING_HYPER_FrameMove();
	void ENSI_SI1_EG_HORNET_STING_HYPER_EventProcess();

	void ENSI_SI2_EG_EXPLOSION_IMPACT_Init();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_Start();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_End();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_FrameMove();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_EventProcess();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_FrameMove();
	void ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_EventProcess();

	void ENSI_SI3_EG_QUEENS_THRONE_Init();
	void ENSI_SI3_EG_QUEENS_THRONE_Start();
	void ENSI_SI3_EG_QUEENS_THRONE_End();
	void ENSI_SI3_EG_QUEENS_THRONE_FrameMove();
	void ENSI_SI3_EG_QUEENS_THRONE_EventProcess();
	void ENSI_SI3_EG_QUEENS_THRONE_HYPER_FrameMove();
	void ENSI_SI3_EG_QUEENS_THRONE_HYPER_EventProcess();


	// ������ȯ
	void ENSI_A_EN_CONVERSION_Init();
	void ENSI_A_EN_CONVERSION_Start();
	void ENSI_A_EN_CONVERSION_EventProcess();

	// �ް��Ϸ�Ʈ�к�
	void ENSI_A_EN_MEGA_ELECTRONBALL_Init();
	void ENSI_A_EN_MEGA_ELECTRONBALL_StartFuture();
	void ENSI_A_EN_MEGA_ELECTRONBALL_Start();
	void ENSI_A_EN_MEGA_ELECTRONBALL_FrameMoveFuture();
	void ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove();
	void ENSI_A_EN_MEGA_ELECTRONBALL_EventProcess();
	void ENSI_A_EN_MEGA_ELECTRONBALL_EndFuture();


	void ENSI_SA_EEG_IRONSCRAPS_Init();
	void ENSI_SA_EEG_IRONSCRAPS_StateStart();
	void ENSI_SA_EEG_IRONSCRAPS_FrameMove();
	void ENSI_SA_EEG_IRONSCRAPS_EventProcess();
	void ENSI_SA_EEG_IRONSCRAPS_StateEnd();

	void ENSI_SA_EEG_IRONSCRAPS_HYPER_Init();
	void ENSI_SA_EEG_IRONSCRAPS_HYPER_StateStart();
	void ENSI_SA_EEG_IRONSCRAPS_HYPER_FrameMove();
	void ENSI_SA_EEG_IRONSCRAPS_HYPER_EventProcess();
	void ENSI_SA_EEG_IRONSCRAPS_HYPER_StateEnd();


	void ENSI_SA_EAT_SPACE_WRENCH_StateStart();
	void ENSI_SA_EAT_SPACE_WRENCH_FrameMove();
	void ENSI_SA_EAT_SPACE_WRENCH_EventProcess();
	void ENSI_SA_EAT_SPACE_WRENCH_StateEnd();

	void ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateStart();
	void ENSI_SA_EAT_SPACE_WRENCH_HYPER_FrameMove();
	void ENSI_SA_EAT_SPACE_WRENCH_HYPER_EventProcess();
	void ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateEnd();

	//ENSI_A_EEG_METAL_DUST_AURA
	void ENSI_A_EEG_METAL_DUST_AURA_Init();
	void ENSI_A_EEG_METAL_DUST_AURA_StateStart();
	void ENSI_A_EEG_METAL_DUST_AURA_FrameMove();
	void ENSI_A_EEG_METAL_DUST_AURA_EventProcess();
	void ENSI_A_EEG_METAL_DUST_AURA_StateEnd();

	void ENSI_A_EEG_METAL_DUST_AURA_HYPER_Init();
	void ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateStart();
	void ENSI_A_EEG_METAL_DUST_AURA_HYPER_FrameMove();
	void ENSI_A_EEG_METAL_DUST_AURA_HYPER_EventProcess();
	void ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateEnd();

	//ENSI_A_EAT_OBERON_GUARD
	void ENSI_A_EAT_OBERON_GUARD_Init();
	void ENSI_A_EAT_OBERON_GUARD_StateStart();
	void ENSI_A_EAT_OBERON_GUARD_FrameMove();
	void ENSI_A_EAT_OBERON_GUARD_EventProcess();
	void ENSI_A_EAT_OBERON_GUARD_StateEnd();

	void ENSI_A_EAT_OBERON_GUARD_HYPER_Init();
	void ENSI_A_EAT_OBERON_GUARD_HYPER_StateStart();
	void ENSI_A_EAT_OBERON_GUARD_HYPER_FrameMove();
	void ENSI_A_EAT_OBERON_GUARD_HYPER_EventProcess();
	void ENSI_A_EAT_OBERON_GUARD_HYPER_StateEnd();


	void ENSI_A_EEG_JUNK_BREAK_Init();
	void ENSI_A_EEG_JUNK_BREAK_FrameMove();
	void ENSI_A_EEG_JUNK_BREAK_Hyper_FrameMove();
	void ENSI_A_EEG_JUNK_BREAK_EventProcess();
	void ENSI_A_EEG_JUNK_BREAK_Hyper_EventProcess();

	void ENSI_A_EAT_HEAVENS_FIST_Init();
	void ENSI_A_EAT_HEAVENS_FIST_FrameMove();
	void ENSI_A_EAT_HEAVENS_FIST_Hyper_FrameMove();
	void ENSI_A_EAT_HEAVENS_FIST_EventProcess();
	void ENSI_A_EAT_HEAVENS_FIST_Hyper_EventProcess();

	//{{ JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�
	void ENSI_SA_EAT_ASSUALT_SPEAR_Init();
	void ENSI_SA_EAT_ASSUALT_SPEAR_FrameMove();
	void ENSI_SA_EAT_ASSUALT_SPEAR_EventProcess();
	void ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_FrameMove();
	void ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_EventProcess();
	//}} JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�

	//{{ JHKang / ������ / 2010/11/11 / ����� ����
	void ENSI_SA_EEG_ATOMIC_SHIELD_Init();
	void ENSI_SA_EEG_ATOMIC_SHIELD_StateStart();
	void ENSI_SA_EEG_ATOMIC_SHIELD_FrameMove();
	void ENSI_SA_EEG_ATOMIC_SHIELD_EventProcess();
	void ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_StateStart();
	void ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_FrameMove();
	void ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_EventProcess();
	//}} JHKang / ������ / 2010/11/11 / ����� ����

	void ShowActiveSkillCutInAndLight( const WCHAR* szBoneName_, const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ = false );
	virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

	void ENSI_COMBO_X_Start();
	void ENSI_COMBO_XX_Start();
	void ENSI_COMBO_XXX_Start();

	void ENSI_EPR_COMBO_ZZZfrontZ_Start();
	void ENSI_EPR_COMBO_ZZZfrontZ_End();
	void ENSI_EPR_COMBO_ZZZfrontZ_FrameMove();
	void ENSI_EPR_COMBO_ZZZfrontZ_EventProcess();


	void ENSI_EPR_COMBO_XXZ_Start();
	void ENSI_EPR_COMBO_XXZ_End();
	void ENSI_EPR_COMBO_XXZ_FrameMove();
	void ENSI_EPR_COMBO_XXZ_EventProcess();

	void ENSI_EPR_DASH_COMBO_ZZ_Init();
	void ENSI_EPR_DASH_COMBO_ZZ_StartFuture();
	void ENSI_EPR_DASH_COMBO_ZZ_Start();
	void ENSI_EPR_DASH_COMBO_ZZ_EventProcess();
	void ENSI_EPR_DASH_COMBO_ZZ_FrameMove();
	void ENSI_EPR_DASH_COMBO_ZZ_End();

	void ENSI_EPR_DASH_COMBO_ZZZ_Init();
	void ENSI_EPR_DASH_COMBO_ZZZ_StartFuture();
	void ENSI_EPR_DASH_COMBO_ZZZ_Start();
	void ENSI_EPR_DASH_COMBO_ZZZ_EventProcess();
	void ENSI_EPR_DASH_COMBO_ZZZ_FrameMove();
	void ENSI_EPR_DASH_COMBO_ZZZ_EndFuture();
	void ENSI_EPR_DASH_COMBO_ZZZ_End();

	void ENSI_A_EVE_PHOTON_BLINK_Init();
	void ENSI_A_EVE_PHOTON_BLINK_Start();
	void ENSI_A_EVE_PHOTON_BLINK_StartFuture();
	void ENSI_A_EVE_PHOTON_BLINK_FrameMoveFuture();
	void ENSI_A_EVE_PHOTON_BLINK_FrameMove();
	void ENSI_A_EVE_PHOTON_BLINK_EventProcess();
	void ENSI_A_EVE_PHOTON_BLINK_EndFuture();
	void ENSI_A_EVE_PHOTON_BLINK_End();


	void ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Init();
	void ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Start();
	void ENSI_A_EPR_SPECTRO_EL_CRYSTAL_FrameMove();
	void ENSI_A_EPR_SPECTRO_EL_CRYSTAL_EventProcess();

	void ENSI_SA_EPR_PARTICLE_RAY_Init();
	void ENSI_SA_EPR_PARTICLE_RAY_Start();
	void ENSI_SA_EPR_PARTICLE_RAY_StartFuture();
	void ENSI_SA_EPR_PARTICLE_RAY_FrameMoveFuture();
	void ENSI_SA_EPR_PARTICLE_RAY_FrameMove();
	void ENSI_SA_EPR_PARTICLE_RAY_EventProcess();
	void ENSI_SA_EPR_PARTICLE_RAY_End();

	void ENSI_SA_EPR_PHOTON_FLARE_Init();
	void ENSI_SA_EPR_PHOTON_FLARE_StateStart();
	void ENSI_SA_EPR_PHOTON_FLARE_FrameMove();
	void ENSI_SA_EPR_PHOTON_FLARE_EventProcess();
	void ENSI_SA_EPR_PHOTON_FLARE_StateEnd();

	void ENSI_SA_EPR_SWEEP_ROLLING_READY_Init();
	void ENSI_SA_EPR_SWEEP_ROLLING_READY_Start();
	void ENSI_SA_EPR_SWEEP_ROLLING_READY_FrameMove();
	void ENSI_SA_EPR_SWEEP_ROLLING_READY_EventProcess();
	void ENSI_SA_EPR_SWEEP_ROLLING_READY_End();

	void ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_Start();
	void ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_FrameMove();
	void ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_EventProcess();
	void ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_End();

	void ENSI_SA_EPR_SWEEP_ROLLING_FIRE_Start();
	void ENSI_SA_EPR_SWEEP_ROLLING_FIRE_FrameMove();
	void ENSI_SA_EPR_SWEEP_ROLLING_FIRE_EventProcess();
	void ENSI_SA_EPR_SWEEP_ROLLING_FIRE_End();

	void ENSI_SA_EPR_SWEEP_ROLLING_END_Start();
	void ENSI_SA_EPR_SWEEP_ROLLING_END_FrameMove();
	void ENSI_SA_EPR_SWEEP_ROLLING_END_EventProcess();
	void ENSI_SA_EPR_SWEEP_ROLLING_END_End();



	void ENSI_SA_EPR_THOUSANDS_OF_STARS_Init();
	void ENSI_SA_EPR_THOUSANDS_OF_STARS_FrameMove();
	void ENSI_SA_EPR_THOUSANDS_OF_STARS_EventProcess();


	void ENSI_SA_EPR_GIGA_STREAM_Init();
	void ENSI_SA_EPR_GIGA_STREAM_Start();
	void ENSI_SA_EPR_GIGA_STREAM_StartFuture();
	void ENSI_SA_EPR_GIGA_STREAM_FrameMoveFuture();
	void ENSI_SA_EPR_GIGA_STREAM_FrameMove();
	void ENSI_SA_EPR_GIGA_STREAM_EventProcess();
	void ENSI_SA_EPR_GIGA_STREAM_End();

	//#pragma region ������ �ʶ�
	void ENSI_A_EBS_TASER_PILUM_Init();
	void ENSI_A_EBS_TASER_PILUM_StartFuture();
	void ENSI_A_EBS_TASER_PILUM_Start();
	void ENSI_A_EBS_TASER_PILUM_FrameMoveFuture();
	void ENSI_A_EBS_TASER_PILUM_FrameMove();
	void ENSI_A_EBS_TASER_PILUM_EventProcess();
	void ENSI_A_EBS_TASER_PILUM_EndFuture();
	//#pragma endregion JHKang / ������ / 2012. 7. 10

	//#pragma region �ν�Ʈ �뽬 ��ŸƮ
	void ENSI_EBS_BOOST_DASH_START_StartFuture();
	void ENSI_EBS_BOOST_DASH_START_EventProcess();
	void ENSI_EBS_BOOST_DASH_START_EndFuture();
	//#pragma endregion JHKang / ������ / 2012. 7. 21

	//#pragma region �ν�Ʈ �뽬
	void ENSI_EBS_BOOST_DASH_StartFuture();
	void ENSI_EBS_BOOST_DASH_Start();
	void ENSI_EBS_BOOST_DASH_FrameMove();
	void ENSI_EBS_BOOST_DASH_FrameMoveFuture();
	void ENSI_EBS_BOOST_DASH_EventProcess();
	void ENSI_EBS_BOOST_DASH_End();
	//#pragma endregion JHKang / ������ / 2012. 7. 21

	//#pragma region �ν�Ʈ �뽬 ���� ��ŸƮ(�� ���� ���� �� �뽬��)
	void ENSI_EBS_BOOST_DASH_AIR_START_StartFuture();
	void ENSI_EBS_BOOST_DASH_AIR_START_Start();
	void ENSI_EBS_BOOST_DASH_AIR_START_FrameMoveFuture();
	void ENSI_EBS_BOOST_DASH_AIR_START_EventProcess();
	void ENSI_EBS_BOOST_DASH_AIR_START_EndFuture();
	//#pragma endregion JHKang / ������ / 2012. 7. 23

	//#pragma region �ν�Ʈ �뽬 ����
	void ENSI_EBS_BOOST_DASH_AIR_StartFuture();
	void ENSI_EBS_BOOST_DASH_AIR_Start();
	void ENSI_EBS_BOOST_DASH_AIR_FrameMove();
	void ENSI_EBS_BOOST_DASH_AIR_FrameMoveFuture();
	void ENSI_EBS_BOOST_DASH_AIR_EventProcess();
	void ENSI_EBS_BOOST_DASH_AIR_EndFuture();
	void ENSI_EBS_BOOST_DASH_AIR_End();
	//#pragma endregion JHKang / ������ / 2012. 7. 23

	//#pragma region �뽬 ���� �޺� X(Push)Z
	void ENSI_EBS_DASH_JUMP_COMBO_XZ_Start();
	void ENSI_EBS_DASH_JUMP_COMBO_XZ_FrameMove();
	void ENSI_EBS_DASH_JUMP_COMBO_XZ_EventProcess();
	void ENSI_EBS_DASH_JUMP_COMBO_XZ_EndFuture();
	void ENSI_EBS_DASH_JUMP_COMBO_XZ_End();
	//#pragma endregion JHKang / ������ / 2012. 7. 27

	//#pragma region �뽬 ���� �޺� ZXXX, (����) �ν�Ʈ �뽬 XXX Front, Up, Down
	void ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_Start();
	void ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_FrameMove();
	void ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EventProcess();
	void ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EndFuture();
	void ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_End();

	void ENSI_EBS_DASH_JUMP_COMBO_ZUpX_Start();
	void ENSI_EBS_DASH_JUMP_COMBO_ZUpX_FrameMove();
	void ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EventProcess();
	void ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EndFuture();
	void ENSI_EBS_DASH_JUMP_COMBO_ZUpX_End();

	void ENSI_EBS_DASH_JUMP_COMBO_ZDownX_Start();
	void ENSI_EBS_DASH_JUMP_COMBO_ZDownX_FrameMove();
	void ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EventProcess();
	void ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EndFuture();
	void ENSI_EBS_DASH_JUMP_COMBO_ZDownX_End();

	void StateChangeBoostAttackIfPossible();
	//#pragma endregion JHKang / ������ / 2012. 7. 30

	//#pragma region ������ �ϵ齺 1��
	void ENSI_EBS_ENERGY_NEEDLES_Init();
	void ENSI_EBS_ENERGY_NEEDLES_Start();
	void ENSI_EBS_ENERGY_NEEDLES_FrameMove();
	void ENSI_EBS_ENERGY_NEEDLES_EventProcess();
	void ENSI_EBS_ENERGY_NEEDLES_End();
	//#pragma endregion JHKang / ������ / 2012. 8. 1

	//#pragma region ������ƽ ��Ʈ 2��
	void ENSI_EBS_ENERGETIC_HEART_Start();
	void ENSI_EBS_ENERGETIC_HEART_FrameMove();
	void ENSI_EBS_ENERGETIC_HEART_EventProcess();
	void ENSI_EBS_ENERGETIC_HEART_End();
	//#pragma endregion JHKang / ������ / 2012. 8

	//#pragma region ���� ���� Start �Լ� �и�
	void SPECTRO_EL_CRYSTAL_Start();
	//#pragma endregion

	//#pragma region SI_SA_EN_ILLUSION_RAY
	void ENSI_A_EN_Illusion_Ray_Init();
	void ENSI_A_EN_Illusion_Ray_Start();
	void ENSI_A_EN_Illusion_Ray_StartFuture();
	void ENSI_A_EN_Illusion_Ray_FrameMoveFuture();
	void ENSI_A_EN_Illusion_Ray_FrameMove();
	void ENSI_A_EN_Illusion_Ray_EventProcess();
	void ENSI_A_EN_Illusion_Ray_End();
	//#pragma endregion �Ϸ��� ����

	//#pragma region SI_A_EN_TESLA_SHOCK
	void ENSI_A_EN_Tesla_Shock_Init();
	void ENSI_A_EN_Tesla_Shock_StateStart();
	void ENSI_A_EN_Tesla_Shock_EventProcess();
	void ENSI_A_EN_Tesla_Shock_StateEnd();
	//#pragma endregion �׽��� ��ũ

	//#pragma region SI_SA_EEG_HORNET_STING_SHAPED_CHARGE
	void ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_Init();
	void ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_FrameMove();
	void ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_EventProcess();
	//#pragma endregion ȣ�� ���� - ������Ʈ ����

	//#pragma region SI_SA_ENS_ATOMIC_BLASTER_SONICGUN
	void ENSI_SA_ENS_Atomic_Blaster_SonicGun_Init();
	void ENSI_SA_ENS_Atomic_Blaster_SonicGun_Start();
	void ENSI_SA_ENS_Atomic_Blaster_SonicGun_FrameMove();
	void ENSI_SA_ENS_Atomic_Blaster_SonicGun_EventProcess();
	//#pragma endregion ����� �������� - �Ҵа�

	//#pragma region SI_SA_SA_EEL_SWEEP_ROLLING_TRI_VULCAN
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Init();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Start();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_FrameMove();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_EventProcess();

	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_Start();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_FrameMove();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_EventProcess();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_End();

	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_Start();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_FrameMove();
	void ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_EventProcess();
	//#pragma endregion ������ �Ѹ� - Ʈ���� ��ĭ

	//#pragma region SI_SA_EAT_HEAVENS_FIST_SWEEPER
	void ENSI_SA_EAT_Heavens_Fist_Sweeper_Init();
	void ENSI_SA_EAT_Heavens_Fist_Sweeper_FrameMove();
	void ENSI_SA_EAT_Heavens_Fist_Sweeper_EventProcess();
	//#pragma endregion ����� �ǽ�Ʈ - ������

	//#pragma region SI_SA_ENS_ASSAULT_SPEAR
	void ENSI_SA_ENS_Assualt_Spear_Init();
	void ENSI_SA_ENS_Assualt_Spear_FrameMove();
	void ENSI_SA_ENS_Assualt_Spear_EventProcess();
	//#pragma endregion �Ʈ ���Ǿ� - �׸޽ý�

	//#pragma region SI_A_EEP_SPIT_FIRE_GRENADE
	void ENSI_A_EEP_Spit_Fire_Grenade_Init();
	void ENSI_A_EEP_Spit_Fire_Grenade_FrameMove();
	void ENSI_A_EEP_Spit_Fire_Grenade_EventProcess();
	//#pragma endregion ���� ���̾� - �׷����̵�

	//#pragma region SI_A_EBS_KUGELBLITZ
	void ENSI_A_EBS_Kugelblitz_StartFuture();
	void ENSI_A_EBS_Kugelblitz_Start();
	void ENSI_A_EBS_Kugelblitz_FrameMoveFuture();
	void ENSI_A_EBS_Kugelblitz_FrameMove();
	void ENSI_A_EBS_Kugelblitz_EventProcess();
	void ENSI_A_EBS_Kugelblitz_EndFuture();
	//#pragma endregion ��Ŭ������

	//#pragma region SI_SA_EEL_LINEAR_DIVIDER
	void ENSI_SA_EEL_LINEAR_DIVIDER_Init();
	void ENSI_SA_EEL_LINEAR_DIVIDER_Start();
	void ENSI_SA_EEL_LINEAR_DIVIDER_StartFuture();
	void ENSI_SA_EEL_LINEAR_DIVIDER_FrameMoveFuture();
	void ENSI_SA_EEL_LINEAR_DIVIDER_FrameMove();
	void ENSI_SA_EEL_LINEAR_DIVIDER_EventProcess();
	void ENSI_SA_EEL_LINEAR_DIVIDER_End();
	//#pragma endregion �巹�� ��ũ


	// ACTIVE
	// ENSI_SA_EPR_SWEEP_ROLLING - X
	// ENSI_SA_EPR_GIGA_STREAM


	//��������, ��ũ �������� �Ϸ���
	void ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Init();
	void ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Start();
	void ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_FrameMove();
	void ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_EventProcess();
	void ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_End();


	//�׸޽ý�, �糪ƽ ��Ŀ��
	void ENSI_HA_ENS_LUNATIC_SCUD_Init();
	void ENSI_HA_ENS_LUNATIC_SCUD_EventProcess();


	// ��Ʋ ������, ����ű ��ƿ����
	void ENSI_HA_EBS_PSYCHIC_ARTILLERY_Init();
	void ENSI_HA_EBS_PSYCHIC_ARTILLERY_FrameMove();
	void ENSI_HA_EBS_PSYCHIC_ARTILLERY_EventProcess();


	//#pragma region ���� ������Ʈ�� �ƴ� �� ���� ���� ����ϴ� �Լ�
		void PlaySoundAtNoAttackState( const wstring& wstrSoundFileName_ );
	//#pragma endregion

};
