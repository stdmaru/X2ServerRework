#include "stdafx.h"
#include "./KTDXEnumToLua.h"

CKTDXEnumToLua::CKTDXEnumToLua(void)
{
}

CKTDXEnumToLua::~CKTDXEnumToLua(void)
{
}

bool CKTDXEnumToLua::LoadEnumToLua( KLuaManager* /*pLuaManager*/, const WCHAR* pEnumFileName )
{
    if ( g_pKTDXApp->LoadLuaTinker( pEnumFileName ) == false )
    {
		ErrorLogMsg( KEM_ERROR104, pEnumFileName );
		return false;
    }

	return true;
}
