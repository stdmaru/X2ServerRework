#pragma once


#include <algorithm>

#include "x2/platform/xml.h"

class CX2GameOption
{
	public:
		enum OptionLevel
		{
			OL_HIGH,
			OL_MEDIUM,
			OL_LOW,
		};

		enum BlackListDenyState
		{
			BL_ON,
			BL_OFF,
			BL_ONLY_FRIEND,
		};


		enum FieldLevel
		{
			FL_HIGH,
			FL_MEDIUM,
			FL_LOW,
		};

		enum SKILL_CUTIN_SHOW_STATE //스킬컷인 출력 상태
		{
			SCSS_ONESELF = 0,	//자기자신만 보기
			SCSS_PARTY_MEMBER,	//파티원 보기
			SCSS_HIDE,			//출력하지 않기
		};

		struct OptionList
		{
			OptionLevel		m_UnitDetail;
			OptionLevel		m_TexDetail;
			OptionLevel		m_MapDetail;
			OptionLevel		m_eEffect;

			D3DXVECTOR2		m_vResolution;

			bool			m_bMusic;
			bool			m_bSound;
			float			m_fMusicVolume;
			float			m_fSoundVolume;

			bool			m_bDynamicCamera;
			bool			m_bPlayGuide;
			int				m_iZoomLevel;	// 0: default, -1:zoom out, +1:zoom in

			int				m_iAutoSetLevel;

			OptionLevel		m_MaxUnitDetail;
			OptionLevel		m_MaxTexDetail;
			OptionLevel		m_MaxMapDetail;
			bool			m_bEnable3DSound;



			FieldLevel		m_FieldDetail;
			FieldLevel		m_MaxFieldDetail;
			//bool			m_bFriend;
			bool			m_bParty;
			bool			m_bSD;				// 필드 SD보기 모드
			bool			m_bFullScreen;
			bool			m_bJoyEnable;


			// 커뮤니티 옵션
			BlackListDenyState				m_eDenyFriendship;
			BlackListDenyState				m_eDenyInviteGuild;
			BlackListDenyState				m_eDenyParty;
			BlackListDenyState				m_eDenyPersonalTrade;
			std::map< UidType, wstring >	m_mapBlackList;

			// 캐릭터 이름에 나올 정보 옵션
			bool			m_bLevel;
			bool			m_bPvpRank;
			bool			m_bGuild;
			bool			m_bNothing;

			// 프라이버스 정보
			bool			m_bRefuseParty;
			bool			m_bRefuseFriend;
			bool			m_bRefusePersonalTrade;
			//{{ kimhc // 2009-10-09 // 길드초대 거부 추가
			bool			m_bRefuseGuildInvitation;
			//}} kimhc // 2009-10-09 // 길드초대 거부 추가

			bool			m_bRefuseRequestCouple;

			bool			m_bRefuseInvitePVP;

			//{{ kimhc // 2010.3.12 //	채팅창 개편
			bool			m_bDisplayObtainingED;		// 채팅창에 ED획득을 표시
			bool			m_bDisplayObtainingEXP;		// 채팅창에 경험치 획득을 표시
			//}} kimhc // 2010.3.12 //	채팅창 개편

			std::vector<int> m_vecPvpMap;

			bool			m_bFlashEffect;		/// 화면 번쩍임 효과
			bool			m_bVerticalSync;	/// vertical sync
			int				m_iFrameLimit;		/// T41: at most this many frames per second, 0 for no limit (GameOptions.lua only)


#ifdef PVP_ZOOM_CAMERA
			bool			m_bAutoCamera;
#endif
			SKILL_CUTIN_SHOW_STATE m_eSkillCutinShowState;

			bool			m_bIsSkillUITypeA;

#if X2_DEV_TOOLS
			bool			m_bShowOnlyMyDamage;
#endif //SHOW_ONLY_MY_DAMAGE



