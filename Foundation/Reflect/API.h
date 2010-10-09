#pragma once

#include <vector>


//
// Platform Settings
//

// sanity check code generation settings
#include "Platform/Compiler.h"

// smartpointer system
#include "Foundation/Memory/SmartPtr.h"


//
// Profile Settings
//

// profile interface, where the global switch is
#include "Foundation/Profile.h"

// tracks profile data in reflect only
//#define REFLECT_PROFILE
//#define REFLECT_PROFILE_INSTANCE_API

#if defined(PROFILE_INSTRUMENT_ALL) || defined(REFLECT_PROFILE)
#define REFLECT_SCOPE_TIMER(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define REFLECT_SCOPE_TIMER(__Str)
#endif

#if defined(PROFILE_INSTRUMENT_ALL) || defined(REFLECT_PROFILE_INSTANCE_API)
#define REFLECT_SCOPE_TIMER_INST(__Str) PROFILE_SCOPE_TIMER(__Str)
#else
#define REFLECT_SCOPE_TIMER_INST(__Str)
#endif


//
// Top Level Forwards
//
namespace Helium
{
    namespace Reflect
    {
        class Object;
        typedef Helium::SmartPtr<Object> ObjectPtr;
        typedef std::vector<ObjectPtr> V_Object;
        typedef Helium::SmartPtr<const Object> ConstObjectPtr;
        typedef std::vector<ConstObjectPtr> V_ConstObject;

        class Element;
        typedef Helium::SmartPtr<Element> ElementPtr;
        typedef std::vector<ElementPtr> V_Element;
        typedef Helium::SmartPtr<const Element> ConstElementPtr;
        typedef std::vector<ConstElementPtr> V_ConstElement;

        class Serializer;
        typedef Helium::SmartPtr<Serializer> SerializerPtr;
        typedef std::vector<SerializerPtr> V_Serializer;
        typedef Helium::SmartPtr<const Serializer> ConstSerializerPtr;
        typedef std::vector<ConstSerializerPtr> V_ConstSerializer;

        // function type for creating object instances
        typedef Object* (*CreateObjectFunc)();

        // generic function for creating a new object, an instance of this fits into a CreateObjectFunc
        template< class T >
        Object* CreateObject()
        {
            return new T();
        }
    }
}

//
// Debugging Settings
//

// enable verbose archive printing
//#define REFLECT_ARCHIVE_VERBOSE

// track allocted instances in memory
//#define REFLECT_OBJECT_TRACKING

// verbose printing for tracked objects
//#define REFLECT_OBJECT_VERBOSE_TRACKING

// track the create/delete stacks of tracked objects
//#define REFLECT_OBJECT_STACK_TRACKING
