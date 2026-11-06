#include "stdafx.h"
#include "./KTDXLog.h"

CKTDXLog g_CKTDXLog;

/*static*/
thread_local CKTDXLog::KTLSData* CKTDXLog::ms_pTLSData = NULL;


CKTDXLog::CKTDXLog(void)
{
    m_dwTimestamp = 0;
    RegisterCurrentThread();
}

CKTDXLog::~CKTDXLog(void)
{
}

bool    CKTDXLog::RegisterCurrentThread()
{
    if ( ms_pTLSData != NULL )
        return true;

    DWORD   dwCurrentThreadId = ::GetCurrentThreadId();
    for( int i = 0; i < KTDXLOG_MAX_THREAD_NUM; i++ )
    {
        DWORD   dwOldValue = 0;
        if ( m_aTLSData[i].m_dwThreadId.compare_exchange_strong( dwOldValue, dwCurrentThreadId ) )
        {
            ms_pTLSData = &m_aTLSData[i];
            return true;
        }
    }
    return false;
}

const char* CKTDXLog::GetLastErrorLog()
{
    if ( ms_pTLSData != NULL )
    {
        return ms_pTLSData->m_LastErrorLog.c_str();
    }
    return "";
}


void CKTDXLog::PushStateLog( const WCHAR* statLog )
{
	string logstr;
	ConvertWCHARToChar( logstr, statLog );
	PushStateLog( logstr.c_str() );
}

void CKTDXLog::PushErrorLog( int errNum, const WCHAR* errorLog )
{
	string logstr;
	ConvertWCHARToChar( logstr, errorLog );
	PushErrorLog( errNum, logstr.c_str() );
}

void CKTDXLog::PushStateLog( const char* statLog )
{
	time_t t0 = time(0);
	stringstream strstm;
	tm* t = localtime(&t0);
    if ( t != NULL )
    {
	    strstm << 1900+t->tm_year << "/" << 1+t->tm_mon << "/" << t->tm_mday << ", " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " > " << statLog;
    }

    if ( ms_pTLSData != NULL )
    {
        DWORD   dwTimestamp = ++m_dwTimestamp;
        ms_pTLSData->m_StateLog.push_back( std::pair<DWORD,std::string>(dwTimestamp,strstm.str()) );
    }
}

void CKTDXLog::PushErrorLog( int errNum )
{
	time_t t0 = time(0);
	stringstream strstm;
	tm* t = localtime(&t0);
    if ( t != NULL )
    {
	    strstm << 1900+t->tm_year << "/" << 1+t->tm_mon << "/" << t->tm_mday << ", " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " > " << "ErrorEnum(" << errNum << ")";
    }

    if ( ms_pTLSData != NULL )
    {
        DWORD   dwTimestamp = ++m_dwTimestamp;
        ms_pTLSData->m_ErrorLog.push_back( std::pair<DWORD,std::string>( dwTimestamp, strstm.str() ) );
    }
}

void CKTDXLog::PushErrorLog( int errNum, const char* errorLog )
{
	time_t t0 = time(0);
	stringstream strstm;
	tm* t = localtime(&t0);
    if ( t != NULL )
    {
	    strstm << 1900+t->tm_year << "/" << 1+t->tm_mon << "/" << t->tm_mday << ", " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " > " << "ErrorEnum(" << errNum << "),\t" << "ErrorMsg(\"" << errorLog << "\")";
    }

    if ( ms_pTLSData != NULL )
    {
        DWORD   dwTimestamp = ++m_dwTimestamp;
        ms_pTLSData->m_ErrorLog.push_back( std::pair<DWORD,std::string>( dwTimestamp, strstm.str() ) );
    }
}

void CKTDXLog::SetLastErrorLog( const char* statLog )
{
    if ( ms_pTLSData != NULL )
    {
        ms_pTLSData->m_LastErrorLog = ( statLog != NULL ) ? statLog : "";
    }
}


void CKTDXLog::SetLastErrorLog( const WCHAR* statLog )
{
    std::string str;
    ConvertWCHARToChar( str, statLog );
    SetLastErrorLog( str.c_str() );
}

void CKTDXLog::StateLogToFile( const char* fileName )
{

    if ( fileName == NULL || fileName[0] == '\0' )
        return;

    int aIndex[KTDXLOG_MAX_THREAD_NUM];
    memset( aIndex, 0, sizeof(aIndex) );
    while( 1 )
    {
        int     iSmallestThread = -1;
        DWORD   dwSmallestTimestamp = 0;
        for( int iThread = 0; iThread < KTDXLOG_MAX_THREAD_NUM; iThread++ )
        {
            KTLSData& kTLSData = m_aTLSData[ iThread ];
            if ( aIndex[ iThread ] < (int) kTLSData.m_StateLog.size() )
            {
                if ( iSmallestThread < 0
                    || kTLSData.m_StateLog[ aIndex[ iThread ] ].first < dwSmallestTimestamp )
                {
                    iSmallestThread = iThread;
                    dwSmallestTimestamp = kTLSData.m_StateLog[ aIndex[ iThread ] ].first;
                }
            }
        }
        if ( iSmallestThread < 0 )
            break;
        KTLSData& kTLSData = m_aTLSData[ iSmallestThread ];
        if ( aIndex[ iSmallestThread ] < (int) kTLSData.m_StateLog.size() )
        {
            KLOG(fileName) << "Thread " << iSmallestThread << " : " << kTLSData.m_StateLog[aIndex[ iSmallestThread ]].second.c_str() << fileout;
            aIndex[ iSmallestThread ]++;
        }
    };

	KLOG(fileName) << "_____LOG_END____" << fileout;
	KLOG(fileName) << "\n\n\n\n\n\n\n" << fileout;


}

