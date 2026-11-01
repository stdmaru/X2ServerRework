// KODBC. Without a database: the .dsn conversion and the character filters. Against a real SQL Server (suite "db"):
// needs the X2_TEST_ODBC environment variable with an ODBC connection string, without DATABASE, to the server of
// docker/docker-compose.yml with the _database backups restored (docs/tools/development-database.md); ctest sets it
// from the X2_TEST_ODBC cache variable.
#include "ODBC/Odbc.h"
#include "ODBC/StoredProcedure.h"
#include "x2/platform/unicode.h"

#include <doctest/doctest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

// writes a file DSN in UTF-16LE with BOM, like the original's
std::filesystem::path WriteDsn( const char* szName, const std::wstring& wstrText )
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / szName;
    std::ofstream file( path, std::ios::binary );
    file.write( "\xFF\xFE", 2 );
    for( char16_t c : x2::platform::WideToUtf16( wstrText ) )
    {
        const char bytes[2] = { static_cast<char>( c & 0xFF ), static_cast<char>( c >> 8 ) };
        file.write( bytes, 2 );
    }
    return path;
}

std::wstring ConnStr( const wchar_t* szDatabase = L"master" )
{
    const char* sz = std::getenv( "X2_TEST_ODBC" );
    if( sz == nullptr || *sz == '\0' )
        return std::wstring();

    std::wstring wstr = x2::platform::Utf8ToWide( sz );
    if( !wstr.empty() && wstr.back() != L';' )
        wstr += L';';
    return wstr + L"DATABASE=" + szDatabase;
}

#define REQUIRE_DB( wstrConn ) \
    if( ( wstrConn ).empty() ) { MESSAGE( "X2_TEST_ODBC is not set: skipped" ); return; }

} // namespace

TEST_CASE( "file DSN: the [ODBC] section in UTF-16, with the driver replaced and WSID dropped" )
{
    const std::filesystem::path path = WriteDsn( "x2_test_odbc_keys.dsn",
        L"[ODBC]\r\nDRIVER=SQL Server\r\nUID=sa\r\nDATABASE=Elsword-Account\r\nWSID=DESKTOP-O8VMLUC\r\n"
        L"APP=Microsoft Data Access Components\r\nSERVER=10.0.0.1,1433\r\n; comment\r\nPWD=p;w{d}\r\n"
        L"[Other]\r\nKEY=ignored\r\n" );

    KODBC::KDsnKeys vecKeys;
    const bool bRead = KODBC::ReadFileDSN( path.wstring(), vecKeys );
    std::filesystem::remove( path );
    REQUIRE( bRead );
    REQUIRE( vecKeys.size() == 7 );
    CHECK( vecKeys[2].first == L"DATABASE" );
    CHECK( vecKeys[2].second == L"Elsword-Account" );

    CHECK( KODBC::BuildConnStrFromDsn( vecKeys, L"ODBC Driver 18 for SQL Server", L"Encrypt=yes;TrustServerCertificate=yes" )
           == L"DRIVER={ODBC Driver 18 for SQL Server};UID=sa;DATABASE=Elsword-Account;"
              L"APP={Microsoft Data Access Components};SERVER=10.0.0.1,1433;PWD={p;w{d}}};"
              L"Encrypt=yes;TrustServerCertificate=yes" );
    CHECK( KODBC::BuildConnStrFromDsn( vecKeys, L"FreeTDS", L"" )
           == L"DRIVER=FreeTDS;UID=sa;DATABASE=Elsword-Account;APP={Microsoft Data Access Components};"
              L"SERVER=10.0.0.1,1433;PWD={p;w{d}}};" );

    CHECK_FALSE( KODBC::ReadFileDSN( L"/nonexistent/x2.dsn", vecKeys ) );
}

TEST_CASE( "character filters of the queries built as text" )
{
    CHECK( KODBC::IsInvalidMarkIn( L"name'--" ) );
    CHECK( KODBC::IsInvalidMarkIn( L"a　b" ) );          // ideographic space
    CHECK_FALSE( KODBC::IsInvalidMarkIn( L"엘소드Elsword" ) );
    CHECK( KODBC::IsInvalidMarkInForLetter( L"it's" ) );
    CHECK_FALSE( KODBC::IsInvalidMarkInForLetter( L"hello; world" ) );

    std::wstring wstr = L"a'b;c";
    KODBC::RemoveInvalidMark( wstr );
    CHECK( wstr == L"a*b*c" );
}

