#include "profile.h"

#include "Memory.h"

#include "Common/Config.h"
#include "Windows/Windows.h"
#include "Console/Console.h"

#include <sstream>

#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <intrin.h>

using namespace Profile;

void Platform::LogFile::Open(const char* file)
{
  m_FileHandle = fopen(file, "wb");
}

void Platform::LogFile::Close()
{
  if (m_FileHandle)
  {
    fclose(m_FileHandle);
  }
}

void Platform::LogFile::Write(const char* data, int size)
{
  if (m_FileHandle)
  {
    fwrite(data, sizeof(char), size, m_FileHandle);
  }
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

void Platform::ReportTime(const char* segment, u64 start_time, double& total_millis)
{
  u64 time = TimerGetClock() - start_time;
  double millis = CyclesToMillis(time);
  PrintProfile("%s took %f ms\n", segment, millis);
  total_millis += millis;
}

const char* Platform::GetOutputFile()
{
  char buf[MAX_PATH];
  GetModuleFileName(NULL, buf, MAX_PATH);

  char modulename[MAX_PATH];
  _splitpath(buf, NULL, NULL, modulename, NULL);

  static char file[MAX_PATH];
  const char* root = getenv( NOCTURNAL_STUDIO_PREFIX "LOG_PATH" );
  if ( root )
  {
    sprintf(file, "%s\\%s\\log\\profile_%s_%.5X_%.5X.bin", root, NOCTURNAL_PROJECT_NAME, modulename, 
                                                           GetCurrentProcessId(), GetCurrentThreadId() );
  }
  else
  {
    printf( "%s is not defined in the environment, cannot open profile output\n", NOCTURNAL_STUDIO_PREFIX "Root" );
  }

  return file;
}

u64 Platform::GetTotalMemory()
{
  MemoryStatus status;
  GetMemoryStatus( &status );
  return status.m_TotalCommit;
}

void Platform::PrintProfile(const char* fmt, ...)
{
  static char buff[Console::MAX_PRINT_SIZE];

  va_list args;
  va_start(args, fmt);
  _vsnprintf(buff, sizeof(buff), fmt, args);
  buff[ sizeof(buff) - 1] = 0; 
  va_end(args);

  Console::PrintString(buff, Console::Streams::Profile, Console::Levels::Default, Console::GetStreamColor( Console::Streams::Profile ), 0);
}

//----------------------------------------------------------
// this thread local storage stuff should probably be wrapped 
// in the real platform library. that would be nicer. 
//

DWORD g_TLS_Key = 0; 

void Platform::ProfileInit()
{
  g_TLS_Key = TlsAlloc(); 
  NOC_ASSERT(g_TLS_Key != TLS_OUT_OF_INDEXES); 
}

void Platform::ProfileCleanup()
{
  TlsFree(g_TLS_Key); 
}

Profile::Context* Platform::GetThreadContext()
{
  return (Profile::Context*) TlsGetValue(g_TLS_Key); 

}

void  Platform::SetThreadContext(Profile::Context* context)
{
  TlsSetValue(g_TLS_Key, context); 
}

