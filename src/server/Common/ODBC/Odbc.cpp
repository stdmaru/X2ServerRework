#include "Odbc.h"
#include "dbg/dbg.hpp"
#include <Thread/Locker.h>
#include <KncUtil.h>
#include "x2/platform/ini.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>

namespace {

using x2::platform::FromSqlWString;
using x2::platform::SqlWPtr;
using x2::platform::SqlWString;
using x2::platform::ToSqlWString;

template< size_t N >
void CopyTruncated( wchar_t ( &dst )[N], const std::wstring& src )
{
    const size_t n = std::min( src.size(), N - 1 );
    std::copy_n( src.begin(), n, dst );
    dst[n] = L'\0';
}

std::wstring Trim( const std::wstring& s )
{
    const size_t b = s.find_first_not_of( L" \t\r\n" );
    if( b == std::wstring::npos )
        return std::wstring();
    return s.substr( b, s.find_last_not_of( L" \t\r\n" ) - b + 1 );
}

bool IEquals( const std::wstring& a, const wchar_t* b )
{
    const std::wstring_view bv( b );
    return a.size() == bv.size()
        && std::equal( a.begin(), a.end(), bv.begin(), []( wchar_t x, wchar_t y ) { return ::towupper( x ) == ::towupper( y ); } );
}

// braces quote a value of a connection string; a '}' inside is doubled
std::wstring QuoteConnValue( const std::wstring& value )
{
    if( value.find_first_of( L" ;{}" ) == std::wstring::npos )
        return value;

    std::wstring quoted = L"{";
    for( wchar_t c : value )
    {
        quoted += c;
        if( c == L'}' )
            quoted += c;
    }
    return quoted + L"}";
}

} // namespace

KODBC::KMaxProcessingInfo KODBC::ms_kMaxProcessingInfo = {
    KncCriticalSection(), L"초기상태", 0, CTime::GetCurrentTime()
};

std::wstring KODBC::ms_wstrODBCDriver  = L"ODBC Driver 18 for SQL Server";
std::wstring KODBC::ms_wstrODBCOptions = L"Encrypt=yes;TrustServerCertificate=yes";

void KODBC::SetODBCDriver( const std::wstring& wstrDriver )
{
    ms_wstrODBCDriver = wstrDriver;
    ms_wstrODBCOptions.clear();
}

void KODBC::SetODBCOptions( const std::wstring& wstrOptions )
{
    ms_wstrODBCOptions = wstrOptions;
}

// The [ODBC] section of a file DSN. The original passed "FILEDSN=<path>" to the Windows driver manager; unixODBC
// reads only narrow files and the original .dsn files are UTF-16, so the file is read here (UTF-16, UTF-8 or CP949).
bool KODBC::ReadFileDSN( const std::wstring& wstrPath, KDsnKeys& vecKeys )
{
    vecKeys.clear();

    std::ifstream file( std::filesystem::path( wstrPath ), std::ios::binary );
    if( !file )
        return false;

    const std::string bytes{ std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>() };
    std::wistringstream lines( x2::platform::DecodeTextFile( bytes ) );

    bool bInSection = false;
    for( std::wstring line; std::getline( lines, line ); )
    {
        line = Trim( line );
        if( line.empty() || line[0] == L';' )
            continue;

        if( line[0] == L'[' )
        {
            bInSection = IEquals( line, L"[ODBC]" );
            continue;
        }

        const size_t pos = line.find( L'=' );
        if( bInSection && pos != std::wstring::npos )
            vecKeys.emplace_back( Trim( line.substr( 0, pos ) ), Trim( line.substr( pos + 1 ) ) );
    }

    return !vecKeys.empty();
}

std::wstring KODBC::BuildConnStrFromDsn( const KDsnKeys& vecKeys, const std::wstring& wstrDriver, const std::wstring& wstrOptions )
{
    std::wstring wstrConnStr = L"DRIVER=" + QuoteConnValue( wstrDriver ) + L";";
    for( const auto& kKey : vecKeys )
    {
        if( IEquals( kKey.first, L"DRIVER" ) || IEquals( kKey.first, L"WSID" ) )
            continue;

        wstrConnStr += kKey.first + L"=" + QuoteConnValue( kKey.second ) + L";";
    }

    const size_t pos = wstrOptions.find_first_not_of( L';' );
    if( pos != std::wstring::npos )
        wstrConnStr += wstrOptions.substr( pos );

    return wstrConnStr;
}

