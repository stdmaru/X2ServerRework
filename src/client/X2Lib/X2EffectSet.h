#pragma once

#include    "indexed_list.h"




class CX2GameUnit;
class CX2SquareUnit;
typedef x2::intrusive_ptr<CX2SquareUnit>    CX2SquareUnitPtr;

class CX2PET;

class CX2RidingPet;
typedef x2::intrusive_ptr<CX2RidingPet> CX2RidingPetPtr;


struct  CX2EffectSet_EffectSetInstance_TAG {};
typedef KHandleType<int,CX2EffectSet_EffectSetInstance_TAG>     CX2EffectSet_EffectSetInstanceHandle;
#define INVALID_EFFECTSET_HANDLE    (CX2EffectSet_EffectSetInstanceHandle::invalid_handle())



class CX2EffectSet
{
public:


    typedef CX2EffectSet_EffectSetInstanceHandle    Handle;



	enum EFFECT_SET_MODE
	{
		ESM_INVALID,
		ESM_GAME,			// 게임내에서
		ESM_FIELD,			// 마을에서
		ESM_COMMON,			// 게임 & 마을
	};


	enum EFFECT_TYPE
	{
		ET_INVALID,
		ET_DAMAGE_EFFECT,
		ET_MESH_PLAYER_UI_MAJOR,
		ET_MESH_PLAYER_UI_MINOR,
		ET_MESH_PLAYER_GAME_MAJOR,
		ET_MESH_PLAYER_GAME_MINOR,
		ET_PARTICLE_UI_MAJOR,
		ET_PARTICLE_UI_MINOR,
		ET_PARTICLE_GAME_MAJOR,
		ET_PARTICLE_GAME_MINOR,
	};

	enum EFFECT_SET_TIMER_TYPE
	{
		ESTT_ANIM,
		ESTT_CUSTOM,
	};



