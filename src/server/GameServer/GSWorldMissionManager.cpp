#include "GameServer.h"
#include "./GSWorldMissionManager.h"
#include "Enum/Enum.h"
#include "Socket/NetCommon.h"

//{{ 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2012. 04. 23	박세훈	어둠의 문 개편
	#include "../Common/X2Data/XSLBuffManager.h"
//}}

ImplementSingleton( KGSWorldMissionManager );

//{{ 2013. 04. 15	박세훈	어둠의 문 개편
KGSWorldMissionManager::KGSWorldMissionManager(void)
{
	m_bActive = false;

	m_iDungeonTargetCount = 0;
	m_iWorldCrystalCount = 0;
	m_iWorldMaxCrystalCount = 0;

	m_iMissionTime = 0;

	m_iDungeonPlayTime = 0;

	m_iTickCount = 0;
	m_usWorldBuff = CXSLBuffManager::BTI_NONE;
	m_usWorldBuffDurationTime = 0;

	m_iIndividualBuffID	= CXSLBuffManager::BTI_NONE;
	m_tWorldMissionEndTime = CTime( 0 );
}

KGSWorldMissionManager::~KGSWorldMissionManager(void)
{
}

void KGSWorldMissionManager::Init()
{
	// 로그인 서버로 부터 이벤트 던전 데이터 얻기
	KEGB_GET_MISSION_INFO_REQ kPacketReq;
	kPacketReq.m_iServerUID = KBaseServer::GetKObj()->GetUID();
	SendToGlobalServer( EGB_GET_MISSION_INFO_REQ, kPacketReq );
}

void KGSWorldMissionManager::UpdateMissionInfo( IN const KEGB_GET_MISSION_INFO_ACK& kPacketAck )
{
	switch( kPacketAck.m_iFlag )
	{
	case KEGB_GET_MISSION_INFO_ACK::FT_NONE:
		{
			START_LOG( cerr, L"받아서는 안되는 타입" )
				<< BUILD_LOG( kPacketAck.m_iFlag )
				<< END_LOG;

			return;
		} break;

	case KEGB_GET_MISSION_INFO_ACK::FT_INIT:
			m_mapDefenseOpenTime = kPacketAck.m_mapDefenseOpenTime;
	case KEGB_GET_MISSION_INFO_ACK::FT_UPDATE:
	case KEGB_GET_MISSION_INFO_ACK::FT_BEGIN:
		{
			m_bActive					= kPacketAck.m_bActive;
			m_iWorldCrystalCount		= kPacketAck.m_iWorldCrystalCount;
			m_iWorldMaxCrystalCount		= kPacketAck.m_iWorldMaxCrystalCount;
			m_iMissionTime				= kPacketAck.m_iMissionTime;
			m_iDungeonPlayTime			= kPacketAck.m_iDungeonPlayTime;
			m_usWorldBuffDurationTime	= kPacketAck.m_usWorldBuffDurationTime;

			if( kPacketAck.m_iFlag == KEGB_GET_MISSION_INFO_ACK::FT_BEGIN )
			{
				m_usWorldBuff				= CXSLBuffManager::BTI_NONE;
				m_usWorldBuffDurationTime	= 0;
				m_kTimer.restart();
			}
		}break;
	case KEGB_GET_MISSION_INFO_ACK::FT_SUCCESS:
	case KEGB_GET_MISSION_INFO_ACK::FT_FAIL:
		{
			m_bActive					= kPacketAck.m_bActive;
			m_iWorldCrystalCount		= kPacketAck.m_iWorldCrystalCount;
			m_iWorldMaxCrystalCount		= kPacketAck.m_iWorldMaxCrystalCount;
			m_iMissionTime				= kPacketAck.m_iMissionTime;
			m_iDungeonPlayTime			= kPacketAck.m_iDungeonPlayTime;
			m_usWorldBuffDurationTime	= kPacketAck.m_usWorldBuffDurationTime;
		} break;
	default:
		{
			START_LOG( cerr, L"받아서는 안되는 타입" )
				<< BUILD_LOG( kPacketAck.m_iFlag )
				<< END_LOG;

			return;
		}break;
	}

	m_iTickCount++;

	START_LOG( clog, L"월드 미션 정보 변경" )
		<< BUILD_LOG( kPacketAck.m_iFlag )
		<< BUILD_LOG( kPacketAck.m_bActive )
		<< BUILD_LOG( kPacketAck.m_iWorldCrystalCount )
		<< BUILD_LOG( kPacketAck.m_iWorldMaxCrystalCount )
		<< BUILD_LOG( kPacketAck.m_iMissionTime )
		<< BUILD_LOG( m_iTickCount )
		<< END_LOG;
}