KODBC::KODBC()
    : m_szDiagMsg{}, m_sqlMsgLen( 0 ), m_sqlState{}, m_sqlNativeErr( 0 )
{
    m_hEnv          = SQL_NULL_HENV;
    m_hDbc          = SQL_NULL_HDBC;
    m_hStmt         = SQL_NULL_HSTMT;

    m_usColumNum    = 0;

    InitializeCriticalSection(&m_cs );

	m_bDBConnStr	= false;

} // KODBC()


KODBC::~KODBC()
{
    Close();
    DeleteCriticalSection(&m_cs );
} // ~KODBC()

bool KODBC::DriverConnect( const wchar_t* szConnStr, bool bDBConnStr )
{
	m_bDBConnStr = bDBConnStr;

	if( m_bDBConnStr )
	{
		KClearString< std::wstring > kConnStr( szConnStr );
		return DriverConnectString( kConnStr );
	}
	else
	{
		return DriverConnectFileDSN( szConnStr );
	}
}

SQLRETURN KODBC::ReadDiag( SQLSMALLINT sHandleType, SQLHANDLE hHandle )
{
    SQLWCHAR szState[6] = {};
    SQLWCHAR szMsg[SQL_MAX_MESSAGE_LENGTH] = {};
    m_sqlMsgLen = 0;
    m_sqlNativeErr = 0;

    const SQLRETURN ret = ::SQLGetDiagRecW( sHandleType, hHandle, 1, szState, &m_sqlNativeErr,
                                            szMsg, SQL_MAX_MESSAGE_LENGTH, &m_sqlMsgLen );
    if( SQLOK( ret ) )
    {
        CopyTruncated( m_sqlState, FromSqlWString( szState ) );
        const SQLSMALLINT len = std::clamp<SQLSMALLINT>( m_sqlMsgLen, 0, SQL_MAX_MESSAGE_LENGTH - 1 );
        CopyTruncated( m_szDiagMsg, FromSqlWString( szMsg, len ) );
    }
    else
    {
        m_sqlState[0] = L'\0';
        m_szDiagMsg[0] = L'\0';
    }

    return ret;
}

bool KODBC::AllocConnection( const std::wstring& wstrConnStr, const std::wstring& wstrLogName )
{
    // allocate environment handle.
    SQLRETURN Ret = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );

    if( !SQLOK( Ret ) )
    {
        START_LOG( cerr, L"DriverConnect() Failed. Ret : " << Ret )
            << BUILD_LOG( wstrLogName )
            << END_LOG;

        return false;
    }

    // set the ODBC version environment attribute.
    Ret = ::SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, ( SQLPOINTER )SQL_OV_ODBC3, SQL_IS_INTEGER );

    if( !SQLOK( Ret ) )
    {
        ReadDiag( SQL_HANDLE_ENV, m_hEnv );
        START_LOG( cerr, L"DriverConnect() Failed. Ret : " << Ret )
            << BUILD_LOG( wstrLogName )
            << BUILD_LOG( m_sqlState )
            << BUILD_LOG( m_szDiagMsg )
            << END_LOG;

        return false;
    }

    // allocate connection handle.
    Ret = ::SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDbc );

    if( !SQLOK( Ret ) )
    {
        ReadDiag( SQL_HANDLE_ENV, m_hEnv );
        START_LOG( cerr, L"DriverConnect() Failed. Ret : " << Ret )
            << BUILD_LOG( wstrLogName )
            << BUILD_LOG( m_sqlState )
            << BUILD_LOG( m_szDiagMsg )
            << END_LOG;

        return false;
    }

    SqlWString InCon = ToSqlWString( wstrConnStr );
    SQLWCHAR OutCon[1024];
    SQLSMALLINT cbOutCon = 0;
    Ret = ::SQLDriverConnectW( m_hDbc, NULL, SqlWPtr( InCon ), SQL_NTS, OutCon,
                               static_cast<SQLSMALLINT>( std::size( OutCon ) ), &cbOutCon, SQL_DRIVER_NOPROMPT );

    if( !SQLOK( Ret ) )
    {
        ReadDiag( SQL_HANDLE_DBC, m_hDbc );
        START_LOG( cerr, L"DriverConnect() Failed. Ret : " << Ret )
            << BUILD_LOG( wstrLogName )
            << BUILD_LOG( m_sqlState )
            << BUILD_LOG( m_szDiagMsg )
            << END_LOG;

        return false;
    }

    // allocate statement handle.
    Ret = ::SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmt );

    if( !SQLOK( Ret ) )
    {
        ReadDiag( SQL_HANDLE_DBC, m_hDbc );
        START_LOG( cerr, L"DriverConnect() Failed. Ret : " << Ret )
            << BUILD_LOG( wstrLogName )
            << BUILD_LOG( m_sqlState )
            << BUILD_LOG( m_szDiagMsg )
            << END_LOG;

        return false;
    }

    dbg::cout << L"DriverConnect() success. m_strDSN : " << wstrLogName << dbg::endl;

    return true;
}