			OptionList()
			{
				m_UnitDetail			= OL_HIGH;
				m_TexDetail				= OL_HIGH;
				m_MapDetail				= OL_HIGH;
				m_eEffect			= OL_HIGH;


				m_vResolution			= D3DXVECTOR2(1024, 768);

				m_bMusic				= true;
				m_bSound				= true;
				m_fMusicVolume			= 0.0f;
				m_fSoundVolume			= 0.0f;

				m_bDynamicCamera		= true;
				m_bPlayGuide			= true;

				m_iZoomLevel			= 0;
				m_iAutoSetLevel			= 0;

				m_MaxUnitDetail			= OL_HIGH;
				m_MaxTexDetail			= OL_HIGH;
				m_MaxMapDetail			= OL_HIGH;

				m_eDenyFriendship		= BL_OFF;
				m_eDenyInviteGuild		= BL_OFF;
				m_eDenyParty			= BL_OFF;
				m_eDenyPersonalTrade    = BL_OFF;

				m_bEnable3DSound		= true;

				m_FieldDetail			= FL_HIGH;
				m_MaxFieldDetail		= FL_HIGH;

				m_bParty				= false;
				m_bSD					= false;

				m_bLevel				= true;;
				m_bPvpRank				= true;
				m_bGuild				= true;
				m_bNothing				= true;


				m_bRefuseParty			= false;
				m_bRefuseFriend			= false;
				m_bRefusePersonalTrade	= false;

				//{{ kimhc // 2009-10-09 // 길드초대 거부 추가
				m_bRefuseGuildInvitation = false;
				//}} kimhc // 2009-10-09 // 길드초대 거부 추가

				m_bRefuseRequestCouple	= false;

				m_bRefuseInvitePVP		= false;

				m_bFullScreen			= false;

				//{{ kimhc // 2010.3.12 //	채팅창 개편
				m_bDisplayObtainingED	= true;
				m_bDisplayObtainingEXP	= true;
				//}} kimhc // 2010.3.12 //	채팅창 개편

				m_bJoyEnable = false;
				m_vecPvpMap.clear();

				m_bFlashEffect			= false;
				m_bVerticalSync			= false;
				m_iFrameLimit			= 0;

#ifdef PVP_ZOOM_CAMERA
				m_bAutoCamera				= false;
#endif
				m_eSkillCutinShowState = SCSS_ONESELF;

				m_bIsSkillUITypeA			= true;

#if X2_DEV_TOOLS
				m_bShowOnlyMyDamage			= false;
#endif //SHOW_ONLY_MY_DAMAGE


			}

		};

	public:
		CX2GameOption(void);
		virtual ~CX2GameOption(void);

		bool OpenScriptFile();
		bool SaveScriptFile();

		bool SetAutoOption( int index );
		bool UpAutoOption();
		bool DownAutoOption();
		int GetAutoSetLevel() { return m_OptionList.m_iAutoSetLevel; }

		void SetTexDetail( OptionLevel optionLevel, bool bForce = false );
		void UpTexDetail();
		void DownTexDetail();

		void SetUnitDetail( OptionLevel optionLevel, bool bForce = false );
		void UpUnitDetail();
		void DownUnitDetail();

		void SetMapDetail( OptionLevel optionLevel, bool bForce = false );
		void UpMapDetail();
		void DownMapDetail();

		void SetEffectDetail( OptionLevel optionLevel, bool bForce = false );
		void UpEffectDetail();
		void DownEffectDetail();

		void SetFieldDetail( FieldLevel optionLevel, bool bForce = false );
		void UpFieldDetail();
		void DownFieldDetail();
		FieldLevel GetFieldOptionOld() { return m_eFieldLevelOld; }

		bool GetFieldParty() { return m_OptionList.m_bParty; }
		void SetFieldParty( bool bParty );
		void SetFieldFriend( bool bFriend );

		bool GetFieldSD(){ return m_OptionList.m_bSD; }
		void SetFieldSD(bool bSD);
		void ToggleFieldSD() { SetFieldSD(!m_OptionList.m_bSD); }

		void SetDynamicCamera( bool bCamera ){ m_OptionList.m_bDynamicCamera = bCamera; }
		void SetPlayGuide( bool bGuide, bool bUpdateDB = true );

		void SetEnable3DSound( bool bEnable3DSound );
		void SetMusic( bool bMusic );
		void SetSound( bool bSound );
		void SetMusicVolume( float fMusicVolume );
		float GetMusicVoluem() { return m_OptionList.m_fMusicVolume; }
		void SetSoundVolume( float fSoundVolume );
		float GetSoundVolume() { return m_OptionList.m_fSoundVolume; }
		void SetResolution( DWORD dwWidth, DWORD dwHeight, int iColorBit = 32 );
		const D3DXVECTOR2& GetResolution() { return m_OptionList.m_vResolution; }

		bool GetIsFullScreen() const { return m_OptionList.m_bFullScreen; }




