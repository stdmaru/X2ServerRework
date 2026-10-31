#include "./Guild.h"
#include "NetError.h"
#include "Enum/Enum.h"
//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
	#include "LoginServer.h"
//}}
//{{ 2009. 10. 28  최육사	길드레벨
	#include "X2Data/XSLGuildManager.h"
//}}
//{{ 2010. 01. 19  최육사	길드게시판
	#include "GuildBoardManager.h"
//}}


KGuild::KGuild( const KGuildInfo& kGuildInfo ) :
m_iGuildMasterUnitUID( 0 ),
m_iLoginedMemberCount( 0 )
{
	m_kGuildInfo = kGuildInfo;

	// 길드 레벨업 체크
	CheckGuildLevelUp();
}

KGuild::~KGuild(void)
{
}

//{{ 2009. 11. 25  최육사	길드스킬

void KGuild::Tick()
{
	// 1분마다 캐시스킬 포인트에 대한 기간 체크
	if( m_kTimer[TM_CHECK_GUILD_CASH_SKILL_POINT].elapsed() < 60.0 )
		return;

	m_kTimer[TM_CHECK_GUILD_CASH_SKILL_POINT].restart();

	if( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		CTime tCurrentTime = CTime::GetCurrentTime();
		if( tCurrentTime > m_kSkillTree.GetGuildCSPointEndDate() )
		{
			// 배운 스킬 트리중에서 CSP로 배운 스킬 되돌리고, 사용된 CSP 포인트 계산하고 m_iMaxCSPoint를 빼서 SP로 돌려주고
			// 장착된 스킬중에 안배운 스킬이 있으면 탈착한다
			KDBE_EXPIRE_GUILD_CASH_SKILL_POINT_REQ kPacketToDB;
			kPacketToDB.m_iGuildUID = GetUID();
			m_kSkillTree.CalcExpireCashSkillPoint( kPacketToDB.m_iGuildSPoint, kPacketToDB.m_iRetrievedSPoint, kPacketToDB.m_vecGuildSkillData );
			SendToGameDB( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_REQ, kPacketToDB );
		}
	}
}

//}}

//{{ 2009. 11. 25  최육사	길드스킬

void KGuild::GetGuildInfo( OUT KGuildInfo& kGuildInfo, OUT std::wstring& wstrGuildMasterName, OUT std::vector< UidType >& vecMemberUnitUID, OUT KGuildSkillInfo& kGuildSkillInfo ) const
{
	vecMemberUnitUID.clear();

	// 길드 정보
	GetGuildInfo( kGuildInfo );

	// 길드원 리스트
	std::vector< KGuildMemberInfoPtr >::const_iterator vit;
	for( vit = m_vecGuildMember.begin(); vit != m_vecGuildMember.end(); ++vit )
	{
		if( (*vit) == NULL )
			continue;

		// 길드 마스터 닉네임
		if( m_iGuildMasterUnitUID == (*vit)->m_iUnitUID )
		{
			wstrGuildMasterName = (*vit)->m_wstrNickName;
		}

		vecMemberUnitUID.push_back( (*vit)->m_iUnitUID );
	}

	// 길드 스킬 정보 얻기
	m_kSkillTree.GetGuildSkillInfo( kGuildSkillInfo );
}

//}}