bool KODBC::DriverConnectFileDSN( const wchar_t* szDsn_ )
{
    if( m_strDSN.size() <= 0 )
    {
        // relative to the working directory, like the original; the configs write the paths with '\'
        std::wstring strDsn_ = szDsn_;
#if !defined(_WIN32)
        std::replace( strDsn_.begin(), strDsn_.end(), L'\\', L'/' );
#endif
        m_strAbsolutePathDSN = ( std::filesystem::current_path() / strDsn_ ).wstring();
        m_strDSN = L"FILEDSN=" + m_strAbsolutePathDSN;
    }

    KDsnKeys vecKeys;
    if( !ReadFileDSN( m_strAbsolutePathDSN, vecKeys ) )
    {
        START_LOG( cerr, L"DriverConnect() Failed. Cannot read the file DSN." )
            << BUILD_LOG( m_strDSN )
            << END_LOG;

        return false;
    }

    return AllocConnection( BuildConnStrFromDsn( vecKeys, ms_wstrODBCDriver, ms_wstrODBCOptions ), m_strDSN );
}

//{{ 2009. 6. 15  최육사	conn string
bool KODBC::DriverConnectString( const KClearString< std::wstring >& kDBConnStr )
{
	// Security String
	if( m_SecurityDBConnStr.IsEmpty() )
	{
		m_SecurityDBConnStr.Init( kDBConnStr.GetString() );

		// keep the string without the password: only the database name, for the logs
		m_strDBConnStr = kDBConnStr.GetString();

		size_t index = m_strDBConnStr.find( L";DATABASE=" );
		if( index != std::wstring::npos )
		{
			m_strDBConnStr.erase( 0, index + 10 );

			index = m_strDBConnStr.find( L";" );
			if( index != std::wstring::npos )
			{
				m_strDBConnStr.erase( index, m_strDBConnStr.size() - index );
			}
		}
	}

	return AllocConnection( m_SecurityDBConnStr.GetClearString().GetString(), m_strDBConnStr );
}
//}}

bool KODBC::ReConnect()
{
    Close();

	bool bRet = false;
	if( m_bDBConnStr )
	{
		bRet = DriverConnectString( KClearString< std::wstring >() );
	}
	else
	{
		bRet = DriverConnectFileDSN( m_strDSN.c_str() );
	}

    if( !bRet )
    {
        dbg::cerr << L"재접속 시도 실패" << END_LOG;
        return false;
    }
    else
    {
        START_LOG( cout, L"재접속 성공" );
        return true;
    }
}

void KODBC::Close()
{
    if( SQL_NULL_HSTMT != m_hStmt )
    {
        ::SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
        m_hStmt = SQL_NULL_HSTMT;
    } // if

    if( SQL_NULL_HDBC != m_hDbc )
    {
        SQLDisconnect( m_hDbc );
        ::SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
        m_hDbc = SQL_NULL_HDBC;
    } // if

    if( SQL_NULL_HENV != m_hEnv )
    {
        ::SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
        m_hEnv = SQL_NULL_HENV;
    } // if

    m_usColumNum    = 0;
} // Close()

