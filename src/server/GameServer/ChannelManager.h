#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include "KncSend.h"
//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	#include "TickDumper.h"
//}}


class KChannelManager
{
	DeclareSingleton( KChannelManager );
	DeclDump;
	DeclToStringW;
	//{{ 2011. 12. 14	최육사	SimLayer Tick퍼포먼스 체크
	DeclareTickDumper;
	//}}
private:

	struct KChannelChangeUserInfo
	{
		KChannelChangeInfo		m_kChannelChangeInfo;	// 채널이동 하면서 가져가는 정보
		boost::timer			m_tWaitingTimer;		// 채널이동 예약 타이머

		KChannelChangeUserInfo( const KChannelChangeInfo& kInfo )
		{
			m_kChannelChangeInfo = kInfo;
		}
	};

public:
	KChannelManager();
	~KChannelManager();

	void Tick();

	//////////////////////////////////////////////////////////////////////////
	// 채널 정보
	void	Init( int iServerGroupID, int iChannelID, const KChannelBonusInfo& kInfo );

	int		GetChannelID()															{ return m_iGameServChannelID; }
	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}
	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int		GetMaxUser() const														{ return m_iMaxUser; }
	void	SetMaxUser( IN const int iMaxUser )										{ m_iMaxUser = iMaxUser; }
	//}}

	void	UpdateChannelList( const std::map< int, KChannelInfo >& mapChannelList );

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	void	UpdateChannelBonusList( const std::map< int, KChannelBonusInfo >& mapChannelBonusList );

	const std::vector< KChannelInfo >& GetChannelInfo()								{ return m_vecChannelInfo; }
	const std::map< int, KChannelBonusInfo >& GetChannelBonusInfo()					{ return m_mapChannelBonusList; }

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	void	GetServerUIDByChannelName( OUT std::wstring& wstrName );
	//}


	const std::vector< KChannelSimpleInfo >& GetChannelList()						{ return m_vecChannelList; }
    const std::vector< KChannelSimpleInfo >& GetChannelList2nd()	{ return m_vecChannelList2nd; }
    void UpdateChannelList2nd( const std::map< int, KChannelInfo >& mapChannelList_ );
    const std::map<int, KChannelInfo >& GetChannelMap()  { return m_mapChannelList; }
    const std::map<int, KChannelInfo >& GetChannelMap2nd()  { return m_mapChannelList2nd; }
    const std::map< int, KChannelBonusInfo >& GetChannelBonusList() { return m_mapChannelBonusList; }
    const std::map< int, KChannelBonusInfo >& GetChannelBonusList2nd() { return m_mapChannelBonusList2nd; }
    void UpdateChannelBonusList2nd( const std::map< int, KChannelBonusInfo >& mapChannelBonusList_ );
	bool	GetServerUIDByChannelID( int iChannelID, UidType& iServerUID );
	void	GetTotalPartyInfo( int& iCurPartyCount, int& iPlayGamePartyCount )		{ iCurPartyCount = m_iCurPartyCount; iPlayGamePartyCount = m_iPlayGamePartyCount; }
	void	GetChannelBonus( u_char ucLevel, float& fAddExpRate, float& fAddEDRate ); // 채널 보너스

	//////////////////////////////////////////////////////////////////////////
	// 채널 이동 예약
	int		GetChannelChangeWaitingUserCount()										{ return m_mapChannelChangeUserList.size(); }
	bool	IsExistChannelChangeUser( UidType iUnitUID )							{ return ( m_mapChannelChangeUserList.find( iUnitUID ) != m_mapChannelChangeUserList.end() ); }
	bool	NewChannelChangeUser( UidType iUnitUID, const KChannelChangeInfo& kInfo );
	bool	DelChannelChangeUser( UidType iUnitUID, KChannelChangeInfo& kInfo );
	bool	DelChannelChangeUser( UidType iUnitUID );
	double	GetMaxWaitingTime()														{ return m_fMaxWaitingTime; }

	// send event
	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );
	void SendToLogDB( unsigned short usEventID );

    template < class T > void SendToLogDB2nd( unsigned short usEventID, const T& data );



private:
	// 채널 정보
	int											m_iServerGroupID;
	int											m_iGameServChannelID;
	std::map< int, KChannelInfo >				m_mapChannelList;

    // 진입구조 개편
    std::map< int, KChannelInfo >				m_mapAnotherServerGroupChannelList;

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	std::map< int, KChannelBonusInfo >			m_mapChannelBonusList;
	std::vector< KChannelInfo >					m_vecChannelInfo;
	std::vector< KChannelSimpleInfo >			m_vecChannelList;
	int											m_iCurPartyCount;
	int											m_iPlayGamePartyCount;
	boost::timer								m_kChannelListRefreshTimer;
	KChannelBonusInfo							m_kBonusInfo;				// 채널 보너스

	// 채널 이동 예약
	std::map< UidType, KChannelChangeUserInfo >	m_mapChannelChangeUserList;
	double										m_fMaxWaitingTime;

	//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
	int                                         m_iMaxUser;
	//}}
    std::map< int, KChannelInfo >				m_mapChannelList2nd; // 다른 서버군의 채널 정보
    std::vector< KChannelSimpleInfo >			m_vecChannelList2nd; // 다른 서버군의 심플 채널 정보
    std::map< int, KChannelBonusInfo >          m_mapChannelBonusList2nd; // 다른 서버군의 채널 보너스 정보
};

DefSingletonInline( KChannelManager );

template < class T >
void KChannelManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_LOG_DB, 0, NULL, usEventID, data );
}

template < class T >
void KChannelManager::SendToLogDB2nd( unsigned short usEventID, const T& data )
{
    KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_LOG_DB_2ND, 0, NULL, usEventID, data );

}
