#include "./SMSPhoneNumberManager.h"

#include <dbg/dbg.hpp>
#include "BaseServer.h"
#include "NetError.h"


//{{ 2012. 09. 26	최육사		우편 복사 버그 수정


ImplementRefreshSingleton( KSMSPhoneNumberManager );



KSMSPhoneNumberManager::KSMSPhoneNumberManager(void)
{
}

KSMSPhoneNumberManager::~KSMSPhoneNumberManager(void)
{
}

ImplToStringW( KSMSPhoneNumberManager )
{
	stm_	<< L"----------[ SMS Phone Number Manager ]----------" << std::endl;

	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	for( int idx = 0; idx < PNLT_MAX; ++idx )
	{
		stm_<< TOSTRINGW( idx ) << std::endl;
		stm_<< TOSTRINGW( m_vecPhoneNumList[idx].size() ) << std::endl;
	}
	//}}

	return stm_;
}

ImplementLuaScriptParser( KSMSPhoneNumberManager )
{
	lua_tinker::class_add<KSMSPhoneNumberManager>( GetLuaState(), "KSMSPhoneNumberManager" );
	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	lua_tinker::class_def<KSMSPhoneNumberManager>( GetLuaState(), "AddPhoneNumber",			&KSMSPhoneNumberManager::AddPhoneNumber_LUA );
	//}}
	lua_tinker::class_def<KSMSPhoneNumberManager>( GetLuaState(), "dump",					&KSMSPhoneNumberManager::Dump );

	lua_tinker::decl( GetLuaState(), "g_SMSPhoneNumberManager", this );

	lua_tinker::decl( GetLuaState(), "PNLT_ELSWORD_DEV_TEAM",			KSMSPhoneNumberManager::PNLT_ELSWORD_DEV_TEAM );
	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}
}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
void KSMSPhoneNumberManager::AddPhoneNumber_LUA( int iType, const char* pPhoneNum )
{
	if( iType < 0  ||  iType >= PNLT_MAX )
	{
		START_LOG( cerr, L"정의되지 않은 타입값이 넘어왔습니다!" )
			<< BUILD_LOG( iType )
			<< BUILD_LOG( pPhoneNum )
			<< END_LOG;
		return;
	}

	std::wstring wstrPhoneNumber = KncUtil::toWideString( pPhoneNum );

	// 폰번호 넣기!
	m_vecPhoneNumList[iType].push_back( wstrPhoneNumber );
}

void KSMSPhoneNumberManager::GetPhoneNumberList( IN const PHONE_NUMBER_LIST_TYPE eType, OUT std::vector< std::wstring >& vecPhoneNumList ) const
{
	vecPhoneNumList.clear();

	if( eType < 0  ||  eType >= PNLT_MAX )
	{
		START_LOG( cerr, L"잘못된 폰 넘버 타입입니다!" )
			<< BUILD_LOG( eType )
			<< END_LOG;
		return;
	}

	vecPhoneNumList = m_vecPhoneNumList[eType];
}
//}}

//}}
