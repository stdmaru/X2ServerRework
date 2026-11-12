#pragma once


#include "./X2BaseUnitViewerUI.h"

	class CX2UnitViewerUI : public CKTDGObject
{
	public:

		enum UNIT_VIEWER_UI_MOTION_TYPE
		{
			UVUMT_WAIT = 0,
			UVUMT_WAIT1,
			UVVMT_WAIT2,
			UVVMT_DOWN_LANDING,
			UVUMT_READY,
			UVUMT_LEVEL_UP,
//#ifdef SERV_UNIT_WAIT_DELETE
			UVUMT_WAIT_DELETE,
//#endif //SERV_UNIT_WAIT_DELETE
		};

		enum UNIT_VIEWER_STATE
		{
			UVS_NONE		= 0,
			UVS_CASHSHOP,
			UVS_ROOM,
			UVS_FIELD,
			UVS_CLASSCHANGE,
			UVS_CREATEUNIT,
			UVS_SELECTUNIT,
			UVS_CHARINFO,
			UVS_WATCH,
		};

	private:
		CX2UnitViewerUI(void);
		virtual ~CX2UnitViewerUI(void);

	public:
		static CX2UnitViewerUI* CreateUnitViewerUI() { return new CX2UnitViewerUI; }

		virtual void	NotifyShowObjectChanged();
		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );

//{{ robobeg : 2008-10-28
        virtual RENDER_HINT   OnFrameRender_Prepare();
		//virtual HRESULT OnFrameRender();
        virtual void    OnFrameRender_Draw();
//}} robobeg : 2008-10-28

//{{ kimhc // 2010.9.9 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
//}} kimhc // 2010.9.9 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

		virtual HRESULT OnResetDevice();
		virtual HRESULT OnLostDevice();

		void PlayAnim( const WCHAR* pName, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType = CKTDGXSkinAnim::XAP_LOOP, bool bTransition = false );

		void SetUnit( CX2Unit* pUnit, CX2UnitViewerUI::UNIT_VIEWER_STATE eState = UVS_NONE );

		//{{ robobeg : 2011-03-18
		static void         AppendToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pUnit, CX2UnitViewerUI::UNIT_VIEWER_STATE eState = UVS_NONE );
		//}} robobeg : 2011-03-18

		CX2Unit* GetUnit() { return m_pUnit; }

		void SetPickUnit(bool val);
		// 케릭터의 인벤토리 안에서 처리
		void UpdateEqip( bool bEconomical = false );
		void RemoveEqip( UidType itemUID, bool bInNoReplaceEmptyToBasic = false );
		void AddEqip( UidType itemUID );

		// 모든 아이템 템플릿에 관해 처리
//{{ robobeg : 2008-10-17
		//void AddEquipByTID( int itemTID );
		//void RemoveEquipByTID( int itemTID );

        void AddEquipByTID( int itemTID, bool bBasicItem = false );
//}} robobeg : 2008-10-17
		void RemoveAllEquips();

		void InitEquipVisibility();
		void UpdateEquipSlotVisibility();

		CKTDGXRenderer::RenderParam* GetRenderParam();
		CKTDGXSkinAnim* GetXSkinAnim(){ return m_pXSkinAnim.get(); }
		CKTDGXSkinAnimPtr GetXSkinAnimPtr() { return m_pXSkinAnim; }
		void SetLightPos( float x, float y, float z )
		{
			m_LightPos.x = x;
			m_LightPos.y = y;
			m_LightPos.z = z;
		}
		void SetLightPos( D3DXVECTOR3 lightPos )
		{
			m_LightPos = lightPos;
		}
		D3DXVECTOR3 GetLightPos() { return m_LightPos; }

		void SetBillBoardType( CKTDGMatrix::BILLBOARD_TYPE billBoardType ){ m_BillBoardType = billBoardType; }

		void ReplaceEmptyToBasic();

		static void SetShowUnitViewerUI( bool bShow ) { m_sbShow = bShow; }

		void PlayByMotionType( CX2UnitViewerUI::UNIT_VIEWER_UI_MOTION_TYPE motionType );

		static bool GetUnitClicked() { return m_sbUnitClicked; }
		static void SetUnitClicked( bool bCheck ) { m_sbUnitClicked = bCheck; }

		int GetEnchantExtraDamageType();

