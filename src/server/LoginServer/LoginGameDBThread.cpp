#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "LoginGameDBThread.h"
#include "LoginServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "Enum/Enum.h"
#include "SimLayer.h"
#include "X2Data/XSLUnit.h"

#define CLASS_TYPE KLoginGameDBThread
ImplPfID( KLoginGameDBThread, PI_LOGIN_GAME_DB );

IMPL_PROFILER_DUMP( KLoginGameDBThread )
{
	for( unsigned int ui = 0; ui < vecDump.size(); ++ui )
	{
		unsigned int iAvg = 0;
		if( vecDump[ui].m_iQueryCount > 0 )	iAvg = vecDump[ui].m_iTotalTime / vecDump[ui].m_iQueryCount;
		DO_QUERY_NO_PROFILE( L"exec dbo.gup_insert_querystats", L"N\'%s\', %d, %d, %d, %d, %d, %d",
			% vecDump[ui].m_wstrQuery
			% vecDump[ui].m_iMinTime
			% iAvg
			% vecDump[ui].m_iMaxTime
			% vecDump[ui].m_iOver1Sec
			% vecDump[ui].m_iQueryCount
			% vecDump[ui].m_iQueryFail
			);
		continue;

end_proc:
		START_LOG( cerr, vecDump[ui].m_wstrQuery )
			<< BUILD_LOG( vecDump[ui].m_iMinTime )
			<< BUILD_LOG( vecDump[ui].m_iMaxTime )
			<< BUILD_LOG( vecDump[ui].m_iTotalTime )
			<< BUILD_LOG( vecDump[ui].m_iQueryCount )
			<< BUILD_LOG( iAvg )
			<< BUILD_LOG( vecDump[ui].m_iOver1Sec )
			<< BUILD_LOG( vecDump[ui].m_iQueryFail )
			<< END_LOG;
	}
}

KLoginGameDBThread::~KLoginGameDBThread(void)
{
}

void KLoginGameDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
        CASE_NOPARAM( DBE_GET_WEB_RANKING_INFO_REQ );

		//{{ 2009. 11. 19  최육사	길드구조개선
		CASE( DBE_GET_GUILD_INFO_REQ );
		CASE( DBE_GUILD_LEVEL_UP_REQ );
		//}}

		//{{ 2009. 11. 24  최육사	길드스킬

		CASE( DBE_GET_GUILD_SKILL_REQ );
		CASE( DBE_INSERT_GUILD_CASH_SKILL_POINT_REQ );
		CASE( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_REQ );
	   _CASE( DBE_ADMIN_GET_GUILD_SKILL_POINT_REQ, KELG_ADMIN_GET_GUILD_SKILL_POINT_REQ );

		//}}

		//////////////////////////////////////////////////////////////////////////
		//{{ 2010. 01. 13  최육사	길드게시판

		CASE_NOPARAM( DBE_GET_GUILD_BOARD_INFO_REQ );
	   _CASE( DBE_GET_GUILD_SKILL_IN_BOARD_REQ, KELG_GET_GUILD_SKILL_IN_BOARD_REQ );
		CASE( DBE_REGISTRATION_GUILD_AD_REQ );
	   _CASE( DBE_MODIFY_REG_GUILD_AD_REQ, KELG_MODIFY_REG_GUILD_AD_REQ );
		CASE( DBE_APPLY_JOIN_GUILD_REQ );
	   _CASE( DBE_DELETE_APLLY_FOR_ACCEPT_JOIN_GUILD_REQ, KELG_ACCEPT_JOIN_GUILD_REQ );
		CASE( DBE_ACCEPT_JOIN_GUILD_REQ );
	   _CASE( DBE_DELETE_APPLY_JOIN_GUILD_REQ, KELG_DELETE_APPLY_JOIN_GUILD_REQ );
		CASE( DBE_DELETE_GUILD_AD_LIST_NOT );

		//}}
		//////////////////////////////////////////////////////////////////////////

		//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
		//}}

		//{{ 2011. 04. 29	최육사	대리상인
		CASE( DBE_INSERT_REWARD_TO_POST_REQ );

		CASE_NOPARAM( DBE_LOAD_PSHOP_AGENCY_REQ );
	   _CASE( DBE_OPEN_PSHOP_AGENCY_REQ, KERM_OPEN_PERSONAL_SHOP_ACK );
	   _CASE( DBE_BREAK_PSHOP_AGENCY_REQ, KERM_BREAK_PSHOP_AGENCY_NOT );
	   _CASE( DBE_REG_PSHOP_AGENCY_ITEM_REQ, KERM_REG_PERSONAL_SHOP_ITEM_ACK );
		CASE( DBE_BUY_PSHOP_AGENCY_ITEM_REQ );
		CASE( DBE_STOP_SALE_PSHOP_AGENCY_REQ );
		CASE( DBE_PICK_UP_FROM_PSHOP_AGENCY_REQ );
		//}}
		//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
		CASE( DBE_INSERT_TITLE_REQ );
		//}}

		//{{ 2012. 02. 22	박세훈	길드 이름 변경권
		CASE( DBE_CREATE_GUILD_REQ );
		CASE( DBE_CHANGE_GUILD_NAME_CHECK_REQ );
		CASE( DBE_CHANGE_GUILD_NAME_REQ );
		//}}
		//{{ 2012. 06. 05	최육사	배틀필드 시스템
	   _CASE( DBE_CHECK_NICKNAME_FOR_INVITE_PARTY_REQ, KELG_INVITE_PARTY_FIND_RECEIVER_REQ );
		//}}
	   //{{ 2013. 3. 18	박세훈	 로컬 랭킹 시스템
	   //}}

	   //{{ 2013. 04. 01	 인연 시스템 - 김민성
	   CASE_NOPARAM( DBE_LOAD_WEDDING_HALL_INFO_REQ );
	   CASE( DBE_WEDDING_HALL_INFO_UPDATE_NOT );
	   _CASE( DBE_RELATIONSHIP_INFO_GAME_DB_REQ, KELG_RELATIONSHIP_INFO_REQ );
	   _CASE( DBE_WEDDING_ITEM_FIND_INFO_REQ, KELG_WEDDING_ITEM_FIND_INFO_REQ );
	   //}

	default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KLoginGameDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KLoginGameDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}

//{{ 2010. 02. 02  최육사	길드 게시판

bool KLoginGameDBThread::DeleteApplyJoinGuildList( IN int iGuildUID, IN UidType iUnitUID )
{
	int iOK = NetError::ERR_ODBC_01;

	// 길드 가입 신청 삭제
	DO_QUERY( L"exec dbo.gup_delete_guild_bbs_join", L"%d, %d", % iGuildUID % iUnitUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 가입 신청 삭제 실패." )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( iGuildUID )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;
		return false;
	}

	return true;
}

bool KLoginGameDBThread::DeleteApplyJoinGuildList( IN const std::vector< KApplyDeleteInfo >& vecInfo )
{
	bool bRet = true;

	// 길드 가입 신청 삭제
	std::vector< KApplyDeleteInfo >::const_iterator vit;
	for( vit = vecInfo.begin(); vit != vecInfo.end(); ++vit )
	{
		if( DeleteApplyJoinGuildList( vit->m_iGuildUID, vit->m_iUnitUID ) == false )
		{
			START_LOG( cerr, L"길드 가입 신청 삭제 실패." )
				<< BUILD_LOG( vit->m_iGuildUID )
				<< BUILD_LOG( vit->m_iUnitUID )
				<< END_LOG;

			bRet = false;
		}
	}

	return bRet;
}

//}}

//{{ 2009. 7. 6  최육사		랭킹 개편
IMPL_ON_FUNC_NOPARAM( DBE_GET_WEB_RANKING_INFO_REQ )
{
	KDBE_GET_WEB_RANKING_INFO_ACK kPacket;

	//////////////////////////////////////////////////////////////////////////
	// 던전 랭킹 얻기
	DO_QUERY_NO_ARG( L"exec dbo.gup_get_rank_exp" );
	while( m_kODBC.Fetch() )
	{
		KDungeonRankingInfo kInfo;

		FETCH_DATA( kInfo.m_iUnitUID
			>> kInfo.m_iRank
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_iEXP );

		kPacket.m_vecDungeonRanking.push_back( kInfo );
	}

	//////////////////////////////////////////////////////////////////////////
	// 대전 랭킹 얻기
	//{{ 2011. 07. 26	최육사	대전 개편
	DO_QUERY_NO_ARG( L"exec dbo.gup_get_rank_vs" );
	while( m_kODBC.Fetch() )
	{
		KPvpRankingInfo kInfo;
		int iPvpRank = 0;

		FETCH_DATA( kInfo.m_iUnitUID
			>> kInfo.m_iRank
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> iPvpRank
			>> kInfo.m_iWin
			>> kInfo.m_iRPoint );

		//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
		kInfo.m_cRank	= static_cast<CXSLUnit::PVP_RANK>( iPvpRank );
		//}}

		kPacket.m_vecPvpRanking.push_back( kInfo );
	}
	//}}

end_proc:
	SendToServer( DBE_GET_WEB_RANKING_INFO_ACK, kPacket );
}
//}}

