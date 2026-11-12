#pragma once
#include "./X2SoundCloseManager.h"


class CX2DamageEffect
{
	public:
        typedef CX2DamageEffect_CEffectHandle   CEffectHandle;

		enum EFFECT_TYPE
		{
			DEET_NONE = 0,
			DEET_BASIC,
		};

		enum POSITION_TYPE
		{
			DEPT_BONE = 0,
			DEPT_UNIT_BONE,
			DEPT_IMPACT_POINT,
			DEPT_POS,
//#ifdef RAVEN_WEAPON_TAKER
			DEPT_DIE_POS,
//#endif RAVEN_WEAPON_TAKER
		};

		enum UNIT_SELECT
		{
			DEUS_ALL = 0,
			DEUS_FOCUS,
			DEUS_MY,
		};


		enum LOCK_ON_TYPE
		{
			LOT_NONE = 0,
			LOT_POS_BLACK_HOLE,
			LOT_UID_BLACK_HOLE,
			LOT_POS_VECTOR,
			LOT_UID_VECTOR,
			//{{ kimhc // 2010-12-17 // 가장 가까운 유닛 선택
			LOT_NEARST_UID_VECTOR,
			//}} kimhc // 2010-12-17 // 가장 가까운 유닛 선택
		//#ifdef CHUNG_SECOND_CLASS_CHANGE
			LOT_NEARST_UID_VECTOR_IN_RANGE,
		//#endif
			LOT_UID_BONE_POS,		/// 록온된 유저의 특정 본을 록온
		};

		enum LOCK_ON_CHECK_SAVE_DIRSPEED
		{
			LOCSD_NONE = 0,				//NOT_FIND_TARGET_MOVE_STOP 옵션이 설정되지 않은 일반 LOCK_ON_DATA
			LOCSD_SAVE_DIR_SPEED,		// NOT_FIND_TARGET_MOVE_STOP 옵션이 설정됨. 현재 DIR_SPEED값을 저장하고 있음.
			LOCSD_NOT_SAVE_DIR_SPEED,	// NOT_FIND_TARGET_MOVE_STOP 옵션이 설정됨. 현재 DIR_SPEED값을 저장하고 있지 않음.
		};

		enum  DAMAGE_EFFECT_TYPE
		{
			DET_NONE = -1,								/// -1 - None
			DET_ARROW_OF_EXPLOSION,						///  0 - 폭발의 화살
			DET_ARROW_OF_EXPLOSION_DELAY,				///  1 - 폭발의 화살 ( 지연 폭파 모드 )
			DET_ARROW_OF_EXPLOSION_DELAY_GROUND,		///  2 - 땅에 적용된 폭발의 화살 ( 지연 폭파 모드 )
			DET_START_OF_DELAYED_FIRING,				///  3 - 지연의 신호탄
			DET_START_OF_DELAYED_FIRING_GROUND,			///  4 - 땅에 적용된 지연의 신호탄
			DET_EXPLOSION_TRAP,							///  5 - 폭발의 덫
			DET_EXPLOSION_TRAP_GROUND,					///  6 - 땅에 적용된 폭발의 덫
			DET_ARROW_OF_EXPLOSION_READY,				///  7 - 폭발 대기
			DET_START_OF_DELAYED_FIRING_READY,			///  8 - 폭발 대기
//#ifdef UPGRADE_SKILL_SYSTEM_2013 //김창한
			DET_TRAPPING_ARROW_BURSTR,					///  9 - 트래핑 애로우 버스터
			DET_TRAPPING_ARROW_BURSTR_GROUND,			/// 10 - 트래핑 애로우 버스터 땅에 적용됨
			DET_TRAPPING_ARROW_FUNGUS,					/// 11 - 트래핑 애로우 펑거스
			DET_TRAPPING_ARROW_FUNGUS_GROUND,			/// 12 - 트래핑 애로우 펑거스 땅에 적용됨
			DET_SEED_OF_RUIN,							/// 13 - 진노의 씨앗
			DET_SEED_OF_RUIN_GROUND,					//  14 - 진노의 씨앗 - 땅
//#endif //UPGRADE_SKILL_SYSTEM_2013
		};

		/// 데미지 이펙터 내 파티클
		struct ParticleData
		{
			POSITION_TYPE	posType;			/// 위치 형태
			wstring			particleName;		/// 파티클 이름(wide-character)
			wstring			boneName;			/// 위치 형태가 뼈대일 경우, 뼈대 이름
			bool			bApplyRotate;		/// 대상의 회전 변환을 적용할 것인가
			bool			bForceDie;			///
			bool			bForceDieHit;		///
			int				triggerCount;		///
			float			triggerTime;		///
			float			fEventTime;			/// 발생 시간
            float           fOffsetX;			/// 오프셋 위치 X
            float           fOffsetY;			/// 오프셋 위치 Y
            float           fOffsetZ;			/// 오프셋 위치 Z
			D3DXVECTOR3		vScale;				/// 크기 비율
			bool			bTrace;				/// 따라 다닐 것인가
			bool			bTraceMore;
			bool            bApplyRotatedOffset; // added by lucidash
			bool			bUseGlobalTime;
			//{{ kimhc // 2010.2.3 //	이브 2차 전직
			D3DXVECTOR3		vMoveAnglePerFrame;
			D3DXVECTOR3		vRotateAngleDegree;
			//}} kimhc // 2010.2.3 //	이브 2차 전직
			bool			bTraceDefenderUnit;
			bool			bScaleByMainEffectScaleY;
			float			fShowLandHeight;	// 라인맵과의 높이차가 fShowLandHeight 이하일 경우만 보이도록 한다. (DEPT_BONE에만 적용됨)
			bool			m_bReverseY;
			int             m_iShowLevel;

			CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeq;

