#pragma once

#pragma warning(disable:4996)
#include "../X2ServerProtocol/X2ServerProtocolLib.h"
#pragma warning(disable:4996)

#include "KTDX.h"
#pragma warning(disable:4996)
#include "Enum/DungeonEnum.h"
#include "Enum/Enum.h"

#include "./X2Define.h"


//{{ kimhc // 2010.3.3 // 광고 노출도 적용
//}} kimhc // 2010.3.3 // 광고 노출도 적용

//{{ 허상형 : [2009/8/4] //	핵실드
//}} 허상형 : [2009/8/4] //	핵실드

//-- 2010. 12. 13.  지헌 : XTRAP 클라이언트 - 해더


// korea nProtect



#include "./X2ServerEvent.h"
#include "./X2ServerPacket.h"
#include "./X2Struct.h"
#include "./X2NetError.h"
#include "./X2PacketLog.h"
#include "./X2GameOption.h"
#include "./X2CheckSumManager.h"

#include "./X2PVPEmblem.h"
#include "./X2EXPTable.h"
#include "./X2Stat.h"

#include "./X2BuffFactor.h"

	#include "./X2SkillTree.h"
	#include "./X2UserSkillTree.h"

#include "./X2Unit_PreHeader.h"
#include "./X2Item_PreHeader.h"
#include "./X2Inventory.h"
#include "./X2Unit.h"

#include "./X2UnitManager.h"
#include "./X2PVPResultTable.h"

#include "./X2DamageManager.h"
#include "./X2EnchantItem.h"
#include "./X2SocketItem.h"
#include "./X2Item.h"
#include "./X2ItemManager.h"
#include "./X2SubEquip.h"
#include "./X2Eqip.h"
#include "./X2UnitViewerUI.h"
#include "./X2NPCUnitViewerUI.h"
#include "./X2User.h"

#include "./X2ComboGuide.h"




#include "./X2WorldObject.h"
#include "./X2WorldObjectMesh.h"
#include "./X2WorldObjectSkinMesh.h"
#include "./X2WorldObjectLightFlow.h"
#include "./X2WorldObjectParticle.h"
#include "./X2WorldLayer.h"
#include "./X2World.h"
#include "./X2WorldManager.h"


#include "./X2Channel.h"
#include "./X2ChannelManager.h"
#include "./X2Room.h"
#include "./X2PVPRoom.h"


#include "./X2Packet.h"
#include "./X2Camera.h"
#include "./X2StringFilter.h"
#include "./X2DamageEffect.h"
#include "./X2EffectSet.h"

#include "./X2BuffBehaviorTemplet.h"
#include "./X2BuffFinalizerTemplet.h"
#include "./X2BuffDisplayerTemplet.h"
#include "./X2BuffTemplet.h"
#include "./X2BuffTempletManager.h"

#include "./X2ComboAndSkillChainSystem.h"

#include "./X2GageData.h"
#include "./X2GageUI.h"
#include "./X2GageManager.h"
#include "./X2ComboManager.h"
#include "./X2ChatWindow.h"
#include "./X2UnitSlashTraceManager.h"

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
#include "./X2FrameUDPPack.h"
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

#include "./X2GameUnit.h"

//{{AFX
#include "./X2GUUser.h"
//}}AFX

#include "./X2GUNPC.h"


#include "./X2SimplePathFinder.h"
#include "./X2NPCAI.h"
#include "./X2AllyNPCAI.h"
#include "./X2GUNPCAI.h"
#include "./X2BattleFieldNpcAi.h"

#include "./X2GUNPCFunc.h"

//{{AFX
#include "./X2GUElsword_SwordMan.h"
#include "./X2GUArme_VioletMage.h"
#include "./X2GULire_ElvenRanger.h"
#include "./X2GURaven.h"
#include "./X2GuEve.h"

//{{ kimhc // 2010.11.25 // 2010-12-23 New Character CHUNG
#include "./X2GUChung.h"
//}} kimhc // 2010.11.25 //  2010-12-23 New Character CHUNG

#include "./X2GUAra.h"

#include "./X2GUEl.h"

#include "./X2GUAdd.h"
//}}AFX


#include "./X2Curtain.h"
#include "./X2DropItemManager.h"


#include "./X2TalkBoxManagerImp.h"
#include "./X2TalkBoxManagerImpPShop.h"
#include "./X2TalkBoxManagerImpPartyLeader.h"
#include "./X2TalkBoxManager.h"
#include "./X2TalkBoxManagerUI.h"
#include "./X2UnitLoader.h"
#include "./X2TextManager.h"
#include "./X2NoticeManager.h"
#include "./X2GameEdit.h"
#include "./X2SlotManager.h"
#include "./X2SlotItem.h"
#include "./X2ItemSlotManager.h"
#include "./X2PageManager.h"
#include "./X2PageMgrItem.h"

//{{ JHKang / 강정훈 / 2012.04.12 / 팝업 UI Class
#include "./X2PopupUIBase.h"
//}}

//{{ 오현빈 // 2012-05-15 // 플레이 가이드 Class
#include "./X2PlayGuide.h"
//}}


#include "./X2Pet.h"
#include "./X2PetAI.h"
#include "./X2PetManager.h"



#include "./X2RidingPetSkillSlot.h"
#include "./X2RidingPet.h"
#include "./X2RidingPetManager.h"

#include "./X2EventScene.h"


#include "./X2SlideShot.h"
#include "./X2Game.h"
#include "./X2GameLoadingTip.h"
#include "./X2PVPGame.h"
#include "./X2PVPGameTeam.h"
#include "./X2PVPGameTeamDeath.h"
#include "./X2PVPGameSurvival.h"
#include "./X2UIUnit.h"
#include "./X2UIUnitManager.h"
#include "./X2SquareUnit.h"
#include "./X2SquareGame.h"

#include "./X2SkillTree.h"
#include "./X2ComboTree.h"
#include "./X2PartyManager.h"
#include "./X2Community.h"

#include "./X2UIHeroMatch.h"




#include "./X2CashShop.h"

#include "./X2UIDragable.h"

#include "./X2PostBox.h"

#include "./X2SecurityPad.h"

#include "./X2KeyPad.h"

#include "./X2CharPopupMenu.h"

#include "./X2UserListPopUp.h"

	#include "./X2UISkillTreeNew.h"

//{{  [11/11/2009 : oasis907 ]
#include "./X2UIGuildSkillTree.h"
//}}  [11/11/2009 : oasis907 ]

//{{ oasis907 : 김상윤 [2010.4.5] //
#include "./X2UISocketItem.h"
//}}
#include "./X2UIPetInventory.h"

#include "./X2UIComboTree.h"

#include "./X2UIEnduranceChecker.h"
#include "./X2UIInventory.h"
#include "./X2UICharInfo.h"
#include "./X2UIWatch.h"
#include "./X2UIPersonalShop.h"
#include "./X2UIPersonalTrade.h"
#include "./X2UIShop.h"
#include "./X2UIManufacture.h"
#include "./X2UIQuickSlot.h"
#include "./X2UIPetInfo.h"
#include "./X2UIQuestNew.h"
#include "./X2UIQuestReceive.h"
#include "./X2UIQuest.h"
#include "./X2Book.h"
//{{ kimhc	// 실시간 엘소드 중 실시간 아이템 획득 과 관련한 임시 인벤토리 클래스
// 던전 내에서 아이템 획득시 인벤토리가 가득 찼을 경우 임시 인벤토리를 생성함
#include "./X2UITempInventory.h"
//}} kimhc	// 실시간 엘소드 중 실시간 아이템 획득 과 관련한 임시 인벤토리 클래스

//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
#include	"./X2UIRankingInfo.h"
//}} kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업

//{{ 2011.9.16 이준호 반디캠 작업
#include "bandicap.h"
//}}



//{{ oasis907 : 김상윤 [2009.12.14] //
#include "./X2UIGuildSkillTreeInfo.h"

#include "./X2UIGuildBoard.h"
//}}

//{{ oasis907 : 김상윤 [2010.3.12] //
#include "./X2UIPersonalShopBoard.h"
//}}


//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
#include	"./X2UIItemExchangeShop.h"
//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

//{{ kimhc // 2009-08-04 // 캐릭터별 은행
#include	"./X2UIPrivateBank.h"
//}} kimhc // 2009-08-04 // 캐릭터별 은행

//{{ 허상형 : [2009/9/8] //	길드 창단과 관리
#include "./X2GuildManager.h"
#include "./X2UIGuild.h"
//}} 허상형 : [2009/9/8] //	길드 창단과 관리

#include "./X2RelationshipManager.h"
#include "./X2UIRelationship.h"


