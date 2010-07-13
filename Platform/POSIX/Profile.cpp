#include "Platform/Profile.h"
#include "Platform/Assert.h"

#include <pthread.h>
#include <assert.h>

void Platform::TraceFile::Open(const tchar* file)
{

}

void Platform::TraceFile::Close()
{

}

void Platform::TraceFile::Write(const tchar* data, int size)
{

}

const tchar* Platform::TraceFile::GetFilePath()
{
    return NULL;
}

u64 Platform::TimerGetClock()
{
    u64 time = 0;
    NOC_BREAK();
    return time;
}

float Platform::CyclesToMillis(u64 cycles)
{
    return (f64)cycles * (f64)(1000.0 / 79800000ULL);
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
    printf("%s took %f ms\n", segment, millis);
    total_millis += millis;
}

u64 Platform::GetTotalMemory()
{
    return 256 * (1 << 20);
}
