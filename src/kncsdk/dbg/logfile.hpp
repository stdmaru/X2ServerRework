// Logging used everywhere through dbg::cout/cerr/cwarn/clog and the START_LOG family (dbg.hpp).
//
// Same interface as the original (Triple Buffer Software logfile, extended by KOG), different internals:
// every thread builds its line in its own buffer and complete lines are written under a lock, to a
// UTF-8 text file and to the console. The original wrote HTML from all threads without locking.
#pragma once

#include "KncUtil.h"
#include "Thread/Locker.h"

#include <map>
#include <ostream>
#include <string>

namespace dbg
{
    enum e_log_mode
    {
        lm_comment, ///< Logs in comment mode
        lm_error,   ///< Logs in error mode
        lm_warning, ///< Logs in warning mode
        lm_info,    ///< Logs in info mode
        lm_comment2,
        lm_error2,
        lm_warning2,
        lm_info2,
        lm_number_of_modes,
    };

    class logfile
    {
    public:
        struct KLogStat
        {
            std::wstring    m_wstrFunctionName;
            std::wstring    m_wstrFileName;
            std::wstring    m_wstrLineNum;
            int             m_iCount = 0;
        };

        logfile( e_log_mode lm, unsigned short usLevel );
        ~logfile() = default;

        logfile& line_file_func( const unsigned int line, const wchar_t* file, const wchar_t* func );

        template<class T> logfile& operator << ( const T& data )
        {
            if( Enabled() )
                Stream() << data;
            return *this;
        }

        logfile& operator << ( const wchar_t* sz );
        logfile& operator << ( wchar_t* sz ) { return *this << static_cast<const wchar_t*>( sz ); }
        logfile& operator << ( const char* sz );
        logfile& operator << ( char* sz ) { return *this << static_cast<const char*>( sz ); }
        logfile& operator << ( const std::wstring& str ) { return *this << str.c_str(); }
        logfile& operator << ( const std::string& str ) { return *this << str.c_str(); }
        logfile& operator << ( const CStringW& str ) { return *this << str.GetString(); }
        logfile& operator << ( bool b ) { return *this << ( b ? L"true" : L"false" ); }
        logfile& operator << ( logfile& (*manip)( logfile& ) ) { return Enabled() ? manip( *this ) : *this; }

        logfile& LogStat( const wchar_t* pStrFunctionName, const wchar_t* pStrFileName, const int iLineNum );
        void GetLogStat( std::map< unsigned int, KLogStat >& mapLogStat ) const;
        void ClearLogStat();

        friend logfile& endl( logfile& );
        friend logfile& endp( logfile& );
        friend logfile& tab( logfile& );
        friend logfile& lbreak( logfile& );

        static void CloseFile();
        static void SetLogLevel( short sLevel );
        static void SetFileNameAtTime( bool bFileNameAtTime );   // true: one file per run under log/
        static void SetLogFileTitle( const wchar_t* szFileTitle );
        static const char* GetLogFileName();
        static std::streamoff GetLogFileSize();

    private:
        bool Enabled() const;
        std::wostream& Stream();    // current thread's pending line
        void Flush( bool bBlankLine );

        e_log_mode  m_mode;
        short       m_sLevel;

        std::map< unsigned int, KLogStat >  m_mapLogStat;
        mutable KncCriticalSection          m_csLogStat;
    };

    extern logfile cout;
    extern logfile cerr;
    extern logfile cwarn;
    extern logfile clog;

    extern logfile cout2;
    extern logfile cerr2;
    extern logfile cwarn2;
    extern logfile clog2;

    logfile& endl( logfile& );
    logfile& endp( logfile& );
    logfile& tab( logfile& );
    logfile& lbreak( logfile& );

} // dbg
