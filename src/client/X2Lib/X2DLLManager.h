#pragma once

// The client's own anti-cheat: unloads the modules the server names (ANTIHACKING_FILGER_22).
//
// The original also hooked LoadLibrary*/LdrLoadDll with 32-bit x86 jumps to record DLL loads and injections, and
// matched the server's names against those records (ANTIHACKING_FILGER_09, 10, 19). Its CallerVerificationStart
// returned false before installing the hooks, so nothing was ever recorded and those checks never fired: the port
// drops them (docs/porting/client-x2lib.md).

class CX2DLLManager
{
	DeclareSingleton( CX2DLLManager );

public:
	CX2DLLManager() = default;
	virtual ~CX2DLLManager() = default;

	/// Unloads every loaded module whose file name, in upper case, is wstrFreeModuleName.
	void ForceFreeModule( const wstring& wstrFreeModuleName );
};

DefSingletonInline( CX2DLLManager );
