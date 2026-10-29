#pragma once

#include "Statistics.h"
#include "Enum/Enum.h"

class KStatisticsLocalLog
{
public:
	enum ELocalLogSpirit
	{
		Max_level = 80,
	};

	enum KLocalLog
	{
		LOG_PVP = 0,
		LOG_PVP_ROOM,
		LOG_DUNGEON_ROOM,
		LOG_TITLE,
		LOG_DUNGEON,
		//{{ 2010. 9. 14	최육사	펫 시스템
		LOG_PET_SUMMON,
		//}}
		//{{ 2010. 10. 11	최육사	서버간 접속 끊김 로그
		LOG_SERV_DISCONNECT,
		//}}
		//{{ 2010. 10. 11	최육사	넥슨 빌링 패킷 로그
		LOG_BILLING_PACKET,
		//}}
		//{{ 2010. 11. 17	최육사	오토핵 검증 기능
		LOG_ABUSER_MORNITORING,
		//}}
#if X2_DEV_TOOLS
		LOG_UNDEFINED_QUEST_TEMPLET,
#endif // SERV_LOG_UNDEFINED_QUEST_TEMPLET

        LOG_MAX,
	};

	struct KLocalLogInfo
	{
		int				m_iStatisticsID;
		std::string		m_strLogFileName;
		std::wfstream	m_wfsLocalLog;

		void Init( int iID, const char* pFileName )
		{
			m_iStatisticsID	 = iID;
			m_strLogFileName = pFileName;
		}
	};

public:
	KStatisticsLocalLog();
	virtual ~KStatisticsLocalLog();

	//{{ 2008. 10. 27  최육사	코드 통합
	void InitLocalLogInfo();
	void InitLocalLog( KLocalLog eEnum );
	std::wfstream& LocalLogStm( KLocalLog eEnum );
	//}}

	void FlushLocalLogStm();
	void ClearLocalLogData();
	void CloseLocalLogData();

	void WriteLocalLogData( std::wfstream& fout );
	void WriteLocalLogHead( std::wfstream& fout, int iStatisticsID );

	void ClearLocalLog();

	void LoadLocalLogFromFile();

	//////////////////////////////////////////////////////////////////////////
	// 헤니르 랭킹 - 로그인 서버 전용이라서 따로 되어있음
	void InitLocalLog_Henir_Ranking( int iRankingType );
	std::wfstream& LocalLogStm_Henir_Ranking( int iRankingType );
	void CloseHenirRankingLogData( int iRankingType );
	void DeleteHenirRankingLog();
	//////////////////////////////////////////////////////////////////////////

private:
	// 일일 통계
	void LoadLocalLogFromFile( int iStatisticsID, const char* strFileName, unsigned int uiLastIndex );
	void FlushLocalLogStm( int iStatisticsID, const char* strFileName );

protected:
	//{{ 2008. 10. 27  최육사	코드 통합
	KLocalLogInfo					m_kLocalLog[LOG_MAX];
	//}}

	//{{ 2009. 7. 7  최육사		랭킹개편
	std::wfstream					m_wfsLocalLog_Henir_Ranking[SEnum::RT_MAX_NUM];
	//}}
};
