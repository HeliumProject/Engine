#pragma once

// Std Libraries
#include <string>

// DLL Export API
#include "Editor/API.h"

// Persistent data member and serialization
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Serializers.h"

//
// These macros define our RTTI data for our application classes
//  We use RTTI:
//   o To get basic information about a type (its name from an instance)
//   o Do type-safe downcasting (w/o dynamic_cast, so we can profile)
//   o Aid in making user-editable attributes
//   o Help with serialization
//

#define EDITOR_DECLARE_TYPE(__Class, __Base)                                \
    REFLECT_DECLARE_ABSTRACT(__Class, __Base)                               \
    static void __Class::EnumerateClass( Reflect::Compositor<__Class>& comp )      \
  {                                                                       \
  }

#define EDITOR_DEFINE_TYPE(__Class)           \
    REFLECT_DEFINE_ABSTRACT(__Class)

#define EDITOR_REGISTER_TYPE(__Class)         \
    REFLECT_REGISTER_ABSTRACT(__Class)

namespace Helium
{
    namespace Editor
    {
        //
        // Object is a basic application object
        //  Its allocated as a reference-counted heap object that is tracked by a smart (shared) pointer
        //

        class EDITOR_CORE_API Object : public Reflect::Object
        {
            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Object, Reflect::Object );
            static void InitializeType();
            static void CleanupType();


            //
            // Constructor
            //

        public:
            Object();
            virtual ~Object();
        };

        typedef Helium::SmartPtr< Object > ObjectPtr;
    }
}