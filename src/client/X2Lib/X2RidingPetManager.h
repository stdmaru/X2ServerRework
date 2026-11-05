#pragma once
/**
	@file	X2RidingPetManager.h
	@author	JHKang
	@brief	Ż �� ���� Ŭ����
	@since  2013-04-02

	@date	JHKang on 2013-04-02,
			- Ŭ���� ����
	@date	JHKang on 2013-04-02,
			- Riding Templet ���� �Լ� ����
	@date	JHKang on 2013-04-03,
			- BackGround �ε� �κ� �� ���ҽ� �ε� �κ� ����
	@date	JHKang on 2013-04-10
			- ������ ����� �� ����� Handler �Լ� ���� -> ���� Ŭ���������� �ۼ���
	@date	JHKang on 2013-04-28
			- Ư�� �̵� ��� �߰�
	@date	JHKang on 2013-05-09
			- �Ŵ��� ��ü�� �̱��� ��ü�� ����( �ڵ� �ǵ�� )
	@date	JHKnag on 2013-06-02
			- �����ǿ� Ż �� �߰�
*/
#include "X2GUUser.h"
#include "X2GUChung.h"
#include "X2GUArme_VioletMage.h"
#include "X2GULire_ElvenRanger.h"

class CX2RidingPet;

struct RidingPetInitTemplet;

//#pragma region CX2RidingPetManager
/** @class : CX2RidingPetManager
	@brief : Ż �� ���� Ŭ����
	@date  : 2013/04/02
*/
class CX2RidingPetManager
{
//#pragma region public
public:
	//#pragma region sRidingPetInfo
	struct sRidingPetInfo : public KRidingPetInfo
	{
		int					  m_iPetListOrder;		// �� ����Ʈ ���� ���� ( ������ ž�� �ð��� ���� ���� )
		KProtectedType<float> m_fProtectStamina;

		sRidingPetInfo():KRidingPetInfo(),
			m_fProtectStamina(0.f)
		{}

		sRidingPetInfo(const KRidingPetInfo& KRidingPetInfo_)
		{
			m_iRidingPetUID			= KRidingPetInfo_.m_iRidingPetUID;
			m_usRindingPetID		= KRidingPetInfo_.m_usRindingPetID;
			m_fStamina				= KRidingPetInfo_.m_fStamina;
			m_wstrDestroyDate		= KRidingPetInfo_.m_wstrDestroyDate;

			m_fProtectStamina		= KRidingPetInfo_.m_fStamina;
			m_iPetListOrder		= static_cast<UINT>(KRidingPetInfo_.m_iLastUnSummonDate);
		}

		sRidingPetInfo& operator=( const KRidingPetInfo& KRidingPetInfo_ )
		{
			m_iRidingPetUID			= KRidingPetInfo_.m_iRidingPetUID;
			m_usRindingPetID		= KRidingPetInfo_.m_usRindingPetID;
			m_fStamina				= KRidingPetInfo_.m_fStamina;
			m_wstrDestroyDate		= KRidingPetInfo_.m_wstrDestroyDate;

			m_fProtectStamina		= KRidingPetInfo_.m_fStamina;
			m_iPetListOrder		= static_cast<UINT>(KRidingPetInfo_.m_iLastUnSummonDate);
			return *this;
		}

		bool Verify()
		{
			if( m_fProtectStamina.Verify() == false )
			{
				g_pKTDXApp->SetFindHacking( true );
				return false;
			}
			else
				return true;
		}
	};
	//#pragma endregion KRidingPetInfo ����ü�� ��ӹ���

	//#pragma region RIDING_PET_UNIT_ID
	enum RIDING_PET_UNIT_ID
	{
		RPUI_NONE			= 0,	/// ����
		RPUI_NASOD_MOBI,			/// ���ҵ� ���
		RPUI_KOUCIKA,				/// �ڿ��ī
		RPUI_ANCIENT_PPORU,			/// ���μ�Ʈ �Ƿ�
		RPUI_SCORPION_TYPE_R,		/// ���� �����ǿ� TYPE-R
		RPUI_ICE_HAMELING,			/// ���̽� �ϸḵ
		RPUI_DARK_HAMELING,			/// ��ũ �ϸḵ
		RPUI_RED_HAMELING,			/// ���� �ϸḵ

		// ��ȹ�� ����� �ɷ�ġ�� �ؿ����� �ݿ����� �ʱ� ���� �߰� �� ID ~
		RPUI_NASOD_MOBI_INT     = 8,    /// ������ ���� ���� ���RT
		RPUI_KOUCIKA_INT        = 9,    /// ������ �ٶ��� �ڿ��ī
		RPUI_ANCIENT_PPORU_INT  = 10,   /// ������ ���̼�Ʈ ���̵� �Ƿ�
		// ~ ��ȹ�� ����� �ɷ�ġ�� �ؿ����� �ݿ����� �ʱ� ���� �߰� �� ID

