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
    class PLATFORM_API Timer
    {
    private:
        uint64_t m_StartTime;

    public:
        Timer()
        {
            Reset();
        }

    private:
        Timer(const Timer& rhs)
        {

        }

    public:
        // reset timer (for re-use)
        void Reset();

        // get elapsed time in millis
        float Elapsed();
    };

    class PLATFORM_API TraceFile
    {
    private:
        FILE_HANDLE m_FileHandle;

    public:
        TraceFile()
            : m_FileHandle (INVALID_FILE_HANDLE)
        {

        }

        void Open(const tchar_t* file);

        void Close();

        void Write(const tchar_t* data, int size);

        static const tchar_t* GetFilePath();
    };

    PLATFORM_API uint64_t TimerGetClock();
    PLATFORM_API float CyclesToMillis(uint64_t cycles);
    PLATFORM_API float TimeTaken(uint64_t start_time);
    PLATFORM_API void ReportTime(const tchar_t* segment, uint64_t start_time, double& total_millis);

    PLATFORM_API uint64_t GetTotalMemory();
}