TEST_SUITE( "db" )
{

TEST_CASE( "connects with a connection string and reads every column type" )
{
    const std::wstring wstrConn = ConnStr();
    REQUIRE_DB( wstrConn );

    KODBC kODBC;
    REQUIRE( kODBC.DriverConnect( wstrConn.c_str(), true ) );
    CHECK( kODBC.m_strDBConnStr == L"master" );   // what the logs show: the password is not kept in clear

    // Korean, a character outside the BMP (a surrogate pair in UTF-16) and values longer than one read
    const std::wstring wstrText = L"엘소드 Elsword \U0001F600";
    REQUIRE( kODBC.Query( L"SELECT CAST(1 AS bit), CAST(-5 AS smallint), CAST(-7 AS smallint), 123456789, "
                          L"CAST(4000000000 AS bigint), CAST(-9000000000 AS bigint), CAST(200 AS tinyint), CAST(-1 AS smallint), "
                          L"CAST(1.5 AS real), N'" + wstrText + L"', 'ascii', REPLICATE(CAST(N'가' AS nvarchar(max)), 3000), "
                          L"CAST(REPLICATE(CAST(0xAB AS varbinary(max)), 2500) AS varbinary(max)), "
                          L"CAST(NULL AS int), CAST(NULL AS nvarchar(10)), N'X', N'wide'" ) );
    REQUIRE( kODBC.Fetch() );

    bool bVal = false;
    short sVal = 0;
    char cNeg = 0;
    int iVal = 0;
    LONGLONG llVal = 0;
    LONGLONG llNeg = 0;
    UCHAR ucVal = 0;
    USHORT usVal = 0;
    float fVal = 0.f;
    std::wstring wstrRead;
    std::string strRead;
    std::wstring wstrLong;
    KSerBuffer kBlob;
    int iNull = 42;
    std::wstring wstrNull = L"not cleared";
    wchar_t cVal = L'?';
    wchar_t szWide[512] = {};

    kODBC >> bVal >> sVal >> cNeg >> iVal >> llVal >> llNeg >> ucVal >> usVal >> fVal >> wstrRead >> strRead
          >> wstrLong >> kBlob >> iNull >> wstrNull >> cVal >> szWide;

    CHECK( bVal );
    CHECK( sVal == -5 );
    CHECK( cNeg == -7 );            // SQL_C_STINYINT
    CHECK( iVal == 123456789 );
    CHECK( llVal == 4000000000LL );
    CHECK( llNeg == -9000000000LL );
    CHECK( ucVal == 200 );
    CHECK( usVal == 65535 );          // SQL_C_SSHORT, like the original
    CHECK( fVal == 1.5f );
    CHECK( wstrRead == wstrText );
    CHECK( strRead == "ascii" );
    CHECK( wstrLong == std::wstring( 3000, L'가' ) );
    REQUIRE( kBlob.GetLength() == 2500 );
    CHECK( static_cast<const unsigned char*>( kBlob.GetData() )[2499] == 0xAB );
    CHECK( iNull == 42 );               // a NULL leaves numbers untouched
    CHECK( wstrNull.empty() );          // and reads as an empty string
    CHECK( cVal == L'X' );
    CHECK( std::wstring( szWide ) == L"wide" );

    CHECK_FALSE( kODBC.Fetch() );       // one row
}

TEST_CASE( "a failed query reports the SQL state and message" )
{
    const std::wstring wstrConn = ConnStr();
    REQUIRE_DB( wstrConn );

    KODBC kODBC;
    REQUIRE( kODBC.DriverConnect( wstrConn.c_str(), true ) );

    CHECK_FALSE( kODBC.Query( L"SELECT * FROM dbo.TableThatDoesNotExist" ) );
    CHECK( std::wstring( kODBC.m_sqlState ) == L"42S02" );
    CHECK( std::wstring( kODBC.m_szDiagMsg ).find( L"TableThatDoesNotExist" ) != std::wstring::npos );
    CHECK( kODBC.GetLastQuery() == L"SELECT * FROM dbo.TableThatDoesNotExist" );

    // the connection is still usable
    REQUIRE( kODBC.Query( L"SELECT 7" ) );
    int iVal = 0;
    REQUIRE( kODBC.BeginFetch() );
    kODBC >> iVal;
    kODBC.EndFetch();
    CHECK( iVal == 7 );
}

TEST_CASE( "a wrong password fails to connect" )
{
    std::wstring wstrConn = ConnStr();
    REQUIRE_DB( wstrConn );

    wstrConn += L";PWD=wrong-password";     // the last PWD wins
    KODBC kODBC;
    CHECK_FALSE( kODBC.DriverConnect( wstrConn.c_str(), true ) );
    CHECK( kODBC.m_sqlState[0] != L'\0' );
    CHECK_FALSE( kODBC.Query( L"SELECT 1" ) );
}

TEST_CASE( "connects with a UTF-16 file DSN of the original, replacing its driver" )
{
    const std::wstring wstrConn = ConnStr( L"Account" );
    REQUIRE_DB( wstrConn );

    // the original's keys, one per line, with the env's DRIVER moved to SetODBCDriver
    std::wstring wstrDsn = L"[ODBC]\r\nDRIVER=SQL Server\r\nWSID=DESKTOP-TEST\r\n";
    std::wstring wstrDriver;
    for( size_t pos = 0; pos < wstrConn.size(); )
    {
        size_t end = wstrConn.find( L';', pos );
        if( end == std::wstring::npos )
            end = wstrConn.size();
        const std::wstring wstrPair = wstrConn.substr( pos, end - pos );
        if( wstrPair.rfind( L"DRIVER=", 0 ) == 0 )
            wstrDriver = wstrPair.substr( 7 );
        else if( !wstrPair.empty() )
            wstrDsn += wstrPair + L"\r\n";
        pos = end + 1;
    }
    REQUIRE( !wstrDriver.empty() );
    const std::filesystem::path path = WriteDsn( "x2_test_odbc.dsn", wstrDsn );

    KODBC::SetODBCDriver( wstrDriver );
    KODBC kODBC;
    const bool bConnected = kODBC.DriverConnect( path.wstring().c_str(), false );
    KODBC::SetODBCDriver( L"ODBC Driver 18 for SQL Server" );
    KODBC::SetODBCOptions( L"Encrypt=yes;TrustServerCertificate=yes" );
    std::filesystem::remove( path );

    REQUIRE( bConnected );
    CHECK( kODBC.m_strDSN == L"FILEDSN=" + path.wstring() );

    REQUIRE( kODBC.Query( L"SELECT DB_NAME()" ) );
    std::wstring wstrDB;
    REQUIRE( kODBC.BeginFetch() );
    kODBC >> wstrDB;
    kODBC.EndFetch();
    CHECK( wstrDB == L"Account" );
}

TEST_CASE( "a procedure called with exec, like DO_QUERY, and a long std::string" )
{
    const std::wstring wstrConn = ConnStr( L"tempdb" );
    REQUIRE_DB( wstrConn );

    KODBC kODBC;
    REQUIRE( kODBC.DriverConnect( wstrConn.c_str(), true ) );

    // #temporary procedure: lives only in this connection. A statement without a result returns SQL_NO_DATA on
    // FreeTDS, which Query reports as success, like the SQL Server driver of production.
    CHECK( kODBC.Query( L"CREATE PROCEDURE #x2_test_sp @iUID bigint, @wstrName nvarchar(32) AS "
                          L"SELECT @iUID + 1, @wstrName + N'!', REPLICATE(CAST('a' AS varchar(max)), 5000)" ) );
    CHECK( kODBC.m_sqlState[0] == L'\0' );     // no error

    REQUIRE( kODBC.Query( L"exec #x2_test_sp 9000000000, N'엘소드'" ) );
    REQUIRE( kODBC.BeginFetch() );
    LONGLONG llUID = 0;
    std::wstring wstrName;
    std::string strLong;
    kODBC >> llUID >> wstrName >> strLong;
    kODBC.EndFetch();

    CHECK( llUID == 9000000001LL );
    CHECK( wstrName == L"엘소드!" );
    CHECK( strLong == std::string( 5000, 'a' ) );
}

TEST_CASE( "a procedure without a result, like the statistics inserts, succeeds" )
{
    const std::wstring wstrConn = ConnStr( L"tempdb" );
    REQUIRE_DB( wstrConn );

    KODBC kODBC;
    REQUIRE( kODBC.DriverConnect( wstrConn.c_str(), true ) );

    // the shape of dbo.P_StatCCUMinute_INS and dbo.P_Rank_SpaceTime_Day_DEL: SET NOCOUNT ON and no SELECT
    REQUIRE( kODBC.Query( L"CREATE TABLE #x2_test_t ( i int )" ) );
    REQUIRE( kODBC.Query( L"CREATE PROCEDURE #x2_test_ins @i int AS SET NOCOUNT ON INSERT INTO #x2_test_t VALUES ( @i )" ) );
    CHECK( kODBC.Query( L"exec #x2_test_ins 7" ) );
    CHECK( kODBC.m_sqlState[0] == L'\0' );
    CHECK_FALSE( kODBC.BeginFetch() );          // no rows to read

    REQUIRE( kODBC.Query( L"SELECT COUNT(*) FROM #x2_test_t" ) );
    int iCount = 0;
    REQUIRE( kODBC.BeginFetch() );
    kODBC >> iCount;
    kODBC.EndFetch();
    CHECK( iCount == 1 );
}

TEST_CASE( "KStoredProcedure reads the characters of a user from Game01" )
{
    const std::wstring wstrConn = ConnStr( L"Game01" );
    REQUIRE_DB( wstrConn );

    KODBC kODBC;
    REQUIRE( kODBC.DriverConnect( wstrConn.c_str(), true ) );

    // user 1 of the backup has characters (GUnit)
    REQUIRE( kODBC.Query( L"SELECT COUNT(*) FROM dbo.GUnit WHERE UserUID = 1 AND Deleted = 0" ) );
    int iUnits = 0;
    REQUIRE( kODBC.BeginFetch() );
    kODBC >> iUnits;
    kODBC.EndFetch();
    REQUIRE( iUnits > 0 );

    KStoredProcedure kSP( kODBC );
    std::vector<KUnitInfo> vecUnitInfo;
    kSP.GetUnitInfo( 1, vecUnitInfo );

    REQUIRE( !vecUnitInfo.empty() );
    for( const KUnitInfo& kUnit : vecUnitInfo )
    {
        CHECK( kUnit.m_nUnitUID > 0 );
        CHECK( !kUnit.m_wstrNickName.empty() );
        CHECK( kUnit.m_ucLevel > 0 );
    }

    // the other calls run against the same data without failing
    kSP.GetEquipItemList( vecUnitInfo );
    kSP.GetLastPosition( vecUnitInfo );
}

} // TEST_SUITE
