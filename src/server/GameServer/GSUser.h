#pragma once
#include "Actor.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"
#include "SystemPacket.h"
#include "ServerPacket.h"

#include "x2/platform/win32.h"    // ::GetTickCount()
#include "CacheData.h"
#include "x2/platform/timer.h"
//{{ 2010. 01. 15  최육사	boost allocator
	#include <ObjectPool/BoostAllocator.h>
//}}

//----------- 2013.02.27 조효진	해외 기본 구조 작업 (SERV_GLOBAL_BASE)-------------------------------//

//{{ 2012.11.13 조효진 닉네임 관련 체크 검사 시 대소문자 구별 안하도록 수정
//#ifdef SERV_STRING_CHECK_IGNORE_CASE
#include "x2/platform/string.h"
//#endif SERV_STRING_CHECK_IGNORE_CASE
//}}



//{{ 2010. 10. 04 조효진	빌링 관련 추가. 대만 코드 참조
#include "BillManager.h"
#include "../Common/OnlyGlobal/AuthAndBilling/KncSendToPublisher.h"
//}}
//---------------------------------------------------------------------------------------------------//

#include "Inventory.h"
//{{ 2009. 5. 30  최육사	임시 인벤토리
#include "TempInventory.h"
//}}
#include "UserQuestManager.h"
//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	#include "UserAccountQuestManager.h"
//}}
#include "UserSkillTree.h"
#include "CharLog.h"
//#include "GGAuth.h"
//{{ 2009. 8. 18  최육사	hack shield
//}}
#include "UserStatistics.h"
#include "MaxProcessTime.h"
#include "UserGameOption.h"
#include "UserTutorialInfo.h"
#include "UserWishList.h"
#include "UserCashInventory.h"
#include "UserLetterBox.h"
#include "UserTitleManager.h"
//{{ 2009. 7. 7  최육사		랭킹개편
#include "UserRankingManager.h"
//}}
#include "RepeatEventFilter/RepeatEventFilter.h"
//{{ 2009. 6. 10  최육사	타이머 매니저
#include "TimerManager.h"
//}}
//{{ 2009. 9. 13  최육사	유저 로그
#include "UserLogManager.h"
//}}
//{{ 2009. 10. 13  최육사	길드
	#include "UserGuildManager.h"
//}}
//{{ 2009. 10. 26  최육사	PC방이벤트
#include "UserEventManager.h"
//}}
//{{ 2010. 01. 08  최육사	추천인리스트
#include "UserRecommendManager.h"
//}}
//{{ 2010. 01. 29  최육사	PC방 상용화
#include "UserPcBangManager.h"
//}}
//{{ 2010. 02. 16  최육사	해킹툴 리스트
	#include "UserHackingManager.h"
//}}
//{{ 2010. 04. 19  최육사	서버 코드 정리
#include "AbuserLog/KAbuserLogManager.h"
//}}

//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
//}} 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템

//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	#include "UserAbuserManager.h"
//}}
//{{ 2010. 07. 21  최육사	펫 시스템
	#include "UserPetManager.h"
//}}
//{{ 2011. 04. 11	최육사	대리 상인
	#include "UserPersonalShopManager.h"
//}}

//{{ 2011. 05. 02  김민성	2차 보안 시스템
	#include "UserUnitSelectRewardManager.h"
//}}

//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
#include "ComeBackUserManager.h"
//}}

//{{ 2011. 06. 21	최육사	대전 개편
	#include "UserPvpManager.h"
//}}
//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "UserSecurityManager.h"
//}}

//{{ 2011. 08. 12   김민성      헤니르 개편
	#include "UserDungeonManager.h"
//}}
//{{ 2011. 12. 20	최육사	배틀필드 시스템
	#include "UserPartyManager.h"
//}}
//{{ 2012. 02. 29	최육사	배틀필드 시스템
	#include "UserSpiritManager.h"
//}}
//{{ 버프 관리 시스템 - 김민성
	#include "UserServerBuffManager.h"
//}}
//{{ 2012. 05. 23	최육사	배틀필드 시스템
	#include "UserUnitManager.h"
//}}
//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
	#include "UserCheatManager.h"
//}}
//{{ 2012. 09. 22	박세훈	빙고 이벤트
//}}
//{{ 2012. 09. 28	최육사		ED&아이템 모니터링 시스템
//}}
//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
//}}
	#include "UserRidingPetManager.h"
//{{ 2013. 04. 01	 인연 시스템 - 김민성
	#include "UserRelationshipManager.h"
//}

	#include "AfterWorkStorage.h"


