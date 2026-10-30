#pragma once

#include "SecurityAssociation.h"
#include "SecureBuffer.h"
#include "SADatabase.h"

namespace KncSecurity
{
    void InitSecurity();
    void ReleaseSecurity();

    KSADatabase& GetSADB();
}