	struct CameraShakeData
	{
		CKTDGCamera::SHAKE_TYPE m_eShakeType;
		float			m_fStartAnimTime;
		int				m_iLimitDistanceSq;			/// 플레이어와 이 거리 내에 있어야 카메라 크래시 발생 (제곱값)
		float			m_fOneDirSpeed;
		float			m_fOneDirAccel;
		float			m_fTwoDirGap;
		float			m_fTwoDirTime;
		float			m_fTwoDirTimeGap;
		bool			m_bFixedLookVec;
		int				m_iEyeShakeCount;
		CMinMax<float>	m_EyeMoveSpeed;
		CMinMax<float>	m_EyeMoveRange;
		int				m_iLookAtShakeCount;
		CMinMax<float>	m_LookAtMoveSpeed;
		CMinMax<float>	m_LookAtMoveRange;

#if X2_DEV_TOOLS
// #ifdef EFFECT_TOOL
		static bool IsSamef_( float a, float b = 0.f )
		{
			if( fabs( a - b ) > 0.0009 )
				return false;
			return true;
		}
		bool operator== ( const CameraShakeData& rhs_ ) const
		{
			if( m_eShakeType				!= rhs_.m_eShakeType			) return false;
			if( m_iLimitDistanceSq			!= rhs_.m_iLimitDistanceSq		) return false;

			if(false == IsSamef_(m_fStartAnimTime	,rhs_.m_fStartAnimTime	)) return false;
			if(false == IsSamef_(m_fOneDirSpeed		,rhs_.m_fOneDirSpeed	)) return false;
			if(false == IsSamef_(m_fOneDirAccel		,rhs_.m_fOneDirAccel	)) return false;
			if(false == IsSamef_(m_fTwoDirGap		,rhs_.m_fTwoDirGap		)) return false;
			if(false == IsSamef_(m_fTwoDirTime		,rhs_.m_fTwoDirTime		)) return false;
			if(false == IsSamef_(m_fTwoDirTimeGap	,rhs_.m_fTwoDirTimeGap	)) return false;

			if( m_bFixedLookVec				!= rhs_.m_bFixedLookVec			) return false;
			if( m_iEyeShakeCount			!= rhs_.m_iEyeShakeCount		) return false;
			if( m_iLookAtShakeCount			!= rhs_.m_iLookAtShakeCount		) return false;
			if( m_EyeMoveSpeed.m_Max		!= rhs_.m_EyeMoveSpeed.m_Max	) return false;
			if( m_EyeMoveSpeed.m_Min		!= rhs_.m_EyeMoveSpeed.m_Min	) return false;
			if( m_EyeMoveRange.m_Max		!= rhs_.m_EyeMoveRange.m_Max	) return false;
			if( m_EyeMoveRange.m_Min		!= rhs_.m_EyeMoveRange.m_Min	) return false;
			if( m_LookAtMoveSpeed.m_Max		!= rhs_.m_LookAtMoveSpeed.m_Max	) return false;
			if( m_LookAtMoveSpeed.m_Min		!= rhs_.m_LookAtMoveSpeed.m_Min	) return false;
			if( m_LookAtMoveRange.m_Max		!= rhs_.m_LookAtMoveRange.m_Max	) return false;
			if( m_LookAtMoveRange.m_Min		!= rhs_.m_LookAtMoveRange.m_Min	) return false;

			return true;
		}
// #endif // EFFECT_TOOL
#endif //defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		CameraShakeData() :
		m_eShakeType( CKTDGCamera::DECT_INVALID ),
		m_fStartAnimTime(0.f),
		m_iLimitDistanceSq( LIMIT_DISTANCE_SQ ),
		m_fOneDirSpeed(0.f),
		m_fOneDirAccel(0.f),
		m_fTwoDirGap(0.f),
		m_fTwoDirTime(0.f),
		m_fTwoDirTimeGap(0.f),
		m_bFixedLookVec(false),
		m_iEyeShakeCount(0),
		m_EyeMoveSpeed(0.f, 0.f),
		m_EyeMoveRange(0.f, 0.f),
		m_iLookAtShakeCount(0),
		m_LookAtMoveSpeed(0.f, 0.f),
		m_LookAtMoveRange(0.f, 0.f)
		{
		}
	};



	struct EffectData
	{
		EFFECT_TYPE		m_eEffectType;
		std::wstring	m_EffectName;
		std::wstring	m_EffectNameHyper;
		float			m_fStartAnimTime;
		std::wstring	m_PositionBoneName;
		// hoons.09-09-25.무기 본에 이펙트를 붙일경우..
		std::wstring	m_PositionWeaponBoneName;
		int				m_iWeaponIndex;
		int				m_iModelIndex;
		// hoons.09-09-25.
		std::wstring	m_PosRidingPetBoneName;
		D3DXVECTOR3		m_vPositionOffset;
		bool			m_bTrace;
		bool			m_bTraceMore;				// 파티클이 trace될 때 bone의 움직임 보다 느리게 따라오는 문제가 있을 때 사용

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		bool			m_bTraceMatrix;
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

		bool			m_bReverseY;

		bool			m_bLandPosition;			// m_PositionBoneName이 null string 인 경우에 unit의 위치를 사용할 것인지 unit의 land position을 사용할 지

		D3DXVECTOR3		m_vRotateRel;
		bool			m_bApplyUnitRotation;		// 캐릭터가 라인맵에 따라 회전한 각도를 이펙트의 각도에 적용시킬지

		bool			m_bApplyUnitRotationMirror;		// 캐릭터가 라인맵에 따라 회전한 각도를 이펙트의 각도에 적용시키면서 x축에 대해서는 방향에 따라 반전 설정

		bool			m_bApplyInstanceScale;		// EffectSetInstance에 지정된 scale을 적용할지 말지
		D3DXVECTOR3		m_vScale;
		D3DXVECTOR3		m_vScaleOnLeft;