KGuildMemberInfoPtr KGuild::GetGuildMember( UidType iUnitUID ) const
{
	std::map< UidType, KGuildMemberInfoPtr >::const_iterator mit = m_mapGuildMember.find( iUnitUID );
	if( mit == m_mapGuildMember.end() )
	{
		START_LOG( cwarn, L"찾는 길드 멤버가 없습니다.!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return KGuildMemberInfoPtr();
	}

	if( mit->second == NULL )
	{
		START_LOG( cerr, L"길드 멤버 포인터 이상.!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return KGuildMemberInfoPtr();
	}

	return mit->second;
}

bool KGuild::IsLoginedMember( IN UidType iUnitUID )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드원 정보 얻기 실패. 길드원 로그인 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	// 로그아웃 시각이 0이라면 로그인된 상태이다.
	return spGuildMember->IsLogined();
}

int KGuild::LoginedMemberCounting() const
{
	int iLoginedMemberCount = 0;

	// 길드원 리스트
	std::vector< KGuildMemberInfoPtr >::const_iterator vit;
	for( vit = m_vecGuildMember.begin(); vit != m_vecGuildMember.end(); ++vit )
	{
		if( (*vit) == NULL )
			continue;

		if( (*vit)->IsLogined() )
		{
			++iLoginedMemberCount;
		}
	}

    return iLoginedMemberCount;
}

void KGuild::UpdateGuildMemberList( IN const std::vector< KGuildMemberInfo >& vecMemberInfo )
{
	std::vector< KGuildMemberInfo >::const_iterator vit;
	for( vit = vecMemberInfo.begin(); vit != vecMemberInfo.end(); ++vit )
	{
		InsertGuildMember( *vit );
	}

	// 길드 멤버 리스트가 업데이트 되었으면 정렬한번 합시다!
	GuildMemberSort();

	// 길드마스터 정보도 업데이트 잘 되었는지 확인
	if( m_iGuildMasterUnitUID == 0 )
	{
		START_LOG( cerr, L"길드원 리스트를 업데이트 했는데 길드마스터가 없다? 절대 일어나서는 안되는 에러!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( vecMemberInfo.size() )
			<< END_LOG;
	}
}

//{{ 2009. 11. 25  최육사	길드스킬
void KGuild::UpdateGuildSkill( IN const KGuildSkillInfo& kInfo )
{
    m_kSkillTree.UpdateGuildSkillTree( kInfo );
}
//}}

bool KGuild::InsertGuildMember( IN const KGuildMemberInfo& kMemberInfo )
{
	if( IsGuildMember( kMemberInfo.m_iUnitUID ) == true )
	{
		START_LOG( cerr, L"이미 길드원 정보가 있다! 일어나서는 안되는 에러!" )
			<< BUILD_LOG( kMemberInfo.m_iUnitUID )
			<< END_LOG;

		return false;
	}

	// 길드원 추가
	KGuildMemberInfoPtr spGuildMember( new KGuildMemberInfo( kMemberInfo ) );

	m_mapGuildMember.insert( std::make_pair( kMemberInfo.m_iUnitUID, spGuildMember ) );

	// 길드 마스터 여부 체크
	if( kMemberInfo.m_ucMemberShipGrade == SEnum::GUG_MASTER )
	{
		if( m_iGuildMasterUnitUID != 0  &&
			m_iGuildMasterUnitUID != kMemberInfo.m_iUnitUID )
		{
			START_LOG( cerr, L"내가 길드마스터인데 길드 마스터가 또 있네?" )
				<< BUILD_LOG( m_iGuildMasterUnitUID )
				<< BUILD_LOG( kMemberInfo.m_iUnitUID )
				<< END_LOG;
		}

		m_iGuildMasterUnitUID = kMemberInfo.m_iUnitUID;
	}

	// 길드원 리스트 추가
	m_vecGuildMember.push_back( spGuildMember );
	return true;
}

bool KGuild::DeleteGuildMember( IN UidType iUnitUID )
{
	std::map< UidType, KGuildMemberInfoPtr >::iterator mit = m_mapGuildMember.find( iUnitUID );
	if( mit == m_mapGuildMember.end() )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 삭제 실패!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	if( mit->second == NULL )
	{
		START_LOG( cerr, L"길드원 포인터가 이상합니다. 절대 발생해서는 안되는 에러!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		// return하지 않습니다. vector 컨테이너것도 지워야 하거든요!
	}

	// map컨테이너에서 길드원 정보 삭제
	m_mapGuildMember.erase( mit );

	// vector컨테이너에서 길드원 정보 삭제
	std::vector< KGuildMemberInfoPtr >::iterator vit;
	for( vit = m_vecGuildMember.begin(); vit != m_vecGuildMember.end(); ++vit )
	{
		if( (*vit) == NULL )
			continue;

		if( (*vit)->m_iUnitUID == iUnitUID )
			break;
	}

	if( vit != m_vecGuildMember.end() )
	{
		m_vecGuildMember.erase( vit );
	}


	return true;
}

bool KGuild::CheckJoinGuild( IN UidType iUnitUID, OUT UidType& iGuildMasterUnitUID )
{
	// 이미 길드멤버인지 검사
	if( IsGuildMember( iUnitUID ) == true )
	{
		START_LOG( cerr, L"이미 길드원 정보가 있다! 일어나서는 안되는 에러!" )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_14 );
		return false;
	}

	// 가입 가능한 인원인지 체크
	if( GetMaxMember() <= ( GetNumMember() + GetWaitingJoinNumMember() ) )
	{
        SET_ERROR( ERR_GUILD_18 );
		return false;
	}

	// 길드 마스터 UnitUID를 얻자!
	iGuildMasterUnitUID = m_iGuildMasterUnitUID;

	// 예약 대기자 리스트에 넣자!
    m_setWaitingForJoinUserList.insert( iUnitUID );

	SET_ERROR( NET_OK );
	return true;
}

void KGuild::RemoveReserve( IN UidType iUnitUID )
{
    m_setWaitingForJoinUserList.erase( iUnitUID );
}

bool KGuild::JoinGuildMember( IN const KGuildMemberInfo& kMemberInfo, IN bool bIsLoginUser )
{
	//{{ 2010. 05. 18  최육사	대전 던전 서버군 통합
	if( KncUid::ExtractServerGroupID( kMemberInfo.m_iUnitUID ) != GetKLoginServer()->GetServerGroupID() )
	{
		START_LOG( cerr, L"다른 서버군의 유저는 길드가입 시킬수 없다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kMemberInfo.m_iUnitUID )
			<< BUILD_LOG( GetKLoginServer()->GetServerGroupID() )
			<< END_LOG;

		return false;
	}
	//}}

	// 길드 멤버 정보 생성
	if( InsertGuildMember( kMemberInfo ) == false )
	{
		START_LOG( cerr, L"길드원 정보 생성 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kMemberInfo.m_iUnitUID )
			<< END_LOG;

		return false;
	}

	// 로그인 타임 설정
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kMemberInfo.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드원 정보 얻기 실패. 절대 일어나서는 안되는 에러!" )
			<< BUILD_LOG( kMemberInfo.m_iUnitUID )
			<< BUILD_LOG( GetUID() )
			<< END_LOG;

		return false;
	}

	// 로그인한 유저라면 로그인 설정을 하자!
	if( bIsLoginUser )
	{
		spGuildMember->m_tLogInTime	= CTime::GetCurrentTime().GetTime();
		spGuildMember->m_tLogOutTime = 0;
	}
	else
	{
		spGuildMember->m_tLogInTime	= 0;
		spGuildMember->m_tLogOutTime = CTime::GetCurrentTime().GetTime();
	}

	// 새로운 길드원이 가입했으면 등급순으로 정렬한번 합시다!
	GuildMemberSort();

	// 로그인 멤버 카운트 세기
	m_iLoginedMemberCount = LoginedMemberCounting();

	// 길드원 가입인 경우 다른 길드원에게 가입 사실을 알리자!
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType		= KEGS_GUILD_MESSAGE_NOT::MT_JOIN_MEMBER;
		kPacket.m_iUnitUID			= kMemberInfo.m_iUnitUID;
		kPacket.m_wstrNickName		= kMemberInfo.m_wstrNickName;
		kPacket.m_iMessageData		= static_cast<int>(kMemberInfo.m_ucMemberShipGrade);
		BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	}

	// 길드 메시지를 로그인한 유저에게 날리자!
	if( m_kGuildInfo.m_wstrGuildMessage.empty() == false  &&  bIsLoginUser == true )
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType		= KEGS_GUILD_MESSAGE_NOT::MT_GUILD_MESSAGE;
		kPacket.m_wstrMessage		= m_kGuildInfo.m_wstrGuildMessage;
		SendToGSCharacter( kMemberInfo.m_iGSUID, kMemberInfo.m_iUnitUID, ELG_GUILD_MESSAGE_NOT, kPacket );
	}

	return true;
}

bool KGuild::KickGuildMemeber( IN UidType iUnitUID, IN UidType iTargetUnitUID )
{
	//// 자신의 길드 정보 찾기.
	//KGuildMemberInfoPtr spSelfInfo = GetGuildMember( iUnitUID );
	//if( spSelfInfo == NULL )
	//{
	//	START_LOG( cerr, L"자신의 길드정보 찾기 실패.!" )
	//		<< BUILD_LOG( GetUID() )
	//		<< BUILD_LOG( iUnitUID )
	//		<< END_LOG;

	//	return false;
	//}

	// 탈퇴하는 유저 정보 찾기
	KGuildMemberInfoPtr spTargetInfo = GetGuildMember( iTargetUnitUID );
	if( spTargetInfo == NULL )
	{
		START_LOG( cerr, L"길드원 정보 얻기 실패." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iTargetUnitUID )
			<< END_LOG;

		return false;
	}

	//switch( spSelfInfo->m_ucMemberShipGrade )
	//{
	//case SEnum::GUG_MASTER:
	//case SEnum::GUG_SYSOP:
	//	{
	//		if( spSelfInfo->m_ucMemberShipGrade >= spTargetInfo->m_ucMemberShipGrade )
	//			return false;
	//	}
	//	break;

	//default:
	//	{
	//		if( iUnitUID != iTargetUnitUID )
	//			return false;
	//	}
	//	break;
	//}

	// 탈퇴자에게도 메시지를 주기위해 메시지부터 날린다!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= ( iUnitUID == iTargetUnitUID ? KEGS_GUILD_MESSAGE_NOT::MT_KICK_MEMBER : KEGS_GUILD_MESSAGE_NOT::MT_KICK_BY_MASTER );
	kPacket.m_iUnitUID		= iTargetUnitUID;
	kPacket.m_wstrNickName  = spTargetInfo->m_wstrNickName;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );

	// 길드 멤버 정보 삭제
	if( DeleteGuildMember( iTargetUnitUID ) == false )
	{
		START_LOG( cerr, L"길드 멤버 정보 삭제 실패! 일어날수 없는 에러!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iTargetUnitUID )
			<< END_LOG;

		return false;
	}

	// 로그인 멤버 카운트 세기
	m_iLoginedMemberCount = LoginedMemberCounting();
	return true;
}

bool KGuild::LoginGuildMember( IN const KGuildMemberInfo& kMemberInfo )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kMemberInfo.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드원 정보 얻기 실패. 길드원 로그인 실패!" )
			<< BUILD_LOG( kMemberInfo.m_iUnitUID )
			<< BUILD_LOG( GetUID() )
			<< END_LOG;

		return false;
	}

	// 길드 마스터
	if( kMemberInfo.m_ucMemberShipGrade == SEnum::GUG_MASTER )
	{
		m_iGuildMasterUnitUID = kMemberInfo.m_iUnitUID;
	}

	// 유닛 정보 업데이트
	spGuildMember->m_iGSUID				= kMemberInfo.m_iGSUID;
	//spGuildMember->m_iUnitUID;								// 변경하지 않습니다.
	spGuildMember->m_wstrNickName		= kMemberInfo.m_wstrNickName;
	spGuildMember->m_cUnitClass			= kMemberInfo.m_cUnitClass;
	spGuildMember->m_ucLevel			= kMemberInfo.m_ucLevel;
	spGuildMember->m_ucMemberShipGrade	= kMemberInfo.m_ucMemberShipGrade;
	spGuildMember->m_iChannelID			= kMemberInfo.m_iChannelID;
	spGuildMember->m_iMapID				= kMemberInfo.m_iMapID;
	spGuildMember->m_cPosition			= kMemberInfo.m_cPosition;
	spGuildMember->m_tLogInTime			= CTime::GetCurrentTime().GetTime();
	spGuildMember->m_tLogOutTime		= 0;
	//spGuildMember->m_wstrMessage;								// 변경하지 않습니다.

	// 길드원이 로그인 했으면 정렬한번 합시다!
	GuildMemberSort();

	// 로그인 멤버 카운트 세기
	m_iLoginedMemberCount = LoginedMemberCounting();

	// 길드원 로그인 사실을 다른 길드원들에게 알리자!
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType		= KEGS_GUILD_MESSAGE_NOT::MT_CONNECT_MEMBER;
		kPacket.m_iUnitUID			= kMemberInfo.m_iUnitUID;
		kPacket.m_wstrNickName		= kMemberInfo.m_wstrNickName;
		kPacket.m_iMessageData		= static_cast<int>(kMemberInfo.m_ucMemberShipGrade);
		BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	}

	// 길드 메시지도 날리자!
	if( m_kGuildInfo.m_wstrGuildMessage.empty() == false )
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType		= KEGS_GUILD_MESSAGE_NOT::MT_GUILD_MESSAGE;
		kPacket.m_wstrMessage		= m_kGuildInfo.m_wstrGuildMessage;
		SendToGSCharacter( kMemberInfo.m_iGSUID, kMemberInfo.m_iUnitUID, ELG_GUILD_MESSAGE_NOT, kPacket );
	}

	// 인사말이 비어있지 않으면 채팅으로 인사말을 날리자
	if( spGuildMember->m_wstrMessage.empty() == false )
	{
		KELG_CHAT_NOT kChatMessage;
		kChatMessage.m_cChatPacketType			= KEGS_CHAT_REQ::CPT_GUILD;
		kChatMessage.m_iSenderUnitUID			= spGuildMember->m_iUnitUID;
		kChatMessage.m_wstrSenderUnitNickName	= spGuildMember->m_wstrNickName;
		kChatMessage.m_iReceiverUnitUID			= 0;
		kChatMessage.m_wstrRecieverUnitNickName = L"";
		kChatMessage.m_wstrMsg					= spGuildMember->m_wstrMessage;
		BroadCast( ELG_CHAT_NOT, kChatMessage );
	}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2010. 01. 19  최육사	길드게시판

	if( kMemberInfo.m_ucMemberShipGrade == SEnum::GUG_MASTER  ||
		kMemberInfo.m_ucMemberShipGrade == SEnum::GUG_SYSOP )
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType = KEGS_GUILD_MESSAGE_NOT::MT_GUILD_AD_PERIOD_INFO;
        if( SiKGuildBoardManager()->GetGuildAdEndDate( GetUID(), kPacket.m_wstrMessage ) == true )
		{
			// 기간 만료가 안되어 있어야함.
			if( SiKGuildBoardManager()->IsExpiredGuildAd( GetUID() ) == false )
			{
				SendToGSCharacter( kMemberInfo.m_iGSUID, kMemberInfo.m_iUnitUID, ELG_GUILD_MESSAGE_NOT, kPacket );
			}
		}
	}

	//}}
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool KGuild::LogoutGuildMember( IN UidType iUnitUID )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드원 정보 얻기 실패. 길드원 로그아웃 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	// 유닛 정보 업데이트
	spGuildMember->m_iGSUID		 = 0;
	spGuildMember->m_iChannelID	 = 0;
	spGuildMember->m_cPosition	 = 0;
	spGuildMember->m_tLogInTime	 = 0;
	spGuildMember->m_tLogOutTime = CTime::GetCurrentTime().GetTime();

	// 길드원이 로그아웃 했으면 정렬한번 합시다!
	GuildMemberSort();

	// 로그인 멤버 카운트 감소
	m_iLoginedMemberCount = LoginedMemberCounting();

	// 길드원 로그아웃 사실을 다른 길드원들에게 알리자!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_DISCONNECT_MEMBER;
	kPacket.m_iUnitUID		= iUnitUID;
	kPacket.m_wstrNickName  = spGuildMember->m_wstrNickName;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	return true;
}

