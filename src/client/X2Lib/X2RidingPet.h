#pragma once
/**
	@file	X2RidingPet.h
	@author	JHKang
	@brief	Ż �� ����
	@since  2013-03-26

	@date	JHKang on 2013-03-26,
			- Ŭ���� ����
	@date	JHKang on 2013-03-28,
			- �⺻ �Լ� ����
	@date	JHKang on 2013-04-01,
			- ���ҽ� �ε� ���� �Լ� ����
	@date	JHKang on 2013-04-02,
			- ��׶��� �ε� ���� �Լ� ����
			- ��ũ��Ʈ �Ľ� �κ� �ۼ�
	@date	JHKang on 2013-04-10
			- ����ȭ ������ �ۼ�
			- ����� ������ ����
			- Bimap�� ����� ������ Index
			- ������ �Լ� ���� -> Ż ���� ����ؼ� �Ҹ�Ǵ� ���� �߻�
	@date	JHKang on 2013-04-11
			- ���Ⱑ �ִ� ���θʿ� ���� ���� ó��
			- "F2" Reset ġƮ ����
	@date	JHKang on 2013-04-14
			- ���� / �ʵ� ������ Ż �� ��� �����ϵ��� ����
	@date	JHKang on 2013-04-16
			- ���� / �ʵ� ������ Ż �� Walk, Wait, Damage, Jump, Dash ���� ���� �ۼ�
	@date	JHKang on 2013-04-17
			- ���� / �ʵ� ������ Ż �� ���� ���� �ۼ�
			- Physic ���� �Ķ���� �Ľ� ���� �߰�
	@date	JHKang on 2013-04-28
			- Ư�� �̵� ��� �߰�
	@data	JHKang on 2013-06-02
			- �����ǿ� Ż �� �߰�

*/
#include <boost/bimap/bimap.hpp>
#include "./X2SoundCloseManager.h"

class CX2RidingPet;
typedef boost::intrusive_ptr<CX2RidingPet> CX2RidingPetPtr;

//#pragma region CX2RidingPet
/** @class : CX2RidingPet
	@brief : Ż ���� ����� �����ϴ� Ŭ����
	@date  : 2013/03/27
*/
class CX2RidingPet : public CKTDGObject, public CKTDXDeviceHolder
{
//#pragma region public
public:
	//#pragma region RIDING_STATE_ID
	enum RIDING_STATE_ID
	{
		RSI_NONE	= 0,	/// ���� ����
		RSI_LOADING,		/// �ε�
		RSI_READY,			/// �غ�
		RSI_PLAY,			/// ��� ��
		RSI_END,			/// ���� ������
	};
	//#pragma endregion Ż �� ���� ����ü

	//#pragma region RIDING_SPECIAL_MOVE
	enum RIDING_SPECIAL_MOVE
	{
		RSM_NONE			= 0,
		RSM_AIR_DASH,		/// ���� �뽬
		RSM_DOUBLE_JUMP,	/// �̴� ����
		RSM_BOOST_DASH,		/// �ν�Ʈ �뽬
		RSM_PASS_DASH,		/// ��� �뽬
		RSM_END,
	};
	//#pragma endregion Ż �� Ư�� �̵�

	//#pragma region InitDeviceData
	struct InitDeviceData
	{
		std::vector<std::wstring>    m_vecstrReadyTexture;		/// �ؽ�ó �̹�����
		std::vector<std::wstring>    m_vecstrReadyXMesh;		/// �޽���
		std::vector<std::wstring>    m_vecstrReadyXSkinMesh;	/// ��Ų �޽���
		std::vector<std::wstring>    m_vecstrReadySound;		/// �����
		std::vector<std::wstring>	 m_vecstrReadyXET;			/// XET ��

		void AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const;

	};
	//#pragma endregion �ʱ�ȭ ����ü

	//#pragma region InitData
	struct InitData
	{
		InitDeviceData	m_device;		/// ����̽� ����ü
		void			AppendToDeviceList( CKTDXDeviceDataList& listInOut_ ) const;
	};
	//#pragma endregion

