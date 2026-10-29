// Logging macros used across the server (START_LOG appears ~13.500 times): names and output kept.
// Literals are not concatenated with __WFUNCTION__/__WFILE__ any more (they are runtime strings,
// see x2/platform/debug.h), everything is streamed instead.
#ifndef TB_DBG_HPP
#define TB_DBG_HPP

#include "logfile.hpp"
#include "assert.hpp"

#define KNC_TIME_FORMAT             _T("%Y-%m-%d %H:%M:%S")
#define LOG_HEADER_STR              __WFUNCTION__ << L"(), " << CTime::GetCurrentTime().Format( KNC_TIME_FORMAT ) << L", "
#define START_LOG( type, data )     dbg::type.LogStat( __WFUNCTION__, __WFILE__, __LINE__ ) << WIDENE(type) L" : " << LOG_HEADER_STR << data << dbg::endl
#define START_LOG_WITH_NAME( type ) START_LOG( type, L", Name : " << m_strName )
#define BUILD_LOG( data )           dbg::tab << WIDENE(data) L" : " << data << dbg::endl
#define BUILD_LOGc( data )          dbg::tab << WIDENE(data) L" : " << static_cast<int>(data) << dbg::endl
#define BUILD_LOGtm( data )         dbg::tab << WIDENE(data) L" : " << data.Format( KNC_TIME_FORMAT ) << dbg::endl
#define END_LOG                     dbg::tab << L"(" << __WFILE__ << L", " << __LINE__ << L")" << dbg::endl
#define LOG_CONDITION( cond, ctrue, cfalse )     \
    dbg::logfile& rlog = cond ? dbg::ctrue : dbg::cfalse; \
    const wchar_t* szLogLevel = cond ? WIDENE(ctrue) : WIDENE(cfalse); \
    rlog << szLogLevel << L" : " << LOG_HEADER_STR << dbg::endl

#define LOG_SUCCESS( success ) LOG_CONDITION( success, clog, cerr )

#define EXCEPTION_LOG( data )       dbg::cout << LOG_HEADER_STR << data << END_LOG
#define PRINT_LASTERROR             dbg::cwarn << NetError::GetLastErrMsg() << L"(" << NetError::GetLastError() << L")" << END_LOG

#ifdef CONVERT_JIF_LIF_TO_ASSERTION
#   define _JIF( x, action )  assert( x )
#   define  LIF(x)            assert( x )
#else
#   define _JIF( x, action )  do { if( !(x) ) { dbg::cerr << __WFUNCTION__ << L"(), in " WIDENE(x) << END_LOG; action; }} while(0)
#   define  LIF(x)            do { if( !(x) ) { dbg::cerr << __WFUNCTION__ << L"(), in " WIDENE(x) << END_LOG; }} while(0)
#endif // CONVERT_JIF_LIF_TO_ASSERTION

#define JIF(x) _JIF( x, return (void)0 )

#endif // TB_DBG_HPP
