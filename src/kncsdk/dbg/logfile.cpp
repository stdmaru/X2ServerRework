#include "logfile.hpp"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>

#include "x2/platform/console.h"
#include "x2/platform/json.h"

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
        const wchar_t* const kTimeFormat = L"%Y-%m-%d %H:%M:%S";
        const wchar_t* const kTab = L"    ";

        // Shared output state. Function-local static: logging may happen during static initialization.
        struct Output
        {
            std::mutex      mutex;
            std::ofstream   file;
            bool            opened = false;     // tried to open (success or not)
            bool            fileNameAtTime = false;
            std::string     fileName = "Logfile Not Initialize Yet.";
            std::wstring    title = L"Log";
            std::atomic<short>  level{ 2 };         // file
            std::atomic<short>  consoleLevel{ -1 }; // -1: same as the file
            bool            colorConsole = x2::platform::IsColorTerminal( stdout );
            logfile::console_writer consoleWriter;
            logfile::record_sink    recordSink;

            short ConsoleLevel() const { const short c = consoleLevel; return c < 0 ? level.load() : c; }
        };

        Output& Out()
        {
            static Output out;
            return out;
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

        // Console tag and color of each mode (the "2" streams look like their base ones).
        struct ConsoleStyle { const char* tag; const char* color; };

        ConsoleStyle StyleOf( e_log_mode mode )
        {
            switch( mode )
            {
            case lm_error:   case lm_error2:   return { "ERROR", "\x1b[1;31m" };
            case lm_warning: case lm_warning2: return { "WARN ", "\x1b[33m" };
            case lm_info:    case lm_info2:    return { "INFO ", "\x1b[36m" };
            default:                           return { "NOTE ", "\x1b[32m" };
            }
        }

        std::wstring_view TrimLeft( std::wstring_view str )
        {
            const size_t pos = str.find_first_not_of( L" \t" );
            return pos == std::wstring_view::npos ? std::wstring_view() : str.substr( pos );
        }

        std::wstring_view TrimRight( std::wstring_view str )
        {
            const size_t pos = str.find_last_not_of( L" \t\r" );
            return pos == std::wstring_view::npos ? std::wstring_view() : str.substr( 0, pos + 1 );
        }

        void AppendUtf8( std::string& out, std::wstring_view str )
        {
            out += KncUtil::toNarrowString( std::wstring( str ) );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // entry

    void entry::Append( std::wstring_view str )
    {
        text += str;
        for( ;; )
        {
            const size_t pos = str.find( L'\n' );
            std::wstring_view piece = str.substr( 0, pos );
            if( !piece.empty() && piece.back() == L'\r' )
                piece.remove_suffix( 1 );
            if( !piece.empty() )
            {
                if( !open )
                {
                    lines.push_back( { line_t::text, {}, {} } );
                    open = true;
                }
                lines.back().value += piece;
            }
            if( pos == std::wstring_view::npos )
                break;
            open = false;
            str.remove_prefix( pos + 1 );
        }
    }

    void entry::EndLine()
    {
        text += L'\n';
        open = false;
    }

    void entry::Header( const header_t& h )
    {
        if( h.type )
        {
            text += h.type;
            text += L" : ";
            type = h.type;
        }
        if( h.func )
        {
            text += h.func;
            func = h.func;
        }
        text += L"(), ";
        if( h.time )
        {
            text += time.Format( kTimeFormat ).GetString();
            text += L", ";
        }
        lines.push_back( { line_t::message, {}, {} } );
        open = true;
    }

    void entry::Field( const wchar_t* name )
    {
        text += kTab;
        text += name;
        text += L" : ";
        lines.push_back( { line_t::field, name, {} } );
        open = true;
    }

    void entry::Location( const wchar_t* fileName, unsigned int lineNum )
    {
        text += kTab;
        text += L"(";
        text += fileName ? fileName : L"";
        text += L", ";
        text += std::to_wstring( lineNum );
        text += L")\n";
        open = false;
        if( !file )
        {
            file = fileName;
            line = lineNum;
        }
    }

    void entry::Break()
    {
        if( !text.empty() && text.back() != L'\n' )
            text += L'\n';
        text += L"-----------------------------------------------\n";
        lines.push_back( { line_t::separator, {}, {} } );
        open = false;
    }

    //////////////////////////////////////////////////////////////////////////
    // record

    record::record( logfile& log )
    {
        if( log.Enabled() )
        {
            m_entry = std::make_unique<entry>();
            m_entry->mode = log.m_mode;
            m_entry->level = log.m_sLevel;
            m_entry->time = CTime::GetCurrentTime();
        }
    }

    record::record( logfile& log, const wchar_t* type, const wchar_t* func, const wchar_t* file, unsigned int line )
        : record( log )
    {
        if( m_entry )
        {
            m_entry->file = file;
            m_entry->line = line;
            m_entry->Header( { type, func, true } );
        }
    }

    record::~record()
    {
        if( m_entry )
            logfile::Write( *m_entry );
    }

    std::wostream* record::BeginValue()
    {
        if( !m_entry->fmt )
            m_entry->fmt.emplace();
        m_entry->fmt->str( std::wstring() );
        return &*m_entry->fmt;
    }

    void record::EndValue()
    {
        m_entry->Append( m_entry->fmt->view() );
    }

    record& record::operator << ( const wchar_t* sz )
    {
        if( m_entry )
            m_entry->Append( sz ? sz : L"(null)" );
        return *this;
    }

    record& record::operator << ( const char* sz )
    {
        if( m_entry )
            m_entry->Append( sz ? KncUtil::toWideString( sz ) : std::wstring( L"(null)" ) );
        return *this;
    }

    record& record::operator << ( const std::wstring& str )
    {
        if( m_entry )
            m_entry->Append( str );
        return *this;
    }

    record& record::operator << ( manip_t m )
    {
        if( !m_entry )
            return *this;
        switch( m.kind )
        {
        case manip_t::endl:
            m_entry->EndLine();
            break;
        case manip_t::endp:
            m_entry->EndLine();
            m_entry->EndLine();
            break;
        case manip_t::tab:
            m_entry->text += kTab;
            if( m_entry->open )
                m_entry->lines.back().value += kTab;
            break;
        case manip_t::lbreak:
            m_entry->Break();
            break;
        }
        return *this;
    }

    record& record::operator << ( const header_t& h )
    {
        if( m_entry )
            m_entry->Header( h );
        return *this;
    }

    record& record::operator << ( const field_t& f )
    {
        if( m_entry )
            m_entry->Field( f.name );
        return *this;
    }

    record& record::operator << ( const location_t& l )
    {
        if( m_entry )
            m_entry->Location( l.file, l.line );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // logfile

    logfile::logfile( e_log_mode lm, unsigned short sLevel )
        : m_mode( lm ), m_sLevel( static_cast<short>( sLevel ) )
    {
    }

    bool logfile::Enabled() const
    {
        const Output& out = Out();
        return m_sLevel <= std::max( out.level.load(), out.ConsoleLevel() );
    }

    record logfile::Start( const wchar_t* type, const wchar_t* func, const wchar_t* file, unsigned int line )
    {
        LogStat( func, file, static_cast<int>( line ) );
        return record( *this, type, func, file, line );
    }

    void logfile::Write( entry& e )
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        if( e.level <= out.level )
        {
            if( !out.opened )
                OpenFile( out );
            if( out.file )
            {
                if( e.text.empty() || e.text.back() != L'\n' )
                    e.text += L'\n';
                out.file << KncUtil::toNarrowString( e.text );
                out.file.flush();
            }
        }
        if( e.level <= out.ConsoleLevel() )
        {
            if( out.recordSink )
                out.recordSink( e );
            const std::string console = RenderConsole( e, out.colorConsole );
            if( console.empty() )
                return;
            if( out.consoleWriter )
            {
                out.consoleWriter( console );
            }
            else
            {
                std::fwrite( console.data(), 1, console.size(), stdout );
                std::fflush( stdout );
            }
        }
    }

    // HH:MM:SS LEVEL Function: message
    //                name  = value
    //                more text
    std::string logfile::RenderConsole( const entry& e, bool color )
    {
        constexpr size_t kIndent = 15;      // "HH:MM:SS LEVEL "
        constexpr size_t kMaxNameWidth = 32;

        const char* const dim = color ? "\x1b[2m" : "";
        const char* const bold = color ? "\x1b[1m" : "";
        const char* const reset = color ? "\x1b[0m" : "";

        // What the console shows: no separators or blank lines; the message line stays even if empty.
        std::vector<const entry::line_t*> lines;
        size_t nameWidth = 0;
        for( const entry::line_t& l : e.lines )
        {
            if( l.kind == entry::line_t::separator )
                continue;
            if( l.kind == entry::line_t::text && TrimRight( l.value ).empty() )
                continue;
            if( l.kind == entry::line_t::field )
                nameWidth = std::max( nameWidth, std::min( l.name.size(), kMaxNameWidth ) );
            lines.push_back( &l );
        }
        if( lines.empty() )
            return {};

        const ConsoleStyle style = StyleOf( e.mode );
        std::string out;
        out += dim;
        out += KncUtil::toNarrowString( e.time.Format( L"%H:%M:%S" ).GetString() );
        out += reset;
        out += ' ';
        if( color )
            out += style.color;
        out += style.tag;
        out += reset;
        out += ' ';

        bool first = true;
        for( const entry::line_t* l : lines )
        {
            if( !first )
                out.append( kIndent, ' ' );
            if( first && e.func )
            {
                out += bold;
                AppendUtf8( out, e.func );
                out += reset;
                out += ':';
                if( !( l->kind == entry::line_t::message && TrimRight( l->value ).empty() ) )
                    out += ' ';
            }
            switch( l->kind )
            {
            case entry::line_t::field:
                out += dim;
                AppendUtf8( out, l->name );
                if( l->name.size() < nameWidth )
                    out.append( nameWidth - l->name.size(), ' ' );
                out += " =";
                out += reset;
                if( !TrimRight( l->value ).empty() )
                    out += ' ';
                AppendUtf8( out, TrimRight( l->value ) );
                break;
            case entry::line_t::message:
                AppendUtf8( out, TrimRight( l->value ) );
                break;
            default:
                // raw text keeps its own layout on the first line (dumps, tables); below it, the tab goes
                AppendUtf8( out, TrimRight( first ? l->value : TrimLeft( l->value ) ) );
                break;
            }
            out += '\n';
            first = false;
        }
        return out;
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
        cout << lbreak << L"Change LogLevel (from " << Out().level.load() << L" to " << sLevel << L" )." << lbreak;
        Out().level = sLevel;
    }

    void logfile::SetConsoleLogLevel( short sLevel )
    {
        Output& out = Out();
        cout << L"Change console LogLevel (from " << out.ConsoleLevel() << L" to "
             << ( sLevel < 0 ? out.level.load() : sLevel ) << L" )." << endl;
        out.consoleLevel = static_cast<short>( sLevel < 0 ? -1 : sLevel );
    }

    short logfile::GetConsoleLogLevel()
    {
        return Out().ConsoleLevel();
    }

    void logfile::SetConsoleWriter( console_writer writer, bool bColor )
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        out.colorConsole = writer ? bColor : x2::platform::IsColorTerminal( stdout );
        out.consoleWriter = std::move( writer );
    }

    void logfile::SetRecordSink( record_sink sink )
    {
        Output& out = Out();
        std::lock_guard lock( out.mutex );
        out.recordSink = std::move( sink );
    }

    std::string logfile::RenderJson( const entry& e )
    {
        using x2::platform::AppendJsonString;
        auto narrow = []( std::wstring_view str ) { return KncUtil::toNarrowString( std::wstring( TrimRight( str ) ) ); };

        static const char* const kStreams[lm_number_of_modes] = { "cout", "cerr", "cwarn", "clog", "cout2", "cerr2", "cwarn2", "clog2" };
        const char* level = "note";
        switch( e.mode )
        {
        case lm_error:   case lm_error2:   level = "error"; break;
        case lm_warning: case lm_warning2: level = "warn";  break;
        case lm_info:    case lm_info2:    level = "info";  break;
        default: break;
        }

        // The message is the header's line (or the first line of a record without one); the other lines are text
        const entry::line_t* message = nullptr;
        for( const entry::line_t& l : e.lines )
            if( l.kind == entry::line_t::message || ( l.kind == entry::line_t::text && !TrimRight( l.value ).empty() ) )
            {
                message = &l;
                break;
            }

        std::string out = "{\"type\":\"log\",\"time\":";
        AppendJsonString( out, KncUtil::toNarrowString( e.time.Format( kTimeFormat ).GetString() ) );
        out += ",\"level\":\"";
        out += level;
        out += "\",\"stream\":\"";
        out += kStreams[e.mode < lm_number_of_modes ? e.mode : 0];
        out += '"';
        if( e.func )
        {
            out += ",\"func\":";
            AppendJsonString( out, KncUtil::toNarrowString( e.func ) );
        }
        if( e.file )
        {
            out += ",\"file\":";
            AppendJsonString( out, KncUtil::toNarrowString( e.file ) );
            out += ",\"line\":" + std::to_string( e.line );
        }
        out += ",\"message\":";
        AppendJsonString( out, message ? narrow( message->value ) : std::string() );
        out += ",\"fields\":[";
        bool first = true;
        for( const entry::line_t& l : e.lines )
        {
            if( l.kind != entry::line_t::field )
                continue;
            if( !first )
                out += ',';
            first = false;
            out += "{\"name\":";
            AppendJsonString( out, narrow( l.name ) );
            out += ",\"value\":";
            AppendJsonString( out, narrow( l.value ) );
            out += '}';
        }
        out += "],\"text\":[";
        first = true;
        for( const entry::line_t& l : e.lines )
        {
            if( &l == message || l.kind != entry::line_t::text || TrimRight( l.value ).empty() )
                continue;
            if( !first )
                out += ',';
            first = false;
            AppendJsonString( out, narrow( l.value ) );
        }
        out += "]}";
        return out;
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

} // dbg
