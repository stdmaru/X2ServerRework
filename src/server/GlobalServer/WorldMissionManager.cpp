#include "SimLayer.h"
#include "GlobalSimLayer.h"
#include "BaseServer.h"
#include "WorldMissionManager.h"
#include "Socket/NetCommon.h"
#include "Enum/Enum.h"
#include "GlobalServerCCUManager.h"
//{{ 버프 관리 시스템 - 김민성
	#include "X2Data//XSLBuffManager.h"
//}

//////////////////////////////////////////////////////////////////////////
//{{ 2011. 04. 13  김민성  글로벌 서버 추가
//////////////////////////////////////////////////////////////////////////

ImplementRefreshSingleton( KWorldMissionManager );

//{{ 2013. 04. 13	박세훈	어둠의 문 개편
KWorldMissionManager::KWorldMissionManager(void)
{
	Init();
}

KWorldMissionManager::~KWorldMissionManager(void)
{
}

void KWorldMissionManager::Init()
{
	m_bWorldMissionOn = true;
	m_bIsFirstTick = true;

	m_bActive = false;
	m_bWorldBuffTime = false;

	m_iStartedCrystalCount		= 0;
	m_iProtectedCrystalCount	= 0;

	m_iTotalCCU = 0;

	SetMissionTime( 0 );

	m_kTimerDOT.restart();
	m_kTimerEnableCheck.restart();
	m_kTimerUpdate.restart();
	m_kTimerWorldBuff.restart();

	m_mapDefenseOpenTime.clear();
	m_tToDayMissionTime = CTime::GetCurrentTime();

	m_iWorldBuffID		= CXSLBuffManager::BTI_NONE;
	m_iIndividualBuffID	= CXSLBuffManager::BTI_NONE;

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}
}

ImplToStringW( KWorldMissionManager )
{
	stm_	<< L"----------[ World Mission Manager ]----------" << std::endl
		<< TOSTRINGW( m_bActive )
		<< BUILD_LOG( m_iTotalCCU )
		<< BUILD_LOG( m_iMissionTime )
		<< BUILD_LOG( m_iNextDOT )
		<< BUILD_LOG( m_usWorldBuffDurationTime )
		<< BUILD_LOG( m_iStartedCrystalCount )
		<< BUILD_LOG( m_iProtectedCrystalCount )
		<< TOSTRINGW( m_kTimerDOT.elapsed() )
		;

	KEGB_GET_MISSION_INFO_ACK kPacket;
	SiKWorldMissionManager()->GetWorldMissionInfo( KEGB_GET_MISSION_INFO_ACK::FT_INIT, kPacket );
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, EGB_UPDATE_MISSION_INFO_NOT, kPacket );

	return stm_;
}

ImplementLuaScriptParser( KWorldMissionManager )
{

	lua_tinker::class_add<KWorldMissionManager>( GetLuaState(), "KWorldMissionManager" );
	lua_tinker::class_def<KWorldMissionManager>( GetLuaState(), "dump",								&KWorldMissionManager::Dump );

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	//#ifdef SERV_INSERT_GLOBAL_SERVER
	lua_tinker::class_def<KWorldMissionManager>( GetLuaState(), "SetDefenseOpenTime",				&KWorldMissionManager::SetDefenseOpenTime_LUA );
	lua_tinker::class_def<KWorldMissionManager>( GetLuaState(), "SetWorldBuffDurationTime",			&KWorldMissionManager::SetWorldBuffDurationTime_LUA );

	lua_tinker::class_def<KWorldMissionManager>( GetLuaState(), "WorldMissionDump",					&KWorldMissionManager::WorldMissionDump_LUA );
	//#endif SERV_INSERT_GLOBAL_SERVER
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	//{{ 2013. 04. 24	박세훈	어둠의 문 개편
	lua_tinker::class_def<KWorldMissionManager>( GetLuaState(), "SetReduceTerm",					&KWorldMissionManager::SetReduceTerm_LUA );
	//}}

	lua_tinker::decl( GetLuaState(), "WorldMissionManager", this );
}

