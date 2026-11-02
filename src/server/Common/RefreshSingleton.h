#pragma once
#include <Thread/Locker.h>
#include <KncUtil.h>
#include <string>
#include <vector>
#include <KncLua.h>
#include <lua_tinker.h>
#include "Lua/KLuaManager.h"
#include "dbg/dbg.hpp"
#include "KAutoPath.h"
#include <ToString.h>

// 2010-01-15
// Refresh Singleton macro: a singleton with two instances.
// 1. It holds a Real Instance and a Temp Instance.
// 2. The Temp Instance pointer is synchronized.
// 3. SwapInstance exchanges the two instances.

//////////////////////////////////////////////////////////////////////////
// 적용하려는 class의 namespace안에 정의한다.
#define DeclareRefreshSingleton( classname ) \
	public: \
		static classname* GetRealInstance(); \
		static classname* GetTempInstance(); \
		static void ReleaseInstance(); \
		static bool SwapInstance( lua_State* pLuaState ); \
		static KncCriticalSection& GetTempCS(); \
	protected: \
		static classname*   ms_selfRealInstance; \
		static classname*	ms_selfTempInstance; \
		static KncCriticalSection ms_selfTempCS

//////////////////////////////////////////////////////////////////////////
// 외부에서 참조하기 위한 인터페이스
#define DefRefreshSingletonInline( classname ) \
	inline classname* Si##classname()		{ return classname::GetRealInstance(); } \
	inline classname* SiTemp##classname()	{ return classname::GetTempInstance(); }

//////////////////////////////////////////////////////////////////////////
// refresh singleton 구현부
#define ImplementRefreshSingleton( classname ) \
	classname* classname::ms_selfRealInstance = NULL; \
	classname* classname::ms_selfTempInstance = NULL; \
	KncCriticalSection classname::ms_selfTempCS; \
	classname* classname::GetRealInstance() \
	{ \
		if( ms_selfRealInstance == NULL ) \
			ms_selfRealInstance = new classname; \
		return ms_selfRealInstance; \
	} \
	classname* classname::GetTempInstance() \
	{ \
		KLocker lock( ms_selfTempCS ); \
		if( ms_selfTempInstance == NULL ) \
			ms_selfTempInstance = new classname; \
		return ms_selfTempInstance; \
	} \
	void classname::ReleaseInstance() \
	{ \
		if( ms_selfRealInstance != NULL ) \
		{ \
			delete ms_selfRealInstance; \
			ms_selfRealInstance = NULL; \
		} \
		{ \
			KLocker lock( ms_selfTempCS ); \
			if( ms_selfTempInstance != NULL ) \
			{ \
				delete ms_selfTempInstance; \
				ms_selfTempInstance = NULL; \
			} \
		} \
	} \
	bool classname::SwapInstance( lua_State* pLuaState ) \
	{ \
		KLocker lock( ms_selfTempCS ); \
		if( ms_selfRealInstance == NULL  ||  ms_selfTempInstance == NULL ) \
			return false; \
		if( ms_selfRealInstance != NULL ) \
		{ \
			delete ms_selfRealInstance; \
			ms_selfRealInstance = NULL; \
		} \
		ms_selfRealInstance = ms_selfTempInstance; \
		ms_selfTempInstance = NULL; \
		ms_selfRealInstance->SetLuaState( pLuaState ); \
		ms_selfRealInstance->RegToLua(); \
		return true; \
	} \
	KncCriticalSection& classname::GetTempCS() { return ms_selfTempCS; } \

//////////////////////////////////////////////////////////////////////////
// Dump To Log File

#define DeclDumpToLogFile \
	public: \
		void DumpToLogFile() { std::wstringstream wstrDump; ToString( wstrDump ); dbg::cout << wstrDump.str() << dbg::endl; } \


//////////////////////////////////////////////////////////////////////////
// Lua Script Parser

#define DeclareLuaScriptParser \
	public: \
		static void RegScriptName( const char* pStrFileName ) { ms_vecScriptName.push_back( pStrFileName ); } \
		bool OpenScriptFile( lua_State* pLuaState ); \
		void RegToLua(); \
		void LuaParsingFail(); \
		const std::wstring& GetParsingLuaFileName() const; \
		bool IsLuaPasingSuccess() const; \
		void SetLuaState( lua_State* pLuaState ) { m_pLuaState = pLuaState; } \
	protected: \
		static std::vector< std::string > ms_vecScriptName; \
	protected: \
		lua_State* GetLuaState() { return m_pLuaState; } \
	protected: \
		bool				m_bLuaParsingSuccess; \
		std::wstring		m_wstrParsingLuaFileName; \
		lua_State*			m_pLuaState

#define ImplementLuaScriptParser( classname ) \
	std::vector< std::string > classname::ms_vecScriptName; \
	void classname::LuaParsingFail() { m_bLuaParsingSuccess = false; } \
	const std::wstring& classname::GetParsingLuaFileName() const { return m_wstrParsingLuaFileName; } \
	bool classname::IsLuaPasingSuccess() const { return m_bLuaParsingSuccess; } \
	bool classname::OpenScriptFile( lua_State* pLuaState ) \
	{ \
		m_bLuaParsingSuccess = true; \
		SetLuaState( pLuaState ); \
		RegToLua(); \
		std::vector< std::string >::const_iterator vit; \
		for( vit = ms_vecScriptName.begin(); vit != ms_vecScriptName.end(); ++vit ) \
		{ \
			m_wstrParsingLuaFileName = KncUtil::toWideString( *vit ); \
			std::string strFilePath = *vit; \
			KAutoPath autoPath; \
			autoPath.GetPullPath( strFilePath ); \
			DWORD m_dwTime = ::GetTickCount(); \
			const int iRet = LUA_DOFILE( GetLuaState(), strFilePath.c_str() ); \
			if( iRet == 0 ) { \
				std::wstring wstrMSG = GetParsingLuaFileName(); \
				wstrMSG += L" load time: "; \
				START_LOG( cout2, wstrMSG ) \
					<< BUILD_LOG( GetTickCount() - m_dwTime ); \
			} \
			else { \
				std::wstring wstrMSG = GetParsingLuaFileName(); \
				wstrMSG += L" script parsing failed!"; \
				std::wstring wstrErrPath = KncUtil::toWideString( strFilePath ); \
				std::wstring wstrErr = KncUtil::toWideString( lua_tostring( GetLuaState(), -1 ) ); \
				START_LOG( cerr, wstrMSG ) \
				    << BUILD_LOG( wstrErrPath ) \
					<< BUILD_LOG( wstrErr ); \
				LuaParsingFail(); \
				return false; \
			} \
		} \
		return true; \
	} \
	void classname::RegToLua()
