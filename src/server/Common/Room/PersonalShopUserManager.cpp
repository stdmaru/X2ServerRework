#include "./PersonalShopUserManager.h"

#include "NetError.h"

KPersonalShopUserManager::KPersonalShopUserManager(void)
{
}

KPersonalShopUserManager::~KPersonalShopUserManager(void)
{
	Reset();
}

void KPersonalShopUserManager::Init( int nMaxUser )
{
	m_dwMaxUser	= ( DWORD )nMaxUser;
}

void KPersonalShopUserManager::Reset()
{
	m_mapPersonalShopUser.clear();
	//{{ 2011. 04. 14	최육사	대리 상인
	m_spPShopAgencyUser.reset();
	//}}
}

KPersonalShopUserPtr KPersonalShopUserManager::GetUser( UidType nCID )
{
	std::map< UidType, KPersonalShopUserPtr >::iterator mit;
	mit = m_mapPersonalShopUser.find( nCID );

	if( mit == m_mapPersonalShopUser.end() )
	{
		START_LOG( cwarn, L"User does not exist." )
			<< BUILD_LOG( nCID )
			<< END_LOG;

		return KPersonalShopUserPtr();
	}

	return mit->second;
}

bool KPersonalShopUserManager::AddUser( KPersonalShopUserPtr spPersonalShopUser )
{
	if( !spPersonalShopUser )
	{
		START_LOG( cerr, L"Invalid user pointer." )
			<< END_LOG;

		return false;
	}

	std::map< UidType, KPersonalShopUserPtr >::iterator mit;
	mit = m_mapPersonalShopUser.find( spPersonalShopUser->GetCID() );

	if( mit != m_mapPersonalShopUser.end() )
	{
		START_LOG( cerr, L"Personal shop user already exists." )
			<< BUILD_LOG( spPersonalShopUser->GetCID() )
			<< END_LOG;

		return false;
	}

	m_mapPersonalShopUser.insert( std::make_pair( spPersonalShopUser->GetCID(), spPersonalShopUser ) );
	return true;
}

bool KPersonalShopUserManager::DeleteUser( UidType nCID )
{
	std::map< UidType, KPersonalShopUserPtr >::iterator mit;
	mit = m_mapPersonalShopUser.find( nCID );

	if( mit == m_mapPersonalShopUser.end() )
	{
		START_LOG( cerr, L"Personal shop user does not exist." )
			<< BUILD_LOG( nCID )
			<< END_LOG;

		return true;
	}

	m_mapPersonalShopUser.erase( mit );
	return true;
}

int KPersonalShopUserManager::DeleteUserByGsUID( IN UidType nGsUID, OUT std::vector< UidType >& vecDelUserUID )
{
	int iCount = 0;

	std::map< UidType, KPersonalShopUserPtr >::iterator mit;
	for( mit = m_mapPersonalShopUser.begin(); mit != m_mapPersonalShopUser.end(); mit++ )
	{
		if( !mit->second )
		{
			START_LOG( cerr, L"Invalid personal shop user pointer." )
				<< BUILD_LOG( mit->first )
				<< END_LOG;

			continue;
		}

		if( mit->second->GetGSUID() == nGsUID )
		{
			//삭제될 user의 uid를 담는다.
			vecDelUserUID.push_back( mit->second->GetCID() );
			iCount++;
		}
	}

	return iCount;
}

bool KPersonalShopUserManager::EnterPersonalShop( IN const KPersonalShopUserInfo& kInfo )
{
	SET_ERROR( NET_OK );

	KPersonalShopUserPtr spPersonalShopUser = MakeUser( kInfo );
	if( !spPersonalShopUser )
	{
		//{{ 2009. 8. 25  최육사	코드 정리
		if( NetError::GetLastError() == NetError::ERR_PERSONAL_SHOP_04 )
		{
			START_LOG( cwarn, L"Failed to create personal shop user: shop capacity exceeded." )
				<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
				<< END_LOG;
		}
		else
		{
			START_LOG( cerr, L"Failed to create personal shop user." )
				<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
				<< END_LOG;
		}
		//}}

		return false;
	}

	if( AddUser( spPersonalShopUser ) == false )
	{
		START_LOG( cerr, L"Failed to add personal shop user." )
			<< BUILD_LOG( spPersonalShopUser->GetCID() )
			<< END_LOG;

		SET_ERROR( ERR_PERSONAL_SHOP_05 );
		return false;
	}

	//상점 개설자를 체크해놓는다.
	if( m_mapPersonalShopUser.size() == 1 )
		spPersonalShopUser->SetHost( true );

	return true;
}

