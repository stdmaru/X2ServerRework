#pragma once

// dmlee 2008.11.29
// 하나의 아이템에 2개의 메시를 붙일 수 있게 코드 수정
// 공용아이템의 경우 하나의 메시만 붙일 수 있는 것으로 코드 유지, 나중에 수정 해야함




#define MAX_SPECIAL_ABILITY_NUM     3
#define MAX_ENCHANT_CARD_NUM    10



class CX2Item
{
public:

#include    "X2Item_Preprocessing.inl"

		enum SPECIAL_SKILL_TYPE
		{
			SST_WOODEN_SPEAR = 1,
			SST_LIGHT_SPEAR,
			SST_FIRE_SPHERE,
			SST_WATER_SPHERE,
			SST_NATURE_SPHERE,
			SST_WIND_SPHERE,
			SST_LIGHT_SPHERE,
			SST_DARK_SPHERE,

			SST_CHILDRENSDAY_SHPERE_01,	/// 분노의 어린이탄
			SST_CHILDRENSDAY_SHPERE_02,	/// 냉정한 어린이탄
			SST_CHILDRENSDAY_SHPERE_03,	/// 사랑의 어린이탄
			SST_CHILDRENSDAY_SHPERE_04,	/// 행복한 어린이탄

			SST_ANGER_SPHERE,		// 울분의 수정구
//#ifdef EVENT_CHINA_THROW_ITEM
			SST_CHINA_EVENT_SPEAR = 1010,
//#endif //EVENT_CHINA_THROW_ITEM
//#ifdef EVENT_ICICLE_THROW_ITEM
			SST_ICICLE_SPEAR = 1011,
//#endif EVENT_ICICLE_THROW_ITEM
			SST_LOVE_LV1,
			SST_LOVE_LV2,
//#ifdef EVENT_FIRE_CRACKER_THROW_ITEM
			SST_FIRE_CRACKER = 1021,
			SST_GOOD_FIRE_CRACKER = 1022,
//#endif //EVENT_FIRE_CRACKER_THROW_ITEM
		};

        enum SPECIAL_SKILL_MOTION_TYPE
        {
            SSMT_THROW = 1,        //던지는 형식
            SSMT_INSTALL,        //설치하는 형식
            SSMT_DIRECT_USE,    //바로 사용하는 형식( 바람 구슬 )
        };

        enum SUPPLEMENT_ITEM_TYPE
        {
            SIT_PLUS_SUCCESS_RATE = 0,
            SIT_NOT_BROKEN,
        };



        struct ItemData : public CX2ItemData_Base
        {

            UidType				m_ItemUID;           //UID
            int					m_ItemID;

            PERIOD_TYPE			m_PeriodType;
            int					m_Endurance;         //내구도
            int					m_Quantity;          //수량

            int					m_EnchantLevel;

            CX2EnchantItem::ItemEnchantedAttribute    m_EnchantedAttribute;

            int					m_Period;            //몇일 짜리 기간제 아이템인가?
            std::wstring		m_wstrExpirationDate;//날짜

            //{{ kimhc // 2009-08-20 // 아이템 봉인
            bool				m_bIsSealed;         // 봉인된 상태인가?
            BYTE				m_ucTimesToBeSealed; // 봉인된 횟수
            //}} kimhc // 2009-08-20 // 아이템 봉인

			vector<int>			m_SocketOption;
			std::vector< int >	m_vecRandomSocket;	 // 기본 미감정 상태의 랜덤 소켓
			bool				m_bIsEvaluation;		 // 아이템 감정 여부

			BYTE				m_byAddedSocketSlot;	// 추가 소켓 슬롯 수

        private:

            void    Init()
            {
                m_ItemUID                = 0;                //UID
                m_ItemID                = 0;

                m_PeriodType            = PT_INFINITY;
                m_Endurance                = -1;                //내구도
                m_Quantity                = 0;                //수량

                m_EnchantLevel            = 0;
                m_EnchantedAttribute.Init();

                //m_EnchantedAttribute.m_aEnchantedType[0]         = 0;
                //m_EnchantedAttribute.m_aEnchantedType[1]         = 0;
                //m_EnchantedAttribute.m_aEnchantedType[2]         = 0;

                m_Period                = 0;
                m_wstrExpirationDate    = L"";

                //{{ kimhc // 2009-08-20 // 아이템 봉인
                m_bIsSealed                = false;                    // 봉인된 상태인가?
                m_ucTimesToBeSealed        = 0;                        // 봉인된 횟수

                m_SocketOption.resize(0);
                //}} kimhc // 2009-08-20 // 아이템 봉인
                m_vecRandomSocket.resize( 0 );
				m_bIsEvaluation	= true;	// 아이템 감정 여부
				m_byAddedSocketSlot = 0;	// 추가 소켓 슬롯 수
            }