bool KGuild::CheckInviteCondition( IN UidType iReceiverUnitUID, IN UidType iSenderUnitUID, OUT KELG_INVITE_GUILD_FIND_USER_REQ& kPacket ) const
{
	// 초대장을 날리려는 길드원이 해당 길드원이 맞는지 찾자
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( iSenderUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닌데 길드초대를 했다." )
			<< BUILD_LOG( iSenderUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_07 );
		return false;
	}

	// 길드 초대가 가능한 등급인지 검사
	switch( spGuildMember->m_ucMemberShipGrade )
	{
	case SEnum::GUG_MASTER:
	case SEnum::GUG_SYSOP:
	case SEnum::GUG_OLD_USER:
	case SEnum::GUG_VIP_USER:
		break;

	default:
		{
			START_LOG( cerr, L"길드 초대 권한이 없는데 길드초대를 했다." )
				<< BUILD_LOG( iSenderUnitUID )
				<< END_LOG;

			SET_ERROR( ERR_GUILD_12 );
			return false;
		}
	}

	// 더이상 길드 제한 인원을 늘릴수 없는가?
	if( GetNumMember() >= KGuildInfo::GUILD_MAX_MEMBER_LIMIT )
	{
		START_LOG( clog, L"이미 길드 인원 제한인데 더 확장하려고함" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iSenderUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_41 );
		return false;
	}

	// 길드 인원이 가득 찼는지 알아보자!
	if( GetMaxMember() <= GetNumMember() )
	{
		// 시간차로 발생 가능한 에러임.
		START_LOG( clog, L"길드원을 더 추가할수 없음." )
			<< BUILD_LOG( iSenderUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_13 );
		return false;
	}

	// 이미 길드에 가입된 유저인지 확인해보자
	if( IsGuildMember( iReceiverUnitUID ) == true )
	{
		// 시간차로 인해 발생할 수 있는 오류임.
		START_LOG( cwarn, L"이미 길드원인 유저를 길드초대를 했다." )
			<< BUILD_LOG( iReceiverUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_14 );
		return false;
	}

	// 초대될 유저에게 보낼 정보
	kPacket.m_kGuildInfo = m_kGuildInfo;
	kPacket.m_wstrSenderNickName = spGuildMember->m_wstrNickName;
	return true;
}

void KGuild::GetGuildUserList( IN const KELG_GET_GUILD_USER_LIST_REQ& kReq, OUT KEGS_GET_GUILD_USER_LIST_ACK& kAck ) const
{
	kAck.m_iOK = NetError::NET_OK;
	kAck.m_uiViewPage = kReq.m_uiViewPage;
	kAck.m_cGuildUserListType = kReq.m_cGuildUserListType;
	kAck.m_iLoginedMemberCount = GetLoginedMemberCount();

	switch( kReq.m_cGuildUserListType )
	{
	case KEGS_GET_GUILD_USER_LIST_REQ::ULT_POSITION:
	case KEGS_GET_GUILD_USER_LIST_REQ::ULT_GRADE:
		{
            GetGuildMemberPositionGrade( kAck.m_uiViewPage, kAck.m_uiTotalPage, kAck.m_vecMemberPositionGradeList );
		}
		break;

	case KEGS_GET_GUILD_USER_LIST_REQ::ULT_MESSAGE:
		{
            GetGuildMemberMessageInfo( kAck.m_uiViewPage, kAck.m_uiTotalPage, kAck.m_vecMemberMessageList );
		}
		break;

	default:
		{
			START_LOG( cerr, L"알수없는 길드유저리스트타입입니다." )
				<< BUILD_LOGc( kReq.m_cGuildUserListType )
				<< END_LOG;
		}
		break;
	}
}

void KGuild::GetGuildMemberPositionGrade( IN OUT UINT& uiViewPage, OUT UINT& uiTotalPage, OUT std::vector< KGuildMemberInfo >& vecMemberList ) const
{
	vecMemberList.clear();

	_JIF( uiViewPage > 0, return; );

	if( m_vecGuildMember.empty() )
	{
		uiViewPage = 1;
		uiTotalPage = 1;
		return;
	}

	uiTotalPage = m_vecGuildMember.size() / MEMBER_PAGE_PER_COUNT;
	if( ( m_vecGuildMember.size() % MEMBER_PAGE_PER_COUNT ) > 0 )
	{
		++uiTotalPage;
	}

	u_int uiBeginIndex = ( uiViewPage * MEMBER_PAGE_PER_COUNT ) - MEMBER_PAGE_PER_COUNT;
	if( m_vecGuildMember.size() <= uiBeginIndex )
	{
		uiViewPage = uiTotalPage; // 마지막 페이지
		uiBeginIndex = ( uiViewPage * MEMBER_PAGE_PER_COUNT ) - MEMBER_PAGE_PER_COUNT;

		if( m_vecGuildMember.size() <= uiBeginIndex )
		{
			START_LOG( cerr, L"일어날 수 없는 에러!" )
				<< BUILD_LOG( uiViewPage )
				<< BUILD_LOG( uiTotalPage )
				<< BUILD_LOG( uiBeginIndex )
				<< BUILD_LOG( m_vecGuildMember.size() )
				<< END_LOG;
			return;
		}
	}

	std::vector< KGuildMemberInfoPtr >::const_iterator vit = m_vecGuildMember.begin();
	vit += uiBeginIndex;

	while( vit != m_vecGuildMember.end() )
	{
		if( (*vit) == NULL )
		{
			START_LOG( cerr, L"길드 멤버 포인터가 이상합니다." )
				<< END_LOG;
			break;
		}

		vecMemberList.push_back( (*(*vit)) );

		if( vecMemberList.size() >= MEMBER_PAGE_PER_COUNT )
			break;

		++vit;
	}
}

void KGuild::GetGuildMemberMessageInfo( IN OUT UINT& uiViewPage, OUT UINT& uiTotalPage, OUT std::vector< KGuildMemberMessageInfo >& vecMemberList ) const
{
	vecMemberList.clear();

	_JIF( uiViewPage > 0, return; );

	if( m_vecGuildMember.empty() )
	{
		uiViewPage = 1;
		uiTotalPage = 1;
		return;
	}

	uiTotalPage = m_vecGuildMember.size() / MEMBER_PAGE_PER_COUNT;
	if( ( m_vecGuildMember.size() % MEMBER_PAGE_PER_COUNT ) > 0 )
	{
		++uiTotalPage;
	}

	u_int uiBeginIndex = ( uiViewPage * MEMBER_PAGE_PER_COUNT ) - MEMBER_PAGE_PER_COUNT;
	if( m_vecGuildMember.size() <= uiBeginIndex )
	{
		uiViewPage = uiTotalPage; // 마지막 페이지
		uiBeginIndex = ( uiViewPage * MEMBER_PAGE_PER_COUNT ) - MEMBER_PAGE_PER_COUNT;

		if( m_vecGuildMember.size() <= uiBeginIndex )
		{
			START_LOG( cerr, L"일어날 수 없는 에러!" )
				<< BUILD_LOG( uiViewPage )
				<< BUILD_LOG( uiTotalPage )
				<< BUILD_LOG( uiBeginIndex )
				<< BUILD_LOG( m_vecGuildMember.size() )
				<< END_LOG;
			return;
		}
	}

	std::vector< KGuildMemberInfoPtr >::const_iterator vit = m_vecGuildMember.begin();
	vit += uiBeginIndex;

	while( vit != m_vecGuildMember.end() )
	{
		if( (*vit) == NULL )
		{
			START_LOG( cerr, L"길드 멤버 포인터가 이상합니다." )
				<< END_LOG;
			break;
		}

		KGuildMemberMessageInfo kInfo;
		kInfo.m_iUnitUID		  = (*vit)->m_iUnitUID;
		kInfo.m_wstrNickName	  = (*vit)->m_wstrNickName;
		kInfo.m_ucMemberShipGrade = (*vit)->m_ucMemberShipGrade;
		kInfo.m_cUnitClass		  = (*vit)->m_cUnitClass;
		kInfo.m_ucLevel			  = (*vit)->m_ucLevel;
		kInfo.m_wstrMessage 	  = (*vit)->m_wstrMessage;
		vecMemberList.push_back( kInfo );

		if( vecMemberList.size() >= MEMBER_PAGE_PER_COUNT )
			break;

		++vit;
	}
}

