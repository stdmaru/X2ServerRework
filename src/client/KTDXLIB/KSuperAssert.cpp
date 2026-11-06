// The engine's assertion and log stream (original code from seokhyun, SF). Changes in the original: no remote logger
// or console; _fileout takes whether to write the buffer out; fileout_ keeps the buffer.

#include "stdafx.h"
#include "KSuperAssert.h"


#include "x2/platform/win32.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>

KSuperAssert g_kSuperAssert;

//---------------------------------------------------------------------------
KSuperAssert::KSuperAssert()
{
}
//---------------------------------------------------------------------------
KSuperAssert& KSuperAssert::SetAssertInfo(const char* expr,
										  unsigned int line, const char* /*func*/, const char* file)
{
	m_strExpression = expr;
	m_strFilename = file;
	m_uiLine = line;

	std::ostringstream os;
	os
		<< "[Assertion]" << std::endl
		<< "File : " << file << std::endl
		<< "Line : " << line << std::endl
		<< "Expr : " << expr << std::endl << std::endl;

	m_strBuf += os.str().c_str();

	return *this;
}
//---------------------------------------------------------------------------
void KSuperAssert::_initBuf()
{
	m_strBuf.clear();
	m_strInfo.clear();
}
//---------------------------------------------------------------------------
void KSuperAssert::_fatal()
{
	m_strBuf += m_strInfo;
	m_strInfo = "";

	while (true)
	{
		int iRes = MessageBoxA(NULL, m_strBuf.c_str(),
			"Assertion", MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_TASKMODAL);
		switch (iRes)
		{
		case IDIGNORE:
			return;
		case IDRETRY:
			// into the debugger (int 3 in the original)
#if defined(__clang__)
			__builtin_debugtrap();
#else
			__builtin_trap();
#endif
			return;
		case IDABORT:
			std::_Exit(3);
		default:
			break;
		}
	}
}
//---------------------------------------------------------------------------
void KSuperAssert::_trace()
{
	m_strBuf += m_strInfo + "\n";
	m_strInfo = "";
	// the debugger's output in the original (OutputDebugStringA)
	std::fputs(m_strBuf.c_str(), stderr);
}


//---------------------------------------------------------------------------
void KSuperAssert::_fileout( bool bFlush )
{
	m_strBuf += m_strInfo + "\n";
	m_strInfo = "";

	if( true == bFlush )
	{
		if (!m_strLogFilename.empty())
		{
			time_t t0 = time(0);

			tm* t = localtime(&t0);
            bool bOK = ( t != NULL );

			std::fstream fout(m_strLogFilename.c_str(), std::ios::out | std::ios::app);
			//fout << "//---------------------------------------------------------------------------" << std::endl;
            if ( bOK == true )
				// the format the US service asked for
				fout << 1900+t->tm_year << "/" << 1+t->tm_mon << "/" << t->tm_mday << " " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << "\t";
			//fout << __DATE__ << ", " << __TIME__ << " > ";
			//fout << "// file: " << __FILE__ << std::endl;
			//fout << "// line: " << __LINE__ << std::endl;
			//fout << "//---------------------------------------------------------------------------" << std::endl;
			fout << m_strBuf;
			//fout << std::endl;
			fout.close();
		}
	}

}
//---------------------------------------------------------------------------
void KSuperAssert::_remotelog()
{
}
//---------------------------------------------------------------------------
void KSuperAssert::SetLogFilename(std::string strFilename)
{
	m_strLogFilename = strFilename;
}
//---------------------------------------------------------------------------
void KSuperAssert::SetRemoteLoggerPagename(std::string strPagename)
{
	m_strRemoteLoggerPagename = strPagename;
}
//---------------------------------------------------------------------------
// manipication
//---------------------------------------------------------------------------
KSuperAssert& fatal_(KSuperAssert& kAssert)
{
	kAssert._fatal();

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& trace_(KSuperAssert& kAssert)
{
	kAssert._trace();

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& fileout_(KSuperAssert& kAssert)
{
	//kAssert._trace();
	kAssert._fileout( false );

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& remotelog_(KSuperAssert& kAssert)
{
	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& fatal(KSuperAssert& kAssert)
{
	kAssert._fatal();
	kAssert._initBuf();

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& trace(KSuperAssert& kAssert)
{
	kAssert._trace();
	kAssert._initBuf();

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& fileout(KSuperAssert& kAssert)
{
	kAssert._fileout( true );
	kAssert._initBuf();

	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& remotelog(KSuperAssert& kAssert)
{
	return kAssert;
}
//---------------------------------------------------------------------------
KSuperAssert& flush(KSuperAssert& kAssert)
{
	kAssert._initBuf();

	return kAssert;
}
//---------------------------------------------------------------------------