void KWorldMissionManager::Tick()
{
	//////////////////////////////////////////////////////////////////////////
	// 10초 마다 Enable체크
	if( m_kTimerEnableCheck.elapsed() < 1.0f )
	{
		return;
	}
	m_kTimerEnableCheck.restart();

	//	처음 실행
	if( m_bIsFirstTick == true && m_kTimerUpdate.elapsed() >= 10.0f)
	{
		//	초기 대기시간은 기본 시간으로 설정한다
		if( SetTodayMissionTime() == true )
		{
			BroadCastMissionStartTime();
		}

		if( m_iNextDOT != 0 )
		{
			BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_INIT );
		}
		m_bIsFirstTick = false;
		return;
	}

	if( IsActive() == true )
	{
		// 크리스탈이 감소되는 카운트 적용
		if( GetReduceTerm() < m_kTimerForReduceTheProtectedCrystalCount.elapsed() )
		{
			IncreaseProtectedCrystalCount( m_iReduceCount );
			m_kTimerForReduceTheProtectedCrystalCount.restart();
		}
	}

	//	게임서버로 정보 업데이트
	if( m_kTimerUpdate.elapsed() >= 10.0f )
	{
		// 정보를 업데이트 하기 위해서는 정보를 새것으로 만들어야 한다.
		BroadCastMissionStartTime();
		BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_UPDATE );

		BuffProcess();
		m_kTimerUpdate.restart();
	}

	if( m_bWorldMissionOn == true )		// EndMission() 이 불리고 당일 이벤트가 없다
	{
		CTime tCurrentTiem = CTime::GetCurrentTime();

		if( tCurrentTiem.GetHour() == 0 && tCurrentTiem.GetMinute() == 0 &&  tCurrentTiem.GetMinute() <= 1 )	// 00:00 가 지났다 -> 다음날이다
		{
			if( SetTodayMissionTime() == true )
			{
				BroadCastMissionStartTime();

			}
			else
			{
				START_LOG( clog, L"오늘 중 디펜스 던전이 없다?!" )
					<< END_LOG;
			}
			return;
		}
	}

	if( m_bIsFirstTick == false )
	{
		CheckWorldMissionProcess();
	}
}

void KWorldMissionManager::InitMissionData( void )
{
	// 깔끔하게 초기화 하기 위한 함수
	m_iTotalCCU = 0;
	m_iTotalCCU = SiKGlobalServerCCUManager()->GetNowTotalCCU(); // 동접

	m_iStartedCrystalCount		= lua_tinker::call<int>( GetLuaState(), "COMPUTE_MIN_MAX_CRYSTAL_COUNT", m_iTotalCCU );
	m_iProtectedCrystalCount	= 0;
	m_iReduceCount				= lua_tinker::call<int>( GetLuaState(), "COMPUTE_REDUCE_CRYSTAL_COUNT", m_iStartedCrystalCount, m_tToDayMissionTime.GetHour() );

	m_kTimerForReduceTheProtectedCrystalCount.restart();

	BuffProcess();

	START_LOG( cout, L"월드미션 초기화" )
		<< BUILD_LOG( m_iStartedCrystalCount )
		<< BUILD_LOG( m_iProtectedCrystalCount )
		<< BUILD_LOG( m_iMissionTime )
		<< BUILD_LOG( m_iTotalCCU )
		<< BUILD_LOG( m_usWorldBuffDurationTime )
		<< BUILD_LOG( m_fReduceTerm )
		<< BUILD_LOG( m_iReduceCount )
		<< END_LOG;
}

void KWorldMissionManager::IncreaseProtectedCrystalCount( IN const int iProtectedCrystalCount )
{
	if( IsActive() == false )
	{
		return;
	}

	int iTemp = m_iProtectedCrystalCount + iProtectedCrystalCount;

	// over/under flow를 검사한다.
	if( abs( m_iStartedCrystalCount ) < abs( iTemp ) )
	{
		iTemp = m_iStartedCrystalCount;

		if( iProtectedCrystalCount < 0 )
		{
			iTemp = ~iTemp + 1;
		}
	}

	m_iProtectedCrystalCount = iTemp;

	START_LOG( clog, L"보호된 수정 갯수 증감 요청" )
		<< BUILD_LOG( iProtectedCrystalCount )
		<< BUILD_LOG( m_iProtectedCrystalCount )
		<<END_LOG;
}

