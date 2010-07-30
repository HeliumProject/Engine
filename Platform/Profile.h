#pragma once 

#include "API.h"
#include "Memory.h"
#include "Types.h"

#ifdef WIN32

struct _iobuf;
typedef struct _iobuf FILE;
#define FILE_HANDLE FILE*
#define INVALID_FILE_HANDLE NULL

#else

#define FILE_HANDLE int
#define INVALID_FILE_HANDLE -1

#endif

namespace Helium
{
    class PLATFORM_API TraceFile
    {
    private:
        FILE_HANDLE m_FileHandle;

    public:
        TraceFile()
            : m_FileHandle (INVALID_FILE_HANDLE)
        {

        }

        void Open(const tchar* file);

        void Close();

        void Write(const tchar* data, int size);

        static const tchar* GetFilePath();
    };

    PLATFORM_API u64 TimerGetClock();
    PLATFORM_API float CyclesToMillis(u64 cycles);
    PLATFORM_API float TimeTaken(u64 start_time);
    PLATFORM_API void ReportTime(const tchar* segment, u64 start_time, double& total_millis);

    PLATFORM_API u64 GetTotalMemory();
}