			ParticleData()
			{
				posType			= DEPT_BONE;
				particleName	= L"";
				boneName		= L"";
				bApplyRotate	= true;
				bForceDie		= false;
				bForceDieHit	= false;
				triggerCount	= -1;
				triggerTime		= -1.0f;
				fEventTime		= -1.0f;

                fOffsetX        = 0.0f;
                fOffsetY        = 0.0f;
                fOffsetZ        = 0.0f;

				vScale			= D3DXVECTOR3( 0, 0, 0 );
				bTrace			= true;
				bTraceMore		= false;
				bApplyRotatedOffset	= false;
				bUseGlobalTime	= false;

				//{{ kimhc // 2010.2.3 //	이브 2차 전직
				vMoveAnglePerFrame = D3DXVECTOR3( 0, 0, 0 );
				vRotateAngleDegree = D3DXVECTOR3( 0, 0, 0 );
				//}} kimhc // 2010.2.3 //	이브 2차 전직
				bTraceDefenderUnit = false;
				bScaleByMainEffectScaleY = false;
				fShowLandHeight = 99999.f;
				m_bReverseY = false;
				m_iShowLevel = CX2GameOption::OL_LOW;

				m_hSeq			= INVALID_PARTICLE_SEQUENCE_HANDLE;
			}

			//{{ kimhc // 2009-10-20 // 독구름 작업 관련 추가
			ParticleData( const ParticleData& t ) { *this = t; }    // copy constructor
			ParticleData& operator=( const ParticleData& t )        // assign operator
			{
				posType			= t.posType;
				particleName	= t.particleName;
				boneName		= t.boneName;
				bApplyRotate	= t.bApplyRotate;
				bForceDie		= t.bForceDie;
				bForceDieHit	= t.bForceDieHit;
				triggerCount	= t.triggerCount;
				triggerTime		= t.triggerTime;
				fEventTime		= t.fEventTime;

				fOffsetX        = t.fOffsetX;
				fOffsetY        = t.fOffsetY;
				fOffsetZ        = t.fOffsetZ;

				vScale			= t.vScale;
				bTrace			= t.bTrace;
				bTraceMore		= t.bTraceMore;

				bUseGlobalTime	= t.bUseGlobalTime;
				//{{ kimhc // 2010.2.3 //	이브 2차 전직
				vMoveAnglePerFrame = t.vMoveAnglePerFrame;
				vRotateAngleDegree = t.vRotateAngleDegree;
				//}} kimhc // 2010.2.3 //	이브 2차 전직
				bTraceDefenderUnit = t.bTraceDefenderUnit;

				m_hSeq			= t.m_hSeq;

				bApplyRotatedOffset = t.bApplyRotatedOffset;
				bScaleByMainEffectScaleY = t.bScaleByMainEffectScaleY;
				fShowLandHeight	= t.fShowLandHeight;

				m_bReverseY = t.m_bReverseY;
				m_iShowLevel = t.m_iShowLevel;

				return *this;
			}
			//}} kimhc // 2009-10-20 // 독구름 작업 관련 추가

			void DestroyParticleSequence();

			void* operator new(size_t s);
			void operator delete(void *p);

		};


		struct DamageEffectData
		{
			POSITION_TYPE	posType;
			wstring			damageEffectName;
			wstring			boneName;
			bool			bApplyRotate;
			float			fEventTime;
			D3DXVECTOR3		vRotateAngleDegree;
            D3DXVECTOR3     vOffset;
			D3DXVECTOR3		vMainEffectScale;
			D3DXVECTOR3		vPassiveParticleScale;
			bool			bApplyRotateOrigin;
			//{{ oasis907 : 김상윤 [2010.2.10] //
			bool			bApplyRotatedOffset;
			//}}
			bool			bApplyMatrix;
			bool			bAutoDie; // 스테이트 이동시 자동 삭제
			bool			bLandCrashOnly;
			bool			bLifeTimeOnly;
			int				iGroupID;	/// 그룹 데미지 설정 추가
			bool			bInheritBuffFactor;		/// 버프팩터를 상속 받을 것인가?
			//bool			bForceDie;
			//bool			bForceDieHit;
			//int			triggerCount;
			//float			triggerTime;
			//CX2DamageEffect	pDamageEffect;
			float			fLifeTime; //코드상에서 DieDamageEffect, HitDamageEffect의 lifetime을 변화시키기 위해 추가
			bool			bOnlyApplyRotateOffset; //vAxisAngleDegree 값은 Hit, Die DamageEffect를 생성하는 DamageEffect의 값을 따라가고
													// vRotateAngleDegree 값만 Hit, Die DamageEffect괄호 안에 정의된 ROTATE_ 값을 받아와서 쓰도록 만듬.

			DamageEffectData()
			{
				posType					= DEPT_BONE;
				damageEffectName		= L"";
				boneName				= L"";
				bApplyRotate			= true;
				fEventTime				= -1.0f;
				vRotateAngleDegree		= D3DXVECTOR3( 0, 0, 0 );
                vOffset					= D3DXVECTOR3( 0, 0, 0 );
				vMainEffectScale		= D3DXVECTOR3( 0, 0, 0 );
				vPassiveParticleScale	= D3DXVECTOR3( 0, 0, 0 );
				bApplyRotateOrigin		= false;
				bApplyRotatedOffset		= false; // oasis907 : 김상윤 [2010.2.10] //
				bApplyMatrix			= false;
				bAutoDie				= false;
				bLandCrashOnly			= false;
				bLifeTimeOnly			= false;
				iGroupID				= -1;		/// 그룹 데미지 설정 추가
				bInheritBuffFactor		= false;
				//bForceDie		= false;
				//bForceDieHit	= false;
				//triggerCount	= -1;
				//triggerTime		= -1.0f;
				//pDamageEffect	= NULL;
				fLifeTime				= -1.f;
				bOnlyApplyRotateOffset	= false;
			}

