#pragma once

#include "Platform/String.h"

#include "Foundation/Reflect/Composite.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Reflect
    {
        // function type for creating object instances
        typedef Object* (*CreateObjectFunc)();

        //
        // Class (struct or class)
        //

        class HELIUM_FOUNDATION_API Class : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Class, Class, Composite );

        protected:
            Class();
            ~Class();

        public:
            // protect external allocation to keep inlined code in this dll
            static Class* Create();

            template< class ClassT >
            static void Create( Class const*& pointer, const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator = NULL )
            {
                Class* type = Class::Create();
                pointer = type;

                // populate reflection information
                Composite::Create< ClassT >( name, baseName, &ClassT::PopulateComposite, type );

                // setup factory function
                type->m_Creator = creator;

                // fetch a potential default instance from the composite
                ClassT* instance = static_cast< ClassT* >( type->Composite::m_Default );
                if ( instance )
                {
                    type->m_Default = instance;
                }
                else
                {
                    // create the default instance
                    if ( pointer->m_Creator )
                    {
                        type->Composite::m_Default = type->m_Default = type->m_Creator();
                    }
                }
            }

        public:
            CreateObjectFunc        m_Creator;  // factory function for creating instances of this class
            StrongPtr< Object >     m_Default;  // the template for this class (by default, the default instance)
        };

        typedef Helium::SmartPtr< Class > ClassPtr;
        typedef Helium::SmartPtr< const Class > ConstClassPtr;

        // Object, the most base class needs explicit implementation
        template<>
        void Class::Create< Object >( Class const*& pointer, const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator );

        template< class ClassT, class BaseT >
        class ClassRegistrar : public TypeRegistrar
        {
        public:
            ClassRegistrar(const tchar_t* name);
            ~ClassRegistrar();

            virtual void Register();
            virtual void Unregister();
        };

        template< class ClassT >
        class ClassRegistrar< ClassT, void > : public TypeRegistrar
        {
        public:
            ClassRegistrar(const tchar_t* name);
            ~ClassRegistrar();

            virtual void Register();
            virtual void Unregister();
        };
    }
}

#include "Foundation/Reflect/Class.inl"
