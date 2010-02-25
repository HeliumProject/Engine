#include "profile.h"
#include "Common/Config.h"

#include <cell/cell_fs.h>
#include <cell/fs/cell_fs_errno.h>
#include <cell/fs/cell_fs_file_api.h>
#include <sys/time_util.h>
#include <pthread.h>
#include <assert.h>

using namespace Profile;

void Platform::LogFile::Open(const char* file)
{
  cellFsOpen(Platform::GetOutputFile(), CELL_FS_O_RDWR, &m_FileHandle, NULL, 0);
}

void Platform::LogFile::Close()
{
  cellFsClose(m_FileHandle);
}

void Platform::LogFile::Write(const char* data, int size)
{
  cellFsWrite(m_FileHandle, data, size, NULL);
}

u64 Platform::TimerGetClock()
{
  u64 time;
  SYS_TIMEBASE_GET(time);
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

void Platform::ReportTime(const char* segment, u64 start_time, double& total_millis)
{
  u64 time = TimerGetClock() - start_time;
  double millis = CyclesToMillis(time);
  printf("%s took %f ms\n", segment, millis);
  total_millis += millis;
}

const char* Platform::GetOutputFile()
{
  static char file[80];
  sprintf( file, "/app_home/%s/temp/Profile_PS3.bin", NOCTURNAL_PROJECT_NAME );
  return file;
}

u64 Platform::GetTotalMemory()
{
  return 256 * (1 << 20);
}

void Platform::PrintProfile(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

//----------------------------------------------------------
// wrappers for thread specific data functions
// 
pthread_key_t g_TLS_Key; 

void Platform::ProfileInit()
{
  int status = pthread_key_create(&g_TLS_Key, NULL); 
  assert( status == 0 && "Profile library could not create pthread_key"); 

  SetThreadContext(NULL); 
}

void Platform::ProfileCleanup()
{
  pthread_key_delete(g_TLS_Key); 
}

Profile::Context* Platform::GetThreadContext()
{
  return (Profile::Context*) pthread_getspecific(g_TLS_Key); 
}

void  Platform::SetThreadContext(Profile::Context* context)
{
  pthread_setspecific(g_TLS_Key, context); 
}