		RPUI_OBERON_CUSTOM_650	= 11,	///	�̺� Ŀ����-������650
		RPUI_ANCIENT_PPORU_NAVER = 12,	///	���̹� ä�θ� �̺�Ʈ ���̼�Ʈ �Ƿ�

		RPUI_OBERON_CUSTOM_650_BLACK_SKULL = 13,	/// �̺� Ŀ���� - ���� ����
		RPUI_OBERON_CUSTOM_650_ANGEL_HEART = 14,	/// �̺� Ŀ���� - ���� ��Ʈ

		RPUI_END,					/// ��
	};
	//#pragma endregion Ż �� ���̵� ����ü

	//#pragma region RIDING_PET_CUSTOM_UI_MSG
	enum RIDING_PET_CUSTOM_UI_MSG
	{
		RPCUM_RIDING_ON,
		RPCUM_BASE_INFO,
		RPCUM_DETAIL_INFO,
	};
	//#pragma endregion UI �޽��� ����ü

	//#pragma region FaceImage
	struct FaceImage
	{
		wstring m_wstrImageName;
		wstring m_wstrKeyName;

		FaceImage()
		{
			Init();
		}

		void Init()
		{
			m_wstrImageName = L"";
			m_wstrKeyName = L"";
		}
	};
	//#pragma endregion Ż �� �̹��� ����

	//#pragma region RidingPetSkillInfo
	struct RidingPetSkillInfo
	{
		wstring m_wstrImageName;	/// �̹��� ���ϸ�
		wstring m_wstrKeyName;		/// �̹��� Ű��
		wstring m_wstrSkillDesc;	/// ��ų ��
		KProtectedType<float>	m_fConsumeMana;		/// ���� �Һ�
		KProtectedType<float>	m_fCoolTime;		/// ��ų ��Ÿ��

		RidingPetSkillInfo()
		{
			m_wstrImageName = L"";
			m_wstrKeyName = L"";
			m_wstrSkillDesc = L"";
			m_fConsumeMana = 0.f;
			m_fCoolTime = 0.f;
		}

		void Verify()
		{
			if( m_fConsumeMana.Verify() == false || m_fCoolTime.Verify() == false )
			{
				g_pKTDXApp->SetFindHacking( true );
			}
		}
	};
	//#pragma endregion ��ų ���� ����ü

	//#pragma region RidingPetBuffInfo
	struct RidingPetBuffInfo
	{
		wstring m_wstrImageName;	/// �̹��� ���ϸ�
		wstring m_wstrKeyName;		/// �̹��� Ű��
		wstring m_wstrSkillDesc;	/// ��ų ��
		BUFF_FACTOR_ID	m_eBuffFactorID;	/// ����Ǿ�� �� ���� ���� ID

		RidingPetBuffInfo()
		{
			m_wstrImageName = L"";
			m_wstrKeyName = L"";
			m_wstrSkillDesc = L"";
			m_eBuffFactorID = BFI_NONE;
		}
	};
	//#pragma endregion ��ų ���� ����ü

	typedef std::map< UidType, CX2RidingPetPtr > RidingPetMap;
	typedef std::vector<CX2RidingPetPtr> RidingPetVec;
	typedef	RidingPetInitTemplet RPInitTemplet;

	//#pragma region RidingPetInitLuaTemplet
	struct	RidingPetInitLuaTemplet
	{
		RPInitTemplet*	m_pInitTemplet;
		KLuaManager*	m_pLuaManager;

		RidingPetInitLuaTemplet()
		{
			m_pInitTemplet = NULL;
			m_pLuaManager = NULL;
		}
	};
	//#pragma endregion ���̵� �� ��� ���� ��׶��� �ε�

	//#pragma region RidingPetTemplet
	struct RidingPetTemplet
	{
		RIDING_PET_UNIT_ID	m_Uid;		/// Ż �� Unit ID
		wstring		m_wstrName;			/// �̸�
		wstring		m_wstrScripName;	/// ��ũ��Ʈ �̸�
		float		m_fIncreaseRate;	/// �ִ� ��� ���� ����
		float		m_fRecoveryRate;	/// ��� ȸ�� ����
		float		m_fMaxStamina;		/// Ż �� �ִ� ���


		float		m_fViewerSizeX;	/// ���� ���� ������ X
		float		m_fViewerSizeY;	/// ���� ���� ������ Y
		float		m_fViewerSizeZ;	/// ���� ���� ������ Z

