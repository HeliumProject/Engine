#pragma once

#include "Foundation/API.h"
#include "Platform/Types.h"

namespace Helium
{
    //
    // Bare Timer
    //

    class FOUNDATION_API Timer
    {
    private:
        u64 m_StartTime;

    public:
        Timer()
        {
            Reset();
        }

    private:
        Timer(const Timer& rhs)
        {

        }

    public:
        // reset timer (for re-use)
        void Reset();

        // get elapsed time in millis
        float Elapsed();
    };
}