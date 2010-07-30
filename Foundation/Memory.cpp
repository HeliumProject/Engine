#include "Memory.h"
#include "Profile.h"
#include "Timer.h"
#include "Log.h"

#include "Platform/Platform.h"
#include "Platform/Thread.h"
#include "Platform/Assert.h"

#include <stdlib.h>

#ifdef WIN32
#define snprintf _snprintf
#define abs64 _abs64
#else
#define abs64 labs
#endif

using namespace Helium;
using namespace Helium::Profile; 

static const u32        g_MaxMemoryPools = 64;
static MemoryPool       g_MemoryPools[g_MaxMemoryPools];
static u32              g_MemoryPoolCount = 0;
static Platform::Thread g_MemoryReportThread;
static bool             g_MemoryReportThreadTerminate = false;
static bool             g_MemoryProfilingEnabled = false;

bool Profile::Settings::MemoryProfilingEnabled()
{
    return g_MemoryProfilingEnabled;
}

static const tchar* MemoryUnitConvert(f32& size)
{
    if (size > 1 << 10)
    {
        if (size > 1 << 20)
        {
            if (size > 1 << 30)
            {
                size /= (float)(1 << 30);
                return TXT( "g" );
            }
            else
            {
                size /= (float)(1 << 20);
                return TXT( "m" );
            }
        }
        else
        {
            size /= (float)(1 << 10);
            return TXT( "k" );
        }
    }
    else
    {
        return TXT( "b" );
    }
}

static Platform::Thread::Return MemoryReportThread(Platform::Thread::Param)
{
    u32 oldCount = g_MemoryPoolCount;
    f32 oldTotal = (f32)Platform::GetTotalMemory();

    while (!g_MemoryReportThreadTerminate)
    {
        f32 total = (f32)Platform::GetTotalMemory();

        // report if we have different memory usage or more pools
        bool report = oldTotal != total || oldCount != g_MemoryPoolCount;

        // check for changed data in each pool
        f32 profiled = 0;
        for (u32 i=0; i<g_MemoryPoolCount; i++)
        {
            profiled += g_MemoryPools[i].m_Size;
            report |= g_MemoryPools[i].m_Size != g_MemoryPools[i].m_Previous;
        }

        // if we have any new info, do the report
        if (report)
        {
            oldCount = g_MemoryPoolCount;
            oldTotal = total;

            float accountedFor = total > 0 ? ((float)profiled / (float)total * 100.f) : 0.f;
            const tchar* profiledUnits = MemoryUnitConvert(profiled);
            const tchar* totalUnits = MemoryUnitConvert(total);

            Log::Profile( TXT( "Memory - Profiled: %.2f%s / Committed: %.2f%s / Accounted for: %.2f%%\n" ), profiled, profiledUnits, total, totalUnits, accountedFor);

            for (u32 i=0; i<g_MemoryPoolCount; i++)
            {
                f32 size = (f32)g_MemoryPools[i].m_Size;
                const tchar* sizeUnits = MemoryUnitConvert( size );

                f32 delta = (f32)abs64(g_MemoryPools[i].m_Size - g_MemoryPools[i].m_Previous);
                const tchar* deltaUnits = MemoryUnitConvert( delta );

                tchar sign = ((i64)g_MemoryPools[i].m_Size - (i64)g_MemoryPools[i].m_Previous) >= 0 ? '+' : '-';

                Log::Profile( TXT( " %-30s: [%7d] %.2f%s (%c%.2f%s)\n" ), g_MemoryPools[i].m_Name, g_MemoryPools[i].m_Count, size, sizeUnits, sign, delta, deltaUnits );

                g_MemoryPools[i].m_Previous = g_MemoryPools[i].m_Size;
            }
        }

        Timer timer;

        while (timer.Elapsed() < 5000 && !g_MemoryReportThreadTerminate)
        {
            Platform::Sleep(10);
        }
    }

    return Platform::Thread::Return(0);
}

u32 Memory::s_InitCount = 0;

//static
bool Memory::Initialize()
{
    if (++s_InitCount == 1)
    {
        g_MemoryProfilingEnabled = true;
        g_MemoryReportThreadTerminate = false;

        if (!g_MemoryReportThread.Create( &MemoryReportThread, NULL, TXT( "Profile Memory Report Thread" ) ))
        {
            HELIUM_BREAK();
        }
    }

    return true;
}

//static
void Memory::Cleanup()
{
    if (--s_InitCount == 0)
    {
        g_MemoryReportThreadTerminate = true;

        g_MemoryReportThread.Wait();
        g_MemoryProfilingEnabled = false;
    }
}

//static
MemoryPoolHandle Memory::CreatePool(const tchar* name)
{
    MemoryPoolHandle pool;

    if (g_MemoryPoolCount < g_MaxMemoryPools)
    {
        pool.m_Index = g_MemoryPoolCount;
        g_MemoryPools[ g_MemoryPoolCount++ ].m_Name = name;
    }
    else
    {
        HELIUM_BREAK(); // out of pools, increase global pool maximum
    }

    return pool;
}

//static
void Memory::Allocate(MemoryPoolHandle pool, u32 size)
{
    if (pool.m_Index >= 0 && size)
    {
        g_MemoryPools[ pool.m_Index ].m_Size += size;
        g_MemoryPools[ pool.m_Index ].m_Count++;
    }
}

//static
void Memory::Deallocate(MemoryPoolHandle pool, u32 size)
{
    if (pool.m_Index >= 0 && size)
    {
        g_MemoryPools[ pool.m_Index ].m_Size -= size;
        g_MemoryPools[ pool.m_Index ].m_Count--;
    }
}