bool KODBC::Query( const std::wstring& wstrQuery )
{
	if( wstrQuery.length() > QUERY_BUFFER_SIZE )
	{
		START_LOG( cerr, L"쿼리 스트링 길이가 너무 길다!" )
			<< BUILD_LOG( wstrQuery.length() )
			<< BUILD_LOG( wstrQuery )
			<< END_LOG;

		// no return: a long query is only suspicious, it still has to run
	}

	m_strLastQuery = wstrQuery;
	SqlWString sqlQuery = ToSqlWString( m_strLastQuery );

	DWORD dwElapTime = ::GetTickCount();  // remembers the slowest query

	// a query without rows returns SQL_NO_DATA
	SQLRETURN ret = ::SQLExecDirectW( m_hStmt, SqlWPtr( sqlQuery ), SQL_NTS );
	ReadDiag( SQL_HANDLE_STMT, m_hStmt );

	// on a lost connection or a bad cursor state, reconnect and run the query again
	if ( ret == SQL_ERROR || ret == SQL_INVALID_HANDLE )
	{
		const wchar_t* err1 = L"08S01";  // Communication link failure
		const wchar_t* err2 = L"24000";  // Invalid cursor state

		if( wcscmp( m_sqlState, err1 ) == 0 || wcscmp( m_sqlState, err2 ) == 0 )
		{
			dbg::cerr << m_strLastQuery << L"(err:" << m_sqlState << L")" << END_LOG;

			Close();

			bool bRet = false;
			if( m_bDBConnStr )
			{
				bRet = DriverConnectString( KClearString< std::wstring >() );
			}
			else
			{
				bRet = DriverConnectFileDSN( m_strDSN.c_str() );
			}

			if( !bRet )
			{
				dbg::cerr << L"재접속 시도 실패" << END_LOG;
				return false;
			}
			else
			{
				ret = ::SQLExecDirectW( m_hStmt, SqlWPtr( sqlQuery ), SQL_NTS );
			}
		}
	}

	dwElapTime = ::GetTickCount() - dwElapTime;

	{   // scope of the lock
		KLocker lock( ms_kMaxProcessingInfo.m_cs );

		if( dwElapTime > ms_kMaxProcessingInfo.m_dwTerm )
		{
			ms_kMaxProcessingInfo.m_strQuery    = m_strLastQuery;
			ms_kMaxProcessingInfo.m_cTime       = CTime::GetCurrentTime();
			ms_kMaxProcessingInfo.m_dwTerm      = dwElapTime;

			START_LOG( clog2, L"쿼리 최대 소요시간 갱신." )
				<< BUILD_LOG( ms_kMaxProcessingInfo.m_strQuery )
				<< BUILD_LOG( ms_kMaxProcessingInfo.m_dwTerm );
		}
	}

	if( SQLOK(ret) == false )
	{
		const SQLRETURN ret2 = ReadDiag( SQL_HANDLE_STMT, m_hStmt );

		START_LOG( cerr, L"Query 실패!!")
			<< BUILD_LOG( ret )
			<< BUILD_LOG( ret2 )
			<< BUILD_LOG( m_sqlState )
			<< BUILD_LOG( m_sqlNativeErr )
			<< BUILD_LOG( m_szDiagMsg )
			<< BUILD_LOG( m_sqlMsgLen )
			<< END_LOG;
	}

	return SQLOK(ret);
}

bool KODBC::BeginFetch()
{
    SQLRETURN   ret;
    m_usColumNum = 0;
    ret = ::SQLFetch( m_hStmt );

    if( !SQLOK(ret) )
    {
        ReadDiag( SQL_HANDLE_STMT, m_hStmt );
        EndFetch();
        return false;
    }

    return true;
}


bool KODBC::Fetch()
{
    m_usColumNum = 0;
    SQLRETURN ret = ::SQLFetch( m_hStmt );

    if( ret == SQL_NO_DATA )
        EndFetch();

    return SQLOK(ret);
} // Fetch()


void KODBC::EndFetch()
{
    if( SQL_NULL_HSTMT != m_hStmt )
        ::SQLCloseCursor( m_hStmt );
}

