#pragma once

#define MAX_KIND_OF_NPC 12

class CX2BattleFieldGame : public CX2Game
{
public:

	typedef std::map<int, KAttribEnchantNpcInfo> KAttribEnchantNpcInfoMap;

	CX2BattleFieldGame();
	virtual ~CX2BattleFieldGame();

	virtual void			Release();

	virtual HRESULT			OnFrameMove( double fTime, float fElapsedTime );
	virtual HRESULT			OnFrameRender();

	virtual void			GameLoading( CX2Room* pRoom );

	virtual void			WorldLoading();

	virtual bool			DetermineLastKill() { return false; }

	virtual	void			ResurrectOtherUser();

	virtual bool			Handler_EGS_NPC_UNIT_DIE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool					Handler_EGS_JOIN_BATTLE_FIELD_NOT( KEGS_JOIN_ROOM_NOT& kPacket_ );


	void					CreateNpcCreatedInfo( const vector<KNPCUnitReq>& vecNpcData_
								, const KAttribEnchantNpcInfoMap& mapAttribNpcInfo_
								, bool bSetNPCLoadComplete
		);
	//virtual bool			Handler_EGS_LEAVE_ROOM_ACK( KEGS_LEAVE_ROOM_ACK& kEGS_LEAVE_ROOM_ACK );
	//virtual bool			Handler_EGS_LEAVE_ROOM_NOT( KEGS_LEAVE_ROOM_NOT& kEGS_LEAVE_ROOM_NOT );

	//virtual bool			Handler_EGS_END_GAME_NOT( KEGS_END_GAME_NOT& kKEGS_END_GAME_NOT );

	//virtual void			UserUnitDieNot( KEGS_USER_UNIT_DIE_NOT& kEGS_USER_UNIT_DIE_NOT );

/*	void					CreateNpcFromDungeonIntrudeSyncPacket( const KEGS_DUNGEON_INTRUDE_SYNC_NOT& kPacket_ );*/
	void					SetBattleFieldId( UINT uiBattleFieldId_ ) { m_pBattleFieldRoom->SetBattleFieldId( uiBattleFieldId_ ); }

	void					SwapWorld();

	UINT					GetWorldId() const { return m_uiWorldId; }
	void					SetWorldId( const UINT uiWorldId_ ) { m_uiWorldId = uiWorldId_; }

	int	GetMyGUUserLastTouchedLineMapIndex( const bool bFutureState_ ) const
	{
		ASSERT( GetMyUnit() );
		return GetMyUnit()->GetLastTouchLineIndex( bFutureState_ );
	}

	bool					IsNearPortalLineMap();

	void					CreateBattleFieldNpcAtStart( const std::vector< KNPCUnitReq >& vecNPCDataList_
								, const KAttribEnchantNpcInfoMap& mapAttribNpcInfo_
		);
	void					CreateBattleFieldNpcByNotData( const std::vector< KNPCUnitNot >& vecNPCDataList_
								, const KAttribEnchantNpcInfoMap& mapAttribNpcInfo_
		);

	void					CreateBattleFieldNpc( const KNPCUnitReq& kNpcUnitData_
								, const KAttribEnchantNpcInfo* pkAttribNpcInfo_
		);

	void					ClearNpcCreatedInfoList();
	bool					DeleteNpcInCreatedInfoList( const UINT uiNpcUid_ );
	virtual bool			DeleteNPCUnitByUID( const UINT uiNpcUid_ );
	bool					GetIsNpcLoadComplete() const {return m_bIsNpcLoadComplete;}

	void					Send_EGS_UPDATE_BATTLE_FIELD_USER_POS_NOT();
	virtual bool			Handler_EGS_LEAVE_BATTLE_FIELD_NOT( const KEGS_LEAVE_BATTLE_FIELD_NOT& kPacket );

	virtual void			AddUserUnit( CX2Room::SlotData* pSlotData_, int iSlotIndex_ );
	CX2GUUser*				CreateAndInsertUserUnit(  CX2Room::SlotData* pSlotData_, int iSlotIndex_ );


	CX2NPCIndicator*		GetNpcIndicator() const {return m_pNPCIndicator;}

	CX2GUNPC *				GetChainedBossGroup ( CX2GUNPC * pGUNPC_ );		// 필드 내의 GroupID 가 같은 보스를 검색
	void					SetShowMonsterIndicator (bool bVal ) { if( NULL != m_pMonsterIndicator) m_pMonsterIndicator->SetShow(bVal); } // 몬스터 알림판을 보이게 함
	CX2MonsterIndicator*	GetMonsterIndicator() const { return m_pMonsterIndicator; }
	void					SetMonsterIndicator(CX2MonsterIndicator* val) { m_pMonsterIndicator = val; }
	void					UpdateCreateUnitDisplayEffect ( CX2GUNPC * pNPC_, bool bIsBossSummoned_ );		// 몬스터 등장 및 모두 사망 시, 해당 몬스터 그룹 이름에 대한 연출
	void					SetIndicatorToNpc ( CX2GUNPC * pNPC_ );		// NPC 에 Indicator 를 할당 및 설정