int KGuild::GetGuildGradeMemberCount( IN u_char ucMemberShipGrade ) const
{
	int iMemberCount = 0;

	std::vector< KGuildMemberInfoPtr >::const_iterator vit;
	for( vit = m_vecGuildMember.begin(); vit != m_vecGuildMember.end(); ++vit )
	{
		if( (*vit) == NULL )
			continue;

		// 해당 길드원 등급인지?
		if( (*vit)->m_ucMemberShipGrade == ucMemberShipGrade )
		{
			++iMemberCount;
		}
	}

	return iMemberCount;
}

void KGuild::GuildChat( IN const KELG_CHAT_REQ& kInfo ) const
{
    KELG_CHAT_NOT kPacket;
	kPacket.m_cChatPacketType			= kInfo.m_cChatPacketType;
	kPacket.m_iSenderUnitUID			= kInfo.m_iSenderUnitUID;
	kPacket.m_wstrSenderUnitNickName	= kInfo.m_wstrSenderUnitNickName;
	kPacket.m_wstrMsg					= kInfo.m_wstrMsg;
	BroadCast( ELG_CHAT_NOT, kPacket );
}

bool KGuild::CheckChangeGradeGuild( IN const KELG_CHECK_CHANGE_GUILD_MEMBER_GRADE_REQ& kReq, OUT KELG_CHECK_CHANGE_GUILD_MEMBER_GRADE_ACK& kAck ) const
{
	SET_ERROR( NET_OK );

	// 미리 정보 담자!
	kAck.m_iTargetUnitUID = kReq.m_iTargetUnitUID;
	kAck.m_ucMemberShipGrade = kReq.m_ucMemberShipGrade;
	kAck.m_bChangeGuildMaster = false;

	// 1. 등급 변경 요청자
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드 멤버가 아닙니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 2. 등급 변경 대상자
	KGuildMemberInfoPtr spTargetMember = GetGuildMember( kReq.m_iTargetUnitUID );
	if( spTargetMember == NULL )
	{
		START_LOG( cerr, L"길드 멤버가 아닙니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iTargetUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 3. 변경 대상자의 등급과 변경하려는 등급 비교
	if( spTargetMember->m_ucMemberShipGrade == kReq.m_ucMemberShipGrade )
	{
		START_LOG( cerr, L"이미 변경된 등급입니다." )
			<< BUILD_LOG( kReq.m_iTargetUnitUID )
			<< BUILD_LOG( spTargetMember->m_ucMemberShipGrade )
			<< BUILD_LOGc( kReq.m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_33 );
		return false;
	}

	// 4. 길드 등급 변경 권한 검사
	if( spGuildMember->m_ucMemberShipGrade == SEnum::GUG_MASTER )
	{
		// 4-1. 길드 마스터가 다른 길드원에게 길드 마스터 위임!
		if( kReq.m_ucMemberShipGrade == SEnum::GUG_MASTER )
		{
			if( spTargetMember->m_ucMemberShipGrade == SEnum::GUG_MASTER )
			{
				START_LOG( cerr, L"한길드에 길드마스터가 2명인가? 있을수 없는에러!" )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( kReq.m_iTargetUnitUID )
					<< END_LOG;

				SET_ERROR( ERR_GUILD_19 );
				return false;
			}

			// 길드 마스터 위임 체크!
			kAck.m_bChangeGuildMaster = true;
		}
		// 4-2. 길드 마스터가 길드 관리자 임명!
		else if( kReq.m_ucMemberShipGrade == SEnum::GUG_SYSOP )
		{
			// 현재 길드 관리자 수 얻기
			const int iGuildSysopCount = GetGuildGradeMemberCount( static_cast<u_char>(SEnum::GUG_SYSOP) );
			int iGuildSysopMaxCount = 0;
			if( GetNumMember() <= 10 )
			{
				iGuildSysopMaxCount = 1;
			}
			else
			{
				iGuildSysopMaxCount = ( ( GetNumMember() - 1 ) / 10 ) + 1; // 길드 관리자는 실제길드원 1~10명부터 1명으로 시작해서 10명당 1명으로 임명가능
			}

			if( iGuildSysopMaxCount <= iGuildSysopCount )
			{
				// 길드 멤버수가 모자라 길드관리자를 임명할 수 없습니다.
				SET_ERROR( ERR_GUILD_34 );
				return false;
			}
		}
	}
	else if( spGuildMember->m_ucMemberShipGrade == SEnum::GUG_SYSOP )
	{
		// 길드 관리자가 길드 마스터 또는 다른 길드관리자의 등급을 변경할 수는 없다.
		if( spTargetMember->m_ucMemberShipGrade == SEnum::GUG_MASTER  ||
			spTargetMember->m_ucMemberShipGrade == SEnum::GUG_SYSOP )
		{
			START_LOG( cerr, L"길드 관리자가 길드 마스터 또는 다른 길드관리자의 등급을 변경하려고 하네?" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kReq.m_iUnitUID )
				<< BUILD_LOG( kReq.m_iTargetUnitUID )
				<< BUILD_LOGc( spTargetMember->m_ucMemberShipGrade )
				<< BUILD_LOGc( kReq.m_ucMemberShipGrade )
				<< END_LOG;

			SET_ERROR( ERR_GUILD_20 ); // 길드원 등급변경 권한이 없습니다.
			return false;
		}

		switch( kReq.m_ucMemberShipGrade )
		{
		case SEnum::GUG_OLD_USER:
		case SEnum::GUG_VIP_USER:
		case SEnum::GUG_NORMAL_USER:
		case SEnum::GUG_NEW_USER:
			break;

		default:
			{
				START_LOG( cerr, L"변경하려는 등급 정보가 이상하다!" )
					<< BUILD_LOG( GetUID() )
					<< BUILD_LOG( kReq.m_iUnitUID )
					<< BUILD_LOGc( kReq.m_ucMemberShipGrade )
					<< END_LOG;

				SET_ERROR( ERR_GUILD_20 ); // 허용되지 않는 등급변경입니다.
				return false;
			}
		}
	}
	else
	{
		START_LOG( cerr, L"길드 마스터 또는 길드 관리자가 아닌데 길드원등급변경을 하려고 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_20 );
		return false;
	}

	// 등급 변경 대상자의 닉네임 저장
	kAck.m_wstrTargetNickName = spTargetMember->m_wstrNickName;
	return true;
}

bool KGuild::UpdateChangeGuildMemberGrade( IN const KDBE_CHANGE_GUILD_MEMBER_GRADE_ACK& kInfo )
{
	// 길드 마스터 위임인 경우
	if( kInfo.m_bChangeGuildMaster )
	{
		const UidType iOldGuildMasterUnitUID = m_iGuildMasterUnitUID;

		KGuildMemberInfoPtr spGuildMaster = GetGuildMember( m_iGuildMasterUnitUID );
		if( spGuildMaster == NULL )
		{
			START_LOG( cerr, L"길드 마스터 정보 얻기 실패!" )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( m_iGuildMasterUnitUID )
				<< END_LOG;

			return false;
		}

		// 일반 길드원으로 등급 하락
		spGuildMaster->m_ucMemberShipGrade = SEnum::GUG_NORMAL_USER;

		// 새로 임명된 유저 UnitUID를 길드 마스터 UnitUID로 저장
		m_iGuildMasterUnitUID = kInfo.m_iTargetUnitUID;

		// 길드마스터의 위임 사실을 다른 길드원들에게 알리자!
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_CHANGE_MEMBER_GRADE;
		kPacket.m_iUnitUID		= iOldGuildMasterUnitUID;
		kPacket.m_wstrNickName	= spGuildMaster->m_wstrNickName;
		kPacket.m_iMessageData	= static_cast<int>(spGuildMaster->m_ucMemberShipGrade);
		BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	}

	// 등급 변경된 유저 찾기
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kInfo.m_iTargetUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iTargetUnitUID )
			<< END_LOG;

		return false;
	}

	// 길드원 등급 정보 업데이트
	spGuildMember->m_ucMemberShipGrade = kInfo.m_ucMemberShipGrade;

	// 길드원 등급이 바뀌었으면 등급순으로 정렬한번 합시다!
	GuildMemberSort();

	// 길드원 등급 변경 사실을 다른 길드원들에게 알리자!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_CHANGE_MEMBER_GRADE;
	kPacket.m_iUnitUID		= kInfo.m_iTargetUnitUID;
	kPacket.m_wstrNickName	= spGuildMember->m_wstrNickName;
	kPacket.m_iMessageData	= static_cast<int>(kInfo.m_ucMemberShipGrade);
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );

	// 길드 마스터 위임인 경우 길드 정보를 갱신해줍시다
	if( kInfo.m_bChangeGuildMaster )
	{
		KEGS_GUILD_INFO_NOT kPacketNot;
		//{{ 2009. 11. 26  최육사	길드스킬
		GetGuildInfo( kPacketNot.m_kGuildInfo, kPacketNot.m_wstrGuildMasterName, kPacketNot.m_vecMemberUnitUID, kPacketNot.m_kGuildSkillInfo );
		//}}
		BroadCast( ELG_GUILD_INFO_NOT, kPacketNot );
	}

	return true;
}