	//#pragma region SoundPlayData
	struct SoundPlayData
	{
		float		m_SoundPlayTime;		/// ��� �ð�
		wstring		m_SoundPlayName;		/// ���� �̸�
		int			m_SoundPlayRate;		/// ��� ��
		bool		m_bOnlyIfMyUnit;		/// ���� ���
		float		m_fMaxSoundDistance;	/// �ִ� �Ÿ�
		bool		m_b3DSound;				/// 3D ����
	};
	//#pragma endregion ���� ����ü

	//#pragma region RidingPetSyncData
	struct RidingPetSyncData
	{
		bool			m_bIsRight;		/// ����
		bool			m_bStateChange;	/// ���� ����
		bool			m_bFootOnLine;	/// ������ ��� �ִ°�?

		char			m_nowState;			/// ����
		char			m_StateChangeNum;	/// ���� ���� ��ȣ
		D3DXVECTOR3		m_vPos;				/// ��ġ

		UCHAR			m_ucLastTouchLineIndex;		/// �ֱ� ������ ���� �ε��� ��ȣ

		RidingPetSyncData()
		{
			Init();
		}

		void Init()
		{
			m_bIsRight = true;
			m_bStateChange = false;

			m_nowState			= -1;
			m_StateChangeNum	= 0;
			INIT_VECTOR3( m_vPos, 0, 0, 0 );

			m_ucLastTouchLineIndex	= 0;
		}

		void SetData( IN RidingPetSyncData* pSyncData_ )	// ���� ������ �ٲ�� ��
		{
			m_bIsRight		= pSyncData_->m_bIsRight;
			m_bStateChange	= pSyncData_->m_bStateChange;

			m_vPos			= pSyncData_->m_vPos;

			m_ucLastTouchLineIndex	= pSyncData_->m_ucLastTouchLineIndex;
		}
	};
	//#pragma endregion Ż �� ����ȭ ������

	//#pragma region RidingPetCondition
	struct RidingPetCondition
	{
		bool		m_bLandConnection;
		float		m_fUnitWidth;
		float		m_fUnitHeight;

		D3DXVECTOR3	m_vDirDegree;
		D3DXVECTOR3	m_vDir;
		D3DXVECTOR3	m_vLandPosition;

		RidingPetCondition()
		{
			Init();
		}

		void Init()
		{
			m_bLandConnection	= true;
			m_fUnitWidth		= 0.f;
			m_fUnitHeight		= 0.f;

			INIT_VECTOR3( m_vDirDegree, 0, 0, 0 );
			INIT_VECTOR3( m_vDir, 0, 0, 0 );
			INIT_VECTOR3( m_vLandPosition, 0, 0, 0 );
		}
	};
	//#pragma endregion ����ü

	//#pragma region RidingPetInfo
	struct RidingPetInfo
	{
		UidType	m_Uid;			/// Ż �� UnitID
		int		m_Id;			/// Ż �� ID

		wstring	m_wstrName;		/// Ż �� �̸�
		bool	m_bUse;			/// ��� ����

		RIDING_SPECIAL_MOVE	m_eSpecialMove;		/// Ư�� �̵�

		KProtectedType<float>	m_fStamina;	/// ü��

		RidingPetInfo()
		{
			Init();
		}

		RidingPetInfo( IN const KRidingPetInfo& rhs_ )
			: m_wstrName( L"" )
		{
			m_Uid		= rhs_.m_iRidingPetUID;
			m_Id		= rhs_.m_usRindingPetID;
			m_fStamina	= rhs_.m_fStamina;
			m_bUse		= true;
		}

		void Init()
		{
			m_Uid = -1;
			m_Id = -1;
			m_wstrName = L"";
			m_bUse = false;
			m_fStamina = 0;
			m_eSpecialMove = RSM_NONE;
		}

		bool Verify()
		{
			if( m_fStamina.Verify() == false )
				return false;
			else
				return true;
		}
	};
	//#pragma endregion Ż �� ���� ����ü