void KGSWorldMissionManager::GetMissionInfo( OUT KEGS_WORLD_MISSION_UPDATE_NOT &kOut )
{
	kOut.m_bActive					= m_bActive;
	kOut.m_iRemainTime				= GetRemainTime();
	kOut.m_iWorldCrystalCount		= m_iWorldCrystalCount;
	kOut.m_iWorldMaxCrystalCount	= m_iWorldMaxCrystalCount;
	kOut.m_iMissionStartRemainTime	= GetMissionStartRemainTime();

	START_LOG( clog, L"월드 미션 정보" )
		<< BUILD_LOG( kOut.m_bActive )
		<< BUILD_LOG( kOut.m_iRemainTime )
		<< BUILD_LOG( kOut.m_iWorldCrystalCount )
		<< BUILD_LOG( kOut.m_iMissionStartRemainTime )
		<< END_LOG;
}

bool KGSWorldMissionManager::UpdateWorldBuffInfo( IN const KEGB_UPDATE_WORLD_BUFF_NOT& kPacket )
{
	m_bActive = kPacket.m_bActive;

	if( kPacket.m_usSuccessWorldBuff == m_usWorldBuff )
	{
		return false;
	}

	m_usWorldBuff				= kPacket.m_usSuccessWorldBuff;
	m_usWorldBuffDurationTime	= kPacket.m_usWorldBuffDurationTime;

	if( m_usWorldBuff == CXSLBuffManager::BTI_NONE )	// 되도록이면 범위 안에 들어가는지 체크하는 것이 좋다.
	{
		START_LOG( clog, L"월드 버프가 변경 되었습니다! 월드 버프 종료!!" )
			<< BUILD_LOG( m_usWorldBuff )
			<< BUILD_LOG( m_usWorldBuffDurationTime )
			<< END_LOG;
	}
	else
	{
		START_LOG( clog, L"월드 버프가 변경 되었습니다! 월드 버프 시전!!" )
			<< BUILD_LOG( m_usWorldBuff )
			<< BUILD_LOG( m_usWorldBuffDurationTime )
			<< END_LOG;
	}

	return true;
}

void KGSWorldMissionManager::GetWorldBuff( OUT KEGS_UPDATE_WORLD_BUFF_NOT& kPacket )
{
	kPacket.m_usSuccessWorldBuff		= m_usWorldBuff;
	kPacket.m_usWorldBuffDurationTime	= static_cast<u_short>(GetWorldBuffEndTime());
}

int KGSWorldMissionManager::GetWorldBuffState()
{
	int ret = CXSLBuffManager::BTI_NONE;

	if( m_usWorldBuffDurationTime > 0 && m_usWorldBuff > CXSLBuffManager::BTI_NONE )
	{
		ret = static_cast<int>(m_usWorldBuff);
	}

	return ret;
}

//{{ 버프 관리 시스템 - 김민성
int KGSWorldMissionManager::GetWorldBuffEndTime()
{
	CTime tCurrentTime = CTime::GetCurrentTime();

	// 요일별 세팅 시간 얻기
	std::map< int, std::vector< KDefenseOpenTime > >::const_iterator mit = m_mapDefenseOpenTime.find( tCurrentTime.GetDayOfWeek() );

	// 해당 요일에 예약된 오픈 시간 존재 여부 검사.
	if( mit == m_mapDefenseOpenTime.end() )
	{
		return CXSLBuffManager::BTI_NONE;
	}

	// 세팅 시간
	const std::vector< KDefenseOpenTime >& vecTime = mit->second;

	// 해당 요일에 예약된 오픈 시간들을 순회하면서, 현재 시간대에 맞는 정보가 있는지 검사한다.
	std::vector< KDefenseOpenTime >::const_iterator it = vecTime.begin();
	for( ; it != vecTime.end(); ++it )
	{
		const KDefenseOpenTime& kDefenseOpenTime = *it;
		CTime tStartTime	= CTime( tCurrentTime.GetYear(), tCurrentTime.GetMonth(), tCurrentTime.GetDay(), kDefenseOpenTime.m_iStartHour, kDefenseOpenTime.m_iStartMinute, 0 );
		CTime tBuffEndTime	= tStartTime + CTimeSpan( 0, 0, kDefenseOpenTime.m_iDurationTime + m_usWorldBuffDurationTime, 0 );

		if( ( tStartTime <= tCurrentTime ) &&
			( tCurrentTime < tBuffEndTime )
			)
		{
			break;
		}
	}

	if( it == vecTime.end() )
	{
		return 0;
	}
	else
	{
		m_tWorldMissionEndTime = CTime( tCurrentTime.GetYear(), tCurrentTime.GetMonth(), tCurrentTime.GetDay(), it->m_iStartHour, it->m_iStartMinute, 0 ) + CTimeSpan( 0, 0, it->m_iDurationTime, 0 );

		__int64 iCurrent = tCurrentTime.GetTime();
		int iBuffEndTime = static_cast<int>( ( m_tWorldMissionEndTime.GetTime() + m_usWorldBuffDurationTime ) - iCurrent );

		return iBuffEndTime;
	}
}
//}