void KWorldMissionManager::ResetProtectedCrystalCount( void )
{
	if( IsActive() == false )
	{
		return;
	}

	m_iProtectedCrystalCount = 0;
}

void KWorldMissionManager::GetWorldMissionInfo( IN const int& iType, OUT KEGB_GET_MISSION_INFO_ACK &kPacketAck ) const
{
	kPacketAck.m_bActive				= IsActive();
	kPacketAck.m_iWorldCrystalCount		= m_iProtectedCrystalCount;
	kPacketAck.m_iWorldMaxCrystalCount	= m_iStartedCrystalCount;
	kPacketAck.m_iMissionTime			= m_iMissionTime;

	if( iType == KEGB_UPDATE_MISSION_INFO_NOT::FT_INIT )
	{
		kPacketAck.m_mapDefenseOpenTime = m_mapDefenseOpenTime;
	}
	kPacketAck.m_iFlag = iType;
	kPacketAck.m_usWorldBuffDurationTime= m_usWorldBuffDurationTime;
}

void KWorldMissionManager::BroadCastMission( IN const KEGB_UPDATE_MISSION_INFO_NOT::FLAG_TYPE eType )
{
	KEGB_UPDATE_MISSION_INFO_NOT	kPacket;

	GetWorldMissionInfo( eType, kPacket );

	// 모든 게임서버에 브로드 캐스팅
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, EGB_UPDATE_MISSION_INFO_NOT, kPacket );
}

void KWorldMissionManager::BroadCastWorldBuff( IN const int _skill )
{
	KEGB_UPDATE_WORLD_BUFF_NOT	kPacket;

	kPacket.m_bActive					= IsActive();
	kPacket.m_usSuccessWorldBuff		= static_cast<u_short>( _skill );
	kPacket.m_usWorldBuffDurationTime	= m_usWorldBuffDurationTime;

	// 모든 게임서버에 브로드 캐스팅
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, EGB_UPDATE_WORLD_BUFF_NOT, kPacket );
}

void KWorldMissionManager::BroadCastMissionStartTime( void )
{
	KEGB_UPDATE_MISSION_START_TIME_NOT kPacket;
	kPacket.m_wsrtStartTime = m_tToDayMissionTime.Format(_T("%Y-%m-%d %H:%M:%S"));

	// 모든 게임서버에 브로드 캐스팅
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, EGB_UPDATE_MISSION_START_TIME_NOT, kPacket );
}

void KWorldMissionManager::StartMission( IN const bool IsReStart /*= false */)
{
	//	이벤트 시작
	m_bActive = true;
	m_tToDayStartTime = CTime::GetCurrentTime();

	if( IsReStart == false )
	{
		InitMissionData();
	}
	else
	{
		SetMissionData();
	}

	BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_BEGIN );

	START_LOG( cout2, L"월드미션 시작!" )
		<< END_LOG;
}

void KWorldMissionManager::EndMission( void )
{
	m_bActive = false;
	m_bWorldBuffTime = ( GetProtectedCrystalCount() < 0 );
	m_kTimerWorldBuff.restart();
	m_bIsFirstTick = true;
	SetMissionTime( 0 );

	bool bSuccess = ( 0 <= GetProtectedCrystalCount() );

	//	이벤트 종료
	if( bSuccess )	//	성공
	{
		START_LOG( cout2, L"월드미션 성공!" )
			<< BUILD_LOG( m_iProtectedCrystalCount )
			<< END_LOG;

		BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_SUCCESS );
		WriteWorldMissionCrystalCount( true );
	}
	else
	{
		START_LOG( cout2, L"월드미션 실패!" )
			<< BUILD_LOG( m_iProtectedCrystalCount )
			<< END_LOG;

		BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_FAIL );
		WriteWorldMissionCrystalCount( true );
	}
}

