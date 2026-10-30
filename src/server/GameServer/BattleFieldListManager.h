#pragma once
#include <KNCSingleton.h>
#include <ToString.h>
#include "BattleFieldList.h"


//{{ 2011. 10. 27	최육사	배틀필드 시스템


class KBattleFieldListManager
{
	DeclareSingleton( KBattleFieldListManager );
	DeclDump;
	DeclToStringW;

public:
	KBattleFieldListManager(void);
	virtual ~KBattleFieldListManager(void);

	KBattleFieldListPtr GetBattleFieldRoomList( IN const int iBattleFieldID );
	bool	IsExist( IN const int iBattleFieldID ) const		{ return ( m_mapBattleFieldList.find( iBattleFieldID ) != m_mapBattleFieldList.end() ); }

	void	UpdateBattleFieldRoomInfo( IN const KBattleFieldRoomInfo& kRoomInfo );
	void	DeleteBattleFieldRoomInfo( IN const KBattleFieldRoomInfo& kRoomInfo );
	//{{ 2012. 11. 20	최육사		배틀필드 방 정리 기능 추가
	void	DeleteBattleFieldRoomInfo( IN const UidType iCnServerUID );
	//}}

	// 조건에 따라 가장 적절한 필드로 이동하기 위한 함수
	bool	GetRoomUIDForJoinBattleField( IN const int iBattleFieldID, IN const UidType iUnitUID, IN const UidType iTargetRoomUID, IN const UidType iPartyUID, OUT UidType& iJoinBFRoomUID );

private:
	std::map< int, KBattleFieldListPtr >	m_mapBattleFieldList;		// key : BattleFieldID, value : BattleFieldList
};

DefSingletonInline( KBattleFieldListManager );


//}}