//#ifdef TITLE_SYSTEM
        void UpdateTitle();
//#endif

		void SetShowAttribEnchant( bool bShow );

//{{ robobeg : 2008-10-17



// X2UnitVIewerUI 가 KTDGManager 에 등록, 삭제될 때 장착 부품들도 같이 등록, 삭제하기 위해 사용됨
        virtual void    NotifyDGManagerChanged( CKTDGManager& refInManager_ );
//}} robobeg : 2008-10-17

		void SetPositionOnScr(float x, float y, float z = 0.8f, float scale = 1.f );
		void SetFixed(bool val);
		bool GetFixed() { return m_bFixed; }


		bool    IsFashionItemEquippedAt( CX2Unit::EQIP_POSITION eqipPos )
		{
			ASSERT( eqipPos >= CX2Unit::EQIP_POSITION( 0 ) && eqipPos < CX2Unit::EP_END );
			return m_ViewEquipFashion[ eqipPos ] != 0;
		}

		void EmotionFrameMove();
		bool PlayEmotion(CX2Unit::EMOTION_TYPE eEmotionId, wstring &wstrEmotionName);
		bool GetPlayerEmotion();
		CX2Unit::EMOTION_TYPE GetNowEmotionId() { return m_ePlayedEmotion; }
		void SetNowEmotionId(CX2Unit::EMOTION_TYPE eId) { m_ePlayedEmotion = eId; }

		float GetScale() { return m_fScale; }

		void SetHideSelectUnitWeapon();		/// 케릭터 선택 화면등에서 꺼내든 무기 숨기는 함수
		void SetShowObjectMeshPlayer(CKTDGXMeshPlayer::CXMeshInstanceHandle MeshInstHandle_, bool bShow_);
		void SetClassChange( IN bool bValue_ ) { m_bChangeClass = bValue_; }


		void SetShowInvenEquip( bool bVal_ ) { m_bShowInvenEquip = bVal_; }

		bool IsPlayAvatarEmotionSoundWithouEmotion ( CX2Unit::EMOTION_TYPE eEmotionId );
	private:

		void ReplaceEmptyToBasic( CX2Unit::EQIP_POSITION eqipPos, int itemID );
//{{ robobeg : 2008-10-17
		//void CheckEmptyEqipPosition();
//}} robobeg : 2008-10-17



		void CreateEnchantedWeaponParticleAtWeapon( const WCHAR* pName1, const WCHAR* pName2 = NULL, const WCHAR* pName3 = NULL );
		void CreateEnchantedWeaponParticleAtHand( const WCHAR* pName1, const WCHAR* pName2 = NULL, const WCHAR* pName3 = NULL, const WCHAR* pName4 = NULL );
		void ProcessEnchantedWeaponEffectAtHand( CX2Unit::UNIT_TYPE unitType );
		void InitEnchantedWeaponEffectAtHand();




		void InitEnchantWeaponEffectForSword();
		void InitEnchantWeaponEffectForBow();

		void DeleteEnchantWeaponEffect();

		void ProcessEnchantWeaponEffectForSword();
		void ProcessEnchantWeaponEffectForBow();
		void ProcessEnchantWeaponEffectForEve();
		void ProcessEnchantWeaponEffectForAra();
		void ProcessEnchantWeaponEffectForAdd();

		void InitWeaponTrace();

//{{ robobeg : 2008-10-17
// 현재의 EQUIP_POSITION 의 부품 착용 여부를 알려주는 함수
        bool    IsItemEquippedAt( CX2Unit::EQIP_POSITION eqipPos )
        {
            ASSERT( eqipPos >= CX2Unit::EQIP_POSITION( 0 ) && eqipPos < CX2Unit::EP_END );
            return m_ViewEquipNormal[ eqipPos ] != 0 || m_ViewEquipFashion[ eqipPos ] != 0;
        }//IsItemEquippedAt()
//}} robobeg : 2008-10-17

		void SetUnitReForm();

		//{{ kimhc // 2010-01-05 // PC방 프리미엄 서비스
		void RemovePCBangEquips();
		//}} kimhc // 2010-01-05 // PC방 프리미엄 서비스

//{{ kimhc // 2010.12.21 // 2010-12-23 New Character CHUNG
		void GetWeaponTraceStartAndEndName( OUT wstring* pwstrTraceStartName_, OUT wstring* pwstrTraceEndName_ );