//{{ 허상형 : [2011/3/14/] //	월드 미션
#include "./X2WorldMissionManager.h"
#include "./X2UIWorldMission.h"
//}} 허상형 : [2011/3/14/] //	월드 미션

#include "./X2GlobalMissionManager.h"
#include "./X2UIGlobalMission.h"

//{{ JHKang / 강정훈 / 2011.6.3 / 휴면 복귀 유저 보상 및 특수 보상에 대한 UI 및 버프 처리
#include "./X2PremiumBuffManager.h"
//}}

//{{ JHKang / 강정훈 / 2011.8.8 / 이탈 유저 방지 시스템
//}}

#include "./X2UISkillNote.h"

#include "./X2CubeOpenImageManager.h"

//{{ 2011.03.16   임규수 아바타 분해 시스템
//}}
//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}
#include "./X2UIManager.h"

#include "./X2MiniMapUI.h"

#include "./X2NPCIndicator.h"


#include "./X2BattleFieldManager.h"

#include "./X2MonsterIndicator.h"
#include "./X2DungeonSubStage.h"
#include "./X2DungeonStage.h"
#include "./X2Dungeon.h"
#include "./X2DungeonManager.h"
#include "./X2DungeonRoom.h"
#include "./X2DungeonGame.h"

#include "./X2BattleFieldRoom.h"
#include "./X2BattleFieldGame.h"

#include "./X2QuestManager.h"
#include "./X2Cursor.h"
#include "./X2LocationManager.h"
#include "./X2TrainingCenterTable.h"

//#ifdef TITLE_SYSTEM
//{{ 2008. 10. 1  최육사
#include "./X2TitleSystem.h"
#include "./X2TitleManager.h"
//}}
//#endif

//{{ 2009. 6. 24  최육사	보상테이블
#include "./X2RewardTable.h"
//}}

#include "./X2NPCIndicator.h"

#include "./X2TFieldNpcShop.h"
#include "./X2TFieldNpc.h"
#include "./X2NoviceGuide.h"
#include "./X2TFieldGame.h"


#include "./X2Informer.h"
#include "./X2InformerSkill.h"
#include "./X2InformerQuest.h"
#include "./X2InformerInven.h"
#include "./X2InformerCashShop.h"
#include "./X2InformerGuildExp.h"
#include "./X2InformerManager.h"
#include "./X2ItemStatCalculator.h"
#include "./X2Data.h"
#include "./X2InstanceData.h"

#include "./X2MemoryHolder.h"
#include "./X2LVUpEventMgr.h"

#include "./X2CouponBox.h"


#include "./X2TutorSystem.h"


#include "./X2LocalMapUI.h"
#include "./X2ChatBox.h"
#include "./X2PartyUI.h"





#include "./X2State.h"
#include "./X2StateOption.h"
#include "./X2StateCommonBG.h"
#include "./X2StateMenu.h"
#include "./X2StateStartUp.h"

#include "./X2StateLogin.h"
#include "./X2StateLoading.h"
#include "./X2StateJoin.h"
#include "./X2StateCreateUnit.h"

#include "./X2StateBattleField.h"

#include "./X2StatePvPLobby.h"
#include "./X2StatePVPRoom.h"
#include "./X2StatePVPGame.h"
#include "./X2StateDungeonRoom.h"
#include "./X2StateDungeonGame.h"
//#include "./X2StateArcadeGame.h"
#include "./X2StateSquareGame.h"

#include "./X2StateField.h"


#include "./X2StatePVPResult.h"
#include "./X2StateDungeonResult.h"
#include "./X2StateTrainingResult.h"
//#include "./X2StateArcadeResult.h"
#include "./X2StateServerSelect.h"
#include "./X2StateTrainingSchool.h"
#include "./X2StateTrainingGame.h"
#include "./X2StateClassChange.h"
//#include "./X2StateArcadeLobby.h"
#include "./X2StateAutoChanger.h"


#include "./X2SystemInfo.h"


//{{ JHKang / 강정훈 / 2010.09.06 / 숫자 출력 형태 변경
#include "./X2NumbersToLanguage.h"
//}} JHKang / 강정훈 / 2010.09.06 / 숫자 출력 형태 변경

#include "../X2/KOGGamePerformanceCheckDefine.h"

#include "./WordWrap.h"
#include <boost/random.hpp>
using boost::mt19937;

#include "./X2StateWeddingGame.h"
#include "./X2WeddingManager.h"
#include "./X2WeddingGame.h"

#include "./X2EmblemManager.h"


#include "./X2BossRaidManager.h"
BOOL CALLBACK EnumChildProc_Thread(HWND , LPARAM);

BOOL CALLBACK EnumWindowsProc_Thread(HWND , LPARAM);
BOOL Pesudo_IsDebuggerPresent();

	#include <boost/random.hpp>
	using boost::mt19937;

template < typename T >
inline void Serialize( KSerBuffer* pBuff, T* pData )
{
	KTDXPROFILE();
	KSerializer ks;
	ks.BeginWriting( pBuff );
	ks.Put( *pData );
	ks.EndWriting();
}

template < typename T >
inline void DeSerialize( KSerBuffer* pBuff, T* pData )
{
	KTDXPROFILE();
	KSerializer ks;
	ks.BeginReading( pBuff );
	ks.Get( *pData );
	ks.EndReading();
}








	struct THREAD_WND_INFO
	{
		wstring m_wstrName;
		wstring m_wstrClass;
	};



class CX2Main : public CKTDXStage
{
	public:
		enum NationFlag
		{
			NF_NONE			= 0x00000000,
			NF_DE			= 0x00000001,
			NF_FR			= 0x00000002,
			NF_IT			= 0x00000003,
			NF_PL			= 0x00000004,
			NF_ES			= 0x00000005,
			NF_TR			= 0x00000006,
			NF_EN			= 0x00000007,

			NF_KR			= 0x00000100,
			NF_JP			= 0x00000200,
			NF_TW			= 0x00000300,
			NF_HK			= 0x00000301,
			NF_US			= 0x00000400,
			NF_CN			= 0x00000600,
			NF_VN			= 0x00000700,
			NF_TH			= 0x00000800,
			NF_ID			= 0x00000900,
			NF_BR			= 0x00001000,
			NF_PH			= 0x00001100,
			NF_IN			= 0x00001200,
		};

		enum X2_STATE
		{
			XS_INVALID = 0,
			XS_START_UP,
			XS_LOGIN,
			XS_JOIN,
			XS_LOADING,
			XS_SERVER_SELECT,
			XS_CREATE_UNIT,
			XS_PVP_LOBBY,
			XS_PVP_ROOM,
			XS_PVP_GAME,
			XS_DUNGEON_ROOM,
			XS_DUNGEON_GAME,
			XS_PVP_RESULT,
			XS_DUNGEON_RESULT,
			XS_VILLAGE_MAP,
			XS_SQUARE_GAME,
			XS_TRAINING_SCHOOL,
			XS_TRAINING_GAME,
			XS_TRAINING_RESULT,
			XS_CLASS_CHANGE,
			XS_ARCADE_RESULT,	/// FieldFix: 삭제하자
			XS_ARCADE_GAME,		/// FieldFix: 삭제하자
			XS_ARCADE_LOBBY,	/// FieldFix: 삭제하자
			XS_ARCADE_ROOM,		/// FieldFix: 삭제하자
			XS_BATTLE_FIELD,
//#ifdef ADDED_RELATIONSHIP_SYSTEM
			XS_WEDDING_GAME,
//#endif // ADDED_RELATIONSHIP_SYSTEM

			XS_END,
		};

		enum X2_PUBLISHER
		{
			XP_NEXON_KOREA = 0,
			XP_GAMANIA_TW = 1,
			XP_GAMANIA_HK = 2,
			XP_NHN_JP = 3,
			XP_GIANT_CN = 4,
			XP_GAMEFORGE_EU = 5,
			XP_ASIASOFT_TH = 6,
			XP_K3RCOMBO_US = 7,
			XP_CJ_ID = 8,
			XP_LEVELUP_BR = 9,
			XP_GARENA_PH = 10,
			XP_FUNIZEN_IN = 11,
		};


		struct ReservedServerPacket
		{
			SERVER_EVENT_ID		m_eServerEventID;
			void*				m_pEvent;
		};

		struct WaitServerPacket
		{
			SERVER_EVENT_ID		waitACK;
			float				fReamainTimeOut;
		};

		struct ReservedMessagePopUp
		{
			vector<X2_STATE>	m_vecTargetState;
			wstring				m_Message;
			int					m_iOKMsg;