	//#pragma region PhysicParam
	struct PhysicParam
	{
		D3DXVECTOR2	m_vNowSpeed;		/// ���� ���ǵ�X,Y
		D3DXVECTOR2	m_vPassiveSpeed;	/// �� ���ǵ�� ��� �������ش�

		float			fDownAccel;		/// �ϰ� ���ӵ�
		float			fReloadAccel;	/// ���� �ӵ� ������
		float			fGAccel;		/// �߷� ���ӵ�
		float			fMaxGSpeed;		/// �ִ� �ϰ� �ӵ� Y
		float			fWalkSpeed;		/// �ȱ� �ӵ�
		float			fRunSpeed;		/// �޸��� �ӵ�
		float			fJumpSpeed;		/// �Ϲ� ���� ��¼ӵ�
		float			fDashJumpSpeed;	/// ��� ���� ��¼ӵ�

		PhysicParam()
		{
			INIT_VECTOR2( m_vNowSpeed, 0.0f, 0.0f );
			INIT_VECTOR2( m_vPassiveSpeed, 0.0f, 0.0f );
			fDownAccel		= 0.0f;
			fReloadAccel	= 0.0f;
			fGAccel			= 0.0f;
			fMaxGSpeed		= 0.0f;
			fWalkSpeed		= 0.0f;
			fRunSpeed		= 0.0f;
			fJumpSpeed		= 0.0f;
			fDashJumpSpeed	= 0.0f;
		}
	};
	//#pragma endregion ���� �Ķ���� ����ü

	//#pragma region STATE_CHANGE_TYPE
	enum STATE_CHANGE_TYPE
	{
		SCT_NO_CHANGE,
		SCT_MOTION_END,
		SCT_FOOT_ON_LINE_FALSE_DOWN,
		SCT_FOOT_ON_LINE_TRUE,
		SCT_POSITIVE_Y_SPEED,
		SCT_NEGATIVE_Y_SPEED,

		SCT_CONDITION_FUNCTION,
		SCT_CONDITION_TABLE,

		SCT_AI_MESSAGE_BEGIN,
		SCT_AI_WAIT,
		SCT_AI_WALK,
		SCT_AI_DASH,
		SCT_AI_JUMP,
		SCT_AI_DOWN,
		SCT_AI_JUMP_DIR,
		SCT_AI_DOWN_DIR,
		SCT_AI_DIR_CHANGE,

		SCT_AI_MESSAGE_END,

	};
	//#pragma endregion ���� ���� Ÿ��

	//#pragma region RidingPetStateData
	struct RidingPetStateData
	{
		int			m_StateID;

	public:
		~RidingPetStateData()
		{
		}

		RidingPetStateData()
		{
			Init();
		}

		virtual void Init()
		{
			m_StateID			= RSI_NONE;
		}

		// ����!! �� copy constructor, assignment operator �����ϸ� �ȵ�
		RidingPetStateData( const RidingPetStateData& t )
		{
			*this = t;
		}

		RidingPetStateData& operator=( const RidingPetStateData& data )
		{
			m_StateID			= data.m_StateID;

			return *this;
		}
	};
	//#pragma endregion ���� ������

	//#pragma region typedef
	typedef boost::bimaps::bimap< char, string > StateBiMap;				/// ���� �� ���� ���� ����� ���� bimap���� �ٲ�
	typedef boost::bimaps::bimap< char, string >::value_type BiMapValue;	/// biMap�� �����ϱ� ���� ��
	typedef std::map< char, RidingPetStateData > RidingPetStateDataMap;		/// StateDataMap
	typedef map< float, bool > EventTimeMap;								/// �̺�Ʈ Ÿ�̸� ��
	//#pragma endregion Ÿ�� ����

//#pragma endregion enum, struct, Inner Class

//#pragma region public
public:
	CX2RidingPet( void );
	virtual ~CX2RidingPet( void );

	static CX2RidingPetPtr CreateRidingPetPtr()
	{
        CX2RidingPetPtr pObject( new CX2RidingPet );
		return pObject;
	}