		bool			m_bStartWhenTraceTargetDead;	// m_pTraceTargetGameUnit이 죽거나 없어졌을 때 반드시 생성되야 하는 이펙트
		bool			m_bPassiveEffect;				// effectsetinstacne의 life time에 영향을 주지 않는 이펙트 이다
		bool			m_bUseSubAttackListSet;			// 게임유닛의 SubAttackListSet을 사용할 것인지 여부(MeshPlayer만 사용가능)

		bool			m_bSetOverUI;					// UI 위로 표시되는 기능 추가 ( Particle만 사용가능 )

		bool			m_bBoneTraceTargetMesh;

		bool			m_bTraceUserTargetForce;		// BoneTraceTargetMesh가 적용중 이라도, 해당 설정이 되어 있으면 유닛을 Trace 한다.

		// 이펙트셋보다 지정된 거리 이상 멀어지면 생성하지 않도록 하기 위한 거리 값 저장 변수
		float			m_fLimitDistanceSq;

#if X2_DEV_TOOLS
// #ifdef EFFECT_TOOL
		static bool IsSamef_( float a, float b = 0.f )
		{
			if( fabs( a - b ) > 0.0009 )
				return false;
			return true;
		}
		bool operator==( const EffectData& rhs_ ) const
		{
			if( m_eEffectType					!= rhs_.m_eEffectType				) return false;
			if( m_EffectName					!= rhs_.m_EffectName				) return false;
			if( m_EffectNameHyper				!= rhs_.m_EffectNameHyper		    ) return false;

			if( false == IsSamef_( m_fStartAnimTime, rhs_.m_fStartAnimTime)			) return false;
			if( m_PositionBoneName				!= rhs_.m_PositionBoneName		    ) return false;
			if( m_PositionWeaponBoneName		!= rhs_.m_PositionWeaponBoneName	) return false;
			if( m_PosRidingPetBoneName			!= rhs_.m_PosRidingPetBoneName		) return false;
			if( m_iWeaponIndex					!= rhs_.m_iWeaponIndex			    ) return false;
			if( m_iModelIndex					!= rhs_.m_iModelIndex			    ) return false;
			if( m_vPositionOffset				!= rhs_.m_vPositionOffset		    ) return false;
			if( m_bTrace						!= rhs_.m_bTrace					) return false;
			if( m_bTraceMore					!= rhs_.m_bTraceMore				) return false;
			if( m_bTraceMatrix					!= rhs_.m_bTraceMatrix			    ) return false;
			if( m_bReverseY						!= rhs_.m_bReverseY				    ) return false;
			if( m_bLandPosition					!= rhs_.m_bLandPosition			    ) return false;
			if( m_vRotateRel					!= rhs_.m_vRotateRel				) return false;
			if( m_bApplyUnitRotation			!= rhs_.m_bApplyUnitRotation		) return false;
			if( m_bApplyInstanceScale			!= rhs_.m_bApplyInstanceScale	    ) return false;
			if( m_vScale						!= rhs_.m_vScale					) return false;
			if( m_vScaleOnLeft					!= rhs_.m_vScaleOnLeft			    ) return false;
			if( m_bStartWhenTraceTargetDead		!= rhs_.m_bStartWhenTraceTargetDead ) return false;
			if( m_bPassiveEffect				!= rhs_.m_bPassiveEffect			) return false;
			if( m_bUseSubAttackListSet			!= rhs_.m_bUseSubAttackListSet	    ) return false;
			if( m_bBoneTraceTargetMesh			!= rhs_.m_bBoneTraceTargetMesh		) return false;

			if( m_bTraceUserTargetForce			!= rhs_.m_bTraceUserTargetForce		) return false;

			if( m_fLimitDistanceSq				!= rhs_.m_fLimitDistanceSq			) return false;

			return true;
		}

// #endif // EFFECT_TOOL
#endif //defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		EffectData()
		: m_eEffectType( ET_INVALID )
		, m_EffectName( L"" )
		, m_EffectNameHyper( L"" )
		, m_fStartAnimTime( 0.f )
		, m_PositionBoneName( L"" )
		, m_PositionWeaponBoneName( L"" )
		, m_iWeaponIndex(0)
		, m_iModelIndex(0)
		, m_PosRidingPetBoneName( L"" )
		, m_vPositionOffset( 0, 0, 0)
		, m_bTrace( false )
		, m_bTraceMore( false )
		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		, m_bTraceMatrix( false )
		, m_bReverseY( false )
		//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		, m_bLandPosition( false )
		, m_vRotateRel( 0, 0, 0 )
		, m_bApplyUnitRotation( true )
		, m_bApplyUnitRotationMirror( false )
		, m_bApplyInstanceScale( true )
		, m_vScale( 0, 0, 0)
		, m_vScaleOnLeft( 0, 0, 0)
		, m_bStartWhenTraceTargetDead( false )
		, m_bPassiveEffect( false )
		, m_bUseSubAttackListSet( false )
		, m_bSetOverUI( false )
		, m_bBoneTraceTargetMesh( false )
		, m_bTraceUserTargetForce( false )
		, m_fLimitDistanceSq( -1.f )
		{
		}
	};