// Reads the whole column (a NULL clears the string and returns false). The original read at most
// 1024 bytes, truncating longer values.
bool KODBC::GetWideData( std::wstring& wstrVal )
{
    m_usColumNum++;
    wstrVal.clear();

    SqlWString buff;
    SQLWCHAR chunk[512];
    for( ;; )
    {
        SQLLEN LenBin = 0;
        const SQLRETURN ret = ::SQLGetData( m_hStmt, m_usColumNum, SQL_C_WCHAR, chunk, sizeof( chunk ), &LenBin );

        if( ret == SQL_NO_DATA )
            break;

        if( !SQLOK( ret ) )
        {
            THROW_INVALID_QUERY();
        }

        if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
            return false;

        // a truncated part fills the buffer, less the terminator
        const size_t n = ( LenBin != SQL_NO_TOTAL && LenBin < static_cast<SQLLEN>( sizeof( chunk ) ) )
                       ? static_cast<size_t>( LenBin ) / sizeof( SQLWCHAR )
                       : std::size( chunk ) - 1;
        buff.append( reinterpret_cast<const SqlWString::value_type*>( chunk ), n );

        if( ret == SQL_SUCCESS )
            break;
    }

    wstrVal = FromSqlWString( buff );
    return true;
}

// Same as GetWideData, in the client character set of the driver (SQL_C_CHAR).
bool KODBC::GetNarrowData( std::string& strVal )
{
    m_usColumNum++;
    strVal.clear();

    char chunk[1024];
    for( ;; )
    {
        SQLLEN LenBin = 0;
        const SQLRETURN ret = ::SQLGetData( m_hStmt, m_usColumNum, SQL_C_CHAR, chunk, sizeof( chunk ), &LenBin );

        if( ret == SQL_NO_DATA )
            break;

        if( !SQLOK( ret ) )
        {
            THROW_INVALID_QUERY();
        }

        if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
            return false;

        const size_t n = ( LenBin != SQL_NO_TOTAL && LenBin < static_cast<SQLLEN>( sizeof( chunk ) ) )
                       ? static_cast<size_t>( LenBin )
                       : sizeof( chunk ) - 1;
        strVal.append( chunk, n );

        if( ret == SQL_SUCCESS )
            break;
    }

    return true;
}

KODBC& KODBC::operator >>(bool& val_)
{
    char        val = 0;
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_STINYINT,
                        &val,
                        sizeof(bool),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    val_ = !(val == 0);
    return *this;
} // operator >>()