//{{ 2009. 11. 19  최육사	길드구조개선
IMPL_ON_FUNC( DBE_GET_GUILD_INFO_REQ )
{
	KDBE_GET_GUILD_INFO_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_kLoginGuildMember = kPacket_.m_kLoginGuildMember;

	//////////////////////////////////////////////////////////////////////////
	// 길드 정보 얻기
	DO_QUERY( L"exec dbo.gup_get_guild_info", L"%d", % kPacket_.m_iGuildUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_kGuildInfo.m_wstrGuildName
			>> kPacket.m_kGuildInfo.m_usMaxNumMember
			>> kPacket.m_kGuildInfo.m_ucGuildLevel
			>> kPacket.m_kGuildInfo.m_iGuildEXP
			>> kPacket.m_kGuildInfo.m_wstrGuildMessage
			>> kPacket.m_kGuildInfo.m_wstrFoundingDay );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 정보 얻기 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GUILD_06;
	}
	else
	{
		// 길드 정보 얻기 성공
		kPacket.m_kGuildInfo.m_iGuildUID = kPacket_.m_iGuildUID;
	}

	//////////////////////////////////////////////////////////////////////////
	// 길드원 리스트 얻기
	DO_QUERY( L"exec dbo.gup_get_guild_member_info", L"%d", % kPacket_.m_iGuildUID );
	while( m_kODBC.Fetch() )
	{
		KGuildMemberInfo kInfo;
		std::wstring wstrLogOutTime;

		FETCH_DATA( kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_ucMemberShipGrade
			>> wstrLogOutTime
			>> kInfo.m_wstrMessage
			);

		CTime tLogOutTime;
		KncUtil::ConvertStringToCTime( wstrLogOutTime, tLogOutTime );
		kInfo.m_tLogOutTime = tLogOutTime.GetTime();

		kPacket.m_vecMemberList.push_back( kInfo );
	}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 11. 25  최육사	길드스킬

	//////////////////////////////////////////////////////////////////////////
	// 캐시스킬 포인트 관련 정보 받아오기
	DO_QUERY( L"exec dbo.gup_get_guild_skill_point_info", L"%d", % kPacket_.m_iGuildUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_kGuildSkillInfo.m_iGuildSPoint
			>> kPacket.m_kGuildSkillInfo.m_iGuildCSPoint
			>> kPacket.m_kGuildSkillInfo.m_iMaxGuildCSPoint
			>> kPacket.m_kGuildSkillInfo.m_wstrGuildCSPointEndDate );

		m_kODBC.EndFetch();
	}
	else
	{
		START_LOG( cerr, L"길드스킬포인트 정보 얻어오기 실패로 기본값을 넣어줌." )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		kPacket.m_kGuildSkillInfo.m_iGuildSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_iGuildCSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_iMaxGuildCSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_wstrGuildCSPointEndDate = L"2000-01-01 00:00:00";
	}

	// GUILD SKILL LIST 받아오기
	DO_QUERY( L"exec dbo.gup_get_guild_skill_list_new", L"%d", % kPacket_.m_iGuildUID );
	while( m_kODBC.Fetch() )
	{
		KGuildSkillData kInfo;

		FETCH_DATA( kInfo.m_iSkillID
			>> kInfo.m_cSkillLevel
			>> kInfo.m_cSkillCSPoint
			);

		kPacket.m_kGuildSkillInfo.m_vecGuildSkill.push_back( kInfo );
	}

	//}}
	//////////////////////////////////////////////////////////////////////////
end_proc:
	SendToUser( LAST_SENDER_UID, DBE_GET_GUILD_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_GUILD_LEVEL_UP_REQ )
{
	KDBE_GUILD_LEVEL_UP_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_wstrGuildName = kPacket_.m_wstrGuildName;
	kPacket.m_ucGuildLevel = kPacket_.m_ucGuildLevel;
	//{{ 2009. 11. 26  최육사	길드스킬
	kPacket.m_iGuildSPoint = kPacket_.m_iGuildSPoint;
	//}}

	//////////////////////////////////////////////////////////////////////////
	// 길드 LEVEL 업데이트
	DO_QUERY( L"exec dbo.gup_update_guild_level", L"%d, %d",
		% kPacket_.m_iGuildUID
		% kPacket_.m_ucGuildLevel
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 레벨 업데이트 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOGc( kPacket_.m_ucGuildLevel )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_29; break; // 트랜젝션 실패
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

	//{{ 2009. 11. 26  최육사	길드스킬
	DO_QUERY( L"exec dbo.gup_update_guild_skill_point", L"%d, %d", % kPacket_.m_iGuildUID % kPacket_.m_iGuildSPoint );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 스킬 포인트 업데이트 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOGc( kPacket_.m_ucGuildLevel )
			<< BUILD_LOG( kPacket_.m_iGuildSPoint )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

	//}}

end_proc:
	SendToServer( DBE_GUILD_LEVEL_UP_ACK, kPacket );
}
//}}

//{{ 2009. 11. 24  최육사	길드스킬

IMPL_ON_FUNC( DBE_GET_GUILD_SKILL_REQ )
{
	KDBE_GET_GUILD_SKILL_ACK kPacket;
	kPacket.m_iOK				 = NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID			 = kPacket_.m_iGuildUID;
	kPacket.m_iUnitUID			 = kPacket_.m_iUnitUID;
	kPacket.m_iGuildSkillID		 = kPacket_.m_iGuildSkillID;
	kPacket.m_iGuildSkillLevel	 = kPacket_.m_iGuildSkillLevel;
	kPacket.m_iGuildSkillCSPoint = kPacket_.m_iGuildSkillCSPoint;
	kPacket.m_iGuildSPoint		 = kPacket_.m_iGuildSPoint;
	kPacket.m_iGuildCSPoint		 = kPacket_.m_iGuildCSPoint;

	// 캐시 스킬 포인트 정보를 갱신
	if( kPacket_.m_iGuildCSPoint >= 0 )
	{
		DO_QUERY( L"exec dbo.gup_update_guild_cash_skill_point_info", L"%d, %d, %d",
			% kPacket_.m_iGuildUID
			% kPacket_.m_iGuildCSPoint
			% kPacket_.m_iMaxGuildCSPoint
			);
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK );
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"기간제 길드 스킬 포인트 DB업데이트 실패!" )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_iGuildCSPoint )
				<< BUILD_LOG( kPacket_.m_iMaxGuildCSPoint )
				<< END_LOG;

			switch( kPacket.m_iOK )
			{
			case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
			case -2: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_06; break; // 길드스킬 배우기 실패
			default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
			}

			goto end_proc;
		}
	}
	// 스킬 포인트 업데이트
	else
	{
		DO_QUERY( L"exec dbo.gup_update_guild_skill_point", L"%d, %d",
			% kPacket_.m_iGuildUID
			% kPacket_.m_iGuildSPoint );
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK );
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"길드 스킬 포인트 DB업데이트 실패!" )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_iGuildSPoint )
				<< END_LOG;

			switch( kPacket.m_iOK )
			{
			case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
			case -2: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_06; break; // 길드스킬 배우기 실패
			default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
			}

			goto end_proc;
		}
	}

	// 스킬 획득 정보를 갱신
	DO_QUERY( L"exec dbo.gup_update_guild_skill_info_new", L"%d, %d, %d, %d",
		% kPacket_.m_iGuildUID
		% kPacket_.m_iGuildSkillID
		% kPacket_.m_iGuildSkillLevel
		% kPacket_.m_iGuildSkillCSPoint );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 스킬 배우기 DB업데이트 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iGuildSkillID )
			<< BUILD_LOG( kPacket_.m_iGuildSkillLevel )
			<< BUILD_LOG( kPacket_.m_iGuildSkillCSPoint )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_06; break;
		case -3: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_06; break;
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_GET_GUILD_SKILL_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_INSERT_GUILD_CASH_SKILL_POINT_REQ )
{
	KDBE_INSERT_GUILD_CASH_SKILL_POINT_ACK kPacket;
	kPacket.m_iOK					= NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID				= kPacket_.m_iGuildUID;
	kPacket.m_iGuildCSPoint			= kPacket_.m_iGuildCSPoint;
	kPacket.m_iPeriod				= kPacket_.m_iPeriod;
	kPacket.m_bUpdateEndDateOnly	= kPacket_.m_bUpdateEndDateOnly;

	// 기간 연장인지? 새로 추가인지?
	if( kPacket_.m_bUpdateEndDateOnly )
	{
		DO_QUERY( L"exec dbo.gup_update_guild_cash_skill_period", L"%d, %d",
			% kPacket_.m_iGuildUID
			% kPacket_.m_iPeriod
			);
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK
				>> kPacket.m_wstrEndDate );
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"failed to insert guild cash skill point!" )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_iGuildCSPoint )
				<< BUILD_LOG( kPacket_.m_iPeriod )
				<< END_LOG;

			switch( kPacket.m_iOK )
			{
			case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
			case -3: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_16; break;
			case -4: kPacket.m_iOK = NetError::ERR_GUILD_SKILL_16; break;
			default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
			}

			goto end_proc;
		}
	}
	else
	{
		DO_QUERY( L"exec dbo.gup_insert_guild_cash_skill_point_info", L"%d, %d, %d",
			% kPacket_.m_iGuildUID
			% kPacket_.m_iGuildCSPoint
			% kPacket_.m_iPeriod
			);
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK
				>> kPacket.m_wstrEndDate );
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"failed to insert guild cash skill point!" )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_iGuildCSPoint )
				<< BUILD_LOG( kPacket_.m_iPeriod )
				<< END_LOG;

			switch( kPacket.m_iOK )
			{
			case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
			default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
			}

			goto end_proc;
		}
	}