	struct EffectSetData
	{
		std::wstring				m_wstrEffectSetName;
		std::vector<EffectData*>	m_vecpEffectData;
		std::vector<CameraShakeData*> m_vecpCameraShakeData;
		EFFECT_SET_TIMER_TYPE		m_eEventTimerType;
		bool						m_bAffectedByUnitScale;

		//bool			m_bDeleteWhenTraceTargetDead;			// m_pTraceTargetGameUnit이 죽거나 없어지면 이펙트도 사라진다
		int				m_iDeleteShakeCount;					// 0보다 클 때 유효하고 이 회수만큼 흔들면 이펙트도 사라진다
#if X2_DEV_TOOLS
// #ifdef EFFECT_TOOL
		bool operator==( const EffectSetData& rhs_) const
		{
			if( m_wstrEffectSetName != rhs_.m_wstrEffectSetName ) return false;
			if( m_eEventTimerType != rhs_.m_eEventTimerType ) return false;
			if( m_bAffectedByUnitScale != rhs_.m_bAffectedByUnitScale ) return false;
			if( m_iDeleteShakeCount != rhs_.m_iDeleteShakeCount ) return false;

			if( m_vecpEffectData.size() != rhs_.m_vecpEffectData.size() ) return false;

			int i = 0;
			for( EffectData* pData : m_vecpEffectData )
			{
				if( (*pData) == (*rhs_.m_vecpEffectData[i]) )
					++i;
				else
					return false;
			}

			if( m_vecpCameraShakeData.size() != rhs_.m_vecpCameraShakeData.size() ) return false;
			i = 0;
			for( CameraShakeData* pData : m_vecpCameraShakeData )
			{
				if( (*pData) == (*rhs_.m_vecpCameraShakeData[i]) )
					++i;
				else
					return false;
			}

			return true;
		}
		EffectSetData& operator=( const EffectSetData& rhs )
		{
			m_wstrEffectSetName = rhs.m_wstrEffectSetName;
			m_eEventTimerType = rhs.m_eEventTimerType;
			m_bAffectedByUnitScale = rhs.m_bAffectedByUnitScale;
			m_iDeleteShakeCount = rhs.m_iDeleteShakeCount;

			m_vecpEffectData.clear();
			for( CX2EffectSet::EffectData* pData : rhs.m_vecpEffectData )
			{
				CX2EffectSet::EffectData* pTempData = new CX2EffectSet::EffectData;
				*pTempData = *pData;
				m_vecpEffectData.push_back( pTempData );
			}

			m_vecpCameraShakeData.clear();
			for( CX2EffectSet::CameraShakeData* pData : rhs.m_vecpCameraShakeData )
			{
				CX2EffectSet::CameraShakeData* pTempData = new CX2EffectSet::CameraShakeData;
				*pTempData = *pData;
				m_vecpCameraShakeData.push_back( pTempData );
			}

			return *this;
		}
// #endif // EFFECT_TOOL
#endif // defined(EFFECT_TOOL) || defined(EXPAND_DEVELOPER_SCRIPT)