			ReservedMessagePopUp()
			{
				m_iOKMsg = -1;
			}
			ReservedMessagePopUp( const ReservedMessagePopUp& t ) { *this = t; }    // copy constructor
			ReservedMessagePopUp& operator=( const ReservedMessagePopUp& t )        // assign operator
			{
				m_vecTargetState = t.m_vecTargetState;
				m_Message = t.m_Message;
				m_iOKMsg = t.m_iOKMsg;

				return *this;
			}
		};


		struct TimedMessagePopUp
		{
			enum MESSAGE_BOX_TYPE
			{
				MBT_NO_BUTTON,
				MBT_OK,
				MBT_OK_CANCEL,
			};

			enum MESSAGE_TYPE
			{
				MT_NONE,
				MT_REQUESTING_JOIN_PARTY,					// 파티 참가 요청자가 기다리는 팝업
				MT_PARTY_LEADER_DECIDE_JOIN_REQUEST,		// 파티 요청을 받을 파티장이 수락, 최소 결정하는 팝업
				MT_PARTY_LEADER_INVITE_PARTY,				// 파티장이 파티 초대후 기다리는 팝업
				MT_DECIDE_INVITE_REQUEST,					// 파티장의 초대를 받은 유저가 수락, 취소 결정하는 팝업
				MT_PVP_INVITE,								// 대련장에서 다른 유저를 초대한 경우
				MT_PVP_DECIDE_INVITE_REQUEST,				// 대련장에서 다른 유저의 초대를 받는 경우
				MT_WANT_TO_ENTER_SECRET_STATE,				// 비밀 스테이지에 입장하고 싶은지 아닌지 묻는 팝업
				MT_PVP_START,
				MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST,		// 던전을 시작할 준비가 되었는지 묻는 팝업
				MT_DUNGEON_MATCH_START_REQUEST,				// 매칭에 의한 던전을 시작할 준비가 되었는지 묻는 팝업
				MT_DUNGEON_MATCH_REMAKING,					// 던전 매칭중 다른 유저가 취소하여 매칭을 다시 한다는 팝업

// #ifdef ADDED_RELATIONSHIP_SYSTEM
				MT_WAIT_CHOICE_COUPLE,						// 커플 성사 대기!
				MT_AGREE_COUPLE,							// 커플 성사 성공!
// #endif // ADDED_RELATIONSHIP_SYSTEM

				MT_ENTER_GATE_OF_DARKNESS,					// 어둠의 문 입장 여부를 묻는 팝업

				MT_WAIT_SUMMON_MARRIAGED_USER,				// 배우자 소환 요청중 팝업
				MT_SELECT_SUMMON_TO_MARRIAGED_USER,			// 자신을 배우자의 위치로 소환시킬 지 묻는 팝업
				MT_OK_CANCEL,
			};

			// TimedMessagePopUp를 식별하기 위한 uid
			struct TimedPopupUserData
			{
				UidType		iMyUID;
				UidType		iOpponentUID;
				int			iTargetPVPChannel;

				TimedPopupUserData()
				{
					iMyUID = -1;
					iOpponentUID = -1;
					iTargetPVPChannel = -1;
				}

				bool operator ==( const TimedPopupUserData& uniqueID )
				{
					if( iMyUID == uniqueID.iMyUID &&
						iOpponentUID == uniqueID.iOpponentUID )		//kimhc // 2010-05-06 // iOppoentUID 와 uniqueID.iMyUID를 비교하던것을 수정
					{
						return true;
					}

					return false;
				}
			};


		public:
			TimedPopupUserData		m_UserData;			// TimedMessagePopUp을 구분하기 위한 ID로도 사용되고, 실제 필요한 userdata를 저장해두는 변수로도 사용한다.
			MESSAGE_TYPE			m_eMessageType;
			CKTDGUIDialogType			m_pDialog;
			float					m_fTimeLeft;
			wstring					m_wstrMsg;
			int						m_iTimeOutMsg;
			//void (*CBOnTimeOut)( const void* );


			TimedMessagePopUp()
			{
				m_eMessageType = MT_NONE;
				m_pDialog	= NULL;
				m_fTimeLeft = 0.f;
				m_wstrMsg	= L"";
				//CBOnTimeOut = NULL;
				m_iTimeOutMsg = -1;
			}

			~TimedMessagePopUp()
			{
				if( m_pDialog != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDialog, NULL, false );
					m_pDialog = NULL;
				}
			}

			void OnFrameMove( double fTime, float fElapsedTime );
		};


		class CheckWindowInfo : public KJohnThread
		{
		public:

			CheckWindowInfo( CKTDXApp* pKTDXApp, CX2InstanceData* pInstanceData
                )
                : m_pKTDXApp( pKTDXApp )
                , m_pInstanceData( pInstanceData )
			{
				m_bLoop = true;
				m_bSendedMail_Thread = false;
				m_bFindChild_Thread = false;

				m_vecDeskThreadInfo_Thread.clear();
				m_vecTopWndInfo_Thread.clear();
				m_bForceExitThread = false;

                m_fGetCheckKomTime_Thread = 200.f;
                m_vecChangeCheckKom_Thread.clear();
			}
			virtual ~CheckWindowInfo()
			{
				m_bForceExitThread = true;
				EndThread(9000);
			}

			void SetNullToKtdxappAndInstanceData() { m_pKTDXApp = NULL; m_pInstanceData = NULL; }
			void SetForceExitThread( const bool bForceExitThread_ ) { m_bForceExitThread = bForceExitThread_; }

			bool GetIsLoop() { return m_bLoop; }
			bool CheckProcess_Thread();
			void ClearWindowInfo_Thread() { m_vecWindowInfo_Thread.clear(); }
			void PushWindowInfo_Thread( const KHackingToolInfo& windowInfo) { m_vecWindowInfo_Thread.push_back(windowInfo); }
			void SetChildWindow_Thread(bool bVal) { m_bFindChild_Thread = bVal; }

			virtual bool BeginThread()
			{
				m_bLoop = true;
				m_bForceExitThread = false;
				return KJohnThread::BeginThread();
			}//BeginThread()

			virtual void EndThread(DWORD dwTimeout_ = INFINITE)
			{
				m_bLoop = false;
				KJohnThread::EndThread(dwTimeout_);
			}//EndThread()

			virtual DWORD RunThread()
			{
#if !X2_DEV_TOOLS
				ELSWORD_VIRTUALIZER_START
#endif

                g_CKTDXLog.RegisterCurrentThread();

				while(1)
				{
					THEMIDA_ENCODE_START

					if( m_bLoop.Verify() == false || m_bForceExitThread.Verify() == false )
						break;

					if( m_pInstanceData != NULL && m_pKTDXApp != NULL )
					{
                        if ( m_pInstanceData->GetChangedHackList_ThreadSafe( m_vecHackList_Thread ) == true )
                        {
							// 주의 : CheckProcess 내부에 발생하는 연산은 오래 걸린다.
							// 멀티쓰레드 환경에서 오래 걸리는 연산은
							// 게임종료시점에서 CheckProcess 이후에 객체 참조는 위험할수도 있다.
							CheckProcess_Thread();
                        }

						if( m_bForceExitThread == true )
							break;


					}

					Sleep(5000);

					if( m_bForceExitThread == true )
						break;

					THEMIDA_ENCODE_END
				}

				m_bLoop = false;
#if !X2_DEV_TOOLS
				ELSWORD_VIRTUALIZER_END
#endif
				return 0;
			}//RunThread()



		private:

			CKTDXApp* m_pKTDXApp;
			CX2InstanceData *m_pInstanceData;
			std::vector<KHackingToolInfo>	m_vecWindowInfo_Thread;
            std::vector<KHackingToolInfo>   m_vecHackList_Thread;

			KProtectedType<bool> m_bLoop;
			KProtectedType<bool> m_bForceExitThread;
			bool m_bSendedMail_Thread;
			bool m_bFindChild_Thread;

			vector<THREAD_WND_INFO> m_vecDeskThreadInfo_Thread;
			vector<THREAD_WND_INFO> m_vecTopWndInfo_Thread;

            float   m_fGetCheckKomTime_Thread;
            std::vector< pair<string, string> >	m_vecChangeCheckKom_Thread;

		};//class TThread : public KJohnThread


