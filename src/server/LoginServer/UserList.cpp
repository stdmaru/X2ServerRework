#include "NetError.h"
#include "ActorManager.h"
#include "LoginUser.h"
#include "SimLayer.h"
//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "GameSysVal/GameSysVal.h"
//}}
//#include "GASHAuthManager.h"

#include "UserList.h"
#include "BaseServer.h"


KUserList::KUserList() :
m_bEnableAuthWait( true ),
//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
m_uiConcurrentUser( 0 ),
//}}
m_iGuestConcurrentUser( 0 )
{
}

KUserList::~KUserList()
{
}

bool KUserList::GetGSUserInfoByUserUID( UidType iUserUID, KGSUserInfo& kInfo )
{
    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mit;

    mit = m_mapUserUID.find( iUserUID );
    if( mit == m_mapUserUID.end() )
    {
        return false;
    }

    if( !mit->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    kInfo = *( mit->second );
    return true;
}

bool KUserList::GetGSUserInfoByUserID( const std::wstring& wstrUserID, KUserList::KGSUserInfo& kInfo )
{
    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mit;

    mit = m_mapUserID.find( wstrUserID );
    if( mit == m_mapUserID.end() )
    {
        return false;
    }

    if( !mit->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( wstrUserID )
            << END_LOG;

        return false;
    }

    kInfo = *( mit->second );
    return true;
}

bool KUserList::GetGSUserInfoByUnitUID( UidType iUnitUID, KGSUserInfo& kInfo )
{
    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mit;

    mit = m_mapUnitUID.find( iUnitUID );
    if( mit == m_mapUnitUID.end() )
    {
        return false;
    }

    if( !mit->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( iUnitUID )
            << END_LOG;

        return false;
    }

    kInfo = *( mit->second );
    return true;
}

bool KUserList::GetGSUserInfoByUnitNickName( std::wstring& wstrUnitNickName, KGSUserInfo& kInfo )
{
	//To do..
	//추후 찾는 유저의 uid(first_sender)를 받아와 서버군코드가 같은 유저의 닉네임을 찾게 수정
	//그럼 키값이 멀티로 서버군코드, 닉네임이 되어야 겠다.

    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mit;

    mit = m_mapUnitNickName.find( wstrUnitNickName );
    if( mit == m_mapUnitNickName.end() )
    {
        return false;
    }

    if( !mit->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( wstrUnitNickName )
            << END_LOG;

        return false;
    }

    kInfo = *( mit->second );
    return true;
}


//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
bool KUserList::GetGSUserInfoByMachineID( IN const std::string& strMachineID, KUserList::KGSUserInfo& kInfo )
{
	std::map< std::string, std::shared_ptr< KGSUserInfo > >::iterator mit;

	mit = m_mapMachineID.find( strMachineID );
	if( mit == m_mapMachineID.end() )
	{
		return false;
	}

	if( !mit->second )
	{
		START_LOG( cerr, L"Invalid pointer." )
			<< BUILD_LOG( strMachineID )
			<< END_LOG;

		return false;
	}

	kInfo = *( mit->second );
	return true;
}
//}}

int KUserList::RegUser( IN const UidType iUserUID, IN const UidType iGSUID, IN const KELG_REGISTER_USER_REQ& kInfo_, IN const bool bChannelChanged )
{
	// 1. 전달인자 값 검사
    if( iUserUID <= 0 )
    {
        START_LOG( cerr, L"Invalid user UID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_01;
    }

    if( kInfo_.m_wstrUserID.empty() )
    {
        START_LOG( cerr, L"Invalid user ID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_02;
    }

    if( iGSUID <= 0 )
    {
        START_LOG( cerr, L"Invalid game server UID." )
            << BUILD_LOG( iGSUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_05;
    }

	// 2. 유저 정보 생성
    std::shared_ptr< KGSUserInfo > spGSUserInfo( new KGSUserInfo() );
    spGSUserInfo->m_nUserUID	 = iUserUID;
    spGSUserInfo->m_wstrUserID	 = kInfo_.m_wstrUserID;
    spGSUserInfo->m_nGSUID		 = iGSUID;
	spGSUserInfo->m_iChannelID	 = kInfo_.m_iChannelID; // 채널ID
    spGSUserInfo->m_uiIP		 = kInfo_.m_uiIP;
    spGSUserInfo->m_byteCanTry	 = kInfo_.m_byteCanTry;
	spGSUserInfo->m_bIsGuestUser = IsGuestUser( kInfo_.m_byteGuestUser ); // 체험ID 제한
	spGSUserInfo->m_cAuthLevel	 = kInfo_.m_cAuthLevel;
	//{{ 2010. 07. 01  최육사	PC방 인증 서버 개편
	spGSUserInfo->m_bChannelChanged = bChannelChanged;
	//}}
	//{{ 2011. 07. 27    김민성    투니랜드 채널링
	//{{ 2012. 04. 02	김민성		넥슨 auth soap
	//}}

	//}}

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	spGSUserInfo->m_strMachineID = kInfo_.m_strMachineID;
	//}}

    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUserUID;
    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mitUserID;
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	// 중복되는 머신ID가 있는지 확인!
	if( KSimLayer::GetKObj()->GetVersionFlag() != KSimLayer::VF_OPEN_TEST )		// 오픈 테섭에서는 검사 제외
	{
		if( SiKGameSysVal()->IsMachineIDDuplicateCheck() == true )
		{
			if( kInfo_.m_strMachineID.empty() )
			{
				START_LOG( cout, L"[Notice] Found a user with an empty machine ID!" )
					<< BUILD_LOG( kInfo_.m_wstrUserID )
					<< BUILD_LOGc( kInfo_.m_cAuthLevel )
					<< BUILD_LOG( kInfo_.m_iChannelID )
					<< BUILD_LOG( kInfo_.m_strMachineID );
			}
			else
			{
				if( m_mapMachineID.find( kInfo_.m_strMachineID ) != m_mapMachineID.end() )
				{
					// 운영자급 이상이 아니면 머신ID로 중복 접속한것인지 체크한다!
					if( kInfo_.m_cAuthLevel < SEnum::UAL_GM )
					{
						// 사내서버가 아닐때만 검사하기!
						if( KSimLayer::GetKObj()->GetAuthFlag() != KSimLayer::AF_INTERNAL )
						{
							//{{ 2012. 10. 12	최육사		2중 접속 버그 수정
							KGSUserInfo kUserInfo;
							if( GetGSUserInfoByMachineID( kInfo_.m_strMachineID, kUserInfo ) == true )
							{
								KLoginUserPtr spServer;
								KActorManager::GetKObj()->Get( kUserInfo.m_nGSUID, spServer );
								if( IS_NOT_NULL( spServer ) )
								{
									START_LOG( cout, L"[Notice] Duplicate machine ID login! Disconnecting the existing session!" )
										<< BUILD_LOG( kUserInfo.m_nUserUID )
										<< BUILD_LOG( kUserInfo.m_wstrUserID )
										<< BUILD_LOG( kInfo_.m_wstrUserID )
										<< BUILD_LOG( kUserInfo.m_strMachineID );

									// 이전에 접속했던 유저를 튕겨내도록 한다.
									int kPacket = NetError::ERR_USER_REGISTRATION_10;
									KEvent kEvent;
									kEvent.SetData( PI_GS_USER, NULL, E_KICK_USER_NOT, kPacket );
									LIF( kEvent.m_kDestPerformer.AddUID( iUserUID ) );
									spServer->KSession::SendPacket( kEvent );
								}
							}
							//}}

							START_LOG( cout, L"[Notice] Multiple users with the same machine ID! Possible hacking?" )
								<< BUILD_LOG( kInfo_.m_wstrUserID )
								<< BUILD_LOG( kInfo_.m_strMachineID );

							// 현재 접속중인 유저를 튕겨낸다.
							return NetError::ERR_USER_REGISTRATION_10;
						}
						else
						{
							START_LOG( cout, L"[Notice] Duplicate machine login by a regular account accepted because this is an in-house server version." )
								<< BUILD_LOG( kInfo_.m_wstrUserID )
								<< BUILD_LOGc( kInfo_.m_cAuthLevel )
								<< BUILD_LOG( kInfo_.m_iChannelID )
								<< BUILD_LOG( kInfo_.m_strMachineID );
						}
					}
					else
					{
						START_LOG( cout, L"[Notice] An operator or developer attempted to log in twice from the same machine. Please investigate." )
							<< BUILD_LOG( kInfo_.m_wstrUserID )
							<< BUILD_LOGc( kInfo_.m_cAuthLevel )
							<< BUILD_LOG( kInfo_.m_iChannelID )
							<< BUILD_LOG( kInfo_.m_strMachineID );
					}
				}
			}
		}
	}
	//}}

	// 3. 이미 존재하는 유저 정보인지 확인
    mitUserUID = m_mapUserUID.find( iUserUID );
    if( mitUserUID != m_mapUserUID.end() )
    {
        START_LOG( cerr, L"User information exists." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        if( !mitUserUID->second )
        {
            START_LOG( cerr, L"Invalid pointer." )
                << BUILD_LOG( iUserUID )
                << END_LOG;

            // 일어나서는 안되는 상황. 그냥 새 정보로 덮어 쓰자.
            m_mapUserUID.erase( mitUserUID );
            m_mapUserUID.insert( std::make_pair( iUserUID, spGSUserInfo ) );

            mitUserID = m_mapUserID.find( kInfo_.m_wstrUserID );
            if( mitUserID != m_mapUserID.end() )
            {
                if( mitUserID->second )
                {
                    START_LOG( cerr, L"Pointer is valid." )
                        << BUILD_LOG( iUserUID )
                        << BUILD_LOG( mitUserID->first )
                        << BUILD_LOG( mitUserID->second->m_nUserUID )
                        << END_LOG;
                }
                m_mapUserID.erase( mitUserID );
            }
            m_mapUserID.insert( std::make_pair( kInfo_.m_wstrUserID, spGSUserInfo ) );
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
			if( kInfo_.m_strMachineID.empty() == false )
			{
				m_mapMachineID.insert( std::make_pair( kInfo_.m_strMachineID, spGSUserInfo ) );
			}
			//}}
            goto end_proc;
        }

        if( mitUserUID->second->m_nGSUID == iGSUID )
        {
            START_LOG( cerr, L"Duplicate connection to the same game server." )  // 이미 검사를 한 경우이므로 일어날 수 없다.
                << BUILD_LOG( iUserUID )                    // 아니면 이전 문제로 정보가 안지워진 경우이다.
                << BUILD_LOG( kInfo_.m_wstrUserID )
                << BUILD_LOG( iGSUID )
                << END_LOG;

            mitUserUID->second.reset();
            mitUserUID->second = spGSUserInfo;
            mitUserID = m_mapUserID.find( kInfo_.m_wstrUserID );
            if( mitUserID != m_mapUserID.end() )
            {
                START_LOG( cerr, L"UserID exists in the map." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( kInfo_.m_wstrUserID )
                    << BUILD_LOG( iGSUID )
                    << END_LOG;

                m_mapUserID.erase( mitUserID );
            }
            else
            {
                START_LOG( cerr, L"UserID is not in the map." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( kInfo_.m_wstrUserID )
                    << BUILD_LOG( iGSUID )
                    << END_LOG;
            }
            m_mapUserID.insert( std::make_pair( kInfo_.m_wstrUserID, spGSUserInfo ) );
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
			if( kInfo_.m_strMachineID.empty() == false )
			{
				m_mapMachineID.insert( std::make_pair( kInfo_.m_strMachineID, spGSUserInfo ) );
			}
			//}}
            goto end_proc;
        }

        KLoginUserPtr spServer;
        KActorManager::GetKObj()->Get( mitUserUID->second->m_nGSUID, spServer );
        if( spServer )   // 타 서버간 이중접속
        {
            START_LOG( cerr2, L"Duplicate connection detected across servers." )
                << BUILD_LOG( spServer->GetName() )
                << BUILD_LOG( iUserUID )
                << BUILD_LOG( kInfo_.m_wstrUserID )
                << END_LOG;

            // 이전에 접속했던 유저를 튕겨내도록 한다.
            int kPacket = NetError::ERR_VERIFY_01;
            KEvent kEvent;
            kEvent.SetData( PI_GS_USER, NULL, E_KICK_USER_NOT, kPacket );
            LIF( kEvent.m_kDestPerformer.AddUID( iUserUID ) );
            spServer->KSession::SendPacket( kEvent );

            // 현재 접속하려는 유저도 튕겨낸다.
            return NetError::ERR_USER_REGISTRATION_00;
        }
        else    // 이전에 접속해있었다는 서버가 연결되어있지 않음. 서버 다운 등으로 잘못된 데이터가 남아있던 경우.
        {
            KLoginUserPtr spServerCurrent;
            KActorManager::GetKObj()->Get( iGSUID, spServerCurrent );
            if( !spServerCurrent )
            {
                // 서버가 죽기 직전 인증 요청을 한 것일까? 어쨌든 이 유저를 리스트에 추가하면 안된다.
                START_LOG( cerr, L"The game server for the user making the authentication request is not connected." )
                    << BUILD_LOG( iGSUID )
                    << BUILD_LOG( iUserUID )
                    << END_LOG;

                return NetError::ERR_USER_REGISTRATION_07;
            }

            mitUserUID->second.reset();
            mitUserUID->second = spGSUserInfo;

            mitUserID = m_mapUserID.find( kInfo_.m_wstrUserID );
            if( mitUserID != m_mapUserID.end() )
            {
                START_LOG( cerr, L"UserID exists in the map." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( kInfo_.m_wstrUserID )
                    << BUILD_LOG( iGSUID )
                    << END_LOG;

                m_mapUserID.erase( mitUserID );
            }
            else
            {
                START_LOG( cerr, L"UserID is not in the map." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( kInfo_.m_wstrUserID )
                    << BUILD_LOG( iGSUID )
                    << END_LOG;
            }

            m_mapUserID.insert( std::make_pair( kInfo_.m_wstrUserID, spGSUserInfo ) );
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
			if( kInfo_.m_strMachineID.empty() == false )
			{
				m_mapMachineID.insert( std::make_pair( kInfo_.m_strMachineID, spGSUserInfo ) );
			}
			//}}
            goto end_proc;
        }
    }

	// 4. 유저 정보 저장
    m_mapUserUID.insert( std::make_pair( iUserUID, spGSUserInfo ) );
    mitUserID = m_mapUserID.find( kInfo_.m_wstrUserID );
    if( mitUserID != m_mapUserID.end() )
    {
        START_LOG( cerr, L"UserUID is not in the map, but UserID is in the map." )
            << BUILD_LOG( iUserUID )
            << BUILD_LOG( kInfo_.m_wstrUserID )
            << END_LOG;

        m_mapUserID.erase( mitUserID );
    }
    m_mapUserID.insert( std::make_pair( kInfo_.m_wstrUserID, spGSUserInfo ) );
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	if( kInfo_.m_strMachineID.empty() == false )
	{
		m_mapMachineID.insert( std::make_pair( kInfo_.m_strMachineID, spGSUserInfo ) );
	}
	//}}


	// 5. 현재 동접 저장
	SetConcurrentUserCount( m_mapUserUID.size() );
    goto end_proc;

end_proc:
    // 6. 인증 서버 로그인 - 정상적인 유저 등록이면 여기로 온다.
	//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
	RegAuthWaitUser( *spGSUserInfo );
	//}}

    return NetError::NET_OK;
}

bool KUserList::UnRegUser( IN const UidType iUserUID )
{
	// 1. 전달인자 검사
    if( iUserUID <= 0 )
    {
        START_LOG( cerr, L"Invalid user UID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUserUID;
    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mitUserID;

	// 2. 삭제하려는 유저가 있는지 검사
    mitUserUID = m_mapUserUID.find( iUserUID );
    if( mitUserUID == m_mapUserUID.end() )
    {
        START_LOG( cerr, L"User to delete was not found." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    if( !mitUserUID->second )
    {
        START_LOG( cerr, L"Invalid user information pointer." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        m_mapUserUID.erase( mitUserUID );
        return false;
    }

    // 유닛이 등록되어 있으면 삭제한다.
    // -> 유저 객체 삭제 시에 유닉 삭제를 거치지 않고 유저 삭제를 해왔으나 꼭 유닛 삭제를 먼저 하도록 변경함.
    // LIF( !UnRegUnit( iUserUID ) );

	//{{ 2009. 7. 9  최육사		유닛정보 먼저 지우고 유저정보 지워야 한다!
	// 3. 유닛 정보가 아직 남아있는지 검사
	if( mitUserUID->second->m_nUnitUID > 0 )
	{
		START_LOG( cerr, L"Attempted to delete user information before deleting unit information!" )
			<< BUILD_LOG( iUserUID )
			<< BUILD_LOG( mitUserUID->second->m_nUnitUID )
			<< END_LOG;
	}
	//}}

    KGSUserInfo kInfo = *( mitUserUID->second );

	// 4. 유저 정보 삭제
    if( kInfo.m_wstrUserID.empty() )
    {
        START_LOG( cerr, L"Invalid user ID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;
    }
    else
    {
        mitUserID = m_mapUserID.find( kInfo.m_wstrUserID );
        if( mitUserID == m_mapUserID.end() )
        {
            START_LOG( cerr, L"Failed to find user by ID." )
                << BUILD_LOG( kInfo.m_wstrUserID )
                << BUILD_LOG( iUserUID )
                << END_LOG;
        }
        else
        {
            m_mapUserID.erase( mitUserID );
        }
    }

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	// 머신ID정보 삭제
	if( kInfo.m_strMachineID.empty() )
	{
		START_LOG( cout, L"[Notice] A user with an empty machine ID logged out!" )
			<< BUILD_LOG( kInfo.m_wstrUserID )
			<< BUILD_LOGc( kInfo.m_cAuthLevel )
			<< BUILD_LOG( kInfo.m_iChannelID );
	}
	else
	{
		std::map< std::string, std::shared_ptr< KGSUserInfo > >::iterator mitMachineID;
		mitMachineID = m_mapMachineID.find( kInfo.m_strMachineID );
		if( mitMachineID == m_mapMachineID.end() )
		{
			START_LOG( cerr, L"Failed to find user by machine ID." )
				<< BUILD_LOG( kInfo.m_strMachineID )
				<< BUILD_LOG( iUserUID )
				<< END_LOG;
		}
		else
		{
			m_mapMachineID.erase( mitMachineID );
		}
	}
	//}}

	//{{ 2011. 09. 27	김민성	PC 방 오과금 방지
	//}}

    m_mapUserUID.erase( mitUserUID );


	// 5. 현재 동접 저장
	SetConcurrentUserCount( m_mapUserUID.size() );



    return true;
}

void KUserList::UnRegAllUserOfThisGS( IN const UidType iGSUID )
{
    START_LOG( cout, L"Handling after game server disconnect." )
        << BUILD_LOG( iGSUID );

    if( iGSUID <= 0 )
    {
        START_LOG( cerr, L"Invalid server UID." )
            << BUILD_LOG( iGSUID )
            << END_LOG;

        return;
    }

    std::vector< UidType > vecUserUID;
    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUserUID;
    for( mitUserUID = m_mapUserUID.begin(); mitUserUID != m_mapUserUID.end(); mitUserUID++ )
    {
        if( !mitUserUID->second )
        {
            START_LOG( cerr, L"Invalid user information pointer." )
                << BUILD_LOG( mitUserUID->first )
                << END_LOG;

            continue;
        }

        if( mitUserUID->second->m_nGSUID == iGSUID )
        {
            vecUserUID.push_back( mitUserUID->first );
        }
    }

    std::vector< UidType >::iterator vitUserUID;
    for( vitUserUID = vecUserUID.begin(); vitUserUID != vecUserUID.end(); vitUserUID++ )
    {
        UnRegUser( *vitUserUID );
    }

    return;
}

int KUserList::RegUnit( IN const UidType iUserUID, IN const UidType iUnitUID, IN const std::wstring& wstrUnitNickName, IN const unsigned int uiKNMSerialNum, IN const bool bDenyFriendShip )
{
    if( iUserUID <= 0 )
    {
        START_LOG( cerr, L"Invalid user UID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_01;
    }

    if( iUnitUID <= 0 )
    {
        START_LOG( cerr, L"Invalid unit UID." )
            << BUILD_LOG( iUserUID )
            << BUILD_LOG( iUnitUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_03;
    }

    if( wstrUnitNickName.empty() )
    {
        START_LOG( cerr, L"Invalid unit nickname." )
            << BUILD_LOG( iUserUID )
            << BUILD_LOG( iUnitUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_04;
    }

	if( uiKNMSerialNum <= 0  )
	{
		START_LOG( cerr, L"Invalid unit messenger SN." )
			<< BUILD_LOG( iUserUID )
			<< BUILD_LOG( iUnitUID )
			<< BUILD_LOG( uiKNMSerialNum )
			<< END_LOG;

		return NetError::ERR_USER_REGISTRATION_09;
	}

    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUserUID;
    mitUserUID = m_mapUserUID.find( iUserUID );
    if( mitUserUID == m_mapUserUID.end() )
    {
        START_LOG( cerr, L"Attempted to register a unit whose account is not registered." )
            << BUILD_LOG( iUserUID )
            << BUILD_LOG( iUnitUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_08;
    }

    if( !mitUserUID->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( iUserUID )
            << BUILD_LOG( iUnitUID )
            << END_LOG;

        return NetError::ERR_USER_REGISTRATION_06;
    }

	//UID로 검색
    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUnitUID;
    mitUnitUID = m_mapUnitUID.find( mitUserUID->second->m_nUnitUID );
    if( mitUnitUID != m_mapUnitUID.end() )
    {
        START_LOG( cerr, L"Unit has not been unregistered." )
            << BUILD_LOG( mitUnitUID->first )
            << END_LOG;

        if( !mitUnitUID->second )
        {
            START_LOG( cerr, L"Invalid pointer." )
                << BUILD_LOG( iUserUID )
                << BUILD_LOG( iUnitUID )
                << BUILD_LOG( wstrUnitNickName )
                << END_LOG;
        }
        else
        {
            LIF( mitUnitUID->second->m_nUserUID == iUserUID );
        }
        m_mapUnitUID.erase( mitUnitUID );
    }
    m_mapUnitUID.insert( std::make_pair( iUnitUID, mitUserUID->second ) );

	//닉네임 검색
    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mitUnitNickName;
    mitUnitNickName = m_mapUnitNickName.find( mitUserUID->second->m_wstrUnitNickName );
    if( mitUnitNickName != m_mapUnitNickName.end() )
    {
        START_LOG( cerr, L"Unit nickname has not been unregistered." )
            << BUILD_LOG( mitUnitNickName->first )
            << END_LOG;

        if( !mitUnitNickName->second )
        {
            START_LOG( cerr, L"Invalid pointer." )
                << BUILD_LOG( iUserUID )
                << BUILD_LOG( iUnitUID )
                << BUILD_LOG( wstrUnitNickName )
                << END_LOG;
        }
        else
        {
            LIF( mitUnitNickName->second->m_nUserUID == iUserUID );
        }
        m_mapUnitNickName.erase( mitUnitNickName );
    }
    m_mapUnitNickName.insert( std::make_pair( wstrUnitNickName, mitUserUID->second ) );

	//기존 데이터 검사를 마친후 오류처리를 한다음 데이터 셋팅
	//포인터 이기때문에 한곳에만 하면된다.
	mitUserUID->second->m_nUnitUID			= iUnitUID;
	mitUserUID->second->m_wstrUnitNickName	= wstrUnitNickName;
	mitUserUID->second->m_uiKNMSerialNum	= uiKNMSerialNum;
	//{{ 2009. 4. 1  최육사		친구차단
	mitUserUID->second->m_bDenyFriendShip	= bDenyFriendShip;
	//}}

    return NetError::NET_OK;
}

bool KUserList::UnRegUnit( IN const UidType iUserUID )
{
    if( iUserUID <= 0 )
    {
        START_LOG( cerr, L"Invalid user UID." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUserUID;
    mitUserUID = m_mapUserUID.find( iUserUID );
    if( mitUserUID == m_mapUserUID.end() )
    {
        START_LOG( cerr, L"Attempted to delete a unit whose account is not registered." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    if( !mitUserUID->second )
    {
        START_LOG( cerr, L"Invalid pointer." )
            << BUILD_LOG( iUserUID )
            << END_LOG;

        return false;
    }

    std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mitUnitUID;
    if( mitUserUID->second->m_nUnitUID > 0 )
    {
        mitUnitUID = m_mapUnitUID.find( mitUserUID->second->m_nUnitUID );
        if( mitUnitUID == m_mapUnitUID.end() )
        {
            START_LOG( cerr, L"Failed to find unit by UID." )
                << BUILD_LOG( mitUserUID->second->m_nUnitUID )
                << END_LOG;
        }
        else
        {
            if( !mitUnitUID->second )
            {
                START_LOG( cerr, L"Invalid pointer." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( mitUserUID->second->m_nUnitUID )
                    << END_LOG;
            }
            else
            {
                LIF( mitUnitUID->second->m_nUserUID == iUserUID );
            }
            m_mapUnitUID.erase( mitUnitUID );
        }

        if( mitUserUID->second->m_wstrUnitNickName.empty() )
        {
            START_LOG( cerr, L"Unit UID is registered, but nickname is not." )
                << BUILD_LOG( iUserUID )
                << BUILD_LOG( mitUserUID->second->m_nUnitUID )
                << END_LOG;
        }
    }
    else
    {
        if( !mitUserUID->second->m_wstrUnitNickName.empty() )
        {
            START_LOG( cerr, L"Unit UID is not registered, but nickname is." )
                << BUILD_LOG( iUserUID )
                << BUILD_LOG( mitUserUID->second->m_wstrUnitNickName )
                << END_LOG;
        }
    }

    std::map< std::wstring, std::shared_ptr< KGSUserInfo > >::iterator mitUnitNickName;
    if( !mitUserUID->second->m_wstrUnitNickName.empty() )
    {
        mitUnitNickName = m_mapUnitNickName.find( mitUserUID->second->m_wstrUnitNickName );
        if( mitUnitNickName == m_mapUnitNickName.end() )
        {
            START_LOG( cerr, L"Failed to find unit by nickname." )
                << BUILD_LOG( mitUserUID->second->m_wstrUnitNickName )
                << END_LOG;
        }
        else
        {
            if( !mitUnitNickName->second )
            {
                START_LOG( cerr, L"Invalid pointer." )
                    << BUILD_LOG( iUserUID )
                    << BUILD_LOG( mitUserUID->second->m_wstrUnitNickName )
                    << END_LOG;
            }
            else
            {
                LIF( mitUnitNickName->second->m_nUserUID == iUserUID );
            }
            m_mapUnitNickName.erase( mitUnitNickName );
        }
    }

	mitUserUID->second->m_nUnitUID = 0;
	mitUserUID->second->m_wstrUnitNickName.clear();
    mitUserUID->second->m_uiKNMSerialNum = 0;
	//{{ 2009. 4. 1  최육사		친구차단
	mitUserUID->second->m_bDenyFriendShip = false;
	//}}

    return true;
}

//{{ 2009. 4. 1  최육사		친구 차단
bool KUserList::SetDenyFriendShip( UidType iUnitUID, bool bDenyFriendShip )
{
	std::map< UidType, std::shared_ptr< KGSUserInfo > >::iterator mit;

	mit = m_mapUnitUID.find( iUnitUID );
	if( mit == m_mapUnitUID.end() )
	{
		return false;
	}

	if( !mit->second )
	{
		// 시간차로 발생가능.
		START_LOG( cwarn, L"Invalid pointer." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;

		return false;
	}

	( mit->second )->m_bDenyFriendShip = bDenyFriendShip;
	return true;
}
//}}

//{{ 2009. 11. 16  최육사	 체험ID동접
void KUserList::DecreaseGuestConcurrentUser()
{
	--m_iGuestConcurrentUser;

	if( m_iGuestConcurrentUser < 0 )
	{
		m_iGuestConcurrentUser = 0;
	}
}
//}}

//{{ 2011. 09. 27	김민성	PC 방 오과금 방지
//}}

//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
bool KUserList::RegAuthWaitUser( IN const KUserList::KGSUserInfo& kUserInfo )
{
	if( IsEnableAuthWait() == false )
		return true;

	std::map< UidType, KAuthWaitUser >::iterator mit;
	mit = m_mapAuthWaitUser.find( kUserInfo.m_nUserUID );
	if( mit != m_mapAuthWaitUser.end() )
	{
		START_LOG( cerr, L"User is already registered as awaiting authentication!" )
			<< BUILD_LOG( kUserInfo.m_nUserUID )
			<< BUILD_LOG( kUserInfo.m_wstrUserID )
			<< END_LOG;
		return false;
	}

	KAuthWaitUser kAuthWaitUser;
	kAuthWaitUser.m_iUserUID = kUserInfo.m_nUserUID;
	m_mapAuthWaitUser.insert( std::make_pair( kUserInfo.m_nUserUID, kAuthWaitUser ) );
	return true;
}

void KUserList::UnRegAuthWaitUser( IN const UidType iUserUID )
{
	m_mapAuthWaitUser.erase( iUserUID );
}

void KUserList::Tick()
{
	if( m_tCheckAuthWaitTimer.elapsed() < 1.0 )
		return;

	m_tCheckAuthWaitTimer.restart();

	std::vector< UidType > vecDeleteUserList;

	std::map< UidType, KAuthWaitUser >::const_iterator mit;
	for( mit = m_mapAuthWaitUser.begin(); mit != m_mapAuthWaitUser.end(); ++mit )
	{
		if( mit->second.m_tTimer.elapsed() > 30.0 )	// 해외는 통신 느려서 30초까지 대기 시간 늘림
		{
			vecDeleteUserList.push_back( mit->first );
		}
	}

	for( const UidType iUserUID : vecDeleteUserList )
	{
		// 일단 먼저 지우고
		UnRegAuthWaitUser( iUserUID );

		KUserList::KGSUserInfo kInfo;
		if( GetGSUserInfoByUserUID( iUserUID, kInfo ) == false )
		{
			START_LOG( cerr, L"Failed to find the user's game server." )
				<< BUILD_LOG( iUserUID )
				<< END_LOG;
			continue;
		}

		START_LOG( cout, L"Nexon PC-bang authentication server: authentication timed out; forcing the game to start!" )
			<< BUILD_LOG( kInfo.m_wstrUserID );

		KENX_AUTH_LOGIN_ACK kPacketAuth;
		kPacketAuth.m_wstrUserID = kInfo.m_wstrUserID;
		kPacketAuth.m_byteAuthorizeResult = 2; // Trial - 프리미엄 없이 게임을 실행한다.

		//{{ 2010. 07. 01  최육사	PC방 인증 서버 개편
		if( kInfo.m_bChannelChanged )
		{
			UidType anTrace[2] = { kInfo.m_nGSUID, -1 };
			KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_USER, kInfo.m_nUserUID, anTrace, ELG_REGISTER_NEXON_CHANNEL_CHANGE_USER_ACK, kPacketAuth );
		}
		else
		{
			UidType anTrace[2] = { kInfo.m_nGSUID, -1 };
			KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_USER, kInfo.m_nUserUID, anTrace, ELG_REGISTER_NEXON_USER_ACK, kPacketAuth );
		}
		//}}
	}
}
//}}