		EffectSetData()
		: m_wstrEffectSetName( L"" )
		, m_eEventTimerType( ESTT_ANIM )
		, m_bAffectedByUnitScale( true )
		//, m_bDeleteWhenTraceTargetDead( false )
		, m_iDeleteShakeCount( -1 )
		{
		}

		~EffectSetData()
		{
			for( EffectData* pEffectData : m_vecpEffectData )
			{
				SAFE_DELETE( pEffectData );
			}
			m_vecpEffectData.clear();


			for( CameraShakeData* pCameraShakeData : m_vecpCameraShakeData )
			{
				SAFE_DELETE( pCameraShakeData );
			}
			m_vecpCameraShakeData.clear();
		}
	};



	struct EffectInstance
	{
		bool												m_bAlive;
		EffectData*											m_pEffectData;
        CX2DamageEffect::CEffectHandle       						m_hDamageEffect;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshPlayer;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hParticleSequence;
		CX2GameUnitoPtr										m_optrGameUnit;

		EffectInstance() :
		m_bAlive( true ),
		m_pEffectData( NULL ),
        m_hDamageEffect( INVALID_DAMAGE_EFFECT_HANDLE ),
		m_hMeshPlayer( INVALID_MESH_INSTANCE_HANDLE ),
		m_hParticleSequence( INVALID_PARTICLE_SEQUENCE_HANDLE ),
		m_optrGameUnit()	/// Destructor 처리는 해주지 않는다
		{
		}

		~EffectInstance();
	};



	struct EffectSetInstance
	{
		EFFECT_SET_MODE					m_eMode;

        Handle                          m_hHandle;
		CKTDGXSkinAnimPtr				m_pXSkinAnim;

		CX2GameUnitoPtr					m_optrGameUnit;				// owner
		CX2GameUnitoPtr					m_optrTraceTargetGameUnit;		// trace target
		CX2SquareUnitPtr				m_pSquareUnit;				// owner
		bool							m_bHyper;


		bool							m_bCustomPosition;
		D3DXVECTOR3						m_vPosition;
		D3DXVECTOR3						m_vRotationDegree;
		D3DXVECTOR3						m_vDirVector;
		D3DXVECTOR3						m_vZVector;


		float							m_fDelayTime;				// effectset instance에 속한 이펙트를 이 시간 만큼 지난 후에 생성한다
		float							m_fPowerRate;				// effect set 안에서 damage effect가 생성될 때 power rate
		float							m_fPowerRateScale;			// damage effect 의 powerrate에 곱해진다
		float							m_fLifeTime;				// effect set이 살아있는 시간

		D3DXVECTOR3						m_vScale;
		bool							m_bDamageEffectScale;				// 어쩌구저쩌구 다무시하고 무조건 위 스케일값 적용시킨다.

		EffectSetData*					m_pEffectSetData;
		std::vector< bool >				m_vecEffectInstanceCreated;		// 이펙트가 생성되었는지 안되었는지
		std::vector<EffectInstance*>	m_vecpEffectInstance;
		std::vector< bool >				m_vecCameraShakeDone;
		float							m_fPrevElapsedTime;
		float							m_fElapsedTime;


		bool							m_bUseAnimationTiming;		// 유닛의 애니메이션 타이밍에 맞춰서 이펙트를 생성하는지 아닌지

		int				m_iCurrDeleteShakeCount;					// 0보다 클 때 유효하고 이 회수만큼 흔들면 이펙트도 사라진다
		//CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqStickShake;

