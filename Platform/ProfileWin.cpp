#include "PlatformPch.h"
#include "Profile.h"

#include "Platform/Print.h"
#include "Platform/ProfileMemory.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <intrin.h>
#include <sstream>

using namespace Helium;
using namespace Helium::Profile;

void Helium::TraceFile::Open(const tchar_t* file)
{
    m_FileHandle = _tfopen(file, TXT("wb"));
}

void Helium::TraceFile::Close()
{
    if (m_FileHandle)
    {
        fclose(m_FileHandle);
    }
}

void Helium::TraceFile::Write(const tchar_t* data, int size)
{
    if (m_FileHandle)
    {
        fwrite(data, sizeof(tchar_t), size, m_FileHandle);
    }
}

const tchar_t* Helium::TraceFile::GetFilePath()
{
    tchar_t buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);

    tchar_t drive[ MAX_PATH ];
    tchar_t dir[ MAX_PATH ];
    tchar_t modulename[ MAX_PATH ];
    _tsplitpath( buf, drive, dir, modulename, NULL );

    static tchar_t file[ MAX_PATH ];
    _sntprintf_s( file, sizeof( file ), MAX_PATH, TXT("%s\\%s\\log\\profile_%s_%.5X_%.5X.bin"), drive, dir, modulename, GetCurrentProcessId(), GetCurrentThreadId() );

    return file;
}

uint64_t Helium::TimerGetClock()
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

float Helium::CyclesToMillis(uint64_t cycles)
{
    return (float)((1000.0 * (double)cycles) / GetClockSpeed());
}

float Helium::TimeTaken(uint64_t start_time)
{
    uint64_t time = TimerGetClock() - start_time;
    return CyclesToMillis(time);
}

void Helium::ReportTime(const tchar_t* segment, uint64_t start_time, double& total_millis)
{
    uint64_t time = TimerGetClock() - start_time;
    double millis = CyclesToMillis(time);
    Helium::Print(TXT("%s took %f ms\n"), segment, millis);
    total_millis += millis;
}

uint64_t Helium::GetTotalMemory()
{
    MemoryStatus status;
    GetMemoryStatus( &status );
    return status.m_TotalCommit;
}
