#pragma once


//{{ 2010. 02. 16  최육사	해킹툴 리스트


class KUserHackingManager
{
	enum HACKING_ENUM
	{
		HE_MIN_TIME = 10,
		HE_MAX_TIME = 20,
	};

public:
	KUserHackingManager(void);
	~KUserHackingManager(void);

	void SetRandomCheckTime();
	double GetHackingToolCheckTime() { return m_fHackingToolCheckTime; }

	void GetHackingToolList( OUT int& iRandomKey, OUT std::vector< KHackingToolInfo >& vecHackingToolList );

	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	void ClearLetterMonitoringList()										{ m_setRecvLetterList.clear(); }
	void ClearTradeMonitoringList()											{ m_setRecvTradeList.clear(); }
	bool AddLetterMonitoringList( IN const UidType& iForomUnit );
	bool AddTradeMonitoringList( IN const UidType& iForomUnit );
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	bool HackingCheckDeserializeFailCount();
	void IncreaseDeserializeFailCount( IN const u_short usEventID );
	//}}

	//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
	void SetEDMonitoringMode( bool bEDMonitoringMode )			{ m_bEDMonitoringMode = bEDMonitoringMode; }
	bool GetEDMonitoringMode()									{ return m_bEDMonitoringMode; }
	//}}

	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	bool CheckHackingUserRecivedPacket( OUT char& cLastSendType );
	void SetHackingUserSendType( char cType );
	bool SetHackingUserReciveType( IN char cType, OUT char& cLastSendType );
	//}}

private:
	double				m_fHackingToolCheckTime;
	//{{ 2011. 04. 05  김민성	우편 및 거래 감시
	std::set< UidType >				m_setRecvLetterList;
	std::set< UidType >				m_setRecvTradeList;
	//}}
	//{{ 2011. 04. 26	최육사	deserialize 실패 종료
	bool							m_bCheckedDeserializeFail;
	int								m_iTotalDeserializeFailCount;
	std::map< u_short, int >		m_mapDeserializeFailList;
	//}}

	//{{ 2011. 09. 20  김민성	게임내 ED 감시 모니터링
	bool							m_bEDMonitoringMode;
	//}}
	//{{ 2012. 06. 04	김민성       서버 해킹 유저 체크 요청 응답으로 변경
	char							m_cLastSendType;
	x2::Timer					m_tHackingUserTimer;
	//}}
};


//}}
