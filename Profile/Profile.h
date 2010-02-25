#pragma once 

#include "API.h"
#include "Timer.h"
#include "Memory.h"


//
// Helpers
//

#define PROFILE_STRINGIFY(x) #x
#define PROFILE_TOSTRING(x) PROFILE_STRINGIFY(x)


//
// Library and Std. Types
//

#include "Common/Types.h"

#ifdef WIN32

struct _iobuf;
typedef struct _iobuf FILE;
# define FILE_HANDLE FILE*
# define INVALID_FILE_HANDLE NULL

#else

# define FILE_HANDLE int
# define INVALID_FILE_HANDLE -1

#endif


//
// Profile code API, for the most part almost all of this code always gets compiled in
// In general its beneficial to leave the accumulation API on all the time.
//

namespace Profile
{
  namespace Settings
  {
    PROFILE_API bool Enabled();
    PROFILE_API bool MemoryProfilingEnabled();
  }

  const static int MAX_DESCRIPTION = 256;

  struct Context; 

  PROFILE_API void Initialize(); 
  PROFILE_API void Cleanup(); 

  //
  // Accumulates information over multiple calls
  //

  class PROFILE_API Accumulator
  {
  public:
    u32   m_Hits; 
    float m_TotalMillis; 

    i32   m_Index;
    char  m_Name[MAX_DESCRIPTION];

    Accumulator(); 
    Accumulator(const char* name);
    Accumulator (const char* function, const char* name);
    ~Accumulator();

    void Init(const char* name);
    void Report();

    static void ReportAll();

  private: 
  };

  //
  // Scope timer prints or logs information
  //

  class PROFILE_API ScopeTimer
  {
  public: 
    ScopeTimer(Accumulator* accum, const char* func, u32 line, const char* desc = NULL); 
    ~ScopeTimer(); 

    char         m_Description[MAX_DESCRIPTION]; 
    u64          m_StartTicks; 
    Accumulator* m_Accum; 
    u32          m_UniqueID; 
    bool         m_Print; 

  private: 
    ScopeTimer(const ScopeTimer& rhs);  // no implementation

  }; 

}
//
// Platform implementation
//

namespace Platform
{
  class PROFILE_API LogFile
  {
  private:
    FILE_HANDLE m_FileHandle;

  public:
    LogFile()
      : m_FileHandle (INVALID_FILE_HANDLE)
    {

    }

    void Open(const char* file);

    void Close();

    void Write(const char* data, int size);
  };

  PROFILE_API u64 TimerGetClock();

  PROFILE_API float CyclesToMillis(u64 cycles);

  PROFILE_API float TimeTaken(u64 start_time);

  PROFILE_API void ReportTime(const char* segment, u64 start_time, double& total_millis);

  PROFILE_API const char* GetOutputFile();

  PROFILE_API u64 GetTotalMemory();

  PROFILE_API void PrintProfile(const char* fmt, ...);

  PROFILE_API void     ProfileInit(); 
  PROFILE_API void     ProfileCleanup(); 

  PROFILE_API Profile::Context* GetThreadContext(); 
  PROFILE_API void              SetThreadContext(Profile::Context* context); 
}

// profile flag check
#ifdef PROFILE_ENABLE
# error PROFILE_ENABLE flag used by another module
#endif

// master profile enable
#define PROFILE_ENABLE

//
// Accumulation API stashes time taken in each profile tag over the course of the entire profile interval
//

// profile flag check
#ifdef PROFILE_ACCUMULATION
# error PROFILE_ACCUMULATION flag used by another module
#endif

// accumulation api enable
#ifdef PROFILE_ENABLE
# define PROFILE_ACCUMULATION
#endif

#define PROFILE_ACCUMULATION


// accumulation macros
#ifdef PROFILE_ACCUMULATION
# define PROFILE_SCOPE_ACCUM(__Accum) \
Profile::ScopeTimer __ScopeAccum ( &__Accum, __FUNCTION__, __LINE__); 
# define PROFILE_SCOPE_ACCUM_VERBOSE(__Accum, __Str) \
Profile::ScopeTimer __ScopeAccum ( &__Accum, __FUNCTION__, __LINE__, Profile::Settings::Enabled() ? __Str : NULL );
#else
# define PROFILE_SCOPE_ACCUM(__Accum)
# define PROFILE_SCOPE_ACCUM_VERBOSE(__Accum, __Str)
#endif


//
// Instrumentation API pervades more code blocks and provides fine-grain profile data to a log file
//

// profile flag check
#ifdef PROFILE_INSTRUMENTATION
# error PROFILE_INSTRUMENTATION flag used by another module
#endif

// instrumentation api enable
#ifdef PROFILE_ENABLE
// uncomment this to enable instrumentation
//# define PROFILE_INSTRUMENTATION
#endif

// flag to instrument all code possible
#ifdef PROFILE_INSTRUMENTATION
// uncomment this to enable instrumentation everywhere
//# define PROFILE_INSTRUMENT_ALL
#endif

#if defined(PROFILE_INSTRUMENTATION) && defined(WIN32)
# pragma message (" Profile Instrumentation Enabled")
#endif

// instrumentation macros
#ifdef PROFILE_INSTRUMENTATION

# define PROFILE_FUNCTION_TIMER() \
  static Profile::Accumulator __Accumulator ( __FUNCTION__ ); \
  Profile::ScopeTimer __ScopeTimer ( &__Accumulator, NULL, __FUNCTION__, 0, false );

# define PROFILE_SCOPE_TIMER(__Description) \
  static Profile::Accumulator __Accumulator ( __FUNCTION__, ":" PROFILE_TOSTRING(__LINE__) ); \
  Profile::ScopeTimer __ScopeTimer ( &__Accumulator, __Description, __FUNCTION__, __LINE__, false );

#else

# define PROFILE_FUNCTION_TIMER()
# define PROFILE_SCOPE_TIMER(__Description)

#endif