#ifdef _DEBUG
#define _VERIFY_POINTER( pointer, ret ) \
	if( !_CrtIsValidHeapPointer( pointer ) ) \
	{ \
		START_LOG( cerr, L ## #pointer L"포인터 값 이상. (state:" << GetStateIDString() << L", name:" << m_strName << L")" ); \
		return ret; \
	}
#else
#define _VERIFY_POINTER( pointer, ret ) \
	if( pointer == NULL ) \
	{ \
		START_LOG( cerr, L ## #pointer L"포인터 값 이상. (state:" << GetStateIDString() << L", name:" << m_strName << L")" ); \
		return ret; \
	}
#endif //#ifdef _DEBUG
#define VERIFY_POINTER( pointer )   _VERIFY_POINTER( pointer, (void)0 )

//{{ 2010. 9. 2	최육사	중폭 패킷 필터 개선
#define _VERIFY_STATE_REPEAT_FILTER( loglevel, varg, reqEventid, ackEventid, ackPacket ) \
	if( m_kRepeatEventFilter.CheckReqEvent( reqEventid, ackEventid ) == false ) \
	{ \
		START_LOG( cwarn, L"Repeat Event Filtering!" ) \
			<< BUILD_LOG( GetCharUID() ) \
			<< BUILD_LOG( KEvent::GetIDStr( reqEventid ) ); \
		return; \
	} \
	if( !m_pkCurrentState->CheckState varg ) \
	{ \
		START_LOG( loglevel, L"상태 오류. [return ack] Name : " << m_strName ) \
			<< L"    현재 상태 : " << GetStateIDString() << dbg::endl \
			<< L"    허용 상태 : " L ## #varg << END_LOG; \
		ackPacket kAck; \
		kAck.m_iOK = NetError::ERR_WRONG_STATE_00; \
		SendPacket( ackEventid, kAck ); \
		return; \
	}

#define VERIFY_STATE_REPEAT_FILTER( varg, reqEventid, ackEventid )			_VERIFY_STATE_REPEAT_FILTER( cerr, varg, reqEventid, ackEventid, K##ackEventid )
#define VERIFY_STATE_WARN_REPEAT_FILTER( varg, reqEventid, ackEventid )		_VERIFY_STATE_REPEAT_FILTER( cwarn, varg, reqEventid, ackEventid, K##ackEventid )
//}}

//{{ 2012. 02. 22	박세훈	길드 이름 변경권

/*

목적: EGS_CHANGE_GUILD_NAME_REQ는 반드시 EGS_CHANGE_GUILD_NAME_CHECK를 정상적으로 통과한 뒤에만 작동 하게끔 한다.

1. EGS_CHANGE_GUILD_NAME_REQ, EGS_CHANGE_GUILD_NAME_CHECK_REQ 를 중복 필터를 건다.
2. 중복 검사가 끝난 후,
m_iOK != NetError::NET_OK 이면
REMOVE_VERIFY_STATE_REPEAT_FILTER( EGS_CHANGE_GUILD_NAME_REQ, EGS_CHANGE_GUILD_NAME_ACK );를 통해	EGS_CHANGE_GUILD_NAME_REQ를 해제하고,
SendPacket( EGS_CHANGE_GUILD_NAME_CHECK_ACK, kPacket_ );를 통해			EGS_CHANGE_GUILD_NAME_CHECK_REQ를 해제한다.

m_iOK == NetError::NET_OK 이면
SendPacket( EGS_CHANGE_GUILD_NAME_CHECK_ACK, kPacket_ );를 통해			EGS_CHANGE_GUILD_NAME_CHECK_REQ만 해제한다.

3. 클라이언트에서 EGS_CHANGE_GUILD_NAME_REQ를 보내왔을 때,
EGS_CHANGE_GUILD_NAME_CHECK_REQ는 걸려 있어선 안되며, 동시에 EGS_CHANGE_GUILD_NAME_REQ는 걸려 있어야만 한다.

- EGS_CHANGE_GUILD_NAME_CHECK 없이 EGS_CHANGE_GUILD_NAME_REQ를 조작하여 보내는 경우를 막았다.
- EGS_CHANGE_GUILD_NAME_CHECK_ACK를 받기 전에 조작된 EGS_CHANGE_GUILD_NAME_REQ를 보내는 경우를 막았다.

프로세스 시작 시에 두개 이상의 조건을 걸고, 처리하기 위함

_JUST_VERIFY_STATE_REPEAT_FILTER : 체크만 한다. reqEventid에 해당하는 정보가 없어도 등록하지 않는다.
_NEGATIVE_VERIFY_STATE_REPEAT_FILTER : !_JUST_VERIFY_STATE_REPEAT_FILTER 와 동일하다. reqEventid에 해당하는 정보가 없을 경우의 처리가 달라질 것을 염두에 두고 만들었다.
*/

#define _JUST_VERIFY_STATE_REPEAT_FILTER( loglevel, varg, reqEventid, ackEventid, ackPacket ) \
	if( m_kRepeatEventFilter.JustVerifyReqEvent( reqEventid ) == false ) \
	{ \
	START_LOG( cwarn, L"Just Verify Event Filtering!" ) \
	<< BUILD_LOG( GetCharUID() ) \
	<< BUILD_LOG( KEvent::GetIDStr( reqEventid ) ); \
	return; \
	} \
	if( !m_pkCurrentState->CheckState varg ) \
	{ \
	START_LOG( loglevel, L"상태 오류. [return ack] Name : " << m_strName ) \
	<< L"    현재 상태 : " << GetStateIDString() << dbg::endl \
	<< L"    허용 상태 : " L ## #varg << END_LOG; \
	ackPacket kAck; \
	kAck.m_iOK = NetError::ERR_WRONG_STATE_00; \
	SendPacket( ackEventid, kAck ); \
	return; \
	}

#define JUST_VERIFY_STATE_REPEAT_FILTER( varg, reqEventid, ackEventid )			_JUST_VERIFY_STATE_REPEAT_FILTER( cerr, varg, reqEventid, ackEventid, K##ackEventid )
#define JUST_VERIFY_STATE_WARN_REPEAT_FILTER( varg, reqEventid, ackEventid )	_JUST_VERIFY_STATE_REPEAT_FILTER( cwarn, varg, reqEventid, ackEventid, K##ackEventid )

#define _NEGATIVE_VERIFY_STATE_REPEAT_FILTER( loglevel, varg, reqEventid, ackEventid, ackPacket ) \
	if( m_kRepeatEventFilter.NegativeCheckReqEvent( reqEventid ) == false ) \
	{ \
	START_LOG( cwarn, L"Negative Event Filtering!" ) \
	<< BUILD_LOG( GetCharUID() ) \
	<< BUILD_LOG( KEvent::GetIDStr( reqEventid ) ); \
	return; \
	} \
	if( !m_pkCurrentState->CheckState varg ) \
	{ \
	START_LOG( loglevel, L"상태 오류. [return ack] Name : " << m_strName ) \
	<< L"    현재 상태 : " << GetStateIDString() << dbg::endl \
	<< L"    허용 상태 : " L ## #varg << END_LOG; \
	ackPacket kAck; \
	kAck.m_iOK = NetError::ERR_WRONG_STATE_00; \
	SendPacket( ackEventid, kAck ); \
	return; \
	}

#define NEGATIVE_VERIFY_STATE_REPEAT_FILTER( varg, reqEventid, ackEventid )			_NEGATIVE_VERIFY_STATE_REPEAT_FILTER( cerr, varg, reqEventid, ackEventid, K##ackEventid )
#define NEGATIVE_VERIFY_STATE_WARN_REPEAT_FILTER( varg, reqEventid, ackEventid )	_NEGATIVE_VERIFY_STATE_REPEAT_FILTER( cwarn, varg, reqEventid, ackEventid, K##ackEventid )

#define REMOVE_VERIFY_STATE_REPEAT_FILTER( reqEventid, ackEventid )								m_kRepeatEventFilter.RemoveReqEvent( reqEventid, ackEventid );

//}}


SmartPointer(KGSUser);

//////////////////////////////////////////////////////////////////////////
BOOST_ALOC_SYNC_CLASS_INHERITANCE( KGSUser, public KActor )
//////////////////////////////////////////////////////////////////////////
{
	friend class KUserQuestManager;	// 연관 함수가 많아서 아직은 없애지 못함.
	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	friend class KUserAccountQuestManager;	// 연관 함수가 많아서 아직은 없애지 못함.
	//}}
	friend class KPartyManager;		// 점차 코드 리팩토링을 해서 friend를 제거해야함.
	//{{ 2010. 01. 28  최육사	통합파티
	friend class KPartyListManager;	// 통합파티
	//}}

    NiDeclareRTTI;
    DeclToStringW;
    DeclPfID;

public:
    KGSUser(void);
    virtual ~KGSUser(void);

    enum
	{
		SEARCH_UNIT_REFRESH_TIME_LIMIT = 3,
		KNM_NEW_FRIEND_REFRESH_TIME_LIMIT = 3,
    };

    enum ENUM_TIMER
    {
        TM_DB_UPDATE = 0,						// DB 업데이트 주기
        TM_WHISPER_TICK,
        TM_ROOM,
		TM_SEARCH_UNIT_REFRESH,
		TM_KNM_NEW_FRIEND_REFRESH,
        TM_INIT,
        TM_UID_UPDATED,
		TM_UNIT_DURATION,
		TM_SQUARE_PLAY,
        TM_CHECK_ITEM_EXPIRATION,
		TM_RESOLVE_JACKPOT,
		TM_SKILL_SLOT_EXPIRATION,
		TM_EVENT_CHECK,							// 시간 이벤트 체크 타이머
		TM_CASH_RES_STONE,						// 실시간 부활석 연속 사용 제한 타이머
		TM_CHECK_TITLE_EXPIRATION,				// 칭호 기간제 검사 타이머
		TM_CHECK_CASH_SKILL_POINT_EXPIRATION,	// cash skill point 타이머
		TM_CHECK_AUTO_PAYMENT,					// 자동결제 체크 타이머
		TM_FIELD_SYNC_GAP,						// 필드 싱크 패킷 간격 타이머
		//{{ 2010. 02. 16  최육사	해킹툴 리스트
		TM_HACKING_TOOL_LIST,					// 해킹툴 리스트
		//}}
		TM_REFRESH_TRADE_BOARD,					// 거래 게시판 검색 쿨타임
		//{{ 2011. 04. 05  김민성	우편 및 거래 감시
		TM_MONITORING_LETTER,
		TM_MONITORING_TRADE,
		//}}
		//{{ 2011. 04. 26	최육사	deserialize 실패 종료
		TM_DESERIALIZE_FAIL,
		//}}
		//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		TM_COME_BACK_USER,
		//}}
		//{{ 2011. 08. 12   김민성      헤니르 개편
		TM_HENIR_REWARD_EVENT,
		//}}
		//{{ 2012. 02. 21	김민성	인벤토리 정렬
		TM_SORT_INVENTORY,						// 아이템 정렬 기능 쿨타임
		//}}
		//{{ 2012. 02. 27	김민성	인벤토리 정렬	- 무거운 기능을 가볍게 변경
		TM_SORT_INVENTORY_USE_TIME,				// 아이템 정렬 기능 패킷 소요시간
		//}}
		//{{ 2012. 04. 21	박세훈	어둠의 문 개편
		TM_CHECK_BUFF_PERIOD,
		//}}
		//{{ 2012. 05. 31	최육사	배틀필드 시스템
		TM_AUTO_PARTY_BONUS_CHECK,				// 자동 파티 보너스 체크 타이머
		//}}
		//{{ 2012. 06. 13	김민성       선택적 셧다운제
		//}}
		//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
		//}}
		//{{ 2012. 10. 15	최육사		ED&아이템 모니터링 시스템
		//}}
		//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
		TM_AUTO_PARTY_WAIT_USER_COUNT,
		//}}
		//{{ 2013. 01. 01	최육사	던전에서 필드로 돌아오지 못하는 오류 수정
		TM_SKILL_CHANGE_COOL_TIME_AT_START_COUNT,
		//}}


		//{{ 2013. 03. 18	 만우절 이벤트_레인보우 버프 - 김민성
		//}

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		TM_RELATIONSHIP_REWARD,
		//}
        TM_TOTAL_NUM,
    };

	//임시로 특정기능패킷중(현재는 방관련) 중복으로 날아오는 패킷체크를 위해.
	struct KRepeatEvent
	{
		u_short		m_usPreEventID;
		int			m_iPreEventIDCnt;

		void Reset()
		{
			m_usPreEventID		= 0;
			m_iPreEventIDCnt	= 0;
		}
	};

	//{{ 2009. 1. 20  최육사	경험치 제한
	class KUserExpManager
	{
	public:
		void SetExp( int iVal )				{ m_iExp.SetValue( iVal ); }
		int GetExp()						{ return m_iExp; }

		int GetChangeExp()					{ return m_iExp.GetChangeValue(); }

		void AddExp( int iExp );

		int GetInitExp()					{ return m_iExp.GetInitValue(); }
		void AdjustInitExp( int iVal )		{ m_iExp.AdjustInitValue( iVal ); }

	private:
		 KCacheData< int > m_iExp;
	};
	//}}

	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	struct KDungeonRequired
	{
		bool							m_bState;
		int								m_Itemid;

		KDungeonRequired()
		{
			Clear();
		}

		void Clear()
		{
			m_bState = false;
			m_Itemid = 0;
		}
	};
	//}}

	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}

    // override
    virtual void	Tick();

	//////////////////////////////////////////////////////////////////////////
	// Public Member Function
public:
	//////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------//
//----------- 2013.02.27 조효진	해외 기본 구조 작업 (SERV_GLOBAL_BASE)-------------------------------//
//---------------------------------------------------------------------------------------------------//

	void CheckTradeBlockUnit();

	void CheckNewDefenceDungeonEnter100Percent();


	//{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
	static bool LastDateSort(const KUnitInfo& refLUnitInfo, const KUnitInfo& refRUnitInfo);
	//}}

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//

	// User Info
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	const std::wstring&	GetUserName() const{ return m_kAccountInfo.m_wstrName; }
//#ifdef	SERV_HACKING_USER_CHECK_COUNT// 작업날짜: 2013-06-02	// 김민성 // 해외팀 주석 제거
	int			GetAuthLevel() const { return m_kAccountInfo.m_iAuthLevel; }
//#else // SERV_HACKING_USER_CHECK_COUNT
	/*
	char			GetAuthLevel()			{ return static_cast< char >(m_kAccountInfo.m_iAuthLevel); }
	*/
//#endif // SERV_HACKING_USER_CHECK_COUNT
	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	bool			CheckAuthLevel();
	//}}
//#else
//	const std::wstring&	GetUserName()		{ return m_wstrUserName; }
//	char			GetAuthLevel()			{ return m_cAuthLevel; }
	//}}
	bool			IsGuestUser() const		{ return ( m_kNexonAccountInfo.m_byteGuestUser == 1 ); }
	bool			IsPcBang()				{ return m_kUserPcBangManager.IsPcBang(); }
	int				GetPcBangType()			{ return m_kUserPcBangManager.GetPcBangType(); }
	KPcBangReward	GetPcBangReward()		{ return m_kUserPcBangManager.GetPcBangReward(); }




	int				GetiRidingPetSummoned();				// 이벤트용 or 캐시용 펫 여부
	int				GetiRidingSummonedPetID();				// 소환된 라이딩용 펫 ID

	void			SetbSC1( bool mbSC1)				{ m_bSC1 = mbSC1; }
	void			SetbSC2( bool mbSC2)				{ m_bSC2 = mbSC2; }

	bool			IsbSC1()							{ return m_bSC1; }
	bool			IsbSC2()							{ return m_bSC2; }

	void			SetCouple( bool bCouple ) { m_bCouple = bCouple; }
	bool			GetCouple() const { return m_bCouple; }
	void			SetRelationTargetUserUid( UidType iRelationTargetUserUid ) { m_iRelationTargetUserUid = iRelationTargetUserUid; }
	UidType			GetRelationTargetUserUid() const { return m_iRelationTargetUserUid; }
	void			SetRelationTargetUserNickname( std::wstring wstrRelationTargetUserNickname ) { m_wstrRelationTargetUserNickname = wstrRelationTargetUserNickname; }
	std::wstring	GetRelationTargetUserNickname() const { return m_wstrRelationTargetUserNickname; }


	//////////////////////////////////////////////////////////////////////////
	// Unit Info
	void			SetUnitInfo( const KUnitInfo& kInfo );
	void            GetUnitInfo( KUnitInfo& kInfo );
	void			ResetUnitInfo();
	bool			ResetStat();			// 전직퀘스트 수행후 불려질 함수.
	//{{ 2010. 8. 18	최육사	서버 코드 정리
	void			ClearTempInventory();
	//}}

	UidType			GetCharUID()const		{ return m_nUnitUID; }
    const std::wstring&	GetCharName() const	{ return m_wstrNickName; }
	char			GetUnitClass() const	{ return m_cUnitClass; }
	void			SetUnitClass( char cUnitClass ){ m_cUnitClass = cUnitClass; }
	int				GetEXP()				{ return m_kEXP.GetExp(); }
	u_char			GetLevel() const		{ return m_ucLevel; }
	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}

	void			SetUnitLoginTime_INT( IN CTime& tLogin )			{ m_tCurUnitLoginTime_INT = tLogin; }
	CTime&			GetUnitLoginTime_INT()								{ return m_tCurUnitLoginTime_INT; }

	int				GetBaseHP()				{ return m_kStat.m_iBaseHP; }
	//{{ 2012. 10. 02	최육사		ED&아이템 모니터링 시스템
	int             GetED()					{ return m_iED; }
	void			DecreaseED( int iDecreaseED ) { m_iED -= iDecreaseED; }
	//}}
	//{{ 2011. 06. 20	최육사	대전 개편
	int             GetRating() const		{ return m_kUserPvpManager.GetRating(); }
	int				GetRPoint()	const		{ return m_kUserPvpManager.GetRPoint(); }
	int				GetAPoint()	const 		{ return m_kUserPvpManager.GetAPoint(); }
	int				GetWin() const			{ return m_kUserPvpManager.GetWin(); }
	int				GetLose() const			{ return m_kUserPvpManager.GetLose(); }
	CXSLUnit::PVP_RANK GetPvpRank() const	{ return m_kUserPvpManager.GetPvpRank(); }
	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	CXSLUnit::PVP_RANK GetPvpRankForClient( void ) const;
	void GetPvpRankForClient( IN const int iOfficialMatchCnt, IN OUT char& m_cRank ) const;	// 캐릭터 리스트 읽어올 때 예외 부분 있으니 참고할 것!
	int				GetOfficialMatchCount( void ) const { return m_kUserPvpManager.GetOfficialMatchCount(); }
	//}}
	//}}
	void			AddAPoint( IN const int iAPoint ) { m_kUserPvpManager.AddAPoint( iAPoint ); }

	void			SetRoomListID( IN const UINT uiRoomListID );
	UINT			GetRoomListID() const						{ return m_uiRoomListID; }

	//{{ 2012. 02. 02	최육사	배틀필드 시스템
	int				GetMapID() const							{ return m_kLastPos.m_iMapID; }
	void			SetMapID( IN const int iMapID )				{ m_kLastPos.m_iMapID = iMapID; }
	const KLastPositionInfo& GetLastPosInfo() const				{ return m_kLastPos; }
	void			SetLastPosInfo( IN const KLastPositionInfo& kInfo ) { m_kLastPos = kInfo; }
	//}}
    UidType         GetRoomUID() const							{ return m_iRoomUID; }
	//{{ 2012. 03. 05	최육사	배틀필드 시스템
	UidType         GetBeforeRoomUID() const					{ return m_iBeforeRoomUID; }
	void			SetRoomUID( IN const UidType iUID );
	//}}
	UidType			GetTradeUID()								{ return m_iTradeUID; }
	void			SetTradeUID( IN const UidType iUID )		{ m_iTradeUID = iUID; }
	UidType			GetPersonalShopUID()						{ return m_iPersonalShopUID; }
	void			SetPersonalShopUID( IN const UidType iUID ) { m_iPersonalShopUID = iUID; }
	//{{ 2011. 04. 11	최육사	대리 상인
	UidType			GetPShopAgencyUID() const					{ return m_kUserPersonalShopManager.GetPShopAgencyUID(); }
	void			SetPShopAgencyUID( IN const UidType iUID )	{ m_kUserPersonalShopManager.SetPShopAgencyUID( iUID ); }
	//}}
	UidType			GetFieldUID() const							{ return m_iFieldUID; }
	//{{ 2012. 03. 05	최육사	배틀필드 시스템
	void			SetFieldUID( IN const UidType iUID );
	//}}

	//{{ 2011. 12. 20	최육사	배틀필드 시스템
	UidType			GetPartyUID() const									{ return m_kUserPartyManager.GetPartyUID(); }
	void			SetPartyUID( UidType iUID )							{ m_kUserPartyManager.SetPartyUID( iUID ); }
	UidType			GetCandidatePartyUID() const						{ return m_kUserPartyManager.GetCandidatePartyUID(); }
	void			SetCandidatePartyUID( UidType iUID )				{ m_kUserPartyManager.SetCandidatePartyUID( iUID ); }
	UidType			GetQuickJoinPartyUID() const						{ return m_kUserPartyManager.GetQuickJoinPartyUID(); }
	void			SetQuickJoinPartyUID( UidType iUID )				{ m_kUserPartyManager.SetQuickJoinPartyUID( iUID ); }
	//{{ 2010. 02. 09  최육사	빠른파티 기능개선
	void			AddKickedPartyUID( IN UidType iPartyUID )			{ m_kUserPartyManager.AddKickedPartyUID( iPartyUID ); }
	bool			CheckKickedPartyUID( IN UidType iPartyUID )			{ return m_kUserPartyManager.CheckKickedPartyUID( iPartyUID ); }
	void			CheckExpiredKickedPartyUID()						{ m_kUserPartyManager.CheckExpiredKickedPartyUID(); }
	//}}
	void			ClearWaitPartyUser()								{ m_kUserPartyManager.ClearWaitPartyUser(); }
	bool			RegWaitPartyUser( IN const UidType iUnitUID )		{ return m_kUserPartyManager.RegWaitPartyUser( iUnitUID ); }
	bool			IsWaitPartyUser( IN const UidType iUnitUID ) const	{ return m_kUserPartyManager.IsWaitPartyUser( iUnitUID ); }
	bool			IsEmptyWaitPartyUser() const						{ return m_kUserPartyManager.IsEmptyWaitPartyUser(); }
	//}}
	//{{ 2012. 10. 25	최육사	배틀필드 시스템
	UidType			GetAutoPartyUID() const								{ return m_kUserDungeonManager.GetAutoPartyUID(); }
	UidType			GetAutoPartyWaitNumber() const						{ return m_kUserDungeonManager.GetAutoPartyWaitNumber(); }
	//}}
	//{{ 2009. 9. 22  최육사	길드
	int				GetGuildUID()			{ return m_kUserGuildManager.GetGuildUID(); }
	//}}
	char            GetUnitType() const			{ return static_cast<char>( CXSLUnit::GetUnitClassToUnitType( static_cast<CXSLUnit::UNIT_CLASS>(GetUnitClass()) ) ); }
	char			GetUnitState();	//유닛의 현재 위치
	//{{ 2010. 7. 29  최육사	서버 코드 정리
    bool			GetPVPChannelClass( OUT int& iPvpChannelClass ) const;
	//}}
	//{{ 2010. 8. 3	최육사	펫 시스템
	UidType			GetSummonedPetUID()		{ return m_kUserPetManager.GetSummonedPetUID(); }

	//}}
	//{{ 2011. 06. 23	최육사	대전 개편
	UidType			GetMatchUID()			{ return m_kUserPvpManager.GetMatchUID(); }
	bool			IsRequestMatchMaking() const { return m_kUserPvpManager.IsRequestMatchMaking(); }
	//}}
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
//#ifdef SERV_TOONILAND_CHANNELING_CONNECT_EVENT // SERV_JAPAN_CHANNELING
	int				GetChannelCode() const	{ return static_cast<int>(m_kNexonAccountInfo.m_uChannelCode); }
//#endif SERV_TOONILAND_CHANNELING_CONNECT_EVENT // SERV_JAPAN_CHANNELING
	//}}

	bool			IsSteamUser() const { return ((m_kNexonAccountInfo.m_uChannelCode > 1) && (m_kNexonAccountInfo.m_uChannelCode < 6)); }	// 스팀 유저, 전환 스팀 유저
	bool			IsPureSteamUser() const { return ((m_kNexonAccountInfo.m_uChannelCode == 2) || (m_kNexonAccountInfo.m_uChannelCode == 3)); }

	bool			IsAeriaUser() const { return ((m_kNexonAccountInfo.m_uChannelCode == 6) || (m_kNexonAccountInfo.m_uChannelCode == 7)); }

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
	//}



	//////////////////////////////////////////////////////////////////////////
	// Get Info
	void			GetTutorUnitUIDList( std::vector< UidType >& vecStudentUnitUID );
	void            GetRoomUserInfo( OUT KRoomUserInfo& kRoomUserInfo, IN const int iRoomType = -1 );
	void			GetPartyUserInfo( KPartyUserInfo& kInfo );
	void            GetSquareUserInfo( KSquareUserInfo& kSquareUserInfo );
	//{{ 2008. 11. 4  최육사	광장개편
	void			GetFieldUserInfo( KFieldUserInfo& kFieldUserInfo );
	//}}
	//{{ 2009. 9. 22  최육사	길드
	void			GetGuildMemberInfo( KGuildMemberInfo& kInfo );
	//}}
	//{{ 2009. 11. 10  최육사	PC방 채널이동
	void			GetChannelChangeInfo( KChannelChangeInfo& kInfo );
	//}}
	void			GetConnectionUnitInfo( KConnectionUnitInfo& kInfo );	//유닛 상태 & 정보

	//{{ 2010. 01. 15  최육사	길드게시판
	void			GetApplyJoinGuildInfo( KApplyJoinGuildInfo& kInfo );
	//}}
	//{{ 2011. 06. 23	최육사	대전 개편
	void			GetMatchUserInfo( OUT KMatchUserInfo& kInfo );
	//}}
	//{{ 2012. 02. 07	최육사	배틀필드 시스템
	void			GetAutoPartyUserInfo( OUT KAutoPartyUserInfo& kInfo );
	//}}

	//////////////////////////////////////////////////////////////////////////
	// Check Function
	bool			CheckCharLevelUp();									// 레벨업 체크
	bool			CheckRequireDungeonID( int iRequireDungeonID );
	//{{ 2012. 09. 21   김민성   마을 입장 오류 메시지 구분
	bool			CheckEnterTheVillage( IN int iMapID, OUT int& iLimitLevel, OUT int& iRequireDungeonID );		// 마을 입장 조건
	//}}
	bool			CheckEnterTheVillage( IN const int iMapID );		// 마을 입장 조건
	bool			CheckMaxED( __int64 iAddED );						// 최대ED 검사
	//{{ 2012. 05. 10	최육사	배틀필드 시스템
	bool			CheckForDungeonGameStart( IN const char cDungeonMode,
											  IN const int iDungeonID,
											  IN const char cDifficultyLevel,
											  IN const bool bCheckForAutoParty,
											  IN const bool bSendErrorPopUp );
	bool CheckItemLevel( IN std::map< int, KInventoryItemInfo >& mapEquippedItem, IN int iDungeonIDAndDif );
	//}}

	bool			CheckRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode );
	bool			CheckRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode, int& iRequireItemID, int& iRequireQuantity );

	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	bool			DeleteRequiredItemToEnterDungeonRoom( int iDungeonID, CXSLDungeon::DUNGEON_MODE eDungeonMode, std::vector< KInventoryItemInfo >& vecInventorySlotInfo, OUT int & iItemID );
	//}}

	//{{ 2010. 8. 6	최육사	펫 시스템
	void			CheckAutoFeed();
	bool			AutoFeedInPetInventory( IN bool bOnlyEatSpecial, OUT int& iFeedItemID, OUT std::vector< KInventoryItemInfo >& vecPetInventorySlotInfo );

	//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
	int				CanIUseTheAutoLootingItem( IN const UidType iPetUID );
	void			SetAutoLootingPet( IN const UidType iPetUID );
	//}}

	int				CanIUseTransformItem( IN const UidType iPetUID );
	void			UseTransformItem( IN int iUsedItemID, IN const UidType iPetUID, OUT int& iBeforePetID, OUT int& iAfterPetID );

	//}}

	//{{ 허상형 : [2011/3/23/] //	월드 미션
	void			CheckUpdateWorldMission();
	//}} 허상형 : [2011/3/23/] //	월드 미션

	bool			IsDenyInviteParty()			{ return m_kUserGameOption.IsDenyParty(); }
	bool			IsExistInventoryItem( int iItemID );
	//{{ 2009. 11. 3  최육사	길드
	bool			IsExistInventoryItemWithBank( int iItemID );
	//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
	bool			IsEnoughSpaceExist( IN const std::map< int, int >& mapItem );
	//}}

	bool			IsCompleteQuest( int iQuestID );
	//}}
	//{{ 09.05.20.hoons.
	bool			IsClearDungeon( int iDungeonID, char cDiffLevel );
	//]]
	//{{ 2009. 8. 4  최육사		봉인스킬
	bool			IsSkillUnsealed( int iSkillID ) { return ( m_kSkillTree.IsSkillUnsealed( iSkillID ) ); }
	//}}
	//{{ 2009. 7. 3  최육사		헤니르 시공
	bool			IsHenirDungeonChallengeMode();
	//}}

	//{{ 2009. 6. 10  최육사	퀵슬롯 쿨타임 리셋
	void			QuickSlotCoolTimeReset()	{ m_kInventory.ResetQuickSlotCoolTime(); }
	//}}

	//{{ 2009. 9. 7  최육사		퀘스트 기능 수정
	bool			IsDungeonGetExp( int iDungeonID, char cDifficulty );
	//}}

	//{{ 2010. 05. 12  최육사	대전 던전 서버군 통합
	bool			IsSameServerGroupUID( IN UidType iUID );
	//}}

	bool			IsEnoughItemExist( IN const std::map< int, int >& mapRequiredItem, IN const bool bOnlyInventory );

	//{{ 2010. 01. 11  최육사	서버코드정리
	bool			IsEnoughItemExistOnlyInventory( IN const std::map< int, int >& mapRequiredItem );
	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	bool			IsEnchantAbuser()			{ return m_kUserAbuserManager.IsEnchantAbuser(); }
	void			CheckEDAbuser( IN int iGetEDState, IN int iBeforeED );
	void			CheckItemAbuser( IN int iGetItemState, IN const std::map< int, int >& mapGetItemInfo );
	//}}
	bool			IsChatBlackListUnit( UidType iUnitUID ) { return m_kUserGameOption.IsChatBlackListUnit( iUnitUID ); }
	void			UserStatisticsIncreaseCount( int iTableID, int iSubTableID , int iColumn, int iCount );
	//}}

	//////////////////////////////////////////////////////////////////////////
	// Update Function
	void			SendLeaveParty( int iReason );
	void			SendLeaveField( int iReason );
	void			SendLeavePVPLobby();
	//{{ 2011. 07. 11	최육사	대전 개편
	void			SendLeavePVPMatch( IN const int iReason );
	//}}
	//{{ 2012. 02. 10	최육사	배틀필드 시스템
	void			SendLeaveAutoParty( IN const int iReason );
	//}}

	void			SendUpdatePartyUnitInfo( bool bChangeField = false );
	//{{ 2010. 02. 20  최육사	채널 파티 통합
	void			SendUpdatePartyMemberList( IN UidType iPartyUID, IN const std::set< UidType >& setPartyUIDList );
	//}}
	void			SendUpdateFriendList();
	//{{ 2009. 10. 8  최육사	길드
	void			SendUpdateGuildMemberInfo();
	//}}
	//{{ 2010. 01. 11  최육사	추천인리스트
	void			SendUpdateRecommendUserInfo( IN const KRecommendUserInfo::RECOMMEND_USER_INFO_TYPE eType );
	//}}

	//{{ 2009. 4. 22  최육사	대전유저리스트
	void			SendUpdatePVPUserInfo( char cState );
	//}}
	//{{ 2009. 6. 5  최육사		장착정보 변경
	void			SendUpdateChangeEquippedItem();
	//}}
	//{{ 2010. 02. 08  최육사	PC방 채널 이동 오류 수정
	void			SendUpdateChangeEquippedItem( IN const std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}
	//{{ 2010. 02. 10  최육사	일일 이벤트 퀘스트
	void			SendUpdateDropQuestItemByIngQuest();
	//}}
	//{{ 2010. 8. 19	최육사	펫 시스템
	void			SendPetAction( IN CXSLPetManager::PET_ACTION_COMMAND ePetAction );
	//}}
	//{{ 2013. 05. 29	최육사	제재 리스트 통합
	void			SendCheckBlockUser( IN const KBlockInfo::BLOCK_TYPE eBlockType );
	void			SendCheckBlockUser( IN const std::vector< int >& vecBlockType );
	//}}
	//{{ 2013. 06. 03	최육사	아이템 개편
	void			SendEnchantNotice( IN const int iItemID, IN const int iEnchantLevel, IN const int iRandomItemID = 0 ) const;
	//}}
	//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
	//}}

	//{{ 2011. 03. 02	최육사	개인 거래 차단
	void			HackUserRegRejectedUser( IN const char cReason
		);
	void			HackUserRegTradeBlock( IN const char cReason );
	void			HackUserRegMonitorsUser( IN const char cReason );
	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	void			HackUserKick( IN const KEGS_SERVER_CHECK_HACK_USER_REQ& kPacket );
	//}}
	//}}
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	void			HackUserRegPacketMornitor();
	//}}

	void            Kick( IN const int iNetError );
	void            Kick( IN const KStatistics::ESIColumn_Disconnect_Reason eDisconnectReason );
	static void     DumpMaxLoginTime()  { ms_kMaxLoginTime.Dump(); }

	//{{ 2012. 02. 27	김민성	인벤토리 정렬	- 무거운 기능을 가볍게 변경
	static void     DumpMaxInventorySortTime()  { ms_kMaxInventorySortTime.Dump(); }
	//}}


	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	bool				IsReserveKickUser()							{ return m_kDungeonRequired.m_bState; }
	void				SetReserveKickUser( bool _state )			{ m_kDungeonRequired.m_bState = _state; }
	int					GetDungeonRequiredItemID()					{ return m_kDungeonRequired.m_Itemid; }
	void				SetDungeonRequiredItemID( int itemid )		{ m_kDungeonRequired.m_Itemid = itemid; }
	//}}

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	void				CheckLetterAndTradeMonitoring();
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	void				CheckDeserializeFailCount();
	//}}
	//{{ 2012. 10. 15	최육사		ED&아이템 모니터링 시스템
	//}}
	//{{ 2012. 05. 31	최육사	배틀필드 시스템
	void				CheckAutoPartyBonus();
	//}}

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	void SetIsUseSecondPW( IN const bool& isUse )					{ m_bUseSecondPW = isUse; }
	void SetSecondPWFailedCount( IN const int& iFailedCount )		{ m_iFailedCount = iFailedCount; }
	int	 GetSecondPWFailedCount()									{ return m_iFailedCount; }
	void SetSecondPW( IN const std::wstring& wstrSecondPW )			{ m_wstrSecondPW = wstrSecondPW; }
	void GetSecondPW( OUT std::wstring& wstrSecondPW )				{ wstrSecondPW = m_wstrSecondPW; }
	void SetLastAuthDate( IN const CTime& tLastAuthDate )			{ m_tLastAuthDate = tLastAuthDate; }
	void SetSuccessSecondSecurity( IN const bool& bSuccess )			{ m_bPassSecondSecurity = bSuccess; }
	void SetIsUseOTP( IN const int& iNexonSecurityType );
	bool GetSuccessSecondSecurity();
	bool GetIsUseSecondPW()											{ return m_bUseSecondPW; }
	bool GetIsUseOTP()												{ return m_bIsUseOTP; }
	int	 GetNexonSecurityType()										{ return m_iNexonSecurityType; }
	bool IsUseSecurity()											{ return m_bIsUseOTP || m_bUseSecondPW; }
	bool IsSameSecondSecurityPW( IN const std::wstring& wstrSecondPW );
	bool CheckIntegritySecondSecurityPW( IN const std::wstring& wstrSecondPW, OUT int& iOK );
	void SendSecurityType();
	void SendFirstLoginSecurityInfo();
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	void SetLastLoginDate( IN const std::wstring& wstrLoginDate )       { m_kAccountInfo.m_wstrLastLogin = wstrLoginDate; }
	std::wstring& GetLastLoginDate()									{ return m_kAccountInfo.m_wstrLastLogin; }
	//}}
	int  GetSecurityType();
	//}}

	//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
	void	UseResurrectionStone( IN bool bIsUse )				{ m_kUserTitleManager.UseResurrectionStone( bIsUse ); }
	bool	IsUseResurrectionStone()							{ return m_kUserTitleManager.IsUseResurrectionStone(); }
	void	SetStartedNumMember( IN int iStartedNumMember )		{ m_kUserTitleManager.SetStartedNumMember( iStartedNumMember ); }
	int		GetStartedNumMember()								{ return m_kUserTitleManager.GetStartedNumMember(); }