//{{ 2011. 05. 13	최육사	대리 상인 좀비 유저 버그 해결
bool KPersonalShopUserManager::EnterPShopAgencyOwner( IN const KPersonalShopUserInfo& kInfo )
{
	SET_ERROR( NET_OK );

    {
        KPersonalShopUserPtr spExistPersonalShopUser = GetUser( kInfo.m_iUnitUID );
        if ( spExistPersonalShopUser )
        {
            DeleteUser( kInfo.m_iUnitUID );
        }
    }

	KPersonalShopUserPtr spPersonalShopUser = MakeUser( kInfo, true );
	if( !spPersonalShopUser )
	{
		START_LOG( cerr, L"Failed to create proxy shop owner user." )
			<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
			<< END_LOG;
		return false;
	}

	if( AddUser( spPersonalShopUser ) == false )
	{
		START_LOG( cerr, L"Failed to add personal shop user." )
			<< BUILD_LOG( spPersonalShopUser->GetCID() )
			<< END_LOG;

		SET_ERROR( ERR_PERSONAL_SHOP_05 );
		return false;
	}

	//상점 개설자를 체크해놓는다.
	spPersonalShopUser->SetHost( true );
	return true;
}

bool KPersonalShopUserManager::EnterPShopAgencyCustomer( IN const KPersonalShopUserInfo& kInfo )
{
	SET_ERROR( NET_OK );

    {
        KPersonalShopUserPtr spExistPersonalShopUser = GetUser( kInfo.m_iUnitUID );
        if ( spExistPersonalShopUser )
        {
            DeleteUser( kInfo.m_iUnitUID );
        }
    }

	KPersonalShopUserPtr spPersonalShopUser = MakeUser( kInfo );
	if( !spPersonalShopUser )
	{
		//{{ 2009. 8. 25  최육사	코드 정리
		if( NetError::GetLastError() == NetError::ERR_PERSONAL_SHOP_04 )
		{
			START_LOG( cwarn, L"Failed to create personal shop user: shop capacity exceeded." )
				<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
				<< END_LOG;
		}
		else
		{
			START_LOG( cerr, L"Failed to create personal shop user." )
				<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
				<< END_LOG;
		}
		//}}

		return false;
	}

	if( AddUser( spPersonalShopUser ) == false )
	{
		START_LOG( cerr, L"Failed to add personal shop user." )
			<< BUILD_LOG( spPersonalShopUser->GetCID() )
			<< END_LOG;

		SET_ERROR( ERR_PERSONAL_SHOP_05 );
		return false;
	}

	return true;
}
//}}

bool KPersonalShopUserManager::LeavePersonalShop( UidType nCID )
{
	KPersonalShopUserPtr spPersonalShopUser = GetUser( nCID );
	if( !spPersonalShopUser )
	{
		START_LOG( cwarn, L"User not found." )
			<< BUILD_LOG( nCID )
			<< END_LOG;

		return true;
	}

	return DeleteUser( nCID );
}

//{{ 2011. 04. 14	최육사	대리 상인

bool KPersonalShopUserManager::CreatePersonalShopAgency( IN const KPersonalShopUserInfo& kInfo )
{
	SET_ERROR( NET_OK );

	m_spPShopAgencyUser = MakeUser( kInfo );
	if( m_spPShopAgencyUser == NULL )
	{
		START_LOG( cerr, L"Failed to create proxy shop user." )
			<< BUILD_LOG( ( int )m_mapPersonalShopUser.size() )
			<< END_LOG;
		return false;
	}

	return true;
}

bool KPersonalShopUserManager::DeletePersonalShopAgency( IN UidType nCID )
{
	if( IsPShopAgencyHost( nCID ) == false )
	{
		START_LOG( cerr, L"Attempted to close a proxy shop without being its owner." )
			<< BUILD_LOG( nCID )
			<< END_LOG;
		return false;
	}

	m_spPShopAgencyUser.reset();
	return true;
}
//}}