end_proc:
	SendToServer( DBE_INSERT_GUILD_CASH_SKILL_POINT_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_REQ )
{
	KDBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK kPacket;
	kPacket.m_iOK				= NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID			= kPacket_.m_iGuildUID;
	kPacket.m_iRetrievedSPoint	= kPacket_.m_iRetrievedSPoint;

	// guild cash skill point 정보를 초기화 한다
	DO_QUERY( L"exec dbo.gup_update_guild_cash_skill_point_info", L"%d, %d, %d",
		% kPacket_.m_iGuildUID
		% 0
		% 0
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"failed to reset guild cash skill point!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iRetrievedSPoint )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

	// SP정보도 DB에 업데이트!
	DO_QUERY( L"exec dbo.gup_update_guild_skill_point", L"%d, %d", % kPacket_.m_iGuildUID % kPacket_.m_iGuildSPoint );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 스킬 포인트 업데이트 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iGuildSPoint )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

	// note!! 300개 이상 skill정보가 바뀌는 경우가 있을까? 로그만 남기기.
	if( kPacket_.m_vecGuildSkillData.size() > 300 )
	{
		START_LOG( cerr, L"too many skill update on cash skill point expiration!" )
			<< BUILD_LOG( (int) kPacket_.m_vecGuildSkillData.size() )
			<< END_LOG;
	}

	for( u_int ui = 0; ui < kPacket_.m_vecGuildSkillData.size(); ++ui )
	{
		const KGuildSkillData& kGuildSkillData = kPacket_.m_vecGuildSkillData[ui];
		DO_QUERY( L"exec dbo.gup_update_guild_skill_info_new", L"%d, %d, %d, %d",
			% kPacket_.m_iGuildUID
			% kGuildSkillData.m_iSkillID
			% kGuildSkillData.m_cSkillLevel
			% kGuildSkillData.m_cSkillCSPoint );
		int iResult = NetError::ERR_UNKNOWN;
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iResult );
			m_kODBC.EndFetch();
		}

		if( iResult != NetError::NET_OK )
		{
			START_LOG( cerr, L"failed to update skill on cash skill point expire!" )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_iRetrievedSPoint )
				<< BUILD_LOG( kGuildSkillData.m_iSkillID )
				<< BUILD_LOG( kGuildSkillData.m_cSkillLevel )
				<< BUILD_LOG( kGuildSkillData.m_cSkillCSPoint )
				<< END_LOG;

			switch( kPacket.m_iOK )
			{
			case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
			default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
			}
		}
	}

end_proc:
	SendToServer( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_ADMIN_GET_GUILD_SKILL_POINT_REQ, KELG_ADMIN_GET_GUILD_SKILL_POINT_REQ )
{
	KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iGuildSPoint = kPacket_.m_iGuildSPoint;

	// 길드 스킬 포인트 올리자!
	DO_QUERY( L"exec dbo.gup_update_guild_skill_point", L"%d, %d", % kPacket_.m_iGuildUID % kPacket_.m_iGuildSPoint );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 스킬 포인트 업데이트 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iGuildSPoint )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

end_proc:
	SendToServer( DBE_ADMIN_GET_GUILD_SKILL_POINT_ACK, kPacket );
}

//}}

//////////////////////////////////////////////////////////////////////////
//{{ 2010. 01. 13  최육사	길드게시판

