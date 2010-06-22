#pragma once

#include "Composite.h"

namespace Reflect
{
    class Field;
    class Class;


    //
    // Class (struct or class)
    //

    class FOUNDATION_API Class : public Composite
    {
    public:
        REFLECTION_TYPE( ReflectionTypes::Class );

        CreateObjectFunc      m_Create;             // factory function for creating instances of this class

    protected:
        Class();
        virtual ~Class();

    public:
        // protect external allocation to keep inlined code in this dll
        static Class* Create();

        template<class T>
        static Class* Create(const std::string& base = "", const std::string& shortName = "", CreateObjectFunc creator = NULL)
        {
            Class* info = Class::Create();

            info->m_TypeID = AssignTypeID();
            info->m_Size = sizeof(T);

            info->m_ShortName = ( shortName.empty() ? ShortenName(typeid(T).name()) : shortName );
            info->m_FullName = typeid(T).name();
            info->m_UIName = info->m_ShortName;

            info->m_Base = base;
            info->m_Create = creator;

            // c++ can give us the address of base class static functions, so check each base class
            bool baseEnumerator = false;
            std::string baseName = info->m_Base;
            while ( !baseEnumerator && !baseName.empty() )
            {
                const Reflect::Composite* base = Reflect::Registry::GetInstance()->GetClass( baseName );
                if (base)
                {
                    baseEnumerator = base->m_Enumerator && base->m_Enumerator == (CompositeEnumerator)&T::EnumerateClass;
                    baseName = base->m_Base;
                }
                else
                {
                    NOC_BREAK(); // if you hit this break your base class is not registered yet!
                    baseName.clear();
                }
            }

            if (!baseEnumerator)
            {
                info->m_Enumerator = (CompositeEnumerator)&T::EnumerateClass;
            }

            // enumerate reflection data, but only if we are concrete (instantiatable)
            if (info->m_Create)
            {
                // create the default object
                Nocturnal::SmartPtr<T> temp = (T*)(info->m_Create());

                // enumerate the fields in the class
                info->EnumerateInstance<T>(*temp);
            }

            if (info->m_FirstFieldID == info->m_LastFieldID)
            {
                info->m_FirstFieldID = info->m_LastFieldID = -1;
            }

            return info;
        }

        //
        // Clone makes a deep copy of the passed object, you can use FieldFlags::Share to flag fields
        //  to not be used when doing the traversal for deep copy cloning.  Just the reference will be copied
        //  in such cases.  Currently we don't provide support for containers of non-cloneable fields.
        //

        static ElementPtr Clone(Element* element);
    };

    typedef Nocturnal::SmartPtr< Class > ClassPtr;
    typedef Nocturnal::SmartPtr< const Class > ConstClassPtr;

    //
    // AbstractInheritor injects virtual API for all reflection types
    //

    template<class D, class B>
    class AbstractInheritor : public B
    {
    public:
        virtual i32 GetType() const NOC_OVERRIDE
        {
            // this function caches a static in our translation unit
            return Reflect::GetType<D>();
        }

        virtual bool HasType(i32 id) const NOC_OVERRIDE
        {
            // this function caches a static in our translation unit
            return id == Reflect::GetType<D>() || B::HasType(id);
        }

        virtual const Reflect::Class* GetClass() const NOC_OVERRIDE
        {
            // this function caches a static in our translation unit
            return Reflect::GetClass<D>();
        }

        static Reflect::Class* CreateClass(const std::string& shortName = "")
        {
            return Reflect::Class::Create<D>(typeid(B).name(), shortName);
        }
    };

    //
    // ConcreteInheritor adds creation factory support to AbstractInheritor
    //

    template<class D, class B>
    class ConcreteInheritor : public AbstractInheritor<D, B>
    {
    public:
        static Reflect::Object* CreateObject()
        {
            return new D;
        }

        static Reflect::Class* CreateClass(const std::string& shortName = "")
        {
            return Reflect::Class::Create<D>(typeid(B).name(), shortName, &CreateObject);
        }
    };
}


//
// Internal Macros
//

// declares creator for constructable types
#define _REFLECT_DECLARE_CREATOR( __Class )                                                                           \
  public:                                                                                                             \
  static Reflect::Object* CreateObject()                                                                            \
{                                                                                                                 \
    return new __Class;                                                                                             \
}

// declares type checking functions
#define _REFLECT_DECLARE_CLASS( __Class, __Base, __Creator )                                                          \
  public:                                                                                                             \
  virtual i32 GetType() const NOC_OVERRIDE                                                                          \
{                                                                                                                 \
    return Reflect::GetType<__Class>();                                                                             \
}                                                                                                                 \
    \
    virtual bool HasType(i32 id) const NOC_OVERRIDE                                                                   \
{                                                                                                                 \
    return Reflect::GetType<__Class>() == id || __Base::HasType(id);                                                \
}                                                                                                                 \
    \
    virtual const Reflect::Class* GetClass() const NOC_OVERRIDE                                                       \
{                                                                                                                 \
    return Reflect::GetClass<__Class>();                                                                            \
}                                                                                                                 \
    \
    static Reflect::Class* CreateClass(const std::string& shortName = "")                                             \
{                                                                                                                 \
    return Reflect::Class::Create<__Class>(typeid(__Base).name(), shortName, __Creator);                            \
}

// defines the static type info vars
#define _REFLECT_DEFINE_CLASS(__Class)


//
// User Macros
//

// declares an NOC_ABSTRACT element (an element that either A: cannot be instantiated or B: is never actually serialized)
#define REFLECT_DECLARE_ABSTRACT(__Class, __Base)                                                                     \
    _REFLECT_DECLARE_CLASS(__Class, __Base, NULL)

// defines the NOC_ABSTRACT element class
#define REFLECT_DEFINE_ABSTRACT(__Class)                                                                              \
    _REFLECT_DEFINE_CLASS(__Class)

// declares a full element with creator
#define REFLECT_DECLARE_CLASS(__Class, __Base)                                                                        \
    _REFLECT_DECLARE_CLASS(__Class, __Base, &__Class::CreateObject)                                                     \
    _REFLECT_DECLARE_CREATOR(__Class)

// defines a full element
#define REFLECT_DEFINE_CLASS(__Class)                                                                                 \
    _REFLECT_DEFINE_CLASS(__Class)

// alias a type name
#define REFLECT_ALIAS_CLASS(__Class, __Alias)                                                                         \
    Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass<__Class>(), __Alias);

// unalias a type name
#define REFLECT_UNALIAS_CLASS(__Class, __Alias)                                                                       \
    Reflect::Registry::GetInstance()->UnAliasType( Reflect::GetClass<__Class>(), __Alias);