//{{ 2013. 04. 15	박세훈	어둠의 문 개편
bool KGSWorldMissionManager::UpdateIndividualBuffInfo( IN const KEGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT& kPacket_ )
{
	if( m_iIndividualBuffID != kPacket_.m_iTheGateOfDarknessIndividualBuffLevel )
	{
		m_iIndividualBuffID = kPacket_.m_iTheGateOfDarknessIndividualBuffLevel;
		return true;
	}

	return false;
}
//}}

//{{ 2012. 03. 03	박세훈	어둠의 문 개편
int KGSWorldMissionManager::GetWorldCrystalCount( void )
{
	if( m_iWorldCrystalCount > WBSC_STEP_THREE )	// 수정의 최대 수를 넘을 경우 최대 수로 설정(client UI's crystal)
	{
		m_iWorldCrystalCount = WBSC_STEP_THREE;
	}

	START_LOG( clog, L"월드 크리스탈 갯수 확인" )
		<< BUILD_LOG( m_iWorldCrystalCount )
		<< END_LOG;

	return m_iWorldCrystalCount;
}

bool KGSWorldMissionManager::GetDefenseDungeonEndTime( OUT CTime& tStartTime, OUT CTime& tEndTime ) const
{
	CTime tCurrentTime = CTime::GetCurrentTime();

	// 요일별 세팅 시간 얻기
	std::map< int, std::vector< KDefenseOpenTime > >::const_iterator mit = m_mapDefenseOpenTime.find( tCurrentTime .GetDayOfWeek() );

	// 해당 요일에 예약된 오픈 시간 존재 여부 검사.
	if( mit == m_mapDefenseOpenTime.end() )
	{
		START_LOG( cerr, L"디펜스 던전 시간 셋팅 값이 존재 하지 않습니다." );
		return false;
	}

	// 세팅 시간
	const std::vector< KDefenseOpenTime >& vecTime = mit->second;

	// 해당 요일에 예약된 오픈 시간들을 순회하면서, 현재 시간대에 맞는 정보가 있는지 검사한다.
	for( std::vector< KDefenseOpenTime >::const_reverse_iterator rit = vecTime.rbegin(); rit != vecTime.rend(); ++rit )
	{
		const KDefenseOpenTime& kDefenseOpenTime = *rit;
		tStartTime = CTime( tCurrentTime .GetYear(), tCurrentTime .GetMonth(), tCurrentTime .GetDay(), kDefenseOpenTime.m_iStartHour, kDefenseOpenTime.m_iStartMinute, 0 );
		tEndTime = tStartTime + CTimeSpan( 0, 0, kDefenseOpenTime.m_iDurationTime, 0 );

		// 던전이 열려 있다면, 현재 시각을 포함하는 던전 오픈 설정 시각을 찾는다.
		if( m_bActive )
		{
			if( ( tStartTime <= tCurrentTime ) && ( tCurrentTime < tEndTime ) )
			{
				return true;
			}
		}
		else if( tEndTime <= tCurrentTime )
		{
			return true;
		}
	}

	CStringW startTime = tStartTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrStartTime = startTime.GetString();
	CStringW endTime = tEndTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrEndTime = endTime.GetString();
	CStringW currentTime = tCurrentTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrCurrentTime = currentTime.GetString();

	START_LOG( cerr, L"디펜스 던전 EndTime 검색 실패")
		<< BUILD_LOG( m_bActive )
		<< BUILD_LOG( wstrCurrentTime )
		<< BUILD_LOG( wstrStartTime )
		<< BUILD_LOG( wstrEndTime )
		<< END_LOG;

	return false;
}
//}}

int KGSWorldMissionManager::GetRemainTime()
{
	if( m_bActive ==true )
	{
		int iTime = static_cast<int>( m_iMissionTime - m_kTimer.elapsed() );
		return iTime > 0 ? iTime : 0;
	}
	else
	{
		return 0;
	}
}

void KGSWorldMissionManager::UpdateMissionStartTime( IN std::wstring& wstrDate )
{
	if( KncUtil::ConvertStringToCTime( wstrDate, m_tWorldMissionNextStartTime ) == false )
	{
		START_LOG( cerr, L"시간 변경 실패!" )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetYear() )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetMonth() )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetDay() )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetHour() )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetMinute() )
			<< BUILD_LOG( m_tWorldMissionNextStartTime.GetSecond() )
			<< BUILD_LOG( wstrDate )
			<< END_LOG;
	}
}

int KGSWorldMissionManager::GetMissionStartRemainTime()
{
	CTime tCurrent = CTime::GetCurrentTime();

	if( m_tWorldMissionNextStartTime > tCurrent )	// 월드 미션이 이후에 존재한다
	{
		CTimeSpan gap = m_tWorldMissionNextStartTime - tCurrent;

		return (int)gap.GetTotalMinutes();
	}
	else if( m_iMissionTime == 0 )					// 월드 미션 이후에 없다
	{
		return -1;
	}
	else											// 월드 미션이 진행 중이다
	{
		return 0;
	}
}
//}}
//}} 2011. 04. 13  김민성  글로벌 서버 추가
