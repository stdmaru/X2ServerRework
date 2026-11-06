#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "x2/platform/unicode.h"

class KSuperAssert
{

public:
    KSuperAssert();

	KSuperAssert& SetAssertInfo(const char* expr, unsigned int line,
		const char* func, const char* file);

	void _initBuf();
	void _fatal();
	void _trace();
	void _fileout(bool bFlush);
	void _remotelog();

	template<typename T>
	KSuperAssert& operator << (const T& val)
	{
		std::ostringstream os;
		os << val;
		m_strInfo += os.str();

		return *this;
	}

	// Wide text goes in as UTF-8 and is echoed to the console (the original converted it to the ANSI code page).
	KSuperAssert& operator << (const wchar_t* val)
	{
		const std::string text = x2::platform::WideToUtf8( val != nullptr ? val : L"" );
		m_strInfo += text;
		std::cout << text << std::endl;

		return *this;
	}
	KSuperAssert& operator << (wchar_t* val)              { return *this << static_cast<const wchar_t*>( val ); }
	KSuperAssert& operator << (const std::wstring& val)   { return *this << val.c_str(); }

	// manipulators
	KSuperAssert& operator << (KSuperAssert& (*_manip) (KSuperAssert&))
	{
		return _manip( *this );
	}

	friend KSuperAssert& flush(KSuperAssert& kAssert);

	// Flush
	friend KSuperAssert& fatal(KSuperAssert& kAssert);
	friend KSuperAssert& trace(KSuperAssert& kAssert);
	friend KSuperAssert& fileout(KSuperAssert& kAssert);
	friend KSuperAssert& remotelog(KSuperAssert& kAssert);

	// Not Flush
	friend KSuperAssert& fatal_(KSuperAssert& kAssert);
	friend KSuperAssert& trace_(KSuperAssert& kAssert);
	friend KSuperAssert& fileout_(KSuperAssert& kAssert);
	friend KSuperAssert& remotelog_(KSuperAssert& kAssert);

	// Names
public:
	void SetLogFilename(std::string strFilename);
	void SetRemoteLoggerPagename(std::string strPagename);

protected:
	std::string m_strLogFilename;
	std::string m_strRemoteLoggerPagename;

protected:
	std::string m_strExpression;
	std::string m_strFilename;
	unsigned int m_uiLine;

	std::string m_strInfo;
	std::string m_strBuf;

};

// The manipulators, at namespace scope: `KLOG(...) << msg << fileout` names them without a call, which a friend
// declared only inside the class does not make visible (MSVC accepted it).
KSuperAssert& flush(KSuperAssert& kAssert);
KSuperAssert& fatal(KSuperAssert& kAssert);
KSuperAssert& trace(KSuperAssert& kAssert);
KSuperAssert& fileout(KSuperAssert& kAssert);
KSuperAssert& remotelog(KSuperAssert& kAssert);
KSuperAssert& fatal_(KSuperAssert& kAssert);
KSuperAssert& trace_(KSuperAssert& kAssert);
KSuperAssert& fileout_(KSuperAssert& kAssert);
KSuperAssert& remotelog_(KSuperAssert& kAssert);

extern KSuperAssert g_kSuperAssert;


// additional macros

#define KLOG(filename)			KDbgStm.SetLogFilename(filename); KDbgStm

#if X2_DEV_TOOLS

#ifdef BUG_TRAP
	#define InHouseLog( msg )

	#define StateLog( msg ) \
    {	\
        g_CKTDXLog.PushStateLog( msg );	\
    }

    #define ErrorLogMsg( errorEnum, errorMsg )		\
    {	\
        g_CKTDXLog.PushErrorLog( errorEnum, errorMsg );	\
    }

    #define ErrorLog( errorEnum )					\
    {	\
        g_CKTDXLog.PushErrorLog( errorEnum );	\
    }

    #define DialogLog( msg )

    #define LastErrorLog( errorMsg )					\
    {	\
        g_CKTDXLog.SetLastErrorLog( errorMsg );	\
    }

    #define ChecksumLog( msg )

