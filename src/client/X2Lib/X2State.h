#pragma once
#include "KTDXStage.h"
#include "X2Cursor.h"


class CX2StateAutoChanger;
class CX2State : public CKTDXStage
{
	public:
		enum STATE_UI_CUSTOM_MSG
		{
			SUCM_EXIT = 3000,
			SUCM_GAME_EDIT_COMMAND,
			SUCM_EXIT_WINDOW,

			SUCM_LAST_MSG_BY_ESC_OK,
			SUCM_LAST_MSG_BY_ESC_CANCEL,

			SUCM_EVENT_CLOCK_MOUSE_OVER,
			SUCM_EVENT_CLOCK_MOUSE_OUT,

			SUCM_GUEST_USER_GOTO_UNIT_SELECT,

			SUCM_ESC_PUSH,	//이거 바꿀려면 DXUT쪽에 SC_CLOSE 쪽도 같이 바꿔줘야함.
			SUCM_USER_INFO_OK,

			//{{ kimhc // 2010-03-26 // 무한 스킬 버그 수정
			SUCM_UNREADY_OK,		// 레디상태가 풀리고 스킬 장착이 진행됨
			SUCM_UNREADY_CANCEL,	// 레디 상태가 안풀리고 스킬 장착이 무시됨
			//}} kimhc // 2010-03-26 // 무한 스킬 버그 수정

			SUCM_PC_BANG_GUIDE_OK,

			SUCM_RESURRECT_OK,
			SUCM_RESURRECT_CANCEL,
			SUCM_RESURRECT_USE_CASH_OK,
			SUCM_RESURRECT_USE_CASH_CANCEL,

			SUCM_AGREE_SUMMON_VILLAGE_BY_MARRIEGED_USER,		/// 마을로 소환 요청을 승락
			SUCM_DISAGREE_SUMMON_VILLAGE_BY_MARRIEGED_USER,		/// 마을로 소환 요청을 거부
			SUCM_AGREE_SUMMON_ROOM_BY_MARRIEGED_USER,			/// 필드로 소환 요청을 승락
			SUCM_DISAGREE_SUMMON_ROOM_BY_MARRIEGED_USER,		/// 필드로 소환 요청을 거부

//#ifdef SERV_ADVERTISEMENT_EVENT
			SUCM_CLOSE_ADVERTISEMENT = 3021, //해외팀 추가된 광고 UI에 사용하는 닫기버튼 2013.07.03 김창한
//#endif SERV_ADVERTISEMENT_EVENT

			SUCM_STATE_CHANGE_POPUP_OK,




//#if defined( SERV_ACCOUNT_BLOCK_MESSAGE_RENEWAL ) || defined( ACCOUNT_BLOCK_MESSAGE_RENEWAL_KR )
			SUCM_BLOCK_ACCOUNT_CONNECT = 3031,
//#endif //SERV_ACCOUNT_BLOCK_MESSAGE_RENEWAL // ACCOUNT_BLOCK_MESSAGE_RENEWAL_KR
		};


		// 초보자 가이드용도로 버튼 클릭은 유도하는 노란 화살표
		struct MouseClickUI
		{
			enum MOUSE_CLICK_STYLE
			{
				MCS_FROM_RIGHT_BOTTOM,
				MCS_FROM_LEFT_BOTTOM,
				MCS_FROM_LEFT_TOP,
				MCS_FROM_RIGHT_TOP,
//#ifdef SERV_NEW_DEFENCE_DUNGEON
				MCS_FROM_RIGHT_TOP_NORMAL, // 클릭 한글 제외한 버전
//#endif // SERV_NEW_DEFENCE_DUNGEON

			};

			enum HIDE_REASON
			{
				HR_COUPON_BOX,
				HR_CHARACTER_ROOM,
				HR_CASH_SHOP,
			};

		public:
			MouseClickUI( CKTDXStage* pStage, D3DXVECTOR2 vPos, float fTime, int nCount, MOUSE_CLICK_STYLE eStyle );
			~MouseClickUI()
			{
				SAFE_DELETE_DIALOG( m_pDLGMouseClick );
			}

