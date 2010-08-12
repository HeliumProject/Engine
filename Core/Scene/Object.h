#pragma once

// Std Libraries
#include <string>

// DLL Export API
#include "Core/API.h"

// Persistent data member and serialization
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Serializers.h"

#define SCENE_DECLARE_TYPE(__Class, __Base) \
REFLECT_DECLARE_ABSTRACT(__Class, __Base) \
static void __Class::EnumerateClass( Reflect::Compositor<__Class>& comp ) {}

#define SCENE_DEFINE_TYPE(__Class) \
REFLECT_DEFINE_ABSTRACT(__Class)

namespace Helium
{
    namespace Core
    {
        //
        // Object is a basic application object
        //  Its allocated as a reference-counted heap object that is tracked by a smart (shared) pointer
        //

        class CORE_API Object : public Reflect::Object
        {
            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Object, Reflect::Object );
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