			//{{ kimhc // 2009-10-20 // 독구름 작업 관련 추가
			DamageEffectData( const DamageEffectData& t ) { *this = t; }    // copy constructor
			DamageEffectData& operator=( const DamageEffectData& t )        // assign operator
			{

				posType					= t.posType;
				damageEffectName		= t.damageEffectName;
				boneName				= t.boneName;
				bApplyRotate			= t.bApplyRotate;
				fEventTime				= t.fEventTime;
				vRotateAngleDegree		= t.vRotateAngleDegree;
				vOffset					= t.vOffset;
				vMainEffectScale		= t.vMainEffectScale;
				vPassiveParticleScale	= t.vPassiveParticleScale;
				bApplyRotateOrigin		= t.bApplyRotateOrigin;
				bApplyRotatedOffset		= t.bApplyRotatedOffset; // oasis907 : 김상윤 [2010.2.10] //
				bApplyMatrix			= t.bApplyMatrix;
				bAutoDie				= t.bAutoDie;
				bLandCrashOnly			= t.bLandCrashOnly;
				bLifeTimeOnly			= t.bLifeTimeOnly;
				iGroupID				= t.iGroupID;			/// 그룹 데미지 설정 추가
				bInheritBuffFactor		= t.bInheritBuffFactor;

				fLifeTime				= t.fLifeTime;
				bOnlyApplyRotateOffset	= t.bOnlyApplyRotateOffset;

				return *this;
			}
			//}} kimhc // 2009-10-20 // 독구름 작업 관련 추가

			void* operator new(size_t s);
			void operator delete(void *p);

		};


		struct CameraData
		{
			UNIT_SELECT		unitSelect;
			CKTDGCamera::SHAKE_TYPE		crashType;
			float			fSpeed;
			float			fAccel;
			float			fGap;
			float			fTime;

			float			fEventTime;

		};

		struct ScreenData
		{
			UNIT_SELECT		unitSelect;
			float			fEventTime;
			int				clearCount;
			D3DXCOLOR		color;

			ScreenData()
			{
				unitSelect	= DEUS_ALL;
				fEventTime	= 0.0f;
				clearCount	= 1;
				color		= 0xffffffff;
			}
		};

		struct SoundData
		{
			float			fEventTime;
			wstring			fileName;
			int				iRate;
			bool			bUseGlobalTime;
			bool			bLoop;

			SoundData()
			{
				fEventTime	= 0.f;
				fileName	= L"";
				iRate		= 100;
				bUseGlobalTime = false;
				bLoop = false;
			}
			SoundData( const SoundData& t ) { *this = t; }    // copy constructor
			SoundData& operator=( const SoundData& t )        // assign operator
			{
				fEventTime = t.fEventTime;
				fileName = t.fileName;
				iRate = t.iRate;
				bUseGlobalTime = t.bUseGlobalTime;
				bLoop = t.bLoop;

				return *this;
			}
		};

		struct LockOnData
		{
			LOCK_ON_TYPE	m_LockOnType;
			D3DXVECTOR3		m_LockOnPos;
			bool			m_bLockOnUserUnit;
			UidType			m_LockOnUnitUID;
			int				m_LockOnNPCUID;
			float			m_fCurveSpeed;
			int				m_iRandomSeed;
//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
			float			m_fInDistanceToDieEffect; /// 이거리 내에 들어오면 이펙트 삭제(Die)
//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.18 // 청 1차 전직
			float			m_fTimeUnlockFromStart;		/// 생성 후 몇초 이후부터 Lock을 수행하기 위해 사용
		//}} kimhc // 2011.1.18 // 청 1차 전직
			float			m_fUnLockTime;
			float			m_fRange;
			bool			m_bIsNotTargetPreLockOnTarget; /// 이전 타겟팅 한 대상을 재 타겟 하지 않도록 하는 기능
			bool			m_bIsOnlyFront; /// 시전자 위치 기준으로 전방으로만 타겟팅 되도록 하는 기능
			bool			m_bSaveIsRight;	/// 시전한 타이밍에 시전자 방향 저장.
			int				m_iSaveLastTouchLineIndex; /// 시전한 타이밍에 시전자의 마지막 라인맵 인덱스 저장.
			D3DXVECTOR3		m_vecSaveOwnerPos; /// 시전한 타이밍에 시전자 위치 저장.
			bool			m_bIsOnlyTargetAttack; /// targetunit만 데미지를 입도록 하는 기능
			LOCK_ON_CHECK_SAVE_DIRSPEED	m_eNotFindTargetMoveStop;	/// 타겟을 찾지 못했으면 제자리에 멈춰있도록 하는 기능.
			float			m_fSaveDirSpeed;						/// Dir Speed를 저장해둘 변수.
			wstring			m_wstrTargetBoneName;					/// 특정 본을 타겟으로 잡음
			//}}
			LockOnData()
			{
				m_LockOnType		= LOT_NONE;
				m_LockOnPos			= D3DXVECTOR3(0,0,0);
				m_bLockOnUserUnit	= true;
				m_LockOnUnitUID		= -1;
				m_LockOnNPCUID		= -1;
				m_fCurveSpeed		= 0.0f;
				m_iRandomSeed		= -1;
				//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
				m_fInDistanceToDieEffect = -1.0f;	// 이거리 내에 들어오면 이펙트 삭제(Die)
				//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG

				//{{ kimhc // 2011.1.18 // 청 1차 전직
				m_fTimeUnlockFromStart = -1.0f;		// 생성 후 몇초 이후부터 Lock을 수행하기 위해 사용
				//}} kimhc // 2011.1.18 // 청 1차 전직
				m_fUnLockTime = 9999.f;
				m_fRange = -1.f;
				m_bIsNotTargetPreLockOnTarget = false; /// 이전 타겟팅 한 대상을 재 타겟 하지 않도록 하는 기능
				m_bIsOnlyFront = false;
				m_bSaveIsRight = false;
				m_iSaveLastTouchLineIndex = -1;
				m_vecSaveOwnerPos = D3DXVECTOR3(0,0,0);
				m_bIsOnlyTargetAttack = false;
				m_eNotFindTargetMoveStop = LOCSD_NONE;
				m_fSaveDirSpeed = 0.f;
				m_wstrTargetBoneName = L"";
			};
		};

