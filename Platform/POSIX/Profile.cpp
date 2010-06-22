#include "Platform/Profile.h"

#include <sys/time_util.h>
#include <pthread.h>
#include <assert.h>

using namespace Profile;

void Platform::TraceFile::Open(const char* file)
{

}

void Platform::TraceFile::Close()
{

}

void Platform::TraceFile::Write(const char* data, int size)
{

}

const char* Platform::TraceFile::GetFilePath()
{
    return NULL;
}

u64 Platform::TimerGetClock()
{
    u64 time;
    SYS_TIMEBASE_GET(time);
    return time;
}

float Platform::CyclesToMillis(u64 cycles)
{
    NOC_COMPILE_ASSERT( false );
    return (f64)cycles * (f64)(1000.0 / 79800000ULL);
}

float Platform::TimeTaken(u64 start_time)
{
    u64 time = TimerGetClock() - start_time;
    return CyclesToMillis(time);
}

void Platform::ReportTime(const char* segment, u64 start_time, double& total_millis)
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
