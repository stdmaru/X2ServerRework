#include "./UserSecurityManager.h"
//{{ 2011. 09. 06	최육사	머신 ID 기능 업데이트
	#include "Base64/base64.h"
//}}
//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	#include "Statistics.h"
//}}


//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단


KUserSecurityManager::KUserSecurityManager(void)
{
	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{  2012. 1. 16	김민성		게임서버에서 Server SN 검사
	//}}
	//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
	//}}

	//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
	m_iKickRandomTime = 0;
	m_cKickState = KT_NONE;
	m_tKickTimer.restart();
	m_iKickReason = KStatistics::eSIColDR_No_Reason;
	//}}
}

KUserSecurityManager::~KUserSecurityManager(void)
{
}

const std::string KUserSecurityManager::GetStringMachineID() const
{
	std::string strResult = KncUtil::toNarrowString( m_wstrMachineID );
	return strResult;
}

void KUserSecurityManager::SetMachineID( IN const std::string& strMachineID )
{
    m_wstrMachineID = KncUtil::toWideString( strMachineID );
}

WORD KUserSecurityManager::GetHashFromMachineID( IN const std::string& strMachineID )
{
	if( strMachineID.size() < 16 )
	{
		return 0;
	}

	WORD wHash = *(WORD*)&strMachineID[14];
	return wHash;
}

WORD KUserSecurityManager::MakeHashFromMachineID( IN const std::string& strMachineID )
{
	if( strMachineID.size() < 16 )
	{
		return 0;
	}

	// 해쉬값 체크!
	BYTE MachineId[16] = {0, };

	for( int i = 0; i < 16; ++i )
	{
		MachineId[i] = strMachineID[i];
	}

	// fills Mac Address, Serial Number.
	WORD wHash = 0;
	for (int i=0; i<6; ++i)
	{
		wHash += ((WORD*)MachineId)[i] * (i+3);
	}

	return wHash;
}

bool KUserSecurityManager::CheckValidMachineID( IN const std::string& strMachineID ) const
{
	// 머신ID가 비어있는지 검사!
	if( strMachineID.empty() == true )
		return false;

	// 머신ID 길이 검사
	if( strMachineID.size() < 16 )
		return false;

	//{{ 2011. 09. 06	최육사	머신 ID 기능 업데이트
	//machine id 유효성 검사 체크 필요
	//위코드 처럼 구현하면 됨.. 단 base64 decode후 검사해야 함
	std::string strDecodingMachineID = base64_decode( strMachineID );

	if( GetHashFromMachineID( strDecodingMachineID ) != MakeHashFromMachineID( strDecodingMachineID ) )
		return false;
	//}}

    return true;
}

//}}

//{{ 2012. 04. 02	김민성		sn 모니터링 기능 분리
//}}

//{{ 2012. 05. 23	김민성       판매 불가 아이템 판매 시도 해킹 유저로 판단
void KUserSecurityManager::SetKickRandomTime( int iMinTime, int iRange )
{
	if( iRange < 1 )
	{
		iRange = 1;
	}

	m_iKickRandomTime = rand() % iRange + iMinTime;
	m_tKickTimer.restart();

	START_LOG( clog2, L"Setting a random delay before termination." )
		<< BUILD_LOG( m_iKickRandomTime )
		<< END_LOG;
}

void KUserSecurityManager::SetKickState( IN char cState )
{
	m_cKickState = cState;

	START_LOG( clog2, L"Setting forced-termination type" )
		<< BUILD_LOGc( m_cKickState )
		<< END_LOG;
}

bool KUserSecurityManager::CheckIsKickUser()
{
	if( m_cKickState == KT_AFTER_RANDOM_TIME )
	{
		if( m_tKickTimer.elapsed() < m_iKickRandomTime )
		{
			return false;
		}

		m_tKickTimer.restart();
		m_cKickState = KT_NONE;

		return true;
	}

	return false;
}
//}}
