#pragma once

class CX2User
{
	public:

		enum X2_USER_AUTH_LEVEL
		{
			XUAL_NORMAL_USER = 0,
			XUAL_SPECIAL_USER,
			XUAL_OPERATOR,
			XUAL_DEV,
//#ifdef LIGHT_OPERATOR_ACCOUNT
			XUAL_LIGHT_OPERATOR = 10,
//#endif LIGHT_OPERATOR_ACCOUNT
		};

		enum HACKING_USER_TYPE
		{
			HUT_NORMAL = 0,
			HUT_DISAGREE_HACK_USER,
			HUT_AGREE_HACK_USER,

			HUT_TOTAL_NUM,
		};

	public:
		struct UserData
		{
			UidType				userUID;			//유저의 유니크 아이디
			wstring				userID;				//아이디

			bool				m_bMan;				// 성별
			UCHAR				m_ucAge;			// 나이

			int					maxUnitCount;		//내가 만들 수 있는 캐릭터 수(가지고 있는것 포함)

			X2_USER_AUTH_LEVEL	userAuthLevel;
			bool				m_bIsPCRoom;
			bool				m_bIsGuestUser;

			wstring		IP;
			int			port;

			HACKING_USER_TYPE	hackingUserType;

			UINT8				m_uChannelCode;		// 0 넥슨, 3 투니랜드
			wstring				m_wstrTooniLandID;	// 투니랜드 아이디

			UserData& operator=( const KEGS_VERIFY_ACCOUNT_ACK& data )
			{

				userUID			= data.m_kAccountInfo.m_nUserUID;				//유저의 유니크 아이디
				userID			= data.m_kAccountInfo.m_wstrID;					//아이디

				m_bMan			= data.m_bSex;
				m_ucAge			= static_cast<UCHAR>( data.m_uiAge );

				maxUnitCount	= 0;							//내가 만들 수 있는 캐릭터 수(가지고 있는것 포함)

				userAuthLevel	= (X2_USER_AUTH_LEVEL)data.m_kAccountInfo.m_iAuthLevel;
                // m_bIsPCRoom
				m_bIsGuestUser	= data.m_kAccountInfo.m_bIsGuestUser;
                //IP
                //port
				hackingUserType = (CX2User::HACKING_USER_TYPE)data.m_cHackingUserType;

				m_uChannelCode		= data.m_uChannelCode;
				m_wstrTooniLandID	= data.m_wstrChannelUserID;	// 투니랜드 아이디
				return *this;
			}

			UserData() :
			    userUID( 0 ), userID()
                , m_bMan( true ), m_ucAge( 0 ), maxUnitCount( 0 )
                , userAuthLevel( XUAL_NORMAL_USER ), m_bIsPCRoom( false ), m_bIsGuestUser( false )
				, IP()
				, port( 0 ), hackingUserType( CX2User::HUT_NORMAL )
				, m_uChannelCode( -1 )		/// CE_NONE
				, m_wstrTooniLandID()
			{}
		};

	public:
		CX2User( const UserData& kUserData );
		~CX2User(void);
		bool		SortFirstByUID(UidType aUnitUID);

		bool		AddUnit( CX2Unit* pUnit );
		int			GetUnitNum() const { return (int)m_UnitList.size(); }
		CX2Unit*	GetUnitByUID( UidType unitUID );
		CX2Unit*	GetUnitByIndex( int index );
		CX2Unit*	GetUnitByNickName( const WCHAR* pNickName );

		bool        IsValidUID() const { return m_UserData.userUID != 0; }
		UidType		GetUID(){ return m_UserData.userUID; }
		void		SelectUnit( CX2Unit* pUnit ){ m_pSelectUnit = pUnit; }
		CX2Unit*	GetSelectUnit(){ return m_pSelectUnit; }

		bool		DeleteUnitByUID( UidType unitUID );
		void		DeleteAllUnit();

//{{ robobeg : 2013-11-01
		//UserData*	GetUserData() const { return m_pUserData; }
        const UserData&	GetUserData() const { return m_UserData; }
        UserData&	AccessUserData() { return m_UserData; }
//}} robobeg : 2013-11-01

		bool		IsMyUnitByUnitUID( UidType unitUid );

		void		SetPCRoom( bool bPCRoom )
		{
            m_UserData.m_bIsPCRoom = bPCRoom;
		}
		bool		GetIsPCRoom()
		{
            return m_UserData.m_bIsPCRoom;
		}

		X2_USER_AUTH_LEVEL GetAuthLevel() { return m_UserData.userAuthLevel; }

		void				SetGlobalCash(KGlobalCashInfo globalCashInfo);

		void				SetGlobalTypeCash(int iCashType, int ulCash);
		unsigned long		GetGlobalCash(int iCashType);
		unsigned long		GetGlobalCashForRescurrection();
		unsigned long		GetGlobalMainCash();
		unsigned long		GetGlobalTotalCash(){ return m_ulGlobalTotalCash; };

		void				SetUserSex( bool bMan ) { m_UserData.m_bMan = bMan; }
		bool				IsMan() const { return m_UserData.m_bMan; }

		void				SetUserAge( UCHAR ucAge ) { m_UserData.m_ucAge = ucAge; }
		UCHAR				GetUserAge() const { return m_UserData.m_ucAge; }

		/// 2014-01-17 // itexpertkim // 김현철 // 네이버 채널링 이슈로 다시 사용





		void				ChangeEquippedSkillState( const int iSkillSlotId_, const int iSkillId_ );

		void				ChangeHideSetDesc() { m_bHideSetDesc = !m_bHideSetDesc; }
		bool				GetHideSetDesc() const { return m_bHideSetDesc; }

		void				SetSharingBankOpen(bool bOpen) { m_bSharingBankOpen = bOpen; }
		bool				IsSharingBankOpen() const { return m_bSharingBankOpen; }

		int					GetIndexByUID( UidType unitUid );

	private:


		//UserData*			m_pUserData;
        UserData			m_UserData;

		unsigned long		m_Cash;
		KGlobalCashInfo		m_GlobalCashInfo;	// 구조체 캐시 Value 생성자에서 초기화 됨
		unsigned long		m_ulGlobalTotalCash;

		CX2Unit*			m_pSelectUnit;
		vector<CX2Unit*>	m_UnitList;

		/// 2014-01-17 // itexpertkim // 김현철 // 네이버 채널링 이슈로 다시 사용





		bool				m_bHideSetDesc;

		bool				m_bSharingBankOpen;
};