//}} kimhc // 2010.12.21 //  2010-12-23 New Character CHUNG

		bool CanNotShowEnchantedWeaponEffectToRiding();

		CX2Unit*				m_pUnit;
		CKTDXDeviceXSkinMesh*	m_pMotion;
        /// 내부적으로 관리하는 객체이므로 smart pointer를 사용하지 않는다.
        /// - jintaeks on 2009-01-12, 16:12
		// dmlee, 2009-03-12 백그라운드 로딩시에 dangling pointer 참조 오류 생기는 문제 때문에 smart poiner로 바꿨습니다.
		CKTDGXSkinAnimPtr		m_pXSkinAnim;
		D3DXVECTOR3				m_LightPos;

//{{ robobeg : 2008-10-17
		//vector<CX2Eqip*>		m_ViewEqipList;
		//bool					m_ViewEqipPos[CX2Unit::EP_END];
		//bool					m_ViewEquipedFashion[CX2Unit::EP_END];

        typedef std::map<UidType,CX2EqipPtr>    ViewEquipMap;

        ViewEquipMap            m_ViewEquipMap;								// ED템, 패션템 모두 들어있음
        UidType                 m_ViewEquipNormal[CX2Unit::EP_END];
        UidType                 m_ViewEquipFashion[CX2Unit::EP_END];
		bool					m_bViewEquipVisible[ CX2Unit::EP_END ];
//}} robobeg : 2008-10-17

		CKTDGMatrix::BILLBOARD_TYPE m_BillBoardType;

		static bool										m_sbShow;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFire;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqWeapon_Fire;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqWeapon_Fire2;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqWeapon_Fire3;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqWeapon_Wind;		// 속성 인챈트된 무기를 들었을 때 손에 붙는 이펙트
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqWeapon_Wind2;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqWeapon_Wind3;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqWeapon_Wind4;

		// 파이로 나이트 불
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFireSPK1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFireSPK2;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqEnergyBullet1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqEnergyBullet2;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqEnergyBullet3;

		static bool										m_sbUnitClicked;

		int												m_ExtraDamageType;

		// 속성 인챈트 이펙트를 표시하기 위해서 사용. trace에는 사용 안함
		CKTDXDeviceXSkinMesh::MultiAnimFrame*			m_pFrame_TRACE_START[ CX2Item::MAX_MODEL_COUNT_A_ITEM ];
		CKTDXDeviceXSkinMesh::MultiAnimFrame*			m_pFrame_TRACE_END[ CX2Item::MAX_MODEL_COUNT_A_ITEM ];

		CKTDXDeviceXSkinMesh::MultiAnimFrame*			m_pFrame_WEAPON_BONE[ CX2Item::MAX_MODEL_COUNT_A_ITEM ];

//#ifdef TITLE_SYSTEM
        CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqEmblem;
        CKTDGParticleSystem::CParticleHandle				m_hPart_Emblem_200;

        int                                             m_iTitleId;
//#endif
		bool		m_bFixed;

		bool		m_bPickUnit;

		CKTDXDeviceSound*		m_pSound_Eve_LevelUp;

		CX2UnitViewerUI::UNIT_VIEWER_UI_MOTION_TYPE		m_eOldMotionType;
		CX2Unit::EMOTION_TYPE							m_ePlayedEmotion;


		bool m_bEnableEquipSlotVisibilityUpdate;

		bool m_bMixedEmotion[AVATAR_EMOTION_NUM];
		CKTDXDeviceSound*				m_pAvatarEmotionSound;

		CKTDXTimer m_TimerResetWeaponTraceFrame;
		float		m_fScale;

		bool		m_bShowGun;
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hChungMiniGunL;	/// 청 미니건 왼쪽
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hChungMiniGunR;	/// 청 미니건 오른쪽

		bool		m_bShowSword;
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hElswordSword;

		bool												m_bShowLTrapSword;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hInstLTrapSwordReady;

		bool												m_bShowRNWTrapSword;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hInstRNWTrapSwordReady;
		bool												m_bChangeClass;


		bool												m_bShowInvenEquip;


		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFireSBH1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFireSBH2;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFireSBH3;
		UNIT_VIEWER_STATE									m_eUnitViewerState;

		bool												m_bIsPlayAvatarEmotionSoundWithoutEmotion;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandElectricALP1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandElectricALP2;

};
