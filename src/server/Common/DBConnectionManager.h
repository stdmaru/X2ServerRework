#pragma once
#include "SubThread.h"
#include "ODBC/Odbc.h"
#include "ODBC/DBUtil.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"

//{{ 2012. 1. 9	최육사		DB접속 암호 이중화

class KDBConnectionManager
{
public:
    KDBConnectionManager();
    virtual ~KDBConnectionManager(void);

	bool GetDBConnectionInfoFromManagerDB( IN const int iServerType, IN const wchar_t* szConnStr, IN const bool bDBConnStr, OUT std::vector< KDBConnectionInfo >& vecDBConnectionInfo );
	bool DecryptDBString( IN const std::wstring& wstrDBString, OUT std::wstring& wstrDecryptString );

protected:
	KODBC			m_kODBC;
};

//}}
