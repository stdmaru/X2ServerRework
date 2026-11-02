#include "BattleFieldParty.h"


//{{ 2011. 11. 2	최육사	배틀필드 시스템


KBattleFieldParty::KBattleFieldParty()
{
	Clear();
}

KBattleFieldParty::~KBattleFieldParty()
{
}

void KBattleFieldParty::Clear()
{
	m_setPartyMemberList.clear();
}

void KBattleFieldParty::UpdatePartyMemberList( IN const std::vector< UidType >& vecInfo )
{
	for( const UidType iUnitUID : vecInfo )
	{
		m_setPartyMemberList.insert( iUnitUID );
	}
}

void KBattleFieldParty::GetPartyMemberList( OUT std::vector< UidType >& vecMemberList ) const
{
	vecMemberList.clear();

	for( const UidType iUnitUID : m_setPartyMemberList )
	{
		vecMemberList.push_back( iUnitUID );
	}
}

bool KBattleFieldParty::JoinPartyMember( IN const UidType iUnitUID )
{
	if( IsPartyMember( iUnitUID ) == true )
	{
		START_LOG( cerr, L"Party member is attempting to join again." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;
		return false;
	}

	m_setPartyMemberList.insert( iUnitUID );
	return true;
}
bool KBattleFieldParty::LeavePartyMember( IN const UidType iUnitUID )
{
	if( IsPartyMember( iUnitUID ) == false )
	{
		START_LOG( cerr, L"Attempted to remove a user who is not a party member." )
			<< BUILD_LOG( iUnitUID )
			<< END_LOG;
		return false;
	}

	m_setPartyMemberList.erase( iUnitUID );
	return true;
}


//}}
