#include "Platform/Profile.h"
#include "Platform/Platform.h"
#include "Platform/Windows/Windows.h"

#include "Memory.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <intrin.h>
#include <sstream>

using namespace Profile;

void Platform::TraceFile::Open(const tchar* file)
{
    m_FileHandle = _tfopen(file, TXT("wb"));
}

void Platform::TraceFile::Close()
{
    if (m_FileHandle)
    {
        fclose(m_FileHandle);
    }
}

void Platform::TraceFile::Write(const tchar* data, int size)
{
    if (m_FileHandle)
    {
        fwrite(data, sizeof(tchar), size, m_FileHandle);
    }
}

const tchar* Platform::TraceFile::GetFilePath()
{
    tchar buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);

    tchar modulename[MAX_PATH];
    _tsplitpath(buf, NULL, NULL, modulename, NULL);

    static tchar file[MAX_PATH];
    const tchar* root = _tgetenv( TXT("HELIUM_Root") );
    if ( root )
    {
        _stprintf(file, TXT("%s\\log\\profile_%s_%.5X_%.5X.bin"), root, modulename, GetCurrentProcessId(), GetCurrentThreadId() );
    }
    else
    {
        _stprintf( TXT("%s is not defined in the environment, cannot open profile output\n"), TXT("HELIUM_Root") );
    }

    return file;
}

u64 Platform::TimerGetClock()
{
    return __rdtsc();
}

inline double GetClockSpeed()
{
    static double clockSpeed = 0.0;
    if (clockSpeed != 0.0)
    {
        return clockSpeed;
    }

    __int64	i64_perf_start, i64_perf_freq, i64_perf_end;
    __int64	i64_clock_start,i64_clock_end;
    double d_loop_period, d_clock_freq;

    //-----------------------------------------------------------------------
    // Query the performance of the Windows high resolution timer.
    //-----------------------------------------------------------------------
    QueryPerformanceFrequency((LARGE_INTEGER*)&i64_perf_freq);

    //-----------------------------------------------------------------------
    // Query the current value of the Windows high resolution timer.
    //-----------------------------------------------------------------------
    QueryPerformanceCounter((LARGE_INTEGER*)&i64_perf_start);
    i64_perf_end = 0;

    //-----------------------------------------------------------------------
    // Time of loop of 250000 windows cycles with RDTSC
    //-----------------------------------------------------------------------
    i64_clock_start = __rdtsc();
    while(i64_perf_end<i64_perf_start+250000)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&i64_perf_end);
    }
    i64_clock_end = __rdtsc();

    //-----------------------------------------------------------------------
    // Caclulate the frequency of the RDTSC timer and therefore calculate
    // the frequency of the processor.
    //-----------------------------------------------------------------------
    i64_clock_end -= i64_clock_start;

    d_loop_period = ((double)(i64_perf_freq)) / 250000.0;
    d_clock_freq = ((double)(i64_clock_end & 0xffffffff))*d_loop_period;

    return (float)(clockSpeed = d_clock_freq);
}

float Platform::CyclesToMillis(u64 cycles)
{
    return (float)((1000.0 * (double)cycles) / GetClockSpeed());
}

float Platform::TimeTaken(u64 start_time)
{
    u64 time = TimerGetClock() - start_time;
    return CyclesToMillis(time);
}

void Platform::ReportTime(const tchar* segment, u64 start_time, double& total_millis)
{
    u64 time = TimerGetClock() - start_time;
    double millis = CyclesToMillis(time);
    Platform::Print(TXT("%s took %f ms\n"), segment, millis);
    total_millis += millis;
}

u64 Platform::GetTotalMemory()
{
    MemoryStatus status;
    GetMemoryStatus( &status );
    return status.m_TotalCommit;
}
