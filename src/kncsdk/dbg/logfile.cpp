#include "logfile.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>

#include "x2/platform/console.h"

namespace dbg
{
    logfile cout(   lm_comment,     0 );
    logfile cerr(   lm_error,       0 );
    logfile cwarn(  lm_warning,     1 );
    logfile clog(   lm_info,        2 );

    logfile cout2(  lm_comment2,    0 );
    logfile cerr2(  lm_error2,      0 );
    logfile cwarn2( lm_warning2,    1 );
    logfile clog2(  lm_info2,       2 );

    namespace
    {
        const char* const kDefaultFileName = "log.log";

        // Shared output state. Function-local statics: logging may happen during static initialization.
        struct Output
        {
            std::mutex      mutex;
            std::ofstream   file;
            bool            opened = false;     // tried to open (success or not)
            bool            fileNameAtTime = false;
            std::string     fileName = "Logfile Not Initialize Yet.";
            std::wstring    title = L"Log";
            short           level = 2;
            bool            colorConsole = x2::platform::IsColorTerminal( stdout );
        };

        Output& Out()
        {
            static Output out;
            return out;
        }

        // Pending line of the current thread (one statement may span several logfile objects only through
        // manipulators, which always act on the same object).
        std::wostringstream& Pending()
        {
            thread_local std::wostringstream stm;
            return stm;
        }

        const char* ConsoleColor( e_log_mode mode )
        {
            switch( mode )
            {
            case lm_error:   case lm_error2:   return "\x1b[31m";
            case lm_warning: case lm_warning2: return "\x1b[33m";
            default:                           return nullptr;
            }
        }

        // with Out().mutex held
        void OpenFile( Output& out )
        {
            out.opened = true;
            namespace fs = std::filesystem;
            fs::path path = fs::current_path();
            if( out.fileNameAtTime )
            {
                std::error_code ec;
                fs::create_directories( path / "log", ec );
                path /= "log";
                path /= KncUtil::toNarrowString(
                    CTime::GetCurrentTime().Format( L"log_%Y.%m.%d_%H.%M.%S.log" ).GetString() );
            }
            else
            {
                path /= kDefaultFileName;
            }
            out.fileName = path.string();
            out.file.open( path, std::ios::binary | std::ios::out | std::ios::trunc );
            if( out.file )
            {
                out.file << KncUtil::toNarrowString( out.title )
                         << KncUtil::toNarrowString( CTime::GetCurrentTime().Format( L" for %Y-%m-%d %H:%M:%S" ).GetString() )
                         << "\n\n";
                out.file.flush();
            }
        }

        void Write( e_log_mode mode, const std::string& line )
        {
            Output& out = Out();
            std::lock_guard lock( out.mutex );
            if( !out.opened )
                OpenFile( out );
            if( out.file )
            {
                out.file << line;
                out.file.flush();
            }
            const char* color = out.colorConsole ? ConsoleColor( mode ) : nullptr;
            if( color )
                std::fputs( color, stdout );
            std::fputs( line.c_str(), stdout );
            if( color )
                std::fputs( "\x1b[0m", stdout );
            std::fflush( stdout );
        }
    }

    logfile::logfile( e_log_mode lm, unsigned short sLevel )
        : m_mode( lm ), m_sLevel( static_cast<short>( sLevel ) )
    {
    }

    bool logfile::Enabled() const
    {
        return m_sLevel <= Out().level;
    }

    std::wostream& logfile::Stream()
    {
        return Pending();
    }

    void logfile::Flush( bool bBlankLine )
    {
        std::wostringstream& stm = Pending();
        std::string line = KncUtil::toNarrowString( stm.str() );
        line += bBlankLine ? "\n\n" : "\n";
        stm.str( std::wstring() );
        stm.clear();
        Write( m_mode, line );
    }

    logfile& logfile::operator << ( const wchar_t* sz )
    {
        if( Enabled() )
            Stream() << ( sz ? sz : L"(null)" );
        return *this;
    }

    logfile& logfile::operator << ( const char* sz )
    {
        if( Enabled() )
            Stream() << ( sz ? KncUtil::toWideString( sz ) : std::wstring( L"(null)" ) );
        return *this;
    }

    logfile& logfile::line_file_func( const unsigned int line, const wchar_t* file, const wchar_t* func )
    {
        std::wstring str = file ? file : L"";
        const size_t pos = str.find_last_of( L"\\/" );
        if( pos != std::wstring::npos )
            str = str.substr( pos + 1 );
        return *this << func << L"(" << line << L")::" << str;
    }

    void logfile::CloseFile()
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        out.file.close();
        out.opened = false;     // reopened (truncated) by the next log line, as in the original
    }

    void logfile::SetLogLevel( short sLevel )
    {
        cout << lbreak << L"Change LogLevel (from " << Out().level << L" to " << sLevel << L" )." << lbreak;
        Out().level = sLevel;
    }

    void logfile::SetFileNameAtTime( bool bFileNameAtTime )
    {
        CloseFile();
        Out().fileNameAtTime = bFileNameAtTime;
    }

    void logfile::SetLogFileTitle( const wchar_t* szFileTitle )
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        out.title = szFileTitle ? szFileTitle : L"";
    }

    const char* logfile::GetLogFileName()
    {
        return Out().fileName.c_str();
    }

    std::streamoff logfile::GetLogFileSize()
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        if( !out.file )
            return 0;
        return static_cast<std::streamoff>( out.file.tellp() );
    }

    logfile& logfile::LogStat( const wchar_t* pStrFunctionName, const wchar_t* pStrFileName, const int iLineNum )
    {
        if( !Enabled() )
            return *this;

        const std::wstring wstrLine = std::to_wstring( iLineNum );
        const std::wstring wstrHashKey = std::wstring( pStrFunctionName ) + pStrFileName + wstrLine;
        const unsigned int uiHashKey = static_cast<unsigned int>( std::hash<std::wstring>{}( wstrHashKey ) );

        KLocker lock( m_csLogStat );
        auto [mit, inserted] = m_mapLogStat.try_emplace( uiHashKey );
        if( inserted )
        {
            mit->second.m_wstrFunctionName = pStrFunctionName;
            mit->second.m_wstrFileName = pStrFileName;
            mit->second.m_wstrLineNum = wstrLine;
        }
        ++mit->second.m_iCount;
        return *this;
    }

    void logfile::GetLogStat( std::map< unsigned int, KLogStat >& mapLogStat ) const
    {
        KLocker lock( m_csLogStat );
        mapLogStat = m_mapLogStat;
    }

    void logfile::ClearLogStat()
    {
        KLocker lock( m_csLogStat );
        m_mapLogStat.clear();
    }

    logfile& endl( logfile& l )
    {
        l.Flush( false );
        return l;
    }

    logfile& endp( logfile& l )
    {
        l.Flush( true );
        return l;
    }

    logfile& tab( logfile& l )
    {
        l.Stream() << L"    ";
        return l;
    }

    logfile& lbreak( logfile& l )
    {
        if( !Pending().str().empty() )
            l.Flush( false );
        l.Stream() << L"-----------------------------------------------";
        l.Flush( false );
        return l;
    }

} // dbg