		struct BlackHoleData
		{
			bool	m_bEnabled;
			float	m_fDraggingSpeed;
			float	m_fDraggingRange;
			bool	m_bDragOnlyHittedUnit;	// 이 데미지 이펙트에 의해 타격을 입은 unit만 빨려들게 하기
			bool	m_bCanPassUnit;

			BlackHoleData()
			{
				m_bEnabled = false;
				m_fDraggingSpeed = 1.f;
				m_fDraggingRange = 1.f;
				m_bDragOnlyHittedUnit = false;
				m_bCanPassUnit = true;
			}
		};

// 데미지 이펙트가 사인 곡선 운동을 하게 함 : 글리터 강령술사가 사용.
		struct SinMoveData
		{

			bool								m_bSinMove;
			float								m_fTimeMax;
			float								m_fTimeMin;
			float								m_fMoveMax;
			float								m_fMoveMin;
			float								m_fMove;
			float								m_fTime;
			float								m_fCurrentTime;

			SinMoveData()
			{
				m_bSinMove							= false;
				m_fTimeMax					= 0.0f;
				m_fTimeMin					= 0.0f;
				m_fMoveMax					= 0.0f;
				m_fMoveMin					= 0.0f;
				m_fMove						= 0.0f;
				m_fTime						= 0.0f;
				m_fCurrentTime				= 0.0f;

			};
		};

		//{{ kimhc // 2009-10-20 // DamageEffect.lua에 있는 데이터를 저장할 수 있는 구조체
		struct DamageEffectDataInLua
		{
			std::vector< DamageEffectData* >				m_vecDieDamageEffect;				// DIE_DAMAGE_EFFECT 테이블
			std::vector< CX2DamageEffect::ParticleData* >	m_vecStartParticle;
			std::vector< CX2DamageEffect::ParticleData* >	m_vecPassiveParticle;
			std::vector< CX2DamageEffect::ParticleData* >	m_vecTimeParticle;
			std::vector< CameraData >						m_vecCameraData;
			std::vector< ScreenData >						m_vecScreenData;
			std::vector< SoundData >						m_vecSoundData;
			std::vector< DamageEffectData* >				m_vecCreateDamageEffect;
			std::vector< CX2DamageEffect::ParticleData* >	m_vecHitParticle;
			std::vector< DamageEffectData* >				m_vecHitDamageEffect;
			CX2DamageManager::DamageData*					m_pDamageDataNext;

			DamageEffectDataInLua() : m_pDamageDataNext( NULL )
			{}
		};
		//}} kimhc // 2009-10-20 // DamageEffect.lua에 있는 데이터를 저장할 수 있는 구조체

		class CEffect
		{
			public:
				~CEffect();

				CEffect( CX2DamageEffect* pManager, KLuaManager* pLuaManager, CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
							D3DXVECTOR3 pos, D3DXVECTOR3 angleDegree, D3DXVECTOR3 moveAxisDegree, float fLandPos = 0.f, float fCreationDelayTime = -1.f,
							float fParabolicTimeToReachTarget = 0.f, float fParabolicTimeToLive = 0.f, const int iMeshPlayerIndex = 0,
                            CEffectHandle hHandle = INVALID_DAMAGE_EFFECT_HANDLE,
                            float fHitAddMp = -1.f
							, bool bPetSkillEffect = false
						);

				void* operator new(size_t s);
				void operator delete(void *p);

				virtual void OnFrameMove( double fTime, float fElapsedTime );
				void OnFrameMove_SoundData();
				void OnFrameMove_BlackHoleData( float fElapsedTime );
				void OnFrameMove_LockOnData( float fElapsedTime_ );
				void OnFrameMove_SineMoveData( float fElapsedTime );
				void OnFrameMove_Trace();

				void SetLandPositionOnLineMap( const D3DXVECTOR3& vLandPositionOnLineMap ) { m_vLandPositionOnLineMap = vLandPositionOnLineMap; }

				//virtual void OnFrameRender();

				void DamageReact( CX2DamageManager::DamageData* pDamageData );
				void AttackResult();

				bool			GetLive(){ return m_bLive; }
                // CX2DamageEffect 내에서만 사용됨! 외부에서 사용하지 말 것
                void            _SetDead()  { m_bLive = false; }
				CX2GameUnit*	GetOwnerUnit(){ return ( null != m_optrOwnerUnit ? m_optrOwnerUnit.GetObservable() : NULL ); }
				const WCHAR*	GetName(){ return m_Name.c_str(); }
				bool			GetAttackedByUnit(){ return m_bAttackedByUnit; }


				CKTDGXMeshPlayer::CXMeshInstanceHandle GetMainEffectHandle() { return m_hMeshInstHandle; }
				CKTDGXMeshPlayer::CXMeshInstance* GetMainEffect();


				LockOnData&		GetLockOnData(){ return m_LockOnData; }
				void			SetLockOnUnitUID( UidType unitUID ){ m_LockOnData.m_LockOnUnitUID = unitUID; }
				void			SetLockOnNPCUID( int NPCUID ){ m_LockOnData.m_LockOnNPCUID = NPCUID; }
				void			SetLockOnPos( D3DXVECTOR3 vPos ) { m_LockOnData.m_LockOnPos = vPos; }
				void			SetLockOnRandomSeed( int iSeed ) { m_LockOnData.m_iRandomSeed = iSeed; }
				void			SetLockOnType( LOCK_ON_TYPE eLockOnType ){ m_LockOnData.m_LockOnType = eLockOnType; }
				UidType			GetLockOnUnitUID () const { return m_LockOnData.m_LockOnUnitUID; }

				BlackHoleData*  GetBlackHoleData() { return &m_BlackHoleData; }

				D3DXVECTOR3		GetPos() { return m_Pos; }
				D3DXVECTOR3		GetBeforePos() { return m_BeforePos; }