bool KPersonalShopUserManager::GetPersonalShopUserGS( UidType nCID, UidType& nGSUID )
{
	std::map< UidType, KPersonalShopUserPtr >::iterator mit;

	mit = m_mapPersonalShopUser.find( nCID );

	if( mit != m_mapPersonalShopUser.end() )
	{
		if( mit->second != NULL )
		{
			nGSUID = mit->second->GetGSUID();

			return true;
		}
		else
		{
			START_LOG( cerr, L"Invalid personal shop user pointer." )
				<< BUILD_LOG( nCID )
				<< END_LOG;
		}

	}
	else
	{
		START_LOG( cwarn, L"Personal shop user not found." )
			<< BUILD_LOG( nCID )
			<< END_LOG;
	}

	return false;
}

void KPersonalShopUserManager::GetUserList( OUT std::map< UidType, std::set< UidType > >& mapUidList_ )
{
	mapUidList_.clear();

	std::map< UidType, KPersonalShopUserPtr >::iterator mit;
	for( mit = m_mapPersonalShopUser.begin(); mit != m_mapPersonalShopUser.end(); mit++ )
	{
		if( !mit->second )
		{
			START_LOG( cerr, L"Invalid room user pointer." )
				<< BUILD_LOG( mit->first )
				<< END_LOG;

			continue;
		}

		// operator[]()의 의도적인 사용. 데이터가 없을 경우 insert되는 것을 인정한다.
		mapUidList_[mit->second->GetGSUID()].insert( mit->second->GetCID() );
	}
}

