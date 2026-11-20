#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "dbg/logfile.hpp"

int main( int argc, char** argv )
{
	// The log opens log.log in the working folder on its first line and keeps it open. Several fixtures switch to a
	// temporary folder they delete afterwards; on Windows an open file keeps the folder from being deleted, so the
	// log is opened here, before any test moves.
	dbg::cout << dbg::lbreak << L"server_common tests" << dbg::lbreak;

	doctest::Context context( argc, argv );
	return context.run();
}
