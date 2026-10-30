#pragma once
#include "Event.h"
#include "CommonPacket.h"

//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
	#include <boost/timer.hpp>
//}}

//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단


class KUserSecurityManager
{
public:
	KUserSecurityManager(void);
	~KUserSecurityManager(void);

	// 정보
	const std::wstring GetWStringMachineID() const { return m_wstrMachineID; }
	const std::string GetStringMachineID() const;
	void SetMachineID( IN const std::string& strMachineID );

	// 검사
	bool CheckValidMachineID( IN const std::string& strMachineID ) const;

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}

	//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	enum KICK_TYPE
	{
		KT_NONE					= 0,
		KT_AFTER_RANDOM_TIME	= 1,
	};

	void SetKickRandomTime( int iMinTime, int iRange );
	void SetKickState( IN char cState );
	bool CheckIsKickUser();
	int	 GetKickReason()					{ return m_iKickReason; }
	void SetKickReason( int iReason )		{ m_iKickReason = iReason; }
	//}}


private:
	static WORD GetHashFromMachineID( IN const std::string& strMachineID );
	static WORD MakeHashFromMachineID( IN const std::string& strMachineID );

private:
	std::wstring					m_wstrMachineID;

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}
	//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
	//}}
	//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	int								m_iKickRandomTime;
	char							m_cKickState;
	boost::timer					m_tKickTimer;
	int								m_iKickReason;
	//}}
};


//}}