IMPL_ON_FUNC_NOPARAM( DBE_GET_GUILD_BOARD_INFO_REQ )
{
	KDBE_GET_GUILD_BOARD_INFO_ACK kPacket;

	// 길드 광고 리스트 얻기
	DO_QUERY_NO_ARG( L"exec dbo.gup_get_guild_bbs_list" );
	while( m_kODBC.Fetch() )
	{
		KGuildAdInfo kInfo;

		FETCH_DATA( kInfo.m_iGuildUID
			>> kInfo.m_wstrGuildName
			>> kInfo.m_ucGuildLevel
			>> kInfo.m_wstrMasterNickName
			>> kInfo.m_usCurNumMember
			>> kInfo.m_wstrAdMessage
			//{{ 2010. 02. 04  최육사	길드 광고 오류 수정
			>> kInfo.m_wstrFoundingDay
			//}}
			>> kInfo.m_wstrAdRegDate
			>> kInfo.m_wstrAdEndDate );

		kPacket.m_vecGuildAdList.push_back( kInfo );
	}

	// 길드 광고 등록 유저 얻기
	DO_QUERY_NO_ARG( L"exec dbo.gup_get_guild_bbs_join_list" );
	while( m_kODBC.Fetch() )
	{
		KApplyJoinGuildInfo kInfo;

		FETCH_DATA( kInfo.m_iApplyJoinGuildUID
			>> kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_wstrMessage
			>> kInfo.m_wstrRegDate );

		kPacket.m_vecApplyJoinGuildList.push_back( kInfo );
	}

end_proc:
	SendToServer( DBE_GET_GUILD_BOARD_INFO_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_GET_GUILD_SKILL_IN_BOARD_REQ, KELG_GET_GUILD_SKILL_IN_BOARD_REQ )
{
	KDBE_GET_GUILD_SKILL_IN_BOARD_ACK kPacket;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 11. 25  최육사	길드스킬

	//////////////////////////////////////////////////////////////////////////
	// 캐시스킬 포인트 관련 정보 받아오기
	DO_QUERY( L"exec dbo.gup_get_guild_skill_point_info", L"%d", % kPacket_.m_iGuildUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_kGuildSkillInfo.m_iGuildSPoint
			>> kPacket.m_kGuildSkillInfo.m_iGuildCSPoint
			>> kPacket.m_kGuildSkillInfo.m_iMaxGuildCSPoint
			>> kPacket.m_kGuildSkillInfo.m_wstrGuildCSPointEndDate );

		m_kODBC.EndFetch();
	}
	else
	{
		START_LOG( cerr, L"길드스킬포인트 정보 얻어오기 실패로 기본값을 넣어줌." )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		kPacket.m_kGuildSkillInfo.m_iGuildSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_iGuildCSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_iMaxGuildCSPoint = 0;
		kPacket.m_kGuildSkillInfo.m_wstrGuildCSPointEndDate = L"2000-01-01 00:00:00";
	}

	// GUILD SKILL LIST 받아오기
	DO_QUERY( L"exec dbo.gup_get_guild_skill_list_new", L"%d", % kPacket_.m_iGuildUID );
	while( m_kODBC.Fetch() )
	{
		KGuildSkillData kInfo;

		FETCH_DATA( kInfo.m_iSkillID
			>> kInfo.m_cSkillLevel
			>> kInfo.m_cSkillCSPoint
			);

		kPacket.m_kGuildSkillInfo.m_vecGuildSkill.push_back( kInfo );
	}

	//}}
	//////////////////////////////////////////////////////////////////////////

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_GET_GUILD_SKILL_IN_BOARD_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_REGISTRATION_GUILD_AD_REQ )
{
	KDBE_REGISTRATION_GUILD_AD_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_sPeriod = kPacket_.m_sPeriod;
	kPacket.m_wstrAdMessage = kPacket_.m_wstrAdMessage;
	kPacket.m_iCost = kPacket_.m_iCost;
	kPacket.m_bExistExpiredAd = kPacket_.m_bExistExpiredAd;

	// 기존에 기간만료된 광고가 있다면 먼저 삭제하자!
	if( kPacket_.m_bExistExpiredAd )
	{
		// 길드 광고 삭제
		DO_QUERY( L"exec dbo.gup_delete_guild_bbs", L"%d", % kPacket_.m_iGuildUID );
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( kPacket.m_iOK );
			m_kODBC.EndFetch();
		}

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"길드 광고 삭제 실패." )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_GUILD_BOARD_06;
			goto end_proc;
		}
	}

	// 길드 광고를 등록하자!
	DO_QUERY( L"exec dbo.gup_insert_guild_bbs", L"%d, N\'%s\', %d", % kPacket_.m_iGuildUID % kPacket_.m_wstrAdMessage % kPacket_.m_sPeriod );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_wstrAdRegDate
			>> kPacket.m_wstrAdEndDate );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 광고 등록 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_sPeriod )
			<< BUILD_LOG( kPacket_.m_wstrAdMessage )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_BOARD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_REGISTRATION_GUILD_AD_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_MODIFY_REG_GUILD_AD_REQ, KELG_MODIFY_REG_GUILD_AD_REQ )
{
	KDBE_MODIFY_REG_GUILD_AD_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_sPeriod = kPacket_.m_sPeriod;
	kPacket.m_wstrAdMessage = kPacket_.m_wstrAdMessage;
	kPacket.m_iCost = kPacket_.m_iCost;

	// 길드 광고 삭제
	DO_QUERY( L"exec dbo.gup_delete_guild_bbs", L"%d", % kPacket_.m_iGuildUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 광고 삭제 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		goto end_proc;
	}

	// 길드 광고를 등록하자!
	DO_QUERY( L"exec dbo.gup_insert_guild_bbs", L"%d, N\'%s\', %d", % kPacket_.m_iGuildUID % kPacket_.m_wstrAdMessage % kPacket_.m_sPeriod );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_wstrAdRegDate
			>> kPacket.m_wstrAdEndDate );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 광고 등록 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_sPeriod )
			<< BUILD_LOG( kPacket_.m_wstrAdMessage )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_BOARD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_MODIFY_REG_GUILD_AD_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_APPLY_JOIN_GUILD_REQ )
{
	KDBE_APPLY_JOIN_GUILD_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_kApplyJoinGuildInfo = kPacket_.m_kApplyJoinGuildInfo;

	// 길드 가입 신청 삭제
	if( DeleteApplyJoinGuildList( kPacket_.m_vecDeleteApply ) == false )
	{
		START_LOG( cerr, L"길드 가입 신청 삭제 실패." )
			<< BUILD_LOG( kPacket_.m_vecDeleteApply.size() )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GUILD_BOARD_12;
		goto end_proc;
	}

	// 길드 가입 신청을 등록하자!
	DO_QUERY( L"exec dbo.gup_insert_guild_bbs_join", L"%d, %d, N\'%s\'",
		% kPacket_.m_kApplyJoinGuildInfo.m_iApplyJoinGuildUID
		% kPacket_.m_kApplyJoinGuildInfo.m_iUnitUID
		% kPacket_.m_kApplyJoinGuildInfo.m_wstrMessage );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_kApplyJoinGuildInfo.m_wstrRegDate );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 광고 등록 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_kApplyJoinGuildInfo.m_iApplyJoinGuildUID )
			<< BUILD_LOG( kPacket_.m_kApplyJoinGuildInfo.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_kApplyJoinGuildInfo.m_wstrMessage )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_BOARD_08; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_APPLY_JOIN_GUILD_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_DELETE_APLLY_FOR_ACCEPT_JOIN_GUILD_REQ, KELG_ACCEPT_JOIN_GUILD_REQ )
{
	KDBE_DELETE_APLLY_FOR_ACCEPT_JOIN_GUILD_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iApplyJoinUnitUID = kPacket_.m_iApplyJoinUnitUID;

	// 길드 가입 신청 삭제
	if( DeleteApplyJoinGuildList( kPacket_.m_iGuildUID, kPacket_.m_iApplyJoinUnitUID ) == false )
	{
		START_LOG( cerr, L"길드 가입 신청 삭제 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iApplyJoinUnitUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GUILD_BOARD_11;
		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_DELETE_APLLY_FOR_ACCEPT_JOIN_GUILD_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_ACCEPT_JOIN_GUILD_REQ )
{
    KDBE_ACCEPT_JOIN_GUILD_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_kJoinGuildMemberInfo = kPacket_.m_kJoinGuildMemberInfo;

	//////////////////////////////////////////////////////////////////////////
	// 길드 가입
	DO_QUERY( L"exec dbo.gup_create_guild_member", L"%d, %d, N\'%s\'",
		% kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID
		% kPacket_.m_iGuildUID
		% L""
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		if( kPacket.m_iOK == -3 )
		{
			START_LOG( cwarn, L"길드 최대 가입 인원 부족." )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID )
				<< END_LOG;
		}
		else
		{
			START_LOG( cerr, L"길드 가입 실패." )
				<< BUILD_LOG( kPacket.m_iOK )
				<< BUILD_LOG( kPacket_.m_iGuildUID )
				<< BUILD_LOG( kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID )
				<< END_LOG;
		}

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_14; break; // 이미 길드에 가입되어있음
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_18; break; // 길드 가입 실패
		case -3: kPacket.m_iOK = NetError::ERR_GUILD_18; break; // 길드 가입 최대 인원 제한
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}
	}

end_proc:
	SendToServer( DBE_ACCEPT_JOIN_GUILD_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_DELETE_APPLY_JOIN_GUILD_REQ, KELG_DELETE_APPLY_JOIN_GUILD_REQ )
{
	KDBE_DELETE_APPLY_JOIN_GUILD_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iDeletedUnitUID = kPacket_.m_iDeletedUnitUID;
	kPacket.m_bDeleteChar = kPacket_.m_bDeleteChar;

	// 길드 가입 신청 삭제
	if( DeleteApplyJoinGuildList( kPacket_.m_iGuildUID, kPacket_.m_iDeletedUnitUID ) == false )
	{
		START_LOG( cerr, L"길드 가입 신청 삭제 실패." )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iDeletedUnitUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GUILD_BOARD_12;
	}

	SendToUser( LAST_SENDER_UID, DBE_DELETE_APPLY_JOIN_GUILD_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_DELETE_GUILD_AD_LIST_NOT )
{
	{ // 길드 광고 삭제
		std::vector< int >::const_iterator vit;
		for( vit = kPacket_.m_vecDeleteGuildAd.begin(); vit != kPacket_.m_vecDeleteGuildAd.end(); ++vit )
		{
			int iOK = NetError::ERR_ODBC_01;

			// 길드 광고 삭제
			DO_QUERY( L"exec dbo.gup_delete_guild_bbs", L"%d", % *vit );
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}

end_proc:
			if( iOK != NetError::NET_OK )
			{
				START_LOG( cerr, L"길드 광고 삭제 실패." )
					<< BUILD_LOG( iOK )
					<< BUILD_LOG( *vit )
					<< END_LOG;
			}
		}
	}

	// 길드 가입 신청 삭제
	if( DeleteApplyJoinGuildList( kPacket_.m_vecDeleteApply ) == false )
	{
        START_LOG( cerr, L"길드 가입 신청 삭제 실패!" )
			<< BUILD_LOG( kPacket_.m_vecDeleteApply.size() )
			<< END_LOG;
	}
}

//}}
//////////////////////////////////////////////////////////////////////////

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

//{{ 2011. 04. 29	최육사	대리상인

IMPL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_REQ )
{
	KDBE_INSERT_REWARD_TO_POST_ACK kPacket;
	kPacket.m_iOK					  	   = NetError::ERR_ODBC_01;
	kPacket.m_iRewardLetter.m_iFromUnitUID = kPacket_.m_iFromUnitUID;
	kPacket.m_iRewardLetter.m_iToUnitUID   = kPacket_.m_iToUnitUID;
	kPacket.m_iRewardLetter.m_cScriptType  = static_cast<char>(kPacket_.m_iRewardType);
	kPacket.m_iRewardLetter.m_iScriptIndex = kPacket_.m_iRewardID;
	kPacket.m_iRewardLetter.m_iQuantity	   = kPacket_.m_sQuantity;
	kPacket.m_iRewardLetter.m_wstrMessage  = kPacket_.m_wstrMessage;

	// 보상
	DO_QUERY( L"exec dbo.gup_insert_post_item", L"%d, %d, %d, %d, %d, N\'%s\'",
		% kPacket_.m_iFromUnitUID
		% kPacket_.m_iToUnitUID
		% kPacket_.m_sQuantity
		% kPacket_.m_iRewardType
		% kPacket_.m_iRewardID
		% kPacket_.m_wstrMessage
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_iRewardLetter.m_iPostNo
			>> kPacket.m_iRewardLetter.m_wstrRegDate );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK )
		{
		case -1:    kPacket.m_iOK = NetError::ERR_REWARD_TO_POST_00;   break;
		}

		goto end_proc;
	}

