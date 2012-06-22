#pragma once

#include "Platform/Platform.h"

#include "Foundation/Profile.h"
#include "Foundation/Memory/ReferenceCounting.h"

#if HELIUM_SHARED
# ifdef HELIUM_REFLECT_EXPORTS
#  define HELIUM_REFLECT_API HELIUM_API_EXPORT
# else
#  define HELIUM_REFLECT_API HELIUM_API_IMPORT
# endif
#else
#define HELIUM_REFLECT_API
#endif

//#define REFLECT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(REFLECT_PROFILE)
#define REFLECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define REFLECT_SCOPE_TIMER(__Str)
#endif

#if !HELIUM_RELEASE
# define REFLECT_CHECK_MEMORY 1
# define REFLECT_CHECK_MEMORY_ASSERT HELIUM_ASSERT
#else
# define REFLECT_CHECK_MEMORY_ASSERT( EXPR )
#endif

namespace Helium
{
    namespace Reflect
    {
        class Type;
        class Composite;
        class Structure;
        class Class;
        class Enumeration;
        class Visitor;

        class Object;
        typedef Helium::StrongPtr<Object> ObjectPtr;
        typedef Helium::StrongPtr<const Object> ConstObjectPtr;

        class Data;
        typedef Helium::StrongPtr<Data> DataPtr;
    }
}
