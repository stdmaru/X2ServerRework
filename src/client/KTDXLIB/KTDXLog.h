#pragma once

#include <atomic>




// The game has exactly one CKTDXLog (g_CKTDXLog): its threads share it.
#define KTDXLOG_MAX_THREAD_NUM 10


class CKTDXLog
{
	public:
		CKTDXLog(void);
		~CKTDXLog(void);

        // A thread calls this before it logs (robobeg, 2014-01-14).
        bool    RegisterCurrentThread();

        // Thread safe (robobeg, 2014-01-14).
		void PushStateLog( const WCHAR* statLog );
		void PushErrorLog( int errNum, const WCHAR* errorLog );

		void PushStateLog( const char* statLog );
		void PushErrorLog( int errNum );
		void PushErrorLog( int errNum, const char* errorLog );

		void SetLastErrorLog( const char* statLog );
		void SetLastErrorLog( const WCHAR* statLog );
        const char *GetLastErrorLog();


        // Not thread safe: called at exit, when every thread has stopped.
		void StateLogToFile( const char* fileName );
		void ErrorLogToFile( const char* fileName, const char* systemInfo = NULL );

        //int GetStateLogSize() { return m_StateLog.size(); }
        //int GetErrorLogSize() { return m_ErrorLog.size(); }
        //const char *GetStateLogStr(int inx) { return m_StateLog[inx].c_str(); }
        //const char *GetErrorLogStr(int inx) { return m_ErrorLog[inx].c_str(); }



	private:


        struct  KTLSData
        {
            std::atomic<DWORD>  m_dwThreadId;
		    std::deque<std::pair<DWORD,string> >	m_StateLog;
		    std::deque<std::pair<DWORD,string> >	m_ErrorLog;
		    string				m_LastErrorLog;

            KTLSData() : m_dwThreadId(0) {}
        };

        KTLSData            m_aTLSData[KTDXLOG_MAX_THREAD_NUM];
		static thread_local KTLSData*
                            ms_pTLSData;
        std::atomic<DWORD>  m_dwTimestamp;

};

extern CKTDXLog g_CKTDXLog;
