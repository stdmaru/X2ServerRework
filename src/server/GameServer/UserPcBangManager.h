#pragma once
#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"

class KUserPcBangManager
{
public:
	KUserPcBangManager(void);
	~KUserPcBangManager(void);

	void SetCanTry( bool bVal )			{ m_bCanTry = bVal; }

	void SetIsPcBang( bool bVal )		{ m_bIsGameBang = bVal; }
	bool IsPcBang()						{ return m_bIsGameBang; }
	void SetPcBangType( int bVal )		{ m_iPcBangType = bVal; }
	int	GetPcBangType()					{ return m_iPcBangType; }
	KPcBangReward GetPcBangReward();

	//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
	void InitPcBangAuth( BYTE bytePCBangAuthResult, BYTE byteAuthorizeType, BYTE byteChargeType );
	//}}

	void SetPcBangMessage( u_int uiArgument );
	u_int GetPcBangMessageArgument();

	bool IsReservedPcBangMessage()		{ return m_bReservePcBangMessage; }

private:
	// PC 방 인증
	bool			m_bCanTry;
	bool			m_bIsGameBang;
	int				m_iPcBangType;
	BYTE			m_bytePCBangAuthResult;
	//{{ 2010. 06. 08  최육사	넥슨PC방 인증 서버 개편
	BYTE			m_byteAuthorizeType;
	BYTE			m_byteChargeType;
	//}}

	// PC방 인증 서버 메시지
	bool			m_bReservePcBangMessage;
	u_int			m_uiArgument;
};