		bool			m_bCreateEffectSetTraceUnit;				// 지금 effectset이 trace 기능을 사용하는지
		bool			m_bTraceUnitDieDeleteEffectSet;				// trace하던 유닛이 죽으면 그 이펙트를 지운다.

		CX2PET *m_pPet;

		D3DXVECTOR3						m_vLocalRotateDegree;

		// 이펙트셋은 기본적으로 게임 유닛 객체를 기준으로 이동 하게 되어 있음.
		// 이 변수를 사용하면, 위치의 주체를 게임유닛이 아닌 해당 이펙트셋 내의 특정 메쉬로 선택 할 수 있음.
		// BUT 주체가 되는 메쉬는 게임 유닛 객체의 위치에 종속 시킬 수 없음

		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hTargetMesh; // 파티클을 특정 본에 붙일 때 타겟팅 할 메쉬 객체 핸들
		EFFECT_TYPE				m_eTargetMeshEffectType;
		bool					m_bIsTargetMesh;

		bool					m_bIsTargetMeshRight;		/// 타겟 매시의 방향 ( 처음 생성시 유닛의 IsRight 값 )

		D3DXVECTOR3				m_vecTargetMeshDirVector;	/// 타겟 매시의 방향 벡터 ( 처음 생성시 유닛의 방향 벡터 값 )
		D3DXVECTOR3				m_vecTargetMeshZVector;		/// 타겟 매시의 Z 벡터 ( 처음 생성시 유닛의 Z 벡터 값 )


#if X2_DEV_TOOLS
		set<wstring>					m_setEmptyBoneName;
#endif //ADD_RESOURCE_ERROR_LOG


		D3DXVECTOR3		m_vBoneTraceTargetMeshScale;	/// 행렬 Trace일 때, BoneTraceTargetMesh의 스케일 값 빼주기 위한 저장 변수

	private:
		EffectSetInstance() {}; // do not use

	public:
		~EffectSetInstance();
			EffectSetInstance( EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit, CX2PET *pPet);


		void OnFrameMove( double /*fTime*/, float fElapsedTime )
		{
			m_fPrevElapsedTime = m_fElapsedTime;
			m_fElapsedTime += fElapsedTime;
		}

		D3DXVECTOR3 GetEffectPosition( EffectData* pEffectData );
		D3DXVECTOR3 GetEffectScale( EffectData* pEffectData );
		D3DXVECTOR3 GetEffectRotation( EffectData* pEffectData );
		D3DXVECTOR3 GetUnitRotation( EffectData* pEffectData );
		bool IsUnitRight();

		//{{kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)
		const D3DXMATRIX* GetCombineMatrix( const EffectData* pEffectData ) const;
			//}}kimhc // 2011-01-17 // 지정한 Bone의 메트릭스 값을 Trace 함 (chung 코드 참고)

		void SetPowerRateScale(const float val) { m_fPowerRateScale = val; }
		void SetUseAnimationTiming(const bool val) { m_bUseAnimationTiming = val; }

		void SetEffectScale( const D3DXVECTOR3& vScale, bool bDamageEffectSclae = false )
		{
			m_vScale = vScale;
			m_bDamageEffectScale = bDamageEffectSclae;
		}
		void SetLocalRotateDegree( const D3DXVECTOR3& vRot )
		{
			m_vLocalRotateDegree = vRot;
		}
		void SetEffectPosition( const D3DXVECTOR3 &vPos, bool bCustomPosition = true )
		{
			m_bCustomPosition = bCustomPosition;

			m_vPosition = vPos;
		}

		CX2GameUnit* GetOwnerUnit() const;

		void SetEffectSetOffset( OUT D3DXVECTOR3& vPos_, IN const D3DXVECTOR3& vOffsetPos_, IN const EffectData* pEffectData_ );

