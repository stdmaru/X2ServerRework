// Logging used everywhere through dbg::cout/cerr/cwarn/clog and the START_LOG family (dbg.hpp).
//
// Same interface as the original (Triple Buffer Software logfile, extended by KOG), different internals
// (decision T16): every logging statement builds one record (dbg::entry) that carries its metadata
// (level, function, time, file/line, BUILD_LOG fields) next to the text. The record is written when the
// statement ends, under a lock, to a UTF-8 text file with the original's exact text and to the console
// with a compact rendering of its own; each destination has its own level. The original wrote HTML
// from all threads without locking.
#pragma once

#include "KncUtil.h"
#include "Thread/Locker.h"

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

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

    // Markers streamed into a record. Each one writes the original's text to the file and gives the
    // console its structure.
    struct manip_t { enum kind_t { endl, endp, tab, lbreak } kind; };
    inline constexpr manip_t endl{ manip_t::endl };
    inline constexpr manip_t endp{ manip_t::endp };
    inline constexpr manip_t tab{ manip_t::tab };
    inline constexpr manip_t lbreak{ manip_t::lbreak };

    // "type : func(), date, " (START_LOG), "func(), date, " (LOG_HEADER_STR) or "func(), " (_JIF)
    struct header_t { const wchar_t* type; const wchar_t* func; bool time; };
    inline header_t header( const wchar_t* func, bool time = true ) { return { nullptr, func, time }; }

    // "    name : " (BUILD_LOG); the value is what is streamed until the next endl
    struct field_t { const wchar_t* name; };
    inline field_t field( const wchar_t* name ) { return { name }; }

    // "    (file, line)" and end of line (END_LOG)
    struct location_t { const wchar_t* file; unsigned int line; };
    inline location_t location( const wchar_t* file, unsigned int line ) { return { file, line }; }

    // One logging statement.
    struct entry
    {
        // Console structure: the first line after a header is its message; fields come from BUILD_LOG,
        // separators from lbreak, everything else is text.
        struct line_t
        {
            enum kind_t { message, text, field, separator } kind;
            std::wstring name;      // field only
            std::wstring value;
        };

        e_log_mode      mode = lm_comment;
        short           level = 0;
        CTime           time;
        const wchar_t*  type = nullptr;     // "cerr", ... (START_LOG / LOG_CONDITION)
        const wchar_t*  func = nullptr;
        const wchar_t*  file = nullptr;
        unsigned int    line = 0;

        std::wstring            text;       // file rendering: exactly the original's text
        std::vector<line_t>     lines;      // console rendering
        bool                    open = false;   // lines.back() still takes text

        std::optional<std::wostringstream>  fmt;    // formats values other than strings; flags stick for the statement

        void Append( std::wstring_view str );
        void EndLine();
        void Header( const header_t& h );
        void Field( const wchar_t* name );
        void Location( const wchar_t* file, unsigned int line );
        void Break();
    };

    class logfile;

    // Built by the first "<<" on a logfile (or by Start) and written when the statement ends.
    class record
    {
    public:
        explicit record( logfile& log );
        record( logfile& log, const wchar_t* type, const wchar_t* func, const wchar_t* file, unsigned int line );
        record( record&& other ) noexcept = default;
        record& operator = ( record&& ) = delete;
        ~record();

        template<class T> record& operator << ( const T& data )
        {
            if( m_entry )
            {
                *BeginValue() << data;
                EndValue();
            }
            return *this;
        }

        record& operator << ( const wchar_t* sz );
        record& operator << ( wchar_t* sz ) { return *this << static_cast<const wchar_t*>( sz ); }
        record& operator << ( const char* sz );
        record& operator << ( char* sz ) { return *this << static_cast<const char*>( sz ); }
        record& operator << ( const std::wstring& str );
        record& operator << ( const std::string& str ) { return *this << str.c_str(); }
        record& operator << ( const CStringW& str ) { return *this << str.GetString(); }
        record& operator << ( bool b ) { return *this << ( b ? L"true" : L"false" ); }
        record& operator << ( manip_t m );
        record& operator << ( const header_t& h );
        record& operator << ( const field_t& f );
        record& operator << ( const location_t& l );

    private:
        std::wostream* BeginValue();    // the record's formatting stream, emptied
        void EndValue();                // appends what was formatted

        std::unique_ptr<entry>  m_entry;    // null when no destination takes this level
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

        // Receives the console rendering of each record (UTF-8, complete lines). The default writes to stdout.
        using console_writer = std::function<void( std::string_view )>;
        // Receives each record the console takes, structured (the control protocol, T30). Called under the log lock.
        using record_sink = std::function<void( const entry& )>;

        logfile( e_log_mode lm, unsigned short usLevel );
        ~logfile() = default;

        template<class T> record operator << ( const T& data )
        {
            record r( *this );
            r << data;
            return r;
        }

        // START_LOG: counts the call site (LogStat) and opens a record with its metadata.
        record Start( const wchar_t* type, const wchar_t* func, const wchar_t* file, unsigned int line );

        logfile& LogStat( const wchar_t* pStrFunctionName, const wchar_t* pStrFileName, const int iLineNum );
        void GetLogStat( std::map< unsigned int, KLogStat >& mapLogStat ) const;
        void ClearLogStat();

        e_log_mode GetMode() const { return m_mode; }
        short GetLevel() const { return m_sLevel; }

        static void CloseFile();
        static void SetLogLevel( short sLevel );        // file (and console, unless it has its own level)
        static void SetConsoleLogLevel( short sLevel ); // console only; -1 follows the file level again
        static short GetConsoleLogLevel();              // the effective level
        static void SetConsoleWriter( console_writer writer, bool bColor );    // empty: back to stdout
        static void SetRecordSink( record_sink sink );                          // empty: none
        static void SetFileNameAtTime( bool bFileNameAtTime );   // true: one file per run under log/
        static void SetLogFileTitle( const wchar_t* szFileTitle );
        static const char* GetLogFileName();
        static std::streamoff GetLogFileSize();

        // Console rendering of a record (empty when it has nothing to show); exposed for the tests.
        static std::string RenderConsole( const entry& e, bool color );
        // One JSON line (no newline): {"type":"log","time":...,"level":...,"message":...,"fields":[...]} (docs/porting/kncsdk.md).
        static std::string RenderJson( const entry& e );

    private:
        friend class record;

        bool Enabled() const;
        static void Write( entry& e );

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

} // dbg
