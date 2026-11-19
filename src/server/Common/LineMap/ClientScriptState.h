#pragma once

// A scratch Lua state that runs the client's own scripts (ClientScript), found by name through KAutoPath, for the
// servers to read the data the client builds from them (decision T53).

#include <string>

struct lua_State;

class KClientScriptState
{
public:
	KClientScriptState();	// a state with the standard libraries
	~KClientScriptState();
	KClientScriptState( const KClientScriptState& ) = delete;
	KClientScriptState& operator=( const KClientScriptState& ) = delete;

	lua_State* Get() const { return m_pState; }

	// The full path of a script found by name, or empty.
	static std::string Find( const std::string& strName );

	// Runs a script found by name. bRequired: a missing script is logged as an error, otherwise as a warning; a script
	// that fails is always an error.
	bool Run( const std::string& strName, bool bRequired );
	bool RunChunk( const char* szChunk );

	// A global string, or empty when it is not one.
	std::string GetGlobalString( const char* szName );

private:
	lua_State* m_pState;
};
