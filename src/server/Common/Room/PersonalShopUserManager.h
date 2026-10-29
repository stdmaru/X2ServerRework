#pragma once

#include "PersonalShopUser.h"

class KPersonalShopUserManager
{
public:
	KPersonalShopUserManager(void);
	~KPersonalShopUserManager(void);

	void Init( IN int nMaxUser );
	void Reset();
	KPersonalShopUserPtr GetUser( IN UidType nCID );
	//{{ 2011. 05. 13	최육사	대리 상인 좀비 유저 버그 해결
	bool IsExistUnitUID( IN const UidType nCID ) { return ( m_mapPersonalShopUser.find( nCID ) != m_mapPersonalShopUser.end() ); }
	//}}

	bool AddUser( IN KPersonalShopUserPtr spRoomUser );
	bool DeleteUser( IN UidType nCID );
	int  DeleteUserByGsUID( IN UidType nGsUID, OUT std::vector< UidType >& vecDelUserUID );   // 해당 게임서버에 접속된 유저는 모두 지운다. (지운 유저수를 리턴)

	bool EnterPersonalShop( IN const KPersonalShopUserInfo& kInfo );
	//{{ 2011. 05. 13	최육사	대리 상인 좀비 유저 버그 해결
	bool EnterPShopAgencyOwner( IN const KPersonalShopUserInfo& kInfo );
	bool EnterPShopAgencyCustomer( IN const KPersonalShopUserInfo& kInfo );
	//}}
	bool LeavePersonalShop( IN UidType nCID );

	//{{ 2011. 04. 14	최육사	대리 상인
	bool CreatePersonalShopAgency( IN const KPersonalShopUserInfo& kInfo );
	bool DeletePersonalShopAgency( IN UidType nCID );
	//}}

	int  GetNumMember(){ return static_cast<int>( m_mapPersonalShopUser.size() ); }
	int  GetMaxMember(){ return static_cast<int>( m_dwMaxUser ); }

	bool GetPersonalShopUserGS( UidType nCID, UidType& nGSUID );
	void GetUserList( OUT std::map< UidType, std::set< UidType > >& mapUidList ); // Server UID 별로 모은 User UID.
	//{{ 2011. 05. 13	최육사	대리 상인 좀비 유저 버그 해결
	void GetUserList( OUT std::vector< UidType >& vecUidList );
	//}}

	bool IsHost( UidType nCID );
	bool GetHostUID( UidType& nUID );
	bool GetHostNickName( std::wstring& wstrNickName );
	//{{ 2011. 04. 14	최육사	대리 상인
	UidType GetPShopAgencyHostUnitUID() const;
	UidType GetPShopAgencyHostGSUID() const;
	bool GetPShopAgencyHostNickName( OUT std::wstring& wstrNickName ) const;
	bool IsPShopAgencyHost( IN const UidType iUnitUID ) const;
	bool IsExistPShopAgency() const	{ return ( m_spPShopAgencyUser != NULL ); }
	//}}

	//{{ 2009. 2. 10  최육사	개인거래 버그 예방
	bool SetUpdateTemp( UidType nCID, const KItemQuantityUpdate& kUpdateTemp, const std::vector< KItemInfo >& vecItemInfoTemp, int iEDTemp );
	bool GetUpdateTemp( UidType nCID, KItemQuantityUpdate& kUpdateTemp, std::vector< KItemInfo >& vecItemInfoTemp, int& iEDTemp );
	//}}

	//{{ 2010. 05. 22  최육사	대전 던전 서버군 통합
	bool IsHostUserSameServerGroup( IN UidType iUnitUID );
	//}}
	//{{ 2011. 04. 14	최육사	대리 상인
	bool IsPShopAgencyHostUserSameServerGroup( IN UidType iUnitUID );
	//}}

	bool IsSquare( UidType nCID );

protected:
	KPersonalShopUserPtr	MakeUser( IN const KPersonalShopUserInfo& kInfo, IN const bool bForce = false );

protected:
	std::map< UidType, KPersonalShopUserPtr >	m_mapPersonalShopUser;
	//{{ 2011. 04. 14	최육사	대리 상인
	KPersonalShopUserPtr						m_spPShopAgencyUser;
	//}}

	DWORD										m_dwMaxUser;
};