				void			SetPos( const D3DXVECTOR3& pos );
				void			SetAngleDegree( D3DXVECTOR3 angleDegree ){ m_AngleDegree = angleDegree; }
				D3DXVECTOR3		GetMoveAxisDegree() const { return m_MoveAxisDegree; }
				void			SetMoveAxisDegree( D3DXVECTOR3 moveAxisDegree ){ m_MoveAxisDegree = moveAxisDegree; }
				void			SetUseDXMatrix( D3DXMATRIX& dxMATRIX )
				{
					m_bUseDXMatrix	= true;
					m_DXMATRIX		= dxMATRIX;
				}


				float GetPowerRate() const { return m_fPowerRate; }
				void  SetPowerRate(float fPowerRate) { m_fPowerRate = fPowerRate; }

				void SetAttackTime(D3DXVECTOR2 val) { m_AttackTime = val; }
				const D3DXVECTOR2 GetAttackTime() const { return m_AttackTime; }
				void SetLandPos(float val) { m_fLandPos = val; }

				bool IsCreationDelayElapsed()
				{
					if( m_timerCreationDelay.elapsed() > (float) m_fCreationDelayTime )
					{
						return true;
					}
					return false;
				}

				CX2DamageManager::DamageData& GetDamageData() { return m_DamageData; }
				CX2DamageManager::DamageData* GetDamageDataPtr() { return &m_DamageData; }
				void SetDamageTime( int iHitCount ) { m_DamageTime = iHitCount; }
				int GetDamageTime() { return m_DamageTime; }

				int GetDamageTimeNow() { return m_DamageTimeNow; }

				bool SetMainEffectScale( float fX, float fY, float fZ );
				bool SetMainEffectScale( D3DXVECTOR3 vScale );

				void SetPassiveParticleScale( float fX, float fY, float fZ );
				void SetPassiveParticleScale( D3DXVECTOR3 vScale );

				void SetHitDamageEffectScale( float fX, float fY, float fZ );
				void SetHitDamageEffectScale( D3DXVECTOR3 vScale );

				void SetDieDamageEffectScale( float fX, float fY, float fZ );
				void SetDieDamageEffectScale( D3DXVECTOR3 vScale );

				void SetScale( float fX, float fY, float fZ );
				void SetScale( D3DXVECTOR3 vScale );

				void SetPassiveparticleTriggerTimeRate( float fPassiveParticleTriggerTimeRate ){ m_fPassiveParticleTriggerTimeRate = fPassiveParticleTriggerTimeRate; }
				float GetPassiveparticleTriggerTimeRate(){ return m_fPassiveParticleTriggerTimeRate; }

				void SetReflected( bool bVal ){ m_bReflected = bVal; }





				void SetScaleByUnit( const D3DXVECTOR3& vScale );
				bool SetMainEffectScaleByUnit( const D3DXVECTOR3& vScale );
				//void SetPassiveParticleScaleByUnit( const D3DXVECTOR3& vScale ) { m_vPassiveParticleScaleByUnit = vScale; }
				void SetHitDamageEffectScaleByUnit( const D3DXVECTOR3& vScale ) { m_vHitDamageScaleByUnit = vScale; }
				void SetDieDamageEffectScaleByUnit( const D3DXVECTOR3& vScale ) { m_vDieDamageScaleByUnit = vScale; }

				D3DXVECTOR3 GetMainEffectScaleByUnit();




				void SetApplyScaleByUnit(bool bVal) { m_bApplyScaleByUnit = bVal; }
				bool GetApplyScaleByUnit() { return m_bApplyScaleByUnit; }

				bool GetFollowLineRight() { return m_bFollowLineRight; }
				void SetFollowLineRight( bool bVal ){ m_bFollowLineRight = bVal; }
				void SetInheritForceDownRate( bool bVal ) { m_bInheritForceDownRate = bVal; }
				void SetForceDownRate( float fVal ){ m_fForceDownRate = fVal; }

				int		GetLaserGroupID(){ return m_iLaserGroupID; }
				void	SetLaserGroupID( int _iLaserGroupID );
				bool	GetUseCannonBall() { return m_bUseCannonBall; }
				DamageEffectDataInLua& GetDamageEffectDataInLua() { return m_DamageEffectDataInLua; }

				int		GetReflectCount() { return m_iReflectCount; }
				void	SetReflectCount( IN int iReflectCount_ ) { m_iReflectCount = iReflectCount_;  }

                CEffectHandle   GetHandle() const { return m_hHandle; }

				/// 데미지 이펙트 인덱스 제어
				int		GetIndex() const { return m_iIndex; }
				void	SetIndex( int iIndex ) { m_iIndex = iIndex; }
				/// 데미지 이펙트 타입 제어
				int		GetType() const { return m_iType; }
				void	SetType( int iType ) { m_iType = iType; }
				/// 폭발의 화살 적용 순서 제어( 지연의 신호탄 )
				int		GetOrderNum() const { return m_iOrderNum; }
				void	SetOrderNum( int iOrderNum ) { m_iOrderNum = iOrderNum; }

				/// 물리, 마법 추가 데미지 제어
				float	GetAddDieEffectDamage() const { return m_fAddDieEffectDamage; }
				void	SetAddDieEffectDamage( float fAddDieEffectDamage ) { m_fAddDieEffectDamage = fAddDieEffectDamage; }

				void	AddDieEffectDamage( CEffect* pCEffect );	/// 다이 데미지 이펙트 생성시 추가 데미지 설정 함수( 지연의 신호탄 )

				void		SetCheckSpectro( IN bool bValue_ ) { m_bCheckSpectro = bValue_; }
				bool	GetCheckSpectro() const { return m_bCheckSpectro; }

				bool GetIsGroupDamage() const { return m_bGroupDamage; }	/// 데미지 이펙트 그룹 데미지 설정 반환 함수

				bool GetOnlyRelationShip() const { return m_bOnlyRelationShip; }