			void SetShow( bool bShow )
			{
				if( m_pDLGMouseClick != NULL )
				{
					m_pDLGMouseClick->SetShow( bShow );
				}
			}

			bool GetShow()
			{
				if( m_pDLGMouseClick == NULL )
					return false;

				return m_pDLGMouseClick->GetShow();
			}

			void MoveDialogToTop()
			{
				g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGMouseClick, true );
			}


		private:
			CKTDGUIDialogType	m_pDLGMouseClick;
		};


		struct FadeInOutNotice		/// 어둠의 문 처럼 페이드 인 ~ 페이드 아웃 되는 공지 출력용 정보 저장 구조체
		{
		public:
			FadeInOutNotice( const WCHAR* pFadeInOutNoticeText_, float fFadeInOutNoticeTime_ )
				: m_wstrFadeInOutNoticeText ( pFadeInOutNoticeText_ )
				,m_fFadeInOutNoticeTime( fFadeInOutNoticeTime_ )
				,m_fFadeInOutNoticeMaxTime( fFadeInOutNoticeTime_ )
				,m_vOffsetPos(D3DXVECTOR2(0.f,0.f))
			{
				if( 5.f > fFadeInOutNoticeTime_ )	/// 일단 최소 출력 시간을 5초로 설정
				{
					fFadeInOutNoticeTime_		= 5.f;
					m_fFadeInOutNoticeMaxTime	= fFadeInOutNoticeTime_;
				}
			}
			const WCHAR*				GetFadeInOutNoticeText() { return m_wstrFadeInOutNoticeText.c_str(); }
			float						GetFadeInOutNoticeMaxTime() { return m_fFadeInOutNoticeMaxTime; }
			float						GetFadeInOutNoticeTime() { return m_fFadeInOutNoticeTime; }
			void						SetFadeInOutNoticeTime( float fFadeInOutNoticeTime_ ) { m_fFadeInOutNoticeTime = fFadeInOutNoticeTime_; }

			const D3DXVECTOR2&			GetOffsetPos(){ return m_vOffsetPos;}
			void						SetOffsetPosX( float fOffsetX_ ){ m_vOffsetPos.x = fOffsetX_; }
			void						SetOffsetPosY( float fOffsetY_ ){ m_vOffsetPos.y = fOffsetY_; }
		private:
			float						m_fFadeInOutNoticeTime;		/// 출력될 텍스트 표시 시간
			float						m_fFadeInOutNoticeMaxTime;	/// 출력될 텍스트 최대 표시 시간
			wstring						m_wstrFadeInOutNoticeText;

			D3DXVECTOR2					m_vOffsetPos;

		};


	public:
		CX2State(void);
		virtual ~CX2State(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual HRESULT OnFrameRender();


		virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

        virtual void	SetLuaFrameMoveFunc_LUA( const char* pFuncName )  { m_LuaFrameMoveFuncName = ( pFuncName ) ? pFuncName : ""; }



		void ResetUnitClassInFieldSquare( UidType unitUID, CX2Unit::UNIT_CLASS eUnitClass );
		void ResetUnitViewerInFieldSquare( UidType unitUID, CX2Unit::UNIT_CLASS eUnitClass );
		//{{ 허상형 : [2009/10/9] //	길드 이름 업데이트
		void ResetUnitGuildNameInFieldSquare( UidType unitUID, wstring wstrGuildName );

		//}} 허상형 : [2009/10/9] //

		CX2Cursor* GetCursor() { return m_pCursor; }

		// 09.03.23 김태완 : 이벤트 타이머 위치 이동
		// void RenderMarketingEventTimer();

		bool GetPersonalTrade() { return m_bPersonalTrade; }
		void SetPersonalTrade(bool val) { m_bPersonalTrade = val; }




		void CreateMouseClickUI( CKTDXStage* pStage, D3DXVECTOR2 vPos, float fTime = 0.5f, int nCount = 1000,
			MouseClickUI::MOUSE_CLICK_STYLE eStyle = MouseClickUI::MCS_FROM_RIGHT_BOTTOM )
		{
			DeleteMouseClickUI();
			m_pMouseClickUI = new MouseClickUI( pStage, vPos, fTime, nCount, eStyle );
		}
		void DeleteMouseClickUI()
		{
			SAFE_DELETE( m_pMouseClickUI );
		}
		bool GetShowMouseClickUI()
		{
			if(m_pMouseClickUI != NULL)
				return m_pMouseClickUI->GetShow();
			return false;
		}
		void SetShowMouseClickUI( bool bShow, MouseClickUI::HIDE_REASON eHideReason );
		void MoveMouseClickUIToTop()
		{
			if( NULL != m_pMouseClickUI )
				m_pMouseClickUI->MoveDialogToTop();
		}


		//////////////////////////////////////////////////////////////////////////
		virtual bool	Handler_EGS_NOTIFY_MSG_REQ_LUA( int noticeCount, const char* pMsg, bool bIsAllServers = false );
		virtual bool	Handler_EGS_ADMIN_MODIFY_UNIT_LEVEL_REQ_LUA( const char* pUnitNickName, int changeLevel, int iChangeEXP = 0 );
		virtual bool	Handler_EGS_ADMIN_CHANGE_ED_REQ_LUA( const char* pUnitNickName, int changeED );

		virtual bool 	Handler_EGS_ADMIN_CHANGE_AUTH_LEVEL_REQ( const WCHAR* userID, CX2User::X2_USER_AUTH_LEVEL level );
		virtual bool 	Handler_EGS_ADMIN_CHANGE_AUTH_LEVEL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool 	Handler_EGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ( CX2User::X2_USER_AUTH_LEVEL level );
		virtual bool 	Handler_EGS_ADMIN_GET_AUTH_LEVEL_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		virtual bool Handler_EGS_CHANGE_OPTION_PLAY_GUIDE_REQ();
		virtual bool Handler_EGS_CHANGE_OPTION_PLAY_GUIDE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_ADMIN_CHANGE_SKILL_POINT_REQ( int iSP );
		virtual bool Handler_EGS_ADMIN_CHANGE_SKILL_POINT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ oasis907 : 김상윤 [2009.11.30] // 길드 스킬 포인트 치트
		virtual bool Handler_EGS_ADMIN_GET_GUILD_SKILL_POINT_REQ( int iSP );
		virtual bool Handler_EGS_ADMIN_GET_GUILD_SKILL_POINT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} oasis907 : 김상윤 [2009.11.30] //


		virtual bool Handler_EGS_ADMIN_INIT_SKILL_TREE_REQ();
		virtual bool Handler_EGS_ADMIN_INIT_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_ADMIN_KICK_USER_REQ_LUA( const char* szUserNickName, bool bIsUserID = false );

		virtual bool Handler_EGS_ADMIN_SET_PC_BANG_REQ( bool bEnable, int iPcBangType = 0 );

		virtual bool Handler_EGS_CHECK_BALANCE_REQ();

		//{{ kimhc // 2009-10-14 // 자동결제 완료후 캐시인벤 갱신
		//}} kimhc // 2009-10-14 // 자동결제 완료후 캐시인벤 갱신

		bool Handler_EGS_OPTION_UPDATE_REQ();
		bool Handler_EGS_OPTION_UPDATE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_EGS_STATE_CHANGE_FIELD_REQ(int iVillageId = 0);
		bool Handler_EGS_STATE_CHANGE_FIELD_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_SEARCH_UNIT_REQ( const WCHAR* wszNickName );
		bool Handler_EGS_SEARCH_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_EGS_GET_CONNECTION_UNIT_INFO_REQ( const WCHAR* wszNickName );
		bool Handler_EGS_GET_CONNECTION_UNIT_INFO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void UpdateUserInfoUI( KEGS_SEARCH_UNIT_ACK &kEvent );

		bool Handler_EGS_GET_CHANNEL_LIST_REQ();
		bool Handler_EGS_GET_CHANNEL_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CHECK_CHANNEL_CHANGE_REQ(int iChannelId);
		bool Handler_EGS_CHECK_CHANNEL_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_DISCONNECT_FOR_CHANNEL_CHANGE_REQ();
		bool Handler_EGS_DISCONNECT_FOR_CHANNEL_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CONNECT_CHANNEL_CHANGE_REQ();
		bool Handler_EGS_CONNECT_CHANNEL_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_JOIN_FIELD_CHANNEL_CHANGE_REQ();
		bool Handler_EGS_JOIN_FIELD_CHANNEL_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_CC_KXPT_PORT_CHECK_REQ();
		bool Handler_CC_KXPT_PORT_CHECK_ACK( const KXPT_PORT_CHECK_ACK& kXPT_PORT_CHECK_ACK );
		void OnFrameMove_GameServerConnect( double fTime, float fElapsedTime );



		//{{ kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
		bool Handler_EGS_EVENT_QUEST_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_EVENT_QUEST_CHECK_FOR_ADMIN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
		bool Handler_EGS_HACKING_TOOL_LIST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_EXPAND_SKILL_NOTE_PAGE_NUM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_REG_SKILL_NOTE_MEMO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
		bool CheckIsReady();					// 현재 레디 상태이면 true, 레디 상태가 아니면 false 를 리턴
		void ShowOkAndCancelMsgForUnReady( int iStringID_ );	// 레디 상태를 풀겠냐는 ok, cancel 메시지 박스
		void ShowOkAndCancelMsgForUnReadyByEquip(); // 레디 상태를 풀겠냐는 ok, cancel 메시지 박스
		void ShowOkAndCancelMsgForUnReadyBySkill(); // 레디 상태를 풀겠냐는 ok, cancel 메시지 박스

		//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

		//{{ 허상형 : [2011/3/23/] //	월드 미션
		void MoveShowEvent(float fTime);

		void OpenStartMission();
		void OpenResultMission( bool bIsClear );
		//}} 허상형 : [2011/3/23/] //	월드 미션

		bool Handler_EGS_PSHOP_AGENCY_MESSAGE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CHANGE_PSHOP_AGENCY_EXPIRATION_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_HENIR_REWARD_COUNT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CHANGE_RANDOM_QUEST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_UPDATE_UNIT_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void Handler_EGS_CHECK_MACHINE_ID_REQ();
		bool Handler_EGS_CHECK_MACHINE_ID_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// 김민성
		// {{ kimhc // 2012-06-18 // 마을로 이동하기만 해도 완료되는 퀘스트 작업
		void CompleteQuestToVisitVillage( const CX2QuestManager::QuestTemplet* pQuestTemplet_ );
		KEGS_QUEST_COMPLETE_REQ CompleteQuestToVisitVillageList( const CX2QuestManager::QuestTemplet* pQuestTemplet_ );
		std::map<int,int> GetSelectRewardItemIDList(const CX2QuestManager::QuestTemplet* pQuestTemplet_ );
		// }} kimhc // 2012-06-18 // 마을로 이동하기만 해도 완료되는 퀘스트 작업

		// {{ 김태환 // 2012-06-25 // 선택적 셧다운제 안내
		// }} 김태환 // 2012-06-25 // 선택적 셧다운제 안내
		bool Handler_EGS_ADMIN_CHEAT_GET_ALL_SKILL_REQ();
		bool Handler_EGS_ADMIN_CHEAT_GET_ALL_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_ADMIN_AUTO_GET_ALL_SKILL_REQ();
		bool Handler_EGS_ADMIN_AUTO_GET_ALL_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_ADMIN_GET_SKILL_REQ( const int iSkillID, const int iSkillLevel );
		bool Handler_EGS_ADMIN_GET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



		void Handler_EGS_JOIN_BATTLE_FIELD_REQ( const UINT uiBattleFieldId_, const USHORT usBattleFieldLineIndex_, const USHORT usBattleFieldPositionValue_, const bool bMoveToMyParty_ = false );
		void Handler_EGS_JOIN_BATTLE_FIELD_REQ( const UINT uiBattleFieldId_, const USHORT usBattleFieldStartLineIndex_ ) const;
		bool Handler_EGS_JOIN_BATTLE_FIELD_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_JOIN_BATTLE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_LEAVE_BATTLE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드
		bool	Handler_EGS_CHECK_PING_SCORE_CHANGE_HOST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} kimhc // 2012-10-16 // 핑이 빠른 유저를 호스트로 변경하는 코드

		void ReturnToPlaceWhereBeforeDungeonStart();

		virtual bool Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
		virtual bool Handler_EGS_STATE_CHANGE_SERVER_SELECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void Send_EGS_UPDATE_PLAY_STATUS_NOT();

		void Send_EGS_USE_SKILL_BUFF_IN_PLAY_NOT( const int iSkillID_, const bool bOn_ );
		bool Handler_EGS_USE_SKILL_BUFF_IN_PLAY_BROAD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_BATTLE_FIELD_DANGEROUS_VALUE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_SURVEY_USER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	/// 계정 중에 필드 10렙을 키운 유저 설문 조사
		bool GetEnableShortCutKey()const { return m_bEnableShortCutKey; }
		void SetEnableShortCutKey(bool bVal){ m_bEnableShortCutKey = bVal; }

		bool GetWaitGameStart() const { return m_bWaitGameStart; }

		/// 유저 그룹에서 입력한 유저 제거
	//	bool Handler_EGS_ADMIN_REMOVE_HERO_PVP_USER_REQ( UidType kpiServerGroupID_, const char* pNickName_ );
		bool Handler_EGS_ADMIN_REMOVE_HERO_PVP_USER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		/// 운영자 공지 기능
		bool Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ( const WCHAR* chMessage_, const WCHAR* chNickName_, int iServerGroupID_ );
		bool Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ_LUA( const char* chMessage_, const char* chNickName_, int iServerGroupID_ );
		bool Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//bool Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ_LUA();

		//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
		bool Handler_EGS_ADMIN_OBSERVER_MODE_REQ( bool bMode );
		//}}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		bool Handler_EGS_TCP_PING( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK_VER2
		bool Handler_EGS_GET_ACTIVE_LAGCHECK_REQ();
		bool Handler_EGS_GET_ACTIVE_LAGCHECK_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK_VER2

