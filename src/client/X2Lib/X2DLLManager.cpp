#include "stdafx.h"
#include "X2DLLManager.h"

#include "x2/platform/modules.h"

ImplementSingleton( CX2DLLManager );

void CX2DLLManager::ForceFreeModule( const wstring& wstrFreeModuleName )
{
	for ( const x2::platform::LoadedModule& module : x2::platform::ListLoadedModules() )
	{
		wstring wstrModuleName = x2::platform::ModuleFileName( module.path );
		MakeUpperCase( wstrModuleName );

		if( wstrModuleName != wstrFreeModuleName )
			continue;

		// Not a real error: logged in case unloading the module ends the client.
		ErrorLogMsg( KEM_ERROR423, wstrModuleName.c_str() );

		x2::platform::UnloadModule( module );
	}
}
