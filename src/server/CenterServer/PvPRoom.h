#pragma once

#include "Room.h"
#include "Lottery.h"
#include "x2/platform/timer.h"

//{{ 2011. 10. 25	최육사	배틀필드 시스템
	#include "PvpMonsterManager.h"
//}}

SmartPointer( KPvPRoom );

class KPvPRoom : public KRoom
{
    NiDeclareRTTI;
    DeclToStringW;

public:
#   undef _ENUM
#   define _ENUM( id ) id,
    enum StatPvP
    {
#   include "StatPvP_def.h"
    };

    struct KRebirthInfo
    {
        UidType nCID;
        int iPosition;
        float fDelay;
        x2::Timer kTimer;
		//{{ 2012. 11. 27 PVP에서 유저가 죽은 이후 부활 시점 수정 - 김민성
		bool	bReady;
		//}

		KRebirthInfo()
		{
			nCID = 0;
			iPosition = 0;
			fDelay = 0.f;
			//{{ 2012. 11. 27 PVP에서 유저가 죽은 이후 부활 시점 수정 - 김민성
			bReady = false;
			//}
		}
    };

	struct KTeamData
	{
		int		m_nMaxAlive;
		float	m_fMaxHP;
		int		m_nAverageLevel;
		int		m_nUnitNum;

		void Init()
		{
			m_nMaxAlive		= 0;
			m_fMaxHP		= 0;
			m_nAverageLevel	= 0;
			m_nUnitNum		= 0;
		}
	};
	struct DROP_ITEMDATA
	{
		bool	m_bIsGet;
		int		m_iDropItemUID;
		int		m_iItemID;
		int		m_iGroup;
		x2::Timer m_TimerOfChangeItem;

		DROP_ITEMDATA()
		{
			m_bIsGet = false;
			m_iDropItemUID = 0;
			m_iItemID = 0;
			m_iGroup = 0;
			m_TimerOfChangeItem.restart();
		}
	};









public:
    KPvPRoom();
    virtual ~KPvPRoom();

    // override.
    virtual void Tick();

protected:
    // Get Set
    virtual void SetRoomInfo( const KRoomInfo& kInfo );

    // override.
    virtual void ProcessNativeEvent( const KEventPtr& spEvent );
    virtual void StartGame();
    virtual void StartPlay();
    virtual void EndPlay();
    virtual void EndGame();
    virtual bool CheckIfPlayEnd();
    virtual bool IsAbleToIntrude();
    virtual int  DecideWinTeam();
    virtual int  GetPlayMode();
    virtual void HandleTimeOutUser( std::map< UidType, UidType >& mapUserCIDNGS );
	virtual void CloseRoom();

    void CheckRebirth();
	void DelRebirth( UidType nCID );	//방에서 나가는 경우에 사용하기 위해
    void ReserveEndPlay();
    void CheckEndPlay();
	void IntrudeTeamBalance( UidType nCID );
	void DeathTeamBalance();
	void AbuserLogToDB( KRoomUserPtr spRoomUser, int iVSPoint, int iEXP );
	void PvPLogToFile( KRoomUserPtr spRoomUser, int iVSPoint, int iEXP, int iResult );

	virtual void SendRoomListInfo( IN const int iCode );
	//{{ 2011. 07. 26	최육사	대전 개편
	virtual void SendPVPUserInfo( UidType iUnitUID, char cState = KCommunityUserInfo::US_NONE );
	virtual void SendAllPVPUserInfo();
	//}}

	int	 GetPVPChannelClass() const						{ return m_iPVPChannelClass; }
	//{{ 2011. 07. 22	최육사	대전 개편
	virtual	bool IsOfficialMatch() const				{ return ( m_iPVPChannelClass == KPVPChannelInfo::PCC_OFFICIAL ); }
	void SetMatchType( IN const SEnum::MATCH_TYPE eMatchType )			{ m_eMatchType = eMatchType; }
	SEnum::MATCH_TYPE GetMatchType() const				{ return m_eMatchType; }
	void SetPvpNpcType( IN const SEnum::PVP_NPC_TYPE ePvpNpcType )		{ m_ePvpNpcType = ePvpNpcType; }
	SEnum::PVP_NPC_TYPE GetPvpNpcType() const			{ return m_ePvpNpcType; }
	//}}

