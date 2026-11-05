#include "stdafx.h"
#include "./X2ServerPacket.h"

CX2ServerPacket::CX2ServerPacket(void)
{
	bool bTest = false;
	if( bTest == false )
		bTest = true;
}

CX2ServerPacket::~CX2ServerPacket(void)
{
}

#include "CommonPacket.cpp"

#include "SystemPacket.cpp"
#include "ClientPacket.cpp"

#include "OnlyGlobal/Packet/CommonPacket_Global.cpp"
#include "OnlyGlobal/Packet/SystemPacket_Global.cpp"
#include "OnlyGlobal/Packet/ClientPacket_Global.cpp"
