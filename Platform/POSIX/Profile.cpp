#include "Platform/Profile.h"
#include "Platform/Assert.h"

#include <pthread.h>
#include <assert.h>

void Helium::TraceFile::Open(const tchar* file)
{

}

void Helium::TraceFile::Close()
{

}

void Helium::TraceFile::Write(const tchar* data, int size)
{

}

const tchar* Helium::TraceFile::GetFilePath()
{
    return NULL;
}

u64 Helium::TimerGetClock()
{
    u64 time = 0;
    HELIUM_BREAK();
    return time;
}

float Helium::CyclesToMillis(u64 cycles)
{
    return (f64)cycles * (f64)(1000.0 / 79800000ULL);
}

float Helium::TimeTaken(u64 start_time)
{
    u64 time = TimerGetClock() - start_time;
    return CyclesToMillis(time);
}

void Helium::ReportTime(const tchar* segment, u64 start_time, double& total_millis)
{
    u64 time = TimerGetClock() - start_time;
    double millis = CyclesToMillis(time);
    printf("%s took %f ms\n", segment, millis);
    total_millis += millis;
}

u64 Helium::GetTotalMemory()
{
    return 256 * (1 << 20);
}