            void    Init( const KInventoryItemInfo& data )
            {
                m_ItemUID               = data.m_iItemUID;                    //UID
                m_ItemID                = data.m_kItemInfo.m_iItemID;

                m_PeriodType            = (CX2Item::PERIOD_TYPE) data.m_kItemInfo.m_cUsageType;
                m_Endurance             = static_cast<int>(data.m_kItemInfo.m_sEndurance);                //내구도
                m_Quantity              = data.m_kItemInfo.m_iQuantity;                    //수량

                m_EnchantLevel          = (int)data.m_kItemInfo.m_cEnchantLevel;
                m_EnchantedAttribute.m_aEnchantedType[0] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant0;
                m_EnchantedAttribute.m_aEnchantedType[1] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant1;
                m_EnchantedAttribute.m_aEnchantedType[2] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant2;

                m_Period                = (int)data.m_kItemInfo.m_sPeriod;
                m_wstrExpirationDate    = data.m_kItemInfo.m_wstrExpirationDate;

                //{{ kimhc // 2009-08-26 // 아이템 봉인
                m_bIsSealed                = data.m_kItemInfo.IsSealedItem();    // 봉인된 상태인가?
                m_ucTimesToBeSealed        = data.m_kItemInfo.GetSealCount();    // 봉인된 횟수
                //}} kimhc // 2009-08-26 // 아이템 봉인

                m_SocketOption          = data.m_kItemInfo.m_vecItemSocket;
				m_vecRandomSocket		= data.m_kItemInfo.m_vecRandomSocket;
				// m_cItemState가 미감정 상태가 아니라면 감정 된 것으로 설정
				m_bIsEvaluation			= (KItemInfo::IS_NOT_EVALUATED != data.m_kItemInfo.m_cItemState);

				m_byAddedSocketSlot = data.m_kItemInfo.m_byteExpandedSocketNum;	// 추가 소켓 슬롯 수
            }

            void    Init( const KItemInfo& data )
            {
                m_ItemUID               = -1;                    //UID
                m_ItemID                = data.m_iItemID;

                m_PeriodType            = (CX2Item::PERIOD_TYPE) data.m_cUsageType;
                m_Endurance             = static_cast<int>(data.m_sEndurance);                //내구도
                m_Quantity              = data.m_iQuantity;                    //수량

                m_EnchantLevel          = (int)data.m_cEnchantLevel;
                m_EnchantedAttribute.m_aEnchantedType[0] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant0;
                m_EnchantedAttribute.m_aEnchantedType[1] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant1;
                m_EnchantedAttribute.m_aEnchantedType[2] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant2;

                m_Period                = (int)data.m_sPeriod;
                m_wstrExpirationDate    = data.m_wstrExpirationDate;


                //{{ kimhc // 2009-08-26 // 아이템 봉인
                m_bIsSealed                = data.IsSealedItem();            // 봉인된 상태인가?
                m_ucTimesToBeSealed        = data.GetSealCount();            // 봉인된 횟수
                //}} kimhc // 2009-08-26 // 아이템 봉인

				m_SocketOption          = data.m_vecItemSocket;
				m_vecRandomSocket		= data.m_vecRandomSocket;
				// m_cItemState가 미감정 상태가 아니라면 감정 된 것으로 설정
				m_bIsEvaluation			= (KItemInfo::IS_NOT_EVALUATED != data.m_cItemState);

				m_byAddedSocketSlot = data.m_byteExpandedSocketNum;	// 추가 소켓 슬롯 수
            }

