// Logging macros used across the server (START_LOG appears ~13.500 times): names and file output kept.
// Literals are not concatenated with __WFUNCTION__/__WFILE__ any more (they are runtime strings,
// see x2/platform/debug.h), everything is streamed instead.
#ifndef TB_DBG_HPP
#define TB_DBG_HPP

#include "logfile.hpp"
#include "assert.hpp"

#define KNC_TIME_FORMAT             _T("%Y-%m-%d %H:%M:%S")
// The macros hand metadata to the logger (decision T16) instead of formatting it: dbg::header, dbg::field and
// dbg::location write the original's text to the file and give the console its structure (logfile.hpp).
#define LOG_HEADER_STR              dbg::header( __WFUNCTION__ )
#define START_LOG( type, data )     dbg::type.Start( WIDENE(type), __WFUNCTION__, __WFILE__, __LINE__ ) << data << dbg::endl
#define START_LOG_WITH_NAME( type ) START_LOG( type, L", Name : " << m_strName )
#define BUILD_LOG( data )           dbg::field( WIDENE(data) ) << data << dbg::endl
#define BUILD_LOGc( data )          dbg::field( WIDENE(data) ) << static_cast<int>(data) << dbg::endl
#define BUILD_LOGtm( data )         dbg::field( WIDENE(data) ) << data.Format( KNC_TIME_FORMAT ) << dbg::endl
#define END_LOG                     dbg::location( __WFILE__, __LINE__ )
// One expression (the caller keeps chaining "<< BUILD_LOG(...) << END_LOG;"). Port: the original declared two locals
// here (rlog, szLogLevel), which a "goto end_proc" from DO_QUERY could not jump over and two of which could not share a scope.
namespace dbg
{
    inline record LogCondition( bool cond, logfile& ltrue, logfile& lfalse, const wchar_t* sztrue, const wchar_t* szfalse,
                                const wchar_t* func, const wchar_t* file, unsigned int line )
    {
        return ( cond ? ltrue : lfalse ).Start( cond ? sztrue : szfalse, func, file, line );
    }
}
#define LOG_CONDITION( cond, ctrue, cfalse )     \
    dbg::LogCondition( (cond), dbg::ctrue, dbg::cfalse, WIDENE(ctrue), WIDENE(cfalse), __WFUNCTION__, __WFILE__, __LINE__ ) << dbg::endl

#define LOG_SUCCESS( success ) LOG_CONDITION( success, clog, cerr )

#define EXCEPTION_LOG( data )       dbg::cout << LOG_HEADER_STR << data << END_LOG
#define PRINT_LASTERROR             dbg::cwarn << NetError::GetLastErrMsg() << L"(" << NetError::GetLastError() << L")" << END_LOG

#ifdef CONVERT_JIF_LIF_TO_ASSERTION
#   define _JIF( x, action )  assert( x )
#   define  LIF(x)            assert( x )
#else
#   define _JIF( x, action )  do { if( !(x) ) { dbg::cerr << dbg::header( __WFUNCTION__, false ) << L"in " WIDENE(x) << END_LOG; action; }} while(0)
#   define  LIF(x)            do { if( !(x) ) { dbg::cerr << dbg::header( __WFUNCTION__, false ) << L"in " WIDENE(x) << END_LOG; }} while(0)
#endif // CONVERT_JIF_LIF_TO_ASSERTION

#define JIF(x) _JIF( x, return (void)0 )

#endif // TB_DBG_HPP
