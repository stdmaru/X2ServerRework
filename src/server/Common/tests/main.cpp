#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <dbg/dbg.hpp>

int main( int argc, char** argv )
{
	// The log file opens in the current folder when the first line is written. Several tests run from a temporary
	// folder and delete it afterwards: a log line there would leave log.log open inside it, which Windows cannot
	// delete. Open it here, in the folder the tests start in.
	START_LOG( clog, L"server_common tests" ) << END_LOG;
	return doctest::Context( argc, argv ).run();
}
