#include "ServerMain.h"

#include "BaseServer.h"

#include "x2/platform/console.h"

#include <ServerConsole.h>

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

extern KBaseServer* CreateKNC();

int KServerMain( int argc, char* argv[] )
{
	x2::platform::InitConsole();

	// Port: the console options (T15/T30) are not arguments of the server
	const std::string strName = argc > 0 ? std::filesystem::path( argv[0] ).stem().string() : std::string();
	KServerConsole::Options kConsoleOptions;
	kConsoleOptions.name = strName;
	kConsoleOptions.control = strName;
	std::vector< KClearString< std::wstring > > vecCmdArgs;
	for( int i = 0; i < argc; ++i )
	{
		const std::string strArg = argv[i];
		if( strArg == "--no-stdin" )
		{
			kConsoleOptions.noStdin = true;		// service mode
			continue;
		}
		if( strArg == "--machine" )
		{
			kConsoleOptions.machine = true;		// stdin/stdout speak the control protocol
			continue;
		}
		if( strArg == "--no-control" )
		{
			kConsoleOptions.control.clear();
			continue;
		}
		if( strArg.rfind( "--control=", 0 ) == 0 )
		{
			kConsoleOptions.control = strArg.substr( 10 );
			continue;
		}
		if( strArg == "-popup" )
		{
			std::fputs( "-popup is not supported: pass the DB id and password as arguments\n", stderr );
			return 1;
		}
		vecCmdArgs.push_back( KncUtil::toWideString( strArg ) );
	}

	// Same sequence as CServerUIDlg::StartupServer/ShutdownServer. The console lives from before the loading to the
	// end of the shutdown, like the original window, whose title was the server name (GameServer.rc: "GameServer").
	KServerConsole kConsole( kConsoleOptions );
	KBaseServer* pkServer = CreateKNC();
	pkServer->SetCmdArgs( vecCmdArgs );
	vecCmdArgs.clear();
	pkServer->SetConsole( &kConsole );

	const bool bInit = pkServer->Init();
	if( bInit )
		pkServer->Run( true );

	pkServer->ShutDown();
	KBaseServer::ReleaseKObj();
	return bInit ? 0 : 1;
}