//#ifdef X2OPTIMIZE_TCP_RELAY_TEST
//		bool Handler_EGS_TCP_RELAY_TEST2( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//#endif//X2OPTIMIZE_TCP_RELAY_TEST

		void SetFadeInOutNotice( const WCHAR* wszMsg_, float fVisibleTime_ = 5.f, float fOffsetPosX_ = 0.f , float fOffsetPosY_ = 0.f );		/// 화면에 페이드 인 ~ 페이드 아웃 되는 공지 설정 함수
		void ProcessFadeInOutNotice();													/// 화면에 페이드 인 ~ 페이드 아웃 되는 공지 갱신 함수

		bool Handler_EGS_TOGGLE_INVISIBLE_REQ();
		//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
		bool Handler_EGS_SHARING_BACK_OPEN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		bool Handler_EGS_BATTLE_FIELD_NPC_DAMAGED_MAP_REQ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );		// DamagedMap 전달 REQ
		bool Handler_EGS_SYNC_BATTLE_FIELD_DAMAGED_MAP_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );		// DamagedMap 전달 NOT
		bool Handler_EGS_ADMIN_BATTLE_FIELD_INCREASE_DANGER_ACK ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	// 위험도 증가 치트

		bool Handler_EGS_CHANGE_GAME_STAT_INFO_IN_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_CALL_MY_LOVER_REQ( int iMapID, D3DXVECTOR3 vPos, unsigned char ucLastTouchLineIndex );
		bool Handler_EGS_CALL_MY_LOVER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CALL_MY_LOVER_VILLAGE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CALL_MY_LOVER_AGREE_VILLAGE_NOT( bool bAgree );
		bool Handler_EGS_CALL_MY_LOVER_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CALL_MY_LOVER_AGREE_ROOM_NOT( bool bAgree );
		bool Handler_EGS_CALL_MY_LOVER_AGREE_RESULT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CALL_MY_LOVER_JOIN_BATTLE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_ADMIN_CHANGE_COUPLE_DATE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_JOIN_WEDDING_HALL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_CALL_MY_LOVER_STATE_CHANGE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){ return false;};


		virtual void PopTalkBox( UidType iUnitUID_, const WCHAR* pWstrMsg_,
			const D3DXCOLOR& coTextColor_, const D3DXCOLOR& coOutColor_, const D3DXCOLOR& coBackColor_ ) {};


		wstring GetExpirationDataDesc( IN const SEnum::AGENCY_SHOP_TYPE eAgencyShopType_, IN const wstring& wstrAgencyShopExpirationDate_ );

		bool Handler_EGS_CATCH_HACKUSER_INFO_NOT( int iCrashType );

		bool Handler_EGS_DIVORCE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		virtual bool UIEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool UICustomFuncProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool UIServerTimeOutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool UIP2PEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//		virtual bool TCPRelayEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual void LuaFrameMove( double fTime, float fElapsedTime );
		//void		 LuaFuncCall( const WCHAR* pFuncName );

		virtual bool RegisterLuaBind();

				//nProtect 와의 인증 처리
				bool Handler_EGS_KNPROTECT_AUTH_CHECK_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
				bool Handler_EGS_KNPROTECT_USER_SHOWDOWN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 허상형 : [2009/8/19] //	핵쉴드 확장서버연동 패킷 처리
		//}} 허상형 : [2009/8/19] //	핵쉴드 확장서버연동 패킷 처리

		bool Handler_EGS_NEW_CHARACTER_TRADE_BLOCK_MSG_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_NOTIFY_MSG_REQ( int noticeCount, const WCHAR* pMsg, bool bIsAllServers = false );
		virtual bool Handler_EGS_NOTIFY_MSG_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_ADMIN_SERVER_STATUS_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_CLIENT_QUIT_REQ();
		virtual bool Handler_EGS_CLIENT_QUIT_ACK();


		virtual bool Handler_EGS_ADMIN_MODIFY_UNIT_LEVEL_REQ( const WCHAR* pUnitNickName, int changeLevel, int iChangeEXP );
		virtual bool Handler_EGS_ADMIN_MODIFY_UNIT_LEVEL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_ADMIN_CHANGE_ED_REQ( const WCHAR* pUnitNickName, int changeED );
		virtual bool Handler_EGS_ADMIN_CHANGE_ED_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		virtual bool Handler_EGS_CHECK_INVALID_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		virtual bool Handler_EGS_UPDATE_QUEST_REQ();
		virtual bool Handler_EGS_UPDATE_QUEST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_UPDATE_QUEST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_CHECK_SUM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_ADMIN_KICK_USER_REQ( const WCHAR* szUserNickName, bool bIsUserID = false );
		virtual bool Handler_EGS_ADMIN_KICK_USER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 2008. 1. 2  최육사  근성도 치트
		virtual bool Handler_EGS_ADMIN_CHANGE_SPIRIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		virtual bool Handler_EGS_ADMIN_SET_PC_BANG_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 2008. 1. 25  최육사  VP 치트
		virtual bool Handler_EGS_ADMIN_CHANGE_VSPOINT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		//{{ 2008. 2. 18  최육사  유닛 클래스 치트
		virtual bool Handler_EGS_ADMIN_CHANGE_UNIT_CLASS_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		//{{ 2008. 10. 24  최육사	타이틀 치트
		virtual bool Handler_EGS_ADMIN_GET_TITLE_CHEAT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		//virtual bool Handler_EGS_REWARD_TO_TUTORIAL_USER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//virtual bool Handler_EGS_REWARD_TO_RECOMMEND_USER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_REWARD_MARKETING_EVENT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_RECOMMEND_USER_RESULT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ kimhc // 2009-12-04 // 크리스마스 이벤트
		virtual bool Handler_EGS_TIME_EVENT_INFO_NOT(  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} kimhc // 2009-12-04 // 크리스마스 이벤트

		virtual bool Handler_EGS_CHECK_BALANCE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_CASH_PRODUCT_INFO_CHANGED_NOT();

		virtual bool Handler_EGS_GUEST_USER_LIMIT_LEVEL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_NEW_PRESENT_CASH_INVENTORY_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_PRESENT_MESSAGE_TO_RECEIVER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		//virtual bool Handler_EGS_GET_POST_LETTER_LIST_REQ();
		//virtual bool Handler_EGS_GET_POST_LETTER_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_GET_RANKING_INFO_REQ();
		virtual bool Handler_EGS_GET_RANKING_INFO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_EGS_DELETE_NICK_NAME_SUCCESS_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 2009. 7. 29  최육사	랭킹 등록 팝업
		bool Handler_EGS_NEW_RECORD_RANKING_SUCCESS_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		bool Handler_EGS_CHANGE_JOB_CASH_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_CHANGE_USER_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_CHANGE_MY_UNIT_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 허상형 : [2009/10/14] //	자동 결제 부활석
		//}} 허상형 : [2009/10/14] //	자동 결제 부활석

		bool Handler_EGS_TRAFFIC_ATTACKER_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		//{{ oasis907 : 김상윤 [2009.11.19] //
		bool Handler_EGS_CHANGE_SKILL_INFO_IN_ROOM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} oasis907 : 김상윤 [2009.11.19] //

		// oasis907 : 김상윤 [2010.1.29] //  PC방 상용화
		virtual bool Handler_EGS_PC_BANG_AUTH_RESULT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ oasis907 : 김상윤 [2010.5.6] // 던전 입장 조건 아이템 이름 표시
		bool Handler_EGS_ERROR_POPUP_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		//{{ kimhc // 2011-08-10 // 채널링 SOAP
		//}} kimhc // 2011-08-10 // 채널링 SOAP
		bool Handler_EGS_UNIT_CLASS_CHANGE_CASH_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_EGS_DECREASE_SPIRIT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_DROP_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_GET_ITEM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_GET_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_GET_ITEM_REALTIME_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_END_GAME_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		virtual bool Handler_EGS_USER_UNIT_DIE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_USER_UNIT_DIE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		virtual bool Handler_EGS_USER_UNIT_DIE_COMPLETE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_UPDATE_PARTY_MEMBER_STATUS_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_GAME_START_TIME_COUNT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void SetShowPictureGameStartCount( const bool bIsDungeonWait, const int iCount_ );


		// 캐릭터선택
		virtual bool HandleMsgByESC();
		virtual bool HandleMsgByESCImp();
		virtual bool PutOffPopUpDialog();
		virtual bool QuitGame();
		virtual void OpenLastMsgPopUp( const WCHAR* pMsg, wstring wstrCustomLuaFileName = L"", wstring wstrPlaySoundFileName = L"" );


		//ESC키 눌러서 게임 종료 말고 다른 메시지가 떠야 할 경우 상속해서 오버라이딩 해줘야할 함수들 =_=a
		virtual bool LastMsgByESC();
		virtual void LastMsgPopOk();
		virtual void LastMsgPopCancel(); //캔슬에 그냥 다이얼로그 없애는것말고 특별한걸 해주고 싶으면 상속받아서 사용바람

		virtual bool ShortCutKeyProcess();

		virtual void UpdateCashUI();

