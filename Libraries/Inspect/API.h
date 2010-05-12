#pragma once

#include "InspectTypes.h"

#include "Common/Assert.h"

#ifdef INSPECT_DLL
# ifdef INSPECT_EXPORTS
#  define INSPECT_API __declspec(dllexport)
# else
#  define INSPECT_API __declspec(dllimport)
# endif
#else
# define INSPECT_API
#endif

#define INSPECT_BASE(__Type)                                  \
  public:                                                     \
    virtual int GetType () const                              \
    {                                                         \
      return __Type;                                          \
    }                                                         \
                                                              \
    virtual bool HasType (int id) const                       \
    {                                                         \
      return __Type == id;                                    \
    }

#define INSPECT_TYPE(__Type)                                  \
  public:                                                     \
    virtual int GetType () const                              \
    {                                                         \
      return __Type;                                          \
    }                                                         \
                                                              \
    virtual bool HasType (int id) const                       \
    {                                                         \
      return __Type == id || __super::HasType(id);            \
    }

// prints console output of size and position info
//#define INSPECT_DEBUG_LAYOUT_LOGIC

// prints stepping of script code compilation
//#define INSPECT_DEBUG_SCRIPT_COMPILE

// prints tracks of Data instances
//#define INSPECT_DEBUG_DATA_TRACKING

// profiling for attribute systems
//#define INSPECT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(INSPECT_PROFILE)
# define INSPECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
# define INSPECT_SCOPE_TIMER(__Str)
#endif