KODBC& KODBC::operator >>(char& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_STINYINT,
                       &val_,
                        sizeof(char),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()

// The original asked for a single wchar_t, room only for the terminator: it always read L'\0'.
KODBC& KODBC::operator >>(wchar_t& val_)
{
    std::wstring wstrVal;
    if( GetWideData( wstrVal ) )
        val_ = wstrVal.empty() ? L'\0' : wstrVal[0];

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(short& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_SHORT,
                        &val_,
                        sizeof(short),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(int& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_SLONG,
                        &val_,
                        sizeof(int),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(UINT& val_)
{
	SQLRETURN   ret;
	SQLLEN  LenBin = 0;

	m_usColumNum++;
	ret = ::SQLGetData( m_hStmt,
		m_usColumNum,
		SQL_C_SLONG,
		&val_,
		sizeof(UINT),
		&LenBin);

	if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
		return *this;

	if( !SQLOK(ret) )
	{
		THROW_INVALID_QUERY();
	} // if

	return *this;
} // operator >>()

KODBC& KODBC::operator >>(UCHAR& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_UTINYINT,
                        &val_,
                        sizeof(UCHAR),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()


KODBC& KODBC::operator >>(USHORT& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_SSHORT,
                        &val_,
                        sizeof(USHORT),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()


#if defined(_WIN32)
KODBC& KODBC::operator >>(DWORD& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_SLONG,
                        &val_,
                        sizeof(DWORD),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()
#endif

KODBC& KODBC::operator >>(ULONGLONG& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
        m_usColumNum,
        SQL_C_SBIGINT,
        &val_,
        sizeof(ULONGLONG),
        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
}

KODBC& KODBC::operator >>(LONGLONG& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
        m_usColumNum,
        SQL_C_SBIGINT,
        &val_,
        sizeof(LONGLONG),
        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
}


KODBC& KODBC::operator >>(std::string& val_)
{
    GetNarrowData( val_ );  // NULL reads as an empty string, like the original

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(char* val_)
{
    SQLRETURN   ret;
    SQLLEN      LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_CHAR,
                        val_,
                        1024,
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()


KODBC& KODBC::operator >>(std::wstring& val_)
{
    GetWideData( val_ );    // NULL reads as an empty string, like the original

    return *this;
} // operator >>()


// val_ holds 512 characters, terminator included (the original's 1024 bytes of a Windows wchar_t)
KODBC& KODBC::operator >>(wchar_t* val_)
{
    std::wstring wstrVal;
    if( GetWideData( wstrVal ) )    // a NULL leaves val_ untouched
    {
        const size_t n = std::min<size_t>( wstrVal.size(), 511 );
        std::copy_n( wstrVal.begin(), n, val_ );
        val_[n] = L'\0';
    }

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(float& val_)
{
    SQLRETURN   ret;
    SQLLEN  LenBin = 0;

    m_usColumNum++;
    ret = ::SQLGetData( m_hStmt,
                        m_usColumNum,
                        SQL_C_FLOAT,
                        &val_,
                        sizeof(float),
                        &LenBin);

    if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
        return *this;

    if( !SQLOK(ret) )
    {
        THROW_INVALID_QUERY();
    } // if

    return *this;
} // operator >>()

KODBC& KODBC::operator >>(KSerBuffer& val_)
{
    SQLRETURN   ret;
    char        cbuff[1024];

    m_usColumNum++;
    val_.Clear();

    for( ;; )
    {
        SQLLEN LenBin = 0;
        ret = ::SQLGetData( m_hStmt,
                            m_usColumNum,
                            SQL_C_BINARY,
                            cbuff,
                            sizeof(cbuff),
                            &LenBin);

        if( ret == SQL_NO_DATA )
            break;

        if( !SQLOK(ret) )
        {
            THROW_INVALID_QUERY();
        } // if

        if( LenBin == SQL_NULL_DATA )   // SQLGetData returns SQL_SUCCESS for a NULL field
            return *this;

        // LenBin is what was left before this call: a truncated part fills the buffer
        const SQLLEN lenPart = ( LenBin == SQL_NO_TOTAL || LenBin > static_cast<SQLLEN>( sizeof(cbuff) ) )
                             ? static_cast<SQLLEN>( sizeof(cbuff) ) : LenBin;
        val_.Write( cbuff, static_cast<int>( lenPart ) );

        if( ret == SQL_SUCCESS )
            break;
    }

    return *this;
}



// 041115
ImplToStringW( KODBC )
{
   // a file DSN has a single section, [ODBC]
   wchar_t szAddress[100] = {0};
   GetPrivateProfileStringW( L"ODBC", L"Address", L"", szAddress, 100, m_strAbsolutePathDSN.c_str() );

   return stm_ << std::endl
               << TOSTRINGW( szAddress )
               << TOSTRINGW( m_strDSN );
}

void KODBC::DumpMaxTime( std::wostream& stm_ )
{
    KLocker lock( ms_kMaxProcessingInfo.m_cs );

    stm_ << L" -- Max Processing query -- " << std::endl
         << L"  query   : " << ms_kMaxProcessingInfo.m_strQuery.c_str() << std::endl
         << L"  elapsed : " << ms_kMaxProcessingInfo.m_dwTerm << std::endl
         << L"  execute : " << (const wchar_t*)ms_kMaxProcessingInfo.m_cTime.Format(_T("%Y-%m-%d %H:%M:%S")) << std::endl;
}

void KODBC::ResetMaxTime()
{
    KLocker lock( ms_kMaxProcessingInfo.m_cs );

    ms_kMaxProcessingInfo.m_dwTerm = 0;
    ms_kMaxProcessingInfo.m_cTime = CTime::GetCurrentTime();
    ms_kMaxProcessingInfo.m_strQuery = (const wchar_t*)ms_kMaxProcessingInfo.m_cTime.Format( L"%Y-%m-%d %H:%M:%S" );
    ms_kMaxProcessingInfo.m_strQuery += L" 에 데이터 초기화 되었습니다.";
}

namespace { // 외부에서 참조당하지 못하도록 anonymous namespace 안에 선언.

    static const wchar_t* g_szInvalidMark[] = {
        L"\'",  // 홑따옴표
        L"\"",  // 쌍따옴표
        L";",   // 세미콜론
        L"--",  // 주석기호(mssql)
        L" ",   // 일반공백
        L"　",  // 넓은공백 - 'ㄱ' 넣고 한자 변환 1번.
        L"\t",  // tab 문자
        L",",   // 쉼표
		//SERV_CHANNELING_AERIA
		//L"~",   // 물결
		L"`",   // 그레이브
		//{{ 2011. 07. 27    김민성    투니랜드 채널링
//#ifdef SERV_TOONILAND_CHANNELING // SERV_JAPAN_CHANNELING //SERV_INT_ONLY
		//L"@",   // 골뱅이								// 주석 처리
//#endif SERV_TOONILAND_CHANNELING // SERV_JAPAN_CHANNELING
		//}}
		L"^",	// 서컴플렉트
		L"&",	// 앰퍼센트
		L"*",	// 곱셈부호
		//L"-",	// 뺄셈부호	//SERV_EMAIL_ID //SERV_INT_ONLY
		L"+",	// 덧셈부호
		L"=",	// 이퀄
		L"|",	// 바
		L"{",	// 중괄호(좌)
		L"}",	// 중괄호(우)
		//L"[",	// 대괄호(좌) //SERV_INT_ONLY
		//L"]",	// 대괄호(우) //SERV_INT_ONLY
		L":",	// 콜론
		L"?",	// 물음표
		L"<",	// 세모괄호(좌)
		L">",	// 세모괄호(우)
//#ifdef SERV_EMAIL_ID //SERV_INT_ONLY
		//L".",	// 닷
//#endif SERV_EMAIL_ID
		L"♪"	// 음표
    };
}

bool KODBC::IsInvalidMarkIn( IN const std::wstring& strData )
{
    for( size_t i = 0; i < NUM_ELEMENTS( g_szInvalidMark ); i++ )
    {
        if( strData.find( g_szInvalidMark[i] ) != std::string::npos )   // 동일한 문자가 발견된다면
            return true;
    }

    return false;   // 아무것도 발견되지 않았다면 false.
}

namespace { // 외부에서 참조당하지 못하도록 anonymous namespace 안에 선언.

	static const wchar_t* g_szInvalidMarkForLetter[] = {
		L"\'",  // 홑따옴표
	};
}

bool KODBC::IsInvalidMarkInForLetter( IN const std::wstring& strData )
{
	for( size_t i = 0; i < NUM_ELEMENTS( g_szInvalidMarkForLetter ); i++ )
	{
		if( strData.find( g_szInvalidMarkForLetter[i] ) != std::string::npos )   // 동일한 문자가 발견된다면
			return true;
	}

	return false;   // 아무것도 발견되지 않았다면 false.
}

namespace { // 외부에서 참조당하지 못하도록 anonymous namespace 안에 선언.

	static const wchar_t* g_szInvalidMarkForCheat[] = {
		L";",   // 세미콜론
		L"--",  // 주석기호(mssql)
		L"　",  // 넓은공백 - 'ㄱ' 넣고 한자 변환 1번.
		L"\t",  // tab 문자
		L",",   // 쉼표
		L"~",   // 물결
		L"`",   // 그레이브
		L"@",   // 골뱅이
		L"^",	// 서컴플렉트
		L"&",	// 앰퍼센트
		L"*",	// 곱셈부호
		L"-",	// 뺄셈부호
		L"+",	// 덧셈부호
		L"=",	// 이퀄
		L"|",	// 바
		L"{",	// 중괄호(좌)
		L"}",	// 중괄호(우)
		L"[",	// 대괄호(좌)
		L"]",	// 대괄호(우)
		L":",	// 콜론
		L"?",	// 물음표
		L"<",	// 세모괄호(좌)
		L">",	// 세모괄호(우)
		L".",	// 닷
		L"♪"	// 음표
	};
}

bool KODBC::IsInvalidMarkInForCheat( IN const std::wstring& strData )
{
	{
		for( size_t i = 0; i < NUM_ELEMENTS( g_szInvalidMarkForCheat ); i++ )
		{
			if( strData.find( g_szInvalidMarkForCheat[i] ) != std::string::npos )   // 동일한 문자가 발견된다면
				return true;
		}

		return false;   // 아무것도 발견되지 않았다면 false.
	}
}

//{{ 2011. 08. 09	최육사	머신ID 블럭기능
//#ifdef SERV_MACHINE_ID_BLOCK

namespace { // 외부에서 참조당하지 못하도록 anonymous namespace 안에 선언.

	static const wchar_t* g_szInvalidMarkForMachineID[] = {
		L"\'",  // 홑따옴표
		L"\"",  // 쌍따옴표
		L";",   // 세미콜론
		L"--",  // 주석기호(mssql)
		L" ",   // 일반공백
		L"　",  // 넓은공백 - 'ㄱ' 넣고 한자 변환 1번.
		L"\t",  // tab 문자
		L",",   // 쉼표
		L"~",   // 물결
		L"`",   // 그레이브
		L"@",   // 골뱅이
		L"^",	// 서컴플렉트
		L"&",	// 앰퍼센트
		L"*",	// 곱셈부호
		L"-",	// 뺄셈부호
		//L"+",	// 덧셈부호
		//L"=",	// 이퀄
		L"|",	// 바
		L"{",	// 중괄호(좌)
		L"}",	// 중괄호(우)
		L"[",	// 대괄호(좌)
		L"]",	// 대괄호(우)
		L":",	// 콜론
		L"?",	// 물음표
		L"<",	// 세모괄호(좌)
		L">",	// 세모괄호(우)
		L".",	// 닷
		L"♪"	// 음표
	};
}

bool KODBC::IsInvalidMarkInForMachineID( IN const std::wstring& strData )
{
	{
		for( size_t i = 0; i < NUM_ELEMENTS( g_szInvalidMarkForMachineID ); ++i )
		{
			if( strData.find( g_szInvalidMarkForMachineID[i] ) != std::string::npos )   // 동일한 문자가 발견된다면
				return true;
		}

		return false;   // 아무것도 발견되지 않았다면 false.
	}
}
//#endif SERV_MACHINE_ID_BLOCK
//}}

//#include <boost/algorithm/string/erase.hpp>

// Changes the data that goes to the DB: avoid it. Each mark becomes '*'.
// The original searched again from the start after each replacement and, '*' being a mark itself, never ended.
void KODBC::RemoveInvalidMark( IN OUT std::wstring& strData )
{
    for( size_t i = 0; i < NUM_ELEMENTS( g_szInvalidMark ); i++ )
    {
        const size_t len = wcslen( g_szInvalidMark[i] );
        for( size_t pos = 0; ( pos = strData.find( g_szInvalidMark[i], pos ) ) != std::wstring::npos; ++pos )
            strData.replace( pos, len, L"*" );
    }
}



//{{ 2012. 04. 17	최육사	DB쿼리 성공 체크
//#ifdef SERV_ALL_DB_QUERY_SUCCESS_CHECK
KDBQuerySuccessChecker::KDBQuerySuccessChecker()
{
	Clear();
}

KDBQuerySuccessChecker::~KDBQuerySuccessChecker()
{
}

void KDBQuerySuccessChecker::Clear()
{
	m_vecSuccessQuery.clear();
	m_vecFailedQuery.clear();
}

void KDBQuerySuccessChecker::AddSuccessQuery( IN const std::wstring& wstrQuery )
{
	m_vecSuccessQuery.push_back( wstrQuery );
}

void KDBQuerySuccessChecker::AddFailedQuery( IN const std::wstring& wstrQuery )
{
	m_vecFailedQuery.push_back( wstrQuery );
}

bool KDBQuerySuccessChecker::IsAllQuerySuccess() const
{
	return ( m_vecSuccessQuery.empty() == false  &&  m_vecFailedQuery.empty() == true );
}

void KDBQuerySuccessChecker::PrintLogSuccessQuery() const
{
	std::vector< std::wstring >::const_iterator vit;
	for( vit = m_vecSuccessQuery.begin(); vit != m_vecSuccessQuery.end(); ++vit )
	{
		START_LOG( cout2, L"Success Query : " << *vit );
	}
}

void KDBQuerySuccessChecker::PrintLogFailedQuery() const
{
	std::vector< std::wstring >::const_iterator vit;
	for( vit = m_vecFailedQuery.begin(); vit != m_vecFailedQuery.end(); ++vit )
	{
		START_LOG( cout2, L"Failed Query : " << *vit );
	}
}
//#endif SERV_ALL_DB_QUERY_SUCCESS_CHECK
//}}