		void SetDenyFriendship(CX2GameOption::BlackListDenyState val) { m_OptionList.m_eDenyFriendship = val; }
		void SetDenyInviteGuild(CX2GameOption::BlackListDenyState val) { m_OptionList.m_eDenyInviteGuild = val; }
		void SetDenyParty(CX2GameOption::BlackListDenyState val) { m_OptionList.m_eDenyParty = val; }
		void SetDenyPersonalTrade(CX2GameOption::BlackListDenyState val) { m_OptionList.m_eDenyPersonalTrade = val; }

		void SetBlackList( std::map< UidType, wstring >& mapBlackList )
		{
			m_OptionList.m_mapBlackList = mapBlackList;
		}

		bool AddBlackList( UidType iUnitUID, wstring wstrNickName )
		{
			std::map< UidType, wstring >::iterator it;
			it = m_OptionList.m_mapBlackList.find( iUnitUID );
			if( it == m_OptionList.m_mapBlackList.end() )
			{
				m_OptionList.m_mapBlackList[ iUnitUID ] = wstrNickName;
				return true;
			}
			return false;
		}

		bool RemoveBlackList( UidType iUnitUID, wstring /*wstrNickName*/ )
		{
			if( m_OptionList.m_mapBlackList.erase( iUnitUID ) > 0 )
			{
				return true;
			}
			return false;
		}

		void CameraZoomIn( int iGap )
		{

			m_OptionList.m_iZoomLevel += iGap;

			if( m_OptionList.m_iZoomLevel < -1 )
				m_OptionList.m_iZoomLevel = -1;

			if( m_OptionList.m_iZoomLevel > 2 )
				m_OptionList.m_iZoomLevel = 2;
		}

		void SetCameraZoomIn( int iLev )
		{
			m_OptionList.m_iZoomLevel = iLev;

			if( m_OptionList.m_iZoomLevel < -1 )
				m_OptionList.m_iZoomLevel = -1;

			if( m_OptionList.m_iZoomLevel > 2 )
				m_OptionList.m_iZoomLevel = 2;
		}
//}}

		float GetCameraDistance();

		void ApplyAllOption( bool bForce = false );

//{{ robobeg : 2013-09-17
		//OptionList* GetOptionList(){ return &m_OptionList; }
        OptionList& GetOptionList(){ return m_OptionList; }
//}} robobeg : 2013-09-17
		vector<OptionList>& GetOptionListPreset() { return m_vecOptionListPreset; }

		void SendFieldOption();
		void SetChangeFieldOption(bool val) { m_bChangeFieldOption = val; }
		bool GetChangeFieldOption() { return m_bChangeFieldOption; }

		// 기타옵션 캐릭터 정보셋팅
		void SetCharInfoLevel( bool bVal )		{ m_OptionList.m_bLevel = bVal; }
		void SetCharInfoPvp( bool bVal )		{ m_OptionList.m_bPvpRank = bVal; }
		void SetCharInfoGuild( bool bVal )		{ m_OptionList.m_bGuild = bVal; }
		void SetCharInfoNothing( bool bVal )	{ m_OptionList.m_bNothing = bVal; }

		// 프라이버스 정보셋팅
		void SetRefuseParty(CX2GameOption::BlackListDenyState val);
		void SetRefuseFriend(CX2GameOption::BlackListDenyState val);
		void SetRefusePersonalTrade(CX2GameOption::BlackListDenyState val);

		void SetRefuseParty(bool val);
		void SetRefuseFriend(bool val);
		void SetRefusePersonalTrade(bool val);

		//{{ kimhc // 2009-10-09 // 길드초대 거부 추가
		void SetRefuseGuildInvitation( CX2GameOption::BlackListDenyState val );
		void SetRefuseGuildInvitation( bool val );
		//}} kimhc // 2009-10-09 // 길드초대 거부 추가

		void SetRefuseRequestCouple( CX2GameOption::BlackListDenyState val );
		void SetRefuseRequestCouple( bool val );

		void SetRefuseInvitePVP( CX2GameOption::BlackListDenyState val );
		void SetRefuseInvitePVP( bool val );

		void SetFullScreen( bool bFullScreen );

		//{{ kimhc // 2010.3.12 //	채팅창 개편
		void SetDisplayObtainingED( bool bVal ) { m_OptionList.m_bDisplayObtainingED = bVal; }
		bool GetDisplayObtainingED() const { return m_OptionList.m_bDisplayObtainingED; }
		void SetDisplayObtainingEXP( bool bVal ) { m_OptionList.m_bDisplayObtainingEXP = bVal; }
		bool GetDisplayObtainingEXP() const { return m_OptionList.m_bDisplayObtainingEXP; }
		//}} kimhc // 2010.3.12 //	채팅창 개편