#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
		struct DeveloperScriptSet
		{
			enum DEVELOPER_SCRIPT_TYPE
			{
				DST_GAME_EFFECT_SET = 0,		// 게임 이펙트 셋
				DST_DAMAGE_EFFECT,				// 데미지 이펙트
				DST_GAME_MAJOR_XMESH_PLAYER,	// 게임 메이저 XMesh
				DST_END,
			};
			enum DEVELOPER_SCRIPT_REFRESH_TYPE
			{
				DSCRT_MERGE = 0,		// 머지 시켜줌 ( 우선권은 가장 늦게 불러오는 순 ), 부하가 심할 수 있다. 필요한 부분만 골라서 넣자.
				DSCRT_SWAP,				// 해당 스크립트로 바꿔치기 함
				DSCRT_END,
			};
			struct DeveloperScript
			{
				DeveloperScript () :
					m_wstrDeveloperScriptFileName ( L"" ),
					m_eDeveloperScriptType ( DST_END ),
					m_eDeveloperScriptRefreshType ( DSCRT_MERGE )	{ }

				wstring m_wstrDeveloperScriptFileName;
				DEVELOPER_SCRIPT_TYPE m_eDeveloperScriptType;
				DEVELOPER_SCRIPT_REFRESH_TYPE m_eDeveloperScriptRefreshType;
			};


		public:
			DeveloperScriptSet () { }

			void Clear() { m_vecDeveloperScriptSet.clear(); }		// 초기화
			bool OpenScriptFile( const wstring& wstrFileName )		// 스크립트 파일 불러오기
			{
				lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pDevScript", this );

				if( true == wstrFileName.empty() )
					return false;

				return g_pKTDXApp->LoadLuaTinker( wstrFileName.c_str() );
			}
			bool AddScriptSet ( DEVELOPER_SCRIPT_TYPE eDevScriptType )	// 타입 지정 스크립트 추가
			{
				KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


				DeveloperScript cDeveloperScript;
				LUA_GET_VALUE(				luaManager,		"NAME",			cDeveloperScript.m_wstrDeveloperScriptFileName,		L""	);
				LUA_GET_VALUE_ENUM(			luaManager,		"REFRESH_TYPE",	cDeveloperScript.m_eDeveloperScriptRefreshType,		DEVELOPER_SCRIPT_REFRESH_TYPE,	DSCRT_MERGE );

				cDeveloperScript.m_eDeveloperScriptType = eDevScriptType;

				m_vecDeveloperScriptSet.push_back( cDeveloperScript );
				return true;
			}

			bool AddEffectSetDevFileList_LUA ()	// 이펙트셋 타입 지정 스크립트 추가
			{
				return AddScriptSet ( DST_GAME_EFFECT_SET );
			}

			bool AddDamageEffectDevFileList_LUA ()	// 데미지이펙트 타입 지정 스크립트 추가
			{
				return AddScriptSet ( DST_DAMAGE_EFFECT );
			}
			bool AddGameMajorXMeshPlayerDevFileList_LUA ()	// XMeshPlayer 타입 지정 스크립트 추가
			{
				return AddScriptSet ( DST_GAME_MAJOR_XMESH_PLAYER );
			}

			bool AddDeveloperScript_LUA ()	// 스크립트 추가 기본형
			{
				KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );


				DeveloperScript cDeveloperScript;
				LUA_GET_VALUE(				luaManager,		"NAME",			cDeveloperScript.m_wstrDeveloperScriptFileName,		L""	);
				LUA_GET_VALUE_ENUM(			luaManager,		"SCRIPT_TYPE",	cDeveloperScript.m_eDeveloperScriptType,			DEVELOPER_SCRIPT_TYPE,	DST_END );
				LUA_GET_VALUE_ENUM(			luaManager,		"REFRESH_TYPE",	cDeveloperScript.m_eDeveloperScriptRefreshType,		DEVELOPER_SCRIPT_REFRESH_TYPE,	DSCRT_MERGE );


				m_vecDeveloperScriptSet.push_back( cDeveloperScript );
				return true;
			}

			vector <DeveloperScript> & GetDeveloperScriptSet () { return m_vecDeveloperScriptSet; }
		private :
			vector <DeveloperScript> m_vecDeveloperScriptSet;
		};
#endif // EXPAND_DEVELOPER_SCRIPT	  // 김종훈, 개발자 스크립트 확장 기능 추가


	public:
		CX2Main(void);
		~CX2Main(void);

		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );
		virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//{{ robobeg : 2013-09-17
		//CX2GameOption*	GetGameOption(){ return	&m_GameOption; }
        CX2GameOption&	GetGameOption(){ return	m_GameOption; }
//}} robobeg : 2013-09-17
		//CX2UnitLoader*	GetUnitLoader(){ return	&m_UnitLoader; }
		CX2Data*		GetX2Data(){ return m_pData; }
		CKTDXStage*		GetNowState(){ return m_pNowState; }
		X2_STATE		GetNowStateID() const { return m_NowStateID; }


		CKTDGUIDialogType	KTDGUIOKMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, int iOKMsg = -1, float fTimeLeft = -1.f, wstring wstrFileName = L"",IN const D3DXVECTOR2 vSize_ = D3DXVECTOR2( 0, 0 ), wstring wstrPopupSoundFileName = L"" );

		CKTDGUIDialogType	KTDGUIMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, wstring wstrFileName = L"", wstring wstrPopupSoundFileName = L"" );
		CKTDGUIDialogType	KTDGUIOkAndCancelMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, int okMsg, CKTDXStage* pNowState, int cancelMsg = -1,
			wstring wstrFileName = L"", IN const D3DXVECTOR2 vSize_ = D3DXVECTOR2( 0, 0 ), wstring wstrPopupSoundFileName = L"" );



		CKTDGUIDialogType	KTDGUIOKMsgBoxPlus( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, int iOKMsg = -1, float fTimeLeft = -1.f );

		CKTDGUIDialogType	KTDGUIInfoBox( D3DXVECTOR2 vPos, const WCHAR* pText, CKTDXStage* pNowState, float fLifeTime = -1.f );
		void				DeleteInfoBox();
		CKTDGUIDialogType	GetInfoBox() { return m_pDlgInfoBox; }

		//{{ 허상형 : [2009/10/1] //	EditBox가 포함된 Dialog 생성 함수. iLimitLength = 입력 길이 제한, bIsByte = 길이를 byte로 할지 여부. 디폴트로 글자 갯수로 판단
		CKTDGUIDialogType	KTDGUIOkAndCancelEditBox( D3DXVECTOR2 vPos, const WCHAR* pText, int iOkMsg, CKTDXStage* pNowState, int iLimitLength = 40, bool bIsByte = false, int iCancelMsg = -1 );
		CKTDGUIDialog*	KTDGUIOKMsgBox_LUA( D3DXVECTOR2 pos, const char* pText, CKTDXStage* pNowState );
		CKTDGUIDialog*	KTDGUIOkAndCancelMsgBox_LUA( D3DXVECTOR2 pos, const char* pText, int okMsg, CKTDXStage* pNowState );


		CKTDGUIDialogType		KTDGUIOkAndCancelEditBox2( D3DXVECTOR2 vPos, const WCHAR* pText, int iOkMsg, CKTDXStage* pNowState, int iLimitLength, bool bIsByte, int iCancelMsg );


		bool			IsWaitingServerPacket( const SERVER_EVENT_ID waitACK );

		void			AddServerPacket( SERVER_EVENT_ID waitACK, float fReamainTimeOut = 0.0f );
		bool			DeleteServerPacket( SERVER_EVENT_ID waitACK );
		void			AllDeleteServerPacket();

		void			AddChannelServerIP( const WCHAR* pServerIP );
		void			AddChannelServerIP_LUA( const char* pServerIP, int iServerPort = 9400 );

		int GetPickedChannelServerIPIndex() const { return m_iPickedChannelServerIPIndex; }
		void SetPickedChannelServerIPIndex(int val) { m_iPickedChannelServerIPIndex = val; }
		int GetDefaultChannelServerIPIndex() const { return m_iDefaultChannelServerIPIndex; }
		void SetDefaultChannelServerIPIndex(int val) { m_iDefaultChannelServerIPIndex = val; }

		int GetChannelServerPort() const { return m_iChannelServerPort; }
		void SetChannelServerPort(int val) {
			m_DefaultChannelServerInfo.m_usMasterPort = val;
		}

		// 09.07.03 태완 : 사내에서 계정생성 되게
#if X2_DEV_TOOLS
		int GetGameServerPortForCreateID() const { return m_iGameServerPortForCreateID; }
		void SetGameServerPortForCreateID(int val) { m_iGameServerPortForCreateID = val; }
		void AddGameServerIPForCreateID_LUA( const char* pServerIP )
		{
			if( NULL == pServerIP )
				return;

			m_wstrGameServerIPForCreateID = L"";
			ConvertUtf8ToWCHAR( m_wstrGameServerIPForCreateID, pServerIP );

		}
		wstring GetGameServerIPForCreateID(){ return m_wstrGameServerIPForCreateID; }