	void					StartElapsedTimeSinceGettingFirstNpcSyncPacket();
	bool					GetCanCheckGettingFirstNpcSyncPacket() const { return m_bCanCheckGettingFirstNpcSyncPacket; }
	void					SetCanCheckGettingFirstNpcSyncPacket(bool val) { m_bCanCheckGettingFirstNpcSyncPacket = val; }
	void					Handler_EGS_CHECK_EXIST_MONSTER_UID_REQ( const KEGS_CHECK_EXIST_MONSTER_UID_REQ& kPacket_ );
	bool					Handler_EGS_CHECK_EXIST_MONSTER_UID_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void					CreateRaidPotal();
	void					DestoryRaidPotalParticle();
	void					CreateLoadingUI();
	bool					GetShowLoadUi() const { if(m_pDLGLoadingState == NULL) return false; return true; }

	void					ResetTimerForSendingPositionInfo(){ m_TimerForSendingPositionInfo.ResetSumOfElapsedTime();}

protected:

	virtual void			UpdateUnitPointer();
	void					CreateBattleFieldNpcByCreatedInfo( const CNpcCreatedInfoPtr NpcCreatedInfoPtr_ );
	virtual void			ResurrectionKeyProcess();

	void					CheckNpcNotReceiveFirstSyncPacketAndRequestToCheckNpcUidList();
	void					CheckNpcNotReceiveFirstSyncPacketAndCollectNpcUid( vector<int>& vecMonsterUidToCheck_ ) const;

	virtual bool			GetIsExsitancePartyMemberHPLessThanPer( const float fHPRate_, const CX2GUUser* pGUUserWhoCallFunction_ ) const;

	void					OnFrameMove_LoadingUI(double fTime, float fElapsedTime);

private:

	void					OpenBattleFieldName();
	void					CreatePotalParticle();
	UINT					GetEmpyIndexNpcUnitList();
protected:
	CX2BattleFieldRoom*		m_pBattleFieldRoom;
	CKTDGUIDialogType		m_pDlgBattleFieldName;
	CKTDXCheckElapsedTime	m_TimerForSendingPositionInfo;
	const int				m_iMAX_NUM_OF_NPC;
	UINT					m_uiWorldId;
	std::deque<CNpcCreatedInfoPtr>			m_deqNpcCreatedInfo;
	bool					m_bIsNpcLoadComplete;

	CX2NPCIndicator*		m_pNPCIndicator;

	CKTDGParticleSystem::CParticleEventSequenceHandle m_hBossDanger;		// 중간 보스 출현 위험 연출
	CKTDGParticleSystem::CParticleEventSequenceHandle m_hBossDangerAdd;		// 중간 보스 출현 위험 연출

	CX2MonsterIndicator	*		m_pMonsterIndicator;		// 필드 중보 Indicator, 차후 확장 가능
	wstring 					m_wstrBossGroupName;		// 필드 보스 그룹 이름

	CKTDXCheckElapsedTime		m_ElapsedTimeSinceGettingFirstNpcSyncPacket;	/// NPC로 부터 첫 싱크 패킷을 받은 이후로 부터의 경과 시간
	bool						m_bCanCheckGettingFirstNpcSyncPacket;			/// NPC로 부터 첫 싱크 패킷을 받았는지 체크 하려면 true

	CKTDGParticleSystem::CParticleEventSequenceHandle			m_hRaidGateParticle;			// 레이드 게이트 파티클 핸들
	CKTDGParticleSystem::CParticleEventSequenceHandle			m_hRaidPortalParticle;			// 레이드 포탈 파티클 핸들
	vector<CKTDGParticleSystem::CParticleEventSequenceHandle>	m_vecRaidGateEffectParticle;	// 레이드 게이트 이펙트 파티클 핸들

	CKTDGUIDialogType		m_pDLGLoadingState;		/// 워프로 이동 했을 때 생성 되는 로딩창

	float					m_fLoadUIReaminTime;

//#ifdef SERV_COMMON_AUTO_OPEN_HARD_CODE
	mutable bool						m_bOpenPotalParticle;
//#endif //SERV_COMMON_AUTO_OPEN_HARD_CODE

};