end_proc:
	SendToServer( DBE_INSERT_REWARD_TO_POST_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( DBE_LOAD_PSHOP_AGENCY_REQ )
{
	KDBE_LOAD_PSHOP_AGENCY_ACK kPacket;

	// 대리 상점 정보 얻기
	DO_QUERY_NO_ARG( L"exec dbo.gup_get_PShop_info" );
	while( m_kODBC.Fetch() )
	{
		KERM_OPEN_PSHOP_AGENCY_BY_SERVER_NOT kInfo;

		FETCH_DATA( kInfo.m_iUserUID
			>> kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cPersonalShopType
			>> kInfo.m_wstrPersonalShopName
			>> kInfo.m_wstrAgencyOpenDate
			>> kInfo.m_wstrAgencyExpirationDate
			>> kInfo.m_bOnSale
			);

		kPacket.m_vecOpenPShopAgency.push_back( kInfo );
	}

	// 대리 상점 인벤 정보 얻기
	{
		for( KERM_OPEN_PSHOP_AGENCY_BY_SERVER_NOT& kInfo : kPacket.m_vecOpenPShopAgency )
		{
			// 상점 판매 물품 얻기
			DO_QUERY( L"exec dbo.P_GPShopItem_SEL", L"%d", % kInfo.m_iUnitUID );
			while( m_kODBC.Fetch() )
			{
				KSellPShopItemBackupData kItemInfo;
				kItemInfo.m_kSellPShopItemInfo.m_cPShopItemType = KSellPersonalShopItemInfo::SPIT_PSHOP_AGENCY;

				//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
				const byte byteArraySize = 5;
				int arrSocketOption[byteArraySize];
				memset( arrSocketOption, 0, sizeof(int) * byteArraySize );
				//}}

				int arrRandomSocketOption[5] = {0};

				FETCH_DATA( kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_iItemUID
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iItemID
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_cUsageType
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iQuantity
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_sEndurance
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_ucSealData
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_cEnchantLevel
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant0
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant1
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant2
					>> arrSocketOption[0]
					>> arrSocketOption[1]
					>> arrSocketOption[2]
					>> arrSocketOption[3]
					>> arrSocketOption[4]
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_byteExpandedSocketNum
					>> arrRandomSocketOption[0]
					>> arrRandomSocketOption[1]
					>> arrRandomSocketOption[2]
					>> arrRandomSocketOption[3]
					>> arrRandomSocketOption[4]
					>> kItemInfo.m_kSellPShopItemInfo.m_iPricePerOne
					>> kItemInfo.m_kSellPShopItemInfo.m_iFeePerOne
					>> kItemInfo.m_kSellPShopItemInfo.m_iTotalSellEDIn
					>> kItemInfo.m_iTotalSoldItemQuantity
					>> kItemInfo.m_iTotalSellCommissionED
					>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_cSlotCategory
					);

					int iCheckRandomIdx;
					for( iCheckRandomIdx = 4; iCheckRandomIdx >= 0; --iCheckRandomIdx )
					{
						if( arrRandomSocketOption[iCheckRandomIdx] != 0 )
							break;
					}

					for( int iIdx = 0; iIdx <= iCheckRandomIdx; ++iIdx )
					{
						kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_vecRandomSocket.push_back( arrRandomSocketOption[iIdx] );
					}


				int iCheckIdx = byteArraySize;
				while( 0 <= --iCheckIdx )
				{
					if( arrSocketOption[iCheckIdx] != 0 )
						break;
				}

				for( int iIdx = 0; iIdx <= iCheckIdx; ++iIdx )
				{
					kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_vecItemSocket.push_back( arrSocketOption[iIdx] );
				}

				kInfo.m_vecSellItemInfo.push_back( kItemInfo );
			}
		}
	}

	// 대리상점이 정상적으로 개설된건지 체크하기!
	{
		for( const KERM_OPEN_PSHOP_AGENCY_BY_SERVER_NOT& kInfo : kPacket.m_vecOpenPShopAgency )
		{
			if( kInfo.m_vecSellItemInfo.empty() == false )
				continue;

			START_LOG( cerr, L"등록된 아이템이 없는데 개설된 상태입니다! 강제로 상점을 닫습니다!" )
				<< BUILD_LOG( kInfo.m_iUserUID )
				<< BUILD_LOG( kInfo.m_iUnitUID )
				<< BUILD_LOG( kInfo.m_wstrNickName )
				<< BUILD_LOG( kInfo.m_wstrAgencyExpirationDate )
				<< BUILD_LOG( kInfo.m_wstrAgencyOpenDate )
				<< BUILD_LOG( kInfo.m_vecSellItemInfo.size() )
				<< END_LOG;

			int iOK = NetError::ERR_ODBC_01;

			// 대리 상인 닫기
			DO_QUERY( L"exec dbo.gup_update_PShop_info_IsPShopOpen", L"%d, %d",
				% kInfo.m_iUnitUID
				% false
				);
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}

			if( iOK != NetError::NET_OK )
			{
				START_LOG( cerr, L"대리 상인 닫기 실패!" )
					<< BUILD_LOG( iOK )
					<< BUILD_LOG( kInfo.m_iUnitUID )
					<< END_LOG;
			}
		}
	}

end_proc:
	SendToServer( DBE_LOAD_PSHOP_AGENCY_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_OPEN_PSHOP_AGENCY_REQ, KERM_OPEN_PERSONAL_SHOP_ACK )
{
	kPacket_.m_iOK = NetError::ERR_ODBC_01;

	//////////////////////////////////////////////////////////////////////////
	// 대리 상인 UID 등록
	DO_QUERY( L"exec dbo.gup_update_PShopinfo", L"%d, %d, N\'%s\', %d, %d",
		% kPacket_.m_iUnitUID
		% true
		% kPacket_.m_wstrAgencyOpenDate
		% true
		% (int)kPacket_.m_cPersonalShopType
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket_.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"대리 상인 UID 등록 실패!" )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iPersonalShopUID )
			<< BUILD_LOG( kPacket_.m_wstrAgencyOpenDate )
			<< BUILD_LOGc( kPacket_.m_cPersonalShopType )
			<< END_LOG;

		kPacket_.m_iOK = NetError::ERR_PERSONAL_SHOP_44;
		goto end_proc;
	}

end_proc:
	SendToRoom( FIRST_SENDER_UID, DBE_OPEN_PSHOP_AGENCY_ACK, kPacket_ );
}

_IMPL_ON_FUNC( DBE_BREAK_PSHOP_AGENCY_REQ, KERM_BREAK_PSHOP_AGENCY_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	// 대리 상인 닫기
	DO_QUERY( L"exec dbo.gup_update_PShop_info_IsPShopOpen", L"%d, %d",
		% kPacket_.m_iHostUID
		% false
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"대리 상인 닫기 실패!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iHostUID )
			<< END_LOG;

		iOK = NetError::ERR_UNKNOWN;
	}

end_proc:
	SendToServer( DBE_BREAK_PSHOP_AGENCY_ACK, kPacket_ );
}

//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
_IMPL_ON_FUNC( DBE_REG_PSHOP_AGENCY_ITEM_REQ, KERM_REG_PERSONAL_SHOP_ITEM_ACK )
{
	UidType iItemUID[SEnum::PAE_SELL_ITEM_LIMIT_NUM] = {0,};
	// SP 쿼리를 만들어 보자
	//{{ 2013. 05. 28	최육사	아이템 개편
	std::wstring wstrQuerySP = L"exec dbo.P_GItem_Total_Pshop_New ";
	//}}
	int iTemp_Type = 2;		// 대리 상점 아이템 등록 sp 타입
	int iTemp_Zero = 0;

	// 대리 상점 이름 등록
	DO_QUERY( L"exec dbo.gup_update_PShop_info_PShopName", L"%d, N\'%s\'",
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrPersonalShopName
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket_.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"대리 상점 이름 등록 실패!" )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrPersonalShopName )
			<< END_LOG;

		kPacket_.m_iOK = NetError::ERR_PERSONAL_SHOP_45;
		goto end_proc;
	}

	wstrQuerySP += x2::str( x2::wformat( L"%d" ) % iTemp_Type );

	// 판매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iUnitUID );

	// 구매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );

	// 판매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// 수수료가 다르기 때문에 각자 처리 해야한다.

	// 구매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// 수수료가 다르기 때문에 각자 처리 해야한다.

	// 등록 아이템
	for( KSellPersonalShopItemInfo& kSellItemInfo : kPacket_.m_vecSellItemInfo )
	{
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_kInventoryItemInfo.m_iItemUID );								// ItemUID
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iQuantity );				// Quantity
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_iPricePerOne );												// price
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % static_cast<int>(kSellItemInfo.m_kInventoryItemInfo.m_cSlotCategory) );		// category
	}

	for(int i = static_cast<int>(kPacket_.m_vecSellItemInfo.size()) ; i < SEnum::PAE_SELL_ITEM_LIMIT_NUM ; ++i )// 아이템이 빈 곳을 채워주는 곳
	{
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// ItemUID
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// Quantity
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// price
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// category
	}

	// 상점 이름
	wstrQuerySP += x2::str( x2::wformat( L", N\'%s\'" ) % kPacket_.m_wstrPersonalShopName );

	DO_QUERY_NO_ARG( wstrQuerySP.c_str() );

	// 아이템은 9개 이므로 9개로 고정한다.

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket_.m_iOK
			>> iItemUID[0]			// 판매되는 아이템
			>> iItemUID[1]
			>> iItemUID[2]
			>> iItemUID[3]
			>> iItemUID[4]
			>> iItemUID[5]
			>> iItemUID[6]
			>> iItemUID[7]
			>> iItemUID[8]
            >> iItemUID[9]
            >> iItemUID[10]
            >> iItemUID[11]
            >> iItemUID[12]
            >> iItemUID[13]
            >> iItemUID[14]
            );

		m_kODBC.EndFetch();

		if( kPacket_.m_iOK != NetError::NET_OK )
		{
            // slect pshopitem..
            GetPShopItem( kPacket_.m_iUnitUID, kPacket_.m_vecAddCompleteItemInfo );

            START_LOG( cerr, L"대리상점 등록 SP 호출이 실패 했습니다." )
                << BUILD_LOG( kPacket_.m_iOK )
                << BUILD_LOG( kPacket_.m_iUnitUID )
                << BUILD_LOG( GET_LAST_QUERY() )
                << BUILD_LOG( kPacket_.m_vecAddCompleteItemInfo.size() )
                << END_LOG;

			goto end_proc;
		}

		int index = 0;
		for( KSellPersonalShopItemInfo& kSellItemInfo : kPacket_.m_vecSellItemInfo )
		{
			KSellPersonalShopItemInfo kItemTemp;
			kItemTemp = kSellItemInfo;
			kItemTemp.m_cPShopItemType = KSellPersonalShopItemInfo::SPIT_PSHOP_AGENCY;
			kItemTemp.m_kInventoryItemInfo.m_iItemUID = iItemUID[index++];
			kPacket_.m_vecAddCompleteItemInfo.push_back( kItemTemp );
		}
	}

end_proc:
	SendToRoom( FIRST_SENDER_UID, DBE_REG_PSHOP_AGENCY_ITEM_ACK, kPacket_ );
}
//}}