//{{ 2011. 09. 19  김민성	칭호 획득 조건 추가
	std::wstring&	GetUserID()									{ return m_kAccountInfo.m_wstrID; }
//}}
	//}}
	//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
	void	ClearUseResurrectionStoneCount()							{ m_kUserTitleManager.ClearUseResurrectionStoneCount(); }
	void	AddUseResurrectionStoneCount()								{ m_kUserTitleManager.AddUseResurrectionStoneCount(); }
	int		GetUseResurrectionStoneCount()								{ return m_kUserTitleManager.GetUseResurrectionStoneCount(); }
	//}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	void CheckComeBackUser();
	void AnnunciateComeBackUser();
	void SendFirstLoginComeBackUser( IN KUnitInfo& kUnit, IN bool bFirst );
	void SendComeBackLogOut();
	//}}

	//{{ 2011. 08. 09  김민성 (2011.08.11) 특정일 이후 생성한 계정에 대하여 신규케릭터 생성 시 아이템 지급 이벤트
	void SetAccountInfo( IN const KAccountInfo& AccountInfo )	{ m_kAccountInfo = AccountInfo; }
	void GetAccountInfo( OUT KAccountInfo& AccountInfo )	{ AccountInfo = m_kAccountInfo; }
	//}}

	//{{ 2011. 08. 10    김민성    채널링 SOAP
	//}}

	//{{ 2011. 08. 12   김민성      헤니르 개편
	void CheckHenirRewardCountEvent();
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
	bool GetEDMonitoringMode()									{ return m_kUserHackingManager.GetEDMonitoringMode(); }
	//}}

	//{{ 2012. 06. 13	김민성       선택적 셧다운제
		//{{ 2011. 09. 29  김민성	셧다운제 도입
		//}
		//{{ 2012. 02. 14    김민성   넥슨 안심 로그인 - 목표 위치기반 로그인
		//}}
	//}}

	//{{ 2011. 08. 29	최육사	캐릭터 레벨업 이벤트
	//}}

	//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	bool GetUnitClassChangeInfo( IN int iItemID, OUT KDBE_BUY_UNIT_CLASS_CHANGE_REQ& kPacketToDB );
	//}}

	bool GetUnlimitedClassChangeInfo( IN int iNewClassID, OUT KDBE_BUY_UNIT_CLASS_CHANGE_REQ& kPacketToDB );

	//{{ 2012. 03. 27	김민성		머신 시간을 검사하자(50분 이상 차이나면 모니터링)
	//}}

	//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
	//}}

	//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	void SetRandomTimeKick( int iReason, int iMinTime, int iRange );
	void CheckRandomTimeKick();
	//}}

	//{{ 2012. 05. 29	김민성       개인 상점 거래 로직 변경
	void SetPersonalShopBuyItemInfo( IN KERM_BUY_PERSONAL_SHOP_ITEM_REQ& kPacket );
	void GetPersonalShopBuyItemInfo( OUT KERM_BUY_PERSONAL_SHOP_ITEM_REQ& kPacket );
	//}}
	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	bool IsGuildMaster( void );
	//}}
	//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
	void SetAgencyShopPickUpItemInfo( IN std::vector< UidType >& vecPickUpItemList );
	void GetAgencyShopPickUpItemInfo( OUT std::vector< UidType >& vecPickUpItemList );
	//}}


	//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
	//}}

	//{{ 2011. 10. 14	최육사	아이템 사용 DB 업데이트 수정
	void OnUseItemInInventory( IN const UidType iItemUID
							//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
//#ifdef SERV_PET_AUTO_LOOTING
							 , IN const __int64 iTempCode
//#endif SERV_PET_AUTO_LOOTING
							//}}
							 , IN const bool bNotDeleteItem = false
							 , IN const int iWarpPointMapID = 0
							 );
	//}}

	//{{ 버프 관리 시스템 - 김민성
	void SendUpdateBuffInfo();
	bool ExistActivateBuff( IN const int iBuffID ) { return m_kUserBuffManager.IsBuffActivated( iBuffID ); }
	bool ActivateBuff( IN const int iBuffID, OUT std::vector< KBuffInfo >& vecActivatedBuff, IN const CTime tUseTime = 0, IN int iPeriod = 0 );
	bool DeactivateBuff( IN const int iBuffID, OUT std::vector< int >& vecDeleteBuff );
	bool ActivateItemBuff( IN const int iUseItemID, OUT std::vector< KBuffInfo >& vecActivatedBuff, OUT std::vector<int>& vecDeactiveBuff, IN const CTime tUseTime = 0 );
	void CheckBuffPeriod();
	void CheckDeselectUnitBuff( IN const bool bSendAck = true );
	int GetEquippedTitleID() { return m_kUserTitleManager.GetEquippedTitleID(); }
	//}
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}

	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	void SendUpdateAccountQuestInfo();
	//}}

	void ToggleInvisible() { m_bInvisible = !m_bInvisible; }
	bool IsInvisible() { return m_bInvisible; }

	//{{ 2013. 3. 11	박세훈	 로컬 랭킹 시스템
	//}}

	void CheckMaxLevel();
	void CheckEnableEventDungeon();

	void CheckEnableCodeEvent();

	//{{ 2013. 03. 18	 만우절 이벤트_레인보우 버프 - 김민성
	//}




	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	void CheckRelationshipReward();
	//}

	void CalculatePvpUnitStat( IN int iPVPChannelClass, IN KStat& kModifiedBaseStatBySkill, IN bool bIsDungeonRoom, IN unsigned int uiHP_OnePoint, IN unsigned int uiUnitLevel, IN OUT KRoomUserInfo& kRoomUserInfo );
	void CalculateDungeonUnitStat( IN bool bRevisionDungeon, IN KStat& kModifiedBaseStatBySkill, IN bool bIsDungeonRoom, IN unsigned int uiHP_OnePoint, IN unsigned int uiUnitLevel, IN OUT KRoomUserInfo& kRoomUserInfo, IN OUT KStatIncreaseRate& kStatIncreaseRateInfo );
	void CalculateBattleFieldUnitStat( IN KStat& kModifiedBaseStatBySkill, IN bool bIsDungeonRoom, IN unsigned int uiHP_OnePoint, IN unsigned int uiUnitLevel, IN OUT KRoomUserInfo& kRoomUserInfo, IN OUT KStatIncreaseRate& kStatIncreaseRateInfo );
	void CalculateTrainingCenterUnitStat( IN KStat& kModifiedBaseStatBySkill, IN bool bIsDungeonRoom, IN unsigned int uiHP_OnePoint, IN unsigned int uiUnitLevel, IN OUT KRoomUserInfo& kRoomUserInfo, IN OUT KStatIncreaseRate& kStatIncreaseRateInfo );
	void CalculateSkillUnitStat( IN OUT KRoomUserInfo& kRoomUserInfo, IN OUT KStatIncreaseRate& kStatIncreaseRateInfo );
	void CheckInventoryExpMedal( IN OUT KRoomUserInfo& kRoomUserInfo );



	bool GetEnterCashShop() { return m_kUserUnitManager.GetEnterCashShop(); }

	void SetSteamClientUser( IN bool bVal_ ) { m_bSteamClient = bVal_; }
	bool IsSteamClientUser() const { return m_bSteamClient; }

	void SetAeriaClientUser( IN bool bVal_ ) { m_bAeriaClient = bVal_; }
	bool IsAeriaClientUser() const { return m_bAeriaClient; }

	//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
	void CheckUpdateGlobalMission();
	//}} 2012. 09. 06	임홍락	글로벌 미션 매니저


	//////////////////////////////////////////////////////////////////////////
	// protected member function