            void    Init( const KPostItemInfo& data )
            {
                m_ItemUID               = -1;                    //UID
                m_ItemID                = data.m_iScriptIndex;

                m_PeriodType            = PT_INFINITY;
                m_Endurance                = -1;                //내구도
                m_Quantity              = data.m_iQuantity;                    //수량

                m_EnchantLevel          = (int)data.m_cEnchantLevel;
                m_EnchantedAttribute.m_aEnchantedType[0] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant0;
                m_EnchantedAttribute.m_aEnchantedType[1] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant1;
                m_EnchantedAttribute.m_aEnchantedType[2] = (CX2EnchantItem::ENCHANT_TYPE) data.m_kAttribEnchantInfo.m_cAttribEnchant2;

                m_Period                = 0;
                m_wstrExpirationDate.resize( 0 );

                //{{ kimhc // 2009-08-26 // 아이템 봉인
                m_bIsSealed                = data.IsSealedItem();            // 봉인된 상태인가?
                m_ucTimesToBeSealed        = data.GetSealCount();            // 봉인된 횟수
                //}} kimhc // 2009-08-26 // 아이템 봉인

				m_SocketOption          = data.m_vecItemSocket;
				m_vecRandomSocket		= data.m_vecRandomSocket;
				// m_cItemState가 미감정 상태가 아니라면 감정 된 것으로 설정
				m_bIsEvaluation			= (KItemInfo::IS_NOT_EVALUATED != data.m_cItemState);

				m_byAddedSocketSlot = data.m_byteExpandedSocketNum;	// 추가 소켓 슬롯 수
            }

        public:

            ItemData()
            {
                Init();
            }

            ItemData( const KInventoryItemInfo& data )
            {
                Init( data );
            }

            ItemData( const KItemInfo& data )
            {
                Init( data );
            }
//{{ robobeg : 2013-11-04
            bool    Initialize( const KPostItemInfo& data );
//}} robobeg : 2013-11-04

            ItemData& operator=( const KInventoryItemInfo& data )
            {
                m_ItemUID               = data.m_iItemUID;                    //UID
                m_ItemID                = data.m_kItemInfo.m_iItemID;

                m_PeriodType            = static_cast<CX2Item::PERIOD_TYPE>( data.m_kItemInfo.m_cUsageType );
                m_Endurance             = static_cast<int>(data.m_kItemInfo.m_sEndurance);                //내구도
                m_Quantity              = data.m_kItemInfo.m_iQuantity;                    //수량

                m_EnchantLevel          = static_cast<int>( data.m_kItemInfo.m_cEnchantLevel );
				m_SocketOption          = data.m_kItemInfo.m_vecItemSocket;
				m_vecRandomSocket		= data.m_kItemInfo.m_vecRandomSocket;
				// m_cItemState가 미감정 상태가 아니라면 감정 된 것으로 설정
				m_bIsEvaluation			= (KItemInfo::IS_NOT_EVALUATED != data.m_kItemInfo.m_cItemState);

                m_Period                = static_cast<int>( data.m_kItemInfo.m_sPeriod );
                m_wstrExpirationDate    = data.m_kItemInfo.m_wstrExpirationDate;

                m_EnchantedAttribute.m_aEnchantedType[0] = static_cast<CX2EnchantItem::ENCHANT_TYPE>( data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant0 );
                m_EnchantedAttribute.m_aEnchantedType[1] = static_cast<CX2EnchantItem::ENCHANT_TYPE>( data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant1 );
                m_EnchantedAttribute.m_aEnchantedType[2] = static_cast<CX2EnchantItem::ENCHANT_TYPE>( data.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant2 );

                //{{ kimhc // 2009-08-26 // 아이템 봉인
                m_bIsSealed                = data.m_kItemInfo.IsSealedItem();    // 봉인된 상태인가?
                m_ucTimesToBeSealed        = data.m_kItemInfo.GetSealCount();    // 봉인된 횟수
                //}} kimhc // 2009-08-26 // 아이템 봉인

				m_byAddedSocketSlot = data.m_kItemInfo.m_byteExpandedSocketNum;	// 추가 소켓 슬롯 수
                return *this;
            }



            int GetAttribEnchantedCount() const
            {
                int iCount = 0;

                for( int i = 0; i<std::ssize( m_EnchantedAttribute.m_aEnchantedType ) ; i++ )
                {
                    if( m_EnchantedAttribute.m_aEnchantedType[i] > 0 )
                    {
                        ++iCount;
                    }
                }

                return iCount;
            }

        };