	virtual HRESULT			OnFrameMove( IN double fTime_, IN float fElapsedTime_ );
	virtual RENDER_HINT		OnFrameRender_Prepare();
	virtual void			OnFrameRender_Draw();
	void					CommonCameraFrameMove();
	void					CommonFrameMove_TimedEvent();

	virtual HRESULT			OnResetDevice();
	virtual HRESULT			OnLostDevice();

	static void InitDevice( IN InitDeviceData& OutDevice_, IN KLuaManager& luaManager_ );
	static void InitMotion( IN InitDeviceData& OutDevice_, IN KLuaManager& luaManager_ );
	static void InitInit( IN InitData& OutInit_, IN KLuaManager& luaManager_ );

	void SetRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ );
	void SetGameRidingPet( IN lua_State* pPetLuaState_, IN UidType uUserUid_ );
	KLuaManager& GetLuaManager() { return m_LuaManager; }

	void SetInfo( IN const RidingPetInfo& ridingPetInfo_, IN RIDING_SPECIAL_MOVE eSpecialMove_ );
	const RidingPetInfo &GetInfo() const { return m_Info; }

	void VerifyRindingPetInfo() { m_Info.Verify(); }

	UidType GetUserUid() const { return m_UserUid; }
	void SetUserUid( IN UidType userUid ) { m_UserUid = userUid; }
	void GetUserInfo( IN D3DXVECTOR3& vPos, IN bool& bIsRight_ );

	bool GetLoaded() const { return m_bLoaded; }
	const D3DXVECTOR3& GetPos() const { return m_SyncData.m_vPos; }
	void SetPos( IN D3DXVECTOR3& vPos_, IN bool bIsRight_ );
	bool InitPos();

	D3DXVECTOR3 GetBonePos( IN const WCHAR* pBoneName_ ) const
	{
		return m_pXSkinAnim->GetCloneFramePosition( pBoneName_ );
	}
	bool GetBonePos( IN const WCHAR* pBoneName_, OUT D3DXVECTOR3& vPos_ ) const
	{
		return m_pXSkinAnim->GetCloneFramePosition( pBoneName_, vPos_);
	}

	const int GetNowState() { return m_SyncData.m_nowState; }

	const CKTDGXSkinAnim* GetXSkinAnim() const
	{
		if ( m_bLoaded && NULL != m_pXSkinAnim )
			return m_pXSkinAnim.get();
		else
			return NULL;
	}

	CKTDGXSkinAnimPtr GetXSkinAnimPtr() const { return m_pXSkinAnim; }

	CKTDGXSkinAnimPtr GetXSkinAnimFuturePtr() const { return m_pXSkinAnimFuture; }

	int GetStateID( IN const char* wcState_ )
	{
		if ( NULL == wcState_ )
			return RSI_NONE;

		return GetStateID( std::string( wcState_ ) );
	}

	int GetStateID( IN const std::string& strInState_ )
	{
		StateBiMap::right_const_iterator rightIter = m_StateBiMap.right.find( strInState_ );

		if ( rightIter == m_StateBiMap.right.end() )
			return RSI_END;

		return rightIter->second;
	}

	float GetUnitWidth() { return m_ConditionData.m_fUnitWidth; }
	float GetUnitHeight() { return m_ConditionData.m_fUnitHeight; }
	const PhysicParam& GetPhysicParam() const { return m_PhysicParam; }

	void StateChangeFuture( IN int iStateID_ );
	void StateChangeFuture( IN const char* stateName_ );

	void StateChange( IN int iStateID_ );
	void StateChange( IN const char* stateName_ );
	void StateChange_LUA( IN const char* stateName_ );


//#pragma endregion function

//#pragma region private
private:
	bool IsValidUser();

	void InitDevice();
	void InitSystem();
	void InitMotion();
	void InitPhysic();
	void InitState();

	void ClearState();
	void DoFrameMove();
//#pragma endregion function