void KGuild::UpdateChangeGuildMessage( IN const KDBE_CHANGE_GUILD_MESSAGE_ACK& kInfo )
{
	// 메시지 변경
    m_kGuildInfo.m_wstrGuildMessage = kInfo.m_wstrMessage;

	// 모든 길드원에게 길드정보 변경을 알리자!
	{
		KEGS_GUILD_INFO_NOT kPacketNot;
		//{{ 2009. 11. 26  최육사	길드스킬
		GetGuildInfo( kPacketNot.m_kGuildInfo, kPacketNot.m_wstrGuildMasterName, kPacketNot.m_vecMemberUnitUID, kPacketNot.m_kGuildSkillInfo );
		//}}
		BroadCast( ELG_GUILD_INFO_NOT, kPacketNot );
	}

	// 변경된 길드 메시지도 날리자!
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType		= KEGS_GUILD_MESSAGE_NOT::MT_GUILD_MESSAGE;
		kPacket.m_wstrMessage		= m_kGuildInfo.m_wstrGuildMessage;
		BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	}
}

bool KGuild::UpdateChangeGuildMemberMessage( IN const KDBE_CHANGE_GUILD_MEMBER_MESSAGE_ACK& kInfo )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kInfo.m_iTargetUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iTargetUnitUID )
			<< END_LOG;

		return false;
	}

	// 변경된 메시지 업데이트!
	spGuildMember->m_wstrMessage = kInfo.m_wstrMessage;

	// 길드원 인사말 변경 사실을 다른 길드원들에게 알리자!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_CHANGE_MEMBER_MESSAGE;
	kPacket.m_iUnitUID		= kInfo.m_iTargetUnitUID;
	kPacket.m_wstrNickName  = spGuildMember->m_wstrNickName;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	return true;
}

// 길드 멤버 정보 갱신
bool KGuild::UpdateGuildMemberInfo( IN const KGuildMemberInfo& kInfo )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kInfo.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		// 시간차로 발생 가능한 오류 입니다.
		START_LOG( cwarn, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iUnitUID )
			<< END_LOG;

		return false;
	}

	const bool bIsChangeLevel = ( spGuildMember->m_ucLevel != kInfo.m_ucLevel );

	// 변경된 정보 업데이트!
	spGuildMember->m_cUnitClass	  = kInfo.m_cUnitClass;
	spGuildMember->m_ucLevel	  = kInfo.m_ucLevel;
	spGuildMember->m_iMapID		  = kInfo.m_iMapID;
	spGuildMember->m_cPosition	  = kInfo.m_cPosition;

	// 변경된 정보중에 레벨정보가 있으면 정렬한번하자!
	if( bIsChangeLevel )
	{
		// 길드원 레벨이 바뀌었으면 등급순으로 정렬한번 합시다!
		GuildMemberSort();
	}

	// 길드원 멤버 정보 변경 사실을 다른 길드원들에게 알리자!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_UPDATE_MEMBER_INFO;
	kPacket.m_iUnitUID		= kInfo.m_iUnitUID;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	return true;
}

bool KGuild::CheckGuildMaxMember( IN const KELG_CHECK_EXPAND_GUILD_MAX_MEMBER_REQ& kInfo )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kInfo.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 또는 길드 관리자 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER  &&
		spGuildMember->m_ucMemberShipGrade != SEnum::GUG_SYSOP )
	{
		START_LOG( cerr, L"길드 마스터 또는 관리자가 아닌데 길드 인원 확장을 하려 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_40 );
		return false;
	}

	// 길드 인원 확장 여유가 되는가?
	if( GetMaxMember() >= KGuildInfo::GUILD_MAX_MEMBER_LIMIT )
	{
		START_LOG( cwarn, L"이미 길드 인원 제한인데 더 확장하려고함" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kInfo.m_iUnitUID )
			<< BUILD_LOG( GetMaxMember() )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_41 );
		return false;
	}

	return true;
}

void KGuild::UpdateGuildMaxMember( IN u_short usMaxNumMember )
{
	// 길드 최대 인원 업데이트
	m_kGuildInfo.m_usMaxNumMember = usMaxNumMember;
	LIF( m_kGuildInfo.m_usMaxNumMember <= KGuildInfo::GUILD_MAX_MEMBER_LIMIT  );

	// 변경된 길드 정보를 모든 길드원에게 알리자!
	KEGS_GUILD_INFO_NOT kPacketNot;
	//{{ 2009. 11. 26  최육사	길드스킬
	GetGuildInfo( kPacketNot.m_kGuildInfo, kPacketNot.m_wstrGuildMasterName, kPacketNot.m_vecMemberUnitUID, kPacketNot.m_kGuildSkillInfo );
	//}}
	BroadCast( ELG_GUILD_INFO_NOT, kPacketNot );

	// 길드 최대 인원 변경 알리기
	{
		KEGS_GUILD_MESSAGE_NOT kPacket;
		kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_EXPAND_MAX_MEMBER;
		kPacket.m_iMessageData = GetMaxMember();
		BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
	}
}

void KGuild::UpdateGuildEXP( IN const KDBE_UPDATE_GUILD_EXP_ACK& kInfo )
{
	// 길드 명예 포인트 업데이트
	m_kGuildInfo.m_iGuildEXP = kInfo.m_iGuildEXP;

	// 길드 명예 포인트 갱신 알리기
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType	= KEGS_GUILD_MESSAGE_NOT::MT_GUILD_EXP;
	kPacket.m_iUnitUID = kInfo.m_iUnitUID;
	kPacket.m_iMessageData = m_kGuildInfo.m_iGuildEXP;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );

	// 길드 레벨업 체크
	CheckGuildLevelUp();
}

void KGuild::GuildMemberSort()
{
	// [참고] 객체를 정렬하기 위해서는 std::stable_sort()함수를 써야함.

	std::stable_sort( m_vecGuildMember.begin(), m_vecGuildMember.end(), &KGuild::GuildMemberGradeSort );
}

bool KGuild::GuildMemberGradeSort( const KGuildMemberInfoPtr spFirst, const KGuildMemberInfoPtr spSecond )
{
	// NULL포인터 검사
	if( spFirst == NULL  ||  spSecond == NULL )
		return false;

	// 1. 접속중인 유저가 높다!
	if( spFirst->IsLogined()  &&  !spSecond->IsLogined() )
	{
		return true;
	}
	// 온라인 유저
	else if( spFirst->IsLogined()  &&  spSecond->IsLogined() )
	{
		// 2. 길드 등급이 높은 순!
		if( spFirst->m_ucMemberShipGrade < spSecond->m_ucMemberShipGrade )
		{
			return true;
		}
		else if( spFirst->m_ucMemberShipGrade == spSecond->m_ucMemberShipGrade )
		{
			// 3. 캐릭터 레벨이 높은 순!
			if( spFirst->m_ucLevel > spSecond->m_ucLevel )
			{
				return true;
			}
			else if( spFirst->m_ucLevel == spSecond->m_ucLevel )
			{
				// 4. 접속 순서가 빠른 순!
				if( spFirst->m_tLogInTime > spSecond->m_tLogInTime )
				{
					return true;
				}
			}
		}
	}
	// 오프라인 유저
	else if( !spFirst->IsLogined()  &&  !spSecond->IsLogined() )
	{
		// 2. 최근 접속 종료한 순서대로
		if( spFirst->m_tLogOutTime > spSecond->m_tLogOutTime )
		{
			return true;
		}
	}

	return false;
}

