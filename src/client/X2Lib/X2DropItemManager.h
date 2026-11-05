#pragma once

class CX2DropItemManager : public CKTDXStage
{
	public:
		struct DropItemData
		{
			KProtectedType<int>		m_ItemID;
			KProtectedType<UidType>	m_ItemUID;
			D3DXVECTOR3				m_Pos;
			KProtectedType<bool>	m_bGet;
			KProtectedType<bool>	m_bGetTry;
			bool					m_bLeft;
			float					m_fNowSpeed;
			float					m_fScale;
			float					m_fCoolTime;
			float					m_fRemainTime;

			const CX2Item::ItemTemplet*	m_pItemTemplet;
			CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeq;
            CKTDGParticleSystem::CParticleHandle				m_hParticle;

			char					m_cEnchantLevel;

			DropItemData()
			{
				m_ItemID		= -1;
				m_ItemUID		= -1;
				m_Pos			= D3DXVECTOR3(0.0f,0.0f,0.0f);
				m_bGet			= false;
				m_bGetTry		= false;
				m_bLeft			= true;
				m_fNowSpeed		= 0.0f;
				m_fScale		= 1.0f;
				m_fCoolTime		= 1.0f;
				m_fRemainTime	= 20.0f;

				m_pItemTemplet	= NULL;
				m_hSeq			= INVALID_PARTICLE_SEQUENCE_HANDLE;
                m_hParticle		= INVALID_PARTICLE_HANDLE;

				m_cEnchantLevel	= 0;
			}
			~DropItemData();

			void Verify();
		};

	public:
		CX2DropItemManager(void);
		virtual ~CX2DropItemManager(void);


		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		void			RenderName();

		void			AddDropItem_LUA( int iItemID, D3DXVECTOR3 pos, float fSpeed, float fRemainTime );

		void			AddDropItem( int iItemID, UidType ItemUID, const D3DXVECTOR3 &pos, bool bLeft, float fSpeed, float fRemainTime = 20.f, char cEnchantLevel = 0 );

		UidType			TryGetDropItem( D3DXVECTOR3 pos );
		bool			GetDropItem( IN const UidType itemUID_, IN const UidType unitUID_,
									OUT int& iItemID_, OUT const CX2Item::ItemTemplet*& pItemTemplet_ );
		int				GetDropItemID( UidType itemUID );

		void			DeleteAllItem();
		int				CountDropItemByID( int iItemID );

		bool			GetDropItemPosition( int iIndex, D3DXVECTOR3& vPos );
        const D3DXVECTOR3*  GetDropItemPosition_LUA( int iIndex );

		int				GetDropItemCount() { return m_DropItemDataList.size(); }
		bool			GetDropItemPositionByUid( int itemUID, D3DXVECTOR3& vPos );
		bool			ChangeDropItemPositionByUid(int itemUID, D3DXVECTOR3 vPos );
		void			AddDropItem( int iItemID, UidType ItemUID, D3DXVECTOR3 pos, float fCoolTime = 0.f, float fRemainTime = 20.f );

		void			SetSummonItemPickupPet(bool bVal){ m_bIsSummonItemPickupPet = bVal; }
		bool			GetIsSummonItemPickupPet() const { return m_bIsSummonItemPickupPet ; }
	private:
		float						m_fElapsedTime;
		vector<DropItemData*>		m_DropItemDataList;

		CKTDXDeviceSound*			m_apSoundGetItem[6];

		float						m_fGetDist;
		static const float			ms_MaximumDistanceSquareToPickUp;

		bool						m_bIsSummonItemPickupPet; // 아이템 먹는 펫 소환 여부
};