	//{{ 2012. 05. 23	최육사	배틀필드 시스템
	virtual void TimeCountFinishProcess( IN const u_short usEventID );
	//}}
	//{{ 2012. 07. 16	박세훈	2012 대전 시즌2
	void RevengeBuffProcess( IN const UidType& iMyUnitUID, IN int iPrevMyTeamKillNumGab, IN int iNowMyTeamKillNumGab );

	int	GetTeamRevengeBuffID( IN const UidType iMyUnitUID ) const;

	//}}

   _DECL_ON_FUNC( ERM_OPEN_PVP_ROOM_REQ, KERM_OPEN_ROOM_REQ );
	//{{ 2011. 06. 23	최육사	대전 개편
   _DECL_ON_FUNC( ERM_OPEN_PVP_ROOM_FOR_MATCH_REQ, KEGB_OPEN_PVP_ROOM_FOR_MATCH_REQ );
	//}}
    DECL_ON_FUNC( ERM_GAME_START_REQ );
	DECL_ON_FUNC( ERM_LEAVE_ROOM_REQ );
	//{{ 2011. 11. 1	최육사	배틀필드 시스템
   _DECL_ON_FUNC( ERM_LEAVE_ROOM_FOR_JOIN_BATTLE_FIELD_REQ, KERM_LEAVE_ROOM_REQ );
   _DECL_ON_FUNC( ERM_LEAVE_ROOM_FOR_ENTER_THE_VILLAGE_REQ, KERM_LEAVE_ROOM_REQ );
	//}}
	//{{ 2012. 10. 17	최육사		P2P & Relay 정보 수집
   _DECL_ON_FUNC( ERM_RESULT_SUCCESS_REQ, KEGS_RESULT_SUCCESS_REQ );
	//}}

	//{{ dmlee 2008.11.13 PVP monster
	_DECL_ON_FUNC( ERM_NPC_UNIT_CREATE_REQ, KEGS_NPC_UNIT_CREATE_REQ );
	_DECL_ON_FUNC( ERM_NPC_UNIT_DIE_REQ, KEGS_NPC_UNIT_DIE_REQ );
	//}} dmlee 2008.11.13 PVP monster

	//{{ 2009. 7. 20  최육사	영웅대전
   _DECL_ON_FUNC( ERM_CHANGE_SLOT_OPEN_REQ, KEGS_CHANGE_SLOT_OPEN_REQ );
	//}}

   _DECL_ON_FUNC( ERM_USER_UNIT_DIE_REQ, KEGS_USER_UNIT_DIE_REQ );
    DECL_ON_FUNC_NOPARAM( ERM_USER_UNIT_DIE_COMPLETE_REQ );
   _DECL_ON_FUNC( ERM_USER_UNIT_RE_BIRTH_POS_REQ, KEGS_USER_UNIT_RE_BIRTH_POS_REQ );
	DECL_ON_FUNC( ERM_MY_USER_UNIT_INFO_TO_SERVER_REQ );
    DECL_ON_FUNC_NOPARAM( ERM_LEAVE_GAME_REQ );
    DECL_ON_FUNC_NOPARAM( ERM_STATE_CHANGE_GAME_INTRUDE_REQ );
   _DECL_ON_FUNC( ERM_INTRUDE_START_REQ, KEGS_INTRUDE_START_REQ );
   _DECL_ON_FUNC( ERM_CHANGE_TEAM_REQ, KEGS_CHANGE_TEAM_REQ );
   _DECL_ON_FUNC( ERM_CHANGE_MAP_REQ, KEGS_CHANGE_MAP_REQ );
   _DECL_ON_FUNC( ERM_CHANGE_WINNING_NUM_KILL_REQ, KEGS_CHANGE_WINNING_NUM_KILL_REQ );





   _DECL_ON_FUNC( ERM_CHECK_INVALID_USER_NOT, UidType );