#endif //!defined(_SERVICE_) || defined(SERV_JOIN_IN_CLIENT_FOR_TW_TEST_SERVER)

		KServerGroupInfo GetPickedChannelServer() const;

		UidType         ExtractServerGroupID( UidType iUID_ )
		{
#ifdef EXTEND_SERVER_GROUP_MASK
			return ( iUID_ >> 52 ) & ( UidType )0x00000000000003ff;
#else
			return ( iUID_ >> 56 ) & ( UidType )0x000000000000003f;
#endif // EXTEND_SERVER_GROUP_MASK
		}

		// 게임서버 관련, p2p 관련
		const WCHAR*	GetConnectedGameServerIP() const { return m_ConnectedGameServerIP.c_str(); }
		void			SetConnectedGameServerIP(const std::wstring& val) { m_ConnectedGameServerIP = val; }
		int				GetConnectedGameServerPort() const { return m_iConnectedGameServerPort; }
		void			SetConnectedGameServerPort(int val) { m_iConnectedGameServerPort = val; }

		void			SetServerUDPPort( int port ){ m_ServerUDPPort = port; }		// 게임서버 udp relay port
		const int		GetServerUDPPort(){ return m_ServerUDPPort; }

		void			SetGameP2PPort( int port ){ m_GameP2PPort = port; }
		const int		GetGameP2PPort(){ return m_GameP2PPort; }

		void			SetServerTimeOut_LUA(float fTime) { m_fServerTimeOut = fTime; }
		float			GetServerTimeOut() { return m_fServerTimeOut; }





		const WCHAR*	GetTRServerIp() { return m_TRServerIp.c_str(); }
		void			SetTRServerIp( const wstring &wstrTRServerIp ) { m_TRServerIp = wstrTRServerIp; };

		int				GetFrameBufferNum(){ return m_FrameBufferNum; }

		void			SetAutoLoginUnitIndex(int index) { m_AutoLoginUnitIndex = index; }
		void			SetSafetyKey_LUA( const char* pKey );
		wstring			GetSafetyKey(){ return m_SafetyKey.c_str(); }

		bool				m_bAutoStart;
		int					m_AutoLoginUnitIndex;

		void			ResetFullScreenResolution(int iWidth, int iHeight);
		bool			IsValidPacket( int enumID );

		void			SetTutorialRandomLoadingBG( int iRandIndex ) { m_iRandLoadingBG = iRandIndex; }
		int				GetTutorialRandomLoadingBG() { return m_iRandLoadingBG; }
		const bool		GetIsPlayingTutorial() { return m_bIsPlayingTutorial; }
		void			SetIsPlayingTutorial( bool bPlaying ) { m_bIsPlayingTutorial = bPlaying; }
		const bool		GetIsExitingTutorial() { return m_bIsExitingTutorial; }
		void			SetIsExitingTutorial( bool bExiting ) { m_bIsExitingTutorial = bExiting; }
		X2_STATE		GetStateIDBeforeTutorial() { return m_StateIDBeforeTutorial; }
		void			SetStateIDBeforeTutorial( X2_STATE stateID ) { m_StateIDBeforeTutorial = stateID; }

		void			SetPlayFieldDungeon( bool bPlay) { m_bIsPlayFieldDungeon = bPlay; }
		const bool		GetPlayFieldDungeon() { return m_bIsPlayFieldDungeon; }
		void			SetExitFieldDungeon( bool bPlay) { m_bIsExitFieldDungeon = bPlay; }
		const bool		GetExitFieldDungeon() { return m_bIsExitFieldDungeon; }


		CX2StateAutoChanger& GetStateAutoChanger() { return m_AutoStateChanger; }

		CX2StringFilter* GetStringFilter() { return m_pStringFilter; }

		bool			InstallFont( const char* pValueName, const char* pFontFileName );
		X2_PUBLISHER	GetPublisher(){ return m_X2Publisher; }
		bool			GetManualLogin(){ return m_bManualLogin; }

		void			SetClientVersion_LUA( char* pVersion );
		const WCHAR*	GetClientVersion(){ return m_ClientVersion.c_str(); }
		void			SetSubClientVersion_LUA( char* pVersion );
		const WCHAR*	GetSubClientVersion_LUA(){ return m_SubClientVersion.c_str(); }


		void			ChangeKoreanAndEnglishKey( bool bKorean );

//{{ kimhc // 2010.3.12 //	채팅창 개편
		bool			GetNativeAndEnglisKeyState();//현재 한영 모드 알아내기 // TRUE면 한글
//}} kimhc // 2010.3.12 //	채팅창 개편