void CKTDXLog::ErrorLogToFile( const char* fileName, const char* systemInfo )
{

    if ( fileName == NULL || fileName[0] == '\0' )
        return;

    int aIndex[KTDXLOG_MAX_THREAD_NUM];
    memset( aIndex, 0, sizeof(aIndex) );
    while( 1 )
    {
        int     iSmallestThread = -1;
        DWORD   dwSmallestTimestamp = 0;
        for( int iThread = 0; iThread < KTDXLOG_MAX_THREAD_NUM; iThread++ )
        {
            KTLSData& kTLSData = m_aTLSData[ iThread ];
            if ( aIndex[ iThread ] < (int) kTLSData.m_ErrorLog.size() )
            {
                if ( iSmallestThread < 0
                    || kTLSData.m_ErrorLog[ aIndex[ iThread ] ].first < dwSmallestTimestamp )
                {
                    iSmallestThread = iThread;
                    dwSmallestTimestamp = kTLSData.m_ErrorLog[ aIndex[ iThread ] ].first;
                }
            }
        }
        if ( iSmallestThread < 0 )
            break;
        KTLSData& kTLSData = m_aTLSData[ iSmallestThread ];
        if ( aIndex[ iSmallestThread ] < (int) kTLSData.m_ErrorLog.size() )
        {
            KLOG(fileName) << "Thread " << iSmallestThread << " : " << kTLSData.m_ErrorLog[aIndex[ iSmallestThread ]].second.c_str() << fileout;
            aIndex[ iSmallestThread ]++;
        }
    };

    for( int iThread = 0; iThread < KTDXLOG_MAX_THREAD_NUM; iThread++ )
    {
        KTLSData& kTLSData = m_aTLSData[ iThread ];
        if ( kTLSData.m_LastErrorLog.empty() == false )
        {
	        KLOG(fileName) << "Thread " << iThread << " LastErrorLog : " << kTLSData.m_LastErrorLog.c_str() << fileout;
        }
    }


//#if 0
//	KLOG(fileName) << "\n" << fileout;
//	for(UINT i=0; i<m_queLastErrorLog.size(); ++i)
//	{
//		KLOG(fileName) << "LastErrorLog(" << i << ") : " << DeQueLastErrorLog() << fileout;
//	}
//#endif
	KLOG(fileName) << "_____LOG_END____" << fileout;

	if( systemInfo != NULL )
	{
		KLOG(fileName) << "------INFO------" << fileout;
		KLOG(fileName) << systemInfo << fileout;
	}


	KLOG(fileName) << "\n\n\n\n\n\n\n" << fileout;


}

//#if 0
//void CKTDXLog::EnQueLastErrorLog( const WCHAR* errorLog )
//{
//	if(m_queLastErrorLog.size() > 5)
//	{
//		m_queLastErrorLog.pop();
//	}
//
//	time_t t0 = time(0);
//
//#ifdef	CONVERSION_VS
//	struct tm t;
//	localtime_s( &t, &t0);
//	wstringstream wstrstm;
//	wstrstm << 1900+t.tm_year << "/" << 1+t.tm_mon << "/" << t.tm_mday << ", " << t.tm_hour << ":" << t.tm_min << ":" << t.tm_sec << " > " << "ErrorMsg(\"" << errorLog << "\")";
//#else	CONVERSION_VS
//	tm* t = localtime(&t0);
//	wstringstream wstrstm;
//	wstrstm << 1900+t->tm_year << "/" << 1+t->tm_mon << "/" << t->tm_mday << ", " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << " > " << "ErrorMsg(\"" << errorLog << "\")";
//#endif	CONVERSION_VS
//
//	m_queLastErrorLog.push( wstrstm.str().c_str() );
//}
//
//const char* CKTDXLog::DeQueLastErrorLog()
//{
//	if(m_queLastErrorLog.size() > 0)
//	{
//		string strErrorLog = "";
//		wstring wstrErrorLog = m_queLastErrorLog.back();
//        m_queLastErrorLog.pop();
//
//
//		ConvertWCHARToChar( strErrorLog, wstrErrorLog.c_str() );
//
//		return strErrorLog.c_str();
//	}
//
//	return NULL;
//}
//#endif
