#include "FoundationPch.h"
#include "Memory.h"
#include "Profile.h"
#include "Log.h"

#include "Platform/Assert.h"
#include "Platform/Memory.h"
#include "Platform/Profile.h"
#include "Platform/ProfileMemory.h"
#include "Platform/Thread.h"

#include <stdlib.h>

#ifdef WIN32
#define snprintf _snprintf
#define abs64 _abs64
#else
#define abs64 labs
#endif

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
HELIUM_DEFINE_DEFAULT_MODULE_HEAP( Foundation );

#if HELIUM_SHARED
#include "Platform/NewDelete.h"
#endif

using namespace Helium;
using namespace Helium::Profile; 

static const uint32_t        g_MaxMemoryPools = 64;
static MemoryPool       g_MemoryPools[g_MaxMemoryPools];
static uint32_t              g_MemoryPoolCount = 0;
static Helium::CallbackThread g_MemoryReportThread;
static bool             g_MemoryReportThreadTerminate = false;
static bool             g_MemoryProfilingEnabled = false;

bool Profile::Settings::MemoryProfilingEnabled()
{
    return g_MemoryProfilingEnabled;
}

static const tchar_t* MemoryUnitConvert(float32_t& size)
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

static void MemoryReportThread(void*)
{
    uint32_t oldCount = g_MemoryPoolCount;
    float32_t oldTotal = (float32_t)Helium::GetTotalMemory();

    while (!g_MemoryReportThreadTerminate)
    {
        float32_t total = (float32_t)Helium::GetTotalMemory();

        // report if we have different memory usage or more pools
        bool report = oldTotal != total || oldCount != g_MemoryPoolCount;

        // check for changed data in each pool
        float32_t profiled = 0;
        for (uint32_t i=0; i<g_MemoryPoolCount; i++)
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
            const tchar_t* profiledUnits = MemoryUnitConvert(profiled);
            const tchar_t* totalUnits = MemoryUnitConvert(total);

            Log::Profile( TXT( "Memory - Profiled: %.2f%s / Committed: %.2f%s / Accounted for: %.2f%%\n" ), profiled, profiledUnits, total, totalUnits, accountedFor);

            for (uint32_t i=0; i<g_MemoryPoolCount; i++)
            {
                float32_t size = (float32_t)g_MemoryPools[i].m_Size;
                const tchar_t* sizeUnits = MemoryUnitConvert( size );

                float32_t delta = (float32_t)abs64(g_MemoryPools[i].m_Size - g_MemoryPools[i].m_Previous);
                const tchar_t* deltaUnits = MemoryUnitConvert( delta );

                tchar_t sign = ((int64_t)g_MemoryPools[i].m_Size - (int64_t)g_MemoryPools[i].m_Previous) >= 0 ? '+' : '-';

                Log::Profile( TXT( " %-30s: [%7d] %.2f%s (%c%.2f%s)\n" ), g_MemoryPools[i].m_Name, g_MemoryPools[i].m_Count, size, sizeUnits, sign, delta, deltaUnits );

                g_MemoryPools[i].m_Previous = g_MemoryPools[i].m_Size;
            }
        }

        SimpleTimer timer;

        while (timer.Elapsed() < 5000 && !g_MemoryReportThreadTerminate)
        {
            Thread::Sleep(10);
        }
    }
}

uint32_t Memory::s_InitCount = 0;

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

        g_MemoryReportThread.Join();
        g_MemoryProfilingEnabled = false;
    }
}

//static
MemoryPoolHandle Memory::CreatePool(const tchar_t* name)
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
void Memory::Allocate(MemoryPoolHandle pool, uint32_t size)
{
    if (pool.m_Index >= 0 && size)
    {
        g_MemoryPools[ pool.m_Index ].m_Size += size;
        g_MemoryPools[ pool.m_Index ].m_Count++;
    }
}

//static
void Memory::Deallocate(MemoryPoolHandle pool, uint32_t size)
{
    if (pool.m_Index >= 0 && size)
    {
        g_MemoryPools[ pool.m_Index ].m_Size -= size;
        g_MemoryPools[ pool.m_Index ].m_Count--;
    }
}