//{{ kimhc // 2010.9.14 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
//}} kimhc // 2010.9.14 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

		void			ResetCrashReport( const WCHAR* wszAddMsg );

		CKTDGFontManager::CUKFont* GetFontForDebuf() { return m_pFontForDebug; }

		UINT			GetConnectedChannelID() { return m_iConnectChannelID; }
		void			SetConnectedChannelID( UINT channelID ) { m_iConnectChannelID = channelID; }

		std::vector<KPVPChannelInfo>& GetChannelList() { return m_vecChannel; }
		void			SetChannelList( std::vector<KPVPChannelInfo>& vecChannel ) { m_vecChannel = vecChannel; }



		// note: 대전채널에 입장한 경우에만 사용해야 함~
		KPVPChannelInfo::PVP_CHANNEL_CLASS GetPVPChannelClass( const UINT iChannelID )
		{
			for( UINT i=0; i<m_vecChannel.size(); i++ )
			{
				KPVPChannelInfo& channelInfo = m_vecChannel[i];
				if( channelInfo.m_iChannelID == iChannelID )
				{
					return (KPVPChannelInfo::PVP_CHANNEL_CLASS) channelInfo.m_cPVPChannelClass;
				}
			}

			return KPVPChannelInfo::PCC_NONE;
		}



		bool			SendQuitMsgToServer();
		int				GetServerUserCount() const		{ return m_iServerUserCount; }
		void			SetServerUserCount( int val )	{ m_iServerUserCount = val; }
		map<int, int>&	GetMapServerPVPRoomCount()		{ return m_mapServerPVPRoomCount; }
		void			SetMapServerPVPRoomCount(map<int, int> val) { m_mapServerPVPRoomCount = val; }
		//{{ 2009. 4. 1  최육사		파티정보
		int				GetCurPartyCount() const		{ return m_iCurPartyCount; }
		void			SetCurPartyCount(int val)		{ m_iCurPartyCount = val; }
		int				GetPlayGamePartyCount() const	{ return m_iPlayGamePartyCount; }
		void			SetPlayGamePartyCount(int val)	{ m_iPlayGamePartyCount = val; }
		//}}



		bool			DeleteFile_LUA( const char* pFileName );
		void			SetNationFlag_LUA( DWORD dwFlag ) { m_dwNationFlag = dwFlag; }
		DWORD			GetNationFlag() { return m_dwNationFlag; }

		CX2GameEdit*	GetGameEdit() { return m_pGameEdit; }
		CX2TextManager*	GetTextManager() { return m_pTextManager; }
		CX2NoticeManager* GetNoticeManager() { return m_pNoticeManager; }

		void			SetQuitType( int quitType ) { m_QuitType = quitType; }
		void			SetBetweenCharToCharSize( HDC hdc, int sizeValue = 0 ) { SetTextCharacterExtra( hdc, sizeValue ); }

		void			BuyItemSuper_LUA( int itemID, int iQuantity = 1, short sPeriod = 0 );

		void			SetAuthLevel_LUA( const CHAR* userID, CX2User::X2_USER_AUTH_LEVEL level );
		void			GetAuthUserList_LUA( CX2User::X2_USER_AUTH_LEVEL level );

		void			BuySuperEqipLevel_LUA( int level );
		//{{ 2008. 1. 2  최육사  근성도 치트
		void			ChangeSpirit( int iChangeSpirit );
		//}}
		//{{ 2008. 1. 25  최육사  VP 치트
		void			ChangeVSPoint( int iRating, int iRPoint, int iAPoint );

		//}}
		//{{ 2008. 2. 18  최육사  유닛 클래스 치트
		void			ChangeUnitClass( char cUnitClass );
		void			ModifyChangeUnitClass( const char* UnitClassName );
		//}}
		//{{ 2008. 10. 24  최육사	타이틀 치트
		void			GetTitleCheat( int iTitleID, int iPeriod = 0 );
		//}}

		void			ChangeCashSkillPointDate( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ );

		void 			InitMyUnitSkillTree();
		void 			SetMyUnitSP( int iSP );

		void			FullMyUnitSkillTree();

		//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력

		void			FullMyUnitSkillTreeByLevel();
		void			SetMyUnitSkillLevel( const int iSkillID, const int iSkillLevel );


		void 			SendStatisticsLocLog();
		void 			SendStatisticsDBLog();


		void			AddDungeonRewardItem( std::vector<KItemInfo>& vecDungeonRewardItem );
		const std::vector<KItemInfo>& GetDungeonRewardItem() { return m_vecDungeonRewardItem; }
		void			ResetDungeonRewardItem()
		{
			m_vecDungeonRewardItem.resize(0);
		}

		bool			GetCenterServerDie() { return m_bCenterServerDie; }
		void			SetCenterServerDie(bool bCheck) { m_bCenterServerDie = bCheck;  }

		void			Handler_EGS_CHECK_SUM_REQ( bool bFileCheck = false );
		//{{김준환 서버시간 받아오기

		int				GetTrainingRewardED() const { return m_iTrainingRewardED; }
		void			SetTrainingRewardED(int val) { m_iTrainingRewardED = val; }
		int				GetTrainingRewardEXP() const { return m_iTrainingRewardEXP; }
		void			SetTrainingRewardEXP(int val) { m_iTrainingRewardEXP = val; }
		bool			GetTrainingSuccess() const { return m_bTrainingGameSuccess; }
		void			SetTrainingSuccess(bool val) { m_bTrainingGameSuccess = val; }

		CX2CouponBox*	GetCouponBox() const { return m_pCouponBox; }


		CX2TutorSystem* GetTutorSystem() const
		{
			return NULL;
		}

        CX2PostBox*     GetPostBox() const { return m_pPostBox; }
        CX2SecurityPad*	GetSecurityPad() const { return m_pSecurityPad; }
		wstring			GetEDString( int edToChange );
		wstring			GetEDString( const WCHAR* pEDToChange );
		int				GetEDFromString( const WCHAR* wszED );
		D3DXCOLOR		GetPriceColor( __int64 iPrice );



		bool 			GetLastStageWasTrainingGame() const { return m_bLastStageWasTrainingGame; }
		void 			SetLastStageWasTrainingGame(bool val) { m_bLastStageWasTrainingGame = val; }
		int				GetNowTrainingID() const { return m_iNowTrainingID; }
		void			SetNowTrainingID(int val) { m_iNowTrainingID = val; }
		bool 			GetAutoStartNextTraining() const { return m_bAutoStartNextTraining; }
		void 			SetAutoStartNextTraining(bool val) { m_bAutoStartNextTraining = val; }

		bool 			GetIsPlayingFreeTraining() const { return m_bIsPlayingFreeTraining; }
		void 			SetIsPlayingFreeTraining(bool val) { m_bIsPlayingFreeTraining = val; }


		CX2PVPEmblem*	GetPVPEmblem() { return m_pX2PVPEmblem; }
		CX2InformerManager* GetInformerManager() { return m_pInformerManager; }
		CX2MemoryHolder*	GetMemoryHolder() { return m_pMemoryHolder; }

		void			IntentionalCrash();	// 고의로 crash 내는 함수
		void			SetFreeCamera( bool bEnable );
		bool			GetFreeCamera() const ;
		void			SetWorldCameraEdit( bool bEnable);
		void			PlayWorldCamera( int iWorldCameraID );
		void			EndWorldCamera();
		void			SetProjectionFov( float fFov );
		D3DXVECTOR2		GetWindowPos( D3DXVECTOR2 windowSize, D3DXVECTOR2 wantMousePoint );
		void			OpenURL( WCHAR* pURL );
		void			CreateStateChangeDLG( const WCHAR* szExplanation, wstring wstrCustomLuaFileName = L"", wstring wstrPlaySoundFileName = L"" );
		void			DeleteStateChangeDLG();


		void			SetShowUnitViewerUI( bool bShow ) { CX2UnitViewerUI::SetShowUnitViewerUI( bShow ); }

		wstring			GetStrByLienBreak( const WCHAR* pOrgStr, int width, int fontIndex );
		wstring			GetStrByLineBreakColor( const WCHAR* pOrgStr, int width, int fontIndex );

		void			SetSquareMaxUserNum( int userNum ) { m_SquareMaxUserNum = userNum; }
		int				GetSquareMaxUserNum() { return m_SquareMaxUserNum; }

		KBrowserWrapper* GetBrowserWrapper() { return &m_BrowserWrapper; }

		CX2LVUpEventMgr* GetLVUpEventMgr() { return m_pLVUpEventMgr; }

		//void			ResetLuaBinder( KLuabinder* pKLuabinder );

		void			AddMemoryInfo( stringstream& strstm );

		void			AddSystemInfo(stringstream& strstm);
		void			AddDLLInfo(stringstream& strstm);
		void			AddProcessInfo(stringstream& strstm, bool bThreadInfo = true);
		void 			UpdateProcessList();
		void 			SetMailNameToFindHack( const WCHAR* wszMailName ) { m_MailNameToFindHack = wszMailName; }
        void            SetUserIdToFindHack_ThreadSafe( const std::string& strUserId )
        {
            CSLock      lock(m_csUserIdToFindHack);
            m_strUserIdToFindHack_CS = strUserId;
        }
        void            GetUserIdToFindHackIfNotEmpty_ThreadSafe( std::string& strID )
        {
            CSLock      lock(m_csUserIdToFindHack);
            if ( m_strUserIdToFindHack_CS.empty() == false )
                strID = m_strUserIdToFindHack_CS;
        }
        bool            IsSameUserIdToFindHack_ThreadSafe( const std::string& strID )
        {
            CSLock      lock(m_csUserIdToFindHack);
            return strID.compare( m_strUserIdToFindHack_CS ) == 0;
        }
		void			SetHackingUser( bool b ) { m_bHackingUser = b; }
		void 			SendHackMail();
		void			SendHackMail_DamageHistory(const char *strComm);
		void			SendHackMail_DLLInjection(const char *strComm);
		void			SendUserMonitoringMail(const char *strComm);

		void			SendHackMail_HackUserNot(const char *strComm);

		void			SendHackMail_AttachFile(const char *strComm, const string& userId);
		void			SendHackMail_FileList(const char *strComm, const string& userId);
		void			SendHackMail_VERIFYVALUE(const char *strComm);
		void			SendHackMailGameStart(int iVal, bool bVal = false);

		void			SendMail_MoneyMonitoring(const char *strComm);

		void			SetPublicIp(wstring wstrPublicIp) { m_wstrPublicIp = wstrPublicIp; }


		bool			GetPresentArrival( CX2StateMenu::PresentArrival& presentArrival );
		void			PushPresentArrival( KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT& kEvent );


		CX2PartyUI*		ResetPartyUI()
		{
			DeletePartyUI();
			m_pPartyUI = new CX2PartyUI((CKTDXStage*) GetNowState());
			return m_pPartyUI;
		}
		CX2PartyUI*		GetPartyUI()
		{
			if( NULL == m_pPartyUI )
			{
				ResetPartyUI();
			}

			return m_pPartyUI;
		}
		void DeletePartyUI() { SAFE_DELETE( m_pPartyUI ); }


		void ClearReservedMessagePopup()
		{
			m_vecReservedMessagePopUp.resize(0);
		}
		void AddReservedMessagePopup( ReservedMessagePopUp& reservedMsg )
		{
			m_vecReservedMessagePopUp.push_back( reservedMsg );
		}
		void PopupReservedMessage();


		void ClearTimedMessagePopUp() { m_listTimedMessagePopUp.clear(); }
		TimedMessagePopUp* GetTimedMessagePopup( CKTDGUIDialogType pDialog );

		//void ClearTimedMessagePopup();
		void RemoveTimedMessagePopup( TimedMessagePopUp::MESSAGE_TYPE eMessageType, const TimedMessagePopUp::TimedPopupUserData& userData );
		void AddTimedMessagePopup( TimedMessagePopUp::MESSAGE_TYPE eMessageType,
			TimedMessagePopUp::MESSAGE_BOX_TYPE eBoxType,
			const TimedMessagePopUp::TimedPopupUserData& userData, const float fTime,
			const wstring& wstrMsg, CKTDXStage* pStage, int okMsg, int cancelMsg, int timeOutMsg = -1 );


		bool IsThereTimedMessagePopup( const TimedMessagePopUp::MESSAGE_TYPE eMessageType )
		{
			BOOST_TEST_FOREACH( const TimedMessagePopUp&, timedMessage, m_listTimedMessagePopUp )
			{
				if( eMessageType == timedMessage.m_eMessageType )
				{
					return true;
				}
			}

			return false;
		}








		bool			KeyProcess();

		static int LineBreak( wstring& wstrText, const CKTDGFontManager::CUKFont* pFont, const int iTextWidth, const wstring& wstrTextColor, const bool bEnableColor, bool bCutRedundantEmptyLine = true );
		static bool IsInvalidNickName( const wstring& wstrNickName, wstring& wstrErrorMessage );



		void AddReservedServerPacket( SERVER_EVENT_ID eServerEventID, void* pEvent )
		{
			ReservedServerPacket reservedServerPacket;
			reservedServerPacket.m_eServerEventID = eServerEventID;
			reservedServerPacket.m_pEvent = pEvent;

			m_vecReservedServerPacket.push_back( reservedServerPacket );
		}

		//bool PopReservedServerPacket( ReservedServerPacket& reservedServerPacket )
		//{
		//	if( false == m_vecReservedServerPacket.empty() )
		//	{
		//		reservedServerPacket = m_vecReservedServerPacket[0];
		//		m_vecReservedServerPacket.erase( m_vecReservedServerPacket.begin() );
		//		return true;
		//	}

		//	return false;
		//}

		//bool PeepReservedServerPacket( ReservedServerPacket& reservedServerPacket )
		//{
		//	if( false == m_vecReservedServerPacket.empty() )
		//	{
		//		reservedServerPacket = m_vecReservedServerPacket[0];
		//		return true;
		//	}
		//
		//	return false;
		//}


		bool IsMyAuthLevelHigherThan( CX2User::X2_USER_AUTH_LEVEL eAuthLevel )
		{
			if( false == GetEnabledDeveloperCheatCode() )
				return false;

			if( NULL == m_pData )
				return false;

			if( NULL == m_pData->GetMyUser() )
				return false;

			if( m_pData->GetMyUser()->GetAuthLevel() < eAuthLevel )
				return false;

			return true;
		}

		bool GetEnabledDeveloperCheatCode() const { return m_bEnabledDeveloperCheatCode; }
		void SetEnabledDeveloperCheatCode(const bool val) { m_bEnabledDeveloperCheatCode = val; }


		//{{ 2009. 7. 26  최육사	party fever cheat
		void IncreasePartyFeverPoint( int iCount );
		//}}

		//{{ 2009. 8. 5  최육사		체험ID치트
		void SetGuestUserMode( bool bVal );
		//}}

		//{{ 허상형 : [2010/3/12/] //	마을 이동 워프 치트
		void WarpByCommand( int iPosition );
		//}} 허상형 : [2010/3/12/] //

		//{{ 오현빈 // 2012-08-22 // 필드 이동 워프 치트
		void FieldWarpByCommand( int iPosition ) ;
		//}} 오현빈 // 2012-08-22 // 필드 이동 워프 치트


		void MakeHackScreenShotTemp( const char *pFileName );
		void MakeHackScreenShot( const char *pFileName );


		CheckWindowInfo *GetCheckWindowInfo() { return m_threadCheckWindowInfo; }

		void			DllManagerReport();

