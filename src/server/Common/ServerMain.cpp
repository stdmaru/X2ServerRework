#include "ServerMain.h"

#include "BaseServer.h"

#include "x2/platform/console.h"

#include <cstdio>
#include <string>
#include <vector>

extern KBaseServer* CreateKNC();

int KServerMain( int argc, char* argv[] )
{
	x2::platform::InitConsole();

	std::vector< KClearString< std::wstring > > vecCmdArgs;
	for( int i = 0; i < argc; ++i )
	{
		const std::string strArg = argv[i];
		if( strArg == "-popup" )
		{
			std::fputs( "-popup is not supported: pass the DB id and password as arguments\n", stderr );
			return 1;
		}
		vecCmdArgs.push_back( KncUtil::toWideString( strArg ) );
	}

	// Same sequence as CServerUIDlg::StartupServer/ShutdownServer; the Lua prompt of the window input field
	// is the interpreter of KBaseServer::Run, which returns at the end of stdin.
	KBaseServer* pkServer = CreateKNC();
	pkServer->SetCmdArgs( vecCmdArgs );
	vecCmdArgs.clear();

	const bool bInit = pkServer->Init();
	if( bInit )
		pkServer->Run( true );

	pkServer->ShutDown();
	KBaseServer::ReleaseKObj();
	return bInit ? 0 : 1;
}