				// 청 택트 궁극기 추가로 메인 이펙트의 회전값 받아오거나 설정하는 부분 추가, 김종훈
				D3DXVECTOR3 GetMainEffectAxisAngleDegree() const		  { return m_vMainEffectAxisAngleDegree; }
				void		SetMainEffectAxisAngleDegree(D3DXVECTOR3 val) { m_vMainEffectAxisAngleDegree = val; }

				void SetLockOnNearstTarget( IN const D3DXVECTOR3& vMyPos_, int iTeam_ , int randomOffset = 0 );

				bool SetLinkMainEffectByPos( IN const D3DXVECTOR3& vMyPos_, IN const D3DXVECTOR3& vTargetPos_,
												   IN const float fDistance_ = 99999.f );

				/// SetLinkMainEffectByPos() 를 쓰기 위해 필요한 길이 배율 반환
				float GetLinkEffectRate() { return m_fLinkEffectRate; }

			protected:
				void SetStartParticle();
				void SetPassiveParticle();
				void SetTimeParticle();
				void SetSuckMagicParticle(const WCHAR* particleName);
				void SetHitParticle();
				void SetHitDamageEffect();
				public:
				void SetDieDamageEffect();
				void DestroyPassiveParticle();
				protected:
				void CreateHitDamageEffect( vector<DamageEffectData*>& vecHitDamageEffect );
				void CreateDieDamageEffect( vector<DamageEffectData*>& vecDieDamageEffect );

				void SetCameraData();
				void SetScreenData();
				void SetSoundData();
				void SetDamageEffect();

				void FollowLine();
				void LoadLockOnData();
				void LoadBlackHoleData();

				//{{ kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(독구름)
				void LoadDieDamageEffect();
				void LoadStartParticle();
				void LoadPassiveParticle();
				void LoadTimeParticle();
				void LoadCameraData();
				void LoadScreenData();
				void LoadSoundData();
				void LoadDamageEffect();
				void LoadHitParticle();
				void LoadHitDamageEffect();

				void SetDamageDataNext( CX2DamageManager::DamageData* pDamageData );

				ParticleData*		LoadParticleTable();
				void SetParticleTable( ParticleData* pData, bool bCreate = true, float fTriggerTimeRate = -1.f );

				DamageEffectDataInLua m_DamageEffectDataInLua;	// 임시(위치 옮겨야함)
				//}} kimhc // 2009-10-20 // DamageEffect.lua에 있는 테이블 로드(독구름)

				void LoadSinMoveData();			// **

				void VectorLockOn( const D3DXVECTOR3& targetPos );


				ParticleData*		LoadParticleSuckMagic( const WCHAR* particleName );

				//{{ kimhc // 2009-10-20 // 기존의 LoadParticleTable을 Load와 Set 으로 분류(일명 독구름 작업)
				//}} kimhc // 2009-10-20 // 기존의 LoadParticleTable을 Load와 Set 으로 분류(일명 독구름 작업)

				void SetParticleData( ParticleData* pData );

				void LoadAfterImage();

                CEffectHandle                     m_hHandle;

				float								m_fElapsedTime;
				bool								m_bFirstFrame;
				bool								m_bLive;
				CX2DamageEffect*					m_pManager;
				KLuaManager*						m_pLuaManager;
				D3DXVECTOR3							m_Pos;
				D3DXVECTOR3							m_BeforePos;
				D3DXVECTOR3							m_AngleDegree;
				D3DXVECTOR3							m_MoveAxisDegree;
				float								m_fLandPos;
				bool								m_bFollowLine;
				bool								m_bFollowLineRight;
				bool								m_bFollowLineNew;		// 경사진 라인맵 따라갈 수 있게
				bool								m_bMustOnLine;

				//{{ kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록
				bool								m_bRefreshLandPos;
				//}} kimhc // 2010.2.10 //	매 프레임 LandPosition 갱신 안되도록

				bool								m_bUseDXMatrix;
				D3DXMATRIX							m_DXMATRIX;

				EFFECT_TYPE							m_EffectType;
				wstring								m_Name;
				float								m_fPowerRate;
				CX2GameUnitoPtr						m_optrOwnerUnit;




				CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hMeshInstHandle;

				D3DXVECTOR3							m_vMainEffectAxisAngleDegree;
				D3DXVECTOR3							m_vMainEffectPosition;

				CX2DamageManager::DamageData		m_DamageData;

				D3DXVECTOR2							m_AttackTime;
				bool								m_bGlobalTime;

				int 								m_DamageTime;
				int									m_DamageTimeNow;
				int									m_PrevDamageTime;
				bool								m_bAttackedByUnit;
				bool								m_bAttackedByEffect;
				bool								m_bReflect;

				vector<ParticleData*>				m_StartParticleDataList;
				vector<ParticleData*>				m_PassiveParticleDataList;
				vector<ParticleData*>				m_TimeParticleDataList;

				vector<CameraData>					m_CameraDataList;
				vector<ScreenData>					m_ScreenDataList;
				vector<SoundData>					m_SoundDataList;

				//std::vector< CKTDXDeviceSound* >	m_vecPlayingLoopSound;			// play중인 loop sound를 이펙트가 사라질 때 꺼주기 위해서

				vector<DamageEffectData*>			m_DamageEffectDataList;



				CX2SoundCloseManager		m_SoundCloseManager;


				float								m_fDamageDataChangeTime;
				float								m_fDamageDataChangeTimeGlobalTime;
				bool								m_bClearHitUnitOnChangeDamageData;

				LockOnData							m_LockOnData;

				SinMoveData							m_SinMoveData;

				BlackHoleData						m_BlackHoleData;

				vector<DamageEffectData*>			m_vecHitDamageEffect;
				vector<DamageEffectData*>			m_vecDieDamageEffect;

				float								m_fCreationDelayTime;
				CKTDXTimer							m_timerCreationDelay;

				D3DXVECTOR3							m_vPassiveParticleScale;
				D3DXVECTOR3							m_vHitDamageScale;
				D3DXVECTOR3							m_vDieDamageScale;