//{{ megagame / 박교현 / 2010.05.12 / 시스템 정보
		CX2SystemInfo*	GetSystemInfo() const	{ return m_pSystemInfo; }
//}} megagame / 박교현 / 2010.05.12 / 시스템 정보


		//{{ JHKang // 강정훈 // 2010/10/06
		bool IsPopUpOnChat(void) { return m_bIsPopUpOnChat; }
		void SetPopUpOnChat( bool bSet ) { m_bIsPopUpOnChat = bSet; }
		//}} JHKang // 강정훈 // 2010/10/06

		std::string GetMachineId();

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        void    SetUDPMode_LUA( const char* pszGameType, const char* pszUDPMode );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        void    SetUDPMode( CX2Game::GAME_TYPE eGameType, CKTDNUDP::EForceConnectMode eUDPMode );
        CKTDNUDP::EForceConnectMode GetUDPMode( CX2Game::GAME_TYPE eGameType );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK





		//{{ 2011.9.16	이준호  반디캠 동영상 캡쳐 지원
		bool					Bandi_StartVideoCapture();
		void					Bandi_Pause_Restart_VidieCapture(bool bPause);	// bPause : true이면 일시 정지, false 일시 정지 해제
		void					Bandi_EndVidioCapture();
		void					Bandi_EnoughDiskSpace(float fElapsedTime);
		bool					IsCapturing()
		{
			if( m_BandiCaptureLibrary.IsCapturing() == TRUE )
				return true;
			return false;
		};

		//}}

		//{{ 2011.9.16	이준호  반디캠 스크린샷 캡쳐 지원
		bool					Bandi_ScreenCapture();
		float					m_fCaptureDelay;
		CBandiCaptureLibrary	m_BandiCaptureLibrary;
		//}}

		void			SetHackToolTime(float fVal) { m_fCheckHackToolTime = fVal; }


		void CreatePlayTimeInformation();														//플레이 시간 안내창 생성
		void CreateShutDownInformation();							//셧다운 안내창 생성

		void FadePlayTimeInformation( float fShowTime );	//플레이 시간 안내창 Fade 효과
		void DeletePlayTimeInformation();					//플레이 시간 안내창 삭제

		void FadeShutDownInformation( float fShowTime );	//셧다운 안내창 Fade 효과
		void DeleteShutDownInformation();					//셧다운 안내창 삭제

		void SetShutDownString( wstring wstrShutDownString ) { m_wstrShutDownString = wstrShutDownString; }		//셧다운 안내 문구 설정
		void SetShutDownInformation( bool bShutDownInfo ) { m_bProcessShutDownInformation = bShutDownInfo; }	//셧다운 생성 설정

		CX2KeyPad*	GetKeyPad() const { return m_pKeyPad; }
		bool GetSurveyUser() const { return m_bSurveyUser; }
		void SetSurveyUser(bool val) { m_bSurveyUser = val; }


		//CX2StateMenu를 상속받지 않는 스테이트 체크
		inline bool IsInheritStateMenu()
		{
			switch( m_NowStateID )
			{
			case XS_START_UP:
			case XS_LOGIN:
			case XS_JOIN:
			case XS_LOADING:
			case XS_SERVER_SELECT:
			case XS_CREATE_UNIT:
			case XS_PVP_GAME:
			case XS_PVP_RESULT:
			case XS_DUNGEON_RESULT:
				return false;
				break;
			default:
				return true;
				break;
			}
		}









		bool GetIsValideCheckKomScript() const { return m_bIsValideCheckKomScript; }
		void SetIsValideCheckKomScript(bool val) { m_bIsValideCheckKomScript = val; }