//{{ 2011. 05. 13	최육사	대리 상인 좀비 유저 버그 해결
void KPersonalShopUserManager::GetUserList( OUT std::vector< UidType >& vecUidList )
{
	vecUidList.clear();

	std::map< UidType, KPersonalShopUserPtr >::const_iterator mit;
	for( mit = m_mapPersonalShopUser.begin(); mit != m_mapPersonalShopUser.end(); ++mit )
	{
		if( !mit->second )
		{
			START_LOG( cerr, L"Invalid room user pointer." )
				<< BUILD_LOG( mit->first )
				<< END_LOG;

			continue;
		}

		vecUidList.push_back( mit->second->GetCID() );
	}

}
//}}
//
//void KTradeUserManager::GetUserInfoList( OUT std::vector< KTradeUserInfo >& vecTradeUserInfo )
//{
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); ++mit )
//	{
//		KTradeUserInfo kTradeUserInfo;
//
//		if( mit->second != NULL )
//		{
//			mit->second->GetTradeUserInfo( kTradeUserInfo );
//
//			vecTradeUserInfo.push_back( kTradeUserInfo );
//		}
//		else
//		{
//			START_LOG( cerr, L"개인 거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			continue;
//		}
//	}
//}
//
//bool KTradeUserManager::GetOtherCID( UidType nCID, UidType& nOtherCID )
//{
//	if( GetNumMember() != 2 )
//	{
//		START_LOG( cerr, L"인원 수 이상." )
//			<< BUILD_LOG( GetNumMember() )
//			<< END_LOG;
//
//		return false;
//	}
//
//	std::map< UidType, KTradeUserPtr >::iterator mit = m_mapTradeUser.find( nCID );
//	if( mit == m_mapTradeUser.end() )
//	{
//		return false;
//	}
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); mit++ )
//	{
//		if( mit->first != nCID )
//		{
//			nOtherCID = mit->first;
//			return true;
//		}
//	}
//
//	START_LOG( cerr, L"절대 이 곳에는 오지 말아야 한다." )
//		<< END_LOG;
//
//	return false;
//}
//
//bool KTradeUserManager::GetOtherTradeUserInfo( UidType nCID, KTradeUserInfo& kOther )
//{
//	if( GetNumMember() != 2 )
//	{
//		START_LOG( cerr, L"인원 수 이상." )
//			<< BUILD_LOG( GetNumMember() )
//			<< END_LOG;
//
//		return false;
//	}
//
//	std::map< UidType, KTradeUserPtr >::iterator mit = m_mapTradeUser.find( nCID );
//	if( mit == m_mapTradeUser.end() )
//	{
//		return false;
//	}
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); mit++ )
//	{
//		if( mit->first != nCID )
//		{
//			if( !mit->second )
//			{
//				START_LOG( cerr, L"포인터 이상." )
//					<< BUILD_LOG( mit->first )
//					<< END_LOG;
//
//				continue;
//			}
//
//			mit->second->GetTradeUserInfo( kOther );
//			return true;
//		}
//	}
//
//	return false;
//}
//
//KTradeUserPtr KTradeUserManager::GetOtherUser( IN UidType nCID )
//{
//	if( GetNumMember() != 2 )
//	{
//		START_LOG( cerr, L"인원 수 이상." )
//			<< BUILD_LOG( GetNumMember() )
//			<< END_LOG;
//
//		return KTradeUserPtr();
//	}
//
//	std::map< UidType, KTradeUserPtr >::iterator mit = m_mapTradeUser.find( nCID );
//	if( mit == m_mapTradeUser.end() )
//	{
//		return KTradeUserPtr();
//	}
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); mit++ )
//	{
//		if( mit->first != nCID )
//		{
//			if( !mit->second )
//			{
//				START_LOG( cerr, L"포인터 이상." )
//					<< BUILD_LOG( mit->first )
//					<< END_LOG;
//
//				continue;
//			}
//
//			return mit->second;
//		}
//	}
//
//	return KTradeUserPtr();
//}
//
//bool KTradeUserManager::GetTradeItem( UidType nCID, KInventoryItemInfo& kInfo )
//{
//	kInfo.m_iItemUID = 0;
//
//	KTradeUserPtr spUser = GetUser( nCID );
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//	spUser->GetTradeItemInfo( kInfo );
//
//	return true;
//}
//
//bool KTradeUserManager::GetTradeED( UidType nCID, int& iED )
//{
//	iED = 0;
//
//	KTradeUserPtr spUser = GetUser( nCID );
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//	iED = spUser->GetTradeED();
//
//	return true;
//}
//
//bool KTradeUserManager::UpdateTradeItem( UidType nCID, KInventoryItemInfo& kInfo )
//{
//	KTradeUserPtr spUser = GetUser( nCID );
//
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	KTradeUserPtr spOtherUser = GetOtherUser( nCID );
//	if( !spOtherUser )
//	{
//		START_LOG( cerr, L"상대방 유저를 찾을 수 없음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	if( spUser->SetTradeItemInfo( kInfo ) )
//	{
//		spOtherUser->CancelOffer();
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool KTradeUserManager::UpdateTradeED( UidType nCID, int iED )
//{
//	KTradeUserPtr spUser = GetUser( nCID );
//
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	KTradeUserPtr spOtherUser = GetOtherUser( nCID );
//	if( !spOtherUser )
//	{
//		START_LOG( cerr, L"상대방 유저를 찾을 수 없음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	if( spUser->SetTradeED( iED ) )
//	{
//		spOtherUser->CancelOffer();
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool KTradeUserManager::Offer( UidType nCID )
//{
//	KTradeUserPtr spUser = GetUser( nCID );
//
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	spUser->Offer();
//	return true;
//}
//
//bool KTradeUserManager::Approve( UidType nCID )
//{
//	KTradeUserPtr spUser = GetUser( nCID );
//
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	spUser->Approve();
//	return true;
//}
//
//bool KTradeUserManager::Confirm( UidType nCID )
//{
//	KTradeUserPtr spUser = GetUser( nCID );
//
//	if( !spUser )
//	{
//		START_LOG( cerr, L"유저가 존재하지 않음." )
//			<< BUILD_LOG( nCID )
//			<< END_LOG;
//
//		return false;
//	}
//
//	spUser->Confirm();
//	return true;
//}
//
//bool KTradeUserManager::IsOffered()
//{
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); ++mit )
//	{
//		if( mit->second == NULL )
//		{
//			START_LOG( cerr, L"개인 거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			return false;
//		}
//		else
//		{
//			if( !mit->second->IsOffered() )
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//bool KTradeUserManager::IsApproved()
//{
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); ++mit )
//	{
//		if( mit->second == NULL )
//		{
//			START_LOG( cerr, L"개인 거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			return false;
//		}
//		else
//		{
//			if( !mit->second->IsApproved() )
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//bool KTradeUserManager::IsConfirmed()
//{
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); ++mit )
//	{
//		if( mit->second == NULL )
//		{
//			START_LOG( cerr, L"개인 거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			return false;
//		}
//		else
//		{
//			if( !mit->second->IsConfirmed() )
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//void KTradeUserManager::GetOfferState( OUT std::map< UidType, bool >& mapOfferState )
//{
//	mapOfferState.clear();
//
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); mit++ )
//	{
//		if( mit->second == NULL )
//		{
//			START_LOG( cerr, L"개인 거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			continue;
//		}
//		else
//		{
//			// 키 중복 검사를 할 필요가 없다.
//			mapOfferState.insert( std::make_pair( mit->first, mit->second->IsOffered() ) );
//		}
//	}
//}
//
//bool KTradeUserManager::IsThereNothingToTrade()
//{
//	std::map< UidType, KTradeUserPtr >::iterator mit;
//
//	for( mit = m_mapTradeUser.begin(); mit != m_mapTradeUser.end(); mit++ )
//	{
//		if( !mit->second )
//		{
//			START_LOG( cerr, L"거래 유저 포인터 이상." )
//				<< BUILD_LOG( mit->first )
//				<< END_LOG;
//
//			continue;
//		}
//
//		KInventoryItemInfo kInventoryItemInfo;
//		mit->second->GetTradeItemInfo( kInventoryItemInfo );
//		if( kInventoryItemInfo.m_iItemUID > 0 )
//		{
//			return false;
//		}
//
//		if( mit->second->GetTradeED() > 0 )
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
//
KPersonalShopUserPtr KPersonalShopUserManager::MakeUser( IN const KPersonalShopUserInfo& kInfo, IN const bool bForce /*= false*/ )
{
	if( m_mapPersonalShopUser.size() >= m_dwMaxUser  &&  bForce == false )
	{
		START_LOG( cwarn, L"Personal shop room is full." )
			<< BUILD_LOG( kInfo.m_iGSUID )
			<< BUILD_LOG( kInfo.m_iUnitUID )
			<< BUILD_LOG( m_dwMaxUser )
			<< END_LOG;

		SET_ERROR( ERR_PERSONAL_SHOP_04 );

		return KPersonalShopUserPtr();
	}

	KPersonalShopUserPtr spPersonalShopUser( new KPersonalShopUser() );
	if( !spPersonalShopUser )
	{
		SET_ERROR( ERR_UNKNOWN );

		return KPersonalShopUserPtr();
	}

	spPersonalShopUser->SetPersonalShopUserInfo( kInfo );
	return spPersonalShopUser;
}