				float								m_fPassiveParticleTriggerTimeRate;	// CreateInstance() 에서 인자로 받은 lifeTime 을 적용해주기 위해
				bool								m_bReflected;


				D3DXVECTOR3							m_vHitDamageScaleByUnit;
				D3DXVECTOR3							m_vDieDamageScaleByUnit;


				float								m_fReactAnimSpeed;
				bool								m_bFollowOnLine;



				//{{ kimhc // 2010.2.3 //	이브 2차 전직
				D3DXVECTOR3							m_vMoveAngle;	// 움직이는 각도 Rotate에 plus 함
				//}} kimhc // 2010.2.3 //	이브 2차 전직

				bool								m_bApplyScaleByUnit;
				bool								m_bInheritForceDownRate;
				float								m_fForceDownRate;

				D3DXVECTOR3							m_vLandPositionOnLineMap;

				int									m_iLaserGroupID;

				bool								m_bUseCannonBall;

				int		m_iReflectCount;

				CKTDGXSkinAfterImage*	m_pAfterImage;		/// 잔상
				D3DXVECTOR2				m_afterImageTime;	/// 잔상 유지 시간
				float					m_fDurationTime;	/// 잔상 유지 시간 확인 타이머

				//{{ mauntain : 김태환 [2012.05.21] 레나 2차 전직 나이트 와쳐
				int						m_iIndex;			/// 해당 DamageEffect의 Index
				int						m_iType;			/// 해당 DamageEffect의 Type
				int						m_iOrderNum;		/// 폭발의 화살 발사 순서
				float					m_fAddDieEffectDamage;		/// 다이 데미지 이펙트 생성시 fPhysic, fMagic에 추가되는 수치
				//}}

				bool	m_bCheckSpectro;	/// 반응장에 생성된 것인지 확인

				bool	m_bGroupDamage;		/// 데미지 이펙트 그룹 데미지 설정


				bool	m_bOnlyRelationShip;

				bool	m_bShouldNotifyToOwnerUnitWhenDie;

				float	m_fLinkEffectRate;	/// SetLinkMainEffectByPos() 를 쓰기 위해 필요한 길이 배율

		}; // CEffect



        enum    EListType
        {
            LIST_LIVE = 0,
            LIST_RESERVE = 1,
            LIST_FREE = 2,
            LIST_NUM
        };
        struct  KInstanceHandleInfo
        {
            CEffect*         m_pInstance;
            WORD             m_wStamp;
            EListType        m_eListType;
            KInstanceHandleInfo()
                : m_pInstance( NULL )
                , m_wStamp(0)
                , m_eListType(LIST_FREE)
            {
            }
        };
        typedef kog::indexed_list<KInstanceHandleInfo>          KInstanceHandleList;
        typedef KInstanceHandleList::iterator                   KInstanceHandleIterator;


	public:
		CX2DamageEffect(void);
		~CX2DamageEffect(void);

		void OpenScriptFile( const WCHAR* pFilename );
		HRESULT OnFrameMove( double fTime, float fElapsedTime );

		CEffect* CreateInstance_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos );
		CEffect* CreateInstance_LUA2( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot );
		CEffect*  CreateInstanceByPet_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot );

		CEffect* CreateInstance_LUA3( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 pos, float fLandPos, D3DXVECTOR3 vRot, float fReserveTime );

		CEffect* CreateInstanceParabolic_LUA( CX2GameUnit* pGameUnit, const char* pName, D3DXVECTOR3 vPos, D3DXVECTOR3 vTargetPos, D3DXVECTOR3 vAcceleration,
			float fTimeToReachTarget, float fTimeToLive );

		CEffect* CreateInstanceParabolic( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
			const D3DXVECTOR3& vPos, const D3DXVECTOR3& vTargetPos, const D3DXVECTOR3& vAcceleration,
			const float fTimeToReachTarget, const float fTimeToLive, const int iMeshPlayerIndex = 0 );


		CEffect* CreateInstance( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
									D3DXVECTOR3 pos,
									D3DXVECTOR3 angleDegree,
									D3DXVECTOR3 moveAxisDegree,
									float fLandPos = 0, bool bReserve = false,
                                    float fReserveTime = -1.f, float fForceDownValueRate = 1.f, float fLifeTimeRate = 1.f,
									int iHitCount = -1, float fHitGap = -1.f, const int iMeshPlayerIndex = 0, float fHitAddMp = -1.f
                                    , bool bInsertFront = false
                                    , bool bPetSkillEffect = false
                                    )
		{
			return CreateInstance( pGameUnit, pName, fPowerRate, pos.x, pos.y, pos.z,
									angleDegree.x, angleDegree.y, angleDegree.z,
									moveAxisDegree.x, moveAxisDegree.y, moveAxisDegree.z, fLandPos, bReserve, fReserveTime, fForceDownValueRate, fLifeTimeRate,
									iHitCount, fHitGap, iMeshPlayerIndex, fHitAddMp
                                    , bInsertFront
                                    , bPetSkillEffect
                                    );
		}

		CEffect* CreateInstance( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
									float x, float y, float z,
									float angleXDegree, float angleYDegree, float angleZDegree,
									float moveAxisXDegree, float moveAxisYDegree, float moveAxisZDegree,
									float fLandPos = 0, bool bReserve = false,
                                    float fReserveTime = -1.f, float fForceDownValueRate = 1.f, float fLifeTimeRate = 1.f,
									int iHitCount = -1, float fHitGap = -1.f, const int iMeshPlayerIndex = 0, float fHitAddMp = -1.f
                                    , bool bInsertFront = false
                                    , bool bPetSkillEffect = false
                                    );


		CEffectHandle   CreateInstanceHandle( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
									D3DXVECTOR3 pos,
									D3DXVECTOR3 angleDegree,
									D3DXVECTOR3 moveAxisDegree,
									float fLandPos = 0, bool bReserve = false,
                                    float fReserveTime = -1.f, float fForceDownValueRate = 1.f, float fLifeTimeRate = 1.f,
									int iHitCount = -1, float fHitGap = -1.f, const int iMeshPlayerIndex = 0, float fHitAddMp = -1.f
                                    , bool bInsertFront = false
                                    , bool bPetSkillEffect = false
                                    )
		{
			CEffect* pEffect = CreateInstance( pGameUnit, pName, fPowerRate, pos, angleDegree, moveAxisDegree,
									fLandPos, bReserve, fReserveTime, fForceDownValueRate, fLifeTimeRate,
									iHitCount, fHitGap, iMeshPlayerIndex, fHitAddMp
                                    , bInsertFront
                                    , bPetSkillEffect
                                    );
            return  ( pEffect != NULL ) ? pEffect->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
		}

		CEffectHandle   CreateInstanceHandle( CX2GameUnit* pGameUnit, const WCHAR* pName, float fPowerRate,
									float x, float y, float z,
									float angleXDegree, float angleYDegree, float angleZDegree,
									float moveAxisXDegree, float moveAxisYDegree, float moveAxisZDegree,
									float fLandPos = 0, bool bReserve = false,
                                    float fReserveTime = -1.f, float fForceDownValueRate = 1.f, float fLifeTimeRate = 1.f,
									int iHitCount = -1, float fHitGap = -1.f, const int iMeshPlayerIndex = 0, float fHitAddMp = -1.f
                                    , bool bInsertFront = false
                                    , bool bPetSkillEffect = false
                                    )
        {
			CEffect* pEffect = CreateInstance( pGameUnit, pName, fPowerRate, x, y, z,
									angleXDegree, angleYDegree, angleZDegree,
									moveAxisXDegree, moveAxisYDegree, moveAxisZDegree, fLandPos, bReserve, fReserveTime, fForceDownValueRate, fLifeTimeRate,
									iHitCount, fHitGap, iMeshPlayerIndex, fHitAddMp
                                    , bInsertFront
                                    , bPetSkillEffect
                                    );
            return  ( pEffect != NULL ) ? pEffect->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
        }




		void PostProcessCreateInstance( CEffect* pCEffect, float fForceDownValueRate, float fLifeTimeRate, int iAddHitCount, float fHitGap );
		void DestroyAllInstances();
