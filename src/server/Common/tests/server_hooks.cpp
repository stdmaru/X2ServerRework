// What each server defines for the Common (globals and factories of the server's main module), so the tests
// link without a server. Nothing here runs in the tests.
#include <Thread/Thread.h>

const wchar_t* g_szConfigFile = L"";
KThread* CreateDBThread( int, const wchar_t*, bool ) { return nullptr; }
