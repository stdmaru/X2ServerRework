#include "KncSecurity.h"
#include "SADatabase.h"
#include "SecureBuffer.h"

#define SAFE_DELETE( x ) if( x != NULL ) { delete x; x = NULL; }

NAMESPACE_BEGIN( KncSecurity )

// Port: the original also kept a Crypto++ AutoSeededRandomPool (GetRandomGenerator), used only by code that
// was already commented out; it went away with Crypto++ (T27).
KSADatabase*            g_pkSADB( NULL );

void InitSecurity()
{
    if( !g_pkSADB ) g_pkSADB    = new KSADatabase;
}

void ReleaseSecurity()
{
    SAFE_DELETE( g_pkSADB );
}

KSADatabase& GetSADB()
{
    if( !g_pkSADB )
	{
		g_pkSADB    = new KSADatabase();
	}

    return *g_pkSADB;
}

NAMESPACE_END
