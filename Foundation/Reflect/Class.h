#pragma once

#include "Platform/String.h"

#include "Foundation/Reflect/Composite.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Registry.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Class (struct or class)
        //

        class FOUNDATION_API Class : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Class );

            CreateObjectFunc        m_Creator;      // factory function for creating instances of this class
            StrongPtr<const Object> m_Default;      // the default instance of this class (constructor-initialized)
            StrongPtr<const Object> m_Template;     // the template for this class (by default, the default instance)

        protected:
            Class();
            virtual ~Class();

        public:
            // protect external allocation to keep inlined code in this dll
            static Class* Create();

            template< class ClassT >
            static Class* Create( const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator = NULL )
            {
                Class* info = Class::Create();

                // populate reflection information
                Composite::Create< ClassT >( name, baseName, &ClassT::AcceptCompositeVisitor, info );

                // setup factory function
                info->m_Creator = creator;

                // create the default instance
                if ( info->m_Creator )
                {
                    info->m_Default = info->m_Creator();
                    info->m_Template = info->m_Default;
                }

                return info;
            }

            // Overloaded functions from Composite
            virtual const void* GetDefaultInstance() const HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< Class > ClassPtr;
        typedef Helium::SmartPtr< const Class > ConstClassPtr;

        template<>
        Class* Class::Create< Object >( const tchar_t* name, const tchar_t* baseName, CreateObjectFunc creator );
    }
}