		CKTDGXMeshPlayer::CXMeshInstance* GetMeshInstanceByTargetMesh();
		bool GetEffectPositionByTargetMesh( OUT D3DXVECTOR3& vPos, IN EffectData* pEffectData );

        Handle  GetHandle() const { return m_hHandle; }
        void    SetHandle( Handle handle )  { m_hHandle = handle; }

		void SetPos( IN const D3DXVECTOR3& vOffsetPos_ );

	};


public:
	CX2EffectSet(
            unsigned char ucSystemID = 0
        );
	~CX2EffectSet(void);


	void OnFrameMove( double fTime, float fElapsedTime );

	void OnFrameMove_CreateEffect( double fTime, float fElapsedTime );
	void OnFrameMove_DeleteEffect();
	void OnFrameMove_UpdateEffect();
	void OnFrameMove_StartCameraShake();

	CX2EffectSet::Handle PlayEffectSetByPet_LUA( const char* strEffectSetName, CX2PET* pPet );
	CX2EffectSet::Handle PlayEffectSetByPet( const wstring& wstrEffectSetName, CX2PET* pPet );

	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit );
	CX2EffectSet::Handle PlayEffectSet( const wstring& wstrEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit = NULL,
		bool bHyper = false, float fPowerRate = -1.f, float fLifeTime = -1.f, D3DXVECTOR3 vScale = D3DXVECTOR3( 1, 1, 1 ),
		bool bCustomPosition = false, const D3DXVECTOR3& vPosition = D3DXVECTOR3( 0, 0, 0 ), const D3DXVECTOR3& vRotationDegree = D3DXVECTOR3( 0, 0, 0 ), const D3DXVECTOR3& vDirVector = D3DXVECTOR3( 0, 0, 0 ) );

#ifdef HANABI_VILLAGE
	CX2EffectSet::Handle PlayEffectSetEvent( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, D3DXVECTOR3 vPosition );
#endif // HANABI_VILLAGE
	CX2EffectSet::Handle PlayEffectSetCustomPos( const wstring& wstrEffectSetName, CX2SquareUnitPtr pSquareUnit, const D3DXVECTOR3& vPos );
	void StopEffectSet( CX2EffectSet::Handle& hHandle );
	void StopEffectSetAll();
	D3DXVECTOR3 GetEffectPosition_LUA( Handle hHandle, int iEffectIndex );

	//{{ kimhc // 2010.4.19 // 비밀던전 작업
	CX2EffectSet::Handle PlayEffectSet_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit );

	CX2EffectSet::Handle PlayEffectSetWithCustomPos_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, D3DXVECTOR3 vPos , D3DXVECTOR3 vRotationDegree);
	void StopEffectSet_LUA( CX2EffectSet::Handle hHandle );
	//}} kimhc // 2010.4.19 // 비밀던전 작업

	//{{ JHKang / 강정훈 / 2011/01/19 / 이펙트를 가지고 있는 대상(NPC)과 이펙트가 따라다닐 대상(User)를 나누어 사용 시
	CX2EffectSet::Handle PlayEffectSetTraceTarget_LUA( const char* pEffectSetName, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit );
	//}} JHKang / 강정훈 / 2011/01/19

	EffectSetInstance* GetEffectSetInstance( CX2EffectSet::Handle hHandle );
    bool               IsLiveInstanceHandle( CX2EffectSet::Handle hHandle ) { return GetEffectSetInstance( hHandle ) != NULL; }
    EffectSetInstance* ValidateInstanceHandle( CX2EffectSet::Handle& hHandle )
    {
        EffectSetInstance* pInstance = GetEffectSetInstance( hHandle );
        if ( pInstance == NULL )
            hHandle = INVALID_EFFECTSET_HANDLE;
        return pInstance;
    }
    void                DestroyInstanceHandle( CX2EffectSet::Handle& hHandle ) { StopEffectSet( hHandle ); }

	EffectSetData* GetEffectSetTemplet( const wstring& wstrEffectSetName );

	void RegisterLuaBind();
	bool OpenScriptFile( const wstring& wstrFileName );
	bool AddEffectSetTemplet_LUA();
	void ClearTempletAndInstance();

	static D3DXVECTOR3 GetZVector( const D3DXVECTOR3& vDirVector );

	void SetEffectSetInstPos( CX2EffectSet::Handle hHandle, D3DXVECTOR3 vPos, bool bCustomPosition = true  )
	{
		if( hHandle == INVALID_EFFECTSET_HANDLE )
			return;

		EffectSetInstance *pInst = GetEffectSetInstance( hHandle );
		if( pInst != NULL )
		{
			pInst->SetEffectPosition( vPos, bCustomPosition );
		}
	}

	CX2EffectSet::Handle PlayEffectSetByMeshPlayer( const WCHAR* wstrEffectSetName,
													CKTDGXMeshPlayer::CXMeshInstanceHandle hTargetMesh, EFFECT_TYPE	eTargetMeshType );

	void SetEffectScale_LUA (CX2EffectSet::Handle hHandle, D3DXVECTOR3 vScale, bool bDamageEffectSclae )
	{
		if( hHandle == INVALID_EFFECTSET_HANDLE )
			return;

		EffectSetInstance *pInst = GetEffectSetInstance( hHandle );
		if( pInst != NULL )
		{
			pInst->SetEffectScale( vScale, bDamageEffectSclae );
		}
	}