	_DECL_ON_FUNC( ERM_CREATE_PVP_ITEM_REQ, KEGS_CREATE_PVP_ITEM_REQ );
	_DECL_ON_FUNC( ERM_GET_ITEM_REQ, KEGS_GET_ITEM_REQ );

	//{{ 2011. 07. 26	최육사	대전 개편
	DECL_ON_FUNC( ERM_UPDATE_PVP_PLAY_UNIT_INFO_NOT );
   _DECL_ON_FUNC( ERM_PVP_NPC_UNIT_RE_BIRTH_POS_REQ, KEGS_PVP_NPC_UNIT_RE_BIRTH_POS_REQ );
   _DECL_ON_FUNC( ERM_PVP_NPC_UNIT_INFO_TO_SERVER_REQ, KEGS_PVP_NPC_UNIT_INFO_TO_SERVER_REQ );
	//}}

   //{{ 2012. 02. 14    김민성   PVP 대전 Item 생성 위치 변경
   _DECL_ON_FUNC( ERM_CHANGE_PVP_ITEM_POS_REQ, KEGS_CHANGE_PVP_ITEM_POS_REQ );
   //}}

   //{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
   //}}
   _DECL_ON_FUNC( ERM_PVP_REMATCH_REQ, KEGS_PVP_REMATCH_REQ );
   virtual void Rematch();
   virtual bool CanRematch();

   _DECL_ON_FUNC( ERM_CHANGE_PVP_ROOM_PUBLIC_REQ, KEGS_CHANGE_PVP_ROOM_PUBLIC_REQ );
   _DECL_ON_FUNC( ERM_CHANGE_PVP_ROOM_NAME_REQ, KEGS_CHANGE_PVP_ROOM_NAME_REQ );

   //{{ 2011. 01. 25  김민성  월드 트리거 전달(던전만 적용중 - 이후 PVP 확장)
   _DECL_ON_FUNC( ECN_WORLD_TRIGGER_RELOCATION_REQ, KEGS_WORLD_TRIGGER_RELOCATION_REQ );
   //}}
protected:
	bool	PvpDropItem( KERM_UPDATE_PVP_UNIT_INFO_NOT& kInfo );
	//{{ 2011. 11. 1	최육사	배틀필드 시스템
	void	OnLeaveRoom( IN const UidType iLAST_SENDER_UID, IN const UidType iFIRST_SENDER_UID, IN const KERM_LEAVE_ROOM_REQ& kPacket_, IN const u_short usEventIDAck );
	//}}
//{{ 2012. 10. 7	박세훈	랜선렉 로그 추가
	//}}
	//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
	virtual	void LogToDB_GamePlayNetWorkInfo( IN const UidType iUnitUID, IN const KGamePlayNetworkInfoPackage& kInfo, IN const bool bLeaveRoom );
	//}}

    //{{ 2013.09.03 하형진  대전 룸에 배틀필드 패킷 전송시 무시
	DECL_ON_FUNC_NOPARAM( ERM_BATTLE_FIELD_ZOMBIE_ALERT_NOT );

protected:
    std::vector< KRebirthInfo >     m_vecRebirthInfo;
	int								m_iPVPChannelClass;

	//{{ 2011. 10. 25	최육사	배틀필드 시스템
	KPvpMonsterManager				m_kPvpMonsterManager;
	//}}

	std::vector<DROP_ITEMDATA>		m_vecDropItem;	// 드롭된 아이템 : 현재는 무조건 1개만 존재
	int								m_iDropItemUID;	// 임시 UID 발급

	//{{ 2011. 07. 19	최육사	대전 개편
	SEnum::MATCH_TYPE				m_eMatchType;
	SEnum::PVP_NPC_TYPE				m_ePvpNpcType;
	//{{ 2011. 10. 25	최육사	배틀필드 시스템
	//}}
	bool							m_bOfficialPvpCancel;
	//}}


	//{{ 2012. 10. 7	박세훈	랜선렉 로그 추가
	//}}
	int								m_iRematchCount;
};