//{{ robobeg : 2013-08-30
		//void DestroyInstance( CEffect* pInstance );
        void DestroyInstanceHandle( CEffectHandle& hInstance, bool bSilently = false );
        void DestroyInstance( CEffect*& pInstance, bool bSilently = false );
        void DestroyInstance_LUA( CEffect* pInstance );
//}} robobeg : 2013-08-30

		void DestroyInstanceSilently( CEffect*& pInstance );
        void DestroyInstanceHandleSilently( CEffectHandle& hInstance );
		void AddLaserGroupHitUnit( int _iLaserGroupID, CX2DamageManager::HitUnit _hitUnit );


		bool IsLiveInstance( CEffect* pInstance );
        bool IsLiveInstanceHandle( CEffectHandle handle );
        CEffect*  GetInstance( CEffectHandle handle, bool bLiveOnly = true );
        CEffect*  ValidateInstanceHandle( CEffectHandle& handle, bool bLiveOnly = true )
        {
            CEffect* pEffect = GetInstance( handle, bLiveOnly );
            if ( pEffect == NULL )
                handle = INVALID_DAMAGE_EFFECT_HANDLE;
            return  pEffect;
        }

		void UnitDeleteProcess( CX2GameUnit* pGameUnit );





        template< typename FUNCTION >
        void        ApplyFunctionToLiveInstances( FUNCTION fn );

		CEffect*				        GetInstanceByIndex( int iIndex );				/// 해당 인덱스를 지닌 데미지 이펙트 반환 함수

//#ifdef	X2OPTIMIZE_REFACTORING_RESOURCE_BACKGROUND_LOAD
		//bool	AppendToDeviceList( CKTDXDeviceDataList& listInOut_, const WCHAR* pTempletName, CKTDXDeviceManager::EPriority ePriority = CKTDXDeviceManager::PRIORITY_HIGH );
		//bool	AppendToDeviceList( CKTDXDeviceDataList& listInOut_, const std::wstring& wstrTempletName, CKTDXDeviceManager::EPriority ePriority = CKTDXDeviceManager::PRIORITY_HIGH );
//#endif	X2OPTIMIZE_REFACTORING_RESOURCE_BACKGROUND_LOAD


    DWORD   ComposeHandle( WORD wIndex, WORD wStamp )
    {
        return wIndex | ( wStamp << 16L );
    }
    void    DecomposeHandle( DWORD dwHandle, OUT WORD& wIndex, OUT WORD& wStamp )
    {
        wIndex = (WORD) dwHandle;
        wStamp = (WORD) ( ( ( dwHandle ) & 0xffff0000 ) >> 16L );
    }

    bool                    IsInCriticalLoop() const { return m_bInCriticalLoop; }

	private:
		float				m_fElapsedTime;
		KLuaManager			m_LuaManager;


        KInstanceHandleList             m_coInstanceHandleList;
        bool                            m_bInCriticalLoop;


		//int					m_iCurrUniqueIndex;
};



template< typename FUNCTION >
void        CX2DamageEffect::ApplyFunctionToLiveInstances( FUNCTION fn )
{
    bool bOldInCriticalLoop = m_bInCriticalLoop;
    m_bInCriticalLoop = true;

    KInstanceHandleIterator iterEnd = m_coInstanceHandleList.end( LIST_LIVE );
    for( KInstanceHandleIterator iter = m_coInstanceHandleList.begin( LIST_LIVE );
        iter != iterEnd;
        ++iter )
    {
        KInstanceHandleInfo& info = *iter;
        CEffect* pEffect = info.m_pInstance;
		if( NULL == pEffect
            || pEffect->GetLive() == false
            )
		   continue;
        fn( *pEffect );
    }

    m_bInCriticalLoop = bOldInCriticalLoop;
}
