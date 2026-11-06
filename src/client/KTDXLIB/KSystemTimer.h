/** @file   KSystemTimer.h
    @desc   interface of KSystemTimer
    @author robobeg@kogstudios.com
    @since  2008-02-12

    @date   jintaeks on 2008-03-04, 9:27
            - OnetimeFirstCall() 수정.
*/

#pragma once

#include "x2/platform/win32.h"


#define KSYSTEMTIMER_TARGET_RESOLUTION      1


///-----------------------------------------------------------------------------
/// @class  KSystemTimer
/// @brief
///
class   KSystemTimer
{
public:
    static bool     ms_bInitialized;
    static UINT     ms_uiTimerRes;

    // OnetimeFirstCall/OnetimeLastCall (timeBeginPeriod/timeEndPeriod) had no callers and were removed.

    static DWORD    GetSystemTimeInMilisecond()
    {
		return ::timeGetTime();
    }//GetSystemTimeInMilisecond()
};//class   KSystemTimer
