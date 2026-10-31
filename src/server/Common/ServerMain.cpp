#include "ServerMain.h"

#include "BaseServer.h"

#include "x2/platform/console.h"
#include "x2/platform/terminal.h"

#include <KncLua.h>

#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

extern KBaseServer* CreateKNC();

int KServerMain( int argc, char* argv[] )
{
	x2::platform::InitConsole();

	std::vector< KClearString< std::wstring > > vecCmdArgs;
	bool bNoStdin = false;
	for( int i = 0; i < argc; ++i )
	{
		const std::string strArg = argv[i];
		if( strArg == "--no-stdin" )
		{
			bNoStdin = true;	// service mode (T15): not an argument of the server
			continue;
		}
		if( strArg == "-popup" )
		{
			std::fputs( "-popup is not supported: pass the DB id and password as arguments\n", stderr );
			return 1;
		}
		vecCmdArgs.push_back( KncUtil::toWideString( strArg ) );
	}

	// Same sequence as CServerUIDlg::StartupServer/ShutdownServer; the Lua prompt of the window input field
	// is the terminal of KBaseServer::Run (lua_interpreter), which returns when the server should shut down.
	KBaseServer* pkServer = CreateKNC();
	pkServer->SetCmdArgs( vecCmdArgs );
	vecCmdArgs.clear();
	// The terminal lives from before the loading to the end of the shutdown, like the original window.
	// Its title was the server name (GameServer.rc: "GameServer").
	const std::string strTitle = argc > 0 ? std::filesystem::path( argv[0] ).stem().string() : std::string();
	std::optional< x2::platform::Terminal > kTerminal;
	if( !bNoStdin )
	{
		kTerminal.emplace();
		AttachTerminal( *kTerminal, strTitle );
	}
	pkServer->SetTerminal( kTerminal ? &*kTerminal : nullptr, strTitle );

	const bool bInit = pkServer->Init();
	if( bInit )
		pkServer->Run( true );

	pkServer->ShutDown();
	KBaseServer::ReleaseKObj();
	if( kTerminal )
		DetachTerminal( *kTerminal );
	return bInit ? 0 : 1;
}
