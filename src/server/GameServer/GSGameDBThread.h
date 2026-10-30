#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below

#include "SubThread.h"
#include "ODBC/Odbc.h"
//{{ 2009. 8. 13  최육사	sp profiler
#include "ODBC/DBUtil.h"
//}}
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

//{{ 2009. 10. 6  최육사	길드
#include "Enum/Enum.h"
//}}
#include "../Common/ODBC/StoredProcedure.h"

class KGSGameDBThread : public KSubThread
{
    DeclareDBThreadWithSP( KGSGameDBThread );
	DeclareSPProfiler;

public:
    KGSGameDBThread();
    virtual ~KGSGameDBThread(void);

protected:
    // derived from KSubThread
    virtual void ProcessEvent( const KEventPtr& spEvent ); // Port: `inline` dropped, it is defined in the .cpp

protected:
    KStoredProcedure m_kSP;

    // DB query function
	//{{ 2009. 5. 28  최육사	채널이동
	//{{ 2012. 12. 10  캐릭터 선택 패킷 분할 - 김민성
	bool Query_SelectUnit( IN const UidType iUserUID, IN const KEGS_SELECT_UNIT_REQ& kReq, OUT KDBE_SELECT_UNIT_ACK& kAck );
	//}}
	//}}
	//{{ 2010. 9. 29	최육사	아이템 정보 리팩토링
	bool Query_GetInventory( IN const UidType iUnitUID,
							 OUT std::map< int, int >& mapInventorySlotSizeResult,
							 OUT std::map< UidType, KInventoryItemInfo >& mapItemResult,
							 OUT std::map< UidType, std::map< UidType, KInventoryItemInfo > >& mapPetItemResult
							 );
	//}}
	//{{ 2010. 9. 8	최육사	아이템 획득 사유
	bool Query_InsertItemList( IN const SEnum::GET_ITEM_REASON eGetItemReason, IN const UidType iUnitUID, IN const std::vector< KItemInfo >& vecItemInfo, OUT std::map< UidType, KItemInfo >& mapInsertedItemInfo, IN const bool bUnSeal = true );
//#else
//	bool Query_InsertItemList( IN const UidType iUnitUID, IN const std::vector< KItemInfo >& vecItemInfo, OUT std::map< UidType, KItemInfo >& mapInsertedItemInfo, IN const bool bUnSeal = true );
	//}}
	void Query_UpdateSealItem( IN const std::set< int >& setSealItem, IN OUT std::map< UidType, KItemInfo >& mapInsertedItemInfo );
    void Query_UpdateDungeonClear( UidType iUnitUID, std::map< int, KDungeonClearInfo >& mapDungeonClearInfo, std::vector< int >& vecFailed );
	void Query_UpdateDungeonPlay( IN UidType iUnitUID, IN std::map< int, KDungeonPlayInfo >& mapDungeonPlayInfo );
    void Query_UpdateTCClear( UidType iUnitUID, std::map< int, KTCClearInfo >& mapTCClearInfo, std::vector< int >& vecFailed );

    bool Query_UpdateItemQuantity( IN UidType iUnitUID, IN std::map< UidType, int >& mapUpdated, OUT std::map< UidType, int >& mapFailed, OUT bool& bUpdateFailed );
	bool Query_UpdateItemEndurance( IN UidType iUnitUID, IN std::map< UidType, int >& mapUpdated, OUT std::map< UidType, int >& mapFailed );
	//{{ 2010. 8. 3	최육사	펫 시스템
	void Query_UpdateItemPosition( IN UidType iUnitUID, IN std::map< UidType, KItemPosition >& mapUpdated, OUT std::map< UidType, KItemPosition >& mapFailed );
	//}}
	//{{ 2009. 12. 15  최육사	아이템 삭제사유
    void Query_DeleteItem( IN std::vector< KDeletedItemInfo >& vecDeleted, OUT std::vector< KDeletedItemInfo >& vecFailed );
	//}}

	//{{ 2009. 10. 28  최육사	길드레벨
	bool Query_GetGuildMemberGrade( IN UidType iUnitUID, OUT u_char& ucGuildMemberGrade );
	//}}