protected:
    void			WriteUnitInfoToDB( bool bFinal, int iEventID = 0 );
    void			WritePlayTimeToDB();
    void			SendDisconnectEvent( u_short usEventID = -1 );
	//{{ 2009. 4. 3  최육사		치트로그
	void			WriteCheatLogToDB( const wchar_t* szCheatCode, const std::wstring& wstrCheatContents );
	void			WriteCheatLogToDB( const wchar_t* szCheatCode, const CStringW& cwstrCheatContents );
	void			WriteCheatLogToDB( const wchar_t* szCheatCode );
	//}}
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	void			WriteCharacterLogToDB( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType );
	void			WriteCharacterLogToDBWithDungeonID( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iDungeonID );
	void			WriteCharacterLogToDBWithProductNo( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iProductNo );
	void			WriteCharacterLogToDBWithParam( IN const KELOG_USER_CHARACTER_LOG_NOT::LOG_TYPE eLogType, IN const int iDungeonID, IN const int iProductNo );
	//}}

	//{{ 2012. 11. 9	최육사	배틀필드 시스템
	void			DungeonGameStart_SingleOpenDungeonRoom( IN const char cDungeonMode, IN const int iDungeonID, IN const char cDifficultyLevel, IN const bool bAutoParty );
	//}}

	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}

	//{{ 2009. 6. 12  최육사	데이터 검증
	bool			CheckValidNickName( const std::wstring& wstrNickName );
	//}}

	// Check Function
	void			CheckTimeEvent();									// 접속 시간 이벤트

	//{{ 2009. 10. 14  최육사	자동결제
	void			CheckAutoPayment();									// 자동결제
	//}}
	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	void			CheckHackingToolList();
	//}}
	//{{ 2010. 07. 04  최육사	스킬 포인트 오류 확인

	bool			CheckSkillPointError() const;


	//}}

	//{{ 2011. 12. 22	최육사	배틀필드 시스템
	void			CheckWaitPartyUser();
	//}}


	//{{ 2011. 06. 17	최육사	패킷 이벤트 보안
	bool			CheckIsValidPacketEvent( IN const KEventPtr& spEvent, IN const wchar_t* pszPacketStructName, IN const bool bIsDeserializeSuccess, IN const bool bIsValidPacketID );
	//}}

	bool			CheckingSpirit( IN const int iDungeonID );
	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	bool			DecreaseSpirit( IN const int iDungeonID, IN const char cUserCount, IN const short sStageNpcCount, IN const bool bIsTutorial, IN const bool bIsBattleField );
	//}}


	//{{ 2011. 06. 20	최육사	대전 개편
	bool			CheckDungeonPartyCondition( IN const int iDungeonID,
												IN const char cDifficultyLevel,
												IN const char cDungeonMode,
												IN const bool bCheckLowLevel,
												IN const bool bDontCheckPartyUID );

	bool			CheckPvpPartyCondition( IN const char cPVPGameType, IN const bool bDontCheckPartyUID );
	//}}

	//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
	//}}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	void			SetHackingUserSendType( IN char cType );
	//}}
	//{{ 2012. 11. 13	최육사		자동파티 대기열 카운트 표시
	void			CheckAutoPartyWaitUserCountRequest();
	//}}

	// Check Expire
    void			ExpireItem( bool bForce = false );
	void			ExpireCashSkillPoint( bool bForce = false );
    void			ExpireSkillSlotB( bool bForce = false );
	void			ExpireTitle( bool bForce = false );

	bool			CheckLimitedPlayTimes( IN const int iDungeonID );


	//////////////////////////////////////////////////////////////////////////
	// packet & event send function
