#pragma once

#include "Platform/Platform.h"

#include "Foundation/API.h"
#include "Foundation/Profile.h"
#include "Foundation/Memory/ReferenceCounting.h"

//#define REFLECT_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(REFLECT_PROFILE)
#define REFLECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define REFLECT_SCOPE_TIMER(__Str)
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
        typedef Helium::StrongPtr<const Data> ConstDataPtr;
    }
}
