#pragma once
#include "SimLayer.h"
#include "X2Data/XSLMain.h"

class KGSSimLayer : public KSimLayer
{
    NiDeclareRTTI;
    DeclDump;
    DeclToStringW;

	//{{ 2011. 04. 04  김민성   LFH
	//}}

public:

    enum SERVER_GROUP_NUM
    {
        SINGLE_SERVER_GROUP = 1,
        DOUBLE_SERVER_GROUP = 2,
    };

    KGSSimLayer(void);
    virtual ~KGSSimLayer(void);

    // derived from KSimLayer
    virtual void Init();
    virtual void Tick();
    virtual void ShutDown();
    virtual void RegToLua();

	//{{ 임홍락 [2012.05.16] 서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
	//}}
			void SendNotMSG( IN char cNotifyType, IN const std::wstring& wstrNotMSG, IN int iCount );
			void SendNotMSG_GRP( const char* szNotMSG, int iCount = 1 );
			//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
			void SendNotMSG_ALL( const char* szNotMSG, int iCount = 1  );
			//}}
			void AddIP( const char* szAddIP );
			bool CheckIP( const char* szIP );

			void AddRepeatEvent( u_short usEventID, int iCnt );

			//{{ 2009. 9. 1  최육사		IOCP종료사유
			void AddAbnormalDisconnectReason( DWORD dwReason );
			void SendToDBAbnormalReason( bool bFinal );
			//}}
			//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
			//}}
			//{{ 2009. 12. 15  최육사	서버관리
	virtual void DumpToLogFile();
			//}}
	//{{ 2011. 04. 04  김민성   LFH
	//}}
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	void		 TickDump_LUA();
	//}}

	void ReloadXTRAP_MAP();
	int GetMapQuantity() { return m_setLoadedMaps.size(); }
	char* GetAllMapPointer() { return &m_vecUsingMap[0]; }

	void DeleteCommonFlag_AllGS_LUA( DWORD dwFlag );
	void AddCommonFlag_AllGS_LUA( DWORD dwFlag );

    void SetServerGroupNum( int iGroupNum );
    int GetServerGroupNum();



protected:
    std::vector< std::string >		m_vecIP;

	//클라이언트에서 보내는 방관련 패킷중 중복으로 날아오는 패킷체크를 위해
	//1 arg : event id
	//2 arg : repeat count
	std::map< u_short, int >		m_mapRepeatEvent;

	//{{ 2009. 9. 1  최육사		IOCP종료사유
	std::map< DWORD, int >			m_mapAbnormalDisconnectReason;
	boost::timer					m_tAbnormalReasonTimer;
	//}}
	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}

	mutable KncCriticalSection      m_csXTrapReload;
	boost::timer					m_tXTrapReloadTimer;
	std::set<std::vector<char> >	m_setLoadedMaps;	// 현재 올라가 있는 맵인지 확인을 위한 정보
	std::vector<char>				m_vecUsingMap;		// 실제 메모리에 올라가는 정보
    int m_iServerGroupNum;


};

DefKObjectInline( KGSSimLayer, KSimLayer );