//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
IMPL_ON_FUNC( DBE_BUY_PSHOP_AGENCY_ITEM_REQ )
{
	UidType iItemUID[SEnum::PAE_SELL_ITEM_LIMIT_NUM] = {0,};
	kPacket_.m_iOK = NetError::ERR_ODBC_01;

	// SP 쿼리를 만들어 보자
	int iTemp_Type = 3;		// 대리 상점 sp 타입 - 물품 구매
	int iTemp_Zero = 0;
	//{{ 2013. 05. 28	최육사	아이템 개편
	std::wstring wstrQuerySP = L"exec dbo.P_GItem_Total_Pshop_New ";
	//}}
	wstrQuerySP += x2::str( x2::wformat( L"%d" ) % iTemp_Type );

	// 판매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iHostUnitUID );

	// 구매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iPriceUnitUID );

	// 판매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );				// 판매자는 회수때 처리

	// 구매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iPriceUnitEDOUT );		// 수수료가 다르기 때문에 각자 처리 해야한다.

	// 판매자 아이템
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_kSellItemDBUpdate.m_iItemUID );	// itemuid - 대리상점 itemuid
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_kSellItemDBUpdate.m_kItemInfo.m_iQuantity );	// quantity
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % 0 );												// price
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % 0 );												// category

	// 구매자 아이템 - 구매자는 아이템이 들어오기만 하기 때문에 없다.
	int index = 1;			// 물건 하나를 등록 했으므로 1로 초기화한다.
	for( ; index < SEnum::PAE_SELL_ITEM_LIMIT_NUM ; ++index )
	{
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );
	}

	std::wstring wstrTemp;
	wstrQuerySP += x2::str( x2::wformat( L", N\'%s\'" ) % wstrTemp );

	DO_QUERY_NO_ARG( wstrQuerySP.c_str() );

	// 아이템은 9개 이므로 9개로 고정한다.

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket_.m_iOK
            >> iItemUID[0]
            >> iItemUID[1]
            >> iItemUID[2]
            >> iItemUID[3]
            >> iItemUID[4]
            >> iItemUID[5]
            >> iItemUID[6]
            >> iItemUID[7]
            >> iItemUID[8]
            >> iItemUID[9]
            >> iItemUID[10]
            >> iItemUID[11]
            >> iItemUID[12]
            >> iItemUID[13]
            >> iItemUID[14]

            );

		m_kODBC.EndFetch();

		if( kPacket_.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"대리 상점 구매 SP 호출이 실패 했습니다." )
				<< BUILD_LOG( kPacket_.m_iOK )
				<< END_LOG;

			goto end_proc;
		}

		// 품목이 하나 뿐이므로
		kPacket_.m_kPriceIntemInfoIN.m_iItemUID = iItemUID[0];

		// 거래시 봉인 상태는 SP 에서 해제한다.
		if( kPacket_.m_kPriceIntemInfoIN.m_kItemInfo.IsSealedItem() == true )
		{
			kPacket_.m_kPriceIntemInfoIN.m_kItemInfo.UnsealItem();
		}
	}

end_proc:
	SendToRoom( FIRST_SENDER_UID, DBE_BUY_PSHOP_AGENCY_ITEM_ACK, kPacket_ );
}
//}}

IMPL_ON_FUNC( DBE_STOP_SALE_PSHOP_AGENCY_REQ )
{
	KDBE_STOP_SALE_PSHOP_AGENCY_ACK kPacket;
	kPacket.m_iGSUID = kPacket_.m_iGSUID;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iReason = kPacket_.m_iReason;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	// 대리상점 거래 중지 요청
	DO_QUERY( L"exec dbo.gup_update_PShop_info_On_Off", L"%d, %d",
		% kPacket_.m_iUnitUID
		% false
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"대리 상인 판매 중지 요청 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_PERSONAL_SHOP_35;
	}

end_proc:
	SendToRoom( FIRST_SENDER_UID, DBE_STOP_SALE_PSHOP_AGENCY_ACK, kPacket );
}

//{{ 2012. 05. 31	김민성       대리 상점 거래 로직 변경
IMPL_ON_FUNC( DBE_PICK_UP_FROM_PSHOP_AGENCY_REQ )
{
	UidType iItemUID[SEnum::PAE_SELL_ITEM_LIMIT_NUM] = {0,};
	KDBE_PICK_UP_FROM_PSHOP_AGENCY_ACK kPacket;
	kPacket.m_iGSUID = kPacket_.m_iGSUID;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	std::vector<KSellPersonalShopItemInfo> vecAddItem;

	// SP 쿼리를 만들어 보자
	//{{ 2013. 05. 28	최육사	아이템 개편
	std::wstring wstrQuerySP = L"exec dbo.P_GItem_Total_Pshop_New ";
	//}}
	int iTemp_Type = 4;		// 대리 상점 아이템 회수 sp 타입
	int iTemp_Zero = 0;

	wstrQuerySP += x2::str( x2::wformat( L"%d" ) % iTemp_Type );

	// 판매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iUnitUID );

	// 구매자
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );

	// 판매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kPacket_.m_iSellItemTotalED );		// 전체 판매된 ED 총합

	// 구매자 ED
	wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );

	// 등록 아이템
	for( KSellPersonalShopItemInfo& kSellItemInfo : kPacket_.m_vecPickUpItemList )
	{
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_kInventoryItemInfo.m_iItemUID );								// ItemUID
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iQuantity );				// Quantity
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % kSellItemInfo.m_iPricePerOne );												// price
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % static_cast<int>(kSellItemInfo.m_kInventoryItemInfo.m_cSlotCategory) );		// category

		vecAddItem.push_back( kSellItemInfo );
	}

	for(int i = static_cast<int>(kPacket_.m_vecPickUpItemList.size()) ; i < SEnum::PAE_SELL_ITEM_LIMIT_NUM ; ++i )
	{
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// ItemUID
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// Quantity
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// price
		wstrQuerySP += x2::str( x2::wformat( L", %d" ) % iTemp_Zero );		// category
	}

	// 상점 이름
	std::wstring wstrTemp;
	wstrQuerySP += x2::str( x2::wformat( L", N\'%s\'" ) % wstrTemp );

	DO_QUERY_NO_ARG( wstrQuerySP.c_str() );

	// 아이템은 9개 이므로 9개로 고정한다.

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
            >> iItemUID[0]
            >> iItemUID[1]
            >> iItemUID[2]
            >> iItemUID[3]
            >> iItemUID[4]
            >> iItemUID[5]
            >> iItemUID[6]
            >> iItemUID[7]
            >> iItemUID[8]
            >> iItemUID[9]
            >> iItemUID[10]
            >> iItemUID[11]
            >> iItemUID[12]
            >> iItemUID[13]
            >> iItemUID[14]
            );

		m_kODBC.EndFetch();

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"대리상점 회수 SP 호출이 실패 했습니다." )
				<< BUILD_LOG( kPacket.m_iOK )
                << BUILD_LOG( GET_LAST_QUERY() )
				<< END_LOG;

			// 실패 처리
			for( KSellPersonalShopItemInfo& kSellItemInfo : kPacket_.m_vecPickUpItemList )
			{
				kPacket.m_vecPickUpFailList.push_back( kSellItemInfo.m_kInventoryItemInfo.m_iItemUID );
			}

			goto end_proc;
		}

		int index = 0;

		for( KSellPersonalShopItemInfo& kSellItemInfo : kPacket_.m_vecPickUpItemList )
		{
			for( const KSellPersonalShopItemInfo& kItemTemp : vecAddItem )
			{
				if( kItemTemp.m_kInventoryItemInfo.m_iItemUID == kSellItemInfo.m_kInventoryItemInfo.m_iItemUID )
				{
					kSellItemInfo.m_iInventoryItemUID = iItemUID[index++];
				}
			}

			// 회수 - 팔리지 않은 아이템의 경우 봉인 정보를 유지 시켜 준다.
		}

		kPacket.m_iSellItemTotalED = kPacket_.m_iSellItemTotalED;
		kPacket.m_vecPickUpItemList = kPacket_.m_vecPickUpItemList;
	}

end_proc:

	SendToRoom( FIRST_SENDER_UID, DBE_PICK_UP_FROM_PSHOP_AGENCY_ACK, kPacket );
}
//}}

//}}

//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
IMPL_ON_FUNC( DBE_INSERT_TITLE_REQ )
{
	KDBE_INSERT_TITLE_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iTitleID = kPacket_.m_iTitleID;

	//{{ 2011. 04. 27	최육사	칭호 획득 아이템 개편
	if( kPacket_.m_bExpandPeriod )
	{
		DO_QUERY( L"exec dbo.gup_update_title", L"%d, %d, %d",
			% kPacket_.m_iUnitUID
			% kPacket_.m_iTitleID
			% kPacket_.m_sPeriod
			);
	}
	else
		//}}
	{
		DO_QUERY( L"exec dbo.gup_insert_Title", L"%d, %d, %d",
			% kPacket_.m_iUnitUID
			% kPacket_.m_iTitleID
			% kPacket_.m_sPeriod
			);
	}

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_wstrEndDate );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"칭호 업데이트가 실패했다? 일어날수 없는 에러." )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iTitleID )
			<< BUILD_LOG( kPacket_.m_sPeriod )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_TITLE_04;
	}

end_proc:
	SendToServer( DBE_INSERT_TITLE_ACK, kPacket );
}
//}}