		wstring		m_wstrDescription;	/// Ż �� ����
		RidingPetInitLuaTemplet				m_InitLuaTemplet;	/// ���ø� ��׶���
		FaceImage							m_FaceImage;		/// �� UI���� �� �̹���
		std::vector<RidingPetSkillInfo>		m_vecAttackSkill;	/// ���� ��ų ���� ����Ʈ
		std::vector<RidingPetBuffInfo>		m_vecBuff;			/// ���� ���� ����Ʈ
		std::vector<int>					m_vecSocketOption;	/// ���� ���� ��ȣ
		CX2RidingPet::RIDING_SPECIAL_MOVE	m_eSpecialMove;		/// Ư�� �̵�

		RidingPetTemplet()
		{
			m_Uid			= RPUI_NONE;
			m_wstrName		= L"";
			m_wstrScripName	= L"";
			m_fIncreaseRate = 1.f;
			m_fRecoveryRate = 0.f;
			m_fMaxStamina	= 0;


			m_fViewerSizeX	= 1.f;
			m_fViewerSizeY	= 1.f;
			m_fViewerSizeZ	= 1.f;

			m_wstrDescription = L"";
// 			#ifdef X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
// 			m_InitLuaTemplet.
// 			#endif //X2OPTIMIZE_GAME_PET_BACKGROUND_LOAD
			m_FaceImage.Init();
			m_vecAttackSkill.clear();
			m_vecBuff.clear();
			m_vecSocketOption.clear();
			m_eSpecialMove = CX2RidingPet::RSM_NONE;
		}

		~RidingPetTemplet();
	};

	typedef std::map< UINT, RidingPetTemplet* > RidingPetTempletMap;
//#pragma endregion Ż �� ������ ������ ���ø�

//#pragma endregion enum, struct, Inner Class

//#pragma region public
public:
	static CX2RidingPetManager* GetInstance();
	static void DestroyInstance();

	HRESULT	OnFrameMove( IN double fTime_, IN float fElapsedTime_ );
	HRESULT OnFrameRender();

	void NotifyShowObjectChanged( IN UidType uUserUid_, IN bool bVal_ );
	void SetShowAll( IN bool bVal_ );
	bool GetShowAll()
	{
		return m_bShow;
	}
	void SetShow( IN UidType uUserUid_, IN bool bVal_ );
	bool GetShow( IN UidType uUserUid_ );
	const bool GetDirectSummon(){ return m_bIsDirectSummon; }
	bool UICustomEventProc( IN HWND hWnd_, IN UINT uMsg_, IN WPARAM wParam_, IN LPARAM lParam_ );
	bool UIServerEventProc( IN HWND hWnd_, IN UINT uMsg_, IN WPARAM wParam_, IN LPARAM lParam_ );

	CX2RidingPetPtr CreateRidingPet( IN UidType uUserUid_, IN const CX2RidingPet::RidingPetInfo& RidingPetInfo_, IN D3DXVECTOR3 vInitPos_ );
	CX2RidingPetPtr CreateGameRidingPet( IN UidType uUserUid_, IN const CX2RidingPet::RidingPetInfo& RidingPetInfo_ );

	void AddRidingPet( IN CX2RidingPetPtr pRidingPet_ );
	RidingPetMap::iterator RemoveRidingPet( IN UidType uUserUid_ );
	void ChangeRidingPet( IN CX2RidingPetPtr pRidingPet_ );
	void RemoveAllRidingPet();
	void RemoveRidingPetList();

	CX2RidingPet* GetMyRidingPet();
	CX2RidingPet* GetRidingPet( IN UidType uUserUid_ );
	CX2RidingPet* GetRidingPetIndex( IN int iIndex_ );
	CX2RidingPet* GetRidingPetByUid( IN UidType iUid_ );
	CX2RidingPetPtr GetRidingPetPtr( IN UidType uUserUid_ );

	UINT GetRidingPetNum()
	{
		return m_mapRidingPet.size();
	}

	void InitPos();

	bool OpenScriptFile( IN const WCHAR* pFileName_ );
	void LoadInitPetData();

	bool AddRidingPetTemplet_LUA();
	CX2RidingPetManager::RidingPetTemplet* CX2RidingPetManager::GetRidingPetTemplet( IN USHORT usId_ ) { return GetRidingPetTemplet(static_cast<RIDING_PET_UNIT_ID>(usId_));}
	CX2RidingPetManager::RidingPetTemplet* CX2RidingPetManager::GetRidingPetTemplet( IN RIDING_PET_UNIT_ID eId_ );

	//#pragma region REQ, ACK, NOT
	void Handler_EGS_CREATE_RIDING_PET_REQ( const UidType _uId );
	bool Handler_EGS_CREATE_RIDING_PET_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void Handler_EGS_SUMMON_RIDING_PET_REQ( UidType petUid );
	bool Handler_EGS_SUMMON_RIDING_PET_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_SUMMON_RIDING_PET_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void Handler_EGS_UNSUMMON_RIDING_PET_REQ( bool bIsDirect_ = false, UidType uidDirectPetId_ = -1 );
	bool Handler_EGS_UNSUMMON_RIDING_PET_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_UNSUMMON_RIDING_PET_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void Handler_EGS_RELEASE_RIDING_PET_REQ( UidType _uId );
	bool Handler_EGS_RELEASE_RIDING_PET_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_RELEASE_RIDING_PET_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	//#pragma endregion

