#pragma once

#include "RefreshSingleton.h"
#include "DBLayer.h"
#include "CommonPacket.h"
#include "X2Data/XSLDungeon.h"


//{{ 2010. 10. 05	최육사	센터서버 해킹체크


class KCnHackingCheckManager
{
	DeclareRefreshSingleton( KCnHackingCheckManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	struct KDungeonResultHackUserCheck
	{
		bool							m_bClear;
		int								m_iStartedMember;
		CXSLDungeon::RANK_TYPE			m_eComboRank;
		CXSLDungeon::RANK_TYPE			m_eTechnicalRank;
		CXSLDungeon::RANK_TYPE			m_eTimeRank;
		CXSLDungeon::RANK_TYPE			m_eDamagedRank;
		int								m_iTechnicalScore;
		int								m_iContinueCount;
		bool							m_bHavePet;

		std::set< u_char >				m_setCheckUnitClass;
		std::set< int >					m_setNoAdventDungeonID;
		int								m_iMinLevelCheck;
	};

	//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	struct KDungeonClearTimeHackUserCheck
	{
		bool							m_bClear;
		int								m_iStartedMember;
		std::set< u_char >				m_setCheckUnitClass;
		std::set< int >					m_setNoAdventDungeonID;
		int								m_iMinLevelCheck;
		int								m_iCheckPlayTime;
	};
	//}}
    typedef std::map<int, int> KMapStageClearTime;
    struct KHenirClearTimeHackUserCheck
    {
        //int                             m_iCheckPlayTime;
        KMapStageClearTime m_mapStageClearTime;
    };

public:
	KCnHackingCheckManager(void);
	~KCnHackingCheckManager(void);

	// for lua
	void SetDungeonResultCheckEnable_LUA( bool bVal );
	bool SetDungeonResultHackUserCheckData_LUA();
	//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	void SetDungeonClearTimeCheckEnable_LUA( bool bVal );
	bool SetDungeonClearTimeHackUserCheckData_LUA();
	void AddPhoneNum_LUA( const char* pszPhoneNum );
	//}}

	// function
	bool IsDungeonResultCheckEnable()	{ return m_bDungeonResultCheckEnable; }
	bool CheckDungeonResultAutoHackUser( IN const int iDungeonID,
								   		IN const char cDifficultyLevel,
								   		IN const u_char ucUnitClass,
								   		IN const int iUnitLevel,
								   		IN const bool bClear,
								   		IN const int iStartedMember,
								   		IN const CXSLDungeon::RANK_TYPE eComboRank,
								   		IN const CXSLDungeon::RANK_TYPE eTechnicalRank,
								   		IN const CXSLDungeon::RANK_TYPE eTimeRank,
								   		IN const CXSLDungeon::RANK_TYPE eDamagedRank,
								   		IN const int iTechnicalScore,
								   		IN const int iContinueCount,
								   		IN const bool bHavePet
								   		) const;

	//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	bool IsDungeonClearTimeCheckEnable()	{ return m_bDungeonClearTimeCheckEnable; }
	bool CheckDungeonClearTimeAutoHackUser( IN const char cAuthLevel,
											IN const UidType iUserUID,
											IN const int iDungeonID,
											IN const char cDifficultyLevel,
											IN const u_char ucUnitClass,
											IN const int iUnitLevel,
											IN const bool bClear,
											IN const int iStartedMember,
											IN const int iPlayTime
											);

	void Tick();

    bool CheckHenirClearTimeHackUser( IN const char cAuthLevel,
                                      IN const UidType iUserUID,
                                      IN const int iDungeonID,
                                      IN const int iStageID,
                                      IN const int iPlayTime );
    bool IsHenirClearTimeCheckEnable() { return m_bHenirClearTimeCheckEnable; }
    void SetHenirClearTimeCheckEnable_LUA( bool bVal );
    bool SetHenirClearTimeHackUserCheckData_LUA();
    bool CheckStageMinimumClearTime( IN const int iStageID_, IN const int iPlayTime_ );


private:
	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	template < class T > void SendToSMSDB( unsigned short usEventID, const T& data );

	//}}

private:
	bool							m_bDungeonResultCheckEnable;
	KDungeonResultHackUserCheck		m_kDungeonResultCheckData;
	//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	bool							m_bDungeonClearTimeCheckEnable;
	KDungeonClearTimeHackUserCheck	m_kDungeonClearTimeCheckData;
	std::set< UidType >				m_setDungeonClearTimeCheckUserList;
	int								m_iDungeonClearTimeCheckCount;
	boost::timer					m_tDungeonClearTimeCheckTimer;
	std::vector< std::wstring >		m_vecPhoneNumList;
	//}}
    KHenirClearTimeHackUserCheck    m_kHenirClearTimeCheckData;
    bool                            m_bHenirClearTimeCheckEnable;
};

template < class T >
void KCnHackingCheckManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), PC_LOG_DB, 0, NULL, usEventID, data );
}

template < class T >
void KCnHackingCheckManager::SendToSMSDB( unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { 0, -1 };

	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PC_SMS_DB, anTrace, usEventID, data );
	LIF( spEvent->m_kDestPerformer.AddUID( 0 ) );

	SiKDBLayer()->QueueingEvent( spEvent );
}

DefRefreshSingletonInline( KCnHackingCheckManager );


//}}