	//{{ 2012. 10. 23	최육사		DB업데이트 코드 리팩토링
	bool Query_UpdateUnitInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq, OUT KDBE_UPDATE_UNIT_INFO_ACK& kAck );
	bool Query_UpdatePvpInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq, OUT KDBE_UPDATE_UNIT_INFO_ACK& kAck );
	bool Query_UpdatePvpInfo( IN const KDBE_QUIT_USER_PVP_RESULT_UPDATE_NOT& kNot );
	bool Query_UpdateLastGamePoint( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateResurrectionStone( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq, OUT KDBE_UPDATE_UNIT_INFO_ACK& kAck );
	bool Query_UpdateQuestInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateTitleInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateSkillSlotInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateCommunityOption( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateWishList( IN const UidType iUserUID, IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateRankingInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateGuildInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateConnectTimeEventInfo( IN const UidType iUserUID, IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdatePetInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq, OUT KDBE_UPDATE_UNIT_INFO_ACK& kAck );
	bool Query_UpdateGameCountInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateHenirRewardCount( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	//{{ 2012. 10. 23	최육사	배틀필드 시스템
	bool Query_UpdateLastPosition( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	bool Query_UpdateGamePlayStatus( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	//}}
	//{{ 2012. 05. 6	박세훈	어둠의 문 개편
	bool Query_UpdateBuffEffect( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	//}}
	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}

	bool Query_UpdateRidingPetInfo( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq, OUT KDBE_UPDATE_UNIT_INFO_ACK& kAck );




	//}}
	//{{ 2013. 07. 08	최육사	공유 은행
	bool Query_UpdateShareItem( IN const KDBE_UPDATE_SHARE_ITEM_REQ& kReq, OUT KDBE_UPDATE_SHARE_ITEM_ACK& kAck );
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	bool Quety_WeddingLetter( IN UidType iSendUserUID, IN KDBE_WEDDING_PROPOSE_REQ& kPacket_, IN OUT KDBE_WEDDING_PROPOSE_ACK& kAck );
	//}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	bool Query_UpdateLastLogOffDate( IN const KDBE_UPDATE_UNIT_INFO_REQ& kReq );
	//}

	void LogAboutCheckingTooManySkillDatas( IN const KDBE_EXPIRE_CASH_SKILL_POINT_REQ& kPacket_ ) const;
// 	void DoQueryUpdatingSkillInfo( OUT int& iOk_, IN const UidType lastSenderUID_, IN const UidType iUnitUID_,
// 		IN const KRetrievedSkillPageData& retrievedSkillPageData_ );

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );

	//{{ 2009. 10. 30  최육사	길드레벨
	template < class T > void SendToLoginServer( unsigned short usEventID, const T& data );
	//}}

	//{{ 2010. 11. 25	조효진	추가 수정 임규수 일본 추가
//#ifdef SERV_SELECT_UNIT_NEW
	// 2014.02.28 이지헌 SendToLogDB 는 SERV_SELECT_UNIT_NEW 없어도 사용하는 곳이 있으므로 디파인 제거 했습니다.
	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	void SendToLogDB( unsigned short usEventID );
//#endif SERV_SELECT_UNIT_NEW
	//}}


protected:
    // event handler
    DECL_ON_FUNC( DBE_UPDATE_UNIT_INFO_REQ );

	_DECL_ON_FUNC( DBE_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ, std::wstring );

   //{{ 2011. 08. 09  김민성 (2011.08.11) 특정일 이후 생성한 계정에 대하여 신규케릭터 생성 시 아이템 지급 이벤트
   DECL_ON_FUNC( DBE_GAME_CREATE_UNIT_REQ );
   //}}
   _DECL_ON_FUNC( DBE_GAME_DELETE_UNIT_REQ, KEGS_DELETE_UNIT_REQ );
   //{{ 2012.02.20 조효진	캐릭터 삭제 프로세스 변경 (삭제 대기 기간 도입)
   _DECL_ON_FUNC( DBE_GAME_FINAL_DELETE_UNIT_REQ, KEGS_FINAL_DELETE_UNIT_REQ );
   _DECL_ON_FUNC( DBE_GAME_RESTORE_UNIT_REQ, KEGS_RESTORE_UNIT_REQ );
   //}}

   _DECL_ON_FUNC( DBE_GAME_SELECT_UNIT_REQ, KEGS_SELECT_UNIT_REQ );
	//{{ 2009. 5. 28  최육사	채널이동
	DECL_ON_FUNC( DBE_CHANNEL_CHANGE_GAME_SELECT_UNIT_REQ );
	//}}

   _DECL_ON_FUNC( DBE_ADMIN_MODIFY_UNIT_LEVEL_REQ, KEGS_ADMIN_MODIFY_UNIT_LEVEL_REQ );
   _DECL_ON_FUNC( DBE_ADMIN_CHANGE_ED_REQ, KEGS_ADMIN_CHANGE_ED_REQ );

    DECL_ON_FUNC( DBE_BUY_ED_ITEM_REQ );
    DECL_ON_FUNC( DBE_INSERT_ITEM_REQ );
	DECL_ON_FUNC( DBE_NEW_QUEST_REQ );
	DECL_ON_FUNC( DBE_GIVE_UP_QUEST_REQ );
	DECL_ON_FUNC( DBE_QUEST_COMPLETE_REQ );
	DECL_ON_FUNC( DBE_INSERT_SKILL_REQ );

	//{{ 2012. 03. 23	박세훈	관리자용 치트키 오류 수정
	_DECL_ON_FUNC( DBE_ADMIN_INIT_SKILL_TREE_REQ, KDBE_INIT_SKILL_TREE_REQ );
	//}}

   _DECL_ON_FUNC( DBE_SEARCH_UNIT_REQ, std::wstring );
   _DECL_ON_FUNC( DBE_KNM_REQUEST_NEW_FRIEND_INFO_REQ, std::wstring );
    DECL_ON_FUNC( DBE_INSERT_TRADE_ITEM_REQ );
	//{{ 2009. 2. 10  최육사	개인거래 버그 예방
	DECL_ON_FUNC( DBE_INSERT_TRADE_ITEM_BY_SERVER_NOT );
	//}}

   _DECL_ON_FUNC( DBE_INSERT_PURCHASED_CASH_ITEM_REQ, KDBE_INSERT_ITEM_REQ );
    DECL_ON_FUNC( DBE_INSERT_PURCHASED_CASH_PACKAGE_REQ );

	DECL_ON_FUNC( DBE_OPEN_RANDOM_ITEM_REQ );
	DECL_ON_FUNC( DBE_ITEM_MANUFACTURE_REQ );

   _DECL_ON_FUNC( DBE_CHANGE_NICK_NAME_REQ, KEGS_CHANGE_NICK_NAME_REQ );

	DECL_ON_FUNC( DBE_NEW_BLACKLIST_USER_REQ );
	DECL_ON_FUNC( DBE_DEL_BLACKLIST_USER_REQ );

	//{{ 2009. 9. 22  최육사	전직캐쉬
	DECL_ON_FUNC( DBE_CHANGE_UNIT_CLASS_REQ );
	//}}

	DECL_ON_FUNC( DBE_INSERT_BUY_PERSONAL_SHOP_ITEM_REQ );

    DECL_ON_FUNC( DBE_ENCHANT_ITEM_REQ );
	//{{ 2008. 12. 21  최육사	강화 복구
	DECL_ON_FUNC( DBE_RESTORE_ITEM_REQ );
	//}}
	DECL_ON_FUNC( DBE_RESOLVE_ITEM_REQ );
	DECL_ON_FUNC( DBE_SOCKET_ITEM_REQ );

	DECL_ON_FUNC( DBE_INIT_SKILL_TREE_REQ );

	DECL_ON_FUNC( DBE_RECOMMEND_USER_REQ );
	DECL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_REQ );
	DECL_ON_FUNC( DBE_PREPARE_INSERT_LETTER_TO_POST_REQ );
	DECL_ON_FUNC( DBE_INSERT_LETTER_TO_POST_REQ );

	//{{ 2008. 9. 18  최육사	우편함
	DECL_ON_FUNC( DBE_GET_POST_LETTER_LIST_REQ );
	DECL_ON_FUNC( DBE_READ_LETTER_NOT );
	DECL_ON_FUNC( DBE_GET_ITEM_FROM_LETTER_REQ );
	DECL_ON_FUNC( DBE_DELETE_LETTER_NOT );
	//}}

	//080407.hoons. 사제관계 지우기
	DECL_ON_FUNC( DBE_DEL_TUTORIAL_REQ );
	DECL_ON_FUNC( DBE_INSERT_TUTORIAL_REQ );

	DECL_ON_FUNC( DBE_RESET_SKILL_REQ );
	DECL_ON_FUNC( DBE_EXPAND_SKILL_SLOT_REQ );
    DECL_ON_FUNC( DBE_EXPAND_INVENTORY_SLOT_REQ );
	//{{ 2008. 12. 14  최육사	캐릭터 슬롯 확장
	DECL_ON_FUNC( DBE_EXPAND_CHAR_SLOT_REQ );
	//}}

    DECL_ON_FUNC( DBE_GET_WISH_LIST_REQ );

	DECL_ON_FUNC( DBE_UPDATE_EVENT_TIME_REQ );


	DECL_ON_FUNC( DBE_UPDATE_INVENTORY_ITEM_POS_NOT );

	DECL_ON_FUNC( DBE_PRESENT_CASH_ITEM_CHECK_NICKNAME_REQ );

	//{{ 2008. 9. 3  최육사		속성강화
	DECL_ON_FUNC( DBE_ATTRIB_ENCHANT_ITEM_REQ );
	DECL_ON_FUNC( DBE_IDENTIFY_ITEM_REQ );
	//}}

	//{{ 2008. 9. 26  최육사	우체국 블랙리스트
	DECL_ON_FUNC( DBE_NEW_POST_BLACK_LIST_REQ );
	DECL_ON_FUNC( DBE_DEL_POST_BLACK_LIST_REQ );
	//}}

	//{{ 2008. 10. 7  최육사	타이틀
	DECL_ON_FUNC( DBE_INSERT_TITLE_REQ );
	//}}

	//{{ 2008. 12. 25  최육사	부여
	DECL_ON_FUNC( DBE_ENCHANT_ATTACH_ITEM_REQ );
	//}}

	//{{ 2008. 11. 18  최육사	아이템 교환
	DECL_ON_FUNC( DBE_ITEM_EXCHANGE_REQ );
	//}}

	//{{ 2009. 4. 8  최육사		닉네임 변경
	DECL_ON_FUNC( DBE_DELETE_NICK_NAME_REQ );
	//}}

	DECL_ON_FUNC( DBE_INSERT_CASH_SKILL_POINT_REQ );
	DECL_ON_FUNC( DBE_EXPIRE_CASH_SKILL_POINT_REQ );

	//{{ 2009. 8. 4  최육사		봉인 스킬
	DECL_ON_FUNC( DBE_UNSEAL_SKILL_REQ );
	//}}

	//{{ 2009. 5. 11  최육사	실시간 아이템
	DECL_ON_FUNC( DBE_GET_ITEM_INSERT_TO_INVENTORY_REQ );
	DECL_ON_FUNC( DBE_GET_TEMP_ITEM_REQ );
	//}}

    DECL_ON_FUNC( DBE_REQUEST_FRIEND_REQ );
    DECL_ON_FUNC( DBE_ACCEPT_FRIEND_REQ );
    DECL_ON_FUNC( DBE_DENY_FRIEND_REQ );
    DECL_ON_FUNC( DBE_BLOCK_FRIEND_REQ );
    DECL_ON_FUNC( DBE_UNBLOCK_FRIEND_REQ );
    DECL_ON_FUNC( DBE_DELETE_FRIEND_REQ );
    DECL_ON_FUNC( DBE_MOVE_FRIEND_REQ );
    DECL_ON_FUNC( DBE_MAKE_FRIEND_GROUP_REQ );
    DECL_ON_FUNC( DBE_RENAME_FRIEND_GROUP_REQ );
    DECL_ON_FUNC( DBE_DELETE_FRIEND_GROUP_REQ );
    DECL_ON_FUNC( DBE_FRIEND_MESSAGE_NOT );

	//{{ 2009. 7. 29  최육사	item set cheat
	DECL_ON_FUNC( DBE_ADMIN_GET_ITEM_SET_NOT );
	//}}

	//{{ 2009. 8. 27  최육사	봉인
	DECL_ON_FUNC( DBE_SEAL_ITEM_REQ );
	DECL_ON_FUNC( DBE_UNSEAL_ITEM_REQ );
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 9. 22  최육사	길드

	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	//}}

   _DECL_ON_FUNC( DBE_INVITE_GUILD_NICKNAME_CHECK_REQ, KELG_INVITE_GUILD_ACK );
	DECL_ON_FUNC( DBE_JOIN_GUILD_REQ );

	DECL_ON_FUNC( DBE_CHANGE_GUILD_MEMBER_GRADE_REQ );
	DECL_ON_FUNC( DBE_CHANGE_GUILD_MESSAGE_REQ );
	DECL_ON_FUNC( DBE_KICK_GUILD_MEMBER_REQ );
	DECL_ON_FUNC( DBE_CHANGE_GUILD_MEMBER_MESSAGE_REQ );

	DECL_ON_FUNC( DBE_DISBAND_GUILD_REQ );
	DECL_ON_FUNC( DBE_EXPAND_GUILD_MAX_MEMBER_REQ );

	//{{ 2009. 10. 27  최육사	길드레벨
	DECL_ON_FUNC( DBE_UPDATE_GUILD_EXP_REQ );
	//}}

	//}}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 11. 24  최육사	길드스킬

	DECL_ON_FUNC( DBE_RESET_GUILD_SKILL_REQ );
	DECL_ON_FUNC( DBE_INIT_GUILD_SKILL_REQ );

	//}}
	//////////////////////////////////////////////////////////////////////////

	//{{ 2009. 12. 8  최육사	크리스마스이벤트
	DECL_ON_FUNC( DBE_CHECK_TIME_EVENT_COMPLETE_REQ );
	//}}
	//{{ 허상형 : [2010/8/19/] //	선물 기능 개편
	DECL_ON_FUNC( DBE_GET_NICKNAME_BY_UNITUID_REQ );
	//}} 허상형 : [2010/8/19/] //	선물 기능 개편

// 현재 사용하는 곳 없음. 주석처리
// #ifdef SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN
// 	DECL_ON_FUNC( DBE_INSERT_GIANT_DELETE_UNIT_REQ );
// #endif //SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN

	//{{ 2010. 01. 11  최육사	추천인리스트
	DECL_ON_FUNC( DBE_GET_RECOMMEND_USER_LIST_REQ );
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

	//{{ 2010. 03. 22  최육사	기술의 노트
	DECL_ON_FUNC( DBE_EXPAND_SKILL_NOTE_PAGE_REQ );
	DECL_ON_FUNC( DBE_REG_SKILL_NOTE_MEMO_REQ );
	//}}

	//{{ 2010. 7. 30 최육사	펫 시스템
	DECL_ON_FUNC( DBE_CREATE_PET_REQ );
	DECL_ON_FUNC( DBE_SUMMON_PET_REQ );

	//{{ 2012. 02. 22	박세훈	펫 이름 변경권
	DECL_ON_FUNC( DBE_CHANGE_PET_NAME_REQ );
	//}}

	//}}

	//{{ 2010. 8. 16	최육사	기간 리셋 아이템 이벤트
	DECL_ON_FUNC( DBE_RESET_PERIOD_ITEM_REQ );
	//}}
	//{{ 2011. 01. 04	최육사	속성 부적
	DECL_ON_FUNC( DBE_ATTRIB_ATTACH_ITEM_REQ );
	//}}
	//{{ 2011. 04. 14	최육사	대리 상인
	DECL_ON_FUNC( DBE_INSERT_PERIOD_PSHOP_AGENCY_REQ );
	DECL_ON_FUNC( DBE_PREPARE_REG_PSHOP_AGENCY_ITEM_REQ );
	DECL_ON_FUNC( DBE_INSERT_TO_INVENTORY_PICK_UP_FROM_PSHOP_AGENCY_REQ );
	//}}
	//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
	DECL_ON_FUNC( DBE_SHARING_BACK_OPEN_REQ);
	//}}
	//{{ 지헌 - 은행 개편 기념 이벤트용 임시 코드(특정 아이템 은행 공유 이용 체크)

	_DECL_ON_FUNC( DBE_GET_SHARE_BANK_REQ, KEGS_GET_SHARE_BANK_REQ  );
	DECL_ON_FUNC( DBE_UPDATE_SHARE_ITEM_REQ );
	_DECL_ON_FUNC( DBE_UPDATE_SHARE_ITEM_FOR_MOVE_SLOT_REQ, KDBE_UPDATE_SHARE_ITEM_REQ );
	_DECL_ON_FUNC( DBE_UPDATE_SEAL_DATA_NOT, KDBE_SEAL_ITEM_REQ );

	DECL_ON_FUNC( DBE_CHANGE_INVENTORY_SLOT_ITEM_REQ );


	DECL_ON_FUNC( DBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT );

	//{{ 2011. 08. 03	최육사	대전 강제 종료에 대한 예외처리
	DECL_ON_FUNC( DBE_QUIT_USER_PVP_RESULT_UPDATE_NOT );
	//}}
	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}
	//{{ 2011. 10. 14	최육사	아이템 사용 DB 업데이트 수정
	DECL_ON_FUNC( DBE_USE_ITEM_IN_INVENTORY_REQ );
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	DECL_ON_FUNC( DBE_BUY_UNIT_CLASS_CHANGE_REQ );
	//}}

	//{{ 2012. 04. 30	박세훈	현자의 주문서 접속 이벤트 ( 우편함 중복 체크 )
	//}}
	//{{ 2012. 05. 08	김민성       개인 거래 로직 변경
	DECL_ON_FUNC( DBE_TRADE_COMPLETE_REQ );
	//}}
	//{{ 2012. 05. 30	최육사	그노시스 기간 치트
	DECL_ON_FUNC( DBE_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ );
	//}}
	//{{ 2012. 07. 24	박세훈	해당 캐릭터의 모든 스킬을 다 찍는 치트
	DECL_ON_FUNC( DBE_ADMIN_CHEAT_GET_ALL_SKILL_REQ );
	//}}

	//{{ 우편 ED 거래시 ED 동기화 - 김민성
	DECL_ON_FUNC( DBE_SYNC_ED_REQ );
	//}}

//{{ 2012. 08. 14	박세훈	대천사의 주화 이벤트 가이드 문구 출력
//}}

//{{ 2012. 08. 21	박세훈	우편 로직 변경
//}}
//{{ 2012 대전 시즌2 전야 런칭 이벤트	- 김민성
//}}

//{{ 2012. 12. 14  계정 미션 시스템 ( 계정단위 퀘스트 ) - 김민성
	_DECL_ON_FUNC( DBE_NEW_ACCOUNT_QUEST_GAME_DB_REQ, KDBE_NEW_QUEST_REQ );
	_DECL_ON_FUNC( DBE_ACCOUNT_QUEST_COMPLETE_GAME_DB_REQ, KDBE_ACCOUNT_QUEST_COMPLETE_ACK );
//}}

//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
	DECL_ON_FUNC( DBE_PET_AUTO_LOOTING_NOT );
//}}

	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}

	DECL_ON_FUNC( DBE_CHANGE_PET_ID_REQ );

	//{{ 2013. 3. 11	박세훈	 로컬 랭킹 시스템
	//}}

	//{{ 2013. 03. 21	 추천인 시스템 개편 - 김민성
	//}
	DECL_ON_FUNC( DBE_INSERT_WARP_VIP_REQ );
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	_DECL_ON_FUNC( DBE_COUPLE_PROPOSE_USER_FIND_REQ, KEGS_COUPLE_PROPOSE_REQ );
	DECL_ON_FUNC( DBE_COUPLE_MAKING_SUCCESS_REQ );
	DECL_ON_FUNC( DBE_WEDDING_PROPOSE_REQ );
	_DECL_ON_FUNC( DBE_CHECK_NICK_NAME_REQ, KEGS_CHECK_NICK_NAME_REQ );
	_DECL_ON_FUNC( DBE_WEDDING_ITEM_INFO_REQ, KEGS_WEDDING_ITEM_INFO_REQ );
	DECL_ON_FUNC( DBE_WEDDING_ITEM_DELETE_REQ );
	DECL_ON_FUNC( DBE_BREAK_UP_REQ );
	DECL_ON_FUNC( DBE_ADMIN_CHANGE_COUPLE_DATE_REQ );
	DECL_ON_FUNC( DBE_CHANGE_LOVE_WORD_REQ );
	DECL_ON_FUNC( DBE_INSERT_WEDDING_REWARD_REQ );
	//}
	DECL_ON_FUNC( DBE_GET_RIDING_PET_LIST_REQ );
	DECL_ON_FUNC( DBE_CREATE_RIDING_PET_REQ );
	DECL_ON_FUNC( DBE_RELEASE_RIDING_PET_REQ );



	//{{ 2013. 05. 15	최육사	아이템 개편
	DECL_ON_FUNC( DBE_ITEM_EVALUATE_REQ );
	DECL_ON_FUNC( DBE_RESTORE_ITEM_EVALUATE_REQ );
	DECL_ON_FUNC( DBE_ITEM_CONVERT_REQ );
	//}}


	DECL_ON_FUNC( DBE_ADMIN_CHANGE_UNIT_CLASS_REQ );
	DECL_ON_FUNC( DBE_ADMIN_AUTO_GET_ALL_SKILL_REQ );
	DECL_ON_FUNC( DBE_ADMIN_GET_SKILL_REQ );



	DECL_ON_FUNC( DBE_READY_TO_SOSUN_EVENT_REQ );

	_DECL_ON_FUNC( DBE_EVENT_PROPOSE_USER_FIND_REQ, KEGS_USE_PROPOSE_ITEM_REQ );
	DECL_ON_FUNC( DBE_EVENT_MAKING_SUCCESS_REQ );
	DECL_ON_FUNC( DBE_EVENT_MAKING_SUCCESS_ACCEPTOR_REQ );
	DECL_ON_FUNC( DBE_EVENT_DIVORCE_REQ );

	DECL_ON_FUNC( DBE_RELEASE_PET_REQ );



	DECL_ON_FUNC( DBE_2013_EVENT_MISSION_COMPLETE_REQ );
	DECL_ON_FUNC( DBE_2014_EVENT_MISSION_COMPLETE_REQ );

	DECL_ON_FUNC( DBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT );

	DECL_ON_FUNC( DBE_REGIST_GLOBAL_MISSION_CLEAR_NOT );


	DECL_ON_FUNC( DBE_ITEM_EXTRACT_REQ );
	DECL_ON_FUNC( DBE_USE_FINALITY_SKILL_REQ );

    DECL_ON_FUNC( DBE_EXPAND_BANK_INVENTORY_REQ );

	DECL_ON_FUNC( DBE_SOCKET_EXPAND_ITEM_REQ );

	DECL_ON_FUNC( DBE_EXPAND_SKILL_PAGE_REQ );
	DECL_ON_FUNC( DBE_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ );

    _DECL_ON_FUNC( DBE_CHARACTER_LIST_REQ, std::wstring );
    _DECL_ON_FUNC( DBE_ENTRY_POINT_CHECK_NICK_NAME_REQ, KEGS_ENTRY_POINT_CHECK_NICK_NAME_REQ );
    DECL_ON_FUNC( DBE_GET_CREATE_UNIT_TODAY_COUNT_REQ );




};

template < class T >
void KGSGameDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_GAME_DB, 0, PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KGSGameDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_GAME_DB, 0, PI_GS_USER, nTo, NULL, usEventID, data );
}

//{{ 2009. 10. 30  최육사	길드레벨
template < class T >
void KGSGameDBThread::SendToLoginServer( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_GAME_DB, 0, PI_LOGIN_USER, 0, NULL, usEventID, data );
}
//}}

//{{ 2010. 11. 25	조효진	추가 수정 임규수 일본 추가
//#ifdef SERV_SELECT_UNIT_NEW
template < class T >
void KGSGameDBThread::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_GAME_DB, 0, PI_LOG_DB, 0, NULL, usEventID, data );
}
//#endif SERV_SELECT_UNIT_NEW
//}}