#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
public :
	typedef std::map< std::wstring, EffectSetData* > EffectSetDataMap;
	EffectSetDataMap & GetMapEffectSetTemplet(){ return m_mapEffectSetTemplet;}
	bool MergeEffectSetTemplet( EffectSetData* pNewEffectSetTemplet_, EffectSetData * pOrgEffectSetTemplet_ );

	bool CreateEffectSetTemplet( EffectSetData* pEffectSetData_ );
	bool DeleteEffectSetTemplet( EffectSetData* pEffectSetData_ ) ;
#endif // EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가

    DWORD   ComposeHandle( WORD wIndex, OUT WORD& wStamp )
    {
        wStamp &= 0x3fff;
        return wIndex | ( wStamp << 16L ) | ( m_ucSystemID << 30L );
    }
    bool    DecomposeHandle( DWORD dwHandle, OUT WORD& wIndex, OUT WORD& wStamp )
    {
        wIndex = (WORD) dwHandle;
        wStamp = (WORD) ( ( dwHandle & 0xffff0000 ) >> 16L );
        unsigned char ucSystemID = (unsigned char) ( wStamp >> 14L );
        wStamp &= 0x3fff;
        return ucSystemID == m_ucSystemID;
    }
    unsigned char GetSystemID()                     { return m_ucSystemID; }

private:


    EffectSetInstance*  _CreateEffectSetInstance(
         EffectSetData* pEffectSetData, CKTDGXSkinAnimPtr pSkinAnim, CX2GameUnit* pGameUnit, CX2GameUnit* pTraceTargetGameUnit, CX2SquareUnitPtr pSquareUnit,
    CX2PET *pPet = NULL
    );

#if !X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
	typedef std::map< std::wstring, EffectSetData* > EffectSetDataMap;
#endif // f EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가
	EffectSetDataMap					m_mapEffectSetTemplet;


    enum    EListType
    {
        LIST_LIVE = 0,
        LIST_FREE = 1,
        LIST_NUM
    };
    struct  KInstanceHandleInfo
    {
        EffectSetInstance*      m_pInstance;
        WORD                    m_wStamp;
        KInstanceHandleInfo()
            : m_pInstance( NULL )
            , m_wStamp(0)
        {
        }
    };
    typedef kog::indexed_list<KInstanceHandleInfo> KInstanceHandleList;
    KInstanceHandleList             m_coInstanceHandleList;




        unsigned char                           m_ucSystemID;
};
