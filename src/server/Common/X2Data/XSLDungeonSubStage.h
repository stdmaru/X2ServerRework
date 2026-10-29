
#include "XSLStat.h"
#include "XSLUnitManager.h"
#include "d3dx9math.h"

//{{ 2012. 12. 21  던전 몬스터 그룹 랜던 배치 - 김민성
#include <Lottery.h>
//}}

class CXSLDungeonSubStage
{
public:
	enum CLEAR_TYPE
	{
		CT_SUB_STAGE	= 0,
		CT_STAGE		= 1,
		CT_STAGE_FORCE	= 2,
		CT_STAGE_SECRET	= 3,
		CT_GAME			= 4,
		CT_FAILURE		= 5,
	};

	enum GO_TYPE
	{
		GT_NONE = 0,
		GT_RIGHT,
		GT_LEFT,
		GT_UP,
		GT_DOWN,
	};

	enum SECRET_STAGE_PAD
	{
		SSP_NONE = 0,
		SSP_SECRET,
		SSP_ELITE,
		SSP_NORMAL,
	};

	struct NextStageData
	{
		CLEAR_TYPE m_eClearType;
		int m_iStageIndex;
		int m_iSubStageIndex;
		int m_iRate;

		int m_iDungeonEndingSpeechTableIndex;	// clear type == CT_GAME인 경우에만 유효

		NextStageData()
		{
			ClearData();
		}

		void ClearData()
		{
			m_eClearType = CT_GAME;
			m_iStageIndex = -1;
			m_iSubStageIndex = -1;
			m_iRate = 100;
			m_iDungeonEndingSpeechTableIndex = 0;
		}
	};

	struct ClearCondtionData
	{
		std::vector<NextStageData>	m_vecNextStage;
	};

	struct NPCData
	{
		int								m_UID;
		CXSLUnitManager::NPC_UNIT_ID	m_UnitID;
		int								m_KeyCode;
		bool							m_bFocusCamera;
		std::vector<int>				m_vecStartPos;
		//int								m_StartPos;
		bool							m_bActive;
		bool							m_bShowGage;
		D3DXVECTOR3						m_AddPos;

		int								m_Rate;
		int								m_Level;

		bool							m_bHasBossGage;
		bool							m_bShowBossName;
		bool							m_bShowSubBossName;
		bool							m_bSiegeMode;
		//{{ 2007. 10. 26  최육사
		bool							m_bIsRight;
		//}}

		bool							m_bSubNPC;
		int								m_iSubNPCIndex;
		int								m_iSubNPCRate;

		bool							m_bNoDrop;
		float							m_fUnitScale;
		//{{ 2009. 12. 18  최육사	던전경험치개편
		CXSLUnitManager::MONSTER_GRADE	m_MonsterGrade;
		//}}
		//{{ 2011. 01. 26	최육사	퀘스트 클리어 조건 추가
		u_short							m_BossGaugeHPLines;
		//}}
		//{{ 2011. 03. 29	손영준	몬스터 팀 변수 추가
		bool							m_bAllyNpc;
		//}}
		float							m_fShowBossNameDelayTime;
		bool							m_bStopAtStartState;
	};

	struct NPCDropData
	{
		CXSLUnitManager::NPC_UNIT_ID	m_UnitID;
		int								m_iDropTimes;
	};

	struct NPCExpRateData
	{
		CXSLUnitManager::NPC_UNIT_ID	m_UnitID;
		float							m_fExpRate;
	};

	struct SubStageData
	{
		int						m_StartLineSet;
		int						m_EndLineSet;
		int						m_MainLineSet;

		//{{ 2012. 12. 21  던전 몬스터 그룹 랜던 배치 - 김민성
		KLottery								m_kNpcGroupRate;
		std::map< int, std::vector<NPCData*> >	m_mapNPCDataList;
			//}}

		std::vector<NPCDropData>	m_NPCDropDataList;

		std::vector<NPCExpRateData>	m_vecNPCExpRateDataList;

		GO_TYPE					m_GoType;
		//{{ 2010. 04. 26  최육사	비밀던전 헬모드
		int						m_ExtraNpcStartPos;
		bool					m_bExtraNpcIsLeft;
		//}}

		std::vector<ClearCondtionData>	m_vecClearCondData;

		SubStageData()
		{
			m_GoType = GT_NONE;
		}

		~SubStageData()
		{
			//{{ 2012. 12. 21  던전 몬스터 그룹 랜던 배치 - 김민성
			std::map< int, std::vector<NPCData*> >::iterator mit = m_mapNPCDataList.begin();
			for( ; mit != m_mapNPCDataList.end() ; ++mit )
			{
				for( int i = 0; i < (int)mit->second.size(); i++ )
				{
					NPCData* pNPCData = mit->second[i];
					SAFE_DELETE( pNPCData );
				}

				mit->second.clear();
			}
			m_mapNPCDataList.clear();
			//}}
		}

		bool LoadData( IN bool bScriptCheck
					 , IN OUT KLuaManager& luaManager
					 );

		int GetNpcDropTimes( int iUnitID );
		float GetNpcExpRate( int iUnitID );
		//void CreateNPC();
		//{{ 2012. 12. 21  던전 몬스터 그룹 랜던 배치 - 김민성
		int  GetRandomNpcGruopID();
		//}}

		int GetSecretStageEnteringEvent( IN int iClearConditionIndex );
		bool GetNextStage( OUT CXSLDungeonSubStage::NextStageData& kNextStageData, IN int iClearConditionIndex, IN int iSecretPadIndex );

	private:
		bool LoadCreatedNpcDropTimesData( KLuaManager& luaManager );
		bool LoadCreatedNpcExpRateData( KLuaManager& luaManager );
		bool LoadNPCData( IN bool bScriptCheck, KLuaManager& luaManager );
		bool FetchNPCData( IN bool bScriptCheck, KLuaManager& luaManager, CXSLDungeonSubStage::NPCData* pNPCData );

		//{{ 2012. 12. 21  던전 몬스터 그룹 랜던 배치 - 김민성
		bool New_LoadNPCData( IN bool bScriptCheck, KLuaManager& luaManager );
		//}}

		bool LoadClearCondition( KLuaManager& luaManager );
	};

public:
	CXSLDungeonSubStage( int subStageIndex, SubStageData* pSubStageData );
	~CXSLDungeonSubStage(void);

protected:
	SubStageData*		m_pSubStageData;
	int					m_SubStageIndex;

	bool				m_bStart;
	int					m_LoadNPCIndex;
};