	CX2RidingPetSkillSlot* GetSkillSlot(){ return m_pSkillSlot; }

	UidType GetRidingPetId() const { return m_RidingPetUid; }
	void SetRidingPetList( const std::vector< sRidingPetInfo >& vecRidingPetList_ ) { m_vecRidingPetList = vecRidingPetList_ ; }
	void GetRidingPetList( OUT std::vector< sRidingPetInfo >& vecRidingPetList_ ) const { vecRidingPetList_ = m_vecRidingPetList; }
	bool GetHasRidingPet() const { return !m_vecRidingPetList.empty(); }

	bool GetRidingPetInfoByUID( IN UidType uiRidingPetUID, OUT sRidingPetInfo& RidingPetInfo_ );

	void GetSortedRidingPetList( OUT std::vector< sRidingPetInfo >& vecRidingPetList_ ) { SortRidingPetList(); GetRidingPetList(vecRidingPetList_); }
	UidType GetFirstRidingPetUID();
	void SortRidingPetList();
	void UpdateRidingPetGage();
	void UnsummonRidingPet();
	bool IsInvalideRiding() const;

	void SetEnableStamina_LUA( float _EnableStamina );
	const float GetEnableStamina(){ return m_fEnableStamina; }

	bool GetRidingOn();	            //���� ž�� �������� üũ(������Ŷ ACK�� ���� �Ŀ� true�� ��)
	bool GetRidingOnState();		//���� ž�� ������ �������� üũ(wait����϶��� ž���ϵ���)
	void DeleteRidingPetOnList( UidType deleteUid_ ); //����Ʈ���� �ش� Ż �� ����
	bool CheckRidingStamina( UidType RidingPetUid_ ); //���� ž�� ������ ���׹̳����� üũ

	// ������ ġƮ �Լ�
	void FullChargeRidingStamina( int iValue_ );

	bool CanPushSummonButton() { return m_ElapsedCanPushSummonButton.CheckElapsedTime(); }
	void RefreshSummonButtonElpasedTime() { m_ElapsedCanPushSummonButton.ResetSumOfElapsedTime(); }

	float GetRecoveryTimePerOne();

#if X2_DEV_TOOLS
	void RefreshRidingPetScript();
#endif // REALTIME_SCRIPT_PATCH

	bool SetRidingPetStaminaPercentUP(int iValue_ );

//#pragma endregion function

//#pragma region private
private:
	CX2RidingPetManager( void );
	virtual ~CX2RidingPetManager( void );
//#pragma endregion function

//#pragma region private
private:
	bool			m_bShow;				/// Ż ���� ����

	static CX2RidingPetManager*	m_pRidingPetManager;
	RidingPetMap		m_mapRidingPet;			/// Ż �� ��
	RidingPetTempletMap	m_mapRidingPetTemplet;	/// Ż �� ���ø� ��
	RidingPetVec		m_vecRemoveRidingPet;	/// ������ Ż �� ���

	CX2RidingPetSkillSlot* m_pSkillSlot;

	UidType		m_RidingPetUid;

	// m_iLastUnSummonDate���� ���������� �ð����� �޾ƿ�����,
	// Ŭ�󿡼� ��� �� ���� Index������ �����ؼ� �����.
	// ��ȿ ���� : 0~1
	std::vector< sRidingPetInfo >		m_vecRidingPetList;

	bool		m_bIsDirectSummon; //��ȯ�����ϰ� �ٷ� ��ȯ�ϴµ� �ʿ�.
	UidType		m_DirectSummonUid; //��ȯ�����ϰ� �ٷ� ��ȯ�� Ż �� uid
	CKTDXCheckElapsedTime m_ElapsedTimeDecrease;
	CKTDXCheckElapsedTime m_ElapsedTimeRecovery;
	CKTDXCheckElapsedTime m_ElapsedCanPushSummonButton;		/// ��ȯ ��ư�� 1�ʳ��� �ٽ� ������ ���ϰ� �ϱ� ���� ����

	float		m_fEnableStamina;	//ž�� ������ ���׹̳� �ּ�ġ
	float		m_fUpdateNowStamina;
//#pragma endregion variable

};
//#pragma endregion Ż �� ���� Ŭ����

//#pragma region RidingPetInitTemplate
struct RidingPetInitTemplate
{
	CX2RidingPet::InitData	m_init;
};
//#pragma endregion ����ü
