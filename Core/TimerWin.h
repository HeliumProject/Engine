//----------------------------------------------------------------------------------------------------------------------
// TimerWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_TIMER_WIN_H
#define LUNAR_CORE_TIMER_WIN_H

#include "Core/Core.h"

#if HELIUM_OS_WIN

#include "Platform/Windows/Windows.h"

namespace Lunar
{
    /// Application timer support.
    class LUNAR_CORE_API Timer
    {
    public:
        /// @name Static Timing Support
        //@{
        static void StaticInitialize();

        inline static uint64_t GetTickCount();
        inline static uint64_t GetStartTickCount();
        inline static uint64_t GetTicksPerSecond();

        inline static float64_t GetSecondsPerTick();
        inline static float64_t GetSeconds();
        //@}

    private:
        /// Performance counter frequency.
        static uint64_t sm_ticksPerSecond;
        /// Tick count on static initialization.
        static uint64_t sm_startTickCount;
        /// Seconds per performance counter tick.
        static float64_t sm_secondsPerTick;
    };
}

#include "Core/TimerWin.inl"

#endif  // HELIUM_OS_WIN

#endif  // LUNAR_CORE_TIMER_H