bool KPersonalShopUserManager::IsHost( UidType nCID )
{
	KPersonalShopUserPtr spUser = GetUser( nCID );

	if( spUser != NULL )
	{
		return spUser->IsHost();
	}
	else
	{
		START_LOG( cerr, L"Personal shop user not found." )
			<< BUILD_LOG( nCID )
			<< END_LOG;
	}

	return false;
}

bool KPersonalShopUserManager::GetHostUID( UidType& nUID )
{
	nUID = 0;

	std::map<UidType, KPersonalShopUserPtr>::iterator mit;

	for( mit = m_mapPersonalShopUser.begin(); mit != m_mapPersonalShopUser.end(); ++mit )
	{
		if( mit->second != NULL )
		{
			if( mit->second->IsHost() == true )
			{
				nUID = mit->second->GetCID();
				return true;
			}
		}
	}

	START_LOG( cerr, L"Personal shop host not found." )
		<< BUILD_LOG( m_mapPersonalShopUser.size() )
		<< END_LOG;

	return false;
}

bool KPersonalShopUserManager::GetHostNickName( std::wstring& wstrNickName )
{
	std::map<UidType, KPersonalShopUserPtr>::iterator mit;

	for( mit = m_mapPersonalShopUser.begin(); mit != m_mapPersonalShopUser.end(); ++mit )
	{
		if( mit->second != NULL )
		{
			if( mit->second->IsHost() == true )
			{
				wstrNickName = mit->second->GetNickName();
				return true;
			}
		}
	}

	START_LOG( cerr, L"Personal shop host not found." )
		<< BUILD_LOG( m_mapPersonalShopUser.size() )
		<< END_LOG;

	return false;
}

//{{ 2011. 04. 14	최육사	대리 상인

UidType KPersonalShopUserManager::GetPShopAgencyHostUnitUID() const
{
	if( m_spPShopAgencyUser == NULL )
	{
		START_LOG( cerr, L"Proxy shop information does not exist." )
			<< END_LOG;
		return 0;
	}

	return m_spPShopAgencyUser->GetCID();
}