void KGuild::CheckGuildLevelUp()
{
	// 길드 레벨업 여부 체크
	u_char ucNowLevel = GetGuildLevel();

	m_kGuildInfo.m_ucGuildLevel = SiCXSLGuildManager()->CheckGuildLevelUp( ucNowLevel, m_kGuildInfo.m_iGuildEXP );

	if( ucNowLevel < m_kGuildInfo.m_ucGuildLevel )
	{
		// 길드 레벨업 했다! GameDB에 레벨업 하러 가자!
		KDBE_GUILD_LEVEL_UP_REQ kPacket;
		kPacket.m_iGuildUID = GetUID();
		kPacket.m_wstrGuildName = m_kGuildInfo.m_wstrGuildName;
		kPacket.m_ucGuildLevel = GetGuildLevel();
		//{{ 2009. 11. 26  최육사	길드스킬
		kPacket.m_iGuildSPoint = m_kSkillTree.GetGuildSPoint() + 1; // 1증가시켜서 업데이트한다.
		//}}

		if( CXSLGuildManager::IsValidSkillPointFigure( kPacket.m_iGuildSPoint ) == false )
		{
			START_LOG( cerr, L"스킬 포인트 값의 범위가 이상합니다." )
				<< BUILD_LOG( kPacket.m_iGuildUID )
				<< BUILD_LOG( kPacket.m_wstrGuildName )
				<< BUILD_LOG( kPacket.m_iGuildSPoint )
				<< END_LOG;
		}

		SendToGameDB( DBE_GUILD_LEVEL_UP_REQ, kPacket );
	}
}

void KGuild::UpdateGuildLevelResult( IN const KDBE_GUILD_LEVEL_UP_ACK& kInfo )
{
	if( kInfo.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 레벨 및 스킬포인트 DB업데이트 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOGc( kInfo.m_ucGuildLevel )
			//{{ 2009. 11. 26  최육사	길드스킬
			<< BUILD_LOG( kInfo.m_iGuildSPoint )
			//}}
			<< END_LOG;

		return;
	}

	//{{ 2009. 11. 26  최육사	길드스킬
	// 길드 스킬 정보 업데이트
	m_kSkillTree.SetGuildSPoint( kInfo.m_iGuildSPoint );
	//}}

	// 길드 레벨업 길드원에게 알리기!
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType = KEGS_GUILD_MESSAGE_NOT::MT_GUILD_LEVEL_UP;
	kPacket.m_iMessageData = static_cast<int>(GetGuildLevel());
	//{{ 2009. 11. 26  최육사	길드스킬
	kPacket.m_iMessageData2 = m_kSkillTree.GetGuildSPoint();
	//}}
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
}


//////////////////////////////////////////////////////////////////////////
//{{ 2009. 11. 23  최육사	길드스킬