//{{ kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트)
		void CheckNRefreshDailyQuest();
//}} kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트)


		// 09.03.23 김태완 : 이벤트 타이머 위치 이동
		//void CloseGuestUserEventTimerPopup();
		//void OpenGuestUserEventTimerPopup();
		bool Handler_EGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_SERVER_CHECK_HACK_USER_REQ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_SERVER_CHECK_HACK_USER_ACK();

		//{{ kimhc // 2010-08-16 // 재접속 하면 기간이 늘어나는 이벤트 전용 아이템 시스템을 구현한 것 입니다.
		bool Handler_EGS_RESET_PERIOD_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}} kimhc // 2010-08-16 // 재접속 하면 기간이 늘어나는 이벤트 전용 아이템 시스템을 구현한 것 입니다.

		bool Handler_EGS_ADMIN_SHOW_ED_CHEAT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		void SendLoadedModuleList(bool bForce = false);

		//{{ 2012. 07. 09	최육사		대기열 시스템
		bool Handler_ECH_WAIT_QUEUE_MESSAGE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}

		void OpenAdvertisementDialog( const wstring& wstrURL );
		virtual bool Handler_EGS_ADVERTISEMENT_EVENT_INFO_NOT(  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_UPDATE_MAX_LEVEL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_UPDATE_CODE_EVENT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
		bool Handler_EGS_UDP_KICK_STATE_CHANGE_FIELD_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool Handler_EGS_FORCE_RELAY_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		//}}




		bool Handler_EGS_GET_ITEM_TEMP_INVENTORY_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_SHOW_DISCONNECT_REASON_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		virtual void UserAndPetListPopUpMenuProcess() {}

        void CheckAndSendingPlayStatus( float fElapsedTime_ );

		bool ProcessGameScoreAndPostEffect();
		virtual void ProcessShowMiniMap( const bool bHide_ ) {};




	public:
		vector<FadeInOutNotice*> m_vecFadeInOutNotice;	/// 페이드 인 ~ 페이드 아웃 공지 정보 저장용 컨테이너

#if X2_DEV_TOOLS
#endif // _IN_HOUSE_







	protected:

		float			m_fTime;
		float			m_fElapsedTime;

		CX2Cursor*		m_pCursor;

		bool			m_bFailToConnectServer;
		CKTDGUIDialogType	m_pMsgOkFailToConnectServer;
		//{{ oasis907 : 김상윤 [2010.2.3] //
		bool			m_bFailPCBANGAuth;
		CKTDGUIDialogType	m_pMsgOkFailPCBANGAuth;
		//}}
		string			m_LuaFrameMoveFuncName;

		CKTDGUIDialogType	m_pMsgBoxQuitGame;		/// 게임 종료 대화 상자
		bool			m_bOpenMsgBoxQuitGame;
		bool			m_bEnableQuitGame;

		CKTDGUIDialogType	m_pDLGMsgBox;


		bool			m_bOneTimeProcAfterContructor;

		bool			m_bIgnoreAllUIEvent;
		float			m_fIgnoreTime;

		CKTDGUIDialogType	m_pDLGLastMsgPopUp;

		bool			m_bEnableShortCutKey;


		CKTDGUIDialogType	m_pDLGEventClock;
		CKTDGUIDialogType	m_pDLGEventClockPopup;
		CKTDGUIDialogType	m_pDLGGuestUserGoUnitSelect;


		CKTDGUIDialogType	m_pDLGRecommendReward;
		CKTDGUIDialogType	m_pDLGTutorReward;
		CKTDGUIDialogType	m_pDLGGuestUserReward;

		bool			m_bCheckPostItem;

        bool            m_bPersonalTrade;

		CKTDGUIDialogType	m_pDLGUserInfo;

		bool			m_bConnectedGameServer;
		bool			m_bPortCheckReq;
		bool			m_bPortCheckWait;
		float			m_fPortCheckWaitTime;
		int				m_PortCheckRetryTime;
		int				m_iChangeChannelId;

		bool			m_bConnectFullChannel;		// Full 채널에 접속시 접속실패하여 접속이 끊길경우 "서버로부터 응답이 없습니다." 메세지 안띄우기 위한 플래그



		MouseClickUI*								m_pMouseClickUI;
		map< MouseClickUI::HIDE_REASON, bool >		m_mapHideMouseClickUI;



		//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
		CKTDGUIDialogType		m_pDLGUnReady;
		//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정


		//{{ 허상형 : [2011/3/24/] //	월드 미션
		float							m_fShowTimeForWorldMissionBuffName;
		CKTDGUIDialogType				m_pDLGWorldMissionBuffName;
		//}} 허상형 : [2011/3/24/] //	월드 미션

		CKTDGParticleSystem::CParticleEventSequenceHandle m_hParticleLogo;

		CKTDGUIDialogType	m_pDlgGameStartCount;


		CKTDGUIDialogType	m_pDLGStateChangePopup;

		CKTDGUIDialogType				m_pDLGPCBangBenefitGuide;


		bool							m_bWaitGameStart;

		CKTDGUIDialogType	m_pDlgAdvertisement;


		/// 서버에서 사용하는 인자값 저장
		UidType							m_uiRoomUID;
		UidType							m_iCalledUserUID;
		UidType							m_iCalledGSUID;
		char							m_cCallType;
		int								m_iMapID;
		D3DXVECTOR3						m_vPos;
		unsigned char					m_LastTouchLineIndex;


		bool							m_bReConnectChannelServer;		// 채널 접속 시, 실패하면 다른 채널을 찾아볼 것인가?

		bool							m_bAlreadyShowErrorMessage;


};
