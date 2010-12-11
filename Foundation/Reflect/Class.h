#pragma once

#include "Foundation/Reflect/Composite.h"
#include "Foundation/Reflect/Registry.h"

#include "Platform/String.h"

namespace Helium
{
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

            CreateObjectFunc      m_Creator;             // factory function for creating instances of this class

        protected:
            Class();
            virtual ~Class();

        public:
            // protect external allocation to keep inlined code in this dll
            static Class* Create();

            template<class T>
            static Class* Create( Name name, Name baseName, CreateObjectFunc creator = NULL )
            {
                Class* info = Class::Create();

                info->m_Size = sizeof(T);
                info->m_Name = name;
                info->m_Creator = creator;
                info->m_UIName = *info->m_Name;

                // lookup base class
                info->m_Base = Reflect::Registry::GetInstance()->GetClass( baseName );

                // if you hit this break your base class is not registered yet!
                HELIUM_ASSERT( info->m_Base );

                // populate base classes' derived class list (unregister will remove it)
                info->m_Base->m_Derived.Insert( info );

                // the static enumerate function for this particular class' fields
                CompositeEnumerator enumerator = (CompositeEnumerator)&T::EnumerateClass;

                // c++ can give us the address of base class static functions,
                //  so check each base class to see if this is really a base class enumerate function
                bool baseEnumerator = false;
                {
                    const Reflect::Composite* base = info->m_Base;
                    while ( !baseEnumerator && base )
                    {
                        if (base)
                        {
                            baseEnumerator = base->m_Enumerator && base->m_Enumerator == enumerator;
                            base = base->m_Base;
                        }
                        else
                        {
                            HELIUM_BREAK(); // if you hit this break your base class is not registered yet!
                            baseName.Clear();
                        }
                    }
                }

                // if our enumerate function isn't one from a base class
                if ( !baseEnumerator )
                {
                    // the enumerator function will populate our field data
                    info->m_Enumerator = enumerator;
                }

                // enumerate reflection data, but only if we are concrete (instantiatable)
                if (info->m_Creator)
                {
                    // create the default object
                    Helium::StrongPtr<T> temp( (T*)(info->m_Creator()) );

                    // enumerate the fields in the class
                    info->EnumerateInstance<T>(*temp);
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

        typedef Helium::SmartPtr< Class > ClassPtr;
        typedef Helium::SmartPtr< const Class > ConstClassPtr;

        template<>
        Class* Class::Create< Object >( Name name, Name baseName, CreateObjectFunc creator );
    }
}