// 치트용
void KWorldMissionManager::SetMissionSuccess_LUA(bool bSuccess)
{
	if( bSuccess )	//	성공
	{
		START_LOG( cout2, L"월드미션 성공 상태로 강제 셋팅!" )
			<< END_LOG;

		BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_SUCCESS );
	}
	else
	{
		START_LOG( cout2, L"월드미션 실패 상태로 강제 셋팅!" )
			<< END_LOG;

		BroadCastMission( KEGB_UPDATE_MISSION_INFO_NOT::FT_FAIL );
	}
}

void KWorldMissionManager::WorldMissionDump_LUA()
{
	m_iTotalCCU = SiKGlobalServerCCUManager()->GetNowTotalCCU();

	START_LOG( cout, L"WorldMission 상테 체크" )
		<< BUILD_LOG( m_iTotalCCU )
		<< BUILD_LOG( m_iMissionTime )
		<< BUILD_LOG( m_iNextDOT )
		<< BUILD_LOG( m_usWorldBuffDurationTime )
		<< BUILD_LOG( m_iStartedCrystalCount )
		<< BUILD_LOG( m_iProtectedCrystalCount )
		<< END_LOG;
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
//#ifdef SERV_INSERT_GLOBAL_SERVER
void KWorldMissionManager::SetDefenseOpenTime_LUA( int iDayOfWeek, int iStartHour, int iStartMin, int iDurationTime )
{
	if( (iDayOfWeek < 1  || iDayOfWeek > 7  ) ||		// 월~일 사이가 아닐때 오류체크
		(iStartHour < 0  || iStartHour > 23 ) ||		// 시간 0~23 사이가 아닐때
		(iStartMin  < 0  || iStartMin  > 59 ) ||		// 분 0~59 사이가 아닐때
		(iDurationTime < 1) )							// 유지시간이 1 보다 작을때
	{
		START_LOG( cerr, L"디펜스 던전 요일 셋팅 값이 이상합니다." )
			<< BUILD_LOG( iDayOfWeek )
			<< BUILD_LOG( iStartHour )
			<< BUILD_LOG( iStartMin )
			<< BUILD_LOG( iDurationTime )
			<< END_LOG;
		return;
	}

	KDefenseOpenTime _info;
	_info.m_iStartHour = iStartHour;
	_info.m_iStartMinute = iStartMin;
	_info.m_iDurationTime = iDurationTime;

	std::map< int, std::vector< KDefenseOpenTime > >::iterator mit = m_mapDefenseOpenTime.find( iDayOfWeek );
	if( mit != m_mapDefenseOpenTime.end() )
	{
		std::vector< KDefenseOpenTime >::iterator vit = mit->second.begin();
		for( ; vit != mit->second.end() ; ++vit )
		{
			if( vit->m_iStartHour == iStartHour && vit->m_iStartMinute == iStartMin )	// 중복 되는 정보는 무시
			{
				START_LOG( cerr, L"디펜스 던전 오픈 시간이 중복 되었습니다." )
					<< BUILD_LOG( iDayOfWeek )
					<< BUILD_LOG( iStartHour )
					<< BUILD_LOG( iStartMin )
					<< BUILD_LOG( iDurationTime )
					<< END_LOG;
				return;
			}
		}

		mit->second.push_back( _info );
	}
	else
	{
		std::vector< KDefenseOpenTime > vecTemp;
		vecTemp.push_back( _info );
		m_mapDefenseOpenTime.insert( std::make_pair( iDayOfWeek, vecTemp) );
	}
	START_LOG( clog, L"디펜스 던전 오픈 시간 추가가 완료 되었습니다." )
		<< BUILD_LOG( iDayOfWeek )
		<< BUILD_LOG( iStartHour )
		<< BUILD_LOG( iStartMin )
		<< BUILD_LOG( iDurationTime )
		<< END_LOG;
}

bool KWorldMissionManager::SetTodayMissionTime( void )
{
	// 요일별 세팅 시간 얻기
	m_tToDayMissionTime = CTime::GetCurrentTime();
	std::map< int, std::vector< KDefenseOpenTime > >::iterator mit = m_mapDefenseOpenTime.find( m_tToDayMissionTime.GetDayOfWeek() );
	if( mit == m_mapDefenseOpenTime.end() )
	{
		START_LOG( cerr, L"디펜스 던전 시간 셋팅 값이 존재 하지 않습니다." );
		return false;
	}

	// 세팅 시간
	std::vector< KDefenseOpenTime > vecTime = mit->second;
	std::vector< KDefenseOpenTime >::iterator vit = vecTime.begin();
	for( ; vit != vecTime.end() ; ++vit )
	{
		if( vit->m_iStartHour > m_tToDayMissionTime.GetHour() )
		{
			m_kTimerDOT.restart();
			m_iNextDOT = (((vit->m_iStartHour - m_tToDayMissionTime.GetHour() ) * 60 ) + ( vit->m_iStartMinute - m_tToDayMissionTime.GetMinute() ) ) * 60 - ( m_tToDayMissionTime.GetSecond()) ;
			SetMissionTime( vit->m_iDurationTime * 60 );

			m_tToDayMissionTime = CTime( m_tToDayMissionTime.GetYear(), m_tToDayMissionTime.GetMonth(), m_tToDayMissionTime.GetDay(), vit->m_iStartHour, vit->m_iStartMinute, 0 );

			START_LOG( cout, L"디펜스 던전 시간 셋팅 완료!!" )
				<< BUILD_LOG(vit->m_iStartHour)
				<< BUILD_LOG(vit->m_iStartMinute)
				<< BUILD_LOG(m_iNextDOT)
				<< BUILD_LOG(m_iMissionTime);
			return true;
		}
		else if( vit->m_iStartHour == m_tToDayMissionTime.GetHour() )
		{
			if( vit->m_iStartMinute >= m_tToDayMissionTime.GetMinute() )
			{
				m_kTimerDOT.restart();
				m_iNextDOT = ( vit->m_iStartMinute - m_tToDayMissionTime.GetMinute() ) * 60 - ( m_tToDayMissionTime.GetSecond());
				SetMissionTime( vit->m_iDurationTime * 60 );

				m_tToDayMissionTime = CTime( m_tToDayMissionTime.GetYear(), m_tToDayMissionTime.GetMonth(), m_tToDayMissionTime.GetDay(), vit->m_iStartHour, vit->m_iStartMinute, 0 );

				START_LOG( cout, L"디펜스 던전 시간 셋팅 완료!!" )
					<< BUILD_LOG(vit->m_iStartHour)
					<< BUILD_LOG(vit->m_iStartMinute)
					<< BUILD_LOG(m_iNextDOT)
					<< BUILD_LOG(m_iMissionTime);
				return true;
			}
		}

		int iStartTimeTotalMin = (vit->m_iStartHour * 60) + vit->m_iStartMinute + vit->m_iDurationTime;
		int iNowTimeTotalMin = m_tToDayMissionTime.GetHour() * 60 + m_tToDayMissionTime.GetMinute();

		if( iStartTimeTotalMin > iNowTimeTotalMin )		// 예외- 이벤트 도중 서버가 내렸다 다시 켜지는 경우 이벤트 시간 초기화
		{
			// 멤버 변수 초기화
			m_iNextDOT = 0;
			m_kTimerDOT.restart();

			// 지금 던전의 시작 시간
			CTime tCurTime = CTime::GetCurrentTime();
			m_tToDayMissionTime = CTime( m_tToDayMissionTime.GetYear(), m_tToDayMissionTime.GetMonth(), m_tToDayMissionTime.GetDay(), vit->m_iStartHour, vit->m_iStartMinute, 0 );

			// 미션 유지 시간
			CTimeSpan gap = tCurTime - m_tToDayMissionTime;
			const int iMissionTime =  static_cast<int>((vit->m_iDurationTime * 60) - gap.GetTotalSeconds());
			SetMissionTime( iMissionTime );

			// DB에 저장된 정보 가져오기
			m_bWorldMissionOn = false;                          // 다른 작업을 무시 하도록 한다.
			KDBE_GET_DEFENCE_CRYSTAL_REQ KReq;
			KReq.m_wsrtStartTime = m_tToDayMissionTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			SendToLogDB( DBE_GET_DEFENCE_CRYSTAL_REQ, KReq );

			START_LOG( cout, L"디펜스 던전 시간 셋팅 완료!!" )
				<< BUILD_LOG(vit->m_iStartHour)
				<< BUILD_LOG(vit->m_iStartMinute)
				<< BUILD_LOG(m_iNextDOT)
				<< BUILD_LOG(m_iMissionTime)
				<< END_LOG;
			return true;
		}
	}

	m_bActive = false;
	m_iNextDOT = 0;
	SetMissionTime( 0 );
	START_LOG( clog, L"오늘 디펜스 던전 시간 지났습니다." );

	return false;
}

void KWorldMissionManager::CheckWorldMissionProcess( void )
{
	if( m_bWorldMissionOn == true && m_bWorldBuffTime == false )
	{
		if( m_bActive == false )
		{
			if( m_iMissionTime <= 0 )
				return;

			if( (int)m_kTimerDOT.elapsed() < m_iNextDOT )
				return;

			StartMission();
		}
		else
		{
			if( m_iMissionTime > GetCorrectMissionTime() )
				return;

			EndMission();
		}
	}

	if( m_bWorldBuffTime && m_kTimerWorldBuff.elapsed() >= m_usWorldBuffDurationTime ) //   버프 시간 수정 // 버프 타임이고 지속시간이 끝났다
	{
		m_bWorldBuffTime = false;
		m_kTimerWorldBuff.restart();	// 시간 초기화
		m_iProtectedCrystalCount = 0;
	}
}

int KWorldMissionManager::GetCorrectMissionTime( void ) const
{
	// 시작 시간을 조금 지났기 때문에 정확한 미션 시간을 보정한다.
	CTime CurrentTime = CTime::GetCurrentTime();

	int iGap = ((CurrentTime.GetHour() - m_tToDayStartTime.GetHour()) * 60 * 60) + ((CurrentTime.GetMinute() - m_tToDayStartTime.GetMinute()) * 60) + (CurrentTime.GetSecond() - m_tToDayStartTime.GetSecond());

	if( iGap < 0 )
	{
		START_LOG( clog, L"던전 미션이 시작하지 않았습니다." )
			<< BUILD_LOG(m_iMissionTime)
			<< BUILD_LOG(iGap)
			<< END_LOG;
		return 0;
	}

	return iGap;
}

void KWorldMissionManager::WriteWorldMissionCrystalCount( IN const bool bforce /*= false */)
{
	if( m_bActive  ||  bforce )
	{
		//	Log DB에 결과저장
		KDBE_INCREASE_DEFENCE_CRYSTAL_REQ PacketReq;
		PacketReq.m_iTargetCrystal			= m_iStartedCrystalCount;
		PacketReq.m_iProtectedCrystalCount	= m_iProtectedCrystalCount;
		PacketReq.m_wsrtStartTime			= m_tToDayMissionTime.Format(_T("%Y-%m-%d %H:%M:%S"));

		SendToLogDB( DBE_INCREASE_DEFENCE_CRYSTAL_REQ, PacketReq );
	}
}

void KWorldMissionManager::SetInitMissionInfo( IN const KDBE_GET_DEFENCE_CRYSTAL_ACK& KPacket )
{
	if( KncUtil::ConvertStringToCTime( KPacket.m_wsrtStartTime, m_tToDayMissionTime ) == false )
	{
		START_LOG( cerr, L"월드 미션 시작 시간이 이상합니다. 변환 실패!" )
			<< BUILD_LOG( KPacket.m_wsrtStartTime )
			<< END_LOG;
		return;
	}

	m_iStartedCrystalCount		= KPacket.m_iTargetCrystal;
	m_iProtectedCrystalCount	= KPacket.m_iProtectedCrystalCount;
	m_bWorldMissionOn			= true;

	StartMission( true );
}

void KWorldMissionManager::SetMissionData( void )
{
	// 비정상 종료로 인한 재 셋팅
	m_iTotalCCU = 0;
	m_iTotalCCU = SiKGlobalServerCCUManager()->GetNowTotalCCU(); // 동접

	m_iReduceCount = lua_tinker::call<int>( GetLuaState(), "COMPUTE_REDUCE_CRYSTAL_COUNT", m_iStartedCrystalCount, m_tToDayMissionTime.GetHour() );

	m_kTimerForReduceTheProtectedCrystalCount.restart();

	BuffProcess();

	START_LOG( cout, L"월드미션 초기화" )
		<< BUILD_LOG( m_iStartedCrystalCount )
		<< BUILD_LOG( m_iProtectedCrystalCount )
		<< BUILD_LOG( m_iReduceCount )
		<< BUILD_LOG( m_iMissionTime )
		<< BUILD_LOG( m_iTotalCCU )
		<< BUILD_LOG( m_usWorldBuffDurationTime )
		<< END_LOG;
}

void KWorldMissionManager::BroadCastIndividualBuffLevel( IN const int& iIndividualBuffLevel ) const
{
	// 모든 게임서버에 브로드 캐스팅
	KEGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT kPacket;
	UidType anTrace[2] = { 0, -1 };

	kPacket.m_iTheGateOfDarknessIndividualBuffLevel	= iIndividualBuffLevel;
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0 , anTrace, EGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT, kPacket );
}
int KWorldMissionManager::CheckWorldBuff( void ) const
{
	if( m_bWorldBuffTime == false )
	{
		return CXSLBuffManager::BTI_NONE;
	}

	if( IsActive() == true )
	{
		return CXSLBuffManager::BTI_NONE;
	}

	if( 0 <= m_iProtectedCrystalCount )
	{
		return CXSLBuffManager::BTI_NONE;
	}

	// 요일별 세팅 시간 얻기
	CTime tCurrent = CTime::GetCurrentTime();
	std::map< int, std::vector< KDefenseOpenTime > >::const_iterator mit = m_mapDefenseOpenTime.find( tCurrent.GetDayOfWeek() );
	if( mit == m_mapDefenseOpenTime.end() )
	{
		START_LOG( cerr, L"디펜스 던전 시간 셋팅 값이 존재 하지 않습니다." );
		return CXSLBuffManager::BTI_NONE;
	}
	return CXSLBuffManager::BTI_NONE;
}

