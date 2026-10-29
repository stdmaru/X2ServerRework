#pragma once

#include "ServerDefine.h"

//{{ 2010. 10. 29	최육사	NetErrorID 타입 정의
typedef int		NetErrorID;	// NetErrorID 타입 정의
//}}

namespace NetError
{

#   undef NET_ERROR
#   define NET_ERROR( id, comment ) id,


//#include "NetError/NetError_def_US.h"
#include "Neterror/NetError_def_BR.h"


	//{{ 2011.11.20 임홍락	서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
	//}}

    //enum NETWORK_ERROR
    //{
        //#include "NetError_def.h"
    //};

	//09. 08. 17 김정협 클라이언트에는 외국버전 그대로 적용
    const wchar_t* GetErrStrF( NetErrorID errorID );
	const wchar_t* GetErrStr( NetErrorID errorID );

    void SetLastError( NetErrorID errorID );
    const wchar_t* GetLastErrMsg();
    NetErrorID GetLastError();
}

#define SET_ERROR( id ) NetError::SetLastError( NetError::id )