bool KGuild::CheckLearnGuildSkill( IN const KELG_GET_GUILD_SKILL_REQ& kReq, OUT KDBE_GET_GUILD_SKILL_REQ& kPacketToDB )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER )
	{
		START_LOG( cerr, L"길드 마스터 또는 관리자가 아닌데 길드 스킬을 배우려 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_00 );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	// 이미 최고레벨까지 배운 스킬인지 검사
	if( m_kSkillTree.IsMasterSkillLevel( kReq.m_iGuildSkillID ) == true )
	{
		START_LOG( cerr, L"이미 최고 레벨까지 배운 길드 스킬입니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_01 );
		return false;
	}

	const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( kReq.m_iGuildSkillID );
	if( pSkillTemplet == NULL )
	{
		START_LOG( cerr, L"스킬 템플릿 정보가 없습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	// 선행스킬을 최고레벨까지 습득했는지 체크
	if( m_kSkillTree.IsAllPrecedingSkillMasterLevel( kReq.m_iGuildSkillID ) == false )
	{
		START_LOG( cerr, L"선행스킬을 아직 마스터레벨까지 습득하지 못했습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_03 );
		return false;
	}


	// 선행 tier에 충분한 SP를 소비했는지 체크
	if( m_kSkillTree.IsTierOpened( kReq.m_iGuildSkillID ) == false )
	{
		START_LOG( cerr, L"선행 tire에서 충분한 SP를 소비하지 않았습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_04 );
		return false;
	}

	// 충분한 SP 혹은 CSP를 가지고 있는지 확인
	if( m_kSkillTree.IsCashSkillPointExpired() )
	{
		if( m_kSkillTree.GetGuildSPoint() <= 0 )
		{
			SET_ERROR( ERR_GUILD_SKILL_05 );
			return false;
		}
	}
	else
	{
		if( ( m_kSkillTree.GetGuildSPoint() + m_kSkillTree.GetGuildCSPoint() ) <= 0 )
		{
			SET_ERROR( ERR_GUILD_SKILL_05 );
			return false;
		}
	}

	int iSkillLevel = 0;
	int iSkillCSPoint = 0;
	if( m_kSkillTree.GetSkillLevelAndCSP( kReq.m_iGuildSkillID, iSkillLevel, iSkillCSPoint ) == false )
	{
		iSkillLevel = 0;
		iSkillCSPoint = 0;
	}

	// DB에 업데이트할 정보
	kPacketToDB.m_iGuildUID			= GetUID();
	kPacketToDB.m_iUnitUID			= kReq.m_iUnitUID;
	kPacketToDB.m_iGuildSkillID		= kReq.m_iGuildSkillID;
	kPacketToDB.m_iGuildSkillLevel	= iSkillLevel + 1;
	kPacketToDB.m_iMaxGuildCSPoint	= m_kSkillTree.GetMaxGuildCSPoint();

	// 기간제 스킬포인트을 획득한 상태인지 검사
	if( m_kSkillTree.IsCashSkillPointExpired() == true )
	{
		kPacketToDB.m_iGuildSkillCSPoint	= iSkillCSPoint;

		kPacketToDB.m_iGuildCSPoint			= -1; // invalid 한 값을 보내서 SP로 배웠다는 것을 구분한다
		kPacketToDB.m_iGuildSPoint			= m_kSkillTree.GetGuildSPoint() - 1;
	}
	else
	{
		// 기간제 스킬 포인트를 획득한 상태라면 지금부터 찍히는 스킬포인트는 기간제 스킬포인트로 가정한다.
		kPacketToDB.m_iGuildSkillCSPoint	= iSkillCSPoint + 1;

		if( m_kSkillTree.GetGuildCSPoint() > 0 )
		{
			kPacketToDB.m_iGuildCSPoint = m_kSkillTree.GetGuildCSPoint() - 1;
			kPacketToDB.m_iGuildSPoint	= m_kSkillTree.GetGuildSPoint();
		}
		else
		{
			kPacketToDB.m_iGuildCSPoint	= -1; // invalid 한 값을 보내서 SP로 배웠다는 것을 구분한다
			kPacketToDB.m_iGuildSPoint	= m_kSkillTree.GetGuildSPoint() - 1;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool KGuild::InsertGuildSkill( IN const KDBE_GET_GUILD_SKILL_ACK& kAck )
{
	//스킬습득시 소모된sp 빼준다.
	if( kAck.m_iGuildCSPoint >= 0 )
	{
		// CSPoint를 사용해서 스킬을 획득한 경우
		m_kSkillTree.SetGuildCSPoint( kAck.m_iGuildCSPoint );
	}
	else
	{
		// SPoint 를 사용해서 스킬 획득한 경우
		m_kSkillTree.SetGuildSPoint( kAck.m_iGuildSPoint );
	}

	if( m_kSkillTree.SetSkillLevelAndCSP( kAck.m_iGuildSkillID, kAck.m_iGuildSkillLevel, kAck.m_iGuildSkillCSPoint ) == false )
		return false;

	// 스킬을 배우면서 변경된 유저의 데이터도 다시 넘겨준다.
	KEGS_GET_GUILD_SKILL_NOT kPacketNot;
	kPacketNot.m_iGuildSkillID		= kAck.m_iGuildSkillID;
	kPacketNot.m_iGuildSkillLv		= kAck.m_iGuildSkillLevel;
	kPacketNot.m_iGuildSkillCSPoint	= kAck.m_iGuildSkillCSPoint;
	kPacketNot.m_iGuildSPoint		= m_kSkillTree.GetGuildSPoint();
	kPacketNot.m_iGuildCSPoint		= m_kSkillTree.GetGuildCSPoint();
	BroadCast( ELG_GET_GUILD_SKILL_NOT, kPacketNot );
	return true;
}

bool KGuild::CheckResetGuildSkill( IN const KELG_RESET_GUILD_SKILL_REQ& kReq, OUT KDBE_RESET_GUILD_SKILL_REQ& kPacketToDB )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER )
	{
		START_LOG( cerr, L"길드 마스터 또는 관리자가 아닌데 길드 스킬 되돌리기를 하려 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_07 );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	// 초기화 정보 얻기
	const CXSLSkillTree::SkillTemplet* pSkillTemplet = SiCXSLSkillTree()->GetSkillTemplet( kReq.m_iGuildSkillID );
	if( pSkillTemplet == NULL )
	{
		START_LOG( cerr, L"존재하지 않는 스킬ID를 초기화 하려 했습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	// 초기화 시킬 수 있을 만큼 스킬 레벨이 높은지  검사
	int iSkillLevel = 0;
	int iSkillCSPoint = 0;
	if( m_kSkillTree.GetSkillLevelAndCSP( kReq.m_iGuildSkillID, iSkillLevel, iSkillCSPoint ) == false )
	{
		START_LOG( cerr, L"클라이언트에서 스킬 검사를 했을텐데.." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	if( iSkillLevel < 1 )
	{
		START_LOG( cerr, L"클라이언트에서 스킬 검사를 했을텐데.." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	// 말단 노드 인지 검사
	if( m_kSkillTree.IsAllFollowingSkillLevelZero( kReq.m_iGuildSkillID ) == false )
	{
		START_LOG( cerr, L"클라이언트에서 말단노드 검사를 했을텐데.." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_09 );
		return false;
	}

	const CXSLSkillTree::SkillTreeTemplet* pSkillTreeTemplet = SiCXSLSkillTree()->GetGuildSkillTreeTemplet( kReq.m_iGuildSkillID );
	if( pSkillTreeTemplet == NULL )
	{
		START_LOG( cerr, L"트리에 존재하지 않는 길드스킬ID를 되돌리기 하려 했습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	// 스킬 되돌리기를 하고 난 후에 후행 tier에 스킬을 못 찍게 되는 경우 스킬 리셋을 할 수 없다
	std::vector< int > vecCumulativeTierSPoint;
	m_kSkillTree.CalcCumulativeUsedSPointOnEachTier( vecCumulativeTierSPoint );
	if( pSkillTreeTemplet->m_iTier < 0 ||
		pSkillTreeTemplet->m_iTier > (int)vecCumulativeTierSPoint.size() - 1 )
	{
		START_LOG( cerr, L"스킬 템플릿의 tier index가 이상합니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( pSkillTreeTemplet->m_iTier )
			<< BUILD_LOG( kReq.m_iGuildSkillID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_02 );
		return false;
	}

	const int MAGIC_SKILL_POINT_PER_TIER = 5;
	for( u_int ui = pSkillTreeTemplet->m_iTier + 1; ui < vecCumulativeTierSPoint.size(); ++ui )
	{
		if( vecCumulativeTierSPoint[ui-1]-1 < (int)ui * MAGIC_SKILL_POINT_PER_TIER )
		{
			START_LOG( cerr, L"후행 tier가 있는 스킬ID를 초기화 하려 했습니다." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( pSkillTreeTemplet->m_iTier )
				<< BUILD_LOG( ui )
				<< BUILD_LOG( vecCumulativeTierSPoint[ui-1] )
				<< BUILD_LOG( kReq.m_iGuildSkillID )
				<< END_LOG;

			SET_ERROR( ERR_GUILD_SKILL_02 );
			return false;
		}
	}

	// DB로 전송
	kPacketToDB.m_iGuildUID = GetUID();
	kPacketToDB.m_iUnitUID = kReq.m_iUnitUID;
	kPacketToDB.m_iGuildSkillID = kReq.m_iGuildSkillID;
	kPacketToDB.m_iGuildSkillLevel = iSkillLevel - 1;
	kPacketToDB.m_iMaxGuildCSPoint = m_kSkillTree.GetMaxGuildCSPoint();

	if( iSkillCSPoint > 0 )
	{
		kPacketToDB.m_iGuildSkillCSPoint = iSkillCSPoint - 1;
	}
	else
	{
		kPacketToDB.m_iGuildSkillCSPoint = iSkillCSPoint;
	}

	if( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		// Cash skill point가 유효한 기간이라면 CSP로 돌려주고, 아니라면 SP로 돌려준다
		kPacketToDB.m_iGuildCSPoint = m_kSkillTree.GetGuildCSPoint() + 1;
		kPacketToDB.m_iGuildSPoint	= m_kSkillTree.GetGuildSPoint();
	}
	else
	{
		kPacketToDB.m_iGuildCSPoint = -1; // invalid 한 값을 보내서 SP로 배웠다는 것을 구분한다
		kPacketToDB.m_iGuildSPoint	= m_kSkillTree.GetGuildSPoint() + 1;
	}

	return true;
}

void KGuild::ResetGuildSkill( IN const KDBE_RESET_GUILD_SKILL_ACK& kAck )
{
	// 스킬 포인트 추가
	if( kAck.m_iGuildCSPoint >= 0 )
	{
		m_kSkillTree.SetGuildCSPoint( kAck.m_iGuildCSPoint );
	}
	else
	{
		m_kSkillTree.SetGuildSPoint( kAck.m_iGuildSPoint );
	}

	// 길드 스킬 되돌리기
	m_kSkillTree.SetSkillLevelAndCSP( kAck.m_iGuildSkillID, kAck.m_iGuildSkillLevel, kAck.m_iGuildSkillCSPoint );

	// 길드 스킬 되돌리기 결과를 길드원들에게 알려주자!
	KEGS_RESET_GUILD_SKILL_NOT kPacketNot;
	kPacketNot.m_iDelGuildSkillID = kAck.m_iGuildSkillID;
	if( kAck.m_iGuildCSPoint >= 0 )
	{
		kPacketNot.m_iRestoreGuildSPoint	   = 0;
		kPacketNot.m_iRestoreGuildCSPoint	   = 1;
	}
	else
	{
		kPacketNot.m_iRestoreGuildSPoint	   = 1;
		kPacketNot.m_iRestoreGuildCSPoint	   = 0;
	}
	BroadCast( ELG_RESET_GUILD_SKILL_NOT, kPacketNot );
}

bool KGuild::CheckInitGuildSkill( IN const KELG_INIT_GUILD_SKILL_REQ& kReq, OUT KDBE_INIT_GUILD_SKILL_REQ& kPacketToDB )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER )
	{
		START_LOG( cerr, L"길드 마스터 또는 관리자가 아닌데 길드 스킬 초기화를 하려 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_11 );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	// 배운 스킬이 없다면..
	int iUsedSPoint = 0;
	int iUsedCSPoint = 0;
	m_kSkillTree.CalcUsedSPointAndCSPoint( iUsedSPoint, iUsedCSPoint );

	if( ( iUsedSPoint <= 0 && iUsedCSPoint <= 0 )  ||  ( iUsedSPoint + iUsedCSPoint <= 0 ) )
	{
		START_LOG( cwarn, L"스킬을 하나도 배우지 않았거나 기본 스킬만 있는 상태인데( 즉, 더이상 초기화 할 필요가 없는데) 초기화 하려고 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOG( iUsedSPoint )
			<< BUILD_LOG( iUsedCSPoint )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_13 );
		return false;
	}

	int iRetrievedSPoint = 0 ;	// 돌려받을 SP
	int iRetrievedCSPoint = 0;	// 돌려받을 CSP

	if( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		iRetrievedSPoint	= iUsedSPoint + iUsedCSPoint + m_kSkillTree.GetGuildCSPoint() - m_kSkillTree.GetMaxGuildCSPoint();
		iRetrievedCSPoint	= m_kSkillTree.GetMaxGuildCSPoint();
	}
	else
	{
		if( 0 == iUsedCSPoint  &&  0 == m_kSkillTree.GetGuildCSPoint() )
		{
			iRetrievedSPoint	= iUsedSPoint;
			iRetrievedCSPoint	= 0;
		}
		else
		{
			START_LOG( cerr, L"cash skill point기한이 다되었는데 스킬트리에 csp가 남아있다." )
				<< BUILD_LOG( GetUID() )
				<< BUILD_LOG( kReq.m_iUnitUID )
				<< BUILD_LOG( iUsedSPoint )
				<< BUILD_LOG( iUsedCSPoint )
				<< BUILD_LOG( m_kSkillTree.GetGuildCSPoint() )
				<< END_LOG;

			SET_ERROR( ERR_GUILD_SKILL_13 );
			return false;
		}
	}

	// DB로 업데이트할 데이터를 얻자!
	kPacketToDB.m_iGuildUID		= GetUID();
	kPacketToDB.m_iUnitUID		= kReq.m_iUnitUID;
	kPacketToDB.m_iItemUID		= kReq.m_iItemUID;
	kPacketToDB.m_iGuildSPoint	= m_kSkillTree.GetGuildSPoint() + iRetrievedSPoint;
	kPacketToDB.m_iGuildCSPoint	= iRetrievedCSPoint;
	return true;
}

void KGuild::InitGuildSkill( IN const KDBE_INIT_GUILD_SKILL_ACK& kAck )
{
	// 스킬포인트 새로 셋팅
	m_kSkillTree.SetGuildSPoint( kAck.m_iGuildSPoint );
	m_kSkillTree.SetGuildCSPoint( kAck.m_iGuildCSPoint );

	// 배운스킬 삭제
	m_kSkillTree.ResetGuildSkillTree();

    // 모든 길드원들에게 길드 스킬 초기화 알려주기
	KEGS_INIT_GUILD_SKILL_NOT kPacketNot;
	kPacketNot.m_iGuildSPoint = m_kSkillTree.GetGuildSPoint();
	kPacketNot.m_iGuildCSPoint = m_kSkillTree.GetGuildCSPoint();
	BroadCast( ELG_INIT_GUILD_SKILL_NOT, kPacketNot );
}

bool KGuild::InsertGuildCashSkillPoint( IN const KELG_INSERT_GUILD_CASH_SKILL_POINT_NOT& kNot )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kNot.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kNot.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER )
	{
		START_LOG( cerr, L"길드 마스터 또는 관리자가 아닌데 길드 스킬 되돌리기를 하려 함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kNot.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_15 );
		return false;
	}

	// 이미 그노시스 대축복 아이템을 사용중이라면 다시 사용할 수 없다.
	if( m_kSkillTree.IsCashSkillPointExpired() == false )
	{
		START_LOG( cerr, L"이미 그노시스 대축복을 사용중이면 다시 사용못함. 체크했을텐데.." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kNot.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_17 );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	KDBE_INSERT_GUILD_CASH_SKILL_POINT_REQ kPacket;
	kPacket.m_iGuildUID = GetUID();
	kPacket.m_iGuildCSPoint = kNot.m_iGuildCSPoint;
	kPacket.m_iPeriod = kNot.m_iPeriod;
	kPacket.m_bUpdateEndDateOnly = ( m_kSkillTree.IsCashSkillPointExpired() == true ? false : true );
	SendToGameDB( DBE_INSERT_GUILD_CASH_SKILL_POINT_REQ, kPacket );
	return true;
}

void KGuild::InsertGuildCashSkillPointResult( IN const KDBE_INSERT_GUILD_CASH_SKILL_POINT_ACK& kAck )
{
	if( kAck.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 캐쉬 스킬 포인트 insert DB업데이트 실패!" )
			<< BUILD_LOG( kAck.m_iGuildCSPoint )
			<< BUILD_LOG( kAck.m_iPeriod )
			<< BUILD_LOG( kAck.m_bUpdateEndDateOnly )
			<< END_LOG;
		return;
	}

	// 기간 연장이 아니라면..
	if( kAck.m_bUpdateEndDateOnly == false )
	{
		m_kSkillTree.SetGuildCSPoint( kAck.m_iGuildCSPoint );
		m_kSkillTree.SetMaxGuildCSPoint( kAck.m_iGuildCSPoint );
	}
	m_kSkillTree.SetGuildCSPointEndDate( kAck.m_wstrEndDate );

	// 길드원들에게 알리자!
	{
		KEGS_GUILD_MESSAGE_NOT kPacketNot;
		kPacketNot.m_cMessageType = KEGS_GUILD_MESSAGE_NOT::MT_INSERT_GUILD_CSPOINT;
		kPacketNot.m_iMessageData = m_kSkillTree.GetGuildCSPoint();
		kPacketNot.m_iMessageData2 = m_kSkillTree.GetMaxGuildCSPoint();
		kPacketNot.m_wstrMessage = kAck.m_wstrEndDate;
        BroadCast( ELG_GUILD_MESSAGE_NOT, kPacketNot );
	}
}

void KGuild::ExpireGuildCashSkillPointResult( IN const KDBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK& kAck )
{
	if( kAck.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"길드 캐쉬 스킬 포인트 기간종료 DB업데이트 실패!" )
			<< BUILD_LOG( kAck.m_iRetrievedSPoint )
			<< END_LOG;
		return;
	}

	// 스킬 정보 갱신
	KEGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT kPacketNot;

	m_kSkillTree.SetGuildSPoint( m_kSkillTree.GetGuildSPoint() + kAck.m_iRetrievedSPoint );
	m_kSkillTree.ExpireCashSkillPoint( kPacketNot.m_vecGuildSkillData );

	// 길드 스킬 변경 정보 전달
	kPacketNot.m_iGuildSPoint = m_kSkillTree.GetGuildSPoint();
	kPacketNot.m_iGuildCSPoint = m_kSkillTree.GetGuildCSPoint();
	BroadCast( ELG_EXPIRE_GUILD_CASH_SKILL_POINT_NOT, kPacketNot );
}

bool KGuild::AdminInsertGuildSkillPoint( IN const KELG_ADMIN_GET_GUILD_SKILL_POINT_REQ& kReq )
{
	SET_ERROR( NET_OK );

	// 길드 소속인가?
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cwarn, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 마스터 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER )
	{
		START_LOG( cwarn, L"길드 마스터가 아닌데 길드 스킬 포인트 치트를 사용하려함." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_SKILL_00 );
		return false;
	}

	// DB에 업데이트하러 가자!
    SendToGameDB( DBE_ADMIN_GET_GUILD_SKILL_POINT_REQ, kReq );
	return true;
}

void KGuild::AdminInsertGuildSkillPointResult( IN const KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK& kAck )
{
    // 치트 결과 업데이트
	m_kSkillTree.SetGuildSPoint( kAck.m_iGuildSPoint );

	// 치트 결과 패킷 전송
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kAck.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kAck.m_iUnitUID )
			<< END_LOG;

		return;
	}

	if( spGuildMember->IsLogined() == false )
	{
		START_LOG( cerr, L"로그인 상태가 아닙니다" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kAck.m_iUnitUID )
			<< END_LOG;

		return;
	}

	// 패킷 전송
	KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK kPacket;
	kPacket.m_iOK = NetError::NET_OK;
	kPacket.m_iGuildSPoint = m_kSkillTree.GetGuildSPoint();
	SendToGSCharacter( spGuildMember->m_iGSUID, spGuildMember->m_iUnitUID, ELG_ADMIN_GET_GUILD_SKILL_POINT_ACK, kPacket );
}

//}}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//{{ 2010. 01. 13  최육사	길드게시판

bool KGuild::CheckRegistrationGuildAd( IN IN UidType iUnitUID )
{
	SET_ERROR( NET_OK );

	KGuildMemberInfoPtr spGuildMember = GetGuildMember( iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 길드 광고를 등록할수 있는 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER  &&
		spGuildMember->m_ucMemberShipGrade != SEnum::GUG_SYSOP )
	{
		START_LOG( cerr, L"길드 광고를 등록할수있는 등급이 아닙니다!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_BOARD_00 );
		return false;
	}

	return true;
}

bool KGuild::MakeGuildAdInfo( IN const KDBE_REGISTRATION_GUILD_AD_ACK& kAck, OUT KGuildAdInfo& kInfo )
{
	KGuildMemberInfoPtr spGuildMember = GetGuildMember( m_iGuildMasterUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"길드 마스터의 정보가 없습니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( m_iGuildMasterUnitUID )
			<< END_LOG;

		return false;
	}

	// 길드 광고 정보 만들기
	kInfo.m_iGuildUID = GetUID();
	kInfo.m_wstrGuildName = GetGuildName();
	kInfo.m_ucGuildLevel = GetGuildLevel();
	kInfo.m_iGuildEXP = m_kGuildInfo.m_iGuildEXP;
	kInfo.m_wstrMasterNickName = spGuildMember->m_wstrNickName;
	kInfo.m_usCurNumMember = static_cast<u_short>(GetNumMember());
	kInfo.m_wstrAdMessage = kAck.m_wstrAdMessage;
	//{{ 2010. 02. 04  최육사	길드 광고 오류 수정
	kInfo.m_wstrFoundingDay	= m_kGuildInfo.m_wstrFoundingDay;
	//}}
	kInfo.m_wstrAdRegDate = kAck.m_wstrAdRegDate;
	kInfo.m_wstrAdEndDate = kAck.m_wstrAdEndDate;
	return true;
}

void KGuild::ResultRegGuildAd( IN const std::wstring& wstrEndDate )
{
	KEGS_GUILD_MESSAGE_NOT kPacket;
	kPacket.m_cMessageType = KEGS_GUILD_MESSAGE_NOT::MT_REG_GUILD_AD;
	kPacket.m_wstrMessage = wstrEndDate;
	BroadCast( ELG_GUILD_MESSAGE_NOT, kPacket );
}

bool KGuild::CheckGetApplyJoinGuildList( IN const KELG_GET_APPLY_JOIN_GUILD_LIST_REQ& kReq )
{
	SET_ERROR( NET_OK );

	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	return true;
}

bool KGuild::CheckDeleteApplyJoin( IN const KELG_DELETE_APPLY_JOIN_GUILD_REQ& kReq )
{
	SET_ERROR( NET_OK );

	KGuildMemberInfoPtr spGuildMember = GetGuildMember( kReq.m_iUnitUID );
	if( spGuildMember == NULL )
	{
		START_LOG( cerr, L"해당 길드원이 아닙니다. 길드원 정보 얻기 실패!" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_19 );
		return false;
	}

	// 신청 삭제 가능한 등급인가?
	if( spGuildMember->m_ucMemberShipGrade != SEnum::GUG_MASTER  &&
		spGuildMember->m_ucMemberShipGrade != SEnum::GUG_SYSOP )
	{
		START_LOG( cerr, L"길드 가입 신청 삭제 가능한 등급이 아닙니다." )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kReq.m_iUnitUID )
			<< BUILD_LOGc( spGuildMember->m_ucMemberShipGrade )
			<< END_LOG;

		SET_ERROR( ERR_GUILD_BOARD_00 );
		return false;
	}

	return true;
}

//}}
//////////////////////////////////////////////////////////////////////////
