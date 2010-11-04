//----------------------------------------------------------------------------------------------------------------------
// TimerWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if HELIUM_OS_WIN

#include "Core/Timer.h"

namespace Lunar
{
    uint64_t Timer::sm_ticksPerSecond = 0;
    uint64_t Timer::sm_startTickCount = 0;
    float64_t Timer::sm_secondsPerTick = 0.0;

    /// Perform static initialization necessary to enable timing support.
    void Timer::StaticInitialize()
    {
        LARGE_INTEGER perfQuery;
        L_VERIFY( QueryPerformanceFrequency( &perfQuery ) );
        L_ASSERT( perfQuery.QuadPart != 0 );

        sm_ticksPerSecond = perfQuery.QuadPart;
        sm_secondsPerTick = 1.0 / static_cast< float64_t >( perfQuery.QuadPart );

        L_VERIFY( QueryPerformanceCounter( &perfQuery ) );
        sm_startTickCount = perfQuery.QuadPart;
    }
}

#endif  // HELIUM_OS_WIN