    public:
        CX2Item( int itemID );
//{{ robobeg : 2013-11-04
        //CX2Item( ItemData* pItemData, CX2Unit* pOwnerUnit );
        CX2Item( const ItemData& kItemData, CX2Unit* pOwnerUnit );
//}} robobeg : 2013-11-04
        ~CX2Item(void);

//{{ robobeg : 2013-11-04
        //ItemData*        GetItemData()            { return m_pItemData; }
        const ItemData&    GetItemData() const      { return m_kItemData; }
        ItemData&          AccessItemData()         { return m_kItemData; }
//}} robobeg : 2013-11-04
        const ItemTemplet*  GetItemTemplet() const    { return m_pItemTemplet; }
		int					GetItemEnchantLevel() const { return m_kItemData.m_EnchantLevel; }

        UidType            GetUID() const            { return m_kItemData.m_ItemUID; }
        void            SetEqip( bool bEqip )    { m_bEqip = bEqip; }
        bool            GetEqip()                { return m_bEqip; }

        CX2Unit*        GetOwnerUnit()            { return m_pOwnerUnit; }

        //float            GetRepairPriceDiscountRate();
        int                GetEDToRepair();
        int                GetVPToRepair();

        int                GetEDToSell();                //아이템 판매가

        int                GetEDToSocketPush();        //박을 때 수수료
        int                GetEDToSocketRemove();

        int                GetSocketOptionNum() const;        //적용된 옵션 갯수 ( 박힌 마법석 갯수 )

        bool            IsDisabled();


        CX2Stat::Stat   GetStat( bool bIncludeSocketOption = false );
        bool            GetEnchantStat( OUT CX2Stat::Stat& stat_ ) const;
		int             GetIEchantedItemLevel() const;    /// 강화와 관련하여 증가된 아이템 레벨
		CX2Stat::Stat   GetStat( IN CX2Unit::UNIT_TYPE eUnitType_, IN CX2Unit::UNIT_CLASS eUnitClass_ );
		bool            GetEnchantStat( OUT CX2Stat::Stat& stat_, IN CX2Unit::UNIT_TYPE eUnitType_, IN CX2Unit::UNIT_CLASS eUnitClass_ ) const;
		int             GetIEchantedItemLevel(IN CX2Unit::UNIT_TYPE eUnitType_, IN CX2Unit::UNIT_CLASS eUnitClass_) const;    /// 강화와 관련하여 증가된 아이템 레벨

		// 감정 여부 얻기. 기본값은 true
        bool			GetIsEvaluation() const { return m_kItemData.m_bIsEvaluation; }

        //+0 불패칸 검
		wstring            GetFullName( IN const bool bAddRandomSocketName_ = false );

		BYTE			GetbyAddedSocketSlot() const;

        //static ItemData* CreateItemData( const KPostItemInfo& kPostItemInfo );

    public:
        static const int        WEAPON_LEVEL_WEIGHT = 4;            /// 아이템 평균 레벨을 구할 때 사용하는 무기 아이템의 레벨 가중치
        static const int        AVERAGE_ITEM_LEVEL_DIVISOR = 8;        /// 아이템 평균 레벨을 구할 때 사용하는 제수

    private:
        static const int        STANDARD_VALUE_PHYSIC_ATTACK = 120;    /// 아이템 1레벨 물공 증가치 (이만큼이 증가해야 1레벨이 오름)
        static const int        STANDARD_VALUE_MAGIC_ATTACK = 120;    /// 아이템 1레벨 마공 증가치    (이만큼이 증가해야 1레벨이 오름)
        static const int        STANDARD_VALUE_PHYSIC_DEFENCE = 30;/// 아이템 1레벨 물방 증가치    (이만큼이 증가해야 1레벨이 오름)
        static const int        STANDARD_VALUE_MAGIC_DEFENCE = 30;    /// 아이템 1레벨 마방 증가치    (이만큼이 증가해야 1레벨이 오름)
        static const int        STANDARD_VALUE_BASE_HP = 3000;        /// 아이템 1레벨 HP 증가치    (이만큼이 증가해야 1레벨이 오름)

        //ItemData*        m_pItemData;
        ItemData        m_kItemData;
        const ItemTemplet*    m_pItemTemplet;
        CX2Unit*        m_pOwnerUnit;

        bool            m_bEqip;
};