		void SetJoyEnable( bool bEnable );
		//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현
		//}} JHKang // 2010.8.27 // 창 모드 전체 화면 구현

		void SetFlashEffect( IN bool bVal_ );
		bool GetFlashEffect() const { return m_OptionList.m_bFlashEffect; }
		void SetVerticalSync( IN bool bVal_ );
		bool GetVerticalSync() const { return m_OptionList.m_bVerticalSync; }
		void SetFrameLimit( IN int iFps_ );
		int GetFrameLimit() const { return m_OptionList.m_iFrameLimit; }


#ifdef PVP_ZOOM_CAMERA
		void SetAutoCamera(bool bVal) { m_OptionList.m_bAutoCamera = bVal; }
		bool GetAutoCamera() { return m_OptionList.m_bAutoCamera; }
		void ResetAutoCamera();

		void SetSubZoomCameraLv(int iLv) { m_iSubZoomCameraLv = iLv; }
		int GetSubZoomCameraLv() { return m_iSubZoomCameraLv; }
		void SetSubZoomCameraTimer( float fTime )
		{
			m_fSubZoomCameraTimer = fTime;
		}
		float GetSubZoomCameraTimer() { return m_fSubZoomCameraTimer; }
		void DoSubZoomCameraTimer( float fElapsedTime )
		{
			if( m_fSubZoomCameraTimer > 0.f )
			{
				m_fSubZoomCameraTimer -= fElapsedTime;
				if( m_fSubZoomCameraTimer < 0.f )
					m_fSubZoomCameraTimer = 0.f;
			}
		}
#endif

#ifdef ADD_MUSIC_SUB_VOLUME
		float GetMusicSubVolume() { return m_fMusicSubVolume; }
		void SetMusicSubVolume(float fVal)
		{
			m_fMusicSubVolume = fVal;
			SetMusicVolume( GetMusicVoluem() );
		}
		bool GetMusic() { return m_OptionList.m_bMusic; }
#endif

		void SetStateCutinShowState( SKILL_CUTIN_SHOW_STATE eCutinShowState_){m_OptionList.m_eSkillCutinShowState = eCutinShowState_;}
		SKILL_CUTIN_SHOW_STATE GetStateCutinShowState() const {return m_OptionList.m_eSkillCutinShowState;}
		void SetIsSkillUITypeA( bool bIsSkillUITypeA_, bool bShowBuffIcon_ = true );
		bool GetIsSkillUITypeA() const { return m_OptionList.m_bIsSkillUITypeA; }

#if X2_DEV_TOOLS
		void SetShowOnlyMyDamage( bool bShowOnlyMyDamage_ ) { m_OptionList.m_bShowOnlyMyDamage = bShowOnlyMyDamage_; }
		bool GetShowOnlyMyDamage() const { return m_OptionList.m_bShowOnlyMyDamage; }
#endif //SHOW_ONLY_MY_DAMAGE


	private:
		OptionList			m_OptionList;
		vector<OptionList>	m_vecOptionListPreset;

		FieldLevel	m_eFieldLevelOld;
		bool		m_bPartyOld;
		bool		m_bFriendOld;
		bool		m_bChangeFieldOption;

		//{{ JHKang // 2010.8.27 // 창 모드 전체 화면 구현
		//}} JHKang // 2010.8.27 // 창 모드 전체 화면 구현

#ifdef PVP_ZOOM_CAMERA
		int	m_iSubZoomCameraLv;
		float m_fSubZoomCameraTimer;

		float m_fDungeonZoom1;
		float m_fDungeonZoom2;
		float m_fDungeonZoom3;
		float m_fPvpZoom1;
		float m_fPvpZoom2;
		float m_fPvpZoom3;
#endif

#ifdef ADD_MUSIC_SUB_VOLUME
		float m_fMusicSubVolume;
#endif

	private:
		void _DecideProperGraphicOption();
		int _CalcCpuScore();
		void _CalcBestGraphicOption( int iCpuTotalScore, int iGpuTotalScore );
		OptionLevel _CalcHighMiddleLow( const x2::platform::XmlElement& kElement, int iCpuTotalScore, int iGpuTotalScore );
		bool _CalcOnOff( const x2::platform::XmlElement& kElement, int iCpuTotalScore, int iGpuTotalScore );
};
