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

        class Object;
        typedef Helium::StrongPtr<Object> ObjectPtr;
        typedef Helium::StrongPtr<const Object> ConstObjectPtr;

        class Data;
        typedef Helium::StrongPtr<Data> DataPtr;
        typedef Helium::StrongPtr<const Data> ConstDataPtr;

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
#define REFLECT_ARCHIVE_VERBOSE

//
// Internal Macros
//

// declares creator for constructable types
#define _REFLECT_DECLARE_CREATOR( __Class ) \
public: \
static Helium::Reflect::Object* CreateObject() { return new __Class; }

// declares type checking functions
#define _REFLECT_DECLARE_CLASS( __Class, __Base ) \
public: \
typedef __Base Base; \
typedef __Class This; \
virtual const Helium::Reflect::Class* GetClass() const HELIUM_OVERRIDE; \
static Helium::Reflect::Class* CreateClass( const tchar_t* name ); \
static const Helium::Reflect::Class* s_Class;

// defines the static type info vars
#define _REFLECT_DEFINE_CLASS( __Class, __Creator ) \
const Helium::Reflect::Class* __Class::GetClass() const \
{ \
    return s_Class; \
} \
\
Helium::Reflect::Class* __Class::CreateClass( const tchar_t* name ) \
{ \
    HELIUM_ASSERT( s_Class == NULL ); \
    HELIUM_ASSERT( __Class::Base::s_Class != NULL ); \
    Helium::Reflect::Class* type = Helium::Reflect::Class::Create<__Class>(name, __Class::Base::s_Class->m_Name, __Creator); \
    s_Class = type; \
    return type; \
} \
const Helium::Reflect::Class* __Class::s_Class = NULL;

// declares type checking functions
#define _REFLECT_DECLARE_ENUMERATION( __Enumeration ) \
public: \
Enum m_Value; \
__Enumeration() : m_Value() {} \
__Enumeration( const __Enumeration& e ) : m_Value( e.m_Value ) {} \
__Enumeration( const Enum& e ) : m_Value( e ) {} \
__Enumeration( intptr_t e ) : m_Value( (Enum)e ) {} \
operator intptr_t() const { return (size_t)m_Value; } \
static Helium::Reflect::Enumeration* CreateEnumeration( const tchar_t* name ); \
static const Helium::Reflect::Type* s_Type; \
static const Helium::Reflect::Enumeration* s_Enumeration;

// defines the static type info vars
#define _REFLECT_DEFINE_ENUMERATION( __Enumeration ) \
Helium::Reflect::Enumeration* __Enumeration::CreateEnumeration( const tchar_t* name ) \
{ \
    HELIUM_ASSERT( s_Enumeration == NULL ); \
    Reflect::Enumeration* type = Reflect::Enumeration::Create<__Enumeration>( name ); \
    s_Type = s_Enumeration = type; \
    return type; \
} \
const Helium::Reflect::Type* __Enumeration::s_Type = NULL; \
const Helium::Reflect::Enumeration* __Enumeration::s_Enumeration = NULL;

//
// User Macros
//

// declares an HELIUM_ABSTRACT object (an object that either A: cannot be instantiated or B: is never actually serialized)
#define REFLECT_DECLARE_ABSTRACT(__Class, __Base) \
    _REFLECT_DECLARE_CLASS(__Class, __Base)

// defines the HELIUM_ABSTRACT object class
#define REFLECT_DEFINE_ABSTRACT(__Class) \
    _REFLECT_DEFINE_CLASS(__Class, NULL)

// declares a full object with creator
#define REFLECT_DECLARE_CLASS(__Class, __Base) \
    _REFLECT_DECLARE_CLASS(__Class, __Base) \
    _REFLECT_DECLARE_CREATOR(__Class)

// defines a full object
#define REFLECT_DEFINE_CLASS(__Class) \
    _REFLECT_DEFINE_CLASS(__Class, &__Class::CreateObject)

// alias a type name
#define REFLECT_ALIAS_CLASS(__Class, __Alias) \
    Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass<__Class>(), __Alias);

// unalias a type name
#define REFLECT_UNALIAS_CLASS(__Class, __Alias) \
    Reflect::Registry::GetInstance()->UnaliasType( Reflect::GetClass<__Class>(), __Alias);

// declares an enumeration
#define REFLECT_DECLARE_ENUMERATION(__Class) \
    _REFLECT_DECLARE_ENUMERATION(__Class)

// defines an enumeration
#define REFLECT_DEFINE_ENUMERATION(__Class) \
    _REFLECT_DEFINE_ENUMERATION(__Class)

