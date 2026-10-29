#include "dbg/dbg.hpp"

#include <doctest/doctest.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string ReadLog()
{
    std::ifstream in( dbg::logfile::GetLogFileName(), std::ios::binary );
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool LogStatFor( const wchar_t* func, int& count )
{
    std::map< unsigned int, dbg::logfile::KLogStat > stats;
    dbg::cerr.GetLogStat( stats );
    for( const auto& [key, stat] : stats )
        if( stat.m_wstrFunctionName.find( func ) != std::wstring::npos )
        {
            count = stat.m_iCount;
            return true;
        }
    return false;
}

void LogSomething( int value )
{
    START_LOG( cerr, L"value " << value << L" 한글" )
        << BUILD_LOG( value )
        << END_LOG;
}

} // namespace

TEST_CASE("START_LOG writes UTF-8 lines with function, level and data")
{
    dbg::logfile::SetLogLevel( 2 );
    LogSomething( 42 );
    const std::string log = ReadLog();
    CHECK(log.find( "cerr : " ) != std::string::npos);
    CHECK(log.find( "LogSomething(), " ) != std::string::npos);
    CHECK(log.find( "value 42 \xED\x95\x9C\xEA\xB8\x80" ) != std::string::npos);
    CHECK(log.find( "    value : 42" ) != std::string::npos);

    int count = 0;
    REQUIRE(LogStatFor( L"LogSomething", count ));
    CHECK(count >= 1);
}

TEST_CASE("log level filters clog")
{
    dbg::logfile::SetLogLevel( 1 );
    START_LOG( clog, L"hidden-marker" );
    dbg::logfile::SetLogLevel( 2 );
    START_LOG( clog, L"visible-marker" );
    const std::string log = ReadLog();
    CHECK(log.find( "hidden-marker" ) == std::string::npos);
    CHECK(log.find( "visible-marker" ) != std::string::npos);
}

TEST_CASE("lines from several threads are not interleaved")
{
    std::vector<std::thread> threads;
    for( int t = 0; t < 4; ++t )
        threads.emplace_back( [t] {
            for( int i = 0; i < 50; ++i )
                dbg::cout << L"thread-" << t << L"-line-" << i << L"-end" << dbg::endl;
        } );
    for( auto& th : threads )
        th.join();

    std::istringstream lines( ReadLog() );
    std::string line;
    int seen = 0;
    while( std::getline( lines, line ) )
    {
        if( line.rfind( "thread-", 0 ) != 0 )
            continue;
        ++seen;
        CHECK(line.size() >= 4);
        CHECK(line.substr( line.size() - 4 ) == "-end");
        CHECK(line.find( "thread-", 1 ) == std::string::npos);
    }
    CHECK(seen == 200);
}

TEST_CASE("JIF / LIF / cassert")
{
    bool reached = false;
    auto f = [&]( bool ok ) -> bool {
        _JIF( ok, return false );
        reached = true;
        return true;
    };
    CHECK_FALSE(f( false ));
    CHECK_FALSE(reached);
    CHECK(f( true ));
    LIF( 1 == 2 );
    cassert( 1 + 1 == 3 )( 1 + 1 ).msg( L"assert message" );
    CHECK(ReadLog().find( "Assert failure" ) != std::string::npos);
}