//{{ 2012. 02. 22	박세훈	길드 이름 변경권
IMPL_ON_FUNC( DBE_CREATE_GUILD_REQ )
{
	KDBE_CREATE_GUILD_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iItemUID = kPacket_.m_iItemUID;

	// 1. 길드 창단 가능한지 체크
	DO_QUERY( L"exec dbo.gup_create_guild_check", L"%d, N\'%s\'", % kPacket_.m_iUnitUID % kPacket_.m_wstrGuildName );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( clog, L"길드 생성할 수 없는것으로 체크됨!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrGuildName );

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_00; break; // 이미 나는 길드에 가입중
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_01; break; // 길드 이름 중복
			//{{ 2009. 10. 26  최육사	길드
		case -3: kPacket.m_iOK = NetError::ERR_GUILD_42; break; // 이 길드 이름은 7일간 사용할 수 없습니다.
			//}}
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}


		goto end_proc;
	}

	// 2. 길드 생성
	DO_QUERY( L"exec dbo.gup_create_guild", L"%d, N\'%s\', %d, N\'%s\'",
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrGuildName
		% 20
		% kPacket_.m_wstrGuildMessage
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_kCreatedGuildInfo.m_iGuildUID
			>> kPacket.m_kCreatedGuildInfo.m_wstrFoundingDay );

		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 생성 실패!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrGuildName )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_04; break; // 길드 창단 에러
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_05; break; // 생성자가 길드원 가입 중 에러
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}


		goto end_proc;
	}
	else
	{
		// 창단 성공하면 초기 길드 정보 세팅
		kPacket.m_kCreatedGuildInfo.m_wstrGuildName		= kPacket_.m_wstrGuildName;
		kPacket.m_kCreatedGuildInfo.m_usMaxNumMember	= 20;
		kPacket.m_kCreatedGuildInfo.m_ucGuildLevel		= 1;
		kPacket.m_kCreatedGuildInfo.m_iGuildEXP			= 0;
		kPacket.m_kCreatedGuildInfo.m_wstrGuildMessage	= kPacket_.m_wstrGuildMessage;
	}

	//{{ 2009. 12. 3  최육사	길드스킬
	// 3. 길드 스킬 포인트
	DO_QUERY( L"exec dbo.gup_update_guild_skill_point", L"%d, %d",
		% kPacket.m_kCreatedGuildInfo.m_iGuildUID
		% 1
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 생성 직후에 스킬 포인트 1포인트 insert가 실패하였다. 있을수 없는에러!" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket.m_kCreatedGuildInfo.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06; break; // 존재하지않는 길드
		default: kPacket.m_iOK = NetError::ERR_UNKNOWN; break;
		}

		goto end_proc;
	}
	else
	{
		// 길드 스킬 포인트 초기값은 1포인트다.
		kPacket.m_kGuildSkillInfo.m_iGuildSPoint = 1;
	}

	//}}

end_proc:
	KncSend( GetPfID(), 0, PI_LOGIN_USER, LAST_SENDER_UID, anTrace_, DBE_CREATE_GUILD_ACK, kPacket );
}
IMPL_ON_FUNC( DBE_CHANGE_GUILD_NAME_CHECK_REQ )
{
	KDBE_CHANGE_GUILD_NAME_CHECK_ACK kPacket;

	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	// 길드 이름 유효성 검사
	DO_QUERY( L"exec dbo.P_GGuild_Name_CHK", L"N\'%s\'", % kPacket_.m_wstrNewGuildName );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_01; break; // 길드 이름 중복
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_42; break; // 이 길드 이름은 7일간 사용할 수 없습니다.
		default: kPacket.m_iOK = NetError::ERR_GUILD_48; break;
		}

		goto end_proc;
	}

end_proc:
	KncSend( GetPfID(), 0, PI_LOGIN_USER, LAST_SENDER_UID, anTrace_, DBE_CHANGE_GUILD_NAME_CHECK_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_CHANGE_GUILD_NAME_REQ )
{
	KDBE_CHANGE_GUILD_NAME_ACK kPacket;

	kPacket.m_iUnitUID = kPacket_.m_iUnitUID;

	// 길드 이름 변경
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_GGuild_Name_UPT", L"%d, N\'%s\'",
		% kPacket_.m_iGuildUID
		% kPacket_.m_wstrNewGuildName
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 이름 변경 실패! ( 아이템 복구 이슈 발생 )" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		switch( kPacket.m_iOK )
		{
		case -1: kPacket.m_iOK = NetError::ERR_GUILD_06;	break;
		case -2: kPacket.m_iOK = NetError::ERR_GUILD_01;	break;
		case -3: kPacket.m_iOK = NetError::ERR_GUILD_42;	break; // 이 길드 이름은 7일간 사용할 수 없습니다.
		default: kPacket.m_iOK = NetError::ERR_GUILD_48;	break;
		}
		goto end_proc;
	}

	kPacket.m_kGuildInfo.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacket.m_wstrOldGuildName = kPacket_.m_wstrOldGuildName;

	kPacket.m_iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.gup_get_guild_info", L"%d", % kPacket_.m_iGuildUID );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_kGuildInfo.m_wstrGuildName
			>> kPacket.m_kGuildInfo.m_usMaxNumMember
			>> kPacket.m_kGuildInfo.m_ucGuildLevel
			>> kPacket.m_kGuildInfo.m_iGuildEXP
			>> kPacket.m_kGuildInfo.m_wstrGuildMessage
			>> kPacket.m_kGuildInfo.m_wstrFoundingDay );

		m_kODBC.EndFetch();

		kPacket.m_wstrOldGuildName = kPacket_.m_wstrOldGuildName;
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 정보 얻기 실패" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::NET_OK;
	}

end_proc:
	KncSend( GetPfID(), 0, PI_LOGIN_USER, LAST_SENDER_UID, anTrace_, DBE_CHANGE_GUILD_NAME_ACK, kPacket );
}
//}}

//{{ 2012. 06. 05	최육사	배틀필드 시스템
_IMPL_ON_FUNC( DBE_CHECK_NICKNAME_FOR_INVITE_PARTY_REQ, KELG_INVITE_PARTY_FIND_RECEIVER_REQ )
{
	// Port: hoisted above the gotos to end_proc (MSVC let goto skip their initialization)
	UidType iUnitUID = 0;
	KELG_INVITE_PARTY_FIND_RECEIVER_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_iHostUnitUID = kPacket_.m_iHostUnitUID;

	// 닉네임 찾기!
	DO_QUERY( L"exec dbo.gup_get_unit_uid", L"N\'%s\'", % kPacket_.m_wstrReceiverNickName );
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iUnitUID );
		m_kODBC.EndFetch();
	}

	if( iUnitUID != 0 )
	{
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_03;
	}
	else
	{
		kPacket.m_iOK = NetError::ERR_SEARCH_UNIT_04;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_CHECK_NICKNAME_FOR_INVITE_PARTY_ACK, kPacket );
}
//}}

//{{ 2013. 3. 18	박세훈	 로컬 랭킹 시스템
//}}

//{{ 2013. 04. 01	 인연 시스템 - 김민성
IMPL_ON_FUNC_NOPARAM( DBE_LOAD_WEDDING_HALL_INFO_REQ )
{
	KDBE_LOAD_WEDDING_HALL_INFO_ACK kPacket;
	kPacket.m_iOK	= NetError::ERR_ODBC_01;

	DO_QUERY_NO_ARG( L"exec dbo.P_GCouple_WeddingInfo_SEL" );

	while( m_kODBC.Fetch() )
	{
		KWeddingHallInfo kWedding;
		// 결혼 완료 안된 것만 가져 오므로
		kWedding.m_bSuccess = false;
		kWedding.m_bDelete = false;
		kWedding.m_iRoomUID = 0;

		FETCH_DATA( kWedding.m_iWeddingUID
				 >> kWedding.m_iGroom
				 >> kWedding.m_iBride
				 >> kWedding.m_cWeddingHallType
				 >> kWedding.m_cOfficiantNPC
				 >> kWedding.m_wstrWeddingDate
				 >> kWedding.m_wstrWeddingMsg );

		kPacket.m_mapWeddingInfo.insert( std::make_pair(kWedding.m_iWeddingUID, kWedding ) );

		// 100 개씩 끊어서 전달 하다록 하자
		if( kPacket.m_mapWeddingInfo.size() >= 100 )
		{
			kPacket.m_iOK	= NetError::NET_OK;
			SendToServer( DBE_LOAD_WEDDING_HALL_INFO_ACK, kPacket );

			// 보냈으니 초기화
			kPacket.m_iOK	= NetError::ERR_ODBC_01;
			kPacket.m_mapWeddingInfo.clear();
		}
	}

	kPacket.m_iOK	= NetError::NET_OK;

end_proc:
	SendToServer( DBE_LOAD_WEDDING_HALL_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_WEDDING_HALL_INFO_UPDATE_NOT )
{
	bool bResult = true;

	std::map< int, KWeddingHallInfo >::iterator mit = kPacket_.m_mapWeddingInfo.begin();
	for( ; mit != kPacket_.m_mapWeddingInfo.end() ; ++mit )
	{
		int iOK	= NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_GCouple_WeddingInfo_UPD", L"%d",
				% mit->second.m_iWeddingUID );

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK	);
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			bResult = false;

			START_LOG( cerr, L"결혼 예약 데이터 update 실패!!" )
				<< BUILD_LOG( mit->second.m_iWeddingUID )
				<< BUILD_LOG( mit->second.m_iGroom )
				<< BUILD_LOG( mit->second.m_iBride )
				<< BUILD_LOGc( mit->second.m_cWeddingHallType )
				<< BUILD_LOGc( mit->second.m_cOfficiantNPC )
				<< BUILD_LOG( mit->second.m_wstrWeddingDate )
				<< END_LOG;
		}
	}

end_proc:
	START_LOG( cout, L"결혼 예약 데이터 update 결과 : " )
		<< BUILD_LOG( bResult )
		<< END_LOG;
}