//#pragma region private
private:
	KLuaManager		m_LuaManager;	/// ��� ��ũ��Ʈ �Ŵ���

	bool	m_bForceBillBoard;		/// ���� ������ ����
	bool	m_bForceColor;			/// ���� ���� ��ȯ
	bool	m_bForceColorChange;	/// ���� ���� ���� ��ȯ
	bool	m_bGame;				/// ���� / �ʵ� ����
	bool	m_bLoaded;				/// �ҷ���
	bool	m_bMine;				/// ���� Ż ��
	bool	m_bIgnoreLineSpeed;		/// ���� ���ǵ� ����
	bool	m_bInit;				/// �ʱ�ȭ �Ϸ�
	bool	m_bInitPos;				/// ��ġ �ʱ�ȭ
	bool	m_bSetData;				/// ������ ����
	bool	m_bUnderWaterHead;		/// ����?

	wstring		m_wstrAniTex;		/// Texture ��
	wstring		m_wstrChangeTex;	/// ����� Texture ��
	wstring		m_wstrMotionName;	/// ��� ��
	wstring		m_wstrRidingMotion;	/// ������ Ÿ�� ���

	CKTDXDeviceXSkinMesh*	m_pMotion;		/// ��Ų �޽�
	CKTDGXSkinAnimPtr		m_pXSkinAnim;	/// �ִϸ��̼�
	CKTDGXSkinAnimPtr		m_pXSkinAnimFuture;	/// �ִϸ��̼�

	CKTDXDeviceXET*			m_pAniXET;		/// XET
	CKTDXDeviceXET*			m_pChangeXET;	/// �ٲ� XET

	CKTDGXRenderer::RenderParam		m_RenderParam;	/// ������ �Ķ����
	D3DXCOLOR		m_cColor;			/// Ż �� ����
	D3DXCOLOR		m_cLineUnitColor;	/// ���� ����

	RidingPetStateData	m_StateData;		/// ���� ������
	RidingPetCondition	m_ConditionData;	/// ����� ������ : ������ ������ �޾ƾ���
	RidingPetInfo		m_Info;				/// Ż �� ����
	RidingPetSyncData	m_SyncData;			/// ��ũ ������ : ������ ������ �޾ƾ���

	PhysicParam		m_PhysicParam;			/// ���� ���� ��꿡 ���Ǵ� �Ķ����

	StateBiMap				m_StateBiMap;		/// ����ID, ���¸�
	RidingPetStateDataMap	m_StateDataMap;		///

	UidType		m_UserUid;		/// ���� �����

	float		m_fElapsedTime;		/// �ð�
	float		m_fNowStateTimer;	/// ���� ���� �ð�
	float		m_fScale;			/// ũ��

	vector<SoundPlayData>	m_vecSoundPlayData;		/// ���� ������
//#pragma endregion variable

//#pragma region public
public:
	int		m_StartState;				/// ���� ����
	int		m_WaitState;				/// ���
	int		m_WaitHabitState;			/// ����
	int		m_WalkState;				/// �ȱ�
	int		m_JumpUpState;				/// ���� ��
	int		m_JumpDownState;			/// ���� �ٿ�
	int		m_JumpLandingState;			/// ����
	int		m_DashState;				/// ���
	int		m_DashEndState;				/// ��� ����
	int		m_DashJumpUpState;			/// ��� ����
	int		m_DashJumpLandingState;		/// ��� ���� ����
	int		m_DamageFrontState;			/// �ǰ� ��
	int		m_DamageBackState;			/// �ǰ� ��
	int		m_DyingState;				/// ����
	int		m_AttackZState;				/// ���� Z
	int		m_JumpAttackZState;			/// ���� ���� Z
	int		m_AttackXState;				/// ���� X
	int		m_SpecialAttackState;		/// Ư�� ����
	int		m_SitReadyState;			/// �ɱ� ����
	int		m_SitWaitState;				/// �ɱ�
	int		m_StandUpState;				/// �Ͼ��
	int		m_SpecialMoveState;			/// Ư�� �̵�
//#pragma endregion variable

};
//#pragma endregion Ż �� Ŭ����

struct RidingPetInitTemplet
{
	CX2RidingPet::InitData	m_init;
};
