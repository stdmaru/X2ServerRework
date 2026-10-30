#include "./UserPcBangManager.h"

	#include "GameSysVal/GameSysVal.h"

KUserPcBangManager::KUserPcBangManager(void) :
m_bCanTry( false ),
m_bIsGameBang( false ),
m_iPcBangType( -1 ),
m_bytePCBangAuthResult( 0 ),
//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
m_byteAuthorizeType( 0 ),
m_byteChargeType( 0 ),
//}}
m_bReservePcBangMessage( false ),
m_uiArgument( 0 )
{
}

KUserPcBangManager::~KUserPcBangManager(void)
{
}

//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편

void KUserPcBangManager::InitPcBangAuth( BYTE bytePCBangAuthResult, BYTE byteAuthorizeType, BYTE byteChargeType )
{
	m_bytePCBangAuthResult	= bytePCBangAuthResult;
	m_byteAuthorizeType		= byteAuthorizeType;
	m_byteChargeType		= byteChargeType;
}

//}}

void KUserPcBangManager::SetPcBangMessage( u_int uiArgument )
{
	m_uiArgument = uiArgument;

	m_bReservePcBangMessage = true;
}

u_int KUserPcBangManager::GetPcBangMessageArgument()
{
	m_bReservePcBangMessage = false;

	return m_uiArgument;
}

KPcBangReward KUserPcBangManager::GetPcBangReward()
{
	return SiKGameSysVal()->GetPcBangReward( GetPcBangType() );
}