int KWorldMissionManager::CheckIndividualBuff( void ) const
{

	if( m_iProtectedCrystalCount < 0 )
	{
		return CXSLBuffManager::BTI_NONE;
	}

	CTime tCurrentTime = CTime::GetCurrentTime();

	// 요일별 세팅 시간 얻기
	std::map< int, std::vector< KDefenseOpenTime > >::const_iterator mit = m_mapDefenseOpenTime.find( tCurrentTime.GetDayOfWeek() );

	// 해당 요일에 예약된 오픈 시간 존재 여부 검사.
	if( mit == m_mapDefenseOpenTime.end() )
	{
		return CXSLBuffManager::BTI_NONE;
	}
	return CXSLBuffManager::BTI_NONE;
}

void KWorldMissionManager::BuffProcess( void )
{
	int iBuff = CheckWorldBuff();
	if( m_iWorldBuffID != iBuff )
	{
		m_iWorldBuffID = iBuff;
		BroadCastWorldBuff( m_iWorldBuffID );
	}

	iBuff = CheckIndividualBuff();
	if( m_iIndividualBuffID != iBuff )
	{
		m_iIndividualBuffID = iBuff;
		BroadCastIndividualBuffLevel( m_iIndividualBuffID );
	}
}

//{{ 공존의 축제 - 지진 효과 - 김민성
//}}
//}}

//////////////////////////////////////////////////////////////////////////
//}} 2011. 04. 13  김민성  글로벌 서버 추가
//////////////////////////////////////////////////////////////////////////