public:
	template < typename T > bool SendPacket( unsigned short usEventID, T& data, bool bLogging = true, bool bCompress = false );
	bool SendID( unsigned short usEventID );

    template < class T > void SendToAccountDB( unsigned short usEventID, const T& data ) const;
    template < class T > void SendToGameDB( unsigned short usEventID, const T& data );
    template < class T > void SendToGameDB2nd( unsigned short usEventID, const T& data );
    template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	template < class T > void SendToSMSDB( unsigned short usEventID, const T& data );
	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}
	//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
	//}}
    template < class T > void SendToCnUser( unsigned short usEventID, const T& data );
	//{{ 2009. 4. 24  최육사	대전유저리스트
	template < class T > void SendToCnServer( unsigned short usEventID, const T& data ) const;
	//}}
    template < class T > void SendToLoginAccountDB( unsigned short usEventID, const T& data );
    template < class T > void _SendToCnRoom( UidType nTo, unsigned short usEventID, const T& data );
    template < class T > void SendToCnRoom( unsigned short usEventID, const T& data );
	//{{ 2011. 04. 29	최육사	대리상인
	template < class T > void _SendToLoginRoom( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToLoginRoom( unsigned short usEventID, const T& data );

	template < class T > void _SendToPShopRoom( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToPShopRoom( unsigned short usEventID, const T& data );
	//}}
	//{{ 2010. 02. 20  최육사	채널 파티 통합
	template < class T > void _SendToCnParty( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToCnParty( unsigned short usEventID, const T& data );
	//}}
	template < class T > void _SendToGSField( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToGSField( unsigned short usEventID, const T& data );
	template < class T > void SendToLoginServer( unsigned short usEventID, const T& data );
	template < class T > void _SendToCharacter( UidType nTo, unsigned short usEventID, const T& data );

	//---------------------------------------------------------//

	template < class T > void SendToKOGBillingDB( unsigned short usEventID, const T& data );
	template < class T > void SendToKOGLoginBillingDB( unsigned short usEventID, const T& data );
	template < class T > void SendToPublisherBilling( unsigned short usEventID, const T& data );
	template < class T > void SendToPublisherBillingDB( unsigned short usEventID, const T& data );
	void SendToKOGBillingDB( unsigned short usEventID );
	//---------------------------------------------------------//

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	template < class T > void SendToGlobalServer( unsigned short usEventID, const T& data );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가
	//{{ 2011. 06. 23	최육사	대전 개편
	template < class T > void _SendToMatch( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToMatch( unsigned short usEventID, const T& data );
	//}}
	//{{ 2012. 02. 08	최육사	배틀필드 시스템
	template < class T > void _SendToAutoParty( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToAutoParty( unsigned short usEventID, const T& data );
	//}}

	//{{ 2010. 11. 12 조효진	채팅 로그 관련 추가
	//}}



    void SendToAccountDB( unsigned short usEventID );
    void SendToGameDB( unsigned short usEventID );
    //void SendToGameDB2nd( unsigned short usEventID );
    void SendToLogDB( unsigned short usEventID );
	void SendToSMSDB( unsigned short usEventID );
	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}
	//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
	//}}
    void SendToCnUser( unsigned short usEventID );
	//{{ 2009. 4. 24  최육사	대전유저리스트
	void SendToCnServer( unsigned short usEventID );
	//}}
    void SendToLoginAccountDB( unsigned short usEventID );
	void SendToLoginServer( unsigned short usEventID );
	void SendToCnRoom( unsigned short usEventID );
	//{{ 2011. 04. 29	최육사	대리상인
	void SendToLoginRoom( unsigned short usEventID );
	void SendToPShopRoom( unsigned short usEventID );
	//}}
	//{{ 2010. 02. 20  최육사	채널 파티 통합
	void SendToCnParty( unsigned short usEventID );
	//}}
	void SendToGSField( unsigned short usEventID );

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	void SendToGlobalServer( unsigned short usEventID );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가


	int GetTheNumberOfSkillPagesAvailable() const { return m_kSkillTree.GetTheNumberOfSkillPagesAvailable(); }


	//////////////////////////////////////////////////////////////////////////
	// event handler
protected:
#   undef EVENT_HANDLER
#   define EVENT_HANDLER( eventid ) DECL_ON_FUNC( eventid );
#	undef EVENT_HANDLER_NOPARAM
#   define EVENT_HANDLER_NOPARAM( eventid ) DECL_ON_FUNC_NOPARAM( eventid );
#	undef _EVENT_HANDLER
#	define _EVENT_HANDLER( eventid, structtype ) _DECL_ON_FUNC( eventid, structtype );

	// 여기서 부터 GSUser Event Handler의 헤더 파일을 추가합니다.
#	include "GSUserSession.h"
#	include "GSUserSecurity.h"
#	include "GSUserGameCommon.h"
#	include "GSUserField.h"
	//{{ 2011. 12. 05	최육사	배틀필드
#	include "GSUserNewParty.h"
	//}}
#	include "GSUserInventory.h"
#	include "GSUserMessenger.h"
#	include "GSUserRoomCommon.h"
#	include "GSUserDungeon.h"
#	include "GSUserPvp.h"
#	include "GSUserTrade.h"
#	include "GSUserPersonalShop.h"
#	include "GSUserCashShop.h"
#	include "GSUserPet.h"
#	include "GSUserGuild.h"
#	include "GSUserLetter.h"
#	include "GSUserAdminCheat.h"
#	include "GSUserMatch.h"
	//{{ 2011. 10. 27	최육사	배틀필드 시스템
#	include "GSUserBattleField.h"
#	include "GSUserAutoParty.h"
	//}}
//#ifdef SERV_COUNTRY_CN
#	include "GSUserGiant.h"
//#endif SERV_COUNTRY_CN
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
#	include "GSUserRelationship.h"
	//}
	//////////////////////////////////////////////////////////////////////////
	// event handling function, util function
protected:
	bool RoutePacket( const KEvent* pkEvent );	// CnUser, GSUser가 같이 선언하지만 상속은 아님. ^ KEvent가 수정되지 않아야 한다. (SmartPtr로는 불가능)

    // override.
    virtual void ProcessEvent( const KEventPtr& spEvent );
	bool ProcessEventExpand( const KEventPtr& spEvent );
    virtual void OnDestroy();   // LOGINED 상태로 빠져나오고, DB data update를 신청한다
    virtual bool CheckExceedRefCount( int nCount ); // derived from KSession.

	//{{ 2009. 5. 29  최육사	함수 통합
	bool OnConnectReq( IN const KEGS_CONNECT_REQ& kReq, OUT KEGS_CONNECT_ACK& kAck );
	bool OnNexonUserAuthenticateAck( IN OUT KELG_NEXON_USER_AUTHENTICATE_ACK& kAck_, OUT KEGS_VERIFY_ACCOUNT_ACK& kAck );
	//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
	bool OnAccountSelectUnitAck( IN OUT KDBE_SELECT_UNIT_ACK& kAck_, IN bool bIsChannelChange );
	void OnSelectUnitSuccess( IN const KDBE_SELECT_UNIT_ACK& kAck_, IN bool bIsChannelChange );
	void SendSelectUnitACK( IN const KDBE_SELECT_UNIT_ACK& kPacket );
	//}}
	//}}
	//{{ 2012. 11. 22	최육사		캐릭터 선택 해제 코드 리팩토링
	void OnDeselectUnit( IN const u_short usEventID, IN const bool bOnDestroy );
	//}}

	//2008.01.29 hoons.. 반복이벤트 체크를 위한 임시함수.
	void CheckRepeatEvent( u_short usEventID );

	// 스킬개편
	void InitTransactionEventFilter();
	bool IsTransactionEnabled( unsigned short usEventID );
	void CheckTransactionReqEvent( unsigned short usEventID );
	void CheckTransactionAckEvent( unsigned short usEventID );

	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}}


	void OnBuyCashItemAck( IN const KEGS_BUY_CASH_ITEM_ACK& kPacket_ );
	int OnGetPurchasedCashItemReq( IN const KEGS_GET_PURCHASED_CASH_ITEM_REQ& kPacket_, IN const KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ& kPacket2_ );

	bool IsDirectPickUpItem( IN const int iItemID );
	int OnPickUpAck( IN const KENX_BT_NISMS_INVENTORY_PICK_UP_ONCE_ACK& kPacket_, IN const KEBILL_PICK_UP_ACK& kPacket2_, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	std::wstring MakeOrderID();
	bool IsAbleToUseCashType(int iCashType);	// 국가별로 사용가능한 캐시 종류가 다름

	//{{ 2012. 04. 30	박세훈	현자의 주문서 접속 이벤트 ( 우편함 중복 체크 )
	//}}

	//{{ 2012. 04. 17	최육사	퀘스트 조건 마을 입장
	void AddTempInventoryItem( IN const int iItemID
							 , IN const u_char ucSealData
							 //{{ 2012. 1. 16	박세훈	던전 및 필드 드랍 아이템에 강화 부여
							 , IN const KItemInfo& kItemInfo
								//}}
							 );
	void SendQuestRewardLetter( IN const int iQuestID, IN const int iRewardItemID, IN const int iQuantity );
	//}}

	//{{ QUEST 개편 - 김민성
	int GetDungeonIDAndDungeonDifficulty()		{ return static_cast<int>( m_kUserDungeonManager.GetDungeonID() + m_kUserDungeonManager.GetDungeonDifficulty() ); }
	//}}
//{{ 2012. 09. 25	박세훈	빙고 이벤트
	//}}

	void SendExpandSkillPageReqToGameDB( const unsigned short usEventID_, const int iTheNumberOfSkillPagesToBeAdded_ );


	//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
	int		GetSlotSize( IN const int iCategory ) const	{	return m_kInventory.GetSlotSize( iCategory );	}
	int		GetExpandedSlotSize( IN const int iCategory ) const	{	return m_kInventory.GetExpandedSlotSize( iCategory );	}
	//}}

	void			SetWarpVipEndDate( IN __int64 trDate_ ) { m_trWarpVipEndDate = trDate_; }


	void	CheckRidingPetProcess( void );

	void			CheckExpirePet();

private:

//---------------------------------------------------------------------------------------------------//
//----------- 2013.02.27 조효진	해외 기본 구조 작업 (SERV_GLOBAL_BASE)-------------------------------//
//---------------------------------------------------------------------------------------------------//

	KGlobalCashInfo					m_GlobalCashInfo;

	//#ifdef SERV_PERSONAL_SHOP_NO_MOVE	// 2012.07.03 lygan_조성욱 // 개인거래에 물품 등록된거 관리
	std::vector< KSellPersonalShopItemInfo >	m_vecPersonalShopSell_ItemInfo;
	//#endif //SERV_PERSONAL_SHOP_NO_MOVE

	CTime							m_tReleaseTradeBlockTime;

	// 2011.07.25 lygan_조성욱 // 중국용 커뮤니티탭 유저 정보 찾기
	int								m_cCommunity_Tap_type;



	__int64							m_trWarpVipEndDate;


	int								m_iGlobalMissionTickCount;
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//




	bool	isCanUseUnsealCashSkillItem( IN int iItemID );

	bool	isCanUseUnsealFinalitySkill( IN int iItemID, IN u_char ucLevel );

	void	GetBossFieldCreateInfo( IN const int iBattleFieldID, OUT KBossFieldCreateInfo& kBossFieldCreateInfo ) const;

	void	CreateBossFieldJoinInfo( OUT KBossFieldJoinInfo& kBossFieldJoinInfo ) const;
	void	SetBossFieldJoinInfo( IN const KBossFieldJoinInfo& kBossFieldJoinInfo )	{	m_kBossFieldJoinInfo = kBossFieldJoinInfo;	}
	void	GetBossFieldJoinInfo( OUT KBossFieldJoinInfo& kBossFieldJoinInfo ) const;

	void	AddLogWhenSkillPagesNumberIsWrong( const WCHAR* wszLog_ );



private:
    UidType                         m_nUnitUID;
	u_int							m_uiKNMSerialNum;		// 넥슨코리아 메신져 사용을 위한 시리얼 번호
    char                            m_cUnitClass;
    std::wstring                    m_wstrNickName;
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	//}}

	//{{ 2012. 09. 28	최육사		ED&아이템 모니터링 시스템
	KCacheDataMaxLimit< int, __int64 > m_iED;
	//}}

    u_char                          m_ucLevel;
	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}

	KUserExpManager					m_kEXP;



	//{{ 2011. 06. 20	최육사	대전 개편
	KUserPvpManager					m_kUserPvpManager;
	//}}
	//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
	std::set< UidType >				m_setMyUnitUIDList;
	//}}
	//{{ 2012. 02. 21	최육사	배틀필드 시스템
	KUserUnitManager				m_kUserUnitManager;
	//}}

	//{{ 2012. 03. 20	최육사	배틀필드 시스템
	KUserSpiritManager				m_kUserSpiritManager;
	//}}

	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

    int                             m_nStraightVictories;
    u_short                         m_usInventorySize;
    u_char                          m_ucQuickSlotSize;

    KStat							m_kStat;

    KInventory                      m_kInventory;
	//{{ 2009. 5. 30  최육사	임시 인벤토리
	KTempInventory					m_kTempInventory;
	//}}
	int                             m_nUnitSlot;            // 캐릭터 슬롯 개수

	//{{ 2012. 01. 26	최육사	배틀필드 시스템
	KLastPositionInfo				m_kLastPos;
	//}}

	//{{ 2011. 06. 20	최육사	대전 개편
	//}}

	KCacheData< int >               m_iNumResurrectionStone;// 부활석.
	//{{ 2009. 10. 14  최육사	자동결제 부활석
	KCacheData< int >               m_iNumAutoPaymentResStone;// 자동결제 부활석.
	CTime							m_tAutoPaymentCheckTime; // 자동결제 빌링패킷 전송예약시각
	//}}

	UINT							m_uiRoomListID;
	UidType                         m_iRoomUID;
	//{{ 2012. 03. 21	최육사	배틀필드 시스템
	UidType							m_iBeforeRoomUID;
	//}}

	//{{ 2011. 12. 20	최육사	배틀필드 시스템
	KUserPartyManager				m_kUserPartyManager;
	//}}

	UidType							m_iTradeUID;
	UidType							m_iPersonalShopUID;
	//{{ 2008. 11. 4  최육사	광장개편
	UidType							m_iFieldUID;
	//}}

	KTimerManager					m_kTimer[TM_TOTAL_NUM];
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	//}}

    CTime                           m_tConnectTime;
	CTime							m_tCurUnitLoginTime_INT;

    std::map< int, KDungeonClearInfo >	m_mapDungeonClear;
	std::map< int, KTCClearInfo >		m_mapTCClear;
	std::map< int, KDungeonPlayInfo >	m_mapDungeonPlay;

	//{{ 2011. 08. 09  김민성 (2011.08.11) 특정일 이후 생성한 계정에 대하여 신규케릭터 생성 시 아이템 지급 이벤트
	KAccountInfo					m_kAccountInfo;
	//}}

    KNexonAccountInfo               m_kNexonAccountInfo;
	KAccountBlockInfo				m_kAccountBlockInfo;	//080318.hoons.. 접속시 블록유저면 받아 놓는다.

    bool                            m_bLoginFail;
    bool                            m_bLogoutFail;

	KUserQuestManager				m_kUserQuestManager;

	//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	KUserAccountQuestManager				m_kUserAccountQuestManager;
	//}}

	KUserSkillTree					m_kSkillTree;

	KUserGameOption					m_kUserGameOption;

    // 로그인서버에 접속 종료를 알릴지 결정한다.
    // 이중접속감지나 또는 채널이동으로 끊기는 경우 로그인서버에 접속 종료를 알릴 필요가 없다.
    bool                            m_bReportDisconnectToLogin;
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	//}}

	// korea nProtect 인증모듈 사용변수.
	//bool							m_bnProtectBegin;
	//KGGAuth						m_kGGAuth;

	//{{ 2009. 8. 18  최육사	hack shield
	// Port: m_kHSAuth (HackShield) and m_cXTrapCSM (XTrap) removed with the anti-cheat (T10)
	//}}
	// 지헌 : XTRAP 서버 - CXTrapCSM 선언

	//{{ 2010. 01. 29  최육사	PC방 상용화
	KUserPcBangManager				m_kUserPcBangManager;
	//}}

	bool							m_bSC1;
	bool							m_bSC2;

	// 유저통계
	KUserStatistics					m_kUserStatistics;

    static KMaxProcessTime          ms_kMaxLoginTime;

	KRepeatEvent					m_kRepeatEvent;

	double							m_dResolveJackpotTime;	// 분해 대박

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	KUserHackingManager				m_kUserHackingManager;
	//}}

	//080405. hoons. 개인사제 데이터 관리
	KUserTutorialInfo				m_kUserTutorialInfo;

	// 장바구니
	KUserWishList					m_kUserWishList;

	// 시간 이벤트
	KUserEventManager				m_kUserEventManager;

	// 어뷰저 여부
	//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저
	KUserAbuserManager				m_kUserAbuserManager;
	//}}
	char							m_cHackingUserType;

	// 캐쉬인벤
	KUserCashInventory				m_kUserCashInventory;

	//{{ 2009. 7. 7  최육사		랭킹개편
	KUserRankingManager				m_kUserRankingManager;
	//}}

	//{{ 2010. 9. 6	최육사	중폭 패킷 필터 개선
//#ifdef SERV_REPEAT_FILTER_REFAC
	KRepeatEventFilterNew			m_kRepeatEventFilter;
//#endif SERV_REPEAT_FILTER_REFAC
	//}}

	KTransactionGuarantor			m_kTransactionGuarantor;

	//{{ 2008. 9. 18  최육사	우편함
	KUserLetterBox					m_kUserLetterBox;
	//}}

	//{{ 2008. 10. 6  최육사	타이틀
	KUserTitleManager				m_kUserTitleManager;
	//}}

	//{{ 2008. 12. 9  최육사	필드 옵션
	KFieldOption					m_kFieldOption;
	//}}

	//{{ 2009. 4. 7  최육사		친구정보
	std::map< std::wstring, UidType > m_mapFriendList;
	//}}

    KMessengerInfo                  m_kMessengerInfo;

	//{{ 2009. 9. 13  최육사	유저 로그
	KUserLogManager					m_kUserLogManager;
	//}}

	//{{ 2009. 9. 22  최육사	길드
	KUserGuildManager				m_kUserGuildManager;
	//}}

	//{{ 2010. 01. 08  최육사	추천인리스트
	KUserRecommendManager			m_kUserRecommendManager;
	//}}

	//{{ 2010. 07. 21  최육사	펫 시스템
	KUserPetManager					m_kUserPetManager;
	//}}

	//{{ 2011. 02. 21  김민성	특정 던전 입장 아이템 버그 수정
	KDungeonRequired				m_kDungeonRequired;
	//}}

	//{{ 2011. 04. 11	최육사	대리 상인
	KUserPersonalShopManager		m_kUserPersonalShopManager;
	//}}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	int								m_iWorldMissionTickCount;
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 05. 02  김민성	2차 보안 시스템
	bool							m_bUseSecondPW;				// 2차 보안을 사용 중인가?
	std::wstring					m_wstrSecondPW;				// 2차 비밀번호
	int								m_iFailedCount;				// 2차 보안 실패 횟수
	CTime							m_tLastAuthDate;			// 최근 인증 받은 시간
	bool							m_bPassSecondSecurity;		// 2차 보안 인증 성공
	bool							m_bIsUseOTP;				// OTP 사용 여부
	int								m_iNexonSecurityType;		// 게임 접속시 사용한 넥슨 보안
	KUserUnitSelectRewardManager	m_kUserUnitSelectRewardManager;	// 접속시 지급 아이템
	//{{ 2011. 08. 09  김민성 KAccontInfo 추가로 삭제됨
	//}}
	//}}

	//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
	KComeBackUserManager			m_kComeBackUserInfo;
	//}}

	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{ 2011. 08. 09	최육사	머신ID 블럭기능
	KUserSecurityManager			m_kUserSecurityManager;
	//}}

	//{{ 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템
	//}} 박교현 : [2010/03/09] //	중국 중독방지 방침미 시스템



	std::vector<int>				m_veciPackageBuyCheck; //2012.01.11 lygan_조성욱 // 캐쉬샵에서 동일 품목의 기간제 아이템 묶음에 대해서 한번만 캐릭터 인벤토리에 들어오게 체크용

	//{{ 2011. 08. 10    김민성    채널링 SOAP
	//}}

	//{{ 2011. 08. 12   김민성      헤니르 개편
	KUserDungeonManager				m_kUserDungeonManager;
	//}}
	//{{ 2012. 02. 27	김민성	인벤토리 정렬	- 무거운 기능을 가볍게 변경
	static KMaxProcessTime          ms_kMaxInventorySortTime;
	//}}

	//{{ 2012. 03. 14	김민성		패킷 시퀀스 카운트 체크
	bool						m_bCheckSequence;
	//}}
	//{{ 버프 관리 시스템 - 김민성
	KUserServerBuffManager			m_kUserBuffManager;
	//}

	//{{ 2012. 05. 16	박세훈	첫 접속 시 가이드 라인 띄워주기
	//}}

		//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
		//}}


		//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
		//}}

		//{{ 2012. 09. 22	박세훈	빙고 이벤트
		//}}
		//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
	KUserCheatManager				m_kUserCheatManager;
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}

		bool							m_bInvisible;

		//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
		//}}

	bool							m_bHeroPvpUser;

		int								m_iMaxLevelTickCount;
		int								m_iActiveDungeonTickCount;


		int								m_iCodeEventTickCount;






		bool							m_bSteamClient;

		bool							m_bAeriaClient;




	KUserRidingPetManager				m_kUserRidingPetManager;

	KAfterWorkStorage					m_kAfterWorkStorage;

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	KUserRelationshipManager			m_kUserRelationshipManager;
	//}






	int									m_iRidingPetSummon;
	USHORT								m_usRidingSummonedPetID;

	bool								m_bCouple;
	UidType								m_iRelationTargetUserUid;
	std::wstring						m_wstrRelationTargetUserNickname;

	CXSLDungeonSubStage::NextStageData	m_kNextStageData;


	u_char								m_ucOldYearMissionRewardedLevel;
	int									m_iNewYearMissionStepID;


	KBossFieldJoinInfo					m_kBossFieldJoinInfo;






};

template < typename T >
bool KGSUser::SendPacket( unsigned short usEventID, T& data, bool bLogging, bool bCompress )
{
	//{{ 2010. 9. 2	최육사	중폭 패킷 필터 개선
//#ifdef SERV_REPEAT_FILTER_REFAC
	m_kRepeatEventFilter.CheckAckEvent( usEventID );
//#endif SERV_REPEAT_FILTER_REFAC
	//}}

	CheckTransactionAckEvent( usEventID ); // EGS_

	//{{ 2009. 9. 13  최육사	유저로그
	m_kUserLogManager.SaveSendPacket( usEventID );
	//}}

    return KSession::SendPacket( PI_GS_PROXY, GetUID(), NULL, usEventID, data, bLogging, bCompress );
}

template < class T >
void KGSUser::SendToAccountDB( unsigned short usEventID, const T& data ) const
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_ACCOUNT_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToGameDB( unsigned short usEventID, const T& data )
{
	if( false == IsTransactionEnabled( usEventID ) )
		return;
	CheckTransactionReqEvent( usEventID ); // DBE_

    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_GS_GAME_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToGameDB2nd( unsigned short usEventID, const T& data )
{
    if( false == IsTransactionEnabled( usEventID ) )
        return;
    CheckTransactionReqEvent( usEventID ); // DBE_

    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_GS_GAME_DB_2ND, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToLogDB( unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_LOG_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToSMSDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_GS_SMS_DB, 0, anTrace, usEventID, data );
}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
//}}

template < class T >
void KGSUser::SendToCnUser( unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_CN_USER, 0, anTrace, usEventID, data );
}

//{{ 2009. 4. 24  최육사	대전유저리스트
template < class T >
void KGSUser::SendToCnServer( unsigned short usEventID, const T& data ) const
{
	UidType anTrace[2] = { 0, -1 }; // Trace가 0이면 같은ip의 센터로 전송한다
	KncSend( PI_GS_USER, GetUID(), PI_CN_SERVER, 0, anTrace, usEventID, data );
}
//}}

template < class T >
void KGSUser::SendToLoginAccountDB( unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { GetUID(), -1 };
    KncSend( PI_GS_USER, GetUID(), PI_LOGIN_ACCOUNT_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::_SendToCnRoom( UidType nTo, unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { GetCharUID(), -1 };
    KncSend( PI_GS_USER, GetCharUID(), PI_CN_ROOM, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToCnRoom( unsigned short usEventID, const T& data )
{
    _SendToCnRoom( GetRoomUID(), usEventID, data );
}

//{{ 2011. 04. 29	최육사	대리상인

template < class T >
void KGSUser::_SendToLoginRoom( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetCharUID(), -1 };
	KncSend( PI_GS_USER, GetCharUID(), PI_LOGIN_ROOM, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToLoginRoom( unsigned short usEventID, const T& data )
{
	_SendToLoginRoom( GetRoomUID(), usEventID, data );
}

template < class T >
void KGSUser::_SendToPShopRoom( UidType nTo, unsigned short usEventID, const T& data )
{
	const CXSLRoom::ROOM_TYPE eRoomType = CXSLRoom::GetRoomType( nTo );
	switch( eRoomType )
	{
	case CXSLRoom::RT_PERSONAL_SHOP:
		_SendToCnRoom( nTo, usEventID, data );
		break;

	case CXSLRoom::RT_PSHOP_AGENCY:
		_SendToLoginRoom( nTo, usEventID, data );
		break;

	default:
		START_LOG( cerr, L"개인상점 방이 아닌데 패킷을 보내려고 하네?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetPersonalShopUID() )
			<< BUILD_LOG( eRoomType )
			<< END_LOG;
		break;
	}
}

template < class T >
void KGSUser::SendToPShopRoom( unsigned short usEventID, const T& data )
{
	const CXSLRoom::ROOM_TYPE eRoomType = CXSLRoom::GetRoomType( GetPersonalShopUID() );
	switch( eRoomType )
	{
	case CXSLRoom::RT_PERSONAL_SHOP:
		_SendToCnRoom( GetPersonalShopUID(), usEventID, data );
		break;

	case CXSLRoom::RT_PSHOP_AGENCY:
		_SendToLoginRoom( GetPersonalShopUID(), usEventID, data );
		break;

	default:
		START_LOG( cerr, L"개인상점 방이 아닌데 패킷을 보내려고 하네?" )
			<< BUILD_LOG( GetCharUID() )
			<< BUILD_LOG( GetPersonalShopUID() )
			<< BUILD_LOG( eRoomType )
			<< END_LOG;
		break;
	}
}

//}}

//{{ 2010. 02. 20  최육사	채널 파티 통합

template < class T >
void KGSUser::_SendToCnParty( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetCharUID(), -1 };
	KncSend( PI_GS_USER, GetCharUID(), PI_CN_PARTY, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToCnParty( unsigned short usEventID, const T& data )
{
	_SendToCnParty( GetPartyUID(), usEventID, data );
}

//}}

template < class T >
void KGSUser::_SendToGSField( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetCharUID(), -1 };
	KncSend( PI_GS_USER, GetCharUID(), PI_GS_FIELD, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToGSField( unsigned short usEventID, const T& data )
{
	_SendToGSField( GetFieldUID(), usEventID, data );
}

template < class T >
void KGSUser::SendToLoginServer( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_LOGIN_USER, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::_SendToCharacter( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { -1, -1 }; // trace는 비운다, ProxyManager::SendPacket()을 호출하지 않기 위해
	KncSend( PI_GS_CHARACTER, GetCharUID(), PI_GS_CHARACTER, nTo, anTrace, usEventID, data );
}


template < class T >
void KGSUser::SendToKOGBillingDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_GS_KOG_BILLING_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToKOGLoginBillingDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_LOGIN_KOG_BILLING_DB, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToPublisherBilling( unsigned short usEventID, const T& data )
{
	// 원래 kncSend 로 해결하려고 했으나 PI 값을 오버로딩 하기 위해서는
	// DBManager처럼 kncSend에서 목적지를 알수 있도록 map으로 목적지 관리가 필요하다.
	// 현재 그런 구조로 확장하기에는 시간이 많이 걸려 아래처럼 대체 함수를 이용한 Queueing으로 처리함.
	// 단, PublisherBilling의 목적지가 한군데가 아닌 여러개라면 이구조로 커버 불가능함.

	UidType anTrace[2] = { GetUID(), -1 };
	KncSendToPublisherBilling( PI_GS_USER, GetUID(), PI_GS_PUBLISHER_BILLING, 0, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToPublisherBillingDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_GS_PUBLISHER_BILLING_DB, 0, anTrace, usEventID, data );
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
template < class T >
void KGSUser::SendToGlobalServer( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetUID(), -1 };
	KncSend( PI_GS_USER, GetUID(), PI_GLOBAL_USER, 0, anTrace, usEventID, data );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 06. 23	최육사	대전 개편
template < class T >
void KGSUser::_SendToMatch( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetCharUID(), -1 };
	KncSend( PI_GS_USER, GetCharUID(), PI_GLOBAL_MATCH_MAKING, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToMatch( unsigned short usEventID, const T& data )
{
	_SendToMatch( GetMatchUID(), usEventID, data );
}
//}}

//{{ 2012. 02. 08	최육사	배틀필드 시스템
template < class T >
void KGSUser::_SendToAutoParty( UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { GetCharUID(), -1 };
	KncSend( PI_GS_USER, GetCharUID(), PI_GLOBAL_AUTO_PARTY, nTo, anTrace, usEventID, data );
}

template < class T >
void KGSUser::SendToAutoParty( unsigned short usEventID, const T& data )
{
	_SendToAutoParty( m_kUserDungeonManager.GetAutoPartyUID(), usEventID, data );
}
//}}

//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
//}}