#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
		bool ResetDeveloperScriptSet ( const WCHAR* pFileName );	// DevScriptTable.lua 파싱
		bool ResetDeveloperScriptSet_GameEffectSet ( DeveloperScriptSet::DeveloperScript devScript );
		bool ResetDeveloperScriptSet_DamageEffect ( DeveloperScriptSet::DeveloperScript devScript );
		bool ResetDeveloperScriptSet_GameMajorXMeshPlayer ( DeveloperScriptSet::DeveloperScript devScript );
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가

		// Wstring 도 GET_REPLACED_STRING 사용 할 수 있도록 추가
		// 스킬 설명 확장이 필요한 경우의 예외 처리, 에너지 스퍼트의 @1 ( 현재 레벨 표기 ) 때문에 추가
		wstring GetReplacedWstring( wstring & wstrModify, char* szParamTypeList, ... );
	private:
		void			RegisterLuabind();
        bool			OpenScriptFile( const WCHAR* pFileName );
		bool			StateChange( int stateID, int iDetailStateID );
		void			PostStateChange();
		void			ReservedServerEventProc();



		bool			DeleteFile( const WCHAR* pFileName );

		void			StateInLog();



		void			DeleteNMCachData();
		void			LoadLuaEnum();








		bool GetWallPaperDirectory( char * pszDir, int iDirSize );

		//{{ 2013.02.27 조효진	해외 기본 구조 작업 (SERV_GLOBAL_BASE)
		std::wstring GetWstrBasicConfigFile();
		//}}
		//{{김준환 서버시간 받아오기



	private:

		wstring				m_ClientVersion;
		wstring				m_SubClientVersion;

		X2_STATE			m_NowStateID;
		CX2State*			m_pNowState;

		int					m_iChannelServerPort;
		std::vector<wstring> m_vecChannelServerIP;
		int					m_iPickedChannelServerIPIndex;
		int					m_iDefaultChannelServerIPIndex;
		KServerGroupInfo	m_DefaultChannelServerInfo;

		std::wstring		m_ConnectedGameServerIP;		// 접속된 게임서버 ip
		int					m_iConnectedGameServerPort;		// 접속된 게임서버 port
		int					m_ServerUDPPort;				// 접속된 게임서버 udp relay port
		int					m_GameP2PPort;
		int					m_FrameBufferNum;

		CX2Data*			m_pData;
		wstring				m_SafetyKey;

		float				m_fServerTimeOut;

		vector<WaitServerPacket*>	m_WaitServerPacketList;
		map<int, int>				m_mapWaitServerPacketList;	// only for debugging; <eventid, count>


		bool				m_bIsPlayFieldDungeon;
		bool				m_bIsExitFieldDungeon;
		bool				m_bIsPlayingTutorial;
		bool				m_bIsExitingTutorial;
		X2_STATE			m_StateIDBeforeTutorial;
		int					m_iRandLoadingBG;



		CX2StringFilter*	m_pStringFilter;
		CX2GameOption		m_GameOption;
		//CX2UnitLoader		m_UnitLoader;

		X2_PUBLISHER		m_X2Publisher;
		bool				m_bManualLogin;

		LPVOID				m_pCrashReportState;

		CKTDGFontManager::CUKFont* m_pFontForDebug;

		UINT							m_iConnectChannelID;
		std::vector<KPVPChannelInfo>	m_vecChannel;

		float						m_fServerDataTime;
		int							m_iServerUserCount;
		map<int, int>				m_mapServerPVPRoomCount;
		//{{ 2009. 4. 1  최육사		파티정보
		int							m_iCurPartyCount;
		int							m_iPlayGamePartyCount;
		//}}

		CX2GameEdit*				m_pGameEdit;
		CX2TextManager*				m_pTextManager;
		CX2NoticeManager*			m_pNoticeManager;

		int							m_QuitType;

		std::vector<KItemInfo>		m_vecDungeonRewardItem;

		bool						m_bCenterServerDie;

		CX2StateAutoChanger			m_AutoStateChanger;
		CX2CheckSumManager*			m_pCheckSumManager;

		wstring						m_PlannerServerIP;

		int							m_iTrainingRewardED;
		int							m_iTrainingRewardEXP;
		bool						m_bTrainingGameSuccess;
		bool						m_bLastStageWasTrainingGame;
		bool						m_bIsPlayingFreeTraining;

		bool						m_bAutoStartNextTraining;
		int							m_iNowTrainingID;


		CX2TutorSystem*				m_pTutorSystem;

		CX2CouponBox*				m_pCouponBox;


		CX2PVPEmblem*				m_pX2PVPEmblem;

        CX2PostBox*                 m_pPostBox;
		CX2SecurityPad*				m_pSecurityPad;
		bool						m_bAutoQuit;
		float						m_fAutoQuitTime;
		float						m_fAutoQuitCoolTime;

		CX2InformerManager*			m_pInformerManager;
		CX2MemoryHolder*			m_pMemoryHolder;

		HINTERNET					m_hInternet;

		CKTDGUIDialogType				m_pDLGStateChange;
		vector<wstring>				m_KOMList;
		vector<wstring>				m_MP3List;

		vector<wstring>				m_UseFontNameList;

		int							m_SquareMaxUserNum;


		KBrowserWrapper				m_BrowserWrapper;		// 넥슨 캐시결제 홈페이지 띄우기 위한 browser


		CX2LVUpEventMgr*			m_pLVUpEventMgr;

		float						m_fTimeToUpdateProcessList;

		wstring						m_MailNameToFindHack;
        MemberCriticalSection       m_csUserIdToFindHack;
        std::string                 m_strUserIdToFindHack_CS;
		bool						m_bHackingUser;
        MemberCriticalSection       m_csProcessNameToFindHack;
		set<wstring>				m_setProcessNameToFindHack_CS;
        MemberCriticalSection       m_csSendedProcessList;
		set<wstring>				m_setSendedProcessList_CS;

		queue< CX2StateMenu::PresentArrival > m_quePresentArrival;

		vector<ReservedMessagePopUp>	m_vecReservedMessagePopUp;

		//vector<TimedMessagePopUp>		m_vecTimedMessagePopUp;
		list<TimedMessagePopUp>			m_listTimedMessagePopUp; // kimhc // 2010.01.22 // list로 수정


		CX2PartyUI*					m_pPartyUI;








		wstring						m_TRServerIp; // 현재 접속한 채널서버 IP



		std::vector< ReservedServerPacket > m_vecReservedServerPacket;
//{{ 09.07.03 태완 : 사내에서 계정생성 되게
#if X2_DEV_TOOLS
		int m_iGameServerPortForCreateID;
		wstring m_wstrGameServerIPForCreateID;
#endif //!defined(_SERVICE_) || defined(SERV_JOIN_IN_CLIENT_FOR_TW_TEST_SERVER)
//}}
		wstring m_wstrID;
		wstring m_wstrPass;

		bool m_bEnabledDeveloperCheatCode;	// 개발자만 제한된 기능을 사용할지 말지

		CKTDGUIDialogType m_pDlgInfoBox;
		float m_fInfoBoxLifeTime;

		CheckWindowInfo					*m_threadCheckWindowInfo;


		float			m_fCheckDebugeTimer;


//{{ megagame / 박교현 / 2010.05.12 / 시스템 정보
		CX2SystemInfo*					m_pSystemInfo;
//}} megagame / 박교현 / 2010.05.12 / 시스템 정보

		//{{ JHKang // 강정훈 // 2010/10/06
		bool		m_bIsPopUpOnChat;
		//}} JHKang // 강정훈 // 2010/10/06

		wstring		m_wstrPublicIp;


		DWORD		m_dwNationFlag;

		float m_fCheckHackToolTime;
		float m_fFileSizeCheckTime;
		__int64 m_iOldCaptureFileSize;

		int				  m_iGameHour;						//클라이언트 시작 후 지난 시간 반환

		CKTDGUIDialogType m_pDlgPlayTimeInformation;		//플레이 시간 안내용 Dialog
		float			  m_fPlayTimeInfoShowTime;			//안내창 유지 시간
		bool			  m_bProcessPlayTimeInformation;	//안내창 Fade 기능 적용 여부


		CKTDGUIDialogType m_pDlgShutDownInformation;		//셧다운 안내용 Dialog
		float			  m_fShutDownInfoShowTime;			//셧다운 안내창 유지 시간
		bool			  m_bProcessShutDownInformation;	//셧다운 안내창 Fade 기능 적용 여부

		wstring			  m_wstrShutDownString;				//셧다운 안내창용 문구







		CX2KeyPad*		m_pKeyPad;
		bool		m_bSurveyUser;			/// 설문조사 대상 유저




		bool m_bIsValideCheckKomScript;	// CheckKom.xml 유효 여부


#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
public:
	void OccurLag_LUA( float fLagTime, int iLagType );

private:
	void _InitKOGGamePerformanceCheck();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
public:
	void ForceHost_LUA( bool bEnable );
	void SwitchConnect_LUA( int iMode );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef UDP_PACKET_ANALYSIS_LOG
public:
	void UdpPacketAnalysisLog_LUA( bool bEnable );
#endif //UDP_PACKET_ANALYSIS_LOG

//{{김준환 서버시간 받아오기

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
public:
	void UdpPacketOverlapTest_LUA( bool bEnable );

	bool IsEnableUdpPacketOverlap() { return m_bUdpPacketOverlap; }

private:
	bool m_bUdpPacketOverlap;

    CKTDNUDP::EForceConnectMode m_aeUDPMode[CX2Game::GT_NUMS];
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK




	//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력



#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
	DeveloperScriptSet			m_DeveloperScriptSet;
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가

public:
	bool	m_bPlayIntroMovie;

}; // CX2Main

extern CX2Data*			g_pData;
extern CX2InstanceData*	g_pInstanceData;
extern CX2Main*			g_pMain;
extern CX2Game*			g_pX2Game;
extern CX2Room*			g_pX2Room;
extern CX2SquareGame*	g_pSquareGame;
extern CX2WeddingGame*	g_pWeddingGame;


extern CX2TFieldGame*	g_pTFieldGame;

extern CX2ChatBox*		g_pChatBox;

extern FILE*			g_fpStream;
extern FILE*			g_fpStreamError;
extern HINTERNET		g_hInternet;




inline void OpenGlobalURL( WCHAR* pURL )
{
	if( g_hInternet == NULL )
		return;

	// 파일이 있는 URL을 열어놓는다.
	HINTERNET hURLFile;
	if ( NULL == ( hURLFile = ::InternetOpenUrl( g_hInternet,
		pURL,
		NULL,
		0,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE,
		0 ) ) )
	{
		ErrorLog( XEM_ERROR107 );
		return;
	}

	::InternetCloseHandle( hURLFile );
}




__forceinline bool CX2Main::OpenScriptFile( const WCHAR* pFileName )
{

	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pMain", this );


    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		ErrorLogMsg( XEM_ERROR14, pFileName );
		return false;
    }

	return true;
}