UidType KPersonalShopUserManager::GetPShopAgencyHostGSUID() const
{
	if( m_spPShopAgencyUser == NULL )
	{
		START_LOG( cerr, L"Proxy shop information does not exist." )
			<< END_LOG;
		return 0;
	}

	return m_spPShopAgencyUser->GetGSUID();
}

bool KPersonalShopUserManager::GetPShopAgencyHostNickName( OUT std::wstring& wstrNickName ) const
{
	if( m_spPShopAgencyUser == NULL )
	{
		START_LOG( cerr, L"Proxy shop information does not exist." )
			<< END_LOG;
		return false;
	}

	wstrNickName = m_spPShopAgencyUser->GetNickName();
	return true;
}

bool KPersonalShopUserManager::IsPShopAgencyHost( IN const UidType iUnitUID ) const
{
	if( m_spPShopAgencyUser == NULL )
	{
		START_LOG( cerr, L"Proxy shop information does not exist." )
			<< END_LOG;
		return false;
	}

	return ( m_spPShopAgencyUser->GetCID() == iUnitUID );
}
//}}

//{{ 2009. 2. 10  최육사	개인거래 버그 예방
bool KPersonalShopUserManager::SetUpdateTemp( UidType nCID, const KItemQuantityUpdate& kUpdateTemp, const std::vector< KItemInfo >& vecItemInfoTemp, int iEDTemp )
{
	KPersonalShopUserPtr spUser = GetUser( nCID );

	if( !spUser )
	{
		START_LOG( cerr, L"User does not exist." )
			<< BUILD_LOG( nCID )
			<< END_LOG;

		return false;
	}

	spUser->SetUpdateTemp( kUpdateTemp, vecItemInfoTemp, iEDTemp );
	return true;
}

bool KPersonalShopUserManager::GetUpdateTemp( UidType nCID, KItemQuantityUpdate& kUpdateTemp, std::vector< KItemInfo >& vecItemInfoTemp, int& iEDTemp )
{
	KPersonalShopUserPtr spUser = GetUser( nCID );

	if( !spUser )
	{
		START_LOG( cerr, L"User does not exist." )
			<< BUILD_LOG( nCID )
			<< END_LOG;

		return false;
	}

	spUser->GetUpdateTemp( kUpdateTemp, vecItemInfoTemp, iEDTemp );
	return true;
}
//}}

//{{ 2010. 05. 22  최육사	대전 던전 서버군 통합

bool KPersonalShopUserManager::IsHostUserSameServerGroup( IN UidType iUnitUID )
{
	UidType iHostUID = 0;
	if( GetHostUID( iHostUID ) == false )
	{
		START_LOG( cerr, L"Personal shop owner information is missing." )
			<< BUILD_LOG( iHostUID )
			<< END_LOG;

		return false;
	}

	if( KncUid::ExtractServerGroupID( iUnitUID ) != KncUid::ExtractServerGroupID( iHostUID ) )
	{
		START_LOG( cerr, L"Personal shop owner and entry requester are in different server groups. This should never happen." )
			<< BUILD_LOG( iUnitUID )
			<< BUILD_LOG( iHostUID )
			<< END_LOG;

		return false;
	}

	return true;
}

//}}

//{{ 2011. 04. 14	최육사	대리 상인
bool KPersonalShopUserManager::IsPShopAgencyHostUserSameServerGroup( IN UidType iUnitUID )
{
	const UidType iHostUID = GetPShopAgencyHostUnitUID();
	if( iHostUID == 0 )
	{
		START_LOG( cerr, L"Personal shop owner information is missing." )
			<< BUILD_LOG( iHostUID )
			<< END_LOG;

		return false;
	}

	if( KncUid::ExtractServerGroupID( iUnitUID ) != KncUid::ExtractServerGroupID( iHostUID ) )
	{
		START_LOG( cerr, L"Personal shop owner and entry requester are in different server groups. This should never happen." )
			<< BUILD_LOG( iUnitUID )
			<< BUILD_LOG( iHostUID )
			<< END_LOG;

		return false;
	}

	return true;
}
//}}


bool KPersonalShopUserManager::IsSquare( UidType _nCID )
{
	KPersonalShopUserPtr spUser = GetUser( _nCID );

	if( spUser != NULL )
	{
		return spUser->IsSquare();
	}
	else
	{
		START_LOG( cerr, L"Personal shop user not found." )
			<< BUILD_LOG( _nCID )
			<< END_LOG;
	}

	return true;
}