_IMPL_ON_FUNC( DBE_RELATIONSHIP_INFO_GAME_DB_REQ, KELG_RELATIONSHIP_INFO_REQ )
{
	KEGS_RELATIONSHIP_INFO_ACK kAck;
	kAck.m_iOK = NetError::ERR_ODBC_01;
	kAck.m_iRequestGSUID = kPacket_.m_iRequestGSUID;
	kAck.m_iRequestUnitUID = kPacket_.m_iRequestUnitUID;
	kAck.m_iAcceptUnitUID = kPacket_.m_iAcceptUnitUID;

	DO_QUERY( L"exec dbo.P_GCouple_Info_SEL_PartnerInfo", L"%d", % kPacket_.m_iAcceptUnitUID );
	if( m_kODBC.BeginFetch() )
	{
		bool bDummy = false;
		int iDummyINT = 0;
		UidType iDummyUID = 0;
		std::wstring wstrLastLogOutTime;

		FETCH_DATA( kAck.m_iOK
			>> kAck.m_ucLevel
			>> kAck.m_cUnitClass
			>> wstrLastLogOutTime
			);

		kAck.m_bIsConnect = false;
		kAck.m_iMapID = 0;
		kAck.m_wstrChannelName = L"미접속";

		CTime tLogOutTime;
		KncUtil::ConvertStringToCTime( wstrLastLogOutTime, tLogOutTime );
		kAck.m_tLastLogOutTime = tLogOutTime.GetTime();

		m_kODBC.EndFetch();
	}

	if( kAck.m_iOK == NetError::NET_OK )
	{
		kAck.m_iOK = NetError::ERR_ODBC_01;

		// 보유 아이템을 얻는다.
		DO_QUERY( L"exec dbo.P_GItem_Equipped_Item_SEL", L"%d", % kPacket_.m_iAcceptUnitUID );
		while( m_kODBC.Fetch() )
		{
			const byte byteArraySize = 5;
			int arrSocketOption[byteArraySize];
			memset( arrSocketOption, 0, sizeof(int) * byteArraySize );

			int iEnchantLevel = 0;
			KInventoryItemInfo kInventoryItemInfo;

			FETCH_DATA( kInventoryItemInfo.m_iItemUID
				>> kInventoryItemInfo.m_kItemInfo.m_iItemID
				>> kInventoryItemInfo.m_kItemInfo.m_cUsageType
				>> kInventoryItemInfo.m_kItemInfo.m_iQuantity
				>> kInventoryItemInfo.m_kItemInfo.m_sEndurance
				>> kInventoryItemInfo.m_kItemInfo.m_sPeriod
				>> kInventoryItemInfo.m_kItemInfo.m_wstrExpirationDate
				>> iEnchantLevel
				>> arrSocketOption[0]
				>> arrSocketOption[1]
				>> arrSocketOption[2]
				>> arrSocketOption[3]
				>> arrSocketOption[4]
				>> kInventoryItemInfo.m_kItemInfo.m_byteExpandedSocketNum
				>> kInventoryItemInfo.m_cSlotCategory
				>> kInventoryItemInfo.m_sSlotID
				);


				//{{ 2008. 2. 20  최육사  강화
				kInventoryItemInfo.m_kItemInfo.m_cEnchantLevel = static_cast<char>(iEnchantLevel);
				//}}

				//{{ 2008. 3. 7  최육사  소켓
				int iCheckIdx = byteArraySize;
				while( 0 <= --iCheckIdx )
				{
					if( arrSocketOption[iCheckIdx] != 0 )
						break;
				}

				for( int iIdx = 0; iIdx <= iCheckIdx; ++iIdx )
				{
					kInventoryItemInfo.m_kItemInfo.m_vecItemSocket.push_back( arrSocketOption[iIdx] );
				}
				//}}

				std::map< int, KInventoryItemInfo >::iterator mit;
				mit = kAck.m_mapOtherUnitEquippedItem.find( kInventoryItemInfo.m_sSlotID );

				if( mit != kAck.m_mapOtherUnitEquippedItem.end() )
				{
					START_LOG( cerr, L"장착 아이템 정보가 중복됨." )
						<< BUILD_LOG( kInventoryItemInfo.m_iItemUID )
						<< BUILD_LOG( static_cast<int>(kInventoryItemInfo.m_sSlotID) )
						<< BUILD_LOG( kInventoryItemInfo.m_kItemInfo.m_iItemID )
						<< END_LOG;
				}
				kAck.m_mapOtherUnitEquippedItem.insert( std::make_pair( kInventoryItemInfo.m_sSlotID, kInventoryItemInfo ) );
		}
	}

	kAck.m_iOK = NetError::NET_OK;

	START_LOG( cout, L"연인의 장착장비 정보 얻기 확인!" )
		<< END_LOG;

end_proc:

	if( kAck.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"연인의 장착장비 정보 얻기 실패!" )
			<< BUILD_LOG( kAck.m_iOK )
			<< END_LOG;
	}

	SendToUser( LAST_SENDER_UID, DBE_RELATIONSHIP_INFO_GAME_DB_ACK, kAck );
};

_IMPL_ON_FUNC( DBE_WEDDING_ITEM_FIND_INFO_REQ, KELG_WEDDING_ITEM_FIND_INFO_REQ )
{
	kPacket_.m_iOK = NetError::NET_OK;

	std::map< UidType, KWeddingItemInfo >::iterator mit = kPacket_.m_mapWeddingItemInfo.begin();
	for( ; mit != kPacket_.m_mapWeddingItemInfo.end() ; ++mit )
	{
		// 신랑 이름이 없을 경우
		if( mit->second.m_wstrGroom.empty() == true && mit->second.m_iGroom > 0 )
		{
			int iOK = 0;
			DO_QUERY( L"exec dbo.P_GUnitNickName_SEL_UnitUID", L"%d", % mit->second.m_iGroom );
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK
						 >> mit->second.m_wstrGroom );

				m_kODBC.EndFetch();
			}

			if( iOK != 0 )
			{
				kPacket_.m_iOK = NetError::ERR_RELATIONSHIP_26;
				break;
			}
		}

		// 신부 이름이 없을 경우
		if( mit->second.m_wstrBride.empty() == true && mit->second.m_iBride > 0 )
		{
			int iOK = 0;
			DO_QUERY( L"exec dbo.P_GUnitNickName_SEL_UnitUID", L"%d", % mit->second.m_iBride );
			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK
					>> mit->second.m_wstrBride );

				m_kODBC.EndFetch();
			}
			if( iOK != 0 )
			{
				kPacket_.m_iOK = NetError::ERR_RELATIONSHIP_26;
				break;
			}
		}
	}

end_proc:

	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"결혼식 예약자 닉네임 얻기 실패!" )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;
	}

	SendToUser( LAST_SENDER_UID, DBE_WEDDING_ITEM_FIND_INFO_ACK, kPacket_ );
}
//}

void KLoginGameDBThread::GetPShopItem( IN const UidType& iUnitUID_, OUT std::vector< KSellPersonalShopItemInfo >& vecSellItemInfo_ )
{
    DO_QUERY( L"exec dbo.P_GPShopItem_SEL", L"%d", % iUnitUID_ );
    while( m_kODBC.Fetch() )
    {
        KSellPShopItemBackupData kItemInfo;
        kItemInfo.m_kSellPShopItemInfo.m_cPShopItemType = KSellPersonalShopItemInfo::SPIT_PSHOP_AGENCY;

        //{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
		const byte byteArraySize = 5;
		int arrSocketOption[byteArraySize];
		memset( arrSocketOption, 0, sizeof(int) * byteArraySize );
        //}}

        int arrRandomSocketOption[5] = {0};

		FETCH_DATA( kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_iItemUID
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iItemID
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_cUsageType
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_iQuantity
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_sEndurance
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_ucSealData
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_cEnchantLevel
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant0
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant1
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_kAttribEnchantInfo.m_cAttribEnchant2
			>> arrSocketOption[0]
			>> arrSocketOption[1]
			>> arrSocketOption[2]
			>> arrSocketOption[3]
			>> arrSocketOption[4]
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_byteExpandedSocketNum
			>> arrRandomSocketOption[0]
			>> arrRandomSocketOption[1]
			>> arrRandomSocketOption[2]
			>> arrRandomSocketOption[3]
			>> arrRandomSocketOption[4]
			>> kItemInfo.m_kSellPShopItemInfo.m_iPricePerOne
			>> kItemInfo.m_kSellPShopItemInfo.m_iFeePerOne
			>> kItemInfo.m_kSellPShopItemInfo.m_iTotalSellEDIn
			>> kItemInfo.m_iTotalSoldItemQuantity
			>> kItemInfo.m_iTotalSellCommissionED
			>> kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_cSlotCategory
			);

        int iCheckRandomIdx;
        for( iCheckRandomIdx = 4; iCheckRandomIdx >= 0; --iCheckRandomIdx )
        {
            if( arrRandomSocketOption[iCheckRandomIdx] != 0 )
                break;
        }

        for( int iIdx = 0; iIdx <= iCheckRandomIdx; ++iIdx )
        {
            kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_vecRandomSocket.push_back( arrRandomSocketOption[iIdx] );
        }


		int iCheckIdx = byteArraySize;
		while( 0 <= --iCheckIdx )
		{
			if( arrSocketOption[iCheckIdx] != 0 )
				break;
		}

        for( int iIdx = 0; iIdx <= iCheckIdx; ++iIdx )
        {
            kItemInfo.m_kSellPShopItemInfo.m_kInventoryItemInfo.m_kItemInfo.m_vecItemSocket.push_back( arrSocketOption[iIdx] );
        }

        vecSellItemInfo_.push_back( kItemInfo.m_kSellPShopItemInfo );
    }

end_proc:
    ;

}
