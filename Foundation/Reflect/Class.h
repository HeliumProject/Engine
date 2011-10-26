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
            static Class* Create( const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator = NULL )
            {
                Class* info = Class::Create();

                // populate reflection information
                Composite::Create< ClassT >( name, baseName, &ClassT::PopulateComposite, info );

                // setup factory function
                info->m_Creator = creator;

                // create the default instance
                if ( info->m_Creator )
                {
                    info->Composite::m_Default = info->m_Default = info->m_Creator();
                }

                return info;
            }

        public:
            CreateObjectFunc        m_Creator;  // factory function for creating instances of this class
            StrongPtr< Object >     m_Default;  // the template for this class (by default, the default instance)
        };

        typedef Helium::SmartPtr< Class > ClassPtr;
        typedef Helium::SmartPtr< const Class > ConstClassPtr;

        // Object, the most base class needs explicit implementation
        template<>
        Class* Class::Create< Object >( const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator );

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
