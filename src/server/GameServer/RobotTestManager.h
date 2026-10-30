#pragma once

#include <KNCSingleton.h>
#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"
#include "X2Data/XSLUnit.h"

class KRobotTestManager
{
	DeclareSingleton( KRobotTestManager );
private:

	typedef std::vector< int >	KRandomItemGroup;

public:
	KRobotTestManager(void);
	~KRobotTestManager(void);

	void FileOpen( int iPatternIndex );
	void FileClose();
	void RobotPet_LUA( bool bVal );
	void PrintRobotScript( const KEGS_FIELD_UNIT_SYNC_DATA_NOT& kEvent, int iMapID );

	// 로봇 랜덤 아이템 장착
	bool InitRobotEquipItemList();
	bool IsRobot( const std::wstring& wstrNickName );
	void RobotFullEquip( const std::wstring& wstrNickName, int iUnitType, std::vector< KInventoryItemSimpleInfo >& vecItem );
	void RobotPet( const std::wstring& wstrNickName, UidType& iSummonedPetUID, std::vector< KPetInfo >& vecPet );

	int GetEquipSlotID( int iEquipPosition, bool bFashion );
	int GetRandomEquipItem( int iSlotID, int iUnitType );

private:
	int					m_iPatternIndex;
	std::wfstream		m_wfsRobotScript;
	bool				m_bIsRobotPet;

	// 로봇 랜덤 아이템 장착
	std::map< int, KRandomItemGroup > m_mapRandomEquip[CXSLUnit::NESI_END];
	static UidType		ms_dummpyItemUID;
};

DefSingletonInline( KRobotTestManager );