#else

	#define InHouseLog( msg ) KLOG("InHouse.log")  << msg << fileout;
	#define StateLog( msg ) KLOG("State.log")  << msg << fileout;


	#define ErrorLogMsg( errorEnum, errorMsg )		\
	{	\
				KLOG("ErrorLog.txt") << "ErrorEnum(" << errorEnum << "),\t" << "ErrorMsg(\"" << errorMsg << "\")," << "\t( " <<__FILE__ << ", " << __FUNCTION__  << ", " << __LINE__ << " )" << fileout;  \
				fprintf( stderr, "\nㅁ ErrorEnum( %d ), %s, %s, %d\n", errorEnum, __FILE__, __FUNCTION__, __LINE__ );	\
	}

	#define ErrorLog( errorEnum )					\
	{	\
				KLOG("ErrorLog.txt") << "ErrorEnum(" << errorEnum << "),\t( " <<__FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << " )" << fileout; \
				std::cout << "ErrorEnum(" << errorEnum << "),\t( " <<__FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << " )" << std::endl; \
				fprintf( stderr, "\nㅁ ErrorEnum( %d ), %s, %s, %d\n", errorEnum, __FILE__, __FUNCTION__, __LINE__ );	\
	}
	#define DialogLog( msg )		\
	{	\
		KLOG("DialogLog.txt")  << msg << ",\t\t" << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << fileout; \
	}

	#define LastErrorLog( errorMsg )


	#define ChecksumLog( msg ) KLOG("Checksum.log")  << msg << fileout;

	//{{ kimhc // 2010.10.5 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
//}} kimhc // 2010.10.5 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

#endif // BUG_TRAP

#else

	#define InHouseLog( msg )

	#define StateLog( msg ) \
	{	\
		g_CKTDXLog.PushStateLog( msg );	\
	}

	#define ErrorLogMsg( errorEnum, errorMsg )		\
	{	\
		g_CKTDXLog.PushErrorLog( errorEnum, errorMsg );	\
	}

	#define ErrorLog( errorEnum )					\
	{	\
		g_CKTDXLog.PushErrorLog( errorEnum );	\
	}

	#define DialogLog( msg )

	#define LastErrorLog( errorMsg )					\
	{	\
		g_CKTDXLog.SetLastErrorLog( errorMsg );	\
	}

	#define ChecksumLog( msg )

//{{ kimhc // 2010.10.5 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
//}} kimhc // 2010.10.5 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

#endif // _SERVICE_


/*
#define P2PLogW( msg )			\
	{ \
		string strP2PLogW; \
		ConvertWCHARToChar( strP2PLogW, msg ); \
		KLOG("P2PLog.txt")  << strP2PLogW.c_str() << fileout; std::cout << strP2PLogW.c_str() << std::endl; \
	}
*/
// Macros
//#ifdef _DEBUG

	#define KDbgStm g_kSuperAssert
	#define KAssert(mExpr) \
		if (!(mExpr)) \
			g_kSuperAssert.SetAssertInfo(#mExpr, __LINE__, __FUNCTION__, __FILE__)._fatal(); \
		else \
			g_kSuperAssert._initBuf();

//#else // _DEBUG
//
//	#define KDbgStm /##/
//	#define KAssert(mExpr) /##/
//
//#endif // _DEBUG

/*
	* 사용 방법
	- Trace :
		KDbgStm << Debug Info << trace;
	- Assert :
		KDbgStm << Debug Info;
		KAssert( exp );

	* 주의
	- KDbgStm 사용 시 여러 라인일 경우 매 라인마다 KDbgStm를 적어주어야 함
		KDbgStm << "Value1";
		KDbgStm << "Value2";
		.
		.
		.

	  ( 다음과 같은 사용 금지 )
		KDbgStm
				<< "Value1"
				<< "Value2"
				.
				.
				.
	  이유는 릴리즈 모드에서의 부하를 모두 없애기 위해 ( 2번째 방법일 경우 함수 호출의 비용이 생김 )
*/